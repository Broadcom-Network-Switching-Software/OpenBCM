/*
 *
 * Cint QOS Setup example code for MPLS
 * In the CINT example we present 2 different functions of QOS in MPLS world:
 * 1. Mapping from Incoming MPLS-EXP label to Internal TC+DP. It will allow the user to set the traffic
 *    management COS according to MPLS label.
 * 2. Mapping from Internal nwk_qos to Outgoing MPLS label. Allow to signal nwk_qos mapping into the
 *    MPLS label.
 *
 * Run script:
 *
 * Test Scenario - start
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c
  cint ../../../../src/examples/dnx/qos/cint_dnx_mpls_qos.c
  c
  int unit = 0;
  int inP = 201;
  int outP = 202;
  dnx_qos_run(unit,inP,outP);
 * Test Scenario - end
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
 *
 */

   /*
    * Map ingress and egress as follows Ingress EXP INTERNAL TC/DP Egress EXP 0 0/GREEN 2 1 0/YELLOW 2 2 1/GREEN 3 3
    * 1/YELLOW 4 4 2/GREEN 5 5 2/RED 6 6 3/YELLOW 7 7 3/RED 7
    */
int qos_map_mpls_internal_pri[8] = { 0, 0, 1, 1, 2, 2, 3, 3 };

bcm_color_t qos_map_mpls_internal_color[8] = { bcmColorGreen,
    bcmColorYellow,
    bcmColorGreen,
    bcmColorYellow,
    bcmColorGreen,
    bcmColorYellow,
    bcmColorYellow,
    bcmColorRed
};

int qos_map_mpls_remark_pri[8] = { 2, 2, 3, 4, 5, 6, 7, 7 };
static int mpls_in_map_id = 3;
static int mpls_eg_map_id = 1;
static int l3_in_map_id = 2;
static int l3_qos_in_internal_map_id = 200;
static int l3_qos_in_internal_map_id_phb = 201;
static int l3_qos_in_internal_map_id_remark = 202;
static int mpls_qos_in_internal_map_id = 100;
static int mpls_qos_eg_internal_map_id = 10;

/*  Map: Incoming-EXP (exp) -> TC (int_pri), DP (color)*/
int
dnx_qos_map_mpls_ingress_profile(
    int unit)
{
    bcm_qos_map_t mpls_in_map, l3_ing_map;
    int flags = 0;
    int mpls_exp;
    int opcode = mpls_qos_in_internal_map_id;

    /** Clear structure */
    bcm_qos_map_t_init(&mpls_in_map);

    /** Create QOS profile */
    BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_create(unit, BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_WITH_ID,
                            &mpls_in_map_id), "");

    /** Create opcode profile */
    BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_create(unit, BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE | BCM_QOS_MAP_WITH_ID,
                            &opcode), "");

    /** Clear structure */
    bcm_qos_map_t_init(&mpls_in_map);
    mpls_in_map.opcode = opcode;
    /*
     * Call map add with BCM_QOS_MAP_OPCODE flag in order to register the internal map ID,
     * this is done once for each map ID and protocol
     */
    flags = BCM_QOS_MAP_MPLS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_add(unit, flags, &mpls_in_map, mpls_in_map_id), "REMARK OPCODE");
    for (mpls_exp = 0; mpls_exp < 8; mpls_exp++)
    {
        /** Clear structure */
        bcm_qos_map_t_init(&mpls_in_map);
        /*
         * Ingress COS profile Maps:
         * Incoming-EXP (exp) -> TC (int_pri), DP (color)
         */
        mpls_in_map.exp = mpls_exp;
        mpls_in_map.remark_int_pri = qos_map_mpls_remark_pri[mpls_exp];

        flags = BCM_QOS_MAP_MPLS | BCM_QOS_MAP_REMARK;
        BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_add(unit, flags, &mpls_in_map, opcode), "");
   }

    /*
     * Call map add with BCM_QOS_MAP_OPCODE flag in order to register the internal map ID,
     * this is done once for each map ID and protocol
     */
      /** Create opcode profile */
    opcode = mpls_qos_in_internal_map_id;
    BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_create(unit, BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB  | BCM_QOS_MAP_OPCODE | BCM_QOS_MAP_WITH_ID,
                            &opcode), "");
    /** Clear structure */
    bcm_qos_map_t_init(&mpls_in_map);
    mpls_in_map.opcode = opcode;
    flags = BCM_QOS_MAP_MPLS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_OPCODE;
    BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_add(unit, flags, &mpls_in_map, mpls_in_map_id), "PHB OPCODE");

    for (mpls_exp = 0; mpls_exp < 8; mpls_exp++)
    {
        /** Clear structure */
        bcm_qos_map_t_init(&mpls_in_map);
        /*
         * Ingress COS profile Maps:
         * Incoming-EXP (exp) -> TC (int_pri), DP (color), NWK_WOS
         */
        mpls_in_map.exp = mpls_exp;
        mpls_in_map.int_pri = qos_map_mpls_internal_pri[mpls_exp];
        mpls_in_map.color = qos_map_mpls_internal_color[mpls_exp];

        flags = BCM_QOS_MAP_MPLS | BCM_QOS_MAP_PHB;
        BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_add(unit, flags, &mpls_in_map, opcode), "");
    }
    return BCM_E_NONE;
}

/* MAp: COLOR, INT_PRI(IN-DSCP_EXP) -> remark_int_pri (Out-DSCP-EXP)*/
int
dnx_qos_map_mpls_egress_profile(
    int unit)
{
    bcm_qos_map_t mpls_eg_map;
    int flags = 0;
    int mpls_exp;
    bcm_color_t color;
    int exp = 0;

    /** Clear structure */
    bcm_qos_map_t_init(&mpls_eg_map);

    /** Create QOS profile */
    if (mpls_eg_map_id !=0)
    {
        BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_WITH_ID, &mpls_eg_map_id), "");
    }
    else
    {
        BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_id_get_by_profile(unit, BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK, mpls_eg_map_id, &mpls_eg_map_id), "");
    }


    /** Create QOS profile */
    BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE | BCM_QOS_MAP_WITH_ID,
                            &mpls_qos_eg_internal_map_id), "");


    /*
     * Call map add with BCM_QOS_MAP_OPCODE flag in order to register the internal map ID,
     * this is done once for each map ID and protocol
     */
    flags = BCM_QOS_MAP_MPLS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    mpls_eg_map.opcode = mpls_qos_eg_internal_map_id;
    BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_add(unit, flags, &mpls_eg_map, mpls_eg_map_id), "");

    /** COLOR, INT_PRI(NEWK_WOS) -> remark_int_pri (Out-DSCP-EXP) */
    int color_index;
    bcm_qos_map_t_init(&mpls_eg_map);
    for (exp = 0; exp < 256; exp++)
    {
        for (color_index = 0; color_index < 2; color_index++)
        {
            mpls_eg_map.color = color_index;
            mpls_eg_map.int_pri = exp;
            /*
             * Egress mapping is:
             * qos_var - 1:1
             * Only yellow is mapped.
             */
            mpls_eg_map.exp = (exp + ((color_index + 1) % 2)) % 8;
            flags = BCM_QOS_MAP_MPLS | BCM_QOS_MAP_REMARK;
            BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_add(unit, flags, &mpls_eg_map, mpls_qos_eg_internal_map_id), "");
        }
    }

    /*
     * set second exp mapping, mpls_0_1[exp] uses qos_variable[2:0], mpls_0_2[exp] uses qos_variable[5:3]
     * COLOR, INT_PRI(NEWK_WOS) -> remark_int_pri (Out-DSCP-EXP)
     */
    bcm_qos_map_t_init(&mpls_eg_map);
    for (exp = 0; exp < 256; exp++)
    {
        for (color_index = 0; color_index < 4; color_index++)
        {
            mpls_eg_map.color = color_index;
            mpls_eg_map.int_pri = exp;
            /*
             * Egress mapping is:
             * qos_var - 1:1
             */
            mpls_eg_map.exp = (exp+1) & 0x7;
            flags = BCM_QOS_MAP_MPLS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_MPLS_SECOND;
            BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_add(unit, flags, &mpls_eg_map, mpls_qos_eg_internal_map_id), "");
        }
    }

    /*
     * set inheritance mapping,
     * COLOR, INT_PRI(NEWK_WOS) -> remark_int_pri (Out-DSCP-EXP)
     */
    bcm_qos_map_t_init(&mpls_eg_map);
    for (exp = 0; exp < 256; exp++)
    {
        for (color_index = 0; color_index < 4; color_index++)
        {
            mpls_eg_map.color = color_index;
            mpls_eg_map.int_pri = exp;
            /*
             * Egress inheritance mapping is:
             * next stage qos is (qos+1)&7
             */
            mpls_eg_map.remark_color = 1;
            mpls_eg_map.remark_int_pri = (exp + 1) & 0x7;
            flags = BCM_QOS_MAP_MPLS | BCM_QOS_MAP_REMARK;
            BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_add(unit, flags, &mpls_eg_map, mpls_eg_map_id), "");
        }
    }

    return BCM_E_NONE;
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

/* perform full mapping of: Incoming-EXP -> Outgoing-EXP */
int
dnx_qos_map_mpls(
    int unit,
    int is_ingress,
    int is_egress)
{

    if (is_ingress)
    {
        BCM_IF_ERROR_RETURN_MSG(dnx_qos_map_mpls_ingress_profile(unit), "error setting up ingress qos profile");
    }

    if (is_egress)
    {
        BCM_IF_ERROR_RETURN_MSG(dnx_qos_map_mpls_egress_profile(unit), "error setting up egress qos profile");
    }

    return BCM_E_NONE;
}

/*
 * Connect GPORT (VLAN-Port) to its QOS-profile
 * Need to load cint file: cint_vpls_mp_basic.c
 */
int
mpls_qos_map_gport(
    int unit,
    int map_ingress,
    int ingress_gport,
    int map_egress,
    int egress_gport,
    int use_gport_from_cint)
{
    char error_msg[200]={0,};
    int ingress_gport_set, egress_gport_set;

    if (use_gport_from_cint == 1)
    {
        ingress_gport_set = cint_vpls_basic_info.mpls_tunnel_id_ingress;
        egress_gport_set = cint_vpls_basic_info.mpls_tunnel_id;
    } else if (use_gport_from_cint == 2)
    {
        ingress_gport_set = cint_vpls_roo_basic_info.core_overlay_mpls_port_id_ing;
        egress_gport_set = cint_vpls_roo_basic_info.core_overlay_mpls_port_id_eg;
    }
    else
    {
        ingress_gport_set = ingress_gport;
        egress_gport_set = egress_gport;
    }
    if (map_ingress)
    {
        snprintf(error_msg, sizeof(error_msg), "setting up ingress gport(0x%08x) to map", ingress_gport_set);
        BCM_IF_ERROR_RETURN_MSG(bcm_qos_port_map_set(unit, ingress_gport_set, mpls_in_map_id, -1), error_msg);
    }

    if (map_egress)
    {
        snprintf(error_msg, sizeof(error_msg), "setting up egress gport(0x%08x) to map", egress_gport_set);
        BCM_IF_ERROR_RETURN_MSG(bcm_qos_port_map_set(unit, egress_gport_set, -1, mpls_eg_map_id), error_msg);
    }

    return BCM_E_NONE;
}

/* Main function of this Cint:
   * Incoming EXP -> TC, DP, nwk_qos mapping
   * nwk_qos + DP -> outgoing EXP mapping
   * Attach ingress QoS profile to the inRIF
   * Attach egress QoS profile to SWAP entry
*/
int
dnx_qos_run(int unit, int inPort, int outPort)
{
    bcm_l3_ingress_t l3i;

    /* Create ingress and egress qos profile and add mapping */
    BCM_IF_ERROR_RETURN_MSG(dnx_qos_map_mpls(unit, 1, 1), "");

    /* Create MPLS LSR */
    BCM_IF_ERROR_RETURN_MSG(multi_dev_mpls_lsr_swap_example(unit, unit, inPort, outPort), "");

    /* Get the configuration of the InRIF. */
    BCM_IF_ERROR_RETURN_MSG(bcm_l3_ingress_get(unit, cint_mpls_lsr_info.in_rif, &l3i), "");

    /* Attch the ingress QoS profile. */
    l3i.qos_map_id = mpls_in_map_id;
    l3i.flags |= BCM_L3_INGRESS_WITH_ID;

    /* Set the configuration of the InRIF. */
    BCM_IF_ERROR_RETURN_MSG(bcm_l3_ingress_create(unit, l3i, cint_mpls_lsr_info.in_rif), "");

    /* Attach qos profile to swap entry */
    BCM_IF_ERROR_RETURN_MSG(mpls_ingress_swap_remark_profile_set(unit, -1), "");

    return BCM_E_NONE;
}

/* For JR2 devices
 * L3 ingress map:
 * Incoming-DSCP-> TC (int_pri), DP (color)
 */
int
dnx_qos_map_l3_ingress_profile(
    int unit)
{
    bcm_qos_map_t l3_in_map;
    int flags = 0;
    int dscp;
    int opcode = l3_qos_in_internal_map_id_remark;

    /** Clear structure */
    bcm_qos_map_t_init(&l3_in_map);

    /** Create QOS profile */
    BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_create(unit, BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_WITH_ID,
                            &l3_in_map_id), "in ingress l3");

    /** Create opcode profile */
    BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_create(unit, BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE | BCM_QOS_MAP_WITH_ID,
                            &opcode), "in ingress l3 opcode");
    l3_qos_in_internal_map_id_remark = opcode;

    /** Clear structure */
    bcm_qos_map_t_init(&l3_in_map);
    l3_in_map.opcode = opcode;
    /*
     * Call map add with BCM_QOS_MAP_OPCODE flag in order to register the internal map ID,
     * this is done once for each map ID and protocol
     */
    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_add(unit, flags, &l3_in_map, l3_in_map_id), "in l3 ingress, REMARK OPCODE");
    for (dscp = 0; dscp < 256; dscp++)
    {
        /** Clear structure */
        bcm_qos_map_t_init(&l3_in_map);
        /*
         * Ingress COS profile Maps:
         * Incoming (dscp) -> TC (int_pri), DP (color)
         */
        l3_in_map.dscp = dscp  ;
        l3_in_map.remark_int_pri = dscp ;

        flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_REMARK;
        BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_add(unit, flags, &l3_in_map, opcode), "in l3 ingress");
   }

    /*
     * Call map add with BCM_QOS_MAP_OPCODE flag in order to register the internal map ID,
     * this is done once for each map ID and protocol
     */
      /** Create opcode profile */
    opcode = l3_qos_in_internal_map_id_phb;
    BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_create(unit, BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB  | BCM_QOS_MAP_OPCODE | BCM_QOS_MAP_WITH_ID,
                            &opcode), "in ingress l3");
    l3_qos_in_internal_map_id_phb = opcode;
    /** Clear structure */
    bcm_qos_map_t_init(&l3_in_map);
    l3_in_map.opcode = opcode;
    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_PHB | BCM_QOS_MAP_OPCODE;
    BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_add(unit, flags, &l3_in_map, l3_in_map_id), "in l3 ingress, PHB OPCODE");

    for (dscp = 0; dscp < 256; dscp++)
    {
        /** Clear structure */
        bcm_qos_map_t_init(&l3_in_map);
        /*
         * Ingress COS profile Maps:
         * Incoming-DSCP -> TC (int_pri), DP (color), NWK_WOS
         */
        l3_in_map.dscp = dscp;
        l3_in_map.int_pri = dscp % 8;
        l3_in_map.color = 1;

        flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_PHB;
        BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_add(unit, flags, &l3_in_map, opcode), "in l3 ingress");
    }
    return BCM_E_NONE;
}

/** Set egress qos remark profile for mpls ingress swap case.*/
int
mpls_ingress_swap_remark_profile_set(int unit, int remark_profile)
{
    int qos_map_id;

    if (remark_profile == -1) {
        qos_map_id = mpls_eg_map_id;
    } else {
        BCM_IF_ERROR_RETURN_MSG(bcm_qos_map_id_get_by_profile(unit, BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK, remark_profile, &qos_map_id), "");
    }

    int multi_swap_command = *(dnxc_data_get(unit, "headers", "feature", "fhei_mpls_cmd_from_eei", NULL));
    if (multi_swap_command)
    {
        bcm_mpls_tunnel_switch_t entry;
        /**In lsr configure : cint_mpls_lsr_info.incoming_label*/
        entry.label = cint_mpls_lsr_info.incoming_label[0];
        BCM_IF_ERROR_RETURN_MSG(bcm_mpls_tunnel_switch_get(unit, &entry), "");

        entry.qos_map_id = qos_map_id;
        entry.flags |= (BCM_MPLS_SWITCH_WITH_ID | BCM_MPLS_SWITCH_REPLACE);
        BCM_IF_ERROR_RETURN_MSG(bcm_mpls_tunnel_switch_create(unit, &entry), "");
    } else
    {
        BCM_IF_ERROR_RETURN_MSG(bcm_qos_control_set(unit, 0, bcmQosControlMplsIngressSwapRemarkProfile, qos_map_id),
            "in bcmQosControlMplsIngressSwapRemarkProfile");
    }

    return BCM_E_NONE;
}

/** Set mpls explicit NULL label qos profile.*/
int
dnx_qos_mpls_explicit_null_label_profile_set(int unit, int map_id, int is_ipv6)
{

    if (is_ipv6) {
        BCM_IF_ERROR_RETURN_MSG(bcm_qos_control_set(unit, BCM_QOS_MAP_IPV6, bcmQosControlMplsExplicitNullIngressQosProfile, map_id), "");
    } else {
        BCM_IF_ERROR_RETURN_MSG(bcm_qos_control_set(unit, BCM_QOS_MAP_IPV4, bcmQosControlMplsExplicitNullIngressQosProfile, map_id), "");
    }

    return BCM_E_NONE;
}
