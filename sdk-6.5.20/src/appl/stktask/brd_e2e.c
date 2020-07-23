/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        brd_e2e.c
 * Purpose:	XGS Board End-to-end flow control programming
 *		Supports boards with up to 4 BCM5695 devices, and
 *              dual BCM56504 board only.
 *		Other XGS1 and XGS2 devices are not end-to-end capable.
 */

#include <soc/drv.h>
#include <soc/higig.h>

#include <bcm/error.h>
#include <bcm/stack.h>
#include <bcm/link.h>
#include <bcm/port.h>
#include <bcm/trunk.h>

#include <appl/cputrans/next_hop.h>
#include <appl/stktask/topo_brd.h>

#define	E2E_CAPABLE(_u)	(SOC_IS_FB_FX_HX(_u))

#ifndef E2E_MAXUNIT
#define	E2E_MAXUNIT	4
#endif

#define	E2E_ETHERTYPE	0x8874
#define	E2E_OPCODE_HOL	0xbc01
#define	E2E_OPCODE_IBP	0xbc02

#ifndef E2E_PKT_DEFRATE
#define	E2E_PKT_DEFRATE		0x100
#endif

#ifndef E2E_CELL_DEFLIMIT
#define	E2E_CELL_DEFLIMIT	73	/* cells for a jumbo + 1 */
#endif

#ifndef E2E_PKT_DEFLIMIT
#define	E2E_PKT_DEFLIMIT	0x7ff	/* no limit */
#endif

#define	E2E_CELL_NOLIMIT	0x1fff

#define	E2E_PKT_NOLIMIT		0x7ff


#ifdef BCM_XGS3_SWITCH_SUPPORT

#ifndef E2E_MAXUNIT_FB
#define E2E_MAXUNIT_FB          2
#endif

#ifndef E2E_FB_PKT_LIMIT
#define E2E_FB_PKT_LIMIT        0x90
#endif

#ifndef E2E_FB_CELL_LIMIT
#define E2E_FB_CELL_LIMIT       0x180
#endif

#ifndef E2E_CELL_DISCARD_LIMIT
#define E2E_CELL_DISCARD_LIMIT  0x300
#endif

#ifndef E2E_PKTCELL_RESET_LIMIT
#define E2E_PKTCELL_RESET_LIMIT 0x1
#endif

#ifndef E2E_MAXTIMER_DEFSEL
#define E2E_MAXTIMER_DEFSEL     0xb
#endif

#ifndef E2E_MINTIMER_DEFSEL
#define E2E_MINTIMER_DEFSEL     0x3
#endif

#endif /* BCM_XGS3_SWITCH)SUPPORT */

STATIC int	_bcm_e2e_pkt_rate = E2E_PKT_DEFRATE;
STATIC int	_bcm_e2e_cell_limit = E2E_CELL_DEFLIMIT;
STATIC int	_bcm_e2e_pkt_limit = E2E_PKT_DEFLIMIT;

int
bcm_board_e2e_config_set(int pkt_rate, int cell_limit, int pkt_limit)
{
    _bcm_e2e_pkt_rate = pkt_rate < 0 ? E2E_PKT_DEFRATE : pkt_rate;
    _bcm_e2e_cell_limit = cell_limit < 0 ? E2E_CELL_DEFLIMIT : cell_limit;
    _bcm_e2e_pkt_limit = pkt_limit < 0 ? E2E_PKT_DEFLIMIT : pkt_limit;
    return BCM_E_NONE;
}

int
bcm_board_e2e_config_get(int *pkt_rate, int *cell_limit, int *pkt_limit)
{
    if (pkt_rate != NULL) {
	*pkt_rate = _bcm_e2e_pkt_rate;
    }
    if (cell_limit != NULL) {
	*cell_limit = _bcm_e2e_cell_limit;
    }
    if (pkt_limit != NULL) {
	*pkt_limit = _bcm_e2e_pkt_limit;
    }
    return BCM_E_NONE;
}

#ifdef BCM_FIREBOLT_SUPPORT
STATIC int
_bcm_board_e2e_fbx_unit(int unit,
                         int lmod,
                         int cos,
                         int vlan,
                         bcm_mac_t mac,
                         int *rmod,
                         int **rpstate)
{
    uint32                val = 0;
    bcm_port_t            port;
    int	                  cell_limit, pkt_limit;
    uint32                val0, val1, val2, val3;
    soc_higig_hdr_t       hg;
    uint32                hgwords[3];
    bcm_trunk_t           tid;
    bcm_trunk_info_t      ta_info;
    int                   member_count;
    bcm_trunk_member_t    member_array[BCM_TRUNK_FABRIC_MAX_PORTCNT];
    bcm_port_t            local_port;
    bcm_trunk_chip_info_t ti;
    int                   idx, trunk_found = 0;

    if (rmod[0] < 0) {
        return BCM_E_PARAM; /* or BCM_E_UNAVAIL ?? */
    }

    BCM_IF_ERROR_RETURN
        (bcm_stk_modport_get(unit, rmod[0], &port));

    sal_memset(&ti, 0, sizeof(bcm_trunk_chip_info_t));
    BCM_IF_ERROR_RETURN(bcm_trunk_chip_info_get(unit, &ti));

    /*
     * Check for fabric trunk membership, and enable 
     * E2E IBP on all trunk members
     */
    for (tid = ti.trunk_fabric_id_min; 
         tid <= ti.trunk_fabric_id_max; tid++) {
         sal_memset(&ta_info, 0, sizeof(bcm_trunk_info_t));
         if (bcm_trunk_get(unit, tid, &ta_info, BCM_TRUNK_FABRIC_MAX_PORTCNT,
                     member_array, &member_count) >= 0) {
             for (idx = 0; idx < member_count; idx++) {
                 BCM_IF_ERROR_RETURN(bcm_port_local_get(unit,
                             member_array[idx].gport, &local_port));
                 if (local_port == port) {
                     trunk_found = 1;
                     break;
                 }
             }
         }

         if (trunk_found) {
             break;
         }
    }

    /*
     * If fabric trunk not found for some reason, populate 
     * the single HG port for IBP programming
     */
    if (!trunk_found) {
        member_count = 1;
        bcm_trunk_member_t_init(&member_array[0]);
        member_array[0].gport = BCM_GPORT_DEVPORT(unit, port);
    }

    SOC_IF_ERROR_RETURN(READ_E2ECONFIGr(unit, &val));

    /*
     * set remote module id
     */
    soc_reg_field_set(unit, E2ECONFIGr, &val,
                      REMOTE_SRCMODIDf, rmod[0]);

    /*
     * Turn on e2e IBP only (no HOL)
     */
    soc_reg_field_set(unit, E2ECONFIGr, &val, E2E_IBP_ENf, 1);
    soc_reg_field_set(unit, E2ECONFIGr, &val, E2E_HOL_ENf, 1);
    soc_reg_field_set(unit, E2ECONFIGr, &val, E2E_MAXTIMER_SELf,
                      E2E_MAXTIMER_DEFSEL);
    soc_reg_field_set(unit, E2ECONFIGr, &val, E2E_MINTIMER_SELf,
                      E2E_MINTIMER_DEFSEL);
    SOC_IF_ERROR_RETURN(WRITE_E2ECONFIGr(unit, val));

    /* define packet higig header */
    sal_memset(&hg, 0, sizeof(hg));
    soc_higig_field_set(unit, &hg, HG_start, SOC_HIGIG_START);
    soc_higig_field_set(unit, &hg, HG_hgi, SOC_HIGIG_HGI);
    soc_higig_field_set(unit, &hg, HG_vlan_id, vlan);
    soc_higig_field_set(unit, &hg, HG_src_mod, lmod);
    soc_higig_field_set(unit, &hg, HG_opcode, SOC_HIGIG_OP_UC);
    soc_higig_field_set(unit, &hg, HG_src_port, port);
    soc_higig_field_set(unit, &hg, HG_dst_port, port);
    soc_higig_field_set(unit, &hg, HG_cos, cos);
    sal_memcpy(hgwords, &hg, sizeof(hgwords));

    /* define packet data header (da, sa, len/type, opcode) */
    val0 = (mac[0] << 24) | (mac[1] << 16) | (mac[2] << 8) | mac[3];
    val1 = (mac[4] << 24) | (mac[5] << 16) | (mac[0] << 8) | mac[1];
    val2 = (mac[2] << 24) | (mac[3] << 16) | (mac[4] << 8) | mac[5];
    val3 = (E2E_ETHERTYPE << 16) | E2E_OPCODE_IBP;

    for (idx = 0; idx < member_count; idx++) {

         BCM_IF_ERROR_RETURN
             (bcm_port_local_get(unit, member_array[idx].gport, &port));
         if (!IS_HG_PORT(unit, port)) {
             continue;
         }

         val = 0;
         SOC_IF_ERROR_RETURN(READ_E2ECONFIGr(unit, &val));
         switch(port) {
         case 24:
           soc_reg_field_set(unit, E2ECONFIGr, &val, XPORT24_SEND_E2E_IBPf, 1);
           break;
    
         case 25:
           soc_reg_field_set(unit, E2ECONFIGr, &val, XPORT25_SEND_E2E_IBPf, 1);
           break;

         case 26:
           soc_reg_field_set(unit, E2ECONFIGr, &val, XPORT26_SEND_E2E_IBPf, 1);
           break;

         case 27:
           soc_reg_field_set(unit, E2ECONFIGr, &val, XPORT27_SEND_E2E_IBPf, 1);
           break;

         default:
           break;
         }
         SOC_IF_ERROR_RETURN(WRITE_E2ECONFIGr(unit, val));

         SOC_IF_ERROR_RETURN(WRITE_XIBP_MH0r(unit, port, hgwords[0]));
         SOC_IF_ERROR_RETURN(WRITE_XIBP_MH1r(unit, port, hgwords[1]));
         SOC_IF_ERROR_RETURN(WRITE_XIBP_MH2r(unit, port, hgwords[2]));

         SOC_IF_ERROR_RETURN(WRITE_XIBP_D0r(unit, port, val0));
         SOC_IF_ERROR_RETURN(WRITE_XIBP_D1r(unit, port, val1));
         SOC_IF_ERROR_RETURN(WRITE_XIBP_D2r(unit, port, val2));
         SOC_IF_ERROR_RETURN(WRITE_XIBP_D3r(unit, port, val3));

         val = 0;
         SOC_IF_ERROR_RETURN(READ_IE2E_CONTROLr(unit, port, &val));
         soc_reg_field_set(unit, IE2E_CONTROLr, &val, IBP_ENABLEf, 1);
         SOC_IF_ERROR_RETURN(WRITE_IE2E_CONTROLr(unit, port, val));

         val = 0;
         SOC_IF_ERROR_RETURN(READ_XPORT_CONFIGr(unit, port, &val));
         soc_reg_field_set(unit, XPORT_CONFIGr, &val, E2E_IBP_ENf, 1);
         SOC_IF_ERROR_RETURN(WRITE_XPORT_CONFIGr(unit, port, val));

         /*
          * Setup pkt and cell limits for ibp
          */
         cell_limit = E2E_FB_CELL_LIMIT;
         pkt_limit = E2E_FB_PKT_LIMIT;

         val = 0;
         soc_reg_field_set(unit, E2EIBPCELLSETLIMITr, &val, CELLSETLIMITf,
                           cell_limit);
         soc_reg_field_set(unit, E2EIBPCELLSETLIMITr, &val, RESETLIMITSELf,
                           E2E_PKTCELL_RESET_LIMIT);
         SOC_IF_ERROR_RETURN(WRITE_E2EIBPCELLSETLIMITr(unit, port, val));

         val = 0;
         soc_reg_field_set(unit, E2EIBPPKTSETLIMITr, &val, PKTSETLIMITf,
                           pkt_limit);
         soc_reg_field_set(unit, E2EIBPPKTSETLIMITr, &val, RESETLIMITSELf,
                           E2E_PKTCELL_RESET_LIMIT);
         SOC_IF_ERROR_RETURN(WRITE_E2EIBPPKTSETLIMITr(unit, port, val));

         SOC_IF_ERROR_RETURN
            (WRITE_E2EIBPDISCARDSETLIMITr(unit, port, 
                                          E2E_CELL_DISCARD_LIMIT));
    }

    /*
     * define packet recognition criteria for IBP
     */
    val = (mac[0] << 8) | mac[1];
    SOC_IF_ERROR_RETURN(WRITE_E2E_IBP_RX_DA_MSr(unit, val));
    val = (mac[2] << 24) | (mac[3] << 16) | (mac[4] << 8) | mac[5];
    SOC_IF_ERROR_RETURN(WRITE_E2E_IBP_RX_DA_LSr(unit, val));
    SOC_IF_ERROR_RETURN(WRITE_E2E_IBP_RX_LENGTH_TYPEr(unit, E2E_ETHERTYPE));
    SOC_IF_ERROR_RETURN(WRITE_E2E_IBP_RX_OPCODEr(unit, E2E_OPCODE_IBP));
       
    return BCM_E_NONE; 
}
#endif /* BCM_FIREBOLT_SUPPORT */

STATIC int
_bcm_board_e2e_unit(int unit,
		    int lmod,
		    int cos,
		    int vlan,
		    bcm_mac_t mac,
		    int *rmod,
		    int **rpstate)
{
   int rv = BCM_E_NONE;

#ifdef BCM_FIREBOLT_SUPPORT
   if (SOC_IS_FIREBOLT(unit)) {
       rv = _bcm_board_e2e_fbx_unit(unit, lmod, cos, vlan, mac,
                                    rmod, rpstate);
   }
#endif

   return rv;
}


int
bcm_board_e2e_set(void)
{
    int         rv = BCM_E_NONE;
    int         unit, neunits, i, cos, vlan;
    int         eunits[E2E_MAXUNIT];
    int         emods[E2E_MAXUNIT];
    int         lmod, nr, j, max_e2e_units;
    int         pstate_arr_size = SOC_MAX_NUM_PORTS * sizeof(int);
    int         *pstate[E2E_MAXUNIT];
    int         rmod[E2E_MAXUNIT-1], *rpstate[E2E_MAXUNIT-1];
    int         pause_tx, pause_rx, link_up;
    bcm_mac_t   mac;
    bcm_port_t  port;

    /* find e2e capable units */
    neunits = 0;
    max_e2e_units = E2E_MAXUNIT;

#ifdef BCM_XGS3_SWITCH_SUPPORT
    for (unit = 0; unit < soc_ndev; unit++) {
         if (SOC_IS_FB_FX_HX(SOC_NDEV_IDX2DEV(unit))) {
             neunits++;
             if (max_e2e_units != E2E_MAXUNIT_FB) {
                 max_e2e_units = E2E_MAXUNIT_FB;
             }
         }
    }

    if (max_e2e_units == E2E_MAXUNIT_FB) {
        /*
         * We've established there is a FB in the system;
         * Allow E2E in a stacked system with only 2 FBs
         */
        if (neunits != max_e2e_units) {
            return BCM_E_UNAVAIL;
        }
    }
#endif

    neunits = 0;
    for (unit = 0; unit < soc_ndev; unit++) {
        if (E2E_CAPABLE(SOC_NDEV_IDX2DEV(unit))) {
            eunits[neunits] = SOC_NDEV_IDX2DEV(unit);
            BCM_IF_ERROR_RETURN(bcm_stk_modid_get(SOC_NDEV_IDX2DEV(unit), &emods[neunits]));
            neunits += 1;
            if (neunits == max_e2e_units) {
                break;
            }
        }
    }

    if (neunits < 2) {  /* need at least two devices */
        return BCM_E_NONE;
    }

    /* use nexthop cos, vlan, mac for E2E packets */
    if (next_hop_cos_get(&cos) < 0) {
        cos = 0;
    }

    if (next_hop_vlan_get(&vlan) < 0) {
        vlan = 1;
    }
    (void)nh_tx_local_mac_get(&mac);

    /* get link/pause state */
    for (i = 0; i < E2E_MAXUNIT; i++) {
        pstate[i] = (int *) sal_alloc(pstate_arr_size, "port_states");
        if (NULL == pstate[i]) {
            /* Deallocate all the previous allocations */
            for (j = i-1; j >= 0; j--) {
                sal_free(pstate[j]);
            }
            return BCM_E_MEMORY;
        }
        sal_memset(pstate[i], 0, pstate_arr_size);
    }

    for (i = 0; i < neunits; i++) {
        unit = eunits[i];
        PBMP_E_ITER(unit, port) {
            if (bcm_port_pause_get(unit, port, &pause_tx, &pause_rx) < 0) {
                continue;
            }
            if (!pause_tx) {
                continue;
            }
            if (bcm_port_link_status_get(unit, port, &link_up) < 0) {
                continue;
            }
            if (!link_up) {
                continue;
            }
            pstate[i][port] = 1;
        }
    }

    /* setup each unit for e2e */
    for (i = 0; i < neunits; i++) {
        unit = eunits[i];
        lmod = emods[i];
        rmod[0] = rmod[1] = rmod[2] = -1;
        nr = 0;
        for (j = 0; j < neunits; j++) {
            if (j == i) {
                continue;
            }
            rmod[nr] = emods[j];
            rpstate[nr] = &pstate[j][0];
            nr += 1;
        }
        rv = _bcm_board_e2e_unit(unit, lmod, cos, vlan, mac, rmod, rpstate);
        if (BCM_FAILURE(rv)) {
            break;
        }
    }

    /* Memory cleanup */
    for (i = 0; i < E2E_MAXUNIT; i++) {
        sal_free(pstate[i]);
    }

    return rv;
}

/*
 * E2E IBP programming for dual FB LMs
 */
int
_bcm_board_dual_xgs3_e2e_set(int unit0, bcm_module_t modid0,
                             int unit1, bcm_module_t modid1)
{
    int         rv = BCM_E_NONE;
#ifdef BCM_FIREBOLT_SUPPORT
    int         unit, i, cos, vlan;
    int         lmod, nr, j;
    int         *pstate[E2E_MAXUNIT_FB];
    int         pstate_arr_size = SOC_MAX_NUM_PORTS * sizeof(int);
    int         rmod[E2E_MAXUNIT_FB], *rpstate[E2E_MAXUNIT_FB];
    int         pause_tx, pause_rx, link_up;
    bcm_mac_t   mac;
    bcm_port_t  port;
    xgs3devinfo_t dev[E2E_MAXUNIT_FB];

    dev[0].unit = unit0;
    dev[0].modid = modid0;
    dev[1].unit = unit1;
    dev[1].modid = modid1;

    /* use nexthop cos, vlan, mac for E2E packets */
    if (next_hop_cos_get(&cos) < 0) {
        cos = 0;
    }

    if (next_hop_vlan_get(&vlan) < 0) {
        vlan = 1;
    }
    (void)nh_tx_local_mac_get(&mac);

    /* get link/pause state */
    for (i = 0; i < E2E_MAXUNIT_FB; i++) {
         unit = dev[i].unit;
         pstate[i] = (int *) sal_alloc(pstate_arr_size, "port_states");
         if (NULL == pstate[i]) {
             /* Deallocate all the previous allocations */
             for (j = i-1; j >= 0; j--) {
                 sal_free(pstate[j]);
             }
             return BCM_E_MEMORY;
         }
         sal_memset(pstate[i], 0, pstate_arr_size);

         PBMP_E_ITER(unit, port) {
            if (bcm_port_pause_get(unit, port, &pause_tx, &pause_rx) < 0) {
                continue;
            }
            if (!pause_tx) {
                continue;
            }
            if (bcm_port_link_status_get(unit, port, &link_up) < 0) {
                continue;
            }
            if (!link_up) {
                continue;
            }
            pstate[i][port] = 1;
         }
    }

    /* setup each unit for e2e */
    for (i = 0; i < E2E_MAXUNIT_FB; i++) {
        unit = dev[i].unit;
        lmod = dev[i].modid;
        rmod[0] = rmod[1] = -1;
        nr = 0;
        for (j = 0; j < E2E_MAXUNIT_FB; j++) {
            if (j == i) {
                continue;
            }
            rmod[nr] = dev[j].modid;
            rpstate[nr] = &pstate[j][0];
            nr += 1;
        }

        rv = _bcm_board_e2e_fbx_unit(unit, lmod, cos, vlan, mac, rmod, rpstate);
        if (BCM_FAILURE(rv)) {
            break;
        }
    }

    /* Memory cleanup */
    for (i = 0; i < E2E_MAXUNIT_FB; i++) {
        sal_free(pstate[i]);
    }
#endif /* BCM_FIREBOLT_SUPPORT */

    return rv;
}


