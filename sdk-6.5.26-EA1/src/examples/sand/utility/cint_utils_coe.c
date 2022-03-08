/* Globals */
int are_system_tools_initialized;

/* Utility functions */
/* initialize_required_system_tools will always be called (by run_cint) before any other CINT function.
   Any required system specific configuration should be place here.
   DO NOT ALTER THIS FUNCTION NAME */
int initialize_required_system_tools(int unit) {
    int rv = BCM_E_NONE;
    return rv;

    if (!are_system_tools_initialized) {
        printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>> Calling required functions for current system <<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");

        /*Place your function calls here and delete me*/

        printf(">>>>>>>>>>>>>>>>>>>>>>>> Finished Calling required functions for current system <<<<<<<<<<<<<<<<<<<<<<<\n");

        are_system_tools_initialized = 1;
    } else {
        return rv;
    }

bcm_port_extender_mapping_info_t coe_mapping_info;
bcm_port_interface_info_t interface_info;
bcm_port_mapping_info_t mapping_info;
uint32 flags;
bcm_gport_t sys_gport;
int coe_ports[4]={200,201,202,203};
int coe_vlans[4]={30, 31, 32, 33};
int index;

for (index=0; index<4; index++) {
    rv = bcm_port_control_set(unit, coe_ports[index], bcmPortControlExtenderEnable, 1);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_control_set failed $rv\n");
        return rv;
    }
    
    coe_mapping_info.vlan = coe_vlans[index];
    coe_mapping_info.phy_port = coe_ports[index];
    rv = bcm_port_extender_mapping_info_set(unit,BCM_PORT_EXTENDER_MAPPING_INGRESS|BCM_PORT_EXTENDER_MAPPING_EGRESS,bcmPortExtenderMappingTypePortVlan,&coe_mapping_info);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_extender_mapping_info_set failed $rv\n");
        return rv;
    }

    BCM_GPORT_SYSTEM_PORT_ID_SET(&sys_gport, coe_ports[index]);
    rv = bcm_stk_sysport_gport_set(unit, sys_gport, coe_ports[index]);
    if (rv != BCM_E_NONE) {
        printf("bcm_stk_sysport_gport_set failed $rv\n");
        return rv;
    }    
}
    return rv;
}

