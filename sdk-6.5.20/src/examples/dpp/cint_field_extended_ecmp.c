
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/*
 * SOC property:
 *    field_presel_mgmt_advanced_mode=1
 *    pmf_kaps_large_db_size=8000
 *
 * This CINT exmpale shows how to create extended ecmp DB for IPv4 program:
 *     Group-Size encoding (OutLIF range):
 *         0x2FFFF => 1 Member
 *         0x2FFFE => 2 Members
 *         0x2FFFD => 4 Members
 *
 *     Program-Selection:
 *         Pre-Selectors:
 *             Forward-Code == IPv4-UC/IPv6-UC 
 *            Group-Size == 1/2/4
 *     Program:
 *        Key:
 *            a.  {Extended-FEC[18:2],LB-Key[1:0]}
 *            b.  {Extended-FEC[18:1],LB-Key[0:0]}
 *            c.  {Extended-FEC[18:0]}
 *    Result: {EEI(24),Destination(19)}
 *  
 * Run:
 *    cint cint_field_extended_ecmp.c
 * cint
 *    field_extended_ecmp_example(int unit); 
 *  
 * Clean: 
 *    field_extended_ecmp_clean_up(int unit);  
 */
struct field_extended_ecmp_s {    
    uint32 group_size_bits;
    int presel_id;
    uint32 outlif_min;
    uint32 outlif_max;
    bcm_field_range_t range;
    bcm_field_group_t group;
    bcm_field_entry_t entry;
    bcm_field_presel_set_t psset;
    int proirity;
    int extended_fec_qual_id;
    int lb_key_qual_id;
};


int field_extended_ecmp_max_kaps_key_length = 14;
int field_extended_ecmp_max_kaps_key_length = 14;

field_extended_ecmp_s field_extended_ecmp_info[3] = {
    {0, 0, 0x2FFFF, 0x2FFFF},
    {1, 1, 0x2FFFE, 0x2FFFE},
    {2, 2, 0x2FFFD, 0x2FFFD}
};

int field_extended_ecmp_example(int unit) {
    bcm_field_group_config_t grp;
    bcm_field_entry_t ent;
    bcm_field_data_qualifier_t data_qual[2];
    uint32 flags;
    uint32 outlif_min, outlif_max;
    uint32 index;
    int presel_id;
    int presel_flags;
    uint8 data[3];
    uint8 mask[3];
    bcm_gport_t gport;
   
    flags = BCM_FIELD_RANGE_OUT_VPORT;
   
    /* Create 3 outlif ranges */
    for (index=0; index<3; ++index) {
    outlif_min = field_extended_ecmp_info[index].outlif_min;
    outlif_max = field_extended_ecmp_info[index].outlif_max;
    BCM_IF_ERROR_RETURN(bcm_field_range_multi_create(unit,
                                   &(field_extended_ecmp_info[index].range),
                                   0 /* overall flags */,
                                   1 /* number of sub-ranges */,
                                   &flags,
                                   &outlif_min,
                                   &outlif_max));

    BCM_FIELD_PRESEL_INIT(field_extended_ecmp_info[index].psset);

    presel_id = field_extended_ecmp_info[index].presel_id;
    presel_flags = BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
    BCM_IF_ERROR_RETURN(bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, presel_id));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_VPortRangeCheck(unit, 
                                      presel_id | BCM_FIELD_QUALIFY_PRESEL, 
                                      field_extended_ecmp_info[index].range, 
                                      0));
    BCM_IF_ERROR_RETURN(bcm_field_qualify_ForwardingType(unit,
                                        presel_id | BCM_FIELD_QUALIFY_PRESEL, 
                                        bcmFieldForwardingTypeIp4Ucast));
    BCM_FIELD_PRESEL_ADD(field_extended_ecmp_info[index].psset, presel_id);

    bcm_field_group_config_t_init(&grp);
    grp.group = field_extended_ecmp_info[index].group;

    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);

    /* 
    * Define the QSET - use FEC and HASH values as qualifiers. 
    */
    bcm_field_data_qualifier_t_init(&data_qual[0]);
    data_qual[0].flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES;
    data_qual[0].stage = bcmFieldStageIngress;
    data_qual[0].qualifier = bcmFieldQualifyDstL3Egress;
    data_qual[0].offset = 0;
    data_qual[0].length = field_extended_ecmp_max_kaps_key_length - field_extended_ecmp_info[index].group_size_bits;
    BCM_IF_ERROR_RETURN(bcm_field_data_qualifier_create(unit, data_qual[0]));
    BCM_IF_ERROR_RETURN(bcm_field_qset_data_qualifier_add(unit, grp.qset, data_qual[0].qual_id));
    field_extended_ecmp_info[index].extended_fec_qual_id = data_qual[0].qual_id;

    if (field_extended_ecmp_info[index].group_size_bits != 0) {
        bcm_field_data_qualifier_t_init(&data_qual[1]);
        data_qual[1].flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES;
        data_qual[1].stage = bcmFieldStageIngress;
        data_qual[1].qualifier = bcmFieldQualifyHashValue;
        data_qual[1].offset = 0;
        data_qual[1].length = field_extended_ecmp_info[index].group_size_bits;
        BCM_IF_ERROR_RETURN(bcm_field_data_qualifier_create(unit, data_qual[1]));
        BCM_IF_ERROR_RETURN(bcm_field_qset_data_qualifier_add(unit, grp.qset, data_qual[1].qual_id));
        field_extended_ecmp_info[index].lb_key_qual_id = data_qual[1].qual_id;
    }
 

    /*
    *  Define the ASET - the result of the look-up should be DEST and EEI.
    */
    BCM_FIELD_ASET_INIT(grp.aset);
    BCM_FIELD_ASET_ADD(grp.aset, bcmFieldActionRedirect);
    BCM_FIELD_ASET_ADD(grp.aset, bcmFieldActionSystemHeaderSet);

    /*  Create the Field group with type Direct Lookup in KAPS */
    grp.group = index;
    grp.priority = BCM_FIELD_GROUP_PRIO_ANY;
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE 
            | BCM_FIELD_GROUP_CREATE_WITH_ID 
            | BCM_FIELD_GROUP_CREATE_LARGE 
            | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET 
            | BCM_FIELD_GROUP_CREATE_WITH_ASET;
    grp.mode = bcmFieldGroupModeDirect;
    grp.preselset = field_extended_ecmp_info[index].psset;
    
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &grp));
    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, grp.group, &ent));

    /* FEC Data qualifier values should be configured acording to the configured FEC LSBs */
    data[0] = 0;
    data[1] = 0xF8 + index << 1;

    mask[0] = (0x3F >> field_extended_ecmp_info[index].group_size_bits);
    mask[1] = 0xFF;

    BCM_IF_ERROR_RETURN(bcm_field_qualify_data(unit, ent, data_qual[0].qual_id, data, mask, 2));

    if (field_extended_ecmp_info[index].group_size_bits != 0) {
        data[0] = 0x0;
        mask[0] = (0x3 >> (2-field_extended_ecmp_info[index].group_size_bits));
        BCM_IF_ERROR_RETURN(bcm_field_qualify_data(unit, ent, data_qual[1].qual_id, data, mask, 1));
    }

    BCM_GPORT_LOCAL_SET(gport, 13);
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit,ent ,bcmFieldActionRedirect, 0, gport));
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit,ent ,bcmFieldActionSystemHeaderSet ,bcmFieldSystemHeaderPphEei,0xCCCCC));
    BCM_IF_ERROR_RETURN(bcm_field_group_install(unit,grp.group));
    field_extended_ecmp_info[index].group = grp.group;
    field_extended_ecmp_info[index].entry = ent;
    }
}

int field_extended_ecmp_clean_up(int unit) {
    int index;

    for (index=0; index<3; ++index) {
        BCM_IF_ERROR_RETURN(bcm_field_entry_destroy(unit, field_extended_ecmp_info[index].entry));
        BCM_IF_ERROR_RETURN(bcm_field_group_destroy(unit, field_extended_ecmp_info[index].group));
        BCM_IF_ERROR_RETURN(bcm_field_data_qualifier_destroy(unit, field_extended_ecmp_info[index].extended_fec_qual_id));
        if (field_extended_ecmp_info[index].group_size_bits != 0) {
            BCM_IF_ERROR_RETURN(bcm_field_data_qualifier_destroy(unit, field_extended_ecmp_info[index].lb_key_qual_id));
        }
       
        BCM_IF_ERROR_RETURN(bcm_field_presel_destroy(unit, field_extended_ecmp_info[index].presel_id));
        BCM_IF_ERROR_RETURN(bcm_field_range_destroy(unit, field_extended_ecmp_info[index].range));
    }
}

