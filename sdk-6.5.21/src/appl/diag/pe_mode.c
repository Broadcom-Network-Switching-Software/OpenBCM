/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:       pe_mode.c
 */

#include <shared/bsl.h>
#include <appl/diag/system.h>

#ifdef PE_MODE_CMD

#include <netinet/in.h>
#include <bcm/extender.h>
#include <bcm/link.h>


#define PE_MODE_ID_ETHERTYPE       0x869f
#define LLDP_TLV_HDR_TYPE_MASK     0xfe00
#define LLDP_TLV_HDR_TYPE_SHIFT    9
#define LLDP_TLV_HDR_LEN_MASK      0x01ff

#define BCM_TLV_TYPE               127
#define BCM_TLV_OUI_VAL            0x18c086
#define BCM_MODE_TLV_SUBTYPE_VAL   0x01
#define BCM_TAG_TLV_SUBTYPE_VAL    0x02
#define BCM_ECID_TLV_SUBTYPE_VAL   0x03
#define BCM_PRE_DEFINED_ECID0      0x11
#define BCM_PRE_DEFINED_ECID1      0x22
#define BCM_PRE_DEFINED_ECID2      0x33
#define BCM_PRE_DEFINED_ECID3      0x44

#define BCM_MODE_TLV_DETECTED_FLAG 1
#define BCM_TAG_TLV_DETECTED_FLAG  2
#define BCM_ECID_TLV_DETECTED_FLAG 4
#define LLDP_TLV_PARSER_DONE       0xFF

#define LLDP_TLV_HDR_LEN           2
#define LLDP_TLV_OUI_LEN           3
#define BCM_ECID_VAL_LEN           4

typedef struct pe_port_info_s {
    int count;
    bcm_gport_t pe_gport;
    bcm_gport_t extender_port_id[4];
} pe_port_info_t;

int pemode_init(int unit);
int pe_mode_enable(int unit);

#endif /* PE_MODE_CMD */


/*
 * Function: cmd_pemode
 * Purpose: initialize pemode
 * Parameters:  u - SOC unit #
 *              a - pointer to args
 * Returns: CMD_OK/CMD_FAIL
 */
cmd_result_t
cmd_pemode(int u, args_t *a)
{

#ifdef PE_MODE_CMD
    pemode_init(u);
    pe_mode_enable(u);
#else
    cli_out("cmd_pemode() API calls commented out\n");
#endif /* PE_MODE_CMD */

    return CMD_OK;
}


#ifdef PE_MODE_CMD

int pe_port_enable(int unit, int pe_port)
{
    int rv;
    /* extender ports */
    bcm_extender_port_t extender_port[4];
    int pe_etag_pcp[4] = {0, 2, 4, 6};
    int pe_etag_vid[4] = {0x11, 0x22, 0x33, 0x44};
    int i, num_pe_port = 4;

    bcm_gport_t pe_gport;       /* gport to access the extended ports */
    int val;
    bcm_vlan_t vlan_id, def_vlan_id;
    int flags, mgroup;
    bcm_vlan_control_vlan_t vlan_ctrl;

    /* get the gport of pe_gport */
    rv = bcm_port_gport_get(unit, pe_port, &pe_gport);
    if (BCM_FAILURE(rv)) {
        cli_out("bcm_port_gport_get() failed (%s)\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_port_control_get(unit, pe_port, bcmPortControlExtenderType, &val) ;
    if (BCM_FAILURE(rv)) {
        cli_out("Failed to get port %d extender type: %s\n", pe_port, bcm_errmsg(rv));
        return rv;
    }
    if (BCM_PORT_EXTENDER_TYPE_SWITCH == val) {
        /* cli_out("Port is in PE MODE already, return without any operations.\n"); */
        return BCM_E_NONE;
    }

    /* set port type for the pe_port */
    rv = bcm_port_control_set(unit, pe_gport, bcmPortControlExtenderType, BCM_PORT_EXTENDER_TYPE_SWITCH);
    if (BCM_FAILURE(rv)) {
        cli_out("bcm_port_control_set() failed (%s)\n", bcm_errmsg(rv));
        return rv;
    }

    /* add pe port */
    for (i=0; i<num_pe_port; i++) {
        /* initialize each of the port extender port structures */
        bcm_extender_port_t_init(&extender_port[i]);
        extender_port[i].flags = 0;
        extender_port[i].port = pe_gport;
        extender_port[i].extended_port_vid = pe_etag_vid[i];
        extender_port[i].pcp_de_select = BCM_EXTENDER_PCP_DE_SELECT_DEFAULT;
        extender_port[i].pcp = pe_etag_pcp[i];
        extender_port[i].de = 0;
        rv = bcm_extender_port_add(unit, &extender_port[i]);
        if (BCM_FAILURE(rv)) {
            cli_out("bcm_extender_port_add(%d) failed (%s)\n", i, bcm_errmsg(rv));
            return rv;
        }
        /*
        cli_out("bcm_extender_port_add(%d) ep_vid: 0x%x; ep_id: 0x%x\n", i,
                extender_port[i].extended_port_vid, extender_port[i].extender_port_id);
        */
    }

    /* get default vlan */
    bcm_vlan_default_get(unit, &def_vlan_id);
    vlan_id = def_vlan_id;

    /* don't remove the Etag on egress of pe port */
    rv = bcm_port_control_set(unit, pe_gport, bcmPortControlEtagEgressDelete, 0);
    if (BCM_FAILURE(rv)) {
        cli_out("bcm_port_control_set(%d) failed (%s)\n", pe_port, bcm_errmsg(rv));
        return rv;
    }

    /* configure entender port vlans for untagged packets */
    for (i=0; i<num_pe_port; i++) {
        rv = bcm_port_untagged_vlan_set(unit, extender_port[i].extender_port_id, vlan_id);
        if (BCM_FAILURE(rv)) {
            cli_out("Set default vlan on extender port %#x failed: %s \n",
                    extender_port[i].extender_port_id,	bcm_errmsg(rv));
            return rv;
        }
    }

    bcm_vlan_control_vlan_t_init(&vlan_ctrl);
    rv = bcm_vlan_control_vlan_get(unit, vlan_id, &vlan_ctrl);
    if(BCM_FAILURE(rv)) {
        cli_out("Failed to get vlan %d information: %s\n", vlan_id, bcm_errmsg(rv));
        return rv;
    }
    mgroup = vlan_ctrl.broadcast_group;

    /* Multicast is not created */
    if (!mgroup) {
        /* create multicast group */
        flags = BCM_MULTICAST_TYPE_EXTENDER;
        rv = bcm_multicast_create(unit, flags, &mgroup);
        if (BCM_FAILURE(rv)) {
            cli_out("Create multigroup failed: %s\n", bcm_errmsg(rv));
            return rv;
        }

        /* configure multicast group vlans */
        bcm_vlan_control_vlan_t_init(&vlan_ctrl);
        vlan_ctrl.broadcast_group        = mgroup;
        vlan_ctrl.unknown_multicast_group = mgroup;
        vlan_ctrl.unknown_unicast_group   = mgroup;
        rv = bcm_vlan_control_vlan_set(unit, vlan_id, vlan_ctrl);
        if(BCM_FAILURE(rv)) {
            cli_out("Failed to configure multicast group on vlan %d: %s\n", vlan_id, bcm_errmsg(rv));
            return rv;
        }
    }

    /* add pe_port to new vlan */
    flags = BCM_VLAN_PORT_UNTAGGED;
    rv = bcm_vlan_gport_add(unit, vlan_id, pe_gport, flags);
    if (BCM_FAILURE(rv)) {
        cli_out("Failed to configure port %#x as untagged port: %s\n", pe_gport, bcm_errmsg(rv));
        return rv;
    }
    for (i=0; i<num_pe_port; i++) {
        rv = bcm_vlan_gport_add(unit, vlan_id, extender_port[i].extender_port_id, flags);
        if (BCM_FAILURE(rv)) {
            cli_out("Error to add extender port %#x to vlan %d: %s\n", 
                    extender_port[i].extender_port_id, vlan_id, bcm_errmsg(rv));
            return rv;
        }
    }

    cli_out("Port %d has been configured as PE mode\n", pe_port);	
    return BCM_E_NONE;
}


int extender_port_traverse_cb(int unit, bcm_extender_port_t *extender_port, void *user_data)
{
    pe_port_info_t pe_port_info;
    sal_memcpy(&pe_port_info, user_data, sizeof(pe_port_info));
    if (extender_port->port == pe_port_info.pe_gport) {
        if (pe_port_info.count >= 4) {
            cli_out("Error, number of extender port of port %#x is bigger than 4\n", 
                   pe_port_info.pe_gport);
            return BCM_E_NONE;
        }
        pe_port_info.extender_port_id[pe_port_info.count] = extender_port->extender_port_id;
        pe_port_info.count ++;
    }
    sal_memcpy(user_data, &pe_port_info, sizeof(pe_port_info));
    return BCM_E_NONE;
}


int pe_port_disable(int unit, int pe_port) 
{
    int rv;
    int i;
    bcm_gport_t pe_gport;       /* gport to access the extended ports */
    int flags, val;
    bcm_vlan_t vlan_id, def_vlan_id;
    pe_port_info_t pe_port_info;

    /* get the gport of pe_gport */
    rv = bcm_port_gport_get(unit, pe_port, &pe_gport);
    if (BCM_FAILURE(rv)) {
        cli_out("bcm_port_gport_get() failed (%s)\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_port_control_get(unit, pe_port, bcmPortControlExtenderType, &val);
    if (BCM_FAILURE(rv)) {
        cli_out("Failed to get port %d extender type: %s\n", pe_port, bcm_errmsg(rv));
        return rv;
    }
    if (BCM_PORT_EXTENDER_TYPE_SWITCH != val) {
        cli_out("Port %d is not in PE mode\n", pe_port);
        return BCM_E_NONE;
    }

    /* get default vlan */
    bcm_vlan_default_get(unit, &def_vlan_id);
    vlan_id = def_vlan_id;

    sal_memset(&pe_port_info, 0, sizeof(pe_port_info));
    pe_port_info.pe_gport = pe_gport;
    rv = bcm_extender_port_traverse(0, extender_port_traverse_cb, (void*)&pe_port_info);
    if (BCM_FAILURE(rv)) {
        cli_out("Failed to get extender ports of port 0x%x: %s\n", pe_gport, bcm_errmsg(rv));
        return rv;
    }
    for (i=0; i<pe_port_info.count; i++)
    {
        rv = bcm_vlan_gport_delete(unit, vlan_id, pe_port_info.extender_port_id[i]);
        if (BCM_FAILURE(rv)) {
            cli_out("Error in deleting extender port 0x%x from vlan %d: %s\n", 
                    pe_port_info.extender_port_id[i], vlan_id, bcm_errmsg(rv));
            return rv;
        }
        rv = bcm_extender_port_delete(unit, pe_port_info.extender_port_id[i]);
        if (BCM_FAILURE(rv)) {
            cli_out("bcm_extender_port_delete(0x%x) failed (%s)\n", 
                    pe_port_info.extender_port_id[i], bcm_errmsg(rv));
            return rv;
        }
    }

    /* set port to ethernet port */
    rv = bcm_port_control_set(unit, pe_gport, bcmPortControlExtenderType, BCM_PORT_EXTENDER_TYPE_NONE);
    if (BCM_FAILURE(rv)) {
        cli_out("bcm_port_control_set() failed (%s)\n", bcm_errmsg(rv));
        return rv;
    }
 
    rv = bcm_port_control_set(unit, pe_gport, bcmPortControlEtagEgressDelete, 1);
    if (BCM_FAILURE(rv)) {
        cli_out("Configure port(%d) to delete ETAG failed (%s)\n", pe_port, bcm_errmsg(rv));
        return rv;
    }
    /* add pe_port to new vlan */
    flags = BCM_VLAN_PORT_UNTAGGED;
    rv = bcm_vlan_gport_add(unit, vlan_id, pe_gport, flags);
    if (BCM_FAILURE(rv)) {
        cli_out("Failed to configure port 0x%x as untagged port: %s\n", pe_gport, bcm_errmsg(rv));
        return rv;
    }

    cli_out("PE mode on port %d has been disabled\n", pe_port);

    return BCM_E_NONE;
}


void dump_pkt(bcm_pkt_t *pkt)
{
    int i;
    cli_out("Packet from port %d", pkt->src_port);
    cli_out("\n");
    for (i=0; i<16; i++) {
        cli_out("===");
    }
    cli_out("\n");
    for (i=0; i<16; i++) {
        cli_out("%02d ", i);
    }
    cli_out("\n");
    for (i=0; i<16; i++) {
        cli_out("===");
    }

    cli_out("\n%02x ", pkt->pkt_data->data[0]);
    for (i=1; i<pkt->pkt_len; i++) {
        if (i%16) {
            cli_out("%02x ", pkt->pkt_data->data[i]);
        } else {
            cli_out("\n%02x ", pkt->pkt_data->data[i]);
        }
    }
    cli_out("\n");
}


int brcm_tlv_parser(char *data, int *length)
{
    int subtype, tlv_hdr, tlv_len, tlv_type;
    int tlv_oui;
    /*int op_mode, tag_mode;*/
    uint16      *tlvptr=(uint16*)data;
    uint8       *uint8ptr;
    uint8       ecid[4];

    tlv_hdr = ntohs(*tlvptr);
    tlv_len = tlv_hdr & LLDP_TLV_HDR_LEN_MASK;
    tlv_type = (tlv_hdr & LLDP_TLV_HDR_TYPE_MASK) >> LLDP_TLV_HDR_TYPE_SHIFT;
    /* cli_out("%s() tlv_hdr 0x%x; tlv_len 0x%x; tlv type 0x%x\n", __func__, tlv_hdr, tlv_len, tlv_type); */
    *length = tlv_len + LLDP_TLV_HDR_LEN;
    if (!tlv_type)
    {
        return LLDP_TLV_PARSER_DONE;
    }
    /* Parser BRCM OUI only */
    if (BCM_TLV_TYPE == tlv_type) {
        uint8ptr =  (uint8*)(data+LLDP_TLV_HDR_LEN);
        tlv_oui = *(uint8ptr++)<<16;
        tlv_oui += *(uint8ptr++)<<8;
        tlv_oui += *(uint8ptr++);
        /*cli_out("BCM TLV OUI value: 0x%x\n", tlv_oui);*/
        if (tlv_oui == BCM_TLV_OUI_VAL) {
            subtype = data[LLDP_TLV_HDR_LEN+LLDP_TLV_OUI_LEN];
            /*cli_out("BCM TLV subtype value: 0x%x\n", subtype);*/
            switch(subtype) {
                case BCM_MODE_TLV_SUBTYPE_VAL:
                    /*op_mode = data[LLDP_TLV_HDR_LEN+LLDP_TLV_OUI_LEN+1];*/
                    /*cli_out("Opertaion mode: %d\n", op_mode);*/
                    return BCM_MODE_TLV_DETECTED_FLAG;
                    break;
                case BCM_TAG_TLV_SUBTYPE_VAL:
                    /*tag_mode = data[LLDP_TLV_HDR_LEN+LLDP_TLV_OUI_LEN+1];*/
                    /*cli_out("Tag mode: %d\n", tag_mode);*/
                    return BCM_TAG_TLV_DETECTED_FLAG;
                    break;
                case BCM_ECID_TLV_SUBTYPE_VAL:
                    uint8ptr =  (uint8*)(data+LLDP_TLV_HDR_LEN+LLDP_TLV_OUI_LEN+1);
                    ecid[0] = *(uint8ptr++);
                    ecid[1] = *(uint8ptr++);
                    ecid[2] = *(uint8ptr++);
                    ecid[3] = *(uint8ptr++);
                    if (BCM_PRE_DEFINED_ECID0 != ecid[0]) {
                        cli_out("Unexpected TLV ECID0 value: 0x%x\n", ecid[0]);
                        return 0;
                    } else if (BCM_PRE_DEFINED_ECID1 != ecid[1]) {
                        cli_out("Unexpected TLV ECID1 value: 0x%x\n", ecid[1]);
                        return 0;
                    } else if (BCM_PRE_DEFINED_ECID2 != ecid[2]) {
                        cli_out("Unexpected TLV ECID2 value: 0x%x\n", ecid[2]);
                        return 0;
                    } else if (BCM_PRE_DEFINED_ECID3 != ecid[3]) {
                        cli_out("Unexpected TLV ECID3 value: 0x%x\n", ecid[3]);
                        return 0;
                    } else {
                        /*cli_out("ECID TLV is detected: 0x%x, 0x%x, 0x%x, 0x%x\n", ecid[0], ecid[1], ecid[2], ecid[3]);*/
                    }
                    return BCM_ECID_TLV_DETECTED_FLAG;
                    break;
                default:
                    cli_out("Unknown subtype in BRCM TLV: %d\n", subtype);
                    return 0;
            }
        }
    }
    return 0;
}


int is_brcm_pe_pkt(bcm_pkt_t* pkt)
{
    int offset, length;
    int ether_type, rv, flags;
    uint16      *ethtype;

    /* Check ether type: offset:16, length: 2*/
    offset = 16;
    length = 2;
    ethtype = (uint16*)(pkt->pkt_data->data+offset);
    ether_type = ntohs(*ethtype);
    /* cli_out("%s() ethtype 0x%x\n", __func__, ether_type); */
    if (ether_type != PE_MODE_ID_ETHERTYPE) {
        return 0;
    }

    /* TLV parser started */
    offset += length;
    flags = 0;
    do {
        rv = brcm_tlv_parser((char*)(pkt->pkt_data->data+offset), &length);
        if (rv == LLDP_TLV_PARSER_DONE) {
            break;
        } else {
            flags |= rv;
        }

        /* dead loop protection */
        if (length <= LLDP_TLV_HDR_LEN) {
            break;
        }

        offset += length;
        if (offset >= pkt->pkt_len - 4) {
            break;
        }
    } while (1);
    /* print flags; */
    if (flags == (BCM_MODE_TLV_DETECTED_FLAG | BCM_TAG_TLV_DETECTED_FLAG | BCM_ECID_TLV_DETECTED_FLAG)) {
        return 1;
    } else {
       cli_out("Failed to parser packet\n");
       dump_pkt(pkt);
    }
    return 0;
}


bcm_rx_t pe_protocol_parser(int unit, bcm_pkt_t* pkt, void* cookie)
{
    int rv;

    if (NULL == pkt) {
        cli_out("pkt is NULL\n");
        return BCM_RX_HANDLED;
    }

    rv = is_brcm_pe_pkt(pkt);
    if (rv) {
        if (pe_port_enable(unit, pkt->src_port)) {
            cli_out("\nFailed to configure port %d into PE mdoe\n", pkt->src_port);
        }
        return BCM_RX_HANDLED;
    }
    else {
        return BCM_RX_NOT_HANDLED;
    }
}


void pe_disable_on_linkdown(int unit, bcm_port_t port, bcm_port_info_t *info)
{
    if (info->linkstatus == 0) {
        pe_port_disable(unit, port);
    }
    return;
}


int pemode_init(int unit)
{
    int rv;
    int port;
    bcm_gport_t gport;
    bcm_port_config_t port_config;
    int val;

    bcm_port_config_t_init(&port_config);

    /* bcmSwitchL3EgressMode must be set */
    rv = bcm_switch_control_set(unit, bcmSwitchL3EgressMode, 1);
    if (BCM_FAILURE(rv)) {
        cli_out("Error to configure device into L3EgressMode: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* initialize the port extender module */
    rv = bcm_extender_init(unit);
    if (BCM_FAILURE(rv)) {
        cli_out("bcm_extender_init() failed: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* set extender ethertype to    0x893f (BR tag) */
    rv = bcm_switch_control_set(unit, bcmSwitchEtagEthertype, 0x893f);
    if (BCM_FAILURE(rv)) {
        cli_out("Error to configure extender more ether type: %s\n", bcm_errmsg(rv));
        return rv;
    }
    
    rv = bcm_port_config_get(unit, &port_config);
    if (BCM_FAILURE(rv)) {
        cli_out("Failed to get port config: %s\n", bcm_errmsg(rv));
        return rv;
    }

    cli_out("Initializing PE Mode\n");
    /* configure the switch to remove the Etag on egress of ethernet port */
    BCM_PBMP_ITER(port_config.all, port){
        rv = bcm_port_gport_get(unit, port, &gport);
        if (BCM_FAILURE(rv)) {
            cli_out("bcm_port_gport_get(%d) failed (%s)\n", port, bcm_errmsg(rv));
            return rv;
        }

        rv = bcm_port_control_get(unit, port, bcmPortControlExtenderType, &val);
        if (BCM_FAILURE(rv)) {
            cli_out("Failed to get port %d extender type: %s\n", port, bcm_errmsg(rv));
            return rv;
        }

        /* If the port is in PE mode, then next*/
        if (BCM_PORT_EXTENDER_TYPE_NONE != val) {
            cli_out("Port %d is Port Extender, do not reconfigure\n", port);
            continue;
        }

        /* Ethernet port, delete ETAG on egress */
        rv = bcm_port_control_set(unit, gport, bcmPortControlEtagEgressDelete, 1);
        if (BCM_FAILURE(rv)) {
            cli_out("Configure port(%d) to delete ETAG failed (%s)\n", port, bcm_errmsg(rv));
            return rv;
        }
    }

    return BCM_E_NONE;
}


int pe_mode_enable(int unit)
{
    bcm_rx_cfg_t rx_cfg;

    cli_out("Initializing PE-ID packet and Link-Down Callbacks\n");
    bcm_rx_cfg_t_init(&rx_cfg);
    bcm_rx_cfg_get(unit, &rx_cfg);

    bcm_rx_register(unit, "PE PACKET PARSER", pe_protocol_parser, BCM_RX_PRIO_MAX, NULL, BCM_RCO_F_ALL_COS);
    bcm_linkscan_register(unit, pe_disable_on_linkdown);
    if (!bcm_rx_active(unit)) {
        cli_out("Starting RX process\n");
        bcm_rx_start(unit, &rx_cfg);
    } else {
        cli_out("RX process is already started\n");
    }
    return CMD_OK;
}

#endif /* PE_MODE_CMD */

