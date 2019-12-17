/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File: 	knet.c
 * Purpose: 	Kernel network control
 */

#include <shared/bsl.h>

#include <soc/drv.h>
#include <soc/dma.h>
#ifdef BCM_CMICX_SUPPORT
#include <soc/cmicx.h>
#endif
#include <soc/knet.h>
#ifdef BCM_PETRA_SUPPORT
#include <soc/dpp/drv.h>
#include <soc/dpp/ARAD/arad_api_mgmt.h>
#endif

#ifdef INCLUDE_KNET

static soc_knet_vectors_t kvect;

#define KNET_OPEN       kvect.kcom.open
#define KNET_CLOSE      kvect.kcom.close
#define KNET_SEND       kvect.kcom.send
#define KNET_RECV       kvect.kcom.recv

static void *knet_chan;

typedef struct knet_cmd_ctrl_s {
    sal_mutex_t cmd_lock;
    sal_mutex_t msg_lock;
    sal_sem_t cmd_done;
    int opcode;
    int seqno;
    int resp_len;
    kcom_msg_t resp_msg;
} knet_cmd_ctrl_t;

static knet_cmd_ctrl_t knet_cmd_ctrl;

/*
 * Simple handler registration (emphasis on simple). 
 */
#define MAX_CALLBACKS 5

static struct callback_ctrl_s {
    soc_knet_rx_cb_t cb; 
    void* cookie; 
} _callback_ctrl[MAX_CALLBACKS]; 

#define KNET_RX_THREAD_STOPPED  0
#define KNET_RX_THREAD_RUNNING  1
#define KNET_RX_THREAD_STOPPING 2

static int knet_rx_thread_run = 0;

int
soc_knet_rx_register(soc_knet_rx_cb_t callback, void *cookie, uint32 flags)
{
    int idx; 

    for (idx = 0; idx < MAX_CALLBACKS; idx++) {
	if (_callback_ctrl[idx].cb == NULL) {
	    _callback_ctrl[idx].cb = callback; 
	    _callback_ctrl[idx].cookie = cookie; 
	    return SOC_E_NONE; 
	}
    }
    return SOC_E_RESOURCE; 
}

int
soc_knet_rx_unregister(soc_knet_rx_cb_t callback)
{
    int idx; 

    for (idx = 0; idx < MAX_CALLBACKS; idx++) {
	if (_callback_ctrl[idx].cb == callback) {
	    _callback_ctrl[idx].cb = NULL; 
	    _callback_ctrl[idx].cookie = NULL; 
	}
    }
    return SOC_E_NONE; 
}

int
soc_knet_handle_cmd_resp(kcom_msg_t *kmsg, unsigned int len, void *cookie)
{
    knet_cmd_ctrl_t *ctrl = (knet_cmd_ctrl_t *)cookie;

    if (kmsg->hdr.type != KCOM_MSG_TYPE_RSP) {
        /* Not handled */
        return 0;
    }
    if (kmsg->hdr.opcode != ctrl->opcode) {
        LOG_CLI((BSL_META("soc_knet_handle_cmd_resp: wrong opcode %d (expected %d)\n"),
                 kmsg->hdr.opcode, ctrl->opcode));
    } else if (kmsg->hdr.seqno != ctrl->seqno) {
        LOG_CLI((BSL_META("soc_knet_handle_cmd_resp: wrong seq no %d (expected %d)\n"),
                 kmsg->hdr.seqno, ctrl->seqno));
    } else {
        if (len > sizeof(ctrl->resp_msg)) {
            LOG_CLI((BSL_META("soc_knet_handle_cmd_resp: resp too long (%d bytes)\n"),
                     len));
        } else {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META("soc_knet_handle_cmd_resp: got %d bytes\n"), len));
            if (kmsg->hdr.status != 0) {
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META("soc_knet_handle_cmd_resp: status %d\n"),
                             kmsg->hdr.status));
            }
            sal_mutex_take(ctrl->msg_lock, sal_mutex_FOREVER);
            sal_memcpy(&ctrl->resp_msg, kmsg, len);
            ctrl->resp_len = len;
            sal_mutex_give(ctrl->msg_lock);
        }
        sal_sem_give(ctrl->cmd_done);
    }
    /* Handled */
    return 1;
}

int
soc_knet_cmd_req(kcom_msg_t *kmsg, unsigned int len, unsigned int buf_size)
{
    int rv;
    knet_cmd_ctrl_t *ctrl = &knet_cmd_ctrl;

    if (knet_rx_thread_run == KNET_RX_THREAD_STOPPED) {
        return SOC_E_UNAVAIL;
    }

    sal_mutex_take(ctrl->cmd_lock, sal_mutex_FOREVER);

    if (kvect.hw_unit_get) {
        /* convert the user unit to hw unit */
        kmsg->hdr.unit = kvect.hw_unit_get(kmsg->hdr.unit, 0);
    }
    kmsg->hdr.type = KCOM_MSG_TYPE_CMD;
    kmsg->hdr.seqno = 0;

    ctrl->opcode = kmsg->hdr.opcode;

    rv = KNET_SEND(KCOM_CHAN_KNET, kmsg, len, buf_size);
    if (rv < 0) {
        LOG_CLI((BSL_META("soc_knet_cmd_req: command failed\n")));
    } else if (rv > 0) {
        /* Synchronous response - no need to wait */
    } else if (sal_sem_take(ctrl->cmd_done, 2000000)) {
        LOG_CLI((BSL_META("soc_knet_cmd_req: command timeout\n")));
        rv = SOC_E_TIMEOUT;
    } else {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META("soc_knet_cmd_req: command OK\n")));
        len = ctrl->resp_len;
        if (len > buf_size) {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META("soc_knet_cmd_req: oversized response "
                                  "(%d bytes, max %d)\n"),
                         len, buf_size));
            len = buf_size;
        }
        sal_mutex_take(ctrl->msg_lock, sal_mutex_FOREVER);
        sal_memcpy(kmsg, &ctrl->resp_msg, len);
        sal_mutex_give(ctrl->msg_lock);
    }

    /* translate the return value for all the valid command response */
    if (rv >= 0) {
        switch (kmsg->hdr.status) {
        case KCOM_E_NONE:
            rv = SOC_E_NONE;
            break;
        case KCOM_E_PARAM:
            rv = SOC_E_PARAM;
            break;
        case KCOM_E_RESOURCE:
            rv = SOC_E_RESOURCE;
            break;
        case KCOM_E_NOT_FOUND:
            rv = SOC_E_NOT_FOUND;
            break;
        default:
            rv = SOC_E_FAIL;
            break;
        }
    }

    sal_mutex_give(ctrl->cmd_lock);

    return rv;
}

int
soc_knet_handle_rx(kcom_msg_t *kmsg, unsigned int len)
{
    int unit;
    int idx;
    int rv = 0;

    /* Check for valid header */
    if (len < sizeof(kcom_msg_hdr_t)) {
        return SOC_E_INTERNAL;
    }

    /* Check for valid unit */
    unit = kmsg->hdr.unit;
    if (!SOC_UNIT_VALID(unit)) {
        return SOC_E_UNIT;
    }

    for (idx = 0; idx < MAX_CALLBACKS; idx++) {
	if (_callback_ctrl[idx].cb != NULL) {
	    rv = _callback_ctrl[idx].cb(kmsg, len, _callback_ctrl[idx].cookie);
            if (rv > 0) {
                /* Message handled */
                return SOC_E_NONE; 
            }
	}
    }
    if (rv == 0) {
        LOG_CLI((BSL_META_U(unit,
                            "soc_knet_handle_rx: unhandled (type=%d, opcode=%d)\n"),
                 kmsg->hdr.type, kmsg->hdr.opcode));
    }
    return SOC_E_NOT_FOUND;
}

void
soc_knet_rx_thread(void *context)
{
    kcom_msg_t kmsg;
    int len;
    char thread_name[SAL_THREAD_NAME_MAX_LEN];
    sal_thread_t    thread;

    thread = sal_thread_self();
    thread_name[0] = 0;
    sal_thread_name(thread, thread_name, sizeof (thread_name));
    while (knet_rx_thread_run == KNET_RX_THREAD_RUNNING) {
        if (kvect.hw_unit_get) {
            /* Pass the hw unit to KNET module to support multiple instance. */
            kmsg.hdr.unit = kvect.hw_unit_get(PTR_TO_INT(context), 0);
        }
        if ((len = KNET_RECV(KCOM_CHAN_KNET, &kmsg, sizeof(kmsg))) < 0) {
            LOG_CLI((BSL_META("knet rx error - thread aborting\n")));
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("AbnormalThreadExit:%s\n"), thread_name)); 
            break;
        }

        if (kvect.hw_unit_get) {
            /* convert the hw unit to user unit */
            kmsg.hdr.unit = kvect.hw_unit_get(kmsg.hdr.unit, 1);
        }
        soc_knet_handle_rx(&kmsg, len);
    }
    knet_rx_thread_run = KNET_RX_THREAD_STOPPED;
}

int
soc_knet_config(soc_knet_vectors_t *vect)
{
    if (vect == NULL) {
        return SOC_E_PARAM;
    }

    kvect = *vect;

    return SOC_E_NONE;
}

int
soc_eth_knet_hw_config(int unit, int type, int chan, uint32 flags, uint32 value)
{
    unsigned int len;
    kcom_msg_eth_hw_config_t kmsg;

    sal_memset(&kmsg, 0, sizeof(kmsg));
    kmsg.hdr.opcode = KCOM_M_ETH_HW_CONFIG;
    kmsg.hdr.unit = unit;
    kmsg.config.type = type;
    kmsg.config.chan = chan;
    kmsg.config.flags = flags;
    kmsg.config.value = value;

    len = sizeof(kmsg);

    return soc_knet_cmd_req((kcom_msg_t *)&kmsg, len, sizeof(kmsg));
}

int
soc_knet_hw_reset(int unit, int channel)
{
    unsigned int len;
    kcom_msg_hw_reset_t kmsg;

    sal_memset(&kmsg, 0, sizeof(kmsg));
    kmsg.hdr.opcode = KCOM_M_HW_RESET;
    kmsg.hdr.unit = unit;
    if (channel >= 0) {
        kmsg.channels = (1 << channel);
    }
    len = sizeof(kmsg);

    return soc_knet_cmd_req((kcom_msg_t *)&kmsg, len, sizeof(kmsg));
}

int
soc_knet_hw_init(int unit)
{
    unsigned int len;
    kcom_msg_hw_init_t kmsg;
#ifdef BCM_PETRA_SUPPORT
    soc_dpp_config_t *dpp = NULL;
#endif

    sal_memset(&kmsg, 0, sizeof(kmsg));
    kmsg.hdr.opcode = KCOM_M_HW_INIT;
    kmsg.hdr.unit = unit;

#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {
        kmsg.cmic_type = 'x';
        kmsg.pkt_hdr_size = soc_cmicx_pktdma_header_size_get(unit);
        if (soc_cm_get_bus_type(unit) & SOC_PCI_DEV_TYPE) {
            kmsg.dma_hi = CMIC_PCIE_SO_OFFSET;
        } else {
            kmsg.dma_hi = 0;
        }
    } else
#endif
    if (soc_feature(unit, soc_feature_cmicm)) {
        kmsg.cmic_type = 'm';
        kmsg.pkt_hdr_size = 0;
    } else {
        kmsg.cmic_type = 'e';
        kmsg.pkt_hdr_size = 0;
    }
#ifdef BCM_PETRA_SUPPORT
    /* make use of pkt_hdr_size to indicate possible Dune headers */
    /* 0x0 - none */
    /* 0x1 - FTMH LB-Key Extension */
    /* 0x2 - FTMH Stacking Extension */
    kmsg.pkt_hdr_size = 0;

    dpp = SOC_DPP_CONFIG(unit);
    if (dpp && (dpp->arad)) {
        if (dpp->arad->init.fabric.ftmh_lb_ext_mode != ARAD_MGMT_FTMH_LB_EXT_MODE_DISABLED) {
            kmsg.pkt_hdr_size |= 0x1;
        }
        if (dpp->arad->init.fabric.ftmh_stacking_ext_mode == 0x1) {
            kmsg.pkt_hdr_size |= 0x2;
        }
    }
#endif
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        /** Update to 0 since there is no ep_to_cpu header */
        kmsg.pkt_hdr_size = 0;
    }
#endif

    kmsg.dcb_type = SOC_DCB_TYPE(unit);
    kmsg.dcb_size = SOC_DCB_SIZE(unit);

    if (SOC_DMA_MODE(unit) == SOC_DMA_MODE_CONTINUOUS) {
        /* For now enable Continuous mode for all the channels */
        kmsg.cdma_channels = ~0;
    }
    len = sizeof(kmsg);

    return soc_knet_cmd_req((kcom_msg_t *)&kmsg, len, sizeof(kmsg));
}

int
soc_knet_reprobe(int unit)
{
    int rv;
    unsigned int len;
    kcom_msg_reprobe_t kmsg;

    sal_memset(&kmsg, 0, sizeof(kmsg));
    kmsg.hdr.opcode = KCOM_M_REPROBE;
    kmsg.hdr.unit = unit;
    len = sizeof(kmsg);

    rv = soc_knet_cmd_req((kcom_msg_t *)&kmsg, len, sizeof(kmsg));

    return rv;
}

int
soc_knet_check_version(int unit)
{
    int rv;
    unsigned int len;
    kcom_msg_version_t kmsg;

    sal_memset(&kmsg, 0, sizeof(kmsg));
    kmsg.hdr.opcode = KCOM_M_VERSION;
    kmsg.hdr.unit = unit;
    kmsg.version = KCOM_VERSION;
    len = sizeof(kmsg);

    rv = soc_knet_cmd_req((kcom_msg_t *)&kmsg, len, sizeof(kmsg));

    if (SOC_SUCCESS(rv)) {
        if (kmsg.version != KCOM_VERSION) {
            rv = SOC_E_INTERNAL;
        }
        if (kmsg.netif_max != KCOM_NETIF_MAX) {
            rv = SOC_E_INTERNAL;
        }
        if (kmsg.filter_max != KCOM_FILTER_MAX) {
            rv = SOC_E_INTERNAL;
        }
    }

    return rv;
}

int
soc_knet_detach(void)
{
    int rv;
    unsigned int len;
    kcom_msg_detach_t kmsg;

    sal_memset(&kmsg, 0, sizeof(kmsg));
    kmsg.hdr.opcode = KCOM_M_DETACH;
    kmsg.hdr.unit = -1;
    len = sizeof(kmsg);

    rv = soc_knet_cmd_req((kcom_msg_t *)&kmsg, len, sizeof(kmsg));

    return rv;
}

int
soc_knet_debug_pkt_set(int unit, int enable)
{
    int rv;
    unsigned int len;
    kcom_msg_dbg_pkt_set_t kmsg;

    sal_memset(&kmsg, 0, sizeof(kmsg));
    kmsg.hdr.opcode = KCOM_M_DBGPKT_SET;
    kmsg.hdr.unit = unit;
    kmsg.enable = enable;
    len = sizeof(kmsg);

    rv = soc_knet_cmd_req((kcom_msg_t *)&kmsg, len, sizeof(kmsg));

    return rv;
}

int
soc_knet_debug_pkt_get(int unit, int *value)
{
    int rv;
    unsigned int len;
    kcom_msg_dbg_pkt_get_t kmsg;

    sal_memset(&kmsg, 0, sizeof(kmsg));
    kmsg.hdr.opcode = KCOM_M_DBGPKT_GET;
    kmsg.hdr.unit = unit;
    len = sizeof(kmsg);

    rv = soc_knet_cmd_req((kcom_msg_t *)&kmsg, len, sizeof(kmsg));
    if (SOC_SUCCESS(rv)) {
        *value = kmsg.value;
    }

    return rv;
}

int
soc_knet_wb_cleanup(int unit)
{
    int rv;
    unsigned int len;
    kcom_msg_wb_cleanup_t kmsg;

    sal_memset(&kmsg, 0, sizeof(kmsg));
    kmsg.hdr.opcode = KCOM_M_WB_CLEANUP;
    kmsg.hdr.unit = unit;
    len = sizeof(kmsg);

    rv = soc_knet_cmd_req((kcom_msg_t *)&kmsg, len, sizeof(kmsg));

    return rv;
}

int
soc_knet_irq_mask_set(int unit, uint32 addr, uint32 mask)
{
    if (kvect.irq_mask_set) {
        return kvect.irq_mask_set(unit, addr, mask);
    }
    soc_pci_write(unit, addr, mask);
    return SOC_E_NONE;
}

int
soc_knet_cleanup(void)
{
    /* Do clean-up only one device left */
    if (soc_ndev > 1) {
        return SOC_E_NONE;
    }

    if (knet_chan != NULL) {
        if (knet_rx_thread_run == KNET_RX_THREAD_RUNNING) {
            int retry = 5000;
            knet_rx_thread_run = KNET_RX_THREAD_STOPPING;
            (void)soc_knet_detach();
            while (knet_rx_thread_run != KNET_RX_THREAD_STOPPED && retry-- > 0) {
                sal_usleep(1000);
            }
            if (retry <= 0) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META("KNET Rx thread will not exit.\n")));
            }
        }
        KNET_CLOSE(KCOM_CHAN_KNET);
    }
    soc_knet_rx_unregister(soc_knet_handle_cmd_resp);
    if (knet_cmd_ctrl.cmd_lock != NULL) {
        sal_mutex_destroy(knet_cmd_ctrl.cmd_lock);
        knet_cmd_ctrl.cmd_lock = NULL;
    }
    if (knet_cmd_ctrl.msg_lock != NULL) {
        sal_mutex_destroy(knet_cmd_ctrl.msg_lock);
        knet_cmd_ctrl.msg_lock = NULL;
    }
    if (knet_cmd_ctrl.cmd_done != NULL) {
        sal_sem_destroy(knet_cmd_ctrl.cmd_done);
        knet_cmd_ctrl.cmd_done = NULL;
    }
    return SOC_E_NONE;
}

int
soc_knet_init(int unit)
{
    int rv;
    sal_thread_t rxthr;
    knet_cmd_ctrl_t *ctrl = &knet_cmd_ctrl;
    kcom_msg_string_t kmsg;

    if (KNET_OPEN == NULL) {
        /* KCOM vectors not assigned */
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "soc_knet_init: No KCOM vectors\n")));
        return SOC_E_CONFIG;
    }

    /* Check support for KNET feature */
    rv = SOC_E_UNAVAIL;
    if (SOC_IS_XGS3_SWITCH(unit)) {
        /* XGS3-style DCBs are supported */
        rv = SOC_E_NONE;
    }
#ifdef BCM_PETRA_SUPPORT
    if (SOC_IS_ARAD(unit)) {
        /* DNX devices are supported */
        rv = SOC_E_NONE;
    }
#endif
#ifdef BCM_DNX_SUPPORT
        if (SOC_IS_DNX(unit)) {
            /* DNX devices are supported */
            rv = SOC_E_NONE;
        }
#endif
    if (rv != SOC_E_NONE) {
        return rv;
    }

    if (knet_rx_thread_run != KNET_RX_THREAD_STOPPED) {
        /* Already initialized, only need to reprobe the device */
        rv = soc_knet_reprobe(unit);
        if (SOC_FAILURE(rv)) {
            LOG_CLI((BSL_META_U(unit,
                                "knet reprobe failed\n")));
            return SOC_E_FAIL;
        }
        return SOC_E_NONE;
    }

    ctrl->cmd_lock = sal_mutex_create("KNET CMD");
    if (ctrl->cmd_lock == NULL) {
        soc_knet_cleanup();
        return SOC_E_RESOURCE;
    }
    ctrl->msg_lock = sal_mutex_create("KNET MSG");
    if (ctrl->msg_lock == NULL) {
        soc_knet_cleanup();
        return SOC_E_RESOURCE;
    }
    ctrl->cmd_done = sal_sem_create("KNET CMD", 1, 0);
    if (ctrl->cmd_done == NULL) {
        soc_knet_cleanup();
        return SOC_E_RESOURCE;
    }

    /* Check if kernel messaging is initialized */
    knet_chan = KNET_OPEN(KCOM_CHAN_KNET);
    if (knet_chan == NULL) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "knet open failed\n")));
        soc_knet_cleanup();
        return SOC_E_FAIL;
    }

    /* Try sending string event to see if kernel module is present */
    sal_memset(&kmsg, 0, sizeof(kmsg));
    kmsg.hdr.opcode = KCOM_M_STRING;
    kmsg.hdr.type = KCOM_MSG_TYPE_EVT;
    sal_strcpy(kmsg.val, "soc_knet_init");
    rv = KNET_SEND(KCOM_CHAN_KNET, &kmsg, sizeof(kmsg), sizeof(kmsg));
    if (rv < 0) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "knet init failed\n")));
        soc_knet_cleanup();
        return SOC_E_FAIL;
    }

    /* Register command response handler */
    soc_knet_rx_register(soc_knet_handle_cmd_resp, ctrl, 0);

    /* Start message handler */
    knet_rx_thread_run = KNET_RX_THREAD_RUNNING;
    rxthr = sal_thread_create("SOC KNET RX", 0,
                              soc_property_get(unit, spn_SOC_KNET_RX_THREAD_PRI, 0),
                              soc_knet_rx_thread, INT_TO_PTR(unit));
    if (rxthr == NULL) {
        LOG_CLI((BSL_META_U(unit,
                            "knet rx thread create failed\n")));
        soc_knet_cleanup();
        return SOC_E_FAIL;
    }

    rv = soc_knet_check_version(unit);
    if (SOC_FAILURE(rv)) {
        LOG_CLI((BSL_META_U(unit,
                            "knet version check failed\n")));
        soc_knet_cleanup();
        return SOC_E_FAIL;
    }

    rv = soc_knet_reprobe(unit);
    if (SOC_FAILURE(rv)) {
        LOG_CLI((BSL_META_U(unit,
                            "knet reprobe failed\n")));
        soc_knet_cleanup();
        return SOC_E_FAIL;
    }

    return SOC_E_NONE;
}

#endif /* INCLUDE_KNET */

int _soc_knet_not_empty;
