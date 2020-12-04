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
   print qos_map_l3(unit);
   qos_map_id_multi_get(unit);
   qos_map_l3_multi_ingress_get(unit, BCM_QOS_MAP_L3|BCM_QOS_MAP_IPV4, l3_in_map_id);
   qos_map_l3_multi_ingress_get(unit, BCM_QOS_MAP_L3|BCM_QOS_MAP_IPV6, l3_in_map_id);
   qos_map_l3_multi_egress_get(unit, BCM_QOS_MAP_L3|BCM_QOS_MAP_IPV4, l3_eg_map_id);
   qos_map_l3_multi_egress_get(unit, BCM_QOS_MAP_L3|BCM_QOS_MAP_IPV6, l3_eg_map_id);
   qos_map_id_destroy(unit, l3_eg_map_id);

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
int qos_map_l3_internal_pri[16] = {0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7};

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

int qos_map_l3_remark_pri[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};

int map_ids[20];
static int map_id_cnt = 0;

static int l3_in_map_id;
static int l3_eg_map_id;

int
qos_map_l3_ingress_profile(int unit)
{
    bcm_qos_map_t l3_in_map;
    int flags = 0;
    int dscp;
    int rv = BCM_E_NONE;

    /* Clear structure */

    bcm_qos_map_t_init(&l3_in_map);
    
    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_INGRESS, &l3_in_map_id);

    if (rv != BCM_E_NONE) {
	printf("error in ingress l3 bcm_qos_map_create()\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
	return rv;
    }

    map_ids[map_id_cnt++] = l3_in_map_id;

    for (dscp=0; dscp<64; dscp++) {

	if (dscp < 32) {
	    flags = BCM_QOS_MAP_L3|BCM_QOS_MAP_IPV4;
	} else {
	    flags = BCM_QOS_MAP_L3|BCM_QOS_MAP_IPV6;
	}

	
	/* Set ingress dscp Priority */
	l3_in_map.dscp = dscp;
	
	/* Set internal priority for this ingress DSCP  */
	l3_in_map.int_pri = qos_map_l3_internal_pri[dscp>>2];
	
	/* Set color for this ingress DSCP Priority  */
	l3_in_map.color = qos_map_l3_internal_color[dscp>>2];
	
	rv = bcm_qos_map_add(unit, flags, &l3_in_map, l3_in_map_id);
	
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
qos_map_l3_egress_profile(int unit)
{
    bcm_qos_map_t l3_eg_map;
    int flags = 0;
    int dscp;
    int MAX_PRI = 64;
    int dp;
    int rv;
 
    /* Clear structure */
    bcm_qos_map_t_init(&l3_eg_map);

    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS, &l3_eg_map_id);

    if (rv != BCM_E_NONE) {
	printf("error in L3 egress bcm_qos_map_create()\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
    } else {
        map_ids[map_id_cnt++] = l3_eg_map_id;
    }

    for (dscp=0; dscp<MAX_PRI; dscp++) {
	for (dp=0; dp < 2; dp++) {
	    /* Set internal priority  */
	    l3_eg_map.int_pri = dscp;
	    
	    if (dscp < MAX_PRI/2) {
		flags = BCM_QOS_MAP_L3|BCM_QOS_MAP_IPV4;
	    } else {
		flags = BCM_QOS_MAP_L3|BCM_QOS_MAP_IPV6;
	    }

	    /* Set internal color */
	    if (dp == 0){
		l3_eg_map.color = bcmColorGreen;
	    } else {
		l3_eg_map.color = bcmColorYellow;
	    }
	
	    /* Set egress DSCP */
	    l3_eg_map.remark_int_pri = qos_map_l3_remark_pri[dscp>>2];
	
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

int
qos_map_l3(int unit)
{
    int rv = BCM_E_NONE;

    rv = qos_map_l3_ingress_profile(unit);
    if (rv) {
	printf("error setting up ingress qos profile\n");
	return rv;
    }

    rv = qos_map_l3_egress_profile(unit);

    if (rv) {
	printf("error setting up egress qos profile\n");
	return rv;
    }
    return rv;
}	

/* Return the map id set up for egress  */
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

int qos_l3_delete_all_maps(int unit, int map_id) {
    int rv = BCM_E_NONE;
    bcm_qos_map_t l3_in_map;
    int dscp = 0;

    /* Clear structure */

    bcm_qos_map_t_init(&l3_in_map);
    
    for (dscp = 0; dscp < 64; dscp++) {

	l3_in_map.dscp = dscp;

	rv = bcm_qos_map_delete(unit, BCM_QOS_MAP_L3|BCM_QOS_MAP_IPV4, l3_in_map, map_id);
	
	if (rv != BCM_E_NONE) {
	    printf("error in l3 ingress IPV4 bcm_qos_map_delete()\n");
	    printf("rv is: $rv \n");
	    printf("(%s)\n", bcm_errmsg(rv));
	    return rv;
	}

	rv = bcm_qos_map_delete(unit, BCM_QOS_MAP_L3|BCM_QOS_MAP_IPV6, l3_in_map, map_id);
	
	if (rv != BCM_E_NONE) {
	    printf("error in l3 ingress IPV6 bcm_qos_map_delete()\n");
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


void print_l3_ingress_map(bcm_qos_map_t *map) {
    char color[8];

    get_color(map->color,&color);
    printf("dscp: %02d\t int_pri: %d\t color: %s\n",
	   map->dscp, map->int_pri, color);
}

void print_l3_egress_map(bcm_qos_map_t *map) {
    char color[8], remark_color[8];

    get_color(map->color,&color);
    get_color(map->remark_color, &remark_color);
    printf("int_pri: %d\t int color: %s\t remark_pri: %02d\n",
	   map->int_pri, color, map->remark_int_pri);
}

int
qos_map_l3_multi_egress_get(int unit, int flags, int map_id)
{
   bcm_qos_map_t *l3_map;
   int rv = 0, array_size = 0, array_count = 0;
   int i = 0;

   rv = bcm_qos_map_multi_get(unit, flags, map_id, array_size, NULL, &array_count);

   if (rv) {
       printf("Error getting multi map count $rv\n");
       return rv;
   }

   l3_map = sal_alloc(256 * array_count, "l3 maps");

   if (l3_map == NULL) {
       printf("Error allocating l3_map buffer\n");
       return rv;
   }

   array_size = array_count/2;

   rv = bcm_qos_map_multi_get(unit, flags, map_id, array_size, l3_map, &array_count);
   if (rv) {
       printf("Error getting qos maps\n");
       sal_free (l3_map);
       return rv;
   }

   printf("array count(%d)\n", array_count);
   printf("l3 map id (0x%x)\n", map_id);

   for(i=0; i<array_count; i++) {
       print_l3_egress_map(l3_map[i]);
   }

   sal_free(l3_map);
   return rv;
}


int
qos_map_l3_multi_ingress_get(int unit, int flags, int map_id)
{
   bcm_qos_map_t *l3_map;
   int rv = 0, array_size = 0, array_count = 0;
   int i = 0;

   rv = bcm_qos_map_multi_get(unit, flags, map_id, array_size, NULL, &array_count);

   if (rv) {
       printf("Error getting multi map count $rv\n");
       return rv;
   }

   l3_map = sal_alloc(256 * array_count, "l3 maps");

   if (l3_map == NULL) {
       printf("Error allocating l3_map buffer\n");
       return rv;
   }

   array_size = array_count;

   rv = bcm_qos_map_multi_get(unit, flags, map_id, array_size, l3_map, &array_count);
   if (rv) {
       printf("Error getting qos maps\n");
       sal_free (l3_map);
       return rv;
   }

   printf("array count(%d)\n", array_count);
   printf("l3 map id (0x%x)\n", map_id);

   for(i=0; i<array_count; i++) {
       print_l3_ingress_map(l3_map[i]);
   }

   sal_free(l3_map);
   return rv;
}


int qos_decode_flags(int flags)
{
    if (flags & BCM_QOS_MAP_L2)
	printf("\tBCM_QOS_MAP_L2\n");
    if (flags & BCM_QOS_MAP_L2_OUTER_TAG)
	printf("\tBCM_QOS_MAP_L2_OUTER_TAG\n");
    if (flags & BCM_QOS_MAP_L2_INNER_TAG)
	printf("\tBCM_QOS_MAP_L2_INNER_TAG\n");
    if (flags & BCM_QOS_MAP_L2_UNTAGGED)
	printf("\tBCM_QOS_MAP_L2_UNTAGGED\n");
    if (flags & BCM_QOS_MAP_L2_VLAN_PCP)
	printf("\tBCM_QOS_MAP_L2_VLAN_PCP\n");
    if (flags & BCM_QOS_MAP_L3)
	printf("\tBCM_QOS_MAP_L3\n");
    if (flags & BCM_QOS_MAP_IPV4)
	printf("\tBCM_QOS_MAP_IPV4\n");
    if (flags & BCM_QOS_MAP_IPV6)
	printf("\tBCM_QOS_MAP_IPV6\n");
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
    
