/*
 * $Id: cint_dnx_utils_flow_id.c
 * $
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 * File: cint_dnx_utils_flow_id.c 
 * Purpose: Example of forcing packet ingressing from ingress-port to flow_id.
 */

/*
 *  
 * Configuration:
 * 
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utils_flow_id.c
 * cint
 * int unit = 0; 
 * int rv = 0; 
 * int in_port = 200; 
 * int flow_id = 1648; 
 * int context_id = 0; 
 * rv = cint_utils_flow_id_set(unit,in_port,flow_id,context_id);
 * print rv; 
 *  
 * Cleaning: 
 * rv = cint_utils_flow_id_clean(unit,context_id);
 */

bcm_field_group_t cint_attach_fg_id=0;

int create_user_defined_trap(int unit)
{
    int trap_id;
    int rv;
    int flags =  BCM_RX_TRAP_TRAP | BCM_RX_TRAP_UPDATE_PRIO;
    
    bcm_rx_trap_config_t  trap_config;

    bcm_rx_trap_config_t_init(&trap_config);

    trap_config.flags     =  flags;
    trap_config.prio = 0;

    printf("create_user_defined_trap \n");

    /* Create the trap */
    rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &trap_id);
    printf("Trap Create - bcmRxTrapUserDefine, ID=0x%x, rc=%d \n", trap_id, rv);
    if(rv != BCM_E_NONE)
    {
      exit;
    }

    /* Set the trap */
    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    printf("Trap Set - ID=0x%x, Flags=0x%x, DestPort=0x%x, rc=%d \n", trap_id, trap_config.flags, trap_config.dest_port, rv);
    if (rv != BCM_E_NONE) 
    {
      exit;
    } 

    return trap_id;
}

int set_ipmf1_trap_action(int unit, bcm_gport_t trap_gport, int cpuQ, int in_port)
{
    int num_quals=1;
    int num_actions=1;
    bcm_field_entry_t cint_attach_entry_id;

    bcm_gport_t in_port_gport = 0;

    bcm_field_entry_info_t ent_info;
    int rv = BCM_E_NONE;

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 0;

    bcm_gport_t active_voq_gport;
    BCM_GPORT_UNICAST_QUEUE_GROUP_SET(active_voq_gport,cpuQ); /** CPU VoQ - FLOW_ID */
    printf("CPU VoQ - FLOW ID is : %d\n", cpuQ);
 
    BCM_GPORT_LOCAL_SET(in_port_gport,in_port);
    ent_info.nof_entry_quals = 1;
    ent_info.entry_qual[0].type = bcmFieldQualifyInPort;
    ent_info.entry_qual[0].value[0] = in_port_gport;
    ent_info.entry_qual[0].mask[0] = 0xff;

    ent_info.nof_entry_actions = 2;
    ent_info.entry_action[0].type = bcmFieldActionTrap;
    ent_info.entry_action[0].value[0] = trap_gport;
    ent_info.entry_action[1].type = bcmFieldActionForward;
    ent_info.entry_action[1].value[0] = (0x400 << 10) |(active_voq_gport & 0xFFFF);

    rv = bcm_field_entry_add(unit, 0, cint_attach_fg_id, &ent_info, &cint_attach_entry_id);
    if (rv != BCM_E_NONE)
    {
       printf("Error (%d), in bcm_field_entry_add\n", rv);
       return rv;
    }
    printf("Entry add: id:(0x%x)  qual val0:(0x%x) act val1:(%d)\n",cint_attach_entry_id,ent_info.entry_qual[0].value[0],
                                                                            ent_info.entry_action[0].value[0]);
    return 0;
}

int field_grp_create_trap_action (int unit, bcm_field_context_t context_id)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    void *dest_char;
    int rv = BCM_E_NONE;


    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;

    /* Set quals */
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyInPort;
    /* Set actions */
    fg_info.nof_actions = 2;
    fg_info.action_types[0] = bcmFieldActionTrap;
    fg_info.action_types[1] = bcmFieldActionForward;


    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "InPortLkup", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_attach_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }
    printf("Created TCAM field group (%d) in iPMF1 \n", cint_attach_fg_id);

    /**Attach IPv4 context**/
    bcm_field_group_attach_info_t_init(&attach_info);

    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "IPv4", sizeof(context_info.name));

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.payload_info.action_types[1] = fg_info.action_types[1];

    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 0;

    
    rv = bcm_field_group_context_attach(unit, 0, cint_attach_fg_id, context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

   return 0;
}



int cint_utils_flow_id_set(int unit, int in_port, int flow_id, bcm_field_context_t context_id)
{
   int trap_id;
   int cpu_voq_number;
   bcm_gport_t cpu_trap_gport;

   printf("cint_utils_flow_id_set: port = %d, flow_id = %d, context_id = %d\n", in_port, flow_id, context_id);

   /* Create user defined trap.*/
   trap_id = create_user_defined_trap(unit);
   BCM_GPORT_TRAP_SET(cpu_trap_gport, trap_id, 8, 0);

   cpu_voq_number = flow_id;
   field_grp_create_trap_action(unit, context_id);
   set_ipmf1_trap_action(unit, cpu_trap_gport, cpu_voq_number, in_port); 
}

int cint_utils_flow_id_clean(int unit, bcm_field_context_t context_id)
{
   int rv = BCM_E_NONE;
   char *proc_name;

   proc_name = "cint_utils_flow_id_clean";

   printf("%s: context_id = %d\n", proc_name, context_id);

   /* Detach the IPMF FG from its context */
   rv = bcm_field_group_context_detach(unit, cint_attach_fg_id, context_id);
   if (rv != BCM_E_NONE)
   {
      printf("%s: Error (%d), bcm_field_group_context_detach(fg = %d, context_id = %d)\n", proc_name, rv, cint_attach_fg_id, context_id);
      return rv;
   }

   return rv;
}
