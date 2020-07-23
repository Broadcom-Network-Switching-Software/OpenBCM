/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DNX FABRIC DIAG
 */


#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_APPL_FABRIC


#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>

#include <sal/appl/io.h>
#include <sal/appl/sal.h>

#include <soc/drv.h>

#include <bcm/error.h>
#include <bcm/fabric.h>

#include <appl/diag/shell.h>

#include <appl/diag/dcmn/fabric.h>
#include <appl/diag/dcmn/diag.h>

#include <soc/dcmn/error.h>


#ifdef BCM_DFE_SUPPORT
#include <appl/diag/dfe/fabric.h>

#include <bcm_int/dfe/dfe_fifo_type.h>
#include <bcm_int/dfe/fabric.h>

#include <soc/dfe/cmn/dfe_diag.h>
#include <soc/dfe/cmn/mbcm.h>
#include <soc/dfe/cmn/dfe_drv.h>
#endif /*BCM_DFE_SUPPORT*/

#ifdef BCM_PETRA_SUPPORT
#include <soc/dpp/drv.h>
#include <soc/dpp/fabric.h>
#include <appl/diag/dpp/fabric.h>
#include <soc/dpp/dpp_config_imp_defs.h>
#endif /*BCM_PETRA_SUPPORT*/

#ifdef BCM_PETRA_SUPPORT
#include <soc/dpp/ARAD/arad_fabric.h>
#endif /*BCM_PETRA_SUPPORT*/

/* 
 *Fabric diag pack types
 */
typedef enum diag_dnx_fabric_type_e
{
    diag_dnx_fabric_type_usage = 0,
    diag_dnx_fabric_type_reachability = 1,
    diag_dnx_fabric_type_connectivity = 2,
    diag_dnx_fabric_type_link = 3,
    diag_dnx_fabric_type_queues = 4,
    diag_dnx_fabric_type_thresholds = 5,
    diag_dnx_fabric_type_properties = 6,
    diag_dnx_fabric_type_link_config = 7,
    diag_dnx_fabric_type_mesh = 8,
    diag_dnx_fabric_type_traffic_profile = 9,
    diag_dnx_fabric_type_mesh_topology = 10,
    diag_dnx_fabric_type_rx_fifo = 11,
    diag_dnx_fabric_type_force = 12,

    /*Must be last!*/
    diag_dnx_fabric_type_nof = 13
} diag_dnx_fabric_diag_type_t;

/*
 * DNX fabric diag pack
 */
const diag_dnx_table_t diag_dnx_fabric_pack[] = {
    /*CMD_NAME,             CMD_ACTION,                     CMD_USAGE,                              CMD_ID*/
    {"usage",               diag_dnx_fabric_pack_usage,     NULL,                                   diag_dnx_fabric_type_usage},
    {"reachability",        diag_dnx_fabric_reachability,   diag_dnx_fabric_reachability_usage,     diag_dnx_fabric_type_reachability},
    {"connectivity",        diag_dnx_fabric_connectivity,   diag_dnx_fabric_connectivity_usage,     diag_dnx_fabric_type_connectivity},
    {"link",                diag_dnx_fabric_link,           diag_dnx_fabric_link_usage,             diag_dnx_fabric_type_link},
    {"queues",              diag_dnx_fabric_queues,         diag_dnx_fabric_queues_usage,           diag_dnx_fabric_type_queues},
    {"thresholds",          diag_dnx_fabric_thresholds,     diag_dnx_fabric_thresholds_usage,       diag_dnx_fabric_type_thresholds},
    {"properties",          diag_dnx_fabric_properties,     diag_dnx_fabric_properties_usage,       diag_dnx_fabric_type_properties},
    {"link_config",         diag_dnx_fabric_link_config,    diag_dnx_fabric_link_config_usage,      diag_dnx_fabric_type_link_config},
    {"mesh",                diag_dnx_fabric_mesh,           diag_dnx_fabric_mesh_usage,             diag_dnx_fabric_type_mesh},
    {"traffic_profile",     diag_dnx_fabric_traffic_profile,diag_dnx_fabric_traffic_profile_usage,  diag_dnx_fabric_type_traffic_profile},
    {"mesh_topology",       diag_dnx_fabric_mesh_topology,  diag_dnx_fabric_mesh_topology_usage,    diag_dnx_fabric_type_mesh_topology},
    {"rx_fifo",             diag_dnx_fabric_rx_fifo,        diag_dnx_fabric_rx_fifo_usage,          diag_dnx_fabric_type_rx_fifo},
    {"force",               diag_dnx_fabric_force,          diag_dnx_fabric_force_usage,            diag_dnx_fabric_type_force},
    {NULL,                  NULL,                           NULL,                                   DNX_DIAG_ID_UNAVAIL}
};

/*DNX fabric diag pack support per {DFE, DPP}*/
STATIC int
diag_dnx_fabric_dfe_support(int unit, int diag_id)
{
    switch (diag_id)
    {
        case diag_dnx_fabric_type_usage:
        case diag_dnx_fabric_type_reachability:
        case diag_dnx_fabric_type_connectivity:
        case diag_dnx_fabric_type_link:
        case diag_dnx_fabric_type_queues:
        case diag_dnx_fabric_type_thresholds:
        case diag_dnx_fabric_type_properties:
        case diag_dnx_fabric_type_link_config:
        case diag_dnx_fabric_type_traffic_profile:
            return TRUE;
            break;

       case diag_dnx_fabric_type_mesh_topology:
            return (SOC_IS_FE3200(unit)) ? TRUE : FALSE;
            break;

        default:
            return FALSE;
    }
}

STATIC int
diag_dnx_fabric_dpp_support(int unit, int diag_id)
{
    switch (diag_id)
    {
        case diag_dnx_fabric_type_usage:
        case diag_dnx_fabric_type_reachability:
        case diag_dnx_fabric_type_connectivity:
        case diag_dnx_fabric_type_link:
        case diag_dnx_fabric_type_link_config:
            return TRUE;
            break;
        case diag_dnx_fabric_type_mesh:
        case diag_dnx_fabric_type_thresholds:
        case diag_dnx_fabric_type_queues:
        case diag_dnx_fabric_type_mesh_topology:
        case diag_dnx_fabric_type_rx_fifo:
        case diag_dnx_fabric_type_force:
            return (((SOC_IS_JERICHO(unit)) && (!SOC_IS_QAX(unit) || soc_feature(unit, soc_feature_packet_tdm_marking)))
                    || SOC_IS_KALIA(unit))
                    ? TRUE : FALSE;
            break;
        default:
            return FALSE;
    }
}

/*DNX fabric diag pack usage*/
cmd_result_t 
diag_dnx_fabric_pack_usage(int unit, args_t *args)
{
    diag_dnx_diag_id_supported_func_ptr_t fabric_diag_support;

    if (SOC_IS_DFE(unit))
    {
        fabric_diag_support = diag_dnx_fabric_dfe_support;
    } else if (SOC_IS_DPP(unit))
    {
        fabric_diag_support = diag_dnx_fabric_dpp_support;
    } else {
        cli_out("Device type is not supported\n");
        return CMD_FAIL;
    }
    cli_out("Fabric diag pack usage:\n");
    cli_out("-----------------------\n");
    diag_dnx_usage_print(unit, diag_dnx_fabric_pack, fabric_diag_support);

    return CMD_OK;
}
char diag_dnx_fabric_diag_pack_usage_str[] = "DNX fabric diag pack - type 'fabric usage' for additionl information\n";

/*DNX fabric pack main function*/
cmd_result_t 
diag_dnx_fabric_diag_pack(int unit, args_t *args)
{
    char *cmd_name;
    diag_dnx_diag_id_supported_func_ptr_t fabric_diag_support;
    cmd_result_t res;

    if (SOC_IS_DFE(unit))
    {
        fabric_diag_support = diag_dnx_fabric_dfe_support;
    } else if (SOC_IS_DPP(unit))
    {
        fabric_diag_support = diag_dnx_fabric_dpp_support;
    } else {
        cli_out("Device type is not supported\n");
        return CMD_FAIL;
    }
    cmd_name = ARG_GET(args);
    res = diag_dnx_dispatch(unit, diag_dnx_fabric_pack, fabric_diag_support, cmd_name, args);
    if (res == CMD_USAGE)
    {
        diag_dnx_fabric_pack_usage(unit, NULL);
        return CMD_FAIL;
    }

    return res;
}

/*
 * Diag Fabric Reachability
 */

void
diag_dnx_fabric_reachability_usage(int unit)
{
    cli_out("'fabric reachability <modid>'- display reachable links to modid\n");
}

cmd_result_t
diag_dnx_fabric_reachability(int unit, args_t *a) {
   int       rc;
   char      *option;
   int modid;
   uint32 links_array[SOC_MAX_NUM_PORTS];
   int port;
   uint32 links_array_isolated[SOC_MAX_NUM_PORTS];
   int array_size, i, nof_isolated_links = 0;
   int minimum_links = -1;
   int isolate = 0;


   option = ARG_GET(a);
   if (!option) {
      return CMD_USAGE;
   }
   modid = sal_ctoi(option, 0);
   
   rc = bcm_fabric_control_get(unit, bcmFabricIsolate, &isolate); 

   if (BCM_FAILURE(rc)) {
       cli_out("bcm_fabric_control_get, control bcmFabricIsolate, failed with status status %s\n", bcm_errmsg(rc));
       return CMD_FAIL;
   }
   

   rc = bcm_fabric_reachability_status_get(unit, modid, SOC_MAX_NUM_PORTS, links_array, &array_size);


   if (BCM_FAILURE(rc)) {
      cli_out("bcm_fabric_reachability_status_get failed with status %s\n", bcm_errmsg(rc));
      return CMD_FAIL;
   }

   if (SOC_IS_ARADPLUS(unit) && !SOC_IS_JERICHO(unit))
   {
   
       rc=bcm_fabric_control_get(unit,bcmFabricMinimalLinksToDestination,&minimum_links);

       if (BCM_FAILURE(rc)) {
           cli_out("bcm_fabric_control_get failed with minimum number of links %s\n", bcm_errmsg(rc));
           return CMD_FAIL;
       } 
   }
   else if (SOC_IS_JERICHO(unit))
   {
       rc = bcm_fabric_destination_link_min_get(unit, 0, modid, &minimum_links);
       if (BCM_FAILURE(rc)) {
           cli_out("bcm_fabric_destination_link_min_get failed %s\n", bcm_errmsg(rc));
           return CMD_FAIL;
       } 
   }

   if (isolate) {
      cli_out("\nThis device is isolated\n\n");
   }
   if (0 == array_size) {
      cli_out("Module %d isn't reachable\n", modid);
      return CMD_OK;
   }

   
   cli_out("Found %d links for module %d:\n", array_size, modid);
   for (i = 0; i < array_size; i++) {
         port = links_array[i];
#ifdef BCM_PETRA_SUPPORT
         if (SOC_IS_ARAD(unit)) {
             port = SOC_DPP_FABRIC_LINK_TO_PORT(unit, links_array[i] + SOC_DPP_DEFS_GET(unit, first_fabric_link_id)/*offset for QMX*/);
         }
#endif   
       rc = bcm_fabric_link_control_get(unit, port, bcmFabricLinkIsolate, &isolate); 
       if (isolate) {
           links_array_isolated[nof_isolated_links] = links_array[i];
           nof_isolated_links ++;
       }
       if (BCM_FAILURE(rc)) {
           cli_out("bcm_fabric_link_control_get, control bcmFabricLinkIsolate, failed with status status %s\n", bcm_errmsg(rc));
           return CMD_FAIL;
       }
       
       if (i == array_size - 1) /* last link */
       {
           cli_out("%d\n", links_array[i]);
       }
       else
       {
           cli_out("%d, ", links_array[i]);
       }
   }

   if (nof_isolated_links) {
       cli_out("Following links are isolated\n"); 
       for (i = 0; i < nof_isolated_links - 1; i++) {

           cli_out("%d, ", links_array_isolated[i]); 
           
       }
       cli_out("%d\n", links_array_isolated[nof_isolated_links - 1]);
   }

   if (minimum_links != -1)
   {
   
       cli_out("The minimum number of links to destination is %d\n",minimum_links);
       if (minimum_links>array_size && minimum_links)
       {
           cli_out("Current number of links is smaller than minimal number of links\n");
       }
   }
   return CMD_OK;
}


/*
 * Diag fabric connectivity
 */

void
diag_dnx_fabric_connectivity_usage(int unit)
{
    cli_out("'fabric connectivity'- display fabric interface connectivity information\n");
}

cmd_result_t
diag_dnx_fabric_connectivity(int unit, args_t *a) {
   int       rc, i, array_size;
   bcm_fabric_link_connectivity_t *links_connectivity_array = NULL;
   int port_index;
   bcm_port_config_t config;
   bcm_port_t port;
   cmd_result_t result = CMD_OK;

   /*Get first SFI port*/
   rc = bcm_port_config_get(unit, &config);
   if ((BCM_FAILURE(rc))) {
       cli_out("ERROR: bcm_port_config_get failed\n");
       return CMD_FAIL;
   }
   
   links_connectivity_array = sal_alloc(sizeof(bcm_fabric_link_connectivity_t) * SOC_MAX_NUM_PORTS, "diag_dnx_fabric_connectivity.links_connectivity_array");
   if(links_connectivity_array == NULL) {
       cli_out("Memory allocation failure\n");
       return CMD_FAIL;
   }
   rc = bcm_fabric_link_connectivity_status_get(unit, SOC_MAX_NUM_PORTS, links_connectivity_array, &array_size);
   if (BCM_FAILURE(rc)) {
      cli_out("bcm_fabric_link_connectivity_status_get failed with status %s\n", bcm_errmsg(rc));
      result = CMD_FAIL;
      goto exit;
   }

   cli_out("Connectivity status:\n");
   cli_out("--------------------\n");
   cli_out(" Link | Logical Port | Remote Module | Remote Link | Remote Device Type\n");
   cli_out(" ----------------------------------------------------------------------\n");
   i = 0;
   BCM_PBMP_ITER(config.sfi, port)
   {
       if (i >= array_size)
       {
           break;
       }

      if (BCM_FABRIC_LINK_NO_CONNECTIVITY != links_connectivity_array[i].link_id) {
         char *dev_type;
         switch (links_connectivity_array[i].device_type) {
         case bcmFabricDeviceTypeFE1:
         case bcmFabricDeviceTypeFE3:
         case bcmFabricDeviceTypeFE13:
            dev_type = "FE13";
            break;

         case bcmFabricDeviceTypeFE2:
            dev_type = "FE2";
            break;

         case bcmFabricDeviceTypeFAP:
         case bcmFabricDeviceTypeFIP:
         case bcmFabricDeviceTypeFOP:
            dev_type = "FAP";
            break;

         case bcmFabricDeviceTypeUnknown:
         default:
            dev_type = "Unknown";
            break;
         }

#ifdef BCM_PETRA_SUPPORT
         if (SOC_IS_DPP(unit)) {
             port_index = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port);
         } else
#endif
         {
            port_index = port;
         }

         cli_out(" %03d  |     %03d      |     %04d      |     %03d     |     %s\n", port_index  , port, links_connectivity_array[i].module_id, links_connectivity_array[i].link_id, dev_type);

         
      }

      i++;
   }

exit:
   sal_free(links_connectivity_array);
   return result;
}

/*
 * Diag fabric link status
 */

void
diag_dnx_fabric_link_usage(int unit)
{
    cli_out("'fabric link <'all'/link_id>'- display fabric link status\n");
}


cmd_result_t
diag_dnx_fabric_link(int unit, args_t *a) {
   int rc, i;
   char      *option;
   bcm_port_t link_id;
   uint32 link_status, errored_token_count;
   uint32 * links_status,*links_errored_token_counts;
   int max_array_size;
   int array_size = 10;
   int strnlen_option;

   option = ARG_GET(a);
   if (!option) {
      return CMD_USAGE;
   }
   strnlen_option = sal_strnlen(option,SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
   /*
    * Make sure at least one of the strings is NULL terminated.
    */
   if (!sal_strncasecmp(option, "all", strnlen_option)) {
      option = ARG_GET(a);
      if (!option) {
#ifdef BCM_DFE_SUPPORT
         if (SOC_IS_DFE(unit)) {
            max_array_size = SOC_DFE_DEFS_GET(unit, nof_links);
         } else
#endif
#ifdef BCM_PETRA_SUPPORT
            if (SOC_IS_DPP(unit)) {
               max_array_size = SOC_DPP_DEFS_GET(unit, nof_fabric_links);
            } else
#endif
               return CMD_USAGE;
      } else {
         max_array_size = sal_ctoi(option, 0);
      }

      links_status = (uint32 *)sal_alloc(max_array_size * sizeof(uint32), "links_status");
      if (NULL == links_status) {
         cli_out("failed to allocate links_status array\n");
         return CMD_FAIL;
      }

      links_errored_token_counts = (uint32 *)sal_alloc(max_array_size * sizeof(uint32), "links_errored_token_counts");
      if (NULL == links_errored_token_counts) {
         cli_out("failed to allocate links_errored_token_counts array\n");
         sal_free(links_status);
         return CMD_FAIL;
      }

#ifdef BCM_DFE_SUPPORT
      if (SOC_IS_DFE(unit)) {
         rc = soc_dfe_fabric_link_status_all_get(unit, max_array_size, links_status, links_errored_token_counts, &array_size);
      } else
#endif
#ifdef BCM_PETRA_SUPPORT
         if (SOC_IS_ARAD(unit)) {
            rc = arad_fabric_link_status_all_get(unit, max_array_size, links_status, links_errored_token_counts, &array_size);
         } else
#endif
         {
            rc = BCM_E_UNAVAIL;
         }
      if (BCM_FAILURE(rc)) {
         cli_out("api call fabric_link_status_all_get failed with status %s\n", bcm_errmsg(rc));
         sal_free(links_status);
         sal_free(links_errored_token_counts);
         return CMD_FAIL;
      }

      cli_out("Links status:\n");
      cli_out("--------------------\n");
      cli_out("    Link  | CRC Error  | Size Error | Code Group Error | Misalign | No Signal Lock | No signal accept | Errored tokens | Errored tokens count\n");
      cli_out(" ----------------------------------------------------------------------------------------------------------------------------------------\n");
      for (i = 0; i < array_size; i++) {

#ifdef BCM_PETRA_SUPPORT
         if (SOC_IS_ARAD(unit)) {
             bcm_port_t port = SOC_DPP_FABRIC_LINK_TO_PORT(unit, i + SOC_DPP_DEFS_GET(unit, first_fabric_link_id)/*offset for QMX*/);
             

             if (!BCM_PBMP_MEMBER(PBMP_SFI_ALL(unit), port))
             {   
                 continue;
             }
            cli_out(" %3d(%3d) |", port, SOC_DPP_FABRIC_PORT_TO_LINK(unit, port));
         } else
#endif
         {

             if (!BCM_PBMP_MEMBER(PBMP_SFI_ALL(unit), i))
             { 
                 continue;
             }
             cli_out(" %8d |", i);

         }


         
         cli_out("     %s    |    %s    |         %s      |   %s    |      %s       |        %s         |      %s       |   %d\n",
                 links_status[i] & BCM_FABRIC_LINK_STATUS_CRC_ERROR ? "***" : " - ",
                 links_status[i] & BCM_FABRIC_LINK_STATUS_SIZE_ERROR ? "***" : " - ",
                 links_status[i] & BCM_FABRIC_LINK_STATUS_CODE_GROUP_ERROR ? "***" : " - ",
                 links_status[i] & BCM_FABRIC_LINK_STATUS_MISALIGN ? "***" : " - ",
                 links_status[i] & BCM_FABRIC_LINK_STATUS_NO_SIG_LOCK ? "***" : " - ",
                 links_status[i] & BCM_FABRIC_LINK_STATUS_NO_SIG_ACCEP ? "***" : " - ",
                 links_status[i] & BCM_FABRIC_LINK_STATUS_ERRORED_TOKENS ? "***" : " - ",
                 links_errored_token_counts[i]);
      }
      sal_free(links_status);
      sal_free(links_errored_token_counts);
      return CMD_FAIL;
   } else {

      /*
       * Make sure at least one of the strings is NULL terminated.
       */
      if (!sal_strncasecmp(option, "clear", strnlen_option)) {
          /*Do nothing - all the indications cleared  on read*/
          cli_out("No need to explicitly clear the indications - all indications cleared on read\n");
          return CMD_OK;
      }

      link_id = sal_ctoi(option, 0);

#ifdef BCM_PETRA_SUPPORT
      if (SOC_IS_DPP(unit)) {
         link_id = SOC_DPP_FABRIC_LINK_TO_PORT(unit, link_id);
      }
#endif

      rc = bcm_fabric_link_status_get(unit, link_id, &link_status, &errored_token_count);
      if (BCM_FAILURE(rc)) {
         cli_out("bcm_fabric_link_status_get failed with status %s\n", bcm_errmsg(rc));
         return CMD_FAIL;
      }

      cli_out("link %d status: \n", link_id);
      cli_out("--------------- \n");
      cli_out("CRC error       : %s\n", link_status & BCM_FABRIC_LINK_STATUS_CRC_ERROR ? "***" : " - ");
      cli_out("Size error      : %s\n", link_status & BCM_FABRIC_LINK_STATUS_SIZE_ERROR ? "***" : " - ");
      cli_out("Code group error: %s\n", link_status & BCM_FABRIC_LINK_STATUS_CODE_GROUP_ERROR ? "***" : " - ");
      cli_out("Misalign        : %s\n", link_status & BCM_FABRIC_LINK_STATUS_MISALIGN ? "***" : " - ");
      cli_out("No signal lock  : %s\n", link_status & BCM_FABRIC_LINK_STATUS_NO_SIG_LOCK ? "***" : " - ");
      cli_out("No signal accept: %s\n", link_status & BCM_FABRIC_LINK_STATUS_NO_SIG_ACCEP ? "***" : " - ");
      cli_out("Errored tokens  : %s\n", link_status & BCM_FABRIC_LINK_STATUS_ERRORED_TOKENS ? "***" : " - ");
      cli_out("Errored tokens count: %d\n", errored_token_count);

      return CMD_OK;

   }
}

/* 
 *Diag fabric queues
 */

void
diag_dnx_fabric_queues_usage(int unit)
{
    cli_out("'fabric queues'- display queues status\n");
}

cmd_result_t
diag_dnx_fabric_queues(int unit, args_t *a) 
{

#ifdef BCM_DFE_SUPPORT   
   if (SOC_IS_DFE(unit)) 
   {
       int rv;
       
       rv = diag_dfe_fabric_queues_print(unit);
       if (BCM_FAILURE(rv)) 
       {
           cli_out("ERRROR: soc_dfe_queues_print failed.\n");
           return CMD_FAIL;
      
       }

   } else 
#endif /*BCM_DFE_SUPPORT*/
#ifdef BCM_PETRA_SUPPORT
    if (SOC_IS_DPP(unit)) {
       int rv;
       
       rv = diag_dpp_fabric_queues_print(unit);
       if (BCM_FAILURE(rv)) {
           cli_out("ERRROR: soc_dfe_queues_print failed.\n");
           return CMD_FAIL;
      
       }
    } else
#endif
    {
        cli_out("Diag is not supported by device type.\n");
        return CMD_FAIL;
    }

   return CMD_OK;
}

 /*
  * Diag fabric thresholds
  */

void
diag_dnx_fabric_thresholds_usage(int unit)
{
    if (SOC_IS_DFE(unit))
    {

        cli_out("'fabric thresholds <threshold_type> [pipe=<0/1/2>] [direction=<rx/tx/middle>] [link=<link_id>]' - display the configured FIFO thresholds\n"
                "Supported threshold types: RCI, GCI, DROP, LLFC, ALMOST_FULL (BCM88950 and above), FULL (BCM88950 and above), SIZE (BCM88950 and above)\n");
    } else if (SOC_IS_JERICHO(unit)) {
        cli_out("'fabric thresholds <threshold_type> [pipe=<0/1/2>] [link=<link_id>]' - display the configured FIFO thresholds\n"
                "Supported threshold types: RCI, LLFC\n");
    } else {
        cli_out("Device type is not supported.\n");
    }
}

STATIC cmd_result_t
diag_dnx_fabric_thresholds_get(int unit, 
                              bcm_port_t port,
                              bcm_fabric_pipe_t pipe,
                              soc_dcmn_fabric_direction_t direction,
                              bcm_fabric_link_threshold_type_t *types,
                              int *values,
                              int nof_types)
{

    uint32 h;
    int rv, i;
    bcm_fabric_pipe_t pipe_get;
    uint32 flags_get;
    int supported;


    /* get thresholds and links */
#ifdef BCM_DFE_SUPPORT
    if (SOC_IS_DFE(unit))
    {
        rv = _bcm_dfe_fabric_fifo_info_get(unit, port, pipe, direction, (uint32 *) &h, &pipe_get, &flags_get);
        if (BCM_FAILURE(rv)) {
            return CMD_FAIL;
        }

    } else
#endif /*BCM_DFE_SUPPORT*/
#ifdef BCM_PETRA_SUPPORT
    if (SOC_IS_DPP(unit)) {
        h = -1; /*all FIFO's*/
        pipe_get = pipe;
        flags_get = 0;
    } else
#endif /*BCM_PETRA_SUPPORT*/
    {
        cli_out("Device type is not supported.\n");
        return  CMD_USAGE;
    }

    for (i = 0; i < nof_types; i++)
    {
        /*check if type is supported for specific type*/
       supported = 1;
#ifdef BCM_DFE_SUPPORT
        if (SOC_IS_DFE(unit))
        {
            rv = diag_dfe_fabric_thresholds_supported_get(unit, port, types[i], &supported);
            if (rv != CMD_OK)
            {
                return rv;
            }
        }
#endif
        if (supported)
        {
            rv = bcm_fabric_link_thresholds_pipe_get(unit, h, pipe_get, flags_get, 1, &types[i], &values[i]);
            if (BCM_FAILURE(rv)) {
                cli_out("ERRROR: bcm_fabric_link_thresholds_get failed.\n");
                return CMD_FAIL;
            }
        } else {
            values[i] = -1;
        }
    }
   
    return CMD_OK;
}

STATIC cmd_result_t
diag_dnx_fabric_thresholds_types_parse(int unit, char *th_name, diag_dnx_fabric_link_th_info_t *thresholds_info)
{
#ifdef BCM_DFE_SUPPORT
    if (SOC_IS_DFE(unit))
    {
        return diag_dfe_fabric_thresholds_types_parse(unit, th_name, thresholds_info);
    }
#endif /*BCM_DFE_SUPPORT*/
#ifdef BCM_PETRA_SUPPORT
    if (SOC_IS_DPP(unit))
    {
        return diag_dpp_fabric_thresholds_types_parse(unit, th_name, thresholds_info);
    }
#endif /*BCM_PETRA_SUPPORT*/

    cli_out("Diagnostic is not supported by this device type.\n");
    return CMD_USAGE;
}

cmd_result_t
diag_dnx_fabric_thresholds_threshold_to_str(int unit, bcm_fabric_link_threshold_type_t type, char **type_name)
{
#ifdef BCM_PETRA_SUPPORT
    if (SOC_IS_DPP(unit)) {
        diag_dpp_fabric_thresholds_threshold_to_str(unit, type, type_name);
    } else 
#endif
#ifdef BCM_DFE_SUPPORT
    if(SOC_IS_DFE(unit)){
        diag_dfe_fabric_thresholds_threshold_to_str(unit, type, type_name);
    } else
#endif         
    {
        cli_out("Diagnostic is not supported by this device type.\n");
        return CMD_USAGE;
    }
    return CMD_OK;
}

STATIC cmd_result_t
diag_dnx_fabric_thresholds_line_print(int unit, 
                                          bcm_port_t port, 
                                          bcm_fabric_pipe_t pipe_id, 
                                          soc_dcmn_fabric_direction_t direction,
                                          bcm_fabric_link_threshold_type_t *types, 
                                          int nof_types)
{
    int i, rv;
    int values[DIAG_DNX_FABRIC_THRESHOLDS_MAX]; 

    rv = diag_dnx_fabric_thresholds_get(unit, port, pipe_id, direction, types, values, nof_types);
    if (rv != CMD_OK)
    {
        return CMD_FAIL;
    }
    if (pipe_id < 0) { /*if pipe == bcmFabricPipeAll*/
        pipe_id = 0;
    }

    if (direction == soc_dcmn_fabric_direction_middle)
    {
        cli_out("    All    |");
    } else if (direction == soc_dcmn_fabric_direction_middle_local_switch_low)
    {
        cli_out("LocalRoute1|");
    } else if (direction == soc_dcmn_fabric_direction_middle_local_switch_high)
    {
        cli_out("LocalRoute2|");
    } else {
        cli_out("    %-3d    |", port); 
    }
    
    cli_out("    %-3d    |", pipe_id);
    for (i=0; i < nof_types; i++) 
    {

        cli_out("    %-3d    |", values[i]);
    }
    cli_out("\n");

    return CMD_OK;

}

STATIC cmd_result_t
diag_dnx_fabric_thresholds_direction_print(int unit, 
                                          bcm_pbmp_t pbmp, 
                                          bcm_fabric_pipe_t pipe_id, 
                                          soc_dcmn_fabric_direction_t direction,
                                          bcm_fabric_link_threshold_type_t *types, 
                                          int nof_types)
{
    int i, rv;
    bcm_fabric_pipe_t pipe_id_start, pipe_id_end;
    char *th_name;
    bcm_fabric_pipe_t pipe;
    bcm_port_t port;

    cli_out("   link    |  pipe     |");
    for (i = 0; i < nof_types; i++)
    {
        rv = diag_dnx_fabric_thresholds_threshold_to_str(unit, types[i], &th_name);
        if (rv != CMD_OK)
        {
            return rv;
        }
        cli_out("    %-7s|", th_name);
    }
    cli_out("\n");


    cli_out("  ---------------------");
    for (i = 0; i < nof_types; i++) 
        cli_out("------------");
    cli_out("\n");

    pipe_id_start = pipe_id;
    pipe_id_end = pipe_id;

#ifdef BCM_DFE_SUPPORT
        if (SOC_IS_DFE(unit) && pipe_id == bcmFabricPipeAll)
        {
            pipe_id_start = bcmFabricPipe0;
            pipe_id_end = bcmFabricPipe0 + SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes - 1;
        }
#endif

    if (direction == soc_dcmn_fabric_direction_middle)
    {
        for (pipe = pipe_id_start; pipe <= pipe_id_end; pipe++)
        {
            rv = diag_dnx_fabric_thresholds_line_print(unit, -1, pipe, direction, types, nof_types);
            if (rv != CMD_OK)
            {
                return rv;
            }


#ifdef BCM_DFE_SUPPORT
            if (SOC_IS_DFE(unit))
            {
                if (SOC_DFE_CONFIG(unit).fabric_local_routing_enable != SOC_DFE_PROPERTY_UNAVAIL &&
                    SOC_DFE_CONFIG(unit).fabric_local_routing_enable)
                {
                    rv = diag_dnx_fabric_thresholds_line_print(unit, -1, pipe, soc_dcmn_fabric_direction_middle_local_switch_low, types, nof_types);
                    if (rv != CMD_OK)
                    {
                        return rv;
                    }
                    rv = diag_dnx_fabric_thresholds_line_print(unit, -1, pipe, soc_dcmn_fabric_direction_middle_local_switch_high, types, nof_types);
                    if (rv != CMD_OK)
                    {
                        return rv;
                    }
                }
            }
 #endif
        }
    } else {

        PBMP_ITER(pbmp, port) 
        {
            for (pipe = pipe_id_start; pipe <= pipe_id_end; pipe++)
            {
                rv = diag_dnx_fabric_thresholds_line_print(unit, port, pipe, direction, types, nof_types);
                if (rv != CMD_OK)
                {
                    return rv;
                }
            }
        }
    }

    return CMD_OK;
}
cmd_result_t
    diag_dnx_fabric_thresholds(int unit, args_t* a)
{
    int rv;
    int port = -1; /* default value -1 :  all links */
    bcm_pbmp_t pbmp;
    bcm_port_config_t config;
    diag_dnx_fabric_link_th_info_t th_info;
    char *th_str = NULL, *direction_s = NULL;
    parse_table_t pt;
    bcm_fabric_pipe_t pipe_id = bcmFabricPipeAll; /*all pipes*/
    int is_rx, is_tx, is_middle;
    
    th_str = ARG_GET(a);

    if (th_str == NULL)
    {
        return CMD_USAGE;
    }

    sal_memset(&th_info, 0x0, sizeof(diag_dnx_fabric_link_th_info_t));
    rv = diag_dnx_fabric_thresholds_types_parse(unit, th_str, &th_info);
    if (rv != CMD_OK)
    {
        return rv;
    }
    
    /* parsing input from user */
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "pipe", PQ_DFL|PQ_INT, &pipe_id, &pipe_id, NULL);
    if(SOC_IS_DFE(unit)) {
        parse_table_add(&pt, "direction", PQ_DFL|PQ_STRING, NULL, &direction_s, NULL);
    }
    parse_table_add(&pt, "link", PQ_DFL|PQ_INT, &port, &port, NULL );

    if (parse_arg_eq(a, &pt) < 0) 
    {
        cli_out("invalid option: %s\n", ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }

    if (SOC_IS_DPP(unit)) {
        is_rx = 1;
        is_tx = is_middle = 0;
    } else if (direction_s != NULL && *direction_s != '\0') {
        int strnlen_direction_s;

        is_rx = is_tx = is_middle = 0;
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        strnlen_direction_s = sal_strnlen(direction_s, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
        if (!sal_strncasecmp(direction_s,"rx",strnlen_direction_s)) {
            is_rx = 1;
        } else if (!sal_strncasecmp(direction_s,"tx",strnlen_direction_s)) {
            is_tx = 1;
        } else if (!sal_strncasecmp(direction_s,"middle",strnlen_direction_s)) {
            is_middle = 1;
        } else {
            cli_out("invalid option: %s\n", direction_s);
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }
    } else {
        is_rx = is_tx = is_middle = 1;
    }

    parse_arg_eq_done(&pt);

    /* ports to print: */
    BCM_PBMP_CLEAR(pbmp);

    rv = bcm_port_config_get(unit, &config);
    if ((BCM_FAILURE(rv))) {
        cli_out("ERROR: bcm_port_config_get failed\n");
        return CMD_FAIL;
    }

    if (port == -1) {
        BCM_PBMP_ASSIGN(pbmp, config.sfi);
    } else {
        if ((port > -1)&&(BCM_PBMP_MEMBER(config.sfi, port))){
            BCM_PBMP_PORT_ADD(pbmp,port);
        } else {
            cli_out("link number is invalid\n");
            return CMD_FAIL;
        }
    }
    

    cli_out("threshold status:\n");
    cli_out("-----------------\n");

    /*RX*/
    if (is_rx && (th_info.th_types_rx_nof > 0))
    {
        cli_out("RX:\n");
        rv = diag_dnx_fabric_thresholds_direction_print(unit, pbmp, pipe_id, soc_dcmn_fabric_direction_rx, th_info.th_types_rx, th_info.th_types_rx_nof);
        if (rv != CMD_OK)
        {
            return rv;
        }
    }

    /*TX*/
    if (is_tx && (th_info.th_types_tx_nof > 0))
    {
        cli_out("TX:\n");
        rv = diag_dnx_fabric_thresholds_direction_print(unit, pbmp, pipe_id, soc_dcmn_fabric_direction_tx, th_info.th_types_tx, th_info.th_types_tx_nof);
        if (rv != CMD_OK)
        {
            return rv;
        }
    }

    /*Middle stage*/
    if (is_middle && (th_info.th_types_middle_nof > 0))
    {
        cli_out("Middle Stage:\n");
        rv = diag_dnx_fabric_thresholds_direction_print(unit, pbmp, pipe_id, soc_dcmn_fabric_direction_middle, th_info.th_types_middle, th_info.th_types_middle_nof);
        if (rv != CMD_OK)
        {
            return rv;
        }
    }

    return CMD_OK;
}

/*
 * Diag Fabric Properties
 */

void
diag_dnx_fabric_properties_usage(int unit)
{
    cli_out("'fabric properties'- display current used fabric properties values\n");
}

cmd_result_t
diag_dnx_fabric_properties(int unit, args_t *a) 
{

#ifdef BCM_DFE_SUPPORT
    if (SOC_IS_DFE(unit))
    {
        soc_dfe_drv_soc_properties_dump(unit);
    }
#endif /*BCM_DFE_SUPPORT*/

    return CMD_OK;
}

/*
 * Diag Fabric link config
 */
void
diag_dnx_fabric_link_config_usage(int unit)
{
    cli_out("'fabric link_config'- display current used fabric link configuration\n");
}

cmd_result_t
diag_dnx_fabric_link_config(int unit, args_t *a) 
{
    int rv;
    diag_dnx_fabric_link_config_t link_config;
    bcm_port_config_t port_config;
    char pcs_str[50], pipe_mapping_buf[20];
    char *pcs, *pipe_mapping_str;
    bcm_port_t link;
    uint32 lane_swap_quad;
    bcm_fabric_pipe_t remote_pipe_map[SOC_DCMN_FABRIC_MAX_NOF_PIPES];

    rv = bcm_port_config_get(unit, &port_config);
    if ((BCM_FAILURE(rv))) {
        cli_out("ERROR: bcm_port_config_get failed\n");
        return CMD_FAIL;
    }
#ifdef BCM_DFE_SUPPORT
    if (SOC_IS_DFE(unit) && SOC_DFE_IS_REPEATER(unit))
    {
        cli_out("link # | enable | speed | ref clock | cl72 |                  pcs                      |  pcp |  nof pipes   |  polarity  | swap \n");
    }
    else
#endif         
    {
        cli_out("link # | enable | speed | ref clock | cl72 |                  pcs                      |  pcp | pipe mapping |  polarity  | swap \n");
    }
    cli_out("-------|--------|-------|-----------|------|-------------------------------------------|------|--------------|------------|----- \n");

    BCM_PBMP_ITER(port_config.sfi, link)
    {

        /*get info*/
        rv = bcm_port_enable_get(unit, link, &link_config.enable);
        if ((BCM_FAILURE(rv))) {
            cli_out("ERROR: bcm_port_enable_get failed\n");
            return CMD_FAIL;
        }

        rv = bcm_port_speed_get(unit, link, &link_config.speed);
        if ((BCM_FAILURE(rv))) {
            cli_out("ERROR: bcm_port_speed_get failed\n");
            return CMD_FAIL;
        }
        rv = bcm_port_control_get(unit, link, bcmPortControlPCS, (int *) &link_config.pcs);
        if ((BCM_FAILURE(rv))) {
            cli_out("ERROR: bcm_port_control_get(bcmPortControlPCS) failed\n");
            return CMD_FAIL;
        }

        link_config.pcs_llfc_extract_cig = 0;
        if (link_config.pcs == bcmPortPCS64b66bFec && !SOC_IS_ARADPLUS_AND_BELOW(unit) && !SOC_IS_FE1600(unit))
        {
            rv = bcm_port_control_get(unit, link, bcmPortControlLlfcCellsCongestionIndEnable, &link_config.pcs_llfc_extract_cig);
            if ((BCM_FAILURE(rv))) {
                cli_out("ERROR: bcm_port_control_get(bcmPortControlLlfcCellsCongestionIndEnable) failed\n");
                return CMD_FAIL;
            }
        }

        if (link_config.pcs == bcmPortPCS64b66bFec || link_config.pcs == bcmPortPCS64b66bRsFec || link_config.pcs == bcmPortPCS64b66bLowLatencyRsFec
                || link_config.pcs == bcmPortPCS64b66b15TRsFec || link_config.pcs == bcmPortPCS64b66b15TLowLatencyRsFec)
        {
            rv = bcm_port_control_get(unit, link, bcmPortControlFecErrorDetectEnable, &link_config.pcs_error_detect);
            if ((BCM_FAILURE(rv))) {
                cli_out("ERROR: bcm_port_control_get(bcmPortControlFecErrorDetectEnable) failed\n");
                return CMD_FAIL;
            }

        } else {
            link_config.pcs_error_detect = 0;
        }

        if (link_config.pcs == bcmPortPCS64b66bFec)
        {
            rv = bcm_port_control_get(unit, link, bcmPortControlLowLatencyLLFCEnable, &link_config.pcs_llfc_low_latency);
            if ((BCM_FAILURE(rv))) {
                cli_out("ERROR: bcm_port_control_get(bcmPortControlLowLatencyLLFCEnable) failed\n");
                return CMD_FAIL;
            }
        } else {
            link_config.pcs_llfc_low_latency = 0;
        }


        rv = bcm_port_phy_control_get(unit, link, BCM_PORT_PHY_CONTROL_CL72, &link_config.cl72);
        if ((BCM_FAILURE(rv))) {
            cli_out("ERROR: bcm_port_phy_control_get(BCM_PORT_PHY_CONTROL_CL72) failed\n");
            return CMD_FAIL;
        }

        rv = bcm_port_phy_control_get(unit, link, BCM_PORT_PHY_CONTROL_TX_POLARITY, &link_config.tx_polarity);
        if ((BCM_FAILURE(rv))) {
            cli_out("ERROR: bcm_port_phy_control_get(BCM_PORT_PHY_CONTROL_TX_POLARITY) failed\n");
            return CMD_FAIL;
        }


        rv = bcm_port_phy_control_get(unit, link, BCM_PORT_PHY_CONTROL_RX_POLARITY, &link_config.rx_polarity);
        if ((BCM_FAILURE(rv))) {
            cli_out("ERROR: bcm_port_phy_control_get(BCM_PORT_PHY_CONTROL_RX_POLARITY) failed\n");
            return CMD_FAIL;
        }


        if (!SOC_IS_ARADPLUS_AND_BELOW(unit) && !SOC_IS_FE1600(unit))
        {      
            rv = bcm_port_phy_control_get(unit, link, BCM_PORT_PHY_CONTROL_LANE_SWAP, &lane_swap_quad);
            if ((BCM_FAILURE(rv))) {
                cli_out("ERROR: bcm_port_phy_control_get(BCM_PORT_PHY_CONTROL_LANE_SWAP) failed\n");
                return CMD_FAIL;
            }
            /*only tx lane swap is supported - shift left to ignore rx lane swap*/
            lane_swap_quad >>= 16;

            link_config.lane_swap  = ((lane_swap_quad >> (4 * (link % 4))) &  0xf) + (link / 4) * 4;
        } else {
            link_config.lane_swap = -1;
        }

        bcm_fabric_link_remote_pipe_mapping_t_init(&link_config.pipe_mapping);
        link_config.pipe_mapping.remote_pipe_mapping = remote_pipe_map;
        link_config.pipe_mapping.remote_pipe_mapping_max_size = SOC_DCMN_FABRIC_MAX_NOF_PIPES;
        link_config.pcp = 0;
        if (!SOC_IS_FE1600(unit) && !SOC_IS_DPP(unit))
        {
            rv = bcm_fabric_link_remote_pipe_mapping_get(unit, link, &link_config.pipe_mapping);
            if ((BCM_FAILURE(rv))) {
                cli_out("ERROR: bcm_fabric_link_remote_pipe_mapping_get failed\n");
                return CMD_FAIL;
            }

            rv = bcm_fabric_link_control_get(unit, link, bcmFabricLinkPcpEnable, &link_config.pcp);
            if ((BCM_FAILURE(rv))) {
                cli_out("ERROR: bcm_fabric_link_control_get(bcmFabricLinkPcpEnable) failed\n");
                return CMD_FAIL;
            }
        } else if (SOC_IS_JERICHO(unit)) { 
#ifdef BCM_JERICHO_SUPPORT
            link_config.pcp = SOC_DPP_CONFIG(unit)->arad->init.fabric.fabric_pcp_enable;
#endif
        }

        link_config.clk_freq = SOC_INFO(unit).port_refclk_int[link];

        /*convert to string*/

        /*pcs string*/
        switch (link_config.pcs)
        {
           case bcmPortPCS64b66b:
               pcs = "64_66";
               break;
           case bcmPortPCS64b66bBec:
               pcs = "BEC";
               break;
           case bcmPortPCS64b66bFec:
               pcs = "KR_FEC";
               break;
           case bcmPortPCS8b10b:
               pcs = "8_10";
               break;
           case bcmPortPCS8b9bLFec:
               pcs = "LFEC";
               break;
           case bcmPortPCS64b66bRsFec:
               pcs = "RS_FEC";
               break;
           case bcmPortPCS64b66bLowLatencyRsFec:
               pcs = "LL_RS_FEC";
               break;
           case bcmPortPCS64b66b15TRsFec:
               pcs = "15T_RS_FEC";
               break;
           case bcmPortPCS64b66b15TLowLatencyRsFec:
               pcs = "15T_LL_RS_FEC";
               break;
           default:
               pcs = "unknown";
               break;
        }
        sal_sprintf(pcs_str, "%-9s%s%s%s", 
                    pcs, 
                    link_config.pcs_llfc_extract_cig ? " (+LlfcCig)" : "           ",
                    link_config.pcs_error_detect ? " (+ErrInd)" : "          ",
                    link_config.pcs_llfc_low_latency ? " (+LlfcLat)" : "           ");

        /*pipe mapping string*/
#ifdef BCM_DFE_SUPPORT
        if (SOC_IS_DFE(unit) && SOC_DFE_IS_REPEATER(unit))
        {
            sal_sprintf(pipe_mapping_buf, "      %d   ", link_config.pipe_mapping.num_of_remote_pipes);
            pipe_mapping_str = pipe_mapping_buf;
        }
        else
#endif
        
        {
            if (link_config.pipe_mapping.num_of_remote_pipes == 0) 
            {
                /*no mapping*/
                pipe_mapping_str = "   None   ";
            } else if (link_config.pipe_mapping.num_of_remote_pipes == 1) {
                sal_sprintf(pipe_mapping_buf, "   0->%d   ", link_config.pipe_mapping.remote_pipe_mapping[0]);
                pipe_mapping_str = pipe_mapping_buf;
            } else if (link_config.pipe_mapping.num_of_remote_pipes == 2) {
                sal_sprintf(pipe_mapping_buf, "0->%d, 1->%d", link_config.pipe_mapping.remote_pipe_mapping[0], link_config.pipe_mapping.remote_pipe_mapping[1]);
                pipe_mapping_str = pipe_mapping_buf;
            } else {
                cli_out("ERROR: convert pipe mapping to string\n");
                return CMD_FAIL;
            }
        }


        cli_out("%03d    |   %1d    | %05d |    %3d    |  %1d   | %s |   %1d  |  %s  | rx:%1d, tx:%1d | %03d \n",
                link, link_config.enable, link_config.speed, link_config.clk_freq, link_config.cl72 ? 1 : 0, pcs_str, link_config.pcp, pipe_mapping_str, link_config.rx_polarity, link_config.tx_polarity, link_config.lane_swap);

    }

    return CMD_OK;
}

/*
 * Diag Fabric Mesh
 */
void
diag_dnx_fabric_mesh_usage(int unit)
{
    cli_out("'fabric mesh'- display current topology mapping\n");
}

cmd_result_t
diag_dnx_fabric_mesh(int unit, args_t *a) {

#ifdef BCM_PETRA_SUPPORT
   int rc, i;
   soc_dpp_fabric_topology_status_t stat;
   char* mesh_type;

   rc = soc_dpp_fabric_topology_status_get(unit, &stat);
   if (BCM_FAILURE(rc)) {
      cli_out("getting fabric topology status failed with status %s\n", bcm_errmsg(rc));
      return CMD_FAIL;
   }

   mesh_type = (stat.is_mesh_mc) ? "Mesh_MC" : "Mesh";

   cli_out(" Fabric Mesh Type: %s\n", mesh_type);
   cli_out(" --------------------------\n");
   cli_out("   Group Dest  |        FAP IDs         |             Links             \n");
   cli_out(" -----------------------------------------------------------------------\n");
   
   cli_out("       0       |");
   if (stat.fap_id_group0_count) {
       for (i = 0; i < stat.fap_id_group0_count; ++i) {
           cli_out("    %04d    ", stat.fap_id_group0_array[i]);
       }
       cli_out("|      ");
       for (i = 0; i < stat.links_group0_count; ++i) {
           cli_out(" %02d ", stat.links_group0_array[i]);
       }
       cli_out("\n");
   } else {
       cli_out("           -            |               -               \n");
   }

   cli_out("       1       |");
   if (stat.fap_id_group1_count) {
       for (i = 0; i < stat.fap_id_group1_count; ++i) {
           cli_out("    %04d    ", stat.fap_id_group1_array[i]);
       }
       cli_out("|      ");
       for (i = 0; i < stat.links_group1_count; ++i) {
           cli_out(" %02d ", stat.links_group1_array[i]);
       }
       cli_out("\n");
   } else {
       cli_out("           -            |               -               \n");
   }

   if (!stat.is_mesh_mc) {
       cli_out("       2       |"); 
       if (stat.fap_id_group2_count) {
           for (i = 0; i < stat.fap_id_group2_count; ++i) {
               cli_out("    %04d    ", stat.fap_id_group2_array[i]);
           }
           cli_out("|      ");
           for (i = 0; i < stat.links_group2_count; ++i) {
               cli_out(" %02d ", stat.links_group2_array[i]);
           }
           cli_out("\n");
       } else {
           cli_out("           -            |               -               \n");
       }
   }
#endif /*BCM_PETRA_SUPPORT*/
   return CMD_OK;
}

void
diag_dnx_fabric_traffic_profile_usage(int unit)
{
    cli_out("'fabric traffic_profile [dest_id=<dest_id>] [source_id=<source_id>]'\n");
    cli_out("               display which traffic cast and priorities sent through the fabric\n");
    cli_out("               can be displayed for a specific source_id and/or dest_id\n");
}

cmd_result_t
diag_dnx_fabric_traffic_profile(int unit, args_t *a) 
{
#ifdef BCM_DFE_SUPPORT
    int rv;
    soc_dfe_diag_fabric_traffic_profile_t traffic_profile;
    int source_id = -1, dest_id = -1, priority;
    char dest_id_buf[10], source_id_buf[10];
    parse_table_t pt;

    /*Parse input*/
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "dest_id", PQ_DFL|PQ_INT, &dest_id, &dest_id, NULL);
    parse_table_add(&pt, "source_id", PQ_DFL|PQ_INT, &source_id, &source_id, NULL);
    if (parse_arg_eq(a, &pt) < 0) 
    {
        cli_out("invalid option: %s\n", ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }

    /*Get traffic info*/
    rv = soc_dfe_diag_fabric_traffic_profile_get(unit, source_id, dest_id, &traffic_profile);
    if (BCM_FAILURE(rv))
    {
      cli_out("getting fabric traffic_profile info failed %s\n", bcm_errmsg(rv));
      return CMD_FAIL;
    }

    /*Print traffic info*/
    if (dest_id != -1)
    {
        sal_snprintf(dest_id_buf, 10, "%d", dest_id);
    }
    if (source_id != -1)
    {
        sal_snprintf(source_id_buf, 10, "%d", source_id);
    }

    cli_out("Traffic Info:\n");
    cli_out("-------------\n");
    cli_out("Checked for cells with source_id = %s and dest_id = %s\n", 
            source_id != -1 ? source_id_buf : "any",
            dest_id != -1 ? dest_id_buf : "any");

    /*Unicast printout*/
    for (priority = 0; priority < soc_dfe_fabric_priority_nof; priority++)
    {
        cli_out("Unicast priorty %d: %s\n", priority, traffic_profile.unicast[priority] ? "yes" : "no");
    }

    /*Multicast printout*/
    for (priority = 0; priority < soc_dfe_fabric_priority_nof; priority++)
    {
        cli_out("Multicast priority %d: %s\n", priority, traffic_profile.multicast[priority] ? "yes" : "no");
    }


#endif /*BCM_DFE_SUPPORT*/

    return CMD_OK;
}

void
diag_dnx_fabric_mesh_topology_usage(int unit)
{
    cli_out("'fabric mesh_topology' - internal diagnostic of mesh_topology block'\n");
    
}

cmd_result_t
diag_dnx_fabric_mesh_topology(int unit, args_t *a) 
{
    int rv, i;
    soc_dcmn_fabric_mesh_topology_diag_t mesh_topology_diag;

    /*Get traffic info*/
    rv = soc_dcmn_fabric_mesh_topology_diag_get(unit, &mesh_topology_diag);
    if (BCM_FAILURE(rv))
    {
      cli_out("getting mesh topology diag failed %s\n", bcm_errmsg(rv));
      return CMD_FAIL;
    }

    /*Print mesh topology diag*/
    cli_out("MESH_TOPOLOGY diag:\n");
    cli_out("===================\n");

    cli_out("\n");
    cli_out("indications:\n");
    cli_out("------------\n");
    cli_out("indication 1 :: %u\n", mesh_topology_diag.indication_1);
    cli_out("indication 2 :: %u\n", mesh_topology_diag.indication_2);
    cli_out("indication 3 :: %u\n", mesh_topology_diag.indication_3);
    cli_out("indication 4 :: %u\n", mesh_topology_diag.indication_4);

    cli_out("\n");
    cli_out("counters:\n");
    cli_out("------------\n");
    cli_out("rx_control_cells_type1 :: %u\n", mesh_topology_diag.rx_control_cells_type1);
    cli_out("rx_control_cells_type2 :: %u\n", mesh_topology_diag.rx_control_cells_type2);
    cli_out("control_cells_type3 :: %u\n", mesh_topology_diag.control_cells_type3);

    cli_out("type2_list ::");
    for (i = 0; i < mesh_topology_diag.link_list_count; i++) {
        if ((i % 5) == 0)  cli_out("\n\t");
        if (mesh_topology_diag.type2_list[i] != -1) {
            cli_out("%02d - 0x%08x, ", i, mesh_topology_diag.type2_list[i]);
        } else {
            cli_out("%02d - N/A       , ", i);
        }
    }
    cli_out("\n");

    cli_out("\n");
    cli_out("status 1:\n");
    cli_out("------------\n");
    cli_out("status_1:: %u\n", mesh_topology_diag.status_1);
    cli_out("status_1_id1:: %u\n", mesh_topology_diag.status_1_id1);
    cli_out("status_1_id2 :: %u\n", mesh_topology_diag.status_1_id2);

    cli_out("\n");
    cli_out("status 2:\n");
    cli_out("------------\n");
    cli_out("status_2:: %u\n", mesh_topology_diag.status_2);
    cli_out("status_2_id1:: %u\n", mesh_topology_diag.status_2_id1);
    cli_out("status_2_id2 :: %u\n", mesh_topology_diag.status_2_id2);

    cli_out("===================\n");

    return CMD_OK;
}

void
diag_dnx_fabric_rx_fifo_usage(int unit)
{
    cli_out("'fabric rx_fifo' - internal fabric rx fifo's status'\n");
    
}


cmd_result_t
diag_dnx_fabric_rx_fifo(int unit, args_t *a) 
{
#ifdef BCM_PETRA_SUPPORT
    if (SOC_IS_DPP(unit)) {
        int rv; 
        soc_dpp_fabric_rx_fifo_diag_t rx_fifo_diag;

        /*Get traffic info*/
        rv = soc_dpp_fabric_rx_fifo_diag_get(unit, &rx_fifo_diag);
        if (BCM_FAILURE(rv))
        {
          cli_out("getting rx fifo diag failed %s\n", bcm_errmsg(rv));
          return CMD_FAIL;
        }

        /*Print mesh topology diag*/
        cli_out("RX FIFO diag:\n");

        cli_out("\n");
        cli_out("EGQ 0:\n");
        cli_out("------------\n");
        cli_out("pipe 1 :: %u\n", rx_fifo_diag.soc_dpp_fabric_egq0_pipe1_watermark);
        cli_out("pipe 2 :: %u\n", rx_fifo_diag.soc_dpp_fabric_egq0_pipe2_watermark);
        cli_out("pipe 3 :: %u\n", rx_fifo_diag.soc_dpp_fabric_egq0_pipe3_watermark);
        
        cli_out("\n");
        cli_out("EGQ 1:\n");
        cli_out("------------\n");
        cli_out("pipe 1 :: %u\n", rx_fifo_diag.soc_dpp_fabric_egq1_pipe1_watermark);
        cli_out("pipe 2 :: %u\n", rx_fifo_diag.soc_dpp_fabric_egq1_pipe2_watermark);
        cli_out("pipe 3 :: %u\n", rx_fifo_diag.soc_dpp_fabric_egq1_pipe3_watermark);

    }
#endif
    return CMD_OK;
}


void
diag_dnx_fabric_force_usage(int unit)
{
    if (SOC_IS_KALIA(unit)){
        cli_out("'fabric force <fabric/local/default>' - Force mapping packets to fabric/local, regardless their destination IDs. Not to be done when traffic is enabled.\nTo disable traffic, use bcm_stk_module_enable and bcm_fabric_control_set with bcmFabricControlCellsEnable parameter.\n");
    }else{
        cli_out("'fabric force <fabric/core0/core1/default>' - Map packets with all destination IDs to fabric/local core 0/local core 1. Not to be done when traffic is enabled.\nTo disable traffic, use bcm_stk_module_enable and bcm_fabric_control_set with bcmFabricControlCellsEnable parameter.\n");
    }
}


cmd_result_t
diag_dnx_fabric_force(int unit, args_t *a) 
{
#ifdef BCM_PETRA_SUPPORT
    if (SOC_IS_DPP(unit)) {
        int rv; 
        soc_dpp_fabric_force_t force = socDppFabricForceRestore;
        char *dest_str = NULL;
        int strnlen_dest_str;

        dest_str = ARG_GET(a);

        if (dest_str == NULL)
        {
            cli_out("Destination input missing\n");
            return CMD_USAGE;
        }
        /*
         * Make sure at least one of the strings is NULL terminated.
         */
        strnlen_dest_str = strnlen(dest_str,SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
        if (!sal_strncasecmp(dest_str, "fabric",strnlen_dest_str)) {
            force = socDppFabricForceFabric;
        } else if(!sal_strncasecmp(dest_str, "core0",strnlen_dest_str)) {
                   force = socDppFabricForceLocal0;
        } else if (!sal_strncasecmp(dest_str, "core1",strnlen_dest_str)) {
                   force = socDppFabricForceLocal1;
        } else if (!sal_strncasecmp(dest_str, "local",strnlen_dest_str)) {
                   force = socDppFabricForceLocal;
        } else if (!sal_strncasecmp(dest_str, "default",strnlen_dest_str)) {
                   force = socDppFabricForceRestore;
        } else {
            force = socDppFabricForceNotSupported;
        }

        rv = soc_dpp_fabric_force_set(unit, force);
        if (BCM_FAILURE(rv))
        {
          cli_out("setting force local/fabric failed %s\n", bcm_errmsg(rv));
          return CMD_FAIL;
        }

    }
#endif
    return CMD_OK;
}



#undef _ERR_MSG_MODULE_NAME

