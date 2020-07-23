/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm/oam.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/oam.h>
#if defined(BCM_ENDURO_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
#include <bcm/rx.h>
#include <soc/higig.h>
#endif /* !BCM_ENDURO_SUPPORT || !BCM_ENDURO_SUPPORT*/

#define CLEAN_UP_AND_RETURN(_result) \
    parse_arg_eq_done(&parse_table); \
    return (_result);

#define _isprintable(_c) (((_c) > 32) && ((_c) < 127))

#define GROUP_LIST_HEADER \
    "ID  Name                                             TX_RD CPU_COPY AlarmPri\n" \
    "--- ------------------------------------------------ ----- -------- ---------\n"

#define ENDPOINT_LIST_HEADER \
    "ID   Grp Name L Period CLDLO VLAN Mod Port Tr MAC address       PktPri IntPri TLV\n" \
    "---- --- ---- - ------ ------ ---- --- ---- -- ----------------- ------ ------ --- \n"

static int events_on = 0;

static void print_group_faults(const char *header_string_p, uint32 faults)
{
    cli_out("%s: %s %s %s %s\n", header_string_p,
            (faults & BCM_OAM_GROUP_FAULT_REMOTE) ? "RDI" : "",
            (faults & BCM_OAM_GROUP_FAULT_CCM_TIMEOUT) ? "CCM_TIMEOUT" : "",
            (faults & BCM_OAM_GROUP_FAULT_CCM_ERROR) ? "CCM_ERROR" : "",
            (faults & BCM_OAM_GROUP_FAULT_CCM_XCON) ? "CCM_XCON" : "");
}

static int _cmd_esw_oam_group_print(int unit, bcm_oam_group_info_t *group_info_p,
    void *user_data)
{
    char name_string[BCM_OAM_GROUP_NAME_LENGTH + 1];
    int byte_index;
    char byte;

    /* Do another get to clear persistent faults */

    group_info_p->clear_persistent_faults =
        BCM_OAM_GROUP_FAULT_REMOTE |
        BCM_OAM_GROUP_FAULT_CCM_TIMEOUT |
        BCM_OAM_GROUP_FAULT_CCM_ERROR |
        BCM_OAM_GROUP_FAULT_CCM_XCON;

    bcm_oam_group_get(unit, group_info_p->id, group_info_p);

    for (byte_index = 0; byte_index < BCM_OAM_GROUP_NAME_LENGTH;
        ++byte_index)
    {
        byte = group_info_p->name[byte_index];

        name_string[byte_index] = _isprintable(byte) ? byte : '.';
    }

    name_string[byte_index] = 0;

    cli_out("%3d %48s %3c %6c %9d\n", group_info_p->id, name_string,
            (group_info_p->flags & BCM_OAM_GROUP_REMOTE_DEFECT_TX) ? '*' : ' ',
            (group_info_p->flags & BCM_OAM_GROUP_COPY_TO_CPU) ? '*' : ' ',
            group_info_p->lowest_alarm_priority);

    if (group_info_p->faults != 0)
    {
        print_group_faults("           Faults", group_info_p->faults);
    }

    if (group_info_p->persistent_faults != 0)
    {
        print_group_faults("Persistent faults", group_info_p->persistent_faults);
    }

    return BCM_E_NONE;
}

static void print_endpoint_faults(const char *header_string_p,
                                  uint32 faults, bcm_oam_endpoint_type_t type)
{
    if ((type == bcmOAMEndpointTypeBHHMPLS) ||
        (type == bcmOAMEndpointTypeBHHMPLSVccv)||
        (type == bcmOAMEndpointTypeBhhSection)||
        (type == bcmOAMEndpointTypeBhhSectionInnervlan)||
        (type == bcmOAMEndpointTypeBhhSectionOuterVlan)||
        (type == bcmOAMEndpointTypeBhhSectionOuterPlusInnerVlan)) {
        cli_out("%s: %s %s %s %s %s %s %s\n", header_string_p,
                (faults & BCM_OAM_BHH_FAULT_CCM_TIMEOUT) ? "CCM_TIMEOUT" : "",
                (faults & BCM_OAM_BHH_FAULT_CCM_RDI) ? "RDI" : "",
                (faults & BCM_OAM_BHH_FAULT_CCM_UNKNOWN_MEG_LEVEL) ? "MEG_LEVEL" : "",
                (faults & BCM_OAM_BHH_FAULT_CCM_UNKNOWN_MEG_ID) ? "MEG_ID" : "",
                (faults & BCM_OAM_BHH_FAULT_CCM_UNKNOWN_MEP_ID) ? "MEP_ID" : "",
                (faults & BCM_OAM_BHH_FAULT_CCM_UNKNOWN_PERIOD) ? "CCM_PERIOD" : "",
                (faults & BCM_OAM_BHH_FAULT_CCM_UNKNOWN_PRIORITY) ? "CCM_PRIORITY" : "");
    } else {
        cli_out("%s: %s %s %s %s\n", header_string_p,
                (faults & BCM_OAM_ENDPOINT_FAULT_CCM_TIMEOUT) ? "CCM_TIMEOUT" : "",
                (faults & BCM_OAM_ENDPOINT_FAULT_REMOTE) ? "RDI" : "",
                (faults & BCM_OAM_ENDPOINT_FAULT_PORT_DOWN) ? "PORT_DOWN" : "",
                (faults & BCM_OAM_ENDPOINT_FAULT_INTERFACE_DOWN) ? "INTERFACE_DOWN" : "");
    }
}

static int _cmd_esw_oam_endpoint_print(int unit,
    bcm_oam_endpoint_info_t *endpoint_info_p, void *user_data)
{
    int local_tx;
    int local_rx;
    int is_remote;
    bcm_module_t module_id;
    bcm_port_t port_id;
    bcm_trunk_t trunk_id;
    bcm_port_t local_id;
    char mac_address_string[MACADDR_STR_LEN];
    int rc = BCM_E_NONE;
#if defined(BCM_ENDURO_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)

    int i;
#endif
    /* Do another get to clear persistent faults */

    endpoint_info_p->clear_persistent_faults =
        BCM_OAM_ENDPOINT_FAULT_CCM_TIMEOUT |
        BCM_OAM_ENDPOINT_FAULT_REMOTE |
        BCM_OAM_ENDPOINT_FAULT_PORT_DOWN |
        BCM_OAM_ENDPOINT_FAULT_INTERFACE_DOWN;

    rc = bcm_oam_endpoint_get(unit, endpoint_info_p->id, endpoint_info_p);
    if (BCM_FAILURE(rc)) {  
        cli_out("bcm_oam_endpoint_get failure\n");
        return rc;
    }

    is_remote = endpoint_info_p->flags & BCM_OAM_ENDPOINT_REMOTE;
    local_tx = !is_remote && endpoint_info_p->ccm_period > 0;

    local_rx = !is_remote && (endpoint_info_p->flags &
        (BCM_OAM_ENDPOINT_CCM_RX | BCM_OAM_ENDPOINT_LOOPBACK |
            BCM_OAM_ENDPOINT_DELAY_MEASUREMENT | 
            BCM_OAM_ENDPOINT_LOSS_MEASUREMENT |BCM_OAM_ENDPOINT_LINKTRACE));

    cli_out("%4d %3d ", endpoint_info_p->id, endpoint_info_p->group);
    
    if (is_remote || local_tx)
    {
        cli_out("%04X ", endpoint_info_p->name);
    }
    else
    {
        cli_out("     ");
    }
   
    if ((endpoint_info_p->level >= 0) && 
        (endpoint_info_p->level <=7)) {
        cli_out("%1d ", endpoint_info_p->level);
    }
    else
    {
        cli_out("     ");
    }

    if (is_remote || local_tx)
    {
        cli_out("%6d ", endpoint_info_p->ccm_period);
    }
    else
    {
        cli_out("       ");
    }

    if (local_rx)
    {
        cli_out("%c%c%c%c%c ",
                (endpoint_info_p->flags & BCM_OAM_ENDPOINT_CCM_RX) ? '*' : ' ',
                (endpoint_info_p->flags & BCM_OAM_ENDPOINT_LOOPBACK) ? '*' : ' ',
                (endpoint_info_p->flags & BCM_OAM_ENDPOINT_DELAY_MEASUREMENT) ?
                '*' : ' ',
                (endpoint_info_p->flags & BCM_OAM_ENDPOINT_LINKTRACE) ?
                '*' : ' ',
                (endpoint_info_p->flags & BCM_OAM_ENDPOINT_LOSS_MEASUREMENT) ?
                '*' : ' ');
    }
    else
    {
        cli_out("     ");
    }

    if (endpoint_info_p->vlan != BCM_VLAN_INVALID) {
        cli_out("%4d ", endpoint_info_p->vlan);
    }
    else
    {
        cli_out("     ");
    }

    if (BCM_FAILURE(_bcm_esw_gport_resolve(unit, endpoint_info_p->gport,
        &module_id, &port_id, &trunk_id, &local_id)))
    {
        cli_out("Bad GPORT   ");
    }
    else if (BCM_GPORT_IS_LOCAL(endpoint_info_p->gport))
    {
        cli_out("    %4d    ", port_id);
    }
    else if (BCM_GPORT_IS_MODPORT(endpoint_info_p->gport))
    {
        cli_out("%3d %4d    ", module_id, port_id);
    }
    else if (BCM_GPORT_IS_TRUNK(endpoint_info_p->gport))
    {
        cli_out("         %2d ", trunk_id);
    } else if (BCM_GPORT_IS_MIM_PORT(endpoint_info_p->gport) ||
               BCM_GPORT_IS_MPLS_PORT(endpoint_info_p->gport)) {
        cli_out("0x%8x", endpoint_info_p->gport);
    }
    else
    {
        cli_out("            ");
    }

    if (local_tx)
    {
        format_macaddr(mac_address_string, endpoint_info_p->src_mac_address);

        cli_out("%17s %6d %6d %3c\n",
                mac_address_string,
                endpoint_info_p->pkt_pri,
                endpoint_info_p->int_pri,
                (endpoint_info_p->flags & (BCM_OAM_ENDPOINT_PORT_STATE_TX
                | BCM_OAM_ENDPOINT_INTERFACE_STATE_TX) ? '*' : ' '));
    }
    else
    {
        cli_out("\n");
    }

    if (endpoint_info_p->faults != 0)
    {
        print_endpoint_faults("           Faults", endpoint_info_p->faults,
                              endpoint_info_p->type);
    }

    if (endpoint_info_p->persistent_faults != 0)
    {
        print_endpoint_faults("Persistent faults",
            endpoint_info_p->persistent_faults, endpoint_info_p->type);
    }

#if defined(BCM_SABER2_SUPPORT)
    if (SOC_IS_SABER2(unit)) {
        if (endpoint_info_p->flags & BCM_OAM_ENDPOINT_LOSS_MEASUREMENT) {
            cli_out("    Priority mapping id: ");
            cli_out(" %d ", endpoint_info_p->pri_map_id);
            cli_out("\n");
        }
    } else
#endif /* BCM_SABER2_SUPPORT */
#if defined(BCM_ENDURO_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if (endpoint_info_p->flags & BCM_OAM_ENDPOINT_LOSS_MEASUREMENT) {
        cli_out("    Priority mapping: ");
        for (i = 0; i < BCM_OAM_INTPRI_MAX; i++) {
             cli_out(" %d ", endpoint_info_p->pri_map[i]);
        }
        cli_out("\n");
    }
#endif /* BCM_ENDURO_SUPPORT */

    return BCM_E_NONE;
}

static int _cmd_esw_oam_group_endpoints_traverse(int unit,
    bcm_oam_group_info_t *group_info_p, void *user_data)
{
    return bcm_oam_endpoint_traverse(unit, group_info_p->id,
        _cmd_esw_oam_endpoint_print, NULL);
}

static int _cmd_esw_oam_event_handle(int unit, uint32 flags,
    bcm_oam_event_type_t event_type, bcm_oam_group_t group,
    bcm_oam_endpoint_t endpoint, void *user_data)
{
    int group_valid = 0;
    int endpoint_valid = 0;
    const char *oam_event_string;

    switch (event_type)
    {
        case bcmOAMEventEndpointPortDown:
            oam_event_string = "Port down";
            endpoint_valid = 1;
            break;

        case bcmOAMEventEndpointPortUp:
            oam_event_string = "Port up";
            endpoint_valid = 1;
            break;

        case bcmOAMEventEndpointInterfaceDown:
            oam_event_string = "Interface down";
            endpoint_valid = 1;
            break;

        case bcmOAMEventEndpointInterfaceUp:
            oam_event_string = "Interface up";
            endpoint_valid = 1;
            break;

        case bcmOAMEventGroupCCMxcon:
            oam_event_string = "CCM xcon";
            group_valid = 1;
            break;

        case bcmOAMEventGroupCCMError:
            oam_event_string = "CCM error";
            group_valid = 1;
            break;

        case bcmOAMEventGroupRemote:
            oam_event_string = "Some remote defect";
            group_valid = 1;
            endpoint_valid = 1;
            break;

        case bcmOAMEventGroupCCMTimeout:
            oam_event_string = "Some CCM timeout";
            group_valid = 1;
            endpoint_valid = 1;
            break;

        default:
            oam_event_string = "Unknown event";
    }

    cli_out("OAM event: %s%s", oam_event_string,
            (flags & BCM_OAM_EVENT_FLAGS_MULTIPLE) ? " (multiple)" : "");

    if (group_valid)
    {
        cli_out(" - Group %d", group);
    }

    if (endpoint_valid)
    {
        cli_out(" - Endpoint %d", endpoint);
    }

    cli_out("\n");

    return BCM_E_NONE;
}

#if defined(BCM_ENDURO_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
static int 
_cmd_esw_oam_tx(int unit, int flags, bcm_gport_t gport_dst, bcm_mac_t *mac_dst,
 bcm_mac_t *mac_src, bcm_oam_endpoint_info_t *endpoint_info)
{
    bcm_pkt_t pkt;
    enet_hdr_t *ep = NULL;
    oam_hdr_t *op = NULL;
    int vp = 0;        
    int rv = 0;
    bcm_module_t module_id, dst_module_id;
    bcm_port_t port_id, dst_port_id;
    bcm_trunk_t trunk_id, dst_trunk_id;
    int local_id, dst_local_id;
    soc_higig_hdr_t *xgh = (soc_higig_hdr_t *)pkt._higig;

    sal_memset(&pkt, 0, sizeof(bcm_pkt_t));
    
    BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, endpoint_info->gport,
        &module_id, &port_id, &trunk_id, &local_id));

    if (BCM_GPORT_IS_MIM_PORT(endpoint_info->gport) ||
        BCM_GPORT_IS_MPLS_PORT(endpoint_info->gport)) {
        vp = 1;
    }

    if ((endpoint_info->flags & BCM_OAM_ENDPOINT_UP_FACING) ||
         BCM_GPORT_IS_TRUNK(endpoint_info->gport)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, gport_dst,
            &dst_module_id, &dst_port_id, &dst_trunk_id, &dst_local_id));
    }

    sal_memset(xgh, 0, sizeof(pkt._higig));
    soc_higig_field_set(unit, xgh, HG_start, SOC_HIGIG2_START);
    soc_higig_field_set(unit, xgh, HG_opcode, SOC_HIGIG_OP_UC);
    soc_higig_field_set(unit, xgh, HG_cos, 0x7);
    
    if (vp) {
        soc_higig_field_set(unit, xgh, HG_ppd_type, 0x2);
        soc_higig_field_set(unit, xgh, HG_fwd_type, 0x4);
        soc_higig_field_set(unit, xgh, HG_multipoint, 0x0);
        soc_higig_field_set(unit, xgh, HG_dst_type, 0);
            soc_higig_field_set(unit, xgh, HG_src_type, 0);
        if (endpoint_info->flags & BCM_OAM_ENDPOINT_UP_FACING) {
            soc_higig_field_set(unit, xgh, HG_dst_vp, dst_local_id);
            /* coverity[copy_paste_error : FALSE] */
            soc_higig_field_set(unit, xgh, HG_src_vp, local_id);
        } else {
            soc_higig_field_set(unit, xgh, HG_dst_vp, local_id);
            soc_higig_field_set(unit, xgh, HG_src_vp, local_id);
        }
    } else {
        soc_higig_field_set(unit, xgh, HG_ppd_type, 0x0);
        soc_higig_field_set(unit, xgh, HG_vlan_tag, endpoint_info->vlan);
        if (endpoint_info->flags & BCM_OAM_ENDPOINT_UP_FACING) {
            soc_higig_field_set(unit, xgh, HG_dst_mod, dst_module_id);
            soc_higig_field_set(unit, xgh, HG_dst_port, dst_port_id);
            if (BCM_GPORT_IS_TRUNK(endpoint_info->gport)) {
                soc_higig_field_set(unit, xgh, HG_tgid, trunk_id);
                soc_higig_field_set(unit, xgh, HG_src_t, 1);
            } else {
                soc_higig_field_set(unit, xgh, HG_src_mod, module_id);
                soc_higig_field_set(unit, xgh, HG_src_port, port_id);
            }
        } else {
            if (BCM_GPORT_IS_TRUNK(endpoint_info->gport)) {
                soc_higig_field_set(unit, xgh, HG_dst_mod, dst_module_id);
                soc_higig_field_set(unit, xgh, HG_dst_port, dst_port_id);
                soc_higig_field_set(unit, xgh, HG_src_mod, dst_module_id);
                soc_higig_field_set(unit, xgh, HG_src_port, 0);
            } else {
                soc_higig_field_set(unit, xgh, HG_dst_mod, module_id);
                soc_higig_field_set(unit, xgh, HG_dst_port, port_id);
                soc_higig_field_set(unit, xgh, HG_src_mod, module_id);
                soc_higig_field_set(unit, xgh, HG_src_port, 0);
            }
        }
    }

    pkt.alloc_ptr = (uint8 *)soc_cm_salloc(unit, 128, "TX");        
    if (pkt.alloc_ptr == NULL) {        
        cli_out("WARNING: Could not alloc tx buffer. Memory error.\n");    
        return (BCM_E_MEMORY);
    } else {        
        pkt._pkt_data.data = pkt.alloc_ptr;        
        pkt.pkt_data = &pkt._pkt_data;        
        pkt.blk_count = 1;        
        pkt._pkt_data.len = 128;    
    }        

    /* setup the packet */    
    pkt.flags &= ~BCM_TX_CRC_FLD;    
    pkt.flags |= BCM_TX_CRC_REGEN; 
    pkt.flags |= BCM_TX_HG_READY | BCM_TX_ETHER;
    
    sal_memset(pkt.pkt_data[0].data, 0, pkt.pkt_data[0].len);        
    ep = (enet_hdr_t *)(pkt.pkt_data[0].data);
    
    ENET_SET_MACADDR(ep->en_dhost, mac_dst);    
    ENET_SET_MACADDR(ep->en_shost, mac_src);
    
    ep->en_tag_tpid = bcm_htons(0x8100);
    ep->en_tag_ctrl = bcm_htons(VLAN_CTRL(5, 0, endpoint_info->vlan));
    ep->en_tag_len  = bcm_htons(0x8902);

    op = (oam_hdr_t *)(&ep->en_snap_dsap);
    op->mdl_ver = (endpoint_info->level << 5);
    op->flags = 0;
    if (flags & BCM_OAM_ENDPOINT_LOSS_MEASUREMENT) {
        op->opcode = 43;
        op->first_tlvoffset = 12;
    } else {
        op->opcode = 47;
        op->first_tlvoffset = 32;
    }

    if ((rv = bcm_tx(unit, &pkt, NULL)) != BCM_E_NONE) {        
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "bcm_tx failed: Unit %d: %s\n"),                   
                   unit, bcm_errmsg(rv)));
    }

    soc_cm_sfree(unit, pkt.alloc_ptr);
    return (rv);
}
#endif /* !BCM_ENDURO_SUPPORT || !BCM_TRIUMPH3_SUPPORT */

char cmd_esw_oam_usage[] = 
#ifdef COMPILER_STRING_CONST_LIMIT
    "Usage: oam <option> [args...]\n"
#else
    "\n"
    "  init\n"
    "  group add [ID=<id>] Name=<name> [RemoteDefect]\n"
    "      [CopyToCpu=true|false] [LowestAlarmPri=<0-6>]\n"
    "       - Create a Maintenance Association Group.\n"
    "  group replace ID=<id> Name=<name> [RemoteDefect]\n"
    "      [CopyToCpu=true|false] [LowestAlarmPri=<0-6>]\n"
    "       - Re-configure an existing Maintenance Association Group.\n"
    "  group delete <id>\n"
    "       - Destory a Maintenance Association Group.\n"
    "  group show\n"
    "       - Display all Maintenance Association Groups.\n"
    "  endpoint add Group=<group_id> [ID=<id>]\n"
    "      [Remote] [CCM] [LB] [DM] [LM] [LT] [PBBTE] [UpMEP]\n"
    "      Name=name Level=<level> PERiod=<period> Vlan=<vlan>\n"
    "      Port=<port>|ModPort=<modid.port>|TrunkGroupId=<tid>\n"
    "      TrunkIndex=<port_index>*\n"
    "      [SrcMACaddress=<mac>] [DstMACaddress=<mac>]\n"
    "      [PktPri=<pri>] [IntPri=<int_pri>]\n"
    "      [TLV] [PortTlv=<1|2>] [IntfTlv=<1-7>]\n"
#if defined(BCM_ENDURO_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    "      [P0=#] [P1=#]\n"
#endif /* !BCM_ENDURO_SUPPORT || !BCM_TRIUMPH3_SUPPORT */
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    "      [OpcodeFlags=<flags>] [CPUPri=<cpu_pri>]\n"
    "      (*must be specified if TrunkGroupId is set)\n"
    "       - Create a Maintenance Endpoint.\n"
#endif /* !BCM_KATANA_SUPPORT || !BCM_TRIUMPH3_SUPPORT */
    "  endpoint replace Group=<group_id> [ID=<id>]\n"
    "      [Remote] [CCM] [LB] [DM] [LM] [LT] [PBBTE] [UpMEP]\n"
    "      Name=name Level=<level> PERiod=<period> Vlan=<vlan>\n"
    "      Port=<port>|ModPort=<modid.port>|TrunkGroupId=<tid>\n"
    "      TrunkIndex=<port_index>*\n"
    "      [SrcMACaddress=<mac>] [DstMACaddress=<mac>]\n"
    "      [PktPri=<pri>] [IntPri=<int_pri>]\n"
    "      [TLV] [PortTlv=<1|2>] [IntfTlv=<1-7>]\n"
#if defined(BCM_ENDURO_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    "      [P0=#] [P1=#]\n"
#endif /* !BCM_ENDURO_SUPPORT || !BCM_TRIUMPH3_SUPPORT */
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    "      [OpcodeFlags=<flags>] [CPUPri=<cpu_pri>]\n"
    "      (*must be specified if TrunkGroupId is set)\n"
    "       - Re-configure an existing Maintenance Endpoint.\n"
#endif /* !BCM_KATANA_SUPPORT || !BCM_TRIUMPH3_SUPPORT */
    "  endpoint delete <id>\n"
    "       - Destroy a Maintenance Endpoint.\n"
    "  endpoint show\n"
    "       - Display all Maintenance Endpoints.\n"
    "  events [on|off]\n"
    "       - Register for events notification.\n"
#if defined(BCM_ENDURO_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    "  tx ID=<endpointid> [DM] [LM] [DstPort=<port>]\n"
    "      [DstMACaddress=<mac>] [SrcMACaddress=<mac>\n"
    "       - Transmit LM or DM packets.\n"
#endif /* !BCM_ENDURO_SUPPORT || !BCM_TRIUMPH3_SUPPORT*/
#endif
    ;

cmd_result_t cmd_esw_oam(int unit, args_t *args)
{
    char *arg_string_p = NULL;
    parse_table_t parse_table;
    bcm_oam_group_info_t group_info;
    bcm_oam_endpoint_info_t endpoint_info;
    int remote_defect = 0;
    char *name_string_p = NULL;
    int is_remote = 0;
    int local_tx;
    int rx_ccm = 0;
    int rx_lb = 0;
    int rx_lm = 0;
    int rx_dm = 0;
    int rx_lt = 0;
    int pbbte = 0;
    int up_mep = 0;
    int endpoint_name;
    int level;
    int ccm_period;
    int vlan;
    bcm_port_t port;
    bcm_mod_port_t modport = {-1, -1};
    int trunk_id;
    int pkt_pri;
    int int_pri;
    bcm_oam_event_types_t event_types = {{0}};
    int result;
    int copy_to_cpu = 0;
    int lowest_alarm_priority = 0;
    int port_tlv = 0;
    int intf_tlv = 0;
    int tlv = 0;
#if defined(BCM_ENDURO_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    int i, pri_map[BCM_OAM_INTPRI_MAX];
    bcm_gport_t gport_dst = BCM_GPORT_INVALID;
    bcm_mac_t mac_src ={ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05 };
    bcm_mac_t mac_dst ={ 0x10, 0x11, 0x12, 0x13, 0x14, 0x15 };
#endif /* !BCM_ENDURO_SUPPORT || !BCM_TRIUMPH3_SUPPORT */
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    int cpu_pri;
    uint32 opcode_flags = 0;
#endif /* !BCM_KATANA_SUPPORT || !BCM_TRIUMPH3_SUPPORT*/
    int trunk_index = _BCM_OAM_INVALID_INDEX;

    arg_string_p = ARG_GET(args);

    if (arg_string_p == NULL)
    {
        return CMD_USAGE;
    }

    if (!sh_check_attached(ARG_CMD(args), unit))
    {
        return CMD_FAIL;
    }

    if (sal_strcasecmp(arg_string_p, "init") == 0)
    {
        result = bcm_oam_init(unit);

        if (BCM_FAILURE(result))
        {
            cli_out("Command failed.  %s.\n", bcm_errmsg(result));

            return CMD_FAIL;
        }

        cli_out("OAM module initialized.\n");
    }
    else if (sal_strcasecmp(arg_string_p, "group") == 0)
    {
		/* BCM.0> oam group ... */
        arg_string_p = ARG_GET(args);

		/* BCM.0> oam group show */
        if (arg_string_p == NULL ||
            sal_strcasecmp(arg_string_p, "show") == 0)
        {
            cli_out(GROUP_LIST_HEADER);

            if (BCM_FAILURE(bcm_oam_group_traverse(unit,
                _cmd_esw_oam_group_print, NULL)))
            {
                return CMD_FAIL;
            }
        }
        else if (sal_strcasecmp(arg_string_p, "delete") == 0)
        {
            if (ARG_CNT(args) <= 0) {
                cli_out("MISSING GROUP ID\n");
                return CMD_FAIL;
            }

            /* BCM.0> oam group delete ... */
            if (BCM_FAILURE(bcm_oam_group_destroy(unit,
                parse_integer(ARG_GET(args)))))
            {
                return CMD_FAIL;
            }
        }
        else
        {
			/* BCM.0> oam group add ... */
			/* BCM.0> oam group replace ... */
            bcm_oam_group_info_t_init(&group_info);

            if (sal_strcasecmp(arg_string_p, "replace") == 0)
            {
                group_info.flags |= BCM_OAM_GROUP_REPLACE;
            }
            else if (sal_strcasecmp(arg_string_p, "add") != 0)
            {
                cli_out("Invalid OAM group command: %s\n", arg_string_p);

                return CMD_FAIL;
            }

            parse_table_init(unit, &parse_table);

            parse_table_add(&parse_table, "ID", PQ_INT,
                (void *) BCM_OAM_GROUP_INVALID, &group_info.id, NULL);

            parse_table_add(&parse_table, "Name", PQ_STRING | PQ_DFL, 0,
                &name_string_p, NULL);

            parse_table_add(&parse_table, "RemoteDefect",
                PQ_NO_EQ_OPT | PQ_DFL | PQ_BOOL, 0,
                &remote_defect, NULL);

			parse_table_add(&parse_table, "CopyTocpu", PQ_DFL | PQ_BOOL, 0,
                &copy_to_cpu, NULL);
		
    		parse_table_add(&parse_table, "LowestAlarmPri", PQ_DFL | PQ_INT,
								0, &lowest_alarm_priority, 0);

            if (parse_arg_eq(args, &parse_table) < 0 || ARG_CNT(args) > 0)
            {
                cli_out("Invalid option: %s\n", ARG_CUR(args));

                CLEAN_UP_AND_RETURN(CMD_FAIL);
            }

            if (group_info.id != BCM_OAM_GROUP_INVALID)
            {
                group_info.flags |= BCM_OAM_GROUP_WITH_ID;
            }

            if (name_string_p == NULL)
            {
                cli_out("A group name is required.\n");

                CLEAN_UP_AND_RETURN(CMD_FAIL);
            }

            sal_strncpy((char *)group_info.name, name_string_p,
                        BCM_OAM_GROUP_NAME_LENGTH - 1);

            if (remote_defect)
            {
                group_info.flags |= BCM_OAM_GROUP_REMOTE_DEFECT_TX;
            }

			if (copy_to_cpu) 
			{
                group_info.flags |= BCM_OAM_GROUP_COPY_TO_CPU;
			}

			switch (lowest_alarm_priority) {
				case 0:
                	group_info.lowest_alarm_priority =
							bcmOAMGroupFaultAlarmPriorityDefectsAll;
					break;
				case 1:
                	group_info.lowest_alarm_priority =
							bcmOAMGroupFaultAlarmPriorityDefectRDICCM;
					break;
				case 2:
                	group_info.lowest_alarm_priority =
							bcmOAMGroupFaultAlarmPriorityDefectMACStatus;
					break;
				case 3:
                	group_info.lowest_alarm_priority =
							bcmOAMGroupFaultAlarmPriorityDefectRemoteCCM;
					break;
				case 4:
                	group_info.lowest_alarm_priority =
							bcmOAMGroupFaultAlarmPriorityDefectErrorCCM;
					break;
				case 5:
                	group_info.lowest_alarm_priority =
							bcmOAMGroupFaultAlarmPriorityDefectXconCCM;
					break;
				case 6:
                	group_info.lowest_alarm_priority =
							bcmOAMGroupFaultAlarmPriorityDefectsNone;
					break;
				default:
                	group_info.lowest_alarm_priority =
							bcmOAMGroupFaultAlarmPriorityDefectsAll;
			}

            result = bcm_oam_group_create(unit, &group_info);

            if (BCM_FAILURE(result))
            {
                cli_out("Command failed.  %s.\n", bcm_errmsg(result));

                CLEAN_UP_AND_RETURN(CMD_FAIL);
            }

            parse_arg_eq_done(&parse_table);
            
            cli_out("OAM group %d created.\n", group_info.id);
        }
    }
#if defined(BCM_ENDURO_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    else if (sal_strcasecmp(arg_string_p, "tx") == 0)
    {    
        parse_table_init(unit, &parse_table);

        parse_table_add(&parse_table, "ID", PQ_INT,
            (void *) BCM_OAM_GROUP_INVALID, &endpoint_info.id, NULL);

        parse_table_add(&parse_table, "DM",
            PQ_NO_EQ_OPT | PQ_DFL | PQ_BOOL, 0,
            &rx_dm, NULL);

        parse_table_add(&parse_table, "LM",
            PQ_NO_EQ_OPT | PQ_DFL | PQ_BOOL, 0,
            &rx_lm, NULL);

        parse_table_add(&parse_table, "DstPort", PQ_PORT, (void *)-1, &port,
                NULL);

        parse_table_add(&parse_table, "DstMACaddress", PQ_MAC | PQ_DFL, 0,
                &mac_dst, NULL);

        parse_table_add(&parse_table, "SrcMACaddress", PQ_MAC | PQ_DFL, 0,
                &mac_src, NULL);
        
        if (parse_arg_eq(args, &parse_table) < 0 || ARG_CNT(args) > 0)
        {
            cli_out("Invalid option: %s\n", ARG_CUR(args));

            CLEAN_UP_AND_RETURN(CMD_FAIL);
        }

        result = bcm_oam_endpoint_get(unit, endpoint_info.id, &endpoint_info);
        
        if (BCM_FAILURE(result))
        {
            cli_out("Command failed.  %s.\n", bcm_errmsg(result));
        
            CLEAN_UP_AND_RETURN(CMD_FAIL);
        }
        
        if (!rx_dm && !rx_lm) {
            
            cli_out("DM or LM is required.\n");

            CLEAN_UP_AND_RETURN(CMD_FAIL);
        }

        if (endpoint_info.flags & BCM_OAM_ENDPOINT_UP_FACING) {
            if (port != -1) {
                if (BCM_GPORT_IS_MIM_PORT(port) || BCM_GPORT_IS_MPLS_PORT(port)) {
                    gport_dst = port;
                } else {
                    BCM_GPORT_LOCAL_SET(gport_dst, port);
                }
            } else {
                cli_out("Destination port is required for UP MEP.\n");
                CLEAN_UP_AND_RETURN(CMD_FAIL);
            }
        } else if (BCM_GPORT_IS_TRUNK(endpoint_info.gport)) {
            if (port != -1) {
                BCM_GPORT_LOCAL_SET(gport_dst, port);
            } else {
                cli_out("Destination port is required for down MEP created on TRUNK.\n");
                CLEAN_UP_AND_RETURN(CMD_FAIL);
            }
        }

        if (rx_dm) {
            pbbte |= BCM_OAM_ENDPOINT_DELAY_MEASUREMENT;
        } else {
            pbbte |= BCM_OAM_ENDPOINT_LOSS_MEASUREMENT;
        }

        result = _cmd_esw_oam_tx(unit, pbbte, gport_dst,
                                 &mac_dst, &mac_src, &endpoint_info);
        if (BCM_FAILURE(result)) {
            cli_out("Command failed.  %s.\n", bcm_errmsg(result));
            CLEAN_UP_AND_RETURN(CMD_FAIL);
        }
        parse_arg_eq_done(&parse_table);
    }
#endif /* !BCM_ENDURO_SUPPORT || !BCM_TRIUMPH3_SUPPORT */

    /* oam endpoint .... */
    else if (sal_strcasecmp(arg_string_p, "endpoint") == 0)
    {
        arg_string_p = ARG_GET(args);

        /* oam endpoint show .... */
        if (arg_string_p == NULL ||
            sal_strcasecmp(arg_string_p, "show") == 0)
        {
            
            cli_out(ENDPOINT_LIST_HEADER);

            if (BCM_FAILURE(bcm_oam_group_traverse(unit,
                _cmd_esw_oam_group_endpoints_traverse, NULL)))
            {
                return CMD_FAIL;
            }
        }

        /* oam endpoint delete .... */
        else if (sal_strcasecmp(arg_string_p, "delete") == 0)
        {
            if (ARG_CNT(args) <= 0) {
                cli_out("MISSING ENDPOINT ID\n");
                return CMD_FAIL;
            }

            result = bcm_oam_endpoint_destroy(unit,
                parse_integer(ARG_GET(args)));

            if (BCM_FAILURE(result))
            {
                cli_out("Command failed. %s\n", bcm_errmsg(result));

                return CMD_FAIL;
            }
        }
        else
        {
            bcm_oam_endpoint_info_t_init(&endpoint_info);

            /* oam endpoint replace .... */
            if (sal_strcasecmp(arg_string_p, "replace") == 0)
            {
                endpoint_info.flags |= BCM_OAM_ENDPOINT_REPLACE;
            }

            /* oam endpoint add .... */
            else if (sal_strcasecmp(arg_string_p, "add") != 0)
            {
                cli_out("Invalid OAM endpoint command: %s\n", arg_string_p);

                return CMD_FAIL;
            }
            
            parse_table_init(unit, &parse_table);

            parse_table_add(&parse_table, "Group", PQ_INT, (void *) -1,
                &endpoint_info.group, NULL);

            parse_table_add(&parse_table, "ID", PQ_INT,
                (void *) BCM_OAM_ENDPOINT_INVALID, &endpoint_info.id, NULL);

            parse_table_add(&parse_table, "Remote",
                PQ_NO_EQ_OPT | PQ_DFL | PQ_BOOL, 0,
                &is_remote, NULL);

            parse_table_add(&parse_table, "CCM",
                PQ_NO_EQ_OPT | PQ_DFL | PQ_BOOL, 0,
                &rx_ccm, NULL);

            parse_table_add(&parse_table, "LB",
                PQ_NO_EQ_OPT | PQ_DFL | PQ_BOOL, 0,
                &rx_lb, NULL);

            parse_table_add(&parse_table, "DM",
                PQ_NO_EQ_OPT | PQ_DFL | PQ_BOOL, 0,
                &rx_dm, NULL);

            parse_table_add(&parse_table, "LM",
                PQ_NO_EQ_OPT | PQ_DFL | PQ_BOOL, 0,
                &rx_lm, NULL);

            parse_table_add(&parse_table, "LT",
                PQ_NO_EQ_OPT | PQ_DFL | PQ_BOOL, 0,
                &rx_lt, NULL);

            parse_table_add(&parse_table, "PBBTE",
                PQ_NO_EQ_OPT | PQ_DFL | PQ_BOOL, 0,
                &pbbte, NULL);

            parse_table_add(&parse_table, "UpMEP",
                PQ_NO_EQ_OPT | PQ_DFL | PQ_BOOL, 0,
                &up_mep, NULL);


            /* Needed for remote and local tx, not for local rx */

            parse_table_add(&parse_table, "Name", PQ_HEX, (void *) -1,
                &endpoint_name, NULL);

            /* Needed for all types */

            parse_table_add(&parse_table, "Level", PQ_INT, (void *) -1,
                &level, NULL);

            /* Needed for remote and local tx, not for local rx */

            parse_table_add(&parse_table, "PERiod", PQ_INT, (void *) -1,
                &ccm_period, NULL);

            /* Needed for all types */

            parse_table_add(&parse_table, "Vlan", PQ_INT,
                (void *) BCM_VLAN_INVALID, &vlan, NULL);

            parse_table_add(&parse_table, "Port", PQ_PORT, (void *) -1, &port,
                NULL);

            parse_table_add(&parse_table, "ModPort", PQ_MOD_PORT | PQ_DFL, 0,
                &modport, NULL);

            parse_table_add(&parse_table, "TrunkGroupId", PQ_INT,
                (void *) BCM_TRUNK_INVALID, &trunk_id, NULL);
            
            parse_table_add(&parse_table, "TrunkIndex", PQ_INT,
                (void *) -1, &trunk_index, NULL);

            /* Needed for local tx, not remote or local rx */

            parse_table_add(&parse_table, "SrcMACaddress", PQ_MAC | PQ_DFL, 0,
                &endpoint_info.src_mac_address, NULL);

            parse_table_add(&parse_table, "DstMACaddress", PQ_MAC | PQ_DFL, 0,
                &endpoint_info.dst_mac_address, NULL);

            /* Needed for local tx, not remote or local rx */

            parse_table_add(&parse_table, "PktPri", PQ_INT, 0, &pkt_pri,
                NULL);

            /* Needed for local tx, not remote or local rx */

            parse_table_add(&parse_table, "IntPri", PQ_INT, 0, &int_pri,
                NULL);

            parse_table_add(&parse_table, "TLV",
                PQ_NO_EQ_OPT | PQ_DFL | PQ_BOOL, 0,
                &tlv, NULL);

            /* Insert port status in transmitted CCM PDU. */
            parse_table_add(&parse_table, "PortTlv", PQ_INT, (void *) 0,
                &port_tlv, NULL);

            /* Insert interface status in transmitted CCM PDU. */
            parse_table_add(&parse_table, "IntfTlv", PQ_INT, (void *) 0,
                &intf_tlv, NULL);

#if defined(BCM_ENDURO_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
            sal_memset(pri_map, 0, BCM_OAM_INTPRI_MAX*sizeof(int));
            /* Priority mapping for LM counter index */
            parse_table_add(&parse_table, "P0", PQ_INT,
                (void *)( 0), &pri_map[0], NULL);
            parse_table_add(&parse_table, "P1", PQ_INT,
                (void *)( 0), &pri_map[1], NULL);
            parse_table_add(&parse_table, "P2", PQ_INT,
                (void *)( 0), &pri_map[2], NULL);
            parse_table_add(&parse_table, "P3", PQ_INT,
                (void *)( 0), &pri_map[3], NULL);
            parse_table_add(&parse_table, "P4", PQ_INT,
                (void *)( 0), &pri_map[4], NULL);
            parse_table_add(&parse_table, "P5", PQ_INT,
                (void *)( 0), &pri_map[5], NULL);
            parse_table_add(&parse_table, "P6", PQ_INT,
                (void *)( 0), &pri_map[6], NULL);
            parse_table_add(&parse_table, "P7", PQ_INT,
                (void *)( 0), &pri_map[7], NULL);
            parse_table_add(&parse_table, "P8", PQ_INT,
                (void *)( 0), &pri_map[8], NULL);
            parse_table_add(&parse_table, "P9", PQ_INT,
                (void *)( 0), &pri_map[9], NULL);
            parse_table_add(&parse_table, "P10", PQ_INT,
                (void *)( 0), &pri_map[10], NULL);
            parse_table_add(&parse_table, "P11", PQ_INT,
                (void *)( 0), &pri_map[11], NULL);
            parse_table_add(&parse_table, "P12", PQ_INT,
                (void *)( 0), &pri_map[12], NULL);
            parse_table_add(&parse_table, "P13", PQ_INT,
                (void *)( 0), &pri_map[13], NULL);
            parse_table_add(&parse_table, "P14", PQ_INT,
                (void *)( 0), &pri_map[14], NULL);
            parse_table_add(&parse_table, "P15", PQ_INT,
                (void *)( 0), &pri_map[15], NULL);
#endif /* !BCM_ENDURO_SUPPORT || !BCM_TRIUMPH3_SUPPORT*/

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
            parse_table_add(&parse_table, "OpcodeFlags", PQ_HEX, (void *) (0),
                                          &opcode_flags, NULL);

            parse_table_add(&parse_table, "CPUPri", PQ_INT, 0, &cpu_pri,
                                          NULL);
#endif /* !BCM_KATANA_SUPPORT || !BCM_TRIUMPH3_SUPPORT */

            if (parse_arg_eq(args, &parse_table) < 0 || ARG_CNT(args) > 0)
            {
                cli_out("Invalid option: %s\n", ARG_CUR(args));

                parse_arg_eq_done(&parse_table);

                return CMD_FAIL;
            }
#if defined(BCM_ENDURO_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
            for (i = 0; i < BCM_OAM_INTPRI_MAX; i++) {
                endpoint_info.pri_map[i] = (uint8)pri_map[i];
            }
#endif /* !BCM_ENDURO_SUPPORT || !BCM_TRIUMPH3_SUPPORT*/

            /* Insert TLV in CCM PDU. */
            if (tlv) {
                endpoint_info.flags |= (BCM_OAM_ENDPOINT_PORT_STATE_TX
                                        | BCM_OAM_ENDPOINT_INTERFACE_STATE_TX);
            }

            /* Update port TLV status value. */
            if (port_tlv) {
                endpoint_info.port_state = port_tlv;
                endpoint_info.flags |= BCM_OAM_ENDPOINT_PORT_STATE_UPDATE;
            }

            /* Update interface TLV status value. */
            if (intf_tlv) {
                endpoint_info.interface_state = intf_tlv;
                endpoint_info.flags |= BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE;
            }

            local_tx = !is_remote && (ccm_period > 0);

            if (endpoint_info.id != BCM_OAM_ENDPOINT_INVALID)
            {
                endpoint_info.flags |= BCM_OAM_ENDPOINT_WITH_ID;
            }

            /* Check for parameters needed for all types */

            if (endpoint_info.group < 0)
            {
                cli_out("A group is required.\n");

                CLEAN_UP_AND_RETURN(CMD_FAIL);
            }

            if ((level < 0) && (!is_remote))
            {
                cli_out("A level is required.\n");

                CLEAN_UP_AND_RETURN(CMD_FAIL);
            }

            if ((vlan == BCM_VLAN_INVALID) && (!is_remote))
            {
                cli_out("A VLAN is required.\n");

                CLEAN_UP_AND_RETURN(CMD_FAIL);
            }

            if (endpoint_name < 0)
            {
                cli_out("An endpoint name is required.\n");

                CLEAN_UP_AND_RETURN(CMD_FAIL);
            }

            if (modport.port != -1)
            {
                BCM_GPORT_MODPORT_SET(endpoint_info.gport, modport.mod,
                    modport.port);

                endpoint_info.trunk_index = _BCM_OAM_INVALID_INDEX;
            }
            else if (port != -1)
            {
                if (BCM_GPORT_IS_MIM_PORT(port) || BCM_GPORT_IS_MPLS_PORT(port)) {
                    endpoint_info.gport = port;
                } else {
                    BCM_GPORT_LOCAL_SET(endpoint_info.gport, port);
                }

                endpoint_info.trunk_index = _BCM_OAM_INVALID_INDEX;
            }
            else if (trunk_id != BCM_TRUNK_INVALID)
            {
                /* 
                 * Trunk index value is required to determine the
                 * designation port for Tx.
                 */
                if ((1 == local_tx) && (-1 == trunk_index)) {

                    cli_out("A TrunkIndex value is required.\n");

                    CLEAN_UP_AND_RETURN(CMD_FAIL);
                }
 
                BCM_GPORT_TRUNK_SET(endpoint_info.gport, trunk_id);

                endpoint_info.trunk_index = trunk_index;
            }
            else
            {
                if (!is_remote) {
                    cli_out("An port or trunk group ID is required.\n");

                    CLEAN_UP_AND_RETURN(CMD_FAIL);
                }
            }

            endpoint_info.level = level;
            endpoint_info.vlan = vlan;
            endpoint_info.name = (uint16) endpoint_name;
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
            endpoint_info.opcode_flags = opcode_flags;
#endif /* !BCM_KATANA_SUPPORT || !BCM_TRIUMPH3_SUPPORT */

            /* Check for parameters needed for remote and local tx */

            if (is_remote || local_tx)
            {
                if (ccm_period < 0)
                {
                    cli_out("A CCM period is required.\n");

                    CLEAN_UP_AND_RETURN(CMD_FAIL);
                }

                endpoint_info.ccm_period = ccm_period;
            }
            
            /* Check for parameters needed only for local tx */

            if (local_tx)
            {
                if (BCM_MAC_IS_ZERO(endpoint_info.src_mac_address))
                {
                    cli_out("Source MAC address is required for CCM TX.\n");

                    CLEAN_UP_AND_RETURN(CMD_FAIL);
                }

#if defined(BCM_ENDURO_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
                if (SOC_IS_ENDURO(unit) || SOC_IS_TRIUMPH3(unit)) {
                    if (BCM_MAC_IS_ZERO(endpoint_info.dst_mac_address)) {
                        cli_out("Destination MAC address is required for CCM TX.\n");

                        CLEAN_UP_AND_RETURN(CMD_FAIL);
                    }
                }
#endif /* !BCM_ENDURO_SUPPORT || !BCM_TRIUMPH3_SUPPORT*/

                if (pkt_pri < 0)
                {
                    cli_out("A packet priority is required.\n");

                    CLEAN_UP_AND_RETURN(CMD_FAIL);
                }

                if (int_pri < 0)
                {
                    cli_out("An internal priority is required.\n");

                    CLEAN_UP_AND_RETURN(CMD_FAIL);
                }

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
                if (cpu_pri < 0)
                {
                    cli_out("A cpu queue is required.\n");

                    CLEAN_UP_AND_RETURN(CMD_FAIL);
                }
                endpoint_info.cpu_qid = cpu_pri;
#endif /* !BCM_KATANA_SUPPORT || !BCM_TRIUMPH3_SUPPORT*/

                endpoint_info.pkt_pri = pkt_pri;
                endpoint_info.int_pri = int_pri;
            }

            if (is_remote)
            {
                endpoint_info.flags |= BCM_OAM_ENDPOINT_REMOTE;
            }

            if (rx_ccm)
            {
                endpoint_info.flags |= BCM_OAM_ENDPOINT_CCM_RX;
            }

            if (rx_lb)
            {
                endpoint_info.flags |= BCM_OAM_ENDPOINT_LOOPBACK;
            }

            if (rx_dm)
            {
                endpoint_info.flags |= BCM_OAM_ENDPOINT_DELAY_MEASUREMENT;
            }
            
            if (rx_lm)
            {
                endpoint_info.flags |= BCM_OAM_ENDPOINT_LOSS_MEASUREMENT;
            }

            if (rx_lt)
            {
                endpoint_info.flags |= BCM_OAM_ENDPOINT_LINKTRACE;
            }
            
            if (pbbte)
            {
                endpoint_info.flags |= BCM_OAM_ENDPOINT_PBB_TE;
            }

            if (up_mep)
            {
                endpoint_info.flags |= BCM_OAM_ENDPOINT_UP_FACING;
            }

            endpoint_info.type = bcmOAMEndpointTypeEthernet;

            result = bcm_oam_endpoint_create(unit, &endpoint_info);

            if (BCM_FAILURE(result))
            {
                cli_out("Command failed. %s\n", bcm_errmsg(result));

                parse_arg_eq_done(&parse_table);

                return CMD_FAIL;
            }

            parse_arg_eq_done(&parse_table);
            
            cli_out("OAM endpoint %d created.\n", endpoint_info.id);
        }
    }
    else if (sal_strcasecmp(arg_string_p, "events") == 0)
    {
        arg_string_p = ARG_GET(args);

        if (arg_string_p != NULL)
        {
            if (sal_strcasecmp(arg_string_p, "on") == 0)
            {
                BCM_OAM_EVENT_TYPE_SET(event_types, bcmOAMEventEndpointPortDown);
                BCM_OAM_EVENT_TYPE_SET(event_types, bcmOAMEventEndpointPortUp);
                BCM_OAM_EVENT_TYPE_SET(event_types, bcmOAMEventEndpointInterfaceDown);
                BCM_OAM_EVENT_TYPE_SET(event_types, bcmOAMEventEndpointInterfaceUp);
                BCM_OAM_EVENT_TYPE_SET(event_types, bcmOAMEventGroupCCMxcon);
                BCM_OAM_EVENT_TYPE_SET(event_types, bcmOAMEventGroupCCMError);
                BCM_OAM_EVENT_TYPE_SET(event_types, bcmOAMEventGroupRemote);
                BCM_OAM_EVENT_TYPE_SET(event_types, bcmOAMEventGroupCCMTimeout);
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
                if (SOC_IS_KATANA(unit)
                    || SOC_IS_TRIUMPH3(unit)) {
                    BCM_OAM_EVENT_TYPE_SET(event_types, bcmOAMEventEndpointInterfaceTestingToUp);
                    BCM_OAM_EVENT_TYPE_SET(event_types, bcmOAMEventEndpointInterfaceUnknownToUp);
                    BCM_OAM_EVENT_TYPE_SET(event_types, bcmOAMEventEndpointInterfaceDormantToUp);
                    BCM_OAM_EVENT_TYPE_SET(event_types, bcmOAMEventEndpointInterfaceNotPresentToUp);
                    BCM_OAM_EVENT_TYPE_SET(event_types, bcmOAMEventEndpointInterfaceLLDownToUp);
                    BCM_OAM_EVENT_TYPE_SET(event_types, bcmOAMEventEndpointInterfaceTesting);
                    BCM_OAM_EVENT_TYPE_SET(event_types, bcmOAMEventEndpointInterfaceUnkonwn);
                    BCM_OAM_EVENT_TYPE_SET(event_types, bcmOAMEventEndpointInterfaceDormant);
                    BCM_OAM_EVENT_TYPE_SET(event_types, bcmOAMEventEndpointInterfaceNotPresent);
                    BCM_OAM_EVENT_TYPE_SET(event_types, bcmOAMEventEndpointInterfaceLLDown);
                }
#endif /* BCM_KATANA_SUPPORT */

                if (BCM_FAILURE(bcm_oam_event_register(unit, event_types,
                    _cmd_esw_oam_event_handle, NULL)))
                {
                    cli_out("Failed to enable OAM events.\n");

                    return CMD_FAIL;
                }

                events_on = 1;
            }
            else if (sal_strcasecmp(arg_string_p, "off") == 0)
            {
                BCM_OAM_EVENT_TYPE_SET(event_types, bcmOAMEventEndpointPortDown);
                BCM_OAM_EVENT_TYPE_SET(event_types, bcmOAMEventEndpointPortUp);
                BCM_OAM_EVENT_TYPE_SET(event_types, bcmOAMEventEndpointInterfaceDown);
                BCM_OAM_EVENT_TYPE_SET(event_types, bcmOAMEventEndpointInterfaceUp);
                BCM_OAM_EVENT_TYPE_SET(event_types, bcmOAMEventGroupCCMxcon);
                BCM_OAM_EVENT_TYPE_SET(event_types, bcmOAMEventGroupCCMError);
                BCM_OAM_EVENT_TYPE_SET(event_types, bcmOAMEventGroupRemote);
                BCM_OAM_EVENT_TYPE_SET(event_types, bcmOAMEventGroupCCMTimeout);
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
                if (SOC_IS_KATANA(unit)
                    || SOC_IS_TRIUMPH3(unit)) {
                    BCM_OAM_EVENT_TYPE_SET(event_types, bcmOAMEventEndpointInterfaceTestingToUp);
                    BCM_OAM_EVENT_TYPE_SET(event_types, bcmOAMEventEndpointInterfaceUnknownToUp);
                    BCM_OAM_EVENT_TYPE_SET(event_types, bcmOAMEventEndpointInterfaceDormantToUp);
                    BCM_OAM_EVENT_TYPE_SET(event_types, bcmOAMEventEndpointInterfaceNotPresentToUp);
                    BCM_OAM_EVENT_TYPE_SET(event_types, bcmOAMEventEndpointInterfaceLLDownToUp);
                    BCM_OAM_EVENT_TYPE_SET(event_types, bcmOAMEventEndpointInterfaceTesting);
                    BCM_OAM_EVENT_TYPE_SET(event_types, bcmOAMEventEndpointInterfaceUnkonwn);
                    BCM_OAM_EVENT_TYPE_SET(event_types, bcmOAMEventEndpointInterfaceDormant);
                    BCM_OAM_EVENT_TYPE_SET(event_types, bcmOAMEventEndpointInterfaceNotPresent);
                    BCM_OAM_EVENT_TYPE_SET(event_types, bcmOAMEventEndpointInterfaceLLDown);
                }
#endif /* BCM_KATANA_SUPPORT */

                if (BCM_FAILURE(bcm_oam_event_unregister(unit, event_types,
                    _cmd_esw_oam_event_handle)))
                {
                    cli_out("Failed to disable OAM events.\n");

                    return CMD_FAIL;
                }

                events_on = 0;
            }
            else
            {
                cli_out("Invalid OAM events option: %s\n", arg_string_p);

                return CMD_FAIL;
            }
        }

        cli_out("OAM events %s\n", events_on ? "on" : "off");
    }
    else
    {
        cli_out("Invalid OAM subcommand: %s\n", arg_string_p);

        return CMD_FAIL;
    }

    return CMD_OK;
}
