/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 *
 * File:        cint_tdm_bypass_mode.c
 * Purpose:     example for configure tdm bypass mode
 * 
 * *
 * The cint (+ Soc properties) contain:
 * 1. Ingress tdm bypass mode configuration. cint works with stream_id_enable=0 (SID extract table disable).
 * 2. 
 *  SOC property configuration:
 *  ---------------------------
 *  config add tdm_mode=TDM_OPTIMIZED
 *  tdm port configuration, for example - config add ucode_port_5.$UNIT=ILKN2.0:core_0.5:egr_ilv:tdm:if_tdm_hybrid
 *  config delete flexe_device_mode.* 
 */


/**
 * \brief
 *  Example for tdm configuration, bypass mode. 
 *  It determine a path from source port to destination port for tdm stream.
 * \param [in] unit -unit id
 * \param [in] src_port - source port
 * \param [in] dst_port - destination port
 * \param [in] stream_id_base - stream id base
 * \return
 *   error indication.
 * \remark
 *   NONE
 * \see
 *   NONE
 */
int
tdm_bypass_mode_example(
    int unit,
    int src_port,
    int dst_port,
    int stream_id_base)
{
    int rv;
    int set_stream_context;
    bcm_gport_t gport;
    bcm_tdm_interface_config_t set_interface_config;
    bcm_tdm_ingress_context_t ingress_context;
    bcm_tdm_egress_editing_t editing;
    int channel_id; 
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    uint32 flags = 0;
    int stream_id_key_size;
    /* Get the first phy of the inquired port */
    rv = bcm_port_get(unit, src_port, &flags, &interface_info, &mapping_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_port_get\n");
        return rv;
    }   

    if (interface_info.interface == BCM_PORT_IF_ILKN)
    {
        channel_id = mapping_info.channel;
        stream_id_key_size = 256; /* for 256 channels in ilkn interface */
    }
    else
    {
        channel_id = 0;
        stream_id_key_size = 1;     
    }
    BCM_GPORT_LOCAL_SET(gport, src_port);
    set_interface_config.stream_id_base = stream_id_base;
    set_interface_config.stream_id_key_size = 1;
    set_interface_config.stream_id_key_offset = 0;
    set_interface_config.stream_id_enable = 0;

    rv = bcm_tdm_interface_config_set(unit, 0, gport, &set_interface_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_tdm_interface_config_set \n");
        return rv;
    }

    /** Create Ingress Context */
    bcm_tdm_ingress_editing_context_t_init(&ingress_context);
    BCM_GPORT_MODPORT_SET(ingress_context.destination_port, unit, dst_port);
    ingress_context.is_mc = FALSE;
    ingress_context.type = bcmTdmIngressEditingPrepend;
    ingress_context.stamp_mcid_with_sid = 0;
    rv = bcm_tdm_ingress_context_create(unit, 0, &set_stream_context, &ingress_context);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_tdm_ingress_context_create \n");
        return rv;
    }

    /** Fill all stream of the interface with the same context */
    BCM_GPORT_TDM_STREAM_SET(gport, set_interface_config.stream_id_base + channel_id);
    rv = bcm_tdm_stream_ingress_context_set(unit, 0, gport, set_stream_context);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_tdm_stream_ingress_context_set \n");
        return rv;
    }

    /** Set egress editing to: Prepended no header to outgoing TDM packet. */   
    BCM_GPORT_LOCAL_SET(gport, dst_port);
    sal_memset(&editing, 0, sizeof(editing));
    flags = 0;
    editing.type = bcmTdmEgressEditingNoHeader;
    rv = bcm_tdm_egress_editing_set(unit, flags, gport, &editing);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_tdm_egress_editing_set \n");
        return rv;
    }    
    return BCM_E_NONE;
}
