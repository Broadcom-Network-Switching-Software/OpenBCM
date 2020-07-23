/*! \file bcmcth_oam_bfd_uc.c
 *
 * BFD uC interface APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <shr/shr_util_pack.h>
#include <shr/shr_ha.h>
#include <shr/shr_types.h>
#include <shr/shr_thread.h>
#include <shr/shr_util.h>

#include <bcmbd/bcmbd_mcs.h>
#include <bcmdrd/bcmdrd.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmdrd/bcmdrd_hal.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmpkt/flexhdr/bcmpkt_rxpmd_flex.h>
#include <bcmissu/issu_api.h>
#include <bcmcth/bcmcth_oam_drv.h>
#include <bcmcth/bcmcth_oam_util.h>
#include <bcmlrd/bcmlrd_conf.h>
#include <bcmltd/chip/bcmltd_str.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmpkt/bcmpkt_lbhdr.h>
#include <bcmpkt/bcmpkt_dev.h>
#include <bcmpkt/flexhdr/bcmpkt_generic_loopback_t.h>
#include <bcmpkt/bcmpkt_rxpmd.h>

#include <bcmcth/bcmcth_port_drv.h>
#include <bcmcth/bcmcth_oam_bfd_uc.h>
#include <bcmcth/bcmcth_oam_bfd.h>
#include <bcmbd/mcs_shared/mcs_msg_common.h>
#include <bcmbd/mcs_shared/mcs_bfd_common.h>
#include <bcmcth/bcmcth_oam_bfd_uc_msg_pack.h>
#include <bcmcth/generated/oam_bfd_ha.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_OAM

/* Get the upper 32b of a 64b */
#define BFD_64_HI(_u64_) (((_u64_) >> 32) & 0xFFFFFFFF)

/* Get the lower 32b of a 64b */
#define BFD_64_LO(_u64_) ((_u64_) & 0xFFFFFFFF)

/* Join 2 32b bfdo 1 64b */
#define BFD_64_GET(_hi_, _lo_) ((((uint64_t)(_hi_)) << 32) | (_lo_))


/* Current messaging version of the SDK */
#define BFD_SDK_MSG_VERSION (0)


/* Expand shorthand enums to LTD enum naming style. */
#define LTD_ENUM(_type_, _member_) \
    BCMLTD_COMMON_OAM_BFD_ ## _type_ ## _T_T_ ## _member_

#define CONTROL_STATE(_state_) LTD_ENUM(CONTROL_STATE, _state_)

#define ENDPOINT_STATE(_state_) LTD_ENUM(ENDPOINT_STATE, _state_)

#define AUTH_TYPE(_type_) LTD_ENUM(AUTH_TYPE, _type_)

#define ENDPOINT_SESSION_INIT_ROLE(_role_) \
    LTD_ENUM(ENDPOINT_SESSION_INIT_ROLE, _role_)

#define ENDPOINT_MODE(_mode_) LTD_ENUM(ENDPOINT_MODE, _mode_)

#define SESSION_STATE(_state_) LTD_ENUM(SESSION_STATE, _state_)

#define DIAG_CODE(_diag_) LTD_ENUM(DIAG_CODE, _diag_)

#define IP_ENDPOINT_TYPE(_type_) LTD_ENUM(IP_ENDPOINT_TYPE, _type_)

#define INNER_IP_TYPE(_type_) LTD_ENUM(TNL_IP_INNER_IP_TYPE, _type_)

#define TNL_MPLS_ENDPOINT_ENCAP_TYPE(_type_) \
    LTD_ENUM(TNL_MPLS_ENDPOINT_ENCAP_TYPE, _type_)

#define TNL_MPLS_ENDPOINT_IP_ENCAP_TYPE(_type_) \
    LTD_ENUM(TNL_MPLS_ENDPOINT_IP_ENCAP_TYPE, _type_)

#define ENDPOINT_EVENT(_event_) LTD_ENUM(ENDPOINT_EVENT, _event_)

#define AUTH_STATE(_state_) LTD_ENUM(AUTH_STATE, _state_)

#define ENDPOINT_ID_STATE(_state_) LTD_ENUM(ENDPOINT_ID_STATE, _state_)

/* Length of the GRE header. */
#define GRE_HDR_LEN (4)

/* Offset of the Dst IPv4 address in the IPv4 header. */
#define IPV4_HDR_DST_IP_OFFSET (16)

/* Offset of the Dst IPv6 address in the IPv6 header. */
#define IPV6_HDR_DST_IP_OFFSET (24)

#define MPLS_LABEL_PACK(_buf_, _label_) SHR_UTIL_PACK_U32(_buf_, _label_)


/* Maximum number of BFD endpoint types. */
#define MAX_ENDPOINT_TYPES (5)

/* Invalid endpoint Id. */
#define INVALID_ENDPOINT_ID (-1)


/* Length of the BFD header. */
#define BFD_HDR_LEN (24)

/* UDP Header */
#define BFD_UDP_HEADER_LENGTH (8)

/* Length of the simple password authentication header. */
#define BFD_AUTH_SP_HDR_LEN(_pwd_len_) (3 + (_pwd_len_))

/* Length of the SHA1 authentication header. */
#define BFD_AUTH_SHA1_HDR_LEN (28)

/* UDP destination port for Single Hop BFD. */
#define BFD_UDP_DST_PORT_SINGLE_HOP (3784)

/* UDP destination port for Multi Hop BFD. */
#define BFD_UDP_DST_PORT_MULTI_HOP (4784)

/* UDP destination port for micro BFD. */
#define BFD_UDP_DST_PORT_MICRO (6784)

/* UDP destination port for BFD Echo packets. */
#define BFD_UDP_DST_PORT_ECHO (3785)

/* First nibble of the ACH. */
#define BFD_ACH_FIRST_NIBBLE (0x1)

/* ACH version. */
#define BFD_ACH_VERSION (0x0)

/* ACH channel types. */
#define BFD_ACH_CHANNEL_TYPE_RAW         (0x0007)
#define BFD_ACH_CHANNEL_TYPE_IPV4        (0x0021)
#define BFD_ACH_CHANNEL_TYPE_IPV6        (0x0057)
#define BFD_ACH_CHANNEL_TYPE_MPLS_TP_CC  (0x0022)


/* Time to wait for the event thread to stop. */
#define BFD_EVENT_THREAD_STOP_TIMEOUT_USECS (5 * 1000 * 1000)

/* Macros for passing unit + warmboot indication to the event thread */
#define BFD_ENCODE_UNIT_WARM(_unit, _wb) ((void *) (uintptr_t) ((_unit << 16) | _wb))
#define BFD_DECODE_UNIT(_val)       ((int) (((uintptr_t) (_val)) >> 16))
#define BFD_DECODE_WB(_val)         (((uintptr_t) (_val)) & 0xffff)

/* Forward declarations. */
static int bfd_uc_msg_rxpmd_flex_format_set(int unit);
static int bfd_event_thread_start(int unit, bool warm);
static int bfd_event_thread_stop(int unit);
static int bfd_uc_msg_match_id_info_set(int unit);
static bcmcth_oam_bfd_endpoint_ha_t * bfd_endpoint_info_get(int unit, uint32_t ep_id);
/* BFD info data structure */
typedef struct bfd_info_s {
    /* BFD control information stored in HA memory. */
    bcmcth_oam_bfd_control_ha_t *ctrl;

    /* Simple password authentication info. */
    bcmcth_oam_bfd_auth_ha_t *auth_sp;

    /* Simple password authentication internal Id bitmap. */
    SHR_BITDCL *auth_sp_int_id_bmp;

    /* SHA1 authentication info. */
    bcmcth_oam_bfd_auth_ha_t *auth_sha1;

    /* SHA1 authentication internal Id bitmap. */
    SHR_BITDCL *auth_sha1_int_id_bmp;

    /* Endpoint information indexed by ep_type, endpoint Id. */
    bcmcth_oam_bfd_endpoint_ha_t *endpoints;

    /* Event thread control. */
    shr_thread_t event_thread_ctrl;

    /* DMA buffer for holding send/receive messages */
    uint8_t *dma_buffer;

    /* Physical address of the DMA buffer */
    uint64_t dma_buffer_p_addr;
} bfd_info_t;

static bfd_info_t *g_bfd_info[BCMDRD_CONFIG_MAX_UNITS] = {0};

/*******************************************************************************
 * Functions to convert enum members from between MCS and LT.
 */
static int
mcs_to_lt_ep_state_convert(mcs_bfd_session_state_t mcs_state,
                           bcmltd_common_oam_bfd_session_state_t_t *lt_state)
{
    switch (mcs_state) {
        case MCS_BFD_SESSION_STATE_ADMIN_DOWN:
            *lt_state = SESSION_STATE(ADMIN_DOWN);
            break;

        case MCS_BFD_SESSION_STATE_DOWN:
            *lt_state = SESSION_STATE(DOWN);
            break;

        case MCS_BFD_SESSION_STATE_INIT:
            *lt_state = SESSION_STATE(INIT);
            break;

        case MCS_BFD_SESSION_STATE_UP:
            *lt_state = SESSION_STATE(UP);
            break;

        default:
            return SHR_E_PARAM;
    }

    return SHR_E_NONE;
}

static int
mcs_to_lt_diag_code_convert(mcs_bfd_diag_code_t mcs_diag_code,
                            bcmltd_common_oam_bfd_diag_code_t_t *lt_diag_code)
{
    switch (mcs_diag_code) {
        case MCS_BFD_DIAG_CODE_NONE:
            *lt_diag_code = DIAG_CODE(NO_DIAGNOSTIC);
            break;

        case MCS_BFD_DIAG_CODE_CTRL_DETECT_TIME_EXPIRED:
            *lt_diag_code = DIAG_CODE(CONTROL_DETECTION_TIME_EXPIRED);
            break;

        case MCS_BFD_DIAG_CODE_ECHO_FAILED:
            *lt_diag_code = DIAG_CODE(ECHO_FUNCTION_FAILED);
            break;

        case MCS_BFD_DIAG_CODE_NEIGHBOR_SIGNALED_SESSION_DOWN:
            *lt_diag_code = DIAG_CODE(NEIGHBOR_SIGNALED_SESSION_DOWN);
            break;

        case MCS_BFD_DIAG_CODE_FORWARDING_PLANE_RESET:
            *lt_diag_code = DIAG_CODE(FORWARDING_PLANE_RESET);
            break;

        case MCS_BFD_DIAG_CODE_PATH_DOWN:
            *lt_diag_code = DIAG_CODE(PATH_DOWN);
            break;

        case MCS_BFD_DIAG_CODE_CONCATENATED_PATH_DOWN:
            *lt_diag_code = DIAG_CODE(CONCATENATED_PATH_DOWN);
            break;

        case MCS_BFD_DIAG_CODE_ADMIN_DOWN:
            *lt_diag_code = DIAG_CODE(ADMINISTRATIVELY_DOWN);
            break;

        case MCS_BFD_DIAG_CODE_REVERSE_CONCATENATED_PATH_DOWN:
            *lt_diag_code = DIAG_CODE(REVERSE_CONCATENATED_PATH_DOWN);
            break;

        case MCS_BFD_DIAG_CODE_MIS_CONNECTIVITY_DEFECT:
            *lt_diag_code = DIAG_CODE(MISCONNECTIVITY_DEFECT);
            break;

        default:
            return SHR_E_PARAM;
    }

    return SHR_E_NONE;
}

static int
lt_to_mcs_diag_code_convert(bcmltd_common_oam_bfd_diag_code_t_t lt_diag_code,
                            mcs_bfd_diag_code_t *mcs_diag_code)
{
    switch (lt_diag_code) {
        case DIAG_CODE(NO_DIAGNOSTIC):
            *mcs_diag_code = MCS_BFD_DIAG_CODE_NONE;
            break;

        case DIAG_CODE(CONTROL_DETECTION_TIME_EXPIRED):
            *mcs_diag_code = MCS_BFD_DIAG_CODE_CTRL_DETECT_TIME_EXPIRED;
            break;

        case DIAG_CODE(ECHO_FUNCTION_FAILED):
            *mcs_diag_code = MCS_BFD_DIAG_CODE_ECHO_FAILED;
            break;

        case DIAG_CODE(NEIGHBOR_SIGNALED_SESSION_DOWN):
            *mcs_diag_code = MCS_BFD_DIAG_CODE_NEIGHBOR_SIGNALED_SESSION_DOWN;
            break;

        case DIAG_CODE(FORWARDING_PLANE_RESET):
            *mcs_diag_code = MCS_BFD_DIAG_CODE_FORWARDING_PLANE_RESET;
            break;

        case DIAG_CODE(PATH_DOWN):
            *mcs_diag_code = MCS_BFD_DIAG_CODE_PATH_DOWN;
            break;

        case DIAG_CODE(CONCATENATED_PATH_DOWN):
            *mcs_diag_code = MCS_BFD_DIAG_CODE_CONCATENATED_PATH_DOWN;
            break;

        case DIAG_CODE(ADMINISTRATIVELY_DOWN):
            *mcs_diag_code = MCS_BFD_DIAG_CODE_ADMIN_DOWN;
            break;

        case DIAG_CODE(REVERSE_CONCATENATED_PATH_DOWN):
            *mcs_diag_code = MCS_BFD_DIAG_CODE_REVERSE_CONCATENATED_PATH_DOWN;
            break;

        case DIAG_CODE(MISCONNECTIVITY_DEFECT):
            *mcs_diag_code = MCS_BFD_DIAG_CODE_MIS_CONNECTIVITY_DEFECT;
            break;

        default:
            return SHR_E_PARAM;
    }

    return SHR_E_NONE;
}

/*******************************************************************************
 * BFD EApp messaging private functions.
 */
static int
bfd_uc_error_convert(uint32_t uc_rv)
{
    int rv = SHR_E_NONE;

    switch (uc_rv) {
        case MCS_BFD_UC_E_NONE:
            rv = SHR_E_NONE;
            break;

        case MCS_BFD_UC_E_INTERNAL:
            rv = SHR_E_INTERNAL;
            break;

        case MCS_BFD_UC_E_MEMORY:
            rv = SHR_E_MEMORY;
            break;

        case MCS_BFD_UC_E_UNIT:
            rv = SHR_E_UNIT;
            break;

        case MCS_BFD_UC_E_PARAM:
            rv = SHR_E_PARAM;
            break;

        case MCS_BFD_UC_E_EMPTY:
            rv = SHR_E_EMPTY;
            break;

        case MCS_BFD_UC_E_FULL:
            rv = SHR_E_FULL;
            break;

        case MCS_BFD_UC_E_NOT_FOUND:
            rv = SHR_E_NOT_FOUND;
            break;

        case MCS_BFD_UC_E_EXISTS:
            rv = SHR_E_EXISTS;
            break;

        case MCS_BFD_UC_E_TIMEOUT:
            rv = SHR_E_TIMEOUT;
            break;

        case MCS_BFD_UC_E_BUSY:
            rv = SHR_E_BUSY;
            break;

        case MCS_BFD_UC_E_FAIL:
            rv = SHR_E_FAIL;
            break;

        case MCS_BFD_UC_E_DISABLED:
            rv = SHR_E_DISABLED;
            break;

        case MCS_BFD_UC_E_BADID:
            rv = SHR_E_BADID;
            break;

        case MCS_BFD_UC_E_RESOURCE:
            rv = SHR_E_RESOURCE;
            break;

        case MCS_BFD_UC_E_CONFIG:
            rv = SHR_E_CONFIG;
            break;

        case MCS_BFD_UC_E_UNAVAIL:
            rv = SHR_E_UNAVAIL;
            break;

        case MCS_BFD_UC_E_INIT:
            rv = SHR_E_INIT;
            break;

        case MCS_BFD_UC_E_PORT:
            rv = SHR_E_PORT;
            break;

        default:
            rv = SHR_E_INTERNAL;
            break;
    }

    return rv;
}

static int
bfd_uc_msg_send_receive(int unit,
                        uint8_t s_subclass, uint16_t s_len, uint64_t s_data,
                        uint8_t r_subclass, uint16_t *r_len,
                        sal_usecs_t timeout)
{
    bfd_info_t *bfd_info = g_bfd_info[unit];
    mcs_msg_data_t send, reply;

    SHR_FUNC_ENTER(unit);

    sal_memset(&send, 0, sizeof(send));
    sal_memset(&reply, 0, sizeof(reply));

    MCS_MSG_MCLASS_SET(send, MCS_MSG_CLASS_BFD);
    MCS_MSG_SUBCLASS_SET(send, s_subclass);
    MCS_MSG_LEN_SET(send, s_len);

    /*
     * Set 'data' to DMA buffer address if a DMA operation is
     * required for send or receive.
     */
    if (MCS_MSG_DMA_MSG(s_subclass) || MCS_MSG_DMA_MSG(r_subclass)) {
        MCS_MSG_DATA_SET(send, BFD_64_LO(bfd_info->dma_buffer_p_addr));
        MCS_MSG_DATA1_SET(send, BFD_64_HI(bfd_info->dma_buffer_p_addr));
    } else {
        MCS_MSG_DATA_SET(send, BFD_64_LO(s_data));
        MCS_MSG_DATA1_SET(send, BFD_64_HI(s_data));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmbd_mcs_msg_send_receive(unit, bfd_info->ctrl->uc_num,
                                    &send, &reply, timeout));

    /* Convert BFD uC error code to SHR_E_XXX format. Return error if the
     * converted value is not SHR_E_NONE.
     */
    SHR_IF_ERR_VERBOSE_EXIT(bfd_uc_error_convert(MCS_MSG_DATA_GET(reply)));

    *r_len = MCS_MSG_LEN_GET(reply);

    /* Return error if we got the wrong class/subclass. */
    if ((MCS_MSG_MCLASS_GET(reply)   != MCS_MSG_CLASS_BFD) ||
        (MCS_MSG_SUBCLASS_GET(reply) != r_subclass)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

 exit:
    SHR_FUNC_EXIT();
}

static int
bfd_uc_msg_appl_init(int unit, int uc)
{
    mcs_msg_data_t send, rcv;
    bcmbd_ukernel_info_t ukernel_info;

    SHR_FUNC_ENTER(unit);

    sal_memset(&send, 0, sizeof(send));
    sal_memset(&rcv, 0, sizeof(rcv));

    MCS_MSG_MCLASS_SET(send, MCS_MSG_CLASS_SYSTEM);
    MCS_MSG_SUBCLASS_SET(send, MCS_MSG_SUBCLASS_SYSTEM_APPL_INIT);
    MCS_MSG_LEN_SET(send, MCS_MSG_CLASS_BFD);
    MCS_MSG_DATA_SET(send, BFD_SDK_MSG_VERSION);


    SHR_IF_ERR_VERBOSE_EXIT(bcmbd_mcs_uc_status(unit, uc, &ukernel_info));
    if (sal_strcmp(ukernel_info.status,"OK") != 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmbd_mcs_msg_send(unit, uc, &send, MCS_BFD_MAX_UC_MSG_TIMEOUT));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmbd_mcs_msg_receive(unit, uc, MCS_MSG_CLASS_BFD,
                               &rcv, MCS_BFD_MAX_UC_MSG_TIMEOUT));

    if (MCS_MSG_LEN_GET(rcv) != 0) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

 exit:
    SHR_FUNC_EXIT();
}

static int
bfd_uc_msg_shutdown(int unit)
{
    bfd_info_t *bfd_info = g_bfd_info[unit];
    bcmbd_ukernel_info_t ukernel_info;
    uint16_t reply_len;

    SHR_FUNC_ENTER(unit);

    if (bfd_info->ctrl->init == false) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmbd_mcs_uc_status(unit, bfd_info->ctrl->uc_num, &ukernel_info));

    SHR_IF_ERR_VERBOSE_EXIT(bfd_event_thread_stop(unit));
    if (sal_strcmp(ukernel_info.status, "OK") == 0) {

        SHR_IF_ERR_VERBOSE_EXIT
            (bfd_uc_msg_send_receive(unit,
                                     MCS_MSG_SUBCLASS_BFD_UNINIT,
                                     0, 0,
                                     MCS_MSG_SUBCLASS_BFD_UNINIT_REPLY,
                                     &reply_len,
                                     MCS_BFD_MAX_UC_MSG_TIMEOUT));

        if (reply_len != 0) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }
    bfd_info->ctrl->init = false;
    bfd_info->ctrl->run = false;

 exit:
    SHR_FUNC_EXIT();
}

static int
bfd_uc_msg_init(int unit, bcmcth_oam_bfd_control_t *entry)
{
    bfd_info_t *bfd_info = g_bfd_info[unit];
    mcs_bfd_msg_ctrl_init_t msg;
    uint8_t *buffer_ptr = NULL;
    uint16_t buffer_len = 0, reply_len = 0;
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
    if (bfd_info->ctrl->init) {
        SHR_IF_ERR_CONT
            (bfd_uc_msg_shutdown(unit));
        bfd_info->ctrl->init = false;
        bfd_info->ctrl->run = false;
    }

    /* Init the app, determine which uC is running the app by chooosing
     * the first uC that returns sucessfully to the APPL_INIT message.
     */
    num_uc = bcmbd_mcs_num_uc(unit);
    for (uc = 0; uc < num_uc ; uc++) {
        rv = bfd_uc_msg_appl_init(unit, uc);
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

    bfd_info->ctrl->uc_num = uc;

    /* Set control message data */
    sal_memset(&msg, 0, sizeof(msg));

    if (entry->internal_local_discr == true) {
        msg.flags |= MCS_BFD_MSG_INIT_F_INTERNAL_LOCAL_DISCR;
    }
    if (entry->static_remote_discr == true) {
        msg.flags |= MCS_BFD_MSG_INIT_F_STATIC_REMOTE_DISCR;
    }

    msg.num_sessions = entry->max_endpoints;
    msg.num_auth_sha1_keys = entry->max_auth_sha1_keys;
    msg.num_auth_sp_keys = entry->max_auth_sp_keys;
    msg.max_pkt_size = entry->max_pkt_size;

    buffer_ptr = bcmcth_oam_bfd_msg_ctrl_init_pack(bfd_info->dma_buffer, &msg);
    buffer_len = buffer_ptr - bfd_info->dma_buffer;

    SHR_IF_ERR_VERBOSE_EXIT
        (bfd_uc_msg_send_receive(unit,
                                 MCS_MSG_SUBCLASS_BFD_INIT,
                                 buffer_len, 0,
                                 MCS_MSG_SUBCLASS_BFD_INIT_REPLY,
                                 &reply_len,
                                 MCS_BFD_MAX_UC_MSG_TIMEOUT));
    if (reply_len != 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    bfd_info->ctrl->init = true;

    /* Set the format of the flex fields in the Rx PMD. */
    SHR_IF_ERR_VERBOSE_EXIT(bfd_uc_msg_rxpmd_flex_format_set(unit));

    /* Send match Id information to app. */
    SHR_IF_ERR_VERBOSE_EXIT(bfd_uc_msg_match_id_info_set(unit));


    bfd_info->ctrl->run = true;

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
bfd_uc_msg_rxpmd_flex_format_set(int unit)
{
    bfd_info_t *bfd_info = g_bfd_info[unit];
    bcmcth_oam_bfd_drv_t *bfd_drv = NULL;
    int fids[MCS_BFD_RXPMD_FLEX_FIELD_COUNT];
    int num_fids, i;
    mcs_bfd_rxpmd_flex_fields_t uc_fid;
    uint32_t rxpmd_flex_ref[BCMPKT_RXPMD_SIZE_WORDS];
    uint32_t rxpmd_flex_mask[BCMPKT_RXPMD_SIZE_WORDS];
    bcmlrd_variant_t variant;
    uint32_t profile;
    uint32_t val;
    int offset, width;
    mcs_bfd_msg_ctrl_rxpmd_flex_format_t msg;
    uint8_t *buffer_ptr = NULL;
    uint16_t buffer_len = 0, reply_len = 0;
    int rv;
    char *fname = NULL;
    bcmdrd_dev_type_t dev_type;
    uint32_t flexdata_len_words;
    uint32_t flexdata_len_bytes;
    uint32_t *flexdata_addr;

    SHR_FUNC_ENTER(unit);

    bfd_drv = bcmcth_oam_bfd_drv_get(unit);
    if ((bfd_drv == NULL) || (bfd_drv->rxpmd_flex_fids_get == NULL)) {
        SHR_EXIT();
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bfd_drv->rxpmd_flex_fids_get(unit, MCS_BFD_RXPMD_FLEX_FIELD_COUNT,
                                      &num_fids, fids));

    sal_memset(&msg, 0, sizeof(msg));

    variant = bcmlrd_variant_get(unit);
    if (variant == BCMLRD_VARIANT_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    SHR_IF_ERR_EXIT(bcmpkt_dev_type_get(unit, &dev_type));
    SHR_IF_ERR_EXIT(
        bcmpkt_rxpmd_flexdata_get(dev_type, rxpmd_flex_ref,
                                  &flexdata_addr, &flexdata_len_words));
    flexdata_len_bytes = flexdata_len_words * 4;

    sal_memset(rxpmd_flex_ref, 0xFF, BCMPKT_RXPMD_SIZE_BYTES);

    
    for (profile = 0; profile < 128; profile++) {
        for (i = 0; i < num_fids; i++) {
            sal_memset(rxpmd_flex_mask, 0, BCMPKT_RXPMD_SIZE_BYTES);

            /* Get mask value */
            rv = bcmpkt_rxpmd_flex_field_get(variant, rxpmd_flex_ref, profile,
                                             fids[i], &val);
            if (rv != SHR_E_NONE) {
                continue;
            }

            /* Set field mask */
            rv = bcmpkt_rxpmd_flex_field_set(variant, rxpmd_flex_mask,
                                             profile, fids[i], val);
            if (rv != SHR_E_NONE) {
                continue;
            }

            bmp_offset_width_get(rxpmd_flex_mask, flexdata_len_bytes * 8,
                                 &offset, &width);

            rv = bcmpkt_rxpmd_flex_field_name_get(fids[i], &fname);
            SHR_IF_ERR_EXIT(rv);
            if (!sal_strcasecmp(fname, "I2E_CLASS_ID_15_0")) {
                uc_fid = MCS_BFD_RXPMD_FLEX_FIELD_I2E_CLASS_ID_15_0;
            } else {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            msg.offsets[uc_fid][profile] = offset;
            msg.widths[uc_fid][profile] = width;
        }
    }

    msg.num_fields   = num_fids;
    msg.num_flow_ids = 128;

    buffer_ptr =
        bcmcth_oam_bfd_msg_ctrl_rxpmd_flex_format_set_pack(bfd_info->dma_buffer,
                                                           &msg);
    buffer_len = buffer_ptr - bfd_info->dma_buffer;

    SHR_IF_ERR_VERBOSE_EXIT
        (bfd_uc_msg_send_receive(unit,
                                 MCS_MSG_SUBCLASS_BFD_RXPMD_FLEX_FORMAT_SET,
                                 buffer_len, 0,
                                 MCS_MSG_SUBCLASS_BFD_RXPMD_FLEX_FORMAT_SET_REPLY,
                                 &reply_len,
                                 MCS_BFD_MAX_UC_MSG_TIMEOUT));

    if (reply_len != 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

 exit:
    SHR_FUNC_EXIT();
}

static int
bfd_uc_msg_match_id_info_set(int unit)
{
    bfd_info_t *bfd_info = g_bfd_info[unit];
    mcs_bfd_msg_ctrl_match_id_set_t msg;
    uint8_t *buffer_ptr = NULL;
    uint16_t buffer_len = 0, reply_len = 0;
    int i = 0;
    const bcmlrd_match_id_db_t *info;
    bcmcth_oam_bfd_drv_t *bfd_drv = NULL;
    const char *str = NULL;

    SHR_FUNC_ENTER(unit);

    bfd_drv = bcmcth_oam_bfd_drv_get(unit);
    if ((bfd_drv == NULL) || (bfd_drv->match_id_string_get == NULL)) {
        SHR_EXIT();
    }

    sal_memset(&msg, 0, sizeof(msg));

    for (i = 0; i < MCS_BFD_MATCH_ID_FIELD_COUNT; i++) {
        str = bfd_drv->match_id_string_get(unit, i);
        if (!str) {
            continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_table_match_id_data_get(unit, str, &info));

        msg.offsets[msg.num_fields] =  info->minbit + info->match_minbit;
        msg.widths[msg.num_fields] = info->maxbit - info->minbit + 1;
        msg.values[msg.num_fields] = info->value;
        msg.num_fields++;
    }

    buffer_ptr = bcmcth_oam_bfd_msg_ctrl_match_id_set_pack(bfd_info->dma_buffer, &msg);
    buffer_len = buffer_ptr - bfd_info->dma_buffer;

    SHR_IF_ERR_VERBOSE_EXIT
        (bfd_uc_msg_send_receive(unit,
                                 MCS_MSG_SUBCLASS_BFD_MATCH_ID_SET,
                                 buffer_len, 0,
                                 MCS_MSG_SUBCLASS_BFD_MATCH_ID_SET_REPLY,
                                 &reply_len,
                                 MCS_BFD_MAX_UC_MSG_TIMEOUT));
    if (reply_len != 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bfd_uc_msg_global_param_set(int unit, mcs_bfd_msg_ctrl_global_param_set_t *msg)
{
    uint8_t *dma_buffer = g_bfd_info[unit]->dma_buffer;
    uint8_t *buffer_ptr = NULL;
    uint16_t buffer_len = 0, reply_len = 0;

    SHR_FUNC_ENTER(unit);

    buffer_ptr = bcmcth_oam_bfd_msg_ctrl_global_param_set_pack(dma_buffer, msg);
    buffer_len = buffer_ptr - dma_buffer;

    SHR_IF_ERR_VERBOSE_EXIT
        (bfd_uc_msg_send_receive(unit,
                                 MCS_MSG_SUBCLASS_BFD_GLOBAL_PARAM_SET,
                                 buffer_len, 0,
                                 MCS_MSG_SUBCLASS_BFD_GLOBAL_PARAM_SET_REPLY,
                                 &reply_len,
                                 MCS_BFD_MAX_UC_MSG_TIMEOUT));
    if (reply_len != 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

 exit:
    SHR_FUNC_EXIT();
}

static int
bfd_uc_msg_global_stats_set(int unit, bcmcth_oam_bfd_stats_t *entry)
{
    mcs_bfd_msg_ctrl_global_stats_set_t msg;
    uint8_t *dma_buffer = g_bfd_info[unit]->dma_buffer;
    uint8_t *buffer_ptr = NULL;
    uint16_t buffer_len = 0, reply_len = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&msg, 0, sizeof(msg));

    if (SHR_BITGET(entry->fbmp, OAM_BFD_STATSt_RX_PKT_VERSION_ERRORf)) {
        msg.valid_bmp |= MCS_BFD_MSG_GLOBAL_STATS_SET_BMP_VER_ERR;
        msg.ver_err_hi = BFD_64_HI(entry->rx_pkt_version_error);
        msg.ver_err_lo = BFD_64_LO(entry->rx_pkt_version_error);
    }
    if (SHR_BITGET(entry->fbmp, OAM_BFD_STATSt_RX_PKT_LENGTH_ERRORf)) {
        msg.valid_bmp |= MCS_BFD_MSG_GLOBAL_STATS_SET_BMP_LEN_ERR;
        msg.len_err_hi = BFD_64_HI(entry->rx_pkt_length_error);
        msg.len_err_lo = BFD_64_LO(entry->rx_pkt_length_error);
    }
    if (SHR_BITGET(entry->fbmp,
                   OAM_BFD_STATSt_RX_PKT_ZERO_DETECT_MULTIPLIERf)) {
        msg.valid_bmp |= MCS_BFD_MSG_GLOBAL_STATS_SET_BMP_ZERO_DETECT_MULT;
        msg.zero_detect_mult_hi = BFD_64_HI(entry->rx_pkt_zero_detect_multiplier);
        msg.zero_detect_mult_lo = BFD_64_LO(entry->rx_pkt_zero_detect_multiplier);
    }
    if (SHR_BITGET(entry->fbmp, OAM_BFD_STATSt_RX_PKT_ZERO_MY_DISCRIMINATORf)) {
        msg.valid_bmp |= MCS_BFD_MSG_GLOBAL_STATS_SET_BMP_ZERO_MY_DISC;
        msg.zero_my_disc_hi = BFD_64_HI(entry->rx_pkt_zero_my_discr);
        msg.zero_my_disc_lo = BFD_64_LO(entry->rx_pkt_zero_my_discr);
    }
    if (SHR_BITGET(entry->fbmp, OAM_BFD_STATSt_RX_PKT_P_AND_F_BITS_SETf)) {
        msg.valid_bmp |= MCS_BFD_MSG_GLOBAL_STATS_SET_BMP_P_F_BIT;
        msg.p_f_bit_hi = BFD_64_HI(entry->rx_pkt_p_and_f_bits_set);
        msg.p_f_bit_lo = BFD_64_LO(entry->rx_pkt_p_and_f_bits_set);
    }
    if (SHR_BITGET(entry->fbmp, OAM_BFD_STATSt_RX_PKT_M_BIT_SETf)) {
        msg.valid_bmp |= MCS_BFD_MSG_GLOBAL_STATS_SET_BMP_M_BIT;
        msg.m_bit_hi = BFD_64_HI(entry->rx_pkt_m_bit_set);
        msg.m_bit_lo = BFD_64_LO(entry->rx_pkt_m_bit_set);
    }
    if (SHR_BITGET(entry->fbmp, OAM_BFD_STATSt_RX_PKT_AUTH_MISMATCHf)) {
        msg.valid_bmp |= MCS_BFD_MSG_GLOBAL_STATS_SET_BMP_AUTH_TYPE_MISMATCH;
        msg.auth_type_mismatch_hi = BFD_64_HI(entry->rx_pkt_auth_mismatch);
        msg.auth_type_mismatch_lo = BFD_64_LO(entry->rx_pkt_auth_mismatch);
    }
    if (SHR_BITGET(entry->fbmp,
                   OAM_BFD_STATSt_RX_PKT_AUTH_SIMPLE_PASSWORD_ERRORf)) {
        msg.valid_bmp |= MCS_BFD_MSG_GLOBAL_STATS_SET_BMP_AUTH_SP_ERR;
        msg.auth_sp_err_hi = BFD_64_HI(entry->rx_pkt_sp_auth_error);
        msg.auth_sp_err_lo = BFD_64_LO(entry->rx_pkt_sp_auth_error);
    }
    if (SHR_BITGET(entry->fbmp, OAM_BFD_STATSt_RX_PKT_AUTH_SHA1_ERRORf)) {
        msg.valid_bmp |= MCS_BFD_MSG_GLOBAL_STATS_SET_BMP_AUTH_SHA1_ERR;
        msg.auth_sha1_err_hi = BFD_64_HI(entry->rx_pkt_sha1_auth_error);
        msg.auth_sha1_err_lo = BFD_64_LO(entry->rx_pkt_sha1_auth_error);
    }
    if (SHR_BITGET(entry->fbmp, OAM_BFD_STATSt_RX_PKT_ENDPOINT_NOT_FOUNDf)) {
        msg.valid_bmp |= MCS_BFD_MSG_GLOBAL_STATS_SET_BMP_AUTH_EP_NOT_FOUND;
        msg.ep_not_found_hi = BFD_64_HI(entry->rx_pkt_endpoint_not_found);
        msg.ep_not_found_lo = BFD_64_LO(entry->rx_pkt_endpoint_not_found);
    }


    buffer_ptr = bcmcth_oam_bfd_msg_ctrl_global_stats_set_pack(dma_buffer,
                                                               &msg);
    buffer_len = buffer_ptr - dma_buffer;

    SHR_IF_ERR_VERBOSE_EXIT
        (bfd_uc_msg_send_receive(unit,
                                 MCS_MSG_SUBCLASS_BFD_GLOBAL_STATS_SET,
                                 buffer_len, 0,
                                 MCS_MSG_SUBCLASS_BFD_GLOBAL_STATS_SET_REPLY,
                                 &reply_len,
                                 MCS_BFD_MAX_UC_MSG_TIMEOUT));
    if (reply_len != 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

 exit:
    SHR_FUNC_EXIT();
}

static int
bfd_uc_msg_global_stats_get(int unit, bcmcth_oam_bfd_stats_t *entry)
{
    mcs_bfd_msg_ctrl_global_stats_get_t msg;
    uint8_t *dma_buffer = g_bfd_info[unit]->dma_buffer;
    uint8_t *buffer_ptr = NULL;
    uint16_t buffer_len = 0, reply_len = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&msg, 0, sizeof(msg));

    SHR_IF_ERR_VERBOSE_EXIT
        (bfd_uc_msg_send_receive(unit,
                                 MCS_MSG_SUBCLASS_BFD_GLOBAL_STATS_GET,
                                 0, 0,
                                 MCS_MSG_SUBCLASS_BFD_GLOBAL_STATS_GET_REPLY,
                                 &reply_len,
                                 MCS_BFD_MAX_UC_MSG_TIMEOUT));

    buffer_ptr = bcmcth_oam_bfd_msg_ctrl_global_stats_get_unpack(dma_buffer,
                                                                 &msg);
    buffer_len = buffer_ptr - dma_buffer;

    if (reply_len != buffer_len) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    entry->rx_pkt_version_error = BFD_64_GET(msg.ver_err_hi, msg.ver_err_lo);
    entry->rx_pkt_length_error = BFD_64_GET(msg.len_err_hi, msg.len_err_lo);
    entry->rx_pkt_zero_detect_multiplier = BFD_64_GET(msg.zero_detect_mult_hi,
                                                      msg.zero_detect_mult_lo);
    entry->rx_pkt_zero_my_discr = BFD_64_GET(msg.zero_my_disc_hi,
                                             msg.zero_my_disc_lo);
    entry->rx_pkt_p_and_f_bits_set = BFD_64_GET(msg.p_f_bit_hi, msg.p_f_bit_lo);
    entry->rx_pkt_m_bit_set = BFD_64_GET(msg.m_bit_hi, msg.m_bit_lo);
    entry->rx_pkt_auth_mismatch = BFD_64_GET(msg.auth_type_mismatch_hi,
                                             msg.auth_type_mismatch_lo);
    entry->rx_pkt_sp_auth_error = BFD_64_GET(msg.auth_sp_err_hi,
                                             msg.auth_sp_err_lo);
    entry->rx_pkt_sha1_auth_error = BFD_64_GET(msg.auth_sha1_err_hi,
                                               msg.auth_sha1_err_lo);
    entry->rx_pkt_endpoint_not_found = BFD_64_GET(msg.ep_not_found_hi,
                                                  msg.ep_not_found_lo);

 exit:
    SHR_FUNC_EXIT();
}

static int
bfd_uc_msg_auth_sp_set(int unit, uint32_t auth_id,
                       bcmcth_oam_bfd_auth_sp_t *entry)
{
    mcs_bfd_msg_ctrl_auth_sp_set_t msg;
    uint8_t *dma_buffer = g_bfd_info[unit]->dma_buffer;
    uint8_t *buffer_ptr = NULL;
    uint16_t buffer_len = 0, reply_len = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&msg, 0, sizeof(msg));
    msg.index = auth_id;
    if (entry != NULL) {
        msg.length = entry->password_len;
        sal_memcpy(msg.password, entry->password, msg.length * sizeof(uint8_t));
    }

    buffer_ptr = bcmcth_oam_bfd_msg_ctrl_auth_sp_set_pack(dma_buffer, &msg);
    buffer_len = buffer_ptr - dma_buffer;

    SHR_IF_ERR_VERBOSE_EXIT
        (bfd_uc_msg_send_receive(unit,
                                 MCS_MSG_SUBCLASS_BFD_AUTH_SP_SET,
                                 buffer_len, 0,
                                 MCS_MSG_SUBCLASS_BFD_AUTH_SP_SET_REPLY,
                                 &reply_len,
                                 MCS_BFD_MAX_UC_MSG_TIMEOUT));
    if (reply_len != 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

 exit:
    SHR_FUNC_EXIT();
}

static int
bfd_uc_msg_auth_sha1_set(int unit, uint32_t auth_id,
                         bcmcth_oam_bfd_auth_sha1_t *entry)
{
    mcs_bfd_msg_ctrl_auth_sha1_set_t msg;
    uint8_t *dma_buffer = g_bfd_info[unit]->dma_buffer;
    uint8_t *buffer_ptr = NULL;
    uint16_t buffer_len = 0, reply_len = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&msg, 0, sizeof(msg));
    if (entry != NULL) {
        sal_memcpy(msg.key, entry->sha1_key,
                   MCS_BFD_AUTH_SHA1_KEY_LENGTH * sizeof(uint8_t));
    }

    buffer_ptr = bcmcth_oam_bfd_msg_ctrl_auth_sha1_set_pack(dma_buffer, &msg);
    buffer_len = buffer_ptr - dma_buffer;

    SHR_IF_ERR_VERBOSE_EXIT
        (bfd_uc_msg_send_receive(unit,
                                 MCS_MSG_SUBCLASS_BFD_AUTH_SHA1_SET,
                                 buffer_len, 0,
                                 MCS_MSG_SUBCLASS_BFD_AUTH_SHA1_SET_REPLY,
                                 &reply_len,
                                 MCS_BFD_MAX_UC_MSG_TIMEOUT));
    if (reply_len != 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

 exit:
    SHR_FUNC_EXIT();
}

static int
bfd_uc_msg_endpoint_set(int unit, mcs_bfd_msg_ctrl_ep_set_t *msg)
{
    uint8_t *dma_buffer = g_bfd_info[unit]->dma_buffer;
    uint8_t *buffer_ptr = NULL;
    uint16_t buffer_len = 0, reply_len = 0;

    SHR_FUNC_ENTER(unit);

    buffer_ptr = bcmcth_oam_bfd_msg_ctrl_ep_set_pack(dma_buffer, msg);
    buffer_len = buffer_ptr - dma_buffer;

    SHR_IF_ERR_VERBOSE_EXIT
        (bfd_uc_msg_send_receive(unit,
                                 MCS_MSG_SUBCLASS_BFD_EP_SET,
                                 buffer_len, 0,
                                 MCS_MSG_SUBCLASS_BFD_EP_SET_REPLY,
                                 &reply_len,
                                 MCS_BFD_MAX_UC_MSG_TIMEOUT));
    if (reply_len != 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

 exit:
    SHR_FUNC_EXIT();
}

static int
bfd_uc_msg_endpoint_delete(int unit, uint32_t endpoint_id)
{
    uint16_t reply_len;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bfd_uc_msg_send_receive(unit,
                                 MCS_MSG_SUBCLASS_BFD_EP_DELETE,
                                 endpoint_id, 0,
                                 MCS_MSG_SUBCLASS_BFD_EP_DELETE_REPLY,
                                 &reply_len,
                                 MCS_BFD_MAX_UC_MSG_TIMEOUT));
    if (reply_len != 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

 exit:
    SHR_FUNC_EXIT();
}

static int
bfd_uc_msg_endpoint_sha1_seq_num_incr(int unit, uint32_t endpoint_id)
{
    uint16_t reply_len;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bfd_uc_msg_send_receive(unit,
                                 MCS_MSG_SUBCLASS_BFD_EP_AUTH_SEQ_NUM_INCR,
                                 endpoint_id, 0,
                                 MCS_MSG_SUBCLASS_BFD_EP_AUTH_SEQ_NUM_INCR_REPLY,
                                 &reply_len,
                                 MCS_BFD_MAX_UC_MSG_TIMEOUT));
    if (reply_len != 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

 exit:
    SHR_FUNC_EXIT();
}

static int
bfd_uc_msg_endpoint_poll(int unit, uint32_t endpoint_id)
{
    uint16_t reply_len;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bfd_uc_msg_send_receive(unit,
                                 MCS_MSG_SUBCLASS_BFD_EP_POLL,
                                 endpoint_id, 0,
                                 MCS_MSG_SUBCLASS_BFD_EP_POLL_REPLY,
                                 &reply_len,
                                 MCS_BFD_MAX_UC_MSG_TIMEOUT));
    if (reply_len != 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

 exit:
    SHR_FUNC_EXIT();
}

static int
bfd_uc_msg_endpoint_status_get(int unit, uint32_t endpoint_id,
                           bcmcth_oam_bfd_endpoint_status_t *entry)
{
    uint8_t *dma_buffer = g_bfd_info[unit]->dma_buffer;
    mcs_bfd_msg_ctrl_ep_status_get_t msg;
    uint8_t *buffer_ptr = NULL;
    uint16_t buffer_len = 0, reply_len = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&msg, 0, sizeof(msg));

    SHR_IF_ERR_VERBOSE_EXIT
        (bfd_uc_msg_send_receive(unit,
                                 MCS_MSG_SUBCLASS_BFD_EP_STATUS_GET,
                                 endpoint_id, 0,
                                 MCS_MSG_SUBCLASS_BFD_EP_STATUS_GET_REPLY,
                                 &reply_len,
                                 MCS_BFD_MAX_UC_MSG_TIMEOUT));

    buffer_ptr = bcmcth_oam_bfd_msg_ctrl_ep_status_get_unpack(dma_buffer, &msg);
    buffer_len = buffer_ptr - dma_buffer;

    if (reply_len != buffer_len) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mcs_to_lt_ep_state_convert(msg.local_state,
                                    &(entry->local_state)));
    SHR_IF_ERR_VERBOSE_EXIT
        (mcs_to_lt_ep_state_convert(msg.remote_state,
                                    &(entry->remote_state)));
    SHR_IF_ERR_VERBOSE_EXIT
        (mcs_to_lt_diag_code_convert(msg.local_diag,
                                     &(entry->local_diag_code)));
    SHR_IF_ERR_VERBOSE_EXIT
        (mcs_to_lt_diag_code_convert(msg.remote_diag,
                                     &(entry->remote_diag_code)));
    entry->local_auth_seq_num = msg.xmt_auth_seq;
    entry->remote_auth_seq_num = msg.rcv_auth_seq;
    entry->poll_sequence_active = msg.poll ? true : false;
    entry->remote_mode = msg.remote_demand ? ENDPOINT_MODE(DEMAND)
                                           : ENDPOINT_MODE(ASYNCHRONOUS);
    entry->local_discr = msg.local_discr;
    entry->remote_discr = msg.remote_discr;
    entry->remote_detect_multiplier = msg.remote_detect_mult;
    entry->remote_min_tx_interval_usecs = msg.remote_min_tx;
    entry->remote_min_rx_interval_usecs = msg.remote_min_rx;
    entry->remote_min_echo_rx_interval_usecs = msg.remote_min_echo_rx;

 exit:
    SHR_FUNC_EXIT();
}

static int
bfd_endpoint_uc_stats_set(int unit, uint32_t endpoint_id,
                          bcmcth_oam_bfd_endpoint_stats_t *entry)
{
    mcs_bfd_msg_ctrl_ep_stats_set_t msg;
    uint8_t *dma_buffer = g_bfd_info[unit]->dma_buffer;
    uint8_t *buffer_ptr = NULL;
    uint16_t buffer_len = 0, reply_len = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&msg, 0, sizeof(msg));

    msg.endpoint_id = endpoint_id;

    if (entry->rx_pkt_cnt_update == true) {
        msg.valid_bmp |= MCS_BFD_MSG_EP_STATS_SET_BMP_PKTS_IN;
        msg.pkts_in_hi = BFD_64_HI(entry->rx_pkt_cnt);
        msg.pkts_in_lo = BFD_64_LO(entry->rx_pkt_cnt);
    }

    if (entry->tx_pkt_cnt_update == true) {
        msg.valid_bmp |= MCS_BFD_MSG_EP_STATS_SET_BMP_PKTS_OUT;
        msg.pkts_out_hi = BFD_64_HI(entry->tx_pkt_cnt);
        msg.pkts_out_lo = BFD_64_LO(entry->tx_pkt_cnt);
    }

    if (entry->rx_pkt_discard_cnt_update == true) {
        msg.valid_bmp |= MCS_BFD_MSG_EP_STATS_SET_BMP_PKTS_DROP;
        msg.pkts_drop_hi = BFD_64_HI(entry->rx_pkt_discard_cnt);
        msg.pkts_drop_lo = BFD_64_LO(entry->rx_pkt_discard_cnt);
    }

    if (entry->rx_pkt_auth_failure_discard_cnt_update == true) {
        msg.valid_bmp |= MCS_BFD_MSG_EP_STATS_SET_BMP_PKTS_AUTH_DROP;
        msg.pkts_auth_drop_hi = BFD_64_HI(entry->rx_pkt_auth_failure_discard_cnt);
        msg.pkts_auth_drop_lo = BFD_64_LO(entry->rx_pkt_auth_failure_discard_cnt);
    }

    if (entry->rx_echo_reply_pkt_cnt_update == true) {
        msg.valid_bmp |= MCS_BFD_MSG_EP_STATS_SET_BMP_PKTS_ECHO_REPLY;
        msg.pkts_echo_reply_hi = BFD_64_HI(entry->rx_echo_reply_pkt_cnt);
        msg.pkts_echo_reply_lo = BFD_64_LO(entry->rx_echo_reply_pkt_cnt);
    }

    buffer_ptr = bcmcth_oam_bfd_msg_ctrl_ep_stats_set_pack(dma_buffer, &msg);
    buffer_len = buffer_ptr - dma_buffer;

    SHR_IF_ERR_VERBOSE_EXIT
        (bfd_uc_msg_send_receive(unit,
                                 MCS_MSG_SUBCLASS_BFD_EP_STATS_SET,
                                 buffer_len, 0,
                                 MCS_MSG_SUBCLASS_BFD_EP_STATS_SET_REPLY,
                                 &reply_len,
                                 MCS_BFD_MAX_UC_MSG_TIMEOUT));
    if (reply_len != 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

 exit:
    SHR_FUNC_EXIT();
}

static int
bfd_uc_msg_endpoint_stats_get(int unit, uint32_t endpoint_id,
                              bcmcth_oam_bfd_endpoint_stats_t *entry)
{
    mcs_bfd_msg_ctrl_ep_stats_get_t msg;
    uint8_t *dma_buffer = g_bfd_info[unit]->dma_buffer;
    uint8_t *buffer_ptr = NULL;
    uint16_t buffer_len = 0, reply_len = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&msg, 0, sizeof(msg));

    SHR_IF_ERR_VERBOSE_EXIT
        (bfd_uc_msg_send_receive(unit,
                                 MCS_MSG_SUBCLASS_BFD_EP_STATS_GET,
                                 endpoint_id, 0,
                                 MCS_MSG_SUBCLASS_BFD_EP_STATS_GET_REPLY,
                                 &reply_len,
                                 MCS_BFD_MAX_UC_MSG_TIMEOUT));

    buffer_ptr = bcmcth_oam_bfd_msg_ctrl_ep_stats_get_unpack(dma_buffer, &msg);
    buffer_len = buffer_ptr - dma_buffer;

    if (reply_len != buffer_len) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }


    entry->rx_pkt_cnt = BFD_64_GET(msg.pkts_in_hi, msg.pkts_in_lo);
    entry->tx_pkt_cnt = BFD_64_GET(msg.pkts_out_hi, msg.pkts_out_lo);
    entry->rx_pkt_discard_cnt = BFD_64_GET(msg.pkts_drop_hi, msg.pkts_drop_lo);
    entry->rx_pkt_auth_failure_discard_cnt = BFD_64_GET(msg.pkts_auth_drop_hi,
                                                        msg.pkts_auth_drop_lo);
    entry->rx_echo_reply_pkt_cnt = BFD_64_GET(msg.pkts_echo_reply_hi,
                                              msg.pkts_echo_reply_lo);

 exit:
    SHR_FUNC_EXIT();
}

static int
bfd_uc_msg_event_mask_set(int unit, bcmcth_oam_bfd_event_control_t *entry)
{
    uint32_t event_mask = 0;
    uint16_t reply_len;

    SHR_FUNC_ENTER(unit);

    if (entry->local_state_admin_down == true) {
        event_mask |= MCS_BFD_EVENT_LOCAL_STATE_ADMIN_DOWN;
    }
    if (entry->local_state_down == true) {
        event_mask |= MCS_BFD_EVENT_LOCAL_STATE_DOWN;
    }
    if (entry->local_state_init == true) {
        event_mask |= MCS_BFD_EVENT_LOCAL_STATE_INIT;
    }
    if (entry->local_state_up == true) {
        event_mask |= MCS_BFD_EVENT_LOCAL_STATE_UP;
    }
    if (entry->remote_state_mode_change == true) {
        event_mask |= MCS_BFD_EVENT_REMOTE_STATE_MODE_CHANGE;
    }
    if (entry->remote_discr_change == true) {
        event_mask |= MCS_BFD_EVENT_REMOTE_DISCRIMINATOR_CHANGE;
    }
    if (entry->remote_param_change == true) {
        event_mask |= MCS_BFD_EVENT_REMOTE_PARAMETER_CHANGE;
    }
    if (entry->remote_poll_bit_set == true) {
        event_mask |= MCS_BFD_EVENT_REMOTE_POLL_BIT_SET;
    }
    if (entry->remote_final_bit_set == true) {
        event_mask |= MCS_BFD_EVENT_REMOTE_FINAL_BIT_SET;
    }
    if (entry->misconn_defect == true) {
        event_mask |= MCS_BFD_EVENT_MISCONNECTIVITY_DEFECT;
    }
    if (entry->misconn_clear == true) {
        event_mask |= MCS_BFD_EVENT_MISCONNECTIVITY_DEFECT_CLEAR;
    }
    if (entry->unexpected_meg_defect == true) {
        event_mask |= MCS_BFD_EVENT_UNEXPECTED_MEG_DEFECT;
    }
    if (entry->unexpected_meg_defect_clear == true) {
        event_mask |= MCS_BFD_EVENT_UNEXPECTED_MEG_DEFECT_CLEAR;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bfd_uc_msg_send_receive(unit,
                                 MCS_MSG_SUBCLASS_BFD_EVENT_MASK_SET,
                                 event_mask, 0,
                                 MCS_MSG_SUBCLASS_BFD_EVENT_MASK_SET_REPLY,
                                 &reply_len,
                                 MCS_BFD_MAX_UC_MSG_TIMEOUT));
    if (reply_len != 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

 exit:
    SHR_FUNC_EXIT();
}
/*******************************************************************************
 * uC alloc and free messages from the SYSM
 */
int
bcmcth_oam_bfd_uc_alloc(int unit, bool warm)
{
    uint32_t ha_alloc_size = 0, ha_req_size = 0;
    bfd_info_t *bfd_info = NULL;
    bcmcth_oam_bfd_drv_t *bfd_drv = NULL;
    uint32_t int_id;
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    bfd_drv = bcmcth_oam_bfd_drv_get(unit);
    SHR_NULL_CHECK(bfd_drv, SHR_E_UNAVAIL);

    SHR_ALLOC(g_bfd_info[unit], sizeof(bfd_info_t), "bcmcthOamBfdInfo");
    bfd_info = g_bfd_info[unit];
    sal_memset(bfd_info, 0, sizeof(bfd_info_t));

    /* BFD control HA memory allocation. */
    ha_req_size = sizeof(bcmcth_oam_bfd_control_ha_t);
    ha_alloc_size = ha_req_size;

    bfd_info->ctrl = shr_ha_mem_alloc(unit, BCMMGMT_OAM_COMP_ID,
                                      BCMOAM_BFD_CTRL_SUB_COMP_ID,
                                      "oamBfdCtrl",
                                      &ha_alloc_size);
    SHR_NULL_CHECK(bfd_info->ctrl, SHR_E_MEMORY);

    SHR_IF_ERR_VERBOSE_EXIT
        ((ha_alloc_size < ha_req_size) ? SHR_E_MEMORY : SHR_E_NONE);

    if (!warm) {
        sal_memset(bfd_info->ctrl, 0, ha_alloc_size);

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmissu_struct_info_report(unit, BCMMGMT_OAM_COMP_ID,
                                        BCMOAM_BFD_CTRL_SUB_COMP_ID, 0,
                                        ha_req_size, 1,
                                        BCMCTH_OAM_BFD_CONTROL_HA_T_ID));

        bfd_info->ctrl->max_endpoints = bfd_drv->max_endpoints;
    }

    /* BFD auth SP HA memory allocation. */
    ha_req_size = sizeof(bcmcth_oam_bfd_auth_ha_t) * bfd_drv->max_auth_sp_keys;
    ha_alloc_size = ha_req_size;

    bfd_info->auth_sp = shr_ha_mem_alloc(unit, BCMMGMT_OAM_COMP_ID,
                                         BCMOAM_BFD_AUTH_SP_SUB_COMP_ID,
                                         "oamBfdAuthSp",
                                         &ha_alloc_size);
    SHR_NULL_CHECK(bfd_info->ctrl, SHR_E_MEMORY);

    SHR_IF_ERR_VERBOSE_EXIT
        ((ha_alloc_size < ha_req_size) ? SHR_E_MEMORY : SHR_E_NONE);

    SHR_ALLOC(bfd_info->auth_sp_int_id_bmp,
              SHR_BITALLOCSIZE(bfd_drv->max_auth_sp_keys),
              "bcmcthOamBfdAuthSpIntIdBmp");
    sal_memset(bfd_info->auth_sp_int_id_bmp, 0,
               SHR_BITALLOCSIZE(bfd_drv->max_auth_sp_keys));

    if (warm == true) {
        /* Recreate the internal Id bitmap. */
        for (i = 0; i < bfd_drv->max_auth_sp_keys; i++) {
            if (bfd_info->auth_sp[i].active == true) {
                int_id = bfd_info->auth_sp[i].int_id;
                SHR_BITSET(bfd_info->auth_sp_int_id_bmp, int_id);
            }
        }
    } else {
        sal_memset(bfd_info->auth_sp, 0, ha_alloc_size);

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmissu_struct_info_report(unit, BCMMGMT_OAM_COMP_ID,
                                              BCMOAM_BFD_AUTH_SP_SUB_COMP_ID, 0,
                                              sizeof(bcmcth_oam_bfd_auth_ha_t),
                                              bfd_drv->max_auth_sp_keys,
                                              BCMCTH_OAM_BFD_AUTH_HA_T_ID));
    }

    /* BFD auth SHA1 HA memory allocation. */
    ha_req_size = sizeof(bcmcth_oam_bfd_auth_ha_t) * bfd_drv->max_auth_sha1_keys;
    ha_alloc_size = ha_req_size;

    bfd_info->auth_sha1 = shr_ha_mem_alloc(unit, BCMMGMT_OAM_COMP_ID,
                                         BCMOAM_BFD_AUTH_SHA1_SUB_COMP_ID,
                                         "oamBfdAuthSha1",
                                         &ha_alloc_size);
    SHR_NULL_CHECK(bfd_info->ctrl, SHR_E_MEMORY);

    SHR_IF_ERR_VERBOSE_EXIT
        ((ha_alloc_size < ha_req_size) ? SHR_E_MEMORY : SHR_E_NONE);

    SHR_ALLOC(bfd_info->auth_sha1_int_id_bmp,
              SHR_BITALLOCSIZE(bfd_drv->max_auth_sha1_keys),
              "bcmcthOamBfdAuthSha1IntIdBmp");
    sal_memset(bfd_info->auth_sha1_int_id_bmp, 0,
               SHR_BITALLOCSIZE(bfd_drv->max_auth_sha1_keys));

    if (warm == true) {
        /* Recreate the internal Id bitmap. */
        for (i = 0; i < bfd_drv->max_auth_sha1_keys; i++) {
            if (bfd_info->auth_sha1[i].active == true) {
                int_id = bfd_info->auth_sha1[i].int_id;
                SHR_BITSET(bfd_info->auth_sha1_int_id_bmp, int_id);
            }
        }
    } else {
        sal_memset(bfd_info->auth_sha1, 0, ha_alloc_size);

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmissu_struct_info_report(unit, BCMMGMT_OAM_COMP_ID,
                                              BCMOAM_BFD_AUTH_SHA1_SUB_COMP_ID,
                                              0,
                                              sizeof(bcmcth_oam_bfd_auth_ha_t),
                                              bfd_drv->max_auth_sha1_keys,
                                              BCMCTH_OAM_BFD_AUTH_HA_T_ID));
    }


    /* Endpoints HA memory allocation. */
    ha_req_size = sizeof(bcmcth_oam_bfd_endpoint_ha_t) * bfd_info->ctrl->max_endpoints;
    ha_alloc_size = ha_req_size;

    bfd_info->endpoints = shr_ha_mem_alloc(unit, BCMMGMT_OAM_COMP_ID,
                                           BCMOAM_BFD_ENDPOINTS_SUB_COMP_ID,
                                           "oamBfdEndp",
                                           &ha_alloc_size);
    SHR_NULL_CHECK(bfd_info->endpoints, SHR_E_MEMORY);

    SHR_IF_ERR_VERBOSE_EXIT
        ((ha_alloc_size < ha_req_size) ? SHR_E_MEMORY : SHR_E_NONE);

    if (!warm) {
        sal_memset(bfd_info->endpoints, 0, ha_alloc_size);

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmissu_struct_info_report(unit, BCMMGMT_OAM_COMP_ID,
                                              BCMOAM_BFD_ENDPOINTS_SUB_COMP_ID,
                                              0,
                                              sizeof(bcmcth_oam_bfd_endpoint_ha_t),
                                              bfd_drv->max_endpoints,
                                              BCMCTH_OAM_BFD_ENDPOINT_HA_T_ID));
    }

    if (bcmdrd_feature_is_sim(unit) == false) {
        /* Allocate DMA buffer */
        bfd_info->dma_buffer = bcmdrd_hal_dma_alloc(unit,
                                                    sizeof(mcs_bfd_msg_ctrl_t),
                                                    "BFD DMA buffer",
                                                    &(bfd_info->dma_buffer_p_addr));
        SHR_NULL_CHECK(bfd_info->dma_buffer, SHR_E_MEMORY);
    }

    if (bfd_info->ctrl->run == true) {
        /* This condition will only be true if this was a warm-boot and the app
         * is running. Respawn the event thread.
         */
        SHR_IF_ERR_VERBOSE_EXIT(bfd_event_thread_start(unit, warm));
    }
exit:
    if (SHR_FUNC_ERR()) {
        if (g_bfd_info[unit] != NULL) {
            if (g_bfd_info[unit]->ctrl != NULL) {
                shr_ha_mem_free(unit, g_bfd_info[unit]->ctrl);
                g_bfd_info[unit]->ctrl = NULL;
            }
            if (g_bfd_info[unit]->auth_sp != NULL) {
                shr_ha_mem_free(unit, g_bfd_info[unit]->auth_sp);
                g_bfd_info[unit]->auth_sp = NULL;
            }
            SHR_FREE(g_bfd_info[unit]->auth_sp_int_id_bmp);
            if (g_bfd_info[unit]->auth_sha1 != NULL) {
                shr_ha_mem_free(unit, g_bfd_info[unit]->auth_sha1);
                g_bfd_info[unit]->auth_sha1 = NULL;
            }
            SHR_FREE(g_bfd_info[unit]->auth_sha1_int_id_bmp);
            if (g_bfd_info[unit]->endpoints != NULL) {
                shr_ha_mem_free(unit, g_bfd_info[unit]->endpoints);
                g_bfd_info[unit]->endpoints = NULL;
            }
            if (g_bfd_info[unit]->dma_buffer != NULL) {
                bcmdrd_hal_dma_free(unit, sizeof(mcs_bfd_msg_ctrl_t),
                                    g_bfd_info[unit]->dma_buffer,
                                    g_bfd_info[unit]->dma_buffer_p_addr);
                g_bfd_info[unit]->dma_buffer = NULL;
                g_bfd_info[unit]->dma_buffer_p_addr = 0;
            }
            SHR_FREE(g_bfd_info[unit]);
        }
    }
    SHR_FUNC_EXIT();
}

void
bcmcth_oam_bfd_uc_free(int unit)
{
    if (g_bfd_info[unit] == NULL) {
        return;
    }

    SHR_FREE(g_bfd_info[unit]->auth_sp_int_id_bmp);
    SHR_FREE(g_bfd_info[unit]->auth_sha1_int_id_bmp);

    if (g_bfd_info[unit]->dma_buffer != NULL) {
        bcmdrd_hal_dma_free(unit, sizeof(mcs_bfd_msg_ctrl_t),
                            g_bfd_info[unit]->dma_buffer,
                            g_bfd_info[unit]->dma_buffer_p_addr);
    }

    SHR_FREE(g_bfd_info[unit]);
}

/*******************************************************************************
 * Private functions
 */
static uint16_t
udp_dst_port_get(bcmltd_common_oam_bfd_ip_endpoint_type_t_t ip_ep_type)
{
    switch (ip_ep_type) {
        case IP_ENDPOINT_TYPE(SINGLE_HOP):
            return BFD_UDP_DST_PORT_SINGLE_HOP;

        case IP_ENDPOINT_TYPE(MULTI_HOP):
            return BFD_UDP_DST_PORT_MULTI_HOP;

        case IP_ENDPOINT_TYPE(MICRO):
            return BFD_UDP_DST_PORT_MICRO;

        default:
            /* This condition will never happen. */
            assert(0);
            return 0;
    }
}

/*
 * Get the IP ttl from the MPLS encap type, return 0 in case IP header is not
 * applicable for the encap type.
 */
static uint8_t
tnl_mpls_ip_ttl_get(bcmltd_common_oam_bfd_tnl_mpls_endpoint_encap_type_t_t encap_type)
{
    switch (encap_type) {
        case TNL_MPLS_ENDPOINT_ENCAP_TYPE(MPLS_LSP):
        case TNL_MPLS_ENDPOINT_ENCAP_TYPE(MPLS_LSP_PHP):
            return 1;

        case TNL_MPLS_ENDPOINT_ENCAP_TYPE(PW):
            return 255;

        default:
            return 0;
    }
}

/*
 * Get the ACH channel type from the MPLS encap type the IP encap type. Return 0
 * if ACH is not applicable.
 */
static uint16_t
ach_channel_type_get(bcmltd_common_oam_bfd_tnl_mpls_endpoint_encap_type_t_t encap_type,
                     bcmltd_common_oam_bfd_tnl_mpls_endpoint_ip_encap_type_t_t ip_encap_type)
{
    switch (encap_type) {
        case TNL_MPLS_ENDPOINT_ENCAP_TYPE(PW):
            if (ip_encap_type == TNL_MPLS_ENDPOINT_IP_ENCAP_TYPE(IPV4)) {
                return BFD_ACH_CHANNEL_TYPE_IPV4;
            } else if (ip_encap_type == TNL_MPLS_ENDPOINT_IP_ENCAP_TYPE(IPV6)) {
                return BFD_ACH_CHANNEL_TYPE_IPV6;
            } else {
                return BFD_ACH_CHANNEL_TYPE_RAW;
            }

        case TNL_MPLS_ENDPOINT_ENCAP_TYPE(MPLS_TP_CC):
        case TNL_MPLS_ENDPOINT_ENCAP_TYPE(MPLS_TP_CC_CV):
            /*
             * In case of CC_CV, return the channel type corresponding to CC,
             * Eapp will modify the encap to use CV channel type when required.
             */
            return BFD_ACH_CHANNEL_TYPE_MPLS_TP_CC;

        default:
            return 0;

    }
}

/*******************************************************************************
 * Event handling
 */

/* Event thread entry point. */
static void
bfd_event_thread(shr_thread_t tc, void *arg)
{
    int unit = BFD_DECODE_UNIT(arg);
    bool warm = BFD_DECODE_WB(arg);
    bfd_info_t *bfd_info = g_bfd_info[unit];
    mcs_msg_data_t msg;
    uint32_t ep_id;
    uint32_t event_bmp;
    int num_events;
    bcmltd_common_oam_bfd_endpoint_event_t_t events[32];
    int rv;
    bcmcth_oam_bfd_endpoint_ha_t *ep_info;

    while (1) {
        rv = bcmbd_mcs_msg_receive(unit, bfd_info->ctrl->uc_num,
                                   MCS_MSG_CLASS_BFD_EVENT,
                                   &msg, SHR_THREAD_FOREVER);
        if (shr_thread_stopping(tc)) {
            /* Thread is stopping. */
            break;
        }
        if ((rv == SHR_E_INIT) && (warm == TRUE)) {
            /* In case of warmboot, wait until messaging system is up */
            continue;
        }
        warm = FALSE;

        if (rv != SHR_E_NONE) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "bcmbd_mcs_msg_receive returned %d \n"),
                       rv));
            continue;
        }

        /* Get data from event message */
        ep_id = MCS_MSG_LEN_GET(msg);
        event_bmp = MCS_MSG_DATA_GET(msg);

        num_events = 0;

        if (event_bmp & MCS_BFD_EVENT_LOCAL_STATE_ADMIN_DOWN) {
            events[num_events] = ENDPOINT_EVENT(LOCAL_STATE_ADMIN_DOWN);
            num_events++;
        }
        if (event_bmp & MCS_BFD_EVENT_LOCAL_STATE_DOWN) {
            events[num_events] = ENDPOINT_EVENT(LOCAL_STATE_DOWN);
            num_events++;
        }
        if (event_bmp & MCS_BFD_EVENT_LOCAL_STATE_INIT) {
            events[num_events] = ENDPOINT_EVENT(LOCAL_STATE_INIT);
            num_events++;
        }
        if (event_bmp & MCS_BFD_EVENT_LOCAL_STATE_UP) {
            events[num_events] = ENDPOINT_EVENT(LOCAL_STATE_UP);
            num_events++;
        }
        if (event_bmp & MCS_BFD_EVENT_REMOTE_STATE_MODE_CHANGE) {
            events[num_events] = ENDPOINT_EVENT(REMOTE_STATE_MODE_CHANGE);
            num_events++;
        }
        if (event_bmp & MCS_BFD_EVENT_REMOTE_DISCRIMINATOR_CHANGE) {
            events[num_events] = ENDPOINT_EVENT(REMOTE_DISCRIMINATOR_CHANGE);
            num_events++;
        }
        if (event_bmp & MCS_BFD_EVENT_REMOTE_PARAMETER_CHANGE) {
            events[num_events] = ENDPOINT_EVENT(REMOTE_PARAMETER_CHANGE);
            num_events++;
        }
        if (event_bmp & MCS_BFD_EVENT_REMOTE_POLL_BIT_SET) {
            events[num_events] = ENDPOINT_EVENT(REMOTE_POLL_BIT_SET);
            num_events++;
        }
        if (event_bmp & MCS_BFD_EVENT_REMOTE_FINAL_BIT_SET) {
            events[num_events] = ENDPOINT_EVENT(REMOTE_FINAL_BIT_SET );
            num_events++;
        }
        if (event_bmp & MCS_BFD_EVENT_MISCONNECTIVITY_DEFECT) {
            events[num_events] = ENDPOINT_EVENT(MISCONNECTIVITY_DEFECT);
            num_events++;
        }
        if (event_bmp & MCS_BFD_EVENT_MISCONNECTIVITY_DEFECT_CLEAR) {
            events[num_events] = ENDPOINT_EVENT(MISCONNECTIVITY_DEFECT_CLEAR);
            num_events++;
        }
        if (event_bmp & MCS_BFD_EVENT_UNEXPECTED_MEG_DEFECT) {
            events[num_events] = ENDPOINT_EVENT(UNEXPECTED_MEG_DEFECT);
            num_events++;
        }
        if (event_bmp & MCS_BFD_EVENT_UNEXPECTED_MEG_DEFECT_CLEAR) {
            events[num_events] = ENDPOINT_EVENT(UNEXPECTED_MEG_DEFECT_CLEAR);
            num_events++;
        }

        ep_info = bfd_endpoint_info_get(unit, ep_id);
        bcmcth_oam_bfd_event_report(unit, ep_info->ep_type, ep_id,
                                    num_events, events);
    }
    return;
}

/*******************************************************************************
 * IPv4 endpoint type specific functions.
 */
static uint16_t
bfd_endpoint_ipv4_encap_len_get(void *type)
{
    return (SHR_IPV4_HEADER_LENGTH + SHR_UDP_HDR_LEN);
}

static void
bfd_endpoint_ipv4_encap_pack(void *type, uint16_t udp_src_port,
                             uint16_t *length, uint8_t *encap,
                             uint8_t **lkey, uint16_t *lkey_length,
                             uint16_t bfd_pkt_length)
{
    bcmcth_oam_bfd_endpoint_ipv4_t *ep;
    shr_util_pack_ipv4_header_t ipv4;
    shr_util_pack_udp_header_t udp;
    uint8_t *cur_ptr = encap;
    uint8_t *ptr_temp;

    ep = (bcmcth_oam_bfd_endpoint_ipv4_t *) type;

    /*
     * cur_ptr is at the start of IPv4 header, find the offset to the Dst IPv4
     * address and set it as the lookup key.
     */
    *lkey = cur_ptr + IPV4_HDR_DST_IP_OFFSET;
    *lkey_length = SHR_IPV4_ADDR_LEN;

    /* IPv4 header. */
    sal_memset(&ipv4, 0, sizeof(ipv4));
    ipv4.version = SHR_IPV4_VERSION;
    ipv4.h_length = SHR_IPV4_HDR_WLEN;
    ipv4.dscp = ep->tos;
    ipv4.ttl = ep->ttl;
    ipv4.length = bfd_pkt_length + SHR_UDP_HDR_LEN + SHR_IPV4_HEADER_LENGTH;
    ipv4.protocol = SHR_IP_PROTO_UDP;
    ipv4.src = ep->src_ip;
    ipv4.dst = ep->dst_ip;
    ptr_temp = cur_ptr;
    cur_ptr = shr_util_pack_ipv4_header_pack(cur_ptr, &ipv4);

    ipv4.sum = shr_ip_chksum(SHR_IPV4_HEADER_LENGTH, ptr_temp);
    cur_ptr = shr_util_pack_ipv4_header_pack(ptr_temp, &ipv4);

    /* UDP header. */
    ptr_temp = cur_ptr;
    sal_memset(&udp, 0, sizeof(udp));
    udp.src = udp_src_port;
    udp.dst = udp_dst_port_get(ep->ip_ep_type);
    udp.length = bfd_pkt_length + SHR_UDP_HDR_LEN;
    cur_ptr = shr_util_pack_udp_header_pack(cur_ptr, &udp);

    udp.sum = shr_ip_chksum(SHR_UDP_HDR_LEN, ptr_temp);
    cur_ptr = shr_util_pack_udp_header_pack(ptr_temp, &udp);
    *length = cur_ptr - encap;
}

static mcs_bfd_encap_types_t
bfd_endpoint_ipv4_mcs_encap_type_get(void *type)
{
    return MCS_BFD_ENCAP_TYPE_V4UDP;
}

static void
bfd_endpoint_ipv4_msg_convert(void *type,
                              mcs_bfd_msg_ctrl_ep_set_t *msg)
{
    bcmcth_oam_bfd_endpoint_ipv4_t *ep;

    ep = (bcmcth_oam_bfd_endpoint_ipv4_t *) type;

    if (ep->ip_ep_type == IP_ENDPOINT_TYPE(MICRO)) {
        msg->flags |= MCS_BFD_EP_SET_F_MICRO_BFD;
    }

    msg->rx_pkt_vlan_id = ep->rx_lookup_vlan_id;
}

static uint32_t
bfd_endpoint_ipv4_update_flags_get(void *type1, void *type2)
{
    bcmcth_oam_bfd_endpoint_ipv4_t *ep1, *ep2;
    uint32_t flags = 0;

    ep1 = (bcmcth_oam_bfd_endpoint_ipv4_t *) type1;
    ep2 = (bcmcth_oam_bfd_endpoint_ipv4_t *) type2;

    if (ep1->ip_ep_type != ep2->ip_ep_type) {
        /* UDP destination port would have changed. */
        flags |= MCS_BFD_EP_SET_UPDATE_F_ENCAP;
    }

    if (sal_memcmp(ep1, ep2, sizeof(bcmcth_oam_bfd_endpoint_ipv4_t)) != 0) {
        flags |= MCS_BFD_EP_SET_UPDATE_F_ENCAP;
    }

    return flags;
}

/*******************************************************************************
 * IPv6 endpoint type specific functions.
 */
static uint16_t
bfd_endpoint_ipv6_encap_len_get(void *type)
{
    return (SHR_IPV6_HEADER_LENGTH + SHR_UDP_HDR_LEN);
}

static void
bfd_endpoint_ipv6_encap_pack(void *type, uint16_t udp_src_port,
                             uint16_t *length, uint8_t *encap,
                             uint8_t **lkey, uint16_t *lkey_length,
                             uint16_t bfd_pkt_length)
{
    bcmcth_oam_bfd_endpoint_ipv6_t *ep;
    shr_util_pack_ipv6_header_t ipv6;
    shr_util_pack_udp_header_t udp;
    uint8_t *cur_ptr = encap;

    ep = (bcmcth_oam_bfd_endpoint_ipv6_t *) type;

    /*
     * cur_ptr is at the start of IPv6 header, find the offset to the Dst Ipv6
     * address and set it as the lookup key.
     */
    *lkey = cur_ptr + IPV6_HDR_DST_IP_OFFSET;
    *lkey_length = SHR_IPV6_ADDR_LEN;

    /* IPv6 header. */
    sal_memset(&ipv6, 0, sizeof(ipv6));
    ipv6.version = SHR_IPV6_VERSION;
    ipv6.t_class = ep->traffic_class;
    ipv6.f_label = 0;
    ipv6.next_header = SHR_IP_PROTO_UDP;
    ipv6.hop_limit = ep->hop_limit;
    ipv6.p_length = bfd_pkt_length + SHR_UDP_HDR_LEN + SHR_IPV6_HEADER_LENGTH;
    sal_memcpy(ipv6.src, ep->src_ip, sizeof(shr_ip6_t));
    sal_memcpy(ipv6.dst, ep->dst_ip, sizeof(shr_ip6_t));
    cur_ptr = shr_util_pack_ipv6_header_pack(cur_ptr, &ipv6);

    /* UDP header. */
    sal_memset(&udp, 0, sizeof(udp));
    udp.src = udp_src_port;
    udp.dst = udp_dst_port_get(ep->ip_ep_type);
    udp.length = bfd_pkt_length + SHR_UDP_HDR_LEN;
    cur_ptr = shr_util_pack_udp_header_pack(cur_ptr, &udp);
    *length = cur_ptr - encap;
}

static mcs_bfd_encap_types_t
bfd_endpoint_ipv6_mcs_encap_type_get(void *type)
{
    return MCS_BFD_ENCAP_TYPE_V6UDP;
}

static void
bfd_endpoint_ipv6_msg_convert(void *type, mcs_bfd_msg_ctrl_ep_set_t *msg)
{
    bcmcth_oam_bfd_endpoint_ipv6_t *ep;

    ep = (bcmcth_oam_bfd_endpoint_ipv6_t *) type;

    if (ep->ip_ep_type == IP_ENDPOINT_TYPE(MICRO)) {
        msg->flags |= MCS_BFD_EP_SET_F_MICRO_BFD;
    }

    msg->rx_pkt_vlan_id = ep->rx_lookup_vlan_id;
}

static uint32_t
bfd_endpoint_ipv6_update_flags_get(void *type1, void *type2)
{
    bcmcth_oam_bfd_endpoint_ipv6_t *ep1, *ep2;
    uint32_t flags = 0;

    ep1 = (bcmcth_oam_bfd_endpoint_ipv6_t *) type1;
    ep2 = (bcmcth_oam_bfd_endpoint_ipv6_t *) type2;


    if (ep1->ip_ep_type != ep2->ip_ep_type) {
        /* UDP destination port would have changed. */
        flags |= MCS_BFD_EP_SET_UPDATE_F_ENCAP;
    }

    if (sal_memcmp(ep1, ep2, sizeof(bcmcth_oam_bfd_endpoint_ipv6_t)) != 0) {
        flags |= MCS_BFD_EP_SET_UPDATE_F_ENCAP;
    }

    return flags;
}

/*******************************************************************************
 * Tunnel IPv4 endpoint type specific functions.
 */
static uint16_t
bfd_endpoint_tnl_ipv4_encap_len_get(void *type)
{
    bcmcth_oam_bfd_endpoint_tnl_ipv4_t *ep;
    uint16_t length = 0;

    ep = (bcmcth_oam_bfd_endpoint_tnl_ipv4_t *) type;

    length += SHR_IPV4_HEADER_LENGTH;

    if (ep->gre == true) {
        length += GRE_HDR_LEN;
    }

    if (ep->inner_ip_type == INNER_IP_TYPE(IPV4)) {
        length += SHR_IPV4_HEADER_LENGTH;
    } else {
        length += SHR_IPV6_HEADER_LENGTH;
    }

    length += SHR_UDP_HDR_LEN;

    return length;
}

static void
bfd_endpoint_tnl_ipv4_encap_pack(void *type, uint16_t udp_src_port,
                                 uint16_t *length, uint8_t *encap,
                                 uint8_t **lkey, uint16_t *lkey_length,
                                 uint16_t bfd_pkt_length)
{
    bcmcth_oam_bfd_endpoint_tnl_ipv4_t *ep;
    shr_util_pack_ipv4_header_t ipv4;
    shr_util_pack_ipv6_header_t ipv6;
    shr_util_pack_gre_header_t gre;
    shr_util_pack_udp_header_t udp;
    int ip_length = 0;
    uint8_t *cur_ptr = encap;
    uint8_t *ptr_tmp;

    ep = (bcmcth_oam_bfd_endpoint_tnl_ipv4_t *) type;

    /*
     * cur_ptr is at the start of IPv4 header, find the offset to the Dst IPv4
     * address and set it as the lookup key.
     */
    *lkey = cur_ptr + IPV4_HDR_DST_IP_OFFSET;
    *lkey_length = SHR_IPV4_ADDR_LEN;

    /* Outer IPv4 header. */
    sal_memset(&ipv4, 0, sizeof(ipv4));
    ipv4.version = SHR_IPV4_VERSION;
    ipv4.h_length = SHR_IPV4_HDR_WLEN;
    ipv4.dscp = ep->tos;
    ipv4.ttl = ep->ttl;

    ip_length = bfd_pkt_length + SHR_UDP_HDR_LEN + SHR_IPV4_HEADER_LENGTH;
    if (ep->gre == true) {
        ip_length += SHR_GRE_HDR_LEN;
    }
    if (ep->inner_ip_type == INNER_IP_TYPE(IPV4)) {
        ip_length += SHR_IPV4_HEADER_LENGTH;
    }
    if (ep->inner_ip_type == INNER_IP_TYPE(IPV6)) {
        ip_length += SHR_IPV6_HEADER_LENGTH;
    }
    if (ep->gre == true) {
        /* Next header is GRE. */
        ipv4.protocol = SHR_IP_PROTO_GRE;
    } else if (ep->inner_ip_type == INNER_IP_TYPE(IPV4)) {
        /* Next header is IPv4. */
        ipv4.protocol = SHR_IP_PROTO_IPV4;
    } else {
        /* Next header is IPv6. */
        ipv4.protocol = SHR_IP_PROTO_IPV6;
    }
    ipv4.length = ip_length;
    ipv4.src = ep->src_ip;
    ipv4.dst = ep->dst_ip;
    ptr_tmp = cur_ptr;
    cur_ptr = shr_util_pack_ipv4_header_pack(cur_ptr, &ipv4);
    ipv4.sum = shr_ip_chksum(SHR_IPV4_HEADER_LENGTH, cur_ptr);
    cur_ptr = shr_util_pack_ipv4_header_pack(ptr_tmp, &ipv4);

    /* GRE header. */
    if (ep->gre == true) {
        sal_memset(&gre, 0, sizeof(gre));
        if (ep->inner_ip_type == INNER_IP_TYPE(IPV4)) {
            gre.protocol = SHR_L2_ETYPE_IPV4;
        } else {
            gre.protocol = SHR_L2_ETYPE_IPV6;
        }
        cur_ptr = shr_util_pack_gre_header_pack(cur_ptr, &gre);
    }

    if (ep->inner_ip_type == INNER_IP_TYPE(IPV4)) {
        /* Inner IPv4 header. */
        sal_memset(&ipv4, 0, sizeof(ipv4));
        ipv4.version = SHR_IPV4_VERSION;
        ipv4.h_length = SHR_IPV4_HDR_WLEN;
        ipv4.dscp = ep->inner_ipv4.tos;
        ipv4.ttl = 255;
        ipv4.protocol = SHR_IP_PROTO_UDP;
        ipv4.src = ep->inner_ipv4.src_ip;
        ipv4.dst = ep->inner_ipv4.dst_ip;
        ipv4.length = bfd_pkt_length + SHR_UDP_HDR_LEN + SHR_IPV4_HEADER_LENGTH;
        ptr_tmp = cur_ptr;
        cur_ptr = shr_util_pack_ipv4_header_pack(cur_ptr, &ipv4);
        ipv4.sum = shr_ip_chksum(SHR_IPV4_HEADER_LENGTH, ptr_tmp);
        cur_ptr = shr_util_pack_ipv4_header_pack(ptr_tmp, &ipv4);

    } else {
        /* Inner IPv6 header. */
        sal_memset(&ipv6, 0, sizeof(ipv6));
        ipv6.version = SHR_IPV6_VERSION;
        ipv6.t_class = ep->inner_ipv6.traffic_class;
        ipv6.f_label = 0;
        ipv6.next_header = SHR_IP_PROTO_UDP;
        ipv6.hop_limit = 255;
        ipv6.p_length = bfd_pkt_length + SHR_UDP_HDR_LEN;
        sal_memcpy(ipv6.src, ep->inner_ipv6.src_ip, sizeof(shr_ip6_t));
        sal_memcpy(ipv6.dst, ep->inner_ipv6.dst_ip, sizeof(shr_ip6_t));
        cur_ptr = shr_util_pack_ipv6_header_pack(cur_ptr, &ipv6);
    }

    /* UDP header. */
    sal_memset(&udp, 0, sizeof(udp));
    udp.src = udp_src_port;
    udp.dst = BFD_UDP_DST_PORT_SINGLE_HOP;
    udp.length = bfd_pkt_length + SHR_UDP_HDR_LEN;
    ptr_tmp = cur_ptr;
    cur_ptr = shr_util_pack_udp_header_pack(cur_ptr, &udp);
    udp.sum = shr_ip_chksum(SHR_UDP_HDR_LEN, ptr_tmp);
    cur_ptr = shr_util_pack_udp_header_pack(ptr_tmp, &udp);
    *length = cur_ptr - encap;
}

static mcs_bfd_encap_types_t
bfd_endpoint_tnl_ipv4_mcs_encap_type_get(void *type)
{
    return MCS_BFD_ENCAP_TYPE_V4UDP;
}

static void
bfd_endpoint_tnl_ipv4_msg_convert(void *type, mcs_bfd_msg_ctrl_ep_set_t *msg)
{
    bcmcth_oam_bfd_endpoint_tnl_ipv4_t *ep;

    ep = (bcmcth_oam_bfd_endpoint_tnl_ipv4_t *) type;
    msg->rx_pkt_vlan_id = ep->rx_lookup_vlan_id;
}

static uint32_t
bfd_endpoint_tnl_ipv4_update_flags_get(void *type1, void *type2)
{
    bcmcth_oam_bfd_endpoint_tnl_ipv4_t *ep1, *ep2;
    uint32_t flags = 0;

    ep1 = (bcmcth_oam_bfd_endpoint_tnl_ipv4_t *) type1;
    ep2 = (bcmcth_oam_bfd_endpoint_tnl_ipv4_t *) type2;

    if (sal_memcmp(ep1, ep2, sizeof(bcmcth_oam_bfd_endpoint_tnl_ipv4_t)) != 0) {
        flags |= MCS_BFD_EP_SET_UPDATE_F_ENCAP;
    }

    return flags;
}

/*******************************************************************************
 * Tunnel IPv6 endpoint type specific functions.
 */
static uint16_t
bfd_endpoint_tnl_ipv6_encap_len_get(void *type)
{
    bcmcth_oam_bfd_endpoint_tnl_ipv6_t *ep;
    uint16_t length = 0;

    ep = (bcmcth_oam_bfd_endpoint_tnl_ipv6_t *) type;

    length += SHR_IPV6_HEADER_LENGTH;

    if (ep->gre == true) {
        length += GRE_HDR_LEN;
    }

    if (ep->inner_ip_type == INNER_IP_TYPE(IPV4)) {
        length += SHR_IPV4_HEADER_LENGTH;
    } else {
        length += SHR_IPV6_HEADER_LENGTH;
    }

    length += SHR_UDP_HDR_LEN;

    return length;
}

static void
bfd_endpoint_tnl_ipv6_encap_pack(void *type, uint16_t udp_src_port,
                                 uint16_t *length, uint8_t *encap,
                                 uint8_t **lkey, uint16_t *lkey_length,
                                 uint16_t bfd_pkt_length)
{
    bcmcth_oam_bfd_endpoint_tnl_ipv6_t *ep;
    shr_util_pack_ipv4_header_t ipv4;
    shr_util_pack_ipv6_header_t ipv6;
    shr_util_pack_gre_header_t gre;
    shr_util_pack_udp_header_t udp;
    uint8_t *cur_ptr = encap;
    int ip_length = 0;
    uint8_t *ptr_tmp = encap;

    ep = (bcmcth_oam_bfd_endpoint_tnl_ipv6_t *) type;

    /*
     * cur_ptr is at the start of IPv6 header, find the offset to the Dst IPv6
     * address and set it as the lookup key.
     */
    *lkey = cur_ptr + IPV6_HDR_DST_IP_OFFSET;
    *lkey_length = SHR_IPV6_ADDR_LEN;

    /* Outer IPv6 header. */
    sal_memset(&ipv6, 0, sizeof(ipv6));
    ipv6.version = SHR_IPV6_VERSION;
    ipv6.t_class = ep->traffic_class;
    ipv6.f_label = 0;

    ip_length = bfd_pkt_length + SHR_UDP_HDR_LEN;
    if (ep->gre == true) {
        ip_length += SHR_GRE_HDR_LEN;
    }
    if (ep->inner_ip_type == INNER_IP_TYPE(IPV4)) {
        ip_length += SHR_IPV4_HEADER_LENGTH;
    }
    if (ep->inner_ip_type == INNER_IP_TYPE(IPV6)) {
        ip_length += SHR_IPV6_HEADER_LENGTH;
    }


    ipv6.p_length = ip_length;
    if (ep->gre == true) {
        /* Next header is GRE. */
        ipv6.next_header = SHR_IP_PROTO_GRE;
    } else if (ep->inner_ip_type == INNER_IP_TYPE(IPV4)) {
        /* Next header is IPv4. */
        ipv6.next_header = SHR_IP_PROTO_IPV4;
    } else {
        /* Next header is IPv6. */
        ipv6.next_header = SHR_IP_PROTO_IPV6;
    }
    ipv6.hop_limit = ep->hop_limit;
    sal_memcpy(ipv6.src, ep->src_ip, sizeof(shr_ip6_t));
    sal_memcpy(ipv6.dst, ep->dst_ip, sizeof(shr_ip6_t));
    cur_ptr = shr_util_pack_ipv6_header_pack(cur_ptr, &ipv6);

    /* GRE header. */
    if (ep->gre == true) {
        sal_memset(&gre, 0, sizeof(gre));
        if (ep->inner_ip_type == INNER_IP_TYPE(IPV4)) {
            gre.protocol = SHR_L2_ETYPE_IPV4;
        } else {
            gre.protocol = SHR_L2_ETYPE_IPV6;
        }
        cur_ptr = shr_util_pack_gre_header_pack(cur_ptr, &gre);
    }

    if (ep->inner_ip_type == INNER_IP_TYPE(IPV4)) {
        /* Inner IPv4 header. */
        sal_memset(&ipv4, 0, sizeof(ipv4));
        ipv4.version = SHR_IPV4_VERSION;
        ipv4.h_length = SHR_IPV4_HDR_WLEN;
        ipv4.dscp = ep->inner_ipv4.tos;
        ipv4.ttl = 255;
        ipv4.protocol = SHR_IP_PROTO_UDP;
        ipv4.src = ep->inner_ipv4.src_ip;
        ipv4.dst = ep->inner_ipv4.dst_ip;
        ipv4.length = bfd_pkt_length + SHR_UDP_HDR_LEN + SHR_IPV4_HEADER_LENGTH;
        ptr_tmp = cur_ptr;
        cur_ptr = shr_util_pack_ipv4_header_pack(cur_ptr, &ipv4);
        ipv4.sum = shr_ip_chksum(SHR_IPV4_HEADER_LENGTH, cur_ptr);
        cur_ptr = shr_util_pack_ipv4_header_pack(ptr_tmp, &ipv4);
    } else {
        /* Inner IPv6 header. */
        sal_memset(&ipv6, 0, sizeof(ipv6));
        ipv6.version = SHR_IPV6_VERSION;
        ipv6.t_class = ep->inner_ipv6.traffic_class;
        ipv6.f_label = 0;
        ipv6.next_header = SHR_IP_PROTO_UDP;
        ipv6.hop_limit = 255;
        ipv6.p_length = bfd_pkt_length + SHR_UDP_HDR_LEN;
        sal_memcpy(ipv6.src, ep->inner_ipv6.src_ip, sizeof(shr_ip6_t));
        sal_memcpy(ipv6.dst, ep->inner_ipv6.dst_ip, sizeof(shr_ip6_t));
        cur_ptr = shr_util_pack_ipv6_header_pack(cur_ptr, &ipv6);
    }

    /* UDP header. */
    sal_memset(&udp, 0, sizeof(udp));
    udp.src = udp_src_port;
    udp.dst = BFD_UDP_DST_PORT_SINGLE_HOP;
    udp.length = bfd_pkt_length + SHR_UDP_HDR_LEN;
    ptr_tmp = cur_ptr;
    cur_ptr = shr_util_pack_udp_header_pack(cur_ptr, &udp);
    udp.sum = shr_ip_chksum(SHR_UDP_HDR_LEN, cur_ptr);
    cur_ptr = shr_util_pack_udp_header_pack(ptr_tmp, &udp);
    *length = cur_ptr - encap;
}

static mcs_bfd_encap_types_t
bfd_endpoint_tnl_ipv6_mcs_encap_type_get(void *type)
{
    return MCS_BFD_ENCAP_TYPE_V6UDP;
}

static void
bfd_endpoint_tnl_ipv6_msg_convert(void *type, mcs_bfd_msg_ctrl_ep_set_t *msg)
{
    bcmcth_oam_bfd_endpoint_tnl_ipv4_t *ep;

    ep = (bcmcth_oam_bfd_endpoint_tnl_ipv4_t *) type;
    msg->rx_pkt_vlan_id = ep->rx_lookup_vlan_id;
}

static uint32_t
bfd_endpoint_tnl_ipv6_update_flags_get(void *type1, void *type2)
{
    bcmcth_oam_bfd_endpoint_tnl_ipv6_t *ep1, *ep2;
    uint32_t flags = 0;

    ep1 = (bcmcth_oam_bfd_endpoint_tnl_ipv6_t *) type1;
    ep2 = (bcmcth_oam_bfd_endpoint_tnl_ipv6_t *) type2;

    if (sal_memcmp(ep1, ep2, sizeof(bcmcth_oam_bfd_endpoint_tnl_ipv6_t)) != 0) {
        flags |= MCS_BFD_EP_SET_UPDATE_F_ENCAP;
    }

    return flags;
}

/*******************************************************************************
 * Tunnel MPLS endpoint type specific functions.
 */
static uint16_t
bfd_endpoint_tnl_mpls_encap_len_get(void *type)
{
    bcmcth_oam_bfd_endpoint_tnl_mpls_t *ep;
    uint16_t length = 0;

    ep = (bcmcth_oam_bfd_endpoint_tnl_mpls_t *) type;

    switch (ep->encap_type) {
        case TNL_MPLS_ENDPOINT_ENCAP_TYPE(MPLS_LSP):
            length += ep->num_labels * SHR_MPLS_HDR_LEN;
            if (ep->ip_encap_type == TNL_MPLS_ENDPOINT_IP_ENCAP_TYPE(IPV4)) {
                length += SHR_IPV4_HEADER_LENGTH;
            } else if (ep->ip_encap_type == TNL_MPLS_ENDPOINT_IP_ENCAP_TYPE(IPV6)) {
                length += SHR_IPV6_HEADER_LENGTH;
            }
            length += SHR_UDP_HDR_LEN;
            break;

        case TNL_MPLS_ENDPOINT_ENCAP_TYPE(MPLS_LSP_PHP):
            if (ep->ip_encap_type == TNL_MPLS_ENDPOINT_IP_ENCAP_TYPE(IPV4)) {
                length += SHR_IPV4_HEADER_LENGTH;
            } else if (ep->ip_encap_type == TNL_MPLS_ENDPOINT_IP_ENCAP_TYPE(IPV6)) {
                length += SHR_IPV6_HEADER_LENGTH;
            }
            length += SHR_UDP_HDR_LEN;
            break;

        case TNL_MPLS_ENDPOINT_ENCAP_TYPE(PW):
            length += ep->num_labels * SHR_MPLS_HDR_LEN;
            if (ep->pw_ach == true) {
                length += SHR_MPLS_ACH_LEN;
            }
            if (ep->ip_encap_type == TNL_MPLS_ENDPOINT_IP_ENCAP_TYPE(IPV4)) {
                length += SHR_IPV4_HEADER_LENGTH;
                length += SHR_UDP_HDR_LEN;
            } else if (ep->ip_encap_type == TNL_MPLS_ENDPOINT_IP_ENCAP_TYPE(IPV6)){
                length += SHR_IPV6_HEADER_LENGTH;
                length += SHR_UDP_HDR_LEN;
            }
            break;

        case TNL_MPLS_ENDPOINT_ENCAP_TYPE(MPLS_TP_CC):
            length += ep->num_labels * SHR_MPLS_HDR_LEN;
            length += SHR_MPLS_ACH_LEN;
            break;

        case TNL_MPLS_ENDPOINT_ENCAP_TYPE(MPLS_TP_CC_CV):
            length += ep->num_labels * SHR_MPLS_HDR_LEN;
            length += SHR_MPLS_ACH_LEN;
            length += ep->source_mep_id_len;
            break;

        default:
            /* This will never happen. */
            assert(0);
            length = 0;

    }

    return length;
}

static void
bfd_endpoint_tnl_mpls_encap_pack(void *type, uint16_t udp_src_port,
                                 uint16_t *length, uint8_t *encap,
                                 uint8_t **lkey, uint16_t *lkey_length,
                                 uint16_t bfd_pkt_length)
{
    bcmcth_oam_bfd_endpoint_tnl_mpls_t *ep;
    shr_util_pack_ipv4_header_t ipv4;
    shr_util_pack_ipv6_header_t ipv6;
    shr_util_pack_udp_header_t udp;
    shr_util_pack_ach_header_t ach;
    uint8_t label_stack[MAX_MPLS_LABELS * SHR_MPLS_HDR_LEN];
    int i;
    uint8_t *cur_ptr = encap, *label_ptr;

    ep = (bcmcth_oam_bfd_endpoint_tnl_mpls_t *) type;

    /* For MPLS endpoints, the lookup label is passed separately. */
    *lkey = NULL;
    *lkey_length = 0;


    /*
     * Form all the headers up-front. Only the required ones are actually
     * packed.
     */

    /* MPLS label stack. */
    label_ptr = label_stack;
    for (i = 0; i < ep->num_labels; i++) {
        MPLS_LABEL_PACK(label_ptr, ep->label_stack[i]);
    }

    /* IPv4. */
    sal_memset(&ipv4, 0, sizeof(ipv4));
    ipv4.version = SHR_IPV4_VERSION;
    ipv4.h_length = SHR_IPV4_HDR_WLEN;
    ipv4.dscp = ep->ipv4.tos;
    ipv4.ttl = tnl_mpls_ip_ttl_get(ep->encap_type);
    ipv4.protocol = SHR_IP_PROTO_UDP;
    ipv4.src = ep->ipv4.src_ip;
    ipv4.dst = ep->ipv4.dst_ip;
    cur_ptr = shr_util_pack_ipv4_header_pack(cur_ptr, &ipv4);

    /* IPv6. */
    sal_memset(&ipv6, 0, sizeof(ipv6));
    ipv6.version = SHR_IPV6_VERSION;
    ipv6.t_class = ep->ipv6.traffic_class;
    ipv6.f_label = 0;
    ipv6.next_header = SHR_IP_PROTO_UDP;
    ipv6.hop_limit = tnl_mpls_ip_ttl_get(ep->encap_type);
    sal_memcpy(ipv6.src, ep->ipv6.src_ip, sizeof(shr_ip6_t));
    sal_memcpy(ipv6.dst, ep->ipv6.dst_ip, sizeof(shr_ip6_t));

    /* UDP header. */
    sal_memset(&udp, 0, sizeof(udp));
    udp.src = udp_src_port;
    udp.dst = BFD_UDP_DST_PORT_SINGLE_HOP;
    udp.length = bfd_pkt_length + SHR_UDP_HDR_LEN;

    /* ACH */
    sal_memset(&ach, 0, sizeof(ach));
    ach.f_nibble = BFD_ACH_FIRST_NIBBLE;
    ach.version = BFD_ACH_VERSION;
    ach.channel_type = ach_channel_type_get(ep->encap_type, ep->ip_encap_type);

    /* Actually pack the encap. */
    switch (ep->encap_type) {
        case TNL_MPLS_ENDPOINT_ENCAP_TYPE(MPLS_LSP):
            sal_memcpy(cur_ptr, label_stack, ep->num_labels * SHR_MPLS_HDR_LEN);
            cur_ptr += ep->num_labels * SHR_MPLS_HDR_LEN;
            if (ep->ip_encap_type == TNL_MPLS_ENDPOINT_IP_ENCAP_TYPE(IPV4)) {
                cur_ptr = shr_util_pack_ipv4_header_pack(cur_ptr, &ipv4);
            } else if (ep->ip_encap_type == TNL_MPLS_ENDPOINT_IP_ENCAP_TYPE(IPV6)) {
                cur_ptr = shr_util_pack_ipv6_header_pack(cur_ptr, &ipv6);
            }
            cur_ptr = shr_util_pack_udp_header_pack(cur_ptr, &udp);
            break;

        case TNL_MPLS_ENDPOINT_ENCAP_TYPE(MPLS_LSP_PHP):
            if (ep->ip_encap_type == TNL_MPLS_ENDPOINT_IP_ENCAP_TYPE(IPV4)) {
                cur_ptr = shr_util_pack_ipv4_header_pack(cur_ptr, &ipv4);
            } else if (ep->ip_encap_type == TNL_MPLS_ENDPOINT_IP_ENCAP_TYPE(IPV6)) {
                cur_ptr = shr_util_pack_ipv6_header_pack(cur_ptr, &ipv6);
            }
            cur_ptr = shr_util_pack_udp_header_pack(cur_ptr, &udp);
            break;

        case TNL_MPLS_ENDPOINT_ENCAP_TYPE(PW):
            sal_memcpy(cur_ptr, label_stack, ep->num_labels * SHR_MPLS_HDR_LEN);
            cur_ptr += ep->num_labels * SHR_MPLS_HDR_LEN;
            if (ep->pw_ach == true) {
                cur_ptr = shr_util_pack_ach_header_pack(cur_ptr, &ach);
            }
            if (ep->ip_encap_type == TNL_MPLS_ENDPOINT_IP_ENCAP_TYPE(IPV4)) {
                cur_ptr = shr_util_pack_ipv4_header_pack(cur_ptr, &ipv4);
            } else if (ep->ip_encap_type == TNL_MPLS_ENDPOINT_IP_ENCAP_TYPE(IPV6)) {
                cur_ptr = shr_util_pack_ipv6_header_pack(cur_ptr, &ipv6);
            }
            cur_ptr = shr_util_pack_udp_header_pack(cur_ptr, &udp);
            break;

        case TNL_MPLS_ENDPOINT_ENCAP_TYPE(MPLS_TP_CC):
        case TNL_MPLS_ENDPOINT_ENCAP_TYPE(MPLS_TP_CC_CV):
            /* In case of CC_CV, pack the CC encap, EApp will modify the encap
             * for CV transmission when required.
             */
            sal_memcpy(cur_ptr, label_stack, ep->num_labels * SHR_MPLS_HDR_LEN);
            cur_ptr += ep->num_labels * SHR_MPLS_HDR_LEN;
            cur_ptr = shr_util_pack_udp_header_pack(cur_ptr, &udp);
            break;

        default:
            /* This will never happen. */
            assert(0);
            break;

    }

    *length = cur_ptr - encap;
}

static mcs_bfd_encap_types_t
bfd_endpoint_tnl_mpls_mcs_encap_type_get(void *type)
{
    bcmcth_oam_bfd_endpoint_tnl_mpls_t *ep;

    ep = (bcmcth_oam_bfd_endpoint_tnl_mpls_t *) type;

    if (ep->ip_encap_type == TNL_MPLS_ENDPOINT_IP_ENCAP_TYPE(IPV4)) {
        return MCS_BFD_ENCAP_TYPE_V4UDP;
    } else if (ep->ip_encap_type == TNL_MPLS_ENDPOINT_IP_ENCAP_TYPE(IPV6)) {
        return MCS_BFD_ENCAP_TYPE_V6UDP;
    } else {
        if (ep->encap_type == TNL_MPLS_ENDPOINT_ENCAP_TYPE(MPLS_TP_CC_CV)) {
            return MCS_BFD_ENCAP_TYPE_MPLS_TP_CC_CV;
        } else if (ep->encap_type == TNL_MPLS_ENDPOINT_ENCAP_TYPE(MPLS_TP_CC)) {
            return MCS_BFD_ENCAP_TYPE_MPLS_TP_CC;
        } else {
            return MCS_BFD_ENCAP_TYPE_RAW;
        }
    }
}

static void
bfd_endpoint_tnl_mpls_msg_convert(void *type, mcs_bfd_msg_ctrl_ep_set_t *msg)
{
    bcmcth_oam_bfd_endpoint_tnl_mpls_t *ep;

    ep = (bcmcth_oam_bfd_endpoint_tnl_mpls_t *) type;

    msg->mpls_label = ep->rx_lookup_label;

    switch (ep->encap_type) {
        case TNL_MPLS_ENDPOINT_ENCAP_TYPE(MPLS_TP_CC):
        case TNL_MPLS_ENDPOINT_ENCAP_TYPE(MPLS_TP_CC_CV):
            msg->source_mep_id_length = ep->source_mep_id_len;
            sal_memcpy(msg->source_mep_id,
                       ep->source_mep_id,
                       ep->source_mep_id_len);

            msg->remote_mep_id_length = ep->remote_mep_id_len;
            sal_memcpy(msg->remote_mep_id,
                       ep->remote_mep_id,
                       ep->remote_mep_id_len);
            break;

        case TNL_MPLS_ENDPOINT_ENCAP_TYPE(MPLS_LSP_PHP):
            msg->flags |= MCS_BFD_EP_SET_F_MPLS_PHP;
            break;

        default:
            /* Nothing to do. */
            break;
    }
}

static uint32_t
bfd_endpoint_tnl_mpls_update_flags_get(void *type1, void *type2)
{
    bcmcth_oam_bfd_endpoint_tnl_mpls_t *ep1, *ep2;
    uint32_t flags = 0;

    ep1 = (bcmcth_oam_bfd_endpoint_tnl_mpls_t *) type1;
    ep2 = (bcmcth_oam_bfd_endpoint_tnl_mpls_t *) type2;

    if (sal_memcmp(ep1, ep2, sizeof(bcmcth_oam_bfd_endpoint_tnl_mpls_t)) != 0) {
        flags |= MCS_BFD_EP_SET_UPDATE_F_ENCAP;
    }

    if (ep1->rx_lookup_label != ep2->rx_lookup_label) {
        flags |= MCS_BFD_EP_SET_UPDATE_F_ENCAP;
    }

    return flags;
}

typedef struct bfd_ep_type_info_s {
    /* Get the length of the L3/L4 encap. */
    uint16_t (*encap_len_get)(void *type);

    /* Pack the L3/L4 encap. */
    void (*encap_pack)(void *type, uint16_t udp_src_port,
                       uint16_t *length, uint8_t *encap,
                       uint8_t **lkey, uint16_t *lkey_length,
                       uint16_t bfd_pkt_length);

    /* Get the encap type passed to the MCS. */
    mcs_bfd_encap_types_t (*mcs_encap_type_get)(void *type);

    /* Endpoint type specific conversion to MCS message. */
    void (*msg_convert)(void *type, mcs_bfd_msg_ctrl_ep_set_t *msg);

    /*
     * Compare 2 endpoints and return flags as per
     * mcs_bfd_msg_ctrl_ep_set_t.update_flags.
     */
    uint32_t (*update_flags_get)(void *type1, void *type2);

    /* L2 ethertype used by this endpoint. */
    uint16_t l2_type;
} bfd_ep_type_info_t;

/*
 * Endpoint type specific information, this is indexed directly by
 * ENDPOINT_TYPE(TYPE).
 */
static bfd_ep_type_info_t g_ep_types_info[] = {
    {
        bfd_endpoint_ipv4_encap_len_get,
        bfd_endpoint_ipv4_encap_pack,
        bfd_endpoint_ipv4_mcs_encap_type_get,
        bfd_endpoint_ipv4_msg_convert,
        bfd_endpoint_ipv4_update_flags_get,
        SHR_L2_ETYPE_IPV4
    },
    {
        bfd_endpoint_ipv6_encap_len_get,
        bfd_endpoint_ipv6_encap_pack,
        bfd_endpoint_ipv6_mcs_encap_type_get,
        bfd_endpoint_ipv6_msg_convert,
        bfd_endpoint_ipv6_update_flags_get,
        SHR_L2_ETYPE_IPV6
    },
    {
        bfd_endpoint_tnl_ipv4_encap_len_get,
        bfd_endpoint_tnl_ipv4_encap_pack,
        bfd_endpoint_tnl_ipv4_mcs_encap_type_get,
        bfd_endpoint_tnl_ipv4_msg_convert,
        bfd_endpoint_tnl_ipv4_update_flags_get,
        SHR_L2_ETYPE_IPV4
    },
    {
        bfd_endpoint_tnl_ipv6_encap_len_get,
        bfd_endpoint_tnl_ipv6_encap_pack,
        bfd_endpoint_tnl_ipv6_mcs_encap_type_get,
        bfd_endpoint_tnl_ipv6_msg_convert,
        bfd_endpoint_tnl_ipv6_update_flags_get,
        SHR_L2_ETYPE_IPV6
    },
    {
        bfd_endpoint_tnl_mpls_encap_len_get,
        bfd_endpoint_tnl_mpls_encap_pack,
        bfd_endpoint_tnl_mpls_mcs_encap_type_get,
        bfd_endpoint_tnl_mpls_msg_convert,
        bfd_endpoint_tnl_mpls_update_flags_get,
        SHR_L2_ETYPE_MPLS_UNICAST
    }
};

/*******************************************************************************
 * Helper functions to the LT handler APIs.
 */

/*
 * Update the parameters in BFD_CONTROL that can be changed during run-time
 * (i.e. without a re-init). The fields modified by the user can be found in
 * fbmp.
 */
static int
bfd_control_run_time_update(int unit,
                            SHR_BITDCL* fbmp, bcmcth_oam_bfd_control_t *entry)
{
    mcs_bfd_msg_ctrl_global_param_set_t msg;
    bfd_info_t *bfd_info = g_bfd_info[unit];

    SHR_FUNC_ENTER(unit);

    if (bfd_info->ctrl->run == false) {
        SHR_EXIT();
    }

    if (SHR_BITGET(fbmp, OAM_BFD_CONTROLt_LOCAL_STATE_DOWN_EVENT_ON_ENDPOINT_CREATIONf)) {
        sal_memset(&msg, 0, sizeof(msg));
        msg.param = MCS_BFD_MSG_GLOBAL_PARAM_EP_STATE_DOWN_ON_CREATE;
        msg.value = entry->local_state_down_event_on_endpoint_creation == true ? 1 : 0;
        SHR_IF_ERR_VERBOSE_EXIT(bfd_uc_msg_global_param_set(unit, &msg));
    }

    if (SHR_BITGET(fbmp, OAM_BFD_CONTROLt_CONTROL_PLANE_INDEPENDENCEf)) {
        sal_memset(&msg, 0, sizeof(msg));
        msg.param = MCS_BFD_MSG_GLOBAL_PARAM_CONTROL_PLANE_INDEPENDENCE;
        msg.value = entry->control_plane_independence == true ? 1 : 0;
        SHR_IF_ERR_VERBOSE_EXIT(bfd_uc_msg_global_param_set(unit, &msg));
    }

    if (SHR_BITGET(fbmp, OAM_BFD_CONTROLt_TXf)) {
        sal_memset(&msg, 0, sizeof(msg));
        msg.param = MCS_BFD_MSG_GLOBAL_PARAM_TX;
        msg.value = entry->tx == true ? 1 : 0;
        SHR_IF_ERR_VERBOSE_EXIT(bfd_uc_msg_global_param_set(unit, &msg));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Set the oper fields depending on whether the app initialization was a success
 * or not.
 */
static void
bfd_control_oper_fields_set(bool success,
                            bcmcth_oam_bfd_control_t *entry,
                            bcmcth_oam_bfd_control_oper_t *oper)
{
    if (success == true) {
        oper->max_endpoints = entry->max_endpoints;
        oper->max_auth_sha1_keys = entry->max_auth_sha1_keys;
        oper->max_auth_sp_keys = entry->max_auth_sp_keys;
        oper->max_pkt_size = entry->max_pkt_size;
        oper->internal_local_discr = entry->internal_local_discr;
        oper->static_remote_discr = entry->static_remote_discr;
        oper->state = CONTROL_STATE(SUCCESS);
    } else {
        sal_memset(oper, 0, sizeof(bcmcth_oam_bfd_control_oper_t));
        oper->state = CONTROL_STATE(APP_NOT_INITIALIZED);
    }
}

static uint16_t
bfd_endpoint_pkt_size_get(bcmcth_oam_bfd_endpoint_t *entry)
{
    bfd_ep_type_info_t *ep_type_info = &(g_ep_types_info[entry->ep_type]);
    uint16_t size = 0;

    size += SHR_MAC_ADDR_LEN * 2;

    size += SHR_VLAN_HDR_LEN * entry->num_vlans;

    size += SHR_L2_ETYPE_LEN;

    size += ep_type_info->encap_len_get(&(entry->type));

    size += BFD_HDR_LEN;

    if (entry->auth_type == AUTH_TYPE(SIMPLE_PASSWORD)) {
        size += BFD_AUTH_SP_HDR_LEN(entry->sp.password_len);
    } else if ((entry->auth_type == AUTH_TYPE(KEYED_SHA1)) ||
               (entry->auth_type == AUTH_TYPE(METICULOUS_KEYED_SHA1))) {
        size += BFD_AUTH_SHA1_HDR_LEN;
    }

    size += SHR_L2_CRC_LEN;

    return size;
}

/* Run the validation and return the appropriate oper state. */
static bcmltd_common_oam_bfd_endpoint_state_t_t
bfd_endpoint_oper_state_get(int unit, bcmcth_oam_bfd_control_t *ctrl,
                            bcmcth_oam_bfd_endpoint_t *entry, bool active)
{
    bfd_info_t *bfd_info = g_bfd_info[unit];

    if (active == false) {
        /* If the endpoint is not yet created in the EApp, i.e. active = false,
         * then check if we can create one more endpoint.
         */
        if (bfd_info->ctrl->num_active_endpoints >=
            bfd_info->ctrl->max_active_endpoints) {
            return ENDPOINT_STATE(NUM_ENDPOINTS_EXCEED_MAX);
        }
    }

    if ((entry->auth_type == AUTH_TYPE(SIMPLE_PASSWORD)) &&
        (entry->auth_sp_found == false)) {
        return ENDPOINT_STATE(AUTH_NOT_EXISTS);
    }

    if ((entry->auth_type == AUTH_TYPE(KEYED_SHA1)) &&
        (entry->auth_sha1_found == false)) {
        return ENDPOINT_STATE(AUTH_NOT_EXISTS);
    }

    if ((entry->auth_type == AUTH_TYPE(METICULOUS_KEYED_SHA1)) &&
        (entry->auth_sha1_found == false)) {
        return ENDPOINT_STATE(AUTH_NOT_EXISTS);
    }

    if (ctrl->oper.max_pkt_size) {
        if (bfd_endpoint_pkt_size_get(entry) > ctrl->oper.max_pkt_size) {
            return ENDPOINT_STATE(PKT_SIZE_EXCEED_MAX);
        }
    }

    return ENDPOINT_STATE(SUCCESS);
}

static uint32_t
bfd_endpoint_update_flags_get(bcmcth_oam_bfd_endpoint_t *cur,
                              bcmcth_oam_bfd_endpoint_t *prev)
{
    bfd_ep_type_info_t *ep_type_info = &(g_ep_types_info[cur->ep_type]);
    uint32_t flags = 0;

    /* Get endpoint type specific update flags. */
    flags |= ep_type_info->update_flags_get(&(cur->type), &(prev->type));

    if (cur->mode != prev->mode) {
        flags |= MCS_BFD_EP_SET_UPDATE_F_DEMAND;
    }
    if (cur->echo != prev->echo) {
        flags |= MCS_BFD_EP_SET_UPDATE_F_ECHO;
    }
    if (cur->local_discr != prev->local_discr) {
        flags |= MCS_BFD_EP_SET_UPDATE_F_LOCAL_DISC;
    }
    if (cur->remote_discr != prev->remote_discr) {
        flags |= MCS_BFD_EP_SET_UPDATE_F_REMOTE_DISC;
    }
    if (cur->diag_code != prev->diag_code) {
        flags |= MCS_BFD_EP_SET_UPDATE_F_DIAG;
    }
    if (cur->min_tx_interval_usecs != prev->min_tx_interval_usecs) {
        flags |= MCS_BFD_EP_SET_UPDATE_F_MIN_TX;
    }
    if (cur->min_rx_interval_usecs != prev->min_rx_interval_usecs) {
        flags |= MCS_BFD_EP_SET_UPDATE_F_MIN_RX;
    }
    if (cur->min_echo_rx_interval_usecs != prev->min_echo_rx_interval_usecs) {
        flags |= MCS_BFD_EP_SET_UPDATE_F_MIN_ECHO_RX;
    }
    if (cur->auth_type != prev->auth_type) {
        flags |= MCS_BFD_EP_SET_UPDATE_F_AUTH;
    } else {
        if (cur->auth_type == AUTH_TYPE(SIMPLE_PASSWORD)) {
            if (cur->sp.id != prev->sp.id) {
                flags |= MCS_BFD_EP_SET_UPDATE_F_AUTH;
            }
        } else if ((cur->auth_type == AUTH_TYPE(KEYED_SHA1)) ||
                   (cur->auth_type == AUTH_TYPE(METICULOUS_KEYED_SHA1))) {
            if (cur->sha1.id != prev->sha1.id) {
                flags |= MCS_BFD_EP_SET_UPDATE_F_AUTH;
            }
        }
    }

    if (sal_memcmp(cur->dst_mac, prev->dst_mac, sizeof(shr_mac_t)) != 0) {
        flags |= MCS_BFD_EP_SET_UPDATE_F_ENCAP;
    }
    if (sal_memcmp(cur->src_mac, prev->src_mac, sizeof(shr_mac_t)) != 0) {
        flags |= MCS_BFD_EP_SET_UPDATE_F_ENCAP;
    }

    if (cur->num_vlans != prev->num_vlans) {
        flags |= MCS_BFD_EP_SET_UPDATE_F_ENCAP;
    } else {
        if (cur->num_vlans != 0) {
            if (sal_memcmp(&(cur->outer_vlan),
                           &(prev->outer_vlan),
                           sizeof(shr_util_pack_vlan_tag_t)) != 0) {
                flags |= MCS_BFD_EP_SET_UPDATE_F_ENCAP;
            }
        }
        if (cur->num_vlans == 2) {
            if (sal_memcmp(&(cur->inner_vlan),
                           &(prev->inner_vlan),
                           sizeof(shr_util_pack_vlan_tag_t)) != 0) {
                flags |= MCS_BFD_EP_SET_UPDATE_F_ENCAP;
            }
        }
    }

    if (cur->udp_src_port != prev->udp_src_port) {
        flags |= MCS_BFD_EP_SET_UPDATE_F_ENCAP;
    }

    return flags;
}

static int
bfd_endpoint_loopback_header_get(int unit,
        uint32_t *lbhdr,
        bcmcth_oam_bfd_endpoint_t *entry) {
    int system_source;
    bcmlrd_variant_t variant;
    int fid;
    bool generic_loopback_is_supported = false;

    SHR_FUNC_ENTER(unit);

    sal_memset(lbhdr, 0, BCMPKT_LBHDR_SIZE_BYTES);

    variant = bcmlrd_variant_get(unit);
    if (variant == BCMLRD_VARIANT_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    SHR_IF_ERR_EXIT(bcmpkt_generic_loopback_t_is_supported(variant,
                &generic_loopback_is_supported));

    if (!generic_loopback_is_supported) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (bcmcth_port_system_source_get(unit, entry->port,
                                       &system_source));

    SHR_IF_ERR_EXIT(bcmpkt_generic_loopback_t_field_id_get("RESERVED_0",
                &fid));
    SHR_IF_ERR_EXIT
        (bcmpkt_generic_loopback_t_field_set(variant, lbhdr, fid, 0x82));

    SHR_IF_ERR_EXIT(bcmpkt_generic_loopback_t_field_id_get("SOURCE_SYSTEM_PORT",
                &fid));
    SHR_IF_ERR_EXIT
        (bcmpkt_generic_loopback_t_field_set(variant, lbhdr, fid, system_source));

    SHR_IF_ERR_EXIT(bcmpkt_generic_loopback_t_field_id_get("FLAGS", &fid));
    SHR_IF_ERR_EXIT
        (bcmpkt_generic_loopback_t_field_set(variant, lbhdr, fid, 1));

exit:
    SHR_FUNC_EXIT();
}

static uint8_t *
bfd_endpoint_loopback_header_pack(uint8_t *buf, uint32_t *lbhdr)
{
    int idx = 0;

    for (idx = 0; idx < BCMPKT_LBHDR_SIZE_WORDS; idx++) {
         SHR_UTIL_PACK_U32(buf, lbhdr[idx]);
    }

    return buf;
}

static int
bfd_endpoint_encap_pack(int unit,
                        bcmcth_oam_bfd_endpoint_t *entry,
                        mcs_bfd_msg_ctrl_ep_set_t *msg)
{
    uint8_t *cur_ptr, *lkey;
    uint16_t lkey_length;
    uint16_t l3_l4_encap_length;
    uint16_t bfd_pkt_length = 0;
    shr_util_pack_l2_header_t l2;
    bfd_ep_type_info_t *ep_type_info = &(g_ep_types_info[entry->ep_type]);
    uint32_t lbhdr[BCMPKT_LBHDR_SIZE_WORDS];

    SHR_FUNC_ENTER(unit);

    cur_ptr = msg->encap_data;

    if (entry->tx_mode == BCMLTD_COMMON_OAM_BFD_ENDPOINT_TX_MODE_T_T_CPU_MASQUERADE) {
        SHR_IF_ERR_EXIT
            (bfd_endpoint_loopback_header_get(unit, lbhdr, entry));
         cur_ptr = bfd_endpoint_loopback_header_pack(cur_ptr, lbhdr);

    }

    /* L2 header */
    sal_memset(&l2, 0, sizeof(l2));
    sal_memcpy(l2.dst_mac, entry->dst_mac, sizeof(shr_mac_t));
    sal_memcpy(l2.src_mac, entry->src_mac, sizeof(shr_mac_t));

    if (entry->num_vlans != 0) {
        sal_memcpy(&l2.outer_vlan_tag, &entry->outer_vlan,
                   sizeof(l2.outer_vlan_tag));

        if (entry->num_vlans == 2) {
            sal_memcpy(&l2.inner_vlan_tag, &entry->inner_vlan,
                       sizeof(l2.inner_vlan_tag));
        }
    }
    l2.etype = ep_type_info->l2_type;
    cur_ptr = shr_util_pack_l2_header_pack(cur_ptr, &l2);

    bfd_pkt_length += BFD_HDR_LEN;
    if (entry->auth_type == AUTH_TYPE(SIMPLE_PASSWORD)) {
        bfd_pkt_length += BFD_AUTH_SP_HDR_LEN(entry->sp.password_len);
    } else if ((entry->auth_type == AUTH_TYPE(KEYED_SHA1)) ||
            (entry->auth_type == AUTH_TYPE(METICULOUS_KEYED_SHA1))) {
        bfd_pkt_length += BFD_AUTH_SHA1_HDR_LEN;
    }

    ep_type_info->encap_pack(&(entry->type), entry->udp_src_port,
                             &l3_l4_encap_length, cur_ptr,
                             &lkey, &lkey_length, bfd_pkt_length);
    cur_ptr += l3_l4_encap_length;

    msg->encap_length = cur_ptr - msg->encap_data;

    /* Populate the lookup key and offset in the encap. For MPLS endpoints, the
     * lookup key is sent seprately as msg->mpls_label.
     */
    msg->lkey_etype = ep_type_info->l2_type;
    if (lkey != NULL) {
        msg->lkey_offset = lkey - msg->encap_data;
        msg->lkey_length = lkey_length;
    }
exit:
    SHR_FUNC_EXIT();
}

/* Convert the endpoint IMM entry to the DMA message sent to the app. */
static int
bfd_endpoint_msg_convert(int unit,
                         bcmcth_oam_bfd_endpoint_t *cur,
                         bcmcth_oam_bfd_endpoint_t *prev,
                         bool create, uint32_t endpoint_id,
                         mcs_bfd_msg_ctrl_ep_set_t *msg)
{
    bfd_ep_type_info_t *ep_type_info = &(g_ep_types_info[cur->ep_type]);
    mcs_bfd_diag_code_t diag_code;

    SHR_FUNC_ENTER(unit);

    sal_memset(msg, 0, sizeof(*msg));

    msg->endpoint_id = endpoint_id;
    if (create == true) {
        msg->flags |= MCS_BFD_EP_SET_F_CREATE;
    } else {
        msg->update_flags |= bfd_endpoint_update_flags_get(prev, cur);
    }

    if (cur->tx_mode == BCMLTD_COMMON_OAM_BFD_ENDPOINT_TX_MODE_T_T_RAW_ETHERNET) {
        msg->flags |= MCS_BFD_EP_SET_F_TX_RAW_INGRESS;
    }
    if (cur->tx_mode == BCMLTD_COMMON_OAM_BFD_ENDPOINT_TX_MODE_T_T_CPU_MASQUERADE) {
        msg->flags |= MCS_BFD_EP_SET_F_TX_LB_HDR;
    }

    /* Convert endpoint type specific fields. */
    ep_type_info->msg_convert(&(cur->type), msg);

    msg->local_discr = cur->local_discr;
    msg->remote_discr = cur->remote_discr;
    msg->local_min_tx = cur->min_tx_interval_usecs;
    msg->local_min_rx = cur->min_rx_interval_usecs;
    msg->local_min_echo_rx = cur->min_echo_rx_interval_usecs;

    switch (cur->auth_type) {
        case AUTH_TYPE(SIMPLE_PASSWORD):
            msg->auth_type = MCS_BFD_AUTH_TYPE_SIMPLE_PASSWORD;
            msg->auth_key = cur->sp.id;
            break;

        case AUTH_TYPE(KEYED_SHA1):
            msg->auth_type = MCS_BFD_AUTH_TYPE_KEYED_SHA1;
            msg->auth_key = cur->sha1.id;
            break;

        case AUTH_TYPE(METICULOUS_KEYED_SHA1):
            msg->auth_type = MCS_BFD_AUTH_TYPE_METICULOUS_KEYED_SHA1;
            msg->auth_key = cur->sha1.id;
            break;

        case AUTH_TYPE(NONE):
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    msg->xmt_auth_seq = cur->initial_sha1_seq_num;
    msg->tx_port = cur->port;
    msg->tx_cos = cur->cos;
    msg->encap_type = ep_type_info->mcs_encap_type_get(cur);

    msg->local_echo = (cur->echo == true) ? 1 : 0;
    msg->passive = (cur->role == ENDPOINT_SESSION_INIT_ROLE(PASSIVE)) ? 1 : 0;
    msg->local_demand = (cur->mode == ENDPOINT_MODE(DEMAND)) ? 1 : 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (lt_to_mcs_diag_code_convert(cur->diag_code, &diag_code));
    msg->local_diag = diag_code;

    msg->local_detect_mult = cur->detect_multiplier;
    SHR_IF_ERR_EXIT
        (bfd_endpoint_encap_pack(unit, cur, msg));

exit:
    SHR_FUNC_EXIT();
}

static void
bfd_internal_ids_setup(int unit, bcmcth_oam_bfd_control_t *entry)
{
    bfd_info_t *bfd_info = g_bfd_info[unit];

    bfd_info->ctrl->max_active_endpoints = entry->max_endpoints;
    bfd_info->ctrl->max_active_auth_sp_keys = entry->max_auth_sp_keys;
    bfd_info->ctrl->max_active_auth_sha1_keys = entry->max_auth_sha1_keys;
}

static void
bfd_internal_ids_clear(int unit)
{
    bcmcth_oam_bfd_drv_t *bfd_drv = NULL;
    bfd_info_t *bfd_info = g_bfd_info[unit];

    bfd_drv = bcmcth_oam_bfd_drv_get(unit);
    assert(bfd_drv != NULL);

    SHR_BITCLR_RANGE(bfd_info->auth_sp_int_id_bmp,
                     0,
                     bfd_drv->max_auth_sp_keys);
    sal_memset(bfd_info->auth_sp, 0, sizeof(bcmcth_oam_bfd_auth_ha_t));
    bfd_info->ctrl->num_active_auth_sp_keys = 0;
    bfd_info->ctrl->max_active_auth_sp_keys = 0;

    SHR_BITCLR_RANGE(bfd_info->auth_sha1_int_id_bmp,
                     0,
                     bfd_drv->max_auth_sha1_keys);
    sal_memset(bfd_info->auth_sha1, 0, sizeof(bcmcth_oam_bfd_auth_ha_t));
    bfd_info->ctrl->num_active_auth_sha1_keys = 0;
    bfd_info->ctrl->max_active_auth_sha1_keys = 0;

    sal_memset(bfd_info->endpoints, 0, sizeof(bcmcth_oam_bfd_endpoint_ha_t));
    bfd_info->ctrl->num_active_endpoints = 0;
    bfd_info->ctrl->max_active_endpoints = 0;
}

static int
bfd_auth_sp_internal_id_alloc(int unit, uint32_t ext_id, uint32_t *int_id)
{
    bfd_info_t *bfd_info = g_bfd_info[unit];
    uint32_t idx;

    SHR_FUNC_ENTER(unit);

    if (bfd_info->ctrl->num_active_auth_sp_keys >=
        bfd_info->ctrl->max_active_auth_sp_keys) {
        /* This should have been validated before calling this function. */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    for (idx = 0; idx < bfd_info->ctrl->max_active_auth_sp_keys; idx++) {
        if (!SHR_BITGET(bfd_info->auth_sp_int_id_bmp, idx)) {
            SHR_BITSET(bfd_info->auth_sp_int_id_bmp, idx);
            *int_id = idx;

            bfd_info->ctrl->num_active_auth_sp_keys++;

            bfd_info->auth_sp[ext_id].int_id = *int_id;
            bfd_info->auth_sp[ext_id].active = true;
            SHR_EXIT();
        }
    }
    SHR_ERR_EXIT(SHR_E_INTERNAL);

exit:
    SHR_FUNC_EXIT();
}

static void
bfd_auth_sp_internal_id_free(int unit, uint32_t ext_id)
{
    bfd_info_t *bfd_info = g_bfd_info[unit];
    uint32_t int_id;

    int_id = bfd_info->auth_sp[ext_id].int_id;
    SHR_BITCLR(bfd_info->auth_sp_int_id_bmp, int_id);

    bfd_info->ctrl->num_active_auth_sp_keys--;
}

static int
bfd_auth_sha1_internal_id_alloc(int unit, uint32_t ext_id, uint32_t *int_id)
{
    bfd_info_t *bfd_info = g_bfd_info[unit];
    uint32_t idx;

    SHR_FUNC_ENTER(unit);

    if (bfd_info->ctrl->num_active_auth_sha1_keys >=
        bfd_info->ctrl->max_active_auth_sha1_keys) {
        /* This should have been validated before calling this function. */
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    for (idx = 0; idx < bfd_info->ctrl->max_active_auth_sha1_keys; idx++) {
        if (!SHR_BITGET(bfd_info->auth_sha1_int_id_bmp, idx)) {
            SHR_BITSET(bfd_info->auth_sha1_int_id_bmp, idx);
            *int_id = idx;

            bfd_info->ctrl->num_active_auth_sha1_keys++;

            bfd_info->auth_sha1[ext_id].int_id = *int_id;
            bfd_info->auth_sha1[ext_id].active = true;
            SHR_EXIT();
        }
    }
    SHR_ERR_EXIT(SHR_E_INTERNAL);

exit:
    SHR_FUNC_EXIT();
}

static void
bfd_auth_sha1_internal_id_free(int unit, uint32_t ext_id)
{
    bfd_info_t *bfd_info = g_bfd_info[unit];
    uint32_t int_id;

    int_id = bfd_info->auth_sha1[ext_id].int_id;
    SHR_BITCLR(bfd_info->auth_sha1_int_id_bmp, int_id);

    bfd_info->ctrl->num_active_auth_sha1_keys--;
}

static bcmcth_oam_bfd_endpoint_ha_t *
bfd_endpoint_info_get(int unit,
                      uint32_t ep_id)
{
    bcmcth_oam_bfd_drv_t *bfd_drv = NULL;
    bfd_info_t *bfd_info = g_bfd_info[unit];

    bfd_drv = bcmcth_oam_bfd_drv_get(unit);
    assert(bfd_drv != NULL);

    return (bfd_info->endpoints + ep_id);
}

static int
bfd_endpoint_misc_params_set(int unit,
                             bcmcth_oam_bfd_endpoint_t *cur,
                             bcmcth_oam_bfd_endpoint_t *prev,
                             uint32_t int_ep_id)
{
    SHR_FUNC_ENTER(unit);

    /* Start/Stop incrementing the sequence number if applicable. */
    if (cur->auth_type == AUTH_TYPE(KEYED_SHA1)) {
        if (prev == NULL) {
            /* Insert is happening. */
            if (cur->sha1_seq_num_incr == true) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bfd_uc_msg_endpoint_sha1_seq_num_incr(unit, int_ep_id));
            }
        } else {
            /* Update is happening. */
            if (prev->auth_type == AUTH_TYPE(KEYED_SHA1)) {
                if (prev->sha1_seq_num_incr == false &&
                    cur->sha1_seq_num_incr == true) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bfd_uc_msg_endpoint_sha1_seq_num_incr(unit, int_ep_id));
                }
            } else {
                if (cur->sha1_seq_num_incr == true) {
                    /* Auth type has changed to KEYED_SHA1, send the increment
                     * message if it is enabled since the default behaviour is
                     * to not increment.
                     */
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bfd_uc_msg_endpoint_sha1_seq_num_incr(unit, int_ep_id));
                }
            }
        }
    }

    if ((cur->mode == ENDPOINT_MODE(DEMAND)) && (cur->poll_sequence == true)) {
        if ((prev == NULL) || (prev->poll_sequence == false)) {
            /* Initiate poll sequence. */
            SHR_IF_ERR_VERBOSE_EXIT(bfd_uc_msg_endpoint_poll(unit, int_ep_id));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bfd_event_thread_start(int unit, bool warm)
{
    bfd_info_t *bfd_info = g_bfd_info[unit];

    SHR_FUNC_ENTER(unit);

    /* Spawn the event thread. */
    bfd_info->event_thread_ctrl = shr_thread_start("bcmcthOamBfdEvent",
                                                   SAL_THREAD_PRIO_DEFAULT,
                                                   bfd_event_thread,
                                                   BFD_ENCODE_UNIT_WARM(unit, warm));
    if (bfd_info->event_thread_ctrl == NULL) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bfd_event_thread_stop(int unit)
{
    bfd_info_t *bfd_info = g_bfd_info[unit];

    SHR_FUNC_ENTER(unit);

    if (bfd_info->event_thread_ctrl == NULL) {
        /* Thread is not running. */
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmbd_mcs_msg_receive_cancel(unit, bfd_info->ctrl->uc_num,
                    MCS_MSG_CLASS_BFD_EVENT));

    SHR_IF_ERR_EXIT
        (shr_thread_stop(bfd_info->event_thread_ctrl,
                         BFD_EVENT_THREAD_STOP_TIMEOUT_USECS));

    bfd_info->event_thread_ctrl = NULL;

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * BFD LT handler APIs
 */
int
bcmcth_oam_bfd_control_insert(int unit,
                              bcmcth_oam_bfd_control_t *entry,
                              bcmcth_oam_bfd_control_oper_t *oper)
{
    bfd_info_t *bfd_info = g_bfd_info[unit];

    SHR_FUNC_ENTER(unit);

    if (entry->bfd == true) {
        /* Initialize the app. */
        SHR_IF_ERR_VERBOSE_EXIT(bfd_uc_msg_init(unit, entry));
    }

    /* Set the oper fields. */
    bfd_control_oper_fields_set(bfd_info->ctrl->run, entry, oper);

    if (bfd_info->ctrl->run == true) {
        /* Send the remaining parameters to the app. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bfd_control_run_time_update(unit, entry->fbmp, entry));

        /* Setup the internal Ids. */
        bfd_internal_ids_setup(unit, entry);

        /* Spawn the event thread. */
        SHR_IF_ERR_VERBOSE_EXIT(bfd_event_thread_start(unit, false));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_oam_bfd_control_update(int unit,
                              bcmcth_oam_bfd_control_t *cur,
                              bcmcth_oam_bfd_control_t *prev,
                              bcmcth_oam_bfd_control_oper_t *oper)
{
    bfd_info_t *bfd_info = g_bfd_info[unit];
    SHR_BITDCLNAME(fbmp, OAM_BFD_CONTROLt_FIELD_COUNT);

    SHR_FUNC_ENTER(unit);

    /* Copy over the current oper fields, these may change if the app's init
     * status changes.
     */
    sal_memcpy(oper, &(cur->oper), sizeof(bcmcth_oam_bfd_control_oper_t));

    if (cur->bfd != prev->bfd) {
        /*
         * The app's init status is changing, check if it is init or shutdown.
         */
        if (cur->bfd == true) {
            /* Initialize the app. */
            SHR_IF_ERR_VERBOSE_EXIT(bfd_uc_msg_init(unit, cur));

            if (bfd_info->ctrl->run == true) {
                /* Send the remaining parameters to the app. Combine the fbmp of
                 * current and previous as there could have been fields set
                 * previously prior to app initialization.
                 */
                SHR_BITOR_RANGE(prev->fbmp, cur->fbmp,
                                0, OAM_BFD_CONTROLt_FIELD_COUNT, fbmp);
                SHR_IF_ERR_VERBOSE_EXIT
                    (bfd_control_run_time_update(unit, fbmp, cur));

                /* Set the oper fields if the init operation was a success. */
                bfd_control_oper_fields_set(bfd_info->ctrl->run, cur, oper);

                /* Setup the internal Ids. */
                bfd_internal_ids_setup(unit, cur);

                /* Spawn the event thread. */
                SHR_IF_ERR_VERBOSE_EXIT(bfd_event_thread_start(unit, false));
            }
        } else {
            /* Shutdown requested. */
            SHR_IF_ERR_VERBOSE_EXIT(bfd_uc_msg_shutdown(unit));

            /* Clear the oper-fields as the app has shutdown. */
            bfd_control_oper_fields_set(false, NULL, oper);

            /* Clear all the allocated internal Ids. */
            bfd_internal_ids_clear(unit);
        }

    } else {
        /* Update run-time parameters if any have changed. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bfd_control_run_time_update(unit, cur->fbmp, cur));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_oam_bfd_control_delete(int unit)
{
    bfd_info_t *bfd_info = g_bfd_info[unit];

    SHR_FUNC_ENTER(unit);

    if (bfd_info->ctrl->init == false) {
        SHR_EXIT();
    }

    /* Stop the event thread. */
    SHR_IF_ERR_VERBOSE_EXIT(bfd_event_thread_stop(unit));

    /* Shutdown the app */
    SHR_IF_ERR_VERBOSE_EXIT(bfd_uc_msg_shutdown(unit));

    /* Clear all the allocated internal Ids. */
    bfd_internal_ids_clear(unit);

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_oam_bfd_stats_update(int unit,
                            bcmcth_oam_bfd_stats_t *entry)
{
    bfd_info_t *bfd_info = g_bfd_info[unit];

    SHR_FUNC_ENTER(unit);

    if (bfd_info->ctrl->run == false) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT(bfd_uc_msg_global_stats_set(unit, entry));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_oam_bfd_stats_lookup(int unit,
                            bcmcth_oam_bfd_stats_t *entry)
{
    bfd_info_t *bfd_info = g_bfd_info[unit];

    SHR_FUNC_ENTER(unit);

    if (bfd_info->ctrl->run == false) {
        sal_memset(entry, 0, sizeof(*entry));
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT(bfd_uc_msg_global_stats_get(unit, entry));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_oam_bfd_auth_sp_insert(int unit,
                              bcmcth_oam_bfd_auth_sp_t *entry,
                              bcmltd_common_oam_bfd_auth_state_t_t *oper_state)
{
    bfd_info_t *bfd_info = g_bfd_info[unit];
    uint32_t int_id;

    SHR_FUNC_ENTER(unit);

    bfd_info->auth_sp[entry->id].in_use = true;

    if (bfd_info->ctrl->num_active_auth_sp_keys >=
        bfd_info->ctrl->max_active_auth_sp_keys) {
        *oper_state = AUTH_STATE(NUM_AUTH_IDS_EXCEED_MAX);
        SHR_EXIT();
    }
    *oper_state = AUTH_STATE(SUCCESS);

    SHR_IF_ERR_VERBOSE_EXIT
        (bfd_auth_sp_internal_id_alloc(unit, entry->id, &int_id));


    SHR_IF_ERR_VERBOSE_EXIT(bfd_uc_msg_auth_sp_set(unit, int_id, entry));

 exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_oam_bfd_auth_sp_update(int unit,
                              bcmcth_oam_bfd_auth_sp_t *entry,
                              bcmltd_common_oam_bfd_auth_state_t_t *oper_state)
{
    bfd_info_t *bfd_info = g_bfd_info[unit];
    uint32_t int_id;

    SHR_FUNC_ENTER(unit);

    /* There is no oper-state for this LT that can change on an UPDATE. */
    *oper_state = entry->oper_state;

    if (bfd_info->auth_sp[entry->id].active == false) {
        SHR_EXIT();
    }

    int_id = bfd_info->auth_sp[entry->id].int_id;
    SHR_IF_ERR_VERBOSE_EXIT(bfd_uc_msg_auth_sp_set(unit, int_id, entry));

 exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_oam_bfd_auth_sp_delete(int unit, uint32_t sp_id)
{
    bfd_info_t *bfd_info = g_bfd_info[unit];
    uint32_t int_id;

    SHR_FUNC_ENTER(unit);

    bfd_info->auth_sp[sp_id].in_use = false;

    if (bfd_info->auth_sp[sp_id].active == false) {
        SHR_EXIT();
    }

    int_id = bfd_info->auth_sp[sp_id].int_id;
    SHR_IF_ERR_VERBOSE_EXIT(bfd_uc_msg_auth_sp_set(unit, int_id, NULL));

    bfd_auth_sp_internal_id_free(unit, sp_id);

 exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_oam_bfd_auth_sha1_insert(int unit,
                                bcmcth_oam_bfd_auth_sha1_t *entry,
                                bcmltd_common_oam_bfd_auth_state_t_t *oper_state)
{
    bfd_info_t *bfd_info = g_bfd_info[unit];
    uint32_t int_id;

    SHR_FUNC_ENTER(unit);

    bfd_info->auth_sha1[entry->id].in_use = true;

    if (bfd_info->ctrl->num_active_auth_sha1_keys >=
        bfd_info->ctrl->max_active_auth_sha1_keys) {
        *oper_state = AUTH_STATE(NUM_AUTH_IDS_EXCEED_MAX);
        SHR_EXIT();
    }
    *oper_state = AUTH_STATE(SUCCESS);

    SHR_IF_ERR_VERBOSE_EXIT
        (bfd_auth_sha1_internal_id_alloc(unit, entry->id, &int_id));

    SHR_IF_ERR_VERBOSE_EXIT(bfd_uc_msg_auth_sha1_set(unit, int_id, entry));

 exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_oam_bfd_auth_sha1_update(int unit,
                                bcmcth_oam_bfd_auth_sha1_t *entry,
                                bcmltd_common_oam_bfd_auth_state_t_t *oper_state)
{
    bfd_info_t *bfd_info = g_bfd_info[unit];
    uint32_t int_id;

    SHR_FUNC_ENTER(unit);

    /* There is no oper-state for this LT that can change on an UPDATE. */
    *oper_state = entry->oper_state;

    if (bfd_info->auth_sha1[entry->id].active == false) {
        SHR_EXIT();
    }

    int_id = bfd_info->auth_sha1[entry->id].int_id;
    SHR_IF_ERR_VERBOSE_EXIT(bfd_uc_msg_auth_sha1_set(unit, int_id, entry));

 exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_oam_bfd_auth_sha1_delete(int unit, uint32_t sha1_id)
{
    bfd_info_t *bfd_info = g_bfd_info[unit];
    uint32_t int_id;

    SHR_FUNC_ENTER(unit);

    bfd_info->auth_sha1[sha1_id].in_use = false;

    if (bfd_info->auth_sha1[sha1_id].active == false) {
        SHR_EXIT();
    }

    int_id = bfd_info->auth_sha1[sha1_id].int_id;
    SHR_IF_ERR_VERBOSE_EXIT(bfd_uc_msg_auth_sha1_set(unit, int_id, NULL));

    bfd_auth_sha1_internal_id_free(unit, sha1_id);

 exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_oam_bfd_endpoint_insert(int unit,
                               bcmcth_oam_bfd_control_t *ctrl,
                               bcmcth_oam_bfd_endpoint_t *entry,
                               bcmltd_common_oam_bfd_endpoint_state_t_t *oper_state)
{
    bcmcth_oam_bfd_endpoint_ha_t *ep_info;
    mcs_bfd_msg_ctrl_ep_set_t msg;

    SHR_FUNC_ENTER(unit);

    /* Set the in_use flag of the endpoint to true. */
    ep_info = bfd_endpoint_info_get(unit, entry->endpoint_id);
    ep_info->in_use = true;
    ep_info->ep_type = entry->ep_type;

    /* Check if the endpoint can be created in the app, else return by setting
     * oper state.
     */
    *oper_state = bfd_endpoint_oper_state_get(unit, ctrl, entry, false);
    if (*oper_state != ENDPOINT_STATE(SUCCESS)) {
        SHR_EXIT();
    }

    /* Convert the entry to app message. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bfd_endpoint_msg_convert(unit, entry, NULL, true, entry->endpoint_id, &msg));

    /* Send the endpoint set message to the app. */
    SHR_IF_ERR_VERBOSE_EXIT(bfd_uc_msg_endpoint_set(unit, &msg));

    ep_info->active = true;

    /* Update the rest of the params. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bfd_endpoint_misc_params_set(unit, entry, NULL, entry->endpoint_id));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_oam_bfd_endpoint_update(int unit,
                               bcmcth_oam_bfd_control_t *ctrl,
                               bcmcth_oam_bfd_endpoint_t *cur,
                               bcmcth_oam_bfd_endpoint_t *prev,
                               bcmltd_common_oam_bfd_endpoint_state_t_t *oper_state)
{
    bcmcth_oam_bfd_endpoint_ha_t *ep_info;
    mcs_bfd_msg_ctrl_ep_set_t msg;
    bool create;

    SHR_FUNC_ENTER(unit);

    ep_info = bfd_endpoint_info_get(unit, cur->endpoint_id);

    /* Get the new oper-state */
    *oper_state = bfd_endpoint_oper_state_get(unit, ctrl, cur, ep_info->active);
    if (*oper_state != ENDPOINT_STATE(SUCCESS)) {
        if (ep_info->active == true) {
            /*
             * Endpoint is active in the app, delete it since the oper state has
             * transitioned away from SUCCESS.
             */
            SHR_IF_ERR_VERBOSE_EXIT
                (bfd_uc_msg_endpoint_delete(unit, cur->endpoint_id));
        }
        SHR_EXIT();
    }

    /* Check if the endpoint needs to be created in the app. */
    if (ep_info->active == true) {
        create = false;
    } else {
        create = true;
    }

    /* Convert to app message. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bfd_endpoint_msg_convert(unit, cur, prev, create,
                                  cur->endpoint_id, &msg));

    /* Send the endpoint set message to the app. */
    SHR_IF_ERR_VERBOSE_EXIT(bfd_uc_msg_endpoint_set(unit, &msg));

    /* Update the rest of the params. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bfd_endpoint_misc_params_set(unit, cur, prev, cur->endpoint_id));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_oam_bfd_endpoint_delete(int unit,
                               bcmltd_common_oam_bfd_endpoint_type_t_t ep_type,
                               uint32_t endpoint_id)
{
    bcmcth_oam_bfd_endpoint_ha_t *ep_info;

    SHR_FUNC_ENTER(unit);

    ep_info = bfd_endpoint_info_get(unit, endpoint_id);
    ep_info->in_use = false;

    if (ep_info->active == false) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bfd_uc_msg_endpoint_delete(unit, endpoint_id));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_oam_bfd_endpoint_status_lookup(int unit,
                                      bcmltd_common_oam_bfd_endpoint_type_t_t ep_type,
                                      uint32_t endpoint_id,
                                      bcmcth_oam_bfd_endpoint_status_t *entry)
{
    bcmcth_oam_bfd_endpoint_ha_t *ep_info;

    SHR_FUNC_ENTER(unit);

    sal_memset(entry, 0, sizeof(*entry));

    ep_info = bfd_endpoint_info_get(unit, endpoint_id);

    if (ep_info->in_use == false) {
        /* Endpoint is not yet created. */
        entry->endpoint_state = ENDPOINT_ID_STATE(ENDPOINT_ID_NOT_FOUND);
        SHR_EXIT();
    } else if (ep_info->active == false) {
        /* Endpoint is not yet active in the app. */
        entry->endpoint_state = ENDPOINT_ID_STATE(ENDPOINT_ID_INACTIVE);
        SHR_EXIT();
    } else {
        /* Endpoint is active in the app. */
        entry->endpoint_state = ENDPOINT_ID_STATE(ENDPOINT_ID_ACTIVE);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bfd_uc_msg_endpoint_status_get(unit, endpoint_id, entry));

exit:
    SHR_FUNC_EXIT();
}

extern int
bcmcth_oam_bfd_endpoint_stats_update(int unit,
                                     bcmltd_common_oam_bfd_endpoint_type_t_t ep_type,
                                     uint32_t endpoint_id,
                                     bcmcth_oam_bfd_endpoint_stats_t *entry)
{
    bcmcth_oam_bfd_endpoint_ha_t *ep_info;

    SHR_FUNC_ENTER(unit);

    ep_info = bfd_endpoint_info_get(unit, endpoint_id);

    if (ep_info->active == false) {
        /* Endpoint is not yet active in the app. */
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bfd_endpoint_uc_stats_set(unit, endpoint_id, entry));

exit:
    SHR_FUNC_EXIT();

}

int
bcmcth_oam_bfd_endpoint_stats_lookup(int unit,
                                     bcmltd_common_oam_bfd_endpoint_type_t_t ep_type,
                                     uint32_t endpoint_id,
                                     bcmcth_oam_bfd_endpoint_stats_t *entry)
{
    bcmcth_oam_bfd_endpoint_ha_t *ep_info;

    SHR_FUNC_ENTER(unit);

    ep_info = bfd_endpoint_info_get(unit, endpoint_id);

    if (ep_info->active == false) {
        /* Endpoint is not yet active in the app. */
        sal_memset(entry, 0, sizeof(*entry));
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bfd_uc_msg_endpoint_stats_get(unit, endpoint_id, entry));

exit:
    SHR_FUNC_EXIT();

}

int
bcmcth_oam_bfd_event_control_update(int unit,
                                    bcmcth_oam_bfd_event_control_t *entry)
{
    bfd_info_t *bfd_info = g_bfd_info[unit];

    SHR_FUNC_ENTER(unit);

    if (bfd_info->ctrl->run == false) {
        /* App is not running, return 0. */
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT(bfd_uc_msg_event_mask_set(unit, entry));

exit:
    SHR_FUNC_EXIT();

}
