/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/*
 * File: cint_pon_anti_spoofing.c
 *
 * Purpose: An example of how to use BCM APIs to implement anti-spoofing(L3 Source Binding) function in pon appplication service.
 *          After enable anti-spoofing function, system will permit/deny traffic pass base source bind information 
 *          of pon gport. SAs of denied packets are not leant.
 *
 *          Here is the binding information, and the mode is decided by IN-lIF bind configuration at  
 *          |----------------|---------------------------------------------------------------|
 *          |     Mode       |     binding configuration                                     |
 *          |----------------|---------------------------------------------------------------|
 *          | IPV4 DHCP      | SIP, SA and flag isn't BCM_L3_SOURCE_BIND_IP6                 |
 *          |----------------|---------------------------------------------------------------|
 *          | IPV4 STATIC    | SIP, SA is zero and flag isn't BCM_L3_SOURCE_BIND_IP6         |
 *          |----------------|---------------------------------------------------------------|
 *          | IPV6 DHCP      | SIPV6, SA and flag is BCM_L3_SOURCE_BIND_IP6                  |
 *          |----------------|---------------------------------------------------------------|
 *          | IPV6 STATIC    | SIPV6, SA is zero and BCM_L3_SOURCE_BIND_IP6                  |
 *          |----------------|---------------------------------------------------------------|
 * Calling sequence:
 *
 * Initialization:
 *  1. Set the following port configureations to config-sand.bcm
 *        ucode_port_128.BCM88650=10GBase-R15
 *        ucode_port_4.BCM88650=10GBase-R14
 *  2. Add the following PON application and anti-spoofing enabling configureations to config-sand.bcm
 *        pon_application_support_enabled_4.BCM88650=TRUE
 *        l3_source_bind_mode=IP
 *  3. Enable pon service:PON port will recognize tunnel Id(100) CTAG(10),NNI port will recognize STAG(100), CTAG(10).
 *        - call pon_anti_spoofing_app()
 *  4. Set IPV4, IPV6 source bind configuration, details:
 *        IPV4 DHCP:   SIP-10.10.10.10, SA-00:00:01:00:00:01
 *        IPV4 STATIC: SIP-10.10.10.11
 *        IPV6 DHCP:   SIP-2000::2, SA-00:00:03:00:00:01
 *        IPV6 STATIC: SIP-2000::3
 *        - call pon_anti_spoofing_enable()
 *  5. Enable FP to drop un-matched traffic.
 *        - call pon_anti_spoofing_drop()
 *  6. Clean all above configuration.
 *        - call pon_anti_spoofing_app_clean()
 *
 * To Activate Above Settings:
 *      BCM> cint examples/dpp/cint_port_tpid.c
 *      BCM> cint examples/dpp/pon/cint_pon_utils.c
 *      BCM> cint examples/dpp/pon/cint_pon_anti_spoofing.c
 *      BCM> cint
 *      cint> pon_anti_spoofing_app(unit, pon_port, nni_port);
 *      cint> pon_anti_spoofing_enable(unit);
 *      cint> pon_anti_spoofing_drop(unit);
 *
 * To clean all above configuration:
 *      cint> pon_anti_spoofing_app_clean(unit);
 */
 
struct pon_anti_spoof_info_s {
    bcm_vlan_t  vsi;
    bcm_gport_t pon_gport;
    bcm_gport_t nni_gport;
    bcm_port_t pon_port;
    bcm_port_t nni_port;
    int tunnel_id;
    int nni_svlan;
    int pon_cvlan;
    int lif_offset;
    bcm_field_group_config_t grp;
    bcm_field_entry_t ent[4];
};

pon_anti_spoof_info_s pon_anti_spoofing_info;
bcm_l3_source_bind_t src_bind[10];
bcm_gport_t pon_dscp_remark_gport;
int bind_entries_num = 0;

/* Initialize PON application configurations.
 * Example: pon_anti_spoofing_init(0, 4, 128);
 */
int pon_anti_spoofing_init(int unit, bcm_port_t pon_port, bcm_port_t nni_port)
{
    bcm_ip6_t sip1;
    int index = 0;

    sal_memset(&pon_anti_spoofing_info, 0, sizeof(pon_anti_spoofing_info));
    pon_anti_spoofing_info.vsi = 0;
    pon_anti_spoofing_info.pon_port = pon_port;
    pon_anti_spoofing_info.nni_port = nni_port;
    pon_anti_spoofing_info.tunnel_id = 100;
    pon_anti_spoofing_info.nni_svlan = 100;
    pon_anti_spoofing_info.pon_cvlan = 10;

    /* Disable trunk in NNI port */
    pon_app_init(unit, pon_port, nni_port, 0, 0);
    /* lif_offset is updated in pon_app_init() based on device type */
    pon_anti_spoofing_info.lif_offset = lif_offset;
    /* src_bind[0] -- ipv4 dhcp */
    /* 10.10.10.10 */
    src_bind[index].ip = 168430090;
    src_bind[index].mac[0] = 0x0;
    src_bind[index].mac[1] = 0x0;
    src_bind[index].mac[2] = 0x1;
    src_bind[index].mac[3] = 0x0;
    src_bind[index].mac[4] = 0x0;
    src_bind[index].mac[5] = 0x1;
    index++;

    /* src_bind[1] -- ipv4 static */
    /* 10.10.10.11 */
    src_bind[index].ip = 168430091;
    index++;

    /* src_bind[2] -- ipv6 dhcp */
    src_bind[index].ip6[15]= 0x02;
    src_bind[index].ip6[0] = 0x20;
    src_bind[index].mac[0] = 0x0;
    src_bind[index].mac[1] = 0x0;
    src_bind[index].mac[2] = 0x3;
    src_bind[index].mac[3] = 0x0;
    src_bind[index].mac[4] = 0x0;
    src_bind[index].mac[5] = 0x1;
    src_bind[index].flags |= BCM_L3_SOURCE_BIND_IP6;
    index++;

    /* src_bind[3] -- ipv6 static */
    src_bind[index].flags |= BCM_L3_SOURCE_BIND_IP6;
    src_bind[index].ip6[15]= 0x03;
    src_bind[index].ip6[0] = 0x20;
    index++;

    /* src_bind[4] -- ipv4 dhcp */
    /* 10.10.10.10 */
    src_bind[index].ip = 168430090;
    src_bind[index].mac[0] = 0x1;
    src_bind[index].mac[1] = 0x2;
    src_bind[index].mac[2] = 0x2;
    src_bind[index].mac[3] = 0x3;
    src_bind[index].mac[4] = 0x4;
    src_bind[index].mac[5] = 0x5;
    index++;

    bind_entries_num = index;

}

/*********************************************************************************
* Setup pon anti-spoofing test model(1:1)
*     PON Port 1 or 7  <------------------------------->  VSI  <---> NNI Port 128
*     Pon 1 Tunnel-ID 100 clvan 10 --------------------|---4096  |----------- SVLAN 100 cvlan 10
*                                                 
* steps
 */
int pon_anti_spoofing_app(int unit, bcm_port_t pon, bcm_port_t nni)
{
    int i = 0;
    int rv = 0;
    bcm_if_t encap_id = 0;
    int action_id = 0;

    pon_anti_spoofing_init(unit, pon, nni);

    rv = vswitch_create(unit, &(pon_anti_spoofing_info.vsi));
    if (rv != BCM_E_NONE)
    {
        printf("Error, vswitch_create returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create PON LIF */
    rv = pon_lif_create(unit, 
                        pon_anti_spoofing_info.pon_port, 
                        match_otag,
                        0, 0,
                        pon_anti_spoofing_info.tunnel_id,
                        pon_anti_spoofing_info.tunnel_id,
                        pon_anti_spoofing_info.pon_cvlan,
                        0, 0, 0,
                        &(pon_anti_spoofing_info.pon_gport));
    if (rv != BCM_E_NONE)
    {
        printf("Error, pon_lif_create returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Add PON LIF to vswitch */
    rv = bcm_vswitch_port_add(unit, 
                              pon_anti_spoofing_info.vsi, 
                              pon_anti_spoofing_info.pon_gport
                             );
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vswitch_port_add returned %s\n", bcm_errmsg(rv));
        return rv;
    }
    /*For downstream*/
    rv = multicast_vlan_port_add(unit, 
                                 pon_anti_spoofing_info.vsi+pon_anti_spoofing_info.lif_offset, 
                                 pon_anti_spoofing_info.pon_port, 
                                 pon_anti_spoofing_info.pon_gport);
    if (rv != BCM_E_NONE)
    {
        printf("Error, multicast_vlan_port_add returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Set PON LIF ingress VLAN editor */
    rv = pon_port_ingress_vt_set(unit,
                                 otag_to_o_i_tag,
                                 pon_anti_spoofing_info.nni_svlan,
                                 pon_anti_spoofing_info.pon_cvlan,
                                 pon_anti_spoofing_info.pon_gport,
                                 0, &action_id
                                 );
    if (rv != BCM_E_NONE)
    {
        printf("Error, pon_port_ingress_vt_set returned %s\n", bcm_errmsg(rv));
        return rv;
    }                                             
    /* Set PON LIF egress VLAN editor */
    rv = pon_port_egress_vt_set(unit,
                                otag_to_o_i_tag,
                                pon_anti_spoofing_info.tunnel_id,
                                pon_anti_spoofing_info.pon_cvlan,
                                0, 
                                pon_anti_spoofing_info.pon_gport
                               );
    if (rv != BCM_E_NONE)
    {
        printf("Error, pon_port_egress_vt_set returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create NNI LIF */
    rv = nni_lif_create(unit,
                       pon_anti_spoofing_info.nni_port,
                       match_o_i_tag,
                       0,
                       pon_anti_spoofing_info.nni_svlan,
                       pon_anti_spoofing_info.pon_cvlan,
                       0,
                       &(pon_anti_spoofing_info.nni_gport),
                       &encap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, nni_lif_create returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    if (soc_property_get(unit , "local_switching_enable",0))
    {
        rv = bcm_port_control_set(unit, pon_anti_spoofing_info.nni_gport, bcmPortControlLocalSwitching, 1);
        if (rv != BCM_E_NONE)
        {
            printf("Error, nni_lif_create returned %s\n", bcm_errmsg(rv));
            return rv;
        }
    }

    /*For upstream*/
    rv = multicast_vlan_port_add(unit, 
                              pon_anti_spoofing_info.vsi,
                              pon_anti_spoofing_info.nni_port,
                              pon_anti_spoofing_info.nni_gport);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, multicast_vlan_port_add returned %s nni_gport\n", bcm_errmsg(rv));
        return rv;
    }

    /* add to vswitch */
    rv = bcm_vswitch_port_add(unit,
                           pon_anti_spoofing_info.vsi,
                           pon_anti_spoofing_info.nni_gport);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vswitch_port_add returned %s nni_gport\n", bcm_errmsg(rv));
        return rv;
    }
    
    /* Set the multicast group offset to flood downstream packets in multicast group(vsi+lif_offset) */
    bcm_port_control_set(unit, pon_anti_spoofing_info.nni_gport, bcmPortControlFloodUnknownUcastGroup, pon_anti_spoofing_info.lif_offset);
    bcm_port_control_set(unit, pon_anti_spoofing_info.nni_gport, bcmPortControlFloodUnknownMcastGroup, pon_anti_spoofing_info.lif_offset);
    bcm_port_control_set(unit, pon_anti_spoofing_info.nni_gport, bcmPortControlFloodBroadcastGroup, pon_anti_spoofing_info.lif_offset);

    /* Will set 4 bind configuration in this PON-LIF gport */
    for (i = 0; i < bind_entries_num; i++)
    {
        src_bind[i].port = pon_anti_spoofing_info.pon_gport;
    }

    return rv;
}

/*********************************************************************************
* Enable anti-spoofing function and set IPV4, IPV6 source bind configuration.
*
* Steps
*     1. Call bcm_l3_source_bind_enable_set() to enable anti-spoofing function
*        in PON-LIF gport.
*     2. Call bcm_l3_source_bind_add() to set bind configuration in this PON-LIF gport.
 */
int pon_anti_spoofing_enable(int unit)
{
    int rv = 0;
    int i = 0;

    /* Enable anti-spoffing in this PON-LIF gport */
    rv = bcm_l3_source_bind_enable_set(unit,
                                       pon_anti_spoofing_info.pon_gport,
                                       1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l3_source_bind_enable_set returned %s pon_gport:%d\n", 
                bcm_errmsg(rv), pon_anti_spoofing_info.pon_gport);
        return rv;
    }

    /* Set 4 bind configuration(IPV4 DHCP, IPV4 STATIC, IPV6 DHCP and IPV6 STATIC)
    * in this PON-LIF gport
    */
    for (i = 0; i < bind_entries_num; i++)
    {
        rv = bcm_l3_source_bind_add(unit, 
                                    &(src_bind[i]));
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_l3_source_bind_add returned %s src_bind[%d]\n", bcm_errmsg(rv), i);
            return rv;
        }
    }

    return rv;
}

/*********************************************************************************
* Delete IPV4, IPV6 source bind configuration and disable anti-spoofing function.
*
* Steps
*     1. Call bcm_l3_source_bind_delete() to delete bind configuration 
*        in this PON-LIF gport.
*     2. Call bcm_l3_source_bind_enable_set() to disable anti-spoofing function
*        in PON-LIF gport.
 */
int pon_anti_spoofing_disable(int unit)
{
    int rv = 0;
    int i = 0;


    /* Delete 4 bind configuration(IPV4 DHCP, IPV4 STATIC, IPV6 DHCP and IPV6 STATIC)
    * in this PON-LIF gport
    */
    for (i = 0; i < bind_entries_num; i++)
    {
        rv = bcm_l3_source_bind_delete(unit, 
                                       &(src_bind[i]));
        if (rv != BCM_E_NONE) 
        {
            printf("Error, bcm_l3_source_bind_add returned %s src_bind[%d]\n", bcm_errmsg(rv), i);
            return rv;
        }
    }
    /* Disable anti-spoffing in this gport */
    rv = bcm_l3_source_bind_enable_set(unit,
                                       pon_anti_spoofing_info.pon_gport,
                                       0);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, bcm_l3_source_bind_enable_set returned %s pon_gport:%d\n", 
                bcm_errmsg(rv), pon_anti_spoofing_info.pon_gport);
        return rv;
    }

    return rv;
}

/*********************************************************************************
* Create a field group to drop un-matched traffic.
*
* Steps
*     1. Create a field group
*     2. Attach a action set to field group
*     3. Add entries to drop/pass traffic
 */
int pon_anti_spoofing_drop(int unit)
{
    bcm_field_aset_t aset;
    bcm_field_entry_t *ent[4];
    bcm_field_group_config_t *grp;
    bcm_field_AppType_t app_type[4] = {bcmFieldAppTypeIp4MacSrcBind, bcmFieldAppTypeIp6MacSrcBind,
        bcmFieldAppTypeIp4SrcBind, bcmFieldAppTypeIp6SrcBind};
    int group_priority = 10;
    int result;
    int auxRes;
    int i = 0;

    grp = &(pon_anti_spoofing_info.grp);
    for (i = 0; i < 4; i++)
    {
        ent[i] = &(pon_anti_spoofing_info.ent[i]);
    }

    bcm_field_group_config_t_init(grp);
    grp->group = -1;

    /* Define the QSET
     *  bcmFieldQualifyInPort: Single Input Port
     *  bcmFieldQualifyAppType: Application type, to see if L3 source bind lookup is done
     *  bcmFieldQualifyL2DestHit/bcmFieldQualifyL3SrcRouteHit: L2 Destination/L3 source lookup success(LEM/LPM look up result)
     *  bcmFieldQualifyIpmcHit:   IP Multicast lookup hit(TCAM look up result)
     *  bcmFieldQualifyStageIngress: Field Processor pipeline ingress stage
     */
    BCM_FIELD_QSET_INIT(grp->qset);
    BCM_FIELD_QSET_ADD(grp->qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(grp->qset, bcmFieldQualifyAppType);
    if (is_device_or_above(unit, JERICHO))
    {
        BCM_FIELD_QSET_ADD(grp->qset, bcmFieldQualifyL3SrcRouteHit);
    }
    BCM_FIELD_QSET_ADD(grp->qset, bcmFieldQualifyL2DestHit);
    BCM_FIELD_QSET_ADD(grp->qset, bcmFieldQualifyIpmcHit);
    BCM_FIELD_QSET_ADD(grp->qset, bcmFieldQualifyStageIngress);

    /*  Create the Field group */
    grp->priority = group_priority;
    result = bcm_field_group_config_create(unit, grp);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_group_create returned %s\n", bcm_errmsg(result));
        auxRes = bcm_field_group_destroy(unit, grp->group);
        return result;
    }

    /*
     *  Define the ASET Field Group
     */
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionDoNotLearn);

    /*  Attach the action set */
    result = bcm_field_group_action_set(unit, grp->group, aset);
    if (BCM_E_NONE != result) 
    {
        printf("Error in bcm_field_group_action_set returned %s\n", bcm_errmsg(result));
        auxRes = bcm_field_group_destroy(unit, grp->group);
        return result;
    }

    /*
     *  Add 4 entries to the Field group.
     *  Match the packets on the l3 source bind is done and look up result.
     *    1. Lookup is done for DHCPv4 and LEM look up result is not-found:
     *       drop
     *    2. Lookup is done for DHCPv6 and LEM look up result is not-found:
     *       drop
     *    3. lookup is done for Staticv4 and LEM(ARAD+)/LPM(JERICHO) look up result is not-found:
     *       drop
     *    4. lookup is done for Staticv6 and TCAM(ARAD+)/LPM(JERICHO) lookup result is not-found:
     *       drop
     */
    for (i = 0 ; i < 4; i++)
    {
        result = bcm_field_entry_create(unit, grp->group, ent[i]);
        if (BCM_E_NONE != result)
        {
            printf("Error %d in bcm_field_entry_create returned %s\n", i, bcm_errmsg(result));
            auxRes = bcm_field_group_destroy(unit, grp->group);
            return result;
        }
    }

    /* IPV4/6 DHCP */
    for (i = 0 ; i < 2; i++)
    {
        result = bcm_field_qualify_AppType(unit, *(ent[i]), app_type[i]);
        if (BCM_E_NONE != result)
        {
            printf("Error ent[0] in bcm_field_qualify_AppType returned %s\n", bcm_errmsg(result));
            auxRes = bcm_field_group_destroy(unit, grp->group);
            return result;
        }
        result = bcm_field_qualify_L2DestHit(unit, *(ent[i]), 0x0, 0x01);
        if (BCM_E_NONE != result)
        {
            printf("Error ent[0] in bcm_field_qualify_L2DestHit returned %s\n", bcm_errmsg(result));
            auxRes = bcm_field_group_destroy(unit, grp->group);
            return result;
        }
        result = bcm_field_action_add(unit, *(ent[i]), bcmFieldActionDrop, 0, 0);
        if (BCM_E_NONE != result)
        {
            printf("Error ent[2] in bcm_field_action_add returned %s\n", bcm_errmsg(result));
            auxRes = bcm_field_group_destroy(unit, grp->group);
            return result;
        }

        result = bcm_field_action_add(unit, *(ent[i]), bcmFieldActionDoNotLearn, 0, 0);
        if (BCM_E_NONE != result)
        {
            printf("Error, bcm_field_action_add ent[2] - bcmFieldActionDoNotLearn result:%s\n", bcm_errmsg(result));
            auxRes = bcm_field_group_destroy(unit, grp->group);
            return result;
        }
        result = bcm_field_entry_prio_set(unit, *(ent[i]), (10 - i));
        if (result) {
            printf("bcm_field_entry_prio_set *(ent[0]) failure\n");
            return result;
        }
    }

    /* IPV4 STATIC */
    i=2;
    result = bcm_field_qualify_AppType(unit, *(ent[i]), app_type[i]);
    if (BCM_E_NONE != result)
    {
        printf("Error ent[%d] in bcm_field_qualify_AppType returned %s\n", i, bcm_errmsg(result));
        auxRes = bcm_field_group_destroy(unit, grp->group);
        return result;
    }
    if (is_device_or_above(unit, JERICHO))
    {
        result = bcm_field_qualify_L3SrcRouteHit(unit, *(ent[i]), 0x0, 0x01);
        if (BCM_E_NONE != result)
        {
            printf("Error ent[%d] in bcm_field_qualify_L3SrcRouteHit returned %s\n", i, bcm_errmsg(result));
            auxRes = bcm_field_group_destroy(unit, grp->group);
            return result;
        }
    }
    else
    {
        result = bcm_field_qualify_L2DestHit(unit, *(ent[i]), 0x0, 0x01);
        if (BCM_E_NONE != result)
        {
            printf("Error ent[%d] in bcm_field_qualify_L2DestHit returned %s\n", i, bcm_errmsg(result));
            auxRes = bcm_field_group_destroy(unit, grp->group);
            return result;
        }
    }
    result = bcm_field_action_add(unit, *(ent[i]), bcmFieldActionDrop, 0, 0);
    if (BCM_E_NONE != result)
    {
        printf("Error ent[%d] in bcm_field_action_add returned %s\n", i, bcm_errmsg(result));
        auxRes = bcm_field_group_destroy(unit, grp->group);
        return result;
    }

    result = bcm_field_action_add(unit, *(ent[i]), bcmFieldActionDoNotLearn, 0, 0);
    if (BCM_E_NONE != result)
    {
        printf("Error, bcm_field_action_add ent[%d] - bcmFieldActionDoNotLearn result:%s\n", i, bcm_errmsg(result));
        auxRes = bcm_field_group_destroy(unit, grp->group);
        return result;
    }
    result = bcm_field_entry_prio_set(unit, *(ent[i]), (10-i));
    if (result) {
        printf("bcm_field_entry_prio_set *(ent[%d]) failure\n", i);
        return result;
    }

    /* IPV6 STATIC */
    i = 3;
    result = bcm_field_qualify_AppType(unit, *(ent[i]), app_type[i]);
    if (BCM_E_NONE != result)
    {
        printf("Error ent[%d] in bcm_field_qualify_AppType returned %s\n", i, bcm_errmsg(result));
        auxRes = bcm_field_group_destroy(unit, grp->group);
        return result;
    }
    if (is_device_or_above(unit, JERICHO))
    {
        result = bcm_field_qualify_L3SrcRouteHit(unit, *(ent[i]), 0x0, 0x01);
        if (BCM_E_NONE != result)
        {
            printf("Error ent[%d] in bcm_field_qualify_L3SrcRouteHit returned %s\n", i, bcm_errmsg(result));
            auxRes = bcm_field_group_destroy(unit, grp->group);
            return result;
        }
    }
    else
    {
        result = bcm_field_qualify_IpmcHit(unit, *(ent[i]), 0x0, 0x01);
        if (BCM_E_NONE != result)
        {
            printf("Error ent[%d] in bcm_field_qualify_IpmcHit returned %s\n", i, bcm_errmsg(result));
            auxRes = bcm_field_group_destroy(unit, grp->group);
            return result;
        }
    }    
    result = bcm_field_action_add(unit, *(ent[i]), bcmFieldActionDrop, 0, 0);
    if (BCM_E_NONE != result)
    {
        printf("Error ent[%d] in bcm_field_action_add returned %s\n", i, bcm_errmsg(result));
        auxRes = bcm_field_group_destroy(unit, grp->group);
        return result;
    }

    result = bcm_field_action_add(unit, *(ent[i]), bcmFieldActionDoNotLearn, 0, 0);
    if (BCM_E_NONE != result)
    {
        printf("Error, bcm_field_action_add ent[%d] - bcmFieldActionDoNotLearn result:%s\n", i, bcm_errmsg(result));
        auxRes = bcm_field_group_destroy(unit, grp->group);
        return result;
    }
    result = bcm_field_entry_prio_set(unit, *(ent[i]), (10-i));
    if (result) {
        printf("bcm_field_entry_prio_set *(ent[%d]) failure\n", i);
        return result;
    }

    /* install */
    result = bcm_field_group_install(unit, grp->group);
    if (BCM_E_NONE != result)
    {
        printf("Error in bcm_field_group_install returned %s\n", bcm_errmsg(result));
        auxRes = bcm_field_group_destroy(unit, grp->group);
        return result;
    }

    /*
     *  Everything went well.
     */
    return result;
}

/*********************************************************************************
* Delete the field group which is create by pon_anti_spoofing_drop().
*
* Steps
*     1. Delete  and destory entries
*     2. Destory field group
 */
int pon_anti_spoofing_drop_destory(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    int i = 0;

    for (i = 0; i < 4; i++)
    {
        rv = bcm_field_entry_remove(unit, pon_anti_spoofing_info.ent[i]);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: bcm_field_entry_remove %d returned %s\n", i, bcm_errmsg(rv));
            return rv;
        }

        rv = bcm_field_entry_destroy(unit, pon_anti_spoofing_info.ent[i]);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: bcm_field_entry_destroy %d returned %s\n", i, bcm_errmsg(rv));
            return rv;
        }
    }
    
    rv = bcm_field_group_destroy(unit, pon_anti_spoofing_info.grp.group);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: bcm_field_group_destroy returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/*********************************************************************************
* clean all configuration. include FG, anti-spoofing and pon service
*
* Steps
*     1. clean FG configuration
*     2. clean anti-spoofing configuration
*     3. clean pon service configuration
 */
int pon_anti_spoofing_app_clean(int unit)
{
    int rv = 0;

    rv = pon_anti_spoofing_drop_destory(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, pon_anti_spoofing_drop_destory returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    rv = pon_anti_spoofing_disable(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, pon_anti_spoofing_disable returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Delete pon gport */
    rv = vswitch_delete_port(unit, pon_anti_spoofing_info.vsi, pon_anti_spoofing_info.pon_gport);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, vswitch_delete_port returned %s remove pon gport failed\n", bcm_errmsg(rv));
        return rv;
    }

    /* Delete nni gport */
    rv = vswitch_delete_port(unit, pon_anti_spoofing_info.vsi, pon_anti_spoofing_info.nni_gport);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vswitch_delete_port returned %s remove nni gport failed\n", bcm_errmsg(rv));
        return rv;
    }

    rv = vswitch_delete(unit, pon_anti_spoofing_info.vsi);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vswitch_delete returned %s remove vsi failed\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

int pon_anti_spoofing_with_dscp_remarking_check_src_bind_result(int unit, bcm_gport_t expected_gport) {
    bcm_field_group_t group;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent;
    uint64 data;
    uint64 mask;
    int rv;

    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyL3SrcRouteValue);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    rv = bcm_field_group_create(unit, qset, BCM_FIELD_GROUP_PRIO_ANY, &group);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_field_group_create (%s)\n", bcm_errmsg(rv));
        return rv;
    }

    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);
    rv = bcm_field_group_action_set(unit, group, aset);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_field_group_action_set (%s)\n", bcm_errmsg(rv));
        return rv;
    }

    /* 1st entry (higher priority), if LPM lookup result is expected, do nothing */
    rv = bcm_field_entry_create(unit, group, &ent);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_field_entry_create (%s)\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_entry_prio_set(unit, ent, 20);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_field_entry_prio_set (%s)\n", bcm_errmsg(rv));
        return rv;
    }

    COMPILER_64_SET(data, 0, BCM_GPORT_SUB_TYPE_FORWARD_GROUP_GET(expected_gport));    
    COMPILER_64_SET(mask, 0, 0x7FFF); /* in ARAD+ LPM result has 15 bits */
    rv = bcm_field_qualify_L3SrcRouteValue(unit, ent, data, mask);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_field_qualify_L3SrcRouteValue (%s)\n", bcm_errmsg(rv));
        return rv;
    }

    pon_anti_spoofing_info.ent[0] = ent;
    
    /* 2nd entry (lower priority), otherwise drop */
    rv = bcm_field_entry_create(unit, group, &ent);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_field_entry_create (%s)\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_entry_prio_set(unit, ent, 10);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_field_entry_prio_set (%s)\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_field_action_add(unit, ent, bcmFieldActionDrop, 0, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_field_action_add (%s)\n", bcm_errmsg(rv));
        return rv;
    }

    pon_anti_spoofing_info.ent[1] = ent;

    rv = bcm_field_group_install(unit, group);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_field_group_install (%s)\n", bcm_errmsg(rv));
        return rv;
    }

    pon_anti_spoofing_info.grp.group = group;
    
    return rv;
}

int pon_anti_spoofing_with_dscp_remark_dvapi(int unit, int pon_port, int nni_port) {
    int rv = BCM_E_NONE;
    bcm_vlan_port_t vlan_port;
    bcm_gport_t pon_gport;
    bcm_vlan_port_t data_port;
    bcm_l3_source_bind_t srcIpBind;
    bcm_vlan_t vsi = 1;

    open_ingress_mc_group(unit, vsi);

    rv = bcm_multicast_ingress_add(unit, vsi, nni_port, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_multicast_ingress_add (%s)\n", bcm_errmsg(rv));
        return rv;
    }

    pon_anti_spoofing_info.vsi = vsi;
    pon_anti_spoofing_info.pon_port = pon_port;
    pon_anti_spoofing_info.nni_port = nni_port;

    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags = BCM_VLAN_PORT_FORWARD_GROUP;
    vlan_port.port = pon_port;
    vlan_port.match_tunnel_value = 100;
    vlan_port.egress_tunnel_value = 100;
    vlan_port.match_vlan = 10;
    vlan_port.vsi = vsi;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL_VLAN;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_create (%s)\n", bcm_errmsg(rv));
        return rv;
    }

    pon_anti_spoofing_info.pon_gport = pon_gport = vlan_port.vlan_port_id;

    rv = bcm_l3_source_bind_enable_set(unit, pon_gport, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_l3_source_bind_enable_set (%s)\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create DSCP remark data entry in EEDB */
    bcm_vlan_port_t_init(&data_port);    
    data_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    data_port.flags = BCM_VLAN_PORT_EXTENDED | BCM_VLAN_PORT_CREATE_EGRESS_ONLY; /* make sure to use these set of flags*/
    data_port.qos_map_id =  pon_gport;                               /* the AC this DATA is associated to */
    data_port.port = nni_port;
    rv = bcm_vlan_port_create(unit, &data_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_create (creating data entry)\n");
        return rv;
    }

    pon_dscp_remark_gport = data_port.vlan_port_id;

    /* update FEC to point to the new data entry */
    uint32 flags = BCM_L3_INGRESS_ONLY | BCM_L3_ENCAP_SPACE_OPTIMIZED | BCM_L3_WITH_ID | BCM_L3_REPLACE;
    bcm_l3_egress_t egr;
    bcm_if_t if_id;
    egr.encap_id = BCM_GPORT_SUB_TYPE_LIF_VAL_GET(data_port.vlan_port_id);
    egr.mpls_label = BCM_MPLS_LABEL_INVALID;
    egr.port = pon_port;
    if_id = BCM_GPORT_SUB_TYPE_FORWARD_GROUP_GET(pon_gport);
    rv = bcm_l3_egress_create(unit, flags, &egr, &if_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_l3_egress_create\n");
        return rv;
    }

    bcm_l3_source_bind_t_init(&srcIpBind);
    
    srcIpBind.ip      = 0xc8010101;
    srcIpBind.ip_mask = 0xFFFFFF00;
    srcIpBind.flags   = BCM_L3_SOURCE_BIND_USE_MASK;
    srcIpBind.port    = pon_gport;
    
    rv = bcm_l3_source_bind_add(unit, &srcIpBind);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_l3_source_bind_add\n");
        return rv;
    }

    rv = pon_anti_spoofing_with_dscp_remarking_check_src_bind_result(unit, pon_gport);

    return rv;
}

int pon_anti_spoofing_with_dscp_remark_dvapi_clearn_up(int unit) {
    bcm_field_entry_destroy(unit, pon_anti_spoofing_info.ent[0]);
    bcm_field_entry_destroy(unit, pon_anti_spoofing_info.ent[1]);
    bcm_field_group_destroy(unit, pon_anti_spoofing_info.grp.group);

    bcm_vlan_port_destroy(unit, pon_dscp_remark_gport);
    bcm_vlan_port_destroy(unit, pon_anti_spoofing_info.pon_gport);

    bcm_multicast_ingress_delete_all(unit, pon_anti_spoofing_info.vsi);

    return 0;
}

int dipv6_compression_result=0x2ff;
int sipv6_compression_result=0xfff;
int pon_anti_spoofing_with_ipv6_compression_result(int unit) {
    bcm_field_group_t group;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent[2];
    uint64 data;
    uint64 mask;
    int rv;
    int index = 0;

    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIpmcValue);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyL2SrcValue);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    rv = bcm_field_group_create(unit, qset, BCM_FIELD_GROUP_PRIO_ANY, &group);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_field_group_create (%s)\n", bcm_errmsg(rv));
        return rv;
    }

    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionDrop);
    rv = bcm_field_group_action_set(unit, group, aset);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_field_group_action_set (%s)\n", bcm_errmsg(rv));
        return rv;
    }

    /* 1st entry (higher priority), drop packets if dipv6 compression result is 0x2ff
     * or sipv6 compression result is 0xfff
     */
    for (index = 0; index < 2; index++) {
        rv = bcm_field_entry_create(unit, group, &ent[index]);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_field_entry_create (%s)\n", bcm_errmsg(rv));
            return rv;
        }

        rv = bcm_field_qualify_InPort(unit, ent[index], pon_anti_spoofing_info.nni_port, 0xffffffff);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_field_qualify_InPort (%s)\n", bcm_errmsg(rv));
            return rv;
        }

        if (index == 0) {
            COMPILER_64_SET(data, 0, dipv6_compression_result);    
            COMPILER_64_SET(mask, 0, 0xFFF); /* DIPv4 compression result takes 12 bits */
            rv = bcm_field_qualify_L2SrcValue(unit, ent[index], data, mask);
            if (rv != BCM_E_NONE) {
                printf("Error, in bcm_field_qualify_InPort (%s)\n", bcm_errmsg(rv));
                return rv;
            }
        }else {
            COMPILER_64_SET(data, 0, sipv6_compression_result);    
            COMPILER_64_SET(mask, 0, 0xFFF); /* DIPv4 compression result takes 12 bits */
            rv = bcm_field_qualify_IpmcValue(unit, ent[index], data, mask);
            if (rv != BCM_E_NONE) {
                printf("Error, in bcm_field_qualify_InPort (%s)\n", bcm_errmsg(rv));
                return rv;
            }

        }

        rv = bcm_field_action_add(unit, ent[index], bcmFieldActionDrop, 0, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_field_action_add (%s)\n", bcm_errmsg(rv));
            return rv;
        }
    }

    rv = bcm_field_group_install(unit, group);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_field_group_install (%s)\n", bcm_errmsg(rv));
        return rv;
    }

    pon_anti_spoofing_info.grp.group = group;
    
    return rv;
}

int pon_anti_spoofing_with_ipv6_compression_dvapi(int unit, int pon_port, int nni_port) {
    int rv = 0;
    int result=0;
    int tt_result=0;
    bcm_l3_source_bind_t nni_src_bind;

    rv = pon_anti_spoofing_app(unit, pon_port, nni_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in pon_anti_spoofing_app\n");
        return rv;
    }

    /* SIPv6 compression */ 
    bcm_l3_source_bind_t_init(&nni_src_bind);
    nni_src_bind.ip6[0] = 0x10;
    nni_src_bind.ip6[1] = 0x11;
    nni_src_bind.ip6[2] = 0x22;
    nni_src_bind.ip6[3] = 0x23;
    nni_src_bind.ip6[4] = 0x34;
    nni_src_bind.ip6[5] = 0x35;
    nni_src_bind.ip6[6] = 0x46;
    nni_src_bind.ip6[7] = 0x47;
    nni_src_bind.ip6[8] = 0x58;
    nni_src_bind.ip6[9] = 0x59;
    nni_src_bind.ip6[10] = 0x6a;
    nni_src_bind.ip6[11] = 0x6b;
    nni_src_bind.ip6[12] = 0x7c;
    nni_src_bind.ip6[13] = 0x7d;
    nni_src_bind.ip6[14] = 0x8e;
    nni_src_bind.ip6[15] = 0x8f;
    nni_src_bind.flags |= BCM_L3_SOURCE_BIND_IP6;
    nni_src_bind.port = pon_anti_spoofing_info.nni_gport;
    tt_result = 0;
    result = sipv6_compression_result;
    nni_src_bind.rate_id = (0x80000000 | ((tt_result) << 15) | (result));
    rv = bcm_l3_source_bind_add(0, &nni_src_bind);
    if (rv != BCM_E_NONE)
    {
        printf("Error, SIP compression, bcm_l3_source_bind_add returned %s nni_gport:%d\n", 
                bcm_errmsg(rv), pon_anti_spoofing_info.nni_gport);
        return rv;
    }

    /* DIPv6 compression */ 
    nni_src_bind.ip6[0] = 0x11;
    nni_src_bind.ip6[1] = 0x11;
    nni_src_bind.ip6[2] = 0x22;
    nni_src_bind.ip6[3] = 0x22;
    nni_src_bind.ip6[4] = 0x33;
    nni_src_bind.ip6[5] = 0x33;
    nni_src_bind.ip6[6] = 0x44;
    nni_src_bind.ip6[7] = 0x44;
    nni_src_bind.ip6[8] = 0x55;
    nni_src_bind.ip6[9] = 0x55;
    nni_src_bind.ip6[10] = 0x66;
    nni_src_bind.ip6[11] = 0x66;
    nni_src_bind.ip6[12] = 0x77;
    nni_src_bind.ip6[13] = 0x77;
    nni_src_bind.ip6[14] = 0x88;
    nni_src_bind.ip6[15] = 0x88;
    nni_src_bind.flags |= BCM_L3_SOURCE_BIND_IP6;
    nni_src_bind.port = pon_anti_spoofing_info.nni_gport;
    tt_result = 0xfff;
    result = dipv6_compression_result;
    nni_src_bind.rate_id = (0x80000000 | ((tt_result) << 15) | (result));
    rv = bcm_l3_source_bind_add(0, &nni_src_bind);
    if (rv != BCM_E_NONE)
    {
        printf("Error, DIP compression, bcm_l3_source_bind_add returned %s nni_gport:%d\n", 
                bcm_errmsg(rv), pon_anti_spoofing_info.nni_gport);
        return rv;
    }

    /* Use PMF to drop the packet SIPv6 compression result is 0xfff/DIPv6 compression result is 0x2ff */
    rv = pon_anti_spoofing_with_ipv6_compression_result(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, pon_anti_spoofing_with_ipv6_compression_result %s\n", 
                bcm_errmsg(rv));
        return rv;
    }

    return rv;

}

int pon_anti_spoofing_with_ipv6_compression_dvapi_clean_up(int unit) {

    int rv =0;

    rv = pon_anti_spoofing_app_clean(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, pon_anti_spoofing_app_clean returned %s\n", 
                bcm_errmsg(rv));
        return rv;
    }

    return rv;

}


