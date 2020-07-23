/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPC_API_FRWRD_FCF_INCLUDED__

#define __SOC_PPC_API_FRWRD_FCF_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_general.h>




#define SOC_PPC_FRWRD_FCF_ZONING_ACT_FRWRD      0x1

#define SOC_PPC_FRWRD_FCF_ZONING_ACT_REDIRECT   0x2



#define SOC_PPC_FRWRD_FCF_ROUTE_LOCAL_DOMAIN    0x1

#define SOC_PPC_FRWRD_FCF_ROUTE_DOMAIN          0x2

#define SOC_PPC_FRWRD_FCF_ROUTE_HOST            0x4

#define SOC_PPC_FRWRD_FCF_ROUTE_HOST_NPORT      0x8

#define SOC_PPC_FRWRD_FCF_ROUTE_DOMAIN_NPORT    0x10





#define SOC_PPC_FRWRD_FCF_ZONE_REMOVE_BY_VFT    0x1

#define SOC_PPC_FRWRD_FCF_ZONE_REMOVE_BY_D_ID   0x2

#define SOC_PPC_FRWRD_FCF_ZONE_REMOVE_BY_S_ID   0x4




typedef uint32 SOC_PPC_FRWRD_FCF_VFI;










typedef enum
{
  
  SOC_PPC_FRWRD_FCF_ROUTE_STATUS_COMMITED = 1,
  
  SOC_PPC_FRWRD_FCF_ROUTE_STATUS_PEND_ADD = 2,
  
  SOC_PPC_FRWRD_FCF_ROUTE_STATUS_PEND_REMOVE = 4,
  
  SOC_PPC_FRWRD_FCF_ROUTE_STATUS_ACCESSED = 8,
  
  SOC_PPC_NOF_FRWRD_FCF_ROUTE_STATUSS = 4
}SOC_PPC_FRWRD_FCF_ROUTE_STATUS;



typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  
  uint8 enable;
} SOC_PPC_FRWRD_FCF_GLBL_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 flags;
  
  SOC_PPC_FRWRD_FCF_VFI vfi;
  
  uint32 d_id;
  
  uint8 prefix_len;


} SOC_PPC_FRWRD_FCF_ROUTE_KEY;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_FRWRD_DECISION_INFO frwrd_decision;
} SOC_PPC_FRWRD_FCF_ROUTE_INFO;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  SOC_PPC_FRWRD_FCF_VFI vfi;
  
  uint32 d_id;
  
  uint32 s_id;

} SOC_PPC_FRWRD_FCF_ZONING_KEY;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 flags;
  
  SOC_PPC_FRWRD_DECISION_INFO frwrd_decision;

} SOC_PPC_FRWRD_FCF_ZONING_INFO;











void
  SOC_PPC_FRWRD_FCF_GLBL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_FCF_GLBL_INFO *info
  );

void
  SOC_PPC_FRWRD_FCF_ROUTE_KEY_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ROUTE_KEY *info
  );
void
  SOC_PPC_FRWRD_FCF_ROUTE_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ROUTE_INFO *info
  );


void
  SOC_PPC_FRWRD_FCF_ZONING_KEY_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ZONING_KEY *info
  );

void
  SOC_PPC_FRWRD_FCF_ZONING_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_FCF_ZONING_INFO *info
  );


#if SOC_PPC_DEBUG_IS_LVL1


const char*
  SOC_PPC_FRWRD_FCF_ROUTE_STATUS_to_string(
    SOC_SAND_IN  SOC_PPC_FRWRD_FCF_ROUTE_STATUS enum_val
  );

void
  SOC_PPC_FRWRD_FCF_GLBL_INFO_print(
    SOC_SAND_IN SOC_PPC_FRWRD_FCF_GLBL_INFO *info
  );

void
  SOC_PPC_FRWRD_FCF_ROUTE_KEY_print(
    SOC_SAND_IN SOC_PPC_FRWRD_FCF_ROUTE_KEY *info
  );

void
  SOC_PPC_FRWRD_FCF_ROUTE_INFO_print(
    SOC_SAND_IN SOC_PPC_FRWRD_FCF_ROUTE_INFO *info
  );


void
  SOC_PPC_FRWRD_FCF_ZONING_KEY_print(
    SOC_SAND_IN SOC_PPC_FRWRD_FCF_ZONING_KEY *info
  );

void
  SOC_PPC_FRWRD_FCF_ZONING_INFO_print(
    SOC_SAND_IN SOC_PPC_FRWRD_FCF_ZONING_INFO *info
  );


#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

