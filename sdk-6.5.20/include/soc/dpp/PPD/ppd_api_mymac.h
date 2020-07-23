/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPD_API_MYMAC_INCLUDED__

#define __SOC_PPD_API_MYMAC_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>
#include <soc/dpp/PPC/ppc_api_mymac.h>

#include <soc/dpp/PPD/ppd_api_general.h>













typedef enum
{
  
  SOC_PPD_MYMAC_MSB_SET = SOC_PPD_PROC_DESC_BASE_MYMAC_FIRST,
  SOC_PPD_MYMAC_MSB_SET_PRINT,
  SOC_PPD_MYMAC_MSB_GET,
  SOC_PPD_MYMAC_MSB_GET_PRINT,
  SOC_PPD_MYMAC_VSI_LSB_SET,
  SOC_PPD_MYMAC_VSI_LSB_SET_PRINT,
  SOC_PPD_MYMAC_VSI_LSB_GET,
  SOC_PPD_MYMAC_VSI_LSB_GET_PRINT,
  SOC_PPD_MYMAC_VRRP_INFO_SET,
  SOC_PPD_MYMAC_VRRP_INFO_SET_PRINT,
  SOC_PPD_MYMAC_VRRP_INFO_GET,
  SOC_PPD_MYMAC_VRRP_INFO_GET_PRINT,
  SOC_PPD_MYMAC_VRRP_MAC_SET,
  SOC_PPD_MYMAC_VRRP_MAC_SET_PRINT,
  SOC_PPD_MYMAC_VRRP_MAC_GET,
  SOC_PPD_MYMAC_VRRP_MAC_GET_PRINT,
  SOC_PPD_MYMAC_TRILL_INFO_SET,
  SOC_PPD_MYMAC_TRILL_INFO_SET_PRINT,
  SOC_PPD_MYMAC_TRILL_INFO_GET,
  SOC_PPD_MYMAC_TRILL_INFO_GET_PRINT,
  SOC_PPD_LIF_MY_BMAC_MSB_SET,
  SOC_PPD_LIF_MY_BMAC_MSB_SET_PRINT,
  SOC_PPD_LIF_MY_BMAC_MSB_GET,
  SOC_PPD_LIF_MY_BMAC_MSB_GET_PRINT,
  SOC_PPD_LIF_MY_BMAC_PORT_LSB_SET,
  SOC_PPD_LIF_MY_BMAC_PORT_LSB_SET_PRINT,
  SOC_PPD_LIF_MY_BMAC_PORT_LSB_GET,
  SOC_PPD_LIF_MY_BMAC_PORT_LSB_GET_PRINT,
  SOC_PPD_MYMAC_GET_PROCS_PTR,
  
  
  SOC_PPD_MYMAC_PROCEDURE_DESC_LAST
} SOC_PPD_MYMAC_PROCEDURE_DESC;











uint32
  soc_ppd_mymac_msb_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                     *my_mac_msb
  );


uint32
  soc_ppd_mymac_msb_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS                     *my_mac_msb
  );


uint32
  soc_ppd_mymac_vsi_lsb_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_VSI_ID                              vsi_ndx,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                     *my_mac_lsb
  );


uint32
  soc_ppd_mymac_vsi_lsb_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_VSI_ID                              vsi_ndx,
    SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS                     *my_mac_lsb
  );


uint32
  soc_ppd_mymac_vrrp_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_MYMAC_VRRP_INFO                     *vrrp_info
  );


uint32
  soc_ppd_mymac_vrrp_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPC_MYMAC_VRRP_INFO                     *vrrp_info
  );


uint32
  soc_ppd_mymac_vrrp_mac_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                vrrp_id_ndx,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                     *vrrp_mac_lsb_key,
    SOC_SAND_IN  uint8                               enable
  );


uint32
  soc_ppd_mymac_vrrp_mac_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                vrrp_id_ndx,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                     *vrrp_mac_lsb_key,
    SOC_SAND_OUT uint8                               *enable
  );


uint32
  soc_ppd_mymac_trill_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPC_MYMAC_TRILL_INFO                    *trill_info
  );


uint32
  soc_ppd_mymac_trill_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPC_MYMAC_TRILL_INFO                    *trill_info
  );


uint32
  soc_ppd_lif_my_bmac_msb_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                     *my_bmac_msb
  );


uint32
  soc_ppd_lif_my_bmac_msb_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS                     *my_bmac_msb
  );


uint32
  soc_ppd_lif_my_bmac_port_lsb_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                     *src_sys_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                     *my_mac_lsb,
    SOC_SAND_IN  uint8                               enable
  );


uint32
  soc_ppd_lif_my_bmac_port_lsb_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                     *src_sys_port_ndx,
    SOC_SAND_INOUT SOC_SAND_PP_MAC_ADDRESS                     *my_mac_lsb,
    SOC_SAND_OUT uint8                               *enable
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

