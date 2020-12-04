/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */



/*
 * cmicx_packet_dma.h
 *
 * this file contains header definitions for the cmicx sim packet dma functionality
 */

#ifndef _CMICX_PACKET_DMA_H_
#define _CMICX_PACKET_DMA_H_

/* includes */

/* defines for packet dma register bits */
#define CMIC_TOP_CONFIG_REG_ENABLE_PIO_WRITE_NON_POSTED_BIT     0
#define CMIC_TOP_CONFIG_REG_IP_2_EP_LOOPBACK_ENABLE_BIT         1
#define CMIC_PKTDMA_INTR_COAL_REG_ENABLE_BIT                   31
#define CMIC_PKTDMA_INTR_COAL_REG_COUNT_LSB                    16
#define CMIC_PKTDMA_INTR_COAL_REG_COUNT_MSB                    30
#define CMIC_PKTDMA_INTR_COAL_REG_TIMER_LSB                     0
#define CMIC_PKTDMA_INTR_COAL_REG_TIMER_MSB                    15
#define CMIC_PKTDMA_CTRL_REG_DIRECTION_BIT                      0
#define CMIC_PKTDMA_CTRL_REG_DMA_EN_BIT                         1
#define CMIC_PKTDMA_CTRL_REG_ABORT_DMA_BIT                      2
#define CMIC_PKTDMA_CTRL_REG_PKTDMA_ENDIANNESS_BIT              3
#define CMIC_PKTDMA_CTRL_REG_DESC_ENDIANNESS_BIT                4
#define CMIC_PKTDMA_CTRL_REG_DROP_RX_PKT_ON_CHAIN_END_BIT       5
#define CMIC_PKTDMA_CTRL_REG_RLD_STS_UPD_DIS_BIT                6
#define CMIC_PKTDMA_CTRL_REG_DESC_DONE_INTR_MODE_BIT            7
#define CMIC_PKTDMA_CTRL_REG_ENABLE_CONTINUOUS_DMA_BIT          8
#define CMIC_PKTDMA_CTRL_REG_CONTIGUOUS_DESCRIPTORS_BIT         9
#define CMIC_PKTDMA_CTRL_REG_EP_TO_CPU_HEADER_SIZE_DWORDS_LSB  10
#define CMIC_PKTDMA_CTRL_REG_EP_TO_CPU_HEADER_SIZE_DWORDS_MSB  15
#define CMIC_PKTDMA_CTRL_REG_EP_TO_CPU_ALIGNMENT_BYTES_LSB     16
#define CMIC_PKTDMA_CTRL_REG_EP_TO_CPU_ALIGNMENT_BYTES_MSB     17
#define CMIC_PKTDMA_CTRL_REG_EP_TO_CPU_HEADER_ENDIANNESS_BIT   18
#define CMIC_PKTDMA_STAT_REG_CHAIN_DONE_BIT                     0
#define CMIC_PKTDMA_STAT_REG_DMA_ACTIVE_BIT                     1
#define CMIC_PKTDMA_STAT_REG_STWT_ADDR_DECODE_ERR_BIT           2
#define CMIC_PKTDMA_STAT_REG_PKTWRRD_ADDR_DECODE_ERR_BIT        3
#define CMIC_PKTDMA_STAT_REG_DESCRD_ADDR_DECODE_ERR_BIT         4
#define CMIC_PKTDMA_STAT_REG_PKTWR_ECC_ERR_BIT                  5
#define CMIC_PKTDMA_STAT_REG_CH_IN_HALT_BIT                     6
#define CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH0_DESC_DONE_BIT               0
#define CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH0_CHAIN_DONE_BIT              1
#define CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH0_INTR_COALESCING_INTR_BIT    2
#define CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH0_DESC_CONTROLLED_INTR_BIT    3
#define CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH1_DESC_DONE_BIT               4
#define CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH1_CHAIN_DONE_BIT              5
#define CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH1_INTR_COALESCING_INTR_BIT    6
#define CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH1_DESC_CONTROLLED_INTR_BIT    7
#define CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH2_DESC_DONE_BIT               8
#define CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH2_CHAIN_DONE_BIT              9
#define CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH2_INTR_COALESCING_INTR_BIT   10
#define CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH2_DESC_CONTROLLED_INTR_BIT   11
#define CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH3_DESC_DONE_BIT              12
#define CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH3_CHAIN_DONE_BIT             13
#define CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH3_INTR_COALESCING_INTR_BIT   14
#define CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH3_DESC_CONTROLLED_INTR_BIT   15
#define CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH4_DESC_DONE_BIT              16
#define CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH4_CHAIN_DONE_BIT             17
#define CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH4_INTR_COALESCING_INTR_BIT   18
#define CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH4_DESC_CONTROLLED_INTR_BIT   19
#define CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH5_DESC_DONE_BIT              20
#define CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH5_CHAIN_DONE_BIT             21
#define CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH5_INTR_COALESCING_INTR_BIT   22
#define CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH5_DESC_CONTROLLED_INTR_BIT   23
#define CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH6_DESC_DONE_BIT              24
#define CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH6_CHAIN_DONE_BIT             25
#define CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH6_INTR_COALESCING_INTR_BIT   26
#define CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH6_DESC_CONTROLLED_INTR_BIT   27
#define CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH7_DESC_DONE_BIT              28
#define CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH7_CHAIN_DONE_BIT             29
#define CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH7_INTR_COALESCING_INTR_BIT   30
#define CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH7_DESC_CONTROLLED_INTR_BIT   31

/* defines for packet dma descriptors */
#define CMIC_PKTDMA_DESC_DWORD2_BYTE_COUNT_LSB                        0
#define CMIC_PKTDMA_DESC_DWORD2_BYTE_COUNT_MSB                       15
#define CMIC_PKTDMA_DESC_DWORD2_CHAIN_BIT                            16
#define CMIC_PKTDMA_DESC_DWORD2_SCATTER_BIT                          17
#define CMIC_PKTDMA_DESC_DWORD2_GATHER_BIT                           17
#define CMIC_PKTDMA_DESC_DWORD2_RELOAD_BIT                           18
#define CMIC_PKTDMA_DESC_DWORD2_HIGIG_PKT_BIT                        19
#define CMIC_PKTDMA_DESC_DWORD2_PURGE_PACKET_BIT                     22
#define CMIC_PKTDMA_DESC_DWORD2_INTERRUPT_LSB                        23
#define CMIC_PKTDMA_DESC_DWORD2_INTERRUPT_MSB                        24
#define CMIC_PKTDMA_DESC_DWORD2_DESCRIPTORS_REMAINING_LSB            25
#define CMIC_PKTDMA_DESC_DWORD2_DESCRIPTORS_REMAINING_MSB            28
#define CMIC_PKTDMA_DESC_DWORD3_PACKET_BYTES_TRANSFERRED_LSB          0
#define CMIC_PKTDMA_DESC_DWORD3_PACKET_BYTES_TRANSFERRED_MSB         15
#define CMIC_PKTDMA_DESC_DWORD3_PACKET_END_BIT                       16
#define CMIC_PKTDMA_DESC_DWORD3_PACKET_START_BIT                     17
#define CMIC_PKTDMA_DESC_DWORD3_PACKET_CELL_ERROR_BIT                18
#define CMIC_PKTDMA_DESC_DWORD3_PACKET_DATA_READ_ECC_ERROR_BIT       19
#define CMIC_PKTDMA_DESC_DWORD3_PACKET_COMPLETION_ERROR_BIT          30
#define CMIC_PKTDMA_DESC_DWORD3_PACKET_DONE_BIT                      31


/* defines for packet dma functionality */
#define CMIC_PACKET_DMA_ACCESS_COUNTDOWN_VALUE       4
#define CMIC_PACKET_DMA_ID_STRING_LENGTH            32
#define CMIC_PACKET_DMA_WORDS_PER_DESCRIPTOR         4
#define CMIC_PACKET_DMA_COS_COUNT                   64
#define CMIC_PACKET_DMA_PACKET_DATA_BUFFER_SIZE   1600
#define CMIC_PACKET_DMA_BYTES_PER_LINE               8
#define CMIC_PACKET_DMA_TEMP_PACKET_DATA_SIZE   (CMIC_PACKET_DMA_PACKET_DATA_BUFFER_SIZE+2*CMICX_BYTES_PER_WORD)


/* enum for packet dma current state */


/* packet dma data struct - contains all the data necessary for packet dma handling */
typedef struct {

    /* identification string for this packet dma module */
    char                 id_string[CMIC_PACKET_DMA_ID_STRING_LENGTH];
    int                  channel_number;

    /* control register values */
    bool                 ctrl_direction;
    bool                 ctrl_dma_en;
    bool                 ctrl_abort_dma;
    bool                 ctrl_pktdma_endianness;
    bool                 ctrl_desc_endianness;
    bool                 ctrl_drop_rx_pkt_on_chain_end;
    bool                 ctrl_rld_sts_upd_dis;
    bool                 ctrl_desc_done_intr_mode;
    bool                 ctrl_enable_continuous_dma;
    bool                 ctrl_contiguous_descriptors;
    int                  ctrl_ep_to_cpu_header_size_dwords;
    int                  ctrl_ep_to_cpu_alignment_bytes;
    bool                 ctrl_ep_to_cpu_header_endianness;

    /* internal control values */
    bool                 last_desc;                   /* indicates the last descriptor in a descriptor stream */
    bool                 last_packet_chunk;           /* indicates that we are dealing with the last chunk in a packet. we either start a new packet or end access after this. */
    bool                 packet_chunk_countdown;      /* holds the current countdown for completing the transfer of this packet chunk. when this reaches 0, the chunk is marked as complete. */
    bool                 chain_pending;               /* indicates that we are still chaining descriptors, regardless of whether we are mid-packet or mid-packet-chunk or not */
    bool                 waiting_on_packet;           /* indicates whether this channel is currently waiting for valid packet data or not (used in the rx case). */
    bool                 header_bytes_remaining;      /* indicates the number of bytes left in the header of a packet, before the header is fully transferred. */

    /* interrupt coalescing values */
    bool                 intr_coal_enable;
    int                  intr_coal_count;
    int                  intr_coal_timer;
    int                  intr_coal_cur_desc_done_count;
    int                  intr_coal_cur_chain_done_count;
    int                  intr_coal_cur_timer;

    /* stat register values */
    bool                 stat_ch_in_halt;
    bool                 stat_pktwr_ecc_err;
    bool                 stat_descrd_addr_decode_err;
    bool                 stat_pktwrrd_addr_decode_err;
    bool                 stat_stwt_addr_decode_err;
    bool                 stat_dma_active;
    bool                 stat_chain_done;
    bool                 irq_stat_desc_done;
    bool                 irq_stat_chain_done;
    bool                 irq_stat_intr_coalescing_intr;
    bool                 irq_stat_desc_controlled_intr;

    /* cos control register values */
    bool                 cos_ctrl_bmp[CMIC_PACKET_DMA_COS_COUNT];

    /* pkt count register values */
    reg_t                txpkt_count;
    reg_t                rxpkt_count;

    /* descriptor address values */
    uint64_t             desc_addr;
    uint64_t             desc_halt_addr;
    uint64_t             cur_desc_addr;

    /* values garnered from descriptors */
    uint64_t             packet_mem_addr;
    int                  desc_byte_count;
    bool                 desc_reload;
    bool                 desc_higig_pkt;
    bool                 desc_purge_packet;
    char                 desc_interrupt;
    int                  desc_descriptors_remaining;
    int                  desc_packet_bytes_transferred;
    bool                 desc_packet_end_bit;
    bool                 desc_packet_start_bit;
    bool                 desc_packet_cell_error_bit;
    bool                 desc_packet_data_read_ecc_error_bit;
    bool                 desc_packet_completion_error_bit;
    bool                 desc_packet_done_bit;

    /* storage for buffered packet data on this channel */
    char                 packet_data_buffer[CMIC_PACKET_DMA_PACKET_DATA_BUFFER_SIZE];
    int                  current_buffer_index;
    int                  current_buffer_read_index;

} cmicx_packet_dma_data_t;



/* packet dma register support functions */
bool cmic_pktdma_ctrl_read(reg_t *reg,cmicx_packet_dma_data_t *);
bool cmic_pktdma_ctrl_write(reg_t reg,cmicx_packet_dma_data_t *);
bool cmic_pktdma_stat_read(reg_t *reg,cmicx_packet_dma_data_t *);
bool cmic_pktdma_intr_coal_read(reg_t *reg,cmicx_packet_dma_data_t *);
bool cmic_pktdma_intr_coal_write(reg_t reg,cmicx_packet_dma_data_t *);
bool cmic_pktdma_cos_ctrl_rx_0_read( reg_t *reg,cmicx_packet_dma_data_t *);
bool cmic_pktdma_cos_ctrl_rx_0_write(reg_t  reg,cmicx_packet_dma_data_t *);
bool cmic_pktdma_cos_ctrl_rx_1_read( reg_t *reg,cmicx_packet_dma_data_t *);
bool cmic_pktdma_cos_ctrl_rx_1_write(reg_t  reg,cmicx_packet_dma_data_t *);
/*
bool cmic_pktdma_shared_irq_stat0_read(reg_t *val,cmicx_cmc_t *cmc);
bool cmic_pktdma_shared_irq_stat_clr0_write(reg_t val,cmicx_cmc_t *cmc);
*/

/* packet dma register access functions */
pcid_read_func_t  cmic_top_config_read;
pcid_write_func_t cmic_top_config_write;
pcid_read_func_t  cmic_top_epintf_release_all_credits_read;
pcid_write_func_t cmic_top_epintf_release_all_credits_write;
pcid_read_func_t  cmic_cmc0_shared_irq_stat0_read;
pcid_write_func_t cmic_cmc0_shared_irq_stat0_write;
pcid_read_func_t  cmic_cmc0_shared_irq_stat_clr0_read;
pcid_write_func_t cmic_cmc0_shared_irq_stat_clr0_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch0_ctrl_read;
pcid_write_func_t cmic_cmc0_pktdma_ch0_ctrl_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch0_desc_addr_lo_read;
pcid_write_func_t cmic_cmc0_pktdma_ch0_desc_addr_lo_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch0_desc_addr_hi_read;
pcid_write_func_t cmic_cmc0_pktdma_ch0_desc_addr_hi_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch0_desc_halt_addr_lo_read;
pcid_write_func_t cmic_cmc0_pktdma_ch0_desc_halt_addr_lo_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch0_desc_halt_addr_hi_read;
pcid_write_func_t cmic_cmc0_pktdma_ch0_desc_halt_addr_hi_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch0_stat_read;
pcid_write_func_t cmic_cmc0_pktdma_ch0_stat_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch0_cos_ctrl_rx_0_read;
pcid_write_func_t cmic_cmc0_pktdma_ch0_cos_ctrl_rx_0_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch0_cos_ctrl_rx_1_read;
pcid_write_func_t cmic_cmc0_pktdma_ch0_cos_ctrl_rx_1_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch0_intr_coal_read;
pcid_write_func_t cmic_cmc0_pktdma_ch0_intr_coal_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch0_curr_desc_lo_read;
pcid_write_func_t cmic_cmc0_pktdma_ch0_curr_desc_lo_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch0_curr_desc_hi_read;
pcid_write_func_t cmic_cmc0_pktdma_ch0_curr_desc_hi_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch0_pkt_count_rxpkt_read;
pcid_write_func_t cmic_cmc0_pktdma_ch0_pkt_count_rxpkt_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch0_pkt_count_txpkt_read;
pcid_write_func_t cmic_cmc0_pktdma_ch0_pkt_count_txpkt_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch1_ctrl_read;
pcid_write_func_t cmic_cmc0_pktdma_ch1_ctrl_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch1_desc_addr_lo_read;
pcid_write_func_t cmic_cmc0_pktdma_ch1_desc_addr_lo_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch1_desc_addr_hi_read;
pcid_write_func_t cmic_cmc0_pktdma_ch1_desc_addr_hi_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch1_desc_halt_addr_lo_read;
pcid_write_func_t cmic_cmc0_pktdma_ch1_desc_halt_addr_lo_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch1_desc_halt_addr_hi_read;
pcid_write_func_t cmic_cmc0_pktdma_ch1_desc_halt_addr_hi_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch1_stat_read;
pcid_write_func_t cmic_cmc0_pktdma_ch1_stat_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch1_cos_ctrl_rx_0_read;
pcid_write_func_t cmic_cmc0_pktdma_ch1_cos_ctrl_rx_0_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch1_cos_ctrl_rx_1_read;
pcid_write_func_t cmic_cmc0_pktdma_ch1_cos_ctrl_rx_1_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch1_intr_coal_read;
pcid_write_func_t cmic_cmc0_pktdma_ch1_intr_coal_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch1_curr_desc_lo_read;
pcid_write_func_t cmic_cmc0_pktdma_ch1_curr_desc_lo_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch1_curr_desc_hi_read;
pcid_write_func_t cmic_cmc0_pktdma_ch1_curr_desc_hi_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch1_pkt_count_rxpkt_read;
pcid_write_func_t cmic_cmc0_pktdma_ch1_pkt_count_rxpkt_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch1_pkt_count_txpkt_read;
pcid_write_func_t cmic_cmc0_pktdma_ch1_pkt_count_txpkt_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch2_ctrl_read;
pcid_write_func_t cmic_cmc0_pktdma_ch2_ctrl_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch2_desc_addr_lo_read;
pcid_write_func_t cmic_cmc0_pktdma_ch2_desc_addr_lo_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch2_desc_addr_hi_read;
pcid_write_func_t cmic_cmc0_pktdma_ch2_desc_addr_hi_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch2_desc_halt_addr_lo_read;
pcid_write_func_t cmic_cmc0_pktdma_ch2_desc_halt_addr_lo_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch2_desc_halt_addr_hi_read;
pcid_write_func_t cmic_cmc0_pktdma_ch2_desc_halt_addr_hi_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch2_stat_read;
pcid_write_func_t cmic_cmc0_pktdma_ch2_stat_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch2_cos_ctrl_rx_0_read;
pcid_write_func_t cmic_cmc0_pktdma_ch2_cos_ctrl_rx_0_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch2_cos_ctrl_rx_1_read;
pcid_write_func_t cmic_cmc0_pktdma_ch2_cos_ctrl_rx_1_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch2_intr_coal_read;
pcid_write_func_t cmic_cmc0_pktdma_ch2_intr_coal_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch2_curr_desc_lo_read;
pcid_write_func_t cmic_cmc0_pktdma_ch2_curr_desc_lo_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch2_curr_desc_hi_read;
pcid_write_func_t cmic_cmc0_pktdma_ch2_curr_desc_hi_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch2_pkt_count_rxpkt_read;
pcid_write_func_t cmic_cmc0_pktdma_ch2_pkt_count_rxpkt_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch2_pkt_count_txpkt_read;
pcid_write_func_t cmic_cmc0_pktdma_ch2_pkt_count_txpkt_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch3_ctrl_read;
pcid_write_func_t cmic_cmc0_pktdma_ch3_ctrl_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch3_desc_addr_lo_read;
pcid_write_func_t cmic_cmc0_pktdma_ch3_desc_addr_lo_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch3_desc_addr_hi_read;
pcid_write_func_t cmic_cmc0_pktdma_ch3_desc_addr_hi_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch3_desc_halt_addr_lo_read;
pcid_write_func_t cmic_cmc0_pktdma_ch3_desc_halt_addr_lo_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch3_desc_halt_addr_hi_read;
pcid_write_func_t cmic_cmc0_pktdma_ch3_desc_halt_addr_hi_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch3_stat_read;
pcid_write_func_t cmic_cmc0_pktdma_ch3_stat_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch3_cos_ctrl_rx_0_read;
pcid_write_func_t cmic_cmc0_pktdma_ch3_cos_ctrl_rx_0_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch3_cos_ctrl_rx_1_read;
pcid_write_func_t cmic_cmc0_pktdma_ch3_cos_ctrl_rx_1_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch3_intr_coal_read;
pcid_write_func_t cmic_cmc0_pktdma_ch3_intr_coal_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch3_curr_desc_lo_read;
pcid_write_func_t cmic_cmc0_pktdma_ch3_curr_desc_lo_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch3_curr_desc_hi_read;
pcid_write_func_t cmic_cmc0_pktdma_ch3_curr_desc_hi_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch3_pkt_count_rxpkt_read;
pcid_write_func_t cmic_cmc0_pktdma_ch3_pkt_count_rxpkt_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch3_pkt_count_txpkt_read;
pcid_write_func_t cmic_cmc0_pktdma_ch3_pkt_count_txpkt_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch4_ctrl_read;
pcid_write_func_t cmic_cmc0_pktdma_ch4_ctrl_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch4_desc_addr_lo_read;
pcid_write_func_t cmic_cmc0_pktdma_ch4_desc_addr_lo_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch4_desc_addr_hi_read;
pcid_write_func_t cmic_cmc0_pktdma_ch4_desc_addr_hi_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch4_desc_halt_addr_lo_read;
pcid_write_func_t cmic_cmc0_pktdma_ch4_desc_halt_addr_lo_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch4_desc_halt_addr_hi_read;
pcid_write_func_t cmic_cmc0_pktdma_ch4_desc_halt_addr_hi_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch4_stat_read;
pcid_write_func_t cmic_cmc0_pktdma_ch4_stat_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch4_cos_ctrl_rx_0_read;
pcid_write_func_t cmic_cmc0_pktdma_ch4_cos_ctrl_rx_0_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch4_cos_ctrl_rx_1_read;
pcid_write_func_t cmic_cmc0_pktdma_ch4_cos_ctrl_rx_1_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch4_intr_coal_read;
pcid_write_func_t cmic_cmc0_pktdma_ch4_intr_coal_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch4_curr_desc_lo_read;
pcid_write_func_t cmic_cmc0_pktdma_ch4_curr_desc_lo_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch4_curr_desc_hi_read;
pcid_write_func_t cmic_cmc0_pktdma_ch4_curr_desc_hi_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch4_pkt_count_rxpkt_read;
pcid_write_func_t cmic_cmc0_pktdma_ch4_pkt_count_rxpkt_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch4_pkt_count_txpkt_read;
pcid_write_func_t cmic_cmc0_pktdma_ch4_pkt_count_txpkt_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch5_ctrl_read;
pcid_write_func_t cmic_cmc0_pktdma_ch5_ctrl_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch5_desc_addr_lo_read;
pcid_write_func_t cmic_cmc0_pktdma_ch5_desc_addr_lo_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch5_desc_addr_hi_read;
pcid_write_func_t cmic_cmc0_pktdma_ch5_desc_addr_hi_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch5_desc_halt_addr_lo_read;
pcid_write_func_t cmic_cmc0_pktdma_ch5_desc_halt_addr_lo_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch5_desc_halt_addr_hi_read;
pcid_write_func_t cmic_cmc0_pktdma_ch5_desc_halt_addr_hi_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch5_stat_read;
pcid_write_func_t cmic_cmc0_pktdma_ch5_stat_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch5_cos_ctrl_rx_0_read;
pcid_write_func_t cmic_cmc0_pktdma_ch5_cos_ctrl_rx_0_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch5_cos_ctrl_rx_1_read;
pcid_write_func_t cmic_cmc0_pktdma_ch5_cos_ctrl_rx_1_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch5_intr_coal_read;
pcid_write_func_t cmic_cmc0_pktdma_ch5_intr_coal_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch5_curr_desc_lo_read;
pcid_write_func_t cmic_cmc0_pktdma_ch5_curr_desc_lo_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch5_curr_desc_hi_read;
pcid_write_func_t cmic_cmc0_pktdma_ch5_curr_desc_hi_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch5_pkt_count_rxpkt_read;
pcid_write_func_t cmic_cmc0_pktdma_ch5_pkt_count_rxpkt_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch5_pkt_count_txpkt_read;
pcid_write_func_t cmic_cmc0_pktdma_ch5_pkt_count_txpkt_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch6_ctrl_read;
pcid_write_func_t cmic_cmc0_pktdma_ch6_ctrl_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch6_desc_addr_lo_read;
pcid_write_func_t cmic_cmc0_pktdma_ch6_desc_addr_lo_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch6_desc_addr_hi_read;
pcid_write_func_t cmic_cmc0_pktdma_ch6_desc_addr_hi_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch6_desc_halt_addr_lo_read;
pcid_write_func_t cmic_cmc0_pktdma_ch6_desc_halt_addr_lo_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch6_desc_halt_addr_hi_read;
pcid_write_func_t cmic_cmc0_pktdma_ch6_desc_halt_addr_hi_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch6_stat_read;
pcid_write_func_t cmic_cmc0_pktdma_ch6_stat_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch6_cos_ctrl_rx_0_read;
pcid_write_func_t cmic_cmc0_pktdma_ch6_cos_ctrl_rx_0_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch6_cos_ctrl_rx_1_read;
pcid_write_func_t cmic_cmc0_pktdma_ch6_cos_ctrl_rx_1_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch6_intr_coal_read;
pcid_write_func_t cmic_cmc0_pktdma_ch6_intr_coal_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch6_curr_desc_lo_read;
pcid_write_func_t cmic_cmc0_pktdma_ch6_curr_desc_lo_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch6_curr_desc_hi_read;
pcid_write_func_t cmic_cmc0_pktdma_ch6_curr_desc_hi_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch6_pkt_count_rxpkt_read;
pcid_write_func_t cmic_cmc0_pktdma_ch6_pkt_count_rxpkt_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch6_pkt_count_txpkt_read;
pcid_write_func_t cmic_cmc0_pktdma_ch6_pkt_count_txpkt_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch7_ctrl_read;
pcid_write_func_t cmic_cmc0_pktdma_ch7_ctrl_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch7_desc_addr_lo_read;
pcid_write_func_t cmic_cmc0_pktdma_ch7_desc_addr_lo_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch7_desc_addr_hi_read;
pcid_write_func_t cmic_cmc0_pktdma_ch7_desc_addr_hi_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch7_desc_halt_addr_lo_read;
pcid_write_func_t cmic_cmc0_pktdma_ch7_desc_halt_addr_lo_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch7_desc_halt_addr_hi_read;
pcid_write_func_t cmic_cmc0_pktdma_ch7_desc_halt_addr_hi_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch7_stat_read;
pcid_write_func_t cmic_cmc0_pktdma_ch7_stat_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch7_cos_ctrl_rx_0_read;
pcid_write_func_t cmic_cmc0_pktdma_ch7_cos_ctrl_rx_0_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch7_cos_ctrl_rx_1_read;
pcid_write_func_t cmic_cmc0_pktdma_ch7_cos_ctrl_rx_1_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch7_intr_coal_read;
pcid_write_func_t cmic_cmc0_pktdma_ch7_intr_coal_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch7_curr_desc_lo_read;
pcid_write_func_t cmic_cmc0_pktdma_ch7_curr_desc_lo_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch7_curr_desc_hi_read;
pcid_write_func_t cmic_cmc0_pktdma_ch7_curr_desc_hi_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch7_pkt_count_rxpkt_read;
pcid_write_func_t cmic_cmc0_pktdma_ch7_pkt_count_rxpkt_write;
pcid_read_func_t  cmic_cmc0_pktdma_ch7_pkt_count_txpkt_read;
pcid_write_func_t cmic_cmc0_pktdma_ch7_pkt_count_txpkt_write;
pcid_read_func_t  cmic_cmc1_shared_irq_stat0_read;
pcid_write_func_t cmic_cmc1_shared_irq_stat0_write;
pcid_read_func_t  cmic_cmc1_shared_irq_stat_clr0_read;
pcid_write_func_t cmic_cmc1_shared_irq_stat_clr0_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch0_ctrl_read;
pcid_write_func_t cmic_cmc1_pktdma_ch0_ctrl_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch0_desc_addr_lo_read;
pcid_write_func_t cmic_cmc1_pktdma_ch0_desc_addr_lo_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch0_desc_addr_hi_read;
pcid_write_func_t cmic_cmc1_pktdma_ch0_desc_addr_hi_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch0_desc_halt_addr_lo_read;
pcid_write_func_t cmic_cmc1_pktdma_ch0_desc_halt_addr_lo_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch0_desc_halt_addr_hi_read;
pcid_write_func_t cmic_cmc1_pktdma_ch0_desc_halt_addr_hi_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch0_stat_read;
pcid_write_func_t cmic_cmc1_pktdma_ch0_stat_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch0_cos_ctrl_rx_0_read;
pcid_write_func_t cmic_cmc1_pktdma_ch0_cos_ctrl_rx_0_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch0_cos_ctrl_rx_1_read;
pcid_write_func_t cmic_cmc1_pktdma_ch0_cos_ctrl_rx_1_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch0_intr_coal_read;
pcid_write_func_t cmic_cmc1_pktdma_ch0_intr_coal_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch0_curr_desc_lo_read;
pcid_write_func_t cmic_cmc1_pktdma_ch0_curr_desc_lo_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch0_curr_desc_hi_read;
pcid_write_func_t cmic_cmc1_pktdma_ch0_curr_desc_hi_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch0_pkt_count_rxpkt_read;
pcid_write_func_t cmic_cmc1_pktdma_ch0_pkt_count_rxpkt_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch0_pkt_count_txpkt_read;
pcid_write_func_t cmic_cmc1_pktdma_ch0_pkt_count_txpkt_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch1_ctrl_read;
pcid_write_func_t cmic_cmc1_pktdma_ch1_ctrl_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch1_desc_addr_lo_read;
pcid_write_func_t cmic_cmc1_pktdma_ch1_desc_addr_lo_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch1_desc_addr_hi_read;
pcid_write_func_t cmic_cmc1_pktdma_ch1_desc_addr_hi_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch1_desc_halt_addr_lo_read;
pcid_write_func_t cmic_cmc1_pktdma_ch1_desc_halt_addr_lo_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch1_desc_halt_addr_hi_read;
pcid_write_func_t cmic_cmc1_pktdma_ch1_desc_halt_addr_hi_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch1_stat_read;
pcid_write_func_t cmic_cmc1_pktdma_ch1_stat_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch1_cos_ctrl_rx_0_read;
pcid_write_func_t cmic_cmc1_pktdma_ch1_cos_ctrl_rx_0_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch1_cos_ctrl_rx_1_read;
pcid_write_func_t cmic_cmc1_pktdma_ch1_cos_ctrl_rx_1_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch1_intr_coal_read;
pcid_write_func_t cmic_cmc1_pktdma_ch1_intr_coal_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch1_curr_desc_lo_read;
pcid_write_func_t cmic_cmc1_pktdma_ch1_curr_desc_lo_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch1_curr_desc_hi_read;
pcid_write_func_t cmic_cmc1_pktdma_ch1_curr_desc_hi_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch1_pkt_count_rxpkt_read;
pcid_write_func_t cmic_cmc1_pktdma_ch1_pkt_count_rxpkt_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch1_pkt_count_txpkt_read;
pcid_write_func_t cmic_cmc1_pktdma_ch1_pkt_count_txpkt_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch2_ctrl_read;
pcid_write_func_t cmic_cmc1_pktdma_ch2_ctrl_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch2_desc_addr_lo_read;
pcid_write_func_t cmic_cmc1_pktdma_ch2_desc_addr_lo_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch2_desc_addr_hi_read;
pcid_write_func_t cmic_cmc1_pktdma_ch2_desc_addr_hi_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch2_desc_halt_addr_lo_read;
pcid_write_func_t cmic_cmc1_pktdma_ch2_desc_halt_addr_lo_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch2_desc_halt_addr_hi_read;
pcid_write_func_t cmic_cmc1_pktdma_ch2_desc_halt_addr_hi_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch2_stat_read;
pcid_write_func_t cmic_cmc1_pktdma_ch2_stat_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch2_cos_ctrl_rx_0_read;
pcid_write_func_t cmic_cmc1_pktdma_ch2_cos_ctrl_rx_0_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch2_cos_ctrl_rx_1_read;
pcid_write_func_t cmic_cmc1_pktdma_ch2_cos_ctrl_rx_1_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch2_intr_coal_read;
pcid_write_func_t cmic_cmc1_pktdma_ch2_intr_coal_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch2_curr_desc_lo_read;
pcid_write_func_t cmic_cmc1_pktdma_ch2_curr_desc_lo_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch2_curr_desc_hi_read;
pcid_write_func_t cmic_cmc1_pktdma_ch2_curr_desc_hi_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch2_pkt_count_rxpkt_read;
pcid_write_func_t cmic_cmc1_pktdma_ch2_pkt_count_rxpkt_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch2_pkt_count_txpkt_read;
pcid_write_func_t cmic_cmc1_pktdma_ch2_pkt_count_txpkt_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch3_ctrl_read;
pcid_write_func_t cmic_cmc1_pktdma_ch3_ctrl_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch3_desc_addr_lo_read;
pcid_write_func_t cmic_cmc1_pktdma_ch3_desc_addr_lo_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch3_desc_addr_hi_read;
pcid_write_func_t cmic_cmc1_pktdma_ch3_desc_addr_hi_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch3_desc_halt_addr_lo_read;
pcid_write_func_t cmic_cmc1_pktdma_ch3_desc_halt_addr_lo_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch3_desc_halt_addr_hi_read;
pcid_write_func_t cmic_cmc1_pktdma_ch3_desc_halt_addr_hi_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch3_stat_read;
pcid_write_func_t cmic_cmc1_pktdma_ch3_stat_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch3_cos_ctrl_rx_0_read;
pcid_write_func_t cmic_cmc1_pktdma_ch3_cos_ctrl_rx_0_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch3_cos_ctrl_rx_1_read;
pcid_write_func_t cmic_cmc1_pktdma_ch3_cos_ctrl_rx_1_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch3_intr_coal_read;
pcid_write_func_t cmic_cmc1_pktdma_ch3_intr_coal_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch3_curr_desc_lo_read;
pcid_write_func_t cmic_cmc1_pktdma_ch3_curr_desc_lo_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch3_curr_desc_hi_read;
pcid_write_func_t cmic_cmc1_pktdma_ch3_curr_desc_hi_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch3_pkt_count_rxpkt_read;
pcid_write_func_t cmic_cmc1_pktdma_ch3_pkt_count_rxpkt_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch3_pkt_count_txpkt_read;
pcid_write_func_t cmic_cmc1_pktdma_ch3_pkt_count_txpkt_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch4_ctrl_read;
pcid_write_func_t cmic_cmc1_pktdma_ch4_ctrl_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch4_desc_addr_lo_read;
pcid_write_func_t cmic_cmc1_pktdma_ch4_desc_addr_lo_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch4_desc_addr_hi_read;
pcid_write_func_t cmic_cmc1_pktdma_ch4_desc_addr_hi_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch4_desc_halt_addr_lo_read;
pcid_write_func_t cmic_cmc1_pktdma_ch4_desc_halt_addr_lo_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch4_desc_halt_addr_hi_read;
pcid_write_func_t cmic_cmc1_pktdma_ch4_desc_halt_addr_hi_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch4_stat_read;
pcid_write_func_t cmic_cmc1_pktdma_ch4_stat_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch4_cos_ctrl_rx_0_read;
pcid_write_func_t cmic_cmc1_pktdma_ch4_cos_ctrl_rx_0_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch4_cos_ctrl_rx_1_read;
pcid_write_func_t cmic_cmc1_pktdma_ch4_cos_ctrl_rx_1_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch4_intr_coal_read;
pcid_write_func_t cmic_cmc1_pktdma_ch4_intr_coal_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch4_curr_desc_lo_read;
pcid_write_func_t cmic_cmc1_pktdma_ch4_curr_desc_lo_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch4_curr_desc_hi_read;
pcid_write_func_t cmic_cmc1_pktdma_ch4_curr_desc_hi_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch4_pkt_count_rxpkt_read;
pcid_write_func_t cmic_cmc1_pktdma_ch4_pkt_count_rxpkt_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch4_pkt_count_txpkt_read;
pcid_write_func_t cmic_cmc1_pktdma_ch4_pkt_count_txpkt_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch5_ctrl_read;
pcid_write_func_t cmic_cmc1_pktdma_ch5_ctrl_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch5_desc_addr_lo_read;
pcid_write_func_t cmic_cmc1_pktdma_ch5_desc_addr_lo_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch5_desc_addr_hi_read;
pcid_write_func_t cmic_cmc1_pktdma_ch5_desc_addr_hi_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch5_desc_halt_addr_lo_read;
pcid_write_func_t cmic_cmc1_pktdma_ch5_desc_halt_addr_lo_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch5_desc_halt_addr_hi_read;
pcid_write_func_t cmic_cmc1_pktdma_ch5_desc_halt_addr_hi_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch5_stat_read;
pcid_write_func_t cmic_cmc1_pktdma_ch5_stat_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch5_cos_ctrl_rx_0_read;
pcid_write_func_t cmic_cmc1_pktdma_ch5_cos_ctrl_rx_0_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch5_cos_ctrl_rx_1_read;
pcid_write_func_t cmic_cmc1_pktdma_ch5_cos_ctrl_rx_1_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch5_intr_coal_read;
pcid_write_func_t cmic_cmc1_pktdma_ch5_intr_coal_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch5_curr_desc_lo_read;
pcid_write_func_t cmic_cmc1_pktdma_ch5_curr_desc_lo_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch5_curr_desc_hi_read;
pcid_write_func_t cmic_cmc1_pktdma_ch5_curr_desc_hi_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch5_pkt_count_rxpkt_read;
pcid_write_func_t cmic_cmc1_pktdma_ch5_pkt_count_rxpkt_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch5_pkt_count_txpkt_read;
pcid_write_func_t cmic_cmc1_pktdma_ch5_pkt_count_txpkt_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch6_ctrl_read;
pcid_write_func_t cmic_cmc1_pktdma_ch6_ctrl_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch6_desc_addr_lo_read;
pcid_write_func_t cmic_cmc1_pktdma_ch6_desc_addr_lo_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch6_desc_addr_hi_read;
pcid_write_func_t cmic_cmc1_pktdma_ch6_desc_addr_hi_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch6_desc_halt_addr_lo_read;
pcid_write_func_t cmic_cmc1_pktdma_ch6_desc_halt_addr_lo_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch6_desc_halt_addr_hi_read;
pcid_write_func_t cmic_cmc1_pktdma_ch6_desc_halt_addr_hi_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch6_stat_read;
pcid_write_func_t cmic_cmc1_pktdma_ch6_stat_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch6_cos_ctrl_rx_0_read;
pcid_write_func_t cmic_cmc1_pktdma_ch6_cos_ctrl_rx_0_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch6_cos_ctrl_rx_1_read;
pcid_write_func_t cmic_cmc1_pktdma_ch6_cos_ctrl_rx_1_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch6_intr_coal_read;
pcid_write_func_t cmic_cmc1_pktdma_ch6_intr_coal_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch6_curr_desc_lo_read;
pcid_write_func_t cmic_cmc1_pktdma_ch6_curr_desc_lo_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch6_curr_desc_hi_read;
pcid_write_func_t cmic_cmc1_pktdma_ch6_curr_desc_hi_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch6_pkt_count_rxpkt_read;
pcid_write_func_t cmic_cmc1_pktdma_ch6_pkt_count_rxpkt_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch6_pkt_count_txpkt_read;
pcid_write_func_t cmic_cmc1_pktdma_ch6_pkt_count_txpkt_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch7_ctrl_read;
pcid_write_func_t cmic_cmc1_pktdma_ch7_ctrl_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch7_desc_addr_lo_read;
pcid_write_func_t cmic_cmc1_pktdma_ch7_desc_addr_lo_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch7_desc_addr_hi_read;
pcid_write_func_t cmic_cmc1_pktdma_ch7_desc_addr_hi_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch7_desc_halt_addr_lo_read;
pcid_write_func_t cmic_cmc1_pktdma_ch7_desc_halt_addr_lo_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch7_desc_halt_addr_hi_read;
pcid_write_func_t cmic_cmc1_pktdma_ch7_desc_halt_addr_hi_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch7_stat_read;
pcid_write_func_t cmic_cmc1_pktdma_ch7_stat_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch7_cos_ctrl_rx_0_read;
pcid_write_func_t cmic_cmc1_pktdma_ch7_cos_ctrl_rx_0_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch7_cos_ctrl_rx_1_read;
pcid_write_func_t cmic_cmc1_pktdma_ch7_cos_ctrl_rx_1_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch7_intr_coal_read;
pcid_write_func_t cmic_cmc1_pktdma_ch7_intr_coal_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch7_curr_desc_lo_read;
pcid_write_func_t cmic_cmc1_pktdma_ch7_curr_desc_lo_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch7_curr_desc_hi_read;
pcid_write_func_t cmic_cmc1_pktdma_ch7_curr_desc_hi_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch7_pkt_count_rxpkt_read;
pcid_write_func_t cmic_cmc1_pktdma_ch7_pkt_count_rxpkt_write;
pcid_read_func_t  cmic_cmc1_pktdma_ch7_pkt_count_txpkt_read;
pcid_write_func_t cmic_cmc1_pktdma_ch7_pkt_count_txpkt_write;

#endif   /* _CMICX_PACKET_DMA_H_ */


