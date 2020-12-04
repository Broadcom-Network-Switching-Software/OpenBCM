/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __JER_PP_OAM_INCLUDED__

#define __JER_PP_OAM_INCLUDED__



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/PPD/ppd_api_oam.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_oam.h>








#define _JER_PP_OAM_MIRROR_STRENGTH 3
#define _JER_PP_OAM_FORWARD_STRENGTH 3




#define JER_PP_DO_NOTHING_AND_EXIT                                         \
          SOC_SAND_IGNORE_UNUSED_VAR(res);                                    \
          goto exit



#define _JER_PP_OAM_MDL_MP_TYPE_NO_MP 0
#define _JER_PP_OAM_MDL_MP_TYPE_MIP 1
#define _JER_PP_OAM_MDL_MP_TYPE_ACTIVE_MATCH 2
#define _JER_PP_OAM_MDL_MP_TYPE_PASSIVE_MATCH 3



#define JERICHO_PP_OAM_EXTRACT_MDL_MP_TYPE_FROM_MP_TYPE_VECTOR_BY_LEVEL(mp_type_vector,level) (((mp_type_vector) >> ((level)*2)) & 0x3)

#define JERICHO_PP_OAM_SET_MDL_MP_TYPE_VECTOR_BY_LEVEL(mp_type_vector,mp_type,level)\
do {\
    uint32 temp_mp_type_vector = mp_type_vector, mp_type_temp=mp_type;\
    SHR_BITCOPY_RANGE(&temp_mp_type_vector, (level)*2 ,&mp_type_temp,0,2);\
    mp_type_vector = temp_mp_type_vector;\
}\
while (0)



#define _JER_PP_OAM_MP_TYPE_FROM_MDL_MP_TYPE(mdl_mp_type) (mdl_mp_type -1)

#define JER_PP_OAM_LOOPBACK_TST_INFO_FLAGS_LBM 0x1
#define JER_PP_OAM_LOOPBACK_TST_INFO_FLAGS_TST 0x2





typedef struct {
    uint32                    endpoint_id;          
    uint8                     flags;                
    SOC_SAND_PP_MAC_ADDRESS   mac_address;          
    uint8                     pkt_pri;              
    uint8                     inner_pkt_pri;        
    uint32                    sys_port;            
    uint32                    local_port;           
    uint8                     is_up_mep;
    bcm_cos_t                 int_pri;              
} JER_PP_OAM_LOOPBACK_TST_INFO;













soc_error_t soc_jer_pp_oam_classifier_init(int unit);


soc_error_t soc_jer_pp_oam_oamp_init(int unit);

soc_error_t
  soc_jer_pp_oamp_control_sat_weight_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT uint32                *sat_weight
  );


int soc_jer_pp_oam_counter_pointer_addition(int unit, const SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY * classifier_mep_entry, int is_active);



soc_error_t soc_jer_pp_oam_oem1_mep_add(
        int unit,
        const SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY * classifier_mep_entry,
        const SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY * oem1_key,
        const SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD * prev_payload,
        SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD * new_payload,
        uint8 is_active,
        uint8 update);


soc_error_t soc_jer_pp_oam_oem1_mep_delete(
        int unit,
        const SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY * classifier_mep_entry,
        const SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY * oem1_key,
        uint32 * new_mp_type_vector,
        uint8 change_to_passive);


soc_error_t
  soc_jer_pp_oam_classifier_default_profile_add(
     SOC_SAND_IN  int                                unit,
     SOC_SAND_IN  ARAD_PP_OAM_DEFAULT_EP_ID          mep_index,
     SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
     SOC_SAND_IN  uint8                              update_action_only
  );


soc_error_t
  soc_jer_pp_bfd_oamp_udp_src_port_check_enable(
     SOC_SAND_IN  int                                unit,
     SOC_SAND_IN  uint8                              enable
  );


soc_error_t
  soc_jer_pp_oam_classifier_default_profile_remove(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  ARAD_PP_OAM_DEFAULT_EP_ID          mep_index
  );



soc_error_t
  soc_jer_pp_oam_classifier_default_profile_action_set(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  ARAD_PP_OAM_DEFAULT_EP_ID          mep_index,
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry
  );


soc_error_t
  soc_jer_pp_oam_inlif_profile_map_set(
     SOC_SAND_IN  int                                                  unit,
     SOC_SAND_IN  uint32                                               inlif_profile,
     SOC_SAND_IN  uint32                                               oam_profile
  );

soc_error_t
  soc_jer_pp_oam_inlif_profile_map_get(
     SOC_SAND_IN  int                                                  unit,
     SOC_SAND_IN  uint32                                               inlif_profile,
     SOC_SAND_OUT uint32                                               *oam_profile
  );


soc_error_t soc_jer_pp_oam_oamp_eth1731_profile_set(
    int                                 unit,
    uint8                          profile_indx,
    const SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY     *eth1731_profile
  );

soc_error_t soc_jer_pp_oam_oamp_eth1731_profile_get(
    int                                 unit,
    uint8                          profile_indx,
    SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY     *eth1731_profile
  );


soc_error_t soc_jer_pp_oam_mp_type_config_init(int unit);


soc_error_t soc_jer_pp_oam_init_eci_tod(
    int                                 unit,
   uint8                                init_ntp,
   uint8                                init_1588
   );




soc_error_t soc_jer_pp_oam_tod_set(
    int                                 unit,
   uint8                                is_ntp,
   uint64                               data
   );

soc_error_t soc_jer_pp_oam_tod_get(
   int                                 unit,
   uint8                               is_ntp,
   uint64                              *data
   );


soc_error_t soc_jer_pp_oam_tod_leap_control_set(
    int                                 unit,
    bcm_oam_timestamp_format_t          ts_format,
    bcm_oam_tod_leap_control_t          leap_info
   );

soc_error_t soc_jer_pp_oam_tod_leap_control_get(
    int                                 unit,
    bcm_oam_timestamp_format_t          ts_format,
    bcm_oam_tod_leap_control_t          *leap_info
   );


soc_error_t soc_jer_pp_oam_sa_addr_msbs_set(
    int unit,
    int profile,
   const uint8 * msbs);



soc_error_t soc_jer_pp_oam_sa_addr_msbs_get(
    int unit,
    int profile,
    uint8 * msbs);




soc_error_t soc_jer_pp_oam_dm_trigger_set(
   int unit,
   int endpoint_id);


soc_error_t soc_jer_pp_oam_classifier_oam1_entries_add(
   int unit);


soc_error_t soc_jer_pp_oam_rfc_6374_lm_config(
   int unit,
   uint32 exp_field_value);


soc_error_t soc_jer_pp_oam_rfc_6374_dm_config(
   int unit,
   uint32 exp_field_value);


soc_error_t soc_jer_oamp_rfc6374_lm_dm_config(
        int unit,
        uint8 is_ilm,
        uint8 timestamp_format);


soc_error_t soc_jer_pp_oam_egress_pcp_set_by_profile_and_tc(
   int unit,
   uint8 tc,
   uint8 outlif_profile,
   uint8 oam_pcp);


soc_error_t soc_jer_pp_oam_sat_init(int unit);

soc_error_t
    soc_jer_pp_oam_loopback_tst_info_init(SOC_SAND_IN int unit,
       SOC_SAND_INOUT JER_PP_OAM_LOOPBACK_TST_INFO *lb_tst_info);

  
 soc_error_t 
 soc_jer_pp_oam_oamp_lb_tst_header_set (SOC_SAND_IN int unit, 
      SOC_SAND_IN JER_PP_OAM_LOOPBACK_TST_INFO *lb_tst_info,
      SOC_SAND_INOUT uint8* header_buffer, 
      SOC_SAND_OUT int *header_offset);



soc_error_t soc_jer_pp_oam_init_from_init_sequence(int unit);

soc_error_t soc_arad_oamp_cpu_port_dp_tc_set(int unit,uint32 dp,uint32 tc);
soc_error_t soc_arad_oamp_tmx_arb_weight_set(int unit,uint32 txm_arbiter_weight);
soc_error_t soc_jer_oamp_sat_arb_weight_set(int unit,uint32 sat_arbiter_weight);
soc_error_t soc_arad_oamp_response_weight_set(int unit,uint32 rsp_arbiter_weight);
soc_error_t soc_jer_pp_oam_classifier_oam1_2_entries_insert_egress_inject( SOC_SAND_IN    int unit,
                                                                           SOC_SAND_IN    SOC_PPC_OAM_LIF_PROFILE_DATA *profile_data,
                                                                           SOC_SAND_INOUT _oam_oam_a_b_table_buffer_t  *oama_buffer,
                                                                           SOC_SAND_INOUT _oam_oam_a_b_table_buffer_t  *oamb_buffer);


soc_error_t soc_jer_oamp_rfc6374_lm_control_code_set(int unit, uint8 is_response, uint32 rfc6374_lm_control_code);

soc_error_t soc_jer_oamp_rfc6374_lm_control_code_get(int unit, uint8 is_response, uint32 *rfc6374_lm_control_code);

soc_error_t soc_jer_oamp_rfc6374_dm_control_code_set(int unit, uint8 is_response, uint32 rfc6374_dm_control_code);

soc_error_t soc_jer_oamp_rfc6374_dm_control_code_get(int unit, uint8 is_response, uint32 *rfc6374_dm_control_code);


soc_error_t soc_jer_oamp_rfc6374_rx_trap_codes_set(int unit, uint8 is_rfc6374_lm, uint8 is_rfc6374_dm, uint32 trap_code);

soc_error_t soc_jer_oamp_rfc6374_rx_filter_table_set(int unit, int control_code, uint8 is_response, uint32 punt_2_ignore_1_pass_0);

soc_error_t soc_jer_oamp_rfc6374_rx_filter_table_get(int unit, int control_code, uint8 is_response, uint32 * punt_2_ignore_1_pass_0);

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif
