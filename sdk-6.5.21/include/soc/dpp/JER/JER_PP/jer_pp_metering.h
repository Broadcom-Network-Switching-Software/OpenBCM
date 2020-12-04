/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __JER_PP_METERING_INCLUDED__

#define __JER_PP_METERING_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_framework.h> 
#include <soc/error.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_metering.h>





typedef struct
{
  int in_pp_port_size_delta;
} __ATTRIBUTE_PACKED__ JER_PACKET_SIZE_PROFILES_TBL_DATA;

#define JER_PP_SW_DB_MULTI_SET_PACKET_SIZE_PROFILE_NOF_MEMBER 8







uint32
  jer_pp_metering_init(
    SOC_SAND_IN  int  unit
  );



uint32
  jer_pp_metering_init_mrps_config(
		SOC_SAND_IN int unit
  );

uint32
  jer_pp_metering_init_mrpsEm_config(
		SOC_SAND_IN int unit
  );


uint32 
  jer_pp_eth_policer_pcd_init(
     SOC_SAND_IN int unit
  );


soc_error_t
  soc_jer_pp_mtr_policer_global_sharing_get(
    int                         unit,
	int                         core_id,
	int        					meter_id,
	int							meter_group,
	uint32* 					global_sharing_ptr
);


soc_error_t
  soc_jer_pp_mtr_policer_global_sharing_set(
    int                         unit,
	int                         core_id,
	int        					meter_id,
	int							meter_group,
	uint32* 					global_sharing_ptr
);


soc_error_t
  soc_jer_pp_mtr_policer_global_mef_10_3_get(
    int                         unit,
	int*       					arg
);



soc_error_t
  soc_jer_pp_mtr_policer_global_mef_10_3_set(
    int                         unit,
	int*       					arg
);



uint32
  jer_pp_mtr_eth_policer_params_set(
    SOC_SAND_IN  int                                    unit,
	SOC_SAND_IN  int                           			core_id,
    SOC_SAND_IN  SOC_PPC_PORT                           port_ndx,
	SOC_SAND_IN  SOC_PPC_MTR_ETH_TYPE                   eth_type_ndx,
    SOC_SAND_IN  SOC_PPC_MTR_BW_PROFILE_INFO            *policer_info
  );


uint32
  jer_pp_mtr_eth_policer_params_get(
    SOC_SAND_IN  int                        	unit,
	SOC_SAND_IN  int                           core_id,
    SOC_SAND_IN  SOC_PPC_PORT                   port_ndx,
	SOC_SAND_IN  SOC_PPC_MTR_ETH_TYPE           eth_type_ndx,
    SOC_SAND_OUT SOC_PPC_MTR_BW_PROFILE_INFO    *policer_info
  );


uint32
  jer_pp_mtr_eth_policer_glbl_profile_set(
	SOC_SAND_IN int       						unit,
	SOC_SAND_IN  int                           core_id,
	SOC_SAND_IN uint32	                		policer_ndx,
	SOC_SAND_IN SOC_PPC_MTR_BW_PROFILE_INFO    	*policer_info
  );


uint32  
  jer_pp_mtr_eth_policer_glbl_profile_get(
	 SOC_SAND_IN  int                      		unit,
	 SOC_SAND_IN  int                           core_id,
	 SOC_SAND_IN  uint32                  		glbl_profile_idx,
	 SOC_SAND_OUT SOC_PPC_MTR_BW_PROFILE_INFO 	*policer_info
  );


uint32  
  jer_pp_mtr_eth_policer_glbl_profile_map_set(
	 SOC_SAND_IN  int                  	unit,
	 SOC_SAND_IN  int                    core_id,
	 SOC_SAND_IN  SOC_PPC_PORT           port,
	 SOC_SAND_IN  SOC_PPC_MTR_ETH_TYPE   eth_type_ndx,
	 SOC_SAND_IN  uint32                 glbl_profile_idx
  );


uint32  
  jer_pp_mtr_eth_policer_glbl_profile_map_get(
	 SOC_SAND_IN  int                  	unit,
	 SOC_SAND_IN  int                   core_id,
	 SOC_SAND_IN  SOC_PPC_PORT          port,
	 SOC_SAND_IN  SOC_PPC_MTR_ETH_TYPE  eth_type_ndx,
	 SOC_SAND_OUT uint32                *glbl_profile_idx
   );


uint32
jer_pp_mtr_policer_ipg_compensation_set(
    int                         unit,
	uint8						ipg_compensation_enabled
);

uint32
jer_pp_mtr_policer_ipg_compensation_get(
    int                         unit,
	uint8						*ipg_compensation_enabled
);


uint32
jer_pp_mtr_policer_hdr_compensation_set(
    int                         unit,
	int                         core_id,
	uint32                      pp_port,
	int                         compensation_size
);

uint32
jer_pp_mtr_policer_hdr_compensation_get(
    int                         unit,
	int                         core_id,
	uint32                      pp_port,
	int                         *compensation_size
);


#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

