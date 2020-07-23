/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_PPC



#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/PPC/ppc_api_slb.h>





struct _soc_ppc_slb_object_info_s;
typedef struct _soc_ppc_slb_object_info_s _soc_ppc_slb_object_info ;






typedef void (*_soc_ppc_slb_object_print_callback)(SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj, SOC_SAND_IN _soc_ppc_slb_object_info *info);


struct _soc_ppc_slb_object_info_s {

  
  SOC_PPC_SLB_OBJECT_TYPE type;

  
  uint32 size;

  
  const char *name;

  
  SOC_PPC_SLB_OBJECT_TYPE base_class;

  
  uint8 is_abstract;

  
  _soc_ppc_slb_object_print_callback print_cb;

};



  enum {
  
    #define PPC_API_SLB_INTERNAL_OBJECT_LIST_ONLY_OBJECT_ACTION(suffix) SOC_PPC_SLB_OBJECT_INTERNAL_CHECK_ ## suffix = SOC_PPC_SLB_OBJECT_TYPE_ ## suffix,
    #define PPC_API_SLB_INTERNAL_OBJECT_LIST_ONLY

    #include <soc/dpp/PPC/ppc_api_slb.h>

    #undef PPC_API_SLB_INTERNAL_OBJECT_LIST_ONLY
    #undef PPC_API_SLB_INTERNAL_OBJECT_LIST_ONLY_OBJECT_ACTION

    SOC_PPC_SLB_OBJECT_INTERNAL_CHECK_NOF_OBJECTS
  } SOC_PPC_SLB_OBJECT_INTERNAL_CHECK;
  SOC_PPC_SLB_C_ASSERT((unsigned int)SOC_PPC_SLB_OBJECT_INTERNAL_CHECK_NOF_OBJECTS == (unsigned int)SOC_PPC_SLB_NOF_OBJECT_TYPES);



  #define PPC_API_SLB_INTERNAL_OBJECT_LIST_ONLY_OBJECT_ACTION(suffix) STATIC void SOC_PPC_SLB_ ## suffix ## _print(SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj, SOC_SAND_IN _soc_ppc_slb_object_info *info);
  #define PPC_API_SLB_INTERNAL_OBJECT_LIST_ONLY

  #include <soc/dpp/PPC/ppc_api_slb.h>

  #undef PPC_API_SLB_INTERNAL_OBJECT_LIST_ONLY
  #undef PPC_API_SLB_INTERNAL_OBJECT_LIST_ONLY_OBJECT_ACTION


static _soc_ppc_slb_object_info _soc_ppc_slb_object_infos[] = { 
   {
    SOC_PPC_SLB_OBJECT_TYPE_BASE,
    sizeof(SOC_PPC_SLB_OBJECT_TYPE),
    "SOC_PPC_SLB_OBJECT_TYPE",
    SOC_PPC_SLB_OBJECT_TYPE_BASE,
    1,
    NULL
  },

#if SOC_PPC_DEBUG_IS_LVL1
  #define PRINT_CB(suffix) SOC_PPC_SLB_ ## suffix ## _print
#else
  #define PRINT_CB(suffix) NULL
#endif 

  #define OBJECT_INFO_GENERAL(suffix, parent_suffix, is_abstract)             \
                   \
                                   \
     {                          \
      SOC_PPC_SLB_OBJECT_TYPE_ ## suffix,                           \
      sizeof(SOC_PPC_SLB_ ## suffix),                               \
      "SOC_PPC_SLB_OBJECT_" #suffix,                                \
      SOC_PPC_SLB_OBJECT_TYPE_ ## parent_suffix,              \
      is_abstract,                                           \
      PRINT_CB(suffix),                                         \
      }

  #define OBJECT_INFO(suffix, parent_suffix)                      OBJECT_INFO_GENERAL(suffix, parent_suffix, 0)
  #define ABSTRACT_OBJECT_INFO(suffix, parent_suffix)             OBJECT_INFO_GENERAL(suffix, parent_suffix, 1)

  
  ABSTRACT_OBJECT_INFO(TRAVERSE_MATCH_RULE, BASE),
    OBJECT_INFO(TRAVERSE_MATCH_RULE_ALL_LAG, TRAVERSE_MATCH_RULE),
    OBJECT_INFO(TRAVERSE_MATCH_RULE_ALL_ECMP, TRAVERSE_MATCH_RULE),
    ABSTRACT_OBJECT_INFO(TRAVERSE_MATCH_RULE_LB_GROUP, TRAVERSE_MATCH_RULE),
      OBJECT_INFO(TRAVERSE_MATCH_RULE_LB_GROUP_LAG, TRAVERSE_MATCH_RULE_LB_GROUP),
      OBJECT_INFO(TRAVERSE_MATCH_RULE_LB_GROUP_ECMP, TRAVERSE_MATCH_RULE_LB_GROUP),
    ABSTRACT_OBJECT_INFO(TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER, TRAVERSE_MATCH_RULE),
      OBJECT_INFO(TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER_LAG, TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER),
      OBJECT_INFO(TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER_ECMP, TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER),

  ABSTRACT_OBJECT_INFO(TRAVERSE_ACTION, BASE),
    OBJECT_INFO(TRAVERSE_ACTION_COUNT,  TRAVERSE_ACTION),
    OBJECT_INFO(TRAVERSE_ACTION_UPDATE, TRAVERSE_ACTION),
    OBJECT_INFO(TRAVERSE_ACTION_REMOVE, TRAVERSE_ACTION),
    
  ABSTRACT_OBJECT_INFO(TRAVERSE_UPDATE_VALUE, BASE),
    OBJECT_INFO(TRAVERSE_UPDATE_VALUE_LAG_MEMBER, TRAVERSE_UPDATE_VALUE),
    OBJECT_INFO(TRAVERSE_UPDATE_VALUE_ECMP_MEMBER, TRAVERSE_UPDATE_VALUE),

  OBJECT_INFO(ENTRY_KEY, BASE),
  OBJECT_INFO(ENTRY_VALUE, BASE), 

  ABSTRACT_OBJECT_INFO(CONFIGURATION_ITEM, BASE),
    OBJECT_INFO(CONFIGURATION_ITEM_SLB_ENTRY_AGING_TIME_IN_SECONDS, CONFIGURATION_ITEM),
    OBJECT_INFO(CONFIGURATION_ITEM_LAG_HASH_FUNCTION, CONFIGURATION_ITEM),
    OBJECT_INFO(CONFIGURATION_ITEM_LAG_HASH_SEED, CONFIGURATION_ITEM),
    OBJECT_INFO(CONFIGURATION_ITEM_LAG_HASH_OFFSET, CONFIGURATION_ITEM),
    OBJECT_INFO(CONFIGURATION_ITEM_ECMP_HASH_SEED, CONFIGURATION_ITEM),
    OBJECT_INFO(CONFIGURATION_ITEM_ECMP_HASH_KEY_OFFSET, CONFIGURATION_ITEM),
    OBJECT_INFO(CONFIGURATION_ITEM_CRC_HASH_SEED, CONFIGURATION_ITEM),
    OBJECT_INFO(CONFIGURATION_ITEM_CRC_HASH_MASK, CONFIGURATION_ITEM),

  #undef ABSTRACT_OBJECT_INFO
  #undef OBJECT_INFO
  #undef OBJECT_INFO_GENERAL
  #undef PRINT_CB
};

SOC_PPC_SLB_C_ASSERT((sizeof(_soc_ppc_slb_object_infos) / sizeof(_soc_ppc_slb_object_infos[0])) == SOC_PPC_SLB_NOF_OBJECT_TYPES);







#if SOC_PPC_DEBUG_IS_LVL1

STATIC void SOC_PPC_SLB_TRAVERSE_MATCH_RULE_print(SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj, SOC_SAND_IN _soc_ppc_slb_object_info *info)
{
}

STATIC void SOC_PPC_SLB_TRAVERSE_MATCH_RULE_ALL_LAG_print(SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj, SOC_SAND_IN _soc_ppc_slb_object_info *info)
{
}

STATIC void SOC_PPC_SLB_TRAVERSE_MATCH_RULE_ALL_ECMP_print(SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj, SOC_SAND_IN _soc_ppc_slb_object_info *info)
{
}

STATIC void SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_print(SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj, SOC_SAND_IN _soc_ppc_slb_object_info *info)
{
}

STATIC void SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_LAG_print(SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj, SOC_SAND_IN _soc_ppc_slb_object_info *info)
{
  SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_LAG *obj = (SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_LAG*)untyped_obj;
  
  LOG_CLI((BSL_META(".lag_ndx = %d(0x%x).\n"), obj->lag_ndx, obj->lag_ndx));
}

STATIC void SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_ECMP_print(SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj, SOC_SAND_IN _soc_ppc_slb_object_info *info)
{
  SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_ECMP *obj = (SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_ECMP*)untyped_obj;
  
  LOG_CLI((BSL_META(".ecmp_ndx = %d(0x%x).\n"), obj->ecmp_ndx, obj->ecmp_ndx));
}

STATIC void SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER_print(SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj, SOC_SAND_IN _soc_ppc_slb_object_info *info)
{
}

STATIC void SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER_LAG_print(SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj, SOC_SAND_IN _soc_ppc_slb_object_info *info)
{
  SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER_LAG *obj = (SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER_LAG*)untyped_obj;
  
  LOG_CLI((BSL_META(".lag_member_ndx = %d(0x%x).\n"), obj->lag_member_ndx, obj->lag_member_ndx));
}

STATIC void SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER_ECMP_print(SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj, SOC_SAND_IN _soc_ppc_slb_object_info *info)
{
  SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER_ECMP *obj = (SOC_PPC_SLB_TRAVERSE_MATCH_RULE_LB_GROUP_MEMBER_ECMP*)untyped_obj;
  
  LOG_CLI((BSL_META(".ecmp_member_ndx = %d(0x%x).\n"), obj->ecmp_member_ndx, obj->ecmp_member_ndx));
}

STATIC void SOC_PPC_SLB_TRAVERSE_UPDATE_VALUE_print(SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj, SOC_SAND_IN _soc_ppc_slb_object_info *info)
{
}

STATIC void SOC_PPC_SLB_TRAVERSE_UPDATE_VALUE_LAG_MEMBER_print(SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj, SOC_SAND_IN _soc_ppc_slb_object_info *info)
{
  SOC_PPC_SLB_TRAVERSE_UPDATE_VALUE_LAG_MEMBER *obj = (SOC_PPC_SLB_TRAVERSE_UPDATE_VALUE_LAG_MEMBER*)untyped_obj;
  
  LOG_CLI((BSL_META(".new_lag_member_ndx = %d(0x%x).\n"), obj->new_lag_member_ndx, obj->new_lag_member_ndx));
}

STATIC void SOC_PPC_SLB_TRAVERSE_UPDATE_VALUE_ECMP_MEMBER_print(SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj, SOC_SAND_IN _soc_ppc_slb_object_info *info)
{
  SOC_PPC_SLB_TRAVERSE_UPDATE_VALUE_ECMP_MEMBER *obj = (SOC_PPC_SLB_TRAVERSE_UPDATE_VALUE_ECMP_MEMBER*)untyped_obj;
  
  LOG_CLI((BSL_META(".new_ecmp_member_ndx = %d(0x%x).\n"), obj->new_ecmp_member_ndx, obj->new_ecmp_member_ndx));
}

STATIC void SOC_PPC_SLB_TRAVERSE_ACTION_print(SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj, SOC_SAND_IN _soc_ppc_slb_object_info *info)
{
}

STATIC void SOC_PPC_SLB_TRAVERSE_ACTION_COUNT_print(SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj, SOC_SAND_IN _soc_ppc_slb_object_info *info)
{
}

STATIC void SOC_PPC_SLB_TRAVERSE_ACTION_REMOVE_print(SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj, SOC_SAND_IN _soc_ppc_slb_object_info *info)
{
}

STATIC void SOC_PPC_SLB_TRAVERSE_ACTION_UPDATE_print(SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj, SOC_SAND_IN _soc_ppc_slb_object_info *info)
{
  SOC_PPC_SLB_TRAVERSE_ACTION_UPDATE *obj = (SOC_PPC_SLB_TRAVERSE_ACTION_UPDATE*)untyped_obj;
  
  LOG_CLI((BSL_META(".traverse_update_value = 0x%p.\n"), (void *)(obj->traverse_update_value)));
  
}

STATIC void SOC_PPC_SLB_ENTRY_KEY_print(SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj, SOC_SAND_IN _soc_ppc_slb_object_info *info)
{
  SOC_PPC_SLB_ENTRY_KEY *obj = (SOC_PPC_SLB_ENTRY_KEY*)untyped_obj;
  
  LOG_CLI((BSL_META(".flow_label_id = 0x%08x%08x.\n"), COMPILER_64_HI(obj->flow_label_id), COMPILER_64_LO(obj->flow_label_id)));
  LOG_CLI((BSL_META(".is_fec = %d.\n"), obj->is_fec));
  LOG_CLI((BSL_META(".lb_group = %d(0x%x).\n"), obj->lb_group.lag_ndx, obj->lb_group.lag_ndx));
}

STATIC void SOC_PPC_SLB_ENTRY_VALUE_print(SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj, SOC_SAND_IN _soc_ppc_slb_object_info *info)
{
  SOC_PPC_SLB_ENTRY_VALUE *obj = (SOC_PPC_SLB_ENTRY_VALUE*)untyped_obj;
  
  LOG_CLI((BSL_META(".ecmp_member_ndx = %d(0x%x).\n"), obj->ecmp_member_ndx, obj->ecmp_member_ndx));
  LOG_CLI((BSL_META(".ecmp_valid = %d.\n"), obj->ecmp_valid));
  LOG_CLI((BSL_META(".lag_ndx = %d(0x%x).\n"), obj->lag_ndx, obj->lag_ndx));
  LOG_CLI((BSL_META(".lag_member_ndx = %d(0x%x).\n"), obj->lag_member_ndx, obj->lag_member_ndx));
  LOG_CLI((BSL_META(".lag_valid = %d.\n"), obj->lag_valid));
}

STATIC void SOC_PPC_SLB_CONFIGURATION_ITEM_print(SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj, SOC_SAND_IN _soc_ppc_slb_object_info *info)
{
}

STATIC void SOC_PPC_SLB_CONFIGURATION_ITEM_SLB_ENTRY_AGING_TIME_IN_SECONDS_print(SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj, SOC_SAND_IN _soc_ppc_slb_object_info *info)
{
  SOC_PPC_SLB_CONFIGURATION_ITEM_SLB_ENTRY_AGING_TIME_IN_SECONDS *obj = (SOC_PPC_SLB_CONFIGURATION_ITEM_SLB_ENTRY_AGING_TIME_IN_SECONDS*)untyped_obj;
  
  LOG_CLI((BSL_META(".age_time_in_seconds = %d(0x%x).\n"), obj->age_time_in_seconds, obj->age_time_in_seconds));
}

STATIC void SOC_PPC_SLB_CONFIGURATION_ITEM_LAG_HASH_FUNCTION_print(SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj, SOC_SAND_IN _soc_ppc_slb_object_info *info)
{
  SOC_PPC_SLB_CONFIGURATION_ITEM_LAG_HASH_FUNCTION *obj = (SOC_PPC_SLB_CONFIGURATION_ITEM_LAG_HASH_FUNCTION*)untyped_obj;
  
  LOG_CLI((BSL_META(".hash_function = 0x%02x).\n"), obj->hash_function));
}

STATIC void SOC_PPC_SLB_CONFIGURATION_ITEM_LAG_HASH_SEED_print(SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj, SOC_SAND_IN _soc_ppc_slb_object_info *info)
{
  SOC_PPC_SLB_CONFIGURATION_ITEM_LAG_HASH_SEED *obj = (SOC_PPC_SLB_CONFIGURATION_ITEM_LAG_HASH_SEED*)untyped_obj;
  
  LOG_CLI((BSL_META(".seed = 0x%04x).\n"), obj->seed));
}

STATIC void SOC_PPC_SLB_CONFIGURATION_ITEM_ECMP_HASH_SEED_print(SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj, SOC_SAND_IN _soc_ppc_slb_object_info *info)
{
  SOC_PPC_SLB_CONFIGURATION_ITEM_ECMP_HASH_SEED *obj = (SOC_PPC_SLB_CONFIGURATION_ITEM_ECMP_HASH_SEED*)untyped_obj;
  
  LOG_CLI((BSL_META(".seed = 0x%04x).\n"), obj->seed));
}

STATIC void SOC_PPC_SLB_CONFIGURATION_ITEM_LAG_HASH_OFFSET_print(SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj, SOC_SAND_IN _soc_ppc_slb_object_info *info)
{
  SOC_PPC_SLB_CONFIGURATION_ITEM_LAG_HASH_OFFSET *obj = (SOC_PPC_SLB_CONFIGURATION_ITEM_LAG_HASH_OFFSET*)untyped_obj;
  
  LOG_CLI((BSL_META(".offset = 0x%04x).\n"), obj->offset));
}

STATIC void SOC_PPC_SLB_CONFIGURATION_ITEM_ECMP_HASH_KEY_OFFSET_print(SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj, SOC_SAND_IN _soc_ppc_slb_object_info *info)
{
  SOC_PPC_SLB_CONFIGURATION_ITEM_ECMP_HASH_KEY_OFFSET *obj = (SOC_PPC_SLB_CONFIGURATION_ITEM_ECMP_HASH_KEY_OFFSET*)untyped_obj;
  
  LOG_CLI((BSL_META(".index = %d, .offset = 0x%06x).\n"), obj->index, obj->offset));
}

STATIC void SOC_PPC_SLB_CONFIGURATION_ITEM_CRC_HASH_SEED_print(SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj, SOC_SAND_IN _soc_ppc_slb_object_info *info)
{
  SOC_PPC_SLB_CONFIGURATION_ITEM_CRC_HASH_SEED *obj = (SOC_PPC_SLB_CONFIGURATION_ITEM_CRC_HASH_SEED*)untyped_obj;
  
  LOG_CLI((BSL_META(".seed = 0x%04x).\n"), obj->seed));
}

STATIC void SOC_PPC_SLB_CONFIGURATION_ITEM_CRC_HASH_MASK_print(SOC_SAND_IN SOC_PPC_SLB_OBJECT *untyped_obj, SOC_SAND_IN _soc_ppc_slb_object_info *info)
{
  SOC_PPC_SLB_CONFIGURATION_ITEM_CRC_HASH_MASK *obj = (SOC_PPC_SLB_CONFIGURATION_ITEM_CRC_HASH_MASK*)untyped_obj;
  
  LOG_CLI((BSL_META(".index = %d, .mask = 0x%16x).\n"), obj->index, obj->mask));
}

#endif 






typedef uint32 (*_soc_ppc_slb_traverse_inheritance_tree_callback)(
                                                               SOC_SAND_IN SOC_PPC_SLB_OBJECT *object, 
                                                               SOC_SAND_IN _soc_ppc_slb_object_info *info, 
                                                               SOC_SAND_IN int depth,
                                                               void *opaque
                                                              );

STATIC uint32 
  _soc_ppc_slb_traverse_inheritance_tree(
    SOC_SAND_IN SOC_PPC_SLB_OBJECT *object, 
    SOC_SAND_IN _soc_ppc_slb_traverse_inheritance_tree_callback callback,
    void *opaque
  )
{
  int depth = 0;
  uint32 rv;
  static const int _soc_ppc_slb_max_inheritance_depth = 16;
  _soc_ppc_slb_object_info info;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(object);

  
  SOC_SAND_VERIFY(object->type < SOC_PPC_SLB_NOF_OBJECT_TYPES);

  info = _soc_ppc_slb_object_infos[object->type];

  for ( depth = 0; (info.type != SOC_PPC_SLB_OBJECT_TYPE_BASE) && (depth < _soc_ppc_slb_max_inheritance_depth); depth++) {

    SOC_SAND_VERIFY(info.base_class < SOC_PPC_SLB_NOF_OBJECT_TYPES);
    info = _soc_ppc_slb_object_infos[info.base_class];
  
    rv = callback(object, &info, depth, opaque);
    SOC_SAND_CHECK_FUNC_RESULT(rv, 100 + depth, exit);
  }
  
  SOC_SAND_VERIFY(depth < _soc_ppc_slb_max_inheritance_depth);

  
  SOC_SAND_VERIFY(info.type == SOC_PPC_SLB_OBJECT_TYPE_BASE);
  rv = callback(object, &info, depth, (void*)opaque);
  SOC_SAND_CHECK_FUNC_RESULT(rv, 200, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void SOC_PPC_SLB_clear(SOC_SAND_OUT SOC_PPC_SLB_OBJECT *info, SOC_SAND_IN SOC_PPC_SLB_OBJECT_TYPE type)
{
  _soc_ppc_slb_object_info obj_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  
  SOC_SAND_VERIFY(type < SOC_PPC_SLB_NOF_OBJECT_TYPES);

  obj_info = _soc_ppc_slb_object_infos[type];

  SOC_SAND_VERIFY(!obj_info.is_abstract);
  
  sal_memset((void*)info, 0x0, obj_info.size);

  info->type = type;

  SOC_SAND_MAGIC_NUM_SET;

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PPC_DEBUG_IS_LVL1

STATIC uint32 _soc_ppc_slb_trv_cb_print(SOC_SAND_IN SOC_PPC_SLB_OBJECT *object, SOC_SAND_IN _soc_ppc_slb_object_info *info, int depth, void *opaque)
{

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  
  if (depth) {
    LOG_CLI((BSL_META("base[%s]:\n"), info->name));
  } else {
    LOG_CLI((BSL_META("%s:\n"), info->name));
  }
  
  if (info->type == SOC_PPC_SLB_OBJECT_TYPE_BASE) {
    LOG_CLI((BSL_META("\t.type = %d[%s].\n"), object->type, info->name));
  } else if (info->print_cb) {
    info->print_cb(object, info);
  }

  SOC_SAND_EXIT_NO_ERROR;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void SOC_PPC_SLB_OBJECT_print(SOC_SAND_IN SOC_PPC_SLB_OBJECT *object)
{
  uint32 rv COMPILER_ATTRIBUTE((unused));

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  SOC_SAND_CHECK_NULL_INPUT(object);

  rv = _soc_ppc_slb_traverse_inheritance_tree(object, _soc_ppc_slb_trv_cb_print, NULL);
  
  SOC_SAND_VERIFY(rv == SOC_SAND_SUCCESS);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 

typedef struct {
  uint8 is_answer;
  SOC_PPC_SLB_OBJECT_TYPE is_type;
} _soc_ppc_slb_trv_cb_is_data;

STATIC uint32 _soc_ppc_slb_trv_cb_is(SOC_SAND_IN SOC_PPC_SLB_OBJECT *object, SOC_SAND_IN _soc_ppc_slb_object_info *info, int depth, void *opaque)
{
  _soc_ppc_slb_trv_cb_is_data *data = (_soc_ppc_slb_trv_cb_is_data *)opaque;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  if (info->type == data->is_type) {
    data->is_answer = TRUE;
  }

  SOC_SAND_EXIT_NO_ERROR;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

uint32 soc_ppc_slb_object_is(SOC_SAND_IN SOC_PPC_SLB_OBJECT *object, SOC_SAND_IN SOC_PPC_SLB_OBJECT_TYPE type, SOC_SAND_OUT uint8 *is_type)
{
  _soc_ppc_slb_trv_cb_is_data data;
  uint32 rv;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  SOC_SAND_CHECK_NULL_INPUT(object);

  data.is_answer = 0;
  data.is_type = type;

  rv = _soc_ppc_slb_traverse_inheritance_tree(object, _soc_ppc_slb_trv_cb_is, &data);
  SOC_SAND_CHECK_FUNC_RESULT(rv, 10, exit);

  *is_type = data.is_answer ? TRUE : FALSE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

typedef struct {
  soc_ppc_slb_traverse_type_tree_cb user_cb;
  void *user_opaque;
} _soc_ppc_slb_trv_cb_trv_data;

STATIC uint32 _soc_ppc_slb_trv_cb_trv(SOC_SAND_IN SOC_PPC_SLB_OBJECT *object, SOC_SAND_IN _soc_ppc_slb_object_info *info, int depth, void *opaque)
{
  _soc_ppc_slb_trv_cb_trv_data *data = (_soc_ppc_slb_trv_cb_trv_data *)opaque;
  uint32 rv;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  rv = data->user_cb(object,info->type,data->user_opaque);
  SOC_SAND_CHECK_FUNC_RESULT(rv, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

uint32 soc_ppc_slb_object_traverse_type_tree(SOC_SAND_IN SOC_PPC_SLB_OBJECT *object, SOC_SAND_IN soc_ppc_slb_traverse_type_tree_cb callback, SOC_SAND_IN void *opaque)
{
  uint32 rv;
  _soc_ppc_slb_trv_cb_trv_data data;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  SOC_SAND_CHECK_NULL_INPUT(object);

  data.user_cb = callback;
  data.user_opaque = (void*)opaque;

  if (callback != NULL) {
    rv = _soc_ppc_slb_traverse_inheritance_tree(object, _soc_ppc_slb_trv_cb_trv, &data);
    SOC_SAND_CHECK_FUNC_RESULT(rv, 10, exit);
  }
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

uint32 soc_ppc_slb_object_type(SOC_SAND_IN SOC_PPC_SLB_OBJECT *object, SOC_SAND_OUT SOC_PPC_SLB_OBJECT_TYPE *type)
{

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(object);
  SOC_SAND_CHECK_NULL_INPUT(type);

  *type = object->type;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

uint32 soc_ppc_slb_init(void)
{
  unsigned int object_type_idx;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  
  for (object_type_idx = 0; object_type_idx < SOC_PPC_SLB_NOF_OBJECT_TYPES; object_type_idx++) {
    
    SOC_SAND_VERIFY(_soc_ppc_slb_object_infos[object_type_idx].type == object_type_idx);
  }
 
  SOC_SAND_EXIT_NO_ERROR;
     
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#include <soc/dpp/SAND/Utils/sand_footer.h>

