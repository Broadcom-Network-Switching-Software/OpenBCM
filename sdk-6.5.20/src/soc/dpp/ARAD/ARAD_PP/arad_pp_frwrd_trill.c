
#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FORWARD



#include <shared/bsl.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/PPD/ppd_api_frwrd_trill.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_trill.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>

#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lem_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_isem_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lif.h>





#define ARAD_PP_FRWRD_TRILL_NICKNAME_KEY_MAX                     (SOC_SAND_U32_MAX)
#define ARAD_PP_FRWRD_TRILL_MC_ID_MAX                            (SOC_SAND_U32_MAX)
#define ARAD_PP_FRWRD_TRILL_TREE_NICK_MAX                        (SOC_SAND_U32_MAX)
#define ARAD_PP_FRWRD_TRILL_ING_NICK_MAX                         (SOC_SAND_U32_MAX)
#define ARAD_PP_FRWRD_TRILL_ADJACENT_EEP_MAX                     (SOC_SAND_U32_MAX)
#define ARAD_PP_FRWRD_TRILL_EXPECT_ADJACENT_EEP_MAX              (SOC_SAND_U32_MAX)
#define ARAD_PP_FRWRD_TRILL_CFG_TTL_MAX                          (SOC_SAND_U32_MAX)

#define ARAD_PP_FRWRD_TRILL_TCAM_MIN_BANKS                       (1)
#define ARAD_PP_FRWRD_TRILL_PREFIX_SIZE                          (0)
#define ARAD_PP_FRWRD_TRILL_MC_TCAM_ID                           (0)


















CONST STATIC
  SOC_PROCEDURE_DESC_ELEMENT
    Arad_pp_procedure_desc_element_frwrd_trill[] =
{
  
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_TRILL_MULTICAST_KEY_MASK_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_TRILL_MULTICAST_KEY_MASK_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_TRILL_MULTICAST_KEY_MASK_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_TRILL_MULTICAST_KEY_MASK_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_TRILL_MULTICAST_KEY_MASK_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_TRILL_MULTICAST_KEY_MASK_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_TRILL_MULTICAST_KEY_MASK_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_TRILL_MULTICAST_KEY_MASK_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_TRILL_UNICAST_ROUTE_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_TRILL_UNICAST_ROUTE_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_TRILL_UNICAST_ROUTE_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_TRILL_UNICAST_ROUTE_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_TRILL_UNICAST_ROUTE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_TRILL_UNICAST_ROUTE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_TRILL_UNICAST_ROUTE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_TRILL_UNICAST_ROUTE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_TRILL_UNICAST_ROUTE_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_TRILL_UNICAST_ROUTE_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_TRILL_UNICAST_ROUTE_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_TRILL_UNICAST_ROUTE_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_TRILL_MULTICAST_ROUTE_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_TRILL_MULTICAST_ROUTE_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_TRILL_MULTICAST_ROUTE_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_TRILL_MULTICAST_ROUTE_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_TRILL_MULTICAST_ROUTE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_TRILL_MULTICAST_ROUTE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_TRILL_MULTICAST_ROUTE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_TRILL_MULTICAST_ROUTE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_TRILL_MULTICAST_ROUTE_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_TRILL_MULTICAST_ROUTE_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_TRILL_MULTICAST_ROUTE_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_TRILL_MULTICAST_ROUTE_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_TRILL_ADJ_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_TRILL_ADJ_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_TRILL_ADJ_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_TRILL_ADJ_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_TRILL_ADJ_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_TRILL_ADJ_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_TRILL_ADJ_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_TRILL_ADJ_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_TRILL_GLOBAL_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_TRILL_GLOBAL_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_TRILL_GLOBAL_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_TRILL_GLOBAL_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_TRILL_GLOBAL_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_TRILL_GLOBAL_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_TRILL_GLOBAL_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPC_FRWRD_TRILL_GLOBAL_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_TRILL_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_FRWRD_TRILL_GET_ERRS_PTR),
  



  
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC
SOC_ERROR_DESC_ELEMENT
    Arad_pp_error_desc_element_frwrd_trill[] =
{
  
  {
    ARAD_PP_FRWRD_TRILL_NICKNAME_KEY_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_TRILL_NICKNAME_KEY_OUT_OF_RANGE_ERR",
    "The parameter 'nickname_key' is out of range. \n\r "
    "The range is: No min - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_TRILL_SUCCESS_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_TRILL_SUCCESS_OUT_OF_RANGE_ERR",
    "The parameter 'success' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_NOF_SUCCESS_FAILURES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_TRILL_MC_ID_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_TRILL_MC_ID_OUT_OF_RANGE_ERR",
    "The parameter 'mc_id' is out of range. \n\r "
    "The range is: No min - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_TRILL_TREE_NICK_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_TRILL_TREE_NICK_OUT_OF_RANGE_ERR",
    "The parameter 'tree_nick' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_TRILL_ING_NICK_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_TRILL_ING_NICK_OUT_OF_RANGE_ERR",
    "The parameter 'ing_nick' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_TRILL_ADJACENT_EEP_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_TRILL_ADJACENT_EEP_OUT_OF_RANGE_ERR",
    "The parameter 'adjacent_eep' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_TRILL_EXPECT_ADJACENT_EEP_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_TRILL_EXPECT_ADJACENT_EEP_OUT_OF_RANGE_ERR",
    "The parameter 'expect_adjacent_eep' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_FRWRD_TRILL_CFG_TTL_OUT_OF_RANGE_ERR,
    "ARAD_PP_FRWRD_TRILL_CFG_TTL_OUT_OF_RANGE_ERR",
    "The parameter 'cfg_ttl' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_U32_MAX.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  



  
SOC_ERR_DESC_ELEMENT_DEF_LAST
};





STATIC
  uint32
    arad_pp_frwrd_mc_id_to_dest(
      SOC_SAND_IN  int      unit,
      SOC_SAND_IN  uint32      mc_id,
      SOC_SAND_OUT uint32      *dest,
      SOC_SAND_OUT uint32      *add_info
    )
{
  SOC_PPC_FRWRD_DECISION_INFO
    em_dest;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PPC_FRWRD_DECISION_INFO_clear(&em_dest);
  *add_info = 0;

  em_dest.type = SOC_PPC_FRWRD_DECISION_TYPE_MC;
  em_dest.dest_id = mc_id;
  em_dest.additional_info.eei.type = SOC_PPC_EEI_TYPE_EMPTY;

  res = arad_pp_fwd_decision_in_buffer_build(
          unit,
          ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_TRILL,
          &em_dest,
          dest,
          add_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_fec_to_em_dest()",0,0);
}

STATIC
  uint32
    arad_pp_frwrd_fec_to_dest(
      SOC_SAND_IN  int          unit,
      SOC_SAND_IN SOC_PPC_FEC_ID   fec_id,
      SOC_SAND_OUT uint32      *dest,
      SOC_SAND_OUT uint32      *add_info
    )
{
  SOC_PPC_FRWRD_DECISION_INFO
    em_dest;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PPC_FRWRD_DECISION_INFO_clear(&em_dest);
  *add_info = 0;

  em_dest.type = SOC_PPC_FRWRD_DECISION_TYPE_FEC;
  em_dest.dest_id = fec_id;
  em_dest.additional_info.eei.type = SOC_PPC_EEI_TYPE_EMPTY;

  res = arad_pp_fwd_decision_in_buffer_build(
          unit,
          ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_TRILL,
          &em_dest,
          dest,
          add_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_fec_to_em_dest()",0,0);
}

STATIC
  void
    arad_pp_frwrd_trill_uc_lem_payload_build(
      SOC_SAND_IN  int                         unit,
      SOC_SAND_IN  SOC_PPC_FEC_ID                 fec_id,
      SOC_SAND_OUT ARAD_PP_LEM_ACCESS_PAYLOAD     *payload
    )
{
  ARAD_PP_LEM_ACCESS_PAYLOAD_clear(payload);
   
  arad_pp_frwrd_fec_to_dest(
    unit,
    fec_id,
    &payload->dest,
    &payload->asd
  );
}



STATIC
  void
    arad_pp_frwrd_trill_mc_lem_payload_build(
      SOC_SAND_IN  int                       unit,
      SOC_SAND_IN  uint32                       mc_id,
      SOC_SAND_OUT ARAD_PP_LEM_ACCESS_PAYLOAD   *payload
    )
{
  ARAD_PP_LEM_ACCESS_PAYLOAD_clear(payload);
   
  arad_pp_frwrd_mc_id_to_dest(
    unit,
    mc_id,
    &payload->dest,
    &payload->asd
  );
}


STATIC
  void
    arad_pp_frwrd_trill_uc_lem_request_key_build(
       SOC_SAND_IN  int                                 unit,
      SOC_SAND_IN ARAD_PP_LEM_ACCESS_KEY_TYPE type,
      SOC_SAND_IN uint32 nickname,
      SOC_SAND_OUT ARAD_PP_LEM_ACCESS_KEY *key
    )
{
  uint32
    num_bits;

  ARAD_PP_LEM_ACCESS_KEY_clear(key);
  
  key->type = type;

  key->nof_params = ARAD_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_TRILL_UC;

  num_bits = ARAD_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_TRILL_UC;
  key->param[0].nof_bits = (uint8)num_bits;
  key->param[0].value[0] = nickname;

  key->prefix.nof_bits = ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_TRILL_UC;
  key->prefix.value = ARAD_PP_LEM_ACCESS_KEY_PREFIX_FOR_TRILL_UC;
}



uint32
    arad_pp_frwrd_trill_mc_lem_request_key_build(
      SOC_SAND_IN  int                  unit,
      SOC_SAND_IN  ARAD_PP_LEM_ACCESS_KEY_TYPE  type,
      SOC_SAND_IN  SOC_PPC_TRILL_MC_ROUTE_KEY   *trill_mc_key,
      SOC_SAND_OUT ARAD_PP_LEM_ACCESS_KEY       *key
    )
{
  uint32
    num_bits;
  SOC_PPC_TRILL_MC_MASKED_FIELDS
    masked_fields;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PPC_TRILL_MC_MASKED_FIELDS_clear(&masked_fields);
  res = arad_pp_frwrd_trill_multicast_key_mask_get_unsafe(
    unit,
    &masked_fields);
  SOC_SAND_CHECK_FUNC_RESULT(res, 27, exit);


  ARAD_PP_LEM_ACCESS_KEY_clear(key);

  
  key->type = type;

  key->nof_params = ARAD_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_TRILL_MC(unit);



  

  if (SOC_IS_DPP_TRILL_FGL(unit)) {
    
    num_bits = ARAD_PP_LEM_ACCESS_KEY_PARAM3_IN_BITS_FOR_TRILL_MC(unit);
    key->param[3].nof_bits = (uint8)num_bits;
    key->param[3].value[0] = trill_mc_key->inner_vid;
  }

  
  num_bits = ARAD_PP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_TRILL_MC(unit);
  key->param[2].nof_bits = (uint8)num_bits;
  key->param[2].value[0] = SOC_IS_DPP_TRILL_FGL(unit) ? trill_mc_key->outer_vid : trill_mc_key->fid;
  
  
  num_bits = ARAD_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_TRILL_MC;
  key->param[1].nof_bits = (uint8)num_bits;
  key->param[1].value[0] = trill_mc_key->esadi;

   
  num_bits = ARAD_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_TRILL_MC;
  key->param[0].nof_bits = (uint8)num_bits;
  key->param[0].value[0] = trill_mc_key->tree_nick;

  key->prefix.nof_bits = ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_TRILL_MC;
  key->prefix.value = ARAD_PP_LEM_ACCESS_KEY_PREFIX_FOR_TRILL_MC;

  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_trill_mc_lem_request_key_build()", 0, 0);
}

#if defined(INCLUDE_KBP)

STATIC
  uint32
    arad_pp_frwrd_trill_uc_kbp_key_mask_encode(
      SOC_SAND_IN  int                       unit,
      SOC_SAND_IN  ARAD_KBP_FRWRD_IP_TBL_ID     frwrd_table_id,
      SOC_SAND_IN  uint32                       nickname_key,
      SOC_SAND_OUT ARAD_PP_LEM_ACCESS_KEY       *data,
      SOC_SAND_OUT ARAD_PP_LEM_ACCESS_KEY       *mask
    )
{
    uint32
        res,
        mask_full = SOC_SAND_U32_MAX;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_PP_LEM_ACCESS_KEY_clear(data);
    ARAD_PP_LEM_ACCESS_KEY_clear(mask);

    
    

    
    ARAD_PP_FRWRD_IP_TCAM_LEM_KEY_ENCODE(unit, frwrd_table_id, 0, nickname_key, data);
    ARAD_PP_FRWRD_IP_TCAM_LEM_KEY_ENCODE(unit, frwrd_table_id, 0, mask_full, mask);


    ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_trill_uc_kbp_key_mask_encode()",0,0);
}




STATIC
  uint32
    arad_pp_frwrd_trill_uc_kbp_result_encode(
      SOC_SAND_IN  int           unit,
      SOC_SAND_IN  SOC_PPC_FEC_ID   fec_id,
      SOC_SAND_OUT ARAD_PP_LEM_ACCESS_PAYLOAD                 *payload
    )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    arad_pp_frwrd_trill_uc_lem_payload_build(
      unit,
      fec_id,
      payload
    );

    ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_trill_uc_kbp_result_encode()",0,0);
}


  void
    arad_pp_frwrd_trill_uc_kbp_result_decode(
      SOC_SAND_IN int                                      unit,
      SOC_SAND_IN ARAD_PP_LEM_ACCESS_PAYLOAD                  *payload,
      SOC_SAND_OUT  SOC_PPC_FEC_ID                            *fec_id
    )
{
    SOC_PPC_FRWRD_DECISION_INFO 
        fwd_decision;

    SOC_PPC_FRWRD_DECISION_INFO_clear(&fwd_decision);

      arad_pp_fwd_decision_in_buffer_parse(
        unit,        
        payload->dest,
        payload->asd,
        payload->flags,
        &fwd_decision    
      );

      
    *fec_id = fwd_decision.dest_id;
}

STATIC
  uint32
    arad_pp_frwrd_trill_uc_kbp_record_add(
      SOC_SAND_IN  int                       unit,
      SOC_SAND_IN  uint32                       nickname_key,
      SOC_SAND_IN  SOC_PPC_FEC_ID               fec_id,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE     *success
    )
{
    uint32
        priority,
        res;
    ARAD_TCAM_ACTION                                
        action;
    ARAD_PP_LEM_ACCESS_KEY       
        data,
        mask;
    ARAD_PP_LEM_ACCESS_PAYLOAD
      payload;
    ARAD_KBP_FRWRD_IP_TBL_ID
        frwrd_table_id;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    soc_sand_os_memset(&action, 0x0, sizeof(action));

    frwrd_table_id = ARAD_KBP_FRWRD_TBL_ID_TRILL_UC;

    
    priority = 0; 

    
    res = arad_pp_frwrd_trill_uc_kbp_key_mask_encode(
            unit,
            frwrd_table_id,
            nickname_key,
            &data,
            &mask
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit); 

    
    res = arad_pp_frwrd_trill_uc_kbp_result_encode(
             unit,
             fec_id,
             &payload
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit); 

    
    res = arad_pp_tcam_route_kbp_add_unsafe(
            unit,
            frwrd_table_id,
            &data,
            &mask,
            priority,
            &payload,
            success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit); 
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_trill_uc_kbp_record_add()",0,0);
}

STATIC
  uint32
    arad_pp_frwrd_trill_uc_kbp_record_remove(
      SOC_SAND_IN  int                       unit,
      SOC_SAND_IN  uint32                       nickname_key
    )
{
    uint32
        res;
    ARAD_PP_LEM_ACCESS_KEY       
        data,
        mask;
    ARAD_KBP_FRWRD_IP_TBL_ID
        frwrd_table_id;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    frwrd_table_id = ARAD_KBP_FRWRD_TBL_ID_TRILL_UC;

    
    res = arad_pp_frwrd_trill_uc_kbp_key_mask_encode(
            unit,
            frwrd_table_id,
            nickname_key,
            &data,
            &mask
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit); 

    
    res = arad_pp_tcam_route_kbp_remove_unsafe(
            unit,
            frwrd_table_id,
            &data,
            &mask
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit); 
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_trill_uc_kbp_record_remove()",0,0);
}

STATIC
  uint32
    arad_pp_frwrd_trill_uc_kbp_record_get(
      SOC_SAND_IN  int                       unit,
      SOC_SAND_IN  uint32                       nickname_key,
      SOC_SAND_OUT SOC_PPC_FEC_ID               *fec_id,
      SOC_SAND_OUT uint8                        *found
    )
{
    uint32
        priority,
        res;
    ARAD_PP_LEM_ACCESS_PAYLOAD
        payload;
    ARAD_PP_LEM_ACCESS_KEY       
        data,
        mask;
    ARAD_KBP_FRWRD_IP_TBL_ID
        frwrd_table_id;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(fec_id);    
    SOC_SAND_CHECK_NULL_INPUT(found);

    frwrd_table_id = ARAD_KBP_FRWRD_TBL_ID_TRILL_UC;

    
    res = arad_pp_frwrd_trill_uc_kbp_key_mask_encode(
            unit,
            frwrd_table_id,
            nickname_key,
            &data,
            &mask
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit); 

    
    res = arad_pp_tcam_route_kbp_get_unsafe(
            unit,
            frwrd_table_id,
            &data,
            &mask,
            &priority,
            &payload,
            found
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit); 

    if (*found) 
    {
        arad_pp_frwrd_trill_uc_kbp_result_decode(
            unit,
            &payload,
            fec_id
          );
    }
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_trill_uc_kbp_record_get()",0,0);
}

STATIC
  uint32
    arad_pp_frwrd_trill_mc_kbp_key_mask_encode(
      SOC_SAND_IN  int                       unit,
      SOC_SAND_IN  ARAD_KBP_FRWRD_IP_TBL_ID   frwrd_table_id,
      SOC_SAND_IN  SOC_PPC_TRILL_MC_ROUTE_KEY   *trill_mc_key,
      SOC_SAND_OUT ARAD_PP_LEM_ACCESS_KEY       *data,
      SOC_SAND_OUT ARAD_PP_LEM_ACCESS_KEY       *mask
    )
{
    uint32
        res,
        mask_void = 0,
        mask_full = SOC_SAND_U32_MAX;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_PP_LEM_ACCESS_KEY_clear(data);
    ARAD_PP_LEM_ACCESS_KEY_clear(mask);

    
    

    
    ARAD_PP_FRWRD_IP_TCAM_LEM_KEY_ENCODE(unit, frwrd_table_id, 0, trill_mc_key->tree_nick, data);
    ARAD_PP_FRWRD_IP_TCAM_LEM_KEY_ENCODE(unit, frwrd_table_id, 0, ((SOC_DPP_CONFIG(unit)->trill.mc_prune_mode)? mask_full: mask_void), mask);
    
    ARAD_PP_FRWRD_IP_TCAM_LEM_KEY_ENCODE(unit, frwrd_table_id, 1, trill_mc_key->fid, data);
    ARAD_PP_FRWRD_IP_TCAM_LEM_KEY_ENCODE(unit, frwrd_table_id, 1, mask_full, mask);
    
    ARAD_PP_FRWRD_IP_TCAM_LEM_KEY_ENCODE(unit, frwrd_table_id, 2, trill_mc_key->esadi, data);
    ARAD_PP_FRWRD_IP_TCAM_LEM_KEY_ENCODE(unit, frwrd_table_id, 2, mask_full, mask);



    ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_trill_mc_kbp_key_mask_encode()",0,0);
}



STATIC
  void
    arad_pp_frwrd_trill_mc_kbp_result_encode(
      SOC_SAND_IN  int   unit,
      SOC_SAND_IN  uint32   mc_id,
      SOC_SAND_OUT ARAD_PP_LEM_ACCESS_PAYLOAD    *payload
    )
{
    arad_pp_frwrd_trill_mc_lem_payload_build(
        unit,
        mc_id,
        payload
    );
}

STATIC
  uint32
    arad_pp_frwrd_trill_mc_kbp_record_add(
      SOC_SAND_IN  int                       unit,
      SOC_SAND_IN  SOC_PPC_TRILL_MC_ROUTE_KEY   *trill_mc_key,
      SOC_SAND_IN  uint32                       mc_id,
      SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE     *success
    )
{
    uint32
        priority,
        res;
    ARAD_TCAM_ACTION                                
        action;
    ARAD_PP_LEM_ACCESS_KEY       
        data,
        mask;
    ARAD_PP_LEM_ACCESS_PAYLOAD
      payload;
    ARAD_KBP_FRWRD_IP_TBL_ID
        frwrd_table_id;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    soc_sand_os_memset(&action, 0x0, sizeof(action));

    frwrd_table_id = ARAD_KBP_FRWRD_TBL_ID_TRILL_MC;

    
    priority = 0; 

    
    res = arad_pp_frwrd_trill_mc_kbp_key_mask_encode(
            unit,
            frwrd_table_id,
            trill_mc_key,
            &data,
            &mask
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit); 

    
    arad_pp_frwrd_trill_mc_kbp_result_encode(
            unit,
            mc_id,
            &payload
          );

    
    res = arad_pp_tcam_route_kbp_add_unsafe(
            unit,
            frwrd_table_id,
            &data,
            &mask,
            priority,
            &payload,
            success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit); 
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_trill_mc_kbp_record_add()",0,0);
}

STATIC
  uint32
    arad_pp_frwrd_trill_mc_kbp_record_remove(
      SOC_SAND_IN  int                       unit,
      SOC_SAND_IN  SOC_PPC_TRILL_MC_ROUTE_KEY   *trill_mc_key
    )
{
    uint32
        res;
    ARAD_PP_LEM_ACCESS_KEY       
        data,
        mask;
    ARAD_KBP_FRWRD_IP_TBL_ID
        frwrd_table_id;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    frwrd_table_id = ARAD_KBP_FRWRD_TBL_ID_TRILL_MC;

    
    res = arad_pp_frwrd_trill_mc_kbp_key_mask_encode(
            unit,
            frwrd_table_id,
            trill_mc_key,
            &data,
            &mask
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit); 

    
    res = arad_pp_tcam_route_kbp_remove_unsafe(
            unit,
            frwrd_table_id,
            &data,
            &mask
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit); 

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_trill_mc_kbp_record_remove()",0,0);
}

STATIC
  uint32
    arad_pp_frwrd_trill_mc_kbp_record_get(
      SOC_SAND_IN  int                       unit,
      SOC_SAND_IN  SOC_PPC_TRILL_MC_ROUTE_KEY   *trill_mc_key,
      SOC_SAND_OUT uint32                       *mc_id,
      SOC_SAND_OUT uint8                        *found
    )
{
    uint32
        priority,
        res;
    ARAD_PP_LEM_ACCESS_PAYLOAD
        payload;
    ARAD_PP_LEM_ACCESS_KEY       
        data,
        mask;
    ARAD_KBP_FRWRD_IP_TBL_ID
        frwrd_table_id;
    SOC_PPC_FRWRD_DECISION_INFO
      fwd_decision;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    frwrd_table_id = ARAD_KBP_FRWRD_TBL_ID_TRILL_MC;

    
    res = arad_pp_frwrd_trill_mc_kbp_key_mask_encode(
            unit,
            frwrd_table_id,
            trill_mc_key,
            &data,
            &mask
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit); 

    
    res = arad_pp_tcam_route_kbp_get_unsafe(
            unit,
            frwrd_table_id,
            &data,
            &mask,
            &priority,
            &payload,
            found
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit); 

    if (*found) 
    {
        SOC_PPC_FRWRD_DECISION_INFO_clear(&fwd_decision);
      res = arad_pp_fwd_decision_in_buffer_parse(
              unit,              
              payload.dest,
              0, 
              ARAD_PP_FWD_DECISION_PARSE_DEST | ARAD_PP_FWD_DECISION_PARSE_EEI,
              &fwd_decision
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 27, exit);

      *mc_id = fwd_decision.dest_id;
    }
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_frwrd_trill_mc_kbp_record_get()",0,0);
}

#endif

uint32
  arad_pp_frwrd_trill_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK,

    fld_val;
  uint64
    field64;
   
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);



  

  

  

  
  if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  90,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHP_LEARN_DATA_VALUES_1r, SOC_CORE_ALL, 0, TRILL_EEI_MSBf,  0xa0));
  }


  
  
  fld_val = SOC_DPP_PP_FRWRD_TRILL_EEDB_INVALID_ENTRY(unit);
  if (SOC_IS_ARAD_B0_AND_ABOVE(unit)) {
      fld_val = ARAD_PP_FRWRD_TRILL_EEDB_INVALID_ENTRY_B0;
  }
  COMPILER_64_SET(field64,0,fld_val);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  110,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, EGQ_TRILL_CONFIGr, SOC_CORE_ALL, 0, TRILL_OUT_LIF_BRIDGEf,  field64));   
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  120,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, EPNI_TRILL_CONFIGr, SOC_CORE_ALL, 0, TRILL_OUT_LIF_BRIDGEf,  field64));

  if (SOC_IS_ARAD_B0_AND_ABOVE(unit)) {
      fld_val = (1<<SOC_DPP_DEFS_GET(unit,out_lif_nof_bits))-1;
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  122,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_INVALID_OUTLIFr, SOC_CORE_ALL, 0, INVALID_OUTLIFf,  fld_val));   
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  124,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_INVALID_OUTLIFr, SOC_CORE_ALL, 0, INVALID_OUTLIFf,  fld_val)); 
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  125,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHP_LINK_LAYER_LOOKUP_CFGr, SOC_CORE_ALL, 0, TRILL_ADJACENCY_MODEf,  1)); 
  }
  
  
  if SOC_IS_ARADPLUS_AND_BELOW(unit) {
      fld_val = 0x14; 
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  130,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_BOUNCE_BACKr, SOC_CORE_ALL, 0, BOUNCE_BACK_THRESHOLDf,  fld_val)); 

      fld_val = 0x44; 
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  140,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_BOUNCE_BACKr, SOC_CORE_ALL, 0, ENABLE_BOUNCE_BACK_FILTERf,  fld_val)); 
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_trill_init_unsafe()", 0, 0);
}


uint32
  arad_pp_l2_lif_trill_uc_get_internal_unsafe(
    SOC_SAND_IN   int                                unit,
    SOC_SAND_IN   uint32                                nickname,
    SOC_SAND_IN   uint8                                 ignore_key,
    SOC_SAND_OUT  SOC_PPC_LIF_ID                        *lif_index,
    SOC_SAND_OUT  SOC_PPC_L2_LIF_TRILL_INFO             *trill_info,
    SOC_SAND_OUT  uint8                                 *is_found
  )
{
  uint32
    res = SOC_SAND_OK;
   
  ARAD_PP_ISEM_ACCESS_KEY
    sem_key;
  ARAD_PP_ISEM_ACCESS_ENTRY
    sem_entry;
  uint8
    found;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  SOC_SAND_CHECK_NULL_INPUT(lif_index);
  SOC_SAND_CHECK_NULL_INPUT(trill_info);
  SOC_SAND_CHECK_NULL_INPUT(is_found);

  SOC_PPC_L2_LIF_TRILL_INFO_clear(trill_info);

  if (!ignore_key)
  {
    ARAD_PP_CLEAR(&sem_key, ARAD_PP_ISEM_ACCESS_KEY, 1);
    sem_key.key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_TRILL_NICK;
    sem_key.key_info.trill.nick_name = nickname;
    ARAD_PP_CLEAR(&sem_entry, ARAD_PP_ISEM_ACCESS_ENTRY, 1);
    res = arad_pp_isem_access_entry_get_unsafe(
            unit,
            &sem_key,
            &sem_entry,
            &found
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
  }
  else
  {
    found = TRUE;
    sem_entry.sem_result_ndx = *lif_index;
  }

  if (found)
  {
    *lif_index = sem_entry.sem_result_ndx;

    do
    {
      ARAD_PP_IHP_LIF_TABLE_TRILL_TBL_DATA
        data;
      uint32
          payload_data[ARAD_PP_LEM_ACCESS_PAYLOAD_NOF_UINT32S];
      ARAD_PP_LEM_ACCESS_PAYLOAD  payload;

      ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
      ARAD_CLEAR(payload_data, uint32, ARAD_PP_LEM_ACCESS_PAYLOAD_NOF_UINT32S);

      res = arad_pp_ihp_lif_table_trill_tbl_get_unsafe(
            unit,
            *lif_index,
            &data
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

    

      payload_data[0] = COMPILER_64_LO(data.learn_info);
      payload_data[1] = COMPILER_64_HI(data.learn_info);

      
      payload.is_learn_data = 1;
      res = arad_pp_lem_access_payload_parse(
          unit,
          payload_data,
          ARAD_PP_LEM_ACCESS_NOF_KEY_TYPES,
          &payload
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 28, exit);

      
      res = arad_pp_fwd_decision_in_buffer_parse(
         unit,          
         payload.dest,
         payload.asd,
         payload.flags, 
         &(trill_info->learn_info)
          );	
      SOC_SAND_CHECK_FUNC_RESULT(res, 29, exit);

      trill_info->fec_id = trill_info->learn_info.dest_id; 
      trill_info->learn_enable = data.tt_learn_enable;
      trill_info->global_lif_id = data.sys_in_lif;
      
      arad_pp_l2_lif_convert_protection_fields_from_hw(unit, data.protection_pointer, data.protection_path,
                                                       &(trill_info->protection_pointer), &(trill_info->protection_pass_value));

      if (SOC_IS_JERICHO(unit)) {
          trill_info->orientation = ((data.orientation_is_hub)? SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB : SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE); 
      }
      *is_found = TRUE;
      goto exit;
      
    } while(0);
  }
  else
  {
    *is_found = FALSE;
    goto exit;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_trill_uc_get_internal_unsafe()", 0, 0);
}

uint32
  arad_pp_l2_lif_trill_mc_get_internal_unsafe(
    SOC_SAND_IN   int                                 unit,
    SOC_SAND_IN   SOC_PPC_TRILL_MC_ROUTE_KEY                  *trill_mc_key,
    SOC_SAND_IN   uint8                                 ignore_key,
    SOC_SAND_OUT  uint32                                  *mc_id,
    SOC_SAND_OUT  uint8                                 *is_found
  )
{
  uint32
    res = SOC_SAND_OK,
    add_info = 0;
  
  ARAD_PP_LEM_ACCESS_REQUEST
    request;
  ARAD_PP_LEM_ACCESS_PAYLOAD
    payload;
  uint8
    found;
  SOC_PPC_FRWRD_DECISION_INFO
    em_dest;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  SOC_PPC_FRWRD_DECISION_INFO_clear(&em_dest);
  ARAD_PP_LEM_ACCESS_REQUEST_clear(&request);
  ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);

  SOC_SAND_CHECK_NULL_INPUT(mc_id);
  SOC_SAND_CHECK_NULL_INPUT(is_found);

  arad_pp_frwrd_trill_mc_lem_request_key_build(
    unit,
    ARAD_PP_LEM_ACCESS_KEY_TYPE_TRILL_MC,
    trill_mc_key,
    &request.key
  );

  res = arad_pp_lem_access_entry_by_key_get_unsafe(
          unit,
          &request.key,
          &payload,
          &found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (!found)
  {
    *is_found = FALSE;
    goto exit;
  }
  
  res = arad_pp_fwd_decision_in_buffer_parse(
          unit,          
          payload.dest,
          add_info,
          ARAD_PP_FWD_DECISION_PARSE_DEST | ARAD_PP_FWD_DECISION_PARSE_EEI,
          &em_dest
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 27, exit);

  *mc_id = em_dest.dest_id;
  *is_found = TRUE;
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_trill_mc_get_internal_unsafe()", 0, 0);
}

uint32
  arad_pp_l2_lif_trill_add_internal_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                              nickname,
    SOC_SAND_IN  uint8                               ignore_key,
    SOC_SAND_IN  SOC_PPC_LIF_ID                      lif_index,
    SOC_SAND_IN  SOC_PPC_L2_LIF_TRILL_INFO           *trill_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE            *success
  )
{
  uint32
    res = SOC_SAND_OK; 
  ARAD_PP_ISEM_ACCESS_KEY
    sem_key;
  ARAD_PP_ISEM_ACCESS_ENTRY
    sem_entry;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  SOC_SAND_CHECK_NULL_INPUT(success);

  if (!ignore_key)
  {
    ARAD_PP_CLEAR(&sem_key, ARAD_PP_ISEM_ACCESS_KEY, 1);
    sem_key.key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_TRILL_NICK;
    sem_key.key_info.trill.nick_name = nickname;
    ARAD_PP_CLEAR(&sem_entry, ARAD_PP_ISEM_ACCESS_ENTRY, 1);
    sem_entry.sem_result_ndx = lif_index;
     
    res = arad_pp_isem_access_entry_add_unsafe(
            unit,
            &sem_key,
            &sem_entry,
            success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
  }
  else
  {
    *success = SOC_SAND_SUCCESS;
  }

 
  if (*success == SOC_SAND_SUCCESS || *success == SOC_SAND_FAILURE_OUT_OF_RESOURCES_2)
  {
    ARAD_PP_IHP_LIF_TABLE_TRILL_TBL_DATA
      data;
    ARAD_PP_LEM_ACCESS_PAYLOAD payload;
    uint32
        payload_data[ARAD_PP_LEM_ACCESS_PAYLOAD_NOF_UINT32S];


    res = arad_pp_ihp_lif_table_trill_tbl_get_unsafe(
            unit,
            lif_index,
            &data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

    
    data.vsi = 0x0; 
    data.tt_learn_enable = trill_info->learn_enable;
    data.tpid_profile = 0;
    data.service_type = ARAD_PP_RIF_ISEM_RES_SERVICE_TYPE;
    data.vsi_assignment_mode = 3; 

    
    arad_pp_l2_lif_convert_protection_fields_to_hw(unit, trill_info->protection_pointer, trill_info->protection_pass_value,
                                                   &data.protection_pointer, &data.protection_path);

    data.sys_in_lif = trill_info->global_lif_id;

    if (SOC_IS_JERICHO(unit)) {
        data.orientation_is_hub = (trill_info->orientation == SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB)?1:0;
        data.orientation_is_hub_valid = 0x1; 
    }

    

    ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload); 

    

    
    res = arad_pp_fwd_decision_in_buffer_build(
       unit, 
       ARAD_PP_FRWRD_DECISION_APPLICATION_TYPE_DFLT,
       &(trill_info->learn_info), 
       &payload.dest,
       &payload.asd
    ); 

    
    payload.flags = ARAD_PP_FWD_DECISION_PARSE_EEI;
    payload.is_learn_data = 1;

    
    res = arad_pp_lem_access_payload_build(
       unit,
       &payload,
       payload_data
    );
    SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
    COMPILER_64_SET(data.learn_info, payload_data[1], payload_data[0]);


    res = arad_pp_ihp_lif_table_trill_tbl_set_unsafe(
            unit,
            lif_index,
            &data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 27, exit);



    
    res = arad_pp_sw_db_lif_table_entry_use_set(
            unit,
            lif_index,
            SOC_PPC_LIF_ENTRY_TYPE_TRILL_NICK,
            0 
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 3001, exit);

    
    if (SOC_IS_JERICHO(unit) && trill_info->is_extended)
    {
        uint64 additional_data;
        COMPILER_64_SET(additional_data,0,0);
           
        res = arad_pp_lif_additional_data_set(unit,lif_index,TRUE,additional_data,FALSE);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 23, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_trill_add_internal_unsafe()", 0, 0);
}
uint32
  arad_pp_l2_lif_trill_remove_internal_unsafe(
    SOC_SAND_IN     int                              unit,
    SOC_SAND_IN     uint32                           nickname,
    SOC_SAND_OUT    SOC_PPC_LIF_ID                   *lif_index
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_ISEM_ACCESS_KEY
    sem_key;
  ARAD_PP_ISEM_ACCESS_ENTRY
    sem_entry;
  uint8
    found;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  

   
  
  
  ARAD_PP_CLEAR(&sem_key, ARAD_PP_ISEM_ACCESS_KEY, 1);
  sem_key.key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_TRILL_NICK;
  sem_key.key_info.trill.nick_name = nickname;

  ARAD_PP_CLEAR(&sem_entry, ARAD_PP_ISEM_ACCESS_ENTRY, 1);
  res = arad_pp_isem_access_entry_get_unsafe(
          unit,
          &sem_key,
          &sem_entry,
          &found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  *lif_index = (SOC_PPC_LIF_ID) sem_entry.sem_result_ndx;

  if (found)
  {
    res = arad_pp_isem_access_entry_remove_unsafe(
            unit,
            &sem_key
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
   
    res = arad_pp_sw_db_lif_table_entry_use_set(
            unit,
            *lif_index,
            SOC_PPC_LIF_ENTRY_TYPE_EMPTY,
            0
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  }



exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_l2_lif_trill_remove_internal_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_trill_multicast_key_mask_set_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PPC_TRILL_MC_MASKED_FIELDS  *masked_fields
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_TRILL_MULTICAST_KEY_MASK_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(masked_fields);

  res = arad_pp_lem_trill_mc_key_build_set(
          unit,
          masked_fields->mask_adjacent_nickname,
          masked_fields->mask_fid,
          masked_fields->mask_ing_nickname
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_trill_multicast_key_mask_set_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_trill_multicast_key_mask_set_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PPC_TRILL_MC_MASKED_FIELDS  *masked_fields
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_TRILL_MULTICAST_KEY_MASK_SET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_TRILL_MC_MASKED_FIELDS, masked_fields, 10, exit);
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_trill_multicast_key_mask_set_verify()", 0, 0);
}

uint32
  arad_pp_frwrd_trill_multicast_key_mask_get_verify(
    SOC_SAND_IN  int                     unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_TRILL_MULTICAST_KEY_MASK_GET_VERIFY);


  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_trill_multicast_key_mask_get_verify()", 0, 0);
}


uint32
  arad_pp_frwrd_trill_multicast_key_mask_get_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_OUT SOC_PPC_TRILL_MC_MASKED_FIELDS  *masked_fields
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_TRILL_MULTICAST_KEY_MASK_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(masked_fields);

  SOC_PPC_TRILL_MC_MASKED_FIELDS_clear(masked_fields);

  res = arad_pp_lem_trill_mc_key_build_get(
          unit,
          &(masked_fields->mask_adjacent_nickname),
          &(masked_fields->mask_fid),
          &(masked_fields->mask_ing_nickname)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_trill_multicast_key_mask_get_unsafe()", 0, 0);
}


uint32
  arad_pp_frwrd_trill_unicast_route_add_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                       nickname_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_DECISION_INFO *fwd_decision,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE     *success
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_LEM_ACCESS_REQUEST
    request;
  ARAD_PP_LEM_ACCESS_PAYLOAD
    payload;
  ARAD_PP_LEM_ACCESS_ACK_STATUS
    ack;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_TRILL_UNICAST_ROUTE_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(success);

#if defined(INCLUDE_KBP)
  
  if(ARAD_KBP_ENABLE_TRILL_UC)
  {
      res = arad_pp_frwrd_trill_uc_kbp_record_add(
                unit,
                nickname_key,
                fwd_decision->dest_id,
                success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  else
#endif
  {
    
    
    
    ARAD_PP_LEM_ACCESS_REQUEST_clear(&request);
    ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
    ARAD_PP_LEM_ACCESS_ACK_STATUS_clear(&ack);

    request.command = ARAD_PP_LEM_ACCESS_CMD_INSERT;
    arad_pp_frwrd_trill_uc_lem_request_key_build(
       unit,
      ARAD_PP_LEM_ACCESS_KEY_TYPE_TRILL_UC,
      nickname_key,
      &request.key
    );

    arad_pp_frwrd_trill_uc_lem_payload_build(
      unit,
      fwd_decision->dest_id,
      &payload
    );

    res = arad_pp_lem_access_entry_add_unsafe(
            unit,
            &request,
            &payload,
            &ack
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
   
    if(ack.is_success == TRUE)
    {
      *success = SOC_SAND_SUCCESS;
    }
    else
    {
      *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_trill_unicast_route_add_unsafe()", nickname_key, 0);
}

uint32
  arad_pp_frwrd_trill_unicast_route_add_verify(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  uint32                      nickname_key,
    SOC_SAND_IN  SOC_PPC_FRWRD_DECISION_INFO *fwd_decision
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_TRILL_UNICAST_ROUTE_ADD_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(nickname_key, ARAD_PP_FRWRD_TRILL_NICKNAME_KEY_MAX, ARAD_PP_FRWRD_TRILL_NICKNAME_KEY_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_NOF(fwd_decision->dest_id, SOC_DPP_DEFS_GET(unit, nof_fecs), SOC_PPC_FEC_ID_OUT_OF_RANGE_ERR, 30, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_trill_unicast_route_add_verify()", nickname_key, 0);
}


uint32
  arad_pp_frwrd_trill_unicast_route_get_unsafe(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  uint32                      nickname_key,
    SOC_SAND_OUT SOC_PPC_FRWRD_DECISION_INFO *learn_info,
    SOC_SAND_OUT uint8                      *is_found
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_LEM_ACCESS_REQUEST
    request;
  ARAD_PP_LEM_ACCESS_PAYLOAD
    payload;  
  uint8 
    found;
  uint32 
    add_info = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_TRILL_UNICAST_ROUTE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(learn_info);
  SOC_SAND_CHECK_NULL_INPUT(is_found);

  
#if defined(INCLUDE_KBP)
  
  if(ARAD_KBP_ENABLE_TRILL_UC) 
  {
      res = arad_pp_frwrd_trill_uc_kbp_record_get(
                unit,
                nickname_key,
                &(learn_info->dest_id),
                is_found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  else
#endif
  {
    ARAD_PP_LEM_ACCESS_REQUEST_clear(&request);
    ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);

    arad_pp_frwrd_trill_uc_lem_request_key_build(
       unit,
      ARAD_PP_LEM_ACCESS_KEY_TYPE_TRILL_UC,
      nickname_key,
      &request.key
    );

    res = arad_pp_lem_access_entry_by_key_get_unsafe(
            unit,
            &request.key,
            &payload,
            &found
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  
    if (!found)
    {
      *is_found = FALSE;
      goto exit;
    }
    
    res = arad_pp_fwd_decision_in_buffer_parse(
            unit,            
            payload.dest,
            add_info,
            ARAD_PP_FWD_DECISION_PARSE_DEST | ARAD_PP_FWD_DECISION_PARSE_EEI,
            learn_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 27, exit);
  
    *is_found = TRUE;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_trill_unicast_route_get_unsafe()", nickname_key, 0);
}

uint32
  arad_pp_frwrd_trill_unicast_route_get_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                      nickname_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_TRILL_UNICAST_ROUTE_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(nickname_key, ARAD_PP_FRWRD_TRILL_NICKNAME_KEY_MAX, ARAD_PP_FRWRD_TRILL_NICKNAME_KEY_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_trill_unicast_route_get_verify()", nickname_key, 0);
}


uint32
  arad_pp_frwrd_trill_unicast_route_remove_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                      nickname_key
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_LEM_ACCESS_REQUEST
    request;
  ARAD_PP_LEM_ACCESS_ACK_STATUS
    ack;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_TRILL_UNICAST_ROUTE_REMOVE_UNSAFE);

#if defined(INCLUDE_KBP)
  
  if(ARAD_KBP_ENABLE_TRILL_UC) 
  {
      res = arad_pp_frwrd_trill_uc_kbp_record_remove(
                unit,
                nickname_key
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  else
#endif
  {

     
    
    
    ARAD_PP_LEM_ACCESS_REQUEST_clear(&request);
    ARAD_PP_LEM_ACCESS_ACK_STATUS_clear(&ack);

    request.command = ARAD_PP_LEM_ACCESS_CMD_DELETE;
    arad_pp_frwrd_trill_uc_lem_request_key_build(
       unit,
      ARAD_PP_LEM_ACCESS_KEY_TYPE_TRILL_UC,
      nickname_key,
      &request.key
    );
    
    res = arad_pp_lem_access_entry_remove_unsafe(
            unit,
            &request,
            &ack);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_trill_unicast_route_remove_unsafe()", nickname_key, 0);
}

uint32
  arad_pp_frwrd_trill_unicast_route_remove_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                      nickname_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_TRILL_UNICAST_ROUTE_REMOVE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(nickname_key, ARAD_PP_FRWRD_TRILL_NICKNAME_KEY_MAX, ARAD_PP_FRWRD_TRILL_NICKNAME_KEY_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_trill_unicast_route_remove_verify()", nickname_key, 0);
}


uint32
  arad_pp_frwrd_trill_ingress_lif_add_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PPC_LIF_ID               lif_index,
    SOC_SAND_IN  uint32                       nickname_key,
    SOC_SAND_IN  SOC_PPC_L2_LIF_TRILL_INFO    *trill_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE     *success
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
      tmp_lif;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(success);

  
  
  
  
  res = arad_pp_l2_lif_trill_add_internal_unsafe(
          unit,
          nickname_key,
          FALSE,
          lif_index,
          trill_info,
          success
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30,exit);

  if (*success == SOC_SAND_SUCCESS || *success == SOC_SAND_FAILURE_OUT_OF_RESOURCES_2)
  {
    res = arad_pp_sw_db_lif_table_entry_use_set(
            unit,
            lif_index,
            SOC_PPC_LIF_ENTRY_TYPE_TRILL_NICK,
            0 
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, arad_pp_sw_db_lif_table_entry_use_set_failue);
  }

  ARAD_DO_NOTHING_AND_EXIT;;

arad_pp_sw_db_lif_table_entry_use_set_failue:

  res = arad_pp_l2_lif_trill_remove_internal_unsafe(
          unit,
          nickname_key,
          &tmp_lif
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_trill_ingress_lif_add_unsafe()", nickname_key, 0);
}

uint32
  arad_pp_frwrd_trill_ingress_lif_add_verify(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  SOC_PPC_LIF_ID              lif_index,
    SOC_SAND_IN  uint32                      nickname_key,
    SOC_SAND_IN  SOC_PPC_L2_LIF_TRILL_INFO  *trill_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_ERR_IF_ABOVE_NOF(lif_index, SOC_DPP_DEFS_GET(unit, nof_local_lifs), SOC_PPC_LIF_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(nickname_key, ARAD_PP_FRWRD_TRILL_NICKNAME_KEY_MAX, ARAD_PP_FRWRD_TRILL_NICKNAME_KEY_OUT_OF_RANGE_ERR, 20, exit);

  res = SOC_PPC_L2_LIF_TRILL_INFO_verify(unit, trill_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_trill_ingress_lif_add_verify()", nickname_key, 0);
}


uint32
  arad_pp_frwrd_trill_ingress_lif_get_unsafe(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  uint32                      nickname_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID             *lif_index,
    SOC_SAND_OUT SOC_PPC_L2_LIF_TRILL_INFO  *trill_info,
    SOC_SAND_OUT uint8                      *is_found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(lif_index);
  SOC_SAND_CHECK_NULL_INPUT(trill_info);
  SOC_SAND_CHECK_NULL_INPUT(is_found);

  
  res = arad_pp_l2_lif_trill_uc_get_internal_unsafe(
          unit,
          nickname_key,
          FALSE,
          lif_index,
          trill_info,
          is_found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_trill_ingress_lif_get_unsafe()", nickname_key, 0);
}

uint32
  arad_pp_frwrd_trill_ingress_lif_get_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                      nickname_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_ERR_IF_ABOVE_MAX(nickname_key, ARAD_PP_FRWRD_TRILL_NICKNAME_KEY_MAX, ARAD_PP_FRWRD_TRILL_NICKNAME_KEY_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_trill_ingress_lif_get_verify()", nickname_key, 0);
}


uint32
  arad_pp_frwrd_trill_ingress_lif_remove_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                      nickname_key,
    SOC_SAND_OUT SOC_PPC_LIF_ID                  *lif_index
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_pp_l2_lif_trill_remove_internal_unsafe(
          unit,
          nickname_key,
          lif_index
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_trill_ingress_lif_remove_unsafe()", nickname_key, 0);
}

uint32
  arad_pp_frwrd_trill_ingress_lif_remove_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                      nickname_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_ERR_IF_ABOVE_MAX(nickname_key, ARAD_PP_FRWRD_TRILL_NICKNAME_KEY_MAX, ARAD_PP_FRWRD_TRILL_NICKNAME_KEY_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_trill_ingress_lif_remove_verify()", nickname_key, 0);
}



uint32
  arad_pp_frwrd_trill_multicast_route_add_unsafe(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PPC_TRILL_MC_ROUTE_KEY         *trill_mc_key,
    SOC_SAND_IN  uint32                             mc_id,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE           *success
  )
{
  uint32 res = SOC_SAND_OK;
  ARAD_PP_LEM_ACCESS_REQUEST request;
  ARAD_PP_LEM_ACCESS_PAYLOAD payload;
  ARAD_PP_LEM_ACCESS_ACK_STATUS ack;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_TRILL_MULTICAST_ROUTE_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(success);

#if defined(INCLUDE_KBP)
  
  if(ARAD_KBP_ENABLE_TRILL_MC) 
  {
      res = arad_pp_frwrd_trill_mc_kbp_record_add(
                unit,
                trill_mc_key,
                mc_id,
                success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  else
#endif
  {

    
    
    
    

    
    
    
    
    
    
    if (((trill_mc_key->fid != 0) || (!SOC_DPP_CONFIG(unit)->trill.mc_prune_mode)) && (mc_id != 0) ) 
    {      
      if (trill_mc_key->tts == 0) {
        ARAD_PP_LEM_ACCESS_REQUEST_clear(&request);
        ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
        ARAD_PP_LEM_ACCESS_ACK_STATUS_clear(&ack);
  
        request.command = ARAD_PP_LEM_ACCESS_CMD_INSERT;
        arad_pp_frwrd_trill_mc_lem_request_key_build(
          unit,
          ARAD_PP_LEM_ACCESS_KEY_TYPE_TRILL_MC,
          trill_mc_key,
          &request.key
          );
  
        arad_pp_frwrd_trill_mc_lem_payload_build(
          unit,
          mc_id,
          &payload
          );
  
        res = arad_pp_lem_access_entry_add_unsafe(
              unit,
              &request,
              &payload,
              &ack
            );
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
     
        if(ack.is_success == TRUE)
        {
          *success = SOC_SAND_SUCCESS;
        }
        else
        {
          *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
        }
      } else {

        uint32 dbal_payload[2] = {0};
        SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];        
        uint32 prio = 0;

        DBAL_QUAL_VALS_CLEAR(qual_vals);
     
        qual_vals[0].type = SOC_PPC_FP_QUAL_IPR2DSP_6EQ7_ESADI;
        qual_vals[0].val.arr[0] = trill_mc_key->esadi;
        qual_vals[0].is_valid.arr[0] = SOC_SAND_U32_MAX;
        qual_vals[1].type = SOC_PPC_FP_QUAL_TRILL_EGRESS_NICK;
        qual_vals[1].val.arr[0] = trill_mc_key->tree_nick;
        qual_vals[1].is_valid.arr[0] = SOC_SAND_U32_MAX;
        qual_vals[2].type = SOC_PPC_FP_QUAL_TRILL_NATIVE_VLAN_VSI;
        qual_vals[2].val.arr[0] = trill_mc_key->outer_vid;
        qual_vals[2].is_valid.arr[0] = SOC_SAND_U32_MAX;

        arad_pp_frwrd_mc_id_to_dest(unit,mc_id,&payload.dest,&payload.asd);

        dbal_payload[0] = payload.dest & 0x7ffff;

        
        
        
        res = arad_pp_dbal_entry_add(unit, SOC_DPP_DBAL_SW_TABLE_ID_TRILL_FLP_TCAM, qual_vals, prio, dbal_payload, success);
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);        
      }
    } 
  }

  exit:
  	SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_trill_multicast_route_add_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_trill_multicast_route_add_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PPC_TRILL_MC_ROUTE_KEY      *trill_mc_key,
    SOC_SAND_IN  uint32                      mc_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_TRILL_MULTICAST_ROUTE_ADD_VERIFY);

  res = SOC_PPC_TRILL_MC_ROUTE_KEY_verify(unit, trill_mc_key);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(mc_id, ARAD_PP_FRWRD_TRILL_MC_ID_MAX, ARAD_PP_FRWRD_TRILL_MC_ID_OUT_OF_RANGE_ERR, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_trill_multicast_route_add_verify()", 0, 0);
}


uint32
  arad_pp_frwrd_trill_multicast_route_get_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PPC_TRILL_MC_ROUTE_KEY      *trill_mc_key,
    SOC_SAND_OUT uint32                      *mc_id,
    SOC_SAND_OUT uint8                     *is_found
  )
{
  uint32 res = SOC_SAND_OK;
  uint8 hit_bit;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_TRILL_MULTICAST_ROUTE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(trill_mc_key);
  SOC_SAND_CHECK_NULL_INPUT(mc_id);
  SOC_SAND_CHECK_NULL_INPUT(is_found);

  *is_found = 0;

#if defined(INCLUDE_KBP)
  
  if(ARAD_KBP_ENABLE_TRILL_MC) 
  {
      res = arad_pp_frwrd_trill_mc_kbp_record_get(
                unit,
                trill_mc_key,
                mc_id,
                is_found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  else
#endif
  {
    
    
    
    
    
    if (trill_mc_key->tts == 0) {
      res = arad_pp_l2_lif_trill_mc_get_internal_unsafe(
              unit,
              trill_mc_key,
              FALSE,
              mc_id,
              is_found
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
    } else {

      if (SOC_DPP_CONFIG(unit)->trill.transparent_service) {
          uint32 dbal_payload;
          SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];        
          uint32 prio = 0;

          DBAL_QUAL_VALS_CLEAR(qual_vals);
       
          qual_vals[0].type = SOC_PPC_FP_QUAL_IPR2DSP_6EQ7_ESADI;
          qual_vals[0].val.arr[0] = trill_mc_key->esadi;
          qual_vals[0].is_valid.arr[0] = SOC_SAND_U32_MAX;
          qual_vals[1].type = SOC_PPC_FP_QUAL_TRILL_EGRESS_NICK;
          qual_vals[1].val.arr[0] = trill_mc_key->tree_nick;
          qual_vals[1].is_valid.arr[0] = SOC_SAND_U32_MAX;
          qual_vals[2].type = SOC_PPC_FP_QUAL_TRILL_NATIVE_VLAN_VSI;
          qual_vals[2].val.arr[0] = trill_mc_key->outer_vid;
          qual_vals[2].is_valid.arr[0] = SOC_SAND_U32_MAX;
          
          res = arad_pp_dbal_entry_get(unit, SOC_DPP_DBAL_SW_TABLE_ID_TRILL_FLP_TCAM, qual_vals, &dbal_payload, &prio, &hit_bit, is_found);
          SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

          if (*is_found) {
            *mc_id = dbal_payload & 0xffff;
          }
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_trill_multicast_route_get_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_trill_multicast_route_get_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PPC_TRILL_MC_ROUTE_KEY      *trill_mc_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_TRILL_MULTICAST_ROUTE_GET_VERIFY);

  res = SOC_PPC_TRILL_MC_ROUTE_KEY_verify(unit, trill_mc_key);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_trill_multicast_route_get_verify()", 0, 0);
}


uint32
  arad_pp_frwrd_trill_multicast_route_remove_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PPC_TRILL_MC_ROUTE_KEY      *trill_mc_key
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_LEM_ACCESS_REQUEST
    request;
  ARAD_PP_LEM_ACCESS_ACK_STATUS
    ack;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_TRILL_MULTICAST_ROUTE_REMOVE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(trill_mc_key);

#if defined(INCLUDE_KBP)
  
  if(ARAD_KBP_ENABLE_TRILL_MC) 
  {
      res = arad_pp_frwrd_trill_mc_kbp_record_remove(
                unit,
                trill_mc_key
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  else
#endif
  {
    
    
    
    
    
    if (trill_mc_key->tts == 0) {
      ARAD_PP_LEM_ACCESS_REQUEST_clear(&request);
      ARAD_PP_LEM_ACCESS_ACK_STATUS_clear(&ack);
  
      request.command = ARAD_PP_LEM_ACCESS_CMD_DELETE;
      arad_pp_frwrd_trill_mc_lem_request_key_build(
        unit,
        ARAD_PP_LEM_ACCESS_KEY_TYPE_TRILL_MC,
        trill_mc_key,
        &request.key
      );
  
      res = arad_pp_lem_access_entry_remove_unsafe(
              unit,
              &request,
              &ack
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    } else {

        if (SOC_DPP_CONFIG(unit)->trill.transparent_service) {

            SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
            SOC_SAND_SUCCESS_FAILURE success;
            
            DBAL_QUAL_VALS_CLEAR(qual_vals);
         
            qual_vals[0].type = SOC_PPC_FP_QUAL_IPR2DSP_6EQ7_ESADI;
            qual_vals[0].val.arr[0] = trill_mc_key->esadi;
            qual_vals[0].is_valid.arr[0] = SOC_SAND_U32_MAX;
            qual_vals[1].type = SOC_PPC_FP_QUAL_TRILL_EGRESS_NICK;
            qual_vals[1].val.arr[0] = trill_mc_key->tree_nick;
            qual_vals[1].is_valid.arr[0] = SOC_SAND_U32_MAX;
            qual_vals[2].type = SOC_PPC_FP_QUAL_TRILL_NATIVE_VLAN_VSI;
            qual_vals[2].val.arr[0] = trill_mc_key->outer_vid;
            qual_vals[2].is_valid.arr[0] = SOC_SAND_U32_MAX;
            
            res = arad_pp_dbal_entry_delete(unit, SOC_DPP_DBAL_SW_TABLE_ID_TRILL_FLP_TCAM, qual_vals, &success);
            SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
        }
    }
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_trill_multicast_route_remove_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_trill_multicast_route_remove_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PPC_TRILL_MC_ROUTE_KEY      *trill_mc_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_TRILL_MULTICAST_ROUTE_REMOVE_VERIFY);

  res = SOC_PPC_TRILL_MC_ROUTE_KEY_verify(unit, trill_mc_key);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_trill_multicast_route_remove_verify()", 0, 0);
}


uint32
  arad_pp_frwrd_trill_adj_info_set_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS           *mac_address_key,
    SOC_SAND_IN  SOC_PPC_TRILL_ADJ_INFO          *mac_auth_info,
    SOC_SAND_IN  uint8                     enable,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE          *success
  )
{
  ARAD_PP_LEM_ACCESS_REQUEST
    request;
  ARAD_PP_LEM_ACCESS_PAYLOAD
    payload;
  ARAD_PP_LEM_ACCESS_ACK_STATUS
    ack_status;
  SOC_PPC_FRWRD_DECISION_INFO
    fwd_decision;
  uint8
    is_found;
  uint32
    new_asd;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_TRILL_ADJ_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mac_address_key);
  SOC_SAND_CHECK_NULL_INPUT(mac_auth_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  ARAD_PP_LEM_ACCESS_REQUEST_clear(&request);
  ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
  ARAD_PP_LEM_ACCESS_ACK_STATUS_clear(&ack_status);

  request.stamp = 0;
  
  res = arad_pp_sa_based_trill_key_to_lem_key_map(
          unit,
          mac_address_key,
          &(mac_auth_info->expect_system_port),
          &(request.key)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  

  
  
  res = arad_pp_lem_access_entry_by_key_get_unsafe(
          unit,
          &(request.key),
          &payload,
          &is_found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  
  if (!is_found && !enable)
  {
    *success = SOC_SAND_SUCCESS;
    goto exit;
  }

  
  payload.age = ARAD_PP_TRILL_SA_AUTH_ENTRY_AGE;
  payload.is_dynamic = ARAD_PP_TRILL_SA_AUTH_ENTRY_IS_DYNAMIC;

  
  if (enable)
  {
      SOC_PPC_FRWRD_DECISION_INFO_clear(&fwd_decision);
      fwd_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT; 
    fwd_decision.type = SOC_PPC_FRWRD_DECISION_TYPE_UC_PORT;
    fwd_decision.dest_id = mac_auth_info->expect_adjacent_eep;

    
    fwd_decision.additional_info.eei.type = SOC_PPC_EEI_TYPE_OUTLIF;
    fwd_decision.additional_info.eei.val.outlif = mac_auth_info->expect_adjacent_eep;










    payload.dest = fwd_decision.dest_id; 

    
   
    res = arad_pp_lem_access_trill_sa_based_asd_build(
            unit,
            mac_auth_info,
            &new_asd
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    payload.asd = new_asd;
    
    request.command = ARAD_PP_LEM_ACCESS_CMD_INSERT;

    payload.flags = ARAD_PP_FWD_DECISION_PARSE_FORMAT_3;
    res = arad_pp_lem_access_entry_add_unsafe(
            unit,
            &request,
            &payload,
            &ack_status
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

  else 
  {
    request.command = ARAD_PP_LEM_ACCESS_CMD_DELETE;
    res = arad_pp_lem_access_entry_remove_unsafe(
            unit,
            &request,
            &ack_status
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

  if (ack_status.is_success == TRUE)
  {
    *success = SOC_SAND_SUCCESS;
  }
  else
  {
    if (ack_status.reason == ARAD_PP_LEM_ACCESS_FAIL_REASON_DELETE_UNKNOWN)
    {
      *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES_2;
    }
    else
    {
      *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
    }
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_trill_adj_info_set_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_trill_adj_info_set_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS           *mac_address_key,
    SOC_SAND_IN  SOC_PPC_TRILL_ADJ_INFO          *mac_auth_info,
    SOC_SAND_IN  uint8                     enable
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_TRILL_ADJ_INFO_SET_VERIFY);

  
  ARAD_PP_STRUCT_VERIFY(SOC_PPC_TRILL_ADJ_INFO, mac_auth_info, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_trill_adj_info_set_verify()", 0, 0);
}

uint32
  arad_pp_frwrd_trill_adj_info_get_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS           *mac_address_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_TRILL_ADJ_INFO_GET_VERIFY);

  

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_trill_adj_info_get_verify()", 0, 0);
}


uint32
  arad_pp_frwrd_trill_adj_info_get_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS           *mac_address_key,
    SOC_SAND_OUT SOC_PPC_TRILL_ADJ_INFO            *mac_auth_info,
    SOC_SAND_OUT uint8                     *enable
  )
{
  ARAD_PP_LEM_ACCESS_REQUEST
    request;
  ARAD_PP_LEM_ACCESS_PAYLOAD
    payload;
  ARAD_PP_LEM_ACCESS_ACK_STATUS
    ack_status;
  SOC_PPC_FRWRD_DECISION_INFO
    fwd_decision;
  uint8
    is_found;
  uint32
    res = SOC_SAND_OK;
                         
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_TRILL_ADJ_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mac_address_key);
  SOC_SAND_CHECK_NULL_INPUT(mac_auth_info);
  SOC_SAND_CHECK_NULL_INPUT(enable);
  
  
  mac_auth_info->expect_adjacent_eep = 0;

  ARAD_PP_LEM_ACCESS_REQUEST_clear(&request);
  ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
  ARAD_PP_LEM_ACCESS_ACK_STATUS_clear(&ack_status);


  request.stamp = 0;
  
  res = arad_pp_sa_based_trill_key_to_lem_key_map(
          unit,
          mac_address_key,
          &(mac_auth_info->expect_system_port),
          &(request.key)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  
  
  res = arad_pp_lem_access_entry_by_key_get_unsafe(
          unit,
          &(request.key),
          &payload,
          &is_found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  
  if (!is_found)
  {
    *enable = FALSE;
    goto exit;
  }

  
  res = arad_pp_lem_access_trill_sa_based_asd_parse(
          unit,
          payload.asd,
          mac_auth_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);
 
  if (mac_auth_info->expect_system_port.sys_id != SOC_PPC_LLP_SA_AUTH_ACCEPT_ALL_PORTS)
  {
      res = arad_pp_fwd_decision_in_buffer_parse(
              unit,              
              payload.dest,
              0, 
              0,
              &(fwd_decision)
              );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
      if (fwd_decision.type == SOC_PPC_FRWRD_DECISION_TYPE_UC_LAG)
      {
        mac_auth_info->expect_system_port.sys_port_type = SOC_TMC_DEST_SYS_PORT_TYPE_LAG;
        mac_auth_info->expect_system_port.sys_id = fwd_decision.dest_id;
      }
      else
      {
        mac_auth_info->expect_system_port.sys_port_type = SOC_TMC_DEST_SYS_PORT_TYPE_SYS_PHY_PORT;
        mac_auth_info->expect_system_port.sys_id = fwd_decision.dest_id;
      }
  }

  *enable = TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_trill_adj_info_get_unsafe()", 0, 0);
}


uint32
  arad_pp_frwrd_trill_global_info_set_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_TRILL_GLOBAL_INFO *glbl_info
  )
{
    uint32
    res = SOC_SAND_OK;
    uint64
        field64;
   
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_TRILL_GLOBAL_INFO_SET_UNSAFE);

  COMPILER_64_SET(field64, 0, glbl_info->cfg_ttl);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, EPNI_TRILL_CONFIGr, SOC_CORE_ALL, 0, TRILL_HOP_CNTf,  field64));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_PP_5r, REG_PORT_ANY, 0, ETHERTYPE_TRILLf,  glbl_info->ethertype));
  
  SOC_SAND_CHECK_NULL_INPUT(glbl_info);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_trill_global_info_set_unsafe()", 0, 0);
}

uint32
  arad_pp_frwrd_trill_global_info_set_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PPC_FRWRD_TRILL_GLOBAL_INFO *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_TRILL_GLOBAL_INFO_SET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(SOC_PPC_FRWRD_TRILL_GLOBAL_INFO, glbl_info, 10, exit);
  
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_trill_global_info_set_verify()", 0, 0);
}

uint32
  arad_pp_frwrd_trill_global_info_get_verify(
    SOC_SAND_IN  int                     unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_TRILL_GLOBAL_INFO_GET_VERIFY);
  
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_trill_global_info_get_verify()", 0, 0);
}


uint32
  arad_pp_frwrd_trill_global_info_get_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_OUT SOC_PPC_FRWRD_TRILL_GLOBAL_INFO *glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint64
      field64;
   
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPC_FRWRD_TRILL_GLOBAL_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(glbl_info);

  SOC_PPC_FRWRD_TRILL_GLOBAL_INFO_clear(glbl_info);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_read(unit, EPNI_TRILL_CONFIGr, SOC_CORE_ALL, 0, TRILL_HOP_CNTf, &field64));
  glbl_info->cfg_ttl = COMPILER_64_LO(field64);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, ECI_GLOBAL_PP_5r, REG_PORT_ANY, 0, ETHERTYPE_TRILLf, &glbl_info->ethertype));

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_trill_global_info_get_unsafe()", 0, 0);
}


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_frwrd_trill_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_frwrd_trill;
}

CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_frwrd_trill_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_frwrd_trill;
}
uint32
  SOC_PPC_TRILL_MC_MASKED_FIELDS_verify(
    SOC_SAND_IN  SOC_PPC_TRILL_MC_MASKED_FIELDS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);


  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_TRILL_MC_MASKED_FIELDS_verify()",0,0);
}

uint32
  SOC_PPC_TRILL_MC_ROUTE_KEY_verify(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PPC_TRILL_MC_ROUTE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tree_nick, ARAD_PP_FRWRD_TRILL_TREE_NICK_MAX, ARAD_PP_FRWRD_TRILL_TREE_NICK_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_NOF(info->fid, SOC_DPP_DEFS_GET(unit, nof_fids), SOC_PPC_FID_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->ing_nick, ARAD_PP_FRWRD_TRILL_ING_NICK_MAX, ARAD_PP_FRWRD_TRILL_ING_NICK_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->adjacent_eep, ARAD_PP_FRWRD_TRILL_ADJACENT_EEP_MAX, ARAD_PP_FRWRD_TRILL_ADJACENT_EEP_OUT_OF_RANGE_ERR, 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_TRILL_MC_ROUTE_KEY_verify()",0,0);
}

uint32
  SOC_PPC_TRILL_ADJ_INFO_verify(
    SOC_SAND_IN  SOC_PPC_TRILL_ADJ_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->expect_adjacent_eep, ARAD_PP_FRWRD_TRILL_EXPECT_ADJACENT_EEP_MAX, ARAD_PP_FRWRD_TRILL_EXPECT_ADJACENT_EEP_OUT_OF_RANGE_ERR, 10, exit);
  

  SOC_SAND_MAGIC_NUM_VERIFY(info);
  ARAD_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_TRILL_ADJ_INFO_verify()",0,0);
}

uint32
  SOC_PPC_FRWRD_TRILL_GLOBAL_INFO_verify(
    SOC_SAND_IN  SOC_PPC_FRWRD_TRILL_GLOBAL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->cfg_ttl, ARAD_PP_FRWRD_TRILL_CFG_TTL_MAX, ARAD_PP_FRWRD_TRILL_CFG_TTL_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PPC_FRWRD_TRILL_GLOBAL_INFO_verify()",0,0);
}
uint32
  arad_pp_frwrd_trill_vsi_entry_add_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     vsi,
    SOC_SAND_IN  uint32                     high_vid,
    SOC_SAND_IN  uint32                     low_vid
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_TRILL_VSI_ENTRY_ADD_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(vsi, ARAD_PP_VSI_ID_MAX, SOC_PPC_VSI_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(high_vid, ARAD_PP_VSI_ID_MAX, SOC_PPC_VSI_ID_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(low_vid, ARAD_PP_VSI_ID_MAX, SOC_PPC_VSI_ID_OUT_OF_RANGE_ERR, 30, exit);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_trill_vsi_entry_add_verify()", vsi, 0);
}

uint32
arad_pp_frwrd_trill_vsi_entry_add_unsafe(
   SOC_SAND_IN  int                                 unit,
   SOC_SAND_IN  uint32                                 vsi,
   SOC_SAND_IN  uint32                                 flags,
   SOC_SAND_IN  uint32                                 high_vid,
   SOC_SAND_IN  uint32                                 low_vid,
   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE               *success
 )
{
 uint32
   res = SOC_SAND_OK;
 ARAD_PP_ISEM_ACCESS_KEY
   sem_key;
 ARAD_PP_ISEM_ACCESS_ENTRY
   sem_entry;
 uint32 
     is_fgl = FALSE;

 SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_TRILL_VSI_ENTRY_ADD_UNSAFE);

 SOC_SAND_CHECK_NULL_INPUT(success);

 is_fgl =  ( (SOC_DPP_CONFIG(unit)->trill.mode == SOC_PPD_TRILL_MODE_FGL) && (low_vid != BCM_VLAN_INVALID));
 ARAD_PP_CLEAR(&sem_key, ARAD_PP_ISEM_ACCESS_KEY, 1);
 sem_key.key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_TRILL_VSI;
 if (is_fgl) {
     sem_key.key_info.trill.flags = ARAD_PP_ISEM_ACCESS_TRILL_VSI_FGL;
 } else {
     sem_key.key_info.trill.flags = ARAD_PP_ISEM_ACCESS_TRILL_VSI_VL;
 } 
 if (flags & SOC_PPD_TRILL_VSI_TRANSPARENT_SERVICE) {
     sem_key.key_info.trill.flags |= ARAD_PP_ISEM_ACCESS_TRILL_VSI_TTS;
 }

 sem_key.key_info.trill.high_vid = high_vid;
 if (low_vid != BCM_VLAN_INVALID) {
   sem_key.key_info.trill.low_vid = low_vid;
 }

 ARAD_PP_CLEAR(&sem_entry, ARAD_PP_ISEM_ACCESS_ENTRY, 1);
 sem_entry.sem_result_ndx = vsi;
 res = arad_pp_isem_access_entry_add_unsafe(
         unit,
         &sem_key,
         &sem_entry,
         success
       );
 SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);



exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_trill_vsi_entry_add_unsafe()", 0, 0);


}
uint32
  arad_pp_frwrd_trill_vsi_entry_remove_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     vsi,
    SOC_SAND_IN  uint32                     high_vid,
    SOC_SAND_IN  uint32                     low_vid
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_TRILL_VSI_ENTRY_REMOVE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(vsi, ARAD_PP_VSI_ID_MAX, SOC_PPC_VSI_ID_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(high_vid, ARAD_PP_VSI_ID_MAX, SOC_PPC_VSI_ID_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(low_vid, ARAD_PP_VSI_ID_MAX, SOC_PPC_VSI_ID_OUT_OF_RANGE_ERR, 30, exit);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_trill_vsi_entry_remove_verify()", vsi, 0);
}

uint32
arad_pp_frwrd_trill_vsi_entry_remove_unsafe(
   SOC_SAND_IN  int                                 unit,
   SOC_SAND_IN  uint32                                 vsi,
   SOC_SAND_IN  uint32                                 flags,   
   SOC_SAND_IN  uint32                                 high_vid,
   SOC_SAND_IN  uint32                                 low_vid
 )
{
 uint32
   res = SOC_SAND_OK;
 ARAD_PP_ISEM_ACCESS_KEY
   sem_key;
 ARAD_PP_ISEM_ACCESS_ENTRY
   sem_entry;
 uint32 
     is_fgl = FALSE;

 SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_TRILL_VSI_ENTRY_REMOVE_UNSAFE);

 is_fgl =  ( (SOC_DPP_CONFIG(unit)->trill.mode == SOC_PPD_TRILL_MODE_FGL) && (low_vid != BCM_VLAN_INVALID));
 ARAD_PP_CLEAR(&sem_key, ARAD_PP_ISEM_ACCESS_KEY, 1);
 sem_key.key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_TRILL_VSI;
 if (is_fgl) {
     sem_key.key_info.trill.flags = ARAD_PP_ISEM_ACCESS_TRILL_VSI_FGL;
 } else {
     sem_key.key_info.trill.flags = ARAD_PP_ISEM_ACCESS_TRILL_VSI_VL;
 } 
 if (flags & SOC_PPD_TRILL_VSI_TRANSPARENT_SERVICE) {
     sem_key.key_info.trill.flags |= ARAD_PP_ISEM_ACCESS_TRILL_VSI_TTS;
 }

 sem_key.key_info.trill.high_vid = high_vid;
 sem_key.key_info.trill.low_vid = low_vid;
 ARAD_PP_CLEAR(&sem_entry, ARAD_PP_ISEM_ACCESS_ENTRY, 1);
 sem_entry.sem_result_ndx = vsi;

 res = arad_pp_isem_access_entry_remove_unsafe(
            unit,
            &sem_key
          );
 SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit); 
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_trill_vsi_remove_add_unsafe()", 0, 0);


}
uint32
arad_pp_frwrd_trill_native_inner_tpid_add_unsafe(
   SOC_SAND_IN  int                                 unit,
   SOC_SAND_IN  uint32                                 tpid,
   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE               *success
 )
{
 uint32
   res = SOC_SAND_OK;
 ARAD_PP_ISEM_ACCESS_KEY
   sem_key;
 ARAD_PP_ISEM_ACCESS_ENTRY
   sem_entry;

 SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_FRWRD_TRILL_NATIVE_INNER_TPID_ADD_UNSAFE);

 SOC_SAND_CHECK_NULL_INPUT(success);

 ARAD_PP_CLEAR(&sem_key, ARAD_PP_ISEM_ACCESS_KEY, 1);
 sem_key.key_type = ARAD_PP_ISEM_ACCESS_KEY_TYPE_TRILL_NATIVE_INNER_TPID;
 sem_key.key_info.trill.native_inner_tpid = tpid;

 
 ARAD_PP_CLEAR(&sem_entry, ARAD_PP_ISEM_ACCESS_ENTRY, 1); 
 res = arad_pp_isem_access_entry_add_unsafe(
         unit,
         &sem_key,
         &sem_entry,
         success
       );
 SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);



exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_trill_native_inner_tpid_add_unsafe()", 0, 0);


}
uint32
arad_pp_frwrd_trill_native_inner_tpid_add_verify(
   SOC_SAND_IN  int                     unit,
   SOC_SAND_IN  uint32                     tpid
 )
{
 uint32
   res = SOC_SAND_OK;

 SOC_SAND_INIT_ERROR_DEFINITIONS(0);


 ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_frwrd_trill_native_inner_tpid_add_verify()", tpid, 0);
}

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 


