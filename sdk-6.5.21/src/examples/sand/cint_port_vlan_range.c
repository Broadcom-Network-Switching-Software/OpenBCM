/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
* 
* File: cint_port_vlan_range.c
* Purpose: An example of how to match incoming VLAN using VLAN range of Petra and Arad.
*          The following CINT provides a calling sequence example to set VLAN range of port to match incoming VLAN, 
*          and bring up one main services 1:1 Model.
*
* Calling sequence:
*
* Initialization:
*  1. Initialize service models.
*  2. Set ports VLAN domain.
*        - call bcm_port_class_set()
*  1. Set all ports to recognize single stag, single ctag and s_c_tag frames.
*        - call port_tpid_init()
*        - call bcm_port_tpid_add()
*
* 1:1 Service:
* Set up sequence:
*  1. Add VLAN range info ports. Need to add VLAN range info before creating LIF.
*        - Call bcm_vlan_translate_action_range_add() with action bcmVlanActionCompressed.        
*  2. Create LIFs
*        - Call bcm_vlan_port_create() with following criterias:
*          BCM_VLAN_PORT_MATCH_PORT_VLAN, BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED
*  7. Cross connect the 2 LIFs
*        - Call bcm_vswitch_cross_connect_add()
* Clean up sequence:
*  1. Delete the cross connected LIFs.
*        - Call bcm_vswitch_cross_connect_delete()
*  2. Delete VLAN range info from ports
*        - Call bcm_vlan_translate_action_range_delete()       
*  3. Delete LIFs.
*        - Call bcm_vlan_port_destroy()
*
* Service Model:
* 1:1 Service:
*    Port_1  <-----------------------------  CrossConnect  ------------>  Port_2
*    SVLAN range 10~20  <----------------------------------------------> SVLAN range 30~40
*    SVLAN range 50~60 CVLAN range 70~80 <-----------------------------> SVLAN range 90~100 CVLAN range 110~120
*
* Traffic within VLAN range:
*  1. Port_1 SVLAN range 10~20 <-CrossConnect-> Port_2 SVLAN range 30~40
*        - From Port_1:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 10
*        - From Port_2:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 30
*        - From Port_1:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 15
*        - From Port_2:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 35
*        - From Port_1:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 20
*        - From Port_2:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 40
*
*  2. Port_1 SVLAN range 50~60 CVLAN range 70~80 <-CrossConnect-> Port_2 SVLAN range 90~100 CVLAN range 110~120
*        - From Port_1:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 50, VLAN tag type 0x9100, VID = 70
*        - From Port_2:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 90, VLAN tag type 0x9100, VID = 110
*        - From Port_1:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 55, VLAN tag type 0x9100, VID = 75
*        - From Port_2:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 95, VLAN tag type 0x9100, VID = 115
*        - From Port_1:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 60, VLAN tag type 0x9100, VID = 80
*        - From Port_2:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 100,VLAN tag type 0x9100, VID = 120
*
* Traffic out of VLAN range, can't be matched. All are dropped:
*  1. Port_1 Tunnel-ID 1000 SVLAN range 10~20 <-CrossConnect-> Port_2 SVLAN range 30~40
*        - From Port_1:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 5
*        - From Port_2:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 25
*        - From Port_1:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 25
*        - From Port_2:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 45
*
*  2. Port_1 SVLAN range 50~60 CVLAN range 70~80 <-CrossConnect-> Port_2 SVLAN range 90~100 CVLAN range 110~120
*        - From Port_1:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 45, VLAN tag type 0x9100, VID = 70
*        - From Port_2:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 85, VLAN tag type 0x9100, VID = 110
*        - From Port_1:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 50, VLAN tag type 0x9100, VID = 65
*        - From Port_2:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 90, VLAN tag type 0x9100, VID = 105
*        - From Port_1:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 45, VLAN tag type 0x9100, VID = 65
*        - From Port_2:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag type 0x8100, VID = 85, VLAN tag type 0x9100, VID = 105
*
* To Activate Above Settings Run:
*      BCM> cint examples/dpp/cint_port_tpid.c
*      BCM> cint examples/dpp/cint_port_vlan_range.c
*      BCM> cint
*      cint> vlan_range_init(unit, port_1, port_2);
*      cint> vlan_range_1_1_service(unit);
*      cint> vlan_range_1_1_service_cleanup(unit);
*
* To Test VLAN Range Settings Only Run:
*      cint> vlan_range_action_add(unit, service_index, port_index);
*      cint> vlan_range_action_get(unit, service_index, port_index);
*      cint> vlan_range_action_delete(unit, service_index, port_index);
*      cint> vlan_range_action_delete_all(unit);
*/

enum vlan_range_service_mode_e {
    stag_to_stag2,         /*SVLAN <-> SVLAN'*/
    s_c_tag_to_s2_c_tag    /*SVLAN + CVLAN <-> SVLAN' + CVLAN*/
};

struct port_info_s {
    bcm_gport_t port;
    bcm_gport_t lif_gport;
    bcm_vlan_t svlan_low;
    bcm_vlan_t svlan_high;
    bcm_vlan_t cvlan_low;
    bcm_vlan_t cvlan_high;
};

struct vlan_range_service_info_s {
    int service_mode;
    port_info_s port_info[2];
};

vlan_range_service_info_s vlan_range_service_info[2];

uint32 num_of_vlan_range_service;
int verbose = 0;

/*
 *Initialize the service models and set up PON application configurations.
 */
int vlan_range_init(int unit, bcm_port_t port_1, bcm_port_t port_2)
{
    int rv = 0;
    int index;
    uint8 avt_mode = 0;
    int device_is_jericho2;

    rv = is_device_jericho2(unit, &device_is_jericho2);

    /* port_1 SVLAN range 10~20 <-CrossConnect-> port_2 SVLAN range 30~40 */
    index = 0;
    vlan_range_service_info[index].service_mode = stag_to_stag2;
    vlan_range_service_info[index].port_info[0].port = port_1;
    vlan_range_service_info[index].port_info[0].svlan_low = 10;
    vlan_range_service_info[index].port_info[0].svlan_high = 20;
    vlan_range_service_info[index].port_info[0].cvlan_low = BCM_VLAN_INVALID;
    vlan_range_service_info[index].port_info[0].cvlan_high = BCM_VLAN_INVALID;

    vlan_range_service_info[index].port_info[1].port = port_2;
    vlan_range_service_info[index].port_info[1].svlan_low = 30;
    vlan_range_service_info[index].port_info[1].svlan_high = 40;
    vlan_range_service_info[index].port_info[1].cvlan_low = BCM_VLAN_INVALID;
    vlan_range_service_info[index].port_info[1].cvlan_high = BCM_VLAN_INVALID;

    /* port_1 SVLAN range 50~60 CVLAN range 70~80 <-CrossConnect-> port_2 SVLAN range 90~100 CVLAN range 110~120 */
    index++;
    vlan_range_service_info[index].service_mode = s_c_tag_to_s2_c_tag;
    
    vlan_range_service_info[index].port_info[0].port = port_1;
    vlan_range_service_info[index].port_info[0].svlan_low = 50;
    vlan_range_service_info[index].port_info[0].svlan_high = 60;
    vlan_range_service_info[index].port_info[0].cvlan_low = 70;
    vlan_range_service_info[index].port_info[0].cvlan_high = 80;

    vlan_range_service_info[index].port_info[1].port = port_2;
    vlan_range_service_info[index].port_info[1].svlan_low = 90;
    vlan_range_service_info[index].port_info[1].svlan_high = 100;
    vlan_range_service_info[index].port_info[1].cvlan_low = 110;
    vlan_range_service_info[index].port_info[1].cvlan_high = 120;

    num_of_vlan_range_service = index+1;

    /* Set VLAN domain of port_1 */
    bcm_port_class_set(unit, port_1, bcmPortClassId, port_1&0xFFF);
    
    /* Set VLAN domain of port_2 */
    bcm_port_class_set(unit, port_2, bcmPortClassId, port_2&0xFFF);
    if (!device_is_jericho2)
    {
        /* Set port_1 to identify double tags packets and treat packets with ctags to stag in Arad */
        port_tpid_init(port_1, 1, 1);
        rv = port_tpid_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, port_tpid_set\n");
            print rv;
            return rv;
        }

        avt_mode = soc_property_get(unit , "bcm886xx_vlan_translate_mode",0);
        if (!avt_mode) {
            /* Set inner tpids of port_1 to recognize single ctag frames */
            for(index = 0; index < port_tpid_info1.nof_inners; ++index){
                rv = bcm_port_tpid_add(unit, port_tpid_info1.port, port_tpid_info1.inner_tpids[index], 0);
                if (rv != BCM_E_NONE) {
                    printf("Error, in bcm_port_inner_tpid_set, tpid=%d, \n", port_tpid_info1.inner_tpids[index]);
                    return rv;
                }
            }
        }

        /* Set port_2 to identify double tags packets and treat packets with ctags to stag in Arad */
        port_tpid_init(port_2,1,1);
        rv = port_tpid_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, port_tpid_set\n");
            print rv;
            return rv;
        }

        if (!avt_mode) {
            /* Set inner tpids of port_2 to recognize single ctag frames */
            for(index = 0; index < port_tpid_info1.nof_inners; ++index){
                rv = bcm_port_tpid_add(unit, port_tpid_info1.port, port_tpid_info1.inner_tpids[index], 0);
                if (rv != BCM_E_NONE) {
                    printf("Error, in bcm_port_inner_tpid_set, tpid=%d, \n", port_tpid_info1.inner_tpids[index]);
                    return rv;
                }
            }
        }
    }
    return 0;
}

/*
 * Create LIF according to the LIF type.
 */
int vlan_range_lif_create(int unit, int service_index, int port_index)
{
    int rv;
    int service_mode;
    bcm_vlan_port_t vlan_port;
    bcm_vlan_action_set_t action;
    bcm_vlan_t vid;
    int device_is_jericho2;

    rv = is_device_jericho2(unit, &device_is_jericho2);

    service_mode = vlan_range_service_info[service_index].service_mode;

    bcm_vlan_port_t_init(&vlan_port);

    vlan_port.port = vlan_range_service_info[service_index].port_info[port_index].port;
    vlan_port.flags = device_is_jericho2?0:(BCM_VLAN_PORT_INNER_VLAN_PRESERVE | BCM_VLAN_PORT_OUTER_VLAN_PRESERVE);

    switch(service_mode) {
        case stag_to_stag2:
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
            vlan_port.match_vlan  = vlan_range_service_info[service_index].port_info[port_index].svlan_low;
            break;
        case s_c_tag_to_s2_c_tag:
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
            vlan_port.match_vlan  = vlan_range_service_info[service_index].port_info[port_index].svlan_low;
            vlan_port.match_inner_vlan  = vlan_range_service_info[service_index].port_info[port_index].cvlan_low;
            break;
        default:
            printf("ERR: nni_lif_create INVALID PARAMETER lif_type %d\n", service_mode);
            return BCM_E_PARAM;
    }
    
    if(verbose) {
        printf("nni_lif created:\n");
        print vlan_port;
    }

    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("nni_lif_create index failed! %s\n", bcm_errmsg(rv));
    }

    vlan_range_service_info[service_index].port_info[port_index].lif_gport = vlan_port.vlan_port_id;

    if(verbose) {
        print vlan_port.vlan_port_id;
    }


    return rv;
}

/*
 * Add VLAN range info to port.
 */
int vlan_range_action_add(int unit, int service_index, int port_index)
{
    int rv;
    bcm_port_t port;
    int service_mode;
    bcm_vlan_t outer_vlan_low;
    bcm_vlan_t outer_vlan_high;
    bcm_vlan_t inner_vlan_low;
    bcm_vlan_t inner_vlan_high;
    bcm_vlan_action_set_t action;

    service_mode = vlan_range_service_info[service_index].service_mode;

    port = vlan_range_service_info[service_index].port_info[port_index].port;
    outer_vlan_low = vlan_range_service_info[service_index].port_info[port_index].svlan_low;
    outer_vlan_high= vlan_range_service_info[service_index].port_info[port_index].svlan_high;
    inner_vlan_low = vlan_range_service_info[service_index].port_info[port_index].cvlan_low;
    inner_vlan_high= vlan_range_service_info[service_index].port_info[port_index].cvlan_high;

    bcm_vlan_action_set_t_init(&action);
    if (service_mode == stag_to_stag2)
    {
        action.ot_outer = bcmVlanActionCompressed;
        action.new_outer_vlan = outer_vlan_low;
    }
    else if (service_mode == s_c_tag_to_s2_c_tag)
    {
        action.dt_outer = bcmVlanActionCompressed;
        action.new_outer_vlan = outer_vlan_low;
        action.dt_inner = bcmVlanActionCompressed;
        action.new_inner_vlan = inner_vlan_low;
    
    }
    
    rv = bcm_vlan_translate_action_range_add(unit, port, outer_vlan_low, outer_vlan_high, inner_vlan_low, inner_vlan_high, &action);
    if (rv != BCM_E_NONE)
    {
        printf("error, bcm_vlan_translate_action_range_add!\n");
        print rv;
        return rv;
    }

    return rv;
}

/*
 * Get VLAN range info from port.
 */
int vlan_range_action_get(int unit, int service_index, int port_index)
{
    int rv;
    bcm_port_t port;
    int service_mode;
    bcm_vlan_t outer_vlan_low;
    bcm_vlan_t outer_vlan_high;
    bcm_vlan_t inner_vlan_low;
    bcm_vlan_t inner_vlan_high;
    bcm_vlan_action_set_t action;

    service_mode = vlan_range_service_info[service_index].service_mode;

    port = vlan_range_service_info[service_index].port_info[port_index].port;
    outer_vlan_low = vlan_range_service_info[service_index].port_info[port_index].svlan_low;
    outer_vlan_high= vlan_range_service_info[service_index].port_info[port_index].svlan_high;
    inner_vlan_low = vlan_range_service_info[service_index].port_info[port_index].cvlan_low;
    inner_vlan_high= vlan_range_service_info[service_index].port_info[port_index].cvlan_high;

    bcm_vlan_action_set_t_init(&action);
    if (service_mode == stag_to_stag2)
    {
        action.ot_outer = bcmVlanActionCompressed;
        action.new_outer_vlan = outer_vlan_low;
    }
    else if (service_mode == s_c_tag_to_s2_c_tag)
    {
        action.dt_outer = bcmVlanActionCompressed;
        action.new_outer_vlan = outer_vlan_low;
        action.dt_inner = bcmVlanActionCompressed;
        action.new_inner_vlan = inner_vlan_low;    
    }

    bcm_vlan_action_set_t_init(&action);
    rv = bcm_vlan_translate_action_range_get(unit, port, outer_vlan_low, outer_vlan_high, inner_vlan_low, inner_vlan_high, &action);
    if (rv != BCM_E_NONE)
    {
        printf("error, bcm_vlan_translate_action_range_get!\n");
        print rv;
        return rv;
    }

    if ((service_mode == stag_to_stag2) && 
        ((action.ot_outer != bcmVlanActionCompressed) || 
        (action.new_outer_vlan != outer_vlan_low)))
    {
        printf("Err, VLAN range action not correct!\n");        
        print action;
        return BCM_E_FAIL;
    }
    else if ((service_mode == s_c_tag_to_s2_c_tag) && 
             ((action.dt_outer != bcmVlanActionCompressed) || 
             (action.new_outer_vlan != outer_vlan_low) || 
             (action.dt_inner != bcmVlanActionCompressed) || 
             (action.new_inner_vlan != inner_vlan_low)))
    {        
        printf("Err, VLAN range action not correct!\n");        
        print action;
        return BCM_E_FAIL;
    }

    print action;

    return;
}

/*
 * Delete VLAN range info from port.
 */
int vlan_range_action_delete(int unit, int service_index, int port_index)
{
    int rv;
    bcm_port_t port;
    bcm_vlan_t outer_vlan_low;
    bcm_vlan_t outer_vlan_high;
    bcm_vlan_t inner_vlan_low;
    bcm_vlan_t inner_vlan_high;

    port = vlan_range_service_info[service_index].port_info[port_index].port;
    outer_vlan_low = vlan_range_service_info[service_index].port_info[port_index].svlan_low;
    outer_vlan_high= vlan_range_service_info[service_index].port_info[port_index].svlan_high;
    inner_vlan_low = vlan_range_service_info[service_index].port_info[port_index].cvlan_low;
    inner_vlan_high= vlan_range_service_info[service_index].port_info[port_index].cvlan_high;


    rv = bcm_vlan_translate_action_range_delete(unit, port, outer_vlan_low, outer_vlan_high, inner_vlan_low, inner_vlan_high);
    if (rv != BCM_E_NONE)
    {
        printf("error, bcm_vlan_translate_action_range_delete!\n");
        print rv;
        return rv;
    }

    return rv;
}

/*
 * Delete all VLAN range info from port.
 */
int vlan_range_action_delete_all(int unit)
{
    int rv;

    rv = bcm_vlan_translate_action_range_delete_all(unit);
    if (rv != BCM_E_NONE)
    {
        printf("error, bcm_vlan_translate_action_range_add!\n");
        print rv;
        return rv;
    }

    return rv;
}

/*
 * Set up 1:1 sercies, using port cross connect. Match VLAN using VLAN range.
 */
int vlan_range_1_1_service(int unit)
{
    int rv;
    int service_index, port_index;
    bcm_vswitch_cross_connect_t gports;

    for (service_index = 0; service_index < num_of_vlan_range_service; service_index++)
    {
        for (port_index = 0; port_index < 2; port_index++)
        {
            /* Add port VLAN range action */
            vlan_range_action_add(unit, service_index, port_index);
        
            /* Create LIF */
            vlan_range_lif_create(unit, service_index, port_index);
        }
        
        /* Cross connect the 2 LIFs */
        bcm_vswitch_cross_connect_t_init(&gports);
        gports.port1 = vlan_range_service_info[service_index].port_info[0].lif_gport;
        gports.port2 = vlan_range_service_info[service_index].port_info[1].lif_gport;
        rv = bcm_vswitch_cross_connect_add(unit, &gports);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vswitch_cross_connect_add\n");
            print rv;
            return rv;
        }
    }

    return rv;
}

/*
 * clean up the configurations of 1:1 sercies. Clear VLAN range info.
 */
int vlan_range_1_1_service_cleanup(int unit)
{
    int rv;
    int service_index, port_index;
    bcm_vswitch_cross_connect_t gports;

    for (service_index = 0; service_index < num_of_vlan_range_service; service_index++)
    {
        /* Delete the cross connected LIFs */
        gports.port1 = vlan_range_service_info[service_index].port_info[0].lif_gport;
        gports.port2 = vlan_range_service_info[service_index].port_info[1].lif_gport;
        rv = bcm_vswitch_cross_connect_delete(unit, &gports);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vswitch_cross_connect_delete\n");
            print rv;
            return rv;
        }

        for (port_index = 0; port_index < 2; port_index++)
        {
            /* Delete PON LIF VLAN range action */
            vlan_range_action_delete(unit, service_index, port_index);
            
            /* Delete PON LIF */
            rv = bcm_vlan_port_destroy(unit, vlan_range_service_info[service_index].port_info[port_index].lif_gport);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_vlan_port_destroy \n");
                return rv;
            }
        }
    }
      
    return rv;        
}

void vlan_range_run_with_default()
{
    int unit = 0;
    int port_1 = 1;
    int port_2 = 3;
    
    vlan_range_init(unit, port_1, port_2);
    vlan_range_1_1_service(unit);    
}

