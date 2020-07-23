/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*~~~~~~~~~~~~~~~~~~~~~~~~~~Fabric: Fabric Mesh configurations~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*  
 * File:        cint_fabric_mesh.c
 * Purpose:     Example of configuration for topology settings in fabric mesh mode.
 * 
 * It is assumed diag_init is executed.
 *
 * Settings include:
 *  o  set modid groups  
 *  o  set link topology
 * 
 *  CINT usage:
 *  o  Run application: 
 *  o  run main function fabric_mesh_config_example
 */


/*  
 * set_modid_group
 * local_dest_id 0-2 (0-1 in MESH_MC)
 * fap_id_count is the number of faps in fap_id_array
 */ 
int set_modid_group(int unit, int local_dest_id, int fap_id_count, bcm_module_t *fap_id_array)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_module_t group_id;

    /* Set group_id according to the given local_dest_id */
    group_id = BCM_FABRIC_GROUP_MODID_SET(local_dest_id);

    /* Set modid group with given fap-id's*/
    rv = bcm_fabric_modid_group_set(unit, group_id, fap_id_count, fap_id_array);
    if (rv != BCM_E_NONE) {
      printf("Error, in modid group set, group_id  $group_id \n");
      return rv;
    }
    return rv;
}


/* 
 * set_mesh_topology_configurations
 * local_dest_id 0-2 (0-1 in MESH_MC)
 * links_count is the number of links in links_array 
 */ 
int set_link_topology(int unit, int local_dest_id, int links_count, bcm_port_t *links_array)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_module_t group_id;

    /* Set group_id according to the given local_dest_id */
    group_id = BCM_FABRIC_GROUP_MODID_SET(local_dest_id);

    /* Set fabric link topology with given links*/
    rv = bcm_fabric_link_topology_set(unit, group_id, links_count, links_array);
    if (rv != BCM_E_NONE) {
      printf("Error, in link topology set, group_id  $group_id \n");
      return rv;
    }
    return rv;
}

/* 
 * Main function
 * FAP functionality example
 */
int fabric_mesh_config_example(int unit, int local_dest_id, int fap_id_count, bcm_module_t *fap_id_array, int links_count, bcm_module_t *links_array)
{
    bcm_error_t rv = BCM_E_NONE;

    /* Set modid group */
    rv = set_modid_group(unit, local_dest_id, fap_id_count, fap_id_array);
    if (rv != BCM_E_NONE) return rv;

    /* Set link topology */
    rv = set_link_topology(unit, local_dest_id, links_count, links_array);
    if (rv != BCM_E_NONE) return rv;

    printf("fabric_mesh_config_example: PASS\n\n");

    return rv;
}

