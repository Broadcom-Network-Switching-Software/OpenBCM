/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shared/alloc.h>
#include <sal/core/libc.h>

#include <soc/cm.h>
#include <soc/drv.h>
#include <shared/bsl.h>

#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/rx.h>
#include <bcm/pkt.h>
/*#include <bcm_int/pkt.h>*/
#include <bcm_int/control.h>

#define UNIT_VALID(unit) \
{ \
  if (!BCM_UNIT_VALID(unit)) { return BCM_E_UNIT; } \
}



char*
  bcm_pkt_dnx_type_to_string(
    bcm_pkt_dnx_type_t dnx_hdr_type
  )
{
  char* str = NULL;
  switch(dnx_hdr_type)
  {
    case bcmPktDnxTypeItmh:
        str = "itmh";
        break;
    case bcmPktDnxTypeFtmh:
        str = "ftmh";
        break;
    case bcmPktDnxTypeOtsh:
        str = "otsh";
        break;
    case bcmPktDnxTypeOtmh:
        str = "otmh";
        break;
    case bcmPktDnxTypeInternals:
        str = "internals";
        break;
    default:
        str = " Unknown";
  }
  return str;
}
char*
  bcm_pkt_dnx_itmh_dest_type_to_string(
     bcm_pkt_dnx_itmh_dest_type_t dest_type
  )
{
  char* str = NULL;
  switch(dest_type)
  {
    case bcmPktDnxItmhDestTypeMulticast:
        str = "Multicast";
        break;
    case bcmPktDnxItmhDestTypeFlow:
        str = "Flow";
        break;
    case bcmPktDnxItmhDestTypeIngressShapingFlow:
        str = "IngressShapingFlow";
        break;
    case bcmPktDnxItmhDestTypeVport:
        str = "Vport";
        break;
    case bcmPktDnxItmhDestTypeSystemPort:
        str = "SystemPort";
        break;
    default:
        str = " Unknown";
  }
  return str;
}

void bcm_pkt_dnx_itmh_dest_dump(
                        bcm_pkt_dnx_itmh_dest_t *itmh_dest
                        )
{

    LOG_VERBOSE(BSL_LS_BCM_PKT,(BSL_META("%s %s %s %s %s %u %s %u %s %u\n\r" ),
             "dest_type:" ,bcm_pkt_dnx_itmh_dest_type_to_string(itmh_dest->dest_type), 
             "dest_extension_type:", bcm_pkt_dnx_itmh_dest_type_to_string(itmh_dest->dest_extension_type), 
             "destination:", itmh_dest->destination,
             "multicast_id:", itmh_dest->multicast_id,
             "destination_ext:", itmh_dest->destination_ext
             ));


}
void bcm_pkt_dnx_itmh_dump(
   bcm_pkt_dnx_itmh_t  *itmh
   )
{

    LOG_VERBOSE(BSL_LS_BCM_PKT,(BSL_META("%s %u %s %u %s %u %s %u\n\r" ),
             "inbound_mirror_disable:" ,itmh->inbound_mirror_disable, 
             "snoop_cmnd:", itmh->snoop_cmnd, 
             "prio:", itmh->prio,
             "color:", itmh->color
             ));



    bcm_pkt_dnx_itmh_dest_dump(&(itmh->dest));


}
char*
 bcm_pkt_dnx_ftmh_action_type_to_string(
      bcm_pkt_dnx_ftmh_action_type_t action_type
  )
{
  char* str = NULL;
  switch(action_type)
  {
    case bcmPktDnxFtmhActionTypeForward:
        str = "Forward";
        break;
    case bcmPktDnxFtmhActionTypeSnoop:
        str = "Snoop";
        break;
    case bcmPktDnxFtmhActionTypeInboundMirror:
        str = "InboundMirror";
        break;
    case bcmPktDnxFtmhActionTypeOutboundMirror:
        str = "OutboundMirror";
        break;
    default:
        str = "Unknown";
  }
  return str;
}


void bcm_pkt_dnx_ftmh_lb_extension_dump(
   bcm_pkt_dnx_ftmh_lb_extension_t  *lb_extension
   )
{
    LOG_VERBOSE(BSL_LS_BCM_PKT,(BSL_META("%s %s %u\n\r" ),
             "lb_extension:" ,
             "lb_key:", lb_extension->lb_key
             ));



}

void bcm_pkt_dnx_ftmh_dest_extension_dump(
   bcm_pkt_dnx_ftmh_dest_extension_t  *dest_extension
   )
{

    LOG_VERBOSE(BSL_LS_BCM_PKT,(BSL_META("%s %s %u\n\r" ),
             "dest_extension:" ,
             "dst_sysport:", dest_extension->dst_sysport
             ));


}
void bcm_pkt_dnx_ftmh_stack_extension_dump(
   bcm_pkt_dnx_ftmh_stack_extension_t  *stack_extension
   )
{

    LOG_VERBOSE(BSL_LS_BCM_PKT,(BSL_META("\t\t %s %s %u\n\r" ),
             "stack_extension:" ,
             "stack_route_history_bmp:", stack_extension->stack_route_history_bmp
             ));

}

void bcm_pkt_dnx_ftmh_dump(
   bcm_pkt_dnx_ftmh_t  *ftmh
   )
{

    LOG_VERBOSE(BSL_LS_BCM_PKT,(BSL_META("%s %u %s %u %s %u %s %u\n\r \
                      %s %u %s %s %s %u %s %u\n\r \
                      %s %u %s %u %s %u\n\r" ), 
             "packet_size:" ,ftmh->packet_size, 
             "prio:", ftmh->prio, 
             "src_sysport:", ftmh->src_sysport,
             "dst_port:", ftmh->dst_port,
             "ftmh_dp:" ,ftmh->ftmh_dp,
             "action_type:", bcm_pkt_dnx_ftmh_action_type_to_string(ftmh->action_type),
             "out_mirror_disable:", ftmh->out_mirror_disable,
             "is_mc_traffic:" ,ftmh->is_mc_traffic,
             "multicast_id:", ftmh->multicast_id,
             "out_vport:", ftmh->out_vport,
             "cni:", ftmh->cni));

    if (ftmh->lb_ext.valid) {
        bcm_pkt_dnx_ftmh_lb_extension_dump(&(ftmh->lb_ext));
    }
    if (ftmh->dest_ext.valid ) {
        bcm_pkt_dnx_ftmh_dest_extension_dump(&(ftmh->dest_ext));
    }
    if (ftmh->stack_ext.valid ) {
        bcm_pkt_dnx_ftmh_stack_extension_dump(&(ftmh->stack_ext));
    }
}
 char*
 bcm_pkt_dnx_otsh_type_to_string(
      bcm_pkt_dnx_otsh_type_t otsh_type
  )
{
  char* str = NULL;
  switch(otsh_type)
  {
    case bcmPktDnxOtshTypeOam:
        str = "Oam";
        break;
    case bcmPktDnxOtshType1588v2:
        str = "L588v2";
        break;
    case bcmPktDnxOtshTypeReserved:
        str = "Reserved";
        break;
    case bcmPktDnxOtshTypeLatency:
        str = "Latency";
        break;
    default:
        str = "Unknown";
  }
  return str;
}

 char*
 bcm_pkt_dnx_otsh_oam_subtype_to_string(
     bcm_pkt_dnx_otsh_oam_subtype_t otsh_subtype
  )
{
  char* str = NULL;
  switch(otsh_subtype)
  {
    case bcmPktDnxOtshOamSubtypeNone:
        str = "None";
        break;
    case bcmPktDnxOtshOamSubtypeLm:
        str = "Lm";
        break;
    case bcmPktDnxOtshOamSubtypeDm1588:
        str = "Dm1588";
        break;
    case bcmPktDnxOtshOamSubtypeDmNtp:
        str = "DmNtp";
        break;
    case bcmPktDnxOtshOamSubtypeOamDefault:
        str = "Default";
        break;
    case bcmPktDnxOtshOamSubtypeLoopback:
        str = "Loopback";
        break;
    case bcmPktDnxOtshOamSubtypeEcn:
        str = "Ecn";
        break;
    default:
        str = "Unknown";
  }
  return str;
}
void bcm_pkt_dnx_otsh_dump(
   bcm_pkt_dnx_otsh_t  *otsh
   )
{
#ifdef COMPILER_HAS_LONGLONG
    LOG_VERBOSE(BSL_LS_BCM_PKT,(BSL_META("%s %s %s %s %s %u %s %u %s %u %s %u %s %u %s %llu (0x%llx) \n\r"),
                    "otsh_type:",bcm_pkt_dnx_otsh_type_to_string(otsh->otsh_type),
                    "oam_sub_type:",bcm_pkt_dnx_otsh_oam_subtype_to_string(otsh->oam_sub_type),
                    "oam_up_mep:",otsh->oam_up_mep,
                    "tp_cmd:", otsh->tp_cmd,
                    "ts_encap:", otsh->ts_encap,
                    "latency_flow_ID:", otsh->latency_flow_ID,
                    "offset:", otsh->offset,
                    "data",(long long unsigned)otsh->oam_ts_data,(long long unsigned)otsh->oam_ts_data 
                   ));
#else
    LOG_VERBOSE(BSL_LS_BCM_PKT,(BSL_META("%s %s %s %s %s %u %s %u %s %u %s %u %s %u %s %u %u \n\r"),
                    "otsh_type:",bcm_pkt_dnx_otsh_type_to_string(otsh->otsh_type),
                    "oam_sub_type:",bcm_pkt_dnx_otsh_oam_subtype_to_string(otsh->oam_sub_type),
                    "oam_up_mep:",otsh->oam_up_mep,
                    "tp_cmd:", otsh->tp_cmd,
                    "ts_encap:", otsh->ts_encap,
                    "latency_flow_ID:", otsh->latency_flow_ID,
                    "offset:", otsh->offset,
                    "data",COMPILER_64_HI(otsh->oam_ts_data),COMPILER_64_LO(otsh->oam_ts_data)
                   ));

#endif

}

void bcm_pkt_dnx_otmh_src_sysport_extension_dump(
   bcm_pkt_dnx_otmh_src_sysport_extension_t  *ext
   )
{
    LOG_VERBOSE(BSL_LS_BCM_PKT,(BSL_META("%s %s %u\n\r"),
                    "otmh_src_sysport_extension:",
                    "src_sysport:", ext->src_sysport
                   ));

}
void bcm_pkt_dnx_otmh_vport_extension_dump(
   bcm_pkt_dnx_otmh_vport_extension_t  *ext
   )
{

    LOG_VERBOSE(BSL_LS_BCM_PKT,(BSL_META("%s %s %u\n\r"),
                    "otmh_vport_extension:",
                    "out_vport", ext->out_vport
                   ));

}
void bcm_pkt_dnx_otmh_dump(
   bcm_pkt_dnx_otmh_t  *otmh
   )
{
    LOG_VERBOSE(BSL_LS_BCM_PKT,(BSL_META("%s %s %s %u %s %u %s %u %s %u\n\r"),
                    "action_type:",bcm_pkt_dnx_ftmh_action_type_to_string(otmh->action_type),
                    "ftmh_dp:",otmh->ftmh_dp,
                    "is_mc_traffic:",otmh->is_mc_traffic,
                    "prio:", otmh->prio,
                    "dst_port:", otmh->dst_port
                   ));
    if (otmh->src_sysport_ext.valid) {
        bcm_pkt_dnx_otmh_src_sysport_extension_dump(&otmh->src_sysport_ext);
    }

    if (otmh->out_vport_ext.valid) {
        bcm_pkt_dnx_otmh_vport_extension_dump(&otmh->out_vport_ext);
    }

}


void bcm_pkt_dnx_internal_dump(
    bcm_pkt_dnx_internal_t *pkt
   )
{  
    LOG_VERBOSE(BSL_LS_BCM_PKT,(BSL_META("%s %u %s %u\n\r"),
                    "trap_qualifier:", pkt->trap_qualifier,
                    "trap_id:", pkt->trap_id
                   ));

    LOG_VERBOSE(BSL_LS_BCM_PKT,(BSL_META("\n\r")));

}
void bcm_pkt_dnx_dump(
    bcm_pkt_t *pkt
   )
{
    int i;

    LOG_VERBOSE(BSL_LS_BCM_PKT,(BSL_META("dnx_header_count: %u\n\r"),pkt->dnx_header_count));

    for (i = 0; i < pkt->dnx_header_count; i++) {
        /*dnx_pkt = (bcm_pkt_dnx_t *)&(pkt->dnx_header_stack[i]);*/
 
        LOG_VERBOSE(BSL_LS_BCM_PKT,(BSL_META("===Header : %u======\n\r"), i));

        LOG_VERBOSE(BSL_LS_BCM_PKT,(BSL_META("type: %s \n\r"), bcm_pkt_dnx_type_to_string(pkt->dnx_header_stack[i].type)));

        switch (pkt->dnx_header_stack[i].type) {
            case bcmPktDnxTypeItmh:
                bcm_pkt_dnx_itmh_dump((bcm_pkt_dnx_itmh_t *)&(pkt->dnx_header_stack[i].itmh));
                break;
            case bcmPktDnxTypeFtmh:
                bcm_pkt_dnx_ftmh_dump((bcm_pkt_dnx_ftmh_t *)&(pkt->dnx_header_stack[i].ftmh));
                break;
            case bcmPktDnxTypeOtsh:
                bcm_pkt_dnx_otsh_dump(&(pkt->dnx_header_stack[i].otsh));
                break;
            case bcmPktDnxTypeOtmh:
                bcm_pkt_dnx_otmh_dump(&(pkt->dnx_header_stack[i].otmh));
                break;
            default:
                bcm_pkt_dnx_internal_dump(&(pkt->dnx_header_stack[i].internal));
                break;
        }

    }

}





