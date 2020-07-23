/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _SOC_CMICM_H
#define _SOC_CMICM_H

#include <soc/register.h>
#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)
#include <soc/mcm/cmicm.h>
#include <soc/cmic.h>
#endif
#include <soc/shared/mos_intr_common.h>

/* Use this file for stuff that are not Auto-Generated */

#define CMIC_CMC_NUM_MAX        3

#define CMC0 0
#define CMC1 1
#define CMC2 2

/* SBUSDMA stuff */
#define CCMDMA_TIMEOUT          (5   * SECOND_USEC)
#define CCMDMA_TIMEOUT_QT       (120 * SECOND_USEC)
#define CMIC_CMCx_CCMDMA_CHAN_MAX      (1)
#define CMIC_CMC0_CCMDMA_CH_MASK       (0x1)
#define CMIC_CMC1_CCMDMA_CH_MASK       (0x1)
#define CMIC_CMC2_CCMDMA_CH_MASK       (0x1)

/* CMIC_CMCx_CCM_DMA_CFG */
#define CCMDMA_ENABLE                   (0x00000001)
#define CCMDMA_ABORT                    (0x00000002)

/* CMIC_CMCx_CCM_DMA_STATUS */
#define ST_CMCx_CCMDMA_DONE             (0x00000001)

/* Extra "utility" Macros, based on the Auto-Generated addresses */
/* x = CMC No, y=Channel No, n=index */

#define CMIC_COMMON_SCHAN_MESSAGEn(n)                    (CMIC_COMMON_SCHAN_MESSAGE_OFFSET + (4*n))
#define CMIC_CMCx_SCHAN_MESSAGEn(x, n)                   (CMIC_CMCx_SCHAN_MESSAGE_OFFSET(x) + (4*n))
#define CMIC_SCHAN_MESSAGE_WORDS_MAX                     22

#define CMIC_SEMAPHORE_n(n)                              (CMIC_SEMAPHORE_1_OFFSET + ((n-1)*8))
#define CMIC_SEMAPHORE_n_SHADOW(n)                       (CMIC_SEMAPHORE_1_SHADOW_OFFSET + ((n-1)*8))
#define CMIC_PKT_PRI_MAP_TABLE_ENTRY_n(n)                (CMIC_PKT_PRI_MAP_TABLE_ENTRY_0_OFFSET + (n*4))
#define CMIC_CMCx_CHy_DMA_CTRL_OFFSET(x,y)               (CMIC_CMCx_CH0_DMA_CTRL_OFFSET(x)+(y*4))
#define CMIC_CMCx_DMA_DESCy_OFFSET(x,y)                  (CMIC_CMCx_DMA_DESC0_OFFSET(x) + (y*4))
#define CMIC_CMCx_CHy_COS_CTRL_RX_0_OFFSET(x,y)          (CMIC_CMCx_CH0_COS_CTRL_RX_0_OFFSET(x) + (y*8))
#define CMIC_CMCx_CHy_COS_CTRL_RX_1_OFFSET(x,y)          (CMIC_CMCx_CH0_COS_CTRL_RX_1_OFFSET(x) + (y*8))
#define CMIC_CMCx_DMA_CHy_INTR_COAL_OFFSET(x,y)          (CMIC_CMCx_DMA_CH0_INTR_COAL_OFFSET(x) + (y*4))
#define CMIC_CMCx_CHy_DMA_CURR_DESC_OFFSET(x,y)          (CMIC_CMCx_CH0_DMA_CURR_DESC_OFFSET(x) + (y*4))
#define CMIC_CMCx_FIFO_CHy_RD_DMA_CFG_OFFSET(x,y)               (CMIC_CMCx_FIFO_CH0_RD_DMA_CFG_OFFSET(x) + (y*4))
#define CMIC_CMCx_FIFO_CHy_RD_DMA_SBUS_START_ADDRESS_OFFSET(x,y) (CMIC_CMCx_FIFO_CH0_RD_DMA_SBUS_START_ADDRESS_OFFSET(x) + (y*4))
#define CMIC_CMCx_FIFO_CHy_RD_DMA_HOSTMEM_START_ADDRESS_OFFSET(x,y) (CMIC_CMCx_FIFO_CH0_RD_DMA_HOSTMEM_START_ADDRESS_OFFSET(x) + (y*4))
#define CMIC_CMCx_FIFO_CHy_RD_DMA_HOSTMEM_READ_PTR_OFFSET(x,y)  (CMIC_CMCx_FIFO_CH0_RD_DMA_HOSTMEM_READ_PTR_OFFSET(x) + (y*8))
#define CMIC_CMCx_FIFO_CHy_RD_DMA_HOSTMEM_WRITE_PTR_OFFSET(x,y) (CMIC_CMCx_FIFO_CH0_RD_DMA_HOSTMEM_WRITE_PTR_OFFSET(x) + (y*8))
#define CMIC_CMCx_FIFO_CHy_RD_DMA_HOSTMEM_THRESHOLD_OFFSET(x,y) (CMIC_CMCx_FIFO_CH0_RD_DMA_HOSTMEM_THRESHOLD_OFFSET(x) + (y*4))
#define CMIC_CMCx_FIFO_CHy_RD_DMA_STAT_OFFSET(x,y)              (CMIC_CMCx_FIFO_CH0_RD_DMA_STAT_OFFSET(x) + (y*4))
#define CMIC_CMCx_FIFO_CHy_RD_DMA_STAT_CLR_OFFSET(x,y)          (CMIC_CMCx_FIFO_CH0_RD_DMA_STAT_CLR_OFFSET(x) + (y*4))
#define CMIC_CMCx_FIFO_CHy_RD_DMA_SBUS_CMD_CONFIG_OFFSET(x,y)   (CMIC_CMCx_FIFO_CH0_RD_DMA_SBUS_CMD_CONFIG_OFFSET(x) + (y*4))
#define CMIC_CMCx_FIFO_CHy_RD_DMA_CUR_HOSTMEM_WRITE_PTR_OFFSET(x,y) (CMIC_CMCx_FIFO_CH0_RD_DMA_CUR_HOSTMEM_WRITE_PTR_OFFSET(x) + (y*4))
#define CMIC_CMCx_FIFO_CHy_RD_DMA_ECCERR_ADDRESS_OFFSET(x,y)    (CMIC_CMCx_FIFO_CH0_RD_DMA_ECCERR_ADDRESS_OFFSET(x) + (y*4))
#define CMIC_CMCx_FIFO_CHy_RD_DMA_ECCERR_CONTROL_OFFSET(x,y)    (CMIC_CMCx_FIFO_CH0_RD_DMA_ECCERR_CONTROL_OFFSET(x) + (y*4))
#define CMIC_CMCx_PKT_COUNT_CHy_TXPKT_OFFSET(x,y)               (CMIC_CMCx_PKT_COUNT_CH0_TXPKT_OFFSET(x) + (y*8))
#define CMIC_CMCx_PKT_COUNT_CHy_RXPKT_OFFSET(x,y)               (CMIC_CMCx_PKT_COUNT_CH0_RXPKT_OFFSET(x) + (y*8))
#define CMIC_CMCx_CHy_RXBUF_THRESHOLD_CONFIG(x,y)               (CMIC_CMCx_CH0_RXBUF_THRESHOLD_CONFIG_OFFSET(x) + (y*4))

/* CMICDV2 supports continuous mode */
#define CMIC_CMCx_DMA_CHy_DESC_HALT_ADDR_OFFSET(x,y)            (CMIC_CMCx_DMA_CH0_DESC_HALT_ADDR_OFFSET(x) + (y*4))

#define MSPI_TXRAM_nn(n)                                (MSPI_TXRAM_00_OFFSET + (4*n))
#define MSPI_RXRAM_nn(n)                                (MSPI_RXRAM_00_OFFSET + (4*n))
#define MSPI_CDRAM_nn(n)                                (MSPI_CDRAM_00_OFFSET + (4*n))

/* Field Masks for some CMICm registers.*/
/* Can be removed if we can use the more generic soc_reg_field_set/get() */

/* CMIC_CMCx_CHy_DMA_CTRL */
#define PKTDMA_DIRECTION                (0x00000001)
#define PKTDMA_ENABLE                   (0x00000002)
#define PKTDMA_ABORT                    (0x00000004)
#define PKTDMA_SEL_INTR_ON_DESC_OR_PKT  (0x00000008)
#define PKTDMA_BIG_ENDIAN               (0x00000010)
#define PKTDMA_DESC_BIG_ENDIAN          (0x00000020)
#define PKTDMA_DROP_RX_PKT_ON_CHAIN_END (0x00000040)
#define RLD_STATUS_UPD_DIS              (0x00000080)
#define PKTDMA_CNTLD_DESC_INTR_MODE     (0x00000100)
#define PKTDMA_CONTINUOUS_ENABLE        (0x00000200) /* Bit 9 */

/* CMIC_CMCx_FIFO_CHy_RD_DMA_CFG */
#define FIFODMA_ENABLE                  (0x00000001)
#define FIFODMA_BIG_ENDIAN              (0x00000800)
#define FIFODMA_ABORT                   (0x80000000)

/* CMIC_CMCx_FIFO_CHy_RD_DMA_STAT */
#define ST_CMCx_FIFODMA_DONE            (0x00000010)

/* CMIC_CMCx_FSCHAN_STATUS(x) */
#define FSCHAN_BUSY                     (0x00000001)

/* CMIC_CMCx_SCHAN_CTRL(x) */
#define SC_CMCx_MSG_CLR                 (0x00000000)
#define SC_CMCx_MSG_START               (0x00000001)
#define SC_CMCx_MSG_DONE                (0x00000002)
#define SC_CMCx_SCHAN_ABORT             (0x00000004)
#define SC_CMCx_MSG_SER_CHECK_FAIL      (0x00100000)
#define SC_CMCx_MSG_NAK                 (0x00200000)
#define SC_CMCx_MSG_TIMEOUT_TST         (0x00400000)

/* CMIC_CMCx_DMA_STAT(x) */
#define DS_CMCx_DMA_CHAIN_DONE(y)       (0x00000001 << (y))
#define DS_CMCx_DMA_DESC_DONE(y)        (0x00000010 << (y))
#define DS_CMCx_DMA_ACTIVE(y)           (0x00000100 << (y))
#define DS_CMCx_DMA_ADDR_DECODE_ERR(y)  (0x00111000 << (y))

/* CMIC_CMCx_DMA_STAT_HI(x) */
#define DS_CMCx_DMA_IN_HALT(y)          (0x08000000 << (y))

/* CMIC_CMCx_DMA_STAT_CLR(x) */
#define DS_DESCRD_CMPLT_CLR(y)          (0x00000001 << (y))
#define DS_INTR_COALESCING_CLR(y)       (0x00000010 << (y))
#define DS_CNTLD_DESC_CLR(y)            (0x00000100 << (y))

/* CMIC_CMCx_SLAM_DMA_CFG(x) */
#define SLDMA_BIG_ENDIAN                (0x40000000)
#define SLDMA_CFG_DIR                   (0x00010000)
#define SLDMA_DONE                      (0x00000001)

/* CMIC_CMCx_TABLE_DMA_CFG(x) */
#define TDMA_BIG_ENDIAN                 (0x00800000)
#define TDMA_DONE                       (0x00000001)

/* CMIC_CMCx_STAT_DMA_CFG(x) */
#define STDMA_BIG_ENDIAN                (0x00040000)
#define STDMA_ITER_DONE_CLR             (0x00020000)

/* CMIC_CMCx_STAT_DMA_STAT(x) */
#define ST_CMCx_DMA_ITER_DONE           (0x00000002)
#define ST_CMCx_DMA_ERR                 (0x00000004)
#define ST_CMCx_DMA_ACTIVE              (0x00000008)

/* CMIC_CMCx_MIIM_STAT(x) */
#define CMIC_MIIM_OPN_DONE                  (0x00000001)

/* CMIC_MIIM_SCAN_STATUS */
#define CMIC_MIIM_SCAN_BUSY                 (0x00000001)

/* CMIC_CPS_RESET */
#define CMIC_CPS_RESET                      (0x00000001)

/* CMIC_MIIM_CLR_SCAN_STATUS */
#define CLR_LINK_STATUS_CHANGE_MASK         (0x00000010) /*bit 4 */

/* CMIC RQ Mask/Stat 0 */
#define IRQ_CMCx_TSLAM_DONE                 0x00000001 /* Bit 0 */
#define IRQ_CMCx_TDMA_DONE                  0x00000002 /* Bit 1 */
#define IRQ_CMCx_FIFO_CH_DMA(ch)            (0x00000020 >> ch) /* Bits 5,4,3,2 */
#define IRQ_CMCx_STAT_ITER_DONE             0x00000040 /* Bit 6 */
#define IRQ_CMCx_SBUSDMA_CH_DONE(ch)        ((ch) == 2 ? 0x40 : (1 << (ch)))
#define IRQ_CMCx_MIIM_OP_DONE               0x00000080 /* Bit 7 */
#define IRQ_CMCx_DESC_DONE(ch)              (0x00004000 >> (2 * (ch))) /* Bits 14,12,10,8 */
#define IRQ_CMCx_CHAIN_DONE(ch)             (0x00008000 >> (2 * (ch))) /* Bits 15,13,11,9 */
#define IRQ_CMCx_INTR_COAL_INTR(ch)         (0x00080000 >> ch) /* Bits 19,18,17,16 */
#define IRQ_CMCx_SCH_OP_DONE                0x00100000 /* Bit 20 */
#define IRQ_CMCx_CCMDMA_DONE                0x00200000 /* Bit 21 */
#define IRQ_CMCx_SW_INTR(host)              (0x00400000 << host) /* Bits 22-25 */
#define IRQ_RCPU_MIIM_OP_DONE               0x00000001 /* Bit 0 */
#define IRQ_CMCx_CNTLD_DESC_DONE(ch)        (0x08000000 << ch) /* Bits 27-30 */

/* CMICM updates */
#define IRQ_SBUSDMA_CH0_DONE 0x00000002
#define IRQ_SBUSDMA_CH1_DONE 0x00000001
#define IRQ_SBUSDMA_CH2_DONE 0x00000040

/* CMIC RQ Mask/Stat 1 */
#define IRQ_CMCx_I2C_INTR                   0x00000001 /* Bit 0 */
#define IRQ_CMCx_BROADSYNC_INTR             0x00000002 /* Bit 1 */
#define IRQ_CMCx_TIMESYNC_INTR              0x00000004 /* Bit 2 */
#define IRQ_CMCx_LINK_STAT_MOD              0x00000008 /* Bit 3 */
#define IRQ_CMCx_PHY_PAUSESCAN_PAUSESTATUS_CHD 0x00000010 /* Bit 4 */
#define IRQ_CMCx_SPI_INTERRUPT              0x00000020 /* Bit 5 */
#define IRQ_CMCx_UART0_INTERRUPT            0x00000040 /* Bit 6 */
#define IRQ_CMCx_UART1_INTERRUPT            0x00000080 /* Bit 7 */
#define IRQ_CMCx_COMMON_SCHAN_DONE          0x00000100 /* Bit 8 */
#define IRQ_CMCx_COMMON_MIIM_OP_DONE        0x00000200 /* Bit 9 */
#define IRQ_CMCx_GPIO_INTR                  0x00000400 /* Bit 10 */
#define IRQ_CMCx_CHIP_FUNC_INTR             0x0007f800 /* Bit 11-18 */
#define IRQ_CMCx_UC_0_PMUIRQ                0x00080000 /* Bit 19 */
#define IRQ_CMCx_UC_1_PMUIRQ                0x00100000 /* Bit 20 */
#define IRQ_CMCx_WDT_O_INTR                 0x00200000 /* Bit 21 */
#define IRQ_CMCx_WDT_1_INTR                 0x00400000 /* Bit 22 */
#define IRQ_CMCx_TIM0_INTR1                 0x00800000 /* Bit 23 */
#define IRQ_CMCx_TIM0_INTR2                 0x01000000 /* Bit 24 */
#define IRQ_CMCx_TIM1_INTR1                 0x02000000 /* Bit 25 */
#define IRQ_CMCx_TIM1_INTR2                 0x04000000 /* Bit 26 */
#define IRQ_CMCx_PCIEINTF_NEEDS_CLEANUP     0x08000000 /* Bit 27 */
#define IRQ_CMCx_SER_INTR                   0x20000000 /* Bit 29 */

/* CMIC Mask/Stat 2 (Parity) */
#define IRQ_CMCx_PARITY                     0xFFFFFFFF /* 32 bits */

/* CMIC PCI completion interrupt */
#define IRQ_CMCx_PCI_CMPL_MASK              0x00080000 /* Bit 19 */
#define PCIE_CMPL_TIMEOUT_INTR_MASK         0x00000040
#define PORTA_2B_ECC_ERR_INTR_EN            0x78000

/* CMIC Mask/Stat 3 (sbus slave 0-31) */
/* CMIC Mask/Stat 4 (sbus slave 32-63) */
#define IRQ_CMCx_BLOCK(bit)                 (1U<<(bit) ) /* Bit 0-31  */


/* SCHAN macros for common CMC, CMC 0, CMC 1, and CMC 2*/
#define CMIC_COMMON_CMCx_SCHAN_CTRL_OFFSET(cmc) (cmc == CMIC_CMC_NUM_MAX) ? \
            CMIC_COMMON_SCHAN_CTRL_OFFSET : CMIC_CMCx_SCHAN_CTRL_OFFSET(cmc);
#define CMIC_COMMON_CMCx_SCHAN_ERR_OFFSET(cmc)  (cmc == CMIC_CMC_NUM_MAX) ? \
            CMIC_COMMON_SCHAN_ERR_OFFSET  : CMIC_CMCx_SCHAN_ERR_OFFSET(cmc);
#define CMIC_COMMON_CMCx_SCHAN_MESSAGEn(cmc, i) (cmc == CMIC_CMC_NUM_MAX) ? \
            CMIC_COMMON_SCHAN_MESSAGEn(i) :CMIC_CMCx_SCHAN_MESSAGEn(cmc, i);


/* SBUSDMA stuff */
#define CMIC_CMCx_SBUSDMA_CHAN_MAX      (3)
#define CMIC_CMC0_SBUSDMA_CH_MASK       (0x7)
#define CMIC_CMC1_SBUSDMA_CH_MASK       (0x7)
#define CMIC_CMC2_SBUSDMA_CH_MASK       (0x7)

#define CMIC_CMCx_SBUSDMA_CHy_CONTROL(x, y)                   \
            (CMIC_CMCx_SBUSDMA_CH0_CONTROL_OFFSET(x)                                  + (y * 0x50))
#define CMIC_CMCx_SBUSDMA_CHy_REQUEST(x, y)                   \
            (CMIC_CMCx_SBUSDMA_CH0_REQUEST_OFFSET(x)                                  + (y * 0x50))
#define CMIC_CMCx_SBUSDMA_CHy_COUNT(x, y)                     \
            (CMIC_CMCx_SBUSDMA_CH0_COUNT_OFFSET(x)                                    + (y * 0x50))
#define CMIC_CMCx_SBUSDMA_CHy_OPCODE(x, y)                    \
            (CMIC_CMCx_SBUSDMA_CH0_OPCODE_OFFSET(x)                                   + (y * 0x50))
#define CMIC_CMCx_SBUSDMA_CHy_ADDRESS(x, y)                   \
            (CMIC_CMCx_SBUSDMA_CH0_SBUS_START_ADDRESS_OFFSET(x)                       + (y * 0x50))
#define CMIC_CMCx_SBUSDMA_CHy_HOSTADDR(x, y)                  \
            (CMIC_CMCx_SBUSDMA_CH0_HOSTMEM_START_ADDRESS_OFFSET(x)                    + (y * 0x50))
#define CMIC_CMCx_SBUSDMA_CHy_DESCADDR(x, y)                  \
            (CMIC_CMCx_SBUSDMA_CH0_DESC_START_ADDRESS_OFFSET(x)                       + (y * 0x50))
#define CMIC_CMCx_SBUSDMA_CHy_STATUS(x, y)                    \
            (CMIC_CMCx_SBUSDMA_CH0_STATUS_OFFSET(x)                                   + (y * 0x50))
#define CMIC_CMCx_SBUSDMA_CHy_CUR_SBUSADDR(x, y)              \
            (CMIC_CMCx_SBUSDMA_CH0_CUR_SBUSDMA_CONFIG_SBUS_START_ADDRESS_OFFSET(x)    + (y * 0x50))
#define CMIC_CMCx_SBUSDMA_CHy_CUR_SBUSDMA_CONFIG_OPCODE(x, y) \
            (CMIC_CMCx_SBUSDMA_CH0_CUR_SBUSDMA_CONFIG_OPCODE_OFFSET(x)                + (y * 0x50))
#define CMIC_CMCx_SBUSDMA_CHy_CUR_SBUSDMA_CONFIG_ADDR(x, y)   \
            (CMIC_CMCx_SBUSDMA_CH0_CUR_SBUSDMA_CONFIG_HOSTMEM_START_ADDRESS_OFFSET(x) + (y * 0x50))

/* CMIC_CMCx_SBUSDMA_CHy_CONTROL fields */
#define SBUSDMA_START                       (0x00000001)
#define SBUSDMA_ABORT                       (0x00000002)
#define SBUSDMA_MODE                        (0x00000004)
#define SBUSDMA_DESC_ENDIANESS              (0x00000008)

/* CMIC_CMCx_SBUSDMA_CHy_STATUS fields */
#define ST_CMCx_SBUSDMA_DONE                (0x00000001)

/* FIFO DMA stuff */
#define CMIC_CMCx_FIFO_CHy_RD_DMA_NUM_OF_ENTRIES_VALID_IN_HOSTMEM_OFFSET(x, y) \
            (CMIC_CMCx_FIFO_CH0_RD_DMA_NUM_OF_ENTRIES_VALID_IN_HOSTMEM_OFFSET(x) + (y * 8))
#define CMIC_CMCx_FIFO_CHy_RD_DMA_NUM_OF_ENTRIES_READ_FRM_HOSTMEM_OFFSET(x, y) \
            (CMIC_CMCx_FIFO_CH0_RD_DMA_NUM_OF_ENTRIES_READ_FRM_HOSTMEM_OFFSET(x) + (y * 8))
#define CMIC_CMCx_FIFO_CHy_RD_DMA_OPCODE_OFFSET(x, y)                          \
            (CMIC_CMCx_FIFO_CH0_RD_DMA_OPCODE_OFFSET(x)                          + (y * 4))

/* This macro may be used to transfer T from microseconds to the value to be entered into
   CMIC_CMC*_FIFO_CH*_RD_DMA_CFG.TIMEOUT_COUNT. */
#define MICROSECONDS_TO_DMA_CFG__TIMEOUT_COUNT(T) (T = (T+249) / 250); 

extern int _soc_mem_address_remap_allocation_init(int unit);
extern int _soc_mem_address_remap_alloc(int unit, int cmc, int ch, int nof_entries, uint32 remap_val, uint32 *remap_entry_base);
extern int _soc_mem_address_remap_dealloc(int unit, int cmc, int ch);
extern int _soc_mem_sbus_fifo_dma_start_memreg(int unit, int ch, 
                                    int is_mem, soc_mem_t mem, soc_reg_t reg, 
                                    int copyno, int force_entry_size, int host_entries, void *host_buff);

extern int _soc_mem_sbus_fifo_dma_start(int unit, int ch, soc_mem_t mem, int copyno,
                                        int host_entries, void *host_buff);
extern int _soc_mem_sbus_fifo_dma_stop(int unit, int ch);

extern int _soc_mem_sbus_fifo_dma_get_num_entries(int unit, int ch, int *count);

extern int _soc_mem_sbus_fifo_dma_set_entries_read(int unit, int ch, uint32 num);

#endif  /* !_SOC_CMICM_H */
