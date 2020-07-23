/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/



#include <soc/dpp/SAND/Utils/sand_header.h>



#include <soc/dpp/SAND/SAND_FM/sand_chip_defines.h>


























char *
  soc_sand_entity_enum_to_str(
    SOC_SAND_IN SOC_SAND_DEVICE_ENTITY entity_type
  )
{
  char
    *str = "";

  switch(entity_type)
  {
   case(SOC_SAND_FE1_ENTITY):
      str = "FE1";
      break;
    case(SOC_SAND_FE2_ENTITY):
      str = "FE2";
      break;
    case(SOC_SAND_FE3_ENTITY):
      str = "FE3";
      break;
    case(SOC_SAND_FAP_ENTITY):
      str = "FAP";
      break;
    case(SOC_SAND_FOP_ENTITY):
      str = "FOP";
      break;
    case(SOC_SAND_FIP_ENTITY):
      str = "FIP";
      break;
    case(SOC_SAND_FE13_ENTITY):
      str = "FE13";
      break;
    case(SOC_SAND_DONT_CARE_ENTITY):
    default:
      str = "???";
      break;
  }

  return str;
}


SOC_SAND_DEVICE_ENTITY
  soc_sand_entity_from_level(
    SOC_SAND_IN SOC_SAND_ENTITY_LEVEL_TYPE level_val
  )
{
  SOC_SAND_DEVICE_ENTITY
    res = SOC_SAND_DONT_CARE_ENTITY;

  switch(level_val)
  {
    case(SOC_SAND_ACTUAL_FAP_VALUE_1):
    case(SOC_SAND_ACTUAL_FIP_VALUE):
    case(SOC_SAND_ACTUAL_FOP_VALUE):
    case(SOC_SAND_ACTUAL_FAP_VALUE):
      res = SOC_SAND_FAP_ENTITY;
      break;
    case(SOC_SAND_ACTUAL_FE1_VALUE):
      res = SOC_SAND_FE1_ENTITY;
      break;
    case(SOC_SAND_ACTUAL_FE2_VALUE):
    case(SOC_SAND_ACTUAL_FE2_VALUE_1):
      res = SOC_SAND_FE2_ENTITY;
      break;
    case(SOC_SAND_ACTUAL_FE3_VALUE):
      res = SOC_SAND_FE3_ENTITY;
      break;
    default:
      res = SOC_SAND_DONT_CARE_ENTITY;
      break;
  }

  return res;
}



#include <soc/dpp/SAND/Utils/sand_footer.h>
