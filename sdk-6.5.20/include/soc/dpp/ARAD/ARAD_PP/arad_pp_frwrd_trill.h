
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_PP_FRWRD_TRILL_INCLUDED__

#define __ARAD_PP_FRWRD_TRILL_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_llp_sa_auth.h>
#include <soc/dpp/ARAD/arad_reg_access.h>






#define ARAD_PP_TRILL_SA_AUTH_ENTRY_IS_DYNAMIC (FALSE)
#define ARAD_PP_TRILL_SA_AUTH_ENTRY_AGE (3)

#define SOC_DPP_PP_FRWRD_TRILL_EEDB_INVALID_ENTRY(unit) ((1 << soc_reg_field_length(unit, EGQ_TRILL_CONFIGr, TRILL_OUT_LIF_BRIDGEf)) - 1)
#define ARAD_PP_FRWRD_TRILL_EEDB_INVALID_ENTRY_B0       0










typedef enum
{
  
  ARAD_PP_FRWRD_TRILL_MULTICAST_KEY_MASK_SET = ARAD_PP_PROC_DESC_BASE_FRWRD_TRILL_FIRST,
  ARAD_PP_FRWRD_TRILL_MULTICAST_KEY_MASK_SET_PRINT,
  ARAD_PP_FRWRD_TRILL_MULTICAST_KEY_MASK_SET_UNSAFE,
  ARAD_PP_FRWRD_TRILL_MULTICAST_KEY_MASK_SET_VERIFY,
  ARAD_PP_FRWRD_TRILL_MULTICAST_KEY_MASK_GET,
  ARAD_PP_FRWRD_TRILL_MULTICAST_KEY_MASK_GET_PRINT,
  ARAD_PP_FRWRD_TRILL_MULTICAST_KEY_MASK_GET_VERIFY,
  ARAD_PP_FRWRD_TRILL_MULTICAST_KEY_MASK_GET_UNSAFE,
  ARAD_PP_FRWRD_TRILL_UNICAST_ROUTE_ADD,
  ARAD_PP_FRWRD_TRILL_UNICAST_ROUTE_ADD_PRINT,
  ARAD_PP_FRWRD_TRILL_UNICAST_ROUTE_ADD_UNSAFE,
  ARAD_PP_FRWRD_TRILL_UNICAST_ROUTE_ADD_VERIFY,
  ARAD_PP_FRWRD_TRILL_UNICAST_ROUTE_GET,
  ARAD_PP_FRWRD_TRILL_UNICAST_ROUTE_GET_PRINT,
  ARAD_PP_FRWRD_TRILL_UNICAST_ROUTE_GET_UNSAFE,
  ARAD_PP_FRWRD_TRILL_UNICAST_ROUTE_GET_VERIFY,
  ARAD_PP_FRWRD_TRILL_UNICAST_ROUTE_REMOVE,
  ARAD_PP_FRWRD_TRILL_UNICAST_ROUTE_REMOVE_PRINT,
  ARAD_PP_FRWRD_TRILL_UNICAST_ROUTE_REMOVE_UNSAFE,
  ARAD_PP_FRWRD_TRILL_UNICAST_ROUTE_REMOVE_VERIFY,
  ARAD_PP_FRWRD_TRILL_MULTICAST_ROUTE_ADD,
  ARAD_PP_FRWRD_TRILL_MULTICAST_ROUTE_ADD_PRINT,
  ARAD_PP_FRWRD_TRILL_MULTICAST_ROUTE_ADD_UNSAFE,
  ARAD_PP_FRWRD_TRILL_MULTICAST_ROUTE_ADD_VERIFY,
  ARAD_PP_FRWRD_TRILL_MULTICAST_ROUTE_GET,
  ARAD_PP_FRWRD_TRILL_MULTICAST_ROUTE_GET_PRINT,
  ARAD_PP_FRWRD_TRILL_MULTICAST_ROUTE_GET_UNSAFE,
  ARAD_PP_FRWRD_TRILL_MULTICAST_ROUTE_GET_VERIFY,
  ARAD_PP_FRWRD_TRILL_MULTICAST_ROUTE_REMOVE,
  ARAD_PP_FRWRD_TRILL_MULTICAST_ROUTE_REMOVE_PRINT,
  ARAD_PP_FRWRD_TRILL_MULTICAST_ROUTE_REMOVE_UNSAFE,
  ARAD_PP_FRWRD_TRILL_MULTICAST_ROUTE_REMOVE_VERIFY,
  ARAD_PP_FRWRD_TRILL_ADJ_INFO_SET,
  ARAD_PP_FRWRD_TRILL_ADJ_INFO_SET_PRINT,
  ARAD_PP_FRWRD_TRILL_ADJ_INFO_SET_UNSAFE,
  ARAD_PP_FRWRD_TRILL_ADJ_INFO_SET_VERIFY,
  ARAD_PP_FRWRD_TRILL_ADJ_INFO_GET,
  ARAD_PP_FRWRD_TRILL_ADJ_INFO_GET_PRINT,
  ARAD_PP_FRWRD_TRILL_ADJ_INFO_GET_VERIFY,
  ARAD_PP_FRWRD_TRILL_ADJ_INFO_GET_UNSAFE,
  SOC_PPC_FRWRD_TRILL_GLOBAL_INFO_SET,
  SOC_PPC_FRWRD_TRILL_GLOBAL_INFO_SET_PRINT,
  SOC_PPC_FRWRD_TRILL_GLOBAL_INFO_SET_UNSAFE,
  SOC_PPC_FRWRD_TRILL_GLOBAL_INFO_SET_VERIFY,
  SOC_PPC_FRWRD_TRILL_GLOBAL_INFO_GET,
  SOC_PPC_FRWRD_TRILL_GLOBAL_INFO_GET_PRINT,
  SOC_PPC_FRWRD_TRILL_GLOBAL_INFO_GET_VERIFY,
  SOC_PPC_FRWRD_TRILL_GLOBAL_INFO_GET_UNSAFE,
  ARAD_PP_FRWRD_TRILL_GET_PROCS_PTR,
  ARAD_PP_FRWRD_TRILL_GET_ERRS_PTR,
  ARAD_PP_FRWRD_TRILL_MULTICAST_SOURCE_ADD_UNSAFE,
  ARAD_PP_FRWRD_TRILL_MULTICAST_SOURCE_ADD_VERIFY,
  ARAD_PP_FRWRD_TRILL_MULTICAST_SOURCE_ADD,
  ARAD_PP_FRWRD_TRILL_MULTICAST_SOURCE_REMOVE,
  ARAD_PP_FRWRD_TRILL_MULTICAST_SOURCE_GET,
  ARAD_PP_FRWRD_TRILL_MULTICAST_SOURCE_REMOVE_UNSAFE,
  ARAD_PP_FRWRD_TRILL_MULTICAST_SOURCE_REMOVE_VERIFY,
  ARAD_PP_FRWRD_TRILL_MULTICAST_SOURCE_GET_VERIFY,
  ARAD_PP_FRWRD_TRILL_MULTICAST_SOURCE_GET_UNSAFE,
  ARAD_PP_FRWRD_TRILL_VSI_ENTRY_ADD,
  ARAD_PP_FRWRD_TRILL_VSI_ENTRY_ADD_VERIFY,
  ARAD_PP_FRWRD_TRILL_VSI_ENTRY_ADD_UNSAFE,
  ARAD_PP_FRWRD_TRILL_VSI_ENTRY_REMOVE,
  ARAD_PP_FRWRD_TRILL_VSI_ENTRY_REMOVE_VERIFY,
  ARAD_PP_FRWRD_TRILL_VSI_ENTRY_REMOVE_UNSAFE,
  ARAD_PP_FRWRD_TRILL_NATIVE_INNER_TPID_ADD,
  ARAD_PP_FRWRD_TRILL_NATIVE_INNER_TPID_ADD_UNSAFE,
  



  
  ARAD_PP_FRWRD_TRILL_PROCEDURE_DESC_LAST
} ARAD_PP_FRWRD_TRILL_PROCEDURE_DESC;

typedef enum
{
  
  ARAD_PP_FRWRD_TRILL_NICKNAME_KEY_OUT_OF_RANGE_ERR = ARAD_PP_ERR_DESC_BASE_FRWRD_TRILL_FIRST,
  ARAD_PP_FRWRD_TRILL_SUCCESS_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_TRILL_MC_ID_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_TRILL_TREE_NICK_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_TRILL_ING_NICK_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_TRILL_ADJACENT_EEP_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_TRILL_EXPECT_ADJACENT_EEP_OUT_OF_RANGE_ERR,
  ARAD_PP_FRWRD_TRILL_CFG_TTL_OUT_OF_RANGE_ERR,
  



  
  ARAD_PP_FRWRD_TRILL_ERR_LAST
} ARAD_PP_FRWRD_TRILL_ERR;









uint32
  arad_pp_frwrd_trill_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );

uint32
  arad_pp_l2_lif_trill_uc_get_internal_unsafe(
    SOC_SAND_IN   int                                unit,
    SOC_SAND_IN   uint32                             nickname,
    SOC_SAND_IN   uint8                              ignore_key,
    SOC_SAND_OUT  SOC_PPC_LIF_ID                    *lif_index,
    SOC_SAND_OUT  SOC_PPC_L2_LIF_TRILL_INFO         *trill_info,
    SOC_SAND_OUT  uint8                             *is_found
  );

uint32
  arad_pp_l2_lif_trill_add_internal_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                              nickname,
    SOC_SAND_IN  uint8                               ignore_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                      lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_TRILL_INFO           *trill_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE            *success
  );


uint32
  arad_pp_frwrd_trill_multicast_key_mask_set_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PPC_TRILL_MC_MASKED_FIELDS  *masked_fields
  );

uint32
  arad_pp_frwrd_trill_multicast_key_mask_set_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PPC_TRILL_MC_MASKED_FIELDS  *masked_fields
  );

uint32
  arad_pp_frwrd_trill_multicast_key_mask_get_verify(
    SOC_SAND_IN  int                     unit
  );


uint32
  arad_pp_frwrd_trill_multicast_key_mask_get_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_OUT SOC_PPC_TRILL_MC_MASKED_FIELDS  *masked_fields
  );


uint32
  arad_pp_frwrd_trill_unicast_route_add_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                       nickname_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_DECISION_INFO *fwd_decision,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success
  );

uint32
  arad_pp_frwrd_trill_unicast_route_add_verify(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  uint32                      nickname_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_DECISION_INFO *fwd_decision
  );


uint32
  arad_pp_frwrd_trill_unicast_route_get_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                     nickname_key,
    SOC_SAND_OUT SOC_PPC_FRWRD_DECISION_INFO *learn_info,
    SOC_SAND_OUT uint8                     *is_found
  );

uint32
  arad_pp_frwrd_trill_unicast_route_get_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                      nickname_key
  );


uint32
  arad_pp_frwrd_trill_unicast_route_remove_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                      nickname_key
  );

uint32
  arad_pp_frwrd_trill_unicast_route_remove_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                      nickname_key
  );


uint32
  arad_pp_frwrd_trill_ingress_lif_add_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_LIF_ID               lif_index,
    SOC_SAND_IN  uint32                       nickname_key,
    SOC_SAND_IN  SOC_PPC_L2_LIF_TRILL_INFO   *trill_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success
  );

uint32
  arad_pp_frwrd_trill_ingress_lif_add_verify(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  SOC_PPC_LIF_ID              lif_index,
    SOC_SAND_IN  uint32                      nickname_key,
    SOC_SAND_IN  SOC_PPC_L2_LIF_TRILL_INFO  *trill_info
  );


uint32
  arad_pp_frwrd_trill_ingress_lif_get_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                     nickname_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID            *lif_index,
    SOC_SAND_OUT SOC_PPC_L2_LIF_TRILL_INFO *trill_info,
    SOC_SAND_OUT uint8                     *is_found
  );

uint32
  arad_pp_frwrd_trill_ingress_lif_get_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                      nickname_key
  );


uint32
  arad_pp_frwrd_trill_ingress_lif_remove_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                      nickname_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                  *lif_index
  );

uint32
  arad_pp_frwrd_trill_ingress_lif_remove_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                      nickname_key
  );



uint32
  arad_pp_frwrd_trill_multicast_route_add_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PPC_TRILL_MC_ROUTE_KEY      *trill_mc_key,
    SOC_SAND_IN  uint32                      mc_id,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE          *success
  );

uint32
  arad_pp_frwrd_trill_multicast_route_add_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PPC_TRILL_MC_ROUTE_KEY      *trill_mc_key,
    SOC_SAND_IN  uint32                      mc_id
  );


uint32
  arad_pp_frwrd_trill_multicast_route_get_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PPC_TRILL_MC_ROUTE_KEY      *trill_mc_key,
    SOC_SAND_OUT uint32                      *mc_id,
    SOC_SAND_OUT uint8                     *is_found
  );

uint32
  arad_pp_frwrd_trill_multicast_route_get_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PPC_TRILL_MC_ROUTE_KEY      *trill_mc_key
  );


uint32
  arad_pp_frwrd_trill_multicast_route_remove_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PPC_TRILL_MC_ROUTE_KEY      *trill_mc_key
  );

uint32
  arad_pp_frwrd_trill_multicast_route_remove_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PPC_TRILL_MC_ROUTE_KEY      *trill_mc_key
  );


uint32
  arad_pp_frwrd_trill_adj_info_set_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS           *mac_address_key,
    SOC_SAND_IN  SOC_PPC_TRILL_ADJ_INFO          *mac_auth_info,
    SOC_SAND_IN  uint8                     enable,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE          *success
  );

uint32
  arad_pp_frwrd_trill_adj_info_set_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS           *mac_address_key,
    SOC_SAND_IN  SOC_PPC_TRILL_ADJ_INFO          *mac_auth_info,
    SOC_SAND_IN  uint8                     enable
  );

uint32
  arad_pp_frwrd_trill_adj_info_get_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS           *mac_address_key
  );


uint32
  arad_pp_frwrd_trill_adj_info_get_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS           *mac_address_key,
    SOC_SAND_OUT SOC_PPC_TRILL_ADJ_INFO          *mac_auth_info,
    SOC_SAND_OUT uint8                     *enable
  );


uint32
  arad_pp_frwrd_trill_global_info_set_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_TRILL_GLOBAL_INFO *glbl_info
  );

uint32
  arad_pp_frwrd_trill_global_info_set_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_TRILL_GLOBAL_INFO *glbl_info
  );

uint32
  arad_pp_frwrd_trill_global_info_get_verify(
    SOC_SAND_IN  int                     unit
  );


uint32
  arad_pp_frwrd_trill_global_info_get_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_TRILL_GLOBAL_INFO *glbl_info
  );


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_frwrd_trill_get_procs_ptr(void);


CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_frwrd_trill_get_errs_ptr(void);

uint32
  SOC_PPC_TRILL_MC_MASKED_FIELDS_verify(
    SOC_SAND_IN  SOC_PPC_TRILL_MC_MASKED_FIELDS *info
  );

uint32
  SOC_PPC_TRILL_MC_ROUTE_KEY_verify(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PPC_TRILL_MC_ROUTE_KEY *info
  );

uint32
  SOC_PPC_TRILL_ADJ_INFO_verify(
    SOC_SAND_IN  SOC_PPC_TRILL_ADJ_INFO *info
  );

uint32
  SOC_PPC_FRWRD_TRILL_GLOBAL_INFO_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_TRILL_GLOBAL_INFO *info
  );

uint32
arad_pp_frwrd_trill_vsi_entry_add_unsafe(
   SOC_SAND_IN  int                                 unit,
   SOC_SAND_IN  uint32                                 vsi,
   SOC_SAND_IN  uint32                                 flags,
   SOC_SAND_IN  uint32                                 high_vid,
   SOC_SAND_IN  uint32                                 low_vid,
   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE               *success);
uint32
  arad_pp_frwrd_trill_vsi_entry_add_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     vsi,
    SOC_SAND_IN  uint32                     high_vid,
    SOC_SAND_IN  uint32                     low_vid
  );
uint32
arad_pp_frwrd_trill_vsi_entry_remove_unsafe(
   SOC_SAND_IN  int                                 unit,
   SOC_SAND_IN  uint32                                 vsi,
   SOC_SAND_IN  uint32                                 flags,
   SOC_SAND_IN  uint32                                 high_vid,
   SOC_SAND_IN  uint32                                 low_vid);

uint32
  arad_pp_frwrd_trill_vsi_entry_remove_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     vsi,
    SOC_SAND_IN  uint32                     high_vid,
    SOC_SAND_IN  uint32                     low_vid
  );

uint32
arad_pp_frwrd_trill_native_inner_tpid_add_unsafe(
   SOC_SAND_IN  int                                 unit,
   SOC_SAND_IN  uint32                                 tpid,
   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE               *success
 );
uint32
arad_pp_frwrd_trill_native_inner_tpid_add_verify(
   SOC_SAND_IN  int                     unit,
   SOC_SAND_IN  uint32                     tpid
 );

uint32
arad_pp_frwrd_trill_native_inner_tpid_remove_unsafe(
   SOC_SAND_IN  int                                 unit,
   SOC_SAND_IN  uint32                                 tpid
 );
uint32
arad_pp_frwrd_trill_native_inner_tpid_remove_verify(
   SOC_SAND_IN  int                     unit,
   SOC_SAND_IN  uint32                     tpid
 );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif



