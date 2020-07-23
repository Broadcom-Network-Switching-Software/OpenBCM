/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        ccm_pack.c
 * Purpose:     CCM pack and unpack routines for:
 *              - CCM Control messages
 *
 * CCM control messages
 *
 * CCM messages between the Host CPU and uController are sent
 * using the uc_message module which allows short messages
 * to be passed (see include/soc/shared/mos_msg_common.h)
 *
 * Additional information for a given message (a long message) is passed
 * using DMA.  The CCM control message types defines the format
 * for these long messages.
 *
 * This file is shared between SDK and uKernel.
 */

#include <soc/shared/ccm_msg.h>


/*
 * Functions:
 *      shr_ccm_msg_ctrl_<type>_pack/unpack
 * Purpose:
 *      The following set of routines pack/unpack specified
 *      CCM control message into/from a given buffer
 * Parameters:
 *   _pack()
 *      buffer  - (OUT) Buffer where to pack message.
 *      msg     - (IN)  CCM control message to pack.
 *   _unpack()
 *      buffer  - (IN)  Buffer with message to unpack.
 *      msg     - (OUT) Returns CCM control message.
 * Returns:
 *      Pointer to next position in buffer.
 * Notes:
 *      Assumes pointers are valid and contain enough memory space.
 */
uint8 *
shr_ccm_msg_ctrl_init_pack(uint8 *buf, shr_ccm_msg_ctrl_init_t *msg)
{

    _SHR_PACK_U32(buf, msg->max_groups);
    _SHR_PACK_U32(buf, msg->max_meps);
    _SHR_PACK_U32(buf, msg->rx_channel);

    return buf;
}

uint8 *
shr_ccm_msg_ctrl_init_unpack(uint8 *buf, shr_ccm_msg_ctrl_init_t *msg)
{

    _SHR_UNPACK_U32(buf, msg->max_groups);
    _SHR_UNPACK_U32(buf, msg->max_meps);
    _SHR_UNPACK_U32(buf, msg->rx_channel);

    return buf;
}

/* MEP data from SDK to UKENREL - Pack routine */
uint8 *
shr_ccm_msg_ctrl_mep_set_pack(uint8 *buf, shr_ccm_msg_ctrl_mep_data_t *msg)
{
    uint16 i;

    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U32(buf, msg->mep_data.flags);

    /* If update, send only modified data */
    if (msg->flags & _MEP_REPLACE) {

        _SHR_PACK_U32(buf, msg->mep_data.ep_id);
        if (msg->flags & _REPLACE_INTF_STATUS) {
            _SHR_PACK_U8(buf, msg->mep_data.intf_state);
        }

        if (msg->flags & _REPLACE_PORT_STATUS) {
            _SHR_PACK_U8(buf, msg->mep_data.port_state);
        }

        if (msg->flags & _REPLACE_TRUNK_INDEX) {
            _SHR_PACK_U32(buf, msg->mep_data.trunk_index);
            _SHR_PACK_U32(buf, msg->mep_data.dglp);
        }

        if (msg->flags & _REPLACE_CTRS) {
            _SHR_PACK_U8(buf, msg->mep_data.num_ctrs);
            for (i=0; i< msg->mep_data.num_ctrs; i++) {
                _SHR_PACK_U8(buf, msg->mep_data.counter_offset[i]);
                _SHR_PACK_U32(buf, msg->mep_data.flex_stat_id[i]);

            }
        }

        if (msg->flags & _MEP_OLP_RECREATE) {
            for (i = 0; i < _FP_OAM_OLP_TX_LEN; i++) {
                _SHR_PACK_U8(buf, msg->OLP_Tx_Hdr[i]);
            }
        }

        return buf;
    }

    /* Else send everything */
    _SHR_PACK_U32(buf, msg->mep_data.ep_id);
    _SHR_PACK_U32(buf, msg->mep_data.group_id);

    for (i=0; i<6; i++) {
        _SHR_PACK_U8(buf, msg->mep_data.da[i]);
    }
    for (i=0; i<6; i++) {
        _SHR_PACK_U8(buf, msg->mep_data.sa[i]);
    }

    _SHR_PACK_U8(buf, msg->mep_data.type);
    _SHR_PACK_U8(buf, msg->mep_data.level);
    _SHR_PACK_U16(buf, msg->mep_data.name);
    _SHR_PACK_U32(buf, msg->mep_data.period);

    _SHR_PACK_U16(buf, msg->mep_data.vlan);
    _SHR_PACK_U16(buf, msg->mep_data.inner_vlan);
    _SHR_PACK_U8(buf, msg->mep_data.pkt_pri);
    _SHR_PACK_U8(buf, msg->mep_data.inner_vlan_pri);
    _SHR_PACK_U16(buf, msg->mep_data.tpid);
    _SHR_PACK_U16(buf, msg->mep_data.inner_tpid);

    _SHR_PACK_U32(buf, msg->mep_data.gport);
    _SHR_PACK_U32(buf, msg->mep_data.sglp);
    _SHR_PACK_U32(buf, msg->mep_data.dglp);
    _SHR_PACK_U32(buf, msg->mep_data.trunk_index);

    _SHR_PACK_U8(buf, msg->mep_data.port_state);
    _SHR_PACK_U8(buf, msg->mep_data.intf_state);
    
    /* We need not pack/unpack faults data from SDK to UKERNEL */

    _SHR_PACK_U8(buf, msg->mep_data.num_ctrs);
    for (i=0; i< msg->mep_data.num_ctrs; i++) {
        _SHR_PACK_U8(buf, msg->mep_data.counter_offset[i]);
        _SHR_PACK_U32(buf, msg->mep_data.flex_stat_id[i]);

    }

    if (msg->flags & _MEP_L2_ENCAP_RECREATE) {
        _SHR_PACK_U8(buf, msg->l2_encap_len);
        for (i = 0; i < msg->l2_encap_len; i++) {
            _SHR_PACK_U8(buf, msg->L2_encap[i]);
        }
    }

    if (msg->flags & _MEP_OLP_RECREATE) {
        for (i = 0; i < _FP_OAM_OLP_TX_LEN; i++) {
            _SHR_PACK_U8(buf, msg->OLP_Tx_Hdr[i]);
        }
    }

    return buf;
}

/* MEP data from SDK to UKENREL - UnPack routine */
uint8 *
shr_ccm_msg_ctrl_mep_set_unpack(uint8 *buf, shr_ccm_msg_ctrl_mep_data_t *msg)
{
    uint16 i;

    _SHR_UNPACK_U32(buf, msg->flags);             
    _SHR_UNPACK_U32(buf, msg->mep_data.flags);

    /* If update, unpack only modified data */
    if (msg->flags & _MEP_REPLACE) {

        _SHR_UNPACK_U32(buf, msg->mep_data.ep_id);
        if (msg->flags & _REPLACE_INTF_STATUS) {
            _SHR_UNPACK_U8(buf, msg->mep_data.intf_state);
        }

        if (msg->flags & _REPLACE_PORT_STATUS) {
            _SHR_UNPACK_U8(buf, msg->mep_data.port_state);
        }

        if (msg->flags & _REPLACE_TRUNK_INDEX) {
            _SHR_UNPACK_U32(buf, msg->mep_data.trunk_index);
            _SHR_UNPACK_U32(buf, msg->mep_data.dglp);
        }

        if (msg->flags & _REPLACE_CTRS) {
            _SHR_UNPACK_U8(buf, msg->mep_data.num_ctrs);
            for (i=0; i< msg->mep_data.num_ctrs; i++) {
                _SHR_UNPACK_U8(buf, msg->mep_data.counter_offset[i]);
                _SHR_UNPACK_U32(buf, msg->mep_data.flex_stat_id[i]);

            }
        }

        if (msg->flags & _MEP_OLP_RECREATE) {
            for (i = 0; i < _FP_OAM_OLP_TX_LEN; i++) {
                _SHR_UNPACK_U8(buf, msg->OLP_Tx_Hdr[i]);
            }
        }

        return buf;
    }

    /* Else unpack everything */
    _SHR_UNPACK_U32(buf, msg->mep_data.ep_id);
    _SHR_UNPACK_U32(buf, msg->mep_data.group_id);

    for (i=0; i<6; i++) {
        _SHR_UNPACK_U8(buf, msg->mep_data.da[i]);
    }
    for (i=0; i<6; i++) {
        _SHR_UNPACK_U8(buf, msg->mep_data.sa[i]);
    }

    _SHR_UNPACK_U8(buf, msg->mep_data.type);
    _SHR_UNPACK_U8(buf, msg->mep_data.level);
    _SHR_UNPACK_U16(buf, msg->mep_data.name);
    _SHR_UNPACK_U32(buf, msg->mep_data.period);

    _SHR_UNPACK_U16(buf, msg->mep_data.vlan);
    _SHR_UNPACK_U16(buf, msg->mep_data.inner_vlan);
    _SHR_UNPACK_U8(buf, msg->mep_data.pkt_pri);
    _SHR_UNPACK_U8(buf, msg->mep_data.inner_vlan_pri);
    _SHR_UNPACK_U16(buf, msg->mep_data.tpid);
    _SHR_UNPACK_U16(buf, msg->mep_data.inner_tpid);

    _SHR_UNPACK_U32(buf, msg->mep_data.gport);
    _SHR_UNPACK_U32(buf, msg->mep_data.sglp);
    _SHR_UNPACK_U32(buf, msg->mep_data.dglp);
    _SHR_UNPACK_U32(buf, msg->mep_data.trunk_index);
    
    _SHR_UNPACK_U8(buf, msg->mep_data.port_state);
    _SHR_UNPACK_U8(buf, msg->mep_data.intf_state);

    /* We need not pack/unpack faults data from SDK to UKERNEL */

    _SHR_UNPACK_U8(buf, msg->mep_data.num_ctrs);
    for (i=0; i< msg->mep_data.num_ctrs; i++) {
        _SHR_UNPACK_U8(buf, msg->mep_data.counter_offset[i]);
        _SHR_UNPACK_U32(buf, msg->mep_data.flex_stat_id[i]);

    }

    if (msg->flags & _MEP_L2_ENCAP_RECREATE) {
        _SHR_UNPACK_U8(buf, msg->l2_encap_len);
        for (i = 0; i < msg->l2_encap_len; i++) {
            _SHR_UNPACK_U8(buf, msg->L2_encap[i]);
        }
    }

    if (msg->flags & _MEP_OLP_RECREATE) {
        for (i = 0; i < _FP_OAM_OLP_TX_LEN; i++) {
            _SHR_UNPACK_U8(buf, msg->OLP_Tx_Hdr[i]);
        }
    }

    return buf;
}

/* Recover Mep data from UKENREL to SDK - Pack routine */
uint8 *
shr_ccm_msg_ctrl_mep_get_pack(uint8 *buf, shr_ccm_msg_ctrl_mep_data_t *msg)
{
    uint16 i;

    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U32(buf, msg->mep_data.flags);

    if (msg->flags & _MEP_GET_FAULTS_ONLY) {
        _SHR_PACK_U32(buf, msg->mep_data.faults);
        _SHR_PACK_U32(buf, msg->mep_data.persistent_faults);
        _SHR_PACK_U8(buf, msg->mep_data.port_state);
        _SHR_PACK_U8(buf, msg->mep_data.intf_state);
        for (i=0; i<6; i++) {
            _SHR_PACK_U8(buf, msg->mep_data.da[i]);
        }
        for (i=0; i<6; i++) {
            _SHR_PACK_U8(buf, msg->mep_data.sa[i]);
        }
        return buf;
    }

    _SHR_PACK_U32(buf, msg->mep_data.ep_id);
    _SHR_PACK_U32(buf, msg->mep_data.group_id);

    for (i=0; i<6; i++) {
        _SHR_PACK_U8(buf, msg->mep_data.da[i]);
    }
    for (i=0; i<6; i++) {
        _SHR_PACK_U8(buf, msg->mep_data.sa[i]);
    }

    _SHR_PACK_U8(buf, msg->mep_data.type);
    _SHR_PACK_U8(buf, msg->mep_data.level);
    _SHR_PACK_U16(buf, msg->mep_data.name);
    _SHR_PACK_U32(buf, msg->mep_data.period);

    _SHR_PACK_U16(buf, msg->mep_data.vlan);
    _SHR_PACK_U16(buf, msg->mep_data.inner_vlan);
    _SHR_PACK_U8(buf, msg->mep_data.pkt_pri);
    _SHR_PACK_U8(buf, msg->mep_data.inner_vlan_pri);
    _SHR_PACK_U16(buf, msg->mep_data.tpid);
    _SHR_PACK_U16(buf, msg->mep_data.inner_tpid);

    _SHR_PACK_U32(buf, msg->mep_data.gport);
    _SHR_PACK_U32(buf, msg->mep_data.sglp);
    _SHR_PACK_U32(buf, msg->mep_data.dglp);
    _SHR_PACK_U32(buf, msg->mep_data.trunk_index);

    _SHR_PACK_U8(buf, msg->mep_data.port_state);
    _SHR_PACK_U8(buf, msg->mep_data.intf_state);

    _SHR_PACK_U32(buf, msg->mep_data.faults);
    _SHR_PACK_U32(buf, msg->mep_data.persistent_faults);

    _SHR_PACK_U8(buf, msg->mep_data.num_ctrs);
    for (i=0; i< msg->mep_data.num_ctrs; i++) {
        _SHR_PACK_U8(buf, msg->mep_data.counter_offset[i]);
        _SHR_PACK_U32(buf, msg->mep_data.flex_stat_id[i]);

    }

    /* We need not pack/unpack L2 Header data from UKERNEL to SDK */

    return buf;
}

/* Recover Mep data from UKENREL to SDK - UnPack routine */
uint8 *
shr_ccm_msg_ctrl_mep_get_unpack(uint8 *buf, shr_ccm_msg_ctrl_mep_data_t *msg)
{
    uint16 i;

    _SHR_UNPACK_U32(buf, msg->flags);             
    _SHR_UNPACK_U32(buf, msg->mep_data.flags);

    if (msg->flags & _MEP_GET_FAULTS_ONLY) {
        _SHR_UNPACK_U32(buf, msg->mep_data.faults);
        _SHR_UNPACK_U32(buf, msg->mep_data.persistent_faults);
        _SHR_UNPACK_U8(buf, msg->mep_data.port_state);
        _SHR_UNPACK_U8(buf, msg->mep_data.intf_state);
        for (i=0; i<6; i++) {
            _SHR_UNPACK_U8(buf, msg->mep_data.da[i]);
        }
        for (i=0; i<6; i++) {
            _SHR_UNPACK_U8(buf, msg->mep_data.sa[i]);
        }
        return buf;
    }

    _SHR_UNPACK_U32(buf, msg->mep_data.ep_id);
    _SHR_UNPACK_U32(buf, msg->mep_data.group_id);

    for (i=0; i<6; i++) {
        _SHR_UNPACK_U8(buf, msg->mep_data.da[i]);
    }
    for (i=0; i<6; i++) {
        _SHR_UNPACK_U8(buf, msg->mep_data.sa[i]);
    }

    _SHR_UNPACK_U8(buf, msg->mep_data.type);
    _SHR_UNPACK_U8(buf, msg->mep_data.level);
    _SHR_UNPACK_U16(buf, msg->mep_data.name);
    _SHR_UNPACK_U32(buf, msg->mep_data.period);

    _SHR_UNPACK_U16(buf, msg->mep_data.vlan);
    _SHR_UNPACK_U16(buf, msg->mep_data.inner_vlan);
    _SHR_UNPACK_U8(buf, msg->mep_data.pkt_pri);
    _SHR_UNPACK_U8(buf, msg->mep_data.inner_vlan_pri);
    _SHR_UNPACK_U16(buf, msg->mep_data.tpid);
    _SHR_UNPACK_U16(buf, msg->mep_data.inner_tpid);

    _SHR_UNPACK_U32(buf, msg->mep_data.gport);
    _SHR_UNPACK_U32(buf, msg->mep_data.sglp);
    _SHR_UNPACK_U32(buf, msg->mep_data.dglp);
    _SHR_UNPACK_U32(buf, msg->mep_data.trunk_index);
    
    _SHR_UNPACK_U8(buf, msg->mep_data.port_state);
    _SHR_UNPACK_U8(buf, msg->mep_data.intf_state);

    _SHR_UNPACK_U32(buf, msg->mep_data.faults);
    _SHR_UNPACK_U32(buf, msg->mep_data.persistent_faults);

    _SHR_UNPACK_U8(buf, msg->mep_data.num_ctrs);
    for (i=0; i< msg->mep_data.num_ctrs; i++) {
        _SHR_UNPACK_U8(buf, msg->mep_data.counter_offset[i]);
        _SHR_UNPACK_U32(buf, msg->mep_data.flex_stat_id[i]);

    }

    /* We need not pack/unpack L2 Header data from UKERNEL to SDK */

    return buf;
}

/* Group data from SDK to UKENREL - Pack routine */
uint8 *
shr_ccm_msg_ctrl_group_set_pack(uint8 *buf, shr_ccm_msg_ctrl_group_data_t *msg)
{
    uint16 i;

    _SHR_PACK_U32(buf, msg->flags);             
    _SHR_PACK_U32(buf, msg->group_data.flags);

    /* If update, send only modified data */
    if (msg->flags & _GRP_REPLACE) {

        _SHR_PACK_U32(buf, msg->group_data.group_id);
        /* Sw RDI is taken care in the flags */
        if (msg->flags & _GRP_REPLACE_LOWEST_ALARM_PRIO) {
            _SHR_PACK_U32(buf, msg->group_data.lowest_alarm_priority);
        }

        return buf;
    }

    /* Else send everything */
    _SHR_PACK_U32(buf, msg->group_data.group_id);
    _SHR_PACK_U32(buf, msg->group_data.lowest_alarm_priority);
    /* We need not pack/unpack faults data from SDK to UKERNEL */
    for (i=0; i<_FP_OAM_MAID_LEN; i++) {
        _SHR_PACK_U8(buf, msg->group_data.name[i]);
    }
     /* We need not pack/unpack lmepid, rmep_bitmap from SDK to UKERNEL */
    return buf;
}

/* Group data from SDK to UKENREL - UnPack routine */
uint8 *
shr_ccm_msg_ctrl_group_set_unpack(uint8 *buf, shr_ccm_msg_ctrl_group_data_t *msg)
{
    uint16 i;

    _SHR_UNPACK_U32(buf, msg->flags);             
    _SHR_UNPACK_U32(buf, msg->group_data.flags);

    /* If update, send only modified data */
    if (msg->flags & _GRP_REPLACE) {

        _SHR_UNPACK_U32(buf, msg->group_data.group_id);
        /* Sw RDI is taken care in the flags */
        if (msg->flags & _GRP_REPLACE_LOWEST_ALARM_PRIO) {
            _SHR_UNPACK_U32(buf, msg->group_data.lowest_alarm_priority);
        }

        return buf;
    }

    /* Else send everything */
    _SHR_UNPACK_U32(buf, msg->group_data.group_id);
    _SHR_UNPACK_U32(buf, msg->group_data.lowest_alarm_priority);
    /* We need not pack/unpack faults data from SDK to UKERNEL */
    for (i=0; i<_FP_OAM_MAID_LEN; i++) {
        _SHR_UNPACK_U8(buf, msg->group_data.name[i]);
    }
     /* We need not pack/unpack lmepid, rmep_bitmap from SDK to UKERNEL */
    return buf;
}

/* Get Group data from UKERNEL to SDK - Pack routine */
uint8 *
shr_ccm_msg_ctrl_group_get_pack(uint8 *buf, shr_ccm_msg_ctrl_group_data_t *msg)
{
    uint16 i;

    _SHR_PACK_U32(buf, msg->flags);
    _SHR_PACK_U32(buf, msg->group_data.flags);

    if (msg->flags & _GRP_GET_FAULTS_ONLY) {
        _SHR_PACK_U32(buf, msg->group_data.faults);
        _SHR_PACK_U32(buf, msg->group_data.persistent_faults);
        return buf;
    }

    _SHR_PACK_U32(buf, msg->group_data.group_id);
    _SHR_PACK_U32(buf, msg->group_data.lmep_id);
    _SHR_PACK_U32(buf, msg->group_data.lowest_alarm_priority);
    _SHR_PACK_U32(buf, msg->group_data.faults);
    _SHR_PACK_U32(buf, msg->group_data.persistent_faults);
    for (i=0; i<_FP_OAM_MAID_LEN; i++) {
        _SHR_PACK_U8(buf, msg->group_data.name[i]);
    }
    for (i=0; i<_CCM_MEP_BIT_FIELD_LEN; i++) {
        _SHR_PACK_U32(buf, msg->group_data.rmep_bitmap[i]);
    }

    return buf;
}

/* Get Group data from UKERNEL to SDK - UnPack routine */
uint8 *
shr_ccm_msg_ctrl_group_get_unpack(uint8 *buf, shr_ccm_msg_ctrl_group_data_t *msg)
{
    uint16 i;

    _SHR_UNPACK_U32(buf, msg->flags);
    _SHR_UNPACK_U32(buf, msg->group_data.flags);

    if (msg->flags & _GRP_GET_FAULTS_ONLY) {
        _SHR_UNPACK_U32(buf, msg->group_data.faults);
        _SHR_UNPACK_U32(buf, msg->group_data.persistent_faults);
        return buf;
    }

    _SHR_UNPACK_U32(buf, msg->group_data.group_id);
    _SHR_UNPACK_U32(buf, msg->group_data.lmep_id);
    _SHR_UNPACK_U32(buf, msg->group_data.lowest_alarm_priority);
    _SHR_UNPACK_U32(buf, msg->group_data.faults);
    _SHR_UNPACK_U32(buf, msg->group_data.persistent_faults);
    for (i=0; i<_FP_OAM_MAID_LEN; i++) {
        _SHR_UNPACK_U8(buf, msg->group_data.name[i]);
    }
    for (i=0; i<_CCM_MEP_BIT_FIELD_LEN; i++) {
        _SHR_UNPACK_U32(buf, msg->group_data.rmep_bitmap[i]);
    }

    return buf;
}

/* Get App data from UKERNEL to SDK - Pack routine */
uint8 *
shr_ccm_msg_ctrl_app_data_pack(uint8 *buf, shr_ccm_msg_ctrl_app_data_t *msg)
{
    uint16 i;

    _SHR_PACK_U32(buf, msg->flags);

    for (i=0; i<_CCM_GRP_BIT_FILED_LEN; i++) {
        _SHR_PACK_U32(buf, msg->group_bitmap[i]);
    }

    return buf;
}

/* Get App data from UKERNEL to SDK - UnPack routine */
uint8 *
shr_ccm_msg_ctrl_app_data_unpack(uint8 *buf, shr_ccm_msg_ctrl_app_data_t *msg)
{
    uint16 i;

    _SHR_UNPACK_U32(buf, msg->flags);

    for (i=0; i<_CCM_GRP_BIT_FILED_LEN; i++) {
        _SHR_UNPACK_U32(buf, msg->group_bitmap[i]);
    }

    return buf;
}


/* Pack port trunk map update msg into buf */
uint8 *
shr_ccm_msg_ctrl_port_trunk_upd_pack(uint8 *buf, shr_ccm_msg_ctrl_port_trunk_upd_t *msg)
{
    uint16 i;
    _SHR_PACK_U16(buf, msg->num_ports);
    _SHR_PACK_U16(buf, msg->trunk_id);
    for (i = 0; i < msg->num_ports; i++) {
        _SHR_PACK_U16(buf, msg->list_of_ports[i]);
    }

    return buf;
}

/* Unpack port trunk map update info from buf to msg structure*/
uint8 *
shr_ccm_msg_ctrl_port_trunk_upd_unpack(uint8 *buf, shr_ccm_msg_ctrl_port_trunk_upd_t *msg)
{
    uint16 i;
    _SHR_UNPACK_U16(buf, msg->num_ports);
    _SHR_UNPACK_U16(buf, msg->trunk_id);
    for (i = 0; i < msg->num_ports; i++) {
        _SHR_UNPACK_U16(buf, msg->list_of_ports[i]);
    }

    return buf;
}
