/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_PP_LLP_FILTER_INCLUDED__

#define __ARAD_PP_LLP_FILTER_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>

#include <soc/dpp/PPC/ppc_api_llp_filter.h>






#define ARAD_PP_LLP_FILTER_DESIGNATED_VLAN_TABLE_SIZE                (8)









typedef enum
{
  
  ARAD_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_SET = ARAD_PP_PROC_DESC_BASE_LLP_FILTER_FIRST,
  ARAD_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_SET_PRINT,
  ARAD_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_SET_UNSAFE,
  ARAD_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_SET_VERIFY,
  ARAD_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_GET,
  ARAD_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_GET_PRINT,
  ARAD_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_GET_VERIFY,
  ARAD_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_GET_UNSAFE,
  ARAD_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_ADD,
  ARAD_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_ADD_PRINT,
  ARAD_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_ADD_UNSAFE,
  ARAD_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_ADD_VERIFY,
  ARAD_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_REMOVE,
  ARAD_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_REMOVE_PRINT,
  ARAD_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_REMOVE_UNSAFE,
  ARAD_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_REMOVE_VERIFY,
  ARAD_PP_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_SET,
  ARAD_PP_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_SET_PRINT,
  ARAD_PP_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_SET_UNSAFE,
  ARAD_PP_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_SET_VERIFY,
  ARAD_PP_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_GET,
  ARAD_PP_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_GET_PRINT,
  ARAD_PP_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_GET_VERIFY,
  ARAD_PP_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_GET_UNSAFE,
  ARAD_PP_LLP_FILTER_DESIGNATED_VLAN_SET,
  ARAD_PP_LLP_FILTER_DESIGNATED_VLAN_SET_PRINT,
  ARAD_PP_LLP_FILTER_DESIGNATED_VLAN_SET_UNSAFE,
  ARAD_PP_LLP_FILTER_DESIGNATED_VLAN_SET_VERIFY,
  ARAD_PP_LLP_FILTER_DESIGNATED_VLAN_GET,
  ARAD_PP_LLP_FILTER_DESIGNATED_VLAN_GET_PRINT,
  ARAD_PP_LLP_FILTER_DESIGNATED_VLAN_GET_VERIFY,
  ARAD_PP_LLP_FILTER_DESIGNATED_VLAN_GET_UNSAFE,
  ARAD_PP_LLP_FILTER_GET_PROCS_PTR,
  ARAD_PP_LLP_FILTER_GET_ERRS_PTR,
  

  
  ARAD_PP_LLP_FILTER_PROCEDURE_DESC_LAST
} ARAD_PP_LLP_FILTER_PROCEDURE_DESC;

typedef enum
{
  
  ARAD_PP_LLP_FILTER_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR = ARAD_PP_ERR_DESC_BASE_LLP_FILTER_FIRST,
  ARAD_PP_LLP_FILTER_VLAN_FORMAT_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_FILTER_SUCCESS_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_FILTER_ACCEPT_OUT_OF_RANGE_ERR,
  

  SOC_PPC_PORTS_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_UNEXPECTED_TAG_FORMAT_VLAN_FORMAT_ERR,
  ARAD_PP_FILTER_VID_NOT_DESIGNATED_ERR,

  
  ARAD_PP_LLP_FILTER_ERR_LAST
} ARAD_PP_LLP_FILTER_ERR;

typedef struct
{
  uint32 arr[ARAD_PP_LLP_FILTER_DESIGNATED_VLAN_TABLE_SIZE];
} ARAD_PP_LLP_FILTER_DESIGNATED_VLAN_TABLE_REF_COUNT;

typedef struct ARAD_PP_LLP_FILTER_s
{
  ARAD_PP_LLP_FILTER_DESIGNATED_VLAN_TABLE_REF_COUNT ref_count;
} ARAD_PP_LLP_FILTER_t;








uint32
  arad_pp_llp_filter_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_llp_filter_ingress_vlan_membership_set_unsafe(
    SOC_SAND_IN  int                                 unit,
	SOC_SAND_IN   int                                core_id,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_IN  uint32                                  ports[SOC_PPC_VLAN_MEMBERSHIP_BITMAP_SIZE]
  );

uint32
  arad_pp_llp_filter_ingress_vlan_membership_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_IN  uint32                                  ports[SOC_PPC_VLAN_MEMBERSHIP_BITMAP_SIZE]
  );

uint32
  arad_pp_llp_filter_ingress_vlan_membership_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                             vid_ndx
  );


uint32
  arad_pp_llp_filter_ingress_vlan_membership_get_unsafe(
    SOC_SAND_IN  int                                 unit,
	SOC_SAND_IN   int                                core_id,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_OUT uint32                                  ports[SOC_PPC_VLAN_MEMBERSHIP_BITMAP_SIZE]
  );


uint32
  arad_pp_llp_filter_ingress_vlan_membership_port_add_unsafe(
    SOC_SAND_IN  int                                 unit,
	SOC_SAND_IN   int                                core_id,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port
  );

uint32
  arad_pp_llp_filter_ingress_vlan_membership_port_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port
  );


uint32
  arad_pp_llp_filter_ingress_vlan_membership_port_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
	SOC_SAND_IN   int                                core_id,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port
  );

uint32
  arad_pp_llp_filter_ingress_vlan_membership_port_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port
  );


uint32
  arad_pp_llp_filter_ingress_acceptable_frames_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT        vlan_format_ndx,
    SOC_SAND_IN  SOC_PPC_ACTION_PROFILE                      *action_profile,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  arad_pp_llp_filter_ingress_acceptable_frames_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT        vlan_format_ndx,
    SOC_SAND_IN  SOC_PPC_ACTION_PROFILE                      *action_profile
  );

uint32
  arad_pp_llp_filter_ingress_acceptable_frames_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT        vlan_format_ndx
  );


uint32
  arad_pp_llp_filter_ingress_acceptable_frames_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT        vlan_format_ndx,
    SOC_SAND_OUT SOC_PPC_ACTION_PROFILE                      *action_profile
  );


uint32
  arad_pp_llp_filter_designated_vlan_set_unsafe(
    SOC_SAND_IN  int                                 unit,
	SOC_SAND_IN   int                                core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid,
    SOC_SAND_IN  uint8                                 accept,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  arad_pp_llp_filter_designated_vlan_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid,
    SOC_SAND_IN  uint8                                 accept
  );

uint32
  arad_pp_llp_filter_designated_vlan_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx
  );


uint32
  arad_pp_llp_filter_designated_vlan_get_unsafe(
    SOC_SAND_IN  int                                 unit,
	SOC_SAND_IN   int                                core_id,
    SOC_SAND_IN  SOC_PPC_PORT                                local_port_ndx,
    SOC_SAND_OUT SOC_SAND_PP_VLAN_ID                           *vid,
    SOC_SAND_OUT uint8                                 *accept
  );


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_llp_filter_get_procs_ptr(void);


CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_llp_filter_get_errs_ptr(void);



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

