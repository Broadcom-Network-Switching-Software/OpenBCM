 /*
 * Configuration example start:
 *
 * Test Scenario - start
  cint ../../../../src/examples/dnx/field/cint_field_trunk_port_quals.c
  cint;
  int unit = 0;
  bcm_core_t core = 0;
  bcm_field_context_t ingress_context_id = 0;
  bcm_field_context_t egress_context_id = 0;
  bcm_field_stage_t stage = bcmFieldStageIngressPMF1;
  bcm_vlan_t vlan = 110;
  bcm_port_t in_port = 201;
  bcm_port_t out_port = 201;
  int port_class = 2;
  cint_field_trunk_port_quals_out_port_main(unit, core, egress_context_id, vlan, out_port);
  cint_field_trunk_port_quals_dst_port_main(unit, core, egress_context_id, vlan, out_port);
  cint_field_trunk_port_quals_in_port_main(unit, core, ingress_context_id, in_port);
  cint_field_trunk_port_quals_src_port_main(unit, core, ingress_context_id, in_port);
  cint_field_trunk_port_quals_pp_port_profile_main(unit, core, vlan, stage, ingress_context_id, port_class);
  cint_field_trunk_port_quals_tm_port_profile_main(unit, core, vlan, stage, ingress_context_id, port_class);
 * Test Scenario - end
 *
 * Configuration example end
 *
 *  This cint shows examples of using TRUNK GPORT as input for OutPort, DstPort, InPort,
 *  SrcPort and PortClass qualifiers in different field stages.
 *
 *  Following qualifiers are covered:
 *      1. cint_field_trunk_port_quals_out_port_main():
 *          - bcmFieldQualifyOutPort ---> bcmFieldStageEgress
 *      2. cint_field_trunk_port_quals_dst_port_main():
 *          - bcmFieldQualifyDstPort ---> bcmFieldStageEgress
 *      3. cint_field_trunk_port_quals_in_port_main():
 *          - bcmFieldQualifyInPort ---> bcmFieldStageIngressPMF1
 *      4. cint_field_trunk_port_quals_src_port_main():
 *          - bcmFieldQualifySrcPort ---> bcmFieldStageIngressPMF1
 *      5. cint_field_trunk_port_quals_pp_port_profile_main():
 *          - bcmFieldQualifyPortClassPacketProcessing ---> bcmFieldStageIngressPMF1 and bcmFieldStageEgress
 *      6. cint_field_trunk_port_quals_tm_port_profile_main():
 *          - bcmFieldQualifyPortClassTrafficManagement ---> bcmFieldStageIngressPMF1 and bcmFieldStageEgress
 *
 */

bcm_field_group_t Cint_field_trunk_port_quals_fg_id;
bcm_field_entry_t Cint_field_trunk_port_quals_entry_handle;
bcm_trunk_t Cint_field_trunk_port_quals_trunk_id;
bcm_mac_t Cint_field_trunk_port_quals_mac_addr;
bcm_gport_t Cint_field_trunk_port_quals_trunk_gport;

int Cint_field_trunk_port_quals_tc_action_value = 6;

/**
* \brief
*
* \param [in] unit    - Device ID.
* \param [in] port  -  Port to be add as trunk member.
*
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_trunk_port_quals_trunk_config(
    int unit,
    bcm_port_t port)
{
    bcm_trunk_pp_port_allocation_info_t trunk_pp_port_aloc_info, trunk_pp_port_aloc_info_get;
    bcm_switch_control_key_t switch_control_key_info;
    bcm_switch_control_info_t switch_control_info;
    bcm_trunk_member_t trunk_members_info;
    bcm_trunk_info_t trunk_info;
    bcm_gport_t gport;

    BCM_TRUNK_ID_SET(Cint_field_trunk_port_quals_trunk_id, 0, 1);
    printf("TRUNK ID 0x%X (%d) was set. \n", Cint_field_trunk_port_quals_trunk_id, Cint_field_trunk_port_quals_trunk_id);

    BCM_IF_ERROR_RETURN_MSG(bcm_trunk_create(unit, BCM_TRUNK_FLAG_DONT_ALLOCATE_PP_PORTS, &Cint_field_trunk_port_quals_trunk_id), "");

    printf("TRUNK ID 0x%X (%d) was created. \n", Cint_field_trunk_port_quals_trunk_id, Cint_field_trunk_port_quals_trunk_id);

    bcm_trunk_pp_port_allocation_info_t_init(&trunk_pp_port_aloc_info);
    trunk_pp_port_aloc_info.core_bitmap = 3;
    BCM_IF_ERROR_RETURN_MSG(bcm_trunk_pp_port_allocation_set(unit, Cint_field_trunk_port_quals_trunk_id, 0, &trunk_pp_port_aloc_info), "");


    bcm_trunk_pp_port_allocation_info_t_init(&trunk_pp_port_aloc_info_get);
    BCM_IF_ERROR_RETURN_MSG(bcm_trunk_pp_port_allocation_get(unit, Cint_field_trunk_port_quals_trunk_id, 0, &trunk_pp_port_aloc_info_get), "");

    printf("Allocated pp_port1 is %d. \n", trunk_pp_port_aloc_info_get.pp_port_per_core_array[0]);
    printf("Allocated pp_port2 is %d. \n", trunk_pp_port_aloc_info_get.pp_port_per_core_array[1]);

    BCM_GPORT_TRUNK_SET(Cint_field_trunk_port_quals_trunk_gport, Cint_field_trunk_port_quals_trunk_id);
    printf("TRUNK GPORT 0x%X (%d) was set. \n", Cint_field_trunk_port_quals_trunk_gport, Cint_field_trunk_port_quals_trunk_gport);

    switch_control_key_info.index = 1;
    switch_control_key_info.type = bcmSwitchPortHeaderType;
    if (*dnxc_data_get(unit,"switch","svtag","enable_by_defualt",NULL))
    {
        /** for svtag regression */
        switch_control_info.value = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    } else {
        switch_control_info.value = BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP;
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_switch_control_indexed_port_set(unit, Cint_field_trunk_port_quals_trunk_gport, switch_control_key_info, switch_control_info), "");

    switch_control_key_info.index = 2;
    switch_control_key_info.type = bcmSwitchPortHeaderType;
    switch_control_info.value = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    BCM_IF_ERROR_RETURN_MSG(bcm_switch_control_indexed_port_set(unit, Cint_field_trunk_port_quals_trunk_gport, switch_control_key_info, switch_control_info), "");

    printf("Switch control indexed was set for TRUNK GPORT 0x%X (%d). \n", Cint_field_trunk_port_quals_trunk_gport, Cint_field_trunk_port_quals_trunk_gport);

    BCM_GPORT_SYSTEM_PORT_ID_SET(gport, port);
    bcm_trunk_member_t_init(&trunk_members_info);
    trunk_members_info.gport = gport;
    if (*dnxc_data_get(unit,"switch","svtag","enable_by_defualt",NULL))
    {
        /** for svvtag regression */
        bcm_switch_control_port_set(unit, Cint_field_trunk_port_quals_trunk_gport, bcmSwitchL3HeaderErrToCpu, 3);
    }

    bcm_trunk_info_t_init(&trunk_info);
    trunk_info.psc = BCM_TRUNK_PSC_PORTFLOW;
    BCM_IF_ERROR_RETURN_MSG(bcm_trunk_set(unit, Cint_field_trunk_port_quals_trunk_id, &trunk_info, 1, trunk_members_info), "");

    printf("TRUNK ID 0x%X (%d) was configured. \n", Cint_field_trunk_port_quals_trunk_id, Cint_field_trunk_port_quals_trunk_id);

    return BCM_E_NONE;
}


/**
* \brief
*   Configure field group, attaching it to a context and adding an entry to it.
*
* \param [in] unit    - Device ID.
* \param [in] is_negative    - Indicates if the function was called for negative test case.
* \param [in] core    - Core ID for which the entry should be configured.
* \param [in] stage  -  PMF stage for which configuration have to be done.
* \param [in] context_id  -  PMF context Id.
* \param [in] qual_type  -  Qualifier type to be tested.
* \param [in] qual_mask  -  Qualifier mask, to mask the whole qualifier value.
* \param [in] trunk_gport - Trunk Gport to be used as qualifier value.
*
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_trunk_port_quals_group_config(
    int unit,
    uint8 is_negative,
    bcm_core_t core,
    bcm_field_stage_t stage,
    bcm_field_context_t context_id,
    bcm_field_qualify_t qual_type,
    uint32 qual_mask,
    bcm_gport_t trunk_gport)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t entry_info;
    void *dest_char;
    int rv = BCM_E_NONE;

    bcm_field_group_info_t_init(&fg_info);
    bcm_field_group_attach_info_t_init(&attach_info);
    bcm_field_entry_info_t_init(&entry_info);

    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = stage;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "trunk_port_quals", sizeof(fg_info.name));
    fg_info.nof_quals = 1;
    fg_info.nof_actions = 1;
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    entry_info.nof_entry_quals = fg_info.nof_quals;
    entry_info.nof_entry_actions = fg_info.nof_actions;
    entry_info.core = core;

    fg_info.qual_types[0] = qual_type;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 0;
    entry_info.entry_qual[0].type = fg_info.qual_types[0];
    entry_info.entry_qual[0].value[0] = trunk_gport;
    entry_info.entry_qual[0].mask[0] = qual_mask;

    fg_info.action_types[0] = bcmFieldActionPrioIntNew;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    entry_info.entry_action[0].type = fg_info.action_types[0];
    entry_info.entry_action[0].value[0] = Cint_field_trunk_port_quals_tc_action_value;

    /** Create the field group. */
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_add(unit, 0, &fg_info, &Cint_field_trunk_port_quals_fg_id), "");


    printf("Field Group ID %d was created. \n", Cint_field_trunk_port_quals_fg_id);

    
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_attach(unit, 0, Cint_field_trunk_port_quals_fg_id, context_id, &attach_info), "");


    printf("Field Group ID %d was attached to Context ID %d. \n", Cint_field_trunk_port_quals_fg_id, context_id);

    /** Add an entry to the created field group. */
    rv = bcm_field_entry_add(unit, 0, Cint_field_trunk_port_quals_fg_id, &entry_info, &Cint_field_trunk_port_quals_entry_handle);
    if (is_negative)
    {
        if (rv != BCM_E_PARAM)
        {
            printf
                (">>>> ERROR: SDK should return error for diff pp ports for diff cores, only supported for entries by core (expected error = BCM_E_PARAM). Error code: %d\n",
                 rv);
            return BCM_E_FAIL;
        }
    }
    else
    {
        if(rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_entry_add\n", rv);
            return rv;
        }

        printf("Entry ID 0x%X (%d) was added to Field Group ID %d. \n", Cint_field_trunk_port_quals_entry_handle,
           Cint_field_trunk_port_quals_entry_handle, Cint_field_trunk_port_quals_fg_id);
    }

    return BCM_E_NONE;
}

/**
* \brief
*   This function configures trunk for a given out_port
*   in order to test the bcmFieldQualifyOutPort qualifier,
*   with trunk gport.
*
* \param [in] unit    - Device ID.
* \param [in] core    - Core ID for which the entry should be configured.
* \param [in] context_id  -  PMF context Id.
* \param [in] vlan  -  VLAN to configure L2 entry.
* \param [in] out_port  -  Port to be add as trunk member.
*
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_trunk_port_quals_out_port_main(
    int unit,
    bcm_core_t core,
    bcm_field_context_t context_id,
    bcm_vlan_t vlan,
    bcm_port_t out_port)
{
    bcm_l2_addr_t l2_addr_info;

    BCM_IF_ERROR_RETURN_MSG(cint_field_trunk_port_quals_trunk_config(unit, out_port), "");


    bcm_l2_addr_t_init(l2_addr_info, Cint_field_trunk_port_quals_mac_addr, vlan);
    l2_addr_info.flags = BCM_L2_STATIC;
    l2_addr_info.port = Cint_field_trunk_port_quals_trunk_gport;
    l2_addr_info.vid = vlan;
    l2_addr_info.tgid = Cint_field_trunk_port_quals_trunk_id;
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, l2_addr_info), "");


    BCM_IF_ERROR_RETURN_MSG(cint_field_trunk_port_quals_group_config(unit, FALSE, core, bcmFieldStageEgress, context_id, bcmFieldQualifyOutPort, 0x1FF, Cint_field_trunk_port_quals_trunk_gport), "");


    return BCM_E_NONE;
}

/**
* \brief
*   This function configures trunk for a given out_port
*   in order to test the bcmFieldQualifyDstPort qualifier,
*   with trunk gport.
*
* \param [in] unit    - Device ID.
* \param [in] core    - Core ID for which the entry should be configured.
* \param [in] context_id  -  PMF context Id.
* \param [in] vlan  -  VLAN to configure L2 entry.
* \param [in] out_port  -  Port to be add as trunk member.
*
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_trunk_port_quals_dst_port_main(
    int unit,
    bcm_core_t core,
    bcm_field_context_t context_id,
    bcm_vlan_t vlan,
    bcm_port_t out_port)
{
    bcm_l2_addr_t l2_addr_info;

    BCM_IF_ERROR_RETURN_MSG(cint_field_trunk_port_quals_trunk_config(unit, out_port), "");


    bcm_l2_addr_t_init(l2_addr_info, Cint_field_trunk_port_quals_mac_addr, vlan);
    l2_addr_info.flags = BCM_L2_STATIC;
    l2_addr_info.port = Cint_field_trunk_port_quals_trunk_gport;
    l2_addr_info.vid = vlan;
    l2_addr_info.tgid = Cint_field_trunk_port_quals_trunk_id;
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, l2_addr_info), "");


    BCM_IF_ERROR_RETURN_MSG(cint_field_trunk_port_quals_group_config(unit, FALSE, core, bcmFieldStageEgress, context_id, bcmFieldQualifyDstPort, 0xFFFF, Cint_field_trunk_port_quals_trunk_gport), "");


    return BCM_E_NONE;
}

/**
* \brief
*   This function configures trunk for a given in_port
*   in order to test the bcmFieldQualifyInPort qualifier,
*   with trunk gport.
*
* \param [in] unit    - Device ID.
* \param [in] core    - Core ID for which the entry should be configured.
* \param [in] context_id  -  PMF context Id.
* \param [in] in_port  -  Port to be add as trunk member.
*
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_trunk_port_quals_in_port_main(
    int unit,
    bcm_core_t core,
    bcm_field_context_t context_id,
    bcm_port_t in_port)
{
    BCM_IF_ERROR_RETURN_MSG(cint_field_trunk_port_quals_trunk_config(unit, in_port), "");

    BCM_IF_ERROR_RETURN_MSG(cint_field_trunk_port_quals_group_config(unit, FALSE, core, bcmFieldStageIngressPMF1, context_id, bcmFieldQualifyInPort, 0x1FF, Cint_field_trunk_port_quals_trunk_gport), "");

    return BCM_E_NONE;
}

/**
* \brief
*   This function configures trunk for a given in_port
*   in order to test the bcmFieldQualifySrcPort qualifier,
*   with trunk gport.
*
* \param [in] unit    - Device ID.
* \param [in] core    - Core ID for which the entry should be configured.
* \param [in] context_id  -  PMF context Id.
* \param [in] in_port  -  Port to be add as trunk member.
*
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_trunk_port_quals_src_port_main(
    int unit,
    bcm_core_t core,
    bcm_field_context_t context_id,
    bcm_port_t in_port)
{
    BCM_IF_ERROR_RETURN_MSG(cint_field_trunk_port_quals_trunk_config(unit, in_port), "");

    BCM_IF_ERROR_RETURN_MSG(cint_field_trunk_port_quals_group_config(unit, FALSE, core, bcmFieldStageIngressPMF1, context_id, bcmFieldQualifySrcPort, 0xFFFF, Cint_field_trunk_port_quals_trunk_gport), "");

    return BCM_E_NONE;
}

/**
* \brief
*   This function creates FG to qualify upon bcmFieldQualifyPortClassPacketProcessing
*   with value of port_class, which was set to the created TRUNK gport.
*
* \param [in] unit    - Device ID.
* \param [in] core    - Core ID for which the entry should be configured.
* \param [in] vlan    - VLAN to configure L2 entry. Relevant for EPMF.
* \param [in] stage    - PMF stage for which configuration have to be done.
* \param [in] context_id  -  PMF context Id.
* \param [in] port_class  -  Class of the configured TRUNK gport to be used as qualifier value.
*
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_trunk_port_quals_pp_port_profile_main(
    int unit,
    bcm_core_t core,
    bcm_vlan_t vlan,
    bcm_field_stage_t stage,
    bcm_field_context_t context_id,
    int port_class)
{
    bcm_l2_addr_t l2_addr_info;
    uint32 returned_class_id;

    if (stage == bcmFieldStageEgress)
    {
        bcm_l2_addr_t_init(l2_addr_info, Cint_field_trunk_port_quals_mac_addr, vlan);
        l2_addr_info.flags = BCM_L2_STATIC;
        l2_addr_info.port = Cint_field_trunk_port_quals_trunk_gport;
        l2_addr_info.vid = vlan;
        l2_addr_info.tgid = Cint_field_trunk_port_quals_trunk_id;
        BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, l2_addr_info), "");

    }

    BCM_IF_ERROR_RETURN_MSG(cint_field_trunk_port_quals_group_config(unit, FALSE, core, stage, context_id, bcmFieldQualifyPortClassPacketProcessing, 0xFFFFFF, port_class), "");


    if (stage == bcmFieldStageEgress)
    {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_class_get(unit, Cint_field_trunk_port_quals_trunk_gport, bcmPortClassFieldEgressPacketProcessingPort, &returned_class_id), "");
        if (returned_class_id != port_class)
        {
            printf("In bcm_port_class_get, bcmPortClassFieldEgressPacketProcessingPort, expected %d, received %d.\n", port_class, returned_class_id);
            return BCM_E_INTERNAL;
        }
    }
    else if (stage == bcmFieldStageIngressPMF1)
    {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_class_get(unit, Cint_field_trunk_port_quals_trunk_gport, bcmPortClassFieldIngressPMF1PacketProcessingPort, &returned_class_id), "");
        if (returned_class_id != port_class)
        {
            printf("In bcm_port_class_get, bcmPortClassFieldIngressPMF1PacketProcessingPort, expected %d, received %d.\n", port_class, returned_class_id);
            return BCM_E_INTERNAL;
        }
    }

    return BCM_E_NONE;
}

/**
* \brief
*   This function creates FG to qualify upon bcmFieldQualifyPortClassTrafficManagement
*   with value of port_class, which was set to the created TRUNK gport.
*
* \param [in] unit    - Device ID.
* \param [in] core    - Core ID for which the entry should be configured.
* \param [in] vlan    - VLAN to configure L2 entry. Relevant for EPMF.
* \param [in] stage    - PMF stage for which configuration have to be done.
* \param [in] context_id  -  PMF context Id.
* \param [in] port_class  -  Class of the configured TRUNK gport to be used as qualifier value.
*
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int cint_field_trunk_port_quals_tm_port_profile_main(
    int unit,
    bcm_core_t core,
    bcm_vlan_t vlan,
    bcm_field_stage_t stage,
    bcm_field_context_t context_id,
    int port_class)
{
    bcm_l2_addr_t l2_addr_info;
    uint32 returned_class_id;

    if (stage == bcmFieldStageEgress)
    {
        bcm_l2_addr_t_init(l2_addr_info, Cint_field_trunk_port_quals_mac_addr, vlan);
        l2_addr_info.flags = BCM_L2_STATIC;
        l2_addr_info.port = Cint_field_trunk_port_quals_trunk_gport;
        l2_addr_info.vid = vlan;
        l2_addr_info.tgid = Cint_field_trunk_port_quals_trunk_id;
        BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, l2_addr_info), "");

    }

    BCM_IF_ERROR_RETURN_MSG(cint_field_trunk_port_quals_group_config(unit, FALSE, core, stage, context_id, bcmFieldQualifyPortClassTrafficManagement, 0xFFFFFF, port_class), "");


    if (stage == bcmFieldStageEgress)
    {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_class_get(unit, Cint_field_trunk_port_quals_trunk_gport, bcmPortClassFieldEgressTrafficManagementPort, &returned_class_id), "");
        if (returned_class_id != port_class)
        {
            printf("In bcm_port_class_get, bcmPortClassFieldEgressTrafficManagementPort, expected %d, received %d.\n", port_class, returned_class_id);
            return BCM_E_INTERNAL;
        }
    }
    else if (stage == bcmFieldStageIngressPMF1)
    {
        BCM_IF_ERROR_RETURN_MSG(bcm_port_class_get(unit, Cint_field_trunk_port_quals_trunk_gport, bcmPortClassFieldIngressPMF1TrafficManagementPort, &returned_class_id), "");
        if (returned_class_id != port_class)
        {
            printf("In bcm_port_class_get, bcmPortClassFieldIngressPMF1TrafficManagementPort, expected %d, received %d.\n", port_class, returned_class_id);
            return BCM_E_INTERNAL;
        }
    }

    return BCM_E_NONE;
}

/**
 * \brief
 *  Destroys all allocated data by the configuration.
 *
 * \param [in] unit   - Device id
 * \param [in] context_id  -  PMF context Id
 *
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int cint_field_trunk_port_quals_destroy(
    int unit,
    bcm_field_context_t context_id)
{
    BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_delete(unit, Cint_field_trunk_port_quals_fg_id, NULL, Cint_field_trunk_port_quals_entry_handle), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_context_detach(unit, Cint_field_trunk_port_quals_fg_id, context_id), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_field_group_delete(unit, Cint_field_trunk_port_quals_fg_id), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_trunk_destroy(unit, Cint_field_trunk_port_quals_trunk_id), "");

    return BCM_E_NONE;
}
