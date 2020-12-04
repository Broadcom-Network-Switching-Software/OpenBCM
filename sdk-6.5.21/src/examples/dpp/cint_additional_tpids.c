/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
* 
* File: cint_jer_additional_tpids.c
* Purpose: An example of Jericho additional TPID usage. 
*              The following CINT provides a calling sequence example to set additional TPIDs. 
*
* Calling sequence:
*
* Initialization:
*  1. Set ports to recognize single stag and single ctag frames.
*        - call port_tpid_init();
*        - call bcm_port_tpid_add();
*  2. Add Ports to VLAN x.
*        - call bcm_vlan_create();
*        - call bcm_vlan_port_add();
*  3. Add additional TPIDs.
*        - Call bcm_switch_tpid_add() with following criterias:
*          bcmTpidTypeOuter, bcmTpidTypeInner
*  4. Get the first outer and inner additional TPIDs.
*        - Call bcm_switch_tpid_get() with following criterias:
*          bcmTpidTypeOuter, bcmTpidTypeInner
*  5. Get all additional TPIDs.
*        - Call bcm_switch_tpid_get_all();
*  6. Delete additional TPIDs one by one.
*        - Call bcm_switch_tpid_delete() with following criterias:
*          bcmTpidTypeOuter, bcmTpidTypeInner
*  7. Delete all additional TPIDs.
*        - Call bcm_switch_tpid_delete_all();
*
* Clean up sequence:
*  1. Remove port from VLAN X.
*        - Call bcm_vlan_port_remove();
*  2. Destroy VLAN X.
*        - Call bcm_vlan_destroy()
*  3. Delete all additional TPIDs.
*        - Call additional_tpid_delete_all();
*
* Service Model:
*     Port 13        <-----------------  VLAN 100-----------> Port 14
*     TPID 0x8100 <-------------------------------------> TPID 0x8100
*     TPID 0x9100 <-------------------------------------> TPID 0x8100
*     TPID 0x8200 <-------------------------------------> TPID 0x8100
*     TPID 0x9200 <-------------------------------------> TPID 0x8100
*     TPID 0x8300 <-------------------------------------> TPID 0x8100
*     TPID 0x9300 <-------------------------------------> TPID 0x8100
*
* Traffic:
*  1. Port 13 <-VLAN 100-> Port 14
*        - From PON port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag tpid 0x8100, VID = 100
*        - From NNI port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag tpid 0x8100, VID = 100
*
*  2. Port 13 <-VLAN 100-> Port 14
*        - From PON port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag tpid 0x9100, VID = 100
*        - From NNI port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag tpid 0x9100, VID = 100
*
*  3. Port 13 <-VLAN 100-> Port 14
*        - From PON port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag tpid 0x8200, VID = 100
*        - From NNI port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag tpid 0x8200, VID = 100
*
*  4. Port 13 <-VLAN 100-> Port 14
*        - From PON port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag tpid 0x9200, VID = 100
*        - From NNI port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag tpid 0x9200, VID = 100
*
*  1. Port 13 <-VLAN 100-> Port 14
*        - From PON port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag tpid 0x8300, VID = 100
*        - From NNI port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag tpid 0x8300, VID = 100
*
*  1. Port 13 <-VLAN 100-> Port 14
*        - From PON port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag tpid 0x9300, VID = 100
*        - From NNI port:
*              -   ethernet header with any DA, SA 
*              -   VLAN tag: VLAN tag tpid 0x9300, VID = 100
*
* To Activate Above Settings Run:
*      BCM> cint examples/dpp/cint_port_tpid.c
*      BCM> cint examples/dpp/cint_jer_additional_tpids.c
*      BCM> cint
*      cint> additional_tpid_service(unit, port_1, port_2, vlan);
*      cint> additional_tpid_service_cleanup(unit, port_1, port_2);
*      cint> additional_tpid_add(unit);
*      cint> additional_tpid_get(unit);
*      cint> additional_tpid_get_all(unit);
*      cint> additional_tpid_delete(unit);
*      cint> additional_tpid_delete_all(unit);
*/

int is_advanced_vlan_translation_mode = 0;

uint16 no_tag = 0;
uint16 c_tag = 1;
uint16 s_tag = 2;
uint16 prio_tag = 3;
uint16 i_tag = 4;
uint16 c_c_tag = 5;
uint16 s_c_tag = 6;
uint16 prio_c_tag = 7;
uint16 c_s_tag = 9;
uint16 s_s_tag = 10;
uint16 prio_s_tag = 11;
int any_tag = 0xFFFFFFFF;

uint32 port_outer_tpid = 0x8100;
uint32 port_inner_tpid = 0x9100;
uint16 additional_outer_tpid[2] = {0x8200, 0x8300};
uint16 additional_inner_tpid[2] = {0x9200, 0x9300};
bcm_vlan_t service_vlan;

/* Initialize port TPID configuration */
int tpid_profile_init(int unit, bcm_port_t port, uint32 tpid_outer, uint32 tpid_inner)
{
    int rv = 0;    
    int index;
    bcm_port_tpid_class_t port_tpid_class;
    
    if (is_advanced_vlan_translation_mode) {
         /* remove old tpids on port */
        rv = bcm_port_tpid_delete_all(unit,port);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_tpid_delete_all $rv\n");
            return rv;
        }

        /* identify TPID on port */
        rv = bcm_port_tpid_add(unit, port, tpid_outer,0);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_tpid_add $rv\n");
            return rv;
        }
        
        /* identify TPID on port */
        rv = bcm_port_inner_tpid_set(unit, port, tpid_inner);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_inner_tpid_set $rv\n");
            return rv;
        }
                
        /* outer: TPID1, inner: ANY ==> S_tag */
        bcm_port_tpid_class_t_init(&port_tpid_class);
        port_tpid_class.port = port;
        port_tpid_class.tpid1 = tpid_outer;
        port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
        port_tpid_class.tag_format_class_id = s_tag;
        port_tpid_class.flags = 0;
        port_tpid_class.vlan_translation_action_id = 0;
        rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_tpid_class_set s-tag $rv\n");
            return rv;
        }

        /* outer: TPID2, inner: ANY ==> c_tag */
        bcm_port_tpid_class_t_init(&port_tpid_class);
        port_tpid_class.port = port;
        port_tpid_class.tpid1 = tpid_inner;
        port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
        port_tpid_class.tag_format_class_id = c_tag;
        port_tpid_class.flags = 0;
        port_tpid_class.vlan_translation_action_id = 0;
        rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_tpid_class_set c-tag $rv\n");
            return rv;
        }
    }else {
        /* set ports to identify double tags packets and treat packets with ctags to stag */
        port_tpid_init(port, 1, 1);
        port_tpid_info1.outer_tpids[0] = tpid_outer;
        port_tpid_info1.inner_tpids[0] = tpid_inner;
        rv = port_tpid_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, port_tpid_set $rv\n");
            return rv;
        }

        /* set inner port tpids to recognize single ctag frames */
        for(index = 0; index < port_tpid_info1.nof_inners; ++index){
            rv = bcm_port_tpid_add(unit, port_tpid_info1.port, port_tpid_info1.inner_tpids[index], 0);
            if (rv != BCM_E_NONE) {
                printf("Error, in bcm_port_tpid_add, tpid=%d, $rv\n", port_tpid_info1.inner_tpids[index]);
                return rv;
            }
        }
    }
    return rv;
}

/* Create VLAN service */
int additional_tpid_service(int unit, int port_1, int port_2, bcm_vlan_t vlan) {
    int rv = 0;
    bcm_pbmp_t pbmp,ubmp;

    is_advanced_vlan_translation_mode = soc_property_get(unit , "bcm886xx_vlan_translate_mode",0);

    tpid_profile_init(unit, port_1, port_outer_tpid, port_inner_tpid);
    tpid_profile_init(unit, port_2, port_outer_tpid, port_inner_tpid);

    rv = bcm_vlan_create(unit, vlan);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_create vlan %d $rv\n", vlan);
        return rv;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_PORT_ADD(pbmp, port_1);
    BCM_PBMP_PORT_ADD(pbmp, port_2);

    rv = bcm_vlan_port_add(unit, vlan, pbmp, ubmp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_add vlan %d $rv\n", vlan);
        return rv;
    }

    service_vlan = vlan;
    
    return rv;
}

/* Clean up service */
int additional_tpid_service_cleanup(int unit, int port_1, int port_2) {
    int rv = 0;
    bcm_pbmp_t pbmp;

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port_1);
    BCM_PBMP_PORT_ADD(pbmp, port_2);

    rv = bcm_vlan_port_remove(unit, service_vlan, pbmp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_add vlan %d $rv\n", vlan);
        return rv;
    }

    rv = bcm_vlan_destroy(unit, service_vlan);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_create vlan %d $rv\n", vlan);
        return rv;
    }
    
    return rv;
}

/* Add all 4 additional TPIDs */
int additional_tpid_add(int unit) {
    int rv = 0;
    bcm_switch_tpid_info_t tpid_info;
    int i = 0;
    int num_of_gl_tpids = 2;

    sal_memset(&tpid_info, 0, sizeof(tpid_info));
    for (i = 0; i < num_of_gl_tpids; i++) {        
        tpid_info.tpid_type = bcmTpidTypeOuter;
        tpid_info.tpid_value = additional_outer_tpid[i];
        rv = bcm_switch_tpid_add(unit, 0, &tpid_info);
        if (rv != BCM_E_NONE) {
            printf("bcm_switch_tpid_add failed! $rv\n");
            return rv;
        }

        tpid_info.tpid_type = bcmTpidTypeInner;
        tpid_info.tpid_value = additional_inner_tpid[i];
        rv = bcm_switch_tpid_add(unit, 0, &tpid_info);
        if (rv != BCM_E_NONE) {
            printf("bcm_switch_tpid_add failed! $rv\n");
            return rv;
        }
    }

    return rv;
}

/* Get the first outer and inner additional TPID */
int additional_tpid_get(int unit) {
    int rv = 0;
    bcm_switch_tpid_info_t tpid_info;
    int i = 0;
    int num_of_gl_tpids = 2;

    sal_memset(&tpid_info, 0, sizeof(tpid_info));
    tpid_info.tpid_type = bcmTpidTypeOuter;
    rv = bcm_switch_tpid_get(unit, &tpid_info);
    if (rv != BCM_E_NONE) {
        printf("bcm_switch_tpid_get failed! $rv\n");
        return rv;
    }

    if (tpid_info.tpid_value != additional_outer_tpid[0]) {
        printf("bcm_switch_tpid_get gets wrong outer TPID 0x%x\n", tpid_info.tpid_value);
        return BCM_E_FAIL;
    }

    sal_memset(&tpid_info, 0, sizeof(tpid_info));
    tpid_info.tpid_type = bcmTpidTypeInner;
    rv = bcm_switch_tpid_get(unit, &tpid_info);
    if (rv != BCM_E_NONE) {
        printf("bcm_switch_tpid_get failed! $rv\n");
        return rv;
    }

    if (tpid_info.tpid_value != additional_inner_tpid[0]) {
        printf("bcm_switch_tpid_get gets wrong inner TPID 0x%x\n", tpid_info.tpid_value);
        return BCM_E_FAIL;
    }

    return rv;
}

/* Get all 4 TPIDs */
int additional_tpid_get_all(int unit) {
    int rv = 0;
    bcm_switch_tpid_info_t tpid_info[4];
    int count;
    int i = 0;
    int num_of_gl_tpids = 2;

    sal_memset(&tpid_info, 0, sizeof(tpid_info));
    rv = bcm_switch_tpid_get_all(unit, 4, &tpid_info, &count);
    if (rv != BCM_E_NONE) {
        printf("bcm_switch_tpid_get_all failed! $rv\n");
        return rv;
    }

    if (count != 4){
        printf("bcm_switch_tpid_get_all gets wrong count %d\n", count);
        return BCM_E_FAIL;
    }

    if ((tpid_info[0].tpid_value != additional_outer_tpid[0]) || (tpid_info[0].tpid_type!= bcmTpidTypeOuter)) {
        printf("bcm_switch_tpid_get_all gets wrong 1st outer TPID 0x%x\n", tpid_info[0].tpid_value);
        return BCM_E_FAIL;
    }

    if ((tpid_info[1].tpid_value != additional_outer_tpid[1]) || (tpid_info[1].tpid_type!= bcmTpidTypeOuter)) {
        printf("bcm_switch_tpid_get_all gets wrong 2nd outer TPID 0x%x\n", tpid_info[1].tpid_value);
        return BCM_E_FAIL;
    }

    if ((tpid_info[2].tpid_value != additional_inner_tpid[0]) || (tpid_info[2].tpid_type!= bcmTpidTypeInner)) {
        printf("bcm_switch_tpid_get_all gets wrong 1st inner TPID 0x%x\n", tpid_info[2].tpid_value);
        return BCM_E_FAIL;
    }

    if ((tpid_info[3].tpid_value != additional_inner_tpid[1]) || (tpid_info[3].tpid_type!= bcmTpidTypeInner)) {
        printf("bcm_switch_tpid_get_all gets wrong 2nd inner TPID 0x%x\n", tpid_info[3].tpid_value);
        return BCM_E_FAIL;
    }

    return rv;
}

/* Delete 4 additionals one by one */
int additional_tpid_delete(int unit) {
    int rv = 0;
    bcm_switch_tpid_info_t tpid_info;
    int i = 0;
    int num_of_gl_tpids = 2;

    sal_memset(&tpid_info, 0, sizeof(tpid_info));
    for (i = 0; i < num_of_gl_tpids; i++) {        
        tpid_info.tpid_type = bcmTpidTypeOuter;
        tpid_info.tpid_value = additional_outer_tpid[i];
        rv = bcm_switch_tpid_delete(unit, &tpid_info);
        if (rv != BCM_E_NONE) {
            printf("additional_tpid_delete failed! $rv\n");
            return rv;
        }

        tpid_info.tpid_type = bcmTpidTypeInner;
        tpid_info.tpid_value = additional_inner_tpid[i];
        rv = bcm_switch_tpid_delete(unit, &tpid_info);
        if (rv != BCM_E_NONE) {
            printf("additional_tpid_delete failed! $rv\n");
            return rv;
        }
    }

    return rv;
}

/* Delete all 4 TPIDs once */
int additional_tpid_delete_all(int unit) {
    int rv = 0;
    
    rv = bcm_switch_tpid_delete_all(unit);
    if (rv != BCM_E_NONE) {
        printf("bcm_switch_tpid_delete_all failed! $rv\n");
        return rv;
    }
    
    return rv;
}

