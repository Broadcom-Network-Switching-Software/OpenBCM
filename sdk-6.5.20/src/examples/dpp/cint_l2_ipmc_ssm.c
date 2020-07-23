/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*

 This feature supports bridging of IPvX IP-compatible multicast utilizing the LEM and PMF. 
 The purpose is to conserve internal TCAM for IPMC Source Specific Multicast bridging in Arad+.
 This feature is enabled via custom soc property, ipmc_l2_ssm_mode_lem.

 Set custom_feature_ipmc_l2_ssm_mode_lem=1


 IPv4 Multicast:
 - 1st LEM lookup: {FID(15), DIP(28}, where DIP=0xE/4 {224.0.0.0 to 239.255.255.255}

 IPv4 Source Specific Multicast:
 - 1st LEM lookup: {FID(15), DIP(28}, where DIP=0xE/4 {224.0.0.0 to 239.255.255.255}
 - 2nd LEM lookup: {FID(15), DIP(24), SIP(32)}
 --- 2nd LEM lookup has higher priority than 1st LEM lookup

 IPv6 Multicast:
 - PMF: {FID(15), DIP(120), Out-LIF(12), LEM-1st-lookup-found, Packet-Format-Qualifier1.Next-Protocol}
 --- Out-LIF and LEM-1st-lookup-found are relevant for IPv6 Source Specific Multicast only.

 IPv6 Source Specific Multicast:
 - 1st LEM lookup: {FID(15), SMAC(48)}, where SMAC[47:0] is the unique representation of the SIP for SSM
 - PMF: {FID(15), DIP(120), Out-LIF(12), LEM-1st-lookup-found, Packet-Format-Qualifier1.Next-Protocol}
 --- Out-LIF is the result of the LEM-1st-lookup -- compressed SIP 

 PMF filtering for IPvX IPMC miss:
 - A catchall entry is added to the PMF DB. If LEM-1st-lookup-found is clear, action is drop.

*/

static int l2_ip6mc_qual_ids[5];
static int sa_eui_64=0;
static int ipv4_dip_0=1;
static int ipv4_dip_1=2;
static int ipv4_dip_2=3;
static int ipv4_dip_3=4;

int l2_ipmc_ssm_pmf_init(int unit, bcm_field_group_t group_id, int group_priority, int default_entry_priority ) 
{
   int					rv = BCM_E_NONE;
   bcm_field_group_config_t 		l2_ipmc_group;
   int					l2_ip4mc_presel_id;
   int					l2_ip6mc_presel_id;
   bcm_field_presel_set_t 		l2_ipmc_pset;
   bcm_field_aset_t 			l2_ipmc_aset;
   int 					l2_ipmc_default_entry;
   bcm_field_data_qualifier_t		l2_ip6mc_dq[5];
   uint16				offset[5] = {0, 0, 4, 0, 4};
   int					length[5] = {14, 4, 4, 4, 3};
   bcm_field_qualify_t			qualifier[5] = {bcmFieldQualifyOutVPort, bcmFieldQualifyDstIp6Low, bcmFieldQualifyDstIp6Low, bcmFieldQualifyDstIp6High, bcmFieldQualifyDstIp6High};
   uint32				flags[5] = {BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES, 0, 0, 0, 0};


   bcm_rx_trap_config_t ipmc_invalid_ip_config;
   int 			ipmc_invalid_ip_id;
   int			qual_index;

   /* 
    * (Optional) Enable the IP Compatible-Multicast Invalid IP trap
    * Set the trap strength higher than the PMF strength 
    */
   bcm_rx_trap_config_t_init(&ipmc_invalid_ip_config);
   ipmc_invalid_ip_config.flags = (BCM_RX_TRAP_UPDATE_DEST);
   ipmc_invalid_ip_config.trap_strength = 6;
   ipmc_invalid_ip_config.dest_port=BCM_GPORT_BLACK_HOLE;
   rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapIpCompMcInvalidIp, &ipmc_invalid_ip_id);
   if (BCM_E_NONE != rv) {
       printf("Error in bcm_rx_trap_type_create, rv %d\n", rv);
       return rv;
   }
   rv = bcm_rx_trap_set(unit, ipmc_invalid_ip_id, &ipmc_invalid_ip_config);
   if (BCM_E_NONE != rv) {
       printf("Error in bcm_rx_trap_type_create, rv %d\n", rv);
       return rv;
   }

   /*
    * Create a PMF TCAM DB
    */
   rv = bcm_field_presel_create(unit, &l2_ip4mc_presel_id);
   if (BCM_E_NONE != rv) {
       printf("Error in bcm_field_presel_create, rv %d\n", rv);
       return rv;
   }

   /*
    * Set the preselector set to {Forwarding-Code = L2, Forwarding-Profile = L2 IPMC}
    */
   rv = bcm_field_qualify_Stage(unit, l2_ip4mc_presel_id | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageIngress);
   if (BCM_E_NONE != rv) {
       printf("Error in bcm_field_qualify_Stage, rv %d\n", rv);
       return rv;
   }
   rv = bcm_field_qualify_ForwardingType(unit, l2_ip4mc_presel_id | BCM_FIELD_QUALIFY_PRESEL, bcmFieldForwardingTypeL2);
   if (BCM_E_NONE != rv) {
       printf("Error in bcm_field_qualify_ForwardingType, rv %d\n", rv);
       return rv;
   }
   rv = bcm_field_qualify_AppType(unit, l2_ip4mc_presel_id | BCM_FIELD_QUALIFY_PRESEL, bcmFieldAppTypeIp4McastL2Ssm);
   if (BCM_E_NONE != rv) {
       printf("Error in bcm_field_qualify_AppType, rv %d\n", rv);
       return rv;
   }

   rv = bcm_field_presel_create(unit, &l2_ip6mc_presel_id);
   if (BCM_E_NONE != rv) {
       printf("Error in bcm_field_presel_create, rv %d\n", rv);
       return rv;
   }

   /*
    * Set the preselector set to {Forwarding-Code = L2, Forwarding-Profile = L2 IPMC}
    */
   rv = bcm_field_qualify_Stage(unit, l2_ip6mc_presel_id | BCM_FIELD_QUALIFY_PRESEL, bcmFieldStageIngress);
   if (BCM_E_NONE != rv) {
       printf("Error in bcm_field_qualify_Stage, rv %d\n", rv);
       return rv;
   }
   rv = bcm_field_qualify_ForwardingType(unit, l2_ip6mc_presel_id | BCM_FIELD_QUALIFY_PRESEL, bcmFieldForwardingTypeL2);
   if (BCM_E_NONE != rv) {
       printf("Error in bcm_field_qualify_ForwardingType, rv %d\n", rv);
       return rv;
   }
   rv = bcm_field_qualify_AppType(unit, l2_ip6mc_presel_id | BCM_FIELD_QUALIFY_PRESEL, bcmFieldAppTypeIp6McastL2Ssm);
   if (BCM_E_NONE != rv) {
       printf("Error in bcm_field_qualify_AppType, rv %d\n", rv);
       return rv;
   }

   BCM_FIELD_PRESEL_INIT(l2_ipmc_pset);
   BCM_FIELD_PRESEL_ADD(l2_ipmc_pset, l2_ip4mc_presel_id);
   BCM_FIELD_PRESEL_ADD(l2_ipmc_pset, l2_ip6mc_presel_id);

   BCM_FIELD_QSET_INIT(l2_ipmc_group.qset);
   BCM_FIELD_QSET_ADD(l2_ipmc_group.qset, bcmFieldQualifyL2SrcHit);
   BCM_FIELD_QSET_ADD(l2_ipmc_group.qset, bcmFieldQualifyLearnVlan);	/* FID */
   BCM_FIELD_QSET_ADD(l2_ipmc_group.qset, bcmFieldQualifyIpType);	/* FID */

   for (qual_index = 0; qual_index < 5; qual_index++) {
       l2_ip6mc_dq[qual_index].flags = flags[qual_index] | BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED;
       l2_ip6mc_dq[qual_index].qualifier = qualifier[qual_index]; 
       l2_ip6mc_dq[qual_index].stage = bcmFieldStageIngress;
       l2_ip6mc_dq[qual_index].offset = offset[qual_index];
       l2_ip6mc_dq[qual_index].length = length[qual_index];
       rv = bcm_field_data_qualifier_create(unit, &l2_ip6mc_dq[qual_index]);
   if (BCM_E_NONE != rv) {
       printf("Error in bcm_field_data_qualifier_create, rv %d\n", rv);
       return rv;
   }
       rv = bcm_field_qset_data_qualifier_add(unit, l2_ipmc_group.qset, l2_ip6mc_dq[qual_index].qual_id);
   if (BCM_E_NONE != rv) {
       printf("Error in bcm_field_qset_data_qualifier_add, rv %d\n", rv);
       return rv;
   }
       l2_ip6mc_qual_ids[qual_index] = l2_ip6mc_dq[qual_index].qual_id;
   }

   BCM_FIELD_ASET_INIT(l2_ipmc_aset);
   BCM_FIELD_ASET_ADD(l2_ipmc_aset, bcmFieldActionDrop);
   BCM_FIELD_ASET_ADD(l2_ipmc_aset, bcmFieldActionRedirectMcast);
   BCM_FIELD_ASET_ADD(l2_ipmc_aset, bcmFieldActionVportNew);

   l2_ipmc_group.priority = group_priority;
   l2_ipmc_group.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET | BCM_FIELD_GROUP_CREATE_WITH_ID;
   l2_ipmc_group.preselset = l2_ipmc_pset;
   l2_ipmc_group.mode = bcmFieldGroupModeAuto;
   l2_ipmc_group.group = group_id;
   rv = bcm_field_group_config_create(unit, &l2_ipmc_group);
   if (BCM_E_NONE != rv) {
       printf("Error in bcm_field_group_config_create, rv %d\n", rv);
       return rv;
   }

   rv = bcm_field_group_action_set(unit, l2_ipmc_group.group, l2_ipmc_aset);
   if (BCM_E_NONE != rv) {
       printf("Error in bcm_field_group_action_set, rv %d\n", rv);
       return rv;
   }

   rv = bcm_field_group_install(unit, l2_ipmc_group.group);
   if (BCM_E_NONE != rv) {
       printf("Error in bcm_field_group_install, rv %d\n", rv);
       return rv;
   }

   /* Install a catch-all entry
    * For IPv4 IPMC, a miss in LEM 1st lookup indicates a miss
    */
   rv = bcm_field_entry_create(unit, l2_ipmc_group.group, &l2_ipmc_default_entry);
   if (BCM_E_NONE != rv) {
       printf("Error in bcm_field_entry_create, rv %d\n", rv);
       return rv;
   }

   rv = bcm_field_qualify_L2SrcHit(unit, l2_ipmc_default_entry, 0, 0x1);
   if (BCM_E_NONE != rv) {
       printf("Error in bcm_field_qualify_L2SrcHit, rv %d\n", rv);
       return rv;
   }
   rv = bcm_field_action_add(unit, l2_ipmc_default_entry, bcmFieldActionDrop, 0, 0);
   if (BCM_E_NONE != rv) {
       printf("Error in bcm_field_action_add, rv %d\n", rv);
       return rv;
   }
   rv = bcm_field_action_add(unit, l2_ipmc_default_entry, bcmFieldActionVportNew, BCM_GPORT_INVALID, 0);
   if (BCM_E_NONE != rv) {
       printf("Error in bcm_field_action_add, rv %d\n", rv);
       return rv;
   }
   rv = bcm_field_entry_prio_set(unit, l2_ipmc_default_entry, default_entry_priority);
   if (BCM_E_NONE != rv) {
       printf("Error in bcm_field_entry_prio_set, rv %d\n", rv);
       return rv;
   }

   rv =  bcm_field_entry_install(unit, l2_ipmc_default_entry);
   if (BCM_E_NONE != rv) {
       printf("Error in bcm_field_entry_install, rv %d\n", rv);
       return rv;
   }

   return rv;
}


int l2_ipmc_ipv6_entry_add(int unit, bcm_field_group_t group_id, bcm_ipmc_addr_t ipmc_data, int entry_priority, bcm_field_entry_t *l2_ipmc_ip6_entry) 
{
    int					rv = BCM_E_NONE;
    uint8 mask_mc_ip6_addr[16] = {0xff, 0xff, 0xff, 0xff, 
    				  0xff, 0xff, 0xff, 0xff, 
    				  0xff, 0xff, 0xff, 0xff, 
    				  0xff, 0xff, 0xff, 0xff};

   rv = bcm_field_entry_create(unit, group_id, l2_ipmc_ip6_entry);
   if (BCM_E_NONE != rv) {
       printf("Error in bcm_field_entry_create, rv %d\n", rv);
       return rv;
   }
 
    rv = bcm_field_qualify_LearnVlan(unit, *l2_ipmc_ip6_entry, ipmc_data.vid, 0xffff);
   if (BCM_E_NONE != rv) {
       printf("Error in bcm_field_qualify_LearnVlan, rv %d\n", rv);
       return rv;
   }
    rv = bcm_field_qualify_IpType(unit, *l2_ipmc_ip6_entry, bcmFieldIpTypeIpv6);
   if (BCM_E_NONE != rv) {
       printf("Error in bcm_field_qualify_IpType, rv %d\n", rv);
       return rv;
   }
    rv = bcm_field_qualify_data(unit, *l2_ipmc_ip6_entry, l2_ip6mc_qual_ids[ipv4_dip_0], &ipmc_data.mc_ip6_addr[12], &mask_mc_ip6_addr[12], 4);
   if (BCM_E_NONE != rv) {
       printf("Error in bcm_field_qualify_data, rv %d\n", rv);
       return rv;
   }
    rv = bcm_field_qualify_data(unit, *l2_ipmc_ip6_entry, l2_ip6mc_qual_ids[ipv4_dip_1], &ipmc_data.mc_ip6_addr[8], &mask_mc_ip6_addr[8], 4);
   if (BCM_E_NONE != rv) {
       printf("Error in bcm_field_qualify_data, rv %d\n", rv);
       return rv;
   }
    rv = bcm_field_qualify_data(unit, *l2_ipmc_ip6_entry, l2_ip6mc_qual_ids[ipv4_dip_2], &ipmc_data.mc_ip6_addr[4], &mask_mc_ip6_addr[4], 4);
   if (BCM_E_NONE != rv) {
       printf("Error in bcm_field_qualify_data, rv %d\n", rv);
       return rv;
   }
    rv = bcm_field_qualify_data(unit, *l2_ipmc_ip6_entry, l2_ip6mc_qual_ids[ipv4_dip_3], &ipmc_data.mc_ip6_addr[1], &mask_mc_ip6_addr[1], 3);
   if (BCM_E_NONE != rv) {
       printf("Error in bcm_field_qualify_data, rv %d\n", rv);
       return rv;
   }
    rv = bcm_field_action_add(unit, *l2_ipmc_ip6_entry, bcmFieldActionRedirectMcast, ipmc_data.group, 0);
   if (BCM_E_NONE != rv) {
       printf("Error in bcm_field_action_add, rv %d\n", rv);
       return rv;
   }
    rv = bcm_field_action_add(unit, *l2_ipmc_ip6_entry, bcmFieldActionVportNew, 0, 0);
   if (BCM_E_NONE != rv) {
       printf("Error in bcm_field_action_add, rv %d\n", rv);
       return rv;
   }
    rv = bcm_field_entry_prio_set(unit, *l2_ipmc_ip6_entry, entry_priority);
   if (BCM_E_NONE != rv) {
       printf("Error in bcm_field_entry_prio_set, rv %d\n", rv);
       return rv;
   }
   rv =  bcm_field_entry_install(unit, *l2_ipmc_ip6_entry);
   if (BCM_E_NONE != rv) {
       printf("Error in bcm_field_entry_install, rv %d\n", rv);
       return rv;
   }
   return rv;
}

int l2_ipmc_ipv6_ssm_entry_add(int unit, bcm_field_group_t group_id, bcm_ipmc_addr_t ipmc_data, uint16 sip_index, int entry_priority, bcm_field_entry_t *l2_ipmc_ssm_ip6_entry) 
{
    int					rv = BCM_E_NONE;
    uint8 sip_index_data[2];
    uint8 sip_index_mask[2] = {0x2f, 0xff};
    uint8 mask_mc_ip6_addr[16] = {0xff, 0xff, 0xff, 0xff, 
    				  0xff, 0xff, 0xff, 0xff, 
    				  0xff, 0xff, 0xff, 0xff, 
    				  0xff, 0xff, 0xff, 0xff};

    sip_index_data[0] = (sip_index >> 8);
    sip_index_data[1] = (sip_index & 0xff);

    rv = bcm_field_entry_create(unit, group_id, l2_ipmc_ssm_ip6_entry);
   if (BCM_E_NONE != rv) {
       printf("Error in bcm_field_entry_create, rv %d\n", rv);
       return rv;
   }
    rv = bcm_field_qualify_LearnVlan(unit, *l2_ipmc_ssm_ip6_entry, ipmc_data.vid, 0xffff);
   if (BCM_E_NONE != rv) {
       printf("Error in bcm_field_qualify_LearnVlan, rv %d\n", rv);
       return rv;
   }
    rv = bcm_field_qualify_L2SrcHit(unit, *l2_ipmc_ssm_ip6_entry, 1, 0x1);
   if (BCM_E_NONE != rv) {
       printf("Error in bcm_field_qualify_L2SrcHit, rv %d\n", rv);
       return rv;
   }
    rv = bcm_field_qualify_IpType(unit, *l2_ipmc_ssm_ip6_entry, bcmFieldIpTypeIpv6);
   if (BCM_E_NONE != rv) {
       printf("Error in bcm_field_qualify_IpType, rv %d\n", rv);
       return rv;
   }
    rv = bcm_field_qualify_data(unit, *l2_ipmc_ssm_ip6_entry, l2_ip6mc_qual_ids[ipv4_dip_0], &ipmc_data.mc_ip6_addr[12], &mask_mc_ip6_addr[12], 4);
   if (BCM_E_NONE != rv) {
       printf("Error in bcm_field_qualify_data, rv %d\n", rv);
       return rv;
   }
    rv = bcm_field_qualify_data(unit, *l2_ipmc_ssm_ip6_entry, l2_ip6mc_qual_ids[ipv4_dip_1], &ipmc_data.mc_ip6_addr[8], &mask_mc_ip6_addr[8], 4);
   if (BCM_E_NONE != rv) {
       printf("Error in bcm_field_qualify_data, rv %d\n", rv);
       return rv;
   }
    rv = bcm_field_qualify_data(unit, *l2_ipmc_ssm_ip6_entry, l2_ip6mc_qual_ids[ipv4_dip_2], &ipmc_data.mc_ip6_addr[4], &mask_mc_ip6_addr[4], 4);
   if (BCM_E_NONE != rv) {
       printf("Error in bcm_field_qualify_data, rv %d\n", rv);
       return rv;
   }
    rv = bcm_field_qualify_data(unit, *l2_ipmc_ssm_ip6_entry, l2_ip6mc_qual_ids[ipv4_dip_3], &ipmc_data.mc_ip6_addr[1], &mask_mc_ip6_addr[1], 3);
   if (BCM_E_NONE != rv) {
       printf("Error in bcm_field_qualify_data, rv %d\n", rv);
       return rv;
   }
    rv = bcm_field_qualify_data(unit, *l2_ipmc_ssm_ip6_entry, l2_ip6mc_qual_ids[sa_eui_64], sip_index_data, sip_index_mask, 2);
   if (BCM_E_NONE != rv) {
       printf("Error in bcm_field_qualify_data, rv %d\n", rv);
       return rv;
   }
    rv = bcm_field_action_add(unit, *l2_ipmc_ssm_ip6_entry, bcmFieldActionRedirectMcast, ipmc_data.group, 0);
   if (BCM_E_NONE != rv) {
       printf("Error in bcm_field_action_add, rv %d\n", rv);
       return rv;
   }
    rv = bcm_field_action_add(unit, *l2_ipmc_ssm_ip6_entry, bcmFieldActionVportNew, 0, 0);
   if (BCM_E_NONE != rv) {
       printf("Error in bcm_field_action_add, rv %d\n", rv);
       return rv;
   }
    rv = bcm_field_entry_prio_set(unit, *l2_ipmc_ssm_ip6_entry, entry_priority);
   if (BCM_E_NONE != rv) {
       printf("Error in bcm_field_entry_prio_set, rv %d\n", rv);
       return rv;
   }
   rv = bcm_field_entry_install(unit, *l2_ipmc_ssm_ip6_entry);
   if (BCM_E_NONE != rv) {
       printf("Error in bcm_field_entry_install, rv %d\n", rv);
       return rv;
   }
   return rv;
}

