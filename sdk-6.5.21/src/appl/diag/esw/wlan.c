/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * WLAN CLI commands
 */

#include <shared/bsl.h>

#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <shared/bsl.h>
#include <soc/debug.h>
#if defined(BCM_TRIUMPH2_SUPPORT)
#include <soc/triumph2.h>
#endif /* BCM_TRIUMPH2_SUPPORT */

#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/l3.h>
#include <bcm/wlan.h>
#include <bcm/l2.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/port.h>

#ifdef INCLUDE_L3
#if defined(BCM_TRIUMPH2_SUPPORT)

#define ENV_WLAN_TYPE_ID "wlan_port_id"
#define ENV_MCAST_TYPE_ID "mcast_type_id"


enum _wlan_tr2_cmd_t {
    WLAN_INIT = 1,
    WLAN_DETACH,
    WLAN_PORT_ADD,
    WLAN_PORT_DEL,
    WLAN_PORT_DEL_ALL,
    WLAN_PORT_GET,
    WLAN_PORT_GET_ALL,
    WLAN_CLIENT_ADD,
    WLAN_CLIENT_DEL,
    WLAN_CLIENT_DEL_ALL,
    WLAN_CLIENT_GET,
    WLAN_CLIENT_GET_ALL,
    WLAN_TUNNEL_INIT_CREATE,
    WLAN_TUNNEL_INIT_DESTROY,
    WLAN_TUNNEL_INIT_GET,
    WLAN_MCAST_GROUP_CREATE,
    WLAN_MCAST_GROUP_DEL,
    WLAN_MCAST_GROUP_PORT_ADD,
    WLAN_MCAST_GROUP_PORT_DEL,
    WLAN_MCAST_GROUP_ADDR,
    WLAN_OPTIONS
};

#define BCM_WLAN_PORT                   1
#define BCM_WLAN_CLIENT                 2
#define BCM_WLAN_TUNNEL                 3
#define MAX_FLAG_LENGTH                35 

#define WLAN_ROE(op, arg)     \
    do {               \
        int __rv__;    \
        __rv__ = op arg;    \
        if (BCM_FAILURE(__rv__)) { \
            cli_out("WLAN_CLI: Error: " #op " failed, %s\n", \
                    bcm_errmsg(__rv__)); \
            return CMD_FAIL; \
        } \
    } while(0)
#define WLAN_CMD(_s)                         \
    if ((cmd = ARG_CUR(a)) == NULL) {        \
        ARG_PREV(a);  \
        cli_out("WLAN_CLI: Error: Missing arg after %s\n", ARG_CUR(a)); \
        return CMD_USAGE;                    \
    } else if(sal_strcasecmp(cmd, _s) == 0)

#define PT_ADD(_pt, _id, _typ, _arg) \
    parse_table_add(&_pt, (_id), PQ_DFL | (_typ), \
                    (void *)(_arg), (void*)(_arg), 0)

#define PT_INIT(_u, _pt) parse_table_init(_u, &_pt); \

#define PT_DONE(_pt) parse_arg_eq_done(&pt);

#define PT_PARSE(_a, _pt) \
    do {     \
        if (parse_arg_eq(_a, &_pt) < 0) { \
            cli_out("WLAN_CLI: Error: Invalid option or expression: %s\n", \
                    ARG_CUR(_a)); \
            PT_DONE(_pt); \
            return CMD_USAGE; \
        } \
    } while(0)

typedef struct _bcm_tr2_wlan_flag_s {
    char      *name;
    uint32     val; 
} _bcm_tr2_wlan_flag_t;

STATIC _bcm_tr2_wlan_flag_t wlan_port_flags[] = {
    {"RePLace", BCM_WLAN_PORT_REPLACE},
    {"WithID", BCM_WLAN_PORT_WITH_ID},
    {"DRop", BCM_WLAN_PORT_DROP},
    {"Network", BCM_WLAN_PORT_NETWORK},
    {"BSSid", BCM_WLAN_PORT_BSSID},
    {"BSSidRadioId",  BCM_WLAN_PORT_BSSID_RADIO},
    {"MatchTunnel", BCM_WLAN_PORT_MATCH_TUNNEL},
    {"EgressClientMultiCast",  BCM_WLAN_PORT_EGRESS_CLIENT_MULTICAST},
    {"EgressBSSid", BCM_WLAN_PORT_EGRESS_BSSID},
    {"EgressTunnelRemote", BCM_WLAN_PORT_EGRESS_TUNNEL_REMOTE},
    {NULL, 0}
};

STATIC _bcm_tr2_wlan_flag_t wlan_client_flags[] = {
    {"RePLace", BCM_WLAN_CLIENT_REPLACE},
    {"RoamedIn", BCM_WLAN_CLIENT_ROAMED_IN},
    {"RoamedOut", BCM_WLAN_CLIENT_ROAMED_OUT},
    {"Authorized", BCM_WLAN_CLIENT_AUTHORIZED},
    {"Captive", BCM_WLAN_CLIENT_CAPTIVE},
    {NULL, 0}
};

STATIC _bcm_tr2_wlan_flag_t wlan_tunnel_flags[] = {
    {"RePLace", BCM_TUNNEL_REPLACE},
    {"WithID", BCM_TUNNEL_INIT_WLAN_TUNNEL_WITH_ID},
    {"VlanTagged", BCM_TUNNEL_INIT_WLAN_VLAN_TAGGED},
    {"MTU", BCM_TUNNEL_INIT_WLAN_MTU},
    {"IDRandom", BCM_TUNNEL_INIT_IP4_ID_SET_RANDOM},
    {"IDFixed",  BCM_TUNNEL_INIT_IP4_ID_SET_FIXED},
    {"DF", BCM_TUNNEL_INIT_SET_DF},
    {"UseInnerDF",  BCM_TUNNEL_INIT_USE_INNER_DF},
    {NULL, 0}
};

int    _bcm_tr2_wlan_cli_port_add(int unit, args_t * a);
int    _bcm_tr2_wlan_cli_port_del(int unit, args_t * a);
int    _bcm_tr2_wlan_cli_port_del_all(int unit, args_t * a);
int    _bcm_tr2_wlan_cli_port_get(int unit, args_t * a);
int    _bcm_tr2_wlan_cli_port_get_all(int unit, args_t * a);
int    _bcm_tr2_wlan_cli_port_print(int unit, bcm_wlan_port_t *wport, 
                                        void *user_data);
int    _bcm_tr2_wlan_cli_client_add(int unit, args_t * a);
int    _bcm_tr2_wlan_cli_client_del(int unit, args_t * a);
int    _bcm_tr2_wlan_cli_client_del_all(int unit, args_t * a);
int    _bcm_tr2_wlan_cli_client_get(int unit, args_t * a);
int    _bcm_tr2_wlan_cli_client_get_all(int unit, args_t * a);
int    _bcm_tr2_wlan_cli_client_print(int unit, bcm_wlan_client_t *client, 
                                      void *user_data);
int    _bcm_tr2_wlan_tunnel_init_create(int unit, args_t *a);
int    _bcm_tr2_wlan_tunnel_init_destroy(int unit, args_t *a);
int    _bcm_tr2_wlan_tunnel_init_show(int unit, args_t *a);
void   _bcm_tr2_wlan_tunnel_init_print(int unit, bcm_tunnel_initiator_t *info);
int    _bcm_tr2_wlan_cli_mcast_group_create(int unit, args_t * a);
int    _bcm_tr2_wlan_cli_mcast_group_del(int unit, args_t * a);
int    _bcm_tr2_wlan_cli_mcast_group_port_add_del(int unit, args_t * a);
int    _bcm_tr2_wlan_cli_mcast_group_addr(int unit, args_t * a);
int    _bcm_tr2_wlan_cli_print_options(int unit, args_t * a);
int    _bcm_tr2_wlan_cli_action(int unit, args_t * a);
int    _bcm_tr2_wlan_cli_parse_flags(char *elo_s, int type);


char       if_tr2_wlan_usage[] = "Only for XGS4 devices.\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "  wlan <option> [args...]\n"
#else
    "  wlan init\n\t"
    "        - Init WLAN software system.\n\t"
    "  wlan detach\n\t"
    "        - Detach WLAN software system.\n\t"
    "  wlan port add Port=<Wlan_port> PhysPort=<port>\n\t"
    "                FLaGs=<flags> IntfCLass=<class>\n\t"
    "                BSSid<bssid> RadioID=<radio_id>\n\t"
    "                MatchTunnel=<tunnel_id> EgressTunnel=<tunnel_id>\n\t"
    "                EgressClientMultiCast=<client_mc_bitmap>\n\t"
    "        - Add a WLAN port.\n\t"
    "  wlan port delete Port=<wlan_port_id>\n\t"
    "        - Delete a WLAN port.\n\t"
    "  wlan port clear\n\t"
    "        - Delete all WLAN ports.\n\t"
    "  wlan port get Port=<wlan_port_id>\n\t"
    "        - Display a WLAN port information.\n\t"
    "  wlan port show\n\t"
    "        - Traverse all WLAN ports.\n\t"
    "  wlan client add FLaGs=<flags> WlanPort=<Wlan_port_id>\n\t"
    "                MAC=<client_mac_addr> HomeAgent=<home_agent_gport> \n\t"
    "                WTP=<access_point_gport>\n\t"
    "        - Add a WLAN client.\n\t"
    "  wlan client delete MAC=<client_mac>\n\t"
    "        - Delete a WLAN client.\n\t"
    "  wlan client clear\n\t"
    "        - Delete all WLAN clients.\n\t"
    "  wlan client get MAC=<client_mac>\n\t"
    "        - Display a WLAN client information.\n\t"
    "  wlan client show\n\t"
    "        - Traverse all WLAN clients.\n\t"
    "  wlan mcast group create [l2|wlan] McastGroup=<mcast_group>\n\t"
    "        - Create a multicast group.\n\t"
    "  wlan mcast group delete McastGroup=<mcast_group>\n\t"
    "        - Delete a multicast group.\n\t"
    "  wlan mcast group port add McastGroup=<mcast_type_id>\n\t"
    "                            WlanPort=<wlan_port_id>\n\t"
    "        - Add WLAN port to multicast group.\n\t"
    "  wlan mcast group port delete  McastGroup=<mcast_type_id>\n\t"
    "                                WlanPort=<wlan_port_id>\n\t"
    "        - Delete WLAN port from multicast group\n\t"
    "  wlan mcast group addr McastGroup=<mcast_type_id>\n\t"
    "                        Mac=<mcast mac> VLAN=<vlan_id>\n\t"
    "        - Add multicast group Mac address.\n\t"
    "  wlan tunnel initiator create FLaGs=<flags> TYpe=<type> TTL=<ttl>\n\t"
    "                               DstMac=<mac> DIP=<ip> SIP=<ip>\n\t"
    "                               DIP6=<ip6> SIP6=<ip6> DSCPSel=<num>\n\t"
    "                               DSCPV=<val> DFSEL4=<num> DFSEL6=<num>\n\t"
    "                               DSCPMap=<mapID> TunnelID=<tid>\n\t"
    "                               L4DstPort=<port> L4SrcPort<port>\n\t"
    "                               SrcMac=<mac> MTU=<mtu> VID=<vid>\n\t"
    "                               TPID=<tpid> PktPri=<pri> PktCfi=<cfi>\n\t"
    "                               Ipv4ID=<val>\n\t"
    "        - Create a WLAN (CAPWAP) tunnel initiator.\n\t"
    "  wlan tunnel initiator destroy TunnelID=<tid>\n\t"
    "        - Destroy a WLAN (CAPWAP) tunnel initiator.\n\t"
    "  wlan tunnel initiator get TunnelID=<tid>\n\t"
    "        - Get a WLAN (CAPWAP) tunnel initiator.\n\t"
    "  wlan options Type=[PORT|CLIENT|TUNNEL]\n\t"
    "        - List various options/flags used in wlan commands.\n\t" "\n"
#endif
          ;

cmd_result_t
if_tr2_wlan(int unit, args_t * a)
{
    int        action = 0;
    cmd_result_t cmd_rv = CMD_OK;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if (ARG_CUR(a) == NULL) {
        return CMD_USAGE;
    }

    action = _bcm_tr2_wlan_cli_action(unit, a);

    switch (action) {
        case WLAN_INIT:
            WLAN_ROE(bcm_wlan_init, (unit));
            break;
        case WLAN_DETACH:
            WLAN_ROE(bcm_wlan_detach, (unit));
            break;
        case WLAN_PORT_ADD:
            cmd_rv = _bcm_tr2_wlan_cli_port_add(unit, a);
            break;
        case WLAN_PORT_DEL:
            cmd_rv = _bcm_tr2_wlan_cli_port_del(unit, a);
            break;
        case WLAN_PORT_DEL_ALL:
            cmd_rv = _bcm_tr2_wlan_cli_port_del_all(unit, a);
            break;
        case WLAN_PORT_GET:
            cmd_rv = _bcm_tr2_wlan_cli_port_get(unit, a);
            break;
        case WLAN_PORT_GET_ALL:
            cmd_rv = _bcm_tr2_wlan_cli_port_get_all(unit, a);
            break;
        case WLAN_CLIENT_ADD:
            cmd_rv = _bcm_tr2_wlan_cli_client_add(unit, a);
            break;
        case WLAN_CLIENT_DEL:
            cmd_rv = _bcm_tr2_wlan_cli_client_del(unit, a);
            break;
        case WLAN_CLIENT_DEL_ALL:
            cmd_rv = _bcm_tr2_wlan_cli_client_del_all(unit, a);
            break;
        case WLAN_CLIENT_GET:
            cmd_rv = _bcm_tr2_wlan_cli_client_get(unit, a);
            break;
        case WLAN_CLIENT_GET_ALL:
            cmd_rv = _bcm_tr2_wlan_cli_client_get_all(unit, a);
            break;
        case WLAN_TUNNEL_INIT_CREATE:
            cmd_rv = _bcm_tr2_wlan_tunnel_init_create(unit, a);
            break;
        case WLAN_TUNNEL_INIT_DESTROY:
            cmd_rv = _bcm_tr2_wlan_tunnel_init_destroy(unit, a);
            break;
        case WLAN_TUNNEL_INIT_GET:
            cmd_rv = _bcm_tr2_wlan_tunnel_init_show(unit, a);
            break;
        case WLAN_MCAST_GROUP_CREATE:
            cmd_rv = _bcm_tr2_wlan_cli_mcast_group_create(unit, a);
            break;
        case WLAN_MCAST_GROUP_DEL:
            cmd_rv = _bcm_tr2_wlan_cli_mcast_group_del(unit, a);
            break;
        case WLAN_MCAST_GROUP_PORT_ADD:
        case WLAN_MCAST_GROUP_PORT_DEL:
            cmd_rv = _bcm_tr2_wlan_cli_mcast_group_port_add_del(unit, a);
            break;
        case WLAN_MCAST_GROUP_ADDR:
            cmd_rv = _bcm_tr2_wlan_cli_mcast_group_addr(unit, a);
            break;
        case WLAN_OPTIONS:
            cmd_rv = _bcm_tr2_wlan_cli_print_options(unit, a);
            break;
        default:
            cli_out("Error:Unknown WLAN command %d\n", action);
            return CMD_USAGE;
    }

    ARG_DISCARD(a);
    return cmd_rv;
}

int
_bcm_tr2_wlan_cli_port_add(int unit, args_t * a)
{
    parse_table_t pt;
    bcm_port_t port = -1;
    bcm_port_t phys_port = -1;
    int        icl = 0;
    int        rid = 0;
    char      *ifl_s = 0;
    uint32     ifl = 0;
    int        wcmc = 0;
    bcm_wlan_port_t wport;
    bcm_mac_t  bssid;
    bcm_gport_t mtunnel = BCM_GPORT_INVALID;
    bcm_gport_t etunnel = BCM_GPORT_INVALID;
    bcm_gport_t gp = BCM_GPORT_INVALID; 
   
    sal_memset(bssid, 0, sizeof(bcm_mac_t));
    ARG_NEXT(a);
    PT_INIT(unit, pt);
    PT_ADD(pt, "FLaGs", PQ_STRING, &ifl_s);
    PT_ADD(pt, "Port", PQ_PORT, &port);
    PT_ADD(pt, "PhysPort", PQ_INT, &phys_port);
    PT_ADD(pt, "IntfCLass", PQ_INT, &icl);
    PT_ADD(pt, "BSSid", PQ_MAC, bssid);
    PT_ADD(pt, "RadioID", PQ_INT, &rid);
    PT_ADD(pt, "MatchTunnel", PQ_INT, &mtunnel);
    PT_ADD(pt, "EgressTunnel", PQ_INT, &etunnel);
    PT_ADD(pt, "EgressClientMultiCast", PQ_INT, &wcmc);

    PT_PARSE(a, pt);

    if (ifl_s) {
        ifl = _bcm_tr2_wlan_cli_parse_flags(ifl_s, BCM_WLAN_PORT);
    }
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit,
                         "Adding port %d FLaGs=%s PhysPort=%d icl=%d\n"
                         "BSSid=%02x:%02x:%02x:%02x:%02x:%02x\n"
                         "RadioID=%d MatchTunnel=%x EgressTunnel=%x\n"
                         "EgressClientMultiCast=%04x\n"),
              port, ifl_s, phys_port, icl, bssid[0], bssid[1], bssid[2],
              bssid[3], bssid[4], bssid[5], rid, mtunnel, etunnel, wcmc));

    PT_DONE(pt);
    bcm_wlan_port_t_init(&wport);
    if (ifl & (BCM_WLAN_PORT_REPLACE | BCM_WLAN_PORT_WITH_ID)) {
        BCM_GPORT_WLAN_PORT_ID_SET(wport.wlan_port_id, port);
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit,
                             "wlan port=%08x\n"), port));
    }
    WLAN_ROE(bcm_port_gport_get, (unit, phys_port, &gp));
    wport.port = gp;
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit,
                         "gport_get gp=%08x\n"), gp));
    wport.flags = ifl;
    wport.if_class = icl;
    memcpy(wport.bssid, bssid, sizeof(sal_mac_addr_t));
    wport.radio = rid;
    wport.match_tunnel = mtunnel;
    wport.egress_tunnel = etunnel;
    wport.client_multicast = wcmc;

    WLAN_ROE(bcm_wlan_port_add, (unit, &wport));
    var_set_integer(ENV_WLAN_TYPE_ID, wport.wlan_port_id, TRUE, FALSE);
    cli_out("env var added : $wlan_port_id=0x%08x\n", wport.wlan_port_id);

    return CMD_OK;
}

int
_bcm_tr2_wlan_cli_port_del(int unit, args_t * a)
{
    parse_table_t pt;
    bcm_gport_t gp = BCM_GPORT_INVALID;
    ARG_NEXT(a);

    PT_INIT(unit, pt);
    PT_ADD(pt, "Port", PQ_PORT, &gp);
    PT_PARSE(a, pt);
    PT_DONE(pt);
    WLAN_ROE(bcm_wlan_port_delete, (unit, gp));

    return CMD_OK;
}

int
_bcm_tr2_wlan_cli_port_del_all(int unit, args_t * a)
{
    ARG_NEXT(a);
    WLAN_ROE(bcm_wlan_port_delete_all, (unit));
    return CMD_OK;
}

int
_bcm_tr2_wlan_cli_port_get(int unit, args_t * a)
{
    parse_table_t pt;
    bcm_gport_t gp = BCM_GPORT_INVALID;
    bcm_wlan_port_t wport;
    ARG_NEXT(a);

    bcm_wlan_port_t_init(&wport);
    PT_INIT(unit, pt);
    PT_ADD(pt, "Port", PQ_PORT, &gp);
    PT_PARSE(a, pt);
    PT_DONE(pt);
    WLAN_ROE(bcm_wlan_port_get, (unit, gp, &wport));
    WLAN_ROE(_bcm_tr2_wlan_cli_port_print, (unit, &wport, NULL));

    return CMD_OK;
}

int
_bcm_tr2_wlan_cli_port_get_all(int unit, args_t * a)
{
    ARG_NEXT(a);
    WLAN_ROE(bcm_wlan_port_traverse,(unit, _bcm_tr2_wlan_cli_port_print, NULL));
    return CMD_OK;
}

int
_bcm_tr2_wlan_cli_client_add(int unit, args_t * a)
{
    parse_table_t pt;
    bcm_mac_t  mac;
    bcm_wlan_client_t client;
    bcm_gport_t ha = BCM_GPORT_INVALID;
    bcm_gport_t wtp = BCM_GPORT_INVALID;
    bcm_gport_t wlan_port = BCM_GPORT_INVALID; 
    char      *cfl_s = 0;
    uint32     cfl = 0;   

    sal_memset(mac, 0, sizeof(bcm_mac_t));
    ARG_NEXT(a);
    PT_INIT(unit, pt);
    PT_ADD(pt, "FLaGs", PQ_STRING, &cfl_s);
    PT_ADD(pt, "WlanPort", PQ_INT, &wlan_port);
    PT_ADD(pt, "MAC", PQ_MAC, mac);
    PT_ADD(pt, "HomeAgent", PQ_INT, &ha);
    PT_ADD(pt, "WTP", PQ_INT, &wtp);

    PT_PARSE(a, pt);

    if (cfl_s) {
        cfl = _bcm_tr2_wlan_cli_parse_flags(cfl_s, BCM_WLAN_CLIENT);
    }
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit,
                         "Adding client FLaGs=%s WlanPort=%d\n"
                         "MAC=%02x:%02x:%02x:%02x:%02x:%02x\n"
                         "HomeAgent=%x WTP=%x\n"),
              cfl_s, wlan_port, mac[0], mac[1], mac[2],
              mac[3], mac[4], mac[5], ha, wtp));

    bcm_wlan_client_t_init(&client);
    client.flags = cfl;
    memcpy(client.mac, mac, sizeof(sal_mac_addr_t));
    client.home_agent = ha;
    client.wtp = wtp;

    PT_DONE(pt);
    WLAN_ROE(bcm_wlan_client_add, (unit, &client));

    return CMD_OK;
}

int
_bcm_tr2_wlan_cli_client_del(int unit, args_t * a)
{
    parse_table_t pt;
    bcm_mac_t mac;
    bcm_wlan_client_t client;
    ARG_NEXT(a);

    bcm_wlan_client_t_init(&client);
    sal_memset(mac, 0, sizeof(bcm_mac_t));
    PT_INIT(unit, pt);
    PT_ADD(pt, "MAC", PQ_MAC, mac);
    PT_PARSE(a, pt);
    PT_DONE(pt);
    WLAN_ROE(bcm_wlan_client_delete, (unit, mac));

    return CMD_OK;
}

int
_bcm_tr2_wlan_cli_client_del_all(int unit, args_t * a)
{
    ARG_NEXT(a);
    WLAN_ROE(bcm_wlan_client_delete_all, (unit));
    return CMD_OK;
}

int
_bcm_tr2_wlan_cli_client_get(int unit, args_t * a)
{
    parse_table_t pt;
    bcm_mac_t mac;
    bcm_wlan_client_t client;
    ARG_NEXT(a);

    bcm_wlan_client_t_init(&client);
    sal_memset(mac, 0, sizeof(bcm_mac_t));
    PT_INIT(unit, pt);
    PT_ADD(pt, "MAC", PQ_MAC, mac);
    PT_PARSE(a, pt);
    PT_DONE(pt);
    WLAN_ROE(bcm_wlan_client_get, (unit, mac, &client));
    WLAN_ROE(_bcm_tr2_wlan_cli_client_print, (unit, &client, NULL));

    return CMD_OK;
}

int
_bcm_tr2_wlan_cli_client_get_all(int unit, args_t * a)
{
    ARG_NEXT(a);
    WLAN_ROE(bcm_wlan_client_traverse, (unit, _bcm_tr2_wlan_cli_client_print, 
                                        NULL));
    return CMD_OK;
}

int
_bcm_tr2_wlan_tunnel_init_create(int unit, args_t *a) 
{
    bcm_tunnel_initiator_t tunnel_init;
    bcm_tunnel_type_t type = 0;
    parse_table_t     pt;     
    bcm_ip6_t         sip6_addr;
    bcm_ip6_t         ip6_addr;
    bcm_mac_t         dst_mac, src_mac;
    bcm_ip_t          sip_addr = 0;
    bcm_ip_t          ip_addr = 0;
    int               ip4_df_sel = 0;
    int               ip6_df_sel = 0;
    int               dscp_val = 0;
    int               dscp_sel = 0;
    int               dscp_map = 0;
    int               ttl = 0;
    int               rv;
    int               tid;
    uint32            tfl = 0;
    char              *tfl_s = 0;
    int               tpid = 0x8100;
    int               vid = BCM_VLAN_INVALID;
    int               mtu = 0;
    int               src_port = 0;
    int               dst_port = 0;
    int               pkt_pri = 0;
    int               pkt_cfi = 0;
    int               ip4_id = 0;

    /* Stack variables initialization. */
    sal_memset(sip6_addr, 0, sizeof(bcm_ip6_t));
    sal_memset(ip6_addr, 0, sizeof(bcm_ip6_t));
    sal_memset(dst_mac, 0, sizeof(bcm_mac_t));
    sal_memset(src_mac, 0, sizeof(bcm_mac_t));

    ARG_NEXT(a);
    PT_INIT(unit, pt);
    PT_ADD(pt, "FLaGs", PQ_STRING, &tfl_s);
    PT_ADD(pt, "TYpe", PQ_INT, &type);
    PT_ADD(pt, "TTL",  PQ_INT, &ttl);
    PT_ADD(pt, "DstMac",  PQ_MAC, dst_mac);
    PT_ADD(pt, "DIP",  PQ_IP, &ip_addr);
    PT_ADD(pt, "SIP",  PQ_IP, &sip_addr);
    PT_ADD(pt, "DIP6", PQ_IP6, &ip6_addr);
    PT_ADD(pt, "SIP6", PQ_IP6, &sip6_addr);
    PT_ADD(pt, "DSCPSel", PQ_INT, &dscp_sel);
    PT_ADD(pt, "DSCPV", PQ_INT, &dscp_val);
    PT_ADD(pt, "DFSEL4", PQ_INT, &ip4_df_sel);
    PT_ADD(pt, "DFSEL6", PQ_BOOL, &ip6_df_sel);
    PT_ADD(pt, "DSCPMap", PQ_INT, &dscp_map);
    PT_ADD(pt, "TunnelID", PQ_INT, &tid);
    PT_ADD(pt, "L4DstPort", PQ_INT, &dst_port);
    PT_ADD(pt, "L4SrcPort", PQ_INT, &src_port);
    PT_ADD(pt, "SrcMac",  PQ_MAC, src_mac);
    PT_ADD(pt, "MTU", PQ_INT, &mtu);
    PT_ADD(pt, "VID", PQ_INT, &vid);
    PT_ADD(pt, "TPID", PQ_INT, &tpid);
    PT_ADD(pt, "PktPri", PQ_INT, &pkt_pri);
    PT_ADD(pt, "PktCfi", PQ_INT, &pkt_cfi);
    PT_ADD(pt, "Ipv4ID", PQ_INT, &ip4_id);

    PT_PARSE(a, pt);

    if (tfl_s) {
        tfl = _bcm_tr2_wlan_cli_parse_flags(tfl_s, BCM_WLAN_TUNNEL);
    }
    PT_DONE(pt);

    /* Initialize data structures. */
    bcm_tunnel_initiator_t_init(&tunnel_init);

    /* Fill tunnel info. */
    tunnel_init.flags = tfl;
    tunnel_init.type = type;
    tunnel_init.ttl = ttl;

    sal_memcpy(tunnel_init.dmac, dst_mac, sizeof(dst_mac));
    sal_memcpy(tunnel_init.smac, src_mac, sizeof(src_mac));

    if (ip4_df_sel) { 
        tunnel_init.flags |= (ip4_df_sel > 1) ?
            BCM_TUNNEL_INIT_USE_INNER_DF : BCM_TUNNEL_INIT_IPV4_SET_DF;
    }
    if (ip6_df_sel) { 
        tunnel_init.flags |= BCM_TUNNEL_INIT_IPV6_SET_DF;
    }

    sal_memcpy(tunnel_init.dip6, ip6_addr, sizeof(bcm_ip6_t));
    sal_memcpy(tunnel_init.sip6, sip6_addr, sizeof(bcm_ip6_t));
    tunnel_init.dip = ip_addr;
    tunnel_init.sip = sip_addr;
    tunnel_init.dscp_sel = dscp_sel;
    tunnel_init.dscp = dscp_val;
    tunnel_init.dscp_map = dscp_map;
    if (tfl & BCM_TUNNEL_INIT_WLAN_MTU) {
        tunnel_init.mtu = mtu;
    }
    tunnel_init.vlan = vid;
    tunnel_init.tpid = tpid;
    tunnel_init.pkt_pri = pkt_pri;
    tunnel_init.pkt_cfi = pkt_cfi;
    tunnel_init.ip4_id = ip4_id;

    if ((rv = bcm_wlan_tunnel_initiator_create(unit, &tunnel_init)) < 0) {
        cli_out("ERROR %s: creating tunnel initiator %s\n",
                ARG_CMD(a), bcm_errmsg(rv));
        return CMD_FAIL;
    }
    return CMD_OK;
}

int
_bcm_tr2_wlan_tunnel_init_destroy(int unit, args_t *a) 
{
    bcm_gport_t            tunnel_id = BCM_GPORT_INVALID;
    parse_table_t          pt;     
    int                    rv;

    PT_INIT(unit, pt);
    PT_ADD(pt, "TunnelID", PQ_INT, &tunnel_id);

    PT_PARSE(a, pt);
    PT_DONE(pt);

    if ((rv = bcm_wlan_tunnel_initiator_destroy(unit, tunnel_id)) < 0) {
        cli_out("ERROR %s: destroying tunnel initiator for %d %s\n",
                ARG_CMD(a), tunnel_id, bcm_errmsg(rv));
        return CMD_FAIL;
    }
    return CMD_OK;
}

int
_bcm_tr2_wlan_tunnel_init_show(int unit, args_t *a) 
{
    bcm_tunnel_initiator_t tunnel_init;
    parse_table_t          pt;     
    bcm_gport_t            tunnel_id = BCM_GPORT_INVALID;
    int                    rv;

    PT_INIT(unit, pt);
    PT_ADD(pt, "TunnelID", PQ_INT, &tunnel_id);

    PT_PARSE(a, pt);
    PT_DONE(pt);

    bcm_tunnel_initiator_t_init(&tunnel_init);
    tunnel_init.tunnel_id = tunnel_id;

    if ((rv = bcm_wlan_tunnel_initiator_get(unit, &tunnel_init)) < 0) {
        cli_out("ERROR %s: getting tunnel initiator for %d %s\n",
                ARG_CMD(a), tunnel_id, bcm_errmsg(rv));
        return CMD_FAIL;
    }

    _bcm_tr2_wlan_tunnel_init_print(unit, &tunnel_init);
    return CMD_OK;
}

void
_bcm_tr2_wlan_tunnel_init_print(int unit, bcm_tunnel_initiator_t *info)
{
    char ip_str[IP6ADDR_STR_LEN + 3];
    char if_mac_str[SAL_MACADDR_STR_LEN];

    cli_out("Tunnel initiator:\n");
    cli_out("\tUnit        = %d\n", unit);
    cli_out("\tTunnel ID   = %d\n", info->tunnel_id);
    cli_out("\tTUNNEL_TYPE = %d\n", info->type);
    cli_out("\tTTL         = %d\n", info->ttl);
    format_macaddr(if_mac_str, info->smac);
    cli_out("\tSRC MAC_ADDR    = %-18s\n", if_mac_str);
    format_macaddr(if_mac_str, info->dmac);
    cli_out("\tDST MAC_ADDR    = %-18s\n", if_mac_str);
    if (_BCM_TUNNEL_OUTER_HEADER_IPV6(info->type)) { 
        format_ip6addr(ip_str, info->dip6);
        cli_out("\tDIP         = 0x%-s\n", ip_str);
        format_ip6addr(ip_str, info->sip6);
        cli_out("\tSIP         = 0x%-s\n", ip_str);
    } else { 
        cli_out("\tDIP         = 0x%08x\n", info->dip);
        cli_out("\tSIP         = 0x%08x\n", info->sip);
        if (info->flags & BCM_TUNNEL_INIT_USE_INNER_DF) {
            cli_out("\tCopy DF from inner header.\n");
        } else if (info->flags & BCM_TUNNEL_INIT_IPV4_SET_DF) {
            cli_out("\tForce DF to 1 for ipv4 payload.\n");
        }
        if (info->flags & BCM_TUNNEL_INIT_IPV6_SET_DF) {
            cli_out("\tForce DF to 1 for ipv6 payload.\n");
        }
    }
    cli_out("\tDSCP_SEL    = 0x%x\n", info->dscp_sel);
    cli_out("\tDSCP        = 0x%x\n", info->dscp);
    cli_out("\tDSCP_MAP    = 0x%x\n", info->dscp_map);
    return;
}

int
_bcm_tr2_wlan_cli_mcast_group_create(int unit, args_t * a)
{
    int        flags = -1;
    int        mcast_group = BCM_VLAN_INVALID;
    char      *cmd;
    parse_table_t pt;

    ARG_NEXT(a);                /* wlan,l2 etc. */
    WLAN_CMD("l2") {
        flags = BCM_MULTICAST_TYPE_L2;
    }
    WLAN_CMD("wlan") {
        flags = BCM_MULTICAST_TYPE_WLAN;
    }
    ARG_NEXT(a);
    PT_INIT(unit, pt);
    PT_ADD(pt, "McastGroup", PQ_INT, &mcast_group);
    PT_PARSE(a, pt);
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit,
                         "mcast group %d\n"), mcast_group));
    flags |= BCM_MULTICAST_WITH_ID;
    PT_DONE(pt);
    WLAN_ROE(bcm_multicast_create, (unit, flags, &mcast_group));
    var_set_integer(ENV_MCAST_TYPE_ID, mcast_group, TRUE, FALSE);
    cli_out("env var added : $mcast_type_id=0x%08x\n", mcast_group);

    return CMD_OK;
}

int
_bcm_tr2_wlan_cli_mcast_group_del(int unit, args_t * a)
{
    int        mcast_group = BCM_VLAN_INVALID;
    parse_table_t pt;

    ARG_NEXT(a);
    PT_INIT(unit, pt);
    PT_ADD(pt, "McastGroup", PQ_INT, &mcast_group);
    PT_PARSE(a, pt);
    PT_DONE(pt);
    WLAN_ROE(bcm_multicast_destroy, (unit, mcast_group));
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit,
                         "mcast group %d destroyed\n"), mcast_group));

    return CMD_OK;
}

int
_bcm_tr2_wlan_cli_mcast_group_port_add_del(int unit, args_t * a)
{
    int        add = 0;
    int        mcast_group = BCM_VLAN_INVALID;
    bcm_module_t modid;
    bcm_port_t port;
    bcm_trunk_t trunk_id;
    int        id;
    bcm_gport_t gp = BCM_GPORT_INVALID;
    bcm_gport_t wlan_port;
    bcm_if_t   encap_id;
    parse_table_t pt;
    char      *cmd;

    ARG_NEXT(a);
    WLAN_CMD("add") {
        add = 1;
    }
    WLAN_CMD("delete") {
        add = 0;
    }
    ARG_NEXT(a);
    PT_INIT(unit, pt);
    PT_ADD(pt, "McastGroup", PQ_INT, &mcast_group);
    PT_ADD(pt, "WlanPort", PQ_INT, &wlan_port);
    PT_PARSE(a, pt);
    PT_DONE(pt);
    WLAN_ROE(_bcm_esw_gport_resolve,
             (unit, wlan_port, &modid, &port, &trunk_id, &id));
    WLAN_ROE(bcm_port_gport_get, (unit, port, &gp));
    WLAN_ROE(bcm_multicast_wlan_encap_get,
             (unit, mcast_group, gp, wlan_port, &encap_id));
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit,
                         "WLAN_CLI: wlan mcast group port %s wlan"
                         "group 0x%x gport 0x%0x wlan_port 0x%08xnh %d\n"),
              add ? "add" : "delete", mcast_group, gp, wlan_port,
              encap_id));
    if (add) {
        WLAN_ROE(bcm_multicast_egress_add,
                 (unit, mcast_group, gp, encap_id));
    } else {
        WLAN_ROE(bcm_multicast_egress_delete,
                 (unit, mcast_group, gp, encap_id));
    }

    return CMD_OK;
}

int
_bcm_tr2_wlan_cli_mcast_group_addr(int unit, args_t * a)
{
    int        mcast_group = BCM_VLAN_INVALID;
    int        vlan;
    bcm_mac_t  mac;
    bcm_l2_addr_t l2_addr;
    parse_table_t pt;

    ARG_NEXT(a);
    PT_INIT(unit, pt);
    PT_ADD(pt, "McastGroup", PQ_INT, &mcast_group);
    PT_ADD(pt, "Mac", PQ_MAC, mac);
    PT_ADD(pt, "VLAN", PQ_INT, &vlan);
    PT_PARSE(a, pt);

    bcm_l2_addr_t_init(&l2_addr, mac, vlan);
    l2_addr.flags = BCM_L2_STATIC | BCM_L2_MCAST;
    l2_addr.l2mc_group = mcast_group;
    PT_DONE(pt);
    WLAN_ROE(bcm_l2_addr_add, (unit, &l2_addr));
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit,
                         "mcast l2 addr add=%02x:%02x:%02x:%02x:%02x:%02x \n"),
              mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]));
    cli_out("WLAN_CLI: mc_group 0x%08x vlan 0x%08x\n", mcast_group, vlan);
    PT_DONE(pt);

    return CMD_OK;
}

int        
_bcm_tr2_wlan_cli_print_options(int unit, args_t * a)
{
    int i;
    char     *type_s = 0;
    parse_table_t pt;
    _bcm_tr2_wlan_flag_t *flags = 0;

    ARG_NEXT(a);
    PT_INIT(unit, pt);
    PT_ADD(pt, "Type", PQ_STRING, &type_s);
    PT_PARSE(a, pt);

    if (parse_cmp("CLIENT", type_s, '\0')) {
        flags = wlan_client_flags;
    } else if (parse_cmp(type_s, "PORT", '\0')) {
        flags = wlan_port_flags;
    } else if (parse_cmp(type_s, "TUNNEL", '\0')) {
        flags = wlan_tunnel_flags;
    }
    PT_DONE(pt);

    if (flags) {
        for (i = 0; flags[i].name != NULL; i++) {
            cli_out("\t%-25s  0x%08x\n", flags[i].name, flags[i].val);
        }
    } else {
        cli_out("Port Flags:\n\t");
        for (i = 0; wlan_port_flags[i].name != NULL; i++) {
            cli_out("%-35s  0x%08x\n\t", wlan_port_flags[i].name, 
                    wlan_port_flags[i].val);
        }
        cli_out("\nClient Flags:\n\t");
        for (i = 0; wlan_client_flags[i].name != NULL; i++) {
            cli_out("%-35s  0x%08x\n\t", wlan_client_flags[i].name, 
                    wlan_client_flags[i].val);
        }
        cli_out("\nTunnel Initiator Flags:\n\t");
        for (i = 0; wlan_tunnel_flags[i].name != NULL; i++) {
            cli_out("%-35s  0x%08x\n\t", wlan_tunnel_flags[i].name, 
                    wlan_tunnel_flags[i].val);
        }
        cli_out("\n");
    }

    return CMD_OK;
}

int
_bcm_tr2_wlan_cli_action(int unit, args_t * a)
{
    char      *cmd;
    int        action = -1;

    WLAN_CMD("init") {
        action = WLAN_INIT;
    }

    WLAN_CMD("detach") {
        action = WLAN_DETACH;
    }

    WLAN_CMD("options") {
        action = WLAN_OPTIONS;
    }

    WLAN_CMD("port") {
        ARG_NEXT(a);
        WLAN_CMD("add") {
            action = WLAN_PORT_ADD;
        }
        WLAN_CMD("delete") {
            action = WLAN_PORT_DEL;
        }
        WLAN_CMD("clear") {
            action = WLAN_PORT_DEL_ALL;
        }
        WLAN_CMD("get") {
            action = WLAN_PORT_GET;
        }
        WLAN_CMD("show") {
            action = WLAN_PORT_GET_ALL;
        }
    }

    WLAN_CMD("client") {
        ARG_NEXT(a);
        WLAN_CMD("add") {
            action = WLAN_CLIENT_ADD;
        }
        WLAN_CMD("delete") {
            action = WLAN_CLIENT_DEL;
        }
        WLAN_CMD("clear") {
            action = WLAN_CLIENT_DEL_ALL;
        }
        WLAN_CMD("get") {
            action = WLAN_CLIENT_GET;
        }
        WLAN_CMD("show") {
            action = WLAN_CLIENT_GET_ALL;
        }
    }

    WLAN_CMD("tunnel") {
        ARG_NEXT(a);
        WLAN_CMD("initiator") {
            ARG_NEXT(a);
            WLAN_CMD("create") {
                action = WLAN_TUNNEL_INIT_CREATE;
            }
            WLAN_CMD("destroy") {
                action = WLAN_TUNNEL_INIT_DESTROY;
            }
            WLAN_CMD("get") {
                action = WLAN_TUNNEL_INIT_GET;
            }
        }
    }

    WLAN_CMD("mcast") {
        ARG_NEXT(a);
        WLAN_CMD("group") {
            ARG_NEXT(a);
            WLAN_CMD("create") {
                action = WLAN_MCAST_GROUP_CREATE;
            }
            WLAN_CMD("delete") {
                action = WLAN_MCAST_GROUP_DEL;
            }
            WLAN_CMD("port") {
                action = WLAN_MCAST_GROUP_PORT_ADD;
            }
            WLAN_CMD("addr") {
                action = WLAN_MCAST_GROUP_ADDR;
            }
        }
    }

    return action;
}

int
_bcm_tr2_wlan_cli_parse_flags(char *input_s, int type)
{
    int cnt;
    int len;
    char *p;
    char f_name[MAX_FLAG_LENGTH];
    _bcm_tr2_wlan_flag_t *flags;
    uint32 result = 0;
    int done = 0;
    int found = 0;

    if (isint(input_s)) {
        result = parse_integer(input_s);
        return result;
    }

    switch (type) {
        case BCM_WLAN_PORT:
            flags = wlan_port_flags;
            break;
        case BCM_WLAN_CLIENT:
            flags = wlan_client_flags;
            break;
        case BCM_WLAN_TUNNEL:
            flags = wlan_tunnel_flags;
            break;
        default:
            flags = 0;
    } /* switch (type) */

    if(!flags) {
        cli_out("WLAN CLI: Error: Discarded unrecognized Flags\n\t %s\n", 
                input_s);
        return 0;
    }

    while (!done) {
        p = (char*)strcaseindex(input_s,",");
        sal_memset(f_name, 0, MAX_FLAG_LENGTH); 
        if (p) {
            sal_strncpy(f_name, input_s, p-input_s);
            input_s = p + 1;
        } else {
            len = (sal_strlen(input_s) >= MAX_FLAG_LENGTH) ? 
                   (MAX_FLAG_LENGTH - 1) : sal_strlen(input_s);
            sal_strncpy(f_name, input_s, len);
            done = 1;
        }
        found = 0;
        for (cnt = 0; flags[cnt].name ; cnt++) {
            if (parse_cmp(flags[cnt].name, f_name, '\0')) {
                result |= flags[cnt].val;
                found = 1;
            }
        }
        if (!found) {
            cli_out("WLAN_CLI: flag %s not recognized, discarded\n", f_name);
        }
    }
    return result;

}

int
_bcm_tr2_wlan_cli_port_print(int unit, bcm_wlan_port_t *wport, void *user_data)
{
    cli_out("\n WLAN Port ID: %x", wport->wlan_port_id);
    cli_out("\n Port: %x", wport->port);
    cli_out("\n Interface Class: %d", wport->if_class);
    cli_out("\n BSSID: %02x:%02x:%02x:%02x:%02x:%02x", wport->bssid[0], 
            wport->bssid[1], wport->bssid[2], wport->bssid[3], wport->bssid[4], 
            wport->bssid[5]);
    cli_out("\n Radio ID: %d", wport->radio);
    cli_out("\n Match Tunnel: %x", wport->match_tunnel);
    cli_out("\n Egress Tunnel: %x", wport->egress_tunnel);
    cli_out("\n Client Multicast Bitmap: %x", wport->client_multicast);
    cli_out("\n Encap ID: %d", wport->encap_id);

    return BCM_E_NONE;
}

int
_bcm_tr2_wlan_cli_client_print(int unit, bcm_wlan_client_t *client, 
                               void *user_data)
{
    cli_out("\n WLAN Port ID: %x", client->wlan_port_id);
    cli_out("\n Client MAC: %02x:%02x:%02x:%02x:%02x:%02x", client->mac[0], 
            client->mac[1], client->mac[2], client->mac[3], client->mac[4], 
            client->mac[5]);
    cli_out("\n Home Agent Port ID: %x", client->home_agent);
    cli_out("\n Access Point Port ID: %x", client->wtp);

    return BCM_E_NONE;
}

#endif /* BCM_TRIUMPH2_SUPPORT */
#endif /* INCLUDE_L3 */
