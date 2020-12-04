/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:       prbs.c
 */

#include "types.h"
#include "error.h"
#include "bcm_utils.h"
#include "tsc_functions.h"
#include "prbs.h"
#include "bcm_xmod_pe_api.h"

int _bcm_pe_tsc_get_tx_prbs_en (phy_ctrl_t *pa, uint8 *enable)
{

    ESTM(*enable = rd_prbs_gen_en());

    return _SHR_E_NONE;
}

int _bcm_pe_tsc_get_rx_prbs_en (phy_ctrl_t *pa, uint8 *enable)
{

    ESTM(*enable = rd_prbs_chk_en());

    return _SHR_E_NONE;
}

int _bcm_pe_prbs_tx_inv_data_get(phy_ctrl_t *pa, uint32 *inv_data)
{
    int __err;

    __err=_SHR_E_NONE;
    *inv_data = (uint32) rd_prbs_gen_inv();
    if(__err) return(__err);

    return _SHR_E_NONE;
}

int _bcm_pe_prbs_rx_inv_data_get(phy_ctrl_t *pa, uint32 *inv_data)
{
    int __err;

    __err=_SHR_E_NONE;
    *inv_data = (uint32) rd_prbs_gen_inv();
    if(__err) return(__err);

    return _SHR_E_NONE;
}

int _bcm_pe_prbs_tx_poly_get(phy_ctrl_t *pa, bcm_pe_prbs_poly_t *prbs_poly)
{
    int __err;

    __err=_SHR_E_NONE;
    *prbs_poly = rd_prbs_gen_mode_sel();
    if(__err) return(__err);

    return _SHR_E_NONE;
}

int _bcm_pe_prbs_rx_poly_get(phy_ctrl_t *pa, bcm_pe_prbs_poly_t *prbs_poly)
{
    int __err;

    __err=_SHR_E_NONE;
    *prbs_poly = rd_prbs_gen_mode_sel();
    if(__err) return(__err);

    return _SHR_E_NONE;
}

int _bcm_pe_prbs_tx_enable_get(phy_ctrl_t *pa, uint32 *enable)
{
    int __err;
    uint8 val = 0;

    __err = _bcm_pe_tsc_get_tx_prbs_en(pa, &val); 
    if(__err) return(__err);
    *enable = val;

    return _SHR_E_NONE;
}

int _bcm_pe_prbs_rx_enable_get(phy_ctrl_t *pa, uint32 *enable)
{
    int __err;
    uint8 val = 0;

    __err = _bcm_pe_tsc_get_rx_prbs_en(pa, &val); 
    if(__err) return(__err);
    *enable = val;

    return _SHR_E_NONE;
}

int _bcm_pe_tsc_tx_prbs_en(phy_ctrl_t *pa, uint8 enable)
{

    if (enable)
        EFUN(wr_prbs_gen_en(0x1));              /* Enable PRBS Generator */
    else
        EFUN(wr_prbs_gen_en(0x0));              /* Disable PRBS Generator */
    return _SHR_E_NONE;
}


int _bcm_pe_tsc_rx_prbs_en(phy_ctrl_t *pa, uint8 enable)
{

    if (enable)
        EFUN(wr_prbs_chk_en(0x1));              /* Enable PRBS Checker */
    else
        EFUN(wr_prbs_chk_en(0x0));              /* Disable PRBS Checker */
    return _SHR_E_NONE;
}

/* Configure PRBS Checker */
int _bcm_pe_tsc_config_rx_prbs(phy_ctrl_t *pa, bcm_pe_prbs_poly_t prbs_poly_mode, srds_prbs_checker_mode_t prbs_checker_mode, uint8 prbs_inv)
{

    EFUN(wr_prbs_chk_mode_sel((uint8)prbs_poly_mode));    /* PRBS Checker Polynomial mode sel  */
    EFUN(wr_prbs_chk_mode((uint8)prbs_checker_mode));     /* PRBS Checker mode sel (PRBS LOCK state machine select) */
    EFUN(wr_prbs_chk_inv(prbs_inv));                       /* PRBS Invert Enable/Disable */
    /* To enable PRBS Checker */
    /* EFUN(wr_prbs_chk_en(0x1)); */
    return _SHR_E_NONE;
}

int _bcm_pe_tsc_config_tx_prbs(phy_ctrl_t *pa, bcm_pe_prbs_poly_t prbs_poly_mode, uint8 prbs_inv)
{

    EFUN(wr_prbs_gen_mode_sel((uint8)prbs_poly_mode));    /* PRBS Generator mode sel */
    EFUN(wr_prbs_gen_inv(prbs_inv));                       /* PRBS Invert Enable/Disable */
    /* To enable PRBS Generator */
    /* EFUN(wr_prbs_gen_en(0x1)); */
    return _SHR_E_NONE;
}

int _bcm_pe_tsc_prbs_err_count_state(phy_ctrl_t *pa, uint32 *prbs_err_cnt, uint8 *lock_lost)
{

    if (!prbs_err_cnt || !lock_lost)
        return _SHR_E_RESOURCE;
    EFUN(bcm_pe_prbs_err_count_ll (pa, prbs_err_cnt));
    *lock_lost    = (*prbs_err_cnt >> 31);
    *prbs_err_cnt = (*prbs_err_cnt & 0x7FFFFFFF);
    return _SHR_E_NONE;
}


int bcm_pe_prbs_config_set(phy_ctrl_t *pa, uint32 flags , bcm_pe_prbs_t *prbs)
{

    /*first check which direction */
    if (BCM_PE_PRBS_DIRECTION_RX_GET(flags)) {
        EFUN(_bcm_pe_tsc_config_rx_prbs(pa, prbs->poly, PRBS_INITIAL_SEED_HYSTERESIS,  prbs->invert));
    } else if (BCM_PE_PRBS_DIRECTION_TX_GET(flags)) {
        EFUN(_bcm_pe_tsc_config_tx_prbs(pa, prbs->poly, prbs->invert));
    } else {
        EFUN(_bcm_pe_tsc_config_rx_prbs(pa, prbs->poly, PRBS_INITIAL_SEED_HYSTERESIS,  prbs->invert));
        EFUN(_bcm_pe_tsc_config_tx_prbs(pa, prbs->poly, prbs->invert));
    }
    return _SHR_E_NONE;
}

int bcm_pe_prbs_config_get(phy_ctrl_t *pa, uint32 flags , bcm_pe_prbs_t *prbs)
{
    bcm_pe_prbs_t config_tmp;

    if (BCM_PE_PRBS_DIRECTION_TX_GET(flags)) {
        EFUN(_bcm_pe_prbs_tx_inv_data_get(pa, &config_tmp.invert));
        EFUN(_bcm_pe_prbs_tx_poly_get(pa, &config_tmp.poly));
        prbs->invert = config_tmp.invert;
        prbs->poly = config_tmp.poly;
    } else if (BCM_PE_PRBS_DIRECTION_RX_GET(flags)) {
        EFUN(_bcm_pe_prbs_rx_inv_data_get(pa, &config_tmp.invert));
        EFUN(_bcm_pe_prbs_rx_poly_get(pa, &config_tmp.poly));
        prbs->invert = config_tmp.invert;
        prbs->poly = config_tmp.poly;
    } else {
        EFUN(_bcm_pe_prbs_tx_inv_data_get(pa, &config_tmp.invert));
        EFUN(_bcm_pe_prbs_tx_poly_get(pa, &config_tmp.poly));
        prbs->invert = config_tmp.invert;
        prbs->poly = config_tmp.poly;
    }
    return _SHR_E_NONE;
}

int bcm_pe_prbs_enable_set(phy_ctrl_t *pa, uint32 flags , uint32 enable)
{
    if (BCM_PE_PRBS_DIRECTION_TX_GET(flags)) {
        EFUN(_bcm_pe_tsc_tx_prbs_en(pa, enable));
    } else if (BCM_PE_PRBS_DIRECTION_RX_GET(flags)) {
        EFUN(_bcm_pe_tsc_rx_prbs_en(pa, enable));
    } else {
        EFUN(_bcm_pe_tsc_tx_prbs_en(pa, enable));
        EFUN(_bcm_pe_tsc_rx_prbs_en(pa, enable));
    }
    return _SHR_E_NONE;
}

int bcm_pe_prbs_enable_get(phy_ctrl_t *pa, uint32 flags , uint32 *enable)
{
    uint32 enable_tmp;
    
    if (BCM_PE_PRBS_DIRECTION_TX_GET(flags)) {
        EFUN(_bcm_pe_prbs_tx_enable_get(pa, &enable_tmp));
        *enable = enable_tmp;
    } else if (BCM_PE_PRBS_DIRECTION_RX_GET(flags)) {
        EFUN(_bcm_pe_prbs_rx_enable_get(pa, &enable_tmp));
        *enable = enable_tmp;
    } else {
        EFUN(_bcm_pe_prbs_tx_enable_get(pa, &enable_tmp));
        *enable = enable_tmp;
        EFUN(_bcm_pe_prbs_rx_enable_get(pa, &enable_tmp));
        *enable &= enable_tmp;
    }
    return _SHR_E_NONE;
}

int bcm_pe_prbs_status_get(phy_ctrl_t *pa, uint32 flags, bcm_pe_prbs_status_t *prbs_status)
{
    uint8 status = 0;
    uint32 prbs_err_count = 0;

    EFUN(bcm_pe_prbs_chk_lock_state(pa, &status));
    if (status) {
        prbs_status->prbs_lock = 1;
        /*next check the lost of lock and error count */
        status = 0;
        EFUN(_bcm_pe_tsc_prbs_err_count_state(pa, &prbs_err_count, &status));
        if (status) {
            /*temp lost of lock */
            prbs_status->prbs_lock_loss = 1;
        } else {
            prbs_status->prbs_lock_loss = 0;
            prbs_status->error_count = prbs_err_count;
        }
    } else {
        prbs_status->prbs_lock = 0;
    }
    return _SHR_E_NONE;
}

int bcm_do_prbs(phy_ctrl_t *pc, int sys_port, int action, int dir, int poly, int invert)
{
    int flags;
    bcm_pe_prbs_t prbs;
    bcm_pe_prbs_status_t prbs_status;
    char    *dirstr[3] = { "TX/RX","RX","TX"};
    char    *polystr[7] = { "Poly7","Poly9","Poly11","Poly15","Poly23","Poly31","Poly58"};

    if (action<PrbsActionSet || action>PrbsActionClear) {
        BCM_LOG_CLI("%s() action (%d) is out of range(0-%d)\n", __FUNCTION__, action, PrbsActionClear);
        return -1;
    }
    if (dir<PrbsDirTxRx || dir>PrbsDirTx) {
        BCM_LOG_CLI("%s() dir (%d) is out of range(0-%d)\n", __FUNCTION__, dir, PrbsDirTx);
        return -1;
    }

    flags = dir;

    if (!pc->module)
        pc->lane_mask = bcm_port_lane_mask(pc->unit, pc->module, sys_port);

    BCM_LOG_CLI("PRBS %s port: %d; lane_mask: 0x%x; direction: %s; %s\n",
                sys_port?"System":"Line", pc->phy_id, pc->lane_mask, dirstr[dir], polystr[poly]);

    pc->tsc_sys_port=sys_port;

    if (action==PrbsActionSet) {

        BCM_LOG_CLI("%s() Set PRBS\n", __FUNCTION__);

        /* activate PRBS */
        if (poly<0 || poly>=PrbsPolyCount) {
            BCM_LOG_CLI("%s() poly (%d) is out of range(0-%d)\n", __FUNCTION__, poly, PrbsPolyCount-1);
            return -1;
        }
        if (invert<0 || invert>1) {
            BCM_LOG_CLI("%s() invert (%d) is out of range(0-1)\n", __FUNCTION__, invert);
            return -1;
        }

        /* configure prbs */
        /* get config */
        SOC_IF_ERROR_RETURN(bcm_pe_prbs_config_get(pc, flags, &prbs));
        /* BCM_LOG_CLI("%s() config_get: poly: %d; invert: %d\n", __FUNCTION__, prbs.poly, prbs.invert); */

        /* set config */
        prbs.poly = poly;
        prbs.invert = invert;
        /* BCM_LOG_CLI("%s() config_set: poly: %d; invert: %d\n", __FUNCTION__, prbs.poly, prbs.invert); */
        SOC_IF_ERROR_RETURN(bcm_pe_prbs_config_set(pc, flags, &prbs));

        /* enable prbs */
        /* BCM_LOG_CLI("%s() enable prbs\n", __FUNCTION__); */
        bcm_pe_prbs_enable_set(pc, flags, 1);

    } else if (action==PrbsActionGet) {

        BCM_LOG_CLI("%s() Get PRBS\n", __FUNCTION__);

        /* read status */
        prbs_status.prbs_lock_loss = prbs_status.error_count = 0;
        SOC_IF_ERROR_RETURN(bcm_pe_prbs_status_get(pc, flags, &prbs_status));
        BCM_LOG_CLI("%s() prbs_lock: %d; prbs_lock_loss: %d; error_count: %d\n", __FUNCTION__,
                prbs_status.prbs_lock, prbs_status.prbs_lock_loss, prbs_status.error_count);

    } else {


        BCM_LOG_CLI("%s() Clear PRBS\n", __FUNCTION__);

        /* disable prbs */
        /* BCM_LOG_CLI("%s() disable prbs\n", __FUNCTION__); */
        bcm_pe_prbs_enable_set(pc, flags, 0);
    }

    return SOC_E_NONE;
}


