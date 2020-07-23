/*! \file bcmcth_oam_bfd_endpoint.c
 *
 * BCMCTH OAM BFD APIs for endpoint LTs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <sal/sal_libc.h>

#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmcth/bcmcth_util.h>
#include <bcmcth/bcmcth_oam_imm.h>
#include <bcmcth/bcmcth_oam_drv.h>
#include <bcmcth/bcmcth_oam_bfd.h>
#include <bcmcth/bcmcth_oam_bfd_uc.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_OAM

/*******************************************************************************
 * BFD IPv4 endpoint functions.
 */
static int
bfd_endpoint_ipv4_field_get(int unit, const bcmltd_field_t *field,
                            bcmcth_oam_bfd_endpoint_t *entry)
{
    bcmcth_oam_bfd_drv_t *bfd_drv = NULL;
    uint32_t fid;
    uint32_t fval32[2];
    uint64_t fval;
    int rv;

    SHR_FUNC_ENTER(unit);

    bfd_drv = bcmcth_oam_bfd_drv_get(unit);
    SHR_NULL_CHECK(bfd_drv, SHR_E_INIT);

    fid = field->id;
    fval = field->data;

    switch (fid) {
        case OAM_BFD_IPV4_ENDPOINTt_ROLEf:
            entry->role = fval;
            break;

        case OAM_BFD_IPV4_ENDPOINTt_TYPEf:
            entry->type.ipv4.ip_ep_type = fval;
            break;

        case OAM_BFD_IPV4_ENDPOINTt_MODEf:
            entry->mode = fval;
            break;

        case OAM_BFD_IPV4_ENDPOINTt_ECHOf:
            entry->echo = fval;
            break;

        case OAM_BFD_IPV4_ENDPOINTt_LOCAL_DISCRIMINATORf:
            entry->local_discr = fval;
            break;

        case OAM_BFD_IPV4_ENDPOINTt_REMOTE_DISCRIMINATORf:
            entry->remote_discr = fval;
            break;

        case OAM_BFD_IPV4_ENDPOINTt_DIAG_CODEf:
            entry->diag_code = fval;
            break;

        case OAM_BFD_IPV4_ENDPOINTt_MIN_TX_INTERVAL_USECSf:
            entry->min_tx_interval_usecs = fval;
            break;

        case OAM_BFD_IPV4_ENDPOINTt_MIN_RX_INTERVAL_USECSf:
            entry->min_rx_interval_usecs = fval;
            break;

        case OAM_BFD_IPV4_ENDPOINTt_MIN_ECHO_RX_INTERVAL_USECSf:
            entry->min_echo_rx_interval_usecs = fval;
            break;

        case OAM_BFD_IPV4_ENDPOINTt_DETECT_MULTIPLIERf:
            entry->detect_multiplier = fval;
            break;

        case OAM_BFD_IPV4_ENDPOINTt_AUTH_TYPEf:
            entry->auth_type = fval;
            break;

        case OAM_BFD_IPV4_ENDPOINTt_OAM_BFD_AUTH_SIMPLE_PASSWORD_IDf:
            entry->sp.id = fval;
            rv = bcmcth_oam_bfd_auth_sp_entry_get(unit, entry->sp.id,
                                                  &(entry->sp));
            if (rv == SHR_E_NONE) {
                entry->auth_sp_found = true;
            }
            break;

        case OAM_BFD_IPV4_ENDPOINTt_OAM_BFD_AUTH_SHA1_IDf:
            entry->sha1.id = fval;
            rv = bcmcth_oam_bfd_auth_sha1_entry_get(unit, entry->sha1.id,
                                                    &(entry->sha1));
            if (rv == SHR_E_NONE) {
                entry->auth_sha1_found = true;
            }
            break;

        case OAM_BFD_IPV4_ENDPOINTt_INITIAL_SHA1_SEQ_NUMf:
            entry->initial_sha1_seq_num = fval;
            break;

        case OAM_BFD_IPV4_ENDPOINTt_SHA1_SEQ_NUM_INCREMENTf:
            entry->sha1_seq_num_incr = fval;
            break;

        case OAM_BFD_IPV4_ENDPOINTt_POLL_SEQUENCEf:
            entry->poll_sequence = fval;
            break;

        case OAM_BFD_IPV4_ENDPOINTt_RX_LOOKUP_VLAN_IDf:
            entry->type.ipv4.rx_lookup_vlan_id = fval;
            break;

        case OAM_BFD_IPV4_ENDPOINTt_PORT_IDf:
            entry->port = fval;
            break;

        case OAM_BFD_IPV4_ENDPOINTt_COSf:
            entry->cos = fval;
            break;

        case OAM_BFD_IPV4_ENDPOINTt_DST_MACf:
            fval32[0] = (uint32_t)fval;
            fval32[1] = (uint32_t)(fval >> 32);
            BCMCTH_MAC_ADDR_FROM_UINT32(entry->dst_mac, fval32);
            break;

        case OAM_BFD_IPV4_ENDPOINTt_SRC_MACf:
            fval32[0] = (uint32_t)fval;
            fval32[1] = (uint32_t)(fval >> 32);
            BCMCTH_MAC_ADDR_FROM_UINT32(entry->src_mac, fval32);
            break;

        case OAM_BFD_IPV4_ENDPOINTt_TAG_TYPEf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bfd_drv->num_vlans_get(fval, &(entry->num_vlans)));
            break;

        case OAM_BFD_IPV4_ENDPOINTt_VLAN_IDf:
            entry->outer_vlan.tci.vid = fval;
            break;

        case OAM_BFD_IPV4_ENDPOINTt_VLAN_PRIf:
            entry->outer_vlan.tci.prio = fval;
            break;

        case OAM_BFD_IPV4_ENDPOINTt_TPIDf:
            entry->outer_vlan.tpid = fval;
            break;

        case OAM_BFD_IPV4_ENDPOINTt_INNER_VLAN_IDf:
            entry->inner_vlan.tci.vid = fval;
            break;

        case OAM_BFD_IPV4_ENDPOINTt_INNER_VLAN_PRIf:
            entry->inner_vlan.tci.prio = fval;
            break;

        case OAM_BFD_IPV4_ENDPOINTt_INNER_TPIDf:
            entry->inner_vlan.tpid = fval;
            break;

        case OAM_BFD_IPV4_ENDPOINTt_SRC_IPV4f:
            entry->type.ipv4.src_ip = fval;
            break;

        case OAM_BFD_IPV4_ENDPOINTt_DST_IPV4f:
            entry->type.ipv4.dst_ip = fval;
            break;

        case OAM_BFD_IPV4_ENDPOINTt_TOSf:
            entry->type.ipv4.tos = fval;
            break;

        case OAM_BFD_IPV4_ENDPOINTt_TTLf:
            entry->type.ipv4.ttl = fval;
            break;

        case OAM_BFD_IPV4_ENDPOINTt_UDP_SRC_PORTf:
            entry->udp_src_port = fval;
            break;

        case OAM_BFD_IPV4_ENDPOINTt_TX_MODEf:
            entry->tx_mode = fval;
            break;

        case OAM_BFD_IPV4_ENDPOINTt_OPERATIONAL_STATEf:
            entry->oper_state = fval;
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bfd_endpoint_ipv4_defaults_get(int unit, bcmcth_oam_bfd_endpoint_t *entry)
{
    bcmcth_oam_bfd_drv_t *bfd_drv = NULL;
    bcmlrd_client_field_info_t *fields_info = NULL;
    size_t fld_num;
    size_t actual_fld_num;
    size_t idx;

    SHR_FUNC_ENTER(unit);

    bfd_drv = bcmcth_oam_bfd_drv_get(unit);
    SHR_NULL_CHECK(bfd_drv, SHR_E_INIT);

    sal_memset(entry, 0, sizeof(*entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_oam_bfd_num_data_fields_get(unit, OAM_BFD_IPV4_ENDPOINTt,
                                            &fld_num));
    SHR_ALLOC(fields_info, sizeof(bcmlrd_client_field_info_t) * fld_num,
              "bcmcthOamBfdEndpointIpv4FldInfo");
    SHR_NULL_CHECK(fields_info, SHR_E_MEMORY);

    SHR_IF_ERR_EXIT
        (bcmlrd_table_fields_def_get(unit, "OAM_BFD_IPV4_ENDPOINT",
                                     fld_num, fields_info, &actual_fld_num));

    for (idx = 0; idx < actual_fld_num; idx++) {
        switch (fields_info[idx].id) {
            case OAM_BFD_IPV4_ENDPOINTt_ROLEf:
                entry->role = fields_info[idx].def.u8;
                break;

            case OAM_BFD_IPV4_ENDPOINTt_TYPEf:
                entry->type.ipv4.ip_ep_type = fields_info[idx].def.u8;
                break;

            case OAM_BFD_IPV4_ENDPOINTt_MODEf:
                entry->mode = fields_info[idx].def.u8;
                break;

            case OAM_BFD_IPV4_ENDPOINTt_ECHOf:
                entry->echo = fields_info[idx].def.is_true;
                break;

            case OAM_BFD_IPV4_ENDPOINTt_DIAG_CODEf:
                entry->diag_code = fields_info[idx].def.u8;
                break;

            case OAM_BFD_IPV4_ENDPOINTt_MIN_TX_INTERVAL_USECSf:
                entry->min_tx_interval_usecs = fields_info[idx].def.u32;
                break;

            case OAM_BFD_IPV4_ENDPOINTt_MIN_RX_INTERVAL_USECSf:
                entry->min_rx_interval_usecs = fields_info[idx].def.u32;
                break;

            case OAM_BFD_IPV4_ENDPOINTt_MIN_ECHO_RX_INTERVAL_USECSf:
                entry->min_echo_rx_interval_usecs = fields_info[idx].def.u32;
                break;

            case OAM_BFD_IPV4_ENDPOINTt_DETECT_MULTIPLIERf:
                entry->detect_multiplier = fields_info[idx].def.u8;
                break;

            case OAM_BFD_IPV4_ENDPOINTt_AUTH_TYPEf:
                entry->auth_type = fields_info[idx].def.u8;
                break;

            case OAM_BFD_IPV4_ENDPOINTt_INITIAL_SHA1_SEQ_NUMf:
                entry->initial_sha1_seq_num = fields_info[idx].def.u32;
                break;

            case OAM_BFD_IPV4_ENDPOINTt_SHA1_SEQ_NUM_INCREMENTf:
                entry->sha1_seq_num_incr = fields_info[idx].def.u8;
                break;

            case OAM_BFD_IPV4_ENDPOINTt_POLL_SEQUENCEf:
                entry->poll_sequence = fields_info[idx].def.u8;
                break;

            case OAM_BFD_IPV4_ENDPOINTt_COSf:
                entry->cos = fields_info[idx].def.u8;
                break;

            case OAM_BFD_IPV4_ENDPOINTt_TAG_TYPEf:
                SHR_IF_ERR_VERBOSE_EXIT
                    (bfd_drv->num_vlans_get(fields_info[idx].def.u8,
                                            &(entry->num_vlans)));
                break;

            case OAM_BFD_IPV4_ENDPOINTt_UDP_SRC_PORTf:
                entry->udp_src_port = fields_info[idx].def.u16;
                break;

            case OAM_BFD_IPV4_ENDPOINTt_OAM_BFD_IPV4_ENDPOINT_IDf:
            case OAM_BFD_IPV4_ENDPOINTt_LOCAL_DISCRIMINATORf:
            case OAM_BFD_IPV4_ENDPOINTt_REMOTE_DISCRIMINATORf:
            case OAM_BFD_IPV4_ENDPOINTt_OAM_BFD_AUTH_SIMPLE_PASSWORD_IDf:
            case OAM_BFD_IPV4_ENDPOINTt_OAM_BFD_AUTH_SHA1_IDf:
            case OAM_BFD_IPV4_ENDPOINTt_RX_LOOKUP_VLAN_IDf:
            case OAM_BFD_IPV4_ENDPOINTt_PORT_IDf:
            case OAM_BFD_IPV4_ENDPOINTt_DST_MACf:
            case OAM_BFD_IPV4_ENDPOINTt_SRC_MACf:
            case OAM_BFD_IPV4_ENDPOINTt_VLAN_IDf:
            case OAM_BFD_IPV4_ENDPOINTt_VLAN_PRIf:
            case OAM_BFD_IPV4_ENDPOINTt_TPIDf:
            case OAM_BFD_IPV4_ENDPOINTt_INNER_VLAN_IDf:
            case OAM_BFD_IPV4_ENDPOINTt_INNER_VLAN_PRIf:
            case OAM_BFD_IPV4_ENDPOINTt_INNER_TPIDf:
            case OAM_BFD_IPV4_ENDPOINTt_SRC_IPV4f:
            case OAM_BFD_IPV4_ENDPOINTt_DST_IPV4f:
            case OAM_BFD_IPV4_ENDPOINTt_TOSf:
            case OAM_BFD_IPV4_ENDPOINTt_TTLf:
            case OAM_BFD_IPV4_ENDPOINTt_TX_MODEf:
            case OAM_BFD_IPV4_ENDPOINTt_OPERATIONAL_STATEf:
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


/* Populate the output fields in OAM_BFD_IPV4_ENDPOINT_STATUS LT. The fields are
 * already sorted on the basis of fid.
 */
static void
bfd_endpoint_ipv4_status_populate(bcmcth_oam_bfd_endpoint_status_t *entry,
                                  bcmltd_fields_t *output_fields)
{
    uint32_t fid;
    uint64_t *data;
    size_t i;

    for (i = 0; i < output_fields->count; i++) {
        fid = output_fields->field[i]->id;
        data = &(output_fields->field[i]->data);

        switch (fid) {
            case OAM_BFD_IPV4_ENDPOINT_STATUSt_LOCAL_STATEf:
                *data = entry->local_state;
                break;

            case OAM_BFD_IPV4_ENDPOINT_STATUSt_REMOTE_STATEf:
                *data = entry->remote_state;
                break;

            case OAM_BFD_IPV4_ENDPOINT_STATUSt_LOCAL_DIAG_CODEf:
                *data = entry->local_diag_code;
                break;

            case OAM_BFD_IPV4_ENDPOINT_STATUSt_REMOTE_DIAG_CODEf:
                *data = entry->remote_diag_code;
                break;

            case OAM_BFD_IPV4_ENDPOINT_STATUSt_LOCAL_AUTH_SEQ_NUMf:
                *data = entry->local_auth_seq_num;
                break;

            case OAM_BFD_IPV4_ENDPOINT_STATUSt_REMOTE_AUTH_SEQ_NUMf:
                *data = entry->remote_auth_seq_num;
                break;

            case OAM_BFD_IPV4_ENDPOINT_STATUSt_POLL_SEQUENCE_ACTIVEf:
                *data = entry->poll_sequence_active;
                break;

            case OAM_BFD_IPV4_ENDPOINT_STATUSt_REMOTE_MODEf:
                *data = entry->remote_mode;
                break;

            case OAM_BFD_IPV4_ENDPOINT_STATUSt_LOCAL_DISCRIMINATORf:
                *data = entry->local_discr;
                break;

            case OAM_BFD_IPV4_ENDPOINT_STATUSt_REMOTE_DISCRIMINATORf:
                *data = entry->remote_discr;
                break;

            case OAM_BFD_IPV4_ENDPOINT_STATUSt_REMOTE_DETECT_MULTIPLIERf:
                *data = entry->remote_detect_multiplier;
                break;

            case OAM_BFD_IPV4_ENDPOINT_STATUSt_REMOTE_MIN_TX_INTERVAL_USECSf:
                *data = entry->remote_min_tx_interval_usecs;
                break;

            case OAM_BFD_IPV4_ENDPOINT_STATUSt_REMOTE_MIN_RX_INTERVAL_USECSf:
                *data = entry->remote_min_rx_interval_usecs;
                break;

            case OAM_BFD_IPV4_ENDPOINT_STATUSt_REMOTE_MIN_ECHO_RX_INTERVAL_USECSf:
                *data = entry->remote_min_echo_rx_interval_usecs;
                break;

            case OAM_BFD_IPV4_ENDPOINT_STATUSt_ENDPOINT_STATEf:
                *data = entry->endpoint_state;
                break;

            default:
                /* Nothing to update. */
                break;
        }
    }
}

/* Populate the output fields in OAM_BFD_IPV4_ENDPOINT_STATS LT. The fields are
 * already sorted on the basis of fid.
 */
static void
bfd_endpoint_ipv4_stats_populate(bcmcth_oam_bfd_endpoint_stats_t *entry,
                                 bcmltd_fields_t *output_fields)
{
    uint32_t fid;
    uint64_t *data;
    size_t i;

    for (i = 0; i < output_fields->count; i++) {
        fid = output_fields->field[i]->id;
        data = &(output_fields->field[i]->data);

        switch (fid) {
            case OAM_BFD_IPV4_ENDPOINT_STATSt_RX_PKT_CNTf:
                *data = entry->rx_pkt_cnt;
                break;

            case OAM_BFD_IPV4_ENDPOINT_STATSt_RX_PKT_DISCARD_CNTf:
                *data = entry->rx_pkt_discard_cnt;
                break;

            case OAM_BFD_IPV4_ENDPOINT_STATSt_RX_PKT_AUTH_FAILURE_DISCARD_CNTf:
                *data = entry->rx_pkt_auth_failure_discard_cnt;
                break;

            case OAM_BFD_IPV4_ENDPOINT_STATSt_RX_ECHO_REPLY_PKT_CNTf:
                *data = entry->rx_echo_reply_pkt_cnt;
                break;

            case OAM_BFD_IPV4_ENDPOINT_STATSt_TX_PKT_CNTf:
                *data = entry->tx_pkt_cnt;
                break;

            default:
                /* Nothing to update. */
                break;
        }
    }
}

static int
bfd_endpoint_ipv4_stats_field_get(int unit, const bcmltd_field_t *field,
                                  bcmcth_oam_bfd_endpoint_stats_t *entry)
{
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    fid = field->id;
    fval = field->data;

    switch (fid) {
        case OAM_BFD_IPV4_ENDPOINT_STATSt_RX_PKT_CNTf:
            entry->rx_pkt_cnt = fval;
            entry->rx_pkt_cnt_update = true;
            break;

        case OAM_BFD_IPV4_ENDPOINT_STATSt_RX_PKT_DISCARD_CNTf:
            entry->rx_pkt_discard_cnt = fval;
            entry->rx_pkt_discard_cnt_update = true;
            break;

        case OAM_BFD_IPV4_ENDPOINT_STATSt_RX_PKT_AUTH_FAILURE_DISCARD_CNTf:
            entry->rx_pkt_auth_failure_discard_cnt = fval;
            entry->rx_pkt_auth_failure_discard_cnt_update = true;
            break;

        case OAM_BFD_IPV4_ENDPOINT_STATSt_RX_ECHO_REPLY_PKT_CNTf:
            entry->rx_echo_reply_pkt_cnt = fval;
            entry->rx_echo_reply_pkt_cnt_update = true;
            break;

        case OAM_BFD_IPV4_ENDPOINT_STATSt_TX_PKT_CNTf:
            entry->tx_pkt_cnt = fval;
            entry->tx_pkt_cnt_update = true;
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * BFD IPv6 endpoint functions.
 */
static int
bfd_endpoint_ipv6_field_get(int unit, const bcmltd_field_t *field,
                            bcmcth_oam_bfd_endpoint_t *entry)
{
    bcmcth_oam_bfd_drv_t *bfd_drv = NULL;
    uint32_t fid;
    uint32_t fval32[2];
    uint64_t fval;
    int rv;

    SHR_FUNC_ENTER(unit);

    bfd_drv = bcmcth_oam_bfd_drv_get(unit);
    SHR_NULL_CHECK(bfd_drv, SHR_E_INIT);

    fid = field->id;
    fval = field->data;

    switch (fid) {
        case OAM_BFD_IPV6_ENDPOINTt_ROLEf:
            entry->role = fval;
            break;

        case OAM_BFD_IPV6_ENDPOINTt_TYPEf:
            entry->type.ipv6.ip_ep_type = fval;
            break;

        case OAM_BFD_IPV6_ENDPOINTt_MODEf:
            entry->mode = fval;
            break;

        case OAM_BFD_IPV6_ENDPOINTt_ECHOf:
            entry->echo = fval;
            break;

        case OAM_BFD_IPV6_ENDPOINTt_LOCAL_DISCRIMINATORf:
            entry->local_discr = fval;
            break;

        case OAM_BFD_IPV6_ENDPOINTt_REMOTE_DISCRIMINATORf:
            entry->remote_discr = fval;
            break;

        case OAM_BFD_IPV6_ENDPOINTt_DIAG_CODEf:
            entry->diag_code = fval;
            break;

        case OAM_BFD_IPV6_ENDPOINTt_MIN_TX_INTERVAL_USECSf:
            entry->min_tx_interval_usecs = fval;
            break;

        case OAM_BFD_IPV6_ENDPOINTt_MIN_RX_INTERVAL_USECSf:
            entry->min_rx_interval_usecs = fval;
            break;

        case OAM_BFD_IPV6_ENDPOINTt_MIN_ECHO_RX_INTERVAL_USECSf:
            entry->min_echo_rx_interval_usecs = fval;
            break;

        case OAM_BFD_IPV6_ENDPOINTt_DETECT_MULTIPLIERf:
            entry->detect_multiplier = fval;
            break;

        case OAM_BFD_IPV6_ENDPOINTt_AUTH_TYPEf:
            entry->auth_type = fval;
            break;

        case OAM_BFD_IPV6_ENDPOINTt_OAM_BFD_AUTH_SIMPLE_PASSWORD_IDf:
            entry->sp.id = fval;
            rv = bcmcth_oam_bfd_auth_sp_entry_get(unit, entry->sp.id,
                                                  &(entry->sp));
            if (rv == SHR_E_NONE) {
                entry->auth_sp_found = true;
            }
            break;

        case OAM_BFD_IPV6_ENDPOINTt_OAM_BFD_AUTH_SHA1_IDf:
            entry->sha1.id = fval;
            rv = bcmcth_oam_bfd_auth_sha1_entry_get(unit, entry->sha1.id,
                                                    &(entry->sha1));
            if (rv == SHR_E_NONE) {
                entry->auth_sha1_found = true;
            }
            break;

        case OAM_BFD_IPV6_ENDPOINTt_INITIAL_SHA1_SEQ_NUMf:
            entry->initial_sha1_seq_num = fval;
            break;

        case OAM_BFD_IPV6_ENDPOINTt_SHA1_SEQ_NUM_INCREMENTf:
            entry->sha1_seq_num_incr = fval;
            break;

        case OAM_BFD_IPV6_ENDPOINTt_POLL_SEQUENCEf:
            entry->poll_sequence = fval;
            break;

        case OAM_BFD_IPV6_ENDPOINTt_RX_LOOKUP_VLAN_IDf:
            entry->type.ipv6.rx_lookup_vlan_id = fval;
            break;

        case OAM_BFD_IPV6_ENDPOINTt_PORT_IDf:
            entry->port = fval;
            break;

        case OAM_BFD_IPV6_ENDPOINTt_COSf:
            entry->cos = fval;
            break;

        case OAM_BFD_IPV6_ENDPOINTt_DST_MACf:
            fval32[0] = (uint32_t)fval;
            fval32[1] = (uint32_t)(fval >> 32);
            BCMCTH_MAC_ADDR_FROM_UINT32(entry->dst_mac, fval32);
            break;

        case OAM_BFD_IPV6_ENDPOINTt_SRC_MACf:
            fval32[0] = (uint32_t)fval;
            fval32[1] = (uint32_t)(fval >> 32);
            BCMCTH_MAC_ADDR_FROM_UINT32(entry->src_mac, fval32);
            break;

        case OAM_BFD_IPV6_ENDPOINTt_TAG_TYPEf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bfd_drv->num_vlans_get(fval, &(entry->num_vlans)));
            break;

        case OAM_BFD_IPV6_ENDPOINTt_VLAN_IDf:
            entry->outer_vlan.tci.vid = fval;
            break;

        case OAM_BFD_IPV6_ENDPOINTt_VLAN_PRIf:
            entry->outer_vlan.tci.prio = fval;
            break;

        case OAM_BFD_IPV6_ENDPOINTt_TPIDf:
            entry->outer_vlan.tpid = fval;
            break;

        case OAM_BFD_IPV6_ENDPOINTt_INNER_VLAN_IDf:
            entry->inner_vlan.tci.vid = fval;
            break;

        case OAM_BFD_IPV6_ENDPOINTt_INNER_VLAN_PRIf:
            entry->inner_vlan.tci.prio = fval;
            break;

        case OAM_BFD_IPV6_ENDPOINTt_INNER_TPIDf:
            entry->inner_vlan.tpid = fval;
            break;

        case OAM_BFD_IPV6_ENDPOINTt_SRC_IPV6u_UPPERf:
            BCMCTH_IPV6_ADDR_UPPER_SET(entry->type.ipv6.src_ip, fval);
            break;

        case OAM_BFD_IPV6_ENDPOINTt_SRC_IPV6u_LOWERf:
            BCMCTH_IPV6_ADDR_LOWER_SET(entry->type.ipv6.src_ip, fval);
            break;

        case OAM_BFD_IPV6_ENDPOINTt_DST_IPV6u_UPPERf:
            BCMCTH_IPV6_ADDR_UPPER_SET(entry->type.ipv6.dst_ip, fval);
            break;

        case OAM_BFD_IPV6_ENDPOINTt_DST_IPV6u_LOWERf:
            BCMCTH_IPV6_ADDR_LOWER_SET(entry->type.ipv6.dst_ip, fval);
            break;

        case OAM_BFD_IPV6_ENDPOINTt_TRAFFIC_CLASSf:
            entry->type.ipv6.traffic_class = fval;
            break;

        case OAM_BFD_IPV6_ENDPOINTt_HOP_LIMITf:
            entry->type.ipv6.hop_limit = fval;
            break;

        case OAM_BFD_IPV6_ENDPOINTt_UDP_SRC_PORTf:
            entry->udp_src_port = fval;
            break;

        case OAM_BFD_IPV6_ENDPOINTt_TX_MODEf:
            entry->tx_mode = fval;
            break;

        case OAM_BFD_IPV6_ENDPOINTt_OPERATIONAL_STATEf:
            entry->oper_state = fval;
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bfd_endpoint_ipv6_defaults_get(int unit, bcmcth_oam_bfd_endpoint_t *entry)
{
    bcmcth_oam_bfd_drv_t *bfd_drv = NULL;
    bcmlrd_client_field_info_t *fields_info = NULL;
    size_t fld_num;
    size_t actual_fld_num;
    size_t idx;

    SHR_FUNC_ENTER(unit);

    bfd_drv = bcmcth_oam_bfd_drv_get(unit);
    SHR_NULL_CHECK(bfd_drv, SHR_E_INIT);

    sal_memset(entry, 0, sizeof(*entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_oam_bfd_num_data_fields_get(unit, OAM_BFD_IPV6_ENDPOINTt,
                                            &fld_num));
    SHR_ALLOC(fields_info, sizeof(bcmlrd_client_field_info_t) * fld_num,
              "bcmcthOamBfdEndpointIpv6FldInfo");
    SHR_NULL_CHECK(fields_info, SHR_E_MEMORY);

    SHR_IF_ERR_EXIT
        (bcmlrd_table_fields_def_get(unit, "OAM_BFD_IPV6_ENDPOINT",
                                     fld_num, fields_info, &actual_fld_num));

    for (idx = 0; idx < actual_fld_num; idx++) {
        switch (fields_info[idx].id) {
            case OAM_BFD_IPV6_ENDPOINTt_ROLEf:
                entry->role = fields_info[idx].def.u8;
                break;

            case OAM_BFD_IPV6_ENDPOINTt_TYPEf:
                entry->type.ipv6.ip_ep_type = fields_info[idx].def.u8;
                break;

            case OAM_BFD_IPV6_ENDPOINTt_MODEf:
                entry->mode = fields_info[idx].def.u8;
                break;

            case OAM_BFD_IPV6_ENDPOINTt_ECHOf:
                entry->echo = fields_info[idx].def.is_true;
                break;

            case OAM_BFD_IPV6_ENDPOINTt_DIAG_CODEf:
                entry->diag_code = fields_info[idx].def.u8;
                break;

            case OAM_BFD_IPV6_ENDPOINTt_MIN_TX_INTERVAL_USECSf:
                entry->min_tx_interval_usecs = fields_info[idx].def.u32;
                break;

            case OAM_BFD_IPV6_ENDPOINTt_MIN_RX_INTERVAL_USECSf:
                entry->min_rx_interval_usecs = fields_info[idx].def.u32;
                break;

            case OAM_BFD_IPV6_ENDPOINTt_MIN_ECHO_RX_INTERVAL_USECSf:
                entry->min_echo_rx_interval_usecs = fields_info[idx].def.u32;
                break;

            case OAM_BFD_IPV6_ENDPOINTt_DETECT_MULTIPLIERf:
                entry->detect_multiplier = fields_info[idx].def.u8;
                break;

            case OAM_BFD_IPV6_ENDPOINTt_AUTH_TYPEf:
                entry->auth_type = fields_info[idx].def.u8;
                break;

            case OAM_BFD_IPV6_ENDPOINTt_INITIAL_SHA1_SEQ_NUMf:
                entry->initial_sha1_seq_num = fields_info[idx].def.u32;
                break;

            case OAM_BFD_IPV6_ENDPOINTt_SHA1_SEQ_NUM_INCREMENTf:
                entry->sha1_seq_num_incr = fields_info[idx].def.u8;
                break;

            case OAM_BFD_IPV6_ENDPOINTt_POLL_SEQUENCEf:
                entry->poll_sequence = fields_info[idx].def.u8;
                break;

            case OAM_BFD_IPV6_ENDPOINTt_COSf:
                entry->cos = fields_info[idx].def.u8;
                break;

            case OAM_BFD_IPV6_ENDPOINTt_TAG_TYPEf:
                SHR_IF_ERR_VERBOSE_EXIT
                    (bfd_drv->num_vlans_get(fields_info[idx].def.u8,
                                            &(entry->num_vlans)));
                break;

            case OAM_BFD_IPV6_ENDPOINTt_UDP_SRC_PORTf:
                entry->udp_src_port = fields_info[idx].def.u16;
                break;

            case OAM_BFD_IPV6_ENDPOINTt_OAM_BFD_IPV6_ENDPOINT_IDf:
            case OAM_BFD_IPV6_ENDPOINTt_LOCAL_DISCRIMINATORf:
            case OAM_BFD_IPV6_ENDPOINTt_REMOTE_DISCRIMINATORf:
            case OAM_BFD_IPV6_ENDPOINTt_OAM_BFD_AUTH_SIMPLE_PASSWORD_IDf:
            case OAM_BFD_IPV6_ENDPOINTt_OAM_BFD_AUTH_SHA1_IDf:
            case OAM_BFD_IPV6_ENDPOINTt_RX_LOOKUP_VLAN_IDf:
            case OAM_BFD_IPV6_ENDPOINTt_PORT_IDf:
            case OAM_BFD_IPV6_ENDPOINTt_DST_MACf:
            case OAM_BFD_IPV6_ENDPOINTt_SRC_MACf:
            case OAM_BFD_IPV6_ENDPOINTt_VLAN_IDf:
            case OAM_BFD_IPV6_ENDPOINTt_VLAN_PRIf:
            case OAM_BFD_IPV6_ENDPOINTt_TPIDf:
            case OAM_BFD_IPV6_ENDPOINTt_INNER_VLAN_IDf:
            case OAM_BFD_IPV6_ENDPOINTt_INNER_VLAN_PRIf:
            case OAM_BFD_IPV6_ENDPOINTt_INNER_TPIDf:
            case OAM_BFD_IPV6_ENDPOINTt_SRC_IPV6u_UPPERf:
            case OAM_BFD_IPV6_ENDPOINTt_SRC_IPV6u_LOWERf:
            case OAM_BFD_IPV6_ENDPOINTt_DST_IPV6u_UPPERf:
            case OAM_BFD_IPV6_ENDPOINTt_DST_IPV6u_LOWERf:
            case OAM_BFD_IPV6_ENDPOINTt_TRAFFIC_CLASSf:
            case OAM_BFD_IPV6_ENDPOINTt_HOP_LIMITf:
            case OAM_BFD_IPV6_ENDPOINTt_TX_MODEf:
            case OAM_BFD_IPV6_ENDPOINTt_OPERATIONAL_STATEf:
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

/* Populate the output fields in OAM_BFD_IPV6_ENDPOINT_STATUS LT. The fields are
 * already sorted on the basis of fid.
 */
static void
bfd_endpoint_ipv6_status_populate(bcmcth_oam_bfd_endpoint_status_t *entry,
                                  bcmltd_fields_t *output_fields)
{
    uint32_t fid;
    uint64_t *data;
    size_t i;

    for (i = 0; i < output_fields->count; i++) {
        fid = output_fields->field[i]->id;
        data = &(output_fields->field[i]->data);

        switch (fid) {
            case OAM_BFD_IPV6_ENDPOINT_STATUSt_LOCAL_STATEf:
                *data = entry->local_state;
                break;

            case OAM_BFD_IPV6_ENDPOINT_STATUSt_REMOTE_STATEf:
                *data = entry->remote_state;
                break;

            case OAM_BFD_IPV6_ENDPOINT_STATUSt_LOCAL_DIAG_CODEf:
                *data = entry->local_diag_code;
                break;

            case OAM_BFD_IPV6_ENDPOINT_STATUSt_REMOTE_DIAG_CODEf:
                *data = entry->remote_diag_code;
                break;

            case OAM_BFD_IPV6_ENDPOINT_STATUSt_LOCAL_AUTH_SEQ_NUMf:
                *data = entry->local_auth_seq_num;
                break;

            case OAM_BFD_IPV6_ENDPOINT_STATUSt_REMOTE_AUTH_SEQ_NUMf:
                *data = entry->remote_auth_seq_num;
                break;

            case OAM_BFD_IPV6_ENDPOINT_STATUSt_POLL_SEQUENCE_ACTIVEf:
                *data = entry->poll_sequence_active;
                break;

            case OAM_BFD_IPV6_ENDPOINT_STATUSt_REMOTE_MODEf:
                *data = entry->remote_mode;
                break;

            case OAM_BFD_IPV6_ENDPOINT_STATUSt_LOCAL_DISCRIMINATORf:
                *data = entry->local_discr;
                break;

            case OAM_BFD_IPV6_ENDPOINT_STATUSt_REMOTE_DISCRIMINATORf:
                *data = entry->remote_discr;
                break;

            case OAM_BFD_IPV6_ENDPOINT_STATUSt_REMOTE_DETECT_MULTIPLIERf:
                *data = entry->remote_detect_multiplier;
                break;

            case OAM_BFD_IPV6_ENDPOINT_STATUSt_REMOTE_MIN_TX_INTERVAL_USECSf:
                *data = entry->remote_min_tx_interval_usecs;
                break;

            case OAM_BFD_IPV6_ENDPOINT_STATUSt_REMOTE_MIN_RX_INTERVAL_USECSf:
                *data = entry->remote_min_rx_interval_usecs;
                break;

            case OAM_BFD_IPV6_ENDPOINT_STATUSt_REMOTE_MIN_ECHO_RX_INTERVAL_USECSf:
                *data = entry->remote_min_echo_rx_interval_usecs;
                break;

            case OAM_BFD_IPV6_ENDPOINT_STATUSt_ENDPOINT_STATEf:
                *data = entry->endpoint_state;
                break;

            default:
                /* Nothing to update. */
                break;
        }
    }

}

/* Populate the output fields in OAM_BFD_IPV6_ENDPOINT_STATS LT. The fields are
 * already sorted on the basis of fid.
 */
static void
bfd_endpoint_ipv6_stats_populate(bcmcth_oam_bfd_endpoint_stats_t *entry,
                                 bcmltd_fields_t *output_fields)
{
    uint32_t fid;
    uint64_t *data;
    size_t i;

    for (i = 0; i < output_fields->count; i++) {
        fid = output_fields->field[i]->id;
        data = &(output_fields->field[i]->data);

        switch (fid) {
            case OAM_BFD_IPV6_ENDPOINT_STATSt_RX_PKT_CNTf:
                *data = entry->rx_pkt_cnt;
                break;

            case OAM_BFD_IPV6_ENDPOINT_STATSt_RX_PKT_DISCARD_CNTf:
                *data = entry->rx_pkt_discard_cnt;
                break;

            case OAM_BFD_IPV6_ENDPOINT_STATSt_RX_PKT_AUTH_FAILURE_DISCARD_CNTf:
                *data = entry->rx_pkt_auth_failure_discard_cnt;
                break;

            case OAM_BFD_IPV6_ENDPOINT_STATSt_RX_ECHO_REPLY_PKT_CNTf:
                *data = entry->rx_echo_reply_pkt_cnt;
                break;

            case OAM_BFD_IPV6_ENDPOINT_STATSt_TX_PKT_CNTf:
                *data = entry->tx_pkt_cnt;
                break;

            default:
                /* Nothing to update. */
                break;
        }
    }
}

static int
bfd_endpoint_ipv6_stats_field_get(int unit, const bcmltd_field_t *field,
                                  bcmcth_oam_bfd_endpoint_stats_t *entry)
{
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    fid = field->id;
    fval = field->data;

    switch (fid) {
        case OAM_BFD_IPV6_ENDPOINT_STATSt_RX_PKT_CNTf:
            entry->rx_pkt_cnt = fval;
            entry->rx_pkt_cnt_update = true;
            break;

        case OAM_BFD_IPV6_ENDPOINT_STATSt_RX_PKT_DISCARD_CNTf:
            entry->rx_pkt_discard_cnt = fval;
            entry->rx_pkt_discard_cnt_update = true;
            break;

        case OAM_BFD_IPV6_ENDPOINT_STATSt_RX_PKT_AUTH_FAILURE_DISCARD_CNTf:
            entry->rx_pkt_auth_failure_discard_cnt = fval;
            entry->rx_pkt_auth_failure_discard_cnt_update = true;
            break;

        case OAM_BFD_IPV6_ENDPOINT_STATSt_RX_ECHO_REPLY_PKT_CNTf:
            entry->rx_echo_reply_pkt_cnt = fval;
            entry->rx_echo_reply_pkt_cnt_update = true;
            break;

        case OAM_BFD_IPV6_ENDPOINT_STATSt_TX_PKT_CNTf:
            entry->tx_pkt_cnt = fval;
            entry->tx_pkt_cnt_update = true;
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * BFD Tunnel IPv4 endpoint functions.
 */
static int
bfd_endpoint_tnl_ipv4_field_get(int unit, const bcmltd_field_t *field,
                                bcmcth_oam_bfd_endpoint_t *entry)
{
    bcmcth_oam_bfd_drv_t *bfd_drv = NULL;
    uint32_t fid;
    uint32_t fval32[2];
    uint64_t fval;
    int rv;

    SHR_FUNC_ENTER(unit);

    bfd_drv = bcmcth_oam_bfd_drv_get(unit);
    SHR_NULL_CHECK(bfd_drv, SHR_E_INIT);

    fid = field->id;
    fval = field->data;

    switch (fid) {
        case OAM_BFD_TNL_IPV4_ENDPOINTt_ROLEf:
            entry->role = fval;
            break;

        case OAM_BFD_TNL_IPV4_ENDPOINTt_INNER_IP_TYPEf:
            entry->type.tnl_ipv4.inner_ip_type = fval;
            break;

        case OAM_BFD_TNL_IPV4_ENDPOINTt_GREf:
            entry->type.tnl_ipv4.gre = fval;
            break;

        case OAM_BFD_TNL_IPV4_ENDPOINTt_MODEf:
            entry->mode = fval;
            break;

        case OAM_BFD_TNL_IPV4_ENDPOINTt_ECHOf:
            entry->echo = fval;
            break;

        case OAM_BFD_TNL_IPV4_ENDPOINTt_LOCAL_DISCRIMINATORf:
            entry->local_discr = fval;
            break;

        case OAM_BFD_TNL_IPV4_ENDPOINTt_REMOTE_DISCRIMINATORf:
            entry->remote_discr = fval;
            break;

        case OAM_BFD_TNL_IPV4_ENDPOINTt_DIAG_CODEf:
            entry->diag_code = fval;
            break;

        case OAM_BFD_TNL_IPV4_ENDPOINTt_MIN_TX_INTERVAL_USECSf:
            entry->min_tx_interval_usecs = fval;
            break;

        case OAM_BFD_TNL_IPV4_ENDPOINTt_MIN_RX_INTERVAL_USECSf:
            entry->min_rx_interval_usecs = fval;
            break;

        case OAM_BFD_TNL_IPV4_ENDPOINTt_MIN_ECHO_RX_INTERVAL_USECSf:
            entry->min_echo_rx_interval_usecs = fval;
            break;

        case OAM_BFD_TNL_IPV4_ENDPOINTt_DETECT_MULTIPLIERf:
            entry->detect_multiplier = fval;
            break;

        case OAM_BFD_TNL_IPV4_ENDPOINTt_AUTH_TYPEf:
            entry->auth_type = fval;
            break;

        case OAM_BFD_TNL_IPV4_ENDPOINTt_OAM_BFD_AUTH_SIMPLE_PASSWORD_IDf:
            entry->sp.id = fval;
            rv = bcmcth_oam_bfd_auth_sp_entry_get(unit, entry->sp.id,
                                                  &(entry->sp));
            if (rv == SHR_E_NONE) {
                entry->auth_sp_found = true;
            }
            break;

        case OAM_BFD_TNL_IPV4_ENDPOINTt_OAM_BFD_AUTH_SHA1_IDf:
            entry->sha1.id = fval;
            rv = bcmcth_oam_bfd_auth_sha1_entry_get(unit, entry->sha1.id,
                                                    &(entry->sha1));
            if (rv == SHR_E_NONE) {
                entry->auth_sha1_found = true;
            }
            break;

        case OAM_BFD_TNL_IPV4_ENDPOINTt_INITIAL_SHA1_SEQ_NUMf:
            entry->initial_sha1_seq_num = fval;
            break;

        case OAM_BFD_TNL_IPV4_ENDPOINTt_SHA1_SEQ_NUM_INCREMENTf:
            entry->sha1_seq_num_incr = fval;
            break;

        case OAM_BFD_TNL_IPV4_ENDPOINTt_POLL_SEQUENCEf:
            entry->poll_sequence = fval;
            break;

        case OAM_BFD_TNL_IPV4_ENDPOINTt_RX_LOOKUP_VLAN_IDf:
            entry->type.tnl_ipv4.rx_lookup_vlan_id = fval;
            break;

        case OAM_BFD_TNL_IPV4_ENDPOINTt_PORT_IDf:
            entry->port = fval;
            break;

        case OAM_BFD_TNL_IPV4_ENDPOINTt_COSf:
            entry->cos = fval;
            break;

        case OAM_BFD_TNL_IPV4_ENDPOINTt_DST_MACf:
            fval32[0] = (uint32_t)fval;
            fval32[1] = (uint32_t)(fval >> 32);
            BCMCTH_MAC_ADDR_FROM_UINT32(entry->dst_mac, fval32);
            break;

        case OAM_BFD_TNL_IPV4_ENDPOINTt_SRC_MACf:
            fval32[0] = (uint32_t)fval;
            fval32[1] = (uint32_t)(fval >> 32);
            BCMCTH_MAC_ADDR_FROM_UINT32(entry->src_mac, fval32);
            break;

        case OAM_BFD_TNL_IPV4_ENDPOINTt_TAG_TYPEf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bfd_drv->num_vlans_get(fval, &(entry->num_vlans)));
            break;

        case OAM_BFD_TNL_IPV4_ENDPOINTt_VLAN_IDf:
            entry->outer_vlan.tci.vid = fval;
            break;

        case OAM_BFD_TNL_IPV4_ENDPOINTt_VLAN_PRIf:
            entry->outer_vlan.tci.prio = fval;
            break;

        case OAM_BFD_TNL_IPV4_ENDPOINTt_TPIDf:
            entry->outer_vlan.tpid = fval;
            break;

        case OAM_BFD_TNL_IPV4_ENDPOINTt_INNER_VLAN_IDf:
            entry->inner_vlan.tci.vid = fval;
            break;

        case OAM_BFD_TNL_IPV4_ENDPOINTt_INNER_VLAN_PRIf:
            entry->inner_vlan.tci.prio = fval;
            break;

        case OAM_BFD_TNL_IPV4_ENDPOINTt_INNER_TPIDf:
            entry->inner_vlan.tpid = fval;
            break;

        case OAM_BFD_TNL_IPV4_ENDPOINTt_SRC_IPV4f:
            entry->type.tnl_ipv4.src_ip = fval;
            break;

        case OAM_BFD_TNL_IPV4_ENDPOINTt_DST_IPV4f:
            entry->type.tnl_ipv4.dst_ip = fval;
            break;

        case OAM_BFD_TNL_IPV4_ENDPOINTt_TOSf:
            entry->type.tnl_ipv4.tos = fval;
            break;

        case OAM_BFD_TNL_IPV4_ENDPOINTt_TTLf:
            entry->type.tnl_ipv4.ttl = fval;
            break;

        case OAM_BFD_TNL_IPV4_ENDPOINTt_INNER_SRC_IPV4f:
            entry->type.tnl_ipv4.inner_ipv4.src_ip = fval;
            break;

        case OAM_BFD_TNL_IPV4_ENDPOINTt_INNER_DST_IPV4f:
            entry->type.tnl_ipv4.inner_ipv4.dst_ip = fval;
            break;

        case OAM_BFD_TNL_IPV4_ENDPOINTt_INNER_TOSf:
            entry->type.tnl_ipv4.inner_ipv4.tos = fval;
            break;

        case OAM_BFD_TNL_IPV4_ENDPOINTt_INNER_SRC_IPV6u_UPPERf:
            BCMCTH_IPV6_ADDR_UPPER_SET(entry->type.tnl_ipv4.inner_ipv6.src_ip, fval);
            break;

        case OAM_BFD_TNL_IPV4_ENDPOINTt_INNER_SRC_IPV6u_LOWERf:
            BCMCTH_IPV6_ADDR_LOWER_SET(entry->type.tnl_ipv4.inner_ipv6.src_ip, fval);
            break;

        case OAM_BFD_TNL_IPV4_ENDPOINTt_INNER_DST_IPV6u_UPPERf:
            BCMCTH_IPV6_ADDR_UPPER_SET(entry->type.tnl_ipv4.inner_ipv6.dst_ip, fval);
            break;

        case OAM_BFD_TNL_IPV4_ENDPOINTt_INNER_DST_IPV6u_LOWERf:
            BCMCTH_IPV6_ADDR_LOWER_SET(entry->type.tnl_ipv4.inner_ipv6.dst_ip, fval);
            break;

        case OAM_BFD_TNL_IPV4_ENDPOINTt_INNER_TRAFFIC_CLASSf:
            entry->type.tnl_ipv4.inner_ipv6.traffic_class = fval;
            break;

        case OAM_BFD_TNL_IPV4_ENDPOINTt_UDP_SRC_PORTf:
            entry->udp_src_port = fval;
            break;

        case OAM_BFD_TNL_IPV4_ENDPOINTt_TX_MODEf:
            entry->tx_mode = fval;
            break;

        case OAM_BFD_TNL_IPV4_ENDPOINTt_OPERATIONAL_STATEf:
            entry->oper_state = fval;
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bfd_endpoint_tnl_ipv4_defaults_get(int unit, bcmcth_oam_bfd_endpoint_t *entry)
{
    bcmcth_oam_bfd_drv_t *bfd_drv = NULL;
    bcmlrd_client_field_info_t *fields_info = NULL;
    size_t fld_num;
    size_t actual_fld_num;
    size_t idx;

    SHR_FUNC_ENTER(unit);

    bfd_drv = bcmcth_oam_bfd_drv_get(unit);
    SHR_NULL_CHECK(bfd_drv, SHR_E_INIT);

    sal_memset(entry, 0, sizeof(*entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_oam_bfd_num_data_fields_get(unit, OAM_BFD_TNL_IPV4_ENDPOINTt,
                                            &fld_num));
    SHR_ALLOC(fields_info, sizeof(bcmlrd_client_field_info_t) * fld_num,
              "bcmcthOamBfdEndpointTnlIpv4FldInfo");
    SHR_NULL_CHECK(fields_info, SHR_E_MEMORY);

    SHR_IF_ERR_EXIT
        (bcmlrd_table_fields_def_get(unit, "OAM_BFD_TNL_IPV4_ENDPOINT",
                                     fld_num, fields_info, &actual_fld_num));

    for (idx = 0; idx < actual_fld_num; idx++) {
        switch (fields_info[idx].id) {
            case OAM_BFD_TNL_IPV4_ENDPOINTt_ROLEf:
                entry->role = fields_info[idx].def.u8;
                break;

            case OAM_BFD_TNL_IPV4_ENDPOINTt_INNER_IP_TYPEf:
                entry->type.tnl_ipv4.inner_ip_type = fields_info[idx].def.u8;
                break;

            case OAM_BFD_TNL_IPV4_ENDPOINTt_GREf:
                entry->type.tnl_ipv4.gre = fields_info[idx].def.is_true;
                break;

            case OAM_BFD_TNL_IPV4_ENDPOINTt_MODEf:
                entry->mode = fields_info[idx].def.u8;
                break;

            case OAM_BFD_TNL_IPV4_ENDPOINTt_ECHOf:
                entry->echo = fields_info[idx].def.is_true;
                break;

            case OAM_BFD_TNL_IPV4_ENDPOINTt_DIAG_CODEf:
                entry->diag_code = fields_info[idx].def.u8;
                break;

            case OAM_BFD_TNL_IPV4_ENDPOINTt_MIN_TX_INTERVAL_USECSf:
                entry->min_tx_interval_usecs = fields_info[idx].def.u32;
                break;

            case OAM_BFD_TNL_IPV4_ENDPOINTt_MIN_RX_INTERVAL_USECSf:
                entry->min_rx_interval_usecs = fields_info[idx].def.u32;
                break;

            case OAM_BFD_TNL_IPV4_ENDPOINTt_MIN_ECHO_RX_INTERVAL_USECSf:
                entry->min_echo_rx_interval_usecs = fields_info[idx].def.u32;
                break;

            case OAM_BFD_TNL_IPV4_ENDPOINTt_DETECT_MULTIPLIERf:
                entry->detect_multiplier = fields_info[idx].def.u8;
                break;

            case OAM_BFD_TNL_IPV4_ENDPOINTt_AUTH_TYPEf:
                entry->auth_type = fields_info[idx].def.u8;
                break;

            case OAM_BFD_TNL_IPV4_ENDPOINTt_INITIAL_SHA1_SEQ_NUMf:
                entry->initial_sha1_seq_num = fields_info[idx].def.u32;
                break;

            case OAM_BFD_TNL_IPV4_ENDPOINTt_SHA1_SEQ_NUM_INCREMENTf:
                entry->sha1_seq_num_incr = fields_info[idx].def.u8;
                break;

            case OAM_BFD_TNL_IPV4_ENDPOINTt_POLL_SEQUENCEf:
                entry->poll_sequence = fields_info[idx].def.u8;
                break;

            case OAM_BFD_TNL_IPV4_ENDPOINTt_COSf:
                entry->cos = fields_info[idx].def.u8;
                break;

            case OAM_BFD_TNL_IPV4_ENDPOINTt_TAG_TYPEf:
                SHR_IF_ERR_VERBOSE_EXIT
                    (bfd_drv->num_vlans_get(fields_info[idx].def.u8,
                                            &(entry->num_vlans)));
                break;

            case OAM_BFD_TNL_IPV4_ENDPOINTt_UDP_SRC_PORTf:
                entry->udp_src_port = fields_info[idx].def.u16;
                break;

            case OAM_BFD_TNL_IPV4_ENDPOINTt_OAM_BFD_TNL_IPV4_ENDPOINT_IDf:
            case OAM_BFD_TNL_IPV4_ENDPOINTt_LOCAL_DISCRIMINATORf:
            case OAM_BFD_TNL_IPV4_ENDPOINTt_REMOTE_DISCRIMINATORf:
            case OAM_BFD_TNL_IPV4_ENDPOINTt_OAM_BFD_AUTH_SIMPLE_PASSWORD_IDf:
            case OAM_BFD_TNL_IPV4_ENDPOINTt_OAM_BFD_AUTH_SHA1_IDf:
            case OAM_BFD_TNL_IPV4_ENDPOINTt_RX_LOOKUP_VLAN_IDf:
            case OAM_BFD_TNL_IPV4_ENDPOINTt_PORT_IDf:
            case OAM_BFD_TNL_IPV4_ENDPOINTt_DST_MACf:
            case OAM_BFD_TNL_IPV4_ENDPOINTt_SRC_MACf:
            case OAM_BFD_TNL_IPV4_ENDPOINTt_VLAN_IDf:
            case OAM_BFD_TNL_IPV4_ENDPOINTt_VLAN_PRIf:
            case OAM_BFD_TNL_IPV4_ENDPOINTt_TPIDf:
            case OAM_BFD_TNL_IPV4_ENDPOINTt_INNER_VLAN_IDf:
            case OAM_BFD_TNL_IPV4_ENDPOINTt_INNER_VLAN_PRIf:
            case OAM_BFD_TNL_IPV4_ENDPOINTt_INNER_TPIDf:
            case OAM_BFD_TNL_IPV4_ENDPOINTt_SRC_IPV4f:
            case OAM_BFD_TNL_IPV4_ENDPOINTt_DST_IPV4f:
            case OAM_BFD_TNL_IPV4_ENDPOINTt_TOSf:
            case OAM_BFD_TNL_IPV4_ENDPOINTt_TTLf:
            case OAM_BFD_TNL_IPV4_ENDPOINTt_INNER_SRC_IPV4f:
            case OAM_BFD_TNL_IPV4_ENDPOINTt_INNER_DST_IPV4f:
            case OAM_BFD_TNL_IPV4_ENDPOINTt_INNER_TOSf:
            case OAM_BFD_TNL_IPV4_ENDPOINTt_INNER_SRC_IPV6u_UPPERf:
            case OAM_BFD_TNL_IPV4_ENDPOINTt_INNER_SRC_IPV6u_LOWERf:
            case OAM_BFD_TNL_IPV4_ENDPOINTt_INNER_DST_IPV6u_UPPERf:
            case OAM_BFD_TNL_IPV4_ENDPOINTt_INNER_DST_IPV6u_LOWERf:
            case OAM_BFD_TNL_IPV4_ENDPOINTt_INNER_TRAFFIC_CLASSf:
            case OAM_BFD_TNL_IPV4_ENDPOINTt_TX_MODEf:
            case OAM_BFD_TNL_IPV4_ENDPOINTt_OPERATIONAL_STATEf:
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

/* Populate the output fields in OAM_BFD_TNL_IPV4_ENDPOINT_STATUS LT. The fields
 * are already sorted on the basis of fid.
 */
static void
bfd_endpoint_tnl_ipv4_status_populate(bcmcth_oam_bfd_endpoint_status_t *entry,
                                      bcmltd_fields_t *output_fields)
{
    uint32_t fid;
    uint64_t *data;
    size_t i;

    for (i = 0; i < output_fields->count; i++) {
        fid = output_fields->field[i]->id;
        data = &(output_fields->field[i]->data);

        switch (fid) {
            case OAM_BFD_TNL_IPV4_ENDPOINT_STATUSt_LOCAL_STATEf:
                *data = entry->local_state;
                break;

            case OAM_BFD_TNL_IPV4_ENDPOINT_STATUSt_REMOTE_STATEf:
                *data = entry->remote_state;
                break;

            case OAM_BFD_TNL_IPV4_ENDPOINT_STATUSt_LOCAL_DIAG_CODEf:
                *data = entry->local_diag_code;
                break;

            case OAM_BFD_TNL_IPV4_ENDPOINT_STATUSt_REMOTE_DIAG_CODEf:
                *data = entry->remote_diag_code;
                break;

            case OAM_BFD_TNL_IPV4_ENDPOINT_STATUSt_LOCAL_AUTH_SEQ_NUMf:
                *data = entry->local_auth_seq_num;
                break;

            case OAM_BFD_TNL_IPV4_ENDPOINT_STATUSt_REMOTE_AUTH_SEQ_NUMf:
                *data = entry->remote_auth_seq_num;
                break;

            case OAM_BFD_TNL_IPV4_ENDPOINT_STATUSt_POLL_SEQUENCE_ACTIVEf:
                *data = entry->poll_sequence_active;
                break;

            case OAM_BFD_TNL_IPV4_ENDPOINT_STATUSt_REMOTE_MODEf:
                *data = entry->remote_mode;
                break;

            case OAM_BFD_TNL_IPV4_ENDPOINT_STATUSt_LOCAL_DISCRIMINATORf:
                *data = entry->local_discr;
                break;

            case OAM_BFD_TNL_IPV4_ENDPOINT_STATUSt_REMOTE_DISCRIMINATORf:
                *data = entry->remote_discr;
                break;

            case OAM_BFD_TNL_IPV4_ENDPOINT_STATUSt_REMOTE_DETECT_MULTIPLIERf:
                *data = entry->remote_detect_multiplier;
                break;

            case OAM_BFD_TNL_IPV4_ENDPOINT_STATUSt_REMOTE_MIN_TX_INTERVAL_USECSf:
                *data = entry->remote_min_tx_interval_usecs;
                break;

            case OAM_BFD_TNL_IPV4_ENDPOINT_STATUSt_REMOTE_MIN_RX_INTERVAL_USECSf:
                *data = entry->remote_min_rx_interval_usecs;
                break;

            case OAM_BFD_TNL_IPV4_ENDPOINT_STATUSt_REMOTE_MIN_ECHO_RX_INTERVAL_USECSf:
                *data = entry->remote_min_echo_rx_interval_usecs;
                break;

            case OAM_BFD_TNL_IPV4_ENDPOINT_STATUSt_ENDPOINT_STATEf:
                *data = entry->endpoint_state;
                break;

            default:
                /* Nothing to update. */
                break;
        }
    }

}

/* Populate the output fields in OAM_BFD_TNL_IPV4_ENDPOINT_STATS LT. The fields
 * are already sorted on the basis of fid.
 */
static void
bfd_endpoint_tnl_ipv4_stats_populate(bcmcth_oam_bfd_endpoint_stats_t *entry,
                                     bcmltd_fields_t *output_fields)
{
    uint32_t fid;
    uint64_t *data;
    size_t i;

    for (i = 0; i < output_fields->count; i++) {
        fid = output_fields->field[i]->id;
        data = &(output_fields->field[i]->data);

        switch (fid) {
            case OAM_BFD_TNL_IPV4_ENDPOINT_STATSt_RX_PKT_CNTf:
                *data = entry->rx_pkt_cnt;
                break;

            case OAM_BFD_TNL_IPV4_ENDPOINT_STATSt_RX_PKT_DISCARD_CNTf:
                *data = entry->rx_pkt_discard_cnt;
                break;

            case OAM_BFD_TNL_IPV4_ENDPOINT_STATSt_RX_PKT_AUTH_FAILURE_DISCARD_CNTf:
                *data = entry->rx_pkt_auth_failure_discard_cnt;
                break;

            case OAM_BFD_TNL_IPV4_ENDPOINT_STATSt_RX_ECHO_REPLY_PKT_CNTf:
                *data = entry->rx_echo_reply_pkt_cnt;
                break;

            case OAM_BFD_TNL_IPV4_ENDPOINT_STATSt_TX_PKT_CNTf:
                *data = entry->tx_pkt_cnt;
                break;

            default:
                /* Nothing to update. */
                break;
        }
    }

}

static int
bfd_endpoint_tnl_ipv4_stats_field_get(int unit, const bcmltd_field_t *field,
                                      bcmcth_oam_bfd_endpoint_stats_t *entry)
{
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    fid = field->id;
    fval = field->data;

    switch (fid) {
        case OAM_BFD_TNL_IPV4_ENDPOINT_STATSt_RX_PKT_CNTf:
            entry->rx_pkt_cnt = fval;
            entry->rx_pkt_cnt_update = true;
            break;

        case OAM_BFD_TNL_IPV4_ENDPOINT_STATSt_RX_PKT_DISCARD_CNTf:
            entry->rx_pkt_discard_cnt = fval;
            entry->rx_pkt_discard_cnt_update = true;
            break;

        case OAM_BFD_TNL_IPV4_ENDPOINT_STATSt_RX_PKT_AUTH_FAILURE_DISCARD_CNTf:
            entry->rx_pkt_auth_failure_discard_cnt = fval;
            entry->rx_pkt_auth_failure_discard_cnt_update = true;
            break;

        case OAM_BFD_TNL_IPV4_ENDPOINT_STATSt_RX_ECHO_REPLY_PKT_CNTf:
            entry->rx_echo_reply_pkt_cnt = fval;
            entry->rx_echo_reply_pkt_cnt_update = true;
            break;

        case OAM_BFD_TNL_IPV4_ENDPOINT_STATSt_TX_PKT_CNTf:
            entry->tx_pkt_cnt = fval;
            entry->tx_pkt_cnt_update = true;
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * BFD Tunnel IPv6 endpoint functions.
 */
static int
bfd_endpoint_tnl_ipv6_field_get(int unit, const bcmltd_field_t *field,
                                bcmcth_oam_bfd_endpoint_t *entry)
{
    bcmcth_oam_bfd_drv_t *bfd_drv = NULL;
    uint32_t fid;
    uint32_t fval32[2];
    uint64_t fval;
    int rv;

    SHR_FUNC_ENTER(unit);

    bfd_drv = bcmcth_oam_bfd_drv_get(unit);
    SHR_NULL_CHECK(bfd_drv, SHR_E_INIT);

    fid = field->id;
    fval = field->data;

    switch (fid) {
        case OAM_BFD_TNL_IPV6_ENDPOINTt_ROLEf:
            entry->role = fval;
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINTt_INNER_IP_TYPEf:
            entry->type.tnl_ipv6.inner_ip_type = fval;
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINTt_GREf:
            entry->type.tnl_ipv6.gre = fval;
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINTt_MODEf:
            entry->mode = fval;
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINTt_ECHOf:
            entry->echo = fval;
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINTt_LOCAL_DISCRIMINATORf:
            entry->local_discr = fval;
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINTt_REMOTE_DISCRIMINATORf:
            entry->remote_discr = fval;
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINTt_DIAG_CODEf:
            entry->diag_code = fval;
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINTt_MIN_TX_INTERVAL_USECSf:
            entry->min_tx_interval_usecs = fval;
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINTt_MIN_RX_INTERVAL_USECSf:
            entry->min_rx_interval_usecs = fval;
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINTt_MIN_ECHO_RX_INTERVAL_USECSf:
            entry->min_echo_rx_interval_usecs = fval;
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINTt_DETECT_MULTIPLIERf:
            entry->detect_multiplier = fval;
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINTt_AUTH_TYPEf:
            entry->auth_type = fval;
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINTt_OAM_BFD_AUTH_SIMPLE_PASSWORD_IDf:
            entry->sp.id = fval;
            rv = bcmcth_oam_bfd_auth_sp_entry_get(unit, entry->sp.id,
                                                  &(entry->sp));
            if (rv == SHR_E_NONE) {
                entry->auth_sp_found = true;
            }
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINTt_OAM_BFD_AUTH_SHA1_IDf:
            entry->sha1.id = fval;
            rv = bcmcth_oam_bfd_auth_sha1_entry_get(unit, entry->sha1.id,
                                                    &(entry->sha1));
            if (rv == SHR_E_NONE) {
                entry->auth_sha1_found = true;
            }
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINTt_INITIAL_SHA1_SEQ_NUMf:
            entry->initial_sha1_seq_num = fval;
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINTt_SHA1_SEQ_NUM_INCREMENTf:
            entry->sha1_seq_num_incr = fval;
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINTt_POLL_SEQUENCEf:
            entry->poll_sequence = fval;
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINTt_RX_LOOKUP_VLAN_IDf:
            entry->type.tnl_ipv6.rx_lookup_vlan_id = fval;
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINTt_PORT_IDf:
            entry->port = fval;
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINTt_COSf:
            entry->cos = fval;
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINTt_DST_MACf:
            fval32[0] = (uint32_t)fval;
            fval32[1] = (uint32_t)(fval >> 32);
            BCMCTH_MAC_ADDR_FROM_UINT32(entry->dst_mac, fval32);
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINTt_SRC_MACf:
            fval32[0] = (uint32_t)fval;
            fval32[1] = (uint32_t)(fval >> 32);
            BCMCTH_MAC_ADDR_FROM_UINT32(entry->src_mac, fval32);
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINTt_TAG_TYPEf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bfd_drv->num_vlans_get(fval, &(entry->num_vlans)));
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINTt_VLAN_IDf:
            entry->outer_vlan.tci.vid = fval;
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINTt_VLAN_PRIf:
            entry->outer_vlan.tci.prio = fval;
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINTt_TPIDf:
            entry->outer_vlan.tpid = fval;
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINTt_INNER_VLAN_IDf:
            entry->inner_vlan.tci.vid = fval;
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINTt_INNER_VLAN_PRIf:
            entry->inner_vlan.tci.prio = fval;
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINTt_INNER_TPIDf:
            entry->inner_vlan.tpid = fval;
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINTt_SRC_IPV6u_UPPERf:
            BCMCTH_IPV6_ADDR_UPPER_SET(entry->type.tnl_ipv6.src_ip, fval);
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINTt_SRC_IPV6u_LOWERf:
            BCMCTH_IPV6_ADDR_LOWER_SET(entry->type.tnl_ipv6.src_ip, fval);
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINTt_DST_IPV6u_UPPERf:
            BCMCTH_IPV6_ADDR_UPPER_SET(entry->type.tnl_ipv6.dst_ip, fval);
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINTt_DST_IPV6u_LOWERf:
            BCMCTH_IPV6_ADDR_LOWER_SET(entry->type.tnl_ipv6.dst_ip, fval);
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINTt_TRAFFIC_CLASSf:
            entry->type.tnl_ipv6.traffic_class = fval;
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINTt_HOP_LIMITf:
            entry->type.tnl_ipv6.hop_limit = fval;
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINTt_INNER_SRC_IPV4f:
            entry->type.tnl_ipv6.inner_ipv4.src_ip = fval;
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINTt_INNER_DST_IPV4f:
            entry->type.tnl_ipv6.inner_ipv4.dst_ip = fval;
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINTt_INNER_TOSf:
            entry->type.tnl_ipv6.inner_ipv4.tos = fval;
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINTt_INNER_SRC_IPV6u_UPPERf:
            BCMCTH_IPV6_ADDR_UPPER_SET(entry->type.tnl_ipv6.inner_ipv6.src_ip, fval);
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINTt_INNER_SRC_IPV6u_LOWERf:
            BCMCTH_IPV6_ADDR_LOWER_SET(entry->type.tnl_ipv6.inner_ipv6.src_ip, fval);
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINTt_INNER_DST_IPV6u_UPPERf:
            BCMCTH_IPV6_ADDR_UPPER_SET(entry->type.tnl_ipv6.inner_ipv6.dst_ip, fval);
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINTt_INNER_DST_IPV6u_LOWERf:
            BCMCTH_IPV6_ADDR_LOWER_SET(entry->type.tnl_ipv6.inner_ipv6.dst_ip, fval);
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINTt_INNER_TRAFFIC_CLASSf:
            entry->type.tnl_ipv6.inner_ipv6.traffic_class = fval;
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINTt_UDP_SRC_PORTf:
            entry->udp_src_port = fval;
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINTt_TX_MODEf:
            entry->tx_mode = fval;
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINTt_OPERATIONAL_STATEf:
            entry->oper_state = fval;
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bfd_endpoint_tnl_ipv6_defaults_get(int unit, bcmcth_oam_bfd_endpoint_t *entry)
{
    bcmcth_oam_bfd_drv_t *bfd_drv = NULL;
    bcmlrd_client_field_info_t *fields_info = NULL;
    size_t fld_num;
    size_t actual_fld_num;
    size_t idx;

    SHR_FUNC_ENTER(unit);

    bfd_drv = bcmcth_oam_bfd_drv_get(unit);
    SHR_NULL_CHECK(bfd_drv, SHR_E_INIT);

    sal_memset(entry, 0, sizeof(*entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_oam_bfd_num_data_fields_get(unit, OAM_BFD_TNL_IPV6_ENDPOINTt,
                                            &fld_num));
    SHR_ALLOC(fields_info, sizeof(bcmlrd_client_field_info_t) * fld_num,
              "bcmcthOamBfdEndpointTnlIpv4FldInfo");
    SHR_NULL_CHECK(fields_info, SHR_E_MEMORY);

    SHR_IF_ERR_EXIT
        (bcmlrd_table_fields_def_get(unit, "OAM_BFD_TNL_IPV6_ENDPOINT",
                                     fld_num, fields_info, &actual_fld_num));

    for (idx = 0; idx < actual_fld_num; idx++) {
        switch (fields_info[idx].id) {
            case OAM_BFD_TNL_IPV6_ENDPOINTt_ROLEf:
                entry->role = fields_info[idx].def.u8;
                break;

            case OAM_BFD_TNL_IPV6_ENDPOINTt_INNER_IP_TYPEf:
                entry->type.tnl_ipv6.inner_ip_type = fields_info[idx].def.u8;
                break;

            case OAM_BFD_TNL_IPV6_ENDPOINTt_GREf:
                entry->type.tnl_ipv6.gre = fields_info[idx].def.is_true;
                break;

            case OAM_BFD_TNL_IPV6_ENDPOINTt_MODEf:
                entry->mode = fields_info[idx].def.u8;
                break;

            case OAM_BFD_TNL_IPV6_ENDPOINTt_ECHOf:
                entry->echo = fields_info[idx].def.is_true;
                break;

            case OAM_BFD_TNL_IPV6_ENDPOINTt_DIAG_CODEf:
                entry->diag_code = fields_info[idx].def.u8;
                break;

            case OAM_BFD_TNL_IPV6_ENDPOINTt_MIN_TX_INTERVAL_USECSf:
                entry->min_tx_interval_usecs = fields_info[idx].def.u32;
                break;

            case OAM_BFD_TNL_IPV6_ENDPOINTt_MIN_RX_INTERVAL_USECSf:
                entry->min_rx_interval_usecs = fields_info[idx].def.u32;
                break;

            case OAM_BFD_TNL_IPV6_ENDPOINTt_MIN_ECHO_RX_INTERVAL_USECSf:
                entry->min_echo_rx_interval_usecs = fields_info[idx].def.u32;
                break;

            case OAM_BFD_TNL_IPV6_ENDPOINTt_DETECT_MULTIPLIERf:
                entry->detect_multiplier = fields_info[idx].def.u8;
                break;

            case OAM_BFD_TNL_IPV6_ENDPOINTt_AUTH_TYPEf:
                entry->auth_type = fields_info[idx].def.u8;
                break;

            case OAM_BFD_TNL_IPV6_ENDPOINTt_INITIAL_SHA1_SEQ_NUMf:
                entry->initial_sha1_seq_num = fields_info[idx].def.u32;
                break;

            case OAM_BFD_TNL_IPV6_ENDPOINTt_SHA1_SEQ_NUM_INCREMENTf:
                entry->sha1_seq_num_incr = fields_info[idx].def.u8;
                break;

            case OAM_BFD_TNL_IPV6_ENDPOINTt_POLL_SEQUENCEf:
                entry->poll_sequence = fields_info[idx].def.u8;
                break;

            case OAM_BFD_TNL_IPV6_ENDPOINTt_COSf:
                entry->cos = fields_info[idx].def.u8;
                break;

            case OAM_BFD_TNL_IPV6_ENDPOINTt_TAG_TYPEf:
                SHR_IF_ERR_VERBOSE_EXIT
                    (bfd_drv->num_vlans_get(fields_info[idx].def.u8,
                                            &(entry->num_vlans)));
                break;

            case OAM_BFD_TNL_IPV6_ENDPOINTt_UDP_SRC_PORTf:
                entry->udp_src_port = fields_info[idx].def.u16;
                break;

            case OAM_BFD_TNL_IPV6_ENDPOINTt_OAM_BFD_TNL_IPV6_ENDPOINT_IDf:
            case OAM_BFD_TNL_IPV6_ENDPOINTt_LOCAL_DISCRIMINATORf:
            case OAM_BFD_TNL_IPV6_ENDPOINTt_REMOTE_DISCRIMINATORf:
            case OAM_BFD_TNL_IPV6_ENDPOINTt_OAM_BFD_AUTH_SIMPLE_PASSWORD_IDf:
            case OAM_BFD_TNL_IPV6_ENDPOINTt_OAM_BFD_AUTH_SHA1_IDf:
            case OAM_BFD_TNL_IPV6_ENDPOINTt_RX_LOOKUP_VLAN_IDf:
            case OAM_BFD_TNL_IPV6_ENDPOINTt_PORT_IDf:
            case OAM_BFD_TNL_IPV6_ENDPOINTt_DST_MACf:
            case OAM_BFD_TNL_IPV6_ENDPOINTt_SRC_MACf:
            case OAM_BFD_TNL_IPV6_ENDPOINTt_VLAN_IDf:
            case OAM_BFD_TNL_IPV6_ENDPOINTt_VLAN_PRIf:
            case OAM_BFD_TNL_IPV6_ENDPOINTt_TPIDf:
            case OAM_BFD_TNL_IPV6_ENDPOINTt_INNER_VLAN_IDf:
            case OAM_BFD_TNL_IPV6_ENDPOINTt_INNER_VLAN_PRIf:
            case OAM_BFD_TNL_IPV6_ENDPOINTt_INNER_TPIDf:
            case OAM_BFD_TNL_IPV6_ENDPOINTt_SRC_IPV6u_UPPERf:
            case OAM_BFD_TNL_IPV6_ENDPOINTt_SRC_IPV6u_LOWERf:
            case OAM_BFD_TNL_IPV6_ENDPOINTt_DST_IPV6u_UPPERf:
            case OAM_BFD_TNL_IPV6_ENDPOINTt_DST_IPV6u_LOWERf:
            case OAM_BFD_TNL_IPV6_ENDPOINTt_TRAFFIC_CLASSf:
            case OAM_BFD_TNL_IPV6_ENDPOINTt_HOP_LIMITf:
            case OAM_BFD_TNL_IPV6_ENDPOINTt_INNER_SRC_IPV4f:
            case OAM_BFD_TNL_IPV6_ENDPOINTt_INNER_DST_IPV4f:
            case OAM_BFD_TNL_IPV6_ENDPOINTt_INNER_TOSf:
            case OAM_BFD_TNL_IPV6_ENDPOINTt_INNER_SRC_IPV6u_UPPERf:
            case OAM_BFD_TNL_IPV6_ENDPOINTt_INNER_SRC_IPV6u_LOWERf:
            case OAM_BFD_TNL_IPV6_ENDPOINTt_INNER_DST_IPV6u_UPPERf:
            case OAM_BFD_TNL_IPV6_ENDPOINTt_INNER_DST_IPV6u_LOWERf:
            case OAM_BFD_TNL_IPV6_ENDPOINTt_INNER_TRAFFIC_CLASSf:
            case OAM_BFD_TNL_IPV6_ENDPOINTt_TX_MODEf:
            case OAM_BFD_TNL_IPV6_ENDPOINTt_OPERATIONAL_STATEf:
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

/* Populate the output fields in OAM_BFD_TNL_IPV6_ENDPOINT_STATUS LT. The fields
 * are already sorted on the basis of fid.
 */
static void
bfd_endpoint_tnl_ipv6_status_populate(bcmcth_oam_bfd_endpoint_status_t *entry,
                                      bcmltd_fields_t *output_fields)
{
    uint32_t fid;
    uint64_t *data;
    size_t i;

    for (i = 0; i < output_fields->count; i++) {
        fid = output_fields->field[i]->id;
        data = &(output_fields->field[i]->data);

        switch (fid) {
            case OAM_BFD_TNL_IPV6_ENDPOINT_STATUSt_LOCAL_STATEf:
                *data = entry->local_state;
                break;

            case OAM_BFD_TNL_IPV6_ENDPOINT_STATUSt_REMOTE_STATEf:
                *data = entry->remote_state;
                break;

            case OAM_BFD_TNL_IPV6_ENDPOINT_STATUSt_LOCAL_DIAG_CODEf:
                *data = entry->local_diag_code;
                break;

            case OAM_BFD_TNL_IPV6_ENDPOINT_STATUSt_REMOTE_DIAG_CODEf:
                *data = entry->remote_diag_code;
                break;

            case OAM_BFD_TNL_IPV6_ENDPOINT_STATUSt_LOCAL_AUTH_SEQ_NUMf:
                *data = entry->local_auth_seq_num;
                break;

            case OAM_BFD_TNL_IPV6_ENDPOINT_STATUSt_REMOTE_AUTH_SEQ_NUMf:
                *data = entry->remote_auth_seq_num;
                break;

            case OAM_BFD_TNL_IPV6_ENDPOINT_STATUSt_POLL_SEQUENCE_ACTIVEf:
                *data = entry->poll_sequence_active;
                break;

            case OAM_BFD_TNL_IPV6_ENDPOINT_STATUSt_REMOTE_MODEf:
                *data = entry->remote_mode;
                break;

            case OAM_BFD_TNL_IPV6_ENDPOINT_STATUSt_LOCAL_DISCRIMINATORf:
                *data = entry->local_discr;
                break;

            case OAM_BFD_TNL_IPV6_ENDPOINT_STATUSt_REMOTE_DISCRIMINATORf:
                *data = entry->remote_discr;
                break;

            case OAM_BFD_TNL_IPV6_ENDPOINT_STATUSt_REMOTE_DETECT_MULTIPLIERf:
                *data = entry->remote_detect_multiplier;
                break;

            case OAM_BFD_TNL_IPV6_ENDPOINT_STATUSt_REMOTE_MIN_TX_INTERVAL_USECSf:
                *data = entry->remote_min_tx_interval_usecs;
                break;

            case OAM_BFD_TNL_IPV6_ENDPOINT_STATUSt_REMOTE_MIN_RX_INTERVAL_USECSf:
                *data = entry->remote_min_rx_interval_usecs;
                break;

            case OAM_BFD_TNL_IPV6_ENDPOINT_STATUSt_REMOTE_MIN_ECHO_RX_INTERVAL_USECSf:
                *data = entry->remote_min_echo_rx_interval_usecs;
                break;

            case OAM_BFD_TNL_IPV6_ENDPOINT_STATUSt_ENDPOINT_STATEf:
                *data = entry->endpoint_state;
                break;

            default:
                /* Nothing to update. */
                break;
        }
    }
}

/* Populate the output fields in OAM_BFD_TNL_IPV6_ENDPOINT_STATS LT. The fields
 * are already sorted on the basis of fid.
 */
static void
bfd_endpoint_tnl_ipv6_stats_populate(bcmcth_oam_bfd_endpoint_stats_t *entry,
                                     bcmltd_fields_t *output_fields)
{
    uint32_t fid;
    uint64_t *data;
    size_t i;

    for (i = 0; i < output_fields->count; i++) {
        fid = output_fields->field[i]->id;
        data = &(output_fields->field[i]->data);

        switch (fid) {
            case OAM_BFD_TNL_IPV6_ENDPOINT_STATSt_RX_PKT_CNTf:
                *data = entry->rx_pkt_cnt;
                break;

            case OAM_BFD_TNL_IPV6_ENDPOINT_STATSt_RX_PKT_DISCARD_CNTf:
                *data = entry->rx_pkt_discard_cnt;
                break;

            case OAM_BFD_TNL_IPV6_ENDPOINT_STATSt_RX_PKT_AUTH_FAILURE_DISCARD_CNTf:
                *data = entry->rx_pkt_auth_failure_discard_cnt;
                break;

            case OAM_BFD_TNL_IPV6_ENDPOINT_STATSt_RX_ECHO_REPLY_PKT_CNTf:
                *data = entry->rx_echo_reply_pkt_cnt;
                break;

            case OAM_BFD_TNL_IPV6_ENDPOINT_STATSt_TX_PKT_CNTf:
                *data = entry->tx_pkt_cnt;
                break;

            default:
                /* Nothing to update. */
                break;
        }
    }
}

static int
bfd_endpoint_tnl_ipv6_stats_field_get(int unit, const bcmltd_field_t *field,
                                      bcmcth_oam_bfd_endpoint_stats_t *entry)
{
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    fid = field->id;
    fval = field->data;

    switch (fid) {
        case OAM_BFD_TNL_IPV6_ENDPOINT_STATSt_RX_PKT_CNTf:
            entry->rx_pkt_cnt = fval;
            entry->rx_pkt_cnt_update = true;
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINT_STATSt_RX_PKT_DISCARD_CNTf:
            entry->rx_pkt_discard_cnt = fval;
            entry->rx_pkt_discard_cnt_update = true;
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINT_STATSt_RX_PKT_AUTH_FAILURE_DISCARD_CNTf:
            entry->rx_pkt_auth_failure_discard_cnt = fval;
            entry->rx_pkt_auth_failure_discard_cnt_update = true;
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINT_STATSt_RX_ECHO_REPLY_PKT_CNTf:
            entry->rx_echo_reply_pkt_cnt = fval;
            entry->rx_echo_reply_pkt_cnt_update = true;
            break;

        case OAM_BFD_TNL_IPV6_ENDPOINT_STATSt_TX_PKT_CNTf:
            entry->tx_pkt_cnt = fval;
            entry->tx_pkt_cnt_update = true;
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * BFD Tunnel MPLS endpoint functions.
 */
static int
bfd_endpoint_tnl_mpls_field_get(int unit, const bcmltd_field_t *field,
                                bcmcth_oam_bfd_endpoint_t *entry)
{
    bcmcth_oam_bfd_drv_t *bfd_drv = NULL;
    uint32_t fid;
    uint32_t fval32[2];
    uint32_t idx;
    uint64_t fval;
    int rv;

    SHR_FUNC_ENTER(unit);

    bfd_drv = bcmcth_oam_bfd_drv_get(unit);
    SHR_NULL_CHECK(bfd_drv, SHR_E_INIT);

    fid = field->id;
    idx = field->idx;
    fval = field->data;

    switch (fid) {
        case OAM_BFD_TNL_MPLS_ENDPOINTt_ROLEf:
            entry->role = fval;
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINTt_ENCAP_TYPEf:
            entry->type.tnl_mpls.encap_type = fval;
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINTt_IP_ENCAP_TYPEf:
            entry->type.tnl_mpls.ip_encap_type = fval;
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINTt_MODEf:
            entry->mode = fval;
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINTt_LOCAL_DISCRIMINATORf:
            entry->local_discr = fval;
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINTt_REMOTE_DISCRIMINATORf:
            entry->remote_discr = fval;
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINTt_DIAG_CODEf:
            entry->diag_code = fval;
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINTt_MIN_TX_INTERVAL_USECSf:
            entry->min_tx_interval_usecs = fval;
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINTt_MIN_RX_INTERVAL_USECSf:
            entry->min_rx_interval_usecs = fval;
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINTt_DETECT_MULTIPLIERf:
            entry->detect_multiplier = fval;
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINTt_AUTH_TYPEf:
            entry->auth_type = fval;
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINTt_OAM_BFD_AUTH_SIMPLE_PASSWORD_IDf:
            entry->sp.id = fval;
            rv = bcmcth_oam_bfd_auth_sp_entry_get(unit, entry->sp.id,
                                                  &(entry->sp));
            if (rv == SHR_E_NONE) {
                entry->auth_sp_found = true;
            }
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINTt_OAM_BFD_AUTH_SHA1_IDf:
            entry->sha1.id = fval;
            rv = bcmcth_oam_bfd_auth_sha1_entry_get(unit, entry->sha1.id,
                                                    &(entry->sha1));
            if (rv == SHR_E_NONE) {
                entry->auth_sha1_found = true;
            }
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINTt_INITIAL_SHA1_SEQ_NUMf:
            entry->initial_sha1_seq_num = fval;
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINTt_SHA1_SEQ_NUM_INCREMENTf:
            entry->sha1_seq_num_incr = fval;
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINTt_POLL_SEQUENCEf:
            entry->poll_sequence = fval;
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINTt_RX_LOOKUP_LABELf:
            entry->type.tnl_mpls.rx_lookup_label = fval;
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINTt_PORT_IDf:
            entry->port = fval;
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINTt_COSf:
            entry->cos = fval;
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINTt_DST_MACf:
            fval32[0] = (uint32_t)fval;
            fval32[1] = (uint32_t)(fval >> 32);
            BCMCTH_MAC_ADDR_FROM_UINT32(entry->dst_mac, fval32);
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINTt_SRC_MACf:
            fval32[0] = (uint32_t)fval;
            fval32[1] = (uint32_t)(fval >> 32);
            BCMCTH_MAC_ADDR_FROM_UINT32(entry->src_mac, fval32);
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINTt_TAG_TYPEf:
            SHR_IF_ERR_VERBOSE_EXIT
                (bfd_drv->num_vlans_get(fval, &(entry->num_vlans)));
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINTt_VLAN_IDf:
            entry->outer_vlan.tci.vid = fval;
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINTt_VLAN_PRIf:
            entry->outer_vlan.tci.prio = fval;
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINTt_TPIDf:
            entry->outer_vlan.tpid = fval;
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINTt_INNER_VLAN_IDf:
            entry->inner_vlan.tci.vid = fval;
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINTt_INNER_VLAN_PRIf:
            entry->inner_vlan.tci.prio = fval;
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINTt_INNER_TPIDf:
            entry->inner_vlan.tpid = fval;
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINTt_NUM_LABELSf:
            entry->type.tnl_mpls.num_labels = fval;
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINTt_LABEL_STACKf:
            entry->type.tnl_mpls.label_stack[idx] = fval;
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINTt_PW_ACHf:
            entry->type.tnl_mpls.pw_ach = fval;
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINTt_SRC_IPV4f:
            entry->type.tnl_mpls.ipv4.src_ip = fval;
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINTt_DST_IPV4f:
            entry->type.tnl_mpls.ipv4.dst_ip = fval;
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINTt_TOSf:
            entry->type.tnl_mpls.ipv4.tos = fval;
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINTt_SRC_IPV6u_UPPERf:
            BCMCTH_IPV6_ADDR_UPPER_SET(entry->type.tnl_mpls.ipv6.src_ip, fval);
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINTt_SRC_IPV6u_LOWERf:
            BCMCTH_IPV6_ADDR_LOWER_SET(entry->type.tnl_mpls.ipv6.src_ip, fval);
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINTt_DST_IPV6u_UPPERf:
            BCMCTH_IPV6_ADDR_UPPER_SET(entry->type.tnl_mpls.ipv6.dst_ip, fval);
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINTt_DST_IPV6u_LOWERf:
            BCMCTH_IPV6_ADDR_LOWER_SET(entry->type.tnl_mpls.ipv6.dst_ip, fval);
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINTt_TRAFFIC_CLASSf:
            entry->type.tnl_mpls.ipv6.traffic_class = fval;
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINTt_UDP_SRC_PORTf:
            entry->udp_src_port = fval;
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINTt_SOURCE_MEP_IDENTIFIER_LENGTHf:
            entry->type.tnl_mpls.source_mep_id_len = fval;
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINTt_SOURCE_MEP_IDENTIFIERf:
            entry->type.tnl_mpls.source_mep_id[idx] = fval;
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINTt_REMOTE_MEP_IDENTIFIER_LENGTHf:
            entry->type.tnl_mpls.remote_mep_id_len = fval;
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINTt_REMOTE_MEP_IDENTIFIERf:
            entry->type.tnl_mpls.remote_mep_id[idx] = fval;
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINTt_TX_MODEf:
            entry->tx_mode = fval;
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINTt_OPERATIONAL_STATEf:
            entry->oper_state = fval;
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bfd_endpoint_tnl_mpls_defaults_get(int unit, bcmcth_oam_bfd_endpoint_t *entry)
{
    bcmcth_oam_bfd_drv_t *bfd_drv = NULL;
    bcmlrd_client_field_info_t *fields_info = NULL;
    size_t fld_num;
    size_t actual_fld_num;
    size_t idx;

    SHR_FUNC_ENTER(unit);

    bfd_drv = bcmcth_oam_bfd_drv_get(unit);
    SHR_NULL_CHECK(bfd_drv, SHR_E_INIT);

    sal_memset(entry, 0, sizeof(*entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_oam_bfd_num_data_fields_get(unit, OAM_BFD_TNL_MPLS_ENDPOINTt,
                                            &fld_num));
    SHR_ALLOC(fields_info, sizeof(bcmlrd_client_field_info_t) * fld_num,
              "bcmcthOamBfdEndpointTnlIpv4FldInfo");
    SHR_NULL_CHECK(fields_info, SHR_E_MEMORY);

    SHR_IF_ERR_EXIT
        (bcmlrd_table_fields_def_get(unit, "OAM_BFD_TNL_MPLS_ENDPOINT",
                                     fld_num, fields_info, &actual_fld_num));

    for (idx = 0; idx < actual_fld_num; idx++) {
        switch (fields_info[idx].id) {
            case OAM_BFD_TNL_MPLS_ENDPOINTt_ROLEf:
                entry->role = fields_info[idx].def.u8;
                break;

            case OAM_BFD_TNL_MPLS_ENDPOINTt_ENCAP_TYPEf:
                entry->type.tnl_mpls.encap_type = fields_info[idx].def.u8;
                break;

            case OAM_BFD_TNL_MPLS_ENDPOINTt_IP_ENCAP_TYPEf:
                entry->type.tnl_mpls.ip_encap_type = fields_info[idx].def.u8;
                break;

            case OAM_BFD_TNL_MPLS_ENDPOINTt_MODEf:
                entry->mode = fields_info[idx].def.u8;
                break;

            case OAM_BFD_TNL_MPLS_ENDPOINTt_DIAG_CODEf:
                entry->diag_code = fields_info[idx].def.u8;
                break;

            case OAM_BFD_TNL_MPLS_ENDPOINTt_MIN_TX_INTERVAL_USECSf:
                entry->min_tx_interval_usecs = fields_info[idx].def.u32;
                break;

            case OAM_BFD_TNL_MPLS_ENDPOINTt_MIN_RX_INTERVAL_USECSf:
                entry->min_rx_interval_usecs = fields_info[idx].def.u32;
                break;

            case OAM_BFD_TNL_MPLS_ENDPOINTt_DETECT_MULTIPLIERf:
                entry->detect_multiplier = fields_info[idx].def.u8;
                break;

            case OAM_BFD_TNL_MPLS_ENDPOINTt_AUTH_TYPEf:
                entry->auth_type = fields_info[idx].def.u8;
                break;

            case OAM_BFD_TNL_MPLS_ENDPOINTt_INITIAL_SHA1_SEQ_NUMf:
                entry->initial_sha1_seq_num = fields_info[idx].def.u32;
                break;

            case OAM_BFD_TNL_MPLS_ENDPOINTt_SHA1_SEQ_NUM_INCREMENTf:
                entry->sha1_seq_num_incr = fields_info[idx].def.u8;
                break;

            case OAM_BFD_TNL_MPLS_ENDPOINTt_POLL_SEQUENCEf:
                entry->poll_sequence = fields_info[idx].def.u8;
                break;

            case OAM_BFD_TNL_MPLS_ENDPOINTt_COSf:
                entry->cos = fields_info[idx].def.u8;
                break;

            case OAM_BFD_TNL_MPLS_ENDPOINTt_TAG_TYPEf:
                SHR_IF_ERR_VERBOSE_EXIT
                    (bfd_drv->num_vlans_get(fields_info[idx].def.u8,
                                            &(entry->num_vlans)));
                break;

            case OAM_BFD_TNL_MPLS_ENDPOINTt_NUM_LABELSf:
                entry->type.tnl_mpls.num_labels = fields_info[idx].def.u8;
                break;

            case OAM_BFD_TNL_MPLS_ENDPOINTt_PW_ACHf:
                entry->type.tnl_mpls.pw_ach = fields_info[idx].def.is_true;
                break;

            case OAM_BFD_TNL_MPLS_ENDPOINTt_UDP_SRC_PORTf:
                entry->udp_src_port = fields_info[idx].def.u16;
                break;

            case OAM_BFD_TNL_MPLS_ENDPOINTt_OAM_BFD_TNL_MPLS_ENDPOINT_IDf:
            case OAM_BFD_TNL_MPLS_ENDPOINTt_LOCAL_DISCRIMINATORf:
            case OAM_BFD_TNL_MPLS_ENDPOINTt_REMOTE_DISCRIMINATORf:
            case OAM_BFD_TNL_MPLS_ENDPOINTt_OAM_BFD_AUTH_SIMPLE_PASSWORD_IDf:
            case OAM_BFD_TNL_MPLS_ENDPOINTt_OAM_BFD_AUTH_SHA1_IDf:
            case OAM_BFD_TNL_MPLS_ENDPOINTt_RX_LOOKUP_LABELf:
            case OAM_BFD_TNL_MPLS_ENDPOINTt_PORT_IDf:
            case OAM_BFD_TNL_MPLS_ENDPOINTt_DST_MACf:
            case OAM_BFD_TNL_MPLS_ENDPOINTt_SRC_MACf:
            case OAM_BFD_TNL_MPLS_ENDPOINTt_VLAN_IDf:
            case OAM_BFD_TNL_MPLS_ENDPOINTt_VLAN_PRIf:
            case OAM_BFD_TNL_MPLS_ENDPOINTt_TPIDf:
            case OAM_BFD_TNL_MPLS_ENDPOINTt_INNER_VLAN_IDf:
            case OAM_BFD_TNL_MPLS_ENDPOINTt_INNER_VLAN_PRIf:
            case OAM_BFD_TNL_MPLS_ENDPOINTt_INNER_TPIDf:
            case OAM_BFD_TNL_MPLS_ENDPOINTt_LABEL_STACKf:
            case OAM_BFD_TNL_MPLS_ENDPOINTt_SRC_IPV4f:
            case OAM_BFD_TNL_MPLS_ENDPOINTt_DST_IPV4f:
            case OAM_BFD_TNL_MPLS_ENDPOINTt_TOSf:
            case OAM_BFD_TNL_MPLS_ENDPOINTt_SRC_IPV6u_UPPERf:
            case OAM_BFD_TNL_MPLS_ENDPOINTt_SRC_IPV6u_LOWERf:
            case OAM_BFD_TNL_MPLS_ENDPOINTt_DST_IPV6u_UPPERf:
            case OAM_BFD_TNL_MPLS_ENDPOINTt_DST_IPV6u_LOWERf:
            case OAM_BFD_TNL_MPLS_ENDPOINTt_TRAFFIC_CLASSf:
            case OAM_BFD_TNL_MPLS_ENDPOINTt_SOURCE_MEP_IDENTIFIER_LENGTHf:
            case OAM_BFD_TNL_MPLS_ENDPOINTt_SOURCE_MEP_IDENTIFIERf:
            case OAM_BFD_TNL_MPLS_ENDPOINTt_REMOTE_MEP_IDENTIFIER_LENGTHf:
            case OAM_BFD_TNL_MPLS_ENDPOINTt_REMOTE_MEP_IDENTIFIERf:
            case OAM_BFD_TNL_MPLS_ENDPOINTt_TX_MODEf:
            case OAM_BFD_TNL_MPLS_ENDPOINTt_OPERATIONAL_STATEf:
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

/* Populate the output fields in OAM_BFD_TNL_MPLS_ENDPOINT_STATUS LT. The fields
 * are already sorted on the basis of fid.
 */
static void
bfd_endpoint_tnl_mpls_status_populate(bcmcth_oam_bfd_endpoint_status_t *entry,
                                      bcmltd_fields_t *output_fields)
{
    uint32_t fid;
    uint32_t idx;
    uint64_t *data;
    size_t i;

    for (i = 0; i < output_fields->count; i++) {
        fid = output_fields->field[i]->id;
        idx = output_fields->field[i]->idx;
        data = &(output_fields->field[i]->data);

        switch (fid) {
            case OAM_BFD_TNL_MPLS_ENDPOINT_STATUSt_LOCAL_STATEf:
                *data = entry->local_state;
                break;

            case OAM_BFD_TNL_MPLS_ENDPOINT_STATUSt_REMOTE_STATEf:
                *data = entry->remote_state;
                break;

            case OAM_BFD_TNL_MPLS_ENDPOINT_STATUSt_LOCAL_DIAG_CODEf:
                *data = entry->local_diag_code;
                break;

            case OAM_BFD_TNL_MPLS_ENDPOINT_STATUSt_REMOTE_DIAG_CODEf:
                *data = entry->remote_diag_code;
                break;

            case OAM_BFD_TNL_MPLS_ENDPOINT_STATUSt_LOCAL_AUTH_SEQ_NUMf:
                *data = entry->local_auth_seq_num;
                break;

            case OAM_BFD_TNL_MPLS_ENDPOINT_STATUSt_REMOTE_AUTH_SEQ_NUMf:
                *data = entry->remote_auth_seq_num;
                break;

            case OAM_BFD_TNL_MPLS_ENDPOINT_STATUSt_POLL_SEQUENCE_ACTIVEf:
                *data = entry->poll_sequence_active;
                break;

            case OAM_BFD_TNL_MPLS_ENDPOINT_STATUSt_REMOTE_MODEf:
                *data = entry->remote_mode;
                break;

            case OAM_BFD_TNL_MPLS_ENDPOINT_STATUSt_LOCAL_DISCRIMINATORf:
                *data = entry->local_discr;
                break;

            case OAM_BFD_TNL_MPLS_ENDPOINT_STATUSt_REMOTE_DISCRIMINATORf:
                *data = entry->remote_discr;
                break;

            case OAM_BFD_TNL_MPLS_ENDPOINT_STATUSt_REMOTE_DETECT_MULTIPLIERf:
                *data = entry->remote_detect_multiplier;
                break;

            case OAM_BFD_TNL_MPLS_ENDPOINT_STATUSt_REMOTE_MIN_TX_INTERVAL_USECSf:
                *data = entry->remote_min_tx_interval_usecs;
                break;

            case OAM_BFD_TNL_MPLS_ENDPOINT_STATUSt_REMOTE_MIN_RX_INTERVAL_USECSf:
                *data = entry->remote_min_rx_interval_usecs;
                break;

            case OAM_BFD_TNL_MPLS_ENDPOINT_STATUSt_MISCONNECTIVITY_DEFECT_MEP_IDENTIFIER_LENGTHf:
                *data = entry->misconn_mep_id_len;
                break;

            case OAM_BFD_TNL_MPLS_ENDPOINT_STATUSt_MISCONNECTIVITY_DEFECT_MEP_IDENTIFIERf:
                *data = entry->misconn_mep_id[idx];
                break;

            case OAM_BFD_TNL_MPLS_ENDPOINT_STATUSt_ENDPOINT_STATEf:
                *data = entry->endpoint_state;
                break;

            default:
                /* Nothing to update. */
                break;
        }
    }
}

/* Populate the output fields in OAM_BFD_TNL_MPLS_ENDPOINT_STATS LT. The fields
 * are already sorted on the basis of fid.
 */
static void
bfd_endpoint_tnl_mpls_stats_populate(bcmcth_oam_bfd_endpoint_stats_t *entry,
                                     bcmltd_fields_t *output_fields)
{
    uint32_t fid;
    uint64_t *data;
    size_t i;

    for (i = 0; i < output_fields->count; i++) {
        fid = output_fields->field[i]->id;
        data = &(output_fields->field[i]->data);

        switch (fid) {
            case OAM_BFD_TNL_MPLS_ENDPOINT_STATSt_RX_PKT_CNTf:
                *data = entry->rx_pkt_cnt;
                break;

            case OAM_BFD_TNL_MPLS_ENDPOINT_STATSt_RX_PKT_DISCARD_CNTf:
                *data = entry->rx_pkt_discard_cnt;
                break;

            case OAM_BFD_TNL_MPLS_ENDPOINT_STATSt_RX_PKT_AUTH_FAILURE_DISCARD_CNTf:
                *data = entry->rx_pkt_auth_failure_discard_cnt;
                break;

            case OAM_BFD_TNL_MPLS_ENDPOINT_STATSt_TX_PKT_CNTf:
                *data = entry->tx_pkt_cnt;
                break;

            default:
                /* Nothing to update. */
                break;
        }
    }
}

static int
bfd_endpoint_tnl_mpls_stats_field_get(int unit, const bcmltd_field_t *field,
                                      bcmcth_oam_bfd_endpoint_stats_t *entry)
{
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    fid = field->id;
    fval = field->data;

    switch (fid) {
        case OAM_BFD_TNL_MPLS_ENDPOINT_STATSt_RX_PKT_CNTf:
            entry->rx_pkt_cnt = fval;
            entry->rx_pkt_cnt_update = true;
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINT_STATSt_RX_PKT_DISCARD_CNTf:
            entry->rx_pkt_discard_cnt = fval;
            entry->rx_pkt_discard_cnt_update = true;
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINT_STATSt_RX_PKT_AUTH_FAILURE_DISCARD_CNTf:
            entry->rx_pkt_auth_failure_discard_cnt = fval;
            entry->rx_pkt_auth_failure_discard_cnt_update = true;
            break;

        case OAM_BFD_TNL_MPLS_ENDPOINT_STATSt_TX_PKT_CNTf:
            entry->tx_pkt_cnt = fval;
            entry->tx_pkt_cnt_update = true;
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/* Information and callbacks for various BFD endpoint types. */
typedef struct bfd_endpoint_type_info_s {
    /* Type of the endpoint. */
    bcmltd_common_oam_bfd_endpoint_type_t_t type;

    /* Table ID for the endpoint. */
    bcmltd_sid_t sid;

    /* Key field Id. */
    uint32_t key_fid;

    /* Operational state fid. */
    uint32_t oper_state_fid;

    /* Table ID for the endpoint status. */
    bcmltd_sid_t status_sid;

    /* Status table key field Id. */
    uint32_t status_key_fid;

    /* Table ID for the endpoint stats. */
    bcmltd_sid_t stats_sid;

    /* Stats table key field Id. */
    uint32_t stats_key_fid;

    /* Populate the defaults for the endpoint type. */
    int (*defaults_get)(int unit, bcmcth_oam_bfd_endpoint_t *entry);

    /* Get the value of a field and populate the structure entry. */
    int (*field_get)(int unit, const bcmltd_field_t *field,
                     bcmcth_oam_bfd_endpoint_t *entry);

    /* Populate the endpoint status fields during lookup. */
    void (*status_populate)(bcmcth_oam_bfd_endpoint_status_t *entry,
                            bcmltd_fields_t *output_fields);

    /* Get the value of a stats field and populate the structure entry. */
    int (*stats_field_get)(int unit, const bcmltd_field_t *field,
                           bcmcth_oam_bfd_endpoint_stats_t *entry);

    /* Populate the endpoint stats fields during lookup. */
    void (*stats_populate)(bcmcth_oam_bfd_endpoint_stats_t *entry,
                           bcmltd_fields_t *output_fields);
} bfd_endpoint_type_info_t;

static bfd_endpoint_type_info_t ep_types_info[] = {
    {
        BCMLTD_COMMON_OAM_BFD_ENDPOINT_TYPE_T_T_IPV4,
        OAM_BFD_IPV4_ENDPOINTt,
        OAM_BFD_IPV4_ENDPOINTt_OAM_BFD_IPV4_ENDPOINT_IDf,
        OAM_BFD_IPV4_ENDPOINTt_OPERATIONAL_STATEf,
        OAM_BFD_IPV4_ENDPOINT_STATUSt,
        OAM_BFD_IPV4_ENDPOINT_STATUSt_OAM_BFD_IPV4_ENDPOINT_IDf,
        OAM_BFD_IPV4_ENDPOINT_STATSt,
        OAM_BFD_IPV4_ENDPOINT_STATSt_OAM_BFD_IPV4_ENDPOINT_IDf,
        bfd_endpoint_ipv4_defaults_get,
        bfd_endpoint_ipv4_field_get,
        bfd_endpoint_ipv4_status_populate,
        bfd_endpoint_ipv4_stats_field_get,
        bfd_endpoint_ipv4_stats_populate
    },
    {
        BCMLTD_COMMON_OAM_BFD_ENDPOINT_TYPE_T_T_IPV6,
        OAM_BFD_IPV6_ENDPOINTt,
        OAM_BFD_IPV6_ENDPOINTt_OAM_BFD_IPV6_ENDPOINT_IDf,
        OAM_BFD_IPV6_ENDPOINTt_OPERATIONAL_STATEf,
        OAM_BFD_IPV6_ENDPOINT_STATUSt,
        OAM_BFD_IPV6_ENDPOINT_STATUSt_OAM_BFD_IPV6_ENDPOINT_IDf,
        OAM_BFD_IPV6_ENDPOINT_STATSt,
        OAM_BFD_IPV6_ENDPOINT_STATSt_OAM_BFD_IPV6_ENDPOINT_IDf,
        bfd_endpoint_ipv6_defaults_get,
        bfd_endpoint_ipv6_field_get,
        bfd_endpoint_ipv6_status_populate,
        bfd_endpoint_ipv6_stats_field_get,
        bfd_endpoint_ipv6_stats_populate
    },
    {
        BCMLTD_COMMON_OAM_BFD_ENDPOINT_TYPE_T_T_TNL_IPV4,
        OAM_BFD_TNL_IPV4_ENDPOINTt,
        OAM_BFD_TNL_IPV4_ENDPOINTt_OAM_BFD_TNL_IPV4_ENDPOINT_IDf,
        OAM_BFD_TNL_IPV4_ENDPOINTt_OPERATIONAL_STATEf,
        OAM_BFD_TNL_IPV4_ENDPOINT_STATUSt,
        OAM_BFD_TNL_IPV4_ENDPOINT_STATUSt_OAM_BFD_TNL_IPV4_ENDPOINT_IDf,
        OAM_BFD_TNL_IPV4_ENDPOINT_STATSt,
        OAM_BFD_TNL_IPV4_ENDPOINT_STATSt_OAM_BFD_TNL_IPV4_ENDPOINT_IDf,
        bfd_endpoint_tnl_ipv4_defaults_get,
        bfd_endpoint_tnl_ipv4_field_get,
        bfd_endpoint_tnl_ipv4_status_populate,
        bfd_endpoint_tnl_ipv4_stats_field_get,
        bfd_endpoint_tnl_ipv4_stats_populate
    },
    {
        BCMLTD_COMMON_OAM_BFD_ENDPOINT_TYPE_T_T_TNL_IPV6,
        OAM_BFD_TNL_IPV6_ENDPOINTt,
        OAM_BFD_TNL_IPV6_ENDPOINTt_OAM_BFD_TNL_IPV6_ENDPOINT_IDf,
        OAM_BFD_TNL_IPV6_ENDPOINTt_OPERATIONAL_STATEf,
        OAM_BFD_TNL_IPV6_ENDPOINT_STATUSt,
        OAM_BFD_TNL_IPV6_ENDPOINT_STATUSt_OAM_BFD_TNL_IPV6_ENDPOINT_IDf,
        OAM_BFD_TNL_IPV6_ENDPOINT_STATSt,
        OAM_BFD_TNL_IPV6_ENDPOINT_STATSt_OAM_BFD_TNL_IPV6_ENDPOINT_IDf,
        bfd_endpoint_tnl_ipv6_defaults_get,
        bfd_endpoint_tnl_ipv6_field_get,
        bfd_endpoint_tnl_ipv6_status_populate,
        bfd_endpoint_tnl_ipv6_stats_field_get,
        bfd_endpoint_tnl_ipv6_stats_populate
    },
    {
        BCMLTD_COMMON_OAM_BFD_ENDPOINT_TYPE_T_T_TNL_MPLS,
        OAM_BFD_TNL_MPLS_ENDPOINTt,
        OAM_BFD_TNL_MPLS_ENDPOINTt_OAM_BFD_TNL_MPLS_ENDPOINT_IDf,
        OAM_BFD_TNL_MPLS_ENDPOINTt_OPERATIONAL_STATEf,
        OAM_BFD_TNL_MPLS_ENDPOINT_STATUSt,
        OAM_BFD_TNL_MPLS_ENDPOINT_STATUSt_OAM_BFD_TNL_MPLS_ENDPOINT_IDf,
        OAM_BFD_TNL_MPLS_ENDPOINT_STATSt,
        OAM_BFD_TNL_MPLS_ENDPOINT_STATSt_OAM_BFD_TNL_MPLS_ENDPOINT_IDf,
        bfd_endpoint_tnl_mpls_defaults_get,
        bfd_endpoint_tnl_mpls_field_get,
        bfd_endpoint_tnl_mpls_status_populate,
        bfd_endpoint_tnl_mpls_stats_field_get,
        bfd_endpoint_tnl_mpls_stats_populate
    },

};

/*
 * Convert a SID to ep_type_info, the SID can be the config, status or stats
 * SID.
 */
static bfd_endpoint_type_info_t *
bfd_endpoint_type_info_get(bcmltd_sid_t sid)
{
    int i, num_types;

    num_types = sizeof(ep_types_info) / sizeof(bfd_endpoint_type_info_t);

    for (i = 0; i < num_types; i++) {
        if ((ep_types_info[i].sid        == sid) ||
            (ep_types_info[i].status_sid == sid) ||
            (ep_types_info[i].stats_sid  == sid)) {
            return &(ep_types_info[i]);
        }
    }
    return NULL;
}

/*******************************************************************************
 * BFD endpoint prepopulation functions.
 */
static int
bfd_endpoint_status_prepopulate(int unit, bcmltd_sid_t sid)
{
    bfd_endpoint_type_info_t *ep_type_info;
    bcmltd_fields_t fields = {0};
    uint32_t max_endpoints;
    uint32_t idx;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_map_table_attr_get(unit, sid,
                                   BCMLRD_MAP_TABLE_ATTRIBUTE_ENTRY_LIMIT,
                                   &max_endpoints));

    ep_type_info = bfd_endpoint_type_info_get(sid);
    SHR_NULL_CHECK(ep_type_info, SHR_E_NOT_FOUND);

    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_oam_bfd_fields_alloc(unit, 1, &fields));

    for (idx = 0; idx < max_endpoints; idx++) {
        fields.field[0]->id = ep_type_info->status_key_fid;
        fields.field[0]->data = idx;
        SHR_IF_ERR_VERBOSE_EXIT(bcmimm_entry_insert(unit, sid, &fields));
    }

 exit:
    bcmcth_oam_bfd_fields_free(unit, 1, &fields);
    SHR_FUNC_EXIT();
}

static int
bfd_endpoint_stats_prepopulate(int unit, bcmltd_sid_t sid)
{
    bfd_endpoint_type_info_t *ep_type_info;
    bcmltd_fields_t fields = {0};
    uint32_t max_endpoints;
    uint32_t idx;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_map_table_attr_get(unit, sid,
                                   BCMLRD_MAP_TABLE_ATTRIBUTE_ENTRY_LIMIT,
                                   &max_endpoints));

    ep_type_info = bfd_endpoint_type_info_get(sid);
    SHR_NULL_CHECK(ep_type_info, SHR_E_NOT_FOUND);

    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_oam_bfd_fields_alloc(unit, 1, &fields));

    for (idx = 0; idx < max_endpoints; idx++) {
        fields.field[0]->id = ep_type_info->stats_key_fid;
        fields.field[0]->data = idx;
        SHR_IF_ERR_VERBOSE_EXIT(bcmimm_entry_insert(unit, sid, &fields));
    }

 exit:
    bcmcth_oam_bfd_fields_free(unit, 1, &fields);
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * IMM event handlers.
 */

/* Get the OAM_BFD_XXX_ENDPOINT entry currently in IMM. */
static int
bfd_endpoint_entry_get(int unit, bcmltd_sid_t sid, uint32_t endpoint_id,
                       bcmcth_oam_bfd_endpoint_t *entry)
{
    bcmltd_fields_t in = {0};
    bcmltd_fields_t out = {0};
    const bcmltd_field_t *field;
    bfd_endpoint_type_info_t *ep_type_info;
    size_t num_fields;
    size_t idx;

    SHR_FUNC_ENTER(unit);

    ep_type_info = bfd_endpoint_type_info_get(sid);
    SHR_NULL_CHECK(ep_type_info, SHR_E_NOT_FOUND);

    /* Allocate memory for key. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_oam_bfd_fields_alloc(unit, 1, &in));
    in.field[0]->id = ep_type_info->key_fid;
    in.field[0]->data = endpoint_id;

    /* Allocate memory for data. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_oam_bfd_num_data_fields_get(unit, sid, &num_fields));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_oam_bfd_fields_alloc(unit, num_fields, &out));

    /* Lookup IMM for the entry. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmimm_entry_lookup(unit, sid, &in, &out));

    sal_memset(entry, 0, sizeof(*entry));

    /* Get the defaults for all the fields. Some of these may get overwritten
     * with the values from IMM.
     */
    SHR_IF_ERR_VERBOSE_EXIT(ep_type_info->defaults_get(unit, entry));

    /* Populate the data fields. */
    for (idx = 0; idx < out.count; idx++) {
        field = out.field[idx];
        SHR_IF_ERR_VERBOSE_EXIT(ep_type_info->field_get(unit, field, entry));
    }

    /* Populate the key fields. */
    entry->ep_type = ep_type_info->type;
    entry->endpoint_id = endpoint_id;

exit:
    bcmcth_oam_bfd_fields_free(unit, 1, &in);
    bcmcth_oam_bfd_fields_free(unit, num_fields, &out);
    SHR_FUNC_EXIT();
}

/*
 * Convert the user passed fields in the stage callback handler to a structure.
 */
static int
bfd_endpoint_stage_fields_parse(int unit,
                                bfd_endpoint_type_info_t *ep_type_info,
                                bcmimm_entry_event_t event,
                                const bcmltd_field_t *key,
                                const bcmltd_field_t *data,
                                bcmcth_oam_bfd_endpoint_t *entry)
{
    const bcmltd_field_t *field;

    SHR_FUNC_ENTER(unit);

    if ((event != BCMIMM_ENTRY_INSERT) && (event != BCMIMM_ENTRY_UPDATE)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (event == BCMIMM_ENTRY_INSERT) {
        /* Get the defaults for all the fields. Some of these may get
         * overwritten with the values from user.
         */
        SHR_IF_ERR_VERBOSE_EXIT(ep_type_info->defaults_get(unit, entry));
    } else {
        /* Get the current view of IMM if it is an update operation. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bfd_endpoint_entry_get(unit, ep_type_info->sid, key->data, entry));
    }


    /* Populate the data fields. */
    field = data;
    while (field != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(ep_type_info->field_get(unit, field, entry));
        field = field->next;
    }

    /* Populate the key fields. */
    entry->ep_type = ep_type_info->type;
    entry->endpoint_id = key->data;

exit:
    SHR_FUNC_EXIT();
}

static int
bfd_endpoint_stage(int unit,
                   bcmltd_sid_t sid,
                   uint32_t trans_id,
                   bcmimm_entry_event_t event,
                   const bcmltd_field_t *key,
                   const bcmltd_field_t *data,
                   void *context,
                   bcmltd_fields_t *output_fields)
{
    bcmcth_oam_bfd_control_t ctrl;
    bcmcth_oam_bfd_endpoint_t old_entry, new_entry;
    bcmltd_common_oam_bfd_endpoint_state_t_t oper_state;
    bfd_endpoint_type_info_t *ep_type_info;

    SHR_FUNC_ENTER(unit);

    ep_type_info = bfd_endpoint_type_info_get(sid);
    SHR_NULL_CHECK(ep_type_info, SHR_E_NOT_FOUND);

    /*
     * Get the OAM_BFD_CONTROL entry, if not found fill in the default values.
     */
    if (bcmcth_oam_bfd_control_entry_get(unit, &ctrl) != SHR_E_NONE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_oam_bfd_control_defaults_get(unit, &ctrl));
    }

    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT
                (bfd_endpoint_stage_fields_parse(unit, ep_type_info,
                                                 BCMIMM_ENTRY_INSERT,
                                                 key, data, &new_entry));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_oam_bfd_endpoint_insert(unit, &ctrl, &new_entry,
                                                &oper_state));
            break;

        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bfd_endpoint_entry_get(unit, ep_type_info->sid, key->data,
                                        &old_entry));

            SHR_IF_ERR_VERBOSE_EXIT
                (bfd_endpoint_stage_fields_parse(unit, ep_type_info,
                                                 BCMIMM_ENTRY_UPDATE,
                                                 key, data, &new_entry));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_oam_bfd_endpoint_update(unit, &ctrl,
                                                &new_entry, &old_entry,
                                                &oper_state));
            break;

        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_oam_bfd_endpoint_delete(unit,
                                                ep_type_info->type, key->data));
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((event == BCMIMM_ENTRY_INSERT) || (event == BCMIMM_ENTRY_UPDATE)) {
        output_fields->field[0]->id = ep_type_info->oper_state_fid;
        output_fields->field[0]->idx = 0;
        output_fields->field[0]->data = oper_state;
        output_fields->count = 1;
    }

exit:
    SHR_FUNC_EXIT();
}

/* OAM_BFD_XXX_ENDPOINT IMM event callback structure. */
static bcmimm_lt_cb_t bfd_endpoint_hdl = {
    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = bfd_endpoint_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL,

    /*! Lookup function */
    .lookup = NULL
};

static int
bfd_endpoint_status_lookup(int unit,
                           bcmltd_sid_t sid,
                           uint32_t trans_id,
                           void *context,
                           bcmimm_lookup_type_t lkup_type,
                           const bcmltd_fields_t *in,
                           bcmltd_fields_t *out)
{
    bcmcth_oam_bfd_endpoint_status_t entry;
    bfd_endpoint_type_info_t *ep_type_info;
    const bcmltd_fields_t *key_fields = NULL;
    uint32_t endpoint_id = 0;
    size_t i;

    SHR_FUNC_ENTER(unit);

    ep_type_info = bfd_endpoint_type_info_get(sid);
    SHR_NULL_CHECK(ep_type_info, SHR_E_NOT_FOUND);

    /* key fields are in the "out" parameter for traverse operations */
    key_fields = (lkup_type == BCMIMM_TRAVERSE) ? out : in;

    /* Parse the key fields to get the endpoint id */
    for (i = 0; i < key_fields->count; i++) {
        if (key_fields->field[i]->id == ep_type_info->status_key_fid) {
            endpoint_id = key_fields->field[i]->data;
            break;
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_oam_bfd_endpoint_status_lookup(unit, ep_type_info->type,
                                               endpoint_id, &entry));
    ep_type_info->status_populate(&entry, out);


exit:
    SHR_FUNC_EXIT();
}

/* OAM_BFD_XXX_ENDPOINT_STATUS IMM event callback structure. */
static bcmimm_lt_cb_t bfd_endpoint_status_hdl = {
    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = NULL,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL,

    /*! Lookup function */
    .lookup = bfd_endpoint_status_lookup
};

static int
bfd_endpoint_stats_stage_fields_parse(int unit,
                                      bfd_endpoint_type_info_t *ep_type_info,
                                      const bcmltd_field_t *data,
                                      bcmcth_oam_bfd_endpoint_stats_t *entry)
{
    const bcmltd_field_t *field;

    SHR_FUNC_ENTER(unit);

    field = data;
    while (field != NULL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ep_type_info->stats_field_get(unit, field, entry));
        field = field->next;
    }

exit:
    SHR_FUNC_EXIT();
}


static int
bfd_endpoint_stats_stage(int unit,
                         bcmltd_sid_t sid,
                         uint32_t trans_id,
                         bcmimm_entry_event_t event,
                         const bcmltd_field_t *key,
                         const bcmltd_field_t *data,
                         void *context,
                         bcmltd_fields_t *output_fields)
{
    bcmcth_oam_bfd_endpoint_stats_t entry;
    bfd_endpoint_type_info_t *ep_type_info;

    SHR_FUNC_ENTER(unit);

    if (event != BCMIMM_ENTRY_UPDATE) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (output_fields) {
        output_fields->count = 0;
    }

    ep_type_info = bfd_endpoint_type_info_get(sid);
    SHR_NULL_CHECK(ep_type_info, SHR_E_NOT_FOUND);

    SHR_IF_ERR_VERBOSE_EXIT
        (bfd_endpoint_stats_stage_fields_parse(unit, ep_type_info,
                                               data, &entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_oam_bfd_endpoint_stats_update(unit, ep_type_info->type,
                                              key->data, &entry));
exit:
    SHR_FUNC_EXIT();
}

static int
bfd_endpoint_stats_lookup(int unit,
                          bcmltd_sid_t sid,
                          uint32_t trans_id,
                          void *context,
                          bcmimm_lookup_type_t lkup_type,
                          const bcmltd_fields_t *in,
                          bcmltd_fields_t *out)
{
    bcmcth_oam_bfd_endpoint_stats_t entry;
    bfd_endpoint_type_info_t *ep_type_info;
    const bcmltd_fields_t *key_fields = NULL;
    uint32_t endpoint_id = 0;
    size_t i;

    SHR_FUNC_ENTER(unit);

    ep_type_info = bfd_endpoint_type_info_get(sid);
    SHR_NULL_CHECK(ep_type_info, SHR_E_NOT_FOUND);

    /* key fields are in the "out" parameter for traverse operations */
    key_fields = (lkup_type == BCMIMM_TRAVERSE) ? out : in;

    /* Parse the key fields to get the endpoint id */
    for (i = 0; i < key_fields->count; i++) {
        if (key_fields->field[i]->id == ep_type_info->stats_key_fid) {
            endpoint_id = key_fields->field[i]->data;
            break;
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_oam_bfd_endpoint_stats_lookup(unit, ep_type_info->type,
                                              endpoint_id, &entry));

    ep_type_info->stats_populate(&entry, out);

exit:
    SHR_FUNC_EXIT();
}

/* OAM_BFD_XXX_ENDPOINT_STATS IMM event callback structure. */
static bcmimm_lt_cb_t bfd_endpoint_stats_hdl = {
    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = bfd_endpoint_stats_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL,

    /*! Lookup function */
    .lookup = bfd_endpoint_stats_lookup
};

/*******************************************************************************
 * Public Functions
 */
int
bcmcth_oam_bfd_endpoint_imm_register(int unit, bool warm)
{
    bcmcth_oam_bfd_imm_reg_t imm_reg[] = {
        {
            OAM_BFD_IPV4_ENDPOINTt,
            NULL,
            &bfd_endpoint_hdl
        },
        {
            OAM_BFD_IPV4_ENDPOINT_STATUSt,
            bfd_endpoint_status_prepopulate,
            &bfd_endpoint_status_hdl
        },
        {
            OAM_BFD_IPV4_ENDPOINT_STATSt,
            bfd_endpoint_stats_prepopulate,
            &bfd_endpoint_stats_hdl
        },
        {
            OAM_BFD_IPV6_ENDPOINTt,
            NULL,
            &bfd_endpoint_hdl
        },
        {
            OAM_BFD_IPV6_ENDPOINT_STATUSt,
            bfd_endpoint_status_prepopulate,
            &bfd_endpoint_status_hdl
        },
        {
            OAM_BFD_IPV6_ENDPOINT_STATSt,
            bfd_endpoint_stats_prepopulate,
            &bfd_endpoint_stats_hdl
        },
        {
            OAM_BFD_TNL_IPV4_ENDPOINTt,
            NULL,
            &bfd_endpoint_hdl
        },
        {
            OAM_BFD_TNL_IPV4_ENDPOINT_STATUSt,
            bfd_endpoint_status_prepopulate,
            &bfd_endpoint_status_hdl
        },
        {
            OAM_BFD_TNL_IPV4_ENDPOINT_STATSt,
            bfd_endpoint_stats_prepopulate,
            &bfd_endpoint_stats_hdl
        },
        {
            OAM_BFD_TNL_IPV6_ENDPOINTt,
            NULL,
            &bfd_endpoint_hdl
        },
        {
            OAM_BFD_TNL_IPV6_ENDPOINT_STATUSt,
            bfd_endpoint_status_prepopulate,
            &bfd_endpoint_status_hdl
        },
        {
            OAM_BFD_TNL_IPV6_ENDPOINT_STATSt,
            bfd_endpoint_stats_prepopulate,
            &bfd_endpoint_stats_hdl
        },
        {
            OAM_BFD_TNL_MPLS_ENDPOINTt,
            NULL,
            &bfd_endpoint_hdl
        },
        {
            OAM_BFD_TNL_MPLS_ENDPOINT_STATUSt,
            bfd_endpoint_status_prepopulate,
            &bfd_endpoint_status_hdl
        },
        {
            OAM_BFD_TNL_MPLS_ENDPOINT_STATSt,
            bfd_endpoint_stats_prepopulate,
            &bfd_endpoint_stats_hdl
        }
    };

    int num_sids;

    SHR_FUNC_ENTER(unit);

    num_sids = sizeof(imm_reg) / sizeof(bcmcth_oam_bfd_imm_reg_t);

    /* Register the IMM callbacks. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_oam_bfd_imm_cb_register(unit, warm, num_sids, imm_reg));

 exit:
    SHR_FUNC_EXIT();
}
