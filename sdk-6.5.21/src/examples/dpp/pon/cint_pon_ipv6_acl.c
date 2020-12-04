/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*
* File: cint_pon_ipv6_acl.c  
* 
* Purpose: examples of IPv6 classificaiton and ACLs.  
* 
* Test case:
*     IPv6 classification based on IPv6 SIP and DIP.
*     1. Map SR IP FE80::01:01:04 to queue 7, COS 7 and VLAN 700.
*     2. Map SR IP FE80::01:01:03 to queue 5, COS 5 and VLAN 500.
*     3. Map DA IP FE80::01:01:02 to queue 3, COS 3 and VLAN 300.
*     4. Map DA IP FE80::01:01:01 to queue 1, COS 1 and VLAN 100.
*
* ACL Service Test cases:
*     1. Drop Tunnel 100, DA 00:00:00:00:00:01, SA 00:00:00:00:00:02, VLANID 3000, UDP, SIP 0xFE80::0001:0101 from pon_port
*     2. Drop Tunnel 101, DA [00:00:00:00:00:01, 00:00:00:FF:FF:FF] from pon_port
*     3. Drop Tunnel 102, SA [00:00:00:00:00:01, 00:00:00:FF:FF:FF] from pon_port
*     3. Drop Tunnel 103, L4SrcPort [2, 0x3FF] from pon_port
*     3. Drop Tunnel 104, L4DstPort [2, 0x3FF] from pon_port
*
* PP Model: N:1 Service.
*     PON Port 4               <-----------------> NNI Port 128 
*     Tunnel-ID 100 CVLAN 10   <--------|
*     Tunnel-ID 100 CVLAN 3000 <--------|
*     Tunnel-ID 101 CVLAN 20   <--------|--------> CVLAN 100
*     Tunnel-ID 102 CVLAN 30   <--------|
*     Tunnel-ID 103 CVLAN 40   <--------|
*     Tunnel-ID 104 CVLAN 50   <--------|
*
* To Activate Above Settings Run:
*     BCM> cint examples/dpp/cint_port_tpid.c
*     BCM> cint examples/dpp/pon/cint_pon_utils.c
*     BCM> cint examples/dpp/pon/cint_pon_up_scheduler.c
*     BCM> cint examples/dpp/pon/cint_pon_ipv6_acl.c
*     BCM> cint
* To Activate PON service models:
*     cint> pon_ipv6_acl_service_init(0, 4, 128);
*     cint> pon_ipv6_acl_service_setup(0);
* To Activate ACL Classification Run:
*     cint> pon_up_scheduler_init(0);
*     cint> pon_up_scheduler_setup(0);
*     cint> pon_ipv6_acl_classification_setup(0);
* To Deactivate ACL Classification Run:
*     cint> pon_ipv6_acl_classification_cleanup(0);
*     cint> pon_up_scheduler_cleanup(0);
* To Activate ACL Service Run:
*     cint> pon_ipv6_acl_init(4);
*     cint> pon_ipv6_acl_setup(0);
* To Deactivate ACL Service Run:
*     cint> pon_ipv6_acl_cleanup(0);
* To Dectivate PON service models:
*     cint> pon_ipv6_acl_service_cleanup(0);
 */

enum ipv6_acl_type_e{
    ipv6_acl_sip,
    ipv6_acl_dip
};

bcm_tunnel_id_t ipv6_tunnel[2][2] = { { 100, 101 }, {102, 103} };    
    
bcm_ip6_t ip6_mask = 
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

bcm_ip6_t ipv6_ip[2][2] = 
    { { {0xFE, 0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x01, 0, 0x01, 0, 0x04},
        {0xFE, 0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x01, 0, 0x01, 0, 0x03} },
      { {0xFE, 0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x01, 0, 0x01, 0, 0x02},
        {0xFE, 0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x01, 0, 0x01, 0, 0x01} } };

int ipv6_remark_cos[2][2] = { {7, 5}, {3, 1} };
bcm_vlan_t ipv6_remark_vlan[2][2] = { {700, 500}, {300, 100} };
bcm_field_group_t ipv6_grp[2];
bcm_field_entry_t ipv6_ent[2][2];


/*******************************************************************************
* IPv6 ACL classification setup
 */
int pon_ipv6_acl_classification_setup(int unit)
{
    bcm_error_t rv = BCM_E_NONE;  
    int type;
    int entries;
    int group_priority[2] = {3, 4};
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;    

    for (type = ipv6_acl_sip; type <= ipv6_acl_dip; type++)
    {
        BCM_FIELD_QSET_INIT(qset);          
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);        
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyTunnelId);
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyEtherType);
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIp6);        
        BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIpProtocol);
        if (type == ipv6_acl_dip) {
            BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstIp6);
        } else {
            BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcIp6);
        }
        
        rv = bcm_field_group_create(unit, qset, group_priority[type], &ipv6_grp[type]);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_group_create_id $rv\n");
            return rv;
        }

        BCM_FIELD_ASET_INIT(aset);
        BCM_FIELD_ASET_ADD(aset, bcmFieldActionOuterVlanPrioNew);
        BCM_FIELD_ASET_ADD(aset, bcmFieldActionPrioIntNew);
        BCM_FIELD_ASET_ADD(aset, bcmFieldActionOuterVlanNew);  
        rv = bcm_field_group_action_set(unit, ipv6_grp[type], aset);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_group_action_set $rv\n");
            return rv;
        }

        for (entries = 0; entries < 2; entries++)
        {                 
            rv = bcm_field_entry_create(unit, ipv6_grp[type], &ipv6_ent[type][entries]);
            if (rv != BCM_E_NONE) {
                printf("Error in bcm_field_entry_create $rv\n");
                return rv;
            }

            rv = bcm_field_qualify_EtherType(unit, ipv6_ent[type][entries], 0x86DD, 0xffff);
            if (rv != BCM_E_NONE) {
                printf("Error in bcm_field_qualify_EtherType $rv %d\n");
                return rv;
            }

            rv = bcm_field_qualify_InPort(unit, ipv6_ent[type][entries], pon_port, 0xffffffff);
            if (rv != BCM_E_NONE) {
                printf("Error in bcm_field_qualify_InPort with in_port $rv %d\n", pon_port);
                return rv;
            }

            rv = bcm_field_qualify_TunnelId(unit, ipv6_ent[type][entries], ipv6_tunnel[type][entries], 0x7ff);
            if (rv != BCM_E_NONE) {
                printf("Error in bcm_field_qualify_TunnelId with in_port $rv %d\n", ipv4_tunnel[type]);
                return rv;
            }

            if (type == ipv6_acl_dip) {
                rv = bcm_field_qualify_DstIp6(unit, ipv6_ent[type][entries], ipv6_ip[type][entries], ip6_mask);
                if (rv != BCM_E_NONE) {
                    printf("Error in bcm_field_qualify_DstIp with dst_ip $rv %d\n", ipv6_ip[type][entries]);
                    return rv;
                }
            } else {
                rv = bcm_field_qualify_SrcIp6(unit, ipv6_ent[type][entries], ipv6_ip[type][entries], ip6_mask);
                if (rv != BCM_E_NONE) {
                    printf("Error in bcm_field_qualify_DstIp with dst_ip $rv %d\n", ipv6_ip[type][entries]);
                    return rv;
                }
            }         

            rv = bcm_field_action_add(unit, ipv6_ent[type][entries], bcmFieldActionOuterVlanPrioNew, ipv6_remark_cos[type][entries], 0 );
            if (rv != BCM_E_NONE)
            {
                printf("ERROR in bcm_field_action_add $rv %d\n", ipv6_remark_cos[type][entries]);
                return rv;
            } 

            rv = bcm_field_action_add(unit, ipv6_ent[type][entries], bcmFieldActionPrioIntNew, ipv6_remark_cos[type][entries], 0 );
            if (rv != BCM_E_NONE)
            {
                printf("ERROR in bcm_field_action_add $rv %d\n", ipv6_remark_cos[type][entries]);
                return rv;
            }

            rv = bcm_field_action_add(unit, ipv6_ent[type][entries], bcmFieldActionOuterVlanNew, ipv6_remark_vlan[type][entries], 0 );
            if (rv != BCM_E_NONE)
            {
                printf("ERROR in bcm_field_action_add $rv %d\n", ipv6_remark_cos[type][entries]);
                return rv;
            }
            
            rv = bcm_field_entry_install(unit, ipv6_ent[type][entries]);
            if (rv != BCM_E_NONE)
            {
                printf("ERROR in bcm_field_entry_install $rv\n");
                return rv;
            }

        }
    } 

    return rv;
}


/*******************************************************************************
* IPv6 ACL classification cleanup
 */
int pon_ipv6_acl_classification_cleanup(int unit)
{
    bcm_error_t rv = BCM_E_NONE;  
    int type;
    int entries;

    for (type = ipv6_acl_sip; type <= ipv6_acl_dip; type++)
    {
        for (entries = 0; entries < 2; entries++)
        {    
            rv = bcm_field_entry_remove(unit, ipv6_ent[type][entries]);
            if (rv != BCM_E_NONE)
            {
                printf("ERROR: bcm_field_entry_remove returned $rv\n");
                return rv;
            }

            rv = bcm_field_entry_destroy(unit, ipv6_ent[type][entries]);
            if (rv != BCM_E_NONE)
            {
                printf("ERROR: bcm_field_entry_destroy returned $rv\n");
                return rv;
            }
        }
        
        rv = bcm_field_group_destroy(unit, ipv6_grp[type]);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: bcm_field_group_destroy returned $rv\n");
            return rv;
        }
    }

    return rv;
}

struct pon_ipv6_l4_port_range_s {
    uint8 valid;
    uint32 port_min;
    uint32 port_max;
    bcm_field_range_t port_range;
};

struct pon_ipv6_acl_s {
    bcm_port_t pon_port;
    bcm_tunnel_id_t tunnel_id;
    bcm_mac_t src_mac;
    bcm_mac_t src_mac_mask;
    bcm_mac_t dst_mac;
    bcm_mac_t dst_mac_mask;
    bcm_vlan_t outer_vlan;
    uint8 ip_protocol;
    bcm_ip6_t sip;
    pon_ipv6_l4_port_range_s l4_src_port_range;
    pon_ipv6_l4_port_range_s l4_dst_port_range;
    uint8 drop_action;
    bcm_field_entry_t eid;
};

int nof_ipv6_acl;
pon_ipv6_acl_s pon_ipv6_acl[5];
bcm_field_group_t pon_ipv6_acl_grp;
bcm_field_group_t pon_ipv6_acl_grp_pri = 14;

/*******************************************************************************
* IPv6 ACL Init
 */
int pon_ipv6_acl_init(bcm_port_t pon_port)
{
    int idx = 0;
	int rv = BCM_E_NONE;

    pon_ipv6_acl[idx].pon_port = pon_port;
    pon_ipv6_acl[idx].tunnel_id = 100;
    pon_ipv6_acl[idx].src_mac[0] = 0x00;
    pon_ipv6_acl[idx].src_mac[1] = 0x00;
    pon_ipv6_acl[idx].src_mac[2] = 0x00;
    pon_ipv6_acl[idx].src_mac[3] = 0x00;
    pon_ipv6_acl[idx].src_mac[4] = 0x00;
    pon_ipv6_acl[idx].src_mac[5] = 0x02;
    pon_ipv6_acl[idx].src_mac_mask[0] = 0xff;
    pon_ipv6_acl[idx].src_mac_mask[1] = 0xff;
    pon_ipv6_acl[idx].src_mac_mask[2] = 0xff;
    pon_ipv6_acl[idx].src_mac_mask[3] = 0xff;
    pon_ipv6_acl[idx].src_mac_mask[4] = 0xff;
    pon_ipv6_acl[idx].src_mac_mask[5] = 0xff;
    pon_ipv6_acl[idx].dst_mac[0] = 0x00;
    pon_ipv6_acl[idx].dst_mac[1] = 0x00;
    pon_ipv6_acl[idx].dst_mac[2] = 0x00;
    pon_ipv6_acl[idx].dst_mac[3] = 0x00;
    pon_ipv6_acl[idx].dst_mac[4] = 0x00;
    pon_ipv6_acl[idx].dst_mac[5] = 0x01;
    pon_ipv6_acl[idx].dst_mac_mask[0] = 0xff;
    pon_ipv6_acl[idx].dst_mac_mask[1] = 0xff;
    pon_ipv6_acl[idx].dst_mac_mask[2] = 0xff;
    pon_ipv6_acl[idx].dst_mac_mask[3] = 0xff;
    pon_ipv6_acl[idx].dst_mac_mask[4] = 0xff;
    pon_ipv6_acl[idx].dst_mac_mask[5] = 0xff;
    pon_ipv6_acl[idx].outer_vlan = 3000;
    pon_ipv6_acl[idx].ip_protocol = 0x11;  /* UDP */
    pon_ipv6_acl[idx].sip[0] = 0xfe;
    pon_ipv6_acl[idx].sip[1] = 0x80;
    pon_ipv6_acl[idx].sip[13] = 0x01;
    pon_ipv6_acl[idx].sip[14] = 0x01;
    pon_ipv6_acl[idx].sip[15] = 0x01;
    pon_ipv6_acl[idx].drop_action = 1;

    idx++;
    pon_ipv6_acl[idx].pon_port = pon_port;
    pon_ipv6_acl[idx].tunnel_id = 101;
    pon_ipv6_acl[idx].dst_mac[0] = 0x00;
    pon_ipv6_acl[idx].dst_mac[1] = 0x00;
    pon_ipv6_acl[idx].dst_mac[2] = 0x00;
    pon_ipv6_acl[idx].dst_mac[3] = 0x00;
    pon_ipv6_acl[idx].dst_mac[4] = 0x00;
    pon_ipv6_acl[idx].dst_mac[5] = 0x01;
    pon_ipv6_acl[idx].dst_mac_mask[0] = 0xff;
    pon_ipv6_acl[idx].dst_mac_mask[1] = 0xff;
    pon_ipv6_acl[idx].dst_mac_mask[2] = 0xff;
    pon_ipv6_acl[idx].dst_mac_mask[3] = 0x00;
    pon_ipv6_acl[idx].dst_mac_mask[4] = 0x00;
    pon_ipv6_acl[idx].dst_mac_mask[5] = 0x00;
    pon_ipv6_acl[idx].drop_action = 1;

    idx++;
    pon_ipv6_acl[idx].pon_port = pon_port;
    pon_ipv6_acl[idx].tunnel_id = 102;
    pon_ipv6_acl[idx].src_mac[0] = 0x00;
    pon_ipv6_acl[idx].src_mac[1] = 0x00;
    pon_ipv6_acl[idx].src_mac[2] = 0x00;
    pon_ipv6_acl[idx].src_mac[3] = 0x00;
    pon_ipv6_acl[idx].src_mac[4] = 0x00;
    pon_ipv6_acl[idx].src_mac[5] = 0x01;
    pon_ipv6_acl[idx].src_mac_mask[0] = 0xff;
    pon_ipv6_acl[idx].src_mac_mask[1] = 0xff;
    pon_ipv6_acl[idx].src_mac_mask[2] = 0xff;
    pon_ipv6_acl[idx].src_mac_mask[3] = 0x00;
    pon_ipv6_acl[idx].src_mac_mask[4] = 0x00;
    pon_ipv6_acl[idx].src_mac_mask[5] = 0x00;
    pon_ipv6_acl[idx].drop_action = 1;

    idx++;
    pon_ipv6_acl[idx].pon_port = pon_port;
    pon_ipv6_acl[idx].tunnel_id = 103;
    pon_ipv6_acl[idx].l4_src_port_range.valid = 1;
    pon_ipv6_acl[idx].l4_src_port_range.port_min = 2;
    pon_ipv6_acl[idx].l4_src_port_range.port_max = 0x3FF;
    pon_ipv6_acl[idx].l4_dst_port_range.valid = 0;
    pon_ipv6_acl[idx].drop_action = 1;

    idx++;
    pon_ipv6_acl[idx].pon_port = pon_port;
    pon_ipv6_acl[idx].tunnel_id = 104;
    pon_ipv6_acl[idx].l4_dst_port_range.valid = 1;
    pon_ipv6_acl[idx].l4_dst_port_range.port_min = 2;
    pon_ipv6_acl[idx].l4_dst_port_range.port_max = 0x3FF;
    pon_ipv6_acl[idx].drop_action = 1;

    nof_ipv6_acl = idx + 1;

	return rv;
}

/*******************************************************************************
* IPv6 ACL Group Create
 */
int pon_ipv6_acl_group_create(int unit)
{
    bcm_error_t rv = BCM_E_NONE;  
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;    

    BCM_FIELD_QSET_INIT(qset);          
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstMac);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcMac);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcIp6);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyTunnelId);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyOuterVlanId);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIp6);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIpProtocol);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyRangeCheck);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);

    rv = bcm_field_group_create(unit, qset, pon_ipv6_acl_grp_pri, &pon_ipv6_acl_grp);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_group_create_id $rv\n");
        return rv;
    }

    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);
    rv = bcm_field_group_action_set(unit, pon_ipv6_acl_grp, aset);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_group_action_set $rv\n");
        return rv;
    }

    return rv;
}


/*******************************************************************************
* IPv6 ACL Service setup
 */
int pon_ipv6_acl_setup(int unit)
{
    bcm_error_t rv = BCM_E_NONE;  
    bcm_field_entry_t entry;
    int idx;
    uint32 range_flags;

    rv = pon_ipv6_acl_group_create(unit);
    if (rv != BCM_E_NONE) {
        printf("Error in pon_ipv6_acl_group_create $rv\n");
        return rv;
    }

    for (idx = 0; idx < nof_ipv6_acl; idx++) {
        rv = bcm_field_entry_create(unit, pon_ipv6_acl_grp, &entry);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_entry_create $rv\n");
            return rv;
        }

        rv = bcm_field_qualify_InPort(unit, entry, pon_ipv6_acl[idx].pon_port, 0xffffffff);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_qualify_InPort with %d in_port $rv\n", pon_ipv6_acl[idx].pon_port);
            return rv;
        }

        rv = bcm_field_qualify_TunnelId(unit, entry, pon_ipv6_acl[idx].tunnel_id, 0x7ff);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_qualify_TunnelId %d with in_port $rv\n", pon_ipv6_acl[idx].tunnel_id);
            return rv;
        }

        if ((pon_ipv6_acl[idx].src_mac[0] != 0) || (pon_ipv6_acl[idx].src_mac[1] != 0)
            || (pon_ipv6_acl[idx].src_mac[2] != 0) || (pon_ipv6_acl[idx].src_mac[3] != 0)
            || (pon_ipv6_acl[idx].src_mac[4] != 0) || (pon_ipv6_acl[idx].src_mac[5] != 0)) {
            
            rv = bcm_field_qualify_SrcMac(unit, entry, pon_ipv6_acl[idx].src_mac, pon_ipv6_acl[idx].src_mac_mask);
            if (rv != BCM_E_NONE) {
                printf("Error in bcm_field_qualify_SrcMac $rv\n");
                return rv;
            }
        }

        if ((pon_ipv6_acl[idx].dst_mac[0] != 0) || (pon_ipv6_acl[idx].dst_mac[1] != 0)
            || (pon_ipv6_acl[idx].dst_mac[2] != 0) || (pon_ipv6_acl[idx].dst_mac[3] != 0)
            || (pon_ipv6_acl[idx].dst_mac[4] != 0) || (pon_ipv6_acl[idx].dst_mac[5] != 0)) {
            
            rv = bcm_field_qualify_DstMac(unit, entry, pon_ipv6_acl[idx].dst_mac, pon_ipv6_acl[idx].dst_mac_mask);
            if (rv != BCM_E_NONE) {
                printf("Error in bcm_field_qualify_DstMac $rv\n");
                return rv;
            }
        }

        if (pon_ipv6_acl[idx].outer_vlan != 0) {
            rv = bcm_field_qualify_OuterVlanId(unit, entry, pon_ipv6_acl[idx].outer_vlan, 0xfff);
            if (rv != BCM_E_NONE) {
                printf("Error in bcm_field_qualify_OuterVlanId $rv\n");
                return rv;
            }
        }

        if (pon_ipv6_acl[idx].ip_protocol != 0) {
            rv = bcm_field_qualify_IpProtocol(unit, entry, pon_ipv6_acl[idx].ip_protocol, 0xff);
            if (rv != BCM_E_NONE) {
                printf("Error in bcm_field_qualify_IpProtocol $rv\n");
                return rv;
            }
        }

        if ((pon_ipv6_acl[idx].sip[0] != 0) || (pon_ipv6_acl[idx].sip[1] != 0)
            || (pon_ipv6_acl[idx].sip[2] != 0) || (pon_ipv6_acl[idx].sip[3] != 0)
            || (pon_ipv6_acl[idx].sip[4] != 0) || (pon_ipv6_acl[idx].sip[5] != 0)
            || (pon_ipv6_acl[idx].sip[6] != 0) || (pon_ipv6_acl[idx].sip[7] != 0)
            || (pon_ipv6_acl[idx].sip[8] != 0) || (pon_ipv6_acl[idx].sip[9] != 0)
            || (pon_ipv6_acl[idx].sip[10] != 0) || (pon_ipv6_acl[idx].sip[11] != 0)
            || (pon_ipv6_acl[idx].sip[12] != 0) || (pon_ipv6_acl[idx].sip[13] != 0)
            || (pon_ipv6_acl[idx].sip[14] != 0) || (pon_ipv6_acl[idx].sip[15] != 0)) {
            rv = bcm_field_qualify_SrcIp6(unit, entry, pon_ipv6_acl[idx].sip, ip6_mask);
            if (rv != BCM_E_NONE) {
                printf("Error in bcm_field_qualify_SrcIp $rv\n");
                return rv;
            }
        }

        if (pon_ipv6_acl[idx].l4_src_port_range.valid != 0) {
            range_flags = BCM_FIELD_RANGE_TCP|BCM_FIELD_RANGE_UDP|BCM_FIELD_RANGE_SRCPORT;
            rv = bcm_field_range_multi_create(unit, 
                                              &pon_ipv6_acl[idx].l4_src_port_range.port_range, 
                                              0 /* overall flags */,
                                              1 /* number of sub-ranges */,
                                              &range_flags,
                                              &pon_ipv6_acl[idx].l4_src_port_range.port_min,
                                              &pon_ipv6_acl[idx].l4_src_port_range.port_max);
            if (rv != BCM_E_NONE) {
                printf("Error in bcm_field_range_multi_create $rv\n");
                return rv;
            }

            rv = bcm_field_qualify_RangeCheck(unit, entry, pon_ipv6_acl[idx].l4_src_port_range.port_range, FALSE);
            if (rv != BCM_E_NONE) {
                printf("Error in bcm_field_qualify_RangeCheck $rv\n");
                return rv;
            }
        }

        if (pon_ipv6_acl[idx].l4_dst_port_range.valid != 0) {            
            range_flags = BCM_FIELD_RANGE_TCP|BCM_FIELD_RANGE_UDP|BCM_FIELD_RANGE_DSTPORT;
            rv = bcm_field_range_multi_create(unit, 
                                              &pon_ipv6_acl[idx].l4_dst_port_range.port_range, 
                                              0 /* overall flags */,
                                              1 /* number of sub-ranges */,
                                              &range_flags,
                                              &pon_ipv6_acl[idx].l4_dst_port_range.port_min,
                                              &pon_ipv6_acl[idx].l4_dst_port_range.port_max);
            if (rv != BCM_E_NONE) {
                printf("Error in bcm_field_range_multi_create $rv\n");
                return rv;
            }
        
            rv = bcm_field_qualify_RangeCheck(unit, entry, pon_ipv6_acl[idx].l4_dst_port_range.port_range, FALSE);
            if (rv != BCM_E_NONE) {
                printf("Error in bcm_field_qualify_RangeCheck $rv\n");
                return rv;
            }
        }

        if (pon_ipv6_acl[idx].drop_action != 0) {
            rv = bcm_field_action_add(unit, entry, bcmFieldActionDrop, 0, 0);
            if (rv != BCM_E_NONE) {
                printf("Error in bcm_field_action_add $rv\n");
                return rv;
            }
        }

        rv = bcm_field_entry_install(unit, entry);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR in bcm_field_entry_install $rv\n");
            return rv;
        }

        pon_ipv6_acl[idx].eid = entry;
    }

    return rv;
}

/*******************************************************************************
* IPv6 ACL Service cleanup
 */
int pon_ipv6_acl_cleanup(int unit)
{
    bcm_error_t rv = BCM_E_NONE;  
    bcm_field_entry_t entry;
    int idx;

    for (idx = 0; idx < nof_ipv6_acl; idx++) {
        entry = pon_ipv6_acl[idx].eid;

        if (pon_ipv6_acl[idx].l4_src_port_range.valid != 0) {
            rv = bcm_field_range_destroy(unit, pon_ipv6_acl[idx].l4_src_port_range.port_range);
            if (rv != BCM_E_NONE) {
                printf("Error in bcm_field_range_destroy idx %d $rv\n", idx);
                return rv;
            }            
        }

        if (pon_ipv6_acl[idx].l4_dst_port_range.valid != 0) {
            rv = bcm_field_range_destroy(unit, pon_ipv6_acl[idx].l4_dst_port_range.port_range);
            if (rv != BCM_E_NONE) {
                printf("Error in bcm_field_range_destroy idx %d $rv\n", idx);
                return rv;
            }            
        }

        rv = bcm_field_entry_remove(unit, entry);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: bcm_field_entry_remove idx %d returned $rv\n", idx);
            return rv;
        }
        
        rv = bcm_field_entry_destroy(unit, entry);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: bcm_field_entry_destroy idx %d returned $rv\n", idx);
            return rv;
        }
    }

    rv = bcm_field_group_destroy(unit, pon_ipv6_acl_grp);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: bcm_field_group_destroy group %d returned $rv\n", pon_ipv6_acl_grp);
        return rv;
    }

    return rv;
}

/*******************************************************************************
*
*  PP model
 */
struct pon_service_info_s{
    int service_mode;
    int up_lif_type;
    bcm_tunnel_id_t tunnel_id;
    bcm_vlan_t up_ovlan;
    bcm_vlan_t up_ivlan;
    uint8 up_pcp;
    bcm_ethertype_t up_ethertype;
    int down_lif_type;
    bcm_vlan_t down_ovlan;
    bcm_vlan_t down_ivlan;
    bcm_gport_t pon_gport;
    bcm_gport_t nni_gport;
};

bcm_port_t pon_port;
bcm_port_t nni_port;
bcm_vlan_t n_1_service_vsi;
int num_of_n_1_services;
pon_service_info_s pon_n_1_service_info[6];
int l4_port_range_test = 0;


/*******************************************************************************
* PP model Init                                                   
 */
int pon_ipv6_acl_service_init(int unit, bcm_port_t port_pon, bcm_port_t port_nni)
{
    bcm_error_t rv = BCM_E_NONE;    
    int idx;

    idx = 0;
    /* PON Port 4 Tunnel-ID 100 CVLAN 10 <-VSI-> NNI Port 128 CVLAN 100*/
    pon_n_1_service_info[idx].service_mode = otag_to_otag2;
    pon_n_1_service_info[idx].tunnel_id  = 100;
    pon_n_1_service_info[idx].up_lif_type = match_otag;
    pon_n_1_service_info[idx].up_ovlan   = 10;
    pon_n_1_service_info[idx].down_lif_type = match_otag;
    pon_n_1_service_info[idx].down_ovlan = 100;

    idx++;
    /* PON Port 4 Tunnel-ID 100 CVLAN 3000 <-VSI-> NNI Port 128 CVLAN 100*/
    pon_n_1_service_info[idx].service_mode = otag_to_otag2;
    pon_n_1_service_info[idx].tunnel_id  = 100;
    pon_n_1_service_info[idx].up_lif_type = match_otag;
    pon_n_1_service_info[idx].up_ovlan   = 3000;
    pon_n_1_service_info[idx].down_lif_type = match_otag;
    pon_n_1_service_info[idx].down_ovlan = 100;

    idx++;
    /* PON Port 4 Tunnel-ID 101 CVLAN 20 <-VSI-> NNI Port 128 CVLAN 100*/
    pon_n_1_service_info[idx].service_mode = otag_to_otag2;
    pon_n_1_service_info[idx].tunnel_id  = 101;
    pon_n_1_service_info[idx].up_lif_type = match_otag;
    pon_n_1_service_info[idx].up_ovlan   = 20;
    pon_n_1_service_info[idx].down_lif_type = match_otag;
    pon_n_1_service_info[idx].down_ovlan = 100;

    idx++;
    /* PON Port 4 Tunnel-ID 102 CVLAN 30 <-VSI-> NNI Port 128 CVLAN 100*/
    pon_n_1_service_info[idx].service_mode = otag_to_otag2;
    pon_n_1_service_info[idx].tunnel_id  = 102;
    pon_n_1_service_info[idx].up_lif_type = match_otag;
    pon_n_1_service_info[idx].up_ovlan   = 30;
    pon_n_1_service_info[idx].down_lif_type = match_otag;
    pon_n_1_service_info[idx].down_ovlan = 100;

    idx++;
    /* PON Port 4 Tunnel-ID 103 CVLAN 40 <-VSI-> NNI Port 128 CVLAN 100*/
    pon_n_1_service_info[idx].tunnel_id  = 103;
    if (l4_port_range_test)
    {
        pon_n_1_service_info[idx].up_lif_type = match_untag;
        pon_n_1_service_info[idx].service_mode = untag_to_otag;
    }
    else
    {
        pon_n_1_service_info[idx].up_lif_type = match_otag;
        pon_n_1_service_info[idx].up_ovlan   = 40;
        pon_n_1_service_info[idx].service_mode = otag_to_otag2;
    }
    pon_n_1_service_info[idx].down_lif_type = match_otag;
    pon_n_1_service_info[idx].down_ovlan = 100;

    idx++;
    /* PON Port 4 Tunnel-ID 104 CVLAN 50 <-VSI-> NNI Port 128 CVLAN 100*/
    pon_n_1_service_info[idx].tunnel_id  = 104;
    if (l4_port_range_test)
    {
        pon_n_1_service_info[idx].up_lif_type = match_untag;
        pon_n_1_service_info[idx].service_mode = untag_to_otag;
    }
    else
    {
        pon_n_1_service_info[idx].up_lif_type = match_otag;
        pon_n_1_service_info[idx].up_ovlan   = 50;
        pon_n_1_service_info[idx].service_mode = otag_to_otag2;
    }
    pon_n_1_service_info[idx].down_lif_type = match_otag;
    pon_n_1_service_info[idx].down_ovlan = 100;

    num_of_n_1_services = idx + 1;

    pon_port = port_pon;
    nni_port = port_nni;
    
    rv = pon_app_init(unit, pon_port, nni_port, 0, 0);
    if (rv != BCM_E_NONE) {
        printf("pon_app_init failed $rv\n");
    } 
    return rv;
}


/*******************************************************************************
* PP model setup                                                   
 */
int pon_ipv6_acl_service_setup(int unit)
{ 
    bcm_error_t rv = BCM_E_NONE;  
    int index;
    bcm_vswitch_cross_connect_t gports;
    int service_mode;
    bcm_tunnel_id_t tunnel_id;
    bcm_vlan_t up_ovlan;
    bcm_vlan_t up_ivlan;
    uint8 up_pcp;
    bcm_ethertype_t up_ethertype;
    bcm_vlan_t down_ovlan;
    bcm_vlan_t down_ivlan;
    bcm_if_t encap_id;
    bcm_gport_t pon_gport, nni_gport;
    int pon_lif_type, nni_lif_type;
    bcm_vlan_action_set_t action;

    rv = vswitch_create(unit, &n_1_service_vsi);
    if (rv != BCM_E_NONE) {
        printf("uc vswitch_create failed $rv\n");
        return rv;
    }

    for (index = 0; index < num_of_n_1_services; index++)    
    {        
        pon_gport = 0;
        nni_gport = 0;
        service_mode = pon_n_1_service_info[index].service_mode;        
        tunnel_id    = pon_n_1_service_info[index].tunnel_id;        
        pon_lif_type = pon_n_1_service_info[index].up_lif_type;        
        up_ovlan     = pon_n_1_service_info[index].up_ovlan;        
        up_ivlan     = pon_n_1_service_info[index].up_ivlan;        
        up_pcp       = pon_n_1_service_info[index].up_pcp;        
        up_ethertype = pon_n_1_service_info[index].up_ethertype;        
        nni_lif_type = pon_n_1_service_info[index].down_lif_type;        
        down_ovlan   = pon_n_1_service_info[index].down_ovlan;        
        down_ivlan   = pon_n_1_service_info[index].down_ivlan; 
        
        /* Create PON LIF */        
        rv = pon_lif_create(unit, pon_port, pon_lif_type, 0, 0, tunnel_id, tunnel_id, up_ovlan, up_ivlan, up_pcp, up_ethertype, &pon_gport); 
        if (rv != BCM_E_NONE) {
            printf("uc pon_lif_create failed (%d) $rv\n", index);
            return rv;
        }
        /* Add PON LIF to vswitch */        
        rv = bcm_vswitch_port_add(unit, n_1_service_vsi, pon_gport);        
        if (rv != BCM_E_NONE) {
            printf("uc bcm_vswitch_port_add failed (%d) $rv\n", index);
            return rv;
        }
        /*For downstream*/
        rv = multicast_vlan_port_add(unit, n_1_service_vsi+lif_offset, pon_port, pon_gport);
        if (rv != BCM_E_NONE) {
            printf("uc multicast_vlan_port_add failed (%d) $rv\n", index);
            return rv;
        }
        /* Set PON LIF ingress VLAN editor */       
        rv = pon_port_ingress_vt_set(unit, service_mode, down_ovlan, down_ivlan, pon_gport, pon_in_map_id_dft, NULL); 
        if (rv != BCM_E_NONE) {
            printf("uc pon_port_ingress_vt_set failed (%d) $rv\n", index);
            return rv;
        }
        /* Set PON LIF egress VLAN editor */        
        rv = pon_port_egress_vt_set(unit, service_mode, tunnel_id, up_ovlan, up_ivlan, pon_gport);   
        if (rv != BCM_E_NONE) {
            printf("uc pon_port_egress_vt_set failed (%d) $rv\n", index);
            return rv;
        }

        /*Only create one NNI LIF*/        
        if (index == 0) {                        
            /* Create NNI LIF */            
            rv = nni_lif_create(unit, nni_port, nni_lif_type, 0, down_ovlan, down_ivlan, 0, &nni_gport, &encap_id);
            if (rv != BCM_E_NONE) {
                printf("uc nni_lif_create failed (%d) $rv\n", index);
                return rv;
            }
            /*For upstream*/            
            rv = multicast_vlan_port_add(unit, n_1_service_vsi, nni_port, nni_gport); 
            if (rv != BCM_E_NONE) {
                printf("uc multicast_vlan_port_add failed (%d) $rv\n", index);
                return rv;
            }
            /* add to vswitch */            
            rv = bcm_vswitch_port_add(unit, n_1_service_vsi, nni_gport);            
            if (rv != BCM_E_NONE) {
                printf("uc bcm_vswitch_port_add failed (%d) $rv\n", index);
                return rv;
            }      
            
            /* Set the multicast group offset to flood downstream packets in multicast group(n_1_service_vsi+lif_offset) */
            bcm_port_control_set(unit, nni_gport, bcmPortControlFloodUnknownUcastGroup, lif_offset);
            bcm_port_control_set(unit, nni_gport, bcmPortControlFloodUnknownMcastGroup, lif_offset);
            bcm_port_control_set(unit, nni_gport, bcmPortControlFloodBroadcastGroup, lif_offset);      
        } else {           
            nni_gport = pon_n_1_service_info[0].nni_gport;        
        }

        pon_n_1_service_info[index].pon_gport = pon_gport;        
        pon_n_1_service_info[index].nni_gport = nni_gport;
    }
    
    return rv;
}


/*******************************************************************************
* PP model cleanup                                                   
 */
int pon_ipv6_acl_service_cleanup(int unit)
{ 
    bcm_error_t rv = BCM_E_NONE;
    int index;

    for (index = 0; index < num_of_n_1_services; index++)
    {
        rv = vswitch_delete_port(unit, n_1_service_vsi, pon_n_1_service_info[index].pon_gport);
        if (rv != BCM_E_NONE) {
            printf("uc vswitch_delete_port failed (%d) $rv\n", index);
            return rv;
        }
    }
    rv = vswitch_delete_port(unit, n_1_service_vsi, pon_n_1_service_info[0].nni_gport);
    if (rv != BCM_E_NONE) {
        printf("uc vswitch_delete_port failed (%d) $rv\n", 0);
        return rv;
    }
    
    rv = vswitch_delete(unit, n_1_service_vsi);
    if (rv != BCM_E_NONE) {
        printf("uc vswitch_delete failed $rv\n");
        return rv;
    }    
    
    return rv;
}


