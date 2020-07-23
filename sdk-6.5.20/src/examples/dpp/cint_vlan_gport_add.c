/* 
* 
*
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
* 
* File: cint_vlan_gport_add.c
* Purpose: Example of employing bcm_vlan_gport_add() in 4 distinct cases. Each case shows its usage of bcm_vlan_gport_add():
* For instance: Case 1 sets ingress vlan membership per port X vlan, so we employ bcm_vlan_gport_add(unit, vlan, port, flags),
* while flags == BCM_VLAN_GPORT_ADD_INGRESS_ONLY
* 
* Explanation: 
* The test illustrates 4 distinct usages of  bcm_vlan_gport_add(). Each usage is defined in a case. 
* To each case we define set-get-toggle functions. A toggle function flips the current state. For instance: If a certain port is not a member of
* a certain vlan, then employing the toggle function would flip this state, thus making this port a member of the vlan. The opposite case follows
* this logic too: a port that is a member of a certain vlan would lose it's membership following the toggle.
* 
* Case 1: Deals with Ingress Vlan membership per Port X Vlan. 
* Case 2: Deals with  Egress tagged mode per Port X Vlan 
* Case 3: Deals with Egress VSI membership per Port X VSI 
* Case 4: Deals with membership in egress vsi multicast group
* 
* Activation of the test requires running the following instructions: 
*   BCM> cint cint_vlan_gport_add.c
*   BCM> cint
*   cint> vlan_gport_basic_example();
*/



int verbose = 1;

/*
* Creates Vlans for the test
*/
int vlan_gport_bridgingInit(int unit)
    {
		int hal_rc = 0;
        int bcm_rc;

        /*
         * Create relevant VLANS for the test
         */
        int vlan;
        for ( vlan = 10; vlan <= 20; vlan = vlan + 10)
        {
            bcm_rc = bcm_vlan_create(unit, vlan);
            if (BCM_FAILURE(bcm_rc))
            {
                printf("bcm_vlan_create() failed for vlan %d\n", vlan);
                hal_rc = -1;
                return hal_rc;
            }
        }

        return hal_rc;
    }


/* Case 1: Ingress Vlan membership per Port X Vlan*/

/*
* Set Ingress Vlan membership per Port X Vlan
*/
int vlan_gport_set_ingress_vlan_membership(int unit, bcm_vlan_t vlan, bcm_gport_t port){

	int flags = BCM_VLAN_GPORT_ADD_INGRESS_ONLY;

	return bcm_vlan_gport_add(unit, vlan, port, flags); 

}

/*
* Toggles Ingress Vlan membership per Port X Vlan: turns set to unset and vice-versa.
*/
int vlan_gport_toggle_ingress_vlan_membership(int unit, bcm_vlan_t vlan, bcm_gport_t port){

	int flags = BCM_VLAN_GPORT_ADD_INGRESS_ONLY | BCM_VLAN_GPORT_ADD_MEMBER_REPLACE;

	return bcm_vlan_gport_add(unit, vlan, port, flags);
}

/*
* Get Ingress Vlan membership per Port X Vlan
*/
int vlan_gport_get_ingress_vlan_membership(int unit, bcm_vlan_t vlan, bcm_gport_t gport, bcm_vlan_gport_info_t *vlan_gport_info){

	bcm_vlan_gport_info_t_init(vlan_gport_info);

	vlan_gport_info->vlan = vlan;
	vlan_gport_info->gport = gport;

	return bcm_vlan_gport_info_get(unit, vlan_gport_info);

}




/* Case 2: Egress tagged mode per Port X Vlan*/

/*
* Set Egress tagged mode per Port X Vlan
*/
int vlan_gport_set_egress_tagged_mode(int unit, bcm_vlan_t vlan, bcm_gport_t port){

	int flags = BCM_VLAN_GPORT_ADD_EGRESS_ONLY | BCM_VLAN_GPORT_ADD_UNKNOWN_UCAST_DO_NOT_ADD | BCM_VLAN_GPORT_ADD_MEMBER_DO_NOT_UPDATE;

	return bcm_vlan_gport_add(unit, vlan, port, flags);
}

/*
* Toggles Egress tagged mode per Port X Vlan: turns set to unset and vice-versa
*/
int vlan_gport_toggle_egress_tagged_mode(int unit, bcm_vlan_t vlan, bcm_gport_t port){

	int flags = BCM_VLAN_GPORT_ADD_EGRESS_ONLY | BCM_VLAN_GPORT_ADD_UNKNOWN_UCAST_DO_NOT_ADD | BCM_VLAN_GPORT_ADD_MEMBER_DO_NOT_UPDATE
		| BCM_VLAN_GPORT_ADD_MEMBER_REPLACE;

	return bcm_vlan_gport_add(unit, vlan, port, flags);
}

/*
* Get Egress tagged mode per Port X Vlan
*/
int vlan_gport_get_egress_tagged_mode(int unit, bcm_vlan_t vlan, bcm_gport_t gport, bcm_vlan_gport_info_t *vlan_gport_info){

	bcm_vlan_gport_info_t_init(vlan_gport_info);

	vlan_gport_info->vlan = vlan;
	vlan_gport_info->gport = gport;

	return bcm_vlan_gport_info_get(unit, vlan_gport_info);

}




/* Case 3: Egress VSI membership per Port X VSI*/

/*
* Set Egress VSI membership per Port X VSI
*/
int vlan_gport_set_egress_vsi_membership(int unit, bcm_vlan_t vsi, bcm_gport_t port){

	int flags = BCM_VLAN_GPORT_ADD_EGRESS_ONLY | BCM_VLAN_GPORT_ADD_UNKNOWN_UCAST_DO_NOT_ADD 
		| BCM_VLAN_GPORT_ADD_TAG_DO_NOT_UPDATE | BCM_VLAN_GPORT_ADD_SERVICE;

	return bcm_vlan_gport_add(unit, vsi, port, flags);
	
}

/*
* Toggles Egress VSI membership per Port X VSI
*/
int vlan_gport_toggle_egress_vsi_membership(int unit, bcm_vlan_t vsi, bcm_gport_t port){

	int flags = BCM_VLAN_GPORT_ADD_EGRESS_ONLY | BCM_VLAN_GPORT_ADD_MEMBER_REPLACE | 
		BCM_VLAN_GPORT_ADD_UNKNOWN_UCAST_DO_NOT_ADD | BCM_VLAN_GPORT_ADD_TAG_DO_NOT_UPDATE | BCM_VLAN_GPORT_ADD_SERVICE;

	return bcm_vlan_gport_add(unit, vsi, port, flags);

}

/*
* Get Egress VSI membership per Port X VSI
*/
int vlan_gport_get_egress_vsi_membership(int unit, bcm_vlan_t vsi, bcm_gport_t gport, bcm_vlan_gport_info_t *vlan_gport_info){

	bcm_vlan_gport_info_t_init(vlan_gport_info);

	vlan_gport_info->flags = BCM_VLAN_GPORT_ADD_SERVICE;
	vlan_gport_info->vsi = vsi;
	vlan_gport_info->gport = gport;

	return bcm_vlan_gport_info_get(unit, vlan_gport_info);
	
}




/* Case 4: Adding port to VSI multicast group */

/*
* Adds port to VSI multicast group
*/
int vlan_gport_set_egress_multicast_vsi(int unit, bcm_vlan_t vsi, bcm_gport_t port){

	int flags = BCM_VLAN_GPORT_ADD_EGRESS_ONLY | BCM_VLAN_GPORT_ADD_MEMBER_DO_NOT_UPDATE
		| BCM_VLAN_GPORT_ADD_TAG_DO_NOT_UPDATE | BCM_VLAN_GPORT_ADD_SERVICE;

	return bcm_vlan_gport_add(unit, vsi, port, flags);

}

/*
* Toggles membership of port in VSI multicast group
*/
int vlan_gport_toggle_egress_multicast_vsi(int unit, bcm_vlan_t vsi, bcm_gport_t port){

	int flags = BCM_VLAN_GPORT_ADD_EGRESS_ONLY | BCM_VLAN_GPORT_ADD_MEMBER_REPLACE |
		BCM_VLAN_GPORT_ADD_MEMBER_DO_NOT_UPDATE | BCM_VLAN_GPORT_ADD_TAG_DO_NOT_UPDATE | BCM_VLAN_GPORT_ADD_SERVICE;

	return bcm_vlan_gport_add(unit, vsi, port, flags);

}

/*
* Checks whether gport is a member in the vsi ,multicast group 
* Notice that this get function is different than those above: This functionality 
* is retrieved from bcm_multicast_egress_get 
*/
int vlan_gport_get_egress_multicast_vsi(int unit, bcm_vlan_t vsi, bcm_gport_t gport){

	bcm_gport_t port_array[10];
	int port_max = 10, port_count, i, tested_port, ret_val = FALSE, mask = 511;
	bcm_multicast_t group = vsi;
	bcm_if_t encap_id_array[10];

	bcm_multicast_egress_get(unit, group, port_max, port_array, encap_id_array, &port_count);

	if (port_count != 0) {
		for (i = 0; i < port_count; i++) {
			tested_port = port_array[i] & mask;
			if (tested_port == gport) {
				ret_val = TRUE;
				return ret_val;
			}
		}
	}
	return ret_val;
}



/*
* Presents an example of running bcm_vlan_gport_add() with 4 different cases, each 
* illustrates a different usage of the api.  
* Each case performs a set-get-toggle-get test 
*/

int vlan_gport_basic_example(uint32 sysPort){

	int bcm_rc;
	int flags;
	int unit=0;
	uint16 localVlan = 20;
	uint16 vsi = 10;
	bcm_vlan_gport_info_t vlan_gport_info;

 
	vlan_gport_bridgingInit(unit);


	/*
	* Case 1: Setting membership for port sysPort and vlan 20 
	* Get function check verifies whether we get vlan_gport_info.flags == BCM_VLAN_GPORT_ADD_INGRESS_ONLY 
	* After toggling we should get: vlan_gport_info.flags == 0
	*/
	bcm_rc = vlan_gport_set_ingress_vlan_membership(unit, localVlan, sysPort); 

	bcm_rc = vlan_gport_get_ingress_vlan_membership(unit, localVlan, sysPort, &vlan_gport_info);
	if (vlan_gport_info.flags != BCM_VLAN_GPORT_ADD_INGRESS_ONLY | bcm_rc != BCM_E_NONE) {
		if (verbose) {
			printf("Error: expected flags should be BCM_VLAN_GPORT_ADD_INGRESS_ONLY \n");
		}
		return BCM_E_FAIL;	
	}

	bcm_rc = vlan_gport_toggle_ingress_vlan_membership(unit, localVlan, sysPort);
	bcm_rc = vlan_gport_get_ingress_vlan_membership(unit, localVlan, sysPort, &vlan_gport_info);
	if (vlan_gport_info.flags != 0 | bcm_rc != BCM_E_NONE) {	
		if (verbose) {
			printf("Error: expected flags should be 0. instead they are: %d \n", vlan_gport_info.flags);
		}
		return BCM_E_FAIL;
	}


	/*
	* Case 2: Setting tagged mode per sysPort and vlan == 20
	* Get function check verifies whether we get vlan_gport_info.flags == 0, which indicates that we are in tagged mode
	* After toggling we should get: vlan_gport_info.flags == BCM_VLAN_GPORT_ADD_UNTAGGED, which indicates untagged mode 
	* Another toggling should bring us back to tagged mode  
	*/

	bcm_rc = vlan_gport_set_egress_tagged_mode(unit, localVlan, sysPort);

	bcm_rc = vlan_gport_get_egress_tagged_mode(unit, localVlan, sysPort, &vlan_gport_info);
	if (vlan_gport_info.flags != 0 | bcm_rc != BCM_E_NONE) {
		if (verbose) {
			printf("Error: expected flags should be 0. instead they are: %d \n", vlan_gport_info.flags);
		}
		return BCM_E_FAIL;
	}

	bcm_rc = vlan_gport_toggle_egress_tagged_mode(unit, localVlan, sysPort);
	bcm_rc = vlan_gport_get_egress_tagged_mode(unit, localVlan, sysPort, &vlan_gport_info);
	if (vlan_gport_info.flags != BCM_VLAN_GPORT_ADD_UNTAGGED | bcm_rc != BCM_E_NONE) {
		if (verbose) {
			printf("Error: expected flags should be BCM_VLAN_GPORT_ADD_UNTAGGED. instead they are: %d \n", vlan_gport_info.flags);
		}
		return BCM_E_FAIL;
	}

	bcm_rc = vlan_gport_toggle_egress_tagged_mode(unit, localVlan, sysPort);
	bcm_rc = vlan_gport_get_egress_tagged_mode(unit, localVlan, sysPort, &vlan_gport_info);
	if (vlan_gport_info.flags != 0 | bcm_rc != BCM_E_NONE) {
		if (verbose) {
			printf("Error: expected flags should be 0. instead they are: %d \n", vlan_gport_info.flags);
		}
		return BCM_E_FAIL;
	}


	/*
	* Case 3: Setting egress vsi membership per sysPort and vsi == 10
	* Get function check verifies whether we get vlan_gport_info.flags == 0, which indicates that we are in tagged mode
	* After toggling we should get: vlan_gport_info.flags == BCM_VLAN_GPORT_ADD_UNTAGGED, which indicates untagged mode 
	* Another toggling should bring us back to tagged mode  
	*/

	bcm_rc = vlan_gport_set_egress_vsi_membership(unit, vsi, sysPort);

	bcm_rc = vlan_gport_get_egress_vsi_membership(unit, vsi, sysPort, &vlan_gport_info);
	if (vlan_gport_info.flags != (BCM_VLAN_GPORT_ADD_SERVICE | BCM_VLAN_GPORT_ADD_EGRESS_ONLY) | bcm_rc != BCM_E_NONE) {
		if (verbose) {
			printf("Error: expected flags should be BCM_VLAN_GPORT_ADD_SERVICE | BCM_VLAN_GPORT_ADD_EGRESS_ONLY . instead they are: %d \n", vlan_gport_info.flags);
		}
		return BCM_E_FAIL;
	}

	bcm_rc = vlan_gport_toggle_egress_vsi_membership(unit, vsi, sysPort);

	bcm_rc = vlan_gport_get_egress_vsi_membership(unit, vsi, sysPort, &vlan_gport_info);
	if (vlan_gport_info.flags != BCM_VLAN_GPORT_ADD_SERVICE | bcm_rc != BCM_E_NONE) {
		if (verbose) {
			printf("Error: expected flags should be BCM_VLAN_GPORT_ADD_SERVICE . instead they are: %d \n", vlan_gport_info.flags);
		}
		return BCM_E_FAIL;
	}


	/*
	* Case 4: Setting membership of port in vsi egress multicast group per sysPort and vsi == 10
	* Get function check verifies whether membership is TRUE (according to the above funciton)
	* After toggling we should get that membership is FALSE  
	*/

	bcm_rc = vlan_gport_set_egress_multicast_vsi(unit, vsi, sysPort);

	bcm_rc = vlan_gport_get_egress_multicast_vsi(unit, vsi, sysPort);
	if (!bcm_rc) {
		if (verbose) {
			printf("Error: expected membership value should be TRUE. instead they are: %d \n", bcm_rc);
		}
		return BCM_E_FAIL;
	}

	bcm_rc = vlan_gport_toggle_egress_multicast_vsi(unit, vsi, sysPort);
	bcm_rc = vlan_gport_get_egress_multicast_vsi(unit, vsi, sysPort);
	if (bcm_rc) {
		if (verbose) {
			printf("Error: expected membership value should be FALSE. instead they are: %d \n", bcm_rc);
		}
		return BCM_E_FAIL;
	}

	return BCM_E_NONE;
}
