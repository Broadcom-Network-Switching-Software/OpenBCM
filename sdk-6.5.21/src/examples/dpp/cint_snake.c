/*~~~~~~~~~~~~~~~~~~~~~~~~~~~Snake Application~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 * File:    cint_snake.c 
 * Purpose: Example for sending 80Gb traffic per device (snake) 
 *          We inject 10Gb from a specific port with a pre-defined MAC address and
 *          without a vlan tag, and send the traffic to the next 
 *          port using the MACT, by assigning a different vlan ID per port. 
 *  
 * This example can be used on a single card, two cards with two CPUs, 
 * or two cards controlled by a single CPU (rpc / central management). 
 *  
 * This configuration can place an 80 Gb load on the device while using
 * a single 10 Gb traffic generator port. It may be used as a base for:
 *      o    L2-based forwarding.
 *      o    Testing the device capabilities under full rate.
 * Due to the snake application setup, a packet arriving from the traffic
 * generator port is sent internally eight times until it returns to the
 * traffic generator port. Therefore, 10G traffic from the generator port
 * results in 80G traffic in the device. 
 */

/*
 * Basic Parameters
 */
int MAX_NUMBER_OF_PORTS = 256;
int MAX_MODIDS_PER_DEVICE = 2;

struct snake_info_s {
    int base_vlan;   /* port vlan id is (base_vlan+port)*/
    int tg_sys_port; /* traffic generator system port */
    uint8 mac[6];    /* mac address to be added to mac table */
    int nof_devices; /* 1/2 */
};

/* 
 * Snake configuration: 
 * Note: On each device, snake configuration might be different.  
 */
void
init(int unit, int unit_2nd, snake_info_s *info, int tg_sys_port) {
    uint8 mac[6] = {0,0,0,0,0,0xff}; /* 00:00:00:00:00:ff */
    /* when traffic generator port is not part of the device set tg_sys_port -1 */
    /* When traffic generator port is part of the device set tg_sys_port = source port */
    info->tg_sys_port = tg_sys_port;
    info->base_vlan = 3800;  
    info->nof_devices = (unit_2nd > 0 ? 2 : 1);
  
    sal_memcpy(info->mac, mac, 6);
}

/*
 * Inject 10Gb from a specific port with a pre-defined MAC address and 
 * without a vlan tag, and send the traffic to the next 
 * port using the MACT, by assigning a different vlan ID per port. 
 */
int
snake(int unit, snake_info_s *p_info, int external_lb, int is_arad) {
    int rv, vid;
    bcm_pbmp_t pbmp,pbmp1;
    bcm_port_t port, next_port, first_port, last_port, port_count;
    bcm_l2_addr_t l2_addr;
    int mymodid;
    bcm_port_config_t config;
    bcm_vlan_port_t vp1,vp2;

    bcm_gport_t gport_sysport;
    int sysport;
  
    rv = bcm_stk_my_modid_get(unit,&mymodid);  
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_stk_my_modid_get \n");
        return rv;
    }

    rv = bcm_port_config_get(unit, &config);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_config_get \n");
        return rv;
    }

    BCM_PBMP_ASSIGN(pbmp, config.e);
    BCM_PBMP_REMOVE(pbmp, config.cpu);
    BCM_PBMP_REMOVE(pbmp, config.rcy);
    /* Find the 1st port */
    BCM_PBMP_ITER(pbmp, port) {
        first_port = port;
        break;
    }

    /* Find the last port */
    BCM_PBMP_ITER(pbmp, port) {
        last_port = port;
    }
	printf("first port=%d, last port=%d\n", first_port, last_port);
    BCM_PBMP_ITER(pbmp, port) {
		printf("port=%d\n", port);
        /* Create vlan */
        vid = p_info->base_vlan + port;
        rv = bcm_vlan_create(unit, vid);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_create, vid=%d, \n", vid);
            return rv;
        }
		printf("1");
      
        /* Calculate next port. If this is the last port, next port is the 1st port */
        next_port = port+1;
        while ((next_port <= last_port) && !BCM_PBMP_MEMBER(pbmp, next_port)) {
            ++next_port;
        }       
        if (next_port > last_port) {
            next_port = first_port;
        }
		printf("next_port=%d\n", next_port);

        /* Add current port and next port to the vlan */

        BCM_PBMP_PORT_SET(pbmp1,port);

        BCM_PBMP_PORT_ADD(pbmp1,next_port);

        
        rv = bcm_vlan_port_add(unit,vid,pbmp1,pbmp1);

        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_port_add, vid=%d, \n", vid);
            return rv;
        }
          
        /* Set pvid for this port */
        rv = bcm_port_untagged_vlan_set(unit, port, vid);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_untagged_vlan_set, vid=%d\n", vid);
            return rv;
        }
      
		if (is_arad) {
            /* map untagged packet */
            rv = bcm_port_class_set(unit,port,bcmPortClassId,port);
            if (rv != BCM_E_NONE) {
                printf("Error, in bcm_port_class_set, vid=%d\n", vid);
                return rv;
            }

			/* the match criteria is port:1, out-vlan:10,  in-vlan:20 */
			vp1.criteria = BCM_VLAN_PORT_MATCH_PORT_INITIAL_VLAN;
			vp1.port = port;
			vp1.match_vlan = vid;
			vp1.vsi = vid;
			
			rv = bcm_vlan_port_create(unit, &vp1);
			if (rv != BCM_E_NONE) {
				printf("Error, in bcm_vlan_port_create, vp1=%d\n", vp1);
				return rv;
			}
		}
		
          
        /* Add mac entry from current port to next port */           
        /* If next port is the first port, it is suppose to go to the other device id */
        bcm_l2_addr_t_init(&l2_addr, p_info->mac, vid);
        l2_addr.flags |= BCM_L2_STATIC;
        if (first_port == next_port) {
            BCM_GPORT_SYSTEM_PORT_ID_SET(l2_addr.port, (next_port + (((p_info->nof_devices == 2) ? (2 - mymodid) : mymodid ) / MAX_MODIDS_PER_DEVICE * MAX_NUMBER_OF_PORTS)));
        } else {
            l2_addr.port = next_port;
        }
          
        rv = bcm_l2_addr_add(unit, &l2_addr);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_l2_addr_add, port=%d, next_port=%d\n", port, next_port);
            print l2_addr;
            return rv;
        }
        
        rv = bcm_stk_gport_sysport_get (unit,port,&gport_sysport);
        sysport = BCM_GPORT_SYSTEM_PORT_ID_GET(gport_sysport);   
      
        /* We set all ports in loopback, except for traffic generator port (tg_port) */
        if (sysport != p_info->tg_sys_port && (external_lb == 0)) {
			printf("SETTING\n");
            rv = bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_PHY);
            if (rv != BCM_E_NONE) {
                printf("Error, in bcm_port_loopback_set, port=%d\n", port);
                return rv;
            }      
        }    
    }
  
    /* If sending traffic from CPU, send traffic from last port to CPU */
    if (p_info->tg_sys_port == 0) {
        bcm_l2_addr_t_init(&l2_addr, p_info->mac, vid);
        l2_addr.flags |= BCM_L2_STATIC;  
        l2_addr.port = p_info->tg_sys_port;
      
        rv = bcm_l2_addr_add(unit, &l2_addr);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_l2_addr_add\n");
            print l2_addr;
            return rv;
        }
    }
    
    return 0;
}

/* 
 * Main function for snake example
 * Parameters: 
 *   unit
 *   unit_2nd - if > 0, traffic will be sent between cards
 *   central_cpu - if 0, cint will be run on a single card. If 1, it will
 *   also configure the 2nd unit.
 */
int snake_example(int unit, int unit_2nd, int central_cpu, int tg_sys_port, int external_lb, int is_arad) {
  snake_info_s info;
  int rv;
  
  init(unit, unit_2nd, &info, tg_sys_port);
  rv = snake(unit, &info, external_lb, is_arad);
  
  if (unit_2nd >= 0 && central_cpu) {
    rv = snake(unit_2nd, &info, external_lb, is_arad);
  }

  return rv;
}
