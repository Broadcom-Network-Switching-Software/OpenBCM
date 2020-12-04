/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
* 
* File: cint_port_extender_dynamic_switching.c
* Purpose: An example of the port extender dynamic switching. 
*          The following CINT provides a calling sequence example to set port extender type between COE and regular dynamically.
*
* Calling sequence:
*
*  1. Add the following port configureations to config-sand.bcm
*        ucode_port_1.BCM88650=10GBase-R13.0
*        ucode_port_2.BCM88650=10GBase-R13.1
*        ucode_port_3.BCM88650=10GBase-R14
*        custom_feature_map_port_extr_enabled.BCM88650=2
*  2. Enable port extender for port X.
*        - call bcm_port_control_set() with bcmPortControlExtenderEnable.
*  3. Map port and COE VLAN ID to in pp port.
*        - call bcm_port_extender_mapping_info_set()
*  3. Set COE VLAN ID which is stamped into egress COE tag.
*        - call bcm_port_class_set() with bcmPortClassProgramEditorEgressPacketProcessing
*  4. Set the system port of the in pp port
*        - call bcm_stk_sysport_gport_set()
*  5. Create service VLAN.
*        - call bcm_vlan_create()
*  6. Add pp ports and local port into VLAN.
*        - call bcm_vlan_port_add()
*
* Service Model:
*     port + COE VID <----> COE PP port   <---VLAN 100---> Local port
*       1   +      10    <----->     1            <-------------->  5
*       1   +      20    <----->     2            <-------------->  5
*
* Traffic:
*  1. Local port 5 <-> VLAN 100 <-> COE port 1 (port 1, COE VID 10)
*        - From Local port:
*              -   ethernet header with DA 00:00:00:00:00:01, SA 00:00:00:00:00:02
*              -   VLAN tag: VLAN tag type 0x8100, VID = 100
*        - From COE port:
*              -   ethernet header with any DA 00:00:00:00:00:02, SA 00:00:00:00:00:01
*              -   VLAN tag: COE VLAN tag type 0x8100, VID = 10, VLAN tag type 0x8100, VID = 100
*
*  2.  COE port 1 (port 1, COE VID 10) <-> VLAN 100 <-> Local port 5
*        - From COE port:
*              -   ethernet header with DA 00:00:00:00:00:02, SA 00:00:00:00:00:01
*              -   VLAN tag:  COE VLAN tag type 0x8100, VID = 10, VLAN tag type 0x8100, VID = 100
*        - From Local port:
*              -   ethernet header with any DA 00:00:00:00:00:02, SA 00:00:00:00:00:01
*              -   VLAN tag: VLAN tag type 0x8100, VID = 100
*
*  3. Local port 5 <-> VLAN 100 <-> COE port 2 (port 1, COE VID 20)
*        - From Local port:
*              -   ethernet header with DA 00:00:00:00:00:03, SA 00:00:00:00:00:02
*              -   VLAN tag: VLAN tag type 0x8100, VID = 100
*        - From COE port:
*              -   ethernet header with any DA 00:00:00:00:00:02, SA 00:00:00:00:00:03
*              -   VLAN tag: COE VLAN tag type 0x8100, VID = 20, VLAN tag type 0x8100, VID = 100
*
*  4.  COE port 2 (port 1, COE VID 20) <-> VLAN 100 <-> Local port 5
*        - From COE port:
*              -   ethernet header with DA 00:00:00:00:00:02, SA 00:00:00:00:00:03
*              -   VLAN tag:  COE VLAN tag type 0x8100, VID = 20, VLAN tag type 0x8100, VID = 100
*        - From Local port:
*              -   ethernet header with any DA 00:00:00:00:00:02, SA 00:00:00:00:00:03
*              -   VLAN tag: VLAN tag type 0x8100, VID = 100
*
*
* Disable port extender for local port 1
* Service Model:
*   local port    ---VLAN 10---> Local port
*       1           ------------->  5
*
*   local port    <---VLAN 100-- Local port
*       1           <-------------  5
*
*  1. Local port 5 <-> VLAN 100 <-> Local port 1
*        - From local port 5:
*              -   ethernet header with DA 00:00:00:00:00:01, SA 00:00:00:00:00:02
*              -   VLAN tag: VLAN tag type 0x8100, VID = 100
*        - From local port 1:
*              -   ethernet header with any DA 00:00:00:00:00:02, SA 00:00:00:00:00:01
*              -   VLAN tag: VLAN tag type 0x8100, VID = 100
*
*  2. Local port 1 COE VID 10 <-> VLAN 100 <-> Local port 5
*        - From local port 1:
*              -   ethernet header with DA 00:00:00:00:00:02, SA 00:00:00:00:00:01
*              -   VLAN tag:  COE VLAN tag type 0x8100, VID = 10, VLAN tag type 0x8100, VID = 100
*        - From local port5 :
*              -   ethernet header with DA 00:00:00:00:00:02, SA 00:00:00:00:00:01
*              -   VLAN tag:  COE VLAN tag type 0x8100, VID = 10, VLAN tag type 0x8100, VID = 100
*
* To Activate Above Settings Run:
*      BCM> cint examples/sand/utility/cint_sand_utils_global.c
*      BCM> cint examples/dpp/cint_port_extender_dynamic_switching.c
*      BCM> cint
*      cint> port_extender_dynamic_swithing_coe_info_init(0, 1, 10, 1);
*      cint> port_extender_dynamic_swithing_coe_info_init(1, 2, 20, 1);
*      cint> port_extender_dynamic_swithing_coe_vlan_init(100, 3);
*      cint> port_extender_dynamic_swithing_coe_service(unit);
*      cint> port_extender_dynamic_swithing_coe_service_clean_up(unit);
*/

bcm_vlan_t service_vlan = 100;

struct coe_port_info_s {
    bcm_vlan_t coe_vlan;
    bcm_port_t coe_port;
    int        pp_port;
    bcm_port_t phy_port;
};

int MAX_NOF_COE_PORTS = 256;
coe_port_info_s coe_port_info[MAX_NOF_COE_PORTS];
bcm_port_t regular_port = 5;
int nof_coe_ports = 2;

/* Init coe port info */
int port_extender_dynamic_swithing_coe_info_init(uint32 index, uint32 coe_port, uint32 pp_port, bcm_port_t coe_vlan, uint32 phy_port)
{
    if (index > nof_coe_ports) {
        return BCM_E_PARAM;
    }

    coe_port_info[index].coe_vlan = coe_vlan;
    coe_port_info[index].pp_port = pp_port;
    coe_port_info[index].coe_port = coe_port;
    coe_port_info[index].phy_port = phy_port;

    return 0;
}

/* Init phy port and vlan */
int port_extender_dynamic_swithing_coe_vlan_init(bcm_vlan_t service_vid, bcm_port_t port)
{
    service_vlan = service_vid;
    regular_port = port;

    return 0;
}

/* Set coe port mapping. */
int port_extender_dynamic_swithing_coe_mapping_set(int unit, int coe_vlan, bcm_port_t coe_port, int pp_port, bcm_port_t phy_port)
{
    int rv = 0;
    bcm_port_extender_mapping_info_t mapping_info;
    bcm_gport_t sys_gport;
    int flags;

    mapping_info.vlan = coe_vlan;
    mapping_info.pp_port = pp_port;
    mapping_info.phy_port = phy_port;
    /* On JR2, both ingress and egress flags is needed */
    if (is_device_or_above(unit, JERICHO2)) {
        flags = BCM_PORT_EXTENDER_MAPPING_INGRESS | BCM_PORT_EXTENDER_MAPPING_EGRESS;
    } else {
        flags = BCM_PORT_EXTENDER_MAPPING_INGRESS;
    }

    /* Map port and COE VLAN ID to in pp port */
    rv = bcm_port_extender_mapping_info_set(unit, flags, bcmPortExtenderMappingTypePortVlan, &mapping_info);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_extender_mapping_info_set failed $rv\n");
        return rv;
    }

    /* Set COE VLAN ID which is stamped into egress COE tag */
    if (!is_device_or_above(unit, JERICHO2)) {
        rv = bcm_port_class_set(unit, coe_port, bcmPortClassProgramEditorEgressPacketProcessing, coe_vlan);
        if (rv != BCM_E_NONE) {
            printf("bcm_port_class_set failed $rv\n");
            return rv;
        }
    }

    /* Set the system port of the in pp port */
    if (is_device_or_above(unit, JERICHO2)) {
        BCM_GPORT_SYSTEM_PORT_ID_SET(&sys_gport, coe_port);
        rv = bcm_stk_sysport_gport_set(unit, sys_gport, coe_port);
    } else {
        rv = bcm_stk_sysport_gport_set(unit, coe_port, coe_port);
    }

    if (rv != BCM_E_NONE) {
        printf("bcm_stk_sysport_gport_set failed $rv\n");
        return rv;
    }

    return rv;
}

/* Get coe port mapping */
int port_extender_dynamic_swithing_coe_mapping_get(int unit, int coe_vlan, bcm_port_t coe_port, bcm_port_t *coe_pp_port)
{
    int rv = 0;
    bcm_port_extender_mapping_info_t mapping_info;

    sal_memset(&mapping_info, 0, sizeof(bcm_port_extender_mapping_info_t));
    mapping_info.vlan = coe_vlan;
    mapping_info.phy_port = coe_port;

    /* Get the mapped in pp port based on port and COE VLAN ID */
    rv = bcm_port_extender_mapping_info_get(unit, BCM_PORT_EXTENDER_MAPPING_INGRESS, bcmPortExtenderMappingTypePortVlan, &mapping_info);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_extender_mapping_info_set failed $rv\n");
        return rv;
    }

    *coe_pp_port = mapping_info.pp_port;

    return rv;
}

int port_extender_dynamic_swithing_enable_set(int unit, int coe_port, int enable)
{
    int rv = 0;
    int index;
    
    rv = bcm_port_control_set(unit, coe_port, bcmPortControlExtenderEnable, enable);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_control_set failed bcmPortControlExtenderEnable $rv\n");
        return rv;
    }

    if (enable) {
        for (index = 0; index < nof_coe_ports; index++) {
            if (coe_port_info[index].coe_port == coe_port) {
                rv = port_extender_dynamic_swithing_coe_mapping_set(unit, coe_port_info[index].coe_vlan, coe_port_info[index].coe_port, coe_port_info[index].pp_port, coe_port_info[index].phy_port);
                if (rv != BCM_E_NONE) {
                    printf("port_extender_dynamic_swithing_coe_mapping_set failed $rv\n");
                    return rv;
                }
            }
        }
    }

    return rv;
}

int port_extender_dynamic_swithing_enable_get(int unit, int coe_port, int *enable)
{
    int rv = 0;
    rv = bcm_port_control_get(unit, coe_port, bcmPortControlExtenderEnable, enable);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_control_get failed bcmPortControlExtenderEnable $rv\n");
        return rv;
    }

    return rv;
}


/* Build the service. VLAN 100/10/20 include both coe and regular ports */
int port_extender_dynamic_swithing_coe_service(int unit)
{
    int rv = 0;
    bcm_pbmp_t pbmp, ubmp;
    int index;
    int chip_get;
    int if_set_force_local = 1;

    if (is_device_or_above(unit,JERICHO)) {
        if_set_force_local = 0;
    }

    if (if_set_force_local == 0)  {
	    /* do nothing for Jericho and QMX */
    } else {
        bshell(unit,"m IPT_FORCE_LOCAL_OR_FABRIC FORCE_LOCAL=1");
    }
    /* create service_vlan for coe ports<>regular port service */
    rv = bcm_vlan_create(unit, service_vlan);
    if (rv != BCM_E_NONE) {
        printf("bcm_vlan_create failed $rv\n");
        return rv;
    }

    for (index = 0; index < nof_coe_ports; index++) {
        if (coe_port_info[index].coe_vlan != 0) {
            rv = bcm_vlan_create(unit, coe_port_info[index].coe_vlan);
            if (rv != BCM_E_NONE) {
                printf("bcm_vlan_create failed $rv\n");
                return rv;
            }
        }
    }
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, regular_port);
    for (index = 0; index < nof_coe_ports; index++) {
        if (coe_port_info[index].coe_port != 0) {
            BCM_PBMP_PORT_ADD(pbmp, coe_port_info[index].coe_port);
        }
    }
    BCM_PBMP_CLEAR(ubmp);

    rv = bcm_vlan_port_add(unit, service_vlan, pbmp, ubmp);
    if (rv != BCM_E_NONE) {
        printf("bcm_vlan_port_add failed $rv\n");
        return rv;
    }

    for (index = 0; index < nof_coe_ports; index++) {
        if (coe_port_info[index].coe_vlan != 0) {
            rv = bcm_vlan_port_add(unit, coe_port_info[index].coe_vlan, pbmp, ubmp);
            if (rv != BCM_E_NONE) {
                printf("bcm_vlan_port_add failed $rv\n");
                return rv;
            }
        }
    }

    for (index = 0; index < nof_coe_ports; index++) {
        if (coe_port_info[index].coe_port != 0) {
            rv = port_extender_dynamic_swithing_enable_set(unit, coe_port_info[index].coe_port, 1);
            if (rv != BCM_E_NONE) {
                printf("port_extender_dynamic_swithing_coe_mapping_set failed $rv\n");
                return rv;
            }
        }
    }

    return rv;
}

/* Clean up the service. */
int port_extender_dynamic_swithing_coe_service_clean_up(int unit)
{
    int rv = 0;
    int index;

    rv = bcm_vlan_destroy(unit, service_vlan);
    if (rv != BCM_E_NONE) {
        printf("bcm_vlan_destroy failed $rv\n");
        return rv;
    }
    
    for (index = 0; index < nof_coe_ports; index++) {
        if (coe_port_info[index].coe_vlan != 0) {
            rv = bcm_vlan_destroy(unit, coe_port_info[index].coe_vlan);
            if (rv != BCM_E_NONE) {
                printf("bcm_vlan_destroy failed $rv\n");
                return rv;
            }
        }
        
        if (coe_port_info[index].coe_port != 0) {
            rv = port_extender_dynamic_swithing_enable_set(unit, coe_port_info[index].coe_port, 0);
            if (rv != BCM_E_NONE) {
                printf("port_extender_dynamic_swithing_coe_mapping_set failed $rv\n");
                return rv;
            }
        }
    }

    return rv;
}


void port_extender_dynamic_switching_bcm_set(int unit)
{
    bcm_info_t info;
    int rv;

    rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_info_get\n");
        return -1;
    }

    if (is_device_or_above(unit,JERICHO)) {
        if ((info.device == 0x8470) || (info.device == 0x8270)) {/*QAX/QUX*/
            bshell(unit,"mod IRE_PKT_SRC_TO_CHANNEL_MAP 0 1 CHANNEL=8");
            bshell(unit,"mod IHP_PTC_INFO 210 1 PTC_OUTER_HEADER_START=6");
            bshell(unit,"mod IHP_VIRTUAL_PORT_TABLE 13450 1 IN_PP_PORT=210");
            bshell(unit,"modr IRE_TAG_SWAP_CONFIGURATION TAG_SWAP_OFFSET_0=14");
        } else if ((info.device == 0x8675) || (info.device == 0x8375)) { /*Jer/Qmx*/
            bshell(unit,"mod IRE_PKT_SRC_TO_CHANNEL_MAP 0 1 CHANNEL=8");
            bshell(unit,"mod IHP_PTC_INFO 210 1 PTC_OUTER_HEADER_START=6");
            bshell(unit,"mod IHP_VIRTUAL_PORT_TABLE 13450 1 IN_PP_PORT=210");
            bshell(unit,"modr IRE_TAG_SWAP_CONFIGURATION TAG_SWAP_N_OFFSET_0=14");
        } else if (info.device == 0x8680) { /*Jer+*/
            bshell(unit,"mod IRE_PKT_SRC_TO_CHANNEL_MAP 0 1 CHANNEL=8");
            bshell(unit,"mod IHP_PTC_INFO 210 1 PTC_OUTER_HEADER_START=6");
            bshell(unit,"mod IHP_VIRTUAL_PORT_TABLE 13450 1 IN_PP_PORT=210");
            bshell(unit,"modr IRE_TAG_SWAP_CONFIGURATION TAG_SWAP_N_OFFSET_0=14");
        } else {
            printf("Error to set!\n");
            return -1;
        }        
    }
    
    return 0;
}



coe_port_info_s coe_port_info_multi_flows[30];

int nof_coe_ports_multi_flows = 30;


int port_extender_dynamic_swithing_coe_info_init_multi_flows(uint32 index, uint32 coe_port, uint32 pp_port, bcm_port_t coe_vlan, uint32 phy_port)
{
    if (index > nof_coe_ports_multi_flows) {
        return BCM_E_PARAM;
    }

    coe_port_info_multi_flows[index].coe_vlan = coe_vlan;
    coe_port_info_multi_flows[index].coe_port = coe_port;
    coe_port_info_multi_flows[index].pp_port = pp_port;
    coe_port_info_multi_flows[index].phy_port = phy_port;

    return 0;
}

int port_extender_dynamic_swithing_enable_set_multi_flows(int unit, int coe_port, int enable)
{
    int rv = 0;
    int index;
    
    rv = bcm_port_control_set(unit, coe_port, bcmPortControlExtenderEnable, enable);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_control_set failed bcmPortControlExtenderEnable $rv\n");
        return rv;
    }

    if (enable) {
        for (index = 0; index < nof_coe_ports_multi_flows; index++) {
            if (coe_port_info_multi_flows[index].coe_port == coe_port) {
                rv = port_extender_dynamic_swithing_coe_mapping_set(unit, 
                    coe_port_info_multi_flows[index].coe_vlan,
                    coe_port_info_multi_flows[index].coe_port,
                    coe_port_info_multi_flows[index].pp_port, 
                    coe_port_info_multi_flows[index].phy_port);
                if (rv != BCM_E_NONE) {
                    printf("port_extender_dynamic_swithing_coe_mapping_set_multi_flows failed $rv\n");
                    return rv;
                }
            }
        }
    }

    return rv;
}



int port_extender_dynamic_swithing_coe_service_multiple_flows(int unit, int enable)
{
    int index;
    int rv = 0;
    
    for (index = 0; index < nof_coe_ports_multi_flows; index++) {
        if (coe_port_info_multi_flows[index].coe_port != 0) {
            rv = port_extender_dynamic_swithing_enable_set_multi_flows(unit, coe_port_info_multi_flows[index].coe_port, enable);
            if (rv != BCM_E_NONE) {
                printf("port_extender_dynamic_swithing_coe_mapping_set failed $rv\n");
                return rv;
            }
            
            bcm_port_class_set(unit, coe_port_info_multi_flows[index].coe_port, bcmPortClassId, coe_port_info_multi_flows[index].coe_port);
        }
    }


}

int port_extender_dynamic_swithing_coe_create_p2p_service(int p1, bcm_vlan_t vlan1, int p2, bcm_vlan_t vlan2)
{
    int rv;
    bcm_vlan_port_t vlan_port_1;
    bcm_vlan_port_t vlan_port_2;
    bcm_vlan_port_t_init(&vlan_port_1);
    bcm_vlan_port_t_init(&vlan_port_2);
    vlan_port_1.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port_2.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port_1.port = p1;
    vlan_port_1.match_vlan = vlan1;
    vlan_port_1.egress_vlan = vlan1;
    vlan_port_2.port = p2;
    vlan_port_2.match_vlan = vlan2;
    vlan_port_2.egress_vlan = vlan2;
    
    rv = bcm_vlan_port_create(0, &vlan_port_1);
    rv = bcm_vlan_port_create(0, &vlan_port_2);

    bcm_vswitch_cross_connect_t gports;
    bcm_vswitch_cross_connect_t_init(&gports);
    gports.port1= vlan_port_1.vlan_port_id;
    gports.port2= vlan_port_2.vlan_port_id;
    rv = bcm_vswitch_cross_connect_add(0, &gports);
}






