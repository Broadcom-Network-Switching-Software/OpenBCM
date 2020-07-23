/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_TDM_INCLUDED__

#define __ARAD_TDM_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/arad_api_tdm.h>






#define ARAD_NOF_TDM_CONTEXT_MAP (2)

#define ARAD_TDM_VERSION_ID                                      (0x2)
#define ARAD_TDM_CELL_SIZE_MIN                                   (65)
#define ARAD_TDM_CELL_SIZE_MAX                                   (254)








typedef enum
{
  
  ARAD_TDM_FTMH_INFO_MODE_OPT_UC = 0,
  
  ARAD_TDM_FTMH_INFO_MODE_OPT_MC = 1,
  
  ARAD_TDM_FTMH_INFO_MODE_STANDARD_UC = 2,
  
  ARAD_TDM_FTMH_INFO_MODE_STANDARD_MC = 3,
  
  ARAD_TDM_NOF_FTMH_INFO_MODE = 4
}ARAD_TDM_FTMH_INFO_MODE;

typedef struct
{
 
  ARAD_INTERFACE_ID
    context_map[ARAD_NOF_TDM_CONTEXT_MAP];  
    int tdm_context_ref_count[ARAD_NOF_TDM_CONTEXT_MAP];
} ARAD_TDM;










int
  arad_tdm_local_to_reassembly_context_get(
    int unit,
    uint32 port,
    uint32 *reassembly_context
  );




uint32 
  arad_tdm_unit_has_tdm(
    SOC_SAND_IN int unit,
    SOC_SAND_OUT uint32 *tdm_found
  );


uint32
  arad_tdm_init(
    SOC_SAND_IN  int  unit
  );


uint32
  arad_tdm_ftmh_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core_id,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  ARAD_TDM_FTMH_INFO            *info
  );

uint32
  arad_tdm_ftmh_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  ARAD_TDM_FTMH_INFO            *info
  );

uint32
  arad_tdm_ftmh_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx
  );


uint32
  arad_tdm_ftmh_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core_id,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_OUT ARAD_TDM_FTMH_INFO            *info
  );


uint32
  arad_tdm_opt_size_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                    cell_size
  );

uint32
  arad_tdm_opt_size_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                    cell_size
  );

uint32
  arad_tdm_opt_size_get_verify(
    SOC_SAND_IN  int                   unit
  );


uint32
  arad_tdm_opt_size_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT uint32                    *cell_size
  );


uint32
  arad_tdm_stand_size_range_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_SAND_U32_RANGE              *size_range
  );

uint32
  arad_tdm_stand_size_range_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_SAND_U32_RANGE              *size_range
  );

uint32
  arad_tdm_stand_size_range_get_verify(
    SOC_SAND_IN  int                   unit
  );


uint32
  arad_tdm_stand_size_range_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT SOC_SAND_U32_RANGE              *size_range
  );


int
  arad_tdm_ofp_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  soc_port_t            port,
    SOC_SAND_IN  uint8                 is_tdm
  );

uint32
  arad_tdm_ofp_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  uint8                   is_tdm
  );

uint32
  arad_tdm_ofp_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx
  );


uint32
  arad_tdm_ofp_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_OUT uint8                   *is_tdm
  );


int
  arad_tdm_ifp_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  soc_port_t            port,
    SOC_SAND_IN  uint8                 is_tdm
  );

uint32
  arad_tdm_ifp_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  uint8                   is_tdm
  );

uint32
  arad_tdm_ifp_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx
  );


int
  arad_tdm_ifp_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  soc_port_t      port,
    SOC_SAND_OUT uint8           *is_tdm
  );


uint32
  arad_tdm_direct_routing_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   direct_routing_profile,
    SOC_SAND_IN  ARAD_TDM_DIRECT_ROUTING_INFO *direct_routing_info,
    SOC_SAND_IN  uint8 enable_rpt_reachable
  );

uint32
  arad_tdm_direct_routing_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   direct_routing_profile,
    SOC_SAND_IN  ARAD_TDM_DIRECT_ROUTING_INFO *direct_routing_info
  );

uint32
  arad_tdm_direct_routing_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   direct_routing_profile
  );


uint32
  arad_tdm_direct_routing_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   direct_routing_profile,
    SOC_SAND_OUT ARAD_TDM_DIRECT_ROUTING_INFO *direct_routing_info,
    SOC_SAND_OUT uint8 *enable
  );


uint32
  arad_tdm_direct_routing_profile_map_set_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     port_ndx,
    SOC_SAND_IN  uint32                     direct_routing_profile 
  );

uint32
  arad_tdm_direct_routing_profile_map_set_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     port_ndx,
    SOC_SAND_IN  uint32                     direct_routing_profile 
  );

uint32
  arad_tdm_direct_routing_profile_map_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx
  );


uint32
  arad_tdm_direct_routing_profile_map_get_unsafe(
    SOC_SAND_IN   int                     unit,
    SOC_SAND_IN   uint32                     port_ndx,
    SOC_SAND_OUT  uint32                    *direct_routing_profile
  );


uint32
  arad_tdm_port_packet_crc_set_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  soc_port_t                 port_ndx,
    SOC_SAND_IN  uint8                      is_enable,         
    SOC_SAND_IN  uint8                      configure_ingress, 
    SOC_SAND_IN  uint8                      configure_egress   
  );

uint32
  arad_tdm_port_packet_crc_set_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     port_ndx,
    SOC_SAND_IN  uint8                     is_enable 
  );

uint32
  arad_tdm_port_packet_crc_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx
  );


uint32
  arad_tdm_port_packet_crc_get_unsafe(
    SOC_SAND_IN   int                     unit,
    SOC_SAND_IN   soc_port_t                 port_ndx,
    SOC_SAND_OUT  uint8                      *is_ingress_enabled,
    SOC_SAND_OUT  uint8                      *is_egress_enabled
  );

#if ARAD_DEBUG_IS_LVL1

uint32
  ARAD_TDM_FTMH_OPT_UC_verify(
    SOC_SAND_IN  ARAD_TDM_FTMH_OPT_UC *info
  );

uint32
  ARAD_TDM_FTMH_OPT_MC_verify(
    SOC_SAND_IN  ARAD_TDM_FTMH_OPT_MC *info
  );

uint32
  ARAD_TDM_FTMH_STANDARD_UC_verify(
    SOC_SAND_IN  ARAD_TDM_FTMH_STANDARD_UC *info
  );

uint32
  ARAD_TDM_FTMH_STANDARD_MC_verify(
    SOC_SAND_IN  ARAD_TDM_FTMH_STANDARD_MC *info
  );

uint32
  ARAD_TDM_FTMH_INFO_verify(
    SOC_SAND_IN  ARAD_TDM_FTMH_INFO *info
  );
uint32
  ARAD_TDM_MC_STATIC_ROUTE_INFO_verify(
    SOC_SAND_IN  ARAD_TDM_MC_STATIC_ROUTE_INFO *info
  );
uint32
  ARAD_TDM_DIRECT_ROUTING_INFO_verify(
    SOC_SAND_IN  uint32 unit,
    SOC_SAND_IN  ARAD_TDM_DIRECT_ROUTING_INFO *info
  );
#endif 


#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

