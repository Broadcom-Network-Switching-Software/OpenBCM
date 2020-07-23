/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Cint QOS PHB example code
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/sand/cint_vswitch_metro_mp_vlan_port.c
 * cint ../../src/examples/sand/cint_qos_l2_phb.c
 * cint
 * qos_map_l2_run_with_tagged(0,201,203);
 * exit;
 *
 * Bridge QOS PHB
 * tx 1 psrc=201 data=0x0000000000010000000001009100b0648100012cffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Source port: 0, Destination port: 0
 * Data: 0x0000000000010000000001009100d25881000320ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f2000000000000000
 */

 /* Map ingress and egress as follows
    * Ingress      INTERNAL    INGRESS       Egress
    * PKT_PRI/CFI  TC/DP        NWK-QOS     PKT_PRI/CFI
    *
    *       0/0    0/GREEN         1            1/0
    *       0/1    0/RED           2            1/1
    *       1/0    1/GREEN         3            2/0
    *       1/1    1/YELLOW        4            2/1
    *       2/0    1/YELLOW        5            3/0
    *       2/1    2/YELLOW        6            3/1
    *       3/0    3/GREEN         7            4/0
    *       3/1    3/RED           7            4/1
    *       4/0    4/GREEN         5            5/0
    *       4/1    4/RED           3            5/1
    *       5/0    5/GREEN         2            6/0
    *       5/1    5/RED           3            2/0
    *       6/0    6/GREEN         4            7
    *       6/1    6/RED           5            7
    *       7/0    7/YELLOW        6            7
    *       7/1    7/RED           7            7
    */
int QOS_MAPPING_TABLE_SIZE = 16;    
int qos_map_l2_internal_pri[QOS_MAPPING_TABLE_SIZE] = {0, 0, 1, 1, 1, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7};
bcm_color_t qos_map_l2_internal_color[QOS_MAPPING_TABLE_SIZE] = {bcmColorGreen,
					     bcmColorRed,
					     bcmColorGreen,
					     bcmColorYellow,
					     bcmColorYellow,
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
					     bcmColorYellow};
bcm_color_t qos_map_l2_internal_color_egr[QOS_MAPPING_TABLE_SIZE] = {bcmColorGreen,
					     bcmColorRed,
					     bcmColorGreen,
					     bcmColorYellow,
					     bcmColorYellow,
					     bcmColorRed,
					     bcmColorGreen,
					     bcmColorRed,
					     bcmColorGreen,
					     bcmColorYellow,
					     bcmColorGreen,
					     bcmColorRed,
					     bcmColorGreen,
					     bcmColorRed,
					     bcmColorGreen,
					     bcmColorYellow};
int ing_nwk_qos_map_remark_intpri[QOS_MAPPING_TABLE_SIZE] = {2, 2, 3, 4, 5, 6, 7, 7, 5, 3, 2, 3, 4, 5, 6, 7};
int qos_map_l2_egress_pcp[QOS_MAPPING_TABLE_SIZE] = {1, 1, 2, 2, 2, 3, 4, 4, 5, 5, 6, 6, 7, 7, 0, 0};
int qos_map_l2_egress_cfi[QOS_MAPPING_TABLE_SIZE] = {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1};
int egr_nwk_qos_map_remark_intpri[QOS_MAPPING_TABLE_SIZE] = {1, 4, 5, 6, 7, 7, 6, 5, 7, 2, 3, 1, 7, 6, 5, 5};
int illegal_qos_id = 0xFFFF;

/*
 * modify the cint example mapping table.
 * modify single entry of tc and dp for a given PKT_PRI/CFI
 */
int
qos_l2_default_mapping_modify(int unit, int pkt_pri, int cfi, int tc, bcm_color_t dp)
{
    int index = (pkt_pri*2+cfi);

    if (index >= QOS_MAPPING_TABLE_SIZE)
    {
        printf("qos_l2_default_mapping_modify failed. index=%d. \n", index);
        return BCM_E_FAIL;
    }
    
    qos_map_l2_internal_pri[index] = tc;
    qos_map_l2_internal_color[index] = dp;
    return BCM_E_NONE;            
}

/*
 * Set QOS ID map for tag_type , current validated only with BCM_QOS_MAP_L2_UNTAGGED
 */
int
qos_map_ingress_profile_map(int unit,int qos_map_id, int opcode, int tag_type)
{
    bcm_qos_map_t l2_in_map;
    int flags = 0;
    int idx = 0;
    int rv = BCM_E_NONE;

    /* Clear structure */
	  bcm_qos_map_t_init(&l2_in_map);

    l2_in_map.opcode = opcode;
    flags = BCM_QOS_MAP_L2 | tag_type | BCM_QOS_MAP_OPCODE;
    rv = bcm_qos_map_add(unit, flags, &l2_in_map, qos_map_id);
    if (rv != BCM_E_NONE)
    {
        printf("error in qos_map_ingress_profile_map bcm_qos_map_add():OPCODE\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

    /* Map L2 table Out-tag */
    for (idx=0; idx<QOS_MAPPING_TABLE_SIZE; idx++) {
        bcm_qos_map_t_init(&l2_in_map);
    	flags = BCM_QOS_MAP_L2 | tag_type;

    	/* Set ingress pkt_pri/cfi Priority */
    	l2_in_map.pkt_pri = idx>>1;
    	l2_in_map.pkt_cfi = idx&1;

    	/* Set internal priority for this ingress pri */
    	l2_in_map.int_pri = qos_map_l2_internal_pri[idx];

    	/* Set color for this ingress Priority  */
    	l2_in_map.color = qos_map_l2_internal_color[idx];

        /* NWK-Qos mapping  */
    	l2_in_map.remark_int_pri = ing_nwk_qos_map_remark_intpri[idx];

        rv = bcm_qos_map_add(unit, flags, &l2_in_map, opcode);

    	if (rv != BCM_E_NONE) {
    		printf("error in PHB ingress bcm_qos_map_add()\n");
    		printf("rv is: $rv \n");
    		printf("(%s)\n", bcm_errmsg(rv));
    		return rv;
        }
    }
    return rv;
}

/*
 * Set QOS ID map for tag_type , current validated only with BCM_QOS_MAP_L2_OUTER_TAG
 */
int
qos_map_egress_profile_map(int unit, int qos_map_id, int opcode, int tag_type)
{
    bcm_qos_map_t l2_eg_map;
    int flags = 0;
    int idx;
    int rv;

    /*
     * Currently BCM API doesn't have internal map priority,
     * for now we use queue_offset field to get this value.
     */

    /* Clear structure */
    bcm_qos_map_t_init(&l2_eg_map);

    l2_eg_map.opcode = opcode;
    flags = BCM_QOS_MAP_L2 | tag_type | BCM_QOS_MAP_OPCODE;
    rv = bcm_qos_map_add(unit, flags, &l2_eg_map, qos_map_id);
    if (rv != BCM_E_NONE)
    {
        printf("error in qos_map_egress_profile_map bcm_qos_map_add():OPCODE\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }

    /*current mark mapping*/
    for (idx = 0; idx < QOS_MAPPING_TABLE_SIZE; idx++)
    {
        flags = BCM_QOS_MAP_L2 | tag_type;

        bcm_qos_map_t_init(&l2_eg_map);
        /* Set internal priority  */
        l2_eg_map.int_pri = qos_map_l2_internal_pri[idx];
        l2_eg_map.color = qos_map_l2_internal_color_egr[idx];

    	/* Set egress pri */
        l2_eg_map.pkt_pri = qos_map_l2_egress_pcp[idx];
        l2_eg_map.pkt_cfi = qos_map_l2_egress_cfi[idx];


	    rv = bcm_qos_map_add(unit, flags, &l2_eg_map, opcode);
        if (rv != BCM_E_NONE)
        {
      	    printf("error in L2 egress bcm_qos_map_add() profile(0x%08x)\n", opcode);
      	    printf("rv is: $rv \n");
      	    printf("(%s)\n", bcm_errmsg(rv));
      	    return rv;
	    }
    }

    /*inheritance to next layer mapping*/
    for (idx = 0; idx < QOS_MAPPING_TABLE_SIZE; idx++)
    {
        flags = BCM_QOS_MAP_L2 | tag_type;

        bcm_qos_map_t_init(&l2_eg_map);
        /* Set internal priority  */
        l2_eg_map.int_pri = qos_map_l2_internal_pri[idx];
        l2_eg_map.color = qos_map_l2_internal_color_egr[idx];
        l2_eg_map.remark_color = qos_map_l2_internal_color_egr[idx];

        /* NWK-QoS-Var*/
        l2_eg_map.remark_int_pri = egr_nwk_qos_map_remark_intpri[idx];

	    rv = bcm_qos_map_add(unit, flags, &l2_eg_map, qos_map_id);
        if (rv != BCM_E_NONE)
        {
      	    printf("error in L2 egress bcm_qos_map_add() profile(0x%08x)\n", qos_map_id);
      	    printf("rv is: $rv \n");
      	    printf("(%s)\n", bcm_errmsg(rv));
      	    return rv;
	    }
    }

    return rv;
}

/*
 * Main function for QOS settings
 * Create QOS ingress profile and set its mapping ETH VLAN fields to TC, DP (PHB QOS)
 * Create QOS egress profile and set its mapping TC,DP to ETH VLAN fields
 */
int
qos_map_l2_example(int unit, int ingress_qos_id, int egress_qos_id, int ingress_opcode_id, int egress_opcode_id)
{
    int rv = BCM_E_NONE;
    int qos_egress     = egress_qos_id;
    int qos_ingress    = ingress_qos_id;
    int opcode_ingress = ingress_opcode_id;
    int opcode_egress  = egress_opcode_id;
    uint32 flags = 0;

    if (ingress_qos_id != illegal_qos_id)
    {
        flags = (BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK);
        if (qos_ingress != -1)
        {
            flags |= BCM_QOS_MAP_WITH_ID;
        }
        rv = bcm_qos_map_create(unit, flags, &qos_ingress);

        if (rv != BCM_E_NONE) {
            printf("error in L3 ingress bcm_qos_map_create()\n");
            printf("rv is: $rv \n");
            printf("(%s)\n", bcm_errmsg(rv));
        }

        flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_OPCODE ;
        if (opcode_ingress != -1)
        {
            flags |= BCM_QOS_MAP_WITH_ID;
        }
        rv = bcm_qos_map_create(unit, flags, &opcode_ingress);
        if (rv != BCM_E_NONE) {
            printf("error in ingress l3 opcode bcm_qos_map_create()\n");
            printf("rv is: $rv \n");
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }

        rv = qos_map_ingress_profile_map(unit, qos_ingress, opcode_ingress, BCM_QOS_MAP_PHB | BCM_QOS_MAP_L2_OUTER_TAG);
        if (rv) {
            printf("error setting up qos_map_ingress_profile_map\n");
            return rv;
        }
        /* To configure nwk-qos
 */

        flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE ;
        opcode_ingress = ingress_opcode_id;
        if (opcode_ingress != -1)
        {
            flags |= BCM_QOS_MAP_WITH_ID;
        }
        rv = bcm_qos_map_create(unit, flags, &opcode_ingress);
        if (rv != BCM_E_NONE) {
            printf("error in ingress l3 opcode bcm_qos_map_create()\n");
            printf("rv is: $rv \n");
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }
        rv = qos_map_ingress_profile_map(unit, qos_ingress, opcode_ingress, BCM_QOS_MAP_REMARK | BCM_QOS_MAP_L2_OUTER_TAG);
        if (rv) {
            printf("error setting up qos_map_ingress_profile_map\n");
            return rv;
        }
    }

    if (egress_qos_id != illegal_qos_id)
    {
        flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK;
        if (qos_egress != -1)
        {
            flags |= BCM_QOS_MAP_WITH_ID;
        }
        if(qos_egress !=0)
        {
            rv = bcm_qos_map_create(unit, flags, &qos_egress);
        }
        else
        {
            qos_egress = ((0x2000 | 0x2) << 16) | qos_egress;
            rv = BCM_E_NONE;
        }
        if (rv != BCM_E_NONE) {
            printf("error in L3 egress bcm_qos_map_create()\n");
            printf("rv is: $rv \n");
            printf("(%s)\n", bcm_errmsg(rv));
        }

        flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE ;
        if (opcode_egress != -1)
        {
            flags |= BCM_QOS_MAP_WITH_ID;
        }
        rv = bcm_qos_map_create(unit, flags, &opcode_egress);
        if (rv != BCM_E_NONE) {
            printf("error in egress l3 opcode bcm_qos_map_create()\n");
            printf("rv is: $rv \n");
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }

        rv = qos_map_egress_profile_map(unit, qos_egress, opcode_egress, BCM_QOS_MAP_REMARK | BCM_QOS_MAP_L2_OUTER_TAG);
        if (rv) {
            printf("error setting up qos_map_l2_egress_profile\n");
            return rv;
        }
    }
    return rv;
}

/*
 * Connect GPORT (VLAN-Port) to its QOS-profile
 */
int
qos_map_gport(int unit, bcm_gport_t gport, int ingress_qos_id, int egress_qos_id)
{
    int rv = BCM_E_NONE;

	rv = bcm_qos_port_map_set(unit, gport, ingress_qos_id, egress_qos_id);
	if (rv) {
	    printf("error setting up gport(0x%08x) to map\n", gport);
	    return rv;
	}

    return rv;
}

/*
 * test profile creation functionality,
 * Connect GPORT (VLAN-Port) to its QOS-profile
 * go thoroug all valid profile values
 * test configure a profile with id functionality
 * use unsupported flags for negative tests
 * try to configure a unvalid profile id
 */
int
qos_profile_creation_test(int unit)
{
    const int NUM_OF_PHB_PROFILES = 1024;
    int rv = BCM_E_NONE;
    int qos_map_id;
    int ingress_qos_map_id = 4;
    int ingress_qos_map_ids_received[NUM_OF_PHB_PROFILES];
    int i;


    /** configuring one profile with ID   */
  	rv = bcm_qos_map_create(unit, BCM_QOS_MAP_INGRESS|BCM_QOS_MAP_WITH_ID, &ingress_qos_map_id);
  	if (rv) {
  	    printf("error failed allocate ingress static profile\n");
  	    return rv;
  	}

    ingress_qos_map_ids_received[0] = ingress_qos_map_id;

    /** trying to configure all other ingress available profiles */
    for (i = 1; i < NUM_OF_PHB_PROFILES; i++)
    {
        qos_map_id = 0;
        rv = bcm_qos_map_create(unit, BCM_QOS_MAP_INGRESS, &qos_map_id);
        if (rv) {
            printf("error failed allocate profile\n");
            return rv;
        }

        if (ingress_qos_map_id == qos_map_id)
        {
            printf("error allocated the same value for static profile and dynamic profile\n");
            return -1;
        }

        ingress_qos_map_ids_received[i] = qos_map_id;
    }

    /** trying to configure profiles exceed to number of legal
     *  profiles */
    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_INGRESS|BCM_QOS_MAP_WITH_ID, &ingress_qos_map_id);
    if (!rv) {
        printf("allocate one profile more than legal value\n");
        return rv;
    }

    printf("finish creating profiles\n");

    for (i = 0; i < NUM_OF_PHB_PROFILES; i++)
    {
        rv = bcm_qos_map_destroy(unit, ingress_qos_map_ids_received[i]);
        if (rv) {
            printf("error failed allocate profile\n");
            return rv;
        }
    }

    printf("finish deleting profiles\n");

    printf("Test PASSED\n");

    return rv;
}

/*
 * test profile creation functionality,
 * Connect GPORT (VLAN-Port) to its QOS-profile
 * go thoroug all valid profile values
 * test configure a profile with id functionality
 * use unsupported flags for negative tests
 * try to configure a unvalid profile id
 */
int
qos_egress_profile_creation_test(int unit)
{
    const int NUM_OF_EGRESS_PROFILES = 32;
    int rv = BCM_E_NONE;
    int qos_map_id;
    int egress_qos_map_id  = 7;
    int egress_qos_map_ids_received[NUM_OF_EGRESS_PROFILES];
    int i;


    /** configuring one profile with ID   */
  	rv = bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS|BCM_QOS_MAP_WITH_ID, &egress_qos_map_id);
  	if (rv) {
  	    printf("error failed allocate ingress static profile\n");
  	    return rv;
  	}

    egress_qos_map_ids_received[0]  = egress_qos_map_id;

    /** trying to configure all other egress available profiles */
    for (i = 1; i < NUM_OF_EGRESS_PROFILES; i++)
    {
        qos_map_id = 0;
        rv = bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS, &qos_map_id);
        if (rv) {
            printf("error failed allocate egress profile\n");
            return rv;
        }

        if (egress_qos_map_id == qos_map_id)
        {
            printf("error allocated the same value for static profile and dynamic profile\n");
            return -1;
        }

        egress_qos_map_ids_received[i] = qos_map_id;
    }

    /** trying to configure profiles exceed to number of legal
     *  profiles */
    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS|BCM_QOS_MAP_WITH_ID, &egress_qos_map_id);
    if (!rv) {
        printf("allocate one profile more than legal value\n");
        return rv;
    }

    printf("finish creating profiles\n");

    for (i = 0; i < NUM_OF_EGRESS_PROFILES; i++)
    {
        rv = bcm_qos_map_destroy(unit, egress_qos_map_ids_received[i]);
        if (rv) {
            printf("error failed allocate profile\n");
            return rv;
        }
    }

    printf("finish deleting profiles\n");

    printf("Test PASSED\n");

    return rv;
}

