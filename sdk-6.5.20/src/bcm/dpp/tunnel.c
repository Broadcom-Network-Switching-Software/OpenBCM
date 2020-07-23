/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_TUNNEL

#include <shared/bsl.h>

#include <bcm_int/common/debug.h>

#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/alloc_mngr_local_lif.h>
#include <bcm_int/dpp/alloc_mngr_glif.h>
#include <bcm_int/dpp/tunnel.h>


#include <bcm_int/control.h>
#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/l2.h>
#include <bcm_int/dpp/l3.h>
#include <bcm_int/dpp/mpls.h>
#include <bcm_int/dpp/qos.h>
#include <bcm/multicast.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/dpp/multicast.h>
#include <bcm_int/dpp/vlan.h>
#include <bcm_int/dpp/mirror.h>
#include <bcm_int/petra_dispatch.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/mpls.h>
#include <bcm_int/dpp/vswitch.h>
#include <bcm_int/dpp/tunnel.h>
#include <bcm_int/dpp/failover.h>
#include <bcm_int/common/field.h>

#include <soc/dpp/mbcm_pp.h>

#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/PPD/ppd_api_llp_parse.h>
#include <soc/dpp/PPD/ppd_api_lif.h>
#include <soc/dpp/PPD/ppd_api_rif.h>
#include <soc/dpp/PPD/ppd_api_frwrd_ilm.h>
#include <soc/dpp/PPD/ppd_api_frwrd_fec.h>
#include <soc/dpp/PPC/ppc_api_eg_encap.h>
#include <soc/dpp/PPC/ppc_api_port.h>

#include <soc/dpp/JER/JER_PP/jer_pp_eg_encap.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lif.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_ptp.h>

#include <shared/swstate/access/sw_state_access.h>
#include <shared/swstate/sw_state_sync_db.h>

static uint8 conn_to_np_enable;
static int conn_to_np_enable_read_soc = 0;
/* Local defines */


#define BCM_DPP_TUNNEL_INIT_CHECK \
    {                                                                                           \
        int _init;                                                                              \
        BCMDNX_IF_ERR_EXIT(L3_ACCESS.dpp_l3_state.init.get(unit, &_init));                     \
        if ((!_init)) {                                                                         \
            BCMDNX_ERR_EXIT_MSG(BCM_E_INIT, (_BSL_BCM_MSG("unit %d not initialised"), unit));   \
        }                                                                                       \
    }

#define _BCM_PETRA_TUNNEL_TERM_FLAGS_UNSUPPORTED \
 (BCM_TUNNEL_INIT_USE_INNER_DF         |     BCM_TUNNEL_TERM_KEEP_INNER_DSCP | \
 BCM_TUNNEL_TERM_WLAN_REMOTE_TERMINATE |     BCM_TUNNEL_TERM_WLAN_SET_ROAM   | \
 BCM_TUNNEL_INIT_IPV4_SET_DF           |     BCM_TUNNEL_INIT_IPV6_SET_DF | \
 BCM_TUNNEL_INIT_IP4_ID_SET_FIXED      |     BCM_TUNNEL_INIT_IP4_ID_SET_RANDOM | \
 BCM_TUNNEL_INIT_WLAN_MTU             |     BCM_TUNNEL_INIT_WLAN_FRAG_ENABLE | \
 BCM_TUNNEL_INIT_WLAN_VLAN_TAGGED     |     BCM_TUNNEL_INIT_WLAN_TUNNEL_WITH_ID)


 
/*
 * when traversing the ILM, to perfrom action on each entry how many 
 * entries to return in each iteration 
 */

    /* is the given gport working gport in protection, assuming gport has protection*/
#define _BCM_PPD_TUNNEL_IS_WORKING_PORT(gport) ((gport)->failover_tunnel_id != BCM_GPORT_TYPE_NONE)
/*
 * MACROs
 */

/* set DF flag in tos-index*/
#define _BCM_DPP_IPV4_TUNNEL_DF_IN_TOS_SET(__tos_index, _df_value)  (__tos_index |= (_df_value) << 3)

#define _BCM_DPP_IPV4_TUNNEL_DF_FROM_TOS_GET(__tos_index)  (((__tos_index) >>  3) & 1)

int _bcm_ip_tunnel_sw_init(int unit) 
{
    SOC_PPC_EG_ENCAP_IP_TUNNEL_SRC_IP ip_tunnel_src_ip;
    SOC_PPC_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_INFO ip_tunnel_size_protocol_template_val;
    BCMDNX_INIT_FUNC_DEFS;

    /* initialize ip tunnel template to zeros */
    SOC_PPC_EG_ENCAP_IP_TUNNEL_SRC_IP_clear(&ip_tunnel_src_ip);
    SOC_PPC_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_INFO_clear(&ip_tunnel_size_protocol_template_val);

    /* set template management that are tunnels point to invalid profile */
    /* set template management to port to -1 data with dummy profile, -1 means use dummy profile, ip = uint(-1) is not valid as tunnel source */
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_template_ip_tunnel_init(unit,-1,&ip_tunnel_src_ip,-1,0xFFFFFFFF,-1,0xFFFFFFFF, -1, &ip_tunnel_size_protocol_template_val));


 exit:
    BCMDNX_FUNC_RETURN;
}

/*********** SW DBs translation functions *******************/
int _bcm_dpp_out_lif_l2_encap_match_add(int unit, int lif)
{
    _bcm_dpp_gport_sw_resources gport_sw_resources;
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_local_lif_to_sw_resources(unit, -1, lif, _BCM_DPP_GPORT_SW_RESOURCES_EGRESS, &gport_sw_resources);
    BCMDNX_IF_ERR_EXIT(rv);

    gport_sw_resources.out_lif_sw_resources.lif_type = _bcmDppLifTypeCpuL2Encap;

    rv = _bcm_dpp_local_lif_sw_resources_set(unit, -1, lif, _BCM_DPP_GPORT_SW_RESOURCES_EGRESS, &gport_sw_resources);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_in_lif_tunnel_term_add(int unit, bcm_tunnel_terminator_t *tunnel_term, int lif)
{
    _bcm_dpp_gport_sw_resources gport_sw_resources;
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_local_lif_to_sw_resources(unit, lif, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS, &gport_sw_resources);
    BCMDNX_IF_ERR_EXIT(rv);

    gport_sw_resources.in_lif_sw_resources.criteria = tunnel_term->type;
    gport_sw_resources.in_lif_sw_resources.flags = tunnel_term->flags;
    gport_sw_resources.in_lif_sw_resources.port = tunnel_term->tunnel_id;
    gport_sw_resources.in_lif_sw_resources.match1 = tunnel_term->dip;
    gport_sw_resources.in_lif_sw_resources.match2 = tunnel_term->sip;
    gport_sw_resources.in_lif_sw_resources.lif_type = _bcmDppLifTypeIpTunnel;

    rv = _bcm_dpp_local_lif_sw_resources_set(unit, lif, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS, &gport_sw_resources);
    BCMDNX_IF_ERR_EXIT(rv); 


    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_in_lif_tunnel_term_get(int unit, bcm_tunnel_terminator_t *tunnel_term, int lif)
{
    _bcm_dpp_gport_sw_resources gport_sw_resources;
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_local_lif_to_sw_resources(unit, lif, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS, &gport_sw_resources);
    BCMDNX_IF_ERR_EXIT(rv);

    if(gport_sw_resources.in_lif_sw_resources.lif_type != _bcmDppLifTypeIpTunnel && gport_sw_resources.in_lif_sw_resources.lif_type != _bcmDppLifTypeL2Gre
       && gport_sw_resources.in_lif_sw_resources.lif_type != _bcmDppLifTypeVxlan) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("LIF is not pointing to IP tunnel Term")));
    }

    tunnel_term->type = gport_sw_resources.in_lif_sw_resources.criteria;
    tunnel_term->flags = gport_sw_resources.in_lif_sw_resources.flags;
    tunnel_term->tunnel_id = gport_sw_resources.in_lif_sw_resources.port;
    tunnel_term->dip = gport_sw_resources.in_lif_sw_resources.match1;
    tunnel_term->sip = gport_sw_resources.in_lif_sw_resources.match2;

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_dpp_out_lif_tunnel_initiator_match_add(int unit, bcm_tunnel_initiator_t *tunnel_init, int lif)
{
    _bcm_dpp_gport_sw_resources gport_sw_resources;
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_local_lif_to_sw_resources(unit, -1, lif, _BCM_DPP_GPORT_SW_RESOURCES_EGRESS, &gport_sw_resources);
    BCMDNX_IF_ERR_EXIT(rv);

    if (SOC_DPP_CONFIG(unit)->pp.lawful_interception_enable && (tunnel_init->type == bcmTunnelTypeVxlan)) {
        gport_sw_resources.out_lif_sw_resources.lif_type = _bcmDppLifTypeVxlan;
    }
    else {
    gport_sw_resources.out_lif_sw_resources.lif_type = _bcmDppLifTypeIpTunnel;
    }

    /* set outlif flag to mark GRE with LB key tunnel*/
    if ((tunnel_init->type == bcmTunnelTypeGreAnyIn4) &&
        (tunnel_init->flags & BCM_TUNNEL_INIT_GRE_KEY_USE_LB)) {
        gport_sw_resources.out_lif_sw_resources.flags |= _BCM_DPP_OUTLIF_IP_TUNNEL_MATCH_INFO_GRE_WITH_LB;
    }

    rv = _bcm_dpp_local_lif_sw_resources_set(unit, -1, lif, _BCM_DPP_GPORT_SW_RESOURCES_EGRESS, &gport_sw_resources);
    BCMDNX_IF_ERR_EXIT(rv);
        
    BCM_EXIT;

exit:
    BCMDNX_FUNC_RETURN;
}



int _bcm_ip_tunnel_sw_cleanup(
    int unit)
{
    BCMDNX_INIT_FUNC_DEFS;
    /* Nothing to do here for now */
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_petra_tunnel_type_support_check
 * Purpose:
 *      Check if tunnel type is supported on a device. 
 * Parameters:
 *      unit        - (IN) Bcm device number.
 *      tnl_type    - (IN) Tunnel type to check.   
 *      tunnel_term - (OUT) Type supported as tunnel terminator.   
 *      tunnel_init - (OUT) Type supported as tunnel initiator.   
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_petra_tunnel_type_support_check(int unit, bcm_tunnel_type_t tnl_type, 
                                     int *tunnel_term, int *tunnel_init)
{
    int term;      /* Tunnel Terminator supported. */
    int init;      /* Tunnel Initiator supported.  */

    switch (tnl_type) {
    case bcmTunnelTypeIpAnyIn4:
    case bcmTunnelTypeGreAnyIn4:
    case bcmTunnelTypeIpAnyIn6:
    case bcmTunnelTypeGreAnyIn6:
          term = TRUE;
          init = TRUE; /* at initiator need explicit type */
          break;
    /* ARAD, Egress only */
    case bcmTunnelTypeErspan: /* ERSPAN tunnel, has to be above IPv4 tunnel */
    case bcmTunnelTypeRspan: /* */
      term = FALSE; 
      init = TRUE; 
      break;

    case bcmTunnelTypeIp4In6: /* for IPv4 need above header type */
    case bcmTunnelTypeIp6In6:
    case bcmTunnelTypeEtherIp4In4:
    case bcmTunnelTypeL2Gre:
    case bcmTunnelTypeVxlan:   
    case bcmTunnelTypeVxlan6:
    case bcmTunnelTypeL2EncapExternalCpu:
        init = term = TRUE; 
        break;
    case bcmTunnelTypeUdp:
        if (SOC_DPP_CONFIG(unit)->pp.udp_tunnel_enable) {
            init = term = TRUE;
        } else {
            init = term = FALSE;
        }
        break;
    case bcmTunnelTypeIp4In4:
    case bcmTunnelTypeIp6In4:      
        init = FALSE;
        term = (SOC_DPP_CONFIG(unit)->pp.ipv4_tunnel_term_bitmap_enable & SOC_DPP_IP_TUNNEL_TERM_DB_DIP_SIP_NEXT_PROTOCOL);
      break;
    default:
          term = init = FALSE;
    }

    if (NULL != tunnel_term) {
        *tunnel_term = term;     
    }
    if (NULL != tunnel_init) {
        *tunnel_init = init;     
    }
    return (BCM_E_NONE);
}

/* setup template, encapsulation size and IP.protocol according to tunnel type
   supported From jericho B0 / QAX */
int
_bcm_petra_tunnel_ipv4_type_to_ppd_template(
    int unit,
    bcm_tunnel_type_t tunnel_type,
    _bcm_tunnel_subtype_t tunnel_subtype,
    SOC_PPC_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_INFO *ip_tunnel_size_protocol_template
 )
{
    BCMDNX_INIT_FUNC_DEFS;

    /* check if ip tunnel template is supported */
    if (!SOC_IS_JERICHO_B0_AND_ABOVE(unit) || !(SOC_JER_PP_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_ENABLE == 1)) {
        BCMDNX_IF_ERR_EXIT(BCM_E_INTERNAL);
    }

    SOC_PPC_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_INFO_clear(ip_tunnel_size_protocol_template);

    switch(tunnel_type) {
        /* simple IPv4 */
        case bcmTunnelTypeIpAnyIn4:          /* RFC 2003/2893: IPv6/4-in-IPv4 tunnel. */
        case bcmTunnelTypeIpAnyIn6:
            /* setup template, ip tunnel size and IP.protocol values */
            ip_tunnel_size_protocol_template->ip_tunnel_template = SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_NONE; /* template value match encapsulation mode value in Arad/Jericho */
            ip_tunnel_size_protocol_template->encapsulation_size = (SOC_SAND_PP_IP_NO_OPTION_NOF_BITS + 0) / SOC_SAND_NOF_BITS_IN_BYTE;
            ip_tunnel_size_protocol_template->protocol = 0;
            ip_tunnel_size_protocol_template->protocol_enable = FALSE;
        break;
        /* IPv4 with GRE */
        case bcmTunnelTypeGreAnyIn4:
        case bcmTunnelTypeGreAnyIn6:
            if (tunnel_subtype == _bcm_tunnel_subtype_gre_with_lb) {
                /* GRE with LB key use GRE8 template, when bcmTunnelTypeGreAnyIn4 type and BCM_TUNNEL_INIT_GRE_KEY_USE_LB flag are set */
                ip_tunnel_size_protocol_template->ip_tunnel_template = SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_ENHANCE_GRE; /* template value match encapsulation mode value in Arad/Jericho */
                ip_tunnel_size_protocol_template->encapsulation_size = (SOC_SAND_PP_IP_NO_OPTION_NOF_BITS + SOC_SAND_PP_GRE8_NOF_BITS) / SOC_SAND_NOF_BITS_IN_BYTE;
                ip_tunnel_size_protocol_template->protocol = SOC_SAND_PP_IP_PROTOCOL_GRE;
                ip_tunnel_size_protocol_template->protocol_enable = TRUE;
            } else {
                /* setup template, ip tunnel size and IP.protocol values */
                ip_tunnel_size_protocol_template->ip_tunnel_template = SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_BASIC_GRE; /* template value match encapsulation mode value in Arad/Jericho */
                ip_tunnel_size_protocol_template->encapsulation_size = (SOC_SAND_PP_IP_NO_OPTION_NOF_BITS + SOC_SAND_PP_GRE4_NOF_BITS) / SOC_SAND_NOF_BITS_IN_BYTE;
                ip_tunnel_size_protocol_template->protocol = SOC_SAND_PP_IP_PROTOCOL_GRE;
                ip_tunnel_size_protocol_template->protocol_enable = TRUE;
            }
        break;
        /* In Arad, VxLan encapsulation mode is the same as L2GRE. */
        case bcmTunnelTypeVxlan:
        case bcmTunnelTypeVxlan6:
            /* setup template, ip tunnel size and IP.protocol values */
            ip_tunnel_size_protocol_template->ip_tunnel_template = SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_VXLAN; /* template value match encapsulation mode value in Arad/Jericho */
            ip_tunnel_size_protocol_template->encapsulation_size = (SOC_SAND_PP_IP_NO_OPTION_NOF_BITS + SOC_SAND_PP_UDP_NOF_BITS + SOC_SAND_PP_VXLAN_NOF_BITS) / SOC_SAND_NOF_BITS_IN_BYTE;
            if (SOC_DPP_CONFIG(unit)->pp.lawful_interception_enable) {
                /* set to 4B's less to create a correct IPv4 length for Lawful Interception */
                ip_tunnel_size_protocol_template->encapsulation_size -= 4;
            }
            ip_tunnel_size_protocol_template->protocol = SOC_SAND_PP_IP_PROTOCOL_UDP;
            ip_tunnel_size_protocol_template->protocol_enable = TRUE;
        break;
        case bcmTunnelTypeL2Gre:
            /* setup template, ip tunnel size and IP.protocol values */

            /* ERSPAN case: indicate that the IP.size should be ERSPAN_TYPEIIoGRE8oIP */
            if (tunnel_subtype == _bcm_tunnel_subtype_l2gre_is_erspan) {
                ip_tunnel_size_protocol_template->ip_tunnel_template = SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_ENHANCE_GRE; /* template value match encapsulation mode value in Arad/Jericho */
                ip_tunnel_size_protocol_template->encapsulation_size = (SOC_SAND_PP_IP_NO_OPTION_NOF_BITS + SOC_SAND_PP_GRE8_NOF_BITS + SOC_SAND_PP_ERSPAN_TYPEII_NOF_BITS) / SOC_SAND_NOF_BITS_IN_BYTE;
                ip_tunnel_size_protocol_template->protocol = SOC_SAND_PP_IP_PROTOCOL_GRE;
                ip_tunnel_size_protocol_template->protocol_enable = TRUE;
            }
            /* GRE8oETH */
            else {
                ip_tunnel_size_protocol_template->ip_tunnel_template = SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_ENHANCE_GRE; /* template value match encapsulation mode value in Arad/Jericho */
                ip_tunnel_size_protocol_template->encapsulation_size = (SOC_SAND_PP_IP_NO_OPTION_NOF_BITS + SOC_SAND_PP_GRE8_NOF_BITS) / SOC_SAND_NOF_BITS_IN_BYTE;
                ip_tunnel_size_protocol_template->protocol = SOC_SAND_PP_IP_PROTOCOL_GRE;
                ip_tunnel_size_protocol_template->protocol_enable = TRUE;
            }
            break;
        case bcmTunnelTypeUdp:
            /* UDP */
            ip_tunnel_size_protocol_template->ip_tunnel_template = SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_VXLAN; /* udp uses vxlan encap. mode */
            ip_tunnel_size_protocol_template->encapsulation_size = (SOC_SAND_PP_IP_NO_OPTION_NOF_BITS + SOC_SAND_PP_UDP_NOF_BITS) / SOC_SAND_NOF_BITS_IN_BYTE; /* ip header encapsulates udp */
            ip_tunnel_size_protocol_template->protocol = SOC_SAND_PP_IP_PROTOCOL_UDP;
            ip_tunnel_size_protocol_template->protocol_enable = TRUE;
        break;
        /* Ether IP */
        case bcmTunnelTypeEtherIp4In4:
            /* setup template, ip tunnel size and IP.protocol values */
            ip_tunnel_size_protocol_template->ip_tunnel_template = SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_ETHER_IP; /* template value match encapsulation mode value in Arad/Jericho */
            ip_tunnel_size_protocol_template->encapsulation_size = (SOC_SAND_PP_IP_NO_OPTION_NOF_BITS + SOC_SAND_PP_GRE2_NOF_BITS) / SOC_SAND_NOF_BITS_IN_BYTE;
            ip_tunnel_size_protocol_template->protocol = SOC_SAND_PP_IP_PROTOCOL_GRE;
            ip_tunnel_size_protocol_template->protocol_enable = TRUE;
        break;
        /* these are not supported as for tunnel into IPv4, user should use IpAnyIn4 */
        case bcmTunnelTypeIp4In4:
        case bcmTunnelTypeIp6In4:
        /* these are not supported as for tunnel into IPv4, user should use GreAnyIn4 */
        case bcmTunnelTypeGre4In4:
        case bcmTunnelTypeGre6In4:
        /* these are not supported as for tunnel into IPv6, user should use IpAnyIn6*/
        case bcmTunnelTypeIp4In6:
        case bcmTunnelTypeIp6In6:
        /* these are not supported as for tunnel into IPv6, user should use GreAnyIn6 */
        case bcmTunnelTypeGre4In6:
        case bcmTunnelTypeGre6In6:
        default:
            return BCM_E_PARAM;
        }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_petra_tunnel_ipv4_type_to_ppd(
    int unit,
    bcm_tunnel_type_t tunnel_type,
    _bcm_tunnel_subtype_t tunnel_subtype,
    SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO *ipv4_encap_info,
    uint32 is_replace
 )
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    /* From jericho B0 / QAX, template represent the tunnel type. Template is mapped to encapsulation mode
     *    Encapsulation size and IP.protocol are also mapped from encapsulation mode.
     *    1) setup template, encapsulation size and IP.protocol according to tunnel type
     *    2) sw allocate encapsulation mode
     *    3) set encapsulation mode in tunnel info */
    if (SOC_IS_JERICHO_B0_AND_ABOVE(unit) && (SOC_JER_PP_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_ENABLE == 1)) {
        int mapping_is_new;
        int old_is_last;
        uint32 flags=0;
        int old_encapsulation_mode;
        SOC_PPC_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_INFO ip_tunnel_size_protocol_template;

        /* setup template, encapsulation size and IP.protocol according to tunnel type */
        rv = _bcm_petra_tunnel_ipv4_type_to_ppd_template(unit, tunnel_type, tunnel_subtype, &ip_tunnel_size_protocol_template);
        BCMDNX_IF_ERR_EXIT(rv);

        if (!is_replace) {  /* allocate new template */
           flags = SHR_TEMPLATE_MANAGE_IGNORE_NOT_EXIST_OLD_TEMPLATE;
           old_encapsulation_mode = _DPP_AM_TEMPLATE_IP_TUNNEL_ENCAPSULATION_MODE_DUMMY_TEMPLATE;
        } else {            /* replace old template */
            flags = 0;
            old_encapsulation_mode = ipv4_encap_info->dest.encapsulation_mode_index;
        }

        /* sw allocate encapsulation mode */
        rv = bcm_dpp_am_template_ip_tunnel_encapsulation_mode_exchange(
                unit,
                old_encapsulation_mode, /* current encapsulation mode value */
                &ip_tunnel_size_protocol_template, /* ip tunnel template */
                &old_is_last, /* indicate that the current encapsulation mode index is no longer used */
                &(ipv4_encap_info->dest.encapsulation_mode_index), /* indicate the new encapsulation mode index */
                &mapping_is_new, /* indicate that we've allocated a new encapsualtion mode index */
                flags);
        BCMDNX_IF_ERR_EXIT(rv);

        /* if old mapping was last, the encapsulation mode has been deallocated,
           we can clear its value */
        if (old_is_last && (old_encapsulation_mode != (ipv4_encap_info->dest.encapsulation_mode_index))) {
            SOC_PPC_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_INFO ip_tunnel_size_protocol_template_cleared;
            SOC_PPC_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_INFO_clear(&ip_tunnel_size_protocol_template_cleared);
            rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_eg_encap_ip_tunnel_size_protocol_template_set,
                                     (unit, old_encapsulation_mode, &ip_tunnel_size_protocol_template_cleared));
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* if new mapping, then add the mapping for the encapsulation mode */
        if (mapping_is_new) {
            rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_eg_encap_ip_tunnel_size_protocol_template_set, 
                                      (unit, ipv4_encap_info->dest.encapsulation_mode_index, &ip_tunnel_size_protocol_template)); 
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }
    /* Before Jericho B0 / QAX, ip tunnel type is encapsulation mode. */
    else {
        switch(tunnel_type) {
        /* simple IPv4 */
        case bcmTunnelTypeIpAnyIn4:          /* RFC 2003/2893: IPv6/4-in-IPv4 tunnel. */
        case bcmTunnelTypeIpAnyIn6:
            ipv4_encap_info->dest.encapsulation_mode = SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_NONE;
        break;
        /* IPv4 with GRE */
        case bcmTunnelTypeGreAnyIn4:
        case bcmTunnelTypeGreAnyIn6:
            if (tunnel_subtype == _bcm_tunnel_subtype_gre_with_lb) {
                /* GRE with LB key use GRE8 template, when bcmTunnelTypeGreAnyIn4 type and BCM_TUNNEL_INIT_GRE_KEY_USE_LB flag are set */
                ipv4_encap_info->dest.encapsulation_mode = SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_ENHANCE_GRE;
            } else {
                ipv4_encap_info->dest.encapsulation_mode = SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_BASIC_GRE;
            }
        break;
        /* In Arad, VxLan encapsulation mode is the same as L2GRE. */
        case bcmTunnelTypeVxlan: 
        #ifdef BCM_88660_A0         
        if (SOC_IS_ARADPLUS(unit)) {
            ipv4_encap_info->dest.encapsulation_mode = SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_VXLAN;
            break;
        }
        #endif
        case bcmTunnelTypeL2Gre:
            ipv4_encap_info->dest.encapsulation_mode = SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_ENHANCE_GRE;
        break;
        /* Ether IP */
        case bcmTunnelTypeEtherIp4In4:
            ipv4_encap_info->dest.encapsulation_mode = SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_ETHER_IP;
        break;
        /* these are not supported as for tunnel into IPv4, user should use IpAnyIn4 */
        case bcmTunnelTypeIp4In4:
        case bcmTunnelTypeIp6In4:
        /* these are not supported as for tunnel into IPv4, user should use GreAnyIn4 */
        case bcmTunnelTypeGre4In4:
        case bcmTunnelTypeGre6In4:
        /* these are not supported as for tunnel into IPv6, user should use IpAnyIn6*/
        case bcmTunnelTypeIp4In6:
        case bcmTunnelTypeIp6In6:
        /* these are not supported as for tunnel into IPv6, user should use GreAnyIn6 */
        case bcmTunnelTypeGre4In6:
        case bcmTunnelTypeGre6In6:            
        default:
            return BCM_E_PARAM;
        }
    }

    BCM_EXIT;

exit:
    BCMDNX_FUNC_RETURN;

}


int
_bcm_petra_tunnel_ipv4_type_from_ppd(
    int unit, 
    SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO *ipv4_encap_info,
    bcm_tunnel_type_t *tunnel_type, 
    _bcm_tunnel_subtype_t *tunnel_subtype
 )
{
    int rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS; 

    /* From jericho B0 / QAX, template represent the tunnel type. Template is mapped to encapsulation mode
     *    Encapsulation size and IP.protocol are also mapped from encapsulation mode.
     *    from encapsulation mode, get the template, encapsulation size, protocol and protocol enabled, 
     *    and tunnel type.  */

    *tunnel_subtype = _bcm_tunnel_subtype_none; 

    if (SOC_IS_JERICHO_B0_AND_ABOVE(unit) && (SOC_JER_PP_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_ENABLE == 1)) { 
        SOC_PPC_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_INFO ip_tunnel_size_protocol_template; 
        uint32 vxlan_encapsulation_size;
        SOC_PPC_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_INFO_clear(&ip_tunnel_size_protocol_template); 

        rv = bcm_dpp_am_template_ip_tunnel_encapsulation_mode_get(
                    unit, 
                    ipv4_encap_info->dest.encapsulation_mode_index, 
                    &ip_tunnel_size_protocol_template); 
        BCMDNX_IF_ERR_EXIT(rv);

        vxlan_encapsulation_size = (SOC_SAND_PP_IP_NO_OPTION_NOF_BITS + SOC_SAND_PP_UDP_NOF_BITS + SOC_SAND_PP_VXLAN_NOF_BITS)/SOC_SAND_NOF_BITS_IN_BYTE;
        if (SOC_DPP_CONFIG(unit)->pp.lawful_interception_enable) {
            vxlan_encapsulation_size -= 4;
        }

        /* simple IPv4 */
        if ((ip_tunnel_size_protocol_template.ip_tunnel_template == SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_NONE) 
            && (ip_tunnel_size_protocol_template.encapsulation_size == ((SOC_SAND_PP_IP_NO_OPTION_NOF_BITS + 0) / SOC_SAND_NOF_BITS_IN_BYTE))
            && (ip_tunnel_size_protocol_template.protocol == 0) 
            && (ip_tunnel_size_protocol_template.protocol_enable == FALSE)
            ) {
            *tunnel_type = bcmTunnelTypeIpAnyIn4;
        } 
        /* IPv4 with GRE */
        else if ((ip_tunnel_size_protocol_template.ip_tunnel_template == SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_BASIC_GRE)
                 && (ip_tunnel_size_protocol_template.encapsulation_size == ((SOC_SAND_PP_IP_NO_OPTION_NOF_BITS + SOC_SAND_PP_GRE4_NOF_BITS) / SOC_SAND_NOF_BITS_IN_BYTE))
                 && (ip_tunnel_size_protocol_template.protocol == SOC_SAND_PP_IP_PROTOCOL_GRE)
                 && (ip_tunnel_size_protocol_template.protocol_enable == TRUE)) {
            *tunnel_type = bcmTunnelTypeGreAnyIn4; 
        }
        /* VXLAN */
        else if ((ip_tunnel_size_protocol_template.ip_tunnel_template == SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_VXLAN)
                 && (ip_tunnel_size_protocol_template.encapsulation_size == vxlan_encapsulation_size)
                 && (ip_tunnel_size_protocol_template.protocol == SOC_SAND_PP_IP_PROTOCOL_UDP)
                 && (ip_tunnel_size_protocol_template.protocol_enable == TRUE)) {
            *tunnel_type = bcmTunnelTypeVxlan;
        }
        /* UDP */
        else if ((ip_tunnel_size_protocol_template.ip_tunnel_template == SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_VXLAN)
                 && (ip_tunnel_size_protocol_template.encapsulation_size == (SOC_SAND_PP_IP_NO_OPTION_NOF_BITS + SOC_SAND_PP_UDP_NOF_BITS) / SOC_SAND_NOF_BITS_IN_BYTE)
                 && (ip_tunnel_size_protocol_template.protocol == SOC_SAND_PP_IP_PROTOCOL_UDP)
                 && (ip_tunnel_size_protocol_template.protocol_enable == TRUE)) {
           *tunnel_type = bcmTunnelTypeUdp;
        }
        /* L2GRE */
        else if ((ip_tunnel_size_protocol_template.ip_tunnel_template == SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_ENHANCE_GRE)
                 && (ip_tunnel_size_protocol_template.encapsulation_size == ((SOC_SAND_PP_IP_NO_OPTION_NOF_BITS + SOC_SAND_PP_GRE8_NOF_BITS) / SOC_SAND_NOF_BITS_IN_BYTE))
                 && (ip_tunnel_size_protocol_template.protocol == SOC_SAND_PP_IP_PROTOCOL_GRE)
                 && (ip_tunnel_size_protocol_template.protocol_enable == TRUE)) {
            *tunnel_type = bcmTunnelTypeL2Gre; 
        }
        /* L2GRE for ERPSAN */
        else if ((ip_tunnel_size_protocol_template.ip_tunnel_template == SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_ENHANCE_GRE)
                 && (ip_tunnel_size_protocol_template.encapsulation_size == ((SOC_SAND_PP_IP_NO_OPTION_NOF_BITS + SOC_SAND_PP_GRE8_NOF_BITS + SOC_SAND_PP_ERSPAN_TYPEII_NOF_BITS) / SOC_SAND_NOF_BITS_IN_BYTE))
                 && (ip_tunnel_size_protocol_template.protocol == SOC_SAND_PP_IP_PROTOCOL_GRE)
                 && (ip_tunnel_size_protocol_template.protocol_enable == TRUE)) {
            *tunnel_type = bcmTunnelTypeL2Gre; 
            *tunnel_subtype = _bcm_tunnel_subtype_l2gre_is_erspan; 
        }

        /* Ether IP */
        else if ((ip_tunnel_size_protocol_template.ip_tunnel_template == SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_ETHER_IP)
                  && (ip_tunnel_size_protocol_template.encapsulation_size == ((SOC_SAND_PP_IP_NO_OPTION_NOF_BITS + SOC_SAND_PP_GRE2_NOF_BITS) / SOC_SAND_NOF_BITS_IN_BYTE))
                  && (ip_tunnel_size_protocol_template.protocol == SOC_SAND_PP_IP_PROTOCOL_GRE)
                  && (ip_tunnel_size_protocol_template.protocol_enable == TRUE)) {
            *tunnel_type = bcmTunnelTypeEtherIp4In4; 
        } else {
            return BCM_E_PARAM;
        }

    }
    /* Before Jericho B0 / QAX, ip tunnel type is encapsulation mode. */
    else {
        switch(ipv4_encap_info->dest.encapsulation_mode) {
        /* simple IPv4 */
        case SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_NONE: 
            *tunnel_type = bcmTunnelTypeIpAnyIn4;
        break;
        /* IPv4 with GRE */
        case SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_BASIC_GRE: 
            *tunnel_type = bcmTunnelTypeGreAnyIn4;
        break;
        /* enhanced only for l2gre */
        case SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_ENHANCE_GRE: 
            *tunnel_type = bcmTunnelTypeL2Gre;
        break;
        /* Ether IP */
        case SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_ETHER_IP: 
            *tunnel_type = bcmTunnelTypeEtherIp4In4;
        break;
        /* VxLAN */
        case SOC_PPC_EG_ENCAP_ENCAPSULATION_MODE_VXLAN:
            *tunnel_type = bcmTunnelTypeVxlan;
        break;
        default:
            return BCM_E_PARAM;
        }

    }

    BCM_EXIT;

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_petra_tunnel_ipv4_type_to_ipv6_type(
    int unit, 
    bcm_tunnel_type_t ipv4_tunnel_type, 
    _bcm_tunnel_subtype_t ipv4_tunnel_subtype,
    bcm_tunnel_type_t *ipv6_tunnel_type
 )
{
    BCMDNX_INIT_FUNC_DEFS; 
    *ipv6_tunnel_type = ipv4_tunnel_type;
    if (ipv4_tunnel_type == bcmTunnelTypeIpAnyIn4) {
        *ipv6_tunnel_type = bcmTunnelTypeIpAnyIn6;
    } else if (ipv4_tunnel_type == bcmTunnelTypeGreAnyIn4) {
        *ipv6_tunnel_type = bcmTunnelTypeGreAnyIn6;
    } else if (ipv4_tunnel_type == bcmTunnelTypeVxlan) {
        *ipv6_tunnel_type = bcmTunnelTypeVxlan6;
    }
    BCM_EXIT;

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * given interface and tunnel returns eep. 
 *  first try from tunnel-id if available
 *  otherwise from intf.
 */
int
_bcm_petra_tunnel_id_eep_get(
    int unit, 
    bcm_l3_intf_t *intf, 
    bcm_tunnel_initiator_t *tunnel,
    int *eep,
    int *is_tunneled)
{
    int eep1=0,eep2=0;
    int is_tunneled1=0,is_tunneled2=0;

    BCMDNX_INIT_FUNC_DEFS;

    /* init to zero */
    *is_tunneled = 0;
    *eep = 0;

    /* from tunnel gport */
    if (tunnel != NULL) {
        eep1 = BCM_GPORT_TUNNEL_ID_GET(tunnel->tunnel_id);
        if(eep1 != _SHR_GPORT_INVALID) {
            is_tunneled1 = 1;
        }
    }
    /* from tunnel interface */
    if (intf != NULL) {
        if(BCM_L3_ITF_TYPE_IS_LIF(intf->l3a_intf_id)) { /* try to get from itf */
            is_tunneled2 = 1;
            eep2 = BCM_L3_ITF_VAL_GET(intf->l3a_intf_id);
        }
        else if (intf->l3a_tunnel_idx != 0) {
            is_tunneled2 = 1;
            eep2 = intf->l3a_tunnel_idx;
        }
    }
    if(is_tunneled2 && is_tunneled1 && eep2 != eep1) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("tunnel-id given in interface and tunnel-gport mismatch")));
    }

    if(is_tunneled2) {
        *eep = eep2;
        *is_tunneled = 1;
    }
    else if(is_tunneled1) {
        *eep = eep1;
        *is_tunneled = 1;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * set intf id and tunnel.tunnel_id to include EEP
 */
int
_bcm_petra_tunnel_id_eep_set(
    int unit, 
    bcm_l3_intf_t *intf, 
    bcm_tunnel_initiator_t *tunnel,
    int eep)
{

    BCM_GPORT_TUNNEL_ID_SET(tunnel->tunnel_id,eep);
    BCM_L3_ITF_SET(intf->l3a_intf_id, BCM_L3_ITF_TYPE_LIF, eep);

    return BCM_E_NONE;
}


/*
 * return wether tunnel type is ipv6 
 */
int
_bcm_petra_tunnel_type_is_ipv6(int unit, bcm_tunnel_type_t tnl_type, uint32 *is_ipv6){

    BCMDNX_INIT_FUNC_DEFS;
    switch(tnl_type) {
        case bcmTunnelTypeIp4In6:
        case bcmTunnelTypeIp6In6:
        case bcmTunnelTypeIpAnyIn6:
        case bcmTunnelTypeGre4In6:
        case bcmTunnelTypeGre6In6:
        case bcmTunnelTypeGreAnyIn6:
        case bcmTunnelTypeVxlan6:
            *is_ipv6 = TRUE;
        break;
        default:
            *is_ipv6 = FALSE;
        break;
    }
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;

}

int
_bcm_petra_ip6_addr_is_valid(int unit, bcm_ip6_t addr, uint32 *is_valid)
{
    uint32 idx = 0;
    BCMDNX_INIT_FUNC_DEFS;

    *is_valid = FALSE;
    for (idx = 0; idx < 16; idx++) {
        if (addr[idx]) {
            *is_valid = TRUE;
           break;
        }
    }
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;

}

int _bcm_dpp_is_ipv6_tunnel_support(int unit, bcm_tunnel_initiator_t* tunnel)
{
    int rv = BCM_E_NONE;
    uint32 is_ipv6_tunnel;
    BCMDNX_INIT_FUNC_DEFS;
    
    rv = _bcm_petra_tunnel_type_is_ipv6(unit,tunnel->type, &is_ipv6_tunnel);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!soc_property_get(unit, spn_BCM886XX_IPV6_TUNNEL_ENABLE, 1) && is_ipv6_tunnel){
        BCMDNX_ERR_EXIT_MSG(BCM_E_DISABLED, (_BSL_BCM_MSG("IPv6 tunnel is disabled")));
    }
    exit:
    BCMDNX_FUNC_RETURN; 
}
 

int _bcm_dpp_is_soc_property_termination_support(int unit, bcm_tunnel_terminator_t* tunnel)
{
    int rv = BCM_E_NONE;
    uint32 is_ipv6_tunnel;
    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_petra_tunnel_type_is_ipv6 (unit,tunnel->type, &is_ipv6_tunnel);
    BCMDNX_IF_ERR_EXIT(rv);
    if (is_ipv6_tunnel) {
        if ((SOC_DPP_CONFIG(unit)->pp.ipv6_tunnel_term_bitmap_enable & (SOC_DPP_IP_TUNNEL_TERM_DB_DIP_SIP_VRF | SOC_DPP_IP_TUNNEL_TERM_DB_DEFAULT)) == 0){
            BCMDNX_ERR_EXIT_MSG(BCM_E_DISABLED, (_BSL_BCM_MSG("IPv6 tunnel is disabled")));
        } 
    }
    else if (tunnel->type == bcmTunnelTypeUdp){
        if (!SOC_DPP_CONFIG(unit)->pp.udp_tunnel_enable) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_DISABLED, (_BSL_BCM_MSG("UDP Tunnel termination is disabled")));
        }
    }
    else
    {
        if (soc_property_get(unit, spn_BCM886XX_IP4_TUNNEL_TERMINATION_MODE, 1)==0 && soc_property_get(unit, spn_BCM886XX_VXLAN_ENABLE, 1)==0 && soc_property_get(unit, spn_BCM886XX_L2GRE_ENABLE, 1)==0 && soc_property_get(unit, spn_BCM886XX_ETHER_IP_ENABLE, 1)==0) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_DISABLED, (_BSL_BCM_MSG("Tunnel termination is disabled")));
        }
    }
    exit:
    BCMDNX_FUNC_RETURN; 
}  


/*
* map bcm tunnel initiator to ppd 
*/
int
_bcm_petra_tunnel_init_ipv6_to_ppd(
    int unit, 
    bcm_tunnel_initiator_t *tunnel,
    SOC_PPC_EG_ENCAP_IPV6_ENCAP_INFO *ipv6_encap_info
 )
{
    int rv = BCM_E_NONE;
    uint32 is_ipv6_tunnel;

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_petra_tunnel_type_is_ipv6(unit,tunnel->type, &is_ipv6_tunnel);
    BCMDNX_IF_ERR_EXIT(rv);

    ipv6_encap_info->tunnel.flow_label = tunnel->flow_label;
    ipv6_encap_info->tunnel.hop_limit = tunnel->ttl;
    ipv6_encap_info->tunnel.traffic_class = tunnel->dscp;
    rv = _bcm_l3_bcm_ipv6_addr_to_sand_ipv6_addr(unit,tunnel->dip6,&ipv6_encap_info->tunnel.dest);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = _bcm_l3_bcm_ipv6_addr_to_sand_ipv6_addr(unit,tunnel->sip6,&ipv6_encap_info->tunnel.src);
    BCMDNX_IF_ERR_EXIT(rv);

    if(tunnel->type == bcmTunnelTypeIp4In6) {
        ipv6_encap_info->tunnel.next_header = SOC_SAND_PP_IP_PROTOCOL_IP_in_IP;
    }
    else if(tunnel->type == bcmTunnelTypeIp6In6) {
        ipv6_encap_info->tunnel.next_header = SOC_SAND_PP_IP_PROTOCOL_IPv6;
    }
    else if(tunnel->type == bcmTunnelTypeVxlan6) {
        ipv6_encap_info->tunnel.next_header = SOC_SAND_PP_IP_PROTOCOL_UDP;
    }
    else if (!is_ipv6_tunnel) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("unsupported tunnel type for IPv6 ")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
* map bcm tunnel initiator from ppd 
*/
int
_bcm_petra_tunnel_init_ipv6_from_ppd(
    int unit, 
    bcm_tunnel_initiator_t *tunnel,
    SOC_PPC_EG_ENCAP_IPV6_ENCAP_INFO *ipv6_encap_info
 )
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    tunnel->ttl = SOC_SAND_PP_TTL_VAL_GET(ipv6_encap_info->tunnel.hop_limit);
    if(SOC_SAND_PP_TTL_IS_UNIFORM_GET(ipv6_encap_info->tunnel.hop_limit)){
        tunnel->flags |= BCM_TUNNEL_TERM_USE_OUTER_TTL;
    }
    if(!SOC_SAND_PP_TOS_IS_UNIFORM_GET(ipv6_encap_info->tunnel.flow_label)){
        tunnel->dscp_sel = bcmTunnelDscpAssign;
        tunnel->dscp = SOC_SAND_PP_TOS_VAL_GET(ipv6_encap_info->tunnel.flow_label);
    }
    else{
        tunnel->dscp_sel = bcmTunnelDscpMap;
        tunnel->dscp = 0;
    }

    if (SOC_DPP_CONFIG(unit)->pp.lawful_interception_enable) {
        tunnel->flow_label = ipv6_encap_info->tunnel.flow_label;
        tunnel->dscp = ipv6_encap_info->tunnel.traffic_class;
    }

    rv = _bcm_l3_sand_ipv6_addr_to_bcm_ipv6_addr(unit,&ipv6_encap_info->tunnel.dest,&tunnel->dip6);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = _bcm_l3_sand_ipv6_addr_to_bcm_ipv6_addr(unit,&ipv6_encap_info->tunnel.src, &tunnel->sip6);
    BCMDNX_IF_ERR_EXIT(rv);

    if(ipv6_encap_info->tunnel.next_header == SOC_SAND_PP_IP_PROTOCOL_IP_in_IP) {
        tunnel->type = bcmTunnelTypeIp4In6;
    }
    else if(ipv6_encap_info->tunnel.next_header == SOC_SAND_PP_IP_PROTOCOL_IPv6) {
        tunnel->type = bcmTunnelTypeIp6In6;
    }

    if (SOC_DPP_CONFIG(unit)->pp.ipv6_tunnel_encap_mode == SOC_DPP_IP6_TUNNEL_ENCAP_MODE_LARGE) {
        tunnel->udp_dst_port = (ipv6_encap_info->tunnel.dest.address[1] & 0xFFFF);
    }
    
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * resolve the ppd tunnel type to use,
 */
int
_bcm_petra_tunnel_init_ipv4_from_ppd(
    int unit, 
    bcm_tunnel_initiator_t *tunnel,
    SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO *ipv4_encap_info
 )
{
    int index;
    uint32 sip = 0;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    SOC_SAND_PP_IPV4_TOS tos_encoded;
    SOC_SAND_PP_IP_TTL ttl_encoded;
    int rv = BCM_E_NONE;
    _bcm_tunnel_subtype_t tunnel_subtype; 

    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_dev_id = (unit);

    /* get info from tunnel entry */
    tunnel->dip = ipv4_encap_info->dest.dest;

    /* outRIF */
    tunnel->vlan = ipv4_encap_info->out_vsi;

    /* get info from profiles */
    /* SIP */
    index = ipv4_encap_info->dest.src_index;
    soc_sand_rv = soc_ppd_eg_encap_ipv4_tunnel_glbl_src_ip_get(soc_sand_dev_id, index, &sip);
    SOC_SAND_IF_ERR_EXIT(soc_sand_rv);
    tunnel->sip = sip;
    

    /* TTL */
    index = ipv4_encap_info->dest.ttl_index;
    soc_sand_rv = soc_ppd_eg_encap_ipv4_tunnel_glbl_ttl_get(soc_sand_dev_id, index, &ttl_encoded);
    SOC_SAND_IF_ERR_EXIT(soc_sand_rv);
    tunnel->ttl = SOC_SAND_PP_TTL_VAL_GET(ttl_encoded);
    if(SOC_SAND_PP_TTL_IS_UNIFORM_GET(ttl_encoded)){
        tunnel->flags |= BCM_TUNNEL_TERM_USE_OUTER_DSCP;
    }

    /* TOS */
    index = ipv4_encap_info->dest.tos_index;
    soc_sand_rv = soc_ppd_eg_encap_ipv4_tunnel_glbl_tos_get(soc_sand_dev_id, index, &tos_encoded);
    SOC_SAND_IF_ERR_EXIT(soc_sand_rv);
    if(!SOC_SAND_PP_TOS_IS_UNIFORM_GET(tos_encoded)){
        tunnel->dscp_sel = bcmTunnelDscpAssign;
        tunnel->dscp = SOC_SAND_PP_TOS_VAL_GET(tos_encoded);
    }
    else{
        tunnel->dscp_sel = bcmTunnelDscpMap;
        tunnel->dscp = 0;
    }

    /* DF-flag: parse tos-index into DF bit*/
    if(SOC_DPP_CONFIG(unit)->pp.ip_tunnel_defrag_set){
        if(_BCM_DPP_IPV4_TUNNEL_DF_FROM_TOS_GET(index) == 1){
            tunnel->flags |= BCM_TUNNEL_INIT_IPV4_SET_DF;
        }
    }

    rv = _bcm_petra_tunnel_ipv4_type_from_ppd(unit,ipv4_encap_info,&(tunnel->type), &tunnel_subtype);
    BCMDNX_IF_ERR_EXIT(rv);

    if (SOC_DPP_CONFIG(unit)->pp.ipv4_tunnel_encap_mode == SOC_DPP_IP4_TUNNEL_ENCAP_MODE_LARGE) {
        tunnel->udp_dst_port = (ipv4_encap_info->dest.dest & 0xFFFF);
    }

exit:
    BCMDNX_FUNC_RETURN;
}



/*
 * map bcm tunnel info to PPD
 */
int
_bcm_petra_tunnel_init_ipv4_to_ppd(
    int unit, 
    bcm_tunnel_initiator_t *tunnel,
    int eep,
    int eep_alloced,
    SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO *ipv4_encap_info
 )
{
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    SOC_PPC_EG_ENCAP_IP_TUNNEL_SRC_IP ip_tunnel_src_ip;
    SOC_SAND_PP_IPV4_TOS tos_encoded;
    SOC_SAND_PP_IP_TTL ttl_encoded;
    int rv = BCM_E_NONE;
    uint32 is_uniform;
    uint8 df_flag = 0;
    uint32 is_ipv6_tunnel;
    int sip_template, ttl_template, tos_template,is_allocated,df_updated=0;
    _bcm_tunnel_subtype_t tunnel_subtype;


    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_dev_id = (unit);

    /* according to type identify if this ipv6 tunnel */
    rv = _bcm_petra_tunnel_type_is_ipv6(unit,tunnel->type, &is_ipv6_tunnel);
    BCMDNX_IF_ERR_EXIT(rv);

    /* set VSI for last EEP */
    ipv4_encap_info->dest.dest = tunnel->dip;

    /* Don't need to set the VSI for IPv6 entry */
    if (!is_ipv6_tunnel) {
        /* outRIF */
        ipv4_encap_info->out_vsi = tunnel->vlan;
    }

    /* SRC IP SET */
    SOC_PPC_EG_ENCAP_IP_TUNNEL_SRC_IP_clear(&ip_tunnel_src_ip);
    if (is_ipv6_tunnel) {
        ip_tunnel_src_ip.v6 = 1;
        rv = _bcm_l3_bcm_ipv6_addr_to_sand_ipv6_addr(unit,tunnel->sip6,&ip_tunnel_src_ip.src_ip);
        BCMDNX_IF_ERR_EXIT(rv); 
    } else {
        ip_tunnel_src_ip.src_ip.address[0] = tunnel->sip;
    }    
    
    /* DSCP SET */
    /* uniform if not taken explictly from entry */
    is_uniform = tunnel->dscp_sel != bcmTunnelDscpAssign;
    SOC_SAND_PP_TOS_SET(tos_encoded,tunnel->dscp,is_uniform);

    /* TTL SET */
    /* uniform if taken from packet*/
    is_uniform = tunnel->flags & BCM_TUNNEL_TERM_USE_OUTER_TTL;
    SOC_SAND_PP_TTL_SET(ttl_encoded,tunnel->ttl,is_uniform);

    /* use template managment to allocate profiles for SIP, TTL and TOS */
    /* eep_alloced tells if this is new Tunnel or not, 1 for old */
    rv = _bcm_dpp_am_template_ip_tunnel_exchange(unit,eep,!eep_alloced,&ip_tunnel_src_ip,ttl_encoded,tos_encoded,
                                                  &sip_template, &ttl_template, &tos_template,&is_allocated);
    BCMDNX_IF_ERR_EXIT(rv);

    /* calc DF bit if enabled*/
    if(SOC_DPP_CONFIG(unit)->pp.ip_tunnel_defrag_set){
        df_flag = (tunnel->flags & BCM_TUNNEL_INIT_IPV4_SET_DF)?1:0;
        /* always write to HW as this template may move from defragement to fragement area */
        df_updated = 1;

        _BCM_DPP_IPV4_TUNNEL_DF_IN_TOS_SET(tos_template,df_flag);
    }

    /* update indexes */
    /* Don't need to the src_index for IPv6 entry */
    ipv4_encap_info->dest.src_index = sip_template;
    ipv4_encap_info->dest.ttl_index = ttl_template;
    ipv4_encap_info->dest.tos_index = tos_template;

    /* check if GRE_LB tunnel type flag is set */
    tunnel_subtype = (tunnel->flags & BCM_TUNNEL_INIT_GRE_KEY_USE_LB) ? _bcm_tunnel_subtype_gre_with_lb : _bcm_tunnel_subtype_none;

    /* update type */
    rv = _bcm_petra_tunnel_ipv4_type_to_ppd(unit,tunnel->type, tunnel_subtype, ipv4_encap_info, (tunnel->flags & BCM_TUNNEL_REPLACE));
    BCMDNX_IF_ERR_EXIT(rv);

    /* if new allocated update hardware, one indication for all */
    if(is_allocated) {
        /* SIP */
        soc_sand_rv = soc_ppd_eg_encap_ipv4_tunnel_glbl_src_ip_set(soc_sand_dev_id, sip_template, tunnel->sip);
        SOC_SAND_IF_ERR_RETURN(soc_sand_rv);
        /* TTL */
        soc_sand_rv = soc_ppd_eg_encap_ipv4_tunnel_glbl_ttl_set(soc_sand_dev_id, ttl_template, ttl_encoded);
        SOC_SAND_IF_ERR_RETURN(soc_sand_rv);
    }

    /* either new template, or due to fragement change */
    if (is_allocated || df_updated) {
        /* TOS */
        soc_sand_rv = soc_ppd_eg_encap_ipv4_tunnel_glbl_tos_set(soc_sand_dev_id, tos_template, tos_encoded);
        SOC_SAND_IF_ERR_RETURN(soc_sand_rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * internal function to build ERSPAN tunnel.
 */
int _bcm_petra_tunnel_initiator_erspan_rspan_set(
   int unit, 
   bcm_l3_intf_t *intf, 
   bcm_tunnel_initiator_t *tunnel,
   int global_outlif /* outlif to use for ERSPAN tunnel */
   )
{
    int rv = BCM_E_NONE;
    bcm_mirror_destination_t
        mirror_dest;
    int
        dummy_global_outlif;
    uint8
        dummy_outlif_valid;
    int 
        replace=0;
    _bcm_lif_type_e usage;
    int local_lif = 0;

    BCMDNX_INIT_FUNC_DEFS;

    bcm_mirror_destination_t_init(&mirror_dest);


    /* is this to update exist tunnel? */
    replace = (tunnel->flags & BCM_TUNNEL_REPLACE)?1:0;

    if (replace) {
        rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, global_outlif, &local_lif);
        BCMDNX_IF_ERR_EXIT(rv);

        BCMDNX_IF_ERR_EXIT( _bcm_dpp_local_lif_sw_resources_lif_usage_get(unit, -1, local_lif, NULL, &usage) );
        if (usage != _bcmDppLifTypeIpTunnel) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Can't replace OutLIF to IP Tunnel, because it's a different type (%d)"),usage));
        }
    }

    /* set ERSPAN tunnel in this place (if not zero)*/
    /* outlif calculated from tunnel->tunnel_id*/
    mirror_dest.encap_id = global_outlif;

    if (tunnel->type == bcmTunnelTypeErspan) {

        /* this mirror has to include GRE tunnel*/
        mirror_dest.flags = BCM_MIRROR_DEST_TUNNEL_IP_GRE;
        /* point to next tunnel in EEDB has to be IPv4 tunnel */
        mirror_dest.tunnel_id = tunnel->l3_intf_id;
        /* mirror has ERSPAN tunnel */
        mirror_dest.flags |= BCM_MIRROR_DEST_TUNNEL_WITH_SPAN_ID;
        mirror_dest.span_id = tunnel->span_id;
    } else {
        mirror_dest.tpid =tunnel->tpid;
        mirror_dest.pkt_prio=tunnel->pkt_pri;
        mirror_dest.vlan_id=tunnel->vlan;
        mirror_dest.vlan_id |= (tunnel->pkt_cfi & 0x1) << 12;
        mirror_dest.flags = BCM_MIRROR_DEST_TUNNEL_RSPAN;
        /*
        * aux_data
        *     0: typical RSPAN processing.
        *        data_entry={2'b0, 16'btpid, 3'bpcp, 1'b0, 12'bvid, 6'b0, 2'bdata_entry_lsb},
        *        Encap = {DA, SA, TPID, VLAN}
        *     1: SPAN with Timestamp-Tag in NTP format.
        *        data_entry={2'b01, 16'btpid, 24'bxx, 6'b0, 2'bdata_entry_lsb},
        *        Encap = {DA, SA, TPID, NTP_TIMESTAMP(48b)}
        *     2: SPAN with Timestamp-Tag in 1588 format.
        *        data_entry={2'b02, 16'btpid, 24'bxx, 6'b0, 2'bdata_entry_lsb},
        *        Encap = {DA, SA, TPID, 1588_TIMESTAMP(48b)}
        */
        mirror_dest.meta_data = tunnel->aux_data;
    }

    /* call mirror internal function to create tunnel */
    rv = bcm_petra_mirror_destination_tunnel_create(
            unit,
            &mirror_dest,
            0,/*not in use */
            &dummy_global_outlif,
            &dummy_outlif_valid,
            replace
          );
    BCMDNX_IF_ERR_EXIT(rv);

    /* set return id, intf to include used outlif */
    rv = _bcm_petra_tunnel_id_eep_set(unit,intf,tunnel,mirror_dest.encap_id);
    BCMDNX_IF_ERR_EXIT(rv);

    if (dummy_outlif_valid) {
        /* Add to SW DB */
        rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, dummy_global_outlif, &local_lif);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = _bcm_dpp_out_lif_tunnel_initiator_match_add(unit, tunnel, local_lif);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * l2 encap internal function used for allocatoion of l2 encap tunnel
 */
int
_bcm_petra_tunnel_l2_encap_data_set(
    int unit, 
    bcm_l3_intf_t *intf,
    bcm_tunnel_initiator_t *tunnel)
{

    int rv = BCM_E_NONE;
    int local_outlif=0;
    uint32 flags=0;
    int global_outlif=0;

    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(tunnel);

        /* for l2 encapsulation set using this function and done */
    if (tunnel->flags & BCM_TUNNEL_WITH_ID) {
        global_outlif = tunnel->tunnel_id;
        flags|=BCM_DPP_AM_FLAG_ALLOC_WITH_ID;
    }

    if (tunnel->flags & BCM_TUNNEL_REPLACE) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("BCM_TUNNEL_REPLACE is not supported for this tunnel type")));
    }

    if (SOC_IS_JERICHO(unit)) {
            bcm_dpp_am_local_out_lif_info_t local_out_lif_info;

            sal_memset(&local_out_lif_info, 0, sizeof(local_out_lif_info));

            /* Set the local lif info */
            local_out_lif_info.app_alloc_info.pool_id = dpp_am_res_eep_ip_tnl;
            local_out_lif_info.app_alloc_info.application_type = bcm_dpp_am_egress_encap_app_l2_encap;
            local_out_lif_info.counting_profile_id = BCM_DPP_AM_COUNTING_PROFILE_NONE;

            rv = _bcm_dpp_gport_alloc_global_and_local_lif(unit, flags, &global_outlif, NULL, &local_out_lif_info);
            BCM_IF_ERROR_RETURN(rv);

            /* Retrieve the allocated local lif */
            local_outlif = local_out_lif_info.base_lif_id;

        } else if (SOC_IS_ARADPLUS(unit)) {


            rv = bcm_dpp_am_l2_encap_eep_alloc(unit, flags , &global_outlif);
            if (rv != BCM_E_NONE) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("allocate bcmTunnelTypeL2EncapExternalCpu tunnel fail")));
            }

            local_outlif = global_outlif;

        }

    
    intf->l3a_intf_id = global_outlif;
    intf->l3a_tunnel_idx=global_outlif;
    rv = _bcm_dpp_out_lif_l2_encap_match_add(unit,local_outlif);
    BCMDNX_IF_ERR_EXIT(rv);


    BCM_GPORT_TUNNEL_ID_SET(tunnel->tunnel_id ,global_outlif);

exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_petra_tunnel_init_encapsulation_mode_exchange_test(
    int unit,
    bcm_tunnel_initiator_t *tunnel,
    int eep,
    int encapsulation_mode_index)
{
    int rv = BCM_E_NONE;
    _bcm_tunnel_subtype_t tunnel_subtype;
    SOC_PPC_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_INFO ip_tunnel_size_protocol_template;
    uint32 is_replace;
    uint32 flags;
    int old_encapsulation_mode;
    int old_is_last;
    int mapping_is_new;
    BCMDNX_INIT_FUNC_DEFS;

    /* check if GRE_LB tunnel type flag is set */
    tunnel_subtype = (tunnel->flags & BCM_TUNNEL_INIT_GRE_KEY_USE_LB) ? _bcm_tunnel_subtype_gre_with_lb : _bcm_tunnel_subtype_none;

    rv = _bcm_petra_tunnel_ipv4_type_to_ppd_template(unit, tunnel->type, tunnel_subtype, &ip_tunnel_size_protocol_template);
    BCMDNX_IF_ERR_EXIT(rv);

    is_replace = (tunnel->flags & BCM_TUNNEL_REPLACE);

    if (!is_replace) {
        /* allocate new template */
        flags = SHR_TEMPLATE_MANAGE_IGNORE_NOT_EXIST_OLD_TEMPLATE;
        old_encapsulation_mode = _DPP_AM_TEMPLATE_IP_TUNNEL_ENCAPSULATION_MODE_DUMMY_TEMPLATE;
    } else {
        /* replace existing template */
        flags = 0;
        old_encapsulation_mode = encapsulation_mode_index;
    }

    /* use template managment to test IP tunnel profile allocation */
    /* eep_alloced tells if this is new Tunnel or not, 1 for old */

    /* sw allocate encapsulation mode test */
    rv = bcm_dpp_am_template_ip_tunnel_encapsulation_mode_exchange_test(
            unit,
            old_encapsulation_mode, /* current encapsulation mode value */
            &ip_tunnel_size_protocol_template,
            &old_is_last, /* indicate that the current encapsulation mode index is no longer used */
            &encapsulation_mode_index, /* indicate the new encapsulation mode index */
            &mapping_is_new, /* indicate that we've allocated a new encapsualtion mode index */
            flags
            );
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/* check if sip, ttl and tos template allocations will suceed */
int
_bcm_petra_tunnel_init_sip_ttl_tos_exchange_test(
    int unit,
    bcm_tunnel_initiator_t *tunnel,
    int eep,
    int eep_alloced)
{
    int rv = BCM_E_NONE;
    uint32 is_ipv6_tunnel;
    SOC_PPC_EG_ENCAP_IP_TUNNEL_SRC_IP ip_tunnel_src_ip;
    SOC_SAND_PP_IPV4_TOS tos_encoded;
    SOC_SAND_PP_IP_TTL ttl_encoded;
    uint32 is_uniform;
    int sip_template, ttl_template, tos_template, is_allocated;

    BCMDNX_INIT_FUNC_DEFS;

    /* SRC IP SET */
    rv = _bcm_petra_tunnel_type_is_ipv6(unit,tunnel->type, &is_ipv6_tunnel);
    BCMDNX_IF_ERR_EXIT(rv);

    SOC_PPC_EG_ENCAP_IP_TUNNEL_SRC_IP_clear(&ip_tunnel_src_ip);
    if (is_ipv6_tunnel) {
        ip_tunnel_src_ip.v6 = 1;
        rv = _bcm_l3_bcm_ipv6_addr_to_sand_ipv6_addr(unit,tunnel->sip6,&ip_tunnel_src_ip.src_ip);
        BCMDNX_IF_ERR_EXIT(rv); 
    } else {
        ip_tunnel_src_ip.src_ip.address[0] = tunnel->sip;
    }        

    /* DSCP SET */
    /* uniform if not taken explictly from entry */
    is_uniform = tunnel->dscp_sel != bcmTunnelDscpAssign;
    SOC_SAND_PP_TOS_SET(tos_encoded,tunnel->dscp,is_uniform);

    /* TTL SET */
    /* uniform if taken from packet */
    is_uniform = tunnel->flags & BCM_TUNNEL_TERM_USE_OUTER_TTL;
    SOC_SAND_PP_TTL_SET(ttl_encoded,tunnel->ttl,is_uniform);

    /* use template managment to test SIP, TTL and TOS profiles allocation */
    /* eep_alloced tells if this is new Tunnel or not, 1 for old */
    rv = _bcm_dpp_am_template_ip_tunnel_exchange_test(unit, eep, !eep_alloced, &ip_tunnel_src_ip, ttl_encoded, tos_encoded,
                                                      &sip_template, &ttl_template, &tos_template, &is_allocated);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/* remove lif allocation of ERSPAN/RSPAN or bfd extended IPv4 tunnel */
int
_bcm_petra_tunnel_lif_destroy(int unit, int global_lif, int local_outlif)
{
    uint32 soc_sand_rv;
    uint32 dealloc_flags = 0;
    int rv = BCM_E_NONE;
    int soc_sand_dev_id;

    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

    soc_sand_dev_id = (unit);
    if (SOC_IS_JERICHO(unit)) {
        rv = _bcm_dpp_gport_delete_global_and_local_lif(unit, global_lif, _BCM_GPORT_ENCAP_ID_LIF_INVALID, local_outlif);
        BCMDNX_IF_ERR_EXIT(rv);
    } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        if (SOC_DPP_CONFIG(unit)->pp.bfd_extended_ipv4_src_ip) {
            rv = bcm_dpp_am_ip_tunnel_eep_dealloc(unit, dealloc_flags, local_outlif);
            if (rv != BCM_E_NONE) {
                BCMDNX_ERR_EXIT_MSG(rv,(_BSL_BCM_MSG("allocate IPv4/IPv6 tunnel fail")));
            }
        } else {
            rv = bcm_dpp_am_eg_data_erspan_dealloc(unit, dealloc_flags, global_lif);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    /* add enry to allocated place */
    soc_sand_rv = soc_ppd_eg_encap_entry_remove(soc_sand_dev_id, SOC_PPC_EG_ENCAP_EEP_TYPE_TUNNEL_EEP, local_outlif);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * globals
 */
int
bcm_petra_tunnel_initiator_data_set(
    int unit, 
    bcm_l3_intf_t *intf, 
    bcm_tunnel_initiator_t *tunnel,
    int vsi_param,
    int ll_eep_param,
    int *tunnel_eep)
{
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    int rv = BCM_E_NONE;
    SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO ipv4_encap_info;    
    SOC_PPC_EG_ENCAP_ENTRY_TYPE ll_entry_type; 
    int global_lif_id = 0;
    int is_tunneled = 0, local_out_lif=0, local_out_lif_2=0, flags = 0;
    int eep_alloced=0;
    SOC_PPC_EG_ENCAP_DATA_INFO
      data_info;
    SOC_PPC_EG_ENCAP_ENTRY_INFO   *encap_entry_info = NULL;
    uint32         next_eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX];
    uint32         nof_entries;
    uint32         ip_tunnel_alloc_flag=0;
    uint32 is_ipv6_tunnel;
    int update = 0, with_id = 0;
    _bcm_lif_type_e
        usage;
    int local_ll_outlif; 
    uint8 counting_profile_is_allocated;
    int new_eep_allocated = 0;  /* temporary variable, should be removed later. will remove:
                                  "clean-up in case of tunnel allocation failure" at exit */
    uint32 data_32;
    uint64 add_data;
    bcm_dpp_am_local_out_lif_info_t outlif_info;
    BCMDNX_INIT_FUNC_DEFS;


    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(intf);
    BCMDNX_NULL_CHECK(tunnel);

    SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO_clear(&ipv4_encap_info);
    sal_memset(&outlif_info, 0, sizeof(outlif_info));

    soc_sand_dev_id = (unit);

    /* according to type identify if this ipv6 tunnel */
    rv = _bcm_petra_tunnel_type_is_ipv6(unit,tunnel->type, &is_ipv6_tunnel);
    BCMDNX_IF_ERR_EXIT(rv);

    /* alloc IPv4 or IPv6 tunnel */
    ip_tunnel_alloc_flag = (is_ipv6_tunnel)?BCM_DPP_AM_FLAG_ALLOC_IPV6:0;

    /* If mode is remark mapping, dscp should be 0*/
    if (tunnel->dscp_sel != bcmTunnelDscpAssign && tunnel->dscp != 0)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("If dscp_sel is not bcmTunnelDscpAssign, dscp should be 0")));
    }

    /* verify parameters for IPv6 */
    if(is_ipv6_tunnel) {

        if(tunnel->flags & BCM_TUNNEL_TERM_USE_OUTER_DSCP) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("IPv6 tunnel initiator can be only Pipe")));
        }
        if((tunnel->flow_label != 0) && !SOC_DPP_CONFIG(unit)->pp.lawful_interception_enable) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Flow label has to be zero")));
        }
        
        if(tunnel->dscp_sel != bcmTunnelDscpAssign) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("IPv6 tunnel initiator can be only pipe ")));
        }

        if (((SOC_DPP_CONFIG(unit)->pp.ipv6_tunnel_encap_mode == SOC_DPP_IP6_TUNNEL_ENCAP_MODE_NORMAL) ||
            (SOC_DPP_CONFIG(unit)->pp.ipv6_tunnel_encap_mode == SOC_DPP_IP6_TUNNEL_ENCAP_MODE_LARGE))&& !DPP_IS_FLAG_SET(tunnel->flags, BCM_TUNNEL_INIT_WIDE)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("IPv6 tunnel initiator flags must set BCM_TUNNEL_INIT_WIDE when IPv6 tunnel encap mode is 2/3  ")));
        }
    }


    /* Read the state of the flags */
    update = (tunnel->flags & BCM_TUNNEL_REPLACE) ? TRUE : FALSE;
    with_id = (tunnel->flags & BCM_TUNNEL_WITH_ID) ? TRUE : FALSE;

    if (update && (!with_id)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_TUNNEL_REPLACE can't be used without BCM_TUNNEL_WITH_ID as well")));
    }

    /* for ARAD interface for tunnel can be only EEP
     * EEP can based by tunnel_id if not zero. 
     * else by interface if not zero. 
     * otherwise eep is allocate from tunnel banks. 
     * and tunnel placements is stored in both tunnel_id and intf_id 
     */
    if (with_id) {
        rv = _bcm_petra_tunnel_id_eep_get(unit,intf,tunnel,&global_lif_id,&is_tunneled);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* above is common for all tunnel types: calculated eep/outlif to use */

    /* for erpsan set using this funtion and done */
    if (tunnel->type == bcmTunnelTypeErspan || tunnel->type == bcmTunnelTypeRspan) {
        rv = _bcm_petra_tunnel_initiator_erspan_rspan_set(unit, intf, tunnel, global_lif_id);
        BCMDNX_IF_ERR_EXIT(rv);
        BCM_EXIT;
    }

    /* if tunneled reuse same eep */
    if (is_tunneled) {
        flags |= BCM_DPP_AM_FLAG_ALLOC_WITH_ID;
        if (SOC_IS_JERICHO(unit)) {
            rv = bcm_dpp_am_global_lif_is_alloced(unit, BCM_DPP_AM_FLAG_ALLOC_EGRESS, global_lif_id);
        } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            rv = bcm_dpp_am_ip_tunnel_eep_is_alloced(unit,ip_tunnel_alloc_flag,global_lif_id);
        }

        if (rv == BCM_E_PARAM) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("check IP tunnel allocation")));
        }
        eep_alloced = (rv == BCM_E_EXISTS)?1:0;
    } else {   /* if not tunneled alloc new eep */
        flags = 0;
        eep_alloced = 0;
    }

    if (update) {
        if (!eep_alloced) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("In case of BCM_TUNNEL_REPLACE, the tunnel_id must exist")));
        }

        rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, global_lif_id, &local_out_lif);
        BCMDNX_IF_ERR_EXIT(rv);


        /* Check that eep is an index of a Tunnel OutLIF (in SW DB) */
        BCMDNX_IF_ERR_EXIT( _bcm_dpp_local_lif_sw_resources_lif_usage_get(unit, -1, local_out_lif, NULL, &usage) );
        if ((usage != _bcmDppLifTypeIpTunnel) &&
            !(SOC_DPP_CONFIG(unit)->pp.lawful_interception_enable && (usage == _bcmDppLifTypeVxlan))) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Can't replace OutLIF to IP Tunnel, because it's a different type (%d)"),usage));
        }

    } else if (with_id && eep_alloced){
        BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("In case tunnel_id already exists, BCM_TUNNEL_REPLACE should be used")));
    }


    /* get local LL outlif from global LL outlif.
       the interface object l3_intf_id of type encap contains global LL outlif */
    rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, BCM_L3_ITF_VAL_GET(tunnel->l3_intf_id), &local_ll_outlif);
    BCMDNX_IF_ERR_EXIT(rv);
    
    rv = soc_ppd_eg_encap_entry_type_get(unit, local_ll_outlif, &ll_entry_type);
    BCM_IF_ERROR_RETURN(rv);

    if(!eep_alloced) {/* need to allocate IP tunnel */
        uint32 ip_tunnel_eep_alloc_type = _BCM_DPP_AM_IP_TUNNEL_EEP_TYPE_DEFAULT; /* type of eedb ip tunnel allocation */

        /* identify type of eedb ip tunnel allocation 
         * - for IP tunnel eedb entry in ROO application for Jericho/QMX devices. (For QAX, default application). 
           - for all other applications: default */
        if (SOC_IS_ARADPLUS(unit)) {
            if (((ll_entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_DATA && SOC_IS_ARADPLUS_A0(unit)) 
                 || ((ll_entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_ROO_LL_ENCAP) && SOC_IS_JERICHO_AND_BELOW(unit)))
                && SOC_IS_ROO_ENABLE(unit)) {
                ip_tunnel_eep_alloc_type = _BCM_DPP_AM_IP_TUNNEL_EEP_TYPE_ROO;
            } 
        }

        if (SOC_IS_JERICHO(unit)) {

            /* Set the local lif info */
            outlif_info.app_alloc_info.pool_id =  (!is_ipv6_tunnel || (SOC_DPP_CONFIG(unit)->pp.lawful_interception_enable && tunnel->type == bcmTunnelTypeIpAnyIn6)) 
                                                      ? dpp_am_res_eep_ip_tnl : dpp_am_res_ipv6_tunnel;
            if(SOC_DPP_L2TP_IS_ENABLE(unit) && tunnel->type == bcmTunnelTypeIpAnyIn4 &&
                    tunnel->udp_dst_port == ARAD_PP_L2TP_ENCAP_PORT && tunnel->udp_src_port == ARAD_PP_L2TP_ENCAP_PORT &&
                    tunnel->encap_access == bcmEncapAccessTunnel1) {
                outlif_info.app_alloc_info.application_type = bcm_dpp_am_egress_encap_app_ip_tunnel_l2tp;
            } else
            outlif_info.app_alloc_info.application_type = (ip_tunnel_eep_alloc_type == _BCM_DPP_AM_IP_TUNNEL_EEP_TYPE_ROO) ?
                                                           bcm_dpp_am_egress_encap_app_ip_tunnel_roo : 0;

            /* Use-LIF-counting-profile */
            rv = bcm_dpp_counter_lif_range_is_allocated(unit, &counting_profile_is_allocated);
            BCMDNX_IF_ERR_EXIT(rv);
            
            if (counting_profile_is_allocated) {
                if ((tunnel->outlif_counting_profile != BCM_STAT_LIF_COUNTING_PROFILE_NONE) && (tunnel->outlif_counting_profile >= SOC_TMC_CNT_LIF_COUNTING_NOF_PROFILES(unit))) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("OutLIF-counting-profile %d is out of range."), tunnel->outlif_counting_profile));
                }
                if ((tunnel->type == bcmTunnelTypeRspan || tunnel->type == bcmTunnelTypeErspan) && tunnel->outlif_counting_profile != BCM_DPP_AM_COUNTING_PROFILE_NONE ) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("OutLIF-counting-profile %d is not supported for (E)RSPAN tunnels."), tunnel->outlif_counting_profile));
                }
                BCMDNX_IF_ERR_EXIT(bcm_dpp_counter_lif_local_profile_get(unit, tunnel->outlif_counting_profile, &outlif_info.counting_profile_id));         
            } else {
                outlif_info.counting_profile_id = BCM_STAT_LIF_COUNTING_PROFILE_NONE;
            }
            if (DPP_IS_FLAG_SET(tunnel->flags, BCM_TUNNEL_INIT_WIDE)) {
                outlif_info.local_lif_flags = BCM_DPP_AM_OUT_LIF_FLAG_WIDE;
            }
            /* Allocate the global and local lif */
            rv = _bcm_dpp_gport_alloc_global_and_local_lif(unit, flags, &global_lif_id, NULL, &outlif_info);
            BCM_IF_ERROR_RETURN(rv);
            new_eep_allocated = 1;

            /* Retrieve the allocated local lif */
            local_out_lif = outlif_info.base_lif_id;

            if (is_ipv6_tunnel && SOC_DPP_CONFIG(unit)->pp.ipv6_tunnel_encap_mode == SOC_DPP_IP6_TUNNEL_ENCAP_MODE_NORMAL) {
                sal_memset(&outlif_info, 0, sizeof(outlif_info));
                outlif_info.app_alloc_info.pool_id =  dpp_am_res_eep_ip_tnl;
                outlif_info.app_alloc_info.application_type = 0;
                outlif_info.counting_profile_id = BCM_STAT_LIF_COUNTING_PROFILE_NONE;
                outlif_info.local_lif_flags = BCM_DPP_AM_OUT_LIF_FLAG_COUPLED;
                if (DPP_IS_FLAG_SET(tunnel->flags, BCM_TUNNEL_INIT_WIDE)) {
                    outlif_info.local_lif_flags |= BCM_DPP_AM_OUT_LIF_FLAG_WIDE;
                }
                /* Note: Just allocate the local lif for Global-LIF-ID is fully controlled by the user, so we don't allocate the global lif here */
                rv = _bcm_dpp_am_local_outlif_alloc(unit, 0, &outlif_info);
                BCM_IF_ERROR_RETURN(rv);

                new_eep_allocated = 1;

                /* Retrieve the allocated local lif */
                local_out_lif_2 = outlif_info.base_lif_id;
            }
                

        } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {

            rv = bcm_dpp_am_ip_tunnel_eep_alloc(unit, ip_tunnel_eep_alloc_type, flags|ip_tunnel_alloc_flag, &global_lif_id);
            if (rv != BCM_E_NONE) {
                    BCMDNX_ERR_EXIT_MSG(rv,(_BSL_BCM_MSG("allocate IPv4/IPv6 tunnel fail")));
            }
            new_eep_allocated = 1;

            local_out_lif = global_lif_id;
        }
    }

    BCMDNX_ALLOC(encap_entry_info, sizeof(*encap_entry_info) * SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX,
        "bcm_petra_tunnel_initiator_data_set.encap_entry_info");
    if(encap_entry_info == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failue\n")));
    }
    if(eep_alloced) {
        soc_sand_rv =
            soc_ppd_eg_encap_entry_get(soc_sand_dev_id,
                                   SOC_PPC_EG_ENCAP_EEP_TYPE_TUNNEL_EEP,
                                   local_out_lif, 1,
                                   encap_entry_info, next_eep,
                                   &nof_entries);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        
        if ((encap_entry_info[0].entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV6_ENCAP) && 
            (encap_entry_info[1].entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV4_ENCAP)) {
            local_out_lif_2 = next_eep[0];
            local_ll_outlif = next_eep[1];
        }
    }
    else{
        next_eep[0] = SOC_PPC_EG_ENCAP_NEXT_EEP_INVALID;
    }

    /* set tunnel info into PPD */

    /* IP tunnel points to link layer. l3_intf_id is an outRif or an encap. */
    if (is_ipv6_tunnel && SOC_DPP_CONFIG(unit)->pp.ipv6_tunnel_encap_mode == SOC_DPP_IP6_TUNNEL_ENCAP_MODE_NORMAL) {
        next_eep[0] = local_out_lif_2;
        next_eep[1] = local_ll_outlif; 
    } else {
        next_eep[0] = local_ll_outlif; 
    }

    /* according to TYPE fill tunnel data */
    if(!is_ipv6_tunnel) {

        /* Set the condition to create a data entry for BFD packets over IPv4 */
        if (SOC_DPP_CONFIG(unit)->pp.bfd_extended_ipv4_src_ip
            && (tunnel->type == bcmTunnelTypeIpAnyIn4)
            && (tunnel->sip != 0) && (tunnel->dip == 0))
        {
                SOC_PPC_EG_ENCAP_DATA_INFO_clear(&data_info);
            SOC_PPD_EG_ENCAP_DATA_BFD_IPV4_FORMAT_SET(unit, tunnel->sip, tunnel->aux_data, &data_info);
            soc_sand_rv = soc_ppd_eg_encap_data_lif_entry_add(soc_sand_dev_id, local_out_lif, &data_info, TRUE, next_eep[0]);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
        else
        {
            /* check if template management will be able to allocate SIP, TTL and TOS template profiles */
            rv = _bcm_petra_tunnel_init_sip_ttl_tos_exchange_test(unit, tunnel, local_out_lif, eep_alloced);
            BCMDNX_IF_ERR_EXIT(rv);
            if (SOC_IS_JERICHO_B0_AND_ABOVE(unit) && (SOC_JER_PP_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_ENABLE == 1)) {
                /* check if template management will be able to allocate IP Template Template profile
                supported on Jericho_B0 and above */
                rv = _bcm_petra_tunnel_init_encapsulation_mode_exchange_test(unit, tunnel, local_out_lif, encap_entry_info[0].entry_val.ipv4_encap_info.dest.encapsulation_mode_index);
                BCMDNX_IF_ERR_EXIT(rv);
            }

            /* update encapsulation mode index from HW */
            if (update 
                && SOC_IS_JERICHO_B0_AND_ABOVE(unit) 
                && (SOC_JER_PP_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_ENABLE == 1)) {        
                ipv4_encap_info.dest.encapsulation_mode_index =  
                    encap_entry_info[0].entry_val.ipv4_encap_info.dest.encapsulation_mode_index;
            }

            rv = _bcm_petra_tunnel_init_ipv4_to_ppd(unit,tunnel,local_out_lif,eep_alloced,&ipv4_encap_info);
            BCMDNX_IF_ERR_EXIT(rv);

            /* Can't replace oam_lif_set and drop bits */
            if (update) {
                ipv4_encap_info.oam_lif_set = encap_entry_info[0].entry_val.ipv4_encap_info.oam_lif_set;
                ipv4_encap_info.drop = encap_entry_info[0].entry_val.ipv4_encap_info.drop;
            }

            /* update outlif profile for VXLAN*/
            if (SOC_IS_JERICHO(unit) && (tunnel->type == bcmTunnelTypeVxlan) &&
                (ll_entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_LL_ENCAP)) {
                /* update orientation in outlif profile */
                if (soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1))  {
                    soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_set,
                                    (unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_ORIENTATION, 1,
                                    &(ipv4_encap_info.outlif_profile)));
                    BCMDNX_IF_ERR_EXIT(soc_sand_rv);
                }

                /* update is_l2_lif in outlif profile. Used in ROO application to build native LL in case a L2_LIF is in the encapsulation stack */
                if (SOC_IS_JERICHO_PLUS(unit)) {
                    soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_set,
                                      (unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_ROO_IS_L2_LIF, 1,
                                      &(ipv4_encap_info.outlif_profile)));
                    BCMDNX_IF_ERR_EXIT(soc_sand_rv);
                }
            }
            
            /* update outlif profile for IPv4 tunnel TOR*/
            if (SOC_DPP_CONFIG(unit)->pp.ipv4_tunnel_encap_mode == SOC_DPP_IP4_TUNNEL_ENCAP_MODE_LARGE) {
                soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_set,
                                (unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_IPV4_TUNNEL_TOR, 1,
                                &(ipv4_encap_info.outlif_profile)));
                BCMDNX_IF_ERR_EXIT(soc_sand_rv);
            }

            if(outlif_info.app_alloc_info.application_type == bcm_dpp_am_egress_encap_app_ip_tunnel_l2tp) {
                soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_set,
                                (unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_L2TP_ENCAP, 1,
                                &(ipv4_encap_info.outlif_profile)));
                BCMDNX_IF_ERR_EXIT(soc_sand_rv);
            }
            /* write to HW entry */
            soc_sand_rv =
            soc_ppd_eg_encap_ipv4_entry_add(soc_sand_dev_id, local_out_lif,
                            &ipv4_encap_info, next_eep[0]);
            SOC_SAND_IF_ERR_EXIT(soc_sand_rv);

            if (SOC_DPP_CONFIG(unit)->pp.ipv4_tunnel_encap_mode == SOC_DPP_IP4_TUNNEL_ENCAP_MODE_LARGE) {
                uint32 data_tmp = tunnel->udp_dst_port;
                /* write additional data for IPv4 entry */
                COMPILER_64_ZERO(add_data);
                data_32 = 0;      
                /* Additional_data[15:0]: UDP_dest_port */
                SHR_BITCOPY_RANGE(&data_32,0,&data_tmp,0,16);
                COMPILER_64_SET(add_data, 0, data_32);
                rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_lif_additional_data_set,(unit, local_out_lif, FALSE, add_data, FALSE)));
                BCMDNX_IF_ERR_EXIT(rv); 
            }
        }
    }
    else{/* IPv6 */
        SOC_PPC_EG_ENCAP_IPV6_ENCAP_INFO 
            ipv6_encap_info;

        SOC_PPC_EG_ENCAP_IPV6_ENCAP_INFO_clear(&ipv6_encap_info);

        rv = _bcm_petra_tunnel_init_ipv6_to_ppd(unit,tunnel,&ipv6_encap_info);
        BCMDNX_IF_ERR_EXIT(rv);

        /* write to Data entry*/
        soc_sand_rv = soc_ppd_eg_encap_ipv6_entry_add(soc_sand_dev_id, local_out_lif, 
                        &ipv6_encap_info, next_eep[0]);
        SOC_SAND_IF_ERR_RETURN(soc_sand_rv);

        /* write to IPv4 entry*/
        if (SOC_DPP_CONFIG(unit)->pp.ipv6_tunnel_encap_mode > SOC_DPP_IP6_TUNNEL_ENCAP_MODE_LEGACY) {
            int local_out_lif_tmp = local_out_lif;
            SOC_PPC_EG_ENCAP_ENTRY_INFO *encap_entry_info_tmp= &encap_entry_info[0];
            uint32 next_eep_tmp = next_eep[0];

            if (SOC_DPP_CONFIG(unit)->pp.ipv6_tunnel_encap_mode == SOC_DPP_IP6_TUNNEL_ENCAP_MODE_NORMAL) {
                local_out_lif_tmp = local_out_lif_2;
                encap_entry_info_tmp= &encap_entry_info[1];
                next_eep_tmp = next_eep[1];
            }

            /* check if template management will be able to allocate SIP, TTL and TOS template profiles */
            rv = _bcm_petra_tunnel_init_sip_ttl_tos_exchange_test(unit, tunnel, local_out_lif_tmp, eep_alloced);
            BCMDNX_IF_ERR_EXIT(rv);
            if (SOC_IS_JERICHO_B0_AND_ABOVE(unit) && (SOC_JER_PP_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_ENABLE == 1)) {
                /* check if template management will be able to allocate IP Template Template profile
                supported on Jericho_B0 and above */
                rv = _bcm_petra_tunnel_init_encapsulation_mode_exchange_test(unit, tunnel, local_out_lif_tmp, encap_entry_info_tmp->entry_val.ipv4_encap_info.dest.encapsulation_mode_index);
                BCMDNX_IF_ERR_EXIT(rv);
            }

            /* update encapsulation mode index from HW */
            if (update 
                && SOC_IS_JERICHO_B0_AND_ABOVE(unit) 
                && (SOC_JER_PP_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_ENABLE == 1)) {        
                ipv4_encap_info.dest.encapsulation_mode_index =  
                    encap_entry_info_tmp->entry_val.ipv4_encap_info.dest.encapsulation_mode_index;
            }
            tunnel->sip = ipv6_encap_info.tunnel.sip_index; /* set PRGE index for sip value */
            rv = _bcm_petra_tunnel_init_ipv4_to_ppd(unit,tunnel,local_out_lif_tmp,eep_alloced,&ipv4_encap_info);
            BCMDNX_IF_ERR_EXIT(rv);
            
            /* IPv6 EEDB_1 */
            ipv4_encap_info.dest.dest = ipv6_encap_info.tunnel.dest.address[3]; /* DIP[127:96] */

            /* update outlif profile for VXLAN*/
            if (SOC_IS_JERICHO(unit) && (tunnel->type == bcmTunnelTypeVxlan6) &&
                (ll_entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_LL_ENCAP)) {
                /* update orientation in outlif profile */
                if (soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1))  {
                    soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_set,
                                    (unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_ORIENTATION, 1,
                                    &(ipv4_encap_info.outlif_profile)));
                    BCMDNX_IF_ERR_EXIT(soc_sand_rv);
                }

                /* update is_l2_lif in outlif profile. Used in ROO application to build native LL in case a L2_LIF is in the encapsulation stack */
                if (SOC_IS_JERICHO_PLUS(unit)) {
                    soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_set,
                                      (unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_ROO_IS_L2_LIF, 1,
                                      &(ipv4_encap_info.outlif_profile)));
                    BCMDNX_IF_ERR_EXIT(soc_sand_rv);
                }
            }
            
            /* update outlif profile for IPv6 tunnel TOR*/
            if (SOC_DPP_CONFIG(unit)->pp.ipv6_tunnel_encap_mode == SOC_DPP_IP6_TUNNEL_ENCAP_MODE_LARGE) {
                soc_sand_rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_set,
                                (unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_IPV6_TUNNEL_TOR, 1,
                                &(ipv4_encap_info.outlif_profile)));
                BCMDNX_IF_ERR_EXIT(soc_sand_rv);
            }
            
            soc_sand_rv = soc_ppd_eg_encap_ipv4_entry_add(soc_sand_dev_id, local_out_lif_tmp, &ipv4_encap_info, next_eep_tmp);
            SOC_SAND_IF_ERR_EXIT(soc_sand_rv);

            /* write additional data for IPv4 entry */
            COMPILER_64_ZERO(add_data);
            data_32 = 0;      
            if (SOC_DPP_CONFIG(unit)->pp.ipv6_tunnel_encap_mode == SOC_DPP_IP6_TUNNEL_ENCAP_MODE_NORMAL) {
                SHR_BITCOPY_RANGE(&data_32,0,ipv6_encap_info.tunnel.dest.address,76,20);
            } else {
                uint32 data_tmp = tunnel->udp_dst_port;
                /* Additional_data[15:0]: UDP_dest_port, Additional_data[19:16]: IPv6-DC-ID[3:0] */
                SHR_BITCOPY_RANGE(&data_32,0,&data_tmp,0,16);
                SHR_BITCOPY_RANGE(&data_32,16,ipv6_encap_info.tunnel.dest.address,92,4);
            }
            COMPILER_64_SET(add_data, 0, data_32);
            rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_lif_additional_data_set,(unit, local_out_lif_tmp, FALSE, add_data, FALSE)));
            BCMDNX_IF_ERR_EXIT(rv);        
        }
    }
   
    /* update SW according to allocated tunnel ID */
    rv = _bcm_petra_tunnel_id_eep_set(unit,intf,tunnel,global_lif_id);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Add to SW DB */
    rv = _bcm_dpp_out_lif_tunnel_initiator_match_add(unit,tunnel,local_out_lif);
    BCMDNX_IF_ERR_EXIT(rv);

    *tunnel_eep = local_out_lif;

exit:
    /* clean-up in case of tunnel allocation failure */
    if (rv != BCM_E_NONE){
        /* deallocate global and local lifs, if they were created */
        if (new_eep_allocated) {
            if (SOC_IS_JERICHO(unit)) {
                int rv1;
                rv1 = _bcm_dpp_gport_delete_global_and_local_lif(unit, global_lif_id, _BCM_GPORT_ENCAP_ID_LIF_INVALID, local_out_lif);
                BCMDNX_IF_ERR_REPORT(rv1);

                if (local_out_lif_2 != 0) {
                    rv1 = _bcm_dpp_am_local_outlif_dealloc_only(unit, local_out_lif_2);
                    BCMDNX_IF_ERR_REPORT(rv1);
                }          
            } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                bcm_dpp_am_ip_tunnel_eep_dealloc(unit,ip_tunnel_alloc_flag,local_out_lif);
            }
        }
    }
    BCM_FREE(encap_entry_info);
    BCMDNX_FUNC_RETURN;
}



int
bcm_petra_tunnel_initiator_set(
    int unit, 
    bcm_l3_intf_t *intf, 
    bcm_tunnel_initiator_t *tunnel)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("API no longer supported. Please use bcm_petra_tunnel_initiator_create instead.")));

  exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_petra_tunnel_initiator_set(
    int unit, 
    bcm_l3_intf_t *intf, 
    bcm_tunnel_initiator_t *tunnel)
{
    int rv = BCM_E_NONE;
    int tunnel_eep;
    int support;
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(tunnel);
    BCMDNX_NULL_CHECK(intf);

    /* L2 tunneling  */
    if (tunnel->type == bcmTunnelTypeL2EncapExternalCpu){
        /* This API is supported for ARAD+ and above */
        if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL,(_BSL_BCM_MSG("L2 tunnel from this API is supported only for ARAD+ and above")));
        }

        rv = _bcm_petra_tunnel_l2_encap_data_set(unit,intf,tunnel);
        BCMDNX_IF_ERR_EXIT(rv);

        BCM_EXIT;
    }

    BCM_DPP_TUNNEL_INIT_CHECK;

    /* Validate ttl. */
    if (!BCM_TTL_VALID(tunnel->ttl)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Param: tunnel->ttl is not valid. Valid range is 0-255")));
    }

    /* Validate IP tunnel DSCP SEL. */
    if (tunnel->dscp_sel > 2 || tunnel->dscp_sel < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Param: tunnel->dscp_sel is not valid. Valid range is 0-2.")));
    }

    /* Validate IP tunnel DSCP value. */
    if (tunnel->dscp > 255 || tunnel->dscp < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Param: tunnel->dscp is not valid. Valid range is 0-255.")));
    }

    rv = _bcm_petra_tunnel_type_support_check(unit, tunnel->type, NULL, &support);
    if (rv != BCM_E_NONE) {
        BCMDNX_ERR_EXIT_MSG(rv,(_BSL_BCM_MSG("IP tunnel not supported")));
    }

    if (FALSE == support) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL,(_BSL_BCM_MSG("Tunnel type not supported.")));
    }

    /* Defragement is set, while not supported */
    if(tunnel->flags & BCM_TUNNEL_INIT_IPV4_SET_DF && !SOC_DPP_CONFIG(unit)->pp.ip_tunnel_defrag_set){
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL,(_BSL_BCM_MSG("Defragement set not supported.")));
    }


    /* ERSPAN from this API is supported for ARAD+ and above */
    if (tunnel->type == bcmTunnelTypeErspan && SOC_IS_ARAD_B1_AND_BELOW(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL,(_BSL_BCM_MSG("Erspan tunnel from this API is supported only for ARAD+ and above")));
    }

    /* GRE with LB key is supported on Jericho_A0 and above */
    if ((tunnel->type == bcmTunnelTypeGreAnyIn4) &&
        (tunnel->flags & BCM_TUNNEL_INIT_GRE_KEY_USE_LB) &&
        !(SOC_IS_JERICHO(unit))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL,(_BSL_BCM_MSG("GRE with LB key tunnel is supported for Jericho_A0 and above.")));
    }

    rv = _bcm_dpp_is_ipv6_tunnel_support(unit,tunnel);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = bcm_petra_tunnel_initiator_data_set(unit, 
                         intf, 
                         tunnel,
                         0,
                         0,
                         &tunnel_eep);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}




int 
bcm_petra_data_entry_set(
    int unit,
    bcm_tunnel_initiator_t *tunnel)
{
    int rv;
    SOC_PPC_EG_ENCAP_DATA_INFO data_info;
    int global_lif_flags = 0,global_lif_id = 0;
    int local_lif_id = 0;
    bcm_dpp_am_local_out_lif_info_t local_out_lif_info;
    int encap_with_id = 0,update = 0;
    uint32 data[SOC_PPC_EG_ENCAP_DATA_INFO_MAX_SIZE];
    uint32 soc_sand_rv;
    uint32 next_local_lif;
    uint32 profile_split_horizon_val;
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);


    encap_with_id = (tunnel->flags & BCM_TUNNEL_WITH_ID) ? TRUE : FALSE;
    update = (tunnel->flags & BCM_TUNNEL_REPLACE) ? TRUE : FALSE;

    if (encap_with_id) {
        global_lif_flags = BCM_DPP_AM_FLAG_ALLOC_WITH_ID;
        global_lif_id =  BCM_GPORT_TUNNEL_ID_GET(tunnel->tunnel_id);
    }

    if (update && !encap_with_id) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("If BCM_TUNNEL_REPLACE flag is set, then BCM_TUNNEL_WITH_ID flag must also be set")));        
    }

    if (update) {
        rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, global_lif_id, &local_lif_id);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        sal_memset(&local_out_lif_info, 0, sizeof(local_out_lif_info));

        if (tunnel->ttl == 0) {
            local_out_lif_info.app_alloc_info.pool_id =  dpp_am_res_lif_dynamic;
            local_out_lif_info.app_alloc_info.application_type = bcm_dpp_am_egress_encap_app_3_tags_data;
            local_out_lif_info.counting_profile_id = BCM_DPP_AM_COUNTING_PROFILE_NONE;
        } else if (tunnel->ttl == 1) {
            local_out_lif_info.app_alloc_info.pool_id =  dpp_am_res_eg_data_erspan;
            local_out_lif_info.app_alloc_info.application_type = bcm_dpp_am_egress_encap_nof_app_types;
            local_out_lif_info.counting_profile_id = BCM_DPP_AM_COUNTING_PROFILE_NONE;    
        } else if (tunnel->ttl == 2) {
            local_out_lif_info.app_alloc_info.pool_id =  dpp_am_res_eg_data_l2tp;
            local_out_lif_info.app_alloc_info.application_type = bcm_dpp_am_egress_encap_nof_app_types;
            local_out_lif_info.counting_profile_id = BCM_DPP_AM_COUNTING_PROFILE_NONE;     
        } 

        rv = _bcm_dpp_gport_alloc_global_and_local_lif(unit, global_lif_flags, &global_lif_id, NULL, &local_out_lif_info);
        BCM_IF_ERROR_RETURN(rv);

        local_lif_id = local_out_lif_info.base_lif_id;

    }

    sal_memset((uint8 *)data, 0, SOC_PPC_EG_ENCAP_DATA_INFO_MAX_SIZE * 4); 
    
    data[0] = tunnel->aux_data;
    data[1] = tunnel->flow_label;

    SOC_PPC_EG_ENCAP_DATA_INFO_clear(&data_info);
    
    sal_memcpy((uint8 *)data_info.data_entry, (uint8 *)data, SOC_PPC_EG_ENCAP_DATA_INFO_MAX_SIZE * 4);    

    /* Horizon split property*/
    profile_split_horizon_val = (tunnel->span_id & 3);
    if (soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1) == 3) {
        profile_split_horizon_val = (tunnel->span_id & 1);
    }
    rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_occ_mgmt_app_set,
        (unit, SOC_OCC_MGMT_TYPE_OUTLIF, SOC_OCC_MGMT_OUTLIF_APP_ORIENTATION,
         profile_split_horizon_val, &(data_info.outlif_profile)));
    BCMDNX_IF_ERR_EXIT(rv);

    /* add data-format lif entry */
    if ((tunnel->dip != 0) && (tunnel->ttl != 0)) {
        rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, tunnel->dip, (int*)&next_local_lif);
        BCMDNX_IF_ERR_EXIT(rv);
        soc_sand_rv = soc_ppd_eg_encap_data_lif_entry_add(unit, local_lif_id, &data_info, TRUE, next_local_lif);
    } else {
        soc_sand_rv = soc_ppd_eg_encap_data_lif_entry_add(unit, local_lif_id, &data_info, FALSE, FALSE);
    }
    
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (!encap_with_id) {
         BCM_GPORT_TUNNEL_ID_SET(tunnel->tunnel_id,global_lif_id);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_data_entry_clear(
    int unit,
    bcm_l3_intf_t *intf)
{
    int rv = 0;
    int global_lif = 0,local_out_lif = 0;
    uint32 soc_sand_rv,soc_sand_dev_id = unit;

   
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

    global_lif = BCM_GPORT_TUNNEL_ID_GET(intf->l3a_source_vp);

    rv = bcm_dpp_am_global_lif_is_alloced(unit, BCM_DPP_AM_FLAG_ALLOC_EGRESS, global_lif);
    if (rv == BCM_E_NOT_FOUND) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Data entry is not found")));
    }
    rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS,global_lif,&local_out_lif);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_dpp_gport_delete_global_and_local_lif(unit, global_lif, _BCM_GPORT_ENCAP_ID_LIF_INVALID, local_out_lif);
    BCMDNX_IF_ERR_EXIT(rv);

    soc_sand_rv = soc_ppd_eg_encap_entry_remove(soc_sand_dev_id,SOC_PPC_EG_ENCAP_EEP_TYPE_DATA,local_out_lif);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        
    /* Remove entry from SW DB */
    rv = _bcm_dpp_local_lif_sw_resources_delete(unit, -1, local_out_lif, _BCM_DPP_GPORT_SW_RESOURCES_EGRESS);
    BCMDNX_IF_ERR_EXIT(rv);


exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_tunnel_lawful_interception_set(
    int unit,
    bcm_l3_intf_t *intf, 
    bcm_tunnel_initiator_t *tunnel)
{
    int rv;
    SOC_PPC_EG_ENCAP_DATA_INFO data_info;
    int alloc_flags = 0;
    int global_lif_id = 0;
    int local_lif_id = 0;
    int next_local_lif;
    bcm_dpp_am_local_out_lif_info_t local_out_lif_info;
    _bcm_dpp_gport_sw_resources gport_sw_resources;
    int encap_with_id = 0;
    int update = 0;
    uint32 data[SOC_PPC_EG_ENCAP_DATA_INFO_MAX_SIZE];
    uint32 soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(tunnel);
    BCMDNX_NULL_CHECK(intf);

    if (!SOC_DPP_CONFIG(unit)->pp.lawful_interception_enable) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Lawful Interception is not enabled or not supported on this device")));        
    }

    encap_with_id = (tunnel->flags & BCM_TUNNEL_WITH_ID) ? TRUE : FALSE;
    update = (tunnel->flags & BCM_TUNNEL_REPLACE) ? TRUE : FALSE;

    if (encap_with_id) {
        alloc_flags = BCM_DPP_AM_FLAG_ALLOC_WITH_ID;
        global_lif_id =  BCM_GPORT_TUNNEL_ID_GET(tunnel->tunnel_id);
    }

    if (update && !encap_with_id) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("If BCM_TUNNEL_REPLACE flag is set, then BCM_TUNNEL_WITH_ID flag must also be set")));        
    }

    if (update) {
        rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, global_lif_id, &local_lif_id);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        if (SOC_IS_JERICHO(unit)) {
            sal_memset(&local_out_lif_info, 0, sizeof(local_out_lif_info));

            /* Lawful Interception shares resource pool with ERSPAN */
            local_out_lif_info.app_alloc_info.pool_id =  dpp_am_res_eg_data_erspan;
            local_out_lif_info.app_alloc_info.application_type = bcm_dpp_am_egress_encap_nof_app_types;
            local_out_lif_info.counting_profile_id = BCM_DPP_AM_COUNTING_PROFILE_NONE;    

            rv = _bcm_dpp_gport_alloc_global_and_local_lif(unit, alloc_flags, &global_lif_id, NULL, &local_out_lif_info);
            BCM_IF_ERROR_RETURN(rv);

            local_lif_id = local_out_lif_info.base_lif_id;
        } else /* ARAD+ */ {
        
            local_lif_id = global_lif_id; /* If the WITH_ID flag is set, the lif value will be in the global lif */
            rv = bcm_dpp_am_eg_data_erspan_alloc(unit, alloc_flags, (SOC_PPC_LIF_ID*)&local_lif_id);
            BCMDNX_IF_ERR_EXIT(rv);
        
            global_lif_id = local_lif_id; /* If the WITH_ID flag was not set, the local_outlif will now hold the newly allocated lif */
        }

    }

    sal_memset((uint8 *)data, 0, SOC_PPC_EG_ENCAP_DATA_INFO_MAX_SIZE * 4); 

    /* Data entry format: { udp_dst_port[15:0], content_id[31:0], 4'b0010 }, LSB[1:0] == ARAD_PRGE_DATA_ENTRY_LSBS_LI */
    data[0] = tunnel->aux_data << 4;
    data[0] |= 2;
    data[1] = (tunnel->udp_dst_port & 0xFFFF) << 4;
    data[1] |= tunnel->aux_data >> 28;

    SOC_PPC_EG_ENCAP_DATA_INFO_clear(&data_info);
    
    sal_memcpy((uint8 *)data_info.data_entry, (uint8 *)data, SOC_PPC_EG_ENCAP_DATA_INFO_MAX_SIZE * 4);    

    /* get local LL outlif from global LL outlif.
       the interface object l3_intf_id of type encap contains global LL outlif */
    rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, BCM_L3_ITF_VAL_GET(tunnel->l3_intf_id), &next_local_lif);
    BCMDNX_IF_ERR_EXIT(rv);

    soc_sand_rv = soc_ppd_eg_encap_data_lif_entry_add(unit, local_lif_id, &data_info, TRUE, next_local_lif);
    
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
   
    /* update SW according to allocated tunnel ID */
    rv = _bcm_petra_tunnel_id_eep_set(unit,intf,tunnel,global_lif_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_dpp_local_lif_to_sw_resources(unit, -1, local_lif_id, _BCM_DPP_GPORT_SW_RESOURCES_EGRESS, &gport_sw_resources);
    BCMDNX_IF_ERR_EXIT(rv);

    gport_sw_resources.out_lif_sw_resources.lif_type = _bcmDppLifTypeLawfulInterception;

    rv = _bcm_dpp_local_lif_sw_resources_set(unit, -1, local_lif_id, _BCM_DPP_GPORT_SW_RESOURCES_EGRESS, &gport_sw_resources);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_tunnel_lawful_interception_clear(
    int unit,
    bcm_l3_intf_t *intf)
{
    int rv = 0;
    int global_lif = 0,local_out_lif = 0;
    uint32 soc_sand_rv,soc_sand_dev_id = unit;
   
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(intf);

    global_lif = BCM_L3_ITF_VAL_GET(intf->l3a_intf_id);

    if (SOC_IS_JERICHO(unit)) {
        rv = bcm_dpp_am_global_lif_is_alloced(unit, BCM_DPP_AM_FLAG_ALLOC_EGRESS, global_lif);
    } else {
        rv = bcm_dpp_am_eg_data_erspan_is_alloced(unit, global_lif);
    }

    if (rv == BCM_E_NOT_FOUND) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Lawful Interception tunnel is not found")));
    }
    
    rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS,global_lif,&local_out_lif);
    BCMDNX_IF_ERR_EXIT(rv);

    soc_sand_rv = soc_ppd_eg_encap_entry_remove(soc_sand_dev_id,SOC_PPC_EG_ENCAP_EEP_TYPE_DATA,local_out_lif);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        
    /* Remove entry from SW DB */
    rv = _bcm_dpp_local_lif_sw_resources_delete(unit, -1, local_out_lif, _BCM_DPP_GPORT_SW_RESOURCES_EGRESS);
    BCMDNX_IF_ERR_EXIT(rv);

    if (SOC_IS_JERICHO(unit)) {
        rv = _bcm_dpp_gport_delete_global_and_local_lif(unit, global_lif, _BCM_GPORT_ENCAP_ID_LIF_INVALID, local_out_lif);
    } else {
        rv = bcm_dpp_am_eg_data_erspan_dealloc(unit, 0, global_lif);
    }

    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_tunnel_lawful_interception_get(
    int unit,
    bcm_l3_intf_t *intf, 
    bcm_tunnel_initiator_t *tunnel)
{
    int rv = BCM_E_NONE;
    int global_lif, local_out_lif, is_tunneled;
    SOC_PPC_EG_ENCAP_ENTRY_INFO encap_entry_info[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX];
    uint32 next_eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX], nof_entries;
    uint32 soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(tunnel);
    BCMDNX_NULL_CHECK(intf);

    rv = _bcm_petra_tunnel_id_eep_get(unit,intf,tunnel,&global_lif,&is_tunneled);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, global_lif, &local_out_lif);
    BCMDNX_IF_ERR_EXIT(rv);

    if (SOC_IS_JERICHO(unit)) {
        rv = bcm_dpp_am_global_lif_is_alloced(unit, BCM_DPP_AM_FLAG_ALLOC_EGRESS, global_lif);
    } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        rv = bcm_dpp_am_eg_data_erspan_is_alloced(unit,global_lif);
    }
    
    if (rv == BCM_E_NOT_FOUND) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("tunnel is not allocated ")));
    }
    
    soc_sand_rv =
        soc_ppd_eg_encap_entry_get(unit,
                               SOC_PPC_EG_ENCAP_EEP_TYPE_TUNNEL_EEP, local_out_lif, 1,
                               encap_entry_info, next_eep, &nof_entries);
    SOC_SAND_IF_ERR_RETURN(soc_sand_rv);

    tunnel->type = bcmTunnelTypeLawfulInterception;
    rv = _bcm_petra_tunnel_id_eep_set(unit, intf, tunnel, global_lif);
    BCMDNX_IF_ERR_EXIT(rv); 

    /* Data entry format: { udp_dst_port[15:0], content_id[31:0], 4'b0010 }, LSB[1:0] == ARAD_PRGE_DATA_ENTRY_LSBS_LI */
    tunnel->aux_data = (encap_entry_info[0].entry_val.data_info.data_entry[0] >> 4);
    tunnel->aux_data |= ((encap_entry_info[0].entry_val.data_info.data_entry[1] & 0xF) << 28);
    tunnel->udp_dst_port = (encap_entry_info[0].entry_val.data_info.data_entry[1] >> 4);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_tunnel_l2_src_encap_set(
    int unit,
    bcm_l3_intf_t *intf,
    bcm_tunnel_initiator_t *tunnel)
{
    int rv;
    SOC_PPC_EG_ENCAP_DATA_INFO data_info;
    int alloc_flags = 0;
    int global_lif_id = 0;
    int local_lif_id = 0;
    int next_local_lif;
    bcm_dpp_am_local_out_lif_info_t local_out_lif_info;
    _bcm_dpp_gport_sw_resources gport_sw_resources;
    int encap_with_id = 0;
    int update = 0;
    uint32 data[SOC_PPC_EG_ENCAP_DATA_INFO_MAX_SIZE];
    _bcm_lif_type_e lif_usage;
    uint32 soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(tunnel);
    BCMDNX_NULL_CHECK(intf);

    if (!SOC_IS_JERICHO(unit) || (!SOC_DPP_CONFIG(unit)->pp.l2_src_encap_enable)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("L2 Source Mac Encap is not enabled or not supported on this device")));
    }

    encap_with_id = (tunnel->flags & BCM_TUNNEL_WITH_ID) ? TRUE : FALSE;
    update = (tunnel->flags & BCM_TUNNEL_REPLACE) ? TRUE : FALSE;

    if (encap_with_id) {
        alloc_flags = BCM_DPP_AM_FLAG_ALLOC_WITH_ID;
        global_lif_id =  BCM_GPORT_TUNNEL_ID_GET(tunnel->tunnel_id);
    }

    if (update && !encap_with_id) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("If BCM_TUNNEL_REPLACE flag is set, then BCM_TUNNEL_WITH_ID flag must also be set")));
    }

    if (update) {
        rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, global_lif_id, &local_lif_id);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        sal_memset(&local_out_lif_info, 0, sizeof(local_out_lif_info));

        /* Custom SA shares resource pool with ERSPAN */
        local_out_lif_info.app_alloc_info.pool_id =  dpp_am_res_eg_data_erspan;
        local_out_lif_info.app_alloc_info.application_type = bcm_dpp_am_egress_encap_nof_app_types;
        local_out_lif_info.counting_profile_id = BCM_DPP_AM_COUNTING_PROFILE_NONE;

        rv = _bcm_dpp_gport_alloc_global_and_local_lif(unit, alloc_flags, &global_lif_id, NULL, &local_out_lif_info);
        BCM_IF_ERROR_RETURN(rv);

        local_lif_id = local_out_lif_info.base_lif_id;
    }

    sal_memset((uint8 *)data, 0, SOC_PPC_EG_ENCAP_DATA_INFO_MAX_SIZE * 4); 

    data[0] = (tunnel->smac[5] << 8) + (tunnel->smac[4] << 16) + (tunnel->smac[3] << 24) + SOC_PPC_PRGE_DATA_ENTRY_LSBS_L2_SRC_ENCAP;
    data[1] = (tunnel->smac[0] << 0) + (tunnel->smac[1] << 8) + (tunnel->smac[2] << 16);

    SOC_PPC_EG_ENCAP_DATA_INFO_clear(&data_info);

    sal_memcpy((uint8 *)data_info.data_entry, (uint8 *)data, SOC_PPC_EG_ENCAP_DATA_INFO_MAX_SIZE * 4);

    /* get local LL outlif from global LL outlif.
       the interface object l3_intf_id of type encap contains global LL outlif */
    if (tunnel->l3_intf_id != 0) {
        rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, BCM_L3_ITF_VAL_GET(tunnel->l3_intf_id), &next_local_lif);
        BCMDNX_IF_ERR_EXIT(rv);

        BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_sw_resources_lif_usage_get(unit, -1, next_local_lif, NULL, &lif_usage));
        if (lif_usage != _bcmDppLifTypeVlan) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Next EEP is not AC")));
        }

        soc_sand_rv = soc_ppd_eg_encap_data_lif_entry_add(unit, local_lif_id, &data_info, TRUE, next_local_lif);
    } else {
        soc_sand_rv = soc_ppd_eg_encap_data_lif_entry_add(unit, local_lif_id, &data_info, FALSE, FALSE);
    }

    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* update SW according to allocated tunnel ID */
    rv = _bcm_petra_tunnel_id_eep_set(unit,intf,tunnel,global_lif_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_dpp_local_lif_to_sw_resources(unit, -1, local_lif_id, _BCM_DPP_GPORT_SW_RESOURCES_EGRESS, &gport_sw_resources);
    BCMDNX_IF_ERR_EXIT(rv);

    gport_sw_resources.out_lif_sw_resources.lif_type = _bcmDppLifTypeL2SrcEncap;

    rv = _bcm_dpp_local_lif_sw_resources_set(unit, -1, local_lif_id, _BCM_DPP_GPORT_SW_RESOURCES_EGRESS, &gport_sw_resources);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_tunnel_l2_src_encap_clear(
    int unit,
    bcm_l3_intf_t *intf)
{
    int rv = 0;
    int global_lif = 0,local_out_lif = 0;
    uint32 soc_sand_rv,soc_sand_dev_id = unit;

    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(intf);

    global_lif = BCM_L3_ITF_VAL_GET(intf->l3a_intf_id);

    if (!SOC_IS_JERICHO(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("L2 src encap tunnel is not supported on this device.")));
    }

    rv = bcm_dpp_am_global_lif_is_alloced(unit, BCM_DPP_AM_FLAG_ALLOC_EGRESS, global_lif);
    if (rv == BCM_E_NOT_FOUND) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("L2 src encap tunnel is not found.")));
    }

    rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, global_lif,&local_out_lif);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_dpp_gport_delete_global_and_local_lif(unit, global_lif, _BCM_GPORT_ENCAP_ID_LIF_INVALID, local_out_lif);
    BCMDNX_IF_ERR_EXIT(rv);

    soc_sand_rv = soc_ppd_eg_encap_entry_remove(soc_sand_dev_id, SOC_PPC_EG_ENCAP_EEP_TYPE_DATA,local_out_lif);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* Remove entry from SW DB */
    rv = _bcm_dpp_local_lif_sw_resources_delete(unit, -1, local_out_lif, _BCM_DPP_GPORT_SW_RESOURCES_EGRESS);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_tunnel_l2_src_encap_get(
    int unit,
    bcm_l3_intf_t *intf,
    bcm_tunnel_initiator_t *tunnel)
{
    int rv = BCM_E_NONE;
    int global_lif, local_out_lif, is_tunneled;
    SOC_PPC_EG_ENCAP_ENTRY_INFO encap_entry_info[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX];
    uint32 next_eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX], nof_entries;
    uint32 soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(tunnel);
    BCMDNX_NULL_CHECK(intf);

    if (!SOC_IS_JERICHO(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("L2 src encap tunnel is not supported on this device.")));
    }

    rv = _bcm_petra_tunnel_id_eep_get(unit,intf,tunnel,&global_lif,&is_tunneled);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, global_lif, &local_out_lif);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = bcm_dpp_am_global_lif_is_alloced(unit, BCM_DPP_AM_FLAG_ALLOC_EGRESS, global_lif);
    if (rv == BCM_E_NOT_FOUND) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("tunnel is not allocated ")));
    }

    soc_sand_rv = soc_ppd_eg_encap_entry_get(unit,
                               SOC_PPC_EG_ENCAP_EEP_TYPE_TUNNEL_EEP, local_out_lif, 1,
                               encap_entry_info, next_eep, &nof_entries);
    SOC_SAND_IF_ERR_RETURN(soc_sand_rv);

    tunnel->type = bcmTunnelTypeL2SrcEncap;
    rv = _bcm_petra_tunnel_id_eep_set(unit, intf, tunnel, global_lif);
    BCMDNX_IF_ERR_EXIT(rv);

    tunnel->smac[5] = (encap_entry_info[0].entry_val.data_info.data_entry[0] >> 8) & 0xff;
    tunnel->smac[4] = (encap_entry_info[0].entry_val.data_info.data_entry[0] >> 16) & 0xff;
    tunnel->smac[3] = (encap_entry_info[0].entry_val.data_info.data_entry[0] >> 24) & 0xff;
    tunnel->smac[2] = (encap_entry_info[0].entry_val.data_info.data_entry[1] >> 16) & 0xff;
    tunnel->smac[1] = (encap_entry_info[0].entry_val.data_info.data_entry[1] >> 8) & 0xff;
    tunnel->smac[0] = encap_entry_info[0].entry_val.data_info.data_entry[1] & 0xff;

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_tunnel_initiator_create(
    int unit, 
    bcm_l3_intf_t *intf, 
    bcm_tunnel_initiator_t *tunnel)
{
    int rv;

    BCMDNX_INIT_FUNC_DEFS;

    if (conn_to_np_enable_read_soc == 0) {
        conn_to_np_enable = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "conn_to_np_enable", 0);
        conn_to_np_enable_read_soc = 1;
    }

    if ((conn_to_np_enable == 1) && (tunnel->type == bcmTunnelTypeCount)) {
        if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL,(_BSL_BCM_MSG("Virtual tunnel is supported only for Jericho and above")));
        }
		rv = bcm_petra_data_entry_set(unit, tunnel);		
    BCMDNX_IF_ERR_EXIT(rv);
        BCM_EXIT;
    } 
    
    if (tunnel->type == bcmTunnelTypeLawfulInterception) {
        rv = bcm_petra_tunnel_lawful_interception_set(unit, intf, tunnel);
        BCMDNX_IF_ERR_EXIT(rv);
        BCM_EXIT;
    }

    if (tunnel->type == bcmTunnelTypeL2SrcEncap) {
        rv = bcm_petra_tunnel_l2_src_encap_set(unit, intf, tunnel);
        BCMDNX_IF_ERR_EXIT(rv);
        BCM_EXIT;
    }

    rv = _bcm_petra_tunnel_initiator_set(unit, intf, tunnel);
    BCMDNX_IF_ERR_EXIT(rv);
    
  exit:
    BCMDNX_FUNC_RETURN;
}


int
bcm_petra_tunnel_initiator_clear(
    int unit, 
    bcm_l3_intf_t *intf)
{
    int global_lif_id, local_out_lif, local_out_lif_2=0, is_tunneled;
    SOC_PPC_EG_ENCAP_IP_TUNNEL_SRC_IP ip_tunnel_src_ip;
    SOC_PPC_EG_ENCAP_ENTRY_INFO encap_entry_info[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX];
    uint32 next_eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX], nof_entries;
    unsigned int soc_sand_dev_id;
    uint32 soc_sand_rv;
    int rv = BCM_E_NONE;
    uint32         ip_tunnel_alloc_flag=0;
    uint8 is_ipv6_tunnel=0;
    _bcm_lif_type_e lif_usage;
    int sip_template, ttl_template, tos_template,is_allocated;

    BCMDNX_INIT_FUNC_DEFS;
 
    BCM_DPP_UNIT_CHECK(unit);

    soc_sand_dev_id = (unit);

    if (conn_to_np_enable_read_soc == 0) {
        conn_to_np_enable = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "conn_to_np_enable", 0);
        conn_to_np_enable_read_soc = 1;
    }

    if ((conn_to_np_enable == 1) && (intf->l3a_intf_flags == BCM_L3_INGRESS_INTERFACE_INVALID)) {
	  if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
		BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL,(_BSL_BCM_MSG("Virtual tunnel is supported only for Jericho and above")));
	  }
	  rv = bcm_petra_data_entry_clear(unit, intf);	
          BCM_EXIT;
     } 


    /* get eep from interface */
    rv = _bcm_petra_tunnel_id_eep_get(unit,intf,0,&global_lif_id,&is_tunneled);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, global_lif_id, &local_out_lif);
    BCMDNX_IF_ERR_EXIT(rv);

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_sw_resources_lif_usage_get(unit, -1, local_out_lif, NULL, &lif_usage));

    if(lif_usage == _bcmDppLifTypeCpuL2Encap) {

        if (SOC_IS_JERICHO(unit)) {
            rv = bcm_dpp_am_global_lif_is_alloced(unit, BCM_DPP_AM_FLAG_ALLOC_EGRESS, global_lif_id);
            if (rv == BCM_E_NOT_FOUND) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("tunnel is not found on this interface")));
            }
            rv=_bcm_dpp_gport_delete_global_and_local_lif(unit, global_lif_id, _BCM_GPORT_ENCAP_ID_LIF_INVALID, local_out_lif);
            BCMDNX_IF_ERR_EXIT(rv);

        } else /*if (SOC_IS_ARADPLUS(unit))*/
        {
 
            /* check if this tunnel is allocated */
            rv = bcm_dpp_am_l2_encap_eep_is_alloced(unit,local_out_lif);
            if (rv == BCM_E_NOT_FOUND) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("tunnel is not found on this interface")));
            }
            rv = bcm_dpp_am_l2_encap_eep_dealloc(unit,local_out_lif);
                BCM_IF_ERROR_RETURN(rv);
        }        

        rv = _bcm_dpp_local_lif_sw_resources_delete(unit, -1, local_out_lif, _BCM_DPP_GPORT_SW_RESOURCES_EGRESS);
        BCMDNX_IF_ERR_EXIT(rv); 

        BCM_EXIT;
    }

    if (lif_usage == _bcmDppLifTypeLawfulInterception) {
        rv = bcm_petra_tunnel_lawful_interception_clear(unit, intf);

        BCMDNX_IF_ERR_EXIT(rv); 
        BCM_EXIT;
    }

    if (SOC_IS_JERICHO(unit) && (lif_usage == _bcmDppLifTypeL2SrcEncap)) {
        rv = bcm_petra_tunnel_l2_src_encap_clear(unit, intf);

        BCMDNX_IF_ERR_EXIT(rv);
        BCM_EXIT;
    }

    /* In ARAD_PLUS _bcmDppLifTypeAny mean not existing LIF */
    if ((lif_usage == _bcmDppLifTypeAny) && (SOC_IS_ARADPLUS_AND_BELOW(unit))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Tunnel is not found on this interface")));
    }

    if ((lif_usage != _bcmDppLifTypeIpTunnel) && (lif_usage != _bcmDppLifTypeVxlan)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("OutLIF is not a tunnel")));
    }

    /* if not then no tunnel, nothing to do */
    if (is_tunneled == 0) {
        return BCM_E_NOT_FOUND;
    }

    /* read entry from HW */
    soc_sand_rv = soc_ppd_eg_encap_entry_get(soc_sand_dev_id,
                     SOC_PPC_EG_ENCAP_EEP_TYPE_TUNNEL_EEP, local_out_lif, 1,
                     encap_entry_info, next_eep, &nof_entries);
    SOC_SAND_IF_ERR_RETURN(soc_sand_rv);

    if(encap_entry_info[0].entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV6_ENCAP) {
        if (SOC_DPP_CONFIG(unit)->pp.ipv6_tunnel_encap_mode == SOC_DPP_IP6_TUNNEL_ENCAP_MODE_NORMAL) {
            if (encap_entry_info[1].entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV4_ENCAP) {
                is_ipv6_tunnel = 1;
            }
        } else {
            is_ipv6_tunnel = 1;
        }
    }
    else if(encap_entry_info[0].entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV4_ENCAP) {
        is_ipv6_tunnel = 0;
    }
    /* ERSPAN / RSPAN*/
    else if (encap_entry_info[0].entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_DATA &&
             (SOC_PPD_EG_ENCAP_DATA_FORMAT_GET(soc_sand_dev_id,&(encap_entry_info->entry_val.data_info)) ||
              (SOC_DPP_CONFIG(unit)->pp.bfd_extended_ipv4_src_ip &&
               SOC_PPD_EG_ENCAP_DATA_BFD_IPV4_FORMAT_CHECK(soc_sand_dev_id,&(encap_entry_info->entry_val.data_info))))) {
        rv = _bcm_petra_tunnel_lif_destroy(unit, global_lif_id, local_out_lif);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Remove entry from SW DB */
        rv = _bcm_dpp_local_lif_sw_resources_delete(unit, -1, local_out_lif, _BCM_DPP_GPORT_SW_RESOURCES_EGRESS);
        BCMDNX_IF_ERR_EXIT(rv); 

        /* done */
        BCM_EXIT;
    }
    else{
        LOG_VERBOSE(BSL_LS_BCM_TUNNEL,
                    (BSL_META_U(unit,
                                "given interface doesn't include ip-tunnel intf %x\n"), intf->l3a_intf_id));
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("given interface doesn't include ip-tunnel")));
    }

    ip_tunnel_alloc_flag = (is_ipv6_tunnel)?BCM_DPP_AM_FLAG_ALLOC_IPV6:0;

   /* check if this tunnel is allocated */
    if (SOC_IS_JERICHO(unit)) {
        rv = bcm_dpp_am_global_lif_is_alloced(unit, BCM_DPP_AM_FLAG_ALLOC_EGRESS, global_lif_id);
    } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        rv = bcm_dpp_am_ip_tunnel_eep_is_alloced(unit,ip_tunnel_alloc_flag,global_lif_id);
    }
    if (rv == BCM_E_NOT_FOUND) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("tunnel is not found on this interface")));
    }

    /* free used profiles.
       ip tunnel profiles are used only for ipv4 tunnels for ipv6 tunnels when encap mode is normal*/
    if (!is_ipv6_tunnel || 
       (is_ipv6_tunnel && (SOC_DPP_CONFIG(unit)->pp.ipv6_tunnel_encap_mode == SOC_DPP_IP6_TUNNEL_ENCAP_MODE_NORMAL))) {
        int local_out_lif_tmp = local_out_lif;
        int old_encapsulation_mode_tmp = encap_entry_info[0].entry_val.ipv4_encap_info.dest.encapsulation_mode_index;

        if (is_ipv6_tunnel && (SOC_DPP_CONFIG(unit)->pp.ipv6_tunnel_encap_mode == SOC_DPP_IP6_TUNNEL_ENCAP_MODE_NORMAL)) {
            local_out_lif_2 = next_eep[0];
            local_out_lif_tmp = local_out_lif_2;
            old_encapsulation_mode_tmp = encap_entry_info[1].entry_val.ipv4_encap_info.dest.encapsulation_mode_index;
        }

        SOC_PPC_EG_ENCAP_IP_TUNNEL_SRC_IP_clear(&ip_tunnel_src_ip);
        rv = _bcm_dpp_am_template_ip_tunnel_exchange(unit,local_out_lif_tmp,0 /* exist entry */,&ip_tunnel_src_ip,0xFFFFFFFF,0xFFFFFFFF,
                                                      &sip_template, &ttl_template, &tos_template,&is_allocated);
        BCMDNX_IF_ERR_EXIT(rv);

        if (SOC_IS_JERICHO_B0_AND_ABOVE(unit) && (SOC_JER_PP_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_ENABLE == 1)) {
            int is_last_ip_tunnel_encapsulation_mode; 
            /* free encapsulation mode sw allocation */
            uint32 flags=SHR_TEMPLATE_MANAGE_SET_WITH_ID;
            int mapping_is_new;
            SOC_PPC_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_INFO data;
            int new_encap_mode = _DPP_AM_TEMPLATE_IP_TUNNEL_ENCAPSULATION_MODE_DUMMY_TEMPLATE;
            SOC_PPC_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_INFO_clear(&data);

            /* sw allocate encapsulation mode */
            rv = bcm_dpp_am_template_ip_tunnel_encapsulation_mode_exchange(
               unit,
               old_encapsulation_mode_tmp, /* current encapsulation mode value */
               &data,
               &is_last_ip_tunnel_encapsulation_mode, /* indicate that the current encapsulation mode index is no longer used */
               &new_encap_mode, /* indicate the new encapsulation mode index */
               &mapping_is_new, /* indicate that we've allocated a new encapsualtion mode index */
               flags
               );
            BCMDNX_IF_ERR_EXIT(rv);

            /* if last, clear mapping encapsulation mode and template,
               encapsulation size and IP.protocol */
            if (is_last_ip_tunnel_encapsulation_mode) {
                SOC_PPC_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_INFO ip_tunnel_size_protocol_template; 
                SOC_PPC_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_INFO_clear(&ip_tunnel_size_protocol_template); 

                rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_eg_encap_ip_tunnel_size_protocol_template_set, 
                                          (unit, old_encapsulation_mode_tmp, &ip_tunnel_size_protocol_template)); 
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
    }

    /* free used eep in case of IPv4 and IPv6 tunnels */
    if (SOC_IS_JERICHO(unit)) {
        rv = _bcm_dpp_gport_delete_global_and_local_lif(unit, global_lif_id, _BCM_GPORT_ENCAP_ID_LIF_INVALID, local_out_lif);
        BCMDNX_IF_ERR_EXIT(rv);
        if (is_ipv6_tunnel && (SOC_DPP_CONFIG(unit)->pp.ipv6_tunnel_encap_mode == SOC_DPP_IP6_TUNNEL_ENCAP_MODE_NORMAL)) {
            rv = _bcm_dpp_am_local_outlif_dealloc_only(unit, local_out_lif_2);
            BCMDNX_IF_ERR_EXIT(rv);
            /* Remove entry from SW DB */
            rv = _bcm_dpp_local_lif_sw_resources_delete(unit, -1, local_out_lif_2, _BCM_DPP_GPORT_SW_RESOURCES_EGRESS);
            BCMDNX_IF_ERR_EXIT(rv); 
        }            
    } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        rv = bcm_dpp_am_ip_tunnel_eep_dealloc(unit,ip_tunnel_alloc_flag,local_out_lif);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* free used EEP entry in hw */
        soc_sand_rv = soc_ppd_eg_encap_entry_remove(soc_sand_dev_id,SOC_PPC_EG_ENCAP_EEP_TYPE_TUNNEL_EEP,local_out_lif);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* Remove entry from SW DB */
    rv = _bcm_dpp_local_lif_sw_resources_delete(unit, -1, local_out_lif, _BCM_DPP_GPORT_SW_RESOURCES_EGRESS);
    BCMDNX_IF_ERR_EXIT(rv); 

exit:
    BCMDNX_FUNC_RETURN;
}


int 
bcm_petra_tunnel_initiator_get(
    int unit, 
    bcm_l3_intf_t *intf, 
    bcm_tunnel_initiator_t *tunnel)
{
    int rv = BCM_E_NONE;
    int global_lif, local_out_lif, is_tunneled;
    SOC_PPC_EG_ENCAP_ENTRY_INFO *encap_entry_info = NULL;
    SOC_PPC_EG_ENCAP_IPV4_TUNNEL_INFO ipv4_tunnel_info;
    uint32 next_eep[SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX], nof_entries;
    uint32 soc_sand_rv;
    uint32         ip_tunnel_alloc_flag=0;
    uint32 is_ipv6_tunnel;
    _bcm_lif_type_e lif_usage;
    _bcm_lif_type_e usage; 
    int global_next_eep;
    uint32 out_lif_flags;

    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(intf);
    BCMDNX_NULL_CHECK(tunnel);

    SOC_PPC_EG_ENCAP_IPV4_TUNNEL_INFO_clear(&ipv4_tunnel_info);

    rv = _bcm_petra_tunnel_type_is_ipv6(unit,tunnel->type, &is_ipv6_tunnel);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = _bcm_dpp_is_ipv6_tunnel_support(unit,tunnel);
    BCMDNX_IF_ERR_EXIT(rv);

    ip_tunnel_alloc_flag = (is_ipv6_tunnel)?BCM_DPP_AM_FLAG_ALLOC_IPV6:0;

    /* check if there is tunnel associated with intf at all */
    /* check if given interface is tunnel interface */
    rv = _bcm_petra_tunnel_id_eep_get(unit,intf,tunnel,&global_lif,&is_tunneled);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, global_lif, &local_out_lif);
    BCMDNX_IF_ERR_EXIT(rv);

    /* if not then no labels */
    if (is_tunneled == 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("tunnel is not found on this interface")));
    }
    
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_sw_resources_lif_usage_get(unit, -1, local_out_lif, NULL, &lif_usage));
    if(lif_usage == _bcmDppLifTypeCpuL2Encap) {

        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Tunnel _bcmDppLifTypeCpuL2Encap is not supportd by this API")));
    }
    /* In ARAD_PLUS _bcmDppLifTypeAny mean not existing LIF */
    if ((lif_usage == _bcmDppLifTypeAny) && (SOC_IS_ARADPLUS_AND_BELOW(unit))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Tunnel is not found on this interface")));
    }

    if ((lif_usage != _bcmDppLifTypeIpTunnel) && (lif_usage != _bcmDppLifTypeVxlan) &&
        !(SOC_DPP_CONFIG(unit)->pp.lawful_interception_enable && (lif_usage == _bcmDppLifTypeLawfulInterception)) &&
        !(SOC_DPP_CONFIG(unit)->pp.l2_src_encap_enable && (lif_usage == _bcmDppLifTypeL2SrcEncap))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("OutLIF is not a tunnel")));
    }

    if (SOC_DPP_CONFIG(unit)->pp.lawful_interception_enable && (lif_usage == _bcmDppLifTypeLawfulInterception)) {
        rv = bcm_petra_tunnel_lawful_interception_get(unit, intf, tunnel);
        BCMDNX_IF_ERR_EXIT(rv);
        BCM_EXIT;
    }

    if (SOC_DPP_CONFIG(unit)->pp.l2_src_encap_enable && (lif_usage == _bcmDppLifTypeL2SrcEncap)) {
        rv = bcm_petra_tunnel_l2_src_encap_get(unit, intf, tunnel);
        BCMDNX_IF_ERR_EXIT(rv);
        BCM_EXIT;
    }

    BCMDNX_ALLOC(encap_entry_info, sizeof(SOC_PPC_EG_ENCAP_ENTRY_INFO)*SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX,
        "bcm_petra_tunnel_initiator_get.encap_entry_info");
    if (encap_entry_info == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("unit %d, failed to allocate memory"), unit));
    }

    soc_sand_rv =
        soc_ppd_eg_encap_entry_get(unit,
                               SOC_PPC_EG_ENCAP_EEP_TYPE_TUNNEL_EEP, local_out_lif, 1,
                               encap_entry_info, next_eep, &nof_entries);
    SOC_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* point to next intf/tunnel */
        if (nof_entries > 1) {

            /* Check in SW DB that the outLIF is LL for overlay (build by l2_egress_create),
               If so, l3_intf_id is an encap */
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_local_lif_sw_resources_lif_usage_get(unit, -1,  next_eep[0], NULL, &usage));

            /* Convert the next eep to global lif before it can be exposed to the user. */
            rv = _bcm_dpp_global_lif_mapping_local_to_global_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, next_eep[0], &global_next_eep);
            BCMDNX_IF_ERR_EXIT(rv);

            if (usage == _bcmDppLifTypeOverlayLinkLayer) {
                BCM_L3_ITF_SET(tunnel->l3_intf_id, BCM_L3_ITF_TYPE_LIF, global_next_eep); 
            } 
            /* otherwise it's an outRif. */
            else {
                BCM_L3_ITF_SET(tunnel->l3_intf_id, BCM_L3_ITF_TYPE_RIF, global_next_eep);
            }
        }

        /* check outlif allocated for the configured usage */
        if (encap_entry_info[0].entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV4_ENCAP || encap_entry_info[0].entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV6_ENCAP) {

            /* check if this tunnel is allocated */
            if (SOC_IS_JERICHO(unit)) {
                rv = bcm_dpp_am_global_lif_is_alloced(unit, BCM_DPP_AM_FLAG_ALLOC_EGRESS, global_lif);
            } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                rv = bcm_dpp_am_ip_tunnel_eep_is_alloced(unit,ip_tunnel_alloc_flag,global_lif);
            }
            if (rv == BCM_E_NOT_FOUND) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("tunnel is not found on this interface")));
            }
        }


        if (encap_entry_info[0].entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV4_ENCAP) {

            /* set ipv4 tunnel info */
            rv = _bcm_petra_tunnel_init_ipv4_from_ppd(unit,tunnel,&encap_entry_info[0].entry_val.ipv4_encap_info);
            BCMDNX_IF_ERR_EXIT(rv);

            /* L2GRE and GRE with LB use same GRE encapsulation, check outlif usage to distinguish */
            if (tunnel->type == bcmTunnelTypeL2Gre) {
                rv = _bcm_dpp_local_outlif_sw_resources_flags_get(unit, local_out_lif, &out_lif_flags);
                BCMDNX_IF_ERR_EXIT(rv);
                if (out_lif_flags & _BCM_DPP_OUTLIF_IP_TUNNEL_MATCH_INFO_GRE_WITH_LB) {
                   tunnel->type = bcmTunnelTypeGreAnyIn4;
                }
            }

        }
        else if (encap_entry_info[0].entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV6_ENCAP) {
            /* set IPv6 tunnel info */
            rv = _bcm_petra_tunnel_init_ipv6_from_ppd(unit,tunnel,&encap_entry_info[0].entry_val.ipv6_encap_info);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        else if ((encap_entry_info[0].entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_DATA)
                 && (SOC_PPD_EG_ENCAP_DATA_FORMAT_GET(unit,&(encap_entry_info->entry_val.data_info))))
        {
            if (SOC_IS_JERICHO(unit)) {
                rv = bcm_dpp_am_global_lif_is_alloced(unit, BCM_DPP_AM_FLAG_ALLOC_EGRESS, global_lif);
            } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                rv = bcm_dpp_am_eg_data_erspan_is_alloced(unit,global_lif);
            }
            if (rv == BCM_E_NOT_FOUND) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("tunnel is not allocated ")));
            }

            if (SOC_PPC_EG_ENCAP_EEDB_IS_FORMAT_ERSPAN(&(encap_entry_info->entry_val.data_info))) {
                /* set ERSPAN tunnel info */
                tunnel->type = bcmTunnelTypeErspan;
                tunnel->span_id = SOC_PPC_EG_ENCAP_DATA_ERSPAN_FORMAT_SPAN_ID_GET(unit,&(encap_entry_info->entry_val.data_info));
            } else { /* RSPAN*/
                tunnel->type = bcmTunnelTypeRspan;
                SOC_PPD_EG_ENCAP_DATA_RSPAN_FORMAT_GET(tunnel->tpid, tunnel->pkt_pri , tunnel->vlan ,&(encap_entry_info->entry_val.data_info));
            }
            rv = _bcm_petra_tunnel_id_eep_set(unit, intf, tunnel, global_lif);
            BCMDNX_IF_ERR_EXIT(rv); 

        } 
        else if((encap_entry_info[0].entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_DATA)
                && (SOC_DPP_CONFIG(unit)->pp.bfd_extended_ipv4_src_ip)
                && (tunnel->type == bcmTunnelTypeIpAnyIn4))
        {
            int bfd_sip = 0;
            int aux_data = 0;
            int prg_val = 0;

            SOC_PPD_EG_ENCAP_DATA_BFD_IPV4_FORMAT_GET(unit, prg_val, bfd_sip, aux_data, &(encap_entry_info->entry_val.data_info));
            if (prg_val == SOC_PPD_EG_ENCAP_BFD_IPV4_PROG_VAR) {
                tunnel->sip = bfd_sip;
                tunnel->aux_data = aux_data;
            }
        }
        else{
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("expected to IP or ERSPAN tunnel entry ")));
        }

        /* set ID */
        rv = _bcm_petra_tunnel_id_eep_set(unit,intf,tunnel,global_lif);
        BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCM_FREE(encap_entry_info);
    BCMDNX_FUNC_RETURN;
}





int
bcm_petra_tunnel_term_key_to_ppd(
    int unit, 
    bcm_tunnel_terminator_t *info,
    SOC_PPC_RIF_IP_TERM_KEY    *term_key
 ) 
{
    int rv = BCM_E_NONE;
    uint32 is_ipv6_tunnel;
    uint32 port_property;

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_petra_tunnel_type_is_ipv6(unit,info->type, &is_ipv6_tunnel);
    BCMDNX_IF_ERR_EXIT(rv);
    if (is_ipv6_tunnel) {
        term_key->flags = SOC_PPC_RIF_IP_TERM_FLAG_IPV6;
        if (info->flags & BCM_TUNNEL_TERM_BUD) {
            term_key->flags |= SOC_PPC_RIF_IP_TERM_FLAG_BUD;
        }
        if (bcm_ip6_mask_length(info->dip6_mask) != 0) {
            term_key->dip6.prefix_len = bcm_ip6_mask_length(info->dip6_mask);
            rv = _bcm_l3_bcm_ipv6_addr_to_sand_ipv6_addr(unit, info->dip6, &(term_key->dip6.ipv6_address));
            BCMDNX_IF_ERR_EXIT(rv);
        }

        if (SOC_IS_JERICHO(unit)
            && (SOC_DPP_CONFIG(unit)->pp.ipv6_tunnel_term_bitmap_enable & SOC_DPP_IP_TUNNEL_TERM_DB_DIP_SIP_VRF)) {
            if (bcm_ip6_mask_length(info->sip6_mask) != 0) {
                term_key->sip6.prefix_len = bcm_ip6_mask_length(info->sip6_mask);
                rv = _bcm_l3_bcm_ipv6_addr_to_sand_ipv6_addr(unit, info->sip6, &(term_key->sip6.ipv6_address));
                BCMDNX_IF_ERR_EXIT(rv);
            }

            if (DPP_VRF_VALID(unit, info->vrf)) {
                /* VRF */
                term_key->vrf = info->vrf; 
                term_key->flags |= SOC_PPC_RIF_IP_TERM_FLAG_VRF_IS_VALID; 
            }

            if (info->flags & BCM_TUNNEL_TERM_USE_TUNNEL_CLASS) {
                /* Compressed-Destination */
                term_key->my_vtep_index = info->tunnel_class;
                term_key->flags |= SOC_PPC_RIF_IP_TERM_FLAG_MY_VTEP_INDEX_IS_VALID; 
                
            } else {
                /* key includes next protocol */
                switch (info->type) {
                case bcmTunnelTypeIp4In6:
                    term_key->ipv4_next_protocol_prefix_len = 8;
                    term_key->ipv4_next_protocol = SOC_SAND_PP_IP_PROTOCOL_IP_in_IP; /* IPV4 */
                    break;
                case bcmTunnelTypeIp6In6:
                    term_key->ipv4_next_protocol_prefix_len = 8;
                    term_key->ipv4_next_protocol = SOC_SAND_PP_IP_PROTOCOL_IPv6; /* IPV6 */
                    break;
                case bcmTunnelTypeGre4In6:
                case bcmTunnelTypeGre6In6:
                case bcmTunnelTypeGreAnyIn6:
                    term_key->ipv4_next_protocol_prefix_len = 8;
                    term_key->ipv4_next_protocol = SOC_SAND_PP_IP_PROTOCOL_GRE; /* GRE */
                    break;
                case bcmTunnelTypeUdp:
                    term_key->ipv4_next_protocol_prefix_len = 8;
                    term_key->ipv4_next_protocol = SOC_SAND_PP_IP_PROTOCOL_UDP; /* UDP */
                    break;
                case bcmTunnelTypeIpAnyIn6:
                    term_key->ipv4_next_protocol_prefix_len = 0; 
                    break; 
                case bcmTunnelTypeVxlan6:
                    term_key->ipv4_next_protocol_prefix_len = 8;
                    term_key->ipv4_next_protocol = SOC_SAND_PP_IP_PROTOCOL_UDP; /* UDP */
                    break;
                default:
                    term_key->ipv4_next_protocol_prefix_len = 0;
                    break;
                }

               
                /* Key include Qulifier next protocol */
                switch (info->type) {
                case bcmTunnelTypeGre4In6:
                    term_key->ip_gre_tunnel = 1; 
                    term_key->qualifier_next_protocol_prefix_len = 4;
                    term_key->qualifier_next_protocol = DPP_PARSER_IP_NEXT_PROTOCOL_IPv4; 
                    break;
                case bcmTunnelTypeGre6In6:
                    term_key->ip_gre_tunnel = 1; 
                    term_key->qualifier_next_protocol_prefix_len = 4;
                    term_key->qualifier_next_protocol = DPP_PARSER_IP_NEXT_PROTOCOL_IPv6;
                    break;
                case bcmTunnelTypeIp4In6:
                    term_key->ip_gre_tunnel = 0; 
                    term_key->qualifier_next_protocol_prefix_len = 4;
                    term_key->qualifier_next_protocol = DPP_PARSER_IP_NEXT_PROTOCOL_IPv4;
                    break;
                case bcmTunnelTypeIp6In6:
                    term_key->ip_gre_tunnel = 0; 
                    term_key->qualifier_next_protocol_prefix_len = 4;
                    term_key->qualifier_next_protocol = DPP_PARSER_IP_NEXT_PROTOCOL_IPv6;
                    break;
                case bcmTunnelTypeGreAnyIn6:
                    term_key->ip_gre_tunnel = 1; 
                    term_key->qualifier_next_protocol_prefix_len = 0;
                    term_key->qualifier_next_protocol = 0; 
                    break;
                case bcmTunnelTypeVxlan6:
                    term_key->ip_gre_tunnel = 0; 
                    term_key->qualifier_next_protocol_prefix_len = 4;
                    term_key->qualifier_next_protocol = 1; /* Eth */
                    break;
                default:
                    term_key->ip_gre_tunnel = 0; 
                    term_key->qualifier_next_protocol_prefix_len= 0;
                    term_key->qualifier_next_protocol = 0; 
                    break;
                }
            }

            /* Port property */
            term_key->port_property_en = 0;
            BCM_PBMP_ITER(info->pbmp, port_property) {
                /* If port property is in the legal range, keep it. Otherwise, return error. */
                if (port_property <= SOC_PPC_PORT_PROPERTY_VLAN_TRANSLATION_MAX) {
                    term_key->port_property_en = 1;
                    term_key->port_property = port_property;
                    break;
                } else {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Given port property is too high.")));
                }
            }
            
            /* Port property not found */
            if (port_property > SOC_PPC_PORT_PROPERTY_VLAN_TRANSLATION_MAX) {
                term_key->port_property_en = 0;
            }
        }
        
        BCM_EXIT;
    }

    /* SIP termination for vxlan and l2gre:
       the taget of this termination is mapping SIP to Learning info
       in this case only sem-result-index is used (by PMF),
       sem-result table is not configured.
     */
    if (info->type == bcmTunnelTypeVxlan || info->type == bcmTunnelTypeL2Gre) {
        if((info->sip_mask != 0 && info->dip_mask == 0) && !SOC_IS_DIP_SIP_VRF_WITH_MY_VTEP_INDEX_ENABLE(unit)){
            term_key->sip = info->sip;
            term_key->sip_prefix_len = bcm_ip_mask_length(info->sip_mask);
            term_key->flags |= SOC_PPC_RIF_IP_TERM_FLAG_KEY_ONLY; /* only add to SEM */
        }
    }


    if(info->sip_mask != 0) {
        term_key->sip = info->sip;
        term_key->sip_prefix_len = bcm_ip_mask_length(info->sip_mask);
    }
    /* key includes DIP */
    if(info->dip_mask != 0) {
        term_key->dip = info->dip;
        term_key->dip_prefix_len = bcm_ip_mask_length(info->dip_mask);
    }

    /* key includes IPV4 next protocol */
    switch (info->type) {
    case bcmTunnelTypeIp4In4:
    case bcmTunnelTypeIp4In6:
        term_key->ipv4_next_protocol_prefix_len = 8;
        term_key->ipv4_next_protocol = SOC_SAND_PP_IP_PROTOCOL_IP_in_IP; /* IPV4 */
        break;
    case bcmTunnelTypeIp6In4:
    case bcmTunnelTypeIp6In6:
        term_key->ipv4_next_protocol_prefix_len = 8;
        term_key->ipv4_next_protocol = SOC_SAND_PP_IP_PROTOCOL_IPv6; /* IPV6 */
        break;
    case bcmTunnelTypeGreAnyIn4:
    case bcmTunnelTypeGre4In4:
    case bcmTunnelTypeGre6In4:
    case bcmTunnelTypeGre4In6:
    case bcmTunnelTypeGre6In6:
    case bcmTunnelTypeGreAnyIn6:
        term_key->ipv4_next_protocol_prefix_len = 8;
        term_key->ipv4_next_protocol = SOC_SAND_PP_IP_PROTOCOL_GRE; /* GRE */
        break;
    case bcmTunnelTypeUdp:
        term_key->ipv4_next_protocol_prefix_len = 8;
        term_key->ipv4_next_protocol = SOC_SAND_PP_IP_PROTOCOL_UDP; /* UDP */
        break;
    case bcmTunnelTypeIpAnyIn4:
    case bcmTunnelTypeIpAnyIn6:
        term_key->ipv4_next_protocol_prefix_len = 0; 
        break; 
    default:
        term_key->ipv4_next_protocol_prefix_len = 0;
    }

    /* In case key includes ipv4 next protocol, check if port property is also part of the key. */
    if (term_key->ipv4_next_protocol_prefix_len) {
        BCM_PBMP_ITER(info->pbmp, port_property){
            /* If port property is in the legal range, keep it. Otherwise, return error. */
            if (port_property <= SOC_PPC_PORT_PROPERTY_VLAN_TRANSLATION_MAX) {
                term_key->port_property_en = 1;
                term_key->port_property = port_property;
                break;
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Given port property is too high.")));
            }
        }

        /* Port property not found */
        if (port_property > SOC_PPC_PORT_PROPERTY_VLAN_TRANSLATION_MAX) {
            term_key->port_property_en = 0;
        }
    }


    /* Indicate it's IP-LIF-Dummy */
    if (SOC_DPP_CONFIG(unit)->pp.ip_lif_dummy_id &&
        (info->flags & BCM_TUNNEL_TERM_TUNNEL_WITH_ID) &&
        ((SOC_DPP_CONFIG(unit)->pp.ip_lif_dummy_id) == BCM_GPORT_TUNNEL_ID_GET(info->tunnel_id))) {
        term_key->flags |= SOC_PPC_RIF_IP_TERM_FLAG_USE_DIP_DUMMY;
        if (SOC_IS_JERICHO(unit) && (SOC_DPP_CONFIG(unit)->pp.ip_lif_dummy_id == SOC_DPP_CONFIG(unit)->pp.explicit_null_support_lif_id)) {
            term_key->flags |= SOC_PPC_RIF_IP_TERM_FLAG_KEY_ONLY;
        }
    }

    /* Indicate it's VXLAN-LIF-Dummy */
    if (SOC_DPP_CONFIG(unit)->pp.ip_lif_dummy_id_vxlan &&
        (info->flags & BCM_TUNNEL_TERM_TUNNEL_WITH_ID) &&
        ((SOC_DPP_CONFIG(unit)->pp.ip_lif_dummy_id_vxlan) == BCM_GPORT_TUNNEL_ID_GET(info->tunnel_id))) {
        term_key->flags |= SOC_PPC_RIF_IP_TERM_FLAG_USE_DIP_DUMMY;
    }

    /* Indicate it's GRE-LIF-Dummy */
    if (SOC_DPP_CONFIG(unit)->pp.ip_lif_dummy_id_gre &&
        (info->flags & BCM_TUNNEL_TERM_TUNNEL_WITH_ID) &&
        ((SOC_DPP_CONFIG(unit)->pp.ip_lif_dummy_id_gre) == BCM_GPORT_TUNNEL_ID_GET(info->tunnel_id))) {
        term_key->flags |= SOC_PPC_RIF_IP_TERM_FLAG_USE_DIP_DUMMY | SOC_PPC_RIF_IP_TERM_FLAG_IP_GRE_TUNNEL_IS_VALID;
    }

    /* Indicate it's IP-LIF-Dummy for port termination */
    if (SOC_DPP_CONFIG(unit)->pp.custom_feature_ip_port_termination_lif_id &&
        (info->flags & BCM_TUNNEL_TERM_TUNNEL_WITH_ID) &&
        ((SOC_DPP_CONFIG(unit)->pp.custom_feature_ip_port_termination_lif_id) == BCM_GPORT_TUNNEL_ID_GET(info->tunnel_id))) {
        term_key->flags |= SOC_PPC_RIF_IP_TERM_FLAG_USE_DIP_DUMMY;
    }
    
    if (SOC_DPP_CONFIG(unit)->pp.custom_feature_ip_l3_gre_port_termination_lif_id &&
        (info->flags & BCM_TUNNEL_TERM_TUNNEL_WITH_ID) &&
        ((SOC_DPP_CONFIG(unit)->pp.custom_feature_ip_l3_gre_port_termination_lif_id) == BCM_GPORT_TUNNEL_ID_GET(info->tunnel_id))) {
        term_key->flags |= SOC_PPC_RIF_IP_TERM_FLAG_USE_DIP_DUMMY | SOC_PPC_RIF_IP_TERM_FLAG_IP_GRE_TUNNEL_IS_VALID;
    }

    /* Indicate it's MPLS-LIF-Dummy for explicit null
         */
    if (SOC_IS_JERICHO(unit) && SOC_DPP_CONFIG(unit)->pp.explicit_null_support 
        && (info->flags & BCM_TUNNEL_TERM_TUNNEL_WITH_ID)
        && BCM_L3_ITF_TYPE_IS_LIF(info->if_class)) {
        term_key->flags = SOC_PPC_RIF_IP_TERM_FLAG_USE_DIP_EXPLICIT_NULL_DUMMY;
        term_key->ttl = BCM_L3_ITF_VAL_GET(info->if_class);

        if ((SOC_DPP_CONFIG(unit)->pp.explicit_null_support_lif_id == BCM_GPORT_TUNNEL_ID_GET(info->tunnel_id)) && term_key->ttl != 2) {
            term_key->flags |= SOC_PPC_RIF_IP_TERM_FLAG_KEY_ONLY;
        } 
    }

    /* vrf is used as termination key in DIP+SIP+VRF mode and
       VXLAN and UDP tunnel modes: */
    if (SOC_IS_JERICHO(unit)
        && ((SOC_DPP_CONFIG(unit)->pp.ipv4_tunnel_term_bitmap_enable & (SOC_DPP_IP_TUNNEL_TERM_DB_DIP_SIP_VRF)) 
            || SOC_IS_DIP_SIP_VRF_WITH_MY_VTEP_INDEX_ENABLE(unit))
        && (DPP_VRF_VALID(unit, info->vrf))) {
        term_key->vrf = info->vrf; 
        /* flag indicates VRF is valid */
        term_key->flags |= SOC_PPC_RIF_IP_TERM_FLAG_VRF_IS_VALID; 
    }

    /* for DIP SIP VRF using my-VTEP-index, there are 2 lookups performed by bcm_tunnel_terminator_create: 
       - SIP, my-vtep-index, VRF -> inLif 
       - DIP, SIP, VRF -> inLif           
       we need to identify which entry we're configuring according to flag indicating tunnel_class is valid
       Note: there is a third lookup: DIP -> my-vtep-index performed by bcm_tunnel_terminator_config
    */ 

    if (info->flags & BCM_TUNNEL_TERM_USE_TUNNEL_CLASS) {
        /* we're configuring lookup: SIP, my-vtep-index, VRF -> inLif */
        term_key->my_vtep_index = info->tunnel_class;
        term_key->flags |= SOC_PPC_RIF_IP_TERM_FLAG_MY_VTEP_INDEX_IS_VALID; 
    }

    /* for DIP SIP VRF tunnel mode */
    if (SOC_IS_JERICHO(unit) 
        && (SOC_DPP_CONFIG(unit)->pp.ipv4_tunnel_term_bitmap_enable & (SOC_DPP_IP_TUNNEL_TERM_DB_DIP_SIP_VRF))) {

        /* Key include GRE.ethertype, is IP GRE tunnel */
        switch (info->type) {
        case bcmTunnelTypeIp4In4:
        case bcmTunnelTypeIp6In4:
        case bcmTunnelTypeIpAnyIn4:
        case bcmTunnelTypeIp4In6:
        case bcmTunnelTypeIp6In6:
        case bcmTunnelTypeIpAnyIn6:
            term_key->gre_ethertype_prefix_len= 0;
            term_key->ip_gre_tunnel = 0; 
            break; 
        case bcmTunnelTypeGreAnyIn4:
        case bcmTunnelTypeGreAnyIn6:
            term_key->gre_ethertype_prefix_len= 0;
            term_key->ip_gre_tunnel = 1; 
            break; 
        case bcmTunnelTypeGre4In4:
        case bcmTunnelTypeGre4In6:
            term_key->gre_ethertype_prefix_len= SOC_SAND_PP_ETHER_TYPE_NOF_BITS;
            term_key->gre_ethertype = SOC_SAND_PP_ETHERNET_ETHERTYPE_IPV4; 
            term_key->ip_gre_tunnel = 1; 
            break;
        case bcmTunnelTypeUdp:
               /* udp tunnel termination uses tt key as gre */
               term_key->gre_ethertype_prefix_len= SOC_SAND_PP_ETHER_TYPE_NOF_BITS;
               term_key->gre_ethertype = info->udp_dst_port;
               term_key->ip_gre_tunnel = 0;
           break;
        case bcmTunnelTypeGre6In4:
        case bcmTunnelTypeGre6In6:
            term_key->gre_ethertype_prefix_len= SOC_SAND_PP_ETHER_TYPE_NOF_BITS;
            term_key->gre_ethertype = SOC_SAND_PP_ETHERNET_ETHERTYPE_IPV6;
            term_key->ip_gre_tunnel = 1; 
            break;
        default:
            term_key->gre_ethertype_prefix_len= 0;
            term_key->ip_gre_tunnel = 0; 
        }

        /* is IP GRE tunnel is valid */
        term_key->flags |= SOC_PPC_RIF_IP_TERM_FLAG_IP_GRE_TUNNEL_IS_VALID; 

    }

exit:
    BCMDNX_FUNC_RETURN;
}




int 
bcm_petra_tunnel_terminator_add(
    int unit, 
    bcm_tunnel_terminator_t *info) 
{
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("API no longer supported. Please use bcm_petra_tunnel_terminator_create instead.")));

  exit:
    BCMDNX_FUNC_RETURN;

}

/* Add a tunnel terminator for DIP-SIP key. */
int 
_bcm_petra_tunnel_terminator_add(
    int unit, 
    bcm_tunnel_terminator_t *info) 
{
    int soc_sand_rv;
    unsigned int soc_sand_dev_id;
    SOC_PPC_RIF_IP_TERM_KEY term_key;
    SOC_PPC_RIF_IP_TERM_INFO term_info;
    SOC_SAND_SUCCESS_FAILURE success;
    int global_lif, local_inlif;
    int rv = BCM_E_NONE;
    int is_uniform=0,is_1_plus_1,update=0,with_id=0,lif_allocated=0;
    uint32 alloc_flags = 0;
    uint32 is_ipv6_tunnel;
    uint8 counting_profile_is_allocated = FALSE; /*Check if count profile was allocated*/

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(info);

    /* check parameters */

    /* Check for valid flags */
    if (info->flags & _BCM_PETRA_TUNNEL_TERM_FLAGS_UNSUPPORTED){
        LOG_VERBOSE(BSL_LS_BCM_TUNNEL,
                    (BSL_META_U(unit,
                                "unsupported flags %x\n"), info->flags));
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("flags value not supported")));
    }

    /* combination for PIPE unfirom*/
    /* cannot keep one of: TTL, DSCP. both or none*/
    if(((info->flags & BCM_TUNNEL_TERM_USE_OUTER_TTL) && !(info->flags & BCM_TUNNEL_TERM_USE_OUTER_DSCP))||
       (!(info->flags & BCM_TUNNEL_TERM_USE_OUTER_TTL) && (info->flags & BCM_TUNNEL_TERM_USE_OUTER_DSCP))
       )
    {
        LOG_VERBOSE(BSL_LS_BCM_TUNNEL,
                    (BSL_META_U(unit,
                                "unsupported flags combination for TTL, DSCP %x\n"), info->flags));
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("unsupported flags combination for TTL, DSCP ")));
    }

    rv = _bcm_dpp_is_soc_property_termination_support(unit, info);
    BCMDNX_IF_ERR_EXIT(rv);

    /* skip ethernet is supported only for arad+ */
    if ((SOC_IS_ARAD_B1_AND_BELOW(unit) || SOC_IS_JERICHO(unit))
        && (info->flags & BCM_TUNNEL_TERM_ETHERNET)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("flags value not supported below 88660_A0")));
    }

    rv = _bcm_petra_tunnel_type_is_ipv6(unit,info->type, &is_ipv6_tunnel);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!is_ipv6_tunnel) {
        /* DIP and tunnel_class (my_vtep_index) are not supported together:
           reminder: use use my_vtep_index for lookup: my_vtep_index, SIP, VRF lookup. */
        if ((info->flags & BCM_TUNNEL_TERM_USE_TUNNEL_CLASS) && (info->dip != 0)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("tunnel_class and DIP are not supported together")));
        }

        /* validate tunnel_class */
        if ((info->flags & BCM_TUNNEL_TERM_USE_TUNNEL_CLASS) && (soc_sand_msb_bit_on(info->tunnel_class) >= (SOC_DPP_CONFIG(unit))->pp.vxlan_tunnel_term_in_sem_my_vtep_index_nof_bits)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("tunnel class value (%d) is invalid,  support only up to %d bits for my-vtep-index"), info->tunnel_class, (SOC_DPP_CONFIG(unit))->pp.vxlan_tunnel_term_in_sem_my_vtep_index_nof_bits));
        }

        /* in vxlan tunnel mode 3, there is a limitation on nof bit for VRF in my-vtep-index, sip, vrf lookup:
           nof bit VRF + nof bit of my-vtep-index <= 15.
           nof bit VRF is indicated using a soc property. 
           we check flag BCM_TUNNEL_TERM_USE_TUNNEL_CLASS to make sure we're configuring this lookup
           reminder: in this tunnel term mode, we can configure an additional lookup: DIP, SIP, VRF */
        if (SOC_IS_DIP_SIP_VRF_WITH_MY_VTEP_INDEX_ENABLE(unit) 
            && (info->flags & BCM_TUNNEL_TERM_USE_TUNNEL_CLASS)
            && (soc_sand_msb_bit_on(info->vrf) >= SOC_DPP_CONFIG(unit)->pp.vxlan_tunnel_term_in_sem_vrf_nof_bits)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("vrf value (%d) is invalid, support only up to %d bits for VRF in this mode"), info->vrf, SOC_DPP_CONFIG(unit)->pp.vxlan_tunnel_term_in_sem_vrf_nof_bits));
        }

        /* for tunnel termination, tunnelType is mandatory */
        if ((SOC_IS_JERICHO(unit) 
            && (SOC_DPP_CONFIG(unit)->pp.ipv4_tunnel_term_bitmap_enable & (SOC_DPP_IP_TUNNEL_TERM_DB_DIP_SIP_VRF))) 
            && (info->type == bcmTunnelTypeNone)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("type value (%d) is invalid, type is a required field"), info->type));
        }
    } else {
        uint32 is_ipv6_valid;

        rv = _bcm_petra_ip6_addr_is_valid(unit, info->dip6, &is_ipv6_valid);
        BCMDNX_IF_ERR_EXIT(rv);
        if ((info->flags & BCM_TUNNEL_TERM_USE_TUNNEL_CLASS) && is_ipv6_valid) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("tunnel_class and DIP are not supported together")));
        }

        /* validate tunnel_class */
        if ((info->flags & BCM_TUNNEL_TERM_USE_TUNNEL_CLASS) && (soc_sand_msb_bit_on(info->tunnel_class) >= 6)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("tunnel class value (%d) is invalid,  support only up to %d bits for my-vtep-index"), info->tunnel_class, (SOC_DPP_CONFIG(unit))->pp.vxlan_tunnel_term_in_sem_my_vtep_index_nof_bits));
        }

        /* in vxlan tunnel mode 3, there is a limitation on nof bit for VRF in my-vtep-index, sip, vrf lookup:
           nof bit VRF + nof bit of my-vtep-index <= 15.
           nof bit VRF is indicated using a soc property. 
           we check flag BCM_TUNNEL_TERM_USE_TUNNEL_CLASS to make sure we're configuring this lookup
           reminder: in this tunnel term mode, we can configure an additional lookup: DIP, SIP, VRF */
        if (SOC_IS_DIP_SIP_VRF_WITH_MY_VTEP_INDEX_ENABLE(unit) 
            && (info->flags & BCM_TUNNEL_TERM_USE_TUNNEL_CLASS)
            && (soc_sand_msb_bit_on(info->vrf) >= SOC_DPP_CONFIG(unit)->pp.ip6_tunnel_term_in_tcam_vrf_nof_bits)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("vrf value (%d) is invalid, support only up to %d bits for VRF in this mode"), info->vrf, SOC_DPP_CONFIG(unit)->pp.vxlan_tunnel_term_in_sem_vrf_nof_bits));
        }
    }    

    soc_sand_dev_id = (unit);
    SOC_PPC_RIF_IP_TERM_INFO_clear(&term_info);
    SOC_PPC_RIF_IP_TERM_KEY_clear(&term_key);

    /* map BCM key to PPD key */
    rv = bcm_petra_tunnel_term_key_to_ppd(unit,info,&term_key);
    BCMDNX_IF_ERR_EXIT(rv);

    /* skip ethernet */
    #ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS(unit)) {
      term_info.skip_ethernet = ((info->flags & BCM_TUNNEL_TERM_ETHERNET) != 0);
    }
    #endif

    /* Read the state of the flags */
    update = (info->flags & BCM_TUNNEL_REPLACE) ? TRUE : FALSE;
    with_id = (info->flags & BCM_TUNNEL_TERM_TUNNEL_WITH_ID) ? TRUE : FALSE;

    /* make sure this tunnel terminator entry doesn't already exist */
    if (!update) {
        int temp_local_lif; 
        SOC_PPC_RIF_IP_TERM_INFO temp_term_info;
        SOC_PPC_RIF_INFO temp_rif_info;
        uint8 found; 

        SOC_PPC_RIF_IP_TERM_INFO_clear(&temp_term_info); 
        SOC_PPC_RIF_INFO_clear(&temp_rif_info); 

        soc_sand_rv = soc_ppd_rif_ip_tunnel_term_get(soc_sand_dev_id, &term_key, (SOC_PPC_LIF_ID*) &temp_local_lif,
                                                     &temp_term_info, &temp_rif_info, &found);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        if (found) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("In case tunnel termination key already exists, BCM_TUNNEL_REPLACE should be used")));
        }
    }

    if(term_key.flags & SOC_PPC_RIF_IP_TERM_FLAG_KEY_ONLY) {
        if (info->tunnel_id == 0) { /* tunnel indicate sem-result-index */
            LOG_VERBOSE(BSL_LS_BCM_TUNNEL,
                        (BSL_META_U(unit,
                                    "tunnel_id has to be set\n")));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("tunnel_id has to be set")));
        }
        
        if (term_key.flags & SOC_PPC_RIF_IP_TERM_FLAG_USE_DIP_EXPLICIT_NULL_DUMMY) {
            rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS, BCM_GPORT_TUNNEL_ID_GET(info->tunnel_id), &local_inlif);
            if (rv == BCM_E_NOT_FOUND) {
                local_inlif = BCM_GPORT_IS_TUNNEL(info->tunnel_id)?BCM_GPORT_TUNNEL_ID_GET(info->tunnel_id):info->tunnel_id;
            } else {
                BCMDNX_IF_ERR_EXIT(rv);
            }
        } 
        else
        {
            local_inlif = BCM_GPORT_IS_TUNNEL(info->tunnel_id)?BCM_GPORT_TUNNEL_ID_GET(info->tunnel_id):info->tunnel_id;
        }

        soc_sand_rv =
        soc_ppd_rif_ip_tunnel_term_add(soc_sand_dev_id, (term_key.flags & SOC_PPC_RIF_IP_TERM_FLAG_USE_DIP_EXPLICIT_NULL_DUMMY)?0:SOC_PPC_RIF_IP_TUNNEL_TERM_ADD_LIF_ID_IS_FEC, 
                                                          &term_key, local_inlif,&term_info,  NULL,&success);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        SOC_SAND_IF_FAIL_RETURN(success);

        BCM_EXIT; /* done */
    }

    /* 1+1 protection */
    /* check if 1+1 protection is set, according to ingress_failover_id is valid */
    is_1_plus_1 = _BCM_PPD_IS_VALID_FAILOVER_ID(info->ingress_failover_id);


    /* set RIF ID, -1 means keep don't update RIF and keep previous value */
    if (info->tunnel_if == BCM_IF_INVALID) {
        term_info.rif = SOC_PPC_RIF_NULL;
    }
    else{
        term_info.rif = info->tunnel_if; 
    }


    /* module uniform or pipe
       uniform if keeping outer TTL and DSCP 
     */
    is_uniform = (info->flags & BCM_TUNNEL_TERM_USE_OUTER_DSCP) && (info->flags & BCM_TUNNEL_TERM_USE_OUTER_TTL);
    term_info.processing_type= (is_uniform)?SOC_PPC_MPLS_TERM_MODEL_UNIFORM :SOC_PPC_MPLS_TERM_MODEL_PIPE;


    /* check update/with-id allocations*/
    if (with_id && BCM_GPORT_IS_TUNNEL(info->tunnel_id)) {
        global_lif = BCM_GPORT_TUNNEL_ID_GET(info->tunnel_id);
        alloc_flags |= BCM_DPP_AM_FLAG_ALLOC_WITH_ID;

        if (SOC_IS_JERICHO(unit)) {
            rv = bcm_dpp_am_global_lif_is_alloced(unit, BCM_DPP_AM_FLAG_ALLOC_INGRESS, global_lif);
        } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            rv = bcm_dpp_am_l3_lif_is_allocated(unit, _BCM_DPP_AM_L3_LIF_IP_TERM, 0, global_lif);
        }
        lif_allocated = (rv == BCM_E_EXISTS) ? TRUE:FALSE;
    }

    /* this call will not update RIF attribute: VRF, cos-profile
       to update RIF attribute call bcm_petra_l3_intf_create/bcm_petra_l3_ingress_create*/
    /* cos profile is set for tunnel according to bcm_petra_qos_port_map_get*/

    /* allocate LIf for IP tunnel termination */
    /* cannot update without given id */
    if (update) {
        if (!with_id) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_TUNNEL_REPLACE can't be used without BCM_TUNNEL_WITH_ID as well")));
        }
        else if (!(alloc_flags & BCM_DPP_AM_FLAG_ALLOC_WITH_ID)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Updated flag is set without provide ID")));
        }
        else if (!lif_allocated) { /* update check LIF already allocated */
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Updated IP tunnel not exist")));
        }

        rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS, global_lif, &local_inlif);
        BCMDNX_IF_ERR_EXIT(rv);

        if (DPP_IS_FLAG_SET(info->flags, BCM_TUNNEL_TERM_WIDE)) {
            uint64 additional_data;

            rv = arad_pp_lif_additional_data_get(unit, local_inlif, TRUE, &additional_data);
            SOCDNX_SAND_IF_ERR_EXIT(rv);
            term_info.additional_data = additional_data;
        }
    } else if (with_id && lif_allocated) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("In case tunnel_id already exists, BCM_TUNNEL_REPLACE should be used")));
    } else { /* allocated LIF */

        if (SOC_IS_JERICHO(unit)) {
            bcm_dpp_am_local_inlif_info_t local_inlif_info;

            /* Set the local lif info */
            sal_memset(&local_inlif_info, 0, sizeof(local_inlif_info));
            local_inlif_info.app_type = bcm_dpp_am_ingress_lif_app_ip_term;
            local_inlif_info.local_lif_flags = DPP_IS_FLAG_SET(info->flags, BCM_TUNNEL_TERM_WIDE) ? 
                                                    BCM_DPP_AM_IN_LIF_FLAG_WIDE : BCM_DPP_AM_IN_LIF_FLAG_COMMON;

            rv = bcm_dpp_counter_lif_range_is_allocated(unit, &counting_profile_is_allocated);
            BCMDNX_IF_ERR_EXIT(rv);

            /*Check if counter profile was allocated or if this is null label it should be reserved */
            if (counting_profile_is_allocated){
                if (info->inlif_counting_profile != BCM_STAT_LIF_COUNTING_PROFILE_NONE && info->inlif_counting_profile >= SOC_TMC_CNT_LIF_COUNTING_NOF_PROFILES(unit)){
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("In-LIF-counting-profile %d is out of range."), info->inlif_counting_profile));
                }
                BCMDNX_IF_ERR_EXIT(bcm_dpp_counter_lif_local_profile_get(unit, info->inlif_counting_profile, &local_inlif_info.counting_profile_id));                
            }
            else if ((SOC_DPP_CONFIG(unit)->pp.explicit_null_support && (SOC_DPP_CONFIG(unit)->pp.explicit_null_support_lif_id == global_lif)) || 
                (SOC_DPP_CONFIG(unit)->pp.explicit_null_support && (global_lif != 0) && 
                 ((soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "logical_interface_for_explicit_null_ttl0", 0) == global_lif) || 
                 (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "logical_interface_for_explicit_null_ttl1", 0) == global_lif)))||
                (SOC_DPP_CONFIG(unit)->pp.ip_lif_dummy_id != 0) || 
                (SOC_DPP_CONFIG(unit)->pp.ip_lif_dummy_id_vxlan != 0) ||
                (SOC_DPP_CONFIG(unit)->pp.ip_lif_dummy_id_gre != 0) ||
                ((SOC_DPP_CONFIG(unit)->pp.custom_feature_ip_l3_gre_port_termination_lif_id == global_lif) ||
                (SOC_DPP_CONFIG(unit)->pp.custom_feature_ip_port_termination_lif_id ==  global_lif) ||
                (SOC_DPP_CONFIG(unit)->pp.custom_feature_mpls_port_termination_lif_id == global_lif))) {
                local_inlif_info.counting_profile_id = BCM_DPP_AM_COUNTING_PROFILE_RESERVED;
            }
            else {
                local_inlif_info.counting_profile_id = BCM_STAT_LIF_COUNTING_PROFILE_NONE;
            }

            /* Allocate the global and local lif */
            rv = _bcm_dpp_gport_alloc_global_and_local_lif(unit, alloc_flags, &global_lif, &local_inlif_info, NULL);
            BCM_IF_ERROR_RETURN(rv);

            LOG_VERBOSE(BSL_LS_BCM_TUNNEL,
                            (BSL_META_U(unit,
                                    "_bcm_petra_tunnel_terminator_add: global_lif 0x%08x local_inlif_info.base_lif_id 0x%08x\n"), global_lif, local_inlif_info.base_lif_id));

            /* Retrieve the allocated local lif */
            local_inlif = local_inlif_info.base_lif_id;

        } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
            rv = bcm_dpp_am_l3_lif_alloc(unit, _BCM_DPP_AM_L3_LIF_IP_TERM, alloc_flags, &global_lif);
            BCMDNX_IF_ERR_EXIT(rv);
            local_inlif = global_lif;
        }
    }

    if (SOC_IS_JERICHO(unit)) {
        term_info.global_lif = global_lif;
    }

    /* if update then don't overwrite this consts */
    if (!update) {
        /* for IP tunnel no learning, l2-GRE can override */
        term_info.learn_enable = 0;
        /* by default spoke so no filtering */
        term_info.orientation = SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE;
    }

    /* 1+1 protection, set protection info in LIF */
    if (is_1_plus_1) {
        _BCM_PPD_FAILOVER_ID_PLUS_1_TO_ID(term_info.protection_pointer, info->ingress_failover_id);
        term_info.protection_pass_value = _BCM_PPD_TUNNEL_IS_WORKING_PORT(info)?0:1;
    }

    term_info.is_extended = DPP_IS_FLAG_SET(info->flags, BCM_TUNNEL_TERM_WIDE);


    /* call ppd for config */
    soc_sand_rv =
	soc_ppd_rif_ip_tunnel_term_add(soc_sand_dev_id, 0, &term_key, local_inlif, &term_info,  NULL,&success);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    SOC_SAND_IF_FAIL_RETURN(success);

    /* tunnel id */
    /* tunnel ID always is LIF-id,
     * also for 1+1: protection, MC-destination is seperate handled object. 
     */
    BCM_GPORT_TUNNEL_ID_SET(info->tunnel_id,global_lif);

    /* store in SW state LIF usage */
    rv = _bcm_dpp_in_lif_tunnel_term_add(unit, info, local_inlif);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/* Creates a new tunnel terminator for DIP-SIP key. */
int 
bcm_petra_tunnel_terminator_create(
    int unit, 
    bcm_tunnel_terminator_t *info) 
{
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_petra_tunnel_terminator_add(unit, info);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
  exit:
    BCMDNX_FUNC_RETURN;
}

/* Delete a tunnel terminator for DIP-SIP key. */
int 
bcm_petra_tunnel_terminator_delete(
    int unit, 
    bcm_tunnel_terminator_t *info)
{
    int soc_sand_rv;
    unsigned int soc_sand_dev_id;
    int rv = BCM_E_NONE;
    int local_inlif, global_inlif;
    SOC_PPC_RIF_IP_TERM_KEY
        term_key;
    uint64 additinal_data;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(info);

    soc_sand_dev_id = (unit);

    if (info->flags & BCM_L3_IP6) {
        LOG_VERBOSE(BSL_LS_BCM_TUNNEL,
                    (BSL_META_U(unit,
                                "IPv6: flags %x\n"), info->flags));
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("IPv6 not supported")));
    }

	rv = _bcm_dpp_is_soc_property_termination_support(unit, info);
    BCMDNX_IF_ERR_EXIT(rv);

    /* get the info before the delete */
    rv = bcm_petra_tunnel_terminator_get(unit, info);
    BCMDNX_IF_ERR_EXIT(rv);

    /* build key */
    SOC_PPC_RIF_IP_TERM_KEY_clear(&term_key);
    rv = bcm_petra_tunnel_term_key_to_ppd(unit,info,&term_key);
    BCMDNX_IF_ERR_EXIT(rv);

    soc_sand_rv =
        soc_ppd_rif_ip_tunnel_term_remove(soc_sand_dev_id, &term_key, (SOC_PPC_LIF_ID*)&local_inlif);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (local_inlif == BCM_GPORT_INVALID) {
        BCMDNX_IF_ERR_EXIT(BCM_E_NOT_FOUND);
    }

    if (DPP_IS_FLAG_SET(info->flags, BCM_TUNNEL_TERM_WIDE))
    {
        /* Call the SOC API for clear the additional data */
        COMPILER_64_SET(additinal_data, 0, 0);
        rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_lif_additional_data_set,(unit, local_inlif, TRUE, additinal_data, TRUE)));
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if (!(term_key.flags & SOC_PPC_RIF_IP_TERM_FLAG_KEY_ONLY)) {

        if (SOC_IS_JERICHO(unit)) {
            rv = _bcm_dpp_global_lif_mapping_local_to_global_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS, local_inlif, &global_inlif);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = _bcm_dpp_gport_delete_global_and_local_lif(unit, global_inlif, local_inlif, _BCM_GPORT_ENCAP_ID_LIF_INVALID);
            BCMDNX_IF_ERR_EXIT(rv);
        } else {
            rv = bcm_dpp_am_l3_lif_dealloc(unit, local_inlif);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        
        rv = _bcm_dpp_local_lif_sw_resources_delete(unit, local_inlif, -1, _BCM_DPP_GPORT_SW_RESOURCES_INGRESS);
        BCMDNX_IF_ERR_EXIT(rv); 
    }
    
exit:
    BCMDNX_FUNC_RETURN;
}

/* Update a tunnel terminator for DIP-SIP key. */
int 
bcm_petra_tunnel_terminator_update(
    int unit, 
    bcm_tunnel_terminator_t *info)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_tunnel_terminator_update is not available")));
exit:
    BCMDNX_FUNC_RETURN;
}

/* Get a tunnel terminator for DIP-SIP key. */
int 
bcm_petra_tunnel_terminator_get(
    int unit, 
    bcm_tunnel_terminator_t *info)
{
    SOC_PPC_RIF_IP_TERM_INFO term_info;
    SOC_PPC_RIF_INFO rif_info;
    uint8 found;
    int soc_sand_rv;
    unsigned int soc_sand_dev_id;
    int global_lif, local_inlif;
    SOC_PPC_RIF_IP_TERM_KEY
        term_key;
    int rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(info);

    soc_sand_dev_id = (unit);
    SOC_PPC_RIF_IP_TERM_INFO_clear(&term_info);
    SOC_PPC_RIF_INFO_clear(&rif_info);

    if (info->flags & BCM_L3_IP6) {
        LOG_VERBOSE(BSL_LS_BCM_TUNNEL,
                    (BSL_META_U(unit,
                                "IPv6: flags %x\n"), info->flags));
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("IPv6 not supported")));
    }

	rv = _bcm_dpp_is_soc_property_termination_support(unit, info);
    BCMDNX_IF_ERR_EXIT(rv);

    /* map BCM key to PPD key */
    SOC_PPC_RIF_IP_TERM_KEY_clear(&term_key);

    rv = bcm_petra_tunnel_term_key_to_ppd(unit,info,&term_key);
    BCMDNX_IF_ERR_EXIT(rv);

    soc_sand_rv = soc_ppd_rif_ip_tunnel_term_get(soc_sand_dev_id, &term_key, (SOC_PPC_LIF_ID*) &local_inlif,
                                                 &term_info, &rif_info, &found);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    
    if (!found) {
        BCMDNX_IF_ERR_EXIT(BCM_E_NOT_FOUND);
    }

    /* set tunnel id handle*/
    rv = _bcm_dpp_global_lif_mapping_local_to_global_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS, local_inlif, &global_lif);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_GPORT_TUNNEL_ID_SET(info->tunnel_id,global_lif);

    if (term_key.flags & SOC_PPC_RIF_IP_TERM_FLAG_KEY_ONLY) {
        BCM_EXIT;
    }

    /* uniform or pipe*/
    if(term_info.processing_type == SOC_PPC_MPLS_TERM_MODEL_UNIFORM) {
        info->flags |= BCM_TUNNEL_TERM_USE_OUTER_DSCP | BCM_TUNNEL_TERM_USE_OUTER_TTL;
    }

    /* Get wide entry info */
    if (SOC_IS_JERICHO(unit)) {
        uint8 is_wide_entry;

        rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_lif_is_wide_entry,(unit, local_inlif, TRUE, &is_wide_entry, NULL));
        BCMDNX_IF_ERR_EXIT(rv);

        info->flags |= (is_wide_entry) ? BCM_TUNNEL_TERM_WIDE : 0;
    }


    /* skip ethernet */
    #ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS(unit)) {
        if (term_info.skip_ethernet) {
            info->flags |= BCM_TUNNEL_TERM_ETHERNET;
        }
    }
    #endif


    /* update from LIF sw state */
    rv = _bcm_dpp_in_lif_tunnel_term_get(unit, info, local_inlif);
    BCMDNX_IF_ERR_EXIT(rv);

    if (term_info.rif != SOC_PPC_RIF_NULL) {
        info->tunnel_if = term_info.rif;
    }
    else{
        info->tunnel_if = BCM_IF_INVALID;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* Set the valid payload VLANs for this WLAN tunnel. */
int 
bcm_petra_tunnel_terminator_vlan_set(
    int unit, 
    bcm_gport_t tunnel, 
    bcm_vlan_vector_t vlan_vec)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported")));
exit:
    BCMDNX_FUNC_RETURN;
}

/* Get the valid payload VLANs for this WLAN tunnel. */
int 
bcm_petra_tunnel_terminator_vlan_get(
    int unit, 
    bcm_gport_t tunnel, 
    bcm_vlan_vector_t *vlan_vec)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported")));
exit:
    BCMDNX_FUNC_RETURN;
}

/* Create a tunnel DSCP map instance. */
int 
bcm_petra_tunnel_dscp_map_create(
    int unit, 
    uint32 flags, 
    int *dscp_map_id)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported")));
exit:
    BCMDNX_FUNC_RETURN;
}

/* Destroy a tunnel DSCP map instance. */
int 
bcm_petra_tunnel_dscp_map_destroy(
    int unit, 
    int dscp_map_id)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported")));
exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * Get the { internal priority, color }-to-DSCP mapping in the specified
 * DSCP map.
 */
int 
bcm_petra_tunnel_dscp_map_get(
    int unit, 
    int dscp_map_id, 
    bcm_tunnel_dscp_map_t *dscp_map)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported")));
exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * Set the { internal priority, color }-to-DSCP mapping in the specified
 * DSCP map.
 */
int 
bcm_petra_tunnel_dscp_map_set(
    int unit, 
    int dscp_map_id, 
    bcm_tunnel_dscp_map_t *dscp_map)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported")));
exit:
    BCMDNX_FUNC_RETURN;
}

/* Set port egress { internal priority, color }-to-DSCP mapping. */
int 
bcm_petra_tunnel_dscp_map_port_set(
    int unit, 
    bcm_port_t port, 
    bcm_tunnel_dscp_map_t *dscp_map)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported")));
exit:
    BCMDNX_FUNC_RETURN;
}

/* Get port egress { internal priority, color }-to-DSCP mapping. */
int 
bcm_petra_tunnel_dscp_map_port_get(
    int unit, 
    bcm_port_t port, 
    bcm_tunnel_dscp_map_t *dscp_map)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported")));
exit:
    BCMDNX_FUNC_RETURN;
}

/* Set or get the global chip tunneling configuration. */
int 
bcm_petra_tunnel_config_get(
    int unit, 
    bcm_tunnel_config_t *tconfig)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported")));
exit:
    BCMDNX_FUNC_RETURN;
}

/* Set or get the global chip tunneling configuration. */
int 
bcm_petra_tunnel_config_set(
    int unit, 
    bcm_tunnel_config_t *tconfig)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported")));
exit:
    BCMDNX_FUNC_RETURN;
}


/* check that required soc properties are enabled */
int _bcm_petra_tunnel_terminator_config_key_verify(int unit, bcm_tunnel_terminator_config_key_t *config_key) {
    int rv = BCM_E_NONE;
    uint32 is_ipv6_tunnel;
    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_petra_tunnel_type_is_ipv6(unit,config_key->type, &is_ipv6_tunnel);
    BCMDNX_IF_ERR_EXIT(rv);
    if (is_ipv6_tunnel) {
        if ((SOC_DPP_CONFIG(unit)->pp.ipv6_tunnel_term_bitmap_enable & SOC_DPP_IP_TUNNEL_TERM_DB_DIP_SIP_VRF )== 0) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_tunnel_terminator_config_add is only supported with soc property spn_BCM886XX_IP6_TUNNEL_TERMINATION_MODE enabled as 2 for IPv6 ")));
        }
    } else {
        if ((SOC_DPP_CONFIG(unit)->pp.ipv4_tunnel_term_bitmap_enable & SOC_DPP_IP_TUNNEL_TERM_DB_VXLAN) == 0) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_tunnel_terminator_config_add is only supported with soc property BCM886XX_VXLAN_ENABLE enabled ")));
        }
        
        if (!SOC_IS_DIP_SIP_VRF_WITH_MY_VTEP_INDEX_ENABLE(unit)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("bcm_tunnel_terminator_config_add is only supported with soc property BCM886XX_VXLAN_TUNNEL_LOOKUP_MODE = 3")));
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* check that required soc properties are enabled */
int _bcm_petra_tunnel_terminator_config_action_verify(int unit, bcm_tunnel_terminator_config_key_t *config_key, bcm_tunnel_terminator_config_action_t *config_action) {
    int rv = BCM_E_NONE;
    uint32 is_ipv6_tunnel;
    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_petra_tunnel_type_is_ipv6(unit,config_key->type, &is_ipv6_tunnel);
    BCMDNX_IF_ERR_EXIT(rv);
    if (is_ipv6_tunnel) {
        if (soc_sand_msb_bit_on(config_action->tunnel_class) >= 6) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("tunnel class value (%d) is invalid, max nof bits available: %d"),config_action->tunnel_class, 6));
        }
    } else {
        if (soc_sand_msb_bit_on(config_action->tunnel_class) >= (SOC_DPP_CONFIG(unit))->pp.vxlan_tunnel_term_in_sem_my_vtep_index_nof_bits) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("tunnel class value (%d) is invalid, max nof bits available: %d"),config_action->tunnel_class, (SOC_DPP_CONFIG(unit))->pp.vxlan_tunnel_term_in_sem_my_vtep_index_nof_bits));
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_petra_tunnel_terminator_config_add_verify( 
    int unit, 
    uint32 flags, 
    bcm_tunnel_terminator_config_key_t *config_key, 
    bcm_tunnel_terminator_config_action_t *config_action) {

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(_bcm_petra_tunnel_terminator_config_key_verify(unit, config_key));
    BCMDNX_IF_ERR_EXIT(_bcm_petra_tunnel_terminator_config_action_verify(unit, config_key, config_action));

exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_petra_tunnel_term_config_key_to_ppd(
    int unit, 
    bcm_tunnel_terminator_config_key_t *config_key, 
    SOC_PPC_RIF_IP_TERM_KEY    *term_key) {

    BCMDNX_INIT_FUNC_DEFS;

    if (SOC_IS_JERICHO(unit)
        && (SOC_DPP_CONFIG(unit)->pp.ipv6_tunnel_term_bitmap_enable & SOC_DPP_IP_TUNNEL_TERM_DB_DIP_SIP_VRF)) {
        if (config_key->flags & BCM_TUNNEL_TERM_BUD) {
            term_key->flags |= SOC_PPC_RIF_IP_TERM_FLAG_BUD;
        }
        /* key includes the DIP */
        switch (config_key->type) {
        case bcmTunnelTypeIp4In6:
        case bcmTunnelTypeIp6In6:
        case bcmTunnelTypeIpAnyIn6:
        case bcmTunnelTypeGre4In6:
        case bcmTunnelTypeGre6In6:
        case bcmTunnelTypeGreAnyIn6:
        case bcmTunnelTypeVxlan6:
            BCMDNX_IF_ERR_EXIT(_bcm_l3_bcm_ipv6_addr_to_sand_ipv6_addr(unit, config_key->dip6, &(term_key->dip6.ipv6_address)));
            term_key->dip_prefix_len = _SHR_L3_IP6_MAX_NETLEN;
            term_key->flags |= SOC_PPC_RIF_IP_TERM_FLAG_IPV6 | SOC_PPC_RIF_IP_TERM_FLAG_KEY_ONLY; 
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Invalid config_key"))); 
            break;
        }

        /* key includes next protocol */
        switch (config_key->type) {
        case bcmTunnelTypeIp4In6:
            term_key->ipv4_next_protocol_prefix_len = 8;
            term_key->ipv4_next_protocol = SOC_SAND_PP_IP_PROTOCOL_IP_in_IP; /* IPV4 */
            break;
        case bcmTunnelTypeIp6In6:
            term_key->ipv4_next_protocol_prefix_len = 8;
            term_key->ipv4_next_protocol = SOC_SAND_PP_IP_PROTOCOL_IPv6; /* IPV6 */
            break;
        case bcmTunnelTypeGre4In6:
        case bcmTunnelTypeGre6In6:
        case bcmTunnelTypeGreAnyIn6:
            term_key->ipv4_next_protocol_prefix_len = 8;
            term_key->ipv4_next_protocol = SOC_SAND_PP_IP_PROTOCOL_GRE; /* GRE */
            break;
        case bcmTunnelTypeUdp:
            term_key->ipv4_next_protocol_prefix_len = 8;
            term_key->ipv4_next_protocol = SOC_SAND_PP_IP_PROTOCOL_UDP; /* UDP */
            break;
        case bcmTunnelTypeIpAnyIn6:
            term_key->ipv4_next_protocol_prefix_len = 0; 
            break; 
        case bcmTunnelTypeVxlan6:
            term_key->ipv4_next_protocol_prefix_len = 8;
            term_key->ipv4_next_protocol = SOC_SAND_PP_IP_PROTOCOL_UDP; /* UDP */
            break;
        default:
            term_key->ipv4_next_protocol_prefix_len = 0;
            break;
        }

       
        /* Key include GRE.ethertype, is IP GRE tunnel */
        switch (config_key->type) {
        case bcmTunnelTypeGre4In6:
            term_key->qualifier_next_protocol_prefix_len = 4;
            term_key->qualifier_next_protocol = DPP_PARSER_IP_NEXT_PROTOCOL_IPv4; 
            break;
        case bcmTunnelTypeGre6In6:
            term_key->qualifier_next_protocol_prefix_len= 4;
            term_key->qualifier_next_protocol = DPP_PARSER_IP_NEXT_PROTOCOL_IPv6;
            break;
        case bcmTunnelTypeIp4In6:
            term_key->qualifier_next_protocol_prefix_len = 4;
            term_key->qualifier_next_protocol = DPP_PARSER_IP_NEXT_PROTOCOL_IPv4;
            break;
        case bcmTunnelTypeIp6In6:
            term_key->qualifier_next_protocol_prefix_len = 4;
            term_key->qualifier_next_protocol = DPP_PARSER_IP_NEXT_PROTOCOL_IPv6;
            break;
        case bcmTunnelTypeVxlan6:
            term_key->qualifier_next_protocol_prefix_len = 4;
            term_key->qualifier_next_protocol = 1; /* Eth */
            break;
        default:
            term_key->qualifier_next_protocol_prefix_len= 0;
            term_key->qualifier_next_protocol = 0; 
            break;
        }
    }   else {
        term_key->dip = config_key->dip; 
        term_key->dip_prefix_len = _SHR_L3_IP_MAX_NETLEN; 
        term_key->flags = SOC_PPC_RIF_IP_TERM_FLAG_KEY_ONLY; 
    }

    BCM_EXIT; 
exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_petra_tunnel_term_config_action_to_ppd(
    int unit, 
    bcm_tunnel_terminator_config_action_t *config_action, 
    SOC_PPC_RIF_IP_TERM_INFO    *term_info) {

    BCMDNX_INIT_FUNC_DEFS;

    term_info->ext_lif_id = config_action->tunnel_class; 

    BCM_EXIT; 
exit:
    BCMDNX_FUNC_RETURN;
}





/* Add tunnel terminator config entries */ 
int bcm_petra_tunnel_terminator_config_add( 
    int unit, 
    uint32 flags, 
    bcm_tunnel_terminator_config_key_t *config_key, 
    bcm_tunnel_terminator_config_action_t *config_action) {

    SOC_PPC_RIF_IP_TERM_KEY term_key;
    SOC_PPC_RIF_IP_TERM_INFO term_info;
    SOC_SAND_SUCCESS_FAILURE success;


    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPC_RIF_IP_TERM_KEY_clear(&term_key); 
    SOC_PPC_RIF_IP_TERM_INFO_clear(&term_info); 

    BCMDNX_IF_ERR_EXIT(
       _bcm_petra_tunnel_terminator_config_add_verify(unit, flags, config_key, config_action)); 

    /* map BCM key to PPD key */
    BCMDNX_IF_ERR_EXIT(
       _bcm_petra_tunnel_term_config_key_to_ppd(unit, config_key, &term_key)); 

    /* map BCM info to PPD info */
    BCMDNX_IF_ERR_EXIT(
       _bcm_petra_tunnel_term_config_action_to_ppd(unit, config_action, &term_info)); 

    /* insert in ISEM */
    BCM_SAND_IF_ERR_EXIT(
       soc_ppd_rif_ip_tunnel_term_add(unit, SOC_PPC_RIF_IP_TUNNEL_TERM_ADD_LIF_ID_IS_MY_VTEP_INDEX, &term_key, config_action->tunnel_class,&term_info,  NULL,&success));
    SOC_SAND_IF_FAIL_RETURN(success);

exit:
    BCMDNX_FUNC_RETURN;

}

/* Get tunnel terminator config entries. */
int bcm_petra_tunnel_terminator_config_get( 
    int unit, 
    bcm_tunnel_terminator_config_key_t *config_key, 
    bcm_tunnel_terminator_config_action_t *config_action) {

    SOC_PPC_RIF_IP_TERM_KEY term_key;
    SOC_PPC_RIF_IP_TERM_INFO term_info;
    uint8 found; 
    SOC_PPC_LIF_ID lif_index = 0;
    SOC_PPC_RIF_INFO temp_rif_info;


    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPC_RIF_IP_TERM_KEY_clear(&term_key);
    SOC_PPC_RIF_IP_TERM_INFO_clear(&term_info); 

    BCMDNX_IF_ERR_EXIT(
       _bcm_petra_tunnel_terminator_config_key_verify(unit, config_key));

    /* map BCM key to PPD key */
    BCMDNX_IF_ERR_EXIT(
       _bcm_petra_tunnel_term_config_key_to_ppd(unit, config_key, &term_key)); 

    /* get ISEM result */
    BCM_SAND_IF_ERR_EXIT(
       soc_ppd_rif_ip_tunnel_term_get(unit, &term_key, &lif_index,&term_info,  &temp_rif_info,&found));
    if (found) {
        config_action->tunnel_class = term_info.ext_lif_id;
    } else {
        BCMDNX_IF_ERR_EXIT(BCM_E_NOT_FOUND);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* Delete tunnel terminator config entries */ 
int bcm_petra_tunnel_terminator_config_delete( 
    int unit, 
    bcm_tunnel_terminator_config_key_t *config_key){

    SOC_PPC_RIF_IP_TERM_KEY term_key;
    SOC_PPC_RIF_IP_TERM_INFO term_info;
    uint8 found; 
    SOC_PPC_LIF_ID lif_index = 0;
    SOC_PPC_RIF_INFO temp_rif_info;
    SOC_SAND_OUT SOC_PPC_LIF_ID tunnel_class; 

    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPC_RIF_IP_TERM_KEY_clear(&term_key);
    SOC_PPC_RIF_IP_TERM_INFO_clear(&term_info); 

    BCMDNX_IF_ERR_EXIT(
        _bcm_petra_tunnel_terminator_config_key_verify(unit, config_key));

    /* map BCM key to PPD key */
    BCMDNX_IF_ERR_EXIT(
        _bcm_petra_tunnel_term_config_key_to_ppd(unit, config_key, &term_key)); 

    /* check if DIP entry exist */
    BCM_SAND_IF_ERR_EXIT(
       soc_ppd_rif_ip_tunnel_term_get(unit, &term_key, &lif_index,&term_info,  &temp_rif_info,&found));

    if (found) {

        /* delete ISEM entry */
        BCM_SAND_IF_ERR_EXIT(soc_ppd_rif_ip_tunnel_term_remove(unit, &term_key, &tunnel_class));

    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Invalid config_key")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}
