/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <soc/mcm/memregs.h>

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_SLB


#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>


#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_slb.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_slb.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_mact.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_mact_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lem_access.h>
#include <soc/dpp/PPC/ppc_api_lag.h>
#include <soc/dpp/PPC/ppc_api_frwrd_mact.h>
#include <soc/dpp/drv.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lag.h>

#include <soc/mem.h>





#define ARAD_PP_SLB_LAG_GROUP_MEMBER_NOF_BITS       (SOC_IS_QUX(unit)? 11 : 14)
#define ARAD_PP_SLB_LAG_GROUP_NOF_BITS              (SOC_IS_QUX(unit)? 5 : 10)


#define ARAD_PP_SLB_MAX_MATCH_RULES                 2

#define ARAD_PP_SLB_PAYLOAD_FEC_NOF_BITS            15

#define ARAD_PP_SLB_NOF_PROGRAMS                    8

#define ARAD_PP_SLB_DEFAULT_AGING_TIME_IN_SECONDS   10
#define ARAD_PP_SLB_NOF_AGES                        8
#define ARAD_PP_SLB_NOF_OWNED                       2

#define ARAD_PP_SLB_PROG_IDX                        2
#define ARAD_PP_SLB_PROG_CFG_PTR                    3

#define ARAD_PP_SLB_LAG_HASH_OFFSET_BIT_NUM         4

#define ARAD_PP_SLB_ECMP_HASH_KEY_OFFSET_BIT_NUM    6
#define ARAD_PP_SLB_ECMP_HASH_KEY_OFFSET_NOF        2
#define ARAD_PP_SLB_CRC_HASH_MASK_NOF               4






typedef uint32 (*_arad_pp_slb_verify)(SOC_SAND_IN int unit, SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj);
typedef const SOC_PPC_SLB_OBJECT * const * SOC_PPC_SLB_CONST_OBJECT_ARRAY;





  STATIC uint32 SOC_PPC_SLB_OBJECT_verify(SOC_SAND_IN int unit, SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj);

  #define PPC_API_SLB_INTERNAL_OBJECT_LIST_ONLY_OBJECT_ACTION(suffix) STATIC uint32 SOC_PPC_SLB_ ## suffix ## _verify(SOC_SAND_IN int unit, SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj);
  #define PPC_API_SLB_INTERNAL_OBJECT_LIST_ONLY

  #include <soc/dpp/PPC/ppc_api_slb.h>

  #undef PPC_API_SLB_INTERNAL_OBJECT_LIST_ONLY
  #undef PPC_API_SLB_INTERNAL_OBJECT_LIST_ONLY_OBJECT_ACTION




static const _arad_pp_slb_verify _arad_pp_slb_verify_arr[] = {
   SOC_PPC_SLB_OBJECT_verify,

  
  #define PPC_API_SLB_INTERNAL_OBJECT_LIST_ONLY_OBJECT_ACTION(suffix)  SOC_PPC_SLB_ ## suffix ## _verify,
  #define PPC_API_SLB_INTERNAL_OBJECT_LIST_ONLY

  #include <soc/dpp/PPC/ppc_api_slb.h>

  #undef PPC_API_SLB_INTERNAL_OBJECT_LIST_ONLY
  #undef PPC_API_SLB_INTERNAL_OBJECT_LIST_ONLY_OBJECT_ACTION

};

SOC_PPC_SLB_C_ASSERT((sizeof(_arad_pp_slb_verify_arr) / sizeof(_arad_pp_slb_verify_arr[0])) == SOC_PPC_SLB_NOF_OBJECT_TYPES);





STATIC uint32 _arad_pp_slb_bit_mask(SOC_SAND_IN uint8 nof_bits)
{
  return (1 << nof_bits) - 1;
}


STATIC uint32 _arad_pp_slb_max_val_for_bits(SOC_SAND_IN uint8 nof_bits)
{
  return _arad_pp_slb_bit_mask(nof_bits);
}

STATIC uint8 _arad_pp_slb_lag_member_nof_bits(SOC_SAND_IN int unit)
{
  return (ARAD_PP_SLB_LAG_GROUP_MEMBER_NOF_BITS - ARAD_PP_SLB_LAG_GROUP_NOF_BITS + SOC_DPP_CONFIG(unit)->arad->init.ports.lag_mode);
}

STATIC uint32 SOC_PPC_SLB_OBJECT_verify(SOC_SAND_IN int unit, SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj)
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  SOC_SAND_MAGIC_NUM_VERIFY(untyped_obj);

  SOC_SAND_EXIT_NO_ERROR;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

STATIC uint32 SOC_PPC_SLB_TRAVERSE_MATCH_RULE_verify(SOC_SAND_IN int unit, SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj)
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  SOC_SAND_EXIT_NO_ERROR;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

STATIC uint32 SOC_PPC_SLB_TRAVERSE_MATCH_RULE_ALL_LAG_verify(SOC_SAND_IN int unit, SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj)
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  SOC_SAND_EXIT_NO_ERROR;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

STATIC uint32 SOC_PPC_SLB_TRAVERSE_MATCH_RULE_ALL_ECMP_verify(SOC_SAND_IN int unit, SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj)
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  SOC_SAND_EXIT_NO_ERROR;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

STATIC uint32 SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_verify(SOC_SAND_IN int unit, SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj)
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  SOC_SAND_EXIT_NO_ERROR;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

STATIC uint32 SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_LAG_verify(SOC_SAND_IN int unit, SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj)
{
  const SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_LAG *object = (const SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_LAG *)untyped_obj;
  uint8 lag_idx_bits = ARAD_PP_SLB_LAG_GROUP_MEMBER_NOF_BITS - _arad_pp_slb_lag_member_nof_bits(unit);

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  SOC_SAND_ERR_IF_ABOVE_MAX(object->lag_ndx, _arad_pp_slb_max_val_for_bits(lag_idx_bits), 0, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

STATIC uint32 SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_ECMP_verify(SOC_SAND_IN int unit, SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj)
{
  const SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_ECMP *object = (const SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_ECMP *)untyped_obj;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  SOC_SAND_ERR_IF_ABOVE_MAX(object->ecmp_ndx, SOC_DPP_CONFIG(unit)->l3.fec_ecmp_reserved, 0, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

STATIC uint32 SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER_verify(SOC_SAND_IN int unit, SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj)
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  SOC_SAND_EXIT_NO_ERROR;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

STATIC uint32 SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER_LAG_verify(SOC_SAND_IN int unit, SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj)
{
  const SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER_LAG *object = (const SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER_LAG *)untyped_obj;
  uint8 lag_member_idx_bits = _arad_pp_slb_lag_member_nof_bits(unit);
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  SOC_SAND_ERR_IF_ABOVE_MAX(object->lag_member_ndx, _arad_pp_slb_max_val_for_bits(lag_member_idx_bits), 0, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

STATIC uint32 SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER_ECMP_verify(SOC_SAND_IN int unit, SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj)
{
  const SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER_ECMP *object = (const SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER_ECMP *)untyped_obj;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  SOC_SAND_ERR_IF_ABOVE_MAX(object->ecmp_member_ndx, SOC_DPP_CONFIG(unit)->l3.ecmp_max_paths, 0, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

STATIC uint32 SOC_PPC_SLB_TRAVERSE_UPDATE_VALUE_verify(SOC_SAND_IN int unit, SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj)
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_EXIT_NO_ERROR;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

STATIC uint32 SOC_PPC_SLB_TRAVERSE_UPDATE_VALUE_LAG_MEMBER_verify(SOC_SAND_IN int unit, SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj)
{
  const SOC_PPC_SLB_TRAVERSE_UPDATE_VALUE_LAG_MEMBER *object = (const SOC_PPC_SLB_TRAVERSE_UPDATE_VALUE_LAG_MEMBER *)untyped_obj;
  uint8 lag_member_idx_bits = _arad_pp_slb_lag_member_nof_bits(unit);

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  SOC_SAND_ERR_IF_ABOVE_MAX(object->new_lag_member_ndx, _arad_pp_slb_max_val_for_bits(lag_member_idx_bits), 0, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

STATIC uint32 SOC_PPC_SLB_TRAVERSE_UPDATE_VALUE_ECMP_MEMBER_verify(SOC_SAND_IN int unit, SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj)
{
  const SOC_PPC_SLB_TRAVERSE_UPDATE_VALUE_ECMP_MEMBER *object = (const SOC_PPC_SLB_TRAVERSE_UPDATE_VALUE_ECMP_MEMBER *)untyped_obj;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  SOC_SAND_ERR_IF_ABOVE_MAX(object->new_ecmp_member_ndx, SOC_DPP_CONFIG(unit)->l3.ecmp_max_paths, 0, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

STATIC uint32 SOC_PPC_SLB_TRAVERSE_ACTION_verify(SOC_SAND_IN int unit, SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj)
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_EXIT_NO_ERROR;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

STATIC uint32 SOC_PPC_SLB_TRAVERSE_ACTION_REMOVE_verify(SOC_SAND_IN int unit, SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj)
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_EXIT_NO_ERROR;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

STATIC uint32 SOC_PPC_SLB_TRAVERSE_ACTION_COUNT_verify(SOC_SAND_IN int unit, SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj)
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_EXIT_NO_ERROR;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

STATIC uint32 SOC_PPC_SLB_TRAVERSE_ACTION_UPDATE_verify(SOC_SAND_IN int unit, SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj)
{
  uint32 rv;
  const SOC_PPC_SLB_TRAVERSE_ACTION_UPDATE *object = (const SOC_PPC_SLB_TRAVERSE_ACTION_UPDATE *)untyped_obj;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  rv = ARAD_PP_SLB_verify(unit, (const SOC_PPC_SLB_OBJECT *)(object->traverse_update_value));
  SOC_SAND_CHECK_FUNC_RESULT(rv, 10, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

STATIC uint32 SOC_PPC_SLB_ENTRY_KEY_verify(SOC_SAND_IN int unit, SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj)
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  SOC_SAND_VERIFY(FALSE);

  SOC_SAND_EXIT_NO_ERROR;
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

STATIC uint32 SOC_PPC_SLB_ENTRY_VALUE_verify(SOC_SAND_IN int unit, SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj)
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  SOC_SAND_VERIFY(FALSE);

  SOC_SAND_EXIT_NO_ERROR
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

STATIC uint32 SOC_PPC_SLB_CONFIGURATION_ITEM_verify(SOC_SAND_IN int unit, SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj)
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_EXIT_NO_ERROR;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

STATIC uint32 SOC_PPC_SLB_CONFIGURATION_ITEM_SLB_ENTRY_AGING_TIME_IN_SECONDS_verify(SOC_SAND_IN int unit, SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj)
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

STATIC uint32 SOC_PPC_SLB_CONFIGURATION_ITEM_LAG_HASH_FUNCTION_verify(SOC_SAND_IN int unit, SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj)
{
  const SOC_PPC_SLB_CONFIGURATION_ITEM_LAG_HASH_FUNCTION *object = (const SOC_PPC_SLB_CONFIGURATION_ITEM_LAG_HASH_FUNCTION*)untyped_obj;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (!((3 <= object->hash_function && object->hash_function <= 5) || (0x10 <= object->hash_function && object->hash_function <= 0x15))) {
    SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_MSG_STR("Illegal hash function %d"), object->hash_function));
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

STATIC uint32 SOC_PPC_SLB_CONFIGURATION_ITEM_LAG_HASH_SEED_verify(SOC_SAND_IN int unit, SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj)
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_EXIT_NO_ERROR;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

STATIC uint32 SOC_PPC_SLB_CONFIGURATION_ITEM_ECMP_HASH_SEED_verify(SOC_SAND_IN int unit, SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj)
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_EXIT_NO_ERROR;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

STATIC uint32 SOC_PPC_SLB_CONFIGURATION_ITEM_CRC_HASH_SEED_verify(SOC_SAND_IN int unit, SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj)
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_EXIT_NO_ERROR;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

STATIC uint32 SOC_PPC_SLB_CONFIGURATION_ITEM_CRC_HASH_MASK_verify(SOC_SAND_IN int unit, SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj)
{
  const SOC_PPC_SLB_CONFIGURATION_ITEM_CRC_HASH_MASK *object = (const SOC_PPC_SLB_CONFIGURATION_ITEM_CRC_HASH_MASK*)untyped_obj;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_ERR_IF_ABOVE_MAX(object->index, ARAD_PP_SLB_CRC_HASH_MASK_NOF - 1, 0, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

STATIC uint32 SOC_PPC_SLB_CONFIGURATION_ITEM_ECMP_HASH_KEY_OFFSET_verify(SOC_SAND_IN int unit, SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj)
{
  const SOC_PPC_SLB_CONFIGURATION_ITEM_ECMP_HASH_KEY_OFFSET *object = (const SOC_PPC_SLB_CONFIGURATION_ITEM_ECMP_HASH_KEY_OFFSET*)untyped_obj;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_ERR_IF_ABOVE_MAX(object->offset, _arad_pp_slb_max_val_for_bits(ARAD_PP_SLB_ECMP_HASH_KEY_OFFSET_BIT_NUM), 0, 10, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(object->index, ARAD_PP_SLB_ECMP_HASH_KEY_OFFSET_NOF - 1, 0, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

STATIC uint32 SOC_PPC_SLB_CONFIGURATION_ITEM_LAG_HASH_OFFSET_verify(SOC_SAND_IN int unit, SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj)
{
  const SOC_PPC_SLB_CONFIGURATION_ITEM_LAG_HASH_OFFSET *object = (const SOC_PPC_SLB_CONFIGURATION_ITEM_LAG_HASH_OFFSET*)untyped_obj;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_ERR_IF_ABOVE_MAX(object->offset, _arad_pp_slb_max_val_for_bits(ARAD_PP_SLB_LAG_HASH_OFFSET_BIT_NUM), 0, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}


STATIC uint32 _arad_pp_slb_init_verify(SOC_SAND_IN int unit)
{
  uint8
    authentication_enable;
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  res = sw_state_access[unit].dpp.soc.arad.pp.oper_mode.authentication_enable.get(unit, &authentication_enable);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  if (authentication_enable) {
    SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_MSG_STR("SA authentication cannot be used with stateful load balancing.\n")));
  }
   
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}


STATIC uint32 _arad_pp_slb_init_general_lem_configuration(SOC_SAND_IN int unit, SOC_SAND_IN uint32 slb_prefix)
{
  uint32 res = SOC_SAND_OK;
  soc_reg_t consistent_hashing_configration_reg = INVALIDr;
  uint32 slb_prefix_4_msb;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  45,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_PP_7r , REG_PORT_ANY, 0, SA_LOOKUP_TYPEf,  3));
  
  
  
  
  
  
#ifdef BCM_88675_A0
  if(SOC_IS_JERICHO(unit)) {
      consistent_hashing_configration_reg = PPDB_B_CONSISTENT_HASHING_CONFIGRATIONr;
      
      slb_prefix_4_msb = slb_prefix >> 1;
  } else 
#endif 
  {
      consistent_hashing_configration_reg = IHP_CONSISTENT_HASHING_CONFIGRATIONr;
      slb_prefix_4_msb = slb_prefix;
  }

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, consistent_hashing_configration_reg, REG_PORT_ANY, 0, MACT_DB_ID_CONSISTENT_HASHING_IDf,  slb_prefix_4_msb));

  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  810,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_PP_1r, REG_PORT_ANY, 0, MACT_DB_ID_PROG_IDf,  slb_prefix_4_msb));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  820,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_PP_1r, REG_PORT_ANY, 0, MACT_DB_ID_PROG_MASKf,  0));
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}


STATIC uint32 _arad_pp_slb_init_hashing(SOC_SAND_IN int unit)
{
  uint32 res = SOC_SAND_OK;
  uint32 i;

  SOC_PPC_SLB_CONFIGURATION_ITEM_LAG_HASH_FUNCTION function;
  SOC_PPC_SLB_CONFIGURATION_ITEM_ECMP_HASH_KEY_OFFSET key_offset;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PPC_SLB_CLEAR(&function, SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_LAG_HASH_FUNCTION);
  SOC_PPC_SLB_CLEAR(&key_offset, SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_ECMP_HASH_KEY_OFFSET);

  
  function.hash_function = SOC_PPC_LAG_LB_CRC_0x8423;
  res = arad_pp_api_impl_slb_set_global_cfg(unit, SOC_PPC_SLB_DOWNCAST(&function, SOC_PPC_SLB_CONFIGURATION_ITEM));
  SOC_SAND_CHECK_FUNC_RESULT(res, 740, exit);

  if (SOC_IS_JERICHO(unit)) {
	  for (i = 0; i < ARAD_PP_SLB_ECMP_HASH_KEY_OFFSET_NOF; i++) {
	      
	      key_offset.offset = (i==0) ? ARAD_PP_SLB_ECMP_HASH_KEY_0_OFFSET_DEFAULT : ARAD_PP_SLB_ECMP_HASH_KEY_1_OFFSET_DEFAULT;
		  key_offset.index  = i;
		  res = arad_pp_api_impl_slb_set_global_cfg(unit, SOC_PPC_SLB_DOWNCAST(&key_offset, SOC_PPC_SLB_CONFIGURATION_ITEM));
		  SOC_SAND_CHECK_FUNC_RESULT(res, 750, exit);
	  }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}


STATIC uint32 _arad_pp_slb_disable_traps(SOC_SAND_IN int unit)
{
  uint32 res = SOC_SAND_OK;
  soc_reg_above_64_val_t forbidden_traps_reg;
  soc_reg_above_64_val_t forbidden_traps_field;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_REG_ABOVE_64_CLEAR(forbidden_traps_reg);
  res = soc_reg_above_64_get(unit, IHB_CONSISTENT_HASHING_FORBIDDEN_TRAPSr, REG_PORT_ANY, 0, forbidden_traps_reg);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 710, exit);

  
  SOC_REG_ABOVE_64_SET_PATTERN(&forbidden_traps_field, 0);
  ARAD_FLD_TO_REG_ABOVE_64(IHB_CONSISTENT_HASHING_FORBIDDEN_TRAPSr, CONSISTENT_HASHING_FORBIDDEN_TRAPSf, forbidden_traps_field, forbidden_traps_reg, 720, exit);

  res = soc_reg_above_64_set(unit, IHB_CONSISTENT_HASHING_FORBIDDEN_TRAPSr, REG_PORT_ANY, 0, forbidden_traps_reg);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 730, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}


STATIC uint32 _arad_pp_slb_init_learning(SOC_SAND_IN int unit)
{
  uint32 res = SOC_SAND_OK;

  unsigned int line_idx;
  unsigned int cmd_idx;
  const unsigned int key_db_profile = ARAD_PP_SLB_PROG_IDX;
  const unsigned int nof_cmds = 8;
  unsigned int desired_line_val;
  unsigned int line_val;
  unsigned int entry_idx;
  const unsigned int entries_per_line = 8;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHB_LBP_GENERAL_CONFIG_0r, SOC_CORE_ALL, 0, CONSISTENT_HASHING_STATE_LEARN_ENABLEf,  1));

  
  desired_line_val = 0;
  for (entry_idx = 0; entry_idx < entries_per_line; entry_idx++) {
    desired_line_val |= 1 << (entry_idx * 3); 
  }

  for (cmd_idx = 0; cmd_idx < nof_cmds; cmd_idx++) {
    line_idx = (key_db_profile << 3) + cmd_idx;
    SOC_SAND_VERIFY(line_idx < 32);
    res = READ_OLP_DSP_EVENT_ROUTEm(unit, MEM_BLOCK_ANY, line_idx, &line_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 8010, exit);

    soc_mem_field32_set(unit, OLP_DSP_EVENT_ROUTEm, &line_val, DSP_EVENT_ROUTE_LINEf, desired_line_val);

    res = WRITE_OLP_DSP_EVENT_ROUTEm(unit, MEM_BLOCK_ANY, line_idx, &line_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 8020, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

 
STATIC uint32 _arad_pp_slb_init_aging(SOC_SAND_IN int unit)
{
  uint32 res = SOC_SAND_OK;

  SOC_PPC_SLB_CONFIGURATION_ITEM_SLB_ENTRY_AGING_TIME_IN_SECONDS cfg_item;
  soc_reg_above_64_val_t reg_above_64_val;
  unsigned int fid_idx;
  unsigned int is_dynamic;
  unsigned int entry_offset;

  const unsigned int entry_sz_in_bits = 4; 
  const unsigned int prog_ptr = ARAD_PP_SLB_PROG_CFG_PTR;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PPC_SLB_CLEAR(&cfg_item, SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_SLB_ENTRY_AGING_TIME_IN_SECONDS);

  
  res = READ_PPDB_B_LARGE_EM_AGE_AGING_MODEr(unit, reg_above_64_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  for (fid_idx = 0; fid_idx < 4; fid_idx++) {
    for (is_dynamic = 0; is_dynamic < 2; is_dynamic++) {
      entry_offset = (ARAD_PP_SLB_PROG_IDX << 3) + (is_dynamic << 2) + fid_idx;
      
      SHR_BITCOPY_RANGE(reg_above_64_val, entry_offset * entry_sz_in_bits, &prog_ptr, 0, 2);
    }
  } 

  res = WRITE_PPDB_B_LARGE_EM_AGE_AGING_MODEr(unit, reg_above_64_val);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  
  cfg_item.age_time_in_seconds = ARAD_PP_SLB_DEFAULT_AGING_TIME_IN_SECONDS;

  
  res = sw_state_access[unit].dpp.soc.arad.pp.slb_config.age_time_in_seconds.set(unit, cfg_item.age_time_in_seconds);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 25, exit);

  res = arad_pp_api_impl_slb_set_global_cfg(unit, SOC_PPC_SLB_DOWNCAST(&cfg_item, SOC_PPC_SLB_CONFIGURATION_ITEM));
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}


STATIC uint32 _arad_pp_slb_init_program_variables(SOC_SAND_IN int unit, SOC_SAND_IN uint8 slb_enable, SOC_SAND_IN uint32 slb_prefix)
{
  unsigned int program_idx;
  uint32 res = SOC_SAND_OK;
  int enable_standard_format;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  enable_standard_format = (SOC_IS_JERICHO(unit)) ? ENABLE_STANDARD_FORMATSf : ITEM_40_40f;

  for (program_idx = 0; program_idx < ARAD_PP_SLB_NOF_PROGRAMS; program_idx++) {
    uint32 reg_val[2] = {0,0};

    res = READ_IHB_CONSISTENT_HASHING_PROGRAM_VARIABLESm(unit, MEM_BLOCK_ANY, program_idx, &reg_val);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 90, exit);

    soc_mem_field32_set(unit, IHB_CONSISTENT_HASHING_PROGRAM_VARIABLESm, reg_val, enable_standard_format , 0);
    soc_mem_field32_set(unit, IHB_CONSISTENT_HASHING_PROGRAM_VARIABLESm, reg_val, ENABLE_LOOKUPf, slb_enable);
    soc_mem_field32_set(unit, IHB_CONSISTENT_HASHING_PROGRAM_VARIABLESm, reg_val, AND_MASKf, 0xF);
    soc_mem_field32_set(unit, IHB_CONSISTENT_HASHING_PROGRAM_VARIABLESm, reg_val, OR_MASKf, slb_prefix);
    soc_mem_field32_set(unit, IHB_CONSISTENT_HASHING_PROGRAM_VARIABLESm, reg_val, KEY_MASK_VALUEf, 0xFFFF); 

    res = WRITE_IHB_CONSISTENT_HASHING_PROGRAM_VARIABLESm(unit, MEM_BLOCK_ANY, program_idx, &reg_val);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 100, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}


uint32
  arad_pp_slb_init_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                                  enable
  )
{
  uint32 res = SOC_SAND_OK;
  uint8 val = enable ? 1 : 0;
  uint32 slb_prefix = 0;
  soc_reg_t
    regs[2];
  uint32
    nof_regs,
    reg_idx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  res = soc_ppc_slb_init();
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit);

#ifdef BCM_88675_A0
  if(SOC_IS_JERICHO(unit)) {
      regs[0] = ECI_GLOBAL_PP_7r;
      regs[1] = INVALIDr;
      nof_regs = 1;
  } else
#endif 
  {
      
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IRR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, CONSISTENT_HASHING_ENf,  val));
      regs[0] = IHP_CONSISTENT_HASHING_CONFIGRATIONr;
      regs[1] = IHB_CONSISTENT_HASHING_CONFIGRATIONr; 
      nof_regs = 2;
  }

  for(reg_idx = 0; reg_idx < nof_regs; ++reg_idx) {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, regs[reg_idx], REG_PORT_ANY, 0, CONSISTENT_HASHING_ENABLEf,  val));
  }

  res = _arad_pp_slb_init_hashing(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = _arad_pp_slb_disable_traps(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  if (enable) {
    
    res = arad_pp_lem_access_app_to_prefix_get(unit, ARAD_PP_LEM_ACCESS_KEY_PREFIX_FOR_SLB, &slb_prefix);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = _arad_pp_slb_init_verify(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    res = _arad_pp_slb_init_general_lem_configuration(unit, slb_prefix);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    res = _arad_pp_slb_init_learning(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    res = _arad_pp_slb_init_aging(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }

  res = _arad_pp_slb_init_program_variables(unit, enable, slb_prefix);
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

typedef struct {
  int unit;
} _arad_pp_slb_verify_traverse_cb_data;

STATIC uint32 _arad_pp_slb_verify_traverse_cb(SOC_SAND_IN SOC_PPC_SLB_OBJECT *object, SOC_SAND_IN SOC_PPC_SLB_OBJECT_TYPE type, void *opaque)
{
  _arad_pp_slb_verify_traverse_cb_data *data = (_arad_pp_slb_verify_traverse_cb_data*)opaque;
  _arad_pp_slb_verify verify_cb;
  uint32 rv;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  verify_cb = _arad_pp_slb_verify_arr[type];

  if (verify_cb) {
    rv = verify_cb(data->unit, object);
    SOC_SAND_CHECK_FUNC_RESULT(rv, 10, exit);
  }

  SOC_SAND_EXIT_NO_ERROR;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

uint32 ARAD_PP_SLB_verify(SOC_SAND_IN int unit, SOC_SAND_IN SOC_PPC_SLB_OBJECT *object)
{
  uint32 rv;
  _arad_pp_slb_verify_traverse_cb_data data;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(object);
  
  data.unit = unit;

  rv = soc_ppc_slb_object_traverse_type_tree(object, &_arad_pp_slb_verify_traverse_cb, &data);
  SOC_SAND_CHECK_FUNC_RESULT(rv, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}



STATIC uint32 
  _arad_pp_slb_object_type_in_types(
    SOC_SAND_IN     int                               unit,
    SOC_SAND_IN     SOC_PPC_SLB_OBJECT *                 object,
    SOC_SAND_IN     SOC_PPC_SLB_OBJECT_TYPE *            allowed_types,
    SOC_SAND_IN     uint32                               nof_allowed_types,
    SOC_SAND_OUT    uint8 *                              yes
  )
{

  unsigned int i;
  uint32 rv;
  SOC_PPC_SLB_OBJECT_TYPE type;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  rv = soc_ppc_slb_object_type(object,&type);
  SOC_SAND_CHECK_FUNC_RESULT(rv, 10, exit);

  *yes = FALSE;

  for (i = 0; i < nof_allowed_types; i++) {
    if (type == allowed_types[i]) {
      *yes = TRUE;
      break;
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}


STATIC uint32
  _arad_pp_slb_verify_match_rule_array(
    SOC_SAND_IN     int                                      unit,
    SOC_SAND_IN     SOC_PPC_SLB_TRAVERSE_MATCH_RULE * const *   match_rules,
    SOC_SAND_IN     uint32                                      nof_match_rules
  )
{
  unsigned int i;
  uint32 rv;
  const uint32 max_match_rules = 2;
  uint8 lb_group_rule_seen = 0;
  uint8 lb_group_member_rule_seen = 0;
  SOC_PPC_SLB_OBJECT_TYPE types[ARAD_PP_SLB_MAX_MATCH_RULES];
  uint8 is_rule_lag_arr[ARAD_PP_SLB_MAX_MATCH_RULES];

  SOC_PPC_SLB_OBJECT_TYPE allowed_types[] = { 
    SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_MATCH_RULE_LB_GROUP_LAG,
    SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_MATCH_RULE_LB_GROUP_ECMP,
    SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER_LAG,
    SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER_ECMP
  };
  uint32 nof_allowed_types = 4;
  SOC_PPC_SLB_OBJECT **rules_object_arr = (SOC_PPC_SLB_OBJECT **)match_rules;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(match_rules);

  
  SOC_SAND_ERR_IF_ABOVE_MAX(nof_match_rules, max_match_rules, 0, 10, exit);
  SOC_SAND_ERR_IF_BELOW_MIN(nof_match_rules, 1, 0, 10, exit);

  
  rv = soc_ppc_slb_object_type(rules_object_arr[0], types + 0);
  SOC_SAND_CHECK_FUNC_RESULT(rv, 5, exit);

  switch (types[0]) {
  case SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_MATCH_RULE_ALL_LAG:
  case SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_MATCH_RULE_ALL_ECMP:
    SOC_SAND_ERR_IF_ABOVE_MAX(nof_match_rules, 1, 0, 10, exit);
    SOC_SAND_EXIT_NO_ERROR;
    break;
  default:
    break;
  }

  
  for (i = 0; i < nof_match_rules; i++) {
    uint8 yes;
    uint8 is_lb_group = FALSE;
    uint8 is_lb_group_member = FALSE;

    rv = soc_ppc_slb_object_type(rules_object_arr[i], types + i);
    SOC_SAND_CHECK_FUNC_RESULT(rv, 5, exit);

    is_rule_lag_arr[i] = (
                      (types[i] == SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_MATCH_RULE_LB_GROUP_LAG) || 
                      (types[i] == SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER_LAG)
                     ) ? 1 : 0;

    
    rv = _arad_pp_slb_object_type_in_types(unit, rules_object_arr[i], allowed_types, nof_allowed_types, &yes);
    SOC_SAND_CHECK_FUNC_RESULT(rv, 10, exit);
    if (!yes) {
      SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_MSG_STR("Invalid object type for match rule.")));
    }

    rv = soc_ppc_slb_object_is(rules_object_arr[i], SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_MATCH_RULE_LB_GROUP, &is_lb_group);
    SOC_SAND_CHECK_FUNC_RESULT(rv, 12, exit);

    rv = soc_ppc_slb_object_is(rules_object_arr[i], SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER, &is_lb_group_member);
    SOC_SAND_CHECK_FUNC_RESULT(rv, 13, exit);

    SOC_SAND_VERIFY(is_lb_group != is_lb_group_member);

    if (is_lb_group && lb_group_rule_seen) {
      SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_MSG_STR("At most one LB_GROUP rule is allowed.")));
    } else if (is_lb_group) {
      lb_group_rule_seen = TRUE;
    }

    if (is_lb_group_member && lb_group_member_rule_seen) {
      SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_MSG_STR("At most one LB_GROUP_MEMBER rule is allowed.")));
    } else if (is_lb_group_member) {
      lb_group_member_rule_seen = TRUE;
    }
  }

  
  if ((nof_match_rules == 2) && (is_rule_lag_arr[0] != is_rule_lag_arr[1])) {
    SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_MSG_STR("ECMP and LAG rules may not be mixed.")));
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}


STATIC uint32
  _arad_pp_slb_verify_traverse_action(
    SOC_SAND_IN     int                               unit,
    SOC_SAND_IN     SOC_PPC_SLB_TRAVERSE_ACTION *        action
  )
{
  SOC_PPC_SLB_OBJECT_TYPE allowed_types[] = { 
    SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_ACTION_COUNT,
    SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_ACTION_REMOVE,
    SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_ACTION_UPDATE
  };
  uint32 nof_allowed_types = 2;
  uint8 yes;
  SOC_PPC_SLB_OBJECT *action_object = (SOC_PPC_SLB_OBJECT*)action;
  uint32 rv;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(action);

  
  rv = _arad_pp_slb_object_type_in_types(unit, action_object, allowed_types, nof_allowed_types, &yes);
  SOC_SAND_CHECK_FUNC_RESULT(rv, 10, exit);

  rv = ARAD_PP_SLB_verify(unit, action_object);
  SOC_SAND_CHECK_FUNC_RESULT(rv, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

uint32 
  arad_pp_api_impl_slb_traverse_verify(
    SOC_SAND_IN     int                                      unit,
    SOC_SAND_IN     SOC_PPC_SLB_TRAVERSE_MATCH_RULE * const*    match_rules,
    SOC_SAND_IN     uint32                                      nof_match_rules,
    SOC_SAND_IN     SOC_PPC_SLB_TRAVERSE_ACTION *               action,
    SOC_SAND_OUT    uint32 *                                    nof_matched_entries
  )
{
  uint32 rv;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  rv = _arad_pp_slb_verify_match_rule_array(unit, match_rules, nof_match_rules);
  SOC_SAND_CHECK_FUNC_RESULT(rv, 10, exit);

  rv = _arad_pp_slb_verify_traverse_action(unit, action);
  SOC_SAND_CHECK_FUNC_RESULT(rv, 20, exit);

  
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}




uint32 
  _arad_pp_slb_slb_rules_to_lem_keys(
    SOC_SAND_IN     int                                      unit,
    SOC_SAND_IN     SOC_PPC_SLB_TRAVERSE_MATCH_RULE * const *   match_rules,
    SOC_SAND_IN     uint32                                      nof_match_rules,
    SOC_SAND_OUT    ARAD_PP_LEM_ACCESS_KEY *                    key,
    SOC_SAND_OUT    ARAD_PP_LEM_ACCESS_KEY *                    key_mask
  )
{
  uint32 rv;
  SOC_PPC_SLB_CONST_OBJECT_ARRAY match_rule_object_arr = (SOC_PPC_SLB_CONST_OBJECT_ARRAY)match_rules;
  uint8 is_destination_fec = FALSE;
  uint32 destination = 0;
  uint8 use_destination_in_rule = FALSE;
  unsigned int i;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_PP_LEM_ACCESS_KEY_clear(key);
  ARAD_PP_LEM_ACCESS_KEY_clear(key_mask);

  
  for (i = 0; i < nof_match_rules; i++) {
    SOC_PPC_SLB_OBJECT_TYPE type;

    rv = soc_ppc_slb_object_type(match_rule_object_arr[i], &type);
    SOC_SAND_CHECK_FUNC_RESULT(rv, 10, exit);

    switch (type) {
    case SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_MATCH_RULE_ALL_ECMP:
      is_destination_fec = TRUE;
      break;
    case SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_MATCH_RULE_ALL_LAG:
      is_destination_fec = FALSE;
      break;
    case SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_MATCH_RULE_LB_GROUP_LAG:
      {
        SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_LAG *lag_group = (SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_LAG *)match_rule_object_arr[i];
        is_destination_fec = FALSE;
        use_destination_in_rule = TRUE;
        destination = lag_group->lag_ndx;
      }
      break;
    case SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_MATCH_RULE_LB_GROUP_ECMP:
      {
        SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_ECMP *ecmp_group = (SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_ECMP *)match_rule_object_arr[i];
        is_destination_fec = TRUE;
        use_destination_in_rule = TRUE;
        destination = ecmp_group->ecmp_ndx;
      }
      break;
    case SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER_LAG:
    case SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER_ECMP:
      break;
    default:
      SOC_SAND_VERIFY(FALSE);
      break;
    }
  }

  key->type = ARAD_PP_LEM_ACCESS_KEY_TYPE_SLB;
  key->prefix.value = ARAD_PP_LEM_ACCESS_KEY_PREFIX_FOR_SLB;
  key->prefix.nof_bits = ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS;
  key->nof_params = ARAD_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_SLB;
  
  key->param[0].nof_bits = ARAD_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_SLB;
  key->param[0].value[0] = 0;
  key->param[0].value[1] = 0;

  
  key->param[1].nof_bits = ARAD_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_SLB;
  key->param[1].value[0] = destination; 

  
  key->param[2].nof_bits = ARAD_PP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_SLB;
  key->param[2].value[0] = is_destination_fec;

  key_mask->prefix.value = _arad_pp_slb_bit_mask(ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS);
  key_mask->prefix.nof_bits = ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS;
  key_mask->nof_params = 3;
  
  key_mask->param[0].nof_bits = ARAD_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_SLB;
  key_mask->param[0].value[0] = 0;
  key_mask->param[0].value[1] = 0;
  key_mask->param[1].nof_bits = ARAD_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_SLB;
  if (use_destination_in_rule) {
    
    
    
    key_mask->param[1].value[0] = _arad_pp_slb_bit_mask(ARAD_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_SLB) & ~1;

    
    if (!is_destination_fec) {
      key_mask->param[1].value[0] = _arad_pp_slb_bit_mask(ARAD_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_SLB - 1) & ~1;
    }
  } else {
    key_mask->param[1].value[0] = 0;
  }
  
  key_mask->param[2].nof_bits = ARAD_PP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_SLB;
  key_mask->param[2].value[0] = _arad_pp_slb_bit_mask(ARAD_PP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_SLB);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}
    

uint32 
  _arad_pp_slb_slb_rules_to_lem_value_rule(
    SOC_SAND_IN     int                                          unit,
    SOC_SAND_IN     SOC_PPC_SLB_TRAVERSE_MATCH_RULE * const *       match_rules,
    SOC_SAND_IN     uint32                                          nof_match_rules,
    SOC_SAND_OUT    SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE *  lem_value_rule
  )
{
  uint32 rv;
  SOC_PPC_SLB_CONST_OBJECT_ARRAY match_rule_object_arr = (SOC_PPC_SLB_CONST_OBJECT_ARRAY)match_rules;
  uint8 is_group_ecmp = FALSE;
  uint32 group_ndx = 0;
  uint32 member_ndx = 0;
  uint8 member_match = FALSE;
  const uint8 lag_group_idx_nof_bits = ARAD_PP_SLB_LAG_GROUP_MEMBER_NOF_BITS - _arad_pp_slb_lag_member_nof_bits(unit);
  unsigned int i;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE_clear(lem_value_rule);

    
  for (i = 0; i < nof_match_rules; i++) {
    SOC_PPC_SLB_OBJECT_TYPE type;

    rv = soc_ppc_slb_object_type(match_rule_object_arr[i], &type);
    SOC_SAND_CHECK_FUNC_RESULT(rv, 10, exit);

    switch (type) {
    case SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_MATCH_RULE_ALL_ECMP:
      is_group_ecmp = TRUE;
      break;
    case SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_MATCH_RULE_ALL_LAG:
      is_group_ecmp = FALSE;
      break;
    case SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_MATCH_RULE_LB_GROUP_LAG:
      {
        SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_LAG *lag_group = (SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_LAG *)match_rule_object_arr[i];

        is_group_ecmp = FALSE;
        group_ndx = lag_group->lag_ndx;
        member_match = TRUE;
      }
      break;
    case SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_MATCH_RULE_LB_GROUP_ECMP:
      {
        

        is_group_ecmp = TRUE;
        
        
      }
      break;
    case SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER_LAG:
      {
        SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER_LAG *lag_group_member = (SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER_LAG *)match_rule_object_arr[i];
        is_group_ecmp = FALSE;
        member_ndx = lag_group_member->lag_member_ndx;
        member_match = TRUE;
      }
      break;
    case SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER_ECMP:
      {
        SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER_ECMP *ecmp_group_member = (SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER_ECMP *)match_rule_object_arr[i];
        is_group_ecmp = TRUE;
        member_ndx = ecmp_group_member->ecmp_member_ndx;
        member_match = TRUE;
      }
      break;
    default:
      SOC_SAND_VERIFY(FALSE);
      break;
    }
  }

  
  
  
  
  lem_value_rule->val_mask.slb_info.match_fec_entries = 0;
  lem_value_rule->val_mask.slb_info.match_lag_entries = 0;
  lem_value_rule->val_mask.slb_info.fec = 0;
  lem_value_rule->val_mask.slb_info.lag_group_and_member = 0;
  lem_value_rule->compare_mask = SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_COMPARE_SLB;

  if (is_group_ecmp) {
    lem_value_rule->val_mask.slb_info.match_fec_entries = 1;
    lem_value_rule->val.slb_info.match_fec_entries = 1;
    if (member_match) {
      lem_value_rule->val_mask.slb_info.fec = _arad_pp_slb_bit_mask(ARAD_PP_SLB_PAYLOAD_FEC_NOF_BITS);
      lem_value_rule->val.slb_info.fec = member_ndx;
    }
  } else {
    lem_value_rule->val_mask.slb_info.match_lag_entries = 1;
    lem_value_rule->val.slb_info.match_lag_entries = 1;
    if (member_match) {
      lem_value_rule->val_mask.slb_info.lag_group_and_member = _arad_pp_slb_bit_mask(ARAD_PP_SLB_LAG_GROUP_MEMBER_NOF_BITS);
      lem_value_rule->val.slb_info.lag_group_and_member = (member_ndx << lag_group_idx_nof_bits) | group_ndx;
    }
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}    

uint32 
  _arad_pp_slb_slb_action_to_lem_action(
    SOC_SAND_IN     int                                          unit,
    SOC_SAND_IN     SOC_PPC_SLB_TRAVERSE_ACTION *                   action,
    SOC_SAND_OUT    SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION *            mact_traverse_action
  )
{
  uint32 rv;
  const SOC_PPC_SLB_OBJECT *action_object = (const SOC_PPC_SLB_OBJECT *)action;
  SOC_PPC_SLB_OBJECT_TYPE type;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_clear(mact_traverse_action);

  rv = soc_ppc_slb_object_type(action_object, &type);
  SOC_SAND_CHECK_FUNC_RESULT(rv, 10, exit);

  switch (type) {
  case SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_ACTION_COUNT:
    mact_traverse_action->type = SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_COUNT;
    break;
  case SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_ACTION_REMOVE:
    mact_traverse_action->type = SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_REMOVE;
    break;
  case SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_ACTION_UPDATE:
    {
      const SOC_PPC_SLB_TRAVERSE_ACTION_UPDATE *update_action = (const SOC_PPC_SLB_TRAVERSE_ACTION_UPDATE *)action;
      SOC_PPC_SLB_OBJECT_TYPE value_update_type;
      uint32 new_value = 0;
      uint8 is_new_value_fec = FALSE;

      rv = soc_ppc_slb_object_type((const SOC_PPC_SLB_OBJECT *)update_action->traverse_update_value, &value_update_type);
      SOC_SAND_CHECK_FUNC_RESULT(rv, 10, exit);

      
      switch (value_update_type) {
      case SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_UPDATE_VALUE_ECMP_MEMBER:
        {
          const SOC_PPC_SLB_TRAVERSE_UPDATE_VALUE_ECMP_MEMBER *ecmp_member_update_value = (const SOC_PPC_SLB_TRAVERSE_UPDATE_VALUE_ECMP_MEMBER *)update_action->traverse_update_value;
          is_new_value_fec = TRUE;
          new_value = ecmp_member_update_value->new_ecmp_member_ndx;
        }
        break;
      case SOC_PPC_SLB_OBJECT_TYPE_TRAVERSE_UPDATE_VALUE_LAG_MEMBER:
        {
          const SOC_PPC_SLB_TRAVERSE_UPDATE_VALUE_LAG_MEMBER *lag_member_update_value = (const SOC_PPC_SLB_TRAVERSE_UPDATE_VALUE_LAG_MEMBER *)update_action->traverse_update_value;
          is_new_value_fec = FALSE;
          new_value = lag_member_update_value->new_lag_member_ndx;
        }
        break;
      default:
        SOC_SAND_VERIFY(FALSE);
        break;
      }

      
      mact_traverse_action->type = SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_UPDATE;
      mact_traverse_action->update_mask = SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_SELECT_REPLACE_SLB;
      if (is_new_value_fec) {
        mact_traverse_action->updated_val.slb_info.fec = new_value;
        mact_traverse_action->updated_val_mask.slb_info.fec = _arad_pp_slb_bit_mask(ARAD_PP_SLB_PAYLOAD_FEC_NOF_BITS);
      } else {
        const uint32 lag_idx_nof_bits = _arad_pp_slb_lag_member_nof_bits(unit);
        const uint32 lag_idx_shift = ARAD_PP_SLB_LAG_GROUP_MEMBER_NOF_BITS - _arad_pp_slb_lag_member_nof_bits(unit);

        mact_traverse_action->updated_val.slb_info.lag_group_and_member = new_value << lag_idx_shift;
        mact_traverse_action->updated_val_mask.slb_info.lag_group_and_member = _arad_pp_slb_bit_mask(lag_idx_nof_bits) << lag_idx_shift;
      }

    }
    break;
  default:
    SOC_SAND_VERIFY(FALSE);
    break;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

uint32 
  arad_pp_api_impl_slb_traverse_unsafe(
    SOC_SAND_IN     int                                      unit,
    SOC_SAND_IN     SOC_PPC_SLB_TRAVERSE_MATCH_RULE * const *   match_rules,
    SOC_SAND_IN     uint32                                      nof_match_rules,
    SOC_SAND_IN     SOC_PPC_SLB_TRAVERSE_ACTION *               action,
    SOC_SAND_OUT    uint32 *                                    nof_matched_entries
  )
{
  uint32 rv;
  ARAD_PP_LEM_ACCESS_KEY key;
  ARAD_PP_LEM_ACCESS_KEY key_mask;
  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE lem_match_value_rule;
  SOC_SAND_TABLE_BLOCK_RANGE block_range;
  SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION mact_action;
  uint8 wait_till_finish = TRUE;
  uint32 nof_matched_entries_internal;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  
  rv = _arad_pp_slb_slb_rules_to_lem_keys(unit, match_rules, nof_match_rules, &key, &key_mask);
  SOC_SAND_CHECK_FUNC_RESULT(rv, 10, exit);

  rv = _arad_pp_slb_slb_rules_to_lem_value_rule(unit, match_rules, nof_match_rules, &lem_match_value_rule);
  SOC_SAND_CHECK_FUNC_RESULT(rv, 20, exit);

  SOC_SAND_TBL_ITER_SET_BEGIN(&block_range.iter);
  block_range.entries_to_scan = SOC_SAND_TBL_ITER_SCAN_ALL;
  block_range.entries_to_act = SOC_SAND_TBL_ITER_SCAN_ALL;

  
  rv = _arad_pp_slb_slb_action_to_lem_action(unit, action, &mact_action);
  SOC_SAND_CHECK_FUNC_RESULT(rv, 30, exit);

  
  SOC_SAND_VERIFY(key.type == ARAD_PP_LEM_ACCESS_KEY_TYPE_SLB);
  SOC_SAND_VERIFY(key.prefix.value == ARAD_PP_LEM_ACCESS_KEY_PREFIX_FOR_SLB);
  SOC_SAND_VERIFY(key.prefix.nof_bits == ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS);
  SOC_SAND_VERIFY(key_mask.prefix.value == ARAD_PP_LEM_ACCESS_KEY_PREFIX_ALL_MASKED);
  SOC_SAND_VERIFY(key_mask.prefix.nof_bits == ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS);

  rv = arad_pp_frwrd_lem_traverse_internal_unsafe(
         unit, 
         &key,
         &key_mask,
         &lem_match_value_rule,
         &block_range,
         &mact_action,
         wait_till_finish,
         &nof_matched_entries_internal
       );
  SOC_SAND_CHECK_FUNC_RESULT(rv, 40, exit);

  if (nof_matched_entries) {
    *nof_matched_entries = nof_matched_entries_internal;
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

uint32
  arad_pp_api_impl_slb_get_block_verify(
    SOC_SAND_IN     int                                      unit,
    SOC_SAND_IN     SOC_PPC_SLB_TRAVERSE_MATCH_RULE * const *   match_rules,
    SOC_SAND_IN     uint32                                      nof_match_rules,
    SOC_SAND_INOUT  SOC_SAND_TABLE_BLOCK_RANGE *                block_range,
    SOC_SAND_OUT    SOC_PPC_SLB_ENTRY_KEY *                     slb_keys,
    SOC_SAND_OUT    SOC_PPC_SLB_ENTRY_VALUE *                   slb_vals,
    SOC_SAND_OUT    uint32 *                                    nof_entries
  )
{

  uint32 rv; 

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  rv = _arad_pp_slb_verify_match_rule_array(unit, match_rules, nof_match_rules);
  SOC_SAND_CHECK_FUNC_RESULT(rv, 10, exit);

  SOC_SAND_CHECK_NULL_INPUT(block_range);

  SOC_SAND_ERR_IF_NOT_EQUALS_VALUE(block_range->entries_to_act, SOC_PPC_SLB_MAX_ENTRIES_FOR_GET_BLOCK, 0, 20, exit);

  SOC_SAND_CHECK_NULL_INPUT(slb_keys);
  SOC_SAND_CHECK_NULL_INPUT(slb_vals);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

uint32
  arad_pp_api_impl_slb_get_block_unsafe(
    SOC_SAND_IN     int                                      unit,
    SOC_SAND_IN     SOC_PPC_SLB_TRAVERSE_MATCH_RULE * const *   match_rules,
    SOC_SAND_IN     uint32                                      nof_match_rules,
    SOC_SAND_INOUT  SOC_SAND_TABLE_BLOCK_RANGE *                block_range,
    SOC_SAND_OUT    SOC_PPC_SLB_ENTRY_KEY *                     slb_keys,
    SOC_SAND_OUT    SOC_PPC_SLB_ENTRY_VALUE *                   slb_vals,
    SOC_SAND_OUT    uint32 *                                    nof_entries
  )
{
  uint32 res;
  uint32 rv;
  ARAD_PP_LEM_ACCESS_KEY key;
  ARAD_PP_LEM_ACCESS_KEY key_mask;
  SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE lem_match_value_rule;
  uint8 not_accessed_only = FALSE;
  unsigned int i;
  ARAD_PP_LEM_ACCESS_KEY *out_lem_keys = NULL;
  ARAD_PP_LEM_ACCESS_PAYLOAD *out_lem_payloads = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_ALLOC(out_lem_keys, ARAD_PP_LEM_ACCESS_KEY, SOC_PPC_SLB_MAX_ENTRIES_FOR_GET_BLOCK, "out_lem_keys");
  ARAD_ALLOC(out_lem_payloads, ARAD_PP_LEM_ACCESS_PAYLOAD, SOC_PPC_SLB_MAX_ENTRIES_FOR_GET_BLOCK, "out_lem_payloads");
  
  
  rv = _arad_pp_slb_slb_rules_to_lem_keys(unit, match_rules, nof_match_rules, &key, &key_mask);
  SOC_SAND_CHECK_FUNC_RESULT(rv, 10, exit);

  rv = _arad_pp_slb_slb_rules_to_lem_value_rule(unit, match_rules, nof_match_rules, &lem_match_value_rule);
  SOC_SAND_CHECK_FUNC_RESULT(rv, 20, exit);

  rv = arad_pp_frwrd_lem_get_block_unsafe(
         unit, 
         &key,
         &key_mask,
         &lem_match_value_rule,
         not_accessed_only,
         block_range,
         out_lem_keys,
         out_lem_payloads,
         nof_entries
       );
  SOC_SAND_CHECK_FUNC_RESULT(rv, 40, exit);

  for (i = 0; i <*nof_entries; i++) {
    uint32 flow_label_arr[] = {0, 0};
    uint32 is_fec = 0;
    const uint32 lag_group_nof_bits = ARAD_PP_SLB_LAG_GROUP_MEMBER_NOF_BITS - _arad_pp_slb_lag_member_nof_bits(unit);
    const uint32 lag_group_mask = _arad_pp_slb_bit_mask(lag_group_nof_bits);

    SOC_SAND_VERIFY(out_lem_keys[i].type == ARAD_PP_LEM_ACCESS_KEY_TYPE_SLB);
    SOC_SAND_VERIFY(out_lem_keys[i].prefix.value == ARAD_PP_LEM_ACCESS_KEY_PREFIX_FOR_SLB);
    SOC_SAND_VERIFY(out_lem_keys[i].prefix.nof_bits == ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS);
    SOC_SAND_VERIFY(out_lem_keys[i].nof_params == ARAD_PP_LEM_ACCESS_KEY_NOF_PARAMS_FOR_SLB);

    SOC_SAND_VERIFY(out_lem_keys[i].param[0].nof_bits == ARAD_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_SLB);
    SOC_SAND_VERIFY(out_lem_keys[i].param[1].nof_bits == ARAD_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_SLB);
    SOC_SAND_VERIFY(out_lem_keys[i].param[2].nof_bits == ARAD_PP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_SLB);
    SOC_SAND_VERIFY(out_lem_keys[i].param[3].nof_bits == 0);
    SOC_SAND_VERIFY(out_lem_keys[i].param[4].nof_bits == 0);

    SOC_PPC_SLB_CLEAR(&slb_keys[i], SOC_PPC_SLB_OBJECT_TYPE_ENTRY_KEY);
    SOC_PPC_SLB_CLEAR(&slb_vals[i], SOC_PPC_SLB_OBJECT_TYPE_ENTRY_VALUE);

    
    SHR_BITCOPY_RANGE(flow_label_arr, 0, out_lem_keys[i].param[0].value, 0, ARAD_PP_LEM_ACCESS_KEY_PARAM0_IN_BITS_FOR_SLB);
    SHR_BITCOPY_RANGE(&slb_keys[i].lb_group.lag_ndx, 0, out_lem_keys[i].param[1].value, 0, ARAD_PP_LEM_ACCESS_KEY_PARAM1_IN_BITS_FOR_SLB);
    SHR_BITCOPY_RANGE(&is_fec, 0, out_lem_keys[i].param[2].value, 0, ARAD_PP_LEM_ACCESS_KEY_PARAM2_IN_BITS_FOR_SLB);
  
    COMPILER_64_SET(slb_keys[i].flow_label_id, flow_label_arr[1], flow_label_arr[0]);
    
    slb_keys[i].is_fec = is_fec;

    if (!slb_keys[i].is_fec) {
      
      slb_keys[i].lb_group.lag_ndx &= (ARAD_PP_SLB_LAG_GROUP_MEMBER_NOF_BITS);
    }

    
    slb_vals[i].lag_member_ndx = (out_lem_payloads[i].slb_lag & _arad_pp_slb_bit_mask(ARAD_PP_SLB_LAG_GROUP_MEMBER_NOF_BITS)) >> lag_group_nof_bits;
    slb_vals[i].lag_ndx = out_lem_payloads[i].slb_lag & lag_group_mask;
    slb_vals[i].ecmp_member_ndx = out_lem_payloads[i].slb_fec;
    slb_vals[i].lag_valid = out_lem_payloads[i].slb_lag_valid;
    slb_vals[i].ecmp_valid = out_lem_payloads[i].slb_fec_valid;

    SOC_SAND_VERIFY(!slb_vals[i].lag_valid || slb_vals[i].lag_ndx == slb_keys[i].lb_group.lag_ndx);
  }

exit:
  ARAD_FREE(out_lem_keys);
  ARAD_FREE(out_lem_payloads);
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}



STATIC uint32
  _arad_pp_slb_aging_time_to_final_entry(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 slb_aging_time_sec,
    SOC_SAND_OUT uint32 *final_age
  )
{
  uint32 rv;
  SOC_PPC_FRWRD_MACT_AGING_INFO aging_info;
  uint32 maximal_selectable_age = ARAD_PP_SLB_NOF_AGES - 2;
  uint32 aging_rate_per_age_sec;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_PPC_FRWRD_MACT_AGING_INFO_clear(&aging_info);

  rv = arad_pp_frwrd_mact_aging_info_get_unsafe(unit, &aging_info);
  SOC_SAND_CHECK_FUNC_RESULT(rv, 10, exit);

  
  
  
  
  
  
  
  
  

  
  

  

  aging_rate_per_age_sec = aging_info.aging_time.sec / ARAD_PP_SLB_NOF_AGES;

  if (slb_aging_time_sec >= (aging_rate_per_age_sec * ARAD_PP_SLB_NOF_AGES)) {
    *final_age = 0;
  } else {
    
    SOC_SAND_VERIFY(slb_aging_time_sec / aging_rate_per_age_sec <= maximal_selectable_age);
    *final_age = maximal_selectable_age - (slb_aging_time_sec / aging_rate_per_age_sec);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

uint32 
  arad_pp_api_impl_slb_set_global_cfg_verify(
    SOC_SAND_IN     int                                    unit,
    SOC_SAND_IN     SOC_PPC_SLB_CONFIGURATION_ITEM *          configuration_item
  )
{
  uint32 rv;
  SOC_PPC_SLB_OBJECT_TYPE valid_types[] = { 
    SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_SLB_ENTRY_AGING_TIME_IN_SECONDS,
    SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_LAG_HASH_FUNCTION,
    SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_LAG_HASH_SEED,
	SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_LAG_HASH_OFFSET,
    SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_ECMP_HASH_SEED,
	SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_ECMP_HASH_KEY_OFFSET,
	SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_CRC_HASH_SEED,
	SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_CRC_HASH_MASK,
  };
  uint32 nof_valid_types = 8;
  uint8 in_valid_types;
  SOC_PPC_SLB_OBJECT_TYPE type;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  rv = soc_ppc_slb_object_type((SOC_PPC_SLB_OBJECT*)configuration_item, &type);
  SOC_SAND_CHECK_FUNC_RESULT(rv, 10, exit);

  rv = _arad_pp_slb_object_type_in_types(
         unit, 
         SOC_PPC_SLB_DOWNCAST(configuration_item, SOC_PPC_SLB_OBJECT), 
         valid_types, 
         nof_valid_types, 
         &in_valid_types
       );
  SOC_SAND_CHECK_FUNC_RESULT(rv, 10, exit);

  if (!in_valid_types) {
    SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_MSG_STR("Invalid type for set_cfg.")));
  }

  rv = ARAD_PP_SLB_verify(unit,(SOC_PPC_SLB_OBJECT*)configuration_item);
  SOC_SAND_CHECK_FUNC_RESULT(rv, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

uint32 
  arad_pp_api_impl_slb_set_global_cfg_unsafe(
    SOC_SAND_IN     int                                    unit,
    SOC_SAND_IN     SOC_PPC_SLB_CONFIGURATION_ITEM *          configuration_item
  )
{
  uint32 rv;
  uint32 res;
  SOC_PPC_SLB_OBJECT_TYPE type;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  rv = soc_ppc_slb_object_type((SOC_PPC_SLB_OBJECT*)configuration_item, &type);
  SOC_SAND_CHECK_FUNC_RESULT(rv, 10, exit);

  switch (type) {
  case SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_SLB_ENTRY_AGING_TIME_IN_SECONDS:
    {
      SOC_PPC_SLB_CONFIGURATION_ITEM_SLB_ENTRY_AGING_TIME_IN_SECONDS *aging_cfg = (SOC_PPC_SLB_CONFIGURATION_ITEM_SLB_ENTRY_AGING_TIME_IN_SECONDS*)configuration_item;
      ARAD_PP_FRWRD_MACT_AGING_EVENT_TABLE aging_event_table;
      uint32 age_ndx;
      uint32 is_owned;
      uint32 final_age;

      ARAD_PP_FRWRD_MACT_AGING_EVENT_TABLE_clear(&aging_event_table);

      rv = _arad_pp_slb_aging_time_to_final_entry(unit, aging_cfg->age_time_in_seconds, &final_age);
      SOC_SAND_CHECK_FUNC_RESULT(rv, 20, exit);

      for (is_owned = 0; is_owned < ARAD_PP_SLB_NOF_OWNED; ++is_owned)
      {
        
        for (age_ndx = 0; age_ndx <= final_age; ++age_ndx)
        {
          aging_event_table.age_action[age_ndx][is_owned].aged_out = FALSE;
          aging_event_table.age_action[age_ndx][is_owned].deleted = TRUE;
          aging_event_table.age_action[age_ndx][is_owned].refreshed = FALSE;
        }

        for (; age_ndx < ARAD_PP_SLB_NOF_AGES; ++age_ndx)
        {
          aging_event_table.age_action[age_ndx][is_owned].aged_out = FALSE;
          aging_event_table.age_action[age_ndx][is_owned].deleted = FALSE;
          aging_event_table.age_action[age_ndx][is_owned].refreshed = FALSE;
        }
      }

      rv = arad_pp_frwrd_mact_age_conf_write(unit, ARAD_PP_SLB_PROG_CFG_PTR, &aging_event_table);
      SOC_SAND_CHECK_FUNC_RESULT(rv, 30, exit);
    }
    break;
  case SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_LAG_HASH_FUNCTION:
    {
      SOC_PPC_SLB_CONFIGURATION_ITEM_LAG_HASH_FUNCTION *hash_function_cfg = (SOC_PPC_SLB_CONFIGURATION_ITEM_LAG_HASH_FUNCTION*)configuration_item;
      uint8 hash_func = hash_function_cfg->hash_function;
      uint32 hw_val;

      rv = arad_pp_lag_hash_func_to_hw_val(unit, hash_func, &hw_val);
      SOC_SAND_CHECK_FUNC_RESULT(rv, 40, exit);
      
      rv = arad_pp_frwrd_fec_unique_polynomial_check(unit, hw_val, ARAD_PP_FRWRD_FEC_HASH_INDEX_FLEXIBLE);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(rv, 40, exit);

      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHB_CONSISTENT_HASHING_LB_KEY_CFGr, SOC_CORE_ALL, 0, CONSISTENT_HASHING_LAG_LB_HASH_INDEXf,  hw_val));
    }
    break;
  case SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_LAG_HASH_SEED:
    {
      SOC_PPC_SLB_CONFIGURATION_ITEM_LAG_HASH_SEED *lag_hash_seed_cfg = (SOC_PPC_SLB_CONFIGURATION_ITEM_LAG_HASH_SEED*)configuration_item;
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHB_CONSISTENT_HASHING_LB_KEY_CFGr, SOC_CORE_ALL, 0, CONSISTENT_HASHING_LAG_LB_KEY_SEEDf,  (uint32)lag_hash_seed_cfg->seed));
    }
    break;
  case SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_LAG_HASH_OFFSET:
    {
      SOC_PPC_SLB_CONFIGURATION_ITEM_LAG_HASH_OFFSET *hash_offset_cfg = (SOC_PPC_SLB_CONFIGURATION_ITEM_LAG_HASH_OFFSET*)configuration_item;
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHB_CONSISTENT_HASHING_LB_KEY_CFGr, SOC_CORE_ALL, 0, CONSISTENT_HASHING_LAG_LB_KEY_SHIFTf,  (uint32)hash_offset_cfg->offset));
    }
    break;
  case SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_ECMP_HASH_SEED:
    {
      SOC_PPC_SLB_CONFIGURATION_ITEM_ECMP_HASH_SEED *ecmp_hash_seed_cfg = (SOC_PPC_SLB_CONFIGURATION_ITEM_ECMP_HASH_SEED*)configuration_item;
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHB_CONSISTENT_HASHING_LB_KEY_CFGr, SOC_CORE_ALL, 0, CONSISTENT_HASHING_ECMP_LB_KEY_SEEDf,  (uint32)ecmp_hash_seed_cfg->seed));
    }
    break;
  case SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_CRC_HASH_SEED:
    {
      SOC_PPC_SLB_CONFIGURATION_ITEM_CRC_HASH_SEED *crc_hash_seed_cfg = (SOC_PPC_SLB_CONFIGURATION_ITEM_CRC_HASH_SEED*)configuration_item;
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHB_CONSISTENT_HASHING_VARIABLESr, SOC_CORE_ALL, 0, CONSISTENT_HASHING_PROGRAM_CRC_SEEDf,  (uint32)crc_hash_seed_cfg->seed));
    }
    break;
#ifdef BCM_JERICHO_SUPPORT
  case SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_ECMP_HASH_KEY_OFFSET:
	{
	  SOC_PPC_SLB_CONFIGURATION_ITEM_ECMP_HASH_KEY_OFFSET *ecmp_hash_key_offset_cfg = (SOC_PPC_SLB_CONFIGURATION_ITEM_ECMP_HASH_KEY_OFFSET*)configuration_item;
	  uint32 mem;
	  switch (ecmp_hash_key_offset_cfg->index) {
	  case 0:
		  mem = ECMP_LB_KEY_CONSISTENT_HASHING_DATA_0_SHIFTf;
	      break;
	  case 1:
	  default:
		  mem = ECMP_LB_KEY_CONSISTENT_HASHING_DATA_1_SHIFTf;
	      break;
	  }
	  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  90,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, IHB_ECMP_LB_KEY_CFGr, SOC_CORE_ALL, 0, mem,  (uint32)ecmp_hash_key_offset_cfg->offset)); 
	}
	break;
  case SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_CRC_HASH_MASK:
	{
	  SOC_PPC_SLB_CONFIGURATION_ITEM_CRC_HASH_MASK *crc_hash_mask_cfg = (SOC_PPC_SLB_CONFIGURATION_ITEM_CRC_HASH_MASK*)configuration_item;
	  uint32 mem;
	  switch (crc_hash_mask_cfg->index) {
	  case 0:
		  mem = CONSISTENT_HASHING_PROGRAM_CRC_MASK_0f;
		  break;
	  case 1:
	      mem = CONSISTENT_HASHING_PROGRAM_CRC_MASK_1f;
		  break;
      case 2:
	      mem = CONSISTENT_HASHING_PROGRAM_CRC_MASK_2f;
		  break;
	  case 3:
	  default:
	      mem = CONSISTENT_HASHING_PROGRAM_CRC_MASK_3f;
		  break;
	  }
	  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  110,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, IHB_CONSISTENT_HASHING_VARIABLESr, SOC_CORE_ALL, 0, mem,  (uint32)crc_hash_mask_cfg->mask)); 
	}
	break;
#endif
  default:
    SOC_SAND_VERIFY(FALSE);
    break;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

uint32 
  arad_pp_api_impl_slb_get_global_cfg_verify(
    SOC_SAND_IN     int                                    unit,
    SOC_SAND_INOUT     SOC_PPC_SLB_CONFIGURATION_ITEM *          configuration_item
  )
{
  uint32 rv;
  SOC_PPC_SLB_OBJECT_TYPE valid_types[] = { 
    SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_SLB_ENTRY_AGING_TIME_IN_SECONDS,
    SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_LAG_HASH_FUNCTION,
    SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_LAG_HASH_SEED,
	SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_LAG_HASH_OFFSET,
    SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_ECMP_HASH_SEED,
	SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_ECMP_HASH_KEY_OFFSET,
	SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_CRC_HASH_SEED,
	SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_CRC_HASH_MASK,
  };

  uint32 nof_valid_types = 8;
  uint8 in_valid_types;
  uint8 is_type;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  rv = _arad_pp_slb_object_type_in_types(unit, SOC_PPC_SLB_DOWNCAST(configuration_item, SOC_PPC_SLB_OBJECT), valid_types, nof_valid_types, &in_valid_types);
  SOC_SAND_CHECK_FUNC_RESULT(rv, 10, exit);

  if (!in_valid_types) {
    SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_MSG_STR("Invalid type for get_cfg.")));
  }

  
  rv = soc_ppc_slb_object_is((SOC_PPC_SLB_OBJECT*)configuration_item, SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_SLB_ENTRY_AGING_TIME_IN_SECONDS, &is_type);
  SOC_SAND_CHECK_FUNC_RESULT(rv, 30, exit);

  if (is_type) {
    rv = ARAD_PP_SLB_verify(unit,(SOC_PPC_SLB_OBJECT*)configuration_item);
    SOC_SAND_CHECK_FUNC_RESULT(rv, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

uint32 
  arad_pp_api_impl_slb_get_global_cfg_unsafe(
    SOC_SAND_IN     int                                    unit,
    SOC_SAND_INOUT  SOC_PPC_SLB_CONFIGURATION_ITEM *          configuration_item
  )
{
  uint32 rv;
  uint32 res;
  uint32 field_val;
  SOC_PPC_SLB_OBJECT_TYPE type;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  rv = soc_ppc_slb_object_type((SOC_PPC_SLB_OBJECT*)configuration_item, &type);
  SOC_SAND_CHECK_FUNC_RESULT(rv, 10, exit);

  switch (type) {
  case SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_SLB_ENTRY_AGING_TIME_IN_SECONDS:
    {
      SOC_PPC_SLB_CONFIGURATION_ITEM_SLB_ENTRY_AGING_TIME_IN_SECONDS *aging_cfg = (SOC_PPC_SLB_CONFIGURATION_ITEM_SLB_ENTRY_AGING_TIME_IN_SECONDS*)configuration_item;
      SOC_PPC_FRWRD_MACT_AGING_INFO aging_info;

      uint32 final_age;
      uint32 aging_rate_per_age_sec;

      rv = _arad_pp_slb_aging_time_to_final_entry(unit, aging_cfg->age_time_in_seconds, &final_age);
      SOC_SAND_CHECK_FUNC_RESULT(rv, 20, exit);

      rv = arad_pp_frwrd_mact_aging_info_get_unsafe(unit, &aging_info);
      SOC_SAND_CHECK_FUNC_RESULT(rv, 30, exit);

      aging_rate_per_age_sec = aging_info.aging_time.sec / ARAD_PP_SLB_NOF_AGES;

      aging_cfg->age_time_in_seconds = (ARAD_PP_SLB_NOF_AGES - 1 - final_age) * aging_rate_per_age_sec;
    }
    break;
  case SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_LAG_HASH_FUNCTION:
    {
      SOC_PPC_SLB_CONFIGURATION_ITEM_LAG_HASH_FUNCTION *hash_function_cfg = (SOC_PPC_SLB_CONFIGURATION_ITEM_LAG_HASH_FUNCTION*)configuration_item;
      uint32 hw_val;
      uint8 hash_func;
      
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IHB_CONSISTENT_HASHING_LB_KEY_CFGr, SOC_CORE_ALL, 0, CONSISTENT_HASHING_LAG_LB_HASH_INDEXf, &hw_val));
      rv = arad_pp_lag_hash_func_from_hw_val(unit, hw_val, &hash_func);
      SOC_SAND_CHECK_FUNC_RESULT(rv, 40, exit);

      hash_function_cfg->hash_function = hash_func;
    }
    break;
  case SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_LAG_HASH_SEED:
    {
      SOC_PPC_SLB_CONFIGURATION_ITEM_LAG_HASH_SEED *lag_hash_seed_cfg = (SOC_PPC_SLB_CONFIGURATION_ITEM_LAG_HASH_SEED*)configuration_item;
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IHB_CONSISTENT_HASHING_LB_KEY_CFGr, SOC_CORE_ALL, 0, CONSISTENT_HASHING_LAG_LB_KEY_SEEDf, &field_val));
      lag_hash_seed_cfg->seed = field_val;
    }
    break;
  case SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_LAG_HASH_OFFSET:
    {
      SOC_PPC_SLB_CONFIGURATION_ITEM_LAG_HASH_OFFSET *hash_offset_cfg = (SOC_PPC_SLB_CONFIGURATION_ITEM_LAG_HASH_OFFSET*)configuration_item;
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IHB_CONSISTENT_HASHING_LB_KEY_CFGr, SOC_CORE_ALL, 0, CONSISTENT_HASHING_LAG_LB_KEY_SHIFTf, &field_val));
      hash_offset_cfg->offset = field_val;
    }
    break;
  case SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_ECMP_HASH_SEED:
    {
      SOC_PPC_SLB_CONFIGURATION_ITEM_ECMP_HASH_SEED *ecmp_hash_seed_cfg = (SOC_PPC_SLB_CONFIGURATION_ITEM_ECMP_HASH_SEED*)configuration_item;
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IHB_CONSISTENT_HASHING_LB_KEY_CFGr, SOC_CORE_ALL, 0, CONSISTENT_HASHING_ECMP_LB_KEY_SEEDf, &field_val));
      ecmp_hash_seed_cfg->seed = field_val;
    }
    break;
  case SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_CRC_HASH_SEED:
    {
      SOC_PPC_SLB_CONFIGURATION_ITEM_CRC_HASH_SEED *crc_hash_seed_cfg = (SOC_PPC_SLB_CONFIGURATION_ITEM_CRC_HASH_SEED*)configuration_item;
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IHB_CONSISTENT_HASHING_VARIABLESr, SOC_CORE_ALL, 0, CONSISTENT_HASHING_PROGRAM_CRC_SEEDf, &field_val));
      crc_hash_seed_cfg->seed = field_val;
    }
    break;
  
#ifdef BCM_JERICHO_SUPPORT
  case SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_ECMP_HASH_KEY_OFFSET:
	{
	  SOC_PPC_SLB_CONFIGURATION_ITEM_ECMP_HASH_KEY_OFFSET *ecmp_hash_key_offset_cfg = (SOC_PPC_SLB_CONFIGURATION_ITEM_ECMP_HASH_KEY_OFFSET*)configuration_item;
	  uint32 mem;
	  switch (ecmp_hash_key_offset_cfg->index) {
	  case 0:
		  mem = ECMP_LB_KEY_CONSISTENT_HASHING_DATA_0_SHIFTf;
		  break;
	  case 1:
	  default:
		  mem = ECMP_LB_KEY_CONSISTENT_HASHING_DATA_1_SHIFTf;
		  break;
	  }
	  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  90,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_read(unit, IHB_ECMP_LB_KEY_CFGr, SOC_CORE_ALL, 0, mem, &field_val)); 
	  ecmp_hash_key_offset_cfg->offset = field_val;
	}
	break;
  case SOC_PPC_SLB_OBJECT_TYPE_CONFIGURATION_ITEM_CRC_HASH_MASK:
	{
	  SOC_PPC_SLB_CONFIGURATION_ITEM_CRC_HASH_MASK *crc_hash_mask_cfg = (SOC_PPC_SLB_CONFIGURATION_ITEM_CRC_HASH_MASK*)configuration_item;
	  uint32 mem;
	  switch (crc_hash_mask_cfg->index) {
	  case 0:
		  mem = CONSISTENT_HASHING_PROGRAM_CRC_MASK_0f;
		  break;
	  case 1:
		  mem = CONSISTENT_HASHING_PROGRAM_CRC_MASK_1f;
		  break;
	  case 2:
		  mem = CONSISTENT_HASHING_PROGRAM_CRC_MASK_2f;
		  break;
	  case 3:
	  default:
		  mem = CONSISTENT_HASHING_PROGRAM_CRC_MASK_3f;
		  break;
	  }
	  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  100,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_read(unit, IHB_CONSISTENT_HASHING_VARIABLESr, SOC_CORE_ALL, 0, mem, &field_val)); 
	  crc_hash_mask_cfg->mask = field_val;
	}
	break;
#endif
  default:
    SOC_SAND_VERIFY(FALSE);
    break;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#include <soc/dpp/SAND/Utils/sand_footer.h>

