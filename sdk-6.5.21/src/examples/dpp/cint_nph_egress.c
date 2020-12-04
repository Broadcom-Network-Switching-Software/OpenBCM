/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
* File: cint_nph_egress.c
* Purpose: An example of the port extender mapping. 
*          The following CINT provides a calling sequence example NPH header egress processing.
*
* Calling sequence:
*
*  1. Add the following port configureations to config-sand.bcm
*        ucode_port_13.BCM88675=10GBase-R64.0:core_0.13
*        ucode_port_14.BCM88675=10GBase-R65.0:core_0.14
*        ucode_port_15.BCM88675=10GBase-R68.0:core_0.15
*        ucode_port_16.BCM88675=10GBase-R69.0:core_0.16
*        ucode_port_128.BCM88675=10GBase-R65.1:core_0.128
*        ...
*        ucode_port_166.BCM88675=10GBase-R65.39:core_0.166
*        custom_feature_vendor_custom_pp_port_14=1
*        custom_feature_vendor_customer65=1
*        field_presel_mgmt_advanced_mode=1
*        bcm886xx_rx_use_hw_trap_id=1
*
*        For 2 pass MC replication, add the following SOC properties:
*            ucode_port_168.BCM88675=RCY.0:core_0.168
*            ucode_port_169.BCM88675=RCY.1:core_0.169
*            custom_feature_vendor_custom_pp_port_168=1
*            custom_feature_vendor_custom_pp_port_169=1
*            tm_port_header_type_out_168.BCM88675=TM
*            tm_port_header_type_out_169.BCM88675=TM
*            ucode_port_66.BCM88675=RCY.0:core_1.66
*            ucode_port_67.BCM88675=RCY.1:core_1.67
*            custom_feature_vendor_custom_pp_port_66=1
*            custom_feature_vendor_custom_pp_port_67=1
*            tm_port_header_type_out_66.BCM88675=TM
*            tm_port_header_type_out_67.BCM88675=TM
*        Note:
*            For two pass MC replication, the 1st path replicates packets to recycle ports, NPH header is
*            re-built, UC/MC bit is reset, {ps, output_fp} fields are udpated according to each of the UC
*            destination. {ps, output_fp} is re-built as follows:
*                ps[2] = rcy_port[0]
*                ps[1:0] = CUD[18:17]
*                output_fp[16:0] = CUD[16:0]
*            Thus two ucode ports (odd/even) need to be added, the LSbit is encoded as the MSbit of PS.
*            Those two ports are defined as custom PP port, and the outgoing header type defined as TM.
*  2. Set PP port mapping according to NPH:
*        - call bcm_port_extender_mapping_info_set()
*               user_define_value should be encoded as { NPH.U/M[0], NPH.SubType[3:0], NPH.DelLength[5:0] }
*           The API does two things, map the packet to a PP port according to the key above, 
*           and set outer_header_start according to the DelLength
*  3. Create VLAN port, match key is { NPH.PS(3), NPH.output_fp(17) }:
*        - call bcm_vlan_create()
*  4. Create VPWS tunnel:
*        - call bcm_mpls_port_add()
*  5. Cross-connect VPWS tunnel to the VLAN port:
*        - call bcm_vswitch_cross_connect_add()
*
* Service 1 (AC/NP->PWE):
*           PORT    +    PS         +   output_fp <---->    PWE/AC
*              14     +     7          +     0x1000  <----->     PWE

* Service 2 (PWE/NP->AC):
*           PORT    +    PS         +   output_fp <---->    PWE/AC   
*              32     +     0          +     0x2000  <----->     AC
*
* Traffic:
*  1. port 14 -> port 13
*        - NPH: 00000a0d c00e1000 f00e0000 05200000
*        - Payload:
*              -   00000100 00010010 94000002 81000064 08004500 006a0000 0000fffd 393ec055 0102c000 00010000
*  2. port 14 -> port 13
*        - NPH: 00000a0d de002000 f00e0000 05200000
*        - Payload:
*              -   00000000 00220000 00000011 81000014 88470002 80280001 40140002 81ff0000 01000001 00109400 
*              -   00028100 00640800 4500006a 00000000 fffd393e c0550102 c0000001
*
*
* LIF extension data:
*     { stag_prio_flag[0], ctag_prio_flag[0], stag_status[1:0], ctag_status[1:0], cos_profile[3:0], stag_pcp_dei[3:0], ctag_pri_cfi[3:0], ctag_vid[11:0]}
*
* run: 
*      cint utility/cint_utils_l2.c 
*      cint utility/cint_utils_l3.c 
*      cint ../sand/utility/cint_sand_utils_global.c 
*      cint ../sand/utility/cint_sand_utils_mpls.c
*      cint utility/cint_utils_mpls_port.c 
*      cint utility/cint_utils_multicast.c 
*      cint cint_port_tpid.c 
*      cint cint_advanced_vlan_translation_mode.c 
*      cint cint_mpls_lsr.c 
*      cint cint_vswitch_metro_mp.c  
*      cint cint_vswitch_cross_connect_p2p.c 
*      cint cint_connect_to_np_vpws.c
*      cint cint_nph_egress.c 
*      BCM> cint
*      config_ivec_value_mapping(0, 0);
*      config_ivec_value_mapping(0, 1);
*      config_ivec_value_mapping(0, 2);
*      config_ivec_value_mapping(0, 3);
*
*      service_port_init(ac_port,pw_port,ilk_port);
*      config_port_ingress(0, ac_port);
*
*      data_entry_init(ac_port, 0x1000, 0, pwe_output_fp, pwe_ps, 1, 0);
*      config_ingress_vpws(0, 0);
*      data_entry_init(pw_port, 0x2000, 0, ac_output_fp, ac_ps, 0, 0);
*      config_ingress_vpws(0, 1);
*
*      nph_egr_pmf_grp_info_init(priority);
*      nph_egress_port_out_dir_ext(0);
*      nph_vpws_service_egress_run_with_defaults(0);
*/

struct nph_map_info_s {
    bcm_port_t np_port;
    uint32 uc;
    uint32 main_type;
    uint32 sub_type;
    uint32 del_length;
    bcm_port_t np_pp_port;
};

struct nph_service_info_egress_s {
    bcm_port_t np_port;
    uint32 ps;
    uint32 output_fp;
    bcm_gport_t vlan_port_id;
    bcm_gport_t gport; /* cross connect gport */
};

enum nph_uc_mc_defines_e {
    nph_uc,
    nph_mc,
    nph_nof_uc
};

enum nph_main_type_defines_e {
    nph_uni,
    nph_nni,
    nph_fpga,
    nph_mirror,
    nph_mac_sync,
    nph_nof_main_type,
    nph_lsp_mc_swap = 0,
    nph_lsp_mc_push = 1,
    nph_vpls_mc = 2
};

enum nph_sub_type_defines_e {
    nph_sub_type_vpws,
    nph_sub_type_vpls,
    nph_sub_type_l3vpn,
    nph_sub_type_swap,
    nph_nof_sub_type,
    nph_sub_type_rcy = 0x8, /* use Sub_type[3] as RCY indication */
    nph_sub_type_size = 0x10
};

enum nph_del_len_defines_e {
    nph_del_len_0,
    nph_del_len_18,
    nph_del_len_22,
    nph_del_len_26,
    nph_del_len_30,
    nph_del_len_34,
    nph_nof_del_len,
    nph_del_len_44 = 6 /* nph_nof_del_len */,
    nph_nof_del_len_vpws
};

int nph_del_len[] = { 0, 18, 22, 26, 30, 34, 44 };

enum nph_vlan_status_defines_e {
    nph_vlan_status_none,
    nph_vlan_status_stag,
    nph_vlan_status_ctag,
    nph_vlan_status_s_ctag
};

enum nph_tag_action_defines_e {
    nph_tag_action_keep_0,
    nph_tag_action_keep_1,
    nph_tag_action_untag,
    nph_tag_action_tag
};

enum nph_vlan_editing_pcp_dei_flag_e {
    nph_pcp_dei_flag_non_uniform,
    nph_pcp_dei_flag_uniform
};

enum nph_vlan_editing_ive_cmd_e {
    nph_ive_cmd_nop=0,

    nph_ive_cmd_vs_none_add_c=4, /* VS=0, stag=keep/untag, ctag=tag */
    nph_ive_cmd_vs_none_add_s, /* VS=0, stag=tag, ctag=keep/untag */
    nph_ive_cmd_vs_none_add_s_add_c, /* VS=0, stag=TAG, ctag=TAG */

    nph_ive_cmd_vs_s_add_c, /* VS=1, stag=keep, ctag=tag */
    nph_ive_cmd_vs_s_swap_s_non_uniform, /* VS=1, stag=tag, non-uniform, ctag=keep/untag */
    nph_ive_cmd_vs_s_swap_s_uniform, /* VS=1, stag=tag, uniform, ctag=keep/untag */
    nph_ive_cmd_vs_s_swap_s_non_uniform_add_c, /* VS=1, stag=tag, non-uniform, ctag=tag */
    nph_ive_cmd_vs_s_swap_s_uniform_add_c, /* VS=1, stag=tag, uniform, ctag=tag */
    nph_ive_cmd_vs_s_del_s, /* VS=1, stag=untag, ctag=keep/untag */
    nph_ive_cmd_vs_s_del_s_add_c, /* VS=1, stag=untag, ctag=tag */

    nph_ive_cmd_vs_c_add_s, /* VS=2, stag=keep/untag, ctag=tag, non-uniform */
    nph_ive_cmd_vs_c_swap_c_non_uniform, /* VS=2, stag=keep/untag, ctag=tag, non-uniform */
    nph_ive_cmd_vs_c_swap_c_uniform, /* VS=2, stag=keep/untag, ctag=tag, uniform */
    nph_ive_cmd_vs_c_add_s_swap_c_non_uniform, /* VS=2, stag=tag, ctag=tag, non-uniform */
    nph_ive_cmd_vs_c_add_s_swap_c_uniform, /* VS=2, stag=tag, ctag=tag, uniform */
    nph_ive_cmd_vs_c_del_c, /* VS=2, stag=keep/untag, ctag=untag */
    nph_ive_cmd_vs_c_add_s_del_c, /* VS=2, stag=tag, ctag=untag */

    nph_ive_cmd_vs_s_c_swap_s_non_uniform, /* VS=3, stag=tag, non-uniform ctag=keep */
    nph_ive_cmd_vs_s_c_swap_s_uniform, /* VS=3, stag=tag, uniform ctag=keep */
    nph_ive_cmd_vs_s_c_swap_c_non_uniform, /* VS=3, stag=keep, ctag=tag, non-uniform */
    nph_ive_cmd_vs_s_c_swap_c_uniform, /* VS=3, stag=keep, ctag=tag, uniform */
    nph_ive_cmd_vs_s_c_del_s, /* VS=3, stag=untag, ctag=keep */
    nph_ive_cmd_vs_s_c_del_s_swap_c_non_uniform, /* VS=3, stag=untag, ctag=tag, non-uniform */
    nph_ive_cmd_vs_s_c_del_s_swap_c_uniform, /* VS=3, stag=untag, ctag=tag, uniform */
    nph_ive_cmd_vs_s_c_del_c, /* VS=3, stag=keep, ctag=untag */
    nph_ive_cmd_vs_s_c_swap_s_non_uniform_del_c, /* VS=3, stag=tag, non-uniform, ctag=untag */
    nph_ive_cmd_vs_s_c_swap_s_uniform_del_c, /* VS=3, stag=tag, uniform, ctag=untag */
    nph_ive_cmd_vs_s_c_del_s_del_c, /* VS=3, stag=untag, ctag=untag */
    nph_ive_cmd_vs_s_c_swap_s_non_uniform_swap_c_non_uniform, /* VS=3, stag=tag, non-uniform, ctag=tag, non-uniform */
    nph_ive_cmd_vs_s_c_swap_s_non_uniform_swap_c_uniform, /* VS=3, stag=tag, non-uniform, ctag=tag, uniform */
    nph_ive_cmd_vs_s_c_swap_s_uniform_swap_c_non_uniform, /* VS=3, stag=tag, uniform, ctag=tag, non-uniform */
    nph_ive_cmd_vs_s_c_swap_s_uniform_swap_c_uniform, /* VS=3, stag=tag, uniform, ctag=tag, uniform */
    nph_ive_cmd_nof_cmds /* number of commands */
};

struct nph_egress_vlan_editing_info_s {
    nph_tag_action_defines_e stag_action;
    nph_tag_action_defines_e ctag_action;
    uint32 cos_profile;
    uint32 stag_vid;
    uint32 ctag_vid;
    uint32 stag_pcp_dei;
    uint32 ctag_pri_cfi;
    uint32 stag_pcp_dei_flag;
    uint32 ctag_pri_cfi_flag;
};

struct nph_egress_vlan_editing_ive_action_s {
    bcm_vlan_action_t outer_action;
    bcm_vlan_action_t inner_action;
    bcm_vlan_action_t outer_prio_action;
    bcm_vlan_action_t inner_prio_action;
};

struct nph_egress_vlan_editing_map_s {
    uint32 vlan_status;
    uint32 stag_pcp_dei_flag;
    uint32 ctag_pri_cfi_flag;
    uint32 stag_status;
    uint32 ctag_status;
    uint32 vlan_status_mask;
    uint32 stag_pcp_dei_flag_mask;
    uint32 ctag_pri_cfi_flag_mask;
    uint32 stag_status_mask;
    uint32 ctag_status_mask;
    nph_vlan_editing_ive_cmd_e action;
};

int pwe_ps=7;
int pwe_output_fp=0x1000;
int ac_ps=0;
int ac_output_fp=0x2000;

int ilk_port=14;
int rcy_port[2]= { 168, 169 };

struct nph_egress_logical_port_define_s {
    int initialized;
    int phy_port;
    int nph_mc_pp_port;
    int nph_all_fpga_pp_port;
    int nph_all_mirror_pp_port;
    int nph_all_mac_sync_pp_port;
    int nph_vpws_port[7];
    int nph_vpls_port[6];
    int nph_l3vpn_uni_port[6];
    int nph_l3vpn_nni_port[6];    
    int nph_mpls_port_uni_port[6];
    int nph_mpls_port_nni_port[6];
    int rcy_port[2];
};

nph_egress_logical_port_define_s nph_egress_logical_port_defines[2];
nph_map_info_s nph_map_info[2240];

nph_service_info_egress_s nph_service_egress_info[2];
nph_egress_vlan_editing_info_s nph_egress_vlan_editing_info[2] = {
    { nph_tag_action_tag, nph_tag_action_tag, 0, 100, 200, 0xE, 0, nph_pcp_dei_flag_uniform, nph_pcp_dei_flag_uniform },
    { nph_tag_action_tag, nph_tag_action_tag, 0, 100, 200, 0xE, 0, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform }
};

nph_egress_vlan_editing_ive_action_s nph_egress_vlan_editing_cmd[nph_ive_cmd_nof_cmds-nph_ive_cmd_vs_none_add_c] = {
    /*                              command description                                     *//*                 outer_action           ,                 inner_action             ,             outer_prio_action          ,          inner_prio_action              */ /* nof_tags_to_remove, outer tag src, outer tag prio src, inner tag src, inner tag prio src */
    /* nph_ive_cmd_vs_none_add_c                                                     */ { bcmVlanActionNone         , bcmVlanActionNone         , bcmVlanActionAdd          , bcmVlanActionNone }, /* 0, EDIT 1, mapped, NOT-SUPPORTED by API */
    /* nph_ive_cmd_vs_none_add_s                                                     */ { bcmVlanActionAdd          , bcmVlanActionNone         , bcmVlanActionAdd          , bcmVlanActionNone }, /* 0, EDIT 0, mapped */
    /* nph_ive_cmd_vs_none_add_s_add_c                                           */ { bcmVlanActionAdd          , bcmVlanActionAdd          , bcmVlanActionAdd          , bcmVlanActionAdd }, /* 0, EDIT 0, mapped; EDIT 1, mapped */

    /* nph_ive_cmd_vs_s_add_c                                                           */ { bcmVlanActionReplace      , bcmVlanActionOuterAdd     , bcmVlanActionOuterAdd     , bcmVlanActionAdd }, /* 1, outer tag, outer tag; EDIT 1, mapped */
    /* nph_ive_cmd_vs_s_swap_s_non_uniform                                     */ { bcmVlanActionReplace      , bcmVlanActionNone         , bcmVlanActionOuterAdd     , bcmVlanActionNone }, /* 1, EDIT 0, outer tag; */
    /* nph_ive_cmd_vs_s_swap_s_uniform                                            */ { bcmVlanActionReplace      , bcmVlanActionNone         , bcmVlanActionAdd          , bcmVlanActionNone }, /* 1, EDIT 0, mapped; */
    /* nph_ive_cmd_vs_s_swap_s_non_uniform_add_c                          */ { bcmVlanActionReplace      , bcmVlanActionAdd          , bcmVlanActionOuterAdd     , bcmVlanActionAdd }, /* 1, EDIT 0, outer tag; EDIT 1, mapped */
    /* nph_ive_cmd_vs_s_swap_s_uniform_add_c                                 */ { bcmVlanActionReplace      , bcmVlanActionAdd          , bcmVlanActionAdd          , bcmVlanActionAdd }, /* 1, EDIT 0, mapped; EDIT 1, mapped */
    /* nph_ive_cmd_vs_s_del_s                                                           */ { bcmVlanActionDelete       , bcmVlanActionNone         , bcmVlanActionNone         , bcmVlanActionNone }, /* 1 */
    /* nph_ive_cmd_vs_s_del_s_add_c                                                 */ { bcmVlanActionNone         , bcmVlanActionNone         , bcmVlanActionAdd          , bcmVlanActionNone }, /* 1, EDIT 1, mapped, NOT-SUPPORTED */

    /* nph_ive_cmd_vs_c_add_s                                                         */{ bcmVlanActionAdd           , bcmVlanActionNone         , bcmVlanActionAdd          , bcmVlanActionNone }, /* 1, EDIT 0, mapped */
    /* nph_ive_cmd_vs_c_swap_c_non_uniform                                    */{ bcmVlanActionNone         , bcmVlanActionNone         , bcmVlanActionOuterAdd     , bcmVlanActionNone }, /* 1, EDIT 1, outer tag, NOT-SUPPORTED */
    /* nph_ive_cmd_vs_c_swap_c_uniform                                           */ { bcmVlanActionNone         , bcmVlanActionNone         , bcmVlanActionAdd          , bcmVlanActionNone }, /* 1, EDIT 1, mapped, NOT-SUPPORTED */
    /* nph_ive_cmd_vs_c_add_s_swap_c_non_uniform                          */{ bcmVlanActionReplace      , bcmVlanActionAdd          , bcmVlanActionAdd          , bcmVlanActionOuterAdd }, /* 1, EDIT 0, mapped; EDIT 1, outer tag */
    /* nph_ive_cmd_vs_c_add_s_swap_c_uniform                                 */{ bcmVlanActionReplace      , bcmVlanActionAdd          , bcmVlanActionAdd          , bcmVlanActionAdd }, /* 1, EDIT 0, mapped; EDIT 1, mapped */
    /* nph_ive_cmd_vs_c_del_c                                                           */ { bcmVlanActionDelete       , bcmVlanActionNone         , bcmVlanActionNone         , bcmVlanActionNone }, /* 1 - duplicated */
    /* nph_ive_cmd_vs_c_add_s_del_c                                                 */{ bcmVlanActionReplace      , bcmVlanActionNone         , bcmVlanActionAdd          , bcmVlanActionNone }, /* 1, EDIT 0, mapped; - duplicated */

    /* nph_ive_cmd_vs_s_c_swap_s_non_uniform                                 */ { bcmVlanActionReplace      , bcmVlanActionNone         , bcmVlanActionOuterAdd     , bcmVlanActionNone }, /* 1, EDIT 0, outer tag */
    /* nph_ive_cmd_vs_s_c_swap_s_uniform                                        */ { bcmVlanActionReplace      , bcmVlanActionNone         , bcmVlanActionAdd          , bcmVlanActionNone }, /* 1, EDIT 0, mapped */
    /* nph_ive_cmd_vs_s_c_swap_c_non_uniform                                 */ { bcmVlanActionNone         , bcmVlanActionReplace      , bcmVlanActionOuterAdd     , bcmVlanActionInnerAdd }, /* 2, outer tag, outer tag; EDIT 1, inner tag */
    /* nph_ive_cmd_vs_s_c_swap_c_uniform                                        */ { bcmVlanActionNone         , bcmVlanActionReplace      , bcmVlanActionOuterAdd     , bcmVlanActionAdd }, /* 2, outer tag, outer tag; EDIT 1, mapped */
    /* nph_ive_cmd_vs_s_c_del_s                                                        */ { bcmVlanActionDelete       , bcmVlanActionNone         , bcmVlanActionNone         , bcmVlanActionNone }, /* 1 - duplicated */
    /* nph_ive_cmd_vs_s_c_del_s_swap_c_non_uniform                        */ { bcmVlanActionNone         , bcmVlanActionNone         , bcmVlanActionInnerAdd     , bcmVlanActionNone }, /* 2, EDIT 1, inner tag, NOT-SUPPORTED */
    /* nph_ive_cmd_vs_s_c_del_s_swap_c_uniform                               */ { bcmVlanActionNone         , bcmVlanActionNone         , bcmVlanActionAdd          , bcmVlanActionNone }, /* 2, EDIT 1, mapped, NOT-SUPPORTED */
    /* nph_ive_cmd_vs_s_c_del_c                                                        */ { bcmVlanActionNone         , bcmVlanActionDelete       , bcmVlanActionOuterAdd     , bcmVlanActionNone }, /* 2, outer tag, outer tag */
    /* nph_ive_cmd_vs_s_c_swap_s_non_uniform_del_c                        */ { bcmVlanActionReplace      , bcmVlanActionDelete       , bcmVlanActionOuterAdd     , bcmVlanActionNone }, /* 2, EDIT 0, outer tag */
    /* nph_ive_cmd_vs_s_c_swap_s_uniform_del_c                               */ { bcmVlanActionReplace      , bcmVlanActionDelete       , bcmVlanActionAdd          , bcmVlanActionNone }, /* 2, EDIT 0, mapped */
    /* nph_ive_cmd_vs_s_c_del_s_del_c                                               */ { bcmVlanActionDelete       , bcmVlanActionDelete       , bcmVlanActionNone         , bcmVlanActionNone }, /* 2 */
    /* nph_ive_cmd_vs_s_c_swap_s_non_uniform_swap_c_non_uniform */ { bcmVlanActionReplace      , bcmVlanActionReplace      , bcmVlanActionOuterAdd     , bcmVlanActionInnerAdd }, /* 2, EDIT 0, outer tag; EDIT 1, inner tag */
    /* nph_ive_cmd_vs_s_c_swap_s_non_uniform_swap_c_uniform        */ { bcmVlanActionReplace      , bcmVlanActionReplace      , bcmVlanActionOuterAdd     , bcmVlanActionAdd }, /* 2, EDIT 0, outer tag; EDIT 1, mapped */
    /* nph_ive_cmd_vs_s_c_swap_s_uniform_swap_c_non_uniform        */ { bcmVlanActionReplace      , bcmVlanActionReplace      , bcmVlanActionAdd          , bcmVlanActionInnerAdd }, /* 2, EDIT 0, mapped; EDIT 1, inner tag */
    /* nph_ive_cmd_vs_s_c_swap_s_uniform_swap_c_uniform               */ { bcmVlanActionReplace      , bcmVlanActionReplace      , bcmVlanActionAdd          , bcmVlanActionAdd } /* 2, EDIT 0, mapped; EDIT 1, mapped */
};

uint32 dont_care=0;
uint32 tag_keep_mask=0x2;
uint32 full_mask_2b=0x3;
uint32 full_mask_1b=0x1;

nph_egress_vlan_editing_map_s nph_egress_vlan_editing_map[] = {
    /* vlan_status,     stag_pcp_dei_flag,                                 ctag_pri_cfi_flag,                  stag_status,                      ctag_status,                  vlan_status_mask,  stag_pcp_dei_flag_mask, ctag_pri_cfi_flag_mask, stag_status_mask, ctag_status_mask, action  */
    {      0, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_keep_0, nph_tag_action_tag   ,        3,            dont_care,            dont_care,     tag_keep_mask, full_mask_2b,  nph_ive_cmd_vs_none_add_c},
    {      0, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_untag , nph_tag_action_tag   ,        3,            dont_care,            dont_care,     full_mask_2b , full_mask_2b,  nph_ive_cmd_vs_none_add_c},
    {      0, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_tag   , nph_tag_action_keep_0,        3,            dont_care,            dont_care,     full_mask_2b , tag_keep_mask,  nph_ive_cmd_vs_none_add_s},
    {      0, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_tag   , nph_tag_action_untag ,        3,            dont_care,            dont_care,     full_mask_2b , full_mask_2b ,  nph_ive_cmd_vs_none_add_s},
    {      0, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_tag   , nph_tag_action_tag   ,        3,            dont_care,            dont_care,     full_mask_2b , full_mask_2b ,  nph_ive_cmd_vs_none_add_s_add_c},
    {      0, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_keep_0, nph_tag_action_keep_0,        3,            dont_care,            dont_care,     tag_keep_mask, tag_keep_mask,  nph_ive_cmd_nop},
    {      0, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_untag , nph_tag_action_untag ,        3,            dont_care,            dont_care,     full_mask_2b , full_mask_2b ,  nph_ive_cmd_nop},

    {      1, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_keep_0, nph_tag_action_tag   ,        3,            dont_care,            dont_care,     tag_keep_mask, full_mask_2b ,  nph_ive_cmd_vs_s_add_c},
    {      1, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_tag   , nph_tag_action_keep_0,        3,            full_mask_1b,         dont_care,     full_mask_2b , tag_keep_mask,  nph_ive_cmd_vs_s_swap_s_non_uniform},
    {      1, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_tag   , nph_tag_action_untag ,        3,            full_mask_1b,         dont_care,     full_mask_2b , full_mask_2b ,  nph_ive_cmd_vs_s_swap_s_non_uniform},
    {      1, nph_pcp_dei_flag_uniform    , nph_pcp_dei_flag_non_uniform, nph_tag_action_tag   , nph_tag_action_keep_0,        3,            full_mask_1b,         dont_care,     full_mask_2b , tag_keep_mask,  nph_ive_cmd_vs_s_swap_s_uniform},
    {      1, nph_pcp_dei_flag_uniform    , nph_pcp_dei_flag_non_uniform, nph_tag_action_tag   , nph_tag_action_untag ,        3,            full_mask_1b,         dont_care,     full_mask_2b , full_mask_2b ,  nph_ive_cmd_vs_s_swap_s_uniform},
    {      1, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_tag   , nph_tag_action_tag   ,        3,            full_mask_1b,         dont_care,     full_mask_2b , full_mask_2b ,  nph_ive_cmd_vs_s_swap_s_non_uniform_add_c},
    {      1, nph_pcp_dei_flag_uniform    , nph_pcp_dei_flag_non_uniform, nph_tag_action_tag   , nph_tag_action_tag   ,        3,            full_mask_1b,         dont_care,     full_mask_2b , full_mask_2b ,  nph_ive_cmd_vs_s_swap_s_uniform_add_c},
    {      1, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_untag , nph_tag_action_keep_0,        3,            dont_care   ,         dont_care,     full_mask_2b , tag_keep_mask,  nph_ive_cmd_vs_s_del_s},
    {      1, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_untag , nph_tag_action_untag ,        3,            dont_care   ,         dont_care,     full_mask_2b , full_mask_2b ,  nph_ive_cmd_vs_s_del_s},
    {      1, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_untag , nph_tag_action_tag   ,        3,            dont_care   ,         dont_care,     full_mask_2b , full_mask_2b ,  nph_ive_cmd_vs_s_del_s_add_c},
    {      1, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_keep_0, nph_tag_action_keep_0,        3,            dont_care   ,         dont_care,     tag_keep_mask, tag_keep_mask,  nph_ive_cmd_nop},
    {      1, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_keep_0, nph_tag_action_untag ,        3,            dont_care   ,         dont_care,     tag_keep_mask, full_mask_2b ,  nph_ive_cmd_nop},

    {      2, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_tag   , nph_tag_action_keep_0,        3,            dont_care,            dont_care,     full_mask_2b , tag_keep_mask,  nph_ive_cmd_vs_c_add_s},
    {      2, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_keep_0, nph_tag_action_tag   ,        3,            dont_care   ,         full_mask_1b,  tag_keep_mask, full_mask_2b ,  nph_ive_cmd_vs_c_swap_c_non_uniform},
    {      2, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_untag , nph_tag_action_tag   ,        3,            dont_care   ,         full_mask_1b,  full_mask_2b , full_mask_2b ,  nph_ive_cmd_vs_c_swap_c_non_uniform},
    {      2, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_uniform    , nph_tag_action_keep_0, nph_tag_action_tag   ,        3,            dont_care   ,         full_mask_1b,  tag_keep_mask, full_mask_2b ,  nph_ive_cmd_vs_c_swap_c_uniform},
    {      2, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_uniform    , nph_tag_action_untag , nph_tag_action_tag   ,        3,            dont_care   ,         full_mask_1b,  full_mask_2b , full_mask_2b ,  nph_ive_cmd_vs_c_swap_c_uniform},
    {      2, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_tag   , nph_tag_action_tag   ,        3,            dont_care   ,         full_mask_1b,  full_mask_2b , full_mask_2b ,  nph_ive_cmd_vs_c_add_s_swap_c_non_uniform},
    {      2, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_uniform    , nph_tag_action_tag   , nph_tag_action_tag   ,        3,            dont_care   ,         full_mask_1b,  full_mask_2b , full_mask_2b ,  nph_ive_cmd_vs_c_add_s_swap_c_uniform},    
    {      2, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_untag , nph_tag_action_untag ,        3,            dont_care   ,         dont_care,     full_mask_2b , full_mask_2b ,  nph_ive_cmd_vs_c_del_c},
    {      2, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_tag   , nph_tag_action_untag ,        3,            dont_care   ,         dont_care,     full_mask_2b , full_mask_2b ,  nph_ive_cmd_vs_c_add_s_del_c},
    {      2, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_keep_0, nph_tag_action_keep_0,        3,            dont_care   ,         dont_care,     tag_keep_mask, tag_keep_mask,  nph_ive_cmd_nop},
    {      2, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_untag , nph_tag_action_keep_0,        3,            dont_care   ,         dont_care,     full_mask_2b , tag_keep_mask,  nph_ive_cmd_nop},

    {      3, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_tag   , nph_tag_action_keep_0,        3,            full_mask_1b,         dont_care,     full_mask_2b , tag_keep_mask,  nph_ive_cmd_vs_s_c_swap_s_non_uniform},
    {      3, nph_pcp_dei_flag_uniform    , nph_pcp_dei_flag_non_uniform, nph_tag_action_tag   , nph_tag_action_keep_0,        3,            full_mask_1b,         dont_care   ,  full_mask_2b , tag_keep_mask,  nph_ive_cmd_vs_s_c_swap_s_uniform},    
    {      3, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_keep_0, nph_tag_action_tag   ,        3,            dont_care   ,         full_mask_1b,  tag_keep_mask, full_mask_2b ,  nph_ive_cmd_vs_s_c_swap_c_non_uniform},
    {      3, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_uniform    , nph_tag_action_keep_0, nph_tag_action_tag   ,        3,            dont_care   ,         full_mask_1b,  tag_keep_mask, full_mask_2b ,  nph_ive_cmd_vs_s_c_swap_c_uniform},    
    {      3, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_untag , nph_tag_action_keep_0,        3,            dont_care   ,         dont_care   ,  full_mask_2b , tag_keep_mask,  nph_ive_cmd_vs_s_c_del_s},
    {      3, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_untag , nph_tag_action_tag   ,        3,            dont_care   ,         full_mask_1b,  full_mask_2b , full_mask_2b ,  nph_ive_cmd_vs_s_c_del_s_swap_c_non_uniform},
    {      3, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_uniform    , nph_tag_action_untag , nph_tag_action_tag   ,        3,            dont_care   ,         full_mask_1b,  full_mask_2b , full_mask_2b ,  nph_ive_cmd_vs_s_c_del_s_swap_c_uniform},    
    {      3, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_keep_0, nph_tag_action_untag ,        3,            dont_care   ,         dont_care   ,  tag_keep_mask, full_mask_2b ,  nph_ive_cmd_vs_s_c_del_c},    
    {      3, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_tag   , nph_tag_action_untag ,        3,            full_mask_1b,         dont_care   ,  full_mask_2b , full_mask_2b ,  nph_ive_cmd_vs_s_c_swap_s_non_uniform_del_c},    
    {      3, nph_pcp_dei_flag_uniform    , nph_pcp_dei_flag_non_uniform, nph_tag_action_tag   , nph_tag_action_untag ,        3,            full_mask_1b,         dont_care,     full_mask_2b , full_mask_2b ,  nph_ive_cmd_vs_s_c_swap_s_uniform_del_c},
    {      3, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_untag , nph_tag_action_untag ,        3,            dont_care   ,         dont_care,     full_mask_2b , full_mask_2b ,  nph_ive_cmd_vs_s_c_del_s_del_c},
    
    {      3, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_tag   , nph_tag_action_tag   ,        3,            full_mask_1b,         full_mask_1b,  full_mask_2b , full_mask_2b ,  nph_ive_cmd_vs_s_c_swap_s_non_uniform_swap_c_non_uniform},
    {      3, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_uniform    , nph_tag_action_tag   , nph_tag_action_tag   ,        3,            full_mask_1b,         full_mask_1b,  full_mask_2b , full_mask_2b ,  nph_ive_cmd_vs_s_c_swap_s_non_uniform_swap_c_uniform},
    {      3, nph_pcp_dei_flag_uniform    , nph_pcp_dei_flag_non_uniform, nph_tag_action_tag   , nph_tag_action_tag   ,        3,            full_mask_1b,         full_mask_1b,  full_mask_2b , full_mask_2b ,  nph_ive_cmd_vs_s_c_swap_s_uniform_swap_c_non_uniform},
    {      3, nph_pcp_dei_flag_uniform    , nph_pcp_dei_flag_uniform    , nph_tag_action_tag   , nph_tag_action_tag   ,        3,            full_mask_1b,         full_mask_1b,  full_mask_2b , full_mask_2b ,  nph_ive_cmd_vs_s_c_swap_s_uniform_swap_c_uniform},
    {      3, nph_pcp_dei_flag_non_uniform, nph_pcp_dei_flag_non_uniform, nph_tag_action_keep_0, nph_tag_action_keep_0,        3,            full_mask_1b,         full_mask_1b,  tag_keep_mask, tag_keep_mask,  nph_ive_cmd_nop}
};

int nph_nof_services = 2;
uint32 nph_ps_mask = 7;
uint32 nph_output_fp_mask = 0x1FFFF;
uint32 nph_output_uc_mask = 1;
uint32 nph_sub_type_mask = 0x3;
uint32 nph_del_length_mask = 0x3F;

int nph_egr_pmf_core_bitmap = 3;
int nph_egr_pmf_default_pri_start = 20;
int nph_egr_pmf_default_presel_id_start = 10;

struct nph_egr_pmf_grp_info_s {
    int initialized;
    
    bcm_field_presel_t vpws_presel_id;
    bcm_field_presel_t vpls_presel_id;
    bcm_field_presel_t l3vpn_uni_presel_id;
    bcm_field_presel_t l3vpn_nni_presel_id;
    bcm_field_presel_t mpls_presel_id;
    bcm_field_presel_t misc_presel_id;
    /* we have 3 bits for PMF program selection based on PP port, 
       among which one should be reserved for NIF ports: */
    /* bcm_field_presel_t nif_presel_id; */

    bcm_field_group_t vlan_edit_grp_id;
    bcm_field_presel_set_t vlan_edit_psset;
    int vlan_edit_grp_pri;
    int vs_qual_id;
    int lif_ext_qual_id;
    bcm_field_entry_t vlan_edit_ent[64];

    bcm_field_group_t ctag_ext_grp_id;
    bcm_field_presel_set_t ctag_ext_psset;
    int ctag_ext_grp_pri;
    int ctag_qual_id;
    bcm_field_entry_t ctag_ext_ent;

    bcm_field_group_t vsi_ext_grp_id;
    bcm_field_presel_set_t vsi_ext_psset;
    int vsi_ext_grp_pri;
    int vsi_qual_id;
    bcm_field_entry_t vsi_ent;

    bcm_field_group_t lif_ext_grp_id;
    bcm_field_presel_set_t lif_ext_psset;
    int lif_ext_grp_pri;
    int lif_qual_id;
    bcm_field_entry_t lif_ext_ent;

    bcm_field_group_t lif_orientation_ext_grp_id;
    bcm_field_presel_set_t lif_orientation_psset;
    int lif_orientation_ext_grp_pri;
    int lif_orientation_qual_id;
    bcm_field_entry_t lif_orientation_ext_ent;

    bcm_field_group_t port_out_ext_grp_id;
    bcm_field_presel_set_t port_out_ext_psset;
    int port_out_ext_grp_pri;
    int port_out_qual_id[2];
    bcm_field_entry_t port_out_ext_ent;

    bcm_field_group_t next_hop_ext_grp_id;
    bcm_field_presel_set_t next_hop_psset;
    int next_hop_ext_grp_pri;
    int next_hop_qual_id;
    int next_hop_const_0_qual_id;
    int next_hop_const_1_qual_id;
    bcm_field_entry_t next_hop_ext_ent;

    bcm_field_group_t fwd_code_set_grp_id;
    bcm_field_presel_set_t fwd_code_set_psset;
    int fwd_code_set_grp_pri;
    int fwd_code_sub_type_qual_id; /* Sub_type */
    int fwd_code_b_f_flag_qual_id; /* B_F */
    bcm_field_entry_t fwd_code_ext_ent[5];

    bcm_field_group_t ttl_ext_grp_id;
    bcm_field_presel_set_t ttl_ext_psset;
    int ttl_ext_grp_pri;
    int ttl_qual_id;
    bcm_field_entry_t ttl_ext_ent;

    bcm_field_group_t egr_trap_grp_id;
    bcm_field_presel_set_t egr_trap_psset;
    int egr_trap_grp_pri;
    int egr_trap_const_0_qual_id[2];
    int egr_trap_const_1_qual_id[2];
    bcm_field_entry_t egr_trap_ent;
};

nph_egr_pmf_grp_info_s nph_egr_pmf_grp_info;

int inlifs[] = {4096, 4097};
int inlif_gports[4];
bcm_multicast_t nph_egr_mcast_id = 0x2000;        

/* Init np port info */
int nph_service_info_egress_init(uint32 index, bcm_port_t np_port, uint32 ps, uint32 output_fp, bcm_gport_t gport)
{
    if (index >= nph_nof_services) {
        return BCM_E_PARAM;
    }

    if ((ps & (~nph_ps_mask)) != 0) {
        return BCM_E_PARAM;
    }
    
    if ((output_fp & (~nph_output_fp_mask)) != 0) {
        return BCM_E_PARAM;
    }

    nph_service_egress_info[index].np_port = np_port;
    nph_service_egress_info[index].ps = ps;
    nph_service_egress_info[index].output_fp = output_fp;
    nph_service_egress_info[index].gport = gport;

    return 0;
}

int nph_pp_port_mapping_set(int unit, int core, bcm_port_t np_port, int uc, int main_type, int sub_type, int del_length, bcm_port_t np_pp_port)
{
    int rv = 0;
    int index;
    int32 user_define_value;
    bcm_port_extender_mapping_info_t mapping_info;

    user_define_value = ((uc&0x1)<<13) | ((main_type&0x7)<<10)|((sub_type&0xF) << 6)|(nph_del_len[del_length]&0x3F);

    mapping_info.user_define_value=user_define_value;
    mapping_info.pp_port=np_pp_port;
    mapping_info.phy_port=np_port;

    /* Map the source board and souce port to in pp port */
    rv = bcm_port_extender_mapping_info_set(unit, BCM_PORT_EXTENDER_MAPPING_INGRESS, bcmPortExtenderMappingTypeUserDefineValue, &mapping_info);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_extender_mapping_info_set failed $rv\n");
        return rv;
    }

    /* CINT dees not support multi-dimentional array, calculate the index */
    index = core*nph_nof_uc;
    index += uc*nph_nof_main_type*nph_sub_type_size*nph_nof_del_len_vpws;
    index += main_type*nph_sub_type_size*nph_nof_del_len;
    index += sub_type*nph_nof_del_len;
    index += del_length;

    nph_map_info[index].np_port = np_port;
    nph_map_info[index].uc = uc;
    nph_map_info[index].main_type = main_type;
    nph_map_info[index].sub_type = sub_type;
    nph_map_info[index].del_length = nph_del_len[del_length];
    nph_map_info[index].np_pp_port = np_pp_port;
    

    return rv;
}

/* Get np port mapping */
int nph_pp_port_mapping_get(int unit, int uc, int main_type, int sub_type, int del_length, bcm_port_t np_port, bcm_port_t *np_pp_port)
{
    int rv = 0;
    int32 user_define_value;
    bcm_port_extender_mapping_info_t mapping_info;

    user_define_value = ((uc&0x1)<<13) | ((main_type&0x7)<<10)|((sub_type&0xF) << 6)|(nph_del_len[del_length]&0x3F);

    sal_memset(&mapping_info, 0, sizeof(mapping_info));
    mapping_info.user_define_value=user_define_value;
    mapping_info.phy_port=np_port;

    /* Get the mapped in pp port based on source board and souce port */
    rv = bcm_port_extender_mapping_info_get(unit, BCM_PORT_EXTENDER_MAPPING_INGRESS, bcmPortExtenderMappingTypeUserDefineValue, &mapping_info);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_extender_mapping_info_set failed $rv\n");
        return rv;
    }

    *np_pp_port = mapping_info.pp_port;

    return rv;
}

int nph_pp_port_mapping_init(int unit, int core, int np_port, int pp_port_start, int mc_rcy_port_start)
{
    int i, rv = 0;
    int uc;
    int main_type;
    int sub_type;
    int del_length;
    int32 user_define_value;
    bcm_port_extender_mapping_info_t mapping_info;

    if (nph_egress_logical_port_defines[core].initialized) {
        return BCM_E_NONE;
    }

    nph_egress_logical_port_defines[core].phy_port = np_port;

    /* 1. NPH.M/U = ucast, NPH.Main_type = FPGA/CPU, NPH.Sub_type = x, NPH.del_len = x */

    /* all NPH UC packets to FPGA/CPU map to the same PP port */
    uc = nph_uc;
    main_type = nph_fpga;

    for (sub_type=nph_sub_type_vpws; sub_type<nph_nof_sub_type; sub_type++) {
        for (del_length=nph_del_len_0; del_length<nph_nof_del_len; del_length++) {
            bcm_port_class_set(unit, pp_port_start, bcmPortClassId, np_port);
            nph_pp_port_mapping_set(unit, core, np_port, uc, main_type, sub_type, del_length, pp_port_start);
        }
    }
    nph_egress_logical_port_defines[core].nph_all_fpga_pp_port = pp_port_start;

    /* 2. NPH.M/U = ucast, NPH.Main_type = mirror, NPH.Sub_type = x, NPH.del_len = x */

    /* all NPH UC mirror packets to FPGA/CPU map to the same PP port */
    uc = nph_uc;
    main_type = nph_mirror;

    for (sub_type=nph_sub_type_vpws; sub_type<nph_nof_sub_type; sub_type++) {
        for (del_length=nph_del_len_0; del_length<nph_nof_del_len; del_length++) {
            bcm_port_class_set(unit, pp_port_start, bcmPortClassId, np_port);
            nph_pp_port_mapping_set(unit, core, np_port, uc, main_type, sub_type, del_length, pp_port_start);
        }
    }    
    nph_egress_logical_port_defines[core].nph_all_mirror_pp_port = pp_port_start;

    /* 3. NPH.M/U = ucast, NPH.Main_type = mac entry sync, NPH.Sub_type = x, NPH.del_len = x */

    /* all NPH UC mac sync packets map to the same PP port */
    uc = nph_uc;
    main_type = nph_mac_sync;

    for (sub_type=nph_sub_type_vpws; sub_type<nph_nof_sub_type; sub_type++) {
        for (del_length=nph_del_len_0; del_length<nph_nof_del_len; del_length++) {
            bcm_port_class_set(unit, pp_port_start, bcmPortClassId, np_port);
            nph_pp_port_mapping_set(unit, core, np_port, uc, main_type, sub_type, del_length, pp_port_start);
        }
    }    

    nph_egress_logical_port_defines[core].nph_all_mac_sync_pp_port = pp_port_start;

    /* In this example FPGA/CPU/Mirror/MAC sync packets (not necessarily) resolve to the sampe PP port */
    pp_port_start++;

    /* 4. NPH.M/U = ucast, NPH.Main_type = UNI/NNI, NPH.Sub_type = VPWS, NPH.del_len = del_len */

    /* The payload is Ethernet after terminate NPH and NPH.del_len bytes */
    uc = nph_uc;
    sub_type = nph_sub_type_vpws;
    for (del_length=nph_del_len_0; del_length<nph_nof_del_len_vpws; del_length++) {
        nph_egress_logical_port_defines[core].nph_vpws_port[del_length] = pp_port_start;
        bcm_port_class_set(unit, pp_port_start, bcmPortClassId, np_port);
        for (main_type = nph_uni; main_type<nph_nni+1; main_type++) {
            nph_pp_port_mapping_set(unit, core, np_port, uc, main_type, sub_type, del_length, pp_port_start);
        }
        pp_port_start++;
    }

    /* 5. NPH.M/U = ucast, NPH.Main_type = UNI/NNI, NPH.Sub_type = VPLS, NPH.del_len = del_len */

    /* The payload is Ethernet after terminate NPH and NPH.del_len bytes */
    uc = nph_uc;
    sub_type = nph_sub_type_vpls;
    for (del_length=nph_del_len_0; del_length<nph_nof_del_len; del_length++) {
        nph_egress_logical_port_defines[core].nph_vpls_port[del_length] = pp_port_start;
        bcm_port_class_set(unit, pp_port_start, bcmPortClassId, np_port);
        nph_pp_port_mapping_set(unit, core, np_port, uc, nph_uni, sub_type, del_length, pp_port_start);
        nph_pp_port_mapping_set(unit, core, np_port, uc, nph_nni, sub_type, del_length, pp_port_start);
        /* IPMC in tunnel, after recycle */
        nph_pp_port_mapping_set(unit, core, np_port, uc, nph_lsp_mc_push, nph_sub_type_rcy|sub_type, del_length, pp_port_start);
        /* VPLS MC, after recycle */
        nph_pp_port_mapping_set(unit, core, np_port, uc, nph_vpls_mc, nph_sub_type_rcy|sub_type, del_length, pp_port_start);

        pp_port_start++;
    }

    /* 6. NPH.M/U = ucast, NPH.Main_type = UNI/NNI, NPH.Sub_type = L3VPN, NPH.del_len = del_len */
    uc = nph_uc;
    sub_type = nph_sub_type_l3vpn;
    for (main_type = nph_uni; main_type<nph_nni+1; main_type++) {
        for (del_length=nph_del_len_0; del_length<nph_nof_del_len; del_length++) {
            if (main_type) {
                nph_egress_logical_port_defines[core].nph_l3vpn_nni_port[del_length] = pp_port_start;
            } else {
                nph_egress_logical_port_defines[core].nph_l3vpn_uni_port[del_length] = pp_port_start;
            }
            bcm_port_class_set(unit, pp_port_start, bcmPortClassId, np_port);
            nph_pp_port_mapping_set(unit, core, np_port, uc, main_type, sub_type, del_length, pp_port_start++);
        }
    }

    /* 7. NPH.M/U = ucast, NPH.Main_type = UNI/NNI, NPH.Sub_type = MPLS, NPH.del_len = del_len */
    uc = nph_uc;
    sub_type = nph_sub_type_swap;
    for (del_length=nph_del_len_0; del_length<nph_nof_del_len; del_length++) {
        nph_egress_logical_port_defines[core].nph_mpls_port_uni_port[del_length] = pp_port_start;
        bcm_port_class_set(unit, pp_port_start, bcmPortClassId, np_port);
        nph_pp_port_mapping_set(unit, core, np_port, uc, nph_uni, sub_type, del_length, pp_port_start);
        /* P2MP */
        nph_pp_port_mapping_set(unit, core, np_port, uc, nph_lsp_mc_swap, nph_sub_type_rcy|sub_type, del_length, pp_port_start);
        pp_port_start++;
        
        bcm_port_class_set(unit, pp_port_start, bcmPortClassId, np_port);
        nph_pp_port_mapping_set(unit, core, np_port, uc, nph_nni, sub_type, del_length, pp_port_start);
        nph_egress_logical_port_defines[core].nph_mpls_port_nni_port[del_length] = pp_port_start;
        pp_port_start++;
    }

    /* 8. NPH.M/U = mcast, NPH.Main_type = x, NPH.Sub_type = x, NPH.del_len = x */

    /* all NPH MC packets map to the same PP port, MC packets will be forwarded to RCY ports for 2-pass processing */
    uc = nph_mc;

    for (main_type=nph_uni; main_type<nph_nof_main_type; main_type++) {
        for (sub_type=nph_sub_type_vpws; sub_type<nph_nof_sub_type; sub_type++) {
            for (del_length=nph_del_len_0; del_length<nph_nof_del_len; del_length++) {
                bcm_port_class_set(unit, pp_port_start, bcmPortClassId, np_port);
                nph_pp_port_mapping_set(unit, core, np_port, uc, main_type, sub_type, del_length, pp_port_start);
            }
        }
    }
    nph_egress_logical_port_defines[core].nph_mc_pp_port = pp_port_start++;

    for (i=0; i<2; i++) {
        /* PP port resolution on MC RCY port is the same as NP port. This is to update the port TM profile */
        nph_egress_logical_port_defines[core].rcy_port[i] = mc_rcy_port_start++;
        bcm_port_class_set(unit, nph_egress_logical_port_defines[core].rcy_port[0], bcmPortClassId, np_port);

        int32 user_define_value;
        bcm_port_extender_mapping_info_t mapping_info;

        user_define_value = ((nph_mc&0x1)<<13) | ((nph_uni&0x7)<<10)|((nph_sub_type_vpws&0xF) << 6)|(0&0x3F);

        mapping_info.user_define_value=user_define_value;
        mapping_info.pp_port=nph_egress_logical_port_defines[core].nph_mc_pp_port;
        mapping_info.phy_port=nph_egress_logical_port_defines[core].rcy_port[i];

        /* Map the source board and souce port to in pp port */
        rv = bcm_port_extender_mapping_info_set(unit, BCM_PORT_EXTENDER_MAPPING_INGRESS, bcmPortExtenderMappingTypeUserDefineValue, &mapping_info);
        if (rv != BCM_E_NONE) {
            printf("bcm_port_extender_mapping_info_set failed $rv\n");
            return rv;
        }
    }

    nph_egress_logical_port_defines[core].initialized = 1;

    return rv;
}

/* pre-selector for NPH.M/U = ucast, NPH.Main_type = UNI/NNI, NPH.Sub_type = VPWS, NPH.del_len = x */
int nph_egr_pmf_vpws_preselector_create(int unit, int presel_id)
{
    bcm_pbmp_t pbm;
    bcm_pbmp_t pbm_mask;
    int core;
    int index;
    int rv;
    int presel_flags = BCM_FIELD_QUALIFY_PRESEL;
          
    /* Create a presel entity */
    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        rv = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, presel_id);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_field_presel_create_stage_id\n");
            return rv;
        }
        presel_flags |= BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
    } else {
        rv = bcm_field_presel_create_id(unit, presel_id);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_field_presel_create_id\n");
            return rv;
        }
    }
    
    /* Define the set of in/out ports */
    BCM_PBMP_CLEAR(pbm);

    for (core=0; core<2; core++) {
        if (nph_egr_pmf_core_bitmap & (0x1L << core)) {
            for (index=0; index<sizeof(nph_egress_logical_port_defines[core].nph_vpws_port)/sizeof(nph_egress_logical_port_defines[core].nph_vpws_port[0]); index++) {
                BCM_PBMP_PORT_ADD(pbm, nph_egress_logical_port_defines[core].nph_vpws_port[index]);
            }
        }
    }
      
    for(index=0; index<512; index++) { 
        BCM_PBMP_PORT_ADD(pbm_mask, index); 
    }

    rv = bcm_field_qualify_Stage(unit, presel_id | presel_flags, bcmFieldStageIngress);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_qualify_Stage\n");
        return rv;
    }

    rv = bcm_field_qualify_InPorts(unit, presel_id | presel_flags, pbm, pbm_mask);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_qualify_InPorts\n");
        return rv;
    }

    return rv;
}

/* pre-selector for NPH.M/U = ucast, NPH.Main_type = UNI/NNI, NPH.Sub_type = VPLS, NPH.del_len = x */
int nph_egr_pmf_vpls_preselector_create(int unit, int presel_id)
{
    bcm_pbmp_t pbm;
    bcm_pbmp_t pbm_mask;
    int core;
    int index;
    int rv;
    int presel_flags = BCM_FIELD_QUALIFY_PRESEL;
          
    /* Create a presel entity */
    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        rv = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, presel_id);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_field_presel_create_stage_id\n");
            return rv;
        }
        presel_flags |= BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
    } else {
        rv = bcm_field_presel_create_id(unit, presel_id);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_field_presel_create_id\n");
            return rv;
        }
    }
    
    /* Define the set of in/out ports */
    BCM_PBMP_CLEAR(pbm);

    for (core=0; core<2; core++) {
        if (nph_egr_pmf_core_bitmap & (0x1L << core)) {
            for (index=0; index<sizeof(nph_egress_logical_port_defines[core].nph_vpls_port)/sizeof(nph_egress_logical_port_defines[core].nph_vpls_port[0]); index++) {
                BCM_PBMP_PORT_ADD(pbm, nph_egress_logical_port_defines[core].nph_vpls_port[index]);
            }
        }
    }
      
    for(index=0; index<512; index++) { 
        BCM_PBMP_PORT_ADD(pbm_mask, index); 
    }

    rv = bcm_field_qualify_Stage(unit, presel_id | presel_flags, bcmFieldStageIngress);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_qualify_Stage\n");
        return rv;
    }

    rv = bcm_field_qualify_InPorts(unit, presel_id | presel_flags, pbm, pbm_mask);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_qualify_InPorts\n");
        return rv;
    }

    return rv;
}

/* pre-selector for NPH.M/U = ucast, NPH.Main_type = UNI, NPH.Sub_type = L3VPN, NPH.del_len = x */
int nph_egr_pmf_l3vpn_uni_preselector_create(int unit, int presel_id)
{
    bcm_pbmp_t pbm;
    bcm_pbmp_t pbm_mask;
    int core;
    int index;
    int rv;
    int presel_flags = BCM_FIELD_QUALIFY_PRESEL;
          
    /* Create a presel entity */
    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        rv = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, presel_id);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_field_presel_create_stage_id\n");
            return rv;
        }
        presel_flags |= BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
    } else {
        rv = bcm_field_presel_create_id(unit, presel_id);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_field_presel_create_id\n");
            return rv;
        }
    }
    
    /* Define the set of in/out ports */
    BCM_PBMP_CLEAR(pbm);

    for (core=0; core<2; core++) {
        if (nph_egr_pmf_core_bitmap & (0x1L << core)) {
            for (index=0; index<sizeof(nph_egress_logical_port_defines[core].nph_l3vpn_uni_port)/sizeof(nph_egress_logical_port_defines[core].nph_l3vpn_uni_port[0]); index++) {
                BCM_PBMP_PORT_ADD(pbm, nph_egress_logical_port_defines[core].nph_l3vpn_uni_port[index]);
            }
        }
    }
      
    for(index=0; index<512; index++) { 
        BCM_PBMP_PORT_ADD(pbm_mask, index); 
    }

    rv = bcm_field_qualify_Stage(unit, presel_id | presel_flags, bcmFieldStageIngress);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_qualify_Stage\n");
        return rv;
    }

    rv = bcm_field_qualify_InPorts(unit, presel_id | presel_flags, pbm, pbm_mask);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_qualify_InPorts\n");
        return rv;
    }

    return rv;
}

/* pre-selector for NPH.M/U = ucast, NPH.Main_type = NNI, NPH.Sub_type = L3VPN, NPH.del_len = x */
int nph_egr_pmf_l3vpn_nni_preselector_create(int unit, int presel_id)
{
    bcm_pbmp_t pbm;
    bcm_pbmp_t pbm_mask;
    int core;
    int index;
    int rv;
    int presel_flags = BCM_FIELD_QUALIFY_PRESEL;
          
    /* Create a presel entity */
    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        rv = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, presel_id);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_field_presel_create_stage_id\n");
            return rv;
        }
        presel_flags |= BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
    } else {
        rv = bcm_field_presel_create_id(unit, presel_id);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_field_presel_create_id\n");
            return rv;
        }
    }
    
    /* Define the set of in/out ports */
    BCM_PBMP_CLEAR(pbm);

    for (core=0; core<2; core++) {
        if (nph_egr_pmf_core_bitmap & (0x1L << core)) {
            for (index=0; index<sizeof(nph_egress_logical_port_defines[core].nph_l3vpn_nni_port)/sizeof(nph_egress_logical_port_defines[core].nph_l3vpn_nni_port[0]); index++) {
                BCM_PBMP_PORT_ADD(pbm, nph_egress_logical_port_defines[core].nph_l3vpn_nni_port[index]);
            }
        }
    }
      
    for(index=0; index<512; index++) { 
        BCM_PBMP_PORT_ADD(pbm_mask, index); 
    }

    rv = bcm_field_qualify_Stage(unit, presel_id | presel_flags, bcmFieldStageIngress);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_qualify_Stage\n");
        return rv;
    }

    rv = bcm_field_qualify_InPorts(unit, presel_id| presel_flags, pbm, pbm_mask);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_qualify_InPorts\n");
        return rv;
    }

    return rv;
}

/* pre-selector for NPH.M/U = ucast, NPH.Main_type = UNI/NNI, NPH.Sub_type = MPLS, NPH.del_len = x */
int nph_egr_pmf_mpls_preselector_create(int unit, int presel_id)
{
    bcm_pbmp_t pbm;
    bcm_pbmp_t pbm_mask;
    int core;
    int index;
    int rv;
    int presel_flags = BCM_FIELD_QUALIFY_PRESEL;
          
    /* Create a presel entity */
    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        rv = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, presel_id);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_field_presel_create_stage_id\n");
            return rv;
        }
        presel_flags |= BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
    } else {
        rv = bcm_field_presel_create_id(unit, presel_id);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_field_presel_create_id\n");
            return rv;
        }
    }
    
    /* Define the set of in/out ports */
    BCM_PBMP_CLEAR(pbm);

    for (core=0; core<2; core++) {
        if (nph_egr_pmf_core_bitmap & (0x1L << core)) {
            for (index=0; index<sizeof(nph_egress_logical_port_defines[core].nph_mpls_port_uni_port)/sizeof(nph_egress_logical_port_defines[core].nph_mpls_port_uni_port[0]); index++) {
                BCM_PBMP_PORT_ADD(pbm, nph_egress_logical_port_defines[core].nph_mpls_port_uni_port[index]);
            }
            for (index=0; index<sizeof(nph_egress_logical_port_defines[core].nph_mpls_port_nni_port)/sizeof(nph_egress_logical_port_defines[core].nph_mpls_port_nni_port[0]); index++) {
                BCM_PBMP_PORT_ADD(pbm, nph_egress_logical_port_defines[core].nph_mpls_port_nni_port[index]);
            }
        }
    }
      
    for(index=0; index<512; index++) { 
        BCM_PBMP_PORT_ADD(pbm_mask, index); 
    }

    rv = bcm_field_qualify_Stage(unit, presel_id | presel_flags, bcmFieldStageIngress);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_qualify_Stage\n");
        return rv;
    }

    rv = bcm_field_qualify_InPorts(unit, presel_id | presel_flags, pbm, pbm_mask);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_qualify_InPorts\n");
        return rv;
    }

    return rv;
}

/* pre-selector for NPH.M/U = ucast, NPH.Main_type = x, NPH.Sub_type = MPLS, NPH.del_len = x */
int nph_egr_pmf_misc_preselector_create(int unit, int presel_id)
{
    bcm_pbmp_t pbm;
    bcm_pbmp_t pbm_mask;
    int core;
    int index;
    int rv;

    int nph_mc_pp_port;
    int nph_all_fpga_pp_port;
    int nph_all_mirror_pp_port;
    int nph_all_mac_sync_pp_port;
    int presel_flags = BCM_FIELD_QUALIFY_PRESEL;
          
    /* Create a presel entity */
    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        rv = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, presel_id);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_field_presel_create_stage_id\n");
            return rv;
        }
        presel_flags |= BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
    } else {
        rv = bcm_field_presel_create_id(unit, presel_id);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_field_presel_create_id\n");
            return rv;
        }
    }

    /* Define the set of in/out ports */
    BCM_PBMP_CLEAR(pbm);

    for (core=0; core<2; core++) {
        if (nph_egr_pmf_core_bitmap & (0x1L << core)) {
            BCM_PBMP_PORT_ADD(pbm, nph_egress_logical_port_defines[core].nph_all_fpga_pp_port);
            BCM_PBMP_PORT_ADD(pbm, nph_egress_logical_port_defines[core].nph_all_mirror_pp_port);
            BCM_PBMP_PORT_ADD(pbm, nph_egress_logical_port_defines[core].nph_all_mac_sync_pp_port);
        }
    }
      
    for(index=0; index<512; index++) { 
        BCM_PBMP_PORT_ADD(pbm_mask, index); 
    }

    rv = bcm_field_qualify_Stage(unit, presel_id | presel_flags, bcmFieldStageIngress);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_qualify_Stage\n");
        return rv;
    }

    rv = bcm_field_qualify_InPorts(unit, presel_id | presel_flags, pbm, pbm_mask);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_qualify_InPorts\n");
        return rv;
    }

    return rv;
}

int nph_egr_vlan_edit_command_group_set(int unit)
{
    bcm_field_aset_t  aset;
    bcm_field_group_config_t grp;
    bcm_field_data_qualifier_t data_qual[2];
    int index;
    int rv;

    if (nph_egr_pmf_grp_info.vlan_edit_grp_id != -1) {
        /* PMF group already created */
        return 0;
    }

    /* NPH.VS */
    index=0;
    bcm_field_data_qualifier_t_init(&data_qual[index]);
    data_qual[index].flags = BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
    data_qual[index].offset = 39;
    data_qual[index].length = 2;
    data_qual[index].offset_base = bcmFieldDataOffsetBasePacketStart;
    rv = bcm_field_data_qualifier_create(unit, &data_qual);
    if (rv != BCM_E_NONE) {
        printf("failed to create DATA qualifier for NPH.VS \n");
        return rv;
    }

    /* stag_prio_flag[0], ctag_prio_flag[0], stag_status[1:0], ctag_status[1:0] */
    index=1;
    bcm_field_data_qualifier_t_init(&data_qual[index]);
    data_qual[index].flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
    data_qual[index].offset = 24;
    data_qual[index].length = 6;
    data_qual[index].qualifier = bcmFieldQualifyInVPortWide;
    rv = bcm_field_data_qualifier_create(unit, &data_qual[index]);
    if (rv != BCM_E_NONE) {
        printf("failed to create DATA qualifier for NPH.VS \n");
        return rv;
    }

    bcm_field_group_config_t_init(&grp);
    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);

    for (index=0; index<2; index++) {
        rv = bcm_field_qset_data_qualifier_add(unit, &grp.qset, data_qual[index].qual_id);
        if (rv != BCM_E_NONE) {
            printf("bcm_field_qset_data_qualifier_add failed: %d\n", rv);
            return rv;
        }
    }

    grp.priority = nph_egr_pmf_grp_info.vlan_edit_grp_pri;
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    grp.mode = bcmFieldGroupModeDirect;
    grp.preselset = nph_egr_pmf_grp_info.vlan_edit_psset;

    rv = bcm_field_group_config_create(unit, &grp);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_config_create failed \n");
        return rv;
    }
    
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionVlanActionSetNew);
    
    rv = bcm_field_group_action_set(unit, grp.group, aset);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_action_set failed: %d\n", rv);
        return rv;
    }

    rv = bcm_field_group_install(unit, grp.group);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_install failed: %d\n", rv);
        return rv;
    }

    nph_egr_pmf_grp_info.vlan_edit_grp_id = grp.group;
    nph_egr_pmf_grp_info.vs_qual_id = data_qual[0].qual_id;
    nph_egr_pmf_grp_info.lif_ext_qual_id = data_qual[1].qual_id;
    
    return rv;
}

int nph_egr_vlan_edit_command_entry_install(int unit)
{
    bcm_field_entry_t ent;
    uint8 data;
    uint8 mask;
    int index;
    int rv;

    for (index=0; index<sizeof(nph_egress_vlan_editing_map)/sizeof(nph_egress_vlan_editing_map[0]); index++) {
        rv = bcm_field_entry_create(unit, nph_egr_pmf_grp_info.vlan_edit_grp_id, &ent);
        if (rv != BCM_E_NONE) {
            printf("bcm_field_entry_create failed: %d\n", rv);
            return rv;
        }

        data = nph_egress_vlan_editing_map[index].vlan_status;
        mask = nph_egress_vlan_editing_map[index].vlan_status_mask;
        
        rv = bcm_field_qualify_data(unit, ent, nph_egr_pmf_grp_info.vs_qual_id, &data, &mask, 1);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_qualify_data: vs\n");
            return rv;
        } 

        data = nph_egress_vlan_editing_map[index].stag_pcp_dei_flag;
        mask = nph_egress_vlan_editing_map[index].stag_pcp_dei_flag_mask;
        data <<=1;
        mask <<=1;
        data |= nph_egress_vlan_editing_map[index].ctag_pri_cfi_flag;
        mask |= nph_egress_vlan_editing_map[index].ctag_pri_cfi_flag_mask;
        data <<=2;
        mask <<=2;
        data |= nph_egress_vlan_editing_map[index].stag_status;
        mask |= nph_egress_vlan_editing_map[index].stag_status_mask;
        data <<=2;
        mask <<=2;
        data |= nph_egress_vlan_editing_map[index].ctag_status;
        mask |= nph_egress_vlan_editing_map[index].ctag_status_mask;

        rv = bcm_field_qualify_data(unit, ent, nph_egr_pmf_grp_info.lif_ext_qual_id, &data, &mask, 1);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_qualify_data: lif_ext\n");
            return rv;
        } 

        rv = bcm_field_action_add(unit, ent, bcmFieldActionVlanActionSetNew, nph_egress_vlan_editing_map[index].action, 0);
        if (rv != BCM_E_NONE) {
            printf("bcm_field_action_add failed: %d\n", rv);
            return rv;
        }

        nph_egr_pmf_grp_info.vlan_edit_ent[index] = ent;
    }

    rv = bcm_field_group_install(unit, nph_egr_pmf_grp_info.vlan_edit_grp_id);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_install failed: %d\n", rv);
    }
    
    return rv;
}


int nph_egr_vlan_edit_ctag_ext_group_set(int unit)
{
    bcm_field_aset_t  aset;
    bcm_field_group_config_t grp;
    bcm_field_data_qualifier_t data_qual;
    bcm_field_entry_t ent;
    bcm_field_extraction_field_t ext;
    bcm_field_extraction_action_t extact;
    int rv;

    if (nph_egr_pmf_grp_info.ctag_ext_grp_id != -1) {
        /* PMF group already created */
        return 0;
    }

    /* ctag_vid[11:0] */
    bcm_field_data_qualifier_t_init(&data_qual);
    data_qual.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
    data_qual.offset = 0;
    data_qual.length = 12;
    data_qual.qualifier = bcmFieldQualifyInVPortWide;
    rv = bcm_field_data_qualifier_create(unit, &data_qual);
    if (rv != BCM_E_NONE) {
        printf("failed to create DATA qualifier for NPH.VS \n");
        return rv;
    }

    bcm_field_group_config_t_init(&grp);
    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);

    rv = bcm_field_qset_data_qualifier_add(unit, &grp.qset, data_qual.qual_id);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_qset_data_qualifier_add failed: %d\n", rv);
        return rv;
    }

    grp.priority = nph_egr_pmf_grp_info.ctag_ext_grp_pri;
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    grp.mode = bcmFieldGroupModeDirectExtraction;
    grp.preselset = nph_egr_pmf_grp_info.ctag_ext_psset;

    rv = bcm_field_group_config_create(unit, &grp);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_config_create failed \n");
        return rv;
    }
    
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionInnerVlanNew);
    
    rv = bcm_field_group_action_set(unit, grp.group, aset);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_action_set failed: %d\n", rv);
        return rv;
    }

    rv = bcm_field_entry_create(unit, grp.group, &ent);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_entry_create: $rv\n");
        return rv;
    }

    extact.action = bcmFieldActionInnerVlanNew;
    ext.flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
    ext.bits = 12;
    ext.lsb = 0;
    ext.qualifier = data_qual.qual_id;
    rv = bcm_field_direct_extraction_action_add(unit, ent, extact, 1, &ext);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_field_direct_extraction_action_add: $rv\n");
        return rv;
    }

    rv = bcm_field_group_install(0, grp.group);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_install failed: %d\n", rv);
        return rv;
    }

    nph_egr_pmf_grp_info.ctag_ext_grp_id = grp.group;
    nph_egr_pmf_grp_info.ctag_qual_id = data_qual.qual_id;
    nph_egr_pmf_grp_info.ctag_ext_ent = ent;
    
    return rv;
}

/* direct extract VSI from NPH.VPN_ID */
int nph_egr_vsi_dir_ext(int unit)
{
    bcm_field_aset_t  aset;
    bcm_field_entry_t ent;
    bcm_field_group_config_t grp;
    bcm_field_data_qualifier_t data_qual;
    bcm_field_extraction_field_t ext[2];
    bcm_field_extraction_action_t extact;
    int rv;

    if (nph_egr_pmf_grp_info.vsi_ext_grp_id != -1) {
        /* PMF group already created */
        return 0;
    }

    /* NPH[44:32] VPN ID */
    bcm_field_data_qualifier_t_init(&data_qual);
    data_qual.flags = BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
    data_qual.offset = 84 /* 128-48+44-40 */;
    data_qual.length = 13;
    data_qual.offset_base = bcmFieldDataOffsetBasePacketStart;
    rv = bcm_field_data_qualifier_create(unit, &data_qual);
    if (rv != BCM_E_NONE) {
        printf("failed to create DATA qualifier for NPH.VS \n");
        return rv;
    }

    bcm_field_group_config_t_init(&grp);
    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);

    rv = bcm_field_qset_data_qualifier_add(unit, &grp.qset, data_qual.qual_id);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_qset_data_qualifier_add failed: %d\n", rv);
        return rv;
    }

    grp.priority = nph_egr_pmf_grp_info.vsi_ext_grp_pri;
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    grp.mode = bcmFieldGroupModeDirectExtraction;
    grp.preselset = nph_egr_pmf_grp_info.vsi_ext_psset;

    rv = bcm_field_group_config_create(unit, &grp);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_config_create failed \n");
        return rv;
    }
    
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionVSwitchNew);
    
    rv = bcm_field_group_action_set(unit, grp.group, aset);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_action_set failed: %d\n", rv);
        return rv;
    }

    rv = bcm_field_entry_create(unit, grp.group, &ent);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_entry_create failed: %d\n", rv);
        return rv;
    }
    
    extact.action = bcmFieldActionVSwitchNew;
    extact.bias = 0;
    
    ext[0].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
    ext[0].bits = 13;
    ext[0].lsb = 0;
    ext[0].qualifier = data_qual.qual_id;

    ext[1].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
    ext[1].bits = 3;
    ext[1].lsb = 0;
    ext[1].value = 0;

    rv = bcm_field_direct_extraction_action_add(unit, ent, extact, 2, &ext);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_direct_extraction_action_add failed: %d\n", rv);
        return rv;
    }

    rv = bcm_field_group_install(unit, grp.group);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_install failed: %d\n", rv);
        return rv;
    }

    nph_egr_pmf_grp_info.vsi_ext_grp_id = grp.group;
    nph_egr_pmf_grp_info.vsi_qual_id = data_qual.qual_id;
    nph_egr_pmf_grp_info.vsi_ent = ent;
    
    return rv;
}

/* direct extract system lif from NPH.SRC_FP */
int nph_egr_inlif_dir_ext(int unit)
{
    bcm_field_aset_t  aset;
    bcm_field_entry_t ent;
    bcm_field_group_config_t grp;
    bcm_field_data_qualifier_t data_qual;
    bcm_field_extraction_field_t ext[2];
    bcm_field_extraction_action_t extact;
    int rv;

    if (nph_egr_pmf_grp_info.lif_ext_grp_id != -1) {
        /* PMF group already created */
        return 0;
    }

    /* NPH[16:0], SRC_FP} */
    bcm_field_data_qualifier_t_init(&data_qual);
    data_qual.flags = BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
    data_qual.offset = 104 /* 128-24+16-16 */;
    data_qual.length = 17;
    data_qual.offset_base = bcmFieldDataOffsetBasePacketStart;
    rv = bcm_field_data_qualifier_create(0, &data_qual);
    if (rv != BCM_E_NONE) {
        printf("failed to create DATA qualifier for NPH.VS \n");
        return rv;
    }

    bcm_field_group_config_t_init(&grp);
    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);

    rv = bcm_field_qset_data_qualifier_add(unit, &grp.qset, data_qual.qual_id);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_qset_data_qualifier_add failed: %d\n", rv);
        return rv;
    }

    grp.priority = nph_egr_pmf_grp_info.lif_ext_grp_pri;
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    grp.mode = bcmFieldGroupModeDirectExtraction;
    grp.preselset = nph_egr_pmf_grp_info.lif_ext_psset;

    rv = bcm_field_group_config_create(unit, &grp);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_config_create failed \n");
        return rv;
    }
    
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionIngressGportSet);
    
    rv = bcm_field_group_action_set(unit, grp.group, aset);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_action_set failed: %d\n", rv);
        return rv;
    }

    rv = bcm_field_entry_create(unit, grp.group, &ent);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_entry_create failed: %d\n", rv);
        return rv;
    }
    
    extact.action = bcmFieldActionIngressGportSet;
    extact.bias = 0;
    
    ext[0].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
    ext[0].bits = 17;
    ext[0].lsb = 0;
    ext[0].qualifier = data_qual.qual_id;

    ext[1].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
    ext[1].bits = 1;
    ext[1].lsb = 0;
    ext[1].value = 0;

    rv = bcm_field_direct_extraction_action_add(unit, ent, extact, 2, &ext);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_direct_extraction_action_add failed: %d\n", rv);
        return rv;
    }

    rv = bcm_field_group_install(unit, grp.group);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_install failed: %d\n", rv);
        return rv;
    }

    nph_egr_pmf_grp_info.lif_ext_grp_id = grp.group;
    nph_egr_pmf_grp_info.lif_qual_id = data_qual.qual_id;
    nph_egr_pmf_grp_info.lif_ext_ent = ent;
    
    return rv;
}

/* direct extract NPH.R/L to InLIF.profile[0], NPH.H/S to orientation_is_hub */
int nph_egr_lif_orientation_dir_ext(int unit)
{
    bcm_field_aset_t  aset;
    bcm_field_entry_t ent;
    bcm_field_group_config_t grp;
    bcm_field_data_qualifier_t data_qual;
    bcm_field_extraction_field_t ext[2];
    bcm_field_extraction_action_t extact;
    int rv;

    if (nph_egr_pmf_grp_info.lif_orientation_ext_grp_id != -1) {
        /* PMF group already created */
        return 0;
    }

    /* NPH[63:60], {R/L, H/S, Tag_num} */
    bcm_field_data_qualifier_t_init(&data_qual);
    data_qual.flags = BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
    data_qual.offset = 71 /* 128-64+63-56 */;
    data_qual.length = 4;
    data_qual.offset_base = bcmFieldDataOffsetBasePacketStart;
    rv = bcm_field_data_qualifier_create(unit, &data_qual);
    if (rv != BCM_E_NONE) {
        printf("failed to create DATA qualifier for NPH.VS \n");
        return rv;
    }

    bcm_field_group_config_t_init(&grp);
    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyInterfaceClassVPort);

    rv = bcm_field_qset_data_qualifier_add(unit, &grp.qset, data_qual.qual_id);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_qset_data_qualifier_add failed: %d\n", rv);
        return rv;
    }

    grp.priority = nph_egr_pmf_grp_info.lif_orientation_ext_grp_pri;
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    grp.mode = bcmFieldGroupModeDirectExtraction;
    grp.preselset = nph_egr_pmf_grp_info.lif_orientation_psset;

    rv = bcm_field_group_config_create(unit, &grp);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_config_create failed \n");
        return rv;
    }
    
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionInterfaceClassVPort);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionOrientationSet);
    
    rv = bcm_field_group_action_set(unit, grp.group, aset);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_action_set failed: %d\n", rv);
        return rv;
    }

    rv = bcm_field_entry_create(unit, grp.group, &ent);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_entry_create failed: %d\n", rv);
        return rv;
    }
    
    extact.action = bcmFieldActionOrientationSet;
    extact.bias = 0;
    
    ext[0].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
    ext[0].bits = 1;
    ext[0].lsb = 2;
    ext[0].qualifier = data_qual.qual_id;

    rv = bcm_field_direct_extraction_action_add(unit, ent, extact, 1, &ext);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_direct_extraction_action_add failed: %d\n", rv);
        return rv;
    }
    
    extact.action = bcmFieldActionInterfaceClassVPort;
    extact.bias = 0;
    
    ext[0].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
    ext[0].bits = 1;
    ext[0].lsb = 3;
    ext[0].qualifier = data_qual.qual_id;

    ext[1].flags = 0;
    ext[1].bits  = 3;
    ext[1].lsb   = 1;
    ext[1].qualifier = bcmFieldQualifyInterfaceClassVPort;

    rv = bcm_field_direct_extraction_action_add(unit, ent, extact, 2, &ext);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_direct_extraction_action_add failed: %d\n", rv);
        return rv;
    }

    rv = bcm_field_group_install(unit, grp.group);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_install failed: %d\n", rv);
        return rv;
    }

    nph_egr_pmf_grp_info.lif_orientation_ext_grp_id = grp.group;
    nph_egr_pmf_grp_info.lif_orientation_qual_id = data_qual.qual_id;
    nph_egr_pmf_grp_info.lif_orientation_ext_ent = ent;
    
    return rv;
}

/* direct extract destination system port from NPH.PORT_OUT */
int nph_egr_port_out_dir_ext(int unit)
{
    bcm_field_aset_t  aset;
    bcm_field_entry_t ent;
    bcm_field_group_config_t grp;
    bcm_field_data_qualifier_t data_qual;
    bcm_field_extraction_field_t ext[2];
    bcm_field_extraction_action_t extact;
    uint8 data;
    uint8 mask;
    int rv;

    if (nph_egr_pmf_grp_info.port_out_ext_grp_id != -1) {
        /* PMF group already created */
        return 0;
    }

    bcm_field_group_config_t_init(&grp);
    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);

    /* NPH[123:120], sub_type} */
    bcm_field_data_qualifier_t_init(&data_qual);
    data_qual.flags = BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
    data_qual.offset = 3;
    data_qual.length = 4;
    data_qual.offset_base = bcmFieldDataOffsetBasePacketStart;
    rv = bcm_field_data_qualifier_create(unit, &data_qual);
    if (rv != BCM_E_NONE) {
        printf("failed to create DATA qualifier for NPH.VS \n");
        return rv;
    }

    rv = bcm_field_qset_data_qualifier_add(unit, &grp.qset, data_qual.qual_id);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_qset_data_qualifier_add failed: %d\n", rv);
        return rv;
    }

    nph_egr_pmf_grp_info.port_out_qual_id[0] = data_qual.qual_id;

    /* NPH[58:48], Port_Out} */
    bcm_field_data_qualifier_t_init(&data_qual);
    data_qual.flags = BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
    data_qual.offset = 66;
    data_qual.length = 11;
    data_qual.offset_base = bcmFieldDataOffsetBasePacketStart;
    rv = bcm_field_data_qualifier_create(unit, &data_qual);
    if (rv != BCM_E_NONE) {
        printf("failed to create DATA qualifier for NPH.VS \n");
        return rv;
    }

    rv = bcm_field_qset_data_qualifier_add(unit, &grp.qset, data_qual.qual_id);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_qset_data_qualifier_add failed: %d\n", rv);
        return rv;
    }

    nph_egr_pmf_grp_info.port_out_qual_id[1] = data_qual.qual_id;

    grp.priority = nph_egr_pmf_grp_info.port_out_ext_grp_pri;
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    grp.mode = bcmFieldGroupModeDirectExtraction;
    grp.preselset = nph_egr_pmf_grp_info.port_out_ext_psset;

    rv = bcm_field_group_config_create(unit, &grp);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_config_create failed \n");
        return rv;
    }
    
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionRedirect);
    
    rv = bcm_field_group_action_set(unit, grp.group, aset);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_action_set failed: %d\n", rv);
        return rv;
    }


    rv = bcm_field_entry_create(unit, grp.group, &ent);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_entry_create failed: %d\n", rv);
        return rv;
    }

    data = 0; /* !RCY */
    mask = 0x8 /* nph_sub_type_rcy */;

    rv = bcm_field_qualify_data(unit, ent, nph_egr_pmf_grp_info.port_out_qual_id[0], &data, &mask, 1);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_qualify_data: sub_type\n");
        return rv;
    } 
    
    extact.action = bcmFieldActionRedirect;
    extact.bias = 0;
    
    ext[0].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
    ext[0].bits = 11;
    ext[0].lsb = 0;
    ext[0].qualifier = data_qual.qual_id;

    ext[1].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
    ext[1].bits = 7;
    ext[1].lsb = 0;
    ext[1].value = 0x20;

    rv = bcm_field_direct_extraction_action_add(unit, ent, extact, 2, &ext);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_direct_extraction_action_add failed: %d\n", rv);
        return rv;
    }

    nph_egr_pmf_grp_info.port_out_ext_ent = ent;

    rv = bcm_field_group_install(unit, grp.group);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_install failed: %d\n", rv);
        return rv;
    }

    nph_egr_pmf_grp_info.port_out_ext_grp_id = grp.group;
    
    return rv;
}

/* direct extract EEI from NPH.NHI_index */
int nph_egr_l3vpn_next_hop_ext(int unit)
{
    bcm_field_aset_t  aset;
    bcm_field_entry_t ent;
    bcm_field_group_config_t grp;
    bcm_field_data_qualifier_t data_qual;
    bcm_field_extraction_field_t ext[3];
    bcm_field_extraction_action_t extact;
    int rv;

    if (nph_egr_pmf_grp_info.next_hop_ext_grp_id != -1) {
        /* PMF group already created */
        return 0;
    }

    /* NPH[15:0], NHI_index} */
    bcm_field_data_qualifier_t_init(&data_qual);
    data_qual.flags = BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
    data_qual.offset = 119; /* 112 + 7 */
    data_qual.length = 16;
    data_qual.offset_base = bcmFieldDataOffsetBasePacketStart;
    rv = bcm_field_data_qualifier_create(0, &data_qual);
    if (rv != BCM_E_NONE) {
        printf("failed to create DATA qualifier for NPH.VS \n");
        return rv;
    }

    bcm_field_group_config_t_init(&grp);
    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);

    rv = bcm_field_qset_data_qualifier_add(unit, &grp.qset, data_qual.qual_id);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_qset_data_qualifier_add failed: %d\n", rv);
        return rv;
    }

    nph_egr_pmf_grp_info.next_hop_qual_id = data_qual.qual_id;

    /* 0xC0 = 8'b11 000000 */
    bcm_field_data_qualifier_t_init(&data_qual);
    data_qual.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES;
    data_qual.offset = 0;
    data_qual.length = 6;
    data_qual.qualifier = bcmFieldQualifyConstantZero;
    rv = bcm_field_data_qualifier_create(0, &data_qual);
    if (rv != BCM_E_NONE) {
        printf("failed to create DATA qualifier for NPH.VS \n");
        return rv;
    }

    rv = bcm_field_qset_data_qualifier_add(unit, &grp.qset, data_qual.qual_id);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_qset_data_qualifier_add failed: %d\n", rv);
        return rv;
    }

    nph_egr_pmf_grp_info.next_hop_const_0_qual_id = data_qual.qual_id;

    bcm_field_data_qualifier_t_init(&data_qual);
    data_qual.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES;
    data_qual.offset = 0;
    data_qual.length = 2;
    data_qual.qualifier = bcmFieldQualifyConstantOne;
    rv = bcm_field_data_qualifier_create(0, &data_qual);
    if (rv != BCM_E_NONE) {
        printf("failed to create DATA qualifier for NPH.VS \n");
        return rv;
    }

    rv = bcm_field_qset_data_qualifier_add(unit, &grp.qset, data_qual.qual_id);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_qset_data_qualifier_add failed: %d\n", rv);
        return rv;
    }

    nph_egr_pmf_grp_info.next_hop_const_1_qual_id = data_qual.qual_id;

    grp.priority = nph_egr_pmf_grp_info.next_hop_ext_grp_pri;
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    grp.mode = bcmFieldGroupModeDirectExtraction;
    grp.preselset = nph_egr_pmf_grp_info.next_hop_psset;

    rv = bcm_field_group_config_create(unit, &grp);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_config_create failed \n");
        return rv;
    }
    
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionSystemHeaderSet);
    
    rv = bcm_field_group_action_set(unit, grp.group, aset);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_action_set failed: %d\n", rv);
        return rv;
    }

    rv = bcm_field_entry_create(unit, grp.group, &ent);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_entry_create failed: %d\n", rv);
        return rv;
    }
    
    extact.action = bcmFieldActionSystemHeaderSet;
    extact.bias = 0;

    /* EEI = 0xC0XXXX */
    ext[0].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
    ext[0].bits = 16;
    ext[0].lsb = 0;
    ext[0].qualifier = nph_egr_pmf_grp_info.next_hop_qual_id;

    ext[1].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
    ext[1].bits = 6;
    ext[1].lsb = 0;
    ext[1].qualifier = nph_egr_pmf_grp_info.next_hop_const_0_qual_id;

    ext[2].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
    ext[2].bits = 2;
    ext[2].lsb = 0;
    ext[2].qualifier = nph_egr_pmf_grp_info.next_hop_const_1_qual_id;

    rv = bcm_field_direct_extraction_action_add(unit, ent, extact, 3, &ext);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_direct_extraction_action_add failed: %d\n", rv);
        return rv;
    }

    rv = bcm_field_group_install(unit, grp.group);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_install failed: %d\n", rv);
        return rv;
    }

    nph_egr_pmf_grp_info.next_hop_ext_grp_id = grp.group;
    nph_egr_pmf_grp_info.next_hop_ext_ent = ent;
    
    return rv;
}

/*
* For MPLS-UC or LSP P2MP traffic, forward code & forward offset needs to be configured 
* through PMF. Forwrd code should be set to "MPLS", and forward offset should be set 
* to header 1 offset 0 (header 0 represents NPH + DEL_LEN bytes).
*/
int nph_egr_fwd_code_and_offset_set(int unit) {
    bcm_field_aset_t  aset;
    bcm_field_group_config_t grp;
    bcm_field_entry_t ent;
    uint8 data;
    uint8 mask;
    bcm_field_data_qualifier_t data_qual;
    bcm_field_action_core_config_t config;
    int i;
    int field_0[] = { 0x02, /* 1'b0, 3'bxxx, 4'b0010, L3VPN */
                      0x03, /* 1'b0, 3'bxxx, 4'b0011, SWAP */
                      0x00, /* 1'b0, 3'b00x, 4'b0000, L3VPN<->L2VPN bridging, B_F=1 */
                      0x19, /* 1'b0, 3'b001, 4'b1001, IPMC in tunnel, 2nd pass */
                      0x0B  /* 1'b0, 3'b000, 4'b1011, P2MP, 2nd pass */
        };
    int mask_0[] = { 0x8F, 0x8F, 0xEF, 0xFF, 0xFF};
    int b_f_flag[] = { 0, 0, 1, 0, 0};
    int fwd_code[] = { bcmFieldForwardingTypeIp4Ucast, bcmFieldForwardingTypeMpls, bcmFieldForwardingTypeIp4Ucast,
        bcmFieldForwardingTypeIp4Ucast/*bcmFieldForwardingTypeIp4Mcast*/, bcmFieldForwardingTypeMpls};
    int rv;

    if (nph_egr_pmf_grp_info.fwd_code_set_grp_id != -1) {
        /* PMF group already created */
        return 0;
    }

    /* NPH[127:120], sub_type} */
    bcm_field_data_qualifier_t_init(&data_qual);
    data_qual.flags = BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
    data_qual.offset = 7;
    data_qual.length = 8;
    data_qual.offset_base = bcmFieldDataOffsetBasePacketStart;
    rv = bcm_field_data_qualifier_create(unit, &data_qual);
    if (rv != BCM_E_NONE) {
        printf("failed to create DATA qualifier for NPH.VS \n");
        return rv;
    }

    bcm_field_group_config_t_init(&grp);
    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);

    rv = bcm_field_qset_data_qualifier_add(unit, &grp.qset, data_qual.qual_id);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_qset_data_qualifier_add failed: %d\n", rv);
        return rv;
    }

    nph_egr_pmf_grp_info.fwd_code_sub_type_qual_id = data_qual.qual_id;

    /* NPH[59], B_F} */
    bcm_field_data_qualifier_t_init(&data_qual);
    data_qual.flags = BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
    data_qual.offset = 67;
    data_qual.length = 1;
    data_qual.offset_base = bcmFieldDataOffsetBasePacketStart;
    rv = bcm_field_data_qualifier_create(unit, &data_qual);
    if (rv != BCM_E_NONE) {
        printf("failed to create DATA qualifier for NPH.VS \n");
        return rv;
    }

    rv = bcm_field_qset_data_qualifier_add(unit, &grp.qset, data_qual.qual_id);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_qset_data_qualifier_add failed: %d\n", rv);
        return rv;
    }

    nph_egr_pmf_grp_info.fwd_code_b_f_flag_qual_id = data_qual.qual_id;

    grp.priority = nph_egr_pmf_grp_info.fwd_code_set_grp_pri;
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    grp.mode = bcmFieldGroupModeAuto;
    grp.preselset = nph_egr_pmf_grp_info.fwd_code_set_psset;
    
    rv = bcm_field_group_config_create(unit, &grp);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_config_create failed \n");
        return rv;
    }

    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionForwardingTypeNew);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionTtlSet);

    rv = bcm_field_group_action_set(unit, grp.group, aset);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_action_set failed $rv\n");
        return rv;
    }

    for (i=0; i<sizeof(field_0)/sizeof(field_0[0]); i++) {
        rv = bcm_field_entry_create(unit, grp.group, &ent);
        if (rv != BCM_E_NONE) {
            printf("bcm_field_entry_create failed $rv\n");
            return rv;
        }

        data = field_0[i];
        mask = mask_0[i];

        rv = bcm_field_qualify_data(unit, ent, nph_egr_pmf_grp_info.fwd_code_sub_type_qual_id, &data, &mask, 1);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_qualify_data: sub_type\n");
            return rv;
        } 

        if (b_f_flag[i] != 0) {
            data = 1;
            mask = 1;
            
            rv = bcm_field_qualify_data(unit, ent, nph_egr_pmf_grp_info.fwd_code_b_f_flag_qual_id, &data, &mask, 1);
            if (rv != BCM_E_NONE) {
                printf("Error in bcm_field_qualify_data: B_F flag\n");
                return rv;
            } 
        }

        config.param0 = fwd_code[i];
        config.param1 = 1; /* forwarding header index */
        config.param2 = 0; /* forwarding offset in the header */

        rv = bcm_field_action_config_add(unit, ent, bcmFieldActionForwardingTypeNew, 1, &config);
        if (rv != BCM_E_NONE) {
            printf("bcm_field_action_config_add failed $rv\n");
            return rv;
        }

        if (fwd_code[i] == bcmFieldForwardingTypeIp4Ucast) {
            rv = bcm_field_action_add(unit, ent, bcmFieldActionTtlSet, 255, 0);
            if (rv != BCM_E_NONE) {
                printf("bcm_field_action_add failed $rv\n");
                return rv;
            }
        }

        nph_egr_pmf_grp_info.fwd_code_ext_ent[i] = ent;
    }
    
    rv = bcm_field_group_install(unit, grp.group);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_install failed $rv\n");
        return rv;
    }

    nph_egr_pmf_grp_info.fwd_code_set_grp_id = grp.group;

    return rv;
}

/*
* Direct extrat TTL for L3VPN/L3VPN<->L2VPN bridging/IPMC in tunnel
*/
int nph_egr_ttl_dir_ext(int unit) {
    bcm_field_aset_t  aset;
    bcm_field_entry_t ent;
    bcm_field_group_config_t grp;
    bcm_field_data_qualifier_t data_qual;
    bcm_field_extraction_field_t ext[2];
    bcm_field_extraction_action_t extact;
    int rv;

    if (nph_egr_pmf_grp_info.ttl_ext_grp_id != -1) {
        /* PMF group already created */
        return 0;
    }

    /* NPH[63:60], {R/L, H/S, Tag_num} */
    bcm_field_data_qualifier_t_init(&data_qual);
    data_qual.flags = 0; /*BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES */
    data_qual.offset = 8;
    data_qual.length = 1;
    data_qual.offset_base = bcmFieldDataOffsetBaseL2Header; /* the header after NPH+del_len */
    rv = bcm_field_data_qualifier_create(unit, &data_qual);
    if (rv != BCM_E_NONE) {
        printf("failed to create DATA qualifier for NPH.VS \n");
        return rv;
    }

    bcm_field_group_config_t_init(&grp);
    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);

    rv = bcm_field_qset_data_qualifier_add(unit, &grp.qset, data_qual.qual_id);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_qset_data_qualifier_add failed: %d\n", rv);
        return rv;
    }

    grp.priority = nph_egr_pmf_grp_info.ttl_ext_grp_pri;
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    grp.mode = bcmFieldGroupModeDirectExtraction;
    grp.preselset = nph_egr_pmf_grp_info.ttl_ext_psset;

    rv = bcm_field_group_config_create(unit, &grp);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_config_create failed \n");
        return rv;
    }
    
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionTtlSet);
    
    rv = bcm_field_group_action_set(unit, grp.group, aset);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_action_set failed: %d\n", rv);
        return rv;
    }

    rv = bcm_field_entry_create(unit, grp.group, &ent);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_entry_create failed: %d\n", rv);
        return rv;
    }
    
    extact.action = bcmFieldActionTtlSet;
    extact.bias = 0;
    
    ext[0].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
    ext[0].bits = 8;
    ext[0].lsb = 0;
    ext[0].qualifier = data_qual.qual_id;

    rv = bcm_field_direct_extraction_action_add(unit, ent, extact, 1, &ext);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_direct_extraction_action_add failed: %d\n", rv);
        return rv;
    }

    rv = bcm_field_group_install(unit, grp.group);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_install failed: %d\n", rv);
        return rv;
    }

    nph_egr_pmf_grp_info.ttl_ext_grp_id = grp.group;
    nph_egr_pmf_grp_info.ttl_qual_id = data_qual.qual_id;
    nph_egr_pmf_grp_info.ttl_ext_ent = ent;
    
    return rv;
}

/*
* SOC properties:
*     bcm886xx_rx_use_hw_trap_id=1
*
* Set TRAP information if Main_type == 2/3/4.
*     { Qualifier[15:0], Strength[2:0], Code[7:0] } = { 16'b0, 3'b111, 8'b10111111 }
*
* trap_id can be any ununsed traps (refer to 886XX-AN101). Use 0xBF here for easy construction.
*/
int nph_egr_pmf_trap_to_cpu(int unit) {
    bcm_field_aset_t  aset;
    bcm_field_entry_t ent;
    bcm_field_group_config_t grp;
    bcm_field_data_qualifier_t data_qual;
    bcm_field_extraction_field_t ext[4];
    bcm_field_extraction_action_t extact;
    uint8 data = 0;
    uint8 mask = 0x7;
    bcm_rx_trap_config_t trap_config;
    bcm_rx_trap_t type;
    int trap_id = 0xBF;
    int rv;

    if (nph_egr_pmf_grp_info.egr_trap_grp_id != -1) {
        /* PMF group already created */
        return 0;
    }

    rv = bcm_rx_trap_type_create(unit, BCM_RX_TRAP_WITH_ID, bcmRxTrapUserDefine, &trap_id);
    if (rv != BCM_E_NONE) {
        printf("Error in: bcm_rx_trap_type_create rv %d\n", rv);
        return rv;
    }
    
    sal_memset(&trap_config, 0, sizeof(trap_config));
    trap_config.flags = (BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP | BCM_RX_TRAP_REPLACE);
    trap_config.trap_strength = 7;
    trap_config.dest_port = BCM_GPORT_LOCAL_CPU; /* should be the FPGA or CPU port */

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_NONE) {
        printf("Error in : bcm_rx_trap_set returned %d\n", rv);
        return rv;
    }

    bcm_field_group_config_t_init(&grp);
    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);

    /* 6bits constant one */
    bcm_field_data_qualifier_t_init(&data_qual);
    data_qual.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES;
    data_qual.offset = 0;
    data_qual.length = 6;
    data_qual.qualifier = bcmFieldQualifyConstantOne;
    rv = bcm_field_data_qualifier_create(0, &data_qual);
    if (rv != BCM_E_NONE) {
        printf("failed to create DATA qualifier for NPH.VS \n");
        return rv;
    }

    rv = bcm_field_qset_data_qualifier_add(unit, &grp.qset, data_qual.qual_id);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_qset_data_qualifier_add failed: %d\n", rv);
        return rv;
    }

    nph_egr_pmf_grp_info.egr_trap_const_1_qual_id[0] = data_qual.qual_id;

    /* 1bits constant 0 */
    bcm_field_data_qualifier_t_init(&data_qual);
    data_qual.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES;
    data_qual.offset = 0;
    data_qual.length = 1;
    data_qual.qualifier = bcmFieldQualifyConstantZero;
    rv = bcm_field_data_qualifier_create(0, &data_qual);
    if (rv != BCM_E_NONE) {
        printf("failed to create DATA qualifier for NPH.VS \n");
        return rv;
    }

    rv = bcm_field_qset_data_qualifier_add(unit, &grp.qset, data_qual.qual_id);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_qset_data_qualifier_add failed: %d\n", rv);
        return rv;
    }

    nph_egr_pmf_grp_info.egr_trap_const_0_qual_id[0] = data_qual.qual_id;

    /* 5bits constant one */
    bcm_field_data_qualifier_t_init(&data_qual);
    data_qual.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES;
    data_qual.offset = 0;
    data_qual.length = 4;
    data_qual.qualifier = bcmFieldQualifyConstantOne;
    rv = bcm_field_data_qualifier_create(0, &data_qual);
    if (rv != BCM_E_NONE) {
        printf("failed to create DATA qualifier for NPH.VS \n");
        return rv;
    }

    rv = bcm_field_qset_data_qualifier_add(unit, &grp.qset, data_qual.qual_id);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_qset_data_qualifier_add failed: %d\n", rv);
        return rv;
    }

    nph_egr_pmf_grp_info.egr_trap_const_1_qual_id[1] = data_qual.qual_id;

    /* 16bits constant 0 */
    bcm_field_data_qualifier_t_init(&data_qual);
    data_qual.flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES;
    data_qual.offset = 0;
    data_qual.length = 16;
    data_qual.qualifier = bcmFieldQualifyConstantZero;
    rv = bcm_field_data_qualifier_create(0, &data_qual);
    if (rv != BCM_E_NONE) {
        printf("failed to create DATA qualifier for NPH.VS \n");
        return rv;
    }

    rv = bcm_field_qset_data_qualifier_add(unit, &grp.qset, data_qual.qual_id);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_qset_data_qualifier_add failed: %d\n", rv);
        return rv;
    }

    nph_egr_pmf_grp_info.egr_trap_const_0_qual_id[1] = data_qual.qual_id;

    grp.priority = nph_egr_pmf_grp_info.egr_trap_grp_pri;
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    grp.mode = bcmFieldGroupModeDirectExtraction;
    grp.preselset = nph_egr_pmf_grp_info.egr_trap_psset;

    rv = bcm_field_group_config_create(unit, &grp);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_config_create failed \n");
        return rv;
    }
    
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionTrap);
    
    rv = bcm_field_group_action_set(unit, grp.group, aset);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_action_set failed: %d\n", rv);
        return rv;
    }

    rv = bcm_field_entry_create(unit, grp.group, &ent);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_entry_create failed: %d\n", rv);
        return rv;
    }
    
    extact.action = bcmFieldActionTrap;
    extact.bias = 0;

    /* {16'b0, 3'b111, 8'b10111111} */
    ext[0].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
    ext[0].bits = 6;
    ext[0].lsb = 0;
    ext[0].qualifier = nph_egr_pmf_grp_info.egr_trap_const_1_qual_id[0];

    ext[1].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
    ext[1].bits = 1;
    ext[1].lsb = 0;
    ext[1].qualifier = nph_egr_pmf_grp_info.egr_trap_const_0_qual_id[0];

    ext[2].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
    ext[2].bits = 4;
    ext[2].lsb = 0;
    ext[2].qualifier = nph_egr_pmf_grp_info.egr_trap_const_1_qual_id[1];

    ext[3].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
    ext[3].bits = 16;
    ext[3].lsb = 0;
    ext[3].qualifier = nph_egr_pmf_grp_info.egr_trap_const_0_qual_id[1];

    rv = bcm_field_direct_extraction_action_add(unit, ent, extact, 4, &ext);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_direct_extraction_action_add failed: %d\n", rv);
        return rv;
    }

    rv = bcm_field_group_install(unit, grp.group);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_install failed: %d\n", rv);
        return rv;
    }

    nph_egr_pmf_grp_info.egr_trap_grp_id = grp.group;
    nph_egr_pmf_grp_info.egr_trap_ent = ent;
    
    return rv;
}

int nph_egr_pmf_init(int unit, int priority_start, int presel_id_start)
{
    int priority = priority_start;
    int rv;
    
    if (nph_egr_pmf_grp_info.initialized) {
        return BCM_E_NONE;
    }
    
    nph_egr_pmf_grp_info.initialized = 1;
    nph_egr_pmf_grp_info.vlan_edit_grp_id = -1;
    nph_egr_pmf_grp_info.ctag_ext_grp_id = -1;
    nph_egr_pmf_grp_info.vsi_ext_grp_id = -1;
    nph_egr_pmf_grp_info.lif_ext_grp_id = -1;
    nph_egr_pmf_grp_info.lif_orientation_ext_grp_id = -1;
    nph_egr_pmf_grp_info.port_out_ext_grp_id = -1;
    nph_egr_pmf_grp_info.fwd_code_set_grp_id = -1;
    nph_egr_pmf_grp_info.ttl_ext_grp_id = -1;
    nph_egr_pmf_grp_info.next_hop_ext_grp_id = -1;
    nph_egr_pmf_grp_info.egr_trap_grp_id = -1;

    nph_egr_pmf_grp_info.vlan_edit_grp_pri = priority++;
    nph_egr_pmf_grp_info.ctag_ext_grp_pri = priority++;
    nph_egr_pmf_grp_info.vsi_ext_grp_pri = priority++;
    nph_egr_pmf_grp_info.lif_ext_grp_pri = priority++;
    nph_egr_pmf_grp_info.lif_orientation_ext_grp_pri = priority++;
    nph_egr_pmf_grp_info.port_out_ext_grp_pri = priority++;
    nph_egr_pmf_grp_info.fwd_code_set_grp_pri = priority++;
    nph_egr_pmf_grp_info.ttl_ext_grp_pri = priority++;
    nph_egr_pmf_grp_info.next_hop_ext_grp_pri = priority++;
    nph_egr_pmf_grp_info.egr_trap_grp_pri = priority++;

    nph_egr_pmf_grp_info.vpws_presel_id = presel_id_start++;
    nph_egr_pmf_vpws_preselector_create(unit, nph_egr_pmf_grp_info.vpws_presel_id);
    nph_egr_pmf_grp_info.vpls_presel_id = presel_id_start++;
    nph_egr_pmf_vpls_preselector_create(unit, nph_egr_pmf_grp_info.vpls_presel_id);
    nph_egr_pmf_grp_info.l3vpn_uni_presel_id = presel_id_start++;
    nph_egr_pmf_l3vpn_uni_preselector_create(unit, nph_egr_pmf_grp_info.l3vpn_uni_presel_id);
    nph_egr_pmf_grp_info.l3vpn_nni_presel_id = presel_id_start++;
    nph_egr_pmf_l3vpn_nni_preselector_create(unit, nph_egr_pmf_grp_info.l3vpn_nni_presel_id);
    nph_egr_pmf_grp_info.mpls_presel_id = presel_id_start++;
    nph_egr_pmf_mpls_preselector_create(unit, nph_egr_pmf_grp_info.mpls_presel_id);
    nph_egr_pmf_grp_info.misc_presel_id = presel_id_start++;
    nph_egr_pmf_misc_preselector_create(unit, nph_egr_pmf_grp_info.misc_presel_id);

    /* selected when payload is ETH after termination */
    BCM_FIELD_PRESEL_INIT(nph_egr_pmf_grp_info.vlan_edit_psset);
    BCM_FIELD_PRESEL_ADD(nph_egr_pmf_grp_info.vlan_edit_psset, nph_egr_pmf_grp_info.vpws_presel_id);      
    BCM_FIELD_PRESEL_ADD(nph_egr_pmf_grp_info.vlan_edit_psset, nph_egr_pmf_grp_info.vpls_presel_id);      

    /* ctag_ext_psset should be same as vlan_edit_psset */
    BCM_FIELD_PRESEL_INIT(nph_egr_pmf_grp_info.ctag_ext_psset);
    BCM_FIELD_PRESEL_ADD(nph_egr_pmf_grp_info.ctag_ext_psset, nph_egr_pmf_grp_info.vpws_presel_id);      
    BCM_FIELD_PRESEL_ADD(nph_egr_pmf_grp_info.ctag_ext_psset, nph_egr_pmf_grp_info.vpls_presel_id);      

    /* VSI extraction, for VPLS & L3VPN */
    BCM_FIELD_PRESEL_INIT(nph_egr_pmf_grp_info.vsi_ext_psset);
    BCM_FIELD_PRESEL_ADD(nph_egr_pmf_grp_info.vsi_ext_psset, nph_egr_pmf_grp_info.vpls_presel_id);      
    BCM_FIELD_PRESEL_ADD(nph_egr_pmf_grp_info.vsi_ext_psset, nph_egr_pmf_grp_info.vpws_presel_id);      
    /* VRF */
    BCM_FIELD_PRESEL_ADD(nph_egr_pmf_grp_info.vsi_ext_psset, nph_egr_pmf_grp_info.l3vpn_uni_presel_id);      
    BCM_FIELD_PRESEL_ADD(nph_egr_pmf_grp_info.vsi_ext_psset, nph_egr_pmf_grp_info.l3vpn_nni_presel_id);      

    /* LIF extraction, for VPLS */
    BCM_FIELD_PRESEL_INIT(nph_egr_pmf_grp_info.lif_ext_psset);
    BCM_FIELD_PRESEL_ADD(nph_egr_pmf_grp_info.lif_ext_psset, nph_egr_pmf_grp_info.vpls_presel_id);      
    
    /* LIF orientation extraction, for VPWS/VPLS/L3VPN/MPLS */
    BCM_FIELD_PRESEL_INIT(nph_egr_pmf_grp_info.lif_ext_psset);
    BCM_FIELD_PRESEL_ADD(nph_egr_pmf_grp_info.lif_ext_psset, nph_egr_pmf_grp_info.vpws_presel_id);      
    BCM_FIELD_PRESEL_ADD(nph_egr_pmf_grp_info.lif_ext_psset, nph_egr_pmf_grp_info.vpls_presel_id);      
    BCM_FIELD_PRESEL_ADD(nph_egr_pmf_grp_info.lif_ext_psset, nph_egr_pmf_grp_info.l3vpn_uni_presel_id);      
    BCM_FIELD_PRESEL_ADD(nph_egr_pmf_grp_info.lif_ext_psset, nph_egr_pmf_grp_info.l3vpn_nni_presel_id);      
    BCM_FIELD_PRESEL_ADD(nph_egr_pmf_grp_info.lif_ext_psset, nph_egr_pmf_grp_info.mpls_presel_id);      
    
    /* port_out extraction, for VPWS/VPLS/L3VPN/MPLS */
    BCM_FIELD_PRESEL_INIT(nph_egr_pmf_grp_info.port_out_ext_psset);
    BCM_FIELD_PRESEL_ADD(nph_egr_pmf_grp_info.port_out_ext_psset, nph_egr_pmf_grp_info.vpws_presel_id);      
    BCM_FIELD_PRESEL_ADD(nph_egr_pmf_grp_info.port_out_ext_psset, nph_egr_pmf_grp_info.vpls_presel_id);      
    BCM_FIELD_PRESEL_ADD(nph_egr_pmf_grp_info.port_out_ext_psset, nph_egr_pmf_grp_info.l3vpn_uni_presel_id);      
    BCM_FIELD_PRESEL_ADD(nph_egr_pmf_grp_info.port_out_ext_psset, nph_egr_pmf_grp_info.l3vpn_nni_presel_id);      
    BCM_FIELD_PRESEL_ADD(nph_egr_pmf_grp_info.port_out_ext_psset, nph_egr_pmf_grp_info.mpls_presel_id);      
    
    /* fwd_code extraction, for MPLS/L3VPN/L3VPN<->L2VPN bridging/IPMC in tunnel */
    BCM_FIELD_PRESEL_INIT(nph_egr_pmf_grp_info.fwd_code_set_psset);
    BCM_FIELD_PRESEL_ADD(nph_egr_pmf_grp_info.fwd_code_set_psset, nph_egr_pmf_grp_info.l3vpn_uni_presel_id);      
    BCM_FIELD_PRESEL_ADD(nph_egr_pmf_grp_info.fwd_code_set_psset, nph_egr_pmf_grp_info.l3vpn_nni_presel_id);      
    BCM_FIELD_PRESEL_ADD(nph_egr_pmf_grp_info.fwd_code_set_psset, nph_egr_pmf_grp_info.mpls_presel_id);      
    /* L3VPN<->L2VPN bridging */
    BCM_FIELD_PRESEL_ADD(nph_egr_pmf_grp_info.fwd_code_set_psset, nph_egr_pmf_grp_info.vpws_presel_id);      
    /* IPMC in tunnel */
    BCM_FIELD_PRESEL_ADD(nph_egr_pmf_grp_info.fwd_code_set_psset, nph_egr_pmf_grp_info.vpls_presel_id);      

    /* TTL extraction for routed packets */
    nph_egr_pmf_grp_info.ttl_ext_psset = nph_egr_pmf_grp_info.fwd_code_set_psset;
    
    /* Next hop index extraction, for VPWS/L3VPN UNI */
    BCM_FIELD_PRESEL_INIT(nph_egr_pmf_grp_info.next_hop_psset);
    BCM_FIELD_PRESEL_ADD(nph_egr_pmf_grp_info.next_hop_psset, nph_egr_pmf_grp_info.vpws_presel_id);      
    BCM_FIELD_PRESEL_ADD(nph_egr_pmf_grp_info.next_hop_psset, nph_egr_pmf_grp_info.l3vpn_uni_presel_id);      
    
    /* Egress trap, for FPGA/CPU/Mirror/MAC sync */
    BCM_FIELD_PRESEL_INIT(nph_egr_pmf_grp_info.egr_trap_psset);
    BCM_FIELD_PRESEL_ADD(nph_egr_pmf_grp_info.egr_trap_psset, nph_egr_pmf_grp_info.misc_presel_id);      

    if ((rv=nph_egr_pmf_trap_to_cpu(unit)) != BCM_E_NONE) {
        return rv;
    }

    if ((rv=nph_egr_vlan_edit_command_group_set(unit)) != BCM_E_NONE) {
        return rv;
    }
    
    if ((rv=nph_egr_vlan_edit_command_entry_install(unit)) != BCM_E_NONE) {
        return rv;
    }

    if ((rv=nph_egr_vlan_edit_ctag_ext_group_set(unit)) != BCM_E_NONE) {
        return rv;
    }
    
    if ((rv=nph_egr_vsi_dir_ext(unit)) != BCM_E_NONE) {
        return rv;
    }

    if ((rv=nph_egr_inlif_dir_ext(unit)) != BCM_E_NONE) {
        return rv;
    }

    if ((rv=nph_egr_lif_orientation_dir_ext(unit)) != BCM_E_NONE) {
        return rv;
    }

    if ((rv=nph_egr_port_out_dir_ext(unit)) != BCM_E_NONE) {
        return rv;
    }

    if ((rv=nph_egr_l3vpn_next_hop_ext(unit)) != BCM_E_NONE) {
        return rv;
    }

    if ((rv=nph_egr_fwd_code_and_offset_set(unit)) != BCM_E_NONE) {
        return rv;
    }

    if ((rv=nph_egr_ttl_dir_ext(unit)) != BCM_E_NONE) {
        return rv;
    }

    nph_egr_pmf_grp_info.initialized = 1;

    return rv;
}

int nph_egr_pmf_clean_up(int unit)
{
    int i;

    for (i=0; i<sizeof(nph_egress_vlan_editing_map)/sizeof(nph_egress_vlan_editing_map[0]); i++) {
        bcm_field_entry_destroy(unit, nph_egr_pmf_grp_info.vlan_edit_ent[i]);
    }

    bcm_field_entry_destroy(unit, nph_egr_pmf_grp_info.ctag_ext_ent);
    bcm_field_entry_destroy(unit, nph_egr_pmf_grp_info.vsi_ent);
    bcm_field_entry_destroy(unit, nph_egr_pmf_grp_info.lif_ext_ent);
    bcm_field_entry_destroy(unit, nph_egr_pmf_grp_info.lif_orientation_ext_ent);
    bcm_field_entry_destroy(unit, nph_egr_pmf_grp_info.port_out_ext_ent);
    bcm_field_entry_destroy(unit, nph_egr_pmf_grp_info.next_hop_ext_ent);
    bcm_field_entry_destroy(unit, nph_egr_pmf_grp_info.ttl_ext_ent);
    bcm_field_entry_destroy(unit, nph_egr_pmf_grp_info.egr_trap_ent);
    
    for (i=0; i<sizeof(nph_egr_pmf_grp_info.fwd_code_ext_ent)/sizeof(nph_egr_pmf_grp_info.fwd_code_ext_ent[0]); i++) {
        bcm_field_entry_destroy(unit, nph_egr_pmf_grp_info.fwd_code_ext_ent[i]);
    }
    
    bcm_field_group_destroy(unit, nph_egr_pmf_grp_info.vlan_edit_grp_id);
    bcm_field_group_destroy(unit, nph_egr_pmf_grp_info.ctag_ext_grp_id);
    bcm_field_group_destroy(unit, nph_egr_pmf_grp_info.vsi_ext_grp_id);
    bcm_field_group_destroy(unit, nph_egr_pmf_grp_info.lif_ext_grp_id);
    bcm_field_group_destroy(unit, nph_egr_pmf_grp_info.lif_orientation_ext_grp_id);
    bcm_field_group_destroy(unit, nph_egr_pmf_grp_info.port_out_ext_grp_id);
    bcm_field_group_destroy(unit, nph_egr_pmf_grp_info.next_hop_ext_grp_id);
    bcm_field_group_destroy(unit, nph_egr_pmf_grp_info.fwd_code_set_grp_id);
    bcm_field_group_destroy(unit, nph_egr_pmf_grp_info.ttl_ext_grp_id);
    bcm_field_group_destroy(unit, nph_egr_pmf_grp_info.egr_trap_grp_id);

    bcm_field_presel_destroy(unit, nph_egr_pmf_grp_info.vpws_presel_id);
    bcm_field_presel_destroy(unit, nph_egr_pmf_grp_info.vpls_presel_id);
    bcm_field_presel_destroy(unit, nph_egr_pmf_grp_info.l3vpn_uni_presel_id);
    bcm_field_presel_destroy(unit, nph_egr_pmf_grp_info.l3vpn_nni_presel_id);
    bcm_field_presel_destroy(unit, nph_egr_pmf_grp_info.mpls_presel_id);
    bcm_field_presel_destroy(unit, nph_egr_pmf_grp_info.misc_presel_id);

    bcm_field_data_qualifier_destroy(unit, nph_egr_pmf_grp_info.vs_qual_id);
    bcm_field_data_qualifier_destroy(unit, nph_egr_pmf_grp_info.lif_ext_qual_id);
    bcm_field_data_qualifier_destroy(unit, nph_egr_pmf_grp_info.ctag_qual_id);
    bcm_field_data_qualifier_destroy(unit, nph_egr_pmf_grp_info.vsi_qual_id);
    bcm_field_data_qualifier_destroy(unit, nph_egr_pmf_grp_info.lif_qual_id);
    bcm_field_data_qualifier_destroy(unit, nph_egr_pmf_grp_info.lif_orientation_qual_id);
    bcm_field_data_qualifier_destroy(unit, nph_egr_pmf_grp_info.port_out_qual_id[0]);
    bcm_field_data_qualifier_destroy(unit, nph_egr_pmf_grp_info.port_out_qual_id[1]);
    bcm_field_data_qualifier_destroy(unit, nph_egr_pmf_grp_info.next_hop_qual_id);
    bcm_field_data_qualifier_destroy(unit, nph_egr_pmf_grp_info.next_hop_const_0_qual_id);
    bcm_field_data_qualifier_destroy(unit, nph_egr_pmf_grp_info.next_hop_const_1_qual_id);
    bcm_field_data_qualifier_destroy(unit, nph_egr_pmf_grp_info.fwd_code_sub_type_qual_id);
    bcm_field_data_qualifier_destroy(unit, nph_egr_pmf_grp_info.fwd_code_b_f_flag_qual_id);
    bcm_field_data_qualifier_destroy(unit, nph_egr_pmf_grp_info.ttl_qual_id);
    bcm_field_data_qualifier_destroy(unit, nph_egr_pmf_grp_info.egr_trap_const_0_qual_id[0]);
    bcm_field_data_qualifier_destroy(unit, nph_egr_pmf_grp_info.egr_trap_const_0_qual_id[1]);
    bcm_field_data_qualifier_destroy(unit, nph_egr_pmf_grp_info.egr_trap_const_1_qual_id[0]);
    bcm_field_data_qualifier_destroy(unit, nph_egr_pmf_grp_info.egr_trap_const_1_qual_id[1]);

    return BCM_E_NONE;
}

int nph_vpws_service_egress_run(int unit)
{
    int rv = 0;
    int index;
    bcm_port_t port;
    bcm_vlan_port_t vp;
    bcm_vswitch_cross_connect_t gports;

    for (index=0; index<nph_nof_services; index++) {
        bcm_vlan_port_t_init(&vp);    
        vp.port = nph_service_egress_info[index].np_port;
        vp.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY | BCM_VLAN_PORT_INGRESS_WIDE;
        vp.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL;
        vp.match_tunnel_value = (nph_service_egress_info[index].ps << 17) | nph_service_egress_info[index].output_fp;
        rv = bcm_vlan_port_create(0, &vp);
        if (rv != BCM_E_NONE) {
            printf("bcm_vlan_port_create failed $rv\n");
            return rv;
        }

        nph_service_egress_info[index].vlan_port_id = vp.vlan_port_id;

        printf("Created VLAN port for {ps, output_fp} (%d, 0x%x)\n", nph_service_egress_info[index].ps, nph_service_egress_info[index].output_fp);

        bcm_vswitch_cross_connect_t_init(&gports);
        gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
        gports.port1 = nph_service_egress_info[index].vlan_port_id;
        gports.port2 = nph_service_egress_info[index].gport;

        /* cross connect the 2 ports */
        if(verbose >= 1) {
            printf("connect port1:0x0%8x with port2:0x0%8x \n", gports.port1, gports.port2);
        }
        
        rv = bcm_vswitch_cross_connect_add(unit, &gports);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vswitch_cross_connect_add\n");
            print rv;
            return rv;
        }
    }

    return rv;
}

/*
 * InLIF extension data is organized as follows:
 *     { 2'bstag_status[1:0], 2'bctag_status[1:0]}
 * IVE command ID is encoded as {NPH.VS[1:0], output_fp.STAG[1:0], output_fp.CTAG[1:0]}
 * IVE command should be defined according to the following table:
 *
 *     VS \ TAG status | Keep | TAG | UNTAG |
 *     ----------------------------------
 *               0            | NOP  | ADD |  NOP    |
 *     ----------------------------------
 *               1            | NOP  |SWAP|  DEL    |
 *     ----------------------------------
 *
 * stag_status:
 *     stag_status[1]
 *         0 - keep ctag
 *         1 - don't keep ctag
 *     stag_status[0]
 *         0 - UNTAG
 *         1 - TAG
 *
 * ctag_status:
 *     ctag_status[1]
 *         0 - keep ctag
 *         1 - don't keep ctag
 *     ctag_status[0]
 *         0 - UNTAG
 *         1 - TAG
 */    
int nph_egress_vlan_editing_info_init(int index, nph_tag_action_defines_e stag_action, nph_tag_action_defines_e ctag_action, uint32 stag_vid, uint32 ctag_vid, uint32 stag_pcp_dei, uint32 ctag_pri_cfi, uint32 stag_pcp_dei_flag, uint32 ctag_pri_cfi_flag)
{
    if (index>1) {
        printf("invalid index\n");
    }
    
    nph_egress_vlan_editing_info[index].stag_action = stag_action;
    nph_egress_vlan_editing_info[index].ctag_action = ctag_action;
    nph_egress_vlan_editing_info[index].stag_vid = stag_vid & 0xFFF;
    nph_egress_vlan_editing_info[index].ctag_vid = ctag_vid &0xFFF;
    nph_egress_vlan_editing_info[index].stag_pcp_dei = stag_pcp_dei & 0xF;
    nph_egress_vlan_editing_info[index].ctag_pri_cfi = ctag_pri_cfi & 0xF;
    nph_egress_vlan_editing_info[index].stag_pcp_dei_flag = stag_pcp_dei_flag & 0x1;
    nph_egress_vlan_editing_info[index].ctag_pri_cfi_flag = ctag_pri_cfi_flag & 0x1;

    return 0;
}

int nph_vpws_service_egr_vlan_edit_vid_set(int unit, bcm_gport_t gport, uint32 s_vid, uint32 c_vid)
{
    bcm_vlan_port_translation_t port_trans;
    uint64 data_w;
    uint32 data;
    int rv;
    
    bcm_vlan_port_translation_t_init(&port_trans);  
    port_trans.flags = BCM_VLAN_ACTION_SET_INGRESS;
    port_trans.gport = gport;
    port_trans.vlan_edit_class_id = 0;
    port_trans.new_outer_vlan = s_vid;
    port_trans.new_inner_vlan = 0;

    rv = bcm_vlan_port_translation_set(unit, &port_trans);
    if (rv != BCM_E_NONE) {
        printf("bcm_vlan_port_translation_set failed\n");
        return rv;
    }

    rv = bcm_port_wide_data_get(unit, gport, BCM_PORT_WIDE_DATA_INGRESS, &data_w);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_wide_data_get failed: $rv\n", rv);
        return rv;
    }

    data = COMPILER_64_LO(data_w);
    data &= ~0xFFF;
    data |= c_vid & 0xFFF;

    COMPILER_64_SET(data_w, 0, data);
    rv = bcm_port_wide_data_set(unit, gport, BCM_PORT_WIDE_DATA_INGRESS, data_w);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_wide_data_set failed: $rv\n", rv);
        return rv;
    }

    return rv;
}

int nph_vpws_service_egr_vlan_edit_profile_set(int unit, bcm_gport_t gport, uint32 stag_prio_flag, uint32 ctag_prio_flag, uint32 cos_profile, uint32 stag_pcp_dei, uint32 ctag_pri_cfi, uint32 stag_staus, uint32 ctag_status)
{
    uint64 data_w;
    uint32 data;
    int rv;

    data = stag_prio_flag & 0x1;
    data <<= 1;
    data |= ctag_prio_flag & 0x1;
    data <<=2;
    data |= stag_staus;
    data <<=2;
    data |= ctag_status;
    data <<=4;
    data |= cos_profile & 0xF;
    data <<=4;
    data |= stag_pcp_dei & 0xF;
    data <<=4;
    data |= ctag_pri_cfi & 0xF;

    if (verbose) {
        printf("data = 0x%08x\n", data);
    }

    rv = bcm_port_wide_data_get(unit, gport, BCM_PORT_WIDE_DATA_INGRESS, &data_w);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_wide_data_get failed: $rv\n", rv);
        return rv;
    }

    data <<=12;
    data |= COMPILER_64_LO(data_w) & 0xFFF;

    COMPILER_64_SET(data_w, 0, data);
    rv = bcm_port_wide_data_set(unit, gport, BCM_PORT_WIDE_DATA_INGRESS, data_w);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_wide_data_set failed: $rv\n", rv);
        return rv;
    }

    return rv;
}

int nph_vpws_service_egr_vlan_editing_table_init(int unit)
{
    bcm_vlan_action_set_t action;
    int vs;
    int stag_status;
    int ctag_status;
    int index;
    int ive_cmd;
    int rv;

    for (index=0, ive_cmd=nph_ive_cmd_vs_none_add_c; index<nph_ive_cmd_nof_cmds-nph_ive_cmd_vs_none_add_c; index++,ive_cmd++) {
        rv = bcm_vlan_translate_action_id_create( unit, BCM_VLAN_ACTION_SET_INGRESS | BCM_VLAN_ACTION_SET_WITH_ID, &ive_cmd);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_create\n");
            return rv;
        }

        bcm_vlan_action_set_t_init(&action);
        action.dt_outer = nph_egress_vlan_editing_cmd[index].outer_action;
        action.dt_inner = nph_egress_vlan_editing_cmd[index].inner_action;
        action.dt_outer_pkt_prio = nph_egress_vlan_editing_cmd[index].outer_prio_action;
        action.dt_inner_pkt_prio = nph_egress_vlan_editing_cmd[index].inner_prio_action;
        action.outer_tpid = 0x8100; 

        rv = bcm_vlan_translate_action_id_set( unit, 
                                               BCM_VLAN_ACTION_SET_INGRESS,
                                               ive_cmd,
                                               &action);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_set %d\n", ive_cmd);
            return rv;
        }                  
    }

    return rv;
}
int nph_vpws_service_egress_run_with_defaults(int unit)
{
    int index;
    int rv = BCM_E_NONE;

    nph_pp_port_mapping_init(unit);
    nph_service_info_egress_init(0, ilk_port, ac_ps, ac_output_fp, vlan_port.vlan_port_id);
    nph_service_info_egress_init(1, ilk_port, pwe_ps, pwe_output_fp, mpls_port.mpls_port_id);

    nph_vpws_service_egress_run(unit);

    for (index=0; index<2; index++) {
        nph_vpws_service_egr_vlan_edit_vid_set(unit, nph_service_egress_info[index].vlan_port_id, 
            nph_egress_vlan_editing_info[index].stag_vid, 
            nph_egress_vlan_editing_info[index].ctag_vid);
        nph_vpws_service_egr_vlan_edit_profile_set(unit, nph_service_egress_info[index].vlan_port_id, 
            nph_egress_vlan_editing_info[index].stag_pcp_dei_flag, 
            nph_egress_vlan_editing_info[index].ctag_pri_cfi_flag, 
            nph_egress_vlan_editing_info[index].cos_profile, 
            nph_egress_vlan_editing_info[index].stag_pcp_dei, 
            nph_egress_vlan_editing_info[index].ctag_pri_cfi, 
            nph_egress_vlan_editing_info[index].stag_action, 
            nph_egress_vlan_editing_info[index].ctag_action);
    }
    
    rv = nph_vpws_service_egr_vlan_editing_table_init(unit);

    return rv;
}

/*
* debug rx +
* cd ../../../../src/examples/dpp
* cint ../sand/utility/cint_sand_utils_global.c 
* cint_port_tpid.c 
* cint_advanced_vlan_translation_mode.c 
* cint_nph_egress.c
* c
*     nph_vpws_service_egress_run_with_defaults_dvapi(0,ac_port,np_port);
*
* Packet injected to NP port:
*     00000A0D9E002000F0FF0000052000000000000000220000000000118100001488470002802800014014000281FF00000100000100109400000281000064000000000000000000000000000000000000
*
* Expexcted:
*     00000100000100109400000281000064000000000000000000000000000000000000
*/
int nph_vpws_service_egress_run_with_defaults_dvapi(int unit, bcm_port_t ac_port, bcm_port_t np_port)
{
    int rv;
    bcm_vlan_port_t vlan_port;

    /* for simplicity we run on core 0 only */
    nph_egr_pmf_core_bitmap=1;

    if ((rv=nph_pp_port_mapping_init(unit, 0, np_port, np_port, rcy_port[0])) != BCM_E_NONE) {
        return rv;
    }

    if ((rv=nph_egr_pmf_init(unit, nph_egr_pmf_default_pri_start, nph_egr_pmf_default_presel_id_start)) != BCM_E_NONE) {
        return rv;
    }

    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags |= BCM_VLAN_PORT_OUTER_VLAN_PRESERVE | BCM_VLAN_PORT_INNER_VLAN_PRESERVE;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = ac_port;
    vlan_port.match_vlan = 20;
    rv = bcm_vlan_port_create(unit, vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create: $rv\n");
        return rv;
    }
    
    nph_service_info_egress_init(0, np_port, ac_ps, ac_output_fp, vlan_port.vlan_port_id);
    nph_nof_services = 1;

    return nph_vpws_service_egress_run(unit);
}

int nph_vpws_service_egress_run_with_defaults_dvapi_clean_up(int unit)
{
    bcm_vswitch_cross_connect_t gports;
    bcm_vswitch_cross_connect_t_init(&gports);
    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    gports.port1 = nph_service_egress_info[0].vlan_port_id;    
    gports.port2 = nph_service_egress_info[0].gport;
    bcm_vswitch_cross_connect_delete(unit, &gports);

    bcm_vlan_port_destroy(unit, nph_service_egress_info[0].vlan_port_id);
    bcm_vlan_port_destroy(unit, nph_service_egress_info[0].gport);

    nph_egr_pmf_clean_up(unit);

    return 0;
}

/*
* debug rx +
* cd ../../../../src/examples/dpp
* cint ../sand/utility/cint_sand_utils_global.c 
* cint_port_tpid.c 
* cint_advanced_vlan_translation_mode.c 
* cint_nph_egress.c
* c
*     nph_vpls_uc_egress_run_with_defaults_dvapi(0,ac_port,np_port);
*
* Packet injected to NP port:
*     01000A0D9E002000F00FF0000052000000000000000220000000000118100001488470002802800014014000281FF00000100000100109400000281000064000000000000000000000000000000000000
*
* Expexcted:
*     00000100000100109400000281000064000000000000000000000000000000000000
*/
int nph_vpls_uc_egress_run_with_defaults_dvapi(int unit, bcm_port_t ac_port, bcm_port_t np_port)
{
    int index;
    int rv;
    int del_len;
    bcm_vlan_port_t vlan_port;


    /* for simplicity we run on core 0 only */
    nph_egr_pmf_core_bitmap=1;

    if ((rv=nph_pp_port_mapping_init(unit, 0, np_port, np_port, rcy_port[0])) != BCM_E_NONE) {
        return rv;
    }

    if ((rv=nph_egr_pmf_init(unit, nph_egr_pmf_default_pri_start, nph_egr_pmf_default_presel_id_start)) != BCM_E_NONE) {
        return rv;
    }

    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags |= BCM_VLAN_PORT_OUTER_VLAN_PRESERVE | BCM_VLAN_PORT_INNER_VLAN_PRESERVE;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = ac_port;
    vlan_port.match_vlan = 20;
    rv = bcm_vlan_port_create(unit, vlan_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create: $rv\n");
        return rv;
    }
    
    nph_service_info_egress_init(0, np_port, ac_ps, ac_output_fp, vlan_port.vlan_port_id);
    nph_nof_services = 1;

    return nph_vpws_service_egress_run(unit);
}

int nph_vpls_uc_egress_run_with_defaults_dvapi_clean_up(int unit)
{
    bcm_vswitch_cross_connect_t gports;
    bcm_vswitch_cross_connect_t_init(&gports);
    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    gports.port1 = nph_service_egress_info[0].vlan_port_id;    
    gports.port2 = nph_service_egress_info[0].gport;
    bcm_vswitch_cross_connect_delete(unit, &gports);

    bcm_vlan_port_destroy(unit, nph_service_egress_info[0].vlan_port_id);
    bcm_vlan_port_destroy(unit, nph_service_egress_info[0].gport);

    nph_egr_pmf_clean_up(unit);

    return 0;
}

/*
 * debug rx +
 * cd ../../../../src/examples/dpp
 * cint utility/cint_utils_l2.c 
 * cint ../sand/utility/cint_sand_utils_global.c 
 * cint utility/cint_utils_multicast.c 
 * cint cint_port_tpid.c 
 * cint cint_advanced_vlan_translation_mode.c 
 * cint cint_nph_egress.c 
 * cint
 * nph_vpls_mc_2_pass_run_with_defaults_dvapi(0, 200, 20,  201);
 *
 * Injected packet:
 *    NPH:
 *      M/U - 1
 *      sub_type - 1
 *      del_len - 0x1E
 *      ps - 0
 *      output_fp - 0x2000
 *  
 *  Expected:
 *      Packet forwarded to two AC port:
 *          AC1 - outer TAG = 20
 *          AC2 - outer TAG = 21
 */
int nph_vpls_mc_2_pass_run_with_defaults_dvapi(int unit, bcm_port_t ac_port, bcm_vlan_t ac_vlan, bcm_port_t np_port)
{
    int i, index;
    int rv;
    int del_len;
    bcm_vlan_port_t vlan_port;
    bcm_gport_t gport;    
    bcm_vswitch_cross_connect_t gports;
    int ps[2] = {0, 4};
    int output_fp[2] = {0x3000, 0x4000};
    int dest;
    bcm_if_t encap_id;
    int nof_service = 2;

    /* for simplicity we run on core 0 only */
    nph_egr_pmf_core_bitmap=1;

    if ((rv=nph_pp_port_mapping_init(unit, 0, np_port, np_port, rcy_port[0])) != BCM_E_NONE) {
        return rv;
    }

    if ((rv=nph_egr_pmf_init(unit, nph_egr_pmf_default_pri_start, nph_egr_pmf_default_presel_id_start)) != BCM_E_NONE) {
        return rv;
    }

    vlan_translate_action_set(unit, 4, 0, 0, bcmVlanActionReplace, bcmVlanActionNone);
    vlan_translate_action_class_set(unit, 4, 0, 0);

    for (i=0; i<nof_service; i++) {
        /* create AC port */
        bcm_vlan_port_t_init(&vlan_port);
        vlan_port.flags |= BCM_VLAN_PORT_WITH_ID;
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        vlan_port.port = ac_port;
        vlan_port.match_vlan = ac_vlan+i;
        BCM_GPORT_VLAN_PORT_ID_SET(vlan_port.vlan_port_id, inlifs[i]);    
        rv = bcm_vlan_port_create(unit, vlan_port);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_create: $rv\n");
            return rv;
        }
        inlif_gports[2*i] = vlan_port.vlan_port_id;

        vlan_port_translation_set(unit, ac_vlan+i, 0, vlan_port.vlan_port_id, 2, 0);
    
        /* create {ps, output_fp} service flows */
        bcm_vlan_port_t_init(&vlan_port);    
        vlan_port.port = np_port;
        vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY | BCM_VLAN_PORT_INGRESS_WIDE;
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL;
        vlan_port.match_tunnel_value = (ps[i] << 17) | output_fp[i];
        rv = bcm_vlan_port_create(0, &vlan_port);
        if (rv != BCM_E_NONE) {
            printf("bcm_vlan_port_create failed $rv\n");
            return rv;
        }
        inlif_gports[2*i+1] = vlan_port.vlan_port_id;

        /* Cross-connect them */
        bcm_vswitch_cross_connect_t_init(&gports);
        gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
        gports.port1 = inlif_gports[2*i+1];
        gports.port2 = inlif_gports[2*i];
        
        /* cross connect the 2 ports */
        if(verbose >= 1) {
            printf("connect port1:0x0%8x with port2:0x0%8x \n", gports.port1, gports.port2);
        }
        
        rv = bcm_vswitch_cross_connect_add(unit, &gports);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vswitch_cross_connect_add\n");
            return rv;
        }
    }

    /* create multicast group */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_WITH_ID | BCM_MULTICAST_INGRESS_GROUP, &nph_egr_mcast_id);     
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create\n");
        return rv;
    }    

    for (i=nof_service-1; i>=0; i--) {
        dest = rcy_port[(ps[i] >> 2) & 0x1];
        BCM_GPORT_LOCAL_SET(gport, dest);    
        encap_id = (ps[i] & 0x3) << 17 | output_fp[i];
        rv = bcm_multicast_ingress_add(unit, nph_egr_mcast_id, gport, encap_id);       
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_multicast_ingress_add\n");
            return rv;
        }    
    }
    
    BCM_GPORT_MCAST_SET(gport, nph_egr_mcast_id);

    nph_service_info_egress_init(0, nph_egress_logical_port_defines[0].nph_mc_pp_port, 0, nph_egr_mcast_id, gport);
    nph_nof_services = 1;

    return nph_vpws_service_egress_run(unit);
}

int nph_vpls_mc_2_pass_run_with_defaults_dvapi_clean_up(int unit)
{
    bcm_vswitch_cross_connect_t gports;
    bcm_vswitch_cross_connect_t_init(&gports);
    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    gports.port1 = nph_service_egress_info[0].vlan_port_id;    
    gports.port2 = nph_service_egress_info[0].gport;
    bcm_vswitch_cross_connect_delete(unit, &gports);

    gports.port1 = inlif_gports[1];    
    gports.port2 = inlif_gports[0];
    bcm_vswitch_cross_connect_delete(unit, &gports);
    gports.port1 = inlif_gports[3];    
    gports.port2 = inlif_gports[2];
    bcm_vswitch_cross_connect_delete(unit, &gports);

    bcm_multicast_destroy(unit, nph_egr_mcast_id);
    bcm_vlan_port_destroy(unit, inlif_gports[0]);
    bcm_vlan_port_destroy(unit, inlif_gports[1]);
    bcm_vlan_port_destroy(unit, inlif_gports[2]);
    bcm_vlan_port_destroy(unit, inlif_gports[3]);
    bcm_vlan_port_destroy(unit, nph_service_egress_info[0].vlan_port_id);

    nph_egr_pmf_clean_up(unit);

    return 0;
}

/*
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/dpp/utility/cint_utils_l3.c
 * cint ../../../../src/examples/dpp/utility/cint_utils_l2.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_mpls.c
 * cint ../../../../src/examples/dpp/utility/cint_utils_mpls_port.c 
 * cint ../../../../src/examples/dpp/utility/cint_utils_multicast.c 
 * cint ../../../../src/examples/dpp/cint_qos.c 
 * cint ../../../../src/examples/dpp/cint_port_tpid.c 
 * cint ../../../../src/examples/dpp/cint_advanced_vlan_translation_mode.c 
 * cint ../../../../src/examples/dpp/cint_mpls_lsr.c 
 * cint ../../../../src/examples/dpp/cint_mpls_tunnel_initiator.c
 * cint ../../../../src/examples/dpp/cint_vswitch_metro_mp.c 
 * cint ../../../../src/examples/dpp/cint_vswitch_cross_connect_p2p.c 
 * cint ../../../../src/examples/dpp/cint_connect_to_np_vpws.c 
 * cint ../../../../src/examples/dpp/cint_nph_egress.c 
 * cint
 *     nph_service_info_egress_init(0,np_port,3,0x2000,0);
 *     nph_mpls_egress_run_with_defaults_dvapi(unit, mpls_port, np_port);
 *
 * Packet injected to NP port:
 *     UC/MC=0
 *     sub_type=3
 *     PS=3
 *     output_fp = 0x2000
 * 
 * Example data:
 *     0300010012062000f00e000105201000000000000033000000000011810000c88847017701404500002e0000000040fff8cc010101017fffff030001020304050508090a
 *
 * Expected packet on MPLS port:
 *     DA=00:00:00:00:00:44
 *     SA=00:00:00:00:00:33
 *     MPLS label = 1001
 */
int nph_mpls_egress_run_with_defaults_dvapi(int unit, bcm_port_t mpls_port, bcm_port_t np_port)
{
    int index = 0;
    int rv;
    int del_len;
    bcm_vlan_port_t vlan_port;
	bcm_vswitch_cross_connect_t gports;

    /* for simplicity we run on core 0 only */
    nph_egr_pmf_core_bitmap=1;

    if ((rv=nph_pp_port_mapping_init(unit, 0, np_port, np_port, rcy_port[0])) != BCM_E_NONE) {
        return rv;
    }

    if ((rv=nph_egr_pmf_init(unit, nph_egr_pmf_default_pri_start, nph_egr_pmf_default_presel_id_start)) != BCM_E_NONE) {
        return rv;
    }

    tunnel_initiator_swap_action=1;
    
    /* create EEDB MPLS SWAP entry */
    mpls_tunnel_initiator_run_with_defaults(unit,-1,mpls_port,mpls_port);

    bcm_vlan_port_t_init(&vlan_port);    
    index=0;
    vlan_port.port = nph_service_egress_info[index].np_port;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY | BCM_VLAN_PORT_INGRESS_WIDE;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL;
    vlan_port.match_tunnel_value = (nph_service_egress_info[index].ps << 17) | nph_service_egress_info[index].output_fp;
    rv = bcm_vlan_port_create(0, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("bcm_vlan_port_create failed $rv\n");
        return rv;
    }

    nph_service_egress_info[index].vlan_port_id = vlan_port.vlan_port_id;

	bcm_vswitch_cross_connect_t_init(&gports);
	gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
	gports.port1 = nph_service_egress_info[0].vlan_port_id;
	gports.port2 = mpls_port;
	gports.encap2 = BCM_GPORT_TUNNEL_ID_GET(outlif_to_count);

    nph_service_egress_info[index].gport = mpls_port;

	rv = bcm_vswitch_cross_connect_add(unit, &gports);
    if (rv != BCM_E_NONE) {
        printf("bcm_vswitch_cross_connect_add failed $rv\n");
        return rv;
    }

    return rv;
}

int nph_mpls_egress_run_with_defaults_dvapi_clean_up(int unit)
{
    bcm_vswitch_cross_connect_t gports;
    bcm_vswitch_cross_connect_t_init(&gports);
    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    gports.port1 = nph_service_egress_info[0].vlan_port_id;    
    gports.port2 = nph_service_egress_info[0].gport;
	gports.encap2 = BCM_GPORT_TUNNEL_ID_GET(outlif_to_count);
    bcm_vswitch_cross_connect_delete(unit, &gports);

    bcm_vlan_port_destroy(unit, nph_service_egress_info[0].vlan_port_id);

    return 0;
}

/*
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/dpp/utility/cint_utils_l3.c
 * cint ../../../../src/examples/dpp/utility/cint_utils_l2.c
 * cint ../../../../src/examples/dpp/utility/cint_utils_vlan.c 
 * cint ../../../../src/examples/dpp/cint_port_tpid.c 
 * cint ../../../../src/examples/dpp/cint_multi_device_utils.c 
 * cint ../../../../src/examples/dpp/cint_advanced_vlan_translation_mode.c 
 * cint ../../../../src/examples/dpp/cint_ip_route.c 
 * cint ../../../../src/examples/dpp/cint_nph_egress.c 
 * cint
 *     nph_service_info_egress_init(0,np_port,3,0x2000,0);
 *     nph_service_info_egress_init(1,np_port,7,0x3000,0);
 *     nph_l3uc_egress_run_with_defaults_dvapi(unit, ac_port, np_port);
 *
 * Packet injected to NP port:
 *     L3UC to UNI
 *         UC/MC=0
 *         main_type=0
 *         sub_type=2
 *         PS=3
 *         output_fp = 0x2000
 *         next_hop_ptr = 0x1000
 * 
 *         Example data:
 *             0200008012062000f0ff000005201000000c000200000010940000028100000f08004500006a00000000fffd7a3cc05501027fffff030000000000000000000000000000000000000000000000000000
 *
 *         Expected packet on AC port:
 *             DA=00:00:00:00:cd:1d
 *             SA=00:0c:00:02:00:00
 *             VLAN = 100
 *     
 *     L3UC to NNI
 *         UC/MC=0
 *         main_type=1
 *         sub_type=2
 *         PS=7
 *         output_fp = 0x3000
 * 
 *         Example data:
 *             12000080120E3000f0ff000005200000000c000200000010940000028100000f08004500006a00000000fffd7a3cc05501027fffff030000000000000000000000000000000000000000000000000000
 *
 *         Expected packet on AC port:
 *             DA=00:00:00:00:cd:1d
 *             SA=00:0c:00:02:00:00
 *             VLAN = 100
 */
int nph_l3uc_egress_run_with_defaults_dvapi(int unit, bcm_port_t ac_port, bcm_port_t np_port)
{
    int index = 0;
    int rv;
    bcm_vlan_port_t vlan_port;
	bcm_vswitch_cross_connect_t gports;

    /* for simplicity we run on core 0 only */
    nph_egr_pmf_core_bitmap=1;

    if ((rv=nph_pp_port_mapping_init(unit, 0, np_port, np_port, rcy_port[0])) != BCM_E_NONE) {
        return rv;
    }

    if ((rv=nph_egr_pmf_init(unit, nph_egr_pmf_default_pri_start, nph_egr_pmf_default_presel_id_start)) != BCM_E_NONE) {
        return rv;
    }
    
    basic_example(unit,1,ac_port,ac_port);

    /* L3UC, UNI */
    bcm_vlan_port_t_init(&vlan_port);    
    vlan_port.port = nph_service_egress_info[index].np_port;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY | BCM_VLAN_PORT_INGRESS_WIDE;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL;
    vlan_port.match_tunnel_value = (nph_service_egress_info[index].ps << 17) | nph_service_egress_info[index].output_fp;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("bcm_vlan_port_create failed $rv\n");
        return rv;
    }

    nph_service_egress_info[index].vlan_port_id = vlan_port.vlan_port_id;

	bcm_vswitch_cross_connect_t_init(&gports);
	gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
	gports.port1 = nph_service_egress_info[index].vlan_port_id;
	gports.port2 = ac_port;
	gports.encap2 = out_rif_used;

    nph_service_egress_info[index].gport = ac_port;

	rv = bcm_vswitch_cross_connect_add(unit, &gports);
    if (rv != BCM_E_NONE) {
        printf("bcm_vswitch_cross_connect_add failed $rv\n");
        return rv;
    }

    index = 1;
    
    /* L3UC, NNI */
    bcm_vlan_port_t_init(&vlan_port);    
    vlan_port.port = nph_service_egress_info[index].np_port;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY | BCM_VLAN_PORT_INGRESS_WIDE;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL;
    vlan_port.match_tunnel_value = (nph_service_egress_info[index].ps << 17) | nph_service_egress_info[index].output_fp;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("bcm_vlan_port_create failed $rv\n");
        return rv;
    }

    nph_service_egress_info[index].vlan_port_id = vlan_port.vlan_port_id;

    bcm_vswitch_cross_connect_t_init(&gports);
    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    gports.port1 = nph_service_egress_info[index].vlan_port_id;
    gports.port2 = ac_port;
    gports.encap2 = BCM_ENCAP_ID_GET(next_hop_used);

    nph_service_egress_info[index].gport = ac_port;

    rv = bcm_vswitch_cross_connect_add(unit, &gports);
    if (rv != BCM_E_NONE) {
        printf("bcm_vswitch_cross_connect_add failed $rv\n");
        return rv;
    }

    return rv;
}

int nph_l3uc_egress_run_with_defaults_dvapi_clean_up(int unit)
{
    int index = 0;
    bcm_vswitch_cross_connect_t gports;
    bcm_vswitch_cross_connect_t_init(&gports);
    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    gports.port1 = nph_service_egress_info[index].vlan_port_id;    
    gports.port2 = nph_service_egress_info[index].gport;
	gports.encap2 = out_rif_used;
    bcm_vswitch_cross_connect_delete(unit, &gports);

    bcm_vlan_port_destroy(unit, nph_service_egress_info[index].vlan_port_id);

    index = 1;
    bcm_vswitch_cross_connect_t_init(&gports);
    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    gports.port1 = nph_service_egress_info[index].vlan_port_id;    
    gports.port2 = nph_service_egress_info[index].gport;
	gports.encap2 = BCM_ENCAP_ID_GET(next_hop_used);;
    bcm_vswitch_cross_connect_delete(unit, &gports);

    bcm_vlan_port_destroy(unit, nph_service_egress_info[index].vlan_port_id);

    nph_egr_pmf_clean_up(unit);

    return 0;
}

/*
 * cd ../../../../src/examples/dpp
 * cint cint_queue_tests.c 
 * cint ../sand/utility/cint_sand_utils_global.c 
 * cint utility/cint_utils_multicast.c 
 * cint utility/cint_utils_vlan.c 
 * cint utility/cint_utils_l3.c 
 * cint utility/cint_utils_l2.c 
 * cint ../sand/utility/cint_sand_utils_mpls.c
 * cint utility/cint_utils_mpls_port.c 
 * cint utility/cint_utils_roo.c 
 * cint cint_port_tpid.c 
 * cint cint_advanced_vlan_translation_mode.c 
 * cint cint_l2_mact.c 
 * cint cint_vswitch_metro_mp.c 
 * cint cint_qos.c 
 * cint cint_mpls_lsr.c 
 * cint cint_vswitch_vpls.c 
 * cint cint_vswitch_vpls_roo.c
 * cint cint_multi_device_utils.c
 * cint cint_ip_route.c
 * cint cint_ip_route_tunnel.c
 * cint cint_nph_egress.c
 *     L3VPN->L2VPN
 *     nph_service_info_egress_init(0,128,0,0x1000,0);
 *     L2VPN->L3VPN
 *     nph_service_info_egress_init(1,128,3,0x2000,0);
 *     nph_l3vpn_to_l2vpn_bridging_egress_run_with_defaults_dvapi(unit, 0, 128);
 *
 * Packet injected to NP port:
 *     L3VPN->L2VPN
 *         0000010016001000f8ff026c0520100300000000cd1d000c00020000810000648847000c81144500001400000000ff00bd82ab111112a0a1a1a2
 *         UC/MC=0
 *         sub_type=0
 *         PS=0
 *         output_fp = 0x1000
 *         del_len = 22
 *         B_F = 1
 *         Expected RX:
 *             000000000055000c00020011810000c88847003e900a007be10a000000000099000c00020066810002bc08004500001400000000ff00bd82ab111112a0a1a1a2
 *
 *     L2VPN->L3VPN
 *         000001002c062000f8ff000105201007000000000055000c00020011810000c88847003e900a007be10a000000000099000c00020066810002bc08004500001400000000ff00bd82ab111112a0a1a1a2
 *         UC/MC=0
 *         sub_type=0
 *         PS=3
 *         output_fp = 0x2000
 *         del_len = 44
 *         B_F = 1
 *         Expected RX:
 *             00000000cd1d000c00020000810000648847000c81144500001400000000ff00bd82ab111112a0a1a1a2
 */
int nph_l3vpn_to_l2vpn_bridging_egress_run_with_defaults_dvapi(int unit, bcm_port_t ac_port, bcm_port_t np_port)
{
    int index = 0;
    int rv;
    bcm_vlan_port_t vlan_port;
	bcm_vswitch_cross_connect_t gports;

    /* for simplicity we run on core 0 only */
    nph_egr_pmf_core_bitmap=1;

    if ((rv=nph_pp_port_mapping_init(unit, 0, np_port, np_port, rcy_port[0])) != BCM_E_NONE) {
        return rv;
    }

    if ((rv=nph_egr_pmf_init(unit, nph_egr_pmf_default_pri_start, nph_egr_pmf_default_presel_id_start)) != BCM_E_NONE) {
        return rv;
    }
    
    is_vswitch_vpls_roo_validate_extended_stack=1;
    vswitch_vpls_roo_run_with_defaults_dvapi(unit, ac_port, ac_port, 1);

    example_ip_to_tunnel(unit, unit, ac_port, ac_port);

    /* L3VPN->L2VPN */
    bcm_vlan_port_t_init(&vlan_port);    
    vlan_port.port = nph_service_egress_info[index].np_port;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY | BCM_VLAN_PORT_INGRESS_WIDE;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL;
    vlan_port.match_tunnel_value = (nph_service_egress_info[index].ps << 17) | nph_service_egress_info[index].output_fp;
    rv = bcm_vlan_port_create(0, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("bcm_vlan_port_create failed $rv\n");
        return rv;
    }

    nph_service_egress_info[index].vlan_port_id = vlan_port.vlan_port_id;

	bcm_vswitch_cross_connect_t_init(&gports);
	gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
	gports.port1 = nph_service_egress_info[index].vlan_port_id;
	gports.port2 = ac_port;
	gports.encap2 = vswitch_vpls_roo_info.first_pwe_outlif + 2;

    nph_service_egress_info[index].gport = ac_port;

	rv = bcm_vswitch_cross_connect_add(unit, &gports);
    if (rv != BCM_E_NONE) {
        printf("bcm_vswitch_cross_connect_add failed $rv\n");
        return rv;
    }

    if(verbose >= 1) {
        printf("connect port1:0x0%8x with port2:0x%8x \n", gports.port1, gports.port2);
    }

    index++;

    /* L2VPN->L3VPN */
    bcm_vlan_port_t_init(&vlan_port);    
    vlan_port.port = nph_service_egress_info[index].np_port;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY | BCM_VLAN_PORT_INGRESS_WIDE;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL;
    vlan_port.match_tunnel_value = (nph_service_egress_info[index].ps << 17) | nph_service_egress_info[index].output_fp;
    rv = bcm_vlan_port_create(0, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("bcm_vlan_port_create failed $rv\n");
        return rv;
    }

    nph_service_egress_info[index].vlan_port_id = vlan_port.vlan_port_id;

	bcm_vswitch_cross_connect_t_init(&gports);
	gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
	gports.port1 = nph_service_egress_info[index].vlan_port_id;
	gports.port2 = ac_port;
	gports.encap2 = BCM_ENCAP_ID_GET(mpls_tunnel_used);

    nph_service_egress_info[index].gport = ac_port;

	rv = bcm_vswitch_cross_connect_add(unit, &gports);
    if (rv != BCM_E_NONE) {
        printf("bcm_vswitch_cross_connect_add failed $rv\n");
        return rv;
    }

    if(verbose >= 1) {
        printf("connect port1:0x0%8x with port2:0x%8x \n", gports.port1, gports.port2);
    }

    return rv;
}

int nph_l3vpn_to_l2vpn_bridging_egress_run_with_defaults_dvapi_clean_up(int unit)
{
    int index = 0;
    bcm_vswitch_cross_connect_t gports;

	bcm_vswitch_cross_connect_t_init(&gports);
	gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
	gports.port1 = nph_service_egress_info[index].vlan_port_id;
	gports.port2 = nph_service_egress_info[index].gport;
	gports.encap2 = vswitch_vpls_roo_info.first_pwe_outlif + 2;
    bcm_vswitch_cross_connect_delete(unit, &gports);
    bcm_vlan_port_destroy(unit, nph_service_egress_info[index].vlan_port_id);

    index = 1;
	bcm_vswitch_cross_connect_t_init(&gports);
	gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
	gports.port1 = nph_service_egress_info[index].vlan_port_id;
	gports.port2 = nph_service_egress_info[index].gport;
	gports.encap2 = BCM_ENCAP_ID_GET(mpls_tunnel_used);
    bcm_vswitch_cross_connect_delete(unit, &gports);
    bcm_vlan_port_destroy(unit, nph_service_egress_info[index].vlan_port_id);

    nph_egr_pmf_clean_up(unit);

    return 0;
}

/*
 * debug rx +
 * cd ../../../../src/examples/dpp
 * cint ../sand/utility/cint_sand_utils_global.c 
 * cint utility/cint_utils_multicast.c 
 * cint utility/cint_utils_vlan.c 
 * cint utility/cint_utils_l3.c 
 * cint utility/cint_utils_l2.c 
 * cint ../sand/utility/cint_sand_utils_mpls.c
 * cint utility/cint_utils_mpls_port.c 
 * cint cint_qos.c 
 * cint cint_port_tpid.c 
 * cint cint_advanced_vlan_translation_mode.c 
 * cint cint_mpls_lsr.c 
 * cint cint_mpls_tunnel_initiator.c 
 * cint cint_vswitch_metro_mp.c 
 * cint cint_vswitch_cross_connect_p2p.c 
 * cint cint_multi_device_utils.c
 * cint cint_ip_route.c
 * cint cint_ip_route_tunnel.c
 * cint cint_nph_egress.c
 * c
 * nph_service_info_egress_init(0,128,3,0x1000,0);
 * nph_p2mp_2_pass_run_with_defaults_dvapi(0,0,128,0,0x1000);
 *
 * Packet injected to NP port:
 *         8300010012001000F0FF000105201000000000000033000000000011810000C88847017701404500002E000000004000F8CC010101017FFFFF03000102030405060708090A000000000000000000000000000000000000000000000000000000
 *         UC/MC=1
 *         main_type=0
 *         sub_type=3
 *         PS=0
 *         mc_fp = 0x2000
 *         del_len = 18
 * Expected RX:
 *         0000000000440000000000338847003e913f4500002e000000004000f8cc010101017fffff03000102030405060708090a00000000000000
 */
int nph_p2mp_2_pass_run_with_defaults_dvapi(int unit, bcm_port_t mpls_port, bcm_port_t np_port, int ps_mc, int mc_id)
{
    int i, index;
    int rv;
    int del_len;
    bcm_vlan_port_t vlan_port;
    bcm_gport_t gport;    
    bcm_vswitch_cross_connect_t gports;
    int ps;
    int output_fp;
    int dest;
    bcm_if_t encap_id;

    nph_mpls_egress_run_with_defaults_dvapi(unit, mpls_port, np_port);

    /* create multicast group */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_WITH_ID | BCM_MULTICAST_INGRESS_GROUP, &nph_egr_mcast_id);     
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create\n");
        return rv;
    }    

    ps = nph_service_egress_info[0].ps;
    output_fp = nph_service_egress_info[0].output_fp;
    
    dest = rcy_port[(ps >> 2) & 0x1];
    BCM_GPORT_LOCAL_SET(gport, dest);    
    encap_id = (ps & 0x3) << 17 | output_fp;

    /* Add {ps, output_fp} as multicast member */
    rv = bcm_multicast_ingress_add(unit, nph_egr_mcast_id, gport, encap_id);       
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_ingress_add\n");
        return rv;
    }    
    
    BCM_GPORT_MCAST_SET(gport, nph_egr_mcast_id);

    nph_service_info_egress_init(1, nph_egress_logical_port_defines[0].nph_mc_pp_port, ps_mc, mc_id, gport);

    index = 1;

    /* Cross connect MC ID to MC group */
    bcm_vlan_port_t_init(&vlan_port);    
    vlan_port.port = nph_service_egress_info[index].np_port;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY | BCM_VLAN_PORT_INGRESS_WIDE;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL;
    vlan_port.match_tunnel_value = (nph_service_egress_info[index].ps << 17) | nph_service_egress_info[index].output_fp;
    rv = bcm_vlan_port_create(0, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("bcm_vlan_port_create failed $rv\n");
        return rv;
    }

    nph_service_egress_info[index].vlan_port_id = vlan_port.vlan_port_id;

    printf("Created VLAN port for {ps, output_fp} (%d, 0x%x)\n", nph_service_egress_info[index].ps, nph_service_egress_info[index].output_fp);

    bcm_vswitch_cross_connect_t_init(&gports);
    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    gports.port1 = nph_service_egress_info[index].vlan_port_id;
    gports.port2 = nph_service_egress_info[index].gport;

    /* cross connect the 2 ports */
    if(verbose >= 1) {
        printf("connect port1:0x0%8x with port2:0x0%8x \n", gports.port1, gports.port2);
    }
    
    rv = bcm_vswitch_cross_connect_add(unit, &gports);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_cross_connect_add\n");
        return rv;
    }

    return rv;
}

int nph_p2mp_2_pass_run_with_defaults_dvapi_clean_up(int unit)
{
    int index = 1;
        
    bcm_vswitch_cross_connect_t gports;
    bcm_vswitch_cross_connect_t_init(&gports);
    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    gports.port1 = nph_service_egress_info[index].vlan_port_id;    
    gports.port2 = nph_service_egress_info[index].gport;
    bcm_vswitch_cross_connect_delete(unit, &gports);
    bcm_vlan_port_destroy(unit, nph_service_egress_info[index].vlan_port_id);

    bcm_multicast_destroy(unit, nph_egr_mcast_id);

    nph_mpls_egress_run_with_defaults_dvapi_clean_up(unit);

    return 0;
}

/*
 * debug rx +
 * cd ../../../../src/examples/dpp
 * cint ../sand/utility/cint_sand_utils_global.c 
 * cint utility/cint_utils_multicast.c 
 * cint utility/cint_utils_vlan.c 
 * cint utility/cint_utils_l3.c 
 * cint utility/cint_utils_l2.c 
 * cint ../sand/utility/cint_sand_utils_mpls.c
 * cint utility/cint_utils_mpls_port.c 
 * cint cint_qos.c 
 * cint cint_port_tpid.c 
 * cint cint_advanced_vlan_translation_mode.c 
 * cint cint_mpls_lsr.c 
 * cint cint_mpls_tunnel_initiator.c 
 * cint cint_vswitch_metro_mp.c 
 * cint cint_vswitch_cross_connect_p2p.c 
 * cint cint_multi_device_utils.c
 * cint cint_ip_route.c
 * cint cint_ip_route_tunnel.c
 * cint cint_nph_egress.c
 * c
 * nph_service_info_egress_init(0,128,3,0x1000,0);
 * nph_ipmc_2_pass_run_with_defaults_dvapi(0,0,128, 0, 0x1000);
 *
 * Packet injected to NP port:
 *         9100010012001000F0FF000105201000000000000033000000000011810000C808004500006e00000000fffd1839c0550102e0010101000102030405060708090A000000000000000000000000000000000000000000000000000000
 *         UC/MC=1
 *         main_type=1
 *         sub_type=1
 *         PS=0
 *         mc_fp = 0x2000
 *         del_len = 18
 * Expected RX:
 *         00000000cd1d000c00020000810000648847000c81144500006e00000000fffd1839c0550102e0010101000102030405060708090a000000
 */
int nph_ipmc_2_pass_run_with_defaults_dvapi(int unit, bcm_port_t mpls_port, bcm_port_t np_port, int ps_mc, int mc_id)
{
    int i, index = 0;
    int rv;
    int del_len;
    bcm_vlan_port_t vlan_port;
    bcm_gport_t gport;    
    bcm_vswitch_cross_connect_t gports;
    int ps;
    int output_fp;
    int dest;
    bcm_if_t encap_id;

    /* for simplicity we run on core 0 only */
    nph_egr_pmf_core_bitmap=1;

    if ((rv=nph_pp_port_mapping_init(unit, 0, np_port, np_port, rcy_port[0])) != BCM_E_NONE) {
        return rv;
    }

    if ((rv=nph_egr_pmf_init(unit, nph_egr_pmf_default_pri_start, nph_egr_pmf_default_presel_id_start)) != BCM_E_NONE) {
        return rv;
    }

    example_ip_to_tunnel(unit, unit, mpls_port, mpls_port);

    /* L2VPN->L3VPN */
    bcm_vlan_port_t_init(&vlan_port);    
    vlan_port.port = nph_service_egress_info[index].np_port;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY | BCM_VLAN_PORT_INGRESS_WIDE;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL;
    vlan_port.match_tunnel_value = (nph_service_egress_info[index].ps << 17) | nph_service_egress_info[index].output_fp;
    rv = bcm_vlan_port_create(0, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("bcm_vlan_port_create failed $rv\n");
        return rv;
    }

    nph_service_egress_info[index].vlan_port_id = vlan_port.vlan_port_id;

	bcm_vswitch_cross_connect_t_init(&gports);
	gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
	gports.port1 = nph_service_egress_info[index].vlan_port_id;
	gports.port2 = mpls_port;
	gports.encap2 = BCM_ENCAP_ID_GET(mpls_tunnel_used);

    nph_service_egress_info[index].gport = mpls_port;

	rv = bcm_vswitch_cross_connect_add(unit, &gports);
    if (rv != BCM_E_NONE) {
        printf("bcm_vswitch_cross_connect_add failed $rv\n");
        return rv;
    }

    /* create multicast group */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_WITH_ID | BCM_MULTICAST_INGRESS_GROUP, &nph_egr_mcast_id);     
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create\n");
        return rv;
    }    

    ps = nph_service_egress_info[0].ps;
    output_fp = nph_service_egress_info[0].output_fp;
    
    dest = rcy_port[(ps >> 2) & 0x1];
    BCM_GPORT_LOCAL_SET(gport, dest);    
    encap_id = (ps & 0x3) << 17 | output_fp;

    /* Add {ps, output_fp} as multicast member */
    rv = bcm_multicast_ingress_add(unit, nph_egr_mcast_id, gport, encap_id);       
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_ingress_add\n");
        return rv;
    }    
    
    BCM_GPORT_MCAST_SET(gport, nph_egr_mcast_id);

    nph_service_info_egress_init(1, nph_egress_logical_port_defines[0].nph_mc_pp_port, ps_mc, mc_id, gport);

    index = 1;

    /* Cross connect MC ID to MC group */
    bcm_vlan_port_t_init(&vlan_port);    
    vlan_port.port = nph_service_egress_info[index].np_port;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY | BCM_VLAN_PORT_INGRESS_WIDE;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_TUNNEL;
    vlan_port.match_tunnel_value = (nph_service_egress_info[index].ps << 17) | nph_service_egress_info[index].output_fp;
    rv = bcm_vlan_port_create(0, &vlan_port);
    if (rv != BCM_E_NONE) {
        printf("bcm_vlan_port_create failed $rv\n");
        return rv;
    }

    nph_service_egress_info[index].vlan_port_id = vlan_port.vlan_port_id;

    printf("Created VLAN port for {ps, output_fp} (%d, 0x%x)\n", nph_service_egress_info[index].ps, nph_service_egress_info[index].output_fp);

    bcm_vswitch_cross_connect_t_init(&gports);
    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    gports.port1 = nph_service_egress_info[index].vlan_port_id;
    gports.port2 = nph_service_egress_info[index].gport;

    /* cross connect the 2 ports */
    if(verbose >= 1) {
        printf("connect port1:0x0%8x with port2:0x0%8x \n", gports.port1, gports.port2);
    }
    
    rv = bcm_vswitch_cross_connect_add(unit, &gports);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_cross_connect_add\n");
        return rv;
    }

    return rv;
}

int nph_ipmc_2_pass_run_with_defaults_dvapi_clean_up(int unit)
{
    int index = 1;
        
    bcm_vswitch_cross_connect_t gports;
    bcm_vswitch_cross_connect_t_init(&gports);
    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    gports.port1 = nph_service_egress_info[index].vlan_port_id;    
    gports.port2 = nph_service_egress_info[index].gport;
    bcm_vswitch_cross_connect_delete(unit, &gports);
    bcm_vlan_port_destroy(unit, nph_service_egress_info[index].vlan_port_id);

    bcm_multicast_destroy(unit, nph_egr_mcast_id);

    nph_mpls_egress_run_with_defaults_dvapi_clean_up(unit);

    return 0;
}

