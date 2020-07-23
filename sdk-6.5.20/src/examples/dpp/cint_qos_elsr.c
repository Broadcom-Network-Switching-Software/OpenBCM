/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 *
 * Cint QOS Setup example code
 *
 * copy to /usr/local/sbin location, run bcm.user
 *
 *  Run script:
 *
 *  cint cint_qos_elsr.c
 *  cint
 *  print qos_map_elsr(0);
 */
 
   /* Map ingress and egress as follows
    * Ingress EXP     INTERNAL PRIO
    *     0                             0
    *     1                             0
    *     2                             1
    *     3                             1
    *     4                             2
    *     5                             2
    *     6                             3
    *     7                             3
    */
int qos_map_mpls_internal_pri[8] = {0, 0, 1, 1, 2, 2, 3, 3};

int map_ids[20];
static int map_id_cnt = 0;

static int mpls_in_map_id;

int
qos_map_mpls_ingress_profile(int unit)
{
    bcm_qos_map_t mpls_in_map;
    int flags = 0;
    int mpls_exp;
    int rv = BCM_E_NONE;

    /* Clear structure */

    bcm_qos_map_t_init(&mpls_in_map);
    
    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_INGRESS|BCM_QOS_MAP_MPLS_ELSP, &mpls_in_map_id);

    if (rv != BCM_E_NONE) {
	printf("error in ingress mpls bcm_qos_map_create()\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
	return rv;
    }

    map_ids[map_id_cnt++] = mpls_in_map_id;

    flags = 0;

    for (mpls_exp=0; mpls_exp<8; mpls_exp++) {
	
	/* Set ingress EXP Priority */
	mpls_in_map.exp = mpls_exp;
	
	/* Set internal priority for this ingress EXP Priority  */
	mpls_in_map.int_pri = qos_map_mpls_internal_pri[mpls_exp];
	
	rv = bcm_qos_map_add(unit, flags, &mpls_in_map, mpls_in_map_id);
	
	if (rv != BCM_E_NONE) {
	    printf("error in mpls ingress bcm_qos_map_add()\n");
	    printf("rv is: $rv \n");
	    printf("(%s)\n", bcm_errmsg(rv));
	    return rv;
	}
    }
    return rv;
}

int
qos_map_elsr(int unit)
{
    int rv = BCM_E_NONE;

    rv = qos_map_mpls_ingress_profile(unit);
    if (rv) {
	printf("error setting up ingress qos profile\n");
	return rv;
    }

    return rv;
}	


int
qos_map_id_mpls_ingress_get(int unit)
{
  return mpls_in_map_id;
}

int qos_mpls_delete_all_maps(int unit, int map_id) {
    int rv = BCM_E_NONE;
    bcm_qos_map_t mpls_in_map;
    int flags = BCM_QOS_MAP_MPLS_ELSP;
    int mpls_exp = 0;

    /* Clear structure */

    bcm_qos_map_t_init(&mpls_in_map);
    
    for (mpls_exp = 0; mpls_exp < 8; mpls_exp++) {

	mpls_in_map.exp = mpls_exp;

	rv = bcm_qos_map_delete(unit, flags, mpls_in_map, map_id);
	
	if (rv != BCM_E_NONE) {
	    printf("error in mpls ingress bcm_qos_map_delete()\n");
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


void print_mpls_map(bcm_qos_map_t *map) {

    printf("exp: %d\tint_pri: %d\n",
	   map->exp, map->int_pri);
}

int
qos_map_mpls_multi_get(int unit, int flags, int map_id)
{
   bcm_qos_map_t *mpls_map;
   int rv = 0, array_size = 0, array_count = 0;
   int i = 0;

   rv = bcm_qos_map_multi_get(unit, flags, map_id, array_size, NULL, &array_count);

   if (rv) {
       printf("Error getting multi map count\n");
       return rv;
   }

   mpls_map = sal_alloc(256 * array_count, "mpls maps");

   if (mpls_map == NULL) {
       printf("Error allocating mpls_map buffer\n");
       return rv;
   }

   array_size = array_count;

   rv = bcm_qos_map_multi_get(unit, flags, map_id, array_size, mpls_map, &array_count);
   if (rv) {
       printf("Error getting qos maps\n");
       sal_free (mpls_map);
       return rv;
   }

   for(i=0; i<array_count; i++) {
       print_mpls_map(mpls_map[i]);
   }

   sal_free(mpls_map);
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
    if (flags & BCM_QOS_MAP_MPLS_ELSP)
	printf("\BCM_QOS_MAP_MPLS_ELSP\n");
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
    
