/*! \file bcmcth_mon_int_drv.c
 *
 * Inband telemetry driver APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <sal/sal_libc.h>
#include <shr/shr_util_pack.h>
#include <shr/shr_ha.h>

#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmcth/bcmcth_mon_util.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmbd/bcmbd_mcs.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmpkt/flexhdr/bcmpkt_rxpmd_flex.h>
#include <bcmpkt/bcmpkt_dev.h>
#include <bcmpkt/bcmpkt_rxpmd.h>
#include <bcmissu/issu_api.h>

#include <bcmcth/bcmcth_mon_drv.h>
#include <bcmcth/bcmcth_mon_rx_flex.h>
#include <bcmcth/bcmcth_mon_int_drv.h>
#include <bcmcth/bcmcth_mon_int_pack.h>
#include <bcmbd/mcs_shared/mcs_msg_common.h>
#include <bcmbd/mcs_shared/mcs_ifa_common.h>
#include <bcmcth/generated/mon_int_ha.h>


/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_MON

/* Get the upper 32b of a 64b */
#define INT_64_HI(_u64_) (((_u64_) >> 32) & 0xFFFFFFFF)

/* Get the lower 32b of a 64b */
#define INT_64_LO(_u64_) ((_u64_) & 0xFFFFFFFF)

/* Join 2 32b into 1 64b */
#define INT_64_GET(_hi_, _lo_) ((((uint64_t)(_hi_)) << 32) | (_lo_))

/* uC messaging timeout in usecs. */
#define IFA_MAX_UC_MSG_TIMEOUT_USECS (5 * 1000 * 1000)

/* Current messaging version of the SDK */
#define INT_IFA_SDK_MSG_VERSION 0

/* Mininum export packet length. */
#define INT_IFA_MIN_EXPORT_PKT_LENGTH 128

/* Invalid collector Id, implies collector is being deleted. */
#define INT_INVALID_COLLECTOR_ID 0XFFFFFFFF

/* Invalid export profile id, implies export profile is being deleted. */
#define INT_INVALID_EXPORT_PROFILE_ID 0XFFFFFFFF

/* IPFIX set ID when exporting the template set. */
#define IFA_IPFIX_SET_ID_TEMPLATE_SET 2

/* The enum represents the MON_INBAND_TELEMETRY_CONTROL LT operational states.
 * The enum is ordered based on the operational states hierarchy.
 */
typedef enum int_control_oper_state_s {
    /* App is not initialized. */
   OPER_STATE_NOT_INITIALIZED = 0,

   /* Collector Id specified, but the collector is not created. */
   OPER_STATE_COLLECTOR_NOT_EXISTS = 1,

   /* Export profile Id specified, but the export profile is not created. */
   OPER_STATE_EXPORT_PROFILE_NOT_EXISTS = 2,

   /* Wire format specified in the export profile is not supported. */
   OPER_STATE_EXPORT_PROFILE_WIRE_FORMAT_NOT_SUPPORTED = 3,

   /* Max packet length in the export profile is invalid. */
   OPER_STATE_EXPORT_PROFILE_INVALID_MAX_PKT_LENGTH = 4,
} int_control_oper_state_t;

/* Mapping of internal oper states to external. Ordered based on the
 * int_control_oper_state_t enum.
 */
static bcmltd_common_mon_inband_telemetry_control_state_t_t int_oper_states[] = {
    BCMLTD_COMMON_MON_INBAND_TELEMETRY_CONTROL_STATE_T_T_APP_NOT_INITIALIZED,
    BCMLTD_COMMON_MON_INBAND_TELEMETRY_CONTROL_STATE_T_T_COLLECTOR_NOT_EXISTS,
    BCMLTD_COMMON_MON_INBAND_TELEMETRY_CONTROL_STATE_T_T_EXPORT_PROFILE_NOT_EXISTS,
    BCMLTD_COMMON_MON_INBAND_TELEMETRY_CONTROL_STATE_T_T_EXPORT_PROFILE_WIRE_FORMAT_NOT_SUPPORTED,
    BCMLTD_COMMON_MON_INBAND_TELEMETRY_CONTROL_STATE_T_T_EXPORT_PROFILE_INVALID_MAX_PKT_LENGTH,
};

/* uC info data structure */
typedef struct int_info_s {
    /* Persistent info to be recovered from HA memory */
    bcmcth_mon_int_ha_t *ha;

    /* DMA buffer for holding send/receive messages */
    uint8_t *dma_buffer;

    /* Physical address of the DMA buffer */
    uint64_t dma_buffer_p_addr;
} int_info_t;

static int_info_t *g_int_info[BCMDRD_CONFIG_MAX_UNITS] = {0};

/*******************************************************************************
 * IFA EApp messaging private functions.
 */
static int
int_ifa_uc_rxpmd_flex_fids_set(int unit);

static int
int_ifa_uc_fifo_format_set(int unit, uint32_t trans_id);

static int
int_ifa_uc_md_format_set(int unit);

static int
int_ifa_convert_uc_error(uint32_t uc_rv)
{
    int rv = SHR_E_NONE;

    switch (uc_rv) {
        case MCS_IFA_UC_E_NONE:
            rv = SHR_E_NONE;
            break;

        case MCS_IFA_UC_E_INTERNAL:
            rv = SHR_E_INTERNAL;
            break;

        case MCS_IFA_UC_E_MEMORY:
            rv = SHR_E_MEMORY;
            break;

        case MCS_IFA_UC_E_UNIT:
            rv = SHR_E_UNIT;
            break;

        case MCS_IFA_UC_E_PARAM:
            rv = SHR_E_PARAM;
            break;

        case MCS_IFA_UC_E_EMPTY:
            rv = SHR_E_EMPTY;
            break;

        case MCS_IFA_UC_E_FULL:
            rv = SHR_E_FULL;
            break;

        case MCS_IFA_UC_E_NOT_FOUND:
            rv = SHR_E_NOT_FOUND;
            break;

        case MCS_IFA_UC_E_EXISTS:
            rv = SHR_E_EXISTS;
            break;

        case MCS_IFA_UC_E_TIMEOUT:
            rv = SHR_E_TIMEOUT;
            break;

        case MCS_IFA_UC_E_BUSY:
            rv = SHR_E_BUSY;
            break;

        case MCS_IFA_UC_E_FAIL:
            rv = SHR_E_FAIL;
            break;

        case MCS_IFA_UC_E_DISABLED:
            rv = SHR_E_DISABLED;
            break;

        case MCS_IFA_UC_E_BADID:
            rv = SHR_E_BADID;
            break;

        case MCS_IFA_UC_E_RESOURCE:
            rv = SHR_E_RESOURCE;
            break;

        case MCS_IFA_UC_E_CONFIG:
            rv = SHR_E_CONFIG;
            break;

        case MCS_IFA_UC_E_UNAVAIL:
            rv = SHR_E_UNAVAIL;
            break;

        case MCS_IFA_UC_E_INIT:
            rv = SHR_E_INIT;
            break;

        case MCS_IFA_UC_E_PORT:
            rv = SHR_E_PORT;
            break;

        default:
            rv = SHR_E_INTERNAL;
            break;
    }

    return rv;
}

static int
int_ifa_uc_msg_send_receive(int unit, uint8_t s_subclass,
                            uint16_t s_len, uint64_t s_data,
                            uint8_t r_subclass, uint16_t *r_len)
{
    int_info_t *int_info = g_int_info[unit];
    mcs_msg_data_t send, reply;

    SHR_FUNC_ENTER(unit);

    sal_memset(&send, 0, sizeof(send));
    sal_memset(&reply, 0, sizeof(reply));

    MCS_MSG_MCLASS_SET(send, MCS_MSG_CLASS_IFA);
    MCS_MSG_SUBCLASS_SET(send, s_subclass);
    MCS_MSG_LEN_SET(send, s_len);

    /*
     * Set 'data' to DMA buffer address if a DMA operation is
     * required for send or receive.
     */
    if (MCS_MSG_DMA_MSG(s_subclass) || MCS_MSG_DMA_MSG(r_subclass)) {
        MCS_MSG_DATA_SET(send, INT_64_LO(int_info->dma_buffer_p_addr));
        MCS_MSG_DATA1_SET(send, INT_64_HI(int_info->dma_buffer_p_addr));
    } else {
        MCS_MSG_DATA_SET(send, INT_64_LO(s_data));
        MCS_MSG_DATA1_SET(send, INT_64_HI(s_data));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmbd_mcs_msg_send_receive(unit, int_info->ha->uc_num,
                                    &send, &reply,
                                    IFA_MAX_UC_MSG_TIMEOUT_USECS));

    /* Convert IFA uC error code to SHR_E_XXX format. Return error if the
     * converted value is not SHR_E_NONE.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (int_ifa_convert_uc_error(MCS_MSG_DATA_GET(reply)));

    *r_len = MCS_MSG_LEN_GET(reply);

    /* Return error if we got the wrong class/subclass. */
    if ((MCS_MSG_MCLASS_GET(reply)   != MCS_MSG_CLASS_IFA) ||
        (MCS_MSG_SUBCLASS_GET(reply) != r_subclass)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

 exit:
    SHR_FUNC_EXIT();
}

static int
int_ifa_uc_appl_init(int unit, int uc)
{
    mcs_msg_data_t send, rcv;
    bcmbd_ukernel_info_t ukernel_info;

    SHR_FUNC_ENTER(unit);

    sal_memset(&send, 0, sizeof(send));
    sal_memset(&rcv, 0, sizeof(rcv));

    MCS_MSG_MCLASS_SET(send, MCS_MSG_CLASS_SYSTEM);
    MCS_MSG_SUBCLASS_SET(send, MCS_MSG_SUBCLASS_SYSTEM_APPL_INIT);
    MCS_MSG_LEN_SET(send, MCS_MSG_CLASS_IFA);
    MCS_MSG_DATA_SET(send, INT_IFA_SDK_MSG_VERSION);


    SHR_IF_ERR_VERBOSE_EXIT(bcmbd_mcs_uc_status(unit, uc, &ukernel_info));
    if (sal_strcmp(ukernel_info.status,"OK") != 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmbd_mcs_msg_send(unit, uc, &send, IFA_MAX_UC_MSG_TIMEOUT_USECS));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmbd_mcs_msg_receive(unit, uc, MCS_MSG_CLASS_IFA, &rcv,
                               IFA_MAX_UC_MSG_TIMEOUT_USECS));

    if (MCS_MSG_LEN_GET(rcv) != 0) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

 exit:
    SHR_FUNC_EXIT();
}

static int
int_ifa_uc_shutdown(int unit)
{
    int_info_t *int_info = g_int_info[unit];
    bcmbd_ukernel_info_t ukernel_info;
    uint16_t reply_len;

    SHR_FUNC_ENTER(unit);

    if (int_info->ha->init == false) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmbd_mcs_uc_status(unit, int_info->ha->uc_num, &ukernel_info));

    if (sal_strcmp(ukernel_info.status, "OK") == 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (int_ifa_uc_msg_send_receive(unit,
                                         MCS_MSG_SUBCLASS_IFA_SHUTDOWN,
                                         0, 0,
                                         MCS_MSG_SUBCLASS_IFA_SHUTDOWN_REPLY,
                                         &reply_len));

        if (reply_len != 0) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }
    int_info->ha->init = false;
    int_info->ha->run = false;
    int_info->ha->collector = false;

 exit:
    SHR_FUNC_EXIT();
}

static int
int_ifa_uc_init(int unit, uint32_t trans_id,
                bcmcth_mon_int_control_entry_t *entry)
{
    int_info_t *int_info = g_int_info[unit];
    mcs_ifa_msg_ctrl_init_t msg;
    bcmcth_mon_int_drv_t *int_drv = NULL;
    uint8_t *buffer_ptr = NULL;
    uint16_t buffer_len = 0, reply_len = 0;
    uint64_t ts;
    int width, depth;
    int uc, num_uc;
    int rv;

    SHR_FUNC_ENTER(unit);

    if (bcmdrd_feature_is_sim(unit) == true) {
        /* Don't attempt initialization in SIM, simply return without error.
         * Since the init flag is not set, subsequent messages will not be sent
         * to app.
         */
        SHR_EXIT();
    }

    /* Shutdown the application if it is already loaded */
    if (int_info->ha->init) {
        SHR_IF_ERR_CONT
            (int_ifa_uc_shutdown(unit));
        int_info->ha->init = false;
        int_info->ha->run = false;
    }

    /* Init the app, determine which uC is running the app by chooosing
     * the first uC that returns sucessfully to the APPL_INIT message.
     */
    num_uc = bcmbd_mcs_num_uc(unit);
    for (uc = 0; uc < num_uc ; uc++) {
        rv = int_ifa_uc_appl_init(unit, uc);
        if (rv == SHR_E_NONE) {
            break;
        }
    }
    if (uc == num_uc) {
        /* None of the uC's responded to the APPL_INIT message. Return without
         * setting the init flag.
         */
        SHR_EXIT();
    }

    int_info->ha->uc_num = uc;

    /* Set control message data */
    sal_memset(&msg, 0, sizeof(msg));
    msg.max_export_pkt_length = entry->max_export_pkt_length;
    msg.max_rx_pkt_length = entry->max_rx_pkt_length;
    msg.device_id = entry->device_id;

    /* Read the FIFO depth and width from H/w and send it to the app. */
    int_drv = bcmcth_mon_int_drv_get(unit);
    if (int_drv && int_drv->fifo_get) {

        SHR_IF_ERR_VERBOSE_EXIT(int_drv->fifo_get(unit,
                                                  MON_INBAND_TELEMETRY_CONTROLt,
                                                  trans_id,
                                                  &width, &depth));
        msg.fifo_depth = depth;
        msg.fifo_width = width;
    }

    /* Get approximate current time in milliseconds and pass it to the app. */
    ts = sal_time() * 1000;
    msg.init_time_lo = INT_64_LO(ts);
    msg.init_time_hi = INT_64_HI(ts);


    buffer_ptr = bcmcth_mon_int_ifa_msg_ctrl_init_pack(int_info->dma_buffer,
                                                       &msg);
    buffer_len = buffer_ptr - int_info->dma_buffer;

    SHR_IF_ERR_VERBOSE_EXIT
        (int_ifa_uc_msg_send_receive(unit,
                                     MCS_MSG_SUBCLASS_IFA_INIT,
                                     buffer_len, 0,
                                     MCS_MSG_SUBCLASS_IFA_INIT_REPLY,
                                     &reply_len));
    if (reply_len != 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    int_info->ha->init = true;


    /* Set the format of the flex fields in the Rx PMD. */
    SHR_IF_ERR_VERBOSE_EXIT(int_ifa_uc_rxpmd_flex_fids_set(unit));

    /* Send the FIFO format to the app */
    SHR_IF_ERR_VERBOSE_EXIT(int_ifa_uc_fifo_format_set(unit, trans_id));

    /* Send the MD format to the app */
    SHR_IF_ERR_VERBOSE_EXIT(int_ifa_uc_md_format_set(unit));

    int_info->ha->run = true;

 exit:
    SHR_FUNC_EXIT();
}

/* From a bitmap get the start offset and width in bits which are set to 1. In
 * case multiple chunks are set, the offset and width of the first chunk is
 * returned.
 */
static void
bmp_offset_width_get(uint32_t *bmp, uint16_t num_bits, int *offset, int *width)
{
    int wp, bp;
    int i;

    *offset = -1;
    *width  = 0;

    for (i = 0; i < num_bits; i++) {
        wp = i / 32;
        bp = i % 32;

        if ((bmp[wp] & (1 << bp)) != 0) {
            if (*offset == -1) {
                *offset = i;
            }
            (*width)++;
        } else {
            if (*offset != -1) {
                break;
            }
        }
    }
}

static int
int_ifa_uc_rxpmd_flex_fids_set(int unit)
{
    int_info_t *int_info = g_int_info[unit];
    bcmcth_mon_int_drv_t *int_drv = NULL;
    int fids[MCS_IFA_RXPMD_FLEX_FIELD_COUNT];
    int i, num_profiles;
    uint32_t rxpmd_flex_ref[BCMPKT_RXPMD_SIZE_WORDS];
    uint32_t rxpmd_flex_mask[BCMPKT_RXPMD_SIZE_WORDS];
    bcmlrd_variant_t variant;
    uint32_t profile;
    uint32_t val;
    int offset, width;
    mcs_ifa_msg_ctrl_rxpmd_flex_format_t msg;
    uint8_t *buffer_ptr = NULL;
    uint16_t buffer_len = 0, reply_len = 0;
    SHR_BITDCL *profile_bmp = NULL;
    int rv;
    bcmdrd_dev_type_t dev_type;
    uint32_t flexdata_len_words;
    uint32_t flexdata_len_bytes;
    uint32_t *flexdata_addr;

    SHR_FUNC_ENTER(unit);

    int_drv = bcmcth_mon_int_drv_get(unit);
    if ((int_drv == NULL) || (int_drv->rxpmd_flex_fids_get == NULL)) {
        SHR_EXIT();
    }
    sal_memset(&msg, 0, sizeof(msg));

    SHR_IF_ERR_VERBOSE_EXIT
        (int_drv->rxpmd_flex_fids_get(unit, MCS_IFA_RXPMD_MAX_PROFILES,
                                      &(msg.num_fields), fids, msg.fields));


    variant = bcmlrd_variant_get(unit);
    if (variant == BCMLRD_VARIANT_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT(bcmpkt_dev_type_get(unit, &dev_type));
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmpkt_rxpmd_flexdata_get(dev_type, rxpmd_flex_ref,
                                  &flexdata_addr, &flexdata_len_words));
    flexdata_len_bytes = flexdata_len_words * 4;

    sal_memset(rxpmd_flex_ref, 0xFF, BCMPKT_RXPMD_SIZE_BYTES);

    SHR_ALLOC(profile_bmp,
              SHR_BITALLOCSIZE(MCS_IFA_RXPMD_MAX_PROFILES),
              "bcmcthMonIntRxPmdProfilesBmp");
    sal_memset(profile_bmp, 0, SHR_BITALLOCSIZE(MCS_IFA_RXPMD_MAX_PROFILES));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_rx_flex_profiles_get(variant, profile_bmp));


    SHR_BITCOUNT_RANGE(profile_bmp, num_profiles, 0,
                       MCS_IFA_RXPMD_MAX_PROFILES);

    SHR_BIT_ITER(profile_bmp, MCS_IFA_RXPMD_MAX_PROFILES, profile) {
        msg.profiles[msg.num_profiles] = profile;
        for (i = 0; i < msg.num_fields; i++) {
            sal_memset(rxpmd_flex_mask, 0, BCMPKT_RXPMD_SIZE_BYTES);

            /* Get mask value */
            rv = bcmpkt_rxpmd_flex_field_get(variant, rxpmd_flex_ref, profile,
                                             fids[i], &val);
            if (rv != SHR_E_NONE) {
                continue;
            }

            /* Set field mask */
            rv = bcmpkt_rxpmd_flex_field_set(variant, rxpmd_flex_mask, profile,
                                             fids[i], val);
            if (rv != SHR_E_NONE) {
                continue;
            }

            bmp_offset_width_get(rxpmd_flex_mask, flexdata_len_bytes * 8,
                                 &offset, &width);

            msg.offsets[msg.num_profiles][i] = offset;
            msg.widths[msg.num_profiles][i] = width;
        }
        msg.num_profiles++;
    }


    buffer_ptr =
        bcmcth_mon_int_ifa_msg_ctrl_rxpmd_flex_format_set_pack(int_info->dma_buffer,
                                                               &msg);
    buffer_len = buffer_ptr - int_info->dma_buffer;

    SHR_IF_ERR_VERBOSE_EXIT
        (int_ifa_uc_msg_send_receive(unit,
                                     MCS_MSG_SUBCLASS_IFA_RXPMD_FLEX_FORMAT_SET,
                                     buffer_len, 0,
                                     MCS_MSG_SUBCLASS_IFA_RXPMD_FLEX_FORMAT_SET_REPLY,
                                     &reply_len));

    if (reply_len != 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

 exit:
    SHR_FREE(profile_bmp);
    SHR_FUNC_EXIT();
}

static int
int_ifa_uc_fifo_format_set(int unit, uint32_t trans_id)
{
    int_info_t *int_info = g_int_info[unit];
    mcs_ifa_msg_ctrl_fifo_format_set_t msg;
    uint8_t *buffer_ptr = NULL;
    uint16_t buffer_len = 0, reply_len = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&msg, 0, sizeof(msg));

    /* Populate the FIFO elements */
    msg.elements[0] = MCS_IFA_FIFO_ELEMENT_MMU_INT_STAT1_15_0;
    msg.elements[1] = MCS_IFA_FIFO_ELEMENT_MMU_INT_STAT1_18_16;
    msg.elements[2] = MCS_IFA_FIFO_ELEMENT_MMU_INT_STAT0_15_0;
    msg.elements[3] = MCS_IFA_FIFO_ELEMENT_MMU_INT_STAT0_18_16;
    msg.elements[4] = MCS_IFA_FIFO_ELEMENT_IFA_ATTR_QUE_TX_BCOUNT_15_0;
    msg.elements[5] = MCS_IFA_FIFO_ELEMENT_IFA_ATTR_QUE_TX_BCOUNT_31_16;
    msg.elements[6] = MCS_IFA_FIFO_ELEMENT_RESIDENCE_TIME_NSEC_15_0;
    msg.elements[7] = MCS_IFA_FIFO_ELEMENT_RESIDENCE_TIME_NSEC_31_16;
    msg.elements[8] = MCS_IFA_FIFO_ELEMENT_ING_PTP_NSEC_15_0;
    msg.elements[9] = MCS_IFA_FIFO_ELEMENT_ING_PTP_NSEC_31_16;
    msg.elements[10] = MCS_IFA_FIFO_ELEMENT_SYS_SOURCE;
    msg.elements[11] = MCS_IFA_FIFO_ELEMENT_GSH_SYS_DESTINATION;
    msg.elements[12] = MCS_IFA_FIFO_ELEMENT_ING_PTP_SEC_15_0;
    msg.elements[13] = MCS_IFA_FIFO_ELEMENT_ING_PTP_SEC_31_16;
    msg.elements[14] = MCS_IFA_FIFO_ELEMENT_CCBE_CTRL_2;
    msg.elements[15] = MCS_IFA_FIFO_ELEMENT_CCBE_CTRL_1;
    msg.elements[16] = MCS_IFA_FIFO_ELEMENT_INT_CONGESTION;
    msg.elements[17] = MCS_IFA_FIFO_ELEMENT_IFA_ATTR_PORT_SPEED;
    msg.elements[18] = MCS_IFA_FIFO_ELEMENT_FWD_HDR_TTL;

    msg.num_elements = 19;


    buffer_ptr = bcmcth_mon_int_ifa_msg_ctrl_fifo_format_set_pack(int_info->dma_buffer,
                                                                &msg);
    buffer_len = buffer_ptr - int_info->dma_buffer;

    SHR_IF_ERR_VERBOSE_EXIT
        (int_ifa_uc_msg_send_receive(unit,
                                     MCS_MSG_SUBCLASS_IFA_FIFO_FORMAT_SET,
                                     buffer_len, 0,
                                     MCS_MSG_SUBCLASS_IFA_FIFO_FORMAT_SET_REPLY,
                                     &reply_len));

    if (reply_len != 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

 exit:
    SHR_FUNC_EXIT();
}

/*
 * Set the information regarding a metadata element into the
 * mcs_ifa_msg_ctrl_md_format_set_t message. This function expects the msg to be
 * initialized prior to calling it the first time.
 */
static int
int_ifa_uc_md_element_msg_set(int unit,
                              mcs_ifa_md_elements_t md,
                              mcs_ifa_msg_ctrl_md_format_set_t *msg)
{
    int idx = msg->num_elements;

    SHR_FUNC_ENTER(unit);

    /*
     * Initializations. Setting msg->num_fifo_parts to 0 indicates that this
     * particular md element cannot be recovered from the FIFO, it must be
     * recovered by some other means - configuration, pkt copied to Cpu etc..
     */
    msg->elements[idx] = md;
    msg->num_fifo_parts[idx] = 0;

    switch (md) {
        case MCS_IFA_MD_ELEMENT_LNS_DEVICE_ID:
            msg->element_widths[idx] = 24;
            break;

        case MCS_IFA_MD_ELEMENT_IP_TTL:
            msg->element_widths[idx] = 8;
            msg->num_fifo_parts[idx] = 1;

            msg->fifo_parts[idx][0] = MCS_IFA_FIFO_ELEMENT_FWD_HDR_TTL;
            msg->fifo_part_offsets[idx][0] = 0;
            msg->fifo_part_widths[idx][0]  = 8;
            break;

        case MCS_IFA_MD_ELEMENT_EGRESS_PORT_SPEED:
            msg->element_widths[idx] = 4;
            msg->num_fifo_parts[idx] = 1;

            msg->fifo_parts[idx][0] = MCS_IFA_FIFO_ELEMENT_IFA_ATTR_PORT_SPEED;
            msg->fifo_part_offsets[idx][0] = 0;
            msg->fifo_part_widths[idx][0]  = 4;
            break;

        case MCS_IFA_MD_ELEMENT_CONGESTION:
            msg->element_widths[idx] = 2;
            msg->num_fifo_parts[idx] = 1;

            msg->fifo_parts[idx][0] = MCS_IFA_FIFO_ELEMENT_INT_CONGESTION;
            msg->fifo_part_offsets[idx][0] = 0;
            msg->fifo_part_widths[idx][0]  = 2;
            break;

        case MCS_IFA_MD_ELEMENT_QUEUE_ID:
            msg->element_widths[idx] = 6;
            msg->num_fifo_parts[idx] = 2;

            msg->fifo_parts[idx][0] = MCS_IFA_FIFO_ELEMENT_CCBE_CTRL_2;
            msg->fifo_part_offsets[idx][0] = 0;
            msg->fifo_part_widths[idx][0]  = 4;

            msg->fifo_parts[idx][1] = MCS_IFA_FIFO_ELEMENT_CCBE_CTRL_1;
            msg->fifo_part_offsets[idx][1] = 2;
            msg->fifo_part_widths[idx][1]  = 2;
            break;

        case MCS_IFA_MD_ELEMENT_RX_TIMESTAMP_SEC:
            msg->element_widths[idx] = 20;
            msg->num_fifo_parts[idx] = 2;

            msg->fifo_parts[idx][0] = MCS_IFA_FIFO_ELEMENT_ING_PTP_SEC_15_0;
            msg->fifo_part_offsets[idx][0] = 0;
            msg->fifo_part_widths[idx][0]  = 16;

            msg->fifo_parts[idx][1] = MCS_IFA_FIFO_ELEMENT_ING_PTP_SEC_31_16;
            msg->fifo_part_offsets[idx][1] = 0;
            msg->fifo_part_widths[idx][1]  = 4;
            break;

        case MCS_IFA_MD_ELEMENT_EGRESS_SYSTEM_PORT:
            msg->element_widths[idx] = 16;
            msg->num_fifo_parts[idx] = 1;

            msg->fifo_parts[idx][0] = MCS_IFA_FIFO_ELEMENT_GSH_SYS_DESTINATION;
            msg->fifo_part_offsets[idx][0] = 0;
            msg->fifo_part_widths[idx][0]  = 16;
            break;

        case MCS_IFA_MD_ELEMENT_INGRESS_SYSTEM_PORT:
            msg->element_widths[idx] = 16;
            msg->num_fifo_parts[idx] = 1;

            msg->fifo_parts[idx][0] = MCS_IFA_FIFO_ELEMENT_SYS_SOURCE;
            msg->fifo_part_offsets[idx][0] = 0;
            msg->fifo_part_widths[idx][0]  = 16;
            break;

        case MCS_IFA_MD_ELEMENT_RX_TIMESTAMP_NANO_SEC:
            msg->element_widths[idx] = 32;
            msg->num_fifo_parts[idx] = 2;

            msg->fifo_parts[idx][0] = MCS_IFA_FIFO_ELEMENT_ING_PTP_NSEC_15_0;
            msg->fifo_part_offsets[idx][0] = 0;
            msg->fifo_part_widths[idx][0]  = 16;

            msg->fifo_parts[idx][1] = MCS_IFA_FIFO_ELEMENT_ING_PTP_NSEC_31_16;
            msg->fifo_part_offsets[idx][1] = 0;
            msg->fifo_part_widths[idx][1]  = 16;
            break;

        case MCS_IFA_MD_ELEMENT_RESIDENCE_TIME_NANO_SEC:
            msg->element_widths[idx] = 32;
            msg->num_fifo_parts[idx] = 2;

            msg->fifo_parts[idx][0] = MCS_IFA_FIFO_ELEMENT_RESIDENCE_TIME_NSEC_15_0;
            msg->fifo_part_offsets[idx][0] = 0;
            msg->fifo_part_widths[idx][0]  = 16;

            msg->fifo_parts[idx][1] = MCS_IFA_FIFO_ELEMENT_RESIDENCE_TIME_NSEC_31_16;
            msg->fifo_part_offsets[idx][1] = 0;
            msg->fifo_part_widths[idx][1]  = 16;
            break;

        case MCS_IFA_MD_ELEMENT_OPAQUE_DATA_1:
            msg->element_widths[idx] = 32;
            msg->num_fifo_parts[idx] = 2;

            msg->fifo_parts[idx][0] = MCS_IFA_FIFO_ELEMENT_IFA_ATTR_QUE_TX_BCOUNT_15_0;
            msg->fifo_part_offsets[idx][0] = 0;
            msg->fifo_part_widths[idx][0]  = 16;

            msg->fifo_parts[idx][1] = MCS_IFA_FIFO_ELEMENT_IFA_ATTR_QUE_TX_BCOUNT_31_16;
            msg->fifo_part_offsets[idx][1] = 0;
            msg->fifo_part_widths[idx][1]  = 16;
            break;

        case MCS_IFA_MD_ELEMENT_OPAQUE_DATA_2:
            msg->element_widths[idx] = 32;
            msg->num_fifo_parts[idx] = 2;

            msg->fifo_parts[idx][0] = MCS_IFA_FIFO_ELEMENT_MMU_INT_STAT0_15_0;
            msg->fifo_part_offsets[idx][0] = 0;
            msg->fifo_part_widths[idx][0]  = 16;

            msg->fifo_parts[idx][1] = MCS_IFA_FIFO_ELEMENT_MMU_INT_STAT0_18_16;
            msg->fifo_part_offsets[idx][1] = 0;
            msg->fifo_part_widths[idx][1]  = 3;
            break;

        case MCS_IFA_MD_ELEMENT_OPAQUE_DATA_3:
            msg->element_widths[idx] = 32;
            msg->num_fifo_parts[idx] = 2;

            msg->fifo_parts[idx][0] = MCS_IFA_FIFO_ELEMENT_MMU_INT_STAT1_15_0;
            msg->fifo_part_offsets[idx][0] = 0;
            msg->fifo_part_widths[idx][0]  = 16;

            msg->fifo_parts[idx][1] = MCS_IFA_FIFO_ELEMENT_MMU_INT_STAT1_18_16;
            msg->fifo_part_offsets[idx][1] = 0;
            msg->fifo_part_widths[idx][1]  = 3;
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    msg->num_elements++;

 exit:
    SHR_FUNC_EXIT();
}

static int
int_ifa_uc_md_format_set(int unit)
{
    int_info_t *int_info = g_int_info[unit];
    mcs_ifa_msg_ctrl_md_format_set_t msg;
    uint8_t *buffer_ptr = NULL;
    uint16_t buffer_len = 0, reply_len = 0;
    int i;
    mcs_ifa_md_elements_t md_fmt[] =
        {
            MCS_IFA_MD_ELEMENT_LNS_DEVICE_ID,
            MCS_IFA_MD_ELEMENT_IP_TTL,
            MCS_IFA_MD_ELEMENT_EGRESS_PORT_SPEED,
            MCS_IFA_MD_ELEMENT_CONGESTION,
            MCS_IFA_MD_ELEMENT_QUEUE_ID,
            MCS_IFA_MD_ELEMENT_RX_TIMESTAMP_SEC,
            MCS_IFA_MD_ELEMENT_EGRESS_SYSTEM_PORT,
            MCS_IFA_MD_ELEMENT_INGRESS_SYSTEM_PORT,
            MCS_IFA_MD_ELEMENT_RX_TIMESTAMP_NANO_SEC,
            MCS_IFA_MD_ELEMENT_RESIDENCE_TIME_NANO_SEC,
            MCS_IFA_MD_ELEMENT_OPAQUE_DATA_1,
            MCS_IFA_MD_ELEMENT_OPAQUE_DATA_2,
            MCS_IFA_MD_ELEMENT_OPAQUE_DATA_3
        };


    SHR_FUNC_ENTER(unit);

    sal_memset(&msg, 0, sizeof(msg));
    msg.flags     |= MCS_IFA_MSG_CTRL_MD_FORMAT_SET_FLAGS_LNS;
    msg.namespace  = 1;

    /* Populate the elements and their widths of the MD elements. */
    for (i = 0; i < COUNTOF(md_fmt); i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (int_ifa_uc_md_element_msg_set(unit, md_fmt[i], &msg));
    }

    buffer_ptr = bcmcth_mon_int_ifa_msg_ctrl_md_format_set_pack(int_info->dma_buffer,
                                                                &msg);
    buffer_len = buffer_ptr - int_info->dma_buffer;

    SHR_IF_ERR_VERBOSE_EXIT
        (int_ifa_uc_msg_send_receive(unit,
                                     MCS_MSG_SUBCLASS_IFA_MD_FORMAT_SET,
                                     buffer_len, 0,
                                     MCS_MSG_SUBCLASS_IFA_MD_FORMAT_SET_REPLY,
                                     &reply_len));
    if (reply_len != 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

 exit:
    SHR_FUNC_EXIT();
}

static int
int_ifa_uc_config_update(int unit, uint16_t num_params, uint32_t *params,
                         uint16_t *num_values,
                         uint32_t values[][MCS_IFA_MSG_CTRL_CONFIG_UPDATE_MAX_VALUES])
{
    int_info_t *int_info = g_int_info[unit];
    mcs_ifa_msg_ctrl_config_update_t msg;
    int i, j;
    uint8_t *buffer_ptr = NULL;
    uint16_t buffer_len = 0, reply_len = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&msg, 0, sizeof(msg));

    msg.num_params  = num_params;
    for (i = 0; i < num_params; i++) {
        msg.params[i] = params[i];
        msg.num_values[i] = num_values[i];
        for (j = 0; j < num_values[i]; j++) {
            msg.values[i][j] = values[i][j];
        }
    }
    buffer_ptr = bcmcth_mon_int_ifa_msg_ctrl_config_update_pack(int_info->dma_buffer,
                                                                &msg);
    buffer_len = buffer_ptr - int_info->dma_buffer;

    SHR_IF_ERR_VERBOSE_EXIT
        (int_ifa_uc_msg_send_receive(unit,
                                     MCS_MSG_SUBCLASS_IFA_CONFIG_UPDATE,
                                     buffer_len, 0,
                                     MCS_MSG_SUBCLASS_IFA_CONFIG_UPDATE_REPLY,
                                     &reply_len));

    if (reply_len != 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

 exit:
    SHR_FUNC_EXIT();
}

static int
int_ifa_uc_ipfix_template_set(int unit, bcmcth_mon_int_ipfix_export_entry_t *entry)
{
    int_info_t *int_info = g_int_info[unit];
    mcs_ifa_msg_ctrl_ipfix_template_set_t msg;
    uint8_t *buffer_ptr = NULL;
    uint16_t buffer_len = 0, reply_len = 0;
    bcmcth_mon_int_drv_t *int_drv = NULL;
    uint16_t element_id, size;
    uint8_t *cur_ptr, *set_length_ptr;
    uint32_t i;

    SHR_FUNC_ENTER(unit);


    int_drv = bcmcth_mon_int_drv_get(unit);
    if (!int_drv                             ||
        !int_drv->ipfix_export_element_get ||
        !int_drv->ipfix_export_element_size_get) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }


    sal_memset(&msg, 0, sizeof(msg));

    msg.set_id        = entry->set_id;
    msg.tx_interval   = entry->tx_interval;
    msg.initial_burst = entry->initial_burst;
    msg.num_elements  = entry->num_elements;

    for (i = 0; i < entry->num_elements; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (int_drv->ipfix_export_element_get(unit,
                                               entry->elements[i].element,
                                               &(msg.elements[i])));
    }


    /* Construct the template set. */
    cur_ptr = msg.template_set;

    /*
     * +----------------------+------------------+
     * |     Bits 0...15      |   Bits 16...31   |
     * +----------------------+------------------+
     * | Set ID = 2(Template) |    Set Length    |
     * +----------------------+------------------+
     * |     Template ID      | Number of fields |
     * +----------------------+------------------+
     * |        Type          |   Field Length   |
     * +----------------------+------------------+
     * |        Type          |   Field Length   |
     * +----------------------+------------------+
     * |        Type          |   Field Length   |
     * +----------------------+------------------+
     */
    SHR_UTIL_PACK_U16(cur_ptr, IFA_IPFIX_SET_ID_TEMPLATE_SET);

    /* Skip over length */
    set_length_ptr = cur_ptr;
    cur_ptr += 2;

    SHR_UTIL_PACK_U16(cur_ptr, entry->set_id);
    SHR_UTIL_PACK_U16(cur_ptr, entry->num_elements);

    for (i = 0; i < entry->num_elements; i++) {
        if (entry->elements[i].enterprise == true) {
            /* Enterprise specific */
            /*
             * +-+--------------------+------------------+
             * |0|      1...15        |     16...31      |
             * +-+--------------------+------------------+
             * |1|      Type          |   Field Length   |
             * +----------------------+------------------+
             * |             Enterprise number           |
             * +----------------------+------------------+
             */
            /* Set the enterprise bit */
            element_id = (1 << 15) | entry->elements[i].info_element_id;
            SHR_UTIL_PACK_U16(cur_ptr, element_id);
            SHR_IF_ERR_VERBOSE_EXIT
                (int_drv->ipfix_export_element_size_get(unit,
                                                        entry->elements[i].element,
                                                        &size));
            SHR_UTIL_PACK_U16(cur_ptr, size);
            SHR_UTIL_PACK_U32(cur_ptr, int_info->ha->ipfix_enterprise_number);
        } else {
            /* Non enterprise element, format below */
            /*
             * +-+--------------------+------------------+
             * |0|      1...15        |     16...31      |
             * +-+--------------------+------------------+
             * |0|      Type          |   Field Length   |
             * +----------------------+------------------+
             */
            SHR_UTIL_PACK_U16(cur_ptr, entry->elements[i].info_element_id);
            SHR_IF_ERR_VERBOSE_EXIT
                (int_drv->ipfix_export_element_size_get(unit,
                                                        entry->elements[i].element,
                                                        &size));
            SHR_UTIL_PACK_U16(cur_ptr, size);
        }
    }

    msg.template_set_length = cur_ptr - msg.template_set;

    /* Populate the set length. */
    SHR_UTIL_PACK_U16(set_length_ptr, cur_ptr - msg.template_set);

    buffer_ptr = bcmcth_mon_int_ifa_msg_ctrl_ipfix_template_set_pack(int_info->dma_buffer,
                                                                     &msg);
    buffer_len = buffer_ptr - int_info->dma_buffer;


    SHR_IF_ERR_VERBOSE_EXIT
        (int_ifa_uc_msg_send_receive(unit,
                                     MCS_MSG_SUBCLASS_IFA_IPFIX_TEMPLATE_SET,
                                     buffer_len, 0,
                                     MCS_MSG_SUBCLASS_IFA_IPFIX_TEMPLATE_SET_REPLY,
                                     &reply_len));

    if (reply_len != 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

 exit:
    SHR_FUNC_EXIT();
}

static int
int_ifa_uc_stats_set(int unit,
                     bcmcth_mon_int_stats_t *entry)
{
    int_info_t *int_info = g_int_info[unit];
    mcs_ifa_msg_ctrl_stats_set_t msg;
    uint16_t buffer_len = 0, reply_len = 0;
    uint8_t *buffer_ptr = NULL;

    SHR_FUNC_ENTER(unit);

    sal_memset(&msg, 0, sizeof(msg));

    if (SHR_BITGET(entry->fbmp, MON_INBAND_TELEMETRY_STATSt_RX_PKT_CNTf)) {
        msg.valid_bmp |= MCS_IFA_MSG_STATS_SET_BMP_RX_PKT_CNT;
        msg.rx_pkt_cnt_hi = INT_64_HI(entry->rx_pkt_cnt);
        msg.rx_pkt_cnt_lo = INT_64_LO(entry->rx_pkt_cnt);
    }

    if (SHR_BITGET(entry->fbmp, MON_INBAND_TELEMETRY_STATSt_TX_PKT_CNTf)) {
        msg.valid_bmp |= MCS_IFA_MSG_STATS_SET_BMP_TX_PKT_CNT;
        msg.tx_pkt_cnt_hi = INT_64_HI(entry->tx_pkt_cnt);
        msg.tx_pkt_cnt_lo = INT_64_LO(entry->tx_pkt_cnt);
    }

    if (SHR_BITGET(entry->fbmp, MON_INBAND_TELEMETRY_STATSt_TX_RECORD_CNTf)) {
        msg.valid_bmp |= MCS_IFA_MSG_STATS_SET_BMP_TX_RECORD_CNT;
        msg.tx_record_cnt_hi = INT_64_HI(entry->tx_record_cnt);
        msg.tx_record_cnt_lo = INT_64_LO(entry->tx_record_cnt);
    }

    if (SHR_BITGET(entry->fbmp,
                   MON_INBAND_TELEMETRY_STATSt_RX_PKT_EXPORT_DISABLED_DISCARDf)) {
        msg.valid_bmp |= MCS_IFA_MSG_STATS_SET_BMP_RX_PKT_EXPORT_DISABLED_DISCARD;
        msg.rx_pkt_export_disabled_discard_hi =
            INT_64_HI(entry->rx_pkt_export_disabled_discard);
        msg.rx_pkt_export_disabled_discard_lo =
            INT_64_LO(entry->rx_pkt_export_disabled_discard);
    }

    if (SHR_BITGET(entry->fbmp,
                   MON_INBAND_TELEMETRY_STATSt_RX_PKT_NO_EXPORT_CONFIG_DISCARDf)) {
        msg.valid_bmp |= MCS_IFA_MSG_STATS_SET_BMP_RX_PKT_NO_EXPORT_CONFIG_DISCARD;
        msg.rx_pkt_no_export_config_discard_hi =
            INT_64_HI(entry->rx_pkt_no_export_config_discard);
        msg.rx_pkt_no_export_config_discard_lo =
            INT_64_LO(entry->rx_pkt_no_export_config_discard);
    }

    if (SHR_BITGET(entry->fbmp,
                   MON_INBAND_TELEMETRY_STATSt_RX_PKT_NO_IPFIX_CONFIG_DISCARDf)) {
        msg.valid_bmp |= MCS_IFA_MSG_STATS_SET_BMP_RX_PKT_NO_IPFIX_CONFIG_DISCARD;
        msg.rx_pkt_no_ipfix_config_discard_hi =
            INT_64_HI(entry->rx_pkt_no_ipfix_config_discard);
        msg.rx_pkt_no_ipfix_config_discard_lo =
            INT_64_LO(entry->rx_pkt_no_ipfix_config_discard);
    }

    if (SHR_BITGET(entry->fbmp,
                   MON_INBAND_TELEMETRY_STATSt_RX_PKT_CURRENT_LENGTH_EXCEED_DISCARDf)) {
        msg.valid_bmp |= MCS_IFA_MSG_STATS_SET_BMP_RX_PKT_CURRENT_LENGTH_EXCEED_DISCARD;
        msg.rx_pkt_current_length_exceed_discard_hi =
            INT_64_HI(entry->rx_pkt_current_length_exceed_discard);
        msg.rx_pkt_current_length_exceed_discard_lo =
            INT_64_LO(entry->rx_pkt_current_length_exceed_discard);
    }

    if (SHR_BITGET(entry->fbmp,
                   MON_INBAND_TELEMETRY_STATSt_RX_PKT_LENGTH_EXCEED_MAX_DISCARDf)) {
        msg.valid_bmp |= MCS_IFA_MSG_STATS_SET_BMP_RX_PKT_LENGTH_EXCEED_MAX_DISCARD;
        msg.rx_pkt_length_exceed_max_discard_hi =
            INT_64_HI(entry->rx_pkt_length_exceed_max_discard);
        msg.rx_pkt_length_exceed_max_discard_lo =
            INT_64_LO(entry->rx_pkt_length_exceed_max_discard);
    }

    if (SHR_BITGET(entry->fbmp,
                   MON_INBAND_TELEMETRY_STATSt_RX_PKT_PARSE_ERROR_DISCARDf)) {
        msg.valid_bmp |= MCS_IFA_MSG_STATS_SET_BMP_RX_PKT_PARSE_ERROR_DISCARD;
        msg.rx_pkt_parse_error_discard_hi =
            INT_64_HI(entry->rx_pkt_parse_error_discard);
        msg.rx_pkt_parse_error_discard_lo =
            INT_64_LO(entry->rx_pkt_parse_error_discard);
    }

    if (SHR_BITGET(entry->fbmp,
                   MON_INBAND_TELEMETRY_STATSt_RX_PKT_UNKNOWN_NAMESPACE_DISCARDf)) {
        msg.valid_bmp |= MCS_IFA_MSG_STATS_SET_BMP_RX_PKT_UNKNOWN_NAMESPACE_DISCARD;
        msg.rx_pkt_unknown_namespace_discard_hi =
            INT_64_HI(entry->rx_pkt_unknown_namespace_discard);
        msg.rx_pkt_unknown_namespace_discard_lo =
            INT_64_LO(entry->rx_pkt_unknown_namespace_discard);
    }

    if (SHR_BITGET(entry->fbmp,
                   MON_INBAND_TELEMETRY_STATSt_RX_PKT_EXCESS_RATE_DISCARDf)) {
        msg.valid_bmp |= MCS_IFA_MSG_STATS_SET_BMP_RX_PKT_EXCESS_RATE_DISCARD;
        msg.rx_pkt_excess_rate_discard_hi =
            INT_64_HI(entry->rx_pkt_excess_rate_discard);
        msg.rx_pkt_excess_rate_discard_lo =
            INT_64_LO(entry->rx_pkt_excess_rate_discard);
    }

    if (SHR_BITGET(entry->fbmp,
                   MON_INBAND_TELEMETRY_STATSt_RX_PKT_INCOMPLETE_METADATA_DISCARDf)) {
        msg.valid_bmp |= MCS_IFA_MSG_STATS_SET_BMP_RX_PKT_INCOMPLETE_METADATA_DISCARD;
        msg.rx_pkt_incomplete_metadata_discard_hi =
            INT_64_HI(entry->rx_pkt_incomplete_metadata_discard);
        msg.rx_pkt_incomplete_metadata_discard_lo =
            INT_64_LO(entry->rx_pkt_incomplete_metadata_discard);
    }

    if (SHR_BITGET(entry->fbmp,
                   MON_INBAND_TELEMETRY_STATSt_TX_PKT_FAILURE_CNTf)) {
        msg.valid_bmp |= MCS_IFA_MSG_STATS_SET_BMP_TX_PKT_FAILURE_CNT;
        msg.tx_pkt_failure_cnt_hi = INT_64_HI(entry->tx_pkt_failure_cnt);
        msg.tx_pkt_failure_cnt_lo = INT_64_LO(entry->tx_pkt_failure_cnt);
    }

    buffer_ptr = bcmcth_mon_int_ifa_msg_ctrl_stats_set_pack(int_info->dma_buffer,
                                                            &msg);
    buffer_len = buffer_ptr - int_info->dma_buffer;

    SHR_IF_ERR_VERBOSE_EXIT
        (int_ifa_uc_msg_send_receive(unit,
                                     MCS_MSG_SUBCLASS_IFA_STATS_SET,
                                     buffer_len, 0,
                                     MCS_MSG_SUBCLASS_IFA_STATS_SET_REPLY,
                                     &reply_len));

    if (reply_len != 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

 exit:
    SHR_FUNC_EXIT();
}

static int
int_ifa_uc_stats_get(int unit,
                     bcmcth_mon_int_stats_t *entry)
{
    int_info_t *int_info = g_int_info[unit];
    mcs_ifa_msg_ctrl_stats_get_t msg;
    uint8_t *buffer_ptr = NULL;
    uint16_t buffer_len = 0, reply_len = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&msg, 0, sizeof(msg));

    SHR_IF_ERR_VERBOSE_EXIT
        (int_ifa_uc_msg_send_receive(unit,
                                     MCS_MSG_SUBCLASS_IFA_STATS_GET,
                                     0, 0,
                                     MCS_MSG_SUBCLASS_IFA_STATS_GET_REPLY,
                                     &reply_len));
    buffer_ptr = bcmcth_mon_int_ifa_msg_ctrl_stats_get_unpack(int_info->dma_buffer,
                                                              &msg);
    buffer_len = buffer_ptr - int_info->dma_buffer;

    if (reply_len != buffer_len) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    entry->rx_pkt_cnt = INT_64_GET(msg.rx_pkt_cnt_hi, msg.rx_pkt_cnt_lo);
    entry->tx_pkt_cnt = INT_64_GET(msg.tx_pkt_cnt_hi, msg.tx_pkt_cnt_lo);

    entry->tx_record_cnt =
        INT_64_GET(msg.tx_record_cnt_hi, msg.tx_record_cnt_lo);

    entry->rx_pkt_export_disabled_discard =
        INT_64_GET(msg.rx_pkt_export_disabled_discard_hi,
                   msg.rx_pkt_export_disabled_discard_lo);

    entry->rx_pkt_no_export_config_discard =
        INT_64_GET(msg.rx_pkt_no_export_config_discard_hi,
                   msg.rx_pkt_no_export_config_discard_lo);

    entry->rx_pkt_no_ipfix_config_discard =
        INT_64_GET(msg.rx_pkt_no_ipfix_config_discard_hi,
                   msg.rx_pkt_no_ipfix_config_discard_lo);

    entry->rx_pkt_current_length_exceed_discard =
        INT_64_GET(msg.rx_pkt_current_length_exceed_discard_hi,
                   msg.rx_pkt_current_length_exceed_discard_lo);

    entry->rx_pkt_length_exceed_max_discard =
        INT_64_GET(msg.rx_pkt_length_exceed_max_discard_hi,
                   msg.rx_pkt_length_exceed_max_discard_lo);

    entry->rx_pkt_parse_error_discard =
        INT_64_GET(msg.rx_pkt_parse_error_discard_hi,
                   msg.rx_pkt_parse_error_discard_lo);

    entry->rx_pkt_unknown_namespace_discard =
        INT_64_GET(msg.rx_pkt_unknown_namespace_discard_hi,
                   msg.rx_pkt_unknown_namespace_discard_lo);

    entry->rx_pkt_incomplete_metadata_discard =
        INT_64_GET(msg.rx_pkt_incomplete_metadata_discard_hi,
                   msg.rx_pkt_incomplete_metadata_discard_lo);

    entry->rx_pkt_excess_rate_discard =
        INT_64_GET(msg.rx_pkt_excess_rate_discard_hi,
                   msg.rx_pkt_excess_rate_discard_lo);

    entry->tx_pkt_failure_cnt = INT_64_GET(msg.tx_pkt_failure_cnt_hi,
                                           msg.tx_pkt_failure_cnt_lo);

 exit:
    SHR_FUNC_EXIT();
}

static int
int_ifa_uc_collector_ipv4_create(int unit,
                                 bcmcth_mon_collector_ipv4_t *collector,
                                 bcmcth_mon_export_profile_t *export_profile)
{
    int_info_t *int_info = g_int_info[unit];
    uint8_t *cur_ptr;
    shr_util_pack_udp_header_t udp;
    shr_util_pack_ipv4_header_t ipv4;
    shr_util_pack_l2_header_t l2;
    mcs_ifa_msg_ctrl_collector_create_t msg;
    uint8_t *buffer_ptr = NULL;
    uint16_t buffer_len = 0, reply_len = 0;
    bcmcth_mon_drv_t *mon_drv = bcmcth_mon_drv_get(unit);

    SHR_FUNC_ENTER(unit);

    sal_memset(&msg, 0, sizeof(msg));

    /* L2 header */
    sal_memset(&l2, 0, sizeof(l2));
    sal_memcpy(l2.dst_mac, collector->dst_mac, sizeof(shr_mac_t));
    sal_memcpy(l2.src_mac, collector->src_mac, sizeof(shr_mac_t));

    /* Call chip specific msg fill routine */
    if (mon_drv && mon_drv->collector_ipv4_vlan_info_copy) {
        mon_drv->collector_ipv4_vlan_info_copy(unit, collector, &l2);
    }
    l2.etype = SHR_L2_ETYPE_IPV4;

    /* IPv4 header. */
    sal_memset(&ipv4, 0, sizeof(ipv4));
    ipv4.version = SHR_IPV4_VERSION;
    ipv4.h_length = SHR_IPV4_HDR_WLEN;
    ipv4.dscp = collector->tos;
    ipv4.ttl = collector->ttl;
    ipv4.protocol = SHR_IP_PROTO_UDP;
    ipv4.src = collector->src_ip;
    ipv4.dst = collector->dst_ip;

    /* UDP header. */
    sal_memset(&udp, 0, sizeof(udp));
    udp.src = collector->src_port;
    udp.dst = collector->dst_port;

    /* Pack the headers in the message and get the offsets. */
    cur_ptr = msg.encap;

    cur_ptr = shr_util_pack_l2_header_pack(cur_ptr, &l2);

    msg.ip_offset = cur_ptr - msg.encap;
    cur_ptr = shr_util_pack_ipv4_header_pack(cur_ptr, &ipv4);

    msg.udp_offset = cur_ptr - msg.encap;
    cur_ptr = shr_util_pack_udp_header_pack(cur_ptr, &udp);

    /* Get the encap length. */
    msg.encap_length = cur_ptr - msg.encap;

    msg.ip_base_checksum =
        shr_util_pack_initial_chksum_get(SHR_IPV4_HEADER_LENGTH,
                                         msg.encap + msg.ip_offset);

    msg.udp_base_checksum =
        shr_util_pack_udp_initial_checksum_get(0, &ipv4, NULL, NULL, &udp);

    msg.observation_domain = collector->observation_domain;
    msg.flags |= MCS_IFA_MSG_CTRL_COLLECTOR_FLAGS_L3_TYPE_IPV4;
    if (collector->udp_chksum == true) {
        msg.flags |= MCS_IFA_MSG_CTRL_COLLECTOR_FLAGS_UDP_CHECKSUM_ENABLE;
    }

    /* Get the required fields from export profile. */
    if (export_profile->packet_len_indicator ==
        BCMLTD_COMMON_EXPORT_PACKET_LEN_INDICATOR_T_T_MAX_PKT_LENGTH) {
        msg.mtu = export_profile->max_packet_length;
    } else {
        msg.num_records = export_profile->num_records;
        msg.flags |= MCS_IFA_MSG_CTRL_COLLECTOR_FLAGS_USE_NUM_RECORDS;
    }

    buffer_ptr = bcmcth_mon_int_ifa_msg_ctrl_collector_create_pack(int_info->dma_buffer,
                                                                   &msg);
    buffer_len = buffer_ptr - int_info->dma_buffer;
    SHR_IF_ERR_VERBOSE_EXIT
        (int_ifa_uc_msg_send_receive(unit,
                                     MCS_MSG_SUBCLASS_IFA_COLLECTOR_CREATE,
                                     buffer_len, 0,
                                     MCS_MSG_SUBCLASS_IFA_COLLECTOR_CREATE_REPLY,
                                     &reply_len));

    if (reply_len != 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    int_info->ha->collector = true;
    /* Set the IPFIX enterprise number. */
    int_info->ha->ipfix_enterprise_number = collector->enterprise_number;

 exit:
    SHR_FUNC_EXIT();

}

static int
int_ifa_uc_collector_ipv6_create(int unit,
                                 bcmcth_mon_collector_ipv6_t *collector,
                                 bcmcth_mon_export_profile_t *export_profile)
{
    int_info_t *int_info = g_int_info[unit];
    uint8_t *cur_ptr;
    shr_util_pack_udp_header_t udp;
    shr_util_pack_ipv6_header_t ipv6;
    shr_util_pack_l2_header_t l2;
    mcs_ifa_msg_ctrl_collector_create_t msg;
    uint8_t *buffer_ptr = NULL;
    uint16_t buffer_len = 0, reply_len = 0;
    bcmcth_mon_drv_t *mon_drv = bcmcth_mon_drv_get(unit);

    SHR_FUNC_ENTER(unit);

    sal_memset(&msg, 0, sizeof(msg));

    /* L2 header */
    sal_memset(&l2, 0, sizeof(l2));
    sal_memcpy(l2.dst_mac, collector->dst_mac, sizeof(shr_mac_t));
    sal_memcpy(l2.src_mac, collector->src_mac, sizeof(shr_mac_t));

    /* Call chip specific msg fill routine */
    if (mon_drv && mon_drv->collector_ipv6_vlan_info_copy) {
        mon_drv->collector_ipv6_vlan_info_copy(unit, collector, &l2);
    }
    l2.etype = SHR_L2_ETYPE_IPV6;

    /* Ipv6 header. */
    sal_memset(&ipv6, 0, sizeof(ipv6));
    ipv6.version = SHR_IPV6_VERSION;
    ipv6.t_class = collector->tos;
    ipv6.f_label = collector->flow_label;
    ipv6.next_header = SHR_IP_PROTO_UDP;
    ipv6.hop_limit = collector->ttl;
    sal_memcpy(ipv6.src, collector->src_ip6_addr, sizeof(shr_ip6_t));
    sal_memcpy(ipv6.dst, collector->dst_ip6_addr, sizeof(shr_ip6_t));

    /* UDP header. */
    sal_memset(&udp, 0, sizeof(udp));
    udp.src = collector->src_port;
    udp.dst = collector->dst_port;

    /* Pack the headers in the message and get the offsets. */
    cur_ptr = msg.encap;

    cur_ptr = shr_util_pack_l2_header_pack(cur_ptr, &l2);

    msg.ip_offset = cur_ptr - msg.encap;
    cur_ptr = shr_util_pack_ipv6_header_pack(cur_ptr, &ipv6);

    msg.udp_offset = cur_ptr - msg.encap;
    cur_ptr = shr_util_pack_udp_header_pack(cur_ptr, &udp);

    /* Get the encap length. */
    msg.encap_length = cur_ptr - msg.encap;

    msg.ip_base_checksum =
        shr_util_pack_initial_chksum_get(SHR_IPV6_HEADER_LENGTH,
                                         msg.encap + msg.ip_offset);

    msg.udp_base_checksum =
        shr_util_pack_udp_initial_checksum_get(1, NULL, &ipv6, NULL, &udp);

    msg.observation_domain = collector->observation_domain;
    if (collector->udp_chksum == true) {
        msg.flags |= MCS_IFA_MSG_CTRL_COLLECTOR_FLAGS_UDP_CHECKSUM_ENABLE;
    }

    /* Get the required fields from export profile. */
    if (export_profile->packet_len_indicator ==
        BCMLTD_COMMON_EXPORT_PACKET_LEN_INDICATOR_T_T_MAX_PKT_LENGTH) {
        msg.mtu = export_profile->max_packet_length;
    } else {
        msg.num_records = export_profile->num_records;
        msg.flags |= MCS_IFA_MSG_CTRL_COLLECTOR_FLAGS_USE_NUM_RECORDS;
    }

    buffer_ptr = bcmcth_mon_int_ifa_msg_ctrl_collector_create_pack(int_info->dma_buffer,
                                                                   &msg);
    buffer_len = buffer_ptr - int_info->dma_buffer;

    SHR_IF_ERR_VERBOSE_EXIT
        (int_ifa_uc_msg_send_receive(unit,
                                     MCS_MSG_SUBCLASS_IFA_COLLECTOR_CREATE,
                                     buffer_len, 0,
                                     MCS_MSG_SUBCLASS_IFA_COLLECTOR_CREATE_REPLY,
                                     &reply_len));

    if (reply_len != 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    int_info->ha->collector = true;
    /* Set the IPFIX enterprise number. */
    int_info->ha->ipfix_enterprise_number = collector->enterprise_number;

 exit:
    SHR_FUNC_EXIT();

}

static int
int_ifa_uc_collector_delete(int unit)
{
    int_info_t *int_info = g_int_info[unit];
    uint16_t reply_len = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (int_ifa_uc_msg_send_receive(unit,
                                     MCS_MSG_SUBCLASS_IFA_COLLECTOR_DELETE,
                                     0, 0,
                                     MCS_MSG_SUBCLASS_IFA_COLLECTOR_DELETE_REPLY,
                                     &reply_len));

    if (reply_len != 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    int_info->ha->collector = false;

 exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * uC alloc and free messages from the SYSM
 */
int
bcmcth_mon_int_uc_alloc(int unit, bool warm)
{
    uint32_t ha_alloc_size = 0, ha_req_size = 0;
    int_info_t *int_info = NULL;
    int idx;

    SHR_FUNC_ENTER(unit);

    SHR_ALLOC(g_int_info[unit], sizeof(int_info_t), "bcmcthMonIntInfo");
    SHR_NULL_CHECK(g_int_info[unit], SHR_E_MEMORY);
    sal_memset(g_int_info[unit], 0, sizeof(int_info_t));
    int_info = g_int_info[unit];

    /* Allocate HA memory */
    ha_req_size = sizeof(bcmcth_mon_int_ha_t);
    ha_alloc_size = ha_req_size;

    int_info->ha = shr_ha_mem_alloc(unit, BCMMGMT_MON_COMP_ID,
                                   BCMMON_INT_SUB_COMP_ID,
                                   "intHaCtrl",
                                   &ha_alloc_size);
    SHR_NULL_CHECK(int_info->ha, SHR_E_MEMORY);

    SHR_IF_ERR_VERBOSE_EXIT
        ((ha_alloc_size < ha_req_size) ? SHR_E_MEMORY : SHR_E_NONE);

    if (!warm) {
        sal_memset(int_info->ha, 0, ha_alloc_size);

        /* Initialize all the oper states to false. */
        for (idx = 0; idx < OPER_STATE_COUNT; idx++) {
            int_info->ha->oper_states[idx] = false;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmissu_struct_info_report(unit, BCMMGMT_MON_COMP_ID,
                                        BCMMON_INT_SUB_COMP_ID, 0,
                                        ha_req_size, 1,
                                        BCMCTH_MON_INT_HA_T_ID));
    }

    if (bcmdrd_feature_is_sim(unit) == false) {
        /* Allocate DMA buffer */
        int_info->dma_buffer = bcmdrd_hal_dma_alloc(unit,
                                                    sizeof(mcs_ifa_msg_ctrl_t),
                                                    "INT DMA buffer",
                                                    &(int_info->dma_buffer_p_addr));
        if (int_info->dma_buffer == NULL) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
    }

 exit:
    if (SHR_FUNC_ERR()) {
        if (g_int_info[unit] != NULL) {
            if (g_int_info[unit]->ha != NULL) {
                shr_ha_mem_free(unit, g_int_info[unit]->ha);
            }
            if (int_info->dma_buffer != NULL) {
                bcmdrd_hal_dma_free(unit, sizeof(mcs_ifa_msg_ctrl_t),
                                    int_info->dma_buffer,
                                    int_info->dma_buffer_p_addr);
                int_info->dma_buffer = NULL;
                int_info->dma_buffer_p_addr = 0;
            }
            SHR_FREE(g_int_info[unit]);
        }
    }
    SHR_FUNC_EXIT();
}

void
bcmcth_mon_int_uc_free(int unit)
{
    if (g_int_info[unit] == NULL) {
        return;
    }
    if (g_int_info[unit]->dma_buffer != NULL) {
        bcmdrd_hal_dma_free(unit, sizeof(mcs_ifa_msg_ctrl_t),
                            g_int_info[unit]->dma_buffer,
                            g_int_info[unit]->dma_buffer_p_addr);
    }
    SHR_FREE(g_int_info[unit]);
}

/*******************************************************************************
 * Translation functions to convert IMM updates to EApp messages.
 */

static int
int_control_defaults_get(int unit, bcmcth_mon_int_control_entry_t *entry)
{
    bcmlrd_client_field_info_t *fields_info = NULL;
    size_t fld_num = MON_INBAND_TELEMETRY_CONTROLt_FIELD_COUNT;
    size_t actual_fld_num;
    size_t idx;

    SHR_FUNC_ENTER(unit);


    SHR_ALLOC(fields_info, sizeof(bcmlrd_client_field_info_t) * fld_num,
              "bcmcthMonIntControlFldInfo");
    SHR_NULL_CHECK(fields_info, SHR_E_MEMORY);

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlrd_table_fields_def_get(unit, "MON_INBAND_TELEMETRY_CONTROL",
                                    fld_num, fields_info, &actual_fld_num));

    for (idx = 0; idx < actual_fld_num; idx++) {
        if (SHR_BITGET(entry->fbmp, fields_info[idx].id)) {
            /* The field is already configured, don't overwrite with default. */
            continue;
        }

        switch (fields_info[idx].id) {
            case MON_INBAND_TELEMETRY_CONTROLt_INBAND_TELEMETRYf:
                entry->inband_telemetry = fields_info[idx].def.is_true;
                break;

            case MON_INBAND_TELEMETRY_CONTROLt_MAX_EXPORT_PKT_LENGTHf:
                entry->max_export_pkt_length = fields_info[idx].def.u16;
                break;

            case MON_INBAND_TELEMETRY_CONTROLt_MAX_RX_PKT_LENGTHf:
                entry->max_rx_pkt_length = fields_info[idx].def.u16;
                break;

            case MON_INBAND_TELEMETRY_CONTROLt_DEVICE_IDENTIFIERf:
                entry->device_id = fields_info[idx].def.u32;
                break;

            case MON_INBAND_TELEMETRY_CONTROLt_EXPORTf:
                entry->export = fields_info[idx].def.is_true;
                break;

            case MON_INBAND_TELEMETRY_CONTROLt_RX_PKT_EXPORT_MAX_LENGTHf:
                entry->rx_pkt_export_max_length = fields_info[idx].def.u16;
                break;

            case MON_INBAND_TELEMETRY_CONTROLt_COLLECTOR_TYPEf:
                entry->collector_type = fields_info[idx].def.u8;
                break;

            case MON_INBAND_TELEMETRY_CONTROLt_MON_COLLECTOR_IPV4_IDf:
                entry->collector_ipv4_id = fields_info[idx].def.u32;
                entry->collector_ipv4_found = false;
                sal_memset(&(entry->collector_ipv4), 0,
                           sizeof(entry->collector_ipv4));
                break;

            case MON_INBAND_TELEMETRY_CONTROLt_MON_COLLECTOR_IPV6_IDf:
                entry->collector_ipv6_id = fields_info[idx].def.u32;
                entry->collector_ipv6_found = false;
                sal_memset(&(entry->collector_ipv6), 0,
                           sizeof(entry->collector_ipv6));
                break;

            case MON_INBAND_TELEMETRY_CONTROLt_MON_EXPORT_PROFILE_IDf:
                entry->export_profile_id = fields_info[idx].def.u32;
                entry->export_profile_found = false;
                sal_memset(&(entry->export_profile), 0,
                           sizeof(entry->export_profile));
                break;

            case MON_INBAND_TELEMETRY_CONTROLt_MAX_EXPORT_PKT_LENGTH_OPERf:
            case MON_INBAND_TELEMETRY_CONTROLt_MAX_RX_PKT_LENGTH_OPERf:
            case MON_INBAND_TELEMETRY_CONTROLt_OPERATIONAL_STATEf:
                /* Don't get the defaults for the read-only fields. */
                break;

            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FREE(fields_info);
    SHR_FUNC_EXIT();
}

/* Do a field by field copy into dst entry. Only copy those fields which are set
 * in the field bitmap of src.
 */
static void
int_control_fields_copy(bcmcth_mon_int_control_entry_t *dst,
                        bcmcth_mon_int_control_entry_t *src)
{
    /* Copy the field bitmap of dst into src. */
    sal_memcpy(&(dst->fbmp), &(src->fbmp), sizeof(dst->fbmp));

    if (SHR_BITGET(src->fbmp,
                   MON_INBAND_TELEMETRY_CONTROLt_INBAND_TELEMETRYf)) {
        dst->inband_telemetry = src->inband_telemetry;
    }

    if (SHR_BITGET(src->fbmp,
                   MON_INBAND_TELEMETRY_CONTROLt_MAX_EXPORT_PKT_LENGTHf)) {
        dst->max_export_pkt_length = src->max_export_pkt_length;
    }

    if (SHR_BITGET(src->fbmp,
                   MON_INBAND_TELEMETRY_CONTROLt_MAX_RX_PKT_LENGTHf)) {
        dst->max_rx_pkt_length = src->max_rx_pkt_length;
    }

    if (SHR_BITGET(src->fbmp,
                   MON_INBAND_TELEMETRY_CONTROLt_DEVICE_IDENTIFIERf)) {
        dst->device_id = src->device_id;
    }

    if (SHR_BITGET(src->fbmp,
                   MON_INBAND_TELEMETRY_CONTROLt_EXPORTf)) {
        dst->export = src->export;
    }

    if (SHR_BITGET(src->fbmp,
                   MON_INBAND_TELEMETRY_CONTROLt_RX_PKT_EXPORT_MAX_LENGTHf)) {
        dst->rx_pkt_export_max_length = src->rx_pkt_export_max_length;
    }

    if (SHR_BITGET(src->fbmp,
                   MON_INBAND_TELEMETRY_CONTROLt_COLLECTOR_TYPEf)) {
        dst->collector_type = src->collector_type;
    }

    if (SHR_BITGET(src->fbmp,
                   MON_INBAND_TELEMETRY_CONTROLt_MON_COLLECTOR_IPV4_IDf)) {
        dst->collector_ipv4_id = src->collector_ipv4_id;
        dst->collector_ipv4_found = src->collector_ipv4_found;
        sal_memcpy(&(dst->collector_ipv4), &(src->collector_ipv4),
                   sizeof(dst->collector_ipv4));

    }

    if (SHR_BITGET(src->fbmp,
                   MON_INBAND_TELEMETRY_CONTROLt_MON_COLLECTOR_IPV6_IDf)) {
        dst->collector_ipv6_id = src->collector_ipv6_id;
        dst->collector_ipv6_found = src->collector_ipv6_found;
        sal_memcpy(&(dst->collector_ipv6), &(src->collector_ipv6),
                   sizeof(dst->collector_ipv6));
    }

    if (SHR_BITGET(src->fbmp,
                   MON_INBAND_TELEMETRY_CONTROLt_MON_EXPORT_PROFILE_IDf)) {
        dst->export_profile_id = src->export_profile_id;
        dst->export_profile_found = src->export_profile_found;
        sal_memcpy(&(dst->export_profile), &(src->export_profile),
                   sizeof(dst->export_profile));
    }

}

static int
int_ipfix_export_defaults_get(int unit,
                              bcmcth_mon_int_ipfix_export_entry_t *entry)
{
    bcmlrd_client_field_info_t *fields_info = NULL;
    size_t fld_num = MON_INBAND_TELEMETRY_IPFIX_EXPORTt_FIELD_COUNT;
    size_t actual_fld_num;
    size_t idx;

    SHR_FUNC_ENTER(unit);


    SHR_ALLOC(fields_info, sizeof(bcmlrd_client_field_info_t) * fld_num,
              "bcmcthMonIntIpfixTemplateFldInfo");
    SHR_NULL_CHECK(fields_info, SHR_E_MEMORY);

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlrd_table_fields_def_get(unit, "MON_INBAND_TELEMETRY_IPFIX_EXPORT",
                                    fld_num, fields_info, &actual_fld_num));

    for (idx = 0; idx < actual_fld_num; idx++) {
        if (SHR_BITGET(entry->fbmp, fields_info[idx].id)) {
            /* The field is already configured, don't overwrite with default. */
            continue;
        }

        switch (fields_info[idx].id) {
            case MON_INBAND_TELEMETRY_IPFIX_EXPORTt_SET_IDf:
                entry->set_id = fields_info[idx].def.u16;
                break;

            case MON_INBAND_TELEMETRY_IPFIX_EXPORTt_NUM_EXPORT_ELEMENTSf:
            case MON_INBAND_TELEMETRY_IPFIX_EXPORTt_EXPORT_ELEMENTf:
            case MON_INBAND_TELEMETRY_IPFIX_EXPORTt_ENTERPRISEf:
            case MON_INBAND_TELEMETRY_IPFIX_EXPORTt_INFORMATION_ELEMENT_IDENTIFIERf:
            case MON_INBAND_TELEMETRY_IPFIX_EXPORTt_TX_INTERVALf:
            case MON_INBAND_TELEMETRY_IPFIX_EXPORTt_INITIAL_BURSTf:
            case MON_INBAND_TELEMETRY_IPFIX_EXPORTt_OPERATIONAL_STATEf:
                /* Invalid or 0 default value. */
                break;

            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FREE(fields_info);
    SHR_FUNC_EXIT();
}

/* Do a field by field copy into dst entry. Only copy those fields which are set
 * in the field bitmap of src.
 */
static void
int_ipfix_export_fields_copy(bcmcth_mon_int_ipfix_export_entry_t *dst,
                             bcmcth_mon_int_ipfix_export_entry_t *src)
{
    uint32_t idx;

    sal_memcpy(&(dst->fbmp), &(src->fbmp), sizeof(dst->fbmp));

    if (SHR_BITGET(src->fbmp,
                   MON_INBAND_TELEMETRY_IPFIX_EXPORTt_SET_IDf)) {
        dst->set_id = src->set_id;
    }

    if (SHR_BITGET(src->fbmp,
                   MON_INBAND_TELEMETRY_IPFIX_EXPORTt_NUM_EXPORT_ELEMENTSf)) {
        dst->num_elements = src->num_elements;
    }

    for (idx = 0; idx < src->num_elements; idx++) {
        if (SHR_BITGET(src->elements[idx].fbmp,
                       MON_INBAND_TELEMETRY_IPFIX_EXPORTt_EXPORT_ELEMENTf)) {
            dst->elements[idx].element = src->elements[idx].element;
        }
        if (SHR_BITGET(src->elements[idx].fbmp,
                       MON_INBAND_TELEMETRY_IPFIX_EXPORTt_ENTERPRISEf)) {
            dst->elements[idx].enterprise = src->elements[idx].enterprise;
        }
        if (SHR_BITGET(src->elements[idx].fbmp,
                       MON_INBAND_TELEMETRY_IPFIX_EXPORTt_INFORMATION_ELEMENT_IDENTIFIERf)) {
            dst->elements[idx].info_element_id = src->elements[idx].info_element_id;
        }
        sal_memcpy(&(dst->elements[idx].fbmp),
                   &(src->elements[idx].fbmp),
                   sizeof(dst->elements[idx].fbmp));
    }

    if (SHR_BITGET(src->fbmp,
                   MON_INBAND_TELEMETRY_IPFIX_EXPORTt_TX_INTERVALf)) {
        dst->tx_interval = src->tx_interval;
    }

    if (SHR_BITGET(src->fbmp,
                   MON_INBAND_TELEMETRY_IPFIX_EXPORTt_INITIAL_BURSTf)) {
        dst->initial_burst = src->initial_burst;
    }
}

static void
int_control_oper_state_set(int unit, int_control_oper_state_t oper_state)
{
    int_info_t *int_info = g_int_info[unit];

    int_info->ha->oper_states[oper_state] = true;
}

static void
int_control_oper_state_clear(int unit, int_control_oper_state_t oper_state)
{
    int_info_t *int_info = g_int_info[unit];

    int_info->ha->oper_states[oper_state] = false;
}

static void
int_control_oper_state_clear_all(int unit)
{
    int_info_t *int_info = g_int_info[unit];
    int idx;

    /* Initialize all the oper states to false. */
    for (idx = 0; idx < OPER_STATE_COUNT; idx++) {
        int_info->ha->oper_states[idx] = false;
    }
}

static void
int_control_oper_fields_set(uint16_t max_export_pkt_length,
                            uint16_t max_rx_pkt_length,
                            bcmcth_mon_int_control_oper_fields_t *oper)
{
    oper->max_export_pkt_length = max_export_pkt_length;
    oper->max_rx_pkt_length = max_rx_pkt_length;
}

static int
int_control_run_time_config_update(int unit,
                                   bcmcth_mon_int_control_entry_t *entry)
{
    int_info_t *int_info = g_int_info[unit];
    uint32_t params[MCS_IFA_MSG_CTRL_CONFIG_UPDATE_PARAM_COUNT];
    uint32_t values[MCS_IFA_MSG_CTRL_CONFIG_UPDATE_PARAM_COUNT][MCS_IFA_MSG_CTRL_CONFIG_UPDATE_MAX_VALUES];
    uint16_t num_values[MCS_IFA_MSG_CTRL_CONFIG_UPDATE_PARAM_COUNT];
    uint16_t num_params;

    SHR_FUNC_ENTER(unit);

    if (int_info->ha->run == false) {
        /* App is not running, return */
        SHR_EXIT();
    }

    num_params = 0;
    if (SHR_BITGET(entry->fbmp, MON_INBAND_TELEMETRY_CONTROLt_EXPORTf)) {
        params[num_params] =
            MCS_IFA_MSG_CTRL_CONFIG_UPDATE_PARAM_EXPORT_ENABLE;
        num_values[num_params] = 1;
        values[num_params][0] = (entry->export == true) ? 1 : 0;
        num_params++;
    }
    if (SHR_BITGET(entry->fbmp,
                   MON_INBAND_TELEMETRY_CONTROLt_RX_PKT_EXPORT_MAX_LENGTHf)) {
        params[num_params] =
            MCS_IFA_MSG_CTRL_CONFIG_UPDATE_PARAM_RX_PKT_EXPORT_MAX_LENGTH;
        num_values[num_params] = 1;
        values[num_params][0] = entry->rx_pkt_export_max_length;
        num_params++;
    }
    if (SHR_BITGET(entry->fbmp,
                   MON_INBAND_TELEMETRY_CONTROLt_DEVICE_IDENTIFIERf)) {
        params[num_params] =
            MCS_IFA_MSG_CTRL_CONFIG_UPDATE_PARAM_DEVICE_ID;
        num_values[num_params] = 1;
        values[num_params][0] = entry->device_id;
        num_params++;
    }

    if (num_params > 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (int_ifa_uc_config_update(unit, num_params,
                                      params, num_values, values));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
export_profile_defaults_get(int unit,
                            bcmcth_mon_export_profile_t *entry)
{
    bcmlrd_client_field_info_t *fields_info = NULL;
    size_t fld_num = MON_EXPORT_PROFILEt_FIELD_COUNT;
    size_t actual_fld_num;
    size_t idx;

    SHR_FUNC_ENTER(unit);

    SHR_ALLOC(fields_info, sizeof(bcmlrd_client_field_info_t) * fld_num,
              "bcmcthMonExportProfileFldInfo");
    SHR_NULL_CHECK(fields_info, SHR_E_MEMORY);

    SHR_IF_ERR_VERBOSE_EXIT
       (bcmlrd_table_fields_def_get(unit, "MON_EXPORT_PROFILE",
                                    fld_num, fields_info, &actual_fld_num));

    for (idx = 0; idx < actual_fld_num; idx++) {
        if (SHR_BITGET(entry->fbmp, fields_info[idx].id)) {
            /* The field is already configured, don't overwrite with default. */
            continue;
        }

        switch (fields_info[idx].id) {
            case MON_EXPORT_PROFILEt_WIRE_FORMATf:
                entry->wire_format = fields_info[idx].def.u8;
                break;

            case MON_EXPORT_PROFILEt_PACKET_LEN_INDICATORf:
                entry->packet_len_indicator = fields_info[idx].def.u8;
                break;

            case MON_EXPORT_PROFILEt_MAX_PKT_LENGTHf:
                entry->max_packet_length = fields_info[idx].def.u32;
                break;

            case MON_EXPORT_PROFILEt_NUM_RECORDSf:
                entry->num_records = fields_info[idx].def.u32;
                break;

            default:
                /* Don't care about other fields. */
                break;
        }
    }

exit:
    SHR_FREE(fields_info);
    SHR_FUNC_EXIT();
}

static void
export_profile_validate(int unit,
                        bcmcth_mon_export_profile_t *export_profile,
                        uint16_t max_export_pkt_length_oper,
                        bool *valid)
{
    *valid = true;

    if (export_profile->wire_format == BCMLTD_COMMON_WIRE_FORMAT_T_T_IPFIX) {
        /* Clear the oper state if it had been set previously. */
        int_control_oper_state_clear(unit,
                                     OPER_STATE_EXPORT_PROFILE_WIRE_FORMAT_NOT_SUPPORTED);
    } else {
        *valid = false;
        int_control_oper_state_set(unit,
                                   OPER_STATE_EXPORT_PROFILE_WIRE_FORMAT_NOT_SUPPORTED);
    }

    if (export_profile->packet_len_indicator ==
        BCMLTD_COMMON_EXPORT_PACKET_LEN_INDICATOR_T_T_MAX_PKT_LENGTH) {
        if ((export_profile->max_packet_length >= INT_IFA_MIN_EXPORT_PKT_LENGTH) &&
            (export_profile->max_packet_length <= max_export_pkt_length_oper)) {

            /* Clear the oper state if it had been set previously. */
            int_control_oper_state_clear(unit,
                                         OPER_STATE_EXPORT_PROFILE_INVALID_MAX_PKT_LENGTH);
        } else {
            *valid = false;
            int_control_oper_state_set(unit,
                                       OPER_STATE_EXPORT_PROFILE_INVALID_MAX_PKT_LENGTH);
        }
    }
}

/* Inspect the collector related fields in the entry and attempt to create the
 * collector. If the collector cannot be created then resolve the oper state.
 * This function needs to be called every time something collector related has
 * changed.
 */
static int
int_collector_config(int unit, bcmcth_mon_int_control_entry_t *entry,
                     bcmcth_mon_int_control_oper_fields_t *oper)
{
    int_info_t *int_info = g_int_info[unit];
    bool collector_ipv4_valid, collector_ipv6_valid;
    bool export_profile_valid;

    SHR_FUNC_ENTER(unit);

    collector_ipv4_valid = collector_ipv6_valid = false;

    switch (entry->collector_type) {

        case BCMLTD_COMMON_MON_INBAND_TELEMETRY_CONTROL_COLLECTOR_TYPE_T_T_IPV4:
            if (entry->collector_ipv4_found == true) {
                collector_ipv4_valid = true;
                int_control_oper_state_clear(unit,
                                             OPER_STATE_COLLECTOR_NOT_EXISTS);
            } else {
                if (entry->collector_ipv4_id == INT_INVALID_COLLECTOR_ID) {
                    /* Collector either not created or being deleted. */
                    int_control_oper_state_clear(unit,
                                                 OPER_STATE_COLLECTOR_NOT_EXISTS);
                } else {
                    int_control_oper_state_set(unit,
                                               OPER_STATE_COLLECTOR_NOT_EXISTS);
                }
            }
            break;

        case BCMLTD_COMMON_MON_INBAND_TELEMETRY_CONTROL_COLLECTOR_TYPE_T_T_IPV6:
            if (entry->collector_ipv6_found == true) {
                collector_ipv6_valid = true;
                int_control_oper_state_clear(unit,
                                             OPER_STATE_COLLECTOR_NOT_EXISTS);
            } else {
                if (entry->collector_ipv6_id == INT_INVALID_COLLECTOR_ID) {
                    /* Collector either not created or being deleted. */
                    int_control_oper_state_clear(unit,
                                                 OPER_STATE_COLLECTOR_NOT_EXISTS);
                } else {
                    int_control_oper_state_set(unit,
                                               OPER_STATE_COLLECTOR_NOT_EXISTS);
                }
            }
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    export_profile_valid = false;
    if (entry->export_profile_found == true) {

        int_control_oper_state_clear(unit,
                                     OPER_STATE_EXPORT_PROFILE_NOT_EXISTS);

        /* Get the defaults for values that are not configured. */
        SHR_IF_ERR_VERBOSE_EXIT
            (export_profile_defaults_get(unit, &(entry->export_profile)));

        /* Validate the export profile and set the oper states if there is a
         * failure.
         */
        export_profile_validate(unit, &(entry->export_profile),
                                oper->max_export_pkt_length,
                                &export_profile_valid);

    } else {
        if (entry->export_profile_id == INT_INVALID_EXPORT_PROFILE_ID) {
            /*
             * Export profile either not created or being deleted. Clear all
             * export profile related oper states.
             */
            int_control_oper_state_clear(unit,
                                         OPER_STATE_EXPORT_PROFILE_NOT_EXISTS);
            int_control_oper_state_clear(unit,
                                         OPER_STATE_EXPORT_PROFILE_WIRE_FORMAT_NOT_SUPPORTED);
            int_control_oper_state_clear(unit,
                                         OPER_STATE_EXPORT_PROFILE_INVALID_MAX_PKT_LENGTH);
        } else {
            /* Export profile Id configured, but entry does not exist. */
            int_control_oper_state_set(unit,
                                       OPER_STATE_EXPORT_PROFILE_NOT_EXISTS);
        }
    }

    /* Create the collector in the app if all the conditions are met. */
    if (int_info->ha->run == true) {
        if (export_profile_valid == true) {
            if (collector_ipv4_valid == true) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (int_ifa_uc_collector_ipv4_create(unit,
                                                      &entry->collector_ipv4,
                                                      &entry->export_profile));
            } else if (collector_ipv6_valid == true) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (int_ifa_uc_collector_ipv6_create(unit,
                                                      &entry->collector_ipv6,
                                                      &entry->export_profile));
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_int_control_insert(int unit,
                              uint32_t trans_id,
                              bcmcth_mon_int_control_entry_t *insert,
                              bcmcth_mon_int_control_oper_fields_t *oper,
                              bool *replay)
{
    int_info_t *int_info = g_int_info[unit];
    bcmcth_mon_int_control_entry_t entry;

    SHR_FUNC_ENTER(unit);

    /* Initializations */
    sal_memset(oper, 0, sizeof(*oper));
    sal_memset(&entry, 0, sizeof(entry));
    *replay = false;

    /* Get the defaults for the fields that are not configured. */
    SHR_IF_ERR_VERBOSE_EXIT(int_control_defaults_get(unit, &entry));

    /* Overwrite the default values with the fields being inserted. */
    int_control_fields_copy(&entry, insert);


    if (entry.inband_telemetry == true) {
        /* Initialize the app */
        SHR_IF_ERR_VERBOSE_EXIT(int_ifa_uc_init(unit, trans_id, &entry));
    }

    if (int_info->ha->run == true) {
        /* The app has succesfully initialized */

        /* Set the oper field values */
        int_control_oper_fields_set(entry.max_export_pkt_length,
                                    entry.max_rx_pkt_length,
                                    oper);

        /* Send the remaining parameters to the app. */
        SHR_IF_ERR_VERBOSE_EXIT
            (int_control_run_time_config_update(unit, &entry));

        /* Enable replay of other config as the app has been initialized. */
        *replay = true;
    } else {
        /* App initalization was not succesful, set the corresponding
         * oper state.
         */
        int_control_oper_state_set(unit, OPER_STATE_NOT_INITIALIZED);
    }

    /* Try to create the collector if all the information is present. Set oper
     * states in case only partial information is available or if there is a
     * validation fail.
     */
    SHR_IF_ERR_VERBOSE_EXIT(int_collector_config(unit, &entry, oper));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_int_control_update(int unit,
                              uint32_t trans_id,
                              bcmcth_mon_int_control_entry_t *cur,
                              bcmcth_mon_int_control_entry_t *update,
                              bcmcth_mon_int_control_oper_fields_t *oper,
                              bool *replay)
{
    int_info_t *int_info = g_int_info[unit];
    bcmcth_mon_int_control_entry_t old_entry, entry;

    SHR_FUNC_ENTER(unit);

    /* Initializations */
    sal_memcpy(oper, &(cur->oper), sizeof(*oper));
    sal_memset(&old_entry, 0, sizeof(old_entry));
    sal_memset(&entry, 0, sizeof(entry));
    *replay = false;

    /* Get the defaults for the fields that are not configured. */
    SHR_IF_ERR_VERBOSE_EXIT(int_control_defaults_get(unit, &old_entry));
    SHR_IF_ERR_VERBOSE_EXIT(int_control_defaults_get(unit, &entry));

    /* Overwrite the default fields with the values that are already configured
     * in IMM.
     */
    int_control_fields_copy(&old_entry, cur);
    int_control_fields_copy(&entry, cur);

    /* Overwrite the default/old fields with the new values that are being
     * configured.
     */
    int_control_fields_copy(&entry, update);

    /* old_entry now contains the view of the table entry as it currently exists
     * (Defaults + User configured). entry now contains the view of the table
     * entry as it should be once the update is complete.
     */

    if (entry.inband_telemetry != int_info->ha->init) {
        /*
         * The app's init status is toggling, check if it is init or shutdown.
         */
        if (entry.inband_telemetry == true) {
            /* App init requested */
            SHR_IF_ERR_VERBOSE_EXIT(int_ifa_uc_init(unit, trans_id, &entry));

            /* Set the oper fields */
            int_control_oper_fields_set(entry.max_export_pkt_length,
                                        entry.max_rx_pkt_length,
                                        oper);

            if (int_info->ha->run == true) {
                /* The app has succesfully initialized, flip the corresponding
                 * oper state.
                 */
                int_control_oper_state_clear(unit, OPER_STATE_NOT_INITIALIZED);

                /* Enable replay of other config as the app has been initialized. */
                *replay = true;
            }
        } else {
            /* Shutdown requested */
            SHR_IF_ERR_VERBOSE_EXIT(int_ifa_uc_shutdown(unit));

            /* Set the oper fields to 0 as the app is shutdown */
            int_control_oper_fields_set(0, 0, oper);

            /* App is not initalized anymore, set the corresponding
             * oper state
             */
            int_control_oper_state_set(unit, OPER_STATE_NOT_INITIALIZED);
        }
    }

    /* Send the run-time config that have changed to the app. */
    SHR_IF_ERR_VERBOSE_EXIT(int_control_run_time_config_update(unit, &entry));

    /* Collector and export profile update handling. */
    if (int_info->ha->collector == true) {
        /* The collector is created in the app, check if it needs to be deleted
         * and recreated.
         */
        bool collector_delete = false;

        if (entry.collector_type != old_entry.collector_type) {
            collector_delete = true;
        } else {
            switch (entry.collector_type) {
                case BCMLTD_COMMON_MON_INBAND_TELEMETRY_CONTROL_COLLECTOR_TYPE_T_T_IPV4:
                    if (entry.collector_ipv4_id != old_entry.collector_ipv4_id) {
                        collector_delete = true;
                    }
                    break;

                case BCMLTD_COMMON_MON_INBAND_TELEMETRY_CONTROL_COLLECTOR_TYPE_T_T_IPV6:
                    if (entry.collector_ipv6_id != old_entry.collector_ipv6_id) {
                        collector_delete = true;
                    }
                    break;

                default:
                    SHR_ERR_EXIT(SHR_E_PARAM);
            }
        }
        if (entry.export_profile_id != old_entry.export_profile_id) {
            collector_delete = true;
        }
        if (collector_delete == true) {
            SHR_IF_ERR_VERBOSE_EXIT(int_ifa_uc_collector_delete(unit));
            int_info->ha->collector = false;
        }
    }

    if (int_info->ha->collector == false) {
        /* See if the collector can be created in the app. If the collector is
         * already created, then don't bother as it implies nothing collector
         * related has changed.
         */

        /* Create the collector or set the appropriate oper state if
         * applicable.
         */
        SHR_IF_ERR_VERBOSE_EXIT(int_collector_config(unit, &entry, oper));
    }
exit:
    SHR_FUNC_EXIT();
}

bcmltd_common_mon_inband_telemetry_control_state_t_t
bcmcth_mon_int_control_oper_state_get(int unit)
{
    int_info_t *int_info = g_int_info[unit];
    int idx;

    for (idx = 0; idx < OPER_STATE_COUNT; idx++) {
        if (int_info->ha->oper_states[idx] == true) {
            return int_oper_states[idx];
        }
    }

    /* Return success if nothing is set as it indicates that there is no
     * problem.
     */
    return BCMLTD_COMMON_MON_INBAND_TELEMETRY_CONTROL_STATE_T_T_SUCCESS;
}

int
bcmcth_mon_int_control_delete(int unit)
{
    int_info_t *int_info = g_int_info[unit];

    SHR_FUNC_ENTER(unit);

    if (int_info->ha->init == true) {
        /* Shutdown the app */
        SHR_IF_ERR_VERBOSE_EXIT(int_ifa_uc_shutdown(unit));
    }

    /* Clear all the operational states. */
    int_control_oper_state_clear_all(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_int_ipfix_export_insert(int unit,
                                   bcmcth_mon_int_ipfix_export_entry_t *insert,
                                   bcmltd_common_mon_inband_telemetry_ipfix_export_state_t_t *oper_state)
{
    int_info_t *int_info = g_int_info[unit];
    bcmcth_mon_int_ipfix_export_entry_t entry;

    SHR_FUNC_ENTER(unit);

    /* Initializations */
    *oper_state = BCMLTD_COMMON_MON_INBAND_TELEMETRY_IPFIX_EXPORT_STATE_T_T_SUCCESS;
    sal_memset(&entry, 0, sizeof(entry));

    if (int_info->ha->run == false) {
        *oper_state = BCMLTD_COMMON_MON_INBAND_TELEMETRY_IPFIX_EXPORT_STATE_T_T_APP_NOT_INITIALIZED;
        SHR_EXIT();
    }

    /* Get the defaults for the fields that are not configured. */
    SHR_IF_ERR_VERBOSE_EXIT(int_ipfix_export_defaults_get(unit, &entry));

    /* Overwrite the default values with the fields being inserted. */
    int_ipfix_export_fields_copy(&entry, insert);

    /* Send the message to app. */
    SHR_IF_ERR_VERBOSE_EXIT(int_ifa_uc_ipfix_template_set(unit, &entry));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_int_ipfix_export_update(int unit,
                                   bcmcth_mon_int_ipfix_export_entry_t *cur,
                                   bcmcth_mon_int_ipfix_export_entry_t *update,
                                   bcmltd_common_mon_inband_telemetry_ipfix_export_state_t_t *oper_state)
{
    int_info_t *int_info = g_int_info[unit];
    bcmcth_mon_int_ipfix_export_entry_t entry;

    SHR_FUNC_ENTER(unit);

    /* Initializations */
    *oper_state = BCMLTD_COMMON_MON_INBAND_TELEMETRY_IPFIX_EXPORT_STATE_T_T_SUCCESS;
    sal_memset(&entry, 0, sizeof(entry));

    if (int_info->ha->run == false) {
        *oper_state = BCMLTD_COMMON_MON_INBAND_TELEMETRY_IPFIX_EXPORT_STATE_T_T_APP_NOT_INITIALIZED;
        SHR_EXIT();
    }
    *oper_state = BCMLTD_COMMON_MON_INBAND_TELEMETRY_IPFIX_EXPORT_STATE_T_T_SUCCESS;

    /* Get the defaults for the fields that are not configured. */
    SHR_IF_ERR_VERBOSE_EXIT(int_ipfix_export_defaults_get(unit, &entry));

    /* Overwrite the default fields with the values that are already configured
     * in IMM.
     */
    int_ipfix_export_fields_copy(&entry, cur);

    /*
     * Overwrite the default/old fields with the new values that are being
     * configured.
     */
    int_ipfix_export_fields_copy(&entry, update);

    /* Send the message to app. */
    SHR_IF_ERR_VERBOSE_EXIT(int_ifa_uc_ipfix_template_set(unit, &entry));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_int_ipfix_export_delete(int unit)
{
    int_info_t *int_info = g_int_info[unit];
    bcmcth_mon_int_ipfix_export_entry_t entry;

    SHR_FUNC_ENTER(unit);

    /* Initializations */
    sal_memset(&entry, 0, sizeof(entry));

    if (int_info->ha->run == false) {
        SHR_EXIT();
    }

    /* Send the message to app. */
    SHR_IF_ERR_VERBOSE_EXIT(int_ifa_uc_ipfix_template_set(unit, &entry));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_int_ipfix_export_replay(int unit,
                                   bcmcth_mon_int_ipfix_export_entry_t *entry,
                                   bcmltd_common_mon_inband_telemetry_ipfix_export_state_t_t *oper_state)
{
    SHR_FUNC_ENTER(unit);

    /* The logic is same for both insert and replay. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_int_ipfix_export_insert(unit, entry, oper_state));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_int_stats_update(int unit, bcmcth_mon_int_stats_t *entry)
{
    int_info_t *int_info = g_int_info[unit];

    SHR_FUNC_ENTER(unit);

    if (int_info->ha->run == false) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT(int_ifa_uc_stats_set(unit, entry));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_int_stats_lookup(int unit, bcmcth_mon_int_stats_t *entry)
{
    int_info_t *int_info = g_int_info[unit];

    SHR_FUNC_ENTER(unit);

    if (int_info->ha->run == false) {
        sal_memset(entry, 0, sizeof(*entry));
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT(int_ifa_uc_stats_get(unit, entry));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_int_collector_ipv4_insert(int unit,
                                     bcmcth_mon_int_control_entry_t *int_entry,
                                     bcmcth_mon_collector_ipv4_t *insert)
{
    bcmcth_mon_int_control_entry_t entry;

    SHR_FUNC_ENTER(unit);

    sal_memset(&entry, 0, sizeof(entry));

    /* Get the current full view of the IMM entry. */
    sal_memcpy(&entry, int_entry, sizeof(entry));
    SHR_IF_ERR_VERBOSE_EXIT(int_control_defaults_get(unit, &entry));

    if ((entry.collector_type != BCMLTD_COMMON_MON_INBAND_TELEMETRY_CONTROL_COLLECTOR_TYPE_T_T_IPV4) ||
        (entry.collector_ipv4_id != insert->collector_ipv4_id)) {
        /* This collector entry is not being used, return. */
        SHR_EXIT();
    }

    entry.collector_ipv4_found = true;
    sal_memcpy(&(entry.collector_ipv4), insert, sizeof(entry.collector_ipv4));

    SHR_IF_ERR_VERBOSE_EXIT(int_collector_config(unit, &entry, &(entry.oper)));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_int_collector_ipv4_update(int unit,
                                     bcmcth_mon_int_control_entry_t *int_entry,
                                     bcmcth_mon_collector_ipv4_t *update)
{
    int_info_t *int_info = g_int_info[unit];
    bcmcth_mon_int_control_entry_t entry;
    bool change;

    SHR_FUNC_ENTER(unit);

    sal_memset(&entry, 0, sizeof(entry));

    /* Get the current full view of the IMM entry. */
    sal_memcpy(&entry, int_entry, sizeof(entry));
    SHR_IF_ERR_VERBOSE_EXIT(int_control_defaults_get(unit, &entry));

    if ((entry.collector_type != BCMLTD_COMMON_MON_INBAND_TELEMETRY_CONTROL_COLLECTOR_TYPE_T_T_IPV4) ||
        (entry.collector_ipv4_id != update->collector_ipv4_id)) {
        /* This collector entry is not being used, return. */
        SHR_EXIT();
    }


    /* Update the modified collector fields. */
    change = false;
    if (SHR_BITGET(update->fbmp, MON_COLLECTOR_IPV4t_TAG_STRUCTUREf)) {
        SHR_BITSET(entry.collector_ipv4.fbmp, MON_COLLECTOR_IPV4t_TAG_STRUCTUREf);
        entry.collector_ipv4.tag_type = update->tag_type;
        change = true;
    }
    if (SHR_BITGET(update->fbmp, MON_COLLECTOR_IPV4t_DST_MACf)) {
        SHR_BITSET(entry.collector_ipv4.fbmp, MON_COLLECTOR_IPV4t_DST_MACf);
        sal_memcpy(&(entry.collector_ipv4.dst_mac), update->dst_mac,
                   sizeof(shr_mac_t));
        change = true;
    }
    if (SHR_BITGET(update->fbmp, MON_COLLECTOR_IPV4t_SRC_MACf)) {
        SHR_BITSET(entry.collector_ipv4.fbmp, MON_COLLECTOR_IPV4t_SRC_MACf);
        sal_memcpy(&(entry.collector_ipv4.src_mac), update->src_mac,
                   sizeof(shr_mac_t));
        change = true;
    }
    if (SHR_BITGET(update->fbmp, MON_COLLECTOR_IPV4t_TPIDf)) {
        SHR_BITSET(entry.collector_ipv4.fbmp, MON_COLLECTOR_IPV4t_TPIDf);
        entry.collector_ipv4.outer_vlan_tag.tpid = update->outer_vlan_tag.tpid;
        change = true;
    }
    if (SHR_BITGET(update->fbmp, MON_COLLECTOR_IPV4t_VLAN_IDf)) {
        SHR_BITSET(entry.collector_ipv4.fbmp, MON_COLLECTOR_IPV4t_VLAN_IDf);
        entry.collector_ipv4.outer_vlan_tag.tci.vid =
            update->outer_vlan_tag.tci.vid;
        change = true;
    }
    if (SHR_BITGET(update->fbmp, MON_COLLECTOR_IPV4t_CFIf)) {
        SHR_BITSET(entry.collector_ipv4.fbmp, MON_COLLECTOR_IPV4t_CFIf);
        entry.collector_ipv4.outer_vlan_tag.tci.cfi =
            update->outer_vlan_tag.tci.cfi;
        change = true;
    }
    if (SHR_BITGET(update->fbmp, MON_COLLECTOR_IPV4t_PRIf)) {
        SHR_BITSET(entry.collector_ipv4.fbmp, MON_COLLECTOR_IPV4t_PRIf);
        entry.collector_ipv4.outer_vlan_tag.tci.prio =
            update->outer_vlan_tag.tci.prio;
        change = true;
    }
    if (SHR_BITGET(update->fbmp, MON_COLLECTOR_IPV4t_INNER_TPIDf)) {
        SHR_BITSET(entry.collector_ipv4.fbmp, MON_COLLECTOR_IPV4t_INNER_TPIDf);
        entry.collector_ipv4.inner_vlan_tag.tpid = update->inner_vlan_tag.tpid;
        change = true;
    }
    if (SHR_BITGET(update->fbmp, MON_COLLECTOR_IPV4t_INNER_VLAN_IDf)) {
        SHR_BITSET(entry.collector_ipv4.fbmp, MON_COLLECTOR_IPV4t_INNER_VLAN_IDf);
        entry.collector_ipv4.inner_vlan_tag.tci.vid =
            update->inner_vlan_tag.tci.vid;
        change = true;
    }
    if (SHR_BITGET(update->fbmp, MON_COLLECTOR_IPV4t_INNER_CFIf)) {
        SHR_BITSET(entry.collector_ipv4.fbmp, MON_COLLECTOR_IPV4t_INNER_CFIf);
        entry.collector_ipv4.inner_vlan_tag.tci.cfi =
            update->inner_vlan_tag.tci.cfi;
        change = true;
    }
    if (SHR_BITGET(update->fbmp, MON_COLLECTOR_IPV4t_INNER_PRIf)) {
        SHR_BITSET(entry.collector_ipv4.fbmp, MON_COLLECTOR_IPV4t_INNER_PRIf);
        entry.collector_ipv4.inner_vlan_tag.tci.prio =
            update->inner_vlan_tag.tci.prio;
        change = true;
    }
    if (SHR_BITGET(update->fbmp, MON_COLLECTOR_IPV4t_DST_IPV4f)) {
        SHR_BITSET(entry.collector_ipv4.fbmp, MON_COLLECTOR_IPV4t_DST_IPV4f);
        entry.collector_ipv4.dst_ip = update->dst_ip;
        change = true;
    }
    if (SHR_BITGET(update->fbmp, MON_COLLECTOR_IPV4t_SRC_IPV4f)) {
        SHR_BITSET(entry.collector_ipv4.fbmp, MON_COLLECTOR_IPV4t_SRC_IPV4f);
        entry.collector_ipv4.src_ip =  update->src_ip;
        change = true;
    }
    if (SHR_BITGET(update->fbmp, MON_COLLECTOR_IPV4t_TOSf)) {
        SHR_BITSET(entry.collector_ipv4.fbmp, MON_COLLECTOR_IPV4t_TOSf);
        entry.collector_ipv4.tos = update->tos;
        change = true;
    }
    if (SHR_BITGET(update->fbmp, MON_COLLECTOR_IPV4t_TTLf)) {
        SHR_BITSET(entry.collector_ipv4.fbmp, MON_COLLECTOR_IPV4t_TTLf);
        entry.collector_ipv4.ttl = update->ttl;
        change = true;
    }
    if (SHR_BITGET(update->fbmp, MON_COLLECTOR_IPV4t_DST_L4_UDP_PORTf)) {
        SHR_BITSET(entry.collector_ipv4.fbmp, MON_COLLECTOR_IPV4t_DST_L4_UDP_PORTf);
        entry.collector_ipv4.dst_port = update->dst_port;
        change = true;
    }
    if (SHR_BITGET(update->fbmp, MON_COLLECTOR_IPV4t_SRC_L4_UDP_PORTf)) {
        SHR_BITSET(entry.collector_ipv4.fbmp, MON_COLLECTOR_IPV4t_SRC_L4_UDP_PORTf);
        entry.collector_ipv4.src_port = update->src_port;
        change = true;
    }
    if (SHR_BITGET(update->fbmp, MON_COLLECTOR_IPV4t_UDP_CHKSUMf)) {
        SHR_BITSET(entry.collector_ipv4.fbmp, MON_COLLECTOR_IPV4t_UDP_CHKSUMf);
        entry.collector_ipv4.udp_chksum = update->udp_chksum;
        change = true;
    }
    if (SHR_BITGET(update->fbmp, MON_COLLECTOR_IPV4t_OBSERVATION_DOMAINf)) {
        SHR_BITSET(entry.collector_ipv4.fbmp, MON_COLLECTOR_IPV4t_OBSERVATION_DOMAINf);
        entry.collector_ipv4.observation_domain = update->observation_domain;
        change = true;
    }

    if (change == false) {
        /* Nothing we care about has changed, ignore the update. */
        SHR_EXIT();
    }

    /* Something has changed, delete the collector and re-add. */
    if (int_info->ha->collector == true) {
        SHR_IF_ERR_VERBOSE_EXIT(int_ifa_uc_collector_delete(unit));
    }

    SHR_IF_ERR_VERBOSE_EXIT(int_collector_config(unit, &entry, &(entry.oper)));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_int_collector_ipv4_delete(int unit,
                                     bcmcth_mon_int_control_entry_t *int_entry,
                                     uint32_t collector_id)
{
    int_info_t *int_info = g_int_info[unit];
    bcmcth_mon_int_control_entry_t entry;

    SHR_FUNC_ENTER(unit);

    sal_memset(&entry, 0, sizeof(entry));

    /* Get the current full view of the IMM entry. */
    sal_memcpy(&entry, int_entry, sizeof(entry));
    SHR_IF_ERR_VERBOSE_EXIT(int_control_defaults_get(unit, &entry));

    if ((entry.collector_type != BCMLTD_COMMON_MON_INBAND_TELEMETRY_CONTROL_COLLECTOR_TYPE_T_T_IPV4) ||
        (entry.collector_ipv4_id != collector_id)) {
        /* This collector entry is not being used, return. */
        SHR_EXIT();
    }


    /* Delete the collector in the app. */
    if (int_info->ha->collector == true) {
        SHR_IF_ERR_VERBOSE_EXIT(int_ifa_uc_collector_delete(unit));
    }

    entry.collector_ipv4_found = false;
    sal_memset(&(entry.collector_ipv4), 0, sizeof(entry.collector_ipv4));
    SHR_IF_ERR_VERBOSE_EXIT(int_collector_config(unit, &entry, &(entry.oper)));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_int_collector_ipv6_insert(int unit,
                                     bcmcth_mon_int_control_entry_t *int_entry,
                                     bcmcth_mon_collector_ipv6_t *insert)
{
    bcmcth_mon_int_control_entry_t entry;

    SHR_FUNC_ENTER(unit);

    sal_memset(&entry, 0, sizeof(entry));

    /* Get the current full view of the IMM entry. */
    sal_memcpy(&entry, int_entry, sizeof(entry));
    SHR_IF_ERR_VERBOSE_EXIT(int_control_defaults_get(unit, &entry));

    if ((entry.collector_type != BCMLTD_COMMON_MON_INBAND_TELEMETRY_CONTROL_COLLECTOR_TYPE_T_T_IPV6) ||
        (entry.collector_ipv6_id != insert->collector_ipv6_id)) {
        /* This collector entry is not being used, return. */
        SHR_EXIT();
    }

    entry.collector_ipv6_found = true;
    sal_memcpy(&(entry.collector_ipv6), insert, sizeof(entry.collector_ipv6));

    SHR_IF_ERR_VERBOSE_EXIT(int_collector_config(unit, &entry, &(entry.oper)));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_int_collector_ipv6_update(int unit,
                                     bcmcth_mon_int_control_entry_t *int_entry,
                                     bcmcth_mon_collector_ipv6_t *update)
{
    int_info_t *int_info = g_int_info[unit];
    bcmcth_mon_int_control_entry_t entry;
    bool change;

    SHR_FUNC_ENTER(unit);

    sal_memset(&entry, 0, sizeof(entry));

    /* Get the current full view of the IMM entry. */
    sal_memcpy(&entry, int_entry, sizeof(entry));
    SHR_IF_ERR_VERBOSE_EXIT(int_control_defaults_get(unit, &entry));

    if ((entry.collector_type != BCMLTD_COMMON_MON_INBAND_TELEMETRY_CONTROL_COLLECTOR_TYPE_T_T_IPV6) ||
        (entry.collector_ipv6_id != update->collector_ipv6_id)) {
        /* This collector entry is not being used, return. */
        SHR_EXIT();
    }


    /* Update the modified collector fields. */
    change = false;
    if (SHR_BITGET(update->fbmp, MON_COLLECTOR_IPV6t_TAG_STRUCTUREf)) {
        SHR_BITSET(entry.collector_ipv6.fbmp, MON_COLLECTOR_IPV6t_TAG_STRUCTUREf);
        entry.collector_ipv6.tag_type = update->tag_type;
        change = true;
    }
    if (SHR_BITGET(update->fbmp, MON_COLLECTOR_IPV6t_DST_MACf)) {
        SHR_BITSET(entry.collector_ipv6.fbmp, MON_COLLECTOR_IPV6t_DST_MACf);
        sal_memcpy(&(entry.collector_ipv6.dst_mac), update->dst_mac,
                   sizeof(shr_mac_t));
        change = true;
    }
    if (SHR_BITGET(update->fbmp, MON_COLLECTOR_IPV6t_SRC_MACf)) {
        SHR_BITSET(entry.collector_ipv6.fbmp, MON_COLLECTOR_IPV6t_SRC_MACf);
        sal_memcpy(&(entry.collector_ipv6.src_mac), update->src_mac,
                   sizeof(shr_mac_t));
        change = true;
    }
    if (SHR_BITGET(update->fbmp, MON_COLLECTOR_IPV6t_TPIDf)) {
        SHR_BITSET(entry.collector_ipv6.fbmp, MON_COLLECTOR_IPV6t_TPIDf);
        entry.collector_ipv6.outer_vlan_tag.tpid = update->outer_vlan_tag.tpid;
        change = true;
    }
    if (SHR_BITGET(update->fbmp, MON_COLLECTOR_IPV6t_VLAN_IDf)) {
        SHR_BITSET(entry.collector_ipv6.fbmp, MON_COLLECTOR_IPV6t_VLAN_IDf);
        entry.collector_ipv6.outer_vlan_tag.tci.vid =
            update->outer_vlan_tag.tci.vid;
        change = true;
    }
    if (SHR_BITGET(update->fbmp, MON_COLLECTOR_IPV6t_CFIf)) {
        SHR_BITSET(entry.collector_ipv6.fbmp, MON_COLLECTOR_IPV6t_CFIf);
        entry.collector_ipv6.outer_vlan_tag.tci.cfi =
            update->outer_vlan_tag.tci.cfi;
        change = true;
    }
    if (SHR_BITGET(update->fbmp, MON_COLLECTOR_IPV6t_PRIf)) {
        SHR_BITSET(entry.collector_ipv6.fbmp, MON_COLLECTOR_IPV6t_PRIf);
        entry.collector_ipv6.outer_vlan_tag.tci.prio =
            update->outer_vlan_tag.tci.prio;
        change = true;
    }
    if (SHR_BITGET(update->fbmp, MON_COLLECTOR_IPV6t_INNER_TPIDf)) {
        SHR_BITSET(entry.collector_ipv6.fbmp, MON_COLLECTOR_IPV6t_INNER_TPIDf);
        entry.collector_ipv6.inner_vlan_tag.tpid = update->inner_vlan_tag.tpid;
        change = true;
    }
    if (SHR_BITGET(update->fbmp, MON_COLLECTOR_IPV6t_INNER_VLAN_IDf)) {
        SHR_BITSET(entry.collector_ipv6.fbmp, MON_COLLECTOR_IPV6t_INNER_VLAN_IDf);
        entry.collector_ipv6.inner_vlan_tag.tci.vid =
            update->inner_vlan_tag.tci.vid;
        change = true;
    }
    if (SHR_BITGET(update->fbmp, MON_COLLECTOR_IPV6t_INNER_CFIf)) {
        SHR_BITSET(entry.collector_ipv6.fbmp, MON_COLLECTOR_IPV6t_INNER_CFIf);
        entry.collector_ipv6.inner_vlan_tag.tci.cfi =
            update->inner_vlan_tag.tci.cfi;
        change = true;
    }
    if (SHR_BITGET(update->fbmp, MON_COLLECTOR_IPV6t_INNER_PRIf)) {
        SHR_BITSET(entry.collector_ipv6.fbmp, MON_COLLECTOR_IPV6t_INNER_PRIf);
        entry.collector_ipv6.inner_vlan_tag.tci.prio =
            update->inner_vlan_tag.tci.prio;
        change = true;
    }
    if (SHR_BITGET(update->fbmp, MON_COLLECTOR_IPV6t_DST_IPV6u_UPPERf) ||
        SHR_BITGET(update->fbmp, MON_COLLECTOR_IPV6t_DST_IPV6u_LOWERf)) {
        SHR_BITSET(entry.collector_ipv6.fbmp, MON_COLLECTOR_IPV6t_DST_IPV6u_UPPERf);
        SHR_BITSET(entry.collector_ipv6.fbmp, MON_COLLECTOR_IPV6t_DST_IPV6u_LOWERf);
        sal_memcpy(entry.collector_ipv6.dst_ip6_addr, update->dst_ip6_addr,
                   sizeof(shr_ip6_t));
        change = true;
    }
    if (SHR_BITGET(update->fbmp, MON_COLLECTOR_IPV6t_SRC_IPV6u_UPPERf) ||
        SHR_BITGET(update->fbmp, MON_COLLECTOR_IPV6t_SRC_IPV6u_LOWERf)) {
        SHR_BITSET(entry.collector_ipv6.fbmp, MON_COLLECTOR_IPV6t_SRC_IPV6u_UPPERf);
        SHR_BITSET(entry.collector_ipv6.fbmp, MON_COLLECTOR_IPV6t_SRC_IPV6u_LOWERf);
        sal_memcpy(entry.collector_ipv6.src_ip6_addr, update->src_ip6_addr,
                   sizeof(shr_ip6_t));
        change = true;
    }
    if (SHR_BITGET(update->fbmp, MON_COLLECTOR_IPV6t_TOSf)) {
        SHR_BITSET(entry.collector_ipv6.fbmp, MON_COLLECTOR_IPV6t_TOSf);
        entry.collector_ipv6.tos = update->tos;
        change = true;
    }
    if (SHR_BITGET(update->fbmp, MON_COLLECTOR_IPV6t_TTLf)) {
        SHR_BITSET(entry.collector_ipv6.fbmp, MON_COLLECTOR_IPV6t_TTLf);
        entry.collector_ipv6.ttl = update->ttl;
        change = true;
    }
    if (SHR_BITGET(update->fbmp, MON_COLLECTOR_IPV6t_FLOW_LABELf)) {
        SHR_BITSET(entry.collector_ipv6.fbmp, MON_COLLECTOR_IPV6t_FLOW_LABELf);
        entry.collector_ipv6.flow_label = update->flow_label;
        change = true;
    }
    if (SHR_BITGET(update->fbmp, MON_COLLECTOR_IPV6t_DST_L4_UDP_PORTf)) {
        SHR_BITSET(entry.collector_ipv6.fbmp, MON_COLLECTOR_IPV6t_DST_L4_UDP_PORTf);
        entry.collector_ipv6.dst_port = update->dst_port;
        change = true;
    }
    if (SHR_BITGET(update->fbmp, MON_COLLECTOR_IPV6t_SRC_L4_UDP_PORTf)) {
        SHR_BITSET(entry.collector_ipv6.fbmp, MON_COLLECTOR_IPV6t_SRC_L4_UDP_PORTf);
        entry.collector_ipv6.src_port = update->src_port;
        change = true;
    }
    if (SHR_BITGET(update->fbmp, MON_COLLECTOR_IPV6t_UDP_CHKSUMf)) {
        SHR_BITSET(entry.collector_ipv6.fbmp, MON_COLLECTOR_IPV6t_UDP_CHKSUMf);
        entry.collector_ipv6.udp_chksum = update->udp_chksum;
        change = true;
    }
    if (SHR_BITGET(update->fbmp, MON_COLLECTOR_IPV6t_OBSERVATION_DOMAINf)) {
        SHR_BITSET(entry.collector_ipv6.fbmp, MON_COLLECTOR_IPV6t_OBSERVATION_DOMAINf);
        entry.collector_ipv6.observation_domain = update->observation_domain;
        change = true;
    }

    if (change == false) {
        /* Nothing we care about has changed, ignore the update. */
        SHR_EXIT();
    }

    /* Something has changed, delete the collector and re-add. */
    if (int_info->ha->collector == true) {
        SHR_IF_ERR_VERBOSE_EXIT(int_ifa_uc_collector_delete(unit));
    }

    SHR_IF_ERR_VERBOSE_EXIT(int_collector_config(unit, &entry, &(entry.oper)));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_int_collector_ipv6_delete(int unit,
                                     bcmcth_mon_int_control_entry_t *int_entry,
                                     uint32_t collector_id)
{
    int_info_t *int_info = g_int_info[unit];
    bcmcth_mon_int_control_entry_t entry;

    SHR_FUNC_ENTER(unit);

    sal_memset(&entry, 0, sizeof(entry));

    /* Get the current full view of the IMM entry. */
    sal_memcpy(&entry, int_entry, sizeof(entry));
    SHR_IF_ERR_VERBOSE_EXIT(int_control_defaults_get(unit, &entry));

    if ((entry.collector_type != BCMLTD_COMMON_MON_INBAND_TELEMETRY_CONTROL_COLLECTOR_TYPE_T_T_IPV6) ||
        (entry.collector_ipv6_id != collector_id)) {
        /* This collector entry is not being used, return. */
        SHR_EXIT();
    }


    /* Delete the collector in the app. */
    if (int_info->ha->collector == true) {
        SHR_IF_ERR_VERBOSE_EXIT(int_ifa_uc_collector_delete(unit));
    }

    entry.collector_ipv6_found = false;
    sal_memset(&(entry.collector_ipv6), 0, sizeof(entry.collector_ipv6));
    SHR_IF_ERR_VERBOSE_EXIT(int_collector_config(unit, &entry, &(entry.oper)));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_int_export_profile_insert(int unit,
                                     bcmcth_mon_int_control_entry_t *int_entry,
                                     bcmcth_mon_export_profile_t *insert)
{
    bcmcth_mon_int_control_entry_t entry;

    SHR_FUNC_ENTER(unit);

    sal_memset(&entry, 0, sizeof(entry));

    /* Get the current full view of the IMM entry. */
    sal_memcpy(&entry, int_entry, sizeof(entry));
    SHR_IF_ERR_VERBOSE_EXIT(int_control_defaults_get(unit, &entry));

    if (entry.export_profile_id != insert->export_profile_id) {
        /* This export profile entry is not being used, return. */
        SHR_EXIT();
    }

    entry.export_profile_found = true;
    sal_memcpy(&(entry.export_profile), insert, sizeof(entry.export_profile));

    SHR_IF_ERR_VERBOSE_EXIT(int_collector_config(unit, &entry, &(entry.oper)));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_int_export_profile_update(int unit,
                                     bcmcth_mon_int_control_entry_t *int_entry,
                                     bcmcth_mon_export_profile_t *update)
{
    int_info_t *int_info = g_int_info[unit];
    bcmcth_mon_int_control_entry_t entry;
    bool change;

    SHR_FUNC_ENTER(unit);

    sal_memset(&entry, 0, sizeof(entry));

    /* Get the current full view of the IMM entry. */
    sal_memcpy(&entry, int_entry, sizeof(entry));
    SHR_IF_ERR_VERBOSE_EXIT(int_control_defaults_get(unit, &entry));

    if (entry.export_profile_id != update->export_profile_id) {
        /* This export profile entry is not being used, return. */
        SHR_EXIT();
    }

    /* Update the modified export profile fields. */
    change = false;
    if (SHR_BITGET(update->fbmp, MON_EXPORT_PROFILEt_WIRE_FORMATf)) {
        SHR_BITSET(entry.export_profile.fbmp, MON_EXPORT_PROFILEt_WIRE_FORMATf);
        entry.export_profile.wire_format = update->wire_format;
        change = true;
    }
    if (SHR_BITGET(update->fbmp, MON_EXPORT_PROFILEt_PACKET_LEN_INDICATORf)) {
        SHR_BITSET(entry.export_profile.fbmp, MON_EXPORT_PROFILEt_PACKET_LEN_INDICATORf);
        entry.export_profile.packet_len_indicator =
            update->packet_len_indicator;
        change = true;
    }
    if (SHR_BITGET(update->fbmp, MON_EXPORT_PROFILEt_MAX_PKT_LENGTHf)) {
        SHR_BITSET(entry.export_profile.fbmp, MON_EXPORT_PROFILEt_MAX_PKT_LENGTHf);
        entry.export_profile.max_packet_length = update->max_packet_length;
        change = true;
    }
    if (SHR_BITGET(update->fbmp, MON_EXPORT_PROFILEt_NUM_RECORDSf)) {
        SHR_BITSET(entry.export_profile.fbmp, MON_EXPORT_PROFILEt_NUM_RECORDSf);
        entry.export_profile.num_records = update->num_records;
        change = true;
    }

    if (change == false) {
        /* Nothing we care about has changed, ignore the update. */
        SHR_EXIT();
    }

    /* Something has changed, delete the collector and re-add. */
    if (int_info->ha->collector == true) {
        SHR_IF_ERR_VERBOSE_EXIT(int_ifa_uc_collector_delete(unit));
    }

    SHR_IF_ERR_VERBOSE_EXIT(int_collector_config(unit, &entry, &(entry.oper)));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_int_export_profile_delete(int unit,
                                     bcmcth_mon_int_control_entry_t *int_entry,
                                     uint32_t export_profile_id)
{
    int_info_t *int_info = g_int_info[unit];
    bcmcth_mon_int_control_entry_t entry;

    SHR_FUNC_ENTER(unit);

    sal_memset(&entry, 0, sizeof(entry));

    /* Get the current full view of the IMM entry. */
    sal_memcpy(&entry, int_entry, sizeof(entry));
    SHR_IF_ERR_VERBOSE_EXIT(int_control_defaults_get(unit, &entry));

    if (entry.export_profile_id != export_profile_id) {
        /* This collector entry is not being used, return. */
        SHR_EXIT();
    }


    /* Delete the collector in the app. */
    if (int_info->ha->collector == true) {
        SHR_IF_ERR_VERBOSE_EXIT(int_ifa_uc_collector_delete(unit));
    }

    entry.export_profile_found = false;
    sal_memset(&(entry.export_profile), 0, sizeof(entry.export_profile));
    SHR_IF_ERR_VERBOSE_EXIT(int_collector_config(unit, &entry, &(entry.oper)));

exit:
    SHR_FUNC_EXIT();
}
