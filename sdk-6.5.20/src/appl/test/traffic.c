/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * XGS/StrataSwitch Traffic Generator aka. "Traffic, Traf-Test, Snake, etc."
 *
 * Switching API Stress Test using A/B Ethernet port pairs as
 * load generators. Different from Snake test in that packet flow
 * is between a source and sink port pair (A/B).
 *
 * When stress-testing XGS/StrataSwitch devices, usually, one will require
 * a SmartBits (TM) or IXIA (TM) network testing device to spray
 * frames to all of the ports and make them switch at line rate.
 *
 * The purpose of this test is to configure a pair of ports as
 * transmitters and receivers while the switch ASIC is configured to only
 * forward a frame between these two port pairs.
 *
 * The CPU then injects a frame into the port pairs going one
 * direction or the other (or bidirectional), and the packet loops
 * forever between the port pairs until the H/W fails, or the user
 * stops the test. Typically, this happens at layer 2, using unicast,
 * broadcast, multicast, or VLAN traffic, however extensions for layer3
 * and higher (via the FFP) are possible.
 *
 * When testing, a "golden" switch (one known to be free of H/W bugs)
 * is stress tested and the result is that the port-pairs should
 * forward the frame forever. Failure for the ASIC to do so indicates
 * faulty hardware and the problem should be investigated and
 * debugged.  An interesting feature of the XGS/StrataSwitch family of
 * devices is that they should forward traffic at line rate for all
 * layers of the test (e.g. MAC, PHY, Wire at 10Mbps, 100Mbps, 1Gbps, etc).
 *
 * In addition, this test allows the switch hardware to be used to
 * test itself, without the cost of a SmartBits or IXIA testing device
 * per switch; moreover, this speeds up testing as any switch which
 * passes this traffic test is known to pass a forwarding test between
 * two theoretical hosts.
 *
 *
 * MAC/PHY (Broadcast Test)
 *
 *     In the MAC or PHY tests, a VLAN is created with each port pair
 *     (A/B) as a member. The PVLAN table reflects the same VLAN ID
 *     and an untagged packet is sent out the port (with the ports in
 *     loopback) with the matching VLAN. As the packet ingresses into
 *     the switch, a DLF occurs, and the resulting unknown unicast or
 *     broadcast frame floods the VLAN, and packets swirl between
 *     ports A/B at line rate.
 *
 *     For example, assuming two ports A(ge0), and B (ge1):
 *
 *     vlan create 10 PBM=portA,portB UBM=portA,portB
 *     pvlan set portA VIDA
 *     pvlan set portB VIDA
 *
 *     tx 1 PBM=portA UBM=portA L=68 VL=10 P=0x12345678 PI=1 \
 *     SM=00:00:00:00:00:02 SMI=0 DM=ff:ff:ff:ff:ff:ff DMI=0 \
 *     COS=0 CRC=Recompute
 *
 *     tx 1 PBM=portB UBM=portB L=68 VL=10 P=0x12345678 PI=1 \
 *     SM=00:00:00:00:00:02 SMI=0 DM=ff:ff:ff:ff:ff:ff DMI=0 \
 *     COS=0 CRC=Recompute
 *
 *
 * External (Unicast Test)
 *
 *     In external test mode, a VLAN is created for each port (A/B)
 *     and both ports are in the tagged and untagged bitmaps. Next,
 *     the PVLAN table is setup with a unique vlan for both A and B.
 *     Finally, an L2 address is setup for each port in the ARL with a
 *     MAC address (of the port), however, the VLAN entry points to
 *     the opposite port, so that when a packet comes back in the link
 *     partner's port, it gets retagged with a VLAN which will flood
 *     it to the originator.
 *
 *     For example, assuming two ports A(ge0), and B (ge1):
 *
 *     vlan create A PBM=portA,portB UBM=portA,portB
 *     vlan create B PBM=portA,portB UBM=portA,portB
 *     pvlan set portA A
 *     pvlan set portB B
 *
 *     l2 add PBM=portA MAC=00:00:00:00:00:portA V=B ST=T
 *     l2 add PBM=portB MAC=00:00:00:00:00:portB V=A ST=T
 *
 *     tx 1 PBM=portB UBM=portB L=68 VL=A P=0x12345678 PI=1 \
 *     SM=00:00:00:00:00:portA SMI=0 DM=00:00:00:00:00:portB \
 *     DMI=0 COS=0 CRC=Recompute
 *
 *     tx 1 PBM=portA UBM=portA L=68 VL=B P=0x12345678 PI=1 \
 *     SM=00:00:00:00:00:portB SMI=0 DM=00:00:00:00:00:portA \
 *     DMI=0 COS=0 CRC=Recompute
 *
 *  General
 *
 *    Both ports are setup to transmit and receive a packet (full-duplex)
 *    by default.
 *
 *    The test runs forever, looping and reporting statistics until it
 *    detects that a packet is lost, at which point an error is
 *    reported and relevant statistics from the MAC or PHY are reported.
 *
 *    In general, when bringing up new switch hardware, one should start by
 *    testing at the MAC layer, then moving to the PHY layer, and
 *    finally, externally, by connected each port pair to it's
 *    neighbor (1-2, 3-4, etc).
 */
#include <sal/types.h>
#include <sal/core/boot.h>
#include <sal/appl/sal.h>
#include <shared/bsl.h>
#include <soc/drv.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/test.h>
#include <appl/test/loopback2.h>

#include <soc/mem.h>
#ifdef BCM_ESW_SUPPORT
#include <soc/firebolt.h>
#include <bcm_int/esw/mbcm.h>
#ifdef BCM_ENDURO_SUPPORT
#include <soc/error.h>
#endif
#endif

#include <bcm/error.h>
#include <bcm/l2.h>
#include <bcm/pkt.h>
#include <bcm/tx.h>
#include <bcm/port.h>
#include <bcm/vlan.h>
#include <bcm/link.h>
#include <bcm/stg.h>
#include <bcm/link.h>
#include <bcm/stat.h>
#include <bcm/stack.h>

#if defined(INCLUDE_MACSEC)
#include <bcm/macsec.h>

static int macsec_stop_poll_task = 1;
#endif /* INCLUDE_MACSEC */

#if defined(BCM_GREYHOUND2_SUPPORT)
#include <soc/port_ability.h>
#endif /* BCM_GREYHOUND2_SUPPORT */
#include "traffic.h"

/* Unicast test */
static sal_mac_addr_t traf_unicast_mac_base = {0,0,0,0,0xfe,0};

/* Unicast test */
static sal_mac_addr_t traf_broadcast_mac_base = {0xff,0xff,0xff,0xff,0xff,0xff};


/*
 * Module local variables.
 */

/* Test Structure for the CLI */
STATIC traffic_test_t     *traf_test[SOC_MAX_NUM_DEVICES];


/*
 * The order in this table should mirror traffic_mode_t
 */
STATIC char *
traffic_mode_str[] = {
    "MAC",
    "PHY",
    "External",
    /*    "ExternalUp",
          "ExternalDown", */
    NULL
};

/*
 * The order in this table should mirror traffic_speed_t
 */
STATIC char *
traffic_speed_str[] = {
    LB2_SPEED_INIT_STR,
    NULL
};


/* Test parameter default options */
static int      dfl_speed = 0,          /* MAX */
                dfl_autoneg = 1,
                dfl_size = 1518,
                dfl_count = 1,
                dfl_poll_interval = 5,  /* seconds between polls */
                dfl_runtime = 60,       /* 60/5 = 12 polls */
                dfl_cleanup = 1,
                dfl_showstats = 0,
                dfl_run2end = 0;
static pbmp_t   dfl_portbitmap;
static uint32   dfl_pattern = 0xa5a4a3a2,
                dfl_pattern_inc = 0;
static traffic_mode_t dfl_runmode = TRAFFIC_INTERNAL_PHY;

#if defined(INCLUDE_MACSEC)
static int dfl_macsec = 0;
#endif /* INCLUDE_MACSEC */

/*
 * Function:
 *      _traffic_xgs3_test_modid_get
 * Purpose:
 *      Get a set of modids that do not conflict with my_modid
 * Parameters:
 *      unit  - SOC unit#
 *      port  - test port
 * Returns:
 *      module id
 */
STATIC int
_traffic_xgs3_test_modid_get(int unit, int port)
{
    int modid;

    if (bcm_stk_my_modid_get(unit, &modid) < 0) {
        return -1;
    }    

    if (SOC_IS_RAPTOR(unit) || SOC_IS_RAVEN(unit)) {
        return (modid >= 8) ? port : port + 8;
    } else {
        return (modid >= 32) ? port : port + 32;
    }
}

#ifdef BCM_ESW_SUPPORT
/*
 * Function:
 *      _traffic_xgs3_modport_set
 * Purpose:
 *      Set the port in MODPORT entry for ingress port/modid
 * Parameters:
 *      unit  - SOC unit#
 *      modid - module id
 *      portA - egress Higig port
 *      portB - ingress Higig port
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_traffic_xgs3_modport_set(int unit, int modid, int portA, int portB)
{
    modport_map_entry_t uc;
    modport_map_sw_entry_t sw_entry;
    bcm_pbmp_t hg;
    int idx, rv;
    uint32 hg_reg;

    if (SOC_MEM_FIELD_VALID(unit, MODPORT_MAP_SWm, DEST0f)) {
        idx = modid;
        soc_mem_lock(unit, MODPORT_MAP_SWm);
        rv = soc_mem_read(unit, MODPORT_MAP_SWm, MEM_BLOCK_ANY, idx,
                          &sw_entry);
        if (SOC_SUCCESS(rv)) {
            soc_mem_field32_set(unit, MODPORT_MAP_SWm, &sw_entry, DEST0f,
                                portA);
            soc_mem_field32_set(unit, MODPORT_MAP_SWm, &sw_entry, ENABLE0f, 1);
            soc_mem_field32_set(unit, MODPORT_MAP_SWm, &sw_entry, DEST1f,
                                portB);
            soc_mem_field32_set(unit, MODPORT_MAP_SWm, &sw_entry, ENABLE1f, 1);
            rv = soc_mem_write(unit, MODPORT_MAP_SWm, MEM_BLOCK_ALL, idx,
                               &sw_entry);
        }
        soc_mem_unlock(unit, MODPORT_MAP_SWm);
        return rv;
    }

    BCM_PBMP_CLEAR(hg);

#ifdef BCM_RAPTOR1_SUPPORT
    if (SOC_IS_RAPTOR(unit)) {
        BCM_PBMP_PORT_SET(hg, portA-1);
        BCM_PBMP_PORT_ADD(hg, portB-1);
        LOG_ERROR(BSL_LS_APPL_TESTS,
                  (BSL_META_U(unit,
                              "portA %d portB %d\n"), portA, portB));
        idx = modid;
    } else
#endif
    if (SOC_IS_RAVEN(unit) || SOC_IS_TRIUMPH2(unit) || 
               SOC_IS_APOLLO(unit) || SOC_IS_VALKYRIE2(unit)) {
        BCM_PBMP_PORT_SET(hg, portA);
        BCM_PBMP_PORT_ADD(hg, portB);
        LOG_ERROR(BSL_LS_APPL_TESTS,
                  (BSL_META_U(unit,
                              "portA %d portB %d\n"), portA, portB));
        idx = modid;
    } else if (SOC_IS_FB_FX_HX(unit)) {
        BCM_PBMP_PORT_SET(hg, portA-24);
        BCM_PBMP_PORT_ADD(hg, portB-24);
        idx = modid;
    } else if (SOC_IS_ENDURO(unit) || SOC_IS_HURRICANE(unit)) {
        BCM_PBMP_PORT_SET(hg, portA-26);
        BCM_PBMP_PORT_ADD(hg, portB-26);
        idx = modid;
    } else if (SOC_IS_HBX(unit)) {
        BCM_PBMP_PORT_SET(hg, portB);
        idx = (portA * (SOC_MODID_MAX(unit) + 1)) + modid;
    } else if (SOC_IS_TR_VL(unit)) {
        uint32 hg_regA, hg_regB;
        SOC_IF_ERROR_RETURN
            (soc_xgs3_port_to_higig_bitmap(unit, portA, &hg_regA));
        SOC_IF_ERROR_RETURN
            (soc_xgs3_port_to_higig_bitmap(unit, portB, &hg_regB));
        SOC_PBMP_WORD_SET(hg, 0, hg_regA | hg_regB);
        idx = modid;
    } else {
        return BCM_E_INTERNAL;
    }

    soc_mem_lock(unit, MODPORT_MAPm);

    rv = READ_MODPORT_MAPm(unit, MEM_BLOCK_ANY, idx, &uc);
    if (SOC_SUCCESS(rv)) {
        hg_reg =  SOC_PBMP_WORD_GET(hg, 0);
        if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
            SOC_IS_VALKYRIE2(unit)) {
            soc_MODPORT_MAPm_field32_set(unit, &uc, HIGIG_PORT_BITMAP_LOf, 
                                         hg_reg);
            hg_reg = SOC_PBMP_WORD_GET(hg, 1);
            soc_MODPORT_MAPm_field32_set(unit, &uc, HIGIG_PORT_BITMAP_HIf, 
                                         hg_reg);
        } else {
            soc_MODPORT_MAPm_field32_set(unit, &uc, HIGIG_PORT_BITMAPf, hg_reg);
        }
        rv = WRITE_MODPORT_MAPm(unit, MEM_BLOCK_ALL, idx, &uc);
    }

    soc_mem_unlock(unit, MODPORT_MAPm);

    return rv;
}
#endif /* BCM_ESW_SUPPORT */

/*
 * Save H/W state and setup test structure for a given unit with the
 * specified mode. If the test_structure is not allocated, a new one
 * is created and returned with the mode settings specified.
 */

STATIC traffic_test_t *
traffic_test_alloc(int unit)
{
    int port, rv = -1;
    traffic_test_t *test;
    bcm_port_abil_t    ability;

    test = sal_alloc(sizeof(traffic_test_t), "Traffic Test config");
    test->port_stats = sal_alloc(SOC_MAX_NUM_PORTS *
                                 sizeof(traffic_port_stat_t),
                                 "Stats");
    test->port_pair = sal_alloc(SOC_MAX_NUM_PORTS *
                                sizeof(traffic_port_config_t),
                                "Port Config");
    test->port_info = sal_alloc(SOC_MAX_NUM_PORTS *
                                sizeof(bcm_port_info_t),
                                "Port Test Configuration");
    test->saved_port_info = sal_alloc(SOC_MAX_NUM_PORTS *
                                      sizeof(bcm_port_info_t),
                                      "Original Port Configuration");
#ifdef BCM_ESW_SUPPORT
    if (SOC_MEM_FIELD_VALID(unit, MODPORT_MAP_SWm, DEST0f)) {
        test->saved_modport_map_sw = sal_alloc(SOC_MAX_NUM_PORTS *
                                               sizeof(modport_map_sw_entry_t),
                                               "Original Modport Map");
    } else {
        test->saved_modport_map = sal_alloc(SOC_MAX_NUM_PORTS *
                                            sizeof(modport_map_entry_t),
                                            "Original Modport Map");
    }
#endif /* BCM_ESW_SUPPORT */

    test->unit = unit;
    test->npkts = dfl_count;
    test->pattern = dfl_pattern;
    test->pattern_inc = dfl_pattern_inc;
    test->ports = dfl_portbitmap;
    test->pkt_size = dfl_size;
    test->mode = dfl_runmode;
    test->req_speed = dfl_speed;
    test->stop_on_fail = TRUE;
    test->cleanup = dfl_cleanup;
    test->showstats = dfl_showstats;
    test->poll_interval = dfl_poll_interval;
    test->runtime = dfl_runtime;
    test->run2end = dfl_run2end;
#if defined(INCLUDE_MACSEC)
    test->macsec_enabled = dfl_macsec;
    test->macsec_info = sal_alloc(sizeof(traffic_macsec_info_t),
                                            "macsec traffic info");
    test->macsec_info->num_assoc = 1;
#endif /* INCLUDE_MACSEC */

    /* Save age timer if it's set */
    bcm_l2_age_timer_get(test->unit, &test->l2agetime);

    if (test->mode == TRAFFIC_INTERNAL_PHY) {
        PBMP_ITER(test->ports, port) {
           if ( bcm_port_ability_get(unit, port, &ability) != BCM_E_NONE) {
                test_error(unit,"======> ERROR : fail to get ability Port %s\n", SOC_PORT_NAME(unit, port));
            }
            if (!(ability & BCM_PORT_ABIL_LB_PHY)) {
                SOC_PBMP_PORT_REMOVE(test->ports, port);
                test_msg("Port %s: Doesn't support PHY loopback. Skip it\n", SOC_PORT_NAME(unit, port));
            }
        }
     }

    /* Cache original port config */
    PBMP_ITER(test->ports, port) {
        if ((rv = bcm_port_info_save(unit, port,
                        &test->saved_port_info[port])) < 0) {
            cli_out("traffic_init: port %s: could not get port info: %s\n",
                    SOC_PORT_NAME(unit, port), bcm_errmsg(rv));
        }
        /* Remove L2 addresses on Switch ASIC's which support them */
        /* this is a no-op on fabrics */
        rv = bcm_l2_addr_delete_by_port(unit, -1, port, BCM_L2_DELETE_STATIC);
        if (rv < 0) {
            cli_out("traffic_test_free: ARL delete by port unsuccessful on port %s: could not set port info: %s\n",
                    SOC_PORT_NAME(unit, port),
                    bcm_errmsg(rv));
        }
#ifdef BCM_ESW_SUPPORT
        /* Save modport maps used for Higig port pairs */
        if (IS_ST_PORT(unit, port) && SOC_IS_FBX(unit)) {
            int modid, idx;
            modid = _traffic_xgs3_test_modid_get(unit, port);
            if (modid < 0) {
                cli_out("traffic_init: port %s: could not get modid\n",
                        SOC_PORT_NAME(unit, port));
            }
            if (SOC_IS_HBX(unit)) {
                idx = (port * (SOC_MODID_MAX(unit) + 1)) + modid;
            } else {
                idx = modid;
            }
            if (SOC_MEM_FIELD_VALID(unit, MODPORT_MAP_SWm, DEST0f)) {
                rv = soc_mem_read(unit, MODPORT_MAP_SWm, MEM_BLOCK_ANY, idx,
                                  &test->saved_modport_map_sw[port]);
            } else {
                rv = soc_mem_read(unit, MODPORT_MAPm, MEM_BLOCK_ANY, idx,
                                  &test->saved_modport_map[port]);
            }
            if (rv < 0) {                                            
                cli_out("traffic_init: "
                        "port %s: could not get modport map: %s\n",
                        SOC_PORT_NAME(unit, port), bcm_errmsg(rv));
            }
        }
#endif /* BCM_ESW_SUPPORT */
    }

    return test;
}

/*
 * Free Test structure, restore H/W state.
 */
STATIC int
traffic_test_free(traffic_test_t* test)
{
    int         unit, port, rv = -1;
    int         ms;
#if defined (BCM_ESW_SUPPORT)  
    uint64      tmp64;
#endif

    if (!test) {
        return rv;
    }

    /*
     * Cleanup means shutdown switching test, we have this
     * option so that on a failure, you can exit the test
     * and use the CLI to debug.
     */
    unit = test->unit;
    if (test->cleanup) {

        if (!SAL_BOOT_QUICKTURN) {
#if defined (BCM_ESW_SUPPORT)  
            COMPILER_64_ZERO(tmp64);
#endif                    
            if (SOC_IS_ESW(unit)) {
#ifdef BCM_ESW_SUPPORT                    
                counter_val_set_by_port(unit, test->ports, tmp64);
#endif
            } else {
            }
        }

        if (!SOC_IS_XGS12_FABRIC(unit)) {
            PBMP_ITER(test->ports, port) {
                if (port != test->port_pair[port].portA) {
                    continue;
                }
                cli_out("TRAFFIC: restoring ports: %s, %s\n",
                        SOC_PORT_NAME(unit, test->port_pair[port].portA),
                        SOC_PORT_NAME(unit, test->port_pair[port].portB));

                bcm_vlan_destroy(unit, test->port_pair[port].vidA);
                bcm_vlan_destroy(unit, test->port_pair[port].vidB);

                /* Very Important: clear EPCLINKr to halt traffic */
                bcm_port_stp_set(unit, test->port_pair[port].portA,
                                 BCM_STG_STP_DISABLE);
                bcm_port_stp_set(unit, test->port_pair[port].portB,
                                 BCM_STG_STP_DISABLE);
            }
            bcm_vlan_port_add(unit, VLAN_ID_DEFAULT, test->ports, test->ports);
        }

        PBMP_ITER(test->ports, port) {
            /* If port is FE port, remove phy_master attributes controlled by it */
            if(IS_FE_PORT(unit, port)) {
              test->saved_port_info[port].action_mask &= ~BCM_PORT_ATTR_PHY_MASTER_MASK;
            }
            /* If the Master mode is not changed, remove phy_master attributes from action_mask */
            ms = 0;
            if ((rv = bcm_port_master_get(unit, port, &ms)) != BCM_E_NONE) {
                cli_out("traffic_test_free: port %s: could not get port Master mode: %s\n",
                        SOC_PORT_NAME(unit, port),
                        bcm_errmsg(rv));
                goto done;
            }
            if (ms == test->saved_port_info[port].phy_master) {
               test->saved_port_info[port].action_mask &= ~BCM_PORT_ATTR_PHY_MASTER_MASK;
            }

            if ((rv = bcm_port_info_restore(unit, port,
                                        &test->saved_port_info[port])) < 0) {
                cli_out("traffic_test_free: port %s: could not set port info: %s\n",
                        SOC_PORT_NAME(unit, port),
                        bcm_errmsg(rv));
                goto done;
            }
            rv = bcm_l2_addr_delete_by_port(unit, -1, port,
                                            BCM_L2_DELETE_STATIC);
            if (rv < 0) {
                cli_out("traffic_test_free: ARL delete by port unsuccessful on port %s: could not set port info: %s\n",
                        SOC_PORT_NAME(unit, port),
                        bcm_errmsg(rv));
                goto done;
            }

#ifdef BCM_ESW_SUPPORT
            /* Restore modport maps used for Higig port pairs */
            if (IS_ST_PORT(unit, port) && SOC_IS_FBX(unit)) {
                int modid, idx;
                modid = _traffic_xgs3_test_modid_get(unit, port);
                if (modid < 0) {
                    cli_out("traffic_test_free: port %s: could not get modid\n",
                            SOC_PORT_NAME(unit, port));
                    goto done;
                }
                if (SOC_IS_HBX(unit) || SOC_IS_RAPTOR(unit) || SOC_IS_RAVEN(unit)) {
                    idx = (port * (SOC_MODID_MAX(unit) + 1)) + modid;
                } else {
                    idx = modid;
                }
                if (SOC_MEM_FIELD_VALID(unit, MODPORT_MAP_SWm, DEST0f)) {
                    rv = soc_mem_write(unit, MODPORT_MAP_SWm, MEM_BLOCK_ANY,
                                       idx, &test->saved_modport_map_sw[port]);
                } else {
                    rv = soc_mem_write(unit, MODPORT_MAPm, MEM_BLOCK_ANY, idx, 
                                       &test->saved_modport_map[port]);
                }
                if (rv  < 0) {
                    cli_out("traffic_test_free: "
                            "port %s: could not set modport map: %s\n",
                            SOC_PORT_NAME(unit, port), bcm_errmsg(rv));
                }
            }
#endif /* BCM_ESW_SUPPORT */
        }

        /* Restore l2 age time */
        bcm_l2_age_timer_set(unit, test->l2agetime);
    }
 done:
    sal_free(test->port_stats);
    sal_free(test->port_pair);
    sal_free(test->port_info);
    sal_free(test->saved_port_info);
#ifdef BCM_ESW_SUPPORT
    if (SOC_MEM_FIELD_VALID(unit, MODPORT_MAP_SWm, DEST0f)) {
        sal_free(test->saved_modport_map_sw);
    } else {
        sal_free(test->saved_modport_map);
    }
#endif /* BCM_ESW_SUPPORT */
    return 0;
}

/*
 * Send frames to destination port pair index. Each port is a
 * transmitter and receiver. Packets are sent in external mode
 * such that  DA=00:00:00:00:fe:x where x=portA, and
 * SA=00:00:00:00:00:portB; in internal test mode, packets
 * are sent with SA=00:00:00:00:00:01,  and DA=broadcast address
 */
int
traffic_test_send_frames(traffic_test_t* test, int port_pair_index)
{
    int rv = BCM_E_NONE;
    int unit = test->unit;
    traffic_port_config_t *config = &test->port_pair[port_pair_index];
    int portA = config->portA;
    int portB = config->portB;
    int mode = test->mode;
    int pad;
    int i;

    bcm_pkt_t *pktA;
    void *bufA;
    enet_hdr_t *ehdrA;
    int sizeA;

    bcm_pkt_t *pktB;
    void *bufB;
    enet_hdr_t *ehdrB;
    int sizeB;

    pad = sizeof(uint32); /* Add size of CRC .. */

    bcm_pkt_alloc(unit, test->pkt_size + pad, 0, &pktA);
    bcm_pkt_alloc(unit, test->pkt_size + pad, 0, &pktB);

    if (pktA == NULL || pktB == NULL) {
        cli_out("Error initializing packet buffers\n");
        goto error;
    }

    bufA = pktA->_pkt_data.data;
    bufB = pktB->_pkt_data.data;

    /* Set packet buffer */
    ehdrA = (enet_hdr_t*)bufA;
    ehdrB = (enet_hdr_t*)bufB;

    pktA->flags = BCM_TX_CRC_APPEND;
    pktB->flags = BCM_TX_CRC_APPEND;
#ifdef BCM_XGS12_FABRIC_SUPPORT
    if (SOC_IS_XGS12_FABRIC(unit)) {
        pktA->dest_port = portB;
        pktA->opcode = BCM_HG_OPCODE_UC;
        pktB->dest_port = portA;
        pktB->opcode = BCM_HG_OPCODE_UC;
    }
#endif
    if (SOC_IS_XGS3_SWITCH(unit)) {

        if (IS_ST_PORT(unit, portA) || IS_ST_PORT(unit, portB)) {
            int modid = _traffic_xgs3_test_modid_get(unit, portA);
            if (modid < 0) {
                cli_out("ERROR: port %s: could not get modid\n",
                        SOC_PORT_NAME(unit, portA));
                goto error;
            }
            pktA->dest_port = portB;
            pktA->opcode = BCM_HG_OPCODE_UC;
            pktA->src_mod = modid + 1;
            pktA->flags |= BCM_TX_SRC_MOD;
            pktA->dest_mod = modid;
            pktB->dest_port = portA;
            pktB->opcode = BCM_HG_OPCODE_UC;
            pktB->src_mod = modid + 1;
            pktB->flags |= BCM_TX_SRC_MOD;
            pktB->dest_mod = modid;
        }
    }
    test->pattern = packet_store((uint8*)ehdrA, test->pkt_size,
                                 test->pattern, test->pattern_inc);

    /* Strata Internal MAC, PHY, or XGS/Fabric packets */
    if (SOC_IS_XGS12_FABRIC(unit) ||
        (mode == TRAFFIC_INTERNAL) ||
        (mode == TRAFFIC_INTERNAL_PHY)) {

        /* PortA packet */
        ENET_COPY_MACADDR(traf_broadcast_mac_base, &ehdrA->en_dhost);
        ENET_COPY_MACADDR(traf_unicast_mac_base,   &ehdrA->en_shost);
        if (!SOC_IS_XGS12_FABRIC(unit)) {
            ehdrA->en_tag_ctrl = soc_htons(VLAN_CTRL(0, 0, config->vidA));
            ehdrA->en_tag_tpid = soc_htons(ENET_DEFAULT_TPID );
        }
        ehdrA->en_tag_len  = soc_htons(test->pkt_size) - 18;

        sizeA = test->pkt_size;
        BCM_PKT_PORT_SET(pktA, portA, TRUE, FALSE);
        BCM_PKT_TX_LEN_SET(pktA, sizeA);

        /* Port B packet */
        ENET_COPY_MACADDR(traf_broadcast_mac_base, &ehdrB->en_dhost);
        ENET_COPY_MACADDR(traf_unicast_mac_base,   &ehdrB->en_shost);
        if (!SOC_IS_XGS12_FABRIC(unit)) {
            ehdrB->en_tag_ctrl = soc_htons(VLAN_CTRL(0, 0, config->vidB));
            ehdrB->en_tag_tpid = soc_htons(ENET_DEFAULT_TPID );
        }
        ehdrB->en_tag_len  = soc_htons(test->pkt_size) - 18;

        sizeB = test->pkt_size;
        BCM_PKT_PORT_SET(pktB, portB, TRUE, FALSE);
        BCM_PKT_TX_LEN_SET(pktB, sizeB);
    } else { /* External && !Fabric */
        /* PortA packet */
        ENET_COPY_MACADDR(&config->l2B.mac, &ehdrA->en_dhost);
        ENET_COPY_MACADDR(&config->l2A.mac, &ehdrA->en_shost);
        ehdrA->en_tag_ctrl = soc_htons(VLAN_CTRL(0, 0, config->vidA));
        ehdrA->en_tag_tpid = soc_htons(ENET_DEFAULT_TPID );
        ehdrA->en_tag_len  = soc_htons(test->pkt_size) - 18;

        sizeA = test->pkt_size;
        BCM_PKT_PORT_SET(pktA, portB, TRUE, FALSE);
        BCM_PKT_TX_LEN_SET(pktA, sizeA);

        /* Port B packet */
        ENET_COPY_MACADDR(&config->l2A.mac, &ehdrB->en_dhost);
        ENET_COPY_MACADDR(&config->l2B.mac, &ehdrB->en_shost);
        ehdrB->en_tag_ctrl = soc_htons(VLAN_CTRL(0, 0, config->vidB));
        ehdrB->en_tag_tpid = soc_htons(ENET_DEFAULT_TPID );
        ehdrB->en_tag_len  = soc_htons(test->pkt_size) - 18;

        sizeB = test->pkt_size;
        BCM_PKT_PORT_SET(pktB, portA, TRUE, FALSE);
        BCM_PKT_TX_LEN_SET(pktB, sizeB);
    }

    for (i=0; i < test->npkts; i++) {
        rv = bcm_tx(unit, pktA, NULL);
        if (rv < 0) {
            cli_out("ERROR: port %s: bcm_tx A #%d failed: %s\n",
                    SOC_PORT_NAME(unit, portB), i, bcm_errmsg(rv));
            goto error;
        }

        rv = bcm_tx(unit, pktB, NULL);
        if (rv < 0) {
            cli_out("ERROR: port %s: bcm_tx B #%d failed: %s\n",
                    SOC_PORT_NAME(unit, portA), i, bcm_errmsg(rv));
            goto error;
        }
    }

 error:
    if (pktA != NULL) {
        bcm_pkt_free(unit, pktA);
    }
    if (pktB != NULL) {
        bcm_pkt_free(unit, pktB);
    }

    return BCM_E_NONE;
}

#if defined(INCLUDE_MACSEC)
typedef struct traffic_macsec_user_data_s {
    int                 unit;
    bcm_port_t          match_port;
    bcm_macsec_core_t   dev_core;
    int                 dev_port;
    int                 found;
    pbmp_t              pbm;
    traffic_test_t     *test;
} traffic_macsec_user_data_t;

static int _traffic_macsec_port_iter_cb(int unit, 
                                        bcm_port_t port, 
                                        bcm_macsec_core_t dev_core, 
                                        bcm_macsec_dev_addr_t dev_addr, 
                                        int dev_port, 
                                        bcm_macsec_dev_io_t devio_f, 
                                        void *user_data)
{
    traffic_macsec_user_data_t *mdata = (traffic_macsec_user_data_t*) user_data;

    if ((mdata->unit == unit) && (mdata->match_port == port)) {
         mdata->found = 1;
         mdata->dev_port = dev_port;
         mdata->dev_core = dev_core;
    }
    return 0;
}

static int _traffic_port_is_macsec_capable(int unit, bcm_port_t port)
{
    traffic_macsec_user_data_t mdata;

    mdata.found = 0;
    mdata.unit = unit;
    mdata.match_port = port;
    
    bcm_macsec_port_traverse(unit, _traffic_macsec_port_iter_cb, (void*) &mdata);
    return (mdata.found) ? 1 : 0;
}

static int
_traffic_alloc_crypto_key(int unit, int port, int ofst, unsigned char *key)
{
    int ii;

    key[0] = unit;
    key[1] = port;
    key[2] = (ofst >> 8) & 0xff;
    key[3] = ofst & 0xff;
    for (ii = 4; ii < 16; ii++) {
        key[ii] = ii;
    }
    return 0;
}

static int _traffic_setup_new_key(int unit, bcm_port_t port, 
                              bcm_port_t portB, traffic_test_t *test)
{
    int symmetric, an, peer_port, rxchanId, txchanId;
    int old_rx_assocId = -1, old_tx_assocId = -1, new_an, rv = BCM_E_NONE;
    bcm_macsec_secure_assoc_t assoc;

    symmetric = ((test->mode == TRAFFIC_INTERNAL_PHY) ||
                 (test->mode == TRAFFIC_INTERNAL)) ? 1 : 0;

    if (symmetric) {
        peer_port = port;
    } else {
        peer_port = portB;
    }
    if (peer_port >= SOC_MAX_NUM_PORTS) {
        cli_out("peer_port num %d too large \n", peer_port );
        return BCM_E_PORT;
    }
    if (port >= SOC_MAX_NUM_PORTS) {
        cli_out("port num %d too large \n", port );
        return BCM_E_PORT;
    }

    an = test->macsec_info->ports[port].cur_an;
    if (an >= 0) {
        old_tx_assocId = 
                test->macsec_info->ports[port].tx_keys[an].assocId;
        old_rx_assocId = 
                test->macsec_info->ports[peer_port].rx_keys[an].assocId;
    }

    new_an = (an + 1) & 3;

    test->macsec_info->ports[port].cur_an = new_an;

    /* Update RX */
    sal_memset(&assoc, 0, sizeof(bcm_macsec_secure_assoc_t));
    assoc.an = new_an;
    assoc.crypto = BCM_MACSEC_CRYPTO_AES_128_GCM;
    assoc.flags = BCM_MACSEC_ASSOC_ACTIVE;

    /* setup TX crypto key */
    _traffic_alloc_crypto_key(unit, port, 
                              test->macsec_info->ports[port].num_alloc++,
                              assoc.aes128_gcm.key);

    /* store the key */
    sal_memcpy(test->macsec_info->ports[port].tx_keys[new_an].crypto_key,
               assoc.aes128_gcm.key, 16);

    rxchanId = test->macsec_info->ports[peer_port].rx_chanId;

    /* create RX assoc */
    rv = bcm_macsec_secure_assoc_create(unit, peer_port, 0, rxchanId, &assoc, 
                 &test->macsec_info->ports[peer_port].rx_keys[new_an].assocId);
    if (rv < 0) {
        cli_out("Failed to create RX secure association \n");
        return BCM_E_PORT;
    }

    sal_memset(&assoc, 0, sizeof(bcm_macsec_secure_assoc_t));
    assoc.an = new_an;
    assoc.flags = BCM_MACSEC_ASSOC_ACTIVE;
    assoc.crypto = BCM_MACSEC_CRYPTO_AES_128_GCM;
    sal_memcpy(assoc.aes128_gcm.key,
               test->macsec_info->ports[port].tx_keys[new_an].crypto_key, 16);

    /* switch over now */
    txchanId = test->macsec_info->ports[port].tx_chanId;
    rv = bcm_macsec_secure_assoc_create(unit, port, 0, txchanId, &assoc, 
                            &test->macsec_info->ports[port].tx_keys[new_an].assocId);
    if (rv < 0) {
        cli_out("Failed to create TX secure association \n");
        return BCM_E_PORT;
    }
    test->macsec_info->ports[port].cur_an = new_an;

    if (old_tx_assocId >= 0) {
        bcm_macsec_secure_assoc_destroy(unit, port, old_tx_assocId);
    }

    if (old_rx_assocId >= 0) {
        bcm_macsec_secure_assoc_destroy(unit, port, old_rx_assocId);
    }

    cli_out("Key updated (p=%s) \n", SOC_PORT_NAME(unit, port));
    return BCM_E_NONE;
}

static int _macsec_calc_assoc_life(int speedM, uint32 availPkt, int before)
{
    uint32 time_ms;

    /* calculate worst cast life of assoc */
    /*
    time_ms = (availPkt ) / (speedM * 1000);
    time_ms *=  64 * 8;
    */

    switch (speedM) {
        case 10:
            time_ms = availPkt / 20;
            break;
        case 100:
            time_ms = availPkt / 195;
            break;
        case 1000:
            time_ms = availPkt / 1953;
            break;
        case 10000:
        default:
            time_ms = availPkt / 19531;
            break;
    }

    return (time_ms <= before) ? 0 : (time_ms - before);
}

static void
_macsec_poll_thread(void *data)
{
    traffic_macsec_user_data_t  *mdata = (traffic_macsec_user_data_t*) data;
    traffic_test_t  *test;
    pbmp_t      pbm;
    int         unit, cur_an, rv;
    bcm_port_t  port, peer_port;
    int         poll_interval, total_time = 0;
    int         wake_time_ms;
    unsigned int    avail_pkt;
    int symmetric, rxchanId, txchanId;
    bcm_macsec_port_config_t config;
    

    /* setup macsec PBMP and */
    test = mdata->test;
    pbm  = test->macsec_info->macsec_pbm;
    unit = mdata->unit;

    if (SOC_PBMP_IS_NULL(pbm)) {
        return;
    }

    while (macsec_stop_poll_task == 0) {
        wake_time_ms = 10000;
        PBMP_ITER(pbm, port) {
            cur_an = test->macsec_info->ports[port].cur_an;
            /* Get the availPkt count for the assoc */
            rv = bcm_macsec_stat_get32(unit, port, bcm_txSAavailablePkts, -1,
                        test->macsec_info->ports[port].tx_keys[cur_an].assocId,
                        &avail_pkt);
            if (rv < 0) {
                continue;
            }
            poll_interval = _macsec_calc_assoc_life(test->port_info[port].speed,
                                                    avail_pkt, 10000);
            if (poll_interval == 0) {
                peer_port = test->port_pair[port].portB;
                /* Update the keys */
                _traffic_setup_new_key(unit, port, peer_port, test);
                continue;
            }
            if (wake_time_ms > poll_interval) {
                wake_time_ms = poll_interval;
            }
        }
        sal_usleep(wake_time_ms * 1000);
        total_time += wake_time_ms;
    }

    symmetric = ((test->mode == TRAFFIC_INTERNAL_PHY) ||
                 (test->mode == TRAFFIC_INTERNAL)) ? 1 : 0;

    if (test->cleanup) {
        PBMP_ITER(pbm, port) {
            if (symmetric) {
                peer_port = port;
            } else {
                peer_port = test->port_pair[port].portB;
            }
            if (peer_port >= SOC_MAX_NUM_PORTS) {
                cli_out("peer_port num %d  >=  %d : MACSEC clean failed\n", 
                        peer_port, SOC_MAX_NUM_PORTS);
                        
                continue;
            }
            txchanId = test->macsec_info->ports[port].tx_chanId;
            if (txchanId >= 0) {
                bcm_macsec_secure_chan_destroy(unit, port, txchanId);
            }
            rxchanId = test->macsec_info->ports[peer_port].rx_chanId;
            if (rxchanId >= 0) {
                bcm_macsec_secure_chan_destroy(unit, peer_port, rxchanId);
            }

            if (bcm_macsec_port_config_get(unit, port, &config) < 0) {
                cli_out("Port %s MACSEC clean failed\n", 
                        SOC_PORT_NAME(unit, port));
                continue;
            }

            config.flags &= ~BCM_MACSEC_PORT_ENABLE;
            if (bcm_macsec_port_config_set(unit, port, &config) < 0) {
                cli_out("Port %s MACSEC clean failed\n", 
                        SOC_PORT_NAME(unit, port));
                continue;
            }
        }
    }

    cli_out("MACSEC Poll task exiting ...\n");
    return;
}

static int 
_traffic_setup_macsec_poll_task(int unit, traffic_test_t* test)
{
    traffic_macsec_user_data_t  mdata;

    if (!test->macsec_enabled) {
        return 0;
    }

    macsec_stop_poll_task = 0;

    /* fill up data for poller task */
    mdata.unit = unit;
    mdata.test = test;

    /* Start task to renew the key when secure assoc expires */
    if (sal_thread_create("macsec_sa_poller",
                          SAL_THREAD_STKSZ, 100,
                          (void (*)(void*))_macsec_poll_thread,
                              (void*) &mdata) == SAL_THREAD_ERROR) {
        return BCM_E_MEMORY;
    }
    return 0;
}   

static int 
_traffic_setup_macsec(int unit, bcm_port_t portA, 
                                 bcm_port_t portB, traffic_test_t* test)
{
    bcm_port_t  port, alt_port, peer_port;
    bcm_macsec_port_config_t config;
    bcm_macsec_secure_chan_t    chan;
    int         txchanId, rxchanId;
    int         symmetric, ii, flowId, jj;
    bcm_macsec_flow_match_t flow;
    bcm_macsec_flow_action_t action;

    symmetric = ((test->mode == TRAFFIC_INTERNAL_PHY) ||
                 (test->mode == TRAFFIC_INTERNAL)) ? 1 : 0;

    /* Setup MACSEC flows and keys */
    if ((test->macsec_enabled == 0) ||
        ((symmetric == 0) &&
         (!_traffic_port_is_macsec_capable(unit, portA) ||
          !_traffic_port_is_macsec_capable(unit, portB)))) {
        return 0; /* Not macsec capable, nothing to do. */
    }

    for (ii = 0; ii < 2; ii++) {
        port = (ii == 0) ? portA : portB;
        alt_port = (ii == 0) ? portB : portA;
        peer_port = (symmetric) ? port : alt_port;

        if (!_traffic_port_is_macsec_capable(unit, port)) {
            continue;
        }

        /* Enable controlled port */
        if (bcm_macsec_port_config_get(unit, port, &config) < 0) {
            return BCM_E_PORT;
        }

        config.flags = BCM_MACSEC_PORT_ENABLE;
        config.max_frame = 1518 + 32;
        for (jj = 0; jj < BCM_MACSEC_PACKET_FORMAT__COUNT; jj++) {
            config.egress_flow_match_set[jj] = BCM_MACSEC_FLOW_MATCH_MACSA;
        }
        if (bcm_macsec_port_config_set(unit, port, &config) < 0) {
            return BCM_E_PORT;
        }

        /* Create TX and RX channels */
        sal_memset(&chan, 0, sizeof(chan));
        chan.flags = BCM_MACSEC_SECURE_CONFIDENTIAL | 
                     BCM_MACSEC_SECURE_SECTAG_TX_SCI;
        sal_memset(chan.sci, 0x33, 8);
        if (bcm_macsec_secure_chan_create(unit, port, 0, &chan, &txchanId) < 0) {
            return BCM_E_PORT;
        }

        test->macsec_info->ports[port].tx_chanId = txchanId;

        /* setup rx channel */
        sal_memset(&chan, 0, sizeof(chan));
        chan.flags = BCM_MACSEC_SECURE_CONFIDENTIAL | 
                                            BCM_MACSEC_SECURE_CHAN_INGRESS;
        sal_memset(chan.sci, 0x33, 8);
        if (bcm_macsec_secure_chan_create(unit, peer_port, 
                                                0, &chan, &rxchanId) < 0) {
            return BCM_E_PORT;
        }

        test->macsec_info->ports[peer_port].rx_chanId = rxchanId;

        /* setup egress flows */
        sal_memset(&flow, 0, sizeof(flow));
        flow.flags = 0;
        flow.direction = BCM_MACSEC_DIR_EGRESS;
        flow.pkt_format = BCM_MACSEC_PACKET_FORMAT_802_3;

        sal_memset(&action, 0, sizeof(action));
        action.flags = BCM_MACSEC_FLOW_ACTION_CONTROLLED_FORWARD;

        if (bcm_macsec_flow_create(unit, port, 0, &flow, &action, &flowId) < 0) {
            return BCM_E_PORT;
        }
    
        BCM_PBMP_PORT_ADD(test->macsec_info->macsec_pbm, port);

        test->macsec_info->ports[port].cur_an = -1;

        _traffic_setup_new_key(unit, port, alt_port, test);
    }

    return BCM_E_NONE;
}

#endif /* INCLUDE_MACSEC */

/*
 * Setup an l2 entry with specified address on
 * the given port.
 */
int
traffic_test_setup_l2addr(int unit, bcm_l2_addr_t* l2addr, int vid, int port)
{
    int rv = BCM_E_NONE;
#ifdef BCM_TRIUMPH_SUPPORT
    int ext_index_max = -1;
#endif /* BCM_TRIUMPH_SUPPORT */

    traf_unicast_mac_base[5] = port;
    bcm_l2_addr_t_init(l2addr, traf_unicast_mac_base, vid);

    l2addr->flags = BCM_L2_STATIC;
    l2addr->port = port;
    if ((rv = bcm_stk_my_modid_get(unit, &l2addr->modid)) < 0) {
        cli_out("ERROR: could not get modid: %s\n", bcm_errmsg(rv));
        return rv;
    }
    if (SOC_PORT_MOD1(unit, port)) {
        l2addr->modid += 1;
    }

#ifdef BCM_TRIUMPH_SUPPORT
    if (soc_mem_is_valid(unit, EXT_L2_ENTRYm)) {
        ext_index_max = SOC_PERSIST(unit)->memState[EXT_L2_ENTRYm].index_max;
        SOC_PERSIST(unit)->memState[EXT_L2_ENTRYm].index_max = -1;
    }
#endif /* BCM_TRIUMPH_SUPPORT */

    if ((rv = bcm_l2_addr_add(unit, l2addr)) < 0) {
        cli_out("ERROR: l2 entry add failed: "
                "port %s, mac %2x:%2x:%2x:%2x:%2x:%2x vlan %d: %s\n",
                SOC_PORT_NAME(unit, port),
                traf_unicast_mac_base[0],
                traf_unicast_mac_base[1],
                traf_unicast_mac_base[2],
                traf_unicast_mac_base[3],
                traf_unicast_mac_base[4],
                traf_unicast_mac_base[5],
                vid,
                bcm_errmsg(rv));
    }
#ifdef BCM_TRIUMPH_SUPPORT
    if (soc_mem_is_valid(unit, EXT_L2_ENTRYm)) {
        SOC_PERSIST(unit)->memState[EXT_L2_ENTRYm].index_max = ext_index_max;
    }
#endif /* BCM_TRIUMPH_SUPPORT */
    return rv;
}

int
traffic_test_setup_maxframe(traffic_test_t *test)
{
    pbmp_t          pbm;
    int             unit, max_pkt;
    soc_port_t      port;

    unit = test->unit;

    BCM_PBMP_ASSIGN(pbm, PBMP_E_ALL(unit));
    BCM_PBMP_AND(pbm, test->ports);
    BCM_PBMP_ITER(pbm, port) {
        BCM_IF_ERROR_RETURN
            (bcm_port_frame_max_get(unit, port, &max_pkt));
        if (max_pkt < test->pkt_size) {
            BCM_IF_ERROR_RETURN
                (bcm_port_frame_max_set(unit, port, test->pkt_size));
        }
    }

    return BCM_E_NONE;
}

/*
 * Setup forwarding to occur between port pairs.
 */
int
traffic_test_setup_port_pair(traffic_test_t* test, int port_pair_index,
                             traffic_mode_t mode, traffic_speed_t speed)
{
    int                 unit = test->unit;
    traffic_port_config_t       *config = &test->port_pair[port_pair_index];
    bcm_port_info_t     *infoA = &test->port_info[config->portA];
    bcm_port_info_t     *infoB = &test->port_info[config->portB];
    int                 portA = config->portA;
    int                 portB = config->portB;
    int                 rv;
    int                 ms;
    pbmp_t              port_pbm;
    bcm_port_resource_t portResrc;

    if (SOC_IS_XGS3_SWITCH(unit)) {
        if (IS_ST_PORT(unit, portA) || IS_ST_PORT(unit, portB)) {
            if (!(IS_ST_PORT(unit, portA) && IS_ST_PORT(unit, portB))) {
                cli_out("ERROR: ports %s, %s: "
                        "Higig port must be paired with Higig port\n",
                        SOC_PORT_NAME(unit, portA),
                        SOC_PORT_NAME(unit, portB));
                return -1;
            }
        }
    }

    /* Setup Ports */
    if (bcm_port_info_save(unit, portA, infoA) < 0) {
        cli_out("ERROR: traffic setup unit %d, ports %s, %s: "
                "%s info save failed\n",
                unit,
                SOC_PORT_NAME(unit, portA),
                SOC_PORT_NAME(unit, portB),
                SOC_PORT_NAME(unit, portA));
        return -1;
    }
    if (bcm_port_info_save(unit, portB, infoB) < 0) {
        cli_out("ERROR: traffic setup unit %d, ports %s, %s: "
                "%s info save failed\n",
                unit,
                SOC_PORT_NAME(unit, portA),
                SOC_PORT_NAME(unit, portB),
                SOC_PORT_NAME(unit, portB));
        return -1;
    }

    /* Get port ability  */
    if (!IS_CD_PORT(unit, portA)) {
        if (bcm_port_ability_local_get(unit, portA, &infoA->port_ability) < 0) {
                    cli_out("ERROR: traffic setup unit %d, ports %s: "
                            "ability get failed\n",
                            unit, SOC_PORT_NAME(unit, portA));
                    return -1;
        }
    }
    if (!IS_CD_PORT(unit, portB)) {
        if (bcm_port_ability_local_get(unit, portB, &infoB->port_ability) < 0) {
                    cli_out("ERROR: traffic setup unit %d, ports %s: "
                            "ability get failed\n",
                            unit, SOC_PORT_NAME(unit, portB));
                    return -1;
        }
    }
    switch (speed) {
    case SPEED_10:
        if ((infoA->port_ability.speed_full_duplex & SOC_PA_SPEED_10MB) && 
            (infoB->port_ability.speed_full_duplex & SOC_PA_SPEED_10MB)) {
            infoA->duplex = TRUE;
            infoB->duplex = TRUE;
            infoA->local_ability.speed_full_duplex =
                infoB->local_ability.speed_full_duplex = SOC_PA_SPEED_10MB;
            infoA->local_ability.speed_half_duplex =
                infoB->local_ability.speed_half_duplex = 0;
        } else if ((infoA->port_ability.speed_half_duplex & SOC_PA_SPEED_10MB) && 
                   (infoB->port_ability.speed_half_duplex & SOC_PA_SPEED_10MB)) {
            infoA->duplex = FALSE;
            infoB->duplex = FALSE;
            infoA->local_ability.speed_full_duplex =
                infoB->local_ability.speed_full_duplex = 0;
            infoA->local_ability.speed_half_duplex =
                infoB->local_ability.speed_half_duplex = SOC_PA_SPEED_10MB;
        } else {
            LOG_ERROR(BSL_LS_APPL_TESTS,
                      (BSL_META_U(unit,
                                  "Unit %d Ports %s-%s: Forcing max speed\n"),
                       unit, SOC_PORT_NAME(unit, portA),
                       SOC_PORT_NAME(unit, portB)));
            speed = SPEED_MAX;
        }
        infoA->speed = infoB->speed = 10;
        break;

    case SPEED_100:
        if ((infoA->port_ability.speed_full_duplex & SOC_PA_SPEED_100MB) && 
            (infoB->port_ability.speed_full_duplex & SOC_PA_SPEED_100MB)) {
            infoA->duplex = TRUE;
            infoB->duplex = TRUE;
            infoA->local_ability.speed_full_duplex =
                infoB->local_ability.speed_full_duplex = SOC_PA_SPEED_100MB;
            infoA->local_ability.speed_half_duplex =
                infoB->local_ability.speed_half_duplex = 0;
        } else if ((infoA->port_ability.speed_half_duplex & SOC_PA_SPEED_100MB) && 
                   (infoB->port_ability.speed_half_duplex & SOC_PA_SPEED_100MB)) {
            infoA->duplex = FALSE;
            infoB->duplex = FALSE;
            infoA->local_ability.speed_full_duplex =
                infoB->local_ability.speed_full_duplex = 0;
            infoA->local_ability.speed_half_duplex =
                infoB->local_ability.speed_half_duplex = SOC_PA_SPEED_100MB;
        } else {
            LOG_ERROR(BSL_LS_APPL_TESTS,
                      (BSL_META_U(unit,
                                  "Unit %d Ports %s-%s: Forcing max speed\n"),
                       unit, SOC_PORT_NAME(unit, portA),
                       SOC_PORT_NAME(unit, portB)));
            speed = SPEED_MAX;
        } 
        infoA->speed = infoB->speed = 100;
        break;

    case SPEED_1GIG:
        if ((infoA->port_ability.speed_full_duplex & SOC_PA_SPEED_1000MB) && 
            (infoB->port_ability.speed_full_duplex & SOC_PA_SPEED_1000MB)) {
            infoA->duplex = TRUE;
            infoB->duplex = TRUE;
            infoA->local_ability.speed_full_duplex =
                infoB->local_ability.speed_full_duplex = SOC_PA_SPEED_1000MB;
            infoA->local_ability.speed_half_duplex =
                infoB->local_ability.speed_half_duplex = 0;
        } else if ((infoA->port_ability.speed_half_duplex & SOC_PA_SPEED_1000MB) && 
                   (infoB->port_ability.speed_half_duplex & SOC_PA_SPEED_1000MB)) {
            infoA->duplex = FALSE;
            infoB->duplex = FALSE;
            infoA->local_ability.speed_full_duplex =
                infoB->local_ability.speed_full_duplex = 0;
            infoA->local_ability.speed_half_duplex =
                infoB->local_ability.speed_half_duplex = SOC_PA_SPEED_1000MB;
        } else {
            LOG_ERROR(BSL_LS_APPL_TESTS,
                      (BSL_META_U(unit,
                                  "Unit %d Ports %s-%s: Forcing max speed\n"),
                       unit, SOC_PORT_NAME(unit, portA),
                       SOC_PORT_NAME(unit, portB)));
            speed = SPEED_MAX;
        }
        infoA->speed = infoB->speed = 1000;
        break;

    case SPEED_2_5GIG:
        if ((infoA->port_ability.speed_full_duplex & SOC_PA_SPEED_2500MB) && 
            (infoB->port_ability.speed_full_duplex & SOC_PA_SPEED_2500MB)) {
            infoA->duplex = TRUE;
            infoB->duplex = TRUE;
            infoA->local_ability.speed_full_duplex =
                infoB->local_ability.speed_full_duplex = SOC_PA_SPEED_2500MB;
            infoA->local_ability.speed_half_duplex =
                infoB->local_ability.speed_half_duplex = 0;
        } else if ((infoA->port_ability.speed_half_duplex & SOC_PA_SPEED_2500MB) && 
                   (infoB->port_ability.speed_half_duplex & SOC_PA_SPEED_2500MB)) {
            infoA->duplex = FALSE;
            infoB->duplex = FALSE;
            infoA->local_ability.speed_full_duplex =
                infoB->local_ability.speed_full_duplex = 0;
            infoA->local_ability.speed_half_duplex =
                infoB->local_ability.speed_half_duplex = SOC_PA_SPEED_2500MB;
        } else {
            LOG_ERROR(BSL_LS_APPL_TESTS,
                      (BSL_META_U(unit,
                                  "Unit %d Ports %s-%s: Forcing max speed\n"),
                       unit, SOC_PORT_NAME(unit, portA),
                       SOC_PORT_NAME(unit, portB)));
            speed = SPEED_MAX;
        } 
        infoA->speed = infoB->speed = 2500;
        break;

    case SPEED_3GIG:
        if ((infoA->port_ability.speed_full_duplex & SOC_PA_SPEED_3000MB) && 
            (infoB->port_ability.speed_full_duplex & SOC_PA_SPEED_3000MB)) {
            infoA->duplex = TRUE;
            infoB->duplex = TRUE;
            infoA->local_ability.speed_full_duplex =
                infoB->local_ability.speed_full_duplex = SOC_PA_SPEED_3000MB;
            infoA->local_ability.speed_half_duplex =
                infoB->local_ability.speed_half_duplex = 0;
        } else if ((infoA->port_ability.speed_half_duplex & SOC_PA_SPEED_3000MB) && 
                   (infoB->port_ability.speed_half_duplex & SOC_PA_SPEED_3000MB)) {
            infoA->duplex = FALSE;
            infoB->duplex = FALSE;
            infoA->local_ability.speed_full_duplex =
                infoB->local_ability.speed_full_duplex = 0;
            infoA->local_ability.speed_half_duplex =
                infoB->local_ability.speed_half_duplex = SOC_PA_SPEED_3000MB;
        } else {
            LOG_ERROR(BSL_LS_APPL_TESTS,
                      (BSL_META_U(unit,
                                  "Unit %d Ports %s-%s: Forcing max speed\n"),
                       unit, SOC_PORT_NAME(unit, portA),
                       SOC_PORT_NAME(unit, portB)));
            speed = SPEED_MAX;
        } 
        infoA->speed = infoB->speed = 3000;
        break;

    case SPEED_10GIG:
        if ((infoA->port_ability.speed_full_duplex & SOC_PA_SPEED_10GB) && 
            (infoB->port_ability.speed_full_duplex & SOC_PA_SPEED_10GB)) {
            infoA->duplex = TRUE;
            infoB->duplex = TRUE;
            infoA->local_ability.speed_full_duplex =
                infoB->local_ability.speed_full_duplex = SOC_PA_SPEED_10GB;
            infoA->local_ability.speed_half_duplex =
                infoB->local_ability.speed_half_duplex = 0;
        } else if ((infoA->port_ability.speed_half_duplex & SOC_PA_SPEED_10GB) && 
                   (infoB->port_ability.speed_half_duplex & SOC_PA_SPEED_10GB)) {
            infoA->duplex = FALSE;
            infoB->duplex = FALSE;
            infoA->local_ability.speed_full_duplex =
                infoB->local_ability.speed_full_duplex = 0;
            infoA->local_ability.speed_half_duplex =
                infoB->local_ability.speed_half_duplex = SOC_PA_SPEED_10GB;
        } else {
            LOG_ERROR(BSL_LS_APPL_TESTS,
                      (BSL_META_U(unit,
                                  "Unit %d Ports %s-%s: Forcing max speed\n"),
                       unit, SOC_PORT_NAME(unit, portA),
                       SOC_PORT_NAME(unit, portB)));
            speed = SPEED_MAX;
        } 
        infoA->speed = infoB->speed = 10000;
        break;

    case SPEED_12GIG:
        if ((infoA->port_ability.speed_full_duplex & SOC_PA_SPEED_12GB) && 
            (infoB->port_ability.speed_full_duplex & SOC_PA_SPEED_12GB)) {
            infoA->duplex = TRUE;
            infoB->duplex = TRUE;
            infoA->local_ability.speed_full_duplex =
                infoB->local_ability.speed_full_duplex = SOC_PA_SPEED_12GB;
            infoA->local_ability.speed_half_duplex =
                infoB->local_ability.speed_half_duplex = 0;
        } else if ((infoA->port_ability.speed_half_duplex & SOC_PA_SPEED_12GB) && 
                   (infoB->port_ability.speed_half_duplex & SOC_PA_SPEED_12GB)) {
            infoA->duplex = FALSE;
            infoB->duplex = FALSE;
            infoA->local_ability.speed_full_duplex =
                infoB->local_ability.speed_full_duplex = 0;
            infoA->local_ability.speed_half_duplex =
                infoB->local_ability.speed_half_duplex = SOC_PA_SPEED_12GB;
        } else {
            LOG_ERROR(BSL_LS_APPL_TESTS,
                      (BSL_META_U(unit,
                                  "Unit %d Ports %s-%s: Forcing max speed\n"),
                       unit, SOC_PORT_NAME(unit, portA),
                       SOC_PORT_NAME(unit, portB)));
            speed = SPEED_MAX;
        } 
        infoA->speed = infoB->speed = 12000;
        break;

    case SPEED_13GIG:
        if ((infoA->port_ability.speed_full_duplex & SOC_PA_SPEED_13GB) && 
            (infoB->port_ability.speed_full_duplex & SOC_PA_SPEED_13GB)) {
            infoA->duplex = TRUE;
            infoB->duplex = TRUE;
            infoA->local_ability.speed_full_duplex =
                infoB->local_ability.speed_full_duplex = SOC_PA_SPEED_13GB;
            infoA->local_ability.speed_half_duplex =
                infoB->local_ability.speed_half_duplex = 0;
        } else if ((infoA->port_ability.speed_half_duplex & SOC_PA_SPEED_13GB) && 
                   (infoB->port_ability.speed_half_duplex & SOC_PA_SPEED_13GB)) {
            infoA->duplex = FALSE;
            infoB->duplex = FALSE;
            infoA->local_ability.speed_full_duplex =
                infoB->local_ability.speed_full_duplex = 0;
            infoA->local_ability.speed_half_duplex =
                infoB->local_ability.speed_half_duplex = SOC_PA_SPEED_13GB;
        } else {
            LOG_ERROR(BSL_LS_APPL_TESTS,
                      (BSL_META_U(unit,
                                  "Unit %d Ports %s-%s: Forcing max speed\n"),
                       unit, SOC_PORT_NAME(unit, portA),
                       SOC_PORT_NAME(unit, portB)));
            speed = SPEED_MAX;
        }
        infoA->speed = infoB->speed = 13000;
        break;
        
    case SPEED_16GIG:
        if ((infoA->port_ability.speed_full_duplex & SOC_PA_SPEED_16GB) && 
            (infoB->port_ability.speed_full_duplex & SOC_PA_SPEED_16GB)) {
            infoA->duplex = TRUE;
            infoB->duplex = TRUE;
            infoA->local_ability.speed_full_duplex =
                infoB->local_ability.speed_full_duplex = SOC_PA_SPEED_16GB;
            infoA->local_ability.speed_half_duplex =
                infoB->local_ability.speed_half_duplex = 0;
        } else if ((infoA->port_ability.speed_half_duplex & SOC_PA_SPEED_16GB) && 
                   (infoB->port_ability.speed_half_duplex & SOC_PA_SPEED_16GB)) {
            infoA->duplex = FALSE;
            infoB->duplex = FALSE;
            infoA->local_ability.speed_full_duplex =
                infoB->local_ability.speed_full_duplex = 0;
            infoA->local_ability.speed_half_duplex =
                infoB->local_ability.speed_half_duplex = SOC_PA_SPEED_16GB;
        } else {
            LOG_ERROR(BSL_LS_APPL_TESTS,
                      (BSL_META_U(unit,
                                  "Unit %d Ports %s-%s: Forcing max speed\n"),
                       unit, SOC_PORT_NAME(unit, portA),
                       SOC_PORT_NAME(unit, portB)));
            speed = SPEED_MAX;
        }
        infoA->speed = infoB->speed = 16000;
        break;

    case SPEED_20GIG:
        if ((infoA->port_ability.speed_full_duplex & SOC_PA_SPEED_20GB) && 
            (infoB->port_ability.speed_full_duplex & SOC_PA_SPEED_20GB)) {
            infoA->duplex = TRUE;
            infoB->duplex = TRUE;
            infoA->local_ability.speed_full_duplex =
                infoB->local_ability.speed_full_duplex = SOC_PA_SPEED_20GB;
            infoA->local_ability.speed_half_duplex =
                infoB->local_ability.speed_half_duplex = 0;
        } else if ((infoA->port_ability.speed_half_duplex & SOC_PA_SPEED_20GB) && 
                   (infoB->port_ability.speed_half_duplex & SOC_PA_SPEED_20GB)) {
            infoA->duplex = FALSE;
            infoB->duplex = FALSE;
            infoA->local_ability.speed_full_duplex =
                infoB->local_ability.speed_full_duplex = 0;
            infoA->local_ability.speed_half_duplex =
                infoB->local_ability.speed_half_duplex = SOC_PA_SPEED_20GB;
        } else {
            LOG_ERROR(BSL_LS_APPL_TESTS,
                      (BSL_META_U(unit,
                                  "Unit %d Ports %s-%s: Forcing max speed\n"),
                       unit, SOC_PORT_NAME(unit, portA),
                       SOC_PORT_NAME(unit, portB)));
            speed = SPEED_MAX;
        } 
        infoA->speed = infoB->speed = 20000;
        break;

    case SPEED_21GIG:
        if ((infoA->port_ability.speed_full_duplex & SOC_PA_SPEED_21GB) && 
            (infoB->port_ability.speed_full_duplex & SOC_PA_SPEED_21GB)) {
            infoA->duplex = TRUE;
            infoB->duplex = TRUE;
            infoA->local_ability.speed_full_duplex =
                infoB->local_ability.speed_full_duplex = SOC_PA_SPEED_21GB;
            infoA->local_ability.speed_half_duplex =
                infoB->local_ability.speed_half_duplex = 0;
        } else if ((infoA->port_ability.speed_half_duplex & SOC_PA_SPEED_21GB) && 
                   (infoB->port_ability.speed_half_duplex & SOC_PA_SPEED_21GB)) {
            infoA->duplex = FALSE;
            infoB->duplex = FALSE;
            infoA->local_ability.speed_full_duplex =
                infoB->local_ability.speed_full_duplex = 0;
            infoA->local_ability.speed_half_duplex =
                infoB->local_ability.speed_half_duplex = SOC_PA_SPEED_21GB;
        } else {
            LOG_ERROR(BSL_LS_APPL_TESTS,
                      (BSL_META_U(unit,
                                  "Unit %d Ports %s-%s: Forcing max speed\n"),
                       unit, SOC_PORT_NAME(unit, portA),
                       SOC_PORT_NAME(unit, portB)));
            speed = SPEED_MAX;
        } 
        infoA->speed = infoB->speed = 21000;
        break;

    case SPEED_24GIG:
        if ((infoA->port_ability.speed_full_duplex & SOC_PA_SPEED_24GB) && 
            (infoB->port_ability.speed_full_duplex & SOC_PA_SPEED_24GB)) {
            infoA->duplex = TRUE;
            infoB->duplex = TRUE;
            infoA->local_ability.speed_full_duplex =
                infoB->local_ability.speed_full_duplex = SOC_PA_SPEED_24GB;
            infoA->local_ability.speed_half_duplex =
                infoB->local_ability.speed_half_duplex = 0;
        } else if ((infoA->port_ability.speed_half_duplex & SOC_PA_SPEED_24GB) && 
                   (infoB->port_ability.speed_half_duplex & SOC_PA_SPEED_24GB)) {
            infoA->duplex = FALSE;
            infoB->duplex = FALSE;
            infoA->local_ability.speed_full_duplex =
                infoB->local_ability.speed_full_duplex = 0;
            infoA->local_ability.speed_half_duplex =
                infoB->local_ability.speed_half_duplex = SOC_PA_SPEED_24GB;
        } else {
            LOG_ERROR(BSL_LS_APPL_TESTS,
                      (BSL_META_U(unit,
                                  "Unit %d Ports %s-%s: Forcing max speed\n"),
                       unit, SOC_PORT_NAME(unit, portA),
                       SOC_PORT_NAME(unit, portB)));
            speed = SPEED_MAX;
        }
        infoA->speed = infoB->speed = 24000;
        break;

    case SPEED_25GIG:
        if ((infoA->port_ability.speed_full_duplex & SOC_PA_SPEED_25GB) && 
            (infoB->port_ability.speed_full_duplex & SOC_PA_SPEED_25GB)) {
            infoA->duplex = TRUE;
            infoB->duplex = TRUE;
            infoA->local_ability.speed_full_duplex =
                infoB->local_ability.speed_full_duplex = SOC_PA_SPEED_25GB;
            infoA->local_ability.speed_half_duplex =
                infoB->local_ability.speed_half_duplex = 0;
        } else if ((infoA->port_ability.speed_half_duplex & SOC_PA_SPEED_25GB) && 
                   (infoB->port_ability.speed_half_duplex & SOC_PA_SPEED_25GB)) {
            infoA->duplex = FALSE;
            infoB->duplex = FALSE;
            infoA->local_ability.speed_full_duplex =
                infoB->local_ability.speed_full_duplex = 0;
            infoA->local_ability.speed_half_duplex =
                infoB->local_ability.speed_half_duplex = SOC_PA_SPEED_25GB;
        } else {
            LOG_ERROR(BSL_LS_APPL_TESTS,
                      (BSL_META_U(unit,
                                  "Unit %d Ports %s-%s: Forcing max speed\n"),
                       unit, SOC_PORT_NAME(unit, portA),
                       SOC_PORT_NAME(unit, portB)));
            speed = SPEED_MAX;
        } 
        infoA->speed = infoB->speed = 25000;
        break;

    case SPEED_30GIG:
        if ((infoA->port_ability.speed_full_duplex & SOC_PA_SPEED_30GB) && 
            (infoB->port_ability.speed_full_duplex & SOC_PA_SPEED_30GB)) {
            infoA->duplex = TRUE;
            infoB->duplex = TRUE;
            infoA->local_ability.speed_full_duplex =
                infoB->local_ability.speed_full_duplex = SOC_PA_SPEED_30GB;
            infoA->local_ability.speed_half_duplex =
                infoB->local_ability.speed_half_duplex = 0;
        } else if ((infoA->port_ability.speed_half_duplex & SOC_PA_SPEED_30GB) && 
                   (infoB->port_ability.speed_half_duplex & SOC_PA_SPEED_30GB)) {
            infoA->duplex = FALSE;
            infoB->duplex = FALSE;
            infoA->local_ability.speed_full_duplex =
                infoB->local_ability.speed_full_duplex = 0;
            infoA->local_ability.speed_half_duplex =
                infoB->local_ability.speed_half_duplex = SOC_PA_SPEED_30GB;
        } else {
            LOG_ERROR(BSL_LS_APPL_TESTS,
                      (BSL_META_U(unit,
                                  "Unit %d Ports %s-%s: Forcing max speed\n"),
                       unit, SOC_PORT_NAME(unit, portA),
                       SOC_PORT_NAME(unit, portB)));
            speed = SPEED_MAX;
        } 
        infoA->speed = infoB->speed = 30000;
        break;

    case SPEED_32GIG:
        if ((infoA->port_ability.speed_full_duplex & SOC_PA_SPEED_32GB) && 
            (infoB->port_ability.speed_full_duplex & SOC_PA_SPEED_32GB)) {
            infoA->duplex = TRUE;
            infoB->duplex = TRUE;
            infoA->local_ability.speed_full_duplex =
                infoB->local_ability.speed_full_duplex = SOC_PA_SPEED_32GB;
            infoA->local_ability.speed_half_duplex =
                infoB->local_ability.speed_half_duplex = 0;
        } else if ((infoA->port_ability.speed_half_duplex & SOC_PA_SPEED_32GB) && 
                   (infoB->port_ability.speed_half_duplex & SOC_PA_SPEED_32GB)) {
            infoA->duplex = FALSE;
            infoB->duplex = FALSE;
            infoA->local_ability.speed_full_duplex =
                infoB->local_ability.speed_full_duplex = 0;
            infoA->local_ability.speed_half_duplex =
                infoB->local_ability.speed_half_duplex = SOC_PA_SPEED_32GB;
        } else {
            LOG_ERROR(BSL_LS_APPL_TESTS,
                      (BSL_META_U(unit,
                                  "Unit %d Ports %s-%s: Forcing max speed\n"),
                       unit, SOC_PORT_NAME(unit, portA),
                       SOC_PORT_NAME(unit, portB)));
            speed = SPEED_MAX;
        } 
        infoA->speed = infoB->speed = 32000;
        break;

    case SPEED_40GIG:
        if ((infoA->port_ability.speed_full_duplex & SOC_PA_SPEED_40GB) && 
            (infoB->port_ability.speed_full_duplex & SOC_PA_SPEED_40GB)) {
            infoA->duplex = TRUE;
            infoB->duplex = TRUE;
            infoA->local_ability.speed_full_duplex =
                infoB->local_ability.speed_full_duplex = SOC_PA_SPEED_40GB;
            infoA->local_ability.speed_half_duplex =
                infoB->local_ability.speed_half_duplex = 0;
        } else if ((infoA->port_ability.speed_half_duplex & SOC_PA_SPEED_40GB) && 
                   (infoB->port_ability.speed_half_duplex & SOC_PA_SPEED_40GB)) {
            infoA->duplex = FALSE;
            infoB->duplex = FALSE;
            infoA->local_ability.speed_full_duplex =
                infoB->local_ability.speed_full_duplex = 0;
            infoA->local_ability.speed_half_duplex =
                infoB->local_ability.speed_half_duplex = SOC_PA_SPEED_40GB;
        } else {
            LOG_ERROR(BSL_LS_APPL_TESTS,
                      (BSL_META_U(unit,
                                  "Unit %d Ports %s-%s: Forcing max speed\n"),
                       unit, SOC_PORT_NAME(unit, portA),
                       SOC_PORT_NAME(unit, portB)));
            speed = SPEED_MAX;
        } 
        infoA->speed = infoB->speed = 40000;
        break;

    case SPEED_42GIG:
        if ((infoA->port_ability.speed_full_duplex & SOC_PA_SPEED_42GB) &&
            (infoB->port_ability.speed_full_duplex & SOC_PA_SPEED_42GB)) {
            infoA->duplex = TRUE;
            infoB->duplex = TRUE;
            infoA->local_ability.speed_full_duplex =
                infoB->local_ability.speed_full_duplex = SOC_PA_SPEED_42GB;
            infoA->local_ability.speed_half_duplex =
                infoB->local_ability.speed_half_duplex = 0;
        } else if ((infoA->port_ability.speed_half_duplex & SOC_PA_SPEED_42GB) &&
                   (infoB->port_ability.speed_half_duplex & SOC_PA_SPEED_42GB)) {
            infoA->duplex = FALSE;
            infoB->duplex = FALSE;
            infoA->local_ability.speed_full_duplex =
                infoB->local_ability.speed_full_duplex = 0;
            infoA->local_ability.speed_half_duplex =
                infoB->local_ability.speed_half_duplex = SOC_PA_SPEED_42GB;
        } else {
            LOG_ERROR(BSL_LS_APPL_TESTS,
                      (BSL_META_U(unit,
                                  "Unit %d Ports %s-%s: Forcing max speed\n"),
                       unit, SOC_PORT_NAME(unit, portA),
                       SOC_PORT_NAME(unit, portB)));             
            speed = SPEED_MAX;
        }
        infoA->speed = infoB->speed = 42000;
        break;

    case SPEED_MAX:
    default:
        speed = SPEED_MAX;
        break;
    }

    if (speed == SPEED_MAX) {
        /* compare and take lesser of
         * bcm_port_speed_max and SOC_INFO(unit).port_speed_max */
        bcm_port_speed_max(unit, portA, &infoA->speed);
        if (infoA->speed > SOC_INFO(unit).port_speed_max[portA]) {
            infoA->speed = SOC_INFO(unit).port_speed_max[portA];
        }
        bcm_port_speed_max(unit, portB, &infoB->speed);
        if (infoB->speed > SOC_INFO(unit).port_speed_max[portB]) {
            infoB->speed = SOC_INFO(unit).port_speed_max[portB];
        }

        /* Pick lesser of two speeds if gigabit attached to fast */
        if (infoA->speed < infoB->speed) {
            cli_out("NOTICE: port %s only capable of speed %d, setting "
                    "link partner to match.\n",
                    SOC_PORT_NAME(unit, portA), infoA->speed);
            infoB->speed = infoA->speed;
        }
        if (infoB->speed < infoA->speed) {
            cli_out("NOTICE: port %s only capable of speed %d, setting "
                    "link partner to match.\n",
                    SOC_PORT_NAME(unit, portB), infoB->speed);
            infoA->speed = infoB->speed;
        }
        infoA->duplex = infoB->duplex = TRUE;
    } 

    /* Setup test mode (internal/external) */
    switch ( mode ) {
    default:
    case TRAFFIC_EXTERNAL_BI:   /* A <-> B */
    case TRAFFIC_EXTERNAL_UP:   /* A -> B  */
    case TRAFFIC_EXTERNAL_DOWN: /* A <- B  */
        infoA->autoneg = infoB->autoneg = dfl_autoneg;
        infoA->enable = infoB->enable = TRUE;
        infoA->stp_state = infoB->stp_state = BCM_STG_STP_FORWARD;
        break;

    case TRAFFIC_INTERNAL:
        infoA->autoneg = infoB->autoneg = FALSE;
        infoA->enable = infoB->enable = TRUE;
        infoA->stp_state = infoB->stp_state = BCM_STG_STP_FORWARD;
        infoA->loopback = infoB->loopback = BCM_PORT_LOOPBACK_MAC;
        break;

    case TRAFFIC_INTERNAL_PHY:
        infoA->autoneg = infoB->autoneg = FALSE;
        infoA->enable = infoB->enable = TRUE;
        infoA->stp_state = infoB->stp_state = BCM_STG_STP_FORWARD;
        infoA->loopback = infoB->loopback = BCM_PORT_LOOPBACK_PHY;
        break;

    }

    /* When ANA is disabled, on Gigabit port: assign A as master */
    if ((!infoA->autoneg) || (!infoB->autoneg)) {
        bcm_port_medium_t medium;
        rv = bcm_port_medium_get(unit, portA, &medium);
        if (rv >= 0) {
            if (medium == BCM_PORT_MEDIUM_COPPER) {
                if (IS_GE_PORT(unit, portA)) {
                    infoA->phy_master = TRUE;
                } else if (IS_GE_PORT(unit, portB)) {
                    infoB->phy_master = TRUE;
                }
            }
        }
    }

    /* If port is FE port, remove phy_master attributes controlled by it */
    if(IS_FE_PORT(unit, portA)) {
       infoA->action_mask &= ~BCM_PORT_ATTR_PHY_MASTER_MASK;
    }
    if(IS_FE_PORT(unit, portB)) {
       infoB->action_mask &= ~BCM_PORT_ATTR_PHY_MASTER_MASK;
    }
    
    /* If the Master mode is not changed, remove phy_master attributes from action_mask */
    ms = 0;
    if ((rv = bcm_port_master_get(unit, portA, &ms)) != BCM_E_NONE) {
        cli_out("traffic setup: port %s: could not get port Master mode: %s\n",
                SOC_PORT_NAME(unit, portA),
                bcm_errmsg(rv));
        return -1;
    }
    if (ms == infoA->phy_master) {
        infoA->action_mask &= ~BCM_PORT_ATTR_PHY_MASTER_MASK;
    }

    ms = 0;
    if ((rv = bcm_port_master_get(unit, portB, &ms)) != BCM_E_NONE) {
        cli_out("traffic setup: port %s: could not get port Master mode: %s\n",
                SOC_PORT_NAME(unit, portB),
                bcm_errmsg(rv));
        return -1;
    }
    if (ms == infoB->phy_master) {
        infoB->action_mask &= ~BCM_PORT_ATTR_PHY_MASTER_MASK;
    }

    /* Commit all changes in one call */
    if (IS_CD_PORT(unit, portA)) {
        bcm_port_resource_t_init(&portResrc);

        /* Retrieve the port resource details for the given port */
        if ((rv=bcm_port_resource_speed_get(unit, portA, &portResrc)) < 0) {
                cli_out("ERROR: traffic setup unit %d, ports %s, %s:"
                        "%s resource speed get failed: %s\n",
                        unit,
                        SOC_PORT_NAME(unit, portA),
                        SOC_PORT_NAME(unit, portB),
                        SOC_PORT_NAME(unit, portA),
                        bcm_errmsg(rv));
                return -1;
        }

        /* Save the new speed to be set */
        portResrc.speed = infoA->speed;

        /* Call the API to set the new port speed */
        rv = bcm_port_resource_speed_set(unit, portA, &portResrc);

    } else {
        /* Call the API to set the new port speed */
        rv = bcm_port_speed_set(unit, portA, infoA->speed);
    }

    if (rv < 0) {
        cli_out("ERROR: traffic setup unit %d, "
                "port %s - speed %d set failed: %s\n",
                unit,
                SOC_PORT_NAME(unit, portA),
                infoA->speed,
                bcm_errmsg(rv));
        return -1;
    }

    if (IS_CD_PORT(unit, portB)) {
        bcm_port_resource_t_init(&portResrc);

        /* Retrieve the port resource details for the given port */
        if ((rv=bcm_port_resource_speed_get(unit, portB, &portResrc)) < 0) {
                cli_out("ERROR: traffic setup unit %d, ports %s, %s:"
                        "%s resource speed get failed: %s\n",
                        unit,
                        SOC_PORT_NAME(unit, portA),
                        SOC_PORT_NAME(unit, portB),
                        SOC_PORT_NAME(unit, portB),
                        bcm_errmsg(rv));
                return -1;
        }

        /* Save the new speed to be set */
        portResrc.speed = infoB->speed;

        /* Call the API to set the new port speed */
        rv = bcm_port_resource_speed_set(unit, portB, &portResrc);

    } else {
        /* Call the API to set the new port speed */
        rv = bcm_port_speed_set(unit, portB, infoB->speed);
    }

    if (rv < 0) {
        cli_out("ERROR: traffic setup unit %d, "
                "port %s - speed %d set failed: %s\n",
                unit,
                SOC_PORT_NAME(unit, portB),
                infoB->speed,
                bcm_errmsg(rv));
        return -1;
    }

    if (((rv = bcm_port_duplex_set(unit, portA, infoA->duplex)) < 0) ||
        ((rv = bcm_port_duplex_set(unit, portB, infoB->duplex)) < 0) ||
        ((rv = bcm_port_loopback_set(unit, portA, infoA->loopback)) < 0) ||
        ((rv = bcm_port_loopback_set(unit, portB, infoB->loopback)) < 0) ||
        ((rv = bcm_port_info_restore(unit, portA, infoA)) < 0)) {
        cli_out("ERROR: traffic setup unit %d, ports %s, %s:"
                " %s info restore failed: %s\n",
                unit,
                SOC_PORT_NAME(unit, portA),
                SOC_PORT_NAME(unit, portB),
                SOC_PORT_NAME(unit, portA),
                bcm_errmsg(rv));
        cli_out("%s: duplex = %d, loopback = %d\n",
                SOC_PORT_NAME(unit, portA), infoA->duplex, infoA->loopback);
        cli_out("%s: duplex = %d, loopback = %d\n",
                SOC_PORT_NAME(unit, portB), infoB->duplex, infoB->loopback);
        return -1;
    }
    if ((rv = bcm_port_info_restore(unit, portB, infoB)) < 0) {
        cli_out("ERROR: traffic setup unit %d, ports %s, %s:"
                " %s info restore failed: %s\n",
                unit,
                SOC_PORT_NAME(unit, portA),
                SOC_PORT_NAME(unit, portB),
                SOC_PORT_NAME(unit, portB),
                bcm_errmsg(rv));
        return -1;
    }

#if defined(INCLUDE_MACSEC)
     if (_traffic_setup_macsec(unit, portA, portB, test) < 0) {
        cli_out("ERROR: MACSEC traffic setup unit %d, ports %s, %s: Failed\n",
                unit,
                SOC_PORT_NAME(unit, portA),
                SOC_PORT_NAME(unit, portB));
        return -1;
     }
#endif /* INCLUDE_MACSEC */

    cli_out("PORT ");


    /* Setup VLAN broadcast domains including only each port-pair */
#ifdef BCM_XGS12_FABRIC_SUPPORT
    if (SOC_IS_XGS12_FABRIC(unit)) {
        int             mod, rv;
        bcm_port_t      xport;

        /* Setup UC: each module id has every port in it (except CPU) */
        PBMP_PORT_ITER(unit, xport) {
            for (mod = 0; mod <= SOC_MODID_MAX(unit); mod++) {
                rv = bcm_stk_ucbitmap_set(unit, xport, mod,
                                          PBMP_PORT_ALL(unit));
                if (rv < 0) {
                    cli_out("ERROR: port %d module %d: "
                            "bcm_stk_ucbitmap_set failed: %s\n",
                            xport, mod, bcm_errmsg(rv));
                    break;
                }
            }
        }

        /* Setup VID=1 to have every port in it (except CPU) */
        BCM_PBMP_CLEAR(port_pbm);
        rv = bcm_vlan_port_add(unit, VLAN_ID_DEFAULT,
                               PBMP_PORT_ALL(unit), port_pbm);
        if (rv < 0) {
            cli_out("ERROR: vlan %d: add all ports failed: %s\n",
                    VLAN_ID_DEFAULT, bcm_errmsg(rv));
        }
    }  else {
#endif /* BCM_XGS12_FABRIC_SUPPORT */

        /* StrataSwitch / Draco */
        config->vidA = TRAFFIC_VID_BASE + portA;
        config->vidB = config->vidA + 1;

        BCM_PBMP_CLEAR(port_pbm);
        BCM_PBMP_PORT_ADD(port_pbm, portA);
        BCM_PBMP_PORT_ADD(port_pbm, portB);

        bcm_vlan_port_remove(unit, VLAN_ID_DEFAULT, port_pbm);

        /*
         * Internal MAC/PHY traffic. One VLAN, port A and B both
         * members. Port based vlan entry is the same for both
         * ports.
         */
        if ((mode == TRAFFIC_INTERNAL) ||
            (mode == TRAFFIC_INTERNAL_PHY)) {

            /* Share VLAN */
            config->vidB = config->vidA;

            if (bcm_vlan_create(unit, config->vidA) < 0) {
                cli_out("ERROR: ports %s, %s vlan %d: cannot create vlan\n",
                        SOC_PORT_NAME(unit, portA),
                        SOC_PORT_NAME(unit, portB),
                        config->vidA);
            }

            bcm_vlan_port_add(unit, config->vidA, port_pbm, port_pbm);

            if (bcm_port_untagged_vlan_set(unit, portA, config->vidA) < 0) {
                cli_out("ERROR: port %s vlan %d: cannot set default vlan\n",
                        SOC_PORT_NAME(unit, portA),
                        config->vidA);
            }
            if (bcm_port_untagged_vlan_set(unit, portB, config->vidA) < 0) {
                cli_out("ERROR: port %s vlan %d: cannot set default vlan\n",
                        SOC_PORT_NAME(unit, portB),
                        config->vidA);
            }
        } else {

            /* External cable traffic, 2 VLANS, port A and B both
             * members, native (port-based VLAN id) is vidA for portA,
             * and vidB for portB, from the CLI, the structure is:
             *
             * vlan create A PBM=portA,portB UBM=portA,portB
             * vlan create B PBM=portA,portB UBM=portA,portB
             * pvlan set portA A
             * pvlan set portB B
 */
            if (bcm_vlan_create(unit, config->vidA) < 0) {
                cli_out("ERROR: ports %s, %s vlan %d: cannot create vlan\n",
                        SOC_PORT_NAME(unit, portA),
                        SOC_PORT_NAME(unit, portB),
                        config->vidA);
            }
            if (bcm_vlan_create(unit, config->vidB) < 0) {
                cli_out("ERROR: ports %s, %s vlan %d: cannot create vlan\n",
                        SOC_PORT_NAME(unit, portA),
                        SOC_PORT_NAME(unit, portB),
                        config->vidB);
            }

            bcm_vlan_port_add(unit, config->vidA, port_pbm, port_pbm);
            bcm_vlan_port_add(unit, config->vidB, port_pbm, port_pbm);

            if (bcm_port_untagged_vlan_set(unit, portA, config->vidA) < 0) {
                cli_out("ERROR: port %s vlan %d: cannot set default vlan\n",
                        SOC_PORT_NAME(unit, portA),
                        config->vidA);
            }

            if (bcm_port_untagged_vlan_set(unit, portB, config->vidB) < 0) {
                cli_out("ERROR: port %s vlan %d: cannot set default vlan\n",
                        SOC_PORT_NAME(unit, portB),
                        config->vidB);
            }

        }
#ifdef BCM_XGS12_FABRIC_SUPPORT
    }
#endif

#ifdef BCM_ESW_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {
        if (IS_ST_PORT(unit, portA) || IS_ST_PORT(unit, portB)) {
            int modid = _traffic_xgs3_test_modid_get(unit, portA);
            if (modid < 0) {
                cli_out("ERROR: port %s: could not get modid\n",
                        SOC_PORT_NAME(unit, portA));
            }
            _traffic_xgs3_modport_set(unit, modid, portA, portB);
            _traffic_xgs3_modport_set(unit, modid, portB, portA);
        }
    }
#endif /* BCM_ESW_SUPPORT */

    cli_out("VLAN ");

    /* Setup ARL, for port to forward traffic of a given
     * MAC address and vlan back into each port
     */
    if (!SOC_IS_XGS12_FABRIC(unit)) {
        /* StrataSwitch / XGS switch */
        if ((mode == TRAFFIC_INTERNAL) ||
            (mode == TRAFFIC_INTERNAL_PHY)) {
            /* DLF storm created */

        } else if (!IS_HG_PORT(unit, portA)) {
            /* External
             *
             * l2 add PBM=portA MAC=00:00:00:00:00:portA V=B ST=T
             * l2 add PBM=portA MAC=00:00:00:00:00:portB V=A ST=T
             */
            traffic_test_setup_l2addr(unit,
                                      &config->l2A, config->vidB, portA);
            traffic_test_setup_l2addr(unit,
                                      &config->l2B, config->vidA, portB);

        }
        cli_out("L2 ");
    }


    /* Force ports into STP forwarding state */
    bcm_port_stp_set(unit, portA, BCM_STG_STP_FORWARD);
    bcm_port_stp_set(unit, portB, BCM_STG_STP_FORWARD);

    /* Clear stats */
    bcm_stat_clear(unit, portA);
    bcm_stat_clear(unit, portB);
    cli_out("STAT\n");

    return 0;
}

/*
 * Print bytes as %d,%2.2fK, %2.2fG, etc.
 */
static void
traffic_show_number(uint64 v)
{
#ifdef COMPILER_HAS_DOUBLE
    COMPILER_DOUBLE value;

    COMPILER_64_TO_DOUBLE(value, v);

    if (value < KILO(1.0)) {
        cli_out("%d", (int)value) ;
    } else if (value >= TERA(1.0)) {
        value /= TERA(1.0);
        cli_out("%2.2fT", value);
    } else if (value >= GIGA(1.0)) {
        value /= GIGA(1.0);
        cli_out("%2.2fG", value);
    } else if (value >= MEGA(1.0)) {
        value /= MEGA(1.0);
        cli_out("%2.2fM", value);
    } else {
        value /= KILO(1.0);
        cli_out("%2.2fK", value);
    }
#else
    uint32 value;
    int prec = 100;
    char *s;

    s = _shr_scale_uint64(v, 1024, prec, &value);
    if (*s) {
        cli_out("%d.%02d%s", value / prec, value % prec, s);
    } else {
        cli_out("%d", value / prec);
    }
#endif
}

static void
traffic_show_measured_number(uint64 v, int interval)
{
#ifdef COMPILER_HAS_DOUBLE
    COMPILER_DOUBLE value;

    COMPILER_64_TO_DOUBLE(value, v);

    value /= interval;

    if (value < KILO(1.0)) {
        cli_out("%d", (int)value) ;
    } else if (value >= TERA(1.0)) {
        value /= TERA(1.0);
        cli_out("%2.2fT", value);
    } else  if (value >= GIGA(1.0)) {
        value /= GIGA(1.0);
        cli_out("%2.2fG", value);
    } else if (value >= MEGA(1.0)) {
        value /= MEGA(1.0);
        cli_out("%2.2fM", value);
    } else {
        value /= KILO(1.0);
        cli_out("%2.2fK", value);
    }
#else
    uint64 value64;
    int t;

    value64 = v;

    /* Shift down to 32 bits */
    t = 0;
    while (COMPILER_64_HI(value64)) {
        COMPILER_64_SHR(value64, 1);
        t++;
    }
    /* Perform 32 bit division with rounding */
    COMPILER_64_SET(value64, 0, _shr_div32r(COMPILER_64_LO(value64), interval));
    /* Shift back - precision is still adequate */
    COMPILER_64_SHL(value64, t);

    traffic_show_number(value64);
#endif
}

/*
 * Print rate as %d,%2.2fG, etc.
 */
static void
traffic_show_measured_rate(uint64 v, int interval, int bytes)
{
#ifdef COMPILER_HAS_DOUBLE
    double value = (COMPILER_64_HI(v) * 4294967296.0 + COMPILER_64_LO(v));
    value /= interval;
    if (bytes) {
        value *=  8;
    }
    if (value < 1000.0) {
        cli_out("%d", (int)value) ;
    } else if (value >= 1000000000.0) {
        value /= 1000000000.0;
        cli_out("%2.2fG", value);
    } else if (value >= 1000000.0) {
        value /= 1000000.0;
        cli_out("%2.2fM", value);
    } else {
        value /= 1000.0;
        cli_out("%2.2fK", value);
    }
#else
    uint64 value;
    uint32 val32;
    int i;
    int prec = 100;
    char *s;

    /* Multiply by 8 if byte count */
    if (bytes) {
        COMPILER_64_SHL(v, 3);
    }
    /* Shift down to 32 bits */
    i = 0;
    while (COMPILER_64_HI(v)) {
        COMPILER_64_SHR(v, 1);
        i++;
    }
    /* Perform 32 bit division with rounding */
    COMPILER_64_SET(value, 0, _shr_div32r(COMPILER_64_LO(v), interval));
    /* Shift back - precision is still adequate */
    COMPILER_64_SHL(value, i);

    s = _shr_scale_uint64(value, 1000, prec, &val32);
    if (*s) {
        cli_out("%d.%02d%s", val32 / prec, val32 % prec, s);
    } else {
        cli_out("%d", val32 / prec);
    }
#endif
}

/*
 * Print time (in seconds) as minutes, hours, etc.
 */
static void
traffic_show_runtime(uint32 value)
{
    if (value >= 12*4*7*60*60*24) {
        value /= 12*4*7*60*60*24;
        cli_out("%d years", value);
    } else if (value >= 4*7*60*60*24 ) {
        value /= 4*7*60*60*24 ;
        cli_out("%d months", value);
    } else if (value >= 7*60*60*24 ) {
        value /= 7*60*60*24 ;
        cli_out("%d weeks", value);
    } else if (value >= 60*60*24) {
        value /= 60*60*24 ;
        cli_out("%d days", value);
    } else if (value >= 60*60) {
        value /= 60*60;
        cli_out("%d hours", value);
    } else if (value >= 60) {
        value /= 60;
        cli_out("%d minutes", value);
    } else {
        cli_out("%d seconds", value) ;
    }
}

/*
 * Run test structure.
 */
int
traffic_run_test(traffic_test_t* test)
{
    uint32      runtime;
    int         port, portA, portB, interval;
    int         waitloop;
    int         rv;
    pbmp_t      pbm, portpbm,failedpbm;
    bcm_port_ability_t portA_ability;
    bcm_port_ability_t portB_ability;
    char        pfmt[SOC_PBMP_FMT_LEN];
    char        bmstr[FORMAT_PBMP_MAX];
    int         unit;
    int         lastportA;
    int         notraffic;
    int         passed = 1;
#if  defined(BCM_ESW_SUPPORT)
    uint32      curr_counter_flags;
    int         curr_counter_interval;
    pbmp_t      curr_counter_pbm;
#endif
#if defined(BCM_ENDURO_SUPPORT)
    uint32      val=0;
#endif
    uint64 rbyt, tbyt, rpkt, tpkt, rerr,raln, rdisc, dropped;
    uint64 drbyt, dtbyt, drpkt, dtpkt, drerr, draln, drdisc, ddropped;

    COMPILER_64_ZERO(rbyt);    COMPILER_REFERENCE(&rbyt);
    COMPILER_64_ZERO(tbyt);    COMPILER_REFERENCE(&tbyt);
    COMPILER_64_ZERO(rpkt);    COMPILER_REFERENCE(&rpkt);
    COMPILER_64_ZERO(tpkt);    COMPILER_REFERENCE(&tpkt);
    COMPILER_64_ZERO(rerr);    COMPILER_REFERENCE(&rerr);
    COMPILER_64_ZERO(raln);    COMPILER_REFERENCE(&raln);
    COMPILER_64_ZERO(rdisc);   COMPILER_REFERENCE(&rdisc);
    COMPILER_64_ZERO(dropped); COMPILER_REFERENCE(&dropped);
    COMPILER_64_ZERO(drbyt);   COMPILER_REFERENCE(&drbyt);
    COMPILER_64_ZERO(dtbyt);   COMPILER_REFERENCE(&dtbyt);
    COMPILER_64_ZERO(drpkt);   COMPILER_REFERENCE(&drpkt);
    COMPILER_64_ZERO(dtpkt);   COMPILER_REFERENCE(&dtpkt);
    COMPILER_64_ZERO(drerr);   COMPILER_REFERENCE(&drerr);
    COMPILER_64_ZERO(draln);   COMPILER_REFERENCE(&draln);
    COMPILER_64_ZERO(drdisc);  COMPILER_REFERENCE(&drdisc);
    COMPILER_64_ZERO(ddropped);COMPILER_REFERENCE(&ddropped);

    unit = test->unit;
    BCM_IF_ERROR_RETURN
        (traffic_test_setup_maxframe(test));

    (void)format_pbmp(unit, bmstr, sizeof(bmstr), test->ports);
    cli_out("TRAFFIC: unit %d ports %s (%s), %d packets, mode %s\n",
            unit,
            bmstr,
            SOC_PBMP_FMT(test->ports, pfmt),
            test->npkts,
            traffic_mode_str[test->mode]);

    /* Disable l2 address aging ... */
    bcm_l2_age_timer_set(unit, 0);

    lastportA = -1;

    PBMP_ITER(test->ports, port) {
        test->port_pair[port].portA = -1;
        if (lastportA < 0) {    /* this will be a port A */
            if (!IS_CD_PORT(unit, port)) {
                if (bcm_port_ability_local_get(unit, port, &portA_ability) < 0) {
                    cli_out("ERROR: traffic setup unit %d, ports %s: "
                            "ability get failed\n",
                            unit, SOC_PORT_NAME(unit, port));
                    return -1;
                }
            }
            lastportA = port;
        } else {                /* this will be a port B */
            if (!IS_CD_PORT(unit, port)) {
                if (bcm_port_ability_local_get(unit, port, &portB_ability) < 0) {
                    cli_out("ERROR: traffic setup unit %d, ports %s: "
                            "ability get failed\n",
                            unit, SOC_PORT_NAME(unit, port));
                    return -1;
                }
                /* coverity[uninit_use:FALSE] */
                if ((!(portA_ability.speed_full_duplex &
                      portB_ability.speed_full_duplex) &&
                    !(portA_ability.speed_half_duplex &
                      portB_ability.speed_half_duplex)) ||
                     (portA_ability.encap != portB_ability.encap)) {
                    BCM_PBMP_PORT_REMOVE(test->ports, lastportA);
                    cli_out("WARNING: no common speed between ports %s, %s:"
                            " skip %s\n",
                            SOC_PORT_NAME(unit, lastportA),
                            SOC_PORT_NAME(unit, port),
                            SOC_PORT_NAME(unit, lastportA));
                    lastportA = port;
                    portA_ability = portB_ability;
                    continue;
                }
            }
#if defined(BCM_GREYHOUND2_SUPPORT)
            if (SOC_IS_GREYHOUND2(unit)) {
                int         speedmaxA, speedmaxB;
                bcm_port_abil_t max_speed_mask;

                /* if testing with sp=maximun(default), make sure both portA and B support the least common maximun speed*/
                if (bcm_port_speed_max(unit, lastportA, &speedmaxA) < 0) {
                    cli_out("Error: Port speed max get fail unit %d, ports %d: "
                            "ability get failed\n",
                            unit, lastportA);
                    return -1;
                }
                if (speedmaxA > SOC_INFO(unit).port_speed_max[lastportA]) {
                    speedmaxA = SOC_INFO(unit).port_speed_max[lastportA];
                }
                if (bcm_port_speed_max(unit, port, &speedmaxB) < 0) {
                    cli_out("Error: Port speed max get fail unit %d, ports %d: "
                            "ability get failed\n",
                            unit, port);
                    return -1;
                }
                if (speedmaxB > SOC_INFO(unit).port_speed_max[port]) {
                    speedmaxB = SOC_INFO(unit).port_speed_max[port];
                }
                if (speedmaxA > speedmaxB) {
                    max_speed_mask = SOC_PA_SPEED(speedmaxB);
                    if (!(portA_ability.speed_full_duplex & max_speed_mask)) {
                        BCM_PBMP_PORT_REMOVE(test->ports, lastportA);
                        cli_out("WARNING: no common max speed between ports %s, %s:"
                                " skip %s\n",
                                SOC_PORT_NAME(unit, lastportA),
                                SOC_PORT_NAME(unit, port),
                                SOC_PORT_NAME(unit, lastportA));
                        lastportA = port;
                        portA_ability = portB_ability;
                        continue;
                    }
                } else if (speedmaxA < speedmaxB) {
                    max_speed_mask = SOC_PA_SPEED(speedmaxA);
                    if (!(portB_ability.speed_full_duplex & max_speed_mask)) {
                        BCM_PBMP_PORT_REMOVE(test->ports, lastportA);
                        cli_out("WARNING: no common max speed between ports %s, %s:"
                                " skip %s\n",
                                SOC_PORT_NAME(unit, lastportA),
                                SOC_PORT_NAME(unit, port),
                                SOC_PORT_NAME(unit, lastportA));
                        lastportA = port;
                        /* coverity[uninit_use:FALSE] */
                        portA_ability = portB_ability;
                        continue;
                    }

                }
            }
#endif /* BCM_GREYHOUND2_SUPPORT */
            test->port_pair[lastportA].portA = lastportA;
            test->port_pair[lastportA].portB = port;
            lastportA = -1;
        }
    }
    if (lastportA != -1) {
        BCM_PBMP_PORT_REMOVE(test->ports, lastportA);
        cli_out("WARNING: odd number of ports, will not use port %s\n",
                SOC_PORT_NAME(unit, lastportA));
    }

    cli_out("TRAFFIC: Port pairing:");
    PBMP_ITER(test->ports, port) {
        if (test->port_pair[port].portA != port) {
            /* skip B ports and unused ports */
            continue;
        }
        cli_out(" %s-%s",
                SOC_PORT_NAME(unit, test->port_pair[port].portA),
                SOC_PORT_NAME(unit, test->port_pair[port].portB));
    }
    cli_out("\n");

    /* First, configure forwarding behaviours */
    PBMP_ITER(test->ports, port) {
        if (test->port_pair[port].portA != port) {      /* skip B ports */
            continue;
        }
        cli_out("TRAFFIC: setting up ports %s, %s: ",
                SOC_PORT_NAME(unit, test->port_pair[port].portA),
                SOC_PORT_NAME(unit, test->port_pair[port].portB));
        traffic_test_setup_port_pair(test, port, test->mode, test->req_speed);
    }

#if defined(INCLUDE_MACSEC)
    if (test->macsec_enabled) {
        _traffic_setup_macsec_poll_task(unit, test);
    }
#endif /* INCLUDE_MACSEC */

    SOC_PBMP_ASSIGN(pbm, test->ports);
    (void)format_pbmp(unit, bmstr, sizeof(bmstr), pbm);
    cli_out("TRAFFIC: waiting for link on ports %s (%s)\n",
            bmstr, SOC_PBMP_FMT(pbm, pfmt));

    rv = bcm_link_wait(unit, &pbm, TRAFFIC_LINK_TIMEOUT);
    if (!BCM_SUCCESS(rv)) {
        SOC_PBMP_ASSIGN(portpbm, test->ports);
        SOC_PBMP_REMOVE(portpbm, pbm);
        (void)format_pbmp(unit, bmstr, sizeof(bmstr), portpbm);
        test_error(unit, "TRAFFIC: failed waiting for link on %s (%s): %s\n",
                   bmstr, SOC_PBMP_FMT(portpbm, pfmt), bcm_errmsg(rv));
        return -1;
    }

    /* Link is up - make sure port characteristics from PHY and MAC are match */
    PBMP_ITER(test->ports, port) {
        bcm_port_update(unit, port, 1);
    }

    /* sleep for half second to allow MAC, serdes and PHY to stabilize.
     * bcm_link_wait may not reflect the internal serdes link status
     */
    sal_usleep(500000);

    /* Wait to for MAC to be stable */ 
    rv = bcm_link_wait(unit, &pbm, TRAFFIC_LINK_TIMEOUT);
    if (!BCM_SUCCESS(rv)) {
        SOC_PBMP_ASSIGN(portpbm, test->ports);
        SOC_PBMP_REMOVE(portpbm, pbm);
        (void)format_pbmp(unit, bmstr, sizeof(bmstr), portpbm);
        test_error(unit, "TRAFFIC: failed waiting for link on %s (%s): %s\n",
                   bmstr, SOC_PBMP_FMT(portpbm, pfmt), bcm_errmsg(rv));
        return -1;
    }

    /*
     * Clear all H/W counters using soc driver,we do it here
     * so that if we are running an external test, we won't see
     * a false-carrier error if it occurs.
     */
#ifdef BCM_ESW_SUPPORT
    if (soc_counter_set32_by_port(unit, test->ports, 0) < 0) {
        cli_out("ERROR: Clear counters failed\n");
        return -1;
    }
#endif    

    /* Finally, inject the frame on each port pair .... */
    PBMP_ITER(test->ports, port) {
        if (test->port_pair[port].portA != port) {      /* skip B ports */
            continue;
        }
        cli_out("TRAFFIC: starting %d byte frames on ports %s, %s\n",
                test->pkt_size,
                SOC_PORT_NAME(unit, test->port_pair[port].portA),
                SOC_PORT_NAME(unit, test->port_pair[port].portB));
        traffic_test_send_frames(test, port);
    }

    /* Loop until user presses control-C, showing status */
    interval = test->poll_interval; /* seconds */
    runtime = 0;

    if (test->runtime <= test->poll_interval) {
        test->runtime = test->poll_interval + 1;
        cli_out("NOTICE: test will poll in %d seconds"
                ", using new test runtime of %d seconds.\n",
                test->poll_interval, test->poll_interval + 1);
    }

    /* Save the current parameters for counter collection task. */
#ifdef BCM_ESW_SUPPORT
        if (soc_counter_status(unit, &curr_counter_flags, &curr_counter_interval,
                           &curr_counter_pbm) < 0) {
            cli_out("ERROR: Failed to get counter collection status\n");
            return -1;
        }
#endif    



    /*
     * Start (or re-start) counter collection with an interval which is two
     * times longer than the polling interval of the test. This prevents
     * the counter collection thread from syncing stats too often
     * giving us predictable results when calling bcm_stat_sync below.
     * Wrap-around should not be an issue since the HW has 64-bit counters.
     */
#ifdef BCM_ESW_SUPPORT
    if (soc_counter_start(unit, curr_counter_flags, 
                          test->poll_interval * 2000000, 
                          test->ports) < 0) {
        cli_out("ERROR: Failed to start counter collection\n");
        return -1;
    }
#endif    

    SOC_PBMP_CLEAR(failedpbm); /*Initialize the pbm*/

    /* Wait a configurable amount of time, then check the stats */
    waitloop = 0;
    while (runtime <= test->runtime) {
        waitloop += 1;

        {
            /* syncing counters before thread goes to sleep */ 
            sal_usleep(interval * SECOND_USEC);
            bcm_stat_sync(unit);

            PBMP_ITER(test->ports, port) {
                 portA =  test->port_pair[port].portA;
                 portB =  test->port_pair[port].portB;
                 if (port != portA) {
                     continue;
                 }
                bcm_stat_get(unit, port, snmpIfInOctets, &rbyt);
                bcm_stat_get(unit, port, snmpIfOutOctets, &tbyt);
                bcm_stat_get(unit, port, snmpDot1dTpPortOutFrames, &tpkt);
                bcm_stat_get(unit, port, snmpDot1dTpPortInFrames, &rpkt);
                bcm_stat_get(unit, port, snmpDot3StatsAlignmentErrors, &raln);
                bcm_stat_get(unit, port, snmpDot3StatsInternalMacReceiveErrors,
                             &rdisc);
                bcm_stat_get(unit, port, snmpIfInDiscards, &dropped);
 
                test->port_stats[port].rbyt = rbyt;
                test->port_stats[port].tbyt = tbyt;
                test->port_stats[port].rpkt = rpkt;
                test->port_stats[port].tpkt = tpkt;
                test->port_stats[port].raln = raln;
                test->port_stats[port].rdisc = rdisc;
                test->port_stats[port].dropped = dropped;
            }
        }

        sal_usleep(interval * SECOND_USEC);

        bcm_stat_sync(unit);

        if (waitloop == 1) {
            cli_out("TRAFFIC: mode %s, %d byte frames, runtime ",
                    traffic_mode_str[test->mode], test->pkt_size);
            traffic_show_runtime(interval);
            cli_out(" of ");
            traffic_show_runtime(test->runtime);
            cli_out("\n");
            cli_out("TRAFFIC: press Ctrl-C to stop test\n");
        } else {
            cli_out("TRAFFIC: running... (%d%% complete)\n",
                    (runtime*100)/test->runtime);
        }
        runtime += interval;

        /* Dump stats */
        PBMP_ITER(test->ports, port) {
            portA =  test->port_pair[port].portA;
            portB =  test->port_pair[port].portB;
            if (port != portA) {
                continue;
            }
            bcm_stat_get(unit, port, snmpIfInOctets, &rbyt);
            bcm_stat_get(unit, port, snmpIfOutOctets, &tbyt);
            bcm_stat_get(unit, port, snmpDot1dTpPortOutFrames, &tpkt);
            bcm_stat_get(unit, port, snmpDot1dTpPortInFrames, &rpkt);
            bcm_stat_get(unit, port, snmpDot3StatsAlignmentErrors, &raln);
            bcm_stat_get(unit, port, snmpDot3StatsInternalMacReceiveErrors,
                         &rdisc);
            bcm_stat_get(unit, port, snmpIfInDiscards, &dropped);

            {
                int speed, duplex;
                uint32 dtpktl, dtbytl, drpktl, drbytl;
                uint32 dralnl, drdiscl, ddroppedl;

                COMPILER_64_DELTA(drbyt, test->port_stats[port].rbyt, rbyt);
                COMPILER_64_DELTA(dtbyt, test->port_stats[port].tbyt, tbyt);
                COMPILER_64_DELTA(drpkt, test->port_stats[port].rpkt, rpkt);
                COMPILER_64_DELTA(dtpkt, test->port_stats[port].tpkt, tpkt);
                COMPILER_64_DELTA(draln, test->port_stats[port].raln, raln);
                COMPILER_64_DELTA(drdisc, test->port_stats[port].rdisc,
                                  rdisc);
                COMPILER_64_DELTA(ddropped, test->port_stats[port].dropped,
                                  dropped);

                COMPILER_64_TO_32_LO(drbytl, drbyt);
                COMPILER_64_TO_32_LO(dtbytl, dtbyt);
                COMPILER_64_TO_32_LO(drpktl, drpkt);
                COMPILER_64_TO_32_LO(dtpktl, dtpkt);
                COMPILER_64_TO_32_LO(dralnl, draln);
                COMPILER_64_TO_32_LO(drdiscl, drdisc);
                COMPILER_64_TO_32_LO(ddroppedl, ddropped);

                /* Ignore RX counters for BCM5670/BCM5675 */
                if (((SOC_IS_XGS_FABRIC(unit)) ||
                     (SOC_IS_XGS3_SWITCH(unit))) &&
                    (IS_HG_PORT(unit, port) || IS_XE_PORT(unit, port)) &&
                    (test->mode == TRAFFIC_INTERNAL)) {
                    drpktl = dtpktl;
                    drbytl = dtbytl;
                }


                bcm_port_speed_get(unit, port, &speed);
                bcm_port_duplex_get(unit, port, &duplex);
                cli_out("Port %s (", SOC_PORT_NAME(unit, port));
                if (speed >= 1000) {
                    if (speed % 1000) {
                        cli_out("%d.%dG %s", speed / 1000,
                                (speed % 1000) / 100,
                                duplex ? "FD" : "HD");
                    } else {
                        cli_out("%dG %s", speed / 1000,
                                duplex ? "FD" : "HD");
                    }
                } else if (speed == 0) {
                    cli_out("-");
                } else {
                    cli_out("%dM %s", speed, duplex ? "FD" : "HD");
                }
                cli_out("): %s\n",
                        dralnl > 0 ? "ALN Error Detected" :
                        drdiscl > 0 ? "Discard Error Detected" :
                        ddroppedl > 0 ? "Dropped Error Detected" :
                        (drpktl == 0 || dtpktl == 0 ||
                        dtbytl == 0 || drbytl == 0) ?
                        "FAIL->NO TRAFFIC" :"PASSED");


                cli_out("\tTX packets ");
                traffic_show_number(tpkt);
                cli_out(" +");
                traffic_show_measured_number(dtpkt, interval);
                cli_out("/s TX Bytes ");
                traffic_show_number(tbyt);
                cli_out("B +");
                traffic_show_measured_number(dtbyt, interval);
                cli_out("B/s [");
                traffic_show_measured_rate(dtbyt, interval, TRUE);
                cli_out("b/s]");
                cli_out("\n\tRX packets ");
                traffic_show_number(rpkt);
                cli_out(" +");
                traffic_show_measured_number(drpkt, interval);
                cli_out("/s RX bytes ");
                traffic_show_number(rbyt);
                cli_out("B +");
                traffic_show_measured_number(drbyt, interval);
                cli_out("B/s [");
                traffic_show_measured_rate(drbyt, interval, TRUE);
                cli_out("b/s]");
                cli_out("\n");

                notraffic = 0;

                /* Zero packet rate increase indicates failure */
                if (drpktl == 0 || dtpktl == 0 || dtbytl == 0 || drbytl == 0) {
                    cli_out("ERROR: traffic stopped on ports %s, %s\n",
                            SOC_PORT_NAME(unit, portA),
                            SOC_PORT_NAME(unit, portB));
                    notraffic = 1;
                }

                if (notraffic || test->showstats) {
                    /* Give us a clue .. */
                    cli_out("PortA (%s) statistics:\n",
                            SOC_PORT_NAME(unit, portA));
                    BCM_PBMP_CLEAR(portpbm);
                    BCM_PBMP_PORT_ADD(portpbm, portA);
                    if (SOC_IS_ESW(unit)) {
#ifdef BCM_ESW_SUPPORT
                        do_show_counters(unit,
                                     INVALIDr,
                                     portpbm,
                                     SHOW_CTR_CHANGED|
                                     SHOW_CTR_SAME|
                                     SHOW_CTR_NZ);
#endif
                    } else {
                    }
                    cli_out("PortB (%s) statistics:\n",
                            SOC_PORT_NAME(unit, portB));
                    BCM_PBMP_CLEAR(portpbm);
                    BCM_PBMP_PORT_ADD(portpbm, portB);
                    if (SOC_IS_ESW(unit)) {
#ifdef BCM_ESW_SUPPORT
                        do_show_counters(unit,
                                     INVALIDr,
                                     portpbm,
                                     SHOW_CTR_CHANGED|
                                     SHOW_CTR_SAME|
                                     SHOW_CTR_NZ);                        
#endif
                    } else {
                    }
                }

                if (notraffic && test->stop_on_fail) {
                    /* Restore original counter collection settings. */
#ifdef BCM_ESW_SUPPORT
                    (void) soc_counter_start(unit, curr_counter_flags,
                                             curr_counter_interval, 
                                             curr_counter_pbm);
#endif    
                    passed = 0;
                    SOC_PBMP_PORT_ADD(failedpbm,port);
                    if (test->run2end == 0) {
                        cli_out("Exiting test..\n");
                        return -1;
                    }
                }
            }
        }
    }

#if defined(BCM_ENDURO_SUPPORT)    
    if (SOC_IS_ENDURO(unit)) {
        /* Check MMU parity error registers */
        SOC_IF_ERROR_RETURN(READ_MEM_FAIL_INT_STATr(unit, &val));
        if (val) {
            cli_out("ERROR: MEM_FAIL_INT_STAT 0x%x\n", val);
            return -1;
        }
        /* coverity[result_independent_of_operands] */
        SOC_IF_ERROR_RETURN(READ_CMIC_CHIP_PARITY_INTR_STATUSr(unit, &val));
        if (val) {
            cli_out("ERROR: CMIC_CHIP_PARITY_INTR_STATUS 0x%x\n", val);
            return -1;
        }
        SOC_IF_ERROR_RETURN(READ_EP_INTR_STATUSr(unit, &val));
        if (val) {
            cli_out("ERROR: EP_INTR_STATUS 0x%x\n", val);
            return -1;
        }
        SOC_IF_ERROR_RETURN(READ_ECC_SINGLE_BIT_ERRORSr(unit, &val));
        if (val) {
            cli_out("ERROR: ECC_SINGLE_BIT_ERRORS 0x%x\n", val);
            return -1;
        }
    }
#endif /* BCM_ENDURO_SUPPORT */

    if (passed) {
        cli_out("TEST passed: ");
        traffic_show_runtime(test->runtime);
        cli_out("\n");
    } else {
        cli_out("TEST failed: \n");
        PBMP_ITER(failedpbm, port) {
            cli_out("Port %s: FAIL->NO TRAFFIC.\n",
            SOC_PORT_NAME(unit, port));
        }
        cli_out("\n");
    }

#if defined(INCLUDE_MACSEC)
    macsec_stop_poll_task = 1;
#endif

    /* Restore original counter collection settings. */

#ifdef BCM_ESW_SUPPORT
    if (soc_counter_start(unit, curr_counter_flags,
                          curr_counter_interval, curr_counter_pbm) < 0) {
        cli_out("ERROR: Failed to restore counter collection\n");
        return -1;
    }
#endif    

    return 0;
}


/*
 * Run Traffic Test structure for the test CLI.
 */
STATIC int
traffic_run_cli(int unit, args_t* args, void* pa)
{
    return traffic_run_test(traf_test[unit]);
}


/*
 * Parse Test Parameters, create test structure
 */
int
traffic_test_init(int unit, args_t *a, void **pa)
{
    parse_table_t       pt;
    int             port = 0;
    int             last_port = 0;
    int             num_ports = 0;

    SOC_PBMP_ASSIGN(dfl_portbitmap, PBMP_PORT_ALL(unit));

    PBMP_ITER(dfl_portbitmap, port) {
        last_port = port;
        num_ports++;
    }

    if (num_ports % 2) {
        BCM_PBMP_PORT_REMOVE(dfl_portbitmap, last_port);
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Pattern",     PQ_HEX|PQ_DFL,  0,
                    &dfl_pattern,NULL);
    parse_table_add(&pt, "PatternIncrement",PQ_HEX|PQ_DFL, 0,
                    &dfl_pattern_inc,NULL);
    parse_table_add(&pt, "TimeInSeconds", PQ_INT|PQ_DFL,0,
                    &dfl_runtime,       NULL);
    parse_table_add(&pt, "CounterPollInterval", PQ_INT|PQ_DFL,0,
                    &dfl_poll_interval, NULL);
    parse_table_add(&pt, "RunMode",     PQ_MULTI|PQ_DFL,0,
                    &dfl_runmode,       traffic_mode_str);
    parse_table_add(&pt, "SPeed",       PQ_MULTI|PQ_DFL,0,
                    &dfl_speed, traffic_speed_str);
    parse_table_add(&pt, "AutoNeg",     PQ_BOOL|PQ_DFL, 0,
                    &dfl_autoneg, NULL);
    parse_table_add(&pt, "Length",      PQ_INT|PQ_DFL,  0,
                    &dfl_size,  NULL);
    parse_table_add(&pt, "Count",      PQ_INT|PQ_DFL,  0,
                    &dfl_count,  NULL);
    parse_table_add(&pt, "PortBitMap",  PQ_PBMP|PQ_DFL, 0,
                    &dfl_portbitmap,    NULL);
    parse_table_add(&pt, "CleanUp",     PQ_BOOL|PQ_DFL, 0,
                    &dfl_cleanup, NULL);
    parse_table_add(&pt, "ShowStats",   PQ_BOOL|PQ_DFL, 0,
                    &dfl_showstats, NULL);
    parse_table_add(&pt, "Run2End",   PQ_BOOL|PQ_DFL, 0,
                    &dfl_run2end, NULL);
#if defined(INCLUDE_MACSEC)
    parse_table_add(&pt, "MacSEC",   PQ_BOOL|PQ_DFL, 0,
                    &dfl_macsec, NULL);
#endif /* INCLUDE_MACSEC */ 

    if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) != 0) {
        test_error(unit,
                   "%s: Invalid option: %s\n",
                   ARG_CMD(a),
                   ARG_CUR(a) ? ARG_CUR(a) : "*");
        parse_arg_eq_done(&pt);
        return -1;
    }
    parse_arg_eq_done(&pt);

    /* Alloc test, save H/W state, setup defaults */
    traf_test[unit] = traffic_test_alloc(unit);

#if defined(BCM_ENDURO_SUPPORT)
    if (SOC_IS_ENDURO(unit)) {
        /* Disable parity interrupt */
        /* coverity[result_independent_of_operands] */
        SOC_IF_ERROR_RETURN(WRITE_CMIC_CHIP_PARITY_INTR_ENABLEr(unit, 0));
    }
#endif /* BCM_ENDURO_SUPPORT */

    return 0;
}

int
traffic_test(int unit, args_t *a, void *pa)
{
    return traffic_run_cli(unit, a, traf_test[unit]);
}

int
traffic_test_done(int unit, void *pa)
{
#if defined(BCM_ENDURO_SUPPORT)
    uint32 val = 0;
#endif
    int rv = 0;

    if (traf_test[unit]->cleanup) {
        /* nothing to do right here */
    } else {
        cli_out("WARNING: CleanupOnExit disabled: "
                "(H/W configuration still active for debug)\n");
        cli_out("*** Zero EPC_LINK to stop traffic.\n");
        cli_out("*** Reboot switch when done.\n");
    }

    rv = traffic_test_free(traf_test[unit]);
    sal_free(traf_test[unit]);

#if defined(BCM_ENDURO_SUPPORT)    
    if (SOC_IS_ENDURO(unit)) {
       /* Check MMU outstanding packets/cells */ 
        SOC_IF_ERROR_RETURN(READ_TOTAL_BUFFER_COUNT_PACKETr(unit, &val));
        if (val) {
	    cli_out("ERROR: TOTAL_BUFFER_COUNT_PACKET 0x%x\n", val);
        }
        SOC_IF_ERROR_RETURN(READ_TOTAL_BUFFER_COUNT_CELLr(unit, &val));
        if (val) {
	    cli_out("ERROR: TOTAL_BUFFER_COUNT_CELL 0x%x\n", val);
        }
        SOC_IF_ERROR_RETURN(READ_CFAPREADPOINTERr(unit, &val));
        if (val != 0x1c) {
	    cli_out("ERROR: CFAPREADPOINTER 0x%x\n", val);
        }
        SOC_IF_ERROR_RETURN(READ_PFAPREADPOINTERr(unit, &val));
        if (val != 0x1c) {
	    cli_out("ERROR: PFAPREADPOINTER 0x%x\n", val);
        }
    }
#endif /* BCM_ENDURO_SUPPORT */

    return rv;
}
