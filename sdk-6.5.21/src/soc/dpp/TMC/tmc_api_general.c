/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/







#include <shared/bsl.h>

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/TMC/tmc_api_general.h>


























const char*
  SOC_TMC_FAR_DEVICE_TYPE_to_string(
    SOC_SAND_IN SOC_TMC_FAR_DEVICE_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_FAR_DEVICE_TYPE_FE1:
    str = "FE1";
  break;

  case SOC_TMC_FAR_DEVICE_TYPE_FE2:
    str = "FE2";
  break;

  case SOC_TMC_FAR_DEVICE_TYPE_FE3:
    str = "FE3";
  break;

  case SOC_TMC_FAR_DEVICE_TYPE_FAP:
    str = "FAP";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_INTERFACE_TYPE_to_string(
    SOC_SAND_IN SOC_TMC_INTERFACE_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_IF_TYPE_NONE:
    str = "NONE";
  break;

  case SOC_TMC_IF_TYPE_CPU:
    str = "CPU";
  break;

  case SOC_TMC_IF_TYPE_RCY:
    str = "RCY";
  break;

  case SOC_TMC_IF_TYPE_OLP:
    str = "OLP";
  break;

  case SOC_TMC_IF_TYPE_NIF:
    str = "NIF";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_INTERFACE_ID_to_string(
    SOC_SAND_IN  SOC_TMC_INTERFACE_ID enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_IF_ID_0:
    str = "0";
  break;
  case SOC_TMC_IF_ID_1:
    str = "1";
  break;
  case SOC_TMC_IF_ID_2:
    str = "2";
  break;
  case SOC_TMC_IF_ID_3:
    str = "3";
  break;
  case SOC_TMC_IF_ID_4:
    str = "4";
  break;
  case SOC_TMC_IF_ID_5:
    str = "5";
  break;
  case SOC_TMC_IF_ID_6:
    str = "6";
  break;
  case SOC_TMC_IF_ID_7:
    str = "7";
  break;
  case SOC_TMC_IF_ID_8:
    str = "8";
  break;
  case SOC_TMC_IF_ID_9:
    str = "9";
  break;
  case SOC_TMC_IF_ID_10:
    str = "10";
  break;
  case SOC_TMC_IF_ID_11:
    str = "11";
  break;
  case SOC_TMC_IF_ID_12:
    str = "12";
  break;
  case SOC_TMC_IF_ID_13:
    str = "13";
  break;
  case SOC_TMC_IF_ID_14:
    str = "14";
  break;
  case SOC_TMC_IF_ID_15:
    str = "15";
  break;
  case SOC_TMC_IF_ID_16:
    str = "16";
  break;
  case SOC_TMC_IF_ID_17:
    str = "17";
  break;
  case SOC_TMC_IF_ID_18:
    str = "18";
  break;
  case SOC_TMC_IF_ID_19:
    str = "19";
  break;
  case SOC_TMC_IF_ID_20:
    str = "20";
  break;
  case SOC_TMC_IF_ID_21:
    str = "21";
  break;
  case SOC_TMC_IF_ID_22:
    str = "22";
  break;
  case SOC_TMC_IF_ID_23:
    str = "23";
  break;
  case SOC_TMC_IF_ID_24:
    str = "24";
  break;
  case SOC_TMC_IF_ID_25:
    str = "25";
  break;
  case SOC_TMC_IF_ID_26:
    str = "26";
  break;
  case SOC_TMC_IF_ID_27:
    str = "27";
  break;
  case SOC_TMC_IF_ID_28:
    str = "28";
  break;
  case SOC_TMC_IF_ID_29:
    str = "29";
  break;
  case SOC_TMC_IF_ID_30:
    str = "30";
  break;
  case SOC_TMC_IF_ID_31:
    str = "31";
  break;
  case SOC_TMC_NIF_ID_XAUI_0:
    str = "XAUI-0";
  break;
  case SOC_TMC_NIF_ID_XAUI_1:
    str = "XAUI-1";
  break;
  case SOC_TMC_NIF_ID_XAUI_2:
    str = "XAUI-2";
  break;
  case SOC_TMC_NIF_ID_XAUI_3:
    str = "XAUI-3";
  break;
  case SOC_TMC_NIF_ID_XAUI_4:
    str = "XAUI-4";
  break;
  case SOC_TMC_NIF_ID_XAUI_5:
    str = "XAUI-5";
  break;
  case SOC_TMC_NIF_ID_XAUI_6:
    str = "XAUI-6";
  break;
  case SOC_TMC_NIF_ID_XAUI_7:
    str = "XAUI-7";
  break;
  case SOC_TMC_NIF_ID_RXAUI_0:
    str = "RXAUI-0";
  break;
  case SOC_TMC_NIF_ID_RXAUI_1:
    str = "RXAUI-1";
  break;
  case SOC_TMC_NIF_ID_RXAUI_2:
    str = "RXAUI-2";
  break;
  case SOC_TMC_NIF_ID_RXAUI_3:
    str = "RXAUI-3";
  break;
  case SOC_TMC_NIF_ID_RXAUI_4:
    str = "RXAUI-4";
  break;
  case SOC_TMC_NIF_ID_RXAUI_5:
    str = "RXAUI-5";
  break;
  case SOC_TMC_NIF_ID_RXAUI_6:
    str = "RXAUI-6";
  break;
  case SOC_TMC_NIF_ID_RXAUI_7:
    str = "RXAUI-7";
  break;
  case SOC_TMC_NIF_ID_RXAUI_8:
    str = "RXAUI-8";
  break;
  case SOC_TMC_NIF_ID_RXAUI_9:
    str = "RXAUI-9";
  break;
  case SOC_TMC_NIF_ID_RXAUI_10:
    str = "RXAUI-10";
  break;
  case SOC_TMC_NIF_ID_RXAUI_11:
    str = "RXAUI-11";
  break;
  case SOC_TMC_NIF_ID_RXAUI_12:
    str = "RXAUI-12";
  break;
  case SOC_TMC_NIF_ID_RXAUI_13:
    str = "RXAUI-13";
  break;
  case SOC_TMC_NIF_ID_RXAUI_14:
    str = "RXAUI-14";
  break;
  case SOC_TMC_NIF_ID_RXAUI_15:
    str = "RXAUI-15";
  break;
  case SOC_TMC_NIF_ID_SGMII_0:
    str = "SGMII-0";
  break;
  case SOC_TMC_NIF_ID_SGMII_1:
    str = "SGMII-1";
  break;
  case SOC_TMC_NIF_ID_SGMII_2:
    str = "SGMII-2";
  break;
  case SOC_TMC_NIF_ID_SGMII_3:
    str = "SGMII-3";
  break;
  case SOC_TMC_NIF_ID_SGMII_4:
    str = "SGMII-4";
  break;
  case SOC_TMC_NIF_ID_SGMII_5:
    str = "SGMII-5";
  break;
  case SOC_TMC_NIF_ID_SGMII_6:
    str = "SGMII-6";
  break;
  case SOC_TMC_NIF_ID_SGMII_7:
    str = "SGMII-7";
  break;
  case SOC_TMC_NIF_ID_SGMII_8:
    str = "SGMII-8";
  break;
  case SOC_TMC_NIF_ID_SGMII_9:
    str = "SGMII-9";
  break;
  case SOC_TMC_NIF_ID_SGMII_10:
    str = "SGMII-10";
  break;
  case SOC_TMC_NIF_ID_SGMII_11:
    str = "SGMII-11";
  break;
  case SOC_TMC_NIF_ID_SGMII_12:
    str = "SGMII-12";
  break;
  case SOC_TMC_NIF_ID_SGMII_13:
    str = "SGMII-13";
  break;
  case SOC_TMC_NIF_ID_SGMII_14:
    str = "SGMII-14";
  break;
  case SOC_TMC_NIF_ID_SGMII_15:
    str = "SGMII-15";
  break;
  case SOC_TMC_NIF_ID_SGMII_16:
    str = "SGMII-16";
  break;
  case SOC_TMC_NIF_ID_SGMII_17:
    str = "SGMII-17";
  break;
  case SOC_TMC_NIF_ID_SGMII_18:
    str = "SGMII-18";
  break;
  case SOC_TMC_NIF_ID_SGMII_19:
    str = "SGMII-19";
  break;
  case SOC_TMC_NIF_ID_SGMII_20:
    str = "SGMII-20";
  break;
  case SOC_TMC_NIF_ID_SGMII_21:
    str = "SGMII-21";
  break;
  case SOC_TMC_NIF_ID_SGMII_22:
    str = "SGMII-22";
  break;
  case SOC_TMC_NIF_ID_SGMII_23:
    str = "SGMII-23";
  break;
  case SOC_TMC_NIF_ID_SGMII_24:
    str = "SGMII-24";
  break;
  case SOC_TMC_NIF_ID_SGMII_25:
    str = "SGMII-25";
  break;
  case SOC_TMC_NIF_ID_SGMII_26:
    str = "SGMII-26";
  break;
  case SOC_TMC_NIF_ID_SGMII_27:
    str = "SGMII-27";
  break;
  case SOC_TMC_NIF_ID_SGMII_28:
    str = "SGMII-28";
  break;
  case SOC_TMC_NIF_ID_SGMII_29:
    str = "SGMII-29";
  break;
  case SOC_TMC_NIF_ID_SGMII_30:
    str = "SGMII-30";
  break;
  case SOC_TMC_NIF_ID_SGMII_31:
    str = "SGMII-31";
  break;
  case SOC_TMC_NIF_ID_QSGMII_0:
    str = "QSGMII-0";
  break;
  case SOC_TMC_NIF_ID_QSGMII_1:
    str = "QSGMII-1";
  break;
  case SOC_TMC_NIF_ID_QSGMII_2:
    str = "QSGMII-2";
  break;
  case SOC_TMC_NIF_ID_QSGMII_3:
    str = "QSGMII-3";
  break;
  case SOC_TMC_NIF_ID_QSGMII_4:
    str = "QSGMII-4";
  break;
  case SOC_TMC_NIF_ID_QSGMII_5:
    str = "QSGMII-5";
  break;
  case SOC_TMC_NIF_ID_QSGMII_6:
    str = "QSGMII-6";
  break;
  case SOC_TMC_NIF_ID_QSGMII_7:
    str = "QSGMII-7";
  break;
  case SOC_TMC_NIF_ID_QSGMII_8:
    str = "QSGMII-8";
  break;
  case SOC_TMC_NIF_ID_QSGMII_9:
    str = "QSGMII-9";
  break;
  case SOC_TMC_NIF_ID_QSGMII_10:
    str = "QSGMII-10";
  break;
  case SOC_TMC_NIF_ID_QSGMII_11:
    str = "QSGMII-11";
  break;
  case SOC_TMC_NIF_ID_QSGMII_12:
    str = "QSGMII-12";
  break;
  case SOC_TMC_NIF_ID_QSGMII_13:
    str = "QSGMII-13";
  break;
  case SOC_TMC_NIF_ID_QSGMII_14:
    str = "QSGMII-14";
  break;
  case SOC_TMC_NIF_ID_QSGMII_15:
    str = "QSGMII-15";
  break;
  case SOC_TMC_NIF_ID_QSGMII_16:
    str = "QSGMII-16";
  break;
  case SOC_TMC_NIF_ID_QSGMII_17:
    str = "QSGMII-17";
  break;
  case SOC_TMC_NIF_ID_QSGMII_18:
    str = "QSGMII-18";
  break;
  case SOC_TMC_NIF_ID_QSGMII_19:
    str = "QSGMII-19";
  break;
  case SOC_TMC_NIF_ID_QSGMII_20:
    str = "QSGMII-20";
  break;
  case SOC_TMC_NIF_ID_QSGMII_21:
    str = "QSGMII-21";
  break;
  case SOC_TMC_NIF_ID_QSGMII_22:
    str = "QSGMII-22";
  break;
  case SOC_TMC_NIF_ID_QSGMII_23:
    str = "QSGMII-23";
  break;
  case SOC_TMC_NIF_ID_QSGMII_24:
    str = "QSGMII-24";
  break;
  case SOC_TMC_NIF_ID_QSGMII_25:
    str = "QSGMII-25";
  break;
  case SOC_TMC_NIF_ID_QSGMII_26:
    str = "QSGMII-26";
  break;
  case SOC_TMC_NIF_ID_QSGMII_27:
    str = "QSGMII-27";
  break;
  case SOC_TMC_NIF_ID_QSGMII_28:
    str = "QSGMII-28";
  break;
  case SOC_TMC_NIF_ID_QSGMII_29:
    str = "QSGMII-29";
  break;
  case SOC_TMC_NIF_ID_QSGMII_30:
    str = "QSGMII-30";
  break;
  case SOC_TMC_NIF_ID_QSGMII_31:
    str = "QSGMII-31";
  break;
  case SOC_TMC_NIF_ID_QSGMII_32:
    str = "QSGMII-32";
  break;
  case SOC_TMC_NIF_ID_QSGMII_33:
    str = "QSGMII-33";
  break;
  case SOC_TMC_NIF_ID_QSGMII_34:
    str = "QSGMII-34";
  break;
  case SOC_TMC_NIF_ID_QSGMII_35:
    str = "QSGMII-35";
  break;
  case SOC_TMC_NIF_ID_QSGMII_36:
    str = "QSGMII-36";
  break;
  case SOC_TMC_NIF_ID_QSGMII_37:
    str = "QSGMII-37";
  break;
  case SOC_TMC_NIF_ID_QSGMII_38:
    str = "QSGMII-38";
  break;
  case SOC_TMC_NIF_ID_QSGMII_39:
    str = "QSGMII-39";
  break;
  case SOC_TMC_NIF_ID_QSGMII_40:
    str = "QSGMII-40";
  break;
  case SOC_TMC_NIF_ID_QSGMII_41:
    str = "QSGMII-41";
  break;
  case SOC_TMC_NIF_ID_QSGMII_42:
    str = "QSGMII-42";
  break;
  case SOC_TMC_NIF_ID_QSGMII_43:
    str = "QSGMII-43";
  break;
  case SOC_TMC_NIF_ID_QSGMII_44:
    str = "QSGMII-44";
  break;
  case SOC_TMC_NIF_ID_QSGMII_45:
    str = "QSGMII-45";
  break;
  case SOC_TMC_NIF_ID_QSGMII_46:
    str = "QSGMII-46";
  break;
  case SOC_TMC_NIF_ID_QSGMII_47:
    str = "QSGMII-47";
  break;
  case SOC_TMC_NIF_ID_QSGMII_48:
    str = "QSGMII-48";
  break;
  case SOC_TMC_NIF_ID_QSGMII_49:
    str = "QSGMII-49";
  break;
  case SOC_TMC_NIF_ID_QSGMII_50:
    str = "QSGMII-50";
  break;
  case SOC_TMC_NIF_ID_QSGMII_51:
    str = "QSGMII-51";
  break;
  case SOC_TMC_NIF_ID_QSGMII_52:
    str = "QSGMII-52";
  break;
  case SOC_TMC_NIF_ID_QSGMII_53:
    str = "QSGMII-53";
  break;
  case SOC_TMC_NIF_ID_QSGMII_54:
    str = "QSGMII-54";
  break;
  case SOC_TMC_NIF_ID_QSGMII_55:
    str = "QSGMII-55";
  break;
  case SOC_TMC_NIF_ID_QSGMII_56:
    str = "QSGMII-56";
  break;
  case SOC_TMC_NIF_ID_QSGMII_57:
    str = "QSGMII-57";
  break;
  case SOC_TMC_NIF_ID_QSGMII_58:
    str = "QSGMII-58";
  break;
  case SOC_TMC_NIF_ID_QSGMII_59:
    str = "QSGMII-59";
  break;
  case SOC_TMC_NIF_ID_QSGMII_60:
    str = "QSGMII-60";
  break;
  case SOC_TMC_NIF_ID_QSGMII_61:
    str = "QSGMII-61";
  break;
  case SOC_TMC_NIF_ID_QSGMII_62:
    str = "QSGMII-62";
  break;
  case SOC_TMC_NIF_ID_QSGMII_63:
    str = "QSGMII-63";
  break;
  case SOC_TMC_NIF_ID_ILKN_0:
    str = "ILKN-0";
  break;
  case SOC_TMC_NIF_ID_ILKN_1:
    str = "ILKN-1";
  break;
  case SOC_TMC_NIF_ID_CGE_0:
	str = "CGE-0";
  break;
  case SOC_TMC_NIF_ID_CGE_1:
	str = "CGE-1";
  break;
  case SOC_TMC_IF_ID_CPU:
    str = "CPU";
  break;
  case SOC_TMC_IF_ID_OLP:
    str = "OLP";
  break;
  case SOC_TMC_IF_ID_RCY:
    str = "RCY";
  break;
  case SOC_TMC_IF_ID_ERP:
    str = "ERP";
  break;
  case SOC_TMC_NIF_ID_10GBASE_R_0:
      str = "10GBASE_R_0";
      break;
  case SOC_TMC_NIF_ID_10GBASE_R_1:
      str = "10GBASE_R_1";
      break;
  case SOC_TMC_NIF_ID_10GBASE_R_2:
      str = "10GBASE_R_2";
      break;
  case SOC_TMC_NIF_ID_10GBASE_R_3:
      str = "10GBASE_R_3";
      break;
  case SOC_TMC_NIF_ID_10GBASE_R_4:
      str = "10GBASE_R_4";
      break;
  case SOC_TMC_NIF_ID_10GBASE_R_5:
      str = "10GBASE_R_5";
      break;
  case SOC_TMC_NIF_ID_10GBASE_R_6:
      str = "10GBASE_R_6";
      break;
  case SOC_TMC_NIF_ID_10GBASE_R_7:
      str = "10GBASE_R_7";
      break;
  case SOC_TMC_NIF_ID_10GBASE_R_8:
      str = "10GBASE_R_8";
      break;
  case SOC_TMC_NIF_ID_10GBASE_R_9:
      str = "10GBASE_R_9";
      break;
  case SOC_TMC_NIF_ID_10GBASE_R_10:
      str = "10GBASE_R_10";
      break;
  case SOC_TMC_NIF_ID_10GBASE_R_11:
      str = "10GBASE_R_11";
      break;
  case SOC_TMC_NIF_ID_10GBASE_R_12:
      str = "10GBASE_R_12";
      break;
  case SOC_TMC_NIF_ID_10GBASE_R_13:
      str = "10GBASE_R_13";
      break;
  case SOC_TMC_NIF_ID_10GBASE_R_14:
      str = "10GBASE_R_14";
      break;
  case SOC_TMC_NIF_ID_10GBASE_R_15:
      str = "10GBASE_R_15";
      break;
  case SOC_TMC_NIF_ID_10GBASE_R_16:
      str = "10GBASE_R_16";
      break;
  case SOC_TMC_NIF_ID_10GBASE_R_17:
      str = "10GBASE_R_17";
      break;
  case SOC_TMC_NIF_ID_10GBASE_R_18:
      str = "10GBASE_R_18";
      break;
  case SOC_TMC_NIF_ID_10GBASE_R_19:
      str = "10GBASE_R_19";
      break;
  case SOC_TMC_NIF_ID_10GBASE_R_20:
      str = "10GBASE_R_20";
      break;
  case SOC_TMC_NIF_ID_10GBASE_R_21:
      str = "10GBASE_R_21";
      break;
  case SOC_TMC_NIF_ID_10GBASE_R_22:
      str = "10GBASE_R_22";
      break;
  case SOC_TMC_NIF_ID_10GBASE_R_23:
      str = "10GBASE_R_23";
      break;
  case SOC_TMC_NIF_ID_10GBASE_R_24:
      str = "10GBASE_R_24";
      break;
  case SOC_TMC_NIF_ID_10GBASE_R_25:
      str = "10GBASE_R_25";
      break;
  case SOC_TMC_NIF_ID_10GBASE_R_26:
      str = "10GBASE_R_26";
      break;
  case SOC_TMC_NIF_ID_10GBASE_R_27:
      str = "10GBASE_R_27";
      break;
  case SOC_TMC_NIF_ID_10GBASE_R_28:
      str = "10GBASE_R_28";
      break;
  case SOC_TMC_NIF_ID_10GBASE_R_29:
      str = "10GBASE_R_29";
      break;
  case SOC_TMC_NIF_ID_10GBASE_R_30:
      str = "10GBASE_R_30";
      break;
  case SOC_TMC_NIF_ID_10GBASE_R_31:
      str = "10GBASE_R_31";
      break;
  case SOC_TMC_NIF_ID_XLGE_0:
      str = "XLGE_0";
      break;
  case SOC_TMC_NIF_ID_XLGE_1:
      str = "XLGE_1";
      break;
  case SOC_TMC_NIF_ID_XLGE_2:
      str = "XLGE_2";
      break;
  case SOC_TMC_NIF_ID_XLGE_3:
      str = "XLGE_3";
      break;
  case SOC_TMC_NIF_ID_XLGE_4:
      str = "XLGE_4";
      break;
  case SOC_TMC_NIF_ID_XLGE_5:
      str = "XLGE_5";
      break;
  case SOC_TMC_NIF_ID_XLGE_6:
      str = "XLGE_6";
      break;
  case SOC_TMC_NIF_ID_XLGE_7:
      str = "XLGE_7";
      break;


  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_TMC_INTERFACE_ID_print(
    SOC_SAND_IN SOC_TMC_INTERFACE_ID if_ndx
  )
{
  LOG_CLI((BSL_META("Interface index: %s\n\r"),SOC_TMC_INTERFACE_ID_to_string(if_ndx)));
}

const char*
  SOC_TMC_FC_DIRECTION_to_string(
    SOC_SAND_IN SOC_TMC_FC_DIRECTION enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_FC_DIRECTION_GEN:
    str = "Generation";
  break;

  case SOC_TMC_FC_DIRECTION_REC:
    str = "Reception";
  break;

  case SOC_TMC_FC_NOF_DIRECTIONS:
    str = " Not initialized";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_COMBO_QRTT_to_string(
    SOC_SAND_IN  SOC_TMC_COMBO_QRTT enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_COMBO_QRTT_0:
    str = "combo_0 (STAR 0)";
  break;
  case SOC_TMC_COMBO_QRTT_1:
    str = "combo_1 (STAR 1)";
  break;
  case SOC_TMC_COMBO_NOF_QRTTS:
    str = " Not initialized";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_CONNECTION_DIRECTION_to_string(
    SOC_SAND_IN SOC_TMC_CONNECTION_DIRECTION enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_CONNECTION_DIRECTION_RX:
    str = "rx";
  break;
  case SOC_TMC_CONNECTION_DIRECTION_TX:
    str = "tx";
  break;
  case SOC_TMC_CONNECTION_DIRECTION_BOTH:
    str = "both";
  break;
  case SOC_TMC_NOF_CONNECTION_DIRECTIONS:
    str = " Not initialized";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_PKT_FRWRD_TYPE_to_string(
    SOC_SAND_IN  SOC_TMC_PKT_FRWRD_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_PKT_FRWRD_TYPE_BRIDGE:
    str = "ll_hdr_bridged";
  break;
  case SOC_TMC_PKT_FRWRD_TYPE_IPV4_UC:
    str = "ipv4_uc_routed";
  break;
  case SOC_TMC_PKT_FRWRD_TYPE_IPV4_MC:
    str = "ipv4_mc_routed";
  break;
  case SOC_TMC_PKT_FRWRD_TYPE_IPV6_UC:
    str = "ipv6_uc";
  break;
  case SOC_TMC_PKT_FRWRD_TYPE_IPV6_MC:
    str = "ipv6_mc";
  break;
  case SOC_TMC_PKT_FRWRD_TYPE_MPLS:
    str = "mpls";
  break;
  case SOC_TMC_PKT_FRWRD_TYPE_TRILL:
    str = "trill";
  break;
  case SOC_TMC_PKT_FRWRD_TYPE_BRIDGE_AFTER_TERM:
    str = "bridged_after_term";
  break;
  case SOC_TMC_PKT_FRWRD_TYPE_CPU_TRAP:
    str = "cpu_trap";
  break;
  case SOC_TMC_PKT_FRWRD_TYPE_TM:
    str = "tm";
    break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_TMC_TUNNEL_TERM_CODE_to_string(
    SOC_SAND_IN  SOC_TMC_TUNNEL_TERM_CODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_TUNNEL_TERM_CODE_NONE:
    str = "none";
  break;
  case SOC_TMC_TUNNEL_TERM_CODE_ETH:
    str = "eth";
  break;
  case SOC_TMC_TUNNEL_TERM_CODE_IPV4_ETH:
    str = "ipv4_eth";
  break;
  case SOC_TMC_TUNNEL_TERM_CODE_MPLS_ETH:
    str = "mpls_eth";
  break;
  case SOC_TMC_TUNNEL_TERM_CODE_MPLS_ETH_CW:
    str = "mpls_eth_cw";
  break;
  case SOC_TMC_TUNNEL_TERM_CODE_MPLS2_ETH:
    str = "mpls2_eth";
  break;
  case SOC_TMC_TUNNEL_TERM_CODE_MPLS2_ETH_CW:
    str = "mpls2_eth_cw";
  break;
  case SOC_TMC_TUNNEL_TERM_CODE_MPLS3_ETH:
    str = "mpls3_eth";
  break;
  case SOC_TMC_TUNNEL_TERM_CODE_MPLS3_ETH_CW:
    str = "mpls3_eth_cw";
  break;
  case SOC_TMC_TUNNEL_TERM_CODE_TRILL_ETH:
    str = "trill_eth";
  break;
  default:
    str = " Unknown";
  }
  return str;
}


void
  SOC_TMC_DEST_SYS_PORT_INFO_clear(
    SOC_SAND_OUT SOC_TMC_DEST_SYS_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_DEST_SYS_PORT_INFO));
  info->type = SOC_TMC_DEST_SYS_PORT_NOF_TYPES;
  info->id = 0;
  info->member_id = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_DEST_INFO_clear(
    SOC_SAND_OUT SOC_TMC_DEST_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_DEST_INFO));
  info->type = SOC_TMC_NOF_DEST_TYPES_ARAD; 
  info->id = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_THRESH_WITH_HYST_INFO_clear(
    SOC_SAND_OUT SOC_TMC_THRESH_WITH_HYST_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_THRESH_WITH_HYST_INFO));
  info->set = 0;
  info->clear = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_THRESH_WITH_FADT_HYST_INFO_clear(
    SOC_SAND_OUT SOC_TMC_THRESH_WITH_FADT_HYST_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_THRESH_WITH_FADT_HYST_INFO));
  info->set.max_threshold = 0;
  info->set.min_threshold = 0;
  info->set.alpha = 0;
  info->clear_offset = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_SWAP_INFO_clear(
    SOC_SAND_OUT SOC_TMC_SWAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_TMC_SWAP_INFO));
  info->mode = SOC_TMC_SWAP_MODE_4_BYTES;
  info->offset = 0;

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_DEST_TYPE_to_string(
    SOC_SAND_IN SOC_TMC_DEST_TYPE enum_val,
    SOC_SAND_IN uint8       short_name
  )
{
  const char* str = NULL;
  if(short_name)
  {
    switch(enum_val)
    {
    case SOC_TMC_DEST_TYPE_QUEUE:
      str = "QUEUE";
      break;

    case SOC_TMC_DEST_TYPE_MULTICAST:
      str = "MULT";
      break;

    case SOC_TMC_DEST_TYPE_SYS_PHY_PORT:
      str = "PORT";
      break;

    case SOC_TMC_DEST_TYPE_LAG:
      str = "lag";
      break;

    case SOC_TMC_NOF_DEST_TYPES_ARAD:
      str = "INVALID";
      break;

    default:
      str = " Unknown";
    }
  }
  else
  {
    switch(enum_val)
    {
    case SOC_TMC_DEST_TYPE_QUEUE:
      str = "QUEUE";
      break;

    case SOC_TMC_DEST_TYPE_MULTICAST:
      str = "MULTICAST";
      break;

    case SOC_TMC_DEST_TYPE_SYS_PHY_PORT:
      str = "SYS_PHY_PORT";
      break;

    case SOC_TMC_DEST_TYPE_LAG:
      str = "LAG";
      break;

    case SOC_TMC_NOF_DEST_TYPES_ARAD:
      str = "Not initialized";
      break;

    default:
      str = " Unknown";
    }
  }
  return str;
}

const char*
  SOC_TMC_DEST_SYS_PORT_TYPE_to_string(
    SOC_SAND_IN SOC_TMC_DEST_SYS_PORT_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_DEST_SYS_PORT_TYPE_SYS_PHY_PORT:
    str = "SYS_PHY_PORT";
  break;

  case SOC_TMC_DEST_SYS_PORT_TYPE_LAG:
    str = "LAG";
  break;

  case SOC_TMC_DEST_SYS_PORT_NOF_TYPES:
    str = "Not initialized";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_TMC_DEST_SYS_PORT_INFO_print(
    SOC_SAND_IN SOC_TMC_DEST_SYS_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Id(type), Member-Id: %u(%s), %u.\n\r"),
           info->id,
           SOC_TMC_DEST_SYS_PORT_TYPE_to_string(info->type),
           info->member_id
           ));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_DEST_INFO_print(
    SOC_SAND_IN SOC_TMC_DEST_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Type %s, "),
           SOC_TMC_DEST_TYPE_to_string(info->type, TRUE)
           ));
  LOG_CLI((BSL_META_U(unit,
                      "Id: %u.\n\r"),info->id));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_THRESH_WITH_HYST_INFO_print(
    SOC_SAND_IN SOC_TMC_THRESH_WITH_HYST_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Set: %u, "),info->set));
  LOG_CLI((BSL_META_U(unit,
                      "Clear: %u.\n\r"),info->clear));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_TMC_DEST_SYS_PORT_INFO_table_format_print(
    SOC_SAND_IN SOC_TMC_DEST_SYS_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "%-12s|"),
           SOC_TMC_DEST_SYS_PORT_TYPE_to_string(info->type)
           ));
  LOG_CLI((BSL_META_U(unit,
                      "%-5d|"),info->id));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

const char*
  SOC_TMC_SWAP_MODE_to_string(
    SOC_SAND_IN  SOC_TMC_SWAP_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_TMC_SWAP_MODE_4_BYTES:
    str = "4_BYTES";
    break;
  case SOC_TMC_SWAP_MODE_8_BYTES:
    str = "8_BYTES";
    break;
  default:
    str = "Unknown";
  }
  return str;
}

void
  SOC_TMC_SWAP_INFO_print(
    SOC_SAND_IN SOC_TMC_SWAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Mode: %s, "), SOC_TMC_SWAP_MODE_to_string(info->mode)));
  LOG_CLI((BSL_META_U(unit,
                      "Offset: %u.\n\r"),info->offset));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>
