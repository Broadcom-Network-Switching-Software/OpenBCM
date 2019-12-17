/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * Purpose: CMICx link scan module
 */
#include <sal/core/boot.h>
#include <sal/core/libc.h>
#include <sal/core/sync.h>
#include <shared/alloc.h>
#include <shared/bsl.h>

#if defined (BCM_ESW_SUPPORT) || defined(BCM_DNXF_SUPPORT) || defined(BCM_DNX_SUPPORT)
#include <soc/drv.h>
#include <soc/cm.h>
#include <soc/cmic.h>
#ifdef BCM_CMICX_SUPPORT
#include <shared/cmicfw/iproc_mbox.h>
#include <shared/cmicfw/iproc_m0ssq.h>
#include <shared/cmicfw/iproc_fwconfig.h>
#include <shared/cmicfw/cmicx_link_defs.h>
#include <shared/cmicfw/cmicx_link.h>
#include <shared/cmicfw/m0_ver.h>

static soc_iproc_m0ssq_shmem_t *ls_shmem = NULL;

static uint32 soc_iproc_sysreg_base_get(int unit)
{
   if (SOC_IS_HELIX5(unit)) {
      return IPROC_MDIO_SYS_BASE_HX5;
   } else {
      return IPROC_MDIO_SYS_BASE;
   }
}

uint32 soc_cmicx_miim_phy_reg_read(int unit, int index)
{
    uint32 base, val;
    uint32 offset = MIIM_PHY_LINK_STATUS0_OFFSET + (index * 4);

    base = soc_iproc_sysreg_base_get(unit);
    soc_iproc_getreg(unit, (base + offset), &val);

    return val;
}

void soc_cmicx_miim_phy_reg_write(int unit, int index, uint32 data)
{
    uint32 base;
    uint32 offset = MIIM_PHY_LINK_STATUS0_OFFSET + (index * 4);

    base = soc_iproc_sysreg_base_get(unit);
    soc_iproc_setreg(unit, (base + offset), data);
}


int soc_iproc_linkscan_process_message(soc_iproc_mbox_info_t *chan, soc_iproc_mbox_msg_t *msg);

/*
 * Function:
 *      _soc_iproc_l2p_pbmp_update
 * Purpose:
 *      Convert given port bitmap from logical to physical port numbering
 * Parameters:
 *      unit number
 *      port bitmap based on logical port numbering
 *      port bitmap converted to physical port numbering
 * Returns:
 *      none
 */
STATIC
void _soc_iproc_l2p_pbmp_update(int unit, soc_pbmp_t logical_pbmp, ls_phy_pbmp_t *phy_pbmp)
{
    soc_port_t port;
    int phy_port = 0;
    soc_info_t *si;

    si = &SOC_INFO(unit);

    SOC_PBMP_ITER(logical_pbmp, port) {
        phy_port = si->port_l2p_mapping[port];
        LS_PHY_PBMP_PORT_ADD(*phy_pbmp, phy_port);
    }

    return;
}

/*
 * Function:
 *      _soc_iproc_p2l_pbmp_update
 * Purpose:
 *      Convert given port bitmap from physical to logical port numbering
 * Parameters:
 *      unit number
 *      port bitmap based on physical port numbering
 *      port bitmap converted to logical port numbering
 * Returns:
 *      none
 */
STATIC
void _soc_iproc_p2l_pbmp_update(int unit, ls_phy_pbmp_t phy_pbmp, soc_pbmp_t *logical_pbmp)
{
    soc_port_t phy_port;
    int port = 0;
    soc_info_t *si;

    si = &SOC_INFO(unit);

    LS_PHY_PBMP_ITER(phy_pbmp, phy_port) {
        port = si->port_p2l_mapping[phy_port];
        SOC_PBMP_PORT_ADD(*logical_pbmp, port);
    }

    return;
}

#if (defined (BCM_DNXF_SUPPORT) || defined (BCM_DNX_SUPPORT))
/*
 * Function:
 *      _soc_iproc_add_cpu_port_to_pbmp
 * Purpose:
 *      Add cpu port(bit 0)to given port bitmap and adjust rest of ports accordingly
 *      This function is needed since port bitmap returned by FW taking into account
 *      cpu port at bit 0 which is required by XGS SDK code, but not required by
 *      DNX SDK code. So for DNX device, the function must be needed.
 * Parameters:
 *      unit number
 *      pointer to port bitmap containing incoming port bitmap
 *      port bitmap to be modified to contain cpu port(bit 0)
 *
 * Returns:
 *      none
 */
STATIC
void _soc_iproc_add_cpu_port_to_pbmp(int unit, uint32 *data, ls_phy_pbmp_t *phy_pbmp)
{
    uint32 tmp1 = 0, tmp2 = 0, idx = 0;

    /* For 1st word, save bit 31 before left shift to add cpu port */
    tmp1 = (data[idx] & 0x80000000) >> 31;
    LS_PHY_PBMP_WORD_SET(*phy_pbmp, 0, (data[idx] << 1));

    for(idx = 1; idx < LS_PHY_PBMP_WORD_MAX; idx++) {
        /* Set saved bit 31 from previous to bit 0 of next word */
        tmp2 = (data[idx] << 1) | tmp1;
        /* Add updated word to pbmp to be returned */
        LS_PHY_PBMP_WORD_SET(*phy_pbmp, idx, tmp2);
        /* Save bit 31 from current word to be used in next iteration */
        tmp1 = (data[idx] & 0x80000000) >> 31;
    }

    return;
}

/*
 * Function:
 *      _soc_iproc_remove_cpu_port_from_pbmp
 * Purpose:
 *      Add cpu port(bit 0)to given port bitmap and adjust rest of ports accordingly
 *      This function is needed since port bitmap returned by FW taking into account
 *      cpu port at bit 0 which is required by XGS SDK code, but not required by
 *      DNX SDK code. So for DNX device, the function must be needed.
 * Parameters:
 *      unit number
 *      pointer to message data containing incoming port bitmap
 *      port bitmap to be modified to contain cpu port
 * Returns:
 *      none
 */
STATIC
void _soc_iproc_remove_cpu_port_from_pbmp(int unit, ls_phy_pbmp_t logical_pbmp, uint32 *data)
{
    uint32 tmp = 0, idx = 0;

    for(idx = 0; idx < (LS_PHY_PBMP_WORD_MAX - 1); idx++) {
        /* Shift out last bit in current word */
        data[idx] = (LS_PHY_PBMP_WORD_GET(logical_pbmp, idx) >> 1);
        /* Get bit 0 from next word and move it to bit 31 of previous */
        tmp = LS_PHY_PBMP_WORD_GET(logical_pbmp, (idx + 1));
        tmp &= 0x1;
        tmp <<= 31;
        data[idx] |= tmp;
    }

    data[idx] = (LS_PHY_PBMP_WORD_GET(logical_pbmp, idx) >> 1);

    return;
}
#endif

/*
 * Function:
 *      _soc_iproc_optimized_cpu_port_to_pbmp_add
 * Purpose:
 *      Add cpu port to given port bitmap and adjust rest of ports accordingly
 *      This function is needed since port bitmap returned by FW does not take
 *      into account cpu port at bit 0 which is required by rest of SDK code
 *      CPU port is not tracked by HW linkscan FW.
 * Parameters:
 *      unit number
 *      port bitmap to be modified to contain cpu port
 * Returns:
 *      none
 */
STATIC
void _soc_iproc_optimized_cpu_port_to_pbmp_add(int unit, ls_phy_pbmp_t *dev_pbmp)
{
    uint32 tmp1 = 0, tmp2 = 0, idx = 0;
    uint32 reg1, reg2;

    /* DNX doesn't use CPU port at bit0, no need to shift. */
    if (SOC_IS_DNX(unit) || SOC_IS_DNXF(unit)) {
        for(idx = 0; idx < LS_PHY_PBMP_WORD_MAX; idx++) {
            reg1 = soc_cmicx_miim_phy_reg_read(unit, idx);
            LS_PHY_PBMP_WORD_SET(*dev_pbmp, idx, reg1);
        }
    } else {
        reg1 = soc_cmicx_miim_phy_reg_read(unit, idx);

        /* For 1st word, save bit 31 before left shift to add cpu port */
        tmp1 = (reg1 & 0x80000000) >> 31;
        LS_PHY_PBMP_WORD_SET(*dev_pbmp, 0, (reg1 << 1));

        for(idx = 1; idx < LS_PHY_PBMP_WORD_MAX; idx++) {
            /* Set saved bit 31 from previous to bit 0 of next word */
            reg2 = soc_cmicx_miim_phy_reg_read(unit, idx);
            tmp2 = (reg2 << 1) | tmp1;
            /* Add updated word to pbmp to be returned */
            LS_PHY_PBMP_WORD_SET(*dev_pbmp, idx, tmp2);
            /* Save bit 31 from current word to be used in next iteration */
            reg1 = soc_cmicx_miim_phy_reg_read(unit, idx);
            tmp1 = (reg1 & 0x80000000) >> 31;
        }
    }
    return;
}
/*
 * Function:
 *     soc_hx5_war_is_enable
 * Purpose:
 *     This function is only for HX5. It will check
 * if HX5HW-2279 is applicable for a given TSC core.
 *     Assume that port flexing won't change PMQ mode,
 * since PMQ qsgmii mode is not support flexing.
 * Parameters:
 *     unit number
 *     core Core number
 *          > 0, Initial WAR enable cache.
 * Returns:
 *     1 if core need WAR.
 */
static int
soc_hx5_war_is_enable(int unit, int core) {

   uint32 qmode = 0;
   uint32 otp_qmode = 0;
   static uint32 hx5_war_en[SOC_MAX_NUM_DEVICES] = {0};
   static uint32 hx5_war_en_valid[SOC_MAX_NUM_DEVICES] = {0};

   if (core < 0) {
       hx5_war_en_valid[unit] = 0;
   } else if (core > 2) {
       return 0;
   }

   if (hx5_war_en_valid[unit]) {
       return ((hx5_war_en[unit] & (1 << core)) != 0);
   }

   /* Read PMQ current mode */
   if (READ_TOP_MISC_GENERIC_CONTROLr(unit, &qmode) < 0) {
       return 0;
   };

   /* Read OTP (QSGMII_ENABLE) */
   READ_DMU_PCU_OTP_CONFIG_8r(unit, &otp_qmode);

   otp_qmode = (otp_qmode >> 25);
   otp_qmode = (otp_qmode & 0x7);
   qmode = qmode & 0x7;

   /*
    * if OTP (QSGMII_ENABLE) is not equal to PMQ current mode.
    * Then set "war enable" as 1
    */
   hx5_war_en[unit] = (otp_qmode ^ qmode);
   hx5_war_en_valid[unit] = 1;

   if (core < 0) {
       return (hx5_war_en[unit] != 0);
   } else {
       return ((hx5_war_en[unit] & (1 << core)) != 0);
   }

}


/*
 * Function:
 *      _soc_hx5_optimized_cpu_port_to_pbmp_add
 * Purpose:
 *      This function is only for HELIX5 with the same purpose with
 *      This function is needed since port bitmap returned by FW does not take
 *      into account cpu port at bit 0 which is required by rest of SDK code
 *      CPU port is not tracked by HW linkscan FW.
 * Parameters:
 *      unit number
 *      port bitmap to be modified to contain cpu port
 * Returns:
 *      none
 */
STATIC
void _soc_hx5_optimized_cpu_port_to_pbmp_add(int unit, ls_phy_pbmp_t *dev_pbmp)
{
    uint32 tmp1 = 0, tmp2 = 0, idx = 0;
    uint32 reg1, reg2;
    pbmp_t lpbmp;
    soc_port_t phy_port;
    int port = 0;
    soc_info_t *si;
    link_status_entry_t link_status;

    si = &SOC_INFO(unit);

    READ_MIIM_INT_PHY_LINK_RAW_STATUSr(unit, idx, &reg1);

    /* For 1st word, save bit 31 before left shift to add cpu port */
    tmp1 = (reg1 & 0x80000000) >> 31;
    LS_PHY_PBMP_WORD_SET(*dev_pbmp, 0, (reg1 << 1));

    for(idx = 1; idx < SOC_REG_NUMELS(unit, MIIM_INT_PHY_LINK_RAW_STATUSr); idx++) {

        /* Set saved bit 31 from previous to bit 0 of next word */
        READ_MIIM_INT_PHY_LINK_RAW_STATUSr(unit, idx, &reg2);
        tmp2 = (reg2 << 1) | tmp1;

        /* Add updated word to pbmp to be returned */
        LS_PHY_PBMP_WORD_SET(*dev_pbmp, idx, tmp2);

        /* Save bit 31 from current word to be used in next iteration */
        READ_MIIM_INT_PHY_LINK_RAW_STATUSr(unit, idx, &reg1);
        tmp1 = (reg1 & 0x80000000) >> 31;
    }

    SOC_PBMP_CLEAR(lpbmp);
    if (SOC_FAILURE(READ_LINK_STATUSm(unit, IPIPE_BLOCK(unit), 0, &link_status))) {
        return;
    };
    sal_memcpy(&lpbmp, &link_status, sizeof(link_status));

    /* WAR for HX5 */
    for (phy_port = 1; phy_port <= 48; phy_port++)
    {
        if (soc_hx5_war_is_enable(unit, (phy_port-1)/16))
        {
           port = si->port_p2l_mapping[phy_port];
           if (port > 0) {
               if (PBMP_MEMBER(lpbmp, port))
               {
                  LS_PHY_PBMP_PORT_ADD(*dev_pbmp, phy_port);
               } else {
                  LS_PHY_PBMP_PORT_REMOVE(*dev_pbmp, phy_port);
               }
           }
        }
    }

    return;
}
/*
 * Function:
 *      soc_cmicx_linkscan_hw_link_cache_get
 * Purpose:
 *      Get the current HW link status from linkscan cached copy
 * Parameters:
 *      unit number
 * Returns:
 *      SOC_E_NONE
 */
int soc_cmicx_linkscan_hw_link_cache_get(int unit, soc_pbmp_t *hw_link)
{
    soc_control_t   *soc = SOC_CONTROL(unit);
    int idx;

    for(idx = 0; idx < _SHR_PBMP_WORD_MAX; idx++) {
        SOC_PBMP_WORD_SET(*hw_link, idx, SOC_PBMP_WORD_GET(soc->cmicx_link_stat, idx));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_iproc_link_status_update
 * Purpose:
 *      Notify the link status change update from Cortex-M0 to linkscan thread
 * Parameters:
 *      unit number
 *      Linkscan message from cortex-M0
 *      contain_cpu_bit  indicate the message include cpu port bit.
 * Returns:
 *      SOC_E_NONE
 */
int soc_iproc_link_status_update(int unit, soc_iproc_mbox_msg_t *msg, int contain_cpu_bit)
{
    soc_control_t   *soc = SOC_CONTROL(unit);
    ls_phy_pbmp_t phy_pbmp;
#if (defined (BCM_DNXF_SUPPORT) || defined (BCM_DNX_SUPPORT))
    ls_phy_pbmp_t updated_phy_pbm;
#endif
    int idx = 0;

    /* Clear phy pbmp */
    LS_PHY_PBMP_CLEAR(phy_pbmp);

    /* Copy message payload */
    for (idx = 0; idx < LS_PHY_PBMP_WORD_MAX; idx++) {
        LS_PHY_PBMP_WORD_SET(phy_pbmp, idx, msg->data[idx]);
    }
#if (defined (BCM_DNXF_SUPPORT) || defined (BCM_DNX_SUPPORT))
    if (SOC_IS_DNX(unit) || SOC_IS_DNXF(unit)) {
       /*
        * DNX need to remove CPU port at bit0.
        * If it is in hw-config response,
        * CPU port bit0 are not included in msg
        */
        
        if (contain_cpu_bit)
        {
            LS_PHY_PBMP_CLEAR(updated_phy_pbm);
            _soc_iproc_remove_cpu_port_from_pbmp(unit, phy_pbmp, updated_phy_pbm.pbits);
            for(idx = 0; idx < LS_PHY_PBMP_WORD_MAX; idx++) {
                LS_PHY_PBMP_WORD_SET(phy_pbmp, idx, LS_PHY_PBMP_WORD_GET(updated_phy_pbm, idx));
            }
        }
        LS_PHY_PBMP_CLEAR(soc->cmicx_link_stat);
    }
#endif
    /* Convert physical pbmp to logical pbmp */
    _soc_iproc_p2l_pbmp_update(unit, phy_pbmp, &soc->cmicx_link_stat);

#if defined (BCM_ESW_SUPPORT)
    if (SOC_IS_ESW(unit)) {
        /* Inform linkscan thread */
        cmicx_esw_linkscan_hw_interrupt(unit);
    }
#endif

#if (defined (BCM_DNXF_SUPPORT) || defined (BCM_DNX_SUPPORT))
    if (SOC_IS_DNXF(unit) || SOC_IS_DNX(unit)) {
        /* Inform linkscan thread */
        cmicx_common_linkscan_hw_interrupt(unit);
    }
#endif

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_iproc_linkscan_process_message
 * Purpose:
 *      Linkscan application handler to handle messages in running state.
 * Parameters:
 *      unit number
 *      Iproc mbox info
 *      Linkscan message from cortex-M0
 * Returns:
 *      SOC_E_xxx
 */
int soc_iproc_linkscan_process_message(soc_iproc_mbox_info_t *chan, soc_iproc_mbox_msg_t *msg)
{
    int rv = 0;

    if (NULL == msg) {
        return SOC_E_PARAM;
    }

    switch(msg->id) {
        case LS_LINK_STATUS_CHANGE:
            
            return soc_iproc_link_status_update(chan->unit, msg, 1);
        default:
            LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(chan->unit, "Invalid M0 message id\n")));
            rv = IPROC_MBOX_ERR_INVCMD;
            break;
    }

    return rv;
}

/*
 * Function:
 *      soc_iproc_linkscan_msg_handler
 * Purpose:
 *      Linkscan message handler to handle messages from Cortex-M0 linkscan application.
 * Parameters:
 *      Registered parameter
 * Returns:
 *      SOC_E_xxx
 */
int soc_iproc_linkscan_msg_handler(void *param)
{
    int rv = 0, respidx;
    soc_iproc_mbox_msg_t *msg;
    soc_iproc_mbox_info_t *rxchan = (soc_iproc_mbox_info_t *)param;

    if (rxchan == NULL)
        return SOC_E_PARAM;

    while(!rv) {
        respidx = soc_iproc_data_recv(rxchan, &msg);
        if (respidx < 0) {
            rv = respidx;
            break;
        }

        if ((!IS_IPROC_RESP_READY(msg)) & (!IS_ASYNC_IPROC_STATUS(msg))) {
            soc_iproc_free(msg);
            break;
        } else {
            rv = soc_iproc_linkscan_process_message(rxchan, msg);
            LOG_VERBOSE(BSL_LS_SOC_M0, (BSL_META_U(rxchan->unit, " Msg processed %d\n"), msg->id));
            soc_iproc_free(msg);
        }
    }

    return rv;
}


/*
 * Function:
 *      soc_cmicx_link_fw_config_init
 * Purpose:
 *      Initialize linkscan firmware config.
 * Parameters:
 *      unit number
 * Returns:
 *      SOC_E_xxx
 */
int
soc_cmicx_link_fw_config_init(int unit) {

    FWLINKSCAN_CTRLr_t ctrl;

    /* Get shared memory of firmware linkscan. */
    SOC_IF_ERROR_RETURN(soc_iproc_m0ssq_shmem_get(unit, "linkscan", &ls_shmem));

    /* Clear shared memory of firmware linkscan as zero. */
    SOC_IF_ERROR_RETURN(soc_iproc_m0ssq_shmem_clear(ls_shmem));

    /* Pause firmware linkscan. */
    FWLINKSCAN_CTRLr_CLR(ctrl);
    FWLINKSCAN_CTRLr_PAUSEf_SET(ctrl, 1);
    SOC_IF_ERROR_RETURN(WRITE_FWLINKSCAN_CTRLr(ls_shmem, ctrl));

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_cmicx_linkscan_hw_init
 * Purpose:
 *      Initialize linkscan mbox and handler.
 * Parameters:
 *      unit number
 * Returns:
 *      SOC_E_xxx
 */
int soc_cmicx_linkscan_hw_init(int unit)
{
    int rv = 0;
    soc_control_t   *soc = SOC_CONTROL(unit);

    if (!soc->iproc_m0ls_init_done) {

        /* Allocate mailbox. */
        SOC_IF_ERROR_RETURN(soc->ls_mbox_id = soc_iproc_mbox_alloc(unit, U0_LINKSCAN_APP));
        soc->ls_txmbox= &soc->iproc_mbox_info[soc->ls_mbox_id][IPROC_MBOX_TYPE_TX];
        soc->ls_rxmbox= &soc->iproc_mbox_info[soc->ls_mbox_id][IPROC_MBOX_TYPE_RX];
        rv = soc_iproc_mbox_handler_register(unit, soc->ls_mbox_id, soc_iproc_linkscan_msg_handler, (void *) soc->ls_rxmbox);
        if (rv < 0) {
            LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "Linkscan Mbox handler register failed\n")));
            soc_iproc_mbox_free(unit, soc->ls_mbox_id);
            return rv;
        }

        soc->iproc_m0ls_init_done = 1;
    }

    return rv;
}

/*
 * Function:
 *      soc_cmicx_linkscan_hw_deinit
 * Purpose:
 *      Cleanup linkscan mbox and handler.
 * Parameters:
 *      unit number
 * Returns:
 *      SOC_E_xxx
 */
int soc_cmicx_linkscan_hw_deinit(int unit)
{
    int rv = 0;
    soc_control_t   *soc = SOC_CONTROL(unit);

    if (soc->iproc_m0ls_init_done) {
        soc_iproc_mbox_free(unit, soc->ls_mbox_id);
        soc->iproc_m0ls_init_done = 0;
    }

    return rv;
}

/* Max time for firmware to complete linkscan one time. (200ms) */
#define LINKSCAN_TIME_MAX        (200000)

/*
 * Function:
 *      soc_cmicx_linkscan_pause
 * Purpose:
 *      Pause firmware linkscan.
 * Parameters:
 *      unit number
 * Returns:
 *      SOC_E_xxx
 */
int soc_cmicx_linkscan_pause(int unit)
{
    FWLINKSCAN_STATUSr_t status;
    FWLINKSCAN_CTRLr_t ctrl;
    int run;
    soc_timeout_t to;

    LOG_VERBOSE(BSL_LS_SOC_M0, (BSL_META_U(unit, "M0 Linkscan pause\n")));

    SOC_IF_ERROR_RETURN(READ_FWLINKSCAN_CTRLr(ls_shmem, &ctrl));

    /* if firmware linkscan is paused then return. */
    if (FWLINKSCAN_CTRLr_PAUSEf_GET(ctrl) != 0) {
        return SOC_E_NONE;
    }

    /* Pause firmware linkscan. */
    FWLINKSCAN_CTRLr_PAUSEf_SET(ctrl, 1);
    SOC_IF_ERROR_RETURN(WRITE_FWLINKSCAN_CTRLr(ls_shmem, ctrl));

    /* Init timeout object with LINKSCAN_TIME_MAX, polling period 100usec. */
    soc_timeout_init(&to, LINKSCAN_TIME_MAX, 10);

    /* Ensure firmware being paused. */
    do {

        /* Check if firmware linkscan is running. */
        SOC_IF_ERROR_RETURN(READ_FWLINKSCAN_STATUSr(ls_shmem, &status));
        run = FWLINKSCAN_STATUSr_RUNf_GET(status);
        if (run == 0) {
            return SOC_E_NONE;
        }

    } while (!soc_timeout_check(&to));

    return SOC_E_TIMEOUT;
}

/*
 * Function:
 *      soc_cmicx_linkscan_continue
 * Purpose:
 *      Continue/start firmware linkscan.
 * Parameters:
 *      unit number
 * Returns:
 *      SOC_E_xxx
 */
int soc_cmicx_linkscan_continue(int unit)
{
    FWLINKSCAN_CTRLr_t ctrl;

    LOG_VERBOSE(BSL_LS_SOC_M0, (BSL_META_U(unit, "M0 Linkscan continue\n")));

    SOC_IF_ERROR_RETURN(READ_FWLINKSCAN_CTRLr(ls_shmem, &ctrl));

    /* if firmware linkscan is not paused then return. */
    if (FWLINKSCAN_CTRLr_PAUSEf_GET(ctrl) == 0) {
        return SOC_E_NONE;
    }

    /* Continue/clear pause of firmware linkscan. */
    FWLINKSCAN_CTRLr_PAUSEf_SET(ctrl, 0);
    SOC_IF_ERROR_RETURN(WRITE_FWLINKSCAN_CTRLr(ls_shmem, ctrl));

    return SOC_E_NONE;

}

/*
 * Function:
 *       soc_cmicx_linkscan_heartbeat
 * Purpose:
 *       Send LS_HW_HEARTBEAT message to Cortex-M0 linkscan application.
 * Parameters:
 *       unit number
 * Returns:
 *       SOC_E_XXX
 */
int soc_cmicx_linkscan_heartbeat(int unit, soc_ls_heartbeat_t *ls_heartbeat)
{
    soc_iproc_mbox_msg_t *msg, *resp;
    int rv;

    if (!SOC_CONTROL(unit)->iproc_mbox_init_done) {
        LOG_VERBOSE(BSL_LS_SOC_M0, (BSL_META_U(unit, "Mbox init not done\n")));
        return SOC_E_NONE;
    }

    LOG_VERBOSE(BSL_LS_SOC_M0, (BSL_META_U(unit, "M0 Linkscan heartbeat\n")));

    sal_memset(ls_heartbeat, 0, sizeof(soc_ls_heartbeat_t));

    ls_heartbeat->host_fw_version = (HOST_FW_VER_MAJOR << 16 | HOST_FW_VER_MINOR);

    msg = soc_iproc_alloc(sizeof(soc_iproc_mbox_msg_t) + sizeof(soc_ls_heartbeat_t));
    if (msg == NULL)
        return SOC_E_MEMORY;

    resp = soc_iproc_alloc(sizeof(soc_iproc_mbox_msg_t) + sizeof(soc_ls_heartbeat_t));
    if (resp == NULL) {
        soc_iproc_free(msg);
        return SOC_E_MEMORY;
    }

    msg->id = LS_HW_HEARTBEAT;
    msg->flags = IPROC_SYNC_MSG | IPROC_RESP_REQUIRED;
    msg->size = sizeof(soc_ls_heartbeat_t) / sizeof(uint32);
    if (sizeof(soc_ls_heartbeat_t) % sizeof(uint32)) {
        msg->size++;
    }

    /* Copy message data */
    sal_memcpy((void *)msg->data, (void *)ls_heartbeat, sizeof(soc_ls_heartbeat_t));

    rv = soc_iproc_data_send_wait(SOC_CONTROL(unit)->ls_txmbox, msg, resp);
    if (rv == IPROC_MSG_SUCCESS) {
        if (IS_IPROC_RESP_READY(resp)) {
            if (IS_IPROC_RESP_SUCCESS(resp)) {
                /* Copy response data */
                sal_memcpy((void *)ls_heartbeat, (void *)resp->data, sizeof(soc_ls_heartbeat_t));
                rv = SOC_E_NONE;
            } else {
                rv = resp->data[0];
            }
        }
    }

    soc_iproc_free(msg);
    soc_iproc_free(resp);

    return rv;
}

/*
 * Function:
 *      soc_cmicx_linkscan_hw_link_get
 * Purpose:
 *      Obtain actual device link status from miim registers
 * Parameters:
 *      unit number
 * Returns:
 *      SOC_E_xxx
 */
int soc_cmicx_linkscan_hw_link_get(int unit, soc_pbmp_t *hw_link)
{
    int rv;
    ls_phy_pbmp_t phy_hw_pbmp;

    LOG_VERBOSE(BSL_LS_SOC_M0, (BSL_META_U(unit, "M0 Linkscan hw link get\n")));

    if (SOC_IS_HELIX5(unit)) {
        /* Add CPU port to port bitmap since M0 FW responds without it */
        _soc_hx5_optimized_cpu_port_to_pbmp_add(unit, &phy_hw_pbmp);
    } else {
        /* Add CPU port to port bitmap since M0 FW responds without it */
        _soc_iproc_optimized_cpu_port_to_pbmp_add(unit, &phy_hw_pbmp);
    }

    /* Convert physical to logical port numbering */
    _soc_iproc_p2l_pbmp_update(unit, phy_hw_pbmp, hw_link);

    rv = SOC_E_NONE;
    return rv;
}

/*
 * Function:
 *       soc_cmicx_linkscan_config
 * Purpose:
 *       Send LS_HW_CONFIG message to Cortex-M0 linkscan application.
 * Parameters:
 *       unit number
 *       hw_mii_pbm - Port bit map of ports to scan with MIIM registers
 * Returns:
 *       SOC_E_XXX
 */
int soc_cmicx_linkscan_config(int unit, soc_pbmp_t *hw_mii_pbm)
{
    soc_iproc_mbox_msg_t *msg, *resp;
    int rv;
    soc_pbmp_t empty_pbmp;
    ls_phy_pbmp_t phy_mii_pbm;
#if (defined (BCM_DNXF_SUPPORT) || defined (BCM_DNX_SUPPORT))  
    ls_phy_pbmp_t updated_phy_pbm;
#endif
    int idx = 0;

    LOG_VERBOSE(BSL_LS_SOC_M0, (BSL_META_U(unit, "M0 Linkscan config\n")));

    if (SOC_IS_HELIX5(unit)) {
        /* Check HX5 WAR enable then cache the result. */
        soc_hx5_war_is_enable(unit, -1);
    }

    if (!SOC_CONTROL(unit)->iproc_mbox_init_done) {
        LOG_VERBOSE(BSL_LS_SOC_M0, (BSL_META_U(unit, "Mbox init not done\n")));
        return SOC_E_NONE;
    }

    SOC_PBMP_CLEAR(empty_pbmp);
    if (SOC_PBMP_EQ(*hw_mii_pbm, empty_pbmp)) {
        LOG_VERBOSE(BSL_LS_SOC_M0, (BSL_META_U(unit, "HW pbmp is empty..skip\n")));
        return SOC_E_NONE;
    }

    msg = soc_iproc_alloc(sizeof(soc_iproc_mbox_msg_t) + sizeof(ls_phy_pbmp_t));
    if (msg == NULL) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "HW Linkscan Mbox msg alloc failed\n")));
        return SOC_E_MEMORY;
    }

    resp = soc_iproc_alloc(sizeof(soc_iproc_mbox_msg_t) + sizeof(ls_phy_pbmp_t));
    if (resp == NULL) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "HW Linkscan Mbox resp alloc failed\n")));
        soc_iproc_free(msg);
        return SOC_E_MEMORY;
    }

    msg->id = LS_HW_CONFIG;
    msg->flags = IPROC_SYNC_MSG | IPROC_RESP_REQUIRED;
    msg->size = sizeof(ls_phy_pbmp_t) / sizeof(uint32);
    if (sizeof(ls_phy_pbmp_t) % sizeof(uint32)) {
        msg->size++;
    }

    LS_PHY_PBMP_CLEAR(phy_mii_pbm);

    /* Convert incoming logical pbmp to physical pbmp */
    _soc_iproc_l2p_pbmp_update(unit, *hw_mii_pbm, &phy_mii_pbm);

#if (defined (BCM_DNXF_SUPPORT) || defined (BCM_DNX_SUPPORT))
    /* DNX need to add CPU port at bit0. */
    if (SOC_IS_DNX(unit) || SOC_IS_DNXF(unit)) {
         LS_PHY_PBMP_CLEAR(updated_phy_pbm);
        _soc_iproc_add_cpu_port_to_pbmp(unit, phy_mii_pbm.pbits, &updated_phy_pbm);
        for(idx = 0; idx < LS_PHY_PBMP_WORD_MAX; idx++) {
            LS_PHY_PBMP_WORD_SET(phy_mii_pbm, idx, LS_PHY_PBMP_WORD_GET(updated_phy_pbm, idx));
        }
    }
#endif
    /* Copy message payload */
    for (idx = 0; idx < LS_PHY_PBMP_WORD_MAX; idx++) {
        msg->data[idx] = LS_PHY_PBMP_WORD_GET(phy_mii_pbm, idx);
    }

    /* Send message to M0 */
    rv = soc_iproc_data_send_wait(SOC_CONTROL(unit)->ls_txmbox, msg, resp);
    if (rv == IPROC_MSG_SUCCESS) {
        if (IS_IPROC_RESP_READY(resp)) {
            if (IS_IPROC_RESP_SUCCESS(resp)) {
                rv = SOC_E_NONE;
                /* Update linkscan thread with link status for configured port */
                
                soc_iproc_link_status_update(unit, resp, 0);
            } else {
                rv = resp->data[0];
            }
        }
    }

    soc_iproc_free(msg);
    soc_iproc_free(resp);

    return rv;
}
#endif /*BCM_CMICX_SUPPORT*/
#endif /*BCM_ESW_SUPPORT*/
