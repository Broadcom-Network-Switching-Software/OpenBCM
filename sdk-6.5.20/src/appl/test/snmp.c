/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SNMP Statistics Retrieval test.
 * 
 * For every port, and every speed possible on that port, can
 * we retrieve SNMP statistics without an error? This test will
 * confirm that this is the case.
 */

#include <stdarg.h>
#include <sal/types.h>

#include <soc/drv.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <soc/debug.h>
#include <sal/appl/pci.h>
#include <bcm/error.h>
#include <bcm/link.h>
#include <bcm/stat.h>
#include <appl/diag/test.h>
#include <shared/bsl.h>
#include "testlist.h"

#if defined (BCM_ESW_SUPPORT) || defined (BCM_PETRA_SUPPORT)

#undef min
#define min(x, y) ((x) <= (y) ? (x) : (y))

typedef struct snmp_test_s {

    int unit;
    int speed;
    int failures;
    int not_impl;
    int linkscan_enable;
    bcm_port_info_t snmp_save_port[SOC_MAX_NUM_PORTS];
    bcm_pbmp_t snmp_multi_set_pbmp;
} snmp_test_t;

int
snmp_test_done(int u, void *pa)
{
    int                 port, rv;
    snmp_test_t         *b = (snmp_test_t *) pa;

    PBMP_PORT_ITER(b->unit, port) {
        if (BCM_PBMP_MEMBER(b->snmp_multi_set_pbmp, port)) {
            /* for multi_set ports, use speed=0 to recover the default configurations */
        } else {
            if ((rv = bcm_port_info_set(b->unit, port,
                                        &b->snmp_save_port[port])) < 0) {
                test_error(b->unit,
                           "Port %d: Could not set port info: %s\n",
                           port + 1, bcm_errmsg(rv));
                return -1;
            }
        }
    }

    sal_free(b);

    return 0;
}

int
snmp_test_init(int u, args_t *a, void **pa)
{
    snmp_test_t         *b = 0;
    int                 port, rv = -1;

    if ((b = sal_alloc(sizeof (snmp_test_t), "snmp")) == 0) {
        goto done;
    }

    sal_memset(b, 0, sizeof (*b));
    b->unit = u;

    *pa = (void *) b;

    /* Save port states */
    PBMP_PORT_ITER(b->unit, port) {
        if ((rv = bcm_port_info_get(b->unit, port,
                                    &b->snmp_save_port[port])) < 0) {
            test_error(b->unit,
                       "Port %d: Could not get port info: %s\n",
                       port + 1, bcm_errmsg(rv));
            return -1;
        }
    }
    
    rv = 0;

 done:
    if (rv < 0) {
        snmp_test_done(u, *pa);
    }

    return rv;
}

static int
snmp_test_begin(snmp_test_t *b,
                int speed)
{
    uint64              v64;
    int                 port, rv, speedToUse = 0;
    bcm_stat_val_t      object;
    bcm_port_ability_t  ability;
    int                 nport = 0, p = 0;
    int                 alloc_sz;
    bcm_port_resource_t *resource = NULL;

    b->failures = 0;
    b->not_impl = 0;
    BCM_PBMP_CLEAR(b->snmp_multi_set_pbmp);

    if (SOC_IS_TOMAHAWK2(b->unit)) {
        BCM_PBMP_ASSIGN(b->snmp_multi_set_pbmp, PBMP_PORT_ALL(b->unit));
        BCM_PBMP_REMOVE(b->snmp_multi_set_pbmp, PBMP_MANAGEMENT(b->unit));
    }

    if (BCM_PBMP_NOT_NULL(b->snmp_multi_set_pbmp)) {
        BCM_PBMP_COUNT(b->snmp_multi_set_pbmp, nport);
        alloc_sz = sizeof(bcm_port_resource_t) * nport * 2;
        resource = sal_alloc(alloc_sz, "port_resource");
        if (resource == NULL) {
            return BCM_E_MEMORY;
        }
        sal_memset(resource, 0, alloc_sz);
    }

    p = 0;
    /* Set up all port speeds */
    PBMP_PORT_ITER(b->unit, port) {
        rv = bcm_port_ability_local_get(b->unit, port, &ability);

        switch(speed) {
        case 10:
            if ((ability.speed_full_duplex & SOC_PA_SPEED_10MB) ||
                (ability.speed_half_duplex & SOC_PA_SPEED_10MB)){
                speedToUse = 10;
                break;
            }
            /* fall through */
        case 100:
            if ((ability.speed_full_duplex & SOC_PA_SPEED_100MB) ||
                (ability.speed_half_duplex & SOC_PA_SPEED_100MB)){
                speedToUse = 100;
                break;
            }
            /* fall through */
        case 1000:
            if ((ability.speed_full_duplex & SOC_PA_SPEED_1000MB) ||
                (ability.speed_half_duplex & SOC_PA_SPEED_1000MB)){
                speedToUse = 1000;
                break;
            }
            /* fall through */
        case 2500:
            if ((ability.speed_full_duplex & SOC_PA_SPEED_2500MB) ||
                (ability.speed_half_duplex & SOC_PA_SPEED_2500MB)){
                speedToUse = 2500;
                break;
            }
            /* fall through */
        case 3000:
            if ((ability.speed_full_duplex & SOC_PA_SPEED_3000MB) ||
                (ability.speed_half_duplex & SOC_PA_SPEED_3000MB)){
                speedToUse = 3000;
                break;
            }
            /* fall through */
        case 10000:
            if ((ability.speed_full_duplex & SOC_PA_SPEED_10GB) ||
                (ability.speed_half_duplex & SOC_PA_SPEED_10GB) ||
                (b->snmp_save_port[port].port_ability.speed_full_duplex & SOC_PA_SPEED_10GB) ||
                (b->snmp_save_port[port].port_ability.speed_half_duplex & SOC_PA_SPEED_10GB)){
                speedToUse = 10000;
                break;
            }
            /* fall through */
        default:
            rv = bcm_port_speed_max(b->unit, port, &speedToUse);
            if (speedToUse < b->snmp_save_port[port].speed_max) {
                speedToUse = b->snmp_save_port[port].speed_max;
            }
        }

        if (bsl_check(bslLayerAppl, bslSourceTests, bslSeverityNormal, b->unit)) {
            LOG_INFO(BSL_LS_APPL_TESTS,
                     (BSL_META_U(b->unit,
                                 "Port %d: full 0x%x half 0x%x actual speed %d\n"),
                     port, ability.speed_full_duplex, ability.speed_half_duplex,
                     speedToUse));
        }
 
        if (BCM_FAILURE(rv) || (!speedToUse)) {
            test_error(b->unit,
                       "Port %d: Could not configure speed : %d\n",
                        port, speed);
        }

        if (BCM_PBMP_NOT_NULL(b->snmp_multi_set_pbmp) &&
            BCM_PBMP_MEMBER(b->snmp_multi_set_pbmp, port)) {
            /*
             * Build port resource
             */
            /* Keep encap, lanes, phy_port the same */
            resource[p].port = port;
            resource[p].physical_port = -1;

            bcm_port_resource_t_init(&resource[p+nport]);
            rv = bcm_port_resource_get(b->unit, port, &resource[p+nport]);
            if (BCM_SUCCESS(rv)) {
                resource[p+nport].speed = speedToUse;
                p++;
            } else {
                test_error(b->unit,
                           "Port %d: Could not get port resouce on speed %d: %s\n",
                           port, speedToUse, bcm_errmsg(rv));
            }
        } else {
            if ((rv = bcm_port_speed_set(b->unit, port, speedToUse)) < 0) {
                test_error(b->unit,
                           "Port %d: Could not set speed to %d: %s\n",
                           port, speedToUse, bcm_errmsg(rv));
            }
        }
    }

    if (BCM_PBMP_NOT_NULL(b->snmp_multi_set_pbmp)) {
        (void)bcm_linkscan_enable_get(b->unit, &b->linkscan_enable);
        (void)bcm_linkscan_enable_set(b->unit, 0);
        PBMP_ITER(b->snmp_multi_set_pbmp, port) {
            (void)bcm_port_enable_set(b->unit, port, 0);
        }
        rv = bcm_port_resource_multi_set(b->unit, nport * 2, resource);
        if (BCM_FAILURE(rv)) {
            test_error(b->unit,
                       "Port resouce configuration failed! %s\n",
                       bcm_errmsg(rv));
        }
        PBMP_ITER(b->snmp_multi_set_pbmp, port) {
            (void)bcm_port_enable_set(b->unit, port, 1);
        }
        (void)bcm_linkscan_enable_set(b->unit, b->linkscan_enable);
        sal_free(resource);
    }

    b->speed = speed;

    /* Now, call bcm_stat_get() for every stat on every port */

    PBMP_PORT_ITER(b->unit, port) {
        /*
         * Now, for every object, find out if it generates an error on
         * that port.
         */
        for (object = 0; object < snmpValCount; object++) {
            rv = bcm_stat_get(b->unit, port, object, &v64);

            if (rv == BCM_E_UNAVAIL) {
                b->not_impl++;
            } else if (rv < 0) {
                cli_out("FAIL: u=%d:speed=%d:port=%d:object=%d:rv=%d: %s\n",
                        b->unit, b->speed,port, object, rv, bcm_errmsg(rv));
                b->failures++;
            } else {
                /* No output */
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(b->unit,
                                        "PASS: u=%d:speed=%d:port=%d:object=%d:rv=%d: %s\n"),
                             b->unit, b->speed, port, object, rv, bcm_errmsg(rv)));
            }
        }
    }

    return 1;
}

static void
snmp_test_end(snmp_test_t *b)
{
    cli_out("All Ports: Max speed=%d: %d failures; %d vars not implemented\n",
            b->speed, b->failures, b->not_impl);
}

int
snmp_test_test(int unit, args_t *a, void *pa)
{
    snmp_test_t         *b = (snmp_test_t *) pa;
    int                 i, speeds[] = {10000, 10, 100, 1000, 10000, 0, -1};
    int                 rc = 0;

    COMPILER_REFERENCE(a);

    for (i = 0; speeds[i] >= 0; i++) {
        snmp_test_begin(b, speeds[i]);
        snmp_test_end(b);

        if (b->failures > 0) {
            rc = -1;
        }
    }

    return rc;
}

#endif /* BCM_ESW_SUPPORT */
