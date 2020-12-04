/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
* 
* File: cint_port_extender_mapping.c
* Purpose: An example of the port extender mapping. 
*          The following CINT provides a calling sequence example to set ingress PP port mapping via user define value.
*
* Calling sequence:
*
*  1. Add the following port configureations to config-sand.bcm
*        field_class_id_size_0.BCM88650=16
*        field_class_id_size_2.BCM88650=16
*        ucode_port_1.BCM88650=10GBase-R13.0
*        ucode_port_2.BCM88650=10GBase-R13.1
*        ucode_port_3.BCM88650=10GBase-R14.0
*        ucode_port_4.BCM88650=10GBase-R14.1
*        ucode_port_5.BCM88650=10GBase-R15
*        custom_feature_vendor_custom_np_port_1=1
*        custom_feature_vendor_custom_np_port_2=1
*        custom_feature_vendor_custom_np_port_3=1
*        custom_feature_vendor_custom_np_port_4=1
*  2. Map the source board and souce port to in pp port.
*        - call bcm_port_extender_mapping_info_set()
*  3. Set the source board and source port which is stamped into eNPHeader.
*        - call bcm_port_class_set() with bcmPortClassProgramEditorEgressPacketProcessing
*  4. Set the system port of the in pp port
*        - call bcm_stk_sysport_gport_set()
*  5. Create service VLAN.
*        - call bcm_vlan_create()
*  6. Add pp ports and physical port into VLAN.
*        - call bcm_vlan_port_add()
*  7. Add a FG to stamp SB, SP to user-header-1.
*        - call bcm_field_group_config_create()
*
* Service Model:
*     Source board + souce port <----> NP PP port   <---VLAN 100---> PHY port
*            1          +             1  <----->     1         <-------------->  5
*            1          +             2  <----->     2         <-------------->  5
*
* Traffic:
*  1. PHY port 5 <-> VLAN 100 <-> NP port 1 (source board 1, source port 1)
*        - From PHY port:
*              -   ethernet header with DA 00:00:00:00:00:01, SA 00:00:00:00:00:02
*              -   VLAN tag: VLAN tag type 0x8100, VID = 100
*        - From NP port:
*              -   eNPHeader 80008830302880000000000
*              -   ethernet header with any DA 00:00:00:00:00:02, SA 00:00:00:00:00:01
*              -   VLAN tag: VLAN tag type 0x8100, VID = 100
*
*  2. NP port 1 (source board 1, source port 1) <-> VLAN 100 <-> PHY port 5
*        - From NP port:
*              -   iNPHeader 000000000000010100000000
*              -   ethernet header with DA 00:00:00:00:00:02, SA 00:00:00:00:00:01
*              -   VLAN tag: VLAN tag type 0x8100, VID = 100
*        - From NNI port:
*              -   ethernet header with any DA 00:00:00:00:00:02, SA 00:00:00:00:00:01
*              -   VLAN tag: VLAN tag type 0x8100, VID = 100
*
*  3. NP port 2 (source board 1, source port 2) <-> VLAN 100 <-> PHY port 5
*        - From NP port:
*              -   iNPHeader 000000000000010100000000
*              -   ethernet header with DA 00:00:00:00:00:03, SA 00:00:00:00:00:01
*              -   VLAN tag: VLAN tag type 0x8100, VID = 100
*        - From NNI port:
*              -   ethernet header with any DA 00:00:00:00:00:03, SA 00:00:00:00:00:01
*              -   VLAN tag: VLAN tag type 0x8100, VID = 100
*
* Service Model:
*     Source board + souce port <----> NP PP port   <---VLAN 200---> Source board + souce port <----> NP PP port 
*            1          +             1  <----->     1          <-------------->        2          +             1  <----->     3
*
* Traffic:
*  1. NP port 3 (source board 2, source port 1) <-> VLAN 200 <-> NP port 1 (source board 1, source port 1)
*        - From NP port:
*              -   iNPHeader 000000000000010200000000
*              -   ethernet header with DA 00:00:00:00:00:01, SA 00:00:00:00:00:04
*              -   VLAN tag: VLAN tag type 0x8100, VID = 200
*        - From NNI port:
*              -   eNPHeader 80008830302880000000000
*              -   ethernet header with any DA 00:00:00:00:00:01, SA 00:00:00:00:00:04
*              -   VLAN tag: VLAN tag type 0x8100, VID = 200
*
* To Activate Above Settings Run:
*      BCM> cint examples/dpp/cint_port_extender_mapping.c
*      BCM> cint
*      cint> port_extender_udf_mapping_np_info_init(0, 1, 1, 1);
*      cint> port_extender_udf_mapping_np_info_init(1, 1, 2, 2);
*      cint> port_extender_udf_mapping_np_info_init(1, 2, 1, 3);
*      cint> port_extender_udf_mapping_np_info_init(1, 2, 2, 4);
*      cint> port_extender_udf_phy_port_vlan_init(100, 200, 5);
*      cint> port_extender_udf_service(0);
*      cint> port_extender_udf_udh_define_field(0);
*      cint> port_extender_udf_udh_add_entry(0, 3, 2, 1);;
*      cint> port_extender_udf_service_clean_up(0);
* To remove a entry
*      cint> bcm_field_entry_remove(0, entry1);
*      cint> bcm_field_entry_destroy(0, entry1);
* To remove a FG
*      cint> bcm_field_group_destroy(0, udh_group.group);
*/

bcm_vlan_t phy_vlan = 100;
bcm_vlan_t np_vlan = 200;

struct np_port_info_s {
    uint32 source_board;
    uint32 source_port;
    bcm_port_t np_port;
};

np_port_info_s np_port_info[4];
bcm_port_t phy_port = 15;
int nof_np_ports = 4;

/* Init np port info */
int port_extender_udf_mapping_np_info_init(uint32 index, uint32 sb, uint32 sp, bcm_port_t np_port)
{
    if (index > nof_np_ports) {
        return BCM_E_PARAM;
    }

    np_port_info[index].source_board = sb;
    np_port_info[index].source_port = sp;
    np_port_info[index].np_port = np_port;

    return 0;
}

/* Init phy port and vlan */
int port_extender_udf_phy_port_vlan_init(bcm_vlan_t phy_vid, bcm_vlan_t np_vid, bcm_port_t port)
{
    phy_vlan = phy_vid;
    np_vlan = np_vid;
    phy_port = port;

    return 0;
}

/* Set np port mapping. np_port should be equal to np_pp_port */
int port_extender_udf_mapping_set(int unit, int sb, int sp, bcm_port_t np_port, bcm_port_t np_pp_port)
{
    int rv = 0;
    int32 user_define_value;
    bcm_port_extender_mapping_info_t mapping_info;

    user_define_value = (sb<<6)|sp;

    mapping_info.user_define_value=user_define_value;
    mapping_info.pp_port=np_pp_port;
    mapping_info.phy_port=np_port;

    /* Map the source board and souce port to in pp port */
    rv = bcm_port_extender_mapping_info_set(unit, BCM_PORT_EXTENDER_MAPPING_INGRESS, bcmPortExtenderMappingTypeUserDefineValue, &mapping_info);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_extender_mapping_info_set failed $rv\n");
        return rv;
    }

    /* Set the destination board and destination port which are stamped into eNPHeader */
    rv = bcm_port_class_set(unit, np_pp_port, bcmPortClassProgramEditorEgressPacketProcessing, user_define_value);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_class_set failed $rv\n");
        return rv;
    }

    /* Set the system port of the in pp port */
    rv = bcm_stk_sysport_gport_set(unit, np_port, np_pp_port);
    if (rv != BCM_E_NONE) {
        printf("bcm_stk_sysport_gport_set failed $rv\n");
        return rv;
    }

    return rv;
}

/* Get np port mapping */
int port_extender_udf_mapping_get(int unit, int sb, int sp, bcm_port_t np_port, bcm_port_t *np_pp_port)
{
    int rv = 0;
    int32 user_define_value;
    bcm_port_extender_mapping_info_t mapping_info;

    user_define_value = (sb<<6)|sp;

    sal_memset(&mapping_info, 0, sizeof(bcm_port_extender_mapping_info_t));
    mapping_info.user_define_value=user_define_value;
    mapping_info.phy_port=np_port;

    /* Get the mapped in pp port based on source board and souce port */
    rv = bcm_port_extender_mapping_info_get(unit, BCM_PORT_EXTENDER_MAPPING_INGRESS, bcmPortExtenderMappingTypeUserDefineValue, &mapping_info);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_extender_mapping_info_set failed $rv\n");
        return rv;
    }

    *np_pp_port = mapping_info.pp_port;

    return rv;
}

/* Build the service mode. VLAN 100 includes both np and phy ports */
int port_extender_udf_service(int unit)
{
    int rv = 0;
    bcm_pbmp_t pbmp, ubmp;
    int index;

    /* create phy_vlan for NP port<>PHY port service */
    rv = bcm_vlan_create(unit, phy_vlan);
    if (rv != BCM_E_NONE) {
        printf("bcm_vlan_create failed $rv\n");
        return rv;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, np_port_info[0].np_port);
    BCM_PBMP_PORT_ADD(pbmp, np_port_info[1].np_port);
    BCM_PBMP_PORT_ADD(pbmp, phy_port);
    BCM_PBMP_CLEAR(ubmp);

    rv = bcm_vlan_port_add(unit, phy_vlan, pbmp, ubmp);
    if (rv != BCM_E_NONE) {
        printf("bcm_vlan_port_add failed $rv\n");
        return rv;
    }

    /* create np_vlan for NP port<>NP port service */
    rv = bcm_vlan_create(unit, np_vlan);
    if (rv != BCM_E_NONE) {
        printf("bcm_vlan_create failed $rv\n");
        return rv;
    }

    BCM_PBMP_CLEAR(pbmp);
    for (index = 0; index < nof_np_ports; index++) {
        if (np_port_info[index].np_port != 0) {
            BCM_PBMP_PORT_ADD(pbmp, np_port_info[index].np_port);
        }
    }
    BCM_PBMP_CLEAR(ubmp);

    rv = bcm_vlan_port_add(unit, np_vlan, pbmp, ubmp);
    if (rv != BCM_E_NONE) {
        printf("bcm_vlan_port_add failed $rv\n");
        return rv;
    }

    for (index = 0; index < nof_np_ports; index++) {
        rv = port_extender_udf_mapping_set(unit, np_port_info[index].source_board, np_port_info[index].source_port, 
            np_port_info[index].np_port, np_port_info[index].np_port);
        if (rv != BCM_E_NONE) {
            printf("port_extender_udf_mapping_set failed $rv\n");
            return rv;
        }
    }

    return rv;
}

/* Clean up the service. */
int port_extender_udf_service_clean_up(int unit)
{
    int rv = 0;
    int index;

    rv = bcm_vlan_destroy(unit, phy_vlan);
    if (rv != BCM_E_NONE) {
        printf("bcm_vlan_destroy failed $rv\n");
        return rv;
    }
    
    rv = bcm_vlan_destroy(unit, np_vlan);
    if (rv != BCM_E_NONE) {
        printf("bcm_vlan_destroy failed $rv\n");
        return rv;
    }

    for (index = 0; index < nof_np_ports; index++) {
        rv = port_extender_udf_mapping_set(unit, np_port_info[index].source_board, np_port_info[index].source_port, 
            np_port_info[index].np_port, 0);
        if (rv != BCM_E_NONE) {
            printf("port_extender_udf_mapping_set failed $rv\n");
            return rv;
        }
    }

    return rv;
}


/* 
 * Purpose: Use PMF to stamp SP, SB into user-header-1
 * UsrHdr -> { 2'b0 , SP(6) , SB(8) }
 *
 *  field_class_id_size_0.BCM88650=16
 *  field_class_id_size_2.BCM88650=16
 */
bcm_field_group_config_t udh_group;
int port_extender_udf_udh_define_field(int unit)
{
    int res = 0;
    bcm_field_qset_t  qset;
    bcm_field_aset_t aset;
    uint32 fg_udh_flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | 
                            BCM_FIELD_GROUP_CREATE_WITH_ASET;

    BCM_FIELD_QSET_INIT(qset);

    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionClassDestSet);

    bcm_field_group_config_t_init(&udh_group);
    udh_group.group=10;
    udh_group.priority=2;
    udh_group.mode=bcmFieldGroupModeAuto;

    udh_group.flags= fg_udh_flags;
    udh_group.aset=aset;
    udh_group.qset=qset;
    res = bcm_field_group_config_create(unit,&udh_group);
    if (res != BCM_E_NONE) {
        printf("Error Field, bcm_field_group_config_create\n");
        return res;
    }
 
    /* for debug */
    /* bcm_field_group_dump(unit,10); */
    return res;
}

bcm_field_entry_t entry1;
int port_extender_udf_udh_add_entry(int unit, int port, int src_board, int src_port)
{
    int res = 0;
    int udh_val;

    res=bcm_field_entry_create(unit,udh_group.group,&entry1);
    if (res != BCM_E_NONE) {
        printf("Error Field, bcm_field_entry_create\n");
        return res;
    }

    res = bcm_field_qualify_InPort(unit,entry1,port,(~0));
    if (res != BCM_E_NONE) {
        printf("Error Field, bcm_field_qualify_InPort\n");
        return res;
    }
    /*as in the header definition, SB is LSB(bits [39:32], SP - MSB(bits[45:40])*/
    udh_val = ((src_port << 8) | src_board);
    printf("udh_val: %x\n", udh_val);
    res=bcm_field_action_add(unit,entry1,bcmFieldActionClassDestSet, udh_val,0);
    if (res != BCM_E_NONE) {
        printf("Error Field, bcm_field_action_add, bcmFieldActionClassDestSet 0x%x\n",udh_val);
        return res;
    }

    res=bcm_field_entry_install(unit,entry1);
    if (res != BCM_E_NONE) {
        printf("Error Field, bcm_field_entry_install\n");
        return res;
    }

    return res;

}


