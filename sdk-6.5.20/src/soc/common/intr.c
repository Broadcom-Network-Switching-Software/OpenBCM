/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC Interrupt Handlers
 *
 * NOTE: These handlers are called from an interrupt context, so their
 *       actions are restricted accordingly.
 */

#include <shared/bsl.h>

#include <sal/core/libc.h>
#include <shared/alloc.h>
#include <shared/util.h>
#include <sal/core/spl.h>
#include <sal/core/sync.h>
#include <sal/core/dpc.h>
#include <sal/core/time.h>
#include <sal/types.h>

#include <soc/drv.h>
#include <soc/dma.h>
#include <soc/i2c.h>
#include <soc/intr.h>

#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif

#ifdef BCM_CMICX_SUPPORT
#include <soc/intr_cmicx.h>
#endif


#ifdef BCM_FIREBOLT_SUPPORT
#include <soc/firebolt.h>
#endif /* BCM_FIREBOLT_SUPPORT */

#ifdef BCM_BRADLEY_SUPPORT
#include <soc/bradley.h>
#endif /* BCM_BRADLEY_SUPPORT */

#ifdef BCM_TRIUMPH_SUPPORT
#include <soc/triumph.h>
#endif /* BCM_TRIUMPH_SUPPORT */

#ifdef BCM_TRIUMPH2_SUPPORT
#include <soc/triumph2.h>
#endif /* BCM_TRIUMPH2_SUPPORT */

#ifdef BCM_TRIDENT_SUPPORT
#include <soc/trident.h>
#endif /* BCM_TRIDENT_SUPPORT */

#ifdef BCM_ENDURO_SUPPORT
#include <soc/enduro.h>
#endif /* BCM_ENDURO_SUPPORT */

#ifdef BCM_HURRICANE_SUPPORT
#include <soc/hurricane.h>
#endif /* BCM_HURRICANE_SUPPORT */

#ifdef BCM_DFE_SUPPORT
#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dfe/cmn/dfe_interrupt.h>
#include <soc/dfe/cmn/dfe_warm_boot.h>
#endif
#ifdef BCM_PETRA_SUPPORT
#include <shared/swstate/access/sw_state_access.h>
#include <soc/dpp/ARAD/arad_interrupts.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#endif
#ifdef BCM_JERICHO_SUPPORT
#include <soc/dpp/JER/jer_intr.h>
#endif
#ifdef BCM_JERICHO_SUPPORT
#include <soc/dpp/QAX/qax_intr.h>
#endif
#ifdef BCM_JERICHO_PLUS_SUPPORT
#include <soc/dpp/JERP/jerp_intr.h>
#endif
#ifdef BCM_QUX_SUPPORT
#include <soc/dpp/QUX/qux_intr.h>
#endif
#ifdef BCM_PETRA_SUPPORT
#include <soc/dpp/mbcm.h>
#endif
#if defined(BCM_JERICHO_SUPPORT) || defined(BCM_88950_SUPPORT) 
#include <soc/dcmn/dcmn_intr_handler.h>
#endif
#ifdef BCM_DNXF_SUPPORT
#include <soc/dnxf/ramon/ramon_intr.h>
/*dnxf sw state*/
#include <soc/dnxf/swstate/auto_generated/access/dnxf_access.h>
#endif
#ifdef BCM_DNX_SUPPORT
#include <soc/dnxc/intr.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_intr.h>
#include <soc/dnx/swstate/auto_generated/access/interrupt_access.h>
#include <soc/dnx/intr/dnx_intr.h>
#endif
#ifdef BCM_CMICX_SUPPORT
#include <soc/iproc.h>
#endif
#ifdef INCLUDE_KNET
#include <soc/knet.h>
#define IRQ_MASK_SET(_u,_a,_m) soc_knet_irq_mask_set(_u,_a,_m)
#else
#define IRQ_MASK_SET(_u,_a,_m) soc_pci_write(_u,_a,_m)
#endif

#define INTR_CMN_ERROR_MAX_INTERRUPTS_SIZE    50
#define SOC_INTERRUPT_DB_PRIORITY_MECHANISM_MAX_LEVELS 300
#define FIRST_PORT_BLOCK_INTER 727

#ifdef BCM_CMICX_SUPPORT
soc_cmic_intr_op_t   _cmic_intr_op[SOC_MAX_NUM_DEVICES];
#endif

#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT) || defined(PORTMOD_SUPPORT)

/* Declare static functions for interrupt handler array */
STATIC void soc_intr_schan_done(int unit, uint32 ignored);
STATIC void soc_intr_pci_parity(int unit, uint32 ignored);
STATIC void soc_intr_pci_fatal(int unit, uint32 ignored);
STATIC void soc_intr_link_stat(int unit, uint32 ignored);
STATIC void soc_intr_gbp_full(int unit, uint32 ignored);
STATIC void soc_intr_arl_xfer(int unit, uint32 ignored);
STATIC void soc_intr_arl_cnt0(int unit, uint32 ignored);
STATIC void soc_intr_arl_drop(int unit, uint32 ignored);
STATIC void soc_intr_arl_mbuf(int unit, uint32 ignored);
STATIC void soc_intr_schan_error(int unit, uint32 ignored);
STATIC void soc_intr_i2c(int unit, uint32 ignored);
STATIC void soc_intr_miim_op(int unit, uint32 ignored);
STATIC void soc_intr_stat_dma(int unit, uint32 ignored);
STATIC void soc_intr_bit21(int unit, uint32 ignored);
STATIC void soc_intr_bit22(int unit, uint32 ignored);
STATIC void soc_intr_bit23(int unit, uint32 ignored);
#ifdef BCM_HERCULES_SUPPORT
STATIC void soc_intr_mmu_stat(int unit, uint32 ignored);
#endif
#if defined(BCM_XGS12_SWITCH_SUPPORT)
STATIC void soc_intr_arl_error(int unit, uint32 ignored);
#endif
STATIC void soc_intr_lpm_lo_parity(int unit, uint32 ignored);
STATIC void soc_intr_bit25(int unit, uint32 ignored);
STATIC void soc_intr_bit26(int unit, uint32 ignored);
STATIC void soc_intr_bit27(int unit, uint32 ignored);
STATIC void soc_intr_bit28(int unit, uint32 ignored);
STATIC void soc_intr_bit31(int unit, uint32 ignored);
STATIC void soc_intr_tdma_done(int unit, uint32 ignored);
STATIC void soc_intr_tslam_done(int unit, uint32 ignored);
STATIC void soc_intr_block(int unit, uint32 block);

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

STATIC intr_handler_t soc_intr_handlers[] = {

 /* Errors (Highest priority) [0..3] */

 { IRQ_PCI_PARITY_ERR,    soc_intr_pci_parity,    0, "PCI_PARITY_ERR"    },
 { IRQ_PCI_FATAL_ERR,    soc_intr_pci_fatal,    0, "PCI_FATAL_ERR"    },
 { IRQ_SCHAN_ERR,    soc_intr_schan_error,    0, "SCHAN_ERR"        },
 { IRQ_GBP_FULL,    soc_intr_gbp_full,    0, "GBP_FULL"        },

 /* S-Channel [4] */

 { IRQ_SCH_MSG_DONE,    soc_intr_schan_done,    0, "SCH_MSG_DONE"    },

 /* MII [5-6] */

 { IRQ_MIIM_OP_DONE,    soc_intr_miim_op,    0, "MIIM_OP_DONE"    },
 { IRQ_LINK_STAT_MOD,    soc_intr_link_stat,    0, "LINK_STAT_MOD"    },

 /* ARL messages [7-10] */

 { IRQ_ARL_MBUF,    soc_intr_arl_mbuf,    0, "ARL_MBUF"        },
 { IRQ_ARL_MBUF_DROP,    soc_intr_arl_drop,    0, "ARL_MBUF_DROP"    },
 { IRQ_ARL_DMA_CNT0,    soc_intr_arl_cnt0,    0, "ARL_DMA_CNT0"    },
 { IRQ_ARL_DMA_XFER,    soc_intr_arl_xfer,    0, "ARL_DMA_XFER"    },

 /* TDMA/TSLAM [11-12] */
 { IRQ_TDMA_DONE,    soc_intr_tdma_done,    0, "TDMA_DONE"            },
 { IRQ_TSLAM_DONE,    soc_intr_tslam_done,    0, "TSLAM_DONE"            },

 /* Packet DMA [13-20] */

 { IRQ_CHAIN_DONE(0),    soc_dma_done_chain,    0, "CH0_CHAIN_DONE"    },
 { IRQ_CHAIN_DONE(1),    soc_dma_done_chain,    1, "CH1_CHAIN_DONE"    },
 { IRQ_CHAIN_DONE(2),    soc_dma_done_chain,    2, "CH2_CHAIN_DONE"    },
 { IRQ_CHAIN_DONE(3),    soc_dma_done_chain,    3, "CH3_CHAIN_DONE"    },

 { IRQ_DESC_DONE(0),    soc_dma_done_desc,    0, "CH0_DESC_DONE"    },
 { IRQ_DESC_DONE(1),    soc_dma_done_desc,    1, "CH1_DESC_DONE"    },
 { IRQ_DESC_DONE(2),    soc_dma_done_desc,    2, "CH2_DESC_DONE"    },
 { IRQ_DESC_DONE(3),    soc_dma_done_desc,    3, "CH3_DESC_DONE"    },

 /* Other (lowest priority) [21-28] */

 { IRQ_BIT21,        soc_intr_bit21,        0, "MMU_IRQ_STAT"    },
 { IRQ_BIT22,        soc_intr_bit22,        0, "IRQ_FIFO_CH1_DMA"    },
 { IRQ_BIT23,        soc_intr_bit23,        0, "IRQ_FIFO_CH2_DMA"    },
 { IRQ_STAT_ITER_DONE,    soc_intr_stat_dma,    0, "STAT_ITER_DONE"    },
 { IRQ_I2C_INTR,    soc_intr_i2c,        0, "I2C_INTR"        },
 { IRQ_ARL_LPM_LO_PAR,    soc_intr_lpm_lo_parity,    0, "LPM_LO_PARITY"    },
 { IRQ_BIT25,           soc_intr_bit25,            0, "LPM_HI_PARITY/BSE"    },
 { IRQ_BIT26,            soc_intr_bit26,            0, "L3_PARITY/CSE"      },
 { IRQ_BIT27,            soc_intr_bit27,            0, "L2_PARITY/HSE"      },
 { IRQ_BIT28,           soc_intr_bit28,         0, "VLAN_PARITY/MEMFAIL"},
 { IRQ_BROADSYNC_INTR,  soc_intr_bit31,         0, "BSAFE_OP_DONE/BROADSYNC_INTR"},

};

#define INTR_HANDLERS_COUNT    COUNTOF(soc_intr_handlers)

/*
 * define some short cuts to start processing interrupts quickly
 * start2: skip to packet processing
 * start1: skip low probability errors
 * else start at 0
 */
#define    INTR_START1_MASK    (IRQ_PCI_PARITY_ERR | \
                IRQ_PCI_FATAL_ERR | \
                IRQ_SCHAN_ERR | \
                IRQ_GBP_FULL)
#define    INTR_START1_POS        4
#define    INTR_START2_MASK    (INTR_START1_MASK | \
                IRQ_SCH_MSG_DONE | \
                IRQ_MIIM_OP_DONE | \
                IRQ_LINK_STAT_MOD | \
                IRQ_ARL_MBUF | \
                IRQ_ARL_MBUF_DROP | \
                IRQ_ARL_DMA_CNT0 | \
                IRQ_ARL_DMA_XFER)
#define    INTR_START2_POS        11

STATIC intr_handler_t soc_intr_block_lo_handlers[] = {
 { IRQ_BLOCK(0),    soc_intr_block,    0,  "BLOCK_0_ERR"    },
 { IRQ_BLOCK(1),    soc_intr_block,    1,  "BLOCK_1_ERR"    },
 { IRQ_BLOCK(2),    soc_intr_block,    2,  "BLOCK_2_ERR"    },
 { IRQ_BLOCK(3),    soc_intr_block,    3,  "BLOCK_3_ERR"    },
 { IRQ_BLOCK(4),    soc_intr_block,    4,  "BLOCK_4_ERR"    },
 { IRQ_BLOCK(5),    soc_intr_block,    5,  "BLOCK_5_ERR"    },
 { IRQ_BLOCK(6),    soc_intr_block,    6,  "BLOCK_6_ERR"    },
 { IRQ_BLOCK(7),    soc_intr_block,    7,  "BLOCK_7_ERR"    },
 { IRQ_BLOCK(8),    soc_intr_block,    8,  "BLOCK_8_ERR"    },
 { IRQ_BLOCK(9),    soc_intr_block,    9,  "BLOCK_9_ERR"    },
 { IRQ_BLOCK(10),    soc_intr_block,    10, "BLOCK_10_ERR"    },
 { IRQ_BLOCK(11),    soc_intr_block,    11, "BLOCK_11_ERR"    },
 { IRQ_BLOCK(12),    soc_intr_block,    12, "BLOCK_12_ERR"    },
 { IRQ_BLOCK(13),    soc_intr_block,    13, "BLOCK_13_ERR"    },
 { IRQ_BLOCK(14),    soc_intr_block,    14, "BLOCK_14_ERR"    },
 { IRQ_BLOCK(15),    soc_intr_block,    15, "BLOCK_15_ERR"    },
 { IRQ_BLOCK(16),    soc_intr_block,    16, "BLOCK_16_ERR"    },
 { IRQ_BLOCK(17),    soc_intr_block,    17, "BLOCK_17_ERR"    },
 { IRQ_BLOCK(18),    soc_intr_block,    18, "BLOCK_18_ERR"    },
 { IRQ_BLOCK(19),    soc_intr_block,    19, "BLOCK_19_ERR"    },
 { IRQ_BLOCK(20),    soc_intr_block,    20, "BLOCK_20_ERR"    },
 { IRQ_BLOCK(21),    soc_intr_block,    21, "BLOCK_21_ERR"    },
 { IRQ_BLOCK(22),    soc_intr_block,    22, "BLOCK_22_ERR"    },
 { IRQ_BLOCK(23),    soc_intr_block,    23, "BLOCK_23_ERR"    },
 { IRQ_BLOCK(24),    soc_intr_block,    24, "BLOCK_24_ERR"    },
 { IRQ_BLOCK(25),    soc_intr_block,    25, "BLOCK_25_ERR"    },
 { IRQ_BLOCK(26),    soc_intr_block,    26, "BLOCK_26_ERR"    },
 { IRQ_BLOCK(27),    soc_intr_block,    27, "BLOCK_27_ERR"    },
 { IRQ_BLOCK(28),    soc_intr_block,    28, "BLOCK_28_ERR"    },
 { IRQ_BLOCK(29),    soc_intr_block,    29, "BLOCK_29_ERR"    },
 { IRQ_BLOCK(30),    soc_intr_block,    30, "BLOCK_30_ERR"    },
 { IRQ_BLOCK(31),    soc_intr_block,    31, "BLOCK_31_ERR"    },
};
STATIC intr_handler_t soc_intr_block_hi_handlers[] = {
 { IRQ_BLOCK(0),    soc_intr_block,    32, "BLOCK_32_ERR"    },
 { IRQ_BLOCK(1),    soc_intr_block,    33, "BLOCK_33_ERR"    },
 { IRQ_BLOCK(2),    soc_intr_block,    34, "BLOCK_34_ERR"    },
 { IRQ_BLOCK(3),    soc_intr_block,    35, "BLOCK_35_ERR"    },
 { IRQ_BLOCK(4),    soc_intr_block,    36, "BLOCK_36_ERR"    },
 { IRQ_BLOCK(5),    soc_intr_block,    37, "BLOCK_37_ERR"    },
 { IRQ_BLOCK(6),    soc_intr_block,    38, "BLOCK_38_ERR"    },
 { IRQ_BLOCK(7),    soc_intr_block,    39, "BLOCK_39_ERR"    },
 { IRQ_BLOCK(8),    soc_intr_block,    40, "BLOCK_40_ERR"    },
 { IRQ_BLOCK(9),    soc_intr_block,    41, "BLOCK_41_ERR"    },
 { IRQ_BLOCK(10),    soc_intr_block,    42, "BLOCK_42_ERR"    },
 { IRQ_BLOCK(11),    soc_intr_block,    43, "BLOCK_43_ERR"    },
 { IRQ_BLOCK(12),    soc_intr_block,    44, "BLOCK_44_ERR"    },
 { IRQ_BLOCK(13),    soc_intr_block,    45, "BLOCK_45_ERR"    },
 { IRQ_BLOCK(14),    soc_intr_block,    46, "BLOCK_46_ERR"    },
 { IRQ_BLOCK(15),    soc_intr_block,    47, "BLOCK_47_ERR"    },
 { IRQ_BLOCK(16),    soc_intr_block,    48, "BLOCK_48_ERR"    },
 { IRQ_BLOCK(17),    soc_intr_block,    49, "BLOCK_49_ERR"    },
 { IRQ_BLOCK(18),    soc_intr_block,    50, "BLOCK_50_ERR"    },
 { IRQ_BLOCK(19),    soc_intr_block,    51, "BLOCK_51_ERR"    },
 { IRQ_BLOCK(20),    soc_intr_block,    52, "BLOCK_52_ERR"    },
 { IRQ_BLOCK(21),    soc_intr_block,    53, "BLOCK_53_ERR"    },
 { IRQ_BLOCK(22),    soc_intr_block,    54, "BLOCK_54_ERR"    },
 { IRQ_BLOCK(23),    soc_intr_block,    55, "BLOCK_55_ERR"    },
 { IRQ_BLOCK(24),    soc_intr_block,    56, "BLOCK_56_ERR"    },
 { IRQ_BLOCK(25),    soc_intr_block,    57, "BLOCK_57_ERR"    },
 { IRQ_BLOCK(26),    soc_intr_block,    58, "BLOCK_58_ERR"    },
 { IRQ_BLOCK(27),    soc_intr_block,    59, "BLOCK_59_ERR"    },
 { IRQ_BLOCK(28),    soc_intr_block,    60, "BLOCK_60_ERR"    },
 { IRQ_BLOCK(29),    soc_intr_block,    61, "BLOCK_61_ERR"    },
 { IRQ_BLOCK(30),    soc_intr_block,    62, "BLOCK_62_ERR"    },
 { IRQ_BLOCK(31),    soc_intr_block,    63, "BLOCK_63_ERR"    },
};
#define INTR_BLOCK_LO_HANDLERS_COUNT    COUNTOF(soc_intr_block_lo_handlers)
#define INTR_BLOCK_HI_HANDLERS_COUNT    COUNTOF(soc_intr_block_hi_handlers)

#define SOC_CMIC_BLK_CLP_0_INDX 24
#define SOC_CMIC_BLK_CLP_1_INDX 25
#define SOC_CMIC_BLK_XLP_0_INDX 27
#define SOC_CMIC_BLK_XLP_1_INDX 28

#define _PORT_BLOCK_FROM_IRQ_STATE2(unit, cmic_irq_state_2, stat2_field, block_bit)   \
        ( (soc_reg_field_get(unit, CMIC_CMC0_IRQ_STAT2r, (cmic_irq_state_2), stat2_field) != 0 ) << block_bit ) 

/*
 * Interrupt handler functions
 */

STATIC void
soc_intr_schan_done(int unit, uint32 ignored)
{
    soc_control_t    *soc = SOC_CONTROL(unit);

    COMPILER_REFERENCE(ignored);

    /* Record the schan control regsiter */
    soc->schan_result[SOC_PCI_CMC(unit)] = soc_pci_read(unit, CMIC_SCHAN_CTRL);

    soc->stat.intr_sc++;

    soc_pci_write(unit, CMIC_SCHAN_CTRL, SC_MSG_DONE_CLR);

    if (soc->schanIntr[SOC_PCI_CMC(unit)]) {
    sal_sem_give(soc->schanIntr[SOC_PCI_CMC(unit)]);
    }
}

STATIC soc_schan_err_t
soc_schan_error_type(int unit, int err_code)
{
    int            bitcount = 0;
    soc_schan_err_t    err = SOC_SCERR_INVALID;

    switch (SOC_CHIP_GROUP(unit)) {
    case SOC_CHIP_BCM5673:
    case SOC_CHIP_BCM5674:
        if (err_code & 0x10) {
            err = SOC_SCERR_MMU_NPKT_CELLS;
            ++bitcount;
        }
    if (err_code & 0x20) {
            err = SOC_SCERR_MEMORY_PARITY;
            ++bitcount;
        }
        /* Fall through */
    case SOC_CHIP_BCM5690:
    case SOC_CHIP_BCM5695:
        if (err_code & 0x1) {
            err = SOC_SCERR_CFAP_OVER_UNDER;
            ++bitcount;
        }
    if (err_code & 0x2) {
            err = SOC_SCERR_MMU_SOFT_RST;
            ++bitcount;
        }
    if (err_code & 0x4) {
            err = SOC_SCERR_CBP_CELL_CRC;
            ++bitcount;
        }
    if (err_code & 0x8) {
            err = SOC_SCERR_CBP_HEADER_PARITY;
            ++bitcount;
        }
        break;
    case SOC_CHIP_BCM5665:
    case SOC_CHIP_BCM5650:
        if (err_code & 0x1) {
            err = SOC_SCERR_CELL_PTR_CRC;
            ++bitcount;
        }
    if (err_code & 0x2) {
            err = SOC_SCERR_CELL_DATA_CRC;
            ++bitcount;
        }
    if (err_code & 0x4) {
            err = SOC_SCERR_FRAME_DATA_CRC;
            ++bitcount;
        }
    if (err_code & 0x8) {
            err = SOC_SCERR_CELL_PTR_BLOCK_CRC;
            ++bitcount;
        }
    if (err_code & 0x10) {
            err = SOC_SCERR_MEMORY_PARITY;
            ++bitcount;
        }
    if (err_code & 0x20) {
            err = SOC_SCERR_PLL_DLL_LOCK_LOSS;
            ++bitcount;
        }
        break;
    default:
        break;
    }

    if (bitcount > 1) {
        err = SOC_SCERR_MULTIPLE_ERR;
    }

    return err;
}

STATIC void
_soc_sch_error_unblock(void *p_unit, void *p2, void *p3, void *p4, void *p5)
{
    COMPILER_REFERENCE(p2);
    COMPILER_REFERENCE(p3);
    COMPILER_REFERENCE(p4);
    COMPILER_REFERENCE(p5);

    soc_intr_enable(PTR_TO_INT(p_unit), IRQ_SCHAN_ERR);
}

STATIC void
soc_intr_schan_error(int unit, uint32 ignored)
{
    soc_control_t    *soc = SOC_CONTROL(unit);
    uint32        scerr, slot;
    int            vld, src, dst, opc, err;

    COMPILER_REFERENCE(ignored);

    /*
     * Read the beginning of the S-chan message so its contents are
     * visible when a PCI bus analyzer is connected.
     */

    soc_pci_analyzer_trigger(unit);

    if (bsl_check(bslLayerSoc, bslSourceIntr, bslSeverityNormal, unit)) {
        slot = soc_pci_read(unit, 0);
        slot = soc_pci_read(unit, 4);
        slot = soc_pci_read(unit, 8);
        slot = soc_pci_read(unit, 0xC);
    }

    scerr = soc_pci_read(unit, CMIC_SCHAN_ERR);    /* Clears intr */
    soc_pci_write(unit, CMIC_SCHAN_ERR, 0); /* Clears intr in some devs */

    soc->stat.intr_sce++;

    /*
     * If the valid bit is not set, it's probably because the error
     * occurred at the same time the software was starting an unrelated
     * S-channel operation.  There is no way to prevent this conflict.
     * We'll indicate that that the valid bit was not set and continue,
     * since the error is probably still latched.
     */

    vld = soc_reg_field_get(unit, CMIC_SCHAN_ERRr, scerr,
           (SOC_IS_XGS3_SWITCH(unit)) ? ERRBITf : VALIDf);
    src = soc_reg_field_get(unit, CMIC_SCHAN_ERRr, scerr, SRC_PORTf);
    dst = soc_reg_field_get(unit, CMIC_SCHAN_ERRr, scerr, DST_PORTf);
    opc = soc_reg_field_get(unit, CMIC_SCHAN_ERRr, scerr, OP_CODEf);
    err = soc_reg_field_get(unit, CMIC_SCHAN_ERRr, scerr, ERR_CODEf);

    if ((!soc->mmu_error_block) || (opc != MEMORY_FAIL_NOTIFY)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "UNIT %d SCHAN ERROR: V/E=%d SRC=%d DST=%d "
                              "OPCODE=%d(%s) ERRCODE=0x%x\n"),
                   unit, vld, src, dst,
                   opc, soc_schan_op_name(opc), err));
    }

#ifdef BCM_XGS3_SWITCH_SUPPORT
        if (SOC_IS_XGS3_SWITCH(unit)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "UNIT %d SCHAN ERROR: Unknown reason\n"),
                       unit));
    } else
#endif
    if (opc == MEMORY_FAIL_NOTIFY) {
    switch (soc_schan_error_type(unit, err)) {
    case SOC_SCERR_CFAP_OVER_UNDER:
        /*
         * The CFAP is empty but a request for a cell pointer came
         * in, or the CFAP is full but a request to return a cell
         * pointer came in.
         */
        soc->stat.err_cfap++;
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "UNIT %d SCHAN ERROR: CFAP oversubscribed\n"),
                   unit));
        break;
    case SOC_SCERR_SDRAM_CHKSUM:
        /*
         * Checksum error occurred when fetching a slot from SDRAM.
         */
        soc->stat.err_sdram++;
        slot = soc_pci_read(unit, CMIC_MEM_FAIL);
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "UNIT %d SCHAN ERROR: SDRAM checksum error, "
                              "slot=0x%x (GBP index 0x%x)\n"),
                   unit, slot, slot * 0x40));
        break;
    case SOC_SCERR_UNEXP_FIRST_CELL:
        /*
         * Unexpected first cell
         */
        soc->stat.err_fcell++;
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "UNIT %d SCHAN ERROR: Unexpected first cell\n"),
                   unit));
        break;
    case SOC_SCERR_MMU_SOFT_RST:
        /*
         * MMU soft reset: received a second start cell without
         * receiving and end cell for the previous packet.
         */
        soc->stat.err_sr++;
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "UNIT %d SCHAN ERROR: MMU soft reset\n"),
                   unit));
        break;
    case SOC_SCERR_CBP_CELL_CRC:
        soc->stat.err_cellcrc++;
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "UNIT %d SCHAN ERROR: CBP Cell CRC error\n"),
                   unit));
        break;
    case SOC_SCERR_CBP_HEADER_PARITY:
        soc->stat.err_cbphp++;
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "UNIT %d SCHAN ERROR: CBP Header parity error\n"),
                   unit));
        break;
    case SOC_SCERR_MMU_NPKT_CELLS:
        soc->stat.err_npcell++;
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "UNIT %d SCHAN ERROR: "
                              "MMU sent cells not in packet\n"),
                   unit));
        break;
    case SOC_SCERR_MEMORY_PARITY:
        soc->stat.err_mp++;
        break;
    case SOC_SCERR_CELL_PTR_CRC:
        soc->stat.err_cpcrc++;
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "UNIT %d SCHAN ERROR: Cell data CRC error\n"),
                       unit));
        break;
    case SOC_SCERR_CELL_DATA_CRC:
        soc->stat.err_cdcrc++;
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "UNIT %d SCHAN ERROR: Cell data CRC error\n"),
                       unit));
        break;
    case SOC_SCERR_FRAME_DATA_CRC:
        soc->stat.err_fdcrc++;
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "UNIT %d SCHAN ERROR: Frame data CRC error\n"),
                       unit));
        break;
    case SOC_SCERR_CELL_PTR_BLOCK_CRC:
        soc->stat.err_cpbcrc++;
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "UNIT %d SCHAN ERROR: "
                                  "Cell pointer block CRC error\n"),
                       unit));
        break;
    case SOC_SCERR_PLL_DLL_LOCK_LOSS:
        soc->stat.err_pdlock++;
        break;
    case SOC_SCERR_MULTIPLE_ERR:
        soc->stat.err_multi++;
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "UNIT %d SCHAN ERROR: Multiple errors: 0x%x\n"),
                   unit, err));
        break;
    case SOC_SCERR_INVALID:
        soc->stat.err_invalid++;
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "UNIT %d SCHAN ERROR: Unknown memory error\n"),
                   unit));
        break;
        default:
            assert(0);
            break;
    }
    }

    if (soc->schanIntrBlk != 0) {
    soc_intr_disable(unit, IRQ_SCHAN_ERR);

    sal_dpc_time(soc->schanIntrBlk, _soc_sch_error_unblock,
             INT_TO_PTR(unit), 0, 0, 0, 0);
    }
}

STATIC void
soc_intr_arl_mbuf(int unit, uint32 ignored)
{
    soc_control_t *soc = SOC_CONTROL(unit);

    COMPILER_REFERENCE(ignored);

#if defined(BCM_SCORPION_SUPPORT)
    if (SOC_IS_SCORPION(unit)) {
        /* Disbale Interrupt, same will be enbled after interrupt handling */
        soc_intr_disable(unit, IRQ_CHIP_FUNC_0);
        soc->stat.intr_chip_func[0]++;
        sal_dpc(soc_scorpion_l2_overflow_interrupt_handler, INT_TO_PTR(unit),
                0, 0, 0, 0);
        return;
    }
#endif

#if defined(BCM_TRX_SUPPORT)
    if (SOC_IS_TRX(unit)) {
        /* IRQ_CHIP_FUNC_0 */
        soc_intr_disable(unit, IRQ_CHIP_FUNC_0);
        soc->stat.intr_chip_func[0]++;
        return;
    }
#endif

    /*
     * Disable the interrupt; it is re-enabled by the ARL thread after
     * it processes the messages.
     */

    soc_intr_disable(unit, IRQ_ARL_MBUF);

    soc->stat.intr_arl_m++;

    if (soc->arl_notify) {
        soc->arl_mbuf_done = 1;
    if (!soc->arl_notified) {
        soc->arl_notified = 1;
        sal_sem_give(soc->arl_notify);
    }
    }
}

STATIC void
soc_intr_arl_drop(int unit, uint32 ignored)
{
    soc_control_t *soc = SOC_CONTROL(unit);

    COMPILER_REFERENCE(ignored);

#if defined(BCM_TRX_SUPPORT)
    if (SOC_IS_TRX(unit)) {
        /* IRQ_CHIP_FUNC_1 */
        soc_intr_disable(unit, IRQ_CHIP_FUNC_1);
        soc->stat.intr_chip_func[1]++;
        return;
    }
#endif

    soc_pci_analyzer_trigger(unit);

    soc_intr_disable(unit, IRQ_ARL_MBUF_DROP);

    soc_pci_write(unit, CMIC_SCHAN_CTRL, SC_ARL_MSG_DROPPED_CLR);

    soc->stat.intr_arl_d++;

    if (soc->arl_notify) {
        soc->arl_msg_drop = 1;
    if (!soc->arl_notified) {
        soc->arl_notified = 1;
        sal_sem_give(soc->arl_notify);
    }
    }
}

STATIC void
soc_intr_arl_cnt0(int unit, uint32 ignored)
{
    soc_control_t *soc = SOC_CONTROL(unit);

    COMPILER_REFERENCE(ignored);

#if defined(BCM_TRX_SUPPORT)
    if (SOC_IS_TRX(unit)) {
        /* IRQ_CHIP_FUNC_4 */
        soc_intr_disable(unit, IRQ_CHIP_FUNC_4);
#if defined(BCM_TRIUMPH_SUPPORT)
        if (SOC_IS_TRIUMPH(unit)) {
            sal_dpc(soc_triumph_esm_intr_status, INT_TO_PTR(unit),
                    0, 0, 0, 0);
        }
#endif /* BCM_TRIUMPH_SUPPORT */
#if defined(BCM_TRIUMPH2_SUPPORT)
        if (SOC_IS_TRIUMPH2(unit)) {
            sal_dpc(soc_triumph2_esm_intr_status, INT_TO_PTR(unit),
                    0, 0, 0, 0);
        }
#endif /* BCM_TRIUMPH2_SUPPORT */
        soc->stat.intr_chip_func[4]++;
        return;
    }
#endif

    soc_pci_write(unit, CMIC_SCHAN_CTRL, SC_ARL_DMA_EN_CLR);
    soc_pci_write(unit, CMIC_SCHAN_CTRL, SC_ARL_DMA_DONE_CLR);

    soc->stat.intr_arl_0++;

    if (soc->arl_notify) {
    soc->arl_dma_cnt0 = 1;
    if (!soc->arl_notified) {
        soc->arl_notified = 1;
        sal_sem_give(soc->arl_notify);
    }
    }
}

STATIC void
soc_intr_arl_xfer(int unit, uint32 ignored)
{
    soc_control_t *soc = SOC_CONTROL(unit);

    COMPILER_REFERENCE(ignored);

#if defined(BCM_TRX_SUPPORT)
    if (SOC_IS_TRX(unit)) {
        /* IRQ_CHIP_FUNC_3 */
        soc_intr_disable(unit, IRQ_CHIP_FUNC_3);
#if defined(BCM_TRIUMPH_SUPPORT)
        if (SOC_IS_TRIUMPH(unit)) {
            sal_dpc(soc_triumph_esm_intr_status, INT_TO_PTR(unit),
                    0, 0, 0, 0);
        }
#endif /* BCM_TRIUMPH_SUPPORT */
#if defined(BCM_TRIUMPH2_SUPPORT)
        if (SOC_IS_TRIUMPH2(unit)) {
            sal_dpc(soc_triumph2_esm_intr_status, INT_TO_PTR(unit),
                    0, 0, 0, 0);
        }
#endif /* BCM_TRIUMPH2_SUPPORT */
        soc->stat.intr_chip_func[3]++;
        return;
    }
#endif

    soc_intr_disable(unit, IRQ_ARL_DMA_XFER);

    soc_pci_write(unit, CMIC_SCHAN_CTRL, SC_ARL_DMA_XFER_DONE_CLR);

    soc->stat.intr_arl_x++;

    if (soc->arl_notify) {
    soc->arl_dma_xfer = 1;
    if (!soc->arl_notified) {
        soc->arl_notified = 1;
        sal_sem_give(soc->arl_notify);
    }
    }
}

STATIC void
soc_intr_tdma_done(int unit, uint32 ignored)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    int cmc = SOC_PCI_CMC(unit);
    int ch = soc->tdma_ch;

    COMPILER_REFERENCE(ignored);

    soc_intr_disable(unit, IRQ_TDMA_DONE);

    soc->stat.intr_tdma++;

    if (soc->tableDmaIntrEnb) {
        sal_sem_give(soc->sbusDmaIntrs[cmc][ch]);
    }

}

STATIC void
soc_intr_tslam_done(int unit, uint32 ignored)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    int cmc = SOC_PCI_CMC(unit);
    int ch = soc->tslam_ch;

    COMPILER_REFERENCE(ignored);

    soc_intr_disable(unit, IRQ_TSLAM_DONE);

    soc->stat.intr_tslam++;

    if (soc->tslamDmaIntrEnb) {
        sal_sem_give(soc->sbusDmaIntrs[cmc][ch]);
    }
}

STATIC void
soc_intr_gbp_full(int unit, uint32 ignored)
{
    soc_control_t    *soc = SOC_CONTROL(unit);

    COMPILER_REFERENCE(ignored);

#if defined(BCM_TRX_SUPPORT)
    if (SOC_IS_TRX(unit)) {
        /* IRQ_CHIP_FUNC_2 */
        soc_intr_disable(unit, IRQ_CHIP_FUNC_2);
        soc->stat.intr_chip_func[2]++;
        return;
    }
#endif

    soc->stat.intr_gbp++;

    soc_pci_analyzer_trigger(unit);

    /*
     * It doesn't make sense to "clear" this interrupt, so we disable
     * the interrupt in the mask register and re-enable it some time
     * later using a deferred procedure call.
     */

    soc_intr_disable(unit, IRQ_GBP_FULL);

#ifdef BCM_GBP_SUPPORT
    sal_dpc(_soc_gbp_full_block, INT_TO_PTR(unit), 0, 0, 0, 0);
#endif
}

STATIC void
soc_intr_link_stat(int unit, uint32 ignored)
{
    soc_control_t    *soc = SOC_CONTROL(unit);

    COMPILER_REFERENCE(ignored);

    soc_pci_analyzer_trigger(unit);

    soc->stat.intr_ls++;

    /* Clear interrupt */

    soc_pci_write(unit, CMIC_SCHAN_CTRL, SC_LINK_STAT_MSG_CLR);

    /* Perform user callout, if one is registered */

    if (soc->soc_link_callout != NULL) {
    (*soc->soc_link_callout)(unit);
    }
}

/*
 * PCI Parity and Fatal Error Reporting
 *
 *    If the interrupt routine prints a message on each error,
 *    the console can be frozen.
 *
 *    For this reason errors are counted for a period of time and
 *    reported together at a maximum rate.
 */

#define PCI_REPORT_TYPE_PARITY        1
#define PCI_REPORT_TYPE_FATAL        2
#define PCI_REPORT_PERIOD        (SECOND_USEC / 4)

STATIC char *_soc_pci_dma_types[] = {
    "DMA CH0",
    "DMA CH1",
    "DMA CH2",
    "DMA CH3"
};

STATIC char *_soc_pci_extended_dma_types[] = {
    "Status write for TX and RX DMA CH0",     /* 0 */
    "Table DMA",                              /* 1 */
    "Memory write for RX DMA CH0",            /* 2 */
    "Stats DMA",                              /* 3 */
    "Status write for TX and RX DMA CH1",     /* 4 */
    "Unknown",                                /* 5 */
    "Memory write for RX DMA CH1",            /* 6 */
    "Unknown",                                /* 7 */
    "Status write for TX and RX DMA CH2",     /* 8 */
    "Unknown",                                /* 9 */
    "Memory write for RX DMA CH2",            /* 10 */
    "Unknown",                                /* 11 */
    "Status write for TX and RX DMA CH3",     /* 12 */
    "Unknown",                                /* 13 */
    "Memory write for RX DMA CH3",            /* 14 */
    "Unknown",                                /* 15 */
    "Descriptor read for TX and RX DMA CH0",  /* 16 */
    "SLAM DMA",                               /* 17 */
    "Memory read for TX DMA CH0",             /* 18 */
    "Unknown",                                /* 19 */
    "Descriptor read for TX and RX DMA CH1",  /* 20 */
    "Unknown",                                /* 21 */
    "Memory read for TX DMA CH1",             /* 22 */
    "Unknown",                                /* 23 */
    "Descriptor read for TX and RX DMA CH2",  /* 24 */
    "Unknown",                                /* 25 */
    "Memory read for TX DMA CH2",             /* 26 */
    "Unknown",                                /* 27 */
    "Descriptor read for TX and RX DMA CH3",  /* 28 */
    "Unknown",                                /* 29 */
    "Memory read for TX DMA CH3",             /* 30 */
    "Unknown"                                 /* 31 */
};

STATIC char *_soc_pci_extended_trx_dma_types[] = {
    "Table DMA",                                /* 0 */
    "Stats DMA",                                /* 1 */
    "Memory write for RX DMA CH0",              /* 2 */
    "Memory write for RX DMA CH1",              /* 3 */
    "Memory write for RX DMA CH2",              /* 4 */
    "Memory write for RX DMA CH3",              /* 5 */
    "Status write for TX and RX DMA CH0",       /* 6 */
    "Status write for TX and RX DMA CH1",       /* 7 */
    "Status write for TX and RX DMA CH2",       /* 8 */
    "Status write for TX and RX DMA CH3",       /* 9 */
    "SLAM DMA",                                 /* 10 */
    "Memory read for TX DMA CH0",               /* 11 */
    "Memory read for TX DMA CH1",               /* 12 */
    "Memory read for TX DMA CH2",               /* 13 */
    "Memory read for TX DMA CH3",               /* 14 */
    "Descriptor read for TX and RX DMA CH0",    /* 15 */
    "Descriptor read for TX and RX DMA CH1",    /* 16 */
    "Descriptor read for TX and RX DMA CH2",    /* 17 */
    "Descriptor read for TX and RX DMA CH3",    /* 18 */
    "FIFO DMA CH0",                             /* 19 */
    "FIFO DMA CH1",                             /* 20 */
    "FIFO DMA CH2",                             /* 21 */
    "FIFO DMA CH3",                             /* 22 */
    "Unknown",                                  /* 23 */
    "Unknown",                                  /* 24 */
    "Unknown",                                  /* 25 */
    "Unknown",                                  /* 26 */
    "Unknown",                                  /* 27 */
    "Unknown",                                  /* 28 */
    "Unknown",                                  /* 29 */
    "Unknown",                                  /* 30 */
    "Unknown"                                   /* 31 */
};

STATIC void
_soc_pci_report_error(void *p_unit, void *stat, void *type,
              void *errcnt_dpc, void *p5)
{
    int             unit = PTR_TO_INT(p_unit);
    soc_control_t    *soc = SOC_CONTROL(unit);
    uint32        errcnt_cur = 0, dmatype_code = 0;
    char        *errtype = NULL, *dmatype = NULL;

    COMPILER_REFERENCE(p5);

    switch (PTR_TO_INT(type)) {
    case PCI_REPORT_TYPE_PARITY:
    soc->pciParityDPC = 0;
    errcnt_cur = soc->stat.intr_pci_pe;
    errtype = "Parity";
        if (soc_feature(unit, soc_feature_extended_pci_error)) {
            dmatype_code = DS_EXT_PCI_PARITY_ERR(PTR_TO_INT(stat));
        } else {
            dmatype_code = DS_PCI_PARITY_ERR(PTR_TO_INT(stat));
        }
    break;
    case PCI_REPORT_TYPE_FATAL:
    soc->pciFatalDPC = 0;
    errcnt_cur = soc->stat.intr_pci_fe;
    errtype = "Fatal";
        if (soc_feature(unit, soc_feature_extended_pci_error)) {
            dmatype_code = DS_EXT_PCI_FATAL_ERR(PTR_TO_INT(stat));
        } else {
            dmatype_code = DS_PCI_FATAL_ERR(PTR_TO_INT(stat));
        }
    break;
    }

    if (soc_feature(unit, soc_feature_extended_pci_error)) {
        if (SOC_IS_TRX(unit)) {
            dmatype =
                _soc_pci_extended_trx_dma_types[dmatype_code];
        } else {
            dmatype = _soc_pci_extended_dma_types[dmatype_code];
        }
    } else {
        dmatype = _soc_pci_dma_types[dmatype_code];
    }

    if (errcnt_cur == PTR_TO_INT(errcnt_dpc) + 1) {
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "UNIT %d ERROR interrupt: "
                          "CMIC_DMA_STAT = 0x%08x "
                          "PCI %s Error on %s\n"),
               unit,
               PTR_TO_INT(stat),
               errtype, dmatype));
    } else {
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "UNIT %d ERROR interrupt: "
                          "%d PCI %s Errors on %s\n"),
               unit, errcnt_cur - PTR_TO_INT(errcnt_dpc),
               errtype, dmatype));
    }
}

STATIC void
soc_intr_pci_parity(int unit, uint32 ignored)
{
    soc_control_t    *soc = SOC_CONTROL(unit);
    uint32        stat;
    int            errcnt;

    COMPILER_REFERENCE(ignored);

    soc_pci_analyzer_trigger(unit);

    stat = soc_pci_read(unit, CMIC_DMA_STAT);

    soc_pci_write(unit, CMIC_SCHAN_CTRL, SC_PCI_PARITY_ERR_CLR);

    errcnt = soc->stat.intr_pci_pe++;

    if (!soc->pciParityDPC) {
    soc->pciParityDPC = 1;
    sal_dpc_time(PCI_REPORT_PERIOD, _soc_pci_report_error,
             INT_TO_PTR(unit), INT_TO_PTR(stat), 
             INT_TO_PTR(PCI_REPORT_TYPE_PARITY), 
             INT_TO_PTR(errcnt), 0);
    }
}

STATIC void
soc_intr_pci_fatal(int unit, uint32 ignored)
{
    soc_control_t    *soc = SOC_CONTROL(unit);
    uint32        stat;
    int            errcnt;

    COMPILER_REFERENCE(ignored);

    soc_pci_analyzer_trigger(unit);

    stat = soc_pci_read(unit, CMIC_DMA_STAT);

    soc_pci_write(unit, CMIC_SCHAN_CTRL, SC_PCI_FATAL_ERR_CLR);

    errcnt = soc->stat.intr_pci_fe++;

    if (!soc->pciFatalDPC) {
    soc->pciFatalDPC = 1;
    sal_dpc_time(PCI_REPORT_PERIOD, _soc_pci_report_error,
             INT_TO_PTR(unit), INT_TO_PTR(stat), 
             INT_TO_PTR(PCI_REPORT_TYPE_FATAL), 
             INT_TO_PTR(errcnt), 0);
    }
}

STATIC void
soc_intr_i2c(int unit, uint32 ignored)
{
    soc_control_t *soc = SOC_CONTROL(unit);

    COMPILER_REFERENCE(ignored);
    
    soc->stat.intr_i2c++;

#if defined (INCLUDE_I2C) && !defined (BCM_SAND_SUPPORT)
    soc_i2c_intr(unit);
#else
    soc_intr_disable(unit, IRQ_I2C_INTR);
#endif
}

STATIC void
soc_intr_miim_op(int unit, uint32 ignored)
{
    soc_control_t *soc = SOC_CONTROL(unit);

    COMPILER_REFERENCE(ignored);

    soc_pci_write(unit, CMIC_SCHAN_CTRL, SC_MIIM_OP_DONE_CLR);

    soc->stat.intr_mii++;

    if (soc->miimIntr) {
    sal_sem_give(soc->miimIntr);
    }
}
   
STATIC void
soc_intr_stat_dma(int unit, uint32 ignored)
{
    soc_control_t *soc = SOC_CONTROL(unit);

    COMPILER_REFERENCE(ignored);

    soc_pci_write(unit, CMIC_DMA_STAT, DS_STAT_DMA_ITER_DONE_CLR);

    soc->stat.intr_stats++;

    if (soc->counter_intr) {
    sal_sem_give(soc->counter_intr);
    }
}

#ifdef BCM_HERCULES_SUPPORT

STATIC void
_soc_intr_mmu_analyze(void *p_unit, void *p2, void *p3, void *p4, void *p5)
{
    int unit = PTR_TO_INT(p_unit);
 
    COMPILER_REFERENCE(p2);
    COMPILER_REFERENCE(p3);
    COMPILER_REFERENCE(p4);
    COMPILER_REFERENCE(p5);
 
    if (soc_mmu_error_all(unit) < 0) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "MMU error analysis failed, MMU interrupt disabled\n")));
    } else {
        soc_intr_enable(unit, IRQ_MMU_IRQ_STAT);
    }
}
 
STATIC void
soc_intr_mmu_stat(int unit, uint32 ignored)
{
    uint32         src, mask;
    soc_control_t     *soc = SOC_CONTROL(unit);

    COMPILER_REFERENCE(ignored);

    src = soc_pci_read(unit, CMIC_MMUIRQ_STAT);
    mask = soc_pci_read(unit, CMIC_MMUIRQ_MASK);

    mask &= ~src;

    /* We know about the port(s), don't interrupt again until serviced */
    soc_pci_write(unit, CMIC_MMUIRQ_MASK, mask);    

    soc->stat.intr_mmu++;

    /* We'll turn this back on if we succeed in the analysis */
    soc_intr_disable(unit, IRQ_MMU_IRQ_STAT);
    sal_dpc(_soc_intr_mmu_analyze, INT_TO_PTR(unit), 0, 0, 0, 0);
}

#endif /* BCM_HERCULES_SUPPORT */


#if defined(BCM_XGS12_SWITCH_SUPPORT)
STATIC void
soc_intr_arl_error(int unit, uint32 ignored)
{
    soc_control_t    *soc;

    COMPILER_REFERENCE(ignored);

    soc = SOC_CONTROL(unit);
    soc->stat.intr_mmu++;    /* should use separate counter */
    soc_intr_disable(unit, IRQ_ARL_ERROR);
    
    
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "UNIT %d ARL ERROR (bucket overflow or parity error\n"),
               unit));
}

#endif /* BCM_XGS12_SWITCH_SUPPORT */

STATIC void
soc_intr_bit21(int unit, uint32 ignored)
{
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        soc_control_t *soc;
        soc = SOC_CONTROL(unit);

        /* IRQ_FIFO_CH0_DMA */
        soc_intr_disable(unit, IRQ_FIFO_CH0_DMA);
        soc->stat.intr_fifo_dma[0]++;

    /* Clear FIFO_CH0_DMA_HOSTMEM_TIMEOUT bit */
    WRITE_CMIC_FIFO_RD_DMA_DEBUGr(unit, 1);

        if (soc->ipfixIntr) {
            /* Ingress IPFIX */
        sal_sem_give(soc->ipfixIntr);
        }
        return;
    }
#endif
#ifdef BCM_HERCULES_SUPPORT
    if (SOC_IS_HERCULES(unit)) {
    soc_intr_mmu_stat(unit, ignored);
    }
#endif /* BCM_HERCULES_SUPPORT */

#if defined(BCM_XGS12_SWITCH_SUPPORT)
    if (SOC_IS_XGS12_SWITCH(unit)) {
    soc_intr_arl_error(unit, ignored);
    }
#endif /* BCM_XGS_SWITCH_SUPPORT */
}

STATIC void
soc_intr_bit22(int unit, uint32 ignored)
{
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        soc_control_t *soc;
        soc = SOC_CONTROL(unit);

        /* IRQ_FIFO_CH1_DMA */
        soc_intr_disable(unit, IRQ_FIFO_CH1_DMA);
        soc->stat.intr_fifo_dma[1]++;

    /* Clear FIFO_CH1_DMA_HOSTMEM_TIMEOUT bit */
    WRITE_CMIC_FIFO_RD_DMA_DEBUGr(unit, 3);

        if (soc->arl_notify) {
            /* Internal L2_MOD_FIFO */
        sal_sem_give(soc->arl_notify);
        }
        return;
    }
#endif
}

STATIC void
soc_intr_bit23(int unit, uint32 ignored)
{
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        soc_control_t *soc;
        soc = SOC_CONTROL(unit);

        /* IRQ_FIFO_CH2_DMA */
        soc_intr_disable(unit, IRQ_FIFO_CH2_DMA);
        soc->stat.intr_fifo_dma[2]++;

    /* Clear FIFO_CH2_DMA_HOSTMEM_TIMEOUT bit */
    WRITE_CMIC_FIFO_RD_DMA_DEBUGr(unit, 5);

        if (soc->arl_notify) {
            /* External EXT_L2_MOD_FIFO */
        sal_sem_give(soc->arl_notify);
        }
        return;
    }
#endif
}

STATIC void
soc_intr_lpm_lo_parity(int unit, uint32 ignored)
{
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        soc_control_t *soc;
        soc = SOC_CONTROL(unit);

        /* IRQ_FIFO_CH3_DMA */
        soc_intr_disable(unit, IRQ_FIFO_CH3_DMA);
        soc->stat.intr_fifo_dma[3]++;

    /* Clear FIFO_CH3_DMA_HOSTMEM_TIMEOUT bit */
    WRITE_CMIC_FIFO_RD_DMA_DEBUGr(unit, 7);

        if (soc->ipfixIntr) {
            /* Egress IPFIX */
        sal_sem_give(soc->ipfixIntr);
        }
        return;
    }
#endif
}

STATIC void
soc_intr_bit25(int unit, uint32 ignored)
{
}

STATIC void
soc_intr_bit26(int unit, uint32 ignored)
{
}

STATIC void
soc_intr_bit27(int unit, uint32 ignored)
{
}

STATIC void
soc_intr_bit28(int unit, uint32 ignored)
{
    soc_pci_analyzer_trigger(unit);

#if defined(BCM_XGS_SUPPORT)
    (void)soc_ser_parity_error_intr(unit);
#endif
}

STATIC void
soc_intr_bit31(int unit, uint32 ignored)
{
#ifdef BCM_TRIUMPH2_SUPPORT
    if (soc_feature(unit, soc_feature_time_support)) {
        soc_control_t    *soc = SOC_CONTROL(unit);
        soc_intr_disable(unit, IRQ_BROADSYNC_INTR);

        /* Perform user callout, if one is registered */
        if (soc->soc_time_callout != NULL) {
            (*soc->soc_time_callout)(unit);
        }
        soc_intr_enable(unit, IRQ_BROADSYNC_INTR);
    }
#endif /* BCM_TRIUMPH2_SUPPORT */
}


STATIC void
soc_intr_block(int unit, uint32 block)
{
    if (block < 32) {
        soc_intr_block_lo_disable(unit, (1<<block));
    } else {
        soc_intr_block_hi_disable(unit, (1<<(block-32)));
    }
    sal_dpc(soc_cmn_block_error, INT_TO_PTR(unit), INT_TO_PTR(block), 0, 0, 0);
}

/*
 * Enable (unmask) or disable (mask) a set of CMIC interrupts.  These
 * routines should be used instead of manipulating CMIC_IRQ_MASK
 * directly, since a read-modify-write is required.  The return value is
 * the previous mask (can pass mask of 0 to just get the current mask).
 */

uint32
soc_intr_enable(int unit, uint32 mask)
{
    uint32 oldMask;
    uint32 newMask;
    int s;

    s = sal_splhi();
    oldMask = SOC_IRQ_MASK(unit);
    SOC_IRQ_MASK(unit) |= mask;
    newMask = SOC_IRQ_MASK(unit);
    /* In polled mode, the hardware IRQ mask is always zero */
    if (SOC_CONTROL(unit)->soc_flags & SOC_F_POLLED) {
            newMask = 0;
    }
    IRQ_MASK_SET(unit, CMIC_IRQ_MASK, newMask);
    sal_spl(s);

    return oldMask;
}

uint32
soc_intr_disable(int unit, uint32 mask)
{
    uint32 oldMask;
    uint32 newMask;
    int s;

    s = sal_splhi();
    oldMask = SOC_IRQ_MASK(unit);
    SOC_IRQ_MASK(unit) &= ~mask;
    newMask = SOC_IRQ_MASK(unit);
    /* In polled mode, the hardware IRQ mask is always zero */
    if (SOC_CONTROL(unit)->soc_flags & SOC_F_POLLED) {
        newMask = 0;
    }
    IRQ_MASK_SET(unit, CMIC_IRQ_MASK, newMask);
    sal_spl(s);

    return oldMask;
}


/*
 * Enable (unmask) or disable (mask) a set of CMIC block specific interrupts. 
 * soc_intr_block_lo_enable/disable handle block 0-31, while
 * soc_intr_block_hi_enable/disable handle block 32-63. 
 * These routines should be used instead of manipulating CMIC_IRQ_MASK_1/CMIC_IRQ_MASK_2
 * directly, since a read-modify-write is required.  The return value is
 * the previous mask (can pass mask of 0 to just get the current mask).
 */

uint32
soc_intr_block_lo_enable(int unit, uint32 mask)
{
    uint32 oldMask = 0;

#ifdef BCM_DFE_SUPPORT
{
    uint32 newMask;
    int s;

    if (SOC_IS_DFE(unit)) {
    s = sal_splhi();
    oldMask = SOC_IRQ1_MASK(unit);
    SOC_IRQ1_MASK(unit) |= mask;
    newMask = SOC_IRQ1_MASK(unit);
    LOG_VERBOSE(BSL_LS_SOC_INTR,
                (BSL_META("%s(): oldMask=0x%x, mask=0x%x, newMask=0x%x\n"),
                 FUNCTION_NAME(), oldMask, mask, newMask));
    /* In polled mode, the hardware IRQ mask is always zero */
    if (SOC_CONTROL(unit)->soc_flags & SOC_F_POLLED) {
        newMask = 0;
    }
    soc_pci_write(unit, CMIC_IRQ_MASK_1, newMask);
    sal_spl(s);
    }
}
#endif /* BCM_DFE_SUPPORT */
    return oldMask;
}

uint32
soc_intr_block_lo_disable(int unit, uint32 mask)
{
    uint32 oldMask = 0;

#ifdef BCM_DFE_SUPPORT
{
    uint32 newMask;
    int s;

    if (SOC_IS_DFE(unit)) {
    s = sal_splhi();
    oldMask = SOC_IRQ1_MASK(unit);
    SOC_IRQ1_MASK(unit) &= ~mask;
    newMask = SOC_IRQ1_MASK(unit);
    LOG_VERBOSE(BSL_LS_SOC_INTR,
                (BSL_META("%s(): oldMask=0x%x, mask=0x%x, newMask=0x%x\n"),
                 FUNCTION_NAME(), oldMask, mask, newMask));
    /* In polled mode, the hardware IRQ mask is always zero */
    if (SOC_CONTROL(unit)->soc_flags & SOC_F_POLLED) {
        newMask = 0;
    }
    soc_pci_write(unit, CMIC_IRQ_MASK_1, newMask);
    sal_spl(s);
    }
}
#endif /* BCM_DFE_SUPPORT */
    return oldMask;
}

uint32
soc_intr_block_hi_enable(int unit, uint32 mask)
{
    uint32 oldMask = 0;

#ifdef BCM_DFE_SUPPORT
{
    uint32 newMask;
    int s;

    if (SOC_IS_DFE(unit)) {
    s = sal_splhi();
    oldMask = SOC_IRQ2_MASK(unit);
    SOC_IRQ2_MASK(unit) |= mask;
    newMask = SOC_IRQ2_MASK(unit);
    LOG_VERBOSE(BSL_LS_SOC_INTR,
                (BSL_META("%s(): oldMask=0x%x, mask=0x%x, newMask=0x%x\n"),
                 FUNCTION_NAME(), oldMask, mask, newMask));
    /* In polled mode, the hardware IRQ mask is always zero */
    if (SOC_CONTROL(unit)->soc_flags & SOC_F_POLLED) {
        newMask = 0;
    }
    soc_pci_write(unit, CMIC_IRQ_MASK_2, newMask);
    sal_spl(s);
    }
}
#endif /* BCM_DFE_SUPPORT */

    return oldMask;
}

uint32
soc_intr_block_hi_disable(int unit, uint32 mask)
{
    uint32 oldMask = 0;

#ifdef BCM_DFE_SUPPORT
{
    uint32 newMask;
    int s;

    if (SOC_IS_DFE(unit)) {
    s = sal_splhi();
    oldMask = SOC_IRQ2_MASK(unit);
    SOC_IRQ2_MASK(unit) &= ~mask;
    newMask = SOC_IRQ2_MASK(unit);
    LOG_VERBOSE(BSL_LS_SOC_INTR,
                (BSL_META("%s(): oldMask=0x%x, mask=0x%x, newMask=0x%x\n"),
                 FUNCTION_NAME(), oldMask, mask, newMask));
    /* In polled mode, the hardware IRQ mask is always zero */
    if (SOC_CONTROL(unit)->soc_flags & SOC_F_POLLED) {
        newMask = 0;
    }
    soc_pci_write(unit, CMIC_IRQ_MASK_2, newMask);
    sal_spl(s);
    }
}
#endif /* BCM_DFE_SUPPORT */
    return oldMask;
}

/*
 * SOC Interrupt Service Routine
 *
 *   In PLI simulation, the intr thread can call this routine at any
 *   time.  The connection is protected at the level of pli_{set/get}reg.
 */

#define POLL_LIMIT 100000

void
soc_intr(void *_unit)
{
    uint32         irqStat, irqMask;
    soc_control_t    *soc;
    int         i = 0;
    int         poll_limit = POLL_LIMIT;
    int                 unit = PTR_TO_INT(_unit);
    int s;

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


    soc->stat.intr++;        /* Update count */

    /*
     * Read IRQ Status and IRQ Mask and AND to determine active ints.
     * These are re-read each time since either can be changed by ISRs.
     *
     * Since interrupts are edge-driven, it's necessary to continue
     * processing them until the IRQ_STAT register reads zero.  If we
     * return without doing that, we may never see another interrupt!
     */
    for (;;) {
    irqStat = soc_pci_read(unit, CMIC_IRQ_STAT);
    if (irqStat == 0) {
        break;
    }
    irqMask = SOC_IRQ_MASK(unit);
    irqStat &= irqMask;
    if (irqStat == 0) {
        break;
    }

    /*
     * find starting point for handler search
     * skip over blocks of high-priority but unlikely entries
     */
    if ((irqStat & INTR_START2_MASK) == 0) {
        i = INTR_START2_POS;
    } else if ((irqStat & INTR_START1_MASK) == 0) {
        i = INTR_START1_POS;
    } else {
        i = 0;
    }

        /*
         * We may have received an interrupt before all data has been
         * posted from the device or intermediate bridge. 
         * The PCI specification requires that we read a device register
         * to make sure pending data is flushed. 
         * Some bridges (we have determined through testing) require more
         * than one read.
         */
        soc_pci_read(unit, CMIC_SCHAN_CTRL); 
        soc_pci_read(unit, CMIC_IRQ_MASK); 

    for (; i < INTR_HANDLERS_COUNT; i++) {
        if (irqStat & soc_intr_handlers[i].mask) {

        /*
         * Bit found, dispatch interrupt
         */

        LOG_INFO(BSL_LS_SOC_INTR,
                 (BSL_META_U(unit,
                             "soc_intr unit %d: dispatch %s\n"),
                  unit, soc_intr_handlers[i].intr_name));

        (*soc_intr_handlers[i].intr_fn)
            (unit, soc_intr_handlers[i].intr_data);

        /*
         * Prevent infinite loop in interrupt handler by
         * disabling the offending interrupt(s).
         */

        if (--poll_limit == 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "soc_intr unit %d: "
                                  "ERROR can't clear interrupt(s): "
                                  "IRQ=0x%x (disabling 0x%x)\n"),
                       unit, irqStat, soc_intr_handlers[i].mask));
            soc_intr_disable(unit, soc_intr_handlers[i].mask);
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

    if (soc_feature(unit, soc_feature_extended_cmic_error)) {
    /* process block specific interrupts for block 0 - 31 */
    for (;;) {
        irqStat = soc_pci_read(unit, CMIC_IRQ_STAT_1);
        if (irqStat == 0) {
        break;
        }
        irqMask = SOC_IRQ1_MASK(unit);
        irqStat &= irqMask;
        if (irqStat == 0) {
        break;
        }
        
        /*
         * We may have received an interrupt before all data has been
         * posted from the device or intermediate bridge. 
         * The PCI specification requires that we read a device register
         * to make sure pending data is flushed. 
         * Some bridges (we have determined through testing) require more
         * than one read.
         */
        soc_pci_read(unit, CMIC_SCHAN_CTRL); 
        soc_pci_read(unit, CMIC_IRQ_MASK_1); 
        
        for (i=0 ; i < INTR_BLOCK_LO_HANDLERS_COUNT; i++) {
        if (irqStat & soc_intr_block_lo_handlers[i].mask) {
            
            /*
             * Bit found, dispatch interrupt
             */
            
            LOG_INFO(BSL_LS_SOC_INTR,
                     (BSL_META_U(unit,
                                 "soc_intr unit %d: dispatch %s\n"),
                      unit, soc_intr_block_lo_handlers[i].intr_name));
            
            (*soc_intr_block_lo_handlers[i].intr_fn)
            (unit, soc_intr_block_lo_handlers[i].intr_data);
            
            /*
             * Prevent infinite loop in interrupt handler by
             * disabling the offending interrupt(s).
             */
            
            if (--poll_limit == 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "soc_intr unit %d: "
                                  "ERROR can't clear interrupt(s): "
                                  "IRQ=0x%x (disabling 0x%x)\n"),
                       unit, irqStat, soc_intr_block_lo_handlers[i].mask));
            soc_intr_block_lo_disable(unit, soc_intr_block_lo_handlers[i].mask);
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
    
    /* process block specific interrupts for block 32 - 63 */
    for (;;) {
        irqStat = soc_pci_read(unit, CMIC_IRQ_STAT_2);
        if (irqStat == 0) {
        break;
        }
        irqMask = SOC_IRQ2_MASK(unit);
        irqStat &= irqMask;
        if (irqStat == 0) {
        break;
        }
        
        /*
         * We may have received an interrupt before all data has been
         * posted from the device or intermediate bridge. 
         * The PCI specification requires that we read a device register
         * to make sure pending data is flushed. 
         * Some bridges (we have determined through testing) require more
         * than one read.
         */
        soc_pci_read(unit, CMIC_SCHAN_CTRL); 
        soc_pci_read(unit, CMIC_IRQ_MASK_2); 
        
        for (i=0; i < INTR_BLOCK_HI_HANDLERS_COUNT; i++) {
        if (irqStat & soc_intr_block_hi_handlers[i].mask) {
            
            /*
             * Bit found, dispatch interrupt
             */
            
            LOG_INFO(BSL_LS_SOC_INTR,
                     (BSL_META_U(unit,
                                 "soc_intr unit %d: dispatch %s\n"),
                      unit, soc_intr_block_hi_handlers[i].intr_name));
            
            (*soc_intr_block_hi_handlers[i].intr_fn)
            (unit, soc_intr_block_hi_handlers[i].intr_data);
            
            /*
             * Prevent infinite loop in interrupt handler by
             * disabling the offending interrupt(s).
             */
            
            if (--poll_limit == 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "soc_intr unit %d: "
                                  "ERROR can't clear interrupt(s): "
                                  "IRQ=0x%x (disabling 0x%x)\n"),
                       unit, irqStat, soc_intr_block_hi_handlers[i].mask));
            soc_intr_block_hi_disable(unit, soc_intr_block_hi_handlers[i].mask);
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
    }

    if (soc_feature(unit, soc_feature_short_cmic_error)) {
        /* Using sal_dpc since there are schan reads in this function
         * and schan read cant be done from interrupt context.
         * the function soc_cmn_error will be excecuted only after this 
         * function will end.
         */
        sal_dpc(soc_cmn_error, INT_TO_PTR(unit), 0, 0, 0, 0);
    }

    /* In polled mode, the hardware IRQ mask is always zero */
    if (SOC_CONTROL(unit)->soc_flags & SOC_F_POLLED) {
#ifdef SAL_SPL_LOCK_ON_IRQ
        sal_spl(s);
#endif
        return;
    }

#ifndef SAL_SPL_LOCK_ON_IRQ
    s = sal_splhi();
#endif

    /*
     * If the interrupt handler is not run in interrupt context, but 
     * rather as a thread or a signal handler, the interrupt handler 
     * must reenable interrupts on the switch controller. Currently
     * we don't distinguish between the two modes of operation, so 
     * we always reenable interrupts here.
     */
    IRQ_MASK_SET(unit, CMIC_IRQ_MASK, SOC_IRQ_MASK(unit));
    if (soc_feature(unit, soc_feature_extended_cmic_error)) {
        soc_pci_write(unit, CMIC_IRQ_MASK_1, SOC_IRQ1_MASK(unit));
        soc_pci_write(unit, CMIC_IRQ_MASK_2, SOC_IRQ2_MASK(unit));
    }

    sal_spl(s);
}

void soc_cmn_block_error(void *unit_vp, void *d1, void *d2, void *d3, void *d4)
{
    int blk, rc = SOC_E_NONE, is_valid, idx, i, nof_interrupts, is_enabled;
    soc_block_info_t* bi = NULL;
    soc_interrupt_db_t* interrupt, *prev_interrupt = NULL;
    int unit = PTR_TO_INT(unit_vp); /*unit should be set before SOC_INIT_FUNC_DEFS*/
    soc_reg_above_64_val_t data, field;
    int interrupt_action;

    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "No interrupts for device\n")));
        return;
    }

    blk = PTR_TO_INT(d1);

    LOG_VERBOSE(BSL_LS_SOC_INTR,
                (BSL_META_U(unit,
                            "enter \n")));

    for (i = 0; SOC_BLOCK_INFO(unit, i).type >= 0; i++) {
        if (SOC_INFO(unit).block_valid[i] && SOC_BLOCK2SCH(unit,i) == blk) {
            bi = &(SOC_BLOCK_INFO(unit, i));
            break;
        }
    }

    if (NULL == bi) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Interrupt for unknown block %d\n"),
                   blk));
        return;
    }
        /* Coverity : 21737 */                            
    if (NULL == SOC_CONTROL(unit)->interrupts_info->interrupt_db_info) {
        LOG_ERROR(BSL_LS_SOC_INTR,                
                       (BSL_META_U(unit, "No interrupts for device\n")));
             return;                                  
    }
    soc_nof_interrupts(unit, &nof_interrupts);

    for (idx = 0; idx < nof_interrupts; idx++) {
        /* Advance to next interrupt */
        interrupt = &SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[idx];

        rc = soc_interrupt_is_valid(unit, bi, interrupt, &is_valid);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            return;
        }
        if(!is_valid) {
            continue;
        }

        rc = soc_interrupt_is_enabled(unit, bi->number, interrupt, &is_enabled);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            return;
        }
        if (!is_enabled) {
            continue;
        }

        interrupt_action = 0;
        if ((NULL != prev_interrupt) && (prev_interrupt->reg == interrupt->reg) && (prev_interrupt->reg_index == interrupt->reg_index)) {
            soc_reg_above_64_field_get(unit, interrupt->reg, data, interrupt->field, field);
        } else {
            rc = soc_reg_above_64_get(unit, interrupt->reg, bi->number, interrupt->reg_index, data);
            if(SOC_FAILURE(rc)) { 
                prev_interrupt = NULL;
                LOG_ERROR(BSL_LS_SOC_INTR,
                          (BSL_META_U(unit,
                                      "%s\n"), soc_errmsg(rc)));
                return;
           }

           prev_interrupt = interrupt;
           soc_reg_above_64_field_get(unit, interrupt->reg, data, interrupt->field, field);
        }

        if (!SOC_REG_ABOVE_64_IS_ZERO(field)) {
            if(interrupt->bit_in_field!= SOC_INTERRUPT_BIT_FIELD_DONT_CARE ) {
                interrupt_action  = SHR_BITGET(field, interrupt->bit_in_field);
            } else {
              interrupt_action = 1;
            }
         }

          /* CallBack */
          if(interrupt_action) {
              soc_event_generate(unit, SOC_SWITCH_EVENT_DEVICE_INTERRUPT, idx, bi->number, 0);
          }
    }

    if (blk < 32) {
        soc_intr_block_lo_enable(unit, (1<<blk));
    } else {
        soc_intr_block_hi_enable(unit, (1<<(blk-32)));
    }
}

void soc_cmn_error(void *unit_vp, void *d1, void *d2, void *d3, void *d4)
{
    int rc = SOC_E_NONE, i;
    int unit = PTR_TO_INT(unit_vp);
    int flags = 0;
    int max_interrupts_size = INTR_CMN_ERROR_MAX_INTERRUPTS_SIZE;
    soc_interrupt_cause_t interrupts[INTR_CMN_ERROR_MAX_INTERRUPTS_SIZE];
    int total_interrupts = 0;
    int interrupt_num = INTR_CMN_ERROR_MAX_INTERRUPTS_SIZE;

    sal_memset(interrupts, 0x0, INTR_CMN_ERROR_MAX_INTERRUPTS_SIZE * sizeof(soc_interrupt_cause_t));

    /* Get all current Active interrupts */
    flags = SOC_ACTIVE_INTERRUPTS_GET_UNMASKED_ONLY; 
    rc = soc_active_interrupts_get(unit, flags, max_interrupts_size, interrupts, &total_interrupts);
    if (SOC_FAILURE(rc)) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rc)));
        return;
    }

    LOG_VERBOSE(BSL_LS_SOC_INTR,
                (BSL_META_U(unit,
                            "interrupt_num=%d, max_interrupts_size=%d, total_interrupts=%d\n"),
                 interrupt_num, max_interrupts_size, total_interrupts));

    if (interrupt_num > total_interrupts) {
        interrupt_num = total_interrupts;
    }

    /* sort interrupts according to priority */
    if(interrupt_num > 1) {
        rc = soc_sort_interrupts_according_to_priority(unit, interrupts, interrupt_num);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            return;
        }
    }

    /* Call CB for every Active interrupt */
    for (i = 0; i < interrupt_num; i++) {
#if defined(BCM_JERICHO_SUPPORT) || defined(BCM_88950_SUPPORT) 
        uint32 interrupt_flags;
        /* at Jericho & FE3200 new interrupt mechanism applied, we need to call bcm cb (dcmn_intr_switch_event_cb) and also to user cb only if applicable */
        if (SOC_IS_JERICHO(unit) || SOC_IS_FE3200(unit)) {
            dcmn_intr_switch_event_cb(unit, SOC_SWITCH_EVENT_DEVICE_INTERRUPT, interrupts[i].id, interrupts[i].index, 0, 0);
            rc = soc_interrupt_flags_get(unit, interrupts[i].id, &interrupt_flags);
            if (SOC_FAILURE(rc)) {
                LOG_ERROR(BSL_LS_SOC_INTR, (BSL_META_U(unit, "%s\n"), soc_errmsg(rc)));
                return;
            }
            if (((interrupt_flags & SOC_INTERRUPT_DB_FLAGS_BCM_AND_USR_CB) == 0) && (SHR_BITGET(&interrupt_flags, SOC_INTERRUPT_DB_FLAGS_CORR_ACT_OVERRIDE_ENABLE) == 0)) {
                continue;
            }
        }
#endif
        soc_event_generate(unit, SOC_SWITCH_EVENT_DEVICE_INTERRUPT, interrupts[i].id, interrupts[i].index, 0);
    }

    /* Enable interrups */
    if (soc_feature(unit, soc_feature_cmicm)) {
        if (!(SOC_CONTROL(unit)->soc_flags & SOC_F_POLLED) && (SOC_IS_ARAD(unit) || SOC_IS_FE3200(unit))) {
#ifdef BCM_CMICM_SUPPORT
            int cmc = SOC_PCI_CMC(unit);
            soc_pci_write(unit, CMIC_CMCx_PCIE_IRQ_MASK3_OFFSET(cmc), SOC_CMCx_IRQ3_MASK(unit, cmc));
            soc_pci_write(unit, CMIC_CMCx_PCIE_IRQ_MASK4_OFFSET(cmc), SOC_CMCx_IRQ4_MASK(unit, cmc));
            if (soc_feature(unit, soc_feature_cmicm_extended_interrupts)) {
                soc_pci_write(unit, CMIC_CMCx_PCIE_IRQ_MASK5_OFFSET(cmc), SOC_CMCx_IRQ5_MASK(unit, cmc));
                soc_pci_write(unit, CMIC_CMCx_PCIE_IRQ_MASK6_OFFSET(cmc), SOC_CMCx_IRQ6_MASK(unit, cmc));
            } else {
                soc_pci_write(unit, CMIC_CMCx_PCIE_IRQ_MASK2_OFFSET(cmc), SOC_CMCx_IRQ2_MASK(unit, cmc));
            }
#endif
        }
    } else {
        soc_pci_write(unit, CMIC_IRQ_MASK_1, SOC_IRQ1_MASK(unit));
        soc_pci_write(unit, CMIC_IRQ_MASK_2, SOC_IRQ2_MASK(unit));
    }

#ifdef PLISIM
    if (SAL_BOOT_PLISIM) {
        /* Turn off Interrupts in PCID - to avoid endless loop */
        if (soc_feature(unit, soc_feature_cmicm)) {
            if (SOC_IS_ARAD(unit)) {
#ifdef BCM_CMICM_SUPPORT
                int cmc = SOC_PCI_CMC(unit);
                soc_pci_write(unit, CMIC_CMCx_IRQ_STAT2_OFFSET(cmc), 0x0);
                soc_pci_write(unit, CMIC_CMCx_IRQ_STAT4_OFFSET(cmc), 0x0);
                soc_pci_write(unit, CMIC_CMCx_IRQ_STAT3_OFFSET(cmc), 0x0);
#endif
            }
        } else {
            soc_pci_write(unit, CMIC_IRQ_STAT_1, 0x0);
            soc_pci_write(unit, CMIC_IRQ_STAT_2, 0x0);
        }
    }
#endif
}

int soc_interrupt_is_valid(int unit, const soc_block_info_t* bi, const soc_interrupt_db_t* inter, int* is_valid /*out*/)
{
    if (bi == NULL || inter == NULL || is_valid == NULL) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Null parameter\n")));
        return SOC_E_PARAM;
    }

    if(!SOC_REG_IS_VALID(unit, inter->reg)){
        *is_valid = 0;
    } else {
        if(SOC_BLOCK_IN_LIST(unit, SOC_REG_INFO(unit,inter->reg).block, bi->type)) {
            *is_valid = 1;
        } else {
            *is_valid = 0;
        }
    }

    return SOC_E_NONE;
}

/*
 */
int soc_interrupt_get(int unit, int block_instance , const soc_interrupt_db_t* inter, int* inter_val /*out*/)
{
    soc_reg_above_64_val_t data, field, field_mask;
    int rc = SOC_E_NONE;

    if (inter == NULL || inter_val == NULL) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Null parameter\n")));
        return SOC_E_PARAM;
    }

    if(!SOC_REG_IS_VALID(unit, inter->reg)){
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Invalid register for the device\n")));
        return SOC_E_INTERNAL;
    }

    rc = soc_reg_above_64_get(unit, inter->reg, block_instance, inter->reg_index, data);
    if (SOC_FAILURE(rc)) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rc)));
        return rc;
    }

    soc_reg_above_64_field_get(unit, inter->reg, data, inter->field, field);

    if (inter->bit_in_field != SOC_INTERRUPT_BIT_FIELD_DONT_CARE) {
        SOC_REG_ABOVE_64_CREATE_MASK(field_mask, 0x1, inter->bit_in_field);
        SOC_REG_ABOVE_64_AND(field, field_mask);
    }

    *inter_val = (SOC_REG_ABOVE_64_IS_ZERO(field) ? 0x0 : 0x1);

    return rc;
}

/*
 * Function:
 *    soc_interrupt_force_get
 * Description:
 *    Set/Clear interrupt test registers bits & appropriate mask register
 * Parameters:
 *  unit        - Device unit number
 *  block_instance   - block_instance
 *  inter            - interrupt info
 *  *inter_val        - return value
 * Returns:
 *      BCM_E_xxx
 */  
int soc_interrupt_force_get(int unit, int block_instance, const soc_interrupt_db_t* inter, int* inter_val)
{
#if defined(BCM_SAND_SUPPORT)
    soc_reg_above_64_val_t data , field_mask;
    soc_field_info_t *finfop;
    int rc = SOC_E_NONE;
    int curr_bit;

    if (inter == NULL) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Null parameter\n")));
        return SOC_E_PARAM;
    }

    SOC_REG_ABOVE_64_CLEAR(data);

    *inter_val = 0; /* When force is not supported */

    if (inter->reg_test != INVALIDr && inter->vector_id != 1) {
        if(!SOC_REG_IS_VALID(unit, inter->mask_reg)){
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "Invalid mask register for the device\n")));
            return SOC_E_INTERNAL;
        }

        rc = soc_reg_above_64_get(unit, inter->reg_test, block_instance, inter->mask_reg_index, data);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            return rc;
        }

        SOC_FIND_FIELD(inter->field,
                       SOC_REG_INFO(unit, inter->reg).fields,
                       SOC_REG_INFO(unit, inter->reg).nFields,
                       finfop);
        if (finfop == NULL) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "Invalid Field Name for the event\n")));
            return SOC_E_INTERNAL;
        }

        curr_bit = finfop->bp;
        if (inter->bit_in_field != SOC_INTERRUPT_BIT_FIELD_DONT_CARE) {
            curr_bit += inter->bit_in_field;
        }

        SOC_REG_ABOVE_64_CREATE_MASK(field_mask, 0x1, curr_bit);
        SOC_REG_ABOVE_64_AND(data, field_mask);
        *inter_val = (SOC_REG_ABOVE_64_IS_ZERO(data) ? 0x0 : 0x1);
    }


    return rc;
#else
    return 0;
#endif
}

int soc_interrupt_enable(int unit, int block_instance, const soc_interrupt_db_t* inter)
{
    soc_reg_above_64_val_t data, field;
    int rc = SOC_E_NONE;

    if (inter == NULL) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Null parameter\n")));
        return SOC_E_PARAM;
    }

    if(!SOC_REG_IS_VALID(unit, inter->mask_reg)){
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Invalid mask register for the device\n")));
        return SOC_E_INTERNAL;
    }

    rc = soc_reg_above_64_get(unit, inter->mask_reg, block_instance, inter->mask_reg_index, data);
    if (SOC_FAILURE(rc)) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rc)));
        return rc;
    }

    if (inter->bit_in_field == SOC_INTERRUPT_BIT_FIELD_DONT_CARE) {
        SOC_REG_ABOVE_64_CLEAR(field);
        SHR_BITSET(field, 0x0);
    } else {
        soc_reg_above_64_field_get(unit, inter->mask_reg, data, inter->mask_field, field);
        SHR_BITSET(field, inter->bit_in_field);
    }

    soc_reg_above_64_field_set(unit, inter->mask_reg, data, inter->mask_field, field);

    rc = soc_reg_above_64_set(unit, inter->mask_reg, block_instance,  inter->mask_reg_index, data);
    if (SOC_FAILURE(rc)) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rc)));
    }

    return rc;
}

/*
 * Function:
 *    soc_interrupt_force
 * Description:
 *    Set/Clear interrupt test registers bits & appropriate mask register
 * Parameters:
 *  unit        - Device unit number
 *  block_instance   - block_instance
 *  inter            - interrupt info
 *  action           - action to do - 0 enable force interrupts, 1 - disable it
 * Returns:
 *      BCM_E_xxx
 */ 
int soc_interrupt_force(int unit, int block_instance, const soc_interrupt_db_t* inter, int action)
{
#if defined(BCM_SAND_SUPPORT)
    soc_reg_above_64_val_t data; /* , field; */
    soc_field_info_t *finfop;
    int rc = SOC_E_NONE;
    int curr_bit;

    if (inter == NULL) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Null parameter\n")));
        return SOC_E_PARAM;
    }

    if (inter->reg_test != INVALIDr && inter->vector_id != 1) {
        if(!SOC_REG_IS_VALID(unit, inter->mask_reg)){
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "Invalid mask register for the device\n")));
            return SOC_E_INTERNAL;
        }

        rc = soc_reg_above_64_get(unit, inter->reg_test, block_instance, inter->reg_index, data);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            return rc;
        }

        SOC_FIND_FIELD(inter->field,
                       SOC_REG_INFO(unit, inter->reg).fields,
                       SOC_REG_INFO(unit, inter->reg).nFields,
                       finfop);
        if (finfop == NULL) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "Invalid Field Name for the event\n")));
            return SOC_E_INTERNAL;
        }

        curr_bit = finfop->bp;
        if (inter->bit_in_field != SOC_INTERRUPT_BIT_FIELD_DONT_CARE) {
            curr_bit += inter->bit_in_field;
        }


        if (action == 0) { /* enable force*/
           /* SOC_REG_ABOVE_64_CLEAR(data); */
            SHR_BITSET(data, curr_bit); 
        } else if (action == 1){ /* disable force*/
            SHR_BITCLR(data, curr_bit);
        } else {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "Invalid action\n")));
            return SOC_E_INTERNAL;
        }

        rc = soc_reg_above_64_set(unit, inter->reg_test, block_instance,  inter->reg_index, data);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            return rc;
        }
    }

    return rc;

#else
    return 0;
#endif
}

int soc_interrupt_disable(int unit, int block_instance, const soc_interrupt_db_t* inter)
{
    soc_reg_above_64_val_t data, field;
    int rc = SOC_E_NONE;

    if (inter == NULL) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Null parameter\n")));
        return SOC_E_PARAM;
    }

    if(!SOC_REG_IS_VALID(unit, inter->mask_reg)){
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Invalid mask register for the device\n")));
        return SOC_E_INTERNAL;
    }

    rc = soc_reg_above_64_get(unit, inter->mask_reg, block_instance, inter->mask_reg_index, data);
    if (SOC_FAILURE(rc)) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rc)));
        return rc;
    }

    if (inter->bit_in_field == SOC_INTERRUPT_BIT_FIELD_DONT_CARE) {
        SOC_REG_ABOVE_64_CLEAR(field);
    } else {
        soc_reg_above_64_field_get(unit, inter->mask_reg, data, inter->mask_field, field);
        SHR_BITCLR(field, inter->bit_in_field);
    }

    soc_reg_above_64_field_set(unit, inter->mask_reg, data, inter->mask_field, field);

    rc = soc_reg_above_64_set(unit, inter->mask_reg, block_instance,  inter->mask_reg_index, data);
    if (SOC_FAILURE(rc)) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rc)));
    }

    return rc;
}

int soc_interrupt_is_enabled(int unit, int block_instance, const soc_interrupt_db_t* inter, int* is_enabled /*out*/)
{
    soc_reg_above_64_val_t data, field, field_mask;
    int rc = SOC_E_NONE;

    if (inter == NULL) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Null parameter\n")));
        return SOC_E_PARAM;
    }

    if(!SOC_REG_IS_VALID(unit, inter->mask_reg)){
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Invalid mask register for the device\n")));
        return SOC_E_INTERNAL;
    }

    rc = soc_reg_above_64_get(unit, inter->mask_reg, block_instance, inter->mask_reg_index, data);
    if (SOC_FAILURE(rc)) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rc)));
        return rc;
    }

    soc_reg_above_64_field_get(unit, inter->mask_reg, data, inter->mask_field, field);

    if (inter->bit_in_field != SOC_INTERRUPT_BIT_FIELD_DONT_CARE) {
        SOC_REG_ABOVE_64_CREATE_MASK(field_mask, 0x1, inter->bit_in_field);
        SOC_REG_ABOVE_64_AND(field, field_mask);
    }

    *is_enabled = (SOC_REG_ABOVE_64_IS_ZERO(field) ? 0x0 : 0x1);

    return rc;
}

int soc_interrupt_is_supported(int unit, int block_instance, int inter_id)
{
    int rc;
    int is_block_valid;
    int nof_interrupts;
    soc_interrupt_db_t *inter, *interrupts;
    soc_block_types_t block_types;
    int blk;
#ifdef BCM_DNX_SUPPORT
    int is_support;
#endif

    interrupts = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info;
    if (interrupts == NULL) {
        return FALSE;
    }

    /* verify interrupt_id */
    soc_nof_interrupts(unit, &nof_interrupts);
    if ((inter_id > nof_interrupts) || inter_id < 0) {
        return FALSE;
    }

    inter = &(interrupts[inter_id]);
    if (!SOC_REG_IS_VALID(unit, inter->reg)) {
        return FALSE; 
    }

    if (inter->vector_id) {
        /* it is vector */
        return FALSE;
    }

#ifdef BCM_DNX_SUPPORT
    if(SOC_IS_DNX(unit)) {
        rc = soc_dnx_is_interrupt_support(unit, inter_id, &is_support);
        if(SOC_FAILURE(rc) || !is_support) {
            return FALSE;
        }
    }
#endif

    block_types = SOC_REG_INFO(unit, inter->reg).block;

    rc = soc_is_valid_block_instance(unit, block_types, block_instance, &is_block_valid);
    if(SOC_FAILURE(rc) || !is_block_valid) {
        return FALSE;
    }

    SOC_BLOCK_ITER(unit, blk, *block_types) {
        if (SOC_BLOCK_INFO(unit, blk).number == block_instance) {
            return TRUE;
        }
    }

    return FALSE;
}


static int soc_interrupt_valid(int unit, int block_instance, int inter_id)
{
    int rc;
    int is_block_valid;
    int nof_interrupts;
    soc_interrupt_db_t *inter, *interrupts;
    soc_block_types_t block_types;
    int blk;

    interrupts = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info;
    if (interrupts == NULL) {
        return FALSE;
    }

    /* verify interrupt_id */
    soc_nof_interrupts(unit, &nof_interrupts);
    if ((inter_id > nof_interrupts) || inter_id < 0) {
        return FALSE;
    }

    inter = &(interrupts[inter_id]);
    if (!SOC_REG_IS_VALID(unit, inter->reg)) {
        return FALSE; 
    }
    block_types = SOC_REG_INFO(unit, inter->reg).block;

    rc = soc_is_valid_block_instance(unit, block_types, block_instance, &is_block_valid);
    if(SOC_FAILURE(rc) || !is_block_valid) {
        return FALSE;
    }

    SOC_BLOCK_ITER(unit, blk, *block_types) {
        if (SOC_BLOCK_INFO(unit, blk).number == block_instance) {
            return TRUE;
        }
    }

    return FALSE;
}


/*
 * Interrupt corrective action function get
 */
int soc_interrupt_has_func_arr(int unit, int interrupt_id)
{
    int nof_interrupts;
    soc_interrupt_db_t *inter, *interrupts;

    interrupts = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info;
    if (interrupts == NULL) {
        return FALSE;
    }

    /* verify interrupt_id */
    soc_nof_interrupts(unit, &nof_interrupts);
    if ((interrupt_id > nof_interrupts) || interrupt_id < 0) {
        return FALSE;
    }

    inter = &(interrupts[interrupt_id]);
    if (!SOC_REG_IS_VALID(unit, inter->reg) || (inter->vector_id)) {
        return FALSE;
    }

    if(inter->func_arr != NULL)
    {
        return TRUE;
    }

    return FALSE;
}

int
soc_interrupt_event_cb_none(
    int unit,
    soc_switch_event_t event,
    uint32 arg1,
    uint32 arg2,
    uint32 arg3,
    void *data)
{
    int nof_interrupts;
    /* Process only corrected events */
    if (SOC_SWITCH_EVENT_DEVICE_INTERRUPT != event) {
        LOG_ERROR(BSL_LS_SOC_INTR, (BSL_META_U(unit, "Event must be BCM_SWITCH_EVENT_DEVICE_INTERRUPT\n")));
        return -1;
    }
    /* verify interrupt_id */
    soc_nof_interrupts(unit, &nof_interrupts);
    if (nof_interrupts <= 0)
    {
        LOG_ERROR(BSL_LS_SOC_INTR, (BSL_META_U(unit, "Cannot get correct nof interrupts\n")));
        return -1;
    }

    if (arg1 > (uint32)nof_interrupts) {
        LOG_ERROR(BSL_LS_SOC_INTR, (BSL_META_U(unit, "Interrupt %d not valid\n"), arg1));
        return -1;
    }
    return 0;
}


/* 
 * Interrupt Clear Functions
 */
int soc_interrupt_clear_on_write(int unit, int block_instance, int interrupt_id)
{
    soc_reg_above_64_val_t data, field;
    int rc = SOC_E_NONE;
    int nof_interrupts;
    soc_interrupt_db_t *inter, *interrupts;

    if (!SOC_INTR_IS_SUPPORTED(unit)) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "No interrupts for device\n")));
        return SOC_E_UNAVAIL;
    }

    interrupts = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info;
    if (interrupts == NULL) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Null parameter\n")));
        return SOC_E_PARAM;
    }

    /*verify interrupt_id*/
    soc_nof_interrupts(unit, &nof_interrupts);
    if ((interrupt_id > nof_interrupts) || interrupt_id < 0) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "event_id is unavail\n")));
        return SOC_E_UNAVAIL;
    }

    inter = &(interrupts[interrupt_id]);
    if (!SOC_REG_IS_VALID(unit, inter->reg)) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Invalid register for the device\n")));
        return SOC_E_INTERNAL;
    }

    if (!SOC_REG_IS_VALID(unit, inter->mask_reg)) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Invalid mask register for the device\n")));
        return SOC_E_INTERNAL;
    }

    SOC_REG_ABOVE_64_CLEAR(data);
    SOC_REG_ABOVE_64_CLEAR(field);

    if (inter->bit_in_field == SOC_INTERRUPT_BIT_FIELD_DONT_CARE) {
        SHR_BITSET(field, 0x0);
    } else {
        SHR_BITSET(field, inter->bit_in_field);
    }

    soc_reg_above_64_field_set(unit, inter->reg, data, inter->field, field);

    rc = soc_reg_above_64_set(unit, inter->reg, block_instance,  inter->reg_index, data);
    if (SOC_FAILURE(rc)) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rc)));
    }

    return rc;
}

int soc_interrupt_clear_on_reg_write(int unit, int block_instance, int interrupt_id)
{
    int rc = SOC_E_NONE;
    int nof_interrupts;
    soc_interrupt_db_t *inter, *interrupts;

    if (!SOC_INTR_IS_SUPPORTED(unit)) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "No interrupts for device\n")));
        return SOC_E_UNAVAIL;
    }

    interrupts = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info;
    if (interrupts == NULL) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Null parameter\n")));
        return SOC_E_PARAM;
    }

    /*verify interrupt_id*/
    soc_nof_interrupts(unit, &nof_interrupts);
    if ((interrupt_id > nof_interrupts) || interrupt_id < 0) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "event_id is unavail\n")));
        return SOC_E_UNAVAIL;
    }

    inter = &(interrupts[interrupt_id]);
    if(!SOC_REG_IS_VALID(unit, ((soc_interrupt_clear_reg_write_t*)inter->interrupt_clear_param1)->status_reg)){
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Invalid register for the device\n")));
        return SOC_E_INTERNAL;
    }

    rc = soc_reg_above_64_set(unit, 
                              ((soc_interrupt_clear_reg_write_t*)inter->interrupt_clear_param1)->status_reg, 
                              block_instance,  
                              inter->reg_index, 
                              ((soc_interrupt_clear_reg_write_t*)inter->interrupt_clear_param1)->data);
    if (SOC_FAILURE(rc)) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rc)));
    }

    return rc;
}

int soc_interrupt_clear_on_clear(int unit, int block_instance, int interrupt_id)
{
    soc_reg_above_64_val_t data, field;
    int rc = SOC_E_NONE;
    int nof_interrupts;
    soc_interrupt_db_t *inter, *interrupts;

    if (!SOC_INTR_IS_SUPPORTED(unit)) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "No interrupts for device\n")));
        return SOC_E_UNAVAIL;
    }

    interrupts = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info;
    if (interrupts == NULL) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Null parameter\n")));
        return SOC_E_PARAM;
    }

    /*verify interrupt_id*/
    soc_nof_interrupts(unit, &nof_interrupts);
    if ((interrupt_id > nof_interrupts) || interrupt_id < 0) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "event_id is unavail\n")));
        return SOC_E_UNAVAIL;
    }

    inter = &(interrupts[interrupt_id]);
    if(!SOC_REG_IS_VALID(unit, inter->reg)){
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Invalid register for the device\n")));
        return SOC_E_INTERNAL;
    }
    if(!SOC_REG_IS_VALID(unit, inter->mask_reg)){
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Invalid mask register for the device\n")));
        return SOC_E_INTERNAL;
    }

    SOC_REG_ABOVE_64_CLEAR(data);
    SOC_REG_ABOVE_64_CLEAR(field);

    rc = soc_reg_above_64_get(unit, inter->reg, block_instance, inter->reg_index, data);
    if (SOC_FAILURE(rc)) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rc)));
        return rc;
    }

    if (inter->bit_in_field != SOC_INTERRUPT_BIT_FIELD_DONT_CARE) {
        soc_reg_above_64_field_get(unit, inter->reg, data, inter->field, field);
        SHR_BITCLR(field, inter->bit_in_field);
    }

    soc_reg_above_64_field_set(unit, inter->reg, data, inter->field, field);

    rc = soc_reg_above_64_set(unit, inter->reg, block_instance,  inter->reg_index, data);
    if (SOC_FAILURE(rc)) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rc)));
    }

    return rc;
}

/* 
 */
int soc_interrupt_clear_on_read_fifo(int unit, int block_instance, int interrupt_id)
{
    soc_interrupt_db_t *inter, *interrupts;
    int nof_interrupts;
    int rc = SOC_E_NONE, read_count;
    soc_reg_above_64_val_t data ;
    int inter_get;

    if (!SOC_INTR_IS_SUPPORTED(unit)) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "No interrupts for device\n")));
        return SOC_E_UNAVAIL;
    }

    interrupts = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info;
    if (interrupts == NULL) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Null parameter\n")));
        return SOC_E_PARAM;
    }

    /*verify interrupt_id*/
    soc_nof_interrupts(unit, &nof_interrupts);
    if ((interrupt_id > nof_interrupts) || interrupt_id < 0) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "event_id is unavail\n")));
        return SOC_E_UNAVAIL;
    }
    inter = &(interrupts[interrupt_id]);
    if(!SOC_REG_IS_VALID(unit, inter->reg)){
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Invalid register for the device\n")));
        return SOC_E_INTERNAL;
    }
    if(!SOC_REG_IS_VALID(unit, inter->mask_reg)){
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Invalid mask register for the device\n")));
        return SOC_E_INTERNAL;
    }

    for(read_count = 0; read_count < ((soc_interrupt_clear_read_fifo_t*)(inter->interrupt_clear_param1))->read_count; read_count++){
        rc = soc_reg_above_64_get(unit, ((soc_interrupt_clear_read_fifo_t*)(inter->interrupt_clear_param1))->fifo_reg, block_instance, inter->reg_index, data);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            return rc;
        }

        rc = soc_interrupt_get(unit, block_instance , inter, &inter_get);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            return rc;
        }

        if (!inter_get) {
            break;
        }
    }

    return rc;
}

/* 
 */
int soc_interrupt_clear_on_read_array_index(int unit, int block_instance, int interrupt_id)
{
    soc_interrupt_db_t *inter, *interrupts;
    int nof_interrupts;
    int rc = SOC_E_NONE, read_count;
    soc_reg_above_64_val_t data ;
    int inter_get;

    if (!SOC_INTR_IS_SUPPORTED(unit)) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "No interrupts for device\n")));
        return SOC_E_UNAVAIL;
    }

    interrupts = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info;
    if (interrupts == NULL) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Null parameter\n")));
        return SOC_E_PARAM;
    }

    /*verify interrupt_id*/
    soc_nof_interrupts(unit, &nof_interrupts);
    if ((interrupt_id > nof_interrupts) || interrupt_id < 0) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "event_id is unavail\n")));
        return SOC_E_UNAVAIL;
    }
    inter = &(interrupts[interrupt_id]);
    if(!SOC_REG_IS_VALID(unit, inter->reg)){
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Invalid register for the device\n")));
        return SOC_E_INTERNAL;
    }
    if(!SOC_REG_IS_VALID(unit, inter->mask_reg)){
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Invalid mask register for the device\n")));
        return SOC_E_INTERNAL;
    }

    for(read_count = 0; read_count < ((soc_interrupt_clear_array_index_t*)(inter->interrupt_clear_param1))->read_count; read_count++){
        rc = soc_reg_above_64_get(unit, 
                                  ((soc_interrupt_clear_array_index_t*)(inter->interrupt_clear_param1))->fifo_reg, 
                                  block_instance, 
                                  ((soc_interrupt_clear_array_index_t*)(inter->interrupt_clear_param1))->reg_index, 
                                  data);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            return rc;
        }

        rc = soc_interrupt_get(unit, block_instance , inter, &inter_get);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            return rc;
        }

        if (!inter_get) {
            break;
        }
    }

    return rc;
}


static int
soc_active_interrupts_handle_vector(int unit, int int_port, int blk, int int_id, int is_unmasked_flag, int max_interrupts_size, soc_interrupt_cause_t *interrupts, int* index)
{
    int rc = SOC_E_NONE;
    int vector_int_bit_idx = 0;
    int vector_int_id;
    int vector_id;
    soc_interrupt_db_t *interrupts_arr;
    soc_reg_above_64_val_t vector_int_data, vector_int_mask_data, vector_int_bitmap;
    soc_field_info_t *finfop;

    interrupts_arr = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info;

    rc = soc_reg_above_64_get(unit, interrupts_arr[int_id].vector_info->int_reg, int_port, interrupts_arr[int_id].vector_info->index, vector_int_data);
    if (SOC_FAILURE(rc)) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rc)));
        return rc;
    }
    rc = soc_reg_above_64_get(unit, interrupts_arr[int_id].vector_info->int_mask_reg, int_port, interrupts_arr[int_id].vector_info->index, vector_int_mask_data);
    if (SOC_FAILURE(rc)) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rc)));
        return rc;
    }

    /* Calc interrupt bit map according to  'flags' */
    SOC_REG_ABOVE_64_COPY(vector_int_bitmap, vector_int_data);
    if (is_unmasked_flag) {
        SOC_REG_ABOVE_64_AND(vector_int_bitmap, vector_int_mask_data);
    }
    LOG_VERBOSE(BSL_LS_SOC_INTR,
                (BSL_META_U(unit,
                            "\t\tvector_int_data=0x%x%x%x%x, vector_int_mask_data=0x%x%x%x%x, vector_int_bitmap=0x%x%x%x%x,\n"),
                            vector_int_data[3], vector_int_data[2], vector_int_data[1], vector_int_data[0],
                            vector_int_mask_data[3], vector_int_mask_data[2], vector_int_mask_data[1], vector_int_mask_data[0],
                            vector_int_bitmap[3], vector_int_bitmap[2], vector_int_bitmap[1], vector_int_bitmap[0]));

    for (; vector_int_bit_idx < SOC_INTERRUPT_INTERRUPT_PER_REG_NUM_MAX; vector_int_bit_idx++) {
        if (!(vector_int_bitmap[vector_int_bit_idx / (8 * sizeof(int))] & (1 << (vector_int_bit_idx % (8 * sizeof(int)))))) {
            LOG_DEBUG(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "no interrupt for vector_int_bit_idx=%d\n"),
                       vector_int_bit_idx));
            continue;
        }

        vector_int_id = interrupts_arr[int_id].vector_info->int_id[vector_int_bit_idx];
        if (vector_int_id == -1) {
            LOG_VERBOSE(BSL_LS_SOC_INTR,
                        (BSL_META_U(unit,
                                    "Reached hidden interrupt. vector_int_bit_idx=%d,\n"),
                         vector_int_bit_idx));
            continue;
        }

    #if !defined(SOC_NO_NAMES)
        LOG_VERBOSE(BSL_LS_SOC_INTR,
                    (BSL_META_U(unit,
                                "\t\tblk=%d, int_id=%d, vector_int_bit_idx=%d, vector_int_id=%d, name=%s,\n"),
                     blk, int_id, vector_int_bit_idx, vector_int_id, interrupts_arr[vector_int_id].name));
    #else
        LOG_VERBOSE(BSL_LS_SOC_INTR,
                    (BSL_META_U(unit,
                                "\t\tblk=%d, int_id=%d, vector_int_bit_idx=%d, vector_int_id=%d,\n"),
                     blk, int_id, vector_int_bit_idx, vector_int_id));
    #endif

        /* Senity check between the interrupt field and the register bit */
        SOC_FIND_FIELD(interrupts_arr[vector_int_id].field, SOC_REG_INFO(unit, interrupts_arr[vector_int_id].reg).fields, SOC_REG_INFO(unit, interrupts_arr[vector_int_id].reg).nFields, finfop);
        if ((finfop->len == 0x1) && (finfop->bp != vector_int_bit_idx)) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "Error: Where finfop->len=%d. finfop->bp=%d != vector_int_bit_idx=%d.\n"),
                       finfop->len, finfop->bp, vector_int_bit_idx));
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "No match between interrupt bit and interrupt field\n")));
            return SOC_E_UNAVAIL;
        }

        vector_id = interrupts_arr[vector_int_id].vector_id;
        if (vector_id == 0) {
            /* no vector - real int*/
            if (*index < max_interrupts_size ) {

                interrupts[*index].index = int_port;
                interrupts[*index].id = vector_int_id;
                LOG_VERBOSE(BSL_LS_SOC_INTR,
                            (BSL_META_U(unit,
                                        "\tinterrupts[%d].index=%d, interrupts[%d].id=%d,\n"),
                             *index, interrupts[*index].index, *index, interrupts[*index].id));
                *index = *index + 1;
            }
        } else {
            rc = soc_active_interrupts_handle_vector(unit, int_port, blk, vector_int_id, is_unmasked_flag, max_interrupts_size, interrupts, index);
            if (rc != SOC_E_NONE) {
                return rc;
            }
        }
    }

    return rc;
}


/* 
 */
int
soc_active_interrupts_get(int unit, int flags, int max_interrupts_size, soc_interrupt_cause_t *interrupts, int *total_interrupts)
{
    int rc = SOC_E_NONE;
    int i = 0, blk = 0, int_bit_idx = 0, vector_int_bit_idx = 0, j;
    int index = 0;
    int int_id, int_port=0; 
    int vector_id;
    int is_unmasked_flag = 0, is_cont_prev_flag = 0, is_blk_intr_assert = 0;
    static int cont_prev_i_blk = 0, cont_prev_bit_idx = 0, cont_prev_vector_bit_idx = 0;
    int cont_prev_start_i_blk = 0;
    int first_blk_loop = 1;

    uint32 cmic_irq_stat[4];/*4 is the amount which needed for maximum 128 blocks per device*/

    soc_block_info_t *bi;
    soc_interrupt_db_t *interrupts_arr;
    soc_interrupt_tree_t *interrupt_tree;

    soc_reg_above_64_val_t block_int_data, block_int_mask_data, block_int_bitmap;
#ifdef BCM_DNX_SUPPORT
    soc_reg_above_64_val_t  eci_irq_status;
#endif
    soc_field_info_t *finfop;

#ifdef BCM_PETRA_SUPPORT
    uint32 cmic_irq_state_2;
#endif /* BCM_PETRA_SUPPORT */

    if (!SOC_INTR_IS_SUPPORTED(unit)) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "No interrupts for device\n")));
        return SOC_E_UNAVAIL;
    }

    if (interrupts == NULL || total_interrupts == NULL) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Null parameter\n")));
        return SOC_E_PARAM;
    }

    interrupts_arr = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info;
    if (interrupts_arr == NULL) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Null parameter\n")));
        return SOC_E_PARAM;
    }

    interrupt_tree = SOC_CONTROL(unit)->interrupts_info->interrupt_tree_info;
    if (interrupt_tree == NULL) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Null parameter\n")));
        return SOC_E_PARAM;
    }

    sal_memset(cmic_irq_stat, 0x0, sizeof(cmic_irq_stat));

    if (flags & SOC_ACTIVE_INTERRUPTS_GET_UNMASKED_ONLY) {
        is_unmasked_flag = 1;
    }

    if (flags & SOC_ACTIVE_INTERRUPTS_GET_CONT_PREV) {
        is_cont_prev_flag = 1;
    }

    if (is_cont_prev_flag) {
        cont_prev_start_i_blk = i = cont_prev_i_blk;
        int_bit_idx = 0 /*cont_prev_bit_idx*/;
        vector_int_bit_idx = 0 /*cont_prev_vector_bit_idx*/;
    } else {
        i = 0;
        int_bit_idx = 0;
        vector_int_bit_idx = 0;
    }

    if (soc_feature(unit, soc_feature_cmicm)) {
#ifdef BCM_CMICM_SUPPORT
        int cmc = SOC_PCI_CMC(unit);
        cmic_irq_stat[0] = soc_pci_read(unit, CMIC_CMCx_IRQ_STAT3_OFFSET(cmc)); 
        cmic_irq_stat[0] &= SOC_CMCx_IRQ3_MASK(unit,cmc);
        cmic_irq_stat[1] = soc_pci_read(unit, CMIC_CMCx_IRQ_STAT4_OFFSET(cmc));
        cmic_irq_stat[1] &= SOC_CMCx_IRQ4_MASK(unit,cmc);
#ifdef BCM_PETRA_SUPPORT 
        /* add to cimic_irq_stat[0] the port interrupts bits from irq_state2 */
        if ( SOC_IS_ARAD(unit) && !(SOC_IS_ARDON(unit)) && !(SOC_IS_JERICHO(unit))) {
            cmic_irq_state_2 = soc_pci_read(unit, CMIC_CMCx_IRQ_STAT2_OFFSET(cmc));
            cmic_irq_state_2 &= SOC_CMCx_IRQ2_MASK(unit,cmc);
            cmic_irq_stat[0] |= _PORT_BLOCK_FROM_IRQ_STATE2(unit, cmic_irq_state_2, PARITY_INTERRUPT_4f, SOC_CMIC_BLK_CLP_0_INDX)
                             | _PORT_BLOCK_FROM_IRQ_STATE2(unit, cmic_irq_state_2, PARITY_INTERRUPT_3f, SOC_CMIC_BLK_CLP_1_INDX) 
                             | _PORT_BLOCK_FROM_IRQ_STATE2(unit, cmic_irq_state_2, PARITY_INTERRUPT_2f, SOC_CMIC_BLK_XLP_0_INDX)    
                             | _PORT_BLOCK_FROM_IRQ_STATE2(unit, cmic_irq_state_2, PARITY_INTERRUPT_1f, SOC_CMIC_BLK_XLP_1_INDX);
        }
#endif /* BCM_PETRA_SUPPORT */
        if (soc_feature(unit, soc_feature_cmicm_extended_interrupts)) {
           cmic_irq_stat[2] = soc_pci_read(unit, CMIC_CMCx_IRQ_STAT5_OFFSET(cmc)); 
           cmic_irq_stat[2] &= SOC_CMCx_IRQ5_MASK(unit,cmc);
           cmic_irq_stat[3] = soc_pci_read(unit, CMIC_CMCx_IRQ_STAT6_OFFSET(cmc));
           cmic_irq_stat[3] &= SOC_CMCx_IRQ6_MASK(unit,cmc);
        }
#endif
    }
#ifdef BCM_CMICX_SUPPORT
    else if (soc_feature(unit, soc_feature_cmicx)) {
        int ii;
        soc_reg_t cmicx_status_reg[4] = {ICFG_CHIP_LP_INTR_RAW_STATUS_REG0r,
                                         ICFG_CHIP_LP_INTR_RAW_STATUS_REG1r,
                                         ICFG_CHIP_LP_INTR_RAW_STATUS_REG2r,
                                         ICFG_CHIP_LP_INTR_RAW_STATUS_REG3r};
        soc_reg_t cmicx_enable_reg[4] = {ICFG_CHIP_LP_INTR_ENABLE_REG0r,
                                         ICFG_CHIP_LP_INTR_ENABLE_REG1r,
                                         ICFG_CHIP_LP_INTR_ENABLE_REG2r,
                                         ICFG_CHIP_LP_INTR_ENABLE_REG3r};
#ifdef BCM_DNX_SUPPORT
        if (SOC_IS_DNX(unit))
        {
            soc_reg_above_64_val_t eci_irq_enable;

            SOC_REG_ABOVE_64_CLEAR(eci_irq_status);
            SOC_REG_ABOVE_64_CLEAR(eci_irq_enable);
            (void)soc_reg_above_64_get(unit, ECI_ECI_INTERRUPTSr, REG_PORT_ANY, 0, eci_irq_status);
            (void)soc_reg_above_64_get(unit, ECI_ECI_INTERRUPTS_MASKr, REG_PORT_ANY, 0, eci_irq_enable);

             SOC_REG_ABOVE_64_AND(eci_irq_status, eci_irq_enable);
        } else
#endif
        {
            uint32 cmic_irq_enable[4] = {0};
            /* ICFG_CHIP_LP_INTR_ENABLE_REG be used for DNX device, but ICFG_CHIP_LP_INTR_RAW_STATUS_REG not */
            for (ii = 0; ii < 4; ii++) {
                (void)soc_cmic_or_iproc_getreg(unit, cmicx_status_reg[ii], &cmic_irq_stat[ii]);
                (void)soc_cmic_or_iproc_getreg(unit, cmicx_enable_reg[ii], &cmic_irq_enable[ii]);
                cmic_irq_stat[ii] &= cmic_irq_enable[ii];
            }
        }
    }
#endif
    else {
        cmic_irq_stat[0] = soc_pci_read(unit, CMIC_IRQ_STAT_1); 
        cmic_irq_stat[1] = soc_pci_read(unit, CMIC_IRQ_STAT_2);
    }

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit)) {
        /* do nothing */
    } else
#endif
    {
        for (j = 0; j < 4; j++) {
            LOG_VERBOSE(BSL_LS_SOC_INTR,
                        (BSL_META_U(unit,
                                    "%s(): cmic_irq_stat[%u]=0x%x\n"),
                         FUNCTION_NAME(), j, cmic_irq_stat[j]));
        }
    }
    while (1) {

        if (SOC_BLOCK_INFO(unit, i).type < 0) {
            i = 0;
        }

        if ((i == cont_prev_start_i_blk) && (first_blk_loop == 0)) {
            break;
        }

        first_blk_loop = 0;

        if (!(SOC_INFO(unit).block_valid[i])) {
            i++;
            continue;
        }

        bi = &(SOC_BLOCK_INFO(unit, i));
        blk=bi->cmic;

#ifdef BCM_DNX_SUPPORT
        if (SOC_IS_DNX(unit))
        {
            is_blk_intr_assert = soc_dnx_is_block_eci_intr_assert(unit, blk, eci_irq_status);
        } else
#endif
        {
             is_blk_intr_assert = SHR_BITGET(cmic_irq_stat, blk);
        }
        if ((!is_blk_intr_assert && (is_unmasked_flag == 1)) || (interrupt_tree[blk].int_reg == INVALIDr)){
            LOG_DEBUG(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "no interrupt for blk=%d\n"),
                       blk));
            i++;
            continue;
        }

        LOG_VERBOSE(BSL_LS_SOC_INTR,
                    (BSL_META_U(unit,
                                "blk=%d, bi->number=%d,\n"),
                     blk, bi->number));

        /* Read block interrupt register */
#if defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
       /* dnxf device have no clp/xlp block */
       if((SOC_IS_ARAD(unit) || SOC_IS_DNX(unit)) &&  (bi->type == SOC_BLK_CLP || bi->type == SOC_BLK_XLP )) {
            int_port = SOC_BLOCK_PORT(unit, i);
        } else 
#endif /* BCM_PETRA_SUPPORT */
        {
        int_port =  bi->number;
        }

        rc = soc_reg_above_64_get(unit, interrupt_tree[blk].int_reg, int_port, interrupt_tree[blk].index, block_int_data);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            return rc;
        }
        rc = soc_reg_above_64_get(unit, interrupt_tree[blk].int_mask_reg, int_port, interrupt_tree[blk].index, block_int_mask_data);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            return rc;
        }

        /* Calc interrupt bit map according to  'flags' */
        SOC_REG_ABOVE_64_COPY(block_int_bitmap, block_int_data);
        if (is_unmasked_flag) {
            SOC_REG_ABOVE_64_AND(block_int_bitmap, block_int_mask_data);
        }
        LOG_VERBOSE(BSL_LS_SOC_INTR,
                    (BSL_META_U(unit,
                                "block_int_data=0x%x 0x%x 0x%x 0x%x, block_int_mask_data=0x%x 0x%x 0x%x 0x%x, block_int_bitmap=0x%x 0x%x 0x%x 0x%x,\n"),
                     block_int_data[0], block_int_data[2], block_int_data[2], block_int_data[3], 
                     block_int_mask_data[0], block_int_mask_data[1], block_int_mask_data[2], block_int_mask_data[3], 
                     block_int_bitmap[0], block_int_bitmap[1], block_int_bitmap[2], block_int_bitmap[3]));

        for (; int_bit_idx < SOC_INTERRUPT_INTERRUPT_PER_REG_NUM_MAX; int_bit_idx++) {
            int_id = interrupt_tree[blk].int_id[int_bit_idx];
            if (int_id == -1) {
                LOG_VERBOSE(BSL_LS_SOC_INTR,
                            (BSL_META_U(unit,
                                        "Reached hidden interrupt. int_bit_idx=%d,\n"),
                             int_bit_idx));
                continue;
            }

            vector_id = interrupts_arr[int_id].vector_id;
            if ((SHR_BITGET(block_int_bitmap, int_bit_idx) == 0) && (!(vector_id == 1 && is_unmasked_flag == 0))) {
                LOG_DEBUG(BSL_LS_SOC_INTR,
                          (BSL_META_U(unit,
                                      "no interrupt for int_bit_idx=%d\n"),
                           int_bit_idx));
                continue;
            }

#if !defined(SOC_NO_NAMES)
            LOG_VERBOSE(BSL_LS_SOC_INTR,
                        (BSL_META_U(unit,
                                    "\tgettind int_id: blk=%d, int_bit_idx=%d, int_id=%d, name=%s, vector_id=%d,\n"),
                         blk, int_bit_idx, int_id, interrupts_arr[int_id].name, vector_id));
#else
            LOG_VERBOSE(BSL_LS_SOC_INTR,
                        (BSL_META_U(unit,
                                    "\tgettind int_id: blk=%d, int_bit_idx=%d, int_id=%d, vector_id=%d,\n"),
                         blk, int_bit_idx, int_id, vector_id));
#endif

            /* Senity check between the interrupt field and the register bit */
            SOC_FIND_FIELD(interrupts_arr[int_id].field, SOC_REG_INFO(unit, interrupts_arr[int_id].reg).fields, SOC_REG_INFO(unit, interrupts_arr[int_id].reg).nFields, finfop);
            if ((finfop->len == 0x1) && (finfop->bp != int_bit_idx)) {
                LOG_ERROR(BSL_LS_SOC_INTR,
                          (BSL_META_U(unit,
                                      "Error: Where finfop->len=%d. finfop->bp=%d != int_bit_idx=%d.\n"),
                           finfop->len, finfop->bp, int_bit_idx));
                LOG_ERROR(BSL_LS_SOC_INTR,
                          (BSL_META_U(unit,
                                      "No match between interrupt bit and interrupt field\n")));
                return SOC_E_UNAVAIL;
            }

            /* handle vecor */
            if (vector_id == 0) {
                /* no vecotr - real int*/
                if (index < max_interrupts_size ) {

                    interrupts[index].index = int_port;
                    interrupts[index].id = int_id;
                    cont_prev_i_blk = i;
                    cont_prev_bit_idx = int_bit_idx;
                    cont_prev_vector_bit_idx = vector_int_bit_idx;
                    LOG_VERBOSE(BSL_LS_SOC_INTR,
                                (BSL_META_U(unit,
                                            "\tinterrupts[%d].index=%d, interrupts[%d].id=%d,\n"),
                                 index, interrupts[index].index, index, interrupts[index].id));
                    index++;
                }
            } else {
                rc = soc_active_interrupts_handle_vector(unit, int_port, blk, int_id, is_unmasked_flag, max_interrupts_size, interrupts, &index);
                if (rc != SOC_E_NONE) {
                    return rc;
                } else {
                    if (index < max_interrupts_size ) {
                        cont_prev_i_blk = i;
                        cont_prev_bit_idx = int_bit_idx;
                        cont_prev_vector_bit_idx = vector_int_bit_idx;
                    }
                }
            }
        }
        int_bit_idx = 0;
        i++;
    }

    *total_interrupts = index;
    LOG_VERBOSE(BSL_LS_SOC_INTR,
                (BSL_META_U(unit,
                            "index=%d, *total_interrupts=%d, cont_prev_i_blk=%d, cont_prev_bit_idx=%d, cont_prev_vector_bit_idx=%d.\n"),
                 index, *total_interrupts, cont_prev_i_blk, cont_prev_bit_idx, cont_prev_vector_bit_idx));

    return rc;
}

int
soc_interrupt_info_get(int unit, int interrupt_id, soc_interrupt_db_t *inter)
{
    soc_interrupt_db_t *interrupts_arr;
    int nof_interrupts;
    int rc = SOC_E_NONE;

    if (!SOC_INTR_IS_SUPPORTED(unit)) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "No interrupts for device\n")));
        return SOC_E_UNAVAIL;
    }

    if (inter == NULL) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Null parameter\n")));
        return SOC_E_PARAM;
    }

    interrupts_arr = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info;
    if (interrupts_arr == NULL) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Null parameter\n")));
        return SOC_E_PARAM;
    }

    rc = soc_nof_interrupts(unit, &nof_interrupts);
    if (SOC_FAILURE(rc)) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rc)));
        return rc;
    }
    if (interrupt_id > nof_interrupts) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "interrupt_id is unavail\n")));
        return SOC_E_UNAVAIL;
    }

#if !defined(SOC_NO_NAMES)
    inter->name = interrupts_arr[interrupt_id].name;
#endif
    inter->reg = interrupts_arr[interrupt_id].reg;
    inter->reg_index = interrupts_arr[interrupt_id].reg_index;
    inter->field = interrupts_arr[interrupt_id].field;   
    inter->mask_reg = interrupts_arr[interrupt_id].mask_reg;
    inter->mask_reg_index = interrupts_arr[interrupt_id].mask_reg_index;
    inter->mask_field = interrupts_arr[interrupt_id].mask_field;
    inter->bit_in_field = interrupts_arr[interrupt_id].bit_in_field;
    inter->reg_test = interrupts_arr[interrupt_id].reg_test;

    return rc;
}

int 
soc_get_interrupt_id(int unit, soc_reg_t reg, int reg_index, soc_field_t field, int bit_in_field, int* interrupt_id)
{
    soc_interrupt_db_t *interrupts_arr;
    int nof_interrupts;
    int i;
    int rc = SOC_E_NONE;

    if (!SOC_INTR_IS_SUPPORTED(unit)) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "No interrupts for device\n")));
        return SOC_E_UNAVAIL;
    }

    interrupts_arr = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info;
    if (interrupts_arr == NULL) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Null parameter\n")));
        return SOC_E_PARAM;
    }

    if (interrupt_id == NULL) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Null parameter\n")));
        return SOC_E_PARAM;
    }
    *interrupt_id = -1;

    /* look for interrupt id*/
    rc = soc_nof_interrupts(unit, &nof_interrupts);
    if (SOC_FAILURE(rc)) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rc)));
        return rc;
    }
    for(i=0; i < nof_interrupts; i++) {
        if(reg == interrupts_arr[i].reg && field == interrupts_arr[i].field && reg_index == interrupts_arr[i].reg_index) {

            if (interrupts_arr[i].bit_in_field == SOC_INTERRUPT_BIT_FIELD_DONT_CARE || 
                interrupts_arr[i].bit_in_field == bit_in_field) {
                *interrupt_id = i;
                break;
            }
        }
    }

    if(*interrupt_id == -1) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "interrupt ID was not found\n")));
        return SOC_E_UNAVAIL;
    }

    return rc;
}

int
soc_get_interrupt_id_specific(int unit, int reg_adress, int reg_block, int field_bit, int* interrupt_id)
{
    soc_interrupt_db_t *interrupts_arr;
    int nof_interrupts;
    soc_field_info_t *finfop;
    int i, blk_indx;
    int rc = SOC_E_NONE;

    if (!SOC_INTR_IS_SUPPORTED(unit)) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "No interrupts for device\n")));
        return SOC_E_UNAVAIL;
    }

    interrupts_arr = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info;
    if (interrupts_arr == NULL) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Null parameter\n")));
        return SOC_E_PARAM;
    }

    *interrupt_id = -1;

    /* find the block index */    
    for(blk_indx = 0 ; SOC_BLOCK_INFO(unit, blk_indx).type >= 0; ++blk_indx ) { 
        if (SOC_INFO(unit).block_valid[blk_indx]) {
            if (reg_block == SOC_BLOCK_INFO(unit, blk_indx).cmic) {
                break;
            }
        }
    }

    if(SOC_BLOCK_INFO(unit, blk_indx).type < 0) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Block number invalid\n")));
        return SOC_E_PARAM;
    }

    rc = soc_nof_interrupts(unit, &nof_interrupts);
    if (SOC_FAILURE(rc)) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rc)));
        return rc;
    }

     /* look for interrupt id*/
    for(i=0; i < nof_interrupts; i++) {
        /* check block */
        if(soc_interrupt_valid(unit, SOC_BLOCK_INFO(unit, blk_indx).number, i) == 0x0) {
            continue; 
        }

        if(SOC_BLOCK_INFO(unit, blk_indx).type != SOC_REG_INFO(unit, interrupts_arr[i].reg).block[0]) {
            continue;    
        }

        /* check address */
        if(reg_adress == (SOC_REG_INFO(unit, interrupts_arr[i].reg ).offset + interrupts_arr[i].reg_index)){


            SOC_FIND_FIELD( interrupts_arr[i].field,
                            SOC_REG_INFO(unit, interrupts_arr[i].reg).fields,
                            SOC_REG_INFO(unit, interrupts_arr[i].reg).nFields,
                            finfop); 

            if(!finfop) {
                continue;
            }
            /* check interrupt bit */
            if(interrupts_arr[i].bit_in_field == SOC_INTERRUPT_BIT_FIELD_DONT_CARE ) {
                if(field_bit != finfop->bp ) {
                    continue;
                }
            }else {
                if(field_bit != finfop->bp + interrupts_arr[i].bit_in_field ) {
                    continue;
                }
            }

            *interrupt_id = i;

            break; 
        }
    }

    if(*interrupt_id == -1) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "interrupt ID was not found\n")));
        return SOC_E_NOT_FOUND;
    }

    return rc;
}

/*number of interrupts per block instance*/
int soc_nof_interrupts(int unit, int* nof_interrupts) {
    int rc = SOC_E_NONE;

    if (nof_interrupts == NULL) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Null parameter\n")));
        return SOC_E_PARAM;
    }

    *nof_interrupts = 0;

#ifdef BCM_DFE_SUPPORT
    if(SOC_IS_DFE(unit)) {
        rc = soc_dfe_nof_interrupts(unit, nof_interrupts);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            return rc;
        }
    }
#endif
#ifdef BCM_PETRA_SUPPORT
#ifdef BCM_QUX_SUPPORT
    if (SOC_IS_QUX(unit)) {
        rc = soc_qux_nof_interrupts(unit, nof_interrupts);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            return rc;
        }
    } else
#endif
#ifdef BCM_JERICHO_PLUS_SUPPORT
    if (SOC_IS_JERICHO_PLUS_ONLY(unit)) {
        rc = soc_jerp_nof_interrupts(unit, nof_interrupts);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            return rc;
        }
    } else
#endif
    if(SOC_IS_ARAD(unit)) {
        MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_nof_interrupts, (unit,nof_interrupts));
    }
#endif
#ifdef BCM_DNXF_SUPPORT
    if(SOC_IS_RAMON(unit)){
        rc = soc_ramon_nof_interrupts(unit, nof_interrupts);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                          (BSL_META_U(unit,
                                "%s\n"), soc_errmsg(rc)));
            return rc;
        }
    }
#endif
#ifdef BCM_DNX_SUPPORT
    if(SOC_IS_DNX(unit)) {
        *nof_interrupts = dnx_data_intr.general.nof_interrupts_get(unit);
    }
#endif
    return rc;
}

int soc_interrupt_get_block_index_from_port(int unit, int interrupt_id, int port)
{
    int index = port;
#if defined(BCM_PETRA_SUPPORT) || defined(BCM_DNXF_SUPPORT) || defined(BCM_DNX_SUPPORT)
    soc_interrupt_db_t *interrupts_arr;
    soc_block_types_t block_types;

    interrupts_arr = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info;

    block_types = SOC_REG_INFO(unit, interrupts_arr[interrupt_id].reg).block;

    if(SOC_IS_ARAD(unit) || SOC_IS_DNXF(unit) || SOC_IS_DNX(unit)){
        if(SOC_BLOCK_IN_LIST(unit, block_types, SOC_BLK_CLP)) {
            index = SOC_PORT_BLOCK_NUMBER(unit, port);
        } else if(SOC_BLOCK_IN_LIST(unit, block_types, SOC_BLK_XLP)){
            index = (SOC_IS_JERICHO(unit) || SOC_IS_DNXF(unit) || SOC_IS_DNX(unit)) ? SOC_PORT_BLOCK_NUMBER(unit, port) : SOC_PORT_BLOCK_NUMBER(unit, port) + SOC_MAX_NUM_CLP_BLKS;
        } 
    }
#endif /* BCM_PETRA_SUPPORT */

    return index;
}

int soc_interrupt_get_intr_port_from_index(int unit, int interrupt_id, int block_instance)
{
    int port=block_instance;
#if defined(BCM_PETRA_SUPPORT) || defined(BCM_DNXF_SUPPORT) || defined(BCM_DNX_SUPPORT)
    int bi_index;
    soc_interrupt_db_t *interrupts_arr;
    soc_block_types_t block_types;
    soc_block_info_t *bi;

    interrupts_arr = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info;
    block_types = SOC_REG_INFO(unit, interrupts_arr[interrupt_id].reg).block;

     if((SOC_IS_ARAD(unit) || SOC_IS_DNXF(unit) || SOC_IS_DNX(unit)) &&
        (SOC_BLOCK_IN_LIST(unit, block_types, SOC_BLK_CLP) || SOC_BLOCK_IN_LIST(unit, block_types, SOC_BLK_XLP))) {

        /* find the block index */
        for (bi_index = 0;  SOC_BLOCK_INFO(unit, bi_index).type >= 0; bi_index++ ) {
            bi = &(SOC_BLOCK_INFO(unit, bi_index));
            if(bi->type == block_types[0] && bi->number == block_instance) {
                break;
            }

        }

        port = SOC_BLOCK_PORT(unit, bi_index);
    }
#endif /* BCM_PETRA_SUPPORT */

    return port;
}

/* 
 * Interrupt aplication Functions - uses Soc DB 
 */ 
int soc_interrupt_flags_set(int unit, int interrupt_id, uint32 flags)
{
    int rc = SOC_E_NONE;

#ifdef BCM_PETRA_SUPPORT
    if(SOC_IS_ARAD(unit)) {
        rc = sw_state_access[unit].dpp.soc.arad.tm.interrupts.interrupt_data.flags.set(unit, interrupt_id, flags);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            return rc;
        }
    } else
#endif /* BCM_PETRA_SUPPORT */
#ifdef BCM_DNXF_SUPPORT
    if(SOC_IS_DNXF(unit)) {
        rc = dnxf_state.intr.flags.set(unit, interrupt_id, flags);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_INTR, (BSL_META_U(unit, "%s\n"), soc_errmsg(rc)));
            return rc;
        }
    } else
#endif
#ifdef BCM_DNX_SUPPORT
    if(SOC_IS_DNX(unit)) {
        rc = intr_db.flags.set(unit, interrupt_id, flags);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_INTR, (BSL_META_U(unit, "%s\n"), soc_errmsg(rc)));
            return rc;
        }
    } else
#endif
#ifdef BCM_DFE_SUPPORT
    if(SOC_IS_DFE(unit)) {
        int nof_interrupts;

        rc = soc_dfe_nof_interrupts(unit, &nof_interrupts);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            return rc;
        }

        if(interrupt_id < 0 || interrupt_id >= nof_interrupts) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "Interrupt_id is out of range\n")));
            return SOC_E_PARAM;
        }

        rc = SOC_DFE_WARM_BOOT_ARR_VAR_SET(unit, INTR_FLAGS, interrupt_id, &flags);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            return rc;
        }
    } else
#endif /* BCM_DFE_SUPPORT */
    {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Unsupported in this unit type\n")));
        return SOC_E_UNIT;
    }

    return rc;
}

int soc_interrupt_flags_get(int unit, int interrupt_id, uint32* flags)
{
    int rc = SOC_E_NONE;

    if (flags == NULL) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Null parameter\n")));
        return SOC_E_PARAM;
    }

#ifdef BCM_PETRA_SUPPORT
    if(SOC_IS_ARAD(unit)) {
        rc = sw_state_access[unit].dpp.soc.arad.tm.interrupts.interrupt_data.flags.get(unit, interrupt_id, flags);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            return rc;
        }
    } else
#endif /* BCM_PETRA_SUPPORT */
#ifdef BCM_DNXF_SUPPORT
    if(SOC_IS_DNXF(unit)){
        rc = dnxf_state.intr.flags.get(unit, interrupt_id, flags);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            return rc;
        }
    } else
#endif
#ifdef BCM_DNX_SUPPORT
    if(SOC_IS_DNX(unit)) {
        rc = intr_db.flags.get(unit, interrupt_id, flags);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            return rc;
        }
    } else
#endif
#ifdef BCM_DFE_SUPPORT
    if(SOC_IS_DFE(unit)) {
        int nof_interrupts;

        rc = soc_dfe_nof_interrupts(unit, &nof_interrupts); 
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            return rc;
        }

        if(interrupt_id < 0 || interrupt_id >= nof_interrupts) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "Interrupt_id is out of range\n")));
            return SOC_E_PARAM;
        }

        rc = SOC_DFE_WARM_BOOT_ARR_VAR_GET(unit, INTR_FLAGS, interrupt_id, flags);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            return rc;
        }
    } else
#endif /* BCM_DFE_SUPPORT */
    {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Unsupported in this unit type\n")));
        return SOC_E_UNIT;
    }

    return rc;
}

int soc_interrupt_storm_timed_period_set(int unit, int interrupt_id, uint32 storm_timed_period)
{
    int rc = SOC_E_NONE;

#ifdef BCM_PETRA_SUPPORT
    if(SOC_IS_ARAD(unit)) {
        rc = sw_state_access[unit].dpp.soc.arad.tm.interrupts.interrupt_data.storm_timed_period.set(unit, interrupt_id, storm_timed_period);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            return rc;
        }
    } else
#endif /* BCM_PETRA_SUPPORT */
#ifdef BCM_DNXF_SUPPORT
    if(SOC_IS_DNXF(unit)) {
        rc = dnxf_state.intr.storm_timed_period.set(unit, interrupt_id, storm_timed_period);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            return rc;
        }
    } else
#endif /* BCM_PETRA_SUPPORT */
#ifdef BCM_DNX_SUPPORT
    if(SOC_IS_DNX(unit)) {
        rc = intr_db.storm_timed_period.set(unit, interrupt_id, storm_timed_period);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            return rc;
        }
    } else
#endif /* BCM_PETRA_SUPPORT */
#ifdef BCM_DFE_SUPPORT
    if(SOC_IS_DFE(unit)) {
        int nof_interrupts;

        /* get num of interrupts */
        rc = soc_dfe_nof_interrupts(unit, &nof_interrupts);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            return rc;
        }

        if(interrupt_id < 0 || interrupt_id >= nof_interrupts) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "Interrupt_id is out of range\n")));
            return SOC_E_PARAM;
        }

        rc = SOC_DFE_WARM_BOOT_ARR_VAR_SET(unit, INTR_STORM_TIMED_PERIOD, interrupt_id, &storm_timed_period);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            return rc;
        }
    } else
#endif /* BCM_DFE_SUPPORT */
    {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Unsupported in this unit type\n")));
        return SOC_E_UNIT;
    }

    return rc;
}

int soc_interrupt_storm_timed_period_get(int unit, int interrupt_id, uint32* storm_timed_period)
{
    int rc = SOC_E_NONE;

    if (storm_timed_period == NULL) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Null parameter\n")));
        return SOC_E_PARAM;
    }

#ifdef BCM_PETRA_SUPPORT
    if(SOC_IS_ARAD(unit)) {
        rc = sw_state_access[unit].dpp.soc.arad.tm.interrupts.interrupt_data.storm_timed_period.get(unit, interrupt_id, storm_timed_period);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            return rc;
        }
    } else
#endif /* BCM_PETRA_SUPPORT */
#ifdef BCM_DNXF_SUPPORT
    if(SOC_IS_DNXF(unit)) {
        rc = dnxf_state.intr.storm_timed_period.get(unit, interrupt_id, storm_timed_period);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            return rc;
        }
    } else
#endif /* BCM_DNXF_SUPPORT */
#ifdef BCM_DNX_SUPPORT
    if(SOC_IS_DNX(unit)) {
        rc = intr_db.storm_timed_period.get(unit, interrupt_id, storm_timed_period);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            return rc;
        }
    } else
#endif
#ifdef BCM_DFE_SUPPORT
    if(SOC_IS_DFE(unit)) {
        int nof_interrupts;

        rc = soc_dfe_nof_interrupts(unit, &nof_interrupts);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            return rc;
        }

        if(interrupt_id < 0 || interrupt_id >= nof_interrupts) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "Interrupt_id is out of range\n")));
            return SOC_E_PARAM;
        }

        rc = SOC_DFE_WARM_BOOT_ARR_VAR_GET(unit, INTR_STORM_TIMED_PERIOD, interrupt_id, storm_timed_period);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            return rc;
        }
    } else
#endif /* BCM_DFE_SUPPORT */
    {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Unsupported in this unit type\n")));
        return SOC_E_UNIT;
    }

    return rc;
}

int soc_interrupt_storm_timed_count_set(int unit, int interrupt_id, uint32 storm_timed_count)
{
    int rc = SOC_E_NONE;

#ifdef BCM_PETRA_SUPPORT
    if(SOC_IS_ARAD(unit)) {
        rc = sw_state_access[unit].dpp.soc.arad.tm.interrupts.interrupt_data.storm_timed_count.set(unit, interrupt_id, storm_timed_count);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            return rc;
        }
    } else
#endif /* BCM_PETRA_SUPPORT */
#ifdef BCM_DNXF_SUPPORT
    if(SOC_IS_DNXF(unit)) {
        rc = dnxf_state.intr.storm_timed_count.set(unit, interrupt_id, storm_timed_count);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            return rc;
        }
    } else
#endif /* BCM_DNXF_SUPPORT */
#ifdef BCM_DNX_SUPPORT
    if(SOC_IS_DNX(unit)) {
        rc = intr_db.storm_timed_count.set(unit, interrupt_id, storm_timed_count);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            return rc;
        }
    } else
#endif /* BCM_DNXF_SUPPORT */
#ifdef BCM_DFE_SUPPORT
    if(SOC_IS_DFE(unit)) {
        int nof_interrupts;

        /* get num of interrupts */
        rc = soc_dfe_nof_interrupts(unit, &nof_interrupts);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            return rc;
        }

        if(interrupt_id < 0 || interrupt_id >= nof_interrupts) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "Interrupt_id is out of range\n")));
            return SOC_E_PARAM;
        }

        rc = SOC_DFE_WARM_BOOT_ARR_VAR_SET(unit, INTR_STORM_TIMED_COUNT, interrupt_id, &storm_timed_count);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            return rc;
        }
    } else
#endif /* BCM_DFE_SUPPORT */
    {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Unsupported in this unit type\n")));
        return SOC_E_UNIT;
    }

    return rc;
}

int soc_interrupt_storm_timed_count_get(int unit, int interrupt_id, uint32* storm_timed_count)
{
    int rc = SOC_E_NONE;

    if (storm_timed_count == NULL) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Null parameter\n")));
        return SOC_E_PARAM;
    }

#ifdef BCM_PETRA_SUPPORT
    if(SOC_IS_ARAD(unit)) {
        rc = sw_state_access[unit].dpp.soc.arad.tm.interrupts.interrupt_data.storm_timed_count.get(unit, interrupt_id, storm_timed_count);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            return rc;
        }
    } else
#endif /* BCM_PETRA_SUPPORT */
#ifdef BCM_DNXF_SUPPORT
    if(SOC_IS_DNXF(unit)) {
        rc = dnxf_state.intr.storm_timed_count.get(unit, interrupt_id, storm_timed_count);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            return rc;
        }
    } else
#endif /* BCM_DNXF_SUPPORT */
#ifdef BCM_DNX_SUPPORT
    if(SOC_IS_DNX(unit)) {
        rc = intr_db.storm_timed_count.get(unit, interrupt_id, storm_timed_count);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            return rc;
        }
    } else
#endif
#ifdef BCM_DFE_SUPPORT
    if(SOC_IS_DFE(unit)) {
        int nof_interrupts;

        rc = soc_dfe_nof_interrupts(unit, &nof_interrupts);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            return rc;
        }

        if(interrupt_id < 0 || interrupt_id >= nof_interrupts) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "Interrupt_id is out of range\n")));
            return SOC_E_PARAM;
        }

        rc = SOC_DFE_WARM_BOOT_ARR_VAR_GET(unit, INTR_STORM_TIMED_COUNT, interrupt_id, storm_timed_count);
        if (SOC_FAILURE(rc)) {
            LOG_ERROR(BSL_LS_SOC_INTR,
                      (BSL_META_U(unit,
                                  "%s\n"), soc_errmsg(rc)));
            return rc;
        }
    } else
#endif /* BCM_DFE_SUPPORT */
    {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Unsupported in this unit type\n")));
        return SOC_E_UNIT;
    }

    return rc;
}

int soc_interrupt_update_storm_detection(int unit, int block_instance, soc_interrupt_db_t *inter) 
{
    uint32 current_time, storm_timed_period, storm_timed_count;
    int inf_index ;
    int rc = SOC_E_NONE;

    if (inter == NULL) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Null parameter\n")));
        return SOC_E_PARAM;
    }

    current_time = sal_time();

    inf_index = soc_interrupt_get_block_index_from_port(unit, inter->id, block_instance);
    if(inf_index < 0) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Invalid parameters\n")));
        return SOC_E_PARAM;
    }

    rc = soc_interrupt_storm_timed_period_get(unit, inter->id, &storm_timed_period);
    if (SOC_FAILURE(rc)) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rc)));
        return rc;
    }

    rc = soc_interrupt_storm_timed_count_get(unit, inter->id, &storm_timed_count);
    if (SOC_FAILURE(rc)) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rc)));
        return rc;
    }

    if (storm_timed_count > 0 && storm_timed_period > 0) {

        if ((current_time - inter->storm_detection_start_time[inf_index]) > storm_timed_period) {
            inter->storm_detection_start_time[inf_index] = current_time;
            inter->storm_detection_occurrences[inf_index] = 0;
        }
        (inter->storm_detection_occurrences[inf_index])++;
    }

    if (SOC_SWITCH_EVENT_NOMINAL_STORM(unit) > 0) {
        if (inter->storm_nominal_count[inf_index] >= SOC_SWITCH_EVENT_NOMINAL_STORM(unit)) {
            inter->storm_nominal_count[inf_index] = 0;
        } else {
            (inter->storm_nominal_count[inf_index])++;
        }
    }

    return rc;
}

int soc_interrupt_is_storm(int unit, int block_instance, soc_interrupt_db_t *inter, int *is_storm_count_period,int *is_storm_nominal) {

    int inf_index = block_instance;
    uint32 storm_timed_count;
    int rc = SOC_E_NONE;

    if (inter == NULL || is_storm_count_period == NULL || is_storm_nominal == NULL) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Null parameter\n")));
        return SOC_E_PARAM;
    }

    *is_storm_count_period = 0x0;
    *is_storm_nominal  = 0x0;

    inf_index = soc_interrupt_get_block_index_from_port(unit, inter->id, block_instance);
    if(inf_index < 0) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Invalid parameters\n")));
        return SOC_E_PARAM;
    }

    rc = soc_interrupt_storm_timed_count_get(unit, inter->id, &storm_timed_count);
    if (SOC_FAILURE(rc)) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rc)));
        return rc;
    }

    if ((storm_timed_count != 0x0) && (inter->storm_detection_occurrences[inf_index] >= storm_timed_count)) {
        inter->storm_detection_occurrences[inf_index] = 0x0;
        *is_storm_count_period = 0x1;
    }

    if ((SOC_SWITCH_EVENT_NOMINAL_STORM(unit) != 0x0) && (inter->storm_nominal_count[inf_index] >= SOC_SWITCH_EVENT_NOMINAL_STORM(unit))){
        inter->storm_nominal_count[inf_index] = 0x0;
        *is_storm_nominal = 0x1;
    }

    return rc;
}

int soc_interrupt_clear_all(int unit){
    int is_valid;
    int nof_interrupts;
    int inter;
    int bi_index, int_port;
    int rc = SOC_E_NONE;
    soc_block_info_t *bi;
    int is_on;
    soc_interrupt_db_t *interrupts_arr;

    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "No interrupts for device\n")));
        return SOC_E_UNAVAIL;
    }

    interrupts_arr = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info;

    soc_nof_interrupts(unit, &nof_interrupts);
    for (bi_index = 0;  SOC_BLOCK_INFO(unit, bi_index).type >= 0; bi_index++ ) {
        for (inter = 0; inter < nof_interrupts ; inter++) {
            if (!SOC_INFO(unit).block_valid[bi_index]) {
                continue;
            }

            bi = &(SOC_BLOCK_INFO(unit, bi_index));
            if(NULL == bi) {
               /*
                * Coverity
                * This is defencive statement.
                *
                * coverity[dead_error_begin]
                */
                LOG_ERROR(BSL_LS_SOC_INTR,
                          (BSL_META_U(unit,
                                      "Unknown block %d\n"), bi_index));
                return SOC_E_FAIL;
            }

            if (!SOC_INFO(unit).block_valid[bi_index]) {
                continue;
            }

            rc = soc_interrupt_is_valid(unit, bi, &(interrupts_arr[inter]), &is_valid);
            if (SOC_FAILURE(rc)) {
                LOG_ERROR(BSL_LS_SOC_INTR,
                          (BSL_META_U(unit,
                                      "%s\n"), soc_errmsg(rc)));
                return rc;
            }
            if (!is_valid) {
                continue;
            }

#if defined(BCM_PETRA_SUPPORT)
            if(SOC_IS_ARAD(unit) &&  (bi->type == SOC_BLK_CLP || bi->type == SOC_BLK_XLP )) {
                soc_port_if_t if_type;
                int_port = SOC_BLOCK_PORT(unit, bi_index);
                if (int_port & SOC_REG_ADDR_INSTANCE_MASK) {
                    continue;
                }

                rc = soc_port_sw_db_interface_type_get(unit, int_port, &if_type);
                if (if_type == SOC_PORT_IF_ILKN) {
                    continue;
                }
            } else 
#endif /* BCM_PETRA_SUPPORT */
            int_port = bi->number;

            rc = soc_interrupt_get(unit, int_port, &(interrupts_arr[inter]), &is_on );

            if (is_on) {
                if(NULL != interrupts_arr[inter].interrupt_clear) {
                    rc = interrupts_arr[inter].interrupt_clear(unit, int_port , inter);
                    if (SOC_FAILURE(rc)) {
                        LOG_ERROR(BSL_LS_SOC_INTR,
                                  (BSL_META_U(unit,
                                              "%s\n"), soc_errmsg(rc)));
                        return rc;
                    }
                }
            }
        }
    }

    return rc;
}

int soc_interrupt_is_all_clear(int unit, int *is_all_clear){
    soc_interrupt_cause_t interrupt;
    int total = 0;
    int rc = SOC_E_NONE;

    if (is_all_clear == NULL) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Null parameter\n")));
        return SOC_E_PARAM;
    }

    rc = soc_active_interrupts_get(unit, 0x0 ,1, &interrupt, &total);
    if (SOC_FAILURE(rc)) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "%s\n"), soc_errmsg(rc)));
        return rc;
    }

    *is_all_clear = (total == 0);

    return rc;
}

int soc_interrupt_is_all_mask(int unit, int *is_all_mask){
    uint32 mask[4];/*4 is the amount which needed for maximum 128 blocks per device*/
    int i;
    int rc = SOC_E_NONE;
#ifdef BCM_CMICX_SUPPORT
    int interrupt_bit;
#endif

    if (is_all_mask == NULL) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Null parameter\n")));
        return SOC_E_PARAM;
    }

    sal_memset(mask, 0x0, sizeof(mask));

#ifdef BCM_CMICX_SUPPORT
    interrupt_bit = soc_feature(unit, soc_feature_iproc_17) ? IPROC17_CHIP_INTR_LOW_PRIORITY : CHIP_INTR_LOW_PRIORITY;

    if (soc_feature(unit, soc_feature_cmicx)) {
        /* interrupt number is CHIP_INTR_LOW_PRIORITY=119 or 127 for iProc17*/
        return soc_cmic_intr_is_mask(unit, interrupt_bit, is_all_mask);
    }
#endif

    if (!soc_feature(unit, soc_feature_cmicm)) {
        mask[0] = SOC_IRQ1_MASK(unit);
        mask[1] = SOC_IRQ2_MASK(unit);
    } else {
    #ifdef BCM_CMICM_SUPPORT
        mask[0] = SOC_CMCx_IRQ3_MASK(unit, SOC_PCI_CMC(unit));
        mask[1] =  SOC_CMCx_IRQ4_MASK(unit, SOC_PCI_CMC(unit));
        if (soc_feature(unit, soc_feature_cmicm_extended_interrupts)) {
            mask[2] = SOC_CMCx_IRQ3_MASK(unit, SOC_PCI_CMC(unit));
            mask[3] =  SOC_CMCx_IRQ4_MASK(unit, SOC_PCI_CMC(unit));
        }
    #endif
    }
    for(i = 0; i < 4; i++) {
        if (mask[i]) {
            *is_all_mask = FALSE;
            return rc;
        }
    }

    *is_all_mask = TRUE;

    return rc;
}

/* 
 * Statistics functions
 */
int soc_interrupt_stat_cnt_increase(int unit, int bi,  int interrupt_id)
{
    int nof_interrupts;
    soc_interrupt_db_t *intr_id_db;
    int rc = SOC_E_NONE;

    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "No interrupts for device\n")));
        return SOC_E_UNAVAIL;
    }

    /*verify interrupt_id*/
    soc_nof_interrupts(unit, &nof_interrupts);
    if ((interrupt_id > nof_interrupts) || interrupt_id < 0) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "event_id is unavail\n")));
        return SOC_E_UNAVAIL;
    }

    /* Get specific interrupt soc db */
    intr_id_db = &(SOC_CONTROL(unit)->interrupts_info->interrupt_db_info[interrupt_id]);

    /* Increase statistics count */
    (intr_id_db->statistics_count[bi]) ++;

    return rc;
}

/*
 * Sort interrupts according to priority 
 */
int soc_sort_interrupts_according_to_priority(int unit, soc_interrupt_cause_t* interrupts, uint32 interrupts_size)
{

    int i,j;
    int left_interrupt_priority,right_interrupt_priority;
    int stop_check_flag;
    uint32 left_intr_flags, right_intr_flags;
    soc_interrupt_cause_t tmp;
    int rc = SOC_E_NONE;

    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "No interrupts for device\n")));
        return SOC_E_UNAVAIL;
    }

    if (interrupts == NULL) {
        LOG_ERROR(BSL_LS_SOC_INTR,
                  (BSL_META_U(unit,
                              "Null parameter\n")));
        return SOC_E_PARAM;
    }

    for(i=interrupts_size-2 ;i>=0;--i) {
        stop_check_flag=1;

        for(j=0;j<=i;j++) {
            rc = soc_interrupt_flags_get(unit, interrupts[j].id, &left_intr_flags);
            if (SOC_FAILURE(rc)) {
                LOG_ERROR(BSL_LS_SOC_INTR,
                          (BSL_META_U(unit,
                                      "%s\n"), soc_errmsg(rc)));
                return rc;
            }
            rc = soc_interrupt_flags_get(unit, interrupts[j+1].id, &right_intr_flags);
            if (SOC_FAILURE(rc)) {
                LOG_ERROR(BSL_LS_SOC_INTR,
                          (BSL_META_U(unit,
                                      "%s\n"), soc_errmsg(rc)));
                return rc;
            }
            left_interrupt_priority = ((left_intr_flags & SOC_INTERRUPT_DB_FLAGS_PRIORITY_MASK) >> SOC_INTERRUPT_DB_FLAGS_PRIORITY_BITS_LSB);
            right_interrupt_priority = ((right_intr_flags & SOC_INTERRUPT_DB_FLAGS_PRIORITY_MASK) >> SOC_INTERRUPT_DB_FLAGS_PRIORITY_BITS_LSB);

            if(left_interrupt_priority > right_interrupt_priority) {   
                tmp = interrupts[j];  
                interrupts[j] = interrupts[j+1];
                interrupts[j+1] = tmp;
                stop_check_flag = 0;  
            }
        }

        if(stop_check_flag == 1) {
            break;
        }
    }

    return rc;
}

#endif /* defined(BCM_ESW_SUPPORT) || defined(BCM_DFE_SUPPORT)|| defined(BCM_PETRA_SUPPORT) || defined(PORTMOD_SUPPORT)*/

#ifdef BCM_CMICX_SUPPORT

/*******************************************
* @function soc_cmic_intr_enable
* purpose Enable a particular interrupt
*
* @param unit [in] unit
* @param param [in] intr_num_t, Interrupt Number
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
int
soc_cmic_intr_enable(int unit, intr_num_t intr)
{
    int rv;

   if (_cmic_intr_op[unit].soc_cmic_intr_enable == NULL) {
        LOG_FATAL(BSL_LS_SOC_INTR, (BSL_META_U(unit,
                  "soc_cmic_intr_enable function is undefined\n")));
        rv = SOC_E_FAIL;
    } else {
        rv =  _cmic_intr_op[unit].soc_cmic_intr_enable(unit, intr);
    }

    return rv;
}

/*******************************************
* @function soc_cmic_intr_disable
* purpose Enable a particular interrupt
*
* @param unit [in] unit
* @param param [in] intr_num_t, Interrupt Number
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
int
soc_cmic_intr_disable(int unit, intr_num_t intr)
{
   int rv;

   if (_cmic_intr_op[unit].soc_cmic_intr_disable == NULL) {
        LOG_FATAL(BSL_LS_SOC_INTR, (BSL_META_U(unit,
                  "soc_cmic_intr_disable function is undefined\n")));
        rv = SOC_E_FAIL;
    } else {
        rv =  _cmic_intr_op[unit].soc_cmic_intr_disable(unit, intr);
    }

    return rv;
}

/*******************************************
* @function soc_cmic_intr_dump
* purpose dump registers particular interrupt
*
* @param unit [in] unit
* @param param [in] intr_num_t, Interrupt Number
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
extern int
soc_cmic_intr_dump(int unit, intr_num_t intr)
{
   int rv;

   if (_cmic_intr_op[unit].soc_cmic_intr_dump == NULL) {
        LOG_FATAL(BSL_LS_SOC_INTR, (BSL_META_U(unit,
                  "soc_cmic_intr_dump function is undefined\n")));
        rv = SOC_E_FAIL;
    } else {
        rv =  _cmic_intr_op[unit].soc_cmic_intr_dump(unit, intr);
    }

    return rv;
}


/*******************************************
* @function soc_cmic_intr_all_enable
* purpose Enable a particular interrupt
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
int
soc_cmic_intr_all_enable(int unit)
{
    int rv;

    if (_cmic_intr_op[unit].soc_cmic_intr_all_enable == NULL) {
        LOG_FATAL(BSL_LS_SOC_INTR, (BSL_META_U(unit,
                  "soc_cmic_intr_all_enable function is undefined\n")));
        rv = SOC_E_FAIL;
    } else {
        rv =  _cmic_intr_op[unit].soc_cmic_intr_all_enable(unit);
    }

    return rv;
}

/*******************************************
* @function soc_cmic_intr_all_disable
* purpose Enable a particular interrupt
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
int
soc_cmic_intr_all_disable(int unit)
{
    int rv;

    if (_cmic_intr_op[unit].soc_cmic_intr_all_disable == NULL) {
        LOG_FATAL(BSL_LS_SOC_INTR, (BSL_META_U(unit,
                  "soc_cmic_intr_all_disable function is undefined\n")));
        rv = SOC_E_FAIL;
    } else {
        rv =  _cmic_intr_op[unit].soc_cmic_intr_all_disable(unit);
    }

    return rv;
}

/*******************************************
* @function soc_cmic_intr_is_mask
* purpose Get a particular interrupt mask
*
* @param unit [in] unit
* @param param [in] intr_num_t, Interrupt Number
* @praram param [out] int, mask
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
int
soc_cmic_intr_is_mask(int unit, intr_num_t intr, int *mask)
{
    int rv;

    if (_cmic_intr_op[unit].soc_cmic_intr_is_mask == NULL) {
        LOG_FATAL(BSL_LS_SOC_INTR, (BSL_META_U(unit,
                  "soc_cmic_intr_is_mask function is undefined\n")));
        rv = SOC_E_FAIL;
    } else {
        rv =  _cmic_intr_op[unit].soc_cmic_intr_is_mask(unit, intr, mask);
    }

    return rv;
}

/*******************************************
* @function soc_cmic_intr_register
* purpose Register the interrupt handler
*
* @param unit [in] unit
* @param param [in] soc_cmic_intr_handler_t pointer
* @param param [in] int, size of the array elements
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
int
soc_cmic_intr_register(int unit, soc_cmic_intr_handler_t *handle, int size)
{
   int rv;

   if (_cmic_intr_op[unit].soc_cmic_intr_register == NULL) {
        LOG_FATAL(BSL_LS_SOC_INTR, (BSL_META_U(unit,
                  "soc_cmicx_intr_register function is undefined\n")));
        rv = SOC_E_FAIL;
    } else {
        rv =  _cmic_intr_op[unit].soc_cmic_intr_register(unit,
                                                          handle,
                                                          size);
    }

    return rv;
}

/*******************************************
* @function soc_cmic_intr_init
* purpose initialize CMICX interrupt framework
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
int
soc_cmic_intr_init(int unit)
{
    int rv = SOC_E_FAIL;

#ifdef BCM_CMICX_SUPPORT
   if (soc_feature(unit, soc_feature_cmicx)) {
       rv = soc_cmicx_intr_init(unit, &_cmic_intr_op[unit]);
   }
#endif

   return rv;
}
#endif /* defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT) || defined(PORTMOD_SUPPORT)*/
