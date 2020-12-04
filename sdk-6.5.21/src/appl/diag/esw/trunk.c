/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Trunk CLI commands
 */

#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/dport.h>
#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm/trunk.h>
#include <bcm/port.h>
#include <bcm/stack.h>
#include <bcm/debug.h>



cmd_result_t
if_esw_trunk(int unit, args_t *a)
{
    char 		*subcmd;
    bcm_trunk_t		tid = -1;
    bcm_pbmp_t 		pbmp;
    int 		r;
    int			psc;
    bcm_pbmp_t 	arg_pbmp;
    uint32       arg_rtag=0;
    uint32	arg_vlan=VLAN_ID_DEFAULT, arg_tid=0;
#ifdef BCM_FIREBOLT_SUPPORT
    int          arg_hash=0;
#endif /* BCM_FIREBOLT_SUPPORT */
    parse_table_t	pt;
    cmd_result_t	retCode;
    bcm_trunk_info_t    t_add_info;
    int			i, j, dport;
    sal_mac_addr_t	mac;
    vlan_id_t		vid;
    bcm_module_t    modid;
    

    if (!sh_check_attached(ARG_CMD(a), unit)) {
	return CMD_FAIL;
    }

    if ((subcmd = ARG_GET(a)) == NULL) {
	return CMD_USAGE;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(arg_pbmp);

    if (sal_strcasecmp(subcmd, "init") == 0) {
        r = bcm_trunk_init(unit);
        if (r < 0) {
            cli_out("%s: trunk init failed: %s\n", ARG_CMD(a), bcm_errmsg(r));
            return CMD_FAIL;
        }

	return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "deinit") == 0) {
        r = bcm_trunk_detach(unit);
        if (r < 0) {
            cli_out("%s: trunk deinit failed: %s\n", ARG_CMD(a), bcm_errmsg(r));
            return CMD_FAIL;
        }

        return CMD_OK;
    }

    /* trunk show command */
    if (sal_strcasecmp(subcmd, "show") == 0) {
        return _bcm_diag_trunk_show(unit, a);
    }

    r = bcm_stk_my_modid_get(unit, &modid);
    if ((r < 0) && (r != BCM_E_UNAVAIL)) {
        cli_out("Could not get mod id %d: %s\n", r, bcm_errmsg(r));
        return r;
    }


    if (r < 0) {  /* If unavailable, indicate unknown */
        modid = -1;
    }

    if (sal_strcasecmp(subcmd, "add") == 0) {
	bcm_trunk_chip_info_t	ti;
        bcm_module_t tm, _tm;
        bcm_port_t   tp, _tp;
        bcm_trunk_member_t *member_array = NULL;

	if ((r = bcm_trunk_chip_info_get(unit, &ti)) < 0) {
	    cli_out("%s: %s\n", ARG_CMD(a), bcm_errmsg(r));
	    return CMD_FAIL;
	}

	parse_table_init(unit, &pt);
        parse_table_add(&pt, "Id",   PQ_DFL|PQ_INT,  0, &arg_tid,  NULL);
        parse_table_add(&pt, "Rtag", PQ_DFL|PQ_INT,  0, &arg_rtag, NULL);
	parse_table_add(&pt, "Pbmp", PQ_DFL|PQ_PBMP|PQ_BCM, 0, &arg_pbmp, NULL);

	if (!parseEndOk(a, &pt, &retCode)) {
	    return retCode;
        }

	tid = arg_tid;
        psc = arg_rtag;
	pbmp = arg_pbmp;

        member_array = sal_alloc(sizeof(bcm_trunk_member_t) *
                BCM_TRUNK_MAX_PORTCNT, "member array");
        if (NULL == member_array) {
            cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(BCM_E_MEMORY));
            return CMD_FAIL;
        }
	j = 0;
        /* coverity[overrun-local] */
        DPORT_BCM_PBMP_ITER(unit, pbmp, dport, i) {
            tm = (soc_feature(unit, soc_feature_mod1) && i > 31) ? modid + 1 : modid;
            tp = i;
            _tm = tm;
            _tp = tp;
            r = bcm_stk_modmap_map(unit, BCM_STK_MODMAP_GET, tm, tp,
                    &tm, &tp);
            if (r == BCM_E_UNAVAIL) {
                tm = _tm;
                tp = _tp;
            } else if (r < 0) {
                sal_free(member_array);
                return CMD_FAIL;
            }
            if (j >= BCM_TRUNK_MAX_PORTCNT) {
                sal_free(member_array);
                return CMD_FAIL;
            }
            bcm_trunk_member_t_init(&member_array[j]);
            if (BCM_GPORT_IS_SET(tp)) {
                member_array[j].gport = tp;
            } else {
                BCM_GPORT_MODPORT_SET(member_array[j].gport, tm, tp);
            }
            j += 1;
        };
        bcm_trunk_info_t_init(&t_add_info);
	t_add_info.dlf_index = BCM_TRUNK_UNSPEC_INDEX;
	t_add_info.mc_index = BCM_TRUNK_UNSPEC_INDEX;
	t_add_info.ipmc_index = BCM_TRUNK_UNSPEC_INDEX;
        t_add_info.psc = psc;

        bcm_trunk_create(unit, BCM_TRUNK_FLAG_WITH_ID, &tid);
	r = bcm_trunk_set(unit, tid, &t_add_info, j, member_array);

        if (r < 0) {
            bcm_trunk_destroy(unit, tid);
        }
        sal_free(member_array);
    } else if (sal_strcasecmp(subcmd, "remove") == 0) {
        bcm_module_t tm, _tm;
        bcm_port_t   tp, _tp;
        bcm_trunk_member_t *member_array = NULL;
        int member_count;

	parse_table_init(unit, &pt);
        parse_table_add(&pt, "Id", PQ_DFL|PQ_INT, 0,
			&arg_tid, NULL);
	parse_table_add(&pt, "Pbmp", PQ_DFL|PQ_PBMP|PQ_BCM, (void *)(0),
			&arg_pbmp, NULL);
	if (!parseEndOk(a, &pt, &retCode)) {
	    return retCode;
        }

        pbmp = arg_pbmp;
	tid = arg_tid;

        member_array = sal_alloc(sizeof(bcm_trunk_member_t) *
                BCM_TRUNK_MAX_PORTCNT, "member array");
        if (NULL == member_array) {
            cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(BCM_E_MEMORY));
            return CMD_FAIL;
        }
	r = bcm_trunk_get(unit, tid, &t_add_info, BCM_TRUNK_MAX_PORTCNT,
                member_array, &member_count);

	if (r >= 0) {
            BCM_PBMP_CLEAR(pbmp);
            r = bcm_stk_my_modid_get(unit, &modid);
            if (r < 0) {
                sal_free(member_array);
                return CMD_FAIL;
            }
            for (i = 0; i < member_count; i++) {
                if (BCM_GPORT_IS_DEVPORT(member_array[i].gport)) {
                    tm = modid;
                    tp = BCM_GPORT_DEVPORT_PORT_GET(member_array[i].gport);
                } else if (BCM_GPORT_IS_MODPORT(member_array[i].gport)) {
                    tm = BCM_GPORT_MODPORT_MODID_GET(member_array[i].gport);
                    tp = BCM_GPORT_MODPORT_PORT_GET(member_array[i].gport);
                } else {
                    sal_free(member_array);
                    return CMD_FAIL;
                }
                if ((tm != modid) && 
                    !soc_feature(unit, soc_feature_mod1))  {
                    tp +=32;
                }
	        BCM_PBMP_PORT_ADD(pbmp, tp);
            }
	    BCM_PBMP_REMOVE(pbmp, arg_pbmp);

	    j = 0;
            DPORT_BCM_PBMP_ITER(unit, pbmp, dport, i) {
                tm = (soc_feature(unit, soc_feature_mod1) && i > 31) ? modid + 1: modid;
                tp = i;
                _tm = tm;
                _tp = tp;
                r = bcm_stk_modmap_map(unit, BCM_STK_MODMAP_GET, tm, tp, 
                        &tm, &tp); 
                if (r == BCM_E_UNAVAIL) {
                    tm = _tm;
                    tp = _tp;
                } else if (r < 0) {
                    sal_free(member_array);
                    return CMD_FAIL;
                }
                bcm_trunk_member_t_init(&member_array[j]);
                if (BCM_GPORT_IS_SET(tp)) {
                    member_array[j].gport = tp;
                } else {
                    BCM_GPORT_MODPORT_SET(member_array[j].gport, tm, tp);
                }
                j += 1;
            }

	    if (j) {
                r = bcm_trunk_set(unit, tid, &t_add_info, j, member_array);
	    } else {
		r = bcm_trunk_destroy(unit, tid);
            }
	}

        sal_free(member_array);
    } else if (sal_strcasecmp(subcmd, "egress") == 0) {
	parse_table_init(unit, &pt);
        parse_table_add(&pt, "Id", PQ_DFL|PQ_INT, 0, &arg_tid, NULL);
	parse_table_add(&pt, "Pbmp", PQ_DFL|PQ_PBMP|PQ_BCM, (void *)(0),
                        &arg_pbmp, NULL);
        if (!parseEndOk( a, &pt, &retCode)) return retCode;
	tid = arg_tid;
        BCM_PBMP_ASSIGN(pbmp, arg_pbmp);
	r = bcm_trunk_egress_set(unit, tid, pbmp);
    } else if (sal_strcasecmp(subcmd, "mcast") == 0) {
	parse_table_init(unit, &pt);
        parse_table_add(&pt, "Id", PQ_DFL|PQ_INT, 0, &arg_tid, NULL);
	parse_table_add(&pt, "Mac", PQ_DFL|PQ_MAC, 0, (void *)mac, NULL);
	parse_table_add(&pt, "Vlanid", 	PQ_DFL|PQ_HEX, 0, &arg_vlan, NULL);
	if (!parseEndOk( a, &pt, &retCode)) {
	    return retCode;
	}

	tid = arg_tid;
	vid = arg_vlan;
	r = bcm_trunk_mcast_join(unit, tid, vid, mac);
#ifdef BCM_FIREBOLT_SUPPORT
    } else if (sal_strcasecmp(subcmd, "hash") == 0) {
        char *subcmd_s;
        bcm_port_t port, dport;

        if ((subcmd_s = ARG_GET(a)) == NULL) {
            return CMD_USAGE;
        }

        if (sal_strcasecmp(subcmd_s, "set") == 0) {
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "Pbmp", PQ_DFL|PQ_PBMP|PQ_BCM, (void *)(0),
                            &arg_pbmp, NULL);
            parse_table_add(&pt, "HashValue", PQ_DFL|PQ_INT, 0, &arg_hash, NULL);
            if (!parseEndOk( a, &pt, &retCode)) {
                return retCode;
            }

            /* coverity[overrun-local] */
            DPORT_BCM_PBMP_ITER(unit, arg_pbmp, dport, port) {
               r = bcm_port_trunk_index_set(unit, port, arg_hash);
            }
        } else if (sal_strcasecmp(subcmd_s, "get") == 0) {
            int hash;

            parse_table_init(unit, &pt);
            parse_table_add(&pt, "Pbmp", PQ_DFL|PQ_PBMP|PQ_BCM, (void *)(0),
                            &arg_pbmp, NULL);
            if (!parseEndOk( a, &pt, &retCode)) {
                return retCode;
            }
            DPORT_BCM_PBMP_ITER(unit, arg_pbmp, dport, port) {
               r = bcm_port_trunk_index_get(unit, port, &hash);
	       if (r >= 0) {
                   cli_out("port %s programmable hash set to %d\n",
                           BCM_PORT_NAME(unit, port), hash);
               }
            }
        } else {
            return CMD_USAGE;
        }
#endif /* BCM_FIREBOLT_SUPPORT */
    } else if (sal_strcasecmp(subcmd, "psc") == 0) {
	parse_table_init(unit, &pt);
        parse_table_add(&pt, "Id", PQ_DFL|PQ_INT, 0, &arg_tid, NULL);
        parse_table_add(&pt, "Rtag", PQ_DFL|PQ_INT, 0, &arg_rtag, NULL);
	if (!parseEndOk(a, &pt, &retCode)) {
	    return retCode;
        }

	tid = arg_tid;
        psc = arg_rtag;

	r = bcm_trunk_psc_set(unit, tid, psc);
    } else {
	return CMD_USAGE;
    }

    if (r < 0) {
	cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(r));
	return CMD_FAIL;
    }

    return CMD_OK;
}

#ifdef BCM_HERCULES15_SUPPORT
char trunkpool_cmd_usage[] =
    "\n\t"
    "trunkpool set [Id=<val>] [PortBitMap=<pbmp>] [Size=<val>]\n\t"
    "              [W0=<val>] [W1=<val>] ... [W7=<val>]\n\t"
    "           - Set trunk pool information\n\t"
    "trunkpool get [Id=<val>] [PortBitMap=<pbmp>]\n\t"
    "           - Get trunk pool information\n\t"
    "Note: Need to configure trunk before setting pool table.\n";

cmd_result_t
cmd_trunkpool(int unit, args_t *a)
{
    char           *subcmd;
    parse_table_t  pt;
    cmd_result_t   retCode;
    bcm_pbmp_t pbmp;
    bcm_port_config_t pcfg;
    int     arg_psize=-1, arg_tid=0;
    int     weights[BCM_TRUNK_MAX_PORTCNT];
    bcm_trunk_info_t add_info;
    int            count[BCM_TRUNK_MAX_PORTCNT];
    int            i, port, dport, r, size;
    bcm_trunk_member_t *member_array = NULL;
    int            member_count;
    bcm_port_t     local_port;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if ((subcmd = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    if (bcm_port_config_get(unit, &pcfg) != BCM_E_NONE) {
        cli_out("%s: Error: bcm ports not initialized\n", ARG_CMD(a));
        return CMD_FAIL;
    }

    BCM_PBMP_CLEAR(pbmp);
    sal_memset(weights, 0, BCM_TRUNK_MAX_PORTCNT * sizeof(int));

    if (sal_strcasecmp(subcmd, "set") == 0) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "PortBitMap", PQ_DFL|PQ_PBMP|PQ_BCM,
                        (void *)( 0), &pbmp, NULL);
        parse_table_add(&pt, "Id",   PQ_DFL|PQ_INT,
                        (void *)( 0), &arg_tid,  NULL);
        parse_table_add(&pt, "Size", PQ_DFL|PQ_INT,
                        (void *)( 0), &arg_psize, NULL);
        parse_table_add(&pt, "W0",      PQ_DFL|PQ_INT,
                        (void *)( 0), &weights[0], NULL);
        parse_table_add(&pt, "W1",      PQ_DFL|PQ_INT,
                        (void *)( 0), &weights[1], NULL);
        parse_table_add(&pt, "W2",      PQ_DFL|PQ_INT,
                        (void *)( 0), &weights[2], NULL);
        parse_table_add(&pt, "W3",      PQ_DFL|PQ_INT,
                        (void *)( 0), &weights[3], NULL);
        parse_table_add(&pt, "W4",      PQ_DFL|PQ_INT,
                        (void *)( 0), &weights[4], NULL);
        parse_table_add(&pt, "W5",      PQ_DFL|PQ_INT,
                        (void *)( 0), &weights[5], NULL);
        parse_table_add(&pt, "W6",      PQ_DFL|PQ_INT,
                        (void *)( 0), &weights[6], NULL);
        parse_table_add(&pt, "W7",      PQ_DFL|PQ_INT,
                        (void *)( 0), &weights[7], NULL);

        if (!parseEndOk(a, &pt, &retCode))
            return retCode;

        if (arg_psize < 0) {
            r = bcm_trunk_get(unit, arg_tid, &add_info,
                    0, NULL, &member_count);
            if (r >= 0) {
                arg_psize = member_count;
            }
        }

        member_array = sal_alloc(sizeof(bcm_trunk_member_t) *
                BCM_TRUNK_MAX_PORTCNT, "member array");
        if (NULL == member_array) {
            cli_out("ERROR: %s %s failed: %s\n",
                    ARG_CMD(a), subcmd, bcm_errmsg(BCM_E_MEMORY));
            return CMD_FAIL;
        }
        BCM_PBMP_AND(pbmp, pcfg.stack_ext);
        /* coverity[overrun-local] */
        DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
            r = bcm_trunk_pool_set(unit, port, arg_tid, arg_psize, weights);
            if (r < 0) {
                cli_out("ERROR: %s %s port %s failed: %s\n",
                        ARG_CMD(a), subcmd, BCM_PORT_NAME(unit, port),
                        bcm_errmsg(r));
                sal_free(member_array);
                return CMD_FAIL;
            }
            cli_out("port %s trunk pool set:\n", BCM_PORT_NAME(unit, port));
            cli_out("     tid=%d   size=%d\n",arg_tid, arg_psize);
            r = bcm_trunk_get(unit, arg_tid, &add_info,
                    BCM_TRUNK_MAX_PORTCNT, member_array, &member_count);
            if (r >= 0) {
                cli_out("     ");
                for (i = 0; i < member_count; i++) {
                    r = bcm_port_local_get(unit, member_array[i].gport,
                            &local_port);
                    if (r >= 0) {
                        if (weights[local_port-1])
                            cli_out("W[%d]=%d ", local_port - 1,
                                    weights[local_port - 1]);
                    }
                }
                cli_out("\n");
            }
        }
        sal_free(member_array);
        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "get") == 0) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "PortBitMap", PQ_DFL|PQ_PBMP|PQ_BCM,
                        (void *)( 0), &pbmp, NULL);
        parse_table_add(&pt, "Id",   PQ_DFL|PQ_INT,
                        (void *)( 0), &arg_tid,  NULL);
        if (!parseEndOk(a, &pt, &retCode))
            return retCode;

        member_array = sal_alloc(sizeof(bcm_trunk_member_t) *
                BCM_TRUNK_MAX_PORTCNT, "member array");
        if (NULL == member_array) {
            cli_out("ERROR: %s %s failed: %s\n",
                    ARG_CMD(a), subcmd, bcm_errmsg(BCM_E_MEMORY));
            return CMD_FAIL;
        }

        BCM_PBMP_AND(pbmp, pcfg.stack_ext);
        DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
            r = bcm_trunk_pool_get(unit, port, arg_tid, &size, count);
            if (r < 0) {
                cli_out("ERROR: %s %s port %s failed: %s\n",
                        ARG_CMD(a), subcmd, BCM_PORT_NAME(unit, port),
                        bcm_errmsg(r));
                sal_free(member_array);
                return CMD_FAIL;
            }
            cli_out("port %s trunk pool get:\n", BCM_PORT_NAME(unit, port));
            cli_out("     tid=%d   size=%d\n",arg_tid, size);
            r = bcm_trunk_get(unit, arg_tid, &add_info,
                    BCM_TRUNK_MAX_PORTCNT, member_array, &member_count);
            if (r >= 0) {
                cli_out("     ");
                for (i = 0; i < member_count; i++) {
                    r = bcm_port_local_get(unit, member_array[i].gport,
                            &local_port);
                    if (r >= 0) {
                        if (count[local_port-1])
                            cli_out("TotalCount[%d]=%d ", local_port - 1,
                                    count[local_port - 1]);
                    }
                }
                cli_out("\n");
            }
        }
        sal_free(member_array);
        return CMD_OK;
    }

    return CMD_USAGE;
}
#endif /* BCM_HERCULES15_SUPPORT */
