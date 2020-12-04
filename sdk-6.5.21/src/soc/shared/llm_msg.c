/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        llm_msg.c
 * Purpose:     LLM message routines for:
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
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif
     
#define _ERR_MSG_MODULE_NAME BSL_SOC_COMMON

#include <shared/bsl.h>
#include <shared/alloc.h>
#include <soc/shared/llm_msg.h>
#include <soc/shared/llm_pack.h>
#include <soc/shared/llm_appl.h>
#ifdef BCM_DPP_SUPPORT
#include <soc/dpp/SAND/Utils/sand_bitstream.h>

#include <soc/error.h>
#include <soc/dcmn/error.h>
#include <soc/uc_msg.h>


/*
 * Function:
 *      shr_llm_msg_send_receive
 * Purpose:
 *      Send a LLM message to ARM CPU
 *      and MAC limit function.
 * Parameters:
 *      unit  (IN) unit number
 *      s_subclass (IN) send subclass
 *      s_len (IN) send length
 *      s_data (IN) send data
 *      r_len (OUT) receive length
 *      r_data (OUT) receive data
 */
int shr_llm_msg_send_receive(int unit, uint8 s_subclass, uint16 s_len, uint32 s_data, 
                             uint8 r_subclass, uint16 *r_len, uint32 * r_data)
{
    int rv = SOC_E_NONE;
    mos_msg_data_t send, reply;
    uint8 *dma_buffer;
    int dma_buffer_len;

    sal_memset(&send, 0, sizeof(send));
    sal_memset(&reply, 0, sizeof(reply));
    send.s.mclass = MOS_MSG_CLASS_LLM;
    send.s.subclass = s_subclass;
    send.s.len = bcm_htons(s_len);

    /* Set 'data' to DMA buffer address if a DMA operation is required for send or receive. */
    dma_buffer = llm_appl_info[unit].dma_buffer;
    dma_buffer_len = llm_appl_info[unit].dma_buffer_len;
    if (MOS_MSG_DMA_MSG(s_subclass) ||
        MOS_MSG_DMA_MSG(r_subclass)) {
        send.s.data = bcm_htonl(soc_cm_l2p(unit, dma_buffer));
    } else {
        send.s.data = bcm_htonl(s_data);
    }

    /* Flush DMA memory */
    if (MOS_MSG_DMA_MSG(s_subclass)) {
        soc_cm_sflush(unit, dma_buffer, s_len);
    }

    /* Invalidate DMA memory to read */
    if (MOS_MSG_DMA_MSG(r_subclass)) {
        soc_cm_sinval(unit, dma_buffer, dma_buffer_len);
    }
    /* Check llm_mutex whether initialized */
    if (llm_appl_info[unit].llm_mutex == NULL) {
        return SOC_E_INTERNAL;
    }
    LLM_MSG_LOCK(unit);
    rv = soc_cmic_uc_msg_send_receive(unit, llm_appl_info[unit].uc_num,
                                      &send, &reply,
                                      SHR_LLM_UC_MSG_TIMEOUT_USECS);
    LLM_MSG_UNLOCK(unit);

    /* Check reply class, subclass */
    if ((rv != SOC_E_NONE) ||
        (reply.s.mclass != MOS_MSG_CLASS_LLM) ||
        (reply.s.subclass != r_subclass)) {
        return SOC_E_INTERNAL;
    }

    *r_data = bcm_ntohl(reply.s.data);
    *r_len = bcm_ntohs(reply.s.len);
    /* Convert BHH uController error code to BCM because ARM store the return code at r_data */
    rv = *r_data;

    return rv;
}

/*
 * Function:
 *      shr_llm_msg_mac_limit_enable
 * Purpose:
 *      Send a LLM message to ARM CPU to disable/enable read reply FIFO
 *      and MAC limit function.
 * Parameters:
 *      mac_limit_enable       (IN) enable/disable mac limit function
 *      reply_fifo_enable      (IN) enable/disable read reply-FIFO function
 */
int shr_llm_msg_init(int unit)
{
    int rv = SOC_E_NONE;
    shr_llm_msg_ctrl_init_t msg;
    uint8 *buffer;      
    uint16 send_len;
    uint16 reply_len;
    uint32 reply;

    sal_memset(&msg, 0x0, sizeof(msg));
    msg.direct_table_bank = 10;
    
    /* Pack control message data into DMA buffer */
    buffer     = llm_appl_info[unit].dma_buffer;
    shr_llm_msg_ctrl_init_pack(buffer, &msg);
    send_len = sizeof(shr_llm_msg_ctrl_init_t);
    
    /* Send LLM Start message to uC */
    rv = shr_llm_msg_send_receive(unit, MOS_MSG_SUBCLASS_LLM_INIT, send_len, 0, MOS_MSG_SUBCLASS_LLM_INIT_REPLY, &reply_len, &reply);
    return rv;
}


/*
 * Function:
 *      shr_llm_msg_mac_limit_enable
 * Purpose:
 *      Send a LLM message to ARM CPU to disable/enable read reply FIFO
 *      and MAC limit function.
 * Parameters:
 *      mac_limit_enable       (IN) enable/disable mac limit function
 *      reply_fifo_enable      (IN) enable/disable read reply-FIFO function
 */
int shr_llm_msg_uninit(int unit)
{
    int rv = SOC_E_NONE;
    shr_llm_msg_ctrl_init_t msg;
    uint8 *buffer;      
    uint16 send_len;    
    uint16 reply_len;
    uint32 reply;

    sal_memset(&msg, 0x0, sizeof(msg));
    
    /* Pack control message data into DMA buffer */
    buffer     = llm_appl_info[unit].dma_buffer;
    shr_llm_msg_ctrl_init_pack(buffer, &msg);
    send_len = sizeof(shr_llm_msg_ctrl_init_t);
    
    /* Send LLM Start message to uC */
    rv = shr_llm_msg_send_receive(unit, MOS_MSG_SUBCLASS_LLM_UNINIT, send_len, 0, MOS_MSG_SUBCLASS_LLM_UNINIT_REPLY, &reply_len, &reply);
    return rv;
}



/*
 * Function:
 *      shr_llm_msg_mac_limit_enable
 * Purpose:
 *      Send a LLM message to ARM CPU to disable/enable read reply FIFO
 *      and MAC limit function.
 * Parameters:
 *      mac_limit_enable       (IN) enable/disable mac limit function
 *      reply_fifo_enable      (IN) enable/disable read reply-FIFO function
 */
int shr_llm_msg_mac_limit_enable(int unit, uint8 mac_limit_enable, uint8 reply_fifo_enable)
{
    int rv = SOC_E_NONE;
    shr_llm_msg_pon_att_enable_t msg;
    uint8 *buffer;
    uint16 send_len;
    uint16 reply_len;
    uint32 reply;
    uint32 att_val = 0;
    uint32 soc_sand_rv;
	
	SOCDNX_INIT_FUNC_DEFS

    sal_memset(&msg, 0x0, sizeof(msg));
    soc_sand_rv = soc_sand_bitstream_set_field(&att_val, 
                                               SHR_LLM_MSG_PON_ATT_MAC_LIMIT_ENABLE_TYPE_LSB,
                                               SHR_LLM_MSG_PON_ATT_MAC_LIMIT_ENABLE_TYPE_NOF_BITS,
                                               SHR_LLM_MSG_PON_ATT_MAC_LIMIT_ENABLE_GLOBAL);
    SOC_SAND_IF_ERR_EXIT(soc_sand_rv);
    soc_sand_rv = soc_sand_bitstream_set(&att_val, 
                                         SHR_LLM_MSG_PON_ATT_MAC_LIMIT_ENABLE_BIT,
                                         mac_limit_enable);
    SOC_SAND_IF_ERR_EXIT(soc_sand_rv);
    soc_sand_rv = soc_sand_bitstream_set(&att_val, 
                                         SHR_LLM_MSG_PON_ATT_MAC_LIMIT_ENABLE_MASK,
                                         TRUE);
    SOC_SAND_IF_ERR_EXIT(soc_sand_rv);
    soc_sand_rv = soc_sand_bitstream_set(&att_val, 
                                         SHR_LLM_MSG_PON_ATT_REPLY_FIFO_ENABLE_BIT,
                                         reply_fifo_enable);
    SOC_SAND_IF_ERR_EXIT(soc_sand_rv);
    soc_sand_rv = soc_sand_bitstream_set(&att_val, 
                                         SHR_LLM_MSG_PON_ATT_REPLY_FIFO_ENABLE_MASK,
                                         TRUE);
    SOC_SAND_IF_ERR_EXIT(soc_sand_rv);
    msg.att_val = att_val;
    msg.type_of_service = LLM_SERVICE_MAC_LIMIT;

    /* Pack control message data into DMA buffer */
    buffer     = llm_appl_info[unit].dma_buffer;
    shr_llm_msg_pon_att_enable_pack(buffer, &msg);
    send_len = sizeof(shr_llm_msg_pon_att_enable_t);

    /* Send LLM SET message to uC */
    rv = shr_llm_msg_send_receive(unit, MOS_MSG_SUBCLASS_LLM_PON_ATT_SERVICE_ENABLE, send_len, 0, 
                                        MOS_MSG_SUBCLASS_LLM_PON_ATT_SERVICE_ENABLE_REPLY, &reply_len, &reply); 
    SOCDNX_IF_ERR_EXIT(rv);
exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      shr_llm_msg_mac_limit_enable
 * Purpose:
 *      Send a LLM message to ARM CPU to disable/enable read reply FIFO
 *      and MAC limit function.
 * Parameters:
 *      mac_limit_enable       (IN) enable/disable mac limit function
 *      reply_fifo_enable      (IN) enable/disable read reply-FIFO function
 */
int shr_llm_msg_mac_limit_set(int unit, uint8 type, shr_llm_msg_pon_att_t *msg)
{
    int rv = SOC_E_NONE;
    uint8 *buffer;
    uint8 send_subclass;    
    uint16 send_len;
    uint8 reply_subclass;
    uint16 reply_len;
    uint32 reply;

    /* Pack control message data into DMA buffer */
    buffer     = llm_appl_info[unit].dma_buffer;
    shr_llm_msg_pon_att_pack(buffer, msg);
    send_len = sizeof(shr_llm_msg_pon_att_t);

    switch(type) {
        case SHR_LLM_MSG_PON_ATT_MAC_BITMAP_SET:
            send_subclass = MOS_MSG_SUBCLASS_LLM_PON_ATT_MAC_BITMAP_SET;
            reply_subclass = MOS_MSG_SUBCLASS_LLM_PON_ATT_MAC_BITMAP_SET_REPLY;
            break;
            
        case SHR_LLM_MSG_PON_ATT_MAC_BITMAP_INCREASE:
            send_subclass = MOS_MSG_SUBCLASS_LLM_PON_ATT_MAC_BITMAP_INCREASE;
            reply_subclass = MOS_MSG_SUBCLASS_LLM_PON_ATT_MAC_BITMAP_INCREASE_REPLY;
            break;

        case SHR_LLM_MSG_PON_ATT_MAC_BITMAP_DECREASE:
            send_subclass = MOS_MSG_SUBCLASS_LLM_PON_ATT_MAC_BITMAP_DECREASE;
            reply_subclass = MOS_MSG_SUBCLASS_LLM_PON_ATT_MAC_BITMAP_DECREASE_REPLY;
            break;            
            
		default:
			return SOC_E_PARAM;
        }     
            

    /* Send LLM SET message to uC */
    rv = shr_llm_msg_send_receive(unit, send_subclass, send_len, 0, 
                                        reply_subclass, &reply_len, &reply); 
    return rv;
}


/*
 * Function:
 *      shr_llm_msg_mac_limit_get
 * Purpose:
 *      Send a LLM message to ARM CPU to Get MAC limit value.
 * Parameters:
 *      msg       (IN/OUT) MAC limit configuration.
 */
int shr_llm_msg_mac_limit_get(int unit, shr_llm_msg_pon_att_t *msg, shr_llm_msg_pon_att_t *rsp)
{
    int rv = SOC_E_NONE;
    uint8 *buffer;
    uint8 send_subclass;    
    uint16 send_len;
    uint8 reply_subclass;
    uint16 reply_len;
    uint32 reply;
    

    /* Pack control message data into DMA buffer */
    buffer     = llm_appl_info[unit].dma_buffer;
    shr_llm_msg_pon_att_pack(buffer, msg);
    send_subclass = MOS_MSG_SUBCLASS_LLM_PON_ATT_MAC_BITMAP_GET;
    send_len = sizeof(shr_llm_msg_pon_att_t);
    reply_subclass = MOS_MSG_SUBCLASS_LLM_PON_ATT_MAC_BITMAP_GET_REPLY;

    /* Send LLM SET message to uC */
    rv = shr_llm_msg_send_receive(unit, send_subclass, send_len, 0, 
                                        reply_subclass, &reply_len, &reply); 

    /* Pack control message data into DMA buffer */
    sal_memset(rsp, 0x0, sizeof(*rsp));
    buffer     = llm_appl_info[unit].dma_buffer;
    shr_llm_msg_pon_att_unpack(buffer, rsp);

    return rv;
}



/*
 * Function:
 *      shr_llm_msg_mac_rx_mode_set_send_receive
 * Purpose:
 *      Send a LLM message to ARM CPU to set the rx mode.
 * Parameters:
 *      mode       0 -> DSP paring mdoe, 1 -> MACT event FIFO mode
 */
int shr_llm_msg_mac_rx_mode_set(int unit, uint32 mode)
{
    int rv = SOC_E_NONE;
    shr_llm_msg_pon_att_rx_mode_t msg;
    uint8 *buffer;
    uint16 send_len;
    uint16 reply_len;
    uint32 reply;

    sal_memset(&msg, 0x0, sizeof(msg));
    msg.mode = mode;    

    /* Pack control message data into DMA buffer */
    buffer     = llm_appl_info[unit].dma_buffer;
    shr_llm_msg_pon_att_rx_mode_pack(buffer, &msg);
    send_len = sizeof(shr_llm_msg_pon_att_rx_mode_t);

    /* Send LLM SET message to uC */
    rv = shr_llm_msg_send_receive(unit, MOS_MSG_SUBCLASS_LLM_PON_ATT_MAC_RX_MODE_SET, send_len, 0, 
                                        MOS_MSG_SUBCLASS_LLM_PON_ATT_MAC_RX_MODE_SET_REPLY, &reply_len, &reply); 
    return rv;
}

/*
 * Function:
 *      shr_llm_msg_mac_move_set_send_receive
 * Purpose:
 *      Send a LLM message to ARM CPU to set the MAC move.
 * Parameters:
 *      port        port number
 *      value       0 -> disable the MAC move, 1 -> enable the MAC move
 */
int shr_llm_msg_mac_move_set(int unit, uint32 port, uint32 value)
{
    int rv = SOC_E_NONE;
    shr_llm_msg_pon_att_mac_move_t msg;
    uint8 *buffer;
    uint16 send_len;
    uint16 reply_len;
    uint32 reply;

    sal_memset(&msg, 0x0, sizeof(msg));
    msg.port = port;
    msg.value = value;    

    /* Pack control message data into DMA buffer */
    buffer     = llm_appl_info[unit].dma_buffer;
    shr_llm_msg_pon_att_mac_move_pack(buffer, &msg);
    send_len = sizeof(shr_llm_msg_pon_att_mac_move_t);

    /* Send LLM SET message to uC */
    rv = shr_llm_msg_send_receive(unit, MOS_MSG_SUBCLASS_LLM_PON_ATT_MAC_MOVE_SET, send_len, 0, 
                                        MOS_MSG_SUBCLASS_LLM_PON_ATT_MAC_MOVE_SET_REPLY, &reply_len, &reply); 
    return rv;
}


/*
 * Function:
 *      shr_llm_msg_mac_move_get_send_receive
 * Purpose:
 *      Send a LLM message to ARM CPU to get the MAC move.
 * Parameters:
 *      port        port number
 *      value       0 -> disable the MAC move, 1 -> enable the MAC move
 */
int shr_llm_msg_mac_move_get(int unit, uint32 port, uint32 *value)
{
    int rv = SOC_E_NONE;
    shr_llm_msg_pon_att_mac_move_t msg;
    uint8 *buffer;
    uint16 send_len;
    uint16 reply_len;
    uint32 reply;

    sal_memset(&msg, 0x0, sizeof(msg));
    msg.port = port;

    /* Pack control message data into DMA buffer */
    buffer     = llm_appl_info[unit].dma_buffer;
    shr_llm_msg_pon_att_mac_move_pack(buffer, &msg);
    send_len = sizeof(shr_llm_msg_pon_att_mac_move_t);

    /* Send LLM GET message to uC */
    rv = shr_llm_msg_send_receive(unit, MOS_MSG_SUBCLASS_LLM_PON_ATT_MAC_MOVE_GET, send_len, 0, 
                                        MOS_MSG_SUBCLASS_LLM_PON_ATT_MAC_MOVE_GET_REPLY, &reply_len, &reply);
    
    if (rv != SOC_E_NONE) {
        return rv;
    }
    
    /* Pack control message data into DMA buffer */
    sal_memset(&msg, 0x0, sizeof(msg));
    buffer     = llm_appl_info[unit].dma_buffer;
    shr_llm_msg_pon_att_mac_move_unpack(buffer, &msg);
    *value = msg.value;
 
    return rv;
}

/*
 * Function:
 *      shr_llm_msg_mac_move_event_set
 * Purpose:
 *      Disable/enable mac move event processing.
 * Parameters:
 *      event_id    event ID
 *      value       0 -> disable, 1 -> enable
 */
int shr_llm_mac_move_event_set(int unit, uint32 event_id, uint32 value)
{
    uint32 soc_sand_rv;

    if ((value != 0) && (value != 1)) {
        return SOC_E_PARAM;
    }

    if (event_id != _LLM_EVENT_MAC_MOVE) {
        return SOC_E_PARAM;
    }
    soc_sand_rv = soc_sand_bitstream_set(&llm_appl_info[unit].event_enable, event_id, value);
    return soc_sand_rv;
}

/*
 * Function:
 *      shr_llm_mac_move_event_enable_get
 * Purpose:
 *      Get mac move event status.
 * Parameters:
 *      event_id    event ID
 *      value       0 -> disable, 1 -> enable
 */
int shr_llm_mac_move_event_get(int unit, uint32 event_id, uint32 *value)
{
    uint32 soc_sand_rv;

    if (event_id != _LLM_EVENT_MAC_MOVE) {
        return SOC_E_PARAM;
    }

    soc_sand_rv = soc_sand_bitstream_get_field(&llm_appl_info[unit].event_enable, event_id, 1, value);
    return soc_sand_rv;
}




/*
 * Function:
 *      shr_llm_msg_mac_limit_update_send_receive
 * Purpose:
 *      Send a LLM message to ARM CPU to update pon attribution.
 *
 * Parameters:
 *      serv_type     (IN) service type, vmac or mac limit
 *      data_type     (IN) data type, CPU request request value or vmac encoding
 *      data          (IN) A pointer to data
 *      data_len      (IN) data length
 */
int shr_llm_msg_pon_attr_set(int unit, uint32 serv_type, uint32 data_type, void *data)
{
    int rv = SOC_E_NONE;
    shr_llm_msg_pon_att_update_t msg;
    uint8 *buffer, *buffer_ptr;
    uint16 send_len;
    uint16 reply_len;
    uint32 reply;
    shr_llm_msg_pon_att_multi_l2_entry_t *multi_l2_entry;
	
	SOCDNX_INIT_FUNC_DEFS

    if (NULL == data) {
        return SOC_E_PARAM;
    }

    sal_memset(&msg, 0x0, sizeof(msg));
    msg.type_of_service = serv_type;
    msg.data_type = data_type;
    switch (data_type) {      
        case SHR_LLM_MSG_PON_ATT_UPDATE_MAC_ENTRY:
            msg.data_len  = sizeof(shr_llm_msg_pon_att_l2_entry_t);
            sal_memcpy(&msg.data.l2_entry, data, sizeof(shr_llm_msg_pon_att_l2_entry_t));
            break;
        case SHR_LLM_MSG_PON_ATT_UPDATE_VMAC_ENCODE:
            msg.data_len  = sizeof(shr_llm_msg_pon_att_vmac_encode_t);
            sal_memcpy(&msg.data.vmac_encode, data, sizeof(shr_llm_msg_pon_att_vmac_encode_t));
            break;
        case SHR_LLM_MSG_PON_ATT_UPDATE_VMAC_PREFIX:
            msg.data_len  = sizeof(shr_llm_msg_pon_att_l2_entry_t);
            sal_memcpy(&msg.data.vmac_prefix, data, sizeof(shr_llm_msg_pon_att_vmac_prefix_t));
            break;
        case SHR_LLM_MSG_PON_ATT_UPDATE_MULTI_MAC_ENTRY:
            multi_l2_entry = (shr_llm_msg_pon_att_multi_l2_entry_t *)data;
            msg.data.multi_l2_entry.l2_entry_count = multi_l2_entry->l2_entry_count;
            msg.data_len  = sizeof(uint32) + multi_l2_entry->l2_entry_count * sizeof(shr_llm_msg_pon_att_l2_entry_t);
            sal_memcpy(&msg.data.multi_l2_entry.l2_entry, multi_l2_entry->l2_entry, 
                multi_l2_entry->l2_entry_count * sizeof(shr_llm_msg_pon_att_l2_entry_t));
            break;
        default:
            return SOC_E_PARAM;
    }
    /* Pack control message data into DMA buffer */
    buffer     = llm_appl_info[unit].dma_buffer;
    buffer_ptr = shr_llm_msg_pon_att_update_pack(buffer, &msg);
    send_len = buffer_ptr - buffer;

    /* Send LLM SET message to uC */
    rv = shr_llm_msg_send_receive(unit, MOS_MSG_SUBCLASS_LLM_PON_ATT_UPDATE, send_len, 0, 
                                        MOS_MSG_SUBCLASS_LLM_PON_ATT_UPDATE_REPLY, &reply_len, &reply);
    SOCDNX_IF_ERR_EXIT(rv);
exit:
    SOCDNX_FUNC_RETURN;
}

#endif /*BCM_DPP_SUPPORT*/
