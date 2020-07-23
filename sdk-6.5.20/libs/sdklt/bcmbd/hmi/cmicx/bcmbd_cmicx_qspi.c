/*! \file bcmbd_cmicx_qspi.c
 *
 * CMICx QSPI driver for 8-bits mode.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_sleep.h>

#include <shr/shr_debug.h>
#include <shr/shr_timeout.h>

#include <bcmbd/bcmbd_cmicx_acc.h>
#include <bcmbd/bcmbd_qspi.h>
#include <bcmbd/bcmbd_qspi_internal.h>
#include <bcmbd/bcmbd_cmicx_qspi.h>

#define BSL_LOG_MODULE BSL_LS_BCMBD_QSPI

/*******************************************************************************
 * Local definitions
 */

/*! Number of prefetch buffer control registers. */
#define CMICX_QSPI_BSPI_B_CTRL_NUM 2

/*! Number of CDRAM registers. */
#define CMICX_QSPI_MSPI_CDRAM_NUM 16

/* CONTinue bit in CDRAM. */
#define CMICX_QSPI_MSPI_CDRAM_BIT_CONT (1 << 7)

/* Bits per transfer enable bit in CDRAM. */
#define CMICX_QSPI_MSPI_CDRAM_BIT_BITSE (1 << 6)

/* Peripheral chip select value in CDRAM. */
#define CMICX_QSPI_MSPI_CDRAM_PCS 0x2

/*! Number of TXRAM registers. */
#define CMICX_QSPI_MSPI_TXRAM_NUM 32

/*! Number of RXRAM registers. */
#define CMICX_QSPI_MSPI_RXRAM_NUM 32

/*! Dummy cycles for BSPI mode read. */
#define CMICX_BSPI_READ_DUMMY_CYCLES 8

/*! Opcode for BSPI mode read. */
#define CMICX_BSPI_CMD_FAST_READ 0xb

/*! System clock in MHz. */
#define CMICX_QSPI_SYS_CLOCK 429

/*! Baud rate in MHz */
#define CMICX_QSPI_SCL_BAUD_RATE 15

/*! QSPI operation timeout value in usec. */
#define CMICX_QSPI_WAIT_TIMEOUT 200000

/*! Bits per words for transfers. */
#define CMICX_QSPI_8BITS_PER_WORD 8

/*! The QSPI data/address lanes */
typedef enum cmicx_qspi_lane_mode_e {
    /*! Single lane */
    CMICX_QSPI_LANE_SINGLE = 0,

    /*! Dual lanes */
    CMICX_QSPI_LANE_DUAL = 0x1,

    /*! Quad lanes */
    CMICX_QSPI_LANE_QUAD = 0x2
} cmicx_qspi_lane_mode_t;


/*******************************************************************************
 * Private functions
 */

static int
cmicx_qspi_bspi_init(int unit)
{
    QSPI_BSPI_FLEX_MODE_ENABLEr_t flex_mode_en;
    QSPI_BSPI_BITS_PER_CYCLEr_t cycle_bits;
    QSPI_BSPI_BITS_PER_PHASEr_t phase_bits;
    QSPI_BSPI_CMD_AND_MODE_BYTEr_t cmd_mode;

    SHR_FUNC_ENTER(unit);

    /* Disable flex mode first */
    READ_QSPI_BSPI_FLEX_MODE_ENABLEr(unit, &flex_mode_en);
    QSPI_BSPI_FLEX_MODE_ENABLEr_BSPI_FLEX_MODE_ENABLEf_SET(flex_mode_en, 0);
    WRITE_QSPI_BSPI_FLEX_MODE_ENABLEr(unit, flex_mode_en);

    /* Data and address lanes */
    READ_QSPI_BSPI_BITS_PER_CYCLEr(unit, &cycle_bits);
    QSPI_BSPI_BITS_PER_CYCLEr_DATA_BPC_SELECTf_SET(cycle_bits,
                                                   CMICX_QSPI_LANE_QUAD);
    QSPI_BSPI_BITS_PER_CYCLEr_ADDR_BPC_SELECTf_SET(cycle_bits,
                                                   CMICX_QSPI_LANE_QUAD);
    WRITE_QSPI_BSPI_BITS_PER_CYCLEr(unit, cycle_bits);

    /* Dummy cycles */
    READ_QSPI_BSPI_BITS_PER_PHASEr(unit, &phase_bits);
    QSPI_BSPI_BITS_PER_PHASEr_DUMMY_CYCLESf_SET(phase_bits,
                                                CMICX_BSPI_READ_DUMMY_CYCLES);
    WRITE_QSPI_BSPI_BITS_PER_PHASEr(unit, phase_bits);

    /* Command byte for BSPI */
    READ_QSPI_BSPI_CMD_AND_MODE_BYTEr(unit, &cmd_mode);
    QSPI_BSPI_CMD_AND_MODE_BYTEr_BSPI_CMD_BYTEf_SET(cmd_mode,
                                                    CMICX_BSPI_CMD_FAST_READ);
    WRITE_QSPI_BSPI_CMD_AND_MODE_BYTEr(unit, cmd_mode);

    /* Enable flex mode to take effect */
    QSPI_BSPI_FLEX_MODE_ENABLEr_BSPI_FLEX_MODE_ENABLEf_SET(flex_mode_en, 1);
    WRITE_QSPI_BSPI_FLEX_MODE_ENABLEr(unit, flex_mode_en);

    SHR_FUNC_EXIT();
}

static int
cmicx_qspi_mspi_init(int unit, uint32_t mode)
{
    uint32_t val;
    QSPI_MSPI_SPCR1_LSBr_t spcr1_lsb;
    QSPI_MSPI_SPCR1_MSBr_t spcr1_msb;
    QSPI_MSPI_NEWQPr_t newqp;
    QSPI_MSPI_ENDQPr_t endqp;
    QSPI_MSPI_SPCR2r_t spcr2;
    QSPI_MSPI_SPCR0_LSBr_t spcr0_lsb;
    QSPI_MSPI_SPCR0_MSBr_t spcr0_msb;

    SHR_FUNC_ENTER(unit);

    /* Basic hardware initialization */
    QSPI_MSPI_SPCR1_LSBr_CLR(spcr1_lsb);
    WRITE_QSPI_MSPI_SPCR1_LSBr(unit, spcr1_lsb);
    QSPI_MSPI_SPCR1_MSBr_CLR(spcr1_msb);
    WRITE_QSPI_MSPI_SPCR1_MSBr(unit, spcr1_msb);
    QSPI_MSPI_NEWQPr_CLR(newqp);
    WRITE_QSPI_MSPI_NEWQPr(unit, newqp);
    QSPI_MSPI_ENDQPr_CLR(endqp);
    WRITE_QSPI_MSPI_ENDQPr(unit, endqp);
    QSPI_MSPI_SPCR2r_CLR(spcr2);
    WRITE_QSPI_MSPI_SPCR2r(unit, spcr2);

    /* Serial clock baud rate configuration */
    READ_QSPI_MSPI_SPCR0_LSBr(unit, &spcr0_lsb);
    val = CMICX_QSPI_SYS_CLOCK / (2 * CMICX_QSPI_SCL_BAUD_RATE);
    if (val < 8) {
        /* coverity[deadcode] */
        val = 8;
    }

    QSPI_MSPI_SPCR0_LSBr_SPBRf_SET(spcr0_lsb, val);
    WRITE_QSPI_MSPI_SPCR0_LSBr(unit, spcr0_lsb);

    /* Mode configuration (8 bits by default) */
    READ_QSPI_MSPI_SPCR0_MSBr(unit, &spcr0_msb);
    QSPI_MSPI_SPCR0_MSBr_MSTRf_SET(spcr0_msb, 1);
    QSPI_MSPI_SPCR0_MSBr_BITSf_SET(spcr0_msb, CMICX_QSPI_8BITS_PER_WORD);
    val = (mode & BCMBD_QSPI_MODE_CPHA) > 0;
    QSPI_MSPI_SPCR0_MSBr_CPHAf_SET(spcr0_msb, val);
    val = (mode & BCMBD_QSPI_MODE_CPOL) > 0;
    QSPI_MSPI_SPCR0_MSBr_CPOLf_SET(spcr0_msb, val);
    WRITE_QSPI_MSPI_SPCR0_MSBr(unit, spcr0_msb);

    SHR_FUNC_EXIT();
}

static int
cmicx_qspi_bspi_prefetch_buffer_flush(int unit)
{
    int idx;
    QSPI_BSPI_B_CTRLr_t buf_ctrl;

    SHR_FUNC_ENTER(unit);

    for (idx = 0; idx < CMICX_QSPI_BSPI_B_CTRL_NUM; idx++) {
        QSPI_BSPI_B_CTRLr_CLR(buf_ctrl);
        WRITE_QSPI_BSPI_B_CTRLr(unit, idx, buf_ctrl);
        QSPI_BSPI_B_CTRLr_SET(buf_ctrl, 1);
        WRITE_QSPI_BSPI_B_CTRLr(unit, idx, buf_ctrl);
    }

    SHR_FUNC_EXIT();
}

static int
cmicx_qspi_mspi_disable(int unit)
{
    int is_mspi = 0;
    QSPI_BSPI_MAST_N_BOOT_CTRLr_t qspi_ctrl;
    QSPI_MSPI_WRITE_LOCKr_t mspi_wr_lock;
    QSPI_MSPI_SPCR2r_t spcr2;

    SHR_FUNC_ENTER(unit);

    /* Check the current mode */
    READ_QSPI_BSPI_MAST_N_BOOT_CTRLr(unit, &qspi_ctrl);
    is_mspi = QSPI_BSPI_MAST_N_BOOT_CTRLr_MAST_N_BOOTf_GET(qspi_ctrl);
    if (!is_mspi) {
        SHR_EXIT();
    }

    /* Make sure no operation is in progress */
    QSPI_MSPI_SPCR2r_CLR(spcr2);
    WRITE_QSPI_MSPI_SPCR2r(unit, spcr2);
    sal_usleep(1);

    /* Disable MSPI write lock */
    QSPI_MSPI_WRITE_LOCKr_CLR(mspi_wr_lock);
    WRITE_QSPI_MSPI_WRITE_LOCKr(unit, mspi_wr_lock);

    /* Flush prefetch buffers */
    cmicx_qspi_bspi_prefetch_buffer_flush(unit);

    /* Switch to BSPI */
    QSPI_BSPI_MAST_N_BOOT_CTRLr_MAST_N_BOOTf_SET(qspi_ctrl, 0);
    WRITE_QSPI_BSPI_MAST_N_BOOT_CTRLr(unit, qspi_ctrl);

exit:
    SHR_FUNC_EXIT();
}

static int
cmicx_qspi_mspi_enable(int unit)
{
    int is_mspi = 0;
    shr_timeout_t to;
    QSPI_BSPI_MAST_N_BOOT_CTRLr_t qspi_ctrl;
    QSPI_BSPI_BUSY_STATUSr_t busy_st;
    QSPI_MSPI_WRITE_LOCKr_t mspi_wr_lock;

    SHR_FUNC_ENTER(unit);

    /* Check the current mode */
    READ_QSPI_BSPI_MAST_N_BOOT_CTRLr(unit, &qspi_ctrl);
    is_mspi = QSPI_BSPI_MAST_N_BOOT_CTRLr_MAST_N_BOOTf_GET(qspi_ctrl);
    if (is_mspi) {
        SHR_EXIT();
    }

    /* Check if boot loading is in progress. */
    shr_timeout_init(&to, CMICX_QSPI_WAIT_TIMEOUT, 0);
    do {
        READ_QSPI_BSPI_BUSY_STATUSr(unit, &busy_st);
        if (shr_timeout_check(&to)) {
            SHR_ERR_EXIT(SHR_E_TIMEOUT);
        }
    } while (QSPI_BSPI_BUSY_STATUSr_GET(busy_st) == 1);

    /* Switch to MSPI */
    QSPI_BSPI_MAST_N_BOOT_CTRLr_MAST_N_BOOTf_SET(qspi_ctrl, 1);
    WRITE_QSPI_BSPI_MAST_N_BOOT_CTRLr(unit, qspi_ctrl);
    sal_usleep(1);

    /* Enable MSPI write lock */
    QSPI_MSPI_WRITE_LOCKr_SET(mspi_wr_lock, 1);
    WRITE_QSPI_MSPI_WRITE_LOCKr(unit, mspi_wr_lock);

exit:
    SHR_FUNC_EXIT();
}

static int
cmicx_qspi_mspi_transfer(int unit, uint32_t flag,
                         size_t wlen, uint8_t *wbuf,
                         size_t rlen, uint8_t *rbuf)
{
    shr_timeout_t to;
    uint8_t *wdata_ptr, *rdata_ptr;
    QSPI_MSPI_CDRAMr_t cdram;
    QSPI_MSPI_TXRAMr_t txram;
    QSPI_MSPI_RXRAMr_t rxram;
    QSPI_MSPI_NEWQPr_t newqp;
    QSPI_MSPI_ENDQPr_t endqp;
    QSPI_MSPI_MSPI_STATUSr_t st;
    QSPI_MSPI_SPCR2r_t spcr2;

    SHR_FUNC_ENTER(unit);

    wdata_ptr = wbuf;
    rdata_ptr = rbuf;
    while ((wlen + rlen) > 0) {
        uint32_t idx, val;
        uint32_t slots, wslots, rslots;

        /* Determine how many tx and rx bytes to process this time */
        if (wlen > CMICX_QSPI_MSPI_CDRAM_NUM) {
            wslots = CMICX_QSPI_MSPI_CDRAM_NUM;
        } else {
            wslots = wlen;
        }
        wlen -= wslots;

        if (rlen > CMICX_QSPI_MSPI_CDRAM_NUM - wslots) {
            rslots = CMICX_QSPI_MSPI_CDRAM_NUM - wslots;
        } else {
            rslots = rlen;
        }
        rlen -= rslots;
        slots = wslots + rslots;

        /* Fill TXRAMs */
        if (wdata_ptr) {
            for (idx = 0; idx < wslots; idx++) {
                QSPI_MSPI_TXRAMr_CLR(txram);
                QSPI_MSPI_TXRAMr_TXRAMf_SET(txram, *wdata_ptr);
                WRITE_QSPI_MSPI_TXRAMr(unit, idx << 1, txram);
                wdata_ptr++;
            }
        }

        /* Fill CDRAMs */
        QSPI_MSPI_CDRAMr_CLR(cdram);
        val = CMICX_QSPI_MSPI_CDRAM_BIT_CONT | CMICX_QSPI_MSPI_CDRAM_PCS;
        QSPI_MSPI_CDRAMr_CDRAMf_SET(cdram, val);
        for (idx = 0; idx < slots; idx++) {
            WRITE_QSPI_MSPI_CDRAMr(unit, idx, cdram);
        }
        /* Deassert CONT bit for the last transfer */
        if ((wlen + rlen == 0) && (flag & BCMBD_QSPI_TRANS_END)) {
            QSPI_MSPI_CDRAMr_CDRAMf_SET(cdram, CMICX_QSPI_MSPI_CDRAM_PCS);
            WRITE_QSPI_MSPI_CDRAMr(unit, slots - 1, cdram);
        }

        /* Setup queue pointers */
        QSPI_MSPI_NEWQPr_CLR(newqp);
        WRITE_QSPI_MSPI_NEWQPr(unit, newqp);
        QSPI_MSPI_ENDQPr_SET(endqp, slots - 1);
        WRITE_QSPI_MSPI_ENDQPr(unit, endqp);

        /* Clear MSPI status */
        QSPI_MSPI_MSPI_STATUSr_CLR(st);
        WRITE_QSPI_MSPI_MSPI_STATUSr(unit, st);

        /* Kick off */
        QSPI_MSPI_SPCR2r_CLR(spcr2);
        QSPI_MSPI_SPCR2r_SPEf_SET(spcr2, 1);
        QSPI_MSPI_SPCR2r_CONT_AFTER_CMDf_SET(spcr2, 1);
        WRITE_QSPI_MSPI_SPCR2r(unit, spcr2);

        /* Wait for completion */
        shr_timeout_init(&to, CMICX_QSPI_WAIT_TIMEOUT, 0);
        do {
            READ_QSPI_MSPI_MSPI_STATUSr(unit, &st);
            if (shr_timeout_check(&to)) {
                SHR_ERR_EXIT(SHR_E_TIMEOUT);
            }
        } while (QSPI_MSPI_MSPI_STATUSr_SPIFf_GET(st) == 0);

        /* Read the received data */
        if (rdata_ptr) {
            for (idx = wslots; idx < slots; idx++) {
                READ_QSPI_MSPI_RXRAMr(unit, (idx << 1) + 1, &rxram);
                *rdata_ptr = QSPI_MSPI_RXRAMr_RXRAMf_GET(rxram);
                rdata_ptr++;
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Driver functions
 */

static int
cmicx_qspi_init(int unit, bcmbd_qspi_conf_t *conf)
{
    uint32_t val;
    CRU_CONTROLr_t cru_ctrl;

    SHR_FUNC_ENTER(unit);

    /* QSPI clock configuration */
    if (conf->max_hz >= 62500000) {
        val = 0x3;
    } else if (conf->max_hz >= 50000000) {
        val = 0x1;
    } else if (conf->max_hz >= 31250000) {
        val = 0x2;
    } else {
        val = 0;
    }
    READ_CRU_CONTROLr(unit, &cru_ctrl);
    CRU_CONTROLr_QSPI_CLK_SELf_SET(cru_ctrl, val);
    WRITE_CRU_CONTROLr(unit, cru_ctrl);

    /* BSPI mode initialization */
    SHR_IF_ERR_EXIT
        (cmicx_qspi_bspi_init(unit));

    /* MSPI mode initialization */
    SHR_IF_ERR_EXIT
        (cmicx_qspi_mspi_init(unit, conf->mode));

exit:
    SHR_FUNC_EXIT();
}

static int
cmicx_qspi_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Do nothing for now */

    SHR_FUNC_EXIT();
}

static int
cmicx_qspi_transfer(int unit, uint32_t flag,
                    size_t wlen, uint8_t *wbuf,
                    size_t rlen, uint8_t *rbuf)
{
    SHR_FUNC_ENTER(unit);

    if (flag & BCMBD_QSPI_TRANS_BEGIN) {
        SHR_IF_ERR_EXIT
            (cmicx_qspi_mspi_enable(unit));
    }

    SHR_IF_ERR_EXIT
        (cmicx_qspi_mspi_transfer(unit, flag, wlen, wbuf, rlen, rbuf));

    if (flag & BCMBD_QSPI_TRANS_END) {
        SHR_IF_ERR_EXIT
            (cmicx_qspi_mspi_disable(unit));
    }

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Driver object
 */

static bcmbd_qspi_drv_t cmicx_qspi_drv = {
    .init = cmicx_qspi_init,
    .cleanup = cmicx_qspi_cleanup,
    .transfer = cmicx_qspi_transfer,
};


/*******************************************************************************
 * Public functions
 */

int
bcmbd_cmicx_qspi_drv_init(int unit)
{
    return bcmbd_qspi_drv_init(unit, &cmicx_qspi_drv);
}
