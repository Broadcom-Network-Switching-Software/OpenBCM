/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        llm_pack.c
 * Purpose:     LLM pack and unpack routines for:
 *              - llm Control messages
 *              - Network Packet headers (PDUs)
 *
 *
 * LLM control messages
 *
 * LLM messages between the Host CPU and uController are sent
 * using the uc_message module which allows short messages
 * to be passed (see include/soc/shared/mos_msg_common.h)
 *
 * Additional information for a given message (a long message) is passed
 * using DMA.  The LLM control message types defines the format
 * for these long messages.
 *
 * This file is shared between SDK and uKernel.
 */

#include <shared/pack.h>
#include <soc/shared/llm_msg.h>
#include <soc/shared/llm_pack.h>
/***********************************************************
 * LLM Control Message max structure size
 *
 * Functions:
 *      shr_max_buffer_get
 * Purpose:
 *      Detects what is the maximum buffer size should be allocated for the transaction
 *      Detects what is the maximum buffer size should be allocated for the transaction
 * Parameters:
 * Returns:
 *      Maximum buffer size.
 */

#define SHR_MAX_BUFFER_MACRO(buf_size,struct_type) buf_size<sizeof(struct_type)?buf_size=sizeof(struct_type):0;
uint32 shr_max_buffer_get(void)
{
    uint32 max_buff_size = 0;
    
    SHR_MAX_BUFFER_MACRO(max_buff_size, shr_llm_msg_ctrl_init_t);
    SHR_MAX_BUFFER_MACRO(max_buff_size, shr_llm_msg_pon_att_t);
    SHR_MAX_BUFFER_MACRO(max_buff_size, shr_llm_msg_app_info_get_t);
    SHR_MAX_BUFFER_MACRO(max_buff_size, shr_llm_msg_ctrl_limit_value_t);
    /*SHR_MAX_BUFFER_MACRO(max_buff_size, shr_llm_PON_ID_attributes_t);*/
    SHR_MAX_BUFFER_MACRO(max_buff_size, shr_llm_pointer_pool_t);
    SHR_MAX_BUFFER_MACRO(max_buff_size, shr_llm_msg_pon_att_enable_t);
    SHR_MAX_BUFFER_MACRO(max_buff_size, shr_llm_msg_pon_att_update_t);
    SHR_MAX_BUFFER_MACRO(max_buff_size, shr_llm_msg_pon_att_mac_move_t);
    SHR_MAX_BUFFER_MACRO(max_buff_size, shr_llm_msg_pon_att_mac_move_event_t);

    return max_buff_size;
}

/***********************************************************
 * LLM Control Message Pack/Unpack
 *
 * Functions:
 *      shr_llm_msg_ctrl_<type>_pack/unpack
 * Purpose:
 *      The following set of routines pack/unpack specified
 *      LLM control message into/from a given buffer
 * Parameters:
 *   _pack()
 *      buffer  - (OUT) Buffer where to pack message.
 *      msg     - (IN)  LLM control message to pack.
 *   _unpack()
 *      buffer  - (IN)  Buffer with message to unpack.
 *      msg     - (OUT) Returns LLM  control message.
 * Returns:
 *      Pointer to next position in buffer.
 * Notes:
 *      Assumes pointers are valid and contain enough memory space.
 */

uint8 *
shr_llm_msg_ctrl_init_pack(uint8 *buf, shr_llm_msg_ctrl_init_t *msg)
{

    _SHR_PACK_U32(buf, msg->dsp_eth_type       );
    _SHR_PACK_U32(buf, msg->voq_mapping_mode   );
    _SHR_PACK_U32(buf, msg->direct_table_bank  );
    _SHR_PACK_U32(buf, msg->direct_table_offset);
    _SHR_PACK_U32(buf, msg->eedb_pointer_database_offset);
    _SHR_PACK_U32(buf, msg->eedb_pointer_database_size);
    _SHR_PACK_U8 (buf, msg->eedb_omac_table_identifyer);

    return buf;
}
uint8 *
shr_llm_msg_ctrl_init_unpack(uint8 *buf, shr_llm_msg_ctrl_init_t *msg)
{

    _SHR_UNPACK_U32(buf, msg->dsp_eth_type       );
    _SHR_UNPACK_U32(buf, msg->voq_mapping_mode   );
    _SHR_UNPACK_U32(buf, msg->direct_table_bank  );
    _SHR_UNPACK_U32(buf, msg->direct_table_offset);
    _SHR_UNPACK_U32(buf, msg->eedb_pointer_database_offset);
    _SHR_UNPACK_U32(buf, msg->eedb_pointer_database_size);
    _SHR_UNPACK_U8 (buf, msg->eedb_omac_table_identifyer);
    return buf;
}

uint8 *
shr_llm_msg_pon_att_pack(uint8 *buf, shr_llm_msg_pon_att_t *msg)
{

    _SHR_PACK_U32(buf, msg->port    );
    _SHR_PACK_U32(buf, msg->tunnel  );
    _SHR_PACK_U32(buf, msg->tunnel_count  );
    _SHR_PACK_U32(buf, msg->bitmap  );
    _SHR_PACK_U32(buf, msg->type_of_service  );

    return buf;
}
uint8 *
shr_llm_msg_pon_att_unpack(uint8 *buf, shr_llm_msg_pon_att_t *msg)
{

    _SHR_UNPACK_U32(buf, msg->port    );
    _SHR_UNPACK_U32(buf, msg->tunnel  );
    _SHR_UNPACK_U32(buf, msg->tunnel_count  );
    _SHR_UNPACK_U32(buf, msg->bitmap  );
    _SHR_UNPACK_U32(buf, msg->type_of_service  );

    return buf;
}

uint8 *
shr_llm_msg_app_info_get_pack(uint8 *buf, shr_llm_msg_app_info_get_t *msg)
{

    _SHR_PACK_U32(buf, msg->counterEvents    );
    _SHR_PACK_U32(buf, msg->counterEventsLearn  );
    _SHR_PACK_U32(buf, msg->suc_counterEventsLearn  );
    _SHR_PACK_U32(buf, msg->counterEventsDelete    );
    _SHR_PACK_U32(buf, msg->suc_counterEventsDelete  );
    _SHR_PACK_U32(buf, msg->counterEventsTransp  );
    _SHR_PACK_U32(buf, msg->suc_counterEventsTransp    );
    _SHR_PACK_U32(buf, msg->counterEventsUnKnown  );
    _SHR_PACK_U32(buf, msg->num_of_activation  );
    _SHR_PACK_U32(buf, msg->time_of_activation  );
    return buf;
}
uint8 *
shr_llm_msg_app_info_get_unpack(uint8 *buf, shr_llm_msg_app_info_get_t *msg)
{

    _SHR_UNPACK_U32(buf, msg->counterEvents    );
    _SHR_UNPACK_U32(buf, msg->counterEventsLearn  );
    _SHR_UNPACK_U32(buf, msg->suc_counterEventsLearn  );
    _SHR_UNPACK_U32(buf, msg->counterEventsDelete    );
    _SHR_UNPACK_U32(buf, msg->suc_counterEventsDelete  );
    _SHR_UNPACK_U32(buf, msg->counterEventsTransp  );
    _SHR_UNPACK_U32(buf, msg->suc_counterEventsTransp    );
    _SHR_UNPACK_U32(buf, msg->counterEventsUnKnown  );
    _SHR_UNPACK_U32(buf, msg->num_of_activation  );
    _SHR_UNPACK_U32(buf, msg->time_of_activation  );

    return buf;
}

uint8 *
shr_llm_msg_ctrl_config_pack(uint8 *buf, shr_llm_msg_ctrl_limit_value_t *msg)
{

    _SHR_PACK_U32(buf, msg->pon_port_id);
    _SHR_PACK_U32(buf, msg->Tunnel_id);
    _SHR_PACK_U32(buf, msg->limit_value);
    _SHR_PACK_U8(buf, msg->limit_reached_direct_get);
    _SHR_PACK_U8(buf, msg->limit_reached_shadow_get);
    _SHR_PACK_U8(buf, msg->above_limit_reported_get);

    return buf;
}
uint8 *
shr_llm_msg_ctrl_config_unpack(uint8 *buf, shr_llm_msg_ctrl_limit_value_t *msg)
{

    _SHR_UNPACK_U32(buf, msg->pon_port_id);
    _SHR_UNPACK_U32(buf, msg->Tunnel_id);
    _SHR_UNPACK_U32(buf, msg->limit_value);
    _SHR_UNPACK_U8(buf, msg->limit_reached_direct_get);
    _SHR_UNPACK_U8(buf, msg->limit_reached_shadow_get);
    _SHR_UNPACK_U8(buf, msg->above_limit_reported_get);

    return buf;
}
uint8 *
shr_llm_msg_pon_db_pack(uint8 *buf, shr_llm_PON_ID_attributes_t *msg)
{
    int i;
    _SHR_PACK_U32(buf, msg->port);
    for (i=0;i<SHR_LLM_MAX_TUNNEL_INDEX;i++)
    {
        _SHR_PACK_U32(buf, msg->entries[i]);
    }
    

    return buf;
}
uint8 *
shr_llm_msg_pon_db_unpack(uint8 *buf, shr_llm_PON_ID_attributes_t *msg)
{

    int i;
    _SHR_UNPACK_U32(buf, msg->port);
    for (i=0;i<SHR_LLM_MAX_TUNNEL_INDEX;i++)
    {
        _SHR_UNPACK_U32(buf, msg->entries[i]);
    }
    
    return buf;
    
}
uint8 *
shr_llm_msg_pon_db_unpack_port(uint8 *buf, uint32 *port)
{

    
    _SHR_UNPACK_U32(buf, *port);
    return buf;
    
}
uint8 *
shr_llm_msg_pon_db_pack_port(uint8 *buf, uint32 *port)
{

    
    _SHR_PACK_U32(buf, *port);
    return buf;
    
}

uint8 *
shr_llm_msg_pointer_pool_pack(uint8 *buf, shr_llm_pointer_pool_t *msg)
{
    int i;
    for (i=0;i<SHR_LLM_PULL_MAX_SIZE;i++)
    {
        _SHR_PACK_U32(buf, msg->entries[i]);
    }
    _SHR_PACK_U32(buf, msg->firstIndexOffset    );
    _SHR_PACK_U16(buf, msg->size                );
    _SHR_PACK_U16(buf, msg->numOfFreeIndexes    );
    _SHR_PACK_U16(buf, msg->numOfReservedIndexes);
    _SHR_PACK_U8 (buf, msg->recentEntry         );
    return buf;
}

uint8 *
shr_llm_msg_pointer_pool_unpack(uint8 *buf, shr_llm_pointer_pool_t *msg)
{

    int i;
    for (i=0;i<SHR_LLM_PULL_MAX_SIZE;i++)
    {
        _SHR_UNPACK_U32(buf, msg->entries[i]);
    }
    _SHR_UNPACK_U32(buf, msg->firstIndexOffset    );
    _SHR_UNPACK_U16(buf, msg->size                );
    _SHR_UNPACK_U16(buf, msg->numOfFreeIndexes    );
    _SHR_UNPACK_U16(buf, msg->numOfReservedIndexes);
    _SHR_UNPACK_U8 (buf, msg->recentEntry         );
    return buf;
   
}

uint8 *
shr_llm_msg_pon_att_enable_pack(uint8 *buf, shr_llm_msg_pon_att_enable_t *msg)
{
    _SHR_PACK_U32(buf, msg->type_of_service);
    _SHR_PACK_U32(buf, msg->att_val);

    return buf;
}

uint8 *
shr_llm_msg_pon_att_enable_unpack(uint8 *buf, shr_llm_msg_pon_att_enable_t *msg)
{

    _SHR_UNPACK_U32(buf, msg->type_of_service);
    _SHR_UNPACK_U32(buf, msg->att_val);

    return buf;
}

uint8 *
shr_llm_msg_pon_att_update_pack(uint8 *buf, shr_llm_msg_pon_att_update_t *msg)
{
    uint32 i = 0, j = 0;
    
    _SHR_PACK_U32(buf, msg->type_of_service);
    _SHR_PACK_U32(buf, msg->data_type);
    _SHR_PACK_U32(buf, msg->data_len);

    switch (msg->data_type) 
    {      
    case SHR_LLM_MSG_PON_ATT_UPDATE_MAC_ENTRY:
        for(i = 0; i < SHR_LLM_MSG_PON_ATT_UPDATE_MAC_ENTRY_NUM; i++)
        {
            _SHR_PACK_U32(buf, msg->data.l2_entry.entry[i]);
        }
        break;
    case SHR_LLM_MSG_PON_ATT_UPDATE_VMAC_ENCODE:
        /* vmac encoding value */
        for(i = 0; i < SHR_LLM_MSG_PON_ATT_UPDATE_VMAC_ENCODE_NUM; i++)
        {
            _SHR_PACK_U32(buf, msg->data.vmac_encode.value[i]);
        }
        /* vmac encoding mask */
        for(i = 0; i < SHR_LLM_MSG_PON_ATT_UPDATE_VMAC_ENCODE_NUM; i++)
        {
            _SHR_PACK_U32(buf, msg->data.vmac_encode.mask[i]);
        }
        break;
    case SHR_LLM_MSG_PON_ATT_UPDATE_VMAC_PREFIX:
        /* vmac prefix */
        for(i = 0; i < SHR_LLM_MSG_PON_ATT_UPDATE_VMAC_PREFIX_NUM; i++)
        {
            _SHR_PACK_U32(buf, msg->data.vmac_prefix.prefix[i]);
        }
        break;
    case SHR_LLM_MSG_PON_ATT_UPDATE_MULTI_MAC_ENTRY:
        /* Multi l2 entry */
        _SHR_PACK_U32(buf, msg->data.multi_l2_entry.l2_entry_count);
        for(i = 0; i < msg->data.multi_l2_entry.l2_entry_count; i++)
        {
            for(j = 0; j < SHR_LLM_MSG_PON_ATT_UPDATE_MAC_ENTRY_NUM; j++)
            {
                _SHR_PACK_U32(buf, msg->data.multi_l2_entry.l2_entry[i].entry[j]);
            }
        }
        break;
    default:
        break;
    }

    return buf;
}

uint8 *
shr_llm_msg_pon_att_update_unpack(uint8 *buf, shr_llm_msg_pon_att_update_t *msg)
{
    uint32 i = 0, j = 0;

    _SHR_UNPACK_U32(buf, msg->type_of_service);
    _SHR_UNPACK_U32(buf, msg->data_type);
    _SHR_UNPACK_U32(buf, msg->data_len);

    switch (msg->data_type)
    {      
    case SHR_LLM_MSG_PON_ATT_UPDATE_MAC_ENTRY:
        for(i = 0; i < SHR_LLM_MSG_PON_ATT_UPDATE_MAC_ENTRY_NUM; i++)
        {
            _SHR_UNPACK_U32(buf, msg->data.l2_entry.entry[i]);
        }
        break;
    case SHR_LLM_MSG_PON_ATT_UPDATE_VMAC_ENCODE:
        /* vmac encoding value */
        for(i = 0; i < SHR_LLM_MSG_PON_ATT_UPDATE_VMAC_ENCODE_NUM; i++)
        {
            _SHR_UNPACK_U32(buf, msg->data.vmac_encode.value[i]);
        }
        /* vmac encoding mask */
        for(i = 0; i < SHR_LLM_MSG_PON_ATT_UPDATE_VMAC_ENCODE_NUM; i++)
        {
            _SHR_UNPACK_U32(buf, msg->data.vmac_encode.mask[i]);
        }
        break;
    case SHR_LLM_MSG_PON_ATT_UPDATE_VMAC_PREFIX:
        /* vmac prefix */
        for(i = 0; i < SHR_LLM_MSG_PON_ATT_UPDATE_VMAC_PREFIX_NUM; i++)
        {
            _SHR_UNPACK_U32(buf, msg->data.vmac_prefix.prefix[i]);
        }
        break;
    case SHR_LLM_MSG_PON_ATT_UPDATE_MULTI_MAC_ENTRY:
        /* Multi l2 entry */
        _SHR_UNPACK_U32(buf, msg->data.multi_l2_entry.l2_entry_count);
        for(i = 0; i < msg->data.multi_l2_entry.l2_entry_count; i++)
        {
            for(j = 0; j < SHR_LLM_MSG_PON_ATT_UPDATE_MAC_ENTRY_NUM; j++)
            {
                _SHR_UNPACK_U32(buf, msg->data.multi_l2_entry.l2_entry[i].entry[j]);
            }
        }
        break;
    default:
        break;
    }

    return buf;
}

uint8 *
shr_llm_msg_pon_att_rx_mode_pack(uint8 *buf, shr_llm_msg_pon_att_rx_mode_t *msg)
{    
    _SHR_PACK_U32(buf, msg->mode);

    return buf;
}

uint8 *
shr_llm_msg_pon_att_rx_mode_unpack(uint8 *buf, shr_llm_msg_pon_att_rx_mode_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->mode);

    return buf;
}

uint8 *
shr_llm_msg_pon_att_mac_move_pack(uint8 *buf, shr_llm_msg_pon_att_mac_move_t *msg)
{    
    _SHR_PACK_U32(buf, msg->port);
    _SHR_PACK_U32(buf, msg->value);

    return buf;
}

uint8 *
shr_llm_msg_pon_att_mac_move_unpack(uint8 *buf, shr_llm_msg_pon_att_mac_move_t *msg)
{
    _SHR_UNPACK_U32(buf, msg->port);
    _SHR_UNPACK_U32(buf, msg->value);

    return buf;
}

uint8 *
shr_llm_msg_pon_att_mac_move_event_pack(uint8 *buf, shr_llm_msg_pon_att_mac_move_event_t *msg)
{    
    uint32 i = 0;

    for (i = 0; i < 6; i++) {
        _SHR_PACK_U8(buf, msg->mac[i]);
    }
    _SHR_PACK_U16(buf, msg->vsi);
    _SHR_PACK_U16(buf, msg->incoming_ac);
    _SHR_PACK_U16(buf, msg->existing_ac);

    return buf;
}

uint8 *
shr_llm_msg_pon_att_mac_move_event_unpack(uint8 *buf, shr_llm_msg_pon_att_mac_move_event_t *msg)
{
    uint32 i = 0;
    for (i = 0; i < 6; i++) {
        _SHR_UNPACK_U8(buf, msg->mac[i]);
    }
    _SHR_UNPACK_U16(buf, msg->vsi);
    _SHR_UNPACK_U16(buf, msg->incoming_ac);
    _SHR_UNPACK_U16(buf, msg->existing_ac);

    return buf;
}



