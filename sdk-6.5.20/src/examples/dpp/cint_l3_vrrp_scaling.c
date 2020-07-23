/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

int vtt_vrrp_group;
int vrrp_vsi_da_field_processor_create(/* in */ int unit)

{
    int group_priority = 11;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    int grp;
    int rv;

    /* Create Qualifier Set*/
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngressVlanTranslation);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyTranslatedOuterVlanId);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifySrcClassField);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyTranslatedInnerVlanId);

    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstMac);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyOamInLifId);

    /* Create Action Set*/ 
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionIngressGportSet);
     
    rv = bcm_field_group_create(unit, qset, group_priority, &grp);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_group_create_id\n");
        return rv;
    }

    rv = bcm_field_group_action_set(unit, grp, aset);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_group_action_set\n");
        return rv;
    }

    vtt_vrrp_group = grp;
	
    return BCM_E_NONE;
}



/*create LIF with no match criteria */
int
vrrp_vsi_da_tcam_entry_create(
    int unit,
    bcm_port_t port,  
    bcm_vlan_t outer_vlan,
	bcm_vlan_t inner_vlan,
    bcm_mac_t dst_mac,
	bcm_port_ethertype_t ether_type,
    bcm_gport_t garbage_gport
    ){
    bcm_field_entry_t ent;
    int grp;
	int rv;
    bcm_mac_t mac_mask = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

    /* always make sure highest bit of ether_type is 0x0, its not needed, all is needed to distinct between
    IPV4 and IPV6 is 1 bit(lowest bit is enough) */
    ether_type = ether_type & 0x7fff;

    grp = vtt_vrrp_group;
	
    rv = bcm_field_entry_create(unit, grp, &ent);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_entry_create\n");
        return rv;
    }

    /* qualifier for outerVlanId (InitialVlanId in case entry is for non-tagged), custom behaviour */
    rv = bcm_field_qualify_TranslatedOuterVlanId(unit, ent, outer_vlan, 0xfff);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_qualify_OuterVlanId\n");
        return rv;
    }
       
    /* qualifier for VD, custom behaviour */
    rv = bcm_field_qualify_SrcClassField(unit, ent, port, 0xff);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_qualify_ClassId\n");
        return rv;
    }

    /* qualifier for innerVlanId (0x0 in case entry is for non-tagged/single-tagged), custom behaviour */
    rv = bcm_field_qualify_TranslatedInnerVlanId(unit, ent, inner_vlan, 0xfff);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_qualify_InnerVlanId\n");
        return rv;
    }

    /* qualifier for DA */
    rv = bcm_field_qualify_DstMac(unit, ent, dst_mac, mac_mask);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_qualify_DstMac\n");
        return result;
    }

    /* qualifier for EtherType, custom behaviour */
    /* always mask the highest bit of this qualifier , all is needed is really 1 lowest bit*/
    rv = bcm_field_qualify_OamInLifId(unit, ent, ether_type, 0x0fff);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_qualify_EtherType\n");
        return rv;
    }

    /* for this type of entires this value doesnt mean anything, just guive a dummy gport */
    rv = bcm_field_action_add(unit, ent, bcmFieldActionIngressGportSet, garbage_gport, 0); 
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_action_add (this one)\n");
        return rv;
    }
    
    rv = bcm_field_entry_install(unit, ent);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_entry_install\n");
        return rv;
    }

    return BCM_E_NONE;
}

int dummy_gport = -1;

int vlan_port_create_dummy(int unit, int dummy_cpu_port){

    int rv = BCM_E_NONE;
    bcm_vlan_port_t vp;
    bcm_vlan_t vlan = 17;
    int vsi = 17;

    if (dummy_gport != -1) {
        printf("already created! you need only one in the system\n");
        return BCM_E_NONE;
    }

    bcm_vlan_port_t_init(&vp);
    vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vp.port = dummy_cpu_port;
    vp.match_vlan = vlan; 
    vp.egress_vlan = vlan;
    vp.vsi = 0;    
    vp.flags = BCM_VLAN_PORT_OUTER_VLAN_PRESERVE|BCM_VLAN_PORT_INNER_VLAN_PRESERVE|BCM_VLAN_PORT_EXTENDED;

    rv = bcm_vlan_port_create(unit, &vp); 
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n"); 
        print rv;
        return rv;
    }

    dummy_gport =  vp.vlan_port_id;
    return rv;
}

int vrrp_main(int unit, bcm_port_t port) {
	bcm_port_t vd = 0;
	bcm_vlan_t outer_vlan = 15;
	bcm_vlan_t inner_vlan = 0;
	bcm_mac_t dst_mac = {0x00,0x0c,0x00,0x02,0x00,0x01};
	bcm_port_ethertype_t ether_type = 0x0800;
	int result;
	
	vlan_port_create_dummy(0, 203);
	
    result = bcm_port_control_set(unit, port, bcmPortControlVrrpScalingPort, 1); 
	if (result != BCM_E_NONE) {
		printf("Error, bcm_port_control_set bcmPortControlVrrpScalingPort\n");
		return result;
	}
    
	
	vrrp_vsi_da_field_processor_create(unit);
	result = vrrp_vsi_da_tcam_entry_create(unit, vd, outer_vlan, inner_vlan, dst_mac, ether_type, dummy_gport);
	if (result != BCM_E_NONE) {
		printf("Error creating VRRP table\n");
		return result;
	}
	result = vrrp_vsi_da_tcam_entry_create(unit, vd, 5, inner_vlan, dst_mac, ether_type, dummy_gport);
	if (result != BCM_E_NONE) {
		printf("Error creating VRRP entry\n");
		return result;
	}
	result = vrrp_vsi_da_tcam_entry_create(unit, vd, 5, 7, dst_mac, ether_type, dummy_gport);
	if (result != BCM_E_NONE) {
		printf("Error creating VRRP entry\n");
		return result;
	}
	return BCM_E_NONE;
}