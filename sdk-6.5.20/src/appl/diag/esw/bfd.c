/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#if defined(INCLUDE_BFD)

#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm/bfd.h>
#include <bcm_int/esw/bfd.h>
#include <bcm_int/esw/port.h>
#if defined(BCM_ENDURO_SUPPORT)
#include <bcm/rx.h>
#include <soc/higig.h>
#endif

#define CLEAN_UP_AND_RETURN(_result) \
    parse_arg_eq_done(&parse_table); \
    return (_result);

#define _isprintable(_c) (((_c) > 32) && ((_c) < 127))

#define ENDPOINT_LIST_HEADER \
    "ID  Type  VRF  GPORT V6  IPADDR  DISCRIMINATOR  CpuPri  IntPri \n" \
    "--  ----  ---  ----- --  ------  -------------  ------  ------ \n"

char cmd_esw_bfd_usage[] = 
#ifdef COMPILER_STRING_CONST_LIMIT
    "Usage: bfd <option> [args...]\n"
#else
    "\n"
    "  bfd init\n"
    "  bfd endpoint add [GPORT=<id>] [TYPE=<type>] [VRF=<id>] [INTPRI=<intpri>]\n"
    "                   [CPUPRI=<qid>] [DISCRIMINATOR=<id>] [V6] [IP=<val>]\n"
    "  bfd endpoint update [ID=<id>] [TYPE=<type>] [VRF=<id>] [INTPRI=<intpri>]\n"
    "                       [CPUPRI=<qid>] [DISCRIMINATOR=<id>] [V6] [IP=<val>]\n"
    "  bfd endpoint delete [ID=<id>]\n"
    "  bfd endpoint show [ID=<id>] \n"
#if defined(BCM_KATANA_SUPPORT)
    "  bfd tx ID=<endpointid> \n"
#endif
    "  bfd trace \n"
#endif
    ;

cmd_result_t cmd_esw_bfd(int unit, args_t *args)
{
    char *arg_string_p = NULL;
    parse_table_t parse_table;
    bcm_bfd_endpoint_info_t endpoint_info;
    int int_pri;
    int result;
    int gport, type, vrf,qid, disc_id, v6, src_ip, dst_ip, label;
    
    arg_string_p = ARG_GET(args);

    if (arg_string_p == NULL)
    {
        return CMD_USAGE;
    }

    if (!sh_check_attached(ARG_CMD(args), unit))
    {
        return CMD_FAIL;
    }

    if (sal_strcasecmp(arg_string_p, "init") == 0)
    {
        result = bcm_bfd_init(unit);

        if (BCM_FAILURE(result))
        {
            cli_out("Command failed.  %s.\n", bcm_errmsg(result));

            return CMD_FAIL;
        }

        cli_out("BFD module initialized.\n");
    }
    
#if defined(BCM_KATANA_SUPPORT)
    else if (sal_strcasecmp(arg_string_p, "tx") == 0)
    {    
#if 0
        parse_table_init(unit, &parse_table);

        parse_table_add(&parse_table, "ID", PQ_INT,
            (void *) BCM_OAM_GROUP_INVALID, &endpoint_info.id, NULL);

        parse_table_add(&parse_table, "DMAC", PQ_MAC | PQ_DFL, 0,
                &mac_dst, NULL);

        parse_table_add(&parse_table, "SMAC", PQ_MAC | PQ_DFL, 0,
                &mac_src, NULL);

        if (parse_arg_eq(args, &parse_table) < 0 || ARG_CNT(args) > 0)
        {
            cli_out("Invalid option: %s\n", ARG_CUR(args));

            CLEAN_UP_AND_RETURN(CMD_FAIL);
        }

        result = bcm_bfd_endpoint_get(unit, endpoint_info.id, &endpoint_info);
        
        if (BCM_FAILURE(result))
        {
            cli_out("Command failed.  %s.\n", bcm_errmsg(result));
        
            CLEAN_UP_AND_RETURN(CMD_FAIL);
        }

        parse_arg_eq_done(&parse_table);
#endif
    }
#endif /* BCM_KATANA_SUPPORT */
    else if (sal_strcasecmp(arg_string_p, "endpoint") == 0)
    {
        arg_string_p = ARG_GET(args);

        bcm_bfd_endpoint_info_t_init(&endpoint_info);

        if (arg_string_p == NULL ||
            sal_strcasecmp(arg_string_p, "show") == 0)
        {
            parse_table_init(unit, &parse_table);
            
            parse_table_add(&parse_table, "ID", PQ_INT,
			(void *) BCM_BFD_ENDPOINT_INVALID, &endpoint_info.id, NULL);

            if (parse_arg_eq(args, &parse_table) < 0 || ARG_CNT(args) > 0)
            {
                cli_out("Invalid option: %s\n", ARG_CUR(args));

                CLEAN_UP_AND_RETURN(CMD_FAIL);
            }
            
            result = bcm_bfd_endpoint_get(unit, endpoint_info.id, &endpoint_info);
            if (BCM_FAILURE(result))
            {
                cli_out("Command failed. %s\n", bcm_errmsg(result));

                CLEAN_UP_AND_RETURN(CMD_FAIL);
            }

	    cli_out(ENDPOINT_LIST_HEADER);

	    cli_out("%1d  ", endpoint_info.id);
	    cli_out("%4d  ", endpoint_info.type);
            cli_out("%4d  ", endpoint_info.vrf_id);
	    cli_out("%4d  ", endpoint_info.gport);
	    cli_out("%2d  ", ((endpoint_info.flags & BCM_BFD_ENDPOINT_IPV6) != 0));
	    cli_out("0x%x ",  endpoint_info.src_ip_addr);
	    cli_out("0x%x ",  endpoint_info.dst_ip_addr);
	    cli_out("%10d ",  endpoint_info.local_discr);
	    cli_out("%12d ",  endpoint_info.cpu_qid);
	    cli_out("%6d \n",  endpoint_info.int_pri);

            CLEAN_UP_AND_RETURN(CMD_OK);

        }
        else if (sal_strcasecmp(arg_string_p, "delete") == 0)
        {
            parse_table_init(unit, &parse_table);

            parse_table_add(&parse_table, "ID", PQ_INT,
                (void *) BCM_BFD_ENDPOINT_INVALID, &endpoint_info.id, NULL);

            if (parse_arg_eq(args, &parse_table) < 0 || ARG_CNT(args) > 0)
            {
                cli_out("Invalid option: %s\n", ARG_CUR(args));

                CLEAN_UP_AND_RETURN(CMD_FAIL);
            }

            result = bcm_bfd_endpoint_destroy(unit, endpoint_info.id);
            
            if (BCM_FAILURE(result))
            {
                cli_out("Command failed. %s\n", bcm_errmsg(result));

                CLEAN_UP_AND_RETURN(CMD_FAIL);
            }

            CLEAN_UP_AND_RETURN(CMD_OK);
        }
        else
        {
            bcm_bfd_endpoint_info_t_init(&endpoint_info);

            if (sal_strcasecmp(arg_string_p, "update") == 0)
            {
                endpoint_info.flags |= BCM_BFD_ENDPOINT_UPDATE;
            }
            else if (sal_strcasecmp(arg_string_p, "add") != 0)
            {
                cli_out("Invalid BFD endpoint command: %s\n", arg_string_p);

                return CMD_FAIL;
            }
          
            parse_table_init(unit, &parse_table);

            parse_table_add(&parse_table, "ID", PQ_INT,
               (void *) BCM_BFD_ENDPOINT_INVALID, &endpoint_info.id, NULL);

            parse_table_add(&parse_table, "GPORT", PQ_INT,
                0, &gport, NULL);
            parse_table_add(&parse_table, "TYPE", PQ_INT,
                0, &type, NULL);
            parse_table_add(&parse_table, "VRF", PQ_INT,
                0, &vrf, NULL);
            parse_table_add(&parse_table, "INTPRI", PQ_INT, 
                0, &int_pri, NULL);
            parse_table_add(&parse_table, "CPUPRI", PQ_INT, 
                0, &qid, NULL);
            parse_table_add(&parse_table, "DISCRIMINATOR", PQ_INT, 
                0, &disc_id, NULL);
            parse_table_add(&parse_table, "V6", PQ_INT, 
                0, &v6, NULL);
            parse_table_add(&parse_table, "SIP", PQ_INT, 
                0, &src_ip, NULL);            
            parse_table_add(&parse_table, "DIP", PQ_INT, 
                0, &dst_ip, NULL);            
            parse_table_add(&parse_table, "LABEL", PQ_INT, 
                0, &label, NULL);            

            if (parse_arg_eq(args, &parse_table) < 0 || ARG_CNT(args) > 0)
            {
                cli_out("Invalid option: %s\n", ARG_CUR(args));

                CLEAN_UP_AND_RETURN(CMD_FAIL);
            }

            if (endpoint_info.id != BCM_BFD_ENDPOINT_INVALID)
            {
                endpoint_info.flags |= BCM_BFD_ENDPOINT_WITH_ID;
            }

            if (v6) {
                endpoint_info.flags |= BCM_BFD_ENDPOINT_IPV6;
            }

            if (int_pri < 0)
            {
                 cli_out("An internal priority is required.\n");

                 CLEAN_UP_AND_RETURN(CMD_FAIL);
            }

            endpoint_info.gport = BCM_GPORT_LOCAL_SET(gport, gport);
            endpoint_info.type = type;
            endpoint_info.vrf_id = vrf;
            endpoint_info.local_discr = disc_id;
            endpoint_info.cpu_qid = qid;
            endpoint_info.int_pri = int_pri;
            endpoint_info.label = label;
            endpoint_info.src_ip_addr = src_ip;
            endpoint_info.dst_ip_addr = dst_ip;

            result = bcm_bfd_endpoint_create(unit, &endpoint_info);

            if (BCM_FAILURE(result))
            {
                cli_out("Command failed. %s\n", bcm_errmsg(result));

                CLEAN_UP_AND_RETURN(CMD_FAIL);
            }

            parse_arg_eq_done(&parse_table);
            
            cli_out("BFD endpoint %d created.\n", endpoint_info.id);
          }

        }
    else if (sal_strcasecmp(arg_string_p, "trace") == 0) {
        _bcm_bfd_dump_trace(unit);
    }
    else
    {
        cli_out("Invalid BFD subcommand: %s\n", arg_string_p);

        return CMD_FAIL;
    }

    return CMD_OK;
}

#else
    int  __no_complilation_complaints_about_bfd__1;
#endif /* INCLUDE_BFD */
