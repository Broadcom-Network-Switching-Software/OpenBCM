/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 * File: cint_trunk.c
 * Purpose: The cint creates a Trunk containing several ports
 *
 *   default trunk example
 *  ----------------------------
 *   run:
 *   cint ../sand/utility/cint_sand_utils_global.c
 *   cint cint_trunk.c
 *   c
 *   run :
 *   print main_trunk(unit, is_hash_mode, double_lbk_rng_enable, port_0, port_1, port_2, port_3, in_local_port, trunk_id);
 *
 *      is_hashing_mode - use hashing lb-key or round-robin mode.
 *      double_lbk_rng_enable - supported only in ARAD+.
 *                              if set needs to config soc property: system_ftmh_load_balancing_ext_mod = STANDBY_MC_LB. 
 *
 * Notes: trunk_create - routine shows example how to create lag 
 */


int trunk_create(int unit, bcm_trunk_t tid, int in_local_port, int out_local_port_num, int *out_local_ports, int is_hash_mode, int double_lbk_rng_enable)
{
    int                 rv= BCM_E_NONE;
    int                 i;
    bcm_trunk_member_t  member_array[20];
    bcm_trunk_info_t    trunk_info;
    int tgport;
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t value;

    sal_memset(member_array, 0, sizeof(member_array));
    sal_memset(&trunk_info, 0, sizeof(trunk_info));

    /* On JR2, need to set trunk id before creating */
    if (is_device_or_above(unit, JERICHO2)){
        BCM_TRUNK_ID_SET(tid, 0, tid);
    }

    /* Creates #tid trunk (no memory allocation and no members inside) */
    rv =  bcm_trunk_create(unit, BCM_TRUNK_FLAG_WITH_ID, &tid);
    if(rv != BCM_E_NONE)
    {
        printf("Error: bcm_trunk_create(), rv=%d \n", rv);
        return rv;
    }
    printf("trunk id create: tid=%d\n", tid);
    BCM_GPORT_TRUNK_SET(tgport, tid);
    printf("trunk gport create: trunk_gport=%d\n", tgport);

    if (is_device_or_above(unit, JERICHO2)){
        key.index=0;
        key.type=bcmSwitchPortHeaderType;
        value.value=BCM_SWITCH_PORT_HEADER_TYPE_ETH;
        rv = bcm_switch_control_indexed_port_set(unit, tgport, key, value);
        if(rv != BCM_E_NONE)
        {
             printf("Error: bcm_switch_control_indexed_port_set() %d, tgport\n");
        }

        for (i=0; i < out_local_port_num; i++) {
            rv = bcm_switch_control_indexed_port_set(unit, out_local_ports[i], key, value);
            if(rv != BCM_E_NONE)
            {
                printf("Error: bcm_switch_control_indexed_port_set() %d, out_local_ports[i]\n");
            }
        }
    }

    /* Prepare local ports to gports */
    for (i=0; i<out_local_port_num; i++)
    {
       BCM_GPORT_SYSTEM_PORT_ID_SET(member_array[i].gport, out_local_ports[i]);
    }

    /* two options are valid BCM_TRUNK_PSC_PORTFLOW or BCM_TRUNK_PSC_ROUND_ROBIN */
    if(is_hash_mode)
    {
        trunk_info.psc= BCM_TRUNK_PSC_PORTFLOW;

        /* Set numbers of headers to take under consideration when creating the LB key for hashing */
        /* bcmSwitchTrunkHashPktHeaderCount isn't supported on JR2 yet */
        if (!is_device_or_above(unit, JERICHO2)){
            rv = bcm_switch_control_port_set(unit, in_local_port, bcmSwitchTrunkHashPktHeaderCount, 0x2);
            if (rv != BCM_E_NONE)
            {
                printf("Error, bcm_switch_control_port_set(), rv=%d.\n", rv);
                return rv;
            }
        }
    }
    else
    {
        if (!is_device_or_above(unit, JERICHO2)){
            trunk_info.psc= BCM_TRUNK_PSC_ROUND_ROBIN;
        } else {
            trunk_info.psc= BCM_TRUNK_PSC_SMOOTH_DIVISION;
        }
    }


    /* Adds members (in member_array) to trunk and activates trunk mechanism */
    rv = bcm_trunk_set(unit, tid, &trunk_info, out_local_port_num, member_array);
    if(rv != BCM_E_NONE)
    {
        printf("Error: bcm_trunk_set(), rv=%d\n", rv);
        return rv;
    }
    printf("trunk set\n");

    /* Check if double lb key range is enabled (relevant for gracefull load balancing) */
    if (double_lbk_rng_enable)
    {
        if(!is_device_or_above(unit,ARAD_PLUS))
        {
            printf("set double_lbk_rng_enable is supported only in ARAD+ / Jericho");
            return BCM_E_PARAM;
        }

        rv = bcm_switch_control_set(unit, bcmSwitchMcastTrunkIngressCommit, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_switch_control_set(), rv=%d.\n", rv);
            return rv;
        }

        rv = bcm_switch_control_set(unit, bcmSwitchMcastTrunkEgressCommit, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_switch_control_set(), rv=%d.\n", rv);
            return rv;
        }
    }

    return rv;

}

int trunk_destroy(int unit, bcm_trunk_t tid, int double_lbk_rng_enable)
{
    int rv;

    rv = bcm_trunk_destroy(unit, tid);
    if(rv != BCM_E_NONE)
    {
        printf("Error: bcm_trunk_destroy(), rv=%d\n", rv);
        return rv;
    }

    if (double_lbk_rng_enable)
    {
        if(!is_device_or_above(unit,ARAD_PLUS))
        {
            printf("set double_lbk_rng_enable is supported only in ARAD+ / Jericho");
            return BCM_E_PARAM;
        }

        rv = bcm_switch_control_set(unit, bcmSwitchMcastTrunkIngressCommit, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_switch_control_set(), rv=%d.\n", rv);
            return rv;
        }

        rv = bcm_switch_control_set(unit, bcmSwitchMcastTrunkEgressCommit, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_switch_control_set(), rv=%d.\n", rv);
            return rv;
        }
    }

    return rv;
}

int l2_addr_destroy(int unit, int is_mc)
{
    bcm_mac_t
    da  = {0x01, 0x02, 0x03, 0x04, 0x05, 0x00};

    if(is_mc) {
        da[4] = 7;
    }

    bcm_l2_addr_delete(unit, da, 1);

    return 0;
}

/*
 * Example how to add port to an existing trunk tid
 */
int existing_trunk_member_add(int unit, bcm_trunk_t tid, int out_local_port, int double_lbk_rng_enable)
{
    int rv;
    bcm_trunk_member_t member_array;

    BCM_GPORT_SYSTEM_PORT_ID_SET(member_array.gport, out_local_port);
    /*
     *  bcm_trunk_member_add adds member_array to an existing tid.
     *  - multiple instances of a same port is allowed (to change ballance load)
     *  - same port cannot be member of different tid's
     */

    rv =  bcm_trunk_member_add(unit,tid,&member_array);
    if(rv != BCM_E_NONE)
    {
        printf("Error: bcm_trunk_member_add(), rv=%d\n", rv);
        return rv;
    }

    if(double_lbk_rng_enable)
    {
        if(!is_device_or_above(unit,ARAD_PLUS))
        {
            printf("set double_lbk_rng_enable is supported only in ARAD+ / Jericho");
            return BCM_E_PARAM;
        }

        rv = bcm_switch_control_set(unit, bcmSwitchMcastTrunkIngressCommit, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_switch_control_set(), rv=%d.\n", rv);
            return rv;
        }

        rv = bcm_switch_control_set(unit, bcmSwitchMcastTrunkEgressCommit, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_switch_control_set(), rv=%d.\n", rv);
            return rv;
        }
    }

    return rv;
}


/*
 * Example how to delete port to an existing trunk tid
 */
int existing_trunk_member_delete(int unit, bcm_trunk_t tid, int out_local_port, int double_lbk_rng_enable)
{
    int rv;
    bcm_trunk_member_t member_array;
    
    BCM_GPORT_SYSTEM_PORT_ID_SET(member_array.gport, out_local_port);
    /*
     *  bcm_trunk_member_add adds member_array to an existing tid.
     *  - if multiple instances of a same port is in trunk, just one instance of it will be deleted
     */
    rv = bcm_trunk_member_delete(unit,tid,&member_array);
    if(rv != BCM_E_NONE) {
        printf("Error: bcm_trunk_member_delete(), rv=%d\n", rv);
        return rv; 
    }
        
    if(double_lbk_rng_enable) {
        
        if(!is_device_or_above(unit,ARAD_PLUS)) {
            printf("set double_lbk_rng_enable is supported only in ARAD+ / Jericho");
            return BCM_E_PARAM;
        }

        rv = bcm_switch_control_set(unit, bcmSwitchMcastTrunkIngressCommit, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_switch_control_set(), rv=%d.\n", rv);
            return rv;
        }

        rv = bcm_switch_control_set(unit, bcmSwitchMcastTrunkEgressCommit, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_switch_control_set(), rv=%d.\n", rv);
            return rv;
        }    
    }
    
    return rv;
}


int l2_addr_config(int unit, bcm_trunk_t tid, int is_mc, int is_egr, int port_0, int port_1, int port_2, int port_3)
{
    int             rv = BCM_E_NONE;
    int             vlan = 0x1;
    int             mc_id;
    int             flags;
    int             i;
    int             out_local_ports[4];
    int             out_local_port_num = 4;
    bcm_gport_t     gport;
    bcm_gport_t     mc_gport_member;
    bcm_gport_t     current_trunk_member_gport;
    bcm_mac_t       da  = {0x01, 0x02, 0x03, 0x04, 0x05, 0x00};
    bcm_l2_addr_t   l2addr;
    bcm_multicast_replication_t rep_array;

    out_local_ports[0] = port_0;
    out_local_ports[1] = port_1;
    out_local_ports[2] = port_2;
    out_local_ports[3] = port_3;

    BCM_GPORT_TRUNK_SET(gport, tid);

    if (is_mc)
    {
        /* Select a MC group for either Ingress or Egress MC*/
        mc_id = is_egr ? 0x3007 : 0x7 ;

        /* Choose mac address for each MC ID */
        da[4] = mc_id % 256;

        /*  Open ingress multycast group, select needed flags */
        flags = BCM_MULTICAST_WITH_ID;
        flags |= is_egr ? BCM_MULTICAST_EGRESS_GROUP : BCM_MULTICAST_INGRESS_GROUP;

        rv = bcm_multicast_create(unit, flags, &mc_id);
        if (rv != BCM_E_NONE)
        {
             printf("Error, in _multicast_create, mc_id mc_id=%d\n", mc_id);
             return rv;
        }

        /* Add MC members */
        BCM_GPORT_TRUNK_SET(mc_gport_member, tid);
        if(is_egr)
        {
            if (!is_device_or_above(unit, JERICHO2)){
                /* Add Trunk Gport to MC group */
                rv = bcm_multicast_egress_add(unit, mc_id, mc_gport_member, 0);
                if (rv != BCM_E_NONE)
                {
                    printf("Error, in bcm_multicast_egress_add, mc_id mc_id=%d, destination gport=0x%x\n", mc_id, mc_gport_member);
                    return rv;
                }
            } else {
                /* Add Trunk Gport to MC group */
                bcm_multicast_replication_t_init(&rep_array);
                rep_array.port = mc_gport_member;
                rv = bcm_multicast_add(unit, mc_id, BCM_MULTICAST_EGRESS_GROUP, 1, &rep_array);
                if (rv != BCM_E_NONE)
                {
                    printf("Error, in bcm_multicast_add with egress, mc_id mc_id=%d, destination gport=0x%x\n", mc_id, mc_gport_member);
                    return rv;
                }
            }
        }
        else
        {
            if (is_device_or_above(unit, ARAD_PLUS))
            {
                if (!is_device_or_above(unit, JERICHO2)){
                    /* Add Trunk Gport to MC group */
                    rv = bcm_multicast_ingress_add(unit, mc_id, mc_gport_member, 0);
                    if (rv != BCM_E_NONE)
                    {
                        printf("Error, in bcm_multicast_ingress_add, mc_id mc_id=%d, destination gport=0x%x\n", mc_id, mc_gport_member);
                        return rv;
                    }
                } else {
                    /* Add Trunk Gport to MC group */
                    bcm_multicast_replication_t_init(&rep_array);
                    rep_array.port = mc_gport_member;
                    rv = bcm_multicast_add(unit, mc_id, BCM_MULTICAST_INGRESS_GROUP, 1, &rep_array);
                    if (rv != BCM_E_NONE)
                    {
                        printf("Error, in bcm_multicast_add with ingress, mc_id mc_id=%d, destination gport=0x%x\n", mc_id, mc_gport_member);
                        return rv;
                    }
                }
            }
            else
            {
                /* Add Trunk members Gport to MC group, needed only in Arad, details in TM User manual */
                for (i = 0; i < out_local_port_num; ++i)
                {
                    BCM_GPORT_LOCAL_SET(current_trunk_member_gport, out_local_ports[i]);
                    rv = bcm_multicast_ingress_add(unit, mc_id, current_trunk_member_gport, 0);
                    if (rv != BCM_E_NONE) {
                        printf("Error, in bcm_multicast_ingress_add, mc_id mc_id=%d, destination gport=0x%x\n", mc_id, current_trunk_member_gport);
                        return rv;
                    }
                }
            }
        }
    }

    /* Initiate l2 mac  */
    bcm_l2_addr_t_init(&l2addr, da, vlan);
    printf("l2 init, da = {0x%x 0x%x 0x%x 0x%x 0x%x 0x%x}\n", da[0], da[1], da[2], da[3], da[4], da[5]);

    /* Set L2 address params */
    l2addr.flags = BCM_L2_STATIC; 
    if(is_mc) 
    {
        l2addr.flags =  l2addr.flags | BCM_L2_MCAST;
        l2addr.l2mc_group = mc_id;
    } 
    else 
    {
        l2addr.port = gport;
        l2addr.vid = vlan;
        l2addr.tgid = tid;  
    }
    
    rv = bcm_l2_addr_add(unit, &l2addr);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, bcm_l2_addr_add(), rv=%d.\n", rv);
        return rv;
    }

    printf("Created lb2: tid=%d is_mc=%d\n", tid, is_mc);

    return rv;
}

int main_trunk(int unit, int is_hash_mode, int double_lbk_rng_enable, int port_0, int port_1, int port_2, int port_3, int in_local_port, bcm_trunk_t tid)
{
    int rv = BCM_E_NONE;
    int out_local_ports[20];
    int out_local_port_num = 4;

    out_local_ports[0] = port_0;
    out_local_ports[1] = port_1;
    out_local_ports[2] = port_2;
    out_local_ports[3] = port_3;

    printf("Trunk #%d create with %d ports\n", tid, out_local_port_num);
    rv = trunk_create(unit, tid, in_local_port, out_local_port_num, out_local_ports, is_hash_mode, double_lbk_rng_enable);
    if (rv != BCM_E_NONE) 
    {
        printf("Error, trunk_create(), rv=%d.\n", rv);
        return rv;
    }

    if (is_device_or_above(unit, JERICHO2)){
        BCM_TRUNK_ID_SET(tid, 0, tid);
    }

    /* configure lb2 for uc */
    rv = l2_addr_config(unit, tid, 0x0, 0, port_0, port_1, port_2, port_3); 
    if(rv != BCM_E_NONE)
    {
         printf("Error: lb2_config() uc\n");
    }

    /* configure lb2 for mc group (whene lag is one of the group) */
    rv = l2_addr_config(unit, tid, 0x1, 0, port_0, port_1, port_2, port_3);
    if(rv != BCM_E_NONE)
    {
         printf("Error: lb2_config() mc\n");
    }

    /* Example to add port to an existing tid */
    /* printf("Trunk #%d add member local port 14\n", tid);
       existing_trunk_member_add(unit, tid, 17); */

    /* Example to delete port to an existing tid */
    /* printf("Trunk #%d delete member local port 14\n", tid);
       existing_trunk_member_delete(unit, tid, 17); */
    
    /* Example to delete all ports from an existing tid, but remain tid */
    /* existing_trunk_member_delete_all(unit, tid); */

    /* Example to delete all ports from an existing tid and remove tid */
    /* trunk_destroy(unit, tid); */

    return 0;
}

/*
 * This function is used to create a trunk with three ports (out_port_0, out_port_1, out_port_2) 
 * It is used for testing the trunk functionality without passing an array as parameter. 
 * This is useful in Cints' Dvapis scripts 
 */
int trunk_create_example_1 (int unit, int tid_num, int in_port, int out_port_0, int out_port_1, int out_port_2)
{
    bcm_trunk_t tid = tid_num;
    int ports[] = {out_port_0, out_port_1, out_port_2};
    trunk_create(unit, tid, in_port, 3, &ports, 0, 0);
}

