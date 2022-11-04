/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

/*
 * Purpose:
 * Example of Linked lists of MC applications.
 * The main MC group is an linked list, and each member of the list is an MC group.
 * Currently, the main MC group and all the member MC groups must be in the same pipeline.
 *
 * Configuration:
 * Test Scenario - start
  cint ../../../../src/examples/dnx/multicast/cint_dnx_multicast_linked_lists.c
  cint
  int unit = 0;
  int rv = 0;
  int in_port = 200;
  int out_port1 = 201;
  int out_port2 = 202;
  int out_port3 = 203;
  rv = multicast_linked_main(unit,in_port,out_port1,out_port2,out_port3,mc_mode);
  print rv;
 * Test Scenario - end
 * 
 * Traffic:
 *  L2 unknown MC traffic is used for the tests, for example IPv4oETH1.
 *  DA - any legal MAC address.
 *  SA - any legal MAC address, not equal to DA.
 *  VLAN -
 *     TPID - 0x9100
 *     VID - 100, which is the main MC group ID.
 *  payload - not care.
 *
 */


int multicast_create_mc_group(
    int unit,
    int ing_egr_indication_flags,
    int mc_id,
    int * ports,
    int nof_ports)
{
    char error_msg[200]={0,};
    int idx;
    uint32 flags;
    bcm_multicast_replication_t rep_array[10];

    /** JR2 egress MC needs fabric replication for each egress core*/
    if ((ing_egr_indication_flags & BCM_MULTICAST_EGRESS_GROUP) && (*dnxc_data_get(unit, "fabric", "general", "blocks_exist", NULL))) {
        int nof_cores = *dnxc_data_get(unit, "device", "general", "nof_cores", NULL);
        bcm_module_t mreps[nof_cores];
        uint32 valid_cores_bmp = *(dnxc_data_get(unit, "device", "general", "valid_cores_bitmap", NULL));
        int nof_copies = 0;
        int core_index;
        for(core_index = 0; core_index < nof_cores; core_index++) {
            if ( valid_cores_bmp & (1 << (core_index))){
                mreps[nof_copies]= core_index;
                nof_copies ++;
            }
        }
        snprintf(error_msg, sizeof(error_msg), "for fabric mc %d", mc_id);
        BCM_IF_ERROR_RETURN_MSG(bcm_fabric_multicast_set(unit, mc_id, 0, nof_copies, mreps), error_msg);
    }

    /** Create the MC group and add the members*/
    flags = BCM_MULTICAST_WITH_ID | ing_egr_indication_flags;
    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(unit, flags, &mc_id), "");

    flags = ing_egr_indication_flags;
    for (idx = 0; idx < nof_ports; idx++) {
        rep_array[idx].port = ports[idx];
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_add(unit, mc_id, flags, nof_ports, rep_array), "");

    printf("Create and add members for %s mc group %d successfully!\n",
           (flags & BCM_MULTICAST_INGRESS_GROUP) ? "ingress" : "egress", mc_id);

    return BCM_E_NONE;
}



/*
 * Example to configure the linked MC group.
 * mc_mode - 0 : main = ingress mc; linked_1 = ingress mc; linked_2 = ingress mc; 
 *           4 : main = egress mc; linked_1 = egress mc;  linked_2 = egress mc;
 * Currently, only mode 0 and 4 are supported.
 */
int multicast_linked_main(
    int unit,
    int in_port,
    int out_port1,
    int out_port2,
    int out_port3,
    int mc_mode)
{
    char error_msg[200]={0,};
    uint32 flags, ing_egr_flags;
    int main_mc_group = 100;
    int linked_mc_group_1 = 200;
    int linked_mc_group_2 = 300;
    int nof_main_cuds = 2;
    int nof_linked_1_cuds = 1;
    int nof_linked_2_cuds = 2;
    bcm_multicast_replication_t linked_1_rep_array[nof_linked_1_cuds];
    bcm_multicast_replication_t linked_2_rep_array[nof_linked_2_cuds];
    int main_mc_ports[nof_main_cuds];
    int linked_mc_1_ports[] = {out_port1};
    int linked_mc_2_ports[] = {out_port2, out_port3};
    int idx;
    int gport;


    if (mc_mode != 0 && mc_mode != 4)
    {
        printf("Error, multicast_linked_main, unsupported mc_mode %d\n", mc_mode);
        return BCM_E_UNAVAIL;
    }

    /** Add the port to vswitch*/
    snprintf(error_msg, sizeof(error_msg), "for port %d", in_port);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_gport_add(unit, main_mc_group, in_port, 0), error_msg);

    BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_create_with_id(unit, main_mc_group), error_msg);

    /** Create the linked MC group 1 and add the members*/
    ing_egr_flags = (mc_mode == 0 || mc_mode == 1) ? BCM_MULTICAST_INGRESS_GROUP : BCM_MULTICAST_EGRESS_GROUP;
    BCM_IF_ERROR_RETURN_MSG(multicast_create_mc_group(unit, ing_egr_flags, linked_mc_group_1, linked_mc_1_ports, nof_linked_1_cuds),
        "for linked group 1");

    /** Create the linked MC group 2 and add the members*/
    ing_egr_flags = (mc_mode == 0 || mc_mode == 2) ? BCM_MULTICAST_INGRESS_GROUP : BCM_MULTICAST_EGRESS_GROUP;
    BCM_IF_ERROR_RETURN_MSG(multicast_create_mc_group(unit, ing_egr_flags, linked_mc_group_2, linked_mc_2_ports, nof_linked_2_cuds),
        "for linked group 2");

    /** Create the main MC group and add the members*/
    ing_egr_flags = (mc_mode == 4) ? BCM_MULTICAST_EGRESS_GROUP : BCM_MULTICAST_INGRESS_GROUP;
    BCM_MCAST_GPORT_GROUP_ID_SET(gport, linked_mc_group_1);
    main_mc_ports[0] = gport;
    BCM_MCAST_GPORT_GROUP_ID_SET(gport, linked_mc_group_2);
    main_mc_ports[1] = gport;
    BCM_IF_ERROR_RETURN_MSG(multicast_create_mc_group(unit, ing_egr_flags, main_mc_group, main_mc_ports, nof_main_cuds),
        "for main group");

    return BCM_E_NONE;
}
