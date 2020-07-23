/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 *
 * Cint QOS Setup example code
 *
 * copy to /usr/local/sbin location, run bcm.user
 * Run script:
 *
   cd ../../../src/examples/dpp
   cint cint_qos_l3.c
   cint
 */
 
   /* Map ingress and egress as follows
    * Ingress DSCP     INTERNAL TC/DP     Egress DSCP
    *     IPV4
    *     0-3              0/GREEN         1
    *     4-7              0/YELLOW        2
    *     8-11             1/GREEN         3
    *     12-15            1/YELLOW        4
    *     16-19            2/GREEN         5
    *     20-23            2/YELLOW        6
    *     24-27            3/GREEN         7
    *     28-31            3/RED           8
    *     32-63            unmapped 0/GREEN
    * Ingress DSCP     INTERNAL TC/DP     Egress DSCP
    *     IPV6
    *     32-35           4/GREEN           9
    *     36-39           4/RED            10
    *     40-43           5/GREEN          11
    *     44-47           5/RED            12
    *     48-51           6/GREEN          13
    *     52-55           6/RED            14
    *     56-59           7/GREEN          15
    *     60-63           7/RED            16
    */
int qos_map_l3_internal_pri[16] = {1, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7};

bcm_color_t qos_map_l3_internal_color[16] = {bcmColorGreen,
					     bcmColorYellow,
					     bcmColorGreen,
					     bcmColorYellow,
					     bcmColorGreen,
					     bcmColorYellow,
					     bcmColorGreen,
					     bcmColorRed,
					     bcmColorGreen,
					     bcmColorRed,
					     bcmColorGreen,
					     bcmColorRed,
					     bcmColorGreen,
					     bcmColorRed,
					     bcmColorGreen,
					     bcmColorRed};

int qos_map_l3_remark_pri[16] = {12, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64};

int map_ids[20];
static int map_id_cnt = 0;

static int l3_in_map_id = 7;
static int l3_eg_map_id = 0;

static int l3_in_opcode_id = 25;
static int l3_eg_opcode_id = 27;

int in_lif_profile_dscp_preserve_value = 2;

/** Used for both IPv4 and IPv6. To avoid create the same qual one more time, we define it here.*/
bcm_field_qualify_t qual_id_fai_2_0 = bcmFieldQualifyCount;
bcm_field_qualify_t qual_id_const_1_bit = bcmFieldQualifyCount;

bcm_field_presel_t  fai_update_presel_id = 58;



int qos_l3_map_to_l2 = 0;
int
qos_map_id_l3_egress_get(int unit)
{
    return l3_eg_map_id;
}

int
qos_map_id_l3_ingress_get(int unit)
{
    return l3_in_map_id;
}

int
qos_map_l3_ingress_remark_profile(int unit)
{
    bcm_qos_map_t l3_in_map;
    int flags = 0;
    int dscp;
    int rv = BCM_E_NONE;

    /* Clear structure */

    bcm_qos_map_t_init(&l3_in_map);
    
    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_WITH_ID, &l3_in_map_id);
    if (rv != BCM_E_NONE) {
        printf("error in ingress l3 bcm_qos_map_create()\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE | BCM_QOS_MAP_WITH_ID, &l3_in_opcode_id);
    if (rv != BCM_E_NONE) {
        printf("error in ingress l3 opcode bcm_qos_map_create()\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

    map_ids[map_id_cnt++] = l3_in_map_id;

    l3_in_map.opcode = l3_in_opcode_id;

    /* 
     * Call map add with BCM_QOS_MAP_OPCODE flag in order to register the internal map ID,
     * this is done once for each map ID and protocol 
     */ 
    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 |  BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    rv = bcm_qos_map_add(unit, flags, &l3_in_map, l3_in_map_id);
      	
    if (rv != BCM_E_NONE) {
        printf("error in L3 ingress bcm_qos_map_add()\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

    /* 
     * Call map add with BCM_QOS_MAP_OPCODE flag in order to register the internal map ID,
     * this is done once for each map ID and protocol 
     */ 
    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV6 |  BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    rv = bcm_qos_map_add(unit, flags, &l3_in_map, l3_in_map_id);

    if (rv != BCM_E_NONE) {
        printf("error in L3 ingress bcm_qos_map_add()\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }
    
    for (dscp=0; dscp<64; dscp++) {
        bcm_qos_map_t_init(&l3_in_map);
      	if (dscp < 32) {
      	    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 |  BCM_QOS_MAP_REMARK ;
      	} else {
      	    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV6 | BCM_QOS_MAP_REMARK;
      	}

        /* Set ingress dscp Priority */
      	l3_in_map.dscp = dscp << 2;
      	
      	/* Set internal priority for this ingress DSCP  */
      	l3_in_map.int_pri = qos_map_l3_internal_pri[dscp>>2];
        l3_in_map.remark_int_pri = qos_map_l3_internal_pri[dscp>>2];
      	
      	/* Set color for this ingress DSCP Priority  */
      	l3_in_map.color = qos_map_l3_internal_color[dscp>>2];
      	
      	rv = bcm_qos_map_add(unit, flags, &l3_in_map, l3_in_opcode_id);
      	
      	if (rv != BCM_E_NONE) {
      	    printf("error in L3 ingress bcm_qos_map_add()\n");
      	    printf("rv is: $rv \n");
      	    printf("(%s)\n", bcm_errmsg(rv));
      	    return rv;
      	}
    }
    return rv;
}

int
qos_map_l3_egress_remark_profile(int unit)
{
    bcm_qos_map_t l3_eg_map;
    int flags = 0;
    int dscp;
    int MAX_PRI = 64;
    int dp;
    int rv;
 
    /* Clear structure */
    bcm_qos_map_t_init(&l3_eg_map);

    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_WITH_ID, &l3_eg_map_id);

    if (rv != BCM_E_NONE) {
        printf("error in L3 egress bcm_qos_map_create()\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
    } else {
        map_ids[map_id_cnt++] = l3_eg_map_id;
    }

    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE | BCM_QOS_MAP_WITH_ID, &l3_eg_opcode_id);
    if (rv != BCM_E_NONE) {
        printf("error in ingress l3 opcode bcm_qos_map_create()\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

    l3_eg_map.opcode = l3_eg_opcode_id;
    /* 
     * Call map add with BCM_QOS_MAP_OPCODE flag in order to register the internal map ID,
     * this is done once for each map ID and protocol 
     */ 
    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    rv = bcm_qos_map_add(unit, flags, &l3_eg_map, l3_eg_map_id);
    	
    if (rv != BCM_E_NONE) {
        printf("error in L3 egress bcm_qos_map_add()\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV6 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    rv = bcm_qos_map_add(unit, flags, &l3_eg_map, l3_eg_map_id);

    if (rv != BCM_E_NONE) {
        printf("error in L3 egress bcm_qos_map_add()\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

    /*add current layer mapping */
    for (dscp=0; dscp<MAX_PRI; dscp++) {
        for (dp=0; dp < 2; dp++) {
            bcm_qos_map_t_init(&l3_eg_map);

            /* Set internal priority  */
            l3_eg_map.int_pri = dscp;
            if (dscp < MAX_PRI/2) {
                flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_REMARK;
            } else {
                flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV6 | BCM_QOS_MAP_REMARK;
            }

            /* Set internal color */
            if (dp == 0){
                l3_eg_map.color = bcmColorGreen;
            } else {
                l3_eg_map.color = bcmColorYellow;
            }
    	
            /* Set egress DSCP */
            l3_eg_map.dscp = qos_map_l3_remark_pri[dscp>>2];
    	
            rv = bcm_qos_map_add(unit, flags, &l3_eg_map, l3_eg_opcode_id);
    	
            if (rv != BCM_E_NONE) {
                 printf("error in L3 egress bcm_qos_map_add()\n");
                 printf("rv is: $rv \n");
                 printf("(%s)\n", bcm_errmsg(rv));
                 return rv;
            }
        }
    }
    /*inheritance to next layer mapping*/
     for (dscp=0; dscp<MAX_PRI; dscp++) {
        for (dp=0; dp < 2; dp++) {
            bcm_qos_map_t_init(&l3_eg_map);

            /*add mapping next layer*/
            /* Set internal priority  */
            l3_eg_map.int_pri = dscp;
            if (dscp < MAX_PRI/2) {
                flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_REMARK;
            } else {
                flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV6 | BCM_QOS_MAP_REMARK;
            }
            /* Set internal color */
            if (dp == 0){
                l3_eg_map.color = bcmColorGreen;
                l3_eg_map.remark_color = bcmColorGreen;
            } else {
                l3_eg_map.color = bcmColorYellow;
                l3_eg_map.remark_color = bcmColorYellow;
            }
    	
			l3_eg_map.remark_int_pri = dscp;
    	
            rv = bcm_qos_map_add(unit, flags, &l3_eg_map, l3_eg_map_id);
    	
            if (rv != BCM_E_NONE) {
              printf("error in L3 egress bcm_qos_map_add()\n");
              printf("rv is: $rv \n");
              printf("(%s)\n", bcm_errmsg(rv));
              return rv;
            }
        }
    }
    return rv;
}

/** 
 *  ing_qos_profile - ingress map id value, in case of -1 the
 *                    default value defined in the global
 *                    variable is used
 *  egr_qos_profile - egress map id value, in case of -1 the
 *                    default value defined in the global
 *                    variable is used
 */
int
qos_map_l3_remark(int unit, int ing_qos_profile, int egr_qos_profile)
{
    int rv = BCM_E_NONE;

    if (ing_qos_profile != -1)
    {
        l3_in_map_id = ing_qos_profile;
    }
    if (egr_qos_profile != -1)
    {
        l3_eg_map_id = egr_qos_profile;
    }
    
    rv = qos_map_l3_egress_remark_profile(unit);

    if (rv) {
	printf("error setting up egress qos profile\n");
	return rv;
    }

    rv = qos_map_l3_ingress_remark_profile(unit);
    if (rv) {
	printf("error setting up ingress qos profile\n");
	return rv;
    }

    return rv;
}

int
qos_map_phb_remark_flags_resolve(int unit, uint32 qos_map_flags, uint32 *flags, int *nof_qos)
{
    if (qos_map_flags & BCM_QOS_MAP_IPV6) {
        *flags |= BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV6;
        *nof_qos = 256;
    } else if (qos_map_flags & BCM_QOS_MAP_IPV4) {
        *flags |= BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4;
        *nof_qos = 64;
    } else if (qos_map_flags & BCM_QOS_MAP_MPLS) {
        *flags |= BCM_QOS_MAP_MPLS;
        *nof_qos = 8;
    } else if (qos_map_flags & BCM_QOS_MAP_L2_OUTER_TAG) {
        *flags |= BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_OUTER_TAG;
        *nof_qos = 16;
    } else if (qos_map_flags & BCM_QOS_MAP_L2_INNER_TAG) {
        *flags |= BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_INNER_TAG;
        *nof_qos = 16;
    } else if (qos_map_flags & BCM_QOS_MAP_L2_UNTAGGED) {
        *flags |= BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_UNTAGGED;
        *nof_qos = 16;
    } else if (qos_map_flags & BCM_QOS_MAP_L2_VLAN_PCP) {
        *flags |= BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_VLAN_PCP;
        *nof_qos = 16;
    } else {
        printf("error config in preparing qos map flags (0x%08X)!\n", qos_map_flags);
        return BCM_E_CONFIG;
    }

    return BCM_E_NONE;
}

int
qos_map_ingress_phb_remark_map_entry_resolve(int unit, uint32 flags, int qos_value, int nof_qos, bcm_qos_map_t *qos_map)
{
    if (flags & BCM_QOS_MAP_IPV6) {
         /** Map-key: ingress dscp Priority, used for L3 remark */
        qos_map->dscp = qos_value;
    } else if (flags & BCM_QOS_MAP_IPV4) {
        /** Map-key: ingress dscp Priority, used for L3 remark */
        qos_map->dscp = qos_value << 2;
    } else if (flags & BCM_QOS_MAP_L2) {
        /** Map-key: ingress pkt Priority and cfi, used for L2 PHP */
        qos_map->pkt_pri = qos_value >> 1;
        qos_map->pkt_cfi = qos_value % 2;
    } else if (flags & BCM_QOS_MAP_L2_INNER_TAG) {
        /** Map-key: ingress pkt Priority and cfi, used for L2 PHB */
        qos_map->inner_pkt_pri = qos_value >> 1;
        qos_map->inner_pkt_cfi = qos_value % 2;
    } else if (flags & BCM_QOS_MAP_MPLS) {
        qos_map->exp = qos_value % 8;
    } else {
        printf("error config found in resolving ingress qos map entry (0x%08X)!\n", flags);
        return BCM_E_CONFIG;
    }

    /** map-result: TC */
    qos_map->int_pri = ((qos_value + 1) & (nof_qos - 1)) % 8;

    /** map-result: DP */
    qos_map->color = ((qos_value + 1) & (nof_qos - 1)) % 2;

    /** map-result: Network_qos*/
    qos_map->remark_int_pri = ((qos_value + 1) & (nof_qos - 1));

    return BCM_E_NONE;
}

int
qos_map_egress_phb_remark_map_entry_resolve(int unit, uint32 flags, int qos_value, int nof_qos, bcm_qos_map_t *qos_map)
{
    /* 
     * For JR2, map-key: int_pri & color, opcode
     *          map-result: dscp(L3) or pkt_pri/cfi(L2) or exp(MPLS)
     *                      and remark_int_pri/remark_color for inhertance
     * For JR+, map-key: int_pri & color, cos-profile
     *          map-result: remark_int_pri
     */

    /** map-key: int_pri & color */
    qos_map->int_pri = qos_value;
    qos_map->color = qos_value % 2;

    /** map-result: current network-qos */
    if (flags & BCM_QOS_MAP_IPV6) {
        qos_map->dscp = ((qos_value - 2) & (nof_qos - 1));
        /* for JR+*/
        qos_map->remark_int_pri = qos_map->dscp;
    } else if (flags & BCM_QOS_MAP_IPV4) {
        qos_map->dscp = ((qos_value - 2) & (nof_qos - 1)) << 2;
        /* for JR+*/
        qos_map->remark_int_pri = qos_map->dscp;
    } else if (flags & BCM_QOS_MAP_L2) {
        qos_map->pkt_pri = ((qos_value - 2) >> 1) & 0x7;
        qos_map->pkt_cfi = (qos_value - 2) & 0x1;
    } else if (flags & BCM_QOS_MAP_L2_INNER_TAG) {
        qos_map->inner_pkt_pri = ((qos_value - 2) >> 1) & 0x7;
        qos_map->inner_pkt_cfi = (qos_value - 2) & 0x1;
    } else if (flags & BCM_QOS_MAP_MPLS) {
        qos_map->exp = (qos_value - 2) & 0x7;
        /* for JR+*/
        qos_map->remark_int_pri = qos_map->exp;
    } else {
        printf("error config found in resolving egress qos maps (0x%08X)!\n", flags);
        return BCM_E_CONFIG;
    }

    if (is_device_or_above(unit,JERICHO2)) {
        /** map-result: next-layer network-qos */
        qos_map->remark_int_pri = qos_value;
        qos_map->remark_color = qos_value % 2;
    }

    return BCM_E_NONE;
}


/*
 * phb_flags - Qos map flags for PHB
 * remark_flags - Qos map flags for REMARK
 */
int
qos_map_ingress_phb_remark_set(int unit, uint32 phb_flags, uint32 remark_flags)
{
    bcm_qos_map_t l3_in_map;
    int dscp;
    int rv = BCM_E_NONE;
    uint32 flags, remark_map_flags, phb_map_flags;
    int qos, nof_qos_phb , nof_qos_remark;
    int opcode_id = l3_in_opcode_id;

    /** Clear structure */
    bcm_qos_map_t_init(&l3_in_map);

    /** 1. Create qos-profile */
    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_WITH_ID;
    if (is_device_or_above(unit,JERICHO2)) {
        flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_WITH_ID;
    }

    rv = bcm_qos_map_create(unit, flags, &l3_in_map_id);
    if (rv != BCM_E_NONE) {
        printf("error (%d) in bcm_qos_map_create() for ingress cos-profile\n", rv);
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

    printf("bcm_qos_map_create ingress cos-profile: %d\n", l3_in_map_id);

    flags = 0;

    rv = qos_map_phb_remark_flags_resolve(unit, remark_flags, &remark_map_flags, &nof_qos_remark);
    if (rv != BCM_E_NONE) {
        printf("error (%d) in resolving remark map flags (0x%X) for ingress\n", rv, remark_flags);
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

    if (is_device_or_above(unit,JERICHO2)) {
        /** Create opcode for PHB */
        opcode_id = l3_in_opcode_id;
        rv = bcm_qos_map_create(unit, BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_OPCODE | BCM_QOS_MAP_WITH_ID, &opcode_id);
        if (rv != BCM_E_NONE) {
            printf("error (%d) in bcm_qos_map_create() for ingress opcode\n", rv);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }

        printf("bcm_qos_map_create ingress PHB opcode: %d\n", l3_in_opcode_id);

        rv = qos_map_phb_remark_flags_resolve(unit, phb_flags, &phb_map_flags, &nof_qos_phb);
        if (rv != BCM_E_NONE) {
            printf("error (%d) in resolving phb map flags (0x%X) for ingress\n", rv, phb_flags);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }

        /** Add the maps for PHB */
        l3_in_map.opcode = opcode_id;
        phb_map_flags |= BCM_QOS_MAP_OPCODE;
        phb_map_flags |= BCM_QOS_MAP_PHB;
        rv = bcm_qos_map_add(unit, phb_map_flags, &l3_in_map, l3_in_map_id);
        if (rv != BCM_E_NONE) {
            printf("error (%d) in bcm_qos_map_add() for ingress opcode for PHB\n", rv);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }

        /** Add maps from {packet.qos & opcode} to {tc,dp} for JR2 only*/
        phb_map_flags &= (~BCM_QOS_MAP_OPCODE);
        for (qos = 0; qos < nof_qos_phb; qos++) {
            bcm_qos_map_t_init(&l3_in_map);
            rv = qos_map_ingress_phb_remark_map_entry_resolve(unit, phb_map_flags, qos, nof_qos_phb, &l3_in_map);
            if (rv != BCM_E_NONE) {
                printf("error (%d) in qos_map_ingress_phb_remark_map_entry_resolve for PHB\n", rv);
                printf("(%s)\n", bcm_errmsg(rv));
                return rv;
            }

            rv = bcm_qos_map_add(unit, phb_map_flags, &l3_in_map, opcode_id);
            if (rv != BCM_E_NONE) {
                printf("error (%d) in bcm_qos_map_add() for ingress qos-map for PHB\n", rv);
                printf("(%s)\n", bcm_errmsg(rv));
                return rv;
            }
        }

        /** Create opcode for remark*/
        opcode_id = l3_in_opcode_id;
        rv = bcm_qos_map_create(unit, BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE | BCM_QOS_MAP_WITH_ID, &opcode_id);
        if (rv != BCM_E_NONE) {
            printf("error (%d) in bcm_qos_map_create() for ingress remark opcode\n", rv);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }
    
        printf("bcm_qos_map_create ingress REMARK opcode: %d\n", l3_in_opcode_id);
    
        l3_in_map.opcode = opcode_id;
    
        /*
         * 3. Map cos-profile and cos-type to opcode.
         * For dscp remarking for example, the map_flags is something like:
         * BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE
         */
        /** Add the maps for remarking */
        remark_map_flags |= BCM_QOS_MAP_OPCODE;
        remark_map_flags |= BCM_QOS_MAP_REMARK;
        rv = bcm_qos_map_add(unit, remark_map_flags, &l3_in_map, l3_in_map_id);
        if (rv != BCM_E_NONE) {
            printf("error (%d) in bcm_qos_map_add() for ingress opcode for remarking\n", rv);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }
    }

    /*
     *  Map packet.qos and opcode to network_qos and tc/dp. 
     * For dscp remarking for example,the map_flags is something like (For JR2):
     * BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_REMARK
     * the map_flags is something like (For JR):
     * BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4
     */
    for (qos = 0; qos < nof_qos_remark; qos++) {
        bcm_qos_map_t_init(&l3_in_map);
        rv = qos_map_ingress_phb_remark_map_entry_resolve(unit, remark_map_flags, qos, nof_qos_remark, &l3_in_map);
        if (rv != BCM_E_NONE) {
            printf("error (%d) in qos_map_ingress_phb_remark_map_entry_resolve for REMARK\n", rv);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }

        if (!is_device_or_above(unit,JERICHO2) && (remark_flags & BCM_QOS_MAP_L3_L2)) {
            remark_map_flags |= BCM_QOS_MAP_L3_L2;
            /*
             * In JR+, if map pcpdei to dscp,l3_in_map.dscp carry pcpdei for mapping to in-dscp.
             * Also it is l3_in_map.dscp is used to map to tc/dp (not so reasonable).
             */
            if (remark_flags & (BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_IPV6 | BCM_QOS_MAP_MPLS)) {
                l3_in_map.dscp = qos;
                l3_in_map.color = 0;
            }
        }

        /** Add maps from {packet.qos & opcode} to {network_qos (and tc/dp for JR+)}*/
        remark_map_flags &= (is_device_or_above(unit,JERICHO2) ? (~BCM_QOS_MAP_OPCODE) : 0xFFFFFFFF);
        if (is_device_or_above(unit,JERICHO2)) {
             rv = bcm_qos_map_add(unit, remark_map_flags, &l3_in_map, opcode_id);
        } else {
            rv = bcm_qos_map_add(unit, remark_map_flags, &l3_in_map, l3_in_map_id);
        }
        if (rv != BCM_E_NONE) {
            printf("error (%d) in bcm_qos_map_add() for ingress qos-map\n", rv);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }
    }

    return rv;
}


/*
 * remark_flags - Qos map flags for egress remark qos map add.
 * fwd_hrd_flags - forward head flags for egress QoS opcode.
 *                 Used for inidicating mark L3/MPLS/ETH
 */
int
qos_map_egress_phb_remark_set(int unit, uint32 remark_flags, uint32 fwd_hrd_flags)
{
    bcm_qos_map_t l3_eg_map;
    int flags = 0;
    int qos;
    int nof_qos = 64;
    int dp;
    int rv;
    uint32 remark_map_flags, create_flags, opcode_app_flags;
 
    /** Clear structure */
    bcm_qos_map_t_init(&l3_eg_map);

    /** 1. Create qos-profile */
    create_flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_WITH_ID;
    if (is_device_or_above(unit,JERICHO2)) {
        create_flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_WITH_ID;
    }
    rv = bcm_qos_map_create(unit, create_flags, &l3_eg_map_id);
    if (rv != BCM_E_NONE) {
        printf("error (%d) in bcm_qos_map_create() for egress cos-profile\n", rv);
        printf("(%s)\n", bcm_errmsg(rv));
    } else {
        map_ids[map_id_cnt++] = l3_eg_map_id;
    }

    printf("bcm_qos_map_create egress cos-profile: %d\n", l3_eg_map_id);

    /** Prepare map flags*/
    remark_map_flags = 0;
    rv = qos_map_phb_remark_flags_resolve(unit, remark_flags, &remark_map_flags, &nof_qos);
    if (rv != BCM_E_NONE) {
        printf("error (%d) in resolving remark_flags (0x%X) for egress\n", rv, remark_flags);
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

    if (qos_l3_map_to_l2 && (remark_map_flags & BCM_QOS_MAP_L2)) {
        nof_qos = 64;
    }
    if (is_device_or_above(unit,JERICHO2)) {
        /** 2. Create opcode */
        rv = bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE | BCM_QOS_MAP_WITH_ID, &l3_eg_opcode_id);
        if (rv != BCM_E_NONE) {
            printf("error  (%d) in bcm_qos_map_create() for egress opcode\n", rv);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }

        printf("bcm_qos_map_create egress opcode: %d\n", l3_eg_opcode_id);

        l3_eg_map.opcode = l3_eg_opcode_id;

        int tmp;
        opcode_app_flags = 0;
        rv = qos_map_phb_remark_flags_resolve(unit, fwd_hrd_flags, &opcode_app_flags, &tmp);
        if (rv != BCM_E_NONE) {
            printf("error (%d) in resolving fwd_hrd_flags (0x%X) for ingress\n", rv, fwd_hrd_flags);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }

        opcode_app_flags |= BCM_QOS_MAP_REMARK;
        opcode_app_flags |= BCM_QOS_MAP_OPCODE;

        /*
         * 3. Map cos-profile and cos-type to opcode.
         * For dscp remarking for example, the map_flags is something like:
         * BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE
         */
        rv = bcm_qos_map_add(unit, opcode_app_flags, &l3_eg_map, l3_eg_map_id);
        if (rv != BCM_E_NONE) {
            printf("error (%d) in bcm_qos_map_add() for ingress opcode\n", rv);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }
    }

    /*
     * 4. Map packet.qos and opcode to network_qos.
     * For dscp remarking for example, the map_flags is something like (For JR2):
     * BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_REMARK
     * the map_flags is something like (For JR):
     * BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4
     */
    for (qos = 0; qos < nof_qos; qos++) {

        if (is_device_or_above(unit,JERICHO2)) {
            remark_map_flags |= BCM_QOS_MAP_REMARK;
        }

        /* 
         * For JR2, map-key: int_pri & color, opcode
         *          map-result: dscp(L3) or pkt_pri/cfi(L2) or exp(MPLS)
         *                      and remark_int_pri/remark_color for inhertance
         * For JR+, map-key: int_pri & color, cos-profile
         *          map-result: remark_int_pri
         */
        rv = qos_map_egress_phb_remark_map_entry_resolve(unit, remark_map_flags, qos, nof_qos, &l3_eg_map);
        if (rv != BCM_E_NONE) {
            printf("error (%d) in qos_map_egress_phb_remark_map_entry_resolve\n", rv);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }

        if (is_device_or_above(unit,JERICHO2)) {
             rv = bcm_qos_map_add(unit, remark_map_flags, &l3_eg_map, l3_eg_opcode_id);
        } else {
             rv = bcm_qos_map_add(unit, remark_map_flags, &l3_eg_map, l3_eg_map_id);
        }
        if (rv != BCM_E_NONE) {
            printf("error (%d) in bcm_qos_map_add() for egress qos-map (%d, %d)\n", rv, qos, nof_qos);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }
    }
    return rv;
}


/**
 *  ing_phb_flags - flags for ingress qos PHB map. The possible flags are:
 *                  BCM_QOS_MAP_IPV4, BCM_QOS_MAP_IPV6, BCM_QOS_MAP_MPLS
 *                  BCM_QOS_MAP_L2_OUTER_TAG, BCM_QOS_MAP_L2_INNER_TAG 
 *                  BCM_QOS_MAP_L2_UNTAGGED, BCM_QOS_MAP_L2_VLAN_PCP
 *  ing_remark_flags - flags for ingress qos REMARK map.
 *                     The possible flags are the same ing_phb_flags.
 *  egr_remark_flags - flags for egress qos REMARK map. 
 *                     The possible flags are the same ing_phb_flags.
 *  ing_gport - gport for ingress LIF for which to set the cos-profile
 *              No action for it if it is invalid.
 *  ing_gport - gport for egress LIF for which to set the cos-profile
 *              No action for it if it is invalid.
 *  ing_qos_profile - ingress map id value, in case of -1 the
 *                    default value defined in the global
 *                    variable is used
 *  egr_qos_profile - egress map id value, in case of -1 the
 *                    default value defined in the global
 *                    variable is used
 *
 *  This procedure create the cos-profile and add qos mapping.
 *  At ingress, it resusts as pph.nwk-qos/in-dscp-exp = packet.qos + 1.
 *  At egress, it resusts as packet.qos = pph.nwk-qos/in-dscp-exp - 2.
 *  So, finally out-packet.qos = in-packet.qos - 1.
 */
int
qos_map_phb_remark_set(int unit,
                       uint32 ing_phb_flags,
                       uint32 ing_remark_flags,
                       uint32 egr_remark_flags,
                       int ing_gport,
                       int egr_gport,
                       int ing_qos_profile,
                       int egr_qos_profile)
{
    int rv = BCM_E_NONE;
    int in_qos_profile, out_qos_profile;

    if (ing_qos_profile != -1)
    {
        l3_in_map_id = ing_qos_profile;
    }
    if (egr_qos_profile != -1)
    {
        l3_eg_map_id = egr_qos_profile;
    }

    rv = qos_map_egress_phb_remark_set(unit, egr_remark_flags, egr_remark_flags);
    if (rv) {
        printf("error (%d) setting up egress qos profile\n", rv);
        return rv;
    }

    if (egr_gport != BCM_GPORT_INVALID)
    {
        out_qos_profile = ((0x2000 | 0x1 | 0x2) << 16) | l3_eg_map_id;
        rv = bcm_qos_port_map_set(unit, egr_gport, -1, out_qos_profile);
        if (rv) {
            printf("error (%d) setting ingress qos profile in OutLif\n", rv);
            return rv;
        }
    }

    rv = qos_map_ingress_phb_remark_set(unit, ing_phb_flags, ing_remark_flags);
    if (rv) {
        printf("error (%d) setting up ingress qos profile\n", rv);
        return rv;
    }

    if (ing_gport != BCM_GPORT_INVALID)
    {
        in_qos_profile = ((0x1000 | 0x1 | 0x2) << 16) | l3_in_map_id;
        rv = bcm_qos_port_map_set(unit, ing_gport, in_qos_profile, -1);
        if (rv) {
            printf("error (%d) setting ingress qos profile in InLif\n", rv);
            return rv;
        }
    }

    return rv;
}

int
qos_map_ingress_phb_remark_clear(int unit, uint32 phb_flags, uint32 remark_flags)
{
    bcm_qos_map_t l3_in_map;
    int rv = BCM_E_NONE;
    uint32 flags, remark_map_flags, phb_map_flags;
    int qos, nof_qos_phb, nof_qos_remark;
    int ing_qos_map_id = ((0x1000 | 0x1 | 0x2) << 16) | l3_in_map_id;
    int ing_qos_phb_opcode = ((0x1000 | 0x8 | 0x1) << 16) | l3_in_opcode_id;
    int ing_qos_remark_opcode = ((0x1000 | 0x8 | 0x2) << 16) | l3_in_opcode_id;

    if (!is_device_or_above(unit, JERICHO2))
    {
        ing_qos_map_id = ((0x1000 | 0x2) << 16) | l3_in_map_id;
    }

    /** Clear structure */
    bcm_qos_map_t_init(&l3_in_map);

    rv = qos_map_phb_remark_flags_resolve(unit, remark_flags, &remark_map_flags, &nof_qos_remark);
    if (rv != BCM_E_NONE) {
        printf("error (%d) in resolving remark map flags (0x%X) for ingress\n", rv, remark_flags);
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

    /*
     * 1. Delete the Maps from cos-profile and cos-type to opcode. 
     * For dscp remarking for example, the map_flags is something like:
     * BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE
     */
    if (is_device_or_above(unit,JERICHO2)) {

        l3_in_map.opcode = ing_qos_remark_opcode;

        /** Delete the map for remark*/
        remark_map_flags |= BCM_QOS_MAP_OPCODE;
        remark_map_flags |= BCM_QOS_MAP_REMARK;
        rv = bcm_qos_map_delete(unit, remark_map_flags, &l3_in_map, ing_qos_map_id);
        if (rv != BCM_E_NONE) {
            printf("error (%d) in bcm_qos_map_delete() for ingress opcode for remark\n", rv);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }

        /** Delete the map for phb*/
        rv = qos_map_phb_remark_flags_resolve(unit, phb_flags, &phb_map_flags, &nof_qos_phb);
        if (rv != BCM_E_NONE) {
            printf("error (%d) in resolving phb map flags (0x%X) for ingress\n", rv, phb_flags);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }

        l3_in_map.opcode = ing_qos_phb_opcode;
        phb_map_flags |= BCM_QOS_MAP_OPCODE;
        phb_map_flags |= BCM_QOS_MAP_PHB;
        rv = bcm_qos_map_delete(unit, phb_map_flags, &l3_in_map, ing_qos_map_id);
        if (rv != BCM_E_NONE) {
            printf("error (%d) in bcm_qos_map_delete() for ingress opcode for phb\n", rv);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }

        /** 2.1 Delete the maps from {packet.qos & opcode} to {tc,dp} for JR2*/
        phb_map_flags &= (~BCM_QOS_MAP_OPCODE);
        for (qos = 0; qos < nof_qos_phb; qos++) {
             bcm_qos_map_t_init(&l3_in_map);
            rv = qos_map_ingress_phb_remark_map_entry_resolve(unit, phb_map_flags, qos, nof_qos_phb, &l3_in_map);
            if (rv != BCM_E_NONE) {
                printf("error (%d) in qos_map_ingress_phb_remark_map_entry_resolve\n", rv);
                printf("(%s)\n", bcm_errmsg(rv));
                return rv;
            }

            rv = bcm_qos_map_delete(unit, phb_map_flags, &l3_in_map, ing_qos_phb_opcode);
            if (rv != BCM_E_NONE) {
                printf("error (%d) in bcm_qos_map_add() for ingress qos-map for PHB\n", rv);
                printf("(%s)\n", bcm_errmsg(rv));
                return rv;
            }
        }
    }

    /*
     * 2.2. Delete the Maps from packet.qos and opcode to network_qos/tc&dp.
     * for dscp remarking for example, the map_flags is something like (For JR2):
     * BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_REMARK
     * the map_flags is something like (For JR):
     * BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4
     */
    for (qos = 0; qos < nof_qos_remark; qos++) {
        rv = qos_map_ingress_phb_remark_map_entry_resolve(unit, remark_map_flags, qos, nof_qos_remark, &l3_in_map);
        if (rv != BCM_E_NONE) {
            printf("error (%d) in qos_map_ingress_phb_remark_map_entry_resolve\n", rv);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }

        /** Delete the maps from {packet.qos, opcode} to {nwk_qos (and tc/dp for JR+)}*/
        remark_map_flags &= (is_device_or_above(unit,JERICHO2) ? (~BCM_QOS_MAP_OPCODE) : 0xFFFFFFFF);
         if (is_device_or_above(unit,JERICHO2)) {
              rv = bcm_qos_map_delete(unit, remark_map_flags, &l3_in_map, ing_qos_remark_opcode);
         } else {
              rv = bcm_qos_map_delete(unit, remark_map_flags, &l3_in_map, ing_qos_map_id);
         }
        
        if (rv != BCM_E_NONE) {
            printf("error (%d) in bcm_qos_map_delete() for ingress qos-map\n", rv);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }
    }

    /** 3. Destroy the opcode */
    if (is_device_or_above(unit,JERICHO2)) {
        rv = bcm_qos_map_destroy(unit, ing_qos_remark_opcode);
        if (rv != BCM_E_NONE) {
            printf("error (%d) in bcm_qos_map_destroy() for ingress opcode\n", rv);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }
        rv = bcm_qos_map_destroy(unit, ing_qos_phb_opcode);
        if (rv != BCM_E_NONE) {
            printf("error (%d) in bcm_qos_map_destroy() for ingress opcode\n", rv);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }

        printf("bcm_qos_map_destroy ingress opcode: %d\n", l3_in_opcode_id);
    }

    /** 4. Destroy cos-profile */
    rv = bcm_qos_map_destroy(unit, ing_qos_map_id);
    if (rv != BCM_E_NONE) {
        printf("error (%d) in bcm_qos_map_create() for ingress cos-profile\n", rv);
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

    printf("bcm_qos_map_destroy ingress cos-profile: %d\n", l3_in_map_id);

    return rv;
}


int
qos_map_egress_phb_remark_clear(int unit, uint32 remark_flags, uint32 fwd_hrd_flags)
{
    bcm_qos_map_t l3_eg_map;
    int flags = 0;
    int qos;
    int nof_qos = 64;
    int dp;
    int rv;
    uint32 remark_map_flags, create_flags, opcode_app_flags;

    int egr_qos_map_id = ((0x2000 | 0x1 | 0x2) << 16) | l3_eg_map_id;
    int egr_qos_opcode = ((0x2000 | 0x8 | 0x2) << 16) | l3_eg_opcode_id;

    if (!is_device_or_above(unit, JERICHO2))
    {
        egr_qos_map_id = ((0x2000 | 0x4) << 16) | l3_eg_map_id;
    }
 
    /** Clear structure */
    bcm_qos_map_t_init(&l3_eg_map);

    /** Prepare map flags*/
    remark_map_flags = 0;
    rv = qos_map_phb_remark_flags_resolve(unit, remark_flags, &remark_map_flags, &nof_qos);
    if (rv != BCM_E_NONE) {
        printf("error (%d) in resolving remark_flags (0x%X) for egress\n", rv, remark_flags);
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

    if (is_device_or_above(unit,JERICHO2)) {

        l3_eg_map.opcode = egr_qos_opcode;

        int tmp;
        opcode_app_flags = 0;
        rv = qos_map_phb_remark_flags_resolve(unit, fwd_hrd_flags, &opcode_app_flags, &tmp);
        if (rv != BCM_E_NONE) {
            printf("error (%d) in resolving fwd_hrd_flags (0x%X) for egress\n", rv, fwd_hrd_flags);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }

        opcode_app_flags |= BCM_QOS_MAP_REMARK;
        opcode_app_flags |= BCM_QOS_MAP_OPCODE;

        /*
         * 1. Delete the Maps from cos-profile and cos-type to opcode.
         * For dscp remarking for example, the map_flags is something like:
         * BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE
         */
        rv = bcm_qos_map_delete(unit, opcode_app_flags, &l3_eg_map, egr_qos_map_id);
        if (rv != BCM_E_NONE) {
            printf("error (%d) in bcm_qos_map_delete() for egress opcode\n", rv);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }
    }

    /*
     * 2. Delete the Maps from packet.qos and opcode to network_qos.
     * For dscp remarking for example, the map_flags is something like (For JR2):
     * BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_REMARK
     * the map_flags is something like (For JR):
     * BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4
     */
    for (qos = 0; qos < nof_qos; qos++) {
        bcm_qos_map_t_init(&l3_eg_map);
        if (is_device_or_above(unit,JERICHO2)) {
            remark_map_flags |= BCM_QOS_MAP_REMARK;
        }

        /* 
         * For JR2, map-key: int_pri & color, opcode
         *          map-result: dscp(L3) or pkt_pri/cfi(L2) or exp(MPLS)
         *                      and remark_int_pri/remark_color for inhertance
         * For JR+, map-key: int_pri & color, cos-profile
         *          map-result: remark_int_pri
         */
        rv = qos_map_egress_phb_remark_map_entry_resolve(unit, remark_map_flags, qos, nof_qos, &l3_eg_map);
        if (rv != BCM_E_NONE) {
            printf("error (%d) in qos_map_egress_phb_remark_map_entry_resolve\n", rv);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }
        if (is_device_or_above(unit,JERICHO2)) {
            rv = bcm_qos_map_delete(unit, remark_map_flags, &l3_eg_map, egr_qos_opcode);
        } else {
            rv = bcm_qos_map_delete(unit, remark_map_flags, &l3_eg_map, egr_qos_map_id);
        }
        if (rv != BCM_E_NONE) {
            printf("error (%d) in bcm_qos_map_delete() for egress qos-map\n", rv);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }
    }

    if (is_device_or_above(unit,JERICHO2)) {
        /** 3. Destroy opcode */
        rv = bcm_qos_map_destroy(unit, egr_qos_opcode);
        if (rv != BCM_E_NONE) {
            printf("error  (%d) in bcm_qos_map_destroy() for egress opcode\n", rv);
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }

        printf("bcm_qos_map_destroy egress opcode: %d\n", l3_eg_opcode_id);
    }

    /** 4. Destroy cos-profile */
    rv = bcm_qos_map_destroy(unit, egr_qos_map_id);
    if (rv != BCM_E_NONE) {
        printf("error (%d) in bcm_qos_map_destroy() for egress cos-profile\n", rv);
        printf("(%s)\n", bcm_errmsg(rv));
    }

    printf("bcm_qos_map_destroy egress cos-profile: %d\n", l3_eg_map_id);
    return rv;
}


/**
 *
 *  This procedure clear the cos-profile and add qos mapping.
 *  At ingress, it resusts as pph.nwk-qos/in-dscp-exp = packet.qos + 1.
 *  At egress, it resusts as packet.qos = pph.nwk-qos/in-dscp-exp - 2.
 *  So, finally out-packet.qos = in-packet.qos - 1.
 */
int
qos_map_phb_remark_clear(int unit,
                         uint32 ing_phb_flags,
                         uint32 ing_remark_flags,
                         uint32 egr_remark_flags,
                         int ing_gport,
                         int egr_gport,
                         int ing_qos_profile,
                         int egr_qos_profile)
{
    int rv = BCM_E_NONE;
    int in_qos_profile, out_qos_profile;

    if (ing_qos_profile != -1)
    {
        l3_in_map_id = ing_qos_profile;
    }
    if (egr_qos_profile != -1)
    {
        l3_eg_map_id = egr_qos_profile;
    }

    in_qos_profile = ((0x1000 | 0x1 | 0x2) << 16) | 0;
    out_qos_profile = ((0x2000 | 0x1 | 0x2) << 16) | 0;

    if (egr_gport != BCM_GPORT_INVALID)
    {
        rv = bcm_qos_port_map_set(unit, egr_gport, -1, out_qos_profile);
        if (rv) {
            printf("error (%d) clear ingress qos profile in OutLif\n", rv);
            return rv;
        }
    }

    rv = qos_map_egress_phb_remark_clear(unit, egr_remark_flags, egr_remark_flags);
    if (rv) {
        printf("error (%d) clear up egress qos profile\n", rv);
        return rv;
    }

    if (ing_gport != BCM_GPORT_INVALID)
    {
        rv = bcm_qos_port_map_set(unit, ing_gport, in_qos_profile, -1);
        if (rv) {
            printf("error (%d) clear ingress qos profile in InLif\n", rv);
            return rv;
        }
    }

    rv = qos_map_ingress_phb_remark_clear(unit, ing_phb_flags, ing_remark_flags);
    if (rv) {
        printf("error (%d) clear up ingress qos profile\n", rv);
        return rv;
    }

    return rv;
}

/*
 * For routing packets, map L3 IPV4.TOS/IPV6.TC/MPLS.EXP to NWK_QOS/IN_DSCP_EXP
 * For routing packets, map L3 IPV4.TOS/IPV6.TC/MPLS.EXP to PHB
 * Possible qos_map_flags: BCM_QOS_MAP_IPV4/BCM_QOS_MAP_IPV6/BCM_QOS_MAP_MPLS
 */
int
qos_map_l3_dscp_config(int unit,
                       uint32 qos_map_flags,
                       int ing_gport,
                       int egr_gport,
                       int ing_qos_profile,
                       int egr_qos_profile)
{
    return qos_map_phb_remark_set(unit,
                                  qos_map_flags,qos_map_flags,qos_map_flags,
                                  ing_gport,egr_gport,ing_qos_profile,egr_qos_profile);
}

int
qos_map_l3_dscp_clear(int unit,
                      uint32 qos_map_flags,
                      int ing_gport,
                      int egr_gport,
                      int ing_qos_profile,
                      int egr_qos_profile)
{
    return qos_map_phb_remark_clear(unit,
                                  qos_map_flags,qos_map_flags,qos_map_flags,
                                  ing_gport,egr_gport,ing_qos_profile,egr_qos_profile);
}

/*
 * For routing packets, map L2 ETH.PCPDEI to NWK_QOS/IN_DSCP_EXP
 * For routing packets, map L3 IPV4.TOS/IPV6.TC/MPLS.EXP to PHB
 * Possible qos_map_flags: BCM_QOS_MAP_IPV4/BCM_QOS_MAP_IPV6/BCM_QOS_MAP_MPLS
 */
int
qos_map_l3_pcpdei_config(int unit,
                         uint32 qos_map_flags,
                         int ing_gport,
                         int egr_gport,
                         int ing_qos_profile,
                         int egr_qos_profile)
{
    uint32 ing_phb_flags, ing_remark_flags, egr_remark_flags;

    if (is_device_or_above(unit,JERICHO2)) {
        ing_phb_flags = BCM_QOS_MAP_L2_OUTER_TAG;
        ing_remark_flags = BCM_QOS_MAP_L2_OUTER_TAG;
        egr_remark_flags = qos_map_flags;
    } else {
        ing_phb_flags = 0;
        ing_remark_flags = qos_map_flags | BCM_QOS_MAP_L3_L2;
        egr_remark_flags = qos_map_flags;
    }
    return qos_map_phb_remark_set(unit,
                                  ing_phb_flags,ing_remark_flags,egr_remark_flags,
                                  ing_gport,egr_gport,ing_qos_profile,egr_qos_profile);
}

int
qos_map_l3_pcpdei_clear(int unit,
                        int32 qos_map_flags,
                        int ing_gport,
                        int egr_gport,
                        int ing_qos_profile,
                        int egr_qos_profile)
{
    uint32 ing_phb_flags, ing_remark_flags, egr_remark_flags;

    if (is_device_or_above(unit,JERICHO2)) {
        ing_phb_flags = BCM_QOS_MAP_L2_OUTER_TAG;
        ing_remark_flags = BCM_QOS_MAP_L2_OUTER_TAG;
        egr_remark_flags = qos_map_flags;
    } else {
        ing_phb_flags = 0;
        ing_remark_flags = qos_map_flags | BCM_QOS_MAP_L3_L2;
        egr_remark_flags = qos_map_flags;
    }

    return qos_map_phb_remark_clear(unit,
                                  ing_phb_flags,ing_remark_flags,egr_remark_flags,
                                  ing_gport,egr_gport,ing_qos_profile,egr_qos_profile);
}

/*
 * For JR2: outlif.remark_profile is used to set qos preserve by BCM_QOS_OPCODE_PRESERVE.
 *          So, a remark_profile should be provided, and port is ignored.
 * For JR1: bcmPortControlPreserveDscpEgress is used to the outlif.outlif_profile.
 *          So, a gport or lif should be provided, and qos_profile is ignored.
 * port_or_qos_profile: JR1 - it's egress gport; JR2 - it's egress remark_profile;
 * Enable: 1: enable preserve(DSCP is not remarked);
 *         0: disable preserve (DSCP is remarked).
 * Note: only routing case is supported.
 */
int qos_l3_dscp_preserve_per_outlif_set(int unit, int port_or_qos_profile, int enable)
{
    int rv =BCM_E_NONE;
    int qos_map_id;
    int dscp_preserve_enable = enable ? 1 : 0;
    uint32 flags;

    if (!is_device_or_above(unit, JERICHO2)) {
        int port = port_or_qos_profile;
        int route_qos_lif;
        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(route_qos_lif, port);

        rv = bcm_port_control_set(unit, route_qos_lif, bcmPortControlPreserveDscpEgress, dscp_preserve_enable);
        if (rv != BCM_E_NONE) {
            printf("set bcmPortControlPreserveDscpEgress failed, port(0x08x)\n", route_qos_lif);
            return rv;
        }
        printf("set bcmPortControlPreserveDscpEgress successfully for port(0x08x) with value of %d\n", route_qos_lif, dscp_preserve_enable);
    } else if (is_device_or_above(unit, JERICHO2)) {
        int qos_profile = port_or_qos_profile;
        flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK;
        rv = bcm_qos_map_id_get_by_profile(unit, flags, qos_profile, &qos_map_id);
        if (rv != BCM_E_NONE) {
            printf("Error %d, bcm_qos_map_id_get_by_profile\n", rv);
        }
        
        /** Set preserve for IPV4.DSCP*/
        bcm_qos_map_t map_entry;
        bcm_qos_map_t_init(&map_entry);
        map_entry.opcode = BCM_QOS_OPCODE_PRESERVE;
        flags = BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE | BCM_QOS_MAP_IPV4;
        rv = dscp_preserve_enable ?
             bcm_qos_map_add(unit, flags, &map_entry, qos_map_id) :
             bcm_qos_map_delete(unit, flags, &map_entry, qos_map_id);
        if (rv != BCM_E_NONE) {
            printf("Error %d, bcm_qos_map_%s for dscp preserve.\n", rv, dscp_preserve_enable ? "add" : "delete");
        }

        /** Set preserve for IPV6.TC*/
        bcm_qos_map_t_init(&map_entry);
        map_entry.opcode = BCM_QOS_OPCODE_PRESERVE;
        flags = BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE | BCM_QOS_MAP_IPV6;
        rv = dscp_preserve_enable ?
             bcm_qos_map_add(unit, flags, &map_entry, qos_map_id) :
             bcm_qos_map_delete(unit, flags, &map_entry, qos_map_id);
        if (rv != BCM_E_NONE) {
            printf("Error %d, bcm_qos_map_%s for dscp preserve.\n", rv, dscp_preserve_enable ? "add" : "delete");
        }
        
        printf("%s DSCP preserve for remark_profile 0x%X successfully!\n", (dscp_preserve_enable ? "Enable" : "Disable"), qos_map_id);
    }

    return rv;
}

/*
 * outlif.remark_profile is used to set qos preserve by BCM_QOS_OPCODE_PRESERVE.
 * Note: only routing case is supported.
 *
 * Note, used in JR2 mode only
 * Note, exposed to TCL
 */
int
qos_l3_dscp_preserve_per_outlif_config(int unit, int egr_port_or_qos_profile, int enable)
{
    int rv;
    int egr_port;
    int egr_qos_profile;

    if (is_device_or_above(unit, JERICHO2)) {
        if (egr_port_or_qos_profile == -1) {
            egr_qos_profile = l3_eg_map_id;
        } else {
            egr_qos_profile = egr_port_or_qos_profile;
        }
        rv = qos_l3_dscp_preserve_per_outlif_set(unit, egr_qos_profile, enable);
    } else if (egr_port_or_qos_profile != -1) {
        egr_port = egr_port_or_qos_profile;
        rv = qos_l3_dscp_preserve_per_outlif_set(unit, egr_port, enable);
    }
    if (rv != BCM_E_NONE) {
        printf("Error, qos_l3_dscp_preserve_per_outlif_config, port(0x08x)\n", egr_port_or_qos_profile);
        return rv;
    }
    
    return rv;
}


/*
 * iPMF for updating FAI[3] with the given value.
 * in-lif-profile is used for preselect
 * forwarding-type is IPv4 or IPv6
 *
 * Note, used in JR2 mode only
 */
int dnx_qos_l3_dscp_preserve_per_inlif_set(int unit, int in_lif_profile, bcm_field_layer_type_t forwarding_type, int enable)
{
    int rv= 0;
    bcm_field_context_t context_id;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_id_t presel_entry_id;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_context_param_info_t param_info;

    int is_jericho2_mode = *(dnxc_data_get(unit,  "headers", "system_headers", "system_headers_mode", NULL));
    if (!is_jericho2_mode) {
        printf("Error, Procedure is used for Jericho2 mode only, for Jericho1 mode, use bcmPortControlPreserveDscpIngress instead!\n");
        return BCM_E_UNAVAIL;
    }

    /********* Create PRESELECTOR *************/
    bcm_field_context_info_t_init(&context_info);
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &context_id);
    if(rv != BCM_E_NONE) {
        printf("dnx_qos_l3_dscp_preserve_per_inlif_set: Error in bcm_field_context_create context %d\n", context_id);
        return rv;
    }

    bcm_field_presel_entry_id_info_init(&presel_entry_id);

    /**For iPMF2, iPMF1 presel must be configured*/
    presel_entry_id.presel_id = fai_update_presel_id;
    presel_entry_id.stage = bcmFieldStageIngressPMF1;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.nof_qualifiers = 2;
    presel_entry_data.context_id = context_id;
    presel_entry_data.entry_valid = TRUE;
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyInVportClass;
    presel_entry_data.qual_data[0].qual_arg = 1;  /** in-lif-profile1 (in-lif-profile0 is from RIF)*/
    presel_entry_data.qual_data[0].qual_value = in_lif_profile;
    presel_entry_data.qual_data[0].qual_mask = 0xFF;
    presel_entry_data.qual_data[1].qual_type = bcmFieldQualifyForwardingType;
    presel_entry_data.qual_data[1].qual_arg = 0;
    presel_entry_data.qual_data[1].qual_value = forwarding_type;
    presel_entry_data.qual_data[1].qual_mask = 0x1F;
    rv = bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data);
    if(rv != BCM_E_NONE) {
        printf("cint_field_evpn_context_create Error in bcm_field_presel_set  \n");
        return rv;
    }

    /********* Add QUALIFIERS *********/
    bcm_field_group_info_t fg_info;
    int qual_and_action_index;
    char *proc_name;
    void *dst_char;
    bcm_field_group_t ingress_fg_id;
    proc_name = "field_dir_extract_fai_to_fai";

    /** Init the fg_info structure. */
    bcm_field_group_info_t_init(&fg_info);

    /*
     * Create user define qualifier with FAI[2:0] for FAI[2:0]
     */
    if (qual_id_fai_2_0 == bcmFieldQualifyCount) {
        bcm_field_qualifier_info_create_t qual_info;
        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size = 3;
        dst_char = &(qual_info.name[0]);
        sal_strncpy_s(dst_char, "FAI_BIT_2_0", sizeof(qual_info.name));
        rv = bcm_field_qualifier_create(unit, 0, &qual_info, &qual_id_fai_2_0);
        if(rv != BCM_E_NONE) {
            printf("%s Error in bcm_field_qualifier_create qual_id_fai_2_0\n", proc_name);
            return rv;
        }
    }

    /*
     * Create user define qualifier with CONST for FAI[3:3]
     */
    if (qual_id_const_1_bit == bcmFieldQualifyCount) {
        bcm_field_qualifier_info_create_t qual_info;
        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size = 1;
        dst_char = &(qual_info.name[0]);
        sal_strncpy_s(dst_char, "CONST_1_BIT", sizeof(qual_info.name));
        rv = bcm_field_qualifier_create(unit, 0, &qual_info, &qual_id_const_1_bit);
        if(rv != BCM_E_NONE) {
            printf("%s Error in bcm_field_qualifier_create qual_id_const_1_bit\n", proc_name);
            return rv;
        }
    }

    /*
     *  Fill the fg_info structure, Add the FG. 
     */
    fg_info.stage = bcmFieldStageIngressPMF2;
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.nof_quals = 2;
    fg_info.qual_types[0] = qual_id_fai_2_0;
    fg_info.qual_types[1] = qual_id_const_1_bit;

    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionForwardingAdditionalInfo;
    fg_info.action_with_valid_bit[0] = FALSE;
    rv = bcm_field_group_add(unit, 0, &fg_info, &ingress_fg_id);
    if (rv != BCM_E_NONE) {
        printf("%s Error (%d), in bcm_field_group_add\n", proc_name, rv);
        return rv;
    }

    /*
     * Attach the field group ID to the context.
     */
    int qual_idx = 0;
    int act_idx = 0;
    bcm_field_group_attach_info_t attach_info;

    bcm_field_group_info_t_init(&fg_info);
    bcm_field_group_info_get(unit, ingress_fg_id, &fg_info);

    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    for(qual_idx=0; qual_idx< fg_info.nof_quals; qual_idx++) {
        attach_info.key_info.qual_types[qual_idx] = fg_info.qual_types[qual_idx];
    }
    for(act_idx=0; act_idx<fg_info.nof_actions; act_idx++) {
        attach_info.payload_info.action_types[act_idx] = fg_info.action_types[act_idx];
    }

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].input_arg = 0; 
    attach_info.key_info.qual_info[0].offset = 0;
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[1].input_arg = enable ? 1 : 0; /** Set FAI[3]*/
    attach_info.key_info.qual_info[1].offset = 0;

    rv = bcm_field_group_context_attach(unit, 0, ingress_fg_id, context_id, &attach_info);
    if(rv != BCM_E_NONE) {
        printf("Error in bcm_field_group_context_attach fg_id %d context_id %d\n", ingress_fg_id, context_id);
        return rv;
    }
    return rv;  
}


/*
 * FAI[3] is used for DSCP preserve enabler from ingress. It is set per in-lif-profile.
 * Note: only routing case is supported.
 */
int
qos_l3_dscp_preserve_per_inlif_config(int unit, int port, int enable)
{
    int rv;
    int is_jericho2_mode = *(dnxc_data_get(unit,  "headers", "system_headers", "system_headers_mode", NULL));

    if (!is_device_or_above(unit, JERICHO2) || !is_jericho2_mode) {
        if (is_device_or_above(unit, JERICHO2)) {
            /** If JR2, the enabler is set to rif-profile. We disguise the rif-id as a tunnel-gport*/
            BCM_GPORT_TUNNEL_ID_SET(port, BCM_L3_ITF_VAL_GET(port));
        }
        rv = bcm_port_control_set(unit, port, bcmPortControlPreserveDscpIngress, enable);
        if (rv != BCM_E_NONE) {
            printf("bcm_port_control_set bcmPortControlPreserveDscpIngress failed, port(0x08x)\n", port);
            return rv;
        }
    } else {
        int in_lif_profile = in_lif_profile_dscp_preserve_value;
    
        /** Set in-lif-profile for PMF to the port!*/
        rv = bcm_port_class_set(unit, port, bcmPortClassFieldIngressVport, in_lif_profile);
        if (rv != BCM_E_NONE) {
            printf("ERROR: in bcm_port_class_set with port 0x%08X, value %d\n", port, in_lif_profile);
            return rv;
        }

        /** Call PMF to enable ingress DSCP preserve.*/
        rv = dnx_qos_l3_dscp_preserve_per_inlif_set(unit, in_lif_profile, bcmFieldLayerTypeIp4, enable);
        if(rv != BCM_E_NONE) {
            printf("Error in dnx_qos_l3_dscp_preserve_per_inlif_set for ipv4!\n");
            return rv;
        }

        fai_update_presel_id++;

        rv = dnx_qos_l3_dscp_preserve_per_inlif_set(unit, in_lif_profile, bcmFieldLayerTypeIp6, enable);
        if(rv != BCM_E_NONE) {
            printf("Error in dnx_qos_l3_dscp_preserve_per_inlif_set for ipv6!\n");
            return rv;
        }
    }

    return rv;
}

/*
 * Config DSCP preserve enabler from ingress and egress.
 * Note: only routing case is supported.
 * Note: exposed to TCL.
 */
int
qos_l3_dscp_preserve_per_inlif_outlif_config(int unit, int ing_port, int egr_port_or_qos_profile)
{
    int rv;
    int enable = 1;
    int egr_qos_profile;
    int egr_port;

    rv = qos_l3_dscp_preserve_per_inlif_config(unit, ing_port, enable);
    if (rv != BCM_E_NONE) {
        printf("Error, qos_l3_dscp_preserve_per_inlif_set, port(0x08x)\n", ing_port);
        return rv;
    }

    if (verbose >= 2) {
        printf("%s DSCP preserve for inlif successfully!\n", (enable ? "Enable" : "Disable"));
    }

    rv = qos_l3_dscp_preserve_per_outlif_config(unit, egr_port_or_qos_profile, enable);
    if (rv != BCM_E_NONE) {
        printf("Error, qos_l3_dscp_preserve_per_outlif_config, port(0x08x)\n", egr_port_or_qos_profile);
        return rv;
    }

    if (verbose >= 2) {
        printf("%s DSCP preserve for outlif successfully!\n", (enable ? "Enable" : "Disable"));
    }

    return rv;
}

