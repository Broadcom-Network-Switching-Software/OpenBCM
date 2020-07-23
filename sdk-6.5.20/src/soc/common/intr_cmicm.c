/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC CMICm Interrupt Handlers
 *
 * NOTE: These handlers are called from an interrupt context, so their
 *       actions are restricted accordingly.
 */

#include <shared/bsl.h>

#include <sal/core/libc.h>
#include <shared/alloc.h>
#include <sal/core/spl.h>
#include <sal/core/sync.h>
#include <sal/core/dpc.h>

#include <soc/debug.h>
#include <soc/drv.h>
#include <soc/dma.h>
#include <soc/i2c.h>
#include <soc/cmicm.h>
#include <soc/feature.h>
#include <soc/intr.h>
#include <soc/mem.h>
#ifdef BCM_PETRA_SUPPORT
#include <soc/dpp/PPD/ppd_api_oam.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_mact_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/dpp/JER/jer_mgmt.h>
#include <soc/dpp/JER/jer_intr.h>
#endif 
#ifdef BCM_KATANA_SUPPORT
#include <soc/katana.h>
#endif /* BCM_KATANA_SUPPORT */
#ifdef BCM_TRIUMPH3_SUPPORT
#include <soc/triumph3.h>
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
#include <soc/trident2.h>
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_MONTEREY_SUPPORT
#include <soc/monterey.h>
#endif /* BCM_MONTEREY_SUPPORT */
#ifdef BCM_APACHE_SUPPORT
#include <soc/apache.h>
#endif /* BCM_APACHE_SUPPORT */
#ifdef BCM_HURRICANE2_SUPPORT
#include <soc/hurricane2.h>
#endif /* BCM_HURRICANE2_SUPPORT */
#ifdef BCM_GREYHOUND_SUPPORT
#include <soc/greyhound.h>
#endif /* BCM_GREYHOUND_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
#include <soc/tomahawk.h>
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
#include <soc/trident3.h>
#endif /* BCM_TRIDENT3_SUPPORT */
#ifdef BCM_DFE_SUPPORT
#include <soc/dfe/cmn/dfe_drv.h>
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
#include <soc/tomahawk3.h>
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_GREYHOUND2_SUPPORT
#include <soc/greyhound2.h>
#endif /* BCM_GREYHOUND_SUPPORT */
#if defined(BCM_TIMESYNC_TIME_CAPTURE_SUPPORT)
#if defined(BCM_ESW_SUPPORT)
extern void soc_esw_timesync_ts_intr(int unit);
#endif
#endif /* defined(BCM_GREYHOUND2_SUPPORT) || defined(BCM_TIMESYNC_TIME_CAPTURE_SUPPORT) */

#ifdef BCM_CMICM_SUPPORT

#define HOST_IRQ_MASK_OFFSET_DIFF (CMIC_CMC0_UC0_IRQ_MASK0_OFFSET              \
                - CMIC_CMC0_PCIE_IRQ_MASK0_OFFSET)

static uint32 soc_cmicm_host_irq_offset[SOC_MAX_NUM_DEVICES] = {0};

#define INTR_MASK_OFFSET(_u,_a) (_a + soc_cmicm_host_irq_offset[_u])

#ifdef INCLUDE_KNET
#include <soc/knet.h>
#define IRQ_MASK0_SET_FUNC soc_knet_irq_mask_set
#else
#define IRQ_MASK0_SET_FUNC soc_pci_write
#endif

#define IRQ_MASK0_SET(_u,_a,_m) IRQ_MASK0_SET_FUNC(_u,INTR_MASK_OFFSET(_u,_a),_m)
#define IRQ_MASKx_SET(_u,_a,_m) soc_pci_write(_u,INTR_MASK_OFFSET(_u,_a),_m)


#include <soc/shared/mos_intr_common.h>
#include <soc/uc_msg.h>

/* Declare static functions for interrupt handler array */
STATIC void soc_cmicm_intr_schan_done(int unit, uint32 vchan);
STATIC void soc_cmicm_intr_miim_op(int unit, uint32 ignored);
STATIC void soc_cmicm_intr_tdma_done(int unit, uint32 ignored);
STATIC void soc_cmicm_intr_tslam_done(int unit, uint32 ignored);
STATIC void soc_cmicm_intr_stat_dma(int unit, uint32 ignored);
STATIC void soc_cmicm_intr_ccmdma_done(int unit, uint32 vchan);
STATIC void soc_cmicm_fifo_dma_done(int unit, uint32 vchan);
STATIC void soc_cmicm_intr_sbusdma_done(int unit, uint32 vchan);
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
STATIC void soc_cmicm_intr_common_schan_done(int unit, uint32 ignored);
STATIC void soc_cmicm_chip_func_intr(int unit, uint32 val);
STATIC void soc_cmicm_intr_link_stat(int unit, uint32 ignored);
STATIC void soc_cmicm_parity_intr(int unit, uint32 val);
STATIC void soc_cmicm_block_lo_intr(int unit, uint32 val);
STATIC void soc_cmicm_link_stat(int unit, uint32 ignored);
STATIC void soc_ser_engine_intr(int unit, uint32 val);
STATIC void soc_cmicm_timesync_intr(int unit, uint32 val);
STATIC void soc_cmicm_pcie_link_down(int unit, uint32 val);
STATIC void soc_cmicdv2_parity_intr(int unit, uint32 val);
STATIC void soc_cmicdv2_chip_parity_intr(int unit, uint32 val);
STATIC void soc_cmicdv2_block_intr4(int unit, uint32 val);
STATIC void soc_cmicdv2_block_intr5(int unit, uint32 val);
STATIC void soc_cmicdv4_block_intr6(int unit, uint32 val);
#endif
#ifdef INCLUDE_RCPU
STATIC void soc_cmicm_rcpu_intr_miim_op(int unit, uint32 ignored);
#endif


/*
 * SOC Interrupt Table
 *
 * The table is stored in priority order:  Interrupts that are listed
 * first have their handlers called first.
 *
 * A handler can clear more than one interrupt bit to prevent a
 * subsequent handler from being called.  E.g., if the DMA CHAIN_DONE
 * handler clears both CHAIN_DONE and DESC_DONE, the DESC_DONE handler
 * will not be called.
 */

typedef void (*ifn_t)(int unit, uint32 data);

typedef struct {
    uint32    mask;
    ifn_t    intr_fn;
    uint32    intr_data;
    char    *intr_name;
} intr_handler_t;

static intr_handler_t soc_cmicm_intr_handlers[] = {
    { IRQ_CMCx_SCH_OP_DONE,    soc_cmicm_intr_schan_done,    0, "SCH_OP_DONE"   },
    { IRQ_CMCx_MIIM_OP_DONE,   soc_cmicm_intr_miim_op,       0, "MIIM_OP_DONE"   },
    { IRQ_CMCx_TDMA_DONE,      soc_cmicm_intr_tdma_done,     0, "TDMA_DONE"   },
    { IRQ_CMCx_TSLAM_DONE,     soc_cmicm_intr_tslam_done,    0, "TSLAM_DONE"   },
    { IRQ_CMCx_CCMDMA_DONE,    soc_cmicm_intr_ccmdma_done,   0, "CCMDMA_DONE"   },

    { IRQ_CMCx_CHAIN_DONE(0),  soc_dma_done_chain, 0, "CH0_CHAIN_DONE" },
    { IRQ_CMCx_CHAIN_DONE(1),  soc_dma_done_chain, 1, "CH1_CHAIN_DONE" },
    { IRQ_CMCx_CHAIN_DONE(2),  soc_dma_done_chain, 2, "CH2_CHAIN_DONE" },
    { IRQ_CMCx_CHAIN_DONE(3),  soc_dma_done_chain, 3, "CH3_CHAIN_DONE" },
    
    { IRQ_CMCx_DESC_DONE(0),   soc_dma_done_desc,  0, "CH0_DESC_DONE"  },
    { IRQ_CMCx_DESC_DONE(1),   soc_dma_done_desc,  1, "CH1_DESC_DONE"  },
    { IRQ_CMCx_DESC_DONE(2),   soc_dma_done_desc,  2, "CH2_DESC_DONE"  },
    { IRQ_CMCx_DESC_DONE(3),   soc_dma_done_desc,  3, "CH3_DESC_DONE"  },

    { IRQ_CMCx_CNTLD_DESC_DONE(0),   soc_dma_done_desc,  0, "CH0_CNTLD_DESC_DONE"  },
    { IRQ_CMCx_CNTLD_DESC_DONE(1),   soc_dma_done_desc,  1, "CH1_CNTLD_DESC_DONE"  },
    { IRQ_CMCx_CNTLD_DESC_DONE(2),   soc_dma_done_desc,  2, "CH2_CNTLD_DESC_DONE"  },
    { IRQ_CMCx_CNTLD_DESC_DONE(3),   soc_dma_done_desc,  3, "CH3_CNTLD_DESC_DONE"  },
    { IRQ_CMCx_STAT_ITER_DONE, soc_cmicm_intr_stat_dma,  0, "STAT_ITER_DONE" },

    { IRQ_CMCx_SW_INTR(CMICM_SW_INTR_UC0), soc_cmic_sw_intr, CMICM_SW_INTR_UC0, "UC0_SW_INTR" },
    { IRQ_CMCx_SW_INTR(CMICM_SW_INTR_UC1), soc_cmic_sw_intr, CMICM_SW_INTR_UC1, "UC1_SW_INTR" },

    { 0, NULL, 0, "" } /* Termination */
};

#ifdef SEPARATE_PKTDMA_INTR_HANDLER
static intr_handler_t soc_cmicm_pktdma_intr_handlers[] = {
    { IRQ_CMCx_CHAIN_DONE(0),  soc_dma_done_chain, 0, "CH0_CHAIN_DONE" },
    { IRQ_CMCx_CHAIN_DONE(1),  soc_dma_done_chain, 1, "CH1_CHAIN_DONE" },
    { IRQ_CMCx_CHAIN_DONE(2),  soc_dma_done_chain, 2, "CH2_CHAIN_DONE" },
    { IRQ_CMCx_CHAIN_DONE(3),  soc_dma_done_chain, 3, "CH3_CHAIN_DONE" },

    { IRQ_CMCx_DESC_DONE(0),   soc_dma_done_desc,  0, "CH0_DESC_DONE"  },
    { IRQ_CMCx_DESC_DONE(1),   soc_dma_done_desc,  1, "CH1_DESC_DONE"  },
    { IRQ_CMCx_DESC_DONE(2),   soc_dma_done_desc,  2, "CH2_DESC_DONE"  },
    { IRQ_CMCx_DESC_DONE(3),   soc_dma_done_desc,  3, "CH3_DESC_DONE"  },

    { IRQ_CMCx_CNTLD_DESC_DONE(0),   soc_dma_done_desc,  0, "CH0_CNTLD_DESC_DONE"  },
    { IRQ_CMCx_CNTLD_DESC_DONE(1),   soc_dma_done_desc,  1, "CH1_CNTLD_DESC_DONE"  },
    { IRQ_CMCx_CNTLD_DESC_DONE(2),   soc_dma_done_desc,  2, "CH2_CNTLD_DESC_DONE"  },
    { IRQ_CMCx_CNTLD_DESC_DONE(3),   soc_dma_done_desc,  3, "CH3_CNTLD_DESC_DONE"  },

    { 0, NULL, 0, "" } /* Termination */
};
#endif

static intr_handler_t soc_cmicm_intr_handlers0_fifo_dma[] = {
    { IRQ_CMCx_SCH_OP_DONE,    soc_cmicm_intr_schan_done,    0, "SCH_OP_DONE"   },
    { IRQ_CMCx_MIIM_OP_DONE,   soc_cmicm_intr_miim_op,       0, "MIIM_OP_DONE"   },
    { IRQ_CMCx_TDMA_DONE,      soc_cmicm_intr_tdma_done,     0, "TDMA_DONE"   },
    { IRQ_CMCx_TSLAM_DONE,     soc_cmicm_intr_tslam_done,    0, "TSLAM_DONE"   },
    { IRQ_CMCx_CCMDMA_DONE,    soc_cmicm_intr_ccmdma_done,   0, "CCMDMA_DONE"   },
    
    { IRQ_CMCx_CHAIN_DONE(0),  soc_dma_done_chain, 0, "CH0_CHAIN_DONE" },
    { IRQ_CMCx_CHAIN_DONE(1),  soc_dma_done_chain, 1, "CH1_CHAIN_DONE" },
    { IRQ_CMCx_CHAIN_DONE(2),  soc_dma_done_chain, 2, "CH2_CHAIN_DONE" },
    { IRQ_CMCx_CHAIN_DONE(3),  soc_dma_done_chain, 3, "CH3_CHAIN_DONE" },
    
    { IRQ_CMCx_DESC_DONE(0),   soc_dma_done_desc,  0, "CH0_DESC_DONE"  },
    { IRQ_CMCx_DESC_DONE(1),   soc_dma_done_desc,  1, "CH1_DESC_DONE"  },
    { IRQ_CMCx_DESC_DONE(2),   soc_dma_done_desc,  2, "CH2_DESC_DONE"  },
    { IRQ_CMCx_DESC_DONE(3),   soc_dma_done_desc,  3, "CH3_DESC_DONE"  },
    
    { IRQ_CMCx_STAT_ITER_DONE, soc_cmicm_intr_stat_dma,  0, "STAT_ITER_DONE" },
    
    { IRQ_CMCx_SW_INTR(CMICM_SW_INTR_UC0), soc_cmic_sw_intr, CMICM_SW_INTR_UC0, "UC0_SW_INTR" },
    { IRQ_CMCx_SW_INTR(CMICM_SW_INTR_UC1), soc_cmic_sw_intr, CMICM_SW_INTR_UC1, "UC1_SW_INTR" },
    { IRQ_CMCx_FIFO_CH_DMA(0), soc_cmicm_fifo_dma_done, 0, "CH0_FIFO_DMA_DONE" },
    { IRQ_CMCx_FIFO_CH_DMA(1), soc_cmicm_fifo_dma_done, 1, "CH1_FIFO_DMA_DONE" },
    { IRQ_CMCx_FIFO_CH_DMA(2), soc_cmicm_fifo_dma_done, 2, "CH2_FIFO_DMA_DONE" },
    { IRQ_CMCx_FIFO_CH_DMA(3), soc_cmicm_fifo_dma_done, 3, "CH3_FIFO_DMA_DONE" },

    { 0, NULL, 0, "" } /* Termination */
};


STATIC intr_handler_t soc_cmicm_intr_handlers0[] = {
    { IRQ_CMCx_SCH_OP_DONE,    soc_cmicm_intr_schan_done,    0, "SCH_OP_DONE"   },
    { IRQ_CMCx_MIIM_OP_DONE,   soc_cmicm_intr_miim_op,       0, "MIIM_OP_DONE"   },

    { IRQ_SBUSDMA_CH0_DONE,    soc_cmicm_intr_sbusdma_done,  0, "SBUS_DMA0_DONE"   },
    { IRQ_SBUSDMA_CH1_DONE,    soc_cmicm_intr_sbusdma_done,  1, "SBUS_DMA1_DONE"   },
    { IRQ_SBUSDMA_CH2_DONE,    soc_cmicm_intr_sbusdma_done,  2, "SBUS_DMA2_DONE"   },

    { IRQ_CMCx_CCMDMA_DONE,    soc_cmicm_intr_ccmdma_done,   0, "CCMDMA_DONE"   },

    { IRQ_CMCx_CHAIN_DONE(0),  soc_dma_done_chain, 0, "CH0_CHAIN_DONE" },
    { IRQ_CMCx_CHAIN_DONE(1),  soc_dma_done_chain, 1, "CH1_CHAIN_DONE" },
    { IRQ_CMCx_CHAIN_DONE(2),  soc_dma_done_chain, 2, "CH2_CHAIN_DONE" },
    { IRQ_CMCx_CHAIN_DONE(3),  soc_dma_done_chain, 3, "CH3_CHAIN_DONE" },
    
    { IRQ_CMCx_DESC_DONE(0),   soc_dma_done_desc,  0, "CH0_DESC_DONE"  },
    { IRQ_CMCx_DESC_DONE(1),   soc_dma_done_desc,  1, "CH1_DESC_DONE"  },
    { IRQ_CMCx_DESC_DONE(2),   soc_dma_done_desc,  2, "CH2_DESC_DONE"  },
    { IRQ_CMCx_DESC_DONE(3),   soc_dma_done_desc,  3, "CH3_DESC_DONE"  },

    { IRQ_CMCx_CNTLD_DESC_DONE(0),   soc_dma_done_desc,  0, "CH0_CNTLD_DESC_DONE"  },
    { IRQ_CMCx_CNTLD_DESC_DONE(1),   soc_dma_done_desc,  1, "CH1_CNTLD_DESC_DONE"  },
    { IRQ_CMCx_CNTLD_DESC_DONE(2),   soc_dma_done_desc,  2, "CH2_CNTLD_DESC_DONE"  },
    { IRQ_CMCx_CNTLD_DESC_DONE(3),   soc_dma_done_desc,  3, "CH3_CNTLD_DESC_DONE"  },

    { IRQ_CMCx_STAT_ITER_DONE, soc_cmicm_intr_stat_dma,  0, "STAT_ITER_DONE" },
    
    { IRQ_CMCx_SW_INTR(CMICM_SW_INTR_RCPU), soc_cmic_sw_intr, CMICM_SW_INTR_RCPU, "RCPU_SW_INTR" },
    { IRQ_CMCx_SW_INTR(CMICM_SW_INTR_UC0), soc_cmic_sw_intr, CMICM_SW_INTR_UC0, "UC0_SW_INTR" },
    { IRQ_CMCx_SW_INTR(CMICM_SW_INTR_UC1), soc_cmic_sw_intr, CMICM_SW_INTR_UC1, "UC1_SW_INTR" },

    { IRQ_CMCx_FIFO_CH_DMA(0), soc_cmicm_fifo_dma_done, 0, "CH0_FIFO_DMA_DONE" },
    { IRQ_CMCx_FIFO_CH_DMA(1), soc_cmicm_fifo_dma_done, 1, "CH1_FIFO_DMA_DONE" },
    { IRQ_CMCx_FIFO_CH_DMA(2), soc_cmicm_fifo_dma_done, 2, "CH2_FIFO_DMA_DONE" },
    { IRQ_CMCx_FIFO_CH_DMA(3), soc_cmicm_fifo_dma_done, 3, "CH3_FIFO_DMA_DONE" },
    
    { 0, NULL, 0, "" } /* Termination */
};

#ifdef SEPARATE_PKTDMA_INTR_HANDLER
STATIC intr_handler_t soc_cmicm_pktdma_intr_handlers0[] = {
    { IRQ_CMCx_CHAIN_DONE(0),  soc_dma_done_chain, 0, "CH0_CHAIN_DONE" },
    { IRQ_CMCx_CHAIN_DONE(1),  soc_dma_done_chain, 1, "CH1_CHAIN_DONE" },
    { IRQ_CMCx_CHAIN_DONE(2),  soc_dma_done_chain, 2, "CH2_CHAIN_DONE" },
    { IRQ_CMCx_CHAIN_DONE(3),  soc_dma_done_chain, 3, "CH3_CHAIN_DONE" },

    { IRQ_CMCx_DESC_DONE(0),   soc_dma_done_desc,  0, "CH0_DESC_DONE"  },
    { IRQ_CMCx_DESC_DONE(1),   soc_dma_done_desc,  1, "CH1_DESC_DONE"  },
    { IRQ_CMCx_DESC_DONE(2),   soc_dma_done_desc,  2, "CH2_DESC_DONE"  },
    { IRQ_CMCx_DESC_DONE(3),   soc_dma_done_desc,  3, "CH3_DESC_DONE"  },

    { IRQ_CMCx_CNTLD_DESC_DONE(0),   soc_dma_done_desc,  0, "CH0_CNTLD_DESC_DONE"  },
    { IRQ_CMCx_CNTLD_DESC_DONE(1),   soc_dma_done_desc,  1, "CH1_CNTLD_DESC_DONE"  },
    { IRQ_CMCx_CNTLD_DESC_DONE(2),   soc_dma_done_desc,  2, "CH2_CNTLD_DESC_DONE"  },
    { IRQ_CMCx_CNTLD_DESC_DONE(3),   soc_dma_done_desc,  3, "CH3_CNTLD_DESC_DONE"  },

    { 0, NULL, 0, "" } /* Termination */
};
#endif

#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
STATIC intr_handler_t soc_cmicm_intr_handlers1[] = {
    { IRQ_CMCx_COMMON_SCHAN_DONE, soc_cmicm_intr_common_schan_done, 0, "COM_SCH_DONE" },
    { IRQ_CMCx_CHIP_FUNC_INTR, soc_cmicm_chip_func_intr, 0, "CHIP FUNC INTR" },
    { IRQ_CMCx_LINK_STAT_MOD, soc_cmicm_link_stat, 0, "PHY_LINKSCAN_LINKSTATUS_CHD"},
    { IRQ_CMCx_LINK_STAT_MOD, soc_cmicm_intr_link_stat, 0, "LINK_STAT_MOD" },
    { IRQ_CMCx_SER_INTR, soc_ser_engine_intr, 0, "SER ENGINE INTR" },
    { IRQ_CMCx_TIMESYNC_INTR, soc_cmicm_timesync_intr, 0, "TIMESYNC INTR" },
    { IRQ_CMCx_PCIEINTF_NEEDS_CLEANUP, soc_cmicm_pcie_link_down, 0, "PCIE_LINK_DOWN INTR" },
    { 0, NULL, 0, "" } /* Termination */
};

STATIC intr_handler_t soc_cmicm_intr_handlers2[] = {
    { IRQ_CMCx_PARITY, soc_cmicm_parity_intr, 0, "PARITY INTR" },
    { 0, NULL, 0, "" } /* Termination */
};

STATIC intr_handler_t soc_cmicm_intr_handlers3[] = {
    { IRQ_CMCx_BLOCK(1),  soc_cmicm_block_lo_intr,  1, "L2_MNGT INTR" },    
    { IRQ_CMCx_BLOCK(2),  soc_cmicm_block_lo_intr,  2, "BLOCK 4 INTR" },
    { IRQ_CMCx_BLOCK(4),  soc_cmicm_block_lo_intr,  4, "BLOCK 4 INTR" },
    { IRQ_CMCx_BLOCK(5),  soc_cmicm_block_lo_intr,  5, "BLOCK 4 INTR" },
    { IRQ_CMCx_BLOCK(6),  soc_cmicm_block_lo_intr,  6, "BLOCK 6 INTR" },
    { IRQ_CMCx_BLOCK(7),  soc_cmicm_block_lo_intr,  7, "BLOCK 4 INTR" },
    { IRQ_CMCx_BLOCK(8),  soc_cmicm_block_lo_intr,  8, "BLOCK 4 INTR" },
    { IRQ_CMCx_BLOCK(9),  soc_cmicm_block_lo_intr,  9, "BLOCK 4 INTR" },
    { IRQ_CMCx_BLOCK(10), soc_cmicm_block_lo_intr,  10, "BLOCK 4 INTR" },
    { IRQ_CMCx_BLOCK(11), soc_cmicm_block_lo_intr,  11, "BLOCK 4 INTR" },
    { IRQ_CMCx_BLOCK(12), soc_cmicm_block_lo_intr,  12, "BLOCK 4 INTR" },
    { IRQ_CMCx_BLOCK(13), soc_cmicm_block_lo_intr,  13, "BLOCK 4 INTR" },
    { IRQ_CMCx_BLOCK(14), soc_cmicm_block_lo_intr,  14, "BLOCK 4 INTR" },
    { IRQ_CMCx_BLOCK(15), soc_cmicm_block_lo_intr,  15, "BLOCK 4 INTR" },
    { IRQ_CMCx_BLOCK(16), soc_cmicm_block_lo_intr,  16, "BLOCK 4 INTR" },
    { IRQ_CMCx_BLOCK(25), soc_cmicm_block_lo_intr, 25, "BLOCK 25 INTR" },
    { IRQ_CMCx_BLOCK(26), soc_cmicm_block_lo_intr, 26, "BLOCK 26 INTR" },
    { IRQ_CMCx_BLOCK(27), soc_cmicm_block_lo_intr, 27, "BLOCK 27 INTR" },
    { 0, NULL, 0, "" } /* Termination */
};

STATIC intr_handler_t soc_cmicm_intr_handlers4[] = {
    { 0, NULL, 0, "" } /* Termination */
};

STATIC intr_handler_t soc_cmicm_intr_handlers5[] = {
    { 0, NULL, 0, "" } /* Termination */
};

STATIC intr_handler_t soc_cmicdv2_intr_handlers2[] = {
    { IRQ_CMCx_BLOCK(19), soc_dma_pci_timeout_handle, 19, "PCI_TIMEOUT_INTR" },
    { IRQ_CMCx_BLOCK(24),  soc_cmicdv2_chip_parity_intr,  24, "NS INTR" },
    { IRQ_CMCx_BLOCK(25),  soc_cmicdv2_chip_parity_intr,  25, "NS DEBUG INTR" },
    { 0, NULL, 0, "" } /* Termination */
};

STATIC intr_handler_t soc_cmicdv2_intr_handlers3[] = {
    { IRQ_CMCx_BLOCK(1),  soc_cmicdv2_parity_intr,  1, "L2_MNGT INTR" },
    { IRQ_CMCx_BLOCK(2),  soc_cmicdv2_parity_intr,  2, "BLOCK 2 INTR" },
    { IRQ_CMCx_BLOCK(3),  soc_cmicdv2_parity_intr,  3, "BLOCK 3 INTR" },
    { IRQ_CMCx_BLOCK(4),  soc_cmicdv2_parity_intr,  4, "BLOCK 4 INTR" },
    { IRQ_CMCx_BLOCK(5),  soc_cmicdv2_parity_intr,  5, "BLOCK 5 INTR" },
    { IRQ_CMCx_BLOCK(6),  soc_cmicdv2_parity_intr,  6, "BLOCK 6 INTR" },
    { IRQ_CMCx_BLOCK(7),  soc_cmicdv2_parity_intr,  7, "BLOCK 7 INTR" },
    { IRQ_CMCx_BLOCK(8),  soc_cmicdv2_parity_intr,  8, "BLOCK 8 INTR" },
    { IRQ_CMCx_BLOCK(9),  soc_cmicdv2_parity_intr,  9, "BLOCK 9 INTR" },
    { IRQ_CMCx_BLOCK(10), soc_cmicdv2_parity_intr,  10, "BLOCK 10 INTR" },
    { IRQ_CMCx_BLOCK(11), soc_cmicdv2_parity_intr,  11, "BLOCK 11 INTR" },
    { IRQ_CMCx_BLOCK(12), soc_cmicdv2_parity_intr,  12, "BLOCK 12 INTR" },
    { IRQ_CMCx_BLOCK(13), soc_cmicdv2_parity_intr,  13, "BLOCK 13 INTR" },
    { IRQ_CMCx_BLOCK(14), soc_cmicdv2_parity_intr,  14, "BLOCK 14 INTR" },
    { IRQ_CMCx_BLOCK(15), soc_cmicdv2_parity_intr,  15, "BLOCK 15 INTR" },
    { IRQ_CMCx_BLOCK(16), soc_cmicdv2_parity_intr,  16, "BLOCK 16 INTR" },
    { IRQ_CMCx_BLOCK(26), soc_cmicdv2_parity_intr, 26, "BLOCK 26 INTR" },
    { IRQ_CMCx_BLOCK(27), soc_cmicdv2_parity_intr, 27, "BLOCK 27 INTR" },
    { IRQ_CMCx_BLOCK(28), soc_cmicdv2_parity_intr,  28, "BLOCK 28 INTR" },
    { IRQ_CMCx_BLOCK(29), soc_cmicdv2_parity_intr,  29, "BLOCK 29 INTR" },
    { IRQ_CMCx_PARITY, soc_cmicdv2_parity_intr, 0, "PARITY INTR" },
    { 0, NULL, 0, "" } /* Termination */
};

STATIC intr_handler_t soc_cmicdv2_intr_handlers4[] = {
    { IRQ_CMCx_BLOCK(0), soc_cmicdv2_block_intr4, 0, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(1), soc_cmicdv2_block_intr4, 1, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(2), soc_cmicdv2_block_intr4, 2, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(3), soc_cmicdv2_block_intr4, 3, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(4), soc_cmicdv2_block_intr4, 4, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(5), soc_cmicdv2_block_intr4, 5, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(6), soc_cmicdv2_block_intr4, 6, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(7), soc_cmicdv2_block_intr4, 7, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(8), soc_cmicdv2_block_intr4, 8, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(9), soc_cmicdv2_block_intr4, 9, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(10), soc_cmicdv2_block_intr4, 10, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(11), soc_cmicdv2_block_intr4, 11, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(12), soc_cmicdv2_block_intr4, 12, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(13), soc_cmicdv2_block_intr4, 13, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(14), soc_cmicdv2_block_intr4, 14, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(15), soc_cmicdv2_block_intr4, 15, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(16), soc_cmicdv2_block_intr4, 16, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(17), soc_cmicdv2_block_intr4, 17, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(18), soc_cmicdv2_block_intr4, 18, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(19), soc_cmicdv2_block_intr4, 19, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(20), soc_cmicdv2_block_intr4, 20, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(21), soc_cmicdv2_block_intr4, 21, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(22), soc_cmicdv2_block_intr4, 22, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(23), soc_cmicdv2_block_intr4, 23, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(24), soc_cmicdv2_block_intr4, 24, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(25), soc_cmicdv2_block_intr4, 25, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(26), soc_cmicdv2_block_intr4, 26, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(27), soc_cmicdv2_block_intr4, 27, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(28), soc_cmicdv2_block_intr4, 28, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(29), soc_cmicdv2_block_intr4, 29, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(30), soc_cmicdv2_block_intr4, 30, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(31), soc_cmicdv2_block_intr4, 31, "PARITY INTR" },
    { 0, NULL, 0, "" } /* Termination */

};

STATIC intr_handler_t soc_cmicdv2_intr_handlers5[] = {
    { IRQ_CMCx_PARITY, soc_cmicdv2_block_intr5, 0, "PARITY INTR" },
    { 0, NULL, 0, "" } /* Termination */
};

STATIC intr_handler_t soc_cmicdv4_intr_handlers6[] = {
    { IRQ_CMCx_BLOCK(0), soc_cmicdv4_block_intr6, 0, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(1), soc_cmicdv4_block_intr6, 1, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(2), soc_cmicdv4_block_intr6, 2, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(3), soc_cmicdv4_block_intr6, 3, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(4), soc_cmicdv4_block_intr6, 4, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(5), soc_cmicdv4_block_intr6, 5, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(6), soc_cmicdv4_block_intr6, 6, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(7), soc_cmicdv4_block_intr6, 7, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(8), soc_cmicdv4_block_intr6, 8, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(9), soc_cmicdv4_block_intr6, 9, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(10), soc_cmicdv4_block_intr6, 10, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(11), soc_cmicdv4_block_intr6, 11, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(12), soc_cmicdv4_block_intr6, 12, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(13), soc_cmicdv4_block_intr6, 13, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(14), soc_cmicdv4_block_intr6, 14, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(15), soc_cmicdv4_block_intr6, 15, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(16), soc_cmicdv4_block_intr6, 16, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(17), soc_cmicdv4_block_intr6, 17, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(18), soc_cmicdv4_block_intr6, 18, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(19), soc_cmicdv4_block_intr6, 19, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(20), soc_cmicdv4_block_intr6, 20, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(21), soc_cmicdv4_block_intr6, 21, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(22), soc_cmicdv4_block_intr6, 22, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(23), soc_cmicdv4_block_intr6, 23, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(24), soc_cmicdv4_block_intr6, 24, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(25), soc_cmicdv4_block_intr6, 25, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(26), soc_cmicdv4_block_intr6, 26, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(27), soc_cmicdv4_block_intr6, 27, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(28), soc_cmicdv4_block_intr6, 28, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(29), soc_cmicdv4_block_intr6, 29, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(30), soc_cmicdv4_block_intr6, 30, "PARITY INTR" },
    { IRQ_CMCx_BLOCK(31), soc_cmicdv4_block_intr6, 31, "PARITY INTR" },
    { 0, NULL, 0, "" } /* Termination */
};

/* IRQ0 handler for CMC of SOC_ARM_CMC(unit, 0)
 *  For now only support TSLAM (for Caladan3 TMU)
 *  and FIFO DMA 0/1 (for Caladan3 COP0/1)
 */
STATIC intr_handler_t soc_cmicm_intr_handlers0_arm_cmc[] = {
    { IRQ_SBUSDMA_CH0_DONE,    soc_cmicm_intr_sbusdma_done,  0, "SBUS_DMA0_DONE"   },
    { IRQ_SBUSDMA_CH1_DONE,    soc_cmicm_intr_sbusdma_done,  1, "SBUS_DMA1_DONE"   },
    { IRQ_SBUSDMA_CH2_DONE,    soc_cmicm_intr_sbusdma_done,  2, "SBUS_DMA2_DONE"   },

    { IRQ_CMCx_FIFO_CH_DMA(0), soc_cmicm_fifo_dma_done, 0, "CH0_FIFO_DMA_DONE" },
    { IRQ_CMCx_FIFO_CH_DMA(1), soc_cmicm_fifo_dma_done, 1, "CH1_FIFO_DMA_DONE" },
    { IRQ_CMCx_FIFO_CH_DMA(2), soc_cmicm_fifo_dma_done, 2, "CH2_FIFO_DMA_DONE" },
    { IRQ_CMCx_FIFO_CH_DMA(3), soc_cmicm_fifo_dma_done, 3, "CH3_FIFO_DMA_DONE" },
    
    { 0, NULL, 0, "" } /* Termination */
};
#endif

#ifdef INCLUDE_RCPU
STATIC intr_handler_t soc_cmicm_rcpu_intr_handlers0[] = {
    { IRQ_RCPU_MIIM_OP_DONE, soc_cmicm_rcpu_intr_miim_op, 0, "RCPU_MIIM_OP_DONE" },
    { 0, NULL, 0, "" } /* Termination */
};
#endif /* INCLUDE_RCPU */
/*
 * Interrupt handler functions
 */

STATIC void
soc_cmicm_intr_schan_done(int unit, uint32 vchan)
{
    soc_control_t    *soc = SOC_CONTROL(unit);
    int cmc = vchan / N_DMA_CHAN;

    /* Record the schan control regsiter */
    soc->schan_result[cmc] = soc_pci_read(unit, CMIC_CMCx_SCHAN_CTRL_OFFSET(cmc));
    soc_pci_write(unit, CMIC_CMCx_SCHAN_CTRL_OFFSET(cmc),
        soc_pci_read(unit, CMIC_CMCx_SCHAN_CTRL_OFFSET(cmc)) & ~SC_CMCx_MSG_DONE);

    soc->stat.intr_sc++;

    if (soc->schanIntr[cmc]) {
        sal_sem_give(soc->schanIntr[cmc]);
    }
}

#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
STATIC void
soc_cmicm_intr_common_schan_done(int unit, uint32 ignored)
{
    soc_control_t    *soc = SOC_CONTROL(unit);
    int cmc = CMIC_CMC_NUM_MAX;

    COMPILER_REFERENCE(ignored);

    /* Record the schan control regsiter */
    soc->schan_result[cmc] = soc_pci_read(unit, CMIC_COMMON_SCHAN_CTRL_OFFSET);
    soc_pci_write(unit, CMIC_COMMON_SCHAN_CTRL_OFFSET,
        soc_pci_read(unit, CMIC_COMMON_SCHAN_CTRL_OFFSET) & ~SC_CMCx_MSG_DONE);

    soc->stat.intr_sc++;

    if (soc->schanIntr[cmc]) {
        sal_sem_give(soc->schanIntr[cmc]);
    }
}
#endif /* defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)  || defined(BCM_DFE_SUPPORT)*/

STATIC void
soc_cmicm_intr_miim_op(int unit, uint32 ignored)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    int cmc = SOC_PCI_CMC(unit);

    COMPILER_REFERENCE(ignored);

    soc_pci_write(unit, CMIC_CMCx_MIIM_CTRL_OFFSET(cmc), 0); /* Clr Read & Write Stat */

    soc->stat.intr_mii++;

    if (soc->miimIntr) {
        sal_sem_give(soc->miimIntr);
    }
}

STATIC int
_sbusdma_cmc_ch_op_proc(int unit, uint32 op, int cmc, int ch)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    int rv = SOC_E_NONE;

    if ((cmc >= SOC_CMCS_NUM_MAX) ||
          (ch < 0) || (ch >=  SOC_SBUSDMA_CH_PER_CMC)) {
          return SOC_E_PARAM;
    }
    LOG_VERBOSE(BSL_LS_SOC_INTR,
                 (BSL_META_U(unit,
                  "op = %u, cmc = %d, ch = %d\n"), op, cmc, ch));
    switch (op) {
        case SOC_SBUSDMA_TYPE_TDMA:
        soc->stat.intr_tdma++;
        if (soc->tableDmaIntrEnb) {
             sal_sem_give(soc->sbusDmaIntrs[cmc][ch]);
        }
        break;
        case SOC_SBUSDMA_TYPE_SLAM:
        soc->stat.intr_tslam++;
        if (soc->tslamDmaIntrEnb) {
            sal_sem_give(soc->sbusDmaIntrs[cmc][ch]);
        }
        break;
#ifdef BCM_SBUSDMA_SUPPORT
        case SOC_SBUSDMA_TYPE_DESC:
        soc->stat.intr_desc++;
        if (SOC_SBUSDMA_DM_INTRENB(unit)) {
            sal_sem_give(soc->sbusDmaIntrs[cmc][ch]);
        }
        break;
#endif
        default:
        LOG_INFO(BSL_LS_SOC_INTR,
                 (BSL_META_U(unit,
                  "Received unallocated sbusdma interrupt !!\n")));
        rv = SOC_E_PARAM;
        break;
    }
    return rv;
}

STATIC void
soc_cmicm_intr_tdma_done(int unit, uint32 ignored)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    int cmc = SOC_PCI_CMC(unit);
    int ch = soc->tdma_ch;

    COMPILER_REFERENCE(ignored);

    (void)soc_cmicm_intr0_disable(unit, IRQ_CMCx_TDMA_DONE);

    _sbusdma_cmc_ch_op_proc(unit, SOC_SBUSDMA_TYPE_TDMA, cmc, ch);
}

STATIC void
soc_cmicm_intr_tslam_done(int unit, uint32 ignored)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    int cmc = SOC_PCI_CMC(unit);
    int ch = soc->tslam_ch;

    COMPILER_REFERENCE(ignored);

    (void)soc_cmicm_intr0_disable(unit, IRQ_CMCx_TSLAM_DONE);

    _sbusdma_cmc_ch_op_proc(unit, SOC_SBUSDMA_TYPE_SLAM, cmc, ch);
}

STATIC void
soc_cmicm_intr_stat_dma(int unit, uint32 ignored)
{
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT) 

    soc_control_t *soc = SOC_CONTROL(unit);
    int cmc = SOC_PCI_CMC(unit);

    COMPILER_REFERENCE(ignored);
    if (SOC_IS_SAND(unit)) {
        return;
    }

    soc_pci_write(unit, CMIC_CMCx_STAT_DMA_CFG_OFFSET(cmc),
        soc_pci_read(unit, CMIC_CMCx_STAT_DMA_CFG_OFFSET(cmc)) | STDMA_ITER_DONE_CLR);

    soc->stat.intr_stats++;

    if (soc->counter_intr) {
        sal_sem_give(soc->counter_intr);
    }
#endif    
}

STATIC void
soc_cmicm_intr_ccmdma_done(int unit, uint32 vchan)
{
    int cmc;
    soc_control_t *soc = SOC_CONTROL(unit);

    if (soc_feature(unit, soc_feature_cmicm_multi_dma_cmc)) {
        cmc = vchan / N_DMA_CHAN;
    } else {
        cmc = SOC_PCI_CMC(unit);
    }

    (void)soc_cmicm_cmcx_intr0_disable(unit, cmc, IRQ_CMCx_CCMDMA_DONE);

    soc->stat.intr_ccmdma++;

    if (soc->ccmDmaIntr[cmc]) {
        sal_sem_give(soc->ccmDmaIntr[cmc]);
    }
}

#ifdef BCM_SBUSDMA_SUPPORT
STATIC uint32 _soc_irq_cmic_sbusdma_ch[] = {
    IRQ_SBUSDMA_CH0_DONE, 
    IRQ_SBUSDMA_CH1_DONE,
    IRQ_SBUSDMA_CH2_DONE
};

STATIC int
_sbusdma_cmc_ch_op_get(int unit, int ch, uint32 *op)
{
     soc_control_t *soc = SOC_CONTROL(unit);

     if (ch == soc->tdma_ch) {
         *op = SOC_SBUSDMA_TYPE_TDMA;
     } else if (ch == soc->tslam_ch) {
         *op = SOC_SBUSDMA_TYPE_SLAM;
     } else if (ch == soc->desc_ch) {
         *op = SOC_SBUSDMA_TYPE_DESC;
     } else {
         return SOC_E_PARAM;
     }

     return SOC_E_NONE;
}

#endif

STATIC void
soc_cmicm_intr_sbusdma_done(int unit, uint32 vchan)
{
#ifdef BCM_SBUSDMA_SUPPORT
    uint32 op;
    int cmc;
    int ch;
    uint32 sbusdma_chan_max  = CMIC_CMCx_SBUSDMA_CHAN_MAX;

#ifdef BCM_TRIDENT3_SUPPORT
    uint32 ifa_cc_num_sbusdma = soc_property_get(unit,
                                                 spn_IFA_CC_NUM_SBUS_DMA, 2);
    if ((SOC_IS_TRIDENT3(unit)) && (ifa_cc_num_sbusdma > 2)) {
        /* CMC_0 2 SBUS DMA Channels are used by IFA CC Eapp on M0 */
        sbusdma_chan_max = (CMIC_CMCx_SBUSDMA_CHAN_MAX - 2);
    }
#endif

    if (soc_feature(unit, soc_feature_cmicm_multi_dma_cmc)) {
        /* vchan  = cmc*N_DMA_CHAN + SBUS_CHANNEL (0 - CMIC_CMCx_SBUSDMA_CHAN_MAX-1) */
        cmc = vchan / N_DMA_CHAN;
        ch  = vchan % N_DMA_CHAN;

        if (ch > sbusdma_chan_max - 1) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                             (BSL_META_U(unit,
                                         "ERROR: sbusdma channel %d !!\n"), ch));
            return;
        }

        (void)soc_cmicm_cmcx_intr0_disable(unit, cmc, _soc_irq_cmic_sbusdma_ch[ch]);

        {
            if (SOC_FAILURE(_sbusdma_cmc_ch_op_get(unit, ch, &op))) {
                LOG_INFO(BSL_LS_SOC_INTR,
                             (BSL_META_U(unit,
                               "Received unallocated sbusdma interrupt cmc %d ch %d !!\n"),
                              cmc, ch));
            } else {
                (void)_sbusdma_cmc_ch_op_proc(unit, op, cmc, ch);

            }
        }
    } else {
        cmc = SOC_PCI_CMC(unit);
        ch = vchan;

        (void)soc_cmicm_cmcx_intr0_disable(unit, cmc, _soc_irq_cmic_sbusdma_ch[ch]);

        if (SOC_FAILURE(_sbusdma_cmc_ch_op_get(unit, ch, &op))) {
                LOG_INFO(BSL_LS_SOC_INTR,
                             (BSL_META_U(unit,
                               "Received unallocated sbusdma interrupt cmc %d ch %d !!\n"),
                              cmc, ch));
        } else {
            (void)_sbusdma_cmc_ch_op_proc(unit, op, cmc, ch);
        }
    }
#else
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(vchan);
#endif
}

STATIC void
soc_cmicm_fifo_dma_done(int unit, uint32 vchan)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    int cmc, ch;
    int oam_status_channel = -1;
    int oam_event_channel = -1;
    int olp_channel = -1;

    if (soc_feature(unit, soc_feature_cmicm_multi_dma_cmc)) {
        cmc = vchan / N_DMA_CHAN;
        ch  = vchan % N_DMA_CHAN;
    } else {
        cmc = SOC_PCI_CMC(unit);
        ch = vchan;
    }
    /* get the channel configure for each source type */
#ifdef BCM_PETRA_SUPPORT    
    if(SOC_IS_JERICHO(unit))
    {
        jer_mgmt_dma_fifo_channel_get(unit, dma_fifo_channel_src_oam_status, &oam_status_channel);
        jer_mgmt_dma_fifo_channel_get(unit, dma_fifo_channel_src_oam_event, &oam_event_channel);
        jer_mgmt_dma_fifo_channel_get(unit, dma_fifo_channel_src_olp, &olp_channel);      
    }
#endif
    if (SOC_IS_JERICHO(unit) && (vchan == oam_status_channel))  {
#ifdef BCM_PETRA_SUPPORT
        uint8 oam_is_init=0;
        uint8 bfd_is_init=0;
        uint32 rv;

        rv = sw_state_access[unit].dpp.soc.arad.pp.oper_mode.oam_enable.get(unit, &oam_is_init); 
        rv = sw_state_access[unit].dpp.soc.arad.pp.oper_mode.bfd_enable.get(unit, &bfd_is_init);
        /* Return value ignored: even if the above function failed we must clear the interrupt.*/
        (void) rv;

        if  (oam_is_init || bfd_is_init) {
            (void)soc_cmicm_cmcx_intr0_disable(unit, cmc, IRQ_CMCx_FIFO_CH_DMA(ch));
            if (jer_oam_stat_fifo_sem[unit]) {
                SOC_CONTROL(unit)->stat.intr_fifo_dma[ch]++;
                sal_sem_give(jer_oam_stat_fifo_sem[unit]);
            }
        }
#endif
    } else if (SOC_IS_JERICHO(unit) && (vchan == oam_event_channel))  {
#ifdef BCM_PETRA_SUPPORT
        uint8 oam_is_init=0;
        uint8 bfd_is_init=0;
        uint32 rv;

        rv = sw_state_access[unit].dpp.soc.arad.pp.oper_mode.oam_enable.get(unit, &oam_is_init); 
        rv = sw_state_access[unit].dpp.soc.arad.pp.oper_mode.bfd_enable.get(unit, &bfd_is_init);
        /* Return value ignored: even if the above function failed we must clear the interrupt.*/
        (void) rv;

        if  (oam_is_init || bfd_is_init) {
            (void)soc_cmicm_cmcx_intr0_disable(unit, cmc, IRQ_CMCx_FIFO_CH_DMA(ch));
            if (jer_oam_event_fifo_sem[unit]) {
                SOC_CONTROL(unit)->stat.intr_fifo_dma[ch]++;
                sal_sem_give(jer_oam_event_fifo_sem[unit]);
            }
        }
#endif
        } else if (SOC_IS_ARADPLUS(unit) && !SOC_IS_JERICHO(unit) && (ch == SOC_MEM_FIFO_DMA_CHANNEL_3))  {
#ifdef BCM_PETRA_SUPPORT
            uint8 oam_is_init=0;
            uint8 bfd_is_init=0;
            uint32 rv;
    
            rv = sw_state_access[unit].dpp.soc.arad.pp.oper_mode.oam_enable.get(unit, &oam_is_init); 
            rv = sw_state_access[unit].dpp.soc.arad.pp.oper_mode.bfd_enable.get(unit, &bfd_is_init);
            /* Return value ignored: even if the above function failed we must clear the interrupt.*/
            (void) rv;
    
            if  (oam_is_init || bfd_is_init) {
                (void)soc_cmicm_intr0_disable(unit, IRQ_CMCx_FIFO_CH_DMA(ch));
                rv = sal_dpc(soc_ppd_oam_dma_event_handler, INT_TO_PTR(unit), INT_TO_PTR(SOC_PPC_OAM_DMA_EVENT_TYPE_EVENT), INT_TO_PTR(cmc), INT_TO_PTR(ch), 0);
                if (rv)
                {
                    LOG_INFO(BSL_LS_SOC_INTR,
                            (BSL_META_U(unit,
                                    "sal_dpc failed to queue soc_ppd_oam_dma_event_handler !!\n")));

                    /* sal_dpc failed. Enable the interrupt */
                    soc_cmicm_cmcx_intr0_enable(unit, cmc, IRQ_CMCx_FIFO_CH_DMA(ch));
                }
            }
#endif

    } else if (SOC_IS_JERICHO(unit) && (vchan == olp_channel))  {
#ifdef BCM_PETRA_SUPPORT

        /* Enable is done in the end of the handler */
        (void)soc_cmicm_cmcx_intr0_disable(unit, cmc, IRQ_CMCx_FIFO_CH_DMA(ch));
        if (jer_mact_fifo_sem[unit])
            sal_sem_give(jer_mact_fifo_sem[unit]);
#endif
    }
    else if (SOC_IS_DFE(unit))
    {
#ifdef BCM_DFE_SUPPORT
        if (SOC_DFE_CONTROL(unit)->rx_thread_fifo_dma_semaphore != NULL)
        {
            (void)soc_cmicm_intr0_disable(unit, IRQ_CMCx_FIFO_CH_DMA(ch));
            sal_sem_give(SOC_DFE_CONTROL(unit)->rx_thread_fifo_dma_semaphore);
        }
#endif
    } else {
        (void)soc_cmicm_cmcx_intr0_disable(unit, cmc, IRQ_CMCx_FIFO_CH_DMA(ch));

#ifdef BCM_CMICM_SUPPORT
        if (soc->fifoDmaMutexs[cmc][ch]) {
            SOC_CONTROL(unit)->stat.intr_fifo_dma[ch]++;
            sal_sem_give(soc->fifoDmaIntrs[cmc][ch]);
        }
#endif
        switch (ch) {
            case SOC_MEM_FIFO_DMA_CHANNEL_0:
#ifdef BCM_TOMAHAWK_SUPPORT
                if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {

                    if (soc->l2modDmaIntrEnb) {
                        SOC_CONTROL(unit)->stat.intr_fifo_dma[ch]++;
                        sal_sem_give(soc->arl_notify);
                    }
                    break;
                }
#endif /* BCM_TOMAHAWK_SUPPORT */
                if (SOC_CONTROL(unit)->ftreportIntrEnb) {
                    SOC_CONTROL(unit)->stat.intr_fifo_dma[ch]++;
                    sal_sem_give(SOC_CONTROL(unit)->ftreportIntr);
                } else if (SOC_IS_TD2_TT2(unit) && soc->l2modDmaIntrEnb) {
                    /* L2 fifo dma ch = SOC_MEM_FIFO_DMA_CHANNEL_0; */
                    SOC_CONTROL(unit)->stat.intr_fifo_dma[ch]++;
                    sal_sem_give(soc->arl_notify);
                }
                break;
            case SOC_MEM_FIFO_DMA_CHANNEL_1:
#ifdef BCM_TOMAHAWK_SUPPORT
                if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
                    if (soc->ctrEvictDmaIntrEnb) {
                        SOC_CONTROL(unit)->stat.intr_fifo_dma[ch]++;
                        sal_sem_give(soc->ctrEvictIntr);
                    }
                    break;
                } else
#endif /* BCM_TOMAHAWK_SUPPORT */
                if (soc->l2modDmaIntrEnb) {
                    SOC_CONTROL(unit)->stat.intr_fifo_dma[ch]++;
                    sal_sem_give(soc->arl_notify);
                }
                break;
            case SOC_MEM_FIFO_DMA_CHANNEL_2:
                if (SOC_CONTROL(unit)->ipfixIntrEnb) {
                    SOC_CONTROL(unit)->stat.intr_fifo_dma[ch]++;
                    sal_sem_give(SOC_CONTROL(unit)->ipfixIntr);
                }
                break;
            case SOC_MEM_FIFO_DMA_CHANNEL_3:
                if (SOC_CONTROL(unit)->ipfixIntrEnb) {
                    SOC_CONTROL(unit)->stat.intr_fifo_dma[ch]++;
                    sal_sem_give(SOC_CONTROL(unit)->ipfixIntr);
                }
                break;
            default:
                LOG_INFO(BSL_LS_SOC_INTR,
                         (BSL_META_U(unit,
                                     "Received unallocated fifo dma interrupt !!\n")));
        }
    }
}

#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
STATIC void
soc_cmicm_chip_func_intr(int unit, uint32 val)
{
    int cmc = SOC_PCI_CMC(unit);
    uint32 irqStat;
#if defined (BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT)
    uint32 irqMask, oldmask;

    oldmask = 0;

    irqMask = SOC_CMCx_IRQ1_MASK(unit,cmc);
#endif

    irqStat = soc_pci_read(unit, CMIC_CMCx_IRQ_STAT1_OFFSET(cmc));

#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TRIDENT2(unit) || SOC_IS_TRIDENT2PLUS(unit)) {
        uint32 rv;
        if (irqStat & ~_SOC_TD2_FUNC_INTR_MASK) {
            (void)soc_cmicm_intr1_disable(unit, irqStat &
                                          ~_SOC_TD2_FUNC_INTR_MASK);
        }

        if (irqStat & _SOC_TD2_FUNC_INTR_MASK) {
            oldmask = soc_cmicm_intr1_disable(unit, irqMask);

            /* dispatch interrupt */
            LOG_INFO(BSL_LS_SOC_INTR,
                     (BSL_META_U(unit,
                                 "soc_cmicm_intr type 1 unit %d: dispatch\n"),
                      unit));

            rv = sal_dpc(soc_td2_process_func_intr, INT_TO_PTR(unit),
                    INT_TO_PTR(oldmask), 0, 0, 0);
            if (rv) {
                LOG_ERROR(BSL_LS_SOC_INTR, (BSL_META_U(unit,
                          "sal_dpc failed to queue event handler !!\n")));
                /* sal_dpc failed. Enable the interrupt */
                soc_cmicm_intr1_enable(unit, irqMask);
            }
        }
    } else
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        uint32 rv;
        if (soc_feature(unit, soc_feature_esm_correction)) {
            if (irqStat & ~(_SOC_TR3_FUNC_PARITY_INTR_MASK |
                            _SOC_TR3_ESM_INTR_MASK)) {
                (void)soc_cmicm_intr1_disable(unit, irqStat &
                                              ~(_SOC_TR3_FUNC_PARITY_INTR_MASK |
                                                _SOC_TR3_ESM_INTR_MASK));
            }

            if (irqStat & (_SOC_TR3_FUNC_PARITY_INTR_MASK |
                            _SOC_TR3_ESM_INTR_MASK)) {
                oldmask = soc_cmicm_intr1_disable(unit, irqMask);
                /* dispatch interrupt */
                LOG_INFO(BSL_LS_SOC_INTR,
                         (BSL_META_U(unit,
                                     "soc_cmicm_intr type 1 unit %d: dispatch\n"),
                          unit));

                if (irqStat & _SOC_TR3_ESM_INTR_MASK) {
                    sal_sem_give(SOC_CONTROL(unit)->esm_recovery_notify);
                }
                rv = sal_dpc(soc_tr3_process_func_intr, INT_TO_PTR(unit),
                        INT_TO_PTR(oldmask), 0, 0, 0);
                if (rv) {
                    LOG_ERROR(BSL_LS_SOC_INTR, (BSL_META_U(unit,
                             "sal_dpc failed to queue event handler !!\n")));
                    /* sal_dpc failed. Enable the interrupt */
                    soc_cmicm_intr1_enable(unit, irqMask);
                }
            }
        } else {
            if (irqStat & ~_SOC_TR3_FUNC_PARITY_INTR_MASK) {
                (void)soc_cmicm_intr1_disable(unit, irqStat &
                                              ~_SOC_TR3_FUNC_PARITY_INTR_MASK);
            }

            if (irqStat & _SOC_TR3_FUNC_PARITY_INTR_MASK) {
                oldmask = soc_cmicm_intr1_disable(unit, irqMask);

                /* dispatch interrupt */
                LOG_INFO(BSL_LS_SOC_INTR,
                         (BSL_META_U(unit,
                                     "soc_cmicm_intr type 1 unit %d: dispatch\n"),
                          unit));

                rv = sal_dpc(soc_tr3_process_func_intr, INT_TO_PTR(unit),
                    INT_TO_PTR(oldmask), 0, 0, 0);
                if (rv) {
                    LOG_ERROR(BSL_LS_SOC_INTR, (BSL_META_U(unit,
                             "sal_dpc failed to queue oam event handler !!\n")));
                    /* sal_dpc failed. Enable the interrupt */
                    soc_cmicm_intr1_enable(unit, irqMask);
                }
            }

        }
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */
    {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "soc_cmicm_intr unit %d: "
                              "Disabling unhandled interrupt(s): %d\n"), unit, irqStat));
        (void)soc_cmicm_intr1_disable(unit, irqStat);
    }
}
#endif /* (BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)*/

#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
STATIC void
soc_cmicm_intr_link_stat(int unit, uint32 ignored)
{
    soc_control_t    *soc = SOC_CONTROL(unit);
    uint32 rval = 0;

    COMPILER_REFERENCE(ignored);

    soc_pci_analyzer_trigger(unit);

    soc->stat.intr_ls++;

    /* Clear interrupt */
    READ_CMIC_MIIM_SCAN_STATUSr(unit, &rval);
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Status: 0x%08x\n"), rval));
    WRITE_CMIC_MIIM_CLR_SCAN_STATUSr(unit, rval);

    /* Perform user callout, if one is registered */

    if (soc->soc_link_callout != NULL) {
        (*soc->soc_link_callout)(unit);
    }
}

STATIC void
soc_ser_engine_intr(int unit, uint32 val)
{
    int cmc;
    uint32 irqMask, irqStat;

    cmc = SOC_PCI_CMC(unit);
    irqMask = SOC_CMCx_IRQ1_MASK(unit,cmc);
    irqStat = soc_pci_read(unit, CMIC_CMCx_IRQ_STAT1_OFFSET(cmc));

    (void)soc_cmicm_intr1_disable(unit, irqMask);

    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "soc_cmicm_intr unit %d: "
                          "Disabling unhandled interrupt(s): %d\n"),
               unit, irqStat));
    (void)soc_cmicm_intr1_disable(unit, irqStat);
}

STATIC void
soc_cmicm_timesync_intr(int unit, uint32 val)
{
    int cmc;
    uint32 irqStat;
    uint32 irqMask;

    cmc = SOC_PCI_CMC(unit);
    irqMask = SOC_CMCx_IRQ1_MASK(unit,cmc);
    irqStat = soc_pci_read(unit, CMIC_CMCx_IRQ_STAT1_OFFSET(cmc));

#if defined(BCM_PETRA_SUPPORT)
    if (SOC_IS_JERICHO(unit)) {
        soc_cmicm_intr1_disable(unit, irqMask);
        soc_timesync_intr(unit);
        (void)soc_cmicm_intr1_enable(unit, irqMask);
    } else
#endif
#if defined(BCM_TIMESYNC_TIME_CAPTURE_SUPPORT)
#if defined(BCM_ESW_SUPPORT)
	if (soc_feature(unit, soc_feature_timesync_time_capture)) {
        soc_cmicm_intr1_disable(unit, irqMask);
        soc_esw_timesync_ts_intr(unit);
        (void)soc_cmicm_intr1_enable(unit, irqMask);
    } else
#endif /* defined(BCM_ESW_SUPPORT) */
#endif
    {
        LOG_ERROR(BSL_LS_SOC_COMMON,
            (BSL_META_U(unit, "soc_cmicm_timesync_intr unit %d: "
                              "Disabling unhandled interrupt(s): %d\n"),
                        unit, irqStat));
        (void)soc_cmicm_intr1_disable(unit, irqMask);
    }
}

STATIC void
soc_cmicm_pcie_link_down(int unit, uint32 val)
{
    int cmc;
    uint32 irqMask, reg;

    cmc = SOC_PCI_CMC(unit);
    irqMask = SOC_CMCx_IRQ1_MASK(unit, cmc);

    soc_cmicm_intr1_disable(unit, irqMask);

    READ_CMIC_PCIE_USERIF_STATUSr(unit, &reg);
    if (soc_reg_field_get(unit, CMIC_PCIE_USERIF_STATUSr, reg, USERIF_RESETf)) {
        /* Abort current DMA operations on all CMCs */
        if (soc_cmicm_dma_abort(unit, 0) != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_DMA, (BSL_META_U(unit, "Failed aborting DMA operations\n")));
        }

        /* Clear CMIC_PCIE_USERIF_STATUS.USERIF_RESET */
        READ_CMIC_PCIE_USERIF_STATUS_CLRr(unit, &reg);
        soc_reg_field_set(unit, CMIC_PCIE_USERIF_STATUS_CLRr, &reg,
                          USERIF_RESETf, 1);
        WRITE_CMIC_PCIE_USERIF_STATUS_CLRr(unit, reg);
    }

    (void)soc_cmicm_intr1_enable(unit, irqMask);
}

STATIC void
soc_cmicm_parity_intr(int unit, uint32 val)
{
    int cmc;
    uint32 irqStat;
#if defined(BCM_XGS_SUPPORT)
    uint32 irqMask, oldmask;
#endif

    cmc = SOC_PCI_CMC(unit);

#if defined(BCM_SABER2_SUPPORT)
    if (SOC_IS_SABER2(unit)) {
        irqStat = soc_pci_read(unit, CMIC_CMCx_IRQ_STAT3_OFFSET(cmc));
    } else {    
        irqStat = soc_pci_read(unit, CMIC_CMCx_IRQ_STAT2_OFFSET(cmc));
    }
#else
    irqStat = soc_pci_read(unit, CMIC_CMCx_IRQ_STAT2_OFFSET(cmc));
#endif

#if defined(BCM_XGS_SUPPORT)
#if defined(BCM_SABER2_SUPPORT)
    if (SOC_IS_SABER2(unit)) {
        irqMask = SOC_CMCx_IRQ3_MASK(unit,cmc);

        oldmask = soc_cmicm_intr3_disable(unit, irqMask);
    } else {
        irqMask = SOC_CMCx_IRQ2_MASK(unit,cmc);
        
        oldmask = soc_cmicm_intr2_disable(unit, irqMask);    
    }
#else
    irqMask = SOC_CMCx_IRQ2_MASK(unit,cmc);

    oldmask = soc_cmicm_intr2_disable(unit, irqMask);
#endif

    /* dispatch interrupt if we have handler */
    if (soc_ser_parity_error_cmicm_intr(INT_TO_PTR(unit), 0,
        INT_TO_PTR(oldmask), 0, 0)) {
        LOG_INFO(BSL_LS_SOC_INTR,
                 (BSL_META_U(unit,
                             "soc_cmicm_intr type 2 unit %d: dispatch\n"),
                  unit));
    } else 
#endif
    {
    /* Decoupling OAM interrupt handler from parity interrupt handler */
#ifdef BCM_TRIUMPH3_SUPPORT
        if (SOC_IS_TRIUMPH3(unit)) {
            uint32 rv;
            rv = sal_dpc((sal_dpc_fn_t)soc_tr3_process_func_intr, INT_TO_PTR(unit),
                    0, INT_TO_PTR(oldmask), 0, 0);
            if (rv) {
                LOG_ERROR(BSL_LS_SOC_INTR, (BSL_META_U(unit,
                         "sal_dpc failed to queue oam event handler !!\n")));
                /* sal_dpc failed. Enable the interrupt */
                soc_cmicm_intr2_enable(unit, irqMask);
            }

        } else
#endif
        {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "soc_cmicm_intr unit %d: "
                              "Disabling unhandled interrupt(s): %d\n"), 
                   unit, irqStat));
#if defined(BCM_SABER2_SUPPORT)
            if (SOC_IS_SABER2(unit)) {
                (void)soc_cmicm_intr3_disable(unit, irqStat);
            } else {            
                (void)soc_cmicm_intr2_disable(unit, irqStat);
            }
#else
            (void)soc_cmicm_intr2_disable(unit, irqStat);
#endif            
        }
    }
}

STATIC void
soc_cmicdv2_chip_parity_intr(int unit, uint32 val)
{
    int cmc;
    uint32 irqStat;
#if defined(BCM_XGS_SUPPORT)
    uint32 irqMask;
#endif

    cmc = SOC_PCI_CMC(unit);
    irqStat = soc_pci_read(unit, CMIC_CMCx_IRQ_STAT2_OFFSET(cmc));

#if defined(BCM_XGS_SUPPORT)
    irqMask = SOC_CMCx_IRQ2_MASK(unit,cmc);

    soc_cmicm_intr2_disable(unit, irqMask & irqStat);

#if defined(BCM_MONTEREY_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_MONTEREY(unit) || SOC_IS_TOMAHAWK3(unit)) {
#if defined (INCLUDE_GDPLL) || defined(BCM_TOMAHAWK3_SUPPORT)
            uint32 rv;
#endif
        /* dispatch interrupt */
        LOG_INFO(BSL_LS_SOC_INTR,
            (BSL_META_U(unit,
            "soc_cmicm_intr type 3 unit %d: dispatch\n"),
            unit));

        switch (val) {
            case 24:
#if defined (INCLUDE_GDPLL) || defined(BCM_TOMAHAWK3_SUPPORT)
                rv = sal_dpc(soc_nanosync_intr, INT_TO_PTR(unit), 0, 0, 0, 0);
                if (rv) {
                    LOG_ERROR(BSL_LS_SOC_INTR, (BSL_META_U(unit,
                         "sal_dpc failed to queue oam event handler !!\n")));
                    /* sal_dpc failed. Enable the interrupt */
                    soc_cmicm_intr2_enable(unit, irqMask);
                }
#endif
                break;

            case 25:
#ifdef INCLUDE_GDPLL
                rv = sal_dpc(soc_nanosync_debug_intr, INT_TO_PTR(unit), 0, 0, 0, 0);
                if (rv) {
                    LOG_ERROR(BSL_LS_SOC_INTR, (BSL_META_U(unit,
                         "sal_dpc failed to queue oam event handler !!\n")));
                    /* sal_dpc failed. Enable the interrupt */
                    soc_cmicm_intr2_enable(unit, irqMask);
                }
#endif
                break;

            default:
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit, "soc_cmicm_intr unit %d: "
                                            "Disabling unhandled interrupt(s): %d\n"),
                                      unit, irqStat));
                (void)soc_cmicm_intr2_disable(unit, irqStat);
        }
    } else
#endif  /*#if defined(BCM_MONTEREY_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)*/
#endif  /* BCM_XGS_SUPPORT */
    {
        LOG_ERROR(BSL_LS_SOC_COMMON,
            (BSL_META_U(unit, "soc_cmicm_intr unit %d: "
                              "Disabling unhandled interrupt(s): %d\n"),
                        unit, irqStat));
        (void)soc_cmicm_intr2_disable(unit, irqStat);
    }
}

STATIC void
soc_cmicdv2_parity_intr(int unit, uint32 val)
{
    int cmc;
    uint32 irqStat;
#if defined(BCM_XGS_SUPPORT)
    uint32 irqMask, oldmask;
#endif

    cmc = SOC_PCI_CMC(unit);
    /* The parity error interrupts are moved to STAT3 for CMICD v2 */
    irqStat = soc_pci_read(unit, CMIC_CMCx_IRQ_STAT3_OFFSET(cmc));

#if defined(BCM_XGS_SUPPORT)
    irqMask = SOC_CMCx_IRQ3_MASK(unit,cmc);

    oldmask = soc_cmicm_intr3_disable(unit, irqMask);

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit)) {
        /* SER interrupts */
        if ((val >= 4 && val <= 16) || 
           ((val >= 26 && val <= 29) && SOC_IS_TOMAHAWK2(unit))) {
            LOG_INFO(BSL_LS_SOC_INTR,
                     (BSL_META_U(unit,
                                 "soc_cmicm_intr type 3 unit %d: dispatch\n"), unit));
            if (soc_ser_parity_error_cmicm_intr(INT_TO_PTR(unit), 0,
                                                INT_TO_PTR(oldmask),
                                                INT_TO_PTR(3),
                                                INT_TO_PTR(val))) {
        
            } else {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "soc_cmicdv2_intr unit %d: "
                                      "Disabling unhandled interrupt(s): %d\n"), 
                           unit, irqStat));
                (void)soc_cmicm_intr3_disable(unit, irqStat);
            }
        } else if (val == 2) { /* PVTMON interrupt */
            sal_dpc(soc_tomahawk_temperature_intr, INT_TO_PTR(unit),
                    0, 0, 0, 0);
        } else if (val == 1) { /* L2 MGMT */
            /* dispatch interrupt */
            LOG_INFO(BSL_LS_SOC_INTR,
                     (BSL_META_U(unit,
                                 "soc_cmicm_intr type 3 unit %d: dispatch\n"),
                      unit));

            sal_dpc(soc_tomahawk_process_func_intr, INT_TO_PTR(unit),
                    INT_TO_PTR(oldmask), 0, 0, 0);
        }
    } else

#endif /* BCM_TOMAHAWK_SUPPORT */

#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        /* SER interrupts */
        if (val <= 10) {
            LOG_INFO(BSL_LS_SOC_INTR,
                     (BSL_META_U(unit,
                                 "soc_cmicm_intr type 3 unit %d: dispatch\n"), unit));
            if (soc_ser_parity_error_cmicm_intr(INT_TO_PTR(unit), 0,
                                                INT_TO_PTR(oldmask), 
                                                INT_TO_PTR(3),
                                                INT_TO_PTR(val))) {
            } else {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "soc_cmicdv2_intr unit %d: "
                                      "Disabling unhandled interrupt(s): %d\n"),
                           unit, irqStat));
                (void)soc_cmicm_intr3_disable(unit, irqStat);
            }
        } else if (val == 11) { /* TOP(PVTMON,AVS) interrupt */
#ifdef BCM_MONTEREY_SUPPORT
            if (SOC_IS_MONTEREY(unit)) {
                sal_dpc(soc_monterey_top_intr, INT_TO_PTR(unit),
                    0, 0, 0, 0);
            } else 
#endif
            { 
                sal_dpc(soc_apache_top_intr, INT_TO_PTR(unit),
                    0, 0, 0, 0);
            }
        } else if ((val == 13) || (val == 14)) { /* L2 Overflow */
            /* dispatch interrupt */
            LOG_INFO(BSL_LS_SOC_INTR,
                     (BSL_META_U(unit,
                                 "soc_cmicm_intr type 3 unit %d: dispatch\n"),
                      unit));

#ifdef BCM_MONTEREY_SUPPORT
            if (SOC_IS_MONTEREY(unit)) {
                sal_dpc(soc_monterey_process_func_intr, INT_TO_PTR(unit),
                        INT_TO_PTR(oldmask), INT_TO_PTR(val), 0, 0);
            } else 
#endif
           {
            sal_dpc(soc_apache_process_func_intr, INT_TO_PTR(unit),
                    INT_TO_PTR(oldmask), INT_TO_PTR(val), 0, 0);
           }
        }
    } else
#endif /* BCM_APACHE_SUPPORT */

    /* dispatch interrupt if we have handler */
    if (soc_ser_parity_error_cmicm_intr(INT_TO_PTR(unit), 0,
        INT_TO_PTR(oldmask), INT_TO_PTR(3), INT_TO_PTR(val))) {
        LOG_INFO(BSL_LS_SOC_INTR,
                 (BSL_META_U(unit,
                             "soc_cmicdv2_intr type 3 unit %d: dispatch\n"),
                  unit));
        
    } else 
#endif
    {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "soc_cmicdv2_intr unit %d: "
                              "Disabling unhandled interrupt(s): %d\n"), 
                   unit, irqStat));
        
        (void)soc_cmicm_intr3_disable(unit, irqStat);
    }
}

STATIC void
soc_cmicm_block_lo_intr(int unit, uint32 val)
{
    uint32  irqStat = 0;
    int cmc;

    cmc = SOC_PCI_CMC(unit);
    irqStat = soc_pci_read(unit, CMIC_CMCx_IRQ_STAT3_OFFSET(cmc));

    {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "soc_cmicm_intr unit %d: "
                              "Disabling unhandled interrupt(s): %d\n"), 
                   unit, irqStat));
        (void)soc_cmicm_intr3_disable(unit, irqStat);
    }
}

#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
STATIC void
soc_cmicdv2_block_intr4(int unit, uint32 val)
{
    int cmc;
    uint32 irqStat;
#if defined(BCM_XGS_SUPPORT)
    uint32 irqMask, oldmask;
#endif

    cmc = SOC_PCI_CMC(unit);
    /* The parity error interrupts are moved to STAT4 for CMICD v2 */
    irqStat = soc_pci_read(unit, CMIC_CMCx_IRQ_STAT4_OFFSET(cmc));

#if defined(BCM_XGS_SUPPORT)
    irqMask = SOC_CMCx_IRQ4_MASK(unit,cmc);

    oldmask = soc_cmicm_intr4_disable(unit, irqMask);

#ifdef BCM_MONTEREY_SUPPORT
    if (SOC_IS_MONTEREY(unit)) {
#ifdef INCLUDE_XFLOW_MACSEC
        if((val == 22) &&
           (!soc_feature(unit, soc_feature_xflow_macsec_poll_intr))) {
            /* MACSEC interrupts */
            sal_dpc(soc_monterey_process_macsec_intr, INT_TO_PTR(unit), 0,
                    INT_TO_PTR(oldmask), INT_TO_PTR(4), INT_TO_PTR(val));
        } else
#endif
        {
            /* dispatch PortMacro interrupt if we have handler */
            sal_dpc(soc_monterey_process_pm_intr, INT_TO_PTR(unit), 0,
                    INT_TO_PTR(oldmask), INT_TO_PTR(4), INT_TO_PTR(val));
        }
    } else
#endif
    if (soc_ser_parity_error_cmicm_intr(INT_TO_PTR(unit), 0,
        INT_TO_PTR(oldmask), INT_TO_PTR(4), INT_TO_PTR(val))) {
        LOG_INFO(BSL_LS_SOC_INTR,
                 (BSL_META_U(unit,
                             "soc_cmicdv2_intr type 4 unit %d: dispatch\n"),
                  unit));
        
    } else 
#endif
    {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "soc_cmicdv2_intr unit %d: "
                              "Disabling unhandled interrupt(s): %d\n"), 
                   unit, irqStat));
        
        (void)soc_cmicm_intr4_disable(unit, irqStat);
    }
}

STATIC void
soc_cmicdv2_block_intr5(int unit, uint32 val)
{
    int cmc;
    uint32 irqStat;
#if defined(BCM_XGS_SUPPORT)
    uint32 irqMask, oldmask;
#endif

    cmc = SOC_PCI_CMC(unit);
    /* The parity error interrupts are moved to STAT5 for CMICD v2 */
    irqStat = soc_pci_read(unit, CMIC_CMCx_IRQ_STAT5_OFFSET(cmc));

#if defined(BCM_XGS_SUPPORT)
    irqMask = SOC_CMCx_IRQ5_MASK(unit,cmc);

    oldmask = soc_cmicm_intr5_disable(unit, irqMask);

    /* dispatch interrupt if we have handler */
    if (soc_ser_parity_error_cmicm_intr(INT_TO_PTR(unit), 0,
        INT_TO_PTR(oldmask), INT_TO_PTR(5), INT_TO_PTR(val))) {
        LOG_INFO(BSL_LS_SOC_INTR,
                 (BSL_META_U(unit,
                             "soc_cmicdv2_intr type 5 unit %d: dispatch\n"),
                  unit));
        
    } else 
#endif
    {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "soc_cmicdv2_intr unit %d: "
                              "Disabling unhandled interrupt(s): %d\n"), 
                   unit, irqStat));
        
        (void)soc_cmicm_intr5_disable(unit, irqStat);
    }
}

STATIC void
soc_cmicdv4_block_intr6(int unit, uint32 val)
{
    int cmc;
    uint32 irqStat;
#if defined(BCM_XGS_SUPPORT)
    uint32 irqMask, oldmask;
#endif

    cmc = SOC_PCI_CMC(unit);
    irqStat = soc_pci_read(unit, CMIC_CMCx_IRQ_STAT6_OFFSET(cmc));

#if defined(BCM_XGS_SUPPORT)
    irqMask = SOC_CMCx_IRQ6_MASK(unit,cmc);
    oldmask = soc_cmicm_intr6_disable(unit, irqMask);

    /* dispatch interrupt if we have handler */
    if (soc_ser_parity_error_cmicm_intr(INT_TO_PTR(unit), 0,
        INT_TO_PTR(oldmask), INT_TO_PTR(6), INT_TO_PTR(val))) {
        LOG_INFO(BSL_LS_SOC_INTR,
                 (BSL_META_U(unit,
                             "soc_cmicdv4_intr type 6 unit %d: dispatch\n"),
                  unit));
    } else
#endif
    {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "soc_cmicdv4_intr unit %d: "
                              "Disabling unhandled interrupt(s): %d\n"),
                   unit, irqStat));

        (void)soc_cmicm_intr6_disable(unit, irqStat);
    }
}

STATIC void
soc_cmicm_link_stat(int unit, uint32 ignored)
{
    soc_control_t    *soc = SOC_CONTROL(unit);

    COMPILER_REFERENCE(ignored);

    soc_pci_analyzer_trigger(unit);

    soc->stat.intr_ls++;

    /* Clear interrupt */

    soc_pci_write(unit, CMIC_MIIM_CLR_SCAN_STATUS_OFFSET, CLR_LINK_STATUS_CHANGE_MASK);

    /* Perform user callout, if one is registered */

    if (soc->soc_link_callout != NULL) {
    (*soc->soc_link_callout)(unit);
    }
}
#endif /* defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)  || defined(BCM_DFE_SUPPORT)*/

#endif /* defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)  || defined(BCM_DFE_SUPPORT)*/

/*
 * Enable (unmask) or disable (mask) a set of CMIC interrupts.  These
 * routines should be used instead of manipulating CMIC_IRQ_MASK
 * directly, since a read-modify-write is required.  The return value is
 * the previous mask (can pass mask of 0 to just get the current mask).
 * for CMICm use CMIC_CMCx_PCIE_IRQ_MASK0.
 */

uint32
soc_cmicm_cmcx_intr0_enable(int unit, int cmc, uint32 mask)
{
    uint32 oldMask;
    uint32 newMask;
    int s;

    s = sal_splhi();
    oldMask = SOC_CMCx_IRQ0_MASK(unit,cmc);
    SOC_CMCx_IRQ0_MASK(unit,cmc) |= mask;
    newMask = SOC_CMCx_IRQ0_MASK(unit,cmc);
    /* In polled mode, the hardware IRQ mask is always zero */
    if (SOC_CONTROL(unit)->soc_flags & SOC_F_POLLED) {
        newMask = 0;
    }
    LOG_INFO(BSL_LS_SOC_INTR,
             (BSL_META_U(unit,
                         "soc_cmicm_intr0_enable cmc %d unit %d: mask 0x%8x\n"),
              cmc, unit, mask));
    IRQ_MASK0_SET(unit, CMIC_CMCx_PCIE_IRQ_MASK0_OFFSET(cmc), newMask);
    sal_spl(s);
    return oldMask;
}

uint32
soc_cmicm_cmcx_intr0_disable(int unit, int cmc, uint32 mask)
{
    uint32 oldMask;
    uint32 newMask;
    int s;

    s = sal_splhi();
    oldMask = SOC_CMCx_IRQ0_MASK(unit,cmc);
    SOC_CMCx_IRQ0_MASK(unit,cmc) &= ~mask;
    newMask = SOC_CMCx_IRQ0_MASK(unit,cmc);
    /* In polled mode, the hardware IRQ mask is always zero */
    if (SOC_CONTROL(unit)->soc_flags & SOC_F_POLLED) {
        newMask = 0;
    }
    LOG_INFO(BSL_LS_SOC_INTR,
             (BSL_META_U(unit,
                         "soc_cmicm_intr0_disable cmc %d unit %d: mask 0x%8x\n"),
              cmc, unit, mask));
    IRQ_MASK0_SET(unit, CMIC_CMCx_PCIE_IRQ_MASK0_OFFSET(cmc), newMask);
    sal_spl(s);
    return oldMask;
}

/*
 * Enable (unmask) or disable (mask) a set of CMICM Common / Switch-Specific 
 * interrupts.  These routines should be used instead of manipulating 
 * CMIC_CMCx_PCIE_IRQ_MASK1 directly, since a read-modify-write is required.
 * The return value is the previous mask (can pass mask of 0 to just
 * get the current mask) 
 */

uint32
soc_cmicm_cmcx_intr1_enable(int unit, int cmc, uint32 mask)
{
    uint32 oldMask;
    uint32 newMask;
    int s;

    s = sal_splhi();
    oldMask = SOC_CMCx_IRQ1_MASK(unit,cmc);
    SOC_CMCx_IRQ1_MASK(unit,cmc) |= mask;
    newMask = SOC_CMCx_IRQ1_MASK(unit,cmc);
    /* In polled mode, the hardware IRQ mask is always zero */
    if (SOC_CONTROL(unit)->soc_flags & SOC_F_POLLED) {
        newMask = 0;
    }
    IRQ_MASKx_SET(unit, CMIC_CMCx_PCIE_IRQ_MASK1_OFFSET(cmc), newMask);
    sal_spl(s);
    return oldMask;
}

uint32
soc_cmicm_cmcx_intr1_disable(int unit, int cmc, uint32 mask)
{
    uint32 oldMask;
    uint32 newMask;
    int s;

    s = sal_splhi();
    oldMask = SOC_CMCx_IRQ1_MASK(unit,cmc);
    SOC_CMCx_IRQ1_MASK(unit,cmc) &= ~mask;
    newMask = SOC_CMCx_IRQ1_MASK(unit,cmc);
    /* In polled mode, the hardware IRQ mask is always zero */
    if (SOC_CONTROL(unit)->soc_flags & SOC_F_POLLED) {
        newMask = 0;
    }
    IRQ_MASKx_SET(unit, CMIC_CMCx_PCIE_IRQ_MASK1_OFFSET(cmc), newMask);
    sal_spl(s);
    return oldMask;
}

uint32
soc_cmicm_cmcx_intr2_enable(int unit, int cmc, uint32 mask)
{
    uint32 oldMask;
    uint32 newMask;
    int s;

    s = sal_splhi();
    oldMask = SOC_CMCx_IRQ2_MASK(unit,cmc);
    SOC_CMCx_IRQ2_MASK(unit,cmc) |= mask;
    newMask = SOC_CMCx_IRQ2_MASK(unit,cmc);
    /* In polled mode, the hardware IRQ mask is always zero */
    if (SOC_CONTROL(unit)->soc_flags & SOC_F_POLLED) {
        newMask = 0;
    }
    IRQ_MASKx_SET(unit, CMIC_CMCx_PCIE_IRQ_MASK2_OFFSET(cmc), newMask);
    sal_spl(s);
    return oldMask;
}

uint32
soc_cmicm_cmcx_intr2_disable(int unit, int cmc, uint32 mask)
{
    uint32 oldMask;
    uint32 newMask;
    int s;

    s = sal_splhi();
    oldMask = SOC_CMCx_IRQ2_MASK(unit,cmc);
    SOC_CMCx_IRQ2_MASK(unit,cmc) &= ~mask;
    newMask = SOC_CMCx_IRQ2_MASK(unit,cmc);
    /* In polled mode, the hardware IRQ mask is always zero */
    if (SOC_CONTROL(unit)->soc_flags & SOC_F_POLLED) {
        newMask = 0;
    }
    IRQ_MASKx_SET(unit, CMIC_CMCx_PCIE_IRQ_MASK2_OFFSET(cmc), newMask);
    sal_spl(s);
    return oldMask;
}

uint32
soc_cmicm_cmcx_intr3_enable(int unit, int cmc, uint32 mask)
{
    uint32 oldMask;
    uint32 newMask;
    int s;

    s = sal_splhi();
    oldMask = SOC_CMCx_IRQ3_MASK(unit,cmc);
    SOC_CMCx_IRQ3_MASK(unit,cmc) |= mask;
    newMask = SOC_CMCx_IRQ3_MASK(unit,cmc);
    /* In polled mode, the hardware IRQ mask is always zero */
    if (SOC_CONTROL(unit)->soc_flags & SOC_F_POLLED) {
        newMask = 0;
    }
    IRQ_MASKx_SET(unit, CMIC_CMCx_PCIE_IRQ_MASK3_OFFSET(cmc), newMask);
    sal_spl(s);
    return oldMask;
}

uint32
soc_cmicm_cmcx_intr3_disable(int unit, int cmc, uint32 mask)
{
    uint32 oldMask;
    uint32 newMask;
    int s;

    s = sal_splhi();
    oldMask = SOC_CMCx_IRQ3_MASK(unit,cmc);
    SOC_CMCx_IRQ3_MASK(unit,cmc) &= ~mask;
    newMask = SOC_CMCx_IRQ3_MASK(unit,cmc);
    /* In polled mode, the hardware IRQ mask is always zero */
    if (SOC_CONTROL(unit)->soc_flags & SOC_F_POLLED) {
        newMask = 0;
    }
    IRQ_MASKx_SET(unit, CMIC_CMCx_PCIE_IRQ_MASK3_OFFSET(cmc), newMask);
    sal_spl(s);

    return oldMask;
}

uint32
soc_cmicm_cmcx_intr4_enable(int unit, int cmc, uint32 mask)
{
    uint32 oldMask;
    uint32 newMask;
    int s;

    s = sal_splhi();
    oldMask = SOC_CMCx_IRQ4_MASK(unit,cmc);
    SOC_CMCx_IRQ4_MASK(unit,cmc) |= mask;
    newMask = SOC_CMCx_IRQ4_MASK(unit,cmc);
    /* In polled mode, the hardware IRQ mask is always zero */
    if (SOC_CONTROL(unit)->soc_flags & SOC_F_POLLED) {
        newMask = 0;
    }
    IRQ_MASKx_SET(unit, CMIC_CMCx_PCIE_IRQ_MASK4_OFFSET(cmc), newMask);
    sal_spl(s);
    return oldMask;
}

uint32
soc_cmicm_cmcx_intr4_disable(int unit, int cmc, uint32 mask)
{
    uint32 oldMask;
    uint32 newMask;
    int s;

    s = sal_splhi();
    oldMask = SOC_CMCx_IRQ4_MASK(unit,cmc);
    SOC_CMCx_IRQ4_MASK(unit,cmc) &= ~mask;
    newMask = SOC_CMCx_IRQ4_MASK(unit,cmc);
    /* In polled mode, the hardware IRQ mask is always zero */
    if (SOC_CONTROL(unit)->soc_flags & SOC_F_POLLED) {
        newMask = 0;
    }
    IRQ_MASKx_SET(unit, CMIC_CMCx_PCIE_IRQ_MASK4_OFFSET(cmc), newMask);
    sal_spl(s);
    return oldMask;
}

uint32
soc_cmicm_cmcx_intr5_enable(int unit, int cmc, uint32 mask)
{
    uint32 oldMask;
    uint32 newMask;
    int s;

    s = sal_splhi();
    oldMask = SOC_CMCx_IRQ5_MASK(unit,cmc);
    SOC_CMCx_IRQ5_MASK(unit,cmc) |= mask;
    newMask = SOC_CMCx_IRQ5_MASK(unit,cmc);
    /* In polled mode, the hardware IRQ mask is always zero */
    if (SOC_CONTROL(unit)->soc_flags & SOC_F_POLLED) {
        newMask = 0;
    }
    IRQ_MASKx_SET(unit, CMIC_CMCx_PCIE_IRQ_MASK5_OFFSET(cmc), newMask);
    sal_spl(s);
    return oldMask;
}

uint32
soc_cmicm_cmcx_intr5_disable(int unit, int cmc, uint32 mask)
{
    uint32 oldMask;
    uint32 newMask;
    int s;
    s = sal_splhi();
    oldMask = SOC_CMCx_IRQ5_MASK(unit,cmc);
    SOC_CMCx_IRQ5_MASK(unit,cmc) &= ~mask;
    newMask = SOC_CMCx_IRQ5_MASK(unit,cmc);
    /* In polled mode, the hardware IRQ mask is always zero */
    if (SOC_CONTROL(unit)->soc_flags & SOC_F_POLLED) {
        newMask = 0;
    }
    IRQ_MASKx_SET(unit, CMIC_CMCx_PCIE_IRQ_MASK5_OFFSET(cmc), newMask);
    sal_spl(s);
    return oldMask;
}

uint32
soc_cmicm_cmcx_intr6_enable(int unit, int cmc, uint32 mask)
{
    uint32 oldMask;
    uint32 newMask;
    int s;

   s = sal_splhi();
    oldMask = SOC_CMCx_IRQ6_MASK(unit,cmc);
    SOC_CMCx_IRQ6_MASK(unit,cmc) |= mask;
    newMask = SOC_CMCx_IRQ6_MASK(unit,cmc);
    /* In polled mode, the hardware IRQ mask is always zero */
    if (SOC_CONTROL(unit)->soc_flags & SOC_F_POLLED) {
        newMask = 0;
    }
    IRQ_MASKx_SET(unit, CMIC_CMCx_PCIE_IRQ_MASK6_OFFSET(cmc), newMask);
    sal_spl(s);
    return oldMask;
}

uint32
soc_cmicm_cmcx_intr6_disable(int unit, int cmc, uint32 mask)
{
    uint32 oldMask;
    uint32 newMask;
    int s;
    s = sal_splhi();

    oldMask = SOC_CMCx_IRQ6_MASK(unit,cmc);
    SOC_CMCx_IRQ6_MASK(unit,cmc) &= ~mask;
    newMask = SOC_CMCx_IRQ6_MASK(unit,cmc);
    /* In polled mode, the hardware IRQ mask is always zero */
    if (SOC_CONTROL(unit)->soc_flags & SOC_F_POLLED) {
        newMask = 0;
    }
    IRQ_MASKx_SET(unit, CMIC_CMCx_PCIE_IRQ_MASK6_OFFSET(cmc), newMask);
    sal_spl(s);
    return oldMask;
}

uint32
soc_cmicm_intr0_enable(int unit, uint32 mask)
{
    return soc_cmicm_cmcx_intr0_enable(unit, SOC_PCI_CMC(unit), mask);
}

uint32
soc_cmicm_intr0_disable(int unit, uint32 mask)
{
    return soc_cmicm_cmcx_intr0_disable(unit, SOC_PCI_CMC(unit), mask);
}

uint32
soc_cmicm_intr1_enable(int unit, uint32 mask)
{
    return soc_cmicm_cmcx_intr1_enable(unit, SOC_PCI_CMC(unit), mask);
}

uint32
soc_cmicm_intr1_disable(int unit, uint32 mask)
{
    return soc_cmicm_cmcx_intr1_disable(unit, SOC_PCI_CMC(unit), mask);
}

uint32
soc_cmicm_intr2_enable(int unit, uint32 mask)
{
    return soc_cmicm_cmcx_intr2_enable(unit, SOC_PCI_CMC(unit), mask);
}

uint32
soc_cmicm_intr2_disable(int unit, uint32 mask)
{
    return soc_cmicm_cmcx_intr2_disable(unit, SOC_PCI_CMC(unit), mask);
}

uint32
soc_cmicm_intr3_enable(int unit, uint32 mask)
{
    return soc_cmicm_cmcx_intr3_enable(unit, SOC_PCI_CMC(unit), mask);
}

uint32
soc_cmicm_intr3_disable(int unit, uint32 mask)
{
    return soc_cmicm_cmcx_intr3_disable(unit, SOC_PCI_CMC(unit), mask);
}

uint32
soc_cmicm_intr4_enable(int unit, uint32 mask)
{
    return soc_cmicm_cmcx_intr4_enable(unit, SOC_PCI_CMC(unit), mask);
}

uint32
soc_cmicm_intr4_disable(int unit, uint32 mask)
{
    return soc_cmicm_cmcx_intr4_disable(unit, SOC_PCI_CMC(unit), mask);
}

uint32
soc_cmicm_intr5_enable(int unit, uint32 mask)
{
    return soc_cmicm_cmcx_intr5_enable(unit, SOC_PCI_CMC(unit), mask);
}

uint32
soc_cmicm_intr5_disable(int unit, uint32 mask)
{
    return soc_cmicm_cmcx_intr5_disable(unit, SOC_PCI_CMC(unit), mask);
}

uint32
soc_cmicm_intr6_enable(int unit, uint32 mask)
{
    return soc_cmicm_cmcx_intr6_enable(unit, SOC_PCI_CMC(unit), mask);
}

uint32
soc_cmicm_intr6_disable(int unit, uint32 mask)
{
    return soc_cmicm_cmcx_intr6_disable(unit, SOC_PCI_CMC(unit), mask);
}
/*
 * SOC CMICm Interrupt Service Routine
 */

#define POLL_LIMIT 100000

void
soc_cmicm_intr(void *_unit)
{
    soc_control_t *soc;
    uint32 irqStat, irqMask;
    int unit = PTR_TO_INT(_unit);
    int cmc = 0, i = 0;
    int poll_limit = POLL_LIMIT;
    intr_handler_t *intr_handler = soc_cmicm_intr_handlers;
    int arm;
    int s;
#ifdef SEPARATE_PKTDMA_INTR_HANDLER
    uint32 pktdma_status = 0x7800ff00;
    int pktdma_poll_limit = 2;
#endif


    soc = SOC_CONTROL(unit);
    /*
     * Our handler is permanently registered in soc_probe().  If our
     * unit is not attached yet, it could not have generated this
     * interrupt.  The interrupt line must be shared by multiple PCI
     * cards.  Simply ignore the interrupt and let another handler
     * process it.
     */
    if (soc == NULL) {
        return;
    }
    if (soc->soc_flags & SOC_F_BUSY) {
        return;
    }
    if (!(soc->soc_flags & SOC_F_ATTACHED)) {
        return;
    }

#ifdef SAL_SPL_LOCK_ON_IRQ
    s = sal_splhi();
#endif

    if (soc->irq_handler_suspend) {
#ifdef SAL_SPL_LOCK_ON_IRQ
        sal_spl(s);
#endif
        return;
    }


    cmc = SOC_PCI_CMC(unit);
    soc->stat.intr++; /* Update count */

    if (SOC_IS_KATANA(unit)) {
        intr_handler = soc_cmicm_intr_handlers0_fifo_dma;
    }

    if (soc_feature(unit, soc_feature_sbusdma)) {
        intr_handler = soc_cmicm_intr_handlers0;
    }
    /*
     * Read IRQ Status and IRQ Mask and AND to determine active ints.
     * These are re-read each time since either can be changed by ISRs.
     */
    for (;;) {
        if (soc_feature(unit, soc_feature_cmicm_multi_dma_cmc) &&
            (SOC_PCI_CMCS_NUM(unit) > 1)) {
            for (i = soc->next_int0_cmc; i < soc->next_int0_cmc + SOC_PCI_CMCS_NUM(unit); i++) {
                cmc = i % SOC_PCI_CMCS_NUM(unit);
                irqStat = soc_pci_read(unit, CMIC_CMCx_IRQ_STAT0_OFFSET(cmc));
                if (irqStat != 0) {
                    irqMask = SOC_CMCx_IRQ0_MASK(unit, cmc);
                    irqStat &= irqMask;
                    if (irqStat != 0) {
#ifdef SEPARATE_PKTDMA_INTR_HANDLER
                        if (irqStat == (irqStat & pktdma_status))
                        {
                            if (--pktdma_poll_limit == 0)
                            {
                                return;
                            }
                            /**  Bypass to interrupts IRQ_STAT1, IRQ_STAT2... */
                            goto check_type1;
                        }
#endif
                        goto detected_irq0;
                    }
                }
                soc->next_int0_cmc = (cmc + 1) % SOC_PCI_CMCS_NUM(unit);
            }
            /** re-assign cmc to origin */
            cmc = SOC_PCI_CMC(unit);
            goto check_type1;
        } else {
            cmc = SOC_PCI_CMC(unit);
            irqStat = soc_pci_read(unit, CMIC_CMCx_IRQ_STAT0_OFFSET(cmc));
            if (irqStat == 0) {
                goto check_type1;  /* No pending Interrupts */
            }
            irqMask = SOC_CMCx_IRQ0_MASK(unit,cmc);
            irqStat &= irqMask;
            if (irqStat == 0) {
                goto check_type1;
            }
#ifdef SEPARATE_PKTDMA_INTR_HANDLER
            if (irqStat == (irqStat & pktdma_status))
            {
                if (--pktdma_poll_limit == 0)
                {
                    return;
                }
                /**  Bypass to interrupts IRQ_STAT1, IRQ_STAT2... */
                goto check_type1;
            }
#endif

        }

detected_irq0:
        i = 0;
        /*
        * We may have received an interrupt before all data has been
        * posted from the device or intermediate bridge. 
        * The PCI specification requires that we read a device register
        * to make sure pending data is flushed. 
        */
        soc_pci_read(unit, CMIC_CMCx_SCHAN_CTRL_OFFSET(cmc)); 
        soc_pci_read(unit, CMIC_CMCx_PCIE_IRQ_MASK0_OFFSET(cmc));
    
        for (; intr_handler[i].mask; i++) {
            if (irqStat & intr_handler[i].mask) {

            /* dispatch interrupt */
            LOG_INFO(BSL_LS_SOC_INTR,
                     (BSL_META_U(unit,
                                 "soc_cmicm_intr type 0 unit %d: dispatch %s\n"),
                      unit, intr_handler[i].intr_name));
    
            (*intr_handler[i].intr_fn)
                (unit, cmc * N_DMA_CHAN + intr_handler[i].intr_data);
    
            /*
             * Prevent infinite loop in interrupt handler by
             * disabling the offending interrupt(s).
             */
            /* coverity[dead_error_condition] */
            if (--poll_limit == 0) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "soc_cmicm_intr unit %d cmc %d: "
                                      "ERROR can't clear type 0 interrupt(s): "
                                      "IRQ=0x%x (disabling 0x%x)\n"),
                           unit, cmc, irqStat, intr_handler[i].mask));
                (void)soc_cmicm_cmcx_intr0_disable(unit, cmc, intr_handler[i].mask);
                poll_limit = POLL_LIMIT;
            }
    
            /*
             * Go back and re-read IRQ status.  Start processing
             * from scratch since handler may clear more than one
             * bit. We don't leave the ISR until all of the bits
             * have been cleared and their handlers called.
             */
            break;
            }
        }
    }
check_type1:

#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
    /* process irq1 (chip_func) */
/* Use the below for open-loop testing with JR based platforms
   IRQ1 mask is not being set inspite of the interrupt being enabled */
#if defined(TIMESYNC_INTR_MASK_FORCE_SET)
    if (SOC_IS_JERICHO(unit)) {
        SOC_CMCx_IRQ1_MASK(unit,cmc) |= IRQ_CMCx_TIMESYNC_INTR;
    }
#endif
    poll_limit = POLL_LIMIT;

    for (;;) {
        irqStat = soc_pci_read(unit, CMIC_CMCx_IRQ_STAT1_OFFSET(cmc));
        if (irqStat == 0) {
            goto check_type2;  /* No pending Interrupts */
        }
        irqMask = SOC_CMCx_IRQ1_MASK(unit,cmc);
        irqStat &= irqMask;
        if (irqStat == 0) {
            goto check_type2;
        }
        
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "soc_cmicm_intr unit %d: irqStat1 = 0x%x\n"),
                     unit, irqStat));

        intr_handler = soc_cmicm_intr_handlers1;
        i = 0;
        
        for (; intr_handler[i].mask; i++) {
            if (irqStat & intr_handler[i].mask) {
                /* dispatch interrupt */
                LOG_INFO(BSL_LS_SOC_INTR,
                         (BSL_META_U(unit,
                                     "soc_cmicm_intr type 1 unit %d: dispatch %s\n"),
                          unit, intr_handler[i].intr_name));
                
                (*intr_handler[i].intr_fn)
                    (unit, intr_handler[i].intr_data);
                
                /* coverity[dead_error_condition] */
                if (--poll_limit == 0) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "soc_cmicm_intr unit %d: "
                                          "ERROR can't clear type 1 interrupt(s): "
                                          "IRQ=0x%x (disabling 0x%x)\n"),
                               unit, irqStat, intr_handler[i].mask));
                    (void)soc_cmicm_intr1_disable(unit, intr_handler[i].mask);
                    poll_limit = POLL_LIMIT;
                }
                break;
            }
        }
        
        /* optimization: don't go back to re-read (PCI transaction) when there is 
         * only 1 interrupt handled in IRQ1 (for now, this is true)
         */
        if (sizeof(soc_cmicm_intr_handlers1)/sizeof(intr_handler_t) == 2) {
            break;
        }
    }
check_type2:
    poll_limit = POLL_LIMIT;
    /* process irq2 (parity error) */
    for (;;) {
#if defined(BCM_SABER2_SUPPORT)
        if (SOC_IS_SABER2(unit)) {
            irqStat = soc_pci_read(unit, CMIC_CMCx_IRQ_STAT3_OFFSET(cmc));
        } else {
            irqStat = soc_pci_read(unit, CMIC_CMCx_IRQ_STAT2_OFFSET(cmc));
        }
#else
        irqStat = soc_pci_read(unit, CMIC_CMCx_IRQ_STAT2_OFFSET(cmc));
#endif        
        if (irqStat == 0) {
            goto check_type3;  /* No pending Interrupts */
        }
#if defined(BCM_SABER2_SUPPORT)
        if (SOC_IS_SABER2(unit)) { 
            irqMask = SOC_CMCx_IRQ3_MASK(unit,cmc);
        } else {
            irqMask = SOC_CMCx_IRQ2_MASK(unit,cmc);
        }
#else
        irqMask = SOC_CMCx_IRQ2_MASK(unit,cmc);
#endif        
        irqStat &= irqMask;
        if (irqStat == 0) {
            goto check_type3;
        }

        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "soc_cmicm_intr unit %d: irqStat2 = 0x%x\n"),
                     unit, irqStat));

        if (soc_feature(unit, soc_feature_cmicd_v2)) {
            intr_handler = soc_cmicdv2_intr_handlers2;
        } else {
            intr_handler = soc_cmicm_intr_handlers2;
        }
        i = 0;
        
        for (; intr_handler[i].mask; i++) {
            if (irqStat & intr_handler[i].mask) {
                /* dispatch interrupt */
                LOG_INFO(BSL_LS_SOC_INTR,
                         (BSL_META_U(unit,
                                     "soc_cmicm_intr type 2 unit %d: dispatch %s\n"),
                          unit, intr_handler[i].intr_name));
                
                (*intr_handler[i].intr_fn)
                    (unit, intr_handler[i].intr_data);
                
                /* coverity[dead_error_condition] */
                if (--poll_limit == 0) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit,
                                          "soc_cmicm_intr unit %d: "
                                          "ERROR can't clear type 2 interrupt(s): "
                                          "IRQ=0x%x (disabling 0x%x)\n"),
                               unit, irqStat, intr_handler[i].mask));
#if defined(BCM_SABER2_SUPPORT)
                    if (SOC_IS_SABER2(unit)) { 
                        (void)soc_cmicm_intr3_disable(unit, intr_handler[i].mask);
                    } else {
                        (void)soc_cmicm_intr2_disable(unit, intr_handler[i].mask);
                    }
#else
                    (void)soc_cmicm_intr2_disable(unit, intr_handler[i].mask);
#endif                    
                    poll_limit = POLL_LIMIT;
                }
                break;
            }
        }
        
        /* optimization: don't go back to re-read (PCI transaction) when there is 
         * only 1 interrupt handled in IRQ1 (for now, this is true)
         */
        if (sizeof(soc_cmicm_intr_handlers1)/sizeof(intr_handler_t) == 2) {
            break;
        }
    }
check_type3:
    if (soc_feature(unit, soc_feature_cmicm_extended_interrupts) && !soc_feature(unit, soc_feature_short_cmic_error)) {
        poll_limit = POLL_LIMIT;
        /* this enable processing of IRQ3/4, the sbus slave interrupts */
        for (;;) {
            irqStat = soc_pci_read(unit, CMIC_CMCx_IRQ_STAT3_OFFSET(cmc));
            if (irqStat == 0) {
                goto check_type4;
            }
            irqMask = SOC_CMCx_IRQ3_MASK(unit,cmc);
            irqStat &= irqMask;
            if (irqStat == 0) {
                goto check_type4;
            }
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "soc_cmicm_intr unit %d: irqStat3 = 0x%x\n"),
                 unit, irqStat));

            if (soc_feature(unit, soc_feature_cmicd_v2)) {
                intr_handler = soc_cmicdv2_intr_handlers3;
            } else {
                intr_handler = soc_cmicm_intr_handlers3;
            }
            i = 0;
            
            for (; intr_handler[i].mask; i++) {
                if (irqStat & intr_handler[i].mask) {
                    
                    /* dispatch interrupt */
                    LOG_INFO(BSL_LS_SOC_INTR,
                             (BSL_META_U(unit,
                                         "soc_cmicm_intr type 3 unit %d: dispatch %s\n"),
                              unit, intr_handler[i].intr_name));
                    
                    (*intr_handler[i].intr_fn)
                    (unit, intr_handler[i].intr_data);
                    
                    if (--poll_limit == 0) {
                        LOG_ERROR(BSL_LS_SOC_COMMON,
                                  (BSL_META_U(unit,
                                              "soc_cmicm_intr unit %d: "
                                              "ERROR can't clear type 3 interrupt(s): "
                                              "IRQ=0x%x (disabling 0x%x)\n"),
                                   unit, irqStat, intr_handler[i].mask));
                        (void)soc_cmicm_intr3_disable(unit, intr_handler[i].mask);
                        poll_limit = POLL_LIMIT;
                    }
                    
                    /* sbus slave interrupt is per block, assuming that
                     * handler will only clear interrupt for its own block
                     * reduce PCI transaction with this assumption
                     */
                    /* break; */
                }
            }
        }

check_type4:
        poll_limit = POLL_LIMIT;
        for (;;) {
            irqStat = soc_pci_read(unit, CMIC_CMCx_IRQ_STAT4_OFFSET(cmc));
            if (irqStat == 0) {
                goto check_type5;
            }
            irqMask = SOC_CMCx_IRQ4_MASK(unit,cmc);
            irqStat &= irqMask;
            if (irqStat == 0) {
                goto check_type5;
            }

            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "soc_cmicm_intr unit %d: irqStat4 = 0x%x\n"),
                         unit, irqStat));

            if (soc_feature(unit, soc_feature_cmicd_v2)) {
                intr_handler = soc_cmicdv2_intr_handlers4;
            } else {
                intr_handler = soc_cmicm_intr_handlers4;
            }
            i = 0;
            
            for (; intr_handler[i].mask; i++) {
                if (irqStat & intr_handler[i].mask) {
                    
                    /* dispatch interrupt */
                    LOG_INFO(BSL_LS_SOC_INTR,
                             (BSL_META_U(unit,
                                         "soc_cmicm_intr type 4 unit %d: dispatch %s\n"),
                              unit, intr_handler[i].intr_name));
                    
                    (*intr_handler[i].intr_fn)
                    (unit, intr_handler[i].intr_data);
                    
                    if (--poll_limit == 0) {
                        LOG_ERROR(BSL_LS_SOC_COMMON,
                                  (BSL_META_U(unit,
                                              "soc_cmicm_intr unit %d: "
                                              "ERROR can't clear type 4 interrupt(s): "
                                              "IRQ=0x%x (disabling 0x%x)\n"),
                                   unit, irqStat, intr_handler[i].mask));
                        (void)soc_cmicm_intr4_disable(unit, intr_handler[i].mask);
                        poll_limit = POLL_LIMIT;
                    }
                    
                    /* sbus slave interrupt is per block, assuming that
                     * handler will only clear interrupt for its own block
                     * reduce PCI transaction with this assumption
                     */
                    /* break; */
                }
            }
        }

check_type5:
        poll_limit = POLL_LIMIT;
        if (!soc_feature(unit, soc_feature_cmicd_v2) &&
            !soc_feature(unit, soc_feature_cmicd_v4))
            goto check_type6;

        for (;;) {
            irqStat = soc_pci_read(unit, CMIC_CMCx_IRQ_STAT5_OFFSET(cmc));
            if (irqStat == 0) {
                goto check_type6;
            }
            irqMask = SOC_CMCx_IRQ5_MASK(unit,cmc);
            irqStat &= irqMask;
            if (irqStat == 0) {
                goto check_type6;
            }

            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "soc_cmicm_intr unit %d: irqStat5 = 0x%x\n"),
                         unit, irqStat));

            if (soc_feature(unit, soc_feature_cmicd_v2)) {
                intr_handler = soc_cmicdv2_intr_handlers5;
            } else {
                intr_handler = soc_cmicm_intr_handlers5;
            }
            i = 0;
            
            for (; intr_handler[i].mask; i++) {
                if (irqStat & intr_handler[i].mask) {
                    
                    /* dispatch interrupt */
                    LOG_INFO(BSL_LS_SOC_INTR,
                             (BSL_META_U(unit,
                                         "soc_cmicm_intr type 5 unit %d: dispatch %s\n"),
                              unit, intr_handler[i].intr_name));
                    
                    (*intr_handler[i].intr_fn)
                    (unit, intr_handler[i].intr_data);
                    
                    if (--poll_limit == 0) {
                        LOG_ERROR(BSL_LS_SOC_COMMON,
                                  (BSL_META_U(unit,
                                              "soc_cmicm_intr unit %d: "
                                              "ERROR can't clear type 5 interrupt(s): "
                                              "IRQ=0x%x (disabling 0x%x)\n"),
                                   unit, irqStat, intr_handler[i].mask));
                        (void)soc_cmicm_intr5_disable(unit, intr_handler[i].mask);
                        poll_limit = POLL_LIMIT;
                    }
                    
                    /* sbus slave interrupt is per block, assuming that
                     * handler will only clear interrupt for its own block
                     * reduce PCI transaction with this assumption
                     */
                    /* break; */
                }
            }
        }

check_type6:
        poll_limit = POLL_LIMIT;
        if (!soc_feature(unit, soc_feature_cmicd_v4))
            goto check_arm_type0;

        for (;;) {
            irqStat = soc_pci_read(unit, CMIC_CMCx_IRQ_STAT6_OFFSET(cmc));
            if (irqStat == 0) {
                goto check_arm_type0;
            }
            irqMask = SOC_CMCx_IRQ6_MASK(unit,cmc);
            irqStat &= irqMask;
            if (irqStat == 0) {
                goto check_arm_type0;
            }

            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "soc_cmicm_intr unit %d: irqStat6 = 0x%x\n"),
                         unit, irqStat));

            intr_handler = soc_cmicdv4_intr_handlers6;
            i = 0;

            for (; intr_handler[i].mask; i++) {
                if (irqStat & intr_handler[i].mask) {

                    /* dispatch interrupt */
                    LOG_INFO(BSL_LS_SOC_INTR,
                             (BSL_META_U(unit,
                                         "soc_cmicm_intr type 6 unit %d: dispatch %s\n"),
                              unit, intr_handler[i].intr_name));

                    (*intr_handler[i].intr_fn)
                    (unit, intr_handler[i].intr_data);

                    if (--poll_limit == 0) {
                        LOG_ERROR(BSL_LS_SOC_COMMON,
                                  (BSL_META_U(unit,
                                              "soc_cmicm_intr unit %d: "
                                              "ERROR can't clear type 6 interrupt(s): "
                                              "IRQ=0x%x (disabling 0x%x)\n"),
                                   unit, irqStat, intr_handler[i].mask));
                        (void)soc_cmicm_intr6_disable(unit, intr_handler[i].mask);
                        poll_limit = POLL_LIMIT;
                    }

                    /* sbus slave interrupt is per block, assuming that
                     * handler will only clear interrupt for its own block
                     * reduce PCI transaction with this assumption
                     */
                    /* break; */
                }
            }
        }
    }

check_arm_type0:
    poll_limit = POLL_LIMIT;
    if (soc_feature(unit, soc_feature_cmicm_multi_dma_cmc) &&
        (SOC_CMCS_NUM(unit) > SOC_PCI_CMCS_NUM(unit))) {
        intr_handler = soc_cmicm_intr_handlers0_arm_cmc;
        poll_limit = POLL_LIMIT;
        
        for (arm = 0; arm < (SOC_CMCS_NUM(unit) - SOC_PCI_CMCS_NUM(unit)); arm++) {
            /* this enable processing of ARM_CMC0/ARM_CMC1 IRQ0 */
            cmc = SOC_ARM_CMC(unit, arm);
        
            for (;;) {
                irqStat = soc_pci_read(unit, CMIC_CMCx_IRQ_STAT0_OFFSET(cmc));
                if (irqStat == 0) {
                    break; /* move to next arm cmc */
                }
                irqMask = SOC_CMCx_IRQ0_MASK(unit,cmc);
                irqStat &= irqMask;
                if (irqStat == 0) {
                    break; /* move to next arm cmc */
                }
                
                i = 0;
                for (; intr_handler[i].mask; i++) {
                    if (irqStat & intr_handler[i].mask) {
                        /* dispatch interrupt */
                        LOG_INFO(BSL_LS_SOC_INTR,
                                 (BSL_META_U(unit,
                                             "soc_cmicm_intr CMC %d type 0 unit %d: dispatch %s\n"),
                                  unit, cmc, intr_handler[i].intr_name));
                        
                        (*intr_handler[i].intr_fn)
                            (unit, ((cmc<<2) + intr_handler[i].intr_data));
                        
                        if (--poll_limit == 0) {
                            LOG_ERROR(BSL_LS_SOC_COMMON,
                                      (BSL_META_U(unit,
                                                  "soc_cmicm_intr unit %d cmc %0d: "
                                                  "ERROR can't clear arm type 0 interrupt(s): "
                                                  "IRQ=0x%x (disabling 0x%x)\n"),
                                       unit, cmc, irqStat, intr_handler[i].mask));
                            (void)soc_cmicm_cmcx_intr0_disable(unit, cmc,
                                                               intr_handler[i].mask);
                            poll_limit = POLL_LIMIT;
                        }
                        break;
                    }
                }
            }
        }
    }
#else
    COMPILER_REFERENCE(arm);
#endif /* defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT) */

    if (soc_feature(unit, soc_feature_short_cmic_error)) {
        /* Using sal_dpc since there are schan reads in this function
         * and schan read cant be done from interrupt context.
         * the function soc_cmn_error will be excecuted only after this 
         * function will end.
         */
        sal_dpc(soc_cmn_error, INT_TO_PTR(unit), 0, 0, 0, 0);
    }

    if (soc->soc_flags & SOC_F_POLLED) {
#ifdef SAL_SPL_LOCK_ON_IRQ
        sal_spl(s);
#endif
        return;
    }

#ifndef SAL_SPL_LOCK_ON_IRQ
    s = sal_splhi();
#endif

    for (cmc = 0; cmc < SOC_PCI_CMCS_NUM(unit); cmc++) {
        if (SOC_CMCx_IRQ0_MASK(unit, cmc)) {
            IRQ_MASK0_SET(unit, CMIC_CMCx_PCIE_IRQ_MASK0_OFFSET(cmc), SOC_CMCx_IRQ0_MASK(unit, cmc));
        }
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)  || defined(BCM_DFE_SUPPORT)
        IRQ_MASKx_SET(unit, CMIC_CMCx_PCIE_IRQ_MASK1_OFFSET(cmc), SOC_CMCx_IRQ1_MASK(unit, cmc));

#if defined(BCM_SABER2_SUPPORT)
        if (SOC_IS_SABER2(unit)) {
            IRQ_MASKx_SET(unit, CMIC_CMCx_PCIE_IRQ_MASK3_OFFSET(cmc), SOC_CMCx_IRQ3_MASK(unit, cmc));
        } else
#endif
        {
            IRQ_MASKx_SET(unit, CMIC_CMCx_PCIE_IRQ_MASK2_OFFSET(cmc), SOC_CMCx_IRQ2_MASK(unit, cmc));
        }

        if (soc_feature(unit, soc_feature_extended_cmic_error)) {
            IRQ_MASKx_SET(unit, CMIC_CMCx_PCIE_IRQ_MASK3_OFFSET(cmc), SOC_CMCx_IRQ3_MASK(unit, cmc));
            IRQ_MASKx_SET(unit, CMIC_CMCx_PCIE_IRQ_MASK4_OFFSET(cmc), SOC_CMCx_IRQ4_MASK(unit, cmc));
            if (soc_feature(unit, soc_feature_cmicm_extended_interrupts)) {
                IRQ_MASKx_SET(unit, CMIC_CMCx_PCIE_IRQ_MASK5_OFFSET(cmc), SOC_CMCx_IRQ5_MASK(unit, cmc));
                IRQ_MASKx_SET(unit, CMIC_CMCx_PCIE_IRQ_MASK6_OFFSET(cmc), SOC_CMCx_IRQ6_MASK(unit, cmc));
            }
        }
        if (soc_feature(unit, soc_feature_cmicd_v2)) {
            IRQ_MASKx_SET(unit, CMIC_CMCx_PCIE_IRQ_MASK3_OFFSET(cmc), SOC_CMCx_IRQ3_MASK(unit, cmc));
            IRQ_MASKx_SET(unit, CMIC_CMCx_PCIE_IRQ_MASK4_OFFSET(cmc), SOC_CMCx_IRQ4_MASK(unit, cmc));
        }
        if (soc_feature(unit, soc_feature_short_cmic_error)) {
            /* When working with this feature other interrupts handling in this reg are done from soc_cmn_error()
             * only cmic parity error (bit 0) is handled by this function.
             */
            IRQ_MASKx_SET(unit, CMIC_CMCx_PCIE_IRQ_MASK2_OFFSET(cmc), SOC_CMCx_IRQ2_MASK(unit, cmc) & 0x1);
        }
#endif /* defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)  || defined(BCM_DFE_SUPPORT) */
    }
 
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
    /* May need to restore the masks in ARM's CMCs as well */
    if (soc_feature(unit, soc_feature_cmicm_multi_dma_cmc) &&
        (SOC_CMCS_NUM(unit) > SOC_PCI_CMCS_NUM(unit))) {
        int arm;

        for (arm = 0; arm < (SOC_CMCS_NUM(unit) - SOC_PCI_CMCS_NUM(unit)); arm++) {
            int arm_cmc = SOC_ARM_CMC(unit, arm);

            soc_pci_write(unit, CMIC_CMCx_PCIE_IRQ_MASK0_OFFSET(arm_cmc), SOC_CMCx_IRQ0_MASK(unit, arm_cmc));
        }
    }
#endif

    sal_spl(s);
}

#ifdef SEPARATE_PKTDMA_INTR_HANDLER
void
soc_cmicm_pktdma_intr(void *_unit)
{
    soc_control_t *soc;
    uint32 irqStat, irqMask;
    int unit = PTR_TO_INT(_unit);
    int cmc = 0, i = 0;
    int poll_limit = 102400; /* 100K */
    intr_handler_t *intr_handler = soc_cmicm_pktdma_intr_handlers;
    uint32 pktdma_status = 0x7800ff00;

#ifdef SAL_SPL_LOCK_ON_IRQ
    int s;
    s = sal_splhi();
#endif

    soc = SOC_CONTROL(unit);

    /*
     * Our handler is permanently registered in soc_probe().  If our
     * unit is not attached yet, it could not have generated this
     * interrupt.  The interrupt line must be shared by multiple PCI
     * cards.  Simply ignore the interrupt and let another handler
     * process it.
     */
    if (soc == NULL || (soc->soc_flags & SOC_F_BUSY) ||
        !(soc->soc_flags & SOC_F_ATTACHED)) {
#ifdef SAL_SPL_LOCK_ON_IRQ
        sal_spl(s);
#endif
        return;
    }

    cmc = SOC_PCI_CMC(unit);
    /* Update count */
    soc->stat.intr++;

    if (soc_feature(unit, soc_feature_sbusdma)) {
        intr_handler = soc_cmicm_pktdma_intr_handlers0;
    }
    /*
     * Read IRQ Status and IRQ Mask and AND to determine active ints.
     * These are re-read each time since either can be changed by ISRs.
     */
    for (;;) {
        if (soc_feature(unit, soc_feature_cmicm_multi_dma_cmc) &&
            (SOC_PCI_CMCS_NUM(unit) > 1)) {
            for (i = soc->next_int0_cmc; i < soc->next_int0_cmc + SOC_PCI_CMCS_NUM(unit); i++) {
                cmc = i % SOC_PCI_CMCS_NUM(unit);
                irqStat = soc_pci_read(unit, CMIC_CMCx_IRQ_STAT0_OFFSET(cmc));
                /** Only handle pktdma interrupts */
                irqStat &= pktdma_status;
                if (irqStat != 0) {
                    irqMask = SOC_CMCx_IRQ0_MASK(unit, cmc);
                    irqStat &= irqMask;
                    if (irqStat != 0) {
                        goto detected_irq0;
                    }
                }
                soc->next_int0_cmc = (cmc + 1) % SOC_PCI_CMCS_NUM(unit);
            }
            /** re-assign cmc to origin */
            cmc = SOC_PCI_CMC(unit);
            goto exit;
        } else {
            cmc = SOC_PCI_CMC(unit);
            irqStat = soc_pci_read(unit, CMIC_CMCx_IRQ_STAT0_OFFSET(cmc));
            /** Only handle pktdma interrupts */
            irqStat &= pktdma_status;
            if (irqStat == 0) {
                goto exit;  /* No pending Interrupts */
            }
            irqMask = SOC_CMCx_IRQ0_MASK(unit,cmc);
            irqStat &= irqMask;
            if (irqStat == 0) {
                goto exit;
            }
        }

detected_irq0:
        i = 0;
        /*
        * We may have received an interrupt before all data has been
        * posted from the device or intermediate bridge.
        * The PCI specification requires that we read a device register
        * to make sure pending data is flushed.
        */
        soc_pci_read(unit, CMIC_CMCx_SCHAN_CTRL_OFFSET(cmc));
        soc_pci_read(unit, CMIC_CMCx_PCIE_IRQ_MASK0_OFFSET(cmc));

        for (; intr_handler[i].mask; i++) {
            if (irqStat & intr_handler[i].mask) {

            /* dispatch interrupt */
            LOG_INFO(BSL_LS_SOC_INTR,
                     (BSL_META_U(unit,
                                 "soc_cmicm_intr type 0 unit %d: dispatch %s\n"),
                      unit, intr_handler[i].intr_name));

            (*intr_handler[i].intr_fn)
                (unit, cmc * N_DMA_CHAN + intr_handler[i].intr_data);

            /*
             * Prevent infinite loop in interrupt handler by
             * disabling the offending interrupt(s).
             */
            /* coverity[dead_error_condition] */
            if (--poll_limit == 0) {
                return;
            }
            /*
             * Go back and re-read IRQ status.  Start processing
             * from scratch since handler may clear more than one
             * bit. We don't leave the ISR until all of the bits
             * have been cleared and their handlers called.
             */
            break;
            }
        }
    }

exit:

    if (soc->soc_flags & SOC_F_POLLED) {
#ifdef SAL_SPL_LOCK_ON_IRQ
        sal_spl(s);
#endif
        return;
    }

    for (cmc = 0; cmc < SOC_PCI_CMCS_NUM(unit); cmc++)
    {
        IRQ_MASK0_SET(unit, CMIC_CMCx_PCIE_IRQ_MASK0_OFFSET(cmc), SOC_CMCx_IRQ0_MASK(unit, cmc));
        IRQ_MASKx_SET(unit, CMIC_CMCx_PCIE_IRQ_MASK1_OFFSET(cmc), SOC_CMCx_IRQ1_MASK(unit, cmc));
        IRQ_MASKx_SET(unit, CMIC_CMCx_PCIE_IRQ_MASK2_OFFSET(cmc), SOC_CMCx_IRQ2_MASK(unit, cmc));
    }
#ifdef SAL_SPL_LOCK_ON_IRQ
    sal_spl(s);
#endif
}
#endif


#ifdef INCLUDE_RCPU
STATIC void
soc_cmicm_rcpu_intr_miim_op(int unit, uint32 ignored)
{
    soc_control_t *soc = SOC_CONTROL(unit);

    COMPILER_REFERENCE(ignored);

    /* Clr Read & Write Stat */
    soc_pci_write(unit, CMIC_RPE_MIIM_CTRL_OFFSET, 0);

    soc->stat.intr_mii++;

    if (soc->miimIntr) {
        sal_sem_give(soc->miimIntr);
    }
}
/*
 * Enable (unmask) or disable (mask) a set of CMIC RPE interrupts.  These
 * routines should be used instead of manipulating CMIC_RPE_RCPU_IRQ_MASK
 * directly, since a read-modify-write is required.  The return value is
 * the previous mask (can pass mask of 0 to just get the current mask).
 */

STATIC uint32
soc_cmicm_rcpu_intrx_enable(int unit, uint32 offset, uint32 mask, uint32 *mask_reg)
{
    uint32 oldMask;
    uint32 newMask;
    int s;
    s = sal_splhi();


    oldMask = *mask_reg;
    *mask_reg |= mask;
    newMask = *mask_reg;
    /* In polled mode, the hardware IRQ mask is always zero */
    if (SOC_CONTROL(unit)->soc_flags & SOC_F_POLLED) {
        newMask = 0;
    }
    LOG_INFO(BSL_LS_SOC_INTR,
             (BSL_META_U(unit,
                         "soc_cmicm_rcpu_intrx_enable unit %d: mask 0x%8x\n"), unit, mask));

    soc_pci_write(unit, offset, newMask);

    sal_spl(s);
    return oldMask;
}

STATIC uint32
soc_cmicm_rcpu_intrx_disable(int unit, uint32 offset, uint32 mask, uint32 *mask_reg)
{
    uint32 oldMask;
    uint32 newMask;
    int s;

    s = sal_splhi();
    oldMask = *mask_reg;
    *mask_reg &= ~mask;
    newMask = *mask_reg;

    /* In polled mode, the hardware IRQ mask is always zero */
    if (SOC_CONTROL(unit)->soc_flags & SOC_F_POLLED) {
        newMask = 0;
    }
    LOG_INFO(BSL_LS_SOC_INTR,
             (BSL_META_U(unit,
                         "soc_cmicm_rcpu_intrx_disable unit %d: mask 0x%8x\n"), unit, mask));
    soc_pci_write(unit, offset, newMask);

    sal_spl(s);
    return oldMask;
}

uint32
soc_cmicm_rcpu_intr0_enable(int unit, uint32 mask)
{
    return soc_cmicm_rcpu_intrx_enable(unit, CMIC_RPE_RCPU_IRQ_MASK0_OFFSET,
                                       mask, &SOC_RCPU_IRQ0_MASK(unit));
}

uint32
soc_cmicm_rcpu_intr0_disable(int unit, uint32 mask)
{
    return soc_cmicm_rcpu_intrx_disable(unit, CMIC_RPE_RCPU_IRQ_MASK0_OFFSET,
                                       mask, &SOC_RCPU_IRQ0_MASK(unit));
}

uint32
soc_cmicm_rcpu_intr1_enable(int unit, uint32 mask)
{
    return soc_cmicm_rcpu_intrx_enable(unit, CMIC_RPE_RCPU_IRQ_MASK1_OFFSET,
                                       mask, &SOC_RCPU_IRQ1_MASK(unit));
}

uint32
soc_cmicm_rcpu_intr1_disable(int unit, uint32 mask)
{
    return soc_cmicm_rcpu_intrx_disable(unit, CMIC_RPE_RCPU_IRQ_MASK1_OFFSET,
                                       mask, &SOC_RCPU_IRQ1_MASK(unit));
}

uint32
soc_cmicm_rcpu_intr2_enable(int unit, uint32 mask)
{
    return soc_cmicm_rcpu_intrx_enable(unit, CMIC_RPE_RCPU_IRQ_MASK2_OFFSET,
                                       mask, &SOC_RCPU_IRQ2_MASK(unit));
}

uint32
soc_cmicm_rcpu_intr2_disable(int unit, uint32 mask)
{
    return soc_cmicm_rcpu_intrx_disable(unit, CMIC_RPE_RCPU_IRQ_MASK2_OFFSET,
                                       mask, &SOC_RCPU_IRQ2_MASK(unit));
}

uint32
soc_cmicm_rcpu_intr3_enable(int unit, uint32 mask)
{
    return soc_cmicm_rcpu_intrx_enable(unit, CMIC_RPE_RCPU_IRQ_MASK3_OFFSET,
                                       mask, &SOC_RCPU_IRQ3_MASK(unit));
}

uint32
soc_cmicm_rcpu_intr3_disable(int unit, uint32 mask)
{
    return soc_cmicm_rcpu_intrx_disable(unit, CMIC_RPE_RCPU_IRQ_MASK3_OFFSET,
                                       mask, &SOC_RCPU_IRQ3_MASK(unit));
}

uint32
soc_cmicm_rcpu_intr4_enable(int unit, uint32 mask)
{
    return soc_cmicm_rcpu_intrx_enable(unit, CMIC_RPE_RCPU_IRQ_MASK4_OFFSET,
                                       mask, &SOC_RCPU_IRQ4_MASK(unit));
}

uint32
soc_cmicm_rcpu_intr4_disable(int unit, uint32 mask)
{
    return soc_cmicm_rcpu_intrx_disable(unit, CMIC_RPE_RCPU_IRQ_MASK4_OFFSET,
                                       mask, &SOC_RCPU_IRQ4_MASK(unit));
}

uint32
soc_cmicm_rcpu_cmc0_intr0_enable(int unit, uint32 mask)
{
    return soc_cmicm_rcpu_intrx_enable(unit, CMIC_CMC0_RCPU_IRQ_MASK0_OFFSET,
                                       mask, &SOC_RCPU_CMC0_IRQ0_MASK(unit));
}

uint32
soc_cmicm_rcpu_cmc0_intr0_disable(int unit, uint32 mask)
{
    return soc_cmicm_rcpu_intrx_disable(unit, CMIC_CMC0_RCPU_IRQ_MASK0_OFFSET,
                                       mask, &SOC_RCPU_CMC0_IRQ0_MASK(unit));
}

uint32
soc_cmicm_rcpu_cmc1_intr0_enable(int unit, uint32 mask)
{
    return soc_cmicm_rcpu_intrx_enable(unit, CMIC_CMC1_RCPU_IRQ_MASK0_OFFSET,
                                       mask, &SOC_RCPU_CMC1_IRQ0_MASK(unit));
}

uint32
soc_cmicm_rcpu_cmc1_intr0_disable(int unit, uint32 mask)
{
    return soc_cmicm_rcpu_intrx_disable(unit, CMIC_CMC1_RCPU_IRQ_MASK0_OFFSET,
                                       mask, &SOC_RCPU_CMC1_IRQ0_MASK(unit));
}

uint32
soc_cmicm_rcpu_cmc2_intr0_enable(int unit, uint32 mask)
{
    return soc_cmicm_rcpu_intrx_enable(unit, CMIC_CMC2_RCPU_IRQ_MASK0_OFFSET,
                                       mask, &SOC_RCPU_CMC2_IRQ0_MASK(unit));
}

uint32
soc_cmicm_rcpu_cmc2_intr0_disable(int unit, uint32 mask)
{
    return soc_cmicm_rcpu_intrx_disable(unit, CMIC_CMC2_RCPU_IRQ_MASK0_OFFSET,
                                       mask, &SOC_RCPU_CMC2_IRQ0_MASK(unit));
}

void
soc_cmicm_rcpu_intr(int unit, soc_rcpu_intr_packet_t *intr_pkt)
{
    uint32 irqStat, irqMask;
    int i = 0;
    intr_handler_t *intr_handler = soc_cmicm_rcpu_intr_handlers0;
    soc_control_t *soc;

#ifdef SAL_SPL_LOCK_ON_IRQ
    int s;

    s = sal_splhi();
#endif

    soc = SOC_CONTROL(unit);

    /*
     * Our handler is permanently registered in soc_probe().  If our
     * unit is not attached yet, it could not have generated this
     * interrupt.  The interrupt line must be shared by multiple PCI
     * cards.  Simply ignore the interrupt and let another handler
     * process it.
     */
    if (soc == NULL || (soc->soc_flags & SOC_F_BUSY) ||
        !(soc->soc_flags & SOC_F_ATTACHED)) {
#ifdef SAL_SPL_LOCK_ON_IRQ
        sal_spl(s);
#endif
        return;
    }

    soc->stat.intr++; /* Update count */

    /*
     * Read IRQ Status and IRQ Mask and AND to determine active ints.
     * These are re-read each time since either can be changed by ISRs.
     */
    
    irqStat = intr_pkt->rcpu_irq0_stat;
    if (irqStat == 0) {
        goto check_rcpu_type1;  /* No pending Interrupts */
    }
    irqMask = intr_pkt->rcpu_irq0_mask;
    irqStat &= irqMask;
    if (irqStat == 0) {
        goto check_rcpu_type1;
    }

    i = 0;

    for (; intr_handler[i].mask; i++) {
        if (irqStat & intr_handler[i].mask) {

            /* dispatch interrupt */
            if (LOG_CHECK(BSL_LS_SOC_RCPU | BSL_INFO) &&
                LOG_CHECK(BSL_LS_SOC_INTR | BSL_INFO)) {
                LOG_CLI((BSL_META_U(unit,
                                    "soc_cmicm_rcpu_intr type 0 unit %d: dispatch %s\n"),
                         unit, intr_handler[i].intr_name));
            }

            (*intr_handler[i].intr_fn)(unit, intr_handler[i].intr_data);
        }
    }

check_rcpu_type1:

#if defined(BCM_ESW_SUPPORT)
    irqStat = intr_pkt->rcpu_irq1_stat;
    if (irqStat == 0) {
        goto check_rcpu_type2;  /* No pending Interrupts */
    }
    irqMask = intr_pkt->rcpu_irq1_mask;
    irqStat &= irqMask;
    if (irqStat == 0) {
        goto check_rcpu_type2;
    }

    intr_handler = soc_cmicm_intr_handlers1;
    i = 0;

    for (; intr_handler[i].mask; i++) {
        if (irqStat & intr_handler[i].mask) {
    
            /* dispatch interrupt, verbose only */
            if (LOG_CHECK(BSL_LS_SOC_RCPU | BSL_INFO) &&
                LOG_CHECK(BSL_LS_SOC_INTR | BSL_INFO)) {
                LOG_CLI((BSL_META_U(unit,
                                    "soc_cmicm_rcpu_intr type 1 unit %d: dispatch %s\n"),
                         unit, intr_handler[i].intr_name));
            }
        }
    }

check_rcpu_type2:

    irqStat = intr_pkt->rcpu_irq2_stat;
    if (irqStat == 0) {
        goto check_rcpu_type3;  /* No pending Interrupts */
    }
    irqMask = intr_pkt->rcpu_irq2_mask;
    irqStat &= irqMask;
    if (irqStat == 0) {
        goto check_rcpu_type2;
    }

    intr_handler = soc_cmicm_intr_handlers2;
    i = 0;

    for (; intr_handler[i].mask; i++) {
        if (irqStat & intr_handler[i].mask) {
    
            /* dispatch interrupt, verbose only */
            if (LOG_CHECK(BSL_LS_SOC_RCPU | BSL_INFO) &&
                LOG_CHECK(BSL_LS_SOC_INTR | BSL_INFO)) {
                LOG_CLI((BSL_META_U(unit,
                                    "soc_cmicm_rcpu_intr type 2 unit %d: dispatch %s\n"),
                         unit, intr_handler[i].intr_name));
            }
        }
    }

check_rcpu_type3:

    irqStat = intr_pkt->rcpu_irq3_stat;
    if (irqStat == 0) {
        goto check_rcpu_type4;  /* No pending Interrupts */
    }
    irqMask = intr_pkt->rcpu_irq3_mask;
    irqStat &= irqMask;
    if (irqStat == 0) {
        goto check_rcpu_type4;
    }

    intr_handler = soc_cmicm_intr_handlers3;
    i = 0;

    for (; intr_handler[i].mask; i++) {
        if (irqStat & intr_handler[i].mask) {
    
            /* dispatch interrupt, verbose only */
            if (LOG_CHECK(BSL_LS_SOC_RCPU | BSL_INFO) &&
                LOG_CHECK(BSL_LS_SOC_INTR | BSL_INFO)) {
                LOG_CLI((BSL_META_U(unit,
                                    "soc_cmicm_rcpu_intr type 3 unit %d: dispatch %s\n"),
                         unit, intr_handler[i].intr_name));
            }
        }
    }

check_rcpu_type4:
    irqStat = intr_pkt->rcpu_irq4_stat;
    if (irqStat == 0) {
        goto check_rcpu_cmc0;  /* No pending Interrupts */
    }
    irqMask = intr_pkt->rcpu_irq4_mask;
    irqStat &= irqMask;
    if (irqStat == 0) {
        goto check_rcpu_cmc0;
    }

    if (soc_feature(unit, soc_feature_cmicd_v2)) {
        intr_handler = soc_cmicdv2_intr_handlers4;
    } else {
        intr_handler = soc_cmicm_intr_handlers4;
    }
    i = 0;

    for (; intr_handler[i].mask; i++) {
        if (irqStat & intr_handler[i].mask) {
    
            /* dispatch interrupt, verbose only */
            if (LOG_CHECK(BSL_LS_SOC_RCPU | BSL_INFO) &&
                LOG_CHECK(BSL_LS_SOC_INTR | BSL_INFO)) {
                LOG_CLI((BSL_META_U(unit,
                                    "soc_cmicm_rcpu_intr type 4 unit %d: dispatch %s\n"),
                         unit, intr_handler[i].intr_name));
            }
        }
    }

check_rcpu_cmc0:

#endif /* defined(BCM_ESW_SUPPORT) */

    irqStat = intr_pkt->cmc0_irq_stat;
    if (irqStat == 0) {
        goto rcpu_intr_exit;  /* No pending Interrupts */
    }
    irqMask = intr_pkt->cmc0_rcpu_irq_mask;
    irqStat &= irqMask;
    if (irqStat == 0) {
        goto rcpu_intr_exit;
    }

    if (soc_feature(unit, soc_feature_sbusdma)) {
        intr_handler = soc_cmicm_intr_handlers0;
    } else {
        intr_handler = soc_cmicm_intr_handlers;
    }

    i = 0;

    for (; intr_handler[i].mask; i++) {
        if (irqStat & intr_handler[i].mask) {

            if (LOG_CHECK(BSL_LS_SOC_RCPU | BSL_INFO) &&
                LOG_CHECK(BSL_LS_SOC_INTR | BSL_INFO)) {
                /* dispatch interrupt, verbose only */
                LOG_CLI((BSL_META_U(unit,
                                    "soc_cmicm_rcpu_cmc0_intr type 0 unit %d: dispatch %s\n"),
                         unit, intr_handler[i].intr_name));
            }
        }
    }

rcpu_intr_exit:

#ifdef SAL_SPL_LOCK_ON_IRQ
    sal_spl(s);
#endif
    return;
}

#endif /* INCLUDE_RCPU */

/*
 * Initialize iProc based iHost irq offset
 */
void
soc_cmicm_ihost_irq_offset_set(int unit)
{
    soc_cmicm_host_irq_offset[unit] = HOST_IRQ_MASK_OFFSET_DIFF;
}

/*
 * Initialize iProc based iHost irq offset
 */
void
soc_cmicm_ihost_irq_offset_reset(int unit)
{
    soc_cmicm_host_irq_offset[unit] = 0;
}

#endif /* CMICM Support */
