/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * tr60 - Linkscan MDIO
 * Loop reading the MII registers ID0 and ID1(RO registers), verifying
 * they never change
 */

#include <shared/bsl.h>

#include <sal/types.h>
#include <sal/core/boot.h>
#include <sal/appl/sal.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <shared/bsl.h>
#include <soc/types.h>
#include <soc/debug.h>

#include <soc/enet.h>
#include <soc/counter.h>
#include <soc/register.h>
#include <soc/memory.h>
#include <soc/phyctrl.h>
#include <soc/phyreg.h>
#include <soc/phy.h>
#if defined(PORTMOD_SUPPORT)
#include <soc/portmod/portmod.h>
#endif

#include <bcm/error.h>
#include <bcm/port.h>
#include <bcm/vlan.h>
#include <bcm/link.h>
#include <appl/test/loopback.h>

#include "testlist.h"
#include <appl/diag/system.h>
#include <appl/diag/progress.h>

#if defined(BCM_PETRA_SUPPORT)
#include <soc/dpp/port_sw_db.h>
#endif

#ifdef PORTMOD_SUPPORT
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_common.h>
#endif /* PORTMOD_SUPPORT */
#include <soc/esw/portctrl.h>

#if defined (BCM_ESW_SUPPORT) || defined (BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT) || defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)

typedef struct linkscan_work_s{
    int         ls_scan[SOC_MAX_NUM_PORTS];
    uint32      ls_id0[SOC_MAX_NUM_PORTS];
    uint32      ls_id1[SOC_MAX_NUM_PORTS];
    uint32      ls_flags[SOC_MAX_NUM_PORTS]; /* Internal phy? */
    pbmp_t      ls_ports;
    int         ls_reads;               /* # reads to perform */
} linkscan_work_t;

static linkscan_work_t *ls_work[SOC_MAX_NUM_DEVICES];

int
ls_test(int u, args_t *args, void *pa)
{
    linkscan_work_t     *ls = (linkscan_work_t *)pa;
    soc_port_t          p;
    int                 rv, r;
    uint32              id0, id1;

    progress_init(ls->ls_reads, 1, 0);
    /*
     * Loop reading the MII registers ID0 and ID1, verifying they never change
     */

    for (r = 0; r < ls->ls_reads; ) {
        PBMP_ITER(ls->ls_ports, p) {
            rv = bcm_port_phy_get(u, p, ls->ls_flags[p],
                     BCM_PORT_PHY_REG_INDIRECT_ADDR(0, 0, MII_PHY_ID0_REG),
                                  &id0);
            if (rv < 0) {
                test_error(u, "Failed to read port %d Phy-id %d ID0: %s\n",
                           p, PORT_TO_PHY_ADDR(u, p), bcm_errmsg(rv));
            }
            rv = bcm_port_phy_get(u, p, ls->ls_flags[p],
                     BCM_PORT_PHY_REG_INDIRECT_ADDR(0, 0, MII_PHY_ID1_REG),
                                  &id1);
            if (rv < 0) {
                test_error(u, "Failed to read port %d Phy-id %d ID1: %s\n",
                           p, PORT_TO_PHY_ADDR(u, p), bcm_errmsg(rv));
            }
            if ((id0 != ls->ls_id0[p]) || (id1 != ls->ls_id1[p])) {
                test_error(u,
                           "Register Compare error: port %s: "
                           "Expected phy id 0x%04x 0x%04x, "
                           "Read phy id 0x%04x 0x%04x",
                           SOC_PORT_NAME(u, p),
                           ls->ls_id0[p], ls->ls_id1[p], id0, id1);
            }
            r += 2;
            progress_report(2);
        }
    }
    progress_done();
    return(0);
}

int
ls_test_init(int u, args_t *args, void **pa)
{
    soc_port_t          p;
    int                 rv;
    linkscan_work_t     *ls;
    parse_table_t       pt;
    pbmp_t              pbmp;
#if defined(PORTMOD_SUPPORT) 
    phymod_core_access_t core_access_arr[MAX_NUM_CORES];
    int max_buf = MAX_NUM_CORES; 
    int nof_cores, is_most_ext;
#endif

    if (ls_work[u] == NULL) {
        ls_work[u] = sal_alloc(sizeof(linkscan_work_t), "ls_test");
        if (ls_work[u] == NULL) {
            test_error(u, "ERROR: cannot allocate memory\n");
            return -1;
        }
        sal_memset(ls_work[u], 0, sizeof(linkscan_work_t));
    }
    ls = ls_work[u];

    parse_table_init(u, &pt);
    parse_table_add(&pt, "Readcount", PQ_INT, (void *)100000, &ls->ls_reads,
                    NULL);

    if (parse_arg_eq(args, &pt) < 0 || ARG_CNT(args) != 0) {
        test_error(u, "%s: Invalid Option: %s\n", ARG_CMD(args),
                   ARG_CUR(args) ? ARG_CUR(args) : "*");
    }
    parse_arg_eq_done(&pt);

    /*
     * Save old state, and enable H/W link scanning on FE ports.
     */

    BCM_PBMP_CLEAR(pbmp);

    BCM_PBMP_ASSIGN(pbmp, PBMP_FE_ALL(u));      /* Handle Strata 24+2 */

    if (BCM_PBMP_IS_NULL(pbmp)) {
        BCM_PBMP_ASSIGN(pbmp, PBMP_GE_ALL(u));
    }

    if (BCM_PBMP_IS_NULL(pbmp)) {
        BCM_PBMP_ASSIGN(pbmp, PBMP_XE_ALL(u));
    }

#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(u)) { 
        BCM_PBMP_ASSIGN(pbmp, PBMP_PORT_ALL(u));
    } 
#endif

#if defined(BCM_PETRA_SUPPORT)
    if (SOC_IS_ARAD(u)) {
        uint32 flags;
        pbmp_t stat_ifs;

        BCM_PBMP_CLEAR(stat_ifs);
        PBMP_ITER(pbmp, p){
            rv = soc_port_sw_db_flags_get(u, p, &flags);
            if (0 > rv) {
            test_error(u, "soc_port_sw_db_flags_get: port %d: %s\n",
                       p, bcm_errmsg(rv));
            sal_free(ls);
            ls_work[u] = NULL;
            return(-1);
            }
            if(SOC_PORT_IS_STAT_INTERFACE(flags)) {
                BCM_PBMP_PORT_ADD(stat_ifs, p);
            }
        }
        BCM_PBMP_REMOVE(pbmp, stat_ifs);
    }
#endif

#if defined(BCM_DFE_SUPPORT) || defined(BCM_PETRA_SUPPORT)
    if (SOC_IS_DFE(u) || SOC_IS_ARAD(u)) {
        BCM_PBMP_OR(pbmp, PBMP_SFI_ALL(u));
    }
#endif

    if (BCM_PBMP_IS_NULL(pbmp)) {
        test_error(u, "Test not available on %s\n", soc_dev_name(u));
        sal_free(ls);
        ls_work[u] = NULL;
        return(-1);
    }

    if (bsl_check(bslLayerAppl, bslSourceTests, bslSeverityNormal, u)) {
        char    s[FORMAT_PBMP_MAX];
        (void)format_pbmp(u, s, sizeof(s), pbmp);
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(u,
                             "Running on ports: %s\n"), s));
    }

    PBMP_ITER(pbmp, p) {
        if (0 > (rv = bcm_port_linkscan_get(u, p, &ls->ls_scan[p]))) {
            test_error(u, "bcm_port_linkscan_get: port %d: %s\n",
                       p, bcm_errmsg(rv));
            sal_free(ls);
            ls_work[u] = NULL;
            return(-1);
        }
    }

    /* At this point, can restore from it */

    BCM_PBMP_CLEAR(ls->ls_ports);
    *pa = (void *)ls;

    PBMP_ITER(pbmp, p) {
        /* Internal or external phy? */
        if (!SOC_PORT_USE_PORTCTRL(u, p)) {
            ls->ls_flags[p] =
                (PORT_TO_PHY_ADDR(u, p) == PORT_TO_PHY_ADDR_INT(u, p)) ?
                SOC_PHY_INTERNAL : 0;
        } 
#if defined(PORTMOD_SUPPORT)            
        else {
            rv = portmod_port_core_access_get(u, p, 0, max_buf, 
                                    core_access_arr, &nof_cores, &is_most_ext);

            if (rv < 0) {
            test_error(u, "portmod_port_chain_core_access_get port %d: %s\n",
                       p, bcm_errmsg(rv));
            sal_free(ls);
            ls_work[u] = NULL;
            return(-1);
            }
            ls->ls_flags[p] = 
                (is_most_ext == 1) ?
                SOC_PHY_INTERNAL : 0;
        }
#endif
        if (0 > (rv = bcm_port_linkscan_set(u, p, BCM_LINKSCAN_MODE_HW))) {
            test_error(u, "bcm_port_linkscan_set: port %d: %s\n",
                       p, bcm_errmsg(rv));
            sal_free(ls);
            ls_work[u] = NULL;
            return(-1);
        }
        rv = bcm_port_phy_get(u, p, ls->ls_flags[p],
             BCM_PORT_PHY_REG_INDIRECT_ADDR(0, 0, MII_PHY_ID0_REG),
                              &ls->ls_id0[p]);
        if (rv < 0) {
            if (rv == SOC_E_UNAVAIL) {
                /* Skip this port, PHY read unsupported. NULL PHY? */
                /* Restore linkscan state here */
                if (0 > (rv = bcm_port_linkscan_set(u, p, ls->ls_scan[p]))) {
                    test_error(u, "bcm_port_linkscan_set: port %d: %s\n",
                               p, bcm_errmsg(rv));
                    sal_free(ls);
                    ls_work[u] = NULL;
                    return(-1);
                }
                LOG_WARN(BSL_LS_APPL_TESTS,
                         (BSL_META_U(u,
                                     "Port %d skipped due to unsupported PHY reads.\n"), p));
                continue;
            } else {
                test_error(u, "Failed to read port %d Phy-addr %d ID0: %s\n",
                           p, PORT_TO_PHY_ADDR(u, p), bcm_errmsg(rv));
                sal_free(ls);
                ls_work[u] = NULL;
                return(-1);
            }
        }
        rv = bcm_port_phy_get(u, p, ls->ls_flags[p],
             BCM_PORT_PHY_REG_INDIRECT_ADDR(0, 0, MII_PHY_ID1_REG),
                              &ls->ls_id1[p]);
        if (rv < 0) {
            test_error(u, "Failed to read port %d Phy-addr %d ID1: %s\n",
                       p, PORT_TO_PHY_ADDR(u, p), bcm_errmsg(rv));
            sal_free(ls);
            ls_work[u] = NULL;
            return(-1);
        }
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(u,
                             "unit %d port %s has phy id 0x%04x 0x%04x\n"),
                  u, SOC_PORT_NAME(u, p), ls->ls_id0[p], ls->ls_id1[p]));
        BCM_PBMP_PORT_ADD(ls->ls_ports, p);
    }

    if (BCM_PBMP_IS_NULL(ls->ls_ports)) {
        test_error(u, "No valid ports left to test!\n");
        sal_free(ls);
        ls_work[u] = NULL;
        return(-1);
    }

    return(0);
}

int
ls_test_done(int u, void *pa)
{
    soc_port_t          p;
    int                 rv;
    linkscan_work_t     *ls = (linkscan_work_t *)pa;

    if (NULL == ls) {
        return(0);
    }

    PBMP_ITER(ls->ls_ports, p) {
        if (0 > (rv = bcm_port_linkscan_set(u, p, ls->ls_scan[p]))) {
            test_error(u, "bcm_port_linkscan_set: port %d: %s\n",
                       p, bcm_errmsg(rv));
            sal_free(ls);
            ls_work[u] = NULL;
            return(-1);
        }
    }
    sal_free(ls);
    ls_work[u] = NULL;
    return(0);
}
#endif /* BCM_ESW_SUPPORT */
