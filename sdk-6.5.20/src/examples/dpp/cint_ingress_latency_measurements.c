/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

 /* Explanation:
 *  This cint includes functions that are required to activate latency measurements between the IPP and ITP.
 *
 * Soc properties need to add for latency feature:
 *    ftmh_dsp_extension_add=1   
 *    phy_1588_dpll_phase_initial_lo=0x40000000
 *    phy_1588_dpll_phase_initial_hi=0x10000000
 *    phy_1588_dpll_frequency_lock=1
 *
 *
 * Calling Sequence:
 *  1. Configure the CRPS for latency measurement using the API bcm_stat_counter_config_set. (or soc property)
 *  2. set an action "bcmFieldActionFabricHeaderSet" via PMF to add for the packet OAM-Sub-Type=Latency measurement.
 *      It means that each packet that pass the qualifier, will include the latency measurement in the IPP.
 *  3. set an action "bcmFieldActionFabricHeaderSet" via PMF to set the LATENCY_FLOW_ID in user header-1
 *  4. In order to read the memory with the highest latenct values (from different flow ids), call API bcm_cosq_max_latency_pkts_get
 *  5. If counter engine was set (source=latency),  use the API bcm_field_stat_get, in order to read a the max latency per latency flow id.

 *   Note: For each LATENCY_FLOW_ID, need to do steps 2&3 (function "latency_configuration_in_pmf_example").
 */




/* latency_configuration_in_pmf_example:
    configure a rule with one qoulifier and two actions in the PMF.
    qualifier: In port
    Action1: bcmFieldActionClassDestSet - set latency flow id in user-header-1
    Action2: configure new header profile which activate the OAM-TS-Latency header.
*/

bcm_cosq_max_latency_pkts_t glb_most_congested_voqs[8];
int glb_memory_actual_count = 0;
uint32 glb_stat_get_val;

int latency_configuration_in_pmf_example(int unit, bcm_field_group_t group, int port, uint16 latency_flow_id)
{
    int result=0;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent;
    bcm_field_stat_t stats[2];
    bcm_port_t in_port;
    int statId;
    int counter_proc;
    int action;
    int is_qax = FALSE;
    is_qumran_ax(unit,&is_qax);

    printf("latency_configuration_in_pmf_example: unit=%d, group=%d, port=%d latency_flow_id=%x\n", unit, group, port, latency_flow_id);

    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);
        
    BCM_FIELD_ASET_INIT(aset);
    if (is_qax) {
        BCM_FIELD_ASET_ADD(aset, bcmFieldActionLatencyFlowId);
    }
    else {
        BCM_FIELD_ASET_ADD(aset, bcmFieldActionClassDestSet);
    }
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionFabricHeaderSet);

    result = bcm_field_group_create_id(unit, qset, BCM_FIELD_GROUP_PRIO_ANY, group);
    if (BCM_E_NONE != result) {
        printf("bcm_field_group_create_id failed \n"); 
        return result;
    }
    result = bcm_field_group_action_set(unit, group, aset);
    if (BCM_E_NONE != result) {
        bcm_field_group_destroy(unit, group);
        printf("bcm_field_group_action_set failed \n");
        return result;
    }
    
    result=bcm_field_entry_create(unit,group,&ent);
    if (result != BCM_E_NONE) {
        printf("Error Field, bcm_field_entry_create\n");
        return result;
    }    
    result = bcm_field_qualify_InPort(unit,ent,port,(~0));
    if (result != BCM_E_NONE) {
        printf("Error Field, bcm_field_qualify_InPort\n");
        return result;
    }
    printf("latency_flow_id: %x\n", latency_flow_id);
    /* it will add to user header-1, the latency_flow_id */
    /* => the user must configure the field (in Jericho) "packet_latency_mesurement_flow_id_sel" to use  user header-1 */
    /* if you want to use user header-2, use action bcmFieldActionClassSourceSet and configure the flow_id_sel to use user header-2*/
    if (is_qax) {
        result=bcm_field_action_add(unit, ent, bcmFieldActionLatencyFlowId, latency_flow_id<<3, 0);
    }
    else {
        result=bcm_field_action_add(unit, ent, bcmFieldActionClassDestSet, latency_flow_id, 0);
    }
    if (result != BCM_E_NONE) {
        printf("Error Field, bcm_field_action_add, bcmFieldActionClassDestSet 0x%x\n", latency_flow_id);
        return result;
    }

    /* Add action to entry */  
    /* Action bcmFieldActionFabricHeaderSet add new header profile.  */
    /* param0=bcmFieldFabricHeaderEthernetlatency means to add a profile which is the same as ethernet + OAM-TS-LATENCY (BUILD_OAM_TS_HEADERf=1)*/
    result = bcm_field_action_add(unit, ent, bcmFieldActionFabricHeaderSet, bcmFieldFabricHeaderEthernetlatency, 0);
    if (result !=  BCM_E_NONE) {                                            
        printf("Error in bcm_field_action_add (%s)\n", bcm_errmsg(rv));
        return result;
    } 

    result = bcm_field_entry_install(unit,ent);
    if (result != BCM_E_NONE) {
        printf("Error Field, bcm_field_entry_install\n");
        return result;
    }
    
    return result;

}

/* ingress_max_latency_table_get: 
    call API to get the latency table from IPT (IPT_MAX_LATENCY memory)
*/
int ingress_max_latency_table_get(int unit)
{
    int rv = BCM_E_NONE;
    int actual_count, i;
    rv = bcm_cosq_max_latency_pkts_get(unit, 0, 0, 8, glb_most_congested_voqs, &glb_memory_actual_count) ;
    if (BCM_E_NONE != rv) {
         printf("bcm_petra_cosq_icgm_max_latency_pkts_get failed\n");
         return rv;
    }    
    printf("------- MAX LATENCY TABLE PRINT: (size=%d) --------\n", glb_memory_actual_count);    
    for(i=0;i<glb_memory_actual_count; i++)
    {
        printf("most_congested_voqs[%d]:\n",i);
        print("\tlatency=%x\n",glb_most_congested_voqs[i].latency);
        printf("\tlatency_flow=%x\n",glb_most_congested_voqs[i].latency_flow);
        printf("\tdest_gport=%x\n",glb_most_congested_voqs[i].dest_gport);
        printf("\tcosq=%x\n",glb_most_congested_voqs[i].cosq);
    }
    return rv;

}

/* ingress_latency_counter_config_set:
    configure a CRPS engine to count latency from ITP. 
*/

int ingress_latency_counter_config_set( int unit, int core, int engineId)
{
    bcm_stat_counter_engine_t counter_engine ; 
    bcm_stat_counter_config_t counter_config ;
    int rv;
    
    counter_config.format.format_type = bcmStatCounterFormatIngressLatency; 
    counter_config.format.counter_set_mapping.counter_set_size = 1;
    counter_config.format.counter_set_mapping.num_of_mapping_entries = 8; 

    /* Counter configuration is independent on the color, drop precedence.*/    
    counter_engine.flags = BCM_STAT_COUNTER_IGNORE_COLORS | BCM_STAT_COUNTER_IGNORE_DISPOSITION;

    counter_config.source.core_id = core; 
    
    counter_config.source.engine_source = bcmStatCounterSourceIngressLatency;
    counter_engine.engine_id =  engineId;
    rv = bcm_stat_counter_config_set(unit, &counter_engine, &counter_config);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_stat_counter_config_set\n");
        return rv;
    }
    return rv;
}

/* ingress_latency_stat_get:
    example how to read the max latency value for a specific latency_flow_id.
*/
int ingress_latency_stat_get( int unit, int latency_flow_id, int engine_id)
{
    int stat_id;
    int rv = BCM_E_NONE;
    uint64 retVal;
    BCM_FIELD_STAT_ID_SET(&stat_id, engine_id, latency_flow_id);

    rv = bcm_field_stat_get(unit, stat_id, bcmFieldStatPackets, &retVal);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_stat_get\n");
        return rv;
    }
    printf("max latency value for latency_flow=0x%X is:", latency_flow_id);
    print("%X",retVal);
    glb_stat_get_val = COMPILER_64_LO(retVal);

    /* 2 values are not valid: */
    /* value=0, means that there was no latency. Not valid */
    /* value {0xFFFFFFFF, 0xFFFFFFFF} means that the delta value is negative. Not valid */
    if((glb_stat_get_val == 0) || ((glb_stat_get_val == 0xFFFFFFFF) && (COMPILER_64_HI(retVal) == 0xFFFFFFFF)))
    {
        printf("ingress_latency_stat_get FAILED. wrong value read from CRPS \n");
        rv = BCM_E_FAIL;
    }
    return rv;
}

int latency_flow_config(int unit, int group_id, int group_prio, bcm_port_t in_port, int latency_flow_id) 
{
    int rv;

    bcm_field_group_config_t group;
    bcm_field_entry_t ent;

    bcm_field_group_config_t_init(&group);
    group.group = group_id;
    group.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ID | BCM_FIELD_GROUP_CREATE_WITH_ASET;
    group.mode = bcmFieldGroupModeAuto;
    group.priority = group_prio;

    /*  QSET  */
    BCM_FIELD_QSET_INIT(group.qset);
    BCM_FIELD_QSET_ADD(group.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(group.qset, bcmFieldQualifyInPort);

    /*  ASET  */
    BCM_FIELD_ASET_INIT(group.aset);
	BCM_FIELD_ASET_ADD(group.aset, bcmFieldActionLatencyFlowId);
    BCM_FIELD_ASET_ADD(group.aset, bcmFieldActionFabricHeaderSet);

    rv = bcm_field_group_config_create(unit, &group);
    if ( rv != BCM_E_NONE ) {
        printf("Error in bcm_field_group_config_create\n");
        return rv;
    }

    /*  entry */
    rv = bcm_field_entry_create(unit, group.group, &ent);
    if ( rv != BCM_E_NONE ) {
        printf("Error in bcm_field_entry_create\n");
        return rv;
    }

    rv = bcm_field_qualify_InPort(unit, ent, in_port, 0xffffffff);
    if ( rv != BCM_E_NONE ) {
        printf("Error in bcm_field_qualify_InPort\n");
        return rv;
    }

    /*  action add  */
    rv = bcm_field_action_add(unit, ent, bcmFieldActionLatencyFlowId, latency_flow_id, 0);
    if ( rv != BCM_E_NONE ) {
        printf("Error in bcm_field_action_add\n");
        return rv;
    }

    rv = bcm_field_action_add(unit, ent, bcmFieldActionFabricHeaderSet, bcmFieldFabricHeaderEthernetlatency, 0);
    if (rv !=  BCM_E_NONE) {                                            
        printf("Error in bcm_field_action_add (%s)\n", bcm_errmsg(rv));
        return result;
    }

    /*  install  */
    rv = bcm_field_group_install(unit, group.group);
    if ( rv != BCM_E_NONE ) {
        printf("Error in bcm_field_group_install\n");
        return rv;
    }

    return rv;
}




