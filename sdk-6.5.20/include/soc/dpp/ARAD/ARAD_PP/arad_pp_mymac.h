/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_PP_MYMAC_INCLUDED__

#define __ARAD_PP_MYMAC_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>

#include <soc/dpp/PPC/ppc_api_mymac.h>














typedef enum
{
  
  ARAD_PP_MYMAC_MSB_SET = ARAD_PP_PROC_DESC_BASE_MYMAC_FIRST,
  ARAD_PP_MYMAC_MSB_SET_PRINT,
  ARAD_PP_MYMAC_MSB_SET_UNSAFE,
  ARAD_PP_MYMAC_MSB_SET_VERIFY,
  ARAD_PP_MYMAC_MSB_GET,
  ARAD_PP_MYMAC_MSB_GET_PRINT,
  ARAD_PP_MYMAC_MSB_GET_VERIFY,
  ARAD_PP_MYMAC_MSB_GET_UNSAFE,
  ARAD_PP_MYMAC_VSI_LSB_SET,
  ARAD_PP_MYMAC_VSI_LSB_SET_PRINT,
  ARAD_PP_MYMAC_VSI_LSB_SET_UNSAFE,
  ARAD_PP_MYMAC_VSI_LSB_SET_VERIFY,
  ARAD_PP_MYMAC_VSI_LSB_GET,
  ARAD_PP_MYMAC_VSI_LSB_GET_PRINT,
  ARAD_PP_MYMAC_VSI_LSB_GET_VERIFY,
  ARAD_PP_MYMAC_VSI_LSB_GET_UNSAFE,
  SOC_PPC_MYMAC_VRRP_INFO_SET,
  SOC_PPC_MYMAC_VRRP_INFO_SET_PRINT,
  SOC_PPC_MYMAC_VRRP_INFO_SET_UNSAFE,
  SOC_PPC_MYMAC_VRRP_INFO_SET_VERIFY,
  SOC_PPC_MYMAC_VRRP_INFO_GET,
  SOC_PPC_MYMAC_VRRP_INFO_GET_PRINT,
  SOC_PPC_MYMAC_VRRP_INFO_GET_VERIFY,
  SOC_PPC_MYMAC_VRRP_INFO_GET_UNSAFE,
  ARAD_PP_MYMAC_VRRP_MAC_SET,
  ARAD_PP_MYMAC_VRRP_MAC_SET_PRINT,
  ARAD_PP_MYMAC_VRRP_MAC_SET_UNSAFE,
  ARAD_PP_MYMAC_VRRP_MAC_SET_VERIFY,
  ARAD_PP_MYMAC_VRRP_MAC_GET,
  ARAD_PP_MYMAC_VRRP_MAC_GET_PRINT,
  ARAD_PP_MYMAC_VRRP_MAC_GET_VERIFY,
  ARAD_PP_MYMAC_VRRP_MAC_GET_UNSAFE,
  SOC_PPC_MYMAC_TRILL_INFO_SET,
  SOC_PPC_MYMAC_TRILL_INFO_SET_PRINT,
  SOC_PPC_MYMAC_TRILL_INFO_SET_UNSAFE,
  SOC_PPC_MYMAC_TRILL_INFO_SET_VERIFY,
  SOC_PPC_MYMAC_TRILL_INFO_GET,
  SOC_PPC_MYMAC_TRILL_INFO_GET_PRINT,
  SOC_PPC_MYMAC_TRILL_INFO_GET_VERIFY,
  SOC_PPC_MYMAC_TRILL_INFO_GET_UNSAFE,
  ARAD_PP_LIF_MY_BMAC_MSB_SET,
  ARAD_PP_LIF_MY_BMAC_MSB_SET_PRINT,
  ARAD_PP_LIF_MY_BMAC_MSB_SET_UNSAFE,
  ARAD_PP_LIF_MY_BMAC_MSB_SET_VERIFY,
  ARAD_PP_LIF_MY_BMAC_MSB_GET,
  ARAD_PP_LIF_MY_BMAC_MSB_GET_PRINT,
  ARAD_PP_LIF_MY_BMAC_MSB_GET_VERIFY,
  ARAD_PP_LIF_MY_BMAC_MSB_GET_UNSAFE,
  ARAD_PP_LIF_MY_BMAC_PORT_LSB_SET,
  ARAD_PP_LIF_MY_BMAC_PORT_LSB_SET_PRINT,
  ARAD_PP_LIF_MY_BMAC_PORT_LSB_SET_UNSAFE,
  ARAD_PP_LIF_MY_BMAC_PORT_LSB_SET_VERIFY,
  ARAD_PP_LIF_MY_BMAC_PORT_LSB_GET,
  ARAD_PP_LIF_MY_BMAC_PORT_LSB_GET_PRINT,
  ARAD_PP_LIF_MY_BMAC_PORT_LSB_GET_VERIFY,
  ARAD_PP_LIF_MY_BMAC_PORT_LSB_GET_UNSAFE,
  ARAD_PP_MYMAC_GET_PROCS_PTR,
  ARAD_PP_MYMAC_GET_ERRS_PTR,
  

  
  ARAD_PP_MYMAC_PROCEDURE_DESC_LAST
} ARAD_PP_MYMAC_PROCEDURE_DESC;

typedef enum
{
  
  ARAD_PP_MYMAC_VRRP_ID_NDX_OUT_OF_RANGE_ERR = ARAD_PP_ERR_DESC_BASE_MYMAC_FIRST,
  ARAD_PP_MYMAC_MODE_OUT_OF_RANGE_ERR,
  ARAD_PP_MYMAC_MY_NICK_NAME_OUT_OF_RANGE_ERR,
  

  ARAD_PP_MYMAC_VSI_LSB_OUT_OF_RANGE,
  ARAD_PP_MYMAC_VRRP_IS_DISABLED_ERR,
  ARAD_PP_MYMAC_LOCAL_PORT_OUT_OF_RANGE_ERR,
  ARAD_PP_MYMAC_VSI_OUT_OF_RANGE_ERR,
  ARAD_PP_MY_BMAC_MSB_LOW_BITS_NOT_ZERO_ERR,
  ARAD_PP_MYMAC_SYS_PORT_NON_SINGLE_PORT_NOT_SUPPORTED_ERR,
  ARAD_PP_MYMAC_SYS_PORT_OUT_OF_RANGE,
  ARAD_PP_LIF_MY_BMAC_PORT_LSB_AUX_TABLE_MODE_MISMATCH_ERR,
  ARAD_PP_MYMAC_MODE_IPV6_NOT_SUPPORTED_ERR,
  ARAD_PP_MYMAC_VRRP_ID_NDX_ABOVE_4K_ERR,
  ARAD_PP_MYMAC_VRRP_ID_NDX_ABOVE_2K_ERR,
  ARAD_PP_MYMAC_VRRP_ID_NDX_ABOVE_1K_ERR,
  ARAD_PP_MYMAC_VRRP_ID_NDX_ABOVE_512_ERR,
  ARAD_PP_MYMAC_VRRP_ID_NDX_ABOVE_256_ERR,
  ARAD_PP_MYMAC_VRID_MAP_FULL_ERR,
  ARAD_PP_MYMAC_VRID_MAP_ENTRY_NOT_FOUND_ERR,
  ARAD_PP_MYMAC_CAM_CORRUPTED,
  ARAD_PP_MYMAC_CAM_NOT_FOUND,
  ARAD_PP_MYMAC_OUT_OF_RANGE,

  
  ARAD_PP_MYMAC_ERR_LAST
} ARAD_PP_MYMAC_ERR;









uint32
  arad_pp_mymac_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_mymac_msb_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *my_mac_msb
  );

uint32
  arad_pp_mymac_msb_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *my_mac_msb
  );

uint32
  arad_pp_mymac_msb_get_verify(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_mymac_msb_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS                       *my_mac_msb
  );


uint32
  arad_pp_mymac_vsi_lsb_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VSI_ID                              vsi_ndx,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *my_mac_lsb
  );

uint32
  arad_pp_mymac_vsi_lsb_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VSI_ID                              vsi_ndx,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *my_mac_lsb
  );

uint32
  arad_pp_mymac_vsi_lsb_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VSI_ID                              vsi_ndx
  );


uint32
  arad_pp_mymac_vsi_lsb_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_VSI_ID                              vsi_ndx,
    SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS                       *my_mac_lsb
  );


uint32
  arad_pp_mymac_vrrp_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_MYMAC_VRRP_INFO                     *vrrp_info
  );

uint32
  arad_pp_mymac_vrrp_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_MYMAC_VRRP_INFO                     *vrrp_info
  );

uint32
  arad_pp_mymac_vrrp_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_mymac_vrrp_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_MYMAC_VRRP_INFO                     *vrrp_info
  );


uint32
  arad_pp_mymac_vrrp_mac_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 vrrp_id_ndx,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                *vrrp_mac_lsb_key,
    SOC_SAND_IN  uint8                                  enable
  );

uint32
  arad_pp_mymac_vrrp_mac_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 vrrp_id_ndx,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                *vrrp_mac_lsb_key,
    SOC_SAND_IN  uint8                                  enable
  );

uint32
  arad_pp_mymac_vrrp_mac_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 vrrp_id_ndx,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                *vrrp_mac_lsb_key
  );


uint32
  arad_pp_mymac_vrrp_mac_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 vrrp_id_ndx,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                *vrrp_mac_lsb_key,
    SOC_SAND_OUT uint8                                  *enable
  );


uint32
  arad_pp_mymac_trill_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_MYMAC_TRILL_INFO                    *trill_info
  );

uint32
  arad_pp_mymac_trill_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_MYMAC_TRILL_INFO                    *trill_info
  );

uint32
  arad_pp_mymac_trill_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_mymac_trill_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_MYMAC_TRILL_INFO                    *trill_info
  );


uint32
  arad_pp_lif_my_bmac_msb_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *my_bmac_msb
  );

uint32
  arad_pp_lif_my_bmac_msb_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *my_bmac_msb
  );

uint32
  arad_pp_lif_my_bmac_msb_get_verify(
    SOC_SAND_IN  int                                 unit
  );


uint32
  arad_pp_lif_my_bmac_msb_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS                       *my_bmac_msb
  );


uint32
  arad_pp_lif_my_bmac_port_lsb_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                       *src_sys_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *my_mac_lsb,
    SOC_SAND_IN  uint8                                 enable
  );

uint32
  arad_pp_lif_my_bmac_port_lsb_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                       *src_sys_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *my_mac_lsb,
    SOC_SAND_IN  uint8                                 enable
  );

uint32
  arad_pp_lif_my_bmac_port_lsb_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                       *src_sys_port_ndx
  );


uint32
  arad_pp_lif_my_bmac_port_lsb_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                       *src_sys_port_ndx,
    SOC_SAND_INOUT SOC_SAND_PP_MAC_ADDRESS                       *my_mac_lsb,
    SOC_SAND_OUT uint8                                 *enable
  );


CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_mymac_get_procs_ptr(void);


CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_mymac_get_errs_ptr(void);

uint32
  SOC_PPC_MYMAC_VRRP_INFO_verify(
    SOC_SAND_IN  SOC_PPC_MYMAC_VRRP_INFO *info
  );

#ifdef BCM_88660_A0

soc_error_t
arad_pp_mymac_vrid_mymac_map_set_to_all_vsi(int unit, int cam_index, int enable);

soc_error_t
arad_pp_mymac_vrrp_cam_info_set(int unit, SOC_PPC_VRRP_CAM_INFO *info);

soc_error_t
arad_pp_mymac_vrrp_cam_info_get(int unit, SOC_PPC_VRRP_CAM_INFO *info);

soc_error_t
arad_pp_mymac_vrrp_cam_info_delete(int unit, uint8 cam_index);

#endif 

uint32
  SOC_PPC_MYMAC_TRILL_INFO_verify(
    SOC_SAND_IN  SOC_PPC_MYMAC_TRILL_INFO *info
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif


