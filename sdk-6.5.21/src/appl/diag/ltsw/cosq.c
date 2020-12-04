/*! \file cosq.c
 *
 * LTSW COSQ CLI commands.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#if defined(BCM_LTSW_SUPPORT)

#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/dport.h>
#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm/cosq.h>
#include <bcm/debug.h>

#include <bcm_int/ltsw/port.h>

/******************************************************************************
 * Private functions
 */

/*!
 * \brief Show config.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  gport         GPORT ID.
 * \param [in]  numq          number of COS queue.
 * \param [in]  flags         flags.
 * \param [in]  sched_gport   sched_gport.
 * \param [in]  user_data     user_data.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_gport_show_config(int unit, bcm_gport_t port, int numq,
                       uint32 flags, bcm_gport_t sched_gport, void *user_data)
{
    bcm_cos_queue_t cosq;
    bcm_port_t user_port = *(bcm_port_t*)(user_data);
    bcm_port_t local_port = 0;
    char *mode_name = NULL, *weight_str = "packets";
    int mode, weight, weight_max, rv;

    local_port = BCM_GPORT_MODPORT_PORT_GET(port);
    if (user_port != local_port) {
        return BCM_E_NONE;
    }

    if (!BCM_GPORT_IS_SCHEDULER(sched_gport)) {
        return BCM_E_NONE;
    }

    for (cosq = 0; cosq < 12; cosq++) {
        rv = bcm_cosq_gport_sched_get(unit, sched_gport, cosq, &mode, &weight);
        if (rv < 0) {
            break;
        }

        switch (mode) {
        case BCM_COSQ_STRICT:
            mode_name = "strict";
            break;
        case BCM_COSQ_ROUND_ROBIN:
            mode_name = "simple round-robin";
            break;
        case BCM_COSQ_WEIGHTED_ROUND_ROBIN:
            mode_name = "weighted round-robin";
            break;
        case BCM_COSQ_WEIGHTED_FAIR_QUEUING:
            mode_name = "weighted fair queuing";
            break;
        case BCM_COSQ_BOUNDED_DELAY:
            mode_name = "weighted round-robin with bounded delay";
            break;
        case BCM_COSQ_DEFICIT_ROUND_ROBIN:
            mode_name = "deficit round-robin";
            weight_str = "Kbytes";
            break;
        default:
            mode_name = NULL;
            break;
        }

        if ((rv = bcm_cosq_sched_weight_max_get(unit,
                                               mode, &weight_max)) < 0) {
            return rv;
        }

        if (cosq == 0 && mode_name) {
            cli_out("  VLAN COSQ Schedule mode: %s\n", mode_name);
        }

        if (mode >= 0 && mode != BCM_COSQ_STRICT) {
            if (cosq == 0) {
                cli_out("  VLAN COSQ Weighting (in %s):\n", weight_str);
            }

            if (weight_max == BCM_COSQ_WEIGHT_UNLIMITED) {
                cli_out("    VLAN COSQ %d = %u %s\n",
                        cosq, (uint32) weight, weight_str);
            } else {
                cli_out("    VLAN COSQ %d = %d %s\n",
                        cosq, weight, weight_str);
            }
        }
    }

    return BCM_E_NONE;
}

/*!
 * \brief Show bandwidth.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  gport         GPORT ID.
 * \param [in]  numq          number of COS queue.
 * \param [in]  flags         flags.
 * \param [in]  sched_gport   sched_gport.
 * \param [in]  user_data     user_data.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
static int
cosq_gport_show_bandwidth(int unit, bcm_gport_t port, int numq,
                          uint32 flags, bcm_gport_t sched_gport, void *user_data)
{
    bcm_cos_queue_t cosq;
    bcm_port_t user_port = *(bcm_port_t*)(user_data);
    bcm_port_t local_port = 0;
    uint32 kbits_sec_min, kbits_sec_max, bw_flags;

    BCM_IF_ERROR_RETURN
        (bcm_port_local_get(unit, port, &local_port));

    if (user_port != local_port) {
        return BCM_E_NONE;
    }

    if (!BCM_GPORT_IS_SCHEDULER(sched_gport)) {
        return BCM_E_NONE;
    }

    for (cosq = 0; cosq < 12; cosq++) {
        if (bcm_cosq_gport_bandwidth_get(unit, sched_gport, cosq,
                   &kbits_sec_min, &kbits_sec_max, &bw_flags) == 0) {
            cli_out("  U %4s | %d | %8d | %8d | %6d\n",
                    bcmi_ltsw_port_name(unit, local_port), cosq, kbits_sec_min,
                    kbits_sec_max, bw_flags);
        }
    }

    return BCM_E_NONE;
}

static int
ltsw_cos_port_bandwidth(int unit, args_t *a, char *c)
{
    uint32 kbits_sec_min, kbits_sec_max, bw_flags;
    bcm_gport_t gport, sched_gport = BCM_GPORT_INVALID;
    int                 r;
    bcm_cos_queue_t     cosq;
    parse_table_t       pt;
    cmd_result_t        retCode;
    bcm_pbmp_t          pbmp;
    bcm_port_t          p;

    BCM_PBMP_CLEAR(pbmp);
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "PortBitMap", PQ_DFL|PQ_PBMP|PQ_BCM,
                    (void *)(0), &pbmp, NULL);
    parse_table_add(&pt, "Queue", PQ_INT,
                    (void *)( 0), &cosq, NULL);
    parse_table_add(&pt, "KbpsMIn", PQ_INT,
                    (void *)( 0), &kbits_sec_min, NULL);
    parse_table_add(&pt, "KbpsMAx", PQ_INT,
                    (void *)( 0), &kbits_sec_max, NULL);
    parse_table_add(&pt, "Flags", PQ_INT,
                    (void *)( 0), &bw_flags, NULL);
    parse_table_add(&pt, "Gport", PQ_INT,
                    (void *)BCM_GPORT_INVALID,
                    &sched_gport, 0);

    if (!parseEndOk(a, &pt, &retCode)) {
        return retCode;
    }

    if (BCM_PBMP_IS_NULL(pbmp)) {
        cli_out("%s ERROR: empty port bitmap\n", ARG_CMD(a));
        return CMD_FAIL;
    }

    if (sched_gport == BCM_GPORT_INVALID) {
        BCM_PBMP_ITER(pbmp, p) {
            BCM_GPORT_LOCAL_SET(gport, p);
            if ((r = bcm_cosq_gport_bandwidth_set(unit, gport, cosq,
                kbits_sec_min,
                kbits_sec_max,
                bw_flags)) < 0) {
                cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(r));
                return CMD_FAIL;
            }
        }
    } else {
        if ((r = bcm_cosq_gport_bandwidth_set(unit, sched_gport, cosq,
            kbits_sec_min,
            kbits_sec_max,
            bw_flags)) < 0) {
            cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(r));
            return CMD_FAIL;
        }
    }

    return CMD_OK;
}

static int
ltsw_cos_port_weight(int unit, args_t *a, char *subcmd, char *c)
{
    bcm_gport_t gport, sched_gport = BCM_GPORT_INVALID;
    int                 r;
    bcm_cos_queue_t     cosq;
    int                 mode = 0;
    parse_table_t       pt;
    cmd_result_t        retCode;
    bcm_pbmp_t          pbmp;
    bcm_port_t          p;
    int weight = 0;

    parse_table_init(unit, &pt);
    if (sal_strcasecmp(subcmd, "strict") != 0 &&
        sal_strcasecmp(subcmd, "simple") != 0) {
        parse_table_add(&pt, "Weight",  PQ_DFL|PQ_INT,
                        (void *)( 0), &weight, NULL);
    }
    parse_table_add(&pt, "Queue",   PQ_DFL|PQ_INT,
                    (void *)(0), &cosq, NULL);
    parse_table_add(&pt, "PortBitMap", PQ_DFL|PQ_PBMP|PQ_BCM,
                    (void *)(0), &pbmp, NULL);
    parse_table_add(&pt, "Gport", PQ_INT,
                    (void *)BCM_GPORT_INVALID,
                    &sched_gport, 0);

    if (!parseEndOk(a, &pt, &retCode)) {
        return retCode;
    }

    if (sal_strcasecmp(subcmd, "fair") == 0) {
        mode = BCM_COSQ_WEIGHTED_FAIR_QUEUING;
    } else if (sal_strcasecmp(subcmd, "simple") == 0) {
        mode = BCM_COSQ_ROUND_ROBIN;
    } else if (sal_strcasecmp(subcmd, "strict") == 0) {
        mode = BCM_COSQ_STRICT;
    } else if (sal_strcasecmp(subcmd, "drr") == 0) {
        mode = BCM_COSQ_DEFICIT_ROUND_ROBIN;
    } else if (sal_strcasecmp(subcmd, "bounded") == 0) {
        mode = BCM_COSQ_BOUNDED_DELAY;
    } else if (sal_strcasecmp(subcmd, "delay") == 0) {
        mode = BCM_COSQ_BOUNDED_DELAY;
    } else {
        mode = BCM_COSQ_WEIGHTED_ROUND_ROBIN;
    }

    if (sched_gport == BCM_GPORT_INVALID) {
        if (BCM_PBMP_IS_NULL(pbmp)) {
            cli_out("%s: ERROR: must specify valid port bitmap.\n",
                    ARG_CMD(a));
            return CMD_FAIL;
        } else {
            BCM_PBMP_ITER(pbmp, p) {
                BCM_GPORT_LOCAL_SET(gport, p);
                    if ((r = bcm_cosq_gport_sched_set(unit, gport, cosq, mode, weight)) < 0) {
                        cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(r));
                        return CMD_FAIL;
                }
            }
        }
    } else {
        if ((r = bcm_cosq_gport_sched_set(unit, sched_gport, cosq, mode, weight)) < 0) {
            cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(r));
            return CMD_FAIL;
        }
    }
    return CMD_OK;
}

static int
ltsw_cos_port_map(int unit,  args_t *a, char *c)
{
    int                 r;
    bcm_cos_queue_t     cosq;
    bcm_cos_t           lastPrio = 0;
    parse_table_t       pt;
    cmd_result_t        retCode;
    bcm_pbmp_t          pbmp;
    bcm_port_t          p;
    uint32 flags = BCM_COSQ_GPORT_UCAST_QUEUE_GROUP |
                   BCM_COSQ_GPORT_MCAST_QUEUE_GROUP;

    BCM_PBMP_CLEAR(pbmp);
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "PortBitMap", PQ_DFL|PQ_PBMP|PQ_BCM,
                    (void *)(0), &pbmp, NULL);
    parse_table_add(&pt, "Pri",     PQ_DFL|PQ_INT,
                    (void *)(0), &lastPrio, NULL);
    parse_table_add(&pt, "Queue",   PQ_DFL|PQ_INT,
                    (void *)(0), &cosq, NULL);

    if (!parseEndOk(a, &pt, &retCode)) {
        if (c != NULL) {
            return retCode;
        }
    }
    if (BCM_PBMP_IS_NULL(pbmp)) {
        cli_out("%s: ERROR: must specify valid port bitmap.\n",
                ARG_CMD(a));
        return CMD_FAIL;
    } else {
        BCM_PBMP_ITER(pbmp, p) {
            r = bcm_cosq_gport_mapping_set(unit, p, lastPrio, flags, -1, cosq);
            if (r < 0) {
                cli_out("%s: ERROR: port %s: %s\n",
                        ARG_CMD(a),
                        bcmi_ltsw_port_name(unit, p),
                        bcm_errmsg(r));
                return CMD_FAIL;
            }
        }
    }

    return CMD_OK;
}

static int
ltsw_cos_port_show(int unit, args_t *a, char *c)
{
    bcm_gport_t gport;
    int                 r;
    bcm_cos_queue_t     cosq;
    int                 weights[16];
    int                 delay = 0;
    int                 numq, mode = 0;
    bcm_cos_t           prio;
    parse_table_t       pt;
    cmd_result_t        retCode;
    bcm_pbmp_t          pbmp;
    bcm_port_t          p;
    int weight_arr_size;
    char *mode_name, *weight_str = "packets";
    int weight_max;
    bcm_pbmp_t temp_pbmp;
    uint32 flags = BCM_COSQ_GPORT_UCAST_QUEUE_GROUP |
                   BCM_COSQ_GPORT_MCAST_QUEUE_GROUP;
    bcm_gport_t node_gport;
    bcm_gport_t port;
    uint32 flag;

    BCM_PBMP_CLEAR(pbmp);
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "PortBitMap", PQ_DFL|PQ_PBMP|PQ_BCM,
                    (void *)(0), &pbmp, NULL);

    if (!parseEndOk( a, &pt, &retCode)) {
        if (c != NULL) {
            return retCode;
        }
    }

    if (BCM_PBMP_IS_NULL(pbmp)) {
        cli_out("%s: ERROR: must specify valid port bitmap.\n",
                ARG_CMD(a));
        return CMD_FAIL;
    }
    BCM_PBMP_ITER(pbmp, p) {
        BCM_PBMP_CLEAR(temp_pbmp);
        SOC_PBMP_PORT_ADD(temp_pbmp, p);

        r = bcm_cosq_port_sched_config_get(unit, temp_pbmp, 12, weights, &weight_arr_size, &mode);

        if (r < 0) {
            cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(r));
            return CMD_FAIL;
        }
        cli_out("\n  Port %s COS configuration:\n", bcmi_ltsw_port_name(unit, p));
        cli_out(" ------------------------------\n");

        switch (mode) {
            case BCM_COSQ_STRICT:
                mode_name = "strict";
                break;
            case BCM_COSQ_ROUND_ROBIN:
                mode_name = "simple round-robin";
                break;
            case BCM_COSQ_WEIGHTED_ROUND_ROBIN:
                mode_name = "weighted round-robin";
                break;
            case BCM_COSQ_WEIGHTED_FAIR_QUEUING:
                mode_name = "weighted fair queuing";
                break;
            case BCM_COSQ_BOUNDED_DELAY:
                mode_name = "weighted round-robin with bounded delay";
                break;
            case BCM_COSQ_DEFICIT_ROUND_ROBIN:
                mode_name = "deficit round-robin";
                weight_str = "Kbytes";
                break;
            default:
                mode_name = NULL;
                break;
        }

        if ((r = bcm_cosq_sched_weight_max_get(unit,
                 mode, &weight_max)) < 0) {
            cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(r));
            return CMD_FAIL;
        }

        BCM_GPORT_LOCAL_SET(gport, p);

        if ((r = bcm_cosq_gport_get(unit, gport, &port, &numq, &flag)) < 0) {
            cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(r));
            return CMD_FAIL;
        }

        cli_out("  Config (max queues): %d\n", numq);
        if (mode_name) {
            cli_out("  Schedule mode: %s\n", mode_name);
        }

        if (mode == BCM_COSQ_STRICT) {
            for (cosq = 0; cosq < numq; cosq++) {
                if (weights[cosq] != BCM_COSQ_WEIGHT_STRICT) {
                    mode = BCM_COSQ_WEIGHTED_ROUND_ROBIN;
                    break;
                }
            }
        }

        if (mode >= 0 && mode != BCM_COSQ_STRICT) {
            cli_out("  Weighting (in %s):\n", weight_str);

            for (cosq = 0; cosq < numq; cosq++) {
                if (weight_max == BCM_COSQ_WEIGHT_UNLIMITED) {
                    cli_out("    COSQ %d = %u %s\n",
                            cosq, (uint32) weights[cosq], weight_str);
                } else {
                    cli_out("    COSQ %d = %d %s\n",
                            cosq, weights[cosq], weight_str);
                }
            }
            if (bcm_cosq_gport_sched_get(unit, gport, 8, &mode, &weights[0]) == 0) {
                cli_out("    COSQ 8 = %d %s (queue for output of S1 scheduler)\n",
                        weights[0], weight_str);
            }
        }
        cli_out("  Priority to queue mappings:\n");

        for (prio = 0; prio < 8; prio++) {
            if ((r = bcm_cosq_gport_mapping_get(unit, p, prio, flags, &node_gport, &cosq)) < 0) {
                cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(r));
                return CMD_FAIL;
            }
            cli_out("    PRIO %d ==> COSQ %d\n", prio, cosq);
        }

        for (prio = 8; prio < 16; prio++) {
            if (bcm_cosq_gport_mapping_get(unit, p, prio, flags, &node_gport, &cosq) < 0) {
                break;
            }
            cli_out("    PRIO %d ==> COSQ %d\n", prio, cosq);
        }

        if (mode == BCM_COSQ_BOUNDED_DELAY) {
            cli_out("  Bounded delay: %d usec\n", delay);
        }

        /* Show VLAN cosq config for this port */
        (void) bcm_cosq_gport_traverse(unit, cosq_gport_show_config, &p);
    }
    return CMD_OK;
}


static int
ltsw_cos_port_config(int unit, args_t *a, char *c)
{
    int                 r;
    parse_table_t       pt;
    cmd_result_t        retCode;
    bcm_pbmp_t          pbmp;
    bcm_port_t          p;
    int num_vlanq = 0;
    bcm_gport_t gport, sched_gport = BCM_GPORT_INVALID;

    BCM_PBMP_CLEAR(pbmp);
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Numcos", PQ_DFL|PQ_INT,
                    (void *)( 0), &num_vlanq, NULL);
    parse_table_add(&pt, "PortBitMap", PQ_DFL|PQ_PBMP|PQ_BCM,
                    (void *)(0), &pbmp, NULL);

    if (!parseEndOk(a, &pt, &retCode)) {
        return retCode;
    }
    if (BCM_PBMP_IS_NULL(pbmp)) {
        cli_out("%s: ERROR: must specify valid port bitmap.\n",
                ARG_CMD(a));
        return CMD_FAIL;
    }

    BCM_PBMP_ITER(pbmp, p) {
        BCM_GPORT_LOCAL_SET(gport, p);
        r = bcm_cosq_gport_add(unit, gport, num_vlanq,
                               BCM_COSQ_GPORT_SCHEDULER, &sched_gport);
        if (r < 0) {
            cli_out("%s: bcm_cosq_gport_add ERROR: port %s: %s\n",
                    ARG_CMD(a),
                    bcmi_ltsw_port_name(unit, p),
                    bcm_errmsg(r));
            return CMD_FAIL;
        }
        r = bcm_cosq_gport_attach(unit, sched_gport, gport, 8);
        if (r < 0) {
            cli_out("%s: bcm_cosq_gport_attach ERROR: port %s: %s\n",
                    ARG_CMD(a),
                    bcmi_ltsw_port_name(unit, p),
                    bcm_errmsg(r));
            return CMD_FAIL;
        } else {
            cli_out(" Added/Attached GPORT %d (0x%08x) to port %s.\n",
            sched_gport, sched_gport, bcmi_ltsw_port_name(unit, p));
        }
    }

    return CMD_OK;
}

static int
ltsw_cos_port(int unit, args_t *a, char *c)
{
    char                *subcmd;

    if ((subcmd = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    if (sal_strcasecmp(subcmd, "config") == 0) {
        return ltsw_cos_port_config(unit, a, c);
    }

    if (sal_strcasecmp(subcmd, "show") == 0) {
        return ltsw_cos_port_show(unit, a, c);
    }

    if (sal_strcasecmp(subcmd, "map") == 0) {
        return ltsw_cos_port_map(unit, a, c);
    }

    if (sal_strcasecmp(subcmd, "weight") == 0 ||
        sal_strcasecmp(subcmd, "drr") == 0 ||
        sal_strcasecmp(subcmd, "bounded") == 0 ||
        sal_strcasecmp(subcmd, "delay") == 0 ||
        sal_strcasecmp(subcmd, "strict") == 0 ||
        sal_strcasecmp(subcmd, "simple") == 0 ||
        sal_strcasecmp(subcmd, "fair") == 0) {
        return ltsw_cos_port_weight(unit, a, subcmd, c);
    }

    if (sal_strcasecmp(subcmd, "bandwidth") == 0) {
        return ltsw_cos_port_bandwidth(unit, a, c);
    }

    return CMD_OK;
}

static int
ltsw_cos_clear(int unit, args_t *a, char *c)
{
    int r;

    if (c != NULL) {
        return CMD_USAGE;
    }

    if ((r = bcm_cosq_init(unit)) < 0) {
        cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(r));
        return CMD_FAIL;
    }

    return CMD_OK;
}

static int
ltsw_cos_strict(int unit, args_t *a, char *c)
{
    int r;
    int weights[16];

    if (c != NULL) {
        return CMD_USAGE;
    }

    sal_memset(weights, 0, (16 * sizeof(int)));
    if ((r = bcm_cosq_sched_config_set(unit, BCM_COSQ_STRICT, 12, weights)) < 0){
        cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(r));
        return CMD_FAIL;
    }

    return CMD_OK;
}

static int
ltsw_cos_simple(int unit, args_t *a, char *c)
{
    int                 r;
    int                 weights[16];


    if (c != NULL) {
        return CMD_USAGE;
    }

    sal_memset(weights, 0, (16 * sizeof(int)));
    if ((r = bcm_cosq_sched_config_set(unit, BCM_COSQ_ROUND_ROBIN,
                                       12, weights)) < 0){
        cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(r));
        return CMD_FAIL;
    }

    return CMD_OK;
}

static int
ltsw_cos_show(int unit,  args_t *a, char *c)
{
    int                 r;
    bcm_cos_queue_t     cosq;
    int                 weights[16];
    int                 delay = 0;
    int                 numq = 12, mode = 0;
    bcm_cos_t           prio;
    int weight_arr_size;
    char *mode_name;
    char *weight_str = "packets";
    int weight_max;
    sal_memset(weights, 0, (16 * sizeof(int)));

    if (c != NULL) {
        return CMD_USAGE;
    }

    if ((r = bcm_cosq_sched_config_get(unit, 12, weights, &weight_arr_size, &mode)) < 0) {
        cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(r));
        return CMD_FAIL;
    }

    switch (mode) {
        case BCM_COSQ_STRICT:
            mode_name = "strict";
            break;
        case BCM_COSQ_ROUND_ROBIN:
            mode_name = "simple round-robin";
            break;
        case BCM_COSQ_WEIGHTED_ROUND_ROBIN:
            mode_name = "weighted round-robin";
            break;
        case BCM_COSQ_WEIGHTED_FAIR_QUEUING:
            mode_name = "weighted fair queuing";
            break;
        case BCM_COSQ_BOUNDED_DELAY:
            mode_name = "weighted round-robin with bounded delay";
            break;
        case BCM_COSQ_DEFICIT_ROUND_ROBIN:
            mode_name = "deficit round-robin";
            weight_str = "Kbytes";
            break;
        default:
            mode_name = NULL;
            break;
    }

    if ((r = bcm_cosq_sched_weight_max_get(unit,
        mode, &weight_max)) < 0) {
        cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(r));
        return CMD_FAIL;
    }

    cli_out("COS configuration:\n");
    cli_out("  Config (max queues): %d\n", numq);
    if (mode_name) {
        cli_out("  Schedule mode: %s\n", mode_name);
    }

    if (mode >= 0 && mode != BCM_COSQ_STRICT) {
        cli_out("  Weighting (in %s):\n", weight_str);
        for (cosq = 0; cosq < numq; cosq++) {
            if (weight_max == BCM_COSQ_WEIGHT_UNLIMITED) {
                cli_out("    COSQ %d = %u %s\n",
                        cosq, (uint32) weights[cosq], weight_str);
            } else {
                cli_out("    COSQ %d = %d %s\n",
                        cosq, weights[cosq], weight_str);
            }
        }
    }
    cli_out("  Priority to queue mappings:\n");

    for (prio = 0; prio < 8; prio++) {
        if ((r = bcm_cosq_mapping_get(unit, prio, &cosq)) < 0) {
            cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(r));
            return CMD_FAIL;
        }
        cli_out("    PRIO %d ==> COSQ %d\n", prio, cosq);
    }

    for (prio = 8; prio < 16; prio++) {
        if (bcm_cosq_mapping_get(unit, prio, &cosq) < 0) {
            break;
        }
        cli_out("    PRIO %d ==> COSQ %d\n", prio, cosq);
    }

    if (mode == BCM_COSQ_BOUNDED_DELAY) {
        cli_out("  Bounded delay: %d usec\n", delay);
    }

    return CMD_OK;
}

static int
ltsw_cos_map(int unit, args_t *a, char *c)
{
    int                 r;
    bcm_cos_queue_t     cosq;
    bcm_cos_t           lastPrio = 0;
    parse_table_t       pt;
    cmd_result_t        retCode;
    bcm_pbmp_t          pbmp;
    bcm_port_t          p;
    uint32 flags = BCM_COSQ_GPORT_UCAST_QUEUE_GROUP |
                   BCM_COSQ_GPORT_MCAST_QUEUE_GROUP;

    if ((r = bcm_cosq_mapping_get(unit, lastPrio, &cosq)) < 0) {
        cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(r));
        return CMD_FAIL;
    }

    BCM_PBMP_CLEAR(pbmp);
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "PortBitMap", PQ_DFL|PQ_PBMP|PQ_BCM,
                    (void *)(0), &pbmp, NULL);
    parse_table_add(&pt, "Pri", PQ_DFL|PQ_INT,
                    (void *)(0), &lastPrio, NULL);
    parse_table_add(&pt, "Queue", PQ_DFL|PQ_INT,
                    (void *)(0), &cosq, NULL);

    if (!parseEndOk(a, &pt, &retCode)) {
        return retCode;
    }

    if (BCM_PBMP_IS_NULL(pbmp)) {
        if ((r = bcm_cosq_mapping_set(unit, lastPrio, cosq)) < 0) {
            cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(r));
            return CMD_FAIL;
        }
    } else {
        /* coverity[overrun-local : FALSE] */
        BCM_PBMP_ITER(pbmp, p) {
            r = bcm_cosq_gport_mapping_set(unit, p, lastPrio, flags, -1, cosq);
            if (r < 0) {
                cli_out("%s: ERROR: port %s: %s\n",
                        ARG_CMD(a),
                        bcmi_ltsw_port_name(unit, p),
                        bcm_errmsg(r));
                return CMD_FAIL;
            }
        }
    }

    return CMD_OK;
}

static int
ltsw_cos_weight(int unit, args_t *a, char *subcmd, char *c)
{
    int                 r;
    int                 weights[16];
    int                 mode = 0;
    parse_table_t       pt;
    cmd_result_t        retCode;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "W0", PQ_DFL|PQ_INT,
                    (void *)( 0), &weights[0], NULL);
    parse_table_add(&pt, "W1", PQ_DFL|PQ_INT,
                    (void *)( 0), &weights[1], NULL);
    parse_table_add(&pt, "W2", PQ_DFL|PQ_INT,
                    (void *)( 0), &weights[2], NULL);
    parse_table_add(&pt, "W3", PQ_DFL|PQ_INT,
                    (void *)( 0), &weights[3], NULL);
    parse_table_add(&pt, "W4", PQ_DFL|PQ_INT,
                    (void *)( 0), &weights[4], NULL);
    parse_table_add(&pt, "W5", PQ_DFL|PQ_INT,
                    (void *)( 0), &weights[5], NULL);
    parse_table_add(&pt, "W6", PQ_DFL|PQ_INT,
                    (void *)( 0), &weights[6], NULL);
    parse_table_add(&pt, "W7", PQ_DFL|PQ_INT,
                    (void *)( 0), &weights[7], NULL);
    parse_table_add(&pt, "W8",  PQ_DFL|PQ_INT,
                    (void *)( 0), &weights[8], NULL);
    parse_table_add(&pt, "W9",  PQ_DFL|PQ_INT,
                    (void *)( 0), &weights[9], NULL);
    parse_table_add(&pt, "W10", PQ_DFL|PQ_INT,
                    (void *)( 0), &weights[10], NULL);
    parse_table_add(&pt, "W11", PQ_DFL|PQ_INT,
                    (void *)( 0), &weights[11], NULL);

    if (!parseEndOk(a, &pt, &retCode)) {
        return retCode;
    }

    if (sal_strcasecmp(subcmd, "fair") == 0) {
        mode = BCM_COSQ_WEIGHTED_FAIR_QUEUING;
    } else if (sal_strcasecmp(subcmd, "drr") == 0) {
        mode = BCM_COSQ_DEFICIT_ROUND_ROBIN;
    } else {
        mode = BCM_COSQ_WEIGHTED_ROUND_ROBIN;
    }
    if ((r = bcm_cosq_sched_config_set(unit, mode, 12, weights)) < 0) {
        cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(r));
        return CMD_FAIL;
    }

    return CMD_OK;
}

static int
ltsw_cos_bounded(int unit, args_t *a, char *c)
{
    int                 r;
    int                 weights[16];
    int                 delay = 0;
    parse_table_t       pt;
    cmd_result_t        retCode;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "W0", PQ_DFL|PQ_INT,
                    (void *)( 0), &weights[0], NULL);
    parse_table_add(&pt, "W1", PQ_DFL|PQ_INT,
                    (void *)( 0), &weights[1], NULL);
    parse_table_add(&pt, "W2", PQ_DFL|PQ_INT,
                    (void *)( 0), &weights[2], NULL);
    parse_table_add(&pt, "W3", PQ_DFL|PQ_INT,
                    (void *)( 0), &weights[3], NULL);
    parse_table_add(&pt, "W4", PQ_DFL|PQ_INT,
                    (void *)( 0), &weights[4], NULL);
    parse_table_add(&pt, "W5", PQ_DFL|PQ_INT,
                    (void *)( 0), &weights[5], NULL);
    parse_table_add(&pt, "W6", PQ_DFL|PQ_INT,
                    (void *)( 0), &weights[6], NULL);
    parse_table_add(&pt, "W7", PQ_DFL|PQ_INT,
                    (void *)( 0), &weights[7], NULL);
    parse_table_add(&pt, "W8",  PQ_DFL|PQ_INT,
                    (void *)( 0), &weights[8], NULL);
    parse_table_add(&pt, "W9",  PQ_DFL|PQ_INT,
                    (void *)( 0), &weights[9], NULL);
    parse_table_add(&pt, "W10", PQ_DFL|PQ_INT,
                    (void *)( 0), &weights[10], NULL);
    parse_table_add(&pt, "W11", PQ_DFL|PQ_INT,
                    (void *)( 0), &weights[11], NULL);
    parse_table_add(&pt, "Delay", PQ_DFL|PQ_INT,
                    (void *)( 0), &delay, NULL);

    if (!parseEndOk(a, &pt, &retCode)) {
        return retCode;
    }

    if ((r = bcm_cosq_sched_config_set(unit, BCM_COSQ_BOUNDED_DELAY,
                                       12, weights)) < 0) {
        cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(r));
        return CMD_FAIL;
    }

    return CMD_OK;
}

static int
ltsw_cos_discard(int unit, args_t *a, char *c)
{
    int                 r;
    parse_table_t       pt;
    cmd_result_t        retCode;
    int                 discard_ena, discard_cap_avg;
    uint32 discard_flags = 0;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Enable", PQ_BOOL,
                    0, &discard_ena, NULL);
    parse_table_add(&pt, "CapAvg", PQ_BOOL,
                    0, &discard_cap_avg, NULL);

    if (!parseEndOk( a, &pt, &retCode)) {
        return retCode;
    }

    if (discard_ena) {
        discard_flags |= BCM_COSQ_DISCARD_ENABLE;
    }
    if (discard_cap_avg) {
        discard_flags |= BCM_COSQ_DISCARD_CAP_AVERAGE;
    }

    if ((r = bcm_cosq_discard_set(unit, discard_flags)) < 0) {
        cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(r));
        return CMD_FAIL;
    }

    return CMD_OK;
}

static int
ltsw_cos_discard_show(int unit, args_t *a, char *c)
{
    uint32              discard_flags = 0;
    bcm_gport_t         gport = 0;
    bcm_cosq_gport_discard_t discard;
    int                 r;
    bcm_cos_queue_t     cosq;
    parse_table_t       pt;
    cmd_result_t        retCode;
    bcm_port_config_t   pcfg;

    if (bcm_port_config_get(unit, &pcfg) != BCM_E_NONE) {
        cli_out("%s: Error: bcm ports not initialized\n", ARG_CMD(a));
        return CMD_FAIL;
    }

    if (ARG_CNT(a)) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Gport", PQ_INT, (void *)( 0), &gport, 0);
        parse_table_add(&pt, "Queue", PQ_INT, (void *)( 0), &cosq, NULL);

        if (!parseEndOk( a, &pt, &retCode)) {
            return retCode;
        }

        if (gport != 0) {
            cli_out(" Discard configuration GPORT=%d (%08x), COSQ %d\n",
                    gport, gport, cosq);

            discard.flags = BCM_COSQ_DISCARD_COLOR_GREEN | BCM_COSQ_DISCARD_BYTES;
            if (bcm_cosq_gport_discard_get(unit, gport, cosq, &discard) == 0) {
                cli_out("    GREEN  (bytes) : Min %d, Max %d, Gain %d, DropProb %d, Ena %d, Cap %d\n",
                        discard.min_thresh, discard.max_thresh, discard.gain,
                        discard.drop_probability,
                        (discard.flags & BCM_COSQ_DISCARD_ENABLE) ? 1 : 0,
                        (discard.flags & BCM_COSQ_DISCARD_CAP_AVERAGE) ? 1 : 0);
            }
            discard.flags = BCM_COSQ_DISCARD_COLOR_YELLOW | BCM_COSQ_DISCARD_BYTES;
            if (bcm_cosq_gport_discard_get(unit, gport, cosq, &discard) == 0) {
                cli_out("    YELLOW (bytes) : Min %d, Max %d, Gain %d, DropProb %d, Ena %d, Cap %d\n",
                        discard.min_thresh, discard.max_thresh, discard.gain,
                        discard.drop_probability,
                        (discard.flags & BCM_COSQ_DISCARD_ENABLE) ? 1 : 0,
                        (discard.flags & BCM_COSQ_DISCARD_CAP_AVERAGE) ? 1 : 0);
            }
            discard.flags = BCM_COSQ_DISCARD_COLOR_RED | BCM_COSQ_DISCARD_BYTES;
            if (bcm_cosq_gport_discard_get(unit, gport, cosq, &discard) == 0) {
                cli_out("    RED    (bytes) : Min %d, Max %d, Gain %d, DropProb %d, Ena %d, Cap %d\n",
                        discard.min_thresh, discard.max_thresh, discard.gain,
                        discard.drop_probability,
                        (discard.flags & BCM_COSQ_DISCARD_ENABLE) ? 1 : 0,
                        (discard.flags & BCM_COSQ_DISCARD_CAP_AVERAGE) ? 1 : 0);
            }

            return CMD_OK;
        }
    }

    if ((r = bcm_cosq_discard_get(unit, &discard_flags)) < 0) {
        if (r != BCM_E_UNAVAIL) {
            cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(r));
            return CMD_FAIL;
        }
    }

    cli_out("  Discard (WRED): discard %s, cap-average %s\n",
            (discard_flags & BCM_COSQ_DISCARD_ENABLE) ?
            "enabled" : "disabled",
            (discard_flags & BCM_COSQ_DISCARD_CAP_AVERAGE) ?
            "enabled" : "disabled");

    return CMD_OK;
}

static int
ltsw_cos_discard_gport(int unit, args_t *a, char *c)
{
    parse_table_t       pt;
    bcm_gport_t         sched_gport;
    bcm_cosq_gport_discard_t discard;
    int                 r;
    bcm_cos_queue_t     cosq;
    int                 discard_ena, discard_cap_avg, gain, drop_prob;
    int                 discard_start, discard_end;
    char *              discard_color;
    char*               discard_mode;
    uint32              color_flags = 0;
    uint32              mode_flags = 0;

    bcm_cosq_gport_discard_t_init(&discard);
    discard_color = NULL;
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "GPort", PQ_INT,
                    (void *)BCM_GPORT_INVALID,
                    &sched_gport, 0);
    parse_table_add(&pt, "Queue", PQ_INT,
                    (void *)( 0), &cosq, NULL);
    parse_table_add(&pt, "Color", PQ_STRING,
                    "green", &discard_color, NULL);
    parse_table_add(&pt, "DropStart", PQ_INT,
                    (void *)( 0), &discard_start, NULL);
    parse_table_add(&pt, "DropEnd", PQ_INT,
                    (void *)( 0), &discard_end, NULL);
    parse_table_add(&pt, "DropProbability", PQ_INT,
                    (void *)( 0), &drop_prob, NULL);
    parse_table_add(&pt, "GAin", PQ_INT,
                    (void *)( 0), &gain, NULL);
    parse_table_add(&pt, "Enable", PQ_BOOL,
                    0, &discard_ena, NULL);
    parse_table_add(&pt, "CapAvg", PQ_BOOL,
                    0, &discard_cap_avg, NULL);
    parse_table_add(&pt, "Mode", PQ_STRING,
                    "byte", &discard_mode, NULL);

    /* Parse remaining arguments */
    if (0 > parse_arg_eq(a, &pt)) {
        cli_out("%s: Error: Invalid option or malformed expression: %s\n",
                ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }

    if (discard_mode != NULL) {
        if (!strncmp(discard_mode, "b", 1)) {
            mode_flags = BCM_COSQ_DISCARD_BYTES;
        } else if (!strncmp(discard_mode, "p", 1)) {
            mode_flags = BCM_COSQ_DISCARD_PACKETS;
        } else {
            cli_out("%s ERROR: Invalid mode value\n", ARG_CMD(a));
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }
    } else {
        mode_flags = BCM_COSQ_DISCARD_BYTES;
    }

    if (discard_color != NULL) {
        if (!strncmp(discard_color, "r", 1)) {
            color_flags = BCM_COSQ_DISCARD_COLOR_RED;
        } else if (!strncmp(discard_color, "y", 1)) {
            color_flags = BCM_COSQ_DISCARD_COLOR_YELLOW;
        } else if (!strncmp(discard_color, "g", 1)) {
            color_flags = BCM_COSQ_DISCARD_COLOR_GREEN;
        } else {
            cli_out("%s ERROR: Invalid color value\n", ARG_CMD(a));
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }
    } else {
        color_flags = BCM_COSQ_DISCARD_COLOR_GREEN;
    }
    if (discard_ena) {
        color_flags |= BCM_COSQ_DISCARD_ENABLE;
    }
    if (discard_cap_avg) {
        color_flags |= BCM_COSQ_DISCARD_CAP_AVERAGE;
    }

    parse_arg_eq_done(&pt);

    discard.flags = color_flags | mode_flags;
    discard.min_thresh = discard_start;
    discard.max_thresh = discard_end;
    discard.drop_probability = drop_prob;
    discard.gain = gain;
    if ((r = bcm_cosq_gport_discard_set(unit, sched_gport, cosq, &discard)) < 0) {
        cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(r));
        return CMD_FAIL;
    }

    return CMD_OK;
}

static int
ltsw_cos_bandwidth(int unit, args_t *a, char *c)
{
    uint32 kbits_sec_min, kbits_sec_max, bw_flags;
    parse_table_t pt;
    bcm_cos_queue_t cosq;
    bcm_pbmp_t pbmp;
    cmd_result_t retCode;
    bcm_port_t p;
    int r;

    BCM_PBMP_CLEAR(pbmp);
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "PortBitMap", PQ_DFL|PQ_PBMP|PQ_BCM,
                    (void *)(0), &pbmp, NULL);
    parse_table_add(&pt, "Queue", PQ_INT,
                    (void *)( 0), &cosq, NULL);
    parse_table_add(&pt, "KbpsMIn", PQ_INT,
                    (void *)( 0), &kbits_sec_min, NULL);
    parse_table_add(&pt, "KbpsMAx", PQ_INT,
                    (void *)( 0), &kbits_sec_max, NULL);
    parse_table_add(&pt, "Flags", PQ_INT,
                    (void *)( 0), &bw_flags, NULL);

    if (!parseEndOk(a, &pt, &retCode)) {
        return retCode;
    }

    if (BCM_PBMP_IS_NULL(pbmp)) {
        cli_out("%s ERROR: empty port bitmap\n", ARG_CMD(a));
        return CMD_FAIL;
    }

    BCM_PBMP_ITER(pbmp, p) {
        if ((r = bcm_cosq_gport_bandwidth_set(unit, p, cosq,
                                             kbits_sec_min,
                                             kbits_sec_max,
                                             bw_flags)) < 0) {
            cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(r));
            return CMD_FAIL;
        }
    }

    return CMD_OK;
}

static int
ltsw_cos_bandwidth_show(int unit, args_t *a, char *c)
{
    uint32 kbits_sec_min, kbits_sec_max, bw_flags;
    bcm_cos_queue_t cosq;
    bcm_gport_t gport;
    bcm_pbmp_t pbmp;
    bcm_port_config_t pcfg;
    bcm_port_t p;
    int numq;
    int r;
    bcm_gport_t port;
    uint32 flag;

    if (c != NULL) {
        return CMD_USAGE;
    }

    if (bcm_port_config_get(unit, &pcfg) != BCM_E_NONE) {
        cli_out("%s: Error: bcm ports not initialized\n", ARG_CMD(a));
        return CMD_FAIL;
    }

    BCM_PBMP_ASSIGN(pbmp, pcfg.port);
    cli_out("  COSQ bandwith configuration:\n");

    cli_out("    port | q | KbpsMin  | KbpsMax  | Flags\n");

    BCM_PBMP_ITER(pbmp, p) {
        cli_out("    -----+---+----------+----------+-------\n");

        BCM_GPORT_LOCAL_SET(gport, p);

        if ((r = bcm_cosq_gport_get(unit, gport, &port, &numq, &flag)) < 0) {
            cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(r));
            return CMD_FAIL;
        }

        for (cosq = 0; cosq < numq; cosq++) {
            if ((r = bcm_cosq_gport_bandwidth_get(unit, gport, cosq,
                       &kbits_sec_min, &kbits_sec_max, &bw_flags)) < 0) {
                cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(r));
                return CMD_FAIL;
            }
            cli_out("    %4s | %d | %8d | %8d | %6d\n",
                    bcmi_ltsw_port_name(unit, p), cosq, kbits_sec_min,
                    kbits_sec_max, bw_flags);
        }
        if ((r = bcm_cosq_gport_bandwidth_get(unit, gport, 8,
                   &kbits_sec_min, &kbits_sec_max, &bw_flags)) == 0) {
            cli_out("    %4s | %d | %8d | %8d | %6d\n",
                    bcmi_ltsw_port_name(unit, p), 8, kbits_sec_min,
                    kbits_sec_max, bw_flags);
        }

        /* Show VLAN cosq bandwidth for this port */
        (void) bcm_cosq_gport_traverse(unit, cosq_gport_show_bandwidth, &p);
    }
    return CMD_OK;
}

/*******************************************************************************
 * Public Functions
 */
cmd_result_t
cmd_ltsw_cos(int unit, args_t *a)
{
    char                *subcmd, *c;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if ((subcmd = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    c = ARG_CUR(a);

    if (sal_strcasecmp(subcmd, "clear") == 0) {
        return ltsw_cos_clear(unit, a, c);
    }

    if (sal_strcasecmp(subcmd, "strict") == 0) {
        return ltsw_cos_strict(unit, a, c);
    }

    if (sal_strcasecmp(subcmd, "simple") == 0) {
        return ltsw_cos_simple(unit, a, c);
    }

    if (sal_strcasecmp(subcmd, "port") == 0) {
        return ltsw_cos_port(unit, a, c);
    }

    if (sal_strcasecmp(subcmd, "show") == 0) {
        return ltsw_cos_show(unit, a, c);
    }

    if (sal_strcasecmp(subcmd, "map") == 0) {
        return ltsw_cos_map(unit, a, c);
    }

    if (sal_strcasecmp(subcmd, "weight") == 0 ||
        sal_strcasecmp(subcmd, "drr") == 0 ||
        sal_strcasecmp(subcmd, "fair") == 0) {
        return ltsw_cos_weight(unit, a, subcmd, c);
    }

    if (sal_strcasecmp(subcmd, "bounded") == 0 ||
        sal_strcasecmp(subcmd, "delay") == 0) {
        return ltsw_cos_bounded(unit, a, c);
    }

    if (sal_strcasecmp(subcmd, "discard") == 0) {
        return ltsw_cos_discard(unit, a, c);
    }

    if (sal_strcasecmp(subcmd, "discard_show") == 0) {
        return ltsw_cos_discard_show(unit, a,  c);
    }

    if (sal_strcasecmp(subcmd, "discard_gport") == 0) {
        return ltsw_cos_discard_gport(unit, a, c);
    }

    if (sal_strcasecmp(subcmd, "bandwidth") == 0) {
        return ltsw_cos_bandwidth(unit, a, c);
    }

    if (sal_strcasecmp(subcmd, "bandwidth_show") == 0) {
        return ltsw_cos_bandwidth_show(unit, a, c);
    }

    return CMD_USAGE;
}

#endif /* BCM_LTSW_SUPPORT */

