/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~VLAN Translation~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_fap_vlan_translation.c
 * Purpose: Example for utilizing the VLAN Translation APIs
 *
 * A VLAN action modifies the VLAN tag(s) attributes of the packet according to the packet 
 * VLAN tag structure. You can configure the default VLAN action for all ports at ingress 
 * and at egress. A lookup key with the Port-Class and the Packet VLAN(s) is built afterwards. 
 * If found, the keys lookup retrieves a VLAN action. This VLAN action is applied to the packet, 
 * and the default VLAN action is not.
 * 
 * CINT usage: 
 *  -   Modifies the port class of the port
 *  -   At ingress (and then at egress), sets the default VLAN action and then provides an 
 *      example of lookup entry. The entry is for double-tagged lookups.
 *
 * To Activate Above Settings Run:    
 *      BCM> cint examples/dpp/cint_port_tpid.c
 *      BCM> cint examples/dpp/cint_fap_vlan_translation.c
 *      BCM> cint
 *      cint> run();
 * 1.  run();  
 *     Initalize basic vlan briding and basic vlan translation case  
 *     Traffics 
 *         ? port_3, (0x8100,2)  --> port_1, (0x9100,3,0x8100,2)
 * 2. vlan_translation_run_1(unit, 10, 20, 30);
 *     vlan translation case 1 for s_tag at ingress rx-pp and c_tag at egress rx-pp
 *     Traffics
 *         ? port_3, (0x8100,3) --> port_1, (0x8100,20,0x9100,30)
 * OR
 * vlan_translation_run_2(unit, 10, 20);
 *     vlan translation case 2 for s_tag at ingress rx-pp and s_tag at egress rx-pp
 *     Traffics
 *         ? port_3, (0x8100,3) --> port_1, (0x8100,10,0x9100,20)
 * OR
 * vlan_translation_run_3(unit, 10, 20);
 *     vlan translation case 3 for s_tag at ingress rx-pp and untag at egress rx-pp
 *     Traffics
 *         ? port_3, (0x8100,3) --> port_1, (0x8100,10,0x9100,20)
 * AND
 * vlan_translation_stop(unit, 3, 3);
 *     to remove vlan translations on vlan 3 that were configured by vlan_translation_run_1/2/3
 *
 * OR
 * vlan_translation_run_4(0unit 10, 20, 30);
 *     vlan translation case 4 for s_c_tag at ingress rx-pp and s_tag at egress rx-pp
 *     Traffics
 *         ? port_3, (0x8100,3,0x9100,10) --> port_1, (0x8100,20,0x9100,30)
 * OR
 * vlan_translation_run_5(unit, 10, 20, 30);
 *     vlan translation case 5 for s_c_tag at ingress rx-pp and c_tag at egress rx-pp
 *     Traffics
 *         ? port_3, (0x8100,3,0x9100,10) --> port_1, (0x8100,20,0x9100,30)
 * OR
 * vlan_translation_run_6(unit, 10, 20, 20);
 *     vlan translation case 6 for s_c_tag at ingress rx-pp and s_c_c_tag at egress
 *     Traffics
 *         ? port_3, (0x8100,3,0x9100,10) --> port_1, (0x8100,20,0x9100,20,0x9100,10)
 * OR
 * vlan_translation_run_7(unit, 10, 20, 30);
 *     vlan translation case 7 for s_c_tag at ingress rx-pp and s_c_tag at egress rx_pp
 *     Traffics
 *         ? port_3, (0x8100,3,0x9100,10) --> port_1, (0x8100,20,0x9100,30,0x9100,10)
 * AND 
 * vlan_translation_stop_1(unit, 3, 10);
 *     to remove vlan translations on AC (3,10) that were configured by vlan_translation_run_4/5/6/7
 */


/*
 * Change the Port class (both ingress and egress) for a port.
 * All the VLAN actions defined for this port class will not 
 * modify the other port behaviours.
 */
struct vlan_translation_info_s {
  int     port1; 
  int     port2; 
  int     vlan1; 
  int     vlan2; 
  int     port_class;
};

vlan_translation_info_s vlan_translation_info;

void
vlan_translation_info_init (int port1, int port2, int vlan1, int vlan2, int port_class) {
    vlan_translation_info.port1 = port1;
    vlan_translation_info.port2 = port2;
    vlan_translation_info.vlan1 = vlan1;
    vlan_translation_info.vlan2 = vlan2;
    vlan_translation_info.port_class = port_class;
}

/* 
 * A new port class creation (i.e., when the port class had no ports before)
 * implies a long API run due to HW architecture 
 */
int 
vlan_translation_run(int unit,int replace_example) 
{
    bcm_pbmp_t pbmp,upbmp,rb, rub;
    int rv;
    bcm_vlan_action_set_t action;
    bcm_gport_t gport;
    
    port_tpid_init(vlan_translation_info.port1,1,1);
    rv = port_tpid_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, port_tpid_set\n");
        return rv;
    }
    port_tpid_init(vlan_translation_info.port2,1,1);
    rv = port_tpid_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, port_tpid_set\n");
        return rv;
    }
    
    rv = bcm_vlan_create(unit,vlan_translation_info.vlan1);
    if (rv != BCM_E_NONE) {
       printf("Error, in bcm_vlan_create\n");
       return rv;
    }
    
    BCM_PBMP_PORT_ADD(pbmp,vlan_translation_info.port1);
    BCM_PBMP_PORT_ADD(pbmp,vlan_translation_info.port2);
    BCM_PBMP_CLEAR(upbmp);
    
    rv = bcm_vlan_port_add(unit, vlan_translation_info.vlan1, pbmp, upbmp);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_add\n");
        return rv;
    }
    
    rv = bcm_vlan_create(unit,vlan_translation_info.vlan2);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create\n");
        return rv;
    }
    
    rv = bcm_vlan_port_add(unit, vlan_translation_info.vlan2, pbmp, upbmp);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_add\n");
        return rv;
    }  
    
    rv = bcm_vlan_port_get(unit, vlan_translation_info.vlan1, &rb, &rub);
	if (rv != BCM_E_NONE) {
	printf("Error, vlan_port_get\n");
	return rv;
	}
    
    BCM_GPORT_LOCAL_SET(gport,vlan_translation_info.port2);
    
    /*
     * The portclassId of vlan_translation_info.port2 is vlan_translation_info.port_class.
     */
    rv = bcm_port_class_set(unit, gport, bcmPortClassId, vlan_translation_info.port_class);
	if (rv != BCM_E_NONE) {
	printf("Error, port_class_set\n");
	return rv;
	}

    /*
     * The portclassId of vlan_translation_info.port1 is vlan_translation_info.port1.
     */	
	rv = bcm_port_class_set(unit, vlan_translation_info.port1, bcmPortClassId, vlan_translation_info.port1);
	if (rv != BCM_E_NONE) {
	    printf("Error, port_class_set\n");
	    return rv;
	}
    
    /*
     * Ingress action example: Add inner vlan
     */
    bcm_vlan_action_set_t_init(&action);
    action.ot_outer = bcmVlanActionNone;
    action.ot_inner = bcmVlanActionAdd;
    action.new_inner_vlan = vlan_translation_info.vlan2;
    
    rv = bcm_vlan_translate_action_create(unit, gport, bcmVlanTranslateKeyPortOuter, vlan_translation_info.vlan1, BCM_VLAN_NONE, &action);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_translate_action_create\n");
        return rv;
    }
    
    if (replace_example) {
        /* 
         * Egress action example: Replace between outer and inner vlan.
         */
        bcm_vlan_action_set_t_init(&action);
        action.dt_outer = bcmVlanActionCopy;
        action.dt_inner = bcmVlanActionCopy;
        action.outer_tpid_action = bcmVlanTpidActionModify;
        action.inner_tpid_action = bcmVlanTpidActionModify;
        action.outer_tpid = 0x9100;
        action.inner_tpid = 0x8100;
    }
    
    rv = bcm_vlan_translate_egress_action_add(unit, vlan_translation_info.port1, vlan_translation_info.vlan1, BCM_VLAN_NONE, &action);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_translate_egress_action_add\n");
        return rv;
    }
    
    /*
     * Expected results upon traffic: Send packets from port1 to port2. Ethernet packet include one outer VLAN with VLAN1 (=2). Expected two VLAN structures with VLAN2 (=3), VLAN1(=2).
     */
    
    return rv;     
}

int run () {
    vlan_translation_info_init (1, 3, 2, 3 , 0);
    return vlan_translation_run(unit,1);
}

/* 
 * Vlan translation in vlan_translation_info.vlan2
 * This function must be called after vlan_translation_run
 * ingress - s tag, remove outer vlan and add inner vlan, c_tag
 * egress - c-tag, add outer vlan and replace inner vlan, s_c_tag
 */
int 
vlan_translation_run_1(int unit, int vlan_0, int vlan_1, int vlan_2) 
{
  int rv;
  bcm_vlan_action_set_t action;
  bcm_gport_t gport;

  /*
   * In order to classify the single inner tag in the egress, the egress port dtag mode is set to EXTERNAL 
   * and the egress port outer TPID is set to 0x9100.
   */	
  rv = bcm_port_dtag_mode_set(unit, vlan_translation_info.port1, BCM_PORT_DTAG_MODE_EXTERNAL);
  if (rv != BCM_E_NONE) {
      printf("Error, bcm_port_dtag_mode_set\n");
      return rv;
  }
  
  port_tpid_init_cep_port_example(vlan_translation_info.port1,1,0);   
  rv = port_tpid_set(unit);
  if (rv != BCM_E_NONE) {
      printf("Error, port_tpid_set\n");
      return rv;
  }
  
  BCM_GPORT_LOCAL_SET(gport,vlan_translation_info.port2);

  /*
   * Ingress action example: remove outer vlan and add inner tag  
   */
  bcm_vlan_action_set_t_init(&action);
  action.ot_outer = bcmVlanActionDelete;
  action.ot_inner = bcmVlanActionAdd;
  action.new_inner_vlan = vlan_0;
  action.outer_tpid_action = bcmVlanTpidActionModify;
  action.inner_tpid_action = bcmVlanTpidActionModify;
  action.outer_tpid = 0x9100;
  action.inner_tpid = 0x8100;
  
	
  rv = bcm_vlan_translate_action_create(unit, gport, bcmVlanTranslateKeyPortOuter, vlan_translation_info.vlan2, BCM_VLAN_NONE, &action);
  if (rv != BCM_E_NONE) {
    	printf("Error, in bcm_vlan_translate_action_create\n");
    	return rv;
  }
	
  /* 
   * Egress action example: add outer tag and replace inner tag.
   */
  bcm_vlan_action_set_t_init(&action);
  action.it_outer = bcmVlanActionAdd;
  action.it_inner = bcmVlanActionReplace;
  action.new_outer_vlan = vlan_1;
  action.new_inner_vlan = vlan_2;
  
  /*
   * In case of Port CEP, VLAN-Domain x Out-VSI x VLAN is the key in ESEM lookup.
   * In bcm_vlan_translate_egress_action_add input is port class and not port. 
   * Since in this case port_class equals port1 then port input port1 is OK.
   */
  rv = bcm_vlan_translate_egress_action_add(unit, vlan_translation_info.port1, vlan_translation_info.vlan2, vlan_0, &action);
  if (rv != BCM_E_NONE) {
    printf("Error, in bcm_vlan_translate_egress_action_add\n");
    return rv;
  }

  return rv; 	
}

 /* 
 * Vlan translation in vlan_translation_info.vlan2
 * This function must be called after vlan_translation_run
 * ingress - s tag, replace outer vlan, s_tag
 * egress - s_tag, add inner vlan, s_c_tag
 */
int 
vlan_translation_run_2(int unit, int vlan_0, int vlan_1) 
{
  int rv;
  bcm_vlan_action_set_t action;
  bcm_gport_t gport;

  rv = bcm_port_dtag_mode_set(unit, vlan_translation_info.port1, BCM_PORT_DTAG_MODE_INTERNAL);
  if (rv != BCM_E_NONE) {
      printf("Error, bcm_port_dtag_mode_set\n");
      return rv;
  }
  
  port_tpid_init(vlan_translation_info.port1,1,1);
    
  rv = port_tpid_set(unit);
  if (rv != BCM_E_NONE) {
      printf("Error, port_tpid_set\n");
      return rv;
  }
  BCM_GPORT_LOCAL_SET(gport,vlan_translation_info.port2);

  /*
   * Ingress action example: replace outer vlan 
   */
  bcm_vlan_action_set_t_init(&action);
  action.ot_outer = bcmVlanActionReplace;
  action.new_outer_vlan = vlan_0;
	
  rv = bcm_vlan_translate_action_create(unit, gport, bcmVlanTranslateKeyPortOuter, vlan_translation_info.vlan2, BCM_VLAN_NONE, &action);
  if (rv != BCM_E_NONE) {
    	printf("Error, in bcm_vlan_translate_action_create\n");
    	return rv;
  }
	
  /* 
   * Egress action example: add inner vlan.
   */
  bcm_vlan_action_set_t_init(&action);
  action.ot_outer = bcmVlanActionNone;
  action.ot_inner = bcmVlanActionAdd;
  action.new_inner_vlan= vlan_1;
  
  rv = bcm_vlan_translate_egress_action_add(unit, vlan_translation_info.port1, vlan_translation_info.vlan2, BCM_VLAN_NONE, &action);
  if (rv != BCM_E_NONE) {
    	printf("Error, in bcm_vlan_translate_egress_action_add\n");
    	return rv;
  }

  return rv; 	
}

/* 
 * Vlan translation in vlan_translation_info.vlan2
 * This function must be called after vlan_translation_run
 * ingress - s tag, remove outer vlan, untag
 * egress - untag, add outer and inner vlan, s_c_tag
 */
int 
vlan_translation_run_3(int unit, int vlan_0, int vlan_1) 
{
  int rv;
  bcm_vlan_action_set_t action;
  bcm_gport_t gport;

  BCM_GPORT_LOCAL_SET(gport,vlan_translation_info.port2);
  
  /*
   * Ingress action example: remove outer vlan  
   */
  bcm_vlan_action_set_t_init(&action);
  action.ot_outer = bcmVlanActionDelete;
	
  rv = bcm_vlan_translate_action_create(unit, gport, bcmVlanTranslateKeyPortOuter, vlan_translation_info.vlan2, BCM_VLAN_NONE, &action);
  if (rv != BCM_E_NONE) {
    printf("Error, in bcm_vlan_translate_action_create\n");
    return rv;
  }
	
  /* 
   * Egress action example: add outer and inner vlan.
   */
  bcm_vlan_action_set_t_init(&action);
  action.ut_outer= bcmVlanActionAdd;
  action.ut_inner = bcmVlanActionAdd;
  action.new_outer_vlan= vlan_0;
  action.new_inner_vlan= vlan_1;
  
  rv = bcm_vlan_translate_egress_action_add(unit, vlan_translation_info.port1, vlan_translation_info.vlan2, BCM_VLAN_NONE, &action);
  if (rv != BCM_E_NONE) {
    printf("Error, in bcm_vlan_translate_egress_action_add\n");
    return rv;
  }

  return rv; 	
}

/* 
 * Delete vlan translation entries created by vlan_translation_run_1/2/3
 * port - physical port
 */
int 
vlan_translation_stop(int unit, int port, int vlan) 
{
  int rv;
  bcm_vlan_action_set_t action;
  bcm_gport_t gport;
	
  BCM_GPORT_LOCAL_SET(gport,port);

  bcm_vlan_action_set_t_init(&action);
  rv = bcm_vlan_translate_action_get(unit, gport, bcmVlanTranslateKeyPortOuter, vlan, BCM_VLAN_NONE, &action);
  if (rv != BCM_E_NONE) {
    printf("Error, in bcm_vlan_translate_action_get\n");
    return rv;
  }
	
  rv = bcm_vlan_translate_action_delete(unit, gport, bcmVlanTranslateKeyPortOuter, vlan, BCM_VLAN_NONE);
  if (rv != BCM_E_NONE) {
    printf("Error, in bcm_vlan_translate_action_delete\n");
    return rv;
  }
	
  bcm_vlan_action_set_t_init(&action);
  rv = bcm_vlan_translate_egress_action_get(unit, vlan_translation_info.port1, vlan, BCM_VLAN_NONE, &action);
  if (rv != BCM_E_NONE) {
    printf("Error, in bcm_vlan_translate_egress_action_add\n");
    return rv;
  }

  rv = bcm_vlan_translate_egress_action_delete(unit, vlan_translation_info.port1, vlan, BCM_VLAN_NONE);
  if (rv != BCM_E_NONE) {
    printf("Error, in bcm_vlan_translate_egress_action_add\n");
    return rv;
  }

  return rv; 	
}


/* 
 * Vlan translation in vlan_translation_info.vlan2
 * This function must be called after vlan_translation_run
 * ingress - s_c tag (vlan2, c_tag), remove inner vlan, s_tag
 * egress - s_tag, replace outer tag and add inner vlan, s_c_tag
 */
int 
vlan_translation_run_4(int unit, int c_tag, int vlan_0, int vlan_1) 
{
  int rv;
  bcm_vlan_action_set_t action;
  bcm_gport_t gport;

  BCM_GPORT_LOCAL_SET(gport,vlan_translation_info.port2);

  /*
   * Ingress action example: remove inner vlan  
   */
  bcm_vlan_action_set_t_init(&action);
  action.dt_outer = bcmVlanActionNone;
  action.dt_inner = bcmVlanActionDelete;
	
  rv = bcm_vlan_translate_action_create(unit, gport, bcmVlanTranslateKeyPortDouble, vlan_translation_info.vlan2, c_tag, &action);
  if (rv != BCM_E_NONE) {
    	printf("Error, in bcm_vlan_translate_action_create\n");
    	return rv;
  }
	
  /* 
   * Egress action example: replace outer tag and add  inner vlan
   */
  bcm_vlan_action_set_t_init(&action);
  action.ot_outer = bcmVlanActionReplace;
  action.ot_inner = bcmVlanActionAdd;
  action.new_outer_vlan= vlan_0;
  action.new_inner_vlan= vlan_1;
  
  rv = bcm_vlan_translate_egress_action_add(unit, vlan_translation_info.port1, vlan_translation_info.vlan2, BCM_VLAN_NONE, &action);
  if (rv != BCM_E_NONE) {
    	printf("Error, in bcm_vlan_translate_egress_action_add\n");
    	return rv;
  }
  
  return rv; 	
}

/* 
 * Vlan translation in vlan_translation_info.vlan2
 * This function must be called after vlan_translation_run
 * ingress - s_c tag (vlan2, c_tag), remove outer vlan, c_tag
 * egress - c_tag, add outer tag and replace inner vlan, s_c_tag
 */
int 
vlan_translation_run_5(int unit, int c_tag, int vlan_0, int vlan_1) 
{
  int rv;
  bcm_vlan_action_set_t action;
  bcm_gport_t gport;

  /*
   * In order to classify the single inner tag in the egress, the egress port dtag mode is set to EXTERNAL 
   * and the egress port outer TPID is set to 0x9100.
   */	
  bcm_port_dtag_mode_set(unit, vlan_translation_info.port1, BCM_PORT_DTAG_MODE_EXTERNAL);
  if (rv != BCM_E_NONE) {
      printf("Error, bcm_port_dtag_mode_set\n");
      return rv;
  }
  
  port_tpid_init_cep_port_example(vlan_translation_info.port1,1,0);
  
  rv = port_tpid_set(unit);
  if (rv != BCM_E_NONE) {
      printf("Error, port_tpid_set\n");
      return rv;
  }

  BCM_GPORT_LOCAL_SET(gport,vlan_translation_info.port2);

  /*
   * Ingress action example: remove outer vlan  
   */
  bcm_vlan_action_set_t_init(&action);
  action.dt_outer = bcmVlanActionDelete;
  action.dt_inner = bcmVlanActionNone;
	
  rv = bcm_vlan_translate_action_create(unit, gport, bcmVlanTranslateKeyPortDouble, vlan_translation_info.vlan2, c_tag, &action);
  if (rv != BCM_E_NONE) {
    	printf("Error, in bcm_vlan_translate_action_create\n");
    	return rv;
  }
	
  /* 
   * Egress action example: add outer tag and add replace  inner vlan
   */
  bcm_vlan_action_set_t_init(&action);
  action.it_outer = bcmVlanActionAdd;
  action.it_inner = bcmVlanActionReplace;
  action.new_outer_vlan= vlan_0;
  action.new_inner_vlan= vlan_1;
  
  rv = bcm_vlan_translate_egress_action_add(unit, vlan_translation_info.port1, vlan_translation_info.vlan2, BCM_VLAN_NONE, &action);
  if (rv != BCM_E_NONE) {
    	printf("Error, in bcm_vlan_translate_egress_action_add\n");
    	return rv;
  }
  
  return rv; 	
}

/* 
 * Vlan translation in vlan_translation_info.vlan2
 * This function must be called after vlan_translation_run
 * ingress - s_c tag (vlan2, c_tag), replace outer vlan and add inner tag, s_c_c_tag
 * vlan_0 == vlan_1
 * egress - no egress action
 */
int 
vlan_translation_run_6(int unit, int c_tag, int vlan_0, int vlan_1) 
{
  int rv;
  bcm_vlan_action_set_t action;
  bcm_gport_t gport;


  rv = bcm_port_dtag_mode_set(unit, vlan_translation_info.port1, BCM_PORT_DTAG_MODE_INTERNAL);
  if (rv != BCM_E_NONE) {
      printf("Error, bcm_port_dtag_mode_set\n");
      return rv;
  }
  
  port_tpid_init(vlan_translation_info.port1,1,1);
 
  rv = port_tpid_set(unit);
  if (rv != BCM_E_NONE) {
      printf("Error, port_tpid_set\n");
      return rv;
  }
  
  BCM_GPORT_LOCAL_SET(gport,vlan_translation_info.port2);

  /*
   * Ingress action example: replace outer vlan  
   */
  bcm_vlan_action_set_t_init(&action);
  action.dt_outer = bcmVlanActionReplace;
  action.dt_inner = bcmVlanActionAdd;
  action.new_outer_vlan = vlan_0;
  action.new_inner_vlan = vlan_0;
	
  rv = bcm_vlan_translate_action_create(unit, gport, bcmVlanTranslateKeyPortDouble, vlan_translation_info.vlan2, c_tag, &action);
  if (rv != BCM_E_NONE) {
    	printf("Error, in bcm_vlan_translate_action_create\n");
    	return rv;
  }
  
  return rv; 	
}

/* 
 * Vlan translation in vlan_translation_info.vlan2
 * This function must be called after vlan_translation_run
 * ingress - s_c tag (vlan2, c_tag), replace outer vlan, s_c_tag
 * egress - s_c_tag, add inner vlan, s_c_c_tag
 */
int 
vlan_translation_run_7(int unit, int c_tag, int vlan_0, int vlan_1) 
{
  int rv;
  bcm_vlan_action_set_t action;
  bcm_gport_t gport;

  BCM_GPORT_LOCAL_SET(gport,vlan_translation_info.port2);

  /*
   * Ingress action example: replace outer vlan  
   */
  bcm_vlan_action_set_t_init(&action);
  action.dt_outer = bcmVlanActionReplace;
  action.dt_inner = bcmVlanActionNone;
  action.new_outer_vlan = vlan_0;
	
  rv = bcm_vlan_translate_action_create(unit, gport, bcmVlanTranslateKeyPortDouble, vlan_translation_info.vlan2, c_tag, &action);
  if (rv != BCM_E_NONE) {
    	printf("Error, in bcm_vlan_translate_action_create\n");
    	return rv;
  }
	
  /* 
   * Egress action example: add inner tag and add replace  inner vlan
   */
  bcm_vlan_action_set_t_init(&action);
  action.dt_outer = bcmVlanActionNone;
  action.dt_inner = bcmVlanActionAdd;
  action.new_inner_vlan= vlan_1;
  
  rv = bcm_vlan_translate_egress_action_add(unit, vlan_translation_info.port1, vlan_translation_info.vlan2, BCM_VLAN_NONE, &action);
  if (rv != BCM_E_NONE) {
    	printf("Error, in bcm_vlan_translate_egress_action_add\n");
    	return rv;
  }
  
  return rv; 	
}

/* 
 * Delete vlan translation entries created by vlan_translation_run_4/5/6/7
 * Ingress entry based on (vlan_2, c_tag)
 * port - physical port
 */
int 
vlan_translation_stop_1(int unit, int port, int c_tag) 
{
  int rv;
  bcm_vlan_action_set_t action;
  bcm_gport_t gport;
	
  BCM_GPORT_LOCAL_SET(gport,port);

  bcm_vlan_action_set_t_init(&action);
  rv = bcm_vlan_translate_action_get(unit, gport, bcmVlanTranslateKeyPortDouble, vlan_translation_info.vlan2, c_tag, &action);
  if (rv != BCM_E_NONE) {
    printf("Error, in bcm_vlan_translate_action_get\n");
    return rv;
  }
	
  rv = bcm_vlan_translate_action_delete(unit, gport, bcmVlanTranslateKeyPortDouble, vlan_translation_info.vlan2, c_tag);
  if (rv != BCM_E_NONE) {
    printf("Error, in bcm_vlan_translate_action_delete\n");
    return rv;
  }
	
  bcm_vlan_action_set_t_init(&action);
  rv = bcm_vlan_translate_egress_action_get(unit, vlan_translation_info.port1, vlan_translation_info.vlan2, BCM_VLAN_NONE, &action);
  if (rv == BCM_E_EXISTS) {
    printf("Egress VLAN translation non-exist\n");
    return rv;
  }
  	
  if (rv != BCM_E_NONE) {
    printf("Error, in bcm_vlan_translate_egress_action_get\n");
    return rv;
  }

  rv = bcm_vlan_translate_egress_action_delete(unit, vlan_translation_info.port1, vlan_translation_info.vlan2, BCM_VLAN_NONE);
  if (rv != BCM_E_NONE) {
    printf("Error, in bcm_vlan_translate_egress_action_delete\n");
    return rv;
  }

  return rv; 	
}

/* 
 * Vlan translation in vlan_translation_info.vlan2
 * This function must be called after vlan_translation_run
 * ingress - s tag, remove outer vlan and add inner vlan, c_tag
 * egress - c-tag, add outer vlan and replace inner vlan, s_c_tag
 */
int 
vlan_translation_run_8(int unit, int vlan_0, int vlan_1, int vlan_2) 
{
  int rv;
  bcm_vlan_action_set_t action;
  bcm_gport_t gport;

  /*
   * In order to classify the single inner tag in the egress, the egress port dtag mode is set to EXTERNAL 
   * and the egress port outer TPID is set to 0x9100.
   */	
  rv = bcm_port_dtag_mode_set(unit, vlan_translation_info.port1, BCM_PORT_DTAG_MODE_EXTERNAL);
  if (rv != BCM_E_NONE) {
      printf("Error, bcm_port_dtag_mode_set\n");
      return rv;
  }
  
  port_tpid_init_cep_port_example(vlan_translation_info.port1,1,0);   
  rv = port_tpid_set(unit);
  if (rv != BCM_E_NONE) {
      printf("Error, port_tpid_set\n");
      return rv;
  }
  
  BCM_GPORT_LOCAL_SET(gport,vlan_translation_info.port2);

  /*
   * Ingress action example: remove outer vlan and add inner tag  
   */
  bcm_vlan_action_set_t_init(&action);
  action.ot_outer = bcmVlanActionDelete;
  action.ot_inner = bcmVlanActionAdd;
  action.new_inner_vlan = vlan_0;
  action.outer_tpid_action = bcmVlanTpidActionModify;
  action.inner_tpid_action = bcmVlanTpidActionModify;
  action.outer_tpid = 0x9100;
  action.inner_tpid = 0x8100;


  rv = bcm_vlan_translate_action_create(unit, gport, bcmVlanTranslateKeyPortOuter, vlan_translation_info.vlan2, BCM_VLAN_NONE, &action);
  if (rv != BCM_E_NONE) {
    	printf("Error, in bcm_vlan_translate_action_create\n");
    	return rv;
  }

  /* 
   * Egress action example: add outer tag and replace inner tag.
   */
  bcm_vlan_action_set_t_init(&action);
  action.it_outer = bcmVlanActionAdd;
  action.it_inner = bcmVlanActionReplace;
  action.new_outer_vlan = vlan_1;
  action.new_inner_vlan = vlan_2;
  action.outer_tpid_action = bcmVlanTpidActionOuter;
  action.inner_tpid_action = bcmVlanTpidActionModify;
  action.outer_tpid = 0x9100;
  action.inner_tpid = 0x8100;
  
  /*
   * In case of Port CEP, VLAN-Domain x Out-VSI x VLAN is the key in ESEM lookup.
   * In bcm_vlan_translate_egress_action_add input is port class and not port. 
   * Since in this case port_class equals port1 then port input port1 is OK.
   */
  rv = bcm_vlan_translate_egress_action_add(unit, vlan_translation_info.port1, vlan_translation_info.vlan2, vlan_0, &action);
  if (rv != BCM_E_NONE) {
    printf("Error, in bcm_vlan_translate_egress_action_add\n");
    return rv;
  }

  return rv; 	
}

/* 
 * Vlan translation in vlan_translation_info.vlan2
 * This function must be called after vlan_translation_run
 * ingress - s_c tag (vlan2, c_tag), remove outer vlan, c_tag
 * egress - c_tag, add outer tag and replace inner vlan, s_c_tag
 */
int 
vlan_translation_run_9(int unit, int c_tag, int vlan_0, int vlan_1) 
{
  int rv;
  bcm_vlan_action_set_t action;
  bcm_gport_t gport;

  /*
   * In order to classify the single inner tag in the egress, the egress port dtag mode is set to EXTERNAL 
   * and the egress port outer TPID is set to 0x9100.
   */
  bcm_port_dtag_mode_set(unit, vlan_translation_info.port1, BCM_PORT_DTAG_MODE_EXTERNAL);
  if (rv != BCM_E_NONE) {
      printf("Error, bcm_port_dtag_mode_set\n");
      return rv;
  }

  port_tpid_init_cep_port_example(vlan_translation_info.port1,1,0);
  
  rv = port_tpid_set(unit);
  if (rv != BCM_E_NONE) {
      printf("Error, port_tpid_set\n");
      return rv;
  }

  BCM_GPORT_LOCAL_SET(gport,vlan_translation_info.port2);

  /*
   * Ingress action example: remove outer vlan  
   */
  bcm_vlan_action_set_t_init(&action);
  action.dt_outer = bcmVlanActionDelete;
  action.dt_inner = bcmVlanActionNone;

  rv = bcm_vlan_translate_action_create(unit, gport, bcmVlanTranslateKeyPortDouble, vlan_translation_info.vlan2, c_tag, &action);
  if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_translate_action_create\n");
        return rv;
  }

  /* 
   * Egress action example: add outer tag and replace inner vlan
   */
  bcm_vlan_action_set_t_init(&action);
  action.it_outer = bcmVlanActionAdd;
  action.it_inner = bcmVlanActionReplace;
  action.new_outer_vlan= vlan_0;
  action.new_inner_vlan= vlan_1;
  action.outer_tpid_action = bcmVlanTpidActionInner;
  action.inner_tpid_action = bcmVlanTpidActionInner;
  
  rv = bcm_vlan_translate_egress_action_add(unit, vlan_translation_info.port1, vlan_translation_info.vlan2, BCM_VLAN_NONE, &action);
  if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_translate_egress_action_add\n");
        return rv;
  }

  return rv; 
}

/* 
 * Delete basic vlan information created by vlan_translation_run
 */
int 
vlan_translation_vlan_destroy(int unit) 
{
  int rv;
  bcm_pbmp_t pbmp;

  BCM_PBMP_PORT_ADD(pbmp,vlan_translation_info.port1);
  BCM_PBMP_PORT_ADD(pbmp,vlan_translation_info.port2);

  rv = bcm_vlan_port_remove(unit, vlan_translation_info.vlan1, pbmp);
  if (rv != BCM_E_NONE) {
    printf("Error, in bcm_vlan_port_remove, vlan=%d, \n", vlan_translation_info.vlan1);
    return rv;
  }

  rv = bcm_vlan_destroy(unit,vlan_translation_info.vlan1);
  if (rv != BCM_E_NONE) {
    printf("Error, in bcm_vlan_destroy, vlan=%d, \n", vlan_translation_info.vlan1);
    return rv;
  }
  
  rv = bcm_vlan_port_remove(unit, vlan_translation_info.vlan2, pbmp);
  if (rv != BCM_E_NONE) {
    printf("Error, in bcm_vlan_port_remove, vlan=%d, \n", vlan_translation_info.vlan2);
    return rv;
  }
  
  rv = bcm_vlan_destroy(unit,vlan_translation_info.vlan2);
  if (rv != BCM_E_NONE) {
    printf("Error, in bcm_vlan_destroy, vlan=%d, \n", vlan_translation_info.vlan2);
    return rv;
  }
  
  return rv; 	
}

int run_with_ports (int unit, int port1, int port2) {
    vlan_translation_info_init (port1, port2, 2, 3 , 0);
    return vlan_translation_run(unit,0);
}