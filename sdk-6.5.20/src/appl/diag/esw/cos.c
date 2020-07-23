/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * COS CLI commands
 */

#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/dport.h>
#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm/cosq.h>
#include <bcm/debug.h>

#if defined(BCM_TOMAHAWK3_SUPPORT)
#define BCM_TH3_COS_COUNT 12
#endif

/*
 * Manage classes of service
 */

STATIC int
_bcm_gport_show_config(int unit, bcm_gport_t port, int numq,
                       uint32 flags, bcm_gport_t sched_gport, void *user_data)
{
    bcm_cos_queue_t cosq;
    bcm_port_t user_port = PTR_TO_INT(user_data);
    bcm_port_t local_port = 0;
    char *mode_name = NULL, *weight_str = "packets";
    int mode, weight, weight_max, rv;

    local_port = BCM_GPORT_MODPORT_PORT_GET(port);
    if ((NUM_MODID(unit) == 2) && (BCM_GPORT_MODPORT_MODID_GET(port) & 1)) {
        local_port += 32;
    }
    if (user_port != local_port) {
        return BCM_E_NONE;
    }
    for (cosq = 0; cosq < 16; cosq++) {
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

STATIC int
_bcm_gport_show_bandwidth(int unit, bcm_gport_t port, int numq,
                          uint32 flags, bcm_gport_t sched_gport, void *user_data)
{
    bcm_cos_queue_t cosq;
    bcm_port_t user_port = PTR_TO_INT(user_data);
    bcm_port_t local_port = 0;
    uint32 kbits_sec_min, kbits_sec_max, bw_flags;

    BCM_IF_ERROR_RETURN(
        bcm_port_local_get(unit, port, &local_port));

    if (user_port != local_port) {
        return BCM_E_NONE;
    }

    for (cosq = 0; cosq < 16; cosq++) {
        if (bcm_cosq_gport_bandwidth_get(unit, sched_gport, cosq,
                   &kbits_sec_min, &kbits_sec_max, &bw_flags) == 0) {
            cli_out("  U %4s | %d | %8d | %8d | %6d\n",
                    BCM_PORT_NAME(unit, local_port), cosq, kbits_sec_min,
                    kbits_sec_max, bw_flags);
        }
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_gport_show_discard(int unit, bcm_gport_t port, int numq,
                        uint32 flags, bcm_gport_t sched_gport, void *user_data)
{
    int rv = BCM_E_NONE;
    bcm_cos_queue_t cosq;
    bcm_port_t user_port = PTR_TO_INT(user_data);
    bcm_port_t local_port = 0;
    int discard_start, discard_slope, discard_time;

    BCM_IF_ERROR_RETURN(
        bcm_port_local_get(unit, port, &local_port));

    if (user_port != local_port) {
        return BCM_E_NONE;
    }

    for (cosq = 0; cosq < 16; cosq++) {
        if ((rv = bcm_cosq_discard_port_get(unit, sched_gport, cosq,
                BCM_COSQ_DISCARD_COLOR_GREEN,
                &discard_start, &discard_slope,
                &discard_time)) < 0) {
            continue;
        }
        cli_out("  U %4s | %d | %6d | %2d | %5d ",
                BCM_PORT_NAME(unit, local_port), cosq, discard_start,
                discard_slope, discard_time);
        if ((rv = bcm_cosq_discard_port_get(unit, sched_gport, cosq,
                BCM_COSQ_DISCARD_COLOR_YELLOW,
                &discard_start, &discard_slope,
                &discard_time)) < 0) {
            cli_out("|  ----  | -- | ----- ");
        } else {
            cli_out("| %6d | %2d | %5d ",
                    discard_start, discard_slope, discard_time);
        }
        if ((rv = bcm_cosq_discard_port_get(unit, sched_gport, cosq,
                BCM_COSQ_DISCARD_COLOR_RED,
                &discard_start, &discard_slope,
                &discard_time)) < 0) {
            return rv;
        }
        cli_out("| %6d | %2d | %5d\n",
                discard_start, discard_slope, discard_time);
    }
    return BCM_E_NONE;
}
                             
cmd_result_t
cmd_esw_cos(int unit, args_t *a)
{
    int			r;
    char		*subcmd, *c;
    bcm_cos_queue_t	cosq;
    int*		weights = NULL;
#if defined(BCM_TOMAHAWK3_SUPPORT)
    int                 weight_arr_size;
#endif
    int			delay = 0;
    int			numq, mode = 0;
    int      num_weights;
    bcm_cos_t		prio;
    bcm_cos_t	lastPrio = 0;
    parse_table_t	pt;
    cmd_result_t	retCode;
    int                 discard_ena, discard_cap_avg, gain, drop_prob;
    int                 discard_start, discard_end, discard_slope, discard_time;
    char *              discard_color;
    char*              discard_mode;
    bcm_pbmp_t          pbmp;
    bcm_port_config_t   pcfg;
    soc_port_t          p, dport;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if ((subcmd = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    if ((r = bcm_cosq_config_get(unit, &numq)) < 0) {
        return CMD_FAIL;
    }

    num_weights = BCM_COS_COUNT;
    if (num_weights < numq) {
        num_weights = numq;
    }

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(unit)) {
        weights = (int *) sal_alloc(
            sizeof(int) * BCM_TH3_COS_COUNT, "weights array");
        sal_memset(weights, 0, BCM_TH3_COS_COUNT * sizeof(int));
    } else
#endif
    {
        weights = (int *) sal_alloc(
            sizeof(int) * num_weights, "weights array");
        sal_memset(weights, 0, num_weights * sizeof(int));
    }

    c = ARG_CUR(a);
    if ((c) == NULL) {
        if (sal_strcasecmp(subcmd, "clear") == 0) {
	    if ((r = bcm_cosq_init(unit)) < 0) {
	        goto bcm_err;
	    }
            if (weights != NULL) {
                sal_free(weights);
                weights = NULL;
            }
	    return CMD_OK;
    	}

	if (sal_strcasecmp(subcmd, "strict") == 0) {
            if (SOC_IS_TOMAHAWK3(unit)) {
#if defined(BCM_TOMAHAWK3_SUPPORT)
                if ((r = bcm_cosq_sched_config_set(unit, BCM_COSQ_STRICT,
					BCM_TH3_COS_COUNT, weights)) < 0)
                {
		    goto bcm_err;
                }
#endif
            } else {
                if ((r = bcm_cosq_sched_set(unit, BCM_COSQ_STRICT,
					weights, 0)) < 0)
                {
		    goto bcm_err;
	        }
            }
            if (weights != NULL) {
                sal_free(weights);
                weights = NULL;
            }
	    return CMD_OK;
	}

	if (sal_strcasecmp(subcmd, "simple") == 0) {
            if (SOC_IS_TOMAHAWK3(unit)) {
#if defined(BCM_TOMAHAWK3_SUPPORT)
                if ((r = bcm_cosq_sched_config_set(unit, BCM_COSQ_ROUND_ROBIN,
					BCM_TH3_COS_COUNT, weights)) < 0)
                {
		    goto bcm_err;
                }
#endif
            } else {
                if ((r = bcm_cosq_sched_set(unit, BCM_COSQ_ROUND_ROBIN,
					weights, 0)) < 0)
                {
		    goto bcm_err;
	        }
            }
            if (weights != NULL) {
                sal_free(weights);
                weights = NULL;
            }
	    return CMD_OK;
	}

    }

    if (SOC_IS_TOMAHAWK3(unit)) {
#if defined(BCM_TOMAHAWK3_SUPPORT)
        r = bcm_cosq_sched_config_get(unit, BCM_TH3_COS_COUNT, weights, &weight_arr_size, &mode);
#endif
    } else {
        r = bcm_cosq_sched_get(unit, &mode, weights, &delay);
    }
    if (r == BCM_E_UNAVAIL) {
	r = 0;
	mode = -1;
    } else if (r < 0) {
	goto bcm_err;
    }

    if (bcm_port_config_get(unit, &pcfg) != BCM_E_NONE) {
        cli_out("%s: Error: bcm ports not initialized\n", ARG_CMD(a));
        if (weights != NULL) {
            sal_free(weights);
            weights = NULL;
        }
        return CMD_FAIL;
    }

    if (sal_strcasecmp(subcmd, "port") == 0) {
        int num_vlanq = 0;
        bcm_gport_t gport, sched_gport = BCM_GPORT_INVALID;

        if ((subcmd = ARG_GET(a)) == NULL) {
            if (weights != NULL) {
                sal_free(weights);
                weights = NULL;
            }
            return CMD_USAGE;
        }
        if (sal_strcasecmp(subcmd, "config") == 0) {
            BCM_PBMP_CLEAR(pbmp);
            parse_table_init(unit, &pt);
	    parse_table_add(&pt, "Numcos", 	PQ_DFL|PQ_INT,
			    (void *)( 0), &num_vlanq, NULL);
            parse_table_add(&pt, "PortBitMap", PQ_DFL|PQ_PBMP|PQ_BCM,
                            (void *)(0), &pbmp, NULL);
        
	    if (!parseEndOk( a, &pt, &retCode)) {
                if (weights != NULL) {
                    sal_free(weights);
                    weights = NULL;
                }
	        return retCode;
	    }
            if (BCM_PBMP_IS_NULL(pbmp)) {
                cli_out("%s: ERROR: must specify valid port bitmap.\n",
                        ARG_CMD(a));
                if (weights != NULL) {
                    sal_free(weights);
                    weights = NULL;
                }
                return CMD_FAIL;
            }

            DPORT_BCM_PBMP_ITER(unit, pbmp, dport, p) {
                BCM_GPORT_LOCAL_SET(gport, p);
                r = bcm_cosq_gport_add(unit, gport, num_vlanq, 
                                       BCM_COSQ_GPORT_SCHEDULER, &sched_gport);
                if (r < 0) {
                    cli_out("%s: bcm_cosq_gport_add ERROR: port %s: %s\n",
                            ARG_CMD(a),
                            BCM_PORT_NAME(unit, p),
                            bcm_errmsg(r));
                    if (weights != NULL) {
                        sal_free(weights);
                        weights = NULL;
                    }
                    return CMD_FAIL;
                }
                r = bcm_cosq_gport_attach(unit, sched_gport, gport, 8);
                if (r < 0) {
                    cli_out("%s: bcm_cosq_gport_attach ERROR: port %s: %s\n",
                            ARG_CMD(a),
                            BCM_PORT_NAME(unit, p),
                            bcm_errmsg(r));
                    if (weights != NULL) {
                        sal_free(weights);
                        weights = NULL;
                    }
                    return CMD_FAIL;
                } else {
                    cli_out(" Added/Attached GPORT %d (0x%08x) to port %s.\n",
                            sched_gport, sched_gport, BCM_PORT_NAME(unit, p));
                }
            }
            if (weights != NULL) {
                sal_free(weights);
                weights = NULL;
            }
	    return CMD_OK;
        }
        if (sal_strcasecmp(subcmd, "show") == 0) {
            char *mode_name, *weight_str = "packets";
            int weight_max;
            bcm_pbmp_t temp_pbmp;

            BCM_PBMP_CLEAR(pbmp);
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "PortBitMap", PQ_DFL|PQ_PBMP|PQ_BCM,
                            (void *)(0), &pbmp, NULL);

            if (!parseEndOk( a, &pt, &retCode)) {
                if (c != NULL) {
                    if (weights != NULL) {
                        sal_free(weights);
                        weights = NULL;
                    }
                    return retCode;
                }
            }
            if (BCM_PBMP_IS_NULL(pbmp)) {
                cli_out("%s: ERROR: must specify valid port bitmap.\n",
                        ARG_CMD(a));
                if (weights != NULL) {
                    sal_free(weights);
                    weights = NULL;
                }
                return CMD_FAIL;
            }
            DPORT_BCM_PBMP_ITER(unit, pbmp, dport, p) {
                BCM_PBMP_CLEAR(temp_pbmp);
                SOC_PBMP_PORT_ADD(temp_pbmp, p);

                if (SOC_IS_TOMAHAWK3(unit)) {
#if defined(BCM_TOMAHAWK3_SUPPORT)
                    r = bcm_cosq_port_sched_config_get(unit, temp_pbmp, BCM_TH3_COS_COUNT, weights, &weight_arr_size, &mode);
#endif
                } else {
                    r = bcm_cosq_port_sched_get(unit, temp_pbmp, &mode, weights, &delay);
                }
                if (r < 0) {
                    goto bcm_err;
                }
                cli_out("\n  Port %s COS configuration:\n", BCM_PORT_NAME(unit, p));
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
                    goto bcm_err;
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
                    BCM_GPORT_LOCAL_SET(gport, p);
                    if (bcm_cosq_gport_sched_get(unit, gport, 8, &mode, &weights[0]) == 0) {
                        cli_out("    COSQ 8 = %d %s (queue for output of S1 scheduler)\n",
                                weights[0], weight_str);
                    }
                }
                cli_out("  Priority to queue mappings:\n");

                for (prio = 0; prio < 8; prio++) {
                    if ((r = bcm_cosq_port_mapping_get(unit, p, prio, &cosq)) < 0) {
                        goto bcm_err;
                    }
                    cli_out("    PRIO %d ==> COSQ %d\n", prio, cosq);
                }

                /* Only newer XGS3 chips support more than 8 priorities */
                for (prio = 8; prio < 16; prio++) {
                    if (bcm_cosq_port_mapping_get(unit, p, prio, &cosq) < 0) {
                        break;
                    }
                    cli_out("    PRIO %d ==> COSQ %d\n", prio, cosq);
                }

                if (mode == BCM_COSQ_BOUNDED_DELAY) {
                    cli_out("  Bounded delay: %d usec\n", delay);
                }

                /* Show VLAN cosq config for this port */
                (void) bcm_cosq_gport_traverse(unit, _bcm_gport_show_config, INT_TO_PTR(p));
            }
            if (weights != NULL) {
                sal_free(weights);
                weights = NULL;
            }
            return CMD_OK;
        }
        if (sal_strcasecmp(subcmd, "map") == 0) {
            BCM_PBMP_CLEAR(pbmp);
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "PortBitMap", PQ_DFL|PQ_PBMP|PQ_BCM,
                            (void *)(0), &pbmp, NULL);
            parse_table_add(&pt, "Pri",     PQ_DFL|PQ_INT,
                            (void *)(0), &lastPrio, NULL);
            parse_table_add(&pt, "Queue",   PQ_DFL|PQ_INT,
                            (void *)(0), &cosq, NULL);

            if (!parseEndOk( a, &pt, &retCode)) {
                if (c != NULL) {
                    if (weights != NULL) {
                        sal_free(weights);
                        weights = NULL;
                    }
                    return retCode;
                }
            }
            if (BCM_PBMP_IS_NULL(pbmp)) {
                cli_out("%s: ERROR: must specify valid port bitmap.\n",
                        ARG_CMD(a));
                if (weights != NULL) {
                    sal_free(weights);
                    weights = NULL;
                }
                return CMD_FAIL;
            } else {
                DPORT_BCM_PBMP_ITER(unit, pbmp, dport, p) {
                    r = bcm_cosq_port_mapping_set(unit, p, lastPrio, cosq);
                    if (r < 0) {
                        cli_out("%s: ERROR: port %s: %s\n",
                                ARG_CMD(a),
                                BCM_PORT_NAME(unit, p),
                                bcm_errmsg(r));
                        if (weights != NULL) {
                            sal_free(weights);
                            weights = NULL;
                        }
                        return CMD_FAIL;
                    }
                }
            }
    
            if (weights != NULL) {
                sal_free(weights);
                weights = NULL;
            }
            return CMD_OK;
        }
        if (sal_strcasecmp(subcmd, "weight") == 0 ||
            sal_strcasecmp(subcmd, "drr") == 0 ||
            sal_strcasecmp(subcmd, "bounded") == 0 ||
            sal_strcasecmp(subcmd, "delay") == 0 ||
            sal_strcasecmp(subcmd, "strict") == 0 ||
            sal_strcasecmp(subcmd, "simple") == 0 ||
            sal_strcasecmp(subcmd, "fair") == 0) {

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
    
            if (!parseEndOk( a, &pt, &retCode)) {
                if (weights != NULL) {
                    sal_free(weights);
                    weights = NULL;
                }
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
                    if (weights != NULL) {
                        sal_free(weights);
                        weights = NULL;
                    }
                    return CMD_FAIL;
                } else {
                    DPORT_BCM_PBMP_ITER(unit, pbmp, dport, p) {
                        BCM_GPORT_LOCAL_SET(gport, p);
                        if ((r = bcm_cosq_gport_sched_set(unit, gport, cosq, mode, weight)) < 0) {
                            goto bcm_err;
                        }
                    }
                }
            } else {
                if ((r = bcm_cosq_gport_sched_set(unit, sched_gport, cosq, mode, weight)) < 0) {
                    goto bcm_err;
                }
            }
            if (weights != NULL) {
                sal_free(weights);
                weights = NULL;
            }
            return CMD_OK;
        }
        if (sal_strcasecmp(subcmd, "bandwidth") == 0) {
            uint32 kbits_sec_min, kbits_sec_max, bw_flags;
    
            BCM_PBMP_CLEAR(pbmp);
            discard_color = NULL;
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
    
            if (!parseEndOk( a, &pt, &retCode)) {
                if (weights != NULL) {
                    sal_free(weights);
                    weights = NULL;
                }
                return retCode;
            }
    
            if (BCM_PBMP_IS_NULL(pbmp)) {
                cli_out("%s ERROR: empty port bitmap\n", ARG_CMD(a));
                if (weights != NULL) {
                    sal_free(weights);
                    weights = NULL;
                }
                return CMD_FAIL;
            }
    
            if (sched_gport == BCM_GPORT_INVALID) {
                DPORT_BCM_PBMP_ITER(unit, pbmp, dport, p) {
                    BCM_GPORT_LOCAL_SET(gport, p);
                    if ((r = bcm_cosq_gport_bandwidth_set(unit, gport, cosq,
                                                          kbits_sec_min,
                                                          kbits_sec_max,
                                                          bw_flags)) < 0) {
                        goto bcm_err; 
                    }
                }
            } else {
                if ((r = bcm_cosq_gport_bandwidth_set(unit, sched_gport, cosq,
                                                      kbits_sec_min,
                                                      kbits_sec_max,
                                                      bw_flags)) < 0) {
                    goto bcm_err;
                }
            }
    
            if (weights != NULL) {
                sal_free(weights);
                weights = NULL;
            }
            return CMD_OK;
        }
        if (weights != NULL) {
            sal_free(weights);
            weights = NULL;
        }
        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "show") == 0) {
	char		*mode_name;
	char		*weight_str = "packets";
        int             weight_max;

        if (c != NULL) {
            if (weights != NULL) {
                sal_free(weights);
                weights = NULL;
            }
	    return CMD_USAGE;
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
            goto bcm_err;
        }

	cli_out("COS configuration:\n");
	cli_out("  Config (max queues): %d\n", numq);
	if (mode_name) {
	    cli_out("  Schedule mode: %s\n", mode_name);
	}

	if (mode >= 0 && mode != BCM_COSQ_STRICT) {
	    cli_out("  Weighting (in %s):\n", weight_str);

	    for (cosq = 0; cosq < NUM_COS(unit); cosq++) {
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
		goto bcm_err;
	    }
	    cli_out("    PRIO %d ==> COSQ %d\n", prio, cosq);
	}

        /* Only newer XGS3 chips support more than 8 priorities */
	for (prio = 8; prio < 16; prio++) {
	    if (bcm_cosq_mapping_get(unit, prio, &cosq) < 0) {
		break;
	    }
	    cli_out("    PRIO %d ==> COSQ %d\n", prio, cosq);
	}

	if (mode == BCM_COSQ_BOUNDED_DELAY) {
	    cli_out("  Bounded delay: %d usec\n", delay);
	}

        if (weights != NULL) {
            sal_free(weights);
            weights = NULL;
        }
	return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "config") == 0) {
        parse_table_init(unit, &pt);
	parse_table_add(&pt, "Numcos", 	PQ_DFL|PQ_INT,
			(void *)( 0), &numq, NULL);

	if (!parseEndOk( a, &pt, &retCode)) {
            if (weights != NULL) {
                sal_free(weights);
                weights = NULL;
            }
	    return retCode;
	}

	if ((r = bcm_cosq_config_set(unit, numq)) < 0) {
	    goto bcm_err;
	}

        if (weights != NULL) {
            sal_free(weights);
            weights = NULL;
        }
	return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "map") == 0) {
        if ((r = bcm_cosq_mapping_get(unit, lastPrio, &cosq)) < 0) {
	    goto bcm_err;
	}

        BCM_PBMP_CLEAR(pbmp);
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "PortBitMap", PQ_DFL|PQ_PBMP|PQ_BCM,
                        (void *)(0), &pbmp, NULL);
	parse_table_add(&pt, "Pri", 	PQ_DFL|PQ_INT,
			(void *)(0), &lastPrio, NULL);
	parse_table_add(&pt, "Queue", 	PQ_DFL|PQ_INT,
			(void *)(0), &cosq, NULL);

	if (!parseEndOk( a, &pt, &retCode)) {
            if (weights != NULL) {
                sal_free(weights);
                weights = NULL;
            }
	    return retCode;
	}

	if (BCM_PBMP_IS_NULL(pbmp)) {
	    if ((r = bcm_cosq_mapping_set(unit, lastPrio, cosq)) < 0) {
		goto bcm_err;
	    }
	} else {
        /* coverity[overrun-local : FALSE] */
        DPORT_BCM_PBMP_ITER(unit, pbmp, dport, p) {
		r = bcm_cosq_port_mapping_set(unit, p, lastPrio, cosq);
		if (r < 0) {
		    cli_out("%s: ERROR: port %s: %s\n",
                            ARG_CMD(a),
                            BCM_PORT_NAME(unit, p),
                            bcm_errmsg(r));
                    if (weights != NULL) {
                        sal_free(weights);
                        weights = NULL;
                    }
		    return CMD_FAIL;
		}
	    }
	}

        if (weights != NULL) {
            sal_free(weights);
            weights = NULL;
        }
	return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "weight") == 0 ||
	sal_strcasecmp(subcmd, "drr") == 0 ||
	sal_strcasecmp(subcmd, "fair") == 0) {
        parse_table_init(unit, &pt);
	parse_table_add(&pt, "W0", 	PQ_DFL|PQ_INT,
			(void *)( 0), &weights[0], NULL);
	parse_table_add(&pt, "W1", 	PQ_DFL|PQ_INT,
			(void *)( 0), &weights[1], NULL);
	parse_table_add(&pt, "W2", 	PQ_DFL|PQ_INT,
			(void *)( 0), &weights[2], NULL);
	parse_table_add(&pt, "W3", 	PQ_DFL|PQ_INT,
			(void *)( 0), &weights[3], NULL);
	parse_table_add(&pt, "W4", 	PQ_DFL|PQ_INT,
			(void *)( 0), &weights[4], NULL);
	parse_table_add(&pt, "W5", 	PQ_DFL|PQ_INT,
			(void *)( 0), &weights[5], NULL);
	parse_table_add(&pt, "W6", 	PQ_DFL|PQ_INT,
			(void *)( 0), &weights[6], NULL);
	parse_table_add(&pt, "W7", 	PQ_DFL|PQ_INT,
			(void *)( 0), &weights[7], NULL);
        if (SOC_IS_TOMAHAWK3(unit)) {
#if defined(BCM_TOMAHAWK3_SUPPORT)
	    parse_table_add(&pt, "W8", 	PQ_DFL|PQ_INT,
			(void *)( 0), &weights[8], NULL);
	    parse_table_add(&pt, "W9", 	PQ_DFL|PQ_INT,
			(void *)( 0), &weights[9], NULL);
	    parse_table_add(&pt, "W10", PQ_DFL|PQ_INT,
			(void *)( 0), &weights[10], NULL);
	    parse_table_add(&pt, "W11", PQ_DFL|PQ_INT,
			(void *)( 0), &weights[11], NULL);
#endif
        }

	if (!parseEndOk( a, &pt, &retCode)) {
            if (weights != NULL) {
                sal_free(weights);
                weights = NULL;
            }
	    return retCode;
	}

	if (sal_strcasecmp(subcmd, "fair") == 0) {
	    mode = BCM_COSQ_WEIGHTED_FAIR_QUEUING;
        } else if (sal_strcasecmp(subcmd, "drr") == 0) {
	    mode = BCM_COSQ_DEFICIT_ROUND_ROBIN;
	} else {
	    mode = BCM_COSQ_WEIGHTED_ROUND_ROBIN;
	}
        if (SOC_IS_TOMAHAWK3(unit)) {
#if defined(BCM_TOMAHAWK3_SUPPORT)
	    if ((r = bcm_cosq_sched_config_set(unit, mode, BCM_TH3_COS_COUNT, weights)) < 0) {
	        goto bcm_err;
	    }
#endif
        } else {
	    if ((r = bcm_cosq_sched_set(unit, mode, weights, 0)) < 0) {
	        goto bcm_err;
            }
        }

        if (weights != NULL) {
            sal_free(weights);
            weights = NULL;
        }
	return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "bounded") == 0 ||
	sal_strcasecmp(subcmd, "delay") == 0) {

        parse_table_init(unit, &pt);
	parse_table_add(&pt, "W0", 	PQ_DFL|PQ_INT,
			(void *)( 0), &weights[0], NULL);
	parse_table_add(&pt, "W1", 	PQ_DFL|PQ_INT,
			(void *)( 0), &weights[1], NULL);
	parse_table_add(&pt, "W2", 	PQ_DFL|PQ_INT,
			(void *)( 0), &weights[2], NULL);
	parse_table_add(&pt, "W3", 	PQ_DFL|PQ_INT,
			(void *)( 0), &weights[3], NULL);
	parse_table_add(&pt, "W4", 	PQ_DFL|PQ_INT,
			(void *)( 0), &weights[4], NULL);
	parse_table_add(&pt, "W5", 	PQ_DFL|PQ_INT,
			(void *)( 0), &weights[5], NULL);
	parse_table_add(&pt, "W6", 	PQ_DFL|PQ_INT,
			(void *)( 0), &weights[6], NULL);
	parse_table_add(&pt, "W7", 	PQ_DFL|PQ_INT,
			(void *)( 0), &weights[7], NULL);
        if (SOC_IS_TOMAHAWK3(unit)) {
#if defined(BCM_TOMAHAWK3_SUPPORT)
	    parse_table_add(&pt, "W8", 	PQ_DFL|PQ_INT,
			(void *)( 0), &weights[8], NULL);
	    parse_table_add(&pt, "W9", 	PQ_DFL|PQ_INT,
			(void *)( 0), &weights[9], NULL);
	    parse_table_add(&pt, "W10",	PQ_DFL|PQ_INT,
			(void *)( 0), &weights[10], NULL);
	    parse_table_add(&pt, "W11",	PQ_DFL|PQ_INT,
			(void *)( 0), &weights[11], NULL);
#endif
        }
	parse_table_add(&pt, "Delay", 	PQ_DFL|PQ_INT,
			(void *)( 0), &delay, NULL);

	if (!parseEndOk( a, &pt, &retCode)) {
            if (weights != NULL) {
                sal_free(weights);
                weights = NULL;
            }
	    return retCode;
	}

        if (SOC_IS_TOMAHAWK3(unit)) {
#if defined(BCM_TOMAHAWK3_SUPPORT)
	    if ((r = bcm_cosq_sched_config_set(unit, BCM_COSQ_BOUNDED_DELAY,
                                   BCM_TH3_COS_COUNT, weights)) < 0) {
	        goto bcm_err;
	    }
#endif
        } else {
	    if ((r = bcm_cosq_sched_set(unit,
				    BCM_COSQ_BOUNDED_DELAY,
				    weights, delay)) < 0) {
	        goto bcm_err;
	    }
        }

        if (weights != NULL) {
            sal_free(weights);
            weights = NULL;
        }
	return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "discard") == 0) {
        uint32 discard_flags = 0;

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Enable", PQ_BOOL,
                        0, &discard_ena, NULL);
        parse_table_add(&pt, "CapAvg", PQ_BOOL,
                        0, &discard_cap_avg, NULL);

	if (!parseEndOk( a, &pt, &retCode)) {
            if (weights != NULL) {
                sal_free(weights);
                weights = NULL;
            }
	    return retCode;
	}

        if (discard_ena) {
            discard_flags |= BCM_COSQ_DISCARD_ENABLE;
        }
        if (discard_cap_avg) {
            discard_flags |= BCM_COSQ_DISCARD_CAP_AVERAGE;
        }

	if ((r = bcm_cosq_discard_set(unit, discard_flags)) < 0) {
	    goto bcm_err;
	}

        if (weights != NULL) {
            sal_free(weights);
            weights = NULL;
        }
	return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "discard_port") == 0) {
        uint32 color_flags = 0;
        bcm_gport_t sched_gport;

        BCM_PBMP_CLEAR(pbmp);
        discard_color = NULL;
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "PortBitMap", PQ_DFL|PQ_PBMP|PQ_BCM,
                        (void *)(0), &pbmp, NULL);
	parse_table_add(&pt, "Queue", PQ_INT,
			(void *)( 0), &cosq, NULL);
        parse_table_add(&pt, "Color", PQ_STRING, 
                        "green", &discard_color, NULL);
	parse_table_add(&pt, "DropSTart", PQ_INT,
			(void *)( 0), &discard_start, NULL);
	parse_table_add(&pt, "DropSLope", PQ_INT,
			(void *)( 0), &discard_slope, NULL);
	parse_table_add(&pt, "AvgTime", PQ_INT,
			(void *)( 0), &discard_time, NULL);
        parse_table_add(&pt, "Gport", PQ_INT, 
                        (void *)BCM_GPORT_INVALID,
                        &sched_gport, 0);

        /* Parse remaining arguments */
        if (0 > parse_arg_eq(a, &pt)) {
            cli_out("%s: Error: Invalid option or malformed expression: %s\n",
                    ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            if (weights != NULL) {
                sal_free(weights);
                weights = NULL;
            }
            return(CMD_FAIL);
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
                if (weights != NULL) {
                    sal_free(weights);
                    weights = NULL;
                }
                return CMD_FAIL;
            }
        } else {
            color_flags = BCM_COSQ_DISCARD_COLOR_GREEN;
        }

        parse_arg_eq_done(&pt);

        if (sched_gport == BCM_GPORT_INVALID) {
            if (BCM_PBMP_IS_NULL(pbmp)) {
                cli_out("%s ERROR: empty port bitmap\n", ARG_CMD(a));
                if (weights != NULL) {
                    sal_free(weights);
                    weights = NULL;
                }
                return CMD_FAIL;
            }

            DPORT_BCM_PBMP_ITER(unit, pbmp, dport, p) {
	        if ((r = bcm_cosq_discard_port_set(unit, p, cosq, color_flags,
                                                   discard_start, discard_slope,
                                                   discard_time)) < 0) {
	            goto bcm_err;
	        }
            }
        } else {
	    if ((r = bcm_cosq_discard_port_set(unit, sched_gport, cosq, color_flags,
                                               discard_start, discard_slope,
                                               discard_time)) < 0) {
	        goto bcm_err;
	    }
        }

        if (weights != NULL) {
            sal_free(weights);
            weights = NULL;
        }
	return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "discard_show") == 0) {
        uint32          discard_flags = 0;
        bcm_gport_t     gport = 0;
        bcm_cos_queue_t cosq = 0;
        bcm_cosq_gport_discard_t discard;

        if (ARG_CNT(a)) {
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "Gport", PQ_INT, (void *)( 0), &gport, 0);
	    parse_table_add(&pt, "Queue", PQ_INT, (void *)( 0), &cosq, NULL);

            if (!parseEndOk( a, &pt, &retCode)) {
                if (weights != NULL) {
                    sal_free(weights);
                    weights = NULL;
                }
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
                discard.flags = BCM_COSQ_DISCARD_COLOR_GREEN | BCM_COSQ_DISCARD_PACKETS;
                if (bcm_cosq_gport_discard_get(unit, gport, cosq, &discard) == 0) {
                    cli_out("\n    GREEN  ( pkts) : Min %d, Max %d, Gain %d, DropProb %d, Ena %d, Cap %d\n",
                            discard.min_thresh, discard.max_thresh, discard.gain,
                            discard.drop_probability,
                            (discard.flags & BCM_COSQ_DISCARD_ENABLE) ? 1 : 0,
                            (discard.flags & BCM_COSQ_DISCARD_CAP_AVERAGE) ? 1 : 0);
                }
                discard.flags = BCM_COSQ_DISCARD_COLOR_YELLOW | BCM_COSQ_DISCARD_PACKETS;
                if (bcm_cosq_gport_discard_get(unit, gport, cosq, &discard) == 0) {
                    cli_out("    YELLOW ( pkts) : Min %d, Max %d, Gain %d, DropProb %d, Ena %d, Cap %d\n",
                            discard.min_thresh, discard.max_thresh, discard.gain,
                            discard.drop_probability,
                            (discard.flags & BCM_COSQ_DISCARD_ENABLE) ? 1 : 0,
                            (discard.flags & BCM_COSQ_DISCARD_CAP_AVERAGE) ? 1 : 0);
                }
                discard.flags = BCM_COSQ_DISCARD_COLOR_RED | BCM_COSQ_DISCARD_PACKETS;
                if (bcm_cosq_gport_discard_get(unit, gport, cosq, &discard) == 0) {
                    cli_out("    RED    ( pkts) : Min %d, Max %d, Gain %d, DropProb %d, Ena %d, Cap %d\n",
                            discard.min_thresh, discard.max_thresh, discard.gain,
                            discard.drop_probability,
                            (discard.flags & BCM_COSQ_DISCARD_ENABLE) ? 1 : 0,
                            (discard.flags & BCM_COSQ_DISCARD_CAP_AVERAGE) ? 1 : 0);
                }
                if (weights != NULL) {
                    sal_free(weights);
                    weights = NULL;
                }
                return CMD_OK;
            }
        }

        if ((r = bcm_cosq_discard_get(unit, &discard_flags)) < 0) {
            if (r != BCM_E_UNAVAIL) {
                goto bcm_err;
            }
        }

        cli_out("  Discard (WRED): discard %s, cap-average %s\n",
                (discard_flags & BCM_COSQ_DISCARD_ENABLE) ? 
                "enabled" : "disabled",
                (discard_flags & BCM_COSQ_DISCARD_CAP_AVERAGE) ? 
                "enabled" : "disabled");

        BCM_PBMP_ASSIGN(pbmp, pcfg.port);
        cli_out("  Discard (WRED) port configuration:\n");
        cli_out("         |   | gr     | gr | gr    | ye     |"
                " ye | ye    | re     | re | re\n");
        cli_out("    port | q | strt   | sl | time  | strt   |"
                " sl | time  | strt   | sl | time\n");
        DPORT_BCM_PBMP_ITER(unit, pbmp, dport, p) {
            cli_out("    -----+---+--------+----+-------+--------+"
                    "----+-------+--------+----+-------\n");
            for (cosq = 0; cosq < numq; cosq++) {
                if ((r = bcm_cosq_discard_port_get(unit, p, cosq, 
                        BCM_COSQ_DISCARD_COLOR_GREEN,
                        &discard_start, &discard_slope,
                        &discard_time)) < 0) {
                    goto bcm_err;
                }
                cli_out("    %4s | %d | %6d | %2d | %5d ",
                        BCM_PORT_NAME(unit, p), cosq, discard_start, 
                        discard_slope, discard_time);
                if ((r = bcm_cosq_discard_port_get(unit, p, cosq, 
                        BCM_COSQ_DISCARD_COLOR_YELLOW,
                        &discard_start, &discard_slope,
                        &discard_time)) < 0) {
                    cli_out("|  ----  | -- | ----- ");
                } else {
                    cli_out("| %6d | %2d | %5d ", 
                            discard_start, discard_slope, discard_time);
                }
                if ((r = bcm_cosq_discard_port_get(unit, p, cosq, 
                        BCM_COSQ_DISCARD_COLOR_RED,
                        &discard_start, &discard_slope,
                        &discard_time)) < 0) {
                    goto bcm_err;
                }
                cli_out("| %6d | %2d | %5d\n", 
                        discard_start, discard_slope, discard_time);
            }

            /* Show VLAN cosq discard settings for this port */
            (void) bcm_cosq_gport_traverse(unit, _bcm_gport_show_discard, INT_TO_PTR(p));
        }

        if (weights != NULL) {
            sal_free(weights);
            weights = NULL;
        }
        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "discard_gport") == 0) {
        uint32 color_flags = 0;
        uint32 mode_flags = 0;
        bcm_gport_t sched_gport;
        bcm_cosq_gport_discard_t discard;

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
            if (weights != NULL) {
                sal_free(weights);
                weights = NULL;
            }
            return(CMD_FAIL);
        }

        if (discard_mode != NULL) {
            if (!strncmp(discard_mode, "b", 1)) {
                mode_flags = BCM_COSQ_DISCARD_BYTES;
            } else if (!strncmp(discard_mode, "p", 1)) {
                mode_flags = BCM_COSQ_DISCARD_PACKETS;
            } else {
                cli_out("%s ERROR: Invalid mode value\n", ARG_CMD(a));
                parse_arg_eq_done(&pt);
                if (weights != NULL) {
                    sal_free(weights);
                    weights = NULL;
                }
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
                if (weights != NULL) {
                    sal_free(weights);
                    weights = NULL;
                }
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
	    goto bcm_err;
	}

        if (weights != NULL) {
            sal_free(weights);
            weights = NULL;
        }
	return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "bandwidth") == 0) {
        uint32 kbits_sec_min, kbits_sec_max, bw_flags;

        BCM_PBMP_CLEAR(pbmp);
        discard_color = NULL;
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

        if (!parseEndOk( a, &pt, &retCode)) {
            if (weights != NULL) {
                sal_free(weights);
                weights = NULL;
            }
            return retCode;
        }

        if (BCM_PBMP_IS_NULL(pbmp)) {
            cli_out("%s ERROR: empty port bitmap\n", ARG_CMD(a));
            if (weights != NULL) {
                sal_free(weights);
                weights = NULL;
            }
            return CMD_FAIL;
        }

        DPORT_BCM_PBMP_ITER(unit, pbmp, dport, p) {
	    if ((r = bcm_cosq_port_bandwidth_set(unit, p, cosq, 
                                                 kbits_sec_min,
                                                 kbits_sec_max,
                                                 bw_flags)) < 0) {
	        goto bcm_err;
	    }
        }

        if (weights != NULL) {
            sal_free(weights);
            weights = NULL;
        }
	return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "bandwidth_show") == 0) {
        uint32 kbits_sec_min, kbits_sec_max, bw_flags;
        bcm_gport_t gport;

        if (c != NULL) {
            if (weights != NULL) {
                sal_free(weights);
                weights = NULL;
            }
	    return CMD_USAGE;
	}

        BCM_PBMP_ASSIGN(pbmp, pcfg.port);
        cli_out("  COSQ bandwith configuration:\n");

        cli_out("    port | q | KbpsMin  | KbpsMax  | Flags\n");

        DPORT_BCM_PBMP_ITER(unit, pbmp, dport, p) {
            cli_out("    -----+---+----------+----------+-------\n");
            for (cosq = 0; cosq < numq; cosq++) {
                if ((r = bcm_cosq_port_bandwidth_get(unit, p, cosq, 
                           &kbits_sec_min, &kbits_sec_max, &bw_flags)) < 0) {
                    goto bcm_err;
                }
                cli_out("    %4s | %d | %8d | %8d | %6d\n",
                        BCM_PORT_NAME(unit, p), cosq, kbits_sec_min,
                        kbits_sec_max, bw_flags);
            }
            BCM_GPORT_LOCAL_SET(gport, p);
            if ((r = bcm_cosq_gport_bandwidth_get(unit, gport, 8,
                       &kbits_sec_min, &kbits_sec_max, &bw_flags)) == 0) {
                cli_out("    %4s | %d | %8d | %8d | %6d\n",
                        BCM_PORT_NAME(unit, p), 8, kbits_sec_min,
                        kbits_sec_max, bw_flags);
            }

            /* Show VLAN cosq bandwidth for this port */
            (void) bcm_cosq_gport_traverse(unit, _bcm_gport_show_bandwidth, INT_TO_PTR(p));
        }
        if (weights != NULL) {
            sal_free(weights);
            weights = NULL;
        }
	return CMD_OK;
    }

    if (weights != NULL) {
        sal_free(weights);
        weights = NULL;
    }
    return CMD_USAGE;

 bcm_err:
    cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(r));
    if (weights != NULL) {
        sal_free(weights);
        weights = NULL;
    }
    return CMD_FAIL;
}
