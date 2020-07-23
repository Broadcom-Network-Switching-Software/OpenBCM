/*
 * $Id:  fb6_cambist.h
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#define FB6_BSK_FTFP_TCAM {fb6_bsk_ftfp_tcam_name, NO_MODE, NO_MODE_FIELD, BSK_FTFP_CAM_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 2, BSK_FTFP_CAM_BIST_STATUSr}

#define FB6_BSK_FTFP2_TCAM {fb6_bsk_ftfp2_tcam_name, NO_MODE, NO_MODE_FIELD, BSK_FTFP2_CAM_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 2, BSK_FTFP2_CAM_BIST_STATUSr}

#define FB6_BSK_AMFP_TCAM {fb6_bsk_amfp_tcam_name, NO_MODE, NO_MODE_FIELD, BSK_AMFP_CAM_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 2, BSK_AMFP_CAM_BIST_STATUSr}

#define FB6_BSK_AEFP_TCAM {fb6_bsk_aefp_tcam_name, NO_MODE, NO_MODE_FIELD, BSK_AEFP_CAM_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 2, BSK_AEFP_CAM_BIST_STATUSr}

#define FB6_BSK_FTFP_LTS_A_KEY_GEN_TCAM {fb6_bsk_ftfp_lts_a_key_gen_tcam_name, NO_MODE, NO_MODE_FIELD, BSK_FTFP_LTS_A_KEY_GEN_CAM_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 1, BSK_FTFP_LTS_A_KEY_GEN_CAM_BIST_STATUSr}

#define FB6_CPU_COS_MAP {fb6_cpu_cos_map_name, NO_MODE, NO_MODE_FIELD, CPU_COS_CAM_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 1, CPU_COS_CAM_BIST_STATUSr}

#define FB6_DST_COMPRESSION {fb6_dst_compression_name, NO_MODE, NO_MODE_FIELD, DST_COMPRESSION_CAM_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 1, DST_COMPRESSION_CAM_BIST_STATUSr}

#define FB6_EFP_TCAM_POOL0 {fb6_efp_tcam_pool0_name, NO_MODE, NO_MODE_FIELD, EFP_SLICE_0_CAM_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 1, EFP_SLICE_0_CAM_BIST_0_STATUSr}

#define FB6_EFP_TCAM_POOL1 {fb6_efp_tcam_pool1_name, NO_MODE, NO_MODE_FIELD, EFP_SLICE_1_CAM_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 1, EFP_SLICE_1_CAM_BIST_0_STATUSr}

#define FB6_EFP_TCAM_POOL2 {fb6_efp_tcam_pool2_name, NO_MODE, NO_MODE_FIELD, EFP_SLICE_2_CAM_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 1, EFP_SLICE_2_CAM_BIST_0_STATUSr}

#define FB6_EFP_TCAM_POOL3 {fb6_efp_tcam_pool3_name, NO_MODE, NO_MODE_FIELD, EFP_SLICE_3_CAM_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 1, EFP_SLICE_3_CAM_BIST_0_STATUSr}

#define FB6_EGR_ADAPT_1_LOGICAL_TBL_SEL_TCAM {fb6_egr_adapt_1_logical_tbl_sel_tcam_name, NO_MODE, NO_MODE_FIELD, EGR_ADAPT_1_KEY_GEN_CAM_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 1, EGR_ADAPT_1_KEY_GEN_CAM_BIST_STATUSr}

#define FB6_EGR_ADAPT_2_LOGICAL_TBL_SEL_TCAM {fb6_egr_adapt_2_logical_tbl_sel_tcam_name, NO_MODE, NO_MODE_FIELD, EGR_ADAPT_2_KEY_GEN_CAM_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 1, EGR_ADAPT_2_KEY_GEN_CAM_BIST_STATUSr}

#define FB6_EGR_FIELD_EXTRACTION_PROFILE_1_TCAM {fb6_egr_field_extraction_profile_1_tcam_name, NO_MODE, NO_MODE_FIELD, EGR_FIELD_EXTRACTION_PROFILE_1_CAM_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 1, EGR_FIELD_EXTRACTION_PROFILE_1_CAM_BIST_STATUSr}

#define FB6_EGR_FIELD_EXTRACTION_PROFILE_2_TCAM {fb6_egr_field_extraction_profile_2_tcam_name, NO_MODE, NO_MODE_FIELD, EGR_FIELD_EXTRACTION_PROFILE_2_CAM_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 1, EGR_FIELD_EXTRACTION_PROFILE_2_CAM_BIST_STATUSr}

#define FB6_EGR_PKT_FLOW_SELECT_TCAM {fb6_egr_pkt_flow_select_tcam_name, NO_MODE, NO_MODE_FIELD, EGR_PKT_FLOW_SELECT_CAM_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 4, EGR_PKT_FLOW_SELECT_CAM_BIST_STATUSr}

#define FB6_EGR_QOS_CTRL_TCAM {fb6_egr_qos_ctrl_tcam_name, NO_MODE, NO_MODE_FIELD, EGR_QOS_CTRL_CAM_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 1, EGR_QOS_CTRL_CAM_BIST_STATUSr}

#define FB6_EGR_ZONE_0_EDITOR_CONTROL_TCAM {fb6_egr_zone_0_editor_control_tcam_name, NO_MODE, NO_MODE_FIELD, EGR_ZONE_0_EDITOR_CONTROL_CAM_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 1, EGR_ZONE_0_EDITOR_CONTROL_CAM_BIST_STATUSr}

#define FB6_EGR_ZONE_1_EDITOR_CONTROL_TCAM {fb6_egr_zone_1_editor_control_tcam_name, NO_MODE, NO_MODE_FIELD, EGR_ZONE_1_EDITOR_CONTROL_CAM_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 1, EGR_ZONE_1_EDITOR_CONTROL_CAM_BIST_STATUSr}

#define FB6_EGR_ZONE_2_EDITOR_CONTROL_TCAM {fb6_egr_zone_2_editor_control_tcam_name, NO_MODE, NO_MODE_FIELD, EGR_ZONE_2_EDITOR_CONTROL_CAM_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 1, EGR_ZONE_2_EDITOR_CONTROL_CAM_BIST_STATUSr}

#define FB6_EGR_ZONE_3_EDITOR_CONTROL_TCAM {fb6_egr_zone_3_editor_control_tcam_name, NO_MODE, NO_MODE_FIELD, EGR_ZONE_3_EDITOR_CONTROL_CAM_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 1, EGR_ZONE_3_EDITOR_CONTROL_CAM_BIST_STATUSr}

#define FB6_EGR_ZONE_4_EDITOR_CONTROL_TCAM {fb6_egr_zone_4_editor_control_tcam_name, NO_MODE, NO_MODE_FIELD, EGR_ZONE_4_EDITOR_CONTROL_CAM_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 1, EGR_ZONE_4_EDITOR_CONTROL_CAM_BIST_STATUSr}

#define FB6_FLEX_RTAG7_HASH_TCAM_ONLY {fb6_flex_rtag7_hash_tcam_only_name, NO_MODE, NO_MODE_FIELD, FLEX_RTAG7_HASH_TCAM_ONLY_CAM_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 1, FLEX_RTAG7_HASH_TCAM_ONLY_CAM_BIST_STATUSr}

#define FB6_FORWARDING_1_LOGICAL_TBL_SEL_TCAM {fb6_forwarding_1_logical_tbl_sel_tcam_name, NO_MODE, NO_MODE_FIELD, FORWARDING_1_KEY_GEN_CAM_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 1, FORWARDING_1_KEY_GEN_CAM_BIST_STATUSr}

#define FB6_FORWARDING_2_LOGICAL_TBL_SEL_TCAM {fb6_forwarding_2_logical_tbl_sel_tcam_name, NO_MODE, NO_MODE_FIELD, FORWARDING_2_KEY_GEN_CAM_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 1, FORWARDING_2_KEY_GEN_CAM_BIST_STATUSr}

#define FB6_FORWARDING_3_LOGICAL_TBL_SEL_TCAM {fb6_forwarding_3_logical_tbl_sel_tcam_name, NO_MODE, NO_MODE_FIELD, FORWARDING_3_KEY_GEN_CAM_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 1, FORWARDING_3_KEY_GEN_CAM_BIST_STATUSr}

#define FB6_IFP_LOGICAL_TABLE_SELECT {fb6_ifp_logical_table_select_name, NO_MODE, NO_MODE_FIELD, IFP_LOGICAL_TABLE_SELECT_CAM_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 1, IFP_LOGICAL_TABLE_SELECT_CAM_BIST_STATUSr}

#define FB6_IP_MULTICAST_TCAM {fb6_ip_multicast_tcam_name, NO_MODE, NO_MODE_FIELD, IP_MULTICAST_TCAM_CAM_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 1, IP_MULTICAST_TCAM_CAM_BIST_STATUSr}

#define FB6_IP_PARSER0_HME_STAGE_TCAM_ONLY_0 {fb6_ip_parser0_hme_stage_tcam_only_0_name, NO_MODE, NO_MODE_FIELD, IP_PARSER0_HME_STAGE_CAM_BIST_CONFIG_0r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 1, IP_PARSER0_HME_STAGE_TCAM_BIST_STATUS_0r}

#define FB6_IP_PARSER1_HME_STAGE_TCAM_ONLY_0 {fb6_ip_parser1_hme_stage_tcam_only_0_name, NO_MODE, NO_MODE_FIELD, IP_PARSER1_HME_STAGE_CAM_BIST_CONFIG_0r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 1, IP_PARSER1_HME_STAGE_TCAM_BIST_STATUS_0r}

#define FB6_IP_PARSER1_HME_STAGE_TCAM_ONLY_1 {fb6_ip_parser1_hme_stage_tcam_only_1_name, NO_MODE, NO_MODE_FIELD, IP_PARSER1_HME_STAGE_CAM_BIST_CONFIG_1r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 1, IP_PARSER1_HME_STAGE_TCAM_BIST_STATUS_1r}

#define FB6_IP_PARSER1_HME_STAGE_TCAM_ONLY_2 {fb6_ip_parser1_hme_stage_tcam_only_2_name, NO_MODE, NO_MODE_FIELD, IP_PARSER1_HME_STAGE_CAM_BIST_CONFIG_2r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 1, IP_PARSER1_HME_STAGE_TCAM_BIST_STATUS_2r}

#define FB6_IP_PARSER1_HME_STAGE_TCAM_ONLY_3 {fb6_ip_parser1_hme_stage_tcam_only_3_name, NO_MODE, NO_MODE_FIELD, IP_PARSER1_HME_STAGE_CAM_BIST_CONFIG_3r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 1, IP_PARSER1_HME_STAGE_TCAM_BIST_STATUS_3r}

#define FB6_IP_PARSER1_HME_STAGE_TCAM_ONLY_4 {fb6_ip_parser1_hme_stage_tcam_only_4_name, NO_MODE, NO_MODE_FIELD, IP_PARSER1_HME_STAGE_CAM_BIST_CONFIG_4r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 1, IP_PARSER1_HME_STAGE_TCAM_BIST_STATUS_4r}

#define FB6_IP_PARSER1_MICE_TCAM_0 {fb6_ip_parser1_mice_tcam_0_name, NO_MODE, NO_MODE_FIELD, IP_PARSER1_MICE_CAM_BIST_CONFIG_0r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 2, IP_PARSER1_MICE_TCAM_BIST_STATUS_0r}

#define FB6_IP_PARSER1_MICE_TCAM_1 {fb6_ip_parser1_mice_tcam_1_name, NO_MODE, NO_MODE_FIELD, IP_PARSER1_MICE_CAM_BIST_CONFIG_1r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 2, IP_PARSER1_MICE_TCAM_BIST_STATUS_1r}

#define FB6_IP_PARSER2_HME_STAGE_TCAM_ONLY_0 {fb6_ip_parser2_hme_stage_tcam_only_0_name, NO_MODE, NO_MODE_FIELD, IP_PARSER2_HME_STAGE_CAM_BIST_CONFIG_0r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 1, IP_PARSER2_HME_STAGE_TCAM_BIST_STATUS_0r}

#define FB6_IP_PARSER2_HME_STAGE_TCAM_ONLY_1 {fb6_ip_parser2_hme_stage_tcam_only_1_name, NO_MODE, NO_MODE_FIELD, IP_PARSER2_HME_STAGE_CAM_BIST_CONFIG_1r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 1, IP_PARSER2_HME_STAGE_TCAM_BIST_STATUS_1r}

#define FB6_IP_PARSER2_HME_STAGE_TCAM_ONLY_2 {fb6_ip_parser2_hme_stage_tcam_only_2_name, NO_MODE, NO_MODE_FIELD, IP_PARSER2_HME_STAGE_CAM_BIST_CONFIG_2r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 1, IP_PARSER2_HME_STAGE_TCAM_BIST_STATUS_2r}

#define FB6_IP_PARSER2_HME_STAGE_TCAM_ONLY_3 {fb6_ip_parser2_hme_stage_tcam_only_3_name, NO_MODE, NO_MODE_FIELD, IP_PARSER2_HME_STAGE_CAM_BIST_CONFIG_3r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 1, IP_PARSER2_HME_STAGE_TCAM_BIST_STATUS_3r}

#define FB6_IP_PARSER2_HME_STAGE_TCAM_ONLY_4 {fb6_ip_parser2_hme_stage_tcam_only_4_name, NO_MODE, NO_MODE_FIELD, IP_PARSER2_HME_STAGE_CAM_BIST_CONFIG_4r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 1, IP_PARSER2_HME_STAGE_TCAM_BIST_STATUS_4r}

#define FB6_IP_PARSER2_MICE_TCAM_0 {fb6_ip_parser2_mice_tcam_0_name, NO_MODE, NO_MODE_FIELD, IP_PARSER2_MICE_CAM_BIST_CONFIG_0r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 2, IP_PARSER2_MICE_TCAM_BIST_STATUS_0r}

#define FB6_IP_PARSER2_MICE_TCAM_1 {fb6_ip_parser2_mice_tcam_1_name, NO_MODE, NO_MODE_FIELD, IP_PARSER2_MICE_CAM_BIST_CONFIG_1r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 2, IP_PARSER2_MICE_TCAM_BIST_STATUS_1r}

#define FB6_L2_USER_ENTRY {fb6_l2_user_entry_name, NO_MODE, NO_MODE_FIELD, L2_USER_ENTRY_ONLY_CAM_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 1, L2_USER_ENTRY_ONLY_CAM_BIST_STATUSr}

#define FB6_L3_TUNNEL {fb6_l3_tunnel_name, NO_MODE, NO_MODE_FIELD, L3_TUNNEL_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 4, L3_TUNNEL_BIST_STATUSr}

#define FB6_MY_STATION_TCAM {fb6_my_station_tcam_name, NO_MODE, NO_MODE_FIELD, IVP_MY_STATION_TCAM_1_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 1, IVP_MY_STATION_TCAM_1_BIST_STATUSr}

#define FB6_MY_STATION_TCAM_2 {fb6_my_station_tcam_2_name, NO_MODE, NO_MODE_FIELD, MY_STATION_TCAM_2_ENTRY_ONLY_CAM_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 1, MY_STATION_TCAM_2_ENTRY_ONLY_CAM_BIST_STATUSr}

#define FB6_PHB_SELECT_TCAM {fb6_phb_select_tcam_name, NO_MODE, NO_MODE_FIELD, PHB_SELECT_TCAM_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 2, PHB_SELECT_TCAM_CAM_BIST_STATUSr}

#define FB6_PKT_FLOW_SELECT_TCAM_0 {fb6_pkt_flow_select_tcam_0_name, NO_MODE, NO_MODE_FIELD, PKT_FLOW_SELECT_TCAM_0_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 2, PKT_FLOW_SELECT_TCAM_0_BIST_STATUSr}

#define FB6_PKT_FLOW_SELECT_TCAM_1 {fb6_pkt_flow_select_tcam_1_name, NO_MODE, NO_MODE_FIELD, PKT_FLOW_SELECT_TCAM_1_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 1, PKT_FLOW_SELECT_TCAM_1_BIST_STATUSr}

#define FB6_PKT_FLOW_SELECT_TCAM_2_ENTRY_ONLY {fb6_pkt_flow_select_tcam_2_entry_only_name, NO_MODE, NO_MODE_FIELD, PKT_FLOW_SELECT_TCAM_2_ONLY_CAM_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 1, PKT_FLOW_SELECT_TCAM_2_ONLY_CAM_BIST_STATUSr}

#define FB6_SRC_COMPRESSION {fb6_src_compression_name, NO_MODE, NO_MODE_FIELD, SRC_COMPRESSION_CAM_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 1, SRC_COMPRESSION_CAM_BIST_STATUSr}

#define FB6_TUNNEL_ADAPT_1_LOGICAL_TBL_SEL_TCAM {fb6_tunnel_adapt_1_logical_tbl_sel_tcam_name, NO_MODE, NO_MODE_FIELD, TUNNEL_ADAPT_1_KEY_GEN_CAM_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 1, TUNNEL_ADAPT_1_KEY_GEN_CAM_BIST_STATUSr}

#define FB6_TUNNEL_ADAPT_2_LOGICAL_TBL_SEL_TCAM {fb6_tunnel_adapt_2_logical_tbl_sel_tcam_name, NO_MODE, NO_MODE_FIELD, TUNNEL_ADAPT_2_KEY_GEN_CAM_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 1, TUNNEL_ADAPT_2_KEY_GEN_CAM_BIST_STATUSr}

#define FB6_TUNNEL_ADAPT_3_LOGICAL_TBL_SEL_TCAM {fb6_tunnel_adapt_3_logical_tbl_sel_tcam_name, NO_MODE, NO_MODE_FIELD, TUNNEL_ADAPT_3_KEY_GEN_CAM_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 1, TUNNEL_ADAPT_3_KEY_GEN_CAM_BIST_STATUSr}

#define FB6_TUNNEL_ADAPT_4_LOGICAL_TBL_SEL_TCAM {fb6_tunnel_adapt_4_logical_tbl_sel_tcam_name, NO_MODE, NO_MODE_FIELD, TUNNEL_ADAPT_4_KEY_GEN_CAM_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 1, TUNNEL_ADAPT_4_KEY_GEN_CAM_BIST_STATUSr}

#define FB6_UTT_TCAM_POOL0 {fb6_utt_tcam_pool0_name, NO_MODE, NO_MODE_FIELD, UTT_TCAM_POOL0_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 2, UTT_TCAM_POOL0_BIST_STATUSr}

#define FB6_UTT_TCAM_POOL1 {fb6_utt_tcam_pool1_name, NO_MODE, NO_MODE_FIELD, UTT_TCAM_POOL1_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 2, UTT_TCAM_POOL1_BIST_STATUSr}

#define FB6_UTT_TCAM_POOL2 {fb6_utt_tcam_pool2_name, NO_MODE, NO_MODE_FIELD, UTT_TCAM_POOL2_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 2, UTT_TCAM_POOL2_BIST_STATUSr}

#define FB6_UTT_TCAM_POOL3 {fb6_utt_tcam_pool3_name, NO_MODE, NO_MODE_FIELD, UTT_TCAM_POOL3_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 2, UTT_TCAM_POOL3_BIST_STATUSr}

#define FB6_UTT_TCAM_POOL4 {fb6_utt_tcam_pool4_name, NO_MODE, NO_MODE_FIELD, UTT_TCAM_POOL4_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 2, UTT_TCAM_POOL4_BIST_STATUSr}

#define FB6_UTT_TCAM_POOL5 {fb6_utt_tcam_pool5_name, NO_MODE, NO_MODE_FIELD, UTT_TCAM_POOL5_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 2, UTT_TCAM_POOL5_BIST_STATUSr}

#define FB6_UTT_TCAM_POOL6 {fb6_utt_tcam_pool6_name, NO_MODE, NO_MODE_FIELD, UTT_TCAM_POOL6_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 2, UTT_TCAM_POOL6_BIST_STATUSr}

#define FB6_UTT_TCAM_POOL7 {fb6_utt_tcam_pool7_name, NO_MODE, NO_MODE_FIELD, UTT_TCAM_POOL7_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 2, UTT_TCAM_POOL7_BIST_STATUSr}

#define FB6_UTT_TCAM_POOL8 {fb6_utt_tcam_pool8_name, NO_MODE, NO_MODE_FIELD, UTT_TCAM_POOL8_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 2, UTT_TCAM_POOL8_BIST_STATUSr}

#define FB6_UTT_TCAM_POOL9 {fb6_utt_tcam_pool9_name, NO_MODE, NO_MODE_FIELD, UTT_TCAM_POOL9_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 2, UTT_TCAM_POOL9_BIST_STATUSr}

#define FB6_UTT_TCAM_POOL10 {fb6_UTT_TCAM_POOL10_name, NO_MODE, NO_MODE_FIELD, UTT_TCAM_POOL10_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 2, UTT_TCAM_POOL10_BIST_STATUSr}

#define FB6_UTT_TCAM_POOL11 {fb6_UTT_TCAM_POOL11_name, NO_MODE, NO_MODE_FIELD, UTT_TCAM_POOL11_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 2, UTT_TCAM_POOL11_BIST_STATUSr}

#define FB6_UTT_TCAM_POOL12 {fb6_UTT_TCAM_POOL12_name, NO_MODE, NO_MODE_FIELD, UTT_TCAM_POOL12_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 2, UTT_TCAM_POOL12_BIST_STATUSr}

#define FB6_UTT_TCAM_POOL13 {fb6_UTT_TCAM_POOL13_name, NO_MODE, NO_MODE_FIELD, UTT_TCAM_POOL13_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 2, UTT_TCAM_POOL13_BIST_STATUSr}

#define FB6_UTT_TCAM_POOL14 {fb6_UTT_TCAM_POOL14_name, NO_MODE, NO_MODE_FIELD, UTT_TCAM_POOL14_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 2, UTT_TCAM_POOL14_BIST_STATUSr}

#define FB6_UTT_TCAM_POOL15 {fb6_UTT_TCAM_POOL15_name, NO_MODE, NO_MODE_FIELD, UTT_TCAM_POOL15_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 2, UTT_TCAM_POOL15_BIST_STATUSr}

#define FB6_VFP_TCAM {fb6_vfp_tcam_name, NO_MODE, NO_MODE_FIELD, VFP_CAM_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 8, VFP_CAM_BIST_STATUSr}

#define FB6_VLAN_SUBNET {fb6_vlan_subnet_name, NO_MODE, NO_MODE_FIELD, VLAN_SUBNET_ONLY_BIST_CONFIG_64r, NO_CONTROL_REGISTER, NO_CONTROL_ENABLE_FIELD, 1, VLAN_SUBNET_ONLY_BIST_STATUSr}


static char fb6_bsk_ftfp_tcam_name[] = "BSK_FTFP_TCAM";
static char fb6_bsk_ftfp2_tcam_name[] = "BSK_FTFP2_TCAM";
static char fb6_bsk_amfp_tcam_name[] = "BSK_AMFP_TCAM";
static char fb6_bsk_aefp_tcam_name[] = "BSK_AEFP_TCAM";


static char fb6_bsk_ftfp_lts_a_key_gen_tcam_name[] = "BSK_FTFP_LTS_A_KEY_GEN_TCAM";

static char fb6_cpu_cos_map_name[] = "CPU_COS_MAP";

static char fb6_dst_compression_name[] = "DST_COMPRESSION";

static char fb6_efp_tcam_pool0_name[] = "EFP_TCAM_POOL0";

static char fb6_efp_tcam_pool1_name[] = "EFP_TCAM_POOL1";

static char fb6_efp_tcam_pool2_name[] = "EFP_TCAM_POOL2";

static char fb6_efp_tcam_pool3_name[] = "EFP_TCAM_POOL3";

static char fb6_egr_adapt_1_logical_tbl_sel_tcam_name[] = "EGR_ADAPT_1_LOGICAL_TBL_SEL_TCAM";

static char fb6_egr_adapt_2_logical_tbl_sel_tcam_name[] = "EGR_ADAPT_2_LOGICAL_TBL_SEL_TCAM";

static char fb6_egr_field_extraction_profile_1_tcam_name[] = "EGR_FIELD_EXTRACTION_PROFILE_1_TCAM";

static char fb6_egr_field_extraction_profile_2_tcam_name[] = "EGR_FIELD_EXTRACTION_PROFILE_2_TCAM";

static char fb6_egr_pkt_flow_select_tcam_name[] = "EGR_PKT_FLOW_SELECT_TCAM";

static char fb6_egr_qos_ctrl_tcam_name[] = "EGR_QOS_CTRL_TCAM";

static char fb6_egr_zone_0_editor_control_tcam_name[] = "EGR_ZONE_0_EDITOR_CONTROL_TCAM";

static char fb6_egr_zone_1_editor_control_tcam_name[] = "EGR_ZONE_1_EDITOR_CONTROL_TCAM";

static char fb6_egr_zone_2_editor_control_tcam_name[] = "EGR_ZONE_2_EDITOR_CONTROL_TCAM";

static char fb6_egr_zone_3_editor_control_tcam_name[] = "EGR_ZONE_3_EDITOR_CONTROL_TCAM";

static char fb6_egr_zone_4_editor_control_tcam_name[] = "EGR_ZONE_4_EDITOR_CONTROL_TCAM";

static char fb6_flex_rtag7_hash_tcam_only_name[] = "FLEX_RTAG7_HASH_TCAM_ONLY";

static char fb6_forwarding_1_logical_tbl_sel_tcam_name[] = "FORWARDING_1_LOGICAL_TBL_SEL_TCAM";

static char fb6_forwarding_2_logical_tbl_sel_tcam_name[] = "FORWARDING_2_LOGICAL_TBL_SEL_TCAM";

static char fb6_forwarding_3_logical_tbl_sel_tcam_name[] = "FORWARDING_3_LOGICAL_TBL_SEL_TCAM";

static char fb6_ifp_logical_table_select_name[] = "IFP_LOGICAL_TABLE_SELECT";

static char fb6_ip_multicast_tcam_name[] = "IP_MULTICAST_TCAM";

static char fb6_ip_parser0_hme_stage_tcam_only_0_name[] = "IP_PARSER0_HME_STAGE_TCAM_ONLY_0";

static char fb6_ip_parser1_hme_stage_tcam_only_0_name[] = "IP_PARSER1_HME_STAGE_TCAM_ONLY_0";

static char fb6_ip_parser1_hme_stage_tcam_only_1_name[] = "IP_PARSER1_HME_STAGE_TCAM_ONLY_1";

static char fb6_ip_parser1_hme_stage_tcam_only_2_name[] = "IP_PARSER1_HME_STAGE_TCAM_ONLY_2";

static char fb6_ip_parser1_hme_stage_tcam_only_3_name[] = "IP_PARSER1_HME_STAGE_TCAM_ONLY_3";

static char fb6_ip_parser1_hme_stage_tcam_only_4_name[] = "IP_PARSER1_HME_STAGE_TCAM_ONLY_4";

static char fb6_ip_parser1_mice_tcam_0_name[] = "IP_PARSER1_MICE_TCAM_0";

static char fb6_ip_parser1_mice_tcam_1_name[] = "IP_PARSER1_MICE_TCAM_1";

static char fb6_ip_parser2_hme_stage_tcam_only_0_name[] = "IP_PARSER2_HME_STAGE_TCAM_ONLY_0";

static char fb6_ip_parser2_hme_stage_tcam_only_1_name[] = "IP_PARSER2_HME_STAGE_TCAM_ONLY_1";

static char fb6_ip_parser2_hme_stage_tcam_only_2_name[] = "IP_PARSER2_HME_STAGE_TCAM_ONLY_2";

static char fb6_ip_parser2_hme_stage_tcam_only_3_name[] = "IP_PARSER2_HME_STAGE_TCAM_ONLY_3";

static char fb6_ip_parser2_hme_stage_tcam_only_4_name[] = "IP_PARSER2_HME_STAGE_TCAM_ONLY_4";

static char fb6_ip_parser2_mice_tcam_0_name[] = "IP_PARSER2_MICE_TCAM_0";

static char fb6_ip_parser2_mice_tcam_1_name[] = "IP_PARSER2_MICE_TCAM_1";

static char fb6_l2_user_entry_name[] = "L2_USER_ENTRY";

static char fb6_l3_tunnel_name[] = "L3_TUNNEL";

static char fb6_my_station_tcam_2_name[] = "MY_STATION_TCAM_2";

static char fb6_my_station_tcam_name[] = "MY_STATION_TCAM";

static char fb6_phb_select_tcam_name[] = "PHB_SELECT_TCAM";

static char fb6_pkt_flow_select_tcam_0_name[] = "PKT_FLOW_SELECT_TCAM_0";

static char fb6_pkt_flow_select_tcam_1_name[] = "PKT_FLOW_SELECT_TCAM_1";

static char fb6_pkt_flow_select_tcam_2_entry_only_name[] = "PKT_FLOW_SELECT_TCAM_2_ENTRY_ONLY";

static char fb6_src_compression_name[] = "SRC_COMPRESSION";

static char fb6_tunnel_adapt_1_logical_tbl_sel_tcam_name[] = "TUNNEL_ADAPT_1_LOGICAL_TBL_SEL_TCAM";

static char fb6_tunnel_adapt_2_logical_tbl_sel_tcam_name[] = "TUNNEL_ADAPT_2_LOGICAL_TBL_SEL_TCAM";

static char fb6_tunnel_adapt_3_logical_tbl_sel_tcam_name[] = "TUNNEL_ADAPT_3_LOGICAL_TBL_SEL_TCAM";

static char fb6_tunnel_adapt_4_logical_tbl_sel_tcam_name[] = "TUNNEL_ADAPT_4_LOGICAL_TBL_SEL_TCAM";

static char fb6_utt_tcam_pool0_name[] = "UTT_TCAM_POOL0";

static char fb6_utt_tcam_pool1_name[] = "UTT_TCAM_POOL1";

static char fb6_utt_tcam_pool2_name[] = "UTT_TCAM_POOL2";

static char fb6_utt_tcam_pool3_name[] = "UTT_TCAM_POOL3";

static char fb6_utt_tcam_pool4_name[] = "UTT_TCAM_POOL4";

static char fb6_utt_tcam_pool5_name[] = "UTT_TCAM_POOL5";

static char fb6_utt_tcam_pool6_name[] = "UTT_TCAM_POOL6";

static char fb6_utt_tcam_pool7_name[] = "UTT_TCAM_POOL7";

static char fb6_utt_tcam_pool8_name[] = "UTT_TCAM_POOL8";

static char fb6_utt_tcam_pool9_name[] = "UTT_TCAM_POOL9";

static char fb6_UTT_TCAM_POOL10_name[] = "UTT_TCAM_POOL10";

static char fb6_UTT_TCAM_POOL11_name[] = "UTT_TCAM_POOL11";

static char fb6_UTT_TCAM_POOL12_name[] = "UTT_TCAM_POOL12";

static char fb6_UTT_TCAM_POOL13_name[] = "UTT_TCAM_POOL13";

static char fb6_UTT_TCAM_POOL14_name[] = "UTT_TCAM_POOL14";

static char fb6_UTT_TCAM_POOL15_name[] = "UTT_TCAM_POOL15";

static char fb6_vfp_tcam_name[] = "VFP_TCAM";

static char fb6_vlan_subnet_name[] = "VLAN_SUBNET";


#define   FB6_TOTAL_CAMBIST_COUNT    74
#define   FB6_DONE_BIT_MASK          0x00000001
#define   FB6_BIST_SUCCESS_MASK      0x00000002


static cambist_t fb6_cambist_array[] = {
                                        FB6_BSK_FTFP_TCAM,
                                        FB6_BSK_FTFP2_TCAM,
                                        FB6_BSK_AMFP_TCAM,
                                        FB6_BSK_AEFP_TCAM,
                                        FB6_BSK_FTFP_LTS_A_KEY_GEN_TCAM,
                                        FB6_CPU_COS_MAP,
                                        FB6_DST_COMPRESSION,
                                        FB6_EFP_TCAM_POOL0,
                                        FB6_EFP_TCAM_POOL1,
                                        FB6_EFP_TCAM_POOL2,
                                        FB6_EFP_TCAM_POOL3,
                                        FB6_EGR_ADAPT_1_LOGICAL_TBL_SEL_TCAM,
                                        FB6_EGR_ADAPT_2_LOGICAL_TBL_SEL_TCAM,
                                        FB6_EGR_FIELD_EXTRACTION_PROFILE_1_TCAM,
                                        FB6_EGR_FIELD_EXTRACTION_PROFILE_2_TCAM,
                                        FB6_EGR_PKT_FLOW_SELECT_TCAM,
                                        FB6_EGR_QOS_CTRL_TCAM,
                                        FB6_EGR_ZONE_0_EDITOR_CONTROL_TCAM,
                                        FB6_EGR_ZONE_1_EDITOR_CONTROL_TCAM,
                                        FB6_EGR_ZONE_2_EDITOR_CONTROL_TCAM,
                                        FB6_EGR_ZONE_3_EDITOR_CONTROL_TCAM,
                                        FB6_EGR_ZONE_4_EDITOR_CONTROL_TCAM,
                                        FB6_FLEX_RTAG7_HASH_TCAM_ONLY,
                                        FB6_FORWARDING_1_LOGICAL_TBL_SEL_TCAM,
                                        FB6_FORWARDING_2_LOGICAL_TBL_SEL_TCAM,
                                        FB6_FORWARDING_3_LOGICAL_TBL_SEL_TCAM,
                                        FB6_IFP_LOGICAL_TABLE_SELECT,
                                        FB6_IP_MULTICAST_TCAM,
                                        FB6_IP_PARSER0_HME_STAGE_TCAM_ONLY_0,
                                        FB6_IP_PARSER1_HME_STAGE_TCAM_ONLY_0,
                                        FB6_IP_PARSER1_HME_STAGE_TCAM_ONLY_1,
                                        FB6_IP_PARSER1_HME_STAGE_TCAM_ONLY_2,
                                        FB6_IP_PARSER1_HME_STAGE_TCAM_ONLY_3,
                                        FB6_IP_PARSER1_HME_STAGE_TCAM_ONLY_4,
                                        FB6_IP_PARSER1_MICE_TCAM_0,
                                        FB6_IP_PARSER1_MICE_TCAM_1,
                                        FB6_IP_PARSER2_HME_STAGE_TCAM_ONLY_0,
                                        FB6_IP_PARSER2_HME_STAGE_TCAM_ONLY_1,
                                        FB6_IP_PARSER2_HME_STAGE_TCAM_ONLY_2,
                                        FB6_IP_PARSER2_HME_STAGE_TCAM_ONLY_3,
                                        FB6_IP_PARSER2_HME_STAGE_TCAM_ONLY_4,
                                        FB6_IP_PARSER2_MICE_TCAM_0,
                                        FB6_IP_PARSER2_MICE_TCAM_1,
                                        FB6_L2_USER_ENTRY,
                                        FB6_L3_TUNNEL,
                                        FB6_MY_STATION_TCAM,
                                        FB6_MY_STATION_TCAM_2,
                                        FB6_PHB_SELECT_TCAM,
                                        FB6_PKT_FLOW_SELECT_TCAM_0,
                                        FB6_PKT_FLOW_SELECT_TCAM_1,
                                        FB6_PKT_FLOW_SELECT_TCAM_2_ENTRY_ONLY,
                                        FB6_SRC_COMPRESSION,
                                        FB6_TUNNEL_ADAPT_1_LOGICAL_TBL_SEL_TCAM,
                                        FB6_TUNNEL_ADAPT_2_LOGICAL_TBL_SEL_TCAM,
                                        FB6_TUNNEL_ADAPT_3_LOGICAL_TBL_SEL_TCAM,
                                        FB6_TUNNEL_ADAPT_4_LOGICAL_TBL_SEL_TCAM,
                                        FB6_UTT_TCAM_POOL0,
                                        FB6_UTT_TCAM_POOL1,
                                        FB6_UTT_TCAM_POOL2,
                                        FB6_UTT_TCAM_POOL3,
                                        FB6_UTT_TCAM_POOL4,
                                        FB6_UTT_TCAM_POOL5,
                                        FB6_UTT_TCAM_POOL6,
                                        FB6_UTT_TCAM_POOL7,
                                        FB6_UTT_TCAM_POOL8,
                                        FB6_UTT_TCAM_POOL9,
                                        FB6_UTT_TCAM_POOL10,
                                        FB6_UTT_TCAM_POOL11,
                                        FB6_UTT_TCAM_POOL12,
                                        FB6_UTT_TCAM_POOL13,
                                        FB6_UTT_TCAM_POOL14,
                                        FB6_UTT_TCAM_POOL15,
                                        FB6_VFP_TCAM,
                                        FB6_VLAN_SUBNET
};
