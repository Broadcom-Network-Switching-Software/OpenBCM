/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 * File: cint_ipmc.c
 * Purpose: to be used in IPMC tests
 * 
 * Cint functions for IP Multicast (IPMC)
 * 
 * APIs used:
 *  bcm_ipmc_add
 *  bcm_ipmc_traverse
 *  bcm_ipmc_remove_all
 *
 * Main function: run_main
 *  Scenario:
 *  1. create interfaces and ingress objects
 *  2. create Multicast Group for each entry
 *  3. clear ipmc table
 *  4. add MulticastNum of IPv4 entries with consecutive DIPs
 *  5. traverse IPv4 IPMC table
 *  6. add MulticastNum of IPv6 entries with consecutive DIPs
 *  7. traverse IPv4 IPMC table
 *  8. traverse IPv6 IPMC table
 *  9. cleanups
 */

/* Global variables */
/* Number of Multicast entries to be added */
int MulticastNum = 4;
struct mcCfg_t {
    int ipmc_index;
    int mcId[MulticastNum];
};
mcCfg_t mc;
mc.ipmc_index = 6000;
/* Entry structures to be used for IPv4 */
bcm_ipmc_addr_t data[MulticastNum];
/* Entry structures to be used for IPv6 */
bcm_ipmc_addr_t data6[MulticastNum];
/* Counter for traversed entries */
int _counter = 0;
/* Flag variable to validate traversed entry */
int OK = BCM_E_NONE;

/* Destroy function to cleanup after test */
int destroyIpmc(int unit) {
    int rv = BCM_E_NONE;
    int i;
    for (i=0; i < MulticastNum; i++) {
        rv = bcm_multicast_destroy(unit, mc.mcId[i]);
        if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND)) {
            printf("Error, in multicast_destroy\n");
            return rv;
        }
    }
    rv = bcm_l3_intf_delete_all(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_l3_intf_delete_all\n");
        return rv;
    }
    rv = bcm_ipmc_remove_all(unit);
    if (rv != BCM_E_NONE) {
        printf("bcm_ipmc_delete_all returned with failure code '%s'\n", bcm_errmsg(rv));
        return rv;
    }
    return rv;
}
/* Function to configure L3 interfaces */
int mcL3Config(int unit, int vid) {
    int rv = BCM_E_NONE;
    int i;
    bcm_l3_intf_t l3if;
    bcm_l3_ingress_t l3_ing_if;
    for (i = 0; i < MulticastNum; i++) {
        bcm_l3_intf_t_init(&l3if);
        l3if.l3a_mac_addr[4] = 0xab;
        l3if.l3a_mac_addr[5] = 0x1d;
        l3if.l3a_vid = vid + i;
        l3if.l3a_flags = 0;
        l3if.l3a_vrf = i;
        if (is_device_or_above(unit,JERICHO2))
        {
            l3if.l3a_vrf = i + 1;
        }
        rv = bcm_l3_intf_create(unit, l3if);
        if (rv != BCM_E_NONE) {
            printf("bcm_l3_intf_create(%d) returned with failure code '%s'\n", ml3if.l3a_vid, bcm_errmsg(rv));
            return rv;
        }
        bcm_l3_ingress_t_init(&l3_ing_if);
        l3_ing_if.urpf_mode = bcmL3IngressUrpfDisable;
        l3_ing_if.flags = BCM_L3_INGRESS_WITH_ID;
        l3_ing_if.flags |= BCM_L3_INGRESS_GLOBAL_ROUTE;
        rv = bcm_l3_ingress_create(unit, l3_ing_if, l3if.l3a_intf_id);
        if (rv != BCM_E_NONE) {
            printf("bcm_l3_ingress_create returned with failure code '%s'\n", bcm_errmsg(rv));
            return rv;
        }
    }
    return rv;
}
/* Function to create Multicast groups for each entry */
int mcL3Create(int unit) {
    int rv = BCM_E_NONE;
    int i;
    bcm_multicast_t mc_id;
    for (i = 0; i < MulticastNum; i++) {
        mc_id = mc.ipmc_index + i;
        rv = bcm_multicast_create(unit,
        BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_TYPE_L3 |
        BCM_MULTICAST_WITH_ID, &mc_id); if (rv != BCM_E_NONE) {
            printf("bcm_multicast_create(%d) returned with failure code '%s'\n", mc.ipmc_index + i, bcm_errmsg(rv));
            return rv;
        }
        mc.mcId[i] = mc_id;
    }
    return rv;
}
/* Function to add IPv4 entries to IPMC table*/
int addMulticast(int unit, int vid) {
    int rv = BCM_E_NONE;
    int i;
    char *kbp_dev_type = soc_property_get_str(unit, spn_EXT_TCAM_DEV_TYPE);
    int is_kbp = kbp_dev_type ? (!(sal_strcmp("NONE", kbp_dev_type)) ? 0 : 1) : 0;

    for (i = 0; i < MulticastNum; i++) {
        bcm_ipmc_addr_t_init(&data[i]);
        data[i].flags = 0;
        data[i].s_ip_addr = 0x00000001 + i;
        data[i].s_ip_mask = 0xFFFFFFFF;
        data[i].mc_ip_addr = 0xE0000001 + i;
        data[i].mc_ip_mask = 0xFFFFFFFF;
        data[i].vrf = i;
        if (is_device_or_above(unit,JERICHO2))
        {
            data[i].vrf = i + 1;
        }
        data[i].vid = vid + i;
        data[i].group = mc.mcId[i];
        if (is_device_or_above(unit,JERICHO2) & is_kbp)
        {
            /* External lookup enabled on JR2 */
            data[i].flags = BCM_IPMC_TCAM;
        }
        rv = bcm_ipmc_add(unit, &data[i]);
        if (rv != BCM_E_NONE) {
            printf("bcm_ipmc_add() returned with failure code '%s'\n", bcm_errmsg(rv));
            return rv;
        }
    }
    return rv;
}

/* Function to add IPv6 entries to IPMC table*/
int addMulticastv6(int unit, int vid) {
    int rv = BCM_E_NONE;
    int i;
    bcm_ip6_t full_mask  = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    char *kbp_dev_type = soc_property_get_str(unit, spn_EXT_TCAM_DEV_TYPE);
    int is_kbp = kbp_dev_type ? (!(sal_strcmp("NONE", kbp_dev_type)) ? 0 : 1) : 0;

    for (i = 0; i < MulticastNum; i++) {
        bcm_ipmc_addr_t_init(&data6[i]);
        data6[i].flags = BCM_IPMC_IP6;
        if (is_device_or_above(unit,JERICHO2) & is_kbp)
        {
            /* External lookup enabled on JR2 */
            data6[i].flags = BCM_IPMC_IP6 | BCM_IPMC_TCAM;
        }
        data6[i].s_ip6_addr[0] = 0x20;
        data6[i].s_ip6_addr[15] = 0x01 + i;
        data6[i].mc_ip6_addr[0] = 0xFF;
        data6[i].mc_ip6_addr[15] = 0x01 + i;
        if (data6[i].mc_ip6_addr[15] == 0) {
            data6[i].mc_ip6_addr[14]++;
        }
        sal_memcpy(data6[i].mc_ip6_mask,full_mask,16);
        if (is_device_or_above(unit,JERICHO2))
        {
            data6[i].vrf = i + 1;
        }
        else
        {
            data6[i].vrf = i;
        }
        data6[i].vid = vid + i;
        data6[i].group = mc.mcId[i];
        rv = bcm_ipmc_add(unit, &data6[i]);
        if (rv != BCM_E_NONE) {
            printf("bcm_ipmc_add() returned with failure code '%s'\n", bcm_errmsg(rv));
            return rv;
        }
    }
    return rv;
}

/* Utility function to print IPv4 address*/
int print_ip_addr(char *msg, uint32 addr) {
  int a,b,c,d;
  a = (addr >> 24) & 0xff;
  b = (addr >> 16) & 0xff;
  c = (addr >> 8) & 0xff;
  d = addr & 0xff;
  printf("%s %d.%d.%d.%d\n", msg, a, b, c, d);
  return BCM_E_NONE;
}

/* Utility function to print IPv6 address*/
int print_ip6_addr(char *msg, uint8 *addr) {
  int i;
  printf("%s", msg);
  for (i = 0; i < 7; i++) {
      printf("%02x%02x:", addr[2*i], addr[2*i+1]);
  }
  printf("%02x%02x\n", addr[14], addr[15]);
  return BCM_E_NONE;
}
/* Function to print traversed entry */
int print_ipmc_addr(bcm_ipmc_addr_t info) {
    int rv = BCM_E_NONE;
    int flags = info.flags;
    if((flags & BCM_IPMC_IP6) == 0) {
        printf("IPv4 entry %d:\n", _counter);
        rv = print_ip_addr("  SIP = ",info.s_ip_addr);
        if (rv != BCM_E_NONE) {
            printf("print_ip_addr() returned with failure code '%s'\n", bcm_errmsg(rv));
            return rv;
        }
        rv = print_ip_addr("  DIP = ",info.mc_ip_addr);
        if (rv != BCM_E_NONE) {
            printf("print_ip_addr() returned with failure code '%s'\n", bcm_errmsg(rv));
            return rv;
        }
    } else {
        printf("IPv6 entry %d:\n", _counter);
        rv = print_ip6_addr("  SIP = ",info.s_ip6_addr);
        if (rv != BCM_E_NONE) {
            printf("print_ip6_addr() returned with failure code '%s'\n", bcm_errmsg(rv));
            return rv;
        }
        rv = print_ip6_addr("  DIP = ",info.mc_ip6_addr);
        if (rv != BCM_E_NONE) {
            printf("print_ip6_addr() returned with failure code '%s'\n", bcm_errmsg(rv));
            return rv;
        }
    }
    printf("  VRF = %d\n", info.vrf);
    printf("  VID = %d\n", info.vid);
    return rv;
}

/* Function to verify traversed entry.
 *  fields checked: SIP, DIP, VID */
int check_ipmc_addr(bcm_ipmc_addr_t info) {
    int rv = BCM_E_NONE;
    int i;
    int flags = info.flags;
    if((flags & BCM_IPMC_IP6) == 0) {
    /* If entry is IPv4 */
        /* Check SIP */
        if (info.s_ip_addr != data[_counter].s_ip_addr) {
            printf("    Error in IPv4 entry %d: SIP mismatch!\n", _counter);
            rv = print_ip_addr("        Expected:", data[_counter].s_ip_addr);
            rv = print_ip_addr("        Actual  :", info.s_ip_addr);
            if (rv != BCM_E_NONE) {
                printf("print_ip_addr() returned with failure code '%s'\n", bcm_errmsg(rv));
                return rv;
            }
            OK = BCM_E_FAIL;
        }
        /* Check DIP */
        if (info.mc_ip_addr != data[_counter].mc_ip_addr) {
            printf("   Error in IPv4 entry %d: DIP mismatch!\n", _counter);
            rv = print_ip_addr("        Expected:", data[_counter].mc_ip_addr);
            rv = print_ip_addr("        Actual  :", info.mc_ip_addr);
            if (rv != BCM_E_NONE) {
                printf("print_ip_addr() returned with failure code '%s'\n", bcm_errmsg(rv));
                return rv;
            }
            OK = BCM_E_FAIL;
        }
        /* Check VID */
        if (info.vid != data[_counter].vid) {
            printf("  Error in IPv4 entry %d: VID mismatch!\n", _counter);
            printf("        Expected: %d\n", data[_counter].vid);
            OK = BCM_E_FAIL;
        }
        printf("  Entry %d checked: SIP, DIP, VID\n", _counter);
    } else {
    /* If entry is IPv6 */
        /* SIP not checked for IPv6 */
        /* Check DIP */
        for (i = 0; i < 16; i++) {
            if (info.mc_ip6_addr[i] != data6[_counter].mc_ip6_addr[i]) {
                printf("  Error in IPv6 entry %d: DIP mismatch!\n", _counter);
                rv = print_ip6_addr("        Expected:", data6[_counter].mc_ip6_addr);
                rv = print_ip6_addr("        Actual  :", info.mc_ip6_addr);
                if (rv != BCM_E_NONE) {
                    printf("print_ip6_addr() returned with failure code '%s'\n", bcm_errmsg(rv));
                    return rv;
                }
                OK = BCM_E_FAIL;
                break;
            }
        }
        /* Check VID */
        if (info.vid != data6[_counter].vid) {
            printf("  Error in IPv6 entry %d: VID mismatch!\n", _counter);
            printf("        Expected: %d\n", data6[_counter].vid);
            OK = BCM_E_FAIL;
        }
        printf("  Entry %d checked: DIP, VID\n", _counter);
    }
    return rv;
}

/* Callback function to be used for bcm_ipmc_traverse */
int ipmcTraversePrintCb(int unit, bcm_ipmc_addr_t *info, void *user_data) {
    int rv = BCM_E_NONE;
    if (info) {
        rv = print_ipmc_addr(*info);
        if (rv != BCM_E_NONE) {
            printf("print_ipmc_addr() returned with failure code '%s'\n", bcm_errmsg(rv));
            return rv;
        }
        rv = check_ipmc_addr(*info);
        if (rv != BCM_E_NONE) {
            printf("check_ipmc_addr() returned with failure code '%s'\n", bcm_errmsg(rv));
        }
        _counter++;
    }
    return rv;
}

/* Main function to be run from a test */
int run_main(int unit, int vid) {
    int rv = BCM_E_NONE;
    int ud;
    /* 1. Create interfaces and ingress objects */
    mcL3Config(unit, vid);
    /* 2. Create Multicast Group for each entry */
    mcL3Create(unit);
    /* 3. Clear IPMC table */
    rv = bcm_ipmc_remove_all(unit);
    if (rv != BCM_E_NONE) {
        printf("bcm_ipmc_delete_all returned with failure code '%s'\n", bcm_errmsg(rv));
        return rv;
    }
    /* 4. Add IPv4 MC entries */
    addMulticast(unit, vid);
    _counter = 0;
    /* 5. Traverse IPv4 IPMC table */
    rv = bcm_ipmc_traverse(unit, 0, ipmcTraversePrintCb, &ud);
    if (rv != BCM_E_NONE) {
        printf("bcm_ipmc_traverse returned with failure code '%s'\n", bcm_errmsg(rv));
        return rv;
    }
    printf("Counter for IPv4 = %d\n", _counter);
    if (_counter != MulticastNum) {
        printf("Error: Number entries traversed not verified.\n  Expected = %d\n  Traversed = %d\n", MulticastNum, _counter);
        return BCM_E_FAIL;
    } else {
        printf(" Verified number of traversed entries: %d\n", MulticastNum);
    }
    /* 6. Add IPv6 MC entries */
    addMulticastv6(unit, vid);
    _counter = 0;
    /* 7. Traverse IPv4 IPMC table */
    rv = bcm_ipmc_traverse(unit, 0, ipmcTraversePrintCb, &ud);
    if (rv != BCM_E_NONE) {
        printf("bcm_ipmc_traverse returned with failure code '%s'\n", bcm_errmsg(rv));
        return rv;
    }
    printf("Counter for IPv4 = %d\n", _counter);
    if (_counter != MulticastNum) {
        printf("Error: Number entries traversed not verified.\n  Expected = %d\n  Traversed = %d\n", MulticastNum, _counter);
        return BCM_E_FAIL;
    } else {
        printf(" Verified number of traversed entries: %d\n", MulticastNum);
    }
    _counter = 0;
    /* 8. Traverse IPv6 IPMC table */
    rv = bcm_ipmc_traverse(unit, BCM_IPMC_IP6, ipmcTraversePrintCb, &ud);
    if (rv != BCM_E_NONE) {
        printf("bcm_ipmc_traverse returned with failure code '%s'\n", bcm_errmsg(rv));
        return rv;
    }
    printf("Counter for IPv6 = %d\n", _counter);
    if (_counter != MulticastNum) {
        printf("Error: Number entries traversed not verified.\n  Expected = %d\n  Traversed = %d\n", MulticastNum, _counter);
        return BCM_E_FAIL;
    } else {
        printf(" Verified number of traversed entries: %d\n", MulticastNum);
    }
    /*It should not compare, Because you add with sequence 1 2 3 4, but the sequence will not be 1 2 3 4 when traverse*/
    if (!is_device_or_above(unit,JERICHO2))
    {
        if (OK != BCM_E_NONE) {
            printf("\nError: Contents of a traversed entry did not match expected.\n");
            return BCM_E_FAIL;
        }
    }
    /* 9. Cleanups */
    destroyIpmc(unit);
    if (rv != BCM_E_NONE) {
        printf("ipmc_destroy returned with failure code '%s'\n", bcm_errmsg(rv));
        return rv;
    }
    return rv;
}
