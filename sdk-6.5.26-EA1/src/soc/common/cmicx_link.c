/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
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

/* CMICx thread safty lock. */
#define CMICX_LINK_LOCK(unit) \
        if (SOC_CONTROL(unit)->cmicx_link_lock) \
        { \
            sal_mutex_take(SOC_CONTROL(unit)->cmicx_link_lock, sal_mutex_FOREVER); \
        }
#define CMICX_LINK_UNLOCK(unit) \
        if (SOC_CONTROL(unit)->cmicx_link_lock) \
        { \
            sal_mutex_give(SOC_CONTROL(unit)->cmicx_link_lock); \
        }

#define  LS_SHMEM(unit) (SOC_CONTROL(unit)->ls_shmem)

#define HWCFG_ASYNC_ENABLE   "cmicx_ls_hw_cfg_async_enable"

#if defined (BCM_56870_A0)
/* Remapping linkscan register bits. */
static soc_cmicx_pport_to_lsbit_t bcm56870_a0_pport_to_lsbit[] = {
    /* pport,  lsbit */
    {  128,    131 },  /* 2nd Management port. */
};
#endif

#if (SRC_SOC_COMMON_ACTIVATE_UNUSED)
static uint32 soc_iproc_sysreg_base_get(int unit)
{
   if (SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit)) {
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
#endif

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
        /*
         * For Ramon, si->port_l2p_mapping[] are alway zero.
         * Assign phy port to be logic port.
         */
#ifdef BCM_DNXF_SUPPORT
        if (SOC_IS_DNXF(unit)) {
            phy_port = port;
        }
#endif
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
    SOC_PBMP_CLEAR(*logical_pbmp);
    if (SOC_IS_DNX(unit))
    {
        SOC_PBMP_ITER(PBMP_ALL(unit), port)
        {
            phy_port = si->port_l2p_mapping[port];
            if ((phy_port < LS_PHY_PBMP_PORT_MAX) && (phy_port >= 0))
            {
                if (LS_PHY_PBMP_MEMBER(phy_pbmp, phy_port))
                {
                    SOC_PBMP_PORT_ADD(*logical_pbmp, port);
                }
            }
        }
    }
    else
    {
        LS_PHY_PBMP_ITER(phy_pbmp, phy_port)
        {
            if (phy_port >= SOC_MAX_NUM_PORTS)
            {
                /* coverity[dead_error_line:FALSE] */
                continue;
            }
            port = si->port_p2l_mapping[phy_port];

#ifdef BCM_DNXF_SUPPORT
            if (SOC_IS_DNXF(unit))
            {
                /*
                 * For Ramon, si->port_l2p_mapping[] are alway zero.
                 * Assign phy port to be logic port.
                 */
                port = phy_port;
            }
#endif
            if ((port < _SHR_PBMP_PORT_MAX) && (port >= 0))
            {
                SOC_PBMP_PORT_ADD(*logical_pbmp, port);
            }
        }
    }

    return;
}

#if defined (BCM_56870_A0)
/*
 * Function:
 *      _soc_td3_pport_lsbit_remap
 * Purpose:
 *      Convert given port bitmap from physical to linkscan reg port numbering
 * Parameters:
 *      unit number
 *      port bitmap based on physical port numbering
 * Returns:
 *      none
 */
STATIC
void _soc_td3_pport_lsbit_remap(int unit, ls_phy_pbmp_t *pbm)
{
    int i;
    soc_cmicx_pport_to_lsbit_t *map;
    ls_phy_pbmp_t new;
    soc_info_t *si;

    si = &SOC_INFO(unit);

    /* Remapping physical port to linkscan bit offset. */
    sal_memcpy(&new, pbm, sizeof(ls_phy_pbmp_t));
    map = bcm56870_a0_pport_to_lsbit;
    for (i = (COUNTOF(bcm56870_a0_pport_to_lsbit) - 1); i >= 0; i--) {
        if (!IS_MANAGEMENT_PORT(unit, si->port_p2l_mapping[map[i].pport])) {
            continue;
        }
        if (LS_PHY_PBMP_MEMBER(*pbm, map[i].pport)) {
            LS_PHY_PBMP_PORT_REMOVE(new, map[i].pport);
            LS_PHY_PBMP_PORT_ADD(new, map[i].lsbit);
        }
    }
    sal_memcpy(pbm, &new, sizeof(ls_phy_pbmp_t));

    return;
}

/*
 * Function:
 *      _soc_td3_lsbit_pport_remap
 * Purpose:
 *      Convert given port bitmap from linkscan reg to physical port numbering
 * Parameters:
 *      unit number
 *      port bitmap based on linkscan reg port numbering
 * Returns:
 *      none
 */
STATIC
void _soc_td3_lsbit_pport_remap(int unit, ls_phy_pbmp_t *pbm)
{
    int i;
    soc_cmicx_pport_to_lsbit_t *map;
    ls_phy_pbmp_t new;
    soc_info_t *si;

    si = &SOC_INFO(unit);

    /* Remapping physical port to linkscan bit offset. */
    sal_memcpy(&new, pbm, sizeof(ls_phy_pbmp_t));
    map = bcm56870_a0_pport_to_lsbit;
    for (i = 0; i < COUNTOF(bcm56870_a0_pport_to_lsbit); i++) {
        if (!IS_MANAGEMENT_PORT(unit, si->port_p2l_mapping[map[i].pport])) {
            continue;
        }
        if (SOC_PBMP_MEMBER(*pbm, map[i].lsbit)) {
            SOC_PBMP_PORT_REMOVE(new, map[i].lsbit);
            SOC_PBMP_PORT_ADD(new, map[i].pport);
        }
    }
    sal_memcpy(pbm, &new, sizeof(ls_phy_pbmp_t));

    return;
}
#endif

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

    CMICX_LINK_LOCK(unit);
    for(idx = 0; idx < _SHR_PBMP_WORD_MAX; idx++) {
        SOC_PBMP_WORD_SET(*hw_link, idx, SOC_PBMP_WORD_GET(soc->cmicx_link_stat, idx));
    }
    CMICX_LINK_UNLOCK(unit);

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
 * Returns:
 *      SOC_E_NONE
 */
int soc_iproc_link_status_update(int unit, soc_iproc_mbox_msg_t *msg)
{
    soc_control_t   *soc = SOC_CONTROL(unit);
    ls_phy_pbmp_t phy_pbmp;
    int idx = 0;

    /* Clear phy pbmp */
    LS_PHY_PBMP_CLEAR(phy_pbmp);

    /* Copy message payload */
    for (idx = 0; idx < LS_PHY_PBMP_WORD_MAX; idx++) {
        LS_PHY_PBMP_WORD_SET(phy_pbmp, idx, msg->data[idx]);
    }

#if defined (BCM_56870_A0)
    /* Map linkscan port offset to physical port */
    if (SOC_IS_TRIDENT3(unit)) {
        _soc_td3_lsbit_pport_remap(unit, &phy_pbmp);
    }
#endif

    CMICX_LINK_LOCK(unit);
    /* Convert physical pbmp to logical pbmp */
    _soc_iproc_p2l_pbmp_update(unit, phy_pbmp, &soc->cmicx_link_stat);
    CMICX_LINK_UNLOCK(unit);

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

int
soc_iproc_flr_link_up(int unit, soc_iproc_mbox_msg_t *msg)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    int idx, wsize;
    soc_pbmp_t flr_linkup;
    ls_phy_pbmp_t flr_linkup_ppbmp;

    /* Get physical port bitmap from mail box. */
    LS_PHY_PBMP_CLEAR(flr_linkup_ppbmp);
    wsize = sizeof(ls_phy_pbmp_t) / sizeof(uint32);
    for (idx = 0; idx < wsize; idx ++) {
         LS_PHY_PBMP_WORD_SET(flr_linkup_ppbmp, idx, msg->data[idx]);
    }

    _soc_iproc_p2l_pbmp_update(unit, flr_linkup_ppbmp, &flr_linkup);

    if (soc->hw_linkscan_flr_linkup_cb) {
        soc->hw_linkscan_flr_linkup_cb(unit, flr_linkup);
        return SOC_E_NONE;
    } else {
        return SOC_E_UNAVAIL;
    }
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
            return soc_iproc_link_status_update(chan->unit, msg);
        case LS_HW_CONFIG:
            LOG_VERBOSE(BSL_LS_SOC_M0, (BSL_META_U(chan->unit, "HW LINK CFG msg recvd\n")));
            if (IS_IPROC_RESP_SUCCESS(msg)) {
                rv = SOC_E_NONE;
                if ((!SOC_IS_DNXF(chan->unit)) && (!SOC_IS_DNX(chan->unit))) {
                    /* Do nothing for configure response message for dnx/dnxf devices */
                    /* Update linkscan thread with link status for configured port */
                    soc_iproc_link_status_update(chan->unit, msg);
                }
            } else if (IS_ASYNC_IPROC_HW_CFG_FAIL(msg)) {
                LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(chan->unit, "HW LINK CFG failed\n")));
                rv = IPROC_MBOX_ERR_INTERNAL;
            }
            break;
        case LS_FLR_LINKUP:
            return soc_iproc_flr_link_up(chan->unit, msg);
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
#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
    FWLINKSCAN_SCHANr_t   schan;
    uint32 schan_num;
#endif

    /* Get shared memory of firmware linkscan. */
    SOC_IF_ERROR_RETURN(soc_iproc_m0ssq_shmem_get(unit, "linkscan", &LS_SHMEM(unit)));

    /* Clear shared memory of firmware linkscan as zero. */
    SOC_IF_ERROR_RETURN(soc_iproc_m0ssq_shmem_clear(LS_SHMEM(unit)));

    /* Pause firmware linkscan. */
    FWLINKSCAN_CTRLr_CLR(ctrl);
    FWLINKSCAN_CTRLr_PAUSEf_SET(ctrl, 1);
    SOC_IF_ERROR_RETURN(WRITE_FWLINKSCAN_CTRLr(LS_SHMEM(unit), ctrl));

#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
    if ((SOC_IS_DNX(unit)) || (SOC_IS_DNXF(unit)))
    {
        schan_num = soc_property_get(unit, spn_CMICX_LINK_SBUS_CHANNEL_ID, CMICX_LS_M0FW_SCHAN_DEF);

        /* Select SBUS channel for linkscan firmware. */
        FWLINKSCAN_SCHANr_CLR(schan);
        FWLINKSCAN_SCHANr_NUMf_SET(schan, schan_num);
        FWLINKSCAN_SCHANr_MAGICf_SET(schan, CMICX_LS_M0FW_SCHAN_MAGIC);
        SOC_IF_ERROR_RETURN(WRITE_FWLINKSCAN_SCHANr(LS_SHMEM(unit), schan));
    }
#endif

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
    FWLINKSCAN_CTRLr_t ctrl;
    uint32 hw_cfg_async = 0;
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

        /* Allocate CMICx-link mutex lock. */
        soc ->cmicx_link_lock = sal_mutex_create("cmicx_link_lock");

        soc->iproc_m0ls_init_done = 1;

        /* Set HW config async mode */
        SOC_IF_ERROR_RETURN(READ_FWLINKSCAN_CTRLr(LS_SHMEM(unit), &ctrl));
        hw_cfg_async = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE,
                                                   HWCFG_ASYNC_ENABLE,0);
        FWLINKSCAN_CTRLr_HW_CFG_ASYNCf_SET(ctrl, hw_cfg_async);
        SOC_IF_ERROR_RETURN(WRITE_FWLINKSCAN_CTRLr(LS_SHMEM(unit), ctrl));
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
        soc_iproc_m0ssq_shmem_free(LS_SHMEM(unit));
        soc_iproc_mbox_free(unit, soc->ls_mbox_id);
        sal_mutex_destroy(soc ->cmicx_link_lock);
        soc ->cmicx_link_lock = NULL;
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

    SOC_IF_ERROR_RETURN(READ_FWLINKSCAN_CTRLr(LS_SHMEM(unit), &ctrl));

    /* if firmware linkscan is paused then return. */
    if (FWLINKSCAN_CTRLr_PAUSEf_GET(ctrl) != 0) {
        return SOC_E_NONE;
    }

    /* Pause firmware linkscan. */
    FWLINKSCAN_CTRLr_PAUSEf_SET(ctrl, 1);
    SOC_IF_ERROR_RETURN(WRITE_FWLINKSCAN_CTRLr(LS_SHMEM(unit), ctrl));

    /* Init timeout object with LINKSCAN_TIME_MAX, polling period 100usec. */
    soc_timeout_init(&to, LINKSCAN_TIME_MAX, 10);

    /* Ensure firmware being paused. */
    do {

        /* Check if firmware linkscan is running. */
        SOC_IF_ERROR_RETURN(READ_FWLINKSCAN_STATUSr(LS_SHMEM(unit), &status));
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

    SOC_IF_ERROR_RETURN(READ_FWLINKSCAN_CTRLr(LS_SHMEM(unit), &ctrl));

    /* if firmware linkscan is not paused then return. */
    if (FWLINKSCAN_CTRLr_PAUSEf_GET(ctrl) == 0) {
        return SOC_E_NONE;
    }

    /* Continue/clear pause of firmware linkscan. */
    FWLINKSCAN_CTRLr_PAUSEf_SET(ctrl, 0);
    SOC_IF_ERROR_RETURN(WRITE_FWLINKSCAN_CTRLr(LS_SHMEM(unit), ctrl));

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
    int alloc_size;

    if (!SOC_CONTROL(unit)->iproc_mbox_init_done) {
        LOG_VERBOSE(BSL_LS_SOC_M0, (BSL_META_U(unit, "Mbox init not done\n")));
        return SOC_E_NONE;
    }

    LOG_VERBOSE(BSL_LS_SOC_M0, (BSL_META_U(unit, "M0 Linkscan heartbeat\n")));

    sal_memset(ls_heartbeat, 0, sizeof(soc_ls_heartbeat_t));

    ls_heartbeat->host_fw_version = (HOST_FW_VER_MAJOR << 16 | HOST_FW_VER_MINOR);

    alloc_size = sizeof(soc_iproc_mbox_msg_t) + sizeof(soc_ls_heartbeat_t);
    msg = soc_iproc_alloc(alloc_size);
    if (msg == NULL)
        return SOC_E_MEMORY;

    resp = soc_iproc_alloc(alloc_size);
    if (resp == NULL) {
        soc_iproc_free(msg);
        return SOC_E_MEMORY;
    }

    sal_memset(msg, 0, alloc_size);
    sal_memset(resp, 0, alloc_size);

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
#if (SRC_SOC_COMMON_ACTIVATE_UNUSED) || defined(BCM_ESW_SUPPORT)
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
    LOG_VERBOSE(BSL_LS_SOC_M0, (BSL_META_U(unit, "M0 Linkscan hw link get\n")));

    return soc_cmicx_linkscan_hw_link_cache_get(unit, hw_link);
}
#endif
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
    int idx = 0;
    int hw_sync_cfg = 0;
    int alloc_size;

    LOG_VERBOSE(BSL_LS_SOC_M0, (BSL_META_U(unit, "M0 Linkscan config\n")));

    if (!SOC_CONTROL(unit)->iproc_mbox_init_done) {
        LOG_VERBOSE(BSL_LS_SOC_M0, (BSL_META_U(unit, "Mbox init not done\n")));
        return SOC_E_NONE;
    }

    SOC_PBMP_CLEAR(empty_pbmp);
    if (SOC_PBMP_EQ(*hw_mii_pbm, empty_pbmp)) {
        LOG_VERBOSE(BSL_LS_SOC_M0, (BSL_META_U(unit, "HW pbmp is empty..skip\n")));
        return SOC_E_NONE;
    }

    alloc_size = sizeof(soc_iproc_mbox_msg_t) + sizeof(ls_phy_pbmp_t);
    msg = soc_iproc_alloc(alloc_size);
    if (msg == NULL) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "HW Linkscan Mbox msg alloc failed\n")));
        return SOC_E_MEMORY;
    }

    resp = soc_iproc_alloc(alloc_size);
    if (resp == NULL) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "HW Linkscan Mbox resp alloc failed\n")));
        soc_iproc_free(msg);
        return SOC_E_MEMORY;
    }

    sal_memset(msg, 0, alloc_size);
    sal_memset(resp, 0, alloc_size);

    msg->id = LS_HW_CONFIG;
    msg->flags = IPROC_SYNC_MSG | IPROC_RESP_REQUIRED;
    msg->size = sizeof(ls_phy_pbmp_t) / sizeof(uint32);
    if (sizeof(ls_phy_pbmp_t) % sizeof(uint32)) {
        msg->size++;
    }

    LS_PHY_PBMP_CLEAR(phy_mii_pbm);

    /* Convert incoming logical pbmp to physical pbmp */
    _soc_iproc_l2p_pbmp_update(unit, *hw_mii_pbm, &phy_mii_pbm);
#if defined (BCM_56870_A0)
    /* Map physical ports to linkscan port offset */
    if (SOC_IS_TRIDENT3(unit)) {
        _soc_td3_pport_lsbit_remap(unit, &phy_mii_pbm);
    }
#endif

    /* Copy message payload */
    for (idx = 0; idx < LS_PHY_PBMP_WORD_MAX; idx++) {
        msg->data[idx] = LS_PHY_PBMP_WORD_GET(phy_mii_pbm, idx);
    }

    /* If Linkscan HW Config Async mode is enabled, send async message. */
    hw_sync_cfg = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE,
                                              HWCFG_ASYNC_ENABLE,0);
    if (hw_sync_cfg) {
        rv = soc_iproc_data_send_nowait(SOC_CONTROL(unit)->ls_txmbox, msg);
    } else {
        /* If Linkscan HW Config Async mode is disabled, send sync message. */
        rv = soc_iproc_data_send_wait(SOC_CONTROL(unit)->ls_txmbox, msg, resp);
        if (rv == IPROC_MSG_SUCCESS) {
            if (IS_IPROC_RESP_READY(resp)) {
                if (IS_IPROC_RESP_SUCCESS(resp)) {
                    rv = SOC_E_NONE;
                    if ((!SOC_IS_DNXF(unit)) && (!SOC_IS_DNX(unit))) {
                        /* Do nothing for configure response message for dnx/dnxf devices */
                        /* Update linkscan thread with link status for configured port */
                        soc_iproc_link_status_update(unit, resp);
                    }
                } else {
                    rv = resp->data[0];
                }
            }
        }
    }

    soc_iproc_free(msg);
    soc_iproc_free(resp);

    return rv;
}

/*!
 * \brief Enable/disable fast link recovery per port.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] enable 0: disable, !=0 enable.
 *
 * \retval SOC_E_NONE No errors.
 */
int
soc_cmicx_linkscan_flr_enable_set(int unit, int pport, int enable)
{
    uint32 value;
    FWLINKSCAN_FLR_ENABLEr_t flr_enable;

    if (!SOC_UNIT_VALID(unit)) {
        return SOC_E_UNIT;
    }

    /* Convert enable to boolean. */
    enable = enable != 0;

    /* Set enable value to FW. */
    SOC_IF_ERROR_RETURN
        (READ_FWLINKSCAN_FLR_ENABLEr(LS_SHMEM(unit), pport / 32, &flr_enable));
    value = FWLINKSCAN_FLR_ENABLEr_GET(flr_enable);
    value &= ~(1 << (pport % 32));
    value |= (enable << (pport % 32));
    FWLINKSCAN_FLR_ENABLEr_SET(flr_enable, value);
    SOC_IF_ERROR_RETURN
        (WRITE_FWLINKSCAN_FLR_ENABLEr(LS_SHMEM(unit), pport / 32, flr_enable));

    return SOC_E_NONE;
}

/*!
 * \brief Configure the timeout threshold per port.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] ms Timeout threshold.
 *
 * \retval SOC_E_NONE No error.
 */
int
soc_cmicx_linkscan_flr_timeout_set(int unit, int pport, uint32 ms)
{
    FWLINKSCAN_FLR_TIMEOUTr_t timeout;

    if (!SOC_UNIT_VALID(unit)) {
        return SOC_E_UNIT;
    }

    /* Linkscan firmware time resolution is 5 ms. */
    ms /= 5;

    if (ms > 255) {
        return SOC_E_PARAM;
    }

    SOC_IF_ERROR_RETURN
        (READ_FWLINKSCAN_FLR_TIMEOUTr(LS_SHMEM(unit), pport / 4, &timeout));

    switch (pport % 4)
    {
        case 0:
            FWLINKSCAN_FLR_TIMEOUTr_BYTE0f_SET(timeout, ms);
            break;
        case 1:
            FWLINKSCAN_FLR_TIMEOUTr_BYTE1f_SET(timeout, ms);
            break;
        case 2:
            FWLINKSCAN_FLR_TIMEOUTr_BYTE2f_SET(timeout, ms);
            break;
        case 3:
            FWLINKSCAN_FLR_TIMEOUTr_BYTE3f_SET(timeout, ms);
            break;
        default:
            break;
    }
    SOC_IF_ERROR_RETURN
        (WRITE_FWLINKSCAN_FLR_TIMEOUTr(LS_SHMEM(unit), pport / 4,timeout));

    return SOC_E_NONE;
}

/*!
 * \brief Get the FLR linkup event counter value.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] ms Timeout threshold.
 *
 * \retval SHR_E_NONE No errors, otherwise failure in initializing hardware
 *         linkscan.
 */
int
soc_cmicx_linkscan_flr_linkup_count_get(int unit, int pport, uint32 *count)
{
    FWLINKSCAN_FLR_LINKUP_CNTr_t flr_linkup_cnt;

    if (!SOC_UNIT_VALID(unit)) {
        return SOC_E_UNIT;
    }

    SOC_IF_ERROR_RETURN
        (READ_FWLINKSCAN_FLR_LINKUP_CNTr(LS_SHMEM(unit),
                                         pport / 4, &flr_linkup_cnt));

    switch (pport % 4)
    {
        case 0:
            *count = FWLINKSCAN_FLR_LINKUP_CNTr_BYTE0f_GET(flr_linkup_cnt);
            FWLINKSCAN_FLR_LINKUP_CNTr_BYTE0f_SET(flr_linkup_cnt, 0);
            break;
        case 1:
            *count = FWLINKSCAN_FLR_LINKUP_CNTr_BYTE1f_GET(flr_linkup_cnt);
            FWLINKSCAN_FLR_LINKUP_CNTr_BYTE1f_SET(flr_linkup_cnt, 0);
            break;
        case 2:
            *count = FWLINKSCAN_FLR_LINKUP_CNTr_BYTE2f_GET(flr_linkup_cnt);
            FWLINKSCAN_FLR_LINKUP_CNTr_BYTE2f_SET(flr_linkup_cnt, 0);
            break;
        case 3:
            *count = FWLINKSCAN_FLR_LINKUP_CNTr_BYTE3f_GET(flr_linkup_cnt);
            FWLINKSCAN_FLR_LINKUP_CNTr_BYTE3f_SET(flr_linkup_cnt, 0);
            break;
        default:
            break;
    }

    SOC_IF_ERROR_RETURN
        (WRITE_FWLINKSCAN_FLR_LINKUP_CNTr(LS_SHMEM(unit),
                                          pport / 4, flr_linkup_cnt));

    return SOC_E_NONE;
}

int
soc_cmicx_linkscan_flr_cb_enable_set(int unit, int en)
{
    FWLINKSCAN_CTRLr_t ctrl;

    SOC_IF_ERROR_RETURN
        (READ_FWLINKSCAN_CTRLr(LS_SHMEM(unit), &ctrl));

    FWLINKSCAN_CTRLr_FLR_LINKUP_CB_ENf_SET(ctrl, (en != 0));

    SOC_IF_ERROR_RETURN
        (WRITE_FWLINKSCAN_CTRLr(LS_SHMEM(unit), ctrl));

    return SOC_E_NONE;
}

static soc_linkscan_drv_t cmicx_linkscan_drv  = {
     .flr_enable_set = soc_cmicx_linkscan_flr_enable_set,
     .flr_timeout_set = soc_cmicx_linkscan_flr_timeout_set,
     .flr_linkup_cnt_get = soc_cmicx_linkscan_flr_linkup_count_get,
     .flr_linkup_cb_en_set = soc_cmicx_linkscan_flr_cb_enable_set

};

/*!
 * \brief Init CMICx linkscan driver.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 *
 */
int
soc_cmicx_linkscan_driver_init(int unit)
{
    return soc_linkscan_drv_set(unit, &cmicx_linkscan_drv);
}

#if defined(BCM_DNXF_SUPPORT) || defined(BCM_DNX_SUPPORT)
/*
 * Function:
 *      soc_cmicx_linkscan_dump
 * Purpose:
 *      dump linkscan m0 related configure
 * Parameters:
 *      unit number
 * Returns:
 *      SOC_E_xxx
 */
int soc_cmicx_linkscan_dump(int unit)
{
    uint32 ii, cpu_id, dev_id;
    FWLINKSCAN_CTRLr_t ctrl;
    FWLINKSCAN_STATUSr_t status;
    FWLINKSCAN_SCHANr_t schan;
    uint32 value[4];
    char* str_m0_counter_name[] =
            {
                "HW linkscan polling number",
                "SCHAN abort number",
                "HW linkscan link change number",
                "M0 Normal exception number",
                "M0 Hard fault exception number",
                "M0 reset exception number"
            };

    SOC_IF_ERROR_RETURN(READ_FWLINKSCAN_CTRLr(LS_SHMEM(unit), &ctrl));
    SOC_IF_ERROR_RETURN(READ_FWLINKSCAN_STATUSr(LS_SHMEM(unit), &status));
    SOC_IF_ERROR_RETURN(READ_FWLINKSCAN_SCHANr(LS_SHMEM(unit), &schan));

    cli_out("LS_SHMEM:\n");
    cli_out("    FWLINKSCAN_CTRLr:    PAUSEf=%d\n", FWLINKSCAN_CTRLr_PAUSEf_GET(ctrl));
    cli_out("    FWLINKSCAN_STATUSr:  INITf=%d, RUNf=%d, ERRORf=%d\n",
            FWLINKSCAN_STATUSr_INITf_GET(status), FWLINKSCAN_STATUSr_RUNf_GET(status),
            FWLINKSCAN_STATUSr_ERRORf_GET(status));
    cli_out("    FWLINKSCAN_SCHANr:   NUMf=%d, MAGICf=0x%x\n",
            FWLINKSCAN_SCHANr_NUMf_GET(schan), FWLINKSCAN_SCHANr_MAGICf_GET(schan));

    cpu_id = soc_m0ssq_tcam_read32(unit, 0, CPU_ID_OFFSET);
    dev_id = soc_m0ssq_tcam_read32(unit, 0, DEVID_ID_OFFSET);
    cli_out("\nCPU_ID_OFFSET=0x%x, DEVID_ID_OFFSET=0x%x\n", cpu_id, dev_id);

    cli_out("\nFirmware defined register:\n");
    cli_out("    mbox:  ");
    for (ii=0; ii<1; ii++)
    {
        value[0] = soc_m0ssq_sram_read32(unit, U0_MBOX_BASE_OFFSET + (ii * 8));
        value[1] = soc_m0ssq_sram_read32(unit, U0_MBOX_SIZE_OFFSET + (ii * 8));
        cli_out("base=0x%x, size=0x%x", value[0], value[1]);
    }
    cli_out("\n");
    value[0] = soc_m0ssq_sram_read32(unit, MBOX_INTERRUPT_MODE_OFFSET);
    value[1] = soc_m0ssq_sram_read32(unit, MAX_IPROC_MBOX_PER_CORE_OFFSET);
    cli_out("    MBOX_INTERRUPT_MODE_OFFSET=%d\n", value[0]);
    cli_out("    MAX_IPROC_MBOX_PER_CORE_OFFSET=%d\n", value[1]);
    value[0] = soc_m0ssq_sram_read32(unit, FW_TAG);
    value[1] = soc_m0ssq_sram_read32(unit, FW_SYSTICK_CNT);
    value[2] = soc_m0ssq_sram_read32(unit, FW_BGLOOP_CNT);
    value[3] = soc_m0ssq_sram_read32(unit, FW_DBG_EN);
    cli_out("    FW_TAG=0x%x(START=0x%x, STOP=0x%x)\n", value[0], FW_TAG_START, FW_TAG_STOP);
    cli_out("    FW_DBG_EN=0x%x(FW_DBG_EN_MAGIC=0x%x)\n", value[3], FW_DBG_EN_MAGIC);

    cli_out("\nM0 debug counter:\n");
    cli_out("    %-35s0x%x\n", "FW_SYSTICK_CNT", value[1]);
    cli_out("    %-35s0x%x\n", "FW_BGLOOP_CNT", value[2]);
    for (ii=0; ii<COUNTOF(str_m0_counter_name); ii++)
    {
        value[0] = soc_m0ssq_sram_read32(unit, FW_DBG_COUNTER_OFFSET+ii*4);
        cli_out("    %-35s0x%x\n", str_m0_counter_name[ii], value[0]);
    }
    value[0] = soc_m0ssq_sram_read32(unit, FW_DBG_COUNTER_OFFSET+12*4);
    value[1] = soc_m0ssq_sram_read32(unit, FW_DBG_COUNTER_OFFSET+13*4);
    value[2] = soc_m0ssq_sram_read32(unit, FW_DBG_COUNTER_OFFSET+14*4);
    value[3] = soc_m0ssq_sram_read32(unit, FW_DBG_COUNTER_OFFSET+15*4);
    cli_out("    %-35s0x%x 0x%x 0x%x 0x%x\n", "DNX Debug Flag", value[0], value[1], value[2], value[3]);

    return SOC_E_NONE;
}
#endif

#endif /*BCM_CMICX_SUPPORT*/
#endif /*BCM_ESW_SUPPORT*/
