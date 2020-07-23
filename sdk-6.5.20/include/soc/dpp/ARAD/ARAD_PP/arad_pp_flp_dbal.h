/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
*/

#ifndef __ARAD_PP_FLP_DBAL_INCLUDED__
#define __ARAD_PP_FLP_DBAL_INCLUDED__


#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_dbal.h>

uint32 arad_pp_dbal_flp_hw_based_key_enable(int unit, int program_id, SOC_DPP_HW_KEY_LOOKUP hw_key_based_lookup_enable);



uint32 arad_pp_flp_dbal_oam_statistics_program_tables_init(int unit, int prog_id);
uint32 arad_pp_flp_dbal_oam_down_untagged_statistics_program_tables_init(int unit, int prog_id);
uint32 arad_pp_flp_dbal_bfd_statistics_program_tables_init(int unit, int prog_id);
uint32 arad_pp_flp_dbal_oam_single_tag_statistics_program_tables_init(int unit, int prog_id);
uint32 arad_pp_flp_dbal_oam_double_tag_statistics_program_tables_init(int unit, int prog_id);
uint32 arad_pp_flp_dbal_bfd_mpls_statistics_program_tables_init(int unit, int prog_id);
uint32 arad_pp_flp_dbal_bfd_pwe_statistics_program_tables_init(int unit, int bfd_statistics_prog_id);
uint32 arad_pp_dbal_flp_ethernet_ing_ivl_learn_tables_create(int unit, int prog_id);
uint32 arad_pp_dbal_flp_ethernet_ing_ivl_inner_learn_tables_create(int unit, int prog_id);
uint32 arad_pp_dbal_flp_ethernet_ing_ivl_fwd_outer_learn_tables_create(int unit, int prog_id);
soc_error_t arad_pp_flp_dbal_bfd_echo_program_tables_init(int unit);

uint32 arad_pp_flp_dbal_ipv4mc_tcam_tables_init(int unit, int mim_learning_prog_id);

uint32 arad_pp_flp_dbal_trill_program_tcam_tables_init(int unit);
uint32 arad_pp_flp_dbal_ipv4uc_rpf_program_tables_init(int unit);
uint32 arad_pp_flp_dbal_ipv4uc_program_tables_init(int unit);
uint32 arad_pp_flp_dbal_ipv4_dc_program_tables_init(int unit);
uint32 arad_pp_flp_dbal_ipv4mc_bridge_program_tables_init(int unit);
uint32 arad_pp_flp_dbal_ipv4mc_bridge_program_tables_init_ivl(int unit,int prog_id_untagged,int prog_id);
uint32 arad_pp_flp_dbal_ipv4compmc_with_rpf_program_tables_init(int unit);
uint32 arad_pp_flp_dbal_ipv4compmc_with_mim_learning_program_tables_init(int unit,int prog_id);
uint32 arad_pp_flp_dbal_ipv4uc_l3vpn_program_tables_init(int unit);
uint32 arad_pp_flp_dbal_ipv6uc_l3vpn_program_tables_init(int unit,int prog_id);
uint32 arad_pp_flp_dbal_ipv4uc_l3vpn_custom_program_tables_init(int unit,int custom_prgrm);
uint32 arad_pp_flp_dbal_ipv4uc_l3vpn_rpf_program_tables_init(int unit, int prog_id);
uint32 arad_pp_flp_dbal_ipv6uc_l3vpn_rpf_program_tables_init(int unit, int prog_id);
uint32 arad_pp_flp_dbal_ipv6uc_program_tables_init(int unit);
uint32 arad_pp_flp_dbal_ipv6uc_with_rpf_program_tables_init(int unit, int prog_id);
uint32 arad_pp_flp_dbal_ipv6mc_program_tables_init(int unit);
uint32 arad_pp_flp_dbal_ipv6mc_ssm_compress_sip_program_tables_init(int unit, int prog_id);
uint32 arad_pp_flp_dbal_ipv6mc_ssm_program_tables_init(int unit, int prog_id);
uint32 arad_pp_flp_dbal_ipv6mc_kbp_table_init(int unit);
uint32 arad_pp_flp_dbal_ipv6uc_kbp_table_init(int unit);
uint32 arad_pp_flp_dbal_ipv6uc_rpf_kbp_table_init(int unit, int prog_id);
uint32 arad_pp_flp_dbal_ipv6uc_kaps_table_create(int unit);
uint32 arad_pp_flp_dbal_ipv6uc_rpf_kaps_table_create(int unit);
uint32 arad_pp_flp_dbal_vpws_tagged_program_tables_init(int unit);
uint32 arad_pp_flp_dbal_lsr_program_tables_init(int unit);
uint32 arad_pp_flp_dbal_oam_outlif_l2g_program_tables_init(int unit, int prog_id);
uint32 arad_pp_flp_dbal_two_pass_mc_encap_to_dest_program_tables_init(int unit, int prog_id);

uint32 arad_pp_flp_dbal_ipv6uc_kbp_sip_sharing_program_tables_init(int unit, int prog_id);

uint32 arad_pp_flp_dbal_fcoe_program_tables_init(int unit, int is_vsan_from_vsi, int prog1, int prog2);
uint32 arad_pp_flp_dbal_fcoe_npv_program_tables_init(int unit, int is_vsan_from_vsi, int fcoe_no_vft_prog_id, int fcoe_vft_prog_id);


uint32 arad_pp_flp_dbal_source_lookup_with_aget_access_enable(int unit, int prog_id);
uint32 arad_pp_flp_dbal_mpls_lsr_stat_table_init(int unit);



uint32 arad_pp_flp_dbal_program_info_dump(int unit, uint32 cur_prog_id);
uint32 arad_pp_flp_dbal_pon_ipv6_sav_static_program_tables_init(int unit,uint8 sa_auth_enabled,uint8 slb_enabled,uint32 tcam_access_profile_id);
uint32 arad_pp_flp_dbal_pon_ipv4_sav_static_program_tables_init(int unit, uint8 sa_auth_enabled,uint8 slb_enabled);
uint32 arad_pp_flp_dbal_pon_sav_arp_program_tables_init(int unit, int is_up, int prog_id);


uint32 arad_pp_flp_dbal_pppoe_ipv4uc_program_tables_init(int unit);

uint32 arad_pp_flp_dbal_pppoe_ipv6uc_program_tables_init(int unit);


uint32 arad_pp_flp_dbal_gpon_l2_ip4mc_tables_init(int unit, int gpon_l2_ip4mc_prog_id);
uint32 arad_pp_flp_dbal_gpon_l2_ip6mc_tables_init(int unit, int gpon_l2_ip4mc_prog_id);

#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif

