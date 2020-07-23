/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPD_API_FRWRD_TRILL_INCLUDED__

#define __SOC_PPD_API_FRWRD_TRILL_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>

#include <soc/dpp/PPC/ppc_api_frwrd_trill.h>

#include <soc/dpp/PPD/ppd_api_general.h>
#include <soc/dpp/PPD/ppd_api_lif.h>






#define SOC_PPD_TRILL_VSI_TRANSPARENT_SERVICE  0x00000001 











typedef enum
{
  
  SOC_PPD_FRWRD_TRILL_MULTICAST_KEY_MASK_SET = SOC_PPD_PROC_DESC_BASE_FRWRD_TRILL_FIRST,
  SOC_PPD_FRWRD_TRILL_MULTICAST_KEY_MASK_SET_PRINT,
  SOC_PPD_FRWRD_TRILL_MULTICAST_KEY_MASK_GET,
  SOC_PPD_FRWRD_TRILL_MULTICAST_KEY_MASK_GET_PRINT,
  SOC_PPD_FRWRD_TRILL_UNICAST_ROUTE_ADD,
  SOC_PPD_FRWRD_TRILL_UNICAST_ROUTE_ADD_PRINT,
  SOC_PPD_FRWRD_TRILL_UNICAST_ROUTE_GET,
  SOC_PPD_FRWRD_TRILL_UNICAST_ROUTE_GET_PRINT,
  SOC_PPD_FRWRD_TRILL_UNICAST_ROUTE_REMOVE,
  SOC_PPD_FRWRD_TRILL_UNICAST_ROUTE_REMOVE_PRINT,
  SOC_PPD_FRWRD_TRILL_MULTICAST_ROUTE_ADD,
  SOC_PPD_FRWRD_TRILL_MULTICAST_ROUTE_ADD_PRINT,
  SOC_PPD_FRWRD_TRILL_MULTICAST_ROUTE_GET,
  SOC_PPD_FRWRD_TRILL_MULTICAST_ROUTE_GET_PRINT,
  SOC_PPD_FRWRD_TRILL_MULTICAST_ROUTE_REMOVE,
  SOC_PPD_FRWRD_TRILL_MULTICAST_ROUTE_REMOVE_PRINT,
  SOC_PPD_FRWRD_TRILL_ADJ_INFO_SET,
  SOC_PPD_FRWRD_TRILL_ADJ_INFO_SET_PRINT,
  SOC_PPD_FRWRD_TRILL_ADJ_INFO_GET,
  SOC_PPD_FRWRD_TRILL_ADJ_INFO_GET_PRINT,
  SOC_PPD_FRWRD_TRILL_GLOBAL_INFO_SET,
  SOC_PPD_FRWRD_TRILL_GLOBAL_INFO_SET_PRINT,
  SOC_PPD_FRWRD_TRILL_GLOBAL_INFO_GET,
  SOC_PPD_FRWRD_TRILL_GLOBAL_INFO_GET_PRINT,
  SOC_PPD_FRWRD_TRILL_GET_PROCS_PTR,
  SOC_PPD_FRWRD_TRILL_MULTICAST_SOURCE_ADD,
  SOC_PPD_FRWRD_TRILL_MULTICAST_SOURCE_REMOVE,
  SOC_PPD_FRWRD_TRILL_MULTICAST_SOURCE_GET,
  SOC_PPD_FRWRD_TRILL_NATIVE_INNER_TPID_ADD,
  SOC_PPD_FRWRD_TRILL_VSI_ENTRY_ADD,
  SOC_PPD_FRWRD_TRILL_VSI_ENTRY_REMOVE,
  



  
  SOC_PPD_FRWRD_TRILL_PROCEDURE_DESC_LAST
} SOC_PPD_FRWRD_TRILL_PROCEDURE_DESC;


typedef enum
{
    SOC_PPD_TRILL_MODE_DISABLED = 0,
    SOC_PPD_TRILL_MODE_VL = 1,
    SOC_PPD_TRILL_MODE_FGL = 2
} SOC_PPD_TRILL_MODE;










uint32
  soc_ppd_frwrd_trill_multicast_key_mask_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PPC_TRILL_MC_MASKED_FIELDS  *masked_fields
  );


uint32
  soc_ppd_frwrd_trill_multicast_key_mask_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT SOC_PPC_TRILL_MC_MASKED_FIELDS  *masked_fields
  );


uint32
  soc_ppd_frwrd_trill_unicast_route_add(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  uint32                      nickname_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_DECISION_INFO *learn_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE   *success
  );


uint32
  soc_ppd_frwrd_trill_unicast_route_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                     nickname_key,
    SOC_SAND_OUT SOC_PPC_FRWRD_DECISION_INFO *learn_info,
    SOC_SAND_OUT uint8                      *is_found
  );


uint32
  soc_ppd_frwrd_trill_unicast_route_remove(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                    nickname_key
  );


uint32
  soc_ppd_frwrd_trill_ingress_lif_add(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  SOC_PPC_LIF_ID              lif_index,
    SOC_SAND_IN  uint32                      nickname_key,
    SOC_SAND_IN  SOC_PPC_L2_LIF_TRILL_INFO  *trill_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE   *success
  );


uint32
  soc_ppd_frwrd_trill_ingress_lif_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                     nickname_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID            *lif_index,
    SOC_SAND_OUT SOC_PPC_L2_LIF_TRILL_INFO *trill_info,
    SOC_SAND_OUT uint8                      *is_found
  );


uint32
  soc_ppd_frwrd_trill_ingress_lif_remove(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                    nickname_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                  *lif_index
  );


uint32
  soc_ppd_frwrd_trill_multicast_route_add(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PPC_TRILL_MC_ROUTE_KEY      *trill_mc_key,
    SOC_SAND_IN  uint32                    mc_id,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE        *success
  );


uint32
  soc_ppd_frwrd_trill_multicast_route_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PPC_TRILL_MC_ROUTE_KEY      *trill_mc_key,
    SOC_SAND_OUT uint32                    *mc_id,
    SOC_SAND_OUT uint8                   *is_found
  );


uint32
  soc_ppd_frwrd_trill_multicast_route_remove(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PPC_TRILL_MC_ROUTE_KEY      *trill_mc_key
  );


uint32
  soc_ppd_frwrd_trill_adj_info_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS         *mac_address_key,
    SOC_SAND_IN  SOC_PPC_TRILL_ADJ_INFO          *mac_auth_info,
    SOC_SAND_IN  uint8                   enable,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE        *success
  );


uint32
  soc_ppd_frwrd_trill_adj_info_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS         *mac_address_key,
    SOC_SAND_OUT SOC_PPC_TRILL_ADJ_INFO          *mac_auth_info,
    SOC_SAND_OUT uint8                   *enable
  );


uint32
  soc_ppd_frwrd_trill_global_info_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_TRILL_GLOBAL_INFO *glbl_info
  );


uint32
  soc_ppd_frwrd_trill_global_info_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_TRILL_GLOBAL_INFO *glbl_info
  );

uint32
  soc_ppd_frwrd_trill_native_inner_tpid_add(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  uint32                             tpid,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE           *success
  );

uint32
  soc_ppd_frwrd_trill_vsi_entry_add(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     vsi,
    SOC_SAND_IN  uint32                     flags,
    SOC_SAND_IN  uint32                     high_vid,
    SOC_SAND_IN  uint32                     low_vid,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE   *success
  );
uint32
  soc_ppd_frwrd_trill_vsi_entry_remove(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     vsi,
    SOC_SAND_IN  uint32                     flags,
    SOC_SAND_IN  uint32                     high_vid,
    SOC_SAND_IN  uint32                     low_vid
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

