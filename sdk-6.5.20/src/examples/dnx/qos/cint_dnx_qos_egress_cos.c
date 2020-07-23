/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
* 
* File: cint_dnx_qos_egress_cos.c
* Purpose: An example of the QoS egress cos. 
*
*          ERPP PMF Resolution provides 4 bit COS Profile to the ETPP across the TM Queues
*          ETPP PRP update ftmh_tc, ftmh_dp, nwk_qos according to egress cos profile. 
*          The following CINT provides a calling sequence example for egress cos profile updated by PMF, and cos profile mapping. 
*
* Calling sequence:
*
* Set up sequence:
*  1. Create IP forward service
*        - Call dnx_basic_example() 
*  2. Create ingress phb profile and mapping.
*  3. attach phb profile to In-RIF
*  4. Create egress cos profile and mapping
*  5. set egress cos profile by PMF
*
* Traffic:
*  1.  IPoMPLSoETH: 
          TPID 0x8100, VID 15, IPv4.TOS
*  2.  output packet :
*           default :      TPID 0x8100, VID 100, IPV4.TOS=IPv4.TOS, tc=ingress_tc,dp=ingress_dp
*           cos profile:   TPID 0x8100, VID 100, IPV4.TOS=mapped_nwk_qos, tc=mapped_tc, dp=mapped_dp
*
* To Activate Above Settings Run:
*      BCM> cint src/examples/dnx/qos/cint_dnx_qos_egress_cos.c
*      BCM> cint
*      cint> dnx_egress_cos_profile_main(unit,in_Port, ingress_tc, ingress_dp, qual_value);
*      cint> dnx_qos_pmf_update_cos_profile(unit, mapped_tc, mapped_dp, mapped_nwk_qos);
*/

int ing_phb_map_id = -1;
int ing_opcode_id = -1;
int cos_map_id = -1;
int pmf_presel_id = 70;
int pmf_group;
int pmf_entry_id;
int pmf_context_id;

/*egress cos profile test main function, set service */
int dnx_egress_cos_profile_main(int unit, int in_port, int out_port, int tc, bcm_color_t dp)
{
    int rv = BCM_E_NONE;
    int inf = 15;
    int vrf = 1;

    /* This function will enable the packet to be routed from in_port to out-port */
    rv = dnx_basic_example(unit, in_port, out_port, -1);
    if (rv != BCM_E_NONE)
    {
    	printf("Error in basic_example\n");
    	return rv;
    }
    /*create ingress mapping*/
    rv = dnx_qos_ingress_phb_map(unit, tc, dp);
    if (rv != BCM_E_NONE)
    {
    	printf("Error in dnx_qos_ingress_phb_map\n");
    	return rv;
    }
    /*attach ingress phb profile on rif*/
    rv = qos_intf_ingress_rif_set(unit, inf, vrf, ing_phb_map_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in qos_intf_ingress_rif_set\n");
        return rv;
    }

    return rv;
}

/*  ingress Map: Incoming-dscp -> TC (int_pri), DP (color)*/
int
dnx_qos_ingress_phb_map(int unit, int tc, bcm_color_t color)
{
    bcm_qos_map_t l3_ing_map;
    int dscp;
    int flags = 0;
    int opcode_id;
    int map_id;
    int rv = BCM_E_NONE;

    /*allocate opcode*/
    flags =  BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_OPCODE;
    rv = bcm_qos_map_create(unit, flags, &opcode_id);
    if (rv != BCM_E_NONE) {
        printf("error in ingress mpls bcm_qos_map_create()\n");
        return rv;
    }
    ing_opcode_id =  opcode_id;
    /*allocate profile*/
    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK;
    rv = bcm_qos_map_create(unit, flags, &map_id);
    if (rv != BCM_E_NONE) {
        printf("error in ingress mpls bcm_qos_map_create()\n");
        return rv;
    }
    ing_phb_map_id = map_id;

    bcm_qos_map_t_init(&l3_ing_map);
    flags = BCM_QOS_MAP_PHB | BCM_QOS_MAP_OPCODE | BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4;
    l3_ing_map.opcode = opcode_id;
    rv = bcm_qos_map_add(unit, flags, &l3_ing_map, map_id);
    if (rv != BCM_E_NONE) {
        printf("error in ingress opcode bcm_qos_map_add()\n");
        return rv;
    }
    /* 
     * Ingress DSCP profile Maps:
     * Incoming-DSCP -> TC (int_pri), DP (color)
     */
    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_PHB;
    for(dscp = 0; dscp < 256; dscp++)
    {
        bcm_qos_map_t_init(&l3_ing_map);
        l3_ing_map.dscp             = dscp;
        l3_ing_map.int_pri      = tc;
        l3_ing_map.color            = color;
        rv = bcm_qos_map_add(unit, flags, &l3_ing_map, opcode_id);
        if (rv != BCM_E_NONE) {
            print rv;
        }
    }

    return rv;
}

/*create cos profile and mapping, update egress cos profile by pmf*/
int dnx_qos_pmf_update_cos_profile(int unit, int map_tc, bcm_color_t map_dp, int map_dscp, int qual_value)
{
    int rv = BCM_E_NONE;
    bcm_field_context_t context_id;
    bcm_field_group_t fg_id;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t group_attach_info;
    bcm_field_context_info_t context_info;
    bcm_field_action_info_t action_info;
    void *dest_char;
    bcm_field_entry_info_t entry_info;
    bcm_field_entry_t entry;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    bcm_field_entry_t entry_handle;

    rv = dnx_egress_cos_profile_map(unit, map_tc, map_dp, map_dscp);
    if (rv != BCM_E_NONE)
    {
        printf("Error in dnx_egress_cos_profile_map\n");
        return rv;
    }

    /*create context*/
    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "COS_PROFILE_CTX", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageEgress, &context_info, &context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }
    pmf_context_id = context_id;
 
    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);
 
    p_id.presel_id = pmf_presel_id;
    p_id.stage = bcmFieldStageEgress;
    p_data.entry_valid = TRUE;
    p_data.context_id = context_id;
    p_data.nof_qualifiers = 1;
   
    p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = qual_value; /* bcmFieldLayerTypeIp4/bcmFieldLayerTypeIp6 */
    p_data.qual_data[0].qual_mask = 0x1F;
    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_presel_set\n", rv);
        return rv;
    }

    /** Field Group: to set Context value */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageEgress;
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyIntPriority;
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionQosMapIdNew;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "egress cos profile", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_add failed \n");
        return rv;
    }
    pmf_group = fg_id;

    /** attach program to given context */
    bcm_field_group_attach_info_t_init(&group_attach_info);
    group_attach_info.key_info.nof_quals = fg_info.nof_quals;
    group_attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    group_attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    group_attach_info.payload_info.nof_actions = fg_info.nof_actions;
    group_attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    rv = bcm_field_group_context_attach(unit, 0, fg_id, context_id, &group_attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_context_attach failed \n");
        return rv;
    }

    /** add entry */
    bcm_field_entry_info_t_init(&entry_info);
    entry_info.nof_entry_quals = fg_info.nof_quals;
    entry_info.nof_entry_actions = fg_info.nof_actions;
    entry_info.entry_qual[0].type = fg_info.qual_types[0];
    entry_info.entry_qual[0].value[0] = 0;
    entry_info.entry_qual[0].mask[0] = 0;
    entry_info.entry_action[0].type = fg_info.action_types[0];
    entry_info.entry_action[0].value[0] = cos_map_id & 0xFFFF;
    rv = bcm_field_entry_add(unit, 0, fg_id, &entry_info, &entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }
    pmf_entry_id = entry_handle;

    return 0;
}

/* creates an entry in the cos profile table which will update the TC and DP in ETPP
 * if tc is larger than BCM_COS_MAX, then tc won't be updated
 * if dp is larger or equal to bcmColorPreserve, then dp won't be updated
 * if dscp is larger than 255, then nwk_qos won't be updated
 */
int dnx_egress_cos_profile_map(int unit, int tc, bcm_color_t dp, int dscp)
{
    int rv = BCM_E_NONE;
    int map_id;
    bcm_qos_map_t map;
    int count = 0;

    bcm_qos_map_t_init(&map);

    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_ENCAP_INTPRI_COLOR, map_id);
    if (BCM_E_NONE != rv)
    {
        printf("Error in bcm_qos_map_create\n");
        return rv;
    }
    cos_map_id = map_id;

    map.int_pri = tc;
    map.color = dp;
    map.dscp = dscp;
    rv = bcm_qos_map_add(unit, BCM_QOS_MAP_ENCAP_INTPRI_COLOR, &map, map_id);
    if (BCM_E_NONE != rv) 
    {
        printf("Error in bcm_qos_map_add\n");
        return rv;
    }

    printf("Created an entry with id: %d, tc : %d, dp : %d, dscp : %d\n", map_id, tc, dp, dscp);
    return rv;
}

/* Deletes and destroys the qos profile and map entry*/
int dnx_egress_cos_profile_clean(int unit)
{
    bcm_qos_map_t map;
    int rv = BCM_E_NONE;
    int dscp;
    int flags = 0;

    bcm_field_entry_delete(unit,pmf_group,NULL,pmf_entry_id);
    bcm_field_group_context_detach(unit,pmf_group,pmf_context_id);
    bcm_field_group_delete(unit,pmf_group);

    /* delete all ingress mapping */
    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_PHB;
    for(dscp = 0; dscp < 256; dscp++)
    {
        bcm_qos_map_t_init(&map);
        map.dscp             = dscp;
        rv = bcm_qos_map_delete(unit, flags, &map, ing_opcode_id);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_qos_map_delete\n");
            print rv;
        }
    }
    /*destory opcode*/
    rv = bcm_qos_map_destroy(unit, ing_opcode_id);
    if (BCM_E_NONE != rv)
    {
        printf("Error in bcm_qos_map_destroy\n");
        return rv;
    }

    /*delete opcode mapping*/
    bcm_qos_map_t_init(&map);
    flags = BCM_QOS_MAP_PHB | BCM_QOS_MAP_OPCODE | BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4;
    rv = bcm_qos_map_delete(unit, flags, &map, ing_phb_map_id);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_qos_map_delete\n");
        print rv;
    }
    /*destory phb map di*/
    rv = bcm_qos_map_destroy(unit, ing_phb_map_id);
    if (BCM_E_NONE != rv)
    {
        printf("Error in bcm_qos_map_destroy\n");
        return rv;
    }

    /**delete egress cos mapping and map id*/
    bcm_qos_map_t_init(&map);
    rv = bcm_qos_map_delete( unit, BCM_QOS_MAP_ENCAP_INTPRI_COLOR, &map, cos_map_id); 
    if (BCM_E_NONE != rv)
    {
        printf("Error in bcm_qos_map_delete\n");
        return rv;
    }
    rv = bcm_qos_map_destroy(unit, cos_map_id);
    if (BCM_E_NONE != rv)
    {
        printf("Error in bcm_qos_map_destroy\n");
        return rv;
    }

    return rv;
}

