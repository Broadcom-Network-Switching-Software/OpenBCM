/* 
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        gport.c
 * Purpose:     Device gport commands.
 */


#include <shared/bsl.h>

#include <soc/drv.h>
#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/cosq.h>
#include <sal/appl/sal.h>
#include <appl/diag/shell.h>
#include <appl/diag/dcmn/gport.h>
#ifdef BCM_PETRA_SUPPORT
#include <bcm_int/dpp/cosq.h>
#include <soc/dpp/TMC/tmc_api_general.h>
#endif
#include <soc/defs.h>

STATIC int
dpp_cosq_gport_traverse_cb_gport(int unit,bcm_gport_t port,
               int numq, uint32 flags,
               bcm_gport_t gport, void *user_data);

char cmd_dpp_gport_usage[] =
    "\n\tGport commands\n\t"
    "Usages:\n\t"
    "gport"
    "\n show the current queue gports in the system\n"
    ;

cmd_result_t
cmd_dpp_gport(int unit, args_t* a)
{
    int rv = BCM_E_NONE;
    dpp_gport_cb_params_t cb_params;
    char* is_count = ARG_CUR(a);
    sal_memset(&cb_params, 0, sizeof(dpp_gport_cb_params_t));


    if((is_count) && (!sal_strcmp(is_count,"count") || !sal_strcmp(is_count,"c"))) {
        cb_params.cmd = DPP_CMD_COSQ_GPORT_COUNT;
        cb_params.gport_count.nof_conn = 0;   
        cb_params.gport_count.nof_uc_qs = 0;  
        cb_params.gport_count.nof_mc_qs = 0;  
        cb_params.gport_count.nof_cl_ses = 0; 
        cb_params.gport_count.nof_fq_ses = 0; 
        cb_params.gport_count.nof_hr_ses = 0;
    }

    else {
        cb_params.cmd = DPP_CMD_COSQ_GPORT_SHOW_ALL;
    }
      

    rv = bcm_cosq_gport_traverse(unit,
                                 dpp_cosq_gport_traverse_cb_gport,
                                 &cb_params);

    if((is_count) && (!sal_strcmp(is_count,"count") || !sal_strcmp(is_count,"c"))) {
        cli_out("Total connectors: %d \n",cb_params.gport_count.nof_conn);
        cli_out("Total unicast queues: %d \n",cb_params.gport_count.nof_uc_qs);
        cli_out("Total multicast queues: %d \n",cb_params.gport_count.nof_mc_qs);
        cli_out("Total cl ses: %d \n",cb_params.gport_count.nof_cl_ses);
        cli_out("Total fq ses: %d \n",cb_params.gport_count.nof_fq_ses);
        cli_out("Total hr ses: %d \n",cb_params.gport_count.nof_hr_ses);
        ARG_GET(a); /*Eat away excess arg*/
    }
    return (rv == BCM_E_NONE ? CMD_OK : CMD_FAIL);
}

STATIC char*
dpp_get_gport_type_string(bcm_gport_t gport) {

  if (BCM_GPORT_IS_MODPORT(gport)) {
      return "ModPort";
  }else if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)){
      return "Unicast Queue Group";
  }else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
      return "Multicast Queue Group";
  }else if (BCM_COSQ_GPORT_IS_ISQ(gport)) {
      return "Ingress Shaping Queue Group ";
  }else if (BCM_GPORT_IS_SCHEDULER(gport)) {
      return "Scheduler ";
  }else if (BCM_COSQ_GPORT_IS_VOQ_CONNECTOR(gport)) {
      return "VOQ Connector ";
  }
  /* gport is invalid */
  return "Invalid";
}
STATIC char *
dpp_get_bw_mode_string(int bw_mode) {
    switch ( bw_mode ) {
        case BCM_COSQ_SP0:
            return "BCM_COSQ_SP0";
        case BCM_COSQ_SP1:
            return "BCM_COSQ_SP1";
        case BCM_COSQ_SP2:
            return "BCM_COSQ_SP2";
        case BCM_COSQ_SP3:
            return "BCM_COSQ_SP3";
        case BCM_COSQ_SP4:
            return "BCM_COSQ_SP4";
        case BCM_COSQ_SP5:
            return "BCM_COSQ_SP5";
        case BCM_COSQ_SP6:
            return "BCM_COSQ_SP6";
        case BCM_COSQ_SP7:
            return "BCM_COSQ_SP7";
        case BCM_COSQ_NONE:
            return "BCM_COSQ_NONE";
    }
    return "bw mode invalid for  devices";
}


void
dpp_cosq_gport_count(bcm_gport_t gport, int flags, gport_count_t* gport_count)
{
    if (BCM_COSQ_GPORT_IS_VOQ_CONNECTOR(gport)) gport_count->nof_conn++;
    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) gport_count->nof_uc_qs++;
    if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) gport_count->nof_mc_qs++;
#ifdef BCM_PETRA_SUPPORT
    if (BCM_GPORT_IS_SCHEDULER(gport)) {
        if (flags & DPP_DEVICE_COSQ_CL_MASK) {
            gport_count->nof_cl_ses++; 
        } else if(flags & DPP_DEVICE_COSQ_FQ_MASK){
            gport_count->nof_fq_ses++;
        } else if(flags & DPP_DEVICE_COSQ_HR_MASK) {
            gport_count->nof_hr_ses++;
        }
    }
#endif /* BCM_PETRA_SUPPORT */
}


int
dpp_cosq_gport_show(int unit,bcm_gport_t gport, int flags, int verbose)
{

    int rv = BCM_E_NONE;
    int cos;
    int base_queue = -1;
    int base_flow = -1;
    int num_cos_levels;
    int mode;
    int weight;
    char *type = NULL;
    int modid = 0;
    int port = 0;
    bcm_gport_t physical_port = 0;
    char *mode_str = NULL;
    uint32 min_size, max_size, flags_2 = 0;
    uint32 kbits_sec_min, kbits_sec_max;

    type = dpp_get_gport_type_string(gport);
    if (strstr(type,"Invalid") != 0) {
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META_U(unit,
                              "ERROR: gport(0x%x) is invalid\n"),gport));
        return (BCM_E_PARAM);
    }
    
    BCM_IF_ERROR_RETURN(bcm_cosq_gport_get(unit, gport, &physical_port, &num_cos_levels, &flags_2));

    modid = BCM_GPORT_MODPORT_MODID_GET(physical_port);

 
    if (BCM_COSQ_GPORT_IS_VOQ_CONNECTOR(gport)) {

        base_flow = BCM_COSQ_GPORT_VOQ_CONNECTOR_ID_GET(gport);

        cli_out("  gp:0x%08x   cos:%d   %32s    flow:%2d)\n",gport, num_cos_levels, type, base_flow);


    }
#ifdef BCM_PETRA_SUPPORT
    else if(BCM_GPORT_IS_SCHEDULER(gport)) {
        base_flow = BCM_GPORT_SCHEDULER_GET(gport);
        if (flags & DPP_DEVICE_COSQ_CL_MASK) {
            cli_out("  gp:0x%08x   cos:%d   %32s    cl se:%2d)\n",gport, num_cos_levels, type, base_flow);
        }
        if (flags & DPP_DEVICE_COSQ_FQ_MASK) {
            cli_out("  gp:0x%08x   cos:%d   %32s    fq se:%2d)\n",gport, num_cos_levels, type, base_flow);
        }
        if (flags & DPP_DEVICE_COSQ_HR_MASK) {
            cli_out("  gp:0x%08x   cos:%d   %32s    hr se:%2d)\n",gport, num_cos_levels, type, base_flow);
        }
    } 
#endif /* BCM_PETRA_SUPPORT */
    else {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            /* get base_queue and num_cos_levels */ 
            base_queue = BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(gport); 
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            base_queue = BCM_GPORT_MCAST_QUEUE_GROUP_QID_GET(gport);
        }
        else if (BCM_COSQ_GPORT_IS_ISQ(gport)) {
            base_queue = BCM_COSQ_GPORT_ISQ_QID_GET(gport);
        }

        port  = BCM_GPORT_MODPORT_PORT_GET(physical_port);
        cli_out("  gp:0x%08x   cos:%d   %32s    mod:%2d   port:%2d voq:%4d)\n",gport, num_cos_levels, type, modid, port, base_queue);
        /* get mode and weighted value for each cos level */
        if (verbose == 1) {
            cli_out("               cos       mode           weight    kbps_min   kbps_max  min_size  max_size  flags\n");
            for(cos=0;cos<num_cos_levels;cos++) {
                BCM_IF_ERROR_RETURN(bcm_cosq_gport_sched_get(unit, gport, cos, &mode, &weight));
                BCM_IF_ERROR_RETURN(bcm_cosq_gport_bandwidth_get(unit, gport, cos, &kbits_sec_min, &kbits_sec_max, &flags_2));
                BCM_IF_ERROR_RETURN(bcm_cosq_gport_size_get(unit, gport, cos, &min_size, &max_size));
                mode_str = dpp_get_bw_mode_string(mode);
                cli_out("              %3d    %-22s %d     %-6d      %-6d     %dKB    %dKB %3d  \n",cos,mode_str,weight,kbits_sec_min,kbits_sec_max, min_size/1024, max_size/1024, flags);
            }
        }
    } 

    return rv;
}


STATIC int
dpp_cosq_gport_traverse_cb_gport(int unit,bcm_gport_t port,
               int numq, uint32 flags,
               bcm_gport_t gport, void *user_data)
{
    int rv = 0;
    int retval = 0;
    bcm_gport_t physical_port = 0;  
    uint32 gport_flag = 0;
    int num_cos_levels=0;
    int _modid;
    int _port;
    dpp_gport_cb_params_t *p_cb_params = (dpp_gport_cb_params_t *)user_data;
    
    if (p_cb_params == NULL) {
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META_U(unit,
                              "callback params pointer is null\n")));
        return (-1);
    }
    
    if (p_cb_params->cmd == DPP_CMD_COSQ_GPORT_SHOW_ALL){
        rv = dpp_cosq_gport_show(unit,gport,flags,p_cb_params->verbose);
        if (rv != BCM_E_NONE) {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META_U(unit,
                                  "show gport(%d) failed\n"), gport));
            retval = -1;
        }
    } else if(p_cb_params->cmd == DPP_CMD_COSQ_GPORT_COUNT){
        dpp_cosq_gport_count(gport,flags,&p_cb_params->gport_count);
    } else {
        /* show only gports that match this modid/and or port */
        rv = bcm_cosq_gport_get(unit,gport,&physical_port,&num_cos_levels,&gport_flag);
        if (rv != BCM_E_NONE) {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META_U(unit,
                                  "gport_get(%d) failed\n"), gport));
            return rv;
        }
        
        _modid = BCM_GPORT_MODPORT_MODID_GET(physical_port);  
        _port  = BCM_GPORT_MODPORT_PORT_GET(physical_port);    
        
        if (p_cb_params->modid == -1) {
            /* show gports that match this port only */
            if (_port == p_cb_params->port) {
                rv = dpp_cosq_gport_show(unit,gport,flags,p_cb_params->verbose);
                if (rv != BCM_E_NONE) {
                    LOG_ERROR(BSL_LS_APPL_SHELL,
                              (BSL_META_U(unit,
                                          "show gport(%d) with port=%d failed\n"), gport,_port));
                    retval = -1;
                }
                
            }
        } else if (p_cb_params->port == -1) {
            /* show gport that match this modid only */
            if (_modid == p_cb_params->modid) {
                rv = dpp_cosq_gport_show(unit,gport,flags,p_cb_params->verbose);
                if (rv != BCM_E_NONE) {
                    LOG_ERROR(BSL_LS_APPL_SHELL,
                              (BSL_META_U(unit,
                                          "show gport(%d) with modid=%d failed\n"), gport,_modid));
                    retval = -1;
                }
        
            }
        } else {
            /* show gports that match both modid and port */
            if ((_modid == p_cb_params->modid && _port == p_cb_params->port)) {
                rv = dpp_cosq_gport_show(unit,gport,flags,p_cb_params->verbose);
                if (rv != BCM_E_NONE) {
                    LOG_ERROR(BSL_LS_APPL_SHELL,
                              (BSL_META_U(unit,
                                          "show gport(%d) failed\n"), gport));
                    retval = -1;
                }
            }
          }
    }
    return retval;
}



