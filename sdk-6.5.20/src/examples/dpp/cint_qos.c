/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 *
 * Cint QOS Setup example code for MPLS
 * In the CINT example we present 2 different functions of QOS in MPLS world:
 * 1. Mapping from Incoming MPLS-EXP label to Internal TC+DP. It will allow the user to set the traffic
 *    management COS according to MPLS label.
 * 2. Mapping from Internal TC-DP to Outgoing MPLS label. Allow to signal Traffic management COS into the
 *    MPLS label.
 *
 * copy to /usr/local/sbin location, run bcm.user
 *
 * Run script:
 * 
 * cint ../../../../src/examples/dpp/cint_port_tpid.c
 * cint ../../../../src/examples/dpp/cint_qos.c
 * cint ../../../../src/examples/dpp/utility/cint_utils_l3.c   
 * cint ../../../../src/examples/dpp/cint_mpls_lsr.c
 * cint ../../../../src/examples/dpp/cint_multi_device_utils.c
 * c
 * int inP = 201;
 * int outP = 202;
 * qos_run(unit,inP,outP); 
 * 
 * Traffic example, MPLS LSR:
 *  The formation of the packet and the received packet are determined by the activation of qos_run. 
 *  Ethernet DA: 00:00:00:00:00:11 , SA: 00:00:63:DE:1A:13 , VLAN: TPID 0x8100 , VID 100;   MPLS: Label:5000, EXP:0
 *  Expect:
 *  MPLS LABEL 8000, EXP 2
 *  Ethernet DA 00:00:00:00:00:22, SA 00:00:00:00:00:11
 *
 * Please note the the in-EXP can be substituted by any of the values in the following table.
 * Expect the matching out-EXP.
 */
 
   /* Map ingress and egress as follows
    * Ingress EXP     INTERNAL TC/DP     Egress EXP
    *     0              0/GREEN         2
    *     1              0/YELLOW        2
    *     2              1/GREEN         3
    *     3              1/YELLOW        4
    *     4              2/GREEN         5
    *     5              2/RED           6
    *     6              3/YELLOW        7
    *     7              3/RED           7
    */
int qos_map_mpls_internal_pri[8] = {0, 0, 1, 1, 2, 2, 3, 3};

bcm_color_t qos_map_mpls_internal_color[8] = {bcmColorGreen,
					      bcmColorYellow,
					      bcmColorGreen,
					      bcmColorYellow,
					      bcmColorGreen,
					      bcmColorRed,
					      bcmColorYellow,
					      bcmColorRed};

int qos_map_mpls_remark_pri[8] = {2, 2, 3, 4, 5, 6, 7, 7};

int map_ids[20];
static int map_id_cnt = 0;

static int mpls_in_map_id;
static int mpls_eg_map_id;
static int eg_remark_id;

/*  Map: Incoming-EXP (exp) -> TC (int_pri), DP (color)*/
int
qos_map_mpls_ingress_profile(int unit)
{
    bcm_qos_map_t mpls_in_map, l3_ing_map;
    int flags = 0;
    int mpls_exp;
    int rv = BCM_E_NONE;

    /* Clear structure */

    bcm_qos_map_t_init(&mpls_in_map);
    
    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_INGRESS, &mpls_in_map_id);

    if (rv != BCM_E_NONE) {
	printf("error in ingress mpls bcm_qos_map_create()\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
	return rv;
    }

    map_ids[map_id_cnt++] = mpls_in_map_id;

    flags = BCM_QOS_MAP_MPLS;

    for (mpls_exp=0; mpls_exp<8; mpls_exp++) {
      	
        /* 
         * Ingress COS profile Maps:
         * Incoming-EXP (exp) -> TC (int_pri), DP (color)
         */
      	mpls_in_map.exp = mpls_exp;
      	mpls_in_map.int_pri = qos_map_mpls_internal_pri[mpls_exp];
      	mpls_in_map.color = qos_map_mpls_internal_color[mpls_exp];

        if (is_device_or_above(unit, ARAD)) {
            /* Incoming-EXP (exp) -> In-DSCP-EXP (remark_int_pri) */
            mpls_in_map.remark_int_pri = mpls_exp;
        }
      	
      	rv = bcm_qos_map_add(unit, flags, &mpls_in_map, mpls_in_map_id);
      	
      	if (rv != BCM_E_NONE) {
      	    printf("error in mpls ingress bcm_qos_map_add()\n");
      	    printf("rv is: $rv \n");
      	    printf("(%s)\n", bcm_errmsg(rv));
      	    return rv;
      	}

        bcm_qos_map_t_init(&l3_ing_map);
        l3_ing_map.int_pri = mpls_exp; /* TC */
        l3_ing_map.color = qos_map_mpls_internal_color[mpls_exp]; /* DP */
        l3_ing_map.inherited_dscp_exp = 1;
        l3_ing_map.exp = mpls_exp;
        l3_ing_map.dscp = 100;
        l3_ing_map.remark_int_pri = 100; /* packet DSCP (TOS) */
        rv = bcm_qos_map_add(unit, BCM_QOS_MAP_L3|BCM_QOS_MAP_IPV4, &l3_ing_map, mpls_in_map_id);
        if (rv != BCM_E_NONE) {
            printf("Error, set QoS mapping, bcm_qos_map_add, rv is:%s\n", bcm_errmsg(rv));
            return rv;
        }
    }
    return rv;
}


/* MAp: COLOR, INT_PRI(IN-DSCP_EXP) -> remark_int_pri (Out-DSCP-EXP)*/
int
qos_map_mpls_egress_profile(int unit)
{
    bcm_qos_map_t mpls_eg_map;
    int flags = 0;
    int mpls_exp;
    int rv;
    bcm_color_t color;
    int exp = 0;
    int is_policer_color = soc_property_get(unit , "policer_color_resolution_mode",0);

    /* Clear structure */
    bcm_qos_map_t_init(&mpls_eg_map);

    flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_WITH_ID;
    if (is_device_or_above(unit, JERICHO)) {
        mpls_eg_map_id = 7;
    } else if (is_device_or_above(unit, ARAD)) {
        mpls_eg_map_id = 3;
    }

    rv = bcm_qos_map_create(unit, flags, &mpls_eg_map_id);

    if (rv != BCM_E_NONE) {
        printf("error in mpls egress bcm_qos_map_create()\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
    }

    map_ids[map_id_cnt++] = mpls_eg_map_id;

    flags = BCM_QOS_MAP_MPLS;

    /* COLOR, INT_PRI(IN-DSCP_EXP) -> remark_int_pri (Out-DSCP-EXP) 
    */ 
    for (exp=0; exp<8; exp++) {
        mpls_eg_map.color = qos_map_mpls_internal_color[exp]; 
        if(is_policer_color && mpls_eg_map.color == bcmColorRed){
            mpls_eg_map.color = bcmColorYellow;
        }
        if (!is_device_or_above(unit, ARAD)) {
            /* In PetraB the color is one bit. */
            mpls_eg_map.color = mpls_eg_map.color & 1;
        }
        mpls_eg_map.int_pri = exp;
        mpls_eg_map.remark_int_pri = qos_map_mpls_remark_pri[exp];
        rv = bcm_qos_map_add(unit, flags, &mpls_eg_map, mpls_eg_map_id);
    	
        if (rv != BCM_E_NONE) {
    	      printf("error in mpls egress bcm_qos_map_add()\n");
            printf("rv is: $rv \n");
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
        }
    }
    
    eg_remark_id = mpls_eg_map_id;
    printf("mpls_eg_map_id is: $mpls_eg_map_id \n");

    return rv;
}


/* perform full mapping of: Incoming-EXP -> Outgoing-EXP */
int
qos_map_mpls(int unit)
{
    int rv = BCM_E_NONE;

    rv = qos_map_mpls_ingress_profile(unit);
    if (rv) {
	printf("error setting up ingress qos profile\n");
	return rv;
    }

    rv = qos_map_mpls_egress_profile(unit);
    if (rv) {
	printf("error setting up egress qos profile\n");
	return rv;
    }
    return rv;
}	


/* Return the map id set up for egress - called from cint_mpls_lsr.c */
/* to set up tunnel.                                                 */
int
qos_map_id_mpls_egress_get(int unit)
{
  return mpls_eg_map_id;
}

int
qos_map_id_mpls_ingress_get(int unit)
{
  return mpls_in_map_id;
}

int qos_mpls_delete_all_maps(int unit, int map_id) {
    int rv = BCM_E_NONE;
    bcm_qos_map_t mpls_in_map;
    int flags = BCM_QOS_MAP_MPLS;
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


/* Utility function: printing the MPLS QOS mapping instance of this file*/
void print_mpls_map(bcm_qos_map_t *map) {
    char color[8], remark_color[8];

    get_color(map->color,&color);
    get_color(map->remark_color, &remark_color);
    printf("pkt_pri: %d\tint_pri: %d\npkt_cfi: %d\tdscp: %d\texp: %d\n",
	   map->pkt_pri, map->int_pri, map->pkt_cfi, map->dscp, map->exp);
    printf("color: %s\tremark pri: %d\nremark color: %s\npolicer offset: %d\n",
	   color, map->remark_int_pri, remark_color, map->policer_offset);
}


/* Dump MPLS QOS mapping information relevant for this file's configurations
*/
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

   mpls_map = sal_alloc(array_count * 100 /* 100 is actually enough size to imitate sizeof(bcm_qos_map_t) */, "mpls maps");

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


/* Decode flags and print their type */
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

/* Main function of this Cint:
   * Perform full mapping of Incoming-EXP -> Outgoing-EXP according to the above specifications
   * Dump MPLS QOS mapping information relevant for this file 
   * Activate the main function of the mpls application 
   * Attach the QOS profile to the inRIF 
*/
int
qos_run(int unit, int inPort, int outPort)
{
    bcm_l3_ingress_t l3i;
    int rv;
    int in_vlan = 100;

    /*
    port_tpid_init(inPort, 1, 0, 0x8100);
    port_tpid_init(outPort, 1, 0, 0x8100);
    port_tpid_set(0);
    */
    qos_map_mpls(unit);
    qos_map_mpls_multi_get(unit, BCM_QOS_MAP_MPLS, map_ids[0]);     
    mpls_lsr_init(outPort,outPort, 0x11, 0x22, 5000, 8000, in_vlan, 200,0);
    /*
    mpls_lsr_init(second_unit, out_port, out_port, 0x11, 0x22, 5000, 8000, 100,200,0);
    */
    mpls_lsr_config(&unit, 1, 0);

    /* Get the configuration of the InRIF. */
    rv = bcm_l3_ingress_get(unit, in_vlan, &l3i);
    if (rv != BCM_E_NONE) {
        printf("Error - The following call failed:\n"); 
        printf("bcm_l3_ingress_get(%d, %d, l3i)\n", unit, in_vlan);
        print l3i;
        print rv;
        return rv;
    }

    /* Attch the ingress QoS profile. */
    l3i.qos_map_id = mpls_in_map_id;
    l3i.flags |= BCM_L3_INGRESS_WITH_ID;

    /* Set the configuration of the InRIF. */
    rv = bcm_l3_ingress_create(unit, l3i, in_vlan);
    if (rv != BCM_E_NONE) {
        printf("Error - The following call failed:\n"); 
        printf("bcm_l3_ingress_create(%d, l3i, %d)\n", unit, in_vlan);
        print l3i;
        print rv;
        return rv;
    }

    return 0;
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
    
