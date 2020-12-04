/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/




#include <shared/bsl.h>

#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#ifdef _MSC_VER
#pragma warning(disable:4308)
#endif 


#define SOC_SAND_ERROR_CODE_FIND_DUPLICATES 1


CONST STATIC
  SOC_PROCEDURE_DESC_ELEMENT
    Soc_sand_procedure_desc_element[]=
{
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_MODULE_OPEN),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_MODULE_CLOSE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_PHYSICAL_READ_FROM_CHIP),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_PHYSICAL_WRITE_TO_CHIP),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_READ_MODIFY_WRITE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_DEVICE_REGISTER),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_DEVICE_UNREGISTER),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_DEVICE_MNGMNT_LOAD_CHIP_VER),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_HANDLES_INIT_HANDLES),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_HANDLES_SHUT_DOWN_HANDLES),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_HANDLES_REGISTER_HANDLE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_HANDLES_UNREGISTER_HANDLE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_HANDLES_UNREGISTER_ALL_DEVICE_HANDLES),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_CLEAR_CHIP_DESCRIPTOR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_INIT_CHIP_DESCRIPTORS),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_DELETE_CHIP_DESCRIPTORS),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_REMOVE_CHIP_DESCRIPTOR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_ADD_CHIP_DESCRIPTOR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_REGISTER_EVENT_CALLBACK),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_UNREGISTER_EVENT_CALLBACK),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_COMBINE_2_EVENT_CALLBACK_HANDLES),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_GET_CHIP_DESCRIPTOR_MUTEX_OWNER),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_SET_CHIP_DESCRIPTOR_MUTEX_OWNER),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_GET_CHIP_DESCRIPTOR_MUTEX_COUNTER),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_SET_CHIP_DESCRIPTOR_MUTEX_COUNTER),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_INC_CHIP_DESCRIPTOR_MUTEX_COUNTER),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_DEC_CHIP_DESCRIPTOR_MUTEX_COUNTER),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_TAKE_CHIP_DESCRIPTOR_MUTEX),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_GIVE_CHIP_DESCRIPTOR_MUTEX),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_GET_CHIP_DESCRIPTOR_BASE_ADDR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_SET_CHIP_DESCRIPTOR_VER_INFO),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_GET_CHIP_DESCRIPTOR_MEMORY_SIZE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_GET_CHIP_DESCRIPTOR_INTERRUPT_CALLBACK_ARRAY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_GET_CHIP_DESCRIPTOR_UNMASK_FUNC),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_DEC_CHIP_DESCRIPTOR_INT_MASK_COUNTER),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_IS_CHIP_DESCRIPTOR_CHIP_VER_BIGGER_EQ),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_GET_CHIP_DESCRIPTOR_CHIP_VER),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_GET_CHIP_DESCRIPTOR_DBG_VER),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_DELTA_LIST_DECREASE_TIME_FROM_SECOND_ITEM),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_DEVICE_MNGMNT_GET_DEVICE_TYPE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_SSR_GET_VER_FROM_HEADER),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_SSR_GET_SIZE_FROM_HEADER),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_GET_CHIP_DESCRIPTOR_CHIP_TYPE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_IS_CHIP_DESCRIPTOR_VALID),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_GET_CHIP_DESCRIPTOR_MAGIC),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_GENERAL_SET_MAX_NUM_DEVICES),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_MODULE_INIT_ALL),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_MODULE_END_ALL),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_TCM_SEND_MESSAGE_TO_Q_FROM_TASK),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_TCM_SEND_MESSAGE_TO_Q_FROM_INT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_TCM_CALLBACK_ENGINE_START),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_TCM_REGISTER_POLLING_CALLBACK),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_TCM_UNREGISTER_POLLING_CALLBACK),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_USER_CALLBACK_UNREGISTER_DEVICE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_UNPACK_RX_SR_DATA_CELL),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_PACK_TX_SR_DATA_CELL),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_INDIRECT_SET_INFO),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_INDIRECT_CLEAR_INFO),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_INDIRECT_CLEAR_INFO_ALL),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_INDIRECT_CHECK_REQUEST_LEGAL),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_INDIRECT_VERIFY_TRIGGER_0),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_INDIRECT_WRITE_ADDRESS),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_INDIRECT_WRITE_VALUE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_INDIRECT_ASSERT_TRIGGER_1),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_INDIRECT_READ_RESULT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_MEM_INTERRUPT_MASK_ADDRESS_CLEAR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_MEM_READ),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_MEM_WRITE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_TRIGGER_VERIFY_0),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_TRIGGER_ASSERT_1),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_DELTA_LIST_TAKE_MUTEX),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_DELTA_LIST_GIVE_MUTEX),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_DELTA_LIST_DESTROY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_DELTA_LIST_INSERT_D),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_DELTA_LIST_DECREASE_TIME_FROM_HEAD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_DELTA_LIST_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_INIT_ERRORS_QUEUE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_SET_USER_ERROR_HANDLER),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_LOAD_ERRORS_QUEUE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_DELETE_ERRORS_QUEUE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_ERROR_HANDLER),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_UNLOAD_ERRORS_QUEUE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_TCM_CALLBACK_ENGINE_MAIN),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_MEM_READ_CALLBACK),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_INDIRECT_READ_FROM_CHIP),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_INDIRECT_WRITE_TO_CHIP),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_READ_FIELD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_WRITE_FIELD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_GET_CHIP_DESCRIPTOR_DEVICE_AT_INIT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_SET_CHIP_DESCRIPTOR_DEVICE_AT_INIT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_TCM_CALLBACK_DELTA_LIST_TAKE_MUTEX),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_BITSTREAM_SET_ANY_FIELD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_BITSTREAM_GET_ANY_FIELD),

  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_IP_ADDR_NUMERIC_TO_STRING),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_IP_ADDR_STRING_TO_NUMERIC),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_INDIRECT_WRITE_IND_INFO_LOW),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_INDIRECT_READ_IND_INFO_LOW),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_TBL_READ),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_TBL_READ_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_TBL_WRITE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_TBL_WRITE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_OCC_BM_CLEAR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_OCC_BM_CREATE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_OCC_BM_DESTROY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_OCC_BM_GET_NEXT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_OCC_BM_ALLOC_NEXT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_OCC_BM_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_OCC_BM_GET_NEXT_HELPER),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_OCC_BM_OCCUP_STATUS_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_OCC_BM_OCCUP_STATUS_SET_HELPER),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_OCC_BM_INIT),
 
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_HASH_TABLE_CREATE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_HASH_TABLE_DESTROY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_HASH_TABLE_ENTRY_LOOKUP),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_HASH_TABLE_ENTRY_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_HASH_TABLE_ENTRY_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_HASH_TABLE_ENTRY_REMOVE_BY_INDEX),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_HASH_TABLE_GET_NEXT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_HASH_TABLE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_HASH_TABLE_FIND_ENTRY),
 
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_GROUP_MEM_LL_CREATE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_GROUP_MEM_LL_CLEAR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_GROUP_MEM_LL_DESTROY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_GROUP_MEM_LL_MEMBERS_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_GROUP_MEM_LL_MEMBER_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_GROUP_MEM_LL_MEMBER_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_GROUP_MEM_LL_MEMBERS_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_GROUP_MEM_LL_MEMBER_GET_GROUP),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_GROUP_MEM_LL_MEMBER_CLEAR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_GROUP_MEM_LL_MEMBER_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_GROUP_MEM_LL_FUNC_RUN),

  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_ARR_MEM_ALLOCATOR_CREATE),

  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_ARR_MEM_ALLOCATOR_DESTROY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_ARR_MEM_ALLOCATOR_MALLOC),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_ARR_MEM_ALLOCATOR_FREE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_ARR_MEM_ALLOCATOR_READ),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_ARR_MEM_ALLOCATOR_WRITE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_ARR_MEM_ALLOCATOR_READ_BLOCK),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_ARR_MEM_ALLOCATOR_CLEAR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_ARR_MEM_ALLOCATOR_BLOCK_SIZE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_PP_IPV4_SUBNET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_PP_MAC_ADDRESS_STRUCT_TO_LONG),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_PP_MAC_ADDRESS_LONG_TO_STRUCT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_PP_MAC_ADDRESS_STRING_PARSE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_PP_MAC_ADDRESS_INC),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_PP_MAC_ADDRESS_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_PP_MAC_ADDRESS_SUB),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_PP_MAC_ADDRESS_REVERSE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_PP_MAC_ADDRESS_ARE_EQUAL),
 
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_MULTI_SET_CREATE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_MULTI_SET_DESTROY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_MULTI_SET_MEMBER_LOOKUP),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_MULTI_SET_MEMBER_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_MULTI_SET_MEMBER_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_MULTI_SET_MEMBER_REMOVE_BY_INDEX),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_MULTI_SET_GET_NEXT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_MULTI_SET_PRINT),
 
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_SORTED_LIST_CREATE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_SORTED_LIST_DESTROY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_SORTED_LIST_ENTRY_LOOKUP),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_SORTED_LIST_ENTRY_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_SORTED_LIST_ENTRY_UPDATE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_SORTED_LIST_ENTRY_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_SORTED_LIST_GET_NEXT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_SORTED_LIST_GET_PREV),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_SORTED_LIST_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_SORTED_LIST_FIND_MATCH_ENTRY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_SORTED_LIST_ENTRY_ADD_BY_ITER),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_SORTED_LIST_ENTRY_REMOVE_BY_ITER),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_SORTED_LIST_ENTRY_VALUE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_SORTED_LIST_GET_FOLLOW),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_PAT_TREE_CREATE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_PAT_TREE_DESTROY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_PAT_TREE_CLEAR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_PAT_TREE_NODE_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_PAT_TREE_NODE_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_PAT_TREE_LPM_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_PAT_TREE_GET_BLOCK),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_PAT_TREE_GET_SIZE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_PAT_TREE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_PACK_DEST_ROUTED_DATA_CELL),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_UNPACK_DEST_ROUTED_DATA_CELL),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_PACK_SOURCE_ROUTED_DATA_CELL),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_UNPACK_SOURCE_ROUTED_DATA_CELL),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_DATA_CELL_TO_BUFFER),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_SAND_BUFFER_TO_DATA_CELL),
  
  
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
} ;


CONST STATIC
  SOC_ERROR_DESC_ELEMENT
    Soc_sand_error_desc_element[] =
{
  {
    SOC_SAND_NO_ERR,
    "SOC_SAND_NO_ERR",
    "No error. OK indication for all modules.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_GEN_ERR,
    "SOC_SAND_GEN_ERR",
    "General error code (unspecified).",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_MALLOC_FAIL,
    "SOC_SAND_MALLOC_FAIL",
    "Failure trying to do memory allocation.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_MUTEX_CREATE_FAIL,
    "SOC_SAND_MUTEX_CREATE_FAIL",
    "Failure trying to do mutex allocation.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_NULL_POINTER_ERR,
    "SOC_SAND_NULL_POINTER_ERR",
    "NULL pointer was given in a forbidden context.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_NULL_USER_CALLBACK_FUNC,
    "SOC_SAND_NULL_USER_CALLBACK_FUNC",
    "One of the driver-usr callback function that was given\n\r"
    "to the driver was NULL function.\n\r"
    "E.g., 'fap10m_register_device()' reset_device_ptr was set to NULL\n\r"
    " and 'fap10m_reset_device()' was called.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_MAX_NUM_DEVICES_OUT_OF_RANGE_ERR,
    "SOC_SAND_MAX_NUM_DEVICES_OUT_OF_RANGE_ERR",
    "Tried to set or illegal/unknown device identifier.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_DRIVER_NOT_STARTED,
    "SOC_SAND_DRIVER_NOT_STARTED",
    "Driver has not been started. Can not activate "
    "driver services.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_DRIVER_ALREADY_STARTED,
    "SOC_SAND_DRIVER_ALREADY_STARTED",
    "Driver has already been started. Can not activate "
    "driver again.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_DRIVER_BUSY,
    "SOC_SAND_DRIVER_BUSY",
    "Just a message: Driver is currently busy. Try\r\n"
    "again, after timeout, until driver gets free.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_ILLEGAL_DEVICE_ID,
    "SOC_SAND_ILLEGAL_DEVICE_ID",
    "Illegal/unknown device identifier as input parameter",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_ILLEGAL_CHIP_DESCRIPTOR,
    "SOC_SAND_ILLEGAL_CHIP_DESCRIPTOR",
    "Illegal chip descriptor for specified device handle.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_TRYING_TO_ACCESS_DELETED_DEVICE,
    "SOC_SAND_TRYING_TO_ACCESS_DELETED_DEVICE",
    "Illegal chip descriptor for specified device handle\r\n"
    "because it has been deleted.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_SYSTEM_TICK_ERR_01,
    "SOC_SAND_SYSTEM_TICK_ERR_01",
    "currently undefined.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_TCM_TASK_PRIORITY_ERR_01,
    "SOC_SAND_TCM_TASK_PRIORITY_ERR_01",
    "currently undefined.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_MIN_TIME_BETWEEN_POLLS_ERR_01,
    "SOC_SAND_MIN_TIME_BETWEEN_POLLS_ERR_01",
    "currently undefined.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_TCM_MOCKUP_INTERRUPTS_ERR_01,
    "SOC_SAND_TCM_MOCKUP_INTERRUPTS_ERR_01",
    "currently undefined.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_USER_ADD_SAND_ERRORS_ERR_01,
    "SOC_SAND_USER_ADD_SAND_ERRORS_ERR_01",
    "Procedure soc_sand_add_sand_errors() returned with\r\n"
    "error indication in soc_sand_module_open().",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_USER_ADD_SAND_PROCEDURES_ERR_01,
    "SOC_SAND_USER_ADD_SAND_PROCEDURES_ERR_01",
    "Procedure soc_sand_add_sand_procedures() returned with\r\n"
    "error indication in soc_sand_module_open().",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_USER_ERROR_HANDLER_ERR_01,
    "SOC_SAND_USER_ERROR_HANDLER_ERR_01",
    "Procedure soc_sand_set_user_error_handler() returned with\r\n"
    "error indication in soc_sand_module_open().",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_MODULE_END_ALL_ERR_01,
    "SOC_SAND_MODULE_END_ALL_ERR_01",
    "Procedure soc_sand_module_end_all() returned with\r\n"
    "error indication in soc_sand_module_open().",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_MODULE_INIT_ALL_ERR_01,
    "SOC_SAND_MODULE_INIT_ALL_ERR_01",
    "Procedure soc_sand_module_init_all() returned with\r\n"
    "error indication in soc_sand_module_open().",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_INTERRUPT_CLEAR_ALL_ERR_01,
    "SOC_SAND_INTERRUPT_CLEAR_ALL_ERR_01",
    "Procedure soc_sand_mem_interrupt_mask_address_clear_all()\r\n"
    "returned with error indication in soc_sand_module_open().",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_INIT_CHIP_DESCRIPTORS_ERR_01,
    "SOC_SAND_INIT_CHIP_DESCRIPTORS_ERR_01",
    "soc_sand_init_chip_descriptors failed.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_HANDLES_INIT_ERR_01,
    "SOC_SAND_HANDLES_INIT_ERR_01",
    "soc_sand_handles_init_handles failed.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_TCM_CALLBACK_ENGINE_START_ERR_01,
    "SOC_SAND_TCM_CALLBACK_ENGINE_START_ERR_01",
    "soc_sand_tcm_callback_engine_start failed.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_INDIRECT_SET_INFO_ERR_01,
    "SOC_SAND_INDIRECT_SET_INFO_ERR_01",
    "soc_sand_indirect_clear_info_all failed.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_INDIRECT_MODULE_NOT_EXIST,
    "SOC_SAND_INDIRECT_MODULE_NOT_EXIST",
    "Tried to access not exist module in the indirect zone.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_REGISTER_DEVICE_001,
    "SOC_SAND_REGISTER_DEVICE_001",
    "Procedure soc_sand_register_device() reports:\r\n"
    "Maximal number of devices has already been\r\n"
    "registered. Can not register more.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_DEPRECATED_ERROR_00001,
    "SOC_SAND_DEPRECATED_ERROR_00001",
    "Deprecated Error - do not use.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_REGISTER_DEVICE_003,
    "SOC_SAND_REGISTER_DEVICE_003",
    "Procedure soc_sand_register_device() reports:\r\n"
    "No live device has been found trying to access specified memory.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_REGISTER_DEVICE_004,
    "SOC_SAND_REGISTER_DEVICE_004",
    "Procedure soc_sand_register_device() reports:\r\n"
    "Input parameter 'base_address' is not long-aligned.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_REGISTER_DEVICE_005,
    "SOC_SAND_REGISTER_DEVICE_005",
    "Procedure soc_sand_register_device() reports:\r\n"
    "Chip type as reported by chip itself does not match registered chip type.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_REGISTER_DEVICE_NULL_BUFFER,
    "SOC_SAND_REGISTER_DEVICE_NULL_BUFFER",
    "Procedure soc_sand_register_device() reports:\r\n"
    "User supplied buffer is NULL.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_DESC_ARRAY_NOT_INIT,
    "SOC_SAND_DESC_ARRAY_NOT_INIT",
    "Procedure soc_sand_register_device() reports:\r\n"
    "Descriptors array / mutex are not initialized.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_ADD_CHIP_DESCRIPTOR_ERR,
    "SOC_SAND_ADD_CHIP_DESCRIPTOR_ERR",
    "Procedure soc_sand_register_device() reports:\r\n"
    "Unknown error return value from soc_sand_add_chip_descriptor().",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_SEM_CREATE_FAIL,
    "SOC_SAND_SEM_CREATE_FAIL",
    "Failure trying to create semaphore.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_SEM_DELETE_FAIL,
    "SOC_SAND_SEM_DELETE_FAIL",
    "Failure trying to delete semaphore.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_SEM_TAKE_FAIL,
    "SOC_SAND_SEM_TAKE_FAIL",
    "Failure trying to take semaphore.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_CHIP_VER_IS_NOT_A,
    "SOC_SAND_CHIP_VER_IS_NOT_A",
    "Failure trying to call RevA only function while running over RevB+ chip.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_CHIP_VER_IS_A,
    "SOC_SAND_CHIP_VER_IS_A",
    "Failure trying to call RevB+ function while running over RevA chip.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_TCM_CALLBACK_DELTA_LIST_SEM_TAKE_FAIL,
    "SOC_SAND_TCM_CALLBACK_DELTA_LIST_SEM_TAKE_FAIL",
    "ERROR at soc_sand_tcm_callback_delta_list_take_mutex:\n\r"
    "current task id cannot take the tcm callback delta\n\r"
    "list semaphore while already having the chip descriptors semaphore.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_CHIP_DESCRIPTORS_SEM_TAKE_ERR_0,
    "SOC_SAND_CHIP_DESCRIPTORS_SEM_TAKE_ERR_0",
    "ERROR at soc_sand_take_chip_descriptor_mutex:\n\r"
    "current task id cannot take the chip descriptor mutex\n\r"
    "list semaphore while already having the handles delta list semaphore.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_SEM_GIVE_FAIL,
    "SOC_SAND_SEM_GIVE_FAIL",
    "Failure trying to give back semaphore.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_SEM_CHANGED_ON_THE_FLY,
    "SOC_SAND_SEM_CHANGED_ON_THE_FLY",
    "Semaphore take has succeeded but it is not the one\r\n"
    "which was intended, initially (It was deleted and\r\n"
    "replaced).",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_IND_TRIGGER_TIMEOUT,
    "SOC_SAND_IND_TRIGGER_TIMEOUT",
    "This error relates to indirect access only.\r\n"
    "Timeout waiting for chip to deassert the 'trigger'\r\n"
    "bit.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_UNREGISTER_DEVICE_001,
    "SOC_SAND_UNREGISTER_DEVICE_001",
    "Procedure soc_sand_user_callback_unregister_device() or\r\n"
    "soc_sand_clear_all_device_peding_services() reports:\r\n"
    "Failed to delete some of the device's active handles",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_UNREGISTER_DEVICE_002,
    "SOC_SAND_UNREGISTER_DEVICE_002",
    "Procedure soc_sand_user_callback_unregister_device() reports:\r\n"
    "Failed to clear device descriptor",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_MEM_READ_001,
    "SOC_SAND_MEM_READ_001",
    "Procedure soc_sand_mem_read() reports:\r\n"
    "Input parameter 'offset' is not long-aligned.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_MEM_READ_002,
    "SOC_SAND_MEM_READ_002",
    "Procedure soc_sand_mem_read() reports:\r\n"
    "Input parameter 'offset' is not long-aligned.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_MEM_READ_US_001,
    "SOC_SAND_MEM_READ_US_001",
    "Procedure soc_sand_mem_read_unsafe() reports:\r\n"
    "Input parameter 'offset' is not long-aligned.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_MEM_READ_US_002,
    "SOC_SAND_MEM_READ_US_002",
    "Procedure soc_sand_mem_read_unsafe() reports:\r\n"
    "Input parameter 'size' is not a multiple of"
    "four.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_MEM_READ_US_003,
    "SOC_SAND_MEM_READ_US_003",
    "Procedure soc_sand_mem_read_unsafe() reports:\r\n"
    "This error relates to direct access only.\r\n"
    "Specified memory range ('offset', 'size') contains\r\n"
    "some addresses which are not readable by this chip\r\n"
    "(probably interrupt sources, which are aoto cleread).",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_MEM_READ_003,
    "SOC_SAND_MEM_READ_003",
    "Procedure soc_sand_mem_read() reports:\r\n"
    "This error relates to direct access only.\r\n"
    "Specified memory range ('offset', 'size') contains\r\n"
    "some addresses which are not readable by this chip\r\n"
    "(probably interrupt sources, which are aoto cleread).",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_MEM_READ_004,
    "SOC_SAND_MEM_READ_004",
    "Procedure soc_sand_mem_read() reports:\r\n"
    "This error relates to indirect access only.\r\n"
    "Specified module (within 'offset') is illegal.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_MEM_READ_005,
    "SOC_SAND_MEM_READ_005",
    "Procedure soc_sand_mem_read() reports:\r\n"
    "This error relates to indirect access only.\r\n"
    "Specified memory range ('offset', 'size')\r\n"
    "contains some addresses which are not readable\r\n"
    "by this chip.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_MEM_READ_US_005,
    "SOC_SAND_MEM_READ_US_005",
    "Procedure soc_sand_mem_read_unsafe() reports:\r\n"
    "This error relates to indirect access only.\r\n"
    "soc_sand_indirect_read_from_chip failed.\r\n",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_MEM_READ_006,
    "SOC_SAND_MEM_READ_006",
    "Procedure soc_sand_mem_read() reports:\r\n"
    "Failed to read from the chip",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_MEM_READ_US_006,
    "SOC_SAND_MEM_READ_US_006",
    "Procedure soc_sand_mem_read_unsafe() reports:\r\n"
    "Failed to read from the chip",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_MEM_READ_007,
    "SOC_SAND_MEM_READ_007",
    "Procedure soc_sand_mem_read() reports:\r\n"
    "Failed to write to chip (relevant only to indirect)",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_MEM_READ_010,
    "SOC_SAND_MEM_READ_010",
    "Procedure soc_sand_mem_read() reports:\r\n"
    "Function - soc_sand_mem_read_unsafe() failed",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_MEM_WRITE_001,
    "SOC_SAND_MEM_WRITE_001",
    "Procedure soc_sand_mem_write() reports:\r\n"
    "Input parameter 'offset' is not long-aligned.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_MEM_WRITE_US_001,
    "SOC_SAND_MEM_WRITE_US_001",
    "Procedure soc_sand_mem_write_unsafe() reports:\r\n"
    "Input parameter 'offset' is not long-aligned.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_MEM_WRITE_002,
    "SOC_SAND_MEM_WRITE_002",
    "Procedure soc_sand_mem_write() reports:\r\n"
    "Input parameter 'size' is not a multiple of"
    "four.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_MEM_WRITE_003,
    "SOC_SAND_MEM_WRITE_003",
    "Procedure soc_sand_mem_write() reports:\r\n"
    "This error relates to direct access only.\r\n"
    "Specified memory range ('offset', 'size') contains\r\n"
    "some addresses which are not writable by this chip\r\n"
    "(probably interrupt sources or interrupt mask).",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_MEM_WRITE_US_003,
    "SOC_SAND_MEM_WRITE_US_003",
    "Procedure soc_sand_mem_write_unsafe() reports:\r\n"
    "This error relates to direct access only.\r\n"
    "Specified memory range ('offset', 'size') contains\r\n"
    "some addresses which are not writable by this chip\r\n"
    "(probably interrupt sources or interrupt mask).",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_MEM_WRITE_004,
    "SOC_SAND_MEM_WRITE_004",
    "Procedure soc_sand_mem_write() reports:\r\n"
    "This error relates to indirect access only.\r\n"
    "Specified module (within 'offset') is illegal.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_MEM_WRITE_005,
    "SOC_SAND_MEM_WRITE_005",
    "Procedure soc_sand_mem_write() reports:\r\n"
    "This error relates to indirect access only.\r\n"
    "Specified memory range ('offset', 'size')\r\n"
    "contains some addresses which are not writable\r\n"
    "by this chip.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_MEM_WRITE_US_005,
    "SOC_SAND_MEM_WRITE_US_005",
    "Procedure soc_sand_mem_write_unsafe() reports:\r\n"
    "This error relates to indirect access only.\r\n"
    "soc_sand_indirect_write_from_chip failed.\r\n",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_MEM_WRITE_006,
    "SOC_SAND_MEM_WRITE_006",
    "Procedure soc_sand_mem_write() reports:\r\n"
    "Failed to write to the chip",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_MEM_WRITE_US_006,
    "SOC_SAND_MEM_WRITE_US_006",
    "Procedure soc_sand_mem_write() reports:\r\n"
    "Failed to write to the chip",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_MEM_WRITE_010,
    "SOC_SAND_MEM_WRITE_010",
    "Procedure soc_sand_mem_write() reports:\r\n"
    "Function - soc_sand_mem_write_unsafe() failed",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_FAIL_REQUEST_LEGAL,
    "SOC_SAND_FAIL_REQUEST_LEGAL",
    "Procedure soc_sand_indirect_read_low() reports:\r\n"
    "Function - soc_sand_indirect_check_request_legal() failed",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_FAIL_VERIFY_0,
    "SOC_SAND_FAIL_VERIFY_0",
    "Procedure soc_sand_indirect_read_low() reports:\r\n"
    "Function - soc_sand_trigger_verify_0_inc_info() failed",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_FAIL_WRITE_ADDRESS,
    "SOC_SAND_FAIL_WRITE_ADDRESS",
    "Procedure soc_sand_indirect_read_low() reports:\r\n"
    "Function - soc_sand_indirect_write_address_inc_info() failed",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_FAIL_WRITE_VALUE,
    "SOC_SAND_FAIL_WRITE_VALUE",
    "Procedure soc_sand_indirect_read_low() reports:\r\n"
    "Function - soc_sand_indirect_write_value_inc_info() failed",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_FAIL_ASSERT_1,
    "SOC_SAND_FAIL_ASSERT_1",
    "Procedure soc_sand_indirect_read_low() reports:\r\n"
    "Function - soc_sand_trigger_assert_1_inc_info() failed",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_FAIL_READ_RESULT,
    "SOC_SAND_FAIL_READ_RESULT",
    "Procedure soc_sand_indirect_read_low() reports:\r\n"
    "Function - soc_sand_indirect_read_result_inc_info() failed",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_TRG_TIMEOUT,
    "SOC_SAND_TRG_TIMEOUT",
    "Timeout on waiting for 'trigger'to be deasserted.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_CALLBACK_FUNCTION_CANT_BE_INT,
    "SOC_SAND_CALLBACK_FUNCTION_CANT_BE_INT",
    "Called to callback function that do\r\n"
    "not use interrupt with interrupt request.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_NOT_CALLBACK_FUNCTION,
    "SOC_SAND_NOT_CALLBACK_FUNCTION",
    "Asked function can not be registered as callback.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_TCM_MAIN_ERR_01,
    "SOC_SAND_TCM_MAIN_ERR_01",
    "Unrecoverable error encountered in TCM.",
    SOC_SAND_SVR_FTL,
    FALSE
  },
  {
    SOC_SAND_TCM_MAIN_ERR_02,
    "SOC_SAND_TCM_MAIN_ERR_02",
    "Removed a deferred action due to an error.",
    SOC_SAND_SVR_FTL,
    FALSE
  },
  {
    SOC_SAND_TCM_MAIN_ERR_03,
    "SOC_SAND_TCM_MAIN_ERR_03",
    "Removed a deferred action due to user registration request.",
    SOC_SAND_SVR_MSG,
    FALSE
  },
  {
    SOC_SAND_NO_SUCH_INDIRECT_MODULE_ERR,
    "SOC_SAND_NO_SUCH_INDIRECT_MODULE_ERR",
    "Procedure soc_sand_indirect_check_request_legal() reports:\n\r"
    "Trying to access non-existing indirect module.",
    SOC_SAND_SVR_MSG,
    FALSE
  },
  {
    SOC_SAND_INDIRECT_OFFSET_SIZE_MISMATCH_ERR,
    "SOC_SAND_INDIRECT_OFFSET_SIZE_MISMATCH_ERR",
    "Procedure soc_sand_indirect_check_request_legal() reports:\n\r"
    "Trying to access non-existing offset/size in the indirect module.",
    SOC_SAND_SVR_MSG,
    FALSE
  },
  {
    SOC_SAND_INDIRECT_SIZE_MISMATCH_ERR,
    "SOC_SAND_INDIRECT_SIZE_MISMATCH_ERR",
    "Procedure soc_sand_indirect_check_request_legal() reports:\n\r"
    "Example to a possible error:\n\r"
    "Say specific module answer size is 4 32 bit register (128 bit, 16 byte, 4 longs)\n\r"
    "(PEC in SOC_SAND_FAP10M). User gave 5 longs as buffer size.\n\r"
    "((5/4) * 4) ==> 4 longs.\n\r"
    "4!=5 ... the result is different than we need. We find a mismatch is sizes.",
    SOC_SAND_SVR_MSG,
    FALSE
  },
  {
    SOC_SAND_INDIRECT_CANT_GET_INFO_ERR,
    "SOC_SAND_INDIRECT_CANT_GET_INFO_ERR",
    "Internal error!!\n\r"
    "The indirect information missing offset information.",
    SOC_SAND_SVR_MSG,
    FALSE
  },
  {
    SOC_SAND_INDIRECT_TABLES_INFO_ORDER_ERR,
    "SOC_SAND_INDIRECT_TABLES_INFO_ORDER_ERR",
    "If a tables_prefix A is a subset of tables_prefix B,\n\r"
    "The structure that contain tables_prefix A, must\n\r"
    "come before the structure that contains tables_prefix B.",
    SOC_SAND_SVR_MSG,
    FALSE
  },
  {
    SOC_SAND_FREE_FAIL,
    "SOC_SAND_FREE_FAIL",
    "Failure trying to free memory,\n\r",
    SOC_SAND_SVR_MSG,
    FALSE
  },
  {
    SOC_SAND_CREDIT_SIZE_ENUM_ERR,
    "SOC_SAND_CREDIT_SIZE_ENUM_ERR",
    "User gave not valid value in SOC_SAND_CREDIT_SIZE.\n\r"
    "Please refer to SOC_SAND_CREDIT_SIZE definition.",
    SOC_SAND_SVR_MSG,
    FALSE
  },
  {
    SOC_SAND_BIT_STREAM_FIELD_SET_SIZE_RANGE_ERR,
    "SOC_SAND_BIT_STREAM_FIELD_SET_SIZE_RANGE_ERR",
    "Functions 'soc_sand_bitstream_get_field()'/'soc_sand_bitstream_set_field()' reports:\n\r"
    "'nof_bits' is bigger than 32 (SOC_SAND_BIT_STREAM_FIELD_SET_SIZE).\n\r"
    "Note, these functions get/set at most 32 bit fields.",
    SOC_SAND_SVR_MSG,
    FALSE
  },
  {
    SOC_SAND_DO_NOT_SUPPORTS_INTERRUPTS_ERR,
    "SOC_SAND_DO_NOT_SUPPORTS_INTERRUPTS_ERR",
    "Function 'XXXX_register_device()' reports:\n\r"
    "User try to register device in SOC_SAND. SOC_SAND was configured to\n\r"
    "user real interrupts (via 'soc_sand_module_open()'). This driver\n\r"
    "do not supports interrupts. Only mock-up interrupts\n\r"
    "('soc_tcmmockup_interrupts' indicator in 'soc_sand_module_open()).",
    SOC_SAND_SVR_MSG,
    FALSE
  },
  {
    SOC_SAND_DIV_BY_ZERO_ERR,
    "SOC_SAND_DIV_BY_ZERO_ERR",
    "Division by zero. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_OVERFLOW_ERR,
    "SOC_SAND_OVERFLOW_ERR",
    "Calculation overflow. \n\r "
    ".\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_ERRORS_FOUND_DUPLICATES_ERR,
    "SOC_SAND_ERRORS_FOUND_DUPLICATES_ERR",
    "Different errors use the same error id. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_PROCS_FOUND_DUPLICATES_ERR,
    "SOC_SAND_PROCS_FOUND_DUPLICATES_ERR",
    "Different procedures use the same proc_id. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },  
  {
    SOC_SAND_ERR_8001,
    "SOC_SAND_ERR_8001",
    "Currently undefined.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_ERR_8002,
    "SOC_SAND_ERR_8002",
    "Currently undefined.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_ERR_8003,
    "SOC_SAND_ERR_8003",
    "Currently undefined.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_ERR_8004,
    "SOC_SAND_ERR_8004",
    "Currently undefined.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_ERR_8005,
    "SOC_SAND_ERR_8005",
    "Currently undefined.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_ERR_8006,
    "SOC_SAND_ERR_8006",
    "Currently undefined.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_ERR_8007,
    "SOC_SAND_ERR_8007",
    "Currently undefined.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_ERR_8008,
    "SOC_SAND_ERR_8008",
    "Currently undefined.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_ERR_8009,
    "SOC_SAND_ERR_8009",
    "Currently undefined.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_ERR_8010,
    "SOC_SAND_ERR_8010",
    "Currently undefined.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_ERR_8011,
    "SOC_SAND_ERR_8011",
    "Currently undefined.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_ERR_8012,
    "SOC_SAND_ERR_8012",
    "Currently undefined.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_ERR_8013,
    "SOC_SAND_ERR_8013",
    "Currently undefined.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_ERR_8014,
    "SOC_SAND_ERR_8014",
    "Currently undefined.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_ERR_8015,
    "SOC_SAND_ERR_8015",
    "Currently undefined.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_ERR_8016,
    "SOC_SAND_ERR_8016",
    "Currently undefined.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_CODE_HAMMING_P_BIT_WIDE_UN_MATCH_ERR,
    "SOC_SAND_CODE_HAMMING_P_BIT_WIDE_UN_MATCH_ERR",
    "Hamming-Code SW module received not correlated data-bit-wide,\n\r"
    "and p-bit-wide. The correlated number can be found from\n\r"
    "'soc_sand_code_hamming_get_p_bit_wide()' func call.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_CODE_HAMMING_UN_SUPPORTED_DATA_BIT_WIDE_ERR,
    "SOC_SAND_CODE_HAMMING_UN_SUPPORTED_DATA_BIT_WIDE_ERR",
    "Hamming-Code SW module received un-supported data-bit-wide.\n\r"
    "The maximum-p-bit-size we support is 32. Hence,\n\r"
    "this problem is practically will no happen.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_MAGIC_NUM_ERR,
    "SOC_SAND_MAGIC_NUM_ERR",
    "When a device driver supports the magic number capability,\n\r"
    " the user has to call a _clear function, before using\n\r"
    " a structure to set information to the device driver.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_ILLEGAL_IP_FORMAT,
    "SOC_SAND_ILLEGAL_IP_FORMAT",
    " Illegal IP decimal format. Must be 16 bytes long at least,\n\r"
    " 4 decimal numbers separated by dots.",
    SOC_SAND_SVR_ERR,
    FALSE
  },

 
  {
    SOC_SAND_HASH_TABLE_SIZE_OUT_OF_RANGE_ERR,
    "SOC_SAND_HASH_TABLE_SIZE_OUT_OF_RANGE_ERR",
    "the given size of the hash table to allocate is out of range.\n\r"
    "Range is 1-256K, preferred power of 2.",
    SOC_SAND_SVR_MSG,
    FALSE
  },
  {
    SOC_SAND_HASH_TABLE_IS_FULL_ERR,
    "SOC_SAND_HASH_TABLE_IS_FULL_ERR",
    "the hash table is full and trying to insert to the hash table.\n\r"
    "the size of the hash table as allocated in the create..\n\r"
    "and this size may not change.",
    SOC_SAND_SVR_MSG,
    FALSE
  },
  {
    SOC_SAND_HASH_TABLE_KEY_ALREADY_EXIST_ERR,
    "SOC_SAND_HASH_TABLE_KEY_ALREADY_EXIST_ERR",
    "trying to add to the hash table key that already present in\n\r"
    "the hash table. key can be present at most once in the hash table.",
    SOC_SAND_SVR_MSG,
    FALSE
  },
  {
    SOC_SAND_HASH_TABLE_KEY_IS_NOT_EXIST_ERR,
    "SOC_SAND_HASH_TABLE_KEY_IS_NOT_EXIST_ERR",
    "trying to update a key that is not exist in the hash table \n\r"
    "should use Add.",
    SOC_SAND_SVR_MSG,
    FALSE
  },
 
  {
    SOC_SAND_GROUP_MEM_LL_NOF_MEMBERS_OUT_OF_RANGE_ERR,
    "SOC_SAND_GROUP_MEM_LL_NOF_MEMBERS_OUT_OF_RANGE_ERR",
    "the number of elements (members) is out of range.\n\r"
    "Range is 1-",
    SOC_SAND_SVR_MSG,
    FALSE
  },
  {
    SOC_SAND_GROUP_MEM_LL_NOF_GROUPS_OUT_OF_RANGE_ERR,
    "SOC_SAND_GROUP_MEM_LL_NOF_GROUPS_OUT_OF_RANGE_ERR",
    "the number of groups is out of range.\n\r"
    "Range is 1-",
    SOC_SAND_SVR_MSG,
    FALSE
  },
  {
    SOC_SAND_GROUP_MEM_LL_MEMBER_ID_OUT_OF_RANGE_ERR,
    "SOC_SAND_GROUP_MEM_LL_MEMBER_ID_OUT_OF_RANGE_ERR",
    "the member (element) id is out of range.\n\r"
    "Range according to nof_elements given in create.",
    SOC_SAND_SVR_MSG,
    FALSE
  },
  {
    SOC_SAND_GROUP_MEM_LL_GROUP_ID_OUT_OF_RANGE_ERR,
    "SOC_SAND_GROUP_MEM_LL_GROUP_ID_OUT_OF_RANGE_ERR",
    "the group id is out of range.\n\r"
    "Range according to nof_groups given in create.",
    SOC_SAND_SVR_MSG,
    FALSE
  },
  {
    SOC_SAND_GROUP_MEM_LL_ILLEGAL_ADD_ERR,
    "SOC_SAND_GROUP_MEM_LL_ILLEGAL_ADD_ERR",
    "try to add element as member to group when it's already.\n\r"
    "a member in a group and auto_remove is FALSE.",
    SOC_SAND_SVR_MSG,
    FALSE
  },
  {
    SOC_SAND_ARR_MEM_ALLOCATOR_NOF_LINES_OUT_OF_RANGE_ERR,
    "SOC_SAND_ARR_MEM_ALLOCATOR_NOF_LINES_OUT_OF_RANGE_ERR",
    "the number of lines (memory) is out of range.\n\r"
    "Range is 1-max_sand_u32",
    SOC_SAND_SVR_MSG,
    FALSE
  },
  {
    SOC_SAND_ARR_MEM_ALLOCATOR_MALLOC_SIZE_OUT_OF_RANGE_ERR,
    "SOC_SAND_ARR_MEM_ALLOCATOR_MALLOC_SIZE_OUT_OF_RANGE_ERR",
    "malloc size is out of range.\n\r"
    "malloc can be in size of 2-size of create memory.",
    SOC_SAND_SVR_MSG,
    FALSE
  },
  {
    SOC_SAND_ARR_MEM_ALLOCATOR_POINTER_OF_RANGE_ERR,
    "SOC_SAND_ARR_MEM_ALLOCATOR_POINTER_OF_RANGE_ERR",
    "Free pointer is out of range..\n\r"
    "free pointer range is 0-size of create memory.",
    SOC_SAND_SVR_MSG,
    FALSE
  },
  {
    SOC_SAND_PP_IPV4_SUBNET_PREF_OUT_OF_RANGE_ERR,
    "SOC_SAND_PP_IPV4_SUBNET_PREF_OUT_OF_RANGE_ERR",
    "the ipv4 prefix len is out of range \n\r"
    "ipv4 prefix length range 0-32.",
    SOC_SAND_SVR_MSG,
    FALSE
  },
  {
    SOC_SAND_PP_IPV6_ADDRESS_IS_NOT_MCAST_ERR,
    "SOC_SAND_PP_IPV6_ADDRESS_IS_NOT_MCAST_ERR",
    "the ipv6 address is not ipv6 multicast address\n\r"
    "prefix is not 0xff.",
    SOC_SAND_SVR_MSG,
    FALSE
  },
  {
    SOC_SAND_PP_MAC_ADDRESS_ILLEGAL_STRING_ERR,
    "SOC_SAND_PP_MAC_ADDRESS_ILLEGAL_STRING_ERR",
    "the mac address string is not valid \n\r"
    "one of the string chars is out of hex values."
    "range of each char is 0-f in hexadecimal.",
    SOC_SAND_SVR_MSG,
    FALSE
  },
    {
    SOC_SAND_PP_IPV4_ADDRESS_OUT_OF_RANGE_ERR,
    "SOC_SAND_PP_IPV4_ADDRESS_OUT_OF_RANGE_ERR",
    "The parameter of type 'SOC_SAND_PP_IPV4_ADDRESS' is out of range. \n\r "
    "The range is: 0 - No_max.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR,
    "SOC_SAND_PP_VLAN_ID_OUT_OF_RANGE_ERR",
    "The parameter of type 'SOC_SAND_PP_VLAN_ID' is out of range. \n\r "
    "The range is: 0 - 4*1024-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_PP_TC_OUT_OF_RANGE_ERR,
    "SOC_SAND_PP_TC_OUT_OF_RANGE_ERR",
    "The parameter of type 'SOC_SAND_PP_TC' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_PP_DP_OUT_OF_RANGE_ERR,
    "SOC_SAND_PP_DP_OUT_OF_RANGE_ERR",
    "The parameter of type 'SOC_SAND_PP_DP' is out of range. \n\r "
    "The range is: 0 - 3.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_PP_DEI_CFI_OUT_OF_RANGE_ERR,
    "SOC_SAND_PP_DEI_CFI_OUT_OF_RANGE_ERR",
    "The parameter of type 'SOC_SAND_PP_DEI_CFI' is out of range. \n\r "
    "The range is: 0 - 1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_PP_PCP_UP_OUT_OF_RANGE_ERR,
    "SOC_SAND_PP_PCP_UP_OUT_OF_RANGE_ERR",
    "The parameter of type 'SOC_SAND_PP_PCP_UP' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_PP_ETHER_TYPE_OUT_OF_RANGE_ERR,
    "SOC_SAND_PP_ETHER_TYPE_OUT_OF_RANGE_ERR",
    "The parameter of type 'SOC_SAND_PP_ETHER_TYPE' is out of range. \n\r "
    "The range is: 0 - 0xffff.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_PP_TPID_OUT_OF_RANGE_ERR,
    "SOC_SAND_PP_TPID_OUT_OF_RANGE_ERR",
    "The parameter of type 'SOC_SAND_PP_TPID' is out of range. \n\r "
    "The range is: 0 - 0xffff.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_PP_IP_TTL_OUT_OF_RANGE_ERR,
    "SOC_SAND_PP_IP_TTL_OUT_OF_RANGE_ERR",
    "The parameter of type 'SOC_SAND_PP_IP_TTL' is out of range. \n\r "
    "The range is: 0 - 255.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_PP_IPV4_TOS_OUT_OF_RANGE_ERR,
    "SOC_SAND_PP_IPV4_TOS_OUT_OF_RANGE_ERR",
    "The parameter of type 'SOC_SAND_PP_IPV4_TOS' is out of range. \n\r "
    "The range is: 0 - 255.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_PP_IPV6_TC_OUT_OF_RANGE_ERR,
    "SOC_SAND_PP_IPV6_TC_OUT_OF_RANGE_ERR",
    "The parameter of type 'SOC_SAND_PP_IPV6_TC' is out of range. \n\r "
    "The range is: 0 - 255.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_PP_MPLS_LABEL_OUT_OF_RANGE_ERR,
    "SOC_SAND_PP_MPLS_LABEL_OUT_OF_RANGE_ERR",
    "The parameter of type 'SOC_SAND_PP_MPLS_LABEL' is out of range. \n\r "
    "The range is: 0 - (1<<20)-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_PP_MPLS_EXP_OUT_OF_RANGE_ERR,
    "SOC_SAND_PP_MPLS_EXP_OUT_OF_RANGE_ERR",
    "The parameter of type 'SOC_SAND_PP_MPLS_EXP' is out of range. \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_PP_DSCP_EXP_OUT_OF_RANGE_ERR,
    "SOC_SAND_PP_MPLS_EXP_OUT_OF_RANGE_ERR",
    "The parameter DSCP_EXP_NDX is out of range. \n\r "
    "The range is: 0 - 255.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_PP_MPLS_DSCP_EXP_OUT_OF_RANGE_ERR,
    "SOC_SAND_PP_DSCP_EXP_OUT_OF_RANGE_ERR",
    "The parameter DSCP_EXP_NDX is out of range. \n\r "
    "For packet type MPLS \n\r "
    "The range is: 0 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_PP_ISID_OUT_OF_RANGE_ERR,
    "SOC_SAND_PP_ISID_OUT_OF_RANGE_ERR",
    "The parameter of type 'SOC_SAND_PP_ISID' is out of range. \n\r "
    "The range is: 0 - No_max.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_PP_IN_RANGE_OUT_OF_RANGE_ERR,
    "SOC_SAND_PP_IN_RANGE_OUT_OF_RANGE_ERR",
    "The parameter IN_RANGE is out of range. \n\r "
    "The range is: 0 - 1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_SORTED_LIST_KEY_DATA_ALREADY_EXIST_ERR,
    "SOC_SAND_SORTED_LIST_KEY_DATA_ALREADY_EXIST_ERR",
    "adding to the sorted list entry with key \n\r"
    "and data that already exist in the list",
    SOC_SAND_SVR_MSG,
    FALSE
  },
  {
    SOC_SAND_SORTED_LIST_ILLEGAL_ITER_ERR,
    "SOC_SAND_SORTED_LIST_ILLEGAL_ITER_ERR",
    "trying to add/remove entry using illegal iterator position",
    SOC_SAND_SVR_MSG,
    FALSE
  },
  {
    SOC_SAND_GET_ERR_TXT_ERR,
    "SOC_SAND_GET_ERR_TXT_ERR",
    "Cannot get error text for error ID. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_VALUE_OUT_OF_RANGE_ERR,
    "SOC_SAND_VALUE_OUT_OF_RANGE_ERR",
    " Value outside the allowed range.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_VALUE_BELOW_MIN_ERR,
    "SOC_SAND_VALUE_BELOW_MIN_ERR",
    " Value below the minimal allowed value.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_VALUE_ABOVE_MAX_ERR,
    "SOC_SAND_VALUE_ABOVE_MAX_ERR",
    " Value above the maximal allowed value.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_SAND_SOC_ERR,
    "SOC_SAND_SOC_ERR",
    "A soc error occurred.",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    (uint32)SOC_SAND_END_ERR_LIST,
    "",
    "",
    SOC_SAND_SVR_FTL,
    FALSE
  }
} ;

static
  uint8
    Soc_no_error_printing = FALSE;


void
  soc_sand_initialize_error_word(
    uint32   proc_id,
    unsigned char   callback,
    uint32   *error_word
    )
{
  soc_sand_set_proc_id_into_error_word   (proc_id, error_word) ;
  soc_sand_set_error_code_into_error_word(SOC_SAND_OK,  error_word) ;
  soc_sand_set_severity_into_error_word  (0, error_word) ;
  soc_sand_set_callback_into_error_word  (callback, error_word) ;
}

void
  soc_sand_set_proc_id_into_error_word(
    uint32   proc_id,
    uint32   *error_word
  )
{
  uint32 local_proc_id = proc_id ;
  if(proc_id > SOC_PROC_ID_MAX_VAL)
  {
    goto exit ;
  }
  local_proc_id = SOC_SAND_SET_FLD_IN_PLACE(local_proc_id, SOC_PROC_ID_SHIFT, SOC_PROC_ID_MASK) ;
  *error_word &= ~SOC_PROC_ID_MASK ;
  *error_word |= local_proc_id ;
exit:
  return ;
}

uint32
  soc_sand_get_proc_id_from_error_word(
    uint32   error_word
  )
{
  uint32 res ;
  res = (uint32)SOC_SAND_GET_FLD_FROM_PLACE(error_word, SOC_PROC_ID_SHIFT, SOC_PROC_ID_MASK) ;
  return res ;
}

void
  soc_sand_set_error_code_into_error_word(
    unsigned short  error_code,
    uint32   *error_word
  )
{
  uint32
      local_error_code = error_code ;
  
  error_code &= SOC_ERROR_CODE_MAX_MASK ;


  local_error_code = SOC_SAND_SET_FLD_IN_PLACE(local_error_code, SOC_ERROR_CODE_SHIFT, SOC_ERROR_CODE_MASK) ;
  *error_word &= ~SOC_ERROR_CODE_MASK ;
  *error_word |= local_error_code ;
  goto exit;
exit:
  return ;
}


uint32
  soc_sand_update_error_code(
    uint32   fun_error_word,
    uint32   *error_word
  )
{
  unsigned short error_code ;
  uint32
      local_error_code;
  
  error_code = (unsigned short)SOC_SAND_GET_FLD_FROM_PLACE(fun_error_word, SOC_ERROR_CODE_SHIFT, SOC_ERROR_CODE_MASK) ;
  local_error_code = error_code;

  error_code &= SOC_ERROR_CODE_MAX_MASK ;
  local_error_code = SOC_SAND_SET_FLD_IN_PLACE(local_error_code, SOC_ERROR_CODE_SHIFT, SOC_ERROR_CODE_MASK) ;
  *error_word &= ~SOC_ERROR_CODE_MASK ;
  *error_word |= local_error_code ;
  goto exit;
exit:
  return *error_word;
}

unsigned short
  soc_sand_get_error_code_from_error_word(
    uint32   error_word
  )
{
  unsigned short res ;
  res = (unsigned short)SOC_SAND_GET_FLD_FROM_PLACE(error_word, SOC_ERROR_CODE_SHIFT, SOC_ERROR_CODE_MASK) ;
  return res ;
}

void
  soc_sand_set_severity_into_error_word(
    unsigned char severity,
    uint32 *error_word
  )
{
  uint32 local_severity = severity ;
  if(severity > SOC_ERR_SEVERE_MAX_VAL)
  {
    goto exit ;
  }
  local_severity = SOC_SAND_SET_FLD_IN_PLACE(local_severity, SOC_ERR_SEVERE_SHIFT, SOC_ERR_SEVERE_MASK) ;
  *error_word &= ~SOC_ERR_SEVERE_MASK ;
  *error_word |= local_severity ;
exit:
  return ;
}

unsigned char
  soc_sand_get_severity_from_error_word(
    uint32 error_word
  )
{
  unsigned char res ;
  res = (unsigned char)SOC_SAND_GET_FLD_FROM_PLACE(error_word, SOC_ERR_SEVERE_SHIFT, SOC_ERR_SEVERE_MASK) ;
  return res ;
}

void
  soc_sand_set_callback_into_error_word(
    unsigned char callback,
    uint32 *error_word
  )
{
  uint32 local_callback = callback ;
  if(callback > SOC_CALLBACK_PROC_MAX_VAL)
  {
    goto exit ;
  }
  local_callback = SOC_SAND_SET_FLD_IN_PLACE(local_callback, SOC_CALLBACK_PROC_SHIFT, SOC_CALLBACK_PROC_MASK) ;
  *error_word &= ~SOC_CALLBACK_PROC_MASK ;
  *error_word |= local_callback ;
exit:
  return ;
}

unsigned char
  soc_sand_get_callback_from_error_word(
    uint32 error_word
  )
{
  unsigned char res ;
  res = (unsigned char)SOC_SAND_GET_FLD_FROM_PLACE(error_word, SOC_CALLBACK_PROC_SHIFT, SOC_CALLBACK_PROC_MASK) ;
  return res ;
}
uint32
  soc_sand_build_error_code(
    unsigned short error_code,
    uint32  proc_id,
    unsigned char  severity,
    unsigned char  is_call_back
  )
{
  uint32
    ex = SOC_SAND_OK ;
  soc_sand_set_error_code_into_error_word(error_code,   &ex) ;
  soc_sand_set_proc_id_into_error_word   (proc_id,      &ex) ;
  soc_sand_set_severity_into_error_word  (severity,     &ex) ;
  soc_sand_set_callback_into_error_word  (is_call_back, &ex) ;
  return ex ;
}



CONST SOC_ERROR_DESC_ELEMENT
  *soc_sand_get_errors_ptr(
    void
  )
{
  CONST SOC_ERROR_DESC_ELEMENT
    *ret ;
  ret = &Soc_sand_error_desc_element[0] ;
  return (ret) ;
}

int
  soc_compare_error_desc_elements(
    void *error_desc_element_1,
    void *error_desc_element_2
  )
{
  int
    ret ;
  ret =
      (int)(((const SOC_ERROR_DESC_ELEMENT *)error_desc_element_1)->err_id) -
      (int)(((const SOC_ERROR_DESC_ELEMENT *)error_desc_element_2)->err_id) ;
  return (ret) ;
}


#define MAX_POOL_ELEMENT 10000
#define MAX_NOF_POOLS 250


static
  SOC_ERROR_DESC_ELEMENT
    *Soc_sand_p_error_desc_element = (SOC_ERROR_DESC_ELEMENT *) 0 ;

uint32
  Soc_sand_error_pool_size = 0 ;

uint32
  Soc_sand_error_pool_num_elements = 0 ;

sal_mutex_t
  Soc_sand_error_pool_mutex = 0 ;


CONST SOC_ERROR_DESC_ELEMENT
  *Soc_sand_error_pools[MAX_NOF_POOLS] = {0};

uint32
  Soc_sand_error_pool_nof_pools = 0;


int
  soc_sand_add_error_pools(
    SOC_SAND_IN SOC_ERROR_DESC_ELEMENT  **error_id_pools,
    uint32 nof_pools
  )
{
  int
    ret,
    totol_nof_errs,
    err_i,
    *pool_sizes,
    offset;
  uint32
    pool_i;
  SOC_ERROR_DESC_ELEMENT
    *error_desc_element;
  CONST SOC_ERROR_DESC_ELEMENT
    *error_desc_element_const;

  pool_sizes = soc_sand_os_malloc_any_size(nof_pools * sizeof(uint32), "pool_sizes");
  if (!pool_sizes)
  {
    ret = 2;
    goto exit;
  }

  totol_nof_errs = 0;
  for (pool_i = 0; pool_i < nof_pools; ++pool_i)
  {
    error_desc_element_const = error_id_pools[pool_i] ;

    for (err_i = 0 ; err_i < MAX_POOL_ELEMENT ; err_i++, error_desc_element_const++)
    {
      if (error_desc_element_const->err_id == SOC_SAND_END_ERR_LIST)
      {
        break ;
      }
    }

    if (err_i ==  MAX_POOL_ELEMENT)
    {
      
      ret = 5 ;
      goto exit_free ;
    }
    else
    {
      pool_sizes[pool_i] = err_i;
      totol_nof_errs += err_i;
    }
  }

  if (totol_nof_errs >  MAX_POOL_ELEMENT)
  {
    
    ret = 8 ;
    goto exit_free ;
  }

  ++totol_nof_errs; 

  error_desc_element = soc_sand_os_malloc_any_size(totol_nof_errs * sizeof(SOC_ERROR_DESC_ELEMENT),"error_desc_element");
  if (!error_desc_element)
  {
    ret = 12;
    goto exit_free;
  }

  offset = 0;
  for (pool_i = 0; pool_i < nof_pools; ++pool_i)
  {
    soc_sand_os_memcpy(
      &error_desc_element[offset],
      error_id_pools[pool_i],
      pool_sizes[pool_i] * sizeof(SOC_ERROR_DESC_ELEMENT)
      );
    offset += pool_sizes[pool_i];
  }

  error_desc_element[offset].err_id = (uint32)SOC_SAND_END_ERR_LIST;

  ret = soc_sand_add_error_pool(error_desc_element);
  soc_sand_os_free_any_size(error_desc_element);

exit_free:
  soc_sand_os_free_any_size(pool_sizes);

exit:
  return ret;
}

int
  soc_sand_add_error_pool(
    SOC_SAND_IN SOC_ERROR_DESC_ELEMENT  *error_pool
  )
{
  int
    ret ;
  char
    *loc_error_desc_element_ch;
  unsigned
    int
      ui,
      error_pool_size,
      error_pool_num_elements;
  SOC_SAND_RET
    soc_sand_ret ;
  SOC_ERROR_DESC_ELEMENT
    *error_desc_element ;
  CONST SOC_ERROR_DESC_ELEMENT
    *error_desc_element_const;
  ret = 0 ;
  if (error_pool == (SOC_ERROR_DESC_ELEMENT *)0)
  {
    ret = 6 ;
    goto exit ;
  }

  
  soc_sand_ret = soc_sand_os_task_lock() ;
  if (soc_sand_ret != SOC_SAND_OK)
  {
    ret = 1 ;
    goto exit ;
  }
  {
    
    if (Soc_sand_error_pool_mutex == 0)
    {
      
      Soc_sand_error_pool_mutex = soc_sand_os_mutex_create() ;
      if (Soc_sand_error_pool_mutex == 0)
      {
        soc_sand_os_task_unlock() ;
        ret = 2 ;
        goto exit ;
      }
    }
  }
  soc_sand_ret = soc_sand_os_task_unlock() ;
  if (soc_sand_ret != SOC_SAND_OK)
  {
    ret = 3 ;
    goto exit ;
  }
  
  soc_sand_ret = soc_sand_os_mutex_take(Soc_sand_error_pool_mutex,SOC_SAND_INFINITE_TIMEOUT) ;
  if (soc_sand_ret != SOC_SAND_OK)
  {
    ret = 4 ;
    goto exit ;
  }
  {
    

    for (ui = 0 ; ui < Soc_sand_error_pool_nof_pools ; ui++)
    {
      if (Soc_sand_error_pools[ui] == error_pool)
      {
        
        soc_sand_ret = soc_sand_os_mutex_give(Soc_sand_error_pool_mutex) ;
        if (soc_sand_ret != SOC_SAND_OK)
        {
          ret = 7 ;
        }
        goto exit;
      }
    }

    
    error_desc_element_const = error_pool ;
    for (ui = 0 ; ui < MAX_POOL_ELEMENT ; ui++, error_desc_element_const++)
    {
      if (error_desc_element_const->err_id == SOC_SAND_END_ERR_LIST)
      {
        break ;
      }
    }
    if (ui == 0)
    {
      
      soc_sand_ret = soc_sand_os_mutex_give(Soc_sand_error_pool_mutex) ;
      if (soc_sand_ret != SOC_SAND_OK)
      {
        ret = 7 ;
      }
      goto exit ;
    }
    error_pool_num_elements = ui ;
    error_pool_size = sizeof(SOC_ERROR_DESC_ELEMENT) * ui ;
    
    if (Soc_sand_p_error_desc_element)
    {
      
      SOC_ERROR_DESC_ELEMENT
        *loc_error_desc_element ;
      error_pool_size += Soc_sand_error_pool_size ;
      error_pool_num_elements += Soc_sand_error_pool_num_elements ;
      error_desc_element =
          loc_error_desc_element =
              (SOC_ERROR_DESC_ELEMENT *)soc_sand_os_malloc(error_pool_size, "loc_error_desc_element") ;
      if (error_desc_element == (SOC_ERROR_DESC_ELEMENT *)0)
      {
        
        soc_sand_os_mutex_give(Soc_sand_error_pool_mutex) ;
        ret = 8 ;
        goto exit ;
      }
      
      soc_sand_os_memcpy(error_desc_element,Soc_sand_p_error_desc_element,Soc_sand_error_pool_size) ;
      
      soc_sand_os_free(Soc_sand_p_error_desc_element);
      Soc_sand_p_error_desc_element = NULL;
      
      loc_error_desc_element_ch = ((char *)loc_error_desc_element);
      loc_error_desc_element_ch += Soc_sand_error_pool_size;
      loc_error_desc_element = ((SOC_ERROR_DESC_ELEMENT*)loc_error_desc_element_ch);
      soc_sand_os_memcpy(loc_error_desc_element,error_pool,error_pool_size - Soc_sand_error_pool_size) ;
    }
    else
    {
      
      error_desc_element = (SOC_ERROR_DESC_ELEMENT *)soc_sand_os_malloc(error_pool_size,"error_desc_element") ;
      if (error_desc_element == (SOC_ERROR_DESC_ELEMENT *)0)
      {
        
        soc_sand_os_mutex_give(Soc_sand_error_pool_mutex) ;
        ret = 9 ;
        goto exit ;
      }
      soc_sand_os_memcpy(error_desc_element,error_pool,error_pool_size) ;
    }
    
    
    soc_sand_os_qsort(
      &error_desc_element[0],error_pool_num_elements,
      sizeof(SOC_ERROR_DESC_ELEMENT),soc_compare_error_desc_elements) ;
    
    Soc_sand_p_error_desc_element = error_desc_element ;
    Soc_sand_error_pool_size = error_pool_size ;
    Soc_sand_error_pool_num_elements = error_pool_num_elements ;
    Soc_sand_error_pools[Soc_sand_error_pool_nof_pools++] = error_pool;
  }

#if SOC_SAND_ERROR_CODE_FIND_DUPLICATES 
  
    {
      uint32
        nof_errors;
      uint8
        *arr,
        end_of_lst_cnt;

      nof_errors = ((1<<SOC_ERROR_CODE_NUM_BITS)-1);
      arr = soc_sand_os_malloc_any_size(nof_errors * sizeof(*arr), "arr");

      if (!arr)
      {
        ret = 10;
        goto exit;
      }

      soc_sand_os_memset(arr, 0, nof_errors * sizeof(*arr));

      end_of_lst_cnt = 0;
      for (ui = 0; ui < error_pool_num_elements; ++ui)
      {
        if (Soc_sand_p_error_desc_element[ui].err_id == SOC_SAND_END_ERR_LIST)
        {
          if (++end_of_lst_cnt > 1)
          {
#if (SOC_SAND_DEBUG >= SOC_SAND_DBG_LVL2)
            LOG_INFO(BSL_LS_SOC_MANAGEMENT,
                     (BSL_META("SOC_SAND_END_ERR_LIST found twice.\n\r")));
#endif
            ret = SOC_SAND_ERRORS_FOUND_DUPLICATES_ERR;
            break;
          }
        }
        else if (Soc_sand_p_error_desc_element[ui].err_id >= nof_errors)
        {
#if (SOC_SAND_DEBUG >= SOC_SAND_DBG_LVL2)
          LOG_INFO(BSL_LS_SOC_MANAGEMENT,
                   (BSL_META("Error ID out of range:\n\r"
"Error_name=%s, err_id=%d\n\r"),
                    Soc_sand_p_error_desc_element[ui].err_name,
                    Soc_sand_p_error_desc_element[ui].err_id
                    ));
#endif
          ret = SOC_SAND_VALUE_OUT_OF_RANGE_ERR;
          break;
        }
        else if (++arr[Soc_sand_p_error_desc_element[ui].err_id] > 1)
        {
          uint32 find_dup_i;

#if (SOC_SAND_DEBUG >= SOC_SAND_DBG_LVL2)
          LOG_INFO(BSL_LS_SOC_MANAGEMENT,
                   (BSL_META("Duplicate found:\n\r"
"Error_name=%s, err_id=%d\n\r"),
                    Soc_sand_p_error_desc_element[ui].err_name,
                    Soc_sand_p_error_desc_element[ui].err_id
                    ));

          for (find_dup_i = 0; find_dup_i < ui; ++find_dup_i)
          {
            if (Soc_sand_p_error_desc_element[find_dup_i].err_id == Soc_sand_p_error_desc_element[ui].err_id)
            {
              LOG_INFO(BSL_LS_SOC_MANAGEMENT,
                       (BSL_META("#2: error_name=%s, proc_id=%d\n\r"),
                        Soc_sand_p_error_desc_element[find_dup_i].err_name,
                        Soc_sand_p_error_desc_element[find_dup_i].err_id
                        ));
              
            }
          }
#endif
          ret = 13;
          
        }
      }

      soc_sand_os_free_any_size(arr);
    }
#endif 

  
  soc_sand_ret = soc_sand_os_mutex_give(Soc_sand_error_pool_mutex) ;
  if (soc_sand_ret != SOC_SAND_OK)
  {
    ret = 10 ;
  }
exit:
  return (ret) ;
}

int
  soc_sand_close_all_error_pool(
    void
  )
{
  int
    ret ;
  SOC_SAND_RET
    soc_sand_ret ;
  ret = 0;
 
  soc_sand_ret = soc_sand_os_task_lock() ;
  if (soc_sand_ret != SOC_SAND_OK)
  {
    ret = 1 ;
    goto exit ;
  }
  {
    
    if (Soc_sand_error_pool_mutex == 0)
    {
      
      Soc_sand_p_error_desc_element = (SOC_ERROR_DESC_ELEMENT *)0 ;
      Soc_sand_error_pool_nof_pools = 0;
      Soc_sand_error_pool_size =
        Soc_sand_error_pool_num_elements = 0 ;
      soc_sand_ret = soc_sand_os_task_unlock() ;
      if (soc_sand_ret != SOC_SAND_OK)
      {
        ret = 2 ;
      }
      goto exit ;
    }
  }
  soc_sand_ret = soc_sand_os_task_unlock() ;
  if (soc_sand_ret != SOC_SAND_OK)
  {
    ret = 3 ;
    goto exit ;
  }
  
  soc_sand_ret = soc_sand_os_mutex_take(Soc_sand_error_pool_mutex,SOC_SAND_INFINITE_TIMEOUT) ;
  if (soc_sand_ret != SOC_SAND_OK)
  {
    ret = 4 ;
    goto exit ;
  }
  {
    
    if (Soc_sand_p_error_desc_element)
    {
      soc_sand_os_free(Soc_sand_p_error_desc_element) ;
    }
    
    Soc_sand_p_error_desc_element = (SOC_ERROR_DESC_ELEMENT *)0 ;
    Soc_sand_error_pool_nof_pools = 0;
    Soc_sand_error_pool_size =
      Soc_sand_error_pool_num_elements = 0 ;
    soc_sand_ret = soc_sand_os_task_lock() ;
    if (soc_sand_ret != SOC_SAND_OK)
    {
      ret = 5 ;
    }
    soc_sand_os_mutex_delete(Soc_sand_error_pool_mutex) ;
    Soc_sand_error_pool_mutex = 0 ;
    soc_sand_ret = soc_sand_os_task_unlock() ;
    if (soc_sand_ret != SOC_SAND_OK)
    {
      ret = 6 ;
    }
  }
exit:
  return (ret) ;
}

int
  soc_sand_add_sand_errors(
    void
  )
{
  int
    ret ;
  ret = 0 ;
  ret = soc_sand_add_error_pool(Soc_sand_error_desc_element) ;
  return (ret) ;
}

int
  soc_sand_linear_find_error(
    uint32      error_code,
    SOC_SAND_IN SOC_ERROR_DESC_ELEMENT *in_error_desc_element,
    CONST SOC_ERROR_DESC_ELEMENT **error_desc_element_ptr
  )
{
  int
    ret ;
  CONST SOC_ERROR_DESC_ELEMENT
    *error_desc_element ;
  unsigned
    int
      ui ;
  ret = 0 ;
  *error_desc_element_ptr = (SOC_ERROR_DESC_ELEMENT *)0 ;
  if (in_error_desc_element == (SOC_ERROR_DESC_ELEMENT *)0)
  {
    
    ret = 1 ;
    goto exit ;
  }
  error_desc_element = in_error_desc_element ;
  
  for (ui = 0 ; ui < MAX_POOL_ELEMENT ; ui++, error_desc_element++)
  {
    if (error_desc_element->err_id == SOC_SAND_END_ERR_LIST)
    {
      break ;
    }
    if (error_desc_element->err_id == error_code)
    {
      *error_desc_element_ptr = error_desc_element ;
      break ;
    }
  }
exit:
  return (ret) ;
}

int
  soc_sand_find_error(
    uint32      error_code,
    SOC_ERROR_DESC_ELEMENT **error_desc_element_ptr
  )
{
  int
    ret ;
  void
    *void_ptr ;
  uint32
      *error_code_ptr ;
  SOC_SAND_RET
    soc_sand_ret ;
  ret = 0 ;
  *error_desc_element_ptr = (SOC_ERROR_DESC_ELEMENT *)0 ;
  
  soc_sand_ret =
    soc_sand_os_mutex_take(Soc_sand_error_pool_mutex,SOC_SAND_INFINITE_TIMEOUT) ;
  if (soc_sand_ret != SOC_SAND_OK)
  {
    ret = 1 ;
    goto exit ;
  }
  {
    
    if (Soc_sand_p_error_desc_element == (SOC_ERROR_DESC_ELEMENT *)0)
    {
      ret = 2 ;
      goto exit ;
    }
    if ((Soc_sand_error_pool_size == 0) ||
          (Soc_sand_error_pool_num_elements == 0))
    {
      ret = 3 ;
      goto exit ;
    }
    error_code_ptr = &error_code ;
    void_ptr =
      soc_sand_os_bsearch(
        (void *)error_code_ptr,
        (void *)Soc_sand_p_error_desc_element,
        (uint32)Soc_sand_error_pool_num_elements,
        sizeof(*Soc_sand_p_error_desc_element),
        soc_compare_error_desc_elements) ;
    *error_desc_element_ptr = (SOC_ERROR_DESC_ELEMENT *)void_ptr ;
  }
  
  soc_sand_ret = soc_sand_os_mutex_give(Soc_sand_error_pool_mutex) ;
  if (soc_sand_ret != SOC_SAND_OK)
  {
    ret = 4 ;
  }
exit:
  return (ret) ;
}



CONST SOC_PROCEDURE_DESC_ELEMENT
  *soc_sand_get_procedures_ptr(
    void
  )
{
  CONST SOC_PROCEDURE_DESC_ELEMENT
    *ret ;
  ret = &Soc_sand_procedure_desc_element[0] ;
  return (ret) ;
}

int
  soc_compare_proc_desc_elements(
    void *procedure_desc_element_1,
    void *procedure_desc_element_2
  )
{
  int
    ret ;
  ret =
      (int)(((const SOC_PROCEDURE_DESC_ELEMENT *)procedure_desc_element_1)->proc_id) -
      (int)(((const SOC_PROCEDURE_DESC_ELEMENT *)procedure_desc_element_2)->proc_id) ;
  return (ret) ;
}

#define MAX_PROC_ID_POOL_ELEMENT 5000
#define MAX_NOF_PROC_ID_POOLS 250


static
  SOC_PROCEDURE_DESC_ELEMENT
    *Soc_procedure_desc_element = (SOC_PROCEDURE_DESC_ELEMENT *) 0 ;

uint32
  Soc_proc_id_pool_size = 0 ;

uint32
  Soc_proc_id_pool_num_elements = 0 ;

sal_mutex_t
  Soc_proc_id_pool_mutex = 0 ;


CONST SOC_PROCEDURE_DESC_ELEMENT
  *Soc_sand_proc_id_pools[MAX_NOF_PROC_ID_POOLS] = {0};

uint32
  Soc_sand_proc_id_pool_nof_pools = 0;

int
  soc_sand_add_proc_id_pools(
    SOC_SAND_IN SOC_PROCEDURE_DESC_ELEMENT  **proc_id_pools,
    uint32 nof_pools
  )
{
  int
    ret,
    totol_nof_procs,
    proc_i,
    *pool_sizes,
    offset;
  uint32
    pool_i;
  SOC_PROCEDURE_DESC_ELEMENT
    *procedure_desc_element;
  CONST SOC_PROCEDURE_DESC_ELEMENT
    *procedure_desc_element_const;

  pool_sizes = soc_sand_os_malloc_any_size(nof_pools * sizeof(int), "pool_sizes add_proc_id_pools");
  if (!pool_sizes)
  {
    ret = 2;
    goto exit;
  }

  totol_nof_procs = 0;
  for (pool_i = 0; pool_i < nof_pools; ++pool_i)
  {
    procedure_desc_element_const = proc_id_pools[pool_i] ;

    for (proc_i = 0 ; proc_i < MAX_PROC_ID_POOL_ELEMENT ; proc_i++, procedure_desc_element_const++)
    {
      if (procedure_desc_element_const->proc_id == SOC_SAND_END_PROC_LIST)
      {
        break ;
      }
    }

    if (proc_i ==  MAX_PROC_ID_POOL_ELEMENT)
    {
      
      ret = 5 ;
      goto exit_free ;
    }
    else
    {
      pool_sizes[pool_i] = proc_i;
      totol_nof_procs += proc_i;
    }
  }

  if (totol_nof_procs >  MAX_PROC_ID_POOL_ELEMENT)
  {
    
    ret = 8 ;
    goto exit_free ;
  }

  ++totol_nof_procs; 

  procedure_desc_element = soc_sand_os_malloc_any_size(totol_nof_procs * sizeof(SOC_PROCEDURE_DESC_ELEMENT),"procedure_desc_element");
  if (!procedure_desc_element)
  {
    ret = 12;
    goto exit_free;
  }

  offset = 0;
  for (pool_i = 0; pool_i < nof_pools; ++pool_i)
  {
    soc_sand_os_memcpy(
      &procedure_desc_element[offset],
      proc_id_pools[pool_i],
      pool_sizes[pool_i] * sizeof(SOC_PROCEDURE_DESC_ELEMENT)
      );

    offset += pool_sizes[pool_i];
  }

  procedure_desc_element[offset].proc_id = (uint32)SOC_SAND_END_PROC_LIST;

  ret = soc_sand_add_proc_id_pool(procedure_desc_element);
  soc_sand_os_free_any_size(procedure_desc_element);

exit_free:
  soc_sand_os_free_any_size(pool_sizes);

exit:
  return ret;
}

int
  soc_sand_add_proc_id_pool(
    SOC_SAND_IN SOC_PROCEDURE_DESC_ELEMENT  *proc_id_pool
  )
{
  int
    ret ;
  unsigned
    int
      ui,
      proc_id_pool_size,
      proc_id_pool_num_elements;
  char
    *loc_procedure_desc_element_ch;
  SOC_SAND_RET
    soc_sand_ret ;
  SOC_PROCEDURE_DESC_ELEMENT
    *procedure_desc_element ;
  CONST SOC_PROCEDURE_DESC_ELEMENT
    *procedure_desc_element_const;
  ret = 0 ;
  if (proc_id_pool == (SOC_PROCEDURE_DESC_ELEMENT *)0)
  {
    ret = 6 ;
    goto exit ;
  }
  
  soc_sand_ret = soc_sand_os_task_lock() ;
  if (soc_sand_ret != SOC_SAND_OK)
  {
    ret = 1 ;
    goto exit ;
  }
  {
    
    if (Soc_proc_id_pool_mutex == 0)
    {
      
      Soc_proc_id_pool_mutex = soc_sand_os_mutex_create() ;
      if (Soc_proc_id_pool_mutex == 0)
      {
        soc_sand_os_task_unlock() ;
        ret = 2 ;
        goto exit ;
      }
    }
  }
  soc_sand_ret = soc_sand_os_task_unlock() ;
  if (soc_sand_ret != SOC_SAND_OK)
  {
    ret = 3 ;
    goto exit ;
  }
  
  soc_sand_ret = soc_sand_os_mutex_take(Soc_proc_id_pool_mutex,SOC_SAND_INFINITE_TIMEOUT) ;
  if (soc_sand_ret != SOC_SAND_OK)
  {
    ret = 4 ;
    goto exit ;
  }
  {
    

    for (ui = 0 ; ui < Soc_sand_proc_id_pool_nof_pools ; ui++)
    {
      if (Soc_sand_proc_id_pools[ui] == proc_id_pool)
      {
        
        soc_sand_ret = soc_sand_os_mutex_give(Soc_proc_id_pool_mutex) ;
        if (soc_sand_ret != SOC_SAND_OK)
        {
          ret = 7 ;
        }
        goto exit;
      }
    }

    
    procedure_desc_element_const = proc_id_pool ;
    for (ui = 0 ; ui < MAX_PROC_ID_POOL_ELEMENT ; ui++, procedure_desc_element_const++)
    {
      if (procedure_desc_element_const->proc_id == SOC_SAND_END_PROC_LIST)
      {
        break ;
      }
    }
    if (ui == 0)
    {
      
      soc_sand_ret = soc_sand_os_mutex_give(Soc_proc_id_pool_mutex) ;
      if (soc_sand_ret != SOC_SAND_OK)
      {
        ret = 7 ;
      }
      goto exit ;
    }
    else if (ui ==  MAX_PROC_ID_POOL_ELEMENT)
    {
      
      soc_sand_os_mutex_give(Soc_proc_id_pool_mutex) ;
      ret = 20 ;
      goto exit ;
    }
    proc_id_pool_num_elements = ui ;
    proc_id_pool_size = sizeof(SOC_PROCEDURE_DESC_ELEMENT) * ui ;
    
    if (Soc_procedure_desc_element)
    {
      
      SOC_PROCEDURE_DESC_ELEMENT
        *loc_procedure_desc_element ;
      proc_id_pool_size += Soc_proc_id_pool_size ;
      proc_id_pool_num_elements += Soc_proc_id_pool_num_elements ;
      procedure_desc_element =
          loc_procedure_desc_element =
              (SOC_PROCEDURE_DESC_ELEMENT *)soc_sand_os_malloc(proc_id_pool_size, "loc_procedure_desc_element") ;
      if (procedure_desc_element == (SOC_PROCEDURE_DESC_ELEMENT *)0)
      {
        
        soc_sand_os_mutex_give(Soc_proc_id_pool_mutex) ;
        ret = 8 ;
        goto exit ;
      }
      
      soc_sand_os_memcpy(procedure_desc_element,
                        Soc_procedure_desc_element,Soc_proc_id_pool_size) ;
      
      soc_sand_os_free(Soc_procedure_desc_element);
      Soc_procedure_desc_element = NULL;
      
      loc_procedure_desc_element_ch = ((char *)loc_procedure_desc_element);
      loc_procedure_desc_element_ch += Soc_proc_id_pool_size ;
      loc_procedure_desc_element = ((SOC_PROCEDURE_DESC_ELEMENT*)loc_procedure_desc_element_ch);

      soc_sand_os_memcpy(loc_procedure_desc_element,proc_id_pool,
                        proc_id_pool_size - Soc_proc_id_pool_size) ;
    }
    else
    {
      
      procedure_desc_element = (SOC_PROCEDURE_DESC_ELEMENT *)soc_sand_os_malloc(proc_id_pool_size, "procedure_desc_element") ;
      if (procedure_desc_element == (SOC_PROCEDURE_DESC_ELEMENT *)0)
      {
        
        soc_sand_os_mutex_give(Soc_proc_id_pool_mutex) ;
        ret = 9 ;
        goto exit ;
      }
      soc_sand_os_memcpy(procedure_desc_element,proc_id_pool,proc_id_pool_size) ;
    }
    
    
    soc_sand_os_qsort(
      &procedure_desc_element[0],proc_id_pool_num_elements,
      sizeof(SOC_PROCEDURE_DESC_ELEMENT),soc_compare_proc_desc_elements) ;
    
    Soc_procedure_desc_element = procedure_desc_element ;
    Soc_proc_id_pool_size = proc_id_pool_size ;
    Soc_proc_id_pool_num_elements = proc_id_pool_num_elements ;
    Soc_sand_proc_id_pools[Soc_sand_proc_id_pool_nof_pools++] = proc_id_pool;

#if SOC_SAND_ERROR_CODE_FIND_DUPLICATES 
    
    {
      uint32
        nof_errors;
      uint8
        *arr,
        end_of_lst_cnt;

      nof_errors = ((1<<SOC_PROC_ID_NUM_BITS)-1);
      arr = soc_sand_os_malloc_any_size(nof_errors * sizeof(*arr), "arr soc_sand_add_proc_id_pool");

      if (!arr)
      {
        ret = 10;
        goto exit;
      }

      soc_sand_os_memset(arr, 0, nof_errors * sizeof(*arr));

      end_of_lst_cnt = 0;
      for (ui = 0; ui < Soc_proc_id_pool_num_elements; ++ui)
      {
        if (Soc_procedure_desc_element[ui].proc_id == SOC_SAND_END_PROC_LIST)
        {
          if (++end_of_lst_cnt > 1)
          {
#if (SOC_SAND_DEBUG >= SOC_SAND_DBG_LVL2)
            LOG_INFO(BSL_LS_SOC_MANAGEMENT,
                     (BSL_META("SOC_SAND_END_PROC_LIST found twice.\n\r")));
#endif
            ret = SOC_SAND_PROCS_FOUND_DUPLICATES_ERR;
            break;
          }
        }
        else if (Soc_procedure_desc_element[ui].proc_id >= nof_errors)
        {
          ret = 12;
          break;
        }
        else if (++arr[Soc_procedure_desc_element[ui].proc_id] > 1)
        {
#if (SOC_SAND_DEBUG >= SOC_SAND_DBG_LVL2)
          uint32 find_dup_i;

          LOG_INFO(BSL_LS_SOC_MANAGEMENT,
                   (BSL_META("Duplicate found:\n\r")));
          LOG_INFO(BSL_LS_SOC_MANAGEMENT,
                   (BSL_META("#1: proc_name=%s, proc_id=%d\n\r"),
                    Soc_procedure_desc_element[ui].proc_name,
                    Soc_procedure_desc_element[ui].proc_id
                    ));

          for (find_dup_i = 0; find_dup_i < ui; ++find_dup_i)
          {
            if (Soc_procedure_desc_element[find_dup_i].proc_id == Soc_procedure_desc_element[ui].proc_id)
            {
              LOG_INFO(BSL_LS_SOC_MANAGEMENT,
                       (BSL_META("#2: proc_name=%s, proc_id=%d\n\r"),
                        Soc_procedure_desc_element[find_dup_i].proc_name,
                        Soc_procedure_desc_element[find_dup_i].proc_id
                        ));
              break;
            }
          }
#endif          
          ret = 13;
        }
      }

      soc_sand_os_free_any_size(arr);
    }
#endif 

  }
  
  soc_sand_ret = soc_sand_os_mutex_give(Soc_proc_id_pool_mutex) ;
  if (soc_sand_ret != SOC_SAND_OK)
  {
    ret = 13 ;
  }
exit:
  return (ret) ;
}

int
  soc_sand_close_all_proc_id_pool(
    void
  )
{
  int
    ret ;
  SOC_SAND_RET
    soc_sand_ret ;
  
  ret = 0;
  
  soc_sand_ret = soc_sand_os_task_lock() ;
  if (soc_sand_ret != SOC_SAND_OK)
  {
    ret = 1 ;
    goto exit ;
  }
  {
    
    if (Soc_proc_id_pool_mutex == 0)
    {
      
      Soc_procedure_desc_element = (SOC_PROCEDURE_DESC_ELEMENT *)0 ;
      Soc_sand_proc_id_pool_nof_pools = 0;
      Soc_proc_id_pool_size =
        Soc_proc_id_pool_num_elements = 0 ;
      soc_sand_ret = soc_sand_os_task_unlock() ;
      if (soc_sand_ret != SOC_SAND_OK)
      {
        ret = 2 ;
      }
      goto exit ;
    }
  }
  soc_sand_ret = soc_sand_os_task_unlock() ;
  if (soc_sand_ret != SOC_SAND_OK)
  {
    ret = 3 ;
    goto exit ;
  }
  
  soc_sand_ret = soc_sand_os_mutex_take(Soc_proc_id_pool_mutex,SOC_SAND_INFINITE_TIMEOUT) ;
  if (soc_sand_ret != SOC_SAND_OK)
  {
    ret = 4 ;
    goto exit ;
  }
  {
    
    if (Soc_procedure_desc_element)
    {
      soc_sand_os_free(Soc_procedure_desc_element) ;
    }
    
    Soc_procedure_desc_element = (SOC_PROCEDURE_DESC_ELEMENT *)0 ;
    Soc_sand_proc_id_pool_nof_pools = 0;
    Soc_proc_id_pool_size =
      Soc_proc_id_pool_num_elements = 0 ;
    soc_sand_ret = soc_sand_os_task_lock() ;
    if (soc_sand_ret != SOC_SAND_OK)
    {
      ret = 5 ;
    }
    soc_sand_os_mutex_delete(Soc_proc_id_pool_mutex) ;
    Soc_proc_id_pool_mutex = 0 ;
    soc_sand_ret = soc_sand_os_task_unlock() ;
    if (soc_sand_ret != SOC_SAND_OK)
    {
      ret = 6 ;
    }
  }
exit:
  return (ret) ;
}

int
  soc_sand_add_sand_procedures(
    void
  )
{
  int
    ret ;
  ret = 0 ;
  ret = soc_sand_add_proc_id_pool(Soc_sand_procedure_desc_element) ;
  return (ret) ;
}

int
  soc_sand_find_procedure(
    uint32          proc_id,
    SOC_PROCEDURE_DESC_ELEMENT **procedure_desc_element_ptr
  )
{
  int
    ret ;
  void
    *void_ptr ;
  uint32
      *proc_id_ptr ;
  SOC_SAND_RET
    soc_sand_ret ;
  ret = 0 ;
  *procedure_desc_element_ptr = (SOC_PROCEDURE_DESC_ELEMENT *)0 ;
  if (Soc_proc_id_pool_mutex == 0)
  {
    ret = 5 ;
    goto exit ;
  }
  
  soc_sand_ret =
    soc_sand_os_mutex_take(Soc_proc_id_pool_mutex,SOC_SAND_INFINITE_TIMEOUT) ;
  if (soc_sand_ret != SOC_SAND_OK)
  {
    ret = 1 ;
    goto exit ;
  }
  {
    
    if (Soc_procedure_desc_element == (SOC_PROCEDURE_DESC_ELEMENT *)0)
    {
      ret = 2 ;
      goto exit ;
    }
    if ((Soc_proc_id_pool_size == 0) ||
          (Soc_proc_id_pool_num_elements == 0))
    {
      ret = 3 ;
      goto exit ;
    }
    proc_id_ptr = &proc_id ;
    void_ptr =
      soc_sand_os_bsearch(
        (void *)proc_id_ptr,
        (void *)Soc_procedure_desc_element,
        (uint32)Soc_proc_id_pool_num_elements,
        sizeof(*Soc_procedure_desc_element),
        soc_compare_proc_desc_elements) ;
    *procedure_desc_element_ptr = (SOC_PROCEDURE_DESC_ELEMENT *)void_ptr ;
  }
  
  soc_sand_ret = soc_sand_os_mutex_give(Soc_proc_id_pool_mutex) ;
  if (soc_sand_ret != SOC_SAND_OK)
  {
    ret = 4 ;
  }
exit:
  return (ret) ;
}

int
  soc_sand_linear_find_procedure(
    uint32          proc_id,
    SOC_SAND_IN SOC_PROCEDURE_DESC_ELEMENT *in_procedure_desc_element,
    CONST SOC_PROCEDURE_DESC_ELEMENT **procedure_desc_element_ptr
  )
{
  int
    ret ;
  CONST SOC_PROCEDURE_DESC_ELEMENT
    *procedure_desc_element ;
  unsigned
    int
      ui ;
  ret = 0 ;
  *procedure_desc_element_ptr = (SOC_PROCEDURE_DESC_ELEMENT *)0 ;
  if (in_procedure_desc_element == (SOC_PROCEDURE_DESC_ELEMENT *)0)
  {
    
    ret = 1 ;
    goto exit ;
  }
  procedure_desc_element = in_procedure_desc_element ;
  
  for (ui = 0 ; ui < MAX_PROC_ID_POOL_ELEMENT ; ui++, procedure_desc_element++)
  {
    if (procedure_desc_element->proc_id == SOC_SAND_END_PROC_LIST)
    {
      break ;
    }
    if (procedure_desc_element->proc_id == proc_id)
    {
      *procedure_desc_element_ptr = procedure_desc_element ;
      break ;
    }
  }
exit:
  return (ret) ;
}


int
  soc_sand_proc_id_to_string(
    SOC_SAND_IN  uint32 in_proc_id,
    SOC_SAND_OUT char    **out_module_name,
    SOC_SAND_OUT char    **out_proc_name
  )
{
  int
    err ;
  SOC_PROCEDURE_DESC_ELEMENT
    *procedure_desc_element ;
  unsigned
    int
      module_id ;
  module_id = in_proc_id >> (SOC_PROC_ID_NUM_BITS - SOC_SAND_MODULE_ID_NUM_BITS) ;
  module_id &= (SOC_SAND_BIT(SOC_SAND_MODULE_ID_NUM_BITS) - 1) ;
  switch (module_id)
  {
    case SOC_SAND_MODULE_IDENTIFIER:
    {
      *out_module_name = "SOC_SAND module" ;
      break ;
    }
    case FE200_MODULE_IDENTIFIER:
    {
      *out_module_name = "SOC_SAND_FE200 module" ;
      break ;
    }
    case FE600_MODULE_IDENTIFIER:
    {
      *out_module_name = "SOC_SAND_FE600 module" ;
      break ;
    }
    case FAP10M_MODULE_IDENTIFIER:
    {
      *out_module_name = "SOC_SAND_FAP10M module" ;
      break ;
    }
    case FAP20V_MODULE_IDENTIFIER:
    {
      *out_module_name = "SOC_SAND_FAP20V module" ;
      break ;
    }
    case FAP21V_MODULE_IDENTIFIER:
    {
      *out_module_name = "SOC_SAND_FAP21V module" ;
      break ;
    }
    case SOC_PETRA_MODULE_IDENTIFIER:
    {
      *out_module_name = "SOC_PETRA module" ;
      break ;
    }
    case SOC_PETRA_PP_MODULE_IDENTIFIER:
    {
      *out_module_name = "SOC_PETRA PP module" ;
      break ;
    }
    case SOC_PB_MODULE_IDENTIFIER:
    {
      *out_module_name = "SOC_PETRA-B module" ;
      break ;
    }
    case SOC_PB_PP_MODULE_IDENTIFIER:
    {
      *out_module_name = "SOC_PETRA-B PP module" ;
      break ;
    }
    case SOC_TMC_MODULE_IDENTIFIER:
    {
      *out_module_name = "TMC module" ;
      break ;
    }
    case TMD_MODULE_IDENTIFIER:
    {
      *out_module_name = "TMD module" ;
      break ;
    }    
    case SOC_PPD_MODULE_IDENTIFIER:
    {
      *out_module_name = "PPD module" ;
      break ;
    }
    case ARAD_MODULE_IDENTIFIER:
    {
      *out_module_name = "ARAD module" ;
      break ;
    }
    case ARAD_PP_MODULE_IDENTIFIER:
    {
      *out_module_name = "ARAD PP module" ;
      break ;
    }
    case T20E_MODULE_IDENTIFIER:
    {
      *out_module_name = "T20E module" ;
      break ;
    }
    case FMF_MODULE_IDENTIFIER:
    {
      *out_module_name = "FMF or PRDS module" ;
      break ;
    }
    default:
    {
      *out_module_name = "Unknown_module" ;
      *out_proc_name = "Unknown_procedure" ;
      err = 1 ;
      goto exit;
    }
  }
  err = soc_sand_find_procedure(in_proc_id,&procedure_desc_element) ;
  if (err || (procedure_desc_element == (SOC_PROCEDURE_DESC_ELEMENT *)0))
  {
    *out_proc_name = "Unknown_procedure" ;
    err = 1 ;
  }
  else
  {
    
    *out_proc_name = (char *)procedure_desc_element->proc_name ;
    err = 0 ;
  }
exit:
  return (err) ;
}

int
  soc_sand_error_code_to_string(
    SOC_SAND_IN  uint32 in_error_code,
    SOC_SAND_OUT char    **out_error_name,
    SOC_SAND_OUT char    **out_error_description
  )
{
  int
    err ;
  SOC_ERROR_DESC_ELEMENT
    *error_desc_element ;
  err = soc_sand_find_error(in_error_code,&error_desc_element) ;
  if (err || (error_desc_element == (SOC_ERROR_DESC_ELEMENT *)0) || (error_desc_element->err_name  == (char *)0))
  {
    *out_error_name = "Unknown_error_code" ;
    *out_error_description = "Unknown error description" ;
    err = 1 ;
  }
  else
  {
    
    *out_error_name = error_desc_element->err_name ;
    *out_error_description = error_desc_element->err_text ;
    err = 0 ;
  }
  return (err) ;
}
void
  soc_sand_error_code_handler(
    SOC_SAND_IN SOC_SAND_RET error_code,
    SOC_SAND_IN char*    error_file,
    SOC_SAND_IN int      error_line,
    SOC_SAND_IN char*    error_func_name,
    SOC_SAND_IN char*    error_msg
  )
{
  char* error_name ;
  char* error_description ;
  char  msg[SOC_SAND_CALLBACK_BUF_SIZE] ;
  uint32 msg_index ;
  uint32 str_size ;

  if (SOC_SAND_OK == error_code)
  {
    
    goto exit ;
  }

  

  error_name = NULL ;
  error_description = NULL ;

  soc_sand_error_code_to_string(error_code, &error_name, &error_description) ;

  msg[0] = '\n' ;
  msg[1] = '\r' ;
  msg[2] = '\0' ;
  msg_index = 2 ;
  if (NULL != error_file)
  {
    str_size = soc_sand_os_strlen(error_file) ;
    if (str_size + msg_index  + 5 < SOC_SAND_CALLBACK_BUF_SIZE-1)
    {
      if (!(Soc_no_error_printing))
      {
        sal_sprintf(msg + msg_index, "File:%s:%d", error_file, error_line) ;
      }
      msg_index = soc_sand_os_strlen(msg) ;
    }
  }
  if (NULL != error_func_name)
  {
    str_size = soc_sand_os_strlen(error_func_name) ;
    if (str_size + msg_index < SOC_SAND_CALLBACK_BUF_SIZE-1)
    {
      if (!(Soc_no_error_printing))
      {
        sal_sprintf(msg + msg_index, "\r\nFunc Name:%s", error_func_name) ;
      }
      msg_index = soc_sand_os_strlen(msg) ;
    }
  }
  if (NULL != error_name)
  {
    str_size = soc_sand_os_strlen(error_name) ;
    if (str_size + msg_index < SOC_SAND_CALLBACK_BUF_SIZE-1)
    {
      if (!(Soc_no_error_printing))
      {
        sal_sprintf(msg + msg_index, "\r\nError Name:%s", error_name) ;
      }
      msg_index = soc_sand_os_strlen(msg) ;
    }
  }
  if (NULL != error_description)
  {
    str_size = soc_sand_os_strlen(error_description) ;
    if (str_size + msg_index < SOC_SAND_CALLBACK_BUF_SIZE-1)
    {
      if (!(Soc_no_error_printing))
      {
        sal_sprintf(msg + msg_index, "\r\nError Description:%s", error_description) ;
      }
      msg_index = soc_sand_os_strlen(msg) ;
    }
  }
  if (NULL != error_msg)
  {
    str_size = soc_sand_os_strlen(error_msg) ;
    if (str_size + msg_index < SOC_SAND_CALLBACK_BUF_SIZE-1)
    {
      if (!(Soc_no_error_printing))
      {
        sal_sprintf(msg + msg_index, "\r\nError Message:%s", error_msg) ;
      }
      msg_index = soc_sand_os_strlen(msg) ;
    }
  }
  soc_sand_error_handler(error_code, msg, 0, 0, 0, 0, 0, 0) ;
exit:
  return ;
}



SOC_SAND_RET
  soc_sand_check_CREDIT_WORTH(
    SOC_SAND_IN  SOC_SAND_CREDIT_SIZE credit_worth
  )
{
  SOC_SAND_RET
    soc_sand_ret;

  if( (credit_worth == SOC_SAND_CR_256) ||
      (credit_worth == SOC_SAND_CR_512) ||
      (credit_worth == SOC_SAND_CR_1024) ||
      (credit_worth == SOC_SAND_CR_2048) ||
      (credit_worth == SOC_SAND_CR_4096) ||
      (credit_worth == SOC_SAND_CR_8192)
   )
  {
    soc_sand_ret = SOC_SAND_OK;
  }
  else
  {
    soc_sand_ret = SOC_SAND_CREDIT_SIZE_ENUM_ERR;
  }
  return soc_sand_ret;
}

#if SOC_SAND_DEBUG




int
  soc_sand_disp_result(
    SOC_SAND_IN uint32 driver_api_result
  )
{
  int
    ret ;
  uint32
      proc_id ;
  int
      err_id ;
  char
    *err_name,
    *err_text,
    *soc_sand_proc_name,
    *soc_sand_module_name ;
  unsigned short
    error_code;

  ret = 0 ;
  error_code = soc_sand_get_error_code_from_error_word( driver_api_result ) ;
  err_id = soc_sand_error_code_to_string(error_code, &err_name,&err_text) ;
  
  if (soc_sand_get_error_code_from_error_word(err_id) != SOC_SAND_OK)
  {
    err_text = "No error code description (or procedure id) found" ;
  }

  proc_id = soc_sand_get_proc_id_from_error_word(driver_api_result) ;
  soc_sand_proc_id_to_string(proc_id,&soc_sand_module_name,&soc_sand_proc_name)  ;
  if (error_code != SOC_SAND_OK)
  {
    if (!(Soc_no_error_printing))
    {
      LOG_CLI((BSL_META("  Code 0x%X (fail):\r\n"
                        "Text        : %s\n\r" 
                        "%s\r\n"               
                        "Procedure id: 0x%04X (Mod: %s, Proc: %s)\n\r"),
               driver_api_result, err_name, err_text,
               proc_id,
               soc_sand_module_name,
               soc_sand_proc_name
               ));
    }
  }
  else
  {
    
  }
  return (ret) ;
}
int
  soc_sand_disp_result_proc(
    SOC_SAND_IN uint32 driver_api_result,
    SOC_SAND_IN char              *proc_name
  )
{
  int
    ret ;
  unsigned short
    error_code;

  ret = 0 ;
  error_code = soc_sand_get_error_code_from_error_word( driver_api_result ) ;
  if (error_code != SOC_SAND_OK)
  {
    if (!(Soc_no_error_printing))
    {
      
      LOG_CLI((BSL_META("\n\r"
                        "**>\'%s\' -\n\r"),
               proc_name));
    }
  }
  ret = soc_sand_disp_result(driver_api_result);

  return (ret) ;
}


void
  soc_sand_no_error_printing_set(
    uint8   no_error_printing
    )
{
  Soc_no_error_printing = no_error_printing ;
}

uint8
  soc_sand_no_error_printing_get(
       void
    )
{
  return Soc_no_error_printing;
}



#endif

