/*! \file bcmbd_cmicd_miim.c
 *
 * CMICd MIIM operations
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_timeout.h>

#include <sal/sal_mutex.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmbd/bcmbd_cmicd_acc.h>

#include <bcmbd/bcmbd_cmicd_miim.h>

#define BSL_LOG_MODULE  BSL_LS_BCMBD_MIIM

/*******************************************************************************
 * Local definitions
 */

/* Operation types for PARAM_MIIM_CYCLE */
#define OPTYPE_INVALID          -1
#define OPTYPE_AUTO             0
#define OPTYPE_CL22_WR          1
#define OPTYPE_CL22_RD          2
#define OPTYPE_CL45_ADDR        4
#define OPTYPE_CL45_WR          5
#define OPTYPE_CL45_RD_INC      6
#define OPTYPE_CL45_RD          7

typedef struct miim_acc_s {
    bool wr;
    bool cl45;
    int optype;
} miim_acc_t;

/* Number of CMCs */
#define NUM_MIIM_CHAN           3

#define MIIM_OP_USEC_MAX        (10 * MSEC_USEC)
#define MIIM_OP_MIN_POLLS       1

typedef struct cmicd_miim_ctrl_s {
    sal_mutex_t chan_lock[NUM_MIIM_CHAN];
} cmicd_miim_ctrl_t;

#define MIIM_LOCK(_u, _op) \
    sal_mutex_take(miim_ctrl[_u].chan_lock[(_op)->chan], SAL_MUTEX_FOREVER)

#define MIIM_UNLOCK(_u, _op) \
    sal_mutex_give(miim_ctrl[_u].chan_lock[(_op)->chan])

/*******************************************************************************
 * Local data
 */

static int miim_max_bus[BCMDRD_CONFIG_MAX_UNITS];

static cmicd_miim_ctrl_t miim_ctrl[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private functions
 */

static void
miim_acc_init(bcmbd_miim_op_t *op, miim_acc_t *acc)
{
    sal_memset(acc, 0, sizeof(*acc));
    acc->optype = OPTYPE_INVALID;
    switch (op->opcode) {
    case BCMBD_MIIM_OPC_CL22_WRITE:
        acc->optype = OPTYPE_AUTO;
        acc->wr = true;
        break;
    case BCMBD_MIIM_OPC_CL22_READ:
        acc->optype = OPTYPE_AUTO;
        break;
    case BCMBD_MIIM_OPC_CL45_WRITE:
        acc->optype = OPTYPE_AUTO;
        acc->cl45 = true;
        acc->wr = true;
        break;
    case BCMBD_MIIM_OPC_CL45_READ:
        acc->optype = OPTYPE_AUTO;
        acc->cl45 = true;
        break;
    default:
        break;
    }
}

static int
cmicd_miim_cleanup(int unit)
{
    int chan;
    cmicd_miim_ctrl_t *mc;

    for (chan = 0; chan < NUM_MIIM_CHAN; chan++) {
        mc = &miim_ctrl[unit];
        if (mc->chan_lock[chan]) {
            sal_mutex_destroy(mc->chan_lock[chan]);
            mc->chan_lock[chan] = NULL;
        }
    }
    return SHR_E_NONE;
}

static int
cmicd_miim_init(int unit)
{
    int chan;
    cmicd_miim_ctrl_t *mc;

    for (chan = 0; chan < NUM_MIIM_CHAN; chan++) {
        mc = &miim_ctrl[unit];
        mc->chan_lock[chan] = sal_mutex_create("bcmbdMiimChan");
        if (mc->chan_lock[chan] == NULL) {
            cmicd_miim_cleanup(unit);
            return SHR_E_MEMORY;
        }
    }
    return SHR_E_NONE;
}

static int
cmicd_miim_op(int unit, bcmbd_miim_op_t *op, uint32_t *data)
{
    int ioerr = 0;
    int rv = SHR_E_NONE;
    CMIC_CMC_MIIM_PARAMr_t param;
    CMIC_CMC_MIIM_ADDRESSr_t addr;
    CMIC_CMC_MIIM_CTRLr_t ctrl;
    CMIC_CMC_MIIM_STATr_t stat;
    CMIC_CMC_MIIM_READ_DATAr_t rdata;
    miim_acc_t acc;
    shr_timeout_t to;

    /* Sanity checks */
    if (op == NULL || data == NULL) {
        return SHR_E_PARAM;
    }
    if (op->busmap) {
        /* No broadcast support */
        return SHR_E_PARAM;
    }
    if (op->chan < 0) {
        
        op->chan = 0;
    } else if (op->chan >= NUM_MIIM_CHAN) {
        return SHR_E_PARAM;
    }

    /* Protocol selection */
    miim_acc_init(op, &acc);
    if (acc.optype == OPTYPE_INVALID) {
        return SHR_E_PARAM;
    }

    /* Calculate PARAM register */
    CMIC_CMC_MIIM_PARAMr_CLR(param);
    CMIC_CMC_MIIM_PARAMr_PHY_IDf_SET(param, op->phyad);
    CMIC_CMC_MIIM_PARAMr_BUS_IDf_SET(param, op->busno);
    CMIC_CMC_MIIM_PARAMr_MIIM_CYCLEf_SET(param, acc.optype);
    if (op->internal) {
        if (miim_max_bus[unit] > 8) {
            /* Use register default layout */
            CMIC_CMC_MIIM_PARAMr_INTERNAL_SELf_SET(param, 1);
        } else {
            /* INTERNAL_SEL bit is bit 3 of the BUS_ID field */
            CMIC_CMC_MIIM_PARAMr_BUS_IDf_SET(param, op->busno | 0x8);
        }
    }
    if (acc.cl45) {
        CMIC_CMC_MIIM_PARAMr_C45_SELf_SET(param, *data);
    }
    if (acc.wr) {
        CMIC_CMC_MIIM_PARAMr_PHY_DATAf_SET(param, *data);
    }

    /* Calculate ADDRESS register */
    CMIC_CMC_MIIM_ADDRESSr_CLR(addr);
    CMIC_CMC_MIIM_ADDRESSr_CLAUSE_45_REGADRf_SET(addr, op->regad);
    if (acc.cl45) {
        CMIC_CMC_MIIM_ADDRESSr_CLAUSE_45_DTYPEf_SET(addr, op->devad);
    }

    MIIM_LOCK(unit, op);

    /* Write PARAM and ADDRESS registers */
    ioerr += WRITE_CMIC_CMC_MIIM_PARAMr(unit, op->chan, param);
    ioerr += WRITE_CMIC_CMC_MIIM_ADDRESSr(unit, op->chan, addr);

    /* Start MIIM operation */
    CMIC_CMC_MIIM_CTRLr_CLR(ctrl);
    ioerr += WRITE_CMIC_CMC_MIIM_CTRLr(unit, op->chan, ctrl);
    if (acc.wr) {
        CMIC_CMC_MIIM_CTRLr_MIIM_WR_STARTf_SET(ctrl, 1);
    } else {
        CMIC_CMC_MIIM_CTRLr_MIIM_RD_STARTf_SET(ctrl, 1);
    }
    ioerr += WRITE_CMIC_CMC_MIIM_CTRLr(unit, op->chan, ctrl);

    /* Poll for completion */
    shr_timeout_init(&to, MIIM_OP_USEC_MAX, MIIM_OP_MIN_POLLS);
    do {
        ioerr += READ_CMIC_CMC_MIIM_STATr(unit, op->chan, &stat);
        if (shr_timeout_check(&to)) {
            break;
        }
    } while (CMIC_CMC_MIIM_STATr_MIIM_OPN_DONEf_GET(stat) == 0);

    if (CMIC_CMC_MIIM_STATr_MIIM_OPN_DONEf_GET(stat) == 0) {
        rv = SHR_E_TIMEOUT;
    } else if (!acc.wr) {
        ioerr += READ_CMIC_CMC_MIIM_READ_DATAr(unit, op->chan, &rdata);
        *data = CMIC_CMC_MIIM_READ_DATAr_GET(rdata);
    }

    MIIM_UNLOCK(unit, op);

    if (ioerr) {
        rv = SHR_E_IO;
    }

    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "MIIM operation failed (op=%d %sbus=%"PRId32" "
                              "phy=%"PRId32" dev=%"PRId32" reg=0x%"PRIx32"\n"),
                   (int)op->opcode, op->internal ? "i" : "e",
                   op->busno, op->phyad, op->devad, op->regad));
    } else {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "MIIM cl%d-%s %sbus=%"PRId32" phyad=%"PRId32" "
                                "devad=%"PRId32" reg=0x%"PRIx32" "
                                "data=0x%"PRIx32"\n"),
                     acc.cl45 ? 45 : 22,
                     acc.wr ? "write" : "read",
                     op->internal ? "i" : "e",
                     op->busno, op->phyad, op->devad, op->regad, *data));
    }
    return rv;
}

static int
cmicd_miim_rate_config_set(int unit, bool internal, int busno,
                           bcmbd_miim_rate_config_t *ratecfg)
{
    int ioerr = 0;
    CMIC_RATE_ADJUSTr_t rate_ext;
    CMIC_RATE_ADJUST_INT_MDIOr_t rate_int;

    if (ratecfg == NULL) {
        return SHR_E_PARAM;
    }

    /* No per-bus setting */
    if (busno != -1) {
        return SHR_E_PARAM;
    }

    if (internal) {
        ioerr += READ_CMIC_RATE_ADJUST_INT_MDIOr(unit, &rate_int);
        CMIC_RATE_ADJUST_INT_MDIOr_DIVIDENDf_SET(rate_int, ratecfg->dividend);
        CMIC_RATE_ADJUST_INT_MDIOr_DIVISORf_SET(rate_int, ratecfg->divisor);
        ioerr += WRITE_CMIC_RATE_ADJUST_INT_MDIOr(unit, rate_int);
    } else {
        ioerr += READ_CMIC_RATE_ADJUSTr(unit, &rate_ext);
        CMIC_RATE_ADJUSTr_DIVIDENDf_SET(rate_ext, ratecfg->dividend);
        CMIC_RATE_ADJUSTr_DIVISORf_SET(rate_ext, ratecfg->divisor);
        ioerr += WRITE_CMIC_RATE_ADJUSTr(unit, rate_ext);
    }

    return ioerr ? SHR_E_FAIL : SHR_E_NONE;
}

/*******************************************************************************
 * Driver object
 */

static bcmbd_miim_drv_t cmicd_miim_drv = {
    .cleanup = cmicd_miim_cleanup,
    .init = cmicd_miim_init,
    .miim_op = cmicd_miim_op,
    .rate_config_set = cmicd_miim_rate_config_set
};

/*******************************************************************************
 * Public functions
 */

int
bcmbd_cmicd_miim_drv_init(int unit)
{
    return bcmbd_miim_drv_init(unit, &cmicd_miim_drv);
}

int
bcmbd_cmicd_miim_max_bus_set(int unit, int max_bus)
{
    miim_max_bus[unit] = max_bus;

    return SHR_E_NONE;
}
