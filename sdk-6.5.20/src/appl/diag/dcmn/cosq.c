/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * COSQ CLI commands
 */

#include <shared/bsl.h>

#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/dport.h>

#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/types.h>
#include <bcm/cosq.h>

#include <shared/bsl.h>

#ifdef BCM_PETRA_SUPPORT
#include <soc/dpp/mbcm.h>

#include <bcm_int/dpp/alloc_mngr_cosq.h>
#define DIAG_DCMN_COSQ_COMPENSTATION_DEFAULT_ID 0xfffffff

char cmd_dpp_cosq_usage[] =
    "Usages:\n\t"
    "  cosq comp ing voq=<id>                         - show ingress compensation\n\t"
    "  cosq comp egr port=<id>                        - show egress compensation\n\t"
    "  cosq comp ing [voq=<id>] Compensation=<value>  - set ingress compensation\n\t"
    "  cosq comp egr [port=<id>] Compensation=<value> - set egress compensation\n\t"
    "  cosq comp egr [port=<id>] cast=<UC/MC>         - set egress unicast/multicast\n\t"
    "  cosq flush enable port=<id>                    - flush port queues\n\t"
    "  cosq flush disable port=<id>                   - flush port queues\n\n\t"

    "  cosq conn ing                                  - show ingress connection\n\t"
    "  cosq conn egr                                  - show egress connection\n"
    ;




int cosq_packet_length_adjust_cb (
    int unit, 
    bcm_gport_t port, 
    int numq, 
    uint32 flags, 
    bcm_gport_t gport, 
    void *user_data)
{
     int rv;
     int* ingress_compensation_ptr = user_data;
     if(BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
         rv = bcm_cosq_control_set(unit, gport, 0, bcmCosqControlPacketLengthAdjust, *ingress_compensation_ptr); 
         if(rv<0) {
             return rv;
         }
     }
     
     return 0;
}

int dpp_cosq_connection_cb(
    int unit,
    bcm_gport_t port,
    int numq,
    uint32 flags,
    bcm_gport_t gport,
    void *user_data)
{
    int rv = BCM_E_NONE;
    int core, i;
    bcm_cosq_gport_connection_t *gport_connect = (bcm_cosq_gport_connection_t *)user_data;
    bcm_gport_t voq, voq_connector, voq_connector_i;
    int is_ingress, is_egress;
    uint32 nof_remote_cores, region;

    is_ingress = (gport_connect->flags & BCM_COSQ_GPORT_CONNECTION_INGRESS) ? TRUE : FALSE;
    is_egress = (gport_connect->flags & BCM_COSQ_GPORT_CONNECTION_EGRESS) ? TRUE : FALSE;

    if (is_ingress && BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        gport_connect->voq = gport;
    } else if (is_egress && BCM_COSQ_GPORT_IS_VOQ_CONNECTOR(gport)) {
        gport_connect->voq_connector = gport;
    } else {
        return rv;
    }

    rv = bcm_cosq_gport_connection_get(unit, gport_connect);
    if (BCM_FAILURE(rv)) {
        cli_out("\nbcm_cosq_gport_connection_get failed(%d) gport(0x%8.8x)!\n", rv, gport);
        return rv;
    }

    voq = BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(gport_connect->voq);
    voq_connector = BCM_COSQ_GPORT_VOQ_CONNECTOR_ID_GET(gport_connect->voq_connector);

    if (is_ingress) {
        core = BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_GET(gport);
        cli_out("        %5d      |        %d       | %3d  |         %5d         |       %d \n", voq, numq, core, voq_connector, gport_connect->remote_modid);
        if(SOC_DPP_CORE_MODE_IS_SYMMETRIC(unit)){
            BCM_DPP_CORES_ITER(BCM_CORE_ALL, core){
                if(core == 0) continue;
                BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(gport,core,voq);
                gport_connect->voq = gport;
                rv = bcm_cosq_gport_connection_get(unit, gport_connect);
                if (BCM_FAILURE(rv)) {
                    cli_out("\nbcm_cosq_gport_connection_get failed(%d) gport(0x%8.8x)!\n", rv, gport);
                    return rv;
                }
                voq_connector = BCM_COSQ_GPORT_VOQ_CONNECTOR_ID_GET(gport_connect->voq_connector);
                cli_out("        %5d      |        %d       | %3d  |         %5d         |       %d \n", voq, numq, core, voq_connector, gport_connect->remote_modid);
            }
        }
    } else { /* is_egress */
        core = BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_GET(gport);
        if (core < 0) {
            cli_out("\nGot illegal core (%d) for voq connector %d", core, voq_connector);
            return BCM_E_FAIL;
        }
        if (SOC_IS_QAX(unit)) {
           voq_connector = BCM_COSQ_FLOW_ID_QAX_ADD_OFFSET(unit, voq_connector);
        }
        region = _BCM_DPP_AM_COSQ_GET_REGION_INDEX_FROM_FLOW_INDEX(voq_connector);
        nof_remote_cores = SOC_DPP_CONFIG(unit)->arad->region_nof_remote_cores[core][region];

        for (i = 0; i < nof_remote_cores; i++) { 
            voq_connector_i = _BCM_DPP_AM_COSQ_GET_FLOW_INDEX_FROM_REMOTE_CORE_INDEX(voq_connector, nof_remote_cores, i);
            if (SOC_IS_QAX(unit)) {
               voq_connector_i = BCM_COSQ_FLOW_ID_QAX_SUB_OFFSET(unit, voq_connector_i);
            }
            cli_out("        %5d      |        %d       | %3d  |         %5d         |       %d \n", voq_connector_i, numq, core, voq, (gport_connect->remote_modid + i));
        }
    }

    return rv;
}

cmd_result_t
cmd_dpp_cosq(int unit, args_t *a)
{
    char                *subcmd;
    uint32              voq, port;
    int                 comp;
    int                 temp_comp;
    parse_table_t       pt;
    int                 rv = BCM_E_NONE;
    int                 rc;
    bcm_gport_t         gport[2] = {BCM_GPORT_INVALID};
    bcm_port_config_t   config;
    int                 cast;
    char                *cast_list[] = {"ALL", "UC", "MC"};
    int                 start = 0, count = 2, gport_start = 0;
    int                 i;
    uint32              is_port_valid;
    bcm_cosq_gport_connection_t cb_params;

    rc=0;
    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if ((subcmd = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    /* sub compensation command */
    if (sal_strcasecmp(subcmd, "comp") == 0) {

        if ((subcmd = ARG_GET(a)) == NULL) {
            return CMD_USAGE;
        }

        /* parse values */
        parse_table_init(unit, &pt);
        voq = DIAG_DCMN_COSQ_COMPENSTATION_DEFAULT_ID;
        parse_table_add(&pt, "voq", PQ_DFL | PQ_INT, &voq, &voq, NULL);
        port = DIAG_DCMN_COSQ_COMPENSTATION_DEFAULT_ID;
        parse_table_add(&pt, "port", PQ_DFL | PQ_INT, &port, &port, NULL);
        comp = DIAG_DCMN_COSQ_COMPENSTATION_DEFAULT_ID;
        parse_table_add(&pt, "Compensation", PQ_DFL | PQ_INT, &comp, &comp, NULL);
        cast = 0;
        parse_table_add(&pt, "cast", PQ_DFL | PQ_MULTI, &cast, &cast, cast_list);

        if (parse_arg_eq(a, &pt) < 0 ) {
            cli_out("%s: ERROR: Unknown option: %s\n",
                    ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }
        parse_arg_eq_done(&pt);

        if (port != DIAG_DCMN_COSQ_COMPENSTATION_DEFAULT_ID) {
            rc = soc_port_sw_db_is_valid_port_get(unit, port, &is_port_valid);
            if (!is_port_valid) {
                cli_out("Port %d is invalid\n", port);
                return CMD_FAIL;
            }
        }


        /* ingress compenstation */
        if (sal_strcasecmp(subcmd, "ing") == 0) 
        {
            /* no voq specified, set all voqs */
            if (voq == DIAG_DCMN_COSQ_COMPENSTATION_DEFAULT_ID) {
                rv = bcm_cosq_gport_traverse(unit, cosq_packet_length_adjust_cb, &comp);
                if (rv != BCM_E_NONE) {
                    return CMD_FAIL;
                    }
            } else {
                /* specific voq */
                 BCM_GPORT_UNICAST_QUEUE_GROUP_SET(gport[0], voq);
                 /* get */
                 if (comp == DIAG_DCMN_COSQ_COMPENSTATION_DEFAULT_ID) {
                     rv = bcm_cosq_control_get(unit, gport[0], 0, bcmCosqControlPacketLengthAdjust, &comp); 
                     if(rv<0) {
                         return CMD_FAIL;
                     }
                  cli_out("Voq %d compensation is %d\n", voq, comp);
                 } else {
                     /* set */
                     rv = bcm_cosq_control_set(unit, gport[0], 0, bcmCosqControlPacketLengthAdjust, comp);
                     if(rv<0) {
                         return CMD_FAIL;
                     }
                   
                }
            }
        }

        /* egress compensation */
        if (sal_strcasecmp(subcmd, "egr") == 0) {

            if (cast == 0) {
                /* configure both gports (UC and MC) */
                start = 0;
                count = 2;
            } else if (cast == 1) {
                /* configure only UC gport (index 0 in gport array) */
                start = 0;
                count = 1;
            } else if (cast == 2) {
                /* configure only MC gport (index 1 in gport array) */
                start = 1;
                count = 1;
            } else {
                cli_out("ERROR: Unknown cast option\n");
                return CMD_FAIL;
            }

            /* no port specified, set all ports */
            if (port == DIAG_DCMN_COSQ_COMPENSTATION_DEFAULT_ID) {
                rv = bcm_port_config_get(unit, &config);
                if (rv != BCM_E_NONE) {
                    cli_out("failed to get port bmps in cint_compensation_set");
                    return CMD_FAIL;
                }

                BCM_PBMP_ITER(config.all, port) {
                    if(!BCM_PBMP_MEMBER(config.sfi, port)) {
                        BCM_COSQ_GPORT_UCAST_EGRESS_QUEUE_SET(gport[0], port); 
                        BCM_COSQ_GPORT_MCAST_EGRESS_QUEUE_SET(gport[1], port); 
                        
                        for (i = 0, gport_start = start; i < count; ++i, ++gport_start) {
                            rv = bcm_cosq_control_set(unit, gport[gport_start], 0, bcmCosqControlPacketLengthAdjust, comp);
                            if(rv<0) {
                                return CMD_FAIL;
                            }
                        }
                    }
                }

            } else {
                /* specific port */
                BCM_COSQ_GPORT_UCAST_EGRESS_QUEUE_SET(gport[0], port); 
                BCM_COSQ_GPORT_MCAST_EGRESS_QUEUE_SET(gport[1], port); 
                
                for (i = 0; i < count; ++i, ++start) {
                    /* get */
                    if (comp == DIAG_DCMN_COSQ_COMPENSTATION_DEFAULT_ID) {
                        rv = bcm_cosq_control_get(unit, gport[start], 0, bcmCosqControlPacketLengthAdjust, &comp); 
                        if(rv<0) {
                            return CMD_FAIL;
                        }
                        cli_out("Port %d compensation is %d\n", port, comp);
                        cli_out("Port %d cast is %s\n", port, cast_list[cast]);
                        comp = DIAG_DCMN_COSQ_COMPENSTATION_DEFAULT_ID;
                    } else {
                        /* set */
                        rv = bcm_cosq_control_get(unit, gport[start], 0, bcmCosqControlPacketLengthAdjust, &temp_comp); 
                        if(rv<0) {
                            return CMD_FAIL;
                        }
                        
                        /* set only in case we have different compenstation */
                        if(temp_comp != comp)
                        {
                            rv = bcm_cosq_control_set(unit, gport[start], 0, bcmCosqControlPacketLengthAdjust, comp);
                            if(rv<0) {
                                return CMD_FAIL;
                            }
                        }
                    }
                }
            }

        }
    } else if(sal_strcasecmp(subcmd, "flush") == 0) {

            if ((subcmd = ARG_GET(a)) == NULL) {
                return CMD_USAGE;
            }

            /* parse values */
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "port", PQ_DFL | PQ_INT, 0, &port, NULL);

            if (parse_arg_eq(a, &pt) < 0 ) {
                cli_out("%s: ERROR: Unknown option: %s\n",
                        ARG_CMD(a), ARG_CUR(a));
                parse_arg_eq_done(&pt);
                return CMD_FAIL;
            }
            parse_arg_eq_done(&pt);

            /* Enable flush */
            if (sal_strcasecmp(subcmd, "Enable") == 0) 
            {
                rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_action_cmd_cosq_flush,(unit,port,TRUE)));
                if (SOC_SAND_FAILURE(rc)) {
                    cli_out("\nFailed to enable port\n\n");
                    return CMD_FAIL;
                } 
            }

            /* Disable flush  */
            if (sal_strcasecmp(subcmd, "Disable") == 0) 
            { 
                rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_action_cmd_cosq_flush,(unit,port,FALSE)));
                if (SOC_SAND_FAILURE(rc)) {
                    cli_out("\nFailed to disable port\n\n");
                    return CMD_FAIL;
                } 
            }

    } else {
        /* ingress/egress connection */
        if (!sal_strcasecmp(subcmd, "conn")) {

            if ((subcmd = ARG_GET(a)) == NULL) {
                return CMD_USAGE;
            }

            sal_memset(&cb_params, 0, sizeof(bcm_cosq_gport_connection_t));

            if(!sal_strcasecmp(subcmd,"ing")) {
                cb_params.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS;
                cli_out("      Voq ID       |    NOF Voqs    | Core |  Remote Voq Connector | Remote Modid \n");
                cli_out("--------------------------------------------------------------------------------- \n");
            } else if (!sal_strcasecmp(subcmd, "egr")) {
                cb_params.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS;
                cli_out(" Voq Connector ID  | NOF Connectors | Core |      Ingress Voq      | Ingress Modid \n");
                cli_out("---------------------------------------------------------------------------------- \n");
            } else {
                return CMD_USAGE;
            }

            rv = bcm_cosq_gport_traverse(unit,
                                         dpp_cosq_connection_cb,
                                         &cb_params);
            if (rv != BCM_E_NONE) {
                return CMD_FAIL;
            }

        }
    }

    return CMD_OK;
}

#endif /*BCM_PETRA_SUPPORT*/
