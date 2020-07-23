/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*

SOC properties:
===============

# Set Jericho parser mode
parser_mode=1

# Enable PPPoE
# 0: disable PPPoE (default)
# 1: enable PPPoE
PPPoE_mode=1

# Enable L2TP,  values are
# 0: disable L2TP (default)
# 1: enable L2TP 
L2TP_mode=1

# The L2TP is encapsulated in an UDPoIP tunnel so setup IPv4 tunnel lookup mode
# allowed values for L2TP are dip_termination and dip_sip_termination
# if dip_sip_vrf_termination is required, ip termination should be configured as dip_sip and l2tp_termination as vrf
#
# IPv4 tunnel lookup mode:
#	0: none 
#	1: dip_sip_termination 
#	2: dip_termination
#	3: both dip_sip and dip_termination
#	4: dip_sip_port_next_protocol_termination 
#	5: dip_sip_port_next_protocol and dip_termination. 
#		In modes 4 and 5, port lookup is done by looking at the LSBs [0..3] of bcmPortClassFieldIngressVlanTranslation
#	6: dip_sip_vrf_termination
#
bcm886xx_ip4_tunnel_termination_mode=2
 
# Disable MPLS_CW, 0 - enable (default), 1 - disable
custom_feature_mpls_cw_disable=1
 
Run:
cd ../../../../src/examples/dpp/
cint ../sand/utility/cint_sand_utils_global.c
cint cint_l2tp_pppoe.c

debug bcm rx verbose
cint

- set ip_tunnel_termination mode according to soc properties
Is_DIP_SIP_Termination = 0;

- configure everything
config_pmf(unit);
config_ports(unit,pppoe_port,l2tp_port,ip_port);
x_connect(unit);

- send 4 ip packets, all should go to pppoe with session numbers 0xcb00..0xcb03
tx_ip(unit,0x0f);


- send 3 pppoe packets
* 1st packet should be tunneled to L2TP with pppoe_session=0xcb00-->l2tp_tunnel:session=0xca00:0000
* 2nd packet should be go to IP since PPPoE termination to L2TP is not defined for session 0xcb01
* 3rd packet should go to IP since we are trying to spoof pppoe_session=0xcb00 on packet with vlan=0xcb0 instead of vlan=0xcb0
tx_pppoe(unit,0x07);

- send 5 l2tp pkts (tunnel:session=0xca00:0000), all should be tunneled to pppoe (l2tp_tunnel:session=0xca00:0000-->pppoe_session=0xcb00)
- They have different l2tp_options and different PPP protocol IDs
tx_l2tp(unit,0x01f);

- delete all tunnels
delete_tunnels(unit);

*/

char Msg[256];
char Cmd[256];
int Unit = 0;
int rc;

bcm_mac_t GlobalMac = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55 };
bcm_mac_t NniExternalMac = { 0x66, 0x11, 0x22, 0x33, 0x44, 0x55 };
bcm_mac_t UniExternalMac = { 0x88, 0x11, 0x22, 0x33, 0x44, 0x55 };
int StationId;

bcm_field_group_config_t CrossConnectFG, L2tpOptionsFG;
bcm_field_presel_set_t CrossConnectPreselSet, L2tpOptionsPreselSet;
int QidL2tpOptions, QidInLifWideData;

bcm_vlan_t Vsi[4], Vlan[4][16];
bcm_vlan_port_t Ac[4][16];
bcm_if_t Rif[4], ArpEncap[4];
bcm_mac_t NextHopMac[4], MyMac[4];
bcm_if_t Fec[4], L2tpFec[4][16], PppoeFec[4][16];

bcm_gport_t IpTunnelTerminatorGport[4], L2tpTerminatorGport[4][16], PppoeTerminatorGport[4][16];

bcm_if_t IpTunnelInitiatorIf[4][16];
bcm_gport_t L2tpTunnelInitiatorGport[4][16];

uint32 Subnet[4][16], Mask[4], Host[4][16];

uint32 L2tpSession[16], PppoeSession[16], L2tpTunnel;

bcm_ip_t NLS_IP;
bcm_ip_t MY_IP;
int Is_DIP_SIP_Termination = 0;
int L2tpVrf;

int idx;

/*---------------------------------------------------------------------------------------------------------------------------------------------------------
Initialize global variables for the 3 different port types
We use array with different index for each port type
In case we have different value per session in l2tp or in pppoe we have 2nd index for the session
---------------------------------------------------------------------------------------------------------------------------------------------------------*/
enum port_idx_t {
	ip_idx = 0,
	l2tp_idx = 2,
	pppoe_idx = 3
};


/*
 * IP port values
 */

Vsi[0] = 0x168;
Vlan[0][0] = 0x168;

/* MyMac=00:11:22:33:41:68 */
sal_memcpy(MyMac[0], GlobalMac, 6); MyMac[0][5] = Vsi[0] & 0xff; MyMac[0][4] = (MyMac[0][4] & 0xf0) | (Vsi[0] >> 8) & 0xf;
/* NextHopMac=66:11:22:33:44:c8 */
sal_memcpy(NextHopMac[0], NniExternalMac, 6); NextHopMac[0][5] = 0xc8;

Subnet[0][0] = 0x0a0ac800;
Mask[0] = 0xffffff00;
Host[0][0] = 0;

/*
 * L2TP port values
 */

Vsi[2] = 0x16a;
Vlan[2][0] = 0x16a;

/* MyMac=00:11:22:33:41:6a */
sal_memcpy(MyMac[2], GlobalMac, 6); MyMac[2][5] = Vsi[2] & 0xff; MyMac[2][4] = (MyMac[2][4] & 0xf0) | (Vsi[2] >> 8) & 0xf;
/* NextHopMac=66:11:22:33:44:ca */
sal_memcpy(NextHopMac[2], NniExternalMac, 6); NextHopMac[2][5] = 0xca;

Subnet[2][0] = 0x0a0aca00;
Mask[2] = 0xffffff00;
Host[2][0] = 0;

NLS_IP = 0x0a0a66ca;
MY_IP = 0x0a0aca6a;
L2tpVrf = 100;

L2tpTunnel = 0x00ca;
for(idx = 0; idx < 16; idx++) {
	L2tpSession[idx] = idx;
}

/*
 * PPPoE port values
 */

Vsi[3] = 0x26b;

/* MyMac=00:11:22:33:42:6b */
sal_memcpy(MyMac[3], GlobalMac, 6); MyMac[3][5] = Vsi[3] & 0xff; MyMac[3][4] = (MyMac[3][4] & 0xf0) | (Vsi[3] >> 8) & 0xf;
/* NextHopMac=88:11:22:33:44:cb */
sal_memcpy(NextHopMac[3], UniExternalMac, 6); NextHopMac[3][5] = 0xcb;

for(idx = 0; idx < 16; idx++) {
	Vlan[3][idx] = 0xcb0 + idx;
	PppoeSession[idx] = 0xcb00 + idx;
	Host[3][idx] = 0x0a0acb40 + idx;
	Subnet[3][idx] = 0x0a0acb40 + idx;
}
Mask[3] = 0xffffffff; /*32 bits mask for adding host only */

/*---------------------------------------------------------------------------------------------------------------------------------------------------------
The cross-connect between L2TP_LIF and PPPOE_LIF is done using PMF with direct extraction
This FG is using a preselector TT-lookup0-found=1
The PMF lookup params are:
- TtProcessingProfile=3 (L2TP)
- TtProcessingProfile=5 (PPPOE)
The PMF action is:
- bcmFieldActionRedirect=the fec pointer from the extended lif data (wide lif)
---------------------------------------------------------------------------------------------------------------------------------------------------------*/
int init_cross_connect_fg(int unit)
{
	int ret_val = BCM_E_NONE;
	bcm_field_qset_t qset;
	bcm_field_aset_t aset;
    bcm_field_data_qualifier_t data_qual;
	bcm_field_presel_t presel1;
	bcm_field_entry_t entry;
	bcm_field_extraction_field_t ext_field[2];
	bcm_field_extraction_action_t ext_action;
	bcm_field_entry_t entry2;
	bcm_field_extraction_action_t ext_action2;
	uint8 val[2], mask[2];

	/* Create Qualifier Set - this is the key */
	BCM_FIELD_QSET_INIT(qset);
	BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyTunnelTerminationProcessingProfile);

	/* in_lif_wide_data data qualifier */
	bcm_field_data_qualifier_t_init(&data_qual);
	data_qual.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES;
	data_qual.qualifier = bcmFieldQualifyInVPortWide;
	data_qual.offset = 0;
	data_qual.length = 20; 
	rc = bcm_field_data_qualifier_create(unit, &data_qual);
	if(rc) { printf("Error-init_cross_connect_fg-3: rc=%d\n", rc); return rc; }

	/* add to qset */
	rc = bcm_field_qset_data_qualifier_add(unit, &qset, data_qual.qual_id);
	if(rc) { printf("Error-init_cross_connect_fg-6: rc=%d\n", rc); return rc; }
	QidInLifWideData = data_qual.qual_id;

	/* Create Action Set */
	BCM_FIELD_ASET_INIT(aset);
	BCM_FIELD_ASET_ADD(aset, bcmFieldActionRedirect);
	
	/* init presel */
	bcm_field_presel_create(unit, &presel1);
	bcm_field_qualify_TunnelTerminated(unit, presel1 | BCM_FIELD_QUALIFY_PRESEL, 0x1, 0x1);

	bcm_field_presel_set_t_init(&CrossConnectPreselSet);
	BCM_FIELD_PRESEL_ADD(CrossConnectPreselSet, presel1);

	/* Create The FG */
	bcm_field_group_config_t_init(&CrossConnectFG);
	CrossConnectFG.flags = BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET | BCM_FIELD_GROUP_CREATE_SMALL;
	CrossConnectFG.qset = qset;
	CrossConnectFG.priority = 0;
	CrossConnectFG.mode = bcmFieldGroupModeDirectExtraction;
	CrossConnectFG.aset = aset;
	CrossConnectFG.preselset = CrossConnectPreselSet;
	sal_memcpy(CrossConnectFG.name, "CrossConnect PPPOE/L2TP", sal_strlen("CrossConnect PPPOE/L2TP") + 1); /* Name for Group. */

	rc = bcm_field_group_config_create(unit, &CrossConnectFG);
	if(rc) { printf("Error-init_cross_connect_fg-10: rc=%d\n", rc); return rc; }

	/*---------------------------------------------------------------------------------------------------------------------------------------------------------
	- TtProcessingProfile=3 (L2TP)
	---------------------------------------------------------------------------------------------------------------------------------------------------------*/
	bcm_field_extraction_action_t_init(&ext_action);
	bcm_field_extraction_field_t_init(&ext_field[0]);
	bcm_field_extraction_field_t_init(&ext_field[1]);
	
	rc = bcm_field_entry_create(unit, CrossConnectFG.group, &entry);
	if(rc) { printf("Error-init_cross_connect_fg-20: rc=%d\n", rc); return rc; }

	/* TtProcessingProfile=3 (L2TP) */
	val[0] = 3;/*bcmFieldTunnelTerminationProcessingProfileL2tp*/
	mask[0] = 0x07;
	rc = bcm_field_qualify_TunnelTerminationProcessingProfile(unit, entry, val[0], mask[0]);
	if(rc) { printf("Error-init_cross_connect_fg-30: rc=%d\n", rc); return rc; }

	ext_action.action = bcmFieldActionRedirect;
	ext_action.bias = 0;

	/* the fec pointer */
	ext_field[0].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
	ext_field[0].bits = 17;
	ext_field[0].lsb = 0;
	ext_field[0].qualifier = QidInLifWideData;

	/* fecpointer is 19 bits starting with 01 folowwed by the actual pointer */
	/* this will fill the 2 prefix bits of 01 */
	ext_field[1].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
	ext_field[1].bits = 2;
	ext_field[1].lsb = 0;
	ext_field[1].qualifier = bcmFieldQualifyCount;
	ext_field[1].value = 01;

	rc = bcm_field_direct_extraction_action_add(unit, entry, ext_action, 2, ext_field);
	if(rc) { printf("Error-init_cross_connect_fg-40: rc=%d\n", rc); return rc; }

	rc = bcm_field_entry_install(unit, entry);
	if(rc) { printf("Error-init_cross_connect_fg-50: rc=%d\n", rc); return rc; }

	/*---------------------------------------------------------------------------------------------------------------------------------------------------------
	- TtProcessingProfile=5 (PPPOE)
	---------------------------------------------------------------------------------------------------------------------------------------------------------*/
	bcm_field_extraction_action_t_init(&ext_action2);
	
	rc = bcm_field_entry_create(unit, CrossConnectFG.group, &entry2);
	if(rc) { printf("Error-init_cross_connect_fg-60: rc=%d\n", rc); return rc; }
	
	/* TtProcessingProfile=5 (PPPoE) */
	val[0] = 5;/*bcmFieldTunnelTerminationProcessingProfilePppoe*/
	mask[0] = 0x07;
	rc = bcm_field_qualify_TunnelTerminationProcessingProfile(unit, entry, val[0], mask[0]);
	if(rc) { printf("Error-init_cross_connect_fg-70: rc=%d\n", rc); return rc; }
	
	rc = bcm_field_direct_extraction_action_add(unit, entry2, ext_action, 2, ext_field);
	if(rc) { printf("Error-init_cross_connect_fg-80: rc=%d\n", rc); return rc; }

	rc = bcm_field_entry_install(unit, entry2);
	if(rc) { printf("Error-init_cross_connect_fg-90: rc=%d\n", rc); return rc; }

	return ret_val;
}

/*---------------------------------------------------------------------------------------------------------------
L2TP can have optional fields. The options are:
P - priority bit, does not affect the size of the header
L - length field - terminated by the parser
S - sequence numbers - 4 bytes
O - offset - 2 bytes + value-of-offset-field
*this implementation supports only value-of-offset-field=0

Only the L option is terminated by the parser.
We need also to terminate other optional fields.
We will do this using this PMF FG

This FG is using a preselector bcmFieldQualifyTunnelTerminated
The PMF lookup param is bcmFieldQualifyTunnelTerminationProcessingProfile with the value:
- bcmFieldTunnelTerminationProcessingProfilePppoe
- bcmFieldTunnelTerminationProcessingProfileL2tp

The action is bcmFieldActionForwardingTypeNew
This action has multiple fields:
- param0 - forwarding_code
- param1 - forwarding_header
- param2 - number_of_bytes_to_strip

We need to set the following:
For l2tp:
- fwd_code=bcmFieldForwardingTypeCustom1
- fwd_header=3
- strip=num of options bytes to strip from the header
*we need only to set fwd_code & strip, fwd_header is just kept the same

For pppoe:
- fwd_code=bcmFieldForwardingTypeCustom1
- fwd_header=2
- strip=0
*we need only to set fwd_code, fwd_header&strip are just kept the same

We use PMF also to handle the P option (priority).
P bit is qualified, and if exist we set TC=7/DP=0
---------------------------------------------------------------------------------------------------------------*/
int init_l2tp_options_fg(int unit)
{
	int ret_val = BCM_E_NONE;
	bcm_field_qset_t qset;
	bcm_field_aset_t aset;
    bcm_field_data_qualifier_t data_qual;
	bcm_field_presel_t presel1;

	/* Create Qualifier Set - this is the key */
	BCM_FIELD_QSET_INIT(qset);
	BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyTunnelTerminationProcessingProfile);

	/* add L2TP options data-header qualifier */
	bcm_field_data_qualifier_t_init(&data_qual);
	data_qual.flags = 0;
	data_qual.offset_base = bcmFieldDataOffsetBaseFirstHeader;
	data_qual.offset = 28; /* ip_header_size=20 + udp_header_size=8 */
	data_qual.length = 1;
	rc = bcm_field_data_qualifier_create(unit, &data_qual);
	if(rc) { printf("Error-init_l2tp_options_fg-10: rc=%d\n", rc); return rc; }

	/* add to qset */
	rc = bcm_field_qset_data_qualifier_add(unit, &qset, data_qual.qual_id);
	if(rc) { printf("Error-init_l2tp_options_fg-20: rc=%d\n", rc); return rc; }
	QidL2tpOptions = data_qual.qual_id;

	/* Create Action Set */
	BCM_FIELD_ASET_INIT(aset);
	BCM_FIELD_ASET_ADD(aset, bcmFieldActionForwardingTypeNew); /* Forwarding-Code & Forwarding-Header */
	BCM_FIELD_ASET_ADD(aset, bcmFieldActionPrioIntNew);
	BCM_FIELD_ASET_ADD(aset, bcmFieldActionDropPrecedence);
	
	/* init presel */
	bcm_field_presel_create(unit, &presel1);
	bcm_field_qualify_TunnelTerminated(unit, presel1 | BCM_FIELD_QUALIFY_PRESEL, 0x1, 0x1);

	bcm_field_presel_set_t_init(&L2tpOptionsPreselSet);
	BCM_FIELD_PRESEL_ADD(L2tpOptionsPreselSet, presel1);

	/* Create The DB */
	bcm_field_group_config_t_init(&L2tpOptionsFG);
	L2tpOptionsFG.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET | BCM_FIELD_GROUP_CREATE_SMALL;
	L2tpOptionsFG.qset = qset;
	L2tpOptionsFG.priority = 1;
	L2tpOptionsFG.mode = bcmFieldGroupModeAuto;
	L2tpOptionsFG.aset = aset;
	L2tpOptionsFG.preselset = L2tpOptionsPreselSet;
	sal_memcpy(L2tpOptionsFG.name, "L2TP Options", sal_strlen("L2TP Options") + 1); /* Name for Group. */

	rc = bcm_field_group_config_create(unit, &L2tpOptionsFG);
	if(rc) { printf("Error-init_l2tp_options_fg-30: rc=%d\n", rc); return rc; }

	return ret_val;
}

/* l2tp header options bit masks */
uint8 T_BIT = 0x80;
uint8 L_BIT = 0x40;
uint8 S_BIT = 0x04;
uint8 O_BIT = 0x02;
uint8 P_BIT = 0x01;

/*---------------------------------------------------------------------------------------------------------------------------------------------------------

---------------------------------------------------------------------------------------------------------------------------------------------------------*/
int add_l2tp_options_actions(int unit, int direction, uint8 options_mask, uint8 options_value, int strip, int tc, bcm_field_entry_t *entry)
{
	uint8 val[4], mask[4];
	bcm_field_action_core_config_t core_config_arr;

	/* first we try to create the entry. IF out of resources we return. */
	rc = bcm_field_entry_create(unit, L2tpOptionsFG.group, entry);
	if(rc) { printf("Error-add_l2tp_options_actions-20: rc=%d\n", rc); return rc; }

	/* set entry priority */
	rc = bcm_field_entry_prio_set(unit, *entry, 20);
	if(rc) { printf("Error-add_l2tp_options_actions-30: rc=%d\n", rc); return rc; }
	
	if(direction == 1) {
		/* L2TP--->PPPOE */
		
		/* qualify TT-Processing-Profile=3 */
		val[0] = 3;/*bcmFieldTunnelTerminationProcessingProfileL2tp*/
		mask[0] = 0x07;
		rc = bcm_field_qualify_TunnelTerminationProcessingProfile(unit, *entry, val[0], mask[0]);
		if(rc) { printf("Error-add_l2tp_options_actions-60: rc=%d\n", rc); return rc; }

		/* qualify the options */
		val[0] = options_value;
		mask[0] = options_mask;
		rc = bcm_field_qualify_data(unit, *entry, QidL2tpOptions, val, mask, 1);
		if(rc) { printf("Error-add_l2tp_options_actions-64: rc=%d\n", rc); return rc; }

		/* strip the options, fwd_code=custom1, forwarding_header=3 */
		core_config_arr.flags = 0;
		core_config_arr.param0 = bcmFieldForwardingTypeCustom1;
		core_config_arr.param1 = 3;
		core_config_arr.param2 = strip;
		core_config_arr.param3 = 0;
		rc = bcm_field_action_config_add(unit, *entry, bcmFieldActionForwardingTypeNew, 1, &core_config_arr);
		if(rc) { printf("Error-add_l2tp_options_actions-70: rc=%d\n", rc); return rc; }

		/* for priority we set tc=7, dp=0 */
		/* this is just an example, you can do other stuff here */
		if(options_mask & options_value & P_BIT) {
			rc = bcm_field_action_add(unit, *entry, bcmFieldActionPrioIntNew, 7, 0);
			if(rc) { printf("Error-add_l2tp_options_actions-80: rc=%d\n", rc); return rc; }

			rc = bcm_field_action_add(unit, *entry, bcmFieldActionDropPrecedence, BCM_FIELD_COLOR_GREEN, 0);
			if(rc) { printf("Error-add_l2tp_options_actions-90: rc=%d\n", rc); return rc; }
		}
	} else {
		/* PPPOE-->L2TP */

		/* qualify TT-Processing-Profile */
		val[0] = 5;/*bcmFieldTunnelTerminationProcessingProfilePppoe*/
		mask[0] = 0x07;
		rc = bcm_field_qualify_TunnelTerminationProcessingProfile(unit, *entry, val[0], mask[0]);
		if(rc) { printf("Error-add_l2tp_options_actions-93: rc=%d\n", rc); return rc; }

		/* add action fwd_code=custom1 */
		rc = bcm_field_action_add(unit, *entry, bcmFieldActionForwardingTypeNew, bcmFieldForwardingTypeCustom1, 2);
		if(rc) { printf("Error-add_l2tp_options_actions-96: rc=%d\n", rc); return rc; }
	}

	/* Install to HW */
	rc = bcm_field_entry_install(unit, *entry);
	if(rc) { printf("Error-add_l2tp_options_actions-98: rc=%d\n", rc); return rc; }

	return 0;
}


/*---------------------------------------------------------------------------------------------------------------

---------------------------------------------------------------------------------------------------------------*/
int init_l2_global_attr(int unit)
{
	bcm_l2_station_t l2_station;

	/* define global mac address */
	bcm_l2_station_t_init(&l2_station);
	sal_memcpy(l2_station.dst_mac, GlobalMac, 6);
	sal_memset(l2_station.dst_mac_mask, 0xff, 6);

	rc = bcm_l2_station_add(unit, &StationId, &l2_station);
	if(rc) { printf("Error-init_l2_global_attr-10: rc=%d\n", rc); return rc; }

	return 0;
}

/*---------------------------------------------------------------------------------------------------------------

---------------------------------------------------------------------------------------------------------------*/
int init_l2_port_attr(int unit, bcm_port_t port)
{
	uint32 vlan_domain;
	bcm_vlan_t vlan;

	/* set VlanDomain=port */
	vlan_domain = port;
	rc = bcm_port_class_set(unit, port, bcmPortClassId, vlan_domain);
	if(rc) { printf("Error-init_l2_port_attr-10: rc=%d\n", rc); return rc; }

	return 0;
}

/*---------------------------------------------------------------------------------------------------------------
this function will create a RIF and ARP entry for a port
we create only the ARP entry without a FEC. 
FEC creation is done seperately since we want to control ARP and FEC creation seperately.
---------------------------------------------------------------------------------------------------------------*/
int init_l3_intf(int unit, bcm_vlan_t vsi, bcm_mac_t my_mac, bcm_mac_t next_hop_mac, int vrf, bcm_if_t *rif, bcm_if_t *arp_encap)
{
	bcm_l3_intf_t l3_intf;
	bcm_l3_ingress_t l3_ingress;
	bcm_l3_egress_t l3_egress;
	bcm_if_t dummy;

	/* create L3 RIF */
	bcm_l3_intf_t_init(&l3_intf);
	sal_memcpy(l3_intf.l3a_mac_addr, my_mac, 6);
	l3_intf.l3a_vid = vsi;
	l3_intf.l3a_ttl = 64;
	rc = bcm_l3_intf_create(unit, &l3_intf);
	if(rc) { printf("Error-init_l3_intf-10: rc=%d\n", rc); return rc; }
	*rif = l3_intf.l3a_intf_id;

	/* create ingress object to further configure the RIF with vrf  */
	bcm_l3_ingress_t_init(&l3_ingress);
	rc = bcm_l3_ingress_get(unit, *rif, &l3_ingress);
	if(rc) { printf("Error-init_l3_intf-20: rc=%d\n", rc); return rc; }

	if(vrf == 0) 
		l3_ingress.flags |= (BCM_L3_INGRESS_WITH_ID | BCM_L3_INGRESS_GLOBAL_ROUTE);
	else
		l3_ingress.flags |= BCM_L3_INGRESS_WITH_ID;
	l3_ingress.vrf = vrf;
	
	rc = bcm_l3_ingress_create(unit, &l3_ingress, rif);
	if(rc) { printf("Error-init_l3_intf-30: rc=%d\n", rc); return rc; }

	/* create egress object only (ARP), here we don't create the FEC */
	bcm_l3_egress_t_init(&l3_egress);
	sal_memcpy(l3_egress.mac_addr, next_hop_mac, 6);
	l3_egress.intf = *rif;
	l3_egress.vlan = vsi;
	rc = bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY, &l3_egress, &dummy);
	if(rc) { printf("Error-init_l3_intf-40: rc=%d\n", rc); return rc; }
	*arp_encap = l3_egress.encap_id;

	printf("L3_INTF: vsi=0x%03x, my_mac=%02x:%02x:%02x:%02x:%02x:%02x, next_hop_mac=%02x:%02x:%02x:%02x:%02x:%02x, vrf=%d, rif=0x%x, arp_encap=0x%x\n",
		vsi,
		my_mac[0], my_mac[1], my_mac[2], my_mac[3], my_mac[4], my_mac[5], 
		next_hop_mac[0], next_hop_mac[1], next_hop_mac[2], next_hop_mac[3], next_hop_mac[4], next_hop_mac[5], 
		vrf, *rif, *arp_encap);
	
	return 0;
}

/*---------------------------------------------------------------------------------------------------------------
create a FEC entry
this function will create a fec that points to the following:
- port=dest_gport
- intf=rif_or_tunnel
- encap_id=arp_encap
---------------------------------------------------------------------------------------------------------------*/
int init_l3_fec(int unit, bcm_gport_t dest_gport, bcm_if_t rif_or_tunnel, bcm_if_t arp_encap, bcm_if_t *fec)
{
	bcm_l3_egress_t l3_egress;

	/* create egress object (FEC-->dest,RIF,ARP) */
	bcm_l3_egress_t_init(&l3_egress);
	l3_egress.port = dest_gport;
	l3_egress.intf = rif_or_tunnel;
	l3_egress.encap_id = arp_encap;
	rc = bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY, &l3_egress, fec);
	if(rc) { printf("Error-init_l3_fec-50: rc=%d\n", rc); return rc; }

	printf("L3_FEC: dest=0x%x, rif_or_tunnel=0x%x, arp_encap=0x%x, fec=0x%x\n",
		dest_gport, rif_or_tunnel, arp_encap, *fec);
	
	return 0;
}

/*---------------------------------------------------------------------------------------------------------------
create a super-FEC entry
this function will create a super-fec that points to the following:
- port=dest_gport
- intf=rif_or_tunnel
- encap_id=arp_encap
---------------------------------------------------------------------------------------------------------------*/
int init_l3_super_fec(int unit, bcm_gport_t dest_gport, bcm_if_t rif_or_tunnel, bcm_if_t arp_encap, bcm_if_t *fec)
{
    bcm_failover_t failover_id_fec = 0;
	bcm_if_t fec_primary, fec_backup;
	bcm_l3_egress_t l3_egress_primary, l3_egress_backup;

    /* create failover id */
    rc = bcm_failover_create(unit, BCM_FAILOVER_FEC, &failover_id_fec);
	if(rc) { printf("Error-init_l3_super_fec-45: rc=%d\n", rc); return rc; }

	/* create egress object (FEC-->dest,RIF,ARP) */
	/* first we create the backup FEC since when creating the primary we need to point to the backup */
	bcm_l3_egress_t_init(&l3_egress_backup);
	l3_egress_backup.port = dest_gport;
	l3_egress_backup.intf = rif_or_tunnel;
	l3_egress_backup.encap_id = arp_encap;
	l3_egress_backup.failover_id = failover_id_fec;
	rc = bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY, &l3_egress_backup, &fec_backup);
	if(rc) { printf("Error-init_l3_super_fec-50: rc=%d\n", rc); return rc; }

	/* create egress object (FEC-->dest,RIF,ARP) */
	/* this is the primary pointing to the backup */
	bcm_l3_egress_t_init(&l3_egress_primary);
	l3_egress_primary.port = dest_gport;
	l3_egress_primary.intf = rif_or_tunnel;
	l3_egress_primary.encap_id = arp_encap;
	l3_egress_primary.failover_id = failover_id_fec;
	l3_egress_primary.failover_if_id = fec_backup;
	rc = bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY, &l3_egress_primary, &fec_primary);
	if(rc) { printf("Error-init_l3_super_fec-55: rc=%d\n", rc); return rc; }

	*fec = fec_primary;

	printf("L3_SUPER_FEC: dest=0x%x, rif_or_tunnel=0x%x, arp_encap=0x%x, fec=0x%x\n",
		dest_gport, rif_or_tunnel, arp_encap, *fec);
	
	return 0;
}

/*---------------------------------------------------------------------------------------------------------------

---------------------------------------------------------------------------------------------------------------*/
int add_route(int unit, uint32 subnet, uint32 mask, uint32 host, bcm_if_t dest_fec)
{
	bcm_l3_route_t l3_route;
	bcm_l3_host_t l3_host;
	
	if(host) {
		/* create host entry */
		bcm_l3_host_t_init(&l3_host);
		l3_host.l3a_intf = dest_fec;
		l3_host.l3a_ip_addr = host;
		rc = bcm_l3_host_add(unit, l3_host);
		if(rc) { printf("Error-add_route-5: rc=%d\n", rc); return rc; }
	}

	if(mask != 0xffffffff || (mask & subnet) != host) {
		/* create routing entry */
		bcm_l3_route_t_init(&l3_route);
		l3_route.l3a_intf = dest_fec;
		l3_route.l3a_subnet = subnet;
		l3_route.l3a_ip_mask = mask;
		rc = bcm_l3_route_add(unit, &l3_route);
		if(rc) { printf("Error-add_route-10: rc=%d\n", rc); return rc; }
	}

	printf("L3_ROUTE: subnet=0x%08x, mask=0x%08x, host=0x%08x, dest_fec=0x%x\n", subnet, mask, host, dest_fec);
	return 0;
}

/*---------------------------------------------------------------------------------------------------------------
IP tunnel termination mode defined in config.bcm.
Supporting two option: 
1. DIP termination 
2. DIP&SIP termination
---------------------------------------------------------------------------------------------------------------*/
int ip_tunnel_terminator(int unit, bcm_port_t port, bcm_ip_t sip, bcm_ip_t dip, bcm_gport_t *tunnel_gport)
{
	bcm_tunnel_terminator_t tt;

	/* IPv4 termination */
	/* the L2TP is encasulated within UDPoIPv4 tunnel so we need to terminate the IPv4 tunnel */
	bcm_tunnel_terminator_t_init(&tt);

	tt.dip = dip;
	tt.dip_mask = -1;

	if(sip == 0) {
		/* dip_termination_mode (configured in config.bcm) requires sip_mask=0 */
		tt.sip_mask = 0;
	} else {
		/* this is for dip&sip termination mode */
		tt.sip = sip;
		tt.sip_mask = 0xffffffff;
	}
	
	tt.type = bcmTunnelTypeIpAnyIn4;
	tt.tunnel_if = -1;
	rc = bcm_tunnel_terminator_create(unit, &tt);
	if(rc) { printf("Error-ip_tunnel_terminator-10: rc=%d\n", rc); return rc; }
	*tunnel_gport = tt.tunnel_id;

	printf("IP_TERM: port=0x%x, sip=0x%08x, dip=0x%08x, tunnel_gport=0x%x\n", port, sip, dip, *tunnel_gport);
	
	return 0;
}

/*---------------------------------------------------------------------------------------------------------------
the l2tp payload is encapsulated within 3 layers
 - inner: L2TP tunnel
 - middle: UDP tunnel
 - outer_tunnel: IPv4 tunnel
This function initiates these tunnels, the out_lif chain is:
- IPv4
- L2TP 
- LL (Arp)
There is no egress object for UDP since we always use: udp_src_port=udp_dst_port=1701

L2TP egress object is a data_entry and therefore can't be the first in the chain of out_lifs. 
Because of this we start the chain with the IPv4 out_lif. This means that we need a IPv4 out_lif for each L2TP session even though 
these IPv4 out_lifs might be identical except for the pointer to the L2TP data_entry.
---------------------------------------------------------------------------------------------------------------*/
int ipv4_udp_l2tp_tunnel_initiator(int unit, bcm_port_t port, uint16 l2tp_tunnel, uint16 l2tp_session, 
												bcm_ip_t sip, bcm_ip_t dip, bcm_if_t arp_encap, 
												bcm_gport_t *ip_tunnel_if, bcm_gport_t *l2tp_tunnel_gport)
{
	bcm_l3_intf_t l3_tunnel_intf;
	bcm_tunnel_initiator_t ti;
    bcm_if_t ppp_initiator_intf_id;
	bcm_ppp_initiator_t info;

	/* create l2tp egress data_entry pointing to an ARP entry */
	sal_memset(&info, 0, sizeof(info));
	info.type = bcmPPPTypeL2TPv2;
	info.l3_intf_id = arp_encap;
	info.session_id = l2tp_session;
	info.l2tpv2_tunnel_id = l2tp_tunnel;		
	rc = bcm_ppp_initiator_create(unit, &info);
	if(rc) { printf("Error-ipv4_udp_l2tp_tunnel_initiator-14: rc=%d\n", rc); return rc; }

	BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(ppp_initiator_intf_id, info.ppp_initiator_id);
	*l2tp_tunnel_gport = info.ppp_initiator_id;

	/* create ipv4 tunnel pointing to the l2tp egress object */
    bcm_tunnel_initiator_t_init(&ti);
	ti.type = bcmTunnelTypeIpAnyIn4;
	ti.sip = sip;
	ti.dip = dip;
	ti.udp_dst_port = 1701; /* we only support dst_port=src_port=1701 */
	ti.udp_src_port = 1701;
	ti.ttl = 31;
	ti.dscp = 2;
	ti.dscp_sel = bcmTunnelDscpAssign;
	ti.l3_intf_id = ppp_initiator_intf_id; /* the l2tp data_entry */
    ti.encap_access = bcmEncapAccessTunnel1; /* if IPv4 out_lif is preceeding in the out_lif chain the L2TP object so we need to access it at phase 1*/
	bcm_l3_intf_t_init(&l3_tunnel_intf);
    rc = bcm_tunnel_initiator_create(unit, &l3_tunnel_intf, &ti);
	if(rc) { printf("Error-ipv4_udp_l2tp_tunnel_initiator-45: rc=%d\n", rc); return rc; }

	*ip_tunnel_if = l3_tunnel_intf.l3a_intf_id;

	printf("L2TP_INIT: port=0x%x, l2tp_tunnel:session=0x%04x:%04x, sip=0x%08x, dip=0x%08x, arp_encap=0x%x, l2tp_tunnel_gport=0x%x, ip_tunnel_if=0x%x\n", 
		port, l2tp_tunnel, l2tp_session, sip, dip, arp_encap, *l2tp_tunnel_gport, *ip_tunnel_if);

/*	
	printf("--------------------------------- OUT_LIF chain ---------------------------------\n");
	sprintf(Cmd, "diag pp lif gl=1 type=out id=0x%x", *ip_tunnel_if & 0xffff);
	bshell(unit, Cmd);
	printf("---------------------------------------------------------------------------------\n");
	sprintf(Cmd, "diag pp lif gl=1 type=out id=0x%x", *l2tp_tunnel_gport & 0xffff);
	bshell(unit, Cmd);
	printf("---------------------------------------------------------------------------------\n");
	sprintf(Cmd, "diag pp lif gl=1 type=out id=0x%x", arp_encap & 0xffff);
	bshell(unit, Cmd);
	printf("---------------------------------------------------------------------------------\n");
*/
	return 0;	
}

/*---------------------------------------------------------------------------------------------------------------
set 17 bits fec pointer into the lif wide data
---------------------------------------------------------------------------------------------------------------*/
int set_wide_lif_data_with_fec(int unit, bcm_gport_t lif_gport, uint32 lif_data)
{
	uint64 d64;
	uint32 d32[2];

	d64 = "1";
	sal_memcpy(d32, &d64, 8);
	
	/* check big/little endian */
	if(d32[0] == 1)
		d32[0] = lif_data & 0x1ffff; /* mask in only 17 bits */
	else
		d32[1] = lif_data & 0x1ffff; /* mask in only 17 bits */
	
	sal_memcpy(&d64, d32, 8);

	rc = bcm_port_wide_data_set(unit, lif_gport, BCM_PORT_WIDE_DATA_INGRESS, d64);
	if(rc) { printf("Error-set_wide_lif_data_with_fec-10: rc=%d\n", rc); return rc; }

	return rc;
}

/*---------------------------------------------------------------------------------------------------------------
create nni port for ip and for l2tp ports
 z=0 - ip
 z=2 - l2tp
Values for Vlan, Vsi, etc. are taken from the global vars
---------------------------------------------------------------------------------------------------------------*/
int init_port_attr_nni(int unit, int z, bcm_port_t port, int vrf)
{
	bcm_gport_t sysport_gport;
	bcm_port_match_info_t match_info;

	/* create ac vsi */
	rc = bcm_vswitch_create_with_id(unit, Vsi[z]);
	if(rc) { printf("Error-init_port_attr_nni-10: rc=%d\n", rc); return rc; }

	/* create ac_lif using PORTxVLAN */
	bcm_vlan_port_t_init(&Ac[z][0]);
	Ac[z][0].criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
	Ac[z][0].port = port;
	Ac[z][0].match_vlan = Vlan[z][0];
	Ac[z][0].egress_vlan = Vlan[z][0];
	Ac[z][0].flags = 0;/*BCM_VLAN_PORT_INGRESS_WIDE;*/
	rc = bcm_vlan_port_create(unit, &Ac[z][0]);
	if(rc) { printf("Error-init_port_attr_nni-20: rc=%d\n", rc); return rc; }

	/* attach lif to vsi */
	rc = bcm_vswitch_port_add(unit, Vsi[z], Ac[z][0].vlan_port_id);
	if(rc) { printf("Error-init_port_attr_nni-40: rc=%d\n", rc); return rc; }

	printf("NNI_AC: port=0x%x, vlan=0x%x, ac_lif=0x%x, ac_encap=0x%x\n", port, Vlan[z][0], Ac[z][0].vlan_port_id, Ac[z][0].encap_id);

	/* create entry in ESEM using PORTxVSI pointing to the Ac */
	/* the Ac will be selected by a lookup of PORTxVSI in ESEM */
	bcm_port_match_info_t_init(&match_info);
	match_info.match = BCM_PORT_MATCH_PORT_VLAN;
	match_info.flags = BCM_PORT_MATCH_EGRESS_ONLY;
	match_info.port = port;
	match_info.match_vlan = Vsi[z];
	rc = bcm_port_match_add(unit, Ac[z][0].vlan_port_id, &match_info);
	if(rc) { printf("Error-init_port_attr_nni-85: rc=%d\n", rc); return rc; }
	
	/* create RIF & ARP entry */

	/* for l2tp intf use vrf */
	rc = init_l3_intf(unit, Vsi[z], MyMac[z], NextHopMac[z], vrf, &Rif[z], &ArpEncap[z]);
	if(rc) { printf("Error-init_port_attr_nni-50: rc=%d\n", rc); return rc; }

	/* for adding routes we need FEC, so create the FEC */
	BCM_GPORT_SYSTEM_PORT_ID_SET(sysport_gport, port);
	rc = init_l3_fec(unit, sysport_gport, Rif[z], ArpEncap[z], &Fec[z]);
	if(rc) { printf("Error-init_port_attr_nni-90: rc=%d\n", rc); return rc; }
	
	/* add route */
	rc = add_route(unit, Subnet[z][0], Mask[z], Host[z][0], Fec[z]);
	if(rc) { printf("Error-init_port_attr_nni-95: rc=%d\n", rc); return rc; }
	
	return 0;
}

/*---------------------------------------------------------------------------------------------------------------
For L2TP interfaces we need to further configure the interface with this function.
For incoming L2TP, it is encapsulated within an IPv4 tunnel so we need to terminate the tunnel.
Tunnel termination mode is either DIP or DIP&SIP (mode is set in SOC properties)
---------------------------------------------------------------------------------------------------------------*/
int init_port_attr_nni_l2tp(int unit, int z, bcm_port_t port)
{
	bcm_ip_t sip;
	bcm_gport_t tunnel_gport;

	/* IPv4 termination */
	/* the L2TP is encasulated within UDPoIPv4 tunnel so we need to terminate the ingress IPv4 tunnel */

	if(Is_DIP_SIP_Termination) 
		sip = NLS_IP;
	else
		sip = 0;
		
	rc = ip_tunnel_terminator(unit, port, sip, MY_IP, &tunnel_gport);
	if(rc) { printf("Error-init_port_attr_nni_l2tp-10: rc=%d\n", rc); return rc; }
	IpTunnelTerminatorGport[z] = tunnel_gport;

	return 0;
}

/*---------------------------------------------------------------------------------------------------------------
For each L2TP tunnel/session we need to call this function.
The previous two functions init_port_attr_nni() & init_port_attr_nni_l2tp() are called once per port
while this function is called multiple times, once per tunnel/session instance.
This function will create the l2tp tunnel terminator using l2tp_tunnel, l2tp_session and vrf
Values for tunnel, session and vrf are taken from global vars
---------------------------------------------------------------------------------------------------------------*/
int init_session_attr_nni_l2tp(int unit, int z, bcm_port_t port, int idx)
{
	bcm_gport_t sysport_gport;
	bcm_ppp_terminator_t pt;

	/* create l2tp terminator */
	sal_memset(&pt, 0, sizeof(pt));
	pt.type = bcmPPPTypeL2TPv2;
	pt.session_id = L2tpSession[idx];
	pt.l2tpv2_tunnel_id = L2tpTunnel;
	pt.network_domain = L2tpVrf;
	pt.flags = BCM_PPP_TERM_WIDE | BCM_PPP_TERM_CROSS_CONNECT;
	rc = bcm_ppp_terminator_create(unit, &pt);
	if(rc) { printf("Error-init_session_attr_nni_l2tp-13: rc=%d\n", rc); return rc; }
	L2tpTerminatorGport[z][idx] = pt.ppp_terminator_id;

	printf("L2TP_TERM: tunnel_gport=0x%x\n", pt.ppp_terminator_id);

	/* create l2tp initiator */
	/* the payload should be encapsulated within L2TP tunnel which is to be encasulated within UDPoIPv4 tunnel */
	/* we need to initiate these two egress tunneles */
	rc = ipv4_udp_l2tp_tunnel_initiator(unit, port, L2tpTunnel, L2tpSession[idx], MY_IP, NLS_IP, ArpEncap[z], &IpTunnelInitiatorIf[z][idx], &L2tpTunnelInitiatorGport[z][idx]);
	if(rc) { printf("Error-init_session_attr_nni_l2tp-40: rc=%d\n", rc); return rc; }

	/* create fec-pointer pointing to the port & out-lif */
	BCM_GPORT_SYSTEM_PORT_ID_SET(sysport_gport, port);
	rc = init_l3_fec(unit, sysport_gport, IpTunnelInitiatorIf[z][idx], 0, &L2tpFec[z][idx]);
	if(rc) { printf("Error-uni2-40: rc=%d\n", rc); return rc; }

	printf("L2TP_FEC: port=0x%x, ip_tunnel_if=0x%x, l2tp_fec_if=0x%x\n", 
		port, IpTunnelInitiatorIf[z][idx], L2tpFec[z][idx]);

	return 0;
}

/*---------------------------------------------------------------------------------------------------------------

---------------------------------------------------------------------------------------------------------------*/
int del_session_attr_nni(int unit, int z, int action, bcm_port_t port, int idx)
{
	bcm_l3_intf_t l3_tunnel_intf;
	bcm_ppp_terminator_t pt;
	bcm_ppp_initiator_t pi;
	
	if(action == 1) {
		if(L2tpTerminatorGport[z][idx]) {
			printf("Delete l2tp tunnel terminator, gport=0x%x\n", L2tpTerminatorGport[z][idx]);
			pt.ppp_terminator_id = L2tpTerminatorGport[z][idx];
			rc = bcm_ppp_terminator_delete(unit, &pt);
			if(rc) { printf("Error-del_session_attr_nni-10: rc=%d\n", rc); return rc; }
			L2tpTerminatorGport[z][idx] = 0;
		}
	} else if(action == 2) {
		if(IpTunnelInitiatorIf[z][idx]) {
			printf("Delete IP tunnel initiator, if=0x%x\n", IpTunnelInitiatorIf[z][idx]);
			bcm_l3_intf_t_init(&l3_tunnel_intf);
			l3_tunnel_intf.l3a_intf_id = IpTunnelInitiatorIf[z][idx];
			rc = bcm_tunnel_initiator_clear(unit, &l3_tunnel_intf);
			if(rc) { printf("Error-del_session_attr_nni-15: rc=%d\n", rc); return rc; }
			IpTunnelInitiatorIf[z][idx] = 0;
		}
	} else if(action == 3) {
		if(L2tpTunnelInitiatorGport[z][idx]) {
			printf("Delete L2TP tunnel initiator, gport=0x%x\n", L2tpTunnelInitiatorGport[z][idx]);
			pi.ppp_initiator_id = L2tpTunnelInitiatorGport[z][idx];
			rc = bcm_ppp_initiator_delete(unit, &pi);
			if(rc) { printf("Error-del_session_attr_nni-20: rc=%d\n", rc); return rc; }
			L2tpTunnelInitiatorGport[z][idx] = 0;
		}
	} else {
		printf("invalidAction=%d\n", action);
		return 1;
	}

	return 0;
}

/*---------------------------------------------------------------------------------------------------------------
this will create a PPPoE UNI port with the following configuration:
this function is called once per port. The next function init_session_attr_uni() is called once per each
Values for Vlan, Vsi, etc. are taken form the global vars.
For PPPoE port z=3
PPPoE session index.
---------------------------------------------------------------------------------------------------------------*/
int init_port_attr_uni(int unit, int z, bcm_port_t port)
{
	/* create pppoe vsi */
	rc = bcm_vswitch_create_with_id(unit, Vsi[z]);
	if(rc) { printf("Error-init_port_attr_uni-10: rc=%d\n", rc); return rc; }

	printf("UNI: port=0x%x, vsi=0x%x\n", port, Vsi[z]);

	/* create RIF */
	rc = init_l3_intf(unit, Vsi[z], MyMac[z], NextHopMac[z], 0, &Rif[z], &ArpEncap[z]);
	if(rc) { printf("Error-init_port_attr_uni-30: rc=%d\n", rc); return rc; }

	return 0;
}

/*---------------------------------------------------------------------------------------------------------------
This function is called once per each PPPoE session.
Example values are for idx=3

L2:
- pppoe_session = 0xcb00 + running_index = 0xcb03
- vlan = 0xcb0 + running_index = 0xcb3
- host_ip=10.10.0xcb.0x40+idx
---------------------------------------------------------------------------------------------------------------*/
int init_session_attr_uni(int unit, int z, bcm_port_t port, int idx)
{
	bcm_gport_t fec_gport, dest_gport;

	/* create pppoe lif with fec */
	bcm_vlan_port_t_init(&Ac[z][idx]);
	Ac[z][idx].criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
	Ac[z][idx].port = port;
	Ac[z][idx].match_vlan = Vlan[z][idx];
	Ac[z][idx].egress_vlan = Vlan[z][idx];
    Ac[z][idx].egress_tunnel_value = PppoeSession[idx];
	Ac[z][idx].flags = BCM_VLAN_PORT_FORWARD_GROUP | BCM_VLAN_PORT_INGRESS_WIDE;

	rc = bcm_vlan_port_create(unit, &Ac[z][idx]);
	if(rc) { printf("Error-init_session_attr_uni-20: rc=%d\n", rc); return rc; }

	printf("UNI_AC: port=0x%x, uni_vlan=0x%x, ac_fec_gport=0x%x, ac_encap=0x%x\n", 
		port, Vlan[z][idx], Ac[z][idx].vlan_port_id, Ac[z][idx].encap_id);

	/* attach lif to vsi */
	rc = bcm_vswitch_port_add(unit, Vsi[z], Ac[z][idx].vlan_port_id);
	if(rc) { printf("Error-init_session_attr_uni-25: rc=%d\n", rc); return rc; }

	BCM_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(fec_gport, (Ac[z][idx].vlan_port_id & 0xFFFF | (1<<29)));
    if(!is_device_or_above(unit, JERICHO_B0)) {
		/* for older devices we need to create a super-fec for building hierarchial fec */
		rc = init_l3_super_fec(unit, fec_gport, Rif[z], ArpEncap[z], &PppoeFec[z][idx]);
		if(rc) { printf("Error-init_session_attr_uni-40: rc=%d\n", rc); return rc; }
   	} else {
		/* translate VLAN_PORT_FORWARD_GROUP to FEC */
		rc = init_l3_fec(unit, fec_gport, Rif[z], ArpEncap[z], &PppoeFec[z][idx]);
	}

	/* add route */
	rc = add_route(unit, Subnet[z][idx], Mask[z], Host[z][idx], PppoeFec[z][idx]);
	if(rc) { printf("Error-init_session_attr_uni-50: rc=%d\n", rc); return rc; }

	return 0;
}

/*---------------------------------------------------------------------------------------------------------------
This function is called per each PPPoE session instance.
It will terminate the pppoe tunnel to be cross-connected to l2tp
- pppoe_session = 0xcb00 + running_index = 0xcb03
---------------------------------------------------------------------------------------------------------------*/
int init_session_attr_uni_pppoe(int unit, int z, bcm_port_t port, int idx)
{
	bcm_ppp_terminator_t pt;

	sal_memset(&pt, 0, sizeof(pt));
	pt.type = bcmPPPTypePPPoE;
	pt.session_id = PppoeSession[idx];
	pt.network_domain = Ac[z][idx].vlan_port_id;
	pt.ppp_terminator_id = Ac[z][idx].vlan_port_id;
	pt.flags = BCM_PPP_TERM_WIDE | BCM_PPP_TERM_SESSION_ANTI_SPOOFING | BCM_PPP_TERM_CROSS_CONNECT;

	rc = bcm_ppp_terminator_create(unit, &pt);
	if(rc) { printf("Error-init_session_attr_uni_pppoe-18: rc=%d\n", rc); return rc; }
	PppoeTerminatorGport[z][idx] = pt.ppp_terminator_id;

	printf("PPPoE_TERM: port=0x%x, pppoe_session=0x%04x, tunnel_gport=0x%x\n", port, PppoeSession[idx], pt.ppp_terminator_id);

	return 0;
}

/*---------------------------------------------------------------------------------------------------------------

---------------------------------------------------------------------------------------------------------------*/
int del_session_attr_uni(int unit, int z, bcm_port_t port, int idx)
{
	bcm_ppp_terminator_t pt;

	if(PppoeTerminatorGport[z][idx]) {
		printf("Delete pppoe tunnel terminator, gport=0x%x\n", PppoeTerminatorGport[z][idx]);
		pt.ppp_terminator_id = PppoeTerminatorGport[z][idx];
		rc = bcm_ppp_terminator_delete(unit, &pt);
		if(rc) { printf("Error-del_session_attr_uni-10: rc=%d\n", rc); return rc; }
		PppoeTerminatorGport[z][idx] = 0;
	}

	return 0;
}

/*---------------------------------------------------------------------------------------------------------------

---------------------------------------------------------------------------------------------------------------*/
int x_connect(int unit)
{
	int idx;

	for(idx = 0; idx < 16; idx++) {

		/* x-connect pppoe-->l2tp */
		if(PppoeTerminatorGport[3][idx] && L2tpFec[2][idx]) {
			set_wide_lif_data_with_fec(unit, PppoeTerminatorGport[3][idx], L2tpFec[2][idx]);
			if(rc) { printf("Error-x_connect: rc=%d\n", rc); return rc; }
			printf("X_CON: pppoe_term_gport[%d]=0x%x --> data=l2tp_fec[%d]=0x%x\n", idx, PppoeTerminatorGport[3][idx], idx, L2tpFec[2][idx]);
		}

		if(L2tpTerminatorGport[2][idx] && PppoeFec[3][idx]) {
			/* x-connect pppoe-->l2tp */
			set_wide_lif_data_with_fec(unit, L2tpTerminatorGport[2][idx], PppoeFec[3][idx]);
			if(rc) { printf("Error-x_connect: rc=%d\n", rc); return rc; }
			printf("X_CON: l2tp_term_gport[%d]=0x%x --> pppoe_fec[%d]=0x%x\n", idx, L2tpTerminatorGport[2][idx], idx, PppoeFec[3][idx]);
		}
	}

	return 0;
}

/*---------------------------------------------------------------------------------------------------------------

---------------------------------------------------------------------------------------------------------------*/
int is_hex(char h)
{
	if(h >= '0' && h <= '9' || h >= 'a' && h <= 'f'  || h >= 'A' && h <= 'F')
		return 1;
	else
		return 0;
}

/*---------------------------------------------------------------------------------------------------------------

---------------------------------------------------------------------------------------------------------------*/
int is_lc_hex(char h)
{
	if(h >= '0' && h <= '9' || h >= 'a' && h <= 'f')
		return 1;
	else
		return 0;
}

/*---------------------------------------------------------------------------------------------------------------

---------------------------------------------------------------------------------------------------------------*/
char *qsp(char *msg)
{
	int i, j, header = 1;

	printf("%s\n", msg);
	
	i = 0;
	j = 0;

	while(msg[i] != '\0') {

		if(is_lc_hex(msg[i]) || header)
			Msg[j++] = msg[i];

		if(i >= 4 && msg[i-4] == 'd' && msg[i-3] == 'a' && msg[i-2] == 't' && msg[i-1] == 'a' && msg[i] == '=')
			header = 0;
		
		i++;
	}

	Msg[j] = '\0';

	/*printf("%s\n", Msg); */
	
	return Msg;
}

/*---------------------------------------------------------------------------------------------------------------

---------------------------------------------------------------------------------------------------------------*/
int tx_ip(int unit, uint32 mask)
{
	printf("\n\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> IP to PPPOE <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n\n");
	
	/* inject ip packet to port 200 that will be forwarded to port 203 and encapsulated within pppoe */
	/* call 4 times, once per pppoe session */
	if(mask & 1) bshell(unit, qsp("tx 1 psrc=200 len=128 data=001122334.168.661122334.001 8100.0.168 0800 IP={45.00.0028.6c13.4000.3f.11.df4c.0a0a1111.0a0acb40} UDP={1000.1001.000a.0000} 1234.aa55aa55aa55aa55.aa55aa55aa55aa55"));
	
	if(mask & 2) bshell(unit, qsp("tx 1 psrc=200 len=128 data=001122334.168.661122334.001 8100.0.168 0800 IP={45.00.0028.6c13.4000.3f.11.df4b.0a0a1111.0a0acb41} UDP={10001.001.000a.0000} 1234.aa55aa55aa55aa55.aa55aa55aa55aa55"));

	if(mask & 4) bshell(unit, qsp("tx 1 psrc=200 len=128 data=001122334.168.661122334.001 8100.0.168 0800 IP={45.00.0028.6c13.4000.3f.11.df4a.0a0a1111.0a0acb42} UDP={10001.001.000a.0000} 1234.aa55aa55aa55aa55.aa55aa55aa55aa55"));

	if(mask & 8) bshell(unit, qsp("tx 1 psrc=200 len=128 data=001122334.168.661122334.001 8100.0.168 0800 IP={45.00.0028.6c13.4000.3f.11.df49.0a0a1111.0a0acb43} UDP={10001.001.000a.0000} 1234.aa55aa55aa55aa55.aa55aa55aa55aa55"));

	return 0;
}

/*---------------------------------------------------------------------------------------------------------------

---------------------------------------------------------------------------------------------------------------*/
int tx_l2tp(int unit, uint32 mask)
{
	printf("\n\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> L2TP to PPPOE <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n\n");

	/* NNI(port#202-l2tp)-->UNI(port#203-pppoe) */
	/* inject to port 202 l2tp packet to be forwarded to pppoe port 203 */
	/* call 4 times, once per l2tp session */
	/* the payload should be forwarded as is regardless if it is IPv4, IPv6 or something else */

	if(mask & 1) 
	/* IPv4oL2TP, PPP-Protocol-ID=0x0021, l2tp_option=none */
 	/*                                                                               Ether IPv4-tunnel:                                                 UDP-tunnel:         L2TP:          PPP- PPP-  IPv4:                                                        IPv4- */
    /*                                        DA            SA            Vtag       Type     tos len  id        ttl proto csum SIP         DIP         src  dst  len  csum      tunn sess Pref proto    tos len  id        ttl proto csum SIP         DIP         payload */
	bshell(unit, qsp("tx 1 psrc=202 len=128 data=001122334.16a 661122334.001 8100-0.16a 0800  45.14..0042.6c13.4000.3f..11....8a3b-0a.0a.66.ca-0a.0a.ca.6a 06a5.06a5.0014.0000 0002.00ca.0000 ff03 0021  45.04..001c.6c13.4000.3f..11....e01e-0a.0a.11.11-0a.0a.ca.6a FFFF12345678FFFF"));

	if(mask & 2) 
	/* IPv6oL2TP, PPP-Protocol-ID=0x0057, l2tp_option=SP */
 	/*                                                                               Ether IPv4-tunnel:                                                 UDP-tunnel:         L2TP:                    PPP- PPP-  IPv4(should be IPv6):                                        IPv4- */
    /*                                        DA            SA            Vtag       Type     tos len  id        ttl proto csum SIP         DIP         src  dst  len  csum SP   tunn sess Ns   Nr   Pref proto    tos len  id        ttl proto csum SIP         DIP         payload */
	bshell(unit, qsp("tx 1 psrc=202 len=128 data=001122334.16a 661122334.001 8100-0.16a 0800  45.14..0042.6c13.4000.3f..11....8a3b-0a.0a.66.ca-0a.0a.ca.6a 06a5.06a5.0014.0000 0902.00ca.0000 0022 0033 ff03 0057  45.04..001c.6c13.4000.3f..11....e01e-0a.0a.11.11-0a.0a.ca.6a FFFF12345678FFFF"));

	if(mask & 4)
	/* Generic payload over L2TP, PPP-Protocol-ID=0x0058, l2tp_option=OS */
 	/*                                                                               Ether IPv4-tunnel:                                                 UDP-tunnel:         L2TP:                          PPP- PPP-   */
    /*                                        DA            SA            Vtag       Type     tos len  id        ttl proto csum SIP         DIP         src  dst  len  csum OS   tunn sess Ns   Nr   offst Pref proto payload */
	bshell(unit, qsp("tx 1 psrc=202 len=128 data=001122334.16a 661122334.001 8100-0.16a 0800  45.14..0042.6c13.4000.3f..11....8a3b-0a.0a.66.ca-0a.0a.ca.6a 06a5.06a5.0014.0000 0a02.00ca.0000 0022 0033 4444  ff03 0058  12345678901234567890123456789012345678901234567890123456"));

	if(mask & 8)
	/* Generic payload over L2TP, PPP-Protocol-ID=0xc0de, l2tp_option=LP */
 	/*                                                                               Ether IPv4-tunnel:                                                 UDP-tunnel:         L2TP:               PPP- PPP-   */
    /*                                        DA            SA            Vtag       Type     tos len  id        ttl proto csum SIP         DIP         src  dst  len  csum LP   Len  tunn sess Pref proto payload */
	bshell(unit, qsp("tx 1 psrc=202 len=128 data=001122334.16a 661122334.001 8100-0.16a 0800  45.14..0042.6c13.4000.3f..11....8a3b-0a.0a.66.ca-0a.0a.ca.6a 06a5.06a5.0014.0000 4102.0000.00ca.0000 ff03 c0de  1111deaddeaddeaddeaddeaddeaddeaddeaddeaddeaddeaddead1111"));

	if(mask & 16)
	/* Generic payload over L2TP, PPP-Protocol-ID=0xc0de, l2tp_option=OP */
	/*																				 Ether IPv4-tunnel: 												UDP-tunnel: 		L2TP:		        PPP- PPP-   */
	/*										  DA			SA			  Vtag		 Type	  tos len  id		 ttl proto csum SIP 		DIP 		src  dst  len  csum OP   tunn sess ofst Pref proto payload */
	bshell(unit, qsp("tx 1 psrc=202 len=128 data=001122334.16a 661122334.001 8100-0.16a 0800  45.14..0042.6c13.4000.3f..11....8a3b-0a.0a.66.ca-0a.0a.ca.6a 06a5.06a5.0014.0000 0302.00ca.0000.0000 ff03 c0de  1111deaddeaddeaddeaddeaddeaddeaddeaddeaddeaddeaddead1111"));

	return 0;
}

/*---------------------------------------------------------------------------------------------------------------

---------------------------------------------------------------------------------------------------------------*/
int tx_pppoe(int unit, uint32 mask)
{
	printf("\n\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> PPPOE to L2TP <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n\n");

	/* UNI(port#203-pppoe)-->NNI(port#202-l2tp) */
	/* inject to port 203 pppoe packets with pppoe-sessions 0xcb00, 0xcb01, 0xcb02 and 0xcb03 */

	if(mask & 1) 
	/* pppoe-session=0xcb00,ppp-poro=0x0021 should be tunneled to port 202 */
	/*																				 Ether PPPoE:			   IPv4:					 ip- */
	/*									      DA			SA			  Vtag		 Type		Sess len     PPP	 tos len  id		ttl proto csum SIP 		DIP 		IP-payload */
	bshell(unit, qsp("tx 1 psrc=203 len=130 data=001122334.26b 8811223344.cb 8100-0.cb0 8864  1100-cb00-0046 0021 45.14..0042.6c13.4000.3f..11....8bdc-0a.0a.66.cb-0a.0a.c8.c8 ffff12345678ffff"));

	if(mask & 2) 
	/* pppoe-session=0xcb02,ppp-poro=0x0021 should be decapsulated from pppoe and forwarded to port 200 */
	/*																				 Ether PPPoE:			   IPv4:					 ip- */
	/*									      DA			SA			  Vtag		 Type		Sess len  PPP	  tos len  id		 ttl proto csum SIP 		DIP 		IP-payload */
	bshell(unit, qsp("tx 1 psrc=203 len=128 data=001122334.26b 8811223344.cb 8100-0.cb1 8864  1100-cb01-0046 0021 45.14..0042.6c13.4000.3f..11....8bdc-0a.0a.66.cb-0a.0a.c8.c8 ffff12345678ffff"));

	if(mask & 4) 
	/* check here the anti-spoofing by using pppoe_session=0xcb00 - packet should go to ip*/
	/* pppoe-session=0xcb01,ppp-poro=0x0055 should be decapsulated from pppoe and forwarded to port 200 */
	/*																				 Ether PPPoE:			    */
	/*										  DA			SA			  Vtag		 Type		Sess len  PPP  payload */
	bshell(unit, qsp("tx 1 psrc=203 len=128 data=001122334.26b 8811223344.cb 8100-0.cb2 8864  1100-cb00-0046 0021 45.14..0042.6c13.4000.3f..11....8bdc-0a.0a.66.cb-0a.0a.c8.c8 ffff12345678ffff"));

	return 0;
}

/*---------------------------------------------------------------------------------------------------------------

---------------------------------------------------------------------------------------------------------------*/
int config_pmf(int unit)
{
	bcm_field_entry_t entry;
	uint8 l2tp_options_mask;
	
	Unit = unit;

	rc = init_cross_connect_fg(unit);
	if(rc) { printf("Error-config_pmf-10: rc=%d\n", rc); return rc; }

	rc = init_l2tp_options_fg(unit);
	if(rc) { printf("Error-config_pmf-10: rc=%d\n", rc); return rc; }

	/*
	 * set actions for l2tp header options
	 */

	/* we look only for T, S, O and P options, L is handled by the parser and need to be masked off. */
	l2tp_options_mask = T_BIT | S_BIT | O_BIT | P_BIT;

	/* strip 4 bytes if SBit=1 */
	rc = add_l2tp_options_actions(unit, 1, l2tp_options_mask, S_BIT, 4, -1, &entry);
	if(rc) { printf("Error-config_pmf-10: rc=%d\n", rc); return rc; }

	/* strip 2 bytes if OBit=1 */
	rc = add_l2tp_options_actions(unit, 1, l2tp_options_mask, O_BIT, 2, -1, &entry);
	if(rc) { printf("Error-config_pmf-10: rc=%d\n", rc); return rc; }

	/* strip 6 bytes if SBit=1 and OBit=1 */
	rc = add_l2tp_options_actions(unit, 1, l2tp_options_mask, S_BIT | O_BIT, 6, -1, &entry);
	if(rc) { printf("Error-config_pmf-10: rc=%d\n", rc); return rc; }

	/* don't strip if all options=0, set high_prio if PBit=1 */
	rc = add_l2tp_options_actions(unit, 1, l2tp_options_mask, P_BIT, 0, -1, &entry);
	if(rc) { printf("Error-config_pmf-10: rc=%d\n", rc); return rc; }

	/* strip 4 bytes if SBit=1, set high_prio if PBit=1 */
	rc = add_l2tp_options_actions(unit, 1, l2tp_options_mask, S_BIT | P_BIT, 4, -1, &entry);
	if(rc) { printf("Error-config_pmf-10: rc=%d\n", rc); return rc; }

	/* strip 2 bytes if OBit=1, set high_prio if PBit=1 */
	rc = add_l2tp_options_actions(unit, 1, l2tp_options_mask, O_BIT | P_BIT, 2, -1, &entry);
	if(rc) { printf("Error-config_pmf-10: rc=%d\n", rc); return rc; }

	/* strip 6 bytes if SBit=1 and OBit=1, set high_prio if PBit=1 */
	rc = add_l2tp_options_actions(unit, 1, l2tp_options_mask, S_BIT | O_BIT | P_BIT, 6, -1, &entry);
	if(rc) { printf("Error-config_pmf-10: rc=%d\n", rc); return rc; }

	/* don't don't strip if all options=0, just set fwd_type & fwd_header for L2TP--->PPPoE */
	rc = add_l2tp_options_actions(unit, 1, l2tp_options_mask, 0, 0, -1, &entry);
	if(rc) { printf("Error-config_pmf-10: rc=%d\n", rc); return rc; }

	/* set fwd_type & fwd_header for PPPoE--->L2TP */
	rc = add_l2tp_options_actions(unit, 0, 0, 0, 0, -1, &entry);
	if(rc) { printf("Error-config_pmf-10: rc=%d\n", rc); return rc; }

	return 0;
}

/*---------------------------------------------------------------------------------------------------------------

---------------------------------------------------------------------------------------------------------------*/
int delete_tunnels(int unit)
{
	int idx;
	int action;

	for(action = 0; action < 4; action++) {

		if(action == 0) {
			for(idx = 0; idx < 16; idx++) {
				rc = del_session_attr_uni(unit, pppoe_idx, 203, idx);
				if(rc) { printf("Error-delete_tunnels-20: rc=%d\n", rc); return rc; }
			}
		} else if(action == 1 || action == 2 || action == 3) {
			/* 1: delete l2tp sessions, calling once per session */
			/* 2: del IP tunnel initiator */
			/* 3: del L2TP tunnel initiator */
			for(idx = 0; idx < 16; idx++) {
				rc = del_session_attr_nni(unit, l2tp_idx, action, 202, idx);
				if(rc) { printf("Error-delete_tunnels-30: rc=%d\n", rc); return rc; }
			}
		}
	}

	return 0;
}

/*---------------------------------------------------------------------------------------------------------------

---------------------------------------------------------------------------------------------------------------*/
int destroy_all(int unit)
{
	int z, i;
	
	rc = delete_tunnels(Unit);
	if(rc) { printf("Error-destroy_all-10: rc=%d\n", rc); return rc; }

	bcm_field_group_flush(Unit, L2tpOptionsFG.group);
	if(rc) { printf("Error-destroy_all-20: rc=%d\n", rc); return rc; }
	bcm_field_group_flush(Unit, CrossConnectFG.group);
	if(rc) { printf("Error-destroy_all-30: rc=%d\n", rc); return rc; }
	printf("Deleted field groups.\n");

	return 0;
}

/*---------------------------------------------------------------------------------------------------------------

---------------------------------------------------------------------------------------------------------------*/
int config_ports(int unit, bcm_port_t uni_port, bcm_port_t nni_port_l2tp, bcm_port_t nni_port_ip)
{
	/* intialize the global attributes */
	rc = init_l2_global_attr(unit);
	if(rc) { printf("Error-config_ports-10: rc=%d\n", rc); return rc; }

	/* initialize the l2 attributes of the ports */
	rc = init_l2_port_attr(unit, nni_port_ip);
	if(rc) { printf("Error-config_ports-15: rc=%d\n", rc); return rc; }
	rc = init_l2_port_attr(unit, nni_port_l2tp);
	if(rc) { printf("Error-config_ports-20: rc=%d\n", rc); return rc; }
	rc = init_l2_port_attr(unit, uni_port);
	if(rc) { printf("Error-config_ports-25: rc=%d\n", rc); return rc; }

	/* IP port */
	rc = init_port_attr_nni(unit, ip_idx, nni_port_ip, 0);
	if(rc) { printf("Error-config_ports-30: rc=%d\n", rc); return rc; }
	
	/* pppoe port */
	rc = init_port_attr_uni(unit, pppoe_idx, uni_port);
	if(rc) { printf("Error-config_ports-35: rc=%d\n", rc); return rc; }

	/* configure pppoe ac per pppoe session */
	rc = init_session_attr_uni(unit, pppoe_idx, uni_port, 0);
	if(rc) { printf("Error-config_ports-40: rc=%d\n", rc); return rc; }
	rc = init_session_attr_uni(unit, pppoe_idx, uni_port, 1);
	if(rc) { printf("Error-config_ports-45: rc=%d\n", rc); return rc; }
	rc = init_session_attr_uni(unit, pppoe_idx, uni_port, 2);
	if(rc) { printf("Error-config_ports-50: rc=%d\n", rc); return rc; }
	rc = init_session_attr_uni(unit, pppoe_idx, uni_port, 3);
	if(rc) { printf("Error-config_ports-55: rc=%d\n", rc); return rc; }
	
	/* configure pppoe tunnel terminator per pppoe_session that is tunneled to l2tp */
	rc = init_session_attr_uni_pppoe(unit, pppoe_idx, uni_port, 0);
	if(rc) { printf("Error-config_ports-60: rc=%d\n", rc); return rc; }
	
	/* L2TP port */
	rc = init_port_attr_nni(unit, l2tp_idx, nni_port_l2tp, L2tpVrf);
	if(rc) { printf("Error-config_ports-65: rc=%d\n", rc); return rc; }
	/* create the ip tunnel terminator */
	rc = init_port_attr_nni_l2tp(unit, l2tp_idx, nni_port_l2tp);
	if(rc) { printf("Error-config_ports-70: rc=%d\n", rc); return rc; }
	
	/* configure l2tp tunnel terminator & initiator per tunnel&session that is tunneled to PPPoE */
	rc = init_session_attr_nni_l2tp(unit, l2tp_idx, nni_port_l2tp, 0);
	if(rc) { printf("Error-config_ports-75: rc=%d\n", rc); return rc; }
	
	return 0;
}

/*---------------------------------------------------------------------------------------------------------------

---------------------------------------------------------------------------------------------------------------*/
int main(int unit)
{
	bshell(unit, "debug bcm rx verbose");
	
	/* set vrf */
	L2tpVrf = 100;
	
	/* set ip_tunnel_termination mode according to soc properties */
	Is_DIP_SIP_Termination = 0;
	
	/* configure everything */
	rc = config_pmf(unit);
	if(rc) { printf("Error-main-10: rc=%d\n", rc); return rc; }

	rc = config_ports(unit, 203, 202, 200);
	if(rc) { printf("Error-main-20: rc=%d\n", rc); return rc; }

	rc = x_connect(unit);
	if(rc) { printf("Error-main-30: rc=%d\n", rc); return rc; }
	
	/* send 4 ip packets, all should go to pppoe with session numbers 0xcb00..0xcb03 */
	rc = tx_ip(unit, 0x0f);
	if(rc) { printf("Error-main-40: rc=%d\n", rc); return rc; }
	
	/* send 3 pppoe packets
	 * 1st packet should be tunneled to L2TP with pppoe_session=0xcb00-->l2tp_tunnel:session=0xca00:0000
	 * 2nd packet should be go to IP since PPPoE termination to L2TP is not defined for session 0xcb01
	 * 3rd packet should go to IP since we are trying to spoof pppoe_session=0xcb00 on packet with vlan=0xcb0 instead of vlan=0xcb0
	 */
	rc = tx_pppoe(unit, 0x07);
	if(rc) { printf("Error-main-50: rc=%d\n", rc); return rc; }
	
	/* 
	 * send 5 l2tp pkts (tunnel:session=0xca00:0000)
	 * all should be tunneled to pppoe (l2tp_tunnel:session=0xca00:0000-->pppoe_session=0xcb00)
	 * pks have different l2tp_options and different PPP protocol IDs
	 */
	rc = tx_l2tp(unit, 0x01f);
	if(rc) { printf("Error-main-60: rc=%d\n", rc); return rc; }
	
	/* delete all tunnels */
	rc = delete_tunnels(unit);
	if(rc) { printf("Error-main-70: rc=%d\n", rc); return rc; }

	/* delete field groups */
	rc = bcm_field_group_flush(Unit, L2tpOptionsFG.group);
	if(rc) { printf("Error-main-75: rc=%d\n", rc); return rc; }
	rc = bcm_field_group_flush(Unit, CrossConnectFG.group);
	if(rc) { printf("Error-main-76: rc=%d\n", rc); return rc; }
	printf("Deleted field groups.\n");

	return 0;
}

