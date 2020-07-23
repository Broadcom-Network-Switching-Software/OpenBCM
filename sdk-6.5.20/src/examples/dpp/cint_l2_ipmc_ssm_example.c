/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * SOC property:
 *    custom_feature_ipmc_l2_ssm_mode_lem=1
 *

 cint ../../../../src/examples/dpp/cint_l2_ipmc_ssm.c
 cint ../../../../src/examples/dpp/cint_l2_ipmc_ssm_example.c

 cint
 int unit=0;
 int in_port = 200;
 int ipmc_out_port = 201;
 int ipmc_ssm_out_port = 202;
 int out_port3 = 203;
 print l2_ipmc_init_example(unit, BCM_FIELD_ENTRY_PRIO_LOWEST);
 print l2_ipmc_example(unit);
 exit;

debug rx +

# IPV4
# match DIP only, mc_id=4096
tx 1 psrc=200 length=128 data=0x01005e0022220011223344558100006408004500006e0000000040ff685def001111ef002222
# miss -- drop packet
tx 1 psrc=200 length=128 data=0x01005e0022230011223344558100006408004500006e0000000040ff685def001111ef002223

# IPV4 SMM
# match dip, sip, mc_id=8192
tx 1 psrc=200 length=128 data=0x01005e0022220011223344558100006408004500006e0000000040ff685d01020304e8002222
# miss dip, drop
tx 1 psrc=200 length=128 data=0x01005e0022230011223344558100006408004500006e0000000040ff685d01020304e8002223
# miss sip, drop
tx 1 psrc=200 length=128 data=0x01005e0022220011223344558100006408004500006e0000000040ff685d01020305e8002222

# IPV6 SSM
# match DIP, SIP, mc_id=8192
tx 1 psrc=200 length=128 data=0x3333000044440011111111118100006486dd6030000000063bfffe80000000000000021111fffe111111ff305555666666667777777700004444
# miss dip, drop
tx 1 psrc=200 length=128 data=0x3333000044430011111111118100006486dd6030000000063bfffe80000000000000021111fffe111111ff305555666666667777777700004443
#miss sip, drop
tx 1 psrc=200 length=128 data=0x3333000044440011111111128100006486dd6030000000063bfffe80000000000000021111fffe111111ff305555666666667777777700004444

 cint
 print l2_ipmc_dump_example(unit);
 exit;

 cint
 print l2_ipmc_teardown_example(unit);
 exit;
 */

int l2_ipmc_ipv4_ssm_entry_add_example(int unit, 
					bcm_ipmc_addr_t *ipmc_addr)
{
    int			rv = BCM_E_NONE;
    bcm_ipmc_addr_t	ip4_ipmc_addr;
    bcm_ipmc_addr_t	ip4_ipmc_ssm_addr;

    bcm_ipmc_addr_t_init(&ip4_ipmc_addr);
    ip4_ipmc_addr.vid = ipmc_addr->vid;
    ip4_ipmc_addr.mc_ip_addr = ipmc_addr->mc_ip_addr;
    ip4_ipmc_addr.group = BCM_MULTICAST_INVALID;
    ip4_ipmc_addr.flags = BCM_IPMC_L2;
    rv = bcm_ipmc_add(unit, &ip4_ipmc_addr);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_ipmc_add, rv %d\n", rv);
	return rv;
    }

    bcm_ipmc_addr_t_init(&ip4_ipmc_ssm_addr);
    ip4_ipmc_ssm_addr.vid = ipmc_addr->vid;
    ip4_ipmc_ssm_addr.s_ip_addr = ipmc_addr->s_ip_addr;
    ip4_ipmc_ssm_addr.mc_ip_addr = ipmc_addr->mc_ip_addr;
    ip4_ipmc_ssm_addr.group = ipmc_addr->group;
    ip4_ipmc_ssm_addr.flags = BCM_IPMC_L2;
    rv = bcm_ipmc_add(unit, &ip4_ipmc_ssm_addr);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_ipmc_add, rv %d\n", rv);
	return rv;
    }

    return rv;
}

int l2_ipmc_ipv4_ssm_entry_dump_example(int unit, bcm_ipmc_addr_t *ipmc_addr)
{
    int                 rv = BCM_E_NONE;
    bcm_ipmc_addr_t     ip4_ipmc_addr;
    bcm_ipmc_addr_t     ip4_ipmc_ssm_addr;

    bcm_ipmc_addr_t_init(&ip4_ipmc_addr);
    ip4_ipmc_addr.flags = BCM_IPMC_L2;
    ip4_ipmc_addr.vid = ipmc_addr->vid;
    ip4_ipmc_addr.mc_ip_addr = ipmc_addr->mc_ip_addr;
    rv = bcm_ipmc_find(unit, &ip4_ipmc_addr);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_ipmc_find, rv %d\n", rv);
        return rv;
    }
    print ip4_ipmc_addr;

    bcm_ipmc_addr_t_init(&ip4_ipmc_ssm_addr);
    ip4_ipmc_ssm_addr.flags = BCM_IPMC_L2;
    ip4_ipmc_ssm_addr.vid = ipmc_addr->vid;
    ip4_ipmc_ssm_addr.s_ip_addr = ipmc_addr->s_ip_addr;
    ip4_ipmc_ssm_addr.mc_ip_addr = ipmc_addr->mc_ip_addr;
    rv = bcm_ipmc_find(unit, &ip4_ipmc_ssm_addr);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_ipmc_find, rv %d\n", rv);
        return rv;
    }
    print ip4_ipmc_ssm_addr;

    return rv;
}

int l2_ipmc_ipv4_ssm_entry_remove_example(int unit,
					  bcm_ipmc_addr_t *ipmc_addr)
{
    int			rv = BCM_E_NONE;
    bcm_ipmc_addr_t	ip4_ipmc_addr;
    bcm_ipmc_addr_t	ip4_ipmc_ssm_addr;

    bcm_ipmc_addr_t_init(&ip4_ipmc_addr);
    ip4_ipmc_addr.flags = BCM_IPMC_L2;
    ip4_ipmc_addr.vid = ipmc_addr->vid;
    ip4_ipmc_addr.mc_ip_addr = ipmc_addr->mc_ip_addr;
    rv = bcm_ipmc_remove(unit, &ip4_ipmc_addr);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_ipmc_remove(1), rv %d\n", rv);
	return rv;
    }


    bcm_ipmc_addr_t_init(&ip4_ipmc_ssm_addr);
    ip4_ipmc_ssm_addr.flags = BCM_IPMC_L2;
    ip4_ipmc_ssm_addr.vid = ipmc_addr->vid;
    ip4_ipmc_ssm_addr.s_ip_addr = ipmc_addr->s_ip_addr;
    ip4_ipmc_ssm_addr.mc_ip_addr = ipmc_addr->mc_ip_addr;
    rv = bcm_ipmc_remove(unit, &ip4_ipmc_ssm_addr);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_ipmc_remove(2), rv %d\n", rv);
	return rv;
    }

    return rv;
}

int l2_ipmc_ipv6_ssm_entry_add_example(int unit, 
					bcm_field_group_t group_id, 
					bcm_ipmc_addr_t *ipmc_addr, 
					uint16 sip_index, 
					int entry_priority, 
					bcm_field_entry_t *entry) 
{
    int			rv = BCM_E_NONE;
    bcm_l2_addr_t	ip6_l2_addr;
    uint8 		eui_64_mac[6];
    bcm_ipmc_addr_t	ip6_ipmc_ssm_addr;
    int			index;

    /* 
     * Derive MAC from 64 lsb of SIP 
     */
    eui_64_mac[0] = ipmc_addr->s_ip6_addr[8];
    eui_64_mac[1] = ipmc_addr->s_ip6_addr[9];
    eui_64_mac[2] = ipmc_addr->s_ip6_addr[10];
    eui_64_mac[3] = ipmc_addr->s_ip6_addr[13];
    eui_64_mac[4] = ipmc_addr->s_ip6_addr[14];
    eui_64_mac[5] = ipmc_addr->s_ip6_addr[15];

    bcm_l2_addr_t_init(&ip6_l2_addr, eui_64_mac, ipmc_addr->vid);
    ip6_l2_addr.port = BCM_GPORT_BLACK_HOLE;
    ip6_l2_addr.encap_id = sip_index;
    ip6_l2_addr.flags = BCM_L2_STATIC|BCM_L2_SRC_HIT;
    rv = bcm_l2_addr_add(unit, &ip6_l2_addr);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_l2_addr_add, rv %d\n", rv);
	return rv;
    }


    bcm_ipmc_addr_t_init(&ip6_ipmc_ssm_addr);
    ip6_ipmc_ssm_addr.vid = ipmc_addr->vid;
    for (index = 0; index < 16; index++) {
        ip6_ipmc_ssm_addr.mc_ip6_addr[index] = ipmc_addr->mc_ip6_addr[index];
    }
    BCM_GPORT_MCAST_SET(ip6_ipmc_ssm_addr.group, ipmc_addr->group);
    rv = l2_ipmc_ipv6_ssm_entry_add(unit, 10, ip6_ipmc_ssm_addr, sip_index, entry_priority, entry);
    if (BCM_E_NONE != rv) {
        printf("Error in l2_ipmc_ipv6_ssm_entry_add, rv %d\n", rv);
	return rv;
    }

    return rv;
}

int l2_ipmc_ipv6_ssm_cb(int unit, bcm_l2_addr_t *info, void *user_data)
{
    print *info;
    return BCM_E_NONE;
}

int l2_ipmc_ipv6_ssm_entry_dump_example(int unit,
                                          bcm_ipmc_addr_t *ipmc_addr,
                                          bcm_field_entry_t *entry)
{
    int                 rv = BCM_E_NONE;
    bcm_l2_addr_t       l2_addr_get;
    uint8               eui_64_mac[6];
    uint32              flags;

    /*
     * Derive MAC from 64 lsb of SIP
     */
    eui_64_mac[0] = ipmc_addr->s_ip6_addr[8];
    eui_64_mac[1] = ipmc_addr->s_ip6_addr[9];
    eui_64_mac[2] = ipmc_addr->s_ip6_addr[10];
    eui_64_mac[3] = ipmc_addr->s_ip6_addr[13];
    eui_64_mac[4] = ipmc_addr->s_ip6_addr[14];
    eui_64_mac[5] = ipmc_addr->s_ip6_addr[15];

    l2_addr_get.flags = BCM_L2_SRC_HIT;
    rv = bcm_l2_addr_get(unit, eui_64_mac, ipmc_addr->vid, &l2_addr_get);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_l2_addr_get, rv %d\n", rv);
        return rv;
    }
    print l2_addr_get;

    rv = bcm_field_entry_dump(unit, *entry);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_entry_dump, rv %d\n", rv);
        return rv;
    }

    return rv;
}


int l2_ipmc_ipv6_ssm_entry_remove_example(int unit,
                                          bcm_ipmc_addr_t *ipmc_addr,
                                          bcm_field_entry_t entry)
{
    int			rv = BCM_E_NONE;
    bcm_l2_addr_t       ip6_l2_addr;
    uint8               eui_64_mac[6];
    uint32 		flags;
    bcm_l2_addr_t 	ip6_l2_addr_mask;
    uint8 		mac_mask[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    /*
     * Derive MAC from 64 lsb of SIP
     */
    eui_64_mac[0] = ipmc_addr->s_ip6_addr[8];
    eui_64_mac[1] = ipmc_addr->s_ip6_addr[9];
    eui_64_mac[2] = ipmc_addr->s_ip6_addr[10];
    eui_64_mac[3] = ipmc_addr->s_ip6_addr[13];
    eui_64_mac[4] = ipmc_addr->s_ip6_addr[14];
    eui_64_mac[5] = ipmc_addr->s_ip6_addr[15];

    flags = BCM_L2_REPLACE_MATCH_STATIC|BCM_L2_REPLACE_DELETE|BCM_L2_REPLACE_MATCH_VLAN|BCM_L2_REPLACE_MATCH_MAC;
    bcm_l2_addr_t_init(&ip6_l2_addr, eui_64_mac, ipmc_addr->vid);
    ip6_l2_addr.flags = BCM_L2_SRC_HIT;
    bcm_l2_addr_t_init(&ip6_l2_addr_mask, mac_mask, ipmc_addr->vid);
    rv = bcm_l2_replace(unit, flags, &ip6_l2_addr, 0, 0, 0);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_l2_replace_match, rv %d\n", rv);
	return rv;
    }

    rv = bcm_field_entry_destroy(unit, entry);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_entry_destroy, rv %d\n", rv);
	return rv;
    }

    return rv;
}


/**********************************************************************************************************************************/

bcm_vlan_t vid = 100;
bcm_vlan_port_t in_vp;
bcm_vlan_port_t ipmc_vp;
bcm_vlan_port_t ipmc_ssm_vp;

int l2_ipmc_example_traffic_setup(int unit, bcm_vlan_t vsi, int *mc_id_ipmc, int *mc_id_ipmc_ssm)
{
    int			rv = BCM_E_NONE;
    bcm_vlan_control_vlan_t vsi_control;
    int encap1;
    int encap2;
    bcm_gport_t     mc_gport;

    rv = bcm_multicast_create(unit, (BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID), mc_id_ipmc);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_multicast_create, rv %d\n", rv);
	return rv;
    }

    rv = bcm_multicast_create(unit, (BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID), mc_id_ipmc_ssm);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_multicast_create, rv %d\n", rv);
	return rv;
    }

    rv = bcm_vswitch_create_with_id(unit, vsi);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_vswitch_create_with_id, rv %d\n", rv);
	return rv;
    }


    rv = bcm_port_tpid_set(unit, in_port, 0x8100);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_port_tpid_set, rv %d\n", rv);
	return rv;
    }

    rv = bcm_port_inner_tpid_set(unit, in_port, 0x9100);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_port_inner_tpid_set, rv %d\n", rv);
	return rv;
    }

    rv = bcm_port_class_set(unit, in_port, bcmPortClassId, in_port);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_port_class_set, rv %d\n", rv);
	return rv;
    }    

    rv = bcm_port_tpid_set(unit, ipmc_out_port, 0x8100);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_port_tpid_set, rv %d\n", rv);
	return rv;
    }

    rv = bcm_port_inner_tpid_set(unit, ipmc_out_port, 0x9100);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_port_inner_tpid_set, rv %d\n", rv);
	return rv;
    }

    rv = bcm_port_class_set(unit, ipmc_out_port, bcmPortClassId, ipmc_out_port);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_port_class_set, rv %d\n", rv);
	return rv;
    }

    rv = bcm_port_tpid_set(unit, ipmc_ssm_out_port, 0x8100);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_port_tpid_set, rv %d\n", rv);
	return rv;
    }

    rv = bcm_port_inner_tpid_set(unit, ipmc_ssm_out_port, 0x9100);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_port_inner_tpid_set, rv %d\n", rv);
	return rv;
    }

    rv = bcm_port_class_set(unit, ipmc_ssm_out_port, bcmPortClassId, ipmc_ssm_out_port);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_port_class_set, rv %d\n", rv);
	return rv;
    }


    bcm_vlan_control_vlan_t_init(&vsi_control);
    vsi_control.forwarding_vlan = vsi;
    vsi_control.unknown_unicast_group   = 99;
    vsi_control.unknown_multicast_group = 99;
    vsi_control.broadcast_group         = 99;
    rv = bcm_vlan_control_vlan_set(unit, vsi, vsi_control);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_vlan_control_vlan_set, rv %d\n", rv);
	return rv;
    }


    bcm_vlan_port_t_init(&in_vp);
    in_vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    in_vp.port = in_port;
    in_vp.match_vlan = vid;
    in_vp.egress_vlan = vid;
    in_vp.vsi = 0;
    rv = bcm_vlan_port_create(unit, &in_vp);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_vlan_port_create, rv %d\n", rv);
	return rv;
    }
    printf("Created in_port 0x%08x\n", in_vp.vlan_port_id);

    rv = bcm_vswitch_port_add(unit, vsi, in_vp.vlan_port_id);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_vswitch_port_add, rv %d\n", rv);
	return rv;
    }


    bcm_vlan_port_t_init(&ipmc_vp);
    ipmc_vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    ipmc_vp.port = ipmc_out_port;
    ipmc_vp.match_vlan = vid;
    ipmc_vp.egress_vlan = vid;
    ipmc_vp.vsi = 0;
    rv = bcm_vlan_port_create(unit, &ipmc_vp);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_vlan_port_create, rv %d\n", rv);
	return rv;
    }
    printf("Created ipmc_vp 0x%08x\n", ipmc_vp.vlan_port_id);


    rv = bcm_multicast_vlan_encap_get(unit, *mc_id_ipmc, ipmc_out_port, ipmc_vp.vlan_port_id, &encap2);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_multicast_vlan_encap_get, rv %d\n", rv);
	return rv;
    }

    rv = bcm_multicast_ingress_add(unit, *mc_id_ipmc, ipmc_out_port, encap2);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_multicast_ingress_add, rv %d\n", rv);
	return rv;
    }

    rv = bcm_vswitch_port_add(unit, vsi, ipmc_vp.vlan_port_id);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_vswitch_port_add, rv %d\n", rv);
	return rv;
    }

    bcm_vlan_port_t_init(&ipmc_ssm_vp);
    ipmc_ssm_vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    ipmc_ssm_vp.port = ipmc_ssm_out_port;
    ipmc_ssm_vp.match_vlan = vid;
    ipmc_ssm_vp.egress_vlan = vid;
    ipmc_ssm_vp.vsi = 0;
    rv = bcm_vlan_port_create(unit, &ipmc_ssm_vp);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_vlan_port_create, rv %d\n", rv);
	return rv;
    }
    printf("Created ipmc_ssm_vp 0x%08x\n", ipmc_ssm_vp.vlan_port_id);

    rv = bcm_multicast_vlan_encap_get(unit, *mc_id_ipmc_ssm, ipmc_ssm_out_port, ipmc_ssm_vp.vlan_port_id, &encap2);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_multicast_vlan_encap_get, rv %d\n", rv);
	return rv;
    }

    rv = bcm_multicast_ingress_add(unit, *mc_id_ipmc_ssm, ipmc_ssm_out_port, encap2);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_multicast_ingress_add, rv %d\n", rv);
	return rv;
    }

    rv = bcm_vswitch_port_add(unit, vsi, ipmc_ssm_vp.vlan_port_id);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_vswitch_port_add, rv %d\n", rv);
	return rv;
    }

    return rv;
}

int l2_ipmc_example_traffic_teardown(int unit, bcm_vlan_t vsi, int mc_id_ipmc, int mc_id_ipmc_ssm)
{
    int rv = BCM_E_NONE;

    rv = bcm_multicast_ingress_delete_all(unit, mc_id_ipmc);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_multicast_ingress_delete_all, rv %d\n", rv);
	return rv;
    }

    rv = bcm_multicast_destroy(unit, mc_id_ipmc);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_multicast_destroy, rv %d\n", rv);
	return rv;
    }

    rv = bcm_multicast_ingress_delete_all(unit, mc_id_ipmc_ssm);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_multicast_ingress_delete_all, rv %d\n", rv);
	return rv;
    }

    rv = bcm_multicast_destroy(unit, mc_id_ipmc_ssm);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_multicast_destroy, rv %d\n", rv);
	return rv;
    }

    rv = bcm_vlan_port_destroy(unit, in_vp.vlan_port_id);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_vlan_port_destroy, rv %d\n", rv);
	return rv;
    }

    rv = bcm_vlan_port_destroy(unit, ipmc_vp.vlan_port_id);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_vlan_port_destroy, rv %d\n", rv);
	return rv;
    }

    rv = bcm_vlan_port_destroy(unit, ipmc_ssm_vp.vlan_port_id);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_vlan_port_destroy, rv %d\n", rv);
	return rv;
    }

    rv = bcm_vswitch_destroy(unit, vsi);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_vlan_port_destroy, rv %d\n", rv);
	return rv;
    }

    return rv;
}

bcm_field_group_t group_id = 10;
int group_priority = 20;

int l2_ipmc_init_example(int unit, int default_entry_priority)
{
    int rv = BCM_E_NONE;

    /* Initialize the L2 IPMC SSM feature */
    rv = l2_ipmc_ssm_pmf_init(unit, group_id, group_priority, default_entry_priority);
    if (BCM_E_NONE != rv) {
        printf("Error in l2_ipmc_ssm_pmf_init, rv %d\n", rv);
	return rv;
    }

    return rv;
}

uint32 mc_ip4_addr = 0xef002222;
uint32 s_ip4_ssm_addr = 0x01020304;
uint32 mc_ip4_ssm_addr = 0xe8002222;
uint8 mc_ip6_addr[16] = {0xff, 0x88, 0x55, 0x55,
                         0x66, 0x66, 0x66, 0x66,
                         0x77, 0x77, 0x77, 0x77,
                         0x00, 0x00, 0x44, 0x44};

uint8 s_ip6_ssm_addr[16] = {0xFE, 0x80, 0x00, 0x00,
			    0x00, 0x00, 0x00, 0x00,
			    0x00, 0x11, 0x11, 0xFF,
			    0xFE, 0x11, 0x11, 0x11};

uint8 mc_ip6_ssm_addr[16] = {0xff, 0x30, 0x55, 0x55,
                      	     0x66, 0x66, 0x66, 0x66,
                      	     0x77, 0x77, 0x77, 0x77,
                      	     0x00, 0x00, 0x44, 0x44};

uint16 sip_index = 1024;

bcm_field_entry_t l2_ipmc_ip6_entry;
bcm_field_entry_t l2_ipmc_ssm_ip6_entry;

int l2_ipmc_example(int unit)
{
    int			rv = BCM_E_NONE;
    bcm_vlan_t vsi = 255;
    int mc_id_ipmc = 0x1000;
    int mc_id_ipmc_ssm = 0x2000;
    bcm_ipmc_addr_t ip4_ipmc_addr;
    bcm_ipmc_addr_t ip4_ipmc_ssm_addr;
    bcm_ipmc_addr_t ip6_ipmc_addr;
    bcm_ipmc_addr_t ip6_ipmc_ssm_addr;
    bcm_l2_addr_t       ip6_l2_addr;
    int ip6_ipmc_entry_priority = 10;
    int ip6_ipmc_ssm_entry_priority = 100;
    int index;
    uint8               eui_64_mac[6];

    l2_ipmc_ip6_entry = 0;
    l2_ipmc_ssm_ip6_entry = 0;

    /* Configure the VSI, Vlan Ports, and Multicast Groups */
    rv = l2_ipmc_example_traffic_setup(unit, vsi, &mc_id_ipmc, &mc_id_ipmc_ssm);
    if (BCM_E_NONE != rv) {
        printf("Error in l2_ipmc_example_traffic_setup, rv %d\n", rv);
	return rv;
    }

    /* Enable L2 IPMC on vlan port {port 13, vid 100} */
    rv = bcm_port_control_set(unit, in_vp.vlan_port_id, bcmPortControlIP4Mcast, 1);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_port_control_set, rv %d\n", rv);
	return rv;
    }

    /* Create IPMC IP4 entry */
    bcm_ipmc_addr_t_init(&ip4_ipmc_addr);
    ip4_ipmc_addr.vid = vsi;
    ip4_ipmc_addr.mc_ip_addr = mc_ip4_addr;
    ip4_ipmc_addr.group = mc_id_ipmc;
    ip4_ipmc_addr.flags = BCM_IPMC_L2;
    print ip4_ipmc_addr;
    rv = bcm_ipmc_add(unit, &ip4_ipmc_addr);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_ipmc_add, rv %d\n", rv);
	return rv;
    }


    /* Create IP SSM IP4 */
    bcm_ipmc_addr_t_init(&ip4_ipmc_ssm_addr);
    ip4_ipmc_ssm_addr.vid = vsi;
    ip4_ipmc_ssm_addr.mc_ip_addr = mc_ip4_ssm_addr;
    ip4_ipmc_ssm_addr.s_ip_addr = s_ip4_ssm_addr;
    ip4_ipmc_ssm_addr.group = mc_id_ipmc_ssm;
    rv = l2_ipmc_ipv4_ssm_entry_add_example(unit, ip4_ipmc_ssm_addr);
    if (BCM_E_NONE != rv) {
        printf("Error in l2_ipmc_ipv4_ssm_entry_add_example, rv %d\n", rv);
	return rv;
    }


    /* IPMC IP6 */
    bcm_ipmc_addr_t_init(&ip6_ipmc_addr);
    ip6_ipmc_addr.vid = vsi;
    for (index = 0; index < 16; index++) {
        ip6_ipmc_addr.mc_ip6_addr[index] = mc_ip6_addr[index];
    }
    BCM_GPORT_MCAST_SET(ip6_ipmc_addr.group, mc_id_ipmc);
    rv = l2_ipmc_ipv6_entry_add(unit, group_id, ip6_ipmc_addr, ip6_ipmc_entry_priority, &l2_ipmc_ip6_entry);
    if (BCM_E_NONE != rv) {
        printf("Error in l2_ipmc_ipv6_entry_add, rv %d\n", rv);
	return rv;
    }


    /* IPMC SSM IP6 */
    bcm_ipmc_addr_t_init(&ip6_ipmc_ssm_addr);
    ip6_ipmc_ssm_addr.vid = vsi;
    for (index = 0; index < 16; index++) {
        ip6_ipmc_ssm_addr.mc_ip6_addr[index] = mc_ip6_ssm_addr[index];
        ip6_ipmc_ssm_addr.s_ip6_addr[index] = s_ip6_ssm_addr[index];
    }
    BCM_GPORT_MCAST_SET(ip6_ipmc_ssm_addr.group, mc_id_ipmc_ssm);
    rv = l2_ipmc_ipv6_ssm_entry_add_example(unit, group_id, ip6_ipmc_ssm_addr, sip_index, ip6_ipmc_ssm_entry_priority, &l2_ipmc_ssm_ip6_entry);
    if (BCM_E_NONE != rv) {
        printf("Error in l2_ipmc_ipv6_ssm_entry_add_example, rv %d\n", rv);
	return rv;
    }
    return rv;
}

int l2_ipmc_teardown_example(int unit)
{
    int rv = BCM_E_NONE;
    bcm_vlan_t vsi = 255;
    int mc_id_ipmc = 0x1000;
    int mc_id_ipmc_ssm = 0x2000;
    bcm_ipmc_addr_t ip4_ipmc_addr;
    bcm_ipmc_addr_t ip4_ipmc_ssm_addr;
    bcm_ipmc_addr_t ip6_ipmc_addr;
    bcm_ipmc_addr_t ip6_ipmc_ssm_addr;
    bcm_l2_addr_t       ip6_l2_addr;
    int ip6_ipmc_entry_priority = 10;
    int ip6_ipmc_ssm_entry_priority = 100;
    int index;
    uint8               eui_64_mac[6];

    /* Delete IPMC IP4 entry */
    bcm_ipmc_addr_t_init(&ip4_ipmc_addr);
    ip4_ipmc_addr.flags = BCM_IPMC_L2;
    ip4_ipmc_addr.vid = vsi;
    ip4_ipmc_addr.mc_ip_addr = mc_ip4_addr;
    rv = bcm_ipmc_remove(unit, &ip4_ipmc_addr);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_ipmc_remove(3), rv %d\n", rv);
        return rv;
    }

    /* Delete IP SSM IP4 */
    bcm_ipmc_addr_t_init(&ip4_ipmc_ssm_addr);
    ip4_ipmc_ssm_addr.vid = vsi;
    ip4_ipmc_ssm_addr.mc_ip_addr = mc_ip4_ssm_addr;
    ip4_ipmc_ssm_addr.s_ip_addr = s_ip4_ssm_addr;
    rv = l2_ipmc_ipv4_ssm_entry_remove_example(unit, ip4_ipmc_ssm_addr);
    if (BCM_E_NONE != rv) {
        printf("Error in l2_ipmc_ipv4_ssm_entry_remove_example, rv %d\n", rv);
        return rv;
    }

    /* Delete IPMC IP6 */
    rv = bcm_field_entry_destroy(unit, l2_ipmc_ip6_entry);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_entry_destroy, rv %d\n", rv);
        return rv;
    }

    /* Delete IPMC SSM IP6 */
    bcm_ipmc_addr_t_init(&ip6_ipmc_ssm_addr);
    ip6_ipmc_ssm_addr.vid = vsi;
    for (index = 0; index < 16; index++) {
        ip6_ipmc_ssm_addr.mc_ip6_addr[index] = mc_ip6_ssm_addr[index];
        ip6_ipmc_ssm_addr.s_ip6_addr[index] = s_ip6_ssm_addr[index];
    }
    rv = l2_ipmc_ipv6_ssm_entry_remove_example(unit, ip6_ipmc_ssm_addr, l2_ipmc_ssm_ip6_entry);
    if (BCM_E_NONE != rv) {
        printf("Error in l2_ipmc_ipv6_ssm_entry_remove_example, rv %d\n", rv);
        return rv;
    }

    /* Enable L2 IPMC on vlan port {port 13, vid 100} */
    rv = bcm_port_control_set(unit, in_vp.vlan_port_id, bcmPortControlIP4Mcast, 0);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_port_control_set, rv %d\n", rv);
	return rv;
    }

    rv = l2_ipmc_example_traffic_teardown(unit, vsi, mc_id_ipmc, mc_id_ipmc_ssm);
    if (BCM_E_NONE != rv) {
        printf("Error in l2_ipmc_example_traffic_teardown, rv %d\n", rv);
        return rv;
    }

    return rv;
}

int l2_ipmc_dump_example(int unit)
{
    int rv = BCM_E_NONE;
    bcm_vlan_t vsi = 255;
    int mc_id_ipmc = 0x3fff;
    int mc_id_ipmc_ssm = 0x7fff;
    bcm_ipmc_addr_t ip4_ipmc_addr;
    bcm_ipmc_addr_t ip4_ipmc_ssm_addr;
    bcm_ipmc_addr_t ip6_ipmc_addr;
    bcm_ipmc_addr_t ip6_ipmc_ssm_addr;
    bcm_l2_addr_t       ip6_l2_addr;
    int ip6_ipmc_entry_priority = 10;
    int ip6_ipmc_ssm_entry_priority = 100;
    int index;
    uint8               eui_64_mac[6];

    /* Dump IPMC IP4 entry */
    bcm_ipmc_addr_t_init(&ip4_ipmc_addr);
    ip4_ipmc_addr.flags = BCM_IPMC_L2;
    ip4_ipmc_addr.vid = vsi;
    ip4_ipmc_addr.mc_ip_addr = mc_ip4_addr;
    rv = bcm_ipmc_find(unit, &ip4_ipmc_addr);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_ipmc_find, rv %d\n", rv);
        return rv;
    }
    print ip4_ipmc_addr;

    /* Dump IP SSM IP4 */
    bcm_ipmc_addr_t_init(&ip4_ipmc_ssm_addr);
    ip4_ipmc_ssm_addr.vid = vsi;
    ip4_ipmc_ssm_addr.mc_ip_addr = mc_ip4_ssm_addr;
    ip4_ipmc_ssm_addr.s_ip_addr = s_ip4_ssm_addr;
    rv = l2_ipmc_ipv4_ssm_entry_dump_example(unit, ip4_ipmc_ssm_addr);
    if (BCM_E_NONE != rv) {
        printf("Error in l2_ipmc_ipv4_ssm_entry_dump_example, rv %d\n", rv);
        return rv;
    }

    /* Dump IPMC IP6 */
    rv = bcm_field_entry_dump(unit, l2_ipmc_ip6_entry);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_entry_dump, rv %d\n", rv);
        return rv;
    }

    /* Dump IPMC SSM IP6 */
    bcm_ipmc_addr_t_init(&ip6_ipmc_ssm_addr);
    ip6_ipmc_ssm_addr.vid = vsi;
    for (index = 0; index < 16; index++) {
        ip6_ipmc_ssm_addr.mc_ip6_addr[index] = mc_ip6_ssm_addr[index];
        ip6_ipmc_ssm_addr.s_ip6_addr[index] = s_ip6_ssm_addr[index];
    }
    rv = l2_ipmc_ipv6_ssm_entry_dump_example(unit, ip6_ipmc_ssm_addr, l2_ipmc_ssm_ip6_entry);

    return rv;
}


