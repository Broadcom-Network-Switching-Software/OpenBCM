/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
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

#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dnx_data/auto_generated/dnx_data_field.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#endif
#ifdef INCLUDE_KNET

static soc_knet_vectors_t kvect;

#define KNET_OPEN       kvect.kcom.open
#define KNET_CLOSE      kvect.kcom.close
#define KNET_SEND       kvect.kcom.send
#define KNET_RECV       kvect.kcom.recv

/* Size of FTMH LB key in bytes by default */
#define FTMH_LB_KEY_EXT_SIZE 3
/* Size of FTMH Stacking Ext in bytes by default */
#define STACKING_EXT_SIZE 0
/* Size of FTMH LB key in bytes on legacy devices */
#define FTMH_LB_EXT_SIZE_LEGACY 1
/* Size of FTMH Stacking Ext in bytes on legacy devices */
#define FTMH_STACKING_SIZE_LEGACY 2
/* Size of PPH base by default */
#define PPH_BASE_SIZE 12
/* Size of PPH base on legacy devices */
#define PPH_BASE_SIZE_LEGACY 7
/** LIF EXTENSION SIZE - 3 bytes */
#define LIF_EXTENSION_SIZE_1XOUT_LIF 3
/** IN_LIF EXTENSION SIZE - 4 bytes */
#define LIF_EXTENSION_SIZE_IN_LIF_PROFILE 4

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

static int
soc_knet_dune_info_init(int unit, kcom_msg_hw_init_t *kmsg)
{
    if (SOC_WARM_BOOT(unit)) {
        /* No update in the case warmboot */
        return SOC_E_NONE;
    }

#ifdef BCM_DNX_SUPPORT
    /* Information to parse Dune sytem headers */
    if (SOC_IS_DNX(unit))
    {
        int system_headers_mode;

        /* No EP_TO_CPU header */
        kmsg->pkt_hdr_size = 0;

        system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
        /* Set system header mode */
        kmsg->system_headers_mode = system_headers_mode;
        if (system_headers_mode == dnx_data_headers.system_headers.system_headers_mode_jericho_get(unit)) {
            /* Set size of FTMH LB-Key Extension, 0 indicates FTMH LB-Key Extension is not present */
            kmsg->ftmh_lb_key_ext_size = dnx_data_headers.system_headers.jr_mode_ftmh_lb_key_ext_mode_get(unit) ? FTMH_LB_EXT_SIZE_LEGACY : 0;

            /* Set size of FTMH Stacking Extension, 0 indicates FTMH Stacking Extension is not present */
            kmsg->ftmh_stacking_ext_size = dnx_data_headers.system_headers.jr_mode_ftmh_stacking_ext_mode_get(unit) ? FTMH_STACKING_SIZE_LEGACY : 0;

            /* PPH base size */
            kmsg->pph_base_size = PPH_BASE_SIZE_LEGACY;

            /* Fill in UDH size for J1 mode */
            kmsg->udh_size += dnx_data_field.udh.field_class_id_size_0_get(unit);
            kmsg->udh_size += dnx_data_field.udh.field_class_id_size_1_get(unit);
            kmsg->udh_size += dnx_data_field.udh.field_class_id_size_2_get(unit);
            kmsg->udh_size += dnx_data_field.udh.field_class_id_size_3_get(unit);
            kmsg->udh_size /= 8;
            kmsg->udh_enable = (kmsg->udh_size != 0);

            /* No LIF extension */
            memset(kmsg->pph_lif_ext_size, 0, sizeof(kmsg->pph_lif_ext_size));
        }
        else {
            /* Set size of FTMH LB-Key Extension */
            kmsg->ftmh_lb_key_ext_size = FTMH_LB_KEY_EXT_SIZE;
            /* Set size of FTMH Stacking Extension */
            kmsg->ftmh_stacking_ext_size = STACKING_EXT_SIZE;
            /* PPH base size */
            kmsg->pph_base_size = PPH_BASE_SIZE;
            /** Suppose JR2 always enable UDH since at least UDH_BASE exists.*/
            kmsg->udh_enable = TRUE;
            kmsg->udh_length_type[0] = dnx_data_field.udh.type_0_length_get(unit);
            kmsg->udh_length_type[1] = dnx_data_field.udh.type_1_length_get(unit);
            kmsg->udh_length_type[2] = dnx_data_field.udh.type_2_length_get(unit);
            kmsg->udh_length_type[3] = dnx_data_field.udh.type_3_length_get(unit);

            /* 0x0: No LIF extension */
            kmsg->pph_lif_ext_size[0] = 0;
            /* 0x1: 1xOutLIF */
            kmsg->pph_lif_ext_size[1] = LIF_EXTENSION_SIZE_1XOUT_LIF;
            /* 0x2: 2xOutLIF */
            kmsg->pph_lif_ext_size[2] = 2 * LIF_EXTENSION_SIZE_1XOUT_LIF;
            /* 0x3: 3xOutLIF */
            kmsg->pph_lif_ext_size[3] = 3 * LIF_EXTENSION_SIZE_1XOUT_LIF;
            /* 0x4: InLIF + InLIF-Profile*/
            kmsg->pph_lif_ext_size[4] = LIF_EXTENSION_SIZE_IN_LIF_PROFILE;
            /* 0x5-0x7: Reserved */
            kmsg->pph_lif_ext_size[5] = 0;
            kmsg->pph_lif_ext_size[6] = 0;
            kmsg->pph_lif_ext_size[7] = 0;
        }
        kmsg->oamp_punted = soc_property_get(unit, "custom_feature_parse_oamp_punt_packet", 0);
        kmsg->no_skip_udh_check = 0;
    }
#endif
#ifdef BCM_PETRA_SUPPORT
    if (SOC_IS_JERICHO(unit)) {
        char *load_blance_mode = NULL;
        uint8 ftmh_lb_key_ext_en = FALSE;
        uint8 ftmh_stacking_ext_en = FALSE;
        uint32 user_header_ndx;

        kmsg->pkt_hdr_size = 0;

        /* Check if FTMH LB-Key Extension is present */
        load_blance_mode = soc_property_get_str(unit, spn_SYSTEM_FTMH_LOAD_BALANCING_EXT_MODE);
        if (load_blance_mode && (sal_strcmp(load_blance_mode, "DISABLED") != 0)) {
            ftmh_lb_key_ext_en = TRUE;
        }
        /* Set size of FTMH LB-Key Extension */
        kmsg->ftmh_lb_key_ext_size = ftmh_lb_key_ext_en ? FTMH_LB_EXT_SIZE_LEGACY : 0;

        /* Check if FTMH Stacking Extension is present */
        ftmh_stacking_ext_en = soc_property_get(unit, spn_STACKING_ENABLE, 0x0) || soc_property_get(unit, spn_STACKING_EXTENSION_ENABLE, 0x0);
        /* Set size of FTMH Stacking Extension */
        kmsg->ftmh_stacking_ext_size = ftmh_stacking_ext_en ? FTMH_STACKING_SIZE_LEGACY : 0;

        for (user_header_ndx = 0; user_header_ndx < 4; user_header_ndx++) {
            kmsg->udh_size += soc_property_port_get(unit, user_header_ndx, spn_FIELD_CLASS_ID_SIZE, 0);
        }
        kmsg->udh_size /= 8;
        /* 
         * Set OAMP port if parser to packets punted by OAMP is required
         * custom_feature_oam_additional_FTMH_on_error_packets is not considered since it's duplicated with custom_feature_parse_oamp_punt_packet
         * trap_packet_parse_test is not checked since it's rarely used
         */
        kmsg->oamp_punted = soc_property_get(unit, "custom_feature_parse_oamp_punt_packet", 0);

        /* Down MEP DM trapped packets will not have UDH present (even if configured), except for QAX when custom_feature_oam_dm_tod_msb_add_enable=0 */
        if (SOC_IS_QAX(unit)) {
            if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_dm_tod_msb_add_enable", 1) == 0) {
                kmsg->no_skip_udh_check = 1;
            }
        }
    }
#endif

    return SOC_E_NONE;
}

int
soc_knet_hw_init(int unit)
{
    unsigned int len;
    kcom_msg_hw_init_t kmsg;

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

    kmsg.dcb_type = SOC_DCB_TYPE(unit);
    kmsg.dcb_size = SOC_DCB_SIZE(unit);

    if (SOC_DMA_MODE(unit) == SOC_DMA_MODE_CONTINUOUS) {
        /* For now enable Continuous mode for all the channels */
        kmsg.cdma_channels = ~0;
    }

    /* Initiate information for decoding Dune sytem headers */
    soc_knet_dune_info_init(unit, &kmsg);

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
#ifdef INCLUDE_PKTIO
    if (soc_feature(unit, soc_feature_sdklt_pktio)) {
        rv = SOC_E_UNAVAIL;
    }
#endif /* INCLUDE_PKTIO */
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

typedef int _soc_knet_not_empty; /* Make ISO compilers happy. */
