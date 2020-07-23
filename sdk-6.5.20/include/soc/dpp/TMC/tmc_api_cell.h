/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_TMC_API_CELL_INCLUDED__

#define __SOC_TMC_API_CELL_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/TMC/tmc_api_general.h>
#include <soc/dpp/SAND/SAND_FM/sand_cell.h>





#define SOC_TMC_CELL_NOF_LINKS_IN_PATH_LINKS            (4)

#define SOC_TMC_CELL_MC_NOF_LINKS                       (2)
#define SOC_TMC_CELL_MC_DATA_IN_UINT32S                   (3)
#define SOC_TMC_CELL_MC_NOF_CHANGES                     (7)











typedef enum
{
  
  SOC_TMC_CELL_FE_LOCATION_FE1 = 0,
  
  SOC_TMC_CELL_FE_LOCATION_FE2 = 1,
  
  SOC_TMC_CELL_NOF_FE_LOCATIONS = 2
}SOC_TMC_CELL_FE_LOCATION;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_SAND_DEVICE_ENTITY  dest_entity_type;
  
  uint8            path_links[SOC_TMC_CELL_NOF_LINKS_IN_PATH_LINKS];

} SOC_TMC_SR_CELL_LINK_LIST;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 data[SOC_TMC_CELL_MC_DATA_IN_UINT32S];

} SOC_TMC_CELL_MC_TBL_DATA;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_TMC_CELL_FE_LOCATION fe_location;
  
  uint32 path_links[SOC_TMC_CELL_MC_NOF_LINKS];
  
  uint8 filter[SOC_TMC_CELL_MC_NOF_CHANGES];

} SOC_TMC_CELL_MC_TBL_INFO;











void
  SOC_TMC_SR_CELL_LINK_LIST_clear(
    SOC_SAND_OUT SOC_TMC_SR_CELL_LINK_LIST *info
  );

void
  SOC_TMC_CELL_MC_TBL_DATA_clear(
    SOC_SAND_OUT SOC_TMC_CELL_MC_TBL_DATA *info
  );

void
  SOC_TMC_CELL_MC_TBL_INFO_clear(
    SOC_SAND_OUT SOC_TMC_CELL_MC_TBL_INFO *info
  );

#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_CELL_FE_LOCATION_to_string(
    SOC_SAND_IN  SOC_TMC_CELL_FE_LOCATION enum_val
  );

void
  SOC_TMC_SR_CELL_LINK_LIST_print(
    SOC_SAND_IN  SOC_TMC_SR_CELL_LINK_LIST *info
  );

void
  SOC_TMC_CELL_MC_TBL_DATA_print(
    SOC_SAND_IN  SOC_TMC_CELL_MC_TBL_DATA *info
  );

void
  SOC_TMC_CELL_MC_TBL_INFO_print(
    SOC_SAND_IN  SOC_TMC_CELL_MC_TBL_INFO *info
  );

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
