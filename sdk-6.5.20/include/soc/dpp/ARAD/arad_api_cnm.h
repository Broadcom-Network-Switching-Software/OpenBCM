/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __ARAD_API_CNM_INCLUDED__

#define __ARAD_API_CNM_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_api_general.h>
#include <soc/dpp/TMC/tmc_api_cnm.h>





#define  ARAD_CNM_NOF_SAMPLING_BASES  (SOC_TMC_CNM_NOF_SAMPLING_BASES)


#define  ARAD_CNM_NOF_PROFILES        (SOC_TMC_CNM_NOF_PROFILES)


#define  ARAD_CNM_MAX_CP_QUEUES      SOC_TM_CNM_MAX_CP_QUEUES








typedef SOC_TMC_CNM_CP_INFO             ARAD_CNM_CP_INFO;
typedef SOC_TMC_CNM_CPQ_INFO            ARAD_CNM_CPQ_INFO;
typedef SOC_TMC_CNM_CP_PROFILE_INFO     ARAD_CNM_CP_PROFILE_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint32 cp_queue_low;
  
  uint32 cp_queue_high;
  
  SOC_TMC_CNM_Q_SET q_set;
  
  uint8 odd_queues_select;

} ARAD_CNM_Q_MAPPING_INFO;

typedef struct
{
  
  
  uint32 ftmh_src_port;

  
  uint32 ftmh_tc;

  
  uint32 ftmh_dp;

  
  SOC_SAND_PP_MAC_ADDRESS mac_sa;

  
  uint32 ether_type;

  
  uint32 pdu_version;

  
  uint32 pdu_reserved_v;

  
  
  uint32 ftmh_otm_port;

  
  uint32 vlan_edit_command_with_cn_tag;
  uint32 vlan_edit_command_without_cn_tag;
  uint32 has_vlan_config;
  uint32 vlan_id;
  uint32 vlan_cfi;
  uint32 vlan_priority;

}ARAD_CNM_CP_PACKET_INFO;


typedef struct
{
  
  uint8 disable_cn_tag_if_exist_in_dram;

  
  uint8 gen_if_no_cn_tag;

  
  uint8 add_cn_tag;

  
  uint8 gen_if_no_pph;

  
  uint32 cn_tag_ethertype;
  uint32 cn_tag_flow_id;

  
  uint8 gen_if_replications;

#if defined(BCM_88650_B0)
  

  
  uint8 disable_fabric_crc_calculation;

  
  uint8 disable_crc_removal;

  
  uint8 enable_cnm_ocb;
#endif

}ARAD_CNM_CP_OPTIONS;









uint32
  arad_cnm_cp_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  ARAD_CNM_CP_INFO                    *info
  );

uint32
  arad_cnm_cp_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_OUT ARAD_CNM_CP_INFO                    *info
  );



uint32
  arad_cnm_queue_mapping_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  ARAD_CNM_Q_MAPPING_INFO             *info
  );

uint32
  arad_cnm_queue_mapping_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_OUT ARAD_CNM_Q_MAPPING_INFO             *info
  );


uint32
  arad_cnm_cpq_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              cpq_ndx,
    SOC_SAND_IN  ARAD_CNM_CPQ_INFO                   *info
  );

uint32
  arad_cnm_cpq_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              cpq_ndx,
    SOC_SAND_OUT ARAD_CNM_CPQ_INFO                   *info
  );


uint32
  arad_cnm_cp_profile_set(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  uint32                         profile_ndx,
    SOC_SAND_IN  ARAD_CNM_CP_PROFILE_INFO       *info
  );

uint32
  arad_cnm_cp_profile_get(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  uint32                         profile_ndx,
    SOC_SAND_OUT ARAD_CNM_CP_PROFILE_INFO       *info
  );


uint32
  arad_cnm_cp_packet_set(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  ARAD_CNM_CP_PACKET_INFO       *info
  );

uint32
  arad_cnm_cp_packet_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_OUT ARAD_CNM_CP_PACKET_INFO       *info
  );


uint32
  arad_cnm_cp_options_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  ARAD_CNM_CP_OPTIONS                 *info
  );

uint32
  arad_cnm_cp_options_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_OUT ARAD_CNM_CP_OPTIONS                 *info
  );



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
