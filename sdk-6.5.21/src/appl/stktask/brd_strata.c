/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        brd_strata.c
 * Purpose:	SL Stacked Board programming
 *		BCM95645K24 (Strata II)
 *		BCM95665P48 (Tucana)
 */

#include <shared/bsl.h>

#include <bcm/error.h>
#include <bcm/stack.h>
#include <bcm/topo.h>
#include <bcm/l2.h>

#include <appl/cputrans/atp.h>
#include <appl/stktask/topology.h>
#include <appl/stktask/topo_brd.h>

#include "topo_int.h"

/*
 * Find output stack port on src_unit to get to a remote modid
 * (duplicate of static function in brd_xgs.c)
 */
STATIC int
_bcm_board_sl_topomap(int src_unit, int dest_modid, bcm_port_t *exit_port)
{
    cpudb_unit_port_t *stk_port;
    topo_cpu_t *tp_cpu;
    int sp_idx, m_idx;
    int mod_id;

    /* Remote module ID; look up in CPU database */
    bcm_board_topo_get(&tp_cpu);
    if (tp_cpu == NULL) {
        return BCM_E_INIT;
    }

    /* Search topo info for the stack port with the given mod id */
    TOPO_FOREACH_STK_PORT(stk_port, tp_cpu, sp_idx) {
        TOPO_FOREACH_TX_MODID(mod_id, tp_cpu, sp_idx, m_idx) {
            if (dest_modid == mod_id && stk_port->unit == src_unit) {
                *exit_port = stk_port->port;
                return BCM_E_NONE;
            }
        }
    }

    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("SL topo map failed unit %d to mod id %d\n"),
                 src_unit, dest_modid));

    return BCM_E_NOT_FOUND;
}

/*
 * Program modport register on a unit to define
 * module routing out each stack port.  Only works on
 * xgs switch devices (not xgs fabrics, not strata switches)
 */
STATIC int
_bcm_board_sl_modport(int unit, topo_cpu_t *tp_cpu)
{
    int			i, m, punit, rv;
    topo_stk_port_t	*tsp;
    cpudb_entry_t	*l_entry;
    bcm_port_t		port;
    bcm_module_t	mod;

    l_entry = &tp_cpu->local_entry;

    /* set everything to the default */
    rv = bcm_stk_modport_clear_all(unit);
    if (rv < 0) {
        LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                    (BSL_META_U(unit,
                    "SL modport %d: modport failed: %s\n"),
                     unit, bcm_errmsg(rv)));
	if (rv == BCM_E_UNAVAIL) {
	    rv = BCM_E_NONE;
	}
	return rv;
    }

    LOG_INFO(BSL_LS_TKS_TOPOLOGY,
             (BSL_META_U(unit,
                         "SL modport unit %d\n"),
              unit));

    /* set up module routing for each stack port port */
    for (i = 0; i < l_entry->base.num_stk_ports; i++) {
        punit = l_entry->base.stk_ports[i].unit;
        port = l_entry->base.stk_ports[i].port;
	if (punit != unit) {
	    continue;
	}

        tsp = &tp_cpu->tp_stk_port[i];
        LOG_INFO(BSL_LS_TKS_TOPOLOGY,
                 (BSL_META_U(unit,
                             "SL modport map unit %d port %d mods %d\n"),
                  unit, port, tsp->tx_mod_num));
        for (m = 0; m < tsp->tx_mod_num; m++) {
            mod = tsp->tx_mods[m];
	    rv = bcm_stk_modport_set(unit, mod, port);
	    if (rv < 0) {
		if (rv == BCM_E_UNAVAIL) {
		    rv = BCM_E_NONE;
		}
		return rv;
	    }
        }
    }
    return BCM_E_NONE;
}

/*
 * Cut broadcast loops by using the cut ports discovered
 * by the topology code.
 */
STATIC int
_bcm_board_sl_loop(int unit, topo_cpu_t *tp_cpu)
{
    int			i, punit;
    bcm_port_t		port, eport;
    cpudb_entry_t	*l_entry;
    bcm_port_config_t	config;
    uint32		flood, block_all;
    bcm_pbmp_t		cutports;

    l_entry = &tp_cpu->local_entry;

    /* find all cutports */
    BCM_PBMP_CLEAR(cutports);
    for (i=0; i < l_entry->base.num_stk_ports; i++) {
	punit = l_entry->base.stk_ports[i].unit;
	port = l_entry->base.stk_ports[i].port;

	if (unit != punit) {
	    continue;
	}

	if (!(l_entry->sp_info[i].flags &
	      (CPUDB_SPF_TX_RESOLVED | CPUDB_SPF_RX_RESOLVED))) {
	    BCM_PBMP_PORT_ADD(cutports, port);
	} else if (l_entry->sp_info[i].flags & CPUDB_SPF_CUT_PORT) {
            LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                        (BSL_META_U(unit,
                        "BRD: Port %d.%d is a cut port\n"),
                         punit, port));
	    BCM_PBMP_PORT_ADD(cutports, port);
	}
    }

    BCM_IF_ERROR_RETURN(bcm_port_config_get(unit, &config));

    block_all = BCM_PORT_FLOOD_BLOCK_BCAST |
	BCM_PORT_FLOOD_BLOCK_UNKNOWN_UCAST |
	BCM_PORT_FLOOD_BLOCK_UNKNOWN_MCAST;

    /*
     * Block all flooding from any cut port.
     * Block all flooding to any cut port.
     * Permit all other flooding, but do not update
     * the CMIC port related blocking entries.
     */
    BCM_PBMP_ITER(config.port, port) {
	BCM_PBMP_ITER(config.port, eport) {
	    if (BCM_PBMP_MEMBER(cutports, port)) {
		flood = block_all;
	    } else if (BCM_PBMP_MEMBER(cutports, eport)) {
		flood = block_all;
	    } else {
		flood = 0;
	    }
	    BCM_IF_ERROR_RETURN
		(bcm_port_flood_block_set(unit, port, eport, flood));
	}
    }

    return BCM_E_NONE;
}


static int		_bcm_sl_l2mac_count;
static bcm_mac_t	_bcm_sl_l2mac[CPUDB_CPU_MAX];

/* Find the local stack port to reach given entry */
STATIC int
_find_stk_port(topo_cpu_t *tp_cpu,
               cpudb_entry_t *local_entry,
               cpudb_entry_t *entry,
               int *unit, bcm_port_t *port)
{
    int i, j;

    for (i = 0; i < local_entry->base.num_stk_ports; i++) {
        for (j = 0; j < tp_cpu->tp_stk_port[i].tx_mod_num; j++) {
            if (tp_cpu->tp_stk_port[i].tx_mods[j] == entry->dest_mod) {
                *unit = local_entry->base.stk_ports[i].unit;
                *port = local_entry->base.stk_ports[i].port;
                return 0;
            }
        }
    }

    return -1;
}

/*
 * Add L2 addresses for all systems in stack.
 */
STATIC int
_bcm_board_sl_l2(int unit, topo_cpu_t *tp_cpu, cpudb_ref_t db_ref,
                 bcm_port_t dflt_exit_port, int is_strata)
{
    cpudb_entry_t	*entry;
    int			atp_cos, atp_vlan, i, rv;
    bcm_l2_addr_t	l2addr;
    int                 l2_unit;
    int                 use_dflt_port = FALSE;

    atp_cos_vlan_get(&atp_cos, &atp_vlan);

    /* delete any old entries for cpus that have left the stack */
    for (i = 0; i < _bcm_sl_l2mac_count; i++) {
	entry = cpudb_mac_lookup(db_ref, _bcm_sl_l2mac[i]);
	if (entry != NULL) {
	    continue;
	}
	rv = bcm_l2_addr_delete(unit, _bcm_sl_l2mac[i], atp_vlan);
	if (rv < 0 && rv != BCM_E_NOT_FOUND) {
            LOG_WARN(BSL_LS_TKS_TOPOLOGY,
                     (BSL_META_U(unit,
                     "BRD: Could not l2 delete mac %x:%x:%x\n"),
                      _bcm_sl_l2mac[i][3],
                      _bcm_sl_l2mac[i][4],
                      _bcm_sl_l2mac[i][5]));
	    return rv;
	}
    }
    _bcm_sl_l2mac_count = 0;

    CPUDB_FOREACH_ENTRY(db_ref, entry) {
	bcm_l2_addr_t_init(&l2addr, entry->base.mac, atp_vlan);
	l2addr.modid = entry->dest_mod;
        l2addr.port = entry->base.dest_port;

        if ((entry != db_ref->local_entry) && is_strata) {
            /* Not local for strata -- use stack port */
            if (_find_stk_port(tp_cpu, db_ref->local_entry, entry,
                               &l2_unit, &l2addr.port) < 0) {
                use_dflt_port = TRUE;
            } else if (l2_unit != unit) { /* Not on this unit */
                use_dflt_port = TRUE;
            }
        }

        if (use_dflt_port) {
            if (dflt_exit_port < 0) {
                LOG_WARN(BSL_LS_TKS_TOPOLOGY,
                         (BSL_META_U(unit,
                                     "BRD: CPU L2 map; not local but no dflt port"
                                     " for unit %d and key " CPUDB_KEY_FMT_EOLN),
                          unit, CPUDB_KEY_DISP(entry->base.key)));
                return BCM_E_FAIL;
            } else {
                l2addr.port = dflt_exit_port;
            }
        }

	l2addr.flags |= BCM_L2_STATIC;
        LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                    (BSL_META_U(unit,
                    "BRD: Adding L2 on %d for " CPUDB_KEY_FMT
                     " to mod %d port %d\n"),
                     unit, CPUDB_KEY_DISP(entry->base.key),
                     l2addr.modid, l2addr.port));
	BCM_IF_ERROR_RETURN(bcm_l2_addr_add(unit, &l2addr));
	sal_memcpy(&_bcm_sl_l2mac[_bcm_sl_l2mac_count++], entry->base.mac,
		   sizeof(bcm_mac_t));
    }
    return BCM_E_NONE;
}

/*
 * Set up PFM to be BCM_PORT_MCAST_FLOOD_UNKNOWN
  */
STATIC int
_bcm_board_sl_mcast_setup(int unit)
{
    bcm_port_t          port;
    bcm_port_config_t config;
    
    BCM_IF_ERROR_RETURN(bcm_port_config_get(unit, &config));
    BCM_PBMP_ITER(config.port, port) {
        BCM_IF_ERROR_RETURN
            (bcm_port_pfm_set(unit, port, BCM_PORT_MCAST_FLOOD_UNKNOWN));
    }
    return BCM_E_NONE;
}


/*
 * Find the single stack port on a device.  If not present, or if
 * more than one, return -1.
 */

STATIC bcm_port_t
_dflt_exit_port_get(cpudb_entry_t *entry, int unit)
{
    bcm_port_t rv = -1;
    int i;

    for (i = 0; i < entry->base.num_stk_ports; i++) {
        if (entry->base.stk_ports[i].unit == unit) {
            if (rv != -1) {
                return -1;
            }
            rv = entry->base.stk_ports[i].port;
        }
    }

    return rv;
}


STATIC int
_bcm_board_sl_common(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref,
                     int is_strata)
{
    cpudb_entry_t	*l_entry;
    int			unit, mod;

    bcm_topo_map_set(_bcm_board_sl_topomap);

    l_entry = &tp_cpu->local_entry;

    for (unit = 0; unit < l_entry->base.num_units; unit++) {
        bcm_port_t dflt_exit_port;

        dflt_exit_port = _dflt_exit_port_get(l_entry, unit);
	mod = tp_cpu->local_entry.mod_ids[unit];
	BCM_IF_ERROR_RETURN(bcm_stk_my_modid_set(unit, mod));
	BCM_IF_ERROR_RETURN(_bcm_board_sl_modport(unit, tp_cpu));
	BCM_IF_ERROR_RETURN(_bcm_board_sl_loop(unit, tp_cpu));
	BCM_IF_ERROR_RETURN(_bcm_board_sl_l2(unit, tp_cpu, db_ref,
                                             dflt_exit_port, is_strata));
        BCM_IF_ERROR_RETURN(_bcm_board_sl_mcast_setup(unit));
    }
    return BCM_E_NONE;
}


/*
 * BCM95645K24 board
 *	1 x 5645 (stackable using SL mode)
 */
int
bcm_board_topo_24f2g_stk(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("TOPO: SL 5645 24FE+2GE board topology handler\n")));
    return _bcm_board_sl_common(tp_cpu, db_ref, TRUE);
}

/*
 * Tucana board (BCM95665P48)
 *	1 x 5665 (stackable using SL mode)
 */

int
bcm_board_topo_48f4g_stk(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("TOPO: SL 5665 48FE+4GE board topology handler\n")));
    return _bcm_board_sl_common(tp_cpu, db_ref, FALSE);
}

/*
 * Back to back 5645 board attached through port 25.  SL stacked.
 */

int
bcm_board_topo_48f2g_stk(topo_cpu_t *tp_cpu, cpudb_ref_t db_ref)
{
    uint32 flags;
    int unit;

    LOG_VERBOSE(BSL_LS_TKS_TOPOLOGY,
                (BSL_META("TOPO: SL b2b 5645 48FE+2GE board topology handler\n")));

    /* Add the internal stack ports */
    for (unit = 0; unit < 2; unit++) {
        BCM_IF_ERROR_RETURN(bcm_stk_port_get(unit, 25, &flags));
        if (!(flags & BCM_STK_ENABLE)) { /* Not yet enabled */
            BCM_IF_ERROR_RETURN(bcm_stk_port_set(unit, 25,
                                                 BCM_BOARD_INT_STK_FLAGS));
        }
    }

    return _bcm_board_sl_common(tp_cpu, db_ref, TRUE);
}
