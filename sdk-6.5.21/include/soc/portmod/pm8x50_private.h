/*
 *
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 *
 * Note: DO NOT INCLUDE THIS FILE IN ANY OTHER FILE OTHER THAN
 *       pm8x50.c
 */

#ifndef _PM8X50_PRIVATE_H_
#define _PM8X50_PRIVATE_H_

/* portmod tsc interrupts type */
typedef enum pm8x50_tsc_intr_type_e {
    pm8x50TscIntrEccNone,
    pm8x50TscIntrEcc1bErr,
    pm8x50TscIntrEcc1bErrRx1588400g = pm8x50TscIntrEcc1bErr,
    pm8x50TscIntrEcc1bErrRx1588Mpp1,
    pm8x50TscIntrEcc1bErrRx1588Mpp0,
    pm8x50TscIntrEcc1bErrTx1588400g,
    pm8x50TscIntrEcc1bErrTx1588Mpp1,
    pm8x50TscIntrEcc1bErrTx1588Mpp0,
    pm8x50TscIntrEcc1bErrUmTable,
    pm8x50TscIntrEcc1bErrAmTable,
    pm8x50TscIntrEcc1bErrSpeedTable,
    pm8x50TscIntrEcc1bErrDeskew,
    pm8x50TscIntrEcc1bErrRsfecRs400gMpp1,
    pm8x50TscIntrEcc1bErrRsfecRs400gMpp0,
    pm8x50TscIntrEcc1bErrRsfecRbufMpp1,
    pm8x50TscIntrEcc1bErrRsfecRbufMpp0,
    pm8x50TscIntrEcc1bErrBaseRFec,
    pm8x50TscIntrEcc1bErrCount = pm8x50TscIntrEcc1bErrBaseRFec,
    pm8x50TscIntrEcc2bErr,
    pm8x50TscIntrEcc2bErrRx1588400g = pm8x50TscIntrEcc2bErr,
    pm8x50TscIntrEcc2bErrRx1588Mpp1,
    pm8x50TscIntrEcc2bErrRx1588Mpp0,
    pm8x50TscIntrEcc2bErrTx1588400g,
    pm8x50TscIntrEcc2bErrTx1588Mpp1,
    pm8x50TscIntrEcc2bErrTx1588Mpp0,
    pm8x50TscIntrEcc2bErrUmTable,
    pm8x50TscIntrEcc2bErrAmTable,
    pm8x50TscIntrEcc2bErrSpdTable,
    pm8x50TscIntrEcc2bErrDeskew,
    pm8x50TscIntrEcc2bErrRsfecRs400gMpp1,
    pm8x50TscIntrEcc2bErrRsfecRs400gMpp0,
    pm8x50TscIntrEcc2bErrRsfecRbufMpp1,
    pm8x50TscIntrEcc2bErrRsfecRbufMpp0,
    pm8x50TscIntrEcc2bErrBaseRFec,
    pm8x50TscIntrEcc2bErrCount = pm8x50TscIntrEcc2bErrBaseRFec,
    pm8x50TscIntrEccCount
}pm8x50_tsc_intr_type_t;


/*
 * Macros to indicate the interrupt types(1-bit or 2-bit)
 * based on the register fields of the Portmacro.
 * CDPORT_TSC_INTR_STATUS,
 * TSC_ECC_1b_ERR - 15-bit field.
 * TSC_ECC_2b_ERR - 15-bit field.
 */
#define PM8x50_TSC_ECC_1B_INTR     0x1
#define PM8x50_TSC_ECC_2B_INTR     0x2


/*
 * Each bit position in the register fields
 * TSC_ECC_2b_ERR, TSC_ECC_2b_ERR indicate
 * a TSC memory as under.
 * TSC_ECC_1b_ERR - 15-bit field.
 * TSC_ECC_2b_ERR - 15-bit field.
 *
 * bit 14: err_intr_en_rx_1588_400g
 * bit 13: err_intr_en_rx_1588_mpp1
 * bit 12: err_intr_en_rx_1588_mpp0
 * bit 11: err_intr_en_tx_1588_400g
 * bit 10: err_intr_en_tx_1588_mpp1
 * bit  9: err_intr_en_tx_1588_mpp0
 * bit  8: err_intr_en_um_tbl
 * bit  7: err_intr_en_am_tbl
 * bit  6: err_intr_en_spd_tbl
 * bit  5: err_intr_en_deskew
 * bit  4: err_intr_en_rsfec_rs400g_mpp1
 * bit  3: err_intr_en_rsfec_rs400g_mpp0
 * bit  2: err_intr_en_rsfec_rbuf_mpp1
 * bit  1: err_intr_en_rsfec_rbuf_mpp0
 * bit  0: err_intr_en_base_r_fec
 */
/* Macros for 1-bit errors. */
#define PM8x50_TSC_ECC_INTR_NONE                   0x0
#define PM8x50_TSC_ECC_1B_INTR_RX_1588_400G        (1 << 14)
#define PM8x50_TSC_ECC_1B_INTR_RX_1588_MPP1        (1 << 13)
#define PM8x50_TSC_ECC_1B_INTR_RX_1588_MPP0        (1 << 12)
#define PM8x50_TSC_ECC_1B_INTR_TX_1588_400G        (1 << 11)
#define PM8x50_TSC_ECC_1B_INTR_TX_1588_MPP1        (1 << 10)
#define PM8x50_TSC_ECC_1B_INTR_TX_1588_MPP0        (1 << 9)
#define PM8x50_TSC_ECC_1B_INTR_UM_TABLE            (1 << 8)
#define PM8x50_TSC_ECC_1B_INTR_AM_TABLE            (1 << 7)
#define PM8x50_TSC_ECC_1B_INTR_SPEED_TABLE         (1 << 6)
#define PM8x50_TSC_ECC_1B_INTR_DESKEW              (1 << 5)
#define PM8x50_TSC_ECC_1B_INTR_RSFEC_400G_MPP1     (1 << 4)
#define PM8x50_TSC_ECC_1B_INTR_RSFEC_400G_MPP0     (1 << 3)
#define PM8x50_TSC_ECC_1B_INTR_RSFEC_RBUF_MPP1     (1 << 2)
#define PM8x50_TSC_ECC_1B_INTR_RSFEC_RBUF_MPP0     (1 << 1)
#define PM8x50_TSC_ECC_1B_INTR_BASE_R_FEC          (1 << 0)

/* Macros for 2-bit errors. */
#define PM8x50_TSC_ECC_2B_INTR_RX_1588_400G        (1 << 14)
#define PM8x50_TSC_ECC_2B_INTR_RX_1588_MPP1        (1 << 13)
#define PM8x50_TSC_ECC_2B_INTR_RX_1588_MPP0        (1 << 12)
#define PM8x50_TSC_ECC_2B_INTR_TX_1588_400G        (1 << 11)
#define PM8x50_TSC_ECC_2B_INTR_TX_1588_MPP1        (1 << 10)
#define PM8x50_TSC_ECC_2B_INTR_TX_1588_MPP0        (1 << 9)
#define PM8x50_TSC_ECC_2B_INTR_UM_TABLE            (1 << 8)
#define PM8x50_TSC_ECC_2B_INTR_AM_TABLE            (1 << 7)
#define PM8x50_TSC_ECC_2B_INTR_SPEED_TABLE         (1 << 6)
#define PM8x50_TSC_ECC_2B_INTR_DESKEW              (1 << 5)
#define PM8x50_TSC_ECC_2B_INTR_RSFEC_400G_MPP1     (1 << 4)
#define PM8x50_TSC_ECC_2B_INTR_RSFEC_400G_MPP0     (1 << 3)
#define PM8x50_TSC_ECC_2B_INTR_RSFEC_RBUF_MPP1     (1 << 2)
#define PM8x50_TSC_ECC_2B_INTR_RSFEC_RBUF_MPP0     (1 << 1)
#define PM8x50_TSC_ECC_2B_INTR_BASE_R_FEC          (1 << 0)

#if 0
/*!
 * PM8x50 TSC to phymod_phy_interrupt_type map.
 */
typedef enum pm8x50_phymod_phy_interrupt_type_e {
     pm8x50PhymodIntrNone = 0x0,
     /**< RX 1588 400G ecc error */
     pm8x50PhymodIntrEccRx1588400g = phymodIntrEccRx1588400g,
     /**< RX 1588 Mpp1 ecc error */
     pm8x50PhymodIntrEccRx1588Mpp1 = phymodIntrEccRx1588Mpp1,
     /**< RX 1588 Mpp0 ecc error */
     pm8x50PhymodIntrEccRx1588Mpp0 = phymodIntrEccRx1588Mpp0,
     /**< TX 1588 400G ecc error */
     pm8x50PhymodIntrEccTx1588400g = phymodIntrEccTx1588400g,
     /**< TX 1588 Mpp1 ecc error */
     pm8x50PhymodIntrEccTx1588Mpp1 = phymodIntrEccTx1588Mpp1,
     /**< TX 1588 Mpp0 ecc error */
     pm8x50PhymodIntrEccTx1588Mpp0 = phymodIntrEccTx1588Mpp0,
     /**< UM table ecc error */
     pm8x50PhymodIntrEccUMTable = phymodIntrEccUMTable,
     /**< AM table ecc error */
     pm8x50PhymodIntrEccAMTable = phymodIntrEccAMTable,
     /**< Speed table ecc error */
     pm8x50PhymodIntrEccSpeedTable = phymodIntrEccSpeedTable,
     /**< Deskew ecc error */
     pm8x50PhymodIntrEccDeskew = phymodIntrEccDeskew,
     /**< Rsfec_Rs400g Mpp1 ecc error */
     pm8x50PhymodIntrEccRsfecRs400gMpp1 = phymodIntrEccRsFECRs400gMpp1,
     /**< Rsfec_Rs400g Mpp0 ecc error */
     pm8x50PhymodIntrEccRsfecRs400gMpp0 = phymodIntrEccRsFECRs400gMpp0,
     /**< RSFEC_RSBUF MPP1 ecc error */
     pm8x50PhymodIntrEccRsfecRbufMpp1 = phymodIntrEccRsFECRbufMpp1,
     /**< RSFEC_RSBUF MPP0 ecc error */
     pm8x50PhymodIntrEccRsfecRbufMpp0 = phymodIntrEccRsFECRbufMpp0,
     /**< BaseR FEC ecc error */
     pm8x50PhymodIntrEccBaseRFec = phymodIntrEccBaseRFEC,
     pm8x50PhymodIntrCount = phymodIntrCount
} pm8x50_phymod_phy_interrupt_type_t;
#endif

typedef struct pm8x50_tsc_err_intr_phymod_map_e{
    uint32  flags;
    phymod_interrupt_type_t phymod_intr_map;
    char    *str;
}pm8x50_tsc_err_intr_phymod_map_t;

pm8x50_tsc_err_intr_phymod_map_t
         pm8x50_tsc_ecc_intr_info [pm8x50TscIntrEccCount] = {
    {
        PM8x50_TSC_ECC_INTR_NONE,
        phymodIntrNone,
        "Interrupt none" 
    },
    {
        PM8x50_TSC_ECC_1B_INTR_RX_1588_400G,
        phymodIntrEccRx1588400g,
        "1-bit RX 1588 400G ecc error"
    },
    {
        PM8x50_TSC_ECC_1B_INTR_RX_1588_MPP1,
        phymodIntrEccRx1588Mpp1,
        "1-bit RX 1588 Mpp1 ecc error"
    },
    {
        PM8x50_TSC_ECC_1B_INTR_RX_1588_MPP0,
        phymodIntrEccRx1588Mpp0,
        "1-bit RX 1588 Mpp0 ecc error"
    },
    {
        PM8x50_TSC_ECC_1B_INTR_TX_1588_400G,
        phymodIntrEccTx1588400g,
        "1-bit TX 1588 400G ecc error"
    },
    {
        PM8x50_TSC_ECC_1B_INTR_TX_1588_MPP1,
        phymodIntrEccTx1588Mpp1,
        "1-bit TX 1588 Mpp1 ecc error"
    },
    {
        PM8x50_TSC_ECC_1B_INTR_TX_1588_MPP0,
        phymodIntrEccTx1588Mpp0,
        "1-bit TX 1588 Mpp0 ecc error"
    },
    {
        PM8x50_TSC_ECC_1B_INTR_UM_TABLE,
        phymodIntrEccUMTable,
        "1-bit Unique Marker table ecc error"
    },
    {
        PM8x50_TSC_ECC_1B_INTR_AM_TABLE,
        phymodIntrEccAMTable,
        "1-bit Alignment Marker table ecc error"
    },
    {
        PM8x50_TSC_ECC_1B_INTR_SPEED_TABLE,
        phymodIntrEccSpeedTable,
        "1-bit Speed table ecc error"
    },
    {
        PM8x50_TSC_ECC_1B_INTR_DESKEW,
        phymodIntrEccDeskew,
        "1-bit Deskew ecc error"
    },
    {
        PM8x50_TSC_ECC_1B_INTR_RSFEC_400G_MPP1,
        phymodIntrEccRsFECRs400gMpp1,
        "1-bit RsFEC_Rs400g Mpp1 ecc error"
    },
    {
        PM8x50_TSC_ECC_1B_INTR_RSFEC_400G_MPP0,
        phymodIntrEccRsFECRs400gMpp0,
        "1-bit RsFEC_Rs400g Mpp1 ecc error"
    },
    {
        PM8x50_TSC_ECC_1B_INTR_RSFEC_RBUF_MPP1,
        phymodIntrEccRsFECRbufMpp1,
        "1-bit RsFEC_RBUF MPP1 ecc error"
    },
    {
        PM8x50_TSC_ECC_1B_INTR_RSFEC_RBUF_MPP0,
        phymodIntrEccRsFECRbufMpp0,
        "1-bit RsFEC_RBUF MPP0 ecc error"
    },
    {
        PM8x50_TSC_ECC_1B_INTR_BASE_R_FEC,
        phymodIntrEccBaseRFEC,
        "1-bit BaseR FEC ecc error"
    },
    {
        PM8x50_TSC_ECC_2B_INTR_RX_1588_400G,
        phymodIntrEccRx1588400g,
        "2-bit RX 1588 400G ecc error"
    },
    {
        PM8x50_TSC_ECC_2B_INTR_RX_1588_MPP1,
        phymodIntrEccRx1588Mpp1,
        "2-bit RX 1588 Mpp1 ecc error"
    },
    {
        PM8x50_TSC_ECC_2B_INTR_RX_1588_MPP0,
        phymodIntrEccRx1588Mpp0,
        "2-bit RX 1588 Mpp0 ecc error"
    },
    {
        PM8x50_TSC_ECC_2B_INTR_TX_1588_400G,
        phymodIntrEccTx1588400g,
        "2-bit TX 1588 400G ecc error"
    },
    {
        PM8x50_TSC_ECC_2B_INTR_TX_1588_MPP1,
        phymodIntrEccTx1588Mpp1,
        "2-bit TX 1588 Mpp1 ecc error"
    },
    {
        PM8x50_TSC_ECC_2B_INTR_TX_1588_MPP0,
        phymodIntrEccTx1588Mpp0,
        "2-bit TX 1588 Mpp0 ecc error"
    },
    {
        PM8x50_TSC_ECC_2B_INTR_UM_TABLE,
        phymodIntrEccUMTable,
        "2-bit Unique Marker table ecc error"
    },
    {
        PM8x50_TSC_ECC_2B_INTR_AM_TABLE,
        phymodIntrEccAMTable,
        "2-bit Alignment Marker table ecc error"
    },
    {
        PM8x50_TSC_ECC_2B_INTR_SPEED_TABLE,
        phymodIntrEccSpeedTable,
        "2-bit Speed table ecc error"
    },
    {
        PM8x50_TSC_ECC_2B_INTR_DESKEW,
        phymodIntrEccDeskew,
        "2-bit Deskew ecc error"
    },
    {
        PM8x50_TSC_ECC_2B_INTR_RSFEC_400G_MPP1,
        phymodIntrEccRsFECRs400gMpp1,
        "2-bit RsFEC_Rs400g Mpp1 ecc error"
    },
    {
        PM8x50_TSC_ECC_2B_INTR_RSFEC_400G_MPP0,
        phymodIntrEccRsFECRs400gMpp0,
        "2-bit RsFEC_Rs400g Mpp1 ecc error"
    },
    {
        PM8x50_TSC_ECC_2B_INTR_RSFEC_RBUF_MPP1,
        phymodIntrEccRsFECRbufMpp1,
        "2-bit RsFEC_RBUF MPP1 ecc error"
    },
    {
        PM8x50_TSC_ECC_2B_INTR_RSFEC_RBUF_MPP0,
        phymodIntrEccRsFECRbufMpp0,
        "2-bit RsFEC_RBUF MPP0 ecc error"
    },
    {
        PM8x50_TSC_ECC_2B_INTR_BASE_R_FEC,
        phymodIntrEccBaseRFEC,
        "2-bit BaseR FEC ecc error"
     }
};

portmod_intr_reg_info_t pm8x50_mac_intr_info[] = {
    {
        PORTMOD_INTR_STATUS_COR | PORTMOD_INTR_BLOCK_CLEAR_REG |
        PORTMOD_INTR_ECC_1B_ERR,
        portmodIntrTypeMibMemSingleBitErr,
        CDMAC_INTR_ENABLEr, MIB_COUNTER_SINGLE_BIT_ERRf,
        CDMAC_INTR_STATUSr, MIB_COUNTER_SINGLE_BIT_ERRf,
        CDMAC_ECC_STATUSr, MIB_COUNTER_SINGLE_BIT_ERRf,
        "CDMAC MIB single bit error"
    },
    {
        PORTMOD_INTR_STATUS_COR | PORTMOD_INTR_BLOCK_CLEAR_REG |
        PORTMOD_INTR_ECC_2B_ERR,
        portmodIntrTypeMibMemDoubleBitErr,
        CDMAC_INTR_ENABLEr, MIB_COUNTER_DOUBLE_BIT_ERRf,
        CDMAC_INTR_STATUSr, MIB_COUNTER_DOUBLE_BIT_ERRf,
        CDMAC_ECC_STATUSr, MIB_COUNTER_DOUBLE_BIT_ERRf,
        "CDMAC MIB double bit error"
    },
    {
        PORTMOD_INTR_STATUS_COR | PORTMOD_INTR_BLOCK_CLEAR_REG |
        PORTMOD_INTR_ECC_MULTIB_ERR,
        portmodIntrTypeMibMemMultipleBitErr,
        CDMAC_INTR_ENABLEr, MIB_COUNTER_MULTIPLE_ERRf,
        CDMAC_INTR_STATUSr, MIB_COUNTER_MULTIPLE_ERRf,
        CDMAC_ECC_STATUSr, MIB_COUNTER_MULTIPLE_ERRf,
        "CDMAC MIB Multiple bit error"
    },
    {
        PORTMOD_INTR_STATUS_COR | PORTMOD_INTR_BLOCK_CLEAR_REG |
        PORTMOD_INTR_ECC_1B_ERR,
        portmodIntrTypeTxCdcSingleBitErr,
        CDMAC_INTR_ENABLEr, TX_CDC_SINGLE_BIT_ERRf,
        CDMAC_INTR_STATUSr, TX_CDC_SINGLE_BIT_ERRf,
        CDMAC_ECC_STATUSr, TX_CDC_SINGLE_BIT_ERRf,
        "CDMAC TX CDC single bit error"
    },
    {
        PORTMOD_INTR_STATUS_COR | PORTMOD_INTR_BLOCK_CLEAR_REG |
        PORTMOD_INTR_ECC_1B_ERR,
        portmodIntrTypeTxCdcDoubleBitErr,
        CDMAC_INTR_ENABLEr, TX_CDC_DOUBLE_BIT_ERRf,
        CDMAC_INTR_STATUSr, TX_CDC_DOUBLE_BIT_ERRf,
        CDMAC_ECC_STATUSr, TX_CDC_DOUBLE_BIT_ERRf,
        "CDMAC TX CDC double bit error"
    },
    {
        PORTMOD_INTR_STATUS_COR | PORTMOD_INTR_BLOCK_CLEAR_REG,
        portmodIntrTypeTxPktUnderflow,
        CDMAC_INTR_ENABLEr, TX_PKT_UNDERFLOWf,
        CDMAC_INTR_STATUSr, TX_PKT_UNDERFLOWf,
        CDMAC_INTR_STATUSr, TX_PKT_UNDERFLOWf,
        "CDMAC TX packet underflow interrupt"
    },
    {
        PORTMOD_INTR_STATUS_COR | PORTMOD_INTR_BLOCK_CLEAR_REG,
        portmodIntrTypeTxPktOverflow,
        CDMAC_INTR_ENABLEr, TX_PKT_OVERFLOWf,
        CDMAC_INTR_STATUSr, TX_PKT_OVERFLOWf,
        CDMAC_INTR_STATUSr, TX_PKT_OVERFLOWf,
        "CDMAC TX packet overflow interrupt"
    },
    {
        0x0, /* No flags */
        portmodIntrTypeCount,
        INVALIDr, INVALIDf,
        INVALIDr, INVALIDf,
        INVALIDr, INVALIDf,
        " "
    }
};

portmod_intr_reg_info_t pm8x50_tsc_core_intr_info[] = {
    {
        PORTMOD_INTR_STATUS_W1TC | PORTMOD_INTR_STATUS_BIT_FLDS |
        PORTMOD_INTR_ECC_2B_ERR,
        portmodIntrTypeTscCore0Err,
        INVALIDr, INVALIDf,
        CDPORT_TSC_INTR_STATUSr, TSC_ECC_2B_ERRf,
        INVALIDr, INVALIDf,
        " "
    },
    {
        PORTMOD_INTR_STATUS_W1TC | PORTMOD_INTR_STATUS_BIT_FLDS |
        PORTMOD_INTR_ECC_1B_ERR,
        portmodIntrTypeTscCore0Err,
        INVALIDr, INVALIDf,
        CDPORT_TSC_INTR_STATUSr, TSC_ECC_1B_ERRf,
        INVALIDr, INVALIDf,
        " "
    }
};

portmod_pm_intr_info_t pm8x50_pm_intr_info[] = {
    {
        portmodIntrTypeMacErr, CDPORT_INTR_ENABLEr, MAC_ERRf,
        1, pm8x50_mac_intr_info,
        CDPORT_INTR_STATUSr, MAC_ERRf,
        1, pm8x50_mac_intr_info,
        INVALIDr, INVALIDf,
        0, NULL,
        INVALIDm, "PM MAC error interrupt"
    },
    {
        portmodIntrTypeTscCore0Err, CDPORT_INTR_ENABLEr, TSC_CORE0_ERRf,
        1, pm8x50_tsc_core_intr_info,
        CDPORT_INTR_STATUSr, TSC_CORE0_ERRf,
        1, pm8x50_tsc_core_intr_info,
        INVALIDr, INVALIDf,
        0, NULL,
        INVALIDm, "PM TSC Core0 error interrupt"
    },
    {
        portmodIntrTypeTscCore1Err, CDPORT_INTR_ENABLEr, TSC_CORE1_ERRf,
        1, pm8x50_tsc_core_intr_info,
        CDPORT_INTR_STATUSr, TSC_CORE1_ERRf,
        1, pm8x50_tsc_core_intr_info,
        INVALIDr, INVALIDf,
        0, NULL,
        INVALIDm, "PM TSC Core1 error interrupt"
    },
    {
        portmodIntrTypeFcReqFull, CDPORT_INTR_ENABLEr, FLOWCONTROL_REQ_FULLf,
        0, NULL,
        CDPORT_INTR_STATUSr, FLOWCONTROL_REQ_FULLf,
        0, NULL,
        INVALIDr, INVALIDf,
        0, NULL,
        INVALIDm, "PM Flow cotrol request interrupt"
    },
    {
        portmodIntrTypePmdErr, CDPORT_INTR_ENABLEr, PMD_ERRf,
        0, NULL,
        CDPORT_INTR_STATUSr, PMD_ERRf,
        0, NULL,
        INVALIDr, INVALIDf,
        0, NULL,
        INVALIDm, "PM PMD error interrupt"
    }
};

#endif /*_PM8X50_PRIVATE_H_*/
