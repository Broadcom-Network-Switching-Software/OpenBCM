/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_pon_mac_management.c
 *
 * Purpose: An example of how to use BCM APIs to implement MAC management functions.
 *
 * To Activate Above Settings:
 *      BCM> cint examples/dpp/cint_port_tpid.c
 *      BCM> cint examples/dpp/pon/cint_pon_utils.c
 *      BCM> cint examples/dpp/pon/cint_pon_mac_management.c
 *      BCM> cint
 *      cint> 
 *      cint> print pon_l2_app(unit, pon, nni);
 *      cint> print l2_mact_run_1(unit);
 *      cint> print l2_mact_run_1(unit);
 *      cint> print l2_mact_run_2(unit);
 *      cint> print l2_mact_run_3(unit, is_both, delete_entries);
 *
 * To clean pon application configuration:
 *      cint> print pon_l2_app_clean(unit);
 *
 * run functions one by one in cint_pon_mac_management.c
 */
struct pon_l2_app_info_s {
    bcm_vlan_t  vsi;
    bcm_gport_t pon_gport;
    bcm_gport_t nni_gport;
    bcm_port_t pon_port;
    bcm_port_t nni_port;
    int tunnel_id;
    int nni_svlan;
    int pon_cvlan;
};

pon_l2_app_info_s pon_l2_app_info;
int counter = 0;

/* Initialize PON application configurations.
 * Example: pon_l2_app_init(0, 1, 128);
 */
void pon_l2_app_init(int unit, bcm_port_t pon_port, bcm_port_t nni_port)
{
    sal_memset(&pon_l2_app_info, 0, sizeof(pon_l2_app_info));
    pon_l2_app_info.vsi = 0;
    pon_l2_app_info.pon_port = pon_port;
    pon_l2_app_info.nni_port = nni_port;
    pon_l2_app_info.tunnel_id = 100;
    pon_l2_app_info.nni_svlan = 100;
    pon_l2_app_info.pon_cvlan = 10;

    /* Disable trunk in NNI port */
    pon_app_init(unit, pon_port, nni_port, 0, 0);
}

/*********************************************************************************
* Setup pon application model
*     PON Port 1 <------------------------------->  VSI  <---> NNI Port 128
*     Pon 1 Tunnel-ID 100 clvan 10 --------------------|---4096  |----------- SVLAN 100 cvlan 10
*                                                 
* steps
 */
int pon_l2_app(int unit, bcm_port_t pon, bcm_port_t nni)
{
    int i = 0;
    int rv = 0;
    bcm_if_t encap_id = 0;
    int action_id = 0;

    pon_l2_app_init(unit, pon, nni);

    rv = vswitch_create(unit, &(pon_l2_app_info.vsi));
    if (rv != BCM_E_NONE)
    {
        printf("Error, vswitch_create returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create PON LIF */
    rv = pon_lif_create(unit, 
                        pon_l2_app_info.pon_port, 
                        match_otag,
                        0, 0,
                        pon_l2_app_info.tunnel_id,
                        pon_l2_app_info.tunnel_id,
                        pon_l2_app_info.pon_cvlan,
                        0, 0, 0,
                        &(pon_l2_app_info.pon_gport));
    if (rv != BCM_E_NONE)
    {
        printf("Error, pon_lif_create returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Add PON LIF to vswitch */
    rv = bcm_vswitch_port_add(unit, 
                              pon_l2_app_info.vsi, 
                              pon_l2_app_info.pon_gport
                             );
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vswitch_port_add returned %s\n", bcm_errmsg(rv));
        return rv;
    }
    /*For downstream*/
    rv = multicast_vlan_port_add(unit, 
                                 pon_l2_app_info.vsi+lif_offset, 
                                 pon_l2_app_info.pon_port, 
                                 pon_l2_app_info.pon_gport);
    if (rv != BCM_E_NONE)
    {
        printf("Error, multicast_vlan_port_add returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Set PON LIF ingress VLAN editor */
    rv = pon_port_ingress_vt_set(unit,
                                 otag_to_o_i_tag,
                                 pon_l2_app_info.nni_svlan,
                                 pon_l2_app_info.pon_cvlan,
                                 pon_l2_app_info.pon_gport,
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
                                pon_l2_app_info.tunnel_id,
                                pon_l2_app_info.pon_cvlan,
                                0, 
                                pon_l2_app_info.pon_gport
                               );
    if (rv != BCM_E_NONE)
    {
        printf("Error, pon_port_egress_vt_set returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Create NNI LIF */
    rv = nni_lif_create(unit,
                       pon_l2_app_info.nni_port,
                       match_o_i_tag,
                       0,
                       pon_l2_app_info.nni_svlan,
                       pon_l2_app_info.pon_cvlan,
                       0,
                       &(pon_l2_app_info.nni_gport),
                       &encap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, nni_lif_create returned %s\n", bcm_errmsg(rv));
        return rv;
    }

    /*For upstream*/
    rv = multicast_vlan_port_add(unit, 
                              pon_l2_app_info.vsi,
                              pon_l2_app_info.nni_port,
                              pon_l2_app_info.nni_gport);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, multicast_vlan_port_add returned %s nni_gport\n", bcm_errmsg(rv));
        return rv;
    }

    /* add to vswitch */
    rv = bcm_vswitch_port_add(unit,
                           pon_l2_app_info.vsi,
                           pon_l2_app_info.nni_gport);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vswitch_port_add  returned %s nni_gport\n", bcm_errmsg(rv));
        return rv;
    }
    
    /* Set the multicast group offset to flood downstream packets in multicast group(vsi+lif_offset) */
    bcm_port_control_set(unit, pon_l2_app_info.nni_gport, bcmPortControlFloodUnknownUcastGroup, lif_offset);
    bcm_port_control_set(unit, pon_l2_app_info.nni_gport, bcmPortControlFloodUnknownMcastGroup, lif_offset);
    bcm_port_control_set(unit, pon_l2_app_info.nni_gport, bcmPortControlFloodBroadcastGroup, lif_offset);

    return rv;
}

/*********************************************************************************
* clean all configuration.
*
* Steps
*     1. clean pon service configuration
 */
int pon_l2_app_clean(int unit)
{
    int rv = 0;

    /* Delete pon gport */
    rv = vswitch_delete_port(unit, pon_l2_app_info.vsi, pon_l2_app_info.pon_gport);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, vswitch_delete_port returned %s, remove pon gport failed\n", bcm_errmsg(rv));
        return rv;
    }

    /* Delete nni gport */
    rv = vswitch_delete_port(unit, pon_l2_app_info.vsi, pon_l2_app_info.nni_gport);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vswitch_delete_port returned %s, remove nni gport failed\n", bcm_errmsg(rv));
        return rv;
    }

    rv = vswitch_delete(unit, pon_l2_app_info.vsi);
    if (rv != BCM_E_NONE)
    {
        printf("Error, vswitch_delete returned %s, remove vsi failed\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/*********************************************************************************
* Add a l2 entry to MACT table.
*/
int l2_entry_add(int unit, bcm_mac_t mac, bcm_vlan_t vlan, int dest_type, int dest_gport, int dest_mc_id){
    int rv = BCM_E_NONE;
    bcm_l2_addr_t l2_addr;

    bcm_l2_addr_t_init(&l2_addr, mac, vlan);   
    if(dest_type == 0)
    {
        /* add static entry */
        l2_addr.flags = BCM_L2_STATIC;
        l2_addr.port = dest_gport;
    } 
    else
    {
        /* add multicast entry */
        l2_addr.flags = BCM_L2_STATIC | BCM_L2_MCAST;
        l2_addr.l2mc_group = dest_mc_id;
    }
    rv = bcm_l2_addr_add(unit,&l2_addr);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, bcm_l2_addr_add returned %s, dest_type %d dest_id %d \n", bcm_errmsg(rv), dest_type, dest_gport );
        return rv;
    }
    return rv;
}

/* Application Sequence:
 *  -   Add L2 entry to MACT table
 *  -   Get L2 entry by vsi and MAC addreess
 *  -   Delete L2 entry from MACT table
 *  -   Enable aging timer and set age time to 60 seconds
 *  -   Get aging time
 *  -   Disable aging timer
 */
int l2_mact_run_1(int unit)
{
    int rv = BCM_E_NONE;
    bcm_mac_t static_mac_addr_1 = {0,0,0,0,2,1};
    bcm_mac_t mac_addr_zero = {0,0,0,0,0,0};
    int aging_time = 60;
    bcm_l2_addr_t l2addr;

    /* Add L2 entry to MACT table */
    rv = l2_entry_add(unit, static_mac_addr_1, pon_l2_app_info.vsi, 0, pon_l2_app_info.pon_gport, 0);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, l2_entry_add returned %s failed, static_mac_addr_1\n", bcm_errmsg(rv));
        return rv;
    }

    /* Get L2 entry by vsi and MAC addreess */
    bcm_l2_addr_t_init(&l2addr, mac_addr_zero, 0);
    rv = bcm_l2_addr_get(unit, static_mac_addr_1, pon_l2_app_info.vsi, &l2addr);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, bcm_l2_addr_get returned %s failed, static_mac_addr_1\n", bcm_errmsg(rv));
        return rv;
    }

    print l2addr;

    /* Delete L2 entry from MACT table */
    rv = bcm_l2_addr_delete(unit, static_mac_addr_1, pon_l2_app_info.vsi);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, bcm_l2_addr_delete returned %s failed, static_mac_addr_1\n", bcm_errmsg(rv));
        return rv;
    }

    /* Enable aging timer */
    rv = bcm_l2_age_timer_set(unit, aging_time);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, bcm_l2_age_timer_set returned %s failed, enable aging timer\n", bcm_errmsg(rv));
        return rv;
    }

    /* Get aging timer */
    aging_time = 0;
    rv = bcm_l2_age_timer_get(unit, &aging_time);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, bcm_l2_age_timer_get returned %s failed, get aging timer\n", bcm_errmsg(rv));
        return rv;
    }

    print aging_time;

    /* Disable aging timer */
    rv = bcm_l2_age_timer_set(unit, 0);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, bcm_l2_age_timer_set returned %s failed, disable aging timer\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/* Application Sequence:
 *  -   Add some static L2 entries
 */
int l2_mact_run_2(int unit)
{
    int rv = BCM_E_NONE;
    bcm_mac_t static_mac_addr_1 = {0,0,0,0,2,1};
    bcm_mac_t static_mac_addr_2 = {0,0,0,0,2,2};
    bcm_mac_t static_mac_addr_3 = {0,0,0,0,2,3};
    bcm_mac_t mac_addr_zero = {0,0,0,0,0,0};
    bcm_l2_addr_t l2addr;

    /* Add L2 entry to MACT table */
    rv = l2_entry_add(unit, static_mac_addr_1, pon_l2_app_info.vsi, 0, pon_l2_app_info.pon_gport, 0);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, l2_entry_add returned %s failed, static_mac_addr_1\n", bcm_errmsg(rv));
        return rv;
    }

    rv = l2_entry_add(unit, static_mac_addr_2, pon_l2_app_info.vsi, 0, pon_l2_app_info.pon_gport, 0);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, l2_entry_add returned %s failed, static_mac_addr_2\n", bcm_errmsg(rv));
        return rv;
    }

    rv = l2_entry_add(unit, static_mac_addr_3, pon_l2_app_info.vsi, 0, pon_l2_app_info.pon_gport, 0);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, l2_entry_add returned %s failed, static_mac_addr_3\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/* MAC table traverse and counter for all entries */
int mac_traverse_callback(int uint, bcm_l2_addr_t* l2addr, void* user_data)
{
	counter++;
	return BCM_E_NONE;
}


/* Application Sequence:
 *  -   Send traffic to learn SAs
 *  -   Get L2 entry counter
 *  -   Get dynamic L2 entry counter in port
 *  -   Get static L2 entry counter in port
 *  -   Get dynamic+static L2 entry counter in port
 *  -   Delete dynamic L2 entries in port
 *  -   Delete static L2 entries in port
 *  -   Delete dynamic+static L2 entries
 */
int l2_mact_run_3(int unit, int is_both, int delete_entries)
{
    int rv = BCM_E_NONE;
    bcm_mac_t mac_addr_zero = {0,0,0,0,0,0};
    bcm_l2_addr_t match_l2addr;
    uint32 flags = 0;

    /* Get L2 entry counter */
    counter = 0;
    rv = bcm_l2_traverse(unit, mac_traverse_callback, NULL);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, bcm_l2_traverse returned %s failed\n", bcm_errmsg(rv));
        return rv;
    }
    printf("L2 entries:%d\n", counter);

    if (is_both)
    {
        /* Get dynamic+static L2 entry counter in port */
        counter = 0;
        bcm_l2_addr_t_init(&match_l2addr, mac_addr_zero, 0);
        match_l2addr.port = pon_l2_app_info.pon_gport;
        flags = BCM_L2_TRAVERSE_MATCH_DEST | BCM_L2_TRAVERSE_MATCH_STATIC \
                | BCM_L2_TRAVERSE_IGNORE_DISCARD_SRC | BCM_L2_TRAVERSE_IGNORE_DES_HIT;
        rv = bcm_l2_matched_traverse(unit, flags, &match_l2addr, mac_traverse_callback, NULL);
        if (rv != BCM_E_NONE) 
        {
            printf("Error, bcm_l2_matched_traverse returned %s failed, dynamic+static l2 entries\n", bcm_errmsg(rv));
            return rv;
        }
        printf("dynamic+static L2 entries in %d:%d\n", pon_l2_app_info.pon_gport, counter);

        if (delete_entries)
        {
            /* Delete dynamic+static L2 entries */
            rv = bcm_l2_addr_delete_by_port(unit, 0, pon_l2_app_info.pon_gport, BCM_L2_DELETE_STATIC);
            if (rv != BCM_E_NONE)
            {
                printf("Error, bcm_l2_addr_delete_by_port returned %s failed, dynamic+static l2 entries\n", bcm_errmsg(rv));
                return rv;
            }
        }
    }
    else
    {
        /* Get dynamic L2 entry counter in port */
        counter = 0;
        bcm_l2_addr_t_init(&match_l2addr, mac_addr_zero, 0);
        match_l2addr.port = pon_l2_app_info.pon_gport;
        flags = BCM_L2_TRAVERSE_MATCH_DEST | BCM_L2_TRAVERSE_IGNORE_DISCARD_SRC | BCM_L2_TRAVERSE_IGNORE_DES_HIT;
        rv = bcm_l2_matched_traverse(unit, flags, &match_l2addr, mac_traverse_callback, NULL);
        if (rv != BCM_E_NONE) 
        {
            printf("Error, bcm_l2_matched_traverse returned %s failed, dynamic l2 entries\n", bcm_errmsg(rv));
            return rv;
        }
        printf("dynamic L2 entries in %d:%d\n", pon_l2_app_info.pon_gport, counter);

        /* Get static L2 entry counter in port */
        counter = 0;
        bcm_l2_addr_t_init(&match_l2addr, mac_addr_zero, 0);
        match_l2addr.port = pon_l2_app_info.pon_gport;
        match_l2addr.flags = BCM_L2_STATIC;
        flags = BCM_L2_TRAVERSE_MATCH_DEST;
        rv = bcm_l2_matched_traverse(unit, flags, &match_l2addr, mac_traverse_callback, NULL);
        if (rv != BCM_E_NONE) 
        {
            printf("Error, bcm_l2_matched_traverse returned %s failed, static l2 entries\n", bcm_errmsg(rv));
            return rv;
        }
        printf("static L2 entries in %d:%d\n", pon_l2_app_info.pon_gport, counter);

        if (delete_entries)
        {
            /* Delete dynamic L2 entries in port */
            rv = bcm_l2_addr_delete_by_port(unit, 0, pon_l2_app_info.pon_gport, 0);
            if (rv != BCM_E_NONE) 
            {
                printf("Error, bcm_l2_addr_delete_by_port returned %s failed, dynamic l2 entries\n", bcm_errmsg(rv));
                return rv;
            }

            /* Delete static L2 entries in port */
            bcm_l2_addr_t_init(&match_l2addr, mac_addr_zero, 0);
            match_l2addr.port = pon_l2_app_info.pon_gport;
            match_l2addr.flags = BCM_L2_STATIC;
            rv = bcm_l2_replace(unit, BCM_L2_REPLACE_DELETE, &match_l2addr, 0, 0, 0);
            if (rv != BCM_E_NONE) 
            {
                printf("Error, bcm_l2_replace returned %s failed, static l2 entries\n", bcm_errmsg(rv));
                return rv;
            }
        }
    }

    return rv;
}

