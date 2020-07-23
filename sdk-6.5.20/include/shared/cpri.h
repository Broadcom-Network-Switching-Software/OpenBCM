/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file defines commo cpri port  parameters.
 *
 * Its contents are not used directly by applications; it is used only
 * by header files of parent APIs which need to define cpri  parameters.
 */

#ifndef _SHR_CPRI_H
#define _SHR_CPRI_H

/* Control Z.130.0  Basic Layer 1 function bit map. */ 
typedef enum _shr_cpri_l1_function_e { 
    _SHR_L1_FUNCTION_RESET       = 0x01,  /**< Reset Reqeust/Acknowledge */
    _SHR_L1_FUNCTION_RAI         = 0x02,   /**< Remote Alarm Indication */
    _SHR_L1_FUNCTION_SDI         = 0x04,   /**< SAP Defect Indication */
    _SHR_L1_FUNCTION_LOS         = 0x08,   /**< Loss of Signal */
    _SHR_L1_FUNCTION_LOF         = 0x10   /**< Loss of Frame */
} _shr_cpri_l1_function_t;

#define _SHR_L1_FUNCTION_RESET_SET(l1_func) (l1_func |= _SHR_L1_FUNCTION_RESET)
#define _SHR_L1_FUNCTION_RAI_SET(l1_func) (l1_func |= _SHR_L1_FUNCTION_RAI)
#define _SHR_L1_FUNCTION_SDI_SET(l1_func) (l1_func |= _SHR_L1_FUNCTION_SDI)
#define _SHR_L1_FUNCTION_LOS_SET(l1_func) (l1_func |= _SHR_L1_FUNCTION_LOS)
#define _SHR_L1_FUNCTION_LOF_SET(l1_func) (l1_func |= _SHR_L1_FUNCTION_LOF)

#define _SHR_L1_FUNCTION_RESET_CLR(l1_func) (l1_func &= ~_SHR_L1_FUNCTION_RESET)
#define _SHR_L1_FUNCTION_RAI_CLR(l1_func) (l1_func &= ~_SHR_L1_FUNCTION_RAI)
#define _SHR_L1_FUNCTION_SDI_CLR(l1_func) (l1_func &= ~_SHR_L1_FUNCTION_SDI)
#define _SHR_L1_FUNCTION_LOS_CLR(l1_func) (l1_func &= ~_SHR_L1_FUNCTION_LOS)
#define _SHR_L1_FUNCTION_LOF_CLR(l1_func) (l1_func &= ~_SHR_L1_FUNCTION_LOF)

#define _SHR_L1_FUNCTION_RESET_GET(l1_func) (l1_func & _SHR_L1_FUNCTION_RESET ? 1 : 0)
#define _SHR_L1_FUNCTION_RAI_GET(l1_func) (l1_func & _SHR_L1_FUNCTION_RAI ? 1 : 0)
#define _SHR_L1_FUNCTION_SDI_GET(l1_func) (l1_func & _SHR_L1_FUNCTION_SDI ? 1 : 0)
#define _SHR_L1_FUNCTION_LOS_GET(l1_func) (l1_func & _SHR_L1_FUNCTION_LOS ? 1 : 0)
#define _SHR_L1_FUNCTION_LOF_GET(l1_func) (l1_func & _SHR_L1_FUNCTION_LOF ? 1 : 0)

/* Control Z.66.0  HDLC rate. */ 
typedef enum _shr_cpri_hdlc_rate_e { 
    _SHR_CPRI_HDLC_RATE_NO_HDLC       = 0x00,  /**< NO HDLC */
    _SHR_CPRI_HDLC_RATE_240KBPS       = 0x01,  /**< 240 Kbps */
    _SHR_CPRI_HDLC_RATE_480KBPS       = 0x02,  /**< 240 Kbps */
    _SHR_CPRI_HDLC_RATE_960KBPS       = 0x03,  /**< 240 Kbps */
    _SHR_CPRI_HDLC_RATE_1920KBPS      = 0x04,  /**< 240 Kbps */
    _SHR_CPRI_HDLC_RATE_2400KBPS      = 0x05,  /**< 240 Kbps */
    _SHR_CPRI_HDLC_RATE_HIGHEST       = 0x06,  /**< Highest Possible Rate */
    _SHR_CPRI_HDLC_RATE_NEGOTIATED    = 0x07  /**< Rate Negotiated on higher Layer */
} _shr_cpri_hdlc_rate_t;

typedef struct _shr_cpri_cw_l1_inband_info_s{
    uint32 eth_ptr;        /* Z.194.0 Ethernet pointer p. */
    uint32 layer1_function; /* Z.130.0 l1 function */
    uint32 hdlc_rate;       /* Z.66.0 HDLC rate */
    uint32 protocol_ver;    /* Z.2.0 version 1 or 2 */
 } _shr_cpri_cw_l1_inband_info_t;

typedef enum _shr_cpri_l1_signal_prot_map_e {
    _SHR_L1_SIGNAL_PROT_MAP_LOF         = 0x1,   /**< Loss of Frame */
    _SHR_L1_SIGNAL_PROT_MAP_LOS         = 0x2,   /**< Loss of Signal */
    _SHR_L1_SIGNAL_PROT_MAP_SDI         = 0x4,   /**< SAP Defect Indication */
    _SHR_L1_SIGNAL_PROT_MAP_RAI         = 0x8,   /**< Remote Alarm Indication */
    _SHR_L1_SIGNAL_PROT_MAP_RESET       = 0x10,  /**< Reset Reqeust/Acknowledge */
    _SHR_L1_SIGNAL_PROT_MAP_ETH_PTR     = 0x20,  /**< Fast Ethernet Pointer */
    _SHR_L1_SIGNAL_PROT_MAP_HDLC_RATE   = 0x40,  /**< HDLC rate */
    _SHR_L1_SIGNAL_PROT_MAP_VER         = 0x80  /**< Protection Version */
} _shr_cpri_l1_signal_prot_map_t;

#define _SHR_L1_SIGNAL_PROT_MAP_LOF_SET(flags) (flags |= _SHR_L1_SIGNAL_PROT_MAP_LOF)
#define _SHR_L1_SIGNAL_PROT_MAP_LOS_SET(flags) (flags |= _SHR_L1_SIGNAL_PROT_MAP_LOS)
#define _SHR_L1_SIGNAL_PROT_MAP_SDI_SET(flags) (flags |= _SHR_L1_SIGNAL_PROT_MAP_SDI)
#define _SHR_L1_SIGNAL_PROT_MAP_RAI_SET(flags) (flags |= _SHR_L1_SIGNAL_PROT_MAP_RAI)
#define _SHR_L1_SIGNAL_PROT_MAP_RESET_SET(flags) (flags |= _SHR_L1_SIGNAL_PROT_MAP_RESET)
#define _SHR_L1_SIGNAL_PROT_MAP_ETH_PTR_SET(flags) (flags |= _SHR_L1_SIGNAL_PROT_MAP_ETH_PTR)
#define _SHR_L1_SIGNAL_PROT_MAP_HDLC_RATE_SET(flags) (flags |= _SHR_L1_SIGNAL_PROT_MAP_HDLC_RATE)
#define _SHR_L1_SIGNAL_PROT_MAP_VER_SET(flags) (flags |= _SHR_L1_SIGNAL_PROT_MAP_VER)

#define _SHR_L1_SIGNAL_PROT_MAP_LOF_CLR(flags) (flags &= ~_SHR_L1_SIGNAL_PROT_MAP_LOF)
#define _SHR_L1_SIGNAL_PROT_MAP_LOS_CLR(flags) (flags &= ~_SHR_L1_SIGNAL_PROT_MAP_LOS)
#define _SHR_L1_SIGNAL_PROT_MAP_SDI_CLR(flags) (flags &= ~_SHR_L1_SIGNAL_PROT_MAP_SDI)
#define _SHR_L1_SIGNAL_PROT_MAP_RAI_CLR(flags) (flags &= ~_SHR_L1_SIGNAL_PROT_MAP_RAI)
#define _SHR_L1_SIGNAL_PROT_MAP_RESET_CLR(flags) (flags &= ~_SHR_L1_SIGNAL_PROT_MAP_RESET)
#define _SHR_L1_SIGNAL_PROT_MAP_ETH_PTR_CLR(flags) (flags &= ~_SHR_L1_SIGNAL_PROT_MAP_ETH_PTR)
#define _SHR_L1_SIGNAL_PROT_MAP_HDLC_RATE_CLR(flags) (flags &= ~_SHR_L1_SIGNAL_PROT_MAP_HDLC_RATE)
#define _SHR_L1_SIGNAL_PROT_MAP_VER_CLR(flags) (flags &= ~_SHR_L1_SIGNAL_PROT_MAP_VER)

#define _SHR_L1_SIGNAL_PROT_MAP_LOF_GET(flags) (flags & _SHR_L1_SIGNAL_PROT_MAP_LOF ? 1 : 0)
#define _SHR_L1_SIGNAL_PROT_MAP_LOS_GET(flags) (flags & _SHR_L1_SIGNAL_PROT_MAP_LOS ? 1 : 0)
#define _SHR_L1_SIGNAL_PROT_MAP_SDI_GET(flags) (flags & _SHR_L1_SIGNAL_PROT_MAP_SDI ? 1 : 0)
#define _SHR_L1_SIGNAL_PROT_MAP_RAI_GET(flags) (flags & _SHR_L1_SIGNAL_PROT_MAP_RAI ? 1 : 0)
#define _SHR_L1_SIGNAL_PROT_MAP_RESET_GET(flags) (flags & _SHR_L1_SIGNAL_PROT_MAP_RESET ? 1 : 0)
#define _SHR_L1_SIGNAL_PROT_MAP_ETH_PTR_GET(flags) (flags & _SHR_L1_SIGNAL_PROT_MAP_ETH_PTR ? 1 : 0)
#define _SHR_L1_SIGNAL_PROT_MAP_HDLC_RATE_GET(flags) (flags & _SHR_L1_SIGNAL_PROT_MAP_HDLC_RATE ? 1 : 0)
#define _SHR_L1_SIGNAL_PROT_MAP_VER_GET(flags) (flags & _SHR_L1_SIGNAL_PROT_MAP_VER ? 1 : 0)



typedef enum _shr_cpri_hdlc_crc_byte_order_e {
    _shrCpriHdlcCrcByteOrderNone = 0, /**< Default 15 .. 0  */
    _shrCpriHdlcCrcByteOrderSwap = 1, /**<  Byte Order Swap [7:0][15:8] */
    _shrCpriHdlcCrcByteOrderCount
} _shr_cpri_hdlc_crc_byte_order_t;

typedef enum _shr_cpri_hdlc_fcs_err_check_e {
    _shrCpriHdlcFcsErrCheck = 0, /**<  Check for FCS Error  */
    _shrCpriHdlcFcsErrNoCheck = 1, /**<  No FCS Error Check.  Ignore FCS. */
    _shrCpriHdlcFcsErrCount
} _shr_cpri_hdlc_fcs_err_check_t;

typedef enum _shr_cpri_hdlc_fcs_size_e {
    _shrCpriHdlcFcsSize8Bits = 0, /**< FCS Size 8 bits  */
    _shrCpriHdlcFcsSize16Bits = 1, /**< FCS Size 16 bits  */
    _shrCpriHdlcFcsSize32Bits = 2, /**< FCS Size 32 bits  */
    _shrCpriHdlcFcsSizeCount
} _shr_cpri_hdlc_fcs_size_t;

typedef enum _shr_cpri_hdlc_filling_flag_pattern_e {
    _shrCpriHdlcFillingFlagPattern7E = 0, /**< Fill 7E  */
    _shrCpriHdlcFillingFlagPattern7F = 1, /**< Fill 7F  */
    _shrCpriHdlcFillingFlagPatternFF = 2, /**< Fill FF  */
    _shrCpriHdlcFillingFlagPatternCount
} _shr_cpri_hdlc_filling_flag_pattern_t;

typedef enum _shr_cpri_hdlc_flag_size_e {
    _shrCpriHdlcNumFlagSizeOneByte = 0, /**< Min 1 Flag Byte  */
    _shrCpriHdlcNumFlagSizeTwoByte = 1, /**< Min 2 Flag Bytes  */
    _shrCpriHdlcNumFlagByteCount
} _shr_cpri_hdlc_flag_size_t;

typedef enum _shr_cpri_hdlc_crc_mode_e {
    _shrCpriHdlcCrcAppend = 0, /**< Append CRC  */
    _shrCpriHdlcCrcReplace = 1, /**< Replace CRC  */
    _shrCpriHdlcCrcNoUpdate = 2, /**< Keep existing CRC.  */
    _shrCpriHdlcCrcCount
} _shr_cpri_hdlc_crc_mode_t;

typedef enum _shr_cpri_hdlc_crc_init_val_e {
    _shrCpriHdlcCrcInitValAll0 = 0, /**< ALL 0s  */
    _shrCpriHdlcCrcInitValAll1 = 1, /**< All 1s  */
    _shrCpriHdlcCrcInitValCount
} _shr_cpri_hdlc_crc_init_val_t;

typedef enum _shr_cpri_vsd_control_subchan_num_bytes_e {
    _shrVsdCtrlSubchanNumBytes1 = 0, /**< Sub Channel Num of Bytes 1. */
    _shrVsdCtrlSubchanNumBytes2 = 1, /**< Sub Channel Num of Bytes 2. */
    _shrVsdCtrlSubchanNumBytes4 = 2, /**< Sub Channel Num of Bytes 4.  */
    _shrVsdCtrlSubchanNumBytesCount
} _shr_cpri_vsd_control_subchan_num_bytes_t;

typedef enum _shr_cpri_vsd_control_flow_num_bytes_e {
    _shrVsdCtrlFlowNumBytes2 = 0, /**< VSD control flow num of Bytes 2. */
    _shrVsdCtrlFlowNumBytes4 = 1, /**< VSD control flow num of Bytes 4. */
    _shrVsdCtrlFlowNumBytesCount
} _shr_cpri_vsd_control_flow_num_bytes_t;

typedef enum _shr_cpri_vsd_control_subchan_size_e {
    _shrVsdCtrlSubchanSize2 = 0, /**< VSD control Sub Chan Size 2 bytes. */
    _shrVsdCtrlSubchanSize4 = 1, /**< VSD control Sub Chan Size 4 bytes. */
    _shrVsdCtrlSubchanSizeCount
} _shr_cpri_vsd_control_subchan_size_t;

typedef struct _shr_cpri_rx_vsd_flow_info_s{
    uint32 hyper_frame_number; /**< Hyper Frame Number */
    uint32 hyper_frame_modulo; /**< Hyper Frame Modulo */
    int filter_zero_data; /**< Filer all 0's Data.  */
    uint32 section_num[4]; /**< VSD Section in this group in Order 0-127.  */
    uint32 num_sector; /**< Number of Sectors. */
    uint32 tag_id; /**< Tag Id */
} _shr_cpri_rx_vsd_flow_info_t;

typedef struct _shr_cpri_tx_vsd_flow_info_s {
    uint32 hyper_frame_number; /**< Hyper Frame Number */
    uint32 hyper_frame_modulo; /**< Hyper Frame Number */
    int repeat_mode; /**< Filer all 0's Data.  */
    uint32 section_num[4]; /**< VSD Sector in this group in Order.  */
    uint32 num_sector; /**< Number of Sectors. */
    uint32 roe_flow_id; /**< Roe Flow ID. */
} _shr_cpri_tx_vsd_flow_info_t;

typedef enum _shr_cpri_cw_filter_mode_e {
    _shrCpriFilterDisable = 0, /**< Disable Filter */
    _shrCpriFilterNonZero = 1, /**< Non-Zero Data only */
    _shrCpriFilterPeriodic = 2, /**< Periodic Generation */
    _shrCpriFilterChange = 3, /**< Content Change Only */
    _shrCpriFilterPatternMatch = 4, /**<  Pattern Match */
    _shrCpriFilterCount
} _shr_cpri_cw_filter_mode_t;

typedef enum _shr_vsd_raw_map_mode_e {
    _shrVsdRawMapModePeriodic = 0, /**< Peridic insert data using Modulo */
    _shrVsdRawMapModeRoeFrame = 1, /**< Use Frame Number from ROE frame. */
    _shrVsdRawMapModeCount
} _shr_vsd_raw_map_mode_t;

typedef enum _shr_cpri_gcw_mask_e {
    _shrCpriGcwMaskNone = 0, /**< None of the Byte are extracted. */
    _shrCpriGcwMaskLSB = 1, /**< LSB Byte is extracted. */
    _shrCpriGcwMaskMSB = 2, /**< MSB Byte is extracted. */
    _shrCpriGcwMaskBOTH = 3, /**< Both Byte are extracted. */
    _shrCpriGcwMaskCount
} _shr_cpri_gcw_mask_t;

/* BRCM_RSVD4 */
typedef enum _shr_rsvd4_control_msg_proc_type_e {
    _shrRsvd4CtrlMsgProcFE = 0, /**< Multi Message Fast Ethernet. */
    _shrRsvd4CtrlMsgProcFCB = 1, /**< FCB . */
    _shrRsvd4CtrlMsgProcPayload = 2, /**< Message Payload Extraction. */
    _shrRsvd4CtrlMsgProcFull = 3, /**< Full Message. */
    _shrRsvd4CtrlMsgProcFullWithTag = 4, /**< Full Message with Tag. */
    _shrRsvd4CtrlMsgProcCount
} _shr_rsvd4_control_msg_proc_type_t;

typedef enum _shr_cpri_tag_option_e {
    _shrCpriTagUseDefault= 0, /**< Tag Use default. */
    _shrCpriTagUseLookup = 1, /**< Tag Use Look up table. */
    _shrCpriTagOptionCount
} _shr_cpri_tag_option_t;

typedef enum _shr_cpri_interrupt_type_e{
    _shrCpriIntrRx1588CapturedTs, /* RX 1588 Fifo intr */
    _shrCpriIntrRx1588TsFifo, /* RX 1588 Fifo intr - Fifo full,  */
    _shrCpriIntrRxEncapCtrlQOverflow, /* Encap Ctrl Queue(0-15) FIFO Over-Flow */
    _shrCpriIntrRxEncapDataQOverflow, /* Encap Data Queue(0-63) FIFO Over-Flow */
    _shrCpriIntrRxEncapGsmTsQErr, /* Encap Per Queue GSM(0-63) Time-Slot Error */ 
    _shrCpriIntrRxEncapIpsmCdcFifoErr, /* Encap IPSM CDC FIFO Error */
    _shrCpriIntrRxEncapRxFrmInFifoOverflow, /* Encap RX-framer Input-buffer FIFO Over Error */
    _shrCpriIntrRxEncapWqFiFoOverflow, /* Encap Per Work-Queue FIFO Over Error */
    _shrCpriIntrRxFrmL1SigChange, /* L1 signal change */
    _shrCpriIntrRxFrmRsvd4SingleTsErr, /*Per Axc(0-63) Single Timestamp Error in RSVD4 mode */
    _shrCpriIntrRxFrmRsvd4Ts, /* Per Axc(0-63) Timestamp interrupt in RSVD4 mode */
    _shrCpriIntrRxGcw, /* Interrupt for RX GCW Word(0-15) is loaded */
    _shrCpriIntrRxPcs64B66BBlkLckLh, /* RX PCS Block lock status - latch high version */
    _shrCpriIntrRxPcs64B66BBlkLckLl, /* RX PCS Block lock status - latch low version */ 
    _shrCpriIntrRxPcs64B66BHiBerLh, /* RX PCS BER - latch high version */
    _shrCpriIntrRxPcs64B66BHiBerLl, /* RX PCS BER - latch low version */
    _shrCpriIntrRxPcsExtractFifoOvrFlow, /* RX PCS extract FIFO overflow intr */
    _shrCpriIntrRxPcsFecCsCwBad, /* cw_bad during CW_SYNC state */
    _shrCpriIntrRxPcsLinkStatusLh, /* RX PCS link status - latch high version */
    _shrCpriIntrRxPcsLinkStatusLl, /* RX PCS link status - latch low version */
    _shrCpriIntrRxPcsLosLh, /* RX PCS LOS - latch high version */
    _shrCpriIntrRxPcsLosLl, /* RX PCS LOS - latch low version */
    _shrCpriIntrTx1588CapturedTs, /* TX 1588 Sw capture interrupt */
    _shrCpriIntrTx1588TsFifo, /* "TX 1588 Fifo capture interrupt. */
    _shrCpriIntrTxDecapAgModePktMiss, /* Agnostic mode missing packet */
    _shrCpriIntrTxDecapDataQBufSizeErr,/* Decap Per Queue Decap Data Queue BUFF SIZE Error */
    _shrCpriIntrTxDecapDataQOvflErr,/* Decap Per Queue Decap Data Queue FIFO Over-Flow Error */
    _shrCpriIntrTxDecapDataQUdflErr,/* Decap Per Queue Decap Data Queue FIFO underflow Error */
    _shrCpriIntrTxFrmArbCpriAckMismatch, /* arbiter cpri ack mismatch */
    _shrCpriIntrTxFrmArbPsizeZero, /* decap sop control word packet size = 0 */
    _shrCpriIntrTxFrmArbRsvd4AckMisalign, /* arbiter RSVD4 ack misaligned */
    _shrCpriIntrTxFrmArbRsvd4ReqConflict, /* arbiter RSVD4 req conflict */ 
    _shrCpriIntrTxFrmBfaHfStartMisalign, /* BFA hf start misalign */
    _shrCpriIntrTxFrmCwaCtrlPktFlowIdErr, /* CWA ctrl flow packet flow id error */
    _shrCpriIntrTxFrmCwaFastEthBufferOverflow, /* CPRI control asm fast ethernet (fast cm) buffer overflow */
    _shrCpriIntrTxFrmCwaHdlcBufferOverflow, /* CPRI control asm hdlc (slow cm) buffer overflow */
    _shrCpriIntrTxFrmCwaMultipleValids, /*  CPRI control asm valids from multiple engines are asserted at same time */
    _shrCpriIntrTxFrmCwaSdvmBufferOverflow, /* CPRI control asm Rsvd5 buffer overflow */
    _shrCpriIntrTxFrmCwaSdvmProcOverflow, /* CPRI control asm RSVD5 proc buffer overflow */
    _shrCpriIntrTxFrmCwaVsdRawBufferOverflow, /* CPRI control asm vsd raw buffer overflow */
    _shrCpriIntrTxFrmDrAckMisalign,/* RSVD4 control proc decap req - decap ack and delayed req misalign */
    _shrCpriIntrTxFrmDrMultiEthPsizeUndersize, /* RSVD4 control proc decap req multi eth - packet undersize */
    _shrCpriIntrTxFrmMultiEthBufferOverflow, /* RSVD4 control multi-msg eth buffer overflow */
    _shrCpriIntrTxFrmMultiEthBufferUnderflow, /* RSVD4 control multi-msg eth buffer underflow */
    _shrCpriIntrTxGcw, /* Interrupt for TX GCW Word(0-15) is loaded */
    _shrCpriIntrTxPcsFifoOverflow, /* TX assembly FIFO overflow */
    _shrCpriIntrTxPcsFifoUnderflow, /* TX assembly FIFO underflow */
    _shrCpriIntrTxPcsGboxOverflow,/* gearbox buffer overflow */
    _shrCpriIntrTxPcsGboxUnderflow, /* gearbox buffer underflow */
    _shrCpriIntrCount /* Should be last */
} _shr_cpri_interrupt_type_t;

typedef struct _shr_cpri_fast_eth_config_s {
  uint32 queue_num;
  int no_fcs_err_check;
  uint32 min_packet_size;
  uint32 max_packet_size;
  int  min_packet_drop;
  int  max_packet_drop;
  int    strip_crc;
} _shr_cpri_fast_eth_config_t;

typedef enum _shr_rsvd4_sync_count_cycle_e {
    _shrRsvd4SyncCountCycleEveryMF = 0, /**< Every Frame. */
    _shrRsvd4SyncCountCycleUpToSixMF = 1, /**< Up to 6 MF . */
    _shrRsvd4SyncCountCycleCount
} _shr_rsvd4_sync_count_cycle_t;

typedef struct _shr_cpri_tag_gen_entry_s {
    uint32 tag_id; /**< Tag ID */
    uint32 mask; /**< Mask */
    uint32 header; /**< Header Info */
    uint32 word_count; /**< Word Count, only for RTWP */
} _shr_cpri_tag_gen_entry_t;

typedef enum _shr_rsvd4_crc_option_e {
    _shrRsvd4CrcOptionUseRoe = 0, /**< Use CRC from ROE packet. */
    _shrRsvd4CrcOptionRegenerate = 1, /**< Regenerate New One . */
    _shrRsvd4CrcOptionCount
} _shr_rsvd4_crc_option_t;

/* RSVD4 TS MSG modes */
typedef enum _shr_rsvd4_msg_ts_mode_e {
    _shrRsvd4MsgTsModeWCDMA = 0, /*  */
    _shrRsvd4MsgTsModeLTE = 1,   /*  */
    _shrRsvd4MsgTsModeGSMDL = 2, /*  */
    _shrRsvd4MsgTsModeGSMUL = 3,  /*  */
    _shrRsvd4MsgTsModeCount = 4  /*  */
} _shr_rsvd4_msg_ts_mode_t;


typedef enum _shr_cpri_1588_capture_mode_e {
    _shrCpri1588CaptureModeMatchBfnHfn = 0,
    _shrCpri1588CaptureModeMatchHfnOnly = 1,
    _shrCpri1588CaptureModeCount
} _shr_cpri_1588_capture_mode_t;

typedef enum _shr_cpri_frame_sync_mode_e {
    _shrCpriFrameSyncModeHyper,
    _shrCpriFrameSyncModeRadio,
    _shrCpriFrameSyncModeBasic,
    _shrCpriFrameSyncModeTunnel,   /* Frame sync mode for tunnel */
    _shrCpriFrameSyncModeCount
} _shr_cpri_frame_sync_mode_t;

typedef enum _shr_rsvd4_sync_mode_e {
    _shrRsvd4SyncModeNoSync,
    _shrRsvd4SyncModeMsgOffset,  /* experimentali, internal use only */
    _shrRsvd4SyncModeMsgAndMasterFrameOffset,
    _shrRsvd4SyncModeTsNoSync,
    _shrRsvd4SyncModeTsMsgOffset,  /* experimental, internal use only */
    _shrRsvd4SyncModeTsMsgAndMasterFrameOffset,
    _shrRsvd4SyncModeCount
} _shr_rsvd4_sync_mode_t;

typedef enum _shr_cpri_roe_payload_multiple_e { 
    _shrCpriPayloadMultiple1X,
    _shrCpriPayloadMultiple1p5X,
    _shrCpriPayloadMultiple2X,
    _shrCpriPayloadMultiple4X,
    _shrCpriPayloadMultiple8X,
    _shrCpriPayloadMultipleCount
} _shr_cpri_roe_payload_multiple_t;

typedef enum _shr_cpri_dir_e { 
    _shrCpriDirTx,
    _shrCpriDirRx,
    _shrCpriDirCount
} _shr_cpri_dir_t;

typedef struct _shr_cpri_presentation_time_s {
    uint32 time; /* 29 bits only, sign bit | 24 bits ns | 4 bits sub ns */
}_shr_cpri_presentation_time_t;

typedef enum _shr_cpri_presentation_ts_attribute_e {
    _shrCpriEncapPresentationTsAdjust,
    _shrCpriEncapPresentationApproxInc,
    _shrCpriEncapPresentationPtpTsAdjust
}_shr_cpri_presentation_ts_attribute_t;

typedef struct _shr_cpri_decap_presentation_config_s {
    uint32 time_mod_offset; /* 20 bits only */
    uint32 mod_count;
}_shr_cpri_decap_presentation_config_t;

typedef enum _shr_cpri_1588_ts_type_e {
    _shrCpri1588TsTypeFifo,
    _shrCpri1588TsTypeCMIC,
    _shrCpri1588TsTypeSw,
    _shrCpri1588TsTypeCount
} _shr_cpri_1588_ts_type_t;

typedef struct _shr_cpri_1588_timestamp_ts_config_s {
    int enable; /* Enable/Disable */
    uint32 modulo_count;  /* modulo count */
}_shr_cpri_1588_timestamp_ts_config_t;

#define _shrIntrDataNumOfData64        8 
typedef struct _shr_cpri_intr_info_s {
    _shr_cpri_interrupt_type_t cpri_intr_type; /**< cpri interrupt type */
    int axc_id; /**< AxC Id */
    int queue_num; /**< queu num */
    int data; /**< reserved */
    uint64 data64[_shrIntrDataNumOfData64]; /**< reserved */
} _shr_cpri_intr_info_t;

/* sync up with bcm_cpri_port_interrupt_callback_t */
typedef void
(*_shr_cpri_port_interrupt_callback_t)(int unit, int port,
                                      _shr_cpri_intr_info_t *data,
                                      void *userdata);

typedef enum _shr_cpri_fec_ts_source_e {
    _shrCpriFecTsSource66b,
    _shrCpriFecTsSource257b
} _shr_cpri_fec_ts_source_t;

typedef enum _shr_cpri_fec_aux_config_e {
    _shrCpriFecAuxConfigScrambleBypassTx,
    _shrCpriFecAuxConfigDescrambleBypassRx,
    _shrCpriFecAuxConfigBypassIndicationEnable,
    _shrCpriFecAuxConfigBypassCorrectionEnable,
    _shrCpriFecAuxConfigCorruptAlternateBlocksEnable,
    _shrCpriFecAuxConfigSymbolErrorCorruptEnable,
    _shrCpriFecAuxConfigSymbolErrorTmrPeriod,
    _shrCpriFecAuxConfigSymbolErrCntThreshold,
    _shrCpriFecAuxConfigSetSymbErrWindow128,
    _shrCpriFecAuxConfigGboxTickDenominator,
    _shrCpriFecAuxConfigGboxTickNumerator,
    _shrCpriFecAuxConfigSoftReset
} _shr_cpri_fec_aux_config_t;

typedef struct _shr_cpri_fec_timestamp_config_e{
    int  enable;
    _shr_cpri_fec_ts_source_t fec_fifo_ts_source_select;  /* 66 or 257 */
    _shr_cpri_fec_ts_source_t fec_cmic_ts_source_select;  /* 66 or 257 */
    uint32 control_bit_offset_0;
    uint32 control_bit_offset_1;
    uint32 control_bit_offset_2;
    uint32 control_bit_offset_3;
} _shr_cpri_fec_timestamp_config_t;

/* ECC related enums */
typedef enum _shr_cpri_ecc_err_type_s {
    _shrCpriEccErrTypeNone,
    _shrCpriEccErrType1Bit,
    _shrCpriEccErrType2Bit,
    _shrCpriEccErrTypeMultiBit,
    _shrCpriEccErrTypeCount
}_shr_cpri_ecc_err_type_t;

/* RoE frame format */
typedef enum _shr_cpri_roe_frame_format_s {
    _shrCpriRoeFrameFormatIEEE1914,
    _shrCpriRoeFrameFormatRsvd3,
    _shrCpriRoeFrameFormatEcpri8,
    _shrCpriRoeFrameFormatEcpri12,
    _shrCpriRoeFrameFormatCount
}_shr_cpri_roe_frame_format_t;

/* RoE Order Info Type .orderInfoType */
typedef enum _shr_cpri_roe_ordering_info_option_s {
    _shrCpriRoeOrderingInfoOptionSeqNum,
    _shrCpriRoeOrderingInfoOptionTimestamp,
    _shrCpriRoeOrderingInfoOptionCount
}_shr_cpri_roe_ordering_info_option_t;

/* Field IDs for CPRI_ENCAP_HDR_FLD_MAPPING */
#define CPRIF_ENCAP_HEADER_FIELD_ID_SUBTYPE             0
#define CPRIF_ENCAP_HEADER_FIELD_ID_FLOW_ID             1
#define CPRIF_ENCAP_HEADER_FIELD_ID_LENGTH_15_8         2
#define CPRIF_ENCAP_HEADER_FIELD_ID_LENGTH_7_0          3
#define CPRIF_ENCAP_HEADER_FIELD_ID_ORDER_INFO_32_24    4
#define CPRIF_ENCAP_HEADER_FIELD_ID_ORDER_INFO_23_16    5
#define CPRIF_ENCAP_HEADER_FIELD_ID_ORDER_INFO_15_8     6
#define CPRIF_ENCAP_HEADER_FIELD_ID_ORDER_INFO_7_0      7
#define CPRIF_ENCAP_HEADER_FIELD_ID_OPCODE              8
#define CPRIF_ENCAP_HEADER_FIELD_ID_UNMAPPED            0xf

/* Encap RoE Header Field. */
typedef enum _shr_cpri_roe_header_field_e {
    _shrCpriRoeHeaderFieldSubType,             /* Subtype          */ 
    _shrCpriRoeHeaderFieldFlowID,              /* Flow ID          */
    _shrCpriRoeHeaderFieldLength_15_8,         /* Length 15:8      */
    _shrCpriRoeHeaderFieldLength_7_0,          /* Length 7:0       */ 
    _shrCpriRoeHeaderFieldOrderInfo_32_24,     /* OrderInfo 32:24  */
    _shrCpriRoeHeaderFieldOrderInfo_23_16,     /* OrderInfo 23:16  */
    _shrCpriRoeHeaderFieldOrderInfo_15_8,      /* OrderInfo 15:8   */
    _shrCpriRoeHeaderFieldOrderInfo_7_0,       /* OrderInfo 7:0    */
    _shrCpriRoeHeaderFieldOpcode,              /* Opcode           */ 
    _shrCpriRoeHeaderFieldUnmapped             /* Unmapped         */ 
} _shr_cpri_roe_header_field_t;

/* Decap RoE Header Field. */
typedef enum _shr_cpri_ecpri_header_field_e {
    _shrCpriEcpriHeaderFieldByte0,            /*  Header Byte 0  */
    _shrCpriEcpriHeaderFieldByte1,            /*  Header Byte 1  */
    _shrCpriEcpriHeaderFieldByte2,            /*  Header Byte 2  */
    _shrCpriEcpriHeaderFieldByte3,            /*  Header Byte 3  */
    _shrCpriEcpriHeaderFieldByte4,            /*  Header Byte 4  */
    _shrCpriEcpriHeaderFieldByte5,            /*  Header Byte 5  */
    _shrCpriEcpriHeaderFieldByte6,            /*  Header Byte 6  */
    _shrCpriEcpriHeaderFieldByte7,            /*  Header Byte 7  */
    _shrCpriEcpriHeaderFieldByte8,            /*  Header Byte 8  */
    _shrCpriEcpriHeaderFieldByte9,            /*  Header Byte 9  */
    _shrCpriEcpriHeaderFieldByte10,           /*  Header Byte 10 */
    _shrCpriEcpriHeaderFieldByte11,           /*  Header Byte 11 */

    _shrCpriEcpriHeaderFieldVlan0Byte0,       /*  VLAN0 Byte 0 */
    _shrCpriEcpriHeaderFieldVlan0Byte1,       /*  VLAN0 Byte 1 */
    _shrCpriEcpriHeaderFieldVlan1Byte0,       /*  VLAN1 Byte 0 */
    _shrCpriEcpriHeaderFieldVlan1Byte1       /*  VLAN1 Byte 1 */
} _shr_cpri_ecpri_header_field_t;

/* CPRI Port speeds */
typedef enum _shr_cpri_port_speed_e {
    _shrCpriPortSpeed1228p8 = 0,     /* 1228.8Mbps */
    _shrCpriPortSpeed2457p6 = 1,     /* 2457.6Mbps */
    _shrCpriPortSpeed3072p0 = 2,     /* 3072.0Mbps */
    _shrCpriPortSpeed4915p2 = 3,     /* 4915.2Mbps */
    _shrCpriPortSpeed6144p0 = 4,     /* 6144.0Mbps */
    _shrCpriPortSpeed9830p4 = 5,     /* 9830.4Mbps */
    _shrCpriPortSpeed10137p6 = 6,    /* 10137.6Mbps */
    _shrCpriPortSpeed12165p12 = 7,   /* 12165.12Mbps */
    _shrCpriPortSpeed24330p24 = 8,   /* 24330.24G */
    _shrCpriPortSpeedInvalid = 9     /* invalid, last entry */
} _shr_cpri_port_speed_t;

/* RSVD4 Config Field */
typedef enum _shr_rsvd4_tx_config_e {
    _shrRsvd4TxConfigForceOffState      = 0,
    _shrRsvd4TxConfigAckT               = 1,
    _shrRsvd4TxConfigForcedIdleAck      = 2,
    _shrRsvd4TxConfigLosEnable          = 3,
    _shrRsvd4TxConfigScrambleEnable     = 4,
    _shrRsvd4TxConfigScrambleSeed       = 5,
    _shrRsvd4TxConfigTransmitterEnable  = 6,
    _shrRsvd4TxConfigCount              = 7
}_shr_rsvd4_tx_config_t;

typedef enum _shr_rsvd4_rx_config_e {
    _shrRsvd4RxConfigDescrambleEnable        = 0,
    _shrRsvd4RxConfigSyncThreshold           = 1,
    _shrRsvd4RxConfigUnsyncThreshold         = 2,
    _shrRsvd4RxConfigFrameSyncThreshold      = 3,
    _shrRsvd4RxConfigFrameUnsyncThreshold    = 4,
    _shrRsvd4RxConfigBlockSize               = 5,
    _shrRsvd4RxConfigFwdIdleToFramer         = 6,
    _shrRsvd4RxConfigCount                   = 7
} _shr_rsvd4_rx_config_t;

/* CPRI Port speeds */
typedef enum _shr_cpri_fec_stat_type_e {
    _shrCpriFecStatTypeCorrectedCodeword    = 0,    /* Corrected Code Word      */
    _shrCpriFecStatTypeUncorrectedCodeword  = 1,    /* UnCorrected Code Word    */
    _shrCpriFecStatTypeBitError             = 2,    /* Bit Error.               */
    _shrCpriFecStatTypeSymbolError          = 3,    /* Symbol Error.            */
    _shrCpriFecStatTypeInvalid              = 4     /* invalid, last entry      */
} _shr_cpri_fec_stat_type_t;
/* CPRI Debug Attributes */
typedef enum _shr_cpri_deubg_attr_e {
    _shrCpriDeubgAttrDecapQueueStatus       = 0,    
    _shrCpriDeubgAttrEncapQueueStatus       = 1,    
    _shrCpriDeubgAttrDecapQueueCounter      = 2,    
    _shrCpriDeubgAttrEncapQueueCounter      = 3,    
    _shrCpriDeubgAttrDecapDroppedCounter    = 4,    
    _shrCpriDeubgAttrCount                  = 5
} _shr_cpri_deubg_attr_t;

#endif  /* !_SHR_CPRI_H */

