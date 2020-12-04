/*! \file bcmbd_cmicx_miim.c
 *
 * CMICx MIIM operations
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
#include <bcmbd/bcmbd_cmicx_acc.h>

#include <bcmbd/bcmbd_cmicx_miim.h>

#define BSL_LOG_MODULE  BSL_LS_BCMBD_MIIM

/*******************************************************************************
 * Local definitions
 */

/* Operation types for PARAMS_MDIO_OP_TYPE */
#define OPTYPE_INVALID          -1
#define OPTYPE_CL22_WR          0
#define OPTYPE_CL22_RD          1
#define OPTYPE_CL45_WR          2
#define OPTYPE_CL45_RD          3

typedef struct miim_acc_s {
    bool wr;
    bool cl45;
    int optype;
} miim_acc_t;

#define NUM_MIIM_CHAN           4
#define NUM_MIIM_RING           12

#define MIIM_OP_USEC_MAX        (10 * MSEC_USEC)
#define MIIM_OP_MIN_POLLS       1

typedef struct cmicx_miim_ctrl_s {
    sal_mutex_t chan_lock[NUM_MIIM_CHAN];
} cmicx_miim_ctrl_t;

#define MIIM_LOCK(_u, _op) \
    sal_mutex_take(miim_ctrl[_u].chan_lock[(_op)->chan], SAL_MUTEX_FOREVER)

#define MIIM_UNLOCK(_u, _op) \
    sal_mutex_give(miim_ctrl[_u].chan_lock[(_op)->chan])

/*******************************************************************************
 * Local data
 */

static cmicx_miim_ctrl_t miim_ctrl[BCMDRD_CONFIG_MAX_UNITS];

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
        acc->optype = OPTYPE_CL22_WR;
        acc->wr = true;
        break;
    case BCMBD_MIIM_OPC_CL22_READ:
        acc->optype = OPTYPE_CL22_RD;
        break;
    case BCMBD_MIIM_OPC_CL45_WRITE:
        acc->optype = OPTYPE_CL45_WR;
        acc->cl45 = true;
        acc->wr = true;
        break;
    case BCMBD_MIIM_OPC_CL45_READ:
        acc->optype = OPTYPE_CL45_RD;
        acc->cl45 = true;
        break;
    default:
        break;
    }
}

static int
cmicx_miim_cleanup(int unit)
{
    int chan;
    cmicx_miim_ctrl_t *mc;

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
cmicx_miim_init(int unit)
{
    int chan;
    cmicx_miim_ctrl_t *mc;

    for (chan = 0; chan < NUM_MIIM_CHAN; chan++) {
        mc = &miim_ctrl[unit];
        mc->chan_lock[chan] = sal_mutex_create("bcmbdMiimChan");
        if (mc->chan_lock[chan] == NULL) {
            cmicx_miim_cleanup(unit);
            return SHR_E_MEMORY;
        }
    }
    return SHR_E_NONE;
}

static int
cmicx_miim_op(int unit, bcmbd_miim_op_t *op, uint32_t *data)
{
    int ioerr = 0;
    int rv = SHR_E_NONE;
    MIIM_CH_PARAMSr_t params;
    MIIM_CH_ADDRESSr_t addr;
    MIIM_CH_CONTROLr_t ctrl;
    MIIM_CH_STATUSr_t stat;
    miim_acc_t acc;
    shr_timeout_t to;

    /* Sanity checks */
    if (op == NULL || data == NULL) {
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

    /* Calculate PARAMS register */
    MIIM_CH_PARAMSr_CLR(params);
    if (op->internal) {
        MIIM_CH_PARAMSr_SEL_INT_PHYf_SET(params, 1);
    }
    if (op->busmap) {
        MIIM_CH_PARAMSr_RING_MAPf_SET(params, op->busmap);
    } else {
        MIIM_CH_PARAMSr_RING_MAPf_SET(params, 1 << op->busno);
    }
    MIIM_CH_PARAMSr_MDIO_OP_TYPEf_SET(params, acc.optype);
    if (acc.wr) {
        MIIM_CH_PARAMSr_PHY_WR_DATAf_SET(params, *data);
    }

    /* Calculate ADDRESS register */
    MIIM_CH_ADDRESSr_CLR(addr);
    MIIM_CH_ADDRESSr_PHY_IDf_SET(addr, op->phyad);
    if (acc.cl45) {
        MIIM_CH_ADDRESSr_CLAUSE_22_REGADRR_OR_45_DTYPEf_SET(addr, op->devad);
        MIIM_CH_ADDRESSr_CLAUSE_45_REG_ADRRf_SET(addr, op->regad);
    } else {
        MIIM_CH_ADDRESSr_CLAUSE_22_REGADRR_OR_45_DTYPEf_SET(addr, op->regad);
    }

    MIIM_LOCK(unit, op);

    /* Write PARAMS and ADDRESS registers */
    ioerr += WRITE_MIIM_CH_PARAMSr(unit, op->chan, params);
    ioerr += WRITE_MIIM_CH_ADDRESSr(unit, op->chan, addr);

    /* Start MIIM operation */
    MIIM_CH_CONTROLr_CLR(ctrl);
    ioerr += WRITE_MIIM_CH_CONTROLr(unit, op->chan, ctrl);
    MIIM_CH_CONTROLr_STARTf_SET(ctrl, 1);
    ioerr += WRITE_MIIM_CH_CONTROLr(unit, op->chan, ctrl);

    /* Poll for completion */
    shr_timeout_init(&to, MIIM_OP_USEC_MAX, MIIM_OP_MIN_POLLS);
    do {
        ioerr += READ_MIIM_CH_STATUSr(unit, op->chan, &stat);
        if (shr_timeout_check(&to)) {
            break;
        }
    } while (MIIM_CH_STATUSr_DONEf_GET(stat) == 0);

    if (MIIM_CH_STATUSr_ERRORf_GET(stat) != 0) {
        rv = SHR_E_FAIL;
    } else if (MIIM_CH_STATUSr_DONEf_GET(stat) == 0) {
        rv = SHR_E_TIMEOUT;
    } else if (!acc.wr) {
        *data = MIIM_CH_STATUSr_PHY_RD_DATAf_GET(stat);
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
cmicx_miim_rate_config_set(int unit, bool internal, int busno,
                           bcmbd_miim_rate_config_t *ratecfg)
{
    int ioerr = 0;
    int rx;
    uint32_t divider;
    MIIM_RING_CONTROLr_t ring_ctl;

    if (ratecfg == NULL) {
        return SHR_E_PARAM;
    }

    if (busno < -1 || busno >= NUM_MIIM_RING) {
        return SHR_E_PARAM;
    }

    if (ratecfg->dividend != 1) {
        return SHR_E_PARAM;
    }

    for (rx = 0; rx < NUM_MIIM_RING; rx++) {
        if (busno >= 0 && rx != busno) {
            continue;
        }
        ioerr += READ_MIIM_RING_CONTROLr(unit, rx, &ring_ctl);
        divider = ratecfg->divisor;
        if (internal) {
            MIIM_RING_CONTROLr_CLOCK_DIVIDER_INTf_SET(ring_ctl, divider);
        } else {
            MIIM_RING_CONTROLr_CLOCK_DIVIDER_EXTf_SET(ring_ctl, divider);
        }
        ioerr += WRITE_MIIM_RING_CONTROLr(unit, rx, ring_ctl);
    }

    return ioerr ? SHR_E_FAIL : SHR_E_NONE;
}

/*******************************************************************************
 * Driver object
 */

static bcmbd_miim_drv_t cmicx_miim_drv = {
    .cleanup = cmicx_miim_cleanup,
    .init = cmicx_miim_init,
    .miim_op = cmicx_miim_op,
    .rate_config_set = cmicx_miim_rate_config_set
};

/*******************************************************************************
 * Public functions
 */

int
bcmbd_cmicx_miim_drv_init(int unit)
{
    return bcmbd_miim_drv_init(unit, &cmicx_miim_drv);
}
