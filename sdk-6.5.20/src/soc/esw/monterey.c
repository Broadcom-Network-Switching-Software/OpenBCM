/*
  $Id: monterey.c $
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        monterey.c
 * Purpose:
 * Requires:
 */

#include <shared/bsl.h>

#include <sal/core/boot.h>
#include <sal/core/dpc.h>
#include <soc/bradley.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/debug.h>
#include <soc/mem.h>
#include <soc/monterey.h>
#include <soc/apache.h>
#include <soc/i2c.h>
#include <soc/mspi.h>
#include <soc/l2x.h>
#include <soc/soc_ser_log.h>
#include <soc/trident2.h>
#include <soc/tdm/core/tdm_top.h>
#include <soc/mcm/memregs.h>
#ifdef BCM_MONTEREY_SUPPORT

#ifdef ALPM_ENABLE
#include <soc/lpm.h>
#include <soc/alpm.h>
#include <soc/esw/alpm_int.h>
#endif /* ALPM_ENABLE */

#include <soc/mmu_config.h>
#include <soc/post.h>

#include <soc/esw/portctrl.h>
#include <soc/bondoptions.h>

#ifdef PORTMOD_SUPPORT
#include <soc/portmod/portmod.h>
#endif /*  PORTMOD_SUPPORT */
#ifdef CPRIMOD_SUPPORT
#include <soc/cprimod/cprimod.h>
#include <shared/cpri.h>
#endif /* CPRIMOD_SUPPORT */
#ifdef INCLUDE_XFLOW_MACSEC
#include <bcm_int/common/xflow_macsec_cmn.h>
#endif

#include <soc/iproc.h>
#include <bcm/pkt.h>
#include <bcm/tx.h>

#define _MONTEREY_BYTES_PER_OBM_CELL    16
#define _MONTEREY_CELLS_PER_OBM         1020

#define _MONTEREY_LOG_PORTS_PER_PIPE    67
#define _MONTEREY_MMU_PORTS_PER_PIPE    67
#define _SOC_MONTEREY_MAX_PORT          (_MONTEREY_LOG_PORTS_PER_PIPE)

#define _MONTEREY_MMU_PACKET_HEADER_BYTES 64    /* bytes */
#define _MONTEREY_MMU_JUMBO_FRAME_BYTES   9216  /* bytes */
#define _MONTEREY_MMU_DEFAULT_MTU_BYTES   1536  /* bytes */

#define _MONTEREY_MMU_BYTES_PER_CELL      208   /* bytes (1664 bits) */
#define _MONTEREY_MMU_NUM_PG              8
#define _MONTEREY_MMU_GLOBAL_HEADROOM_CELL_PER_PIPE 50
#define _MONTEREY_MMU_PG_MIN              8

#define SOC_MONTEREY_MMU_CFG_QGROUP_MAX   128

#define SOC_MONTEREY_MMU_MCQ_ENTRY        12288
#define SOC_MONTEREY_MMU_RQE_ENTRY        1024

#define _MONTEREY_MMU_TOTAL_MCQ_ENTRY(unit)  SOC_MONTEREY_MMU_MCQ_ENTRY

#define _MONTEREY_MMU_TOTAL_RQE_ENTRY(unit)  SOC_MONTEREY_MMU_RQE_ENTRY

#define _SOC_MONTEREY_DEFIP_MAX_TCAMS   8
#define _SOC_MONTEREY_DEFIP_TCAM_DEPTH  64

#define IS_OVERSUB_PORT(unit,port)           \
        (SOC_PBMP_MEMBER(PBMP_OVERSUB(unit), port))

#define INVALID_PORT  -1

/* defines for Reserved Buffer for Flex Port Macro when new Flex Port is
   enabled */
/* lossless defines */
#define _MN_RSVD_BUF_10G_FLEX_HDRM_LOSSLESS 185
#define _MN_RSVD_BUF_25G_FLEX_HDRM_LOSSLESS  214

#define _MN_RSVD_BUF_FLEX_QGRP 16
#define _MN_RSVD_BUF_10G_FLEX_ASF 4
#define _MN_RSVD_BUF_25G_FLEX_ASF 16

/* defines for Reserved Buffer for Ports when Flex Port for the macro is NOT
   enabled*/
#define _MN_RSVD_BUF_NON_FLEX_OB_100G_HDRM_LOSSLESS 687
#define _MN_RSVD_BUF_NON_FLEX_100G_HDRM_LOSSLESS  558
#define _MN_RSVD_BUF_NON_FLEX_OB_50G_HDRM_LOSSLESS  352
#define _MN_RSVD_BUF_NON_FLEX_50G_HDRM_LOSSLESS   298
#define _MN_RSVD_BUF_NON_FLEX_OB_40G_HDRM_LOSSLESS  338
#define _MN_RSVD_BUF_NON_FLEX_40G_HDRM_LOSSLESS  284
#define _MN_RSVD_BUF_NON_FLEX_OB_25G_HDRM_LOSSLESS  214
#define _MN_RSVD_BUF_NON_FLEX_25G_HDRM_LOSSLESS  191
#define _MN_RSVD_BUF_NON_FLEX_OB_20G_HDRM_LOSSLESS  206
#define _MN_RSVD_BUF_NON_FLEX_20G_HDRM_LOSSLESS  183
#define _MN_RSVD_BUF_NON_FLEX_OB_10G_HDRM_LOSSLESS  185
#define _MN_RSVD_BUF_NON_FLEX_10G_HDRM_LOSSLESS  162
#define _MN_MMU_RSVD_BUF_FLEX_PGMIN_LOSSLESS 8
#define _MN_RSVD_BUF_NON_FLEX_PGMIN 8
#define _MN_RSVD_BUF_NON_FLEX_QGRP 16
#define _MN_RSVD_BUF_NON_FLEX_ASF 16
#define _MN_RSVD_BUF_NON_FLEX_10G_ASF 4
/* mmu_mc_egr_qentry_min in Settings sheet */
#define _MN_MMU_PER_COSQ_EGRESS_QENTRY_RSVD 6
#define _MN_RSVD_BUF_10G_FLEX_QGRP 16
#define _MN_MMU_NUM_MCQ_PORT_MACRO 10
#define _MN_MMU_RDB_PORT_MIN 90

#define _MN_MMU_EXPRESS_BUFFER_TOTAL_1 10200

/*
 * TDM algo does not support all speeds. SDK needs to adjust
 * the speeds to  what is supported in the algorithm.
 *
 * Anything < 1G will be passed as 1G.
 * 5G needs to be passed as 10G.
 * 2.5G is supported in the TDM. We can pass as is.
 */
#define MONTEREY_TDM_SPEED_ADJUST(unit, port, speed)          \
    do {                                                    \
        speed = SOC_PORTCTRL_HG2_TO_IEEE_BW_GET(speed);     \
        if ((speed > 0) && (speed < 10000)) {               \
            if (IS_OVERSUB_PORT(unit, port)) {              \
                speed = 10000;                              \
            } else if (speed < 1000) {                      \
                speed = 1000;                               \
            } else if (speed == 5000) {                     \
                speed = 10000;                              \
            }                                               \
        }                                                   \
    } while (0)
int soc_monterey_is_any_port_flex_enable(int unit);
int soc_mn_port_macro_flex_enabled(int unit, int port_macro, int * flex_enabled);
int
bcmi_mn_preemption_capability_get(int unit, bcm_port_t port,
                                    uint32* arg);

typedef enum {
    _SOC_PARITY_TYPE_NONE,
    _SOC_PARITY_TYPE_GENERIC,
    _SOC_PARITY_TYPE_PARITY,
    _SOC_PARITY_TYPE_ECC,
    _SOC_PARITY_TYPE_MMU_SER,
    _SOC_PARITY_TYPE_START_ERR,
    _SOC_PARITY_TYPE_SER,
    _SOC_PARITY_TYPE_BST,
    _SOC_PARITY_TYPE_MAC_RX_CDC,
    _SOC_PARITY_TYPE_MAC_TX_CDC,
    _SOC_PARITY_TYPE_MAC_RX_TS,
    _SOC_PARITY_TYPE_MIB,
    _SOC_PARITY_TYPE_OBM
#ifdef INCLUDE_XFLOW_MACSEC
    ,_SOC_PARITY_TYPE_MACSEC_SER,
    _SOC_PARITY_TYPE_MACSEC_FUNCTIONAL
#endif
} _soc_monterey_ser_info_type_t;

typedef struct _soc_monterey_fifo_ser_info_s {
    soc_reg_t   enable_reg;
    soc_field_t enable_field;
    uint8       type; /* 0: mem, 1: reg, 2: bus */
    soc_mem_t   mem;
    soc_reg_t   reg;
    char        *name_str; /* Used when mem == INVALIDm or reg == INVALIDr or type == bus
                              (either not accessible or multiple) */
} _soc_monterey_fifo_ser_info_t;

typedef struct _soc_monterey_ser_block_info_s {
    int         type;
    soc_block_t blocktype;
    char        name[16];
    soc_reg_t   fifo_reset_reg;
    void        *info;
} _soc_monterey_ser_block_info_t;

typedef struct _soc_monterey_ser_reg_s {
    soc_reg_t reg;
    char      *mem_str;
} _soc_monterey_ser_reg_t;

typedef struct _soc_monterey_ser_info_s {
    _soc_monterey_ser_info_type_t   type;
    struct _soc_monterey_ser_info_s *info;
    int                        id;
    soc_field_t                group_reg_enable_field;
    soc_field_t                group_reg_status_field;
    soc_mem_t                  mem;
    char                      *mem_str;
    soc_reg_t                  enable_reg;
    soc_field_t                enable_field;
    soc_reg_t                  intr_status_reg;
    _soc_monterey_ser_reg_t     *intr_status_reg_list;
    soc_reg_t                  intr_clr_reg;
    soc_field_t                intr_clr_field;
    soc_field_t               *intr_clr_field_list;
} _soc_monterey_ser_info_t;

typedef struct _soc_monterey_ser_route_block_s {
    uint32              cmic_bit;
    soc_block_t         blocktype;
    soc_reg_t           enable_reg;
    soc_reg_t           status_reg;
    soc_field_t         enable_field;
    _soc_monterey_ser_info_t *info;
    uint8               id;
} _soc_monterey_ser_route_block_t;

typedef struct _soc_monterey_intr_info_s {
    _soc_monterey_ser_info_type_t   type;
    struct _soc_monterey_ser_info_s *info;
    int                        id;
    soc_field_t                group_reg_enable_field;
    soc_field_t                group_reg_status_field;
    soc_mem_t                  mem;
    char                      *mem_str;
    soc_reg_t                  enable_reg;
    soc_field_t                enable_field;
    soc_reg_t                  intr_status_reg;
    _soc_monterey_ser_reg_t     *intr_status_reg_list;
    soc_reg_t                  intr_clr_reg;
    soc_field_t                intr_clr_field;
    soc_field_t               *intr_clr_field_list;
} _soc_monterey_intr_info_t;

typedef struct _soc_monterey_intr_route_block_s {
    uint32                    cmic_bit;
    soc_block_t               blocktype;
    soc_reg_t                 enable_reg;
    soc_reg_t                 status_reg;
    soc_field_t               enable_field;
    _soc_monterey_intr_info_t *info;
    uint8                     id;
} _soc_monterey_pm_cpri_intr_info_t;

typedef struct _soc_monterey_pm_ecc_info_s {
    _soc_monterey_ser_info_type_t   type;
    _shr_cpri_ecc_err_type_t ecc_err_type; /* 1-bit, 2-bit, multi-bit */
    int                      port;
    soc_mem_t                mem;
    uint32                   mem_index;
    char                     *mem_str;
} _soc_monterey_pm_ecc_info_t;

typedef struct _soc_monterey_mmu_traffic_ctrl_s {
    uint8   thdo_drop_bmp[_SOC_MONTEREY_MAX_PORT];
} _soc_monterey_mmu_traffic_ctrl_t;

typedef struct _soc_mn_mmu_buffer_config_s {
    int total_physical_memory;
    int cfapfullsetpoint;
    int total_cell_memory;
    int num_mem_banks;
    int num_10g_ports;
    int num_20g_ports;
    int num_40g_ports;
    int num_100g_ports;
    int num_ovsb_10g_ports;
    int num_ovsb_20g_ports;
    int num_ovsb_40g_ports;
    int num_ovsb_100g_ports;
} _soc_mn_mmu_buffer_config_t;

#define _MN_NUM_SKEWS   1
STATIC _soc_mn_mmu_buffer_config_t soc_mn_buffer_config[_MN_NUM_SKEWS] = {
    { 24576, 20416, 20416, 6, 64, 0, 0, 0, 64, 0, 0, 0 }
};
STATIC int _soc_mn_skew_index;

#define _MONTEREY_MMU_PHYSICAL_CELLS                                     \
        soc_mn_buffer_config[_soc_mn_skew_index].total_physical_memory

#define _MONTEREY_CFAPFULLSETPOINT                                    \
        soc_mn_buffer_config[_soc_mn_skew_index].cfapfullsetpoint
#define _MONTEREY_MMU_RSVD_CELLS_CFAP                                 \
        soc_mn_buffer_config[_soc_mn_skew_index].num_mem_banks *    \
                            (23 + 2 + 10)

#define _MONTEREY_MMU_TOTAL_CELLS                                     \
        soc_mn_buffer_config[_soc_mn_skew_index].total_cell_memory

#define   MAC_ENABLE_SET_CTRL          (1 << 0)
#define   MMU_TRAFFIC_EN_CTRL          (1 << 1)
#define   COSQ_SCHED_SET_CTRL          (1 << 2)
static    _soc_monterey_mmu_traffic_ctrl_t
              *_soc_monterey_mmu_traffic_ctrl[SOC_MAX_NUM_DEVICES];
static    sal_mutex_t _fwd_ctrl_lock[SOC_MAX_NUM_DEVICES];
#define   FWD_CTRL_LOCK(unit) \
          sal_mutex_take(_fwd_ctrl_lock[unit], sal_mutex_FOREVER)
#define   FWD_CTRL_UNLOCK(unit) \
          sal_mutex_give(_fwd_ctrl_lock[unit])

typedef struct tsc_dpll_refclk_connect
{
    int pll0_refclk_select;      /* 0: select PAD ref clock; 1: select internal LC ref clock, refer to pll0_lc_refclk_select*/
    int pll0_master_enable;      /* 0: disable; 1: enable ref clock output for other PLL */
    int pll0_lc_refclk_select;   /* 0: select pll0_lc_refclk; 1 select pll1_lc_refclk */
    int pll1_refclk_select;      /* 0: select PAD ref clock; 1: select internal LC ref clock, refer to pll1_lc_refclk_select*/
    int pll1_master_enable;      /* 0: disable; 1: enable ref clock output for other PLL */
    int pll1_lc_refclk_select;   /* 0: select pll0_lc_refclk; 1: select pll1_lc_refclk */
}tsc_dpll_refclk_connect_t;

#define PM_NUM  16

/* CPM cores: TSC0 - TSC2, TSC13 - TSC15               */
/*           select pll0_lc_refclk (122.88MHz) as CPRI */
/*           select pll1_lc_refclk (156.25MHz) as ETH  */
/* PM4X25 cores: TSC3 - TSC4, TSC11 - TSC12            */
/*           TSC3/TSC12 are master PLL core            */
/*           select pll0_lc_refclk                     */
/* PM4X10 cores: TSC5 - TSC7, TSC8 - TSC10             */
/*           TSC6/TSC9 are master PLL core             */
/*           select pll0_lc_refclk                     */
/* This table is for all CPM cores working in Ethernet */
tsc_dpll_refclk_connect_t monterey_tsc_refclk_connect[PM_NUM] =
{
    /* TSC0 CPM4X25 */
    /* PLL0/PLL1 select LC refclk;  disable refclk output; select pll1_lc_refclk */
    {1, 0, 1,  1, 0, 1},
    /* TSC1 CPM4X25 */
    /* PLL0/PLL1 select LC refclk,  disable refclk output, select pll1_lc_refclk */
    {1, 0, 1,  1, 0, 1},
    /* TSC2 CPM4X25 PLL1 master PLL*/
    /* PLL0 select LC refclk,  disable refclk output, select pll1_lc_refclk */
    /* PLL1 select LC refclk,  enable refclk output, select pll1_lc_refclk */
    {1, 0, 1,  1, 1, 1},
    /* TSC3 PM4X25 PLL0 master PLL*/
    /* PLL0 select LC refclk,  enable refclk output, select pll0_lc_refclk */
    /* PLL1 select LC refclk,  disable refclk output, select pll0_lc_refclk */
    {1, 1, 0,  1, 0, 0},
    /* TSC4 PM4X25 */
    /* PLL0/PLL1 select LC refclk,  disable refclk output, select pll0_lc_refclk */
    {1, 0, 0,  1, 0, 0},
    /* TSC5 PM4X10 */
    /* PLL0/PLL1 select LC refclk,  disable refclk output, select pll0_lc_refclk */
    {1, 0, 0,  1, 0, 0},
    /* TSC6 PM4X10  PLL0/PLL1 master pll*/
    /* PLL0/PLL1 select LC refclk,  enable refclk output, select pll0_lc_refclk */
    {1, 1, 0,  1, 1, 0},
    /* TSC7 PM4X10 */
    /* PLL0/PLL1 select LC refclk,  disable refclk output, select pll0_lc_refclk */
    {1, 0, 0,  1, 0, 0},
    /* TSC8 PM4X10 */
    /* PLL0/PLL1 select LC refclk,  disable refclk output, select pll0_lc_refclk */
    {1, 0, 0,  1, 0, 0},
    /* TSC9 PM4X10  PLL0/PLL1 master pll*/
    /* PLL0/PLL1 select LC refclk,  enable refclk output, select pll0_lc_refclk */
    {1, 1, 0,  1, 1, 0},
    /* TSC10 PM4X10 */
    /* PLL0/PLL1 select LC refclk,  disable refclk output, select pll0_lc_refclk */
    {1, 0, 0,  1, 0, 0},
    /* TSC11 PM4X25 */
    /* PLL0/PLL1 select LC refclk,  disable refclk output, select pll0_lc_refclk */
    {1, 0, 0,  1, 0, 0},
    /* TSC12 PM4X25 PLL0 master PLL*/
    /* PLL0 select LC refclk,  enable refclk output, select pll0_lc_refclk */
    /* PLL1 select LC refclk,  disable refclk output, select pll0_lc_refclk */
    {1, 1, 0,  1, 0, 0},
    /* TSC13 CPM4X25 PLL1 master PLL*/
    /* PLL0 select LC refclk,  disable refclk output, select pll1_lc_refclk */
    /* PLL1 select LC refclk,  enable refclk output, select pll1_lc_refclk */
    {1, 0, 1,  1, 1, 1},
    /* TSC14 CPM4X25 */
    /* PLL0/PLL1 select LC refclk,  disable refclk output, select pll1_lc_refclk */
    {1, 0, 1,  1, 0, 1},
    /* TSC15 CPM4X25 */
    /* PLL0/PLL1 select LC refclk,  disable refclk output, select pll1_lc_refclk */
    {1, 0, 1,  1, 0, 1}
};

#define monterey_tsc_is_master_pll(pm)  \
     monterey_tsc_refclk_connect[pm].pll0_master_enable || \
     monterey_tsc_refclk_connect[pm].pll1_master_enable

/* Forward declarations */
STATIC int
_soc_monterey_mdio_reg_read(int unit, uint32 phy_addr,
                            uint32 phy_reg, uint32 *phy_data);
STATIC int
_soc_monterey_mdio_reg_write(int unit, uint32 phy_addr,
                             uint32 phy_reg, uint32 phy_data);

#define _SOC_MONTEREY_SER_TYPE_REG 0
#define _SOC_MONTEREY_SER_TYPE_MEM 1
#define _SOC_MONTEREY_SER_TYPE_BUS 2
#define _SOC_MONTEREY_SER_TYPE_BUF 3

/* These are now generated from the regsfile processing into bcm56670_a0.c
   structures are defined in chip.h */
extern _soc_mem_ser_en_info_t _soc_bcm56670_a0_ip_mem_ser_info[];
extern _soc_reg_ser_en_info_t _soc_bcm56670_a0_ip_reg_ser_info[];
extern _soc_mem_ser_en_info_t _soc_bcm56670_a0_ep_mem_ser_info[];
extern _soc_reg_ser_en_info_t _soc_bcm56670_a0_ep_reg_ser_info[];
extern _soc_mem_ser_en_info_t _soc_bcm56670_a0_mmu_mem_ser_info[];
#if (defined(BCM_56670_B0) ||  defined(BCM_56670_C0))  && defined(INCLUDE_XFLOW_MACSEC)
extern _soc_mem_ser_en_info_t _soc_bcm56670_b0_macsec_mem_ser_info[];
#endif

/* These can be re-used from TD2P. As naming is SOC independent
 * and information is same we'll reuse them here
 */
extern _soc_buffer_ser_en_info_t  _soc_mmu_buffer_ser_info[];


static _soc_bus_ser_en_info_t _soc_monterey_ip_bus_ser_info [] = {
    { "IPARS", IPARS_SER_CONTROLr, IPARS_BUS_PARITY_ENf },
    { "IRSEL1", IRSEL1_SER_CONTROLr, IRSEL1_BUS_PARITY_ENf },
    { "IRSEL2", IRSEL2_SER_CONTROLr, IRSEL2_BUS_PARITY_ENf },
    { "ISW1", ISW1_SER_CONTROLr, ISW1_BUS_PARITY_ENf },
    { "IVXLT", IVXLT_SER_CONTROLr, IVXLT_BUS_PARITY_ENf },
    { "LEARN_FIFO", IARB_SER_CONTROLr, LEARN_FIFO_ECC_ENf },
    { "MPLS", MPLS_SER_CONTROLr, MPLS_BUS_PAR_ENf, },
    { "SW2_EOP_BUF_C", ISW2_SER_CONTROL_1r, SW2_EOP_BUFFER_C_PARITY_ENf },
    { "SW2_EOP_BUF_A", ISW2_SER_CONTROL_1r, SW2_EOP_BUFFER_A_PARITY_ENf },
    { "VP", VP_SER_CONTROLr, VP_BUS_PAR_ENf },
    { "IFP", IFP_PARITY_CONTROLr, IFP_BUS_PARITY_ENf },
    { "IL2L3", IL2L3_BUS_SER_CONTROLr, PARITY_ENf },
    { "", INVALIDr } /* end */
};

static _soc_buffer_ser_en_info_t _soc_monterey_ip_buffer_ser_info[] = {
    { "COUNTER_STAGING", IFP_PARITY_CONTROLr, COUNTER_MUX_DATA_STAGING_PARITY_ENf },
    { "METER_STAGING", IFP_PARITY_CONTROLr, METER_MUX_DATA_STAGING_PARITY_ENf },
    { "CMIC", IARB_SER_CONTROLr, CMIC_BUF_ECC_ENf },
    { "IP STATS COUNTERS", ISW2_SER_CONTROL_1r, IP_COUNTERS_PARITY_ENf },
    { "RDISC/RUC/RPORTD/RDBGC0", ISW2_SER_CONTROL_1r, RDBGC_MEM_INST0_PARITY_ENf },
    { "HG STATS COUNTERS", ISW2_SER_CONTROL_1r, HG_COUNTERS_PARITY_ENf },
    { "RDBGC5/RDBGC6/RDBGC7/RDBGC8", ISW2_SER_CONTROL_1r, RDBGC_MEM_INST2_PARITY_ENf },
    { "RDBGC1/RDBGC2/RDBGC3/RDBGC4", ISW2_SER_CONTROL_1r, RDBGC_MEM_INST1_PARITY_ENf },
    { "ING_NIV_RX_FRAMES_ERRe", ISW2_SER_CONTROL_1r, ING_NIV_RX_FRAMES_ERROR_DROP_PARITY_ENf },
    { "ING_NIV_RX_FRAMES_FRWD", ISW2_SER_CONTROL_1r, ING_NIV_RX_FRAMES_FORWARDING_DROP_PARITY_ENf },
    { "ING_NIV_RX_FRAMES_VLAN", ISW2_SER_CONTROL_1r, ING_NIV_RX_FRAMES_VLAN_TAGGED_PARITY_ENf },
    { "DLB_HGT_FLOWSET_TIME", DLB_HGT_SER_CONTROLr, DLB_HGT_FLOWSET_TIMESTAMP_PAGE_PARITY_ENf },
    { "DLB_HGT_FLOWSET", DLB_HGT_SER_CONTROLr, DLB_HGT_FLOWSET_PARITY_ENf },

    { "", INVALIDr }
};
static _soc_buffer_ser_en_info_t _soc_monterey_ep_buffer_ser_info[] = {
    { "INITBUF", EGR_EL3_ECC_PARITY_CONTROLr, INITBUF_ECC_ENf },
    { "EGR MPB",  EGR_EL3_ECC_PARITY_CONTROLr, EGR_MPB_ECC_ENf },
    { "EDB_CM_MEM", EGR_EDATABUF_PARITY_CONTROLr, CM_ECC_ENf },
    { "XLP5_CELL_BUF",  EGR_EDATABUF_PARITY_CONTROLr, XLP5_ECC_ENf  },
    { "XLP4_CELL_BUF",  EGR_EDATABUF_PARITY_CONTROLr, XLP4_ECC_ENf  },
    { "XLP3_CELL_BUF",  EGR_EDATABUF_PARITY_CONTROLr, XLP3_ECC_ENf  },
    { "XLP2_CELL_BUF",  EGR_EDATABUF_PARITY_CONTROLr, XLP2_ECC_ENf  },
    { "XLP1_CELL_BUF",  EGR_EDATABUF_PARITY_CONTROLr, XLP1_ECC_ENf  },
    { "XLP0_CELL_BUF",  EGR_EDATABUF_PARITY_CONTROLr, XLP0_ECC_ENf  },
    { "CLP9_CELL_BUF",  EGR_EDATABUF_PARITY_CONTROLr, CLP9_ECC_ENf  },
    { "CLP8_CELL_BUF",  EGR_EDATABUF_PARITY_CONTROLr, CLP8_ECC_ENf  },
    { "CLP7_CELL_BUF",  EGR_EDATABUF_PARITY_CONTROLr, CLP7_ECC_ENf  },
    { "CLP6_CELL_BUF",  EGR_EDATABUF_PARITY_CONTROLr, CLP6_ECC_ENf  },
    { "CLP5_CELL_BUF",  EGR_EDATABUF_PARITY_CONTROLr, CLP5_ECC_ENf  },
    { "CLP4_CELL_BUF",  EGR_EDATABUF_PARITY_CONTROLr, CLP4_ECC_ENf  },
    { "CLP3_CELL_BUF",  EGR_EDATABUF_PARITY_CONTROLr, CLP3_ECC_ENf  },
    { "CLP2_CELL_BUF",  EGR_EDATABUF_PARITY_CONTROLr, CLP2_ECC_ENf  },
    { "CLP1_CELL_BUF",  EGR_EDATABUF_PARITY_CONTROLr, CLP1_ECC_ENf  },
    { "CLP0_CELL_BUF",  EGR_EDATABUF_PARITY_CONTROLr, CLP0_ECC_ENf  },
    { "LPBK_BUF",       EGR_EDATABUF_PARITY_CONTROLr, LBP_ECC_ENf  },
    { "XLP5_PREEMPT_CELL_BUF",  EGR_EDB_PREEMPT_XFR_COUNT_PARITY_ENr, XLP5_ECC_ENf  },
    { "XLP4_PREEMPT_CELL_BUF",  EGR_EDB_PREEMPT_XFR_COUNT_PARITY_ENr, XLP4_ECC_ENf  },
    { "XLP3_PREEMPT_CELL_BUF",  EGR_EDB_PREEMPT_XFR_COUNT_PARITY_ENr, XLP3_ECC_ENf  },
    { "XLP2_PREEMPT__CELL_BUF",  EGR_EDB_PREEMPT_XFR_COUNT_PARITY_ENr, XLP2_ECC_ENf  },
    { "XLP1_PREEMPT_CELL_BUF",  EGR_EDB_PREEMPT_XFR_COUNT_PARITY_ENr, XLP1_ECC_ENf  },
    { "XLP0_PREEMPT__CELL_BUF",  EGR_EDB_PREEMPT_XFR_COUNT_PARITY_ENr, XLP0_ECC_ENf  },
    { "CLP9_PREEMPT_CELL_BUF",  EGR_EDB_PREEMPT_XFR_COUNT_PARITY_ENr, CLP9_ECC_ENf  },
    { "CLP8_PREEMPT_CELL_BUF",  EGR_EDB_PREEMPT_XFR_COUNT_PARITY_ENr, CLP8_ECC_ENf  },
    { "CLP7_PREEMPT_CELL_BUF",  EGR_EDB_PREEMPT_XFR_COUNT_PARITY_ENr, CLP7_ECC_ENf  },
    { "CLP_6PREEMPT__CELL_BUF",  EGR_EDB_PREEMPT_XFR_COUNT_PARITY_ENr, CLP6_ECC_ENf  },
    { "CLP5_PREEMPT_CELL_BUF",  EGR_EDB_PREEMPT_XFR_COUNT_PARITY_ENr, CLP5_ECC_ENf  },
    { "CLP4_PREEMPT_CELL_BUF",  EGR_EDB_PREEMPT_XFR_COUNT_PARITY_ENr, CLP4_ECC_ENf  },
    { "CLP3_PREEMPT_CELL_BUF",  EGR_EDB_PREEMPT_XFR_COUNT_PARITY_ENr, CLP3_ECC_ENf  },
    { "CLP2_PREEMPT__CELL_BUF",  EGR_EDB_PREEMPT_XFR_COUNT_PARITY_ENr, CLP2_ECC_ENf  },
    { "CLP1_PREEMPT_CELL_BUF",  EGR_EDB_PREEMPT_XFR_COUNT_PARITY_ENr, CLP1_ECC_ENf  },
    { "CLP_0PREEMPT__CELL_BUF",  EGR_EDB_PREEMPT_XFR_COUNT_PARITY_ENr, CLP0_ECC_ENf  },
    { "XLP5_PREEMPT_CELL_BUF1",  EGR_EDB_PREEMPT_LOOKUP_PARITY_ENr, XLP5_ECC_ENf  },
    { "XLP4_PREEMPT_CELL_BUF1",  EGR_EDB_PREEMPT_LOOKUP_PARITY_ENr, XLP4_ECC_ENf  },
    { "XLP3_PREEMPT_CELL_BUF1",  EGR_EDB_PREEMPT_LOOKUP_PARITY_ENr, XLP3_ECC_ENf  },
    { "XLP2_PREEMPT__CELL_BUF1", EGR_EDB_PREEMPT_LOOKUP_PARITY_ENr, XLP2_ECC_ENf  },
    { "XLP1_PREEMPT_CELL_BUF1",  EGR_EDB_PREEMPT_LOOKUP_PARITY_ENr, XLP1_ECC_ENf  },
    { "XLP0_PREEMPT__CELL_BUF1", EGR_EDB_PREEMPT_LOOKUP_PARITY_ENr, XLP0_ECC_ENf  },
    { "CLP9_PREEMPT_CELL_BUF1",  EGR_EDB_PREEMPT_LOOKUP_PARITY_ENr, CLP9_ECC_ENf  },
    { "CLP8_PREEMPT_CELL_BUF1",  EGR_EDB_PREEMPT_LOOKUP_PARITY_ENr, CLP8_ECC_ENf  },
    { "CLP7_PREEMPT_CELL_BUF1",  EGR_EDB_PREEMPT_LOOKUP_PARITY_ENr, CLP7_ECC_ENf  },
    { "CLP_6PREEMPT__CELL_BUF1", EGR_EDB_PREEMPT_LOOKUP_PARITY_ENr, CLP6_ECC_ENf  },
    { "CLP5_PREEMPT_CELL_BUF1",  EGR_EDB_PREEMPT_LOOKUP_PARITY_ENr, CLP5_ECC_ENf  },
    { "CLP4_PREEMPT_CELL_BUF1",  EGR_EDB_PREEMPT_LOOKUP_PARITY_ENr, CLP4_ECC_ENf  },
    { "CLP3_PREEMPT_CELL_BUF1",  EGR_EDB_PREEMPT_LOOKUP_PARITY_ENr, CLP3_ECC_ENf  },
    { "CLP2_PREEMPT__CELL_BUF1", EGR_EDB_PREEMPT_LOOKUP_PARITY_ENr, CLP2_ECC_ENf  },
    { "CLP1_PREEMPT_CELL_BUF1",  EGR_EDB_PREEMPT_LOOKUP_PARITY_ENr, CLP1_ECC_ENf  },
    { "CLP0_PREEMPT_CELL_BUF1",  EGR_EDB_PREEMPT_LOOKUP_PARITY_ENr, CLP0_ECC_ENf  },
    { "EP_XLP5__DATABUF_CONTROL_INFO", EGR_EDB_CTRL_PARITY_ENr, XLP5_ECC_ENf  },
    { "EP_XLP4__DATABUF_CONTROL_INFO", EGR_EDB_CTRL_PARITY_ENr, XLP4_ECC_ENf  },
    { "EP_XLP3__DATABUF_CONTROL_INFO", EGR_EDB_CTRL_PARITY_ENr, XLP3_ECC_ENf  },
    { "EP_XLP2__DATABUF_CONTROL_INFO", EGR_EDB_CTRL_PARITY_ENr, XLP2_ECC_ENf  },
    { "EP_XLP1__DATABUF_CONTROL_INFO", EGR_EDB_CTRL_PARITY_ENr, XLP1_ECC_ENf  },
    { "EP_XLP0__DATABUF_CONTROL_INFO", EGR_EDB_CTRL_PARITY_ENr, XLP0_ECC_ENf  },
    { "EP_CLP9__DATABUF_CONTROL_INFO", EGR_EDB_CTRL_PARITY_ENr, CLP9_ECC_ENf  },
    { "EP_CLP8__DATABUF_CONTROL_INFO", EGR_EDB_CTRL_PARITY_ENr, CLP8_ECC_ENf  },
    { "EP_CLP7__DATABUF_CONTROL_INFO", EGR_EDB_CTRL_PARITY_ENr, CLP7_ECC_ENf  },
    { "EP_CLP6__DATABUF_CONTROL_INFO", EGR_EDB_CTRL_PARITY_ENr, CLP6_ECC_ENf  },
    { "EP_CLP5__DATABUF_CONTROL_INFO", EGR_EDB_CTRL_PARITY_ENr, CLP5_ECC_ENf  },
    { "EP_CLP4__DATABUF_CONTROL_INFO", EGR_EDB_CTRL_PARITY_ENr, CLP4_ECC_ENf  },
     { "EP_CLP3__DATABUF_CONTROL_INFO", EGR_EDB_CTRL_PARITY_ENr, CLP3_ECC_ENf  },
    { "EP_CLP2__DATABUF_CONTROL_INFO", EGR_EDB_CTRL_PARITY_ENr, CLP2_ECC_ENf  },
    { "EP_CLP1__DATABUF_CONTROL_INFO", EGR_EDB_CTRL_PARITY_ENr, CLP1_ECC_ENf  },
    { "EP_CLP0__DATABUF_CONTROL_INFO", EGR_EDB_CTRL_PARITY_ENr, CLP0_ECC_ENf  },
    { "EP_XLP5__PKT_LEN", EGR_EDB_PKT_LEN_PARITY_ENr, XLP5_ECC_ENf  },
    { "EP_XLP4__PKT_LEN", EGR_EDB_PKT_LEN_PARITY_ENr, XLP4_ECC_ENf  },
    { "EP_XLP3__PKT_LEN", EGR_EDB_PKT_LEN_PARITY_ENr, XLP3_ECC_ENf  },
    { "EP_XLP2__PKT_LEN", EGR_EDB_PKT_LEN_PARITY_ENr, XLP2_ECC_ENf  },
    { "EP_XLP1__PKT_LEN", EGR_EDB_PKT_LEN_PARITY_ENr, XLP1_ECC_ENf  },
    { "EP_XLP0__PKT_LEN", EGR_EDB_PKT_LEN_PARITY_ENr, XLP0_ECC_ENf  },
    { "EP_CLP9__PKT_LEN", EGR_EDB_PKT_LEN_PARITY_ENr, CLP9_ECC_ENf  },
    { "EP_CLP8__PKT_LEN", EGR_EDB_PKT_LEN_PARITY_ENr, CLP8_ECC_ENf  },
    { "EP_CLP7__PKT_LEN", EGR_EDB_PKT_LEN_PARITY_ENr, CLP7_ECC_ENf  },
    { "EP_CLP6__PKT_LEN", EGR_EDB_PKT_LEN_PARITY_ENr, CLP6_ECC_ENf  },
    { "EP_CLP5__PKT_LEN", EGR_EDB_PKT_LEN_PARITY_ENr, CLP5_ECC_ENf  },
    { "EP_CLP4__PKT_LEN", EGR_EDB_PKT_LEN_PARITY_ENr, CLP4_ECC_ENf  },
    { "EP_CLP3__PKT-LEN", EGR_EDB_PKT_LEN_PARITY_ENr, CLP3_ECC_ENf  },
    { "EP_CLP2__PKT_LEN", EGR_EDB_PKT_LEN_PARITY_ENr, CLP2_ECC_ENf  },
    { "EP_CLP1__PKT_LEN", EGR_EDB_PKT_LEN_PARITY_ENr, CLP1_ECC_ENf  },
    { "EP_CLP0__PKT_LEN", EGR_EDB_PKT_LEN_PARITY_ENr, CLP0_ECC_ENf  },


    { "", INVALIDr }
};



static _soc_bus_ser_en_info_t _soc_ep_bus_ser_info [] = {
    { "EFP",     EFP_PARITY_CONTROLr,      EFP_BUS_PARITY_ENf     },
    { "EFPPARS", EGR_EFPPARS_SER_CONTROLr, EFPPARS_BUS_PARITY_ENf },
    { "EHCPM",   EGR_EHCPM_SER_CONTROLr,   EHCPM_BUS_PARITY_ENf },
    { "EPMOD",   EGR_EPMOD_SER_CONTROLr,   EPMOD_BUS_PARITY_ENf },
    { "EVLAN",   EGR_VLAN_SER_CONTROLr,    EVLAN_BUS_PARITY_ENf },
    { "", INVALIDr } /* end */
};



static const _soc_monterey_ser_block_info_t _soc_monterey_ser_block_info[] = {
    { _SOC_MONTEREY_SER_TYPE_REG, SOC_BLK_IPIPE, "IPIPE regs", INVALIDr, _soc_bcm56670_a0_ip_reg_ser_info },
    { _SOC_MONTEREY_SER_TYPE_MEM, SOC_BLK_IPIPE, "IPIPE mems", ING_SER_FIFO_CTRLr, _soc_bcm56670_a0_ip_mem_ser_info },
    { _SOC_MONTEREY_SER_TYPE_BUS, SOC_BLK_IPIPE, "IPIPE buses",INVALIDr, _soc_monterey_ip_bus_ser_info },
    { _SOC_MONTEREY_SER_TYPE_BUF, SOC_BLK_IPIPE, "IPIPE buff" ,INVALIDr, _soc_monterey_ip_buffer_ser_info },
    { _SOC_MONTEREY_SER_TYPE_REG, SOC_BLK_EPIPE, "EPIPE regs", INVALIDr, _soc_bcm56670_a0_ep_reg_ser_info },
    { _SOC_MONTEREY_SER_TYPE_BUS, SOC_BLK_EPIPE, "EPIPE buses",INVALIDr, _soc_ep_bus_ser_info },
    { _SOC_MONTEREY_SER_TYPE_BUF, SOC_BLK_EPIPE, "EPIPE buff" ,INVALIDr, _soc_monterey_ep_buffer_ser_info },
    { _SOC_MONTEREY_SER_TYPE_MEM, SOC_BLK_EPIPE, "EPIPE mems", EGR_SER_FIFO_CTRLr, _soc_bcm56670_a0_ep_mem_ser_info },
    { _SOC_MONTEREY_SER_TYPE_MEM, SOC_BLK_MMU,   "MMU mems", INVALIDr, _soc_bcm56670_a0_mmu_mem_ser_info },
    { _SOC_MONTEREY_SER_TYPE_BUF, SOC_BLK_MMU,   "MMU buff", INVALIDr, _soc_mmu_buffer_ser_info },
#if (defined(BCM_56670_B0) ||  defined(BCM_56670_C0))  && defined(INCLUDE_XFLOW_MACSEC)
    { _SOC_MONTEREY_SER_TYPE_MEM, SOC_BLK_MACSEC,"MACSEC mems", INVALIDr, _soc_bcm56670_b0_macsec_mem_ser_info },
#endif
    { -1, 0 }
};


STATIC _soc_monterey_ser_info_t _soc_monterey_mmu_ser_info[] = {
    { _SOC_PARITY_TYPE_GENERIC, NULL, 0,
        DEQ_NOT_IP_ERR_ENf,
        DEQ_NOT_IP_ERRf,
        INVALIDm, "MMU DEQ NOT IP",
        INVALIDr, INVALIDf,
        INVALIDr, NULL,
        INVALIDr, INVALIDf, NULL },
    { _SOC_PARITY_TYPE_START_ERR, NULL, 0,
        START_BY_START_ERR_ENf,
        START_BY_START_ERRf,
        INVALIDm, "MMU START BY START",
        INVALIDr, INVALIDf,
        INVALIDr, NULL,
        INVALIDr, INVALIDf, NULL },
    { _SOC_PARITY_TYPE_MMU_SER, NULL, 0,
        MEM_PAR_ERR_ENf,
        MEM_PAR_ERRf,
        INVALIDm, "MMU MEM PAR",
        INVALIDr, INVALIDf,
        MEM_FAIL_INT_STATr, NULL,
        INVALIDr, INVALIDf, NULL },
    { _SOC_PARITY_TYPE_BST, NULL, 0,
        INVALIDf,
        BST_THDI_INTf,
        INVALIDm, NULL,
        INVALIDr, INVALIDf,
        INVALIDr, NULL,
        INVALIDr, INVALIDf, NULL },
    { _SOC_PARITY_TYPE_BST, NULL, 0,
        INVALIDf,
        BST_THDO_INTf,
        INVALIDm, NULL,
        INVALIDr, INVALIDf,
        INVALIDr, NULL,
        INVALIDr, INVALIDf, NULL },
    { _SOC_PARITY_TYPE_BST, NULL, 0,
            INVALIDf,
        BST_CFAP_INTf,
        INVALIDm, NULL,
        INVALIDr, INVALIDf,
        INVALIDr, NULL,
        INVALIDr, INVALIDf, NULL },
    { _SOC_PARITY_TYPE_NONE } /* table terminator */
};

STATIC _soc_monterey_ser_reg_t _soc_monterey_pg_bod_status_reg[] = {
    { PGW_BOD_CA0_ECC_STATUSr, "PGW BOD CA0 ECC" },
    { PGW_BOD_CA1_ECC_STATUSr, "PGW BOD CA1 ECC" },
    { PGW_BOD_CA2_ECC_STATUSr, "PGW BOD CA2 ECC" },
    { PGW_BOD_CA3_ECC_STATUSr, "PGW BOD CA3 ECC" },
    { PGW_BOD_CA4_ECC_STATUSr, "PGW BOD CA4 ECC" },
    { PGW_BOD_CA5_ECC_STATUSr, "PGW BOD CA5 ECC" },
    { PGW_BOD_CA6_ECC_STATUSr, "PGW BOD CA6 ECC" },
    { PGW_BOD_CA7_ECC_STATUSr, "PGW BOD CA7 ECC" },
    { INVALIDr }
};
STATIC _soc_monterey_ser_info_t _soc_monterey_pg_ser_info[] = {
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        PGW_OBM_DATA_MEM_ECC_ERR_ENf,
        PGW_OBM_DATA_MEM_ECC_ERRf,
        INVALIDm, "PGW OBM0",
        IDB_OBM0_SER_CONTROLr, DATA_ECC_ENABLEf,
        IDB_OBM0_DATA_ECC_STATUSr, NULL,
        INVALIDr, INVALIDf, NULL },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        PGW_OBM_DATA_MEM_ECC_ERR_ENf,
        PGW_OBM_DATA_MEM_ECC_ERRf,
        INVALIDm, "PGW OBM1",
        IDB_OBM1_SER_CONTROLr, DATA_ECC_ENABLEf,
        IDB_OBM1_DATA_ECC_STATUSr, NULL,
        INVALIDr, INVALIDf, NULL },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        PGW_OBM_DATA_MEM_ECC_ERR_ENf,
        PGW_OBM_DATA_MEM_ECC_ERRf,
        INVALIDm, "PGW OBM2",
        IDB_OBM2_SER_CONTROLr, DATA_ECC_ENABLEf,
        IDB_OBM2_DATA_ECC_STATUSr, NULL,
        INVALIDr, INVALIDf, NULL },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        PGW_OBM_DATA_MEM_ECC_ERR_ENf,
        PGW_OBM_DATA_MEM_ECC_ERRf,
        INVALIDm, "PGW OBM3",
        IDB_OBM3_SER_CONTROLr, DATA_ECC_ENABLEf,
        IDB_OBM3_DATA_ECC_STATUSr, NULL,
        INVALIDr, INVALIDf, NULL },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        PGW_OBM_DATA_MEM_ECC_ERR_ENf,
        PGW_OBM_DATA_MEM_ECC_ERRf,
        INVALIDm, "PGW OBM4",
        IDB_OBM4_SER_CONTROLr, DATA_ECC_ENABLEf,
        IDB_OBM4_DATA_ECC_STATUSr, NULL,
        INVALIDr, INVALIDf, NULL },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        PGW_OBM_DATA_MEM_ECC_ERR_ENf,
        PGW_OBM_DATA_MEM_ECC_ERRf,
        INVALIDm, "PGW OBM5",
        IDB_OBM5_SER_CONTROLr, DATA_ECC_ENABLEf,
        IDB_OBM5_DATA_ECC_STATUSr, NULL,
        INVALIDr, INVALIDf, NULL },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        PGW_OBM_DATA_MEM_ECC_ERR_ENf,
        PGW_OBM_DATA_MEM_ECC_ERRf,
        INVALIDm, "PGW OBM6",
        IDB_OBM6_SER_CONTROLr, DATA_ECC_ENABLEf,
        IDB_OBM6_DATA_ECC_STATUSr, NULL,
        INVALIDr, INVALIDf, NULL },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        PGW_OBM_DATA_MEM_ECC_ERR_ENf,
        PGW_OBM_DATA_MEM_ECC_ERRf,
        INVALIDm, "PGW OBM7",
        IDB_OBM7_SER_CONTROLr, DATA_ECC_ENABLEf,
        IDB_OBM7_DATA_ECC_STATUSr, NULL,
        INVALIDr, INVALIDf, NULL },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        PGW_OBM_QUEUE_MEM_ECC_ERR_ENf ,
        PGW_OBM_QUEUE_MEM_ECC_ERRf,
        INVALIDm, "PGW OBM0",
        IDB_OBM0_QUEUE_SER_CONTROLr, ECC_ENABLEf,
        IDB_OBM0_QUEUE_ECC_STATUSr, NULL,
        INVALIDr, INVALIDf, NULL },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
       PGW_OBM_QUEUE_MEM_ECC_ERR_ENf ,
        PGW_OBM_QUEUE_MEM_ECC_ERRf,
        INVALIDm, "PGW OBM1",
        IDB_OBM1_QUEUE_SER_CONTROLr, ECC_ENABLEf,
        IDB_OBM1_QUEUE_ECC_STATUSr, NULL,
        INVALIDr, INVALIDf, NULL },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
       PGW_OBM_QUEUE_MEM_ECC_ERR_ENf ,
        PGW_OBM_QUEUE_MEM_ECC_ERRf,
        INVALIDm, "PGW OBM2",
        IDB_OBM2_QUEUE_SER_CONTROLr, ECC_ENABLEf,
        IDB_OBM2_QUEUE_ECC_STATUSr, NULL,
        INVALIDr, INVALIDf, NULL },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
       PGW_OBM_QUEUE_MEM_ECC_ERR_ENf ,
        PGW_OBM_QUEUE_MEM_ECC_ERRf,
        INVALIDm, "PGW OBM3",
        IDB_OBM3_QUEUE_SER_CONTROLr, ECC_ENABLEf,
        IDB_OBM3_QUEUE_ECC_STATUSr, NULL,
        INVALIDr, INVALIDf, NULL },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
       PGW_OBM_QUEUE_MEM_ECC_ERR_ENf ,
        PGW_OBM_QUEUE_MEM_ECC_ERRf,
        INVALIDm, "PGW OBM4",
        IDB_OBM4_QUEUE_SER_CONTROLr, ECC_ENABLEf,
        IDB_OBM4_QUEUE_ECC_STATUSr, NULL,
        INVALIDr, INVALIDf, NULL },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
       PGW_OBM_QUEUE_MEM_ECC_ERR_ENf ,
        PGW_OBM_QUEUE_MEM_ECC_ERRf,
        INVALIDm, "PGW OBM5",
        IDB_OBM5_QUEUE_SER_CONTROLr, ECC_ENABLEf,
        IDB_OBM5_QUEUE_ECC_STATUSr, NULL,
        INVALIDr, INVALIDf, NULL },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
       PGW_OBM_QUEUE_MEM_ECC_ERR_ENf ,
        PGW_OBM_QUEUE_MEM_ECC_ERRf,
        INVALIDm, "PGW OBM6",
        IDB_OBM6_QUEUE_SER_CONTROLr, ECC_ENABLEf,
        IDB_OBM6_QUEUE_ECC_STATUSr, NULL,
        INVALIDr, INVALIDf, NULL },
    { _SOC_PARITY_TYPE_ECC, NULL, 0,
       PGW_OBM_QUEUE_MEM_ECC_ERR_ENf ,
        PGW_OBM_QUEUE_MEM_ECC_ERRf,
        INVALIDm, "PGW OBM7",
        IDB_OBM7_QUEUE_SER_CONTROLr, ECC_ENABLEf,
        IDB_OBM7_QUEUE_ECC_STATUSr, NULL,
        INVALIDr, INVALIDf, NULL },

    { _SOC_PARITY_TYPE_ECC, NULL, 0,
        PGW_CA_DATA_MEM_ECC_ERR_ENf,
        PGW_CA_DATA_MEM_ECC_ERRf,
        INVALIDm, "PGW BOD",
        PGW_BOD_ECC_ENABLEr, BOD_ECC_ENABLEf,
        INVALIDr, _soc_monterey_pg_bod_status_reg,
        INVALIDr, INVALIDf, NULL },
    { _SOC_PARITY_TYPE_OBM, NULL, 0,
        PGW_OBM_DROPS_OR_CA_OVERFLOW_ERR_ENf,
        PGW_OBM_DROPS_OR_CA_OVERFLOW_ERRf,
        INVALIDm, "PGW OBM",
        INVALIDr, INVALIDr,
        INVALIDr, NULL,
        INVALIDr, INVALIDf, NULL },
    { _SOC_PARITY_TYPE_NONE } /* table terminator */
};


#ifdef INCLUDE_XFLOW_MACSEC
STATIC _soc_monterey_ser_info_t _soc_monterey_macsec_ser_info[] = {
    { _SOC_PARITY_TYPE_MACSEC_SER, NULL, 0,
        ECCERR_INT_ENf,
        INVALIDf,
        INVALIDm, NULL,
        INVALIDr, INVALIDf,
        INVALIDr, NULL,
        INVALIDr, INVALIDf, NULL },
    { _SOC_PARITY_TYPE_MACSEC_FUNCTIONAL, NULL, 0,
        EGRESS_SA_SOFT_EXPIRE_INT_ENf,
        INVALIDf,
        INVALIDm, NULL,
        INVALIDr, INVALIDf,
        INVALIDr, NULL,
        INVALIDr, INVALIDf, NULL },
    { _SOC_PARITY_TYPE_MACSEC_FUNCTIONAL, NULL, 0,
        EGRESS_SA_EXPIRE_INT_ENf,
        INVALIDf,
        INVALIDm, NULL,
        INVALIDr, INVALIDf,
        INVALIDr, NULL,
        INVALIDr, INVALIDf, NULL },
    { _SOC_PARITY_TYPE_MACSEC_FUNCTIONAL, NULL, 0,
        INGRESS_SA_SOFT_EXPIRE_INT_ENf,
        INVALIDf,
        INVALIDm, NULL,
        INVALIDr, INVALIDf,
        INVALIDr, NULL,
        INVALIDr, INVALIDf, NULL },
    { _SOC_PARITY_TYPE_MACSEC_FUNCTIONAL, NULL, 0,
        INGRESS_SA_EXPIRE_INT_ENf,
        INVALIDf,
        INVALIDm, NULL,
        INVALIDr, INVALIDf,
        INVALIDr, NULL,
        INVALIDr, INVALIDf, NULL },
    { _SOC_PARITY_TYPE_NONE } /* table terminator */
};
#endif
static const
_soc_monterey_ser_route_block_t  _soc_monterey_ser_route_blocks_irq3[] = {
    { 0x00000001, /* MMU_TO_CMIC_MEMFAIL_INTR */
      SOC_BLK_MMU, MEM_FAIL_INT_ENr, MEM_FAIL_INT_STATr, INVALIDf,
      _soc_monterey_mmu_ser_info, 0 },
    { 0x00000002, /* EP1_TO_CMIC_PERR_INTR */
      SOC_BLK_EPIPE, EGR_INTR_ENABLEr, INVALIDr, SER_FIFO_NON_EMPTYf, NULL, 0 },
    { 0x00000004, /* EP2_TO_CMIC_PERR_INTR */
      SOC_BLK_EPIPE, EGR_INTR_ENABLEr, INVALIDr, SER_FIFO_NON_EMPTYf, NULL, 0 },
    { 0x00000008, /* IP0_TO_CMIC_PERR_INTR */
      SOC_BLK_IPIPE, INVALIDr, INVALIDr, INVALIDf, NULL, 0 },
    { 0x00000010, /* IP1_TO_CMIC_PERR_INTR */
      SOC_BLK_IPIPE, INVALIDr, INVALIDr, INVALIDf, NULL, 0 },
    { 0x00000020, /* IP2_TO_CMIC_PERR_INTR */
      SOC_BLK_IPIPE, INVALIDr, INVALIDr, INVALIDf, NULL, 0 },
    { 0x00000040, /* IP3_TO_CMIC_PERR_INTR */
      SOC_BLK_IPIPE, INVALIDr, INVALIDr, INVALIDf, NULL, 0 },
    { 0x00000080, /* IP4_TO_CMIC_PERR_INTR */
      SOC_BLK_IPIPE, INVALIDr, INVALIDr, INVALIDf, NULL, 0 },
    { 0x00000100, /* IP5_TO_CMIC_PERR_INTR */
      SOC_BLK_IPIPE, INVALIDr, INVALIDr, INVALIDf, NULL, 0 },
    { 0x00000200, /* PGW_0_TO_CMIC_PERR_INTR */
      SOC_BLK_PGW_CL, PGW_INTR_ENABLEr, PGW_INTR_STATUSr, INVALIDf,
      _soc_monterey_pg_ser_info, 0 },
    { 0x00000400, /* PGW_1_TO_CMIC_PERR_INTR */
      SOC_BLK_PGW_CL, PGW_INTR_ENABLEr, PGW_INTR_STATUSr, INVALIDf,
      _soc_monterey_pg_ser_info, 1 },
    /* bits 11 12 13 and 14 do not look SER related. Leaving them alone */
    { 0 } /* table terminator */
};
/*
static soc_field_t _soc_monterey_pm_rx_cdc_clear_field[] = {
    CLEAR_RX_CDC_SINGLE_BIT_ERRf, CLEAR_RX_CDC_DOUBLE_BIT_ERRf
};
static soc_field_t _soc_monterey_pm_tx_cdc_clear_field[] = {
    CLEAR_TX_CDC_SINGLE_BIT_ERRf, CLEAR_TX_CDC_DOUBLE_BIT_ERRf
};


static soc_field_t _soc_monterey_pm_rx_ts_clear_field[] = {
    CLEAR_RX_TS_MEM_SINGLE_BIT_ERRf, CLEAR_RX_TS_MEM_DOUBLE_BIT_ERRf
};
*/
static _soc_monterey_ser_reg_t _soc_monterey_pm_xlp_rx_status_reg[] = {
    { XLPORT_MIB_RSC0_ECC_STATUSr, "XLP MIB RSC0 ECC" },
    { XLPORT_MIB_RSC1_ECC_STATUSr, "XLP MIB RSC1 ECC"},
    { INVALIDr }

};
static _soc_monterey_ser_reg_t _soc_monterey_pm_xlp_tx_status_reg[] = {
    { XLPORT_MIB_TSC0_ECC_STATUSr, "XLP MIB TSC0 ECC" },
    { XLPORT_MIB_TSC1_ECC_STATUSr, "XLP MIB TSC1 ECC" },
    { INVALIDr }
};

static  _soc_monterey_ser_info_t _soc_monterey_xlport_ser_info[] = {
    /* In addition to below we have TSC_ERROR, RX_FLOWCONTROL_REQ_FULL
     * interrupts in XLPORT. Currently not being handled.
     */
    { _SOC_PARITY_TYPE_MIB, NULL, 0,
      MIB_RX_MEM_ERRf, MIB_RX_MEM_ERRf,
      INVALIDm, "MIB RX Statistic Counter memory",
      XLPORT_ECC_CONTROLr, MIB_RSC_MEM_ENf,
      INVALIDr, _soc_monterey_pm_xlp_rx_status_reg,
      XLPORT_MIB_RESETr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_MIB, NULL, 0,
      MIB_TX_MEM_ERRf, MIB_TX_MEM_ERRf,
      INVALIDm, "MIB TX Statistic Counter memory",
      XLPORT_ECC_CONTROLr, MIB_TSC_MEM_ENf,
      INVALIDr, _soc_monterey_pm_xlp_tx_status_reg,
      XLPORT_MIB_RESETr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_NONE } /* table terminator */
};

static _soc_monterey_ser_reg_t _soc_monterey_pm_clp_rx_status_reg[] = {
    { CLPORT_MIB_RSC0_ECC_STATUSr, "CLP MIB RSC0 ECC" },
    { CLPORT_MIB_RSC1_ECC_STATUSr, "CLP MIB RSC1 ECC" }
};

static _soc_monterey_ser_reg_t _soc_monterey_pm_clp_tx_status_reg[] = {
    { CLPORT_MIB_TSC0_ECC_STATUSr, "CLP MIB TSC0 ECC" },
    { CLPORT_MIB_TSC1_ECC_STATUSr, "CLP MIB TSC1 ECC" }
};
static  _soc_monterey_ser_info_t _soc_monterey_clport_ser_info[] = {
    /* In addition to below we have TSC_ERROR, PMD_ERR, RX_FLOWCONTROL_REQ_FULL
     * interrupts in CLPORT. Currently not being handled
     */
    { _SOC_PARITY_TYPE_MIB, NULL, 0,
      MIB_RX_MEM_ERRf, MIB_RX_MEM_ERRf,
      INVALIDm, "MIB RX Statistic Counter memory",
      CLPORT_ECC_CONTROLr, MIB_RSC_MEM_ENf,
      INVALIDr, _soc_monterey_pm_clp_rx_status_reg,
      CLPORT_MIB_RESETr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_MIB, NULL, 0,
      MIB_TX_MEM_ERRf, MIB_TX_MEM_ERRf,
      INVALIDm, "MIB TX Statistic Counter memory",
      CLPORT_ECC_CONTROLr, MIB_TSC_MEM_ENf,
      INVALIDr, _soc_monterey_pm_clp_tx_status_reg,
      CLPORT_MIB_RESETr, INVALIDf, NULL
    },
    { _SOC_PARITY_TYPE_NONE } /* table terminator */
};


static const
_soc_monterey_ser_route_block_t  _soc_monterey_ser_route_blocks_irq4[] = {
    {0x00000001, /* Port Macro0 (PM0) interrupt */
     SOC_BLK_CLPORT, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
     _soc_monterey_clport_ser_info, 0},
    {0x00000002, /* Port Macro1 (PM1) interrupt */
     SOC_BLK_CLPORT, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
     _soc_monterey_clport_ser_info, 1},
    {0x00000004, /* Port Macro2 (PM2) interrupt */
     SOC_BLK_CLPORT, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
     _soc_monterey_clport_ser_info, 2},
    {0x00000008, /* Port Macro3 (PM3) interrupt */
     SOC_BLK_CLPORT, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
     _soc_monterey_clport_ser_info, 3},
    {0x00000010, /* Port Macro4 (PM4) interrupt */
     SOC_BLK_CLPORT, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
     _soc_monterey_clport_ser_info, 0},
    {0x00000020, /* Port Macro5 (PM5) interrupt */
     SOC_BLK_XLPORT, XLPORT_INTR_ENABLEr, XLPORT_INTR_STATUSr, INVALIDf,
     _soc_monterey_xlport_ser_info, 1},
    {0x00000040, /* Port Macro6 (PM6) interrupt */
     SOC_BLK_XLPORT, XLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
     _soc_monterey_xlport_ser_info, 2},
    {0x00000080, /* Port Macro7 (PM7) interrupt */
     SOC_BLK_XLPORT, XLPORT_INTR_ENABLEr, XLPORT_INTR_STATUSr, INVALIDf,
     _soc_monterey_xlport_ser_info, 7},
    {0x00000100, /* Port Macro8 (PM8) interrupt */
     SOC_BLK_XLPORT, XLPORT_INTR_ENABLEr, XLPORT_INTR_STATUSr, INVALIDf,
     _soc_monterey_xlport_ser_info, 8},
    {0x00000200, /* Port Macro9 (PM9) interrupt */
     SOC_BLK_XLPORT, XLPORT_INTR_ENABLEr, XLPORT_INTR_STATUSr, INVALIDf,
     _soc_monterey_xlport_ser_info, 9},
    {0x00000400, /* Port Macro10 (PM10) interrupt */
     SOC_BLK_XLPORT, XLPORT_INTR_ENABLEr, XLPORT_INTR_STATUSr, INVALIDf,
     _soc_monterey_xlport_ser_info, 10},
    {0x00000800, /* Port Macro11 (PM11) interrupt */
     SOC_BLK_CLPORT, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
     _soc_monterey_clport_ser_info, 1},
    {0x00001000, /* Port Macro12 (PM12) interrupt */
     SOC_BLK_CLPORT, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
     _soc_monterey_clport_ser_info, 4},
    {0x00002000, /* Port Macro13 (PM13) interrupt */
     SOC_BLK_CLPORT, CLPORT_INTR_ENABLEr, CLPORT_INTR_STATUSr, INVALIDf,
     _soc_monterey_clport_ser_info, 5},
    /* Bits 14-31 are reserved */
    {0} /* table terminator */
};

#ifdef INCLUDE_XFLOW_MACSEC
static const
_soc_monterey_ser_route_block_t  _soc_monterey_macsec_ser_route_blocks_irq4[] = {
    { 0x400000, /* MACSEC_INTR */
      SOC_BLK_MACSEC, SP_GLB_INTR_ENr, SP_GLB_INTR_STATUSr, INVALIDf,
      _soc_monterey_macsec_ser_info, 0 },
    {0} /* table terminator */
};
#endif

_soc_monterey_pm_cpri_intr_info_t
 _soc_monterey_pm_cpri_intr_info_irq4[] = {
    {0x00010000, /* Port Macro0 (PM0) CPRI interrupt */
     SOC_BLK_CPRI, CPRI_INTR_ENABLEr, CPRI_INTR_STATUSr, INVALIDf,
     NULL, 0},
    {0x00020000, /* Port Macro1 (PM1) CPRI interrupt */
     SOC_BLK_CPRI, CPRI_INTR_ENABLEr, CPRI_INTR_STATUSr, INVALIDf,
     NULL, 1},
    {0x00040000, /* Port Macro2 (PM2) CPRI interrupt */
     SOC_BLK_CPRI, CPRI_INTR_ENABLEr, CPRI_INTR_STATUSr, INVALIDf,
     NULL, 2},
    {0x00080000, /* Port Macro13 (PM13) CPRI interrupt */
     SOC_BLK_CPRI, CPRI_INTR_ENABLEr, CPRI_INTR_STATUSr, INVALIDf,
     NULL, 3},
    {0x00100000, /* Port Macro14 (PM14) CPRI interrupt */
     SOC_BLK_CPRI, CPRI_INTR_ENABLEr, CPRI_INTR_STATUSr, INVALIDf,
     NULL, 4},
    {0x00200000, /* Port Macro15 (PM15) interrupt */
     SOC_BLK_CPRI, CPRI_INTR_ENABLEr, CPRI_INTR_STATUSr, INVALIDf,
     NULL, 5},
     /* _soc_monterey_intr_info, 0}, */
    {0} /* table terminator */
};

static soc_ser_functions_t _monterey_ser_functions;
static soc_pm_intr_functions_t _monterey_pm_intr_functions;

extern uint8 _soc_alpm_mode[SOC_MAX_NUM_DEVICES];

static uint32 _monterey_ser_delay = 0;

/* Values map to HW encodings */
enum soc_mn_port_ca_mode_e {
    SOC_MN_PORT_CA_MODE_SINGLE      = 0,
    SOC_MN_PORT_CA_MODE_DUAL        = 1,
    SOC_MN_PORT_CA_MODE_QUAD        = 2,
    SOC_MN_PORT_CA_MODE_TRI_211     = 3,
    SOC_MN_PORT_CA_MODE_TRI_112     = 4,
    SOC_MN_PORT_CA_MODE_TRI_411     = 5,
    SOC_MN_PORT_CA_MODE_TRI_114     = 6,
    SOC_MN_PORT_CA_MODE_COUNT       /* unused */
};
int IS_ROE_ENABLED(int unit) {
    uint16              dev_id;
    uint8               rev_id;

    soc_cm_get_id(unit, &dev_id, &rev_id);
    if (dev_id == BCM56675_DEVICE_ID) {
        return FALSE;
    } else {
        return TRUE;
    }
}
STATIC int
_soc_monterey_thdo_hw_set(int unit, soc_port_t port, int enable);
/*
 * SINGLE   => {VALUE => 0, DESC => "Port 0 valid.  10G/20G/25G/40G/50G/100G."},
 * DUAL     => {VALUE => 1, DESC => "Port 0 and 2 valid.  10G/20G/25G/40G/50G."},
 * QUAD     => {VALUE => 2, DESC => "Port 0, 1, 2, and 3 valid.  10G/20G/25G."},
 * TRI211   => {VALUE => 3, DESC => "Port 0 is 20G, port 2 10G, and port 3 10G."},
 * TRI112   => {VALUE => 4, DESC => "Port 0 is 10G, port 1 10G, and port 2 20G."},
 * TRI411   => {VALUE => 5, DESC => "Port 0 is 40G, port 2 10G, and port 3 10G."},
 * TRI114   => {VALUE => 6, DESC => "Port 0 is 10G, port 1 10G, and port 2 40G."},
 */
STATIC int
soc_monterey_port_ca_mode_get(int unit, int flexport, int xlp, int *mode)
{
    soc_info_t *si;
    int port, phy_port_base, lane;
    int num_lanes[MONTEREY_PORTS_PER_TSC];
    int speed_max[MONTEREY_PORTS_PER_TSC];

    si = &SOC_INFO(unit);
    *mode = SOC_MN_PORT_CA_MODE_QUAD;

    phy_port_base = 1 + (xlp * MONTEREY_PORTS_PER_TSC);
    for (lane = 0; lane < MONTEREY_PORTS_PER_TSC; lane++) {
        port = si->port_p2l_mapping[phy_port_base + lane];
        if (port == -1 || SOC_PBMP_MEMBER(si->all.disabled_bitmap, port)) {
            num_lanes[lane] = -1;
            speed_max[lane] = -1;
        } else {
            num_lanes[lane] = si->port_num_lanes[port];
            speed_max[lane] = si->port_speed_max[port];
        }
    }

    if (num_lanes[0] >= 4) {
        *mode = SOC_MN_PORT_CA_MODE_SINGLE;
    } else if ((num_lanes[0] == 2) && (num_lanes[2] == 2)) {
        *mode = SOC_MN_PORT_CA_MODE_DUAL;
    } else if ((num_lanes[0] == 1) && (num_lanes[1] == 1) &&
               (num_lanes[2] == 1) && (num_lanes[3] == 1)) {
        *mode = SOC_MN_PORT_CA_MODE_QUAD;
    } else if (num_lanes[0] == 2) {
        *mode = (speed_max[0] >= 40000) ?
                SOC_MN_PORT_CA_MODE_TRI_411 : SOC_MN_PORT_CA_MODE_TRI_211;
    } else if (num_lanes[2] == 2) {
        *mode = (speed_max[2] >= 40000) ?
                SOC_MN_PORT_CA_MODE_TRI_114 : SOC_MN_PORT_CA_MODE_TRI_112;
    }
    return SOC_E_NONE;
}


int
_soc_monterey_idb_port_init(int unit, int reset, int lossless, soc_port_t port)
{
    soc_info_t *si;
    soc_mem_t mem;
    soc_reg_t reg;
    int obm, pgw, pgw_inst, index;
    int port_osub, phy_port, subport;
    uint32 rval32 ;
    uint64 rval64;
    idb_obm0_pri_map_port0_entry_t entry;

    uint32 discard_limit = 0, express_discard = 0, lossy_discard = 0, lossy_low_pri = 0;
    uint32 port_xon = 0, port_xoff = 0, lossless0_xon = 0 ,lossless0_xoff = 0;

    static const soc_reg_t obm_ctrl_regs[] = {
        IDB_OBM0_CONTROLr, IDB_OBM1_CONTROLr,
        IDB_OBM2_CONTROLr, IDB_OBM3_CONTROLr,
        IDB_OBM4_CONTROLr, IDB_OBM5_CONTROLr,
        IDB_OBM6_CONTROLr, IDB_OBM7_CONTROLr
    };
    static const soc_reg_t obm_thresh0_regs[] = {
        IDB_OBM0_THRESHOLD_0r, IDB_OBM1_THRESHOLD_0r,
        IDB_OBM2_THRESHOLD_0r, IDB_OBM3_THRESHOLD_0r,
        IDB_OBM4_THRESHOLD_0r, IDB_OBM5_THRESHOLD_0r,
        IDB_OBM6_THRESHOLD_0r, IDB_OBM7_THRESHOLD_0r
    };
    static const soc_reg_t obm_thresh1_regs[] = {
        IDB_OBM0_THRESHOLD_1r, IDB_OBM1_THRESHOLD_1r,
        IDB_OBM2_THRESHOLD_1r, IDB_OBM3_THRESHOLD_1r,
        IDB_OBM4_THRESHOLD_1r, IDB_OBM5_THRESHOLD_1r,
        IDB_OBM6_THRESHOLD_1r, IDB_OBM7_THRESHOLD_1r
    };
    static const soc_reg_t obm_fc_thresh0_regs[] = {
        IDB_OBM0_FC_THRESHOLD_0r, IDB_OBM1_FC_THRESHOLD_0r,
        IDB_OBM2_FC_THRESHOLD_0r, IDB_OBM3_FC_THRESHOLD_0r,
        IDB_OBM4_FC_THRESHOLD_0r, IDB_OBM5_FC_THRESHOLD_0r,
        IDB_OBM6_FC_THRESHOLD_0r, IDB_OBM7_FC_THRESHOLD_0r
    };
    static const soc_reg_t obm_fc_thresh1_regs[] = {
        IDB_OBM0_FC_THRESHOLD_1r, IDB_OBM1_FC_THRESHOLD_1r,
        IDB_OBM2_FC_THRESHOLD_1r, IDB_OBM3_FC_THRESHOLD_1r,
        IDB_OBM4_FC_THRESHOLD_1r, IDB_OBM5_FC_THRESHOLD_1r,
        IDB_OBM6_FC_THRESHOLD_1r, IDB_OBM7_FC_THRESHOLD_1r
    };
    static const soc_reg_t obm_shared_regs[] = {
        IDB_OBM0_SHARED_CONFIGr, IDB_OBM1_SHARED_CONFIGr,
        IDB_OBM2_SHARED_CONFIGr, IDB_OBM3_SHARED_CONFIGr,
        IDB_OBM4_SHARED_CONFIGr, IDB_OBM5_SHARED_CONFIGr,
        IDB_OBM6_SHARED_CONFIGr, IDB_OBM7_SHARED_CONFIGr
    };
    static const soc_reg_t obm_max_usage_regs[] = {
        IDB_OBM0_MAX_USAGE_SELECTr, IDB_OBM1_MAX_USAGE_SELECTr,
        IDB_OBM2_MAX_USAGE_SELECTr, IDB_OBM3_MAX_USAGE_SELECTr,
        IDB_OBM4_MAX_USAGE_SELECTr, IDB_OBM5_MAX_USAGE_SELECTr,
        IDB_OBM6_MAX_USAGE_SELECTr, IDB_OBM7_MAX_USAGE_SELECTr
    };

    static const soc_reg_t obm_port_config_regs[] = {
        IDB_OBM0_PORT_CONFIGr, IDB_OBM1_PORT_CONFIGr,
        IDB_OBM2_PORT_CONFIGr, IDB_OBM3_PORT_CONFIGr,
        IDB_OBM4_PORT_CONFIGr, IDB_OBM5_PORT_CONFIGr,
        IDB_OBM6_PORT_CONFIGr, IDB_OBM7_PORT_CONFIGr
    };
    static const soc_reg_t obm_fc_config_regs[] = {
        IDB_OBM0_FLOW_CONTROL_CONFIGr, IDB_OBM1_FLOW_CONTROL_CONFIGr,
        IDB_OBM2_FLOW_CONTROL_CONFIGr, IDB_OBM3_FLOW_CONTROL_CONFIGr,
        IDB_OBM4_FLOW_CONTROL_CONFIGr, IDB_OBM5_FLOW_CONTROL_CONFIGr,
        IDB_OBM6_FLOW_CONTROL_CONFIGr, IDB_OBM7_FLOW_CONTROL_CONFIGr
    };

    static const soc_reg_t obm_bank_config_regs[] = {
        IDB_OBM0_BANK_CONFIGr, IDB_OBM1_BANK_CONFIGr,
        IDB_OBM2_BANK_CONFIGr, IDB_OBM3_BANK_CONFIGr,
        IDB_OBM4_BANK_CONFIGr, IDB_OBM5_BANK_CONFIGr,
        IDB_OBM6_BANK_CONFIGr, IDB_OBM7_BANK_CONFIGr
    };

    static const soc_field_t obm_oversub_fields[] = {
        PORT0_OVERSUB_ENABLEf, PORT1_OVERSUB_ENABLEf,
        PORT2_OVERSUB_ENABLEf, PORT3_OVERSUB_ENABLEf
    };

    static const soc_mem_t obm_pri_map_port_mem[] = {
        IDB_OBM0_PRI_MAP_PORT0m, IDB_OBM0_PRI_MAP_PORT1m,
        IDB_OBM0_PRI_MAP_PORT2m, IDB_OBM0_PRI_MAP_PORT3m,
        IDB_OBM1_PRI_MAP_PORT0m, IDB_OBM1_PRI_MAP_PORT1m,
        IDB_OBM1_PRI_MAP_PORT2m, IDB_OBM1_PRI_MAP_PORT3m,
        IDB_OBM2_PRI_MAP_PORT0m, IDB_OBM2_PRI_MAP_PORT1m,
        IDB_OBM2_PRI_MAP_PORT2m, IDB_OBM2_PRI_MAP_PORT3m,
        IDB_OBM3_PRI_MAP_PORT0m, IDB_OBM3_PRI_MAP_PORT1m,
        IDB_OBM3_PRI_MAP_PORT2m, IDB_OBM3_PRI_MAP_PORT3m,
        IDB_OBM4_PRI_MAP_PORT0m, IDB_OBM4_PRI_MAP_PORT1m,
        IDB_OBM4_PRI_MAP_PORT2m, IDB_OBM4_PRI_MAP_PORT3m,
        IDB_OBM5_PRI_MAP_PORT0m, IDB_OBM5_PRI_MAP_PORT1m,
        IDB_OBM5_PRI_MAP_PORT2m, IDB_OBM5_PRI_MAP_PORT3m,
        IDB_OBM6_PRI_MAP_PORT0m, IDB_OBM6_PRI_MAP_PORT1m,
        IDB_OBM6_PRI_MAP_PORT2m, IDB_OBM6_PRI_MAP_PORT3m,
        IDB_OBM7_PRI_MAP_PORT0m, IDB_OBM7_PRI_MAP_PORT1m,
        IDB_OBM7_PRI_MAP_PORT2m, IDB_OBM7_PRI_MAP_PORT3m,
        IDB_OBM8_PRI_MAP_PORT0m, IDB_OBM8_PRI_MAP_PORT1m,
        IDB_OBM8_PRI_MAP_PORT2m, IDB_OBM8_PRI_MAP_PORT3m,
    };


    static const soc_field_t offset_ob_prio[] = {
        OFFSET0_OB_PRIORITYf, OFFSET1_OB_PRIORITYf, OFFSET2_OB_PRIORITYf,
        OFFSET3_OB_PRIORITYf, OFFSET4_OB_PRIORITYf, OFFSET5_OB_PRIORITYf,
        OFFSET6_OB_PRIORITYf, OFFSET7_OB_PRIORITYf, OFFSET8_OB_PRIORITYf,
        OFFSET9_OB_PRIORITYf, OFFSET10_OB_PRIORITYf, OFFSET11_OB_PRIORITYf,
        OFFSET12_OB_PRIORITYf, OFFSET13_OB_PRIORITYf, OFFSET14_OB_PRIORITYf,
        OFFSET15_OB_PRIORITYf,
    };
    static const soc_reg_t obm_ct_thresh_regs[] = {
        IDB_OBM0_CT_THRESHOLDr, IDB_OBM1_CT_THRESHOLDr,
        IDB_OBM2_CT_THRESHOLDr, IDB_OBM3_CT_THRESHOLDr,
        IDB_OBM4_CT_THRESHOLDr, IDB_OBM5_CT_THRESHOLDr,
        IDB_OBM6_CT_THRESHOLDr, IDB_OBM7_CT_THRESHOLDr,
        IDB_OBM8_CT_THRESHOLDr,
    };

    soc_pbmp_t pbmp_xport_roe_backplane;
    soc_pbmp_t pbmp_xport_roe_bbu;
    soc_pbmp_t pbmp_xport_roe_mcu;
    soc_pbmp_t pbmp_xport_roe_fronthaul;


    SOC_PBMP_CLEAR(pbmp_xport_roe_backplane);
    SOC_PBMP_CLEAR(pbmp_xport_roe_bbu);
    SOC_PBMP_CLEAR(pbmp_xport_roe_mcu);
    SOC_PBMP_CLEAR(pbmp_xport_roe_fronthaul);
    si = &SOC_INFO(unit);

    /* Calculation of obm and lanes(subport)
       obm is 0 for phy_port 1 to 4
       1 for phy port 5 to 8 and so on till 8 for phy_port 33 to 36
       Similarly obm 0 for phy_port 37 to 40 and so on
       lane(subport) is 0,1,2,3 for phy_port 1,2,3,4 resp
       lane(subport) is 0,1,2,3 for phy_port 5,6,7,8 resp and so on
       Block index is PGW0 for ports 1 to 36
       Block index is PGW1 for ports 37 to 72
    */
    phy_port = si->port_l2p_mapping[port];

    /*  Deleting a port  */
    if (phy_port == -1) {
        return SOC_E_PARAM;
    }

    if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port) || IS_MACSEC_PORT(unit, port)) {
        /*  nothing to do for non-idb ports */
        return SOC_E_NONE;
    }

    /* OBM group (num groups = num tsc's) belongs to PGW0/PGW1  */
    obm = si->port_serdes[port];
    obm %= MONTEREY_TSCS_PER_PGW;

    pgw = si->port_group[port];
    pgw_inst = (pgw | SOC_REG_ADDR_INSTANCE_MASK);
    subport = (phy_port - 1) % MONTEREY_PORTS_PER_TSC;
    pbmp_xport_roe_backplane = soc_property_get_pbmp_default(unit, spn_PBMP_XPORT_ROE_BACKPLANE,
                                 pbmp_xport_roe_backplane);
    pbmp_xport_roe_bbu = soc_property_get_pbmp_default(unit, spn_PBMP_XPORT_ROE_BBU,
                                 pbmp_xport_roe_bbu);
    pbmp_xport_roe_mcu = soc_property_get_pbmp_default(unit, spn_PBMP_XPORT_ROE_MCU,
                                 pbmp_xport_roe_mcu);
    pbmp_xport_roe_fronthaul = soc_property_get_pbmp_default(unit, spn_PBMP_XPORT_ROE_FRONTHAUL,
                                 pbmp_xport_roe_fronthaul);

    if(!IS_ROE_PORT(unit,port)) {

       COMPILER_64_ZERO(rval64);
       SOC_IF_ERROR_RETURN(READ_PGW_OBM_PARSING_CONTROLr(unit, port ,&rval64));
       soc_reg64_field32_set(unit, PGW_OBM_PARSING_CONTROLr,
                     &rval64, USE_PM_PREEMPT_INPUTf , 0);
       SOC_IF_ERROR_RETURN(WRITE_PGW_OBM_PARSING_CONTROLr(unit, port, rval64));
    }
    if (si->port_speed_max[port] <= 11000) {
            discard_limit = 375;
            if(IS_ROE_PORT(unit,port)) {
               discard_limit = 383;
               express_discard =  383;
            } else if(SOC_PBMP_MEMBER(pbmp_xport_roe_backplane, port)) {
               express_discard =  187;
            }
            else if(SOC_PBMP_MEMBER(pbmp_xport_roe_mcu, port)) {
               express_discard = 0;
            } else {
               express_discard = lossless ? 1535 : 375;
            }
            if(IS_ROE_PORT(unit,port)) {
               lossy_discard =  187;
            } else if(SOC_PBMP_MEMBER(pbmp_xport_roe_backplane, port)) {
               lossy_discard =  187;
            } else if(SOC_PBMP_MEMBER(pbmp_xport_roe_mcu, port)) {
               lossy_discard =  375;
            } else {
               lossy_discard = lossless ? 196 : 371;
            }
            if(IS_ROE_PORT(unit,port)) {
               lossy_low_pri =  187;
            } else if(SOC_PBMP_MEMBER(pbmp_xport_roe_backplane, port)) {
               lossy_low_pri =  165;
            } else if(SOC_PBMP_MEMBER(pbmp_xport_roe_mcu, port)) {
               lossy_low_pri =  268;
            } else {
               lossy_low_pri = lossless ? 100 : 282;
            }
            port_xon = lossless ? 223 : 1535;
            port_xoff = lossless ? 233 : 1535;
            lossless0_xon = lossless ? 36 : 1535;
            lossless0_xoff = lossless ? 46 : 1535;
    } else if (si->port_speed_max[port] <= 21000) {
               discard_limit = 383;
            if(IS_ROE_PORT(unit,port)) {
               express_discard =  383;
               discard_limit = 383;
            } else if(SOC_PBMP_MEMBER(pbmp_xport_roe_backplane, port)) {
               express_discard =  187;
            }
            else if(SOC_PBMP_MEMBER(pbmp_xport_roe_mcu, port)) {
               express_discard = 0;
            } else {
            express_discard = lossless ? 1535 : 375;
            }
            if(IS_ROE_PORT(unit,port)) {
               lossy_discard =  187;
            } else if(SOC_PBMP_MEMBER(pbmp_xport_roe_backplane, port)) {
               lossy_discard =  187;
            } else if(SOC_PBMP_MEMBER(pbmp_xport_roe_backplane, port)) {
               lossy_discard =  375;
            } else {
               lossy_discard = lossless ? 196 : 371;
            }
            if(IS_ROE_PORT(unit,port)) {
               lossy_low_pri =  0;
            } else if(SOC_PBMP_MEMBER(pbmp_xport_roe_backplane, port)) {
               lossy_low_pri =  165;
            } else if(SOC_PBMP_MEMBER(pbmp_xport_roe_mcu, port)) {
               lossy_low_pri =  268;
            } else {
               lossy_low_pri = lossless ? 100 : 282;
            }
            port_xon = lossless ? 206 : 1535;
            port_xoff = lossless ? 216 : 1535;
            lossless0_xon = lossless ? 36 : 1535;
            lossless0_xoff = lossless ? 46 : 1535;
    } else if (si->port_speed_max[port] <= 27000) {
            discard_limit = 375;
            if(IS_ROE_PORT(unit,port)) {
               express_discard =  383;
               discard_limit = 383;
            } else if(SOC_PBMP_MEMBER(pbmp_xport_roe_backplane, port)) {
               express_discard =  187;
            }
            else if(SOC_PBMP_MEMBER(pbmp_xport_roe_mcu, port)) {
               express_discard = 0;
            } else {
            express_discard = lossless ? 1535 : 375;
            }
            if(IS_ROE_PORT(unit,port)) {
               lossy_discard =  0;
            } else if(SOC_PBMP_MEMBER(pbmp_xport_roe_backplane, port)) {
               lossy_discard =  187;
            } else if(SOC_PBMP_MEMBER(pbmp_xport_roe_backplane, port)) {
               lossy_discard =  375;
            } else {
               lossy_discard = lossless ? 196 : 371;
            }
            if(IS_ROE_PORT(unit,port)) {
               lossy_low_pri =  0;
            } else if(SOC_PBMP_MEMBER(pbmp_xport_roe_backplane, port)) {
               lossy_low_pri =  165;
            } else if(SOC_PBMP_MEMBER(pbmp_xport_roe_mcu, port)) {
               lossy_low_pri =  268;
            } else {
               lossy_low_pri = lossless ? 100 : 282;
            }
            port_xon = lossless ? 198 : 1535;
            port_xoff = lossless ? 208 : 1535;
            lossless0_xon = lossless ? 36 : 1535;
            lossless0_xoff = lossless ? 46 : 1535;
    } else if (si->port_speed_max[port] <= 42000) {
            if((SOC_PBMP_MEMBER(pbmp_xport_roe_backplane, port)) ||
                (SOC_PBMP_MEMBER(pbmp_xport_roe_bbu, port))) {
               discard_limit =  1512 ;

            } else {
               discard_limit = 754;
            }
            express_discard = lossless ? 1535 : 756;
            if(SOC_PBMP_MEMBER(pbmp_xport_roe_mcu, port)) {
               express_discard = 0;
            }
            if(SOC_PBMP_MEMBER(pbmp_xport_roe_backplane, port)) {
               lossy_discard =  756;
            } else {
               lossy_discard = lossless ? 196 : 750;
            }
            if(SOC_PBMP_MEMBER(pbmp_xport_roe_backplane, port) ||
                SOC_PBMP_MEMBER(pbmp_xport_roe_bbu, port)) {
               lossy_low_pri = 670;
            } else {
               lossy_low_pri = lossless ? 100 : 661;
            }
            port_xon = lossless ? 499 : 1535;
            port_xoff = lossless ? 509 : 1535;
            lossless0_xon = lossless ? 98 : 1535;
            lossless0_xoff = lossless ? 108 : 1535;
    } else if (si->port_speed_max[port] <= 53000) {
            if(SOC_PBMP_MEMBER(pbmp_xport_roe_bbu, port)) {
               discard_limit =  754;
            } else {
               discard_limit = 1512;
            }
            if(SOC_PBMP_MEMBER(pbmp_xport_roe_bbu, port)) {
               express_discard = 377;
            }
            else if(SOC_PBMP_MEMBER(pbmp_xport_roe_mcu, port)) {
               express_discard = 0;
            } else {
               express_discard = 754;
            }
            if(SOC_PBMP_MEMBER(pbmp_xport_roe_bbu, port)) {
               lossy_discard = 377;
            } else {
               lossy_discard = lossless ? 196 : 750;
            }
            if(SOC_PBMP_MEMBER(pbmp_xport_roe_bbu, port)) {
               lossy_low_pri = 334;
            } else {
               lossy_low_pri = lossless ? 100 : 661;
            }
            port_xon = lossless ? 483 : 1535;
            port_xoff = lossless ? 493 : 1535;
            lossless0_xon = lossless ? 98 : 1535;
            lossless0_xoff = lossless ? 108 : 1535;
    } else { /* greter than 53000 including 106000 */
               discard_limit = 1512;
            if(SOC_PBMP_MEMBER(pbmp_xport_roe_fronthaul, port)) {
               express_discard =  756;
            }
            else if(SOC_PBMP_MEMBER(pbmp_xport_roe_mcu, port)) {
               express_discard = 0;
            } else {
               express_discard = lossless ? 1535 : 1512;
            }
            if(SOC_PBMP_MEMBER(pbmp_xport_roe_fronthaul, port)) {
               lossy_discard = 756;
            } else {
               lossy_discard = lossless ? 196 : 1508;
            }
            if(SOC_PBMP_MEMBER(pbmp_xport_roe_fronthaul, port)) {
               lossy_low_pri = 670;
            } else {
               lossy_low_pri = lossless ? 100 : 1419;
            }
            port_xon = lossless ? 483 : 1535;
            port_xoff = lossless ? 493 : 1535;
            lossless0_xon = lossless ? 248 : 1535;
            lossless0_xoff = lossless ? 258 : 1535;
    }

    if (reset) {
       discard_limit = 0;
       lossy_discard = 0;
       lossy_low_pri = 0;
       port_xon = 0;
       port_xoff = 0;
       lossless0_xon = 0;
       lossless0_xoff = 0;
    }


    rval32 = 0;
    reg = obm_port_config_regs[obm];

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, pgw_inst, subport, &rval32));
    soc_reg_field_set(unit, reg, &rval32, PORT_PRIf, 2);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, pgw_inst, subport, rval32));

    rval32 = 0;
    reg = obm_max_usage_regs[obm];

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, pgw_inst, 0, &rval32));
    soc_reg_field_set(unit, reg, &rval32, PRIORITY_SELECTf, 2);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, pgw_inst, 0, rval32));

    COMPILER_64_ZERO(rval64);
    reg = obm_thresh0_regs[obm];

    SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, pgw_inst, subport, &rval64));
    soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS1_DISCARDf, 1535);
    soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_DISCARDf, 1535);
    soc_reg64_field32_set(unit, reg, &rval64, LOSSY_DISCARDf, lossy_discard);
    soc_reg64_field32_set(unit, reg, &rval64, LOSSY_LOW_PRIf, lossy_low_pri);
    soc_reg64_field32_set(unit, reg, &rval64, LOSSY_MINf, 0);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, pgw_inst, subport, rval64));

    COMPILER_64_ZERO(rval64);
    reg = obm_thresh1_regs[obm];

    SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, pgw_inst, subport, &rval64));
    soc_reg64_field32_set(unit, reg, &rval64, DISCARD_LIMITf, discard_limit);
    soc_reg64_field32_set(unit, reg, &rval64, EXPRESS_DISCARDf, express_discard);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, pgw_inst, subport, rval64));

    rval32 = 0;
    reg = obm_shared_regs[obm];

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, pgw_inst, subport, &rval32));
    soc_reg_field_set(unit, reg, &rval32, SOP_DISCARD_MODEf,  1);
    soc_reg_field_set(unit, reg, &rval32, SOP_THRESHOLDf,  1535);
    soc_reg_field_set(unit, reg, &rval32, DISCARD_THRESHOLDf, 1535);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, pgw_inst, subport, rval32));

    COMPILER_64_ZERO(rval64);
    reg = obm_fc_thresh0_regs[obm];

    SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, pgw_inst, subport, &rval64));
    soc_reg64_field32_set(unit, reg, &rval64, PORT_XOFFf, port_xoff);
    soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS1_XONf, 1535);
    soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS1_XOFFf, 1535);
    soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_XONf, lossless0_xon);
    soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_XOFFf, lossless0_xoff);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, pgw_inst, subport, rval64));

    COMPILER_64_ZERO(rval64);
    reg = obm_fc_thresh1_regs[obm];

    SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, pgw_inst, subport, &rval64));
    soc_reg64_field32_set(unit, reg, &rval64, PORT_XONf, port_xon);
    soc_reg64_field32_set(unit, reg, &rval64, EXPRESS_XONf, 1535);
    soc_reg64_field32_set(unit, reg, &rval64, EXPRESS_XOFFf, 1535);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, pgw_inst, subport, rval64));

    if (!IS_OVERSUB_PORT(unit, port) ||
        ((IS_OVERSUB_PORT(unit, port)) &&
            ((si->port_speed_max[port] == 40000) ||
            (si->port_speed_max[port] == 42000)))) {
        COMPILER_64_ZERO(rval64);
        reg = obm_ct_thresh_regs[obm];

        SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, pgw_inst, subport, &rval64));
        soc_reg64_field32_set(unit, reg, &rval64, CUT_THROUGH_OKf, 2);
        SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, pgw_inst, subport, rval64));
    }

    COMPILER_64_ZERO(rval64);
    reg = obm_fc_config_regs[obm];

    SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, pgw_inst, subport, &rval64));
    soc_reg64_field32_set(unit, reg, &rval64, XOFF_REFRESH_TIMERf, 256);
    soc_reg64_field32_set(unit, reg, &rval64, PORT_FC_ENf, lossless ? 1 : 0);
    soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_FC_ENf, lossless ? 1 : 0);
    soc_reg64_field32_set(unit, reg, &rval64, LOSSLESS0_PRIORITY_PROFILEf,
            lossless ? 65535 : 0);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, pgw_inst, subport, rval64));

    COMPILER_64_ZERO(rval64);
    reg = obm_bank_config_regs[obm];

    SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, pgw_inst, subport, &rval64));
    soc_reg64_field32_set(unit, reg, &rval64, BANK_DISCARD_THRESHOLDf, 760);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, pgw_inst, subport, rval64));

    rval32 = 0;
    reg =  obm_ctrl_regs[obm];
    port_osub = 1;

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, pgw_inst, 0, &rval32));
    soc_reg_field_set(unit, reg, &rval32, obm_oversub_fields[subport], port_osub);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, pgw_inst, 0, rval32));

    if (lossless) {
        mem = obm_pri_map_port_mem[(phy_port - 1) % MONTEREY_PORTS_PER_PGW];
        sal_memset(&entry, 0, sizeof(entry));
        SOC_IF_ERROR_RETURN(soc_monterey_shadow_entry_get(unit, mem, pgw, &entry));
        for (index = 0; index < COUNTOF(offset_ob_prio); index++) {
            soc_mem_field32_set(unit, mem, &entry, offset_ob_prio[index], 2);
        }
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, PGW_CL_BLOCK(unit, pgw), 0, &entry));
        SOC_IF_ERROR_RETURN(soc_monterey_shadow_entry_set(unit, mem, pgw, &entry));
    } else if (IS_ROE_ENABLED(unit)) {
        mem = obm_pri_map_port_mem[(phy_port - 1) % MONTEREY_PORTS_PER_PGW];
        sal_memset(&entry, 0, sizeof(entry));
        SOC_IF_ERROR_RETURN(soc_monterey_shadow_entry_get(unit, mem, pgw, &entry));

        for (index = 0; index < COUNTOF(offset_ob_prio); index++) {
             if (IS_ROE_PORT(unit,port)) {
                          soc_mem_field32_set(unit, mem, &entry,
                                              offset_ob_prio[index], 4);
             } else {
                 if (index == 3) {
                     soc_mem_field32_set(unit, mem, &entry,
                                         offset_ob_prio[index], 1);
                 } else {
                     if (!SOC_PBMP_MEMBER(pbmp_xport_roe_mcu, port)) {
                         if ( index == 5 || index == 7) {
                              soc_mem_field32_set(unit, mem, &entry,
                              offset_ob_prio[index], 4);
                         }
                     }
                 }
             }
        }
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, PGW_CL_BLOCK(unit, pgw), 0, &entry));
        SOC_IF_ERROR_RETURN(soc_monterey_shadow_entry_set(unit, mem, pgw, &entry));
    }

    return SOC_E_NONE;
}
STATIC int
_soc_monterey_port_blk_ca_mode_set(int unit, int flexport, int xlp)
{
    int rv;
    soc_reg_t reg;
    int ca_port_mode = 0;
    int pgw_inst;
    uint32 r32val0, r32val1;

    static const soc_reg_t obm_ca_ctrl_regs[] = {
        IDB_OBM0_CA_CONTROLr, IDB_OBM1_CA_CONTROLr, IDB_OBM2_CA_CONTROLr,
        IDB_OBM3_CA_CONTROLr, IDB_OBM4_CA_CONTROLr, IDB_OBM5_CA_CONTROLr,
        IDB_OBM6_CA_CONTROLr, IDB_OBM7_CA_CONTROLr,
    };

    /* Calculate the mode of the Cell Assembly for the port block */
    rv = soc_monterey_port_ca_mode_get(unit, flexport, xlp, &ca_port_mode);
    SOC_IF_ERROR_RETURN(rv);

    r32val0 = 0;
    reg = obm_ca_ctrl_regs[xlp % MONTEREY_TSCS_PER_PGW];
    pgw_inst = ((xlp / MONTEREY_TSCS_PER_PGW) | SOC_REG_ADDR_INSTANCE_MASK);
    /* Debug output */
    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "Port Block CellAssembly Mode: "
                            "xlp=%d  pgw=%d reg=%s ca_mode=%d \n"),
                 xlp, (xlp / MONTEREY_TSCS_PER_PGW), SOC_REG_NAME(unit, reg), ca_port_mode));


    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, pgw_inst, 0, &r32val0));
    soc_reg_field_set(unit, reg, &r32val0, PORT_MODEf, ca_port_mode);
    r32val1 = r32val0;
    if (!flexport) {
        soc_reg_field_set(unit, reg, &r32val0, PORT0_RESETf, 1);
        soc_reg_field_set(unit, reg, &r32val0, PORT1_RESETf, 1);
        soc_reg_field_set(unit, reg, &r32val0, PORT2_RESETf, 1);
        soc_reg_field_set(unit, reg, &r32val0, PORT3_RESETf, 1);

        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, pgw_inst, 0, r32val0));
    }
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, pgw_inst, 0, r32val1));

    return SOC_E_NONE;
}

int
soc_monterey_idb_init(int unit)
{
    int rv;
    int lossless;
    soc_port_t port;
    int xlp, blk_index;
    soc_block_t port_blocks[] = {SOC_BLK_XLPORT,
                                 SOC_BLK_CLPORT,
                                 SOC_BLOCK_TYPE_INVALID};

    lossless = soc_property_get(unit, spn_MMU_LOSSLESS, 0);
    /* Per-port settings */
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(_soc_monterey_idb_port_init(unit, FALSE, lossless, port));
    }

    SOC_BLOCKS_ITER(unit, blk_index, port_blocks) {
        port = SOC_BLOCK_PORT(unit, blk_index);
        xlp = SOC_INFO(unit).port_serdes[port];
        rv = _soc_monterey_port_blk_ca_mode_set(unit, FALSE, xlp);
        SOC_IF_ERROR_RETURN(rv);
    }

    return SOC_E_NONE;
}


STATIC int
soc_mn_mmu_delay_insertion_set (int unit, int port)
{
    int phy_port, mmu_port;
    int val = 0;
    uint32 rval;
    soc_info_t *si;

    si = &SOC_INFO(unit);

    phy_port = si->port_l2p_mapping[port];
    mmu_port = si->port_p2m_mapping[phy_port];


    SOC_IF_ERROR_RETURN(soc_reg32_get(unit,
                ES_PIPE0_MMU_3DBG_Cr, REG_PORT_ANY, mmu_port, &rval));

    if (IS_OVERSUB_PORT(unit, port ) && si->port_speed_max[port] >= 20000) {
        val = 120 + (sal_rand() % 20);
    } else {
        /* linerate port or OS with < 20G */
    }
    soc_reg_field_set(unit, ES_PIPE0_MMU_3DBG_Cr, &rval, FIELD_Af, val);

    SOC_IF_ERROR_RETURN(soc_reg32_set(unit,
                ES_PIPE0_MMU_3DBG_Cr, REG_PORT_ANY, mmu_port, rval));

    return SOC_E_NONE;
}


STATIC int
_soc_monterey_ser_enable_info(int unit, int block_info_idx, int inst, int port,
                            soc_reg_t group_reg, uint64 *group_rval,
                            const _soc_monterey_ser_info_t *info_list,
                            soc_mem_t mem, int enable);

STATIC int
_soc_monterey_ser_enable_mmu_1bit_ecc (int unit, int enable)
{
    int i, f;
    uint64 rval64;
    uint32 rval;
    soc_field_info_t *fieldp;
    soc_reg_info_t   *regp;
    soc_reg_t reg;

    soc_reg_t _1b_reg_list[] = {
        CFAP_EN_COR_ERR_RPTr,
        MMU_DEQ_EN_COR_ERR_RPTr,
        MMU_ENQ_EN_COR_ERR_RPTr,
        MMU_EPRG_EN_COR_ERR_RPTr,
        INTFI_EN_COR_ERR_RPTr,
        MMU_PQE_EN_COR_ERR_RPTr,
        RQE_EN_COR_ERR_RPTr,
        MMU_TDM_EN_COR_ERR_RPTr,
        THDI_EN_COR_ERR_RPTr,
        MMU_THDM_DB_EN_COR_ERR_RPTr,
        MMU_THDM_MCQE_EN_COR_ERR_RPTr,
        MMU_THDU_EN_COR_ERR_RPTr,
        MMU_TOQ_EN_COR_ERR_RPTr,
        HSP_EN_COR_ERR_RPTr
    };
    for (i = 0; i < COUNTOF(_1b_reg_list); i++) {
        reg = _1b_reg_list[i];
        regp = &(SOC_REG_INFO(unit, reg));
        if (SOC_REG_IS_64(unit, reg)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_get(unit, reg, REG_PORT_ANY, 0, &rval64));
            for (f = 0; f < regp->nFields; f++) {
                fieldp = &(regp->fields[f]);
                soc_reg64_field32_set(unit, reg, &rval64, fieldp->field,
                                      enable ? 1 : 0);
            }
            SOC_IF_ERROR_RETURN
                (soc_reg_set(unit, reg, REG_PORT_ANY, 0, rval64));
        } else {
            SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
            for (f = 0; f < regp->nFields; f++) {
                fieldp = &(regp->fields[f]);
                soc_reg_field_set(unit, reg, &rval, fieldp->field,
                                  enable ? 1 : 0);
            }
            SOC_IF_ERROR_RETURN
                (soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
        }
    }

    return SOC_E_NONE;
}


#ifdef INCLUDE_XFLOW_MACSEC
STATIC int
_soc_monterey_macsec_ser_enable (int unit, int enable)
{
#if  defined(BCM_56670_B0) || defined(BCM_56670_C0)
    int i;
    uint32 rval;
    soc_reg_t reg;
    soc_field_t field;
    _soc_mem_ser_en_info_t * macsec_mem_ser_info = _soc_bcm56670_b0_macsec_mem_ser_info;

    for (i = 0; macsec_mem_ser_info[i].mem != INVALIDm; i++) {
        reg = macsec_mem_ser_info[i].en_ctrl.ctrl_type.en_reg;
        field = macsec_mem_ser_info[i].en_ctrl.en_fld;
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        soc_reg_field_set(unit, reg, &rval, field,
                enable ? 0 : 1);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
        LOG_VERBOSE(BSL_LS_SOC_SOCMEM, (BSL_META_U(unit,
                        "SER enable for: %s\n"),
                    SOC_MEM_NAME(unit, macsec_mem_ser_info[i].mem)));

    }
#endif
    return SOC_E_NONE;
}
#endif

STATIC int
_soc_monterey_ser_enable_info(int unit, int block_info_idx, int inst, int port,
                            soc_reg_t group_reg, uint64 *group_rval,
                            const _soc_monterey_ser_info_t *info_list,
                            soc_mem_t mem, int enable)
{
    const _soc_monterey_ser_info_t *info;
    int info_index, rv, rv1;
    soc_reg_t reg;
    uint32 rval;
    uint64 rval64, mac_err_mask;
#ifdef INCLUDE_XFLOW_MACSEC
    uint8 parity_enable = 0;
#endif

    rv = SOC_E_NOT_FOUND;

    /* Loop through each info entry in the list */
    for (info_index = 0; ; info_index++) {
        info = &info_list[info_index];
        if (info->type == _SOC_PARITY_TYPE_NONE) {
            /* End of table */
            break;
        }

        if (mem != INVALIDm && info->mem != mem) {
            continue;
        }

        rv = SOC_E_NONE;

        /* Enable the info entry in the group register */
        if (info->group_reg_enable_field != INVALIDf) {
            soc_reg64_field32_set(unit, group_reg, group_rval,
                                  info->group_reg_enable_field, enable ? 1 : 0);
        }

        /* Handle different parity error reporting style */
        switch (info->type) {
        case _SOC_PARITY_TYPE_MAC_TX_CDC:
        case _SOC_PARITY_TYPE_MAC_RX_CDC:
        case _SOC_PARITY_TYPE_MAC_RX_TS:
            COMPILER_64_SET(mac_err_mask, 0, 0x7800);
            if (info->group_reg_enable_field == INVALIDf) {
                COMPILER_64_OR(*group_rval, mac_err_mask);
            }
            /* fall through */
        case _SOC_PARITY_TYPE_ECC:
        case _SOC_PARITY_TYPE_PARITY:
            reg = info->enable_reg;
            if (!SOC_REG_IS_VALID(unit, reg)) {
                break;
            }
            if (SOC_REG_IS_64(unit, reg)) {
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, reg, port, 0, &rval64));
                soc_reg64_field32_set(unit, reg, &rval64, info->enable_field,
                                      enable ? 1 : 0);
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, reg, port, 0, rval64));
            } else {
                SOC_IF_ERROR_RETURN
                    (soc_reg32_get(unit, reg, port, 0, &rval));
                soc_reg_field_set(unit, reg, &rval, info->enable_field,
                                  enable ? 1 : 0);
                SOC_IF_ERROR_RETURN
                    (soc_reg32_set(unit, reg, port, 0, rval));
            }
            LOG_VERBOSE(BSL_LS_SOC_SOCMEM, (BSL_META_U(unit,
                        "SER enable for: %s\n"),
                        (info->mem == INVALIDm) ? info->mem_str :
                        SOC_MEM_NAME(unit, info->mem)));
            break;
        case _SOC_PARITY_TYPE_MMU_SER:
            rv1 = _soc_monterey_ser_enable_mmu_1bit_ecc(unit, enable);
            if (SOC_FAILURE(rv1) && rv1 != SOC_E_NOT_FOUND) {
                return rv;
            }
            LOG_VERBOSE(BSL_LS_SOC_SOCMEM, (BSL_META_U(unit,
                        "SER enable for: %s\n"),
                        (info->mem == INVALIDm) ? info->mem_str :
                        SOC_MEM_NAME(unit, info->mem)));
            break;
#ifdef INCLUDE_XFLOW_MACSEC
        case _SOC_PARITY_TYPE_MACSEC_SER:
            parity_enable = soc_property_get(unit, spn_PARITY_ENABLE, TRUE);
            if (info->group_reg_enable_field != INVALIDf) {
                soc_reg64_field32_set(unit, group_reg, group_rval,
                                      info->group_reg_enable_field, (enable & parity_enable) ? 1 : 0);
            }
            rv1 = _soc_monterey_macsec_ser_enable(unit, (enable & parity_enable) ? 1 : 0);
            if (SOC_FAILURE(rv1) && rv1 != SOC_E_NOT_FOUND) {
                return rv;
            }
            break;
#endif
        case _SOC_PARITY_TYPE_GENERIC:
        case _SOC_PARITY_TYPE_START_ERR:
        case _SOC_PARITY_TYPE_BST:
            LOG_VERBOSE(BSL_LS_SOC_SOCMEM, (BSL_META_U(unit,
                        "SER enable for: %s\n"),
                        (info->mem == INVALIDm) ? info->mem_str :
                        SOC_MEM_NAME(unit, info->mem)));
        default:
            break;
        } /* Handle different parity error reporting style */
        if (mem != INVALIDm) {
            break;
        }

    } /* Loop through each info entry in the route block */

    return rv;
}

STATIC int
_soc_monterey_pm_cpri_intr_enable(int unit, int enable)
{
    uint32    cmic_rval = 0, cmic_bit = 0;
    int       rbi;
    const _soc_monterey_pm_cpri_intr_info_t *rb;

    SOC_IF_ERROR_RETURN(READ_CMIC_CMC0_PCIE_IRQ_MASK4r(unit, &cmic_rval));

    for (rbi = 0; ; rbi++) {
         rb = &_soc_monterey_pm_cpri_intr_info_irq4[rbi];
        cmic_bit = rb->cmic_bit;
        if (cmic_bit == 0) {
            /* End of table */
            break;
        }

        if (enable) {
            cmic_rval |= cmic_bit;
        }

    } /* end of rbi for */

    if (enable) {
        /* Write CMIC enable register */
        (void)soc_cmicm_intr4_enable(unit, cmic_rval);
    } else {
        /* Write CMIC disable register */
        (void)soc_cmicm_intr4_disable(unit, cmic_rval);
    }

    return SOC_E_NONE;
}
#ifdef INCLUDE_XFLOW_MACSEC
STATIC int
_soc_monterey_macsec_intr_enable (int unit, int enable)
{
    uint32    cmic_rval = 0, cmic_bit = 0;
    uint64    rval64;
    int       rbi, rv = SOC_E_NONE;
    const _soc_monterey_ser_route_block_t   *rb;

    SOC_IF_ERROR_RETURN(READ_CMIC_CMC0_PCIE_IRQ_MASK4r(unit, &cmic_rval));

    for (rbi = 0; ; rbi++) {
         rb = &_soc_monterey_macsec_ser_route_blocks_irq4[rbi];
        cmic_bit = rb->cmic_bit;
        if (cmic_bit == 0) {
            /* End of table */
            break;
        }
        if (rb->info == NULL) {
            continue;
        }
        if (enable) {
            cmic_rval |= cmic_bit;
        }

        if (rb->enable_reg != INVALIDr) {
            SOC_IF_ERROR_RETURN
                (soc_reg_get(unit, rb->enable_reg, REG_PORT_ANY, 0, &rval64));

            rv = _soc_monterey_ser_enable_info(unit, 0, rb->id, REG_PORT_ANY,
                                         rb->enable_reg, &rval64,
                                         rb->info, INVALIDm, enable);
        }
        if (rv == SOC_E_NOT_FOUND) {
            continue;
        } else if (SOC_FAILURE(rv)) {
            return rv;
        }

        if (rb->enable_reg != INVALIDr) {
            /* Write per route block parity enable register */
            SOC_IF_ERROR_RETURN
                (soc_reg_set(unit, rb->enable_reg, REG_PORT_ANY, 0, rval64));
        }
    } /* end of rbi for */

    if (enable) {
        /* Write CMIC enable register */
        (void)soc_cmicm_intr4_enable(unit, cmic_rval);
    } else {
        /* Write CMIC disable register */
        (void)soc_cmicm_intr4_disable(unit, cmic_rval);
    }

    return SOC_E_NONE;
}
#endif

STATIC int
_soc_monterey_ser_enable_pm (int unit, int enable)
{
    uint32    cmic_rval = 0, cmic_bit = 0;
    uint64    rval64;
    int       rbi, block_info_idx, port, rv = SOC_E_NONE;
    const _soc_monterey_ser_route_block_t   *rb;

    SOC_IF_ERROR_RETURN(READ_CMIC_CMC0_PCIE_IRQ_MASK4r(unit, &cmic_rval));

    for (rbi = 0; ; rbi++) {
         rb = &_soc_monterey_ser_route_blocks_irq4[rbi];
        cmic_bit = rb->cmic_bit;
        if (cmic_bit == 0) {
            /* End of table */
            break;
        }
        if (rb->info == NULL) {
            continue;
        }
        if (enable) {
            cmic_rval |= cmic_bit;
        }
        port = INVALID_PORT;
        SOC_BLOCK_ITER(unit, block_info_idx, rb->blocktype) {
            if (SOC_BLOCK_INFO(unit, block_info_idx).number == rb->id) {
                port = SOC_BLOCK_PORT(unit, block_info_idx);
                break;
            }
        }

        if (port == INVALID_PORT) {
            continue;
        }

        if (rb->enable_reg != INVALIDr) {
            if (SOC_BLOCK_IN_LIST(unit, SOC_REG_INFO(unit, rb->enable_reg).block,
                                  SOC_BLK_PORT) && (port == REG_PORT_ANY)) {
                    /* This port block is not configured */
                    continue;
            }
            SOC_IF_ERROR_RETURN
                (soc_reg_get(unit, rb->enable_reg, port, 0, &rval64));

            rv = _soc_monterey_ser_enable_info(unit, block_info_idx, rb->id, port,
                                         rb->enable_reg, &rval64,
                                         rb->info, INVALIDm, enable);
        }
        if (rv == SOC_E_NOT_FOUND) {
            continue;
        } else if (SOC_FAILURE(rv)) {
            return rv;
        }

        if (rb->enable_reg != INVALIDr) {
            /* Write per route block parity enable register */
            SOC_IF_ERROR_RETURN
                (soc_reg_set(unit, rb->enable_reg, port, 0, rval64));
        }
    } /* end of rbi for */

    if (enable) {
        /* Write CMIC enable register */
        (void)soc_cmicm_intr4_enable(unit, cmic_rval);
    } else {
        /* Write CMIC disable register */
        (void)soc_cmicm_intr4_disable(unit, cmic_rval);
    }

    return SOC_E_NONE;
}

STATIC int
_soc_monterey_l3_mem_sram_info_get(int unit, int *base, int *count,
                                 soc_mem_t *view,
                                 int mem_index[][_SOC_SER_MAX_ENTRIES_PER_BKT])
{
    int i, j, base_index;
    int copyno;
    uint8 *vmap;
    uint32 *cache;
    struct mem_list_s {
        soc_mem_t mem;         /* view name */
        int       ram_count;   /* Num of entries in the bucket */
        int       entry_count; /* Num of writes to correct LP memory */
    };
    struct mem_list_s mem_list[] = {
        /*
         * If the hash bucket corresponding to the error LP index has
         * all single wide entries, it is sufficient to write any of the one
         * entry to hardware and the LP memory would be fixed in HW.
         * For double wide entries also writing one entry would be sufficient
         * to clear the error on the LP entry but for some keys we need to write
         * all the entries (2) in the bucket for the LP memory to be fixed.
         * For quad wide, anyway a single entry correction would fix the LP memory
         * error because the QW entry touches all the physical entries in
         * the bucket.
         */
        {L3_ENTRY_IPV4_UNICASTm,   4, 1},
        {L3_ENTRY_IPV4_MULTICASTm, 2, 2},
        {L3_ENTRY_IPV6_UNICASTm,   2, 2},
        {L3_ENTRY_IPV6_MULTICASTm, 1, 1}
    };
    view[0] = view[1] = INVALIDm;
    for (i = 0; i < COUNTOF(mem_list); i++) {
        copyno = SOC_MEM_BLOCK_ANY(unit, mem_list[i].mem);
        cache = SOC_MEM_STATE(unit, mem_list[i].mem).cache[copyno];
        vmap = SOC_MEM_STATE(unit, mem_list[i].mem).vmap[copyno];
        if (mem_list[i].mem == L3_ENTRY_IPV4_UNICASTm) {
            /*
             * Each LP memory index corresponds to 4 single wide entries.
             * The ISS (shared) region will start after the dedicated region.
             */
            base_index = (soc_mem_index_count(unit, L3_ENTRY_LPm) * 4)
                         + (*base * 4);
        } else if ((mem_list[i].mem == L3_ENTRY_IPV4_MULTICASTm) ||
                   (mem_list[i].mem == L3_ENTRY_IPV6_UNICASTm)) {
            /*
             * Each LP memory index corresponds to 2 double wide entries.
             * The ISS (shared) region will start after the dedicated region.
             */
            base_index = (soc_mem_index_count(unit, L3_ENTRY_LPm) * 2)
                         + (*base * 2);
        } else {
            /*
             * Each LP memory index corresponds to 1 quad wide entry.
             * The ISS (shared) region will start after the dedicated region.
             */
            base_index = soc_mem_index_count(unit, L3_ENTRY_LPm) + *base;
        }

        for (j = 0; j < mem_list[i].ram_count; j++) {
            if ((cache != NULL) && (vmap != NULL) &&
                (CACHE_VMAP_TST(vmap, base_index+j))) {
                if ((mem_list[i].mem == L3_ENTRY_IPV4_UNICASTm) ||
                    (mem_list[i].mem == L3_ENTRY_IPV6_MULTICASTm)) {
                    view[0] = mem_list[i].mem;
                    *count = mem_list[i].entry_count;
                    mem_index[0][0] = base_index;
                } else {
                    view[j] = mem_list[i].mem;
                    *count = mem_list[i].entry_count;
                    mem_index[j][0] = base_index+j;
                }
            }
        }
        if (*count == 1) {
            break;
        }
    }
    /*
     * For double wide views, if only one entry is valid in a given bucket,
     * the other entry in the same bucket also needs to be written (any double
     * wide view is fine because we will be writing null-entry anyway).
     * Using the same view as its neighbor here. The index for the view is
     * either +1 or -1 based on the original base_index. E.g., if the base_index
     * is 2048, the correction indexes would be 2048 (with the orig entry data)
     * and 2049 (with null-entry). If the base_index is 2049, the correction
     * indexes would be 2049 (with the orig entry data) and 2048 (with
     * null-entry).
     */
    if (*count == 2) {
        if ((view[0] == INVALIDm) && (view[1] != INVALIDm)) {
            view[0] = view[1];
            mem_index[0][0] = mem_index[1][0] - 1;
        } else if ((view[1] == INVALIDm) && (view[0] != INVALIDm)) {
            view[1] = view[0];
            mem_index[1][0] = mem_index[0][0] + 1;
        }
    }
    if (!(*count)) {
        /* Default view as L3_ENTRY_IPV4_UNICASTm */
        view[0] = mem_list[0].mem;
        *count = mem_list[0].entry_count;
        mem_index[0][0] = (soc_mem_index_count(unit, L3_ENTRY_LPm) * 4)
                          + (*base * 4);
    }
    return SOC_E_NONE;
}

STATIC int
_soc_monterey_ser_enable_all (int unit, int enable)
{
    uint8                                 rbi, bcount, type;
    uint16                                pcount;
    uint32                                cmic_rval;
    uint32                                rval, cmic_bit;
    int                                   port = REG_PORT_ANY;
    int                                   rv, block_info_idx;
    uint64                                rval64;
    soc_reg_t                             reg, ecc1b_reg;
    soc_field_t                           field, ecc1b_field;
    _soc_reg_ser_en_info_t                *reg_info;
    _soc_mem_ser_en_info_t                *mem_info;
    _soc_bus_ser_en_info_t                *bus_info;
    _soc_buffer_ser_en_info_t             *buf_info;
    char                                  *str_type;
    char                                  *str_name;
    const _soc_monterey_ser_route_block_t   *rb;
    const _soc_monterey_ser_block_info_t    *ser_block_info;
    static char *parity_module_str[4]     = {"REG", "MEM", "BUS", "BUF"};
    int done = 0;

    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(READ_CMIC_CMC0_PCIE_IRQ_MASK3r(unit, &cmic_rval));

    /* Enable new fifo mechanism based SER stuff */
    ser_block_info = _soc_monterey_ser_block_info;
    for (bcount = 0; ser_block_info[bcount].blocktype; bcount++) {
        done = 0;
        if (SOC_BLK_MACSEC == ser_block_info[bcount].blocktype) {
            continue;
        }

        LOG_VERBOSE(BSL_LS_SOC_SOCMEM,
                   (BSL_META_U(unit,
                               "Unit %d SER enable for: %s\n"),
                                    unit, ser_block_info[bcount].name));

        type = ser_block_info[bcount].type;
        reg_info = (_soc_reg_ser_en_info_t*)ser_block_info[bcount].info;
        mem_info = (_soc_mem_ser_en_info_t*)ser_block_info[bcount].info;
        bus_info = (_soc_bus_ser_en_info_t*)ser_block_info[bcount].info;
        buf_info = (_soc_buffer_ser_en_info_t*)ser_block_info[bcount].info;
        for (pcount = 0; ; pcount++) {
            ecc1b_reg = INVALIDr;
            ecc1b_field = INVALIDf;
            switch (type) {
            case _SOC_MONTEREY_SER_TYPE_REG:
                if (reg_info[pcount].reg == INVALIDr) {
                    done = 1;
                } else {
                    reg = reg_info[pcount].en_ctrl.ctrl_type.en_reg;
                    field = reg_info[pcount].en_ctrl.en_fld;
                    str_name = SOC_REG_NAME(unit, reg_info[pcount].reg);
                }
                break;
            case _SOC_MONTEREY_SER_TYPE_MEM:
                if (mem_info[pcount].mem == INVALIDm) {
                    done = 1;
                } else {
                    if (soc_feature(unit, soc_feature_parity_injection_l2)) {
                        if ((mem_info[pcount].mem == L2_USER_ENTRYm) ||
                            (mem_info[pcount].mem == L2_USER_ENTRY_DATA_ONLYm)){
                            continue;
                        }
                    }
                    if (SOC_MEM_INFO(unit, mem_info[pcount].mem).flags &
                        SOC_MEM_FLAG_SER_PARITY_ENABLE_SKIP) {
                        continue;
                    }
                    reg = mem_info[pcount].en_ctrl.ctrl_type.en_reg;
                    field = mem_info[pcount].en_ctrl.en_fld;
                    ecc1b_reg = mem_info[pcount].ecc1b_ctrl.ctrl_type.en_reg;
                    ecc1b_field = mem_info[pcount].ecc1b_ctrl.en_fld;
                    str_name = SOC_MEM_NAME(unit, mem_info[pcount].mem);
                }
                break;
            case _SOC_MONTEREY_SER_TYPE_BUS:
                if (bus_info[pcount].en_reg == INVALIDr) {
                    done = 1;
                } else {
                    reg = bus_info[pcount].en_reg;
                    field = bus_info[pcount].en_fld;
                    str_name = bus_info[pcount].bus_name;
                }
                break;
            case _SOC_MONTEREY_SER_TYPE_BUF:
                if (buf_info[pcount].en_reg == INVALIDr) {
                    done = 1;
                } else {
                    reg = buf_info[pcount].en_reg;
                    field = buf_info[pcount].en_fld;
                    str_name = buf_info[pcount].buffer_name;
                }
                break;
            default:
                LOG_ERROR(BSL_LS_SOC_MEM,
                          (BSL_META_U(unit,
                                      "Unknown parity module [bcount: %d][pcount: %d].\n"),
                                       bcount, pcount));
                return SOC_E_INTERNAL;
            }
            if (done) {
                break;
            } else {
                str_type = parity_module_str[type];
            }
            /* NOTE: Do not use the field modify routine in the following as
                     some regs do not return the correct value due to which the
                     modify routine skips the write */
            if (SOC_REG_IS_64(unit, reg)) {
                SOC_IF_ERROR_RETURN
                    (soc_reg_get(unit, reg, port, 0, &rval64));
                soc_reg64_field32_set(unit, reg, &rval64, field, enable ? 1 : 0);
                SOC_IF_ERROR_RETURN
                    (soc_reg_set(unit, reg, port, 0, rval64));
            } else {
                SOC_IF_ERROR_RETURN
                    (soc_reg32_get(unit, reg, port, 0, &rval));
                soc_reg_field_set(unit, reg, &rval, field, enable ? 1 : 0);
                SOC_IF_ERROR_RETURN
                    (soc_reg32_set(unit, reg, port, 0, rval));
            }
            if (ecc1b_reg != INVALIDr && ecc1b_field != INVALIDf) {
                if (SOC_REG_IS_64(unit, ecc1b_reg)) {
                    SOC_IF_ERROR_RETURN
                        (soc_reg_get(unit, ecc1b_reg, port, 0, &rval64));
                    soc_reg64_field32_set(unit, ecc1b_reg, &rval64, ecc1b_field, enable ? 1 : 0);
                    SOC_IF_ERROR_RETURN
                        (soc_reg_set(unit, ecc1b_reg, port, 0, rval64));
                } else {
                    SOC_IF_ERROR_RETURN
                        (soc_reg32_get(unit, ecc1b_reg, port, 0, &rval));
                    soc_reg_field_set(unit, ecc1b_reg, &rval, ecc1b_field, enable ? 1 : 0);
                    if (20 == SOC_REG_ACC_TYPE(unit, ecc1b_reg)) {
                        SOC_IF_ERROR_RETURN(
                            soc_reg32_set(unit, ecc1b_reg, \
                                          SOC_REG_ADDR_INSTANCE_MASK, 0, rval));
                    } else {
                        SOC_IF_ERROR_RETURN
                            (soc_reg32_set(unit, ecc1b_reg, port, 0, rval));
                    }
                }
            }
            LOG_VERBOSE(BSL_LS_SOC_SOCMEM,
                      (BSL_META_U(unit,
                                  "SER enable for %s: %s\n"),
                                   str_type, str_name));
        }

        /* Loop through each place-and-route of block entry
         Handle entries with valid REG & FIELD values */
        for (rbi = 0; ; rbi++) {
            rb = &_soc_monterey_ser_route_blocks_irq3[rbi];
            cmic_bit = rb->cmic_bit;
            if (cmic_bit == 0) {
                /* End of table */
                break;
            }
            if (rb->blocktype == ser_block_info[bcount].blocktype) {
                cmic_rval |= cmic_bit;
                if (rb->enable_reg != INVALIDr && rb->enable_field != INVALIDf) {
                    SOC_IF_ERROR_RETURN
                        (soc_reg_field32_modify(unit, rb->enable_reg,
                            REG_PORT_ANY, rb->enable_field, enable ? 1 : 0));
                }
            }
        }

        /* reset (toggle) fifo if applicable */
        if (ser_block_info[bcount].fifo_reset_reg != INVALIDr) {
            SOC_IF_ERROR_RETURN
                 (soc_reg_field32_modify(unit,
                     ser_block_info[bcount].fifo_reset_reg,
                                         REG_PORT_ANY, FIFO_RESETf, 1));
            SOC_IF_ERROR_RETURN
                 (soc_reg_field32_modify(unit,
                     ser_block_info[bcount].fifo_reset_reg,
                                         REG_PORT_ANY, FIFO_RESETf, 0));
        }
    }

    /* Enable 1B error reporting for some special items */

    /* Description of IARB_EN_COR_ERR_RPT says, supress these errors
     * by setting bits. So basically we are supressing error reporting
     * by setting bits.
     * Then again, following TD2P code...
     */
    SOC_IF_ERROR_RETURN
         (soc_reg_field32_modify(unit, IARB_EN_COR_ERR_RPTr,
                                 REG_PORT_ANY,
                                 CMIC_BUFFER_1BIT_ERROR_REPORTf,enable?1:0));
    SOC_IF_ERROR_RETURN
         (soc_reg_field32_modify(unit, IARB_EN_COR_ERR_RPTr,
                                 REG_PORT_ANY,
                                 LEARN_FIFO_1BIT_ERROR_REPORTf, enable?1:0));

    /* Loop through each place-and-route block
       Handle entries with non-null INFO */
    for (rbi = 0; ; rbi++) {
        rb = &_soc_monterey_ser_route_blocks_irq3[rbi];
        cmic_bit = rb->cmic_bit;
        if (cmic_bit == 0) {
            /* End of table */
            break;
        }
        if (rb->info == NULL) {
            continue;
        }
        if (enable) {
            cmic_rval |= cmic_bit;
        }

        SOC_BLOCK_ITER(unit, block_info_idx, rb->blocktype) {
            if (SOC_BLOCK_INFO(unit, block_info_idx).number == rb->id) {
                    port = SOC_BLOCK_PORT(unit, block_info_idx);
                    break;
            }
        }
        if (rb->enable_reg != INVALIDr) {
            if (SOC_BLOCK_IN_LIST(unit, SOC_REG_INFO(unit, rb->enable_reg).block,
                SOC_BLK_PORT) && (port == REG_PORT_ANY)) {
                    /* This port block is not configured */
                    continue;
            }
            SOC_IF_ERROR_RETURN
                (soc_reg_get(unit, rb->enable_reg, port, 0, &rval64));
        }

        rv = _soc_monterey_ser_enable_info(unit, block_info_idx, rb->id, port,
                                         rb->enable_reg, &rval64,
                                         rb->info, INVALIDm, enable);
        if (rv == SOC_E_NOT_FOUND) {
            continue;
        } else if (SOC_FAILURE(rv)) {
            return rv;
        }

        if (rb->enable_reg != INVALIDr) {
            /* Write per route block parity enable register */
            SOC_IF_ERROR_RETURN
                (soc_reg_set(unit, rb->enable_reg, port, 0, rval64));
        }
    }

    if (enable) {
        uint32 rval1;
        /* MMU enables */

        SOC_IF_ERROR_RETURN(READ_MMU_CCP_EN_COR_ERR_RPTr(unit, &rval1));
        soc_reg_field_set(unit, MMU_CCP_EN_COR_ERR_RPTr, &rval1,CCP0_RESEQf, 0);
        SOC_IF_ERROR_RETURN(WRITE_MMU_CCP_EN_COR_ERR_RPTr(unit, rval1));

        SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));

        soc_reg_field_set(unit, MISCCONFIGr, &rval, INIT_MEMf, 0);
        SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));

        soc_reg_field_set(unit, MISCCONFIGr, &rval, INIT_MEMf, 1);
        SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));

        sal_usleep(SAL_BOOT_QUICKTURN ? 100000 : 10); /* Allow things to stabalize */

        soc_reg_field_set(unit, MMU_CCP_EN_COR_ERR_RPTr, &rval1,CCP0_RESEQf, 1);
        SOC_IF_ERROR_RETURN(WRITE_MMU_CCP_EN_COR_ERR_RPTr(unit, rval1));

        /* Write CMIC enable register */
        (void)soc_cmicm_intr3_enable(unit, cmic_rval);
    } else {
        /* MMU disables */
        SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
        soc_reg_field_set(unit, MISCCONFIGr, &rval, PARITY_ENf, 0);
        SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));
        SOC_IF_ERROR_RETURN(WRITE_PARITY_ENr(unit, 0));

        /* Write CMIC disable register */
        (void)soc_cmicm_intr3_disable(unit, cmic_rval);
    }

    SOC_IF_ERROR_RETURN(_soc_monterey_ser_enable_pm(unit,enable));

    return SOC_E_NONE;
}

STATIC int
_soc_monterey_clear_mmu_memory(int unit, soc_mem_t mem)
{
    int i, count;
    static soc_mem_t mmu_mems[] = {
        MMU_INTFI_XPIPE_FC_MAP_TBL0m,
        MMU_INTFI_XPIPE_FC_MAP_TBL1m,
        MMU_INTFI_XPIPE_FC_MAP_TBL2m
    };

    count = COUNTOF(mmu_mems);
    /* MMU mem clear to avoid SER issue */
    for (i = 0; i < count; i++) {
        if (mem != INVALIDm) {
            if (mem == mmu_mems[i]) {
                SOC_IF_ERROR_RETURN
                    (soc_mem_clear(unit, mmu_mems[i], COPYNO_ALL, TRUE));
                break;
            }
        } else {
            SOC_IF_ERROR_RETURN
                (soc_mem_clear(unit, mmu_mems[i], COPYNO_ALL, TRUE));
        }
    }
    return SOC_E_NONE;
}

static int _soc_mn_num_shared_alpm_banks[SOC_MAX_NUM_DEVICES] = {0};
static soc_mem_t *_soc_monterey_alpm_bkt_view_map[SOC_MAX_NUM_DEVICES];
#define _SER_TEST_MEM_MN_ALPM_MASK(num_uft_banks)    ((num_uft_banks == 4) ? 0xFFFF : 0x7FFF)
#define _SER_TEST_MEM_MN_ALPM_SHIFT(num_uft_banks)   ((num_uft_banks == 4) ? 16 : 15)

int
_soc_monterey_mem_sram_info_get (int unit, soc_mem_t mem, int index,
                               _soc_ser_sram_info_t *sram_info)
{
    int i,  base,  offset ;
    int entries_per_ram = 0,  contiguous = 0;

    sram_info->disable_reg = INVALIDr;
    sram_info->disable_field = INVALIDf;
    sram_info->force_reg = INVALIDr;
    sram_info->force_field = INVALIDf;

    switch (mem) {
    case L2Xm:
    case L2_ENTRY_ONLY_ECCm:
        if (mem == L2Xm) {
            sram_info->disable_reg = L2_ENTRY_PARITY_CONTROLr;
            sram_info->disable_field = DISABLE_SBUS_MEMWR_PARITY_CHECKf;
        }

            sram_info->ram_count = SOC_MONTEREY_NUM_EL_L2;
            entries_per_ram = SOC_MONTEREY_RAM_OFFSET_L2_BANK;
            offset = index % entries_per_ram;
            base = offset;
            if (index >= SOC_MONTEREY_NUM_ENTRIES_PER_L2_BANK) {
                base = offset + SOC_MONTEREY_NUM_ENTRIES_PER_L2_BANK;
            }
        break;
    case L3_ENTRY_ONLYm:
    case L3_ENTRY_IPV4_UNICASTm:
    case L3_ENTRY_IPV4_MULTICASTm:
    case L3_ENTRY_IPV6_UNICASTm:
    case L3_ENTRY_IPV6_MULTICASTm:
    case L3_ENTRY_ONLY_ECCm:
            sram_info->ram_count = 4;
            entries_per_ram = 2048;
            base = index;
        break;

    case VLAN_MACm:
    case VLAN_XLATEm:
    case VLAN_XLATE_ECCm:
        contiguous = 1;
        sram_info->disable_reg = VLAN_XLATE_DBGCTRL_0r;
        sram_info->disable_field = DISABLE_SBUS_MEMWR_PARITY_CHECKf;
        sram_info->force_reg = VLAN_XLATE_DBGCTRL_0r;
        sram_info->force_field = FORCE_XOR_GENf;
        sram_info->ram_count = SOC_MONTEREY_NUM_EL_VLAN_XLATE;
        base = (index/4) * 4;
        break;
    case EGR_VLAN_XLATEm:
    case EGR_VLAN_XLATE_ECCm:
        contiguous = 1;
        sram_info->disable_reg = EGR_VLAN_XLATE_CONTROLr;
        sram_info->disable_field = DISABLE_SBUS_MEMWR_PARITY_CHECKf;
        sram_info->force_reg = EGR_VLAN_XLATE_CONTROLr;
        sram_info->force_field = FORCE_XOR_GENf;
        sram_info->ram_count = SOC_MONTEREY_NUM_EL_EGR_VLAN_XLATE;
        base = (index/4) * 4;
        break;
    case ING_L3_NEXT_HOPm:
        sram_info->ram_count = SOC_MONTEREY_NUM_EL_ING_L3_NEXT_HOP;
        entries_per_ram = SOC_MONTEREY_RAM_OFFSET_ING_L3_NEXT_HOP;
        base = index % entries_per_ram;
        break;
    case L3_IPMCm:
        sram_info->ram_count = SOC_MONTEREY_NUM_EL_L3_IPMC;
        entries_per_ram = SOC_MONTEREY_RAM_OFFSET_L3_IPMC;
        base = index % entries_per_ram;
        break;
    case L2MCm:
        sram_info->ram_count = SOC_MONTEREY_NUM_EL_L2MC;
        entries_per_ram = SOC_MONTEREY_RAM_OFFSET_L2MC;
        base = index % entries_per_ram;
        break;
    case L2_ENTRY_LPm:
    case L3_ENTRY_LPm:
    case VLAN_XLATE_LPm:
    case EGR_VLAN_XLATE_LPm:
        sram_info->view[0] = mem;
        sram_info->index_count[0] = 1;
        sram_info->ram_count = 1;
        entries_per_ram = 0;
        base = index;
        break;
    case L2_ENTRY_ISS_LPm:
        base = (soc_mem_index_count(unit, L2_ENTRY_LPm) * 4) + (index * 4);
        sram_info->ram_count = 1;
        sram_info->view[0] = L2Xm;
        break;
    case L3_ENTRY_ISS_LPm:
        base = index;
        SOC_IF_ERROR_RETURN(_soc_monterey_l3_mem_sram_info_get(unit, &base,
                    &sram_info->ram_count, &sram_info->view[0],
                    sram_info->mem_indexes));
        return SOC_E_NONE;
    default:
        return SOC_E_PARAM;
    }
    sram_info->mem_indexes[0][0] = base;
    if (contiguous) {
        for (i = 1; i < sram_info->ram_count; i++) {
            sram_info->mem_indexes[i][0] = sram_info->mem_indexes[i-1][0] + 1;
        }
    } else {
        for (i = 1; i < sram_info->ram_count; i++) {
            sram_info->mem_indexes[i][0] =  sram_info->mem_indexes[i-1][0] + entries_per_ram;
        }
    }
    return SOC_E_NONE;
}


STATIC int
_soc_monterey_mmu_init_default_val(int unit)
{

    return SOC_E_NONE;
}

int
_soc_monterey_mem_ser_control(int unit, soc_mem_t mem, int copyno,
                            int enable)
{
    if (enable) {
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, TOP_SOFT_RESET_REGr,
                                           REG_PORT_ANY, TOP_MMU_RST_Lf, 0));
        sal_usleep(1000);
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, TOP_SOFT_RESET_REGr,
                                           REG_PORT_ANY, TOP_MMU_RST_Lf, 1));
        sal_usleep(1000);
    }
    if (soc_property_get(unit, spn_PARITY_ENABLE, TRUE)) {
        SOC_IF_ERROR_RETURN(_soc_monterey_clear_mmu_memory(unit, INVALIDm));
        SOC_IF_ERROR_RETURN(_soc_monterey_ser_enable_all(unit, enable));
    }
    if (enable) {
        SOC_IF_ERROR_RETURN
            (_soc_monterey_mmu_init_default_val(unit));
    }
    return SOC_E_NONE;
}

void
_soc_monterey_mem_parity_info(int unit, int block_info_idx, int pipe,
                         soc_field_t field_enum, uint32 *minfo)
{
    *minfo = (SOC_BLOCK2SCH(unit, block_info_idx) << SOC_ERROR_BLK_BP)
        | ((pipe & 0xff) << SOC_ERROR_PIPE_BP)
        | (field_enum & SOC_ERROR_FIELD_ENUM_MASK);
}

int
_soc_monterey_mem_is_dyn(int unit, soc_mem_t mem)
{
    if (SOC_MEM_FIELD_VALID(unit, mem, HITf) ||
        SOC_MEM_FIELD_VALID(unit, mem, HIT0f) ||
        SOC_MEM_FIELD_VALID(unit, mem, HIT_0f) ||
        SOC_MEM_FIELD_VALID(unit, mem, HITDA_0f) ||
        SOC_MEM_FIELD_VALID(unit, mem, HITSA_0f) ||
        SOC_MEM_FIELD_VALID(unit, mem, HITSAf) ||
        SOC_MEM_FIELD_VALID(unit, mem, HITDAf) ||
        SOC_MEM_FIELD_VALID(unit, mem, B0_HITf) ||
        SOC_MEM_FIELD_VALID(unit, mem, B1_HITf)) {
        return 1;
    }
    return 0;
}

int
_soc_monterey_populate_ser_log(int unit, soc_reg_t parity_enable_reg,
                          soc_field_t parity_enable_field,
                          soc_mem_t mem,
                          int mem_block,
                          soc_acc_type_t acc_type,
                          int index,
                          sal_usecs_t detect_time,
                          uint32 sblk,
                          uint32 address)
{
    uint32 tmp_entry[SOC_MAX_MEM_WORDS];
    int log_entry_size, id, entry_dw;
    uint32 *cache;
    uint8 *vmap;
    soc_ser_log_tlv_memory_t log_mem;
    soc_ser_log_tlv_generic_t log_generic;
#ifdef INCLUDE_XFLOW_MACSEC
    int block_type = SOC_BLOCK_TYPE(unit, mem_block);
#endif

    sal_memset(&log_generic, 0, sizeof(log_generic));
    sal_memset(&log_mem, 0, sizeof(log_mem));

    /*
    must be large enough for at least generic and terminator, as well as the
    memory type since we might decode it in soc_ser_correction.
    */
    log_entry_size = sizeof(soc_ser_log_tlv_hdr_t)*3 +
                     sizeof(soc_ser_log_tlv_generic_t) +
                     sizeof(soc_ser_log_tlv_memory_t);

    if (mem == INVALIDm) {
        mem = soc_addr_to_mem_extended(unit, sblk, acc_type, address);
        if (mem != INVALIDm) {
            if (sblk) {
                SOC_MEM_BLOCK_ITER(unit, mem, mem_block) {
                    if (SOC_BLOCK2OFFSET(unit, mem_block) == sblk) {
                        break;
                    }
                }
            } else {
                mem_block = SOC_MEM_BLOCK_ANY(unit, mem);
            }
        }
    }
    /*
    If we have decoded the memory we can record its contents/cache
    */
    if((mem != INVALIDm) &&
       (SOC_MEM_SER_CORRECTION_TYPE(unit, mem) != 0)) {
        entry_dw = soc_mem_entry_words(unit, mem);

        /* Check to make sure this isn't a duplicate */
        /* Search for a log entry with the same mem, and index with within the last 5 seconds */
        if (soc_ser_log_find_recent(unit, mem, index, sal_time_usecs()) > 0) {
            return 0;
        }

        log_entry_size += sizeof(soc_ser_log_tlv_hdr_t) + entry_dw*4;

        cache = SOC_MEM_STATE(unit, mem).cache[mem_block];
        vmap = SOC_MEM_STATE(unit, mem).vmap[mem_block];
        if (SOC_MEM_INFO(unit, mem).flags & SOC_MEM_FLAG_CACHABLE) {
            log_entry_size += sizeof(soc_ser_log_tlv_hdr_t) + entry_dw*4;
        }

        /* create the entry based on determined size, save id */
        id = soc_ser_log_create_entry(unit, log_entry_size);

        /* Add the memory information to the log now so we can detect duplicate errors */
        log_generic.time = detect_time;
        log_mem.memory = mem;
        log_mem.index = index;
        soc_ser_log_add_tlv(unit, id, SOC_SER_LOG_TLV_MEMORY,
            sizeof(soc_ser_log_tlv_memory_t), &log_mem);
        soc_ser_log_add_tlv(unit, id, SOC_SER_LOG_TLV_GENERIC,
            sizeof(soc_ser_log_tlv_generic_t), &log_generic);

        /* Disable Parity */
        if (!_soc_monterey_mem_is_dyn(unit, mem) &&
            (parity_enable_reg != INVALIDr) &&
            (parity_enable_field != INVALIDf)) {
#ifdef INCLUDE_XFLOW_MACSEC
            if (soc_feature(unit, soc_feature_xflow_macsec)) {
                if (soc_reg_field32_modify(unit, parity_enable_reg,
                            REG_PORT_ANY, parity_enable_field, (block_type != SOC_BLK_MACSEC) ? 0 : 1) < 0) {
                    return 0;
                }
            } else
#endif
            {
                if (soc_reg_field32_modify(unit, parity_enable_reg,
                            REG_PORT_ANY, parity_enable_field, 0) < 0) {
                    return 0;
                }
            }
        }

        /* Enable Parity */
        if (!_soc_monterey_mem_is_dyn(unit, mem) &&
            (parity_enable_reg != INVALIDr) &&
            (parity_enable_field != INVALIDf)) {
#ifdef INCLUDE_XFLOW_MACSEC
            if (soc_feature(unit, soc_feature_xflow_macsec)) {
                if (soc_reg_field32_modify(unit, parity_enable_reg,
                            REG_PORT_ANY, parity_enable_field, (block_type != SOC_BLK_MACSEC) ? 0 : 1) < 0) {
                    return 0;
                }
            } else
#endif
            {
                if (soc_reg_field32_modify(unit, parity_enable_reg,
                            REG_PORT_ANY, parity_enable_field, 0) < 0) {
                    return 0;
                }
            }

             /*Enable NACK on read */
              soc_mem_read_extended(unit, SOC_MEM_SCHAN_ERR_RETURN,
                              mem, 0, mem_block,
                              index, tmp_entry);
              /* fill in the memory contents tlv */
              if (soc_ser_log_add_tlv(unit, id, SOC_SER_LOG_TLV_CONTENTS,
                  entry_dw*4, tmp_entry) < 0) {
                  return 0;
              }

#ifdef INCLUDE_XFLOW_MACSEC
            if (soc_feature(unit, soc_feature_xflow_macsec)) {
                if (soc_reg_field32_modify(unit, parity_enable_reg,
                            REG_PORT_ANY, parity_enable_field, (block_type != SOC_BLK_MACSEC) ? 1 : 0) < 0) {
                    return 0;
                }
            } else
#endif
            {
                if (soc_reg_field32_modify(unit, parity_enable_reg,
                            REG_PORT_ANY, parity_enable_field, 1) < 0) {
                    return 0;
                }

            }

        }


        if(cache != NULL && CACHE_VMAP_TST(vmap, index)) {
            /* fill in the memory cache tlv */
            if (soc_ser_log_add_tlv(unit, id, SOC_SER_LOG_TLV_CACHE,
                entry_dw*4, (cache + index*entry_dw)) < 0) {
                return 0;
            }
        }
    } else {
        id = soc_ser_log_create_entry(unit, log_entry_size);
    }

    return id;
}

STATIC int
_soc_monterey_ser_process_parity(int unit, int block_info_idx, int pipe,
                               int port, const _soc_monterey_ser_info_t *info,
                               int schan, char *prefix_str, char *mem_str)
{
    int rv;
    _soc_monterey_ser_reg_t reg_entry[2], *reg_ptr;
    soc_reg_t reg;
    uint32 rval, minfo;
    uint32 multiple, entry_idx, idx, has_error;
    char *mem_str_ptr;
    _soc_ser_correct_info_t spci;

    if (schan) {
        /* Some table does not have NACK register */
        return SOC_E_NONE;
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
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, reg, port, 0, &rval));

        if (soc_reg_field_get(unit, reg, rval, PARITY_ERRf)) {
            has_error = TRUE;
            multiple = soc_reg_field_get(unit, reg, rval, MULTIPLE_ERRf);
            entry_idx = soc_reg_field_get(unit, reg, rval, ENTRY_IDXf);
            _soc_monterey_mem_parity_info(unit, block_info_idx, pipe,
                                     info->group_reg_status_field, &minfo);
            soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                               SOC_SWITCH_EVENT_DATA_ERROR_PARITY,
                               entry_idx, minfo);
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
            if (idx == 0 && info->mem != INVALIDm) {
                sal_memset(&spci, 0, sizeof(spci));
                spci.flags = SOC_SER_SRC_MEM | SOC_SER_REG_MEM_KNOWN | SOC_SER_LOG_WRITE_CACHE;
                if (multiple) {
                    spci.flags |= SOC_SER_ERR_MULTI;
                }
                spci.reg = INVALIDr;
                spci.mem = info->mem;
                spci.blk_type = -1;
                spci.index = entry_idx;
                spci.parity_type = info->type;
                spci.detect_time = sal_time_usecs();
                spci.log_id = _soc_monterey_populate_ser_log(unit,
                                          info->enable_reg,
                                          info->enable_field,
                                          spci.mem,
                                          SOC_MEM_BLOCK_ANY(unit, spci.mem),
                                          spci.acc_type,
                                          spci.index,
                                          spci.detect_time,
                                          spci.sblk,
                                          spci.addr);
                rv = soc_ser_correction(unit, &spci);
                if (spci.log_id != 0) {
                    soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                       SOC_SWITCH_EVENT_DATA_ERROR_LOG,
                                       spci.log_id, 0);
                }
                if (SOC_FAILURE(rv)) {
                    /* Add reporting failed to correct event flag to
                     * application */
                    soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                            SOC_SWITCH_EVENT_DATA_ERROR_FAILEDTOCORRECT,
                            entry_idx, minfo);
                    return rv;
                }
            }
        }

        /* Clear parity status */
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, reg, port, 0, 0));
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
_soc_monterey_ser_process_ecc(int unit, int block_info_idx, int pipe, int port,
                              const _soc_monterey_ser_info_t *info,
                              int schan, char *prefix_str, char *mem_str)
{
    int rv;
    _soc_monterey_ser_reg_t reg_entry[2], *reg_ptr;
    soc_reg_t reg = INVALIDr;
    uint32 rval, minfo,has_error = 0;
     uint64 rval64;
    uint32 ecc_err, multiple, double_bit, entry_idx, idx;
    char *mem_str_ptr;
    _soc_ser_correct_info_t spci;

    if (schan) {
        /* Some table does not have NACK register */
        return SOC_E_NONE;
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
            if (SOC_REG_IS_64(unit, reg)) {
                SOC_IF_ERROR_RETURN
                (soc_reg64_get(unit, reg, port, 0, &rval64));
                ecc_err = soc_reg64_field32_get(unit, reg, rval64, ECC_ERRf);
                multiple = soc_reg64_field32_get(unit, reg, rval64, MULTIPLE_ERRf);
                double_bit = soc_reg64_field32_get(unit, reg, rval64, DOUBLE_BIT_ERRf);
                entry_idx = soc_reg64_field32_get(unit, reg, rval64, ENTRY_IDXf);
            } else {
               SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, reg, port, 0, &rval));
          	  ecc_err = soc_reg_field_get(unit, reg, rval, ECC_ERRf);
           	 multiple = soc_reg_field_get(unit, reg, rval, MULTIPLE_ERRf);
           	 double_bit = soc_reg_field_get(unit, reg, rval, DOUBLE_BIT_ERRf);
            	entry_idx = soc_reg_field_get(unit, reg, rval, ENTRY_IDXf);
            }

             if (ecc_err) {
                  has_error = TRUE;
             }
            _soc_monterey_mem_parity_info(unit, block_info_idx, pipe,
                                     info->group_reg_status_field, &minfo);
            soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                               SOC_SWITCH_EVENT_DATA_ERROR_ECC,
                               entry_idx, minfo);
            sal_memset(&spci, 0, sizeof(spci));
            if (double_bit) {
                spci.double_bit = 1;
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "%s %s entry %d double-bit ECC error\n"),
                           prefix_str, mem_str_ptr, entry_idx));
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
            if (idx == 0 && info->mem != INVALIDm) {
                spci.flags = SOC_SER_SRC_MEM | SOC_SER_REG_MEM_KNOWN | SOC_SER_LOG_WRITE_CACHE;
                if (multiple) {
                    spci.flags |= SOC_SER_ERR_MULTI;
                }
                spci.reg = INVALIDr;
                spci.mem = info->mem;
                spci.blk_type = -1;
                spci.index = entry_idx;
                spci.parity_type = info->type;
                spci.detect_time = sal_time_usecs();
                spci.log_id = _soc_monterey_populate_ser_log(unit,
                                          info->enable_reg,
                                          info->enable_field,
                                          spci.mem,
                                          SOC_MEM_BLOCK_ANY(unit, spci.mem),
                                          spci.acc_type,
                                          spci.index,
                                          spci.detect_time,
                                          spci.sblk,
                                          spci.addr);
                rv = soc_ser_correction(unit, &spci);
                if (spci.log_id != 0) {
                    soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                       SOC_SWITCH_EVENT_DATA_ERROR_LOG,
                                       spci.log_id, 0);
                }
                if (SOC_FAILURE(rv)) {
                    /* Add reporting failed to correct event flag to
                     * application */
                    soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                            SOC_SWITCH_EVENT_DATA_ERROR_FAILEDTOCORRECT,
                            entry_idx, minfo);
                    return rv;
                }
            }

        if (SOC_REG_IS_64(unit, reg)) {
            COMPILER_64_ZERO(rval64);
            SOC_IF_ERROR_RETURN
                (soc_reg64_set(unit, reg, port, 0, rval64));
        } else {
            SOC_IF_ERROR_RETURN
                (soc_reg32_set(unit, reg, port, 0, 0));
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
_soc_monterey_ser_process_mac(int unit, int block_info_idx, int pipe, int port,
                            const _soc_monterey_ser_info_t *info,
                            int schan, char *prefix_str, char *mem_str)
{
    _soc_monterey_ser_reg_t reg_entry[2], *reg_ptr;
    soc_reg_t reg;
    soc_field_t single_bit_f, double_bit_f;
    uint64 rval;
    uint32 single_bit = 0, double_bit = 0, idx;
    char *mem_str_ptr;

    if (schan) {
        /* Some table does not have NACK register */
        return SOC_E_NONE;
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

    switch (info->type) {
        case _SOC_PARITY_TYPE_MAC_TX_CDC:
            single_bit_f = TX_CDC_SINGLE_BIT_ERRf;
            double_bit_f = TX_CDC_DOUBLE_BIT_ERRf;
            break;
        case _SOC_PARITY_TYPE_MAC_RX_CDC:
            single_bit_f = RX_CDC_SINGLE_BIT_ERRf;
            double_bit_f = RX_CDC_DOUBLE_BIT_ERRf;
            break;
        case _SOC_PARITY_TYPE_MAC_RX_TS:
            single_bit_f = RX_TS_MEM_SINGLE_BIT_ERRf;
            double_bit_f = RX_TS_MEM_DOUBLE_BIT_ERRf;
            break;
        default:
            return SOC_E_PARAM;
    }

    for (idx = 0; reg_ptr[idx].reg != INVALIDr; idx++) {
        reg = reg_ptr[idx].reg;
        mem_str_ptr = reg_ptr[idx].mem_str != NULL ? reg_ptr[idx].mem_str : mem_str;
        SOC_IF_ERROR_RETURN(soc_reg64_get(unit, reg, port, 0, &rval));

        single_bit = soc_reg64_field32_get(unit, reg, rval, single_bit_f);
        double_bit = soc_reg64_field32_get(unit, reg, rval, double_bit_f);
        if (single_bit || double_bit) {
            soc_event_generate(unit, SOC_SWITCH_EVENT_DATA_ERROR_PARITY,
                               SOC_SWITCH_EVENT_DATA_ERROR_ECC, 0, 0);
            if (double_bit) {
                soc_event_generate(unit, SOC_SWITCH_EVENT_DATA_ERROR_PARITY,
                                   SOC_SWITCH_EVENT_DATA_ERROR_UNCORRECTABLE, 0, 0);
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "%s %s double-bit ECC error on port %d\n"),
                           prefix_str, mem_str_ptr, port));
            } else {
                soc_event_generate(unit, SOC_SWITCH_EVENT_DATA_ERROR_PARITY,
                                   SOC_SWITCH_EVENT_DATA_ERROR_AUTO_CORRECTED, 0, 0);
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "%s %s single-bit ECC error on port %d\n"),
                           prefix_str, mem_str_ptr, port));
            }
        }

        /* Clear parity status by a rising edge on intr_clr_field in intr_clr_reg */
        SOC_IF_ERROR_RETURN
            (soc_reg64_get(unit, info->intr_clr_reg, port, 0, &rval));
        if (single_bit) {
            soc_reg64_field32_set(unit, info->intr_clr_reg, &rval, info->intr_clr_field_list[0], 0);
            SOC_IF_ERROR_RETURN
                (soc_reg64_set(unit, info->intr_clr_reg, port, 0, rval));
            soc_reg64_field32_set(unit, info->intr_clr_reg, &rval, info->intr_clr_field_list[0], 1);
            SOC_IF_ERROR_RETURN
                (soc_reg64_set(unit, info->intr_clr_reg, port, 0, rval));
        }
        if (double_bit) {
            soc_reg64_field32_set(unit, info->intr_clr_reg, &rval, info->intr_clr_field_list[1], 0);
            SOC_IF_ERROR_RETURN
                (soc_reg64_set(unit, info->intr_clr_reg, port, 0, rval));
            soc_reg64_field32_set(unit, info->intr_clr_reg, &rval, info->intr_clr_field_list[1], 1);
            SOC_IF_ERROR_RETURN
                (soc_reg64_set(unit, info->intr_clr_reg, port, 0, rval));
        }
    }

    return SOC_E_NONE;
}
STATIC int
_soc_monterey_ser_process_mib(int unit, int block_info_idx, int pipe, int port,
                            const _soc_monterey_ser_info_t *info,
                                                        int schan, char *prefix_str, char *mem_str)
{
    _soc_monterey_ser_reg_t reg_entry[2], *reg_ptr;
    soc_reg_t reg;
    uint32 rval;
    uint32 ecc_err, multiple, double_bit, entry_idx, idx, has_error;
    char *mem_str_ptr;
    if (schan) {
    /* Some table does not have NACK register */
        return SOC_E_NONE;
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
             SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, port, 0, &rval));
             ecc_err = soc_reg_field_get(unit, reg, rval, ECC_ERRf);
             multiple = soc_reg_field_get(unit, reg, rval, MULTIPLE_ERRf);
             double_bit = soc_reg_field_get(unit, reg, rval, DOUBLE_BIT_ERRf);
             entry_idx = soc_reg_field_get(unit, reg, rval, ENTRY_IDXf);
             if (ecc_err) {
                 has_error = TRUE;
                 soc_event_generate(unit, SOC_SWITCH_EVENT_DATA_ERROR_PARITY,
                                    SOC_SWITCH_EVENT_DATA_ERROR_ECC, 0, 0);
                 if (double_bit) {
                     soc_event_generate(unit, SOC_SWITCH_EVENT_DATA_ERROR_PARITY,
                                        SOC_SWITCH_EVENT_DATA_ERROR_UNCORRECTABLE,
                                        entry_idx, 0);

                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U
                              (unit, "%s %s double-bit ECC error on port %d\n"),
                              prefix_str, mem_str_ptr, port));
                 } else {
                     soc_event_generate(unit, SOC_SWITCH_EVENT_DATA_ERROR_PARITY,
                                        SOC_SWITCH_EVENT_DATA_ERROR_AUTO_CORRECTED,
                                        entry_idx, 0);
                      LOG_ERROR(BSL_LS_SOC_COMMON,
                                (BSL_META_U
                                (unit, "%s %s single-bit ECC error on port %d\n"),
                                prefix_str, mem_str_ptr, port));

                       }
                       if (multiple) {
                           LOG_ERROR(BSL_LS_SOC_COMMON,
                           (BSL_META_U(unit, "%s %s has multiple ECC errors\n"),
                            prefix_str, mem_str_ptr));
                       }
                       if (double_bit) {
                           /* Entry Idx:
                            * 0-15  Port 0
                            * 16-31 Port 1
                            * 32-47 Port 2
                            * 48-63 Port 3
                            * In case of double-bit error we need to reset MIB counters
                            * for that port to recover
                            */
                            rval = 0x1 << ((entry_idx & 63) / 16);
                            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, info->intr_clr_reg, port, 0, rval));
                            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, info->intr_clr_reg, port, 0, 0x0));
                      }
            }
/* Clear parity status */
                  SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, 0));
     }
    if (!has_error) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
       (BSL_META_U(unit, "%s %s ECC hardware inconsistency\n"),
        prefix_str, mem_str));
    }
    return SOC_E_NONE;
}

STATIC void
_soc_monterey_ser_control_reg_get(int unit,
                                void *fifo_ser_list,
                                soc_mem_t   mem,
                                soc_reg_t   *ser_control_reg,
                                soc_field_t *ser_enable_field)
{
    int i;
    _soc_mem_ser_en_info_t *ser_en_info = NULL;

    if ((fifo_ser_list == NULL) || (ser_control_reg == NULL) ||
        (ser_enable_field == NULL)) {
        return;
    }

    ser_en_info = (_soc_mem_ser_en_info_t*)fifo_ser_list;
    for (i = 0; ser_en_info[i].mem != INVALIDm; i++) {
        if (ser_en_info[i].mem == mem) {
            *ser_control_reg = ser_en_info[i].en_ctrl.ctrl_type.en_reg;
            *ser_enable_field = ser_en_info[i].en_ctrl.en_fld;
            break;
        }
    }
}

STATIC int
_soc_monterey_ser_process_mmu_err (int unit, int block_info_idx,
                                 const _soc_monterey_ser_info_t *info,
                                 char *prefix_str)
{
    uint32 rval, err, addr, bidx;
    uint64 rval64;
    soc_reg_t reg = MMU_MEM_FAIL_ADDR_64r;
    uint8 blk_idx;
    uint32 sblk = 0;
    int rv = SOC_E_NONE;
    _soc_ser_correct_info_t spci;
    soc_reg_t parity_enable_reg = INVALIDr;
    soc_field_t parity_enable_field = INVALIDf;

    SOC_IF_ERROR_RETURN(READ_MEM_SER_FIFO_STSr(unit, &rval));
    if (soc_reg_field_get(unit, MEM_SER_FIFO_STSr, rval, EMPTYf)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "MMU SER interrupt with empty fifo !!\n")));
        SOC_IF_ERROR_RETURN(READ_MEM_FAIL_INT_STATr(unit, &rval));
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "MMU ERR status: 0x%08x\n"), rval));
        SOC_IF_ERROR_RETURN(WRITE_MEM_FAIL_INT_STATr(unit, 0));
        return rv;
    }
    SOC_BLOCK_ITER(unit, blk_idx, SOC_BLK_MMU) {
        sblk = SOC_BLOCK2SCH(unit, blk_idx);
        break;
    }
    do {
        SOC_IF_ERROR_RETURN(READ_MMU_MEM_FAIL_ADDR_64r(unit, &rval64));
        err = soc_reg64_field32_get(unit, reg, rval64, ERR_TYPEf);
        addr = soc_reg64_field32_get(unit, reg, rval64, EADDRf);
        bidx = soc_reg64_field32_get(unit, reg, rval64, BIDXf);

        LOG_ERROR(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "MMU ERR Type: %s, Addr: 0x%08x, module: %d\n"),
                             (err == 1) ? "1B error" : "2B error", addr, bidx));

        /* Address reported in MMU_MEM_FAIL_ADDR_64 is the entry offset within
         * MMU sub block. The complete Sbus address is composed of MMU sub block
         * base address and offset within MMU sub block address space.
         */
        addr = (bidx << 26) | addr;
        sal_memset(&spci, 0, sizeof(spci));
        spci.flags |= SOC_SER_SRC_MEM;
        spci.reg = INVALIDr;
        spci.mem = INVALIDm;
        spci.acc_type = -1;
        spci.blk_type = SOC_BLK_MMU;
        spci.sblk = sblk;
        spci.detect_time = sal_time_usecs();

        /* Try to decode memory */
        spci.mem = soc_addr_to_mem_extended(unit, sblk, _SOC_ACC_TYPE_PIPE_ANY, addr);
        if (spci.mem != INVALIDm) {
            spci.index = addr - soc_mem_base(unit, spci.mem);
            spci.flags |= SOC_SER_REG_MEM_KNOWN;
            _soc_monterey_ser_control_reg_get(unit,
                                            _soc_bcm56670_a0_mmu_mem_ser_info,
                                            spci.mem,
                                            &parity_enable_reg,
                                            &parity_enable_field);
        } else {
            spci.flags |= SOC_SER_REG_MEM_UNKNOWN;
        }
        soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                           SOC_SWITCH_EVENT_DATA_ERROR_ECC,
                           sblk | SOC_SER_ERROR_DATA_BLK_ADDR_SET, addr);
        spci.flags |= SOC_SER_LOG_WRITE_CACHE;
        spci.log_id = _soc_monterey_populate_ser_log(unit, parity_enable_reg,
                                                   parity_enable_field,
                                                   spci.mem, blk_idx,
                                                   _SOC_ACC_TYPE_PIPE_ANY,
                                                   spci.index,
                                                   spci.detect_time, spci.sblk,
                                                   spci.addr);
        if ((spci.mem == INVALIDm) ||
            (SOC_MEM_SER_CORRECTION_TYPE(unit, spci.mem) != 0)) {
            rv = soc_ser_correction(unit, &spci);
            if (SOC_FAILURE(rv)) {
                soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                   SOC_SWITCH_EVENT_DATA_ERROR_FAILEDTOCORRECT,
                                   sblk | SOC_SER_ERROR_DATA_BLK_ADDR_SET, addr);
            }
        } else {
            rv = SOC_E_NONE;
            soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                               SOC_SWITCH_EVENT_DATA_ERROR_AUTO_CORRECTED,
                               sblk | SOC_SER_ERROR_DATA_BLK_ADDR_SET, addr);
        }

        if (spci.log_id != 0) {
            soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                               SOC_SWITCH_EVENT_DATA_ERROR_LOG,
                               spci.log_id, 0);
        }
        SOC_IF_ERROR_RETURN(READ_MEM_FAIL_INT_CTRr(unit, &rval));
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "MMU ERR ctr: %d\n"), rval));
        SOC_IF_ERROR_RETURN(READ_MEM_SER_FIFO_STSr(unit, &rval));
    } while (!soc_reg_field_get(unit, MEM_SER_FIFO_STSr, rval, EMPTYf));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, info->intr_status_reg, REG_PORT_ANY,
                                info->group_reg_status_field, 0));

    SOC_IF_ERROR_RETURN
    (soc_reg_field32_modify(unit, MEM_FAIL_INT_CLEARr, REG_PORT_ANY,
                           info->group_reg_status_field, 1));
    return rv;
}

STATIC int
_soc_monterey_ser_process_start_err(int unit, int block_info_idx,
                                    const _soc_monterey_ser_info_t *info,
                                    char *prefix_str)
{
    soc_info_t *si;
    uint64 rval64;
    uint32 pbmp_31_0, pbmp_63_32;
    soc_pbmp_t pbmp;
    uint32 minfo ;
    int port, mmu_port, phy_port;
    soc_ser_log_tlv_generic_t log_generic;
    int log_id;

    sal_memset(&log_generic, 0, sizeof(soc_ser_log_tlv_generic_t));

    si = &SOC_INFO(unit);

    /* mmu port 52-0 */
    SOC_IF_ERROR_RETURN(READ_START_BY_START_ERROR_0r(unit, &rval64));
    pbmp_31_0 = COMPILER_64_LO(rval64);
    pbmp_63_32 = COMPILER_64_HI(rval64);
    SOC_PBMP_CLEAR(pbmp);
    SOC_PBMP_WORD_SET(pbmp, 0, pbmp_31_0);
    SOC_PBMP_WORD_SET(pbmp, 1, pbmp_63_32);

    SOC_PBMP_ITER(pbmp, mmu_port) {
        phy_port = si->port_m2p_mapping[mmu_port];
        port = si->port_p2l_mapping[phy_port];
        _soc_monterey_mem_parity_info(unit, block_info_idx, 0,
                                 info->group_reg_status_field, &minfo);
        /* Report uncorrectable event flag to application */
        soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                           SOC_SWITCH_EVENT_DATA_ERROR_UNCORRECTABLE, 0,
                           minfo);

        log_generic.time = sal_time_usecs();
        log_generic.boot_count = soc_ser_log_get_boot_count(unit);
        log_generic.block_type = SOC_BLOCK_INFO(unit, block_info_idx).type;
        log_generic.parity_type = info->type;

        log_id = soc_ser_log_create_entry(unit,
            sizeof(soc_ser_log_tlv_generic_t) +
            sizeof(soc_ser_log_tlv_hdr_t) *2);

        soc_ser_log_add_tlv(unit, log_id, SOC_SER_LOG_TLV_GENERIC,
            sizeof(soc_ser_log_tlv_generic_t), &log_generic);
        soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                           SOC_SWITCH_EVENT_DATA_ERROR_LOG, log_id, 0);
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "%s port %d start error detected\n"), prefix_str,
                              port));
    }

    /* Clear parity status */
    COMPILER_64_ZERO(rval64);
    SOC_IF_ERROR_RETURN(WRITE_START_BY_START_ERROR_0r(unit, rval64));
    SOC_IF_ERROR_RETURN(WRITE_START_BY_START_ERROR_1r(unit, rval64));

    return SOC_E_NONE;
}

STATIC int
_soc_monterey_process_ser (int unit, int block_info_idx, int inst, int pipe,
                         int port, soc_reg_t group_reg, uint64 group_rval,
                         const _soc_monterey_ser_info_t *info_list,
                         char *prefix_str);

STATIC _soc_monterey_bst_hw_cb monterey_bst_cb;

STATIC int
_soc_monterey_process_mmu_bst(int unit)
{
    int rv = SOC_E_NONE;
    if (monterey_bst_cb) {
        rv = monterey_bst_cb(unit);
    }
    return rv;
}

int soc_monterey_set_bst_callback(int unit, _soc_monterey_bst_hw_cb cb)
{
    monterey_bst_cb = cb;
    return SOC_E_NONE;
}

STATIC int
_soc_monterey_ser_process_pm_ecc(int unit, int port,
                                 const _soc_monterey_pm_ecc_info_t *info)
{

    if (info->ecc_err_type) {

        soc_event_generate(unit, SOC_SWITCH_EVENT_DATA_ERROR_PARITY,
                           SOC_SWITCH_EVENT_DATA_ERROR_ECC, 0, 0);

        if (info->ecc_err_type & PORTMOD_PM_CPRI_ECC_ERR_1_BIT) {

            soc_event_generate(unit, SOC_SWITCH_EVENT_DATA_ERROR_PARITY,
                           SOC_SWITCH_EVENT_DATA_ERROR_AUTO_CORRECTED, 0, 0);

            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "\n%s[%s]\n\r "
                                  "single-bit ECC error on port %d\n"),
                                  info->mem_str,
                                  SOC_MEM_NAME(unit, info->mem), port));

        } else if (info->ecc_err_type & PORTMOD_PM_CPRI_ECC_ERR_2_BIT) {

            soc_event_generate(unit, SOC_SWITCH_EVENT_DATA_ERROR_PARITY,
                           SOC_SWITCH_EVENT_DATA_ERROR_UNCORRECTABLE, 0, 0);

            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "\n%s[%s]\n\r "
                                  "double-bit ECC error on port %d\n"),
                                  info->mem_str,
                                  SOC_MEM_NAME(unit, info->mem), port));

        } else {

            soc_event_generate(unit, SOC_SWITCH_EVENT_DATA_ERROR_PARITY,
                           SOC_SWITCH_EVENT_DATA_ERROR_UNCORRECTABLE, 0, 0);

            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "\n%s[%s]\n\r "
                                  "multi-bit ECC error on port %d\n"),
                                  info->mem_str,
                                  SOC_MEM_NAME(unit, info->mem), port));

        }
    }

    return SOC_E_NONE;
}

STATIC int
_soc_monterey_process_ser (int unit, int block_info_idx, int inst, int pipe,
                         int port, soc_reg_t group_reg, uint64 group_rval,
                         const _soc_monterey_ser_info_t *info_list,
                         char *prefix_str)
{
    const _soc_monterey_ser_info_t *info;
    int info_index;
    char *mem_str = "INVALIDm" ;
    uint32 minfo;
    soc_ser_log_tlv_generic_t log_generic;
    int log_id;

    sal_memset(&log_generic, 0, sizeof(soc_ser_log_tlv_generic_t));

    /* Loop through each info entry in the list */
    for (info_index = 0; ; info_index++) {
        info = &info_list[info_index];
        if (info->type == _SOC_PARITY_TYPE_NONE) {
            /* End of table */
            break;
        }

        /* Check status for the info entry in the group register */
        if ((info->group_reg_status_field != INVALIDf) &&
            (!soc_reg64_field32_get(unit, group_reg, group_rval,
                                    info->group_reg_status_field))) {
            continue;
        }

        if (info->mem_str) {
            mem_str = info->mem_str;
        } else if (info->mem != INVALIDm) {
            mem_str = SOC_MEM_NAME(unit, info->mem);
        } else if (info->group_reg_status_field != INVALIDf) {

            mem_str = SOC_FIELD_NAME(unit, info->group_reg_status_field);
        }

        /* Handle different parity error reporting style */
        switch (info->type) {
        case _SOC_PARITY_TYPE_GENERIC:
            _soc_monterey_mem_parity_info(unit, block_info_idx, 0,
                                     info->group_reg_status_field, &minfo);
            /* Report uncorrectable event flag to application */
            soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                               SOC_SWITCH_EVENT_DATA_ERROR_UNCORRECTABLE, 0,
                               minfo);

            log_generic.time = sal_time_usecs();
            log_generic.boot_count = soc_ser_log_get_boot_count(unit);
            log_generic.block_type = SOC_BLOCK_INFO(unit, block_info_idx).type;
            log_generic.parity_type = info->type;

            log_id = soc_ser_log_create_entry(unit,
                sizeof(soc_ser_log_tlv_generic_t) +
                sizeof(soc_ser_log_tlv_hdr_t) *2);

            soc_ser_log_add_tlv(unit, log_id, SOC_SER_LOG_TLV_GENERIC,
                sizeof(soc_ser_log_tlv_generic_t), &log_generic);
            soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                               SOC_SWITCH_EVENT_DATA_ERROR_LOG, log_id, 0);

            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "%s %s asserted\n"), prefix_str, mem_str));
            break;
        case _SOC_PARITY_TYPE_PARITY:
            /* PARITY_ERRf, MULTIPLE_ERRf, ENTRY_IDXf */
            SOC_IF_ERROR_RETURN
                (_soc_monterey_ser_process_parity(unit, block_info_idx,
                                                  pipe, port, info, FALSE,
                                                  prefix_str, mem_str));
            break;
        case _SOC_PARITY_TYPE_ECC:
            /* ECC_ERRf, MULTIPLE_ERRf, DOUBLE_BIT_ERRf, ENTRY_IDXf */
            SOC_IF_ERROR_RETURN
                (_soc_monterey_ser_process_ecc(unit, block_info_idx, pipe,
                                               port, info, FALSE,
                                               prefix_str, mem_str));
            break;
          case _SOC_PARITY_TYPE_MIB:
             /* ECC_ERRf, MULTIPLE_ERRf, DOUBLE_BIT_ERRf, ENTRY_IDXf */
               SOC_IF_ERROR_RETURN
                 (_soc_monterey_ser_process_mib(unit, block_info_idx, pipe,
                    port, info, FALSE,
                    prefix_str, mem_str));
                  /* MIB_RX_MEM_ERR and MIB_TX_MEM_ERR in PORT_INTR_STATUS are write 1 to clear
                   * So we write back the status we read.
                   */
                 SOC_IF_ERROR_RETURN(soc_reg_set(unit, group_reg, port, 0, group_rval));
             break;
        case _SOC_PARITY_TYPE_MAC_TX_CDC:
        case _SOC_PARITY_TYPE_MAC_RX_CDC:
        case _SOC_PARITY_TYPE_MAC_RX_TS:
            /* xxx_SINGLE_BIT_ERRf, xxx_DOUBLE_BIT_ERRf */
            SOC_IF_ERROR_RETURN
                (_soc_monterey_ser_process_mac(unit, block_info_idx, pipe,
                                             port, info, FALSE,
                                             prefix_str, mem_str));
             /* MAC_RX_CDC_MEM_ERR, MAC_TX_CDC_MEM_ERR, MAC_RX_TS_CDC_MEM_ERR
                          * in PORT_INTR_STATUS are write 1 to clear
                                       * So we write back the status we read.
                                                    */
              SOC_IF_ERROR_RETURN(soc_reg_set(unit, group_reg, port, 0, group_rval));
            break;
        case _SOC_PARITY_TYPE_START_ERR:
            SOC_IF_ERROR_RETURN
                (_soc_monterey_ser_process_start_err(unit, block_info_idx,
                                                     info, prefix_str));
            break;
        case _SOC_PARITY_TYPE_MMU_SER:
            SOC_IF_ERROR_RETURN
                (_soc_monterey_ser_process_mmu_err(unit, block_info_idx,
                                                   info, prefix_str));
            break;
        case _SOC_PARITY_TYPE_BST:
            SOC_IF_ERROR_RETURN(_soc_monterey_process_mmu_bst(unit));
            break;
        case _SOC_PARITY_TYPE_OBM:
            SOC_IF_ERROR_RETURN(_soc_monterey_obm_interrupt_cb(unit));
            break;
        default:
            break;
        } /* Handle different parity error reporting style */
    } /* Loop through each info entry in the list */

    return SOC_E_NONE;
}

STATIC char *_soc_monterey_ser_hwmem_base_info[] = {
    "CMIC PKT BUFFER - In Iarb",
    "CPU PKT BUFFER - In Iarb",
    "Invalid value",
    "EINITBUF_PACKET_BUFFER - In Iarb",
    "INGRESS PACKET BUFFER - In Ipars",
    "Invalid value",
    "Invalid value",
    "Invalid value",
    "Invalid value",
    "SW2_EOP_BUFFER_A - In Iesmif",
    "SW2_EOP_BUFFER_B - In Iesmif",
    "SW2_EOP_BUFFER_C - In Iesmif",
    "Invalid value",
    "Invalid value",
    "Invalid value",
    "Invalid value",
    "EP MPB DATA - In El3",
    "EP INITBUF - In Ehcpm",
    "CM DATA BUFFER - In Edatabuf",
    "XLP0 DATA BUFFER - In Edatabuf",
    "XLP1 DATA BUFFER - In Edatabuf",
    "XLP2 DATA BUFFER - In Edatabuf",
    "XLP3 DATA BUFFER - In Edatabuf",
    "XLP4 DATA BUFFER - In Edatabuf",
    "XLP5 DATA BUFFER - In Edatabuf",
    "XLP6 DATA BUFFER - In Edatabuf",
    "XLP7 DATA BUFFER - In Edatabuf",
    "XLP8 DATA BUFFER - In Edatabuf",
    "XLP9 DATA BUFFER - In Edatabuf",
    "XLP10 DATA BUFFER - In Edatabuf",
    "XLP11 DATA BUFFER - In Edatabuf",
    "XLP12 DATA BUFFER - In Edatabuf",
    "XLP13 DATA BUFFER - In Edatabuf",
    "CLP0 DATA BUFFER - In Edatabuf",
    "CLP1 DATA BUFFER - In Edatabuf",
    "CLP2 DATA BUFFER - In Edatabuf",
    "CLP3 DATA BUFFER - In Edatabuf",
    "XLP RESI0 DATA BUFFER - In Edatabuf",
    "XLP RESI1 DATA BUFFER - In Edatabuf",
    "XLP RESI2 DATA BUFFER - In Edatabuf",
    "XLP RESI3 DATA BUFFER - In Edatabuf",
    "Invalid value",
    "Invalid value",
    "Invalid value",
    "Invalid value",
    "Invalid value",
    "Invalid value",
    "Invalid value",
    "Invalid value",
    "XLP0 EDB CTRL BUFFER - In Edatabuf",
    "XLP1 EDB CTRL BUFFER - In Edatabuf",
    "XLP2 EDB CTRL BUFFER - In Edatabuf",
    "XLP3 EDB CTRL BUFFER - In Edatabuf",
    "XLP4 EDB CTRL BUFFER - In Edatabuf",
    "XLP5 EDB CTRL BUFFER - In Edatabuf",
    "XLP6 EDB CTRL BUFFER - In Edatabuf",
    "XLP7 EDB CTRL BUFFER - In Edatabuf",
    "XLP8 EDB CTRL BUFFER - In Edatabuf",
    "XLP9 EDB CTRL BUFFER - In Edatabuf",
    "XLP10 EDB CTRL BUFFER - In Edatabuf",
    "XLP11 EDB CTRL BUFFER - In Edatabuf",
    "XLP12 EDB CTRL BUFFER - In Edatabuf",
    "XLP13 EDB CTRL BUFFER - In Edatabuf",
    "XLP14 EDB CTRL BUFFER - In Edatabuf",
    "XLP15 EDB CTRL BUFFER - In Edatabuf",
    "CLP0 EDB CTRL BUFFER - In Edatabuf",
    "CLP1 EDB CTRL BUFFER - In Edatabuf",
    "CLP2 EDB CTRL BUFFER - In Edatabuf",
    "CLP3 EDB CTRL BUFFER - In Edatabuf",
    "LEARN FIFO - In IARB",
    "EGR VLAN BUS",
    "EGR HCPM BUS",
    "EGR PMOD BUS",
    "EGR FPPARS BUS",
    "EFP BUS",
    "IVP BUS",
    "ICFG BUS",
    "Invalid value",
    "Invalid value",
    "Invalid value",
    "Invalid value",
    "Invalid value",
    "Invalid value",
    "Invalid value",
    "Invalid value",
    "Invalid value",
    "IPARS BUS",
    "IVXLT BUS",
    "IMPLS BUS",
    "IL2L3 BUS",
    "Invalid value",
    "IFP BUS",
    "IRSEL1 BUS",
    "ISW1 BUS",
    "IRSEL2 BUS"
};

#define _SOC_MONTEREY_SER_REG 1
#define _SOC_MONTEREY_SER_MEM 0

STATIC void
_soc_monterey_print_ser_fifo_details (int unit, uint8 regmem, soc_block_t blk,
                                    uint32 sblk, int pipe, uint32 address,
                                    uint32 stage, uint32 base, uint32 index,
                                    uint32 hwmbase, uint32 type, uint32 drop,
                                    uint32 non_sbus)
{
    uint32   hwmbase_max;

    if (bsl_check(bslLayerSoc, bslSourceCommon, bslSeverityError, unit)) {
        switch (type) {
        case 0:
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Error in: SOP cell.\n")));
            break;
        case 1:
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Error in: MOP cell.\n")));
            break;
        case 2:
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Error in: EOP cell.\n")));
            break;
        case 3:
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Error in: SBUS transaction.\n")));
            break;
        case 4:
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Error in: miscellaneous transaction.\n")));
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Invalid error reported !!\n")));
            break;
        }
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Blk: %d, Pipe: %d, Address: 0x%08x, base: 0x%x, stage: %d, index: %d\n"),
                              sblk, pipe, address, base, stage, index));
        if (regmem == _SOC_MONTEREY_SER_MEM) {
            if (hwmbase >= 0x80) {
                hwmbase -= 0x28; /* handle the gap */
            }
            if (non_sbus) {
                hwmbase_max = sizeof(_soc_monterey_ser_hwmem_base_info)/sizeof(_soc_monterey_ser_hwmem_base_info[0]);
                /* coverity[overrun-local] */
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Mem hwbase: 0x%x [%s]\n"), hwmbase,
                                      (hwmbase < hwmbase_max) ? _soc_monterey_ser_hwmem_base_info[hwmbase] : "--"));
            }
        }
        if (drop) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "SER caused packet drop.\n")));
        }
    }
}

#define IF_IPIPE_ARBITER_LOCK(_unit_, _blk_) \
    if (_blk_ == SOC_BLK_IPIPE) {\
        IP_ARBITER_LOCK(_unit_);\
    }

#define IF_IPIPE_ARBITER_UNLOCK(_unit_, _blk_) \
    if (_blk_ == SOC_BLK_IPIPE) {\
        IP_ARBITER_UNLOCK(_unit_);\
    }

static uint32 _monterey_ser_status_bus_fmt_get (uint32 *entry, int fld)
{
    switch(fld) {
    case VALIDf: /* Bit 1*/
        return (entry[0] & 0x00000001);
    case INSTRUCTION_TYPEf: /* Bits 1-4 */
        return ((entry[0] & 0x0000001E) >> 1);
    case HWMEMBASEf: /* Bits 23-30 */
    case MEMBASEf:
        return ((entry[0] & 0x7F800000) >> 23);
    case MEMINDEXf: /* Bits 5-22 */
        return ((entry[0] & 0x007FFFE0) >> 5);
    case REGINDEXf: /* Bits 5-12 */
        return ((entry[0] & 0x00001FE0) >> 5);
    case REGBASEf: /* Bits 13-29 */
        return ((entry[0] & 0x3FFFE000) >> 13);
    case PIPE_STAGEf: /* Bits 31-36 */
        return (((entry[0] & 0x80000000) >> 30) |
                ((entry[1] & 0x0000001F) << 1));
    case ADDRESSf: /* Bits 36-5 */
        return ((entry[0] >> 5) |
                ((entry[1] & 0x0000001F) << 27));
    case DROPf: /* Bit 37 */
        return (entry[1] & 0x00000020);
    case ECC_PARITYf: /* Bits 38-39 */
        return ((entry[1] & 0x000000C0) >> 6);
    case MEM_TYPEf:/* Bit 40 */
        return (entry[1] & 0x00000100);
    case NON_SBUSf: /* Bit 41 */
        return (entry[1] & 0x00000200);
    case MULTIPLEf: /* Bit 42 */
        return (entry[1] & 0x00000400);
    default:
        return -1;
    }
}

int
soc_monterey_mem_index_remap(int unit, int index, soc_mem_t *mem)
{
    int logical_index = -1;
    switch (*mem) {
        case L3_DEFIPm:
        case L3_DEFIP_PAIR_128m:
        case L3_DEFIP_ONLYm:
        case L3_DEFIP_PAIR_128_ONLYm:
        case L3_DEFIP_DATA_ONLYm:
        case L3_DEFIP_PAIR_128_DATA_ONLYm:
            logical_index =  soc_monterey_l3_defip_index_remap(unit, *mem, index);
            if (logical_index == -1) {
                /* This case arises when the passed index (physical) belongs
                 * to wider view, but the passed mem is narrow view
                 * Note that below API rewrites mem to the correct (wide) view.
                 */
                logical_index = soc_monterey_l3_defip_mem_index_get(unit, index, mem);
            }
            return logical_index;
        default:
            return index;
    }
}

STATIC uint32
_soc_monterey_flex_ctr_addr_check (int unit, uint32 address, soc_block_t blk,
                                 uint32 stage, uint32 index)
{
    uint32 index2 = index;

    
    if ((SOC_BLK_IPIPE == blk) && (22 == stage)) {
        switch (address & 0xfffff000) {
            /* ING_FLEX_CTR_OFFSET_TABLE_*m */
        case 0x56800000:
        case 0x56801000:
        case 0x56802000:
        case 0x56803000:
        case 0x56840000:
        case 0x56841000:
        case 0x56842000:
        case 0x56843000:
            index2 = address & 0x3ff; break; /* 1K depth */
            /* ING_FLEX_CTR_COUNTER_TABLE_*m */
        case 0x56804000:
        case 0x56808000:
        case 0x5680c000:
        case 0x56810000:
        case 0x56844000:
        case 0x56848000:
        case 0x5684c000:
        case 0x56850000:
            index2 = address & 0xfff; break; /* 4K depth */
        default:
            break;
        }
    }

    if ((SOC_BLK_EPIPE == blk) && (10 == stage)) {
        switch (address & 0XFFFFF000) {
        case 0x2a800000:
        case 0x2a801000:
        case 0x2a802000:
        case 0x2a803000:
            index2 = address & 0x3ff; break; /* 1K depth */
        case 0x2a804000:
        case 0x2a808000:
        case 0x2a80c000:
        case 0x2a810000:
            index2 = address & 0xfff; break; /* 4K depth */
        default:
            break;
        }
    }
    return index2;
}

STATIC int
_soc_monterey_process_ser_fifo (int unit, soc_block_t blk, char *prefix_str)
{
    int            rv;
    uint8          bidx;
    soc_mem_t      mem;
    char           blk_str[10];
    void          *ser_info_table = NULL;
    uint32         entry[SOC_MAX_MEM_WORDS], address = 0;
    uint32         reg_val = 0, mask = 0;
    uint32         stage = 0, addrbase = 0, index = 0, hwmbase = 0, type = 0;
    uint32         sblk = 0, regmem = 0, non_sbus = 0, drop = 0, ecc_parity = 0;
    soc_reg_t      parity_enable_reg = INVALIDr;
    soc_field_t    parity_enable_field = INVALIDf;
    _soc_ser_correct_info_t spci;
    soc_reg_t reg = INVALIDr;

    switch (blk) {
    case SOC_BLK_IPIPE:
        mem = ING_SER_FIFOm;
        ser_info_table = _soc_bcm56670_a0_ip_mem_ser_info;
        sal_sprintf(blk_str, "IPIPE");
        break;
    case SOC_BLK_EPIPE:
        mem = EGR_SER_FIFOm;
        ser_info_table = _soc_bcm56670_a0_ep_mem_ser_info;
        mask = 0x00000001; /* SER_FIFO_NON_EMPTYf */
        reg = EGR_INTR_STATUSr;
        sal_sprintf(blk_str, "EPIPE");
        break;
    default:
        return SOC_E_PARAM;
    }

    IF_IPIPE_ARBITER_LOCK(unit, blk);
    do {
        sal_memset(&spci, 0, sizeof(spci));
        rv = soc_mem_pop(unit, mem, MEM_BLOCK_ANY, entry);
        if (rv != SOC_E_NONE) {
            IF_IPIPE_ARBITER_UNLOCK(unit, blk);
        }
        SOC_IF_ERROR_RETURN(rv);
        /* process entry */
        if (_monterey_ser_status_bus_fmt_get(entry,VALIDf)) {
            ecc_parity = _monterey_ser_status_bus_fmt_get(entry, ECC_PARITYf);
            regmem = _monterey_ser_status_bus_fmt_get(entry, MEM_TYPEf);
            address = _monterey_ser_status_bus_fmt_get(entry, ADDRESSf);
            stage = _monterey_ser_status_bus_fmt_get(entry, PIPE_STAGEf);
            type = _monterey_ser_status_bus_fmt_get(entry, INSTRUCTION_TYPEf);
            drop = _monterey_ser_status_bus_fmt_get(entry, DROPf);
            SOC_BLOCK_ITER(unit, bidx, blk) {
                sblk = SOC_BLOCK2SCH(unit, bidx);
                break;
            }
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "%s\n"), prefix_str));
            if (_monterey_ser_status_bus_fmt_get(entry, MULTIPLEf)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Multiple: ")));
            }
            if (regmem == _SOC_MONTEREY_SER_REG ) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Reg: ")));
            } else {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Mem: ")));
            }
            spci.double_bit = 0;
            switch (ecc_parity) {
            case 0:
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Parity error..\n")));
                 spci.parity_type = SOC_PARITY_TYPE_PARITY;
                break;

            case 1:
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Corrected single bit ECC error..\n")));
                 spci.parity_type = SOC_PARITY_TYPE_ECC;
                break;
            case 2:
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Double or Multiple bit ECC error..\n")));

                spci.parity_type = SOC_PARITY_TYPE_ECC;
                spci.double_bit = 1;
                spci.flags |= SOC_SER_ERR_MULTI;

                break;
            default:
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Invalid SER issue !!\n")));

                IF_IPIPE_ARBITER_UNLOCK(unit, blk);
                return SOC_E_INTERNAL;
            }
            if (regmem == _SOC_MONTEREY_SER_MEM) {
                /* process mem */
                non_sbus = _monterey_ser_status_bus_fmt_get(entry, NON_SBUSf);
                addrbase = _monterey_ser_status_bus_fmt_get(entry, MEMBASEf);
                index = _monterey_ser_status_bus_fmt_get(entry, MEMINDEXf);
                hwmbase = _monterey_ser_status_bus_fmt_get(entry, HWMEMBASEf);


                index = _soc_monterey_flex_ctr_addr_check(unit, address, blk,
                                                        stage, index);
                if (non_sbus == 0) {
                    soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                       ecc_parity == 0 ?
                                       SOC_SWITCH_EVENT_DATA_ERROR_PARITY :
                                       SOC_SWITCH_EVENT_DATA_ERROR_ECC,
                                       sblk | SOC_SER_ERROR_DATA_BLK_ADDR_SET,
                                       address);
                    _soc_monterey_print_ser_fifo_details(unit, 0, blk, sblk, 0,
                                                       address, stage, addrbase,
                                                       index, hwmbase, type,
                                                       drop, non_sbus);
                    spci.flags = SOC_SER_SRC_MEM;
                    spci.reg = INVALIDr;
                    spci.mem = INVALIDm;
                    spci.blk_type = blk;
                    spci.sblk = sblk;
                    spci.addr = address - index;
                    spci.index = index;
                    spci.stage = stage;
                    spci.detect_time = sal_time_usecs();
                    spci.mem = soc_addr_to_mem_extended(unit,spci.sblk,
                                                        -1,spci.addr);
                    if (spci.mem != INVALIDm) {
                        spci.acc_type = _SOC_ACC_TYPE_PIPE_ANY;
                        spci.flags |= SOC_SER_REG_MEM_KNOWN;
                        spci.index = soc_monterey_mem_index_remap(unit, index, &(spci.mem));
                    }
                    else {
                        /* Invalid mem, use the first acc_type_ptr*/
                        spci.acc_type = _SOC_ACC_TYPE_PIPE_ANY;
                        spci.flags |= SOC_SER_REG_MEM_UNKNOWN;
                    }

                    if (spci.mem != INVALIDm) {
                        _soc_monterey_ser_control_reg_get(unit, ser_info_table,
                            spci.mem,
                            &parity_enable_reg,
                            &parity_enable_field);
                    }
                    spci.flags |= SOC_SER_LOG_WRITE_CACHE;
                    spci.log_id = _soc_monterey_populate_ser_log(unit,
                                              parity_enable_reg,
                                              parity_enable_field,
                                              spci.mem,
                                              bidx,
                                              spci.acc_type,
                                              spci.index,
                                              spci.detect_time,
                                              spci.sblk,
                                              spci.addr);

                    rv = soc_ser_correction(unit, &spci);
                    if (SOC_FAILURE(rv)) {
                        if (rv != SOC_E_NOT_FOUND) {
                            IF_IPIPE_ARBITER_UNLOCK(unit, blk);
                            /* Add reporting failed to correct event flag to
                             * application */
                            soc_event_generate(unit,
                                    SOC_SWITCH_EVENT_PARITY_ERROR,
                                    SOC_SWITCH_EVENT_DATA_ERROR_FAILEDTOCORRECT,
                                    sblk |
                                    SOC_SER_ERROR_DATA_BLK_ADDR_SET, address);
                            return rv;
                        }
                    }
                    if (spci.log_id != 0) {
                        soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                           SOC_SWITCH_EVENT_DATA_ERROR_LOG,
                                           spci.log_id, 0);
                    }

                } else {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "%s SER mem address un-accessable !!\n"), blk_str));
                    soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                       SOC_SWITCH_EVENT_DATA_ERROR_UNCORRECTABLE, 0, 0);
                    _soc_monterey_print_ser_fifo_details(unit, 0, blk, sblk, 0, address,
                                                         stage, addrbase, index, hwmbase,
                                                         type, drop, non_sbus);
                }
            } else {
                /* process reg */
                non_sbus = _monterey_ser_status_bus_fmt_get(entry, NON_SBUSf);
                addrbase = _monterey_ser_status_bus_fmt_get(entry, REGBASEf);
                index = _monterey_ser_status_bus_fmt_get(entry, REGINDEXf);

                if (non_sbus == 0) {
                    soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                       ecc_parity == 0 ?
                                       SOC_SWITCH_EVENT_DATA_ERROR_PARITY :
                                       SOC_SWITCH_EVENT_DATA_ERROR_ECC,
                                       sblk |
                                       SOC_SER_ERROR_DATA_BLK_ADDR_SET, address);
                    _soc_monterey_print_ser_fifo_details(unit, 1, blk, sblk, 0,
                                                       address, stage, addrbase,
                                                       index, 0, type, drop,
                                                       non_sbus);
                    spci.flags = SOC_SER_SRC_REG | SOC_SER_REG_MEM_UNKNOWN;
                    spci.reg = INVALIDr;
                    spci.mem = INVALIDm;
                    spci.blk_type = blk;
                    spci.sblk = sblk;
                    spci.addr = address;
                    spci.index = index;
                    spci.stage = stage;
                    spci.detect_time = sal_time_usecs();
                    spci.log_id = soc_ser_log_create_entry(unit,
                            sizeof(soc_ser_log_tlv_generic_t) +
                            sizeof(soc_ser_log_tlv_register_t) +
                            sizeof(soc_ser_log_tlv_hdr_t) *3);
                    rv = soc_ser_correction(unit, &spci);
                    if (SOC_FAILURE(rv)) {
                        IF_IPIPE_ARBITER_UNLOCK(unit, blk);
                        /* Add reporting failed to correct event flag to
                         * application */
                        soc_event_generate(unit,
                                    SOC_SWITCH_EVENT_PARITY_ERROR,
                                    SOC_SWITCH_EVENT_DATA_ERROR_FAILEDTOCORRECT,
                                    sblk |
                                    SOC_SER_ERROR_DATA_BLK_ADDR_SET, address);
                        return rv;
                    }
                    if (spci.log_id != 0) {
                        soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                           SOC_SWITCH_EVENT_DATA_ERROR_LOG,
                                           spci.log_id, 0);
                    }
                } else {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "%s SER reg address un-accessable !!\n"), blk_str));
                    soc_event_generate(unit, SOC_SWITCH_EVENT_PARITY_ERROR,
                                       SOC_SWITCH_EVENT_DATA_ERROR_UNCORRECTABLE, 0, 0);
                    _soc_monterey_print_ser_fifo_details(unit, 0, blk, sblk, 0,
                                                       address, stage, addrbase,
                                                       index, hwmbase, type,
                                                       drop, non_sbus);
                }
            }
        } else {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "unit %d Got invalid mem pop from %s !!\n"),
                                  unit, SOC_MEM_NAME(unit, mem)));
        }

        if (rv != SOC_E_NONE) {
            IF_IPIPE_ARBITER_UNLOCK(unit, blk);
        }

        /* check if any more pending */
        if (reg == INVALIDr) {
            rv = READ_CMIC_CMC0_IRQ_STAT2r(unit, &reg_val);
        } else {
            rv = soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val);
        }
        SOC_IF_ERROR_RETURN(rv);
    } while (reg_val & mask);
    IF_IPIPE_ARBITER_UNLOCK(unit, blk);

    return SOC_E_NONE;
}

STATIC int
_soc_monterey_ser_process_irq3 (int unit, int bit_pos)
{
    uint8      rbi;
    int        port = REG_PORT_ANY;
    uint32     cmic_rval, cmic_bit;
    uint64     rb_enable64, rb_rval64, tmp64;
    const      _soc_monterey_ser_route_block_t *rb;
    char       prefix_str[10];
    int        block_info_idx;
    soc_stat_t *stat = SOC_STAT(unit);
    int ser_error = 0;
    int pgw_inst = 0 ;
    int pgw = 0;
    sal_sprintf(prefix_str, "Unit: %d \n", unit);

    /* Read CMIC parity status register */
    SOC_IF_ERROR_RETURN
        (READ_CMIC_CMC0_IRQ_STAT3r(unit, &cmic_rval));
    if (cmic_rval == 0) {
        return SOC_E_NONE;
    }
    /* Loop through each place-and-route block entry */
    for (rbi = 0; ; rbi++) {
        rb = &_soc_monterey_ser_route_blocks_irq3[rbi];
        cmic_bit = rb->cmic_bit;
        if (cmic_bit == 0) {
            /* End of table */
            break;
        }
        if (cmic_bit != (1 << bit_pos)) {
            continue;
        }
        if (!(cmic_rval & cmic_bit)) {
            /* No interrupt bit asserted for the route block */
            continue;
        }
        if (rb->blocktype == SOC_BLK_IPIPE || rb->blocktype == SOC_BLK_EPIPE) {
            /* New fifo style processing */
            (void)_soc_monterey_process_ser_fifo(unit, rb->blocktype, prefix_str);
            stat->ser_err_fifo++;
        } else {
            /* Legacy processing */

            SOC_BLOCK_ITER(unit, block_info_idx, rb->blocktype) {
                if (SOC_BLOCK_INFO(unit, block_info_idx).number == rb->id) {
                    port = SOC_BLOCK_PORT(unit, block_info_idx);
                    break;
                }
            }
            if (SOC_BLOCK_IN_LIST(unit, SOC_REG_INFO(unit, rb->enable_reg).block,
                SOC_BLK_PORT) && (port == REG_PORT_ANY)) {
                    /* This port block is not configured */
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "unit %d SER error on disabled port block %d !!\n"),
                                          unit, block_info_idx));
                    sal_usleep(SAL_BOOT_QUICKTURN ? 10000000 : 1000000); /* Don't reenable too soon */
                    continue;
            }
            if(rb->blocktype == SOC_BLK_PGW_CL) {
              for ( pgw = 0; pgw <= 1; pgw++)
              {
                   pgw_inst = (pgw | SOC_REG_ADDR_INSTANCE_MASK);
                   SOC_IF_ERROR_RETURN
                   (soc_reg_get(unit, rb->status_reg, pgw_inst, 0, &rb_rval64));
                   if (!COMPILER_64_IS_ZERO(rb_rval64)) {
                       ser_error = 1;
                       break;
                   }
              }
              if(!ser_error)
              {
                 continue;
              }

            } else {
            /* Read per route block parity status register */
               SOC_IF_ERROR_RETURN
                  (soc_reg_get(unit, rb->status_reg, port, 0, &rb_rval64));
               if (COMPILER_64_IS_ZERO(rb_rval64)) {
                   continue;
               }
           }
           SOC_IF_ERROR_RETURN
                (_soc_monterey_process_ser(unit, block_info_idx, rb->id, -1,
                                         port, rb->status_reg, rb_rval64,
                                         rb->info, prefix_str));

            SOC_IF_ERROR_RETURN
                (soc_reg_get(unit, rb->enable_reg, port, 0, &rb_enable64));
            COMPILER_64_SET(tmp64, COMPILER_64_HI(rb_rval64), COMPILER_64_LO(rb_rval64));
            COMPILER_64_NOT(tmp64);
            COMPILER_64_AND(rb_enable64, tmp64);
            SOC_IF_ERROR_RETURN
                (soc_reg_set(unit, rb->enable_reg, port, 0, rb_enable64));
            COMPILER_64_OR(rb_enable64, rb_rval64);
            SOC_IF_ERROR_RETURN
                (soc_reg_set(unit, rb->enable_reg, port, 0, rb_enable64));

            stat->ser_err_int++;
        }
    }
    return SOC_E_NONE;
}

STATIC int
_soc_monterey_ser_process_irq4 (int unit, int bit_pos)
{
    uint8      rbi;
    int        port = REG_PORT_ANY;
    uint32     cmic_rval, cmic_bit;
    uint64     rb_enable64, rb_rval64, tmp64;
    char       prefix_str[10];
    int        block_info_idx;
    soc_stat_t *stat = SOC_STAT(unit);

    const _soc_monterey_ser_route_block_t *rb;

    sal_sprintf(prefix_str, "\nUnit: %d: ", unit);

    /* Read CMIC parity status register */
    SOC_IF_ERROR_RETURN
        (READ_CMIC_CMC0_IRQ_STAT4r(unit, &cmic_rval));
    if (cmic_rval == 0) {
        return SOC_E_NONE;
    }

    /* Loop through each place-and-route block entry */
    for (rbi = 0; ; rbi++) {
            rb = &_soc_monterey_ser_route_blocks_irq4[rbi];
        cmic_bit = rb->cmic_bit;
        if (cmic_bit == 0) {
            /* End of table */
            break;
        }
        if (cmic_bit != (1 << bit_pos)) {
            continue;
        }
        if (!(cmic_rval & cmic_bit)) {
            /* No interrupt bit asserted for the route block */
            continue;
        }

        SOC_BLOCK_ITER(unit, block_info_idx, rb->blocktype) {
            if (SOC_BLOCK_INFO(unit, block_info_idx).number == rb->id) {
                port = SOC_BLOCK_PORT(unit, block_info_idx);
                break;
            }
        }

        if (SOC_BLOCK_IN_LIST(unit, SOC_REG_INFO(unit, rb->enable_reg).block,
                              SOC_BLK_PORT) && (port == REG_PORT_ANY)) {
            /* This port block is not configured */
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "unit %d SER error on disabled port block %d !!\n"),
                       unit, block_info_idx));
            sal_usleep(SAL_BOOT_QUICKTURN ? 10000000 : 1000000); /* Don't reenable too soon */
            continue;
        }

        /* Read per route block parity status register */
        SOC_IF_ERROR_RETURN
            (soc_reg_get(unit, rb->status_reg, port, 0, &rb_rval64));
        if (COMPILER_64_IS_ZERO(rb_rval64)) {
            continue;
        }

        SOC_IF_ERROR_RETURN
            (_soc_monterey_process_ser(unit, block_info_idx, rb->id, -1,
                                     port, rb->status_reg, rb_rval64,
                                     rb->info, prefix_str));
        SOC_IF_ERROR_RETURN
            (soc_reg_get(unit, rb->enable_reg, port, 0, &rb_enable64));
        COMPILER_64_SET(tmp64, COMPILER_64_HI(rb_rval64), COMPILER_64_LO(rb_rval64));
        COMPILER_64_NOT(tmp64);
        COMPILER_64_AND(rb_enable64, tmp64);
        SOC_IF_ERROR_RETURN
            (soc_reg_set(unit, rb->enable_reg, port, 0, rb_enable64));
        COMPILER_64_OR(rb_enable64, rb_rval64);
        SOC_IF_ERROR_RETURN
            (soc_reg_set(unit, rb->enable_reg, port, 0, rb_enable64));

        stat->ser_err_int++;
    }

    return SOC_E_NONE;
}

#define IRQ3_STATUS_REG 3
#define IRQ4_STATUS_REG 4
STATIC int
_soc_monterey_ser_process_all (int unit, int top_irq, int bit_pos)
{
    switch (top_irq) {
    case IRQ3_STATUS_REG:
        SOC_IF_ERROR_RETURN(_soc_monterey_ser_process_irq3(unit,bit_pos));
        break;
    case IRQ4_STATUS_REG:
        SOC_IF_ERROR_RETURN(_soc_monterey_ser_process_irq4(unit,bit_pos));
        break;
    default:
        return SOC_E_INTERNAL; /* Shouldn't come here */
    }

    return SOC_E_NONE;
}

void
soc_monterey_ser_set_long_delay(int delay)
{
    _monterey_ser_delay = delay;
    return;
}

STATIC void
soc_monterey_ser_error (void *unit_vp, void *d1,
                      void *d2, void *d3, void *d4)
{
    int unit     = PTR_TO_INT(unit_vp);
    int irq_reg  = PTR_TO_INT(d3);
    int old_mask = 0;

    (void)_soc_monterey_ser_process_all(unit,irq_reg,PTR_TO_INT(d4));

    sal_usleep(SAL_BOOT_QUICKTURN ? 10000000 : (_monterey_ser_delay != 0 ? 10000000 : 100000));

    if (d2 != NULL) {
        old_mask = PTR_TO_INT(d2);
        if (irq_reg == 3) {
            (void)soc_cmicm_intr3_enable(unit, old_mask);
        } else if (irq_reg == 4) {
            (void)soc_cmicm_intr4_enable(unit, old_mask);
        }
    }
    /* soc_intr_enable(unit, IRQ_MEM_FAIL); */
}

/* SER processing for TCAMs */
static _soc_generic_ser_info_t _soc_monterey_tcam_ser_info_template[] = {
    /* HW SER engine protection */
    { L3_DEFIPm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_4BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 97}, {1, 97}, {98, 193}, {99, 193} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_NO_DMA |
      _SOC_SER_FLAG_REMAP_READ  | _SOC_SER_FLAG_SIZE_VERIFY},
    { L3_DEFIP_PAIR_128m, INVALIDm, _SOC_SER_TYPE_PARITY,
      _SOC_SER_PARITY_8BITS,
      _SOC_SER_INTERLEAVE_NONE,
      { {0, 99}, {100, 195}, {196, 291}, {292, 387} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_NO_DMA |
      _SOC_SER_FLAG_REMAP_READ  | _SOC_SER_FLAG_SIZE_VERIFY},
    { FP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_4BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 165}, {1, 165}, {166, 353}, {167, 353} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_XY_READ},
    { EFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_4BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 237}, {1, 237}, {238, 473}, {239, 473} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_NO_DMA},
    { VFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_4BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 235}, {1, 235}, {236, 469}, {237, 469} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_NO_DMA},
    { ING_SNATm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_4BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_XY_READ},
    { L3_TUNNELm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_4BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 145}, {2, 145}, {146, 289}, {148, 289} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_XY_READ},
    { L2_USER_ENTRYm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_4BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_XY_READ},

    { VLAN_SUBNETm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_4BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_XY_READ},
    { MY_STATION_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY,
      _SOC_SER_PARITY_4BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_XY_READ},
    { FP_UDF_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_4BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_XY_READ},
    { CPU_COS_MAPm, INVALIDm, _SOC_SER_TYPE_PARITY, _SOC_SER_PARITY_4BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 144}, {1, 144}, {145, 288}, {146, 288} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_XY_READ},
    { IP_MULTICAST_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY,
      _SOC_SER_PARITY_4BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 144}, {1, 144}, {145, 288}, {146, 288} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_XY_READ},

    /* SW memscan SER engine protection */
    { UDF_CONDITIONAL_CHECK_TABLE_CAMm, INVALIDm, _SOC_SER_TYPE_PARITY,
      _SOC_SER_PARITY_4BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 64}, {1, 64}, {65, 128}, {66, 128} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_XY_READ},
    { MY_STATION_TCAM_2m, INVALIDm, _SOC_SER_TYPE_PARITY,
      _SOC_SER_PARITY_4BITS,
      _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_XY_READ},
     /* Below memories have SW memscan SER engine protection */
    { FP_GLOBAL_MASK_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY,
      -1, -1,
      { {0, 177}, {2, 177}, {178, 353}, {179, 353} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_SW_COMPARE |
      _SOC_SER_FLAG_OVERLAY},
    { FP_GM_FIELDSm, INVALIDm, _SOC_SER_TYPE_PARITY,
      -1, -1,
      { {0, 177}, {2, 177}, {178, 353}, {179, 353} }, 0, 0, 0, 0,
         _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_SW_COMPARE |
      _SOC_SER_FLAG_OVERLAY | _SOC_SER_FLAG_OVERLAY_CASE},
#ifdef INCLUDE_XFLOW_MACSEC
    { ISEC_SP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY,
      -1, -1,
      { {0, 239}, {1, 239}, {240, 480}, {241, 480} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_SW_COMPARE
      },
    { ISEC_SC_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY,
      -1, -1,
      { {0, 75}, {2, 75}, {76,153 }, {78, 153} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_SW_COMPARE
      },
#endif
    { INVALIDm },
};

static _soc_generic_ser_info_t *_soc_monterey_tcam_ser_info[SOC_MAX_NUM_DEVICES];

STATIC int
_soc_monterey_tcam_ser_init (int unit)
{
    int alloc_size;

    /* First, make per-unit copy of the master TCAM list */
    alloc_size = sizeof(_soc_monterey_tcam_ser_info_template);
    if (NULL == _soc_monterey_tcam_ser_info[unit]) {
        if ((_soc_monterey_tcam_ser_info[unit] =
             sal_alloc(alloc_size, "monterey tcam list")) == NULL) {
            return SOC_E_MEMORY;
        }
    }

    /* Make a fresh copy of the TCAM template info */
    sal_memcpy(_soc_monterey_tcam_ser_info[unit],
               &(_soc_monterey_tcam_ser_info_template),
               alloc_size);

    return soc_generic_ser_init(unit, _soc_monterey_tcam_ser_info[unit]);
}

STATIC void
soc_monterey_ser_fail (int unit)
{
    soc_generic_ser_process_error(unit, _soc_monterey_tcam_ser_info[unit],
                                  _SOC_PARITY_TYPE_SER);
}


int
soc_monterey_mem_config(int unit)
{
    soc_persist_t *sop;
    int l2_entries, cfg_l2_entries;
    int l3_entries, cfg_l3_entries;
    int min_l2_entries;
    int min_l3_entries;
    int max_l2_entries;
    int max_l3_entries;
    int num_ecmp_rh_flowset_entries;
    int num_ipv6_128b_entries = 0;
    int config_v6_entries = 0;
    int defip_config = 0;
    uint16              dev_id;
    uint8               rev_id;

    sop = SOC_PERSIST(unit);
    soc_cm_get_id(unit, &dev_id, &rev_id);

    min_l2_entries = 32; /* 32K dedicated L2 entries */
    min_l3_entries = 8; /* 8K dedicated L3 entries */
    max_l2_entries = 32; /* Dedicated (32K) + Shared (0)*/
    max_l3_entries = 8; /* Dedicated (8K) + Shared (0)*/

    if (SAL_BOOT_SIMULATION) {
        /*
         * Smaller table lengths for BCMSIM environment.
         */
        max_l2_entries = min_l2_entries;
        max_l3_entries = min_l3_entries;
    }

    cfg_l2_entries = soc_property_get(unit, spn_L2_MEM_ENTRIES, -1);
    l2_entries = cfg_l2_entries == -1 ? max_l2_entries * 1024 : cfg_l2_entries;

    if (l2_entries <= min_l2_entries * 1024) { /* Min table size */
        l2_entries = min_l2_entries * 1024;
    } else if (l2_entries <= max_l2_entries * 1024) {
        /*
         * COVERITY
         * Future chips may have different min, max. Hence not removing it.
         */
        /* coverity[dead_error_line] */
        l2_entries = min_l2_entries * 1024  ;
    } else {
        LOG_CLI((BSL_META_U(unit,
                            "The specified l2_mem_entries (%d) exceeds 272K\n"),
                            cfg_l2_entries));
        return SOC_E_PARAM;
    }

    cfg_l3_entries = soc_property_get(unit, spn_L3_MEM_ENTRIES, -1);
    l3_entries = cfg_l3_entries == -1 ? (min_l3_entries * 1024 ) : cfg_l3_entries;
    if (l3_entries <= min_l3_entries * 1024) { /* 4k dedicated L3 entries */
        l3_entries = min_l3_entries * 1024;
    } else if (l3_entries <= max_l3_entries * 1024) {
        /*
         * COVERITY
         * Future chips may have different min, max. Hence not removing it.
         */
        /* coverity[dead_error_line] */
        l3_entries = min_l3_entries * 1024 ;
    } else {
        LOG_CLI((BSL_META_U(unit,
                            "The specified l3_mem_entries (%d) exceeds 260K\n"),
                            cfg_l3_entries));
        return SOC_E_PARAM;
    }



    /* Adjust L2 table size */
    sop->memState[L2Xm].index_max = l2_entries - 1;
    sop->memState[L2_ENTRY_ONLYm].index_max = l2_entries - 1;
    sop->memState[L2_ENTRY_ONLY_ECCm].index_max = l2_entries - 1;
    sop->memState[L2_HITDA_ONLYm].index_max = l2_entries / 4 - 1;
    sop->memState[L2_HITSA_ONLYm].index_max = l2_entries / 4 - 1;
    sop->memState[L2_ENTRY_LPm].index_max = min_l2_entries*1024 / 4 - 1;
    sop->memState[L2_ENTRY_ISS_LPm].index_max =
        (l2_entries - min_l2_entries*1024) / 4 - 1;

    /* Adjust L3 table size */
    sop->memState[L3_ENTRY_ONLYm].index_max = l3_entries - 1;
    sop->memState[L3_ENTRY_IPV4_UNICASTm].index_max = l3_entries - 1;
    sop->memState[L3_ENTRY_IPV4_MULTICASTm].index_max = l3_entries / 2 - 1;
    sop->memState[L3_ENTRY_IPV6_UNICASTm].index_max = l3_entries/ 2 - 1;
    sop->memState[L3_ENTRY_IPV6_MULTICASTm].index_max = l3_entries / 4 - 1;
    sop->memState[L3_ENTRY_ONLY_ECCm].index_max = l3_entries - 1;
    sop->memState[L3_ENTRY_HIT_ONLYm].index_max = l3_entries / 4 - 1;
    sop->memState[L3_ENTRY_LPm].index_max = min_l3_entries*1024 / 4 - 1;
    sop->memState[L3_ENTRY_ISS_LPm].index_max =
        (l3_entries - min_l3_entries*1024) / 4 - 1;


    /* LAG and ECMP resilient hashing features share the same flow set table.
     * The table can be configured in one of 3 modes:
     * - dedicated to LAG resilient hashing,
     * - dedicated to ECMP resilient hashing,
     * - split evenly between LAG and ECMP resilient hashing.
     */
    num_ecmp_rh_flowset_entries = soc_property_get(unit,
            spn_ECMP_RESILIENT_HASH_SIZE, 512);
    switch (num_ecmp_rh_flowset_entries) {
        case 0:
            sop->memState[RH_ECMP_FLOWSETm].index_max = -1;
            break;
        case 512:
            sop->memState[RH_LAG_FLOWSETm].index_max /= 2;
            sop->memState[RH_ECMP_FLOWSETm].index_max /= 2;
            break;
        case 1024:
            sop->memState[RH_LAG_FLOWSETm].index_max = -1;
            break;
        default:
            return SOC_E_CONFIG;
    }

    if (!soc_feature(unit, soc_feature_lpm_tcam)) {
        SOC_CONTROL(unit)->l3_defip_max_tcams = 0;
    } else if (soc_feature(unit, soc_feature_l3_2k_defip_table)) {
        SOC_CONTROL(unit)->l3_defip_max_tcams = 2;
    } else {
        SOC_CONTROL(unit)->l3_defip_max_tcams = _SOC_MONTEREY_DEFIP_MAX_TCAMS;
    }
    SOC_CONTROL(unit)->l3_defip_tcam_size = _SOC_MONTEREY_DEFIP_TCAM_DEPTH;

    if (!soc_feature(unit, soc_feature_lpm_tcam)) {
        /* LPM is disabled, disable all L3_DEFIP memories */
        sop->memState[L3_DEFIPm].index_max = -1;
        sop->memState[L3_DEFIP_PAIR_128m].index_max = -1;
        sop->memState[L3_DEFIP_PAIR_128_ONLYm].index_max = -1;
        sop->memState[L3_DEFIP_PAIR_128_DATA_ONLYm].index_max = -1;
        sop->memState[L3_DEFIP_PAIR_128_HIT_ONLYm].index_max = -1;
        sop->memState[L3_DEFIP_ONLYm].index_max = -1;
        sop->memState[L3_DEFIP_DATA_ONLYm].index_max = -1;
        sop->memState[L3_DEFIP_HIT_ONLYm].index_max = -1;
        SOC_CONTROL(unit)->l3_defip_index_remap = 0;
    } else if (soc_property_get(unit, "l3_defip_sizing", TRUE)) {
        if (!soc_property_get(unit, spn_L3_ALPM_ENABLE, 0)) {
            defip_config = soc_property_get(unit, spn_IPV6_LPM_128B_ENABLE, 1);

            num_ipv6_128b_entries = soc_property_get(unit,
                                        spn_NUM_IPV6_LPM_128B_ENTRIES,
                                        (defip_config ? 128 : 0));
            num_ipv6_128b_entries = num_ipv6_128b_entries +
                                    (num_ipv6_128b_entries % 2);

            config_v6_entries = num_ipv6_128b_entries;
            if (soc_property_get(unit, spn_LPM_SCALING_ENABLE, 0)) {
                num_ipv6_128b_entries = 0;
                if (!soc_property_get(unit, spn_LPM_IPV6_128B_RESERVED, 1)) {
                    config_v6_entries = ((config_v6_entries /
                                        SOC_CONTROL(unit)->l3_defip_tcam_size) +
                                        ((config_v6_entries %
                                         SOC_CONTROL(unit)->l3_defip_tcam_size)
                                        ? 1 : 0)) * SOC_CONTROL(unit)->l3_defip_tcam_size;
                }
            }
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
        } else {
            if (soc_property_get(unit, spn_IPV6_LPM_128B_ENABLE, 1)) {
                /* slightly different processing for v6-128 */
                num_ipv6_128b_entries = soc_property_get(unit,
                                            spn_NUM_IPV6_LPM_128B_ENTRIES,
                                            128);
                num_ipv6_128b_entries = num_ipv6_128b_entries +
                                        (num_ipv6_128b_entries % 2);
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

                SOC_CONTROL(unit)->l3_defip_index_remap =
                                              (num_ipv6_128b_entries / 2) * 2;
            } else {
                sop->memState[L3_DEFIP_PAIR_128m].index_max = -1;
                sop->memState[L3_DEFIP_PAIR_128_ONLYm].index_max = -1;
                sop->memState[L3_DEFIP_PAIR_128_DATA_ONLYm].index_max = -1;
                sop->memState[L3_DEFIP_PAIR_128_HIT_ONLYm].index_max = -1;
            }
        }
        soc_l3_defip_indexes_init(unit, config_v6_entries);
    }

    if (NULL == _soc_monterey_alpm_bkt_view_map[unit]) {
        if ((_soc_monterey_alpm_bkt_view_map[unit] =
             sal_alloc(sizeof(soc_mem_t) * SOC_MONTEREY_ALPM_MAX_BKTS,
                       "alpm_bkt_map")) == NULL) {
            return SOC_E_MEMORY;
        }
    }

    memset(_soc_monterey_alpm_bkt_view_map[unit], INVALIDm,
           sizeof(soc_mem_t) * SOC_MONTEREY_ALPM_MAX_BKTS);

    return SOC_E_NONE;
}

int soc_monterey_get_alpm_banks(int unit)
{
    int rv = SOC_E_NONE;
    soc_reg_t reg = ISS_LOG_TO_PHY_BANK_MAP_2r;
    uint32 bank_mode = 0;
    uint32 map = 0;

    if (_soc_mn_num_shared_alpm_banks[unit] == 0) {
        rv = soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &map);
        if (rv >= 0) {
            bank_mode = soc_reg_field_get(unit, reg, map, ALPM_BANK_MODEf);
        }

        if (bank_mode == 1) {
            _soc_mn_num_shared_alpm_banks[unit] = 2;
        } else {
            _soc_mn_num_shared_alpm_banks[unit] = 4;
        }
    }

    return _soc_mn_num_shared_alpm_banks[unit];
}

int
soc_monterey_alpm_mode_get(int unit)
{
    return _soc_alpm_mode[unit];
}

#define _MONTEREY_INDEX_1K_ENTRIES  1024
#define _MONTEREY_INDEX_2K_ENTRIES  2048
#define _MONTEREY_INDEX_3K_ENTRIES  3072
#define _MONTEREY_INDEX_4K_ENTRIES  4096
#define _MONTEREY_INDEX_6K_ENTRIES  6144

/* Map logical (always starts from 0 and contiguous) index to physical index
   which can have a starting offset and/or holes.
   Input  : logical index
   Returns: physical index */
int
soc_monterey_l3_defip_index_map (int unit, soc_mem_t mem, int index)
{
    int wide = 0;
    int alpm_mode = _soc_alpm_mode[unit];
    int is_urpf_mode = SOC_CONTROL(unit)->l3_defip_urpf;
    int defip128_tbl_sz = SOC_CONTROL(unit)->l3_defip_index_remap;

    /* don't remap for ALPM */
    if (defip128_tbl_sz == 0) {
        return index;
    }

    if (mem == L3_DEFIP_PAIR_128m ||
        mem == L3_DEFIP_PAIR_128_ONLYm ||
        mem == L3_DEFIP_PAIR_128_DATA_ONLYm ||
        mem == L3_DEFIP_PAIR_128_HIT_ONLYm) {
        wide = 1;
    }

    if (alpm_mode == 2) {
        if (is_urpf_mode) {
            if (wide == 0) {
                if (index >= _MONTEREY_INDEX_2K_ENTRIES) {
                    return index + (defip128_tbl_sz * 2);
                }
                return index + defip128_tbl_sz;
            } else {
                if (index >= _MONTEREY_INDEX_1K_ENTRIES) {
                    return index + defip128_tbl_sz / 2;
                }
                return index;
            }
        }

        /* not uRPF */
        return (wide) ? index : index + (defip128_tbl_sz * 2);
    }
    if (alpm_mode == 1) {
        uint32 incr = soc_mem_index_count(unit, mem)/4;
        uint32 entries_per_tcam = _MONTEREY_INDEX_1K_ENTRIES;
        uint32 tcam_num, tcam_idx;
        uint32 cnt_128;
        uint32 tmp;

        if (is_urpf_mode) {
            if (wide) {
                tcam_num = index/incr;
                tcam_idx = index % incr;
                return tcam_num * entries_per_tcam + tcam_idx;
            }
            incr = incr/2;
            tcam_num = index/incr;
            tcam_idx = index % incr;
            return tcam_num * entries_per_tcam + tcam_idx +
                                defip128_tbl_sz/4;
        }
        if (wide > 0) {
            if (index < (soc_mem_index_count(unit, mem) / 2)) {
                return index;
            }
            /* make index relative to upper half */
            index -= (soc_mem_index_count(unit, mem) / 2);
            return index + _MONTEREY_INDEX_2K_ENTRIES;
        }

        cnt_128 = defip128_tbl_sz;

        if (cnt_128/2 <= _MONTEREY_INDEX_1K_ENTRIES) {
            if ((cnt_128/2 + index) < _MONTEREY_INDEX_1K_ENTRIES) {
                return index + cnt_128/2;
            } else if ((cnt_128 + index) < _MONTEREY_INDEX_4K_ENTRIES) {
                return index + cnt_128;
            } else if ((cnt_128 + index + cnt_128/2) <
                       (_MONTEREY_INDEX_4K_ENTRIES + _MONTEREY_INDEX_1K_ENTRIES)) {
                return index + cnt_128 + cnt_128/2;
            } else {
                return index + 2 * cnt_128;
            }
        } else if (cnt_128/2 <= _MONTEREY_INDEX_2K_ENTRIES) {
            tmp = cnt_128 - _MONTEREY_INDEX_2K_ENTRIES;
            if ((tmp/2 + index + _MONTEREY_INDEX_2K_ENTRIES) < _MONTEREY_INDEX_3K_ENTRIES) {
                return index + _MONTEREY_INDEX_2K_ENTRIES + tmp/2;
            } else if ((cnt_128 + index) < _MONTEREY_INDEX_4K_ENTRIES) {
                return index + cnt_128;
            } else if ((cnt_128 + tmp/2 + index + _MONTEREY_INDEX_2K_ENTRIES) <
                       (_MONTEREY_INDEX_4K_ENTRIES + _MONTEREY_INDEX_3K_ENTRIES)) {
                return index + cnt_128 + tmp/2 + _MONTEREY_INDEX_2K_ENTRIES;
            } else {
                return index + cnt_128 * 2;
            }
        }

        if (index < soc_mem_index_count(unit, mem)/2) {
            return index + defip128_tbl_sz;
        } else {
            /* make index relative to upper half */
            index -= (soc_mem_index_count(unit, mem)/2);
            return index + 2 * _MONTEREY_INDEX_2K_ENTRIES +
                   defip128_tbl_sz;
       }
    }

    if (is_urpf_mode) { /* URPF mode */
        return soc_l3_defip_urpf_index_map(unit, wide, index);
    }

    return soc_l3_defip_index_map(unit, wide, index);
}

/* Reverse map physical index to logical index.
   Input  : physical index
   Returns: logical index */
int
soc_monterey_l3_defip_index_remap(int unit, soc_mem_t mem, int index)
{
    int wide = 0;
    int alpm_mode = _soc_alpm_mode[unit];
    int is_urpf_mode = SOC_CONTROL(unit)->l3_defip_urpf;
    int defip128_tbl_sz = SOC_CONTROL(unit)->l3_defip_index_remap;

    /* don't remap for ALPM */
    if (defip128_tbl_sz == 0) {
        return index;
    }

    if (mem == L3_DEFIP_PAIR_128m ||
        mem == L3_DEFIP_PAIR_128_ONLYm ||
        mem == L3_DEFIP_PAIR_128_DATA_ONLYm ||
        mem == L3_DEFIP_PAIR_128_HIT_ONLYm) {
        wide = 1;
    }

    if (alpm_mode == 2) {
        if (is_urpf_mode) { /* URPF mode */
            if (wide == 0) {
                if (index >= _MONTEREY_INDEX_6K_ENTRIES) {
                    return index - (defip128_tbl_sz * 2);
                }
                return index - defip128_tbl_sz;
            } else {
                if (index >= _MONTEREY_INDEX_1K_ENTRIES) {
                    return index - defip128_tbl_sz / 2;
                }
                return index;
            }
        }
        return (wide) ? index : index - (defip128_tbl_sz * 2);
    }
    if (alpm_mode == 1) {
        int cnt_128;

        if (is_urpf_mode) {
            uint32 num_chunks, chunk_offset;
            if (wide) {
                num_chunks = index / _MONTEREY_INDEX_1K_ENTRIES;
                chunk_offset = index % _MONTEREY_INDEX_1K_ENTRIES;
                return num_chunks * soc_mem_index_count(unit, mem)/4
                       + chunk_offset;
            }
            num_chunks = index / _MONTEREY_INDEX_1K_ENTRIES;
            chunk_offset = index % _MONTEREY_INDEX_1K_ENTRIES;
            chunk_offset -= (defip128_tbl_sz/4);
            return num_chunks*soc_mem_index_count(unit, mem)/8 + chunk_offset;

        }

        cnt_128 = defip128_tbl_sz;

        if (wide) {
            if (index < cnt_128/2) {
                return index;
            } else if ((index >= _MONTEREY_INDEX_2K_ENTRIES) &&
                       (index < _MONTEREY_INDEX_2K_ENTRIES + cnt_128/2)){
                return index - _MONTEREY_INDEX_2K_ENTRIES + cnt_128/2;
            } else {
                return -1;
            }
        }

        if (cnt_128/2 <= _MONTEREY_INDEX_1K_ENTRIES) {
            if ((index < _MONTEREY_INDEX_1K_ENTRIES) &&
                (index >= cnt_128/2)) {
                return index - cnt_128/2;
            } else if ((index >= _MONTEREY_INDEX_1K_ENTRIES + cnt_128/2) &&
                           (index < _MONTEREY_INDEX_4K_ENTRIES)) {
                return index - cnt_128;
            } else if ((index < _MONTEREY_INDEX_4K_ENTRIES + _MONTEREY_INDEX_1K_ENTRIES) &&
                           (index >= _MONTEREY_INDEX_4K_ENTRIES + cnt_128/2)) {
                return index - cnt_128 - cnt_128/2;
            } else {
                return index - cnt_128 * 2;
            }
        } else if (cnt_128/2 <= _MONTEREY_INDEX_2K_ENTRIES) {
            if (index < _MONTEREY_INDEX_3K_ENTRIES) {
                return index - _MONTEREY_INDEX_2K_ENTRIES - \
                       (cnt_128 - _MONTEREY_INDEX_2K_ENTRIES)/2;
            } else if (index < _MONTEREY_INDEX_4K_ENTRIES) {
                return index - cnt_128;
            } else if (index < _MONTEREY_INDEX_4K_ENTRIES + _MONTEREY_INDEX_3K_ENTRIES) {
                return index - cnt_128 - _MONTEREY_INDEX_2K_ENTRIES - \
                       (cnt_128 - _MONTEREY_INDEX_2K_ENTRIES)/2;
            } else {
                return index - cnt_128 * 2;
            }
        }
    }
    if (is_urpf_mode) { /* URPF mode */
        return soc_l3_defip_urpf_index_remap(unit, wide, index);
    }

    return soc_l3_defip_index_remap(unit, wide, index);
}

/* Given a physical index (always in terms of the narrow entry)
   return the logical index and memory type */
int
soc_monterey_l3_defip_mem_index_get(int unit, int pindex, soc_mem_t *mem)
{
    return soc_l3_defip_index_mem_map(unit, pindex, mem);
}

void
_soc_monterey_alpm_bkt_view_set(int unit, int index, soc_mem_t view)
{
    int bkt = soc_monterey_get_alpm_banks(unit) + 1;
    bkt = (index >> (bkt / 2)) & SOC_MONTEREY_ALPM_BKT_MASK;

    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Unit:%d ALPM bkt set index:%d bkt:%d view:%s\n"),
                 unit, index, bkt, SOC_MEM_NAME(unit, view)));
    _soc_monterey_alpm_bkt_view_map[unit][bkt] = view;
}

soc_mem_t
_soc_monterey_alpm_bkt_view_get(int unit, int index)
{
    soc_mem_t view;
    int bkt = soc_monterey_get_alpm_banks(unit) + 1;
    bkt = (index >> (bkt / 2)) & SOC_MONTEREY_ALPM_BKT_MASK;

    view = _soc_monterey_alpm_bkt_view_map[unit][bkt];
    LOG_VERBOSE(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "Unit:%d ALPM bkt get index:%d bkt:%d view:%s\n"),
                 unit, index, bkt, SOC_MEM_NAME(unit, view)));
    return view;
}

int
_soc_monterey_mem_cpu_write_control(int unit, soc_mem_t mem, int copyno,
                                    int enable, int *orig_enable)
{
    soc_reg_t reg;
    soc_field_t field;
    int blk, port;
    soc_block_t xlport_blocks[] = {SOC_BLK_XLPORT, SOC_BLK_XLPORTB0, SOC_BLOCK_TYPE_INVALID};
    uint32 rval, fval, orig_fval, enable_fval, disable_fval;

    enable_fval = 1;
    disable_fval = 0;

    switch (mem) {
    case XLPORT_WC_UCMEM_DATAm:
        reg = XLPORT_WC_UCMEM_CTRLr;
        field = ACCESS_MODEf;
        SOC_BLOCKS_ITER(unit, blk, xlport_blocks) {
            port = SOC_BLOCK_PORT(unit, blk);
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, port, 0, &rval));
            /* It will use the setting from the last block */
            *orig_enable = soc_reg_field_get(unit, reg, rval, field);
            soc_reg_field_set(unit, reg, &rval, field, enable ? 1 : 0);
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, rval));
        }
        return SOC_E_NONE;
    case L3_DEFIP_ALPM_RAWm:
    case L3_DEFIP_ALPM_IPV4m:
    case L3_DEFIP_ALPM_IPV4_1m:
    case L3_DEFIP_ALPM_IPV6_64m:
    case L3_DEFIP_ALPM_IPV6_64_1m:
    case L3_DEFIP_ALPM_IPV6_128m:
        reg = ISS_MEMORY_CONTROL_84r;
        field = BYPASS_ISS_MEMORY_LPf;
        enable_fval = 0xf;
        break;
    case MMU_INTFI_PFC_ST_TBLm:
        reg = INTFI_CFGr;
        field = PFC_ST_TBL_DISABLEf;
        break;
    case LINK_STATUSm:
        reg = SW2_HW_CONTROLr;
        field = LINK_STATUS_UPDATE_ENABLEf;
        break;
    case MMU_INTFO_QCN_CNM_TIMER_TBLm:
    case MMU_INTFI_XPIPE_FC_MAP_TBL2m:
    case MMU_MTRI_BKPMETERINGBUCKET_MEM_0m:
    case MMU_MTRO_BUCKET_L0_MEM_0m:
    case MMU_MTRO_BUCKET_L1_MEM_0m:
    case MMU_MTRO_BUCKET_L1_MEM_1m:
    case MMU_MTRO_BUCKET_L2_MEM_0m:
    case MMU_MTRO_BUCKET_L2_MEM_1m:
    case MMU_MTRO_BUCKET_L2_MEM_2m:
    case MMU_MTRO_BUCKET_L2_MEM_3m:
    case MMU_MTRO_BUCKET_L2_MEM_4m:
    case MMU_MTRO_BUCKET_L2_MEM_5m:
    case MMU_MTRO_BUCKET_L2_MEM_6m:
    case MMU_MTRO_BUCKET_L2_MEM_7m:
    case MMU_WRED_AVG_QSIZE_X_PIPEm:
    case MMU_WRED_CONFIG_X_PIPEm:
    case MMU_WRED_PORT_SP_DROP_THD_X_PIPEm:
    case MMU_WRED_QGROUP_DROP_THD_X_PIPEm:
    case MMU_WRED_UC_QUEUE_DROP_THD_DEQ_X_PIPE_0m:
    case MMU_WRED_UC_QUEUE_DROP_THD_ENQ_X_PIPE_0m:
        reg = MISCCONFIGr;
        field = REFRESH_ENf;
        enable_fval = 0;
        disable_fval = 1;
        break;
    default:
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
    orig_fval = soc_reg_field_get(unit, reg, rval, field);
    fval = enable ? enable_fval : disable_fval;

    *orig_enable = orig_fval == enable_fval;
    if (fval != orig_fval) {
        soc_reg_field_set(unit, reg, &rval, field, fval);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
    }

    return SOC_E_NONE;
}

/*
 * cpu port (mmu port 65): 48 queues (1300-1347)
 * loopback port (mmu port 66): 10 queues (1290- 1299)
 * MACSEC (mmu port 64) 10 queues (1280-1289)
 */
#define _MONTEREY_CPU_QUEUE_BASE  1300
#define _MONTEREY_MACSEC_QUEUE_BASE   1280
#define _MONTEREY_LB_QUEUE_BASE   1290
#define _MONTEREY_HSP_PORTS       64
#define _MONTEREY_UC_QUEUE_BASE   0
#define _MONTEREY_MC_QUEUE_BASE   640
#define _MONTEREY_NUMBER_OF_UC_QUEUE   640
#define _MONTEREY_NUMBER_OF_MC_QUEUE   708
#define IS_MN_HSP_PORT(u,p)   \
    ((_soc_apache_port_sched_type_get((u),(p)) == SOC_APACHE_SCHED_HSP) ? 1 : 0)


int
soc_monterey_num_cosq_init(int unit)
{
    soc_info_t *si;
    int port, phy_port, mmu_port;
    int hsp_uc_cosq_base, hsp_mc_cosq_base;

    si = &SOC_INFO(unit);

    si->port_cosq_base[CMIC_PORT(unit)] = _MONTEREY_CPU_QUEUE_BASE - _MONTEREY_MC_QUEUE_BASE;
    si->port_num_cosq[CMIC_PORT(unit)] = 48;
    si->port_num_cosq[LB_PORT(unit)] = 10;
    si->port_cosq_base[LB_PORT(unit)] = _MONTEREY_LB_QUEUE_BASE - _MONTEREY_MC_QUEUE_BASE ;
    si->port_cosq_base[MACSEC_PORT(unit)] = _MONTEREY_MACSEC_QUEUE_BASE - _MONTEREY_MC_QUEUE_BASE ;
    si->port_num_cosq[MACSEC_PORT(unit)] = 10;
    si->port_cosq_base[64] = 1280 - _MONTEREY_MC_QUEUE_BASE;


    hsp_uc_cosq_base = 0;
    hsp_mc_cosq_base = 0;

    for (mmu_port = 0; mmu_port < _MONTEREY_MMU_PORTS_PER_PIPE; mmu_port++) {
       phy_port = si->port_m2p_mapping[mmu_port];
        if (phy_port == -1) {
            continue;
        }

        port = si->port_p2l_mapping[phy_port];
        if (port == -1) {
            continue;
        }

        if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port) || IS_MACSEC_PORT(unit, port)) {
            continue;
        }

            si->port_num_cosq[port] = 10;
            si->port_cosq_base[port] = hsp_mc_cosq_base + (mmu_port * si->port_num_cosq[port]);
            si->port_num_uc_cosq[port] = 10;
            /* Made port_uc_cosq_base a function of mmu_port number
                * rather than a running number (as calculated by
                * uc_cosq_base before). If it is a running number, &
                * the port numbers come & go dynamically (as in flex port)
                * uc_cosq_base overlap. With port_uc_cosq_base a function
                * of mmu_port, each port has a fixed offset
 */
            si->port_uc_cosq_base[port] = hsp_uc_cosq_base + (mmu_port * 10);

    }
    return SOC_E_NONE;

}

STATIC int
soc_monterey_frequency_get(int unit, int *max_freq, int *def_freq)
{
    if (soc_feature(unit, soc_feature_untethered_otp)) {
        *max_freq = SOC_BOND_INFO(unit)->max_core_freq;
        *def_freq = SOC_BOND_INFO(unit)->default_core_freq;
    } else {
        *def_freq = *max_freq = 862;
    }

    return SOC_E_NONE;
}

int
soc_monterey_tsc_map_get(int unit, uint32 *tsc_map, uint32 *force_hg)
{

    if (soc_feature(unit, soc_feature_untethered_otp)) {
        *tsc_map = SOC_BOND_INFO(unit)->tsc_enable;
        *force_hg = SOC_BOND_INFO(unit)->tsc_force_hg;
    } else {
        *tsc_map = 0xffffffff;
        *force_hg = 0x0;
    }

    return SOC_E_NONE;
}

typedef struct soc_monterey_valid_fallback_cores_s {
    soc_100g_lane_config_t lane_config;
    int                    valid_lanes[3];
}soc_monterey_valid_fallback_cores_t;

const soc_monterey_valid_fallback_cores_t soc_monterey_valid_an_cores[] = {
    {SOC_LANE_CONFIG_100G_4_4_2, {TRUE, TRUE, TRUE} },
    {SOC_LANE_CONFIG_100G_3_4_3, {TRUE, TRUE, TRUE} },
    {SOC_LANE_CONFIG_100G_2_4_4, {TRUE, TRUE, TRUE} },
    {SOC_INVALID_LANE_CONFIG}
};
/*
STATIC int
soc_monterey_port_is_cpri_supported(int unit, int phy_port) {
      if (((phy_port > 0) && (phy_port < 13)) ||
           ((phy_port > 52) && (phy_port < 65))) {
             return TRUE;
      }
      return FALSE;
}*/
 int
soc_monterey_port_is_falcon(int unit, int phy_port)
{

    if (((phy_port >= 1) && (phy_port <= 20)) ||
        ((phy_port >= 45) && (phy_port <= 64))) {
        return TRUE;
    }

    return FALSE;
}

STATIC int
soc_monterey_port_is_2p5g_forced(int unit, int phy_port)
{
    int tsc_id, bit_pos;
    if (soc_monterey_port_is_falcon(unit, phy_port) ||
        !soc_feature(unit, soc_feature_untethered_otp)) {
        return FALSE;
    }

    tsc_id = ((phy_port - 1) / MONTEREY_PORTS_PER_TSC);

    /* SOC_BOND_INFO(unit)->eagle_force_2p5g is 14bit wide representing 14 Eagle TSCs in AP
     * In case TSC belongs in PGW 1, subtract 2 TSC falcons of PGW 0 to get correct bit pos
     */
    bit_pos = (tsc_id >= MONTEREY_TSCS_PER_PGW) ? (tsc_id - MONTEREY_NUM_CLP_PER_PGW) : tsc_id;

    if (!((SOC_BOND_INFO(unit)->tsce_max_2p5g) & (1 << bit_pos))) {
        return FALSE;
    }
    return TRUE;
}

/* If CLPORT0/1/2/3/4/5 block is disabled */
#define IS_100G_DISABLED(clp_block) \
    (soc_feature(unit, soc_feature_untethered_otp) ? (!((SOC_BOND_INFO(unit)->clmac_map) & (1 << (clp_block)))) : (0))

STATIC soc_error_t
_soc_monterey_port_lanes_set(int unit, int phy_port, int *lanes, int port_bandwidth)
{
    if (soc_monterey_port_is_falcon(unit, phy_port)) {
        *lanes =   (port_bandwidth > 53000) ?
                       (IS_100G_DISABLED(1) && (phy_port == 29)) ? -1
                     : (IS_100G_DISABLED(2) && (phy_port == 33)) ? -1
                     : (IS_100G_DISABLED(4) && (phy_port == 65)) ? -1
                     : (IS_100G_DISABLED(5) && (phy_port == 69)) ? -1
                     : 4
                 : (port_bandwidth > 42000) ? 2
                 : (port_bandwidth > 27000) ?
                       (IS_100G_DISABLED(1) && (phy_port == 29)) ? 4
                     : (IS_100G_DISABLED(2) && (phy_port == 33)) ? 4
                     : (IS_100G_DISABLED(4) && (phy_port == 65)) ? 4
                     : (IS_100G_DISABLED(5) && (phy_port == 69)) ? 4
                     : 2
                     : (port_bandwidth == SOC_PORTCTRL_HG2_TO_IEEE_BW_GET(21000)) ? 2
                 : 1;
    } else {
                 *lanes =  ((port_bandwidth > 21000)? 4
                 :((port_bandwidth > 11000) ? 2
                 : 1));
    }
    return ((*lanes == -1) ? SOC_E_CONFIG : SOC_E_NONE);
}

/* Function to check legal BW values per port
 */
STATIC int
_soc_monterey_port_speed_supported(int unit, int port, int phy_port,
                                 int port_lanes, int port_bandwidth)
{
    soc_pbmp_t xe_pbmp;

    if (port_lanes == -1) {
        if (SOC_FAILURE(_soc_monterey_port_lanes_set(unit, phy_port, &port_lanes, port_bandwidth))) {
            return FALSE;
        }
    }

    switch (port_bandwidth) {
    case 1000:
        /*  Eagles or Falcons - 1 x lanes */
        return (port_lanes == 1);
    case 2500:
    case 5000:
        /*  Eagles or Falcons - 1 x lanes */
        return (port_lanes == 1);
    case 10000:
        /*  Falcons - 1 x lanes and Eagles - 1, 2 or 4 lanes */
        return (!soc_monterey_port_is_falcon(unit, phy_port) ||
                (soc_monterey_port_is_falcon(unit, phy_port) && (port_lanes == 1)));
    case 20000:
        /*  Eagles or Falcons - 2 x lanes */
        return ((!soc_monterey_port_is_falcon(unit, phy_port) && (port_lanes == 4)) || (port_lanes == 2));
    case 25000:
        /*  Falcons only - 1 x lanes */
        return ((soc_monterey_port_is_falcon(unit, phy_port) && (port_lanes == 1)) ||
                (!soc_monterey_port_is_falcon(unit, phy_port) && (port_lanes == 4)));
    case 30000:
        /*  Eagles only - 4 x lanes */
        return (!soc_monterey_port_is_falcon(unit, phy_port) && (port_lanes == 4));
    case 40000:
        /*  Eagles - 4 x lanes; Falcons - 2, 4 lanes */
        return ((!soc_monterey_port_is_falcon(unit, phy_port) && (port_lanes == 4)) ||
                (soc_monterey_port_is_falcon(unit, phy_port) && ((port_lanes == 2) || (port_lanes == 4))));
    case 50000:
        /*  Falcons only - 2,4 lanes */
        return (soc_monterey_port_is_falcon(unit, phy_port) && ((port_lanes == 2)));
    case 100000:
        /*  Eagles - 10 x lanes; Falcons - 4 lanes */
        return ((!soc_monterey_port_is_falcon(unit, phy_port) && (port_lanes == 10)) ||
                (soc_monterey_port_is_falcon(unit, phy_port) &&  (port_lanes == 4)));

    case 11000:
    case 21000:
    case 27000:
    case 42000:
    case 53000:
    case 106000:
        SOC_PBMP_CLEAR(xe_pbmp);
        xe_pbmp = soc_property_get_pbmp_default(unit,
                               spn_PBMP_XPORT_XE, xe_pbmp);
        if (!SOC_PBMP_MEMBER(xe_pbmp, port)) {
            return _soc_monterey_port_speed_supported(unit, port, phy_port, port_lanes, SOC_PORTCTRL_HG2_TO_IEEE_BW_GET(port_bandwidth));
        } else {
            LOG_CLI((BSL_META_U(unit,
                                "Port %d: Invalid HG2 bandwidth %d Gb\n"),
                     port, port_bandwidth / 1000));
            return FALSE;
        }
        break;

    default:
        LOG_CLI((BSL_META_U(unit,
                            "Port %d: Invalid bandwidth %d Gb\n"),
                 port, port_bandwidth / 1000));
        return FALSE;
        break;
    }

    return FALSE;
}

int
soc_monterey_port_config_bandwidth_check(int unit)
{
    int rv = SOC_E_NONE;
    int oversub_mode;
    int port_bandwidth;
    int *blk_port = NULL;
    int *blk_port_bandwidth = NULL;
    int index, port, phy_port, lane_count, blk, pgw;

    int speed_lanes_1 = 0, speed_lanes_2 = 0;
    int blk_linerate_bandwidth, blk_oversub_bandwidth;
    int pgw_port[MONTEREY_PORTS_PER_PGW];
    int pgw_port_bandwidth[MONTEREY_PORTS_PER_PGW];
    int pgw_linerate_bandwidth[MONTEREY_PGWS_PER_DEV], pgw_oversub_bandwidth[MONTEREY_PGWS_PER_DEV];
    int max_pgw_core_bandwidth[MONTEREY_PGWS_PER_DEV];
    int max_pgw_oversub_bandwidth[MONTEREY_PGWS_PER_DEV];
    int os_ratio[MONTEREY_PGWS_PER_DEV];
    /* MAX OS ratio for a few FB5 configs is > 1.5 but is limited only to a single PGW */
    int max_os_ratio = 1750, def_os_ratio = 1500;
    uint32 blk_configured_map, blk_inactive_map, blk_oversub_map;
    int falcon_lr_bandwidth = 0, falcon_os_bandwidth = 0, *falcon_bw_ptr = 0;

    for (pgw = 0; pgw < MONTEREY_PGWS_PER_DEV; pgw++) {
        max_pgw_core_bandwidth[pgw] = (SOC_INFO(unit).bandwidth) / MONTEREY_PGWS_PER_DEV;
        max_pgw_oversub_bandwidth[pgw] = (SOC_INFO(unit).io_bandwidth) / MONTEREY_PGWS_PER_DEV;
    }

    oversub_mode = 1;
/*In monterey even the LR config is oversub as it is line rate only for 256 bytes packet*/

    /* Bandwidth check for ports in each XLPORT/CLPORT block */
    for (pgw = 0; pgw < MONTEREY_PGWS_PER_DEV; pgw++) {
        pgw_linerate_bandwidth[pgw] = 0;
        pgw_oversub_bandwidth[pgw] = 0;

        /* Prepare per PGW local variables for ease of processing */
        for (index = 0; index < MONTEREY_PORTS_PER_PGW; index++) {
            phy_port = 1 + pgw * MONTEREY_PORTS_PER_PGW + index;
            port = SOC_INFO(unit).port_p2l_mapping[phy_port];
            pgw_port[index] = port;
            if (port == -1) {
                pgw_port_bandwidth[index] = 0;
            } else {
                 pgw_port_bandwidth[index] = SOC_INFO(unit).port_speed_max[port];
                 MONTEREY_TDM_SPEED_ADJUST(unit, port, pgw_port_bandwidth[index]);
            }
        }

        for (blk = 0; blk < MONTEREY_TSCS_PER_PGW; blk++) {

            blk_port = &pgw_port[blk * MONTEREY_PORTS_PER_TSC];
            blk_port_bandwidth = &pgw_port_bandwidth[blk * MONTEREY_PORTS_PER_TSC];

            blk_configured_map = 0;
            for (index = 0; index < MONTEREY_PORTS_PER_TSC; index++) {
                if (blk_port[index] != -1) {
                    blk_configured_map |= 1 << index;
                }
            }
            if (blk_configured_map == 0) {
                continue;
            }

            /* Normal port block handling */
            blk_inactive_map = 0;
            blk_oversub_map = 0;
            blk_linerate_bandwidth = 0;
            blk_oversub_bandwidth = 0;
            for (index = 0; index < MONTEREY_PORTS_PER_TSC; index++) {
                port = blk_port[index];
                phy_port = SOC_INFO(unit).port_l2p_mapping[port];
                if (port == -1) {
                    continue;
                }
                port_bandwidth = blk_port_bandwidth[index];
                    blk_oversub_map |= 1 << index;
                if (SOC_PBMP_MEMBER(SOC_INFO(unit).all.disabled_bitmap, port)) {
                    blk_inactive_map |= 1 << index;
                } else {
                    if (blk_oversub_map & (1 << index)) {
                        falcon_bw_ptr = &falcon_os_bandwidth;
                        blk_oversub_bandwidth += port_bandwidth;
                    } else {
                        falcon_bw_ptr = &falcon_lr_bandwidth;
                        blk_linerate_bandwidth += port_bandwidth;
                    }
                }

                /* 0-4 Blocks are PM4x25(falcon) and 5-8 are PM4x10 eagle  in pgw0 */
                /* 0-2 eagle  Blocks are PM4x10(falcon) and 3-8- are PM4x25  in pgw1 */
                if ((pgw == 0 && blk >= 5  && blk <= 7) ||
                     (pgw == 1 && blk >= 0  && blk <= 2)) {

                      if((port_bandwidth > 21000 && (index != 0)) ||
                        (port_bandwidth > 11000 && (index & 1))) {

                        LOG_CLI((BSL_META_U(unit,
                                            "Port %d: Physical port %d can not "
                                            "be configured to %d Gb\n"),
                                 port, phy_port, (port_bandwidth / 1000)));
                        rv = SOC_E_FAIL;
                    }

                    speed_lanes_1 = 11000;
                    speed_lanes_2 = 21000;
               }

                if ((pgw == 0 && blk >= 0 && blk <= 4) ||
                     (pgw == 1 && blk >=3 &&  blk <= 7)) {
                    if ((port_bandwidth > 106000) ||
                        (port_bandwidth > 53000 && (index != 0)) ||
                        (port_bandwidth > 27000 && (index & 0))) {

                        LOG_CLI((BSL_META_U(unit,
                                            "Port %d: Physical port %d can not "
                                            "be configured to %d Gb\n"),
                                 port, phy_port, (port_bandwidth / 1000)));
                        rv = SOC_E_FAIL;
                    }

                    speed_lanes_1 = 27000;
                    speed_lanes_2 = 53000;
                    if (falcon_bw_ptr) {
                        *falcon_bw_ptr += port_bandwidth;
                    }
                }
            }

            /* Check if any of the lanes used by first port of block is
             * not used by other port */
            port_bandwidth = blk_port_bandwidth[0];
            if (port_bandwidth > speed_lanes_2) {
                lane_count = 4;
            } else if (port_bandwidth > speed_lanes_1) {
                lane_count = 2;
            } else {
                lane_count = 1;
            }

            for (index = 1; index < lane_count; index++) {
                port = blk_port[index];
                if (port == -1 || SOC_PBMP_MEMBER(SOC_INFO(unit).all.disabled_bitmap, port)) {
                    continue;
                }
                LOG_CLI((BSL_META_U(unit,
                                    "Port %d bandwidth %d Gb and "
                                    "port %d can not be both configured\n"),
                         blk_port[0], port_bandwidth / 1000, port));
                rv = SOC_E_FAIL;
            }

            /* Check if any of the lanes used by third port of block is
             * not used by other port */
            port_bandwidth = blk_port_bandwidth[2];
            if (port_bandwidth > speed_lanes_1) {
                port = blk_port[3];
                if (port != -1 && !SOC_PBMP_MEMBER(SOC_INFO(unit).all.disabled_bitmap, port)) {
                    LOG_CLI((BSL_META_U(unit,
                                        "Port %d bandwidth %d Gb and port %d "
                                        "can not be both configured\n"),
                             blk_port[2], port_bandwidth / 1000, port));
                    rv = SOC_E_FAIL;
                }
                if (blk_port[0] == -1) {
                    LOG_CLI((BSL_META_U(unit,
                               "Physical port %d needs to be "
                               "configured in order to work with "
                               "port %d\n"),
                               1 + pgw * MONTEREY_PORTS_PER_PGW +
                               blk * MONTEREY_PORTS_PER_TSC,
                               blk_port[2]));
                    rv = SOC_E_FAIL;
                }
            }

            if (blk_inactive_map == blk_configured_map) {
                LOG_CLI((BSL_META_U(unit, "Flex port")));
                for (index = 0; index < MONTEREY_PORTS_PER_TSC; index++) {
                    if (blk_port[index] != -1) {
                        LOG_CLI((BSL_META_U(unit, " %d"), blk_port[index]));
                    }
                }
                LOG_CLI((BSL_META_U(unit, ": Can not be all inactive\n")));
                rv = SOC_E_FAIL;
                continue;
            }

            if (blk_inactive_map != 0) {
                if (blk_oversub_map != 0 &&
                    blk_oversub_map != blk_configured_map) {
                    LOG_CLI((BSL_META_U(unit, "Flex port")));
                    for (index = 0; index < MONTEREY_PORTS_PER_TSC;
                         index++) {
                        if (blk_port[index] != -1) {
                            LOG_CLI((BSL_META_U(unit, " %d"), blk_port[index]));
                        }
                    }
                    LOG_CLI((BSL_META_U(unit,
                                        ": Can not be configured to "
                                        "different oversubscription "
                                        "mode within a port block\n")));
                    rv = SOC_E_FAIL;
                    continue;
                }
            }

            if (falcon_lr_bandwidth != 0 && falcon_os_bandwidth != 0) {
                LOG_CLI((BSL_META_U(unit,
                                    "TDM algorith does not support mixed LR and OS ports "
                                    "modes on PM4x25 ports. LR BW = %d Gbps & OS BW = %d Gbps\n"),
                         falcon_lr_bandwidth / 1000, falcon_os_bandwidth / 1000));
                rv = SOC_E_FAIL;
            }


            if (blk_linerate_bandwidth != 0 && blk_oversub_map != 0) {
                LOG_CLI((BSL_META_U(unit,
                                    "TSC %d cannot have mixed LR and OS ports "
                                    "LR Bandwidth = %d Gbps & OS Bandwidth = %d Gbps\n"),
                         blk, blk_linerate_bandwidth / 1000, blk_oversub_bandwidth / 1000));
                rv = SOC_E_FAIL;
            }

             pgw_linerate_bandwidth[pgw] += blk_linerate_bandwidth;
             pgw_oversub_bandwidth[pgw] += blk_oversub_bandwidth;
        } /* for (blk = 0; blk < MONTEREY_TSCS_PER_PGW; blk++) */

        if ((oversub_mode == 1) && (pgw_linerate_bandwidth[pgw] != 0)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit, "Oversub mode: Linerate BW for PGW %d is not 0.\n"), pgw));
            rv = SOC_E_FAIL;
        } else if ((oversub_mode == 0) && (pgw_oversub_bandwidth[pgw] != 0)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit, "Linerate mode: Oversub BW for PGW %d is not 0.\n"), pgw));
            rv = SOC_E_FAIL;
        } else if (oversub_mode == 2) {
            /*
             * OS ratio = (IO BW - LR BW) / (MAX LR BW - IO LR BW)
             */
            os_ratio[pgw] = (((pgw_oversub_bandwidth[pgw]) * 1000) / (max_pgw_core_bandwidth[pgw] - pgw_linerate_bandwidth[pgw]));
        }

        if (oversub_mode != 2) {
            /* Linerate Bandwidth check */
            if (pgw_linerate_bandwidth[pgw] > max_pgw_core_bandwidth[pgw]) {
                LOG_CLI((BSL_META_U(unit,
                                    "PGW_CL%d total line rate bandwidth "
                                    "(%d Gb) exceeds %d Gb\n"),
                        pgw, (pgw_linerate_bandwidth[pgw] / 1000),
                        (max_pgw_core_bandwidth[pgw] / 1000)));
                rv = SOC_E_FAIL;
            }

            /* Oversub Bandwidth check */
            if ((pgw_linerate_bandwidth[pgw] + pgw_oversub_bandwidth[pgw]) > max_pgw_oversub_bandwidth[pgw]) {
                LOG_CLI((BSL_META_U(unit,
                                    "PGW_CL%d total oversub bandwidth "
                                    "(%d Gb) exceeds %d Gb\n"),
                        pgw, ((pgw_linerate_bandwidth[pgw] + pgw_oversub_bandwidth[pgw]) / 1000),
                        (max_pgw_oversub_bandwidth[pgw] / 1000)));
                rv = SOC_E_FAIL;
            }
        }
       /* Oversub Bandwidth check */
        if ((pgw_linerate_bandwidth[pgw]+ pgw_oversub_bandwidth[pgw]) > max_pgw_oversub_bandwidth[pgw]) {
            LOG_CLI((BSL_META_U(unit,
                                "PGW_CL%d total oversub bandwidth "
                                "(%d Gb) exceeds %d Gb\n"),
                     pgw, ((pgw_linerate_bandwidth[pgw] + pgw_oversub_bandwidth[pgw]) / 1000),
                     (max_pgw_oversub_bandwidth[pgw] / 1000)));
            rv = SOC_E_FAIL;
    }
    } /* for (pgw = 0; pgw < MONTEREY_PGWS_PER_DEV; pgw++) */

    if (oversub_mode == 2) {
        /* For mixed-mode we can have asymmetrical bandwidth distribution
         * Check the OS ratio in this case
         */
        if ((os_ratio[MONTEREY_PGW0] > def_os_ratio) && (os_ratio[MONTEREY_PGW0] < max_os_ratio)) {
            if (os_ratio[MONTEREY_PGW1] > def_os_ratio) {
                LOG_CLI((BSL_META_U(unit,
                                    "PGW_CL1 total oversub bandwidth "
                                    "(%d Gb) exceeds limit\n"),
                                    ((pgw_oversub_bandwidth[MONTEREY_PGW1]) / 1000)));
                rv = SOC_E_FAIL;
            }
        } else if ((os_ratio[MONTEREY_PGW1] > def_os_ratio) && (os_ratio[MONTEREY_PGW1] < max_os_ratio)) {
            if (os_ratio[MONTEREY_PGW0] > def_os_ratio) {
                LOG_CLI((BSL_META_U(unit,
                                    "PGW_CL0 total oversub bandwidth "
                                    "(%d Gb) exceeds limit\n"),
                                    ((pgw_oversub_bandwidth[MONTEREY_PGW0]) / 1000)));
                rv = SOC_E_FAIL;
            }
        }
    }

    return rv;
}


int
soc_monterey_mmu_hsp_port_reserve(int unit, int port, int speed_max)
{
    int def_freq, max_freq;
    int min_hsp_ovs_speed;

    SOC_IF_ERROR_RETURN(soc_monterey_frequency_get(unit, &max_freq, &def_freq));
    def_freq = soc_property_get(unit, spn_CORE_CLOCK_FREQUENCY, def_freq);

    switch (def_freq) {
        case 841:
        case 793:
            min_hsp_ovs_speed = 100000;
            break;
        default:
            min_hsp_ovs_speed = 40000;
            break;

    }

    if (IS_OVERSUB_PORT(unit, port) && speed_max >= min_hsp_ovs_speed) {
        return TRUE;
    }

    return FALSE;
}


STATIC int
soc_monterey_mmu_ports_assign(int unit)
{
    int port, phy_port, mmu_port;
    int port_is_hsp;
    int num_phy_port = 67;
    soc_info_t *si;
    /*soc_pbmp_t pbmp;*/
    int num_phy_ports = MONTEREY_PORTS_PER_PIPE;

    si = &SOC_INFO(unit);

    /* Setup vector-based scheduler (VBS)
     * a.k.a high speed port scheduler (HSP) pbm */
    for (phy_port = 1; phy_port <= num_phy_ports; phy_port++) {
        port = si->port_p2l_mapping[phy_port];
        if (port == -1) {
            continue;
        }

        /* Usage:
         *  port_sched_hsp_<log_port> = 1 ;            Port is in HSP
         *  port_sched_hsp_<log_port> = 0 ;            Port is in LLS
         *  port_sched_hsp_<log_port> = -1 (default) ; SDK determines sched type
 */
        port_is_hsp = 1;

        if (port_is_hsp) {
            SOC_PBMP_PORT_ADD(si->eq_pbm, port);
        } else {
            SOC_PBMP_PORT_REMOVE(si->eq_pbm, port);
        }

    }

    mmu_port = 0;

#if 0 
    /* First assign the lowest MMU port number for VBS (HSP) ports */
    pbmp = si->xpipe_pbm;
    SOC_PBMP_AND(pbmp, si->eq_pbm);
    SOC_PBMP_ITER(pbmp, port) {
        if ( port <= _MONTEREY_LOG_PORTS_PER_PIPE ) {
           phy_port = si->port_l2p_mapping[port];
           if ( phy_port <= num_phy_port) {
               si->port_p2m_mapping[phy_port] = mmu_port;
               si->port_m2p_mapping[mmu_port] = phy_port;
               mmu_port++;
            }
        }
    }
#endif

    /* Assign MMU port for all OS ports with speed >= 100G */
    for (phy_port = 1; phy_port <= num_phy_port; phy_port++) {
        if (si->port_p2m_mapping[phy_port] != -1) {
            continue;
        }

        port = si->port_p2l_mapping[phy_port];
        if (port == -1) {
            continue;
        }

        if ((IS_OVERSUB_PORT(unit, port)) &&
            (si->port_speed_max[port] >= 100000)) {
            si->port_p2m_mapping[phy_port] = mmu_port;
            si->port_m2p_mapping[mmu_port] = phy_port;
            mmu_port++;
        }
    }

    /* Assign MMU port for all OS ports with speed >= 40G */
    for (phy_port = 1; phy_port <= num_phy_port; phy_port++) {
        if (si->port_p2m_mapping[phy_port] != -1) {
            continue;
        }

        port = si->port_p2l_mapping[phy_port];
        if (port == -1) {
            continue;
        }

        if ((IS_OVERSUB_PORT(unit, port)) &&
            (si->port_speed_max[port] >= 40000)) {
            si->port_p2m_mapping[phy_port] = mmu_port;
            si->port_m2p_mapping[mmu_port] = phy_port;
            mmu_port++;
        }
    }

    /* Assign MMU port for all ports with speed >= 100G */
    for (phy_port = 1; phy_port <= num_phy_port; phy_port++) {
        if (si->port_p2m_mapping[phy_port] != -1) {
            continue;
        }

        port = si->port_p2l_mapping[phy_port];
        if (port == -1) {
            continue;
        }

        if (si->port_speed_max[port] >= 100000) {
            si->port_p2m_mapping[phy_port] = mmu_port;
            si->port_m2p_mapping[mmu_port] = phy_port;
            mmu_port++;
        }
    }

    /* Assign MMU port for all ports with speed >= 40G */
    for (phy_port = 1; phy_port <= num_phy_port; phy_port++) {
        if (si->port_p2m_mapping[phy_port] != -1) {
            continue;
        }

        port = si->port_p2l_mapping[phy_port];
        if (port == -1) {
            continue;
        }

        if (si->port_speed_max[port] >= 40000) {
            si->port_p2m_mapping[phy_port] = mmu_port;
            si->port_m2p_mapping[mmu_port] = phy_port;
            mmu_port++;
        }
    }
    /* Assign MMU port for 1st phy port in PM4x25|PM4x10 */
    for (phy_port = 1; phy_port <= num_phy_port; phy_port += MONTEREY_PORTS_PER_TSC) {
        if (si->port_p2m_mapping[phy_port] != -1) {
            continue;
        }

        if (si->port_p2l_mapping[phy_port] == -1) {
            continue;
        }

        si->port_p2m_mapping[phy_port] = mmu_port;
        si->port_m2p_mapping[mmu_port] = phy_port;
        mmu_port++;
    }

    /* Assign MMU port for all remaining ports with speeds >= 25G */
    for (phy_port = 1; phy_port <= num_phy_port; phy_port++) {
        if (si->port_p2m_mapping[phy_port] != -1) {
            continue;
        }

        port = si->port_p2l_mapping[phy_port];
        if (port == -1) {
            continue;
        }

        if (si->port_speed_max[port] >= 25000) {
            si->port_p2m_mapping[phy_port] = mmu_port;
            si->port_m2p_mapping[mmu_port] = phy_port;
            mmu_port++;
        }
    }

    /* Assign MMU port for all remaining ports with speeds >= 10G */
    for (phy_port = 1; phy_port <= num_phy_port; phy_port++) {
        if (si->port_p2m_mapping[phy_port] != -1) {
            continue;
        }

        port = si->port_p2l_mapping[phy_port];
        if (port == -1) {
            continue;
        }

        if (si->port_speed_max[port] >= 10000) {
            si->port_p2m_mapping[phy_port] = mmu_port;
            si->port_m2p_mapping[mmu_port] = phy_port;
            mmu_port++;
        }
    }
    /* Assign MMU port for all remaining ports */
    for (phy_port = 1; phy_port <= num_phy_port; phy_port++) {
        if (si->port_p2m_mapping[phy_port] != -1) {
            continue;
        }

        si->port_p2m_mapping[phy_port] = mmu_port;
        si->port_m2p_mapping[mmu_port] = phy_port;
        mmu_port++;
    }


    return SOC_E_NONE;
}
STATIC int
_soc_mn_ports_speed_init_update(int unit)
{
    int port;
    soc_info_t *si = &SOC_INFO(unit);

    for (port = 0; port < SOC_MAX_NUM_PORTS; port++) {
        /* Set init speed to max speed by default */
        /* Used during port probe and portmod attach */
        si->port_init_speed[port] = si->port_speed_max[port];
    }

    return SOC_E_NONE;
}

/*
 * Monterey port mapping
 *     64 physical ports: port 0-64 in X pipe
 *     106 logical ports
 *     108 mmu ports: port 0-53 in X pipe, port 64-117 in Y pipe
 *     cpu port number is fixed: physical 0, logical 0, mmu 65
 *     loopback port number is fixed: physical 65, logical 65, mmu 66
 *     management port can be:
 *     - one 10G port
 *     - one to four ports with speed up to 2.5G each
 * Physical port number in eacc port group
 * This routine will setup:
 *     SOC_INFO(unit).port_p2l_mapping[]
 *     SOC_INFO(unit).port_l2p_mapping[]
 *     SOC_INFO(unit).port_p2m_mapping[]
 *     SOC_INFO(unit).port_m2p_mapping[]
 *     SOC_INFO(unit).port_speed_max[]
 *     SOC_INFO(unit).port_group[]
 *     SOC_INFO(unit).port_serdes[]
 *     SOC_INFO(unit).port_num_lanes[]
 *     SOC_INFO(unit).phy_port_base[]
 *     SOC_INFO(unit).mmu_port_base[]
 * 100+G port:
 *     - can only be @ 760MHz
 *     - needs to be HSP
 *     - cannot be in oversubscribe mode
 * 40/42G port:
 *     - needs to be HSP except @ 760MHz
 *     - oversubscribe mode can support both cut-through and store-and-forward
 * sub-40G port:
 *     - oversubscribe mode can only support store-and-forward
 */
int
soc_monterey_port_config_init(int unit, uint16 dev_id, uint8 rev_id)
{
    char *config_str, *sub_str, *sub_str_end;
    static const char str_2p5[] = "2.5";
    char str_buf[8];
    soc_info_t *si;
    int  rv;
    int num_port, num_phy_port;
    int port, phy_port;
    int port_bandwidth, port_active_lanes;
    int def_freq, max_freq;
    soc_100g_lane_config_t lane_config = SOC_LANE_CONFIG_100G_DEFAULT;
    int fallback_core = SOC_LANE_CONFIG_100G_DEFAULT;
    int index, tsc_id;
    char option_inactive, option_lanes;
    int port_count;
    int oversub_mode = 0;
    soc_pbmp_t oversub_pbm;
    soc_pbmp_t xe_pbmp;
    soc_pbmp_t cpri_pbmp;
    uint32 tsc_map, force_hg;
    int freq_list_len;
    int const *freq_list;
    int tdm_freq = 0 ;
    static const int monterey_freq_list[] = { 862, 817, 667, 550, 450};
    int int_ports[3][3] = {

                         /* {log, phy, mmu} */
                            {0,   0,   65}, /* CPU */
                            {65,  65,  66}, /* LB */
                            {66,  66,  64}, /* MACSEC */
        };

    SOC_PBMP_CLEAR(xe_pbmp);
    SOC_PBMP_CLEAR(oversub_pbm);
    SOC_PBMP_CLEAR(cpri_pbmp);
    xe_pbmp = soc_property_get_pbmp_default(unit,
                                            spn_PBMP_XPORT_XE, xe_pbmp);
    cpri_pbmp = soc_property_get_pbmp_default(unit,
                                            spn_PBMP_XPORT_CPRI, cpri_pbmp);
    si = &SOC_INFO(unit);

    {
        num_port = 64;
        num_phy_port = 64;
    }

    SOC_IF_ERROR_RETURN(soc_monterey_frequency_get(unit, &max_freq, &def_freq));
    def_freq = soc_property_get(unit, spn_CORE_CLOCK_FREQUENCY, def_freq);
    tdm_freq = soc_property_get(unit, spn_BCM_TDM_FREQUENCY, 0);
    if (def_freq <= max_freq) {
        freq_list = monterey_freq_list;
        freq_list_len = sizeof(monterey_freq_list) / sizeof(int);
        for (index = 0; index < freq_list_len; index++) {
            if (def_freq == freq_list[index]) {
                break;
            }
        }

        if (index < freq_list_len) {
            si->frequency = def_freq;
        } else {
            LOG_CLI((BSL_META_U(unit,
                       "*** Input core clock frequency %dMHz is not "
                       "supported!\n"), def_freq));
        }
    }
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit, "Running core clock frequency %dMhz\n"), def_freq));

    /* Setup max I/O and core bandwidth */
    switch (si->frequency) {
    case 862:
        if (tdm_freq == 862) {
            si->io_bandwidth = 1000000;
        } else if (tdm_freq == 861) {
            si->io_bandwidth = 800000;
        } else {
          if (soc_feature(unit, soc_feature_xflow_macsec)) {
              si->io_bandwidth = 920000;
          } else  {
              si->io_bandwidth = 1000000;
          }
        }
        si->bandwidth = 548000;
        break;
    case 817:
        if (tdm_freq == 815) {
            si->io_bandwidth = 760000;
        } else if (tdm_freq == 816) {
            si->io_bandwidth = 700000;
        } else if (tdm_freq == 666) {
            si->io_bandwidth = 800000;
        } else if (tdm_freq == 667) {
            si->io_bandwidth = 800000;
        } else {
            si->io_bandwidth = soc_property_get(unit, spn_BCM_TDM_IO_BANDWIDTH, 800000);
            if (!(si->io_bandwidth == 800000 || si->io_bandwidth == 720000 ||
                si->io_bandwidth == 460000)) {
                LOG_CLI((BSL_META_U(unit,
                       "*** Input  IO Bandwidth %d is not yet "
                       "supported using default value %d !\n"), si->io_bandwidth,800000));
                si->io_bandwidth = 800000;
            }

        }
        if (dev_id == BCM56675_DEVICE_ID) {
            if (soc_feature(unit, soc_feature_xflow_macsec)) {
                si->io_bandwidth = 860000;
            } else {
                si->io_bandwidth = 920000;
            }
            si->bandwidth = 515000;
        } else {
            si->bandwidth = 490000;
        }
        break;
    case 667:
        if (tdm_freq == 666) {
            si->io_bandwidth = 800000;
        } else if (tdm_freq == 667) {
            si->io_bandwidth = 800000;
        } else {
            si->io_bandwidth = soc_property_get(unit, spn_BCM_TDM_IO_BANDWIDTH, 780000);
            if (!(si->io_bandwidth == 720000 || si->io_bandwidth == 580000 ||
                si->io_bandwidth == 360000 || si->io_bandwidth == 780000 ||
                si->io_bandwidth == 400000)) {
                LOG_CLI((BSL_META_U(unit,
                       "*** Input  IO Bandwidth %d is not yet "
                       "supported using default value %d !\n"), si->io_bandwidth,720000));
                si->io_bandwidth = 720000;
           }
        }
        if (dev_id == BCM56675_DEVICE_ID) {
            if (soc_feature(unit, soc_feature_xflow_macsec)) {
                si->io_bandwidth = 680000;
            } else {
                si->io_bandwidth = 720000;
            }
            si->bandwidth = 420000;
        } else {
            si->bandwidth = 490000;

        }
        break;
    case 550:
        si->io_bandwidth = soc_property_get(unit, spn_BCM_TDM_IO_BANDWIDTH, 400000);
        if (!(si->io_bandwidth == 460000 || si->io_bandwidth == 580000 ||
              si->io_bandwidth == 280000 || si->io_bandwidth == 400000)) {
              LOG_CLI((BSL_META_U(unit,
                       "*** Input  IO Bandwidth %d is not yet "
                     "supported using default value %d !\n"), si->io_bandwidth,580000));
              si->io_bandwidth = 580000;
        }
        if (dev_id == BCM56675_DEVICE_ID) {
            si->io_bandwidth = 540000;
            si->bandwidth = 340000;
        } else {
            si->bandwidth = 342000;
        }


        break;
    case 450:
        si->io_bandwidth = soc_property_get(unit, spn_BCM_TDM_IO_BANDWIDTH, 200000);
        si->bandwidth = 275000;
        if (!(si->io_bandwidth == 460000 || si->io_bandwidth == 360000 ||
              si->io_bandwidth == 200000 || si->io_bandwidth == 400000) ) {
              LOG_CLI((BSL_META_U(unit,
                       "*** Input  IO Bandwidth %d is not yet "
                     "supported using default value %d !\n"), si->io_bandwidth,460000));
              si->io_bandwidth = 460000;
        }
        if (dev_id == BCM56675_DEVICE_ID) {
            if (soc_feature(unit, soc_feature_xflow_macsec)) {
                si->io_bandwidth = 420000;
            } else {
                si->io_bandwidth = 480000;
            }
        }

        break;
    default:
        LOG_CLI((BSL_META_U(unit,
                       "*** Input core clock frequency %dMHz is not yet "
                       "supported!\n"), si->frequency));
        return SOC_E_INTERNAL; /* unknown frequency */
    }


    oversub_mode = soc_property_get(unit, spn_OVERSUBSCRIBE_MODE, 0);
    if (oversub_mode == 2) {
        oversub_pbm = soc_property_get_pbmp(unit, spn_PBMP_OVERSUBSCRIBE, 0);
    }

    SOC_IF_ERROR_RETURN(soc_monterey_tsc_map_get(unit, &tsc_map, &force_hg));

    /* Initialize the soc_info params */
    for (port = 0; port < SOC_MAX_NUM_PORTS; port++) {
        si->port_l2p_mapping[port] = -1;
        si->port_speed_max[port] = -1;
        si->port_group[port] = -1;
        si->port_serdes[port] = -1;
        si->port_num_lanes[port] = -1;

        /* Indexed by physical port */
        si->port_p2l_mapping[port] = -1;
        si->port_p2m_mapping[port] = -1;

        /* Indexed by mmu port */
        si->port_m2p_mapping[port] = -1;
    }



    for (index = 0; index < COUNTOF(int_ports); index++) {
        /* int_ports is an array of log, phy & mmu ports in that order */
        si->port_p2l_mapping[int_ports[index][1]] = int_ports[index][0];
        si->port_p2m_mapping[int_ports[index][1]] = int_ports[index][2];

        SOC_PBMP_PORT_SET(si->xpipe_pbm, int_ports[index][0]);
    }


    SOC_PBMP_CLEAR(si->oversub_pbm);
    SOC_PBMP_CLEAR(si->management_pbm);
    SOC_PBMP_CLEAR(si->all.disabled_bitmap);

    si->phy_port_base[0] = 1;               /* First physical port */
    si->mmu_port_base[0] = 0;               /* First mmu port */
    port_count = 0;

    rv = SOC_E_NONE;
    for (port = 0; port <= num_port; port++) {

        config_str = soc_property_port_get_str(unit, port, spn_PORTMAP);
        if (config_str == NULL) {
            continue;
        }

	/*
	 * portmap.<port>=<physical port number>:<bandwidth in Gb>:[<num_lanes>]:[i]
	 */
        sub_str = config_str;

        /* Parse physical port number */
        phy_port = sal_ctoi(sub_str, &sub_str_end);
        if (sub_str == sub_str_end) {
            LOG_CLI((BSL_META_U(unit,
                       "Port %d: Missing physical port information \"%s\"\n"),
                       port, config_str));
            rv = SOC_E_FAIL;
            continue;
        }

        /* phy_port valid range [1..72] */
        if (phy_port < 1 || phy_port > num_phy_port) {
            LOG_CLI((BSL_META_U(unit,
                       "Port %d: Invalid physical port number %d\n"),
                       port, phy_port));
            rv = SOC_E_FAIL;
            continue;
        }

        tsc_id = (phy_port - 1) / MONTEREY_PORTS_PER_TSC;
        if (!(tsc_map & (1 << tsc_id))) {
            LOG_CLI((BSL_META_U(unit,
                       "Port %d: Physical port %d is in disabled TSC %d\n"),
                       port, phy_port, tsc_id));
            rv = SOC_E_FAIL;
            continue;
        }

        if ((force_hg & (1 << tsc_id)) &&
            (SOC_PBMP_MEMBER(xe_pbmp, port))) {
            LOG_CLI((BSL_META_U(unit,
                       "Port %d: non-hg port in TSC(%d) which is hg-only\n"),
                       port, tsc_id));
            rv = SOC_E_FAIL;
            continue;
        }

        if (si->port_p2l_mapping[phy_port] != -1) {
            LOG_CLI((BSL_META_U(unit,
                       "Port %d: Physical port %d is used by port %d\n"),
                       port, phy_port, si->port_p2l_mapping[phy_port]));
            rv = SOC_E_FAIL;
            continue;
        }

        if (port_count > num_port) {
            LOG_CLI((BSL_META_U(unit,
                       "Can not configure more than %d port\n"), num_port));
            rv = SOC_E_FAIL;
            continue;
        }

        /* Skip ':' between physical port number and bandwidth */
        sub_str = sub_str_end;
        if (*sub_str != '\0') {
            if (*sub_str != ':') {
                LOG_CLI((BSL_META_U(unit,
                                    "Port %d: Bad config string \"%s\"\n"),
                         port, config_str));
                rv = SOC_E_FAIL;
                continue;
            }
            sub_str++;
        }

        /* Parse bandwidth - special case 2.5G */
        for (index = 0; index < sizeof(str_2p5) - 1; index++) {
            if (sub_str[index] == '\0') {
                break;
            }
            str_buf[index] = sub_str[index];
        }
        str_buf[index] = '\0';

        if (!sal_strcmp(str_buf, str_2p5)) {
            port_bandwidth = 2500;
            sub_str_end = &sub_str[sizeof(str_2p5) - 1];
        } else {
            port_bandwidth = sal_ctoi(sub_str, &sub_str_end) * 1000;
            if (sub_str == sub_str_end) {
                LOG_CLI((BSL_META_U(unit,
                                    "Port %d: Missing bandwidth "
                                    "information \"%s\"\n"),
                         port, config_str));
                rv = SOC_E_FAIL;
                continue;
            }
        }

        /* Check if option presents */
        option_inactive = option_lanes = 0;
        sub_str = sub_str_end;
        if (*sub_str != '\0') {
            /* Skip ':' between bandwidth and options */
            if (*sub_str != ':') {
                LOG_CLI((BSL_META_U(unit,
                                    "Port %d: Bad config string \"%s\"\n"),
                           port, config_str));
                rv = SOC_E_FAIL;
                continue;
            }
            sub_str++;

            /* Read 100G lane configuration if present.
             * Skip if options are present */
            if (*sub_str != '\0') {
                if (*sub_str == 'i') {
                    option_inactive = *sub_str;
                    sub_str++;
                } else if ((*sub_str == '1') ||
                           (*sub_str == '2') ||
                           (*sub_str == '4')) {
                    option_lanes = *sub_str;
                    sub_str++;

                    if (*sub_str != '\0') {
                        if (*sub_str != 'i') {
                            LOG_CLI((BSL_META_U(unit,
                                       "Port %d: Bad config string \"%s\"\n"),
                                       port, config_str));
                            rv = SOC_E_FAIL;
                            continue;
                        }
                        option_inactive = *sub_str;
                        sub_str++;
                    }
                } else {
                    LOG_CLI((BSL_META_U(unit,
                               "Port %d: Bad config string \"%s\"\n"),
                               port, config_str));
                    rv = SOC_E_FAIL;
                    continue;
                }
            }
        }

        /* Check trailing string */
        if (*sub_str != '\0') {
            LOG_CLI((BSL_META_U(unit,
                       "Port %d: Bad config string \"%s\"\n"),
                       port, config_str));
            rv = SOC_E_FAIL;
            continue;
        }

        /*
         * Use new properties for 100GE mode lane and
         * fallback autoneg core if these are specified.
         */
        switch (option_lanes) {
            case '1': port_active_lanes = 1; break;
            case '2': port_active_lanes = 2; break;
            case '4': port_active_lanes = 4; break;
            default :
                if (SOC_FAILURE(_soc_monterey_port_lanes_set(unit, phy_port,
                                                           &port_active_lanes,
                                                           port_bandwidth))) {
                    rv = SOC_E_FAIL;
                }
                break;
        }
        if (option_inactive == 'i') {
            SOC_PBMP_PORT_ADD(si->all.disabled_bitmap, port);
            port_active_lanes = -1;
        }

        /* PORT bandwidth checks  */
        if ((soc_monterey_port_is_2p5g_forced(unit, phy_port)) &&
            (port_bandwidth > 2500)) {
            rv = SOC_E_FAIL;
            LOG_CLI((BSL_META_U(unit,
                                "Port %d: Invalid bandwidth %d Gb\n"
                                "Max bandwidth supported is 2.5G\n"),
                     port, port_bandwidth / 1000));

            continue;
        }
        if (!_soc_monterey_port_speed_supported(unit, port, phy_port,
                                              port_active_lanes, port_bandwidth)) {
            rv = SOC_E_FAIL;
            continue;
        }


        /* Update soc_info */
        si->port_p2l_mapping[phy_port] = port;
        si->port_l2p_mapping[port] = phy_port;
        si->port_speed_max[port] = port_bandwidth;
        si->port_100g_lane_config[port] = lane_config;
        si->port_fallback_lane[port]    = fallback_core;
        si->port_num_lanes[port] = port_active_lanes;

        if (SOC_PBMP_MEMBER(oversub_pbm,port) || (oversub_mode == 1)) {
            SOC_PBMP_PORT_ADD(si->oversub_pbm, port);
        }

        port_count++;
    }
    /* Legacy Flex config ":i" case detected, disabled ports in config */
    if (SOC_PBMP_NOT_NULL(SOC_PORT_DISABLED_BITMAP(unit, all))) {
        si->flex_eligible = TRUE;
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        rv = soc_monterey_flexport_scache_recovery(unit);
        if (SOC_FAILURE(rv)) {
            if (rv == SOC_E_NOT_FOUND) {
                rv = SOC_E_NONE;
            } else {
                return rv;
            }
        }
    } else {
        /* Cold boot create */
        SOC_IF_ERROR_RETURN(soc_monterey_flexport_scache_allocate(unit));
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    if (port_count == 0) {
#ifdef __KERNEL__
        /* config.bcm is not parsed yet at the time 1st call of soc_attach
           in Kernel mode SDK. Shouldn't return error code. */
        return SOC_E_NONE;
#else
#if 0
        if (SOC_CONTROL(unit)->soc_flags & SOC_F_ATTACHED) { /* not for PCID */
            rv = SOC_E_FAIL;
        }
#endif
#endif
    }
    /* Bandwidth check for ports in each XLPORT/CLPORT block */

    if (SOC_SUCCESS(rv)) {
        rv = soc_monterey_port_config_bandwidth_check(unit);
    }
    if (SOC_FAILURE(rv)) {
        for (phy_port = 1; phy_port < 66; phy_port++) {
            si->port_p2l_mapping[phy_port] = -1;
        }
        return rv;
    }

    if (bsl_check(bslLayerSoc, bslSourceCommon, bslSeverityVerbose, unit)) {
        LOG_CLI((BSL_META_U(unit, "physical to logical mapping:")));
        for (index = 0; index < num_phy_port; index++) {
            if (index % 16 == 0) {
                LOG_CLI((BSL_META_U(unit, "\n    ")));
            }
            LOG_CLI((BSL_META_U(unit, " %3d"),
                       si->port_p2l_mapping[index]));
        }
        LOG_CLI((BSL_META_U(unit, "\n")));
        LOG_CLI((BSL_META_U(unit, "physical port bandwidth:")));
        for (index = 0; index < num_phy_port; index++) {
            if (index % 16 == 0) {
                LOG_CLI((BSL_META_U(unit, "\n    ")));
            }
            port = si->port_p2l_mapping[index];
            if (port == -1) {
                LOG_CLI((BSL_META_U(unit, "  -1")));
            } else if (si->port_speed_max[port] == 2500) {
                LOG_CLI((BSL_META_U(unit, " 2.5")));
            } else {
                LOG_CLI((BSL_META_U(unit, " %3d"),
                           si->port_speed_max[port] / 1000));
            }
        }
        LOG_CLI((BSL_META_U(unit, "\n")));
    }

    /* Setup port_group, num_lanes, xpipe_pbm  */
    for (port = 0; port <= num_port; port++) {
        phy_port = si->port_l2p_mapping[port];
        if (phy_port == -1) { /* unused port */
            continue;
        }
        /* 26 ports per PGW_CL */
        si->port_group[port] = (phy_port - 1) / MONTEREY_PORTS_PER_PGW;
        si->port_serdes[port] = (phy_port - 1) / MONTEREY_PORTS_PER_TSC;

        SOC_PBMP_PORT_ADD(si->xpipe_pbm, port);

    }
     SOC_PBMP_ASSIGN(si->pipe_pbm[0], si->xpipe_pbm);
    /* Assign mmu ports for all active and inactive ports */
    SOC_IF_ERROR_RETURN(soc_monterey_mmu_ports_assign(unit));

    if (bsl_check(bslLayerSoc, bslSourceCommon, bslSeverityVerbose, unit)) {
        LOG_CLI((BSL_META_U(unit, "physical to mmu mapping:")));
        for (index = 0; index < num_phy_port; index++) {
            if (index % 16 == 0) {
                LOG_CLI((BSL_META_U(unit, "\n    ")));
            }
            LOG_CLI((BSL_META_U(unit, " %3d"),
                       si->port_p2m_mapping[index]));
        }
        LOG_CLI((BSL_META_U(unit, "\n")));
    }

    SOC_IF_ERROR_RETURN(soc_mn_phy_info_init(unit));
     /*  update port_init_speed array for all ports */
    SOC_IF_ERROR_RETURN(_soc_mn_ports_speed_init_update(unit));
    SOC_IF_ERROR_RETURN(soc_mn_port_ovs_speed_group_validate(unit));
    SOC_IF_ERROR_RETURN(soc_mn_port_mixed_sister_speed_validate(unit));
    return SOC_E_NONE;
}

soc_error_t
soc_monterey_port_reg_blk_index_get(int unit, int port, soc_block_type_t blktype, int *block)
{
    uint16 dev_id;
    uint8  rev_id;
    int phy_port;

    soc_cm_get_id(unit, &dev_id, &rev_id);
    phy_port = SOC_INFO(unit).port_l2p_mapping[port];

    switch (blktype) {
        case SOC_BLK_CLG2PORT:
        case SOC_BLK_CLPORT:
            if (IS_CXX_PORT(unit, port) && IS_CL_PORT(unit, port)) {
                if (phy_port == 17) {
                    *block = (rev_id == BCM56560_A0_REV_ID ? 11 : 7);
                } else if (phy_port == 53) {
                    *block = (rev_id == BCM56560_A0_REV_ID ? 14 : 8);
                } else {
                    return SOC_E_PARAM;
                }
            }
            break;
        case SOC_BLK_CXXPORT:
            if ((phy_port >= 17) && (phy_port <= 28)) {
                *block = (rev_id == BCM56560_A0_REV_ID ? 18 : 10);
            } else if ((phy_port >= 53) && (phy_port <= 64)) {
                *block = (rev_id == BCM56560_A0_REV_ID ? 19 : 11);
            } else {
                return SOC_E_PARAM;
            }
            break;
        default:
            break;
    }
    return SOC_E_NONE;
}


#define _SOC_MN_DEF_TEMP_MAX   130

static uint32 _soc_mn_temp_mon_mask[SOC_MAX_NUM_DEVICES];


/*
 * Function: _soc_monterey_tsc_disable
 *
 * Purpose:
 *     Function to dsiable unused TSCs in the chip.
 */
 int
soc_monterey_tsc_disable(int unit)
{
    soc_info_t *si = &SOC_INFO(unit);
    uint32 tsc_enable = 0;
    int lane, phy_port, tsc, port;

    for (phy_port = 1;
         phy_port <= MONTEREY_PORTS_PER_PIPE;
         phy_port = (phy_port + MONTEREY_PORTS_PER_TSC)) {

        tsc = (phy_port - 1) / MONTEREY_PORTS_PER_TSC;
        for (lane = 0; lane < MONTEREY_PORTS_PER_TSC; lane++) {
            port = si->port_p2l_mapping[phy_port + lane];
            if (port != -1) {
                tsc_enable |= (1 << tsc);
                if (IS_CXX_PORT(unit, port) && IS_CL_PORT(unit,port)) {
                    tsc_enable |= (1 << (tsc + 1));
                    tsc_enable |= (1 << (tsc + 2));
                }
                break;
            }
        }
    }

    /* TSC2/3/6/9/12/13 are master PLL cores. Never disable them */
    tsc_enable |= 0x324c;

    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, TOP_TSC_ENABLEr,
                                      REG_PORT_ANY, 0, tsc_enable));

    return SOC_E_NONE;
}

static int
_soc_monterey_tsc_refclk_set(int unit)
{
    uint64    rval64;
    soc_reg_t reg;
    int phy_port, pm;
    tsc_dpll_refclk_connect_t *refclk_connect;
    int sleep_usec = SAL_BOOT_QUICKTURN ? 500000 : 1100;
    soc_info_t *si = &SOC_INFO(unit);

    SHR_BITCLR_RANGE(si->pm_ref_master_bitmap, 0, SOC_MAX_NUM_BLKS);

    for(pm = 0; pm < PM_NUM; pm++) {
        phy_port = pm * 4 + 1;
        if (soc_monterey_port_is_falcon(unit, phy_port)) {
            reg = CLPORT_XGXS0_CTRL_REGr;
        } else {
            reg = XLPORT_XGXS0_CTRL_REGr;
        }
        refclk_connect = &monterey_tsc_refclk_connect[pm];

        /* set refclock connect */
        SOC_REG_RST_VAL_GET(unit, reg, rval64);

        soc_reg64_field32_set(unit, reg, &rval64, PLL0_REFIN_ENf, refclk_connect->pll0_refclk_select);
        soc_reg64_field32_set(unit, reg, &rval64, PLL0_REFOUT_ENf, refclk_connect->pll0_master_enable);
        soc_reg64_field32_set(unit, reg, &rval64, PLL0_LC_REFSELf, refclk_connect->pll0_lc_refclk_select);

        soc_reg64_field32_set(unit, reg, &rval64, PLL1_REFIN_ENf, refclk_connect->pll1_refclk_select);
        soc_reg64_field32_set(unit, reg, &rval64, PLL1_REFOUT_ENf, refclk_connect->pll1_master_enable);
        soc_reg64_field32_set(unit, reg, &rval64, PLL1_LC_REFSELf, !refclk_connect->pll1_lc_refclk_select);

        SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg, phy_port, 0, rval64));

         /* OOS master PLL core */
         if(monterey_tsc_is_master_pll(pm)) {
             /* De-assert IDDQ */
             soc_reg64_field32_set(unit, reg, &rval64, IDDQf, 0);
             SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg, phy_port, 0, rval64));

             /* De-assert power down */
             soc_reg64_field32_set(unit, reg, &rval64, PWRDWNf, 0);
             SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg, phy_port, 0, rval64));
             sal_usleep(sleep_usec);

             /* Reset XGXS */
             soc_reg64_field32_set(unit, reg, &rval64, RSTB_HWf, 0);
             SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg, phy_port, 0, rval64));
             sal_usleep(sleep_usec + 10000);

             /* Bring XGXS out of reset */
             soc_reg64_field32_set(unit, reg, &rval64, RSTB_HWf, 1);
             SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, reg, phy_port, 0, rval64));

             /* Mark as master pll core */
             SHR_BITSET(si->pm_ref_master_bitmap, SOC_PORT_BINDEX(unit, phy_port));
         }

         /* set internal termination to 200 Ohm */
         if (soc_monterey_port_is_falcon(unit, phy_port)) {
             if (refclk_connect->pll0_master_enable) {
                 SOC_REG_RST_VAL_GET(unit, CLPORT_PMD_PLL0_CTRL_CONFIGr, rval64);
                 soc_reg64_field32_set(unit, CLPORT_PMD_PLL0_CTRL_CONFIGr, &rval64, PLL0_RTERM200f, 1);
                 SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, CLPORT_PMD_PLL0_CTRL_CONFIGr, phy_port, 0, rval64));
             }

             if (refclk_connect->pll1_master_enable) {
                SOC_REG_RST_VAL_GET(unit, CLPORT_PMD_PLL1_CTRL_CONFIGr, rval64);
                soc_reg64_field32_set(unit, CLPORT_PMD_PLL1_CTRL_CONFIGr, &rval64, PLL1_RTERM200f, 1);
                SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, CLPORT_PMD_PLL1_CTRL_CONFIGr, phy_port, 0, rval64));
             }
         } else {
            if (refclk_connect->pll0_master_enable || refclk_connect->pll1_master_enable) {
                SOC_REG_RST_VAL_GET(unit, XLPORT_PMD_PLL_CTRL_CONFIGr, rval64);
                soc_reg64_field32_set(unit, XLPORT_PMD_PLL_CTRL_CONFIGr, &rval64, REFCLK_TERM_SELf, 1);
                SOC_IF_ERROR_RETURN(soc_reg_rawport_set(unit, XLPORT_PMD_PLL_CTRL_CONFIGr, phy_port, 0, rval64));
            }
         }
    }

    return SOC_E_NONE;
}

void
soc_monterey_sbus_ring_map_config(int unit)
{
    uint16              dev_id;
    uint8               rev_id;

    /*
     *  SBUS Ring # Start   Through Blocks                                                      End
     *      0       CMIC    IP                                                                  CMIC
     *      1       CMIC    EP                                                                  CMIC
     *      2       CMIC    MMU                                                                 CMIC
     *      3       CMIC    TSC0 - TSC1 - TSC2 - TSC3 - PGW0 - TSC4_5_6 - TSC7 - TSC8           CMIC
     *      4       CMIC    TSC9 - TSC10 - TSC11 - TSC12 - PGW1 - TSC13_14_15 - TSC16 - TSC17   CMIC
     *      5       CMIC    TOP REGISTERS - OTPC - AVS - SER                                    CMIC
     *      6       CMIC    RDB                                                                 CMIC
     *      7       CMIC    Not used                                                            CMIC
     */

    WRITE_CMIC_SBUS_RING_MAP_0_7r(unit, 0x43052100);
    WRITE_CMIC_SBUS_RING_MAP_8_15r(unit, 0x33333333);
    WRITE_CMIC_SBUS_RING_MAP_16_23r(unit, 0x44444333);
    WRITE_CMIC_SBUS_RING_MAP_24_31r(unit, 0x50444444);
    WRITE_CMIC_SBUS_RING_MAP_32_39r(unit, 0x00643055);
    WRITE_CMIC_SBUS_RING_MAP_40_47r(unit, 0x00000000);
    WRITE_CMIC_SBUS_RING_MAP_48_55r(unit, 0x00000000);
    WRITE_CMIC_SBUS_RING_MAP_56_63r(unit, 0x00000000);


    soc_cm_get_id(unit, &dev_id, &rev_id);

    if (rev_id == BCM56560_A0_REV_ID) {
        WRITE_CMIC_SBUS_TIMEOUTr(unit, 0x1000);
    } else {
        WRITE_CMIC_SBUS_TIMEOUTr(unit, 0x7d0);
    }

    return;
}

int
soc_monterey_chip_reset(int unit)
{
    uint16 dev_id;
    uint8 rev_id;
    uint32 rval, to_usec;
    soc_reg_t reg;
    int index;
    int num_ecmp_rh_flowset_entries;
    int rh_table_config_encoding;
#if defined(BCM_RIOT_SUPPORT) || defined(BCM_MULTI_LEVEL_ECMP_SUPPORT)
    int num_overlay_ecmp_rh_flowset_entries;
    int rh_bank_sel = 0x0;
    int ecmp_levels = 1;
#endif
    soc_pbmp_t cpri_pbmp;
    int temp_mask, temp_thr, power_reset = 0;
    static const soc_reg_t temp_thr_reg[] = {
        TOP_PVTMON_0_INTR_THRESHOLDr, TOP_PVTMON_1_INTR_THRESHOLDr,
        TOP_PVTMON_2_INTR_THRESHOLDr, TOP_PVTMON_3_INTR_THRESHOLDr,
        TOP_PVTMON_4_INTR_THRESHOLDr, TOP_PVTMON_5_INTR_THRESHOLDr,
        TOP_PVTMON_6_INTR_THRESHOLDr, TOP_PVTMON_7_INTR_THRESHOLDr
    };
    static const char *temp_thr_prop[] = {
        "temp0_threshold", "temp1_threshold", "temp2_threshold",
        "temp3_threshold", "temp4_threshold", "temp5_threshold",
        "temp6_threshold", "temp7_threshold"
    };
    static const soc_reg_t lcpll_reset_field[] = {
       TOP_XG_PLL0_RST_Lf, TOP_XG_PLL1_RST_Lf,
       TOP_XG_PLL2_RST_Lf, TOP_XG_PLL3_RST_Lf
    };
    static const soc_reg_t lcpll_post_reset_field[] = {
       TOP_XG_PLL0_POST_RST_Lf, TOP_XG_PLL1_POST_RST_Lf,
       TOP_XG_PLL2_POST_RST_Lf, TOP_XG_PLL3_POST_RST_Lf
    };


#if 0 
    static const soc_reg_t lcpll_ctrl1_reg[] = {
        TOP_XG_PLL0_CTRL_1r, TOP_XG_PLL1_CTRL_1r,
        TOP_XG_PLL2_CTRL_1r, TOP_XG_PLL3_CTRL_1r,
    };
    static const soc_reg_t lcpll_ctrl3_reg[] = {
        TOP_XG_PLL0_CTRL_3r, TOP_XG_PLL1_CTRL_3r,
        TOP_XG_PLL2_CTRL_3r, TOP_XG_PLL3_CTRL_3r
    };
    static const soc_reg_t lcpll_ctrl4_reg[] = {
        TOP_XG_PLL0_CTRL_4r, TOP_XG_PLL1_CTRL_4r,
        TOP_XG_PLL2_CTRL_4r, TOP_XG_PLL3_CTRL_4r
    };
#endif
    static const soc_reg_t lcpll_status_reg[] = {
        TOP_XG_PLL0_STATUSr, TOP_XG_PLL1_STATUSr,
        TOP_XG_PLL2_STATUSr, TOP_XG_PLL3_STATUSr
    };

    /* Settings for 500 MHz TSPLL output clock. */
    unsigned ts_ref_freq = 0;
    unsigned ts_idx = 0;

    static const soc_pll_param_t ts_pll[] = {
    /*     Fref, Ndiv_int, Ndiv_frac,  Pdiv, Mdiv, Ka, Ki, Kp, VCO_div2 */
      {12800000,      234,   0x60000,     1,    3,  0,  3,  7,        1}, /* Ndiv = 234.375 */
      {20000000,      150,         0,     1,    3,  0,  3,  7,        1}, /* Ndiv = 150.0 */
      {25000000,      120,         0,     1,    3,  0,  3,  7,        1}, /* Ndiv = 120.0 */
      {32000000,       93,   0xC0000,     1,    3,  0,  3,  7,        1}, /* Ndiv = 93.75 */
      {50000000,       60,         0,     1,    3,  0,  3,  7,        1}, /* Ndiv = 60.0 */
      {125000000,      24,         0,     1,    3,  0,  3,  7,        1}, /* Ndiv = 24.0 */
      {       0,       60,         0,     1,    3,  0,  3,  7,        1}  /* 50 MHz, from internal reference. */
    };

    /* Settings for 20 MHz BSPLL output clock. */
    unsigned bs_ref_freq = 0;
    unsigned bs_idx = 0;
    uint32 bs_ndiv_high, bs_ndiv_low;

    static const soc_pll_param_t bs_pll[] = {
    /*     Fref, Ndiv_int,   Ndiv_frac,  Pdiv, Mdiv, Ka, Ki, Kp, VCO_div2 */
      {12800000,      273,  0x70000000,     1,  175,  0,  2,  3,        1},
      {20000000,      175,           0,     1,  175,  0,  2,  3,        1},
      {25000000,      140,           0,     1,  175,  0,  2,  3,        1},
      {32000000,      109,  0x60000000,     1,  175,  0,  2,  3,        1},
      {50000000,       70,           0,     1,  175,  0,  2,  3,        1},
      {       0,       60,           0,     1,  150,  0,  2,  3,        1} /* 25 MHz, from internal reference. */
    };
    static const int monterey_freq_list[] = { 862, 817, 667, 550, 450};
    int max_frequency, def_freq;
    int freq_list_len = 0 , sel = 0;
    int const *freq_list;
    soc_info_t *si;
    si = &SOC_INFO(unit);
    to_usec = SAL_BOOT_QUICKTURN ? (250 * MILLISECOND_USEC) :
                                   (10 * MILLISECOND_USEC);
    soc_monterey_sbus_ring_map_config(unit);

    sal_usleep(to_usec);

    soc_cm_get_id(unit, &dev_id, &rev_id);
    SOC_IF_ERROR_RETURN(soc_monterey_frequency_get(unit, &max_frequency, &def_freq));
    freq_list = monterey_freq_list;
    freq_list_len = sizeof(monterey_freq_list) / sizeof(int);
    if (si->frequency != def_freq) {
        for (sel = 0; sel < freq_list_len; sel++) {
            if (si->frequency == freq_list[sel]) {
                break;
            }
        }

        LOG_CLI((BSL_META_U(unit,
                   "*** change CORE_CLK_FREQ_SEL to %d\n"), sel));
        LOG_CLI((BSL_META_U(unit,
                   "*** change core clock frequency to %dMHz\n"),
                   si->frequency));

        SOC_IF_ERROR_RETURN(READ_TOP_CORE_CLK_FREQ_SELr(unit, &rval));
        soc_reg_field_set(unit, TOP_CORE_CLK_FREQ_SELr, &rval,
                          SW_CORE_CLK_SEL_ENf, 1);
        SOC_IF_ERROR_RETURN(WRITE_TOP_CORE_CLK_FREQ_SELr(unit, rval));
        soc_reg_field_set(unit, TOP_CORE_CLK_FREQ_SELr, &rval,
                          CORE_CLK_FREQ_SELf, sel);
        SOC_IF_ERROR_RETURN(WRITE_TOP_CORE_CLK_FREQ_SELr(unit, rval));

        sal_usleep(to_usec);
    }



    /*
     * For TD2+ and TD+ compatible packages where there is no 1.25V supply in the package,
     * ams_pll_drv_hv_disable needs to be set to 1.
     *
     * ams_pll_drv_hv_disable: This is the main TX driver mode for Falcon, and is properly enabled
     * when TVDD1P25 = 1.25V, and tx_drv_hv_disable is set to its default value, "0".
     * To prevent damage to the output driver, this bit must always be "0" if TVDD1P25 = 1.25V
     */

    SOC_IF_ERROR_RETURN(READ_TOP_XG_PLL0_CTRL_5r(unit, &rval));
    power_reset =  !soc_reg_field_get(unit, TOP_XG_PLL0_CTRL_5r, rval,
                      RESERVEDf);

    /* Assert nanosync reset before asserting PLL's post_resetb */
    rval = 0;
    SOC_IF_ERROR_RETURN(READ_NANOSYNC_S0_IDM_IDM_RESET_CONTROLr(unit, &rval));
    soc_reg_field_set(unit, NANOSYNC_S0_IDM_IDM_RESET_CONTROLr, &rval,
                        RESETf, 1);
    SOC_IF_ERROR_RETURN(WRITE_NANOSYNC_S0_IDM_IDM_RESET_CONTROLr(unit, rval));

    /* Enable Legacy HO enable */
    rval = 0 ;
    SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_6r(unit, &rval));
    soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_6r, &rval,
                        LEGACY_HO_LOGIC_ENf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL0_CTRL_6r(unit, rval));

    /* bs_ndiv_low maps to lower 32 bits (i.e., first 32bits) of the 48 bit Ndiv */
    SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_5r(unit,&rval));
    soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_5r, &rval, PLL_FBDIV_0f, 0x0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL0_CTRL_5r(unit, rval));

    /* bs_ndiv_high maps to higher 16 bits (i.e., last 16bits) of the 48 bit Ndiv */
    SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_6r(unit,&rval));
    soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_6r, &rval, PLL_FBDIV_1f, 0x0F00);
    SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL0_CTRL_6r(unit, rval));

     /* Enable BS_PLL0_HO_BYP_ENABLE */
    rval = 0;
    SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROL_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_MISC_CONTROL_2r, &rval,
                        BS_PLL0_HO_BYP_ENABLEf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_MISC_CONTROL_2r(unit, rval));

    /* Enable Legacy HO enable for BS1*/
    rval = 0 ;
    SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL1_CTRL_6r(unit, &rval));
    soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_6r, &rval,
                        LEGACY_HO_LOGIC_ENf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL1_CTRL_6r(unit, rval));

    /* bs_ndiv_low maps to lower 32 bits (i.e., first 32bits) of the 48 bit Ndiv */
    SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL1_CTRL_5r(unit,&rval));
    soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_5r, &rval, PLL_FBDIV_0f, 0x0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL1_CTRL_5r(unit, rval));

    /* bs_ndiv_high maps to higher 16 bits (i.e., last 16bits) of the 48 bit Ndiv */
    SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL1_CTRL_6r(unit,&rval));
    soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_6r, &rval, PLL_FBDIV_1f, 0x0F00);
    SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL1_CTRL_6r(unit, rval));

     /* Enable BS_PLL1_HO_BYP_ENABLE */
    rval = 0;
    SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROL_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_MISC_CONTROL_2r, &rval,
                        BS_PLL1_HO_BYP_ENABLEf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_MISC_CONTROL_2r(unit, rval));

    rval = 0 ;
    /* Put TS, BS PLLs to reset before changing PLL control registers */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                        TOP_TS_PLL_RST_Lf, 0);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                        TOP_BS_PLL0_RST_Lf, 0);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                        TOP_BS_PLL1_RST_Lf, 0);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                        TOP_TS_PLL_POST_RST_Lf, 0);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                        TOP_BS_PLL0_POST_RST_Lf, 0);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                        TOP_BS_PLL1_POST_RST_Lf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));

    if (power_reset) {
        rval = 0;
        SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
        for (index = 0; index < COUNTOF(lcpll_reset_field); index = index + 3) {
            soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                              lcpll_reset_field[index], 0);
        }
        SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));

        rval = 0;
        SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROL_2r(unit, &rval));
        soc_reg_field_set(unit, TOP_MISC_CONTROL_2r, &rval,
                          XG_PLL0_HO_BYP_ENABLEf, 1);
        soc_reg_field_set(unit, TOP_MISC_CONTROL_2r, &rval,
                          XG_PLL3_HO_BYP_ENABLEf, 1);
        SOC_IF_ERROR_RETURN(WRITE_TOP_MISC_CONTROL_2r(unit, rval));

        rval = 0;
        SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
        for (index = 0; index < COUNTOF(lcpll_post_reset_field); index = index + 3) {
            soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                              lcpll_post_reset_field[index], 0);
        }
        SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));

        rval = 0;
        SOC_IF_ERROR_RETURN(READ_TOP_XG_PLL0_CTRL_5r(unit, &rval));
        soc_reg_field_set(unit,TOP_XG_PLL0_CTRL_5r, &rval,
                   ISO_INf,1);
        SOC_IF_ERROR_RETURN(WRITE_TOP_XG_PLL0_CTRL_5r(unit, rval));
        sal_usleep(to_usec);
        rval = 0;
        SOC_IF_ERROR_RETURN(READ_TOP_XG_PLL0_CTRL_5r(unit, &rval));
        soc_reg_field_set(unit,TOP_XG_PLL0_CTRL_5r, &rval,
                   XG_PLL_PWRDWNf,1);
        SOC_IF_ERROR_RETURN(WRITE_TOP_XG_PLL0_CTRL_5r(unit, rval));
        sal_usleep(to_usec);
        rval = 0;
        SOC_IF_ERROR_RETURN(READ_TOP_XG_PLL0_CTRL_1r(unit, &rval));
        soc_reg_field_set(unit,TOP_XG_PLL0_CTRL_1r, &rval,
                   POWER_ON_LDOf,0);
        SOC_IF_ERROR_RETURN(WRITE_TOP_XG_PLL0_CTRL_1r(unit, rval));
        sal_usleep(to_usec);
        rval = 0;

        if (soc_property_get(unit, spn_LC_PLL_EXT_CLOCK, 0)) {
            SOC_IF_ERROR_RETURN(READ_TOP_XG_PLL0_CTRL_0r(unit, &rval));
            soc_reg_field_set(unit,TOP_XG_PLL0_CTRL_0r, &rval,
                          CH0_MDIVf,0x5);
            SOC_IF_ERROR_RETURN(WRITE_TOP_XG_PLL0_CTRL_0r(unit, rval));

            rval = 0;
            SOC_IF_ERROR_RETURN(READ_TOP_XG_PLL0_CTRL_8r(unit, &rval));
            soc_reg_field_set(unit,TOP_XG_PLL0_CTRL_8r, &rval,
                       PLL_FBDIV_1f,0x580);
            SOC_IF_ERROR_RETURN(WRITE_TOP_XG_PLL0_CTRL_8r(unit, rval));
        }
        rval = 0;
        SOC_IF_ERROR_RETURN(READ_TOP_XG_PLL0_CTRL_1r(unit, &rval));
        soc_reg_field_set(unit,TOP_XG_PLL0_CTRL_1r, &rval,
             POWER_ON_LDOf,1);
        SOC_IF_ERROR_RETURN(WRITE_TOP_XG_PLL0_CTRL_1r(unit, rval));
        sal_usleep(to_usec);

        rval = 0;
        SOC_IF_ERROR_RETURN(READ_TOP_XG_PLL0_CTRL_5r(unit, &rval));
        soc_reg_field_set(unit,TOP_XG_PLL0_CTRL_5r, &rval,
                 XG_PLL_PWRDWNf,0);
        SOC_IF_ERROR_RETURN(WRITE_TOP_XG_PLL0_CTRL_5r(unit, rval));
        sal_usleep(to_usec);

        rval = 0;
        SOC_IF_ERROR_RETURN(READ_TOP_XG_PLL0_CTRL_5r(unit, &rval));
        soc_reg_field_set(unit,TOP_XG_PLL0_CTRL_5r, &rval,
                ISO_INf,0);
        SOC_IF_ERROR_RETURN(WRITE_TOP_XG_PLL0_CTRL_5r(unit, rval));
        sal_usleep(to_usec);


        SOC_IF_ERROR_RETURN(READ_TOP_XG_PLL3_CTRL_5r(unit, &rval));
        soc_reg_field_set(unit,TOP_XG_PLL3_CTRL_5r, &rval,
                   ISO_INf,1);
        SOC_IF_ERROR_RETURN(WRITE_TOP_XG_PLL3_CTRL_5r(unit, rval));
        sal_usleep(to_usec);
        rval = 0;
        SOC_IF_ERROR_RETURN(READ_TOP_XG_PLL3_CTRL_5r(unit, &rval));
        soc_reg_field_set(unit,TOP_XG_PLL3_CTRL_5r, &rval,
                  XG_PLL_PWRDWNf,1);
        SOC_IF_ERROR_RETURN(WRITE_TOP_XG_PLL3_CTRL_5r(unit, rval));
        sal_usleep(to_usec);

        rval = 0;
        SOC_IF_ERROR_RETURN(READ_TOP_XG_PLL3_CTRL_1r(unit, &rval));
        soc_reg_field_set(unit,TOP_XG_PLL3_CTRL_1r, &rval,
                   POWER_ON_LDOf,0);
        SOC_IF_ERROR_RETURN(WRITE_TOP_XG_PLL3_CTRL_1r(unit, rval));
        sal_usleep(to_usec);

        if (soc_property_get(unit, spn_LC_PLL_EXT_CLOCK, 0)) {
            rval = 0;
            SOC_IF_ERROR_RETURN(READ_TOP_XG_PLL3_CTRL_0r(unit, &rval));
            soc_reg_field_set(unit,TOP_XG_PLL3_CTRL_0r, &rval,
                           CH0_MDIVf,0x5);
            SOC_IF_ERROR_RETURN(WRITE_TOP_XG_PLL3_CTRL_0r(unit, rval));

            rval = 0;
            SOC_IF_ERROR_RETURN(READ_TOP_XG_PLL3_CTRL_8r(unit, &rval));
            soc_reg_field_set(unit,TOP_XG_PLL3_CTRL_8r, &rval,
                          PLL_FBDIV_1f,0x580);
            SOC_IF_ERROR_RETURN(WRITE_TOP_XG_PLL3_CTRL_8r(unit, rval));
        }
        rval = 0;
        SOC_IF_ERROR_RETURN(READ_TOP_XG_PLL3_CTRL_1r(unit, &rval));
        soc_reg_field_set(unit,TOP_XG_PLL3_CTRL_1r, &rval,
                POWER_ON_LDOf,1);
        SOC_IF_ERROR_RETURN(WRITE_TOP_XG_PLL3_CTRL_1r(unit, rval));
        sal_usleep(to_usec);


         rval = 0;
         SOC_IF_ERROR_RETURN(READ_TOP_XG_PLL3_CTRL_5r(unit, &rval));
         soc_reg_field_set(unit,TOP_XG_PLL3_CTRL_5r, &rval,
                 XG_PLL_PWRDWNf,0);
         SOC_IF_ERROR_RETURN(WRITE_TOP_XG_PLL3_CTRL_5r(unit, rval));
         sal_usleep(to_usec);


        rval = 0;
        SOC_IF_ERROR_RETURN(READ_TOP_XG_PLL3_CTRL_5r(unit, &rval));
        soc_reg_field_set(unit,TOP_XG_PLL3_CTRL_5r, &rval,
                  ISO_INf,0);
        SOC_IF_ERROR_RETURN(WRITE_TOP_XG_PLL3_CTRL_5r(unit, rval));
        sal_usleep(to_usec);

    }

    rval = 0;

    SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_CTRL_7r(unit, &rval));
    soc_reg_field_set(unit, TOP_TS_PLL_CTRL_7r, &rval,
                        PI_RESETf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_TS_PLL_CTRL_7r(unit, rval));

    rval = 0;
    SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_DITHER_FSM_CTRL_0r(unit, &rval));
    soc_reg_field_set(unit, TOP_TS_PLL_DITHER_FSM_CTRL_0r, &rval,
                        DITHER_CTRL_N1f, SOC_MONTEREY_TS_PLL_N1);
    soc_reg_field_set(unit, TOP_TS_PLL_DITHER_FSM_CTRL_0r, &rval,
                        DITHER_CTRL_N2f, SOC_MONTEREY_TS_PLL_N2);
    SOC_IF_ERROR_RETURN(WRITE_TOP_TS_PLL_DITHER_FSM_CTRL_0r(unit, rval));
    rval = 0;
    SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_DITHER_FSM_CTRL_1r(unit, &rval));
    soc_reg_field_set(unit, TOP_TS_PLL_DITHER_FSM_CTRL_1r, &rval,
                        DITHER_CTRL_M1f, SOC_MONTEREY_TS_PLL_M1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_TS_PLL_DITHER_FSM_CTRL_1r(unit, rval));
    rval = 0;
    SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_DITHER_FSM_CTRL_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_TS_PLL_DITHER_FSM_CTRL_2r, &rval,
                        DITHER_CTRL_M2f, SOC_MONTEREY_TS_PLL_M1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_TS_PLL_DITHER_FSM_CTRL_2r(unit, rval));

    rval = 0;
    SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_DITHER_FSM_CTRL_3r(unit, &rval));
    soc_reg_field_set(unit, TOP_TS_PLL_DITHER_FSM_CTRL_3r, &rval,
                        DITHER_CTRL_PMAX1f, SOC_MONTEREY_TS_PLL_PMAX1);
    soc_reg_field_set(unit, TOP_TS_PLL_DITHER_FSM_CTRL_3r, &rval,
                       DITHER_CTRL_PMAX2f, SOC_MONTEREY_TS_PLL_PMAX2);
    SOC_IF_ERROR_RETURN(WRITE_TOP_TS_PLL_DITHER_FSM_CTRL_3r(unit, rval));
    rval = 0;
    /* Configure TS PLL */
    ts_ref_freq = soc_property_get(unit, spn_PTP_TS_PLL_FREF, 0);  /* 0->internal reference */

    for (ts_idx = 0; ts_idx < sizeof(ts_pll)/sizeof(ts_pll[0]); ++ts_idx) {
        if (ts_pll[ts_idx].ref_freq == ts_ref_freq) {
            break;
        }
    }
    if (ts_idx == sizeof(ts_pll)/sizeof(ts_pll[0])) {
        /* Valid configuration lookup failed. Use internal reference. */
        ts_idx = 5;
        ts_ref_freq = 0;
    }

    /* Do not change PLL settings for internal reference */
    if(ts_ref_freq) {
        SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_CTRL_1r(unit, &rval));
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_1r, &rval, FREF_SELf, (ts_ref_freq != 0));
        SOC_IF_ERROR_RETURN(WRITE_TOP_TS_PLL_CTRL_1r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_CTRL_3r(unit,&rval));
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_3r, &rval, PDIVf,
                          soc_property_get(unit, spn_PTP_TS_PLL_PDIV, ts_pll[ts_idx].pdiv));
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_3r, &rval, CH0_MDIVf,
                          soc_property_get(unit, spn_PTP_TS_PLL_MNDIV, ts_pll[ts_idx].mdiv));
        SOC_IF_ERROR_RETURN(WRITE_TOP_TS_PLL_CTRL_3r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_CTRL_4r(unit,&rval));
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_4r, &rval, NDIV_INTf,
                          soc_property_get(unit, spn_PTP_TS_PLL_N, ts_pll[ts_idx].ndiv_int));
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_4r, &rval, NDIV_FRACf, ts_pll[ts_idx].ndiv_frac);
        SOC_IF_ERROR_RETURN(WRITE_TOP_TS_PLL_CTRL_4r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_CTRL_5r(unit,&rval));
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_5r, &rval, KAf,
                          soc_property_get(unit, spn_PTP_TS_KA, ts_pll[ts_idx].ka));
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_5r, &rval, KIf,
                          soc_property_get(unit, spn_PTP_TS_KI, ts_pll[ts_idx].ki));
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_5r, &rval, KPf,
                          soc_property_get(unit, spn_PTP_TS_KP, ts_pll[ts_idx].kp));
        SOC_IF_ERROR_RETURN(WRITE_TOP_TS_PLL_CTRL_5r(unit, rval));
    }

    /* Set 250Mhz (implies 4ns resolution) default timesync clock to
       calculate assymentric delays */
    SOC_TIMESYNC_PLL_CLOCK_NS(unit) = (1/250 * 1000); /* clock period in nanoseconds */

    /* Configure BroadSync PLLs. */
    bs_ref_freq = soc_property_get(unit, spn_PTP_BS_FREF, 0);  /* 0->internal reference */

    for (bs_idx = 0; bs_idx < sizeof(bs_pll)/sizeof(bs_pll[0]); ++bs_idx) {
        if (bs_pll[bs_idx].ref_freq == bs_ref_freq) {
            break;
        }
    }
    if (bs_idx == sizeof(bs_pll)/sizeof(bs_pll[0])) {
        /* Valid configuration lookup failed. Use internal reference. */
        bs_idx = 5;
        bs_ref_freq = 0;
    }

        /* PLL settings for internal & external reference */

        bs_ndiv_high = ((soc_property_get(unit, spn_PTP_BS_NDIV_INT,
                            bs_pll[bs_idx].ndiv_int) << 6) |
                       ((soc_property_get(unit, spn_PTP_BS_NDIV_FRAC,
                            bs_pll[bs_idx].ndiv_frac) >> 26) & 0x3f));
        bs_ndiv_low = ((soc_property_get(unit, spn_PTP_BS_NDIV_FRAC,
                            bs_pll[bs_idx].ndiv_frac) << 6) & 0xffffc000);

        /* Configure BS PLL0 */
        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_1r(unit, &rval));
        soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_1r, & rval, FREF_SELf, (bs_ref_freq != 0));
        SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL0_CTRL_1r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_3r(unit, &rval));
        soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_3r, &rval, PDIVf,
                          soc_property_get(unit, spn_PTP_BS_PDIV, bs_pll[bs_idx].pdiv));
        soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_3r, &rval, CH0_MDIVf,
                          soc_property_get(unit, spn_PTP_BS_MNDIV, bs_pll[bs_idx].mdiv));
        SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL0_CTRL_3r(unit, rval));
        /* bs_ndiv_low maps to lower 32 bits (i.e., first 32bits) of the 48 bit Ndiv */
        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_5r(unit,&rval));
        soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_5r, &rval, PLL_FBDIV_0f, bs_ndiv_low);
        SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL0_CTRL_5r(unit, rval));

        /* bs_ndiv_high maps to higher 16 bits (i.e., last 16bits) of the 48 bit Ndiv */
        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_6r(unit,&rval));
        soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_6r, &rval, PLL_FBDIV_1f, bs_ndiv_high);
        SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL0_CTRL_6r(unit, rval));

#if 0
        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_4r(unit, &rval));
        soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_4r, &rval, KAf,
                          soc_property_get(unit, spn_PTP_BS_KA, bs_pll[bs_idx].ka));
        soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_4r, &rval, KIf,
                          soc_property_get(unit, spn_PTP_BS_KI, bs_pll[bs_idx].ki));
        soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_4r, &rval, KPf,
                          soc_property_get(unit, spn_PTP_BS_KP, bs_pll[bs_idx].kp));
        SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL0_CTRL_4r(unit, rval));
#endif

        /* Configure BS PLL1 */
        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL1_CTRL_1r(unit, &rval));
        soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_1r, & rval, FREF_SELf, (bs_ref_freq != 0));
        SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL1_CTRL_1r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL1_CTRL_3r(unit, &rval));
        soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_3r, &rval, PDIVf,
                          soc_property_get(unit, spn_PTP_BS_PDIV, bs_pll[bs_idx].pdiv));
        soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_3r, &rval, CH0_MDIVf,
                          soc_property_get(unit, spn_PTP_BS_MNDIV, bs_pll[bs_idx].mdiv));
        SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL1_CTRL_3r(unit, rval));

        /* bs_ndiv_low maps to lower 32 bits (i.e., first 32bits) of the 48 bit Ndiv */
        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL1_CTRL_5r(unit,&rval));
        soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_5r, &rval, PLL_FBDIV_0f, bs_ndiv_low);
        SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL1_CTRL_5r(unit, rval));

        /* bs_ndiv_high maps to higher 16 bits (i.e., last 16bits) of the 48 bit Ndiv */
        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL1_CTRL_6r(unit,&rval));
        soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_6r, &rval, PLL_FBDIV_1f, bs_ndiv_high);
        SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL1_CTRL_6r(unit, rval));

#if 0
        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL1_CTRL_4r(unit, &rval));
        soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_4r, &rval, KAf,
                          soc_property_get(unit, spn_PTP_BS_KA, bs_pll[bs_idx].ka));
        soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_4r, &rval, KIf,
                          soc_property_get(unit, spn_PTP_BS_KI, bs_pll[bs_idx].ki));
        soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_4r, &rval, KPf,
                          soc_property_get(unit, spn_PTP_BS_KP, bs_pll[bs_idx].kp));
        SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL1_CTRL_4r(unit, rval));

#endif

    /* Set the BS PLL update rate to 12.5M */
    SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_7r(unit, &rval));
    soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_7r, &rval, PROG_DIV_CTRLf, 0x2);
    SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL0_CTRL_7r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL1_CTRL_7r(unit, &rval));
    soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_7r, &rval, PROG_DIV_CTRLf, 0x2);
    SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL1_CTRL_7r(unit, rval));

    /* De-assert nanosync reset before de-asserting PLL's post_resetb */
    SOC_IF_ERROR_RETURN(READ_NANOSYNC_S0_IDM_IDM_ERROR_LOG_COMPLETEr(unit, &rval));
    soc_reg_field_set(unit, NANOSYNC_S0_IDM_IDM_ERROR_LOG_COMPLETEr, &rval, ERROR_COMPLETEf, 1);
    soc_reg_field_set(unit, NANOSYNC_S0_IDM_IDM_ERROR_LOG_COMPLETEr, &rval, OVERFLOW_COMPLETEf, 1);
    SOC_IF_ERROR_RETURN(WRITE_NANOSYNC_S0_IDM_IDM_ERROR_LOG_COMPLETEr(unit, rval));

    SOC_IF_ERROR_RETURN(READ_NANOSYNC_S0_IDM_IDM_RESET_CONTROLr(unit, &rval));
    soc_reg_field_set(unit, NANOSYNC_S0_IDM_IDM_RESET_CONTROLr, &rval,
                        RESETf, 0);
    SOC_IF_ERROR_RETURN(WRITE_NANOSYNC_S0_IDM_IDM_RESET_CONTROLr(unit, rval));

    /* Bring LCPLL, time sync PLL, BroadSync PLL out of reset */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    if (power_reset) {
        for (index = 0; index < COUNTOF(lcpll_reset_field); index = index + 3 ) {
            soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                              lcpll_reset_field[index], 1);
            soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                              lcpll_post_reset_field[index], 1);
        }
    }
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_TS_PLL_RST_Lf,
                      1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_BS_PLL0_RST_Lf,
                      1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_BS_PLL1_RST_Lf,
                      1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_AVS_PMB_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_ARS_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_AVS_PVTMON_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));
    sal_usleep(to_usec);
    SOC_PBMP_CLEAR(cpri_pbmp);
    cpri_pbmp = soc_property_get_pbmp_default(unit,
                                            spn_PBMP_XPORT_CPRI, cpri_pbmp);
    if (!SAL_BOOT_SIMULATION) {
        /* Check LCPLL lock status */
        for (index = 0; index < 4; index++) {
            reg = lcpll_status_reg[index];
            SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
            if (!soc_reg_field_get(unit, reg, rval, TOP_XGPLL_LOCKf)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit, "LCPLL %d not locked on unit %d "
                                 "status = 0x%08x\n"), index, unit, rval));
            }
        }
        /* Check time sync lock status */
        SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_STATUSr(unit, &rval));
        if (!soc_reg_field_get(unit, TOP_TS_PLL_STATUSr, rval, PLL_LOCKf)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit, "TS_PLL not locked on unit %d "
                             "status = 0x%08x\n"), unit, rval));
        }

        /* Check BroadSync lock status */
        rval=0;
        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_STATUSr(unit, &rval));
        if (!soc_reg_field_get(unit, TOP_BS_PLL0_STATUSr, rval, PLL_LOCKf)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit, "BS_PLL0 not locked on unit %d "
                             "status = 0x%08x\n"), unit, rval));
        }

        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL1_STATUSr(unit, &rval));
        if (!soc_reg_field_get(unit, TOP_BS_PLL1_STATUSr, rval, PLL_LOCKf)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit, "BS_PLL1 not locked on unit %d "
                             "status = 0x%08x\n"), unit, rval));
        }
    }

    /* De-assert LCPLL's post reset */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));


    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_TS_PLL_POST_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_BS_PLL0_POST_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_BS_PLL1_POST_RST_Lf, 1);

    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_PVT_MON_MIN_RST_Lf, 1);

    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));
    if (power_reset ) {
        rval = 0 ;
        SOC_IF_ERROR_RETURN(READ_TOP_XG_PLL0_CTRL_5r(unit, &rval));
        soc_reg_field_set(unit, TOP_XG_PLL0_CTRL_5r, &rval,
                          RESERVEDf, 1);
        SOC_IF_ERROR_RETURN(WRITE_TOP_XG_PLL0_CTRL_5r(unit, rval));
    }

    /* Disable BS_PLL0_HO_BYP_ENABLE */
    rval = 0;
    SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROL_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_MISC_CONTROL_2r, &rval,
                        BS_PLL0_HO_BYP_ENABLEf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_MISC_CONTROL_2r(unit, rval));

    /* Check BroadSync lock status */
    rval=0;
    SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_STATUSr(unit, &rval));
    if (!soc_reg_field_get(unit, TOP_BS_PLL0_STATUSr, rval, PLL_LOCKf)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit, "2.BS_PLL0 not locked on unit %d \t"
                         "status = 0x%08x\n"), unit, rval));
    } else {

        /* Disable Legacy HO enable */
        rval = 0 ;
        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_6r(unit, &rval));
        soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_6r, &rval,
                            LEGACY_HO_LOGIC_ENf, 0);
        SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL0_CTRL_6r(unit, rval));


        /* Check BroadSync lock status */
        rval=0;
        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_STATUSr(unit, &rval));
        if (!soc_reg_field_get(unit, TOP_BS_PLL0_STATUSr, rval, PLL_LOCKf)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit, "3. BS_PLL0 not locked on unit %d \t"
                             "status = 0x%08x\n"), unit, rval));
        }
#if 0
        rval=0;
        SOC_IF_ERROR_RETURN(READ_NS_GDPLL_OUTPUT_ARRAY_000_MSBr(unit, &rval));
        LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit, "NS_GDPLL_OUTPUT_ARRAY_000_MSB: 0x%08x\n"), unit, rval));
        rval=0;
        SOC_IF_ERROR_RETURN(READ_NS_GDPLL_OUTPUT_ARRAY_000_LSBr(unit, &rval));
        LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit, "NS_GDPLL_OUTPUT_ARRAY_000_LSB: 0x%08x\n"), unit, rval));
#endif
    }

    /* Disable BS_PLL1_HO_BYP_ENABLE */
    rval = 0;
    SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROL_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_MISC_CONTROL_2r, &rval,
                        BS_PLL1_HO_BYP_ENABLEf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_MISC_CONTROL_2r(unit, rval));

    /* Check BroadSync lock status */
    rval=0;
    SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL1_STATUSr(unit, &rval));
    if (!soc_reg_field_get(unit, TOP_BS_PLL1_STATUSr, rval, PLL_LOCKf)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit, "2.BS_PLL1 not locked on unit %d \t"
                         "status = 0x%08x\n"), unit, rval));
    } else {
        /* Disable Legacy HO enable */
        rval = 0 ;
        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL1_CTRL_6r(unit, &rval));
        soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_6r, &rval,
                            LEGACY_HO_LOGIC_ENf, 0);
        SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL1_CTRL_6r(unit, rval));

        /* Check BroadSync lock status */
        rval=0;
        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL1_STATUSr(unit, &rval));
        if (!soc_reg_field_get(unit, TOP_BS_PLL1_STATUSr, rval, PLL_LOCKf)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit, "3. BS_PLL1 not locked on unit %d \t"
                             "status = 0x%08x\n"), unit, rval));
        }
    }

    rval = 0;
    sal_usleep(to_usec);

    /* Running TS1 time-staper for now.  Might be changed later during
     * time-module porting
     */
    if (!SAL_BOOT_SIMULATION) {

        SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS1_COUNTER_ENABLEr(unit, &rval));
        soc_reg_field_set(unit, NS_TIMESYNC_TS1_COUNTER_ENABLEr, &rval,
                          ENABLEf, 0);
        SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_TS1_COUNTER_ENABLEr(unit, rval));

        SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS1_INIT_ACCUMULATOR_2r(unit, &rval));
        soc_reg_field_set(unit, NS_TIMESYNC_TS1_INIT_ACCUMULATOR_2r, &rval,
                          ACC2f, 0);
        SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_TS1_INIT_ACCUMULATOR_2r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS1_INIT_ACCUMULATOR_1r(unit, &rval));
        soc_reg_field_set(unit, NS_TIMESYNC_TS1_INIT_ACCUMULATOR_1r, &rval,
                          ACC1f, 0);
        SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_TS1_INIT_ACCUMULATOR_1r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS1_INIT_ACCUMULATOR_0r(unit, &rval));
        soc_reg_field_set(unit, NS_TIMESYNC_TS1_INIT_ACCUMULATOR_0r, &rval,
                          ACC0f, 0);
        SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_TS1_INIT_ACCUMULATOR_0r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS1_COUNTER_ENABLEr(unit, &rval));
        soc_reg_field_set(unit, NS_TIMESYNC_TS1_COUNTER_ENABLEr, &rval,
                          ENABLEf, 1);
        SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_TS1_COUNTER_ENABLEr(unit, rval));

        SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS1_FREQ_CTRL_FRAC_UPPERr(unit, &rval));
        soc_reg_field_set(unit, NS_TIMESYNC_TS1_FREQ_CTRL_FRAC_UPPERr, &rval,
                          UINCf, 0x2000);
        SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_TS1_FREQ_CTRL_FRAC_UPPERr(unit, rval));

        SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS1_FREQ_CTRL_FRAC_LOWERr(unit, &rval));
        soc_reg_field_set(unit, NS_TIMESYNC_TS1_FREQ_CTRL_FRAC_LOWERr, &rval,
                          LINCf, 0x0);
        SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_TS1_FREQ_CTRL_FRAC_LOWERr(unit, rval));

        SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS0_COUNTER_ENABLEr(unit, &rval));
        soc_reg_field_set(unit, NS_TIMESYNC_TS0_COUNTER_ENABLEr, &rval,
                          ENABLEf, 0);
        SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_TS0_COUNTER_ENABLEr(unit, rval));

        SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS0_INIT_ACCUMULATOR_2r(unit, &rval));
        soc_reg_field_set(unit, NS_TIMESYNC_TS0_INIT_ACCUMULATOR_2r, &rval,
                          ACC2f, 0);
        SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_TS0_INIT_ACCUMULATOR_2r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS0_INIT_ACCUMULATOR_1r(unit, &rval));
        soc_reg_field_set(unit, NS_TIMESYNC_TS0_INIT_ACCUMULATOR_1r, &rval,
                          ACC1f, 0);
        SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_TS0_INIT_ACCUMULATOR_1r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS0_INIT_ACCUMULATOR_0r(unit, &rval));
        soc_reg_field_set(unit, NS_TIMESYNC_TS0_INIT_ACCUMULATOR_0r, &rval,
                          ACC0f, 0);
        SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_TS0_INIT_ACCUMULATOR_0r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS0_COUNTER_ENABLEr(unit, &rval));
        soc_reg_field_set(unit, NS_TIMESYNC_TS0_COUNTER_ENABLEr, &rval,
                          ENABLEf, 1);
        SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_TS0_COUNTER_ENABLEr(unit, rval));

        SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_UPPERr(unit, &rval));
        soc_reg_field_set(unit, NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_UPPERr, &rval,
                          UINCf, 0x2000);
        SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_UPPERr(unit, rval));

        SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_LOWERr(unit, &rval));
        soc_reg_field_set(unit, NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_LOWERr, &rval,
                          LINCf, 0x0);
        SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_TS0_FREQ_CTRL_FRAC_LOWERr(unit, rval));

        SOC_IF_ERROR_RETURN(READ_NS_TIMESYNC_COUNTER_CONFIG_SELECTr(unit, &rval));
        soc_reg_field_set(unit, NS_TIMESYNC_COUNTER_CONFIG_SELECTr, &rval,
                          ENABLE_COMMON_CONTROLf, 0x0);
        SOC_IF_ERROR_RETURN(WRITE_NS_TIMESYNC_COUNTER_CONFIG_SELECTr(unit, rval));
    }

    /* Set correct value for BG_ADJ */
    SOC_IF_ERROR_RETURN(READ_TOP_PVTMON_CTRL_0r(unit, &rval));
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_0r, &rval, BG_ADJf, 0);
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_0r, &rval, MODEf, 2);
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_CTRL_0r(unit, rval));

    /* Enable high temperature interrupt monitoring.
     * Default on: pvtmon6 (close to core_plls at center of die).
     */
    temp_mask = soc_property_get(unit, "temp_monitor_select", 1<<6);
    /* The above is a 9 bit mask to indicate which temp sensor to hook up to
     * the interrupt.
     */
    rval = 0;
    for (index = 0; index <COUNTOF(temp_thr_reg); index++) {
        uint32 trval;

        /* data = 410.04-(temp/0.48705) = (410040-(temp*1000))/487
         * Note: Since this is a high temp value we can safely assume it to
         * always be a +ive number. This is in degrees celcius.
         */
        temp_thr = soc_property_get(unit, temp_thr_prop[index], _SOC_MN_DEF_TEMP_MAX);
        if (temp_thr > 410) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Unsupported temp value %d !! Max 410. Using %d.\n"),
                       temp_thr, _SOC_MN_DEF_TEMP_MAX));
            temp_thr = _SOC_MN_DEF_TEMP_MAX;
        }
        /* Convert temperature to config data */
        temp_thr = (410040-(temp_thr*1000))/487;
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, temp_thr_reg[index], REG_PORT_ANY, 0, &trval));
        soc_reg_field_set(unit, temp_thr_reg[index], &trval, MAX_THRESHOLDf,
                          temp_thr);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, temp_thr_reg[index], REG_PORT_ANY, 0, trval));
        if (temp_mask & (1 << index)) {
            rval |= (1 << ((index * 2) + 1)); /* 2 bits per pvtmon, using min */
        }
    }
    _soc_mn_temp_mon_mask[unit] = temp_mask;

    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_MASKr(unit, rval));
    /* Enable temp mon interrupt */
    (void)soc_cmicm_intr3_enable(unit, 0x4); /* PVTMON_INTR bit 2 */

    /* Bring port blocks out of reset */
    rval = 0;
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_PGW0_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_PGW1_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_TS_RST_Lf, 1);
    WRITE_TOP_SOFT_RESET_REGr(unit, rval);

    sal_usleep(to_usec);

    /* Bring IP, EP, and MMU blocks out of reset */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MMU_RST_Lf, 1);

    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_IP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_EP_RST_Lf, 1);

    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));

    sal_usleep(to_usec);

    /* Bring PMs out of reset */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_3r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval, TOP_PM0_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval, TOP_PM1_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval, TOP_PM2_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval, TOP_PM3_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval, TOP_PM4_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval, TOP_PM5_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval, TOP_PM6_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval, TOP_PM7_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval, TOP_PM8_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval, TOP_PM9_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval, TOP_PM10_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval, TOP_PM11_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval, TOP_PM12_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval, TOP_PM13_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval, TOP_PM14_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval, TOP_PM15_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_3r, &rval, TOP_MACSEC_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_3r(unit, rval));
    sal_usleep(to_usec);



    num_ecmp_rh_flowset_entries = soc_property_get(unit,
            spn_ECMP_RESILIENT_HASH_SIZE, 32768);
    switch (num_ecmp_rh_flowset_entries) {
        case 0:
            rh_table_config_encoding = 2;
            break;
        case 32768:
            rh_table_config_encoding = 0;
            break;
        case 65536:
            rh_table_config_encoding = 1;
            break;
        default:
            return SOC_E_CONFIG;
    }

#if defined(BCM_RIOT_SUPPORT) || defined(BCM_MULTI_LEVEL_ECMP_SUPPORT)
    ecmp_levels = soc_property_get(unit, spn_L3_ECMP_LEVELS, 1);
    if ((soc_feature(unit, soc_feature_riot) ||
         soc_feature(unit, soc_feature_multi_level_ecmp)) &&
        ecmp_levels > 1) {

        num_overlay_ecmp_rh_flowset_entries = soc_property_get(unit,
            spn_RIOT_OVERLAY_ECMP_RESILIENT_HASH_SIZE, 0);

        if (SOC_FIELD_RANGE_CHECK(
            num_overlay_ecmp_rh_flowset_entries, 0, 16384)) {

            num_overlay_ecmp_rh_flowset_entries = 16384;
        } else if (SOC_FIELD_RANGE_CHECK(
            num_overlay_ecmp_rh_flowset_entries, 16384, 32768)) {

            num_overlay_ecmp_rh_flowset_entries = 32768;
        } else if (SOC_FIELD_RANGE_CHECK(
            num_overlay_ecmp_rh_flowset_entries, 32768, 49152)) {

            num_overlay_ecmp_rh_flowset_entries = 49152;
        } else if (SOC_FIELD_RANGE_CHECK(
            num_overlay_ecmp_rh_flowset_entries, 49152, 65536)) {

            num_overlay_ecmp_rh_flowset_entries = 65536;
        }

        switch (num_overlay_ecmp_rh_flowset_entries) {
        case 0:
            rh_bank_sel = 0x0;
            break;
        case 16384:
            if (num_ecmp_rh_flowset_entries >= 16384) {
                rh_bank_sel = 0x1;
            } else {
                LOG_ERROR(BSL_LS_BCM_COMMON, (BSL_META_U(unit,
                    "%s:Total ECMP entries %d are less than RH entries :%d \n"),
                    FUNCTION_NAME(),  num_ecmp_rh_flowset_entries,
                    num_overlay_ecmp_rh_flowset_entries));
                return SOC_E_CONFIG;
            }
            break;
        case 32768:
            if (num_ecmp_rh_flowset_entries >= 32768) {
                rh_bank_sel = 0x3;
            } else {
                LOG_ERROR(BSL_LS_BCM_COMMON, (BSL_META_U(unit,
                    "%s:Total ECMP entries %d are less than RH entries :%d \n"),
                    FUNCTION_NAME(),  num_ecmp_rh_flowset_entries,
                    num_overlay_ecmp_rh_flowset_entries));
                return SOC_E_CONFIG;
            }
            break;
        case 49152:
            if (num_ecmp_rh_flowset_entries >= 49152) {
                rh_bank_sel = 0x7;
            } else {
                LOG_ERROR(BSL_LS_BCM_COMMON, (BSL_META_U(unit,
                    "%s:Total ECMP entries %d are less than RH entries :%d \n"),
                    FUNCTION_NAME(),  num_ecmp_rh_flowset_entries,
                    num_overlay_ecmp_rh_flowset_entries));
                return SOC_E_CONFIG;
            }
            break;
        case 65536:
            if (num_ecmp_rh_flowset_entries >= 65536) {
                rh_bank_sel = 0xf;
            } else {
                LOG_ERROR(BSL_LS_BCM_COMMON, (BSL_META_U(unit,
                    "%s:Total ECMP entries %d are less than RH entries :%d \n"),
                    FUNCTION_NAME(),  num_ecmp_rh_flowset_entries,
                    num_overlay_ecmp_rh_flowset_entries));
                return SOC_E_CONFIG;
            }
            break;
        default:
                LOG_ERROR(BSL_LS_BCM_COMMON, (BSL_META_U(unit,
                    "%s: Value for overlay RH entries is not correct : %d \n"),
                    FUNCTION_NAME(),  num_overlay_ecmp_rh_flowset_entries));
            return SOC_E_CONFIG;
    }
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, RH_ECMP_FLOWSET_BANK_SELr,
                REG_PORT_ANY, BANK_SELf,
                rh_bank_sel));
  }
#endif
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, ENHANCED_HASHING_CONTROLr,
                REG_PORT_ANY, RH_FLOWSET_TABLE_CONFIG_ENCODINGf,
                rh_table_config_encoding));

    READ_TOP_PVTMON_CTRL_1r(unit, &rval);
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON_ADC_RESETBf, 1);
    WRITE_TOP_PVTMON_CTRL_1r(unit, rval);
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON_ADC_RESETBf, 0);
    WRITE_TOP_PVTMON_CTRL_1r(unit, rval);
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON_ADC_RESETBf, 1);
    WRITE_TOP_PVTMON_CTRL_1r(unit, rval);
    /* Bypass unused Port Blocks */
     /* Bypass unused Port Blocks */
    if (soc_property_get(unit, "disable_unused_port_blocks", TRUE)) {
        SOC_IF_ERROR_RETURN(soc_monterey_tsc_disable(unit));
    }


    /* Do not use SBUS MDIO by default */
    SOC_FUNCTIONS(unit)->soc_sbus_mdio_read = NULL;
    SOC_FUNCTIONS(unit)->soc_sbus_mdio_write = NULL;

    /* Optionally allow SBUS MDIO for debug */
    if (soc_property_get(unit, "monterey_sbus_mdio", 1)) {
        SOC_FUNCTIONS(unit)->soc_sbus_mdio_read = _soc_monterey_mdio_reg_read;
        SOC_FUNCTIONS(unit)->soc_sbus_mdio_write = _soc_monterey_mdio_reg_write;
    }

    if (!(SOC_CONTROL(unit)->soc_flags & SOC_F_ALL_MODULES_INITED) &&
            soc_feature(unit, soc_feature_turbo_boot)) {
        SOC_HW_RESET_START(unit);
    }

    return SOC_E_NONE;
}

int
soc_monterey_tsc_reset(int unit)
{
    int blk, port;
    uint32 rval;
    uint64 rval64;


    /* Set tsc reference clock connection */
    SOC_IF_ERROR_RETURN(_soc_monterey_tsc_refclk_set(unit));

    /* Though TSC/MAC reset is controlled by PortMod internally,
     * this is needed in case access to MAC registers is needed
     * before PortMod attach for cases like:
     * - To enable SER MAC interrupts
     * - Various sanity register read/write tests
     */

    SOC_BLOCK_ITER(unit, blk, SOC_BLK_CLPORT) {
        port = SOC_BLOCK_PORT(unit, blk);
        if(!(monterey_tsc_is_master_pll(SOC_INFO(unit).port_serdes[port]))) {
            SOC_IF_ERROR_RETURN(soc_tsc_xgxs_reset(unit, port, 0));
        }

        SOC_IF_ERROR_RETURN(READ_CLPORT_MAC_CONTROLr(unit, port, &rval));
        soc_reg_field_set(unit, CLPORT_MAC_CONTROLr, &rval, XMAC0_RESETf, 0);
        SOC_IF_ERROR_RETURN(WRITE_CLPORT_MAC_CONTROLr(unit, port, rval));
    }

    SOC_BLOCK_ITER(unit, blk, SOC_BLK_XLPORT) {
        port = SOC_BLOCK_PORT(unit, blk);
        if(!(monterey_tsc_is_master_pll(SOC_INFO(unit).port_serdes[port]))) {
            SOC_IF_ERROR_RETURN(soc_tsc_xgxs_reset(unit, port, 0));
        }

        SOC_IF_ERROR_RETURN(READ_XLPORT_MAC_CONTROLr(unit, port, &rval));
        soc_reg_field_set(unit, XLPORT_MAC_CONTROLr, &rval, XMAC0_RESETf, 0);
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_MAC_CONTROLr(unit, port, rval));
    }

    PBMP_PORT_ITER(unit, port) {
        if(IS_ROE_PORT(unit, port)) {
            /* PLL0 select pll0_lc_refclk for CPRI, keep PLL1 pll1_lc_refclk for Ethernet */
            SOC_IF_ERROR_RETURN(soc_reg_get(unit, CLPORT_XGXS0_CTRL_REGr, port, 0, &rval64));
            soc_reg64_field32_set(unit, CLPORT_XGXS0_CTRL_REGr, &rval64, PLL0_LC_REFSELf, 0);
            SOC_IF_ERROR_RETURN(soc_reg_set(unit, CLPORT_XGXS0_CTRL_REGr, port, 0, rval64));

            SOC_IF_ERROR_RETURN(READ_TOP_CTRL_CONFIGr(unit, port, &rval));
            soc_reg_field_set(unit, TOP_CTRL_CONFIGr, &rval, CIP_TOP_CTRLf, 0);
            soc_reg_field_set(unit, TOP_CTRL_CONFIGr, &rval, PMD_IDDQf, 0);
            soc_reg_field_set(unit, TOP_CTRL_CONFIGr, &rval, CIP_TOP_CTRLf, 0);
            SOC_IF_ERROR_RETURN(WRITE_TOP_CTRL_CONFIGr(unit, port, rval));
            SOC_IF_ERROR_RETURN(READ_CPMPORT_PMD_CTRLr(unit, port, &rval));
            soc_reg_field_set(unit, CPMPORT_PMD_CTRLr, &rval, PMD_POR_H_RSTBf, 0);
            SOC_IF_ERROR_RETURN(WRITE_CPMPORT_PMD_CTRLr(unit, port, rval));

            if (!SAL_BOOT_PLISIM) {
                SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &rval));
                soc_reg_field_set(unit, CPRI_RST_CTRLr, &rval, RESET_TX_Hf, 0);
                soc_reg_field_set(unit, CPRI_RST_CTRLr, &rval, RESET_RX_Hf, 0);
                soc_reg_field_set(unit, CPRI_RST_CTRLr, &rval, RESET_CIP_TX_Hf, 0);
                soc_reg_field_set(unit, CPRI_RST_CTRLr, &rval, RESET_CIP_RX_Hf, 0);
                soc_reg_field_set(unit, CPRI_RST_CTRLr, &rval, RESET_DATAPATH_RX_Hf, 0);
                soc_reg_field_set(unit, CPRI_RST_CTRLr, &rval, RESET_DATAPATH_TX_Hf, 0);
                SOC_IF_ERROR_RETURN(WRITE_CPRI_RST_CTRLr(unit, port, rval));
            }
        }
    }
/*
*/
    return SOC_E_NONE;
}


STATIC int
_soc_monterey_clear_enabled_port_data (int unit)
{
    uint64 rval64;
    int    port;
    uint32 evc = 0;
    egr_vlan_control_1_entry_t entry1;
    egr_vlan_control_2_entry_t entry2;
    egr_vlan_control_3_entry_t entry3;

    sal_memset(&entry1, 0, sizeof(entry1));
    sal_memset(&entry2, 0, sizeof(entry2));
    sal_memset(&entry3, 0, sizeof(entry3));

    /* Some registers are implemented in memory, need to clear them in order
     * to have correct parity value */
    COMPILER_64_ZERO(rval64);
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_EGR_VLAN_CONTROL_1m(unit, MEM_BLOCK_ANY, port, &entry1));
        SOC_IF_ERROR_RETURN(WRITE_EGR_VLAN_CONTROL_2m(unit, MEM_BLOCK_ANY, port, &entry2));
        SOC_IF_ERROR_RETURN(WRITE_EGR_VLAN_CONTROL_3m(unit, MEM_BLOCK_ANY, port, &entry3));

        SOC_IF_ERROR_RETURN(WRITE_EGR_PVLAN_EPORT_CONTROLr(unit, port, 0));
        SOC_IF_ERROR_RETURN(WRITE_EGR_SF_SRC_MODID_CHECKr(unit, port, rval64));
        SOC_IF_ERROR_RETURN(WRITE_EGR_1588_LINK_DELAY_64r(unit, port, rval64));
        SOC_IF_ERROR_RETURN(WRITE_EGR_IPMC_CFG2m(unit, MEM_BLOCK_ALL, port, &evc));
        SOC_IF_ERROR_RETURN(WRITE_STORM_CONTROL_METER_CONFIGr(unit, port, 0));
        SOC_IF_ERROR_RETURN(WRITE_SFLOW_ING_THRESHOLDr(unit, port, 0));
    }

    return SOC_E_NONE;
}

STATIC int
_soc_monterey_clear_all_port_data (int unit)
{
    int    r, p;
    uint32 addr;

    SOC_IF_ERROR_RETURN(_soc_monterey_clear_enabled_port_data(unit));

    

    /* Clear TDBGCn regs for all indexes */
    for (r = 0; r < 12; r++) {
        addr = 0x28000000 + r*0x100;
        for (p = 0; p < 106; p++) {
            _soc_reg32_set(unit, 0x2, 1, addr+p, 0);
            _soc_reg32_set(unit, 0x2, 2, addr+p, 0);
        }
    }

    /* Clear TPCE */
    addr = 0x28000f00;
    for (p = 0; p < 106; p++) {
        _soc_reg32_set(unit, 0x2, 1, addr+p, 0);
        _soc_reg32_set(unit, 0x2, 2, addr+p, 0);
    }
    return SOC_E_NONE;
}

STATIC int
_soc_monterey_clear_all_memory(int unit, int mmu_init)
{
    int                 blk;
    int                 pgw, pgw_inst, num_pgw = 0;
    uint32              rval, in_progress[1];
    int                 hw_reset_to, index, count;
    soc_reg_t           hw_reset_reg;
    soc_timeout_t       to;
    static const struct {
        soc_mem_t mem;
        uint32 skip_flags; /* always skip on QUICKTURN or XGSSIM */
    } cam_list[] = {
        { CPU_COS_MAPm,                     BOOT_F_PLISIM },
        { EFP_TCAMm,                        BOOT_F_PLISIM },
        { FP_GLOBAL_MASK_TCAMm,             BOOT_F_PLISIM },
        { FP_TCAMm,                         BOOT_F_PLISIM },
        { VFP_TCAMm,                        BOOT_F_PLISIM },
        { FP_UDF_TCAMm,                     BOOT_F_PLISIM },
        { ING_SNATm,                        BOOT_F_PLISIM },
        { IP_MULTICAST_TCAMm,               BOOT_F_PLISIM },
        { L2_USER_ENTRYm,                   BOOT_F_PLISIM },
        { L3_DEFIPm,                        BOOT_F_PLISIM },
        { L3_DEFIP_PAIR_128m,               BOOT_F_PLISIM },
        { L3_TUNNELm,                       BOOT_F_PLISIM },
        { MY_STATION_TCAMm,                 BOOT_F_PLISIM },
        { UDF_CONDITIONAL_CHECK_TABLE_CAMm, BOOT_F_PLISIM },
        { MY_STATION_TCAM_2m,               BOOT_F_PLISIM },
        { VLAN_SUBNETm,                     0 },/* VLAN API needs all 0 mask */
        { MY_STATION_TCAM_2m,               BOOT_F_PLISIM },
#ifdef PLISIM
        /* In HW, these are the same as FP_GLOBAL_MASK_TCAM.
         * In simulation, they are separate instances.
         */
        { FP_GM_FIELDSm,                    BOOT_F_PLISIM }
#endif
    };

    if (mmu_init) {
        uint32 rval1;
        /* Start MMU memory initialization */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, MISCCONFIGr, REG_PORT_ANY, INIT_MEMf, 0));

        SOC_IF_ERROR_RETURN(READ_MMU_CCP_EN_COR_ERR_RPTr(unit, &rval1));
        soc_reg_field_set(unit, MMU_CCP_EN_COR_ERR_RPTr, &rval1,CCP0_RESEQf, 0);
        SOC_IF_ERROR_RETURN(WRITE_MMU_CCP_EN_COR_ERR_RPTr(unit, rval1));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, MISCCONFIGr, REG_PORT_ANY, INIT_MEMf, 1));

        sal_usleep(SAL_BOOT_QUICKTURN ? 100000 : 10); /* Allow things to stabalize */
        soc_reg_field_set(unit, MMU_CCP_EN_COR_ERR_RPTr, &rval1,CCP0_RESEQf, 1);
        SOC_IF_ERROR_RETURN(WRITE_MMU_CCP_EN_COR_ERR_RPTr(unit, rval1));
    }

    SOC_IF_ERROR_RETURN
        (_soc_monterey_mmu_init_default_val(unit));

    /*
     * Reset the PGW, IPIPE and EPIPE block
     */
    rval = 0;
    hw_reset_reg = PGW_ING_HW_RESET_CONTROL_2r;
    soc_reg_field_set(unit, hw_reset_reg, &rval, RESET_ALLf, 1);
    soc_reg_field_set(unit, hw_reset_reg, &rval, VALIDf, 1);

    SOC_BLOCK_ITER(unit, blk, SOC_BLK_PGW_CL) {
        num_pgw += 1;
        pgw = SOC_BLOCK_NUMBER(unit, blk);
        pgw_inst = (pgw | SOC_REG_ADDR_INSTANCE_MASK);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, hw_reset_reg, pgw_inst, 0, rval));
    }

    rval = 0;
    hw_reset_reg = ING_HW_RESET_CONTROL_2r;
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_1r(unit, rval));
    soc_reg_field_set(unit, hw_reset_reg, &rval, RESET_ALLf, 1);
    soc_reg_field_set(unit, hw_reset_reg, &rval, VALIDf, 1);

    /* Set count to # entries of largest IPIPE table */
    count = (1 << soc_reg_field_length(unit, hw_reset_reg, COUNTf)) - 1;
    soc_reg_field_set(unit, hw_reset_reg, &rval, COUNTf, count);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, hw_reset_reg, REG_PORT_ANY, 0, rval));

    sal_usleep(500);
    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_0r(unit, rval));

    hw_reset_reg = EGR_HW_RESET_CONTROL_1r;
    soc_reg_field_set(unit, hw_reset_reg, &rval, RESET_ALLf, 1);
    soc_reg_field_set(unit, hw_reset_reg, &rval, VALIDf, 1);
    /* Set count to # entries of largest EPIPE table (EGR_L3_NEXT_HOP) */
    count = (1 << soc_reg_field_length(unit, hw_reset_reg, COUNTf)) - 1;
    soc_reg_field_set(unit, hw_reset_reg, &rval, COUNTf, count);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, hw_reset_reg, REG_PORT_ANY, 0, rval));

    sal_usleep(500);
    /* For simulation, set timeout to 10 sec.  Otherwise, timeout = 50 ms */
    if (SAL_BOOT_QUICKTURN) {
        hw_reset_to = 10000000;
    } else {
        hw_reset_to = 50000;
    }
    soc_timeout_init(&to, hw_reset_to, 0);

    /* Wait for PGW memory initialization done. */
    hw_reset_reg = PGW_ING_HW_RESET_CONTROL_2r;
    /*  in_progress = ((1 << num_pgw) - 1); */  /* 1b for each PGW */
    in_progress[0] = 0;
    SHR_BITSET_RANGE(in_progress, 0, num_pgw);  /* 1b for each PGW */

    do {
        SOC_BLOCK_ITER(unit, blk, SOC_BLK_PGW_CL) {
            pgw = SOC_BLOCK_NUMBER(unit, blk);
            pgw_inst = (pgw | SOC_REG_ADDR_INSTANCE_MASK);
            if (SHR_BITGET(in_progress, pgw)) { /* PGW RESET not done yet */
                SOC_IF_ERROR_RETURN(soc_reg32_get(unit, hw_reset_reg,
                                                  pgw_inst, 0, &rval));

                if (soc_reg_field_get(unit, hw_reset_reg, rval, DONEf)) {
                    SHR_BITCLR(in_progress, pgw); /* Clear bit corresponding to the PGW */
                }
            }
        }

        if (soc_timeout_check(&to)) {
            if (!SAL_BOOT_BCMSIM) {
                LOG_WARN(BSL_LS_SOC_COMMON,
                         (BSL_META_U(unit, "PGW_ING_HW_RESET timeout\n")));
            }
            break;
        }
    } while (in_progress[0] != 0);

    /* Wait for EPIPE memory initialization done. */
    in_progress[0] = 1;
    hw_reset_reg = EGR_HW_RESET_CONTROL_1r;
    do {
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, hw_reset_reg, REG_PORT_ANY, 0, &rval));
        if (soc_reg_field_get(unit, hw_reset_reg, rval, DONEf)) {
            in_progress[0] = 0;
        }
        if (soc_timeout_check(&to)) {
            if (!SAL_BOOT_BCMSIM) {
                LOG_WARN(BSL_LS_SOC_COMMON,
                         (BSL_META_U(unit, "EGR_HW_RESET timeout\n")));
            }
            break;
        }
    } while (in_progress[0] != 0);

    /* Wait for IPIPE memory initialization done. */
    in_progress[0] = 1;
    hw_reset_reg = ING_HW_RESET_CONTROL_2r;
    do {
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, hw_reset_reg, REG_PORT_ANY, 0, &rval));
        if (soc_reg_field_get(unit, hw_reset_reg, rval, DONEf)) {
            in_progress[0] = 0;
        }

        if (soc_timeout_check(&to)) {
            if (!SAL_BOOT_BCMSIM) {
                LOG_WARN(BSL_LS_SOC_COMMON,
                         (BSL_META_U(unit, "ING_HW_RESET timeout\n")));
            }
            break;
        }
    } while (in_progress[0] != 0);

    /* TCAM tables are not handled by hardware reset control */
    if (!SAL_BOOT_QUICKTURN && !SAL_BOOT_XGSSIM && !SAL_BOOT_BCMSIM) {
        for (index = 0; index < sizeof(cam_list) / sizeof(cam_list[0]);
             index++) {
            if (sal_boot_flags_get() & cam_list[index].skip_flags) {
                continue;
            }
            SOC_IF_ERROR_RETURN
                (soc_mem_clear(unit, cam_list[index].mem, COPYNO_ALL, TRUE));
        }
    }

    return SOC_E_NONE;
}


STATIC int
_soc_monterey_port_mapping_init(int unit)
{
    soc_info_t *si;
    soc_mem_t mem;
    uint32 rval;
    ing_physical_to_logical_port_number_mapping_table_entry_t entry;
    int port, phy_port;
    int num_port, num_phy_port;

    si = &SOC_INFO(unit);

    /* Ingress physical to logical port mapping */
    mem = ING_PHYSICAL_TO_LOGICAL_PORT_NUMBER_MAPPING_TABLEm;
    num_phy_port = soc_mem_index_count(unit, mem);
    sal_memset(&entry, 0, sizeof(entry));
    for (phy_port = 0; phy_port < num_phy_port; phy_port++) {
        port = si->port_p2l_mapping[phy_port];
        soc_mem_field32_set(unit, mem, &entry, LOGICAL_PORT_NUMBERf,
                            port == -1 ? 0x7f : port);
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, phy_port, &entry));
    }

    /* Ingress logical to physical port mapping */
    num_port = soc_mem_index_count(unit, PORT_TABm) - 1;

    for (port = 0; port < num_port; port++) {
        phy_port = si->port_l2p_mapping[port];
        rval = 0;
        if (phy_port != -1) {
            soc_reg_field_set(unit, IFP_GM_LOGICAL_TO_PHYSICAL_MAPPINGr, &rval,
                              VALIDf, 1);

            /* IFP_GM_LOGICAL_TO_PHYSICAL_MAPP?ING.PHYSICAL_PORT_NUM is
             * merely a unique stream ID, it's not physical port number */
            soc_reg_field_set(unit, IFP_GM_LOGICAL_TO_PHYSICAL_MAPPINGr, &rval,
                              PHYSICAL_PORT_NUMf,
                              si->port_p2m_mapping[phy_port] & 0x7f);
        }
        SOC_IF_ERROR_RETURN
            (WRITE_IFP_GM_LOGICAL_TO_PHYSICAL_MAPPINGr(unit, port, rval));
    }

    /* Egress logical to physical port mapping */
    for (port = 0; port < num_port; port++) {
        phy_port = si->port_l2p_mapping[port];
        rval = 0;
        soc_reg_field_set(unit, EGR_LOGICAL_TO_PHYSICAL_PORT_NUMBER_MAPPINGr,
                          &rval, PHYSICAL_PORT_NUMBERf,
                          phy_port == -1 ? 0x7f : phy_port);
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_LOGICAL_TO_PHYSICAL_PORT_NUMBER_MAPPINGr(unit, port,
                                                                rval));
    }

    /* MMU to physical port mapping and MMU to logical port mapping */
    for (port = 0; port < num_port; port++) {
        phy_port = si->port_l2p_mapping[port];
        if (phy_port == -1) {
            continue;
        }
        rval = 0;
        soc_reg_field_set(unit, MMU_PORT_TO_PHY_PORT_MAPPINGr, &rval,
                          PHY_PORTf, phy_port);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, MMU_PORT_TO_PHY_PORT_MAPPINGr, port, 0,
                           rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_PORT_TO_LOGIC_PORT_MAPPINGr, &rval,
                          LOGIC_PORTf, port);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, MMU_PORT_TO_LOGIC_PORT_MAPPINGr, port, 0,
                           rval));
    }

    return SOC_E_NONE;
}

#define MONTEREY_PGW_TDM_LENGTH                 MN_LR_LLS_LEN
#define MONTEREY_PGW_TDM_OVS_SIZE               MN_OS_LLS_GRP_LEN
#define MONTEREY_PGW_TDM_SLOTS_PER_REG          4
#define MONTEREY_MMU_TDM_LENGTH                 512
#define MONTEREY_MMU_OVS_GROUP_COUNT            MN_OS_VBS_GRP_NUM
#define MONTEREY_MMU_OVS_WT_GROUP_COUNT         8
#define MONTEREY_MMU_OVS_GROUP_TDM_LENGTH       MN_OS_VBS_GRP_LEN
#define MONTEREY_IARB_TDM_LENGTH                MN_LR_IARB_STATIC_LEN


#define OVS_TOKEN MN_OVSB_TOKEN
#define IDL1_TOKEN MN_IDL1_TOKEN
#define IDL2_TOKEN MN_IDL2_TOKEN
#define AUX_TOKEN  MN_AUX_TOKEN
#define SBUS_TOKEN MN_SBUS_TOKEN

#define NUM_EXT_PORTS MN_NUM_EXT_PORTS

#define PORT_STATE_UNUSED          0   /*  PORT_STATE__DISABLED */
#define PORT_STATE_LINERATE        1   /*  PORT_STATE__LINERATE */
#define PORT_STATE_OVERSUB         2   /*  PORT_STATE__OVERSUB  */
#define PORT_STATE_SUBPORT         3   /*  PORT_STATE__COMBINED */

enum _mn_ovs_speed_classes {
    mn_ovs_speed_None = 0,
    mn_ovs_speed_5G   = 2,
    mn_ovs_speed_10G  = 4,
    mn_ovs_speed_20G  = 8,
    mn_ovs_speed_25G  = 10,
    mn_ovs_speed_40G  = 16,
    mn_ovs_speed_50G  = 20,
    mn_ovs_speed_100G = 40
};

/*
 * MMU OVS TDM min. slot bandwidth is 5G.
 * Anything less than 5G will be grouped together and will have provision for up to 5G.
 */
#define MONTEREY_TDM_MMU_SPEED_GET(unit, speed)             \
    do {                                                  \
        speed = SOC_PORTCTRL_HG2_TO_IEEE_BW_GET(speed);   \
        if ((speed <= 5000) && (speed > 0)) {             \
            speed = 5000;                                 \
        }                                                 \
    } while (0)

 int
_soc_monterey_mmu_ovs_speed_class_map_get(int unit, int *speed, int *spg, uint32 *sp_spacing)
{
 soc_info_t *si = &SOC_INFO(unit);
 MONTEREY_TDM_MMU_SPEED_GET(unit, *speed);
    switch (*speed) {
        case 5000:
            *sp_spacing = 0x14;
            *spg = mn_ovs_speed_5G;
            break;
        case 10000:
            *sp_spacing = 0x14;
            *spg = mn_ovs_speed_10G;
            break;
        case 20000:
            *sp_spacing = 0x0a;
            *spg = mn_ovs_speed_20G;
            break;
        case 25000:
            *sp_spacing = 0x0a;
            *spg = mn_ovs_speed_25G;
            break;
        case 40000:
            if (si->frequency == 550)
            {
                *sp_spacing = 0x8;
            } else if(si->frequency == 450 ) {
                *sp_spacing = 0x4;
            } else {
                *sp_spacing = 0x0a;
            }
            *spg = mn_ovs_speed_40G;
            break;
        case 50000:
            if (si->frequency == 667)
            {
                *sp_spacing = 0x8;
            } else if(si->frequency == 450 || si->frequency == 550 ) {
                *sp_spacing = 0x4;
            } else {
                *sp_spacing = 0x0a;
            }
            *spg = mn_ovs_speed_50G;
            break;
        case 100000:
            *sp_spacing = 0x04;
            *spg = mn_ovs_speed_100G;
            break;
        default:
            *sp_spacing = 0x0;
            *spg = mn_ovs_speed_None;
            return SOC_E_FAIL;
    }

    return SOC_E_NONE;

}


typedef struct soc_monterey_tdm_s {
    int tdm_bw;
    int is_oversub;

    int speed[NUM_EXT_PORTS];
    int port_state[NUM_EXT_PORTS];

    int pgw_tdm[MONTEREY_PGWS_PER_DEV][MONTEREY_PGW_TDM_LENGTH];
    int mmu_tdm[MONTEREY_MMU_TDM_LENGTH + 1];

    struct {
        int pgw_ovs_tdm[MONTEREY_PGWS_PER_DEV][MONTEREY_PGW_TDM_LENGTH];
        int pgw_ovs_spacing[MONTEREY_PGWS_PER_DEV][MONTEREY_PGW_TDM_LENGTH];
    } ingress_ovs_tdm;

    struct {
        int mmu_ovs_group_tdm[MONTEREY_MMU_OVS_GROUP_COUNT][MONTEREY_MMU_OVS_GROUP_TDM_LENGTH];
        int mmu_ovs_group_speed[MONTEREY_MMU_OVS_GROUP_COUNT]; /* Speed for each OVS grp */
        int mmu_ovs_group_wt_group[MONTEREY_MMU_OVS_GROUP_COUNT]; /* Weighti group for each OVS grp */
    } egress_ovs_tdm;

    int iarb_tdm_wrap_ptr;
    int iarb_tdm[MONTEREY_IARB_TDM_LENGTH];
    int pgw0len;
    int pgw1len;
} soc_monterey_tdm_t;



struct soc_monterey_misc_info_s {
    int tdm_active_cal;
    uint16 tdm_wtg_used[MONTEREY_MMU_OVS_WT_GROUP_COUNT];
    soc_monterey_tdm_t tdm0;
    soc_monterey_tdm_t tdm1;
} *soc_monterey_misc_info[SOC_MAX_NUM_DEVICES] = {0};

#define MN_MISC(unit) (soc_monterey_misc_info[unit])




static const soc_field_t mn_mmu_ovs_group_wt_select_fields[] = {
    GRP0f, GRP1f, GRP2f, GRP3f, GRP4f, GRP5f, GRP6f, GRP7f
};


static const soc_reg_t mn_pgw_lr_tdm_regs[2][40] = {
    {
        PGW_LR_TDM_REG_0r,  PGW_LR_TDM_REG_1r,
        PGW_LR_TDM_REG_2r,  PGW_LR_TDM_REG_3r,
        PGW_LR_TDM_REG_4r,  PGW_LR_TDM_REG_5r,
        PGW_LR_TDM_REG_6r,  PGW_LR_TDM_REG_7r,
        PGW_LR_TDM_REG_8r,  PGW_LR_TDM_REG_9r,
        PGW_LR_TDM_REG_10r, PGW_LR_TDM_REG_11r,
        PGW_LR_TDM_REG_12r, PGW_LR_TDM_REG_13r,
        PGW_LR_TDM_REG_14r, PGW_LR_TDM_REG_15r,
        PGW_LR_TDM_REG_16r,  PGW_LR_TDM_REG_17r,
        PGW_LR_TDM_REG_18r,  PGW_LR_TDM_REG_19r,
        PGW_LR_TDM_REG_20r,  PGW_LR_TDM_REG_21r,
        PGW_LR_TDM_REG_22r,  PGW_LR_TDM_REG_23r,
        PGW_LR_TDM_REG_24r,  PGW_LR_TDM_REG_25r,
        PGW_LR_TDM_REG_26r, PGW_LR_TDM_REG_27r,
        PGW_LR_TDM_REG_28r, PGW_LR_TDM_REG_29r,
        PGW_LR_TDM_REG_30r, PGW_LR_TDM_REG_31r,
        PGW_LR_TDM_REG_32r, PGW_LR_TDM_REG_33r,
        PGW_LR_TDM_REG_34r, PGW_LR_TDM_REG_35r,
        PGW_LR_TDM_REG_36r, PGW_LR_TDM_REG_37r,
        PGW_LR_TDM_REG_38r, PGW_LR_TDM_REG_39r
    },
    {
        PGW_LR_TDM2_REG_0r,  PGW_LR_TDM2_REG_1r,
        PGW_LR_TDM2_REG_2r,  PGW_LR_TDM2_REG_3r,
        PGW_LR_TDM2_REG_4r,  PGW_LR_TDM2_REG_5r,
        PGW_LR_TDM2_REG_6r,  PGW_LR_TDM2_REG_7r,
        PGW_LR_TDM2_REG_8r,  PGW_LR_TDM2_REG_9r,
        PGW_LR_TDM2_REG_10r, PGW_LR_TDM2_REG_11r,
        PGW_LR_TDM2_REG_12r, PGW_LR_TDM2_REG_13r,
        PGW_LR_TDM2_REG_14r, PGW_LR_TDM2_REG_15r,
        PGW_LR_TDM2_REG_16r,  PGW_LR_TDM2_REG_17r,
        PGW_LR_TDM2_REG_18r,  PGW_LR_TDM2_REG_19r,
        PGW_LR_TDM2_REG_20r,  PGW_LR_TDM2_REG_21r,
        PGW_LR_TDM2_REG_22r,  PGW_LR_TDM2_REG_23r,
        PGW_LR_TDM2_REG_24r,  PGW_LR_TDM2_REG_25r,
        PGW_LR_TDM2_REG_26r, PGW_LR_TDM2_REG_27r,
        PGW_LR_TDM2_REG_28r, PGW_LR_TDM2_REG_29r,
        PGW_LR_TDM2_REG_30r, PGW_LR_TDM2_REG_31r,
        PGW_LR_TDM2_REG_32r, PGW_LR_TDM2_REG_33r,
        PGW_LR_TDM2_REG_34r, PGW_LR_TDM2_REG_35r,
        PGW_LR_TDM2_REG_36r, PGW_LR_TDM2_REG_37r,
        PGW_LR_TDM2_REG_38r, PGW_LR_TDM2_REG_39r
    }
};

static const soc_field_t mn_pgw_tdm_fields[] = {
    TDM_ENTRY0_PORT_IDf,  TDM_ENTRY1_PORT_IDf,
    TDM_ENTRY2_PORT_IDf,  TDM_ENTRY3_PORT_IDf,
    TDM_ENTRY4_PORT_IDf,  TDM_ENTRY5_PORT_IDf,
    TDM_ENTRY6_PORT_IDf,  TDM_ENTRY7_PORT_IDf,
    TDM_ENTRY8_PORT_IDf,  TDM_ENTRY9_PORT_IDf,
    TDM_ENTRY10_PORT_IDf, TDM_ENTRY11_PORT_IDf,
    TDM_ENTRY12_PORT_IDf, TDM_ENTRY13_PORT_IDf,
    TDM_ENTRY14_PORT_IDf, TDM_ENTRY15_PORT_IDf,
    TDM_ENTRY16_PORT_IDf, TDM_ENTRY17_PORT_IDf,
    TDM_ENTRY18_PORT_IDf, TDM_ENTRY19_PORT_IDf,
    TDM_ENTRY20_PORT_IDf, TDM_ENTRY21_PORT_IDf,
    TDM_ENTRY22_PORT_IDf, TDM_ENTRY23_PORT_IDf,
    TDM_ENTRY24_PORT_IDf, TDM_ENTRY25_PORT_IDf,
    TDM_ENTRY26_PORT_IDf, TDM_ENTRY27_PORT_IDf,
    TDM_ENTRY28_PORT_IDf, TDM_ENTRY29_PORT_IDf,
    TDM_ENTRY30_PORT_IDf, TDM_ENTRY31_PORT_IDf,
    TDM_ENTRY32_PORT_IDf, TDM_ENTRY33_PORT_IDf,
    TDM_ENTRY34_PORT_IDf, TDM_ENTRY35_PORT_IDf,
    TDM_ENTRY36_PORT_IDf, TDM_ENTRY37_PORT_IDf,
    TDM_ENTRY38_PORT_IDf, TDM_ENTRY39_PORT_IDf,
    TDM_ENTRY40_PORT_IDf, TDM_ENTRY41_PORT_IDf,
    TDM_ENTRY42_PORT_IDf, TDM_ENTRY43_PORT_IDf,
    TDM_ENTRY44_PORT_IDf, TDM_ENTRY45_PORT_IDf,
    TDM_ENTRY46_PORT_IDf, TDM_ENTRY47_PORT_IDf,
    TDM_ENTRY48_PORT_IDf, TDM_ENTRY49_PORT_IDf,
    TDM_ENTRY50_PORT_IDf, TDM_ENTRY51_PORT_IDf,
    TDM_ENTRY52_PORT_IDf, TDM_ENTRY53_PORT_IDf,
    TDM_ENTRY54_PORT_IDf, TDM_ENTRY55_PORT_IDf,
    TDM_ENTRY56_PORT_IDf, TDM_ENTRY57_PORT_IDf,
    TDM_ENTRY58_PORT_IDf, TDM_ENTRY59_PORT_IDf,
    TDM_ENTRY60_PORT_IDf, TDM_ENTRY61_PORT_IDf,
    TDM_ENTRY62_PORT_IDf, TDM_ENTRY63_PORT_IDf,
    TDM_ENTRY64_PORT_IDf,  TDM_ENTRY65_PORT_IDf,
    TDM_ENTRY66_PORT_IDf,  TDM_ENTRY67_PORT_IDf,
    TDM_ENTRY68_PORT_IDf,  TDM_ENTRY69_PORT_IDf,
    TDM_ENTRY70_PORT_IDf,  TDM_ENTRY71_PORT_IDf,
    TDM_ENTRY72_PORT_IDf,  TDM_ENTRY73_PORT_IDf,
    TDM_ENTRY74_PORT_IDf, TDM_ENTRY75_PORT_IDf,
    TDM_ENTRY76_PORT_IDf, TDM_ENTRY77_PORT_IDf,
    TDM_ENTRY78_PORT_IDf, TDM_ENTRY79_PORT_IDf,
    TDM_ENTRY80_PORT_IDf, TDM_ENTRY81_PORT_IDf,
    TDM_ENTRY82_PORT_IDf, TDM_ENTRY83_PORT_IDf,
    TDM_ENTRY84_PORT_IDf, TDM_ENTRY85_PORT_IDf,
    TDM_ENTRY86_PORT_IDf, TDM_ENTRY87_PORT_IDf,
    TDM_ENTRY88_PORT_IDf, TDM_ENTRY89_PORT_IDf,
    TDM_ENTRY90_PORT_IDf, TDM_ENTRY91_PORT_IDf,
    TDM_ENTRY92_PORT_IDf, TDM_ENTRY93_PORT_IDf,
    TDM_ENTRY94_PORT_IDf, TDM_ENTRY95_PORT_IDf,
    TDM_ENTRY96_PORT_IDf, TDM_ENTRY97_PORT_IDf,
    TDM_ENTRY98_PORT_IDf, TDM_ENTRY99_PORT_IDf,
    TDM_ENTRY100_PORT_IDf, TDM_ENTRY101_PORT_IDf,
    TDM_ENTRY102_PORT_IDf, TDM_ENTRY103_PORT_IDf,
    TDM_ENTRY104_PORT_IDf, TDM_ENTRY105_PORT_IDf,
    TDM_ENTRY106_PORT_IDf, TDM_ENTRY107_PORT_IDf,
    TDM_ENTRY108_PORT_IDf, TDM_ENTRY109_PORT_IDf,
    TDM_ENTRY110_PORT_IDf, TDM_ENTRY111_PORT_IDf,
    TDM_ENTRY112_PORT_IDf, TDM_ENTRY113_PORT_IDf,
    TDM_ENTRY114_PORT_IDf, TDM_ENTRY115_PORT_IDf,
    TDM_ENTRY116_PORT_IDf, TDM_ENTRY117_PORT_IDf,
    TDM_ENTRY118_PORT_IDf, TDM_ENTRY119_PORT_IDf,
    TDM_ENTRY120_PORT_IDf, TDM_ENTRY121_PORT_IDf,
    TDM_ENTRY122_PORT_IDf, TDM_ENTRY123_PORT_IDf,
    TDM_ENTRY124_PORT_IDf, TDM_ENTRY125_PORT_IDf,
    TDM_ENTRY126_PORT_IDf, TDM_ENTRY127_PORT_IDf,
    TDM_ENTRY128_PORT_IDf, TDM_ENTRY129_PORT_IDf,
    TDM_ENTRY130_PORT_IDf, TDM_ENTRY131_PORT_IDf,
    TDM_ENTRY132_PORT_IDf, TDM_ENTRY133_PORT_IDf,
    TDM_ENTRY134_PORT_IDf, TDM_ENTRY135_PORT_IDf,
    TDM_ENTRY136_PORT_IDf, TDM_ENTRY137_PORT_IDf,
    TDM_ENTRY138_PORT_IDf, TDM_ENTRY139_PORT_IDf,
    TDM_ENTRY140_PORT_IDf, TDM_ENTRY141_PORT_IDf,
    TDM_ENTRY142_PORT_IDf, TDM_ENTRY143_PORT_IDf,
    TDM_ENTRY144_PORT_IDf, TDM_ENTRY145_PORT_IDf,
    TDM_ENTRY146_PORT_IDf, TDM_ENTRY147_PORT_IDf,
    TDM_ENTRY148_PORT_IDf, TDM_ENTRY149_PORT_IDf,
    TDM_ENTRY150_PORT_IDf, TDM_ENTRY151_PORT_IDf,
    TDM_ENTRY152_PORT_IDf, TDM_ENTRY153_PORT_IDf,
    TDM_ENTRY154_PORT_IDf, TDM_ENTRY155_PORT_IDf,
    TDM_ENTRY156_PORT_IDf, TDM_ENTRY157_PORT_IDf,
    TDM_ENTRY158_PORT_IDf, TDM_ENTRY159_PORT_IDf,
};

static const soc_reg_t mn_pgw_ovs_tdm_regs[] = {
    PGW_OS_TDM_REG_0r, PGW_OS_TDM_REG_1r,
    PGW_OS_TDM_REG_2r, PGW_OS_TDM_REG_3r,
    PGW_OS_TDM_REG_4r, PGW_OS_TDM_REG_5r,
    PGW_OS_TDM_REG_6r, PGW_OS_TDM_REG_7r,
    PGW_OS_TDM_REG_8r, PGW_OS_TDM_REG_9r,
    PGW_OS_TDM_REG_10r, PGW_OS_TDM_REG_11r
};
static const soc_reg_t mn_pgw_ovs_spacing_regs[] = {
    PGW_OS_PORT_SPACING_REG_0r, PGW_OS_PORT_SPACING_REG_1r,
    PGW_OS_PORT_SPACING_REG_2r, PGW_OS_PORT_SPACING_REG_3r,
    PGW_OS_PORT_SPACING_REG_4r, PGW_OS_PORT_SPACING_REG_5r,
    PGW_OS_PORT_SPACING_REG_6r, PGW_OS_PORT_SPACING_REG_7r,
    PGW_OS_PORT_SPACING_REG_8r, PGW_OS_PORT_SPACING_REG_9r,
    PGW_OS_PORT_SPACING_REG_10r, PGW_OS_PORT_SPACING_REG_11r
};
static const soc_field_t mn_pgw_spacing_fields[] = {
    SLOT0f, SLOT1f, SLOT2f, SLOT3f, SLOT4f, SLOT5f, SLOT6f, SLOT7f,
    SLOT8f, SLOT9f, SLOT10f, SLOT11f, SLOT12f, SLOT13f, SLOT14f, SLOT15f,
    SLOT16f, SLOT17f, SLOT18f, SLOT19f, SLOT20f, SLOT21f, SLOT22f, SLOT23f,
    SLOT24f, SLOT25f, SLOT26f, SLOT27f, SLOT28f, SLOT29f, SLOT30f, SLOT31f,
    SLOT32f, SLOT33f, SLOT34f, SLOT35f, SLOT36f, SLOT37f, SLOT38f, SLOT39f,
    SLOT40f, SLOT41f, SLOT42f, SLOT43f, SLOT44f, SLOT45f, SLOT46f, SLOT47f
};
#define MONTEREY_PGW_TDM_HSP_SLOTS 9
static const soc_reg_t pgw_hsp_config_regs[MONTEREY_PGW_TDM_HSP_SLOTS] = {
    PGW_HSP_CONFIG_0r, PGW_HSP_CONFIG_1r, PGW_HSP_CONFIG_2r,
    PGW_HSP_CONFIG_3r, PGW_HSP_CONFIG_4r, PGW_HSP_CONFIG_5r,
    PGW_HSP_CONFIG_6r, PGW_HSP_CONFIG_7r, PGW_HSP_CONFIG_8r,
};

static const soc_reg_t pgw_hsp_config_fields[MONTEREY_PGW_TDM_HSP_SLOTS] = {
    PHY_PORT_NO_0f, PHY_PORT_NO_1f, PHY_PORT_NO_2f,
    PHY_PORT_NO_3f, PHY_PORT_NO_4f, PHY_PORT_NO_5f,
    PHY_PORT_NO_6f, PHY_PORT_NO_7f, PHY_PORT_NO_8f,
};

static const soc_reg_t mn_tdm_pgw_ctrl = PGW_TDM_CONTROLr;
static const soc_reg_t mn_tdm_mmu_ctrl = ES_PIPE0_TDM_CONFIGr;
static const soc_reg_t mmu_ovs_group_regs[MONTEREY_MMU_OVS_GROUP_COUNT] = {
    ES_PIPE0_OVR_SUB_GRP0_TBLr, ES_PIPE0_OVR_SUB_GRP1_TBLr,
    ES_PIPE0_OVR_SUB_GRP2_TBLr, ES_PIPE0_OVR_SUB_GRP3_TBLr,
    ES_PIPE0_OVR_SUB_GRP4_TBLr, ES_PIPE0_OVR_SUB_GRP5_TBLr,
    ES_PIPE0_OVR_SUB_GRP6_TBLr, ES_PIPE0_OVR_SUB_GRP7_TBLr
};

static const soc_reg_t mmu_ovs_group_wt_regs[8] = {
    ES_PIPE0_OVR_SUB_GRP_WT0r, ES_PIPE0_OVR_SUB_GRP_WT1r,
    ES_PIPE0_OVR_SUB_GRP_WT2r, ES_PIPE0_OVR_SUB_GRP_WT3r,
    ES_PIPE0_OVR_SUB_GRP_WT4r, ES_PIPE0_OVR_SUB_GRP_WT5r,
    ES_PIPE0_OVR_SUB_GRP_WT6r, ES_PIPE0_OVR_SUB_GRP_WT7r
};



#define MN_TDM_WT_GRP_SPG_GET(unit, _wtg)     ((MN_MISC(unit)->tdm_wtg_used[_wtg] & 0xff))
#define MN_TDM_WT_GRP_REFCNT_GET(unit, _wtg)  ((MN_MISC(unit)->tdm_wtg_used[_wtg] >> 8) & 0xff)

#define MONTEREY_TDM_WT_GRP_SPEED_GET(unit, _wtg)   ((MN_TDM_WT_GRP_SPG_GET(unit, _wtg) * 2500))


#define MONTEREY_TDM_WT_GRP_RESERVE(unit, _wtg, _spg)                    \
    do {                                                               \
        uint16 _tmp = (MN_TDM_WT_GRP_REFCNT_GET(unit, _wtg) + 1);      \
        MN_MISC(unit)->tdm_wtg_used[_wtg] = ((_spg) | (_tmp << 8));    \
    } while (0)

#define MONTEREY_TDM_WT_GRP_FREE(unit, _wtg)                             \
    do {                                                               \
        uint16 _tmp = (MN_TDM_WT_GRP_REFCNT_GET(unit, _wtg) - 1);      \
        if (MN_TDM_WT_GRP_REFCNT_GET(unit, _wtg) <= 1) {               \
            soc_monterey_misc_info[unit]->tdm_wtg_used[_wtg] = 0;        \
        } else {                                                       \
            soc_monterey_misc_info[unit]->tdm_wtg_used[_wtg] =           \
                (MN_TDM_WT_GRP_SPG_GET(unit, _wtg) | (_tmp << 8));     \
        }                                                              \
    } while (0)

#define MONTEREY_TDM_WT_GRP_LOG(unit)                                             \
    do {                                                                        \
        int _wtg;                                                               \
        LOG_VERBOSE(BSL_LS_SOC_TDM,                                             \
                    (BSL_META_U(unit,                                           \
                                "Weight groups and reference counts: \n\t")));  \
        for (_wtg = 0; _wtg < 4; _wtg++)  {                                     \
            LOG_VERBOSE(BSL_LS_SOC_TDM,                                         \
                        (BSL_META_U(unit,                                       \
                            "Grp = %d Refcnt = %d Speed Grp = %d Speed = %d;"), \
                            _wtg, MN_TDM_WT_GRP_REFCNT_GET(unit, _wtg),         \
                            MN_TDM_WT_GRP_SPG_GET(unit, _wtg),                  \
                            MONTEREY_TDM_WT_GRP_SPEED_GET(unit, _wtg)));          \
        }                                                                       \
        LOG_VERBOSE(BSL_LS_SOC_TDM, (BSL_META_U(unit, "\n")));                  \
    } while (0)

#define MONTEREY_TDM_WT_GRP_USABLE(unit, _wtg, _spg)   \
    ((MN_TDM_WT_GRP_REFCNT_GET(unit, _wtg) == 0) || (MN_TDM_WT_GRP_SPG_GET(unit, _wtg) == _spg))

#define MONTEREY_TDM_ACTIVE_CAL(unit) soc_monterey_misc_info[unit]->tdm_active_cal
#define MONTEREY_TDM_STRUCT_ACTIVE(unit) (!MONTEREY_TDM_ACTIVE_CAL(unit)) ? \
            &(soc_monterey_misc_info[unit]->tdm0) : &(soc_monterey_misc_info[unit]->tdm1)
#define MONTEREY_TDM_STRUCT_BACKUP(unit) (MONTEREY_TDM_ACTIVE_CAL(unit)) ? \
            &(soc_monterey_misc_info[unit]->tdm0) : &(soc_monterey_misc_info[unit]->tdm1)

#define MONTEREY_TDM_PGW_LR_REG(cur_cal, slot) mn_pgw_lr_tdm_regs[cur_cal][slot/4]

#define MONTEREY_TDM_PGW_OS_REG(slot) mn_pgw_ovs_tdm_regs[slot/4]

#define MONTEREY_TDM_PGW_OS_SPACING_REG(slot) mn_pgw_ovs_spacing_regs[slot/4]

#define MONTEREY_TDM_PGW_SLOT(slot)  mn_pgw_tdm_fields[slot]
#define MONTEREY_TDM_PGW_OS_SPACING_FIELD(slot)  mn_pgw_spacing_fields[slot]

#define MONTEREY_TDM_MMU_TBL(cur_cal) \
            (!cur_cal) ? ES_PIPE0_TDM_TABLE_0m : ES_PIPE0_TDM_TABLE_1m

#if !defined(SOC_NO_NAMES)
#define MONTEREY_TDM_PGW_LOG(_u, _desc, _r, _f, _v)            \
            LOG_INFO(BSL_LS_SOC_COMMON,                      \
                        (BSL_META_U(_u,                      \
                                    _desc ": %s.%s = %d\n"), \
                                    SOC_REG_NAME(_u, _r),    \
                                    soc_fieldnames[_f], _v))
#else
#define MONTEREY_TDM_PGW_LOG(_u, _desc, _r, _f, _v) \
            LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META_U(_u, _desc ": = %d\n"), _v))
#endif

STATIC int
soc_monterey_tdm_calc(int unit, soc_monterey_tdm_t *monterey_tdm)
{
    int rv, pgw, group, index;
    soc_info_t *si = &SOC_INFO(unit);
    tdm_soc_t monterey_chip_pkg;
    tdm_mod_t *monterey_tdm_pkg;
    int tdm_freq = 0, io_bw ;
    uint16      dev_id;
    uint8       rev_id;
    int         is_macsec = 0;
    sal_memset(&monterey_chip_pkg, 0, sizeof(tdm_soc_t));

    monterey_chip_pkg.unit = unit;
    monterey_chip_pkg.num_ext_ports = MN_NUM_EXT_PORTS;
    monterey_chip_pkg.state = sal_alloc((monterey_chip_pkg.num_ext_ports)*sizeof(int *), "port state list");
    monterey_chip_pkg.speed = sal_alloc((monterey_chip_pkg.num_ext_ports)*sizeof(int *), "port speed list");

    for (index = 1; index < monterey_chip_pkg.num_ext_ports; index++) {
        monterey_chip_pkg.state[index] = monterey_tdm->port_state[index];
    }
    monterey_chip_pkg.state[0] = 1;  /* enable cpu port */
    monterey_chip_pkg.state[65] = 1; /* enable loopback port */
    monterey_chip_pkg.state[66] = 1; /* enable MACSEC port */

    for (index = 0; index < monterey_chip_pkg.num_ext_ports; index++) {
        monterey_chip_pkg.speed[index] = monterey_tdm->speed[index];
    }
    if (soc_property_get(unit, "tdm_check_disable", 0)) {
        monterey_chip_pkg.soc_vars.mn.tdm_chk_en = 1;
    } else {
        monterey_chip_pkg.soc_vars.mn.tdm_chk_en = 0;
    }

    /*
     * Map detected core clk frequency to TDM algorithm internal code
     * value.
     */
    tdm_freq = soc_property_get(unit, spn_BCM_TDM_FREQUENCY,0);
    if ( tdm_freq) {
         monterey_chip_pkg.clk_freq =tdm_freq;
    } else {
         monterey_chip_pkg.clk_freq = si->frequency;
    }
    if ( !tdm_freq) {
         soc_cm_get_id(unit, &dev_id, &rev_id);
         if (dev_id == BCM56675_DEVICE_ID) {
             monterey_chip_pkg.soc_vars.mn.sku_core_bw = si->bandwidth/1000;
         } else {
             monterey_chip_pkg.soc_vars.mn.sku_core_bw = si->io_bandwidth/1000;
         }
    } else {
         monterey_chip_pkg.soc_vars.mn.sku_core_bw = 0;
   }
   if (soc_feature(unit, soc_feature_xflow_macsec)) {
       monterey_chip_pkg.soc_vars.mn.is_macsec = 1;
       if (dev_id == BCM56675_DEVICE_ID) {
           monterey_chip_pkg.soc_vars.mn.sku_core_bw -= 40;
       }
   }

    monterey_tdm_pkg = _soc_set_tdm_tbl(SOC_SEL_TDM(&monterey_chip_pkg));

    if (!monterey_tdm_pkg) {
        LOG_CLI((BSL_META_U(unit,
            "Unable to configure TDM, please contact your "
            "Field Applications Engineer or Sales Manager for "
            "additional assistance.\n")));
        return SOC_E_FAIL;
    }
    monterey_tdm->pgw0len =
               monterey_tdm_pkg->_chip_data.soc_pkg.soc_vars.mn.pgw0_len;
    monterey_tdm->pgw1len =
               monterey_tdm_pkg->_chip_data.soc_pkg.soc_vars.mn.pgw1_len;
    /*  PGW0 LR/OS Calendars */
    sal_memcpy(monterey_tdm->pgw_tdm[0],
               monterey_tdm_pkg->_chip_data.cal_0.cal_main,
               sizeof(int)*MONTEREY_PGW_TDM_LENGTH);
    sal_memcpy(monterey_tdm->ingress_ovs_tdm.pgw_ovs_tdm[0],
               monterey_tdm_pkg->_chip_data.cal_0.cal_grp[0],
               sizeof(int)*MONTEREY_PGW_TDM_OVS_SIZE);
    sal_memcpy(monterey_tdm->ingress_ovs_tdm.pgw_ovs_spacing[0],
               monterey_tdm_pkg->_chip_data.cal_0.cal_grp[1],
               sizeof(int)*MONTEREY_PGW_TDM_OVS_SIZE);

    /*  PGW1 LR/OS Calendars */
    sal_memcpy(monterey_tdm->pgw_tdm[1],
               monterey_tdm_pkg->_chip_data.cal_1.cal_main,
               sizeof(int)*MONTEREY_PGW_TDM_LENGTH);
    sal_memcpy(monterey_tdm->ingress_ovs_tdm.pgw_ovs_tdm[1],
               monterey_tdm_pkg->_chip_data.cal_1.cal_grp[0],
               sizeof(int)*MONTEREY_PGW_TDM_OVS_SIZE);
    sal_memcpy(monterey_tdm->ingress_ovs_tdm.pgw_ovs_spacing[1],
               monterey_tdm_pkg->_chip_data.cal_1.cal_grp[1],
               sizeof(int)*MONTEREY_PGW_TDM_OVS_SIZE);

    sal_memcpy(monterey_tdm->mmu_tdm,
               monterey_tdm_pkg->_chip_data.cal_2.cal_main,
               sizeof(int)*MONTEREY_MMU_TDM_LENGTH);

    sal_memcpy(monterey_tdm->egress_ovs_tdm.mmu_ovs_group_tdm[0],
               monterey_tdm_pkg->_chip_data.cal_2.cal_grp[0],
               sizeof(int)*MONTEREY_MMU_OVS_GROUP_TDM_LENGTH);
    sal_memcpy(monterey_tdm->egress_ovs_tdm.mmu_ovs_group_tdm[1],
               monterey_tdm_pkg->_chip_data.cal_2.cal_grp[1],
               sizeof(int)*MONTEREY_MMU_OVS_GROUP_TDM_LENGTH);
    sal_memcpy(monterey_tdm->egress_ovs_tdm.mmu_ovs_group_tdm[2],
               monterey_tdm_pkg->_chip_data.cal_2.cal_grp[2],
               sizeof(int)*MONTEREY_MMU_OVS_GROUP_TDM_LENGTH);
    sal_memcpy(monterey_tdm->egress_ovs_tdm.mmu_ovs_group_tdm[3],
               monterey_tdm_pkg->_chip_data.cal_2.cal_grp[3],
               sizeof(int)*MONTEREY_MMU_OVS_GROUP_TDM_LENGTH);
    sal_memcpy(monterey_tdm->egress_ovs_tdm.mmu_ovs_group_tdm[4],
               monterey_tdm_pkg->_chip_data.cal_2.cal_grp[4],
               sizeof(int)*MONTEREY_MMU_OVS_GROUP_TDM_LENGTH);
    sal_memcpy(monterey_tdm->egress_ovs_tdm.mmu_ovs_group_tdm[5],
               monterey_tdm_pkg->_chip_data.cal_2.cal_grp[5],
               sizeof(int)*MONTEREY_MMU_OVS_GROUP_TDM_LENGTH);
    sal_memcpy(monterey_tdm->egress_ovs_tdm.mmu_ovs_group_tdm[6],
               monterey_tdm_pkg->_chip_data.cal_2.cal_grp[6],
               sizeof(int)*MONTEREY_MMU_OVS_GROUP_TDM_LENGTH);
    sal_memcpy(monterey_tdm->egress_ovs_tdm.mmu_ovs_group_tdm[7],
              monterey_tdm_pkg->_chip_data.cal_2.cal_grp[7],
               sizeof(int)*MONTEREY_MMU_OVS_GROUP_TDM_LENGTH);

    sal_free(monterey_chip_pkg.state);
    sal_free(monterey_chip_pkg.speed);

    monterey_tdm_pkg->_chip_exec[TDM_CHIP_EXEC__FREE](monterey_tdm_pkg);
    sal_free(monterey_tdm_pkg);
    if (LOG_CHECK(BSL_LS_SOC_TDM | BSL_INFO)) {
        for (pgw = 0; pgw < MONTEREY_PGWS_PER_DEV; pgw++) {
            LOG_CLI((BSL_META_U(unit, "PGW_CL%d pgw_tdm:"), pgw));
            for (index = 0; index < MONTEREY_PGW_TDM_LENGTH; index++) {
                if (index % 16 == 0) {
                    LOG_CLI((BSL_META_U(unit, "\n    ")));
                }
                LOG_CLI((BSL_META_U(unit, " %3d"),
                           monterey_tdm->pgw_tdm[pgw][index]));
            }
            LOG_CLI((BSL_META_U(unit, "\n")));
            LOG_CLI((BSL_META_U(unit, "PGW_CL%d pgw_ovs_tdm:"), pgw));
            for (index = 0; index < MONTEREY_PGW_TDM_LENGTH; index++) {
                if (index % 16 == 0) {
                    LOG_CLI((BSL_META_U(unit, "\n    ")));
                }
                LOG_CLI((BSL_META_U(unit, " %3d"),
                           monterey_tdm->ingress_ovs_tdm.pgw_ovs_tdm[pgw][index]));
            }
            LOG_CLI((BSL_META_U(unit, "\n")));
            LOG_CLI((BSL_META_U(unit, "PGW_CL%d pgw_ovs_spacing:"), pgw));
            for (index = 0; index < MONTEREY_PGW_TDM_LENGTH; index++) {
                if (index % 16 == 0) {
                    LOG_CLI((BSL_META_U(unit, "\n    ")));
                }
                LOG_CLI((BSL_META_U(unit, " %3d"),
                             monterey_tdm->ingress_ovs_tdm.pgw_ovs_spacing[pgw][index]));
            }
            LOG_CLI((BSL_META_U(unit, "\n")));
        }
        LOG_CLI((BSL_META_U(unit, "mmu_tdm:")));
        for (index = 0; index < MONTEREY_MMU_TDM_LENGTH; index++) {
            if (index % 16 == 0) {
                LOG_CLI((BSL_META_U(unit, "\n    ")));
            }
            LOG_CLI((BSL_META_U(unit, " %3d"),
                       monterey_tdm->mmu_tdm[index]));
        }
        LOG_CLI((BSL_META_U(unit, "\n")));
        for (group = 0; group < MONTEREY_MMU_OVS_GROUP_COUNT; group++) {
            LOG_CLI((BSL_META_U(unit, "group %d ovs_group_tdm"), group));
            for (index = 0; index < MONTEREY_MMU_OVS_GROUP_TDM_LENGTH;
                 index++) {
                if (index % 16 == 0) {
                    LOG_CLI((BSL_META_U(unit, "\n    ")));
                }
                LOG_CLI((BSL_META_U(unit, " %3d"),
                           monterey_tdm->egress_ovs_tdm.mmu_ovs_group_tdm[group][index]));
            }
            LOG_CLI((BSL_META_U(unit, "\n")));
        }
    }

    if (LOG_CHECK(BSL_LS_SOC_TDM | BSL_INFO)) {
        LOG_CLI((BSL_META_U(unit, "tdm_bw: %dG\n"), monterey_tdm->tdm_bw));
        LOG_CLI((BSL_META_U(unit, "ovs state: %d\n"),
                   monterey_tdm->is_oversub));
    }
    if (!tdm_freq) {
        tdm_freq = si->frequency;
        if (dev_id == BCM56675_DEVICE_ID) {
            io_bw = si->bandwidth/1000;
            if (soc_feature(unit, soc_feature_xflow_macsec)) {
                io_bw -= 40 ;
                is_macsec = 1;
            }
        } else {
             io_bw = si->io_bandwidth/1000;
        }
    } else {
        io_bw = 0;

    }

    rv = tdm_mn_set_iarb_tdm(tdm_freq,
                            monterey_tdm->is_oversub,
                            &monterey_tdm->iarb_tdm_wrap_ptr,
                            monterey_tdm->iarb_tdm,io_bw,is_macsec);

    if (rv == 0) {
        LOG_CLI((BSL_META_U(unit,
                   "Unable to configure IARB TDM, please contact your "
                   "Field Applications Engineer or Sales Manager for "
                   "additional assistance.\n")));
        return SOC_E_FAIL;
    }

    if ((LOG_CHECK(BSL_LS_SOC_TDM | BSL_INFO))) {
        LOG_CLI((BSL_META_U(unit,
                   "iarb_tdm: (wrap_ptr %d)"),
                   monterey_tdm->iarb_tdm_wrap_ptr));
        for (index = 0; index < MONTEREY_IARB_TDM_LENGTH; index++) {
            if (index > monterey_tdm->iarb_tdm_wrap_ptr) {
                break;
            }
            if (index % 16 == 0) {
                LOG_CLI((BSL_META_U(unit, "\n    ")));
            }
            LOG_CLI((BSL_META_U(unit, " %3d"),
                       monterey_tdm->iarb_tdm[index]));
        }
        LOG_CLI((BSL_META_U(unit, "\n")));
    }

    return SOC_E_NONE;
}

#define MONTEREY_PGW0_TDM_WRAP_PTR  0x8b
#define MONTEREY_PGW1_TDM_WRAP_PTR  0x97

STATIC int
_soc_monterey_tdm_min_spacing_init(int unit)
{
    int phy_port,mmu_port  ;
    int port,profile_x;
    soc_info_t *si;
    uint32 rval;
    si = &SOC_INFO(unit);

    PBMP_PORT_ITER(unit, port) {
        phy_port = si->port_l2p_mapping[port];
         if (phy_port == -1 ) {
                 continue;
         }
         mmu_port = si->port_p2m_mapping[phy_port];
         if (mmu_port == -1) {
                 continue;
         }
         if (mmu_port < 16 ) {
             if (si->port_speed_max[port] < 50000) {
                 SOC_IF_ERROR_RETURN(soc_reg32_get(unit, ES_PIPE0_MIN_SPACINGr,
                                        REG_PORT_ANY, 0, &rval));
                 profile_x = soc_reg_field_get(unit, ES_PIPE0_MIN_SPACINGr, rval, PROFILEf );
                 profile_x = (profile_x | (1 << mmu_port)) ;
                 soc_reg_field_set(unit, ES_PIPE0_MIN_SPACINGr, &rval, PROFILEf, profile_x);
                 SOC_IF_ERROR_RETURN(soc_reg32_set(unit, ES_PIPE0_MIN_SPACINGr,
                                                      REG_PORT_ANY, 0, rval));


             }

         }
    }

    return SOC_E_NONE;
}

STATIC int
soc_monterey_tdm_update(int unit, int init_time, int cur_cal,
                      soc_monterey_tdm_t *monterey_tdm)
{
    int ix, rv;
    int count, slot;
    int pgw, pgw_inst, length;
    int pgw_hsp_config_index[2] = {0, 0};    /* for each pgw */
    int mmu_idle_port_ix = 0;
    int mmu_idle_ports[] = {76, 78, 79, 80}; /* IDLE MMU ports in Apache */

    soc_port_t port, phy_port, mmu_port;
    soc_port_t pgw_hsp_ports[2][MONTEREY_TSCS_PER_PGW] = {{0}, {0}};

    soc_mem_t mem;
    soc_reg_t reg, hsp_reg;
    soc_field_t field, cal_end_f, cal_end_single_f, lr_tdm_wrap_f, hsp_fields[2];
    es_pipe0_tdm_table_0_entry_t entry;

    uint32 rval, hsp_values[2];
    uint64 rval64, ctrl_rval64;

    soc_info_t *si;

    int  group, weight, wt_group;
    int speed_class, speed_group ,speed = 0 ;
    uint32 wt_group_select = 0;
    uint32 same_spacing = 0;
    uint32 speed_spacing = 0;
    int tdm_freq = 0;

    si = &SOC_INFO(unit);
    /* Configure PGW TDM for MONTEREY */
    for (pgw = 0; pgw < MONTEREY_PGWS_PER_DEV; pgw++) {

        /* coverity[negative_returns : FALSE] */
        if (!SOC_INFO(unit).block_valid[PGW_CL_BLOCK(unit, pgw)]) {
            continue;
        }

        pgw_inst = pgw | SOC_REG_ADDR_INSTANCE_MASK;

        rv = soc_reg_get(unit, mn_tdm_pgw_ctrl, pgw_inst, 0, &ctrl_rval64);
        SOC_IF_ERROR_RETURN(rv);

        /* Configure PGW line rate ports TDM */
        count = 0;
        COMPILER_64_ZERO(rval64);

        for (slot= 0; slot < MONTEREY_PGW_TDM_LENGTH; slot++) {

            reg = MONTEREY_TDM_PGW_LR_REG(cur_cal, slot);
            field = MONTEREY_TDM_PGW_SLOT(slot);

            phy_port = monterey_tdm->pgw_tdm[pgw][slot];

            if (phy_port == NUM_EXT_PORTS) {
                phy_port = 0xff;
            } else {
                if (phy_port == OVS_TOKEN) {
                    phy_port = 0x44;
                }
            }


            soc_reg64_field32_set(unit, reg, &rval64, field, phy_port);

            MONTEREY_TDM_PGW_LOG(unit, "PGW LR TDM config", reg, field, phy_port);

            if (((slot + 1) % MONTEREY_PGW_TDM_SLOTS_PER_REG) == 0) {

                SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, pgw_inst, 0, rval64));
                COMPILER_64_ZERO(rval64);
            }
        }
        tdm_freq = soc_property_get(unit, spn_BCM_TDM_FREQUENCY,0);
        switch (tdm_freq) {

               case 815:
                         if ( pgw == 0 ) {
                             count =0x8b;
                         } else {
                             count = 0x97;
                         }
                         break;
               case 816:
                         if ( pgw == 0 ) {
                             count =0x83;
                         } else {
                             count = 0x87;
                         }
                         break;
               case 817:
                         if ( pgw == 0 ) {
                             count =0x8b;
                         } else {
                             count = 0x97;
                         }
                         break;

               case 818 :
                         count = 0x9f;
                         break;
               case 819 :
                         count =0x7f;
                         break;
               case 820 :
                             count =0x97;
                         break;
               case 666:
                         if ( pgw == 0 ) {
                             count =0x4F;
                         } else {
                             count = 0x2B;
                         }
                         break;
               case 667:
                         if ( pgw == 0 ) {
                             count =0x37;
                         } else {
                             count = 0x4F;
                         }
                         break;
               case 550 :
                         if ( pgw == 0 ) {
                             count =0x54;
                         } else {
                             count = 0x50;
                         }
                         break;
               case 861 :
                         count =0x2f;
                         break;
               case 862 :
                         count =0x2f;
                         break;
               default:
                         if ( pgw == 0 ) {
                             count =monterey_tdm->pgw0len - 1 ;
                         } else {
                             count =monterey_tdm->pgw1len - 1;
                         }
                         break;
        }
        lr_tdm_wrap_f = (!cur_cal) ? LR_TDM_WRAP_PTRf : LR_TDM2_WRAP_PTRf;

        if (count > 0) {
            soc_reg64_field32_set(unit, mn_tdm_pgw_ctrl, &ctrl_rval64,
                                  lr_tdm_wrap_f, count );
        }
        soc_reg64_field32_set(unit, mn_tdm_pgw_ctrl, &ctrl_rval64,
                              LR_TDM_SELf, cur_cal);
        /* Always set both LR_TDM_ENABLE and LR_TDM2_ENABLE.
         * Even when switching, we need to make sure the the current
         * calendar is active so that it services the slots until the
         * end of the calendar when it actually switches to the new one.
 */
        soc_reg64_field32_set(unit, mn_tdm_pgw_ctrl, &ctrl_rval64,
                              LR_TDM_ENABLEf, 1);
        soc_reg64_field32_set(unit, mn_tdm_pgw_ctrl, &ctrl_rval64,
                              LR_TDM2_ENABLEf, 1);

        /* Configure PGW oversubscription ports TDM */
        count = 0;
        COMPILER_64_ZERO(rval64);
        for (slot= 0; slot < MONTEREY_PGW_TDM_OVS_SIZE; slot++) {

            reg = MONTEREY_TDM_PGW_OS_REG(slot);
            field = MONTEREY_TDM_PGW_SLOT(slot);

            phy_port = monterey_tdm->ingress_ovs_tdm.pgw_ovs_tdm[pgw][slot];;
            MONTEREY_TDM_PGW_LOG(unit, "PGW OS TDM config", reg, field, phy_port);

            if (phy_port == NUM_EXT_PORTS) {
                phy_port = 0xff;
            } else {
                count++;
            }

            soc_reg64_field32_set(unit, reg, &rval64, field, phy_port);

            if (((slot + 1) % MONTEREY_PGW_TDM_SLOTS_PER_REG) == 0) {

                SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, pgw_inst, 0, rval64));
                COMPILER_64_ZERO(rval64);
            }
            if (phy_port == 0xff) {
                continue;
            }
            port = SOC_INFO(unit).port_p2l_mapping[phy_port];
            if(((phy_port > 12) && (phy_port <= 20)) || ((phy_port > 56) && (phy_port <= 64)))
            {
                if (SOC_INFO(unit).port_speed_max[port] >= 25000) {
                    for (ix = 0; ix < pgw_hsp_config_index[pgw]; ix++) {
                        if (pgw_hsp_ports[pgw][ix] == phy_port) {
                            break;
                        }
                    }
                    if (ix < pgw_hsp_config_index[pgw]) {
                        continue;
                    }
                    pgw_hsp_ports[pgw][pgw_hsp_config_index[pgw]] = phy_port;
                    hsp_reg = pgw_hsp_config_regs[pgw_hsp_config_index[pgw]];
                    hsp_fields[0] = pgw_hsp_config_fields[pgw_hsp_config_index[pgw]];
                    hsp_values[0] = phy_port;
                    hsp_fields[1] = ENTRIES_IN_CALf;
                    speed = SOC_PORTCTRL_HG2_TO_IEEE_BW_GET(SOC_INFO(unit).port_speed_max[port]);
                    hsp_values[1] = speed < 10000 ? 1 : ((speed + 5000) / 12500);

                    rv = soc_reg_fields32_modify(unit, hsp_reg, pgw_inst,
                                             2, hsp_fields, hsp_values);
                    SOC_IF_ERROR_RETURN(rv);

                    pgw_hsp_config_index[pgw] += 1;
               }
           }
        }

        /* Enable os_skip_cnt_arb_sel by default in Apache */
        soc_reg64_field32_set(unit, mn_tdm_pgw_ctrl, &ctrl_rval64,
                              OS_SKIP_CNT_ARB_SELf, TRUE);

        while (pgw_hsp_config_index[pgw] < MONTEREY_TSCS_PER_PGW) {
            hsp_reg = pgw_hsp_config_regs[pgw_hsp_config_index[pgw]];
            hsp_fields[0] = pgw_hsp_config_fields[pgw_hsp_config_index[pgw]];
            hsp_values[0] = 0x0;
            hsp_fields[1] = ENTRIES_IN_CALf;
            hsp_values[1] = 0x0;

            rv = soc_reg_fields32_modify(unit, hsp_reg, pgw_inst,
                                         2, hsp_fields, hsp_values);
            SOC_IF_ERROR_RETURN(rv);
            pgw_hsp_config_index[pgw] += 1;

        }

        /* OS_TDM_WRAP_PTR and OS_TDM_ENABLE programming */
        soc_reg64_field32_set(unit, mn_tdm_pgw_ctrl, &ctrl_rval64,
                              OS_TDM_WRAP_PTRf, (count - 1));
        soc_reg64_field32_set(unit, mn_tdm_pgw_ctrl, &ctrl_rval64,
                              OS_TDM_ENABLEf, count ? 1 : 0);

        /* Configure PGW oversubscription port spacing */
        COMPILER_64_ZERO(rval64);
        for (slot= 0; slot < MONTEREY_PGW_TDM_OVS_SIZE; slot++) {

            reg = MONTEREY_TDM_PGW_OS_SPACING_REG(slot);
            field = MONTEREY_TDM_PGW_OS_SPACING_FIELD(slot);

            phy_port = monterey_tdm->ingress_ovs_tdm.pgw_ovs_spacing[pgw][slot];
            MONTEREY_TDM_PGW_LOG(unit, "PGW OS Spacing config", reg, field, phy_port);

            soc_reg64_field32_set(unit, reg, &rval64, field, phy_port);

            if (((slot + 1) % MONTEREY_PGW_TDM_SLOTS_PER_REG) == 0) {
                SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, pgw_inst, 0, rval64));
                COMPILER_64_ZERO(rval64);
            }
        }

        rv = soc_reg_set(unit, mn_tdm_pgw_ctrl, pgw_inst, 0, ctrl_rval64);
        SOC_IF_ERROR_RETURN(rv);
    }


    /* Configure MMU TDM */
    mem = MONTEREY_TDM_MMU_TBL(cur_cal);

    length = MONTEREY_MMU_TDM_LENGTH;
    while ((monterey_tdm->mmu_tdm[length] == NUM_EXT_PORTS)) {
        length--;
    }

    rv = soc_reg32_get(unit, mn_tdm_mmu_ctrl, REG_PORT_ANY, 0, &rval);
    SOC_IF_ERROR_RETURN(rv);

    sal_memset(&entry, 0, soc_mem_entry_words(unit, mem) * sizeof(uint32));

    for (slot = 0; slot <= length; slot++) {
        if (!(slot % 2)) {
            sal_memset(&entry, 0, soc_mem_entry_words
                                        (unit, mem) * sizeof(uint32));
        }
        phy_port = monterey_tdm->mmu_tdm[slot];

        /*
         * TDM algo inserts CMIC, LB and ANC tokens for Auxillary
         * bandwidth in each TDM. ANC token needs to be replaced with
         * internal port 77 and CMIC & LB will be mapped to their mmu ports.
         *
         * Each TDM will also have a dedicated slot for guaranteed SBUS ops.
         * While the ANC slots can be used for SBUS as well, there could be
         * cases where the PRG (being of higher priority over SBUS) hogs SBUS
         * operations from getting handled. MMU port 81 will be programmed for
         * these tokens appearing in the algorithm.
         *
         * For unclaimed bandwidth (compared to max LR BW) from each config,
         * TDM algo inserts IDLE tokens. The idle tokens needs to be replaced
         * with internal mmu ports for Purge or SBUS operations. However, since
         * Hardware flags 1in4 violation errors even with these internal mmu
         * ports, we would iterate over all available internal ports (leaving out
         * ANC port - 77, of course).
 */

        if ((phy_port == SBUS_TOKEN)) {
            mmu_port = 81; /* Dedicated token for guaranteed SBUS ops */
        } else if (phy_port == AUX_TOKEN) {
            mmu_port = 77; /* Dedicated ANC slot */
        } else if(phy_port == IDL1_TOKEN) {
            /* iter over all available internal ports to avoid 1:4 violation */
            mmu_port = mmu_idle_ports[mmu_idle_port_ix];
            mmu_idle_port_ix = (mmu_idle_port_ix + 1) % COUNTOF(mmu_idle_ports);
        } else if (phy_port == OVS_TOKEN) {
            mmu_port = 0x44; /* Oversub port */
        } else if (phy_port >= NUM_EXT_PORTS) {
            mmu_port = 0x7f; /* Invalid Port */
        } else {
            mmu_port = SOC_INFO(unit).port_p2m_mapping[phy_port];
        }
        field = (slot % 2) ? PORT_NUM_ODDf : PORT_NUM_EVENf;
        soc_mem_field32_set(unit, mem, &entry, field, mmu_port);
        /* two slots in each entry; length can be odd too */
        if ((slot % 2) || (slot == length)) {
            SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                                              (slot / 2), &entry));
        }
    }

    cal_end_f = (cur_cal) ? CAL1_ENDf : CAL0_ENDf;
    cal_end_single_f = (cur_cal) ? CAL1_END_SINGLEf : CAL0_END_SINGLEf;

    /*
     * Examples:
     *   1) cal length = 401; cal_end = 401/2 - 0 = 200, cal_end_single = 1
     *   2) cal length = 402; cal_end = 402/2 - 1 = 200, cal_end_single = 0
 */
    soc_reg_field_set(unit, mn_tdm_mmu_ctrl, &rval, cal_end_f,
                      (slot % 2) ? (slot / 2): ((slot / 2) - 1));
    if (slot % 2) {
        soc_reg_field_set(unit, mn_tdm_mmu_ctrl, &rval, cal_end_single_f, 1);
    }

    soc_reg_field_set(unit, mn_tdm_mmu_ctrl, &rval, OPP_CPULB_ENf, 1);
    soc_reg_field_set(unit, mn_tdm_mmu_ctrl, &rval, ENABLEf, 1);
    soc_reg_field_set(unit, mn_tdm_mmu_ctrl, &rval, CURR_CALf, cur_cal);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, mn_tdm_mmu_ctrl, REG_PORT_ANY, 0, rval));


    rval = 0;
    for (group = 0; group < MONTEREY_MMU_OVS_GROUP_COUNT; group++) {
        speed_class = monterey_tdm->egress_ovs_tdm.mmu_ovs_group_speed[group];
        reg = mmu_ovs_group_regs[group];
        for (slot = 0; slot < MONTEREY_MMU_OVS_GROUP_TDM_LENGTH; slot++) {

            phy_port = monterey_tdm->egress_ovs_tdm.mmu_ovs_group_tdm[group][slot];
            if (phy_port >= NUM_EXT_PORTS) {
                mmu_port = 0x7f;
            } else {
                mmu_port = si->port_p2m_mapping[phy_port];
                if (speed_class == 0) {
                    speed_class = si->port_speed_max[si->port_p2l_mapping[phy_port]];
                }
            }
            soc_reg_field_set(unit, reg, &rval, MMU_PORTf, (mmu_port & 0x7f));
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, slot, rval));
        }

         if (SOC_FAILURE(_soc_monterey_mmu_ovs_speed_class_map_get(
                        unit, &speed_class, &speed_group, &speed_spacing))) {
            continue;
        }
        if (init_time) {

            for (wt_group = 0; wt_group < MONTEREY_MMU_OVS_WT_GROUP_COUNT; wt_group++) {
                if (MONTEREY_TDM_WT_GRP_USABLE(unit, wt_group, speed_group)) {
                    MONTEREY_TDM_WT_GRP_RESERVE(unit, wt_group, speed_group);
                    break;
                }
            }

            MONTEREY_TDM_WT_GRP_LOG(unit);

            monterey_tdm->egress_ovs_tdm.mmu_ovs_group_wt_group[group] = wt_group;
            monterey_tdm->egress_ovs_tdm.mmu_ovs_group_speed[group] = speed_class;

        } else {
            wt_group = monterey_tdm->egress_ovs_tdm.mmu_ovs_group_wt_group[group];
            speed_class = monterey_tdm->egress_ovs_tdm.mmu_ovs_group_speed[group];
        }

        if (wt_group >= MONTEREY_MMU_OVS_WT_GROUP_COUNT) {
            return SOC_E_INTERNAL;
        }

        /* Apache supports 4 oversub speeds only */
        soc_reg_field_set(unit, ES_PIPE0_GRP_WT_SELECTr, &wt_group_select,
                         mn_mmu_ovs_group_wt_select_fields[group], wt_group);
        reg = ES_PIPE0_OVR_SUB_GRP_CFGr;
        soc_reg_field_set(unit, reg, &same_spacing, SAME_SPACINGf, speed_spacing);
        rv = soc_reg32_set(unit, reg, REG_PORT_ANY, group, same_spacing);
        SOC_IF_ERROR_RETURN(rv);

    }

    for (wt_group = 0; wt_group < MONTEREY_MMU_OVS_WT_GROUP_COUNT; wt_group++) {

        speed_class = MONTEREY_TDM_WT_GRP_SPEED_GET(unit, wt_group);
        if (speed_class == 0) {
            weight = 0x0;
        } else {
            /* use 2500M as weight unit */
            weight = (speed_class > 2500 ? speed_class : 2500) / 2500;
        }
        reg = mmu_ovs_group_wt_regs[wt_group];
        rv = soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval);
        SOC_IF_ERROR_RETURN(rv);

        /* The proportional weights are already programmed  */
        if (soc_reg_field_get(unit, reg, rval, WEIGHTf) == weight) {
            continue;
        }
        soc_reg_field_set(unit, reg, &rval, WEIGHTf, (weight ));
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
    }


    reg = ES_PIPE0_GRP_WT_SELECTr;
    rv = soc_reg32_set(unit, reg, REG_PORT_ANY, 0, wt_group_select);
    SOC_IF_ERROR_RETURN(rv);

    SOC_IF_ERROR_RETURN(_soc_monterey_tdm_min_spacing_init(unit));

    return SOC_E_NONE;
}

STATIC int soc_mn_port_resource_tdm_calculate(int unit, soc_monterey_tdm_t *tdm);


STATIC int
soc_monterey_tdm_reinit(int unit)
{
    soc_reg_t reg;
    int phy_port, mmu_port;
    int slot, group, wt_group, speed_group, speed_class;
    uint32 rval, wt_group_select, speed_spacing;

    soc_monterey_tdm_t *monterey_tdm;
    soc_info_t *si = &SOC_INFO(unit);

    reg = ES_PIPE0_TDM_CONFIGr;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
    MONTEREY_TDM_ACTIVE_CAL(unit) = soc_reg_field_get(unit, reg, rval, CURR_CALf);

    monterey_tdm = MONTEREY_TDM_STRUCT_ACTIVE(unit);

    SOC_IF_ERROR_RETURN(soc_mn_port_resource_tdm_calculate(unit, monterey_tdm));

    reg = ES_PIPE0_GRP_WT_SELECTr;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &wt_group_select));


    for (group = 0; group < MONTEREY_MMU_OVS_GROUP_COUNT; group++) {
        speed_class = 0;
        speed_group = 0;
        reg = mmu_ovs_group_regs[group];

        wt_group = soc_reg_field_get(unit, ES_PIPE0_GRP_WT_SELECTr, wt_group_select,
                                     mn_mmu_ovs_group_wt_select_fields[group]);

        for (slot = 0; slot < MONTEREY_MMU_OVS_GROUP_TDM_LENGTH; slot++) {

            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, slot, &rval));
            mmu_port = soc_reg_field_get(unit, reg, rval, MMU_PORTf);

            if (mmu_port == 0x7f) {
                phy_port = NUM_EXT_PORTS;
            } else {
                phy_port = SOC_INFO(unit).port_m2p_mapping[mmu_port];
                if (speed_class == 0) {
                    speed_class = si->port_speed_max[si->port_p2l_mapping[phy_port]];
                    if (SOC_FAILURE(_soc_monterey_mmu_ovs_speed_class_map_get(
                                    unit, &speed_class, &speed_group, &speed_spacing))) {
                        continue;
                    }
                    MONTEREY_TDM_WT_GRP_RESERVE(unit, wt_group, speed_group);
                }
            }
            monterey_tdm->egress_ovs_tdm.mmu_ovs_group_tdm[group][slot] = phy_port;
        }
        MONTEREY_TDM_WT_GRP_LOG(unit);

        monterey_tdm->egress_ovs_tdm.mmu_ovs_group_speed[group] = speed_class;
        monterey_tdm->egress_ovs_tdm.mmu_ovs_group_wt_group[group] = wt_group;
    }

    return SOC_E_NONE;
}

STATIC int
soc_monterey_tdm_init(int unit)
{
    soc_info_t *si;
    soc_pbmp_t pbmp;

    int rv, index, cur_cal;
    int slot, port, phy_port, mmu_port;
    soc_monterey_tdm_t *monterey_tdm;
    int roe_speed_cpri = 0;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 values[4];
    soc_field_t fields[4];
    soc_mem_t mem;
    static const soc_mem_t iarb_tdm_mem = IARB_MAIN_TDMm;

    cur_cal = MONTEREY_TDM_ACTIVE_CAL(unit);
    monterey_tdm = MONTEREY_TDM_STRUCT_ACTIVE(unit);

    sal_memset(monterey_tdm, 0, sizeof(soc_monterey_tdm_t));

    si = &SOC_INFO(unit);

    PBMP_PORT_ITER(unit, port) {
        if (SOC_PBMP_MEMBER(si->all.disabled_bitmap, port)) {
            continue;
        }
        phy_port = si->port_l2p_mapping[port];

        monterey_tdm->speed[phy_port] = si->port_speed_max[port];
        MONTEREY_TDM_SPEED_ADJUST(unit, port, monterey_tdm->speed[phy_port]);
        if(IS_CPRI_PORT(unit,port)) {
           roe_speed_cpri = soc_property_port_get(unit, port, spn_CPRI_PORT_INIT_SPEED_ID, -1);
           if(roe_speed_cpri > 5) {
              monterey_tdm->speed[phy_port] = 25000;

           }
        }
        monterey_tdm->port_state[phy_port] =
            IS_OVERSUB_PORT(unit, port) ? PORT_STATE_OVERSUB : PORT_STATE_LINERATE;

        /*
         * Primary port will be setup as line-rate or oversub.
         * Subsequent physical ports (sister-ports) will be either
         * DISABLED or SUBPORT/COMBINED mode.
         *
         * TDM Algo expects lanes info based on speed.
         */
        if (soc_monterey_port_is_falcon(unit, phy_port)) { /*  Falcons  */
            if (monterey_tdm->speed[phy_port] > 25000) {
                monterey_tdm->port_state[phy_port + 1] = PORT_STATE_SUBPORT;
            }
            if (monterey_tdm->speed[phy_port] > 50000) {
                monterey_tdm->port_state[phy_port + 2] = PORT_STATE_SUBPORT;
                monterey_tdm->port_state[phy_port + 3] = PORT_STATE_SUBPORT;
            }
        } else {                                         /*  Eagles  */
            if (monterey_tdm->speed[phy_port] > 10000) {
                monterey_tdm->port_state[phy_port + 1] = PORT_STATE_SUBPORT;
            }
            if (monterey_tdm->speed[phy_port] > 20000) {
                monterey_tdm->port_state[phy_port + 2] = PORT_STATE_SUBPORT;
                monterey_tdm->port_state[phy_port + 3] = PORT_STATE_SUBPORT;
            }
            if (monterey_tdm->speed[phy_port] > 40000) {
                monterey_tdm->port_state[phy_port + 4] = PORT_STATE_SUBPORT;
                monterey_tdm->port_state[phy_port + 5] = PORT_STATE_SUBPORT;
                monterey_tdm->port_state[phy_port + 6] = PORT_STATE_SUBPORT;
                monterey_tdm->port_state[phy_port + 7] = PORT_STATE_SUBPORT;
                monterey_tdm->port_state[phy_port + 8] = PORT_STATE_SUBPORT;
                monterey_tdm->port_state[phy_port + 9] = PORT_STATE_SUBPORT;
            }
        }

    }

    /* CPU, LB and MACSEC ports */
    monterey_tdm->speed[0] = 1000;
    monterey_tdm->speed[65] = 1000;
    monterey_tdm->speed[66] = 20000;

    monterey_tdm->tdm_bw = si->bandwidth / 1000;

    SOC_PBMP_ASSIGN(pbmp, si->oversub_pbm);
    SOC_PBMP_AND(pbmp, si->xpipe_pbm);

    /* tell tdm code the device is oversub if any ports is oversub */
    monterey_tdm->is_oversub = SOC_PBMP_NOT_NULL(pbmp);
    SOC_PBMP_ASSIGN(pbmp, si->oversub_pbm);

    for (slot = 0; slot <= MONTEREY_MMU_TDM_LENGTH; slot++) {
        monterey_tdm->mmu_tdm[slot] = NUM_EXT_PORTS;
    }

    if (LOG_CHECK(BSL_LS_SOC_TDM | BSL_INFO)) {
        LOG_CLI((BSL_META_U(unit, "tdm_bw: %dG\n"), monterey_tdm->tdm_bw));
        LOG_CLI((BSL_META_U(unit, "port speed:")));
        for (index = 0; index < NUM_EXT_PORTS; index++) {
            if (index % 8 == 0) {
                LOG_CLI((BSL_META_U(unit, "\n    ")));
            }
            LOG_CLI((BSL_META_U(unit, " %06d(%03d)"),
                     monterey_tdm->speed[index], index));
        }
        LOG_CLI((BSL_META_U(unit, "\n")));
        LOG_CLI((BSL_META_U(unit, "port state map:")));
        for (index = 0; index < NUM_EXT_PORTS; index++) {
            if (index % 8 == 0) {
                LOG_CLI((BSL_META_U(unit, "\n    ")));
            }
            if (index == 0 || index == (NUM_EXT_PORTS - 1)) {
                LOG_CLI((BSL_META_U(unit, " --------")));
            } else {
                LOG_CLI((BSL_META_U(unit, " %03d(%03d)"),
                         monterey_tdm->port_state[index], index));
            }
        }
        LOG_CLI((BSL_META_U(unit, "\n")));
    }


    SOC_IF_ERROR_RETURN(soc_monterey_tdm_calc(unit, monterey_tdm));

    SOC_IF_ERROR_RETURN(soc_monterey_tdm_update(unit, 1, cur_cal, monterey_tdm));


    /* Configure IARB TDM */
    mem = iarb_tdm_mem;
    sal_memset(entry, 0, soc_mem_entry_words(unit, mem) * sizeof(uint32));
    for (slot = 0; slot < MONTEREY_IARB_TDM_LENGTH; slot++) {
        if (slot > monterey_tdm->iarb_tdm_wrap_ptr) {
            break;
        }
/* temp fix for tdm issue once new tdm is in place this check shold be removed*/
     if ((monterey_tdm->iarb_tdm[slot] > 0x7 || monterey_tdm->iarb_tdm[slot] < 0) &&
            slot == monterey_tdm->iarb_tdm_wrap_ptr) {
            monterey_tdm->iarb_tdm[slot] = 3;
     }
        if (monterey_tdm->iarb_tdm[slot] > 0x7)  {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "Invalid IARB TDM slot(%d) = %d !!\n"),
                                  slot, monterey_tdm->iarb_tdm[slot]));
        }
        soc_mem_field32_set(unit, mem, entry, TDM_SLOTf,
                            monterey_tdm->iarb_tdm[slot]);
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, slot, entry));
    }

    /* IARB_TDM_CONTROL */
    fields[0] = TDM_WRAP_PTRf;
    values[0] = monterey_tdm->iarb_tdm_wrap_ptr - 1;
    fields[1] = DISABLEf;
    values[1] = 0;
    SOC_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, IARB_TDM_CONTROLr, REG_PORT_ANY, 2,
                                 fields, values));


    fields[0] = FIELD_Af;
    values[0] = 1023;
    fields[1] = FIELD_Bf;
    values[1] = 9215;
    fields[2] = FIELD_Cf;
    values[2] = 16;
    rv = soc_reg_fields32_modify(unit, EGR_FORCE_SAF_CONFIGr, REG_PORT_ANY, 3, fields, values);
    SOC_IF_ERROR_RETURN(rv);

    fields[0] = FIELD_Af;
    values[0] = 1;
    fields[1] = DI_ACT_THRf;
    values[1] = 20;
    rv = soc_reg_fields32_modify(unit, ES_PIPE0_MMU_1DBG_Cr, REG_PORT_ANY, 2, fields, values);
    SOC_IF_ERROR_RETURN(rv);

    rv = soc_reg_field32_modify(unit, ES_PIPE0_MMU_DI_THRr, REG_PORT_ANY, OS_SLT_LMTf, 1000);
    SOC_IF_ERROR_RETURN(rv);

    rv = soc_reg_field32_modify(unit, ES_PIPE0_MMU_2DBG_C_1r, REG_PORT_ANY, FIELD_Bf, 0);
    SOC_IF_ERROR_RETURN(rv);
    rv = soc_reg_field32_modify(unit, ES_PIPE0_MMU_2DBG_C_0r, REG_PORT_ANY,
                                FIELD_Af, (si->frequency * 200));
    SOC_IF_ERROR_RETURN(rv);


    /* values array for filling mmu_1dbg_a regs  */
    values[0] = values[1] = values[2] = 0;

    PBMP_PORT_ITER(unit, port) {

        phy_port = si->port_l2p_mapping[port];
        if (phy_port == -1 ) {
            continue;
        }
        mmu_port = si->port_p2m_mapping[phy_port];
        if (mmu_port == -1) {
            continue;
        }

        SOC_IF_ERROR_RETURN(soc_mn_mmu_delay_insertion_set(unit, port));


    }
    return SOC_E_NONE;
}

STATIC int
_soc_monterey_hash_init(int unit)
{
    soc_field_t fields[4];
    uint32 values[4];

    /* L2 dedicated banks */
    fields[0] = BANK0_HASH_OFFSETf;
    values[0] = 0;  /* CRC32_LOWER */
    fields[1] = BANK1_HASH_OFFSETf;
    values[1] = 16; /* CRC32_UPPER */
    SOC_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, L2_TABLE_HASH_CONTROLr, REG_PORT_ANY, 2,
                                 fields, values));

    /* L3 dedicated banks */
    fields[0] = BANK6_HASH_OFFSETf;
    values[0] = 0;  /* CRC32_LOWER */
    fields[1] = BANK7_HASH_OFFSETf;
    values[1] = 12;  /* CRC32 >> 12 */
    fields[2] = BANK8_HASH_OFFSETf;
    values[2] = 24; /* CRC16_LOWER << 8 | CRC32 >> 24 */
    fields[3] = BANK9_HASH_OFFSETf;
    values[3] = 36; /* CRC16 >> 4 */
    SOC_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, L3_TABLE_HASH_CONTROLr, REG_PORT_ANY, 4,
                                 fields, values));

    return SOC_E_NONE;
}

#define PORTS_PER_LED_CHAIN 64

STATIC int
_soc_monterey_ledup_init(int unit)
{
    uint32 rval = 0, ix;
    uint8 remap_value[SOC_MAX_NUM_PORTS];
    int phy_port;

    /* configure the LED scan delay cycles */
    SOC_IF_ERROR_RETURN(READ_CMIC_LEDUP0_CTRLr(unit, &rval));
    soc_reg_field_set(unit, CMIC_LEDUP0_CTRLr, &rval, LEDUP_SCAN_START_DELAYf, 0x1A);
    soc_reg_field_set(unit, CMIC_LEDUP0_CTRLr, &rval, LEDUP_SCAN_INTRA_PORT_DELAYf, 4);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_CTRLr(unit, rval));

    rval =0;
    /* Initialize LED Port remap registers */

    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_12_15r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_16_19r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_20_23r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_24_27r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_28_31r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_32_35r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_36_39r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_40_43r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_44_47r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_48_51r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_52_55r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_60_63r(unit, rval));

     for (ix = 0; ix < PORTS_PER_LED_CHAIN; ix++) {
        /* Chain 0: Phy Ports 1-64
         * LED HW Status arrives descending order to CMIC
         * i.e Status for Port 64 comes first and for Port 1 last
         */
        phy_port = PORTS_PER_LED_CHAIN - ix;
        remap_value[ix] = phy_port - 1   ;

    }

    /* Configure LED Port remap registers */
    ix = 0;
    rval = 0;
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r, &rval, REMAP_PORT_0f,  remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r, &rval, REMAP_PORT_1f, remap_value[ix++] );
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r, &rval, REMAP_PORT_2f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r, &rval, REMAP_PORT_3f, remap_value[ix++]);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r, &rval, REMAP_PORT_4f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r, &rval, REMAP_PORT_5f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r, &rval, REMAP_PORT_6f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r, &rval, REMAP_PORT_7f, remap_value[ix++]);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r, &rval, REMAP_PORT_8f,  remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r, &rval, REMAP_PORT_9f,  remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r, &rval, REMAP_PORT_10f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r, &rval, REMAP_PORT_11f, remap_value[ix++]);
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_12_15r, &rval, REMAP_PORT_12f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_12_15r, &rval, REMAP_PORT_13f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_12_15r, &rval, REMAP_PORT_14f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_12_15r, &rval, REMAP_PORT_15f, remap_value[ix++]);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_12_15r(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_16_19r, &rval,REMAP_PORT_16f,  remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_16_19r, &rval, REMAP_PORT_17f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_16_19r, &rval, REMAP_PORT_18f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_16_19r, &rval, REMAP_PORT_19f, remap_value[ix++]);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_16_19r(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_20_23r, &rval, REMAP_PORT_20f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_20_23r, &rval, REMAP_PORT_21f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_20_23r, &rval, REMAP_PORT_22f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_20_23r, &rval, REMAP_PORT_23f, remap_value[ix++]);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_20_23r(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_24_27r, &rval, REMAP_PORT_24f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_24_27r, &rval, REMAP_PORT_25f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_24_27r, &rval, REMAP_PORT_26f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_24_27r, &rval, REMAP_PORT_27f, remap_value[ix++]);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_24_27r(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_28_31r, &rval, REMAP_PORT_28f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_28_31r, &rval,REMAP_PORT_29f,  remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_28_31r, &rval, REMAP_PORT_30f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_28_31r, &rval, REMAP_PORT_31f, remap_value[ix++]);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_28_31r(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_32_35r, &rval, REMAP_PORT_32f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_32_35r, &rval, REMAP_PORT_33f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_32_35r, &rval, REMAP_PORT_34f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_32_35r, &rval, REMAP_PORT_35f, remap_value[ix++]);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_32_35r(unit, rval));




    rval = 0;
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_36_39r, &rval, REMAP_PORT_36f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_36_39r, &rval, REMAP_PORT_37f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_36_39r, &rval, REMAP_PORT_38f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_36_39r, &rval, REMAP_PORT_39f, remap_value[ix++]);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_36_39r(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_40_43r, &rval, REMAP_PORT_40f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_40_43r, &rval, REMAP_PORT_41f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_40_43r, &rval, REMAP_PORT_42f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_40_43r, &rval, REMAP_PORT_43f, remap_value[ix++]);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_40_43r(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_44_47r, &rval, REMAP_PORT_44f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_44_47r, &rval, REMAP_PORT_45f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_44_47r, &rval, REMAP_PORT_46f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_44_47r, &rval, REMAP_PORT_47f, remap_value[ix++]);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_44_47r(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_48_51r, &rval, REMAP_PORT_48f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_48_51r, &rval, REMAP_PORT_49f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_48_51r, &rval, REMAP_PORT_50f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_48_51r, &rval, REMAP_PORT_51f, remap_value[ix++]);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_48_51r(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_52_55r, &rval, REMAP_PORT_52f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_52_55r, &rval, REMAP_PORT_53f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_52_55r, &rval, REMAP_PORT_54f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_52_55r, &rval, REMAP_PORT_55f, remap_value[ix++]);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_52_55r(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_56_59r, &rval, REMAP_PORT_56f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_56_59r, &rval, REMAP_PORT_57f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_56_59r, &rval, REMAP_PORT_58f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_56_59r, &rval, REMAP_PORT_59f, remap_value[ix++]);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_56_59r(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_60_63r, &rval, REMAP_PORT_60f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_60_63r, &rval, REMAP_PORT_61f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_60_63r, &rval, REMAP_PORT_62f, remap_value[ix++]);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_60_63r, &rval, REMAP_PORT_63f, remap_value[ix++]);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_60_63r(unit, rval));



    /* initialize the UP0, UP1 data ram */
    rval = 0;
    for (ix = 0; ix < 256; ix++) {
        SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_DATA_RAMr(unit,ix, rval));
    }

    return SOC_E_NONE;
}
int
bcm_monterey_send_packetport(int unit , int port , int cos , int num )
{

	int     j=0;
	int   pkt_size = 256 ;
	bcm_pkt_t         *pkt = NULL;
	char pkt_DA_SA[12] = {0x00,0x00,0x33,0x44,0x55,0x66,0x00,0x01,0x02,0x03,0x04,0x05};
	uint8 pkt_vlan_lengthtype[6] = {0x81,0x00,0x00,0x20,0x0,0x00};
	SOC_IF_ERROR_RETURN(bcm_pkt_alloc(unit, pkt_size, BCM_TX_CRC_REGEN, &pkt));
	sal_memset(pkt->pkt_data[0].data, 0xAA, pkt_size);
	sal_memcpy(pkt->pkt_data->data, pkt_DA_SA, 12);
	sal_memcpy(pkt->pkt_data->data+12, pkt_vlan_lengthtype, 6);
	SOC_PBMP_CLEAR(pkt->tx_pbmp); /* with ether mode, clear destination pbmp */
	SOC_PBMP_PORT_SET(pkt->tx_pbmp, port);
	SOC_PBMP_CLEAR(pkt->tx_upbmp); /* send all packets tagged */
	pkt->cos = cos ;
	for ( j =0 ; j < num ; j++ )  {
       	     bcm_tx(unit, pkt, NULL);
	}
	return SOC_E_NONE;

}


#if defined(SER_TR_TEST_SUPPORT)
soc_ser_test_functions_t ser_monterey_test_fun;
#endif
/*
STATIC int
_soc_monterey_egr_buf_reset(int unit, int port, int reset)
{
    uint32 entry[SOC_MAX_MEM_WORDS];

    SOC_IF_ERROR_RETURN(
        READ_EGR_PER_PORT_BUFFER_SFT_RESETm(unit, MEM_BLOCK_ALL,
                                            port, entry));
    soc_mem_field32_set(unit, EGR_PER_PORT_BUFFER_SFT_RESETm,
                        entry, ENABLEf, reset);
    SOC_IF_ERROR_RETURN(
        WRITE_EGR_PER_PORT_BUFFER_SFT_RESETm(unit, MEM_BLOCK_ALL,
                                             port, entry));

    return SOC_E_NONE;
}
*/
STATIC int
_soc_portctrl_monterey_port_init(int unit)
{
    soc_info_t *si;
    int port;

    si = &SOC_INFO(unit);


    /* Cut through settings for all ports */
    PBMP_ALL_ITER(unit, port) {
        if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port)  ) {
            continue;
        }
        SOC_IF_ERROR_RETURN(soc_monterey_port_asf_set(unit, port, si->port_speed_max[port]));
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_monterey_idb_buf_reset
 * Purpose:
 *      Poll until Cell Assembly and OBM buffers are empty.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical SOC port number.
 *      reset   - (IN) Reset.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_monterey_idb_buf_reset(int unit, soc_port_t port, int reset)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_reg_t reg;
    soc_field_t field;
    uint64 rval64;
    int phy_port, port_index, xlp, pgw, obm, pgw_inst;
    soc_timeout_t to;
    int use_count, fifo_empty;
    int wait_time = 250000;

    static const soc_reg_t pgw_obm_use_counter_regs[] = {
        IDB_OBM0_USAGEr, IDB_OBM1_USAGEr, IDB_OBM2_USAGEr,
        IDB_OBM3_USAGEr, IDB_OBM4_USAGEr, IDB_OBM5_USAGEr,
        IDB_OBM6_USAGEr, IDB_OBM7_USAGEr, IDB_OBM8_USAGEr
    };
    static const soc_reg_t obm_ctrl_regs[] = {
        IDB_OBM0_CONTROLr, IDB_OBM1_CONTROLr, IDB_OBM2_CONTROLr,
        IDB_OBM3_CONTROLr, IDB_OBM4_CONTROLr, IDB_OBM5_CONTROLr,
        IDB_OBM6_CONTROLr, IDB_OBM7_CONTROLr, IDB_OBM8_CONTROLr
    };
    static const soc_reg_t obm_ca_ctrl_regs[] = {
        IDB_OBM0_CA_CONTROLr, IDB_OBM1_CA_CONTROLr, IDB_OBM2_CA_CONTROLr,
        IDB_OBM3_CA_CONTROLr, IDB_OBM4_CA_CONTROLr, IDB_OBM5_CA_CONTROLr,
        IDB_OBM6_CA_CONTROLr, IDB_OBM7_CA_CONTROLr, IDB_OBM8_CA_CONTROLr
    };
    static const soc_field_t port_reset_fields[] = {
        PORT0_RESETf, PORT1_RESETf, PORT2_RESETf, PORT3_RESETf
    };

    if (SAL_BOOT_QUICKTURN) {
        wait_time *= 20;
        wait_time *= 3;
    }
    /* Get physical port PGW information */
    phy_port = si->port_l2p_mapping[port];
    pgw = si->port_group[port];
    xlp = si->port_serdes[port];
    port_index = (phy_port - 1) % 4;

    obm = xlp % MONTEREY_TSCS_PER_PGW;
    pgw_inst = pgw | SOC_REG_ADDR_INSTANCE_MASK;

    /* Assume values returned above are valid */
    reg = pgw_obm_use_counter_regs[obm];
    field = TOTAL_COUNTf;

    if (reset && (!SAL_BOOT_BCMSIM)) {
        /* Poll until Cell Assembly and OBM buffers are empty */
        soc_timeout_init(&to, wait_time, 0);
        for (;;) {
            /* Get PGW use counter */
            SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, pgw_inst, port_index, &rval64));
            use_count = soc_reg64_field32_get(unit, reg, rval64, field);

            /* Get PGW fifo empty flag */
            SOC_IF_ERROR_RETURN(READ_PGW_BOD_STATUSr(unit, port, &rval64));
            fifo_empty = soc_reg64_field32_get(unit, PGW_BOD_STATUSr,
                                               rval64, EXP_PGW_BOD_FIFO_EMPTYf);

            fifo_empty |= soc_reg64_field32_get(unit, PGW_BOD_STATUSr,
                                               rval64, PREEMPT_PGW_BOD_FIFO_EMPTYf);
            LOG_INFO(BSL_LS_SOC_PORT,
                      (BSL_META_U(unit,
                                  "PGW/OBM buffer reset : port %d, %s (log=%d phy=%d), "
                                  "subport: %d tsc: %d obm:%d pgw: %d"
                                  "(use_count: %d) "
                                  "(pgw_bod_fifo_emty:%d)\n"),
                       unit, SOC_PORT_NAME(unit, port), port, phy_port,
                       port_index, xlp, obm, pgw,
                       use_count, fifo_empty));

            if ((use_count == 0) && (fifo_empty == 1)) {
                break;
            }

            if (soc_timeout_check(&to)) {
                LOG_ERROR(BSL_LS_SOC_PORT,
                          (BSL_META_U(unit,
                                      "PGW/OBM buffer reset timeout: port %d, %s, "
                                      "timeout (use_count: %d) "
                                      "(pgw_bod_fifo_emty:%d)\n"),
                           unit, SOC_PORT_NAME(unit, port),
                           use_count, fifo_empty));
                return SOC_E_INTERNAL;
            }
        }
    }

    reg = obm_ctrl_regs[obm];
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, reg, pgw_inst,
                                               port_reset_fields[port_index], reset));

    reg = obm_ca_ctrl_regs[obm];
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, reg, pgw_inst,
                                               port_reset_fields[port_index], reset));

    return SOC_E_NONE;
}

STATIC int
_soc_monterey_pm_intr_init(int unit)
{
    memset(&_monterey_pm_intr_functions, 0,
           sizeof(_monterey_pm_intr_functions));
    _monterey_pm_intr_functions._soc_port_macro_intr_f =
        &soc_monterey_ser_error;
    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_monterey_mmu_ep_credit_reset
 * Purpose:
 *      Initalize EP credits in MMU so EP is in charge
 *      of distributing the correct number of credits.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical SOC port number.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_soc_monterey_mmu_ep_credit_reset(int unit, soc_port_t port)
{
    soc_info_t *si = &SOC_INFO(unit);
    int phy_port;
    int mmu_port;
    soc_reg_t reg;
    soc_reg_t reg1;
    uint32 rval;

    /* Get physical port */
    phy_port = si->port_l2p_mapping[port];
    if (phy_port == -1) {
        return SOC_E_INTERNAL;
    }

    /* Get MMU port */
    mmu_port = si->port_p2m_mapping[phy_port];
    if (mmu_port == -1) {
        return SOC_E_INTERNAL;
    }
    reg = ES_PIPE0_MMU_PORT_CREDIT_PREEMPTr;
    reg1 = ES_PIPE0_MMU_PORT_CREDIT_EXPRESSr;

    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, reg1, REG_PORT_ANY, mmu_port, &rval));
    soc_reg_field_set(unit, reg1, &rval, INIT_CREDITf, 0);
    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, reg1, REG_PORT_ANY, mmu_port, rval));
    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, reg, REG_PORT_ANY, mmu_port, &rval));
    soc_reg_field_set(unit, reg, &rval, INIT_CREDITf, 0);
    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, reg, REG_PORT_ANY, mmu_port, rval));
    return SOC_E_NONE;
}


STATIC int
_soc_monterey_port_credit_size_get(int unit, soc_port_t port, int *ep_credit_size)
{
    soc_info_t *si;
    si = &SOC_INFO(unit);

    /*
     * For MACSEC, CPU and LB port port_speed_max is -1.
     */
    if (si->port_speed_max[port] == -1) {
        if (IS_MACSEC_PORT(unit, port) ) {
            *ep_credit_size =  20;
        } else {
            *ep_credit_size =  11;
        }
    } else if (si->port_speed_max[port] <= 11000) {
        *ep_credit_size = IS_OVERSUB_PORT(unit,port) ? 7 : 11;
    } else if (si->port_speed_max[port] <= 27000) {
        *ep_credit_size = IS_OVERSUB_PORT(unit,port) ? 11 : 16;
    } else if (si->port_speed_max[port] <= 42000) {
        *ep_credit_size = IS_OVERSUB_PORT(unit,port) ? 23 : 20;
    } else if (si->port_speed_max[port] <= 53000) {
        *ep_credit_size = IS_OVERSUB_PORT(unit,port) ? 17 : 23;
    } else {
        *ep_credit_size = IS_OVERSUB_PORT(unit,port) ? 30 : 39;
    }

    *ep_credit_size = *ep_credit_size - 1;

     return SOC_E_NONE;
 }
/*
  * Function:
  *      _soc_monterey_mmu_ep_credit_set
  * Purpose:
  *      Initalize EP credits in MMU so EP is in charge
  *      of distributing the correct number of credits.
  * Parameters:
  *      unit    - (IN) Unit number.
 *      port    - (IN) Logical SOC port number.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_soc_monterey_mmu_ep_credit_set(int unit, soc_port_t port)
{
    soc_info_t *si = &SOC_INFO(unit);
    int phy_port;
    int mmu_port;
    int ep_credit_size = 0;
    soc_reg_t reg;
    uint32 rval;

    /* Get physical port */
    phy_port = si->port_l2p_mapping[port];
    if (phy_port == -1) {
        return SOC_E_INTERNAL;
    }

    /* Get MMU port */
    mmu_port = si->port_p2m_mapping[phy_port];
    if (mmu_port == -1) {
        return SOC_E_INTERNAL;
    }

    /* Wait until credits are initialised at non-optimum value
     * EP passes MMU 1 credit per cycle, so worse case delay is
     *  95 cycles after EGR_ENABLE is set.
     * @793.75MHz, this is ~120ns.
     */
    sal_usleep(1);

    reg = ES_PIPE0_MMU_PORT_CREDIT_EXPRESSr;

    SOC_IF_ERROR_RETURN(_soc_monterey_port_credit_size_get(unit, port, &ep_credit_size));
    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, reg, REG_PORT_ANY, mmu_port, &rval));
    soc_reg_field_set(unit, reg, &rval, INIT_CREDITf, ep_credit_size);
    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, reg, REG_PORT_ANY, mmu_port, rval));

     return SOC_E_NONE;
 }
/*
 * Function:
 *      soc_monterey_edb_buf_reset
 * Purpose:
 *      Initialize EP credits in MMU, release EDB port buffer and
 *      enable cell request in EP.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical SOC port number.
 *      reset   - (IN) Reset.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_monterey_edb_buf_reset(int unit, soc_port_t port, int reset)
{
    soc_info_t *si = &SOC_INFO(unit);
    uint32 entry[SOC_MAX_MEM_WORDS];
    int phy_port;
    uint32 rval;
    int level;
    soc_timeout_t to;
    int wait_time = 250000;
    uint32 preemption_enable = 0;
    if (SAL_BOOT_QUICKTURN) {
        wait_time *= 20;
    }

    /* Get physical port */
    phy_port = si->port_l2p_mapping[port];
    if (phy_port == -1) {
        return SOC_E_INTERNAL;
    }

    if (reset) {
        /* Set register to get number of used entries in EDB buffer */
        SOC_IF_ERROR_RETURN
            (READ_EGR_EDB_MISC_CTRLr(unit, &rval));
        soc_reg_field_set(unit, EGR_EDB_MISC_CTRLr, &rval,
                          SELECT_CURRENT_USED_ENTRIESf, 1);
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_EDB_MISC_CTRLr(unit, rval));

        soc_timeout_init(&to, wait_time, 0);

        /* Poll until EDB buffer is empty */
        for (;;) {
            SOC_IF_ERROR_RETURN
                (READ_EGR_MAX_USED_ENTRIESm(unit, MEM_BLOCK_ALL,
                                            phy_port, entry));
            level = soc_mem_field32_get(unit, EGR_MAX_USED_ENTRIESm,
                                        entry, LEVELf);
            if (level == 0) {
                break;
            }
            if (soc_timeout_check(&to)) {
                LOG_ERROR(BSL_LS_SOC_PORT,
                          (BSL_META_U(unit,
                                      "EDB buffer drain timeout: "
                                      "port %d, %s, "
                                      "timeout (pending: %d)\n"),
                           unit, SOC_PORT_NAME(unit, port), level));
                return SOC_E_INTERNAL;
            }
        }

        /*
         * Hold EDB port buffer in reset state and disable cell
         * request generation in EP.
         */
        SOC_IF_ERROR_RETURN
            (READ_EGR_PORT_CREDIT_RESETm(unit, MEM_BLOCK_ALL,
                                                 phy_port, entry));
        soc_mem_field32_set(unit, EGR_PORT_CREDIT_RESETm,
                            entry, VALUEf, 1);
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_PORT_CREDIT_RESETm(unit, MEM_BLOCK_ALL,
                                                  phy_port, entry));
        SOC_IF_ERROR_RETURN
            (READ_EGR_ENABLEm(unit, MEM_BLOCK_ALL, phy_port, entry));
        soc_mem_field32_set(unit, EGR_ENABLEm, entry, PRT_ENABLEf, 0);
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_ENABLEm(unit, MEM_BLOCK_ALL, phy_port, entry));
        SOC_IF_ERROR_RETURN
            (READ_EGR_PREEMPT_ENABLEm(unit, MEM_BLOCK_ALL, phy_port, entry));
        soc_mem_field32_set(unit, EGR_PREEMPT_ENABLEm, entry, PRT_ENABLEf, 0);
        SOC_IF_ERROR_RETURN
                (WRITE_EGR_PREEMPT_ENABLEm(unit, MEM_BLOCK_ALL, phy_port, entry));
    } else {


        /*
         * Release EDB port buffer reset and
         * enable cell request generation in EP.
         */
        SOC_IF_ERROR_RETURN
            (READ_EGR_PORT_CREDIT_RESETm(unit, MEM_BLOCK_ALL,
                                                 phy_port, entry));
        soc_mem_field32_set(unit, EGR_PORT_CREDIT_RESETm,
                            entry, VALUEf, 0);
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_PORT_CREDIT_RESETm(unit, MEM_BLOCK_ALL,
                                                  phy_port, entry));

        SOC_IF_ERROR_RETURN
            (READ_EGR_ENABLEm(unit, MEM_BLOCK_ALL, phy_port, entry));
        soc_mem_field32_set(unit, EGR_ENABLEm, entry, PRT_ENABLEf, 1);
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_ENABLEm(unit, MEM_BLOCK_ALL, phy_port, entry));

        SOC_IF_ERROR_RETURN(
        bcmi_mn_preemption_capability_get(unit , port , &preemption_enable));
        if (preemption_enable ) {
            SOC_IF_ERROR_RETURN
               (READ_EGR_PREEMPT_ENABLEm(unit, MEM_BLOCK_ALL, phy_port, entry));
            soc_mem_field32_set(unit, EGR_PREEMPT_ENABLEm, entry, PRT_ENABLEf, 1);
            SOC_IF_ERROR_RETURN
                (WRITE_EGR_PREEMPT_ENABLEm(unit, MEM_BLOCK_ALL, phy_port, entry));
        }
    }
    return SOC_E_NONE;
}

int
soc_monterey_ports_edb_config(int unit, int asf, soc_pbmp_t pbm)
{
    int rv;
   /* uint32 osct_pbm;
    uint64  lrct_pbm;*/
    soc_info_t *si;
    soc_port_t port, phy_port ;
    /*int pm_id, pgw_inst;*/
      int linerate;

    soc_mem_t edb_mem = EGR_EDB_XMIT_CTRLm;
    soc_mem_t edb_mem1 = EGR_PREEMPT_EDB_XMIT_CTRLm;
    soc_field_t edb_fields[3] = {START_CNTf, WAIT_FOR_MOPf, WAIT_FOR_2ND_MOPf};
    uint32 edb_values[3] = {0};


    si = &SOC_INFO(unit);

    /* Remove extraneous ports from the input list */
   /*osct_pbm = 0;
    COMPILER_64_ZERO(lrct_pbm);*/
    SOC_PBMP_AND(pbm, PBMP_ALL(unit));

    SOC_PBMP_ITER(pbm, port) {
        phy_port = si->port_l2p_mapping[port];
        /*mmu_port = si->port_p2m_mapping[phy_port];
        pgw_inst = si->port_group[port] | SOC_REG_ADDR_INSTANCE_MASK;*/
        linerate = (!SOC_PBMP_MEMBER(si->oversub_pbm, port));

        /* *pm_id = ((phy_port - 1) / MONTEREY_PORTS_PER_TSC) % MONTEREY_TSCS_PER_PGW;*/
        if (!soc_monterey_port_is_falcon(unit, phy_port)) { /* Eagle Cores */
            if (si->port_speed_max[port] <= 21000) {  /* 0-21G */
                edb_values[0] = (linerate) ? (asf ? 7 : 0) : (asf ? 0 : 7);
                edb_values[1] = (linerate) ? (asf ? 0 : 1) : 0;
                edb_values[2] = 0; /* linerate/oversub and cut-thru or no cut-thru */

            } else if (si->port_speed_max[port] <= 42000) {  /* 21G-42G */
                edb_values[0] = (linerate) ? (asf ? 7 : 0) : (asf ? 11 : 7);
                edb_values[1] = (linerate) ? (asf ? 0 : 1) : 0;
                edb_values[2] = 0; /* linerate/oversub and cut-thru or no cut-thru */

            } else { /* 42G or more (100G)  */
                edb_values[0] = (linerate) ? (asf ? 7 : 0) : (asf ? 0 : 4);
                edb_values[1] = (linerate) ? (asf ? 0 : 1) : 0;
                edb_values[2] = 0; /* linerate/oversub and cut-thru or no cut-thru */

            }
        } else { /*  Falcon Cores  */
            if (si->port_speed_max[port] <= 11000) {  /* 0-11G */
                edb_values[0] = (linerate) ? (asf ? 3 : 0) : (asf ? 0 : 4);
                edb_values[1] = (linerate) ? (asf ? 0 : 1) : 0;
                edb_values[2] = 0; /* linerate/oversub and cut-thru or no cut-thru */

            } else if (si->port_speed_max[port] <= 27000) {  /* 11G-27G */
                edb_values[0] = (linerate) ? (asf ? 3 : 0) : (asf ? 0 : 4);
                edb_values[1] = (linerate) ? (asf ? 0 : 1) : 0;
                edb_values[2] = 0; /* linerate/oversub and cut-thru or no cut-thru */

            } else if (si->port_speed_max[port] <= 42000) {  /* 27G-42G */
                edb_values[0] = (linerate) ? (asf ? 3 : 0) : (asf ? 5 : 4);
                edb_values[1] = (linerate) ? (asf ? 0 : 1) : 0;
                edb_values[2] = 0; /* linerate/oversub and cut-thru or no cut-thru */

            } else { /* 42G or more (50G, 53G, 100G, 106G)  */
                edb_values[0] = (linerate) ? (asf ? 7 : 0) : (asf ? 0 : 4);
                edb_values[1] = (linerate) ? (asf ? 0 : 1) : 0;
                edb_values[2] = 0; /* linerate/oversub and cut-thru or no cut-thru */
            }
        }

        rv = soc_mem_fields32_modify(unit, edb_mem, phy_port, 3, edb_fields, edb_values);
        SOC_IF_ERROR_RETURN(rv);
        rv = soc_mem_fields32_modify(unit, edb_mem1, phy_port, 3, edb_fields, edb_values);

        SOC_IF_ERROR_RETURN(rv);

    }


    return SOC_E_NONE;
}


STATIC int
soc_monterey_edb_init(int unit)
{
    soc_pbmp_t pbm;

    SOC_PBMP_ASSIGN(pbm, PBMP_PORT_ALL(unit));
    SOC_IF_ERROR_RETURN(soc_monterey_ports_edb_config(unit, 0, pbm));

    return SOC_E_NONE;
}



STATIC int
_soc_monterey_misc_deinit (int unit)
{

    SOC_IF_ERROR_RETURN(soc_mn_phy_info_deinit(unit));

    if (soc_monterey_misc_info[unit] != NULL) {
        sal_free(soc_monterey_misc_info[unit]);
        soc_monterey_misc_info[unit] = NULL;
    }

    return SOC_E_NONE;
}

STATIC int
soc_mnc_mem_modify_range(int unit, soc_mem_t mem, int min_idx, int max_idx,
                         int nfields, soc_field_t *fields, uint32 *values)
{
    uint8 *mbuf;
    void  *entry_ptr;
    int    e_idx, f_idx, size, rv;

    size = SOC_MEM_TABLE_BYTES(unit, mem);
    mbuf = soc_cm_salloc(unit, size, "soc_mem_name[mem]");

    if (NULL == mbuf) {
        return SOC_E_MEMORY;
    }

    rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY, min_idx, max_idx, mbuf);
    if (SOC_FAILURE(rv)) {
        soc_cm_sfree(unit, mbuf);
        return rv;
    }

    for (e_idx = min_idx; e_idx <= max_idx; e_idx++) {
        entry_ptr = soc_mem_table_idx_to_pointer(unit, mem, void *, mbuf, e_idx);
        for (f_idx = 0; f_idx < nfields; f_idx++) {
            /* If value is -1, program entry_idx */
            soc_mem_field32_set(unit, mem, entry_ptr, fields[f_idx],
                                (values[f_idx] == -1 ? e_idx : values[f_idx]));
        }
    }

    rv = soc_mem_write_range(unit, mem, MEM_BLOCK_ANY, min_idx, max_idx, mbuf);
    soc_cm_sfree(unit, mbuf);
    SOC_IF_ERROR_RETURN(rv);

    return SOC_E_NONE;
}

STATIC int
_soc_monterey_misc_init (int unit)
{
    static int rtag7_field_width[] = { 16, 16, 4, 16, 8, 8, 16, 16 };
    soc_info_t *si;
    uint32 rval = 0;
    uint64 rval64;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_pbmp_t pbmp;
    int port, min_port, max_port;
    int block_info_idx;
    int index, count, sub_sel, offset;
    int freq, target_freq, divisor, dividend, delay;
    int parity_enable = 0, cache_fpgm;
    soc_field_t fields[2];
    uint32 values[2];


    si = &SOC_INFO(unit);

    if (soc_monterey_misc_info[unit] == NULL) {
        soc_monterey_misc_info[unit] = sal_alloc(sizeof(*soc_monterey_misc_info[unit]),
                                               "monterey misc info");
        if (soc_monterey_misc_info[unit] == NULL) {
            return SOC_E_MEMORY;
        }
    }


    sal_memset(soc_monterey_misc_info[unit], 0, (sizeof(*soc_monterey_misc_info[unit])));

    /* Stop the mem scan task before all of the parity init takes place */
    SOC_IF_ERROR_RETURN(soc_generic_ser_mem_scan_stop(unit));
    SOC_IF_ERROR_RETURN(soc_monterey_shadow_create(unit));
    if (!SOC_WARM_BOOT(unit)) {
        /*  Initialize PORT MIB counter */
        SOC_BLOCK_ITER(unit, block_info_idx, SOC_BLK_CLPORT) {
            port = SOC_BLOCK_PORT(unit, block_info_idx);
            soc_reg_field_set(unit, CLPORT_MIB_RESETr, &rval, CLR_CNTf, 0xf);
            SOC_IF_ERROR_RETURN(WRITE_CLPORT_MIB_RESETr(unit, port, rval));
            SOC_IF_ERROR_RETURN(WRITE_CLPORT_MIB_RESETr(unit, port, 0));
        }
        SOC_BLOCK_ITER(unit, block_info_idx, SOC_BLK_CLG2PORT) {
            port = SOC_BLOCK_PORT(unit, block_info_idx);
            soc_reg_field_set(unit, CLPORT_MIB_RESETr, &rval, CLR_CNTf, 0xf);
            SOC_IF_ERROR_RETURN(WRITE_CLPORT_MIB_RESETr(unit, port, rval));
            SOC_IF_ERROR_RETURN(WRITE_CLPORT_MIB_RESETr(unit, port, 0));
        }
        SOC_BLOCK_ITER(unit, block_info_idx, SOC_BLK_XLPORT) {
            port = SOC_BLOCK_PORT(unit, block_info_idx);
            soc_reg_field_set(unit, XLPORT_MIB_RESETr, &rval, CLR_CNTf, 0xf);
            SOC_IF_ERROR_RETURN(WRITE_XLPORT_MIB_RESETr(unit, port, rval));
            SOC_IF_ERROR_RETURN(WRITE_XLPORT_MIB_RESETr(unit, port, 0));
        }
        SOC_BLOCK_ITER(unit, block_info_idx, SOC_BLK_XLPORTB0) {
            port = SOC_BLOCK_PORT(unit, block_info_idx);
            soc_reg_field_set(unit, XLPORT_MIB_RESETr, &rval, CLR_CNTf, 0xf);
            SOC_IF_ERROR_RETURN(WRITE_XLPORT_MIB_RESETr(unit, port, rval));
            SOC_IF_ERROR_RETURN(WRITE_XLPORT_MIB_RESETr(unit, port, 0));
        }

    }

    /* Stop the mem scan task before all of the parity init takes place */
    SOC_IF_ERROR_RETURN(soc_generic_ser_mem_scan_stop(unit));
    SOC_IF_ERROR_RETURN(soc_generic_sram_mem_scan_stop(unit));

    parity_enable = soc_property_get(unit, spn_PARITY_ENABLE, TRUE);
    if (parity_enable) {
        if ((!SAL_BOOT_SIMULATION || SAL_BOOT_QUICKTURN) &&
            !SOC_WARM_BOOT(unit)) {
            /* Certain mems/regs need to be cleared before enabling SER */
            SOC_IF_ERROR_RETURN(_soc_monterey_clear_mmu_memory(unit, INVALIDm));
            SOC_IF_ERROR_RETURN(_soc_monterey_clear_all_port_data(unit));
        }
        SOC_IF_ERROR_RETURN(_soc_monterey_tcam_ser_init(unit));
        SOC_IF_ERROR_RETURN(_soc_monterey_ser_enable_all(unit, TRUE));
#ifdef INCLUDE_MEM_SCAN
        soc_mem_scan_ser_list_register(unit, TRUE,
                                       _soc_monterey_tcam_ser_info[unit]);
#endif
        memset(&_monterey_ser_functions, 0, sizeof(soc_ser_functions_t));
        _monterey_ser_functions._soc_ser_fail_f = &soc_monterey_ser_fail;
        _monterey_ser_functions._soc_ser_parity_error_cmicm_intr_f =
            &soc_monterey_ser_error;
        soc_ser_function_register(unit, &_monterey_ser_functions);
        soc_monterey_ser_set_long_delay(FALSE);
#if defined(SER_TR_TEST_SUPPORT)
        /*Initialize chip-specific functions for SER testing*/
        memset(&ser_monterey_test_fun, 0, sizeof(soc_ser_test_functions_t));
        ser_monterey_test_fun.inject_error_f = &soc_monterey_ser_inject_error;
        ser_monterey_test_fun.test_mem = &soc_monterey_ser_test_mem;
        ser_monterey_test_fun.test = &soc_monterey_ser_test;
        ser_monterey_test_fun.parity_control = &_ser_test_parity_control_reg_set;
        ser_monterey_test_fun.injection_support = &soc_monterey_ser_error_injection_support;
        soc_ser_test_functions_register(unit, &ser_monterey_test_fun);
#endif /*defined(SER_TR_TEST_SUPPORT*/

    }

    if (!SOC_WARM_BOOT(unit)) {
        SOC_IF_ERROR_RETURN
            (_soc_monterey_clear_all_memory(unit, (parity_enable ? 0 : 1)));
        if (!parity_enable) {
            /* Clear enabled port's regs if not done above */
            SOC_IF_ERROR_RETURN(_soc_monterey_clear_enabled_port_data(unit));
        }
    }

    
    SOC_IF_ERROR_RETURN(_soc_monterey_port_mapping_init(unit));

    if (!(SAL_BOOT_BCMSIM || SAL_BOOT_XGSSIM)) {
        if (!SOC_WARM_BOOT(unit)) {
            if(!soc_property_get(unit, "skip_tdm_init", 0)) {
               SOC_IF_ERROR_RETURN(soc_monterey_tdm_init(unit));
            }
            SOC_IF_ERROR_RETURN(soc_monterey_idb_init(unit));
            SOC_IF_ERROR_RETURN(soc_monterey_edb_init(unit));
        } else {
            SOC_IF_ERROR_RETURN(soc_monterey_tdm_reinit(unit));
        }
    }

    sal_memset(entry, 0, sizeof(cpu_pbm_entry_t));
    soc_mem_pbmp_field_set(unit, CPU_PBMm, entry, BITMAPf, &PBMP_CMIC(unit));
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, CPU_PBMm, MEM_BLOCK_ALL, 0, entry));

    sal_memset(entry, 0, sizeof(cpu_pbm_2_entry_t));
    soc_mem_pbmp_field_set(unit, CPU_PBM_2m, entry, BITMAPf, &PBMP_CMIC(unit));
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, CPU_PBM_2m, MEM_BLOCK_ALL, 0, entry));

    sal_memset(entry, 0, sizeof(multipass_loopback_bitmap_entry_t));
    soc_mem_pbmp_field_set(unit, MULTIPASS_LOOPBACK_BITMAPm, entry, BITMAPf,
                           &PBMP_LB(unit));
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, MULTIPASS_LOOPBACK_BITMAPm,
                                      MEM_BLOCK_ALL, 0, entry));

    sal_memset(entry, 0, sizeof(egr_ing_port_entry_t));
    soc_mem_field32_set(unit, EGR_ING_PORTm, entry, PORT_TYPEf, 1);
    PBMP_HG_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, EGR_ING_PORTm,
                                          MEM_BLOCK_ALL, port, entry));
    }
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, EGR_ING_PORTm, MEM_BLOCK_ALL,
                                      si->cpu_hg_index, entry));
    soc_mem_field32_set(unit, EGR_ING_PORTm, entry, PORT_TYPEf, 2);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, EGR_ING_PORTm, MEM_BLOCK_ALL,
                                      LB_PORT(unit), entry));

    /* For FP_GLOBAL_MASK_TCAM, we don't have the pbmp's necessary to
     * initialize the cache until now.  This is the time to check for
     * the cache permission of this table.
     */
    cache_fpgm = soc_feature(unit, soc_feature_mem_cache) &&
            soc_property_get(unit, spn_MEM_CACHE_ENABLE,
                             (SAL_BOOT_RTLSIM || SAL_BOOT_XGSSIM) ? 0 : 1);
    if (soc_property_get(unit, spn_MEM_CHECK_NOCACHE_OVERRIDE, 0)) {
        char mem_name[100];
        char *mptr;

        sal_memset(mem_name, 0, sizeof(mem_name));
        sal_strlcpy(mem_name, "mem_nocache_", sizeof(mem_name));
        mptr = &mem_name[sal_strlen(mem_name)];
        sal_strlcpy(mptr, SOC_MEM_NAME(unit, FP_GLOBAL_MASK_TCAMm),
                    sizeof(mem_name) - sal_strlen(mem_name) - 1);
        if (soc_property_get(unit, mem_name, 0)) {
            cache_fpgm = 0;
        }
    }

    if (cache_fpgm && (SOC_SWITCH_BYPASS_MODE(unit) != SOC_SWITCH_BYPASS_MODE_L3_AND_FP)) {
        SOC_MEM_INFO(unit, FP_GLOBAL_MASK_TCAMm).flags |=
            SOC_MEM_FLAG_CACHABLE;
        SOC_IF_ERROR_RETURN
            (soc_mem_cache_set(unit, FP_GLOBAL_MASK_TCAMm,
                               MEM_BLOCK_ALL, TRUE));

        SOC_MEM_INFO(unit, FP_GM_FIELDSm).flags |=
            SOC_MEM_FLAG_CACHABLE;
        SOC_IF_ERROR_RETURN
            (soc_mem_cache_set(unit, FP_GM_FIELDSm,
                               MEM_BLOCK_ALL, TRUE));
    }

    SOC_IF_ERROR_RETURN(_soc_portctrl_monterey_port_init(unit));
    SOC_IF_ERROR_RETURN(_soc_monterey_pm_intr_init(unit));
    SOC_IF_ERROR_RETURN(_soc_monterey_pm_cpri_intr_enable(unit, TRUE));

    SOC_IF_ERROR_RETURN(READ_MISCCONFIGr(unit, &rval));
    soc_reg_field_set(unit, MISCCONFIGr, &rval, METERING_CLK_ENf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MISCCONFIGr(unit, rval));
    /* Enable dual hash tables */
    SOC_IF_ERROR_RETURN(_soc_monterey_hash_init(unit));
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, L3_ENTRY_CONTROL_10r,
                        REG_PORT_ANY, DISABLE_L3_ENTRY_LPf, 0xf));

    rval = 0;
    soc_reg_field_set(unit, L2_AGE_DEBUGr, &rval, AGE_COUNTf,
                      soc_mem_index_max(unit, L2Xm));
    SOC_IF_ERROR_RETURN(WRITE_L2_AGE_DEBUGr(unit, rval));
    /*
     * Egress Enable (for non-serdes ports).
     * For PM ports, it is set during port enable/disable sequence.
     */
    sal_memset(entry, 0, sizeof(egr_enable_entry_t));
    soc_mem_field32_set(unit, EGR_ENABLEm, entry, PRT_ENABLEf, 1);

    /* Only enable internal ports in EGR_ENABLE.
     *  The packet processing ports are enabled via port enable sequence.
     */
    SOC_PBMP_CLEAR(pbmp);
    SOC_PBMP_PORT_ADD(pbmp, CMIC_PORT(unit));
    SOC_PBMP_PORT_ADD(pbmp, LB_PORT(unit));
    SOC_PBMP_PORT_ADD(pbmp, MACSEC_PORT(unit));

    PBMP_ITER(pbmp, port) {
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_ENABLEm(unit, MEM_BLOCK_ALL, port, entry));
        SOC_IF_ERROR_RETURN(_soc_monterey_mmu_ep_credit_set(unit, port));
    }

    SOC_PBMP_CLEAR(pbmp);
    SOC_PBMP_ASSIGN(pbmp, PBMP_CMIC(unit));
    soc_mem_pbmp_field_set(unit, EPC_LINK_BMAPm, entry, PORT_BITMAPf, &pbmp);
    SOC_IF_ERROR_RETURN(WRITE_EPC_LINK_BMAPm(unit, MEM_BLOCK_ALL, 0, entry));

    /*
     * Enable all ports (except RDB port)
     * RDB port is an ingress-only port that should not have any packets
     * enqueued against it, even if the port is added to any vlan or other
     * forwarding table by mistake.
     */
#if __GNUC__ > 6
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmemset-elt-size"
#endif
    /*
     * We are NOT doing a sal_memset(entry,0,sizeof(entry)) for
     * performance reasons, because the actual soc_mem_field_set()
     * will only write sizeof(ing_dest_port_enable_entry_t)=20
     * bytes while sizeof(entry)=SOC_MAX_MEM_BYTES can be up to 651
     * depending on which devices are compiled in.
     */
    sal_memset(entry, 0, sizeof(ing_dest_port_enable_entry_t));
#if __GNUC__ > 6
#pragma GCC diagnostic pop
#endif
    SOC_PBMP_CLEAR(pbmp);
    SOC_PBMP_ASSIGN(pbmp, PBMP_ALL(unit));
    soc_mem_pbmp_field_set(unit, ING_DEST_PORT_ENABLEm, entry, PORT_BITMAPf, &pbmp);
    SOC_IF_ERROR_RETURN(WRITE_ING_DEST_PORT_ENABLEm(unit, MEM_BLOCK_ALL, 0, entry));

    min_port = SOC_PORT_MIN(unit, all);
    max_port = SOC_PORT_MAX(unit, all);

    fields[0] = DESTf; values[0] = -1; /* -1 => port */
    fields[1] = ENABLEf; values[1] = 1;
    SOC_IF_ERROR_RETURN(soc_mnc_mem_modify_range(unit, MODPORT_MAP_SUBPORTm,
                                                 min_port, max_port,
                                                 2, fields, values));

    /* setting up my_modid */
    SOC_IF_ERROR_RETURN(READ_MY_MODID_SET_2_64r(unit, &rval64));

    soc_reg64_field32_set(unit, MY_MODID_SET_2_64r, &rval64,
                          MODID_0_VALIDf, 1);
    soc_reg64_field32_set(unit, MY_MODID_SET_2_64r, &rval64,
                          MODID_0f, SOC_BASE_MODID(unit));

    SOC_IF_ERROR_RETURN(WRITE_MY_MODID_SET_2_64r(unit, rval64));


    SOC_IF_ERROR_RETURN(READ_ING_CONFIG_64r(unit, &rval64));
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &rval64,
                          L3SRC_HIT_ENABLEf, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &rval64,
                          L2DST_HIT_ENABLEf, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &rval64,
                          APPLY_EGR_MASK_ON_L2f, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &rval64,
                          APPLY_EGR_MASK_ON_L3f, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &rval64,
                          ARP_RARP_TO_FPf, 0x3); /* enable both ARP & RARP */
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &rval64,
                          ARP_VALIDATION_ENf, 1);

    /*
     * If riot is enabled,
     *     MY_STATION1 will be used for tunnels.
     *     MY_STATION2 will be used for routing.
     * Else,
     *     MY_STATION1 should be used for both tunnels and routing.
     *     For this, USE_MY_STATION1_FOR_NON_TUNNELS should be set to TRUE.
     */
    if (!soc_feature(unit, soc_feature_riot)) {
        soc_reg64_field32_set(unit, ING_CONFIG_64r, &rval64,
                USE_MY_STATION1_FOR_NON_TUNNELSf, 1);
    }

    SOC_IF_ERROR_RETURN(WRITE_ING_CONFIG_64r(unit, rval64));

    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, EGR_CONFIG_1r,
                                               REG_PORT_ANY, RING_MODEf, 1));


    /* T2OQ needs to be setup for all ports that require 1 in 9 spacing or less
     *  In Apache, it is safe to program all mmu ports 0-15 in T2OQ (whether in
     *  high speed mode or not). In TD2+, part of the MCQDB memory was re-used
     *  for T2OQ; Apache has separate 160 deep MCQDB memory */
    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, TOQ_MC_CFG1r,
                                               REG_PORT_ANY, IS_MC_T2OQ_PORT0f,
                                               0xffff));

    /* The HW defaults for EGR_VLAN_CONTROL_1.VT_MISS_UNTAG == 1, which
     * causes the outer tag to be removed from packets that don't have
     * a hit in the egress vlan tranlation table. Set to 0 to disable this.
     */
    fields[0] = VT_MISS_UNTAGf; values[0] = 0;
    fields[1] = REMARK_OUTER_DOT1Pf; values[1] = 1;
    SOC_IF_ERROR_RETURN(soc_mnc_mem_modify_range(unit, EGR_VLAN_CONTROL_1m,
                                                 min_port, max_port,
                                                 2, fields, values));
    SOC_PBMP_ASSIGN(pbmp, PBMP_ALL(unit));
    SOC_PBMP_REMOVE(pbmp, PBMP_LB(unit));
    SOC_PBMP_REMOVE(pbmp, PBMP_MACSEC_ALL(unit));
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, ING_EN_EFILTER_BITMAPm,
                                     MEM_BLOCK_ANY, 0, &entry));
    soc_mem_pbmp_field_set(unit, ING_EN_EFILTER_BITMAPm, &entry, BITMAPf, &pbmp);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, ING_EN_EFILTER_BITMAPm,
                                      MEM_BLOCK_ANY, 0, &entry));

    /* Multicast range initialization */
    SOC_IF_ERROR_RETURN
        (soc_hbx_higig2_mcast_sizes_set(unit,
             soc_property_get(unit, spn_HIGIG2_MULTICAST_VLAN_RANGE,
                              SOC_HBX_MULTICAST_RANGE_DEFAULT),
             soc_property_get(unit, spn_HIGIG2_MULTICAST_L2_RANGE,
                              soc_mem_index_count(unit, L2MCm) / 2),
             soc_property_get(unit, spn_HIGIG2_MULTICAST_L3_RANGE,
                              soc_mem_index_count(unit, L2MCm) / 2)));
    SOC_IF_ERROR_RETURN
        (soc_hbx_mcast_size_set(unit, soc_property_get(unit,
             spn_MULTICAST_L2_RANGE, soc_mem_index_count(unit, L2MCm) / 2)));
    SOC_IF_ERROR_RETURN
        (soc_hbx_ipmc_size_set(unit, soc_property_get(unit,
             spn_MULTICAST_L3_RANGE, soc_mem_index_count(unit, L2MCm) / 2)));

    /* Setup SW2_FP_DST_ACTION_CONTROL */
    fields[0] = HGTRUNK_RES_ENf;
    values[0] = 1;
    fields[1] = LAG_RES_ENf;
    values[1] = 1;
    SOC_IF_ERROR_RETURN(soc_reg_fields32_modify(unit, SW2_FP_DST_ACTION_CONTROLr,
                                                REG_PORT_ANY, 2, fields, values));

    /* Populate and enable RTAG7 Macro flow offset table */
    if (soc_mem_is_valid(unit, RTAG7_FLOW_BASED_HASHm)) {
        count = soc_mem_index_max(unit, RTAG7_FLOW_BASED_HASHm);
        sal_memset(entry, 0, sizeof(rtag7_flow_based_hash_entry_t));
        for (index = 0; index < count; ) {
            for (sub_sel = 0; (sub_sel < 8) && (index < count); sub_sel++) {
                for (offset = 0;
                     offset < rtag7_field_width[sub_sel] && index < count;
                     offset++) {
                    soc_mem_field32_set(unit, RTAG7_FLOW_BASED_HASHm, &entry,
                                        SUB_SEL_ECMPf, sub_sel);
                    soc_mem_field32_set(unit, RTAG7_FLOW_BASED_HASHm, &entry,
                                        OFFSET_ECMPf, offset);
#if defined(BCM_RIOT_SUPPORT) || defined(BCM_MULTI_LEVEL_ECMP_SUPPORT)
                    if ((soc_feature(unit, soc_feature_riot) ||
                        soc_feature(unit, soc_feature_multi_level_ecmp))) {

                        soc_mem_field32_set(unit, RTAG7_FLOW_BASED_HASHm, &entry,
                                            SUB_SEL_ECMP_LEVEL1f, sub_sel);
                        soc_mem_field32_set(unit, RTAG7_FLOW_BASED_HASHm, &entry,
                                            OFFSET_ECMP_LEVEL1f, offset);
                    }
#endif
                    SOC_IF_ERROR_RETURN
                        (soc_mem_write(unit, RTAG7_FLOW_BASED_HASHm,
                                       MEM_BLOCK_ANY, index, &entry));
                    index++;
                }
            }
        }

        rval = 0;
        soc_reg_field_set(unit, RTAG7_HASH_SELr, &rval, USE_FLOW_SEL_ECMPf, 1);
#if defined(BCM_RIOT_SUPPORT) || defined(BCM_MULTI_LEVEL_ECMP_SUPPORT)
        if ((soc_feature(unit, soc_feature_riot) ||
            soc_feature(unit, soc_feature_multi_level_ecmp))) {

            soc_reg_field_set(unit, RTAG7_HASH_SELr,
                &rval, USE_FLOW_SEL_ECMP1f, 1);
        }
#endif
        soc_reg_field_set(unit, RTAG7_HASH_SELr, &rval,
                                                USE_FLOW_SEL_TRILL_ECMPf, 1);
        SOC_IF_ERROR_RETURN(WRITE_RTAG7_HASH_SELr(unit, rval));
    }

    freq = si->frequency;

    /*
     * Set external MDIO freq to around 6MHz
     * target_freq = core_clock_freq * DIVIDEND / DIVISOR / 2
     */
    target_freq = 6;
    divisor = (freq + (target_freq * 2 - 1)) / (target_freq * 2);
    divisor = soc_property_get(unit, spn_RATE_EXT_MDIO_DIVISOR, divisor);
    dividend = soc_property_get(unit, spn_RATE_EXT_MDIO_DIVIDEND, 1);
    rval = 0;
    soc_reg_field_set(unit, CMIC_RATE_ADJUSTr, &rval, DIVISORf, divisor);
    soc_reg_field_set(unit, CMIC_RATE_ADJUSTr, &rval, DIVIDENDf, dividend);
    WRITE_CMIC_RATE_ADJUSTr(unit, rval);

    /*
     * Set internal MDIO freq to around 12MHz
     * Valid range is from 2.5MHz to 20MHz
     * target_freq = core_clock_freq * DIVIDEND / DIVISOR / 2
     * or
     * DIVISOR = core_clock_freq * DIVIDENT / (target_freq * 2)
     */
    target_freq = 12;
    divisor = (freq + (target_freq * 2 - 1)) / (target_freq * 2);
    divisor = soc_property_get(unit, spn_RATE_INT_MDIO_DIVISOR, divisor);
    dividend = soc_property_get(unit, spn_RATE_INT_MDIO_DIVIDEND, 1);
    rval = 0;
    soc_reg_field_set (unit, CMIC_RATE_ADJUST_INT_MDIOr, &rval, DIVISORf,
                       divisor);
    soc_reg_field_set (unit, CMIC_RATE_ADJUST_INT_MDIOr, &rval, DIVIDENDf,
                       dividend);
    WRITE_CMIC_RATE_ADJUST_INT_MDIOr(unit, rval);

    delay = soc_property_get(unit, spn_MDIO_OUTPUT_DELAY, -1);
    if (delay >= 1  && delay <= 15) {
        /* coverity[result_independent_of_operands] */
        SOC_IF_ERROR_RETURN(READ_CMIC_MIIM_CONFIGr(unit, &rval));
        soc_reg_field_set(unit, CMIC_MIIM_CONFIGr, &rval, MDIO_OUT_DELAYf,
                          delay);
        /* coverity[result_independent_of_operands] */
        SOC_IF_ERROR_RETURN(WRITE_CMIC_MIIM_CONFIGr(unit, rval));
    }

    if (!SOC_WARM_BOOT(unit)) {
        SOC_IF_ERROR_RETURN(_soc_monterey_ledup_init(unit));
    }

    if (soc_mspi_init(unit) != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "unit %d : MSPI Init Failed\n"), unit));
    }

    if (parity_enable) {
        SOC_IF_ERROR_RETURN(soc_generic_ser_mem_scan_start(unit));
        SOC_IF_ERROR_RETURN(soc_generic_sram_mem_scan_start(unit));
    }

    soc_ser_log_init(unit, NULL, 0);

    /* Setup config needed for EP-Redirection */
    /* Enable re-direction */
    rval = 0;
    soc_reg_field_set(unit, EP_REDIR_CONTROLr, &rval, ENABLEf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, EP_REDIR_CONTROLr,
                                      REG_PORT_ANY, 0, rval));

#ifdef INCLUDE_XFLOW_MACSEC
    if (soc_feature(unit, soc_feature_xflow_macsec)) {
        SOC_IF_ERROR_RETURN(_soc_xflow_macsec_init(unit));
        if (!soc_feature(unit, soc_feature_xflow_macsec_poll_intr)) {
            /* MACSEC has single CMIC bit for both functional and ECC interrupts */
            SOC_IF_ERROR_RETURN(_soc_monterey_macsec_intr_enable(unit, TRUE));
        }
    }
#endif
    return SOC_E_NONE;
}

uint32 _soc_monterey_mmu_port(int unit, int port)
{
    soc_info_t *si = &SOC_INFO(unit);
    return si->port_p2m_mapping[si->port_l2p_mapping[port]];
}

uint32 _soc_monterey_piped_mem_index(int unit, soc_port_t port,
                                     soc_mem_t mem, int arr_off)
{
    int     mmu_port;

    mmu_port = SOC_MONTEREY_MMU_PORT(unit, port);

    switch (mem) {
    case MMU_THDM_DB_PORTSP_CONFIG_0m:
    case MMU_THDM_MCQE_PORTSP_CONFIG_0m:

       return (mmu_port & 0x7f) * _MONTEREY_MMU_NUM_POOL + arr_off;
    case THDI_PORT_SP_CONFIG_Xm:
    case MMU_THDU_XPIPE_CONFIG_PORTm:
    case MMU_THDU_XPIPE_RESUME_PORTm:
        return (mmu_port & 0x7f) * _MONTEREY_MMU_NUM_POOL + arr_off;
    case THDI_PORT_PG_CONFIG_Xm:
        return (mmu_port & 0x7f) * _MONTEREY_MMU_NUM_PG + arr_off;
   default:
        return 0xffffffff;
    }
}

#define _MONTEREY_MMU_BYTES_TO_CELLS(_byte_)  \
    (((_byte_) + _MONTEREY_MMU_BYTES_PER_CELL - 1) / _MONTEREY_MMU_BYTES_PER_CELL)

STATIC void
_soc_monterey_mmu_init_dev_config(int unit, _soc_mmu_device_info_t *devcfg,
                             int lossless)
{
    soc_info_t *si;
    si = &SOC_INFO(unit);

    sal_memset(devcfg, 0, sizeof(_soc_mmu_device_info_t));

    devcfg->max_pkt_byte = si->max_mtu;
    devcfg->mmu_hdr_byte = _MONTEREY_MMU_PACKET_HEADER_BYTES;
    devcfg->jumbo_pkt_size = _MONTEREY_MMU_JUMBO_FRAME_BYTES;
    devcfg->default_mtu_size = _MONTEREY_MMU_DEFAULT_MTU_BYTES;
    devcfg->mmu_cell_size = _MONTEREY_MMU_BYTES_PER_CELL;


    devcfg->mmu_total_cell = _MONTEREY_MMU_TOTAL_CELLS  -
                             (lossless ? 16 : 0);

    devcfg->num_pg = _MONTEREY_MMU_NUM_PG;
    devcfg->num_service_pool = _MONTEREY_MMU_NUM_POOL;
    devcfg->flags = SOC_MMU_CFG_F_PORT_MIN | SOC_MMU_CFG_F_PORT_POOL_MIN |
                    SOC_MMU_CFG_F_RQE | SOC_MMU_CFG_F_EGR_MCQ_ENTRY;
    devcfg->total_mcq_entry = _MONTEREY_MMU_TOTAL_MCQ_ENTRY(unit);
    devcfg->rqe_queue_num = 11;
}

STATIC int
_soc_monterey_default_lossless_pg_headroom(int unit, soc_port_t port)
{
    soc_info_t *si;
    if (IS_ROE_ENABLED(unit)) {
        return 0;
    }
    if (IS_CPU_PORT(unit, port) ) {
        return 50;
    }
    if (IS_LB_PORT(unit, port)) {
        return 162;
    }

    if (IS_MACSEC_PORT(unit, port)) {
        return 117;
    }

    si = &SOC_INFO(unit);
    if (SOC_PBMP_MEMBER(si->oversub_pbm, port)) {
        if (si->port_speed_max[port] >= 100000) {
            return 687;
        } else if (si->port_speed_max[port] >= 50000) {
             return 352;
        } else if (si->port_speed_max[port] >= 40000) {
            return 338;
        } else if (si->port_speed_max[port] >= 25000) {
            return 214;
        } else if (si->port_speed_max[port] >= 20000) {
            return 206;
        } else {
            return 185;
        }
    } else {
        if (si->port_speed_max[port] >= 100000) {
            return 558;
        } else if (si->port_speed_max[port] >= 50000) {
             return 298;
        } else if (si->port_speed_max[port] >= 40000) {
            return 284;
        } else if (si->port_speed_max[port] >= 25000) {
            return 191;
        } else if (si->port_speed_max[port] >= 20000) {
            return 183;
        } else {
            return 162;
        }
    }

    return 0;
}

STATIC int
_soc_monterey_min_cell_rsvd_per_mcq(int unit, int port, int default_val)
{
    int freq, speed, osub;
    soc_info_t *si;
    uint8 rev_id;

    soc_cm_get_id(unit, NULL, &rev_id);

    if (rev_id == BCM56850_A1_REV_ID) {
        si = &SOC_INFO(unit);
        speed = si->port_speed_max[port];
        osub = (SOC_PBMP_MEMBER(si->oversub_pbm, port)) ? 1 : 0;

        if (speed <= 11000) {
            return (osub) ? 10 : 5;
        } else if (speed <= 42000) {
            freq = si->frequency;
            return ((freq >= 760) && !osub) ? 9 : 16;
        }
    }

    return default_val;
}


/*
* Function:
*     soc_monterey_mcq_entries_rsvd_port
* Purpose:
*     For a given port, soc_mn_mcq_entries_rsvd_port calculates
*     the number of MC q entries (Reserved Egress Queue Entries).
*     This function only gives MC q entries for
*     for flex disabled ports.
*     For flex enabled port macros, this function is not used.
* Parameters:
*     unit    - (IN) Unit number.
*     port    - (IN) logical port num
* Returns:
*     Reserved Egress Queue Entries for a given port
 */
int soc_mn_mcq_entries_rsvd_port(int unit, int port)
{
    int port_num_cosq;
    soc_info_t *si;
    int mcq_entry = 0;

    si = &SOC_INFO(unit);
    port_num_cosq = si->port_num_cosq[port];
    mcq_entry = port_num_cosq * _MN_MMU_PER_COSQ_EGRESS_QENTRY_RSVD;
    return mcq_entry;
}

/*
* Function:
*     soc_mn_egr_buf_rsvd_port
* Purpose:
*     For Cpu port or loopback port,
*     soc_td2p_egr_buf_rsvd_port calculates
*     the reserved queue min/guarantee.
*     This function only gives cumulative (for all queues)
*     queue guarantee for CPU port or Loopback port.
* Parameters:
*     unit    - (IN) Unit number.
*     port    - (IN) logical port num for CPU port or LB port
* Returns:
*     Reserved Cumulative (total for all queues) Queue Min/Guarantee value
 */
int soc_mn_egr_buf_rsvd_port(int unit, int port, int default_mtu_cells)
{
    int port_num_cosq;
    soc_info_t *si;
    int egr_rsvd = 0;
    int min_cell_per_mcq;

    if (!IS_CPU_PORT(unit, port) && !IS_LB_PORT(unit, port)) {
        return 0;
    }

    si = &SOC_INFO(unit);

    port_num_cosq = si->port_num_cosq[port];

    if (soc_feature(unit, soc_feature_min_cell_per_queue)) {
        min_cell_per_mcq = _soc_monterey_min_cell_rsvd_per_mcq
                             (unit, port, default_mtu_cells);
    } else {
        min_cell_per_mcq = default_mtu_cells;
    }

    egr_rsvd = port_num_cosq * min_cell_per_mcq;
    return egr_rsvd;
}

STATIC void
_soc_monterey_mmu_config_buf_default(int unit, _soc_mmu_cfg_buf_t *buf,
                                _soc_mmu_device_info_t *devcfg, int lossless)
{
    soc_info_t *si;
    _soc_mmu_cfg_buf_pool_t *buf_pool;
    _soc_mmu_cfg_buf_port_t *buf_port;
    _soc_mmu_cfg_buf_port_pool_t *buf_port_pool;
    _soc_mmu_cfg_buf_prigroup_t *buf_prigroup;
    _soc_mmu_cfg_buf_queue_t *buf_queue;
    _soc_mmu_cfg_buf_mcengine_queue_t *buf_rqe_queue;
    int max_packet_cells, pg_min, default_mtu_cells;
    int port, idx, per_q_guarentee;
    int total_pool_size = 0, egr_shared_total = 0;
    int q_reserved = 0, in_reserved = 0;
    int mcq_entry_reserved = 0;
    int mcq_entry_shared_total;
    int rqe_entry_shared_total;
    _soc_mmu_cfg_buf_qgroup_t *queue_grp;
    int asf_rsvd = 0, ing_rsvd = 0, egr_rsvd = 0;
    int pool0_share = 0 ;
    int pool1_share = 0;
    int mn_mmu_express_buffer_total = _MN_MMU_EXPRESS_BUFFER_TOTAL_1;
    si = &SOC_INFO(unit);
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Initializing default MMU config (u=%d)\n"), unit));
    max_packet_cells = _MMU_CFG_MMU_BYTES_TO_CELLS(devcfg->max_pkt_byte +
                                                   devcfg->mmu_hdr_byte,
                                                   devcfg->mmu_cell_size);
    pg_min = _MONTEREY_MMU_PG_MIN;
    default_mtu_cells = _MMU_CFG_MMU_BYTES_TO_CELLS(devcfg->default_mtu_size +
                                                   devcfg->mmu_hdr_byte,
                                                   devcfg->mmu_cell_size);

    total_pool_size = devcfg->mmu_total_cell;

    buf->headroom = 2 * _MONTEREY_MMU_GLOBAL_HEADROOM_CELL_PER_PIPE;

    in_reserved += (buf->headroom/2);

    per_q_guarentee = (lossless) ? 0 : default_mtu_cells;
    if (asf_rsvd < buf->rsvd_buffers[0].asf_rsvd_cells) {
            asf_rsvd = buf->rsvd_buffers[0].asf_rsvd_cells;
    }
    if (egr_rsvd < buf->rsvd_buffers[0].egr_rsvd_cells) {
        egr_rsvd = buf->rsvd_buffers[0].egr_rsvd_cells;
    }
    if (ing_rsvd < buf->rsvd_buffers[0].ing_rsvd_cells) {
        ing_rsvd = buf->rsvd_buffers[0].ing_rsvd_cells;
    }
    PBMP_ALL_ITER(unit, port) {
        mcq_entry_reserved += si->port_num_cosq[port] *
            _MN_MMU_PER_COSQ_EGRESS_QENTRY_RSVD;
        q_reserved += (si->port_num_cosq[port] + si->port_num_uc_cosq[port]) *
            per_q_guarentee;
    }
    egr_shared_total = total_pool_size - q_reserved - 168;


    mcq_entry_shared_total = _MONTEREY_MMU_TOTAL_MCQ_ENTRY(unit) - mcq_entry_reserved;

    rqe_entry_shared_total = _MONTEREY_MMU_TOTAL_RQE_ENTRY(unit) - 160 - 72;

/* the division needs to be diffrent for ethernet and the other sku supported
 *  based on sku type we should update the share values
 */
    if (IS_ROE_ENABLED(unit)) {
            mn_mmu_express_buffer_total = _MN_MMU_EXPRESS_BUFFER_TOTAL_1;
    } else {
        mn_mmu_express_buffer_total = 0;
    }
    if (IS_ROE_ENABLED(unit)) {
        pool0_share = total_pool_size - mn_mmu_express_buffer_total;
        pool1_share = mn_mmu_express_buffer_total;
    } else {
        pool0_share =  10000 | _MMU_CFG_BUF_PERCENT_FLAG;
        pool1_share = 0;
    }
    for (idx = 0; idx < _MONTEREY_MMU_NUM_POOL; idx++) {
        buf_pool = &buf->pools[idx];

        if (idx == 0) {
            buf_pool->size = pool0_share ;
            buf_pool->yellow_size = pool0_share ;
            buf_pool->red_size = pool0_share ;
            buf_pool->total_mcq_entry = _MONTEREY_MMU_TOTAL_MCQ_ENTRY(unit)  ;
            buf_pool->total_rqe_entry = (rqe_entry_shared_total*45)/100 ;
            buf_pool->mcq_entry_reserved = mcq_entry_reserved;
         }  else if (idx == 1 && IS_ROE_ENABLED(unit)) {
            buf_pool->size = pool1_share ;
            buf_pool->yellow_size = pool1_share ;
            buf_pool->red_size = pool1_share ;
            buf_pool->total_mcq_entry = _MONTEREY_MMU_TOTAL_MCQ_ENTRY(unit) ;
            buf_pool->total_rqe_entry = (rqe_entry_shared_total*54)/100l;
            buf_pool->mcq_entry_reserved =  mcq_entry_reserved;
        } else {
            buf_pool->size = 0;
            buf_pool->yellow_size = 0;
            buf_pool->red_size = 0;
            buf_pool->total_mcq_entry = 0;
            buf_pool->total_rqe_entry = 0;
            buf_pool->mcq_entry_reserved = 0;
        }
    }

    for (idx = 0; idx < SOC_MONTEREY_MMU_CFG_QGROUP_MAX; idx++) {
        queue_grp = &buf->qgroups[idx];
        queue_grp->pool_limit = egr_shared_total;
        if (lossless) {
            queue_grp->guarantee = 0;
            queue_grp->pool_scale = -1;
            queue_grp->discard_enable = 0;
            queue_grp->yellow_limit = 0 | _MMU_CFG_BUF_DYNAMIC_FLAG;
            queue_grp->red_limit = 0 | _MMU_CFG_BUF_DYNAMIC_FLAG;
        } else {
            queue_grp->guarantee = 16;
            queue_grp->discard_enable = 1;
            queue_grp->pool_scale = 8;
        }
        queue_grp->pool_resume = default_mtu_cells * 2;
        queue_grp->yellow_resume = default_mtu_cells * 2;
        queue_grp->red_resume = default_mtu_cells * 2;
    }

    PBMP_ALL_ITER(unit, port) {
        buf_port = &buf->ports[port];

        /* internal priority to priority group mapping */
        for (idx = 0; idx < 16; idx++) {
            buf_port->pri_to_prigroup[idx] = 7;
            if (IS_ROE_ENABLED(unit)) {
                switch (idx) {
                     case 3:
                     buf_port->pri_to_prigroup[idx] = 3;
                     break;
                     case 4:
                     buf_port->pri_to_prigroup[idx] = 1;
                     break;
                     case 5:
                     buf_port->pri_to_prigroup[idx] = 5;
                     break;
                     case 7:
                     buf_port->pri_to_prigroup[idx] = 7;
                     break;
                     default :
                     buf_port->pri_to_prigroup[idx] = 0;
                 }
            }
            if (lossless && IS_RDB_PORT(unit, port)) {
                if (idx < 5) {
                    buf_port->pri_to_prigroup[idx] = idx + 2;
                }
            }
        }

        /* priority group to pool mapping */
        for (idx = 0; idx < _MONTEREY_MMU_NUM_PG; idx++) {
             if (IS_CPRI_PORT(unit,port) && (idx == 6 || idx == 7 || idx == 0)) {
                 buf_port->prigroups[idx].pool_idx = 1;
             } else if (IS_RSVD4_PORT(unit,port) && (idx != 1)) {
                 buf_port->prigroups[idx].pool_idx = 1;
             }else  if(!IS_CPRI_PORT(unit,port) && !IS_RSVD4_PORT(unit,port) &&
                            IS_ROE_ENABLED(unit) && (idx == 5 || idx == 7)) {
                 buf_port->prigroups[idx].pool_idx = 1;
             } else {
                 buf_port->prigroups[idx].pool_idx = 0;
             }
        }

        for (idx = 0; idx < _MONTEREY_MMU_NUM_POOL; idx++) {
            buf_port_pool = &buf_port->pools[idx];
            buf_port_pool->guarantee = 0;
            buf_port_pool->pool_limit = 0;
            buf_port_pool->pool_resume = 0;
            if (idx == 0) {
                buf_port_pool->pool_limit = total_pool_size -  mn_mmu_express_buffer_total;
                buf_port_pool->pool_resume =
                            total_pool_size - (default_mtu_cells * 2) - mn_mmu_express_buffer_total;
            } else if (IS_ROE_ENABLED(unit) && (idx == 1)) {
                buf_port_pool->pool_limit =  mn_mmu_express_buffer_total;
                buf_port_pool->pool_resume =
                            mn_mmu_express_buffer_total - (default_mtu_cells * 2) ;
            }
            in_reserved += buf_port_pool->guarantee;
        }

        buf_port->pkt_size = max_packet_cells;
        /* priority group */
        for (idx = 0; idx < _MONTEREY_MMU_NUM_PG; idx++) {
            buf_prigroup = &buf_port->prigroups[idx];
            buf_prigroup->guarantee = 0;
            buf_prigroup->user_delay = -1;
            buf_prigroup->switch_delay = -1;
            buf_prigroup->pkt_size = max_packet_cells;
            buf_prigroup->device_headroom_enable = 0;
            buf_prigroup->pool_limit = 0;
            buf_prigroup->pool_floor = 0;
            buf_prigroup->pool_scale = -1;
            buf_prigroup->headroom = 0;
            buf_prigroup->pool_resume = 0;
            buf_prigroup->flow_control_enable = 0;
            if (idx == 7) {
                buf_prigroup->device_headroom_enable = 1;
                buf_prigroup->flow_control_enable = lossless;
                if (lossless) {
                    buf_prigroup->headroom =
                        _soc_monterey_default_lossless_pg_headroom(unit, port);
                    buf_prigroup->guarantee = pg_min;
                    buf_prigroup->pool_scale = 8;
                    buf_prigroup->pool_resume = default_mtu_cells * 2;
                }
                if (!lossless && IS_CPU_PORT(unit, port)) {
                    buf_prigroup->headroom =
                        _soc_monterey_default_lossless_pg_headroom(unit, port);
                }
            } else {
                buf_prigroup->device_headroom_enable = 1;
            }
            in_reserved += buf_prigroup->guarantee + buf_prigroup->headroom;
        }

        /* multicast queue */
        for (idx = 0; idx < si->port_num_cosq[port]; idx++) {
            buf_queue = &buf_port->queues[idx];
             if (IS_CPRI_PORT(unit,port) && (idx == 6 || idx == 7 || idx == 0)) {
                buf_queue->qgroup_id = ((1 <= port) && (port <= 64)) ? 2 : 1;
             } else if (IS_RSVD4_PORT(unit,port) && (idx != 1)) {
                buf_queue->qgroup_id = ((1 <= port) && (port <= 64)) ? 2 : 1;
             } else {
               buf_queue->qgroup_id = ((1 <= port) && (port <= 64)) ? 0 : -1;
            }
            buf_queue->mcq_entry_guarantee = 0;
            if (lossless) {
                buf_queue->guarantee = (((1 <= port) && (port <= 64)) ||
                                        IS_CPU_PORT(unit, port) ||
                                        IS_LB_PORT(unit, port)) ? 0 : default_mtu_cells;
                buf_queue->discard_enable = IS_MACSEC_PORT(unit, port) ? 1 : 0;
                buf_queue->pool_scale = IS_MACSEC_PORT(unit, port) ? 8 : -1;
                buf_queue->pool_limit = egr_shared_total;
                buf_queue->mcqe_limit = mcq_entry_shared_total;
                buf_queue->yellow_limit = mcq_entry_shared_total;
                buf_queue->red_limit = mcq_entry_shared_total;
                buf_queue->color_discard_enable = 0;
                buf_queue->pool_resume = 16;
            } else {
                buf_queue->guarantee = ((1 <= port) && (port <= 64)) ? 0 : default_mtu_cells;
                if (IS_CPRI_PORT(unit,port) && (idx !=6 && idx !=7 && idx !=0)) {
                    buf_queue->guarantee = 8;

                } else if (IS_RSVD4_PORT(unit,port) && (idx != 1)) {
                    buf_queue->guarantee = 8;

                } else if (!IS_CPRI_PORT(unit,port) && !IS_RSVD4_PORT(unit,port) &&
                        IS_ROE_ENABLED(unit) && (idx == 1 || idx == 3)) {
                    buf_queue->guarantee = 8;
                }
                buf_queue->discard_enable = 1;
                if (IS_CPRI_PORT(unit,port) && (idx == 6 || idx == 7 || idx == 0)) {
                    buf_queue->discard_enable = 0;

                } else if (IS_RSVD4_PORT(unit,port) && (idx != 1)) {
                    buf_queue->discard_enable = 0;

                } else if (!IS_CPRI_PORT(unit,port) && !IS_RSVD4_PORT(unit,port) &&
                        IS_ROE_ENABLED(unit) && (idx == 5 || idx == 7)) {
                    buf_queue->discard_enable = 0;
                }
                buf_queue->pool_scale = 8;
                buf_queue->pool_limit = 0;
                buf_queue->yellow_limit = 0 | _MMU_CFG_BUF_DYNAMIC_FLAG
                                            | _MMU_CFG_BUF_DYNAMIC_MAPPING_TYPE_1;
                buf_queue->red_limit = 0 | _MMU_CFG_BUF_DYNAMIC_FLAG
                                         | _MMU_CFG_BUF_DYNAMIC_MAPPING_TYPE_1;
                buf_queue->color_discard_enable = 0;
                buf_queue->pool_resume = 16;

           }
        }

        /* unicast queue */
        for (idx = 0; idx < si->port_num_uc_cosq[port]; idx++) {
            buf_queue = &buf_port->queues[si->port_num_cosq[port] + idx];
             if (IS_CPRI_PORT(unit,port) && (idx == 6 || idx == 7 || idx == 0)) {
                buf_queue->qgroup_id = ((1 <= port) && (port <= 64)) ? 2 : 1;
             } else if (IS_RSVD4_PORT(unit,port) && (idx != 1)) {
                buf_queue->qgroup_id = ((1 <= port) && (port <= 64)) ? 2 : 1;
             } else if (!IS_CPRI_PORT(unit,port) && !IS_RSVD4_PORT(unit,port) &&
                        IS_ROE_ENABLED(unit) && (idx == 5 || idx == 7)) {
                buf_queue->qgroup_id = ((1 <= port) && (port <= 64)) ? 2 : 1;
            }else {
                buf_queue->qgroup_id = ((1 <= port) && (port <= 64)) ? 0 : 1;
            }
            if (lossless) {
                buf_queue->guarantee = 0;
                buf_queue->discard_enable = 0;
                buf_queue->pool_scale = -1;
                buf_queue->pool_limit = egr_shared_total ;
                buf_queue->yellow_limit = egr_shared_total;
                buf_queue->red_limit = egr_shared_total;
                buf_queue->color_discard_enable = 0;
                buf_queue->pool_resume = default_mtu_cells * 2;
                buf_queue->yellow_resume = default_mtu_cells * 2;
                buf_queue->red_resume = default_mtu_cells * 2;
            } else {

                if (IS_CPRI_PORT(unit,port) && (idx != 6 && idx != 7 && idx != 0)) {
                    buf_queue->guarantee = ((1 <= port) && (port <= 64)) ? 8 : 0;
                 } else if (IS_RSVD4_PORT(unit,port) && (idx == 1)) {
                    buf_queue->guarantee = ((1 <= port) && (port <= 64)) ? 8 : 0;
                } else if (!IS_CPRI_PORT(unit,port) && !IS_RSVD4_PORT(unit,port) &&
                            IS_ROE_ENABLED(unit) && (idx == 1 || idx == 3))  {
                    buf_queue->guarantee = ((1 <= port) && (port <= 64)) ? 8 : 0;
                } else {
                    buf_queue->guarantee = ((1 <= port) && (port <= 64)) ? 0 : default_mtu_cells;
                }
                if (IS_CPRI_PORT(unit,port) && (idx != 6 && idx != 7 && idx != 0)) {
                    buf_queue->discard_enable = 1;
                } else if (IS_RSVD4_PORT(unit,port) && (idx == 1)) {
                    buf_queue->discard_enable = 1;
                } else if (!IS_CPRI_PORT(unit,port) && !IS_RSVD4_PORT(unit,port)) {
                    buf_queue->discard_enable = 1;
                } else {
                    buf_queue->discard_enable = 0;
                }

                buf_queue->pool_scale = 8;
                buf_queue->pool_limit = 0;
                buf_queue->yellow_limit = 0 | _MMU_CFG_BUF_DYNAMIC_FLAG
                                            | _MMU_CFG_BUF_DYNAMIC_MAPPING_TYPE_1;
                buf_queue->red_limit = 0 | _MMU_CFG_BUF_DYNAMIC_FLAG
                                         | _MMU_CFG_BUF_DYNAMIC_MAPPING_TYPE_1;
                buf_queue->color_discard_enable = 0;
                buf_queue->pool_resume = default_mtu_cells * 2;
                buf_queue->yellow_resume = default_mtu_cells * 2;
                buf_queue->red_resume = default_mtu_cells * 2;
            }
        }

        /* queue to pool mapping */
        for (idx = 0;
             idx < si->port_num_cosq[port] + si->port_num_uc_cosq[port]; idx++) {
            if (IS_CPRI_PORT(unit,port) && (idx == 0 || idx ==6 || idx ==7 ||
                idx == 10 || idx == 16 || idx == 17)) {
                buf_port->queues[idx].pool_idx = 1;
            } else if (IS_RSVD4_PORT(unit,port) && (idx != 1  && idx != 11))  {
                buf_port->queues[idx].pool_idx = 1;
            } else if(!IS_CPRI_PORT(unit,port) && !IS_RSVD4_PORT(unit,port) &&
                      IS_ROE_ENABLED(unit) && ((idx == 15 || idx == 17)
               && !IS_CPU_PORT(unit, port) && !IS_MACSEC_PORT(unit,port))) {
               buf_port->queues[idx].pool_idx = 1;
            } else  {
               buf_port->queues[idx].pool_idx = 0;
            }
        }
    }

    PBMP_ALL_ITER(unit, port) {
        buf_port = &buf->ports[port];
        for (idx = 0; idx < _MONTEREY_MMU_NUM_PG; idx++) {
             buf_prigroup = &buf_port->prigroups[idx];
             if (!lossless) {
                 if (IS_CPRI_PORT(unit,port) && (idx == 0 || idx == 6 || idx ==7 )) {
                       buf_prigroup->pool_limit = mn_mmu_express_buffer_total;
                 } else if (IS_RSVD4_PORT(unit,port) && (idx != 1))  {
                       buf_prigroup->pool_limit = mn_mmu_express_buffer_total;
                }  else if (!IS_CPRI_PORT(unit,port) && !IS_RSVD4_PORT(unit,port) &&
                            IS_ROE_ENABLED(unit) && (idx == 5 || idx ==7))
                       buf_prigroup->pool_limit = mn_mmu_express_buffer_total;
                 else {
                       buf_prigroup->pool_limit = total_pool_size - in_reserved -
                                           mn_mmu_express_buffer_total - 16;
                 }
             }
       }
    }

    /* RQE */
    for (idx = 0; idx < 11; idx++) {
        buf_rqe_queue = &buf->rqe_queues[idx];
        if ( idx == 9 || idx == 10 ) {
            buf_rqe_queue->pool_idx = 1;
        } else {
            buf_rqe_queue->pool_idx = 0;
        }
        buf_rqe_queue->yellow_limit = egr_shared_total;
        buf_rqe_queue->red_limit = egr_shared_total;
        if (lossless) {
            buf_rqe_queue->guarantee = 8;
            buf_rqe_queue->discard_enable = 0;
            buf_rqe_queue->pool_scale = -1;
            buf_rqe_queue->pool_limit = egr_shared_total;
        } else {

            if ( idx == 9 || idx == 10 ) {
                buf_rqe_queue->discard_enable = 0;
                buf_rqe_queue->guarantee = 0;
            } else {
                buf_rqe_queue->guarantee = default_mtu_cells;
                buf_rqe_queue->discard_enable = 1;
            }
            buf_rqe_queue->pool_scale = 8;
            buf_rqe_queue->pool_limit = 0;
            buf_rqe_queue->yellow_limit = 0 | _MMU_CFG_BUF_DYNAMIC_FLAG;
            buf_rqe_queue->red_limit = 0 | _MMU_CFG_BUF_DYNAMIC_FLAG;
        }
    }

}

STATIC int
_soc_monterey_pool_scale_to_limit(int size, int scale)
{
    int factor = 1000;

    switch (scale) {
        case 7: factor = 875; break;
        case 6: factor = 750; break;
        case 5: factor = 625; break;
        case 4: factor = 500; break;
        case 3: factor = 375; break;
        case 2: factor = 250; break;
        case 1: factor = 125; break;
        case 0:
        default:
            factor = 1000; break;
    }
    return (size * factor)/1000;
}
STATIC int
_soc_monterey_mmu_config_buf_set_hw(int unit, _soc_mmu_cfg_buf_t *buf,
                               _soc_mmu_device_info_t *devcfg, int lossless,
                               soc_port_t input_port)
{
    soc_info_t *si;
    _soc_mmu_cfg_buf_pool_t *buf_pool;
    _soc_mmu_cfg_buf_port_t *buf_port;
    _soc_mmu_cfg_buf_prigroup_t *buf_prigroup;
    _soc_mmu_cfg_buf_queue_t *buf_queue;
    _soc_mmu_cfg_buf_port_pool_t *buf_port_pool;
    _soc_mmu_cfg_buf_mcengine_queue_t *buf_rqe_queue;
    soc_mem_t mem, mem1, mem2, mem3;
    uint32 rval, fval, rval2, rval3;
    uint32 entry0[SOC_MAX_MEM_WORDS], entry1[SOC_MAX_MEM_WORDS];
    thdi_port_pg_config_entry_t pg_config_mem;
    thdi_port_sp_config_entry_t thdi_sp_config;
    mmu_thdo_config_qgroup_entry_t cfg_qgrp;
    mmu_thdu_xpipe_offset_qgroup_entry_t offset_qgrp;
    int default_mtu_cells, limit, midx, pri, rlimit, min_resume_limit;
    int port, base, numq, idx;
    int jumbo_frame_cells, pval, rqlen, qbase;
    _soc_mmu_cfg_buf_qgroup_t *queue_grp;
    int port_count = 0;
    soc_reg_t reg = INVALIDr;
    static const soc_field_t prigroup_reg[] = {
        THDI_PORT_PRI_GRP0r, THDI_PORT_PRI_GRP1r
    };
    static const soc_field_t prigroup_field[] = {
        PRI0_GRPf, PRI1_GRPf, PRI2_GRPf, PRI3_GRPf,
        PRI4_GRPf, PRI5_GRPf, PRI6_GRPf, PRI7_GRPf,
        PRI8_GRPf, PRI9_GRPf, PRI10_GRPf, PRI11_GRPf,
        PRI12_GRPf, PRI13_GRPf, PRI14_GRPf, PRI15_GRPf
    };
    static const soc_field_t prigroup_spid_field[] = {
        PG0_SPIDf, PG1_SPIDf, PG2_SPIDf, PG3_SPIDf,
        PG4_SPIDf, PG5_SPIDf, PG6_SPIDf, PG7_SPIDf
    };
    int index1;
    int pool_resume = 0;
    uint16      dev_id;
    uint8       rev_id;
    int         ing_rsvd = 0, egr_rsvd = 0, asf_rsvd = 0;
    int total_rsvd = 0 ;
    int  cpu_hdrm = 0;
    int jumbo_packet_cells = 45;
    soc_cm_get_id(unit, &dev_id, &rev_id);

    min_resume_limit = soc_feature(unit, soc_feature_min_resume_limit_1) ? 8 : 0;

    si = &SOC_INFO(unit);

    jumbo_frame_cells = _MMU_CFG_MMU_BYTES_TO_CELLS(devcfg->jumbo_pkt_size +
                                                   devcfg->mmu_hdr_byte,
                                                   devcfg->mmu_cell_size);
    default_mtu_cells = _MMU_CFG_MMU_BYTES_TO_CELLS(devcfg->default_mtu_size +
                                                   devcfg->mmu_hdr_byte,
                                                   devcfg->mmu_cell_size);

    idx = 0 ;
    rval = 0;
    fval = _MONTEREY_CFAPFULLSETPOINT;
    if (input_port == -1) {
        soc_reg_field_set(unit, CFAPFULLSETPOINTr, &rval, CFAPFULLSETPOINTf,
                fval);
        SOC_IF_ERROR_RETURN(WRITE_CFAPFULLSETPOINTr(unit, rval));
        rval = 0;
        soc_reg_field_set(unit, CFAPFULLRESETPOINTr, &rval,
                CFAPFULLRESETPOINTf, fval - 2 * jumbo_frame_cells);
        SOC_IF_ERROR_RETURN(WRITE_CFAPFULLRESETPOINTr(unit, rval));
        rval = 0;
        SOC_IF_ERROR_RETURN(WRITE_TOQ_MC_CFG0r(unit, rval));
    }
    if (asf_rsvd < buf->rsvd_buffers[idx].asf_rsvd_cells) {
            asf_rsvd = buf->rsvd_buffers[idx].asf_rsvd_cells;
    }
    if (egr_rsvd < buf->rsvd_buffers[idx].egr_rsvd_cells) {
           egr_rsvd = buf->rsvd_buffers[idx].egr_rsvd_cells;
    }
    if (ing_rsvd < buf->rsvd_buffers[idx].ing_rsvd_cells) {
            ing_rsvd = buf->rsvd_buffers[idx].ing_rsvd_cells;
    }
    LOG_VERBOSE(BSL_LS_SOC_MMU,
                (BSL_META_U(unit,
                            "MMU config set HW: Cells rsvd : %d\n"),
                            (ing_rsvd + egr_rsvd + asf_rsvd)));

    /* internal priority to priority group mapping */
    PBMP_ALL_ITER(unit, port) {
        if ((input_port != port) &&
                (input_port != -1)) {
            continue;
        }
        buf_port = &buf->ports[port];

        for (idx = 0; idx < 16; idx++) {
            if (idx % 8 == 0) { /* 8 fields per register */
                reg = prigroup_reg[idx / 8];
                rval = 0;
            }
            soc_reg_field_set(unit, reg, &rval, prigroup_field[idx],
                              buf_port->pri_to_prigroup[idx]);
            if (idx % 8 == 7) { /* 8 fields per register */
                SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, rval));
            }
        }
    }
    if (input_port == -1) {
        /* Input thresholds */
        rval = 0;
        soc_reg_field_set(unit, THDI_GLOBAL_HDRM_LIMIT_PIPEXr,
                &rval, GLOBAL_HDRM_LIMITf, buf->headroom/2);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, THDI_GLOBAL_HDRM_LIMIT_PIPEXr,
                    REG_PORT_ANY, 0, rval));


        fval = 0;
        for (idx = 0; idx < _MONTEREY_MMU_NUM_POOL; idx++) {
            if ((buf->pools[idx].size & ~_MMU_CFG_BUF_PERCENT_FLAG) != 0) {
                fval |= 1 << idx;
            }
        }

        rval = 0;
        soc_reg_field_set(unit, THDI_BYPASSr, &rval, INPUT_THRESHOLD_BYPASSf, 0);
        SOC_IF_ERROR_RETURN(WRITE_THDI_BYPASSr(unit, rval));


        rval = 0;
        soc_reg_field_set(unit, THDI_POOL_CONFIGr, &rval, COLOR_AWAREf, 0);
        soc_reg_field_set(unit, THDI_POOL_CONFIGr, &rval, PUBLIC_ENABLEf, 1);
        SOC_IF_ERROR_RETURN(WRITE_THDI_POOL_CONFIGr(unit, rval));

        for (idx = 0; idx < _MONTEREY_MMU_NUM_POOL; idx++) {
            buf_pool = &buf->pools[idx];
            if ((buf_pool->size & ~_MMU_CFG_BUF_PERCENT_FLAG) == 0) {
                continue;
            }
            ing_rsvd = buf_pool->prigroup_headroom + buf_pool->prigroup_guarantee;


            if (lossless) {
                limit = devcfg->mmu_total_cell - (ing_rsvd+
                        (buf->headroom/2) +
                        buf_pool->queue_guarantee + asf_rsvd);
            } else {
                    cpu_hdrm = _soc_monterey_default_lossless_pg_headroom(
                            unit,
                            CMIC_PORT(unit));
                if(idx == 0) {
                   if (IS_ROE_ENABLED(unit)) {
                       cpu_hdrm = 0 ;
                   }
                   limit = buf_pool->total -
                   (buf->headroom/2) -  cpu_hdrm - asf_rsvd  - 16 ;
                } else {
                   limit = buf_pool->total;
                }
            }

            PBMP_ALL_ITER(unit, port) {
                port_count++;
            }
            port_count -= 3; /*macsec, cpu and lb */
            if (IS_ROE_ENABLED(unit)) {
                pool_resume = jumbo_packet_cells * 2;
            } else {
                pool_resume = (port_count / 2) *  default_mtu_cells;
            }
            rval = 0;
            soc_reg_field_set(unit, THDI_BUFFER_CELL_LIMIT_SPr, &rval, LIMITf, limit);
            SOC_IF_ERROR_RETURN(WRITE_THDI_BUFFER_CELL_LIMIT_SPr(unit, idx, rval));

            rval = 0;
            soc_reg_field_set(unit, THDI_CELL_RESET_LIMIT_OFFSET_SPr, &rval,
                    OFFSETf, pool_resume);
            SOC_IF_ERROR_RETURN(WRITE_THDI_CELL_RESET_LIMIT_OFFSET_SPr(unit, idx, rval));
        }

        rval = 0;
        SOC_IF_ERROR_RETURN(WRITE_THDI_BUFFER_CELL_LIMIT_PUBLIC_POOLr(unit, rval));

        /* output thresholds */
        SOC_IF_ERROR_RETURN(READ_OP_THDU_CONFIGr(unit, &rval));
        soc_reg_field_set(unit, OP_THDU_CONFIGr, &rval, ENABLE_QUEUE_AND_GROUP_TICKETf, 1);
        soc_reg_field_set(unit, OP_THDU_CONFIGr, &rval, ENABLE_UPDATE_COLOR_RESUMEf, 1);
        soc_reg_field_set(unit, OP_THDU_CONFIGr, &rval, MOP_POLICY_1Bf, 1);
        soc_reg_field_set(unit, OP_THDU_CONFIGr, &rval, MOP_POLICY_1Af, 0);
        SOC_IF_ERROR_RETURN(WRITE_OP_THDU_CONFIGr(unit, rval));

        SOC_IF_ERROR_RETURN(READ_MMU_THDM_DB_DEVICE_THR_CONFIGr(unit, &rval));
        soc_reg_field_set(unit, MMU_THDM_DB_DEVICE_THR_CONFIGr, &rval, MOP_POLICYf, 1);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_DB_DEVICE_THR_CONFIGr(unit, rval));

        /* per service pool settings */
        for (idx = 0; idx < _MONTEREY_MMU_NUM_POOL; idx++) {
            buf_pool = &buf->pools[idx];
            if ((buf_pool->size & ~_MMU_CFG_BUF_PERCENT_FLAG) == 0) {
                continue;
            }
            total_rsvd = buf_pool->queue_guarantee  ;
            if (idx == 0) {
                limit = buf_pool->total - (total_rsvd + asf_rsvd +
                                           (IS_ROE_ENABLED(unit) ? 0 : 16));
             } else {
                limit = buf_pool->total ;
             }
            if (limit <= 0) {
                limit = 0;
            }

            rval = 0;
            soc_reg_field_set(unit, MMU_THDM_DB_POOL_SHARED_LIMITr, &rval,
                    SHARED_LIMITf, limit);
            SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_DB_POOL_SHARED_LIMITr(unit, idx, rval));

            rval = 0;
            soc_reg_field_set(unit, MMU_THDM_DB_POOL_YELLOW_SHARED_LIMITr,
                    &rval, YELLOW_SHARED_LIMITf, (limit + 7)/8);
            SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_DB_POOL_YELLOW_SHARED_LIMITr(unit, idx, rval));

            rval = 0;
            soc_reg_field_set(unit, MMU_THDM_DB_POOL_RED_SHARED_LIMITr,
                    &rval, RED_SHARED_LIMITf, (limit + 7)/8);
            SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_DB_POOL_RED_SHARED_LIMITr(unit, idx, rval));

            rval = 0;
            soc_reg_field_set(unit, MMU_THDM_DB_POOL_RESUME_LIMITr,
                    &rval, RESUME_LIMITf, (limit + 7) /8);
            SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_DB_POOL_RESUME_LIMITr(unit, idx, rval));

            rval = 0;
            soc_reg_field_set(unit, MMU_THDM_DB_POOL_YELLOW_RESUME_LIMITr,
                    &rval, YELLOW_RESUME_LIMITf, (limit + 7)/8);
            SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_DB_POOL_YELLOW_RESUME_LIMITr(unit, idx, rval));

            rval = 0;
            soc_reg_field_set(unit, MMU_THDM_DB_POOL_RED_RESUME_LIMITr,
                    &rval, RED_RESUME_LIMITf, (limit + 7)/8);
            SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_DB_POOL_RED_RESUME_LIMITr(unit, idx, rval));

            total_rsvd = 0 ;
            total_rsvd = buf_pool->mcq_entry_reserved;

            /* mcq entries */
            limit = buf_pool->total_mcq_entry - total_rsvd;

            rval = 0;
            soc_reg_field_set(unit, MMU_THDM_MCQE_POOL_SHARED_LIMITr,
                    &rval, SHARED_LIMITf, limit/4);
            SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_MCQE_POOL_SHARED_LIMITr(unit,
                        idx, rval));

            rval = 0;
            soc_reg_field_set(unit, MMU_THDM_MCQE_POOL_YELLOW_SHARED_LIMITr,
                    &rval, YELLOW_SHARED_LIMITf, (limit + 7)/ 8);
            SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_MCQE_POOL_YELLOW_SHARED_LIMITr(unit,
                        idx, rval));

            rval = 0;
            soc_reg_field_set(unit, MMU_THDM_MCQE_POOL_RED_SHARED_LIMITr,
                    &rval, RED_SHARED_LIMITf, (limit + 7)/8);
            SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_MCQE_POOL_RED_SHARED_LIMITr(unit,
                        idx, rval));

            rval = 0;
            soc_reg_field_set(unit, MMU_THDM_MCQE_POOL_RESUME_LIMITr,
                    &rval, RESUME_LIMITf, (limit + 7)/8);
            SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_MCQE_POOL_RESUME_LIMITr(unit,
                        idx, rval));

            rval = 0;
            soc_reg_field_set(unit, MMU_THDM_MCQE_POOL_YELLOW_RESUME_LIMITr,
                    &rval, YELLOW_RESUME_LIMITf, (limit + 7)/8);
            SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_MCQE_POOL_YELLOW_RESUME_LIMITr(unit,
                        idx, rval));

            rval = 0;
            soc_reg_field_set(unit, MMU_THDM_MCQE_POOL_RED_RESUME_LIMITr,
                    &rval, RED_RESUME_LIMITf, (limit + 7)/8);
            SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_MCQE_POOL_RED_RESUME_LIMITr(unit,
                        idx, rval));
        }

        rval = 0;
        soc_reg_field_set(unit, MMU_THDR_DB_CONFIGr, &rval,
                CLEAR_DROP_STATE_ON_CONFIG_UPDATEf, 1);
        soc_reg_field_set(unit, MMU_THDR_DB_CONFIGr, &rval, MOP_POLICY_1Bf, 1);
        soc_reg_field_set(unit, MMU_THDR_DB_CONFIGr, &rval, MOP_POLICY_1Af, 0);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_DB_CONFIGr(unit, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_THDR_QE_CONFIGr, &rval,
                CLEAR_DROP_STATE_ON_CONFIG_UPDATEf, 1);
        SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_QE_CONFIGr(unit, rval));

        /* configure Q-groups */
        for (idx = 0; idx < SOC_MONTEREY_MMU_CFG_QGROUP_MAX; idx++) {
            queue_grp = &buf->qgroups[idx];

            mem = MMU_THDU_XPIPE_CONFIG_QGROUPm;
            mem1 = MMU_THDU_XPIPE_OFFSET_QGROUPm;

            sal_memset(&cfg_qgrp, 0, sizeof(cfg_qgrp));
            sal_memset(&offset_qgrp, 0, sizeof(offset_qgrp));

            soc_mem_field32_set(unit, mem, &cfg_qgrp,
                    Q_MIN_LIMIT_CELLf, queue_grp->guarantee);

            if (queue_grp->pool_scale != -1) {
                soc_mem_field32_set(unit, mem, &cfg_qgrp,
                        Q_SHARED_ALPHA_CELLf, queue_grp->pool_scale);
                soc_mem_field32_set(unit, mem, &cfg_qgrp,
                        Q_LIMIT_DYNAMIC_CELLf, 1);
            } else {
                soc_mem_field32_set(unit, mem, &cfg_qgrp,
                        Q_SHARED_LIMIT_CELLf, queue_grp->pool_limit);
            }

            if (queue_grp->red_limit & _MMU_CFG_BUF_DYNAMIC_FLAG) {
                pval = _soc_monterey_pool_scale_to_limit(queue_grp->pool_limit,
                        queue_grp->red_limit & _MMU_CFG_BUF_DYNAMIC_FLAG);
                soc_mem_field32_set(unit, mem, &cfg_qgrp,
                        LIMIT_RED_CELLf, (pval + 7)/8);

            } else {
                soc_mem_field32_set(unit, mem, &cfg_qgrp,
                        LIMIT_RED_CELLf, queue_grp->red_limit);
                soc_mem_field32_set(unit, mem, &cfg_qgrp,
                        Q_COLOR_LIMIT_DYNAMIC_CELLf, 1);
            }

            if (queue_grp->yellow_limit & _MMU_CFG_BUF_DYNAMIC_FLAG) {
                pval = _soc_monterey_pool_scale_to_limit(queue_grp->pool_limit,
                        queue_grp->yellow_limit & _MMU_CFG_BUF_DYNAMIC_FLAG);
                soc_mem_field32_set(unit, mem, &cfg_qgrp,
                        LIMIT_YELLOW_CELLf, (pval + 7)/8);

            } else {
                soc_mem_field32_set(unit, mem, &cfg_qgrp,
                        LIMIT_YELLOW_CELLf, queue_grp->yellow_limit);
                soc_mem_field32_set(unit, mem, &cfg_qgrp,
                        Q_COLOR_LIMIT_DYNAMIC_CELLf, 1);
            }
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem, MEM_BLOCK_ALL, idx, &cfg_qgrp));

            soc_mem_field32_set(unit, mem1,
                    &offset_qgrp, RESET_OFFSET_CELLf, queue_grp->pool_resume / 8);
            soc_mem_field32_set(unit, mem1, &offset_qgrp , RESET_OFFSET_YELLOW_CELLf,
                    queue_grp->yellow_resume / 8);
            soc_mem_field32_set(unit, mem1, &offset_qgrp , RESET_OFFSET_RED_CELLf,
                    queue_grp->red_resume / 8);

            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem1, MEM_BLOCK_ALL, idx, &offset_qgrp));
        }
    }

    /* Input port per port settings */
    PBMP_ALL_ITER(unit, port) {
        if ((input_port != port) &&
                (input_port != -1)) {
            continue;
        }

        buf_port = &buf->ports[port];

        rval = 0;
        for (idx = 0; idx < _MONTEREY_MMU_NUM_PG; idx++) {
            soc_reg_field_set(unit, THDI_PORT_PG_SPIDr, &rval,
                              prigroup_spid_field[idx],
                              buf_port->prigroups[idx].pool_idx);
        }
        SOC_IF_ERROR_RETURN(WRITE_THDI_PORT_PG_SPIDr(unit, port, rval));

        mem = THDI_PORT_SP_CONFIG_Xm;
        /* Per port per pool settings */
        for (idx = 0; idx < _MONTEREY_MMU_NUM_POOL; idx++) {
            buf_port_pool = &buf_port->pools[idx];
            midx = SOC_MONTEREY_MMU_PIPED_MEM_INDEX(unit, port, mem, idx);
            sal_memset(&thdi_sp_config, 0, sizeof(thdi_sp_config));
            soc_mem_field32_set(unit, mem, &thdi_sp_config,
                                PORT_SP_MIN_LIMITf, buf_port_pool->guarantee);
            soc_mem_field32_set(unit, mem, &thdi_sp_config,
                           PORT_SP_RESUME_LIMITf, buf_port_pool->pool_resume);
            soc_mem_field32_set(unit, mem, &thdi_sp_config,
                               PORT_SP_MAX_LIMITf, buf_port_pool->pool_limit);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem, MEM_BLOCK_ALL, midx, &thdi_sp_config));
        }

        fval = 0;
        for (idx = 0; idx < _MONTEREY_MMU_NUM_PG; idx++) {
            if (buf_port->prigroups[idx].flow_control_enable != 0) {
                for (pri=0; pri < 16; pri++) {
                    if (buf_port->pri_to_prigroup[pri] == idx) {
                        fval |= 1 << pri;
                    }
                }
            }
        }

        rval = 0;
        soc_reg_field_set(unit, THDI_INPUT_PORT_XON_ENABLESr, &rval,
                                          INPUT_PORT_RX_ENABLEf, 1);
        /* For CPU and MACSEC port we don't configure pause enable */
        if (IS_CPU_PORT(unit, port) || IS_MACSEC_PORT(unit, port)) {
            fval = 0;
        }
        soc_reg_field_set(unit, THDI_INPUT_PORT_XON_ENABLESr, &rval,
                          PORT_PRI_XON_ENABLEf, fval);
        soc_reg_field_set(unit, THDI_INPUT_PORT_XON_ENABLESr, &rval,
                          PORT_PAUSE_ENABLEf, fval ? 1 : 0);
        SOC_IF_ERROR_RETURN(WRITE_THDI_INPUT_PORT_XON_ENABLESr(unit, port, rval));

        rval = 0;
        soc_reg_field_set(unit, THDI_PORT_MAX_PKT_SIZEr, &rval,
                            PORT_MAX_PKT_SIZEf, buf_port->pkt_size);
        SOC_IF_ERROR_RETURN(WRITE_THDI_PORT_MAX_PKT_SIZEr(unit, rval));

        /* Input port per port per priority group settings */
        mem = THDI_PORT_PG_CONFIG_Xm;
        for (idx = 0; idx < _MONTEREY_MMU_NUM_PG; idx++) {
            buf_prigroup = &buf->ports[port].prigroups[idx];

            midx = SOC_MONTEREY_MMU_PIPED_MEM_INDEX(unit, port, mem, idx);
            sal_memset(&pg_config_mem, 0, sizeof(pg_config_mem));
            soc_mem_field32_set(unit, mem, &pg_config_mem,
                                PG_MIN_LIMITf, buf_prigroup->guarantee);

            if (buf_prigroup->pool_scale != -1) {
                soc_mem_field32_set(unit, mem, &pg_config_mem, PG_SHARED_DYNAMICf, 1);
                soc_mem_field32_set(unit, mem, &pg_config_mem,
                                    PG_SHARED_LIMITf, buf_prigroup->pool_scale);
            } else {
                soc_mem_field32_set(unit, mem, &pg_config_mem,
                                   PG_SHARED_LIMITf, buf_prigroup->pool_limit);
            }

            soc_mem_field32_set(unit, mem, &pg_config_mem,
                         PG_GBL_HDRM_ENf, buf_prigroup->device_headroom_enable);
            soc_mem_field32_set(unit, mem, &pg_config_mem,
                                       PG_HDRM_LIMITf, buf_prigroup->headroom);

            soc_mem_field32_set(unit, mem, &pg_config_mem,
                                 PG_RESET_OFFSETf, buf_prigroup->pool_resume);

            soc_mem_field32_set(unit, mem, &pg_config_mem,
                                  PG_RESET_FLOORf, buf_prigroup->pool_floor);

            SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem,
                                           MEM_BLOCK_ALL, midx, &pg_config_mem));
        }
    }

    /***********************************
     * THDO
 */
    /* Output port per port per queue setting for regular multicast queue */
    PBMP_ALL_ITER(unit, port) {
        if ((input_port != port) &&
                (input_port != -1)) {
            continue;
        }

        numq = si->port_num_cosq[port];
        qbase = si->port_cosq_base[port];
        if (numq == 0) {
            continue;
        }

        mem = MMU_THDM_DB_QUEUE_CONFIG_0m;

        base = soc_monterey_l2_hw_index(unit, qbase, 0) - _MONTEREY_MC_QUEUE_BASE;

        mem1 = MMU_THDM_DB_QUEUE_OFFSET_0m;
        mem2 = MMU_THDM_MCQE_QUEUE_OFFSET_0m;

        for (idx = 0; idx < numq; idx++) {
            buf_queue = &buf->ports[port].queues[idx];

            sal_memset(entry0, 0, sizeof(mmu_thdm_db_queue_config_0_entry_t));

            soc_mem_field32_set(unit, mem, entry0,
                                Q_MIN_LIMITf, buf_queue->guarantee);
            if (buf_queue->discard_enable) {
                soc_mem_field32_set(unit, mem, entry0, Q_LIMIT_ENABLEf, 1);
            }
            if (buf_queue->color_discard_enable) {
                soc_mem_field32_set(unit, mem, entry0, Q_COLOR_LIMIT_ENABLEf, 1);
            }
            if (buf_queue->pool_scale != -1) {
                soc_mem_field32_set(unit, mem, entry0, Q_LIMIT_DYNAMICf, 1);
                soc_mem_field32_set(unit, mem, entry0,
                           Q_SHARED_ALPHAf, buf_queue->pool_scale);
            } else {
                /* Q_LIMIT_DYNAMIC_CELLf is 0 */
                soc_mem_field32_set(unit, mem, entry0,
                               Q_SHARED_LIMITf, buf_queue->pool_limit);
            }
            if ((buf_queue->yellow_limit & _MMU_CFG_BUF_DYNAMIC_FLAG) ||
                (buf_queue->red_limit & _MMU_CFG_BUF_DYNAMIC_FLAG)) {
                soc_mem_field32_set(unit, mem, entry0,
                                    Q_COLOR_LIMIT_DYNAMICf, 1);
                soc_mem_field32_set(unit, mem, entry0, YELLOW_SHARED_LIMITf,
                     buf_queue->yellow_limit
                              & ~_MMU_CFG_BUF_DYNAMIC_FLAG
                              & ~_MMU_CFG_BUF_DYNAMIC_MAPPING_TYPE_1);
                soc_mem_field32_set(unit, mem, entry0, RED_SHARED_LIMITf,
                     buf_queue->red_limit
                              & ~_MMU_CFG_BUF_DYNAMIC_FLAG
                              & ~_MMU_CFG_BUF_DYNAMIC_MAPPING_TYPE_1);
            } else {
                /* Q_COLOR_LIMIT_DYNAMIC_CELLf is 0 */
                soc_mem_field32_set(unit, mem, entry0, YELLOW_SHARED_LIMITf,
                     (buf_queue->pool_limit + 7) / 8);
                soc_mem_field32_set(unit, mem, entry0, RED_SHARED_LIMITf,
                     (buf_queue->pool_limit + 7) / 8);
            }

            soc_mem_field32_set(unit, mem, entry0,
                                Q_SPIDf, buf_queue->pool_idx);

            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem, MEM_BLOCK_ALL, base + idx,
                               entry0));

            sal_memset(entry0, 0, sizeof(mmu_thdm_db_queue_offset_0_entry_t));

            soc_mem_field32_set(unit, mem1, entry0,
                                RESUME_OFFSETf, (default_mtu_cells * 2)/8);
            soc_mem_field32_set(unit, mem1, entry0,
                                YELLOW_RESUME_OFFSET_PROFILE_SELf, 0);
            soc_mem_field32_set(unit, mem1, entry0,
                                RED_RESUME_OFFSET_PROFILE_SELf, 0);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem1, MEM_BLOCK_ALL, base + idx,
                               entry0));

            sal_memset(entry0, 0, sizeof(mmu_thdm_mcqe_queue_offset_0_entry_t));
            soc_mem_field32_set(unit, mem1, entry0,
                    RESUME_OFFSETf, 1);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem2, MEM_BLOCK_ALL, base + idx,
                               entry0));

            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MMU_THDM_DB_QUEUE_RESUME_OFFSET_PROFILE_YELLOWr, 0,
                                        YELLOW_RESUME_OFFSETf,
                                        2));

            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MMU_THDM_DB_QUEUE_RESUME_OFFSET_PROFILE_REDr, 0,
                                        RED_RESUME_OFFSETf,
                                        2));
        }

        mem = MMU_THDM_MCQE_QUEUE_CONFIG_0m;
        for (idx = 0; idx < numq; idx++) {
            buf_queue = &buf->ports[port].queues[idx];

            sal_memset(entry0, 0, sizeof(mmu_thdm_mcqe_queue_config_0_entry_t));

            soc_mem_field32_set(unit, mem, entry0,
                                Q_MIN_LIMITf, buf_queue->mcq_entry_guarantee/4);
            if (buf_queue->discard_enable) {
                soc_mem_field32_set(unit, mem, entry0, Q_LIMIT_ENABLEf, 1);
            }
            if (buf_queue->color_discard_enable) {
                soc_mem_field32_set(unit, mem, entry0, Q_COLOR_LIMIT_ENABLEf, 1);
            }
            if (buf_queue->pool_scale != -1) {
                soc_mem_field32_set(unit, mem, entry0, Q_LIMIT_DYNAMICf, 1);
                soc_mem_field32_set(unit, mem, entry0,
                           Q_SHARED_ALPHAf, buf_queue->pool_scale);
            } else {
                /* Q_LIMIT_DYNAMIC_CELLf is 0 */
                soc_mem_field32_set(unit, mem, entry0,
                               Q_SHARED_LIMITf, buf_queue->mcqe_limit/4);
            }
            if ((buf_queue->yellow_limit & _MMU_CFG_BUF_DYNAMIC_FLAG) ||
                (buf_queue->red_limit & _MMU_CFG_BUF_DYNAMIC_FLAG)) {
                soc_mem_field32_set(unit, mem, entry0,
                                    Q_COLOR_LIMIT_DYNAMICf, 1);
                soc_mem_field32_set(unit, mem, entry0, YELLOW_SHARED_LIMITf,
                     buf_queue->yellow_limit
                              & ~_MMU_CFG_BUF_DYNAMIC_FLAG
                              & ~_MMU_CFG_BUF_DYNAMIC_MAPPING_TYPE_1);
                soc_mem_field32_set(unit, mem, entry0, RED_SHARED_LIMITf,
                     buf_queue->red_limit
                              & ~_MMU_CFG_BUF_DYNAMIC_FLAG
                              & ~_MMU_CFG_BUF_DYNAMIC_MAPPING_TYPE_1);
            } else {
                /* Q_COLOR_LIMIT_DYNAMIC_CELLf is 0 */
                soc_mem_field32_set(unit, mem, entry0, YELLOW_SHARED_LIMITf,
                     (buf_queue->yellow_limit + 7) / 8);
                soc_mem_field32_set(unit, mem, entry0, RED_SHARED_LIMITf,
                     (buf_queue->red_limit + 7)/ 8);
            }

            soc_mem_field32_set(unit, mem, entry0,
                                Q_SPIDf, buf_queue->pool_idx);

            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem, MEM_BLOCK_ALL, base + idx,
                               entry0));

            sal_memset(entry0, 0, sizeof(mmu_thdm_db_queue_offset_0_entry_t));

            soc_mem_field32_set(unit, mem1, entry0,
                                RESUME_OFFSETf, (default_mtu_cells * 2)/8);
            soc_mem_field32_set(unit, mem1, entry0,
                                YELLOW_RESUME_OFFSET_PROFILE_SELf, 0);
            soc_mem_field32_set(unit, mem1, entry0,
                                RED_RESUME_OFFSET_PROFILE_SELf, 0);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem1, MEM_BLOCK_ALL, base + idx,
                               entry0));

            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MMU_THDM_MCQE_QUEUE_RESUME_OFFSET_PROFILE_YELLOWr, 0,
                                        YELLOW_RESUME_OFFSETf,
                                        1));

            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MMU_THDM_MCQE_QUEUE_RESUME_OFFSET_PROFILE_REDr, 0,
                                        RED_RESUME_OFFSETf,
                                        1));
        }

        /* Per  port per pool */
        for (idx = 0; idx < _MONTEREY_MMU_NUM_POOL; idx++) {
            buf_pool = &buf->pools[idx];
            if (buf_pool->total == 0  || idx > 0 ) {
                continue;
            }

            if (IS_ROE_ENABLED(unit)) {
                limit = _MONTEREY_MMU_TOTAL_CELLS - 16  ;
            } else {
                limit = buf_pool->total - buf_pool->queue_guarantee  ;
            }
            if ((limit - 2 * default_mtu_cells) > 0) {
                rlimit = ((limit - 2 * default_mtu_cells) < min_resume_limit) ?
                    min_resume_limit : limit - 2 * default_mtu_cells;
            } else {
                rlimit = min_resume_limit;
            }
            mem2 = MMU_THDM_DB_PORTSP_CONFIG_0m;
            index1 = SOC_MONTEREY_MMU_PIPED_MEM_INDEX(unit, port, mem2, idx);
            sal_memset(entry0, 0, sizeof(mmu_thdm_db_portsp_config_0_entry_t));

            soc_mem_field32_set(unit, mem2, entry0, SHARED_LIMITf, limit);
            soc_mem_field32_set(unit, mem2, entry0, RED_SHARED_LIMITf, (limit + 7)/8);
            soc_mem_field32_set(unit, mem2, entry0, YELLOW_SHARED_LIMITf, (limit + 7)/8);

            soc_mem_field32_set(unit, mem2, entry0,
                                            SHARED_LIMIT_ENABLEf, !lossless);

            soc_mem_field32_set(unit, mem2, entry0, SHARED_RESUME_LIMITf, (rlimit + 7)/8);
            soc_mem_field32_set(unit, mem2, entry0, YELLOW_RESUME_LIMITf, (rlimit + 7)/8);
            soc_mem_field32_set(unit, mem2, entry0, RED_RESUME_LIMITf, (rlimit + 7)/8);

            SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem2, MEM_BLOCK_ALL,
                                index1, entry0));

            mem2 = MMU_THDM_MCQE_PORTSP_CONFIG_0m;
            sal_memset(entry0, 0, sizeof(mmu_thdm_mcqe_portsp_config_0_entry_t));

            limit = buf_pool->total_mcq_entry - buf_pool->mcq_entry_reserved;
            rlimit = (limit < min_resume_limit) ? min_resume_limit : limit;

            soc_mem_field32_set(unit, mem2, entry0, SHARED_LIMITf, (limit + 3)/4);
            soc_mem_field32_set(unit, mem2, entry0, SHARED_RESUME_LIMITf,
                    ((rlimit/8) - 1));
            soc_mem_field32_set(unit, mem2, entry0, SHARED_LIMIT_ENABLEf, !lossless);
            soc_mem_field32_set(unit, mem2, entry0, RED_SHARED_LIMITf, (limit + 7)/8);
            soc_mem_field32_set(unit, mem2, entry0, RED_RESUME_LIMITf,
                    ((rlimit + 7)/8 - 1));
            soc_mem_field32_set(unit, mem2, entry0, YELLOW_SHARED_LIMITf, (limit + 7)/8);
            soc_mem_field32_set(unit, mem2, entry0, YELLOW_RESUME_LIMITf,
                    ((rlimit + 7)/8 -1));

            SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem2, MEM_BLOCK_ALL,
                                index1, entry0));
        }
    }

    /* Output port per port per queue setting for regular unicast queue */
    PBMP_PORT_ITER(unit, port) {
        if ((input_port != port) &&
                (input_port != -1)) {
            continue;
        }

        if(IS_RDB_PORT(unit, port)) {
            continue;
        }
        /* per port regular unicast queue */
        numq = si->port_num_uc_cosq[port];
        qbase = si->port_uc_cosq_base[port];
        if (numq == 0) {
            continue;
        }
        base = soc_monterey_l2_hw_index(unit, qbase, 1);
        mem = MMU_THDU_XPIPE_CONFIG_QUEUEm;
        mem1 = MMU_THDU_XPIPE_OFFSET_QUEUEm;
        mem2 = MMU_THDU_XPIPE_Q_TO_QGRP_MAPm;
        for (idx = 0; idx < numq; idx++) {
            buf_queue = &buf->ports[port].queues[si->port_num_cosq[port] + idx];

            sal_memset(entry0, 0, sizeof(mmu_thdu_xpipe_config_queue_entry_t));
            sal_memset(entry1, 0, sizeof(mmu_thdu_xpipe_offset_queue_entry_t));

            soc_mem_field32_set(unit, mem, entry0,
                                Q_MIN_LIMIT_CELLf, buf_queue->guarantee);
            if (buf_queue->pool_scale != -1) {
                soc_mem_field32_set(unit, mem, entry0, Q_LIMIT_DYNAMIC_CELLf, 1);
                soc_mem_field32_set(unit, mem, entry0,
                           Q_SHARED_ALPHA_CELLf, buf_queue->pool_scale);
            } else {
                /* Q_LIMIT_DYNAMIC_CELLf is 0 */
                soc_mem_field32_set(unit, mem, entry0,
                               Q_SHARED_LIMIT_CELLf, buf_queue->pool_limit);
            }
            soc_mem_field32_set(unit, mem1,
                entry1, RESET_OFFSET_CELLf, buf_queue->pool_resume / 8);

            if ((buf_queue->yellow_limit & _MMU_CFG_BUF_DYNAMIC_FLAG) ||
                (buf_queue->red_limit & _MMU_CFG_BUF_DYNAMIC_FLAG)) {
                soc_mem_field32_set(unit, mem, entry0,
                                    Q_COLOR_LIMIT_DYNAMIC_CELLf, 1);
                soc_mem_field32_set(unit, mem, entry0, LIMIT_YELLOW_CELLf,
                     buf_queue->yellow_limit
                              & ~_MMU_CFG_BUF_DYNAMIC_FLAG
                              & ~_MMU_CFG_BUF_DYNAMIC_MAPPING_TYPE_1);
                soc_mem_field32_set(unit, mem, entry0, LIMIT_RED_CELLf,
                     buf_queue->red_limit
                              & ~_MMU_CFG_BUF_DYNAMIC_FLAG
                              & ~_MMU_CFG_BUF_DYNAMIC_MAPPING_TYPE_1);
            } else {
                /* Q_COLOR_LIMIT_DYNAMIC_CELLf is 0 */
                soc_mem_field32_set(unit, mem, entry0, LIMIT_YELLOW_CELLf,
                                         (buf_queue->yellow_limit + 7) / 8);
                soc_mem_field32_set(unit, mem, entry0, LIMIT_RED_CELLf,
                                         (buf_queue->red_limit + 7) / 8);
            }
            soc_mem_field32_set(unit, mem1, entry1, RESET_OFFSET_YELLOW_CELLf,
                                buf_queue->yellow_resume / 8);
            soc_mem_field32_set(unit, mem1, entry1, RESET_OFFSET_RED_CELLf,
                                buf_queue->red_resume / 8);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem, MEM_BLOCK_ALL, base + idx, entry0));

            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem1, MEM_BLOCK_ALL, base + idx, entry1));

            sal_memset(entry0, 0, sizeof(mmu_thdo_q_to_qgrp_map_entry_t));
            soc_mem_field32_set(unit, mem2,
                    entry0, Q_SPIDf, buf_queue->pool_idx);
                soc_mem_field32_set(unit, mem2, entry0, QGROUP_VALIDf, lossless ? 0 : 1);
                if (IS_CPRI_PORT(unit,port) && ( idx == 0 || idx == 6 || idx ==7 )) {
                    soc_mem_field32_set(unit, mem2, entry0, Q_LIMIT_ENABLEf,  0 ) ;
                    soc_mem_field32_set(unit, mem2, entry0, QGROUP_VALIDf,  1);
                    soc_mem_field32_set(unit, mem2, entry0, USE_QGROUP_MINf,0);
               }  else if (IS_RSVD4_PORT(unit,port) && ( idx != 1)) {
                    soc_mem_field32_set(unit, mem2, entry0, Q_LIMIT_ENABLEf,  0 ) ;
                    soc_mem_field32_set(unit, mem2, entry0, QGROUP_VALIDf,  1);
                    soc_mem_field32_set(unit, mem2, entry0, USE_QGROUP_MINf,0);

                } else if (!IS_CPRI_PORT(unit,port) && !IS_RSVD4_PORT(unit,port) &&
                           IS_ROE_ENABLED(unit) && (idx ==5 || idx ==7)) {
                    soc_mem_field32_set(unit, mem2, entry0, Q_LIMIT_ENABLEf,  0 ) ;
                    soc_mem_field32_set(unit, mem2, entry0, QGROUP_VALIDf,  1);
                    soc_mem_field32_set(unit, mem2, entry0, USE_QGROUP_MINf,0);
                } else {
                    soc_mem_field32_set(unit, mem2, entry0, QGROUP_VALIDf, lossless ? 0 : 1);
                    soc_mem_field32_set(unit, mem2, entry0, Q_LIMIT_ENABLEf, lossless ? 0 : 1);
                    soc_mem_field32_set(unit, mem2, entry0, USE_QGROUP_MINf,lossless ? 0 :1);
                }
                soc_mem_field32_set(unit, mem2, entry0, QGROUPf,
                                    (buf_queue->qgroup_id == -1) ? 0 : buf_queue->qgroup_id);

            SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem2, MEM_BLOCK_ALL,
                                base + idx, entry0));
        }

        /* Per  port per pool unicast */
        for (idx = 0; idx < _MONTEREY_MMU_NUM_POOL; idx++) {
            buf_pool = &buf->pools[idx];

            if (buf_pool->total == 0) {
                continue;
            }
            if (IS_ROE_ENABLED(unit)) {
                limit = _MONTEREY_MMU_TOTAL_CELLS - 16  ;
            } else {
                limit = buf_pool->total - buf_pool->queue_guarantee  ;
            }
            mem2 = MMU_THDU_XPIPE_CONFIG_PORTm;
            index1 = SOC_MONTEREY_MMU_PIPED_MEM_INDEX(unit, port, mem2, idx);
            mem3 = MMU_THDU_XPIPE_RESUME_PORTm;
            sal_memset(entry0, 0, sizeof(mmu_thdu_xpipe_config_port_entry_t));
            sal_memset(entry1, 0, sizeof(mmu_thdu_xpipe_resume_port_entry_t));

            soc_mem_field32_set(unit, mem2, entry0, SHARED_LIMITf, limit);
            soc_mem_field32_set(unit, mem3, entry1,
                         SHARED_RESUMEf, ((limit - (default_mtu_cells * 2)) + 7)/8);

            soc_mem_field32_set(unit, mem2, entry0, YELLOW_LIMITf, (limit + 7)/8);
            soc_mem_field32_set(unit, mem3, entry1,
                            YELLOW_RESUMEf, ((limit - (default_mtu_cells * 2)) + 7)/8);

            soc_mem_field32_set(unit, mem2, entry0, RED_LIMITf, (limit + 7)/8);
            soc_mem_field32_set(unit, mem3, entry1,
                              RED_RESUMEf, ((limit - (default_mtu_cells * 2)) + 7)/8);

            SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem2, MEM_BLOCK_ALL,
                                index1, entry0));
            SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem3, MEM_BLOCK_ALL,
                                index1, entry1));
        }
    }



    if (input_port == -1) {
        /* RQE */
        for (idx = 0; idx < 11; idx++) {
            buf_rqe_queue = &buf->rqe_queues[idx];

            rval = 0;
            soc_reg_field_set(unit, MMU_THDR_DB_LIMIT_MIN_PRIQr,
                    &rval, MIN_LIMITf, buf_rqe_queue->guarantee);
            SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_DB_LIMIT_MIN_PRIQr(unit, idx, rval));

            rval = 0;
            rval2 = 0;
            rval3 = 0;
            soc_reg_field_set(unit, MMU_THDR_DB_CONFIG1_PRIQr,
                    &rval, SPIDf, buf_rqe_queue->pool_idx);

            if ((buf_rqe_queue->red_limit & _MMU_CFG_BUF_DYNAMIC_FLAG) ||
                    (buf_rqe_queue->yellow_limit & _MMU_CFG_BUF_DYNAMIC_FLAG)) {
                soc_reg_field_set(unit, MMU_THDR_DB_CONFIG1_PRIQr,
                        &rval, COLOR_LIMIT_DYNAMICf, 1);
                soc_reg_field_set(unit, MMU_THDR_DB_LIMIT_COLOR_PRIQr, &rval3,
                        SHARED_RED_LIMITf, buf_rqe_queue->red_limit & ~_MMU_CFG_BUF_DYNAMIC_FLAG);
                soc_reg_field_set(unit, MMU_THDR_DB_LIMIT_COLOR_PRIQr, &rval3,
                        SHARED_YELLOW_LIMITf, buf_rqe_queue->yellow_limit & ~_MMU_CFG_BUF_DYNAMIC_FLAG);
            } else {
                soc_reg_field_set(unit, MMU_THDR_DB_LIMIT_COLOR_PRIQr, &rval3,
                        SHARED_RED_LIMITf, (buf_rqe_queue->red_limit + 7)/ 8);

                soc_reg_field_set(unit, MMU_THDR_DB_LIMIT_COLOR_PRIQr, &rval3,
                        SHARED_YELLOW_LIMITf, (buf_rqe_queue->yellow_limit + 7) / 8);
            }

            soc_reg_field_set(unit, MMU_THDR_DB_CONFIG1_PRIQr, &rval, LIMIT_ENABLEf,
                    (buf_rqe_queue->discard_enable ? 1 : 0));

            if (buf_rqe_queue->pool_scale != -1) {
                soc_reg_field_set(unit, MMU_THDR_DB_CONFIG1_PRIQr,
                        &rval, DYNAMIC_ENABLEf, 1);
                soc_reg_field_set(unit, MMU_THDR_DB_CONFIG_PRIQr, &rval2,
                        SHARED_ALPHAf, buf_rqe_queue->pool_scale);
            } else {
                soc_reg_field_set(unit, MMU_THDR_DB_CONFIG_PRIQr, &rval2,
                        SHARED_LIMITf, buf_rqe_queue->pool_limit);
            }
            soc_reg_field_set(unit, MMU_THDR_DB_CONFIG_PRIQr, &rval2, RESET_OFFSETf, 2);

            SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_DB_CONFIG1_PRIQr(unit, idx, rval));
            SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_DB_CONFIG_PRIQr_REG32(unit, idx, rval2));
            SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_DB_LIMIT_COLOR_PRIQr(unit, idx, rval3));

            rval = 0;
            soc_reg_field_set(unit, MMU_THDR_DB_RESET_OFFSET_COLOR_PRIQr,
                    &rval, RESET_OFFSET_REDf, (default_mtu_cells * 2)/8);
            soc_reg_field_set(unit, MMU_THDR_DB_RESET_OFFSET_COLOR_PRIQr,
                    &rval, RESET_OFFSET_YELLOWf, (default_mtu_cells * 2)/8);
            SOC_IF_ERROR_RETURN(
                    WRITE_MMU_THDR_DB_RESET_OFFSET_COLOR_PRIQr(unit, idx, rval));

            /* queue entry */
            buf_pool = &buf->pools[buf_rqe_queue->pool_idx];
            fval = ((buf_pool->total_rqe_entry + 7) / 8);
            rval = 0;
            soc_reg_field_set(unit, MMU_THDR_QE_LIMIT_MIN_PRIQr,
                    &rval, MIN_LIMITf, buf_rqe_queue->guarantee/8);
            SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_QE_LIMIT_MIN_PRIQr(unit, idx, rval));

            rval = 0;
            rval2 = 0;
            rval3 = 0;
            soc_reg_field_set(unit, MMU_THDR_QE_CONFIG1_PRIQr,
                    &rval, SPIDf, buf_rqe_queue->pool_idx);

            soc_reg_field_set(unit, MMU_THDR_QE_CONFIG1_PRIQr,
                    &rval, COLOR_LIMIT_DYNAMICf, lossless ? 0 : 1);
            soc_reg_field_set(unit, MMU_THDR_QE_LIMIT_COLOR_PRIQr,
                    &rval3, SHARED_RED_LIMITf, lossless ? fval : 0);
            soc_reg_field_set(unit, MMU_THDR_QE_LIMIT_COLOR_PRIQr,
                    &rval3, SHARED_YELLOW_LIMITf, lossless ? fval : 0);
            soc_reg_field_set(unit, MMU_THDR_QE_CONFIG1_PRIQr, &rval, DYNAMIC_ENABLEf,
                    lossless ? 0 :1);
            soc_reg_field_set(unit, MMU_THDR_QE_CONFIG1_PRIQr, &rval, LIMIT_ENABLEf,
                     (buf_rqe_queue->discard_enable ? 1 : 0));
            soc_reg_field_set(unit, MMU_THDR_QE_CONFIG_PRIQr, &rval2,
                    SHARED_LIMITf, lossless ? fval : 8);
            soc_reg_field_set(unit, MMU_THDR_QE_CONFIG_PRIQr, &rval2, RESET_OFFSETf, 1);
            SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_QE_CONFIG1_PRIQr(unit, idx, rval));
            SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_QE_CONFIG_PRIQr(unit, idx, rval2));
            SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_QE_LIMIT_COLOR_PRIQr(unit, idx, rval3));

            rval = 0;
            soc_reg_field_set(unit, MMU_THDR_QE_RESET_OFFSET_COLOR_PRIQr,
                    &rval, RESET_OFFSET_REDf, default_mtu_cells/8);
            soc_reg_field_set(unit, MMU_THDR_QE_RESET_OFFSET_COLOR_PRIQr,
                    &rval, RESET_OFFSET_YELLOWf, default_mtu_cells/8);
            SOC_IF_ERROR_RETURN(
                    WRITE_MMU_THDR_QE_RESET_OFFSET_COLOR_PRIQr(unit, idx, rval));

        }

        /* per pool RQE settings */
        total_rsvd = 0;
        for (idx = 0; idx < 4; idx++) {
            buf_pool = &buf->pools[idx];
            if (((buf_pool->size & ~_MMU_CFG_BUF_PERCENT_FLAG) == 0) ||
                    (buf_pool->total == 0)) {
                continue;
            }
            total_rsvd = buf_pool->queue_guarantee;
            if (idx == 0) {
                limit = buf_pool->total - total_rsvd - asf_rsvd - 16;
            } else {
                limit = buf_pool->total ;
            }


            rval = 0;
            soc_reg_field_set(unit, MMU_THDR_DB_CONFIG_SPr,
                    &rval, SHARED_LIMITf, limit);
            soc_reg_field_set(unit, MMU_THDR_DB_CONFIG_SPr, &rval, RESUME_LIMITf,
                    (limit - (default_mtu_cells * 2) + 7)/8);
            SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_DB_CONFIG_SPr_REG32(unit, idx, rval));

            rval = 0;
            soc_reg_field_set(unit, MMU_THDR_DB_SP_SHARED_LIMITr, &rval,
                    SHARED_RED_LIMITf, (limit + 7)/8);
            soc_reg_field_set(unit, MMU_THDR_DB_SP_SHARED_LIMITr, &rval,
                    SHARED_YELLOW_LIMITf, (limit + 7)/8);
            SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_DB_SP_SHARED_LIMITr(unit, idx, rval));

            rval = 0;
            soc_reg_field_set(unit, MMU_THDR_DB_RESUME_COLOR_LIMIT_SPr, &rval,
                    RESUME_RED_LIMITf, (limit - (default_mtu_cells * 2) + 7)/8);
            soc_reg_field_set(unit, MMU_THDR_DB_RESUME_COLOR_LIMIT_SPr, &rval,
                    RESUME_YELLOW_LIMITf, (limit - (default_mtu_cells * 2) + 7)/8);
            SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_DB_RESUME_COLOR_LIMIT_SPr(unit,
                        idx, rval));
            if (lossless) {
                rqlen = ((buf_pool->total_rqe_entry + 7) / 8) - 1 ;
            } else {
                rqlen = ((buf_pool->total_rqe_entry + 7) / 8) ;
            }
            if (rqlen == 0) {
                continue;
            }

            rval = 0;
            soc_reg_field_set(unit, MMU_THDR_QE_CONFIG_SPr, &rval, SHARED_LIMITf, rqlen);
            soc_reg_field_set(unit, MMU_THDR_QE_CONFIG_SPr, &rval, RESUME_LIMITf, rqlen - 1);
            SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_QE_CONFIG_SPr(unit, idx, rval));

            rval = 0;
            soc_reg_field_set(unit, MMU_THDR_QE_SHARED_COLOR_LIMIT_SPr, &rval,
                    SHARED_RED_LIMITf, rqlen);
            soc_reg_field_set(unit, MMU_THDR_QE_SHARED_COLOR_LIMIT_SPr, &rval,
                    SHARED_YELLOW_LIMITf, rqlen);
            SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_QE_SHARED_COLOR_LIMIT_SPr(unit, idx, rval));

            rval = 0;
            soc_reg_field_set(unit, MMU_THDR_QE_RESUME_COLOR_LIMIT_SPr, &rval,
                    RESUME_RED_LIMITf, rqlen - 1);
            soc_reg_field_set(unit, MMU_THDR_QE_RESUME_COLOR_LIMIT_SPr, &rval,
                    RESUME_YELLOW_LIMITf, rqlen - 1);
            SOC_IF_ERROR_RETURN(WRITE_MMU_THDR_QE_RESUME_COLOR_LIMIT_SPr(unit,
                        idx, rval));
        }

        /* Device level config setting */
        if (soc_property_get(unit, spn_PORT_UC_MC_ACCOUNTING_COMBINE, 0)) {
            SOC_IF_ERROR_RETURN(READ_MMU_THDM_DB_DEVICE_THR_CONFIGr(unit, &rval));
            soc_reg_field_set(unit, MMU_THDM_DB_DEVICE_THR_CONFIGr, &rval,
                    UC_MC_PORTSP_COMB_ACCT_ENABLEf, 1);
            SOC_IF_ERROR_RETURN(WRITE_MMU_THDM_DB_DEVICE_THR_CONFIGr(unit, rval));
        }
    }
    return SOC_E_NONE;
}

STATIC int
_soc_monterey_mmu_config_shared_update_check(int unit, int val1, int val2, int flags)
{
    int rv = 0;
    if (flags == 1) {
        if (val1 > val2) {
            rv = 1;
        }
    } else {
        if (val1 < val2) {
            rv = 1;
        }
    }
    return rv;
}

int
soc_monterey_mmu_config_shared_buf_recalc(int unit, uint32 spid, int shared_size, int flags)
{
    soc_info_t *si;
    soc_mem_t mem, mem2;
    soc_reg_t reg = INVALIDr;
    soc_field_t field = INVALIDf;
    uint32 rval, rval2;
    uint32 entry0[SOC_MAX_MEM_WORDS];
    thdi_port_pg_config_entry_t pg_config_mem;
    mmu_thdo_config_qgroup_entry_t cfg_qgrp;
    int port, base, qbase, numq, idx;
    int cur_limit, midx, index1;
    int granularity;
    uint16      dev_id;
    uint8       rev_id;

    static const soc_field_t pg_spid_field[] = {
        PG0_SPIDf, PG1_SPIDf, PG2_SPIDf, PG3_SPIDf,
        PG4_SPIDf, PG5_SPIDf, PG6_SPIDf, PG7_SPIDf
    };

    if (spid >= _MONTEREY_MMU_NUM_POOL) {
        return SOC_E_PARAM;
    }

    soc_cm_get_id(unit, &dev_id, &rev_id);

    si = &SOC_INFO(unit);

    /* per service pool settings */
    idx = spid;
    rval = 0;
    reg = MMU_THDM_DB_POOL_SHARED_LIMITr;
    field = SHARED_LIMITf;
    granularity = 1;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, idx, &rval));
    cur_limit = soc_reg_field_get(unit, reg, rval, field);
    if (_soc_monterey_mmu_config_shared_update_check(unit, cur_limit, shared_size/granularity, flags)) {
        soc_reg_field_set(unit, reg, &rval, field, (shared_size/granularity));
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, idx, rval));
    }

    rval = 0;
    reg = MMU_THDM_DB_POOL_YELLOW_SHARED_LIMITr;
    field = YELLOW_SHARED_LIMITf;
    granularity = 8;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, idx, &rval));
    cur_limit = soc_reg_field_get(unit, reg, rval, field);
    if (_soc_monterey_mmu_config_shared_update_check(unit, cur_limit, shared_size/granularity, flags)) {
        soc_reg_field_set(unit, reg, &rval, field, (shared_size/granularity));
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, idx, rval));
    }

    rval = 0;
    reg = MMU_THDM_DB_POOL_RED_SHARED_LIMITr;
    field = RED_SHARED_LIMITf;
    granularity = 8;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, idx, &rval));
    cur_limit = soc_reg_field_get(unit, reg, rval, field);
    if (_soc_monterey_mmu_config_shared_update_check(unit, cur_limit, shared_size/granularity, flags)) {
        soc_reg_field_set(unit, reg, &rval, field, (shared_size/granularity));
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, idx, rval));
    }

    /* configure Q-groups */
    mem = MMU_THDU_XPIPE_CONFIG_QGROUPm;
    for (idx = 0; idx < SOC_MONTEREY_MMU_CFG_QGROUP_MAX; idx++) {
        sal_memset(&cfg_qgrp, 0, sizeof(cfg_qgrp));

        SOC_IF_ERROR_RETURN
                (soc_mem_read(unit, mem, MEM_BLOCK_ALL, idx, &cfg_qgrp));

        field = Q_SHARED_LIMIT_CELLf;
        granularity = 1;
        if (!soc_mem_field32_get(unit, mem, &cfg_qgrp, Q_LIMIT_DYNAMIC_CELLf)) {
            cur_limit = soc_mem_field32_get(unit, mem, &cfg_qgrp, field);
            if (_soc_monterey_mmu_config_shared_update_check(unit, cur_limit, shared_size/granularity, flags)) {
                soc_mem_field32_set(unit, mem, &cfg_qgrp,
                                    field, shared_size/granularity);
            }
        }

        granularity = 8;
        if (!soc_mem_field32_get(unit, mem, &cfg_qgrp,
                                 Q_COLOR_LIMIT_DYNAMIC_CELLf)) {
            field = LIMIT_RED_CELLf;
            cur_limit = soc_mem_field32_get(unit, mem, &cfg_qgrp, field);
            if (_soc_monterey_mmu_config_shared_update_check(unit, cur_limit, shared_size/granularity, flags)) {
                soc_mem_field32_set(unit, mem, &cfg_qgrp,
                                    field, shared_size/granularity);
            }

            field = LIMIT_YELLOW_CELLf;
            cur_limit = soc_mem_field32_get(unit, mem, &cfg_qgrp, field);
            if (_soc_monterey_mmu_config_shared_update_check(unit, cur_limit, shared_size/granularity, flags)) {
                soc_mem_field32_set(unit, mem, &cfg_qgrp,
                                    field, shared_size/granularity);
            }
        }

        SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem, MEM_BLOCK_ALL, idx, &cfg_qgrp));

    }

    /* Input port per port settings */
    PBMP_ALL_ITER(unit, port) {
        rval = 0;
        /* Input port per port per priority group settings */
        mem = THDI_PORT_PG_CONFIG_Xm;
        field = PG_SHARED_LIMITf;
        granularity = 1;
        for (idx = 0; idx < _MONTEREY_MMU_NUM_PG; idx++) {
            SOC_IF_ERROR_RETURN(READ_THDI_PORT_PG_SPIDr(unit, port, &rval));
            if (spid != soc_reg_field_get(unit, THDI_PORT_PG_SPIDr, rval,
                            pg_spid_field[idx])){
                continue;
            }
            midx = SOC_MONTEREY_MMU_PIPED_MEM_INDEX(unit, port, mem, idx);
            sal_memset(&pg_config_mem, 0, sizeof(pg_config_mem));
            SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem,
                                             MEM_BLOCK_ALL, midx, &pg_config_mem));
            if (!soc_mem_field32_get(unit, mem, &pg_config_mem, PG_SHARED_DYNAMICf)) {
                cur_limit = soc_mem_field32_get(unit, mem, &pg_config_mem, field);
                if (_soc_monterey_mmu_config_shared_update_check(unit, cur_limit, shared_size/granularity, flags)) {
                    soc_mem_field32_set(unit, mem, &pg_config_mem,
                                        field, shared_size/granularity);
                }
            }

            SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem,
                                           MEM_BLOCK_ALL, midx, &pg_config_mem));
        }
    }

    /***********************************
     * THDO
 */
    /* Output port per port per queue setting for regular multicast queue */
    PBMP_PORT_ITER(unit, port) {

        numq = si->port_num_cosq[port];
        qbase = si->port_cosq_base[port];
        if (numq == 0) {
            continue;
        }

        mem = MMU_THDM_DB_QUEUE_CONFIG_0m;
        base = soc_monterey_l2_hw_index(unit, qbase, 0) - _MONTEREY_MC_QUEUE_BASE;

        for (idx = 0; idx < numq; idx++) {
            sal_memset(entry0, 0, sizeof(mmu_thdm_db_queue_config_0_entry_t));

            SOC_IF_ERROR_RETURN
                (soc_mem_read(unit, mem, MEM_BLOCK_ALL, base + idx, entry0));

            if (spid != soc_mem_field32_get(unit, mem, entry0, Q_SPIDf)){
                continue;
            }
            field = Q_SHARED_LIMITf;
            granularity = 1;
            if (!soc_mem_field32_get(unit, mem, entry0, Q_LIMIT_DYNAMICf)) {
                cur_limit = soc_mem_field32_get(unit, mem, entry0, field);
                if (_soc_monterey_mmu_config_shared_update_check(unit, cur_limit, shared_size/granularity, flags)) {
                    soc_mem_field32_set(unit, mem, entry0,
                                        field, shared_size/granularity);
                }
            }

            granularity = 8;
            if (!soc_mem_field32_get(unit, mem, entry0, Q_COLOR_LIMIT_DYNAMICf)) {
                field = YELLOW_SHARED_LIMITf;
                cur_limit = soc_mem_field32_get(unit, mem, entry0, field);
                if (_soc_monterey_mmu_config_shared_update_check(unit, cur_limit, shared_size/granularity, flags)) {
                    soc_mem_field32_set(unit, mem, entry0,
                                        field, shared_size/granularity);
                }
                field = RED_SHARED_LIMITf;
                cur_limit = soc_mem_field32_get(unit, mem, entry0, field);
                if (_soc_monterey_mmu_config_shared_update_check(unit, cur_limit, shared_size/granularity, flags)) {
                    soc_mem_field32_set(unit, mem, entry0,
                                        field, shared_size/granularity);
                }
            }
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem, MEM_BLOCK_ALL, base + idx,
                               entry0));
        }

        /* Per  port per pool */
        idx = spid;
        mem = MMU_THDM_DB_PORTSP_CONFIG_0m;
        index1 = SOC_MONTEREY_MMU_PIPED_MEM_INDEX(unit, port, mem, idx);
        sal_memset(entry0, 0, sizeof(mmu_thdm_db_portsp_config_0_entry_t));

        SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                            index1, entry0));

        field = SHARED_LIMITf;
        granularity = 1;
        cur_limit = soc_mem_field32_get(unit, mem, entry0, field);
        if (_soc_monterey_mmu_config_shared_update_check(unit, cur_limit, shared_size/granularity, flags)) {
            soc_mem_field32_set(unit, mem, entry0,
                                field, shared_size/granularity);
        }

        field = RED_SHARED_LIMITf;
        granularity = 8;
        cur_limit = soc_mem_field32_get(unit, mem, entry0, field);
        if (_soc_monterey_mmu_config_shared_update_check(unit, cur_limit, shared_size/granularity, flags)) {
            soc_mem_field32_set(unit, mem, entry0,
                                field, shared_size/granularity);
        }

        field = YELLOW_SHARED_LIMITf;
        granularity = 8;
        cur_limit = soc_mem_field32_get(unit, mem, entry0, field);
        if (_soc_monterey_mmu_config_shared_update_check(unit, cur_limit, shared_size/granularity, flags)) {
            soc_mem_field32_set(unit, mem, entry0,
                                field, shared_size/granularity);
        }
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                            index1, entry0));
    }

    /* Output port per port per queue setting for regular unicast queue */
    PBMP_PORT_ITER(unit, port) {
        /* per port regular unicast queue */
        numq = si->port_num_uc_cosq[port];
        qbase = si->port_uc_cosq_base[port];

        if (numq == 0) {
            continue;
        }
        base = soc_monterey_l2_hw_index(unit, qbase, 1);
        mem = MMU_THDU_XPIPE_CONFIG_QUEUEm;
        mem2 = MMU_THDU_XPIPE_Q_TO_QGRP_MAPm;
        for (idx = 0; idx < numq; idx++) {
            sal_memset(entry0, 0, sizeof(mmu_thdu_xpipe_q_to_qgrp_map_entry_t));
            SOC_IF_ERROR_RETURN
                (soc_mem_read(unit, mem2, MEM_BLOCK_ALL, base+idx, entry0));
            if (spid != soc_mem_field32_get(unit, mem2, entry0, Q_SPIDf)){
                continue;
            }
            sal_memset(entry0, 0, sizeof(mmu_thdu_xpipe_config_queue_entry_t));

            SOC_IF_ERROR_RETURN
                (soc_mem_read(unit, mem, MEM_BLOCK_ALL, base + idx, entry0));

            field = Q_SHARED_LIMIT_CELLf;
            granularity = 1;
            if (!soc_mem_field32_get(unit, mem, entry0, Q_LIMIT_DYNAMIC_CELLf)) {
                cur_limit = soc_mem_field32_get(unit, mem, entry0, field);
                if (_soc_monterey_mmu_config_shared_update_check(unit, cur_limit, shared_size/granularity, flags)) {
                    soc_mem_field32_set(unit, mem, entry0,
                                        field, shared_size/granularity);
                }
            }

            granularity = 8;
            if (!soc_mem_field32_get(unit, mem, entry0, Q_COLOR_LIMIT_DYNAMIC_CELLf)) {
                field = LIMIT_YELLOW_CELLf;
                cur_limit = soc_mem_field32_get(unit, mem, entry0, field);
                if (_soc_monterey_mmu_config_shared_update_check(unit, cur_limit, shared_size/granularity, flags)) {
                    soc_mem_field32_set(unit, mem, entry0,
                                        field, shared_size/granularity);
                }
                field = LIMIT_RED_CELLf;
                cur_limit = soc_mem_field32_get(unit, mem, entry0, field);
                if (_soc_monterey_mmu_config_shared_update_check(unit, cur_limit, shared_size/granularity, flags)) {
                    soc_mem_field32_set(unit, mem, entry0,
                                        field, shared_size/granularity);
                }
            }
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem, MEM_BLOCK_ALL, base + idx, entry0));
        }

        /* Per  port per pool unicast */
        idx = spid;
        mem = MMU_THDU_XPIPE_CONFIG_PORTm;
        index1 = SOC_MONTEREY_MMU_PIPED_MEM_INDEX(unit, port, mem, idx);
        sal_memset(entry0, 0, sizeof(mmu_thdu_xpipe_config_port_entry_t));

        SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                            index1, entry0));

        field = SHARED_LIMITf;
        granularity = 1;
        cur_limit = soc_mem_field32_get(unit, mem, entry0, field);
        if (_soc_monterey_mmu_config_shared_update_check(unit, cur_limit, shared_size/granularity, flags)) {
            soc_mem_field32_set(unit, mem, entry0,
                                field, shared_size/granularity);
        }

        granularity = 8;
        field = YELLOW_LIMITf;
        cur_limit = soc_mem_field32_get(unit, mem, entry0, field);
        if (_soc_monterey_mmu_config_shared_update_check(unit, cur_limit, shared_size/granularity, flags)) {
            soc_mem_field32_set(unit, mem, entry0,
                                field, shared_size/granularity);
        }
        field = RED_LIMITf;
        cur_limit = soc_mem_field32_get(unit, mem, entry0, field);
        if (_soc_monterey_mmu_config_shared_update_check(unit, cur_limit, shared_size/granularity, flags)) {
            soc_mem_field32_set(unit, mem, entry0,
                                field, shared_size/granularity);
        }

        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                            index1, entry0));
    }

    /* RQE */
    for (idx = 0; idx < 11; idx++) {
        rval = 0;
        rval2 = 0;

        SOC_IF_ERROR_RETURN(READ_MMU_THDR_DB_CONFIG1_PRIQr(unit, idx, &rval));
        if (spid != soc_reg_field_get(unit, MMU_THDR_DB_CONFIG1_PRIQr, rval, SPIDf)){
            continue;
        }

        reg = MMU_THDR_DB_CONFIG_PRIQr;
        field = SHARED_LIMITf;
        granularity = 1;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, idx, &rval2));
        if (!soc_reg_field_get(unit, MMU_THDR_DB_CONFIG1_PRIQr, rval,
                               DYNAMIC_ENABLEf)) {
            cur_limit = soc_reg_field_get(unit, reg, rval2, field);
            if (_soc_monterey_mmu_config_shared_update_check(unit, cur_limit, shared_size/granularity, flags)) {
                soc_reg_field_set(unit, reg, &rval2, field, (shared_size/granularity));
                SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, idx, rval2));
            }
        }

        rval2 = 0;
        reg = MMU_THDR_DB_LIMIT_COLOR_PRIQr;
        granularity = 8;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, idx, &rval2));

        if (!soc_reg_field_get(unit, MMU_THDR_DB_CONFIG1_PRIQr, rval,
                               COLOR_LIMIT_DYNAMICf)) {
            field = SHARED_RED_LIMITf;
            cur_limit = soc_reg_field_get(unit, reg, rval2, field);
            if (_soc_monterey_mmu_config_shared_update_check(unit, cur_limit, shared_size/granularity, flags)) {
                soc_reg_field_set(unit, reg, &rval2, field, (shared_size/granularity));
            }

            field = SHARED_YELLOW_LIMITf;
            cur_limit = soc_reg_field_get(unit, reg, rval2, field);
            if (_soc_monterey_mmu_config_shared_update_check(unit, cur_limit, shared_size/granularity, flags)) {
                soc_reg_field_set(unit, reg, &rval2, field, (shared_size/granularity));
            }
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, idx, rval2));
        }

    }

    /* per pool RQE settings */
    idx = spid;
    reg = MMU_THDR_DB_CONFIG_SPr;
    field = SHARED_LIMITf;
    granularity = 1;

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, idx, &rval2));

    cur_limit = soc_reg_field_get(unit, reg, rval, field);

    if (_soc_monterey_mmu_config_shared_update_check(unit, cur_limit, shared_size/granularity, flags)) {
        soc_reg_field_set(unit, reg, &rval2, field, (shared_size/granularity));
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, idx, rval2));
    }

    reg = MMU_THDR_DB_SP_SHARED_LIMITr;
    granularity = 8;

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, idx, &rval2));

    field =SHARED_YELLOW_LIMITf;
    cur_limit = soc_reg_field_get(unit, reg, rval, field);

    if (_soc_monterey_mmu_config_shared_update_check(unit, cur_limit, shared_size/granularity, flags)){
        soc_reg_field_set(unit, reg, &rval2, field, (shared_size/granularity));
    }

    field = SHARED_RED_LIMITf;
    cur_limit = soc_reg_field_get(unit, reg, rval, field);

    if (_soc_monterey_mmu_config_shared_update_check(unit, cur_limit, shared_size/granularity, flags)){
        soc_reg_field_set(unit, reg, &rval2, field, (shared_size/granularity));
    }
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, idx, rval2));
    return SOC_E_NONE;
}

int
soc_monterey_mmu_config_init(int unit, int test_only, soc_port_t port)
{
    int rv, tport;
    int lossless;
    _soc_mmu_cfg_buf_t *buf;
    _soc_mmu_device_info_t devcfg;
    soc_info_t *si;

    buf = soc_mmu_cfg_alloc(unit);
    if (!buf) {
        return SOC_E_MEMORY;
    }
    si = &SOC_INFO(unit);
    lossless = soc_property_get(unit, spn_MMU_LOSSLESS, 0);

    _soc_monterey_mmu_init_dev_config(unit, &devcfg, lossless);
    _soc_monterey_mmu_config_buf_default(unit, buf, &devcfg, lossless);
    if (soc_property_get(unit, spn_MMU_CONFIG_OVERRIDE, 1) == 0) {
        _soc_mmu_cfg_buf_read(unit, buf, &devcfg);
    }
    rv = _soc_mmu_cfg_buf_check(unit, buf, &devcfg);
    if (!test_only) {
        if (SOC_FAILURE(rv)) {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "MMU config: Use default setting\n")));
            _soc_monterey_mmu_config_buf_default(unit, buf, &devcfg, lossless);
            _soc_mmu_cfg_buf_calculate(unit, buf, &devcfg);
            PBMP_ALL_ITER(unit, tport) {
            buf->pools[0].mcq_entry_reserved += (si->port_num_cosq[tport] *
             _MN_MMU_PER_COSQ_EGRESS_QENTRY_RSVD);
            }
        }
        PBMP_ALL_ITER(unit, tport) {
            buf->pools[0].mcq_entry_reserved += (si->port_num_cosq[tport] *
             _MN_MMU_PER_COSQ_EGRESS_QENTRY_RSVD);
        }
        rv = _soc_monterey_mmu_config_buf_set_hw(unit, buf, &devcfg, lossless, port);
    }

    soc_mmu_cfg_free(unit, buf);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "MMU THDI/THDO init done\n")));
    return rv;
}

STATIC int
_soc_monterey_mmu_init(int unit)
{
    uint32 rval;
    uint64 rval64;
    soc_info_t *si;
    int total_entries, clock_period, refresh_time_val;
    int cpu_slots;
    int msbus_idle_enable;
    int alloc_size;
    int port = 0;


    /*Monterey has fixed 12Mb can not override through OTP */
    _soc_mn_skew_index = 0;

    /* MMU Settings Configuration */
    
    SOC_IF_ERROR_RETURN(soc_monterey_mmu_config_init(unit, 0 /* for testing SAL_BOOT_BCMSIM */, -1));
    /* Enable IP to CMICM credit transfer */
    rval = 0;
    soc_reg_field_set(unit, IP_TO_CMICM_CREDIT_TRANSFERr, &rval,
                      TRANSFER_ENABLEf, 1);
    soc_reg_field_set(unit, IP_TO_CMICM_CREDIT_TRANSFERr, &rval,
                      NUM_OF_CREDITSf, 32);
    SOC_IF_ERROR_RETURN(WRITE_IP_TO_CMICM_CREDIT_TRANSFERr(unit, rval));
    COMPILER_64_ZERO(rval64);
    rval = 0;
    SOC_IF_ERROR_RETURN(
                    READ_VBS_FC_CONFIGr(unit, &rval));
    soc_reg_field_set(unit, VBS_FC_CONFIGr, &rval,
                      FORCE_COSMASK_51_48f , 3);
    SOC_IF_ERROR_RETURN(WRITE_VBS_FC_CONFIGr(unit, rval));

    rval = 0;
    SOC_IF_ERROR_RETURN(
                    READ_TOQ_STATUSr(unit, &rval));
    soc_reg_field_set(unit, TOQ_STATUSr, &rval,
                      FORCE_INIT_DONEf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOQ_STATUSr(unit, rval));

    if (_fwd_ctrl_lock[unit] == NULL) {
        _fwd_ctrl_lock[unit] = sal_mutex_create("_fwd_ctrl_lock");
    }

    if (_fwd_ctrl_lock[unit] == NULL) {
        return SOC_E_MEMORY;
    }

    /*mmu traffic control*/
    alloc_size = sizeof(_soc_monterey_mmu_traffic_ctrl_t);
    if (_soc_monterey_mmu_traffic_ctrl[unit] == NULL) {
        _soc_monterey_mmu_traffic_ctrl[unit] =
            sal_alloc(alloc_size,"_soc_monterey_mmu_traffic_ctrl");
        if (_soc_monterey_mmu_traffic_ctrl[unit] == NULL) {
            return SOC_E_MEMORY;
        }
    }
    sal_memset(_soc_monterey_mmu_traffic_ctrl[unit], 0, alloc_size);


    /* Enable all ports */
    PBMP_ALL_ITER(unit, port)
    {
      if (IS_CPU_PORT(unit, port) ||
          IS_MN_HSP_PORT(unit,port) ||
          IS_MACSEC_PORT(unit,port) ||
          IS_LB_PORT(unit, port))
          { _soc_monterey_thdo_hw_set(unit, port, 1);

       }
    }
    si = &SOC_INFO(unit);

    /*
     * Refresh Value =
     *  ((total no of entities to be refreshed) X Core clk period in ns)/Wred tick period 50 ns
     */

    total_entries = 4096 + 1024;
    cpu_slots = (total_entries * 2) / 16; /* 2 slots for every 16 entries refreshed */
    clock_period = (1 * 1000 * 1000) / si->frequency; /*clock period in microseconds */

    SOC_IF_ERROR_RETURN(READ_WRED_REFRESH_CONTROLr(unit, &rval));
    msbus_idle_enable = soc_reg_field_get(unit, WRED_REFRESH_CONTROLr,
            rval, MSBUS_IDLE_ENABLEf);

    if (msbus_idle_enable) {
        refresh_time_val = ((total_entries + cpu_slots) * clock_period) / 50;
    } else {
        refresh_time_val = (total_entries * clock_period) / 50;
    }
    refresh_time_val = refresh_time_val / 1000;  /* micro to nano seconds */

    soc_reg_field_set(unit, WRED_REFRESH_CONTROLr, &rval,
            REFRESH_TIME_VALf, refresh_time_val);
    soc_reg_field_set(unit, WRED_REFRESH_CONTROLr, &rval,
            REFRESH_DISABLEf, 0);
    SOC_IF_ERROR_RETURN(WRITE_WRED_REFRESH_CONTROLr(unit, rval));

    PBMP_ALL_ITER(unit, port) {
        if (IS_CPU_PORT(unit, port) ||
            IS_LB_PORT(unit, port))
        {
              continue;
        }
        COMPILER_64_ZERO(rval64);
        soc_reg64_field32_set(unit, INTFO_CONGST_STr, &rval64, ENf, 1);
        SOC_IF_ERROR_RETURN(WRITE_INTFO_CONGST_STr(unit, port, rval64));
        rval = 0;
        soc_reg_field_set(unit, THDU_PORT_E2ECC_COS_SPIDr, &rval,
                                                  COS0_E2E_DS_ENf , 1);
        soc_reg_field_set(unit, THDU_PORT_E2ECC_COS_SPIDr, &rval,
                                                  COS1_E2E_DS_ENf , 1);
        soc_reg_field_set(unit, THDU_PORT_E2ECC_COS_SPIDr, &rval,
                                                  COS2_E2E_DS_ENf , 1);
        soc_reg_field_set(unit, THDU_PORT_E2ECC_COS_SPIDr, &rval,
                                                  COS3_E2E_DS_ENf , 1);
        soc_reg_field_set(unit, THDU_PORT_E2ECC_COS_SPIDr, &rval,
                                                  COS4_E2E_DS_ENf , 1);
        soc_reg_field_set(unit, THDU_PORT_E2ECC_COS_SPIDr, &rval,
                                                  COS5_E2E_DS_ENf , 1);
        soc_reg_field_set(unit, THDU_PORT_E2ECC_COS_SPIDr, &rval,
                                                  COS6_E2E_DS_ENf , 1);
        soc_reg_field_set(unit, THDU_PORT_E2ECC_COS_SPIDr, &rval,
                                                  COS7_E2E_DS_ENf , 1);
        WRITE_THDU_PORT_E2ECC_COS_SPIDr(unit , port , rval);
    }

    /*  update port_init_speed array for all ports */
    SOC_IF_ERROR_RETURN(_soc_mn_ports_speed_init_update(unit));

    return SOC_E_NONE;
}


STATIC int
_soc_monterey_age_timer_get(int unit, int *age_seconds, int *enabled)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    uint32 rval;

    if (!soc->l2x_sw_aging) {
        SOC_IF_ERROR_RETURN(READ_L2_AGE_TIMERr(unit, &rval));
        *enabled = soc_reg_field_get(unit, L2_AGE_TIMERr, rval, AGE_ENAf);
        *age_seconds = soc_reg_field_get(unit, L2_AGE_TIMERr, rval, AGE_VALf);
    } else {
        *enabled = soc->l2x_age_pid != SAL_THREAD_ERROR &&
                   soc->l2x_age_enable ?
                   1 : 0;
        *age_seconds = soc->l2x_age_pid != SAL_THREAD_ERROR &&
                       soc->l2x_age_enable ?
                       soc->l2x_age_interval : 0;
    }

    return SOC_E_NONE;
}

STATIC int
_soc_monterey_age_timer_max_get(int unit, int *max_seconds)
{
    soc_control_t *soc = SOC_CONTROL(unit);

    if (!soc->l2x_sw_aging) {
        *max_seconds =
            soc_reg_field_get(unit, L2_AGE_TIMERr, 0xffffffff, AGE_VALf);
    } else {
        *max_seconds = 0x7fffffff;
    }

    return SOC_E_NONE;
}

STATIC int
_soc_monterey_age_timer_set(int unit, int age_seconds, int enable)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    sal_usecs_t interval = soc->l2x_age_interval;
    uint32 rval = 0;

    if (!soc->l2x_sw_aging) {
        soc_reg_field_set(unit, L2_AGE_TIMERr, &rval, AGE_ENAf, enable);
        soc_reg_field_set(unit, L2_AGE_TIMERr, &rval, AGE_VALf, age_seconds);
        SOC_IF_ERROR_RETURN(WRITE_L2_AGE_TIMERr(unit, rval));
    } else if (soc->l2x_age_interval) {
        if (!enable) {
            soc->l2x_age_enable = 0;
        } else {
            if (age_seconds) {
                SOC_CONTROL_LOCK(unit);
                soc->l2x_age_interval = age_seconds;
                SOC_CONTROL_UNLOCK(unit);
            }
            soc->l2x_age_enable = 1;
            if (interval != age_seconds) {
                sal_sem_give(soc->l2x_age_notify);
            }
        }
    } else {
        if (enable) {
            SOC_IF_ERROR_RETURN
                (soc_td2_l2_bulk_age_start(unit, age_seconds));
        }
    }

    return SOC_E_NONE;
}

#define TSC_REG_ADDR_TSCID_SET(_phy_reg, _phyad) \
        ((_phy_reg) |= ((_phyad) & 0x1f) << 19)

#define TSC_REG_ADDR_TSCID_SET(_phy_reg, _phyad) \
        ((_phy_reg) |= ((_phyad) & 0x1f) << 19)

STATIC int
_soc_monterey_mdio_addr_to_port(uint32 phy_addr)
{
    int bus, offset = 0;
    int mdio_addr = (phy_addr & 0x1f);

    /* Must be internal MDIO address */
    if ((phy_addr & 0x80) == 0) {
        return 0;
    }

    bus = PHY_ID_BUS_NUM(phy_addr);
    if (bus > 2) {
        return 0;
    }

    switch (bus) {
    case 0: offset = (mdio_addr < 0xd)? 0 : 40; break;
    case 1: offset = (mdio_addr < 0x9)? 12 : 36; break;
    case 2: offset = 20; break;
    }

    return (mdio_addr + offset);
}

STATIC int
_soc_monterey_mdio_reg_read(int unit, uint32 phy_addr,
                            uint32 phy_reg, uint32 *phy_data)
{
    int port, phy_port, blk;
    soc_mem_t ucmem_data;
    soc_reg_t ucmem_ctrl;

    /* Physical port based on MDIO address */
    phy_port = _soc_monterey_mdio_addr_to_port(phy_addr);
    port = SOC_INFO(unit).port_p2l_mapping[phy_port];
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port ,0);
    if ((((phy_port > 0) && (phy_port < 13)) ||
                 ((phy_port > 52) && (phy_port < 65)))
               && !(IS_ROE_PORT(unit, port))) {
        blk = SOC_PORT_IDX_BLOCK(unit, phy_port ,1);
    }

    if (soc_monterey_port_is_falcon(unit, phy_port)) {
        if ((((phy_port > 0) && (phy_port < 13)) ||
                 ((phy_port > 52) && (phy_port < 65)))
             && (IS_ROE_PORT(unit, port)))  {
            ucmem_data = CPMPORT_WC_UCMEM_DATAm;
            ucmem_ctrl = CLPORT_WC_UCMEM_CTRLr;
        } else {
            ucmem_data = CLPORT_WC_UCMEM_DATAm;
            ucmem_ctrl = CLPORT_WC_UCMEM_CTRLr;
        }
    } else {
        ucmem_data = XLPORT_WC_UCMEM_DATAm;
        ucmem_ctrl = XLPORT_WC_UCMEM_CTRLr;
    }

    LOG_INFO(BSL_LS_SOC_MII,
             (BSL_META_U(unit,
                         "soc_monterey_mdio_reg_read[%d]: %d/%d/%d\n"),
              unit, phy_addr, phy_port, port));

    TSC_REG_ADDR_TSCID_SET(phy_reg, phy_addr);

    /* Call common S-bus MDIO read function */
    return soc_sbus_mdio_reg_read(unit, port, blk, 0,
                                  phy_addr, phy_reg, phy_data,
                                  ucmem_data, ucmem_ctrl);

}

STATIC int
_soc_monterey_mdio_reg_write(int unit, uint32 phy_addr,
                             uint32 phy_reg, uint32 phy_data)
{
    int port, phy_port, blk;
    soc_mem_t ucmem_data;
    soc_reg_t ucmem_ctrl;

    /* Physical port based on MDIO address */
    phy_port = _soc_monterey_mdio_addr_to_port(phy_addr);
    port = SOC_INFO(unit).port_p2l_mapping[phy_port];
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port ,0);
    if ((((phy_port > 0) && (phy_port < 13)) ||
                 ((phy_port > 52) && (phy_port < 65)))
               && !(IS_ROE_PORT(unit, port))) {
     blk = SOC_PORT_IDX_BLOCK(unit, phy_port ,1);
    }

    if (soc_monterey_port_is_falcon(unit, phy_port)) {
        if ((((phy_port > 0) && (phy_port < 13)) ||
                 ((phy_port > 52) && (phy_port < 65)))
                  && (IS_ROE_PORT(unit, port))) {
            ucmem_data = CPMPORT_WC_UCMEM_DATAm;
            ucmem_ctrl = CLPORT_WC_UCMEM_CTRLr;
        } else {
            ucmem_data = CLPORT_WC_UCMEM_DATAm;
            ucmem_ctrl = CLPORT_WC_UCMEM_CTRLr;
        }
    } else {
        ucmem_data = XLPORT_WC_UCMEM_DATAm;
        ucmem_ctrl = XLPORT_WC_UCMEM_CTRLr;
    }

    LOG_INFO(BSL_LS_SOC_MII,
             (BSL_META_U(unit,
                         "soc_monterey_mdio_reg_read[%d]: %d/%d/%d\n"),
              unit, phy_addr, phy_port, port));

    TSC_REG_ADDR_TSCID_SET(phy_reg, phy_addr);


    /* Call common S-bus MDIO read function */
    return soc_sbus_mdio_reg_write(unit, port, blk, 0,
                                   phy_addr, phy_reg, phy_data,
                                   ucmem_data, ucmem_ctrl);
}

static const soc_reg_t pvtmon_result_reg[] = {
    TOP_PVTMON_RESULT_0r, TOP_PVTMON_RESULT_1r,
    TOP_PVTMON_RESULT_2r, TOP_PVTMON_RESULT_3r,
    TOP_PVTMON_RESULT_4r, TOP_PVTMON_RESULT_5r,
    TOP_PVTMON_RESULT_6r, TOP_PVTMON_RESULT_7r,
};


STATIC void
soc_monterey_temperature_intr(int unit)
{
    uint32 rval, trval;
    int i, rv;
    soc_port_t port;
    soc_block_t blk;
    soc_block_t xlport_blocks[] = {SOC_BLK_XLPORT, SOC_BLOCK_TYPE_INVALID};
    soc_block_t clport_blocks[] = {SOC_BLK_CLPORT, SOC_BLOCK_TYPE_INVALID};
/*FIXME CPRI HANDLE
 */
    /* Raise event to app for it to take further graceful actions */
    for (i = 0; i < COUNTOF(pvtmon_result_reg); i++) {
        if ((0x1 << i)  & _soc_mn_temp_mon_mask[unit]) {
            if ((rv = soc_reg32_get(unit, pvtmon_result_reg[i], REG_PORT_ANY, 0,
                                    &trval)) != SOC_E_NONE) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "MN Top Intr, Reg access error.\n")));
            }
            trval = soc_reg_field_get(unit, pvtmon_result_reg[i], trval,
                                      PVT_DATAf);
            /* Convert data to temperature.
             * temp = 410.04-(data*0.48705) = (410040-(data*487))/1000
             * Note: Since this is a high temp interrupt we can safely assume
             * that this will end up being a +ive value.
             */
            trval = (410040-(trval*487))/1000;
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "TempMon %d: %d deg C.\n"), i, trval));
            (void)soc_event_generate(unit, SOC_SWITCH_EVENT_ALARM,
                                     SOC_SWITCH_EVENT_ALARM_HIGH_TEMP, i, trval);
        }
    }

    /* Optionally hold system in reset.
     * Note: The main intention is to stop the chip from getting fried and halt
     *       all schan accesses as the h/w will not respond anymore.
     *       We are not implementing a gracefull recovery, the unit needs to be
     *       rebooted after this.
     */
    if (soc_property_get(unit, "temp_monitor_shutdown", 1)) {
        if ((rv = WRITE_TOP_PVTMON_INTR_MASKr(unit, 0)) != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "MN Temp Intr, Reg access error.\n")));
        }

        /* Stop all schan transactions on this unit */
#ifdef  INCLUDE_I2C
        if ((rv = soc_i2c_detach(unit)) != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "MN Temp Intr, i2c detach error.\n")));
        }
#endif
        if (!SOC_IS_RCPU_ONLY(unit)) {
            /* Free up DMA memory */
            if ((rv = soc_dma_detach(unit)) != SOC_E_NONE) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "MN Temp Intr, dma detach error.\n")));
            }
        }
#ifdef INCLUDE_MEM_SCAN
        if ((rv = soc_mem_scan_stop(unit)) != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "MN Temp Intr, mem scan stop error.\n")));
        }
#endif /* INCLUDE_MEM_SCAN */
        /* Terminate counter module */
        if ((rv = soc_counter_stop(unit)) != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "MN Temp Intr, counter stop error.\n")));
        }
        if (SOC_SBUSDMA_DM_INFO(unit)) {
            if ((rv = soc_sbusdma_desc_detach(unit)) != SOC_E_NONE) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "MN Temp Intr, sbusdma stop error.\n")));
            }
        }
        if (soc_feature(unit, soc_feature_arl_hashed)) {
            /* Stop L2X thread */
            if ((rv = soc_l2x_stop(unit)) != SOC_E_NONE) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "MN Temp Intr, L2x stop error.\n")));
            }

        }

        rv = READ_TOP_SOFT_RESET_REGr(unit, &rval);
        if (SOC_E_NONE != rv) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "MN Temp Intr, Reg access error.\n")));
        }
        soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_TS_RST_Lf, 0);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MMU_RST_Lf, 0);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_EP_RST_Lf, 0);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_IP_RST_Lf, 0);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_RDB_RST_Lf, 0);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_PGW0_RST_Lf, 0);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_PGW1_RST_Lf, 0);
        rv = WRITE_TOP_SOFT_RESET_REGr(unit, rval);
        if (SOC_E_NONE != rv) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "MN Temp Intr, Reg access error.\n")));
        }

        if ((rv = READ_TOP_SOFT_RESET_REG_2r(unit, &rval)) != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "MN Temp Intr, Reg access error.\n")));
        }
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_XG_PLL0_RST_Lf,
                          0);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_XG_PLL1_RST_Lf,
                          0);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_XG_PLL2_RST_Lf,
                          0);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_XG_PLL3_RST_Lf,
                          0);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_TS_PLL_RST_Lf,
                          1);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_BS_PLL0_RST_Lf,
                          0);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_BS_PLL1_RST_Lf,
                          0);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_AVS_RST_Lf, 1);
        if ((rv = WRITE_TOP_SOFT_RESET_REG_2r(unit, rval)) != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "MN Temp Intr, Reg access error.\n")));
        }
        /* Powerdown Falcon analog cores */
        SOC_BLOCKS_ITER(unit, blk, clport_blocks) {
            port = SOC_BLOCK_PORT(unit, blk);
            rv = READ_CLPORT_XGXS0_CTRL_REGr(unit, port, &rval);
            if (SOC_E_NONE != rv) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "MN Temp Intr, Reg access error.\n")));
            }
            soc_reg_field_set(unit, CLPORT_XGXS0_CTRL_REGr, &rval, IDDQf, 1);
            soc_reg_field_set(unit, CLPORT_XGXS0_CTRL_REGr, &rval, PWRDWNf, 1);
            soc_reg_field_set(unit, CLPORT_XGXS0_CTRL_REGr, &rval, RSTB_HWf, 0);
            rv = WRITE_CLPORT_XGXS0_CTRL_REGr(unit, port, rval);
            if (SOC_E_NONE != rv) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "MN Temp Intr, Reg access error.\n")));
            }
        }
        /* Powerdown Eagle analog cores */
        SOC_BLOCKS_ITER(unit, blk, xlport_blocks) {
            port = SOC_BLOCK_PORT(unit, blk);
            rv = READ_XLPORT_XGXS0_CTRL_REGr(unit, port, &rval);
            if (SOC_E_NONE != rv) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "MN Temp Intr, Reg access error.\n")));
            }
            soc_reg_field_set(unit, XLPORT_XGXS0_CTRL_REGr, &rval, IDDQf, 1);
            soc_reg_field_set(unit, XLPORT_XGXS0_CTRL_REGr, &rval, PWRDWNf, 1);
            soc_reg_field_set(unit, XLPORT_XGXS0_CTRL_REGr, &rval, RSTB_HWf, 0);
            rv = WRITE_XLPORT_XGXS0_CTRL_REGr(unit, port, rval);
            if (SOC_E_NONE != rv) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "MN Temp Intr, Reg access error.\n")));
            }
        }

        /* Disable all interrupts */
        soc_cmicm_intr0_disable(unit, ~0);
        soc_cmicm_intr1_disable(unit, ~0);
        soc_cmicm_intr2_disable(unit, ~0);
        soc_cmicm_intr3_disable(unit, ~0);
        soc_cmicm_intr4_disable(unit, ~0);
        soc_cmicm_intr5_disable(unit, ~0);

        /* Power down core PLL, once this is done all SCHAN accesses
         * will fail.
         */
        rv = READ_TOP_CORE_PLL_CTRL_1r(unit, &rval);
        if (SOC_E_NONE != rv) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "MN Temp Intr, Reg access error.\n")));
        }
        soc_reg_field_set(unit, TOP_CORE_PLL_CTRL_1r, &rval, ISO_INf, 1);
        soc_reg_field_set(unit, TOP_CORE_PLL_CTRL_1r, &rval, PWRONf, 0);
        soc_reg_field_set(unit, TOP_CORE_PLL_CTRL_1r, &rval, PWRON_LDOf, 0);
        WRITE_TOP_CORE_PLL_CTRL_1r(unit, rval);
        SCHAN_LOCK(unit);

        /* Clear all outstanding DPCs owned by this unit */
        sal_dpc_cancel(INT_TO_PTR(unit));
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "\nReboot the system..")));
    }
}


void
soc_monterey_top_intr(void *unit_vp, void *d1, void *d2,
                                   void *d3, void *d4)
{
    uint32 rval, pvtrval;
    int rv, unit = PTR_TO_INT(unit_vp);

    if ((rv = READ_TOP_INTR_STATUSr(unit, &rval)) != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "MN TOP Intr, Reg access error.\n")));
    }

    /* Bits 0-17 for PVTMON and rest for AVS */
    pvtrval = rval & ((1 << 18) - 1);
    if (pvtrval) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "High temp interrupt: 0x%08x\n"), rval));

        soc_monterey_temperature_intr(unit);
        return;
    }

    

    return;
}

int
soc_monterey_temperature_monitor_get(int unit,
            int temperature_max,
            soc_switch_temperature_monitor_t *temperature_array,
            int *temperature_count)
{
    soc_reg_t reg;
    int index;
    uint32 rval;
    int fval, cur, peak,valid = 0 ,done = 0;
    int num_entries_out;
    soc_timeout_t to;

    if (temperature_count) {
        *temperature_count = 0;
    }
    if (COUNTOF(pvtmon_result_reg) > temperature_max) {
        num_entries_out = temperature_max;
    } else {
        num_entries_out = COUNTOF(pvtmon_result_reg);
    }


    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TOP_PVTMON_CTRL_1r, REG_PORT_ANY,
                                   PVTMON_SELECTf, 0));
     /* Sequence for AVS PVTMON measurements starts here
     * Needed before reading Monitor #8: TOP_PVTMON_RESULT_8
     */
    /* Stop on going measurements */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, AVS_REG_HW_MNTR_SEQUENCER_INITr, REG_PORT_ANY,
                                SEQUENCER_INITf, 1));
    /* Clear all measurements */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, AVS_REG_HW_MNTR_MEASUREMENTS_INIT_PVT_MNTRr, REG_PORT_ANY,
                                M_INIT_PVT_MNTRf, 0xff));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, AVS_REG_HW_MNTR_MEASUREMENTS_INIT_PVT_MNTRr, REG_PORT_ANY,
                                M_INIT_PVT_MNTRf, 0x0));

    /* Restart measurements */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, AVS_REG_HW_MNTR_SEQUENCER_INITr, REG_PORT_ANY,
                                SEQUENCER_INITf, 0));

    soc_timeout_init(&to, 1500, 1);
    /* Poll until valid bit is set */
    for (;;) {
        SOC_IF_ERROR_RETURN(READ_AVS_REG_RO_REGISTERS_0_PVT_TEMPERATURE_MNTR_STATUSr(unit,
                            &rval));
        valid = soc_reg_field_get(unit, AVS_REG_RO_REGISTERS_0_PVT_TEMPERATURE_MNTR_STATUSr,
                                  rval, VALID_DATAf);
        done = soc_reg_field_get(unit, AVS_REG_RO_REGISTERS_0_PVT_TEMPERATURE_MNTR_STATUSr,
                                 rval, DONEf);
        if (valid && done) {
            break;
        }
        if (soc_timeout_check(&to)) {
            LOG_ERROR(BSL_LS_SOC_DIAG,
                      (BSL_META_U(unit, "AVS Temperature Monitor: Timeout\n")));
            return SOC_E_INTERNAL;
        }
    }

    /* Sequence for AVS PVTMON measurements end here */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TOP_PVTMON_CTRL_1r, REG_PORT_ANY,
            PVTMON_ADC_RESETBf, 1));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TOP_PVTMON_CTRL_1r, REG_PORT_ANY,
            PVTMON_ADC_RESETBf, 0));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TOP_PVTMON_CTRL_1r, REG_PORT_ANY,
            PVTMON_ADC_RESETBf, 1));


    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_PVT_MON_MAX_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_PVT_MON_MIN_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));

    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_PVT_MON_MAX_RST_Lf, 0);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_PVT_MON_MIN_RST_Lf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));

    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_PVT_MON_MAX_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_PVT_MON_MIN_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));
    sal_usleep(1000);

    for (index = 0; index < num_entries_out; index++) {
        reg = pvtmon_result_reg[index];
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));

        fval = soc_reg_field_get(unit, reg, rval, PVT_DATAf);
        cur = (4100400 - (4870 * fval)) / 1000;
        fval = soc_reg_field_get(unit, reg, rval, MIN_PVT_DATAf);
        peak = (4100400 - (4870 * fval)) / 1000;
        (temperature_array + index)->curr = cur;
        (temperature_array + index)->peak    = peak;
    }
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_PVT_MON_MAX_RST_Lf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_PVT_MON_MAX_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));

    if (temperature_count) {
        *temperature_count=num_entries_out;
    }

    return SOC_E_NONE;
}

int
soc_monterey_show_material_process(int unit)
{
    soc_reg_t reg;
    int index;
    uint32 rval, fval, nmos[COUNTOF(pvtmon_result_reg)], n_avg, p_avg;

    READ_TOP_PVTMON_CTRL_1r(unit, &rval);
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON_PWRDNf, 0);
    WRITE_TOP_PVTMON_CTRL_1r(unit, rval);
    sal_usleep(1000);
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON_PWRDNf, 1);
    WRITE_TOP_PVTMON_CTRL_1r(unit, rval);
    sal_usleep(1000);

    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TOP_PVTMON_CTRL_1r, REG_PORT_ANY,
                                PVTMON_SELECTf, 1));
    sal_usleep(1000);

    p_avg = 0;

    /* Read NMOS information */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TOP_PVTMON_CTRL_0r, REG_PORT_ANY,
                                MEASUREMENT_CALLIBRATIONf, 5));

    sal_usleep(1000);

    n_avg = 0;
    for (index = 0; index < COUNTOF(pvtmon_result_reg); index++) {
        reg = pvtmon_result_reg[index];
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        nmos[index] = soc_reg_field_get(unit, reg, rval, PVT_DATAf);
        n_avg += nmos[index];
    }

    /* Read PMOS information and print both NMOS and PMOS value */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TOP_PVTMON_CTRL_0r, REG_PORT_ANY,
                                MEASUREMENT_CALLIBRATIONf, 7));

    sal_usleep(1000);

    p_avg = 0;
    for (index = 0; index < COUNTOF(pvtmon_result_reg); index++) {
        reg = pvtmon_result_reg[index];
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        fval = soc_reg_field_get(unit, reg, rval, PVT_DATAf);
        p_avg += fval;

        LOG_CLI((BSL_META_U(unit,
                   "Material process location %d: NMOS = %3d PMOS = %3d\n"),
                   index, nmos[index], fval));
    }

    LOG_CLI((BSL_META_U(unit,
               "Average:                     NMOS = %3d PMOS = %3d\n"),
               n_avg / COUNTOF(pvtmon_result_reg),
               p_avg / COUNTOF(pvtmon_result_reg)));

    return SOC_E_NONE;
}

int
soc_monterey_show_ring_osc(int unit)
{
    static const struct {
        int osc_sel;
        soc_field_t field0;
        int value0;
        soc_field_t field1;
        int value1;
        char *name;
    } osc_tbl[] = {
        { 2, IROSC_SELf, 0, INVALIDf, -1, "Core ring 0 five stages" },
        { 2, IROSC_SELf, 1, INVALIDf, -1, "Core ring 0 nine stages" },
        { 3, IROSC_SELf, 0, INVALIDf, -1, "Core ring 1 five stages" },
        { 3, IROSC_SELf, 1, INVALIDf, -1, "Core ring 1 nine stages" },
        { 4, SRAM_OSC_0_PENf, 0, SRAM_OSC_0_NENf, 1, "SRAM ring 0 NMOS" },
        { 5, SRAM_OSC_0_PENf, 1, SRAM_OSC_0_NENf, 0, "SRAM ring 0 PMOS" },
        { 6, SRAM_OSC_1_PENf, 0, SRAM_OSC_1_NENf, 1, "SRAM ring 1 NMOS" },
        { 7, SRAM_OSC_1_PENf, 1, SRAM_OSC_1_NENf, 0, "SRAM ring 1 PMOS" },
        { 8, SRAM_OSC_2_PENf, 0, SRAM_OSC_2_NENf, 1, "SRAM ring 2 NMOS" },
        { 9, SRAM_OSC_2_PENf, 1, SRAM_OSC_2_NENf, 0, "SRAM ring 2 PMOS" },
        { 10, SRAM_OSC_3_PENf, 0, SRAM_OSC_3_NENf, 1, "SRAM ring 3 NMOS" },
        { 11, SRAM_OSC_3_PENf, 1, SRAM_OSC_3_NENf, 0, "SRAM ring 3 PMOS" }
    };
    soc_reg_t ctrl_reg, stat_reg;
    uint32 rval, fval;
    int index, core_clk, quo, rem, frac, retry;

    core_clk = SOC_INFO(unit).frequency * 1024;
    ctrl_reg = TOP_RING_OSC_CTRLr;
    stat_reg = TOP_OSC_COUNT_STATr;

    for (index = 0; index < COUNTOF(osc_tbl); index++) {
        rval = 0;
        /*
         * set OSC_CNT_RSTBf to 0 to do softreset
         * set OSC_CNT_START to 0 to hold the counter until it selects
         * the input signal
         */
        SOC_IF_ERROR_RETURN(WRITE_TOP_RING_OSC_CTRLr(unit, rval));
        soc_reg_field_set(unit, ctrl_reg, &rval, OSC_ENABLEf, 1);
        soc_reg_field_set(unit, ctrl_reg, &rval, IROSC_ENf, 1);
        soc_reg_field_set(unit, ctrl_reg, &rval, osc_tbl[index].field0,
                          osc_tbl[index].value0);
        if (osc_tbl[index].field1 != INVALIDf) {
            soc_reg_field_set(unit, ctrl_reg, &rval, osc_tbl[index].field1,
                              osc_tbl[index].value1);
        }
        soc_reg_field_set(unit, ctrl_reg, &rval, OSC_SELf,
                          osc_tbl[index].osc_sel);
        SOC_IF_ERROR_RETURN(WRITE_TOP_RING_OSC_CTRLr(unit, rval));
        soc_reg_field_set(unit, ctrl_reg, &rval, OSC_CNT_RSTBf, 1);
        SOC_IF_ERROR_RETURN(WRITE_TOP_RING_OSC_CTRLr(unit, rval));
        soc_reg_field_set(unit, ctrl_reg, &rval, OSC_CNT_STARTf, 1);
        SOC_IF_ERROR_RETURN(WRITE_TOP_RING_OSC_CTRLr(unit, rval));

        for (retry = 0; retry < 10; retry++) {
            sal_usleep(1000);
            SOC_IF_ERROR_RETURN(READ_TOP_OSC_COUNT_STATr(unit, &rval));
            if (!soc_reg_field_get(unit, stat_reg, rval, OSC_CNT_DONEf)) {
                continue;
            }

            fval = soc_reg_field_get(unit, stat_reg, rval, OSC_CNT_VALUEf);
            quo = core_clk / fval;
            rem = core_clk - quo * fval;
            frac = (rem * 10000) / fval;
            LOG_CLI((BSL_META_U(unit, "%s: %d.%04d Mhz\n"),
                       osc_tbl[index].name, quo, frac));
            break;
        }
    }

    return SOC_E_NONE;
}

int
soc_monterey_show_voltage(int unit)
{
    soc_reg_t reg;
    int index;
    uint32 rval, fval, avg;

    SOC_IF_ERROR_RETURN(READ_TOP_PVTMON_CTRL_1r(unit, &rval));
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON_ADC_RESETBf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_CTRL_1r(unit, rval));
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON_ADC_RESETBf, 1);
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_1r, &rval, PVTMON_SELECTf, 4);
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_CTRL_1r(unit, rval));
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_AVS_PVTMON_RST_Lf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_AVS_PVTMON_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));

    sal_sleep(1);
    avg = 0;

    /* Read Voltages. Ignores result of PVTMON8 */
    for (index = 0; index < COUNTOF(pvtmon_result_reg)  ; index++) {
        reg = pvtmon_result_reg[index];
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        fval = soc_reg_field_get(unit, reg, rval, PVT_DATAf);
        /* (PVT_DATA * 880) / 1024 * 0.7 */
        fval = (fval * 880 * 10) / (1024 * 7);
        avg += fval;
        LOG_CLI((BSL_META_U(unit,
                            "Voltage monitor %d: voltage = %d.%03dV\n"),
                 index, (fval/1000), (fval %1000)));
    }

    avg /= (COUNTOF(pvtmon_result_reg) );
    LOG_CLI((BSL_META_U(unit,
                        "Average voltage is = %d.%03dV\n"),
             (avg/1000), (avg %1000)));

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_monterey_process_func_intr
 * Description:
 *      Chip specific DPC function to service interrupts
 * Parameters:
 *      unit_vp       - Device number
 *      d1            - IRQ3 mask
 *      d2            - IRQ3 number (to distinguish between
 *                      different types of interrupts).
 */
void
soc_monterey_process_func_intr(void *unit_vp, void *d1, void *d2, void *d3, void *d4)
{
    int unit = PTR_TO_INT(unit_vp);
    int irq3_num = PTR_TO_INT(d2);
    uint32 rval;

    if ((irq3_num == 13) /* L2LU INTR */ &&
        soc_feature(unit, soc_feature_l2_overflow)) {
        if (READ_IL2LU_INTR_STATUSr(unit, &rval)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "unit %d: Error reading %s reg !!\n"),
                                  unit, SOC_REG_NAME(unit, IL2LU_INTR_STATUSr)));
            return;
        }
        if (soc_reg_field_get(unit, IL2LU_INTR_STATUSr, rval,
                              L2_LEARN_INSERT_FAILUREf)) {
            soc_td2_l2_overflow_interrupt_handler(unit);
        }
    }

    sal_usleep(SAL_BOOT_QUICKTURN ? 100000 : 1000); /* Don't reenable too soon */
    if (d1 != NULL) {
        (void)soc_cmicm_intr3_enable(unit,
                                     PTR_TO_INT(d1));
    }

    return;
}

int
soc_monterey_port_asf_speed_set(int unit, soc_port_t port, int speed)
{
    soc_monterey_port_ct_speed_t asf_speed_mode;
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_ASF_PORT_CFGr(unit, port, &rval));

    switch (speed) {
        case 10:
            asf_speed_mode = SOC_MONTEREY_PORT_CT_SPEED_10M_FULL;
            break;
        case 100:
            asf_speed_mode = SOC_MONTEREY_PORT_CT_SPEED_100M_FULL;
            break;
        case 1000:
            asf_speed_mode = SOC_MONTEREY_PORT_CT_SPEED_1000M_FULL;
            break;
        case 2500:
            asf_speed_mode = SOC_MONTEREY_PORT_CT_SPEED_2500M_FULL;
            break;
        case 5000:
            asf_speed_mode = SOC_MONTEREY_PORT_CT_SPEED_5000M_FULL;
            break;
        case 10000:
            asf_speed_mode = SOC_MONTEREY_PORT_CT_SPEED_10000M_FULL;
            break;
        case 11000:
            asf_speed_mode = SOC_MONTEREY_PORT_CT_SPEED_11000M_FULL;
            break;
        case 20000:
            asf_speed_mode = SOC_MONTEREY_PORT_CT_SPEED_20000M_FULL;
            break;
        case 21000:
            asf_speed_mode = SOC_MONTEREY_PORT_CT_SPEED_21000M_FULL;
            break;
        case 25000:
            asf_speed_mode = SOC_MONTEREY_PORT_CT_SPEED_25000M_FULL;
            break;
        case 27000:
            asf_speed_mode = SOC_MONTEREY_PORT_CT_SPEED_27000M_FULL;
            break;
        case 40000:
            asf_speed_mode = SOC_MONTEREY_PORT_CT_SPEED_40000M_FULL;
            break;
        case 42000:
            asf_speed_mode = SOC_MONTEREY_PORT_CT_SPEED_42000M_FULL;
            break;
        case 50000:
            asf_speed_mode = SOC_MONTEREY_PORT_CT_SPEED_50000M_FULL;
            break;
        case 53000:
            asf_speed_mode = SOC_MONTEREY_PORT_CT_SPEED_53000M_FULL;
            break;
        case 100000:
            asf_speed_mode = SOC_MONTEREY_PORT_CT_SPEED_100000M_FULL;
            break;
        case 106000:
            asf_speed_mode = SOC_MONTEREY_PORT_CT_SPEED_106000M_FULL;
            break;
        case 0:
            return SOC_E_NONE;
        default:
            return SOC_E_PARAM;
    }

    soc_reg_field_set(unit, ASF_PORT_CFGr, &rval,
                      ASF_PORT_SPEEDf, asf_speed_mode);

    SOC_IF_ERROR_RETURN(WRITE_ASF_PORT_CFGr(unit, port, rval));
    return SOC_E_NONE;
}


int
soc_monterey_port_asf_set(int unit, soc_port_t port, int speed)
{
    uint32 rval;
    soc_reg_t reg;
    int mmu_port, phy_port;
    int asf_credit_thresh_lo, asf_credit_thresh_hi;
    int ep_credit_size = 0, pfc_watermark = 0;
    egr_pfc_control_entry_t pfc_control;
    egr_mmu_credit_limit_entry_t egr_mmu_credit_entry;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    mmu_port = SOC_INFO(unit).port_p2m_mapping[phy_port];
if (SOC_IS_MONTEREY(unit)) {
    return SOC_E_NONE;

}
    
    if (!soc_property_port_get(unit, port, "pfc_optimized_settings", 1)) {
        if (speed <= 11000) {
            asf_credit_thresh_lo = 4;
            asf_credit_thresh_hi = 9;
        } else if (speed <= 21000) {
            asf_credit_thresh_lo = 8;
            asf_credit_thresh_hi = 18;
        } else if (speed <= 42000) {
            asf_credit_thresh_lo = 18;
            asf_credit_thresh_hi = 36;
        } else {
            asf_credit_thresh_lo = 30;
            asf_credit_thresh_hi = 72;
        }
    } else {
        if (speed <= 11000) {
            asf_credit_thresh_lo = 3;
            asf_credit_thresh_hi = 7;
            ep_credit_size = IS_OVERSUB_PORT(unit,port) ? 7 : 11;
            pfc_watermark = IS_OVERSUB_PORT(unit,port) ? 13 : 19;
        } else if (speed <= 27000) {
            asf_credit_thresh_lo = 4;
            asf_credit_thresh_hi = 12;
            ep_credit_size = IS_OVERSUB_PORT(unit,port) ? 11 : 16;
            pfc_watermark = IS_OVERSUB_PORT(unit,port) ? 25 : 19;
        } else if (speed <= 42000) {
            asf_credit_thresh_lo = 5;
            asf_credit_thresh_hi = 16;
            ep_credit_size = IS_OVERSUB_PORT(unit,port) ? 23 : 20;
            pfc_watermark = 31;
        } else if (speed <= 53000) {
            asf_credit_thresh_lo = 5;
            asf_credit_thresh_hi = 17;
            ep_credit_size = IS_OVERSUB_PORT(unit,port) ? 17 : 23;
            pfc_watermark = IS_OVERSUB_PORT(unit,port) ? 37 : 31;
        } else {
            asf_credit_thresh_lo = 7;
            asf_credit_thresh_hi = 30;
            ep_credit_size = IS_OVERSUB_PORT(unit,port) ? 30 : 39;
            pfc_watermark = 61;
        }
    }

    ep_credit_size = ep_credit_size - 1;
    rval = 0;
    reg = ES_PIPE0_ASF_CREDIT_THRESH_LOr;
    soc_reg_field_set(unit, reg, &rval, THRESHf, asf_credit_thresh_lo);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, mmu_port, rval));

    rval = 0;
    reg = ES_PIPE0_ASF_CREDIT_THRESH_HIr;
    soc_reg_field_set(unit, reg, &rval, THRESHf, asf_credit_thresh_hi);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, mmu_port, rval));
    SOC_IF_ERROR_RETURN(soc_monterey_port_asf_speed_set(unit, port, speed));


    
    if (soc_property_port_get(unit, port, "pfc_optimized_settings", 1)) {

        /* EGR_PFC_CONTROL */
        sal_memset(&pfc_control, 0, sizeof(egr_pfc_control_entry_t));

        soc_mem_field32_set(unit, EGR_PFC_CONTROLm, &pfc_control, ENABLEf, 1);
        soc_mem_field32_set(unit, EGR_PFC_CONTROLm, &pfc_control, WATERMARKf, pfc_watermark);

        SOC_IF_ERROR_RETURN(WRITE_EGR_PFC_CONTROLm(unit, MEM_BLOCK_ALL, phy_port, &pfc_control));

        /* EGR_MMU_CREDIT_LIMIT */
        sal_memset(&egr_mmu_credit_entry, 0, sizeof(egr_mmu_credit_limit_entry_t));

        soc_mem_field32_set(unit, EGR_MMU_CREDIT_LIMITm, &egr_mmu_credit_entry, ENABLEf, 1);
        soc_mem_field32_set(unit, EGR_MMU_CREDIT_LIMITm, &egr_mmu_credit_entry, CELL_COUNTSf, ep_credit_size);

        SOC_IF_ERROR_RETURN(WRITE_EGR_MMU_CREDIT_LIMITm(unit, MEM_BLOCK_ALL, phy_port, &egr_mmu_credit_entry));
    }

    return SOC_E_NONE;
}

int
soc_monterey_port_speed_update(int unit, soc_port_t port, int speed)
{
    soc_pbmp_t pbm;
    int lossless;

    /* Update Edatabuf transmit start count */
    SOC_PBMP_CLEAR(pbm);
    SOC_PBMP_PORT_ADD(pbm, port);
    SOC_IF_ERROR_RETURN(soc_monterey_ports_edb_config(unit, 0, pbm));

    SOC_IF_ERROR_RETURN(soc_monterey_port_asf_set(unit, port, speed));

    /* Update oversubscription buffer manager (OBM) threshold */
    lossless = soc_property_get(unit, spn_MMU_LOSSLESS, 0);
    SOC_IF_ERROR_RETURN(_soc_monterey_idb_port_init(unit, FALSE, lossless, port));

    SOC_IF_ERROR_RETURN(soc_mn_mmu_delay_insertion_set(unit, port));

    return SOC_E_NONE;
}



STATIC int
_soc_monterey_thdo_hw_set(int unit, soc_port_t port, int enable)
{
    uint64 rval64, rval64_tmp;
    int mmu_port, phy_port, i,mmu_port_tmp;
    soc_info_t *si;
    soc_reg_t reg_tmp;
    soc_reg_t reg[3][2] = {
        {
            THDU_OUTPUT_PORT_RX_ENABLE0_64r,
            THDU_OUTPUT_PORT_RX_ENABLE1_64r
        },
        {
            MMU_THDM_DB_PORTSP_RX_ENABLE0_64r,
            MMU_THDM_DB_PORTSP_RX_ENABLE1_64r
        },
        {
            MMU_THDM_MCQE_PORTSP_RX_ENABLE0_64r,
            MMU_THDM_MCQE_PORTSP_RX_ENABLE1_64r
        }
    };

    si = &SOC_INFO(unit);

    phy_port = si->port_l2p_mapping[port];
    mmu_port = si->port_p2m_mapping[phy_port];
    if (mmu_port > 67) {
        return SOC_E_PARAM;
    }

    for (i = 0; i < 3; i++) {
        COMPILER_64_ZERO(rval64);
        COMPILER_64_ZERO(rval64_tmp);
        if (mmu_port < 64) {
            reg_tmp = reg[i][0];
            mmu_port_tmp = mmu_port;
        } else {
            if (i == 0) {
                continue;
            }
            reg_tmp = reg[i][1];
            mmu_port_tmp = mmu_port - 64;
        }

        SOC_IF_ERROR_RETURN(
            soc_reg_get(unit, reg_tmp, REG_PORT_ANY, 0, &rval64));

        if (mmu_port_tmp < 32) {
            COMPILER_64_SET(rval64_tmp, 0, 1 << mmu_port_tmp);
        } else {
            COMPILER_64_SET(rval64_tmp, 1 << (mmu_port_tmp - 32), 0);
        }

        if (enable) {
            COMPILER_64_OR(rval64, rval64_tmp);
        } else {
            COMPILER_64_NOT(rval64_tmp);
            COMPILER_64_AND(rval64, rval64_tmp);
        }

        SOC_IF_ERROR_RETURN(
            soc_reg_set(unit, reg_tmp, REG_PORT_ANY, 0, rval64));
    }

    return SOC_E_NONE;
}
STATIC int
_soc_monterey_thdo_hw_get(int unit, soc_port_t port, int *enable)
{
    uint64 rval64;
    int mmu_port, phy_port, i, reg_grp = 0;
    int rv = SOC_E_NONE;
    soc_info_t *si;
    soc_reg_t reg_tmp;
    soc_reg_t reg[3][2] = {
        {
            THDU_OUTPUT_PORT_RX_ENABLE0_64r,
            THDU_OUTPUT_PORT_RX_ENABLE1_64r
        },
        {
            MMU_THDM_DB_PORTSP_RX_ENABLE0_64r,
            MMU_THDM_DB_PORTSP_RX_ENABLE1_64r
        },
        {
            MMU_THDM_MCQE_PORTSP_RX_ENABLE0_64r,
            MMU_THDM_MCQE_PORTSP_RX_ENABLE1_64r
        }
    };
    int max_reg = 3;

    si = &SOC_INFO(unit);

    phy_port = si->port_l2p_mapping[port];
    mmu_port = si->port_p2m_mapping[phy_port];

    COMPILER_64_ZERO(rval64);
    if (mmu_port  < 64 ) {
        mmu_port &= 0x3f;
    } else {
        mmu_port &= 0x7;
        reg_grp = 1;
    }

    for (i = 0; i < max_reg; i++) {
        reg_tmp = reg[i][reg_grp];

        SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg_tmp, REG_PORT_ANY, 0, &rval64));

        if (COMPILER_64_BITTEST(rval64 , mmu_port)) {
            *enable = TRUE;
        } else {
            *enable = FALSE;
        }
    }
    return rv;
}

/*
 * Function:
 *      _soc_monterey_port_thdo_rx_enable_set
 * Purpose:
 *     enable/disable  the traffic to the port according to
 *     different callers.
 *  the traffic to the port can be enabled only if all of the callers
 *  are to enable it.
 * Parameters:
 *     unit                - (IN) unit number
 *     port                - (IN) local port
 *     enable              - (IN) enable/disable the traffic to the port.
 *     flag                - (IN) different callers.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_soc_monterey_port_thdo_rx_enable_set(int unit, soc_port_t port,
                                 int enable, uint8 flag)
{
    int   rv = SOC_E_NONE;
    uint8 *thdo_drop_bmp;
    _soc_monterey_mmu_traffic_ctrl_t *mmu_traffic_ctrl;
    int cur_enable =0 ;
    FWD_CTRL_LOCK(unit);

    mmu_traffic_ctrl = _soc_monterey_mmu_traffic_ctrl[unit];
    if (mmu_traffic_ctrl ==  NULL) {
        FWD_CTRL_UNLOCK(unit);
        return SOC_E_INIT;
    }
    thdo_drop_bmp =
        &(mmu_traffic_ctrl->thdo_drop_bmp[port]);

    if (!enable) {
        if (*thdo_drop_bmp & flag) {
            FWD_CTRL_UNLOCK(unit);
            return rv;
        }

        if (*thdo_drop_bmp == 0) {
            rv = _soc_monterey_thdo_hw_set(unit, port, enable);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,(BSL_META_U(unit, "Clearing the registers failed.\n")));
                FWD_CTRL_UNLOCK(unit);
                return rv;
            }
        }

        *thdo_drop_bmp |= flag;

        FWD_CTRL_UNLOCK(unit);
        return rv;
    } else {
        if (0 == *thdo_drop_bmp) {
            rv = _soc_monterey_thdo_hw_get(unit, port, &cur_enable);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Get the registers fail.\n")));
                FWD_CTRL_UNLOCK(unit);
                return rv;
            }
            /* Warmboot: latest value of *thdo_drop_bmp is not sync to WB cache.
            * We execute "sc controlsync=1" when one port is link up. But when we exit SDK,
            * the port is link down, so the conrresponding bit in THDU_OUTPUT_PORT_RX_ENABLE0_64r
            * of the port is zero.  After WarmBoot SDK,
            * we can't set conrresponding bit in THDU_OUTPUT_PORT_RX_ENABLE0_64r
            * for the port when it is link up, because, the value of thdo_drop_bmp is zero.
            */
            if(cur_enable == FALSE) {
                LOG_INFO(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Latest thdo_drop_bmp was not updated to WB cache!!!\n")));
                *thdo_drop_bmp |= flag;
            }
        }

        if (*thdo_drop_bmp != flag) {
            /*other callers are in thdo drop*/
            *thdo_drop_bmp &= ~flag;
            FWD_CTRL_UNLOCK(unit);
            return rv;
        }

        /*Ok, only self is in thdo drop, enbable it*/
        rv = _soc_monterey_thdo_hw_set(unit, port, enable);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,(BSL_META_U(unit, "Setting the registers failed.\n")));
            FWD_CTRL_UNLOCK(unit);
            return rv;
        }

        *thdo_drop_bmp &= ~flag;

        FWD_CTRL_UNLOCK(unit);
        return rv;
    }

    FWD_CTRL_UNLOCK(unit);
    return rv;
}


int
soc_monterey_port_traffic_egr_enable_set(int unit, soc_port_t port,
                               int enable)
{
    return _soc_monterey_port_thdo_rx_enable_set(unit, port, enable,
                                            COSQ_SCHED_SET_CTRL);
}

int
soc_monterey_port_mmu_tx_enable_set(int unit, soc_port_t port,
                               int enable)
{
    return _soc_monterey_port_thdo_rx_enable_set(unit, port, enable,
                                           MMU_TRAFFIC_EN_CTRL);
}

int
soc_monterey_port_edb_reset(int unit, soc_port_t port)
{
     soc_info_t *si = &SOC_INFO(unit);
     uint32 entry[SOC_MAX_MEM_WORDS];
     int phy_port ;
     /* Get physical port */
     phy_port = si->port_l2p_mapping[port];
     if (phy_port == -1) {
           return SOC_E_INTERNAL;
     }
     SOC_IF_ERROR_RETURN
       (READ_EGR_PER_PORT_BUFFER_SFT_RESETm(unit, MEM_BLOCK_ALL,
                                          phy_port, entry));
     soc_mem_field32_set(unit, EGR_PER_PORT_BUFFER_SFT_RESETm,
                         entry, ENABLEf, 1);
     SOC_IF_ERROR_RETURN
     (WRITE_EGR_PER_PORT_BUFFER_SFT_RESETm(unit, MEM_BLOCK_ALL,
                                           phy_port, entry));
     /* Initialize EP credits in MMU */
     SOC_IF_ERROR_RETURN(_soc_monterey_mmu_ep_credit_reset(unit, port));

     SOC_IF_ERROR_RETURN
       (READ_EGR_PER_PORT_BUFFER_SFT_RESETm(unit, MEM_BLOCK_ALL,
                                          phy_port, entry));
     soc_mem_field32_set(unit, EGR_PER_PORT_BUFFER_SFT_RESETm,
                         entry, ENABLEf, 0);
     SOC_IF_ERROR_RETURN
     (WRITE_EGR_PER_PORT_BUFFER_SFT_RESETm(unit, MEM_BLOCK_ALL,
                                           phy_port, entry));
     return SOC_E_NONE;
}
int
soc_monterey_port_thdo_rx_enable_set(int unit, soc_port_t port,
                                     int enable)
{
    return _soc_monterey_port_thdo_rx_enable_set(unit, port, enable,
                                            MAC_ENABLE_SET_CTRL);
}


STATIC int
_soc_mn_resource_data_check(int unit, int num_res, soc_port_resource_t *res)
{
    int i, j;
    /* null check for all resource data before accessing register */
    for (i = 0 ; i < num_res ; ++i) {
        soc_port_resource_t *p = &res[i];
        if (NULL == p) {
            /*
            * COVERITY
            * It is kept intentionally as a defensive check.
            */
            /* coverity[dead_error_line] */
            return SOC_E_PARAM;
        }
        for (j = 0; j < p->num_lanes ; ++j) {
            soc_port_lane_info_t *p_lane_info = p->lane_info[j];
            if (NULL == p_lane_info) {
                return SOC_E_PARAM;
            }
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_mn_pgw_obm_set
 * Purpose:
 *      Reconfigure PGW_OBM registers in flex port sequence
 *      This function will do two things,
 *      1. clear all registers for old ports which exists in pre-Flex status
 *      2. initialize all registers for new ports which exists in post-Flex status
 *
 * Parameters:
 *      unit         - (IN) Unit number.
 *      pre_num_res  - (IN) Number of resource data which exist in pre-Flex status
 *      post_num_res - (IN) Number of resource data which exist in post-Flex status
 *      pre_res      - (IN) Resource data structure pointer in pre-Flex status
 *      post_res     - (IN) Resource data structure pointer in post-Flex status
 *
 * Returns:
 *      SOC_E_XXX
 */
int
soc_mn_pgw_obm_set(int unit,
                     int pre_num_res,  soc_port_resource_t *pre_res,
                     int post_num_res, soc_port_resource_t *post_res)
{
    int i;
    int port, xlp;
    int lossless;

    SOC_IF_ERROR_RETURN(
        _soc_mn_resource_data_check(unit, pre_num_res, pre_res));
    SOC_IF_ERROR_RETURN(
        _soc_mn_resource_data_check(unit, post_num_res, post_res));

    lossless = soc_property_get(unit, spn_MMU_LOSSLESS, 0);
    for (i = 0; i < pre_num_res; i++) {
        if (pre_res[i].flags & SOC_PORT_RESOURCE_I_MAP) {
            continue;
        }
        if (pre_res[i].physical_port == -1) {
            continue;
        }
        port = pre_res[i].logical_port;
        _soc_monterey_idb_port_init(unit, TRUE, lossless, port);
        xlp = (pre_res[i].physical_port % (MONTEREY_TSCS_PER_PGW * MONTEREY_PGWS_PER_DEV));
        SOC_IF_ERROR_RETURN(_soc_monterey_port_blk_ca_mode_set(unit, TRUE, xlp));
    }

    for (i = 0; i < post_num_res; i++) {
        port = post_res[i].logical_port;
        _soc_monterey_idb_port_init(unit, FALSE, lossless, port);
        xlp = SOC_INFO(unit).port_serdes[port];
        SOC_IF_ERROR_RETURN(_soc_monterey_port_blk_ca_mode_set(unit, TRUE, xlp));
    }


    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_mn_obm_threshold_default_set
 * Purpose:
 *      Reconfigure PGW Threshold in flex port sequence
 *      In this function following registers will be initialized.
 *          PGW_OBM[0-3]_THRESHOLD
 *          PGW_OBM[0-3]_THRESHOLD2
 *      If default_flag is PGW_OBM_INIT_HW_DEFAULT,
 *      register will be intialized by HW reset value
 *      If default_flag is PGW_OBM_INIT_SW_DEFAULT,
 *      register will be intialized by SW reset value
 *
 * Parameters:
 *      unit         - (IN) Unit number
 *      num_res      - (IN) Number of resource
 *      res          - (IN) Resource data structure
 *      lossless     - (IN) Lossless configuration of device
 *      default_flag - (IN) PGW_OBM_INIT_HW_DEFAULT
 *                          Use HW reset value for register value
 *                   - (IN) PGW_OBM_INIT_SW_DEFAULT
 *                          Use SW default value
 *
 * Assumption:
 *      This function cannot access SOC_INFO data structure.
 *      So assume followings condition should be checked
 *      before calling this function.
 *
 *      1. pgw block is valid
 *             si->block_valid[PGW_CL_BLOCK(unit, pgw)] is true
 *      2. port block is used
 *             block = PGW_CL_BLOCK(unit, pgw);
 *             SOC_BLOCK_PORT(unit, block) < 0
 *      3. num_res should be correct number of resource data structure.
 *         all resource data structure should be accessed by using
 *         pointer offset by res_num. If the pointer of resource data is NULL,
 *         returns SOC_E_PARAM.
 *
 * Returns:
 *      SOC_E_XXX
 */
STATIC
soc_error_t soc_mn_obm_threshold_default_set(int unit,
                                               int num_res,
                                               soc_port_resource_t *res,
                                               int lossless,
                                               int reset)
{
    int i;

    for (i = 0 ; i < num_res ; ++i) {

        
        SOC_IF_ERROR_RETURN(_soc_monterey_idb_port_init(unit, reset, lossless, res->logical_port));

    }

    return SOC_E_NONE;
}

STATIC int
soc_mn_obm_threshold_hw_default_set(int unit, int num_res,
                                      soc_port_resource_t *res,
                                      int lossless)
{

    SOC_IF_ERROR_RETURN(soc_mn_obm_threshold_default_set(unit, num_res, res,
                                               lossless,
                                               TRUE));
    return SOC_E_NONE;
}

STATIC int
soc_mn_obm_threshold_sw_default_set(int unit, int num_res,
                                      soc_port_resource_t *res,
                                      int lossless)
{
    SOC_IF_ERROR_RETURN(soc_mn_obm_threshold_default_set(unit, num_res, res,
                                               lossless,
                                               FALSE));
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_mn_obm_threshold_set
 * Purpose:
 *  Reconfigure PGW registers in flex port sequence
 *  In this function PGW OBM threshold registers will be initialized.
 *  This function will do two things,
 *  1. clear all registers for old ports which exists in pre-Flex status
 *  2. initialize all registers for new ports which exists in post-Flex status
 *
 * Parameters:
 *  unit         - (IN) Unit number.
 *  pre_num_res  - (IN) Number of resource data which exist in pre-Flex status
 *  post_num_res - (IN) Number of resource data which exist in post-Flex status
 *  pre_res      - (IN) Resource data structure pointer in pre-Flex status
 *  post_res     - (IN) Resource data structure pointer in post-Flex status
 *  lossless     - (IN) mmu lossless information
 *
 * Returns:
 *  SOC_E_XXX
 *
 * Note : For 100G port, this functions expects the ovsersub field to be
 * disabled.
 */
soc_error_t soc_mn_obm_threshold_set(int unit,
                         int pre_num_res, soc_port_resource_t *pre_res,
                         int post_num_res, soc_port_resource_t *post_res,
                         int lossless)
{
    
    SOC_IF_ERROR_RETURN(_soc_mn_resource_data_check(unit, pre_num_res,
                                                      pre_res));
    SOC_IF_ERROR_RETURN(_soc_mn_resource_data_check(unit, post_num_res,
                                                      post_res));
    SOC_IF_ERROR_RETURN(soc_mn_obm_threshold_hw_default_set(unit,
                        pre_num_res, pre_res, lossless));

    SOC_IF_ERROR_RETURN(soc_mn_obm_threshold_sw_default_set(unit,
                        post_num_res, post_res, lossless));

    return SOC_E_NONE;
}


/*
 * This function is used to enable or disable cut through when a port is
 * operating in store and forward mode.
 * Refer TD2_Cutthrough_programming_guideline.pdf
 * Caller must pass the correct port_info pointer, corresponding to the port
 * being configured (and sent by the BCM layer)
 * To enable cut-through, pass enable = 1
 * To disable cut-through, pass enable = 0
 */
int
soc_mn_cut_thru_enable_disable(int unit, soc_port_t port, int enable)
{
    uint32 rval;
    uint32 speed;
    soc_pbmp_t pbm;

    /* Cut-through not supported on non-packet-forwarding ports */
    if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port) || IS_RDB_PORT(unit, port)) {
        return SOC_E_PORT;
    }

    /* When in Oversub, only 40G speeds support CT  */
    speed = SOC_INFO(unit).port_speed_max[port];
    speed = SOC_PORTCTRL_HG2_TO_IEEE_BW_GET(speed);
    if (IS_OVERSUB_PORT(unit, port) && speed != 40000) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "ASF supported only on 40G/42G ports "
                              "when in Oversub\n")));
        return SOC_E_PORT;
    }


    /* Mask unnecessary bits */
    enable &= 0x1;

    SOC_IF_ERROR_RETURN(READ_ASF_PORT_CFGr(unit, port, &rval));

    speed = soc_reg_field_get(unit, ASF_PORT_CFGr, rval, ASF_PORT_SPEEDf);

    /* Set port speed to 0 */
    soc_reg_field_set(unit, ASF_PORT_CFGr, &rval, ASF_PORT_SPEEDf, 0);
    SOC_IF_ERROR_RETURN(WRITE_ASF_PORT_CFGr(unit, port, rval));

    if (!enable) {
        /* Wait 8ms for draining all cells */
        sal_usleep(8000);
    }


    SOC_PBMP_CLEAR(pbm);
    SOC_PBMP_PORT_ADD(pbm, port);
    SOC_IF_ERROR_RETURN(soc_monterey_ports_edb_config(unit, enable, pbm));


    /* Set UC_ASF_ENABLE, MC_ASF_ENABLE bits to 1 */
    soc_reg_field_set(unit, ASF_PORT_CFGr, &rval, UC_ASF_ENABLEf, enable);
    soc_reg_field_set(unit, ASF_PORT_CFGr, &rval, MC_ASF_ENABLEf, enable);
    SOC_IF_ERROR_RETURN(WRITE_ASF_PORT_CFGr(unit, port, rval));

    if (enable) {
        /* Wait 1 micro-sec */
        sal_usleep(1);
    }

    soc_reg_field_set(unit, ASF_PORT_CFGr, &rval, ASF_PORT_SPEEDf, speed);
    SOC_IF_ERROR_RETURN(WRITE_ASF_PORT_CFGr(unit, port, rval));

    return SOC_E_NONE;
}


int soc_mn_max_speed_port_find(int phy_port,
                                        soc_port_resource_t *port_info,
                                        int port_info_arr_size)
{
    int idx, index, speed;

    index = 0;
    speed = port_info[index].speed;

    for (idx = 1; idx < port_info_arr_size; idx++) {
        /* If all speeds are equal, '>' will return the 1st index,
         * '>=' will return the last index
         */
        if (port_info[idx].speed > speed) {
            speed = port_info[idx].speed;
            index = idx;
        }
    }

    return index;
}

/*
 * This function checks if cut-through is enabled for a given port.
 * If either unicast or multicast cut-through is enabled, the function
 * returns TRUE else FALSE is returned
 */
int
soc_monterey_is_cut_thru_enabled(int unit, soc_port_t port, int *enable)
{
    uint32 rval;
    uint32 uc_enable, mc_enable;

    /* Cut-through not supported on following ports */
    if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port) || IS_RDB_PORT(unit, port)) {
        return SOC_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(READ_ASF_PORT_CFGr(unit, port, &rval));

    uc_enable = soc_reg_field_get(unit, ASF_PORT_CFGr, rval, UC_ASF_ENABLEf);
    mc_enable = soc_reg_field_get(unit, ASF_PORT_CFGr, rval, MC_ASF_ENABLEf);

    if (uc_enable || mc_enable) {
        *enable = TRUE;
    } else {
        *enable = FALSE;
    }

    return SOC_E_NONE;
}

/*
 * This function is used to clear various counters in EP block. The
 * counters to clear are mentioned in 'Reconfigure EP' section of TD2+ Flexport
 */
STATIC int soc_mn_ep_counters_clear(int unit, soc_port_resource_t *port_info)
{
    uint64 val64;

    COMPILER_64_ZERO(val64);

   SOC_IF_ERROR_RETURN(WRITE_TPCEr(unit, port_info->logical_port, val64));

    SOC_IF_ERROR_RETURN(WRITE_TDBGC0r(unit, port_info->logical_port, val64));
    SOC_IF_ERROR_RETURN(WRITE_TDBGC1r(unit, port_info->logical_port, val64));
    SOC_IF_ERROR_RETURN(WRITE_TDBGC2r(unit, port_info->logical_port, val64));
    SOC_IF_ERROR_RETURN(WRITE_TDBGC3r(unit, port_info->logical_port, val64));
    SOC_IF_ERROR_RETURN(WRITE_TDBGC4r(unit, port_info->logical_port, val64));
    SOC_IF_ERROR_RETURN(WRITE_TDBGC5r(unit, port_info->logical_port, val64));
    SOC_IF_ERROR_RETURN(WRITE_TDBGC6r(unit, port_info->logical_port, val64));
    SOC_IF_ERROR_RETURN(WRITE_TDBGC7r(unit, port_info->logical_port, val64));
    SOC_IF_ERROR_RETURN(WRITE_TDBGC8r(unit, port_info->logical_port, val64));
    SOC_IF_ERROR_RETURN(WRITE_TDBGC9r(unit, port_info->logical_port, val64));
    SOC_IF_ERROR_RETURN(WRITE_TDBGC10r(unit, port_info->logical_port, val64));
    SOC_IF_ERROR_RETURN(WRITE_TDBGC11r(unit, port_info->logical_port, val64));

    return SOC_E_NONE;
}


STATIC int
soc_mn_port_resource_tdm_calculate(int unit, soc_monterey_tdm_t *tdm)
{
    int index;
    soc_info_t *si;
    soc_pbmp_t pbmp;
    int port = 0, slot = 0, phy_port = 0;
    int roe_speed_cpri,roe_speed_rsvd4;
    /* soc info has new values */
    si = &SOC_INFO(unit);

    sal_memset(tdm, 0, sizeof(soc_monterey_tdm_t));

    PBMP_PORT_ITER(unit, port) {
        if (SOC_PBMP_MEMBER(si->all.disabled_bitmap, port)) {
            continue;
        }
        phy_port = si->port_l2p_mapping[port];
        tdm->speed[phy_port] = si->port_speed_max[port];
        if(IS_CPRI_PORT(unit,port) || IS_RSVD4_PORT(unit,port)) {
           if (si->port_speed_max[port] >= 10137)  {
               tdm->speed[phy_port] = 25000;
           }else {
               if (SOC_PBMP_MEMBER(si->roe_compression, port)) {
                   roe_speed_cpri = soc_property_port_get(unit, port, spn_CPRI_PORT_INIT_SPEED_ID, -1);
                   roe_speed_rsvd4 =  soc_property_port_get(unit, port, spn_RSVD4_PORT_INIT_SPEED_ID, -1);
                   if ((roe_speed_cpri > 0) && IS_CPRI_PORT(unit,port)) {
                       if ((roe_speed_cpri > 4)) {
                            tdm->speed[phy_port] = 25000;
                       } else {
                            tdm->speed[phy_port] = 10000;
                       }
                   } else if ((roe_speed_rsvd4 > 0) && IS_RSVD4_PORT(unit,port)) {
                       if ((roe_speed_rsvd4 > 8)) {
                            tdm->speed[phy_port] = 25000;
                       } else {
                            tdm->speed[phy_port] = 10000;
                       }
                   } else {
                       if (si->port_speed_max[port] > 6144)  {
                           tdm->speed[phy_port] = 25000;
                       } else {
                            tdm->speed[phy_port] = 10000;
                       }
                   }
               } else {
                   if (si->port_speed_max[port] == 10000) {
                       roe_speed_cpri = soc_property_port_get(unit, port, spn_CPRI_PORT_INIT_SPEED_ID, -1);
                       if ((roe_speed_cpri > 5)) {
                            tdm->speed[phy_port] = 25000;
                       }
                   } else {
                       tdm->speed[phy_port] = 10000;
                   }
               }
           }

        }

        MONTEREY_TDM_SPEED_ADJUST(unit, port, tdm->speed[phy_port]);

        tdm->port_state[phy_port] =
            IS_OVERSUB_PORT(unit, port) ? PORT_STATE_OVERSUB : PORT_STATE_LINERATE;

        /*
         * Primary port will be setup as line-rate or oversub.
         * Subsequent physical ports (sister-ports) will be either
         * DISABLED or SUBPORT/COMBINED mode.
         *
         * TDM Algo expects lanes info based on speed.
         */
        if (soc_monterey_port_is_falcon(unit, phy_port)) { /*  Falcons  */
            if (tdm->speed[phy_port] > 25000) {
                tdm->port_state[phy_port + 1] = PORT_STATE_SUBPORT;
            }
            if (tdm->speed[phy_port] > 50000) {
                tdm->port_state[phy_port + 2] = PORT_STATE_SUBPORT;
                tdm->port_state[phy_port + 3] = PORT_STATE_SUBPORT;
            }
        } else {                                         /*  Eagles  */
            if (tdm->speed[phy_port] > 10000) {
                tdm->port_state[phy_port + 1] = PORT_STATE_SUBPORT;
            }
            if (tdm->speed[phy_port] > 20000) {
                tdm->port_state[phy_port + 2] = PORT_STATE_SUBPORT;
                tdm->port_state[phy_port + 3] = PORT_STATE_SUBPORT;
            }
            if (tdm->speed[phy_port] > 40000) {
                tdm->port_state[phy_port + 4] = PORT_STATE_SUBPORT;
                tdm->port_state[phy_port + 5] = PORT_STATE_SUBPORT;
                tdm->port_state[phy_port + 6] = PORT_STATE_SUBPORT;
                tdm->port_state[phy_port + 7] = PORT_STATE_SUBPORT;
                tdm->port_state[phy_port + 8] = PORT_STATE_SUBPORT;
                tdm->port_state[phy_port + 9] = PORT_STATE_SUBPORT;
            }
        }
    }
    /* CPU, LB and RDB ports */
    tdm->speed[0] = 1000;
    tdm->speed[65] = 1000;
/*Macsec port is 20G in monterey*/
    tdm->speed[66] = 20000;
    tdm->tdm_bw = si->bandwidth / 1000;
    SOC_PBMP_ASSIGN(pbmp, si->oversub_pbm);
    SOC_PBMP_AND(pbmp, si->xpipe_pbm);
    /* tell tdm code the device is oversub if any ports is oversub */
    tdm->is_oversub = SOC_PBMP_NOT_NULL(pbmp);
    SOC_PBMP_ASSIGN(pbmp, si->oversub_pbm);

    for (slot = 0; slot <= MONTEREY_MMU_TDM_LENGTH; slot++) {
        tdm->mmu_tdm[slot] = NUM_EXT_PORTS;
    }

    if (LOG_CHECK(BSL_LS_SOC_TDM | BSL_INFO)) {
        LOG_CLI((BSL_META_U(unit, "tdm_bw: %dG\n"), tdm->tdm_bw));
        LOG_CLI((BSL_META_U(unit, "port speed:")));
        for (index = 0; index < NUM_EXT_PORTS; index++) {
            if (index % 8 == 0) {
                LOG_CLI((BSL_META_U(unit, "\n    ")));
            }
            LOG_CLI((BSL_META_U(unit, " %06d(%03d)"),
                     tdm->speed[index], index));
        }
        LOG_CLI((BSL_META_U(unit, "\n")));
        LOG_CLI((BSL_META_U(unit, "port state map:")));
        for (index = 0; index < NUM_EXT_PORTS; index++) {
            if (index % 8 == 0) {
                LOG_CLI((BSL_META_U(unit, "\n    ")));
            }
            if (index == 0 || index == (NUM_EXT_PORTS - 1)) {
                LOG_CLI((BSL_META_U(unit, " --------")));
            } else {
                LOG_CLI((BSL_META_U(unit, " %03d(%03d)"),
                         tdm->port_state[index], index));
            }
        }
        LOG_CLI((BSL_META_U(unit, "\n")));
    }


    SOC_IF_ERROR_RETURN(soc_monterey_tdm_calc(unit, tdm));


    return SOC_E_NONE;
}



STATIC int
soc_mn_ovr_sub_tdm_update(int unit,
                          soc_monterey_tdm_t *tdm,
                          int pre_count,
                          soc_port_resource_t *pre_res,
                          int post_count,
                          soc_port_resource_t *post_res)
{
    int phy_port;
    int group, wt_group, speed_group;
    int res_count, speed_class;
    int slot, empty_slot_cnt;
    uint32 speed_spacing;
    soc_monterey_tdm_t *cur_tdm;


    tdm = MONTEREY_TDM_STRUCT_BACKUP(unit);
    cur_tdm = MONTEREY_TDM_STRUCT_ACTIVE(unit);

    /* Remove old ports from the existing egress overusb groups */
    for (res_count = 0; res_count < pre_count; res_count++) {
        soc_port_resource_t *p = &pre_res[res_count];

        /* If OBM is not enabled then continue */
        if (!p->oversub) {
            continue;
        }

        if (p->flags & SOC_PORT_RESOURCE_INACTIVE) {
            continue;
        }

        phy_port = p->physical_port;

        for (group = 0; group < MONTEREY_MMU_OVS_GROUP_COUNT; group++) {
            if (cur_tdm->egress_ovs_tdm.mmu_ovs_group_speed[group] == 0) {
                continue;
            }
            empty_slot_cnt = 0;
            for (slot = 0; slot < MONTEREY_MMU_OVS_GROUP_TDM_LENGTH; slot++) {
                if (cur_tdm->egress_ovs_tdm.mmu_ovs_group_tdm[group][slot] == phy_port) {
                    cur_tdm->egress_ovs_tdm.mmu_ovs_group_tdm[group][slot] = NUM_EXT_PORTS;

                }
                if (cur_tdm->egress_ovs_tdm.mmu_ovs_group_tdm[group][slot] >= NUM_EXT_PORTS) {
                    empty_slot_cnt++;
                }
            }

            /* Mark group that has no more valid mmu ports as free */
            if (empty_slot_cnt == MONTEREY_MMU_OVS_GROUP_TDM_LENGTH) {
                MONTEREY_TDM_WT_GRP_FREE(unit, cur_tdm->egress_ovs_tdm.mmu_ovs_group_wt_group[group]);
                cur_tdm->egress_ovs_tdm.mmu_ovs_group_speed[group] = 0;
                cur_tdm->egress_ovs_tdm.mmu_ovs_group_wt_group[group] = MONTEREY_MMU_OVS_WT_GROUP_COUNT;
                break;
            }
         }
    }

    /* Add 'new' ports to the egress schedulers */
    for (res_count = 0; res_count < post_count; res_count++) {
        soc_port_resource_t *p = &post_res[res_count];

        /* If OBM is not enabled then continue */
        if (!p->oversub) {
            continue;
        }

        if (p->flags & SOC_PORT_RESOURCE_INACTIVE) {
            continue;
        }

        speed_class = p->speed;
        phy_port = p->physical_port;


        if (SOC_FAILURE(_soc_monterey_mmu_ovs_speed_class_map_get(
                        unit, &speed_class, &speed_group, &speed_spacing))) {

            LOG_ERROR(BSL_LS_SOC_PORT,
                      (BSL_META_U(unit,
                                  "Invalid Speed Class %d for port %d (physical = %d)"),
                       speed_class, p->logical_port, phy_port));

            return SOC_E_INTERNAL;
        }

        MONTEREY_TDM_WT_GRP_LOG(unit);

        /* Try to fit the new port in the group with same speed ports */
        for (group = 0; group < MONTEREY_MMU_OVS_GROUP_COUNT; group++) {
            /* Group with different speed ports; skip! */
            if (cur_tdm->egress_ovs_tdm.mmu_ovs_group_speed[group] != speed_class) {
                continue;
            }

            for (slot = 0; slot < MONTEREY_MMU_OVS_GROUP_TDM_LENGTH; slot++) {
                /*  break when an empty slot is found */
                if (cur_tdm->egress_ovs_tdm.mmu_ovs_group_tdm[group][slot] >= NUM_EXT_PORTS) {
                    break;
                }
            }
            if (slot < MONTEREY_MMU_OVS_GROUP_TDM_LENGTH) {
                cur_tdm->egress_ovs_tdm.mmu_ovs_group_tdm[group][slot] = phy_port;
                phy_port = NUM_EXT_PORTS;
                break;
            }
        }

        if (phy_port == NUM_EXT_PORTS) {
            /* Found an empty slot and filled; continue to next */
            continue;
        }

        /* Could not get a group with same speed ports, try a free group  */
        for (group = 0; group < MONTEREY_MMU_OVS_GROUP_COUNT; group++) {
            if (cur_tdm->egress_ovs_tdm.mmu_ovs_group_speed[group] == 0) {
                break;
            }
        }

        if (group < MONTEREY_MMU_OVS_GROUP_COUNT) {
            cur_tdm->egress_ovs_tdm.mmu_ovs_group_tdm[group][0] = phy_port;
            cur_tdm->egress_ovs_tdm.mmu_ovs_group_speed[group] = speed_class;

            for (wt_group = 0; wt_group < MONTEREY_MMU_OVS_WT_GROUP_COUNT; wt_group++) {
                if (MN_TDM_WT_GRP_SPG_GET(unit, wt_group) == speed_group) {
                    MONTEREY_TDM_WT_GRP_RESERVE(unit, wt_group, speed_group);
                    break;
                }
            }

            if (wt_group >= MONTEREY_MMU_OVS_WT_GROUP_COUNT) {
                for (wt_group = 0; wt_group < MONTEREY_MMU_OVS_WT_GROUP_COUNT; wt_group++) {
                    if (MN_TDM_WT_GRP_REFCNT_GET(unit, wt_group) == 0) {
                        MONTEREY_TDM_WT_GRP_RESERVE(unit, wt_group, speed_group);
                        break;
                    }
                }
            }

            if (wt_group >= MONTEREY_MMU_OVS_WT_GROUP_COUNT) {
                /* We could not find a weight group for this new port */
                LOG_ERROR(BSL_LS_SOC_PORT,
                    (BSL_META_U(unit,
                                "Could not find a usable weight group for "
                                "port %d (physical = %d; speed_class = %d)"),
                    p->logical_port, phy_port, speed_class));
                return SOC_E_INTERNAL;
            }

            cur_tdm->egress_ovs_tdm.mmu_ovs_group_wt_group[group] = wt_group;

            continue;
        }

        /* We could not find a group that can fit this new port */
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "Could not find a usable oversub group for "
                              "port %d (physical = %d; speed_class = %d)"),
                   p->logical_port, phy_port, speed_class));

        return SOC_E_INTERNAL;
     }

    sal_memcpy(&(tdm->egress_ovs_tdm), &(cur_tdm->egress_ovs_tdm), sizeof(tdm->egress_ovs_tdm));

    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_mn_port_resource_tdm_set
 * Purpose:
 *      Reconfigure the ingress and egress schedulers for
 *      the new port configuration.
 *
 *      This includes reconfiguration of:
 *      - OBM Thresholds
 *      - EDB Prebuffer
 *      - Ingress Oversub Scheduler
 *      - Egress Oversub Scheduler
 *      - Line-Rate Scheduler
 * Parameters:
 *      unit                 - (IN) Unit number.
 *      curr_port_info_size  - (IN) Size of current port information array
 *      curr_port_info       - (IN) Per-port array with current flexing info
 *      new_port_info_size   - (IN) Size of new port information array
 *      new_port_info        - (IN) Per-port array with new flexing info
 *      si_info              - (IN) Contains subset of fields from
 *                             soc_info_t structure
 *                             prior to the flexing operation
 *      lossless             - (IN) MMU lossless information
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      Assumptions
 *      - soc info structure members have been updated to reflect the latest
 *        flexing operation
 *      - There is no overlapping between logical or physical ports in the
 *        arrays passed, that is, operations like 4x10->1x40->4x10 on the same
 *        set of ports have been rejected at bcm layer itself, and do not reach
 *        this function
 *      - Pre-FlexPort array
 *        Contains current information on ports to be modified after
 *        the FlexPort operation.  This should include ports whose
 *        mappings are either deleted or remapped.
 *      - Post-FlexPort array
 *        Contains information on new configuration for
 *        ports that are active (present) after the FlexPort operation.
 *        This should include ports whose mappings that are remapped or new.
 */
int soc_mn_port_resource_tdm_set(int unit, int curr_port_info_size,
                                   soc_port_resource_t *curr_port_info,
                                   int new_port_info_size,
                                   soc_port_resource_t *new_port_info,
                                   soc_mn_info_t *si_info,
                                   int lossless)
{
    int rv;
    uint32 rval;
    soc_monterey_tdm_t *tdm;

    /* Sec 6.6.3.2, #1 and #2 will be filled as part of oversubscription
     * code changes
     */

    /* Reconfigure EDB prebuffer 6.6.3.2 (#3) */

    SOC_IF_ERROR_RETURN(READ_EGR_FLEXPORT_EXTRA_HOLDINGr(unit, &rval));
    soc_reg_field_set(unit, EGR_FLEXPORT_EXTRA_HOLDINGr, &rval,
                      CLPORT_CELL_COUNTSf, 0x1);
    soc_reg_field_set(unit, EGR_FLEXPORT_EXTRA_HOLDINGr, &rval,
                      XLPORT_CELL_COUNTSf, 0x3);
    soc_reg_field_set(unit, EGR_FLEXPORT_EXTRA_HOLDINGr, &rval,
                      ENABLEf, 0x1);
    SOC_IF_ERROR_RETURN(WRITE_EGR_FLEXPORT_EXTRA_HOLDINGr(unit, rval));

    /* Reconfigure EDB prebuffer back up calendar 6.6.3.2 (#4), for line rate
     * TDM calendar. Oversub related portion will be added later
     */

    /* Configure ingress scheduler back up calendar 6.6.3.2.2 (#1, bullet #2) */
    tdm = MONTEREY_TDM_STRUCT_BACKUP(unit);
    SOC_IF_ERROR_RETURN(soc_mn_port_resource_tdm_calculate(unit, tdm));

    /* Reconfigure oversub scheduler (Sections 6.6.3.2.3, 6.6.3.2.4).
     * Function internally determines if oversub is enabled/disabled
     */
    rv = soc_mn_ovr_sub_tdm_update(unit, tdm,
                                   curr_port_info_size, curr_port_info,
                                   new_port_info_size, new_port_info);
    SOC_IF_ERROR_RETURN(rv);


    /* toggle the active calendar */
    MONTEREY_TDM_ACTIVE_CAL(unit) ^= 1;

    /* update the tdm calendar config in the hardware */
    SOC_IF_ERROR_RETURN(soc_monterey_tdm_update(unit, 0, MONTEREY_TDM_ACTIVE_CAL(unit), tdm));

    /* Reconfigure EDB prebuffer 6.6.3.2 (#6) */

    sal_usleep(1);

    SOC_IF_ERROR_RETURN(READ_EGR_FLEXPORT_EXTRA_HOLDINGr(unit, &rval));
    soc_reg_field_set(unit, EGR_FLEXPORT_EXTRA_HOLDINGr, &rval,
                      CLPORT_CELL_COUNTSf, 0x1);
    soc_reg_field_set(unit, EGR_FLEXPORT_EXTRA_HOLDINGr, &rval,
                      XLPORT_CELL_COUNTSf, 0x3);
    soc_reg_field_set(unit, EGR_FLEXPORT_EXTRA_HOLDINGr, &rval,
                      ENABLEf, 0x0);
    SOC_IF_ERROR_RETURN(WRITE_EGR_FLEXPORT_EXTRA_HOLDINGr(unit, rval));


    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_mn_port_resource_pgw_set
 * Purpose:
 *      Reconfigure PGW registers in flex port sequence
 *      In this function following registers will be initialized.
 *      This function will do two things,
 *      1. clear all registers for old ports which exists in pre-Flex status
 *      2. initialize all registers for new ports which exists in post-Flex
 *         status
 *
 * Parameters:
 *      unit         - (IN) Unit number.
 *      pre_num_res  - (IN) Number of resource data which exist in pre-Flex
 *                          status
 *      post_num_res - (IN) Number of resource data which exist in post-Flex
 *                          status
 *      pre_res      - (IN) Resource data structure pointer in pre-Flex status
 *      post_res     - (IN) Resource data structure pointer in post-Flex status
 *      lossless     - (IN) MMU lossless information
 *
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      - Assumes FlexPort cases are valid.
 *      - Pre-FlexPort array
 *        Contains current information on ports to be modified after
 *        the FlexPort operation.  This should include ports whose
 *        mappings are either deleted or remapped.
 *      - Post-FlexPort array
 *        Contains information on new configuration for
 *        ports that are active (present) after the FlexPort operation.
 *        This should include ports whose mappings that are remapped or new.
 */
int
soc_mn_port_resource_pgw_set(int unit,
                      int pre_num_res,  soc_port_resource_t *pre_res,
                      int post_num_res, soc_port_resource_t *post_res,
                      int lossless)
{
    int i;

    SOC_IF_ERROR_RETURN(
        _soc_mn_resource_data_check(unit, pre_num_res, pre_res));
    SOC_IF_ERROR_RETURN(
        _soc_mn_resource_data_check(unit, post_num_res, post_res));

    for (i = 0; i < post_num_res; i++) {
         /* Reconfigure the Port Mode for Port Macro */
        SOC_IF_ERROR_RETURN(
            soc_portctrl_port_mode_set(unit, post_res[i].logical_port, post_res[i].mode));
    }

    SOC_IF_ERROR_RETURN(soc_mn_pgw_obm_set(unit,
                                           pre_num_res, pre_res,
                                           post_num_res, post_res));

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_mn_port_resource_ip_set
 * Purpose:
 *      Reconfigure IP.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      curr_port_info_size - (IN) Size of current port information array
 *      curr_port_info - (IN) Per-port array with current flexing info
 *      new_port_info_size - (IN) Size of new port information array
 *      new_port_info - (IN) Per-port array with new flexing info
 *      si_info - (IN) Contains subset of fields from soc_info_t structure
 *                     prior to the flexing operation
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      Assumptions
 *      - soc info structure members have been updated to reflect the latest
 *        flexing operation
 *      - There is no overlapping between logical or physical ports in the
 *        arrays passed, example, operations like 4x10->1x40->4x10 on the same
 *        set of ports have been rejected at bcm layer itself, and do not reach
 *        this function
 *      - Pre-FlexPort array
 *        Contains current information on ports to be modified after
 *        the FlexPort operation.  This should include ports whose
 *        mappings are either deleted or remapped.
 *      - Post-FlexPort array
 *        Contains information on new configuration for
 *        ports that are active (present) after the FlexPort operation.
 *        This should include ports whose mappings that are remapped or new.
 */
int soc_mn_port_resource_ip_set(int unit, int curr_port_info_size,
                                  soc_port_resource_t *curr_port_info,
                                  int new_port_info_size,
                                  soc_port_resource_t *new_port_info,
                                  soc_mn_info_t *si_info)
{
    soc_info_t *si;
    soc_mem_t mem;
    ing_physical_to_logical_port_number_mapping_table_entry_t entry;
    int port, phy_port;
    int num_phy_port;

    si = &SOC_INFO(unit);

    /* Ingress physical to logical port mapping */
    mem = ING_PHYSICAL_TO_LOGICAL_PORT_NUMBER_MAPPING_TABLEm;
    num_phy_port = soc_mem_index_count(unit, mem);

    for (phy_port = 0; phy_port < num_phy_port; phy_port++) {
        sal_memset(&entry, 0, sizeof(entry));

        /* The p2l mapping has been updated before this function is called */
        port = si->port_p2l_mapping[phy_port];

        soc_mem_field32_set(unit, mem, &entry, LOGICAL_PORT_NUMBERf,
                            port == -1 ? 0x7F : port);
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, phy_port,
                                          &entry));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_mn_port_resource_ep_set
 * Purpose:
 *      Reconfigure EP.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      curr_port_info_size - (IN) Size of current port information array
 *      curr_port_info - (IN) Per-port array with current flexing info
 *      new_port_info_size - (IN) Size of new port information array
 *      new_port_info - (IN) Per-port array with new flexing info
 *      si_info - (IN) Contains subset of fields from soc_info_t structure
 *                     prior to the flexing operation
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      Assumptions
 *      - soc info structure members have been updated to reflect the latest
 *        flexing operation
 *      - There is no overlapping between logical or physical ports in the
 *        arrays passed, example, operations like 4x10->1x40->4x10 on the same
 *        set of ports have been rejected at bcm layer itself, and do not reach
 *        this function
 *      - Pre-FlexPort array
 *        Contains current information on ports to be modified after
 *        the FlexPort operation.  This should include ports whose
 *        mappings are either deleted or remapped.
 *      - Post-FlexPort array
 *        Contains information on new configuration for
 *        ports that are active (present) after the FlexPort operation.
 *        This should include ports whose mappings that are remapped or new.
 */
int soc_mn_port_resource_ep_set(int unit, int curr_port_info_size,
                                  soc_port_resource_t *curr_port_info,
                                  int new_port_info_size,
                                  soc_port_resource_t *new_port_info,
                                  soc_mn_info_t *si_info)
{
    soc_info_t *si;
    uint32 rval;
    int num_port, port, idx, phy_port;

    si = &SOC_INFO(unit);

    /* Ingress logical to physical port mapping */
    num_port = soc_mem_index_count(unit, PORT_TABm) - 1;

    /* Egress logical to physical port mapping */
    for (port = 0; port < num_port; port++) {
        /*
         * Use logical to physical number mapping from soc info structure
         * since it has been updated before this function is called
         */
        phy_port = si->port_l2p_mapping[port];

        rval = 0;
        soc_reg_field_set(unit, EGR_LOGICAL_TO_PHYSICAL_PORT_NUMBER_MAPPINGr,
                          &rval, PHYSICAL_PORT_NUMBERf,
                          phy_port == -1 ? 0x7F : phy_port);
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_LOGICAL_TO_PHYSICAL_PORT_NUMBER_MAPPINGr(unit, port,
                                                                rval));
    }

    /* Clear counters for the newly added ports */
    for (idx = 0; idx < new_port_info_size; idx++) {
        SOC_IF_ERROR_RETURN(soc_mn_ep_counters_clear(unit,
                                                       &new_port_info[idx]));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_mn_port_resource_mmu_mapping_set
 * Purpose:
 *      Set the HW MMU Port mappings.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      nport     - (IN) Number of elements in array resource.
 *      resource  - (IN) Port Resource FlexPort configuration.
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      Assumes FlexPort cases are valid.
 *      Assumes SOC INFO data structure has been updated.
 */
int
soc_mn_port_resource_mmu_mapping_set(int unit, int nport,
                                       soc_port_resource_t *resource)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_port_resource_t *pr;
    int i;
    int logic_port;
    int phy_port;
    uint32 rval;

    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {
        /*
         * Skip for legacy inactive configurations since
         * MMU mappings are kept the same.
         */
        if (pr->flags & SOC_PORT_RESOURCE_I_MAP) {
            LOG_VERBOSE(BSL_LS_SOC_PORT,
                        (BSL_META_U(unit,
                                    "  Skip port with inactive configuration, "
                                    "mappings are kept the same, "
                                    "mmu=%d logical=%d physical=%d\n"),
                         si->port_p2m_mapping
                         [si->port_l2p_mapping[pr->logical_port]],
                         pr->logical_port,
                         si->port_l2p_mapping[pr->logical_port]));
            continue;
        }

        if (pr->physical_port == -1) {
            /* Remove old MMU mappings. */
            logic_port = 0x7F;
            phy_port = 0x7F;

        } else {
            logic_port = pr->logical_port;
            phy_port = pr->physical_port;
        }

        rval = 0;
        soc_reg_field_set(unit, MMU_PORT_TO_PHY_PORT_MAPPINGr, &rval,
                          PHY_PORTf, phy_port);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, MMU_PORT_TO_PHY_PORT_MAPPINGr, pr->logical_port,
                           0, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_PORT_TO_LOGIC_PORT_MAPPINGr, &rval,
                          LOGIC_PORTf, logic_port);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, MMU_PORT_TO_LOGIC_PORT_MAPPINGr, pr->logical_port,
                           0, rval));

        LOG_VERBOSE(BSL_LS_SOC_PORT,
                    (BSL_META_U(unit,
                                "MMU port mappings: "
                                "mmu=%d logical=%d physical=%d\n"),
                     si->port_p2m_mapping
                     [si->port_l2p_mapping[pr->logical_port]],
                     logic_port, phy_port));
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_mn_port_resource_mmu_set
 * Purpose:
 *      Called to reconfigure the MMU hardware.
 * Parameters:
 *      unit           - (IN) Device Number
 *      nport          - (IN) The number of port modifications in flexport_data
 *      flexport_data  - (IN) An array of port changes that are being done
 * Returns:
 *      SOC_E_NONE if parameters are valid, an error otherwise.
 * Note:
 *      Assumes FlexPort cases are valid.
 *      Assumes SOC INFO data structure has been updated.
 */
int
soc_mn_port_resource_mmu_set(int unit, int nport,
                               soc_port_resource_t *flexport_data)
{
    int i;

    /* Update MMU Port Mappings */
    SOC_IF_ERROR_RETURN
        (soc_mn_port_resource_mmu_mapping_set(unit, nport,
                                                flexport_data));

    /* Configure all MMU registers for new ports */
    for (i = 0; i < nport; i++) {
        if (flexport_data[i].physical_port != -1) {
            SOC_IF_ERROR_RETURN(
            soc_monterey_mmu_port_resource_set(unit, flexport_data[i].logical_port,
                                           flexport_data[i].speed));
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *     soc_mn_is_port_flex_enable
 * Purpose:
 *     Find out if port_flex_enable{physical port num} config
 *     is present on the given physical port in config.bcm
 * Parameters:
 *     unit    - (IN) Unit number.
 *     phy_port - (IN) physical port num
 * Returns:
 *     TRUE or FALSE
 */
STATIC int
soc_mn_is_port_flex_enable(int unit, int phy_port)
{
    int enable = 0;

    /* This property does not apply to cpu port &
     * loopback port
 */

    if (phy_port > 0 && phy_port < 129) {
        enable = soc_property_phys_port_get(unit,
                                            phy_port,
                                            spn_PORT_FLEX_ENABLE, 0);
    }

    return enable;
}


/* Tells if a port macro (indexed by TSC4, not TSC12) is flex port enabled.
 *
 * Port macro flex on or off is determined by the first port in that macro.
 * For TSC12 it's the first port in the first TSC4. For a TSC4 that is NOT
 * part of a TSC12, it's also the first port of that port macro.  For a TSC4
 * that is a part of a TSC12, refer to the first port of the first port macro
 * for that TSC12.
 *
 * Parameters:
 *      unit          - (IN) Unit number.
 *      port_macro    - (IN) port macro desired
 *      flex_enabled  - (OUT) whether the port macro is enabled or disabled
 *
 *  Returns:
 *      SOC_E_xxx
 */
int
soc_mn_port_macro_flex_enabled(int unit, int port_macro, int * flex_enabled)
{
    int port_num = -1;

    if ((port_macro < 0) || (port_macro > 18)) {
        return SOC_E_PARAM;
    }

    port_num = (MONTEREY_PORTS_PER_TSC * port_macro) + 1;

    *flex_enabled = soc_mn_is_port_flex_enable(unit, port_num);

    return SOC_E_NONE;
}


/*
* Function:
*     soc_monterey_port_icc_width_set
* Purpose:
*     Program PORT_INITIAL_COPY_COUNT_WIDTHr
* Parameters:
*     unit    - (IN) Unit number.
*     port    - (IN) Logical SOC port number.
* Returns:
*     SOC_E_XXX
 */
STATIC int
soc_monterey_port_icc_width_set(int unit, soc_port_t port)
{
    int count_width = 0;

    if (SOC_INFO(unit).port_speed_max[port] < 25000) {
        count_width = 1; /* 1 ICC bit  */
    } else if (SOC_INFO(unit).port_speed_max[port] <= 53000) {
        count_width = 2; /* 2 ICC bits  */
    } else {
        count_width = 3; /* 3 ICC bits  */
    }

    SOC_IF_ERROR_RETURN(
        soc_reg_field32_modify(unit, PORT_INITIAL_COPY_COUNT_WIDTHr,
            port, BIT_WIDTHf, count_width ? (count_width - 1) : 0));

    return SOC_E_NONE;
}

/*
 * Function:
 *     soc_monterey_repl_port_agg_map_init
 * Purpose:
 *     Initialize L3MC Port Agg Map.
 * Parameters:
 *     unit    - (IN) Unit number.
 *     port    - (IN) Logical SOC port number.
 * Returns:
 *     SOC_E_xxx
 */
STATIC int
soc_monterey_repl_port_agg_map_init(int unit, soc_port_t port)
{
    soc_info_t *si;
    int phy_port, mmu_port;
    uint32 regval;

    si = &SOC_INFO(unit);
    phy_port = si->port_l2p_mapping[port];
    mmu_port = si->port_p2m_mapping[phy_port];

    /* configure mmu port to repl aggregateId map */
    regval = 0;
    soc_reg_field_set(unit, MMU_TOQ_REPL_PORT_AGG_MAPr, &regval,
                      L3MC_PORT_AGG_IDf, mmu_port % 74);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, MMU_TOQ_REPL_PORT_AGG_MAPr,
                port,0,regval));

    return SOC_E_NONE;
}

/*
* Function:
*     soc_monterey_ipmc_repl_init
* Purpose:
*     Initialize IPMC Replication settings
* Parameters:
*     unit    - (IN) Unit number.
*     port    - (IN) Logical SOC port number.
* Returns:
*     SOC_E_XXX
 */
STATIC int
soc_monterey_ipmc_repl_init(int unit, soc_port_t port)
{
    /* Program MMU_TOQ_REPL_PORT_AGG_MAP
 */
    SOC_IF_ERROR_RETURN(soc_monterey_repl_port_agg_map_init(unit, port));

    return SOC_E_NONE;
}

/*
* Function:
* soc_monterey_mmu_port_resource_set
* Purpose:
* Reconfigure MMU for flexport operation
* Parameters:
* unit    - (IN) Unit number.
* port    - (IN) Logical SOC port number.
* reset   - (IN) Reset.
* Returns:
* SOC_E_XXX
 */
int soc_monterey_mmu_port_resource_set(int unit, soc_port_t port, int speed)
{

    /* Flex port operation not allowed on CPU or loopback port
 */
     if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port) || IS_RDB_PORT(unit, port)) {
         return SOC_E_PARAM;
     }

    /* Configure all MMU registers for new ports whose setting is
     * based on port speed.
 */
    SOC_IF_ERROR_RETURN(soc_monterey_port_speed_update(unit, port, speed));

    /* Program PORT_INITIAL_COPY_COUNT_WIDTH
 */
    SOC_IF_ERROR_RETURN(soc_monterey_port_icc_width_set(unit, port));

    /* Reconfigure IPMC replication
 */
    SOC_IF_ERROR_RETURN(soc_monterey_ipmc_repl_init(unit, port));

    return SOC_E_NONE;
}

/*
* Function:
*     soc_monterey_is_any_port_flex_enable
* Purpose:
*     Find out if
*     port_flex_enable=1 OR
*     port_flex_enable{physical port num}=1
*     config is present in config.bcm
* Parameters:
*     unit    - (IN) Unit number.
* Returns:
*     TRUE or FALSE
*     If such a config is present, this function
*     returns 1 - implying that flex is enable on
*     at least 1 port (port macro) in the system
*     This function returns 0, if flex is not enabled
*     on any port (port macro) in the system
 */
int soc_monterey_is_any_port_flex_enable(int unit)
{
    int phy_port, num_phy_port;

    num_phy_port = 67;
    for (phy_port = 0; phy_port < num_phy_port; phy_port++) {
        if (soc_property_phys_port_get(unit,
                                       phy_port,
                                       spn_PORT_FLEX_ENABLE, 0)) {
            return TRUE;
        }
    }

    return FALSE;
}




#if defined(SER_TR_TEST_SUPPORT)

#define MAX_HW_TCAMS  32
typedef struct ser_monterey_skipped_mem_s {
    soc_mem_t mem;
    soc_acc_type_t acc_type;
} ser_monterey_skipped_mem_t;

const ser_monterey_skipped_mem_t monterey_skipped_mems[] = {
    /* Below memories have only one entry */
    {L2_BULK_MATCH_DATAm, _SOC_ACC_TYPE_PIPE_ANY},
    {L2_BULK_MATCH_MASKm, _SOC_ACC_TYPE_PIPE_ANY},
    {L2_BULK_REPLACE_DATAm, _SOC_ACC_TYPE_PIPE_ANY},
    {L2_BULK_REPLACE_MASKm, _SOC_ACC_TYPE_PIPE_ANY},
    {L2_LEARN_INSERT_FAILUREm, _SOC_ACC_TYPE_PIPE_ANY},
    {MPLS_ENTROPY_LABEL_DATAm, _SOC_ACC_TYPE_PIPE_ANY},
    {ING_SNAT_ONLYm, _SOC_ACC_TYPE_PIPE_ANY},

   /* FP_GLOBAL_MASK_TCAM and FP_GM_FILEDSm have special handling in memscan
    * since H/W space is shared between these two mems.
    * It is protected by SW memscan SER engine
    */
    {FP_GLOBAL_MASK_TCAMm, _SOC_ACC_TYPE_PIPE_ANY},
/*  MMU_MTRO_EGRMETERINGCONFIG_MEM will be tested below two are for debug */
    {MMU_MTRO_EGRMETERINGCONFIG_MEM_Am, _SOC_ACC_TYPE_PIPE_ANY},
    {MMU_MTRO_EGRMETERINGCONFIG_MEM_Bm, _SOC_ACC_TYPE_PIPE_ANY},
/*debug memory*/
    {MMU_MAPPER_X_LSBm, _SOC_ACC_TYPE_PIPE_ANY},
/*ser flags not proper in regfile*/
    {EGR_LPORT_PROFILEm, _SOC_ACC_TYPE_PIPE_ANY},
/*Not able to write to this table need to check*/
    {L3_ENTRY_LPm, _SOC_ACC_TYPE_PIPE_ANY},
    {INVALIDm}
};

int
ser_test_monterey_mem_index_remap(int unit, ser_test_data_t *test_data,
                                  int *mem_has_ecc)
{
    int uft_bkt_bank = 0;
    int num_uft_banks = 0;
    int uft_le_fv = 0;
    int uft_le_ecc = 0;
    int remap_status = 0;

    *mem_has_ecc = 0;
    test_data->mem = test_data->mem_fv;
    test_data->index = test_data->index_fv;

    num_uft_banks = 0;
    switch (test_data->mem_fv) {
    case VLAN_XLATEm:
    case VLAN_MACm:
        test_data->mem = VLAN_XLATE_ECCm;
        *mem_has_ecc = 1;
        break;
    case EGR_VLAN_XLATEm:
        test_data->mem = EGR_VLAN_XLATE_ECCm;
        *mem_has_ecc = 1;
        break;
    case L2Xm:
    case L2_ENTRY_ONLYm:
        test_data->mem = L2_ENTRY_ONLY_ECCm;
        *mem_has_ecc = 1;
        break;
    case L3_ENTRY_ONLYm:
    case L3_ENTRY_IPV4_UNICASTm:
        test_data->mem = L3_ENTRY_ONLY_ECCm;
        test_data->index = test_data->index_fv;
        *mem_has_ecc = 1;
        break;
    case L3_ENTRY_IPV4_MULTICASTm:
    case L3_ENTRY_IPV6_UNICASTm:
        test_data->mem = L3_ENTRY_ONLY_ECCm;
        test_data->index = test_data->index_fv*2;
        *mem_has_ecc = 1;
        break;
    case L3_ENTRY_IPV6_MULTICASTm:
        test_data->mem = L3_ENTRY_ONLY_ECCm;
        test_data->index = test_data->index_fv*4;
        *mem_has_ecc = 1;
        break;
    case L3_DEFIP_ALPM_IPV4m: /* 6:4 */
        *mem_has_ecc = 1;
        test_data->mem = L3_DEFIP_ALPM_ECCm;
        uft_bkt_bank = test_data->index_fv & _SER_TEST_MEM_MN_ALPM_MASK(num_uft_banks);
        uft_le_fv = test_data->index_fv >> _SER_TEST_MEM_MN_ALPM_SHIFT(num_uft_banks);
        switch (uft_le_fv) {
        case 0: uft_le_ecc = 0; break;
        case 1: uft_le_ecc = 0; break; /* or 1 */
        case 2: uft_le_ecc = 1; break;
        case 3: uft_le_ecc = 2; break;
        case 4: uft_le_ecc = 2; break; /* or 3 */
        case 5: uft_le_ecc = 3; break;
        default: remap_status = SOC_E_PARAM; uft_le_ecc = uft_le_fv; break;
        }
        test_data->index = (uft_le_ecc <<
            _SER_TEST_MEM_MN_ALPM_SHIFT(num_uft_banks)) + uft_bkt_bank;
        break;
    case L3_DEFIP_ALPM_IPV4_1m: /* 4:4 */
    case L3_DEFIP_ALPM_IPV6_64m:
        *mem_has_ecc = 1;
        test_data->mem = L3_DEFIP_ALPM_ECCm;
        test_data->index = test_data->index_fv;
        break;
    case L3_DEFIP_ALPM_IPV6_64_1m: /* 3:4 */
        *mem_has_ecc = 1;
        test_data->mem = L3_DEFIP_ALPM_ECCm;
            uft_bkt_bank = test_data->index_fv & _SER_TEST_MEM_MN_ALPM_MASK(num_uft_banks);
            uft_le_fv = test_data->index_fv >> _SER_TEST_MEM_MN_ALPM_SHIFT(num_uft_banks);
        switch (uft_le_fv) {
        case 0: uft_le_ecc = 0; break; /* or 1 */
        case 1: uft_le_ecc = 1; break; /* or 2 */
        case 2: uft_le_ecc = 2; break; /* or 3 */
        default: remap_status = SOC_E_PARAM; uft_le_ecc = uft_le_fv; break;
        }
        test_data->index = (uft_le_ecc <<
            _SER_TEST_MEM_MN_ALPM_SHIFT(num_uft_banks)) + uft_bkt_bank;
        break;
    case L3_DEFIP_ALPM_IPV6_128m: /* 2:4 */
        *mem_has_ecc = 1;
        test_data->mem = L3_DEFIP_ALPM_ECCm;
            uft_bkt_bank = test_data->index_fv & _SER_TEST_MEM_MN_ALPM_MASK(num_uft_banks);
            uft_le_fv = test_data->index_fv >> _SER_TEST_MEM_MN_ALPM_SHIFT(num_uft_banks);
        switch (uft_le_fv) {
        case 0: uft_le_ecc = 0; break; /* or 1 */
        case 1: uft_le_ecc = 2; break; /* or 3 */
        default: remap_status = SOC_E_PARAM; uft_le_ecc = uft_le_fv; break;
        }
        test_data->index = (uft_le_ecc <<
            _SER_TEST_MEM_MN_ALPM_SHIFT(num_uft_banks)) + uft_bkt_bank;
        break;
    case L3_DEFIP_ALPM_RAWm: /* 1:4 */
        *mem_has_ecc = 1;
        test_data->mem = L3_DEFIP_ALPM_ECCm;
            uft_bkt_bank = test_data->index_fv & _SER_TEST_MEM_MN_ALPM_MASK(num_uft_banks);
            uft_le_fv = test_data->index_fv >> _SER_TEST_MEM_MN_ALPM_SHIFT(num_uft_banks);
        switch (uft_le_fv) {
        case 0: uft_le_ecc = 0; break; /* or 1,2,3 */
        default: remap_status = SOC_E_PARAM; uft_le_ecc = uft_le_fv; break;
        }
        test_data->index = (uft_le_ecc <<
            _SER_TEST_MEM_MN_ALPM_SHIFT(num_uft_banks)) + uft_bkt_bank;
        break;
    default:
        test_data->mem = test_data->mem_fv;
        test_data->index = test_data->index_fv;
        break;
    }
    if ((test_data->mem != test_data->mem_fv) ||
        (test_data->index != test_data->index_fv)) {
        LOG_VERBOSE(BSL_LS_SOC_SER,
            (BSL_META_U(unit,
                        "unit %d, ser_test_mem_index_remap: "
                        "mem_fv %s, index_fv %0d, mem %s, index %0d, "
                        "uft_bkt_bank %0d, uft_le_fv %0d, "
                        "uft_le_ecc %0d, remap_status %0d \n"),
             unit, SOC_MEM_NAME(unit, test_data->mem_fv), test_data->index_fv,
             SOC_MEM_NAME(unit, test_data->mem), test_data->index,
             uft_bkt_bank, uft_le_fv, uft_le_ecc, remap_status));
    }
    return remap_status;
}

int
soc_monterey_mem_is_eligible_for_scan(int unit, soc_mem_t mem)
{
    switch (mem) {
        case L3_DEFIP_ALPM_ECCm:
        case L2_ENTRY_LPm:
        case L3_ENTRY_LPm:
        case VLAN_XLATE_LPm:
        case EGR_VLAN_XLATE_LPm:
        case L2_ENTRY_ISS_LPm:
        case L3_ENTRY_ISS_LPm:
        return 1;

        default:
        break;
    }
    return 0;
}

int
soc_monterey_ser_test_skip_check (int unit, soc_mem_t mem)
{
    int i;

    if (!SOC_MEM_IS_VALID(unit, mem) ||
        ((mem != INVALIDm) &&
         ((SOC_MEM_INFO(unit, mem).flags &
           SOC_MEM_FLAG_SER_PARITY_ENABLE_SKIP)))) {
        return 1;
    }

    if (soc_mem_index_count(unit, mem) <= 0) {
        return 1;
    }

    for (i = 0; monterey_skipped_mems[i].mem != INVALIDm; i++) {
        if (monterey_skipped_mems[i].mem == mem) {
            return 1;
        }
    }
    if (!(SOC_MEM_INFO(unit, mem).flags & SOC_MEM_SER_FLAGS)) {
        return 1;
    }
    /* Skip the memory if the relevant feature and memory is disabled. */
    if (!soc_feature(unit, soc_feature_l3)
        || !soc_feature(unit, soc_feature_lpm_tcam) ) {
        /* Skip L3_DEFIP memorys */
        if ((L3_DEFIPm == mem) || (L3_DEFIP_PAIR_128m == mem)) return 1;
    }
    if (!soc_feature(unit, soc_feature_l3) ||
        soc_feature(unit, soc_feature_no_tunnel)) {
        /* Skip ING L3 Tunnel memorys */
        if ((L3_TUNNELm == mem) || (L3_TUNNEL_DATA_ONLYm == mem)) return 1;
    }
    if (!soc_feature(unit, soc_feature_nat)) {
        /* Skip NAT  memorys */
        if (ING_SNATm == mem) return 1;
    }
    
    /* Skip MPLS memories */

    if (ING_MPLS_EXP_MAPPINGm == mem) {
        return 1;
    }

    return 0;
}

STATIC
soc_error_t
_soc_monterey_tcam_ser_mem_info_get (int unit, soc_mem_t mem, int *index_p)
{
    int i;
    _soc_generic_ser_info_t *tcams = _soc_monterey_tcam_ser_info[unit];

    for (i = 0; tcams[i].mem != INVALIDm; i++) {
        if (tcams[i].mem == mem) {
            *index_p = i;
            return SOC_E_NONE;
        }
    }
    return SOC_E_NOT_FOUND;
}

STATIC soc_error_t
_soc_monterey_ser_mem_info_get (int unit, soc_mem_t mem, _soc_mem_ser_en_info_t** ser_mem_info)
{
    int i, j;
    _soc_mem_ser_en_info_t *ser_en_info = NULL;

    for (i = 0; _soc_monterey_ser_block_info[i].blocktype != 0; i++) {
        if ((_soc_monterey_ser_block_info[i].type == _SOC_MONTEREY_SER_TYPE_MEM) &&
            ((_soc_monterey_ser_block_info[i].blocktype == SOC_BLK_IPIPE) ||
             (_soc_monterey_ser_block_info[i].blocktype == SOC_BLK_EPIPE) ||
#ifdef INCLUDE_XFLOW_MACSEC
             (_soc_monterey_ser_block_info[i].blocktype == SOC_BLK_MACSEC) ||
#endif
             (_soc_monterey_ser_block_info[i].blocktype == SOC_BLK_MMU))) {
            ser_en_info = _soc_monterey_ser_block_info[i].info;
            for (j = 0; ser_en_info[j].mem != INVALIDm; j++) {
                if (ser_en_info[j].mem == mem) {
                    *ser_mem_info = &ser_en_info[j];
                    return SOC_E_NONE;
                }
            }
        }
    }

    return SOC_E_NOT_FOUND;
}
/*
 * Function:
 *      soc_monterey_ser_error_injection_support
 * Purpose:
 *      Checks if a memory is supported by error injection interface
 *
 * Parameters:
 *      unit        - (IN) Device Number
 *      memory      - (IN) Test data required for SER test
 *      pipe        - (IN) How many indices to test for each memory
 *
 * Returns:
 *  SOC_E_NONE if memory / reg is supported, SOC_E_UNAVAIL if unsupported
 */
soc_error_t
soc_monterey_ser_error_injection_support (int unit, soc_mem_t mem,
                                        int pipe)
{
    int        rv = SOC_E_UNAVAIL;
    _soc_mem_ser_en_info_t      *ser_mem_info = NULL;
    int        tcam_idx = 0;

    if (soc_monterey_ser_test_skip_check (unit,mem)) {
        return rv;
    }

    rv = _soc_monterey_ser_mem_info_get(unit, mem, &ser_mem_info);
    if (rv == SOC_E_NONE) {
        return rv;
    }

    rv = _soc_monterey_tcam_ser_mem_info_get(unit, mem, &tcam_idx);
    if (rv == SOC_E_NONE) {
        return rv;
    }

    return rv;
}

/*
 * Function:
 *      _soc_monterey_perform_ser_test
 * Purpose:
 *      Performs test operations common to TCAM and FIFO memory tests before
 *      invoking common SER test.
 * Parameters:
 *      unit        - (IN) Device Number
 *      test_data   - (IN) Test data required for SER test
 *      test_type   - (IN) How many indices to test for each memory
 *      mem_failed  - (OUT) Incremented when memories fail the test.
 * Returns:
 *  SOC_E_NONE if test passes, an error otherwise (multiple types of errors are
 *      possible.)
 */
soc_error_t
_soc_monterey_perform_ser_test (int unit, ser_test_data_t *test_data,
                              _soc_ser_test_t test_type,
                              int *mem_skipped, int *mem_failed)
{
    int skip_mem = FALSE;
    soc_error_t rv = SOC_E_NONE;
    uint32 flags = 0;

    if (soc_monterey_ser_test_skip_check (unit,test_data->mem)) {
        skip_mem = TRUE;
    }
#ifdef INCLUDE_XFLOW_MACSEC
    if(soc_feature(unit, soc_feature_xflow_macsec) &&
       ((ISEC_SP_TCAMm == test_data->mem) ||
       (ISEC_SC_TCAMm == test_data->mem))) {
        skip_mem = TRUE;
    }
#endif
    if (!skip_mem) {
         if (test_data->mem == ING_VLAN_VFI_MEMBERSHIPm ||
             test_data->mem == MMU_MTRI_BKPMETERINGCONFIG_MEM_0m ||
             test_data->mem == MMU_MTRO_EGRMETERINGCONFIG_MEMm) {
             flags |= SOC_INJECT_ERROR_2BIT_ECC;
        }
        rv = ser_test_mem(unit, flags, test_data, test_type, mem_failed);
    } else {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Memory %s skipped due to known issues.\n"),
                     SOC_MEM_NAME(unit,test_data->mem)));
        (*mem_skipped)++;
    }

    return rv;
}

/*
 * Function:
 *      soc_monterey_ser_tcam_test
 * Purpose:
 *      Tests that SER is working for fifo and SRAM memories
 * Parameters:
 *      unit        - (IN) Device Number
 *      test_type   - (IN) How many indices to test for each memory
 * Returns:
 *  the number of tests which are failed
 */
int
soc_monterey_ser_tcam_test (int unit, _soc_ser_test_t test_type) {
    uint32 tmp_entry[SOC_MAX_MEM_WORDS], fieldData[SOC_MAX_REG_FIELD_WORDS];
    ser_test_data_t test_data;
    int mem_failed = 0, mem_tests = 0, mem_skipped = 0;
    _soc_generic_ser_info_t *tcams = _soc_monterey_tcam_ser_info[unit];
    int i,rv=0;
    soc_acc_type_t acc_type;
    soc_field_t    test_field = VALIDf;

    for (i = 0; tcams[i].mem != INVALIDm; i++) {
        mem_tests++;
        if ( i >= MAX_HW_TCAMS) {
            mem_skipped++;
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "Memory %s skipped due to lack of test mechanism \
                                    for Software-protected TCAMS.\n"),
                         SOC_MEM_NAME(unit,test_data.mem)));
            continue;
        }

        acc_type = _SOC_ACC_TYPE_PIPE_ANY;

        if (tcams[i].mem == L3_DEFIPm) {
            test_field = VALID0f;
        } else if (tcams[i].mem == L3_DEFIP_PAIR_128m) {
            test_field = VALID0_LWRf;
        } else
#ifdef INCLUDE_XFLOW_MACSEC
            if (soc_feature(unit, soc_feature_xflow_macsec) &&
                (tcams[i].mem == ISEC_SP_TCAMm)) {
            test_field = MASKf;
        } else
#endif
        {
            test_field = VALIDf;
        }

        soc_ser_create_test_data(unit, tmp_entry, fieldData, SER_RANGE_ENABLEr,
                                 i, INVALIDf, tcams[i].mem, test_field,
                                 MEM_BLOCK_ANY, REG_PORT_ANY, acc_type, 0,
                                 &test_data);

       rv = _soc_monterey_perform_ser_test(unit, &test_data, test_type,
                                     &mem_skipped, &mem_failed);
        if (SOC_FAILURE(rv)) {
           LOG_CLI((BSL_META_U(unit,
           "TCAM SER test fail for mem: \t %s\n\n"),
            SOC_MEM_NAME(unit,test_data.mem)));
        }
    }

    LOG_CLI((BSL_META_U(unit,
               "\nTCAM memories tested on unit %d: %d\n"), unit, mem_tests));
    LOG_CLI((BSL_META_U(unit,
               "TCAM tests passed:\t%d\n"),
               mem_tests - mem_failed - mem_skipped));
    LOG_CLI((BSL_META_U(unit,
               "TCAM tests skipped:\t%d (use verbose option to see "
               "skipped memories)\n"),
               mem_skipped));
    LOG_CLI((BSL_META_U(unit,
               "TCAM tests failed:\t%d\n\n"), mem_failed));

    return mem_failed;
}

static int
soc_monterey_test_field_get (int unit, _soc_mem_ser_en_info_t * ser_mem_info,
                             int index, soc_field_t *field)
{
    if ((field == NULL) || (ser_mem_info == NULL)) {
        return BCM_E_PARAM;
    }

    *field = EVEN_PARITYf;

#ifdef INCLUDE_XFLOW_MACSEC
    if (soc_feature(unit, soc_feature_xflow_macsec)) {
        if (ser_mem_info->mem == ESEC_SC_TABLEm) {
            *field = SECTAG_OFFSETf;
        } else if (ser_mem_info->mem == ESEC_SA_TABLEm) {
            /* ESEC_SA_TABLEm has 4 banks of memory with individual SER control and status */
            switch (index % 4) {
                case 0:
                    ser_mem_info->en_ctrl.ctrl_type.en_reg =ECC_CTLSTS_ESATBL0r;
                    ser_mem_info->ecc1b_ctrl.ctrl_type.en_reg = ECC_CTLSTS_ESATBL0r;
                    break;
                case 1:
                    ser_mem_info->en_ctrl.ctrl_type.en_reg =ECC_CTLSTS_ESATBL1r;
                    ser_mem_info->ecc1b_ctrl.ctrl_type.en_reg = ECC_CTLSTS_ESATBL1r;
                    break;
                case 2:
                    ser_mem_info->en_ctrl.ctrl_type.en_reg =ECC_CTLSTS_ESATBL2r;
                    ser_mem_info->ecc1b_ctrl.ctrl_type.en_reg = ECC_CTLSTS_ESATBL2r;
                    break;
                case 3:
                    ser_mem_info->en_ctrl.ctrl_type.en_reg =ECC_CTLSTS_ESATBL3r;
                    ser_mem_info->ecc1b_ctrl.ctrl_type.en_reg = ECC_CTLSTS_ESATBL3r;
                    break;
                default:
                    break;
            }
        }
    }
#endif
    return BCM_E_NONE;
}

/*
 * Function:
 *      soc_monterey_ser_hardware_test
 * Purpose:
 *      Tests that SER is working for fifo and SRAM memories
 * Parameters:
 *      unit        - (IN) Device Number
 *      test_type   - (IN) How many indices to test for each memory
 * Returns:
 *  the number of tests which are failed
 */
int
soc_monterey_ser_hardware_test (int unit, _soc_ser_test_t test_type) {
    ser_test_data_t      test_data;
    int                  mem_failed = 0, mem_tests = 0, mem_skipped = 0;
    int                  i,j,rv;
    soc_acc_type_t       acc_type;
    _soc_mem_ser_en_info_t *ser_en_info = NULL;
    uint32 tmp_entry[SOC_MAX_MEM_WORDS], fieldData[SOC_MAX_REG_FIELD_WORDS];

    for (i = 0; _soc_monterey_ser_block_info[i].blocktype != 0; i++) {
        if ((_soc_monterey_ser_block_info[i].type == _SOC_MONTEREY_SER_TYPE_MEM) &&
            ((_soc_monterey_ser_block_info[i].blocktype == SOC_BLK_IPIPE) ||
             (_soc_monterey_ser_block_info[i].blocktype == SOC_BLK_EPIPE) ||
#ifdef INCLUDE_XFLOW_MACSEC
             (_soc_monterey_ser_block_info[i].blocktype == SOC_BLK_MACSEC) ||
#endif
             (_soc_monterey_ser_block_info[i].blocktype == SOC_BLK_MMU))) {
            ser_en_info = _soc_monterey_ser_block_info[i].info;
            for (j = 0; ser_en_info[j].mem != INVALIDm; j++) {
                mem_tests += 1;
                (void)soc_monterey_test_field_get(unit, &ser_en_info[j], 0,
                                            &test_data.test_field);
                acc_type = -1;
                soc_ser_create_test_data(unit, tmp_entry, fieldData,
                                         ser_en_info[j].en_ctrl.ctrl_type.en_reg,
                                         SOC_INVALID_TCAM_PARITY_BIT,
                                         ser_en_info[j].en_ctrl.en_fld,
                                         ser_en_info[j].mem,
                                         test_data.test_field,
                                         MEM_BLOCK_ANY,
                                         REG_PORT_ANY, acc_type, 0,
                                         &test_data);
                if ((test_data.mem_info == NULL) ||
                    (!(soc_mem_index_count(unit, ser_en_info[j].mem) > 0))) {
                    mem_skipped++;
                    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                                (BSL_META_U(unit,
                                 "Memory %s skipped due to lack of"
                                 " mem_info structure or being disabled.\n"),
                                 SOC_MEM_NAME(unit,test_data.mem)));
                    continue;
                }
                rv = _soc_monterey_perform_ser_test(unit, &test_data,
                                                  test_type, &mem_skipped,
                                                  &mem_failed);
                if (SOC_FAILURE(rv)) {
                    LOG_CLI((BSL_META_U(unit,
                                        " H/W SER  test fail for mem: \t %s\n\n"),
                             SOC_MEM_NAME(unit,test_data.mem)));
                }
            }
        }
    }

    LOG_CLI((BSL_META_U(unit,
               "\nH/W memories tested on unit %d: %d\n"), unit, mem_tests));
    LOG_CLI((BSL_META_U(unit,
               "H/W tests passed:\t%d\n"),
               mem_tests - mem_failed - mem_skipped));
    LOG_CLI((BSL_META_U(unit,
               "H/W tests skipped:\t%d (use verbose option to see "
               "skipped memories)\n"),
               mem_skipped));
    LOG_CLI((BSL_META_U(unit,
               "H/W tests failed:\t%d\n\n"), mem_failed));
    return mem_failed;
}


/*
 * Function:
 *      soc_monterey_ser_test
 * Purpose:
 *      Tests that SER is working for all supported memories.
 * Parameters:
 *      unit        - (IN) Device Number
 *      test_type   - (IN) How many indices to test for each memory
 * Returns:
 *  SOC_E_NONE if test passes, an error otherwise (multiple types of errors are
 *      possible.)
 */
soc_error_t
soc_monterey_ser_test (int unit, _soc_ser_test_t test_type) {
    int errors = 0;

    /*Test TCAM memories*/
    errors += soc_monterey_ser_tcam_test(unit, test_type);

    /*Test for FIFO memories*/
    errors += soc_monterey_ser_hardware_test(unit, test_type);

    if (errors == 0) {
        return SOC_E_NONE;
    } else {
        return SOC_E_FAIL;
    }
}

#define TR_MEM_NAME_SIZE_MAX    100

#ifdef SOC_MEM_NAME
#define TR_TEST_MEM_NAME_GET(_unit, _msg, _mem) do {                    \
        const char *_mem_name = SOC_MEM_NAME(_unit_, _mem);             \
        if (sal_strlen(_mem_name) < TR_MEM_NAME_SIZE_MAX) {             \
            sal_strcpy(_msg, _mem_name);                                \
        }                                                               \
    } while (0)
#else
#define TR_TEST_MEM_NAME_GET(_unit, _msg, _mem)
#endif

#define TR_TEST_MEM_PRINT(_unit, _msg, _mem) do {                       \
        sal_sprintf(_msg, "Mem ID: %d", _mem);                          \
        TR_TEST_MEM_NAME_GET(_unit, _msg, _mem);                        \
        LOG_CLI((BSL_META_U(_unit,                                      \
                            "Memory %s is valid, but not currently testable.\n"), \
                 _msg));                                                \
    } while (0)

/*
 * Function:
 *      soc_monterey_ser_inject_or_test_mem
 * Purpose:
 *      Provide a common function for injecting errors and testing single
 *      single memories
 * Parameters:
 *      unit        - (IN) Device Number
 *      mem         - (IN) The memory into which an error will be injected
 *      pipeTarget  - (IN) The pipe (x/y) to use when injecting the error
 *      index       - (IN) The index into which the error will be injected.
 *      test_type   - (IN) How many indices to test in the passes memory
 *      cmd         - (IN) TRUE if a command-line test is desired.
 * Returns:
 *      SOC_E_NONE if test passes, an error otherwise (multiple types of errors
 *      are possible.)
 */
soc_error_t
soc_monterey_ser_inject_or_test_mem (int unit, soc_mem_t mem, int pipe_target,
                                   int block, int index,
                                   _soc_ser_test_t test_type,
                                   int inject_only, int cmd, uint32 flags)
{
    uint32 tmp_entry[SOC_MAX_MEM_WORDS], fieldData[SOC_MAX_REG_FIELD_WORDS];
    ser_test_data_t tcam_test_data, hw_test_data, *test_data;
    int             tcam_idx = 0;
        _soc_generic_ser_info_t *tcams = _soc_monterey_tcam_ser_info[unit];
    int error_count = 0, skip_count = 0, found_mem = FALSE;
    char fail_message[400];
    soc_error_t rv = SOC_E_NONE;
    soc_field_t test_field = INVALIDf;
    soc_acc_type_t soc_acc_type_target = _SOC_ACC_TYPE_PIPE_ANY;
    _soc_mem_ser_en_info_t * ser_mem_info = NULL;
    int mem_has_ecc = 0, is_tcam = 0, is_fifo_mem = 0;


    rv = _soc_monterey_tcam_ser_mem_info_get(unit, mem, &tcam_idx);
    if (rv == SOC_E_NONE) {
        if (tcams[tcam_idx].mem == L3_DEFIPm) {
            test_field = VALID0f;
        } else if (tcams[tcam_idx].mem == L3_DEFIP_PAIR_128m) {
            test_field = VALID0_LWRf;
        } else
#ifdef INCLUDE_XFLOW_MACSEC
            if (soc_feature(unit, soc_feature_xflow_macsec) &&
                (tcams[tcam_idx].mem == ISEC_SP_TCAMm)) {
            test_field = MASKf;
        } else
#endif
        {
            test_field = VALIDf;
        }
        soc_ser_create_test_data(unit, tmp_entry, fieldData,
                                 SER_RANGE_ENABLEr, tcam_idx, INVALIDf, mem,
                                 test_field, block, REG_PORT_ANY,
                                 soc_acc_type_target, index, &tcam_test_data);
        found_mem = TRUE;
        is_tcam = 1;
    }

        rv = _soc_monterey_ser_mem_info_get (unit, mem, &ser_mem_info);
        if (rv == SOC_E_NONE) {
            if (mem == ING_L3_NEXT_HOPm) {
                test_field= ECC_1f;
            } else {
#ifdef INCLUDE_XFLOW_MACSEC
                if (soc_feature(unit, soc_feature_xflow_macsec)) {
                    SOC_IF_ERROR_RETURN
                            (soc_monterey_test_field_get(unit, ser_mem_info, index,
                                                    &test_field));
                }
#endif
            }
        soc_ser_create_test_data(unit, tmp_entry, fieldData,
                           ser_mem_info->en_ctrl.ctrl_type.en_reg,
                           SOC_INVALID_TCAM_PARITY_BIT,
                           ser_mem_info->en_ctrl.en_fld,
                           mem, test_field, block,
                           REG_PORT_ANY, soc_acc_type_target,
                           index, &hw_test_data);
        if (SOC_REG_IS_VALID(unit, ser_mem_info->ecc1b_ctrl.ctrl_type.en_reg) &&
                        (INVALIDf != ser_mem_info->ecc1b_ctrl.en_fld)) {
                                        flags |= SOC_INJECT_ERROR_2BIT_ECC;
        }
         SOC_IF_ERROR_RETURN
                     (ser_test_mem_index_remap(unit, &hw_test_data, &mem_has_ecc));
         if (mem_has_ecc) {
             flags |= SOC_INJECT_ERROR_2BIT_ECC;
         }
/* ING_VLAN_VFI_MEMBERSHIP is ECC protected, however
         * there is no 1bit error reporting control register for it.
                  */
         if (mem == ING_VLAN_VFI_MEMBERSHIPm) {
             flags |= SOC_INJECT_ERROR_2BIT_ECC;
         }
         found_mem = TRUE;
         is_fifo_mem = 1;
    }
    if (found_mem) {
       test_data = is_tcam ? &tcam_test_data : &hw_test_data;
                test_data->acc_type = _SOC_ACC_TYPE_PIPE_ANY;
                if (inject_only) {
                    /* Check if the memory is to be skipped */
                    if (soc_monterey_ser_test_skip_check(unit,mem)) {
                       TR_TEST_MEM_PRINT(unit, fail_message, mem);
                       return SOC_E_UNAVAIL;
                    }

                    /*Disable parity*/
                    SOC_IF_ERROR_RETURN(_ser_test_parity_control(unit, test_data, 0));
                    /*Read the memory (required for successful injection)*/
                    SOC_IF_ERROR_RETURN(ser_test_mem_read(unit, 0, test_data));
                     /*Inject error*/
                    SOC_IF_ERROR_RETURN(soc_ser_test_inject_full(unit, flags,
                                                                 test_data));
                    /*Enable parity*/
                    rv = _ser_test_parity_control(unit, test_data, 1);
                } else {
                    if (cmd) {
                        tcam_test_data.acc_type = _SOC_ACC_TYPE_PIPE_ANY;
                        ser_test_cmd_generate(unit, &tcam_test_data);
                        hw_test_data.acc_type = _SOC_ACC_TYPE_PIPE_ANY;
                        ser_test_cmd_generate(unit, &hw_test_data);
                    } else {
                        if (is_tcam) {
                            tcam_test_data.acc_type = _SOC_ACC_TYPE_PIPE_ANY;
                            rv = _soc_monterey_perform_ser_test(unit, &tcam_test_data, test_type,
                                                              &skip_count, &error_count);
                            if (SOC_SUCCESS(rv)) {
                                LOG_CLI((BSL_META_U(unit,
                                        "TCAM SER test PASSED for memory %s\n\n"),
                                        tcam_test_data.mem_name));
                            } else {
                                LOG_CLI((BSL_META_U(unit,
                                        "TCAM SER test FAILED for memory %s\n\n"),
                                        tcam_test_data.mem_name));
                            }
                    }

                    if (is_fifo_mem) {
                        hw_test_data.acc_type = _SOC_ACC_TYPE_PIPE_ANY;
                       rv = _soc_monterey_perform_ser_test(unit, &hw_test_data, test_type,
                                                                     &skip_count, &error_count);
                                      if (SOC_SUCCESS(rv)) {
                                           LOG_CLI((BSL_META_U(unit,
                                                   "H/W SER test PASSED for memory %s\n\n"),
                                                   hw_test_data.mem_name));
                                       } else {
                                           LOG_CLI((BSL_META_U(unit,
                                                  "H/W SER test FAILED for memory %s\n\n"),
                                                   hw_test_data.mem_name));
                                       }
                                   }

                                   if (skip_count != 0 ) {
                                       LOG_CLI((BSL_META_U(unit,
                                                  "Test skipped due to known issues with "
                                                  "this memory.\n")));
                                   }
                               }
                           }
    } else {
        TR_TEST_MEM_PRINT(unit, fail_message, mem);
        rv = SOC_E_UNAVAIL;
    }
    return rv;
}
#undef TR_TEST_MEM_PRINT

/*
 * Function:
 *      soc_monterey_ser_inject_error
 * Purpose:
 *      Injects an error into a single monterey memory
 * Parameters:
 *      unit        - (IN) Device Number
 *      mem         - (IN) The memory into which an error will be injected
 *      pipeTarget  - (IN) The pipe (x/y) to use when injecting the error
 *      index       - (IN) The index into which the error will be injected.
 */
soc_error_t
soc_monterey_ser_inject_error (int unit, uint32 flags, soc_mem_t mem,
                             int pipe_target, int block, int index)
{
    /* Check if memory needs to be skipped */
    if (ser_monterey_test_fun.injection_support) {
        SOC_IF_ERROR_RETURN(
            soc_monterey_ser_error_injection_support(unit, mem, pipe_target));
    }

    return soc_monterey_ser_inject_or_test_mem(unit, mem, pipe_target, block,
                                             index, SER_SINGLE_INDEX, TRUE,
                                             FALSE, flags);
}

/*
 * Function:
 *      soc_monterey_ser_test_mem
 * Purpose:
 *      Perform SER test on a single memory, or generate a test the user can
 *      enter by the command line.
 * Parameters:
 *      unit        - (IN) Device Number
 *      mem         - (IN) The memory into which an error will be injected
 *      test_type   - (IN) How many indices to test in the passes memory
 *      cmd         - (IN) TRUE if a command-line test is desired.
 * Returns:
 *      SOC_E_NONE if test passes, an error otherwise (multiple types of errors
 *      are possible.)
 */
soc_error_t
soc_monterey_ser_test_mem (int unit, soc_mem_t mem, _soc_ser_test_t test_type,
                        int cmd)
{
    return soc_monterey_ser_inject_or_test_mem (unit, mem, _SOC_ACC_TYPE_PIPE_ANY,
                                              MEM_BLOCK_ANY, 0,
                                              test_type, FALSE, cmd, FALSE);
}
#endif /*defined(SER_TR_TEST_SUPPORT)*/

soc_error_t
soc_monterey_gatingconfig_init(int unit) {


     uint32 rval = 0;

     soc_reg_field_set(unit, IP_SER_CLK_GATE_CTRLr , &rval, DISABLE_CLK_GATEf, 1);
     SOC_IF_ERROR_RETURN(WRITE_IP_SER_CLK_GATE_CTRLr(unit ,rval));
     rval = 0;
     soc_reg_field_set(unit, ICFG_CLK_GATING_CONTROLr , &rval, DISABLE_CLK_GATINGf, 1);
     soc_reg_field_set(unit, ICFG_CLK_GATING_CONTROLr , &rval, DISABLE_CLK_GATING_DATA_BUSf,1);
     soc_reg_field_set(unit, ICFG_CLK_GATING_CONTROLr , &rval, DISABLE_CLK_GATING_IPRC_DATAf,1);
     SOC_IF_ERROR_RETURN(WRITE_ICFG_CLK_GATING_CONTROLr(unit ,rval));
     rval = 0;
     soc_reg_field_set(unit, EGR_EFPPARS_CLK_GATING_CONTROLr , &rval, DISABLE_CLK_GATINGf, 1);
     soc_reg_field_set(unit, EGR_EFPPARS_CLK_GATING_CONTROLr , &rval, DISABLE_CLK_GATING_DATA_BUSf,1);
     soc_reg_field_set(unit, EGR_EFPPARS_CLK_GATING_CONTROLr , &rval, DISABLE_CLK_GATING_ARRAYSf,1);
     SOC_IF_ERROR_RETURN(WRITE_EGR_EFPPARS_CLK_GATING_CONTROLr(unit ,rval));
     rval = 0;

     soc_reg_field_set(unit, EGR_EHCPM_CLK_GATING_CONTROLr , &rval, DISABLE_CLK_GATINGf, 1);
     soc_reg_field_set(unit, EGR_EHCPM_CLK_GATING_CONTROLr, &rval, DISABLE_CLK_GATING_DATA_BUSf,1);
     soc_reg_field_set(unit, EGR_EHCPM_CLK_GATING_CONTROLr, &rval, DISABLE_CLK_GATING_ARRAYSf,1);
     SOC_IF_ERROR_RETURN(WRITE_EGR_EHCPM_CLK_GATING_CONTROLr(unit ,rval));
     rval = 0;

     soc_reg_field_set(unit, IPARS_CLK_GATING_CONTROLr , &rval, DISABLE_CLK_GATINGf, 1);
     soc_reg_field_set(unit, IPARS_CLK_GATING_CONTROLr , &rval, DISABLE_CLK_GATING_DATA_BUSf,1);
     soc_reg_field_set(unit, IPARS_CLK_GATING_CONTROLr , &rval, DISABLE_CLK_GATING_IPRC_DATAf,1);
     SOC_IF_ERROR_RETURN(WRITE_IPARS_CLK_GATING_CONTROLr(unit ,rval));
     rval = 0;


     soc_reg_field_set(unit, EGR_EIPT_CLK_GATING_CONTROLr , &rval, DISABLE_CLK_GATINGf, 1);
     soc_reg_field_set(unit, EGR_EIPT_CLK_GATING_CONTROLr , &rval, DISABLE_CLK_GATING_ARRAYSf, 1);
     SOC_IF_ERROR_RETURN(WRITE_EGR_EIPT_CLK_GATING_CONTROLr(unit ,rval));
     rval = 0;

     soc_reg_field_set(unit, EGR_EL3_CLK_GATING_CONTROLr , &rval, DISABLE_CLK_GATINGf, 1);
     soc_reg_field_set(unit, EGR_EL3_CLK_GATING_CONTROLr , &rval, DISABLE_CLK_GATING_DATA_BUSf,1);
     soc_reg_field_set(unit, EGR_EL3_CLK_GATING_CONTROLr , &rval, DISABLE_CLK_GATING_SER_STATUS_BUSf,1);
     soc_reg_field_set(unit, EGR_EL3_CLK_GATING_CONTROLr , &rval, DISABLE_CLK_GATING_ARRAYSf,1);
     SOC_IF_ERROR_RETURN(WRITE_EGR_EL3_CLK_GATING_CONTROLr(unit ,rval));
     rval = 0;

     soc_reg_field_set(unit,IVXLT_CONTROLr, &rval, DISABLE_CLK_GATINGf, 1);
     soc_reg_field_set(unit, IVXLT_CONTROLr , &rval, VLAN_XLATE_DISABLE_CLK_GATINGf, 1);
     SOC_IF_ERROR_RETURN(WRITE_IVXLT_CONTROLr(unit ,rval));
     rval = 0;

     soc_reg_field_set(unit, EGR_EPMOD_CLK_GATING_CONTROLr , &rval, DISABLE_CLK_GATINGf, 1);
     soc_reg_field_set(unit, EGR_EPMOD_CLK_GATING_CONTROLr , &rval, DISABLE_CLK_GATING_DATA_BUSf,1);
     soc_reg_field_set(unit, EGR_EPMOD_CLK_GATING_CONTROLr , &rval, DISABLE_CLK_GATING_ARRAYSf,1);
     SOC_IF_ERROR_RETURN(WRITE_EGR_EPMOD_CLK_GATING_CONTROLr(unit ,rval));
     rval = 0;

     soc_reg_field_set(unit, EGR_ESW_CLK_GATING_CONTROLr , &rval, DISABLE_CLK_GATINGf, 1);
     soc_reg_field_set(unit, EGR_ESW_CLK_GATING_CONTROLr , &rval, DISABLE_CLK_GATING_ARRAYSf, 1);
     SOC_IF_ERROR_RETURN(WRITE_EGR_ESW_CLK_GATING_CONTROLr(unit ,rval));
     rval = 0;

     soc_reg_field_set(unit, EGR_ESW_CLK_GATING_CONTROLr , &rval, DISABLE_CLK_GATINGf, 1);
     SOC_IF_ERROR_RETURN(WRITE_EGR_ESW_CLK_GATING_CONTROLr(unit ,rval));
     rval = 0;
     soc_reg_field_set(unit, IVP_CLK_GATING_CONTROLr , &rval, DISABLE_CLK_GATINGf, 1);
     SOC_IF_ERROR_RETURN(WRITE_IVP_CLK_GATING_CONTROLr(unit ,rval));
     rval = 0;
     soc_reg_field_set(unit, EGR_EVLAN_CLK_GATING_CONTROLr , &rval, DISABLE_CLK_GATINGf, 1);
     soc_reg_field_set(unit, EGR_EVLAN_CLK_GATING_CONTROLr , &rval, DISABLE_CLK_GATING_IN_HASH_TABLESf, 1);
     soc_reg_field_set(unit, EGR_EVLAN_CLK_GATING_CONTROLr , &rval, DISABLE_CLK_GATING_ARRAYSf, 1);
     SOC_IF_ERROR_RETURN(WRITE_EGR_EVLAN_CLK_GATING_CONTROLr(unit ,rval));
     rval = 0;

     soc_reg_field_set(unit, IMPLS_CLK_GATING_CONTROLr , &rval, DISABLE_CLK_GATINGf, 1);
     SOC_IF_ERROR_RETURN(WRITE_IMPLS_CLK_GATING_CONTROLr(unit ,rval));
     rval = 0;

     soc_reg_field_set(unit,EGR_EFP_CLK_GATING_CONTROLr , &rval, DISABLE_CLK_GATINGf, 1);
     soc_reg_field_set(unit, EGR_EFP_CLK_GATING_CONTROLr , &rval, DISABLE_CLK_GATING_DATA_BUSf,1);
     soc_reg_field_set(unit, EGR_EFP_CLK_GATING_CONTROLr , &rval, DISABLE_CLK_GATING_ARRAYSf,1);
     WRITE_EGR_EFP_CLK_GATING_CONTROLr(unit ,rval);
     rval = 0;

     soc_reg_field_set(unit,EGR_EFPMOD_CLK_GATING_CONTROLr , &rval, DISABLE_CLK_GATINGf, 1);
     soc_reg_field_set(unit, EGR_EFPMOD_CLK_GATING_CONTROLr , &rval, DISABLE_CLK_GATING_DATA_BUSf,1);
     soc_reg_field_set(unit, EGR_EFPMOD_CLK_GATING_CONTROLr , &rval, DISABLE_CLK_GATING_ARRAYSf,1);
     WRITE_EGR_EFPMOD_CLK_GATING_CONTROLr(unit ,rval);

     soc_reg_field_set(unit, IDISC_CLK_GATING_CONTROLr , &rval, DISABLE_CLK_GATINGf, 1);
     WRITE_IDISC_CLK_GATING_CONTROLr(unit ,rval);
     rval = 0;

     soc_reg_field_set(unit, IL2LU_CONTROLr , &rval, DISABLE_CLK_GATINGf, 1);
     soc_reg_field_set(unit, IL2LU_CONTROLr , &rval, L2_ISS_DISABLE_CLK_GATINGf, 1);
     soc_reg_field_set(unit, IL2LU_CONTROLr , &rval, L2_IPRC_DATA_DISABLE_CLK_GATINGf, 1);
     SOC_IF_ERROR_RETURN(WRITE_IL2LU_CONTROLr(unit ,rval));
     rval = 0;

     soc_reg_field_set(unit, IL3LU_CONTROLr , &rval, DISABLE_CLK_GATINGf, 1);
     soc_reg_field_set(unit, IL3LU_CONTROLr , &rval, L3_ISS_DISABLE_CLK_GATINGf, 1);
     soc_reg_field_set(unit, IL3LU_CONTROLr , &rval, L3_IPRC_DATA_DISABLE_CLK_GATINGf, 1);
     SOC_IF_ERROR_RETURN(WRITE_IL3LU_CONTROLr(unit ,rval));
     rval = 0;
     soc_reg_field_set(unit, ILPM_CONTROLr , &rval, DISABLE_CLK_GATINGf, 1);
     soc_reg_field_set(unit, ILPM_CONTROLr , &rval, LPM_IPRC_DATA_DISABLE_CLK_GATINGf, 1);
     SOC_IF_ERROR_RETURN(WRITE_ILPM_CONTROLr(unit ,rval));
     rval = 0;

     soc_reg_field_set(unit, IRSEL1_CLK_GATING_CONTROLr , &rval, DISABLE_CLK_GATINGf, 1);
     SOC_IF_ERROR_RETURN(WRITE_IRSEL1_CLK_GATING_CONTROLr(unit ,rval));
     rval = 0;

     soc_reg_field_set(unit, ISW1_CLK_GATING_CONTROLr , &rval, DISABLE_CLK_GATINGf, 1);
     SOC_IF_ERROR_RETURN(WRITE_ISW1_CLK_GATING_CONTROLr(unit ,rval));
     rval = 0;

     soc_reg_field_set(unit, IFP_CLK_GATING_CONTROLr , &rval, DISABLE_CLK_GATINGf, 1);
     SOC_IF_ERROR_RETURN(WRITE_IFP_CLK_GATING_CONTROLr(unit ,rval));
     rval = 0;

     soc_reg_field_set(unit, IRSEL2_CLK_GATING_CONTROLr , &rval, DISABLE_CLK_GATINGf, 1);
     soc_reg_field_set(unit, IRSEL2_CLK_GATING_CONTROLr , &rval, DISABLE_CLK_GATING_ARRAYSf, 1);
     SOC_IF_ERROR_RETURN(WRITE_IRSEL2_CLK_GATING_CONTROLr(unit ,rval));
     rval = 0;

     soc_reg_field_set(unit,  ISW2_CLK_GATING_CTRLr , &rval, DISABLE_CLK_GATINGf, 1);
     soc_reg_field_set(unit,  ISW2_CLK_GATING_CTRLr , &rval, DISABLE_CLK_GATING_ARRAYSf, 1);
     SOC_IF_ERROR_RETURN(WRITE_ISW2_CLK_GATING_CTRLr(unit ,rval));
     rval = 0;
     return SOC_E_NONE;
}

#ifdef INCLUDE_XFLOW_MACSEC
/*
 * Function:
 *      soc_monterey_process_macsec_intr
 * Description:
 *      Monterey specific DPC function to service MACSEC interrupts
 * Parameters:
 *      unit_vp       - Device number
 *      d1            - IRQ4 mask
 *      d2            - IRQ4 number (to distinguish between
 *                      different types of interrupts).
 */
void
soc_monterey_process_macsec_intr(void *unit_vp, void *d1,
                                 void *d2, void *d3, void *d4)
{
    int unit = PTR_TO_INT(unit_vp);
    int irq4_num = PTR_TO_INT(d4);
    int mask = PTR_TO_INT(d2);
    uint8 rbi;
    const _soc_monterey_ser_route_block_t *rb;
    uint64 rb_rval64;

    for (rbi = 0; ; rbi++) {
        rb = &_soc_monterey_macsec_ser_route_blocks_irq4[rbi];
        if (rb->cmic_bit == 0) {
            break;
        }

        if (rb->cmic_bit != (1 << irq4_num)) {
            continue;
        }
        COMPILER_64_ZERO(rb_rval64);
        /* Read per route block status register */
        (void)soc_reg_get(unit, rb->status_reg, REG_PORT_ANY, 0, &rb_rval64);
        if (COMPILER_64_IS_ZERO(rb_rval64)) {
            continue;
        }

        /* Process ECC interrupts */
        if(soc_reg64_field32_get(unit, rb->status_reg, rb_rval64,
                                 ECCERR_INTf)) {
            (void)xflow_macsec_process_ecc_intr(unit, NULL);
        }
        /* Process MACSEC functional interrupts */
        (void)xflow_macsec_process_functional_intr(unit, rb_rval64);

    } /* For loop */

    /* Enable the interrupts */
    if (d2 != NULL) {
        (void)soc_cmicm_intr4_enable(unit, mask);
    }

    return;
}
#endif
/*
 * Function:
 *      soc_monterey_process_pm_intr
 * Description:
 *      Monterey specific DPC function to service PM interrupts
 * Parameters:
 *      unit_vp       - Device number
 *      d1            - IRQ4 mask
 *      d2            - IRQ4 number (to distinguish between
 *                      different types of interrupts).
 */
void
soc_monterey_process_pm_intr(void *unit_vp, void *d1,
                             void *d2, void *d3, void *d4)
{
    int unit = PTR_TO_INT(unit_vp);
    int irq4_num = PTR_TO_INT(d4);
    int mask = PTR_TO_INT(d2);
    uint8 rbi;
    int port = -1, block_info_idx;
    const _soc_monterey_pm_cpri_intr_info_t *rb;
    portmod_cpri_ecc_intr_info_t ecc_info;
    _soc_monterey_pm_ecc_info_t  pm_ecc_info;
    int portmod_pm_id = -1;
    int rv = 0;

    for (rbi = 0; ; rbi++) {
        rb = &_soc_monterey_pm_cpri_intr_info_irq4[rbi];
        if (rb->cmic_bit == 0) {
            break;
        }

        if (rb->cmic_bit != (1 << irq4_num)) {
            continue;
        }

        SOC_BLOCK_ITER(unit, block_info_idx, rb->blocktype) {
            if (SOC_BLOCK_INFO(unit, block_info_idx).number == rb->id) {
                port = SOC_BLOCK_PORT(unit, block_info_idx);
                break;
            }
        }

        if (port != -1) {

            /* get portmod  pm_id from the phy port. */
            rv = portmod_phy_pm_id_get(unit, port, &portmod_pm_id);

            if (rv == SOC_E_NONE) {
                /* Process other CPRI interrupts */
                (void)portmod_cpri_pm_interrupt_process(unit, portmod_pm_id);

                memset(&ecc_info, 0, sizeof(ecc_info));

                /* Process ECC interrupts */
                (void)portmod_cpri_pm_ecc_interrupt_status_get(unit, portmod_pm_id,
                                                               &ecc_info);

                /* Report ECC errors */
                if (ecc_info.ecc_num_bits_err != PORTMOD_PM_CPRI_ECC_ERR_NONE) {
                    pm_ecc_info.type            = _SOC_PARITY_TYPE_ECC;
                    pm_ecc_info.ecc_err_type    = ecc_info.ecc_num_bits_err;
                    pm_ecc_info.mem             = ecc_info.err_mem_info;
                    pm_ecc_info.mem_index       = ecc_info.err_addr;
                    pm_ecc_info.port            = ecc_info.port;
                    pm_ecc_info.mem_str         = ecc_info.mem_str;

                    _soc_monterey_ser_process_pm_ecc(unit,
                                            ecc_info.port, &pm_ecc_info);

                    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                                (BSL_META_U(unit, "ECC interrupt info\n"
                                "m - %s, ecc_num_bits - %d, addr %x\n"),
                                SOC_MEM_NAME(unit, ecc_info.err_mem_info),
                                ecc_info.ecc_num_bits_err,
                                ecc_info.err_addr));
                }
            } /* rv == SOC_E_NONE */
        } /* (port != -1) */
    } /* For loop */

    /* Enable the interrupts */
    if (d2 != NULL) {
        (void)soc_cmicm_intr4_enable(unit, mask);
    }

    return;
}

int
soc_monterey_mmu_pfc_tx_config_set(int unit,
                    bcm_port_t port, int pfc_enable)
{
    soc_info_t *si;
    soc_reg_t reg;
    int obm, pgw, pgw_inst;
    int phy_port, subport;
    static const soc_reg_t obm_fc_config_regs[] = {
        IDB_OBM0_FLOW_CONTROL_CONFIGr, IDB_OBM1_FLOW_CONTROL_CONFIGr,
        IDB_OBM2_FLOW_CONTROL_CONFIGr, IDB_OBM3_FLOW_CONTROL_CONFIGr,
        IDB_OBM4_FLOW_CONTROL_CONFIGr, IDB_OBM5_FLOW_CONTROL_CONFIGr,
        IDB_OBM6_FLOW_CONTROL_CONFIGr, IDB_OBM7_FLOW_CONTROL_CONFIGr
    };
    uint64 rval64;

    si = &SOC_INFO(unit);
    phy_port = si->port_l2p_mapping[port];

    if (phy_port == -1) {
        return SOC_E_PARAM;
    }
    if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port) ||
        IS_MACSEC_PORT(unit, port)) {
        /*  nothing to do for non-idb ports */
        return SOC_E_NONE;
    }

    pgw = si->port_group[port];

    /* OBM group (num groups = num tsc's) belongs to PGW0/PGW1  */
    obm = si->port_serdes[port];
    obm %= MONTEREY_TSCS_PER_PGW;
    pgw = si->port_group[port];
    pgw_inst = (pgw | SOC_REG_ADDR_INSTANCE_MASK);
    subport = (phy_port - 1) % MONTEREY_PORTS_PER_TSC;

    COMPILER_64_ZERO(rval64);
    reg = obm_fc_config_regs[obm];

    SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, pgw_inst, subport, &rval64));
    soc_reg64_field32_set(unit, reg, &rval64, FC_TYPEf, pfc_enable ? 1 : 0);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, pgw_inst, subport, rval64));
    return SOC_E_NONE;
}

/*
 * Apache chip driver functions.
 */
soc_functions_t soc_monterey_drv_funs = {
    _soc_monterey_misc_init,
    _soc_monterey_mmu_init,
    _soc_monterey_age_timer_get,
    _soc_monterey_age_timer_max_get,
    _soc_monterey_age_timer_set,
    NULL,
    _soc_monterey_mdio_reg_read,
    _soc_monterey_mdio_reg_write,
     soc_monterey_bond_info_init,
    _soc_monterey_misc_deinit
};


#endif /* BCM_MONTEREY_SUPPORT */
