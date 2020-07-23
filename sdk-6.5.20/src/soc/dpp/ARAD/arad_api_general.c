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

#define _ERR_MSG_MODULE_NAME BSL_SOC_COMMON



#include <shared/bsl.h>

#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_api_general.h>
#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_sw_db.h>



























uint32
  arad_fap_port_id_verify(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID  port_id
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_FAP_PORT_ID_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    port_id, ARAD_MAX_FAP_PORT_ID,
    ARAD_FAP_PORT_ID_INVALID_ERR,10,exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_fap_port_id_verify()",port_id,0);
}

uint32
  arad_drop_precedence_verify(
    SOC_SAND_IN  uint32      dp_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_DROP_PRECEDENCE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    dp_ndx, ARAD_MAX_DROP_PRECEDENCE,
    ARAD_DROP_PRECEDENCE_OUT_OF_RANGE_ERR,10,exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_fap_port_id_verify()",dp_ndx,0);
}

uint32
  arad_traffic_class_verify(
    SOC_SAND_IN  uint32      tc_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_TRAFFIC_CLASS_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    tc_ndx, ARAD_TR_CLS_MAX,
    ARAD_TRAFFIC_CLASS_OUT_OF_RANGE_ERR,10,exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_traffic_class_verify()",tc_ndx,0);
}

uint32
  arad_if_type_from_id(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  ARAD_INTERFACE_ID     arad_nif_id,
    SOC_SAND_OUT ARAD_INTERFACE_TYPE   *interface_type
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IF_TYPE_FROM_ID);

  SOC_SAND_CHECK_NULL_INPUT(interface_type);

  *interface_type = ARAD_IF_TYPE_NONE;

  if (ARAD_NIF_IS_TYPE_ID(XAUI, arad_nif_id))
  {
    *interface_type = ARAD_IF_TYPE_NIF;
  }
  else if (ARAD_NIF_IS_TYPE_ID(RXAUI, arad_nif_id))
  {
    *interface_type = ARAD_IF_TYPE_NIF;
  }
  else if (ARAD_NIF_IS_TYPE_ID(10GBASE_R, arad_nif_id))
  {
    *interface_type = ARAD_IF_TYPE_NIF;
  }
  else if (ARAD_NIF_IS_TYPE_ID(SGMII, arad_nif_id))
  {
    *interface_type = ARAD_IF_TYPE_NIF;
  }
  else if (ARAD_NIF_IS_TYPE_ID(ILKN, arad_nif_id))
  {
    *interface_type = ARAD_IF_TYPE_NIF;
  }
  else if (ARAD_NIF_IS_TYPE_ID(CGE, arad_nif_id))
  {
    *interface_type = ARAD_IF_TYPE_NIF;
  }
  else if (ARAD_NIF_IS_TYPE_ID(XLGE, arad_nif_id))
  {
    *interface_type = ARAD_IF_TYPE_NIF;
  }
  else if (ARAD_IS_CPU_IF_ID(arad_nif_id))
  {
    *interface_type = ARAD_IF_TYPE_CPU;
  }
  else if (ARAD_IS_OLP_IF_ID(arad_nif_id))
  {
    *interface_type = ARAD_IF_TYPE_OLP;
  }
  else if (ARAD_IS_OAMP_IF_ID(arad_nif_id))
  {
    *interface_type = ARAD_IF_TYPE_OAMP;
  }
  else if (ARAD_IS_RCY_IF_ID(arad_nif_id))
  {
    *interface_type = ARAD_IF_TYPE_RCY;
  }
  else if (ARAD_IS_ERP_IF_ID(arad_nif_id))
  {
    *interface_type = ARAD_IF_TYPE_ERP;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_if_type_from_id()",arad_nif_id,0);
}


uint32
  arad_interface_id_verify(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  ARAD_INTERFACE_ID      arad_nif_id
  )
{
  uint8
    not_found = TRUE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_INTERFACE_ID_VERIFY);

  if (ARAD_NIF_IS_TYPE_ID(XAUI, arad_nif_id))
  {
    not_found = FALSE;
  }
  else if (ARAD_NIF_IS_TYPE_ID(RXAUI, arad_nif_id))
  {
    not_found = FALSE;
  }
  else if (ARAD_NIF_IS_TYPE_ID(SGMII, arad_nif_id))
  {
    not_found = FALSE;
  }
  else if (ARAD_NIF_IS_TYPE_ID(10GBASE_R, arad_nif_id))
  {
    not_found = FALSE;
  }
  else if (ARAD_NIF_IS_TYPE_ID(CGE, arad_nif_id))
  {
    not_found = FALSE;
  }
  else if (ARAD_NIF_IS_TYPE_ID(XLGE, arad_nif_id))
  {
    not_found = FALSE;
  }
  else if (ARAD_NIF_IS_TYPE_ID(ILKN, arad_nif_id))
  {
    not_found = FALSE;
  }
  else if (ARAD_IS_CPU_IF_ID(arad_nif_id))
  {
    not_found = FALSE;
  }
  else if (ARAD_IS_OLP_IF_ID(arad_nif_id))
  {
    not_found = FALSE;
  }
  else if (ARAD_IS_TM_INTERNAL_PKT_IF_ID(arad_nif_id))
  {
    not_found = FALSE;
  }
  else if (ARAD_IS_OAMP_IF_ID(arad_nif_id))
  {
    not_found = FALSE;
  }
  else if (ARAD_IS_RCY_IF_ID(arad_nif_id))
  {
    not_found = FALSE;
  }
  else if (ARAD_IS_ERP_IF_ID(arad_nif_id))
  {
    not_found = FALSE;
  }
  else if (ARAD_IS_NONE_IF_ID(arad_nif_id))
  {
    not_found = FALSE;
  }

  if(not_found)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_INTERFACE_INDEX_OUT_OF_RANGE_ERR, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_interface_id_verify(unit, )",arad_nif_id,0);
}

void
  arad_ARAD_DEST_SYS_PORT_INFO_clear(
    SOC_SAND_OUT ARAD_DEST_SYS_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_DEST_SYS_PORT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_DEST_INFO_clear(
    SOC_SAND_OUT ARAD_DEST_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_DEST_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_THRESH_WITH_HYST_INFO_clear(
    SOC_SAND_OUT ARAD_THRESH_WITH_HYST_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_THRESH_WITH_HYST_INFO_clear(info);
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_SWAP_INFO_clear(
    SOC_SAND_OUT ARAD_SWAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_SWAP_INFO_clear(info);
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if ARAD_DEBUG_IS_LVL1
const char*
  arad_ARAD_FAR_DEVICE_TYPE_to_string(
    SOC_SAND_IN  ARAD_FAR_DEVICE_TYPE enum_val
  )
{
  return SOC_TMC_FAR_DEVICE_TYPE_to_string(enum_val);
}

const char*
  arad_ARAD_INTERFACE_TYPE_to_string(
    SOC_SAND_IN  ARAD_INTERFACE_TYPE enum_val
  )
{
  return SOC_TMC_INTERFACE_TYPE_to_string(enum_val);
}

const char*
  arad_ARAD_INTERFACE_ID_to_string(
    SOC_SAND_IN  ARAD_INTERFACE_ID enum_val
  )
{
  return SOC_TMC_INTERFACE_ID_to_string(enum_val);
}

const char*
  arad_ARAD_FC_DIRECTION_to_string(
    SOC_SAND_IN  ARAD_FC_DIRECTION enum_val
  )
{
  return SOC_TMC_FC_DIRECTION_to_string(enum_val);
}

const char*
  arad_ARAD_COMBO_QRTT_to_string(
    SOC_SAND_IN  ARAD_COMBO_QRTT enum_val
  )
{
  return SOC_TMC_COMBO_QRTT_to_string(enum_val);
}

const char*
  arad_ARAD_DEST_TYPE_to_string(
    SOC_SAND_IN ARAD_DEST_TYPE enum_val,
    SOC_SAND_IN uint8       short_name
  )
{
  return SOC_TMC_DEST_TYPE_to_string(enum_val, short_name);
}

const char*
  arad_ARAD_DEST_SYS_PORT_TYPE_to_string(
    SOC_SAND_IN  ARAD_DEST_SYS_PORT_TYPE enum_val
  )
{
  return SOC_TMC_DEST_SYS_PORT_TYPE_to_string(enum_val);
}

const char*
  arad_ARAD_CONNECTION_DIRECTION_to_string(
    SOC_SAND_IN  ARAD_CONNECTION_DIRECTION enum_val
  )
{
  return SOC_TMC_CONNECTION_DIRECTION_to_string(enum_val);
}

void
  arad_ARAD_INTERFACE_ID_print(
    SOC_SAND_IN ARAD_INTERFACE_ID if_ndx
  )
{
  LOG_CLI((BSL_META("Interface index: %s\n\r"),arad_ARAD_INTERFACE_ID_to_string(if_ndx)));
}

void
  arad_ARAD_DEST_SYS_PORT_INFO_print(
    SOC_SAND_IN  ARAD_DEST_SYS_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_DEST_SYS_PORT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_DEST_INFO_print(
    SOC_SAND_IN  ARAD_DEST_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_DEST_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_THRESH_WITH_HYST_INFO_print(
    SOC_SAND_IN ARAD_THRESH_WITH_HYST_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_THRESH_WITH_HYST_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  arad_ARAD_DEST_SYS_PORT_INFO_table_format_print(
    SOC_SAND_IN ARAD_DEST_SYS_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_DEST_SYS_PORT_INFO_table_format_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

const char*
  arad_ARAD_SWAP_MODE_to_string(
    SOC_SAND_IN  ARAD_SWAP_MODE enum_val
  )
{
  return SOC_TMC_SWAP_MODE_to_string(enum_val);
}

void
  arad_ARAD_SWAP_INFO_print(
    SOC_SAND_IN ARAD_SWAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_SWAP_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 


#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif 
