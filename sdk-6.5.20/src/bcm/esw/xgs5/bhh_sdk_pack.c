/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * File:        bhh_sdk_pack.c
 * Purpose:     BHH pack and unpack routines for:
 *              - BHH Control messages
 */
#if defined(INCLUDE_BHH)

#include <bcm_int/esw/bhh_sdk_pack.h>
#include <shared/pack.h>
#include <soc/feature.h>
#include <soc/drv.h>

uint8 *
bhh_sdk_version_exchange_msg_unpack(uint8 *buf, bhh_sdk_version_exchange_msg_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->version);
    return buf;
}

uint8 *
bhh_sdk_version_exchange_msg_pack(uint8 *buf, bhh_sdk_version_exchange_msg_t *msg)
{
    _SHR_PACK_U32(buf, msg->version);
    return buf;
}

uint8 *
bhh_sdk_msg_ctrl_sess_get_unpack(uint8 *buf,
                                 bhh_sdk_msg_ctrl_sess_get_t *msg)
{ 
    int i;

    _SHR_UNPACK_U32(buf, msg->sess_id);
    _SHR_UNPACK_U8(buf, msg->enable);
    _SHR_UNPACK_U8(buf, msg->passive);
    _SHR_UNPACK_U8(buf, msg->local_demand);
    _SHR_UNPACK_U8(buf, msg->remote_demand);
    _SHR_UNPACK_U8(buf, msg->local_sess_state);
    _SHR_UNPACK_U8(buf, msg->remote_sess_state);
    _SHR_UNPACK_U8(buf, msg->mel);
    _SHR_UNPACK_U16(buf, msg->mep_id);
    for (i = 0; i < SHR_BHH_MEG_ID_LENGTH; i++) {
        _SHR_UNPACK_U8(buf, msg->meg_id[i]);
    }
    _SHR_UNPACK_U32(buf, msg->local_period);
    _SHR_UNPACK_U8(buf, msg->encap_type);
    _SHR_UNPACK_U32(buf, msg->encap_length);

    if(BHH_UC_FEATURE_CHECK(BHH_OLP_ENCAP)) {
        for (i = 0; i < msg->encap_length; i++) {
            _SHR_UNPACK_U8(buf, msg->encap_data[i]);
        }
    } else {
        for (i = 0; i < SHR_BHH_MAX_ENCAP_LENGTH; i++) {
            _SHR_UNPACK_U8(buf, msg->encap_data[i]);
        }
    }

    _SHR_UNPACK_U32(buf, msg->tx_port);
    _SHR_UNPACK_U32(buf, msg->tx_cos);
    _SHR_UNPACK_U32(buf, msg->tx_pri);
    _SHR_UNPACK_U32(buf, msg->tx_qnum);
    if(BHH_UC_FEATURE_CHECK(BHH_WB_SESS_GET)) {
        _SHR_UNPACK_U32(buf, msg->mpls_label);
        _SHR_UNPACK_U32(buf, msg->if_num);
        _SHR_UNPACK_U32(buf, msg->flags);
    }
    if(BHH_UC_FEATURE_CHECK(BHH_REMOTE_EP_NAME)) {
        _SHR_UNPACK_U16(buf, msg->remote_mep_id);
    }
    if(BHH_UC_FEATURE_CHECK(BHH_FAULTS_GET)) {
       _SHR_UNPACK_U32(buf, msg->fault_flags);
    }

    if(BHH_UC_FEATURE_CHECK(BHH_PASSIVE_MEP)) {
       _SHR_UNPACK_U32(buf, msg->remote_period);
    }
    if (BHH_UC_FEATURE_CHECK(BHH_PRIORITY_EVENT)) {
        _SHR_UNPACK_U8(buf, msg->priority);
    }

    return buf;
}

uint8 *
bhh_sdk_msg_ctrl_loss_get_unpack(uint8 *buf,
                                 bhh_sdk_msg_ctrl_loss_get_t *msg)
{
    int i;

    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U32(buf, msg->sess_id);
    _SHR_UNPACK_U32(buf, msg->period);
    _SHR_UNPACK_U32(buf, msg->loss_threshold);
    _SHR_UNPACK_U32(buf, msg->loss_nearend);
    _SHR_UNPACK_U32(buf, msg->tx_nearend);
    _SHR_UNPACK_U32(buf, msg->rx_nearend);
    _SHR_UNPACK_U32(buf, msg->tx_farend);
    _SHR_UNPACK_U32(buf, msg->rx_farend);
    _SHR_UNPACK_U32(buf, msg->rx_oam_packets);
    _SHR_UNPACK_U32(buf, msg->tx_oam_packets);
    _SHR_UNPACK_U32(buf, msg->int_pri);
    _SHR_UNPACK_U8 (buf, msg->pkt_pri);

    if(BHH_UC_FEATURE_CHECK(BHH_LOSS_GET)) {
        _SHR_UNPACK_U32(buf, msg->loss_farend);
    }

    if(BHH_UC_FEATURE_CHECK(BHH_OLP_ENCAP)) {
        _SHR_UNPACK_U16(buf, msg->encap_length);
        for (i = 0; i < msg->encap_length; i++) {
            _SHR_UNPACK_U8(buf, msg->encap_data[i]);
        }
    }

    return buf;
}

uint8 *
bhh_sdk_msg_ctrl_init_pack(uint8 unit, uint8 *buf, bhh_sdk_msg_ctrl_init_t *msg)
{
    int i;

    _SHR_PACK_U32(buf, msg->num_sessions);
    _SHR_PACK_U32(buf, msg->rx_channel);
    _SHR_PACK_U32(buf, msg->node_id);

    for (i = 0;i < SHR_BHH_CARRIER_CODE_LEN;i++) {
        _SHR_PACK_U8(buf, msg->carrier_code[i]);
    }

    if (soc_feature(unit, soc_feature_oam_pm))  {
        if ((SOC_IS_SABER2(unit)) || (SOC_IS_APACHE(unit))) {
            if(BHH_UC_FEATURE_CHECK(BHH_PM_MODE)) {
                _SHR_PACK_U32(buf, msg->data_collection_mode);
            }
        }
    }

    if(BHH_UC_FEATURE_CHECK(BHH_OLP_ENCAP)) {
        _SHR_PACK_U16(buf, msg->max_encap_length);
    }

    if(BHH_UC_FEATURE_CHECK(BHH_OLP_ENCAP_VALID)) {
        _SHR_PACK_U32(buf, msg->flags);
    }

    if(BHH_UC_FEATURE_CHECK(BHH_KT2_PM_SUPPORT)) {
        _SHR_PACK_U32(buf, msg->data_collection_mode);
    }
    
    return buf;
}

uint8 *
bhh_sdk_msg_ctrl_rmep_create_pack(uint8 *buf, bhh_sdk_msg_ctrl_rmep_create_t *msg)
{
    _SHR_PACK_U32(buf, msg->sess_id);
    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U16(buf, msg->remote_mep_id);
    _SHR_PACK_U8(buf, msg->enable);

    if(BHH_UC_FEATURE_CHECK(BHH_PASSIVE_MEP)) {
        _SHR_PACK_U32(buf, msg->period);
    }

    return buf;
}

uint8 *
bhh_sdk_msg_ctrl_loss_add_pack(uint8 *buf, bhh_sdk_msg_ctrl_loss_add_t *msg)
{
    int i;

    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U32(buf, msg->sess_id);
    _SHR_PACK_U32(buf, msg->period);
    _SHR_PACK_U32(buf, msg->int_pri);
    _SHR_PACK_U8 (buf, msg->pkt_pri);

    if(BHH_UC_FEATURE_CHECK(BHH_OLP_ENCAP)) {
        _SHR_PACK_U16(buf, msg->encap_length);
        for (i = 0; i < msg->encap_length; i++) {
            _SHR_PACK_U8(buf, msg->encap_data[i]);
        }
    }

    return buf;
}

uint8 *
bhh_sdk_msg_ctrl_delay_add_pack(uint8 *buf, bhh_sdk_msg_ctrl_delay_add_t *msg)
{
    int i;

    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U32(buf, msg->sess_id);
    _SHR_PACK_U32(buf, msg->period);
    _SHR_PACK_U32(buf, msg->int_pri);
    _SHR_PACK_U8 (buf, msg->pkt_pri);
    _SHR_PACK_U8 (buf, msg->dm_format);

    if(BHH_UC_FEATURE_CHECK(BHH_OLP_ENCAP)) {
        _SHR_PACK_U16(buf, msg->encap_length);
        for (i = 0; i < msg->encap_length; i++) {
            _SHR_PACK_U8(buf, msg->encap_data[i]);
        }
    }
    if (BHH_UC_FEATURE_CHECK(BHH_DM_DATA_TLV)) {
        _SHR_PACK_U16(buf, msg->data_tlv_len);
    }
    return buf;
}

uint8 *
bhh_sdk_msg_ctrl_loopback_add_pack(uint8 *buf,
                                   bhh_sdk_msg_ctrl_loopback_add_t *msg)
{
    int i;

    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U32(buf, msg->sess_id);
    _SHR_PACK_U32(buf, msg->period);
    _SHR_PACK_U32(buf, msg->ttl);

    if(BHH_UC_FEATURE_CHECK(BHH_OLP_ENCAP)) {
        _SHR_PACK_U16(buf, msg->encap_length);
        for (i = 0; i < msg->encap_length; i++) {
            _SHR_PACK_U8(buf, msg->encap_data[i]);
        }
    }

    if (BHH_UC_FEATURE_CHECK(BHH_PRIORITY_EVENT)) {
        _SHR_PACK_U32(buf, msg->int_pri);
        _SHR_PACK_U8 (buf, msg->pkt_pri);
    }

    return buf;
}

uint8 *
bhh_sdk_msg_ctrl_sess_set_pack(uint8 *buf,
                               bhh_sdk_msg_ctrl_sess_set_t *msg)
{
    uint16 i;

    _SHR_PACK_U32(buf, msg->sess_id);
    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U8(buf, msg->passive);
    _SHR_PACK_U8(buf, msg->mel);
    _SHR_PACK_U16(buf, msg->mep_id);
    for (i = 0; i < SHR_BHH_MEG_ID_LENGTH; i++) {
        _SHR_PACK_U8(buf, msg->meg_id[i]);
    }
    _SHR_PACK_U32(buf, msg->period);
    _SHR_PACK_U8(buf, msg->encap_type);
    _SHR_PACK_U32(buf, msg->encap_length);

    if(BHH_UC_FEATURE_CHECK(BHH_OLP_ENCAP)) {
        for (i = 0; i < msg->encap_length; i++) {
            _SHR_PACK_U8(buf, msg->encap_data[i]);
        }
    } else {
        for (i = 0; i < SHR_BHH_MAX_ENCAP_LENGTH; i++) {
            _SHR_PACK_U8(buf, msg->encap_data[i]);
        }
    }

    _SHR_PACK_U32(buf, msg->tx_port);
    _SHR_PACK_U32(buf, msg->tx_cos);
    _SHR_PACK_U32(buf, msg->tx_pri);
    _SHR_PACK_U32(buf, msg->tx_qnum);
    _SHR_PACK_U32(buf, msg->mpls_label);
    _SHR_PACK_U32(buf, msg->if_num);
    _SHR_PACK_U32(buf, msg->lm_counter_index);

    if(BHH_UC_FEATURE_CHECK(BHH_OLP_ENCAP)) {
        _SHR_PACK_U16(buf, msg->inner_vlan);
        _SHR_PACK_U16(buf, msg->outer_vlan);
        _SHR_PACK_U8(buf, msg->endpoint_type);
        _SHR_PACK_U8(buf, msg->num_session_entries);
    }
    if (BHH_UC_FEATURE_CHECK(BHH_PRIORITY_EVENT)) {
        _SHR_PACK_U8(buf, msg->priority);
    }

    if (BHH_UC_FEATURE_CHECK(BHH_TRUNK_SUPPORT)) {
        _SHR_PACK_U16(buf, msg->rx_port);
        _SHR_PACK_U8(buf, msg->trunk_valid);
    }

    return buf;
}

uint8 *
bhh_sdk_msg_ctrl_session_update_pack(uint8 *buf, bhh_sdk_msg_session_update_t *msg)
{
    int i;

    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U32(buf, msg->sess_id);
    _SHR_PACK_U8(buf, msg->msg_type);
    _SHR_PACK_U8(buf, msg->rdi);

    _SHR_PACK_U16(buf, msg->encap_length);
    for (i = 0; i < msg->encap_length; i++) {
        _SHR_PACK_U8(buf, msg->encap_data[i]);
    }

    return buf;
}

uint8 *
bhh_sdk_msg_ctrl_delay_get_unpack(uint8 *buf, bhh_sdk_msg_ctrl_delay_get_t *msg)
{
    int i;

    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U32(buf, msg->sess_id);
    _SHR_UNPACK_U32(buf, msg->period);
    _SHR_UNPACK_U32(buf, msg->delay_seconds);
    _SHR_UNPACK_U32(buf, msg->delay_nanoseconds);
    _SHR_UNPACK_U32(buf, msg->txf_seconds);
    _SHR_UNPACK_U32(buf, msg->txf_nanoseconds);
    _SHR_UNPACK_U32(buf, msg->rxf_seconds);
    _SHR_UNPACK_U32(buf, msg->rxf_nanoseconds);
    _SHR_UNPACK_U32(buf, msg->txb_seconds);
    _SHR_UNPACK_U32(buf, msg->txb_nanoseconds);
    _SHR_UNPACK_U32(buf, msg->rxb_seconds);
    _SHR_UNPACK_U32(buf, msg->rxb_nanoseconds);
    _SHR_UNPACK_U32(buf, msg->rx_oam_packets);
    _SHR_UNPACK_U32(buf, msg->tx_oam_packets);
    _SHR_UNPACK_U32(buf, msg->int_pri);
    _SHR_UNPACK_U8 (buf, msg->pkt_pri);
    _SHR_UNPACK_U8 (buf, msg->dm_format);

    if(BHH_UC_FEATURE_CHECK(BHH_OLP_ENCAP)) {
        _SHR_UNPACK_U16(buf, msg->encap_length);
        for (i = 0; i < msg->encap_length; i++) {
            _SHR_UNPACK_U8(buf, msg->encap_data[i]);
        }
    }

    if(BHH_UC_FEATURE_CHECK(BHH_DM_DATA_TLV)) {
        _SHR_UNPACK_U16(buf, msg->data_tlv_len);
    }

    return buf;
}

uint8 *
bhh_sdk_msg_ctrl_loopback_get_unpack(uint8 *buf,
                                     bhh_sdk_msg_ctrl_loopback_get_t *msg)
{
    int i;

    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U32(buf, msg->sess_id);
    _SHR_UNPACK_U32(buf, msg->period);
    _SHR_UNPACK_U32(buf, msg->ttl);
    _SHR_UNPACK_U32(buf, msg->discovery_flags);
    _SHR_UNPACK_U32(buf, msg->discovery_id);
    _SHR_UNPACK_U32(buf, msg->discovery_ttl);
    _SHR_UNPACK_U32(buf, msg->rx_count);
    _SHR_UNPACK_U32(buf, msg->tx_count);
    _SHR_UNPACK_U32(buf, msg->drop_count);
    _SHR_UNPACK_U32(buf, msg->unexpected_response);
    _SHR_UNPACK_U32(buf, msg->out_of_sequence);
    _SHR_UNPACK_U32(buf, msg->local_mipid_missmatch);
    _SHR_UNPACK_U32(buf, msg->remote_mipid_missmatch);
    _SHR_UNPACK_U32(buf, msg->invalid_target_mep_tlv);
    _SHR_UNPACK_U32(buf, msg->invalid_mep_tlv_subtype);
    _SHR_UNPACK_U32(buf, msg->invalid_tlv_offset);

    if(BHH_UC_FEATURE_CHECK(BHH_OLP_ENCAP)) {
        _SHR_UNPACK_U16(buf, msg->encap_length);
        for (i = 0; i < msg->encap_length; i++) {
            _SHR_UNPACK_U8(buf, msg->encap_data[i]);
        }
    }

    if (BHH_UC_FEATURE_CHECK(BHH_PRIORITY_EVENT)) {
        _SHR_UNPACK_U32(buf, msg->int_pri);
        _SHR_UNPACK_U8 (buf, msg->pkt_pri);
    }

    return buf;
}

uint8 *
bhh_sdk_msg_ctrl_trunk_map_update_pack(uint8 *buf,
                                       bhh_sdk_msg_ctrl_trunk_map_update_t *msg)
{
    uint16 i;

    _SHR_PACK_U16(buf, msg->num_ports);
    _SHR_PACK_U16(buf, msg->trunk_id);
    for (i = 0; i < msg->num_ports; i++) {
        _SHR_PACK_U16(buf, msg->port_list[i]);
    }

    return buf;
}

uint8 *
bhh_sdk_msg_ctrl_trunk_map_get_unpack(uint8 *buf,
                                      bhh_sdk_msg_ctrl_trunk_map_get_t *msg)
{
   uint16 i;

    _SHR_UNPACK_U16(buf, msg->num_ports);
    _SHR_UNPACK_U16(buf, msg->trunk_id);
    for (i = 0; i < msg->num_ports; i++) {
        _SHR_UNPACK_U16(buf, msg->port_list[i]);
    }

    return buf;
}

uint8 *
bhh_sdk_msg_ctrl_csf_add_pack(uint8 *buf, bhh_sdk_msg_ctrl_csf_add_t *msg)
{
    uint16 i;

    _SHR_PACK_U16(buf, msg->sess_id);
    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U8 (buf, msg->type);
    _SHR_PACK_U32(buf, msg->period);
    _SHR_PACK_U8 (buf, msg->int_pri);
    _SHR_PACK_U8 (buf, msg->pkt_pri);

    _SHR_PACK_U16(buf, msg->encap_length);
    for (i = 0; i < msg->encap_length; i++) {
        _SHR_PACK_U8(buf, msg->encap_data[i]);
    }

    return buf;
}

uint8 *
bhh_sdk_msg_ctrl_csf_get_unpack(uint8 *buf, bhh_sdk_msg_ctrl_csf_get_t *msg)
{
    uint16 i;

    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U8 (buf, msg->type);
    _SHR_UNPACK_U32(buf, msg->period);
    _SHR_UNPACK_U8 (buf, msg->int_pri);
    _SHR_UNPACK_U8 (buf, msg->pkt_pri);

    _SHR_UNPACK_U16(buf, msg->encap_length);
    for (i = 0; i < msg->encap_length; i++) {
        _SHR_UNPACK_U8(buf, msg->encap_data[i]);
    }

    _SHR_UNPACK_U32(buf, msg->tx_csf_packets);
    return buf;
}

uint8 *
bhh_sdk_msg_ctrl_ach_channel_type_msg_pack(uint8 *buf, uint32 *msg)
{
    _SHR_PACK_U32(buf, *msg);
    return buf;
}

#else /* INCLUDE_BHH */
typedef int bcm_xgs5_bfd_not_empty; /* Make ISO compilers happy. */
#endif  /* INCLUDE_BHH */
