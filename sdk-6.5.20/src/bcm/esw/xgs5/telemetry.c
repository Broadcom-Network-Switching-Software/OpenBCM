/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Telemetry - Common telemetry infrastructure interface
 * Purpose: APIs to configure telemetry instances
 *          which will be used by instrumentation applications.
 */

#if defined(INCLUDE_TELEMETRY)

#include <shared/bsl.h>
#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/uc.h>
#include <shared/alloc.h>
#include <soc/shared/shr_pkt.h>
#include <soc/shared/st_pack.h>
#include <soc/tomahawk.h>

#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/module.h>

#include <bcm_int/esw/telemetry.h>
#include <bcm_int/esw/xgs5.h>
#include <bcm_int/esw/switch.h>
#include <bcm_int/esw/collector.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/st_feature.h>
#include <bcm_int/esw/st_sdk_pack.h>
#include <bcm_int/esw/st_sdk_msg.h>

/*Global to hold the ifa firmware version for feature compatibility check */
uint32 st_firmware_version = 0;

/* Telemetry global information */
_bcm_telemetry_global_info_t *telemetry_global_info[BCM_MAX_NUM_UNITS] = {0};

/* List of telemetry instances info*/
_bcm_telemetry_instance_info_t *telemetry_instances_info_list[BCM_MAX_NUM_UNITS]
                               = {0};

/* Telemetry module invalid value definition */
#define BCM_TELEMETRY_INVALID_VALUE -1

/* Default max export length is 1500 bytes */
#define BCM_TELEMETRY_DEFAULT_MAX_EXPORT_LENGTH 1500

/******************************************************
*                                                     *
*                 UTILITY FUNCTIONS                   *
 */

/*
 * Function:
 *     _bcm_xgs5_telemetry_is_instance_exists
 * Purpose:
 *     Checks whether a telemetry instance exists or not.
 * Parameters:
 *     unit          - (IN) BCM device number
 *     telemetry_instance_id   - (IN) The telemetry instance ID
 *     instance_index          - (IN) If exists, Index of that instance
 *     first_free_index        - (IN) Index of first free instance
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_xgs5_telemetry_is_instance_exists(int unit,
                           int telemetry_instance_id,
                           int *instance_index,
                           int *first_free_index)
{
    int i = 0, rv = BCM_E_NONE, match_found = 0;
    _bcm_telemetry_instance_info_t *telemetry_instance_list_node = NULL;

    for (i=0; i< BCM_INT_TELEMETRY_MAX_INSTANCES; i++) {
       telemetry_instance_list_node =
               &(telemetry_instances_info_list[unit][i]);
       if(telemetry_instance_list_node->instance_id == telemetry_instance_id)
       {
           match_found = 1;
           *instance_index = i;
           break;
       } else if (telemetry_instance_list_node->instance_id ==
                  BCM_TELEMETRY_INVALID_INDEX) {
           *first_free_index = i;
       }
    }

    if(match_found == 0)
    {
        rv = BCM_E_NOT_FOUND;
    }
    LOG_DEBUG(BSL_LS_BCM_TELEMETRY,
            (BSL_META_U(unit,
                        "TELEMETRY(unit %d) Instance exist check: %s" 
                        " (Telemetry instance=0x%x).\n"), unit, FUNCTION_NAME(),
                         telemetry_instance_id));
    return rv;
}

/******************************************************
*                                                     *
*        ST EMBEDDED APP MESSAGING FUNCTIONS          *
 */

/*
 * Function:
 *      _bcm_xgs5_st_convert_uc_error
 * Purpose:
 *      Converts uController error code to corresponding BCM error code.
 * Parameters:
 *      uc_rv  - (IN) uController error code to convert.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 * Notes:
 */
STATIC int
_bcm_xgs5_st_convert_uc_error(uint32 uc_rv)
{
    int rv = BCM_E_NONE;

    switch(uc_rv) {
    case SHR_ST_UC_E_NONE:
        rv = BCM_E_NONE;
        break;
    case SHR_ST_UC_E_INTERNAL:
        rv = BCM_E_INTERNAL;
        break;
    case SHR_ST_UC_E_MEMORY:
        rv = BCM_E_MEMORY;
        break;
    case SHR_ST_UC_E_UNIT:
        rv = BCM_E_UNIT;
        break;
    case SHR_ST_UC_E_PARAM:
        rv = BCM_E_PARAM;
        break;
    case SHR_ST_UC_E_EMPTY:
        rv = BCM_E_EMPTY;
        break;
    case SHR_ST_UC_E_FULL:
        rv = BCM_E_FULL;
        break;
    case SHR_ST_UC_E_NOT_FOUND:
        rv = BCM_E_NOT_FOUND;
        break;
    case SHR_ST_UC_E_EXISTS:
        rv = BCM_E_EXISTS;
        break;
    case SHR_ST_UC_E_TIMEOUT:
        rv = BCM_E_TIMEOUT;
        break;
    case SHR_ST_UC_E_BUSY:
        rv = BCM_E_BUSY;
        break;
    case SHR_ST_UC_E_FAIL:
        rv = BCM_E_FAIL;
        break;
    case SHR_ST_UC_E_DISABLED:
        rv = BCM_E_DISABLED;
        break;
    case SHR_ST_UC_E_BADID:
        rv = BCM_E_BADID;
        break;
    case SHR_ST_UC_E_RESOURCE:
        rv = BCM_E_RESOURCE;
        break;
    case SHR_ST_UC_E_CONFIG:
        rv = BCM_E_CONFIG;
        break;
    case SHR_ST_UC_E_UNAVAIL:
        rv = BCM_E_UNAVAIL;
        break;
    case SHR_ST_UC_E_INIT:
        rv = BCM_E_INIT;
        break;
    case SHR_ST_UC_E_PORT:
        rv = BCM_E_PORT;
        break;
    default:
        rv = BCM_E_INTERNAL;
        break;
    }

    return rv;
}

STATIC int
_bcm_xgs5_st_msg_send_receive(int unit, uint8 s_subclass,
                               uint16 s_len, uint32 s_data,
                               uint8 r_subclass, uint16 *r_len,
                               sal_usecs_t timeout)
{
    int rv;
    mos_msg_data_t send, reply;

    uint32 uc_rv;
    _bcm_telemetry_global_info_t *telemetry_config_per_unit =
                           TELEMETRY_GLOBAL_INFO_GET(unit);
    uint8 *dma_buffer;
    int dma_buffer_len;

    dma_buffer = telemetry_config_per_unit->dma_buffer;
    dma_buffer_len = telemetry_config_per_unit->dma_buffer_len;

    /* Flush DMA memory */
    if (MOS_MSG_DMA_MSG(s_subclass)) {
        soc_cm_sflush(unit, dma_buffer, s_len);
    }

    /* Invalidate DMA memory to read */
    if (MOS_MSG_DMA_MSG(r_subclass)) {
        soc_cm_sinval(unit, dma_buffer, dma_buffer_len);
    }

    sal_memset(&send, 0, sizeof(send));
    sal_memset(&reply, 0, sizeof(reply));
    send.s.mclass = MOS_MSG_CLASS_ST;
    send.s.subclass = s_subclass;
    send.s.len = bcm_htons(s_len);

    /*
     * Set 'data' to DMA buffer address if a DMA operation is
     * required for send or receive.
     */
    if (MOS_MSG_DMA_MSG(s_subclass) ||
        MOS_MSG_DMA_MSG(r_subclass)) {
        send.s.data = bcm_htonl(soc_cm_l2p(unit, dma_buffer));
    } else {
        send.s.data = bcm_htonl(s_data);
    }

    rv = soc_cmic_uc_msg_send_receive(unit, telemetry_config_per_unit->uC,
                                      &send, &reply,
                                      timeout);
    if (rv != SOC_E_NONE){
        return BCM_E_INTERNAL;
    }

    /* Convert ST uController error code to BCM */
    uc_rv  = bcm_ntohl(reply.s.data);
    rv     = _bcm_xgs5_st_convert_uc_error(uc_rv);

    *r_len = bcm_ntohs(reply.s.len);

    /*Check reply class and subclass*/
    if((rv == SOC_E_NONE) && ((reply.s.mclass != MOS_MSG_CLASS_ST) ||
        (reply.s.subclass != r_subclass)))
    {
        return BCM_E_INTERNAL;
    }
    return rv;
}

STATIC int
bcm_xgs5_st_eapp_instance_get_msg (int unit,
                               int instance_id,
                               bcm_telemetry_config_t *telemetry_config,
                               int config_count)
{
    shr_st_msg_ctrl_instance_get_t instance_get_msg;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    _bcm_telemetry_global_info_t  *telemetry_per_unit_global_info =
                                    TELEMETRY_GLOBAL_INFO_GET(unit);
    int i,j;
    bcm_telemetry_config_t *object_config = NULL;

    sal_memset(&instance_get_msg, 0, sizeof(instance_get_msg));
    BCM_IF_ERROR_RETURN(_bcm_xgs5_st_msg_send_receive(unit,
                                    MOS_MSG_SUBCLASS_ST_INSTANCE_GET,
                                    instance_id, 0,
                                    MOS_MSG_SUBCLASS_ST_INSTANCE_GET_REPLY,
                                    &reply_len, 
                                    SHR_ST_MAX_UC_MSG_TIMEOUT));

    buffer_req      = telemetry_per_unit_global_info->dma_buffer;
    buffer_ptr      = shr_st_msg_ctrl_instance_get_unpack(buffer_req,
                                                        &instance_get_msg);
    buffer_len      = buffer_ptr - buffer_req;

    if (reply_len != buffer_len) {
        LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                  (BSL_META_U(unit,
                  "TELEMETRY(unit %d) Error: Port "
                  " get failed from BTE\n"), unit));
        return BCM_E_INTERNAL;
    }

    if (config_count != instance_get_msg.num_ports)
    {
        LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                  (BSL_META_U(unit,
                "TELEMETRY(unit %d) Error: Num Ports %d"
                " and config count %d mismatch in instance(%d) get from BTE\n"),
                  unit, instance_get_msg.num_ports, config_count, instance_id));
        return BCM_E_INTERNAL;

    }

    for (i= 0; i <  instance_get_msg.num_ports; i++)
    {
        object_config = &(telemetry_config[i]);
        BCM_GPORT_LOCAL_SET(object_config->gport,instance_get_msg.lports[i]);
        object_config->component_id = instance_get_msg.component_id;

        sal_memset(object_config->object_type, bcmTelemetryObjectTypeNone,
                   sizeof(object_config->object_type));
        j = 0;
        if (instance_get_msg.stat_types[i] & SHR_ST_STAT_TYPE_IF_INGRESS)
        {
            object_config->object_type[j] = bcmTelemetryObjectTypeIntfIngress;
            j++;
        }
        
        if (instance_get_msg.stat_types[i] & SHR_ST_STAT_TYPE_IF_EGRESS)
        {
            object_config->object_type[j] = bcmTelemetryObjectTypeIntfEgress;
            j++;
        }
        if (instance_get_msg.stat_types[i] & SHR_ST_STAT_TYPE_QUEUE_EGRESS)
        {
            object_config->object_type[j] =
                                     bcmTelemetryObjectTypeIntfEgressQueue;
            j++;
        }
        if (instance_get_msg.stat_types[i] & SHR_ST_STAT_TYPE_IF_INGRESS_ERRORS)
        {
            object_config->object_type[j] =
                                     bcmTelemetryObjectTypeIntfIngressError;
            j++;
        }

        sal_memcpy(object_config->port_name, instance_get_msg.if_names[i],
                               instance_get_msg.if_name_lengths[i]);
        object_config->port_name[instance_get_msg.if_name_lengths[i]] = '\0';
    }

    return BCM_E_NONE;
}
STATIC int
bcm_xgs5_st_eapp_instance_create_msg (int unit,
                               int instance_id,
                               bcm_telemetry_config_t *telemetry_config,
                               int config_count)
{
    shr_st_msg_ctrl_instance_create_t instance_create_msg;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    _bcm_telemetry_global_info_t  *telemetry_per_unit_global_info =
                                    TELEMETRY_GLOBAL_INFO_GET(unit);
    int i,j;
    bcm_telemetry_config_t *object_config = NULL;
    uint16 lports[SHR_ST_MAX_PORTS], config_lport = 0;
    uint32 stat_types = 0;
    char *port_name;

    sal_memset(&instance_create_msg, 0, sizeof(instance_create_msg));
   
    for (i= 0; i < config_count; i++)
    {
        object_config = &(telemetry_config[i]);
        config_lport = BCM_GPORT_LOCAL_GET(object_config->gport);
        
        lports[i] = config_lport;
        stat_types = 0;
        for (j = 0; j < bcmTelemetryObjectTypeCount; j++)
        {
            if(object_config->object_type[j] ==
                                    bcmTelemetryObjectTypeIntfIngress)
            {
                stat_types |= SHR_ST_STAT_TYPE_IF_INGRESS;
            }
            if(object_config->object_type[j] ==
                                    bcmTelemetryObjectTypeIntfEgress)
            {
                stat_types |= SHR_ST_STAT_TYPE_IF_EGRESS;
            }
            if(object_config->object_type[j] ==
                                    bcmTelemetryObjectTypeIntfEgressQueue)
            {
                stat_types |= SHR_ST_STAT_TYPE_QUEUE_EGRESS;
            }
            if(object_config->object_type[j] ==
                                    bcmTelemetryObjectTypeIntfIngressError)
            {
                stat_types |= SHR_ST_STAT_TYPE_IF_INGRESS_ERRORS;
            }
        }
        instance_create_msg.stat_types[i] = stat_types;
        instance_create_msg.component_id = object_config->component_id;
        port_name = (char*)object_config->port_name;
        instance_create_msg.if_name_lengths[i] =
                                       sal_strlen(port_name);
        sal_memcpy(&(instance_create_msg.if_names[i]), port_name,
                                          sal_strlen(port_name));
    }
    instance_create_msg.num_ports = config_count;
    instance_create_msg.instance_id = instance_id;
    sal_memcpy(instance_create_msg.lports, 
               lports,
               (instance_create_msg.num_ports * sizeof(uint16)));

    buffer_req      = telemetry_per_unit_global_info->dma_buffer;
    buffer_ptr      = shr_st_msg_ctrl_instance_create_pack(buffer_req,
                                                        &instance_create_msg);
    buffer_len      = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN(_bcm_xgs5_st_msg_send_receive(unit,
                                    MOS_MSG_SUBCLASS_ST_INSTANCE_CREATE,
                                    buffer_len, 0,
                                    MOS_MSG_SUBCLASS_ST_INSTANCE_CREATE_REPLY,
                                    &reply_len, 
                                    SHR_ST_MAX_UC_MSG_TIMEOUT));
    return BCM_E_NONE;
}

STATIC int
bcm_xgs5_st_eapp_instance_delete_msg (int unit, int instance_id)
{
    uint16 reply_len = 0;

    BCM_IF_ERROR_RETURN(_bcm_xgs5_st_msg_send_receive(unit, 
                                    MOS_MSG_SUBCLASS_ST_INSTANCE_DELETE,
                                    instance_id, 0,
                                    MOS_MSG_SUBCLASS_ST_INSTANCE_DELETE_REPLY,
                                    &reply_len, 
                                    SHR_ST_MAX_UC_MSG_TIMEOUT));
    return BCM_E_NONE;
}

STATIC int
bcm_xgs5_st_eapp_port_get_msg(int unit)
{
    shr_st_msg_ctrl_ports_get_t ports_get_msg;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    _bcm_telemetry_global_info_t  *telemetry_per_unit_global_info =
                                    TELEMETRY_GLOBAL_INFO_GET(unit);

    sal_memset(&ports_get_msg, 0, sizeof(ports_get_msg));

    BCM_IF_ERROR_RETURN(_bcm_xgs5_st_msg_send_receive(unit,
                                    MOS_MSG_SUBCLASS_ST_PORTS_GET,
                                    0, 0,
                                    MOS_MSG_SUBCLASS_ST_PORTS_GET_REPLY,
                                    &reply_len, 
                                    SHR_ST_MAX_UC_MSG_TIMEOUT));

    buffer_req      = telemetry_per_unit_global_info->dma_buffer;
    buffer_ptr      = shr_st_msg_ctrl_ports_get_unpack(buffer_req,
                                                        &ports_get_msg);
    buffer_len      = buffer_ptr - buffer_req;

    if (reply_len != buffer_len) {
        LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                  (BSL_META_U(unit,
                  "TELEMETRY(unit %d) Error: Port "
                  " get failed from BTE\n"), unit));
        return BCM_E_INTERNAL;
    }

    telemetry_per_unit_global_info->num_ports = ports_get_msg.num_ports;
    sal_memcpy(telemetry_per_unit_global_info->lports, ports_get_msg.lports,
               (ports_get_msg.num_ports * sizeof(uint16)));

    return BCM_E_NONE;
}

STATIC int
bcm_xgs5_st_eapp_port_delete_msg (int unit,
                               bcm_telemetry_config_t *telemetry_config,
                               int config_count)
{
    shr_st_msg_ctrl_ports_delete_t ports_delete_msg;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL, match_found = 0;
    uint16 buffer_len = 0, reply_len = 0;
    _bcm_telemetry_global_info_t  *telemetry_per_unit_global_info =
                                    TELEMETRY_GLOBAL_INFO_GET(unit);
    int i,j,k=0,next_pos = 0;
    bcm_telemetry_config_t *object_config = NULL;
    uint16 config_lport = 0, list_lport = 0;
    uint16 lports_list[SHR_ST_MAX_PORTS], del_lports_list[SHR_ST_MAX_PORTS];
    uint16 num_ports;

    sal_memset(&ports_delete_msg, 0, sizeof(ports_delete_msg));
    num_ports = telemetry_per_unit_global_info->num_ports;
  
    sal_memcpy(lports_list, telemetry_per_unit_global_info->lports,
               (num_ports * sizeof(uint16))); 
    /* Currently FW does not support multi instance export hence below
       port add/delete will be operating in a single instance context */
 
    /*Deleting from ports list which does not present in new config */
    for (i= 0; i < num_ports; i++)
    {
        match_found = 0;
        list_lport = lports_list[i];
        for (j= 0; j < config_count; j++) {
            object_config = &(telemetry_config[j]);
            config_lport = BCM_GPORT_LOCAL_GET(object_config->gport);
            if (config_lport == list_lport) {
                match_found = 1;
                break;
            }
        }
        if(match_found == 0)
        {
            lports_list[i] = 0;
            telemetry_per_unit_global_info->num_ports--;
            del_lports_list[k] = list_lport;
            k++;
        }
    }
    /*Construct the original port list without any gaps */
    sal_memset(telemetry_per_unit_global_info->lports, 0,
               (num_ports * sizeof(uint16)));
    for (i=0; i< num_ports; i++)
    {
        if(lports_list[i] != 0)
        {
            telemetry_per_unit_global_info->lports[next_pos] =
                                    lports_list[i];
            next_pos++;
        }
    }

    ports_delete_msg.num_ports = k;
    sal_memcpy(ports_delete_msg.lports,
               del_lports_list,
               (ports_delete_msg.num_ports * sizeof(uint16)));

    buffer_req      = telemetry_per_unit_global_info->dma_buffer;
    buffer_ptr      = shr_st_msg_ctrl_ports_delete_pack(buffer_req,
                                                        &ports_delete_msg);
    buffer_len      = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN(_bcm_xgs5_st_msg_send_receive(unit,
                                    MOS_MSG_SUBCLASS_ST_PORTS_DELETE,
                                    buffer_len, 0,
                                    MOS_MSG_SUBCLASS_ST_PORTS_DELETE_REPLY,
                                    &reply_len, 
                                    SHR_ST_MAX_UC_MSG_TIMEOUT));
    return BCM_E_NONE;
}

STATIC int
bcm_xgs5_st_eapp_port_add_msg (int unit,
                               bcm_telemetry_config_t *telemetry_config,
                               int config_count)
{
    shr_st_msg_ctrl_ports_add_t ports_add_msg;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL, match_found = 0;
    uint16 buffer_len = 0, reply_len = 0;
    _bcm_telemetry_global_info_t  *telemetry_per_unit_global_info =
                                    TELEMETRY_GLOBAL_INFO_GET(unit);
    int i,j;
    bcm_telemetry_config_t *object_config = NULL;
    uint16 config_lport = 0;
    uint16 pports[SHR_ST_MAX_PORTS] = {0}, mports[SHR_ST_MAX_PORTS] = {0};
    uint16 num_ports, phy_port = 0;
    soc_info_t *si = &SOC_INFO(unit);

    sal_memset(&ports_add_msg, 0, sizeof(ports_add_msg));
    num_ports = telemetry_per_unit_global_info->num_ports;
   
    /* Currently FW does not support multi instance export hence below
       port add/delete will be operating in a single instance context */
 
    /*Adding new ports to existing ports list */
    for (i= 0; i < config_count; i++)
    {
        match_found = 0;
        object_config = &(telemetry_config[i]);
        config_lport = BCM_GPORT_LOCAL_GET(object_config->gport);
        for (j= 0; j < num_ports; j++) {
            if (config_lport == telemetry_per_unit_global_info->lports[j]) {
                match_found = 1;
                break;
            }
        }
        if(match_found == 0)
        {
            telemetry_per_unit_global_info->lports
                     [telemetry_per_unit_global_info->num_ports] = config_lport;
            phy_port = si->port_l2p_mapping[config_lport];
            pports[telemetry_per_unit_global_info->num_ports] = phy_port;
            mports[telemetry_per_unit_global_info->num_ports] =
            si->port_p2m_mapping[phy_port];
            telemetry_per_unit_global_info->num_ports++; 
        }
    }

    ports_add_msg.num_ports = telemetry_per_unit_global_info->num_ports;
    sal_memcpy(ports_add_msg.lports,
               telemetry_per_unit_global_info->lports,
               (ports_add_msg.num_ports * sizeof(uint16)));
    sal_memcpy(ports_add_msg.pports, pports,
               (ports_add_msg.num_ports * sizeof(uint16)));
    sal_memcpy(ports_add_msg.mports, mports,
               (ports_add_msg.num_ports * sizeof(uint16)));

    buffer_req      = telemetry_per_unit_global_info->dma_buffer;
    buffer_ptr      = shr_st_msg_ctrl_ports_add_pack(buffer_req,
                                                        &ports_add_msg);
    buffer_len      = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN(_bcm_xgs5_st_msg_send_receive(unit,
                                    MOS_MSG_SUBCLASS_ST_PORTS_ADD,
                                    buffer_len, 0,
                                    MOS_MSG_SUBCLASS_ST_PORTS_ADD_REPLY,
                                    &reply_len, 
                                    SHR_ST_MAX_UC_MSG_TIMEOUT));
    return BCM_E_NONE;
}

STATIC int
_bcm_xgs5_st_eapp_collector_encap_build_pack (int unit,
        bcm_collector_info_t *collector_info,
        st_sdk_msg_ctrl_collector_create_t *collector_create_msg)
{
    uint8          *buffer  = collector_create_msg->encap;
    uint8          *cur_ptr;
    uint16         etype = 0;
    shr_udp_header_t  udp;
    shr_ipv4_header_t ipv4;
    shr_l2_header_t   l2;
    uint16         ip_offset = 0;
    uint16         udp_offset = 0;

    sal_memset(&udp, 0, sizeof(udp));
    sal_memset(&ipv4, 0, sizeof(ipv4));
    sal_memset(&l2, 0, sizeof(l2));


    /*
     * Get necessary headers/labels information.
     *
     * Following order is important since some headers/labels
     * may depend on previous header/label information.
     */
    if (collector_info->transport_type == bcmCollectorTransportTypeIpv4Udp) {
        BCM_IF_ERROR_RETURN
            (_bcm_xgs5_collector_udp_header_get(unit, collector_info, &udp));
    }

    if (collector_info->transport_type == bcmCollectorTransportTypeIpv4Udp) {
        BCM_IF_ERROR_RETURN
            (_bcm_xgs5_collector_ip_headers_get(unit, collector_info,
                                          &etype,
                                          &ipv4));
    }

    /* Always build L2 Header */
    BCM_IF_ERROR_RETURN
        (_bcm_xgs5_collector_l2_header_get(unit, collector_info, etype, &l2));

    /*
     * Pack header/labels into given buffer (network packet format).
     *
     * Following packing order must be followed to correctly
     * build the packet encapsulation.
     */
    cur_ptr = buffer;

    /* L2 Header is always present */
    cur_ptr = shr_l2_header_pack(cur_ptr, &l2);

    if (collector_info->transport_type == bcmCollectorTransportTypeIpv4Udp) {
        /*
         * IP offset
         */
        ip_offset = cur_ptr - buffer;

        cur_ptr = shr_ipv4_header_pack(cur_ptr, &ipv4);
    }

    if (collector_info->transport_type == bcmCollectorTransportTypeIpv4Udp) {
        /*
         * UDP offset
         */
        udp_offset = cur_ptr - buffer;
        cur_ptr = shr_udp_header_pack(cur_ptr, &udp);
    }


    /* Set export pkt encapsulation length */
    collector_create_msg->encap_length = cur_ptr - buffer;

    /* Set export pkt IP base offset */
    collector_create_msg->ip_offset = ip_offset;

    /* Set export pkt IP Base checksum */
    collector_create_msg->ip_base_checksum =
             shr_initial_chksum_get(SHR_IPV4_HEADER_LENGTH, buffer + ip_offset);

    /* Set export pkt UDP Base checksum */
    if (collector_info->transport_type == bcmCollectorTransportTypeIpv4Udp) {
        collector_create_msg->udp_base_checksum =
                              shr_udp_initial_checksum_get(0, &ipv4, NULL, NULL, &udp);
    }
    /* Set export pkt UDP base offset */
    collector_create_msg->udp_offset = udp_offset;

    return BCM_E_NONE;
}

STATIC int
bcm_xgs5_st_eapp_collector_create_msg (int unit,
                    bcm_collector_t collector_id, int export_profile_id)
{
    st_sdk_msg_ctrl_collector_create_t collector_create_msg;
    bcm_collector_info_t collector_info;
    bcm_collector_export_profile_t export_profile_info;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    _bcm_telemetry_global_info_t  *telemetry_per_unit_global_info =
                                    TELEMETRY_GLOBAL_INFO_GET(unit);
    int rv = BCM_E_NONE;

    sal_memset(&collector_info, 0, sizeof(collector_info));
    /* Get the collector info for this collector id */
    rv = bcm_esw_collector_get(unit, collector_id, &collector_info);

    /* Return BCM_E_NOT_FOUND if not found */
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if (SOC_IS_TOMAHAWK3(unit)) {
        if (collector_info.protobuf.system_id_length == 0)
        {
            LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                    (BSL_META_U(unit,
                                "TELEMETRY(unit %d) Error: System ID "
                                " was not configured\n"), unit));
            return BCM_E_CONFIG;
        }

        /* Send the system ID to FW */
        rv = _bcm_xgs5_telemetry_system_id_set(unit,
                collector_info.protobuf.system_id_length,
                collector_info.protobuf.system_id);

        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }

    /* Get the export profile info for this profile id */
    rv = bcm_esw_collector_export_profile_get(unit, export_profile_id,
                                              &export_profile_info);

    /* Return BCM_E_NOT_FOUND if not found */
    if (BCM_FAILURE(rv)) {
        return rv;
    }
    if (export_profile_info.export_interval != _SHR_ST_EXPORT_INTERVAL_USECS) {
        LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                  (BSL_META_U(unit,
                              "TELEMETRY(unit %d) Error: Invalid export_interval=%u\n"),
                   unit, export_profile_info.export_interval));
        return BCM_E_PARAM;
    }

    if (collector_info.transport_type != bcmCollectorTransportTypeIpv4Udp)
    {
        LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                  (BSL_META_U(unit,
                  "TELEMETRY(unit %d) Error: unsupported Transport type\n"),
                   unit));
        return BCM_E_PARAM;
    }

    sal_memset(&collector_create_msg, 0, sizeof(collector_create_msg));
    /*Implement multiple collector when it really being supported by BTE */
    collector_create_msg.id = 0;
    collector_create_msg.component_id = collector_info.protobuf.component_id;

    if (export_profile_info.flags &
                            BCM_COLLECTOR_EXPORT_PROFILE_FLAGS_USE_NUM_RECORDS) {
        collector_create_msg.num_port_in_pkt = export_profile_info.num_records;
    } else {
        collector_create_msg.mtu = export_profile_info.max_packet_length;
    }
    if (collector_info.udp.flags & BCM_COLLECTOR_UDP_FLAGS_CHECKSUM_ENABLE) {
        collector_create_msg.flags |= SHR_ST_MSG_CTRL_COLLECTOR_FLAGS_UDP_CHECKSUM;
    }

    rv = _bcm_xgs5_st_eapp_collector_encap_build_pack(unit,
                        &collector_info,
                        &collector_create_msg);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if (!(export_profile_info.flags &
                 BCM_COLLECTOR_EXPORT_PROFILE_FLAGS_USE_NUM_RECORDS)) {
        if ((export_profile_info.max_packet_length >
             telemetry_per_unit_global_info->max_pkt_size) ||
            (export_profile_info.max_packet_length <
             (collector_create_msg.encap_length + SHR_L2_CRC_LENGTH))) {
            LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                      (BSL_META_U(unit,
                                  "TELEMETRY(unit %d) Error: Invalid max_packet_length=%u\n"),
                       unit, export_profile_info.max_packet_length));
            return BCM_E_PARAM;
        }
    }

    buffer_req      = telemetry_per_unit_global_info->dma_buffer;
    buffer_ptr      = st_sdk_msg_ctrl_collector_create_pack(buffer_req,
                                                        &collector_create_msg);
    buffer_len      = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN(_bcm_xgs5_st_msg_send_receive(unit,
                                    MOS_MSG_SUBCLASS_ST_COLLECTOR_CREATE,
                                    buffer_len, 0,
                                    MOS_MSG_SUBCLASS_ST_COLLECTOR_CREATE_REPLY,
                                    &reply_len, 
                                    SHR_ST_MAX_UC_MSG_TIMEOUT));
    return BCM_E_NONE;
}

STATIC int
bcm_xgs5_st_eapp_collector_delete_msg (int unit,
                    int int_collector_id)
{
    uint16 reply_len = 0;


    BCM_IF_ERROR_RETURN(_bcm_xgs5_st_msg_send_receive(unit, 
                                    MOS_MSG_SUBCLASS_ST_COLLECTOR_DELETE,
                                    int_collector_id, 0,
                                    MOS_MSG_SUBCLASS_ST_COLLECTOR_DELETE_REPLY,
                                    &reply_len, 
                                    SHR_ST_MAX_UC_MSG_TIMEOUT));
    return BCM_E_NONE;

}

STATIC int
bcm_xgs5_st_eapp_collector_attach_msg (int unit, int instance_id,
                                       uint32 export_interval)
{
    shr_st_msg_ctrl_instance_collector_attach_t collector_attach_msg;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    _bcm_telemetry_global_info_t  *telemetry_per_unit_global_info =
                                    TELEMETRY_GLOBAL_INFO_GET(unit);

    sal_memset(&collector_attach_msg, 0, sizeof(collector_attach_msg));
    /*Implement multiple collector when it really being supported by BTE */
    collector_attach_msg.instance_id = instance_id;
    collector_attach_msg.export_interval_usecs = export_interval;
    collector_attach_msg.collector_id = 0;

    buffer_req      = telemetry_per_unit_global_info->dma_buffer;
    buffer_ptr      = shr_st_msg_ctrl_instance_collector_attach_pack(buffer_req,
                                                        &collector_attach_msg);
    buffer_len      = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN(_bcm_xgs5_st_msg_send_receive(unit,
                            MOS_MSG_SUBCLASS_ST_INSTANCE_COLLECTOR_ATTACH,
                            buffer_len, 0,
                            MOS_MSG_SUBCLASS_ST_INSTANCE_COLLECTOR_ATTACH_REPLY,
                            &reply_len, 
                            SHR_ST_MAX_UC_MSG_TIMEOUT));
    return BCM_E_NONE;
}

STATIC int
bcm_xgs5_st_eapp_collector_detach_msg (int unit,
                    int instance_id)
{
    uint16 reply_len = 0;

    BCM_IF_ERROR_RETURN(_bcm_xgs5_st_msg_send_receive(unit,
                            MOS_MSG_SUBCLASS_ST_INSTANCE_COLLECTOR_DETACH,
                            instance_id, 0,
                            MOS_MSG_SUBCLASS_ST_INSTANCE_COLLECTOR_DETACH_REPLY,
                            &reply_len, 
                            SHR_ST_MAX_UC_MSG_TIMEOUT));
    return BCM_E_NONE;
}

STATIC
int _bcm_xgs5_st_eapp_system_id_set_msg(int unit)
{
    shr_st_msg_system_id_t system_id_set_msg;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    _bcm_telemetry_global_info_t *telemetry_config_per_unit =
                           TELEMETRY_GLOBAL_INFO_GET(unit);


    sal_memset(&system_id_set_msg, 0, sizeof(system_id_set_msg));
    system_id_set_msg.system_id_length =
                    telemetry_config_per_unit->system_id_length;
    sal_memcpy(system_id_set_msg.system_id,
               telemetry_config_per_unit->system_id,
               (system_id_set_msg.system_id_length * sizeof(uint8)));

    buffer_req      = telemetry_config_per_unit->dma_buffer;
    buffer_ptr      = shr_st_msg_system_id_pack(buffer_req,
                                                    &system_id_set_msg);
    buffer_len      = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN(_bcm_xgs5_st_msg_send_receive(unit,
                                        MOS_MSG_SUBCLASS_ST_SYSTEM_ID_SET,
                                        buffer_len, 0,
                                        MOS_MSG_SUBCLASS_ST_SYSTEM_ID_SET_REPLY,
                                        &reply_len,
                                        SHR_ST_MAX_UC_MSG_TIMEOUT));
    return BCM_E_NONE;
}

STATIC
int _bcm_xgs5_st_eapp_system_id_get_msg(int unit)
{
    shr_st_msg_system_id_t system_id_get_msg;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    _bcm_telemetry_global_info_t  *telemetry_per_unit_global_info =
                                    TELEMETRY_GLOBAL_INFO_GET(unit);

    sal_memset(&system_id_get_msg, 0, sizeof(system_id_get_msg));

    BCM_IF_ERROR_RETURN(_bcm_xgs5_st_msg_send_receive(unit,
                                    MOS_MSG_SUBCLASS_ST_SYSTEM_ID_GET,
                                    0, 0,
                                    MOS_MSG_SUBCLASS_ST_SYSTEM_ID_GET_REPLY,
                                    &reply_len, 
                                    SHR_ST_MAX_UC_MSG_TIMEOUT));

    buffer_req      = telemetry_per_unit_global_info->dma_buffer;
    buffer_ptr      = shr_st_msg_system_id_unpack(buffer_req,
                                                        &system_id_get_msg);
    buffer_len      = buffer_ptr - buffer_req;

    if (reply_len != buffer_len) {
        LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                  (BSL_META_U(unit,
                  "TELEMETRY(unit %d) Error: System ID "
                  " get failed from BTE\n"), unit));
        return BCM_E_INTERNAL;
    }

    telemetry_per_unit_global_info->system_id_length =
                                  system_id_get_msg.system_id_length;
    sal_memcpy(telemetry_per_unit_global_info->system_id,
            system_id_get_msg.system_id,
            (telemetry_per_unit_global_info->system_id_length * sizeof(uint8)));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs5_st_eapp_instance_export_stats_get_msg
 * Purpose:
 *      Send the Instance export statistics get message to EApp
 * Parameters:
 *      unit                - (IN)  BCM device number
 *      stats               - (OUT) Export statistics.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_xgs5_st_eapp_instance_export_stats_get_msg(
                                    int unit,
                                    bcm_telemetry_instance_export_stats_t *stats)
{
    shr_st_msg_instance_export_stats_t msg;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    _bcm_telemetry_global_info_t  *telemetry_per_unit_global_info =
                                    TELEMETRY_GLOBAL_INFO_GET(unit);

    sal_memset(&msg, 0, sizeof(msg));

    BCM_IF_ERROR_RETURN(
        _bcm_xgs5_st_msg_send_receive(
                             unit,
                             MOS_MSG_SUBCLASS_ST_INSTANCE_EXPORT_STATS_GET,
                             0, 0,
                             MOS_MSG_SUBCLASS_ST_INSTANCE_EXPORT_STATS_GET_REPLY,
                             &reply_len,
                             SHR_ST_MAX_UC_MSG_TIMEOUT));

    buffer_req = telemetry_per_unit_global_info->dma_buffer;
    buffer_ptr = shr_st_msg_instance_export_stats_unpack(buffer_req, &msg);
    buffer_len = buffer_ptr - buffer_req;

    if (reply_len != buffer_len) {
        LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                  (BSL_META_U(unit,
                  "TELEMETRY(unit %d) Error: Instance export stats "
                  " get failed from BTE\n"), unit));
        return BCM_E_INTERNAL;
    }

    COMPILER_64_SET(stats->tx_pkts, msg.tx_pkts_hi, msg.tx_pkts_lo);
    COMPILER_64_SET(stats->tx_pkt_fails, msg.tx_pkt_fails_hi, msg.tx_pkt_fails_lo);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs5_st_eapp_instance_export_stats_set_msg
 * Purpose:
 *      Send the Instance export statistics set message to EApp
 * Parameters:
 *      unit                - (IN)  BCM device number
 *      stats               - (IN)  Export statistics.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_xgs5_st_eapp_instance_export_stats_set_msg(
                                    int unit,
                                    bcm_telemetry_instance_export_stats_t *stats)
{
    shr_st_msg_instance_export_stats_t msg;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    _bcm_telemetry_global_info_t  *telemetry_per_unit_global_info =
                                    TELEMETRY_GLOBAL_INFO_GET(unit);

    sal_memset(&msg, 0, sizeof(msg));

    msg.tx_pkts_hi = COMPILER_64_HI(stats->tx_pkts);
    msg.tx_pkts_lo = COMPILER_64_LO(stats->tx_pkts);

    msg.tx_pkt_fails_hi = COMPILER_64_HI(stats->tx_pkt_fails);
    msg.tx_pkt_fails_lo = COMPILER_64_LO(stats->tx_pkt_fails);

    buffer_req = telemetry_per_unit_global_info->dma_buffer;
    buffer_ptr = shr_st_msg_instance_export_stats_pack(buffer_req, &msg);
    buffer_len = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN(
        _bcm_xgs5_st_msg_send_receive(
                             unit,
                             MOS_MSG_SUBCLASS_ST_INSTANCE_EXPORT_STATS_SET,
                             buffer_len, 0,
                             MOS_MSG_SUBCLASS_ST_INSTANCE_EXPORT_STATS_SET_REPLY,
                             &reply_len,
                             SHR_ST_MAX_UC_MSG_TIMEOUT));


    return BCM_E_NONE;
}

/******************************************************
*                                                     *
*        TELEMETRY INSTANCE LIST MANAGEMENT FUNCTIONS *
 */

/*
 * Function:
 *      _bcm_xgs5_telemetry_system_id_set
 * Purpose:
 *      Configures Telemetry system ID information.
 * Parameters:
 *      unit                - (IN) BCM device number
 *      system_id_len       - (IN) length of telemetry system ID
 *      system_id           - (IN) System ID value
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_xgs5_telemetry_system_id_set(
        int unit, int system_id_len,
        uint8 *system_id)
{
    int rv = BCM_E_NONE;
    _bcm_telemetry_global_info_t *telemetry_config_per_unit =
                           TELEMETRY_GLOBAL_INFO_GET(unit);
    _bcm_telemetry_instance_info_t *telemetry_instance_list_node = NULL;

    if (telemetry_config_per_unit == NULL) {
        return BCM_E_INIT;
    }

    if ((telemetry_config_per_unit->system_id_length != 0) &&
        (system_id_len == 0))
    {
        /*Check whether instance has valid config count
         * if so reject system ID reset
         */
        telemetry_instance_list_node =
            &(telemetry_instances_info_list[unit][0]);

        if (telemetry_instance_list_node && 
                (telemetry_instance_list_node->config_count !=
                 BCM_TELEMETRY_INVALID_VALUE))
        {

            LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                     (BSL_META_U(unit,
                     "TELEMETRY(unit %d) Error: Telemetry System ID "
                     " cannot be reset with telemetry config exist \n"), unit));
        }
    }

    if ((system_id_len < 0) ||
            (system_id_len > BCM_TELEMETRY_MAX_SYSTEM_ID_LENGTH)) {
        LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                (BSL_META_U(unit,
                            "TELEMETRY(unit %d) Error: Invalid "
                            " System ID length \n"), unit));
        return BCM_E_PARAM;
    }

    if (system_id == NULL) {
        LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                (BSL_META_U(unit,
                            "TELEMETRY(unit %d) Error: Missing "
                            " System ID value\n"), unit));
        return BCM_E_PARAM;
    }

    if ((system_id_len == 0) &&
            (telemetry_config_per_unit->system_id_length == 0)) {
        LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                (BSL_META_U(unit,
                            "TELEMETRY(unit %d) Error: "
                            " System ID length cnnot be zero\n"), unit));
        return BCM_E_PARAM;
    }

    if ((system_id_len == 0) &&
            (telemetry_config_per_unit->system_id_length != 0)) {

        sal_memset(telemetry_config_per_unit->system_id, 0,
                (telemetry_config_per_unit->system_id_length * sizeof(uint8)));
    } else {
        sal_memcpy(telemetry_config_per_unit->system_id, system_id,
                (system_id_len * sizeof(uint8)));
    }

    telemetry_config_per_unit->system_id_length = system_id_len;

    rv = _bcm_xgs5_st_eapp_system_id_set_msg(unit);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                (BSL_META_U(unit,
                            "TELEMETRY(unit %d) Error: system ID "
                            " set failed to BTE %s\n"), unit, bcm_errmsg(rv)));
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs5_telemetry_system_id_get
 * Purpose:
 *      Fetches Telemetry system ID information.
 * Parameters:
 *      unit                - (IN) BCM device number
 *      max_system_id_len   - (IN) Max length of telemetry system ID
 *      system_id_len       - (IN) System ID configured length
 *      system_id           - (IN) System ID value
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_xgs5_telemetry_system_id_get(
        int unit, int max_system_id_len,
        int *system_id_len,
        uint8 *system_id)
{
    _bcm_telemetry_global_info_t *telemetry_config_per_unit =
                           TELEMETRY_GLOBAL_INFO_GET(unit);

    if (telemetry_config_per_unit == NULL) {
        return BCM_E_INIT;
    }

    if (system_id_len == NULL) {
        LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                (BSL_META_U(unit,
                            "TELEMETRY(unit %d) Error: NULL "
                            " System ID length passed \n"), unit));
        return BCM_E_PARAM;
    }


    if(max_system_id_len == 0)
    {
        *system_id_len = telemetry_config_per_unit->system_id_length;
        return BCM_E_NONE;
    }

    if (max_system_id_len < (int)telemetry_config_per_unit->system_id_length)
    {
         LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                (BSL_META_U(unit,
                            "TELEMETRY(unit %d) Error: Invalid max system ID "
                            " length passed \n"), unit));
        return BCM_E_PARAM;
    }

    if (system_id == NULL) {
        LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                (BSL_META_U(unit,
                            "TELEMETRY(unit %d) Error: NULL "
                            " System ID passed \n"), unit));
        return BCM_E_PARAM;
    }

    sal_memset(system_id, 0, (max_system_id_len * sizeof(uint8)));
    *system_id_len = telemetry_config_per_unit->system_id_length;
    sal_memcpy(system_id, telemetry_config_per_unit->system_id,
            (telemetry_config_per_unit->system_id_length * sizeof(uint8)));

    return BCM_E_NONE;    
}

/*
 * Function:
 *     _bcm_xgs5_telemetry_instance_config_update
 * Purpose:
 *     Updates a telemetry instance config information.
 * Parameters:
 *     unit          - (IN) BCM device number
 *     telemetry_instance_id   - (IN) The telemetry instance ID
 *     core                    - (IN) FW core on which instance have to run
 *      config_count        - (IN)  Number of telemetry objects to be
 *                                  associated with this instance.
 *     telemetry_config - (IN) Telemetry instance configuration.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_telemetry_instance_config_update(int unit,
                           int telemetry_instance_id,
                           int core,
                           int config_count,
                           bcm_telemetry_config_t *telemetry_config)
{
    int index = BCM_TELEMETRY_INVALID_INDEX;
    int cur_instance_index = BCM_TELEMETRY_INVALID_INDEX,  rv = BCM_E_NONE;
    int first_free_index = BCM_TELEMETRY_INVALID_INDEX;
    _bcm_telemetry_instance_info_t *telemetry_instance_list_node = NULL;
    _bcm_telemetry_global_info_t  *telemetry_per_unit_global_info =
                                    TELEMETRY_GLOBAL_INFO_GET(unit);
    int i;
    bcm_port_t port;
    bcm_gport_t gport;

    if (telemetry_per_unit_global_info == NULL) {
        return BCM_E_INIT;
    }

    if (config_count < 0)
    {
        LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                  (BSL_META_U(unit,
                  "TELEMETRY(unit %d) Error: Invalid "
                  " configuration count passed\n"), unit));
        return BCM_E_PARAM;
    }

    if (!SOC_IS_TOMAHAWK3(unit)) {
        if ((config_count != 0) &&
                (telemetry_per_unit_global_info->system_id_length == 0))
        {
            LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                    (BSL_META_U(unit,
                                "TELEMETRY(unit %d) Error: System ID "
                                " was not configured\n"), unit));
            return BCM_E_CONFIG;
        }
    }
    if (core != telemetry_per_unit_global_info->uC)
    {
        LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                  (BSL_META_U(unit,
                  "TELEMETRY(unit %d) Error: Only core %u is allowed to "
                              " configure for Telemetry instances\n"), unit,
                   telemetry_per_unit_global_info->uC));
        return BCM_E_PARAM;
    }

    for (i = 0; i < config_count; i++) {
        gport = telemetry_config[i].gport;
        if (!BCM_GPORT_IS_LOCAL(gport)) {
            LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                    (BSL_META_U(unit,
                          "TELEMETRY(unit %d) Error: only local "
                          " type gport add is supported \n"), unit));
            return BCM_E_CONFIG;
        }
        port = BCM_GPORT_LOCAL_GET(gport);
        if (!(IS_E_PORT(unit, port)) || (IS_MANAGEMENT_PORT(unit, port))) {
            LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                    (BSL_META_U(unit,
                                "TELEMETRY(unit %d) Error: Invalid port=%d\n"),
                     unit, port));
            return BCM_E_PARAM;
        }
    }

    rv = _bcm_xgs5_telemetry_is_instance_exists(unit, telemetry_instance_id,
                                   &cur_instance_index, &first_free_index);
    if(rv == BCM_E_NOT_FOUND)
    {
        if (first_free_index == BCM_TELEMETRY_INVALID_INDEX)
        {
            LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                      (BSL_META_U(unit,
                      "TELEMETRY(unit %d) Error: Not allowed to "
                      "configure more than supported %d telemetry instances\n"),
                      unit, BCM_INT_TELEMETRY_MAX_INSTANCES));
            return BCM_E_RESOURCE;
        }
        index = first_free_index;
        if (config_count) {
            telemetry_per_unit_global_info->num_telemetry_instances++;
        }
        rv = BCM_E_NONE; 
    } else {
        index = cur_instance_index;
        if (!config_count) {
            telemetry_per_unit_global_info->num_telemetry_instances--;
        } 
    }

    telemetry_instance_list_node =
        &(telemetry_instances_info_list[unit][index]);

    if (telemetry_instance_list_node->config_count !=
                                        BCM_TELEMETRY_INVALID_VALUE)
    {
        rv = bcm_xgs5_st_eapp_port_delete_msg(unit,
                                 telemetry_config,
                                 config_count);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                    (BSL_META_U(unit,
                          "TELEMETRY(unit %d) Error: port "
                          " delete failed to BTE %s\n"), unit, bcm_errmsg(rv)));
            return rv;
        }
        if (config_count == 0)
        {
            rv = bcm_xgs5_st_eapp_instance_delete_msg(unit,
                    telemetry_instance_list_node->instance_id);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                        (BSL_META_U(unit,
                          "TELEMETRY(unit %d) Error: Instance "
                          " delete failed to BTE %s\n"), unit, bcm_errmsg(rv)));
                return rv;
            }
        }
        sal_free(telemetry_instance_list_node->telemetry_config);
        telemetry_instance_list_node->telemetry_config = NULL;
        telemetry_instance_list_node->config_count =
                                         BCM_TELEMETRY_INVALID_VALUE;
        telemetry_instance_list_node->instance_id = BCM_TELEMETRY_INVALID_INDEX;

    }

    if (config_count) {
        _BCM_TELEMETRY_ALLOC(telemetry_instance_list_node->telemetry_config,
                bcm_telemetry_config_t,
                (config_count * sizeof(bcm_telemetry_config_t)),
                "TELEMETRY_CONFIG_INFO");
        sal_memcpy(telemetry_instance_list_node->telemetry_config,
                telemetry_config,
                (config_count *sizeof(bcm_telemetry_config_t)));
        telemetry_instance_list_node->config_count = config_count;
        telemetry_instance_list_node->instance_id = telemetry_instance_id;
        telemetry_instance_list_node->core = core;
        rv = bcm_xgs5_st_eapp_port_add_msg(unit,
                                 telemetry_instance_list_node->telemetry_config,
                                 telemetry_instance_list_node->config_count);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                    (BSL_META_U(unit,
                          "TELEMETRY(unit %d) Error: port "
                          " add failed to BTE %s\n"), unit, bcm_errmsg(rv)));
            return rv;
        }
        rv = bcm_xgs5_st_eapp_instance_create_msg(unit,
                                 telemetry_instance_list_node->instance_id,
                                 telemetry_instance_list_node->telemetry_config,
                                 telemetry_instance_list_node->config_count);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                    (BSL_META_U(unit,
                          "TELEMETRY(unit %d) Error: Instance "
                          " add failed to BTE %s\n"), unit, bcm_errmsg(rv)));
            return rv;
        }
    }
    LOG_DEBUG(BSL_LS_BCM_TELEMETRY,
            (BSL_META_U(unit,
                        "TELEMETRY(unit %d) Config Info update: %s" 
                        " (Telemetry instance=0x%x).\n"), unit, FUNCTION_NAME(),
                         telemetry_instance_id));
    return rv;
}

/*
 * Function:
 *     _bcm_xgs5_telemetry_instance_config_get
 * Purpose:
 *     Updates a telemetry instance config information.
 * Parameters:
 *     unit          - (IN) BCM device number
 *     telemetry_instance_id   - (IN) The telemetry instance ID
 *     core             - (IN) FW Core on which instance is being run 
 *     max_count        - (IN) Max objects requested 
 *     telemetry_config - (OUT) Telemetry instance configuration.
 *     config_count        - (OUT)  Number of telemetry objects to be
 *                                  associated with this instance.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_telemetry_instance_config_get(int unit,
                           int telemetry_instance_id,
                           int core,
                           int max_count,
                           bcm_telemetry_config_t *telemetry_config,
                           int *config_count)
{
    int first_free_index = 0, cur_instance_index = 0, rv = BCM_E_NONE;
    _bcm_telemetry_instance_info_t *telemetry_instance_list_node = NULL;
    _bcm_telemetry_global_info_t  *telemetry_per_unit_global_info =
                                    TELEMETRY_GLOBAL_INFO_GET(unit);

    if (telemetry_per_unit_global_info == NULL) {
        return BCM_E_INIT;
    }
    rv = _bcm_xgs5_telemetry_is_instance_exists(unit, telemetry_instance_id,
                                   &cur_instance_index, &first_free_index);

    /* If not found, return BCM_E_NOT_FOUND */
    if (BCM_FAILURE(rv)) {
        return rv;
    }
    telemetry_instance_list_node =
               &(telemetry_instances_info_list[unit][cur_instance_index]);
    *config_count = telemetry_instance_list_node->config_count;
    if (max_count < *config_count) 
    {
        *config_count = max_count;    
    }
    if (max_count == 0)
    {
        *config_count = telemetry_instance_list_node->config_count;    
        return rv;
    }
    if (telemetry_config != NULL)
    { 
        sal_memcpy(telemetry_config,
                telemetry_instance_list_node->telemetry_config,
                ((*config_count) *sizeof(bcm_telemetry_config_t)));
    }
    LOG_DEBUG(BSL_LS_BCM_TELEMETRY,
            (BSL_META_U(unit,
                        "TELEMETRY(unit %d) Config Info get: %s" 
                        " (Telemetry instance=0x%x).\n"), unit, FUNCTION_NAME(),
                         telemetry_instance_id));
    return rv;
}

/*
 * Function:
 *     _bcm_xgs5_telemetry_instance_export_config_update
 * Purpose:
 *     Updates a telemetry instance export config information.
 * Parameters:
 *     unit          - (IN) BCM device number
 *     telemetry_instance_id   - (IN) The telemetry instance ID
 *     collector_id            - (IN) Collector ID
 *     export_profile_id       - (IN) Export profile ID
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_telemetry_instance_export_config_update(int unit,
                           int telemetry_instance_id,
                           bcm_collector_t collector_id,
                           int export_profile_id)
{
    int first_free_index = 0, cur_instance_index = 0, rv = BCM_E_NONE;
    _bcm_telemetry_instance_info_t *telemetry_instance_list_node = NULL;
    bcm_collector_info_t collector_info;
    bcm_collector_export_profile_t profile_info;
    _bcm_telemetry_global_info_t  *telemetry_per_unit_global_info =
                                    TELEMETRY_GLOBAL_INFO_GET(unit);

    if (telemetry_per_unit_global_info == NULL) {
        return BCM_E_INIT;
    }
    rv = _bcm_xgs5_telemetry_is_instance_exists(unit, telemetry_instance_id,
                                   &cur_instance_index, &first_free_index);

    /* If not found, return BCM_E_NOT_FOUND */
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    telemetry_instance_list_node =
               &(telemetry_instances_info_list[unit][cur_instance_index]);

    if(collector_id != BCM_TELEMETRY_INVALID_VALUE)
    {
        rv = bcm_esw_collector_get(unit, collector_id, &collector_info);

        /* If not found, return BCM_E_NOT_FOUND */
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                    (BSL_META_U(unit,
                                "TELEMETRY(unit %d) Error: Invalid "
                                " collector ID passed\n"), unit));
            return rv;
        }

        rv = bcm_esw_collector_export_profile_get(unit, export_profile_id,
                                                  &profile_info);

        /* If not found, return BCM_E_NOT_FOUND */
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                    (BSL_META_U(unit,
                                "TELEMETRY(unit %d) Error: Invalid "
                                " export profile ID passed\n"), unit));
            return rv;
        }

        if (profile_info.wire_format != bcmCollectorWireFormatProtoBuf) {
            LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                    (BSL_META_U(unit,
                                "TELEMETRY(unit %d) Error: Invalid "
                                " export profile wire format\n"), unit));
            return BCM_E_PARAM;
        }

        if ((telemetry_instance_list_node->collector_id != 
                                             BCM_TELEMETRY_INVALID_VALUE) &&
            (telemetry_instance_list_node->collector_id != collector_id))
        {
            LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                    (BSL_META_U(unit,
                                "TELEMETRY(unit %d) Error: Only "
                                " one collector is supported\n"), unit));
            return BCM_E_CONFIG;    
        }
        if ((telemetry_instance_list_node->export_profile_id != 
                                             BCM_TELEMETRY_INVALID_VALUE) &&
            (telemetry_instance_list_node->export_profile_id !=
                                                    export_profile_id))
        {
            LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                    (BSL_META_U(unit,
                                "TELEMETRY(unit %d) Error: Only "
                                " one collector is supported\n"), unit));
            return BCM_E_CONFIG;    
        }
        /* 
         * Check if I am the only user for this collector. Else
         * return error.
         */
        if (_bcm_esw_collector_check_user_is_other(unit, collector_id, 
                            _BCM_COLLECTOR_EXPORT_APP_ST_PROTOBUF)) {
            LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                    (BSL_META_U(unit,
                                "TELEMETRY(unit %d) Error: Collector "
                                " is already attached to another application\n"), unit));
            return BCM_E_CONFIG;    
        } 
    }

    if (telemetry_instance_list_node->collector_id ==
                                    BCM_TELEMETRY_INVALID_VALUE)
    {
        
        rv = bcm_xgs5_st_eapp_collector_create_msg(unit, collector_id,
                                                   export_profile_id);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                    (BSL_META_U(unit,
                          "TELEMETRY(unit %d) Error: collector "
                          " add failed to BTE %s\n"), unit, bcm_errmsg(rv)));
            return rv;
        }
        rv = bcm_xgs5_st_eapp_collector_attach_msg(unit,
                                                   telemetry_instance_id,
                                                   profile_info.export_interval);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                    (BSL_META_U(unit,
                          "TELEMETRY(unit %d) Error: collector "
                          " attach failed to BTE %s\n"), unit, bcm_errmsg(rv)));
            return rv;
        }  
        telemetry_instance_list_node->num_collectors++;

        rv = _bcm_esw_collector_ref_count_update(unit, collector_id, 1);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                    (BSL_META_U(unit,
                          "TELEMETRY(unit %d) Error: collector "
                          " ref_count update failed  %s\n"), unit, bcm_errmsg(rv)));
            return rv;
        }
        rv = _bcm_esw_collector_user_update(unit, collector_id, _BCM_COLLECTOR_EXPORT_APP_ST_PROTOBUF);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                    (BSL_META_U(unit,
                          "TELEMETRY(unit %d) Error: collector "
                          " user update failed  %s\n"), unit, bcm_errmsg(rv)));
            return rv;
        }
        rv = _bcm_esw_collector_export_profile_ref_count_update(unit,
                                                                export_profile_id,
                                                                1);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                    (BSL_META_U(unit,
                          "TELEMETRY(unit %d) Error: export profile "
                          " ref_count update failed  %s\n"), unit, bcm_errmsg(rv)));
            return rv;
        }

    } else if (collector_id == BCM_TELEMETRY_INVALID_VALUE){
        rv = bcm_xgs5_st_eapp_collector_detach_msg(unit,
                                                    telemetry_instance_id);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                    (BSL_META_U(unit,
                          "TELEMETRY(unit %d) Error: collector "
                          " detach failed to BTE %s\n"), unit, bcm_errmsg(rv)));
            return rv;
        }
        /* Hard code BTE internal collector id to zero as of now  as
           only one collector supported*/
        rv = bcm_xgs5_st_eapp_collector_delete_msg(unit, 0);         
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                    (BSL_META_U(unit,
                          "TELEMETRY(unit %d) Error: collector "
                          " delete failed to BTE %s\n"), unit, bcm_errmsg(rv)));
            return rv;
        }
        telemetry_instance_list_node->num_collectors--;

        rv = _bcm_esw_collector_ref_count_update(
                                     unit,
                                     telemetry_instance_list_node->collector_id,
                                     -1);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                    (BSL_META_U(unit,
                          "TELEMETRY(unit %d) Error: collector "
                          " ref_count update failed  %s\n"), unit, bcm_errmsg(rv)));
            return rv;
        }
        rv = _bcm_esw_collector_export_profile_ref_count_update(
                                      unit,
                                      telemetry_instance_list_node->export_profile_id,
                                      -1);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                    (BSL_META_U(unit,
                          "TELEMETRY(unit %d) Error: collector "
                          " ref_count update failed  %s\n"), unit, bcm_errmsg(rv)));
            return rv;
        }
    }

    telemetry_instance_list_node->collector_id = collector_id;
    telemetry_instance_list_node->export_profile_id = export_profile_id;

    LOG_DEBUG(BSL_LS_BCM_TELEMETRY,
            (BSL_META_U(unit,
                        "TELEMETRY(unit %d) Export Config Info update: %s" 
                        " (Telemetry instance=0x%x).\n"), unit, FUNCTION_NAME(),
                         telemetry_instance_id));
    return rv;
}

/*
 * Function:
 *     _bcm_xgs5_telemetry_instance_export_config_get
 * Purpose:
 *     Fetches a telemetry instance export config information.
 * Parameters:
 *     unit          - (IN) BCM device number
 *     telemetry_instance_id   - (IN) The telemetry instance ID
 *     collector_id            - (IN) Pointer to collector ID
 *     export_profile_id       - (IN) Pointer to export profile ID
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_telemetry_instance_export_config_get(int unit,
                           int telemetry_instance_id,
                           bcm_collector_t *collector_id,
                           int *export_profile_id)
{
    int first_free_index = 0, cur_instance_index = 0, rv = BCM_E_NONE;
    _bcm_telemetry_instance_info_t *telemetry_instance_list_node = NULL;
    _bcm_telemetry_global_info_t  *telemetry_per_unit_global_info =
                                    TELEMETRY_GLOBAL_INFO_GET(unit);

    if (telemetry_per_unit_global_info == NULL) {
        return BCM_E_INIT;
    }
    rv = _bcm_xgs5_telemetry_is_instance_exists(unit, telemetry_instance_id,
                                   &cur_instance_index, &first_free_index);

    /* If not found, return BCM_E_NOT_FOUND */
    if (BCM_FAILURE(rv)) {
        return rv;
    }
    telemetry_instance_list_node =
               &(telemetry_instances_info_list[unit][cur_instance_index]);

     *collector_id = telemetry_instance_list_node->collector_id;
     *export_profile_id = telemetry_instance_list_node->export_profile_id;

    LOG_DEBUG(BSL_LS_BCM_TELEMETRY,
            (BSL_META_U(unit,
                        "TELEMETRY(unit %d) Export Config Info update: %s" 
                        " (Telemetry instance=0x%x).\n"), unit, FUNCTION_NAME(),
                         telemetry_instance_id));
    return rv;
}

/*
 * Function:
 *      bcm_xgs5_telemetry_instance_export_stats_get
 * Purpose:
 *      Get the telemetry export statistics.
 * Parameters:
 *      unit                - (IN)  BCM device number
 *      telemetry_instance  - (IN)  Telemetry instance ID
 *      collector_id        - (IN)  Collector ID
 *      stats               - (OUT) Export statistics.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_xgs5_telemetry_instance_export_stats_get(
                                    int unit,
                                    int telemetry_instance,
                                    bcm_collector_t collector_id,
                                    bcm_telemetry_instance_export_stats_t *stats)
{
    int first_free_index = 0, cur_instance_index = 0;
    int rv = BCM_E_NONE;
    bcm_collector_info_t collector_info;
    _bcm_telemetry_global_info_t *telemetry_per_unit_global_info =
                                    TELEMETRY_GLOBAL_INFO_GET(unit);

    if (telemetry_per_unit_global_info == NULL) {
        return BCM_E_INIT;
    }

    if (stats == NULL) {
        return BCM_E_PARAM;
    }

    rv = _bcm_xgs5_telemetry_is_instance_exists(unit, telemetry_instance,
                                                &cur_instance_index,
                                                &first_free_index);
    /* If not found, return BCM_E_NOT_FOUND */
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                  (BSL_META_U(unit,
                              "TELEMETRY(unit %d) Error: Invalid "
                              " instance ID passed\n"), unit));
        return rv;
    }

    rv = bcm_esw_collector_get(unit, collector_id, &collector_info);
    /* If not found, return BCM_E_NOT_FOUND */
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                  (BSL_META_U(unit,
                              "TELEMETRY(unit %d) Error: Invalid "
                              " collector ID passed\n"), unit));
        return rv;
    }

    rv = _bcm_xgs5_st_eapp_instance_export_stats_get_msg(unit, stats);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_xgs5_telemetry_instance_export_stats_set
 * Purpose:
 *      Set the telemetry export statistics.
 * Parameters:
 *      unit                - (IN) BCM device number
 *      telemetry_instance  - (IN) Telemetry instance ID
 *      collector_id        - (IN) Collector ID
 *      stats               - (IN) Export statistics.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_xgs5_telemetry_instance_export_stats_set(
                                    int unit,
                                    int telemetry_instance,
                                    bcm_collector_t collector_id,
                                    bcm_telemetry_instance_export_stats_t *stats)
{
    int first_free_index = 0, cur_instance_index = 0;
    int rv = BCM_E_NONE;
    bcm_collector_info_t collector_info;
    _bcm_telemetry_global_info_t *telemetry_per_unit_global_info =
                                    TELEMETRY_GLOBAL_INFO_GET(unit);

    if (telemetry_per_unit_global_info == NULL) {
        return BCM_E_INIT;
    }

    if (stats == NULL) {
        return BCM_E_PARAM;
    }

    rv = _bcm_xgs5_telemetry_is_instance_exists(unit, telemetry_instance,
                                                &cur_instance_index,
                                                &first_free_index);
    /* If not found, return BCM_E_NOT_FOUND */
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                  (BSL_META_U(unit,
                              "TELEMETRY(unit %d) Error: Invalid "
                              " instance ID passed\n"), unit));
        return rv;
    }

    rv = bcm_esw_collector_get(unit, collector_id, &collector_info);
    /* If not found, return BCM_E_NOT_FOUND */
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                  (BSL_META_U(unit,
                              "TELEMETRY(unit %d) Error: Invalid "
                              " collector ID passed\n"), unit));
        return rv;
    }

    rv = _bcm_xgs5_st_eapp_instance_export_stats_set_msg(unit, stats);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_xgs5_telemetry_instance_list_destroy
 * Purpose:
 *     Destroys the telemetry instance list.
 * Parameters:
 *     unit          - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_xgs5_telemetry_instance_list_destroy (int unit)
{
    if (telemetry_instances_info_list[unit] != NULL) {
        sal_free(telemetry_instances_info_list[unit]);
    }

    telemetry_instances_info_list[unit] = NULL;
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_xgs5_telemetry_instance_list_create
 * Purpose:
 *     Creates the telemetry instance list.
 * Parameters:
 *     unit          - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_xgs5_telemetry_instance_list_create (int unit)
{
    int i = 0;
    _bcm_telemetry_instance_info_t *telemetry_instance_node = NULL;

    if (telemetry_instances_info_list[unit] == NULL) {
        _BCM_TELEMETRY_ALLOC(telemetry_instances_info_list[unit],
                             _bcm_telemetry_instance_info_t,
                             sizeof(_bcm_telemetry_instance_info_t) *
                             (BCM_INT_TELEMETRY_MAX_INSTANCES),
                             "telemetry instances list");
    }

    if (telemetry_instances_info_list[unit] == NULL) {
        return BCM_E_MEMORY;
    }

    for (i= 0;
         i< BCM_INT_TELEMETRY_MAX_INSTANCES; i++) {
        telemetry_instance_node = &(telemetry_instances_info_list[unit][i]);
        telemetry_instance_node->instance_id =
                      BCM_TELEMETRY_INVALID_INDEX;
        telemetry_instance_node->config_count =
                      BCM_TELEMETRY_INVALID_VALUE;
        telemetry_instance_node->collector_id =
                      BCM_TELEMETRY_INVALID_VALUE;
        telemetry_instance_node->export_profile_id =
                      BCM_TELEMETRY_INVALID_VALUE;
    }
    return BCM_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT

typedef enum _bcm_xgs5_telemetry_wb_sequence_e {
    _bcmTelemetryWbSequenceConfig        = 0,
    _bcmTelemetryWbSequenceModuleStatus  = 1,
    _bcmTelemetryWbSequenceCount
} _bcm_xgs5_telemetry_wb_sequence_t;

#define BCM_WB_TELEMETRY_CONFIG_VERSION_1_0      SOC_SCACHE_VERSION(1,0)
#define BCM_WB_TELEMETRY_CONFIG_DEFAULT_VERSION  BCM_WB_TELEMETRY_CONFIG_VERSION_1_0

#define BCM_WB_TELEMETRY_MODULE_STATUS_VERSION_1_0      SOC_SCACHE_VERSION(1,0)
#define BCM_WB_TELEMETRY_MODULE_STATUS_DEFAULT_VERSION  BCM_WB_TELEMETRY_MODULE_STATUS_VERSION_1_0

#define TELEMETRY_SCACHE_WRITE(_scache, _val, _type)                     \
    do {                                                          \
        _type _tmp = (_type) (_val);                              \
        sal_memcpy((_scache), &(_tmp), sizeof(_type));            \
        (_scache) += sizeof(_type);                               \
    } while(0)

#define TELEMETRY_SCACHE_READ(_scache, _var, _type)                      \
    do {                                                          \
        _type _tmp;                                               \
        sal_memcpy(&(_tmp), (_scache), sizeof(_type));            \
        (_scache) += sizeof(_type);                               \
        (_var)     = (_tmp);                                      \
    } while(0)


/*
 * Function:
 *     _bcm_telemetry_config_sync
 * Purpose:
 *     Syncs all telemetry instances information to scache
 * Parameters:
 *     unit   - (IN) Device unit number
 *     scache - (IN) Pointer to current scache location
 * Returns:
 *     BCM_E_XXX
 */
STATIC void
_bcm_telemetry_config_sync(int unit, uint8 **scache)
{

    int i;
    _bcm_telemetry_instance_info_t *telemetry_instance = NULL;
    _bcm_telemetry_global_info_t  *telemetry_per_unit_global_info =
                                    TELEMETRY_GLOBAL_INFO_GET(unit);
    

    /* Number of telemetry instances configured */
    TELEMETRY_SCACHE_WRITE(*scache,
               telemetry_per_unit_global_info->num_telemetry_instances, uint32);

    for (i = 0;
         i < BCM_INT_TELEMETRY_MAX_INSTANCES; i++) {

        telemetry_instance = &(telemetry_instances_info_list[unit][i]);

        if (telemetry_instance->instance_id == BCM_TELEMETRY_INVALID_INDEX) {
            /* Telemetry instance does not exist, continue */
            continue;
        }

        /* Telemetry instance Id */
        TELEMETRY_SCACHE_WRITE(*scache, telemetry_instance->instance_id, int);

        /* Telemetry instance core */
        TELEMETRY_SCACHE_WRITE(*scache, telemetry_instance->core, int);

        /* Telemetry instance configuration count */
        TELEMETRY_SCACHE_WRITE(*scache, telemetry_instance->config_count, int);

        /* Telemetry instance collector ID*/
        TELEMETRY_SCACHE_WRITE(*scache, telemetry_instance->collector_id,
                              bcm_collector_t);

        /* Telemetry instance export profile ID*/
        TELEMETRY_SCACHE_WRITE(*scache, telemetry_instance->export_profile_id,
                              int);


    }
}

/*
 * Function:
 *     _bcm_xgs5_telemetry_config_sync
 * Purpose:
 *     Syncs Telemetry config data to scache
 * Parameters:
 *     unit   - (IN) Device unit number
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_xgs5_telemetry_config_sync(int unit)
{

    int                     stable_size;
    soc_scache_handle_t     scache_handle;
    uint8                  *scache = NULL;
    _bcm_telemetry_global_info_t  *telemetry_per_unit_global_info =
                                    TELEMETRY_GLOBAL_INFO_GET(unit);

    if (telemetry_per_unit_global_info == NULL) {
        return BCM_E_NONE;
    }

    /* Get Telemetry module storage size. */
    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    /* If level 2 store is not configured return from here. */
    if (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit) || (stable_size == 0)) {
        return BCM_E_NONE;
    }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit,
                          BCM_MODULE_TELEMETRY, _bcmTelemetryWbSequenceConfig);
    BCM_IF_ERROR_RETURN(_bcm_esw_scache_ptr_get(unit, scache_handle, 0, 0,
                                                &scache, 0, NULL));
    if (NULL == scache) {
        return BCM_E_INTERNAL;
    }

    _bcm_telemetry_config_sync(unit, &scache);

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_telemetry_recover
 * Purpose:
 *     Recover Telemetry data from scache
 * Parameters:
 *     unit   - (IN) Device unit number
 *     scache - (IN) Pointer to current scache location
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_telemetry_recover(int unit, uint8 **scache)
{
    int i, rv = BCM_E_NONE;
    _bcm_telemetry_instance_info_t *telemetry_instance = NULL;
    _bcm_telemetry_global_info_t  *telemetry_per_unit_global_info =
                                    TELEMETRY_GLOBAL_INFO_GET(unit);
    

    /* Number of telemetry instances configured */
    TELEMETRY_SCACHE_READ(*scache,
               telemetry_per_unit_global_info->num_telemetry_instances, uint32);

    for (i = 0;
         i < telemetry_per_unit_global_info->num_telemetry_instances; i++) {

        telemetry_instance = &(telemetry_instances_info_list[unit][i]);

        /* Telemetry instance Id */
        TELEMETRY_SCACHE_READ(*scache, telemetry_instance->instance_id, int);

        /* Transport instance core */
        TELEMETRY_SCACHE_READ(*scache, telemetry_instance->core, int);

        /* Transport instance  config count */
        TELEMETRY_SCACHE_READ(*scache, telemetry_instance->config_count, int);

        /* Telemetry instance collector ID*/
        TELEMETRY_SCACHE_READ(*scache, telemetry_instance->collector_id,
                              bcm_collector_t);

        /* Telemetry instance export profile ID*/
        TELEMETRY_SCACHE_READ(*scache, telemetry_instance->export_profile_id,
                              int);

        if (telemetry_instance->config_count) {
            _BCM_TELEMETRY_ALLOC(telemetry_instance->telemetry_config,
            bcm_telemetry_config_t,
            (telemetry_instance->config_count * sizeof(bcm_telemetry_config_t)),
                    "TELEMETRY_CONFIG_INFO");
        }
        rv = bcm_xgs5_st_eapp_instance_get_msg(unit,
                                          telemetry_instance->instance_id,
                                          telemetry_instance->telemetry_config,
                                          telemetry_instance->config_count);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                    (BSL_META_U(unit,
                                "TELEMETRY(unit %d) Error: Instance "
                                " get failed from BTE"), unit));
            return rv;
        }

        rv = _bcm_esw_collector_ref_count_update(unit,
                                                 telemetry_instance->collector_id,
                                                 1);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                    (BSL_META_U(unit,
                          "TELEMETRY(unit %d) Error: collector "
                          " ref_count update failed  %s\n"), unit, bcm_errmsg(rv)));
            return rv;
        }
        rv = _bcm_esw_collector_export_profile_ref_count_update(
                                          unit,
                                          telemetry_instance->export_profile_id,
                                          1);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                    (BSL_META_U(unit,
                          "TELEMETRY(unit %d) Error: export profile "
                          " ref_count update failed  %s\n"), unit, bcm_errmsg(rv)));
            return rv;
        }
        rv = _bcm_esw_collector_user_update(unit,
                                            telemetry_instance->collector_id,
                                            _BCM_COLLECTOR_EXPORT_APP_ST_PROTOBUF);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                    (BSL_META_U(unit,
                          "TELEMETRY(unit %d) Error: collector "
                          " user update failed  %s\n"), unit, bcm_errmsg(rv)));
            return rv;
        }

    }

    rv = bcm_xgs5_st_eapp_port_get_msg(unit);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                (BSL_META_U(unit,
                            "TELEMETRY(unit %d) Error: ports "
                            " get failed from BTE"), unit));
        return rv;
    }

    rv = _bcm_xgs5_st_eapp_system_id_get_msg(unit);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                (BSL_META_U(unit,
                            "TELEMETRY(unit %d) Error: system ID "
                            " get failed to BTE %s\n"), unit, bcm_errmsg(rv)));
        return rv;
    }
     
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_telemetry_config_scache_size_get
 * Purpose:
 *     Get the size required to sync telemetry data to scache
 * Parameters:
 *     unit   - (IN) Device unit number
 * Returns:
 *     Required size
 */
STATIC uint32
_bcm_telemetry_config_scache_size_get(int unit)
{
    uint32 telemetry_total_size = 0;
    uint32 telemetry_instances_info_size = 0;

    /* Max telemetry instances configured */
    telemetry_total_size += sizeof(uint32);

    /* Telemetry instance Id */
    telemetry_instances_info_size += sizeof(int);

    /* Telemetry instance core ID */
    telemetry_instances_info_size += sizeof(int);

    /* Telemetry instance config count */
    telemetry_instances_info_size += sizeof(int);

    /* Telemetry instance collector ID*/
    telemetry_instances_info_size += sizeof(bcm_collector_t);

    /* Telemetry instance export profile ID*/
    telemetry_instances_info_size += sizeof(int);

    /* Multiply by number of telemetry instances */
    telemetry_instances_info_size *= BCM_INT_TELEMETRY_MAX_INSTANCES;

    telemetry_total_size += telemetry_instances_info_size;

    return telemetry_total_size;
}

/*
 * Function:
 *      _bcm_telemetry_config_scache_alloc
 * Purpose:
 *      Telemetry WB scache alloc for storing configuration
 * Parameters:
 *      unit    - (IN) Unit number.
 *      create  - (IN) 1 - Create, 0 - Realloc
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int _bcm_telemetry_config_scache_alloc(int unit, int create)
{
    uint32                  cur_size = 0;
    uint32                  rqd_size = 0;
    int                     rv = BCM_E_NONE;
    soc_scache_handle_t     scache_handle;
    uint8                  *scache = NULL;

    rqd_size += _bcm_telemetry_config_scache_size_get(unit);

    SOC_SCACHE_HANDLE_SET(scache_handle, unit,
                          BCM_MODULE_TELEMETRY, _bcmTelemetryWbSequenceConfig);

    if (create) {
        rv = _bcm_esw_scache_ptr_get(unit, scache_handle, 1, rqd_size,
                               &scache, BCM_WB_TELEMETRY_CONFIG_DEFAULT_VERSION, NULL);
        BCM_IF_ERROR_RETURN(rv);
    } else {
        /* Get current size */
        rv = soc_scache_ptr_get(unit, scache_handle, &scache, &cur_size);
        SOC_IF_ERROR_RETURN(rv);

        if (rqd_size > cur_size) {
            /* Request the extra size */
            rv = soc_scache_realloc(unit, scache_handle, rqd_size - cur_size);
            SOC_IF_ERROR_RETURN(rv);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_xgs5_telemetry_module_status_sync
 * Purpose:
 *     Syncs Telemetry module status to scache
 * Parameters:
 *     unit   - (IN) Device unit number
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_xgs5_telemetry_module_status_sync(int unit)
{

    int                     stable_size;
    soc_scache_handle_t     scache_handle;
    uint8                  *scache = NULL;
    _bcm_telemetry_global_info_t  *telemetry_per_unit_global_info =
                                    TELEMETRY_GLOBAL_INFO_GET(unit);

    /* Get Telemetry module storage size. */
    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    /* If level 2 store is not configured return from here. */
    if (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit) || (stable_size == 0)) {
        return BCM_E_NONE;
    }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit,
                          BCM_MODULE_TELEMETRY, _bcmTelemetryWbSequenceModuleStatus);
    BCM_IF_ERROR_RETURN(_bcm_esw_scache_ptr_get(unit, scache_handle, 0, 0,
                                                &scache, 0, NULL));
    if (NULL == scache) {
        return BCM_E_INTERNAL;
    }


    if (telemetry_per_unit_global_info == NULL) {
        /* Module not running */
        TELEMETRY_SCACHE_WRITE(scache, 0, uint8);
    } else {
        /* Module in running state */
        TELEMETRY_SCACHE_WRITE(scache, 1, uint8);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_telemetry_module_status_scache_size_get
 * Purpose:
 *     Get the size required to sync telemetry module status to scache
 * Parameters:
 *     unit   - (IN) Device unit number
 * Returns:
 *     Required size
 */
STATIC uint32
_bcm_telemetry_module_status_scache_size_get(int unit)
{
    return (sizeof(uint8));
}

/*
 * Function:
 *      _bcm_telemetry_module_status_scache_alloc
 * Purpose:
 *      Telemetry WB scache alloc for storing module_status
 * Parameters:
 *      unit    - (IN) Unit number.
 *      create  - (IN) 1 - Create, 0 - Realloc
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int _bcm_telemetry_module_status_scache_alloc(int unit, int create)
{
    uint32                  cur_size = 0;
    uint32                  rqd_size = 0;
    int                     rv = BCM_E_NONE;
    soc_scache_handle_t     scache_handle;
    uint8                  *scache = NULL;

    rqd_size += _bcm_telemetry_module_status_scache_size_get(unit);

    SOC_SCACHE_HANDLE_SET(scache_handle, unit,
                          BCM_MODULE_TELEMETRY,
                          _bcmTelemetryWbSequenceModuleStatus);

    if (create) {
        rv = _bcm_esw_scache_ptr_get(unit, scache_handle, 1, rqd_size,
                               &scache, BCM_WB_TELEMETRY_MODULE_STATUS_DEFAULT_VERSION, NULL);
        BCM_IF_ERROR_RETURN(rv);
    } else {
        /* Get current size */
        rv = soc_scache_ptr_get(unit, scache_handle, &scache, &cur_size);
        SOC_IF_ERROR_RETURN(rv);

        if (rqd_size > cur_size) {
            /* Request the extra size */
            rv = soc_scache_realloc(unit, scache_handle, rqd_size - cur_size);
            SOC_IF_ERROR_RETURN(rv);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_xgs5_telemetry_sync
 * Purpose:
 *     Syncs Telemetry data to scache
 * Parameters:
 *     unit   - (IN) Device unit number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_xgs5_telemetry_sync(int unit)
{
    BCM_IF_ERROR_RETURN(_bcm_xgs5_telemetry_config_sync(unit));
    BCM_IF_ERROR_RETURN(_bcm_xgs5_telemetry_module_status_sync(unit));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_telemetry_scache_alloc
 * Purpose:
 *      Telemetry WB scache alloc
 * Parameters:
 *      unit    - (IN) Unit number.
 *      create  - (IN) 1 - Create, 0 - Realloc
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int _bcm_telemetry_scache_alloc(int unit, int create)
{
    BCM_IF_ERROR_RETURN(_bcm_telemetry_config_scache_alloc(unit, create));
    BCM_IF_ERROR_RETURN(_bcm_telemetry_module_status_scache_alloc(unit, create));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_telemetry_wb_config_recover
 * Purpose:
 *      Telemetry WB config recovery
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int _bcm_telemetry_wb_config_recover(int unit)
{
    int                 stable_size;       /* Secondary storage size.   */
    uint8               *scache = NULL;
    soc_scache_handle_t scache_handle;
    int                 rv = BCM_E_NONE;
    uint16              recovered_ver = 0;

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    if (!SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit) && (stable_size > 0)) {
        SOC_SCACHE_HANDLE_SET(scache_handle, unit,
                              BCM_MODULE_TELEMETRY, _bcmTelemetryWbSequenceConfig);

        rv = _bcm_esw_scache_ptr_get(unit, scache_handle, 0, 0,
                                     &scache, BCM_WB_TELEMETRY_CONFIG_DEFAULT_VERSION,
                                     &recovered_ver);
        if (BCM_E_NOT_FOUND == rv) {
            /* Upgrading from SDK release that does not have warmboot state */
            rv = _bcm_telemetry_scache_alloc(unit, 1);
            return rv;
        } else if (BCM_FAILURE(rv)) {
            return rv;
        }

        if (NULL == scache) {
            return BCM_E_NOT_FOUND;
        }
    } else {
        return BCM_E_NONE;
    }

    /* Scache recovery */
    BCM_IF_ERROR_RETURN(_bcm_telemetry_recover(unit, &scache));

    /* Realloc any extra scache that may be needed */
    BCM_IF_ERROR_RETURN(_bcm_telemetry_config_scache_alloc(unit, 0));

    return rv;
}

/*
 * Function:
 *      _bcm_xgs5_telemetry_wb_module_status_recover
 * Purpose:
 *      Find out if the telemetry module was running prior to warmboot
 * Parameters:
 *      unit        - (IN)  BCM device number
 *      init        - (OUT) 1 - Module initialized,
 *                          0 - Module detached
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_xgs5_telemetry_wb_module_status_recover(int unit, int *init)
{
    int                 stable_size;       /* Secondary storage size.   */
    uint8               *scache = NULL;
    soc_scache_handle_t scache_handle;
    int                 rv = BCM_E_NONE;
    uint16              recovered_ver = 0;

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    if (!SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit) && (stable_size > 0)) {
        SOC_SCACHE_HANDLE_SET(scache_handle, unit,
                              BCM_MODULE_TELEMETRY,
                              _bcmTelemetryWbSequenceModuleStatus);

        rv = _bcm_esw_scache_ptr_get(unit, scache_handle, 0, 0,
                                     &scache,
                                     BCM_WB_TELEMETRY_MODULE_STATUS_DEFAULT_VERSION,
                                     &recovered_ver);
        if (BCM_E_NOT_FOUND == rv) {
            /* Upgrading from SDK release that does not have
             * warmboot state for module status
             */
            rv = _bcm_telemetry_module_status_scache_alloc(unit, 1);

            /* Set init = 1 to avoid breaking upgrade from older release */
            *init = 1;
            return rv;
        } else if (BCM_FAILURE(rv)) {
            return rv;
        }

        if (NULL == scache) {
            return BCM_E_NOT_FOUND;
        }
    } else {
        return BCM_E_NONE;
    }

    /* Scache recovery */
    TELEMETRY_SCACHE_READ(scache, *init, uint8);

    /* Realloc any extra scache that may be needed */
    BCM_IF_ERROR_RETURN(_bcm_telemetry_module_status_scache_alloc(unit, 0));

    return BCM_E_NONE;
}

#endif /* BCM_WARM_BOOT_SUPPORT */

/******************************************************
*                                                     *
*        Telemetry APP APIs                           *
 */

STATIC
int bcm_xgs5_telemetry_eapp_detach(int unit)
{
    _bcm_telemetry_global_info_t *telemetry_config_per_unit =
                           TELEMETRY_GLOBAL_INFO_GET(unit);
    int                          rv = BCM_E_NONE;
    int                          uc, status = 0;
    uint16                       reply_len = 0;

    if (telemetry_config_per_unit->uc_loaded == 0) {
         return BCM_E_NONE;
    }
    uc = telemetry_config_per_unit->uC;

    if (!SOC_WARM_BOOT(unit)) {
        /* Un Init the app */
        SOC_IF_ERROR_RETURN(soc_uc_status(unit, uc, &status));
        if (status) {
            rv = _bcm_xgs5_st_msg_send_receive(unit,
                                             MOS_MSG_SUBCLASS_ST_SHUTDOWN,
                                             0, 0,
                                             MOS_MSG_SUBCLASS_ST_SHUTDOWN_REPLY,
                                             &reply_len,
                                             SHR_ST_MAX_UC_MSG_TIMEOUT);
            if (BCM_SUCCESS(rv) && (reply_len != 0)) {
                return BCM_E_INTERNAL;
            }
        }
    }
    telemetry_config_per_unit->uc_loaded = 0;

    /*
     * Free DMA buffer
 */
    if (telemetry_config_per_unit->dma_buffer != NULL) {
        soc_cm_sfree(unit, telemetry_config_per_unit->dma_buffer);
    }

    return BCM_E_NONE;
}

#define _BCM_TELEMETRY_TH3_MCQ_MODE_DEFAULT 2

STATIC
int _bcm_xgs5_st_eapp_send_hw_config_msg(int unit)
{
    shr_st_hw_config_t hw_config_msg;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    _bcm_telemetry_global_info_t *telemetry_config_per_unit =
                           TELEMETRY_GLOBAL_INFO_GET(unit);
    int mc_q_mode, num_mcq;

    mc_q_mode = soc_property_get(unit, spn_MMU_PORT_NUM_MC_QUEUE,
                                 _BCM_TELEMETRY_TH3_MCQ_MODE_DEFAULT);
    num_mcq = (mc_q_mode == 0) ? 0 : (mc_q_mode == 1) ? 2 :
              (mc_q_mode == 2) ? 4 : (mc_q_mode == 3) ? 6 : 4;

    sal_memset(&hw_config_msg, 0, sizeof(hw_config_msg));
    hw_config_msg.config_value[SHR_ST_CONFIG_PORT_NUM_MC_QUEUES] = num_mcq;
    hw_config_msg.num_config_elements = 1;

    buffer_req      = telemetry_config_per_unit->dma_buffer;
    buffer_ptr      = shr_st_msg_ctrl_config_pack(buffer_req, &hw_config_msg);
    buffer_len      = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN(_bcm_xgs5_st_msg_send_receive(unit,
                                               MOS_MSG_SUBCLASS_ST_CONFIG,
                                               buffer_len, 0,
                                               MOS_MSG_SUBCLASS_ST_CONFIG_REPLY,
                                               &reply_len,
                                               SHR_ST_MAX_UC_MSG_TIMEOUT));
    return BCM_E_NONE;
}

STATIC
int _bcm_xgs5_st_eapp_send_ctrl_init_msg(int unit)
{
    st_sdk_msg_ctrl_init_t ctrl_init_msg;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    _bcm_telemetry_global_info_t *telemetry_config_per_unit =
                           TELEMETRY_GLOBAL_INFO_GET(unit);


    sal_memset(&ctrl_init_msg, 0, sizeof(ctrl_init_msg));
    ctrl_init_msg.max_export_pkt_length =
                                  telemetry_config_per_unit->max_pkt_size;
    /*Init time in milli seconds  as per proto file*/
    ctrl_init_msg.init_time_l = (sal_time() *1000);
    ctrl_init_msg.num_instances = BCM_INT_TELEMETRY_MAX_INSTANCES;
    ctrl_init_msg.num_collectors = 1;
    if (ST_UC_FEATURE_CHECK(ST_MAX_NUM_PORTS)) {
        ctrl_init_msg.max_num_ports  = telemetry_config_per_unit->max_num_ports;
    }

    buffer_req      = telemetry_config_per_unit->dma_buffer;
    buffer_ptr      = st_sdk_msg_ctrl_init_pack(buffer_req, &ctrl_init_msg);
    buffer_len      = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN(_bcm_xgs5_st_msg_send_receive(unit,
                                                 MOS_MSG_SUBCLASS_ST_INIT,
                                                 buffer_len, 0,
                                                 MOS_MSG_SUBCLASS_ST_INIT_REPLY,
                                                 &reply_len,
                                                 SHR_ST_MAX_UC_MSG_TIMEOUT));
    return BCM_E_NONE;
}

STATIC
int bcm_xgs5_telemetry_eapp_init(int unit)
{
    _bcm_telemetry_global_info_t *telemetry_config_per_unit =
                           TELEMETRY_GLOBAL_INFO_GET(unit);
    int rv = BCM_E_NONE;
    int uc = 1;

    /* Init the app, determine which core is running the app by chooosing
     * the first uC that returns sucessfully
     */
    for (uc = 0; uc < CMICM_NUM_UCS; uc++) {
        if (!soc_uc_in_reset(unit, uc)) {
            rv = soc_cmic_uc_appl_init(unit, uc, MOS_MSG_CLASS_ST,
                                       SHR_ST_MAX_UC_MSG_TIMEOUT,
                                       ST_SDK_VERSION, ST_UC_MIN_VERSION,
                                       NULL, NULL);
            if (SOC_E_NONE == rv) {
                telemetry_config_per_unit->uc_loaded = 1;
                break;
            }
        }
    }

    if (telemetry_config_per_unit->uc_loaded == 1) {
        telemetry_config_per_unit->uC = uc;
    }  else {
        /* Could not find or start Telemetry appl */
        LOG_WARN(BSL_LS_BCM_BFD,
                 (BSL_META_U(unit,
                             "uKernel Telemetry application not available\n")));
        return BCM_E_NONE;
    }

    /*
     * Allocate DMA buffer
     *
     * DMA buffer will be used to send and receive 'long' messages
     * between SDK Host and uController (BTE).
     */
    telemetry_config_per_unit->dma_buffer_len = sizeof(shr_st_msg_ctrl_t);
    telemetry_config_per_unit->dma_buffer = soc_cm_salloc(unit,
                                      telemetry_config_per_unit->dma_buffer_len,
                                      "Telemetry DMA buffer");
    if (!telemetry_config_per_unit->dma_buffer) {
        BCM_IF_ERROR_RETURN(bcm_xgs5_telemetry_eapp_detach(unit));
        return BCM_E_MEMORY;
    }
    sal_memset(telemetry_config_per_unit->dma_buffer, 0,
               telemetry_config_per_unit->dma_buffer_len);

    if (!SOC_WARM_BOOT(unit)) {
        /* Send the init config to UKERNEL */
        rv = _bcm_xgs5_st_eapp_send_ctrl_init_msg(unit);
        if (BCM_FAILURE(rv)) {
            BCM_IF_ERROR_RETURN(bcm_xgs5_telemetry_eapp_detach(unit));
            return rv;
        }
        if (SOC_IS_TOMAHAWK3(unit)) {
            rv = _bcm_xgs5_st_eapp_send_hw_config_msg(unit);
            if (BCM_FAILURE(rv)) {
                BCM_IF_ERROR_RETURN(bcm_xgs5_telemetry_eapp_detach(unit));
                return rv;
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs5_telemetry_detach
 * Purpose:
 *      Detaches the Telemetry module.
 * Parameters:
 *      unit         - (IN) BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_xgs5_telemetry_detach(int unit)
{
    _bcm_telemetry_global_info_t *telemetry_config_per_unit =
                           TELEMETRY_GLOBAL_INFO_GET(unit);
    int rv = BCM_E_NONE;

    if (telemetry_config_per_unit == NULL) {
        /* Telemetry not initialized */
        return BCM_E_NONE;
    }
    /* De-init the embedded app */
    rv = bcm_xgs5_telemetry_eapp_detach(unit);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Counter module settings */
    if (SOC_CONTROL(unit)->egr_perq_xmt_ctr_evict_disable_st) {
        SOC_CONTROL(unit)->egr_perq_xmt_ctr_evict_disable_st = 0;
        rv = soc_counter_egr_perq_xmt_ctr_evict_enable(unit);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                    (BSL_META_U(unit,
                                "TELEMETRY(unit %d) Error: telemetry "
                                " counter enable failed %s\n"), unit, bcm_errmsg(rv)));
            return rv;
        }
    }

    /* Free up telemetry instances list */
    BCM_IF_ERROR_RETURN(_bcm_xgs5_telemetry_instance_list_destroy(unit));

    sal_free(telemetry_config_per_unit);
    telemetry_global_info[unit] = NULL;

    return BCM_E_NONE; 
}

/*
 * Function:
 *      _bcm_xgs5_telemetry_init
 * Purpose:
 *      Initializes the Telemetry module.
 * Parameters:
 *      unit         - (IN) BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_xgs5_telemetry_init(int unit)
{
     _bcm_telemetry_global_info_t *telemetry_config_per_unit =
                             TELEMETRY_GLOBAL_INFO_GET(unit);
    int rv = BCM_E_NONE;
    uint16 max_num_ports = 0;
#ifdef BCM_WARM_BOOT_SUPPORT
    int init = 0;
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        BCM_IF_ERROR_RETURN
            (_bcm_xgs5_telemetry_wb_module_status_recover(unit, &init));

        if (init == 0) {
            /* Module was not initialized prior to WB, skip initializing it now */
            return BCM_E_NONE;
        }
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    max_num_ports = soc_property_get(unit,
                                     spn_TELEMETRY_MAX_PORTS_MONITOR,
                                     SHR_ST_MAX_PORTS);
    if (!max_num_ports) {
        return BCM_E_NONE;
    }

    if (max_num_ports > SHR_ST_MAX_PORTS) {
        LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                (BSL_META_U(unit,
                            "TELEMETRY(unit %d) Error: Invalid "
                            " Maximum ports \n"), unit));
        return BCM_E_CONFIG;
    }

    if (telemetry_config_per_unit != NULL) {
        BCM_IF_ERROR_RETURN(_bcm_xgs5_telemetry_detach(unit));
        telemetry_config_per_unit = NULL;
    }

    _BCM_TELEMETRY_ALLOC(telemetry_config_per_unit, _bcm_telemetry_global_info_t,
                 sizeof(_bcm_telemetry_global_info_t), "TELEMETRY_INFO_PER_UNIT");
    if (telemetry_config_per_unit == NULL) {
        return BCM_E_MEMORY;
    }
    telemetry_global_info[unit] = telemetry_config_per_unit;

    telemetry_config_per_unit->max_pkt_size = soc_property_get(unit,
                                       spn_TELEMETRY_EXPORT_MAX_PACKET_SIZE,
                                       BCM_TELEMETRY_DEFAULT_MAX_EXPORT_LENGTH);
     if ((telemetry_config_per_unit->max_pkt_size <
                            SHR_ST_MAX_COLLECTOR_ENCAP_LENGTH) ||
        (telemetry_config_per_unit->max_pkt_size > SHR_ST_MAX_EXPORT_LENGTH)) {
        LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                (BSL_META_U(unit,
                            "TELEMETRY(unit %d) Error: Invalid "
                            " export pkt size\n"), unit));
        return BCM_E_CONFIG;
    }

    telemetry_config_per_unit->max_num_ports = max_num_ports;

     /* Init the embedded app */
     rv = bcm_xgs5_telemetry_eapp_init(unit);
     if (BCM_FAILURE(rv)) {
         LOG_WARN(BSL_LS_BCM_TELEMETRY,
                 (BSL_META_U(unit,
                             "TELEMETRY(unit %d) Error: telemetry "
                             " App Init failed %s\n"), unit, bcm_errmsg(rv)));
         return rv;
     }

     if (telemetry_config_per_unit->uc_loaded == 0) {
         BCM_IF_ERROR_RETURN(_bcm_xgs5_telemetry_detach(unit));
         return BCM_E_NONE;
     }

     /* Counter module settings */
     rv = soc_counter_egr_perq_xmt_ctr_evict_disable(unit);
     if (BCM_FAILURE(rv)) {
         LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                 (BSL_META_U(unit,
                             "TELEMETRY(unit %d) Error: telemetry "
                             " counter disable failed %s\n"), unit, bcm_errmsg(rv)));
         return rv;
     }
     SOC_CONTROL(unit)->egr_perq_xmt_ctr_evict_disable_st = 1;

    /* Create telemetry list */
    BCM_IF_ERROR_RETURN(_bcm_xgs5_telemetry_instance_list_create(unit));

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        rv = _bcm_telemetry_wb_config_recover(unit);
    } else {
        rv = _bcm_telemetry_scache_alloc(unit, 1);
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    if (rv == BCM_E_NOT_FOUND) {
        /* Continue with initialization if scache not found */
        rv = BCM_E_NONE;
    }
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_TELEMETRY,
                (BSL_META_U(unit,
                            "TELEMETRY(unit %d) Error: telemetry "
                            " Init failed\n"), unit));
        return (rv);
    }

    return BCM_E_NONE;
}

char *_bcm_xgs5_telemetry_object_type_str[bcmTelemetryObjectTypeCount] =
{
    "None",
    "Interface Ingress",
    "Interface Egress",
    "Interface Egress Queue",
    "Interface Ingress Error",
    "INT Metadata",
};

STATIC void
_bcm_xgs5_telemetry_instnace_config_dump(int unit,
        bcm_telemetry_config_t *telemetry_config)
{
    int i = 0;
     LOG_CLI((BSL_META("\t\tTelemetry instance component Id = %u\r\n"),
                    telemetry_config->component_id));
     LOG_CLI((BSL_META("\t\tTelemetry instance gport = %d\r\n"),
                    telemetry_config->gport));
    LOG_CLI((BSL_META_U(unit,"Telemetry port name =  %s\r\n"),
                             telemetry_config->port_name));
    LOG_CLI((BSL_META_U(unit,"Object types configured: \r\n")));
    for (i = 0;i<bcmTelemetryObjectTypeCount; i++) {
        LOG_CLI((BSL_META("\tObject type = %s\r\n"),
        _bcm_xgs5_telemetry_object_type_str[telemetry_config->object_type[i]]));
    }
}

/*
 * Function:
 *     _bcm_xgs5_telemetry_dump
 * Purpose:
 *     Dump the telemetry info.
 * Parameters:
 *     unit          - (IN) BCM device number
 *     telemetry_node - (IN) Telemetry information.
 * Returns:
 *     Nothing
 */
STATIC void
_bcm_xgs5_telemetry_instance_dump (int unit, 
    _bcm_telemetry_instance_info_t *telemetry_instance_node)
{
     int i = 0;

     LOG_CLI((BSL_META("Telemetry instance ID = %d\r\n"),
              telemetry_instance_node->instance_id));
     LOG_CLI((BSL_META("Telemetry instance core ID = %d\r\n"),
              telemetry_instance_node->core));
     LOG_CLI((BSL_META("Telemetry instance config count = %d\r\n"),
              telemetry_instance_node->config_count));
     for (i = 0;i<telemetry_instance_node->config_count; i++) {
         _bcm_xgs5_telemetry_instnace_config_dump(unit,
                 &(telemetry_instance_node->telemetry_config[i]));

     }
     LOG_CLI((BSL_META("Telemetry instance collector ID = %d\r\n"),
              telemetry_instance_node->collector_id));
     LOG_CLI((BSL_META("Telemetry instance export profile ID = %d\r\n"),
              telemetry_instance_node->export_profile_id));
}

/*
 * Function:
 *     _bcm_xgs5_telemetry_list_dump
 * Purpose:
 *     Dump the telemetry list.
 * Parameters:
 *     unit          - (IN) BCM device number
 * Returns:
 *     Nothing
 */
STATIC void
_bcm_xgs5_telemetry_list_dump (int unit)
{
    _bcm_telemetry_instance_info_t *telemetry_instance_node = NULL;
    int i;

    if (telemetry_instances_info_list[unit] == NULL) {
        return;
    }

    for (i = 0;
         i < BCM_INT_TELEMETRY_MAX_INSTANCES; i++) {
        telemetry_instance_node = &(telemetry_instances_info_list[unit][i]);
        if (telemetry_instance_node->instance_id !=
                BCM_TELEMETRY_INVALID_INDEX) {
            _bcm_xgs5_telemetry_instance_dump(unit, telemetry_instance_node);
        }
    } 
}

/* Dump routine for dumping
 * telemetry list and export profile list.
 */
void _bcm_xgs5_telemetry_sw_dump(int unit)
{
    _bcm_telemetry_global_info_t *telemetry_config_per_unit = NULL;

    telemetry_config_per_unit = TELEMETRY_GLOBAL_INFO_GET(unit);

    if (telemetry_config_per_unit == NULL) {
        return;
    }

    LOG_CLI((BSL_META_U(unit,"Telemetry Global Info: \r\n")));
    LOG_CLI((BSL_META_U(unit,"Telemetry System_id =  %s\r\n"),
                telemetry_config_per_unit->system_id));
    LOG_CLI((BSL_META_U(unit,"Telemetry System_id length =  %u\r\n"),
                telemetry_config_per_unit->system_id_length));
    LOG_CLI((BSL_META_U(unit,"Telemetry maximum packet size =  %u\r\n"),
                telemetry_config_per_unit->max_pkt_size));

    _bcm_xgs5_telemetry_list_dump(unit);
}

#else /* INCLUDE_TELEMETRY */
typedef int _bcm_xgs5_telemetry_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_TELEMETRY */
