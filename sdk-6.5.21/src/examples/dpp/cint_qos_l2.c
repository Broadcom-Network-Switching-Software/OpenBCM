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
 */

 
   /* Map ingress and egress as follows
    * Ingress      INTERNAL        Egress 
    * PKT_PRI/CFI  TC/DP           PKT_PRI/CFI

    *       0/0    0/GREEN               1/0
    *       0/1    0/YELLOW              1/1
    *       1/0    1/GREEN               2/0
    *       1/1    1/YELLOW              2/1
    *       2/0    2/GREEN               3/0
    *       2/1    2/YELLOW              3/1
    *       3/0    3/GREEN               4/0
    *       3/1    3/RED                 4/1
    *       4/0    4/GREEN               5/0 
    *       4/1    4/RED                 5/1
    *       5/0    5/GREEN               6/0
    *       5/1    5/RED                 6/1
    *       6/0    6/GREEN               7
    *       6/1    6/RED                 7
    *       7/0    7/YELLOW              7
    *       7/1    7/RED                 7
    */
int qos_map_l2_internal_pri[16] = {0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7};

bcm_color_t qos_map_l2_internal_color[16] = {bcmColorGreen,
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

int qos_map_l2_egress_pcp[16] = {1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 7, 7};
int qos_map_l2_egress_cfi[16] = {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1}; 

int map_ids[20];
static int map_id_cnt = 0;

static int l2_in_map_id = -1;
static int pcp_in_map_id = -1;
static int l2_eg_map_id = -1;

int
qos_map_l2_ingress_profile(int unit, int tag_type, int vlan_if_set_else_lif_cos)
{
    bcm_qos_map_t l2_in_map;
    int flags = 0;
    int idx = 0;
    int rv = BCM_E_NONE;

   /* Flags only used for vlan pcp table */
    if (vlan_if_set_else_lif_cos) {
	if ((tag_type != BCM_QOS_MAP_L2_UNTAGGED) &&
	    (tag_type != BCM_QOS_MAP_L2_OUTER_TAG) &&
	    (tag_type != BCM_QOS_MAP_L2_INNER_TAG)) {
	    printf("tag type invalid\n");
	    return -1;
	}
    }

    /* Clear structure */
    if (vlan_if_set_else_lif_cos == 0) {     

	bcm_qos_map_t_init(&l2_in_map);

	rv = bcm_qos_map_create(unit, BCM_QOS_MAP_INGRESS, &l2_in_map_id);

	if (rv != BCM_E_NONE) {
	    printf("error in ingress l2 bcm_qos_map_create()\n");
	    printf("rv is: $rv \n");
	    printf("(%s)\n", bcm_errmsg(rv));
	    return rv;
	}

	map_ids[map_id_cnt++] = l2_in_map_id;

    } else {

	bcm_qos_map_t_init(&l2_in_map);

	rv = bcm_qos_map_create(unit, BCM_QOS_MAP_INGRESS|BCM_QOS_MAP_L2_VLAN_PCP, &pcp_in_map_id);

	if (rv != BCM_E_NONE) {
	    printf("error in ingress PCP bcm_qos_map_create()\n");
	    printf("rv is: $rv \n");
	    printf("(%s)\n", bcm_errmsg(rv));
	    return rv;
	}
	
	map_ids[map_id_cnt++] = pcp_in_map_id;
    }

    for (idx=0; idx<16; idx++) {
      
	flags = BCM_QOS_MAP_L2|tag_type;
	
	/* Set ingress pkt_pri/cfi Priority */
	l2_in_map.pkt_pri = idx>>1;
	l2_in_map.pkt_cfi = idx&1;
	
	/* Set internal priority for this ingress pri */
	l2_in_map.int_pri = qos_map_l2_internal_pri[idx];
	
	/* Set color for this ingress Priority  */
	l2_in_map.color = qos_map_l2_internal_color[idx];
	

	if (vlan_if_set_else_lif_cos == 0) { 
	    rv = bcm_qos_map_add(unit, flags, &l2_in_map, l2_in_map_id);
	
	    if (rv != BCM_E_NONE) {
		printf("error in L2 ingress bcm_qos_map_add()\n");
		printf("rv is: $rv \n");
		printf("(%s)\n", bcm_errmsg(rv));
		return rv;
	    }
	} else {
	    rv = bcm_qos_map_add(unit, flags | BCM_QOS_MAP_L2_VLAN_PCP, &l2_in_map, pcp_in_map_id);
	    
	    if (rv != BCM_E_NONE) {
		printf("error in PCP ingress bcm_qos_map_add()\n");
		printf("rv is: $rv \n");
		printf("(%s)\n", bcm_errmsg(rv));
		return rv;
	    }
	}
    }
    return rv;
}

int
qos_map_l2_egress_profile(int unit, int tag_type)
{
    bcm_qos_map_t l2_eg_map;
    int flags = 0;
    int idx;
    int rv;
 
	if ((tag_type != BCM_QOS_MAP_L2_UNTAGGED) &&
	    (tag_type != BCM_QOS_MAP_L2_OUTER_TAG) &&
	    (tag_type != BCM_QOS_MAP_L2_INNER_TAG) &&
	    (tag_type != BCM_QOS_MAP_MIM_ITAG)) {
	    printf("tag type invalid\n");
	    return -1;
	}

    /* Clear structure */
    bcm_qos_map_t_init(&l2_eg_map);

    if (tag_type == BCM_QOS_MAP_MIM_ITAG) {
	rv = bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_MIM_ITAG, &l2_eg_map_id);

	if (rv != BCM_E_NONE) {
	    printf("error in L2 egress bcm_qos_map_create() for mim itag\n");
	    printf("rv is: $rv \n");
	    printf("(%s)\n", bcm_errmsg(rv));
	}

    } else {
	rv = bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_L2_VLAN_PCP, &l2_eg_map_id);

	if (rv != BCM_E_NONE) {
	    printf("error in L2 egress bcm_qos_map_create() for egress pcp vlan\n");
	    printf("rv is: $rv \n");
	    printf("(%s)\n", bcm_errmsg(rv));
	}
    }
    map_ids[map_id_cnt++] = l2_eg_map_id;


    for (idx=0; idx<16; idx++) {
	
	/* Set internal priority  */
	l2_eg_map.int_pri = qos_map_l2_internal_pri[idx];

	
	flags = BCM_QOS_MAP_L2|tag_type|BCM_QOS_MAP_L2_VLAN_PCP;


	/* Set internal color */
	if (qos_map_l2_internal_color[idx] == bcmColorGreen){
	    l2_eg_map.color = bcmColorGreen;
	} else {
	    l2_eg_map.color = bcmColorYellow;
	}
	
	/* Set egress pri */
        l2_eg_map.pkt_pri = qos_map_l2_egress_pcp[idx];
        l2_eg_map.pkt_cfi = qos_map_l2_egress_cfi[idx];
	
	rv = bcm_qos_map_add(unit, flags, &l2_eg_map, l2_eg_map_id);
	
	if (rv != BCM_E_NONE) {
	    printf("error in L2 egress bcm_qos_map_add() profile(0x%08x)\n", l2_eg_map_id);
	    printf("rv is: $rv \n");
	    printf("(%s)\n", bcm_errmsg(rv));
	    return rv;
	}
    }
    return rv;
}


int
qos_map_l2_untagged(int unit)
{
    int rv = BCM_E_NONE;

    rv = qos_map_l2_ingress_profile(unit, BCM_QOS_MAP_L2_UNTAGGED, 1);
    if (rv) {
	printf("error setting up ingress qos profile\n");
	return rv;
    }

    rv = qos_map_l2_egress_profile(unit, BCM_QOS_MAP_L2_UNTAGGED);
    if (rv) {
	printf("error setting up egress qos profile\n");
	return rv;
    }

    return rv;
}

int
qos_map_l2_inner(int unit)
{
    int rv = BCM_E_NONE;

    rv = qos_map_l2_ingress_profile(unit, BCM_QOS_MAP_L2_INNER_TAG, 1);
    if (rv) {
	printf("error setting up ingress qos profile\n");
	return rv;
    }

    rv = qos_map_l2_egress_profile(unit, BCM_QOS_MAP_L2_INNER_TAG);
    if (rv) {
	printf("error setting up egress qos profile\n");
	return rv;
    }

    return rv;
}

int
qos_map_l2_outer(int unit)
{
    int rv = BCM_E_NONE;

    /* Map outer tag for PCP vlan case */
    rv = qos_map_l2_ingress_profile(unit, BCM_QOS_MAP_L2_OUTER_TAG, 1);
    if (rv) {
	printf("error setting up ingress qos profile\n");
	return rv;
    }

    rv = qos_map_l2_egress_profile(unit, BCM_QOS_MAP_L2_OUTER_TAG);
    if (rv) {
	printf("error setting up egress qos profile\n");
	return rv;
    }

    return rv;
}

int
qos_map_l2_itag(int unit)
{
    int rv = BCM_E_NONE;

    /* Map egress itag */

    rv = qos_map_l2_egress_profile(unit, BCM_QOS_MAP_MIM_ITAG);
    if (rv) {
	printf("error setting up egress qos profile mim itag\n");
	return rv;
    }

    return rv;
}

int
qos_map_port_vlan(int unit, bcm_port_t port, bcm_vlan_t vlan, int vlan_if_set_else_lif_cos)
{
    int rv = BCM_E_NONE;
    int in_map_id, eg_map_id;

    if (vlan_if_set_else_lif_cos) {
	rv = bcm_qos_port_vlan_map_set(unit, port, vlan, pcp_in_map_id, l2_eg_map_id);
	if (rv) {
	    printf("error setting up port(%d) vlan(%d) to map\n", port, vlan);
	    return rv;
	}
	printf("pcp_in_map_id(0x%08x)\n", pcp_in_map_id);
	printf("l2_eg_map_id(0x%08x)\n", l2_eg_map_id);
    } else {
	rv = bcm_qos_port_vlan_map_set(unit, port, vlan, l2_in_map_id, l2_eg_map_id);
	if (rv) {
	    printf("error setting up port(%d) vlan(%d) to map\n", port, vlan);
	    return rv;
	}
	printf("l2_in_map_id(0x%08x)\n", l2_in_map_id);
	printf("l2_eg_map_id(0x%08x)\n", l2_eg_map_id);
    }

    in_map_id = -1; 
    rv = bcm_qos_port_vlan_map_get(unit, port, vlan, &in_map_id, &eg_map_id);
    if (rv) {
	printf("error getting port(%d) vlan(%d) maps\n", port, vlan);
	return rv;
    }
    printf("ingress map returned (0x%08x)\n", in_map_id);
    printf("egress map returned (0x%08x)\n", eg_map_id);

    return rv;
}	
int
qos_map_gport(int unit, bcm_gport_t gport, int vlan_if_set_else_lif_cos)
{
    int rv = BCM_E_NONE;
    int in_map_id, eg_map_id;
    
    if (vlan_if_set_else_lif_cos) {
	rv = bcm_qos_port_map_set(unit, gport, pcp_in_map_id, l2_eg_map_id);
	if (rv) {
	    printf("error setting up gport(0x%08x) to map\n", gport);
	    return rv;
	}
	printf("pcp_in_map_id(0x%08x)\n", pcp_in_map_id);
	printf("l2_eg_map_id(0x%08x)\n", l2_eg_map_id);
    } else {
	rv = bcm_qos_port_map_set(unit, gport, l2_in_map_id, l2_eg_map_id);
	if (rv) {
	    printf("error setting up gport(0x%08x) to map\n", gport);
	    return rv;
	}
	printf("l2_in_map_id(0x%08x)\n", l2_in_map_id);
	printf("l2_eg_map_id(0x%08x)\n", l2_eg_map_id);
    }
    in_map_id = -1;
    rv = bcm_qos_port_map_get(unit, gport, &in_map_id, &eg_map_id);
    if (rv) {
	printf("error getting gport(0x%08x) maps\n", gport);
	return rv;
    }
    printf("ingress map returned (0x%08x)\n", in_map_id);
    printf("egress map returned (0x%08x)\n", eg_map_id);

    return rv;
}	


/* Return the map id set up for egress  */
int
qos_map_id_l2_egress_get(int unit)
{
  return l2_eg_map_id;
}

int
qos_map_id_l2_ingress_get(int unit)
{
  return l2_in_map_id;
}

int
qos_map_id_l2_pcp_ingress_get(int unit)
{
  return pcp_in_map_id;
}

int qos_l2_delete_all_maps(int unit, int map_id) {
    int rv = BCM_E_NONE;
    bcm_qos_map_t l2_in_map;
    int idx = 0;
    int type = BCM_QOS_MAP_TYPE_GET(map_id);

    /* Clear structure */

    bcm_qos_map_t_init(&l2_in_map);
    
    for (idx = 0; idx < 16; idx++) {

	l2_in_map.pkt_pri = idx>>1;
	l2_in_map.pkt_cfi = idx&1;

	rv = bcm_qos_map_delete(unit,type | BCM_QOS_MAP_L2|BCM_QOS_MAP_L2_OUTER_TAG, l2_in_map, map_id);
	if (rv != BCM_E_NONE) {
	    printf("error in l2 ingress bcm_qos_map_delete()\n");
	    printf("rv is: $rv \n");
	    printf("(%s)\n", bcm_errmsg(rv));
	    return rv;
	}

	rv = bcm_qos_map_delete(unit,type | BCM_QOS_MAP_L2|BCM_QOS_MAP_L2_INNER_TAG, l2_in_map, map_id);
	if (rv != BCM_E_NONE) {
	    printf("error in l2 ingress bcm_qos_map_delete()\n");
	    printf("rv is: $rv \n");
	    printf("(%s)\n", bcm_errmsg(rv));
	    return rv;
	}

	rv = bcm_qos_map_delete(unit,type | BCM_QOS_MAP_L2|BCM_QOS_MAP_L2_UNTAGGED, l2_in_map, map_id);
	if (rv != BCM_E_NONE) {
	    printf("error in l2 ingress bcm_qos_map_delete()\n");
	    printf("rv is: $rv \n");
	    printf("(%s)\n", bcm_errmsg(rv));
	    return rv;
	}
    }
    return rv;
}

int qos_map_id_destroy(int unit, int map_id) 
{
    int rv;
   
    rv = bcm_qos_map_destroy(unit, map_id);
    if (rv) {
	printf("Map ID operation failed\n");
    } else {
	printf("Map ID 0x%x destroyed\n",map_id);
    }

	return rv;
}

int get_color(int color, char *str) {
    int rv = 0;
    switch (color) {
	case 0:
	    sal_strcpy(str,"Green ");
	    break;
	case 1:
	    sal_strcpy(str,"Yellow");
	    break;
	case 2:
	    sal_strcpy(str,"Red   ");
	    break;
	default:
	    sal_strcpy(str,"******");
	    rv = -1;
    }
    return rv;
}


void print_qos_map(bcm_qos_map_t *map) {
    char color[8], remark_color[8];

    get_color(map->color,&color);
    get_color(map->remark_color, &remark_color);
    printf("pkt_pri: %d\tint_pri: %d\npkt_cfi: %d\tdscp: %d\texp: %d\n",
	   map->pkt_pri, map->int_pri, map->pkt_cfi, map->dscp, map->exp);
    printf("color: %s\tremark pri: %d\nremark color: %s\npolicer offset: %d\n",
	   color, map->remark_int_pri, remark_color, map->policer_offset);
}


void print_l2_ingress_map(bcm_qos_map_t *map) {
    char color[8];

    get_color(map->color,&color);
    printf("pkt_pri: %d\t pkt_cfi: %d\t int_pri: %d\t color: %s\n",
	   map->pkt_pri, map->pkt_cfi, map->int_pri, color);
}

void print_l2_egress_map(bcm_qos_map_t *map) {
    char color[8], remark_color[8];

    get_color(map->color,&color);
    get_color(map->remark_color, &remark_color);
    printf("int_pri: %d\t int color: %s\t pkt_pri: %d pkt_cfi: %d\n",
	   map->int_pri, color, map->pkt_pri, map->pkt_cfi);
}

int
qos_map_l2_multi_egress_get(int unit, int flags, int map_id)
{
   bcm_qos_map_t *l2_map;
   int rv = 0, array_size = 0, array_count = 0;
   int i = 0;

   rv = bcm_qos_map_multi_get(unit, flags, map_id, array_size, NULL, &array_count);

   if (rv) {
       printf("Error getting multi map count $rv\n");
       return rv;
   }

   l2_map = sal_alloc(256 * array_count, "l2 maps");

   if (l2_map == NULL) {
       printf("Error allocating l2_map buffer\n");
       return rv;
   }

   array_size = array_count;

   rv = bcm_qos_map_multi_get(unit, flags, map_id, array_size, l2_map, &array_count);
   if (rv) {
       printf("Error getting qos maps\n");
       sal_free (l2_map);
       return rv;
   }

   printf("array count(%d)\n", array_count);
   printf("l2 map id (0x%x)\n", map_id);

   for(i=0; i<array_count; i++) {
       print_l2_egress_map(l2_map[i]);
   }

   sal_free(l2_map);
   return rv;
}


int
qos_map_l2_multi_ingress_get(int unit, int flags, int map_id)
{
   bcm_qos_map_t *l2_map;
   int rv = 0, array_size = 0, array_count = 0;
   int i = 0;

   rv = bcm_qos_map_multi_get(unit, flags, map_id, array_size, NULL, &array_count);

   if (rv) {
       printf("Error getting multi map count $rv\n");
       return rv;
   }

   l2_map = sal_alloc(256 * array_count, "l2 maps");

   if (l2_map == NULL) {
       printf("Error allocating l2_map buffer\n");
       return rv;
   }

   array_size = array_count;

   rv = bcm_qos_map_multi_get(unit, flags, map_id, array_size, l2_map, &array_count);
   if (rv) {
       printf("Error getting qos maps\n");
       sal_free (l2_map);
       return rv;
   }

   printf("array count(%d)\n", array_count);
   printf("l2 map id (0x%x)\n", map_id);

   for(i=0; i<array_count; i++) {
       print_l2_ingress_map(l2_map[i]);
   }

   sal_free(l2_map);
   return rv;
}


int qos_decode_flags(int flags)
{
    if (flags & BCM_QOS_MAP_L2)
	printf("\tBCM_QOS_MAP_L2\n");
    if (flags & BCM_QOS_MAP_L2_INNER_TAG)
	printf("\tBCM_QOS_MAP_L2_INNER_TAG\n");
    if (flags & BCM_QOS_MAP_L2_UNTAGGED)
	printf("\tBCM_QOS_MAP_L2_UNTAGGED\n");
    if (flags & BCM_QOS_MAP_L2_VLAN_PCP)
	printf("\tBCM_QOS_MAP_L2_VLAN_PCP\n");
    /* L2 and OUTER share same define */
    if ((flags & BCM_QOS_MAP_L2) &&
	((flags & BCM_QOS_MAP_L2_INNER_TAG) == 0) &&
	((flags & BCM_QOS_MAP_L2_UNTAGGED) == 0)) {
	printf("\tBCM_QOS_MAP_L2_OUTER_TAG\n");
    }
    if (flags & BCM_QOS_MAP_MIM_ITAG)
	printf("\tBCM_QOS_MAP_MIM_ITAG\n");
    if (flags & BCM_QOS_MAP_L3)
	printf("\tBCM_QOS_MAP_L3\n");
    if (flags & BCM_QOS_MAP_IPV6)
	printf("\tBCM_QOS_MAP_IPV6\n");
    if (flags & BCM_QOS_MAP_IPV4)
	printf("\tBCM_QOS_MAP_IPV4\n");
    if (flags & BCM_QOS_MAP_MPLS)
	printf("\tBCM_QOS_MAP_MPLS\n");
    if (flags & BCM_QOS_MAP_ENCAP)
	printf("\tBCM_QOS_MAP_ENCAP\n");
    if (flags & BCM_QOS_MAP_INGRESS)
	printf("\tBCM_QOS_MAP_INGRESS\n");
    if (flags & BCM_QOS_MAP_EGRESS)
	printf("\tBCM_QOS_MAP_EGRESS\n");
}

int 
qos_map_id_multi_get(int unit) 
{
    int array_size = 0, array_count = 0;
    int *map_ids, *flags;
    int rv = BCM_E_NONE, idx = 0;

    rv = bcm_qos_multi_get(unit, 0, 0, 0, &array_count);
    if (rv) {
	printf("Error getting qos map id count\n");
	return rv;
    }

    array_size = array_count;

    map_ids = sal_alloc(4 * array_size, "map ids");
    if (map_ids == NULL) {
	printf ("Error allocating map_id buffer\n");
	return rv;
    }
	
    flags = sal_alloc(4 * array_size, "map flags");
    if (flags == NULL) {
	printf ("Error allocating map_id flag buffer\n");
	sal_free(map_ids);
	return rv;
    }
	

    rv = bcm_qos_multi_get(unit, array_size, map_ids, flags, &array_count);
    if (rv) {
	printf("Error getting qos map ids and flags\n");
	sal_free(map_ids);
	sal_free(flags);
	return rv;
    }

    for (idx = 0; idx < array_count; idx++) {
	printf("%d: map_id: 0x%x\tflags: 0x%x\n",idx,map_ids[idx],flags[idx]);
	qos_decode_flags(flags[idx]);
    }

    sal_free(map_ids);
    sal_free(flags);
    return rv;
}

/* load 
cint cint_port_tpid.c
cint cint_vswitch_metro_p2p.c 
cint cint_qos_l2.c
to run this function.
*/
int
qos_map_l2_run_with_untagged (int unit, int port_1, int port_2){
    int rv;
	int index = 0;

 	rv = vswitch_p2p_init(unit, port_1, port_2, 100, 101, 800, 801);

	if (rv != BCM_E_NONE) {
        return rv;
    }

	rv = qos_map_l2_untagged(unit);

	if (rv != BCM_E_NONE) {
        return rv;
    }

	rv = qos_map_gport(unit, vswitch_info_1.gport1, 1);

	if (rv != BCM_E_NONE) {
        return rv;
    }
	
    rv = qos_map_gport(unit, vswitch_info_1.gport2, 1);

	if (rv != BCM_E_NONE) {
        return rv;
    }
	

    return rv;
}

/*delete l2 QoS mapping on ingress*/
int qos_l2_delete_in_maps(int unit, int tag_type, int map_id) {
    int rv = BCM_E_NONE;
    bcm_qos_map_t l2_in_map;
    int idx = 0;

    /* Clear structure */
    bcm_qos_map_t_init(&l2_in_map);
    
    for (idx = 0; idx < 16; idx++) {

        l2_in_map.pkt_pri = idx>>1;
        l2_in_map.pkt_cfi = idx&1;

        rv = bcm_qos_map_delete(unit, BCM_QOS_MAP_L2_VLAN_PCP | BCM_QOS_MAP_INGRESS | tag_type | BCM_QOS_MAP_L2, l2_in_map, map_id);
        if (rv != BCM_E_NONE) {
	        printf("error in l2 ingress bcm_qos_map_delete()\n");
	        printf("rv is: $rv \n");
	        printf("(%s)\n", bcm_errmsg(rv));
	        return rv;
        }
    }
    return rv;
}

/*delete l2 QoS mapping on egress*/
int qos_l2_delete_eg_maps(int unit, int tag_type, int map_id) {
    int rv = BCM_E_NONE;
    bcm_qos_map_t l2_eg_map;
    int idx = 0;

    /* Clear structure */
    bcm_qos_map_t_init(&l2_eg_map);

    for (idx = 0; idx < 16; idx++) {

        l2_eg_map.int_pri = qos_map_l2_internal_pri[idx];
        if (qos_map_l2_internal_color[idx] == bcmColorGreen){
	        l2_eg_map.color = bcmColorGreen;
	    } else {
	        l2_eg_map.color = bcmColorYellow;
	    }

        rv = bcm_qos_map_delete(unit, BCM_QOS_MAP_L2_VLAN_PCP | BCM_QOS_MAP_EGRESS | tag_type | BCM_QOS_MAP_L2, l2_eg_map, map_id);
        if (rv != BCM_E_NONE) {
	        printf("error in l2 egress bcm_qos_map_delete()\n");
	        printf("rv is: $rv \n");
	        printf("(%s)\n", bcm_errmsg(rv));
	        return rv;
        }
    }
    return rv;
}

/*Remove all l2 QoS mapping on untagged*/
int
qos_map_l2_delete_with_untagged (int unit){
    int rv;
	int index = 0;

    /*delete l2 untagged QoS mapping at ingress*/
    rv = qos_l2_delete_in_maps(unit, BCM_QOS_MAP_L2_UNTAGGED, map_ids[0]);
    if (rv != BCM_E_NONE) {
        printf("Error qos_l2_delete_in_maps, rv (%d) map_id (%d)\n", rv, map_ids[0]);
        return rv;
    }

	rv = qos_map_id_destroy(unit,map_ids[0]);
    if (rv != BCM_E_NONE) {
        printf("Error qos_map_id_destroy, rv (%d) map_id (%d)\n", rv, map_ids[0]);
        return rv;
    }

	/*delete l2 untagged QoS mapping at egress*/
    rv = qos_l2_delete_eg_maps(unit, BCM_QOS_MAP_L2_UNTAGGED, map_ids[1]);
    if (rv != BCM_E_NONE) {
        printf("Error qos_l2_delete_eg_maps, rv (%d) map_id (%d)\n", rv, map_ids[1]);
        return rv;
    }
	
	rv = qos_map_id_destroy(unit,map_ids[1]);
    if (rv != BCM_E_NONE) {
        printf("Error qos_map_id_destroy, rv (%d) map_id (%d)\n", rv, map_ids[1]);
        return rv;
    }

	return rv;
}


int
qos_map_l2_stop_with_untagged (int unit, int port_1, int port_2){
    int rv;
    int index = 0;

    rv = qos_map_l2_delete_with_untagged(unit);
    if (rv != BCM_E_NONE) {
        printf("Error qos_map_l2_delete_with_untagged, rv (%d)\n", rv);
        return rv;
    }
	
    map_id_cnt = 0;

    rv = vswitch_p2p_deinit(unit);
    if (rv != BCM_E_NONE) {
        printf("Error vswitch_p2p_deinit failed, rv (%d)\n", rv);
        return rv;
    }
	
    return rv;
}

/* load 
cint_mim_mp.c
cint_mim_p2p.c
cint cint_qos_l2.c
to run this function.
*/
int
qos_map_l2_run_with_untagged_mim (int unit, int port_1, int port_2){
    int rv;

    rv = p2p_main(unit, port_1, port_2);
    if (rv != BCM_E_NONE) {
        printf("Error p2p_main, rv (%d)\n", rv);		
        return rv;
    }

    rv = qos_map_l2_itag(unit);

    if (rv != BCM_E_NONE) {
        return rv;
    }

    /*apply egress QoS mapping on MiM port*/
    rv = qos_map_gport(unit, mim_p2p_info.mim_port.mim_port_id, 1);

    if (rv != BCM_E_NONE) {
        return rv;
    }
	
    return rv;
}

/* load 
cint cint_port_tpid.c
cint cint_vswitch_metro_p2p.c 
cint cint_qos_l2.c
to run this function.
*/
int
qos_map_l2_cos_all_zero_run (int unit, int port_1, int port_2){
    int rv;
    int index = 0;
    int pcp;
    int flags;
    bcm_qos_map_t l2_in_map;

    rv = vswitch_p2p_init(unit, port_1, port_2, 100, 101, 800, 801);

    if (rv != BCM_E_NONE) {
        return rv;
    }

    bcm_qos_map_t_init(&l2_in_map);

    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_INGRESS, &l2_in_map_id);
    if (rv != BCM_E_NONE) {
        printf("error in ingress l2 bcm_qos_map_create()\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
    }
    for (index=0; index<16; index++) {
        flags = BCM_QOS_MAP_L2|BCM_QOS_MAP_L2_UNTAGGED;
        /* Set ingress pkt_pri/cfi Priority */
        l2_in_map.pkt_pri = index>>1;
        l2_in_map.pkt_cfi = index&1;
        /* Set internal priority for this ingress pri */
        l2_in_map.int_pri = qos_map_l2_internal_pri[index];
        /* Set color for this ingress Priority  */
        l2_in_map.color = qos_map_l2_internal_color[index];
        rv = bcm_qos_map_add(unit, flags, &l2_in_map, l2_in_map_id);

        if (rv != BCM_E_NONE) {
            printf("error in L2 ingress bcm_qos_map_add()\n");
            return rv;
        }
    }
    /* Set color for this ingress Priority  */
    for (index=0; index<16; index++) {
        qos_map_l2_internal_pri[index]=0;
        qos_map_l2_internal_color[index]=0;
    }

    for (index=0; index<16; index++) {
        flags = BCM_QOS_MAP_L2|BCM_QOS_MAP_L2_OUTER_TAG;
        /* Set ingress pkt_pri/cfi Priority */
        l2_in_map.pkt_pri = index>>1;
        l2_in_map.pkt_cfi = index&1;
        /* Set internal priority for this ingress pri */
        l2_in_map.int_pri = qos_map_l2_internal_pri[index];
        /* Set color for this ingress Priority  */
        l2_in_map.color = qos_map_l2_internal_color[index];
        rv = bcm_qos_map_add(unit, flags, &l2_in_map, l2_in_map_id);

        if (rv != BCM_E_NONE) {
            printf("error in L2 ingress bcm_qos_map_add()\n");
            return rv;
        }
    }
	rv = qos_map_gport(unit, vswitch_info_1.gport1, 0);

	if (rv != BCM_E_NONE) {
        return rv;
    }
    rv = qos_map_gport(unit, vswitch_info_1.gport2, 0);

	if (rv != BCM_E_NONE) {
        return rv;
    }

    return rv;
}


/* load 
cint cint_port_tpid.c
cint cint_qos_l2.c
cint cint_ip_route.c 
to run this function.
*/
int
qos_map_l2_cos_mapping_pcp_dscp_mapping_pcp (int unit, int port_1, int port_2){
    int index = 0;
    int flags=0;
    bcm_qos_map_t map_entry;
    int in_map_id;
    create_l3_intf_s intf;
    int rv = BCM_E_NONE;
    bcm_l3_ingress_t l3_ing_if;
    int ing_intf_in;
	int ing_intf_out;
    int fec =0;
    int encap_id=0;
    int in_vlan = 100;
    int out_vlan = 15;
    bcm_vlan_port_t vlan_port;
    int route = 0x7fffff03;
    int mask = 0xffffffff;
    create_l3_egress_s l3_egress;
    bcm_mac_t mac_address  = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };  /* my-MAC */
    bcm_mac_t next_hop_mac  = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d }; /* next_hop_mac1 */


    bcm_port_class_set(unit, port_1, bcmPortClassId, port_1);
    bcm_port_class_set(unit, port_2, bcmPortClassId, port_2);

    /*allocate qos map id */
    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_INGRESS, &in_map_id);
    if (rv != BCM_E_NONE) {
        printf("error in ingress l2 bcm_qos_map_create()\n");
        return rv;
    }
    /*add qos maping from pcp to tc&dp*/
    bcm_qos_map_t_init(&map_entry);
    for (index=0; index<16; index++) {
        flags = BCM_QOS_MAP_L2|BCM_QOS_MAP_L2_OUTER_TAG;
        map_entry.pkt_pri = index>>1;
        map_entry.pkt_cfi = index&1;
        map_entry.int_pri = (index>>1) + 1;
        if(map_entry.int_pri > 7) {
            map_entry.int_pri =7;
        }
        map_entry.color = 0;
        rv = bcm_qos_map_add(unit, flags, &map_entry, in_map_id);

        if (rv != BCM_E_NONE) {
            printf("error in L2 ingress bcm_qos_map_add()\n");
            return rv;
        }
    }
    /*add qos L3 mapping, key use L2 pcp*/
    bcm_qos_map_t_init(&map_entry);
    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_L3_L2;
    for (index=0; index<16; index++) {
        map_entry.dscp = index;
        map_entry.remark_int_pri = (index>>1) + 2;
        rv = bcm_qos_map_add(unit, flags, &map_entry, in_map_id);

        if (rv != BCM_E_NONE) {
            printf("error in L2 ingress bcm_qos_map_add()\n");
            return rv;
        }
    }

    /*In-RIF*/
    intf.vsi = in_vlan;
    intf.my_global_mac = mac_address;
    intf.my_lsb_mac = mac_address;
    rv = l3__intf_rif__create(unit, &intf);
    ing_intf_in = intf.rif;
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
    }
    /*out RIF*/
    intf.vsi = out_vlan;
    intf.my_global_mac = mac_address;
    intf.my_lsb_mac = mac_address;
    rv = l3__intf_rif__create(unit, &intf);
    ing_intf_out = intf.rif;
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
    }
    /*In AC*/
	bcm_vlan_port_t_init(&vlan_port); 
	vlan_port.port = port_1;
	vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;	 
	vlan_port.match_vlan = in_vlan;
    vlan_port.vsi = ing_intf_in;
	rv = bcm_vlan_port_create(unit, &vlan_port); 
	if (rv < BCM_E_NONE)
	{
		printf("bcm_vlan_port_create failed \n");
		return rv;
    }
    /*set qos map id to ingress AC*/
    rv = bcm_qos_port_map_set(unit, vlan_port.vlan_port_id, in_map_id, -1);
    if (rv != BCM_E_NONE) {
        printf("Error, create egress object, out_sysport=%d, in unit %d\n", port_2, unit);
    }
    /*egress arp*/
    l3_egress.out_gport = port_2;
    l3_egress.vlan = out_vlan;
    l3_egress.next_hop_mac_addr = next_hop_mac;
    l3_egress.out_tunnel_or_rif = ing_intf_out;
    rv = l3__egress__create(unit, &l3_egress);
    if (rv != BCM_E_NONE) {
        printf("Error, create egress object, out_sysport=%d, in unit %d\n", port_2, unit);
    }
    encap_id = l3_egress.arp_encap_id;
    fec = l3_egress.fec_id;

    rv = internal_ip_route(unit, route, mask, 0, fec, 0) ;
    if (rv != BCM_E_NONE) {
        printf("Error, in function internal_ip_route, \n");
        return rv;
    }

    return rv;
}

