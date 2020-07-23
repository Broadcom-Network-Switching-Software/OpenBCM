/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_MANAGEMENT

#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_vtt.h>
#include <soc/dpp/PPC/ppc_api_port.h>



typedef void   (*vtt_table_property_set)    (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 additional_info); 
typedef void   (*vtt_program_property_set)  (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY *prog, uint32 additional_info);         
typedef uint32 (*vt_program_selection_set) (int unit, ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 additional_info);
typedef uint32 (*tt_program_selection_set) (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 additional_info);


uint32 arad_pp_dbal_vtt_program_init(int unit); 
uint32 arad_pp_dbal_vt_program_init(int unit);  
uint32 arad_pp_dbal_vtt_cam_init(int unit);
uint32 arad_pp_dbal_vtt_program_set(int unit);  
uint32 arad_pp_dbal_vt_program_set(int unit);
uint32 arad_pp_dbal_tt_program_set(int unit);
uint32 arad_pp_dbal_vtt_cam_set(int unit);      
uint32 arad_pp_dbal_vt_program_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 program ); 
uint32 arad_pp_dbal_tt_program_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 program ); 

uint32 _arad_pp_dbal_vtt_fc_vsan_mode_update(uint32 unit, uint32 prog_id, uint32 disassociate_table_id); 


#define SW_DB_DBAL2VTT(dbal_index) ((dbal_index) - SOC_DPP_DBAL_SW_TABLE_ID_VTT_FIRST)  
#define SOC_DPP_NOF_VTT_SW_DB_TABLES (SOC_DPP_DBAL_SW_TABLE_ID_VTT_LAST - SOC_DPP_DBAL_SW_TABLE_ID_VTT_FIRST + 1) 
#define PROGRAM_PROPERTY_CLEAR(program_property)   sal_memset(program_property, 0, sizeof(ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY)) 
#define SW_DB_PROPERTY_CLEAR(sw_db_property    )   sal_memset(sw_db_property,   0, sizeof(ARAD_PP_DBAL_VTT_SW_DB_PROPERTY))  




void arad_pp_dbal_sw_db_vdxvid_sem_property_set                  (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 vid_type            ); 
void arad_pp_dbal_sw_db_evb_sem_property_set                     (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 vid_type            );
void arad_pp_dbal_sw_db_vdxvid_pcp_sem_property_set              (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 vid_type            );
void arad_pp_dbal_sw_db_mpls_unidexed_prefix_sem_property_set    (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 mpls_label          ); 
void arad_pp_dbal_sw_db_mpls_gal_sem_property_set                (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 mpls_label          );
void arad_pp_dbal_sw_db_mpls_port_sem_property_set               (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 vid_type            ); 
void arad_pp_dbal_sw_db_mpls_indexed_sem_property_set            (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 mpls_indexed        );
void arad_pp_dbal_sw_db_mpls_eli_unidexed_prefix_sem_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 mpls_label          );
void arad_pp_dbal_sw_db_mpls_eli_indexed_sem_property_set        (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 mpls_eli_indexed    );
void arad_pp_dbal_sw_db_vid_tcam_property_set                    (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 vid_type            ); 
void arad_pp_dbal_sw_db_inner_outer_vid_pcp_tcam_property_set    (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 unused              );
void arad_pp_dbal_sw_db_spoofv4_property_set                     (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 not_in_use          ); 
void arad_pp_dbal_sw_db_dipv6_tcam_property_set                  (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 compressed          ); 
void arad_pp_dbal_sw_db_spoofv6_tcam_property_set                (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 compressed          ); 
void arad_pp_dbal_sw_db_mpls_l1_l2_tcam_property_set             (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 not_in_use          );
void arad_pp_dbal_sw_db_mpls_frr_coupling_tcam_property_set      (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 mpls_label_vid_type );
void arad_pp_dbal_sw_db_mpls_rif_indexed_sem_property_set        (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 mpls_index          );
void arad_pp_dbal_sw_db_trill_property_set                       (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 not_in_use          ); 
void arad_pp_dbal_sw_db_trill_ingress_nickname_property_set      (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 not_in_use          );
void arad_pp_dbal_sw_db_trill_uc_property_set                    (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 nof_tags            );
void arad_pp_dbal_sw_db_trill_tcam_property_set                  (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 not_in_use          );
void arad_pp_dbal_sw_db_dip_sem_property_set                     (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 dummy               );
void arad_pp_dbal_sw_db_sip_sem_property_set                     (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 not_in_use          );
void arad_pp_dbal_sw_db_dip_sip_tcam_property_set                (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 not_in_use          );
void arad_pp_dbal_sw_db_ethernet_header_isid_property_set        (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 not_in_use          );
void arad_pp_dbal_sw_db_ethernet_header_bsa_isid_property_set    (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 not_in_use          );
void arad_pp_dbal_sw_db_mc_dipv4_rif_property_set                (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 unused              );
void arad_pp_dbal_sw_db_tunnel_property_set                      (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 unused              );
void arad_pp_dbal_sw_db_tunnel_port_property_set                 (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 tunnel_type         );
void arad_pp_dbal_sw_db_qinq_property_set                        (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 vid_type            );
void arad_pp_dbal_sw_db_5_tupple_tcam_property_set               (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 unused              );
void arad_pp_dbal_sw_db_vlan_domain_vid_property_set             (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 vid_type            );
void arad_pp_dbal_sw_db_tst1_tcam_property_set                   (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 unused              );
void arad_pp_dbal_sw_db_vrrp_tcam_property_set                   (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 unused              );
void arad_pp_dbal_sw_db_tst1_explicit_null_tcam_property_set     (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 unused              );
void arad_pp_dbal_sw_db_qinq_compressed_sem_property_set         (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 tpid                ); 
void arad_pp_dbal_sw_db_qinany_sem_property_set                  (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 tpid                ); 
void arad_pp_dbal_sw_db_qinany_pcp_sem_property_set              (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 tpid                ); 
void arad_pp_dbal_sw_db_1q_sem_property_set                      (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 tpid                ); 
void arad_pp_dbal_sw_db_1q_compressed_sem_property_set           (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 tpid                ); 
void arad_pp_dbal_sw_db_1q_pcp_compressed_sem_property_set       (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 tpid                ); 
void arad_pp_dbal_sw_db_untagged_sem_property_set                (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 tpid                ); 
void arad_pp_dbal_sw_db_tst2_tcam_property_set                   (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 tpid                ); 
void arad_pp_dbal_sw_db_test2_tcam_property_set                  (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 tpid                ); 
void arad_pp_dbal_sw_db_test2_sem_property_set                   (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 tpid                );       
void arad_pp_dbal_sw_db_extender_untag_check_sem_property_set    (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 unused              ); 
void arad_pp_dbal_sw_db_extender_pe_sem_property_set             (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 vid_type            ); 
void arad_pp_dbal_sw_db_extender_channel_reg_sem_property_set    (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 unused              ); 
void arad_pp_dbal_sw_db_src_port_da_tcam_property_set            (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 vid_type            ); 
void arad_pp_dbal_sw_db_dip_sip_vrf_tcam_property_set            (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 unused              ); 
void arad_pp_dbal_sw_db_my_vtep_index_sip_vrf_sem_property_set   (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 unused              ); 
void arad_pp_dbal_sw_db_gre_port_property_set                    (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 unused              );
void arad_pp_dbal_sw_db_custom_pp_port_tunnel_property_set       (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 unused              );
void arad_pp_dbal_sw_db_custom_pp_port_lsp_ecmp_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 unused);
void arad_pp_dbal_sw_db_vlan_domain_mpls_property_set              (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 mpls_index              );
void arad_pp_dbal_sw_db_oam_stat_property_set                    (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 fwrd_key            );
void arad_pp_dbal_sw_db_bfd_stat_property_set                    (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 fwrd_key            );
void arad_pp_dbal_sw_db_tm_stat_property_set                     (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 stat_type           );
void arad_pp_dbal_sw_db_fc_with_vft_property_set                 (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 vsan_source         );
void arad_pp_dbal_sw_db_fc_property_set                          (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 vsan_source         );
void arad_pp_dbal_sw_db_pon_vdxtunnelxvid_property_set           (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 key_type            );
void arad_pp_dbal_sw_db_section_oam_property_set                 (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 key_type            );
void arad_pp_dbal_sw_db_mpls_l1_mpls4_tcam_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 unused);
void arad_pp_dbal_sw_db_ipv6_dip_np_et_tcam_property_set                  (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 vid_type            ); 
void arad_pp_dbal_sw_db_ipv6_sip_vrf_comp_dest_tcam_property_set (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 key_type            ); 
void arad_pp_dbal_sw_db_mpls_l1_tcam_property_set                (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 unused);
void arad_pp_dbal_sw_db_mpls_l4b_l5g_tcam_property_set           (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 mpls_label);
void arad_pp_dbal_sw_db_generalized_rch_property_set (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 is_rch0            );
void arad_pp_dbal_sw_db_l2tp_property_set (int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 unused);
void arad_pp_dbal_sw_db_pppoe_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 unused);


 ARAD_PP_DBAL_VTT_TABLE     vtt_tables[SOC_DPP_NOF_VTT_SW_DB_TABLES] =
{

    { {0}, "VTT VDxINITAILVID SEM A"                   , SOC_DPP_DBAL_SW_TABLE_ID_VDxINITIALVID_SEM_A                                , arad_pp_dbal_sw_db_vdxvid_sem_property_set                   , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , INITIAL_VID           }, 
    { {0}, "VTT VDxINITAILVID SEM B"                   , SOC_DPP_DBAL_SW_TABLE_ID_VDxINITIALVID_SEM_B                                , arad_pp_dbal_sw_db_vdxvid_sem_property_set                   , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , INITIAL_VID           }, 
    { {0}, "VTT VDxOUTERVID SEM A"                     , SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_A                                  , arad_pp_dbal_sw_db_vdxvid_sem_property_set                   , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , OUTER_VID             }, 
    { {0}, "VTT VDxOUTERVID SEM B"                     , SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_B                                  , arad_pp_dbal_sw_db_vdxvid_sem_property_set                   , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , OUTER_VID             }, 
    { {0}, "VTT VDxOUTER_INNER_VID SEM A"              , SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTER_INNER_VID_SEM_A                           , arad_pp_dbal_sw_db_vdxvid_sem_property_set                   , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , OUTER_INNER_VID       }, 
    { {0}, "VTT VDxOUTER_INNER_VID SEM B"              , SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTER_INNER_VID_SEM_B                           , arad_pp_dbal_sw_db_vdxvid_sem_property_set                   , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , OUTER_INNER_VID       }, 
    { {0}, "VTT VDxOUTER_INITIAL_VID SEM A"            , SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTER_INITIAL_VID_SEM_A                         , arad_pp_dbal_sw_db_vdxvid_sem_property_set                   , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , OUTER_INITIAL_VID     }, 
    { {0}, "VTT VDxOUTER_INITIAL_VID SEM B"            , SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTER_INITIAL_VID_SEM_B                         , arad_pp_dbal_sw_db_vdxvid_sem_property_set                   , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , OUTER_INITIAL_VID     }, 
    { {0}, "VTT VDxOUTERVID PCP SEM A"                 , SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_PCP_SEM_A                              , arad_pp_dbal_sw_db_vdxvid_pcp_sem_property_set               , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , OUTER_VID             }, 
    { {0}, "VTT EVB SINGLE TAG SEM B"                  , SOC_DPP_DBAL_SW_TABLE_ID_EVB_SINGLE_TAG_SEM_B                               , arad_pp_dbal_sw_db_evb_sem_property_set                      , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , OUTER_VID             },
    { {0}, "VTT EVB DOUBLE TAG SEM B"                  , SOC_DPP_DBAL_SW_TABLE_ID_EVB_DOUBLE_TAG_SEM_B                               , arad_pp_dbal_sw_db_evb_sem_property_set                      , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , OUTER_INNER_VID       },
    { {0}, "VTT INITIAL VID TCAM"                      , SOC_DPP_DBAL_SW_TABLE_ID_INITIAL_VID_TCAM                                   , arad_pp_dbal_sw_db_vid_tcam_property_set                     , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM           , INITIAL_VID           }, 
    { {0}, "VTT OUTER VID TCAM"                        , SOC_DPP_DBAL_SW_TABLE_ID_OUTER_VID_TCAM                                     , arad_pp_dbal_sw_db_vid_tcam_property_set                     , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM           , OUTER_VID             }, 
    { {0}, "VTT INNER_OUTER VID PCP TCAM"              , SOC_DPP_DBAL_SW_TABLE_ID_INNER_OUTER_VID_PCP_TCAM                           , arad_pp_dbal_sw_db_inner_outer_vid_pcp_tcam_property_set     , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM           , 0                     },
    { {0}, "VTT SPOOFv4  SEM B"                        , SOC_DPP_DBAL_SW_TABLE_ID_SPOOFv4_SEM_B                                      , arad_pp_dbal_sw_db_spoofv4_property_set                      , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , 0                     }, 
    { {0}, "VTT DIPv6 TCAM"                            , SOC_DPP_DBAL_SW_TABLE_ID_DIPv6_TCAM                                         , arad_pp_dbal_sw_db_dipv6_tcam_property_set                   , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM           , IP_NOT_COMPRESSED     }, 
    { {0}, "VTT DIPv6 COMPRESSED TCAM"                 , SOC_DPP_DBAL_SW_TABLE_ID_DIPv6_COMPRESSED_TCAM                              , arad_pp_dbal_sw_db_dipv6_tcam_property_set                   , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM           , IP_COMPRESSED         }, 
    { {0}, "VTT SIPv6 TCAM"                            , SOC_DPP_DBAL_SW_TABLE_ID_SPOOFv6_TCAM                                       , arad_pp_dbal_sw_db_spoofv6_tcam_property_set                 , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM           , IP_NOT_COMPRESSED     }, 
    { {0}, "VTT SIPv6 COMPRESSED TCAM"                 , SOC_DPP_DBAL_SW_TABLE_ID_SPOOFv6_COMPRESSED_TCAM                            , arad_pp_dbal_sw_db_spoofv6_tcam_property_set                 , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM           , IP_COMPRESSED         }, 
                                                                                                                                                                                                       
    { {0}, "VTT MPLS DUMMY SEM A"                      , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_DUMMY_UNINDEXED_SEM_A                         , arad_pp_dbal_sw_db_mpls_unidexed_prefix_sem_property_set     , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_DUMMY            }, 
    { {0}, "VTT MPLS LABEL1 UNINDEXED SEM A"           , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_UNINDEXED_SEM_A                            , arad_pp_dbal_sw_db_mpls_unidexed_prefix_sem_property_set     , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_0          }, 
    { {0}, "VTT MPLS LABEL1 UNINDEXED SEM B"           , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_UNINDEXED_SEM_B                            , arad_pp_dbal_sw_db_mpls_unidexed_prefix_sem_property_set     , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_0          }, 
    { {0}, "VTT MPLS LABEL2 UNINDEXED SEM A"           , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_UNINDEXED_SEM_A                            , arad_pp_dbal_sw_db_mpls_unidexed_prefix_sem_property_set     , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_1          }, 
    { {0}, "VTT MPLS LABEL2 UNINDEXED SEM B"           , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_UNINDEXED_SEM_B                            , arad_pp_dbal_sw_db_mpls_unidexed_prefix_sem_property_set     , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_1          }, 
    { {0}, "VTT MPLS LABEL3 UNINDEXED SEM A"           , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_UNINDEXED_SEM_A                            , arad_pp_dbal_sw_db_mpls_unidexed_prefix_sem_property_set     , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_2          }, 
    { {0}, "VTT MPLS LABEL3 UNINDEXED SEM B"           , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_UNINDEXED_SEM_B                            , arad_pp_dbal_sw_db_mpls_unidexed_prefix_sem_property_set     , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_2          }, 
    { {0}, "VTT MPLS LABEL1 GAL SEM B"                 , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_GAL_SEM_B                                  , arad_pp_dbal_sw_db_mpls_gal_sem_property_set                 , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_0          }, 
    { {0}, "VTT MPLS LABEL2 GAL SEM B"                 , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_GAL_SEM_B                                  , arad_pp_dbal_sw_db_mpls_gal_sem_property_set                 , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_1          }, 
    { {0}, "VTT MPLS LABEL3 GAL SEM B"                 , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_GAL_SEM_B                                  , arad_pp_dbal_sw_db_mpls_gal_sem_property_set                 , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_2          }, 
    { {0}, "VTT MPLS LABEL port SEM B"                 , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_PORT_L1_L1IDX_SEM_B                           , arad_pp_dbal_sw_db_mpls_port_sem_property_set                , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_0          },
    { {0}, "VTT MPLS LABEL port SEM B"                 , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_PORT_L3_L1IDX_SEM_B                           , arad_pp_dbal_sw_db_mpls_port_sem_property_set                , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_0          },
    { {0}, "VTT MPLS LABEL port SEM A"                 , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_PORT_L1_L1IDX_SEM_A                           , arad_pp_dbal_sw_db_mpls_port_sem_property_set                , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_0          },
    { {0}, "VTT MPLS LABEL port SEM A"                 , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_PORT_L3_L1IDX_SEM_A                           , arad_pp_dbal_sw_db_mpls_port_sem_property_set                , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_0          },
    { {0}, "VTT MPLS LABEL1 L1 INDEXED SEM A"          , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L1IDX_SEM_A                                , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_0_IDX0     },
    { {0}, "VTT MPLS LABEL1 L1 INDEXED SEM B"          , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L1IDX_SEM_B                                , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_0_IDX0     },
    { {0}, "VTT MPLS LABEL1 L2 INDEXED SEM A"          , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L2IDX_SEM_A                                , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_0_IDX1     },
    { {0}, "VTT MPLS LABEL1 L2 INDEXED SEM B"          , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L2IDX_SEM_B                                , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_0_IDX1     },
    { {0}, "VTT MPLS LABEL1 L3 INDEXED SEM A"          , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L3IDX_SEM_A                                , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_0_IDX2     },
    { {0}, "VTT MPLS LABEL1 L4 INDEXED SEM B"          , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L3IDX_SEM_B                                , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_0_IDX2     },
    { {0}, "VTT MPLS LABEL1 L13 INDEXED SEM A"         , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L13IDX_SEM_A                               , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_0_IDX13    },
    { {0}, "VTT MPLS LABEL1 L13 INDEXED SEM B"         , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L13IDX_SEM_B                               , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_0_IDX13    },
    { {0}, "VTT MPLS LABEL1 L12 INDEXED SEM A"         , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L12IDX_SEM_A                               , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_0_IDX12    },
    { {0}, "VTT MPLS LABEL1 L12 INDEXED SEM B"         , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L12IDX_SEM_B                               , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_0_IDX12    },
    { {0}, "VTT MPLS LABEL2 L1 INDEXED SEM A"          , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L1IDX_SEM_A                                , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_1_IDX0     },
    { {0}, "VTT MPLS LABEL2 L1 INDEXED SEM B"          , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L1IDX_SEM_B                                , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_1_IDX0     },
    { {0}, "VTT MPLS LABEL2 L2 INDEXED SEM A"          , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L2IDX_SEM_A                                , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_1_IDX1     },
    { {0}, "VTT MPLS LABEL2 L2 INDEXED SEM B"          , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L2IDX_SEM_B                                , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_1_IDX1     },
    { {0}, "VTT MPLS LABEL2 L3 INDEXED SEM A"          , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L3IDX_SEM_A                                , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_1_IDX2     },
    { {0}, "VTT MPLS LABEL2 L4 INDEXED SEM B"          , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L3IDX_SEM_B                                , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_1_IDX2     },
    { {0}, "VTT MPLS LABEL2 L13 INDEXED SEM A"         , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L13IDX_SEM_A                               , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_1_IDX13    },
    { {0}, "VTT MPLS LABEL2 L13 INDEXED SEM B"         , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L13IDX_SEM_B                               , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_1_IDX13    },
    { {0}, "VTT MPLS LABEL2 L12 INDEXED SEM A"         , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L12IDX_SEM_A                               , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_1_IDX12    },
    { {0}, "VTT MPLS LABEL2 L12 INDEXED SEM B"         , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L12IDX_SEM_B                               , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_1_IDX12    },
    { {0}, "VTT MPLS LABEL3 L1 INDEXED SEM A"          , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_L1IDX_SEM_A                                , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_2_IDX0     },
    { {0}, "VTT MPLS LABEL3 L1 INDEXED SEM B"          , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_L1IDX_SEM_B                                , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_2_IDX0     },
    { {0}, "VTT MPLS LABEL3 L2 INDEXED SEM A"          , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_L2IDX_SEM_A                                , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_2_IDX1     },
    { {0}, "VTT MPLS LABEL3 L2 INDEXED SEM B"          , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_L2IDX_SEM_B                                , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_2_IDX1     },
    { {0}, "VTT MPLS LABEL3 L3 INDEXED SEM A"          , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_L3IDX_SEM_A                                , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_2_IDX2     },
    { {0}, "VTT MPLS LABEL3 L4 INDEXED SEM B"          , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_L3IDX_SEM_B                                , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_2_IDX2     },
    { {0}, "VTT MPLS LABEL3 L13 INDEXED SEM A"         , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_L13IDX_SEM_A                               , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_2_IDX13    },
    { {0}, "VTT MPLS LABEL3 L13 INDEXED SEM B"         , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_L13IDX_SEM_B                               , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_2_IDX13    },
    { {0}, "VTT MPLS LABEL3 L12 INDEXED SEM A"         , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_L12IDX_SEM_A                               , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_2_IDX12    },
    { {0}, "VTT MPLS LABEL3 L12 INDEXED SEM B"         , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_L12IDX_SEM_B                               , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_2_IDX12    },
    { {0}, "VTT MPLS LABEL4 L1 INDEXED SEM A"          , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_L1IDX_SEM_A                                , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_3_IDX0     },
    { {0}, "VTT MPLS LABEL4 L1 INDEXED SEM B"          , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_L1IDX_SEM_B                                , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_3_IDX0     },
    { {0}, "VTT MPLS LABEL4 L2 INDEXED SEM A"          , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_L2IDX_SEM_A                                , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_3_IDX1     },
    { {0}, "VTT MPLS LABEL4 L2 INDEXED SEM B"          , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_L2IDX_SEM_B                                , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_3_IDX1     },
    { {0}, "VTT MPLS LABEL4 L3 INDEXED SEM A"          , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_L3IDX_SEM_A                                , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_3_IDX2     },
    { {0}, "VTT MPLS LABEL4 L4 INDEXED SEM B"          , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_L3IDX_SEM_B                                , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_3_IDX2     },
    { {0}, "VTT MPLS LABEL4 L13 INDEXED SEM A"         , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_L13IDX_SEM_A                               , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_3_IDX13    },
    { {0}, "VTT MPLS LABEL4 L13 INDEXED SEM B"         , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_L13IDX_SEM_B                               , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_3_IDX13    },
    { {0}, "VTT MPLS LABEL4 L12 INDEXED SEM A"         , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_L12IDX_SEM_A                               , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_3_IDX12    },
    { {0}, "VTT MPLS LABEL4 L12 INDEXED SEM B"         , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_L12IDX_SEM_B                               , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_3_IDX12    },
    { {0}, "VTT MPLS LABEL1 BOS L1 INDEXED SEM A"      , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_BOS_L1IDX_SEM_A                            , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_0_BOS_IDX0 },
    { {0}, "VTT MPLS LABEL1 BOS L1 INDEXED SEM B"      , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_BOS_L1IDX_SEM_B                            , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_0_BOS_IDX0 },
    { {0}, "VTT MPLS LABEL1 BOS L2 INDEXED SEM A"      , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_BOS_L2IDX_SEM_A                            , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_0_BOS_IDX1 },
    { {0}, "VTT MPLS LABEL1 BOS L2 INDEXED SEM B"      , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_BOS_L2IDX_SEM_B                            , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_0_BOS_IDX1 },
    { {0}, "VTT MPLS LABEL1 BOS L3 INDEXED SEM A"      , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_BOS_L3IDX_SEM_A                            , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_0_BOS_IDX2 },
    { {0}, "VTT MPLS LABEL1 BOS L4 INDEXED SEM B"      , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_BOS_L3IDX_SEM_B                            , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_0_BOS_IDX2 },
    { {0}, "VTT MPLS LABEL1 BOS L13 INDEXED SEM A"     , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_BOS_L13IDX_SEM_A                           , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_0_BOS_IDX13},
    { {0}, "VTT MPLS LABEL1 BOS L13 INDEXED SEM B"     , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_BOS_L13IDX_SEM_B                           , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_0_BOS_IDX13},
    { {0}, "VTT MPLS LABEL1 BOS L12 INDEXED SEM A"     , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_BOS_L12IDX_SEM_A                           , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_0_BOS_IDX12},
    { {0}, "VTT MPLS LABEL1 BOS L12 INDEXED SEM B"     , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_BOS_L12IDX_SEM_B                           , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_0_BOS_IDX12},
    { {0}, "VTT MPLS LABEL2 BOS L1 INDEXED SEM A"      , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_BOS_L1IDX_SEM_A                            , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_1_BOS_IDX0 },
    { {0}, "VTT MPLS LABEL2 BOS L1 INDEXED SEM B"      , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_BOS_L1IDX_SEM_B                            , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_1_BOS_IDX0 },
    { {0}, "VTT MPLS LABEL2 BOS L2 INDEXED SEM A"      , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_BOS_L2IDX_SEM_A                            , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_1_BOS_IDX1 },
    { {0}, "VTT MPLS LABEL2 BOS L2 INDEXED SEM B"      , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_BOS_L2IDX_SEM_B                            , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_1_BOS_IDX1 },
    { {0}, "VTT MPLS LABEL2 BOS L3 INDEXED SEM A"      , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_BOS_L3IDX_SEM_A                            , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_1_BOS_IDX2 },
    { {0}, "VTT MPLS LABEL2 BOS L4 INDEXED SEM B"      , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_BOS_L3IDX_SEM_B                            , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_1_BOS_IDX2 },
    { {0}, "VTT MPLS LABEL2 BOS L13 INDEXED SEM A"     , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_BOS_L13IDX_SEM_A                           , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_1_BOS_IDX13},
    { {0}, "VTT MPLS LABEL2 BOS L13 INDEXED SEM B"     , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_BOS_L13IDX_SEM_B                           , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_1_BOS_IDX13},
    { {0}, "VTT MPLS LABEL2 BOS L12 INDEXED SEM A"     , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_BOS_L12IDX_SEM_A                           , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_1_BOS_IDX12},
    { {0}, "VTT MPLS LABEL2 BOS L12 INDEXED SEM B"     , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_BOS_L12IDX_SEM_B                           , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_1_BOS_IDX12},
    { {0}, "VTT MPLS LABEL3 BOS L1 INDEXED SEM A"      , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_BOS_L1IDX_SEM_A                            , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_2_BOS_IDX0 },
    { {0}, "VTT MPLS LABEL3 BOS L1 INDEXED SEM B"      , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_BOS_L1IDX_SEM_B                            , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_2_BOS_IDX0 },
    { {0}, "VTT MPLS LABEL3 BOS L2 INDEXED SEM A"      , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_BOS_L2IDX_SEM_A                            , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_2_BOS_IDX1 },
    { {0}, "VTT MPLS LABEL3 BOS L2 INDEXED SEM B"      , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_BOS_L2IDX_SEM_B                            , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_2_BOS_IDX1 },
    { {0}, "VTT MPLS LABEL3 BOS L3 INDEXED SEM A"      , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_BOS_L3IDX_SEM_A                            , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_2_BOS_IDX2 },
    { {0}, "VTT MPLS LABEL3 BOS L4 INDEXED SEM B"      , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_BOS_L3IDX_SEM_B                            , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_2_BOS_IDX2 },
    { {0}, "VTT MPLS LABEL3 BOS L13 INDEXED SEM A"     , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_BOS_L13IDX_SEM_A                           , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_2_BOS_IDX13},
    { {0}, "VTT MPLS LABEL3 BOS L13 INDEXED SEM B"     , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_BOS_L13IDX_SEM_B                           , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_2_BOS_IDX13},
    { {0}, "VTT MPLS LABEL3 BOS L12 INDEXED SEM A"     , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_BOS_L12IDX_SEM_A                           , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_2_BOS_IDX12},
    { {0}, "VTT MPLS LABEL3 BOS L12 INDEXED SEM B"     , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_BOS_L12IDX_SEM_B                           , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_2_BOS_IDX12},
    { {0}, "VTT MPLS LABEL4 BOS L1 INDEXED SEM A"      , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_BOS_L1IDX_SEM_A                            , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_3_BOS_IDX0 },
    { {0}, "VTT MPLS LABEL4 BOS L1 INDEXED SEM B"      , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_BOS_L1IDX_SEM_B                            , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_3_BOS_IDX0 },
    { {0}, "VTT MPLS LABEL4 BOS L2 INDEXED SEM A"      , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_BOS_L2IDX_SEM_A                            , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_3_BOS_IDX1 },
    { {0}, "VTT MPLS LABEL4 BOS L2 INDEXED SEM B"      , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_BOS_L2IDX_SEM_B                            , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_3_BOS_IDX1 },
    { {0}, "VTT MPLS LABEL4 BOS L3 INDEXED SEM A"      , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_BOS_L3IDX_SEM_A                            , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_3_BOS_IDX2 },
    { {0}, "VTT MPLS LABEL4 BOS L4 INDEXED SEM B"      , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_BOS_L3IDX_SEM_B                            , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_3_BOS_IDX2 },
    { {0}, "VTT MPLS LABEL4 BOS L13 INDEXED SEM A"     , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_BOS_L13IDX_SEM_A                           , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_3_BOS_IDX13},
    { {0}, "VTT MPLS LABEL4 BOS L13 INDEXED SEM B"     , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_BOS_L13IDX_SEM_B                           , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_3_BOS_IDX13},
    { {0}, "VTT MPLS LABEL4 BOS L12 INDEXED SEM A"     , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_BOS_L12IDX_SEM_A                           , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_3_BOS_IDX12},
    { {0}, "VTT MPLS LABEL4 BOS L12 INDEXED SEM B"     , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_BOS_L12IDX_SEM_B                           , arad_pp_dbal_sw_db_mpls_indexed_sem_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_3_BOS_IDX12},
    { {0}, "VTT MPLS LABEL1  ELI UNINDEXED SEM A"      , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_ELI_UNINDEXED_SEM_A                        , arad_pp_dbal_sw_db_mpls_eli_unidexed_prefix_sem_property_set , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_0          },
    { {0}, "VTT MPLS LABEL1  ELI UNINDEXED SEM B"      , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_ELI_UNINDEXED_SEM_B                        , arad_pp_dbal_sw_db_mpls_eli_unidexed_prefix_sem_property_set , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_0          },
    { {0}, "VTT MPLS LABEL2  ELI UNINDEXED SEM A"      , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_ELI_UNINDEXED_SEM_A                        , arad_pp_dbal_sw_db_mpls_eli_unidexed_prefix_sem_property_set , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_1          },
    { {0}, "VTT MPLS LABEL2  ELI UNINDEXED SEM B"      , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_ELI_UNINDEXED_SEM_B                        , arad_pp_dbal_sw_db_mpls_eli_unidexed_prefix_sem_property_set , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_1          },
    { {0}, "VTT MPLS LABEL1 ELI L1 INDEXED SEM A"      , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_ELI_L1IDX_SEM_A                            , arad_pp_dbal_sw_db_mpls_eli_indexed_sem_property_set         , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_0_IDX0     },
    { {0}, "VTT MPLS LABEL1 ELI L1 INDEXED SEM B"      , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_ELI_L1IDX_SEM_B                            , arad_pp_dbal_sw_db_mpls_eli_indexed_sem_property_set         , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_0_IDX0     },
    { {0}, "VTT MPLS LABEL1 ELI L2 INDEXED SEM A"      , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_ELI_L2IDX_SEM_A                            , arad_pp_dbal_sw_db_mpls_eli_indexed_sem_property_set         , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_0_IDX1     },
    { {0}, "VTT MPLS LABEL1 ELI L2 INDEXED SEM B"      , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_ELI_L2IDX_SEM_B                            , arad_pp_dbal_sw_db_mpls_eli_indexed_sem_property_set         , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_0_IDX1     },
    { {0}, "VTT MPLS LABEL1 ELI L3 INDEXED SEM A"      , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_ELI_L3IDX_SEM_A                            , arad_pp_dbal_sw_db_mpls_eli_indexed_sem_property_set         , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_0_IDX2     },
    { {0}, "VTT MPLS LABEL1 ELI L4 INDEXED SEM B"      , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_ELI_L3IDX_SEM_B                            , arad_pp_dbal_sw_db_mpls_eli_indexed_sem_property_set         , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_0_IDX2     },
    { {0}, "VTT MPLS LABEL1 ELI L13 INDEXED SEM A"     , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_ELI_L13IDX_SEM_A                           , arad_pp_dbal_sw_db_mpls_eli_indexed_sem_property_set         , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_0_IDX13    },
    { {0}, "VTT MPLS LABEL1 ELI L13 INDEXED SEM B"     , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_ELI_L13IDX_SEM_B                           , arad_pp_dbal_sw_db_mpls_eli_indexed_sem_property_set         , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_0_IDX13    },
    { {0}, "VTT MPLS LABEL1 ELI L12 INDEXED SEM A"     , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_ELI_L12IDX_SEM_A                           , arad_pp_dbal_sw_db_mpls_eli_indexed_sem_property_set         , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_0_IDX12    },
    { {0}, "VTT MPLS LABEL1 ELI L12 INDEXED SEM B"     , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_ELI_L12IDX_SEM_B                           , arad_pp_dbal_sw_db_mpls_eli_indexed_sem_property_set         , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_0_IDX12    },
    { {0}, "VTT MPLS LABEL2 ELI L1 INDEXED SEM A"      , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_ELI_L1IDX_SEM_A                            , arad_pp_dbal_sw_db_mpls_eli_indexed_sem_property_set         , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_1_IDX0     },
    { {0}, "VTT MPLS LABEL2 ELI L1 INDEXED SEM B"      , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_ELI_L1IDX_SEM_B                            , arad_pp_dbal_sw_db_mpls_eli_indexed_sem_property_set         , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_1_IDX0     },
    { {0}, "VTT MPLS LABEL2 ELI L2 INDEXED SEM A"      , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_ELI_L2IDX_SEM_A                            , arad_pp_dbal_sw_db_mpls_eli_indexed_sem_property_set         , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_1_IDX1     },
    { {0}, "VTT MPLS LABEL2 ELI L2 INDEXED SEM B"      , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_ELI_L2IDX_SEM_B                            , arad_pp_dbal_sw_db_mpls_eli_indexed_sem_property_set         , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_1_IDX1     },
    { {0}, "VTT MPLS LABEL2 ELI L3 INDEXED SEM A"      , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_ELI_L3IDX_SEM_A                            , arad_pp_dbal_sw_db_mpls_eli_indexed_sem_property_set         , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_1_IDX2     },
    { {0}, "VTT MPLS LABEL2 ELI L4 INDEXED SEM B"      , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_ELI_L3IDX_SEM_B                            , arad_pp_dbal_sw_db_mpls_eli_indexed_sem_property_set         , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_1_IDX2     },
    { {0}, "VTT MPLS LABEL2 ELI L13 INDEXED SEM A"     , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_ELI_L13IDX_SEM_A                           , arad_pp_dbal_sw_db_mpls_eli_indexed_sem_property_set         , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_1_IDX13    },
    { {0}, "VTT MPLS LABEL2 ELI L13 INDEXED SEM B"     , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_ELI_L13IDX_SEM_B                           , arad_pp_dbal_sw_db_mpls_eli_indexed_sem_property_set         , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_1_IDX13    },
    { {0}, "VTT MPLS LABEL2 ELI L12 INDEXED SEM A"     , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_ELI_L12IDX_SEM_A                           , arad_pp_dbal_sw_db_mpls_eli_indexed_sem_property_set         , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_1_IDX12    },
    { {0}, "VTT MPLS LABEL2 ELI L12 INDEXED SEM B"     , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_ELI_L12IDX_SEM_B                           , arad_pp_dbal_sw_db_mpls_eli_indexed_sem_property_set         , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_1_IDX12    },
    { {0}, "VTT RIF MPLS LABEL1 L1 INDEXED SEM A"      , SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_L1IDX_SEM_A                            , arad_pp_dbal_sw_db_mpls_rif_indexed_sem_property_set         , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_0_IDX0      },
    { {0}, "VTT RIF MPLS LABEL1 L1 INDEXED SEM B"      , SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_L1IDX_SEM_B                            , arad_pp_dbal_sw_db_mpls_rif_indexed_sem_property_set         , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_0_IDX0      },
    { {0}, "VTT RIF MPLS LABEL1 L2 INDEXED SEM A"      , SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_L2IDX_SEM_A                            , arad_pp_dbal_sw_db_mpls_rif_indexed_sem_property_set         , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_0_IDX1      },
    { {0}, "VTT RIF MPLS LABEL1 L2 INDEXED SEM B"      , SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_L2IDX_SEM_B                            , arad_pp_dbal_sw_db_mpls_rif_indexed_sem_property_set         , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_0_IDX1      },
    { {0}, "VTT RIF MPLS LABEL1 L3 INDEXED SEM A"      , SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_L3IDX_SEM_A                            , arad_pp_dbal_sw_db_mpls_rif_indexed_sem_property_set         , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_0_IDX2      },
    { {0}, "VTT RIF MPLS LABEL1 L4 INDEXED SEM B"      , SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_L3IDX_SEM_B                            , arad_pp_dbal_sw_db_mpls_rif_indexed_sem_property_set         , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_0_IDX2      },
    { {0}, "VTT RIF MPLS LABEL1 L13 INDEXED SEM A"     , SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_L13IDX_SEM_A                           , arad_pp_dbal_sw_db_mpls_rif_indexed_sem_property_set         , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_0_IDX13     },
    { {0}, "VTT RIF MPLS LABEL1 L13 INDEXED SEM B"     , SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_L13IDX_SEM_B                           , arad_pp_dbal_sw_db_mpls_rif_indexed_sem_property_set         , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_0_IDX13     },
    { {0}, "VTT RIF MPLS LABEL1 L12 INDEXED SEM A"     , SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_L12IDX_SEM_A                           , arad_pp_dbal_sw_db_mpls_rif_indexed_sem_property_set         , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_0_IDX12     },
    { {0}, "VTT RIF MPLS LABEL1 L12 INDEXED SEM B"     , SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_L12IDX_SEM_B                           , arad_pp_dbal_sw_db_mpls_rif_indexed_sem_property_set         , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_0_IDX12     },
    { {0}, "VTT RIF MPLS LABEL1 ELI L1 INDEXED SEM A"  , SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_ELI_L1IDX_SEM_A                        , arad_pp_dbal_sw_db_mpls_rif_indexed_sem_property_set         , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_0_ELI_IDX0  },
    { {0}, "VTT RIF MPLS LABEL1 ELI L1 INDEXED SEM B"  , SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_ELI_L1IDX_SEM_B                        , arad_pp_dbal_sw_db_mpls_rif_indexed_sem_property_set         , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_0_ELI_IDX0  },
    { {0}, "VTT RIF MPLS LABEL1 ELI L2 INDEXED SEM A"  , SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_ELI_L2IDX_SEM_A                        , arad_pp_dbal_sw_db_mpls_rif_indexed_sem_property_set         , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_0_ELI_IDX1  },
    { {0}, "VTT RIF MPLS LABEL1 ELI L2 INDEXED SEM B"  , SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_ELI_L2IDX_SEM_B                        , arad_pp_dbal_sw_db_mpls_rif_indexed_sem_property_set         , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_0_ELI_IDX1  },
    { {0}, "VTT RIF MPLS LABEL1 ELI L3 INDEXED SEM A"  , SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_ELI_L3IDX_SEM_A                        , arad_pp_dbal_sw_db_mpls_rif_indexed_sem_property_set         , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_0_ELI_IDX2  },
    { {0}, "VTT RIF MPLS LABEL1 ELI L4 INDEXED SEM B"  , SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_ELI_L3IDX_SEM_B                        , arad_pp_dbal_sw_db_mpls_rif_indexed_sem_property_set         , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_0_ELI_IDX2  },
    { {0}, "VTT RIF MPLS LABEL1 ELI L13 INDEXED SEM A" , SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_ELI_L13IDX_SEM_A                       , arad_pp_dbal_sw_db_mpls_rif_indexed_sem_property_set         , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_0_ELI_IDX13 },
    { {0}, "VTT RIF MPLS LABEL1 ELI L13 INDEXED SEM B" , SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_ELI_L13IDX_SEM_B                       , arad_pp_dbal_sw_db_mpls_rif_indexed_sem_property_set         , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_0_ELI_IDX13 },
    { {0}, "VTT RIF MPLS LABEL1 ELI L12 INDEXED SEM A" , SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_ELI_L12IDX_SEM_A                       , arad_pp_dbal_sw_db_mpls_rif_indexed_sem_property_set         , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_0_ELI_IDX12 },
    { {0}, "VTT RIF MPLS LABEL1 ELI L12 INDEXED SEM B" , SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_ELI_L12IDX_SEM_B                       , arad_pp_dbal_sw_db_mpls_rif_indexed_sem_property_set         , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_0_ELI_IDX12 },
    { {0}, "VTT MPLS LABEL1 + LABEL2 TCAM"             , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L2_TCAM                                    , arad_pp_dbal_sw_db_mpls_l1_l2_tcam_property_set              , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM           , 0                      },
    { {0}, "VTT MPLS FRR(L1 L2) INITIAL VID TCAM"      , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_FRR_INITIAL_VID_TCAM                          , arad_pp_dbal_sw_db_mpls_frr_coupling_tcam_property_set       , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM           , (MPLS_LABEL_0 << 2) | INITIAL_VID },
    { {0}, "VTT MPLS FRR(L1 L2) OUTER VID TCAM"        , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_FRR_OUTER_VID_TCAM                            , arad_pp_dbal_sw_db_mpls_frr_coupling_tcam_property_set       , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM           , (MPLS_LABEL_0 << 2) | OUTER_VID   },
    { {0}, "VTT MPLS MPLS4  INITIAL VID (L1)  TCAM"    , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_MPLS4_INITIAL_VID_L1_TCAM  ,                    arad_pp_dbal_sw_db_mpls_l1_mpls4_tcam_property_set       , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM           , INITIAL_VID   },
    { {0}, "VTT MPLS MPLS4  OUTER VID (L1)  TCAM"      , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_MPLS4_OUTER_VID_L1_TCAM  ,                      arad_pp_dbal_sw_db_mpls_l1_mpls4_tcam_property_set       , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM           , OUTER_VID   },
    { {0}, "VTT MPLS LABEL1 TCAM"                      , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_TCAM                                       , arad_pp_dbal_sw_db_mpls_l1_tcam_property_set                 , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM           , MPLS_LABEL_0   },
    { {0}, "VTT MPLS INITIAL VID LABEL4 BOS LABEL5 GAL TCAM"     , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_INITIAL_VID_L4B_L5G_TCAM            , arad_pp_dbal_sw_db_mpls_l4b_l5g_tcam_property_set            , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM           , INITIAL_VID   },
    { {0}, "VTT MPLS OUTER VID LABEL4 BOS LABEL5 GAL TCAM"       , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_OUTER_VID_L4B_L5G_TCAM              , arad_pp_dbal_sw_db_mpls_l4b_l5g_tcam_property_set            , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM           , OUTER_VID   },
    { {0}, "VTT VDxINNERVID SEM A"                     , SOC_DPP_DBAL_SW_TABLE_ID_VDxINNERVID_SEM_A                                  , arad_pp_dbal_sw_db_vdxvid_sem_property_set                   , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , INNER_VID             }, 
    { {0}, "VTT VDxINNERVID SEM B"                     , SOC_DPP_DBAL_SW_TABLE_ID_VDxINNERVID_SEM_B                                  , arad_pp_dbal_sw_db_vdxvid_sem_property_set                   , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , INNER_VID             }, 
    { {0}, "VTT VDxOUTER_INNER_PCP_VID SEM A"          , SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTER_INNER_PCP_VID_SEM_A                       , arad_pp_dbal_sw_db_vdxvid_sem_property_set                   , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , OUTER_INNER_VID_PCP   }, 
    { {0}, "VTT VDxOUTER_INNER_PCP_VID SEM B"          , SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTER_INNER_PCP_VID_SEM_B                       , arad_pp_dbal_sw_db_vdxvid_sem_property_set                   , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , OUTER_INNER_VID_PCP   }, 

                                                                                                                                                                                                      
    { {0}, "VTT TRILL SEM A"                           , SOC_DPP_DBAL_SW_TABLE_ID_TRILL_SEM_A                                        , arad_pp_dbal_sw_db_trill_property_set                        , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , 0                       },
    { {0}, "VTT DESIGNATED VID TRILL SEM B"            , SOC_DPP_DBAL_SW_TABLE_ID_DESIGNATED_VID_TRILL_SEM_B                         , arad_pp_dbal_sw_db_vdxvid_sem_property_set                   , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , TRILL_DESIGNATED_VID    },
    { {0}, "VTT TRILL INGRESS NICKNAME SEM A"          , SOC_DPP_DBAL_SW_TABLE_ID_TRILL_ING_NICK_SEM_A                               , arad_pp_dbal_sw_db_trill_ingress_nickname_property_set       , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , 0                       },
    { {0}, "VTT TRILL UC ONE TAG SEM B"                , SOC_DPP_DBAL_SW_TABLE_ID_TRILL_UC_ONE_TAG_SEM_B                             , arad_pp_dbal_sw_db_trill_uc_property_set                     , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , TRILL_ONE_TAG           },
    { {0}, "VTT TRILL UC TWO TAG SEM B"                , SOC_DPP_DBAL_SW_TABLE_ID_TRILL_UC_TWO_TAG_SEM_B                             , arad_pp_dbal_sw_db_trill_uc_property_set                     , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , TRILL_TWO_TAG           },
    { {0}, "VTT TRILL INGRESS NICKNAME TCAM"           , SOC_DPP_DBAL_SW_TABLE_ID_TRILL_TCAM                                         , arad_pp_dbal_sw_db_trill_tcam_property_set                   , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM           , 0                       },
    { {0}, "VTT DIPv4 SEM A"                           , SOC_DPP_DBAL_SW_TABLE_ID_DIPv4_SEM_A                                        , arad_pp_dbal_sw_db_dip_sem_property_set                      , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , NOT_DUMMY_TYPE          },
    { {0}, "VTT DIPv4 SEM B"                           , SOC_DPP_DBAL_SW_TABLE_ID_DIPv4_SEM_B                                        , arad_pp_dbal_sw_db_dip_sem_property_set                      , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , NOT_DUMMY_TYPE          },
    { {0}, "VTT DIPv4 DUMMY SEM A"                     , SOC_DPP_DBAL_SW_TABLE_ID_DIPv4_DUMMY_SEM_A                                  , arad_pp_dbal_sw_db_dip_sem_property_set                      , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , DUMMY_TYPE              },
    { {0}, "VTT DIPv4 DUMMY SEM B"                     , SOC_DPP_DBAL_SW_TABLE_ID_DIPv4_DUMMY_SEM_B                                  , arad_pp_dbal_sw_db_dip_sem_property_set                      , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , DUMMY_TYPE              },
    { {0}, "VTT SIPv4 SEM A"                           , SOC_DPP_DBAL_SW_TABLE_ID_SIPv4_SEM_A                                        , arad_pp_dbal_sw_db_sip_sem_property_set                      , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , 0                       },
    { {0}, "VTT DIP SIP INITIAL VID TCAM"              , SOC_DPP_DBAL_SW_TABLE_ID_DIP_SIP_INITIAL_VID_TCAM                           , arad_pp_dbal_sw_db_dip_sip_tcam_property_set                 , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM           , INITIAL_VID             },
    { {0}, "VTT DIP SIP OUTER VID TCAM"                , SOC_DPP_DBAL_SW_TABLE_ID_DIP_SIP_OUTER_VID_TCAM                             , arad_pp_dbal_sw_db_dip_sip_tcam_property_set                 , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM           , OUTER_VID               },
    { {0}, "VTT ETHERNET HEADER I-SID SEM A"           , SOC_DPP_DBAL_SW_TABLE_ID_ETHERNET_HEADER_ISID_SEM_A                         , arad_pp_dbal_sw_db_ethernet_header_isid_property_set         , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , 0                       },
    { {0}, "VTT ETHERNET HEADER BSA I-SID SEM B"       , SOC_DPP_DBAL_SW_TABLE_ID_ETHERNET_HEADER_BSA_ISID_SEM_B                     , arad_pp_dbal_sw_db_ethernet_header_bsa_isid_property_set     , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , 0                       },
    { {0}, "VTT MC DIPv4 RIF ID SEM A"                 , SOC_DPP_DBAL_SW_TABLE_ID_MC_DIPv4_RIF_SEM_A                                 , arad_pp_dbal_sw_db_mc_dipv4_rif_property_set                 , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , 0                       },
    { {0}, "VTT TUNNEL SEM B"                          , SOC_DPP_DBAL_SW_TABLE_ID_TUNNEL_SEM_B                                       , arad_pp_dbal_sw_db_tunnel_property_set                       , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , 0                       },
    { {0}, "VTT TUNNEL SEM B"                          , SOC_DPP_DBAL_SW_TABLE_VXLAN_ID_TUNNEL_SEM_B                                 , arad_pp_dbal_sw_db_tunnel_port_property_set                  , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , VXLAN                   },
    { {0}, "VTT TUNNEL SEM B"                          , SOC_DPP_DBAL_SW_TABLE_L2GRE_ID_TUNNEL_SEM_B                                 , arad_pp_dbal_sw_db_tunnel_port_property_set                  , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , L2_GRE                  },
    { {0}, "VTT SINGLE TAG VLAN DOMAIN SEM B"          , SOC_DPP_DBAL_SW_TABLE_ID_SINGLE_TAG_VD_SEM_B                                , arad_pp_dbal_sw_db_qinq_property_set                         , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , OUTER_VID               },
    { {0}, "VTT DOUBLE TAG VLAN DOMAIN A"              , SOC_DPP_DBAL_SW_TABLE_ID_DOUBLE_TAG_VD_SEM_A                                , arad_pp_dbal_sw_db_qinq_property_set                         , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , OUTER_INNER_VID         },
    { {0}, "VTT 5 TUPPLE TCAM"                         , SOC_DPP_DBAL_SW_TABLE_ID_5_TUPPLE_TCAM                                      , arad_pp_dbal_sw_db_5_tupple_tcam_property_set                , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM           , 0                       },
    { {0}, "VTT VLAN DOMAIN SEM A"                     , SOC_DPP_DBAL_SW_TABLE_ID_VD_SEM_A                                           , arad_pp_dbal_sw_db_vlan_domain_vid_property_set              , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , TST1_VID_UNTAGGED       },
    { {0}, "VTT INITIAL VID VLAN DOMAIN SEM A"         , SOC_DPP_DBAL_SW_TABLE_ID_INITIAL_VD_SEM_A                                   , arad_pp_dbal_sw_db_vlan_domain_vid_property_set              , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , TST1_VID_INITIAL_TAG    },
    { {0}, "VTT OUTER VID VLAN DOMAIN SEM B"           , SOC_DPP_DBAL_SW_TABLE_ID_OUTER_VD_SEM_B                                     , arad_pp_dbal_sw_db_vlan_domain_vid_property_set              , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , TST1_VID_CMPRSD_ONE_TAG },
    { {0}, "VTT OUTER INNER VID VLAN DOMAIN SEM A"     , SOC_DPP_DBAL_SW_TABLE_ID_OUTER_INNER_VD_SEM_A                               , arad_pp_dbal_sw_db_vlan_domain_vid_property_set              , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , TST1_VID_DOUBLE_TAG     },
    { {0}, "VTT TST1 UNTAGGED TCAM"                    , SOC_DPP_DBAL_SW_TABLE_ID_UNTAGGED_TST1_TCAM                                 , arad_pp_dbal_sw_db_tst1_tcam_property_set                    , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM           , TST1_VID_UNTAGGED       },
    { {0}, "VTT TST1 ONE TAG TCAM"                     , SOC_DPP_DBAL_SW_TABLE_ID_ONE_TAG_TST1_TCAM                                  , arad_pp_dbal_sw_db_tst1_tcam_property_set                    , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM           , TST1_VID_ONE_TAG        },
    { {0}, "VTT TST1 DOUBLE TCAM"                      , SOC_DPP_DBAL_SW_TABLE_ID_DOUBLE_TAG_TST1_TCAM                               , arad_pp_dbal_sw_db_tst1_tcam_property_set                    , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM           , TST1_VID_DOUBLE_TAG     },
                                                                                                                                                                                                                            
    { {0}, "VTT VRRP UNTAGGED TCAM"                    , SOC_DPP_DBAL_SW_TABLE_ID_UNTAGGED_VRRP_TCAM                                 , arad_pp_dbal_sw_db_vrrp_tcam_property_set                    , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM           , TST1_VID_UNTAGGED       },
    { {0}, "VTT VRRP ONE TAG TCAM"                     , SOC_DPP_DBAL_SW_TABLE_ID_ONE_TAG_VRRP_TCAM                                  , arad_pp_dbal_sw_db_vrrp_tcam_property_set                    , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM           , TST1_VID_ONE_TAG        },
    { {0}, "VTT VRRP DOUBLE TCAM"                      , SOC_DPP_DBAL_SW_TABLE_ID_DOUBLE_TAG_VRRP_TCAM                               , arad_pp_dbal_sw_db_vrrp_tcam_property_set                    , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM           , TST1_VID_DOUBLE_TAG     },
                                                                                                                                                                                                                            
    { {0}, "VTT TST1 MPLS TCAM"                        , SOC_DPP_DBAL_SW_TABLE_ID_TST1_MPLS_TCAM                                     , arad_pp_dbal_sw_db_tst1_explicit_null_tcam_property_set      , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM           , 0                       },
    { {0}, "VTT QinQ COMPRESSED TPID1 SEMA"            , SOC_DPP_DBAL_SW_TABLE_ID_QINQ_COMPRESSED_TPID1_SEM_A                        , arad_pp_dbal_sw_db_qinq_compressed_sem_property_set          , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , TPID1                   },
    { {0}, "VTT QinQ COMPRESSED TPID2 SEMA"            , SOC_DPP_DBAL_SW_TABLE_ID_QINQ_COMPRESSED_TPID2_SEM_A                        , arad_pp_dbal_sw_db_qinq_compressed_sem_property_set          , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , TPID2                   },
    { {0}, "VTT QinAny TPID1 SEMB"                     , SOC_DPP_DBAL_SW_TABLE_ID_QINANY_TPID1_SEM_B                                 , arad_pp_dbal_sw_db_qinany_sem_property_set                   , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , TPID1                   },
    { {0}, "VTT QinAny TPID2 SEMB"                     , SOC_DPP_DBAL_SW_TABLE_ID_QINANY_TPID2_SEM_B                                 , arad_pp_dbal_sw_db_qinany_sem_property_set                   , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , TPID2                   },
    { {0}, "VTT QinAny PCP TPID1 SEMB"                 , SOC_DPP_DBAL_SW_TABLE_ID_QINANY_PCP_TPID1_SEM_A                             , arad_pp_dbal_sw_db_qinany_pcp_sem_property_set               , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , TPID1                   },
    { {0}, "VTT QinAny PCP TPID2 SEMB"                 , SOC_DPP_DBAL_SW_TABLE_ID_QINANY_PCP_TPID2_SEM_A                             , arad_pp_dbal_sw_db_qinany_pcp_sem_property_set               , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , TPID2                   },
    { {0}, "VTT .1Q TPID1 SEMA"                        , SOC_DPP_DBAL_SW_TABLE_ID_1Q_TPID1_SEM_A                                     , arad_pp_dbal_sw_db_1q_sem_property_set                       , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , TPID1                   },
    { {0}, "VTT .1Q TPID2 SEMA"                        , SOC_DPP_DBAL_SW_TABLE_ID_1Q_TPID2_SEM_A                                     , arad_pp_dbal_sw_db_1q_sem_property_set                       , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , TPID2                   },
    { {0}, "VTT .1Q COMPRESSED TPID1 SEMB"             , SOC_DPP_DBAL_SW_TABLE_ID_1Q_COMPRESSED_TPID1_SEM_B                          , arad_pp_dbal_sw_db_1q_compressed_sem_property_set            , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , TPID1                   },
    { {0}, "VTT .1Q COMPRESSED TPID2 SEMB"             , SOC_DPP_DBAL_SW_TABLE_ID_1Q_COMPRESSED_TPID2_SEM_B                          , arad_pp_dbal_sw_db_1q_compressed_sem_property_set            , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , TPID2                   },
    { {0}, "VTT .1Q PCP COMPRESSED TPID1 SEMA"         , SOC_DPP_DBAL_SW_TABLE_ID_1Q_PCP_COMPRESSED_TPID1_SEM_A                      , arad_pp_dbal_sw_db_1q_pcp_compressed_sem_property_set        , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , TPID1                   },
    { {0}, "VTT .1Q PCP COMPRESSED TPID2 SEMA"         , SOC_DPP_DBAL_SW_TABLE_ID_1Q_PCP_COMPRESSED_TPID2_SEM_A                      , arad_pp_dbal_sw_db_1q_pcp_compressed_sem_property_set        , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , TPID2                   },
    { {0}, "VTT UNTAGGED  SEMA"                        , SOC_DPP_DBAL_SW_TABLE_ID_UNTAGGED_SEM_A                                     , arad_pp_dbal_sw_db_untagged_sem_property_set                 , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , 0                       },
    { {0}, "VTT PORT EXTENDER UNTAG CHECK SEMA"        , SOC_DPP_DBAL_SW_TABLE_ID_EXTENDER_UNTAG_CHECK_SEM_A                         , arad_pp_dbal_sw_db_extender_untag_check_sem_property_set     , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , 0                       },
    { {0}, "VTT PORT EXTENDER PE SEMA"                 , SOC_DPP_DBAL_SW_TABLE_ID_EXTENDER_PE_SEM_A                                  , arad_pp_dbal_sw_db_extender_pe_sem_property_set              , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , OUTER_VID               },
    { {0}, "VTT PORT EXTENDER PE UT SEMA"              , SOC_DPP_DBAL_SW_TABLE_ID_EXTENDER_PE_UT_SEM_A                               , arad_pp_dbal_sw_db_extender_pe_sem_property_set              , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , INITIAL_VID             },
    { {0}, "VTT PORT EXTENDER CHANNEL REG. SEMB"       , SOC_DPP_DBAL_SW_TABLE_ID_EXTENDER_CHANNEL_REG_SEM_B                         , arad_pp_dbal_sw_db_extender_channel_reg_sem_property_set     , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , 0                       },
    { {0}, "VTT TST2 TCAM"                             , SOC_DPP_DBAL_SW_TABLE_ID_TST2_TCAM                                          , arad_pp_dbal_sw_db_tst2_tcam_property_set                    , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM           , 0                       },
    { {0}, "VTT TEST2 TCAM"                            , SOC_DPP_DBAL_SW_TABLE_ID_TEST2_TCAM                                         , arad_pp_dbal_sw_db_test2_tcam_property_set                   , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM           , 0                       },
    { {0}, "VTT TEST2 SEMA"                            , SOC_DPP_DBAL_SW_TABLE_ID_TEST2_SEM_A                                        , arad_pp_dbal_sw_db_test2_sem_property_set                    , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , 0                       },
    { {0}, "VTT FULL MY MAC OUTER VID TCAM"            , SOC_DPP_DBAL_SW_TABLE_ID_SRC_PORT_DA_OUTER_VID_TCAM                         , arad_pp_dbal_sw_db_src_port_da_tcam_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM           , OUTER_VID               },
    { {0}, "VTT FULL MY MAC INITIAL VID TCAM"          , SOC_DPP_DBAL_SW_TABLE_ID_SRC_PORT_DA_INITIAL_VID_TCAM                       , arad_pp_dbal_sw_db_src_port_da_tcam_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM           , 0                       },
    { {0}, "VTT DIP SIP VRF TCAM"                      , SOC_DPP_DBAL_SW_TABLE_ID_DIP_SIP_VRF_TCAM                                   , arad_pp_dbal_sw_db_dip_sip_vrf_tcam_property_set             , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM           , 0                       },
    { {0}, "VTT DUMMY GRE SEM B"                       , SOC_DPP_DBAL_SW_TABLE_ID_GRE_DUMMY_SEM_B                                    , arad_pp_dbal_sw_db_gre_port_property_set                     , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , 0                       },
    { {0}, "VTT MPLS LABEL4 UNINDEXED SEM A"           , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_UNINDEXED_SEM_A                            , arad_pp_dbal_sw_db_mpls_unidexed_prefix_sem_property_set     , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_3            }, 
    { {0}, "VTT MPLS LABEL4 UNINDEXED SEM B"           , SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_UNINDEXED_SEM_B                            , arad_pp_dbal_sw_db_mpls_unidexed_prefix_sem_property_set     , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_3            }, 
    { {0}, "VTT MY-VTEP-INDEX SIP VRF SEM A"           , SOC_DPP_DBAL_SW_TABLE_ID_MY_VTEP_INDEX_SIP_VRF_SEM_A                        , arad_pp_dbal_sw_db_my_vtep_index_sip_vrf_sem_property_set    , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , 0                       },
    { {0}, "VTT CUSTOM PP PORT SEM A"                  , SOC_DPP_DBAL_SW_TABLE_ID_CUSTOM_PP_PORT_TUNNEL_SEMA                         , arad_pp_dbal_sw_db_custom_pp_port_tunnel_property_set        , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , 0                       },
    { {0}, "VTT CUSTOM PP PORT LSP ECMP TCAM"                  , SOC_DPP_DBAL_SW_TABLE_ID_CUSTOM_PP_PORT_LSP_ECMP_TCAM, arad_pp_dbal_sw_db_custom_pp_port_lsp_ecmp_property_set        , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM          , 0                       },
    { {0}, "VTT VLAN DOMAIN MPLS L1 SEM A"             , SOC_DPP_DBAL_SW_TABLE_ID_VLAN_DOMAIN_MPLS_L1_SEMA                                , arad_pp_dbal_sw_db_vlan_domain_mpls_property_set               , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_0                       },
    { {0}, "VTT VLAN DOMAIN MPLS L1 SEM B"             , SOC_DPP_DBAL_SW_TABLE_ID_VLAN_DOMAIN_MPLS_L1_SEMB                                , arad_pp_dbal_sw_db_vlan_domain_mpls_property_set               , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_0                       },
    { {0}, "VTT VLAN DOMAIN MPLS L2 SEM A"             , SOC_DPP_DBAL_SW_TABLE_ID_VLAN_DOMAIN_MPLS_L2_SEMA                                , arad_pp_dbal_sw_db_vlan_domain_mpls_property_set               , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , MPLS_LABEL_1                       },
    { {0}, "VTT VLAN DOMAIN MPLS L2 SEM B"             , SOC_DPP_DBAL_SW_TABLE_ID_VLAN_DOMAIN_MPLS_L2_SEMB                                , arad_pp_dbal_sw_db_vlan_domain_mpls_property_set               , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , MPLS_LABEL_1                       },
    { {0}, "VTT OAM STATISTICS SEM A"                  , SOC_DPP_DBAL_SW_TABLE_ID_OAM_STAT_SEM_A                                     , arad_pp_dbal_sw_db_oam_stat_property_set                     , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , OAM_FWD_CODE_0          },
    { {0}, "VTT OAM STATISTICS SEM B"                  , SOC_DPP_DBAL_SW_TABLE_ID_OAM_STAT_SEM_B                                     , arad_pp_dbal_sw_db_oam_stat_property_set                     , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , OAM_FWD_CODE_1          },
    { {0}, "VTT BFD STATISTICS SEM A"                  , SOC_DPP_DBAL_SW_TABLE_ID_BFD_STAT_SEM_A                                     , arad_pp_dbal_sw_db_bfd_stat_property_set                     , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , BFD_FWD_CODE_0          },
    { {0}, "VTT BFD STATISTICS SEM B"                  , SOC_DPP_DBAL_SW_TABLE_ID_BFD_STAT_SEM_B                                     , arad_pp_dbal_sw_db_bfd_stat_property_set                     , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , BFD_FWD_CODE_1          },
    { {0}, "VTT TM STATISTICS SEM A"                   , SOC_DPP_DBAL_SW_TABLE_ID_TM_STAT_SEM_A                                      , arad_pp_dbal_sw_db_tm_stat_property_set                      , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , IS_OAM_STAT             },
    { {0}, "VTT TM STATISTICS SEM B"                   , SOC_DPP_DBAL_SW_TABLE_ID_TM_STAT_SEM_B                                      , arad_pp_dbal_sw_db_tm_stat_property_set                      , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , IS_BFD_STAT             },
    { {0}, "VTT FC WITH VFT VSAN FROM VFT SEM B"       , SOC_DPP_DBAL_SW_TABLE_ID_FC_WITH_VFT_VSAN_VFT_SEMB                          , arad_pp_dbal_sw_db_fc_with_vft_property_set                  , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , VSAN_FROM_VFT           },
    { {0}, "VTT FC WITH VFT VSAN FROM VSI SEM B"       , SOC_DPP_DBAL_SW_TABLE_ID_FC_WITH_VFT_VSAN_VSI_SEMB                          , arad_pp_dbal_sw_db_fc_with_vft_property_set                  , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , VSAN_FROM_VSI           },
    { {0}, "VTT FC VSAN FROM VFT SEM B"                , SOC_DPP_DBAL_SW_TABLE_ID_FC_VSAN_VFT_SEMB                                   , arad_pp_dbal_sw_db_fc_property_set                           , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , VSAN_FROM_VFT           },
    { {0}, "VTT FC VSAN FROM VSI SEM B"                , SOC_DPP_DBAL_SW_TABLE_ID_FC_VSAN_VSI_SEMB                                   , arad_pp_dbal_sw_db_fc_property_set                           , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , VSAN_FROM_VSI           },
    { {0}, "VTT PON TUNNEL SEMA"                       , SOC_DPP_DBAL_SW_TABLE_ID_PON_VDxTUNNEL_SEM_A                                , arad_pp_dbal_sw_db_pon_vdxtunnelxvid_property_set            , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , PON_TUNNEL_SEMA                        },
    { {0}, "VTT PON TUNNEL SEMB"                       , SOC_DPP_DBAL_SW_TABLE_ID_PON_VDxTUNNEL_SEM_B                                , arad_pp_dbal_sw_db_pon_vdxtunnelxvid_property_set            , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , PON_TUNNEL_SEMB                        },
    { {0}, "VTT PON TUNNEL TCAM"                       , SOC_DPP_DBAL_SW_TABLE_ID_PON_VDxTUNNELxETHTYPE_TCAM                         , arad_pp_dbal_sw_db_pon_vdxtunnelxvid_property_set            , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM           , PON_TUNNEL_ETHTYPE                     },
    { {0}, "VTT PON TUNNEL OUTER_VID SEMA"             , SOC_DPP_DBAL_SW_TABLE_ID_PON_VDxTUNNELxOTUER_VID_SEM_A                      , arad_pp_dbal_sw_db_pon_vdxtunnelxvid_property_set            , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , PON_TUNNEL_OUTER_VID_SEMA              },
    { {0}, "VTT PON TUNNEL PCP OUTER_VID TCAM"         , SOC_DPP_DBAL_SW_TABLE_ID_PON_VDxTUNNELxETHTYPExPCPxOUTER_VID_TCAM           , arad_pp_dbal_sw_db_pon_vdxtunnelxvid_property_set            , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM           , PON_TUNNEL_ETHTYPE_PCP_OUTER_VID       },
    { {0}, "VTT PON TUNNEL OUTER_INNER_VID SEMA"       , SOC_DPP_DBAL_SW_TABLE_ID_PON_VDxTUNNELxOTUER_VIDxINNER_VID_SEM_A            , arad_pp_dbal_sw_db_pon_vdxtunnelxvid_property_set            , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , PON_TUNNEL_OUTER_INNER_VID_SEMA        },
    { {0}, "VTT PON TUNNEL OUTER_VID SEMB"             , SOC_DPP_DBAL_SW_TABLE_ID_PON_VDxTUNNELxOTUER_VID_SEM_B                      , arad_pp_dbal_sw_db_pon_vdxtunnelxvid_property_set            , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , PON_TUNNEL_OUTER_VID_SEMB              },
    { {0}, "VTT PON TUNNEL OUTER_INNER_VID SEMB"       , SOC_DPP_DBAL_SW_TABLE_ID_PON_VDxTUNNELxOTUER_VIDxINNER_VID_SEM_B            , arad_pp_dbal_sw_db_pon_vdxtunnelxvid_property_set            , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B          , PON_TUNNEL_OUTER_INNER_VID_SEMB        },
    { {0}, "VTT PON TUNNEL PCP OUTER_INNER_VID TCAM"   , SOC_DPP_DBAL_SW_TABLE_ID_PON_VDxTUNNELxETHTYPExPCPxOUTER_VIDxINNER_VID_TCAM , arad_pp_dbal_sw_db_pon_vdxtunnelxvid_property_set            , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM           , PON_TUNNEL_ETHTYPE_PCP_OUTER_INNER_VID },
    { {0}, "VTT SECTION OAM PORT ETH OUT_VID TCAM"     , SOC_DPP_DBAL_SW_TABLE_ID_SECTION_OAM_VDxOTUER_VIDxETH_TCAM                  , arad_pp_dbal_sw_db_section_oam_property_set                  , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM            , SECTION_OAM_PORT_OUTER_VID_ETH },
    { {0}, "VTT SECTION OAM PORT ETH  OUT_INNER_VID TCAM"   , SOC_DPP_DBAL_SW_TABLE_ID_SECTION_OAM_VDxOTUER_VIDxINNER_VIDxETH_TCAM   , arad_pp_dbal_sw_db_section_oam_property_set                  , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM            , SECTION_OAM_PORT_OUTER_INNER_VID_ETH },
    { {0}, "VTT IPV6 DIP NP ET INITIAL VID TCAM"       , SOC_DPP_DBAL_SW_TABLE_ID_IPV6_DIP_NP_ET_INITIAL_VID_TCAM                    , arad_pp_dbal_sw_db_ipv6_dip_np_et_tcam_property_set          , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM           , INITIAL_VID     },
    { {0}, "VTT IPV6 DIP NP ET OUTER VID TCAM"         , SOC_DPP_DBAL_SW_TABLE_ID_IPV6_DIP_NP_ET_OUTER_VID_TCAM                      , arad_pp_dbal_sw_db_ipv6_dip_np_et_tcam_property_set          , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM           , OUTER_VID     },
    { {0}, "VTT IPV6 SIP VRF COMP DEST TCAM"           , SOC_DPP_DBAL_SW_TABLE_ID_IPV6_SIP_VRF_COMP_DEST_TCAM                        , arad_pp_dbal_sw_db_ipv6_sip_vrf_comp_dest_tcam_property_set  , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM           , 0     },
    { {0}, "VTT IPV6 DIP NP ET INITIAL VID AFTER RECYCLE TCAM" , SOC_DPP_DBAL_SW_TABLE_ID_IPV6_DIP_NP_ET_INITIAL_VID_AFTER_RECYCLE_TCAM , arad_pp_dbal_sw_db_ipv6_dip_np_et_tcam_property_set       , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM           , INITIAL_VID     },
    { {0}, "VTT IPV6 DIP NP ET OUTER VID AFTER RECYCLE TCAM"   , SOC_DPP_DBAL_SW_TABLE_ID_IPV6_DIP_NP_ET_OUTER_VID_AFTER_RECYCLE_TCAM   , arad_pp_dbal_sw_db_ipv6_dip_np_et_tcam_property_set       , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM           , OUTER_VID     },
    { {0}, "VTT DIPv6 AFTER RECYCLE TCAM"                     , SOC_DPP_DBAL_SW_TABLE_ID_DIPv6_AFTER_RECYCLE_TCAM                    , arad_pp_dbal_sw_db_dipv6_tcam_property_set                   , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM           , IP_NOT_COMPRESSED     }, 
    { {0}, "VTT_GENERALIZED_RCH0_TCAM"                 , SOC_DPP_DBAL_SW_TABLE_ID_GENERALIZED_RCH0_TCAM                              , arad_pp_dbal_sw_db_generalized_rch_property_set              , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM           , 1                       }, 
    { {0}, "VTT_GENERALIZED_RCH1_TCAM"                 , SOC_DPP_DBAL_SW_TABLE_ID_GENERALIZED_RCH1_TCAM                              , arad_pp_dbal_sw_db_generalized_rch_property_set              , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM           , 0                       }, 
    { {0}, "VTT L2TP"                                  , SOC_DPP_DBAL_SW_TABLE_ID_L2TP_ISEM_A                                 , arad_pp_dbal_sw_db_l2tp_property_set                     , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , DUMMY_FOR_EXPLICIT_NULL_TYPE            }, 
    { {0}, "VTT PPPOE"                                 , SOC_DPP_DBAL_SW_TABLE_ID_PPPOE_ISEM_A                                , arad_pp_dbal_sw_db_pppoe_property_set                    , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A          , DUMMY_FOR_EXPLICIT_NULL_TYPE            }, 
    { {0}, "VTT DIPv4 DUMMY EXPLICIT NULL SEM A"	   , SOC_DPP_DBAL_SW_TABLE_ID_DIPv4_EXPLICIT_NULL_DUMMY_SEM_A 		     , arad_pp_dbal_sw_db_dip_sem_property_set                      , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_A		   , DUMMY_FOR_EXPLICIT_NULL_TYPE 			 },
    { {0}, "VTT DIPv4 DUMMY EXPLICIT NULL SEM B"	   , SOC_DPP_DBAL_SW_TABLE_ID_DIPv4_EXPLICIT_NULL_DUMMY_SEM_B 		     , arad_pp_dbal_sw_db_dip_sem_property_set	                    , SOC_DPP_DBAL_PHYSICAL_DB_TYPE_SEM_B		   , DUMMY_FOR_EXPLICIT_NULL_TYPE 			 },
};







void arad_pp_dbal_vt_program_tm_property_set                                     (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 tm_stat    );
void arad_pp_dbal_vt_program_vdxinitialvid_property_set                          (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused     );
void arad_pp_dbal_vt_program_vdxinitialvid_l1_property_set                       (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused    );
void arad_pp_dbal_vt_program_vdxoutervid_property_set                            (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused     );
void arad_pp_dbal_vt_program_vdxoutervid_l1_property_set                         (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused     );
void arad_pp_dbal_vt_program_vdxouter_inner_vid_property_set                     (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused     );
void arad_pp_dbal_vt_program_vdxouter_inner_vid_or_outervid_property_set         (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused     );
void arad_pp_dbal_vt_program_vid_l1frr_property_set                              (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 vid_type   );
void arad_pp_dbal_vt_program_outer_pcp_property_set                              (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused     );
void arad_pp_dbal_vt_program_outer_inner_pcp_property_set                        (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused     );
void arad_pp_dbal_vt_program_outer_inner_pcp_or_outer_inner_or_outer_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused     );
void arad_pp_dbal_vt_program_vid_vid_property_set                                (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 vid_type   );
void arad_pp_dbal_vt_program_indexed_vid_l1frr_property_set                      (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 vid_type   );
void arad_pp_dbal_vt_program_vdxinitialvid_trill_property_set                    (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused     );
void arad_pp_dbal_vt_program_vdxoutervid_trill_property_set                      (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused     );
void arad_pp_dbal_vt_program_vdxouter_inner_vid_trill_property_set               (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused     );
void arad_pp_dbal_vt_program_vd_designated_vid_trill_property_set                (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused     );
void arad_pp_dbal_vt_program_vdxinitialvid_l1l2_property_set                     (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused     );
void arad_pp_dbal_vt_program_vdxoutervid_l1l2_property_set                       (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused     );
void arad_pp_dbal_vt_program_vdxinitialvid_l1_l2eli_property_set                 (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused     );
void arad_pp_dbal_vt_program_vdxoutervid_l1_l2eli_property_set                   (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused     );
void arad_pp_dbal_vt_program_mpls_port_termination_property_set                  (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused     );
void arad_pp_dbal_vt_program_indexed_vdxinitialvid_property_set                  (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 mpls_label );
void arad_pp_dbal_vt_program_indexed_vdxoutervid_property_set                    (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 mpls_label );
void arad_pp_dbal_vt_program_ipv4_vdxinitial_property_set                        (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused     );
void arad_pp_dbal_vt_program_ipv4_vdxouter_property_set                          (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused     );
void arad_pp_dbal_vt_program_ipv4_port_termination_property_set                  (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused     );
void arad_pp_dbal_vt_program_evb_property_set                                    (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 vid_type   );
void arad_pp_dbal_vt_program_double_tag_priority_property_set                    (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused     );
void arad_pp_dbal_vt_program_double_tag_property_set                                          (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused     );
void arad_pp_dbal_vt_program_ipv4_vdxinitial_recycle_property_set                (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused     );
void arad_pp_dbal_vt_program_ipv4_vdxouter_recycle_property_set                  (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused     );
void arad_pp_dbal_vt_program_single_tag_5tupple_qinq_property_set                (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused     );
void arad_pp_dbal_vt_program_double_tag_5tupple_qinq_property_set                (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused     );
void arad_pp_dbal_vt_program_tst1_property_set                                   (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 vid_type   );
void arad_pp_dbal_vt_program_vrrp_property_set                                   (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 vid_type   );
void arad_pp_dbal_vt_program_exp_null_tst1_property_set                          (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 vid_type   );
void arad_pp_dbal_vt_program_outer_inner_pcp_tst2_property_set                   (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 tpid       );
void arad_pp_dbal_vt_program_outer_pcp_tst2_property_set                         (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 tpid       );
void arad_pp_dbal_vt_program_outer_inner_tst2_property_set                       (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 tpid       );
void arad_pp_dbal_vt_program_outer_tst2_property_set                             (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 tpid       );
void arad_pp_dbal_vt_program_untagged_tst2_property_set                          (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused     );
void arad_pp_dbal_vt_program_test2_property_set                                  (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused     );
void arad_pp_dbal_vt_program_extender_pe_property_set                            (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 vid_type   );
void arad_pp_dbal_vt_program_custom_pp_port_tunnel_property_set                  (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused     );
void arad_pp_dbal_vt_program_vlan_domain_mpls_property_set                       (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused);
void arad_pp_dbal_vt_program_pon_untagged_property_set                           (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused);
void arad_pp_dbal_vt_program_pon_one_tag_property_set                            (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused);
void arad_pp_dbal_vt_program_pon_two_tags_property_set                           (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused);
void arad_pp_dbal_vt_program_pon_two_tags_vs_tunnel_id_property_set              (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused);
void arad_pp_dbal_vt_program_section_oam_one_tag_property_set                    (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused);
void arad_pp_dbal_vt_program_section_oam_two_tags_property_set                   (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused);
void arad_pp_dbal_vt_program_indexed_vdxinitialvid_mpls4_l1l2_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 mpls_label );
void arad_pp_dbal_vt_program_indexed_vdxoutervid_mpls4_l1l2_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 mpls_label );
void arad_pp_dbal_vt_program_ipv6_vdxinitial_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 is_recycle);
void arad_pp_dbal_vt_program_ipv6_vdxouter_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 is_recycle);
void arad_pp_dbal_vt_program_3mtse_plus_frr_vdxinitialvid_l2l1_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 mpls_label );
void arad_pp_dbal_vt_program_3mtse_plus_frr_vdxOuterVid_l2l1_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 mpls_label );
void arad_pp_dbal_vt_program_3mtse_plus_frr_vdxinitialvid_l2_l4b_l5g_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 mpls_label );
void arad_pp_dbal_vt_program_3mtse_plus_frr_vdxOuterVid_l2_l4b_l5g_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 mpls_label );
void arad_pp_dbal_vt_program_2mtse_plus_frr_vid_frr_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 vid_type);
void arad_pp_dbal_vt_program_generalized_rch_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 is_rch0);


ARAD_PP_DBAL_VTT_PROGRAM_INFO vt_dbal_programs[ARAD_PP_DBAL_VT_NOF_PROGRAMS] = {

    {ARAD_PP_DBAL_VT_PROG_TM                                                        , arad_pp_dbal_vt_program_tm_property_set                                      , {-1}    , TM_STAT_OFF     },
    {ARAD_PP_DBAL_VT_PROG_TM_STAT                                                   , arad_pp_dbal_vt_program_tm_property_set                                      , {-1}    , TM_STAT_ON      },
    {ARAD_PP_DBAL_VT_PROG_VDxINITIALVID                                             , arad_pp_dbal_vt_program_vdxinitialvid_property_set                           , {-1}    , 0               },
    {ARAD_PP_DBAL_VT_PROG_VDxOUTERVID                                               , arad_pp_dbal_vt_program_vdxoutervid_property_set                             , {-1}    , 0               },
    {ARAD_PP_DBAL_VT_PROG_VDxINITIALVID_L1                                          , arad_pp_dbal_vt_program_vdxinitialvid_l1_property_set                        , {-1}    , 0               },
    {ARAD_PP_DBAL_VT_PROG_VDxOUTERVID_L1                                            , arad_pp_dbal_vt_program_vdxoutervid_l1_property_set                          , {-1}    , 0               },
    {ARAD_PP_DBAL_VT_PROG_VDxOUTER_INNER_VID_L1                                     , arad_pp_dbal_vt_program_vdxouter_inner_vid_property_set                      , {-1}    , 0               },
    {ARAD_PP_DBAL_VT_PROG_VDxOUTER_INNER_VID_OR_OUTER_VID_L1                        , arad_pp_dbal_vt_program_vdxouter_inner_vid_or_outervid_property_set          , {-1}    , 0               },
    {ARAD_PP_DBAL_VT_PROG_VD_INITIALVID_L1FRR                                       , arad_pp_dbal_vt_program_vid_l1frr_property_set                               , {-1}    , INITIAL_VID     },
    {ARAD_PP_DBAL_VT_PROG_VD_OUTERVID_L1FRR                                         , arad_pp_dbal_vt_program_vid_l1frr_property_set                               , {-1}    , OUTER_VID       },
    {ARAD_PP_DBAL_VT_PROG_VD_OUTER_OR_VD_OUTER_PCP                                  , arad_pp_dbal_vt_program_outer_pcp_property_set                               , {-1}    , 0               },
    {ARAD_PP_DBAL_VT_PROG_VD_OUTER_INNER_OR_VD_OUTER_INNER_OUTERPCP                 , arad_pp_dbal_vt_program_outer_inner_pcp_property_set                         , {-1}    , 0               },
    {ARAD_PP_DBAL_VT_PROG_HIGH_VD_OUTER_INNER_OUTERPCP_OR_VD_OUTER_INNER_OR_VD_OUTER, arad_pp_dbal_vt_program_outer_inner_pcp_or_outer_inner_or_outer_property_set , {-1}    , 0               },
    {ARAD_PP_DBAL_VT_PROG_VD_INITIALVID_OR_VD_INITIALVID                            , arad_pp_dbal_vt_program_vid_vid_property_set                                 , {-1}    , INITIAL_VID     },
    {ARAD_PP_DBAL_VT_PROG_VD_OUTER_OR_VD_OUTER                                      , arad_pp_dbal_vt_program_vid_vid_property_set                                 , {-1}    , OUTER_VID       },
    {ARAD_PP_DBAL_VT_PROG_VD_OUTER_INNER_OR_VD_OUTER_INNER                          , arad_pp_dbal_vt_program_vid_vid_property_set                                 , {-1}    , OUTER_INNER_VID },
    {ARAD_PP_DBAL_VT_PROG_INDX_VD_INITIALVID_FRR_L1                                 , arad_pp_dbal_vt_program_indexed_vid_l1frr_property_set                       , {-1}    , INITIAL_VID     },
    {ARAD_PP_DBAL_VT_PROG_INDX_VD_OUTERVID_FRR_L1                                   , arad_pp_dbal_vt_program_indexed_vid_l1frr_property_set                       , {-1}    , OUTER_VID       },
    {ARAD_PP_DBAL_VT_PROG_VDxINITIALVID_TRILL                                       , arad_pp_dbal_vt_program_vdxinitialvid_trill_property_set                     , {-1}    , 0               },
    {ARAD_PP_DBAL_VT_PROG_VDxOUTERVID_TRILL                                         , arad_pp_dbal_vt_program_vdxoutervid_trill_property_set                       , {-1}    , 0               },
    {ARAD_PP_DBAL_VT_PROG_VDxOUTER_INNER_VID_TRILL                                  , arad_pp_dbal_vt_program_vdxouter_inner_vid_trill_property_set                , {-1}    , 0               },
    {ARAD_PP_DBAL_VT_PROG_VD_DESIGNATED_VID_TRILL                                   , arad_pp_dbal_vt_program_vd_designated_vid_trill_property_set                 , {-1}    , 0               },
    {ARAD_PP_DBAL_VT_PROG_VDxINITIALVID_L1_L2ELI                                    , arad_pp_dbal_vt_program_vdxinitialvid_l1_l2eli_property_set                  , {-1}    , 0               },
    {ARAD_PP_DBAL_VT_PROG_VDxOUTERVID_L1_L2ELI                                      , arad_pp_dbal_vt_program_vdxoutervid_l1_l2eli_property_set                    , {-1}    , 0               },
    {ARAD_PP_DBAL_VT_PROG_VDxINITIALVID_L1L2                                        , arad_pp_dbal_vt_program_vdxinitialvid_l1l2_property_set                      , {-1}    , 0               },
    {ARAD_PP_DBAL_VT_PROG_VDxOUTERVID_L1L2                                          , arad_pp_dbal_vt_program_vdxoutervid_l1l2_property_set                        , {-1}    , 0               },
    {ARAD_PP_DBAL_VT_PROG_INDEXED_VDxINITIALVID_L1                                  , arad_pp_dbal_vt_program_indexed_vdxinitialvid_property_set                   , {-1}    , MPLS_LABEL_0    },
    {ARAD_PP_DBAL_VT_PROG_INDEXED_VDxOUTERVID_L1                                    , arad_pp_dbal_vt_program_indexed_vdxoutervid_property_set                     , {-1}    , MPLS_LABEL_0    },
    {ARAD_PP_DBAL_VT_PROG_MPLS_PORT_L1                                              , arad_pp_dbal_vt_program_mpls_port_termination_property_set                     , {-1}    , MPLS_LABEL_0    },
    {ARAD_PP_DBAL_VT_PROG_INDEXED_VDxINITIALVID_L3                                  , arad_pp_dbal_vt_program_indexed_vdxinitialvid_property_set                   , {-1}    , MPLS_LABEL_2    },
    {ARAD_PP_DBAL_VT_PROG_INDEXED_VDxOUTERVID_L3                                    , arad_pp_dbal_vt_program_indexed_vdxoutervid_property_set                     , {-1}    , MPLS_LABEL_2    },
    {ARAD_PP_DBAL_VT_PROG_IPV4_VDxINITIALVID                                        , arad_pp_dbal_vt_program_ipv4_vdxinitial_property_set                         , {-1}    , 0               },
    {ARAD_PP_DBAL_VT_PROG_IPV4_VDxOUTERVID                                          , arad_pp_dbal_vt_program_ipv4_vdxouter_property_set                           , {-1}    , 0               },
    {ARAD_PP_DBAL_VT_PROG_IPV4_PORT                                                 , arad_pp_dbal_vt_program_ipv4_port_termination_property_set                   , {-1}    , 0               },
    {ARAD_PP_DBAL_VT_PROG_EVB_UN_C_TAG                                              , arad_pp_dbal_vt_program_evb_property_set                                      , {-1}    , OUTER_VID       },
    {ARAD_PP_DBAL_VT_PROG_EVB_S_TAG                                                 , arad_pp_dbal_vt_program_evb_property_set                                      , {-1}    , OUTER_INNER_VID },
    {ARAD_PP_DBAL_VT_PROG_DOUBLE_TAG_PRIORITY_INITIAL_VID                           , arad_pp_dbal_vt_program_double_tag_priority_property_set                      , {-1}    , 0               },
    {ARAD_PP_DBAL_VT_PROG_DOUBLE_TAG_INITIAL_VID                                    , arad_pp_dbal_vt_program_double_tag_property_set                              , {-1}    , OUTER_INITIAL_VID},
    {ARAD_PP_DBAL_VT_PROG_IPV4_VDxINITIALVID_AFTER_RCY                              , arad_pp_dbal_vt_program_ipv4_vdxinitial_recycle_property_set                 , {-1}    , 0               },
    {ARAD_PP_DBAL_VT_PROG_IPV4_VDxOUTERVID_AFTER_RCY                                , arad_pp_dbal_vt_program_ipv4_vdxouter_recycle_property_set                   , {-1}    , 0               },
    {ARAD_PP_DBAL_VT_PROG_SINGLE_TAG_5_TUPPLE_QINQ                                  , arad_pp_dbal_vt_program_single_tag_5tupple_qinq_property_set                 , {-1}    , 0               },
    {ARAD_PP_DBAL_VT_PROG_DOUBLE_TAG_5_TUPPLE_QINQ                                  , arad_pp_dbal_vt_program_double_tag_5tupple_qinq_property_set                 , {-1}    , 0               },
    {ARAD_PP_DBAL_VT_PROG_UNTAGGED_TST1                                             , arad_pp_dbal_vt_program_tst1_property_set                                    , {-1}    , TST1_VID_UNTAGGED   },
    {ARAD_PP_DBAL_VT_PROG_ONE_TAG_TST1                                              , arad_pp_dbal_vt_program_tst1_property_set                                    , {-1}    , TST1_VID_ONE_TAG    },
    {ARAD_PP_DBAL_VT_PROG_DOUBLE_TAG_TST1                                           , arad_pp_dbal_vt_program_tst1_property_set                                    , {-1}    , TST1_VID_DOUBLE_TAG },


    {ARAD_PP_DBAL_VT_PROG_UNTAGGED_VRRP                                             , arad_pp_dbal_vt_program_vrrp_property_set                                    , {-1}    , TST1_VID_UNTAGGED   },
    {ARAD_PP_DBAL_VT_PROG_ONE_TAG_VRRP                                              , arad_pp_dbal_vt_program_vrrp_property_set                                    , {-1}    , TST1_VID_ONE_TAG    },
    {ARAD_PP_DBAL_VT_PROG_DOUBLE_TAG_VRRP                                           , arad_pp_dbal_vt_program_vrrp_property_set                                    , {-1}    , TST1_VID_DOUBLE_TAG },


    {ARAD_PP_DBAL_VT_PROG_UNTAGGED_EXP_NULL_TST1                                    , arad_pp_dbal_vt_program_exp_null_tst1_property_set                           , {-1}    , TST1_VID_UNTAGGED   },
    {ARAD_PP_DBAL_VT_PROG_ONE_TAG_EXP_NULL_TST1                                     , arad_pp_dbal_vt_program_exp_null_tst1_property_set                           , {-1}    , TST1_VID_ONE_TAG    },
    {ARAD_PP_DBAL_VT_PROG_DOUBLE_TAG_EXP_NULL_TST1                                  , arad_pp_dbal_vt_program_exp_null_tst1_property_set                           , {-1}    , TST1_VID_DOUBLE_TAG },
    {ARAD_PP_DBAL_VT_PROG_OUTER_INNER_PCP_1_TST2                                    , arad_pp_dbal_vt_program_outer_inner_pcp_tst2_property_set                    , {-1}    , TPID1           },
    {ARAD_PP_DBAL_VT_PROG_OUTER_INNER_PCP_2_TST2                                    , arad_pp_dbal_vt_program_outer_inner_pcp_tst2_property_set                    , {-1}    , TPID2           },
    {ARAD_PP_DBAL_VT_PROG_OUTER_PCP_1_TST2                                          , arad_pp_dbal_vt_program_outer_pcp_tst2_property_set                          , {-1}    , TPID1           },
    {ARAD_PP_DBAL_VT_PROG_OUTER_PCP_2_TST2                                          , arad_pp_dbal_vt_program_outer_pcp_tst2_property_set                          , {-1}    , TPID2           },
    {ARAD_PP_DBAL_VT_PROG_OUTER_INNER_1_TST2                                        , arad_pp_dbal_vt_program_outer_inner_tst2_property_set                        , {-1}    , TPID1           },
    {ARAD_PP_DBAL_VT_PROG_OUTER_INNER_2_TST2                                        , arad_pp_dbal_vt_program_outer_inner_tst2_property_set                        , {-1}    , TPID2           },
    {ARAD_PP_DBAL_VT_PROG_OUTER_1_TST2                                              , arad_pp_dbal_vt_program_outer_tst2_property_set                              , {-1}    , TPID1           },
    {ARAD_PP_DBAL_VT_PROG_OUTER_2_TST2                                              , arad_pp_dbal_vt_program_outer_tst2_property_set                              , {-1}    , TPID2           },
    {ARAD_PP_DBAL_VT_PROG_UNTAGGED_TST2                                             , arad_pp_dbal_vt_program_untagged_tst2_property_set                           , {-1}    , 0               },
    {ARAD_PP_DBAL_VT_PROG_VT_TEST2                                                  , arad_pp_dbal_vt_program_test2_property_set                                   , {-1}    , 0               },
    {ARAD_PP_DBAL_VT_PROG_EXTENDER_PE                                               , arad_pp_dbal_vt_program_extender_pe_property_set                             , {-1}    , OUTER_VID       },
    {ARAD_PP_DBAL_VT_PROG_EXTENDER_PE_UT                                            , arad_pp_dbal_vt_program_extender_pe_property_set                             , {-1}    , INITIAL_VID     },
    {ARAD_PP_DBAL_VT_PROG_VT_CUSTOM_PP_PORT_TUNNEL                                  , arad_pp_dbal_vt_program_custom_pp_port_tunnel_property_set                   , {-1}    , 0               },
    {ARAD_PP_DBAL_VT_PROG_VD_INITIALVID_VLAN_DOMAIN_MPLS_L1                         , arad_pp_dbal_vt_program_vlan_domain_mpls_property_set                        , {-1}    , 0               },
    {ARAD_PP_DBAL_VT_PROG_PON_UNTAGGED                                              , arad_pp_dbal_vt_program_pon_untagged_property_set                            , {-1}    , 0               },
    {ARAD_PP_DBAL_VT_PROG_PON_ONE_TAG                                               , arad_pp_dbal_vt_program_pon_one_tag_property_set                             , {-1}    , 0               },
    {ARAD_PP_DBAL_VT_PROG_PON_TWO_TAGS                                              , arad_pp_dbal_vt_program_pon_two_tags_property_set                            , {-1}    , 0               },
    {ARAD_PP_DBAL_VT_PROG_PON_TWO_TAGS_VS_TUNNEL_ID                                 , arad_pp_dbal_vt_program_pon_two_tags_vs_tunnel_id_property_set               , {-1}    , 0               },
    {ARAD_PP_DBAL_VT_PROG_SECTION_OAM_ONE_TAG                                       , arad_pp_dbal_vt_program_section_oam_one_tag_property_set                     , {-1}    , 0               },
    {ARAD_PP_DBAL_VT_PROG_SECTION_OAM_TWO_TAGS                                      , arad_pp_dbal_vt_program_section_oam_two_tags_property_set                    , {-1}    , 0               },
    {ARAD_PP_DBAL_VT_PROG_INDEXED_VDxINITIALVID_MPLS4_L1L2                          , arad_pp_dbal_vt_program_indexed_vdxinitialvid_mpls4_l1l2_property_set        , {-1}    , 0               },
    {ARAD_PP_DBAL_VT_PROG_INDEXED_VDxOUTERVID_MPLS4_L1L2                            , arad_pp_dbal_vt_program_indexed_vdxoutervid_mpls4_l1l2_property_set          , {-1}    , 0               },
    {ARAD_PP_DBAL_VT_PROG_IPV6_VDxINITIALVID                                        , arad_pp_dbal_vt_program_ipv6_vdxinitial_property_set                         , {-1}    , 0               },
    {ARAD_PP_DBAL_VT_PROG_IPV6_VDxOUTERVID                                          , arad_pp_dbal_vt_program_ipv6_vdxouter_property_set                           , {-1}    , 0               },
	   {ARAD_PP_DBAL_VT_PROG_IPV6_VDxINITIALVID_AFTER_RECYCLE                          , arad_pp_dbal_vt_program_ipv6_vdxinitial_property_set                         , {-1}    , 1               },
	   {ARAD_PP_DBAL_VT_PROG_IPV6_VDxOUTERVID_AFTER_RECYCLE                            , arad_pp_dbal_vt_program_ipv6_vdxouter_property_set                           , {-1}    , 1               },
    {ARAD_PP_DBAL_VT_PROG_GENERALIZED_RCH0                                          , arad_pp_dbal_vt_program_generalized_rch_property_set                         , {-1}    , 1               },
    {ARAD_PP_DBAL_VT_PROG_GENERALIZED_RCH1                                          , arad_pp_dbal_vt_program_generalized_rch_property_set                         , {-1}    , 0               },
	  {ARAD_PP_DBAL_VT_PROG_3MTSE_PLUS_FRR_VDxINITIALVID_L2_L1                        , arad_pp_dbal_vt_program_3mtse_plus_frr_vdxinitialvid_l2l1_property_set       , {-1}    , 0               },
	  {ARAD_PP_DBAL_VT_PROG_3MTSE_PLUS_FRR_VDxOUTERVID_L2_L1                          , arad_pp_dbal_vt_program_3mtse_plus_frr_vdxOuterVid_l2l1_property_set         , {-1}    , 0               },
	  {ARAD_PP_DBAL_VT_PROG_3MTSE_PLUS_FRR_VDxINITIALVID_L2_L4B_L5GAL                 , arad_pp_dbal_vt_program_3mtse_plus_frr_vdxinitialvid_l2_l4b_l5g_property_set , {-1}    , 0               },
	  {ARAD_PP_DBAL_VT_PROG_3MTSE_PLUS_FRR_VDxOUTERVID_L2_L4B_L5GAL                   , arad_pp_dbal_vt_program_3mtse_plus_frr_vdxOuterVid_l2_l4b_l5g_property_set   , {-1}    , 0               },
	  {ARAD_PP_DBAL_VT_PROG_2MTSE_PLUS_FRR_VDxINITIALVID_FRR                          , arad_pp_dbal_vt_program_2mtse_plus_frr_vid_frr_property_set                  , {-1}    , INITIAL_VID     },
	  {ARAD_PP_DBAL_VT_PROG_2MTSE_PLUS_FRR_VDxOUTERVID_FRR                            , arad_pp_dbal_vt_program_2mtse_plus_frr_vid_frr_property_set                  , {-1}    , OUTER_VID       },
	  {ARAD_PP_DBAL_VT_PROG_2MTSE_PLUS_FRR_VDxOUTER_INNER_VID_FRR                     , arad_pp_dbal_vt_program_2mtse_plus_frr_vid_frr_property_set                  , {-1}    , OUTER_INNER_VID },
	  {ARAD_PP_DBAL_VT_PROG_2MTSE_PLUS_FRR_VDxOUTER_INNER_PCP_VID_FRR                 , arad_pp_dbal_vt_program_2mtse_plus_frr_vid_frr_property_set                  , {-1}    , OUTER_INNER_VID_PCP }

};



void arad_pp_dbal_tt_program_tm_property_set                            (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused          );
void arad_pp_dbal_tt_program_oam_property_set                           (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused          );
void arad_pp_dbal_tt_program_bfd_property_set                           (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused          );
void arad_pp_dbal_tt_program_unindexed_mpls_property_set                (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 mpls_label      );
void arad_pp_dbal_tt_program_mldp_overlay_after_rcy_property_set        (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused          );
void arad_pp_dbal_tt_program_indexed_mpls_l1_l1_property_set            (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 is_gal          );
void arad_pp_dbal_tt_program_indexed_mpls_l1_l2_property_set            (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 is_gal          );
void arad_pp_dbal_tt_program_indexed_mpls_l2_l2_property_set            (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused          );
void arad_pp_dbal_tt_program_indexed_mpls_l2_l3_gal_property_set        (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused          );
void arad_pp_dbal_tt_program_unindexed_mpls_l2_l3_eli_property_set      (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused          );
void arad_pp_dbal_tt_program_indexed_mpls_l2_l3_eli_property_set        (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused          );
void arad_pp_dbal_tt_program_l1_l2_inrif_property_set                   (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused          );
void arad_pp_dbal_tt_program_l1_l2eli_inrif_property_set                (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused          );
void arad_pp_dbal_tt_program_l1_l2_l3eli_inrif_property_set             (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused          );
void arad_pp_dbal_tt_program_indexed_mpls_l3_l2_property_set            (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused          );
void arad_pp_dbal_tt_program_mpls_port_termination_property_set         (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused          ); 
void arad_pp_dbal_tt_program_indexed_mpls_l3_l4_property_set            (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused          );
void arad_pp_dbal_tt_program_arp_property_set                           (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused          );
void arad_pp_dbal_tt_program_bridge_property_set                        (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused          );
void arad_pp_dbal_tt_program_uknown_l3_property_set                     (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused          );
void arad_pp_dbal_tt_program_trill_property_set                         (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused          );
void arad_pp_dbal_tt_program_trill_trap_property_set                    (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused          );
void arad_pp_dbal_tt_program_fc_with_vft_property_set                   (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 vsan_source     );
void arad_pp_dbal_tt_program_fc_property_set                            (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 vsan_source     );
void arad_pp_dbal_tt_program_mac_in_mac_property_set                    (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 mac_in_mac_type );
void arad_pp_dbal_tt_program_ipv4_router_property_set                   (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused          );
void arad_pp_dbal_tt_program_ipv4_router_pwe_gre_dip_found_property_set (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused          );
void arad_pp_dbal_tt_program_ipv4_router_dip_found_property_set         (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 is_recycle      );
void arad_pp_dbal_tt_program_ipv4_router_dip_found_port_property_set    (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 tunnel_type     );
void arad_pp_dbal_tt_program_ipv4_router_uc_dip_not_found_property_set  (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused          );
void arad_pp_dbal_tt_program_ipv4_router_mc_dip_not_found_property_set  (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused          );
void arad_pp_dbal_tt_program_ipv4_router_compatible_mc_property_set     (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused          );
void arad_pp_dbal_tt_program_ipv6_router_property_set                   (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 is_recycle      );
void arad_pp_dbal_tt_program_dipv6_compressed_property_set              (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused          );
void arad_pp_dbal_tt_program_extender_untag_check_set                   (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 extender_type   );
void arad_pp_dbal_tt_program_ipv4_router_dip_sip_vrf_property_set       (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused          );
void arad_pp_dbal_tt_program_ipv4_router_gre_port_set                   (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused          );
void arad_pp_dbal_tt_program_ipv4_for_explicit_mpls_property_set        (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused          );
void arad_pp_dbal_tt_program_tunnel_two_explicit_mpls_property_set      (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused          );
void arad_pp_dbal_tt_program_tunnel_three_explicit_mpls_property_set    (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused          );
void arad_pp_dbal_tt_program_extender_cb_set                            (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 extender_type   );
void arad_pp_dbal_tt_program_indexed_mpls_l1_gal_only_property_set      (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 is_gal);
void arad_pp_dbal_tt_program_indexed_mpls_l2_gal_only_property_set      (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused);
void arad_pp_dbal_tt_program_vlan_domain_mpls_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused);
void arad_pp_dbal_tt_program_index_as_order_l2_l3_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused);
void arad_pp_dbal_tt_program_index_as_order_l2_l2_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused);
void arad_pp_dbal_tt_program_index_as_order_l1_l2_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused);
void arad_pp_dbal_tt_program_index_as_order_l1_l1_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused);
void arad_pp_dbal_tt_program_index_as_order_nop_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused);
void arad_pp_dbal_tt_program_mpls4_nop_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused);
void arad_pp_dbal_tt_program_ip_disable_on_lif_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused);
void arad_pp_dbal_tt_program_ipv6_router_comp_dest_found_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused);
void arad_pp_dbal_tt_program_pppoe_property_set               (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused);
void arad_pp_dbal_tt_program_3mtse_plus_frr_property_set                (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 vtt_3mtse_frr_type);
void arad_pp_dbal_tt_program_2mtse_plus_frr_property_set                (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 vtt_2mtse_frr_type);
void arad_pp_dbal_tt_program_generalized_rch_property_mpls_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused);
void arad_pp_dbal_tt_program_mymac_ip_disable_trap_property_set (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused);
void arad_pp_dbal_tt_program_l2tp_property_set (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused);


ARAD_PP_DBAL_VTT_PROGRAM_INFO tt_dbal_programs[ARAD_PP_DBAL_TT_NOF_PROGRAMS] = {

    {ARAD_PP_DBAL_TT_PROG_TM                            , arad_pp_dbal_tt_program_tm_property_set                             , {-1}    , 0                    },
    {ARAD_PP_DBAL_TT_PROG_OAM_STAT                      , arad_pp_dbal_tt_program_oam_property_set                            , {-1}    , 0                    },
    {ARAD_PP_DBAL_TT_PROG_BFD_STAT                      , arad_pp_dbal_tt_program_bfd_property_set                            , {-1}    , 0                    },
    {ARAD_PP_DBAL_TT_PROG_MLDP_OVERLAY_AFTER_RCY        , arad_pp_dbal_tt_program_mldp_overlay_after_rcy_property_set         , {-1}    , 0                    },
    {ARAD_PP_DBAL_TT_PROG_MPLS_L2                       , arad_pp_dbal_tt_program_unindexed_mpls_property_set                 , {-1}    , MPLS_LABEL_1         },
    {ARAD_PP_DBAL_TT_PROG_MPLS_L3                       , arad_pp_dbal_tt_program_unindexed_mpls_property_set                 , {-1}    , MPLS_LABEL_2         },
    {ARAD_PP_DBAL_TT_PROG_MPLS_L2_L3_ELI                , arad_pp_dbal_tt_program_unindexed_mpls_l2_l3_eli_property_set       , {-1}    , 0                    },
    {ARAD_PP_DBAL_TT_PROG_INDEXED_L1_L1                 , arad_pp_dbal_tt_program_indexed_mpls_l1_l1_property_set             , {-1}    , 0                    },
    {ARAD_PP_DBAL_TT_PROG_INDEXED_L1_L1_GAL             , arad_pp_dbal_tt_program_indexed_mpls_l1_l1_property_set             , {-1}    , 1                    },
    {ARAD_PP_DBAL_TT_PROG_INDEXED_L1_L2                 , arad_pp_dbal_tt_program_indexed_mpls_l1_l2_property_set             , {-1}    , 0                    },
    {ARAD_PP_DBAL_TT_PROG_INDEXED_L1_L2_GAL             , arad_pp_dbal_tt_program_indexed_mpls_l1_l2_property_set             , {-1}    , 1                    },
    {ARAD_PP_DBAL_TT_PROG_INDEXED_L2_L2                 , arad_pp_dbal_tt_program_indexed_mpls_l2_l2_property_set             , {-1}    , 0                    },
    {ARAD_PP_DBAL_TT_PROG_INDEXED_L2_L3_GAL             , arad_pp_dbal_tt_program_indexed_mpls_l2_l3_gal_property_set         , {-1}    , 0                    },
    {ARAD_PP_DBAL_TT_PROG_INDEXED_L2_L3_ELI             , arad_pp_dbal_tt_program_indexed_mpls_l2_l3_eli_property_set         , {-1}    , 0                    },
    {ARAD_PP_DBAL_TT_PROG_INDEXED_L1_L2_INRIF           , arad_pp_dbal_tt_program_l1_l2_inrif_property_set                    , {-1}    , 0                    },
    {ARAD_PP_DBAL_TT_PROG_INDEXED_L1_L2ELI_INRIF        , arad_pp_dbal_tt_program_l1_l2eli_inrif_property_set                 , {-1}    , 0                    },
    {ARAD_PP_DBAL_TT_PROG_INDEXED_L1_L2_L3ELI_INRIF     , arad_pp_dbal_tt_program_l1_l2_l3eli_inrif_property_set              , {-1}    , 0                    },
    {ARAD_PP_DBAL_TT_PROG_INDEXED_L3_L2                 , arad_pp_dbal_tt_program_indexed_mpls_l3_l2_property_set             , {-1}    , 0                    },
    {ARAD_PP_DBAL_TT_PROG_INDEXED_MPLS_PORT_L3_L2       , arad_pp_dbal_tt_program_mpls_port_termination_property_set          , {-1}    , 0                    }, 
    {ARAD_PP_DBAL_TT_PROG_INDEXED_L3_L4                 , arad_pp_dbal_tt_program_indexed_mpls_l3_l4_property_set             , {-1}    , 0                    },
    {ARAD_PP_DBAL_TT_PROG_ARP                           , arad_pp_dbal_tt_program_arp_property_set                            , {-1}    , 0                    },
    {ARAD_PP_DBAL_TT_PROG_BRIDGE_STAR                   , arad_pp_dbal_tt_program_bridge_property_set                         , {-1}    , 0                    },
    {ARAD_PP_DBAL_TT_PROG_TT_UNKNOWN_L3                 , arad_pp_dbal_tt_program_uknown_l3_property_set                      , {-1}    , 0                    },
    {ARAD_PP_DBAL_TT_PROG_TRILL_ONE_TAG                 , arad_pp_dbal_tt_program_trill_property_set                          , {-1}    , TRILL_ONE_TAG        },
    {ARAD_PP_DBAL_TT_PROG_TRILL_TWO_TAGS                , arad_pp_dbal_tt_program_trill_property_set                          , {-1}    , TRILL_TWO_TAG        },
    {ARAD_PP_DBAL_TT_PROG_TRILL_TRAP                    , arad_pp_dbal_tt_program_trill_trap_property_set                     , {-1}    , 0                    },
    {ARAD_PP_DBAL_TT_PROG_FC_WITH_VFT                   , arad_pp_dbal_tt_program_fc_with_vft_property_set                    , {-1}    , VSAN_INIT            },
    {ARAD_PP_DBAL_TT_PROG_FC                            , arad_pp_dbal_tt_program_fc_property_set                             , {-1}    , VSAN_INIT            },
    {ARAD_PP_DBAL_TT_PROG_MIM_MC                        , arad_pp_dbal_tt_program_mac_in_mac_property_set                     , {-1}    , MAC_IN_MAC_MC        },
    {ARAD_PP_DBAL_TT_PROG_MIM_WITH_BTAG                 , arad_pp_dbal_tt_program_mac_in_mac_property_set                     , {-1}    , MAC_IN_MAC_WITH_BTAG },
    {ARAD_PP_DBAL_TT_PROG_IPV4_ROUTER                   , arad_pp_dbal_tt_program_ipv4_router_property_set                    , {-1}    , 0                    },
    {ARAD_PP_DBAL_TT_PROG_IPV4_ROUTER_PWE_GRE_DIP_FOUND , arad_pp_dbal_tt_program_ipv4_router_pwe_gre_dip_found_property_set  , {-1}    , 0                    },
    {ARAD_PP_DBAL_TT_PROG_IPV4_ROUTER_DIP_FOUND         , arad_pp_dbal_tt_program_ipv4_router_dip_found_property_set          , {-1}    , 0                    },
    {ARAD_PP_DBAL_TT_PROG_IPV4_ROUTER_L2_GRE_PORT       , arad_pp_dbal_tt_program_ipv4_router_dip_found_port_property_set     , {-1}    , L2_GRE               },
    {ARAD_PP_DBAL_TT_PROG_IPV4_ROUTER_VXLAN_PORT        , arad_pp_dbal_tt_program_ipv4_router_dip_found_port_property_set     , {-1}    , VXLAN                },
    {ARAD_PP_DBAL_TT_PROG_IPV4_ROUTER_UC_DIP_NOT_FOUND  , arad_pp_dbal_tt_program_ipv4_router_uc_dip_not_found_property_set   , {-1}    , 0                    },
    {ARAD_PP_DBAL_TT_PROG_IPV4_ROUTER_MC_DIP_NOT_FOUND  , arad_pp_dbal_tt_program_ipv4_router_mc_dip_not_found_property_set   , {-1}    , 0                    },
    {ARAD_PP_DBAL_TT_PROG_IPV4_ROUTER_COMPATIBLE_MC     , arad_pp_dbal_tt_program_ipv4_router_compatible_mc_property_set      , {-1}    , 0                    },
    {ARAD_PP_DBAL_TT_PROG_IPV6_ROUTER                   , arad_pp_dbal_tt_program_ipv6_router_property_set                    , {-1}    , 0                    },
    {ARAD_PP_DBAL_TT_PROG_DIPV6_COMPRESSED              , arad_pp_dbal_tt_program_dipv6_compressed_property_set               , {-1}    , 0                    },
    {ARAD_PP_DBAL_TT_PROG_EXTENDER_UNTAG_CHECK_IP_MC    , arad_pp_dbal_tt_program_extender_untag_check_set                    , {-1}    , EXTENDER_IP_MC       },
    {ARAD_PP_DBAL_TT_PROG_EXTENDER_UNTAG_CHECK_IP_UC    , arad_pp_dbal_tt_program_extender_untag_check_set                    , {-1}    , EXTENDER_IP_UC       },
    {ARAD_PP_DBAL_TT_PROG_EXTENDER_UNTAG_CHECK          , arad_pp_dbal_tt_program_extender_untag_check_set                    , {-1}    , EXTENDER_ETH         },
    {ARAD_PP_DBAL_TT_PROG_IPV4_ROUTER_DIP_SIP_VRF       , arad_pp_dbal_tt_program_ipv4_router_dip_sip_vrf_property_set        , {-1}    , 0                    },
    {ARAD_PP_DBAL_TT_PROG_IPV4_ROUTER_L3_GRE_PORT       , arad_pp_dbal_tt_program_ipv4_router_gre_port_set                    , {-1}    , 0                    },
    {ARAD_PP_DBAL_TT_PROG_IPV4_FOR_EXPLICIT_NULL        , arad_pp_dbal_tt_program_ipv4_for_explicit_mpls_property_set         , {-1}    , 0                    },
    {ARAD_PP_DBAL_TT_PROG_TUNNEL_TWO_EXPLICIT_NULL      , arad_pp_dbal_tt_program_tunnel_two_explicit_mpls_property_set       , {-1}    , 0                    },
    {ARAD_PP_DBAL_TT_PROG_EXTENDER_CB_IP_MC             , arad_pp_dbal_tt_program_extender_cb_set                             , {-1}    , EXTENDER_IP_MC       },
    {ARAD_PP_DBAL_TT_PROG_EXTENDER_CB_IP_UC             , arad_pp_dbal_tt_program_extender_cb_set                             , {-1}    , EXTENDER_IP_UC       },
    {ARAD_PP_DBAL_TT_PROG_EXTENDER_CB                   , arad_pp_dbal_tt_program_extender_cb_set                             , {-1}    , EXTENDER_ETH         },
    {ARAD_PP_DBAL_TT_PROG_INDEXED_L1_GAL_ONLY           , arad_pp_dbal_tt_program_indexed_mpls_l1_gal_only_property_set       , {-1}    , 0                    },
    {ARAD_PP_DBAL_TT_PROG_INDEXED_L2_GAL_ONLY           , arad_pp_dbal_tt_program_indexed_mpls_l2_gal_only_property_set       , {-1}    , 0                    },
    {ARAD_PP_DBAL_TT_PROG_VLAN_DOMAIN_MPLS_L2           , arad_pp_dbal_tt_program_vlan_domain_mpls_property_set               , {-1}    , 0                    },
    {ARAD_PP_DBAL_TT_PROG_INDEX_AS_ORDER_L2_L3          , arad_pp_dbal_tt_program_index_as_order_l2_l3_property_set           , {-1}    , 0                    },
    {ARAD_PP_DBAL_TT_PROG_INDEX_AS_ORDER_L2_L2          , arad_pp_dbal_tt_program_index_as_order_l2_l2_property_set           , {-1}    , 0                    },
    {ARAD_PP_DBAL_TT_PROG_INDEX_AS_ORDER_L1_L2          , arad_pp_dbal_tt_program_index_as_order_l1_l2_property_set           , {-1}    , 0                    },
    {ARAD_PP_DBAL_TT_PROG_INDEX_AS_ORDER_L1_L1          , arad_pp_dbal_tt_program_index_as_order_l1_l1_property_set           , {-1}    , 0                    },
    {ARAD_PP_DBAL_TT_PROG_INDEX_AS_ORDER_NOP            , arad_pp_dbal_tt_program_index_as_order_nop_property_set             , {-1}    , 0                    },
    {ARAD_PP_DBAL_TT_PROG_INDEX_MPLS4_L3_L4             , arad_pp_dbal_tt_program_indexed_mpls_l3_l2_property_set             , {-1}    , 1                    },
    {ARAD_PP_DBAL_TT_PROG_INDEX_MPLS4_NOP               , arad_pp_dbal_tt_program_mpls4_nop_property_set                      , {-1}    , 1                    },
    {ARAD_PP_DBAL_TT_PROG_IP_DISABLE_ON_LIF             , arad_pp_dbal_tt_program_ip_disable_on_lif_property_set              , {-1}    , 0                    },
    {ARAD_PP_DBAL_TT_PROG_IPV4_DISABLE_ON_LIF           , arad_pp_dbal_tt_program_ip_disable_on_lif_property_set              , {-1}    , 0                    },
    {ARAD_PP_DBAL_TT_PROG_IPV6_DISABLE_ON_LIF           , arad_pp_dbal_tt_program_ip_disable_on_lif_property_set              , {-1}    , 0                    },
    {ARAD_PP_DBAL_TT_PROG_IPV6_ROUTER_COMP_DEST_FOUND   , arad_pp_dbal_tt_program_ipv6_router_comp_dest_found_property_set    , {-1}    , 0                    },
    {ARAD_PP_DBAL_TT_PROG_IPV6_ROUTER_AFTER_RECYCLE     , arad_pp_dbal_tt_program_ipv6_router_property_set                    , {-1}    , 1                    },
    {ARAD_PP_DBAL_TT_PROG_PPPOE                         , arad_pp_dbal_tt_program_pppoe_property_set                          , {-1}    , 0                    },
	  {ARAD_PP_DBAL_TT_PROG_L2TP						  , arad_pp_dbal_tt_program_l2tp_property_set							, {-1}	  , 1					 },
    {ARAD_PP_DBAL_TT_PROG_GENERALIZED_RCH_MPLS          , arad_pp_dbal_tt_program_generalized_rch_property_mpls_set           , {-1}    , 0                    },
    {ARAD_PP_DBAL_TT_PROG_MYMAC_IP_DISABLED_TRAP        , arad_pp_dbal_tt_program_mymac_ip_disable_trap_property_set          , {-1}    , 0                    },
    {ARAD_PP_DBAL_TT_PROG_3MTSE_PLUS_FRR_L1_L1          , arad_pp_dbal_tt_program_3mtse_plus_frr_property_set                 , {-1}    , VTT_3MTSE_FRR_L1_L1      },
    {ARAD_PP_DBAL_TT_PROG_3MTSE_PLUS_FRR_TFL_L3_L3      , arad_pp_dbal_tt_program_3mtse_plus_frr_property_set                 , {-1}    , VTT_3MTSE_FRR_L3_L3      },
    {ARAD_PP_DBAL_TT_PROG_3MTSE_PLUS_FRR_L3_L1          , arad_pp_dbal_tt_program_3mtse_plus_frr_property_set                 , {-1}    , VTT_3MTSE_FRR_L3_L1      },
    {ARAD_PP_DBAL_TT_PROG_3MTSE_PLUS_FRR_TFL_L4_L3      , arad_pp_dbal_tt_program_3mtse_plus_frr_property_set                 , {-1}    , VTT_3MTSE_FRR_L4_L3      },
    {ARAD_PP_DBAL_TT_PROG_3MTSE_PLUS_FRR_L1             , arad_pp_dbal_tt_program_3mtse_plus_frr_property_set                 , {-1}    , VTT_3MTSE_FRR_L1         },
    {ARAD_PP_DBAL_TT_PROG_3MTSE_PLUS_FRR_L2_L1          , arad_pp_dbal_tt_program_3mtse_plus_frr_property_set                 , {-1}    , VTT_3MTSE_FRR_L2_L1      },
    {ARAD_PP_DBAL_TT_PROG_2MTSE_PLUS_FRR_L1_L1          , arad_pp_dbal_tt_program_2mtse_plus_frr_property_set                 , {-1}    , VTT_2MTSE_FRR_L1_L1      },
    {ARAD_PP_DBAL_TT_PROG_2MTSE_PLUS_FRR_L2_L2          , arad_pp_dbal_tt_program_2mtse_plus_frr_property_set                 , {-1}    , VTT_2MTSE_FRR_L2_L2      },
    {ARAD_PP_DBAL_TT_PROG_2MTSE_PLUS_FRR_L2_L1          , arad_pp_dbal_tt_program_2mtse_plus_frr_property_set                 , {-1}    , VTT_2MTSE_FRR_L2_L1      },
    {ARAD_PP_DBAL_TT_PROG_2MTSE_PLUS_FRR_L3_L2          , arad_pp_dbal_tt_program_2mtse_plus_frr_property_set                 , {-1}    , VTT_2MTSE_FRR_L3_L2      },
    {ARAD_PP_DBAL_TT_PROG_IPV4_ROUTER_DIP_FOUND_IPMC_RCY, arad_pp_dbal_tt_program_ipv4_router_dip_found_property_set          , {-1}    , 1                    }

};






int32 arad_pp_dbal_vt_cam_tm_set                             (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused       );
int32 arad_pp_dbal_vt_cam_vdxinitialvid_set                  (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 port_profile );
int32 arad_pp_dbal_vt_cam_vdxinitialvid_l1_set               (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 port_profile );
int32 arad_pp_dbal_vt_cam_vdxoutervid_set                    (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 port_profile );
int32 arad_pp_dbal_vt_cam_vdxoutervid_l1_set                 (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 port_profile );
int32 arad_pp_dbal_vt_cam_vdxouter_inner_vid_set             (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused       );
int32 arad_pp_dbal_vt_cam_mpls_port_termination_set          (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused       );
int32 arad_pp_dbal_vt_cam_vdxouter_inner_vid_or_outer_vid_set(int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused       );
int32 arad_pp_dbal_vt_cam_initialvid_l1frr_set               (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused       );
int32 arad_pp_dbal_vt_cam_outervid_l1frr_set                 (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused       );
int32 arad_pp_dbal_vt_cam_vdxinitialvid_l1l2_set             (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused       );
int32 arad_pp_dbal_vt_cam_vdxoutervid_l1l2_set               (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused       );
int32 arad_pp_dbal_vt_cam_vdxinitialvid_trill_set            (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused       );
int32 arad_pp_dbal_vt_cam_vdxoutervid_trill_set              (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused       );
int32 arad_pp_dbal_vt_cam_vdxouter_inner_vid_trill_set       (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused       );
int32 arad_pp_dbal_vt_cam_vd_designated_vid_trill_set        (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused       );
int32 arad_pp_dbal_vt_cam_vdxiniitialvid_l1_l2eli_set        (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused       );
int32 arad_pp_dbal_vt_cam_vdxoutervid_l1_l2eli_set           (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused       );

int32 arad_pp_dbal_vt_cam_ipv4_vdxinitialvid_set             (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused       );
int32 arad_pp_dbal_vt_cam_ipv4_vdxoutervid_set               (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused       );
int32 arad_pp_dbal_vt_cam_ipv4_port_termination_set          (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused       );
int32 arad_pp_dbal_vt_cam_evb_set                            (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 vid_type     );
int32 arad_pp_dbal_vt_cam_double_tag_pri_set                 (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused       );
int32 arad_pp_dbal_vt_cam_double_tag_set                     (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused       );
int32 arad_pp_dbal_vt_cam_ipv4_vdxinitialvid_recycle_set     (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused       );
int32 arad_pp_dbal_vt_cam_ipv4_vdxoutervid_recycle_set       (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused       );
int32 arad_pp_dbal_vt_cam_5_tuple_qinq_set                   (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 vid_type     );
int32 arad_pp_dbal_vt_cam_untagged_tst1_set                  (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused       );
int32 arad_pp_dbal_vt_cam_one_tag_tst1_set                   (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused       );
int32 arad_pp_dbal_vt_cam_double_tag_tst1_set                (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused       );

int32 arad_pp_dbal_vt_cam_untagged_vrrp_set                  (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused       );
int32 arad_pp_dbal_vt_cam_one_tag_vrrp_set                   (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused       );
int32 arad_pp_dbal_vt_cam_double_tag_vrrp_set                (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused       );

int32 arad_pp_dbal_vt_cam_untagged_exp_null_tst1_set         (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused       );
int32 arad_pp_dbal_vt_cam_one_tag_exp_null_tst1_set          (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused       );
int32 arad_pp_dbal_vt_cam_double_tag_exp_null_tst1_set       (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused       );
int32 arad_pp_dbal_vt_cam_outer_inner_pcp_tst2_set           (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 tpid         );
int32 arad_pp_dbal_vt_cam_outer_pcp_tst2_set                 (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 tpid         );
int32 arad_pp_dbal_vt_cam_outer_inner_tst2_set               (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 tpid         );
int32 arad_pp_dbal_vt_cam_outer_tst2_set                     (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 tpid         );
int32 arad_pp_dbal_vt_cam_untagged_tst2_set                  (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused       );
int32 arad_pp_dbal_vt_cam_test2_set                          (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused       );
int32 arad_pp_dbal_vt_cam_extender_pe_set                    (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 vid_type     );
int32 arad_pp_dbal_vt_cam_custom_pp_port_tunnel_set          (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused       );
int32 arad_pp_dbal_vt_cam_vlan_domain_mpls_l1_set                 (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused       );
int32 arad_pp_dbal_vt_cam_pon_all_progs_set                  (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 prog_sel_pon_type);
int32 arad_pp_dbal_vt_cam_section_oam_all_progs_set          (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 prog_sel_pon_type);
int32 arad_pp_dbal_vt_cam_vdxinitialvid_mpls4_set(int unit, ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 port_profile);
int32 arad_pp_dbal_vt_cam_vdxoutervid_mpls4_set(int unit,ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int line, uint32 unused);
int32 arad_pp_dbal_vt_cam_ipv6_vdxinitialvid_set             (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 is_recycle);
int32 arad_pp_dbal_vt_cam_ipv6_vdxoutervid_set               (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 is_recycle);
int32 arad_pp_dbal_vt_cam_3mtse_plus_frr_vdxinitialvid_set   (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 prog_sel_3mtse_plus_frr_type);
int32 arad_pp_dbal_vt_cam_3mtse_plus_frr_vdxoutervid_set     (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 prog_sel_3mtse_plus_frr_type);
int32 arad_pp_dbal_vt_cam_2mtse_plus_frr_vdxouter_inner_vid_l1_set(int unit, ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int line, uint32 unused);
int32 arad_pp_dbal_vt_cam_2mtse_plus_frr_vdxouter_inner_vid_pcp_l1_set(int unit, ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int line, uint32 unused);
int32 arad_pp_dbal_vt_cam_generalized_rch_set               (int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 is_rch0);



ARAD_PP_DBAL_VT_PROGRAM_SELECTION_INFO vt_dbal_program_selection[ARAD_PP_DBAL_VT_NOF_PROGRAM_SELECTION] = {

        {ARAD_PP_DBAL_VT_PROG_SELECTION_TM                                                         , arad_pp_dbal_vt_cam_tm_set                             , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 0                                    },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_TM_STAT                                                    , arad_pp_dbal_vt_cam_tm_set                             , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 0                                    },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_VD_INITIALVID                                              , arad_pp_dbal_vt_cam_vdxinitialvid_set                  , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, SOC_PPC_NOF_PORT_DEFINED_VT_PROFILES },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_VD_INITIALVID_L1                                           , arad_pp_dbal_vt_cam_vdxinitialvid_l1_set            , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, SOC_PPC_NOF_PORT_DEFINED_VT_PROFILES },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_VD_OUTERVID                                                , arad_pp_dbal_vt_cam_vdxoutervid_set                    , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, SOC_PPC_NOF_PORT_DEFINED_VT_PROFILES },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_VD_OUTERVID_L1                                             , arad_pp_dbal_vt_cam_vdxoutervid_l1_set              , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, SOC_PPC_NOF_PORT_DEFINED_VT_PROFILES },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_VD_OUTER_INNER_VID                                         , arad_pp_dbal_vt_cam_vdxouter_inner_vid_set             , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 0                                    },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_VD_OUTER_INNER_VID_OR_OUTER_VID                            , arad_pp_dbal_vt_cam_vdxouter_inner_vid_or_outer_vid_set, {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 0                                    },

        {ARAD_PP_DBAL_VT_PROG_SELECTION_VD_INITIALVID_L1FRR                                        , arad_pp_dbal_vt_cam_initialvid_l1frr_set               , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 0                                    },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_VD_OUTERVID_L1FRR                                          , arad_pp_dbal_vt_cam_outervid_l1frr_set                 , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 0                                    },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_VD_INITIALVID_L1L2                                         , arad_pp_dbal_vt_cam_vdxinitialvid_l1l2_set             , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 0                                    },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_VD_OUTERVID_L1L2                                           , arad_pp_dbal_vt_cam_vdxoutervid_l1l2_set               , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 0                                    },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_VD_OUTER_OR_VD_OUTER_PCP                                   , arad_pp_dbal_vt_cam_vdxoutervid_set                    , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 0                                    },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_VD_OUTER_INNER_OR_VD_OUTER_INNER_OUTERPCP                  , arad_pp_dbal_vt_cam_vdxouter_inner_vid_set             , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 0                                    },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_HIGH_VD_OUTER_INNER_OUTERPCP_OR_VD_OUTER_INNER_OR_VD_OUTER , arad_pp_dbal_vt_cam_vdxouter_inner_vid_or_outer_vid_set, {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 0                                    },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_VD_INITIALVID_OR_VD_INITIALVID                             , arad_pp_dbal_vt_cam_vdxinitialvid_set                  , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, SOC_PPC_NOF_PORT_DEFINED_VT_PROFILES },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_VD_OUTER_OR_VD_OUTER                                       , arad_pp_dbal_vt_cam_vdxoutervid_set                    , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 0                                    },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_VD_OUTER_INNER_OR_VD_OUTER_INNER                           , arad_pp_dbal_vt_cam_vdxouter_inner_vid_set             , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 0                                    },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_INDX_VD_INITIALVID_FRR_L1                                  , arad_pp_dbal_vt_cam_initialvid_l1frr_set               , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 0                                    },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_INDX_VD_OUTERVID_FRR_L1                                    , arad_pp_dbal_vt_cam_outervid_l1frr_set                 , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 0                                    },

        {ARAD_PP_DBAL_VT_PROG_SELECTION_VD_INITIALVID_TRILL                                        , arad_pp_dbal_vt_cam_vdxinitialvid_trill_set            , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 0                                    },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_VD_OUTERVID_TRILL                                          , arad_pp_dbal_vt_cam_vdxoutervid_trill_set              , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 0                                    },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_VD_OUTER_INNER_VID_TRILL                                   , arad_pp_dbal_vt_cam_vdxouter_inner_vid_trill_set       , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 0                                    },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_VD_DESIGNATED_VID_TRILL                                    , arad_pp_dbal_vt_cam_vd_designated_vid_trill_set        , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 0                                    },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_VD_INITIALVID_L1_L2ELI                                     , arad_pp_dbal_vt_cam_vdxiniitialvid_l1_l2eli_set        , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 0                                    },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_VD_OUTERVID_L1_L2ELI                                       , arad_pp_dbal_vt_cam_vdxoutervid_l1_l2eli_set           , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 0                                    },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_VD_INDEXED_VDxINITIALVID_L1                                , arad_pp_dbal_vt_cam_vdxinitialvid_l1_set                  , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, SOC_PPC_NOF_PORT_DEFINED_VT_PROFILES },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_VD_INDEXED_VDxOUTERVID_L1                                  , arad_pp_dbal_vt_cam_vdxoutervid_l1_set              , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, SOC_PPC_NOF_PORT_DEFINED_VT_PROFILES },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_VD_MPLS_PORT_L1                                            , arad_pp_dbal_vt_cam_mpls_port_termination_set          , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 0                                    },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_VD_INDEXED_VDxINITIALVID_L3                                , arad_pp_dbal_vt_cam_vdxinitialvid_l1_set                  , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, SOC_PPC_NOF_PORT_DEFINED_VT_PROFILES },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_VD_INDEXED_VDxOUTERVID_L3                                  , arad_pp_dbal_vt_cam_vdxoutervid_l1_set              , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, SOC_PPC_NOF_PORT_DEFINED_VT_PROFILES },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_IPV4_VDxINITIALVID                                         , arad_pp_dbal_vt_cam_ipv4_vdxinitialvid_set             , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 0                                    },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_IPV4_VDxOUTERVID                                           , arad_pp_dbal_vt_cam_ipv4_vdxoutervid_set               , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 0                                    },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_IPV4_PORT_TERMINATION                                      , arad_pp_dbal_vt_cam_ipv4_port_termination_set          , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 0                                    },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_EVB_UN_C_TAG                                              , arad_pp_dbal_vt_cam_evb_set                              , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, OUTER_VID },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_EVB_S_TAG                                                 , arad_pp_dbal_vt_cam_evb_set                              , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, OUTER_INNER_VID                                    },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_DOUBLE_TAG_PRIORITY_INITIAL_VID                           , arad_pp_dbal_vt_cam_double_tag_pri_set                   , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 0                                    },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_DOUBLE_TAG_INITIAL_VID                                    , arad_pp_dbal_vt_cam_double_tag_set                       , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 0                                    },

        {ARAD_PP_DBAL_VT_PROG_SELECTION_IPV4_VDxINITIALVID_AFTER_RCY                               , arad_pp_dbal_vt_cam_ipv4_vdxinitialvid_recycle_set     , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 0                                    },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_IPV4_VDxOUTERVID_AFTER_RCY                                 , arad_pp_dbal_vt_cam_ipv4_vdxoutervid_recycle_set       , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 0                                    },

        {ARAD_PP_DBAL_VT_PROG_SELECTION_SINGLE_TAG_5_TUPLE_QINQ                                    , arad_pp_dbal_vt_cam_5_tuple_qinq_set                   , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, TST1_VID_ONE_TAG                     },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_DOUBLE_TAG_5_TUPLE_QINQ                                    , arad_pp_dbal_vt_cam_5_tuple_qinq_set                   , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, TST1_VID_DOUBLE_TAG                  },


        {ARAD_PP_DBAL_VT_PROG_SELECTION_UNTAGGED_TST1                                              , arad_pp_dbal_vt_cam_untagged_tst1_set                  , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 0                                    },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_ONE_TAG_TST1                                               , arad_pp_dbal_vt_cam_one_tag_tst1_set                   , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 0                                    },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_DOUBLE_TAG_TST1                                            , arad_pp_dbal_vt_cam_double_tag_tst1_set                , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 0                                    },


        {ARAD_PP_DBAL_VT_PROG_SELECTION_UNTAGGED_VRRP                                              , arad_pp_dbal_vt_cam_untagged_vrrp_set                  , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 0                                    },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_ONE_TAG_VRRP                                               , arad_pp_dbal_vt_cam_one_tag_vrrp_set                   , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 0                                    },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_DOUBLE_TAG_VRRP                                            , arad_pp_dbal_vt_cam_double_tag_vrrp_set                , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 0                                    },


        {ARAD_PP_DBAL_VT_PROG_SELECTION_UNTAGGED_EXP_NULL_TST1                                     , arad_pp_dbal_vt_cam_untagged_exp_null_tst1_set         , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 0                                    },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_ONE_TAG_EXP_NULL_TST1                                      , arad_pp_dbal_vt_cam_one_tag_exp_null_tst1_set          , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 0                                    },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_DOUBLE_TAG_EXP_NULL_TST1                                   , arad_pp_dbal_vt_cam_double_tag_exp_null_tst1_set       , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 0                                    },

        {ARAD_PP_DBAL_VT_PROG_SELECTION_OUTER_INNER_PCP_1_TST2                                     , arad_pp_dbal_vt_cam_outer_inner_pcp_tst2_set           , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, TPID1                                },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_OUTER_INNER_PCP_2_TST2                                     , arad_pp_dbal_vt_cam_outer_inner_pcp_tst2_set           , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, TPID2                                },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_OUTER_PCP_1_TST2                                           , arad_pp_dbal_vt_cam_outer_pcp_tst2_set                 , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, TPID1                                },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_OUTER_PCP_2_TST2                                           , arad_pp_dbal_vt_cam_outer_pcp_tst2_set                 , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, TPID2                                },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_OUTER_INNER_1_TST2                                         , arad_pp_dbal_vt_cam_outer_inner_tst2_set               , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, TPID1                                },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_OUTER_INNER_2_TST2                                         , arad_pp_dbal_vt_cam_outer_inner_tst2_set               , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, TPID2                                },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_OUTER_1_TST2                                               , arad_pp_dbal_vt_cam_outer_tst2_set                     , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, TPID1                                },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_OUTER_2_TST2                                               , arad_pp_dbal_vt_cam_outer_tst2_set                     , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, TPID2                                },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_UNTAGGED_TST2                                              , arad_pp_dbal_vt_cam_untagged_tst2_set                  , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 0                                    },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_TEST2                                                      , arad_pp_dbal_vt_cam_test2_set                          , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 0                                    },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_EXTENDER_PE                                                , arad_pp_dbal_vt_cam_extender_pe_set                    , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, OUTER_VID                            },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_EXTENDER_PE_UT                                             , arad_pp_dbal_vt_cam_extender_pe_set                    , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, INITIAL_VID                          },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_CUSTOM_PP_PORT_TUNNEL                                      , arad_pp_dbal_vt_cam_custom_pp_port_tunnel_set          , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 0                                    },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_VD_INITIALVID_VLAN_DOMAIN_MPLS_L1                          , arad_pp_dbal_vt_cam_vlan_domain_mpls_l1_set                 , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 0                                    },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_PON_UNTAGGED                                               , arad_pp_dbal_vt_cam_pon_all_progs_set                  , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, PROG_SEL_PON_UNTAGGED                },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_PON_PRIORITY_TAG                                           , arad_pp_dbal_vt_cam_pon_all_progs_set                  , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, PROG_SEL_PON_PRIORITY_TAG            },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_PON_ONE_TAG_1                                              , arad_pp_dbal_vt_cam_pon_all_progs_set                  , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, PROG_SEL_PON_ONE_TAG_1               },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_PON_ONE_TAG_2                                              , arad_pp_dbal_vt_cam_pon_all_progs_set                  , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, PROG_SEL_PON_ONE_TAG_2               },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_PON_TWO_TAGS                                               , arad_pp_dbal_vt_cam_pon_all_progs_set                  , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, PROG_SEL_PON_TWO_TAGS                },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_PON_TWO_TAGS_VS_TUNNEL_ID                                  , arad_pp_dbal_vt_cam_pon_all_progs_set                  , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, PROG_SEL_PON_TWO_TAGS_VS_TUNNEL_ID   },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_PON_IGNORE_2ND_TAG                                         , arad_pp_dbal_vt_cam_pon_all_progs_set                  , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, PROG_SEL_PON_IGNORE_2ND_TAG          },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_SECTION_OAM_ONE_TAG_1                                      , arad_pp_dbal_vt_cam_section_oam_all_progs_set          , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, PROG_SEL_SECTION_OAM_ONE_TAG_1       },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_SECTION_OAM_ONE_TAG_2                                      , arad_pp_dbal_vt_cam_section_oam_all_progs_set          , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, PROG_SEL_SECTION_OAM_ONE_TAG_2       },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_SECTION_OAM_DOUBLE_TAGS                                    , arad_pp_dbal_vt_cam_section_oam_all_progs_set          , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, PROG_SEL_SECTION_OAM_TWO_TAGS        },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_VD_INDEXED_VDxINITIALVID_MPLS4_L1L2                        , arad_pp_dbal_vt_cam_vdxinitialvid_mpls4_set            , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, SOC_PPC_NOF_PORT_DEFINED_VT_PROFILES },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_VD_INDEXED_VDxOUTERVID_MPLS4_L1L2                          , arad_pp_dbal_vt_cam_vdxoutervid_mpls4_set              , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, SOC_PPC_NOF_PORT_DEFINED_VT_PROFILES },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_IPV6_VDxINITIALVID                                         , arad_pp_dbal_vt_cam_ipv6_vdxinitialvid_set             , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 0                                    },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_IPV6_VDxOUTERVID                                           , arad_pp_dbal_vt_cam_ipv6_vdxoutervid_set               , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 0                                    },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_IPV6_VDxINITIALVID_AFTER_RECYCLE                           , arad_pp_dbal_vt_cam_ipv6_vdxinitialvid_set             , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 1                                    },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_IPV6_VDxOUTERVID_AFTER_RECYCLE                             , arad_pp_dbal_vt_cam_ipv6_vdxoutervid_set               , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 1                                    },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_GENERALIZED_RCH0                                           , arad_pp_dbal_vt_cam_generalized_rch_set                , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 1                                    },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_GENERALIZED_RCH1                                           , arad_pp_dbal_vt_cam_generalized_rch_set                , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 0                                    },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_3MTSE_PLUS_FRR_UNTAGGED_M3_PLUS_ELI                        , arad_pp_dbal_vt_cam_3mtse_plus_frr_vdxinitialvid_set   , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, PROG_SEL_3MTSE_PLUS_FRR_UNTAGGED_M3_PLUS_ELI    },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_3MTSE_PLUS_FRR_TAG_1_M3_PLUS_ELI                           , arad_pp_dbal_vt_cam_3mtse_plus_frr_vdxoutervid_set     , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, PROG_SEL_3MTSE_PLUS_FRR_TAG_1_M3_PLUS_ELI       },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_3MTSE_PLUS_FRR_UNTAGGED_M2_PLUS                            , arad_pp_dbal_vt_cam_3mtse_plus_frr_vdxinitialvid_set   , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, PROG_SEL_3MTSE_PLUS_FRR_UNTAGGED_M2_PLUS        },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_3MTSE_PLUS_FRR_TAG_1_M2_PLUS                               , arad_pp_dbal_vt_cam_3mtse_plus_frr_vdxoutervid_set     , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, PROG_SEL_3MTSE_PLUS_FRR_TAG_1_M2_PLUS           },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_3MTSE_PLUS_FRR_UNTAGGED                                    , arad_pp_dbal_vt_cam_3mtse_plus_frr_vdxinitialvid_set   , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, PROG_SEL_3MTSE_PLUS_FRR_UNTAGGED                },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_3MTSE_PLUS_FRR_TAG_1                                       , arad_pp_dbal_vt_cam_3mtse_plus_frr_vdxoutervid_set     , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, PROG_SEL_3MTSE_PLUS_FRR_TAG_1                   },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_2MTSE_PLUS_FRR_DOUBLE_TAG_L1                               , arad_pp_dbal_vt_cam_2mtse_plus_frr_vdxouter_inner_vid_l1_set     , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 0                   },
        {ARAD_PP_DBAL_VT_PROG_SELECTION_2MTSE_PLUS_FRR_DOUBLE_TAG_PCP_L1                           , arad_pp_dbal_vt_cam_2mtse_plus_frr_vdxouter_inner_vid_pcp_l1_set     , {-1}, {ARAD_PP_DBAL_VT_PROG_INVALID}, 0                   }

};



int32 arad_pp_dbal_tt_cam_tm_set                           (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused          );
int32 arad_pp_dbal_tt_cam_oam_bfd_stat_set                 (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 stat_type       );
int32 arad_pp_dbal_tt_cam_mpls2_set                        (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused          );
int32 arad_pp_dbal_tt_cam_mpls_l3_l2_set                   (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused          );
int32 arad_pp_dbal_tt_cam_mpls_port_l3_l2_set              (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused          );
int32 arad_pp_dbal_tt_cam_mpls3_set                        (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused          );
int32 arad_pp_dbal_tt_cam_mpls_l1_l1_set                   (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 is_gal          );
int32 arad_pp_dbal_tt_cam_mpls_l1_l2_set                   (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 is_gal          );
int32 arad_pp_dbal_tt_cam_mpls_l1_l3_eli_set               (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 is_gal          );
int32 arad_pp_dbal_tt_cam_mpls_l1_l2__eli_inrif_set        (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused          );
int32 arad_pp_dbal_tt_cam_mpls_l2_l2_set                   (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused          );
int32 arad_pp_dbal_tt_cam_mpls_l2_l3_gal_set               (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused          );
int32 arad_pp_dbal_tt_cam_mldp_overlay_after_recycle_set   (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused          );
int32 arad_pp_dbal_tt_cam_arp_set                          (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused          );
int32 arad_pp_dbal_tt_cam_bridge_set                       (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 bridge_cause    );
int32 arad_pp_dbal_tt_cam_uknown_l3_set                    (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused          );
int32 arad_pp_dbal_tt_cam_trill_set                        (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 nof_tags        );
int32 arad_pp_dbal_tt_cam_trill_trap_set                   (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused          );
int32 arad_pp_dbal_tt_cam_fc_set                           (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused          );
int32 arad_pp_dbal_tt_cam_mac_in_mac_set                   (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 mac_in_mac_type );
int32 arad_pp_dbal_tt_cam_ipv4_router_set                  (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused          );
int32 arad_pp_dbal_tt_cam_ipv4_router_tunnels_set          (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused          );
int32 arad_pp_dbal_tt_cam_ipv4_router_dip_found_set        (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 is_vpls         );
int32 arad_pp_dbal_tt_cam_ipv4_port_router_dip_found_set   (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 tunnel_type     );
int32 arad_pp_dbal_tt_cam_ipv4_router_ipmc_after_rcy_set   (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 is_vpls         );
int32 arad_pp_dbal_tt_cam_ipv4_router_uc_dip_not_found_set (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused          );
int32 arad_pp_dbal_tt_cam_ipv4_router_mc_dip_not_found_set (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused          );
int32 arad_pp_dbal_tt_cam_ipv4_router_compatible_mc_set    (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 nof_tags        );
int32 arad_pp_dbal_tt_cam_ipv6_router_set                  (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 is_recycle);
int32 arad_pp_dbal_tt_cam_dipv6_compressed_set             (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 mac_in_mac_type );
int32 arad_pp_dbal_tt_cam_extender_untag_check_set         (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 extender_type   );
int32 arad_pp_dbal_tt_cam_extender_cb_set                  (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 extender_type   );

int32 arad_pp_dbal_tt_cam_mpls_mac_set                     (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line);
int32 arad_pp_dbal_tt_cam_mpls_mac_and_raw_mpls_set        (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line);

int32 arad_pp_dbal_tt_cam_pon_ipv4_bridge_set                  (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused          );
int32 arad_pp_dbal_tt_cam_pon_ipv6_bridge_set                  (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused          );
int32 arad_pp_dbal_tt_cam_pon_ipv4_bridge_compatible_mc_set    (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 nof_tags        );
int32 arad_pp_dbal_tt_cam_pon_ipv4_bridge_dip_found_set        (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 is_vpls         );
int32 arad_pp_dbal_tt_cam_ipv4_router_dip_sip_vrf_set          (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 dummy           );
int32 arad_pp_dbal_tt_cam_ipv4_for_explicit_null_set           (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused          );
int32 arad_pp_dbal_tt_cam_tunnel_for_two_explicit_null_set     (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection,int next_line, uint32 unused           );
int32 arad_pp_dbal_tt_cam_mpls_l1_gal_exactly_set              (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 is_gal);
int32 arad_pp_dbal_tt_cam_mpls_l2_gal_exactly_set              (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused);
int32 arad_pp_dbal_tt_cam_vlan_domain_mpls_l2_set(int unit,  ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused);
int32 arad_pp_dbal_tt_cam_mpls_index_as_order_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 function);
int32 arad_pp_dbal_tt_cam_mpls4_l3_l4_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int line, uint32 sem_hit);
int32 arad_pp_dbal_tt_cam_ip_disable_on_lif_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection,int next_line, uint32 type);
int32 arad_pp_dbal_tt_cam_ipv6_router_comp_dest_found_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 is_recycle);
int32 arad_pp_dbal_tt_cam_pppoe_set                          (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused          );
int32 arad_pp_dbal_tt_cam_3mtse_plus_frr_set                 (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 vtt_3mtse_plus_frr_type);
int32 arad_pp_dbal_tt_cam_2mtse_plus_frr_set                 (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 vtt_2mtse_plus_frr_type);
int32 arad_pp_dbal_tt_cam_generalized_rch_ip_set            (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused          );
int32 arad_pp_dbal_tt_cam_generalized_rch_mpls_set            (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused          );
int32 arad_pp_dbal_tt_cam_l2tp_set (int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused          );


ARAD_PP_DBAL_TT_PROGRAM_SELECTION_INFO tt_dbal_program_selection[ARAD_PP_DBAL_TT_NOF_PROGRAM_SELECTION] = {

        {ARAD_PP_DBAL_TT_PROG_SELECTION_TM                            , arad_pp_dbal_tt_cam_tm_set                            , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 0                    },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_OAM_STAT                      , arad_pp_dbal_tt_cam_oam_bfd_stat_set                  , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, IS_OAM_STAT          },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_BFD_STAT                      , arad_pp_dbal_tt_cam_oam_bfd_stat_set                  , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, IS_BFD_STAT          },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_MLDP_OVERLAY_AFTER_RCY        , arad_pp_dbal_tt_cam_mldp_overlay_after_recycle_set    , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 0                    },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_UNINDEXED_MPLS_L2             , arad_pp_dbal_tt_cam_mpls2_set                         , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 0                    },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_INDEXED_MPLS_L3_L2            , arad_pp_dbal_tt_cam_mpls_l3_l2_set                    , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 0                    },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_INDEXED_MPLS_PORT_L3_L2       , arad_pp_dbal_tt_cam_mpls_port_l3_l2_set               , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 0                    },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_UNINDEXED_MPLS_L3             , arad_pp_dbal_tt_cam_mpls3_set                         , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 0                    },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_UNINDEXED_MPLS_L2_L3_ELI      , arad_pp_dbal_tt_cam_mpls_l1_l3_eli_set                , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 0                    },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_INDEXED_L1_L1                 , arad_pp_dbal_tt_cam_mpls_l1_l1_set                    , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 0                    },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_INDEXED_L1_L1_GAL             , arad_pp_dbal_tt_cam_mpls_l1_l1_set                    , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 1                    },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_INDEXED_L1_L2                 , arad_pp_dbal_tt_cam_mpls_l1_l2_set                    , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 0                    },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_INDEXED_L1_L2_GAL             , arad_pp_dbal_tt_cam_mpls_l1_l2_set                    , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 1                    },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_INDEXED_L2_L2                 , arad_pp_dbal_tt_cam_mpls_l2_l2_set                    , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 0                    },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_INDEXED_L2_L3_GAL             , arad_pp_dbal_tt_cam_mpls_l2_l3_gal_set                , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 0                    },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_INDEXED_L2_L3_ELI             , arad_pp_dbal_tt_cam_mpls_l1_l3_eli_set                , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 0                    },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_INDEXED_L1_L2_INRIF           , arad_pp_dbal_tt_cam_mpls2_set                         , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 0                    },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_INDEXED_L1_L2ELI_INRIF        , arad_pp_dbal_tt_cam_mpls_l1_l2__eli_inrif_set         , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 0                    },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_INDEXED_L1_L2_L3ELI_INRIF     , arad_pp_dbal_tt_cam_mpls_l1_l3_eli_set                , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 0                    },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_INDEXED_MPLS_L3_L4            , arad_pp_dbal_tt_cam_mpls3_set                         , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 0                    },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_INDEX_AS_ORDER_L2_L3        , arad_pp_dbal_tt_cam_mpls_index_as_order_set                         , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 1                    },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_INDEX_AS_ORDER_L2_L2        , arad_pp_dbal_tt_cam_mpls_index_as_order_set                         , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 2                    },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_INDEX_AS_ORDER_L1_L2        , arad_pp_dbal_tt_cam_mpls_index_as_order_set                         , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 3                    },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_INDEX_AS_ORDER_L1_L1        , arad_pp_dbal_tt_cam_mpls_index_as_order_set                         , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 4                    },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_INDEX_AS_ORDER_NOP        , arad_pp_dbal_tt_cam_mpls_index_as_order_set                         , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 5                    },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_ARP                           , arad_pp_dbal_tt_cam_arp_set                           , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 0                    },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_BRIDGE_STAR                   , arad_pp_dbal_tt_cam_bridge_set                        , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, BRIDGE_WITHOUT_MYMAC },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_BRIDGE_STAR_ROUTE_DISABLE     , arad_pp_dbal_tt_cam_bridge_set                        , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, BRIDGE_ROUTE_DISABLE },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_BRIDGE_STAR_ROUTE_DISABLE_IP  , arad_pp_dbal_tt_cam_bridge_set                        , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, BRIDGE_ROUTE_DISABLE_IP },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_BRIDGE_OAM                    , arad_pp_dbal_tt_cam_bridge_set                        , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, BRIDGE_OAM           },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_UNKNOWN_L3                    , arad_pp_dbal_tt_cam_uknown_l3_set                     , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 0                    },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_TRILL_ONE_TAG                 , arad_pp_dbal_tt_cam_trill_set                         , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, TRILL_ONE_TAG        },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_TRILL_TWO_TAGS                , arad_pp_dbal_tt_cam_trill_set                         , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, TRILL_TWO_TAG        },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_TRILL_TRAP                    , arad_pp_dbal_tt_cam_trill_trap_set                    , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 0                    },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_FC_WITH_VFT                   , arad_pp_dbal_tt_cam_fc_set                            , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, WITH_VFT             },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_FC                            , arad_pp_dbal_tt_cam_fc_set                            , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, NO_VFT               },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_MIM_MC                        , arad_pp_dbal_tt_cam_mac_in_mac_set                    , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, MAC_IN_MAC_MC        },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_MIM_WITH_BTAG                 , arad_pp_dbal_tt_cam_mac_in_mac_set                    , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, MAC_IN_MAC_WITH_BTAG },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_IPV4_ROUTER                   , arad_pp_dbal_tt_cam_ipv4_router_set                   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 0                    },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_IPV4_ROUTER_PWE_GRE_IPMC_RCY  , arad_pp_dbal_tt_cam_ipv4_router_ipmc_after_rcy_set    , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 1                    },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_IPV4_ROUTER_PWE_GRE_DIP_FOUND , arad_pp_dbal_tt_cam_ipv4_router_dip_found_set         , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 1                    },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_IPV4_ROUTER_DIP_FOUND_IPMC_RCY, arad_pp_dbal_tt_cam_ipv4_router_ipmc_after_rcy_set    , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 0                    },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_IPV4_ROUTER_DIP_FOUND         , arad_pp_dbal_tt_cam_ipv4_router_dip_found_set         , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 0                    },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_IPV4_ROUTER_L2_GRE_PORT       , arad_pp_dbal_tt_cam_ipv4_port_router_dip_found_set    , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, L2_GRE               },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_IPV4_ROUTER_VXLAN_PORT        , arad_pp_dbal_tt_cam_ipv4_port_router_dip_found_set    , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, VXLAN                },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_IPV4_ROUTER_UC_DIP_NOT_FOUND  , arad_pp_dbal_tt_cam_ipv4_router_uc_dip_not_found_set  , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 0                    },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_IPV4_ROUTER_MC_DIP_NOT_FOUND  , arad_pp_dbal_tt_cam_ipv4_router_mc_dip_not_found_set  , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 0                    },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_IPV4_ROUTER_COMPATIBLE_MC     , arad_pp_dbal_tt_cam_ipv4_router_compatible_mc_set     , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 0                    },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_IPV6_ROUTER                   , arad_pp_dbal_tt_cam_ipv6_router_set                   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 0                    },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_DIPV6_COMPRESSED              , arad_pp_dbal_tt_cam_dipv6_compressed_set              , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 0                    },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_EXTENDER_UNTAG_CHECK_IP_MC    , arad_pp_dbal_tt_cam_extender_untag_check_set          , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, EXTENDER_IP_MC       },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_EXTENDER_UNTAG_CHECK_IP_UC    , arad_pp_dbal_tt_cam_extender_untag_check_set          , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, EXTENDER_IP_UC       },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_EXTENDER_UNTAG_CHECK          , arad_pp_dbal_tt_cam_extender_untag_check_set          , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, EXTENDER_ETH         },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_PON_IP_BRIDGE_COMPATIBLE_MC   , arad_pp_dbal_tt_cam_pon_ipv4_bridge_compatible_mc_set , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 0 				   },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_PON_IPV4_BRIDGE_UC            , arad_pp_dbal_tt_cam_pon_ipv4_bridge_set               , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 0 				   },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_PON_IPV6_BRIDGE_UC            , arad_pp_dbal_tt_cam_pon_ipv6_bridge_set               , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 0 				   },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_PON_IPV4_BRIDGE_DIP_FOUND     , arad_pp_dbal_tt_cam_pon_ipv4_bridge_dip_found_set     , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 0 				   },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_IPV4_ROUTER_UC_DIP_SIP_VRF    , arad_pp_dbal_tt_cam_ipv4_router_dip_sip_vrf_set       , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 0 				   },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_IPV4_ROUTER_L3_GRE_PORT       , arad_pp_dbal_tt_cam_ipv4_router_dip_sip_vrf_set       , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, DUMMY_TYPE           },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_IPV4_FOR_EXPLICIT_NULL        , arad_pp_dbal_tt_cam_ipv4_for_explicit_null_set        , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 0 				   },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_TUNNEL_FOR_TWO_EXPLICIT_NULL  , arad_pp_dbal_tt_cam_tunnel_for_two_explicit_null_set  , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 0 				   },        
        {ARAD_PP_DBAL_TT_PROG_SELECTION_EXTENDER_CB_IP_MC             , arad_pp_dbal_tt_cam_extender_cb_set                   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, EXTENDER_IP_MC       },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_EXTENDER_CB_IP_UC             , arad_pp_dbal_tt_cam_extender_cb_set                   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, EXTENDER_IP_UC       },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_EXTENDER_CB                   , arad_pp_dbal_tt_cam_extender_cb_set                   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, EXTENDER_ETH         },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_IPV4_ROUTER_TUNNELS           , arad_pp_dbal_tt_cam_ipv4_router_tunnels_set           , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 0                    },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_INDEXED_L1_GAL_ONLY           , arad_pp_dbal_tt_cam_mpls_l1_gal_exactly_set           , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 0                    },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_INDEXED_L2_GAL_ONLY           , arad_pp_dbal_tt_cam_mpls_l2_gal_exactly_set           , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 0                    } ,    
        {ARAD_PP_DBAL_TT_PROG_SELECTION_VLAN_DOMAIN_MPLS_L2           , arad_pp_dbal_tt_cam_vlan_domain_mpls_l2_set                , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 0                    },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_INDEXED_MPLS4_L3_L4           , arad_pp_dbal_tt_cam_mpls4_l3_l4_set                   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 0                    },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_INDEXED_MPLS4_NOP             , arad_pp_dbal_tt_cam_mpls4_l3_l4_set                   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 1                    },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_IP_DISABLE_ON_LIF             , arad_pp_dbal_tt_cam_ip_disable_on_lif_set             , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 1                    },        
        {ARAD_PP_DBAL_TT_PROG_SELECTION_IPV4_DISABLE_ON_LIF           , arad_pp_dbal_tt_cam_ip_disable_on_lif_set             , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 2                    },        
        {ARAD_PP_DBAL_TT_PROG_SELECTION_IPV6_DISABLE_ON_LIF           , arad_pp_dbal_tt_cam_ip_disable_on_lif_set             , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 3                    },      
        {ARAD_PP_DBAL_TT_PROG_SELECTION_IPV6_ROUTER_COMP_DEST_FOUND   , arad_pp_dbal_tt_cam_ipv6_router_comp_dest_found_set   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 0                    },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_IPV6_ROUTER_AFTER_RECYCLE     , arad_pp_dbal_tt_cam_ipv6_router_set                   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 1                    },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_IPV6_ROUTER_COMP_DEST_FOUND_AFTER_RECYCLE   , arad_pp_dbal_tt_cam_ipv6_router_comp_dest_found_set   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 1      },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_PPPOE                         , arad_pp_dbal_tt_cam_pppoe_set                         , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 0                    },
		{ARAD_PP_DBAL_TT_PROG_SELECTION_L2TP						  , arad_pp_dbal_tt_cam_l2tp_set						  , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 0 				   },        
		{ARAD_PP_DBAL_TT_PROG_SELECTION_GENERALIZED_RCH_IP            , arad_pp_dbal_tt_cam_generalized_rch_ip_set            , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 0                    },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_GENERALIZED_RCH_MPLS          , arad_pp_dbal_tt_cam_generalized_rch_mpls_set          , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, 0                    },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_3MTSE_PLUS_FRR_M1                            , arad_pp_dbal_tt_cam_3mtse_plus_frr_set                   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, VTT_3MTSE_PLUS_FRR_M1               },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_3MTSE_PLUS_FRR_M2_HIT_SEM                    , arad_pp_dbal_tt_cam_3mtse_plus_frr_set                   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, VTT_3MTSE_PLUS_FRR_M2_HIT_SEM       },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_3MTSE_PLUS_FRR_M2_MISS_SEM                   , arad_pp_dbal_tt_cam_3mtse_plus_frr_set                   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, VTT_3MTSE_PLUS_FRR_M2_MISS_SEM      },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_3MTSE_PLUS_FRR_M2                            , arad_pp_dbal_tt_cam_3mtse_plus_frr_set                   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, VTT_3MTSE_PLUS_FRR_M2      },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_3MTSE_PLUS_FRR_M3_PLUS_ELI_IML_SEM_HIT       , arad_pp_dbal_tt_cam_3mtse_plus_frr_set                   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, VTT_3MTSE_PLUS_FRR_M3_PLUS_ELI_IML_SEM_HIT      },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_3MTSE_PLUS_FRR_M3_PLUS_ELI_IML_SEM_MISS      , arad_pp_dbal_tt_cam_3mtse_plus_frr_set                   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, VTT_3MTSE_PLUS_FRR_M3_PLUS_ELI_IML_SEM_MISS      },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_3MTSE_PLUS_FRR_M3_PLUS_ELI                   , arad_pp_dbal_tt_cam_3mtse_plus_frr_set                   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, VTT_3MTSE_PLUS_FRR_M3_PLUS_ELI      },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_3MTSE_PLUS_FRR_M3_ELI                        , arad_pp_dbal_tt_cam_3mtse_plus_frr_set                   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, VTT_3MTSE_PLUS_FRR_M3_ELI           },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_3MTSE_PLUS_FRR_M3_HIT_TCAM                   , arad_pp_dbal_tt_cam_3mtse_plus_frr_set                   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, VTT_3MTSE_PLUS_FRR_M3_HIT_SEM_AND_TCAM      },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_3MTSE_PLUS_FRR_M3_MISS_TCAM                  , arad_pp_dbal_tt_cam_3mtse_plus_frr_set                   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, VTT_3MTSE_PLUS_FRR_M3_DC_SEM_AND_TCAM     },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_3MTSE_PLUS_FRR_M3_HIT_SEM                    , arad_pp_dbal_tt_cam_3mtse_plus_frr_set                   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, VTT_3MTSE_PLUS_FRR_M3_HIT_SEM       },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_3MTSE_PLUS_FRR_M3_MISS_SEM                   , arad_pp_dbal_tt_cam_3mtse_plus_frr_set                   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, VTT_3MTSE_PLUS_FRR_M3_MISS_SEM      },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_3MTSE_PLUS_FRR_M3_PLUS_HIT_SEM_AND_TCAM      , arad_pp_dbal_tt_cam_3mtse_plus_frr_set                   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, VTT_3MTSE_PLUS_FRR_M3_PLUS_HIT_SEM_AND_TCAM       },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_3MTSE_PLUS_FRR_M3_PLUS_HIT_TCAM              , arad_pp_dbal_tt_cam_3mtse_plus_frr_set                   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, VTT_3MTSE_PLUS_FRR_M3_PLUS_HIT_TCAM },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_3MTSE_PLUS_FRR_M3_PLUS_MISS_TCAM             , arad_pp_dbal_tt_cam_3mtse_plus_frr_set                   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, VTT_3MTSE_PLUS_FRR_M3_PLUS_MISS_TCAM},
        {ARAD_PP_DBAL_TT_PROG_SELECTION_3MTSE_PLUS_FRR_M3_PLUS_MISS_SEM_AND_TCAM     , arad_pp_dbal_tt_cam_3mtse_plus_frr_set                   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, VTT_3MTSE_PLUS_FRR_M3_PLUS_MISS_SEM_AND_TCAM       },
		{ARAD_PP_DBAL_TT_PROG_SELECTION_3MTSE_PLUS_FRR_M3_PLUS_ELI_GAL               , arad_pp_dbal_tt_cam_3mtse_plus_frr_set                   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, VTT_3MTSE_PLUS_FRR_M3_PLUS_ELI_GAL  },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_3MTSE_PLUS_FRR_M3_PLUS_ELI_HIT_SEM           , arad_pp_dbal_tt_cam_3mtse_plus_frr_set                   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, VTT_3MTSE_PLUS_FRR_M3_PLUS_ELI_HIT_SEM       },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_3MTSE_PLUS_FRR_M3_PLUS_ELI_MISS_SEM          , arad_pp_dbal_tt_cam_3mtse_plus_frr_set                   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, VTT_3MTSE_PLUS_FRR_M3_PLUS_ELI_MISS_SEM       },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_3MTSE_PLUS_FRR_M3_PLUS_ELI_GAL_HIT_TCAM      , arad_pp_dbal_tt_cam_3mtse_plus_frr_set                   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, VTT_3MTSE_PLUS_FRR_M3_PLUS_ELI_GAL_HIT_SEM_AND_TCAM       },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_3MTSE_PLUS_FRR_M3_PLUS_ELI_GAL_MISS_TCAM     , arad_pp_dbal_tt_cam_3mtse_plus_frr_set                   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, VTT_3MTSE_PLUS_FRR_M3_PLUS_ELI_GAL_DC_SEM_AND_TCAM       },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_3MTSE_PLUS_FRR_M3_PLUS_NO_RANGE_HIT_TCAM     , arad_pp_dbal_tt_cam_3mtse_plus_frr_set                   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, VTT_3MTSE_PLUS_FRR_M3_PLUS_NO_RANGE_HIT_TCAM       },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_3MTSE_PLUS_FRR_M3_PLUS_NO_RANGE_MISS_TCAM    , arad_pp_dbal_tt_cam_3mtse_plus_frr_set                   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, VTT_3MTSE_PLUS_FRR_M3_PLUS_NO_RANGE_MISS_TCAM       },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_3MTSE_PLUS_FRR_M3_PLUS_ELI_HIT_TCAM          , arad_pp_dbal_tt_cam_3mtse_plus_frr_set                   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, VTT_3MTSE_PLUS_FRR_M3_PLUS_DFLT_HIT_SEM_AND_TCAM       },
        {ARAD_PP_DBAL_TT_PROG_SELECTION_3MTSE_PLUS_FRR_M3_PLUS_ELI_MISS_TCAM         , arad_pp_dbal_tt_cam_3mtse_plus_frr_set                   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, VTT_3MTSE_PLUS_FRR_M3_PLUS_DFLT_DC_SEM_AND_TCAM       },
		{ARAD_PP_DBAL_TT_PROG_SELECTION_2MTSE_PLUS_FRR_M1                            , arad_pp_dbal_tt_cam_2mtse_plus_frr_set                   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, VTT_2MTSE_PLUS_FRR_M1                 },
		{ARAD_PP_DBAL_TT_PROG_SELECTION_2MTSE_PLUS_FRR_M3_PLUS_R1_ELI                , arad_pp_dbal_tt_cam_2mtse_plus_frr_set                   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, VTT_2MTSE_PLUS_FRR_M3_PLUS_R1_ELI     },
		{ARAD_PP_DBAL_TT_PROG_SELECTION_2MTSE_PLUS_FRR_M2_HIT_TCAM                   , arad_pp_dbal_tt_cam_2mtse_plus_frr_set                   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, VTT_2MTSE_PLUS_FRR_M2_HIT_TCAM        },
		{ARAD_PP_DBAL_TT_PROG_SELECTION_2MTSE_PLUS_FRR_M2_MISS_TCAM                  , arad_pp_dbal_tt_cam_2mtse_plus_frr_set                   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, VTT_2MTSE_PLUS_FRR_M2_MISS_TCAM       },
		{ARAD_PP_DBAL_TT_PROG_SELECTION_2MTSE_PLUS_FRR_M3_PLUS_R3_GAL_HIT_TCAM       , arad_pp_dbal_tt_cam_2mtse_plus_frr_set                   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, VTT_2MTSE_PLUS_FRR_M3_PLUS_R3_GAL_HIT_TCAM       },
		{ARAD_PP_DBAL_TT_PROG_SELECTION_2MTSE_PLUS_FRR_M3_PLUS_R3_GAL_MISS_TCAM      , arad_pp_dbal_tt_cam_2mtse_plus_frr_set                   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, VTT_2MTSE_PLUS_FRR_M3_PLUS_R3_GAL_MISS_TCAM       },
		{ARAD_PP_DBAL_TT_PROG_SELECTION_2MTSE_PLUS_FRR_M3_R2_IML                     , arad_pp_dbal_tt_cam_2mtse_plus_frr_set                   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, VTT_2MTSE_PLUS_FRR_M3_R2_IML       },
		{ARAD_PP_DBAL_TT_PROG_SELECTION_2MTSE_PLUS_FRR_M2                            , arad_pp_dbal_tt_cam_2mtse_plus_frr_set                   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, VTT_2MTSE_PLUS_FRR_M2                 },
		{ARAD_PP_DBAL_TT_PROG_SELECTION_2MTSE_PLUS_FRR_M3_ELI                        , arad_pp_dbal_tt_cam_2mtse_plus_frr_set                   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, VTT_2MTSE_PLUS_FRR_M3_ELI             },
		{ARAD_PP_DBAL_TT_PROG_SELECTION_2MTSE_PLUS_FRR_M3_PLUS_ELI_GAL               , arad_pp_dbal_tt_cam_2mtse_plus_frr_set                   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, VTT_2MTSE_PLUS_FRR_M3_PLUS_ELI_GAL    },
		{ARAD_PP_DBAL_TT_PROG_SELECTION_2MTSE_PLUS_FRR_M3_PLUS_ELI_HIT_TCAM          , arad_pp_dbal_tt_cam_2mtse_plus_frr_set                   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, VTT_2MTSE_PLUS_FRR_M3_PLUS_ELI_HIT_TCAM    },
		{ARAD_PP_DBAL_TT_PROG_SELECTION_2MTSE_PLUS_FRR_M3_PLUS_ELI_MISS_TCAM         , arad_pp_dbal_tt_cam_2mtse_plus_frr_set                   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, VTT_2MTSE_PLUS_FRR_M3_PLUS_ELI_MISS_TCAM    },
		{ARAD_PP_DBAL_TT_PROG_SELECTION_2MTSE_PLUS_FRR_M3_PLUS_R2_ELI                , arad_pp_dbal_tt_cam_2mtse_plus_frr_set                   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, VTT_2MTSE_PLUS_FRR_M3_PLUS_R2_ELI        },
		{ARAD_PP_DBAL_TT_PROG_SELECTION_2MTSE_PLUS_FRR_M3_PLUS_NO_RANGE_HIT_TCAM     , arad_pp_dbal_tt_cam_2mtse_plus_frr_set                   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, VTT_2MTSE_PLUS_FRR_M3_PLUS_NO_RANGE_HIT_TCAM    },
		{ARAD_PP_DBAL_TT_PROG_SELECTION_2MTSE_PLUS_FRR_M3_PLUS_NO_RANGE_MISS_TCAM    , arad_pp_dbal_tt_cam_2mtse_plus_frr_set                   , {-1}, {ARAD_PP_DBAL_TT_PROG_INVALID}, VTT_2MTSE_PLUS_FRR_M3_PLUS_NO_RANGE_MISS_TCAM    },


};



void arad_pp_dbal_vtt_program_property_swap(ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property );
void arad_pp_dbal_vtt_find_namespace_database(int unit, SOC_PPC_MPLS_TERM_NAMESPACE_TYPE namespace_required, SOC_PPC_MPLS_TERM_NAMESPACE_TYPE *namespace, uint32 *database);


uint32 arad_pp_vtt_init(int unit)
{

    int res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    if(is_g_prog_soc_prop_initilized[unit] == 0)
    {
        res = arad_pp_isem_access_programs_soc_properties_get(unit);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }

    
    res = arad_pp_dbal_vtt_program_init(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

    
    res = arad_pp_isem_access_init_unsafe(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 21, exit);
    res = arad_pp_dbal_vtt_cam_init(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

    
    res = arad_pp_dbal_vtt_program_set(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

    
    res = arad_pp_dbal_vtt_cam_set(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 23, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_vtt_init()", 0, 0);
}


uint32 arad_pp_dbal_vtt_program_init(int unit)
{
	int i,res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    for (i = 0; i < ARAD_PP_DBAL_VT_NOF_PROGRAMS; i++) {
        vt_dbal_programs[i].prog_used[unit] = - 1;
    }

    for (i = 0; i < ARAD_PP_DBAL_TT_NOF_PROGRAMS; i++) {
        tt_dbal_programs[i].prog_used[unit] = - 1;
    }

    
    for(i=0; i < ARAD_PP_DBAL_VT_NOF_PROGRAM_SELECTION; i++){
        vt_dbal_program_selection[i].priority[unit]  = -1;
        vt_dbal_program_selection[i].prog_name[unit] = ARAD_PP_DBAL_VT_PROG_INVALID;
    }

    
    for(i=0; i < ARAD_PP_DBAL_TT_NOF_PROGRAM_SELECTION; i++){
        tt_dbal_program_selection[i].priority[unit]  = -1;
        tt_dbal_program_selection[i].prog_name[unit] = ARAD_PP_DBAL_TT_PROG_INVALID;
    }

    
    res = arad_pp_isem_prog_programs_init(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    
    

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_dbal_vtt_program_init()", 0, 0);
}

uint32 arad_pp_dbal_vt_program_init(int unit)
{
    int last_dbal_vt_program_id = 0;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    if(g_prog_soc_prop[unit].custom_feature_vt_tst2)
    {
        if(SOC_IS_ARADPLUS_A0(unit) && g_prog_soc_prop[unit].oam_statistics_enable)
            vt_dbal_programs[ARAD_PP_DBAL_VT_PROG_TM_STAT               ].prog_used[unit] = last_dbal_vt_program_id++;
        else
            vt_dbal_programs[ARAD_PP_DBAL_VT_PROG_TM                    ].prog_used[unit] = last_dbal_vt_program_id++;
        vt_dbal_programs[ARAD_PP_DBAL_VT_PROG_OUTER_INNER_PCP_1_TST2].prog_used[unit] = last_dbal_vt_program_id++;
        vt_dbal_programs[ARAD_PP_DBAL_VT_PROG_OUTER_INNER_PCP_2_TST2].prog_used[unit] = last_dbal_vt_program_id++;
        vt_dbal_programs[ARAD_PP_DBAL_VT_PROG_OUTER_PCP_1_TST2      ].prog_used[unit] = last_dbal_vt_program_id++;
        vt_dbal_programs[ARAD_PP_DBAL_VT_PROG_OUTER_PCP_2_TST2      ].prog_used[unit] = last_dbal_vt_program_id++;
        vt_dbal_programs[ARAD_PP_DBAL_VT_PROG_OUTER_INNER_1_TST2    ].prog_used[unit] = last_dbal_vt_program_id++;
        vt_dbal_programs[ARAD_PP_DBAL_VT_PROG_OUTER_INNER_2_TST2    ].prog_used[unit] = last_dbal_vt_program_id++;
        vt_dbal_programs[ARAD_PP_DBAL_VT_PROG_OUTER_1_TST2          ].prog_used[unit] = last_dbal_vt_program_id++;
        vt_dbal_programs[ARAD_PP_DBAL_VT_PROG_OUTER_2_TST2          ].prog_used[unit] = last_dbal_vt_program_id++;
        vt_dbal_programs[ARAD_PP_DBAL_VT_PROG_UNTAGGED_TST2         ].prog_used[unit] = last_dbal_vt_program_id++;
    }

    if (last_dbal_vt_program_id > (SOC_DPP_DEFS_GET(unit, nof_vtt_programs)))
    {
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_ISEM_ACCESS_PROGRAMS_FULL_ERR, 8100, exit); \
    }
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_dbal_vt_program_init()", 0, 0);
}
uint32 arad_pp_dbal_vtt_cam_init(int unit)
{
    int priority = 0;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    if(g_prog_soc_prop[unit].custom_feature_vt_tst2)
    {
        if(SOC_IS_ARADPLUS_A0(unit) && g_prog_soc_prop[unit].oam_statistics_enable) {
            vt_dbal_program_selection[ARAD_PP_DBAL_VT_PROG_SELECTION_TM_STAT].priority[unit]  = priority++;
            vt_dbal_program_selection[ARAD_PP_DBAL_VT_PROG_SELECTION_TM_STAT].prog_name[unit] = ARAD_PP_DBAL_VT_PROG_TM_STAT;
        } else {
            vt_dbal_program_selection[ARAD_PP_DBAL_VT_PROG_SELECTION_TM].priority[unit]  = priority++;
            vt_dbal_program_selection[ARAD_PP_DBAL_VT_PROG_SELECTION_TM].prog_name[unit] = ARAD_PP_DBAL_VT_PROG_TM;
        }
        vt_dbal_program_selection[ARAD_PP_DBAL_VT_PROG_SELECTION_OUTER_INNER_PCP_1_TST2].priority[unit]  = priority++;
        vt_dbal_program_selection[ARAD_PP_DBAL_VT_PROG_SELECTION_OUTER_INNER_PCP_1_TST2].prog_name[unit] = ARAD_PP_DBAL_VT_PROG_OUTER_INNER_PCP_1_TST2;
        priority++;
        vt_dbal_program_selection[ARAD_PP_DBAL_VT_PROG_SELECTION_OUTER_INNER_PCP_2_TST2].priority[unit]  = priority++;
        vt_dbal_program_selection[ARAD_PP_DBAL_VT_PROG_SELECTION_OUTER_INNER_PCP_2_TST2].prog_name[unit] = ARAD_PP_DBAL_VT_PROG_OUTER_INNER_PCP_2_TST2;
        priority++;

        vt_dbal_program_selection[ARAD_PP_DBAL_VT_PROG_SELECTION_OUTER_PCP_1_TST2      ].priority[unit]  = priority++;
        vt_dbal_program_selection[ARAD_PP_DBAL_VT_PROG_SELECTION_OUTER_PCP_1_TST2      ].prog_name[unit] = ARAD_PP_DBAL_VT_PROG_OUTER_PCP_1_TST2;

        vt_dbal_program_selection[ARAD_PP_DBAL_VT_PROG_SELECTION_OUTER_PCP_2_TST2      ].priority[unit]  = priority++;
        vt_dbal_program_selection[ARAD_PP_DBAL_VT_PROG_SELECTION_OUTER_PCP_2_TST2      ].prog_name[unit] = ARAD_PP_DBAL_VT_PROG_OUTER_PCP_2_TST2;

        vt_dbal_program_selection[ARAD_PP_DBAL_VT_PROG_SELECTION_OUTER_INNER_1_TST2    ].priority[unit]  = priority++;
        vt_dbal_program_selection[ARAD_PP_DBAL_VT_PROG_SELECTION_OUTER_INNER_1_TST2    ].prog_name[unit] = ARAD_PP_DBAL_VT_PROG_OUTER_INNER_1_TST2;
        priority++;

        vt_dbal_program_selection[ARAD_PP_DBAL_VT_PROG_SELECTION_OUTER_INNER_2_TST2    ].priority[unit]  = priority++;
        vt_dbal_program_selection[ARAD_PP_DBAL_VT_PROG_SELECTION_OUTER_INNER_2_TST2    ].prog_name[unit] = ARAD_PP_DBAL_VT_PROG_OUTER_INNER_2_TST2;
        priority++;

        vt_dbal_program_selection[ARAD_PP_DBAL_VT_PROG_SELECTION_OUTER_1_TST2          ].priority[unit]  = priority++;
        vt_dbal_program_selection[ARAD_PP_DBAL_VT_PROG_SELECTION_OUTER_1_TST2          ].prog_name[unit] = ARAD_PP_DBAL_VT_PROG_OUTER_1_TST2;

        vt_dbal_program_selection[ARAD_PP_DBAL_VT_PROG_SELECTION_OUTER_2_TST2          ].priority[unit]  = priority++;
        vt_dbal_program_selection[ARAD_PP_DBAL_VT_PROG_SELECTION_OUTER_2_TST2          ].prog_name[unit] = ARAD_PP_DBAL_VT_PROG_OUTER_2_TST2;

        vt_dbal_program_selection[ARAD_PP_DBAL_VT_PROG_SELECTION_UNTAGGED_TST2         ].priority[unit]  = priority++;
        vt_dbal_program_selection[ARAD_PP_DBAL_VT_PROG_SELECTION_UNTAGGED_TST2         ].prog_name[unit] = ARAD_PP_DBAL_VT_PROG_UNTAGGED_TST2;
    }

    if (priority > (SOC_DPP_DEFS_GET(unit, nof_vtt_program_selection_lines)))
    {
        SOC_SAND_SET_ERROR_CODE(ARAD_PP_ISEM_ACCESS_PROGRAMS_FULL_ERR, 8100, exit); \
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_dbal_vtt_cam_init()", 0, 0);

}





void arad_pp_dbal_sw_db_vdxvid_sem_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 vid_type)
{

    sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_VLAN_DOMAIN ;

    switch(vid_type)
    {
        case INITIAL_VID:
        case TRILL_DESIGNATED_VID:
            sw_db_property->qual_info[1].qual_type     = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
            sw_db_property->qual_info[1].qual_nof_bits = 2;
            sw_db_property->qual_info[2].qual_type     = SOC_PPC_FP_QUAL_INITIAL_VID;
            sw_db_property->nof_qulifiers = 3;
            break;
        case OUTER_VID:
            sw_db_property->qual_info[1].qual_type     = SOC_PPC_FP_QUAL_IRPP_PROG_VAR; 
            sw_db_property->qual_info[1].qual_nof_bits = 2;
            sw_db_property->qual_info[2].qual_type     = SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID;
            sw_db_property->nof_qulifiers = 3;
            break;
        case OUTER_INNER_VID:
            sw_db_property->qual_info[1].qual_type     = SOC_PPC_FP_QUAL_IRPP_ALL_ONES;
            sw_db_property->qual_info[1].qual_nof_bits = 2;
            sw_db_property->qual_info[2].qual_type     = SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID;
            sw_db_property->qual_info[3].qual_type     = SOC_PPC_FP_QUAL_CMPRSD_INNER_VID;

            sw_db_property->nof_qulifiers = 4;
            break;
        case OUTER_INITIAL_VID:
            sw_db_property->qual_info[1].qual_type     = SOC_PPC_FP_QUAL_IRPP_ALL_ONES;
            sw_db_property->qual_info[1].qual_nof_bits = 2;
            sw_db_property->qual_info[2].qual_type     = SOC_PPC_FP_QUAL_INITIAL_VID;
            sw_db_property->qual_info[3].qual_type     = SOC_PPC_FP_QUAL_CMPRSD_INNER_VID;

            sw_db_property->nof_qulifiers = 4;
            break;
        case OUTER_INNER_VID_PCP:
        	sw_db_property->qual_info[1].qual_type     = SOC_PPC_FP_QUAL_IRPP_ALL_ONES;
            sw_db_property->qual_info[1].qual_nof_bits = 2;
        	sw_db_property->qual_info[2].qual_type     = SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID;
        	sw_db_property->qual_info[3].qual_type     = SOC_PPC_FP_QUAL_CMPRSD_INNER_VID;
        	sw_db_property->qual_info[4].qual_type     = SOC_PPC_FP_QUAL_HDR_VLAN_TAG_PRI_CFI;
        	sw_db_property->nof_qulifiers = 5;
            break;
        case INNER_VID:
        	sw_db_property->qual_info[1].qual_type     = SOC_PPC_FP_QUAL_IRPP_PROG_VAR; 
            sw_db_property->qual_info[1].qual_nof_bits = 2;
        	sw_db_property->qual_info[2].qual_type     = SOC_PPC_FP_QUAL_CMPRSD_INNER_VID;
            sw_db_property->nof_qulifiers = 3;
			break;

        default:
            sw_db_property->nof_qulifiers = -1;
            break;
    }

    if(vid_type == TRILL_DESIGNATED_VID)
        sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_TRILL_DESIGNATED_VID_PREFIX;
    else
        sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_BRIDGE_PREFIX;

}

#ifndef __KERNEL__

#if (((SOC_PPC_FP_QUAL_MPLS_KEY3 - SOC_PPC_FP_QUAL_MPLS_KEY2) != (SOC_PPC_FP_QUAL_MPLS_KEY2 -SOC_PPC_FP_QUAL_MPLS_KEY1)) || \
     ((SOC_PPC_FP_QUAL_MPLS_KEY3 - SOC_PPC_FP_QUAL_MPLS_KEY2) != (SOC_PPC_FP_QUAL_MPLS_KEY1 -SOC_PPC_FP_QUAL_MPLS_KEY0)))
#error "Jericho MPLS Key qualifiers are on different distance"
#endif

#if ((SOC_PPC_FP_QUAL_HDR_MPLS_BOS3 - SOC_PPC_FP_QUAL_HDR_MPLS_BOS2) != (SOC_PPC_FP_QUAL_HDR_MPLS_BOS2 -SOC_PPC_FP_QUAL_HDR_MPLS_BOS1))
#error "ARAD MPLS BOS qualifiers are on different distance"
#endif

#if ((SOC_PPC_FP_QUAL_HDR_MPLS_LABEL3 - SOC_PPC_FP_QUAL_HDR_MPLS_LABEL2) != (SOC_PPC_FP_QUAL_HDR_MPLS_LABEL2 -SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1))
#error "ARAD MPLS HDR qualifiers are on different distance"
#endif

#endif


void arad_pp_dbal_sw_db_mpls_unidexed_prefix_sem_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 mpls_label)
{
    int index=0;
    int nof_qualifiers = 1;

    if(mpls_label == MPLS_DUMMY ) {
        nof_qualifiers = 2;
        sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_MPLS_BOS1;

        sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ONES;
        sw_db_property->qual_info[1].qual_nof_bits = 21;

    } else {
        if(SOC_IS_JERICHO(unit))
        {
            if (g_prog_soc_prop[unit].evpn_enable) {
                sw_db_property->qual_info[index].qual_type = SOC_PPC_FP_QUAL_KEY0_EVPN_BOS_EXPECTED + mpls_label*(SOC_PPC_FP_QUAL_KEY1_EVPN_BOS_EXPECTED - SOC_PPC_FP_QUAL_KEY0_EVPN_BOS_EXPECTED);
                nof_qualifiers = 2;
                index++;
            }
            sw_db_property->qual_info[index].qual_type = SOC_PPC_FP_QUAL_MPLS_KEY0 + mpls_label*(SOC_PPC_FP_QUAL_MPLS_KEY1 - SOC_PPC_FP_QUAL_MPLS_KEY0);
            sw_db_property->qual_info[index].qual_nof_bits = 20;

        }
        else
        {
            nof_qualifiers = 2;

            if(g_prog_soc_prop[unit].is_bos_in_key_enable)
            {
                sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_MPLS_BOS1 + mpls_label*(SOC_PPC_FP_QUAL_HDR_MPLS_BOS2 - SOC_PPC_FP_QUAL_HDR_MPLS_BOS1);
                nof_qualifiers = 3;
                index = 1;
            }

            sw_db_property->qual_info[index].qual_type = SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1 + mpls_label*(SOC_PPC_FP_QUAL_HDR_MPLS_LABEL2 - SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1);
            sw_db_property->qual_info[index].qual_nof_bits = 4;
            sw_db_property->qual_info[index].qual_offset   = 16;

            sw_db_property->qual_info[index + 1].qual_type = SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1 + mpls_label*(SOC_PPC_FP_QUAL_HDR_MPLS_LABEL2- SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1);
            sw_db_property->qual_info[index + 1].qual_nof_bits = 16;

        }
    }

    sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_MPLS_UNINDEXED_PREFIX;
    sw_db_property->nof_qulifiers = nof_qualifiers;
}

void arad_pp_dbal_sw_db_mpls_gal_sem_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 mpls_label)
{
    int index=0;
    int nof_qualifiers = 1;

    if(SOC_IS_JERICHO(unit))
    {
        sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_MPLS_KEY0 + mpls_label*(SOC_PPC_FP_QUAL_MPLS_KEY1 - SOC_PPC_FP_QUAL_MPLS_KEY0);
        sw_db_property->qual_info[0].qual_nof_bits = 20;

    }
    else
    {
        nof_qualifiers = 2;

        sw_db_property->qual_info[index].qual_type = SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1 + mpls_label*(SOC_PPC_FP_QUAL_HDR_MPLS_LABEL2 - SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1);
        sw_db_property->qual_info[index].qual_nof_bits = 4;
        sw_db_property->qual_info[index].qual_offset   = 16;

        sw_db_property->qual_info[index + 1].qual_type = SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1 + mpls_label*(SOC_PPC_FP_QUAL_HDR_MPLS_LABEL2- SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1);
        sw_db_property->qual_info[index + 1].qual_nof_bits = 16;

    }

    sw_db_property->prefix        = ARAD_PP_ISEM_ACCESS_MPLS_PWE_GAL;
    sw_db_property->nof_qulifiers = nof_qualifiers;
}


void arad_pp_dbal_sw_db_mpls_port_sem_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY *sw_db_property, uint32 mpls_indexed) {
    int index = 0;
    int nof_qualifiers = 1;
    sw_db_property->nof_qulifiers = nof_qualifiers;

    sw_db_property->qual_info[index].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ONES;
    sw_db_property->qual_info[index].qual_nof_bits = 2;

    
    if (g_prog_soc_prop[unit].mpls_index) {
        sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_MPLS_L1_PREFIX;
    } else {
        sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_MPLS_UNINDEXED_PREFIX;
    }
}


void arad_pp_dbal_sw_db_mpls_indexed_sem_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 mpls_indexed )
{
    int index=0;
    int nof_qualifiers = 1;

    if(SOC_IS_JERICHO(unit))
    {
        mpls_indexed = mpls_indexed % MPLS_LABEL_BOS_START;

        if (g_prog_soc_prop[unit].evpn_enable) {
            sw_db_property->qual_info[index].qual_type = SOC_PPC_FP_QUAL_KEY0_EVPN_BOS_EXPECTED + (mpls_indexed / SOC_PPC_MPLS_TERM_NOF_NAMESPACE_TYPES)*(SOC_PPC_FP_QUAL_KEY1_EVPN_BOS_EXPECTED - SOC_PPC_FP_QUAL_KEY0_EVPN_BOS_EXPECTED);
            nof_qualifiers = 2;
            index++;
        }

		if (!SOC_DPP_CONFIG(unit)->pp.vtt_pwe_vid_search) { 
			sw_db_property->qual_info[index].qual_type = SOC_PPC_FP_QUAL_MPLS_KEY0 + (mpls_indexed / SOC_PPC_MPLS_TERM_NOF_NAMESPACE_TYPES) * (SOC_PPC_FP_QUAL_MPLS_KEY1 - SOC_PPC_FP_QUAL_MPLS_KEY0);
			sw_db_property->qual_info[index].qual_nof_bits = 20;
		} else {
			sw_db_property->qual_info[index].qual_type = SOC_PPC_FP_QUAL_MPLS_KEY0 + (mpls_indexed / SOC_PPC_MPLS_TERM_NOF_NAMESPACE_TYPES) * (SOC_PPC_FP_QUAL_MPLS_KEY1 - SOC_PPC_FP_QUAL_MPLS_KEY0);
			sw_db_property->qual_info[index].qual_nof_bits = 32;

            index++;
			sw_db_property->qual_info[index].qual_type = SOC_PPC_FP_QUAL_MPLS_KEY0 + (mpls_indexed / SOC_PPC_MPLS_TERM_NOF_NAMESPACE_TYPES) * (SOC_PPC_FP_QUAL_MPLS_KEY1 - SOC_PPC_FP_QUAL_MPLS_KEY0);
			sw_db_property->qual_info[index].qual_offset = 32;
			sw_db_property->qual_info[index].qual_nof_bits = 14;

			sw_db_property->prefix_len = 4; 
            nof_qualifiers++;
		}
    }
    else
    {
        nof_qualifiers = 2;

        if(mpls_indexed/MPLS_LABEL_BOS_START)
        {
            mpls_indexed = mpls_indexed % MPLS_LABEL_BOS_START;
            sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_MPLS_BOS1 + (mpls_indexed / SOC_PPC_MPLS_TERM_NOF_NAMESPACE_TYPES)*(SOC_PPC_FP_QUAL_HDR_MPLS_BOS2 - SOC_PPC_FP_QUAL_HDR_MPLS_BOS1);
            nof_qualifiers = 3;
            index = 1;
        }

        sw_db_property->qual_info[index].qual_type = SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1 + (mpls_indexed / SOC_PPC_MPLS_TERM_NOF_NAMESPACE_TYPES)*(SOC_PPC_FP_QUAL_HDR_MPLS_LABEL2 - SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1);
        sw_db_property->qual_info[index].qual_nof_bits = 4;
        sw_db_property->qual_info[index].qual_offset   = 16;

        sw_db_property->qual_info[index + 1].qual_type = SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1 + (mpls_indexed / SOC_PPC_MPLS_TERM_NOF_NAMESPACE_TYPES)*(SOC_PPC_FP_QUAL_HDR_MPLS_LABEL2 - SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1);
        sw_db_property->qual_info[index + 1].qual_nof_bits = 16;

    }

	sw_db_property->nof_qulifiers = nof_qualifiers;
	if (!SOC_DPP_CONFIG(unit)->pp.vtt_pwe_vid_search) { 
		sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_NAMESPACE_TO_PREFIX_WO_ELI(mpls_indexed % SOC_PPC_MPLS_TERM_NOF_NAMESPACE_TYPES); 
	}else{
		sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_NAMESPACE_TO_PREFIX_WO_ELI_4_bit(mpls_indexed % SOC_PPC_MPLS_TERM_NOF_NAMESPACE_TYPES); 
	}
}

void arad_pp_dbal_sw_db_mpls_eli_unidexed_prefix_sem_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 mpls_label )
{
    int index=0;
    int nof_qualifiers = 1;

    if(SOC_IS_JERICHO(unit))
    {
        sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_MPLS_KEY0 + mpls_label*(SOC_PPC_FP_QUAL_MPLS_KEY1 - SOC_PPC_FP_QUAL_MPLS_KEY0);
        sw_db_property->qual_info[0].qual_nof_bits = 20;

    }
    else
    {
        nof_qualifiers = 2;

        if(g_prog_soc_prop[unit].is_bos_in_key_enable)
        {
            sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_MPLS_BOS1 + (mpls_label+2)*(SOC_PPC_FP_QUAL_HDR_MPLS_BOS2 - SOC_PPC_FP_QUAL_HDR_MPLS_BOS1);
            nof_qualifiers = 3;
            index = 1;
        }

        sw_db_property->qual_info[index].qual_type = SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1 + mpls_label*(SOC_PPC_FP_QUAL_HDR_MPLS_LABEL2 - SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1);
        sw_db_property->qual_info[index].qual_nof_bits = 4;
        sw_db_property->qual_info[index].qual_offset   = 16;

        sw_db_property->qual_info[index + 1].qual_type = SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1 + mpls_label*(SOC_PPC_FP_QUAL_HDR_MPLS_LABEL2- SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1);
        sw_db_property->qual_info[index + 1].qual_nof_bits = 16;

    }

    sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_MPLS_ELI_UNINDEXED_PREFIX;
    sw_db_property->nof_qulifiers = nof_qualifiers;
}

void arad_pp_dbal_sw_db_mpls_eli_indexed_sem_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 mpls_eli_indexed )
{
    int index=0;
    int nof_qualifiers = 1;

    if(SOC_IS_JERICHO(unit))
    {
        sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_MPLS_KEY0 + (mpls_eli_indexed / SOC_PPC_MPLS_TERM_NOF_NAMESPACE_TYPES)*(SOC_PPC_FP_QUAL_MPLS_KEY1 - SOC_PPC_FP_QUAL_MPLS_KEY0);
        sw_db_property->qual_info[0].qual_nof_bits = 20;

    }
    else
    {
        nof_qualifiers = 2;

        if(g_prog_soc_prop[unit].is_bos_in_key_enable)
        {
            sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_MPLS_BOS1 + ((mpls_eli_indexed / SOC_PPC_MPLS_TERM_NOF_NAMESPACE_TYPES)+2)*(SOC_PPC_FP_QUAL_HDR_MPLS_BOS2 - SOC_PPC_FP_QUAL_HDR_MPLS_BOS1);
            nof_qualifiers = 3;
            index = 1;
        }

        sw_db_property->qual_info[index].qual_type = SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1 + (mpls_eli_indexed / SOC_PPC_MPLS_TERM_NOF_NAMESPACE_TYPES)*(SOC_PPC_FP_QUAL_HDR_MPLS_LABEL2 - SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1);
        sw_db_property->qual_info[index].qual_nof_bits = 4;
        sw_db_property->qual_info[index].qual_offset   = 16;

        sw_db_property->qual_info[index + 1].qual_type = SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1 + (mpls_eli_indexed / SOC_PPC_MPLS_TERM_NOF_NAMESPACE_TYPES)*(SOC_PPC_FP_QUAL_HDR_MPLS_LABEL2- SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1);
        sw_db_property->qual_info[index + 1].qual_nof_bits = 16;

    }

    sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_NAMESPACE_TO_PREFIX_ELI( mpls_eli_indexed % SOC_PPC_MPLS_TERM_NOF_NAMESPACE_TYPES);
    sw_db_property->nof_qulifiers = nof_qualifiers;
}

void arad_pp_dbal_sw_db_mpls_rif_indexed_sem_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 mpls_indexed )
{
    int index=0;
    int nof_qualifiers = 2;

    sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_IN_RIF_VALID_IN_RIF;

    if(SOC_IS_JERICHO(unit))
    {
        sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_MPLS_KEY0;
        sw_db_property->qual_info[1].qual_nof_bits = 20;

    }
    else
    {
        nof_qualifiers = 3;

        if(g_prog_soc_prop[unit].is_bos_in_key_enable)
        {
            sw_db_property->qual_info[1].qual_type = ((mpls_indexed / SOC_PPC_MPLS_TERM_NOF_NAMESPACE_TYPES) == 1) ? SOC_PPC_FP_QUAL_HDR_MPLS_BOS3 : SOC_PPC_FP_QUAL_HDR_MPLS_BOS1;
            nof_qualifiers = 4;
            index = 2;
        }

        sw_db_property->qual_info[index].qual_type = SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1;
        sw_db_property->qual_info[index].qual_nof_bits = 4;
        sw_db_property->qual_info[index].qual_offset   = 16;

        sw_db_property->qual_info[index + 1].qual_type = SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1;
        sw_db_property->qual_info[index + 1].qual_nof_bits = 16;

    }

    if((mpls_indexed / SOC_PPC_MPLS_TERM_NOF_NAMESPACE_TYPES) == 1)
    {
        sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_NAMESPACE_TO_PREFIX_ELI( mpls_indexed % SOC_PPC_MPLS_TERM_NOF_NAMESPACE_TYPES);
    }
    else
    {
        sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_NAMESPACE_TO_PREFIX_WO_ELI( mpls_indexed % SOC_PPC_MPLS_TERM_NOF_NAMESPACE_TYPES);
    }

    sw_db_property->nof_qulifiers = nof_qualifiers;
}

void arad_pp_dbal_sw_db_evb_sem_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 vid_type)
{

    sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_VLAN_DOMAIN ;
    sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ONES;
    sw_db_property->qual_info[1].qual_nof_bits = 2;

    if(vid_type == OUTER_VID) 
    {
        sw_db_property->qual_info[2].qual_type     = SOC_PPC_FP_QUAL_IRPP_PROG_VAR; 
        sw_db_property->qual_info[2].qual_nof_bits = 12;
    }
    else 
        sw_db_property->qual_info[2].qual_type     = SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID;


    sw_db_property->qual_info[3].qual_type = SOC_PPC_FP_QUAL_INITIAL_VID;

    sw_db_property->nof_qulifiers = 4;
    sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_BRIDGE_PREFIX;
}


void arad_pp_dbal_sw_db_vdxvid_pcp_sem_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 vid_type)
{
    sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_VLAN_DOMAIN ;
    sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_IRPP_PROG_VAR; 
    sw_db_property->qual_info[1].qual_nof_bits = 2;
    sw_db_property->qual_info[2].qual_type = SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID;
    sw_db_property->qual_info[3].qual_type = SOC_PPC_FP_QUAL_HDR_VLAN_TAG_PRI_CFI;
    sw_db_property->nof_qulifiers = 4;

    if(g_prog_soc_prop[unit].pon_enable){
        if (_BCM_PPD_PON_PP_PORT_MAPPING_BY_PASS_IN_JER(unit)) {
            sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_PON_VD_OUTER_PCP_PREFIX << 2;
        } else {
            sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_PON_VD_OUTER_PCP_PREFIX;
        }
    } else {
        sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_VD_OUTER_PCP_PREFIX;
    }

}

void arad_pp_dbal_sw_db_vid_tcam_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 vid_type)
{
    if(vid_type == INITIAL_VID)
        sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_INITIAL_VID;
    else
        sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID;

    sw_db_property->prefix = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    sw_db_property->nof_qulifiers = 1;
}

void arad_pp_dbal_sw_db_inner_outer_vid_pcp_tcam_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 not_in_use)
{
    sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID;
    sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_VLAN_DOMAIN;
    sw_db_property->qual_info[2].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ONES;
    sw_db_property->qual_info[2].qual_nof_bits = 2;
    sw_db_property->qual_info[3].qual_type = SOC_PPC_FP_QUAL_CMPRSD_INNER_VID;
    sw_db_property->qual_info[4].qual_type = SOC_PPC_FP_QUAL_HDR_VLAN_TAG_PRI_CFI;

    sw_db_property->nof_qulifiers = 5;
}

void arad_pp_dbal_sw_db_spoofv4_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 not_in_use)
{

    sw_db_property->qual_info[0].qual_type     = SOC_PPC_FP_QUAL_HDR_IPV4_SIP;
    sw_db_property->qual_info[0].qual_offset   = 16;
    sw_db_property->qual_info[0].qual_nof_bits = 16;
    sw_db_property->qual_info[1].qual_type     = SOC_PPC_FP_QUAL_HDR_IPV4_SIP;
    sw_db_property->qual_info[1].qual_nof_bits = 16;
    sw_db_property->qual_info[2].qual_type     = SOC_PPC_FP_QUAL_HDR_SA;
    sw_db_property->qual_info[2].qual_nof_bits = 8;
    sw_db_property->prefix = (SOC_IS_ARADPLUS_AND_BELOW(unit) ? 1 : ARAD_PP_ISEM_ACCESS_IPV4_SPOOF_PREFIX);
    sw_db_property->prefix_len = (SOC_IS_ARADPLUS_AND_BELOW(unit) ? 1 : ARAD_PP_ISEM_ACCESS_NOF_BITS_PREFIX);
    sw_db_property->nof_qulifiers = 3;

}


void arad_pp_dbal_sw_db_dipv6_tcam_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 compressed)
{
    if (g_prog_soc_prop[unit].ipv6_term_dip_sip_enable) {
        int qual_num = 0;
        
        sw_db_property->qual_info[qual_num].qual_type     = SOC_PPC_FP_QUAL_HDR_IPV6_DIP_LOW;
        sw_db_property->qual_info[qual_num].qual_offset   = 32;
        sw_db_property->qual_info[qual_num].qual_nof_bits = 32;
        qual_num++;
        sw_db_property->qual_info[qual_num].qual_type     = SOC_PPC_FP_QUAL_HDR_IPV6_DIP_LOW;
        sw_db_property->qual_info[qual_num].qual_offset   = 16;
        sw_db_property->qual_info[qual_num].qual_nof_bits = 16;
        qual_num++;
        sw_db_property->qual_info[qual_num].qual_type     = SOC_PPC_FP_QUAL_HDR_IPV6_DIP_LOW;
        sw_db_property->qual_info[qual_num].qual_offset   = 0;
        sw_db_property->qual_info[qual_num].qual_nof_bits = 16;
        qual_num++;
        sw_db_property->qual_info[qual_num].qual_type     = SOC_PPC_FP_QUAL_HDR_IPV6_DIP_HIGH;
        sw_db_property->qual_info[qual_num].qual_offset   = 32;
        sw_db_property->qual_info[qual_num].qual_nof_bits = 32;
        qual_num++;
        sw_db_property->qual_info[qual_num].qual_type     = SOC_PPC_FP_QUAL_HDR_IPV6_DIP_HIGH;
        sw_db_property->qual_info[qual_num].qual_nof_bits = 32;
        qual_num++;
        sw_db_property->qual_info[qual_num].qual_type     = SOC_PPC_FP_QUAL_IN_RIF_VALID_VRF;  
        sw_db_property->qual_info[qual_num].qual_nof_bits = (SOC_DPP_CONFIG(unit))->pp.ip6_tunnel_term_in_tcam_vrf_nof_bits;
        qual_num++;
        sw_db_property->qual_info[qual_num].qual_type     = SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2;  
        sw_db_property->qual_info[qual_num].qual_nof_bits = 11;
        qual_num++;
        sw_db_property->qual_info[qual_num].qual_type     = SOC_PPC_FP_QUAL_IRPP_IN_PORT_KEY_GEN_VAR;    
        sw_db_property->qual_info[qual_num].qual_nof_bits = 4;   
        qual_num++;
        sw_db_property->nof_qulifiers = qual_num;  
    } else {
        if (compressed)
        {
            sw_db_property->qual_info[0].qual_type     = SOC_PPC_FP_QUAL_HDR_IPV6_DIP_LOW;
            sw_db_property->qual_info[0].qual_nof_bits = 8;
            sw_db_property->qual_info[1].qual_type     = SOC_PPC_FP_QUAL_HDR_IPV6_DIP_HIGH;
            sw_db_property->qual_info[1].qual_offset   = 32;
            sw_db_property->qual_info[1].qual_nof_bits = 32;
            sw_db_property->qual_info[2].qual_type     = SOC_PPC_FP_QUAL_HDR_IPV6_DIP_HIGH;
            sw_db_property->qual_info[2].qual_nof_bits = 32;
            sw_db_property->nof_qulifiers = 3;
        }
        else
        {
            sw_db_property->qual_info[0].qual_type     = SOC_PPC_FP_QUAL_HDR_IPV6_DIP_LOW;
            sw_db_property->qual_info[0].qual_offset   = 32;
            sw_db_property->qual_info[0].qual_nof_bits = 32;
            
            sw_db_property->qual_info[1].qual_type     = SOC_PPC_FP_QUAL_HDR_IPV6_DIP_LOW;
            sw_db_property->qual_info[1].qual_offset   = 16;
            sw_db_property->qual_info[1].qual_nof_bits = 16;
            sw_db_property->qual_info[2].qual_type     = SOC_PPC_FP_QUAL_HDR_IPV6_DIP_LOW;
            sw_db_property->qual_info[2].qual_offset   = 0;
            sw_db_property->qual_info[2].qual_nof_bits = 16;
            
            sw_db_property->qual_info[3].qual_type     = SOC_PPC_FP_QUAL_HDR_IPV6_DIP_HIGH;
            sw_db_property->qual_info[3].qual_offset   = 32;
            sw_db_property->qual_info[3].qual_nof_bits = 32;
            sw_db_property->qual_info[4].qual_type     = SOC_PPC_FP_QUAL_HDR_IPV6_DIP_HIGH;
            sw_db_property->qual_info[4].qual_nof_bits = 32;
            sw_db_property->nof_qulifiers = 5;
        }
    }
}


void arad_pp_dbal_sw_db_spoofv6_tcam_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 compressed)
{
    if (compressed)
    {
        sw_db_property->qual_info[0].qual_type     = SOC_PPC_FP_QUAL_HDR_IPV6_SIP_HIGH;
        sw_db_property->qual_info[0].qual_offset   = 32;
        sw_db_property->qual_info[0].qual_nof_bits = 32;
        sw_db_property->qual_info[1].qual_type     = SOC_PPC_FP_QUAL_HDR_IPV6_SIP_HIGH;
        sw_db_property->qual_info[1].qual_nof_bits = 32;
        sw_db_property->qual_info[2].qual_type     = SOC_PPC_FP_QUAL_INITIAL_VSI;
        sw_db_property->qual_info[2].qual_nof_bits = 8;
        sw_db_property->nof_qulifiers = 3;
    }
    else
    {
        sw_db_property->qual_info[0].qual_type     = SOC_PPC_FP_QUAL_HDR_IPV6_SIP_LOW;
        sw_db_property->qual_info[0].qual_offset   = 32;
        sw_db_property->qual_info[0].qual_nof_bits = 32;
        sw_db_property->qual_info[1].qual_type     = SOC_PPC_FP_QUAL_HDR_IPV6_SIP_LOW;
        sw_db_property->qual_info[1].qual_nof_bits = 32;
        sw_db_property->qual_info[2].qual_type     = SOC_PPC_FP_QUAL_HDR_IPV6_SIP_HIGH;
        sw_db_property->qual_info[2].qual_offset   = 32;
        sw_db_property->qual_info[2].qual_nof_bits = 32;
        sw_db_property->qual_info[3].qual_type     = SOC_PPC_FP_QUAL_HDR_IPV6_SIP_HIGH;
        sw_db_property->qual_info[3].qual_nof_bits = 32;
        sw_db_property->qual_info[4].qual_type     = SOC_PPC_FP_QUAL_HDR_SA;
        sw_db_property->qual_info[4].qual_nof_bits = 8;
        sw_db_property->nof_qulifiers = 5;
    }
}

void arad_pp_dbal_sw_db_mpls_l1_l2_tcam_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 unused)
{

    if(SOC_IS_JERICHO(unit))
    {
        sw_db_property->nof_qulifiers = 2;

        sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_MPLS_KEY0;
        sw_db_property->qual_info[0].qual_nof_bits = 20;
        sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_MPLS_KEY1;
        sw_db_property->qual_info[1].qual_nof_bits = 20;

    }
    else
    {
        sw_db_property->nof_qulifiers = 3;

        sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1;
        sw_db_property->qual_info[0].qual_nof_bits = 4;
        sw_db_property->qual_info[0].qual_offset   = 16;

        sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1;
        sw_db_property->qual_info[1].qual_nof_bits = 16;

        sw_db_property->qual_info[2].qual_type = SOC_PPC_FP_QUAL_HDR_MPLS_LABEL2;
        sw_db_property->qual_info[2].qual_nof_bits = 20;
    }

}

void arad_pp_dbal_sw_db_mpls_l1_mpls4_tcam_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 vid)
{

    if(SOC_IS_JERICHO(unit))
    {
        sw_db_property->nof_qulifiers = 2;

        sw_db_property->qual_info[0].qual_type = (vid == OUTER_VID)?SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID:SOC_PPC_FP_QUAL_INITIAL_VID;

        sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_MPLS_KEY0;
        sw_db_property->qual_info[1].qual_nof_bits = 20;

    }
    else
    {
        sw_db_property->nof_qulifiers = 2;

        sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1;
        sw_db_property->qual_info[0].qual_nof_bits = 4;
        sw_db_property->qual_info[0].qual_offset   = 16;

        sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1;
        sw_db_property->qual_info[1].qual_nof_bits = 16;
    }

}

void arad_pp_dbal_sw_db_mpls_l1_tcam_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 mpls_index)
{

    if(SOC_IS_JERICHO(unit))
    {
        sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_MPLS_KEY0 + mpls_index *(SOC_PPC_FP_QUAL_MPLS_KEY1 - SOC_PPC_FP_QUAL_MPLS_KEY0);
        sw_db_property->qual_info[0].qual_nof_bits = 20;
        sw_db_property->nof_qulifiers = 1;
    }
    else
    {
        sw_db_property->qual_info[0].qual_type     = SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1;
        sw_db_property->qual_info[0].qual_nof_bits = 4;
        sw_db_property->qual_info[0].qual_offset   = 16;
        sw_db_property->qual_info[1].qual_type     = SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1;
        sw_db_property->qual_info[1].qual_nof_bits = 16;

        sw_db_property->nof_qulifiers = 2;
    }
}

void arad_pp_dbal_sw_db_mpls_l4b_l5g_tcam_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 vid)
{
	if(SOC_IS_JERICHO(unit))
	{

		sw_db_property->qual_info[0].qual_type = (vid == OUTER_VID)?SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID:SOC_PPC_FP_QUAL_INITIAL_VID;
		sw_db_property->qual_info[0].qual_nof_bits = 12;

		sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_MPLS_LABEL5;
        sw_db_property->qual_info[1].qual_nof_bits = 4;
        sw_db_property->qual_info[1].qual_offset   = 16;
		sw_db_property->qual_info[2].qual_type = SOC_PPC_FP_QUAL_HDR_MPLS_LABEL5;
		sw_db_property->qual_info[2].qual_nof_bits = 16;

		sw_db_property->qual_info[3].qual_type = SOC_PPC_FP_QUAL_HDR_MPLS_BOS4;
		sw_db_property->qual_info[3].qual_nof_bits = 1;

		sw_db_property->nof_qulifiers = 4;
   }

}

void arad_pp_dbal_sw_db_mpls_frr_coupling_tcam_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 mpls_label_vid_type)
{
    if((mpls_label_vid_type % NOF_VID_TYPES) ==  INITIAL_VID )
        sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_INITIAL_VID;
    else
        sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID;

    if(SOC_IS_JERICHO(unit))
    {
        sw_db_property->nof_qulifiers = 3;

        sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_MPLS_KEY0 + (mpls_label_vid_type / NOF_VID_TYPES)*(SOC_PPC_FP_QUAL_MPLS_KEY1 - SOC_PPC_FP_QUAL_MPLS_KEY0);
        sw_db_property->qual_info[1].qual_nof_bits = 20;
        sw_db_property->qual_info[2].qual_type = SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1 + (mpls_label_vid_type / NOF_VID_TYPES)*(SOC_PPC_FP_QUAL_HDR_MPLS_LABEL2 - SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1);

    }
    else
    {
        sw_db_property->nof_qulifiers = 4;

        sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1 + (mpls_label_vid_type / NOF_VID_TYPES)*(SOC_PPC_FP_QUAL_HDR_MPLS_LABEL2 - SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1);
        sw_db_property->qual_info[1].qual_nof_bits = 4;
        sw_db_property->qual_info[1].qual_offset   = 16;

        sw_db_property->qual_info[2].qual_type = SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1 + (mpls_label_vid_type / NOF_VID_TYPES)*(SOC_PPC_FP_QUAL_HDR_MPLS_LABEL2 - SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1);
        sw_db_property->qual_info[2].qual_nof_bits = 16;

        sw_db_property->qual_info[3].qual_type = SOC_PPC_FP_QUAL_HDR_MPLS_LABEL2 + (mpls_label_vid_type / NOF_VID_TYPES)*(SOC_PPC_FP_QUAL_HDR_MPLS_LABEL2 - SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1);
    }
}

void arad_pp_dbal_sw_db_trill_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 unused)
{
    sw_db_property->nof_qulifiers = 1;

    sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_TRILL_NATIVE_ETH_INNER_TPID;

    sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_INNER_TPID_PREFIX;

}

void arad_pp_dbal_sw_db_trill_ingress_nickname_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 not_in_use)
{
    sw_db_property->nof_qulifiers          = 1;
    sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_TRILL_INGRESS_NICK;
    sw_db_property->prefix                 = ARAD_PP_ISEM_ACCESS_TRILL_NICK_PREFIX;
}

void arad_pp_dbal_sw_db_trill_uc_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 nof_tags)
{
    if(g_prog_soc_prop[unit].trill_mode == 1)
        sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_VLAN_DOMAIN ;
    else
    {
        sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES ;
        if(SOC_IS_JERICHO(unit))
            sw_db_property->qual_info[0].qual_nof_bits = 9;
        else
            sw_db_property->qual_info[0].qual_nof_bits = 8;
    }

    switch(nof_tags)
    {
        case TRILL_ONE_TAG:
            sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_IRPP_PROG_VAR; 
            sw_db_property->qual_info[1].qual_nof_bits = 2;
            sw_db_property->qual_info[2].qual_type = SOC_PPC_FP_QUAL_TRILL_NATIVE_VLAN_VSI;
            sw_db_property->nof_qulifiers = 3;
        break;
        case TRILL_TWO_TAG:
            sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ONES;
            sw_db_property->qual_info[1].qual_nof_bits = 2;
            sw_db_property->qual_info[2].qual_type = SOC_PPC_FP_QUAL_TRILL_NATIVE_VLAN_VSI;
            sw_db_property->qual_info[3].qual_type = SOC_PPC_FP_QUAL_TRILL_NATIVE_INNER_VLAN_VSI;
            sw_db_property->nof_qulifiers = 4;
        break;
        default:
            sw_db_property->nof_qulifiers = -1;
        break;
    }

    if(g_prog_soc_prop[unit].trill_mode == 1)
        sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_BRIDGE_PREFIX;
    else
        sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_TRILL_VSI_PREFIX;

}
void arad_pp_dbal_sw_db_trill_tcam_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 not_in_use)
{
    sw_db_property->nof_qulifiers = 1;
    sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_TRILL_NATIVE_VLAN_VSI;

}







void arad_pp_dbal_sw_db_dip_sem_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 dummy)
{

    if(dummy == DUMMY_TYPE)
    {
        if (SOC_DPP_CONFIG(unit)->pp.ip_lif_dummy_id_vxlan || SOC_DPP_CONFIG(unit)->pp.ip_lif_dummy_id_gre) {
            sw_db_property->qual_info[0].qual_type     = SOC_PPC_FP_QUAL_IRPP_ALL_ONES;
            sw_db_property->qual_info[1].qual_type     = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
            sw_db_property->qual_info[1].qual_nof_bits = 2;
            sw_db_property->qual_info[2].qual_type     = SOC_PPC_FP_QUAL_HDR_IPV4_NEXT_PRTCL;
            sw_db_property->nof_qulifiers = 3;
        } else {
            sw_db_property->qual_info[0].qual_type     = SOC_PPC_FP_QUAL_IRPP_ALL_ONES;
            sw_db_property->qual_info[1].qual_type     = SOC_PPC_FP_QUAL_IRPP_ALL_ONES;
            sw_db_property->qual_info[1].qual_nof_bits = 1;
            sw_db_property->nof_qulifiers = 2;
        }
    } else if (dummy == DUMMY_FOR_EXPLICIT_NULL_TYPE)
    {
        sw_db_property->qual_info[0].qual_type     = SOC_PPC_FP_QUAL_IRPP_ALL_ONES;
        sw_db_property->qual_info[1].qual_type     = SOC_PPC_FP_QUAL_IRPP_ALL_ONES;
        sw_db_property->qual_info[1].qual_nof_bits = 2;
        sw_db_property->qual_info[2].qual_type     = SOC_PPC_FP_QUAL_HDR_MPLS_TTL1;

        sw_db_property->nof_qulifiers = 3;
    }
    else
    {
        sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_IPV4_DIP;
        sw_db_property->nof_qulifiers = 1;
    }     


    sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_IP_PREFIX;
}

void arad_pp_dbal_sw_db_sip_sem_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 not_in_use)
{
    sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_IPV4_SIP;
    sw_db_property->nof_qulifiers = 1;

    sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_OVERLAY_SIP_PREFIX;
}
void arad_pp_dbal_sw_db_dip_sip_tcam_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 vid_type)
{
    sw_db_property->nof_qulifiers = 3;

    if(vid_type == INITIAL_VID)
        sw_db_property->qual_info[0].qual_type    = SOC_PPC_FP_QUAL_INITIAL_VID;
    else
        sw_db_property->qual_info[0].qual_type    = SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID;

    sw_db_property->qual_info[1].qual_type        = SOC_PPC_FP_QUAL_HDR_IPV4_DIP;
    sw_db_property->qual_info[2].qual_type        = SOC_PPC_FP_QUAL_HDR_IPV4_SIP;
    if(g_prog_soc_prop[unit].ipv4_term_dip_sip_enable == 2)
    {
        sw_db_property->qual_info[3].qual_type     = SOC_PPC_FP_QUAL_HDR_IPV4_NEXT_PRTCL;
        sw_db_property->qual_info[4].qual_type     = SOC_PPC_FP_QUAL_IRPP_IN_PORT_KEY_GEN_VAR;
        sw_db_property->qual_info[4].qual_nof_bits = 4; 
        sw_db_property->nof_qulifiers = 5;
    }
}

void arad_pp_dbal_sw_db_ethernet_header_isid_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 unused)
{
    if (SOC_DPP_CONFIG(unit)->pp.test2) {

        sw_db_property->nof_qulifiers = 2; 
        sw_db_property->qual_info[0].qual_type    =  SOC_PPC_FP_QUAL_EID;
        sw_db_property->qual_info[1].qual_type    =  SOC_PPC_FP_QUAL_HDR_VLAN_TAG_ID;

    } else {
        sw_db_property->nof_qulifiers = 1; 
        sw_db_property->qual_info[0].qual_type    =  SOC_PPC_FP_QUAL_ETH_HEADER_ISID;

        if(g_prog_soc_prop[unit].test1 == 1)
        {
            sw_db_property->qual_info[0].qual_nof_bits = 16;
            sw_db_property->qual_info[0].qual_offset   = 8;
        }
    }
    

    sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_ISID_DOMAIN_ISID_PREFIX;
}

void arad_pp_dbal_sw_db_ethernet_header_bsa_isid_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 unused)
{
    sw_db_property->nof_qulifiers = 3;
    sw_db_property->qual_info[0].qual_type     =  SOC_PPC_FP_QUAL_ETH_HEADER_ISID;
    sw_db_property->qual_info[1].qual_type     =  SOC_PPC_FP_QUAL_HDR_SA;
    sw_db_property->qual_info[1].qual_offset   =  8;
    sw_db_property->qual_info[1].qual_nof_bits =  16;
    sw_db_property->qual_info[2].qual_type     =  SOC_PPC_FP_QUAL_HDR_SA;
    sw_db_property->qual_info[2].qual_offset   =  0;
    sw_db_property->qual_info[2].qual_nof_bits =  4;

    sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_ISID_DOMAIN_ISID_PREFIX;
}

void arad_pp_dbal_sw_db_mc_dipv4_rif_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 unused)
{
    sw_db_property->nof_qulifiers = 2;

    sw_db_property->qual_info[0].qual_type     = SOC_PPC_FP_QUAL_HDR_IPV4_DIP;
    sw_db_property->qual_info[0].qual_offset   = 4;
    sw_db_property->qual_info[0].qual_nof_bits = 28;

    sw_db_property->qual_info[1].qual_type     = SOC_PPC_FP_QUAL_IN_RIF_VALID_VRF;
    sw_db_property->qual_info[1].qual_nof_bits = 9;

    sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_RPA_ID_PREFIX;
}

void arad_pp_dbal_sw_db_tunnel_port_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 tunnel_type)
{

    
    if(tunnel_type == L2_GRE)
    {
        sw_db_property->nof_qulifiers = 2;
        sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_GRE_KEY;
        sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_GRE_CRKS;
        if (SOC_IS_L2GRE_VDC_ENABLE(unit)) {
            sw_db_property->nof_qulifiers = 3;
            sw_db_property->qual_info[2].qual_type = SOC_PPC_FP_QUAL_VLAN_DOMAIN;
        }
    } else {
        sw_db_property->nof_qulifiers = 1;
        sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_VXLAN_VNI;
        if (SOC_IS_VXLAN_VDC_ENABLE(unit)) {
            sw_db_property->nof_qulifiers = 2;
            sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_VLAN_DOMAIN;
        }
    }
    sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_IP_VSI_PREFIX;
}

void arad_pp_dbal_sw_db_tunnel_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 not_in_use)
{

    
    if(g_prog_soc_prop[unit].nvgre_enable)
    {
        sw_db_property->nof_qulifiers = 2;
        sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_GRE_KEY;
        sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_GRE_CRKS;
        if (SOC_IS_L2GRE_VDC_ENABLE(unit)) {
            sw_db_property->nof_qulifiers = 3;
            sw_db_property->qual_info[2].qual_type = SOC_PPC_FP_QUAL_VLAN_DOMAIN;
        }
    }

    if(g_prog_soc_prop[unit].vxlan_enable )
    {
        sw_db_property->nof_qulifiers = 1;
        sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_VXLAN_VNI;
        if (SOC_IS_VXLAN_VDC_ENABLE(unit)) {
            sw_db_property->nof_qulifiers = 2;
            sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_VLAN_DOMAIN;
        }
    }

    if(g_prog_soc_prop[unit].e_o_ip_enable)
    {
        sw_db_property->nof_qulifiers = 1;
        sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_NATIVE_VLAN_VSI;
    }

    sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_IP_VSI_PREFIX;
}
void arad_pp_dbal_sw_db_qinq_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 vid_type)
{

    sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_INITIAL_VID;
    sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_VLAN_DOMAIN;

    if(vid_type == OUTER_INNER_VID)
    {
        sw_db_property->nof_qulifiers          = 3;
        sw_db_property->qual_info[2].qual_type = SOC_PPC_FP_QUAL_CMPRSD_INNER_VID;
        sw_db_property->prefix                 = ARAD_PP_ISEM_ACCESS_FLEXIBLE_Q_IN_Q_DOUBLE;
    }
    else
    {
        sw_db_property->nof_qulifiers = 2;
        sw_db_property->prefix        = ARAD_PP_ISEM_ACCESS_FLEXIBLE_Q_IN_Q_SINGLE;
    }
}

void arad_pp_dbal_sw_db_5_tupple_tcam_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 unused)
{
    sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_INITIAL_VID;
    sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_IPV4_DIP;
    sw_db_property->qual_info[2].qual_type = SOC_PPC_FP_QUAL_HDR_IPV4_NEXT_PRTCL;
    sw_db_property->qual_info[3].qual_type = SOC_PPC_FP_QUAL_HDR_IPV4_SIP;
    sw_db_property->qual_info[4].qual_type = SOC_PPC_FP_QUAL_HDR_IPV4_DEST_PORT;
    sw_db_property->qual_info[5].qual_type = SOC_PPC_FP_QUAL_HDR_IPV4_SRC_PORT;

    sw_db_property->nof_qulifiers = 6;

}
void arad_pp_dbal_sw_db_vlan_domain_vid_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 vid_type)
{
    sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_VLAN_DOMAIN;

    switch(vid_type)
    {
    case TST1_VID_UNTAGGED:
        sw_db_property->nof_qulifiers = 1;
        sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_BRIDGE_DT;
        break;
    case TST1_VID_INITIAL_TAG:
        sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_INITIAL_VID;
        sw_db_property->nof_qulifiers = 2;
        sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_BRIDGE_DT;
        break;
    case TST1_VID_CMPRSD_ONE_TAG:
        sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID;
        sw_db_property->nof_qulifiers = 2;
        sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_BRIDGE_ST;
        break;
    case TST1_VID_DOUBLE_TAG:
        sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID;
        sw_db_property->qual_info[2].qual_type = SOC_PPC_FP_QUAL_CMPRSD_INNER_VID;
        sw_db_property->nof_qulifiers = 3;
        sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_BRIDGE_DT;
        break;
    default:
        sw_db_property->nof_qulifiers = -1;
        break;
    }

}

void arad_pp_dbal_sw_db_tst1_tcam_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 vid_type)
{

    SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX] = {{0}};
    int is_table_initiated = 0;
    uint32 res = 0;

    sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_INITIAL_VID;

    switch(vid_type)
    {
    case TST1_VID_UNTAGGED:
        sw_db_property->qual_info[1].qual_type     = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        sw_db_property->qual_info[1].qual_nof_bits = 12;
        sw_db_property->qual_info[2].qual_type     = SOC_PPC_FP_QUAL_VLAN_DOMAIN;
        sw_db_property->qual_info[3].qual_type     = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        sw_db_property->qual_info[3].qual_nof_bits = 28;
        sw_db_property->qual_info[4].qual_type     = SOC_PPC_FP_QUAL_UNTAG_HDR_ETHERTYPE;
        sw_db_property->nof_qulifiers = 5;
        break;
    case TST1_VID_ONE_TAG:
        sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        sw_db_property->qual_info[1].qual_nof_bits = 12;
        sw_db_property->qual_info[2].qual_type = SOC_PPC_FP_QUAL_VLAN_DOMAIN;
        sw_db_property->qual_info[3].qual_type = SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID;
        sw_db_property->qual_info[4].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        sw_db_property->qual_info[4].qual_nof_bits = 12;
        sw_db_property->qual_info[5].qual_type = SOC_PPC_FP_QUAL_HDR_VLAN_TAG_PRI_CFI;
        sw_db_property->qual_info[6].qual_type = SOC_PPC_FP_QUAL_ONE_TAG_HDR_ETHERTYPE;
        sw_db_property->nof_qulifiers = 7;
        break;

    case TST1_VID_DOUBLE_TAG:
        sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_2ND_VLAN_TAG_ID;
        sw_db_property->qual_info[2].qual_type = SOC_PPC_FP_QUAL_VLAN_DOMAIN;
        sw_db_property->qual_info[3].qual_type = SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID;
        sw_db_property->qual_info[4].qual_type = SOC_PPC_FP_QUAL_CMPRSD_INNER_VID;
        sw_db_property->qual_info[5].qual_type = SOC_PPC_FP_QUAL_HDR_VLAN_TAG_PRI_CFI;
        sw_db_property->qual_info[6].qual_type = SOC_PPC_FP_QUAL_DOUBLE_TAG_HDR_ETHERTYPE;
        sw_db_property->nof_qulifiers = 7;
        break;
    default:
        sw_db_property->nof_qulifiers = -1;
        break;
    }

    
    res = arad_pp_dbal_table_is_initiated(unit, SOC_DPP_DBAL_SW_TABLE_ID_TST1_TCAM_SKELETON, &is_table_initiated);
    if (res) {
            LOG_CLI((BSL_META("Fail in arad_pp_dbal_table_is_initiated\n")));
        }
    if (!is_table_initiated) {
        DBAL_QUAL_INFO_CLEAR(&qual_info, SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);       
        qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_VLAN_TAG_ID;
        qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_2ND_VLAN_TAG_ID;
        qual_info[2].qual_type = SOC_PPC_FP_QUAL_VLAN_DOMAIN;        
        qual_info[3].qual_type = SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID;
        qual_info[4].qual_type = SOC_PPC_FP_QUAL_CMPRSD_INNER_VID;        
        qual_info[5].qual_type = SOC_PPC_FP_QUAL_HDR_VLAN_TAG_CFI;
        qual_info[6].qual_type = SOC_PPC_FP_QUAL_HDR_VLAN_TAG_PRI;        
        qual_info[7].qual_type = SOC_PPC_FP_QUAL_HDR_ETHERTYPE;
        
        res = arad_pp_dbal_table_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_TST1_TCAM_SKELETON, ARAD_PP_ISEM_ACCESS_VT_CLASSIFICATIONS_EFP, 0,
                                                 SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM, 8, SOC_DPP_DBAL_ATI_TCAM_STATIC, qual_info, "VT TST1 skeleton");

        if (res) {
            LOG_CLI((BSL_META("Fail creating table VT TST1 skeleton\n")));
        }

        arad_pp_dbal_table_stage_create(unit, SOC_DPP_DBAL_SW_TABLE_ID_TST1_TCAM_SKELETON, SOC_PPC_FP_DATABASE_STAGE_INGRESS_VT, 0,-1);
    }

}

void arad_pp_dbal_sw_db_vrrp_tcam_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 vid_type)
{

    sw_db_property->nof_qulifiers = 6;
    sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_TCAM_VRRP_VSI_DA_PROFILE;

    switch(vid_type)
    {
    case TST1_VID_UNTAGGED:
        sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_INITIAL_VID;
        sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_VLAN_DOMAIN;
        sw_db_property->qual_info[2].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        sw_db_property->qual_info[2].qual_nof_bits = 12;
        sw_db_property->qual_info[3].qual_type = SOC_PPC_FP_QUAL_HDR_DA;
        sw_db_property->qual_info[3].qual_nof_bits = 32;
        sw_db_property->qual_info[3].qual_offset = 16;
        sw_db_property->qual_info[4].qual_type = SOC_PPC_FP_QUAL_HDR_DA;
        sw_db_property->qual_info[4].qual_nof_bits = 16;
        sw_db_property->qual_info[4].qual_offset = 0;
        sw_db_property->qual_info[5].qual_type     = SOC_PPC_FP_QUAL_UNTAG_HDR_ETHERTYPE;
        break;
    case TST1_VID_ONE_TAG:
        sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID;
        sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_VLAN_DOMAIN;
        sw_db_property->qual_info[2].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
        sw_db_property->qual_info[2].qual_nof_bits = 12;
        sw_db_property->qual_info[3].qual_type = SOC_PPC_FP_QUAL_HDR_DA;
        sw_db_property->qual_info[3].qual_nof_bits = 32;
        sw_db_property->qual_info[3].qual_offset = 16;
        sw_db_property->qual_info[4].qual_type = SOC_PPC_FP_QUAL_HDR_DA;
        sw_db_property->qual_info[4].qual_nof_bits = 16;
        sw_db_property->qual_info[4].qual_offset = 0;
        sw_db_property->qual_info[5].qual_type = SOC_PPC_FP_QUAL_ONE_TAG_HDR_ETHERTYPE;
        break;

    case TST1_VID_DOUBLE_TAG:
        sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID;
        sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_VLAN_DOMAIN;
        sw_db_property->qual_info[2].qual_type = SOC_PPC_FP_QUAL_CMPRSD_INNER_VID;
        sw_db_property->qual_info[3].qual_type = SOC_PPC_FP_QUAL_HDR_DA;
        sw_db_property->qual_info[3].qual_nof_bits = 32;
        sw_db_property->qual_info[3].qual_offset = 16;
        sw_db_property->qual_info[4].qual_type = SOC_PPC_FP_QUAL_HDR_DA;
        sw_db_property->qual_info[4].qual_nof_bits = 16;
        sw_db_property->qual_info[4].qual_offset = 0;
        sw_db_property->qual_info[5].qual_type = SOC_PPC_FP_QUAL_DOUBLE_TAG_HDR_ETHERTYPE;
        break;
    default:
        sw_db_property->nof_qulifiers = -1;
        break;
    }

}

void arad_pp_dbal_sw_db_tst1_explicit_null_tcam_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 unused)
{

    sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_INITIAL_VID;

    if(SOC_IS_JERICHO(unit))
    {
        sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_MPLS_KEY0;
        sw_db_property->qual_info[1].qual_nof_bits = 20;
        sw_db_property->nof_qulifiers = 2;
    }
    else
    {
        sw_db_property->qual_info[1].qual_type     = SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1;
        sw_db_property->qual_info[1].qual_nof_bits = 4;
        sw_db_property->qual_info[1].qual_offset   = 16;
        sw_db_property->qual_info[2].qual_type     = SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1;
        sw_db_property->qual_info[2].qual_nof_bits = 16;

        sw_db_property->nof_qulifiers = 3;
    }
}

void arad_pp_dbal_sw_db_qinq_compressed_sem_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 tpid)
{

    sw_db_property->nof_qulifiers = 3;

    sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_INITIAL_VID;
    sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_VLAN_DOMAIN;
    sw_db_property->qual_info[2].qual_type = SOC_PPC_FP_QUAL_CMPRSD_INNER_VID;

    if(tpid == TPID1)
        sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_QINQ_COMPRESSED_TPID1_PREFIX;
    else
        sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_QINQ_COMPRESSED_TPID2_PREFIX;

}

void arad_pp_dbal_sw_db_qinany_sem_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 tpid)
{
    sw_db_property->nof_qulifiers = 2;

    sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_INITIAL_VID;
    sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_VLAN_DOMAIN;

    if(tpid == TPID1)
        sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_QINANY_TPID1_PREFIX;
    else
        sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_QINANY_TPID2_PREFIX;

}
void arad_pp_dbal_sw_db_qinany_pcp_sem_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 tpid)
{
    sw_db_property->nof_qulifiers = 3;

    sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_INITIAL_VID;
    sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_VLAN_DOMAIN;
    sw_db_property->qual_info[2].qual_type = SOC_PPC_FP_QUAL_HDR_VLAN_TAG_PRI_CFI;

    if(tpid == TPID1)
        sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_QINANY_PCP_TPID1_PREFIX;
    else
        sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_QINANY_PCP_TPID2_PREFIX;
}

void arad_pp_dbal_sw_db_1q_sem_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 tpid)
{
    sw_db_property->nof_qulifiers = 2;

    sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_INITIAL_VID;
    sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_VLAN_DOMAIN;

    if(tpid == TPID1)
        sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_1Q_TPID1_PREFIX;
    else
        sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_1Q_TPID2_PREFIX;

}
void arad_pp_dbal_sw_db_1q_compressed_sem_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 tpid)
{
    sw_db_property->nof_qulifiers = 2;

    sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_VLAN_DOMAIN;
    sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID;

    if(tpid == TPID1)
        sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_1Q_COMPRESSED_TPID1_PREFIX;
    else
        sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_1Q_COMPRESSED_TPID2_PREFIX;
}
void arad_pp_dbal_sw_db_1q_pcp_compressed_sem_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 tpid)
{
    sw_db_property->nof_qulifiers = 3;

    sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_VLAN_DOMAIN;
    sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID;
    sw_db_property->qual_info[2].qual_type = SOC_PPC_FP_QUAL_HDR_VLAN_TAG_PRI_CFI;

    if(tpid == TPID1)
        sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_1Q_COMPRESSED_PCP_TPID1_PREFIX;
    else
        sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_1Q_COMPRESSED_PCP_TPID2_PREFIX;
}

void arad_pp_dbal_sw_db_untagged_sem_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 unused)
{

    sw_db_property->nof_qulifiers = 2;

    sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_INITIAL_VID;
    sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_VLAN_DOMAIN;

    sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_QINQ_COMPRESSED_TPID1_PREFIX;
}

void arad_pp_dbal_sw_db_test2_sem_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 unused)
{

    sw_db_property->nof_qulifiers = 2;

    sw_db_property->qual_info[0].qual_type    =  SOC_PPC_FP_QUAL_EID;
    sw_db_property->qual_info[1].qual_type    =  SOC_PPC_FP_QUAL_HDR_VLAN_TAG_ID;

    sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_TEST2_VT_ISA_PREFIX;
}


void arad_pp_dbal_sw_db_tst2_tcam_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 unused)
{
    sw_db_property->nof_qulifiers = 5;

    sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_INITIAL_VID;
    sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_VLAN_DOMAIN;
    sw_db_property->qual_info[2].qual_type = SOC_PPC_FP_QUAL_HDR_VLAN_TAG_PRI_CFI;
    sw_db_property->qual_info[3].qual_type = SOC_PPC_FP_QUAL_HDR_VLAN_TAG_TPID;
    sw_db_property->qual_info[4].qual_type = SOC_PPC_FP_QUAL_HDR_2ND_VLAN_TAG_ID;

    sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_TCAM_TST2_PROFILE;

}

void arad_pp_dbal_sw_db_test2_tcam_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 unused)
{
    sw_db_property->nof_qulifiers = 3;

    sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_IRPP_SRC_PP_PORT;

    sw_db_property->qual_info[1].qual_nof_bits = 32;
    sw_db_property->qual_info[1].qual_offset   = 16;
    sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_DA; 

    sw_db_property->qual_info[2].qual_nof_bits = 16;
    sw_db_property->qual_info[2].qual_offset   = 0;
    sw_db_property->qual_info[2].qual_type = SOC_PPC_FP_QUAL_HDR_DA;   

}

void arad_pp_dbal_sw_db_src_port_da_tcam_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 vid_type)
{
    sw_db_property->nof_qulifiers = 4;

    if (vid_type == INITIAL_VID) {
        sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_INITIAL_VID;
    } else {
        sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID;
    }
    
    sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_IRPP_SRC_PP_PORT;

    sw_db_property->qual_info[2].qual_nof_bits = 32;
    sw_db_property->qual_info[2].qual_offset   = 16;
    sw_db_property->qual_info[2].qual_type = SOC_PPC_FP_QUAL_HDR_DA;  

    sw_db_property->qual_info[3].qual_nof_bits = 16;
    sw_db_property->qual_info[3].qual_offset   = 0;
    sw_db_property->qual_info[3].qual_type = SOC_PPC_FP_QUAL_HDR_DA;  

    sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_TCAM_SRC_PORT_DA_DB_PROFILE;
}

void arad_pp_dbal_sw_db_gre_port_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 unused) 
{
        sw_db_property->nof_qulifiers = 2;
        sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ONES; 
        sw_db_property->qual_info[0].qual_nof_bits = 32;
        sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_IPV4_NEXT_PRTCL;  
        sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_IP_PREFIX;
}


void arad_pp_dbal_sw_db_dip_sip_vrf_tcam_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 unused) 
{
    sw_db_property->nof_qulifiers = 7;

    sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_IPV4_DIP; 
    sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_IPV4_SIP; 
    sw_db_property->qual_info[2].qual_type = SOC_PPC_FP_QUAL_IN_RIF_VALID_VRF; 
    sw_db_property->qual_info[3].qual_type = SOC_PPC_FP_QUAL_HDR_IPV4_NEXT_PRTCL;  
    sw_db_property->qual_info[4].qual_type = SOC_PPC_FP_QUAL_GRE_PROTOCOL_TYPE;     
    
    sw_db_property->qual_info[5].qual_type = SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER2_GRE_PARSED;  
    sw_db_property->qual_info[6].qual_type = SOC_PPC_FP_QUAL_IRPP_IN_PORT_KEY_GEN_VAR;    
    sw_db_property->qual_info[6].qual_nof_bits = 4;   
}


void arad_pp_dbal_sw_db_extender_untag_check_sem_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 unused)
{
    sw_db_property->nof_qulifiers = 1;

    sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_PORT_EXTENDER_ETAG;

    sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_EXTENDER_UNTAG_CHECK_PREFIX;
}


void arad_pp_dbal_sw_db_extender_pe_sem_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 vid_type)
{
    sw_db_property->nof_qulifiers = 4;

    sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_VLAN_DOMAIN;
    sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_PORT_EXTENDER_ECID;

    if (vid_type == INITIAL_VID) {
        sw_db_property->qual_info[2].qual_type = SOC_PPC_FP_QUAL_INITIAL_VID;
        sw_db_property->qual_info[3].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;

    } else {
        sw_db_property->qual_info[2].qual_type = SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID;
        sw_db_property->qual_info[3].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ONES;
    }

    sw_db_property->qual_info[3].qual_nof_bits = 1;
    sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_EXTENDER_PE_PREFIX;
}

void arad_pp_dbal_sw_db_extender_channel_reg_sem_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 unused)
{
    sw_db_property->nof_qulifiers = 2;

    sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_VLAN_DOMAIN;
    sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_PORT_EXTENDER_ECID;

    sw_db_property->prefix = DBAL_PREFIX_NOT_DEFINED;		
}

void arad_pp_dbal_sw_db_my_vtep_index_sip_vrf_sem_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 unused)
{

    
    sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_ISB_FOUND_IN_LIF_IDX; 
    sw_db_property->qual_info[0].qual_nof_bits = (SOC_DPP_CONFIG(unit))->pp.vxlan_tunnel_term_in_sem_my_vtep_index_nof_bits + 1; 
    sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_IPV4_SIP; 
    sw_db_property->qual_info[2].qual_type = SOC_PPC_FP_QUAL_IN_RIF_VALID_VRF; 
    sw_db_property->qual_info[2].qual_nof_bits = (SOC_DPP_CONFIG(unit))->pp.vxlan_tunnel_term_in_sem_vrf_nof_bits; 
    sw_db_property->nof_qulifiers = 3; 

    sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_VTEP_INDEX_SIP_VRF_MY_PREFIX >> 4; 
    sw_db_property->prefix_len = 2; 
    
}

void arad_pp_dbal_sw_db_custom_pp_port_tunnel_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 unused)
{
    sw_db_property->nof_qulifiers = 2;

    sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_VLAN_DOMAIN;
    sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_CUSTOM_PP_HEADER_OUTPUT_FP;

    sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_CUSTOM_PP_PORT_TUNNEL_PREFIX;
}

void arad_pp_dbal_sw_db_custom_pp_port_lsp_ecmp_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 unused)
{
    sw_db_property->nof_qulifiers = 3;

    sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_CUSTOM_PP_HEADER_OUTPUT_LSP_ECMP_FP;
    sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_CUSTOM_PP_HEADER_OUTPUT_FP;
    sw_db_property->qual_info[2].qual_type = SOC_PPC_FP_QUAL_VLAN_DOMAIN;

    sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_TCAM_CUSTOM_PP_LSP_ECMP_PROFILE;
}

void arad_pp_dbal_sw_db_vlan_domain_mpls_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 mpls_label)
{
    if (SOC_IS_JERICHO(unit)) {
        sw_db_property->nof_qulifiers = 2;
        
        sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_VLAN_DOMAIN;
        sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_MPLS_KEY0 + mpls_label*(SOC_PPC_FP_QUAL_MPLS_KEY1 - SOC_PPC_FP_QUAL_MPLS_KEY0);
        sw_db_property->qual_info[1].qual_nof_bits = 20;

    } else {
        sw_db_property->nof_qulifiers = 3;
        
        sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_VLAN_DOMAIN;
        sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1 + mpls_label*(SOC_PPC_FP_QUAL_HDR_MPLS_LABEL2 - SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1);
        sw_db_property->qual_info[1].qual_nof_bits = 4;
        sw_db_property->qual_info[1].qual_offset    = 16;
        sw_db_property->qual_info[2].qual_type = SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1 + mpls_label*(SOC_PPC_FP_QUAL_HDR_MPLS_LABEL2- SOC_PPC_FP_QUAL_HDR_MPLS_LABEL1);;
        sw_db_property->qual_info[2].qual_nof_bits = 16;

    }

    sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_VLAN_DOMAIN_MPLS_PREFIX;
	
}

void arad_pp_dbal_sw_db_oam_stat_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 fwrd_key)
{
    sw_db_property->nof_qulifiers = 1;

    if(fwrd_key == OAM_FWD_CODE_0)
    {
        sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_OAM_ETHERTYPE;
    }
    else if(fwrd_key == OAM_FWD_CODE_1)
    {
        sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_OAM_2ND_ETHERTYPE;
    }

    sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_OAM_STAT_TT_PREFIX;
}

void arad_pp_dbal_sw_db_bfd_stat_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 fwrd_key)
{
    sw_db_property->nof_qulifiers = 1;

    if(fwrd_key == BFD_FWD_CODE_0)
    {
        sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_BFD_PPH_FWD_CODE;
        sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_BFD_STAT_TT_ISA_PREFIX;
    }
    else if(fwrd_key == BFD_FWD_CODE_1)
    {
        sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_BFD_1ST_NIBBLE_AFTER_LABEL;
        sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_BFD_STAT_TT_ISB_PREFIX;
    }
}

void arad_pp_dbal_sw_db_tm_stat_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 stat_type)
{
    sw_db_property->nof_qulifiers = 2;

    sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_HDR_PTCH2_OPAQUE;
    sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_HDR_ITMH_PPH_TYPE;

    if(stat_type == IS_OAM_STAT)
    {
        sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_OAM_STAT_VT_PREFIX;
    }
    else if(stat_type == IS_BFD_STAT)
    {
        sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_BFD_STAT_VT_PREFIX;
    }
}

void arad_pp_dbal_sw_db_fc_with_vft_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 vsan_source)
{
    if(vsan_source == VSAN_FROM_VFT)
    {
        sw_db_property->nof_qulifiers = 2;
        sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_FC_WITH_VFT_VFT_ID;
        sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_FC_WITH_VFT_D_ID;
    }

    if(vsan_source == VSAN_FROM_VSI)
    {
       sw_db_property->nof_qulifiers = 3;
       sw_db_property->qual_info[2].qual_type = SOC_PPC_FP_QUAL_INITIAL_VSI;
       sw_db_property->qual_info[2].qual_nof_bits = 12;
       sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
       sw_db_property->qual_info[1].qual_nof_bits = 1;
       sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_FC_WITH_VFT_D_ID;
    }

    sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_FC_PREFIX;
}


void arad_pp_dbal_sw_db_fc_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 vsan_source)
{
    if(vsan_source == VSAN_FROM_VFT)
    {
        sw_db_property->nof_qulifiers = 2;
        sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_IRPP_IN_PORT_KEY_GEN_VAR;
        sw_db_property->qual_info[1].qual_nof_bits = 13;
        sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_FC_D_ID;
    }

    if(vsan_source == VSAN_FROM_VSI)
    {
       sw_db_property->nof_qulifiers = 3;
       sw_db_property->qual_info[2].qual_type = SOC_PPC_FP_QUAL_INITIAL_VSI;
       sw_db_property->qual_info[2].qual_nof_bits = 12;
       sw_db_property->qual_info[1].qual_type = SOC_PPC_FP_QUAL_IRPP_ALL_ZEROES;
       sw_db_property->qual_info[1].qual_nof_bits = 1;
       sw_db_property->qual_info[0].qual_type = SOC_PPC_FP_QUAL_FC_D_ID;
    }

    sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_FC_PREFIX;
}


void arad_pp_dbal_sw_db_pon_vdxtunnelxvid_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 key_type)
{
    sw_db_property->qual_info[0].qual_type     = SOC_PPC_FP_QUAL_VLAN_DOMAIN ;
    if (!soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "gpon_application_enable", 0)) {
        sw_db_property->qual_info[0].qual_nof_bits = (_BCM_PPD_PON_PP_PORT_MAPPING_BY_PASS_IN_JER(unit) ? 6 : (SOC_IS_JERICHO(unit) ? 4 : 3));
    } else {
        sw_db_property->qual_info[0].qual_nof_bits = 7;
    }
    sw_db_property->qual_info[1].qual_type     = SOC_PPC_FP_QUAL_TUNNEL_ID;


    switch(key_type)
    {
        case PON_TUNNEL_SEMA:
            sw_db_property->nof_qulifiers = 2;
            sw_db_property->prefix        = (!g_prog_soc_prop[unit].tls_db_enable) ? ARAD_PP_ISEM_ACCESS_NULL_PREFIX : ARAD_PP_ISEM_ACCESS_PON_UNTAGGED_PREFIX;
            break;
        case PON_TUNNEL_SEMB:
            sw_db_property->nof_qulifiers = 2;
            sw_db_property->prefix        = (!g_prog_soc_prop[unit].custom_pon_enable) ? ARAD_PP_ISEM_ACCESS_PON_TLS_PREFIX : ARAD_PP_ISEM_ACCESS_NULL_PREFIX;
            break;
        case PON_TUNNEL_ETHTYPE:
            sw_db_property->nof_qulifiers           = 3;
            sw_db_property->qual_info[2].qual_type  = SOC_PPC_FP_QUAL_UNTAG_HDR_ETHERTYPE;
            sw_db_property->prefix                  = ARAD_PP_ISEM_ACCESS_TCAM_PON_EXTEND_LKP_DB_PROFILE;
            break;
        case PON_TUNNEL_OUTER_VID_SEMA:
            sw_db_property->nof_qulifiers         = 3;
            sw_db_property->qual_info[2].qual_type = SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID;
            sw_db_property->prefix                = ARAD_PP_ISEM_ACCESS_PON_ONE_TAG_PREFIX;
            break;
        case PON_TUNNEL_OUTER_VID_SEMB:
            sw_db_property->nof_qulifiers              = 3;
            sw_db_property->qual_info[2].qual_type     = SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID;
            sw_db_property->prefix                     = ARAD_PP_ISEM_ACCESS_PON_ONE_TAG_PREFIX;
            break;
        case PON_TUNNEL_OUTER_INNER_VID_SEMA:
            sw_db_property->nof_qulifiers = 4;
            sw_db_property->qual_info[2].qual_type     = SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID;
            sw_db_property->qual_info[3].qual_type     = SOC_PPC_FP_QUAL_CMPRSD_INNER_VID;
            sw_db_property->prefix = (SOC_IS_ARADPLUS_AND_BELOW(unit) ? (ARAD_PP_ISEM_ACCESS_PON_TWO_TAGS_PREFIX>>2) : ARAD_PP_ISEM_ACCESS_PON_TWO_TAGS_PREFIX);
            sw_db_property->prefix_len = (SOC_IS_ARADPLUS_AND_BELOW(unit) ? 2 : ARAD_PP_ISEM_ACCESS_NOF_BITS_PREFIX);
            break;
        case PON_TUNNEL_OUTER_INNER_VID_SEMB:
            sw_db_property->nof_qulifiers = 4;
            sw_db_property->qual_info[2].qual_type     = SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID;
            sw_db_property->qual_info[3].qual_type     = SOC_PPC_FP_QUAL_CMPRSD_INNER_VID;
            sw_db_property->prefix = (SOC_IS_ARADPLUS_AND_BELOW(unit) ? (ARAD_PP_ISEM_ACCESS_CUS_PON_TWO_TAGS_PREFIX>>2) : ARAD_PP_ISEM_ACCESS_CUS_PON_TWO_TAGS_PREFIX);
            sw_db_property->prefix_len = (SOC_IS_ARADPLUS_AND_BELOW(unit) ? 2 : ARAD_PP_ISEM_ACCESS_NOF_BITS_PREFIX);
            break;
        case PON_TUNNEL_ETHTYPE_PCP_OUTER_VID:
            sw_db_property->nof_qulifiers = 5;
            sw_db_property->qual_info[2].qual_type     = SOC_PPC_FP_QUAL_ONE_TAG_HDR_ETHERTYPE;
            sw_db_property->qual_info[3].qual_type     = SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID;
            sw_db_property->qual_info[4].qual_type     = SOC_PPC_FP_QUAL_HDR_VLAN_TAG_PRI_CFI;
            sw_db_property->prefix                     = ARAD_PP_ISEM_ACCESS_TCAM_PON_EXTEND_LKP_DB_PROFILE;
            break;
        case PON_TUNNEL_ETHTYPE_PCP_OUTER_INNER_VID:
            sw_db_property->nof_qulifiers = 6;
            sw_db_property->qual_info[2].qual_type     = SOC_PPC_FP_QUAL_DOUBLE_TAG_HDR_ETHERTYPE;
            sw_db_property->qual_info[3].qual_type     = SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID;
            sw_db_property->qual_info[4].qual_type     = SOC_PPC_FP_QUAL_HDR_VLAN_TAG_PRI_CFI;
            sw_db_property->qual_info[5].qual_type     = SOC_PPC_FP_QUAL_CMPRSD_INNER_VID;
            sw_db_property->prefix                     = ARAD_PP_ISEM_ACCESS_TCAM_PON_EXTEND_LKP_DB_PROFILE;
            break;
        default:
            sw_db_property->nof_qulifiers = -1;
            break;
    }

    if (_BCM_PPD_PON_PP_PORT_MAPPING_BY_PASS_IN_JER(unit)) {
        sw_db_property->prefix_len = 4;
    }
}


void arad_pp_dbal_sw_db_section_oam_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 key_type)
{
    switch(key_type)
    {
        case SECTION_OAM_PORT_OUTER_VID_ETH:
            sw_db_property->nof_qulifiers = 3;
            sw_db_property->qual_info[0].qual_type     = SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID;
            sw_db_property->qual_info[1].qual_type     = SOC_PPC_FP_QUAL_ONE_TAG_HDR_ETHERTYPE;
            sw_db_property->qual_info[2].qual_type     = SOC_PPC_FP_QUAL_VLAN_DOMAIN;
            sw_db_property->prefix                     = ARAD_PP_ISEM_ACCESS_TCAM_SECTION_OAM_PROFILE;
            break;
        case SECTION_OAM_PORT_OUTER_INNER_VID_ETH:
            sw_db_property->nof_qulifiers = 4;
            sw_db_property->qual_info[0].qual_type     = SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID;
            sw_db_property->qual_info[1].qual_type     = SOC_PPC_FP_QUAL_CMPRSD_INNER_VID;
            sw_db_property->qual_info[2].qual_type     = SOC_PPC_FP_QUAL_DOUBLE_TAG_HDR_ETHERTYPE;
            sw_db_property->qual_info[3].qual_type     = SOC_PPC_FP_QUAL_VLAN_DOMAIN;
            sw_db_property->prefix                     = ARAD_PP_ISEM_ACCESS_TCAM_SECTION_OAM_DOU_TAGS_PROFILE;
            break;

        default:
            sw_db_property->nof_qulifiers = -1;
            break;
    }

}




void arad_pp_dbal_sw_db_ipv6_dip_np_et_tcam_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 vid_type)
{
    int qual_num = 0;
    
    if(vid_type == INITIAL_VID) {
        sw_db_property->qual_info[qual_num].qual_type    = SOC_PPC_FP_QUAL_INITIAL_VID;
    } else {
        sw_db_property->qual_info[qual_num].qual_type    = SOC_PPC_FP_QUAL_CMPRSD_OUTER_VID;
    }    
    qual_num++;
    sw_db_property->qual_info[qual_num].qual_type     = SOC_PPC_FP_QUAL_HDR_IPV6_DIP_LOW;
    sw_db_property->qual_info[qual_num].qual_offset   = 32;
    sw_db_property->qual_info[qual_num].qual_nof_bits = 32;
    qual_num++;
    sw_db_property->qual_info[qual_num].qual_type     = SOC_PPC_FP_QUAL_HDR_IPV6_DIP_LOW;
    sw_db_property->qual_info[qual_num].qual_offset   = 16;   
    sw_db_property->qual_info[qual_num].qual_nof_bits = 16;
    qual_num++;
    sw_db_property->qual_info[qual_num].qual_type     = SOC_PPC_FP_QUAL_HDR_IPV6_DIP_LOW;
    sw_db_property->qual_info[qual_num].qual_offset   = 0;   
    sw_db_property->qual_info[qual_num].qual_nof_bits = 16;
    qual_num++;
    sw_db_property->qual_info[qual_num].qual_type     = SOC_PPC_FP_QUAL_HDR_IPV6_DIP_HIGH;
    sw_db_property->qual_info[qual_num].qual_offset   = 32;
    sw_db_property->qual_info[qual_num].qual_nof_bits = 32;
    qual_num++;
    sw_db_property->qual_info[qual_num].qual_type     = SOC_PPC_FP_QUAL_HDR_IPV6_DIP_HIGH;
    sw_db_property->qual_info[qual_num].qual_offset   = 0;
    sw_db_property->qual_info[qual_num].qual_nof_bits = 32;
    qual_num++;
    sw_db_property->qual_info[qual_num].qual_type     = SOC_PPC_FP_QUAL_HDR_IPV6_NEXT_PRTCL;  
    qual_num++;
    sw_db_property->qual_info[qual_num].qual_type     = SOC_PPC_FP_QUAL_PACKET_FORMAT_QUALIFIER1;     
    sw_db_property->qual_info[qual_num].qual_offset   = 4;
    sw_db_property->qual_info[qual_num].qual_nof_bits = 7;
    qual_num++;
    sw_db_property->nof_qulifiers = qual_num;
}



void arad_pp_dbal_sw_db_ipv6_sip_vrf_comp_dest_tcam_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 key_type)
{
    int qual_num = 0;
    
    sw_db_property->qual_info[qual_num].qual_type     = SOC_PPC_FP_QUAL_HDR_IPV6_SIP_LOW;
    sw_db_property->qual_info[qual_num].qual_offset   = 32;
    sw_db_property->qual_info[qual_num].qual_nof_bits = 32;
    qual_num++;
    sw_db_property->qual_info[qual_num].qual_type     = SOC_PPC_FP_QUAL_HDR_IPV6_SIP_LOW;
    sw_db_property->qual_info[qual_num].qual_offset   = 16; 
    sw_db_property->qual_info[qual_num].qual_nof_bits = 16;
    qual_num++;
    sw_db_property->qual_info[qual_num].qual_type     = SOC_PPC_FP_QUAL_HDR_IPV6_SIP_LOW;
    sw_db_property->qual_info[qual_num].qual_offset   = 0; 
    sw_db_property->qual_info[qual_num].qual_nof_bits = 16;
    qual_num++;
    sw_db_property->qual_info[qual_num].qual_type     = SOC_PPC_FP_QUAL_HDR_IPV6_SIP_HIGH;
    sw_db_property->qual_info[qual_num].qual_offset   = 32;
    sw_db_property->qual_info[qual_num].qual_nof_bits = 32;
    qual_num++;
    sw_db_property->qual_info[qual_num].qual_type     = SOC_PPC_FP_QUAL_HDR_IPV6_SIP_HIGH;
    sw_db_property->qual_info[qual_num].qual_nof_bits = 32;
    qual_num++;
    sw_db_property->qual_info[qual_num].qual_type     = SOC_PPC_FP_QUAL_IN_RIF_VALID_VRF;  
    sw_db_property->qual_info[qual_num].qual_nof_bits = 14;
    qual_num++;
    sw_db_property->qual_info[qual_num].qual_type     = SOC_PPC_FP_QUAL_VT_TCAM_MATCH_IN_LIF_IDX; 
    sw_db_property->qual_info[qual_num].qual_nof_bits = 7;  
    qual_num++;
    sw_db_property->qual_info[qual_num].qual_type     = SOC_PPC_FP_QUAL_IRPP_IN_PORT_KEY_GEN_VAR;    
    sw_db_property->qual_info[qual_num].qual_nof_bits = 4;   
    qual_num++;
    sw_db_property->nof_qulifiers = qual_num;
}




void arad_pp_dbal_sw_db_generalized_rch_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 is_rch0)
{
    if(is_rch0 == 1) {
        sw_db_property->qual_info[0].qual_type     = SOC_PPC_FP_QUAL_HDR_SA;
        sw_db_property->qual_info[0].qual_offset   = 33;
        sw_db_property->qual_info[0].qual_nof_bits = 15;
    } else {
        sw_db_property->qual_info[0].qual_type     = SOC_PPC_FP_QUAL_HDR_SA;
        
        sw_db_property->qual_info[0].qual_offset   = 10;
        sw_db_property->qual_info[0].qual_nof_bits = 14;
    }   

    sw_db_property->nof_qulifiers = 1;
}


void arad_pp_dbal_sw_db_pppoe_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 unused)
{
	sw_db_property->nof_qulifiers = 2;
	sw_db_property->qual_info[0].qual_type	   = SOC_PPC_FP_QUAL_IRPP_IN_LIF; 
	sw_db_property->qual_info[0].qual_nof_bits = 16;
	sw_db_property->qual_info[1].qual_type	   = SOC_PPC_FP_QUAL_HDR_PPPOE_SESSION_ID; 
	sw_db_property->qual_info[1].qual_nof_bits = 16;

    sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_PPPOE_PREFIX;
}


void arad_pp_dbal_sw_db_l2tp_property_set(int unit, ARAD_PP_DBAL_VTT_SW_DB_PROPERTY* sw_db_property, uint32 unused)
{
	sw_db_property->nof_qulifiers = 2;
	sw_db_property->qual_info[0].qual_type	   = SOC_PPC_FP_QUAL_L2TP_TUNNEL_SESSION; 
	sw_db_property->qual_info[0].qual_nof_bits = 32;
	
	sw_db_property->qual_info[1].qual_type	   = SOC_PPC_FP_QUAL_IRPP_VRF; 
	sw_db_property->qual_info[1].qual_offset   = 0;
	sw_db_property->qual_info[1].qual_nof_bits = 12;
	
    sw_db_property->prefix = ARAD_PP_ISEM_ACCESS_L2TP_PREFIX;
}









uint32  arad_pp_dbal_vtt_program_set(int unit){

    int i;
    int res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    for(i=0; i<SOC_DPP_NOF_VTT_SW_DB_TABLES; i++)
    {
        vtt_tables[i].valid_indication[unit] = 0;
    }

    
    res = arad_pp_dbal_vt_program_set(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    
    res = arad_pp_dbal_tt_program_set(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_dbal_vtt_program_set()", 0, 0);
}


uint32  arad_pp_dbal_vt_program_set(int unit)
{
    ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY program_property;
    ARAD_PP_DBAL_VTT_SW_DB_PROPERTY   sw_db_property;
    SOC_DPP_DBAL_KEY_TO_TABLE         keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] = {{0}};

    int sw_db_index;
    int res;
    int i,j;
    int ati_for_dbal = SOC_DPP_DBAL_ATI_NONE;
    int tcam_db_id = SOC_DPP_DBAL_ATI_NONE;
    int nof_keys_to_create;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    for(i=0; i<ARAD_PP_DBAL_VT_NOF_PROGRAMS; i++)
    {
        
        if(vt_dbal_programs[i].prog_used[unit] != -1)
        {
            
            PROGRAM_PROPERTY_CLEAR(&program_property);
            
            (vt_dbal_programs[i].vtt_program_property_set)(unit, &program_property, vt_dbal_programs[i].additional_info);

            if (program_property.tcam_profile == ARAD_PP_TCAM_DYNAMIC_ACCESS_ID_BASE) {
                
                ati_for_dbal = SOC_DPP_DBAL_ATI_NONE;
                tcam_db_id = 0;                
            }else if(program_property.tcam_profile != ARAD_TCAM_ACCESS_PROFILE_INVALID)
            {                
                ati_for_dbal = SOC_DPP_DBAL_ATI_TCAM_STATIC;
                tcam_db_id = program_property.tcam_profile;                                 
            }else{
                
                ati_for_dbal = SOC_DPP_DBAL_ATI_TCAM_DUMMY;
            }

            if(program_property.nof_dbs == 0)
            {
               nof_keys_to_create = program_property.nof_keys;
            }
            else
            {
               nof_keys_to_create = program_property.nof_dbs;
            }

            
            for(j=0; j < nof_keys_to_create; j++)
            {
                
                sw_db_index = program_property.vtt_table_index[j];

                
                if(!(vtt_tables[sw_db_index].valid_indication[unit]))
                {
                    
                    SW_DB_PROPERTY_CLEAR(&sw_db_property);
                    sw_db_property.prefix_len = ARAD_PP_ISEM_ACCESS_NOF_BITS_PREFIX; 

                    
                    vtt_tables[sw_db_index].valid_indication[unit] = 1;
                    
                    vtt_tables[sw_db_index].vtt_table_property_set(unit, &sw_db_property,vtt_tables[sw_db_index].table_additional_info);

                    if (vtt_tables[sw_db_index].mem_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM) {                        
                        if(ati_for_dbal == SOC_DPP_DBAL_ATI_TCAM_STATIC) {
                            sw_db_property.prefix = tcam_db_id;
                        }
                    }

                    
                    res = arad_pp_dbal_table_create(unit, vtt_tables[sw_db_index].dbal_sw_db_id,
                                                          sw_db_property.prefix,
                                                          sw_db_property.prefix_len,
                                                          vtt_tables[sw_db_index].mem_type,
                                                          sw_db_property.nof_qulifiers,
                                                          ati_for_dbal,
                                                          sw_db_property.qual_info,
                                                          vtt_tables[sw_db_index].name);
                   SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);
                }

                
                keys_to_table_id[j].key_id      = MEM_TYPE_TO_KEY(vtt_tables[sw_db_index].mem_type);
                keys_to_table_id[j].sw_table_id = vtt_tables[sw_db_index].dbal_sw_db_id;
                keys_to_table_id[j].lookup_number = 0;
                keys_to_table_id[j].public_lpm_lookup_size = 0;
            }

            if(program_property.nof_keys != 0)
            {
                if(program_property.implicit_flag)
                {
                    
                    res = arad_pp_dbal_program_to_tables_associate_implicit(unit,
                                                                            vt_dbal_programs[i].prog_used[unit],
                                                                            SOC_PPC_FP_DATABASE_STAGE_INGRESS_VT,
                                                                            keys_to_table_id,
                                                                            program_property.qual_to_ce_info.qualifier_to_ce_id,
                                                                            program_property.nof_keys);
                }
                else
                {
                    
                    res = arad_pp_dbal_program_to_tables_associate(unit,
                                                                   vt_dbal_programs[i].prog_used[unit],
                                                                   SOC_PPC_FP_DATABASE_STAGE_INGRESS_VT,
                                                                   keys_to_table_id,
                                                                   program_property.qual_to_ce_info.use_32_bit_ce,
                                                                   program_property.nof_keys);
                }
                SOC_SAND_CHECK_FUNC_RESULT(res, 200 + i, exit);
            }

            
            res = arad_pp_dbal_vt_program_property_set(unit, &program_property, vt_dbal_programs[i].prog_used[unit]);
            SOC_SAND_CHECK_FUNC_RESULT(res, 300 + i, exit);

        }

    }
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_dbal_vt_program_set()", 0, 0);
}



uint32 arad_pp_dbal_vt_program_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 program )
{
    uint32 res;
    ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION_TBL_DATA ihp_vtt1st_key_construction_tbl_data;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    ARAD_CLEAR(&ihp_vtt1st_key_construction_tbl_data, ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION_TBL_DATA, 1);

    ihp_vtt1st_key_construction_tbl_data.dbal = 1;

    res = arad_pp_ihp_vtt1st_key_construction_tbl_get_unsafe(unit, program, &ihp_vtt1st_key_construction_tbl_data );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    ihp_vtt1st_key_construction_tbl_data.processing_profile           = program_property->processing_profile;
    ihp_vtt1st_key_construction_tbl_data.key_program_variable         = program_property->key_program_variable;

#ifdef BCM_88660_A0
    
    if (program_property->is_use_strength)
    {
        ihp_vtt1st_key_construction_tbl_data.vlan_translation_is_use_strength    = program_property->is_use_strength;
        ihp_vtt1st_key_construction_tbl_data.vlan_translation_0_pd_tcam_strength = program_property->pd_0_tcam_strength;
        ihp_vtt1st_key_construction_tbl_data.vlan_translation_0_pd_isa_strength  = program_property->pd_0_isa_strength;
        ihp_vtt1st_key_construction_tbl_data.vlan_translation_0_pd_isb_strength  = program_property->pd_0_isb_strength;
        ihp_vtt1st_key_construction_tbl_data.vlan_translation_1_pd_tcam_strength = program_property->pd_1_tcam_strength;
        ihp_vtt1st_key_construction_tbl_data.vlan_translation_1_pd_isa_strength  = program_property->pd_1_isa_strength;
        ihp_vtt1st_key_construction_tbl_data.vlan_translation_1_pd_isb_strength  = program_property->pd_1_isb_strength;
    }
    else
#endif
    {
        ihp_vtt1st_key_construction_tbl_data.vlan_translation_0_pd_bitmap = program_property->pd_bitmap_0;
        ihp_vtt1st_key_construction_tbl_data.vlan_translation_1_pd_bitmap = program_property->pd_bitmap_1;
    }

    ihp_vtt1st_key_construction_tbl_data.dbal = 1;
    res = arad_pp_ihp_vtt1st_key_construction_tbl_set_unsafe(unit, program, &ihp_vtt1st_key_construction_tbl_data );
    SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_dbal_vt_program_vt_vdxinitialvid_l1_set()", 0, 0);
}

void arad_pp_dbal_vt_program_tm_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 tm_stat)
{
    if (tm_stat == TM_STAT_OFF)
    {
        program_property->nof_keys = 0;
    }
    else if(tm_stat == TM_STAT_ON)
    {
        program_property->nof_keys = 2;
        program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_TM_STAT_SEM_A);
        program_property->vtt_table_index[1] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_TM_STAT_SEM_B);

        program_property->pd_bitmap_0   = (0x10);   
        program_property->pd_bitmap_1   = (0x8);    
    }

    program_property->tcam_profile  = ARAD_TCAM_ACCESS_PROFILE_INVALID;
}


void arad_pp_dbal_vt_program_vdxinitialvid_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
    int vdxinitial_vid_index=0;

    program_property->vtt_table_index[0]             = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxINITIALVID_SEM_A);
    program_property->pd_bitmap_0                    = (0x10);  
    program_property->nof_keys = 2;
    program_property->vtt_table_index[1]             = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_INITIAL_VID_TCAM);
    program_property->tcam_profile                   = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->processing_profile             = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_3MTSE_PLUS_FRR_NO_MPLS;

    if((g_prog_soc_prop[unit].mpls_1_database == ARAD_PP_ISEM_ACCESS_ID_ISEM_A)  ||
       (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 21) || (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 23))
    {
        arad_pp_dbal_vtt_program_property_swap( program_property );
        program_property->vtt_table_index[0]             = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxINITIALVID_SEM_B);
        vdxinitial_vid_index=1;
    }

    if(!SOC_IS_JERICHO(unit))
    {
        program_property->qual_to_ce_info.use_32_bit_ce[vdxinitial_vid_index][0] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[vdxinitial_vid_index][1] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[vdxinitial_vid_index][2] = 1;
    }

}


void arad_pp_dbal_vt_program_vdxinitialvid_l1_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
    int vdxinitial_vid_index=0;

    program_property->nof_keys = 3;
    program_property->vtt_table_index[0]             = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxINITIALVID_SEM_A);
    program_property->vtt_table_index[1]             = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_UNINDEXED_SEM_B);
    program_property->tcam_profile                   = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->vtt_table_index[2]             = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_INITIAL_VID_TCAM);
    program_property->pd_bitmap_0                    = (0x10);  
    program_property->pd_bitmap_1                    = (0x8);  

    if(g_prog_soc_prop[unit].mpls_1_database == ARAD_PP_ISEM_ACCESS_ID_ISEM_A)
    {
        arad_pp_dbal_vtt_program_property_swap( program_property );
        program_property->vtt_table_index[0]             = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_UNINDEXED_SEM_A);
        program_property->vtt_table_index[1]             = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxINITIALVID_SEM_B);
        vdxinitial_vid_index=1;
    }

    if(!SOC_IS_JERICHO(unit))
    {
        program_property->qual_to_ce_info.use_32_bit_ce[vdxinitial_vid_index][0] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[vdxinitial_vid_index][1] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[vdxinitial_vid_index][2] = 1;
    }

}


void arad_pp_dbal_vt_program_vdxoutervid_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused )
{
    int vdxouter_vid_index =0;

    program_property->vtt_table_index[0]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_A);
    program_property->pd_bitmap_0          = (0x10);
    program_property->nof_keys = 2;
    program_property->vtt_table_index[1]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_OUTER_VID_TCAM);
    program_property->tcam_profile         = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->key_program_variable = (0x2); 
    program_property->processing_profile   = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_3MTSE_PLUS_FRR_NO_MPLS;

    if((g_prog_soc_prop[unit].mpls_1_database == ARAD_PP_ISEM_ACCESS_ID_ISEM_A) ||
      (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 21) || (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 23))
    {
        arad_pp_dbal_vtt_program_property_swap( program_property );
        program_property->vtt_table_index[0]             = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_B);
        vdxouter_vid_index =1;
    }

    if(!SOC_IS_JERICHO(unit))
    {
        program_property->qual_to_ce_info.use_32_bit_ce[vdxouter_vid_index][0] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[vdxouter_vid_index][1] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[vdxouter_vid_index][2] = 1;
    }
}


void arad_pp_dbal_vt_program_vdxoutervid_l1_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused )
{
    int vdxouter_vid_index =0;

    program_property->nof_keys = 3;
    program_property->vtt_table_index[0]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_A);
    program_property->vtt_table_index[1]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_UNINDEXED_SEM_B);
    program_property->tcam_profile         = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->vtt_table_index[2]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_OUTER_VID_TCAM);
    program_property->pd_bitmap_0          = (0x10);  
    program_property->pd_bitmap_1          = (0x8);  

    program_property->key_program_variable = (0x2); 

    if(g_prog_soc_prop[unit].mpls_1_database == ARAD_PP_ISEM_ACCESS_ID_ISEM_A)
    {
        arad_pp_dbal_vtt_program_property_swap( program_property );
        program_property->vtt_table_index[0]             = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_UNINDEXED_SEM_A);
        program_property->vtt_table_index[1]             = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_B);
        vdxouter_vid_index =1;
    }

    if(!SOC_IS_JERICHO(unit))
    {
        program_property->qual_to_ce_info.use_32_bit_ce[vdxouter_vid_index][0] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[vdxouter_vid_index][1] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[vdxouter_vid_index][2] = 1;
    }
}


void arad_pp_dbal_vt_program_vdxouter_inner_vid_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused )
{
    int vdxouter_inner_vid_index =1;

    program_property->nof_keys = 2;
    program_property->vtt_table_index[0]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTER_INNER_VID_SEM_B);

    program_property->tcam_profile         = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->vtt_table_index[1]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_OUTER_VID_TCAM);
    program_property->pd_bitmap_0          = (0x8);  
    program_property->pd_bitmap_1          = (0x0);

    program_property->key_program_variable = (0x2); 

    if(((g_prog_soc_prop[unit].mpls_1_database == ARAD_PP_ISEM_ACCESS_ID_ISEM_A) && !((SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode >= 20) && (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode <= 23)))
    	|| (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 20))
    {
        arad_pp_dbal_vtt_program_property_swap( program_property );
        program_property->vtt_table_index[0]             = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTER_INNER_VID_SEM_A);
        vdxouter_inner_vid_index =0;
    }

    if(!SOC_IS_JERICHO(unit))
    {
        program_property->qual_to_ce_info.use_32_bit_ce[vdxouter_inner_vid_index][0] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[vdxouter_inner_vid_index][1] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[vdxouter_inner_vid_index][2] = 1;
    }

}

void arad_pp_dbal_vt_program_vdxouter_inner_vid_or_outervid_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused )
{
    program_property->nof_keys = 3;

    program_property->implicit_flag = 1;

    if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 22) {
    	program_property->vtt_table_index[0]             = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTER_INNER_VID_SEM_B);
    	program_property->vtt_table_index[1]             = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_A);
    }
    else {
    	program_property->vtt_table_index[0]             = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTER_INNER_VID_SEM_A);
    	program_property->vtt_table_index[1]             = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_B);
    }

    program_property->qual_to_ce_info.qualifier_to_ce_id[0][0] = SOC_IS_JERICHO(unit) ? 11 : 7;
    program_property->qual_to_ce_info.qualifier_to_ce_id[0][1] = SOC_IS_JERICHO(unit) ? 10 : 6;
    program_property->qual_to_ce_info.qualifier_to_ce_id[0][2] = 1;
    program_property->qual_to_ce_info.qualifier_to_ce_id[0][3] = 0;


    program_property->qual_to_ce_info.qualifier_to_ce_id[1][0] = SOC_IS_JERICHO(unit) ? 11 : 7;
    program_property->qual_to_ce_info.qualifier_to_ce_id[1][1] = SOC_IS_JERICHO(unit) ? 3  : 5;
    program_property->qual_to_ce_info.qualifier_to_ce_id[1][2] = 1;

    program_property->tcam_profile  = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->vtt_table_index[2]       = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_OUTER_VID_TCAM);
    program_property->qual_to_ce_info.qualifier_to_ce_id[2][0] = 1;

    if (SOC_IS_JERICHO(unit)) {
        program_property->is_use_strength = 1;
        if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 22) {
            program_property->pd_0_isa_strength = 1;
            program_property->pd_0_isb_strength = 2;
        } else {
            program_property->pd_0_isa_strength = 2;
            program_property->pd_0_isb_strength = 1;
        }
    } else {
        program_property->pd_bitmap_0              = (0x18); 
        program_property->pd_bitmap_1              = (0x0);
    }
    program_property->key_program_variable = (0x2); 

}


void arad_pp_dbal_vt_program_vid_l1frr_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 vid_type)
{
    program_property->nof_keys = 3;


    program_property->vtt_table_index[0]   = (vid_type == INITIAL_VID) ? SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxINITIALVID_SEM_A) : SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_A);
    program_property->vtt_table_index[1]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_UNINDEXED_SEM_B);
    program_property->vtt_table_index[2]   = (vid_type == INITIAL_VID) ? SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_FRR_INITIAL_VID_TCAM) : SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_FRR_OUTER_VID_TCAM);
    program_property->tcam_profile         = ARAD_PP_ISEM_ACCESS_TCAM_MPLS_FRR_DB_PROFILE;
    program_property->pd_bitmap_0          = (0x10);
    program_property->pd_bitmap_1          = (0x28);

    program_property->key_program_variable = (0x2); 

    if(g_prog_soc_prop[unit].mpls_1_database == ARAD_PP_ISEM_ACCESS_ID_ISEM_A)
    {
        arad_pp_dbal_vtt_program_property_swap( program_property );
        program_property->vtt_table_index[0] += 1;
        program_property->vtt_table_index[1] -= 1;
    }

    program_property->key_program_variable = (0x2); 

    if(!SOC_IS_JERICHO(unit))
    {
        program_property->implicit_flag = 1;

        program_property->qual_to_ce_info.qualifier_to_ce_id[0][0] = 2;
        program_property->qual_to_ce_info.qualifier_to_ce_id[0][1] = 1;
        program_property->qual_to_ce_info.qualifier_to_ce_id[0][2] = 0;

        if(g_prog_soc_prop[unit].is_bos_in_key_enable)
        {
            program_property->qual_to_ce_info.qualifier_to_ce_id[1][0] = 6;
            program_property->qual_to_ce_info.qualifier_to_ce_id[1][1] = 5;
            program_property->qual_to_ce_info.qualifier_to_ce_id[1][2] = 4;
        }
        else
        {
            program_property->qual_to_ce_info.qualifier_to_ce_id[1][0] = 5;
            program_property->qual_to_ce_info.qualifier_to_ce_id[1][1] = 4;

        }

        program_property->qual_to_ce_info.qualifier_to_ce_id[2][0] = 7;
        program_property->qual_to_ce_info.qualifier_to_ce_id[2][1] = 5;
        program_property->qual_to_ce_info.qualifier_to_ce_id[2][2] = 4;
        program_property->qual_to_ce_info.qualifier_to_ce_id[2][3] = 3;
    }
}

void arad_pp_dbal_vt_program_indexed_vid_l1frr_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 vid_type)
{
    program_property->nof_keys = 3;

    program_property->vtt_table_index[0]   = (vid_type == INITIAL_VID) ? SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxINITIALVID_SEM_A) : SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_A);
    program_property->vtt_table_index[1]   = SW_DB_DBAL2VTT(( g_prog_soc_prop[unit].is_bos_in_key_enable ? SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_BOS_L1IDX_SEM_B : SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L1IDX_SEM_B));
       program_property->vtt_table_index[1] += 2*(g_prog_soc_prop[unit].mpls_1_namespace);
    program_property->tcam_profile         = ARAD_PP_ISEM_ACCESS_TCAM_MPLS_FRR_DB_PROFILE;
    program_property->vtt_table_index[2]   = (vid_type == INITIAL_VID) ? SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_FRR_INITIAL_VID_TCAM) : SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_FRR_OUTER_VID_TCAM);
    program_property->pd_bitmap_0          = (0x10);
    program_property->pd_bitmap_1          = (0x28);

    program_property->key_program_variable = (0x2); 

    if(g_prog_soc_prop[unit].mpls_1_database == ARAD_PP_ISEM_ACCESS_ID_ISEM_A)
    {
        arad_pp_dbal_vtt_program_property_swap( program_property );
        program_property->vtt_table_index[0] += 1;
        program_property->vtt_table_index[1] -= 1;
    }

    if(!SOC_IS_JERICHO(unit))
    {
        program_property->implicit_flag = 1;

        program_property->qual_to_ce_info.qualifier_to_ce_id[0][0] = 2;
        program_property->qual_to_ce_info.qualifier_to_ce_id[0][1] = 1;
        program_property->qual_to_ce_info.qualifier_to_ce_id[0][2] = 0;

        if(g_prog_soc_prop[unit].is_bos_in_key_enable)
        {
            program_property->qual_to_ce_info.qualifier_to_ce_id[1][0] = 6;
            program_property->qual_to_ce_info.qualifier_to_ce_id[1][1] = 5;
            program_property->qual_to_ce_info.qualifier_to_ce_id[1][2] = 4;
        }
        else
        {
            program_property->qual_to_ce_info.qualifier_to_ce_id[1][0] = 5;
            program_property->qual_to_ce_info.qualifier_to_ce_id[1][1] = 4;
        }


        program_property->qual_to_ce_info.qualifier_to_ce_id[2][0] = 7;
        program_property->qual_to_ce_info.qualifier_to_ce_id[2][1] = 5;
        program_property->qual_to_ce_info.qualifier_to_ce_id[2][2] = 4;
        program_property->qual_to_ce_info.qualifier_to_ce_id[2][3] = 3;
    }
}

void arad_pp_dbal_vt_program_outer_pcp_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
    program_property->nof_keys = 3;

    program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_PCP_SEM_A);
    program_property->vtt_table_index[1] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_B);
    program_property->vtt_table_index[2] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_OUTER_VID_TCAM);

    program_property->key_program_variable = (0x2); 

    program_property->tcam_profile       = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->pd_bitmap_0        = (0x18); 
    program_property->pd_bitmap_1        = (0x0);
    if(!SOC_IS_JERICHO(unit))
    {
        program_property->qual_to_ce_info.use_32_bit_ce[0][0] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[0][1] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[0][2] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[2][0] = 1;
    }
}
void arad_pp_dbal_vt_program_outer_inner_pcp_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
    program_property->nof_keys = 2;

    program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTER_INNER_VID_SEM_A);
    program_property->vtt_table_index[1] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_INNER_OUTER_VID_PCP_TCAM);

    program_property->key_program_variable = (0x2); 

    program_property->tcam_profile       = ARAD_PP_ISEM_ACCESS_TCAM_ETH_INNER_OUTER_PCP_DB_PROFILE;
    program_property->pd_bitmap_0        = (0x30); 
    program_property->pd_bitmap_1        = (0x0);

    program_property->implicit_flag = 1;

    
    program_property->qual_to_ce_info.qualifier_to_ce_id[0][0] = 6;
    program_property->qual_to_ce_info.qualifier_to_ce_id[0][1] = 4;
    program_property->qual_to_ce_info.qualifier_to_ce_id[0][2] = 2;
    program_property->qual_to_ce_info.qualifier_to_ce_id[0][3] = 1;

    program_property->qual_to_ce_info.qualifier_to_ce_id[1][0] = 7;
    program_property->qual_to_ce_info.qualifier_to_ce_id[1][1] = 6;
    program_property->qual_to_ce_info.qualifier_to_ce_id[1][2] = 4;
    program_property->qual_to_ce_info.qualifier_to_ce_id[1][3] = 1;
    program_property->qual_to_ce_info.qualifier_to_ce_id[1][4] = 0;

}
void arad_pp_dbal_vt_program_outer_inner_pcp_or_outer_inner_or_outer_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused     )
{
    program_property->nof_keys = 3;

    program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTER_INNER_VID_SEM_A);
    program_property->vtt_table_index[1] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_B);
    program_property->vtt_table_index[2] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_INNER_OUTER_VID_PCP_TCAM);

    program_property->key_program_variable = (0x2); 

    program_property->tcam_profile       = ARAD_PP_ISEM_ACCESS_TCAM_ETH_INNER_OUTER_PCP_DB_PROFILE;
    program_property->pd_bitmap_0        = (0x38); 
    program_property->pd_bitmap_1        = (0x0);

    program_property->implicit_flag = 1;

    
    program_property->qual_to_ce_info.qualifier_to_ce_id[0][0] = 6;
    program_property->qual_to_ce_info.qualifier_to_ce_id[0][1] = 4;
    program_property->qual_to_ce_info.qualifier_to_ce_id[0][2] = 2;
    program_property->qual_to_ce_info.qualifier_to_ce_id[0][3] = 1;

    program_property->qual_to_ce_info.qualifier_to_ce_id[1][0] = 6;
    program_property->qual_to_ce_info.qualifier_to_ce_id[1][1] = 3;
    program_property->qual_to_ce_info.qualifier_to_ce_id[1][2] = 2;

    program_property->qual_to_ce_info.qualifier_to_ce_id[2][0] = 7;
    program_property->qual_to_ce_info.qualifier_to_ce_id[2][1] = 6;
    program_property->qual_to_ce_info.qualifier_to_ce_id[2][2] = 4;
    program_property->qual_to_ce_info.qualifier_to_ce_id[2][3] = 1;
    program_property->qual_to_ce_info.qualifier_to_ce_id[2][4] = 0;
}

void arad_pp_dbal_vt_program_vid_vid_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 vid_type)
{
    program_property->nof_keys = 3;

    if(vid_type == INITIAL_VID)
    {
        program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxINITIALVID_SEM_A);
        program_property->vtt_table_index[1] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxINITIALVID_SEM_B);
        program_property->vtt_table_index[2] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_INITIAL_VID_TCAM);
    }

    if(vid_type == OUTER_VID)
    {
        program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_A);
        program_property->vtt_table_index[1] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_B);
        program_property->vtt_table_index[2] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_OUTER_VID_TCAM);
    }

    if(vid_type == OUTER_INNER_VID)
    {
        program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTER_INNER_VID_SEM_A);
        program_property->vtt_table_index[1] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTER_INNER_VID_SEM_B);
        program_property->vtt_table_index[2] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_OUTER_VID_TCAM);

        if(SOC_IS_JERICHO(unit))
        {
            
            program_property->implicit_flag = 1;

            program_property->qual_to_ce_info.qualifier_to_ce_id[0][0] = 11;
            program_property->qual_to_ce_info.qualifier_to_ce_id[0][1] = 10;
            program_property->qual_to_ce_info.qualifier_to_ce_id[0][2] = 1;
            program_property->qual_to_ce_info.qualifier_to_ce_id[0][3] = 0;

            program_property->qual_to_ce_info.qualifier_to_ce_id[1][0] = 11;
            program_property->qual_to_ce_info.qualifier_to_ce_id[1][1] = 10;
            program_property->qual_to_ce_info.qualifier_to_ce_id[1][2] = 1;
            program_property->qual_to_ce_info.qualifier_to_ce_id[1][3] = 0;

            program_property->tcam_profile	= ARAD_TCAM_ACCESS_PROFILE_INVALID;
            program_property->qual_to_ce_info.qualifier_to_ce_id[2][0] = 1;
        }
    }

    program_property->key_program_variable = (0x2); 

    program_property->tcam_profile       = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->pd_bitmap_0        = (0x18); 
    program_property->pd_bitmap_1        = (0x0);

    if(SOC_IS_ARADPLUS_AND_BELOW(unit))
    {
        program_property->implicit_flag = 1;

        if (vid_type == OUTER_INNER_VID) 
        {
            program_property->qual_to_ce_info.qualifier_to_ce_id[0][0] = 6;
            program_property->qual_to_ce_info.qualifier_to_ce_id[0][1] = 4;
            program_property->qual_to_ce_info.qualifier_to_ce_id[0][2] = 2;
            program_property->qual_to_ce_info.qualifier_to_ce_id[0][3] = 1;

            program_property->qual_to_ce_info.qualifier_to_ce_id[1][0] = 6;
            program_property->qual_to_ce_info.qualifier_to_ce_id[1][1] = 4;
            program_property->qual_to_ce_info.qualifier_to_ce_id[1][2] = 2;
            program_property->qual_to_ce_info.qualifier_to_ce_id[1][3] = 1;
        }
        else
        {
            program_property->qual_to_ce_info.qualifier_to_ce_id[0][0] = 4;
            program_property->qual_to_ce_info.qualifier_to_ce_id[0][1] = 1;
            program_property->qual_to_ce_info.qualifier_to_ce_id[0][2] = 0;
            
            program_property->qual_to_ce_info.qualifier_to_ce_id[1][0] = 4;
            program_property->qual_to_ce_info.qualifier_to_ce_id[1][1] = 1;
            program_property->qual_to_ce_info.qualifier_to_ce_id[1][2] = 0;
        }
        program_property->qual_to_ce_info.qualifier_to_ce_id[2][0] = 0;
    }
}

void arad_pp_dbal_vt_program_vdxinitialvid_trill_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused )
{
    program_property->nof_keys = 3;

    program_property->vtt_table_index[0]             = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_TRILL_SEM_A);
    program_property->vtt_table_index[1]             = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxINITIALVID_SEM_B);
    program_property->tcam_profile                   = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->vtt_table_index[2]             = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_INITIAL_VID_TCAM);
    program_property->pd_bitmap_0                    = (0x8);
    program_property->pd_bitmap_1                    = (0x0);

    if(!SOC_IS_JERICHO(unit))
    {
        program_property->qual_to_ce_info.use_32_bit_ce[0][0] = 1;
    }
}

void arad_pp_dbal_vt_program_vdxoutervid_trill_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused )
{
    program_property->nof_keys = 3;

    program_property->vtt_table_index[0]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_TRILL_SEM_A);
    program_property->vtt_table_index[1]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_B);
    program_property->tcam_profile         = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->vtt_table_index[2]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_OUTER_VID_TCAM);
    program_property->pd_bitmap_0          = (0x8);  
    program_property->pd_bitmap_1          = (0x10);
    program_property->key_program_variable = (0x2); 

    if(!SOC_IS_JERICHO(unit))
    {
        program_property->qual_to_ce_info.use_32_bit_ce[1][0] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[1][1] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[1][2] = 1;
    }
}

void arad_pp_dbal_vt_program_vd_designated_vid_trill_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused )
{
    program_property->nof_keys = 3;

    program_property->vtt_table_index[0]             = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_TRILL_SEM_A);
    program_property->vtt_table_index[1]             = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_DESIGNATED_VID_TRILL_SEM_B);
    program_property->vtt_table_index[2]             = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_OUTER_VID_TCAM);
    program_property->tcam_profile                   = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->pd_bitmap_0                    = (0x8);  
    program_property->pd_bitmap_1                    = (0x10);

    if(!SOC_IS_JERICHO(unit))
    {
        program_property->qual_to_ce_info.use_32_bit_ce[0][0] = 1;
    }
}

void arad_pp_dbal_vt_program_vdxouter_inner_vid_trill_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused )
{

    program_property->nof_keys = 3;

    program_property->vtt_table_index[0]             = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_TRILL_SEM_A);
    program_property->vtt_table_index[1]             = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTER_INNER_VID_SEM_B);
    program_property->vtt_table_index[2]             = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_OUTER_VID_TCAM);
    program_property->tcam_profile                   = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->pd_bitmap_0                    = (0x8);  
    program_property->pd_bitmap_1                    = (0x0);

    if(!SOC_IS_JERICHO(unit))
    {
        program_property->qual_to_ce_info.use_32_bit_ce[1][0] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[1][1] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[1][2] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[1][3] = 1;
    }

}
void arad_pp_dbal_vt_program_vdxinitialvid_l1l2_property_set            (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused )
{

}

void arad_pp_dbal_vt_program_vdxoutervid_l1l2_property_set              (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused )
{

}

void arad_pp_dbal_vt_program_vdxinitialvid_l1_l2eli_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused )
{
    int vdxinitial_vid_index=0;

    program_property->nof_keys = 3;
    program_property->vtt_table_index[0]      = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxINITIALVID_SEM_A);
    if(g_prog_soc_prop[unit].mpls_index)
    {
        program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_ELI_L1IDX_SEM_B + (2*(g_prog_soc_prop[unit].mpls_1_namespace)));
    }
    else
    {
        program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_ELI_UNINDEXED_SEM_B);
    }
    program_property->tcam_profile                   = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->vtt_table_index[2]             = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_INITIAL_VID_TCAM);
    program_property->pd_bitmap_0                    = (0x10);  
    program_property->pd_bitmap_1                    = (0x8);  

    if(g_prog_soc_prop[unit].mpls_1_database == ARAD_PP_ISEM_ACCESS_ID_ISEM_A)
    {
        arad_pp_dbal_vtt_program_property_swap( program_property );
        program_property->vtt_table_index[0]             = program_property->vtt_table_index[1] - 1;
        program_property->vtt_table_index[1]             = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxINITIALVID_SEM_B);
        vdxinitial_vid_index=1;
    }

    if(!SOC_IS_JERICHO(unit))
    {
        program_property->qual_to_ce_info.use_32_bit_ce[vdxinitial_vid_index][0] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[vdxinitial_vid_index][1] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[vdxinitial_vid_index][2] = 1;
    }

}

void arad_pp_dbal_vt_program_vdxoutervid_l1_l2eli_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused )
{
    int vdxouter_vid_index =0;

    program_property->nof_keys = 3;
    program_property->vtt_table_index[0]             = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_A);
    if(g_prog_soc_prop[unit].mpls_index)
    {
        program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_ELI_L1IDX_SEM_B + (2*(g_prog_soc_prop[unit].mpls_1_namespace)));
    }
    else
    {
        program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_ELI_UNINDEXED_SEM_B);
    }
    program_property->tcam_profile                   = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->vtt_table_index[2]             = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_OUTER_VID_TCAM);
    program_property->pd_bitmap_0                    = (0x10);  
    program_property->pd_bitmap_1                    = (0x8);  

    if(g_prog_soc_prop[unit].mpls_1_database == ARAD_PP_ISEM_ACCESS_ID_ISEM_A)
    {
        arad_pp_dbal_vtt_program_property_swap( program_property );
        program_property->vtt_table_index[0]             = program_property->vtt_table_index[1] - 1;
        program_property->vtt_table_index[1]             = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_B);
        vdxouter_vid_index =1;
    }

    program_property->key_program_variable = (0x2); 

    if(!SOC_IS_JERICHO(unit))
    {
        program_property->qual_to_ce_info.use_32_bit_ce[vdxouter_vid_index][0] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[vdxouter_vid_index][1] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[vdxouter_vid_index][2] = 1;
    }
}


void arad_pp_dbal_vt_program_indexed_vdxinitialvid_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 mpls_label )
{
    int vdxinitial_vid_index=0;
    uint32 database, namespace;

    program_property->nof_keys = 3;
    program_property->vtt_table_index[0]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxINITIALVID_SEM_A);

    if(mpls_label == MPLS_LABEL_0)
    {
        namespace = g_prog_soc_prop[unit].mpls_1_namespace;
        database  = g_prog_soc_prop[unit].mpls_1_database;
        program_property->vtt_table_index[1]   = SW_DB_DBAL2VTT(( g_prog_soc_prop[unit].is_bos_in_key_enable ? SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_BOS_L1IDX_SEM_B : SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L1IDX_SEM_B));
    }
    else
    {
        arad_pp_dbal_vtt_find_namespace_database(unit, SOC_PPC_MPLS_TERM_NAMESPACE_L3, &namespace, &database);
        program_property->vtt_table_index[1]   = SW_DB_DBAL2VTT(( g_prog_soc_prop[unit].is_bos_in_key_enable ? SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_BOS_L1IDX_SEM_B : SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_L1IDX_SEM_B));
    }

    program_property->vtt_table_index[1] += 2*namespace;


    program_property->tcam_profile         = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->vtt_table_index[2]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_INITIAL_VID_TCAM);
    program_property->pd_bitmap_0          = (0x10);  
    program_property->pd_bitmap_1          = (0x8);  
    if (g_prog_soc_prop->ingress_full_mymac_1) {
        program_property->tcam_profile                   = ARAD_PP_ISEM_ACCESS_TCAM_SRC_PORT_DA_DB_PROFILE;
        program_property->vtt_table_index[2]             = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_SRC_PORT_DA_INITIAL_VID_TCAM);
        program_property->qual_to_ce_info.use_32_bit_ce[2][0] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[2][1] = 1;
    }
    if(database == ARAD_PP_ISEM_ACCESS_ID_ISEM_A)
    {
        arad_pp_dbal_vtt_program_property_swap( program_property );
        program_property->vtt_table_index[0]             = program_property->vtt_table_index[1] - 1;
        program_property->vtt_table_index[1]             = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxINITIALVID_SEM_B);
        vdxinitial_vid_index=1;
    }

    if(!SOC_IS_JERICHO(unit))
    {
        program_property->qual_to_ce_info.use_32_bit_ce[vdxinitial_vid_index][0] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[vdxinitial_vid_index][1] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[vdxinitial_vid_index][2] = 1;
    }

}

void arad_pp_dbal_vt_program_indexed_vdxinitialvid_mpls4_l1l2_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 mpls_label )
{
    int vdxinitial_vid_index=0;
    uint32 database, namespace;

    program_property->nof_keys = 3;
    program_property->vtt_table_index[0]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxINITIALVID_SEM_A);

    namespace = g_prog_soc_prop[unit].mpls_1_namespace;
    database  = g_prog_soc_prop[unit].mpls_1_database;
    program_property->vtt_table_index[1]   = SW_DB_DBAL2VTT(( g_prog_soc_prop[unit].is_bos_in_key_enable ? SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_BOS_L1IDX_SEM_B : SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L1IDX_SEM_B));
    program_property->vtt_table_index[1] += 2*namespace;

    program_property->tcam_profile         = ARAD_PP_ISEM_ACCESS_TCAM_MPLS_KEY_0_PROFILE;
    program_property->vtt_table_index[2]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_MPLS4_INITIAL_VID_L1_TCAM);
    program_property->pd_bitmap_0          = (0x10);  
    program_property->pd_bitmap_1          = (0x20);  
    if(database == ARAD_PP_ISEM_ACCESS_ID_ISEM_A)
    {
        arad_pp_dbal_vtt_program_property_swap( program_property );
        program_property->vtt_table_index[0]             = program_property->vtt_table_index[1] - 1;
        program_property->vtt_table_index[1]             = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxINITIALVID_SEM_B);
        vdxinitial_vid_index=1;
    }

    if(!SOC_IS_JERICHO(unit))
    {
        program_property->qual_to_ce_info.use_32_bit_ce[vdxinitial_vid_index][0] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[vdxinitial_vid_index][1] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[vdxinitial_vid_index][2] = 1;
    }

}



void arad_pp_dbal_vt_program_mpls_port_termination_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 mpls_label) {

    program_property->nof_keys = 3;
    program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_A);

    program_property->vtt_table_index[1] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_PORT_L1_L1IDX_SEM_B);

    program_property->tcam_profile                   = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->vtt_table_index[2]             = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_OUTER_VID_TCAM);

    if (g_prog_soc_prop->ingress_full_mymac_1) {
        program_property->tcam_profile                   = ARAD_PP_ISEM_ACCESS_TCAM_SRC_PORT_DA_DB_PROFILE;
        program_property->vtt_table_index[2] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_SRC_PORT_DA_OUTER_VID_TCAM);
        program_property->qual_to_ce_info.use_32_bit_ce[2][0] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[2][1] = 1;
    }

    program_property->pd_bitmap_0                    = (0x10);  
    program_property->pd_bitmap_1                    = (0x8);  

    program_property->key_program_variable = (0x2); 

    if(g_prog_soc_prop[unit].mpls_1_database == ARAD_PP_ISEM_ACCESS_ID_ISEM_A)
    {
        arad_pp_dbal_vtt_program_property_swap( program_property );
        program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_PORT_L1_L1IDX_SEM_A);
        program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_B);
    }


}




void arad_pp_dbal_vt_program_indexed_vdxoutervid_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 mpls_label )
{
    int vdxouter_vid_index =0;
    uint32 database, namespace;

    program_property->nof_keys = 3;
    program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_A);

    if(mpls_label == MPLS_LABEL_0)
    {
        namespace = g_prog_soc_prop[unit].mpls_1_namespace;
        database  = g_prog_soc_prop[unit].mpls_1_database;
        program_property->vtt_table_index[1]   = SW_DB_DBAL2VTT(( g_prog_soc_prop[unit].is_bos_in_key_enable ? SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_BOS_L1IDX_SEM_B : SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L1IDX_SEM_B));
    }
    else
    {
        arad_pp_dbal_vtt_find_namespace_database(unit, SOC_PPC_MPLS_TERM_NAMESPACE_L3, &namespace, &database);
        program_property->vtt_table_index[1]   = SW_DB_DBAL2VTT(( g_prog_soc_prop[unit].is_bos_in_key_enable ? SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_BOS_L1IDX_SEM_B : SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_L1IDX_SEM_B));
    }

    program_property->vtt_table_index[1] += 2*namespace;

    program_property->tcam_profile                   = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->vtt_table_index[2]             = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_OUTER_VID_TCAM);

    if (g_prog_soc_prop->ingress_full_mymac_1) {
        program_property->tcam_profile                   = ARAD_PP_ISEM_ACCESS_TCAM_SRC_PORT_DA_DB_PROFILE;
        program_property->vtt_table_index[2]             = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_SRC_PORT_DA_OUTER_VID_TCAM);
        program_property->qual_to_ce_info.use_32_bit_ce[2][0] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[2][1] = 1;
    }

    program_property->pd_bitmap_0                    = (0x10);  
    program_property->pd_bitmap_1                    = (0x8);  

    program_property->key_program_variable = (0x2); 

    if(database == ARAD_PP_ISEM_ACCESS_ID_ISEM_A)
    {
        arad_pp_dbal_vtt_program_property_swap( program_property );
        program_property->vtt_table_index[0]             = program_property->vtt_table_index[1] - 1;
        program_property->vtt_table_index[1]             = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_B);
        vdxouter_vid_index =1;
    }

    if(!SOC_IS_JERICHO(unit))
    {
        program_property->qual_to_ce_info.use_32_bit_ce[vdxouter_vid_index][0] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[vdxouter_vid_index][1] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[vdxouter_vid_index][2] = 1;
    }
}

void arad_pp_dbal_vt_program_indexed_vdxoutervid_mpls4_l1l2_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 mpls_label )
{
    int vdxouter_vid_index =0;
    uint32 database, namespace;

    program_property->nof_keys = 3;
    program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_A);

    namespace = g_prog_soc_prop[unit].mpls_1_namespace;
    database  = g_prog_soc_prop[unit].mpls_1_database;

    program_property->vtt_table_index[1]   = SW_DB_DBAL2VTT(( g_prog_soc_prop[unit].is_bos_in_key_enable ? SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_BOS_L1IDX_SEM_B : SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L1IDX_SEM_B));

    program_property->vtt_table_index[1] += 2*namespace;


    program_property->tcam_profile                   = ARAD_PP_ISEM_ACCESS_TCAM_MPLS_KEY_0_PROFILE;

    program_property->vtt_table_index[2]             = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_MPLS4_OUTER_VID_L1_TCAM);


    program_property->pd_bitmap_0                    = (0x10);  
    program_property->pd_bitmap_1                    = (0x20);  

    program_property->key_program_variable = (0x2); 

    if(database == ARAD_PP_ISEM_ACCESS_ID_ISEM_A)
    {
        arad_pp_dbal_vtt_program_property_swap( program_property );
        program_property->vtt_table_index[0]             = program_property->vtt_table_index[1] - 1;
        program_property->vtt_table_index[1]             = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_B);
        vdxouter_vid_index =1;
    }

    if(!SOC_IS_JERICHO(unit))
    {
        program_property->qual_to_ce_info.use_32_bit_ce[vdxouter_vid_index][0] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[vdxouter_vid_index][1] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[vdxouter_vid_index][2] = 1;
    }
}


void arad_pp_dbal_vt_program_ipv4_vdxinitial_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
    int vdxinitial_vid_sem_index = 0;
    int dip_sem_index = 1;
    int dip_sip_tcam_index = 2;

    program_property->nof_keys = 3;

    program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxINITIALVID_SEM_A);
    program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_DIPv4_SEM_B);
    program_property->vtt_table_index[2]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_DIP_SIP_INITIAL_VID_TCAM);

    if (SOC_IS_JERICHO(unit) && (g_prog_soc_prop[unit].ipv4_term_dip_sip_enable == 3)) { 
        program_property->tcam_profile = ARAD_TCAM_ACCESS_PROFILE_INVALID; 
    } else if (g_prog_soc_prop[unit].ipv4_term_dip_sip_enable) {
        program_property->tcam_profile = ARAD_PP_ISEM_ACCESS_TCAM_IPV4_TT_ETH_DB_PROFILE; 
    } else {
        program_property->tcam_profile = ARAD_TCAM_ACCESS_PROFILE_INVALID; 
    }

    program_property->pd_bitmap_0 = (0x10);  
    program_property->pd_bitmap_1 = (0x28);

    if(g_prog_soc_prop[unit].mpls_1_database == ARAD_PP_ISEM_ACCESS_ID_ISEM_A)
    {
        arad_pp_dbal_vtt_program_property_swap( program_property );
        program_property->vtt_table_index[0]       = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_DIPv4_SEM_A);
        program_property->vtt_table_index[1]       = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxINITIALVID_SEM_B);
        dip_sem_index = 0;
        vdxinitial_vid_sem_index = 1;
    }

    
    if (SOC_IS_JERICHO(unit) && g_prog_soc_prop[unit].ipv4_term_dip_sip_enable == 3) { 
        program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_INITIAL_VID_TCAM);
        program_property->nof_keys = 2;
        if (g_prog_soc_prop[unit].mpls_1_database == ARAD_PP_ISEM_ACCESS_ID_ISEM_A) 
        {
            program_property->vtt_table_index[0]       = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxINITIALVID_SEM_B);
            dip_sem_index = 1;
            vdxinitial_vid_sem_index = 0;
        }
    }

    if(!SOC_IS_JERICHO(unit))
    {
        program_property->implicit_flag = 1;

        program_property->qual_to_ce_info.qualifier_to_ce_id[vdxinitial_vid_sem_index][0] = 7;
        program_property->qual_to_ce_info.qualifier_to_ce_id[vdxinitial_vid_sem_index][1] = 6;
        program_property->qual_to_ce_info.qualifier_to_ce_id[vdxinitial_vid_sem_index][2] = 5;

        program_property->qual_to_ce_info.qualifier_to_ce_id[dip_sem_index][0] = 3;

        program_property->qual_to_ce_info.qualifier_to_ce_id[dip_sip_tcam_index][0] = 4;
        program_property->qual_to_ce_info.qualifier_to_ce_id[dip_sip_tcam_index][1] = 3;
        program_property->qual_to_ce_info.qualifier_to_ce_id[dip_sip_tcam_index][2] = 2;
        program_property->qual_to_ce_info.qualifier_to_ce_id[dip_sip_tcam_index][3] = 1;
        program_property->qual_to_ce_info.qualifier_to_ce_id[dip_sip_tcam_index][4] = 0;

    }

}



void arad_pp_dbal_vt_program_ipv4_port_termination_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused) {

    program_property->nof_keys = 2;

    program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxINITIALVID_SEM_A);
    program_property->vtt_table_index[1] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_DIPv4_DUMMY_SEM_B);

    program_property->pd_bitmap_0 = (0x10);  
    program_property->pd_bitmap_1 = (0x28);

    if (g_prog_soc_prop[unit].mpls_1_database == ARAD_PP_ISEM_ACCESS_ID_ISEM_A) {
        arad_pp_dbal_vtt_program_property_swap(program_property);
        program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_DIPv4_DUMMY_SEM_A);
        program_property->vtt_table_index[1] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxINITIALVID_SEM_B);
    }

}


void arad_pp_dbal_vt_program_ipv4_vdxouter_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
    int vdxouter_vid_sem_index = 0;
    int dip_sem_index = 1;
    int dip_sip_tcam_index = 2;

    program_property->nof_keys = 3;

    program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_A);
    program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_DIPv4_SEM_B);
    program_property->vtt_table_index[2]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_DIP_SIP_OUTER_VID_TCAM);

    if (SOC_IS_JERICHO(unit) && (g_prog_soc_prop[unit].ipv4_term_dip_sip_enable == 3)) { 
        program_property->tcam_profile = ARAD_TCAM_ACCESS_PROFILE_INVALID; 
    } else if (g_prog_soc_prop[unit].ipv4_term_dip_sip_enable) {
        program_property->tcam_profile = ARAD_PP_ISEM_ACCESS_TCAM_IPV4_TT_ETH_DB_PROFILE; 
    } else {
        program_property->tcam_profile = ARAD_TCAM_ACCESS_PROFILE_INVALID; 
    }


    program_property->pd_bitmap_0 = (0x10);  
    program_property->pd_bitmap_1 = (0x28); 

    program_property->key_program_variable = (0x2); 

    if(g_prog_soc_prop[unit].mpls_1_database == ARAD_PP_ISEM_ACCESS_ID_ISEM_A)
    {
        arad_pp_dbal_vtt_program_property_swap( program_property );
        program_property->vtt_table_index[0]       = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_DIPv4_SEM_A);
        program_property->vtt_table_index[1]       = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_B);
        dip_sem_index = 0;
        vdxouter_vid_sem_index = 1;
    }


    
    if (SOC_IS_JERICHO(unit) && g_prog_soc_prop[unit].ipv4_term_dip_sip_enable == 3)
    {
        program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_OUTER_VID_TCAM);
        program_property->nof_keys = 2;
        if (g_prog_soc_prop[unit].mpls_1_database == ARAD_PP_ISEM_ACCESS_ID_ISEM_A) 
        {
            program_property->vtt_table_index[0]       = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_B);
            dip_sem_index = 1;
            vdxouter_vid_sem_index = 0;
        }
        
    }


    if(!SOC_IS_JERICHO(unit))
    {
        program_property->implicit_flag = 1;

        program_property->qual_to_ce_info.qualifier_to_ce_id[vdxouter_vid_sem_index][0] = 7;
        program_property->qual_to_ce_info.qualifier_to_ce_id[vdxouter_vid_sem_index][1] = 6;
        program_property->qual_to_ce_info.qualifier_to_ce_id[vdxouter_vid_sem_index][2] = 5;

        program_property->qual_to_ce_info.qualifier_to_ce_id[dip_sem_index][0] = 3;

        program_property->qual_to_ce_info.qualifier_to_ce_id[dip_sip_tcam_index][0] = 4;
        program_property->qual_to_ce_info.qualifier_to_ce_id[dip_sip_tcam_index][1] = 3;
        program_property->qual_to_ce_info.qualifier_to_ce_id[dip_sip_tcam_index][2] = 2;
        program_property->qual_to_ce_info.qualifier_to_ce_id[dip_sip_tcam_index][3] = 1;
        program_property->qual_to_ce_info.qualifier_to_ce_id[dip_sip_tcam_index][4] = 0;
    }



}

void arad_pp_dbal_vt_program_ipv6_vdxinitial_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 is_recycle)
{
    int vdxouter_vid_sem_index = 0;
    int dip_np_et_tcam_index = 1;

    program_property->processing_profile = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_IPV6_CASCADE;
    program_property->nof_keys = 2;

    program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxINITIALVID_SEM_A);
    if (is_recycle) {
        program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_IPV6_DIP_NP_ET_INITIAL_VID_AFTER_RECYCLE_TCAM);
    } else {
        program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_IPV6_DIP_NP_ET_INITIAL_VID_TCAM);
    }

    if (g_prog_soc_prop[unit].ipv6_term_dip_sip_enable) { 
        if (is_recycle) {
            program_property->tcam_profile = ARAD_PP_ISEM_ACCESS_TCAM_IPV6_AFTER_RECYCLE_CASCADE_VT_DB_PROFILE; 
        } else {
            program_property->tcam_profile = ARAD_PP_ISEM_ACCESS_TCAM_IPV6_CASCADE_VT_DB_PROFILE;
        } 
    } else {
        program_property->tcam_profile = ARAD_TCAM_ACCESS_PROFILE_INVALID; 
    }

    program_property->pd_bitmap_0 = (0x10);  
    program_property->pd_bitmap_1 = (0x20);   
    
    program_property->implicit_flag = 1;
    program_property->qual_to_ce_info.qualifier_to_ce_id[vdxouter_vid_sem_index][0] = 6;
    program_property->qual_to_ce_info.qualifier_to_ce_id[vdxouter_vid_sem_index][1] = 3;
    program_property->qual_to_ce_info.qualifier_to_ce_id[vdxouter_vid_sem_index][2] = 2;

    program_property->qual_to_ce_info.qualifier_to_ce_id[dip_np_et_tcam_index][0] = 11; 
    program_property->qual_to_ce_info.qualifier_to_ce_id[dip_np_et_tcam_index][1] = 10; 
    program_property->qual_to_ce_info.qualifier_to_ce_id[dip_np_et_tcam_index][2] = 9;  
    program_property->qual_to_ce_info.qualifier_to_ce_id[dip_np_et_tcam_index][3] = 8;  
    program_property->qual_to_ce_info.qualifier_to_ce_id[dip_np_et_tcam_index][4] = 5;  
    program_property->qual_to_ce_info.qualifier_to_ce_id[dip_np_et_tcam_index][5] = 4;  
    program_property->qual_to_ce_info.qualifier_to_ce_id[dip_np_et_tcam_index][6] = 1;  
    program_property->qual_to_ce_info.qualifier_to_ce_id[dip_np_et_tcam_index][7] = 0;  
}

void arad_pp_dbal_vt_program_ipv6_vdxouter_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 is_recycle)
{
    int vdxouter_vid_sem_index = 0;
    int dip_np_et_tcam_index = 1;
    
    program_property->processing_profile = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_IPV6_CASCADE;
    program_property->nof_keys = 2;

    program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_A);
    if (is_recycle) {
        program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_IPV6_DIP_NP_ET_OUTER_VID_AFTER_RECYCLE_TCAM);
    } else {
        program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_IPV6_DIP_NP_ET_OUTER_VID_TCAM);
    }

    if (g_prog_soc_prop[unit].ipv6_term_dip_sip_enable) { 
        if (is_recycle) {
            program_property->tcam_profile = ARAD_PP_ISEM_ACCESS_TCAM_IPV6_AFTER_RECYCLE_CASCADE_VT_DB_PROFILE; 
        } else {
            program_property->tcam_profile = ARAD_PP_ISEM_ACCESS_TCAM_IPV6_CASCADE_VT_DB_PROFILE;
        } 
    } else {
        program_property->tcam_profile = ARAD_TCAM_ACCESS_PROFILE_INVALID; 
    }

    program_property->pd_bitmap_0 = (0x10);  
    program_property->pd_bitmap_1 = (0x20); 

    program_property->key_program_variable = (0x2); 

    program_property->implicit_flag = 1;
    program_property->qual_to_ce_info.qualifier_to_ce_id[vdxouter_vid_sem_index][0] = 6;
    program_property->qual_to_ce_info.qualifier_to_ce_id[vdxouter_vid_sem_index][1] = 3;
    program_property->qual_to_ce_info.qualifier_to_ce_id[vdxouter_vid_sem_index][2] = 2;

    program_property->qual_to_ce_info.qualifier_to_ce_id[dip_np_et_tcam_index][0] = 11; 
    program_property->qual_to_ce_info.qualifier_to_ce_id[dip_np_et_tcam_index][1] = 10; 
    program_property->qual_to_ce_info.qualifier_to_ce_id[dip_np_et_tcam_index][2] = 9;  
    program_property->qual_to_ce_info.qualifier_to_ce_id[dip_np_et_tcam_index][3] = 8;  
    program_property->qual_to_ce_info.qualifier_to_ce_id[dip_np_et_tcam_index][4] = 5;  
    program_property->qual_to_ce_info.qualifier_to_ce_id[dip_np_et_tcam_index][5] = 4;  
    program_property->qual_to_ce_info.qualifier_to_ce_id[dip_np_et_tcam_index][6] = 1;  
    program_property->qual_to_ce_info.qualifier_to_ce_id[dip_np_et_tcam_index][7] = 0;  
}


void arad_pp_dbal_vt_program_evb_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 vid_type)
{

    program_property->nof_keys = 2;

    if(vid_type == OUTER_VID)
    {
        program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_EVB_SINGLE_TAG_SEM_B);
        program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_INITIAL_VID_TCAM);
    }
    else
    {
        program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_EVB_DOUBLE_TAG_SEM_B);
        program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_OUTER_VID_TCAM);
    }

    program_property->key_program_variable  = (ARAD_PP_ISEM_ACCESS_EVB_DEFAULT_S_CHANNEL);
    program_property->tcam_profile          = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->pd_bitmap_0           = (0x8); 
    program_property->pd_bitmap_1           = (0x0);

    program_property->qual_to_ce_info.use_32_bit_ce[1][0] = 1;
}

void arad_pp_dbal_vt_program_double_tag_priority_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
    program_property->nof_keys = 3;

    program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTER_INITIAL_VID_SEM_A);
    program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxINITIALVID_SEM_B);
    program_property->vtt_table_index[2]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_INITIAL_VID_TCAM);
    program_property->tcam_profile        = ARAD_TCAM_ACCESS_PROFILE_INVALID;

    program_property->pd_bitmap_0 = (0x18); 
    program_property->pd_bitmap_1 = (0x0);

    if(!SOC_IS_JERICHO(unit))
    {
        program_property->qual_to_ce_info.use_32_bit_ce[0][0] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[0][1] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[0][2] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[0][3] = 1;
    }
}

void arad_pp_dbal_vt_program_double_tag_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
    program_property->nof_keys = 2;

    if((g_prog_soc_prop[unit].mpls_1_database == ARAD_PP_ISEM_ACCESS_ID_ISEM_A) || (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 20) || (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 22)){
        program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTER_INITIAL_VID_SEM_A);
        program_property->pd_bitmap_0 = (0x10); 
    } else {
        program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTER_INITIAL_VID_SEM_B);
        program_property->pd_bitmap_0 = (0x8); 
    }

    program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_INITIAL_VID_TCAM);
    program_property->tcam_profile        = ARAD_TCAM_ACCESS_PROFILE_INVALID;

    program_property->pd_bitmap_1 = (0x0);

    if(!SOC_IS_JERICHO(unit))
    {
        program_property->qual_to_ce_info.use_32_bit_ce[1][0] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[1][1] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[1][2] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[1][3] = 1;
    }
}

void arad_pp_dbal_vt_program_ipv4_vdxinitial_recycle_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
    int vdxinitial_vid_sem_index = 0;

    program_property->nof_keys = 3;

    program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxINITIALVID_SEM_A);
    program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_DIPv4_DUMMY_SEM_B);
    program_property->vtt_table_index[2]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_INITIAL_VID_TCAM);
    program_property->tcam_profile        = ARAD_TCAM_ACCESS_PROFILE_INVALID;

    program_property->pd_bitmap_0 = (0x10);  
    program_property->pd_bitmap_1 = (0x28);

    if(g_prog_soc_prop[unit].mpls_1_database == ARAD_PP_ISEM_ACCESS_ID_ISEM_A)
    {
        arad_pp_dbal_vtt_program_property_swap( program_property );
        program_property->vtt_table_index[0]       = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_DIPv4_DUMMY_SEM_A);
        program_property->vtt_table_index[1]       = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxINITIALVID_SEM_B);
        vdxinitial_vid_sem_index = 1;
    }

    if(!SOC_IS_JERICHO(unit))
    {
        program_property->qual_to_ce_info.use_32_bit_ce[vdxinitial_vid_sem_index][0] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[vdxinitial_vid_sem_index][1] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[vdxinitial_vid_sem_index][2] = 1;
    }

}
void arad_pp_dbal_vt_program_ipv4_vdxouter_recycle_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
    int vdxouter_vid_sem_index = 0;

    program_property->nof_keys = 3;

    program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_A);
    program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_DIPv4_DUMMY_SEM_B);
    program_property->vtt_table_index[2]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_OUTER_VID_TCAM);
    program_property->tcam_profile        = ARAD_TCAM_ACCESS_PROFILE_INVALID;

    program_property->pd_bitmap_0 = (0x10);  
    program_property->pd_bitmap_1 = (0x28);

    program_property->key_program_variable = (0x2); 

    if(g_prog_soc_prop[unit].mpls_1_database == ARAD_PP_ISEM_ACCESS_ID_ISEM_A)
    {
        arad_pp_dbal_vtt_program_property_swap( program_property );
        program_property->vtt_table_index[0]       = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_DIPv4_DUMMY_SEM_A);
        program_property->vtt_table_index[1]       = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_B);
        vdxouter_vid_sem_index = 1;
    }

    if(!SOC_IS_JERICHO(unit))
    {
        program_property->qual_to_ce_info.use_32_bit_ce[vdxouter_vid_sem_index][0] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[vdxouter_vid_sem_index][1] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[vdxouter_vid_sem_index][2] = 1;
    }

}

void arad_pp_dbal_vt_program_single_tag_5tupple_qinq_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
    program_property->nof_keys = 2;
    program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_SINGLE_TAG_VD_SEM_B);
    program_property->vtt_table_index[1] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_5_TUPPLE_TCAM);
    program_property->tcam_profile       = ARAD_PP_ISEM_ACCESS_TCAM_IPV4_MATCH_VT_DB_PROFILE;

    program_property->pd_bitmap_0        = (0x28); 
    program_property->pd_bitmap_1        = (0x0);

    if(!SOC_IS_JERICHO(unit))
    {
        program_property->implicit_flag = 1;

        program_property->qual_to_ce_info.qualifier_to_ce_id[0][0] = 7;
        program_property->qual_to_ce_info.qualifier_to_ce_id[0][1] = 4;

        program_property->qual_to_ce_info.qualifier_to_ce_id[1][0] = 7;
        program_property->qual_to_ce_info.qualifier_to_ce_id[1][1] = 6;
        program_property->qual_to_ce_info.qualifier_to_ce_id[1][2] = 5;
        program_property->qual_to_ce_info.qualifier_to_ce_id[1][3] = 3;
        program_property->qual_to_ce_info.qualifier_to_ce_id[1][4] = 2;
        program_property->qual_to_ce_info.qualifier_to_ce_id[1][5] = 0;
    }
    else
    {
        program_property->qual_to_ce_info.use_32_bit_ce[1][0] = 1;
    }

}
void arad_pp_dbal_vt_program_double_tag_5tupple_qinq_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
    program_property->nof_keys = 3;
    program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_DOUBLE_TAG_VD_SEM_A);
    program_property->vtt_table_index[1] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_SINGLE_TAG_VD_SEM_B);
    program_property->vtt_table_index[2] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_5_TUPPLE_TCAM);
    program_property->tcam_profile       = ARAD_PP_ISEM_ACCESS_TCAM_IPV4_MATCH_VT_DB_PROFILE;

    program_property->pd_bitmap_0        = (0x38); 
    program_property->pd_bitmap_1        = (0x0);

    program_property->implicit_flag = 1;

    program_property->qual_to_ce_info.qualifier_to_ce_id[0][0] = SOC_IS_JERICHO(unit) ? 11:7;
    program_property->qual_to_ce_info.qualifier_to_ce_id[0][1] = SOC_IS_JERICHO(unit) ? 9:4;
    program_property->qual_to_ce_info.qualifier_to_ce_id[0][2] = SOC_IS_JERICHO(unit) ? 8:1;

    program_property->qual_to_ce_info.qualifier_to_ce_id[1][0] = SOC_IS_JERICHO(unit) ? 11:7;
    program_property->qual_to_ce_info.qualifier_to_ce_id[1][1] = SOC_IS_JERICHO(unit) ? 9:4;

    program_property->qual_to_ce_info.qualifier_to_ce_id[2][0] = SOC_IS_JERICHO(unit) ? 11:7;
    program_property->qual_to_ce_info.qualifier_to_ce_id[2][1] = SOC_IS_JERICHO(unit) ? 10:6;
    program_property->qual_to_ce_info.qualifier_to_ce_id[2][2] = SOC_IS_JERICHO(unit) ? 6:5;
    program_property->qual_to_ce_info.qualifier_to_ce_id[2][3] = SOC_IS_JERICHO(unit) ? 5:3;
    program_property->qual_to_ce_info.qualifier_to_ce_id[2][4] = SOC_IS_JERICHO(unit) ? 4:2;
    program_property->qual_to_ce_info.qualifier_to_ce_id[2][5] = 0;

}
void arad_pp_dbal_vt_program_tst1_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 vid_type)
{
    int tcam_indx = 2;
    program_property->implicit_flag = 1;

    switch(vid_type)
     {
     case TST1_VID_UNTAGGED:
         program_property->nof_keys = 2;
         program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VD_SEM_A);
         program_property->vtt_table_index[1] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_UNTAGGED_TST1_TCAM);
         tcam_indx = 1;
         program_property->pd_bitmap_0        = (0x30); 
         program_property->pd_bitmap_1        = (0x0);

         program_property->qual_to_ce_info.qualifier_to_ce_id[0][0] = 5; 

         break;
     case TST1_VID_ONE_TAG:
         program_property->nof_keys = 3;
         program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_INITIAL_VD_SEM_A);
         program_property->vtt_table_index[1] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_OUTER_VD_SEM_B);
         program_property->vtt_table_index[2] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_ONE_TAG_TST1_TCAM);
         program_property->pd_bitmap_0        = (0x38); 
         program_property->pd_bitmap_1        = (0x0);

         program_property->qual_to_ce_info.qualifier_to_ce_id[0][0] = 5; 
         program_property->qual_to_ce_info.qualifier_to_ce_id[0][1] = SOC_IS_JERICHO(unit) ? 3 : 4; 

         program_property->qual_to_ce_info.qualifier_to_ce_id[1][0] = 5; 
         program_property->qual_to_ce_info.qualifier_to_ce_id[1][1] = SOC_IS_JERICHO(unit) ? 4 : 3; 
         break;
     case TST1_VID_DOUBLE_TAG:
         program_property->nof_keys = 3;
         program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_OUTER_INNER_VD_SEM_A);
         program_property->vtt_table_index[1] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_OUTER_VD_SEM_B);
         program_property->vtt_table_index[2] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_DOUBLE_TAG_TST1_TCAM);
         program_property->pd_bitmap_0        = (0x38); 
         program_property->pd_bitmap_1        = (0x0);

         program_property->qual_to_ce_info.qualifier_to_ce_id[0][0] = 5; 
         program_property->qual_to_ce_info.qualifier_to_ce_id[0][1] = SOC_IS_JERICHO(unit) ? 4 : 3; 
         program_property->qual_to_ce_info.qualifier_to_ce_id[0][2] = 2; 

         program_property->qual_to_ce_info.qualifier_to_ce_id[1][0] = 5; 
         program_property->qual_to_ce_info.qualifier_to_ce_id[1][1] = SOC_IS_JERICHO(unit) ? 4 : 3; 

         break;
     }

    program_property->tcam_profile       = ARAD_PP_ISEM_ACCESS_VT_CLASSIFICATIONS_EFP;

    program_property->qual_to_ce_info.qualifier_to_ce_id[tcam_indx][0] = 7;
    program_property->qual_to_ce_info.qualifier_to_ce_id[tcam_indx][1] = 6;
    program_property->qual_to_ce_info.qualifier_to_ce_id[tcam_indx][2] = 5;
    program_property->qual_to_ce_info.qualifier_to_ce_id[tcam_indx][3] = SOC_IS_JERICHO(unit) ? 4 : 3;
    program_property->qual_to_ce_info.qualifier_to_ce_id[tcam_indx][4] = 2;
    program_property->qual_to_ce_info.qualifier_to_ce_id[tcam_indx][5] = 1; 
    program_property->qual_to_ce_info.qualifier_to_ce_id[tcam_indx][6] = 0;



}


void arad_pp_dbal_vt_program_vrrp_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 vid_type)
{
    int tcam_indx = 2;
    program_property->implicit_flag = 1;
    program_property->processing_profile = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_VRRP;
    switch(vid_type)
     {
     case TST1_VID_UNTAGGED:
         program_property->nof_keys = 2;
         program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VD_SEM_A);
         program_property->vtt_table_index[1] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_UNTAGGED_VRRP_TCAM);
         tcam_indx = 1;
         program_property->pd_bitmap_0        = (0x10); 
         program_property->pd_bitmap_1        = (0x0);

         program_property->qual_to_ce_info.qualifier_to_ce_id[0][0] = SOC_IS_JERICHO(unit) ? 8 : 6; 

         break;
     case TST1_VID_ONE_TAG:
         program_property->nof_keys = 3;
         program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_INITIAL_VD_SEM_A);
         program_property->vtt_table_index[1] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_OUTER_VD_SEM_B);
         program_property->vtt_table_index[2] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_ONE_TAG_VRRP_TCAM);
         program_property->pd_bitmap_0        = (0x18); 
         program_property->pd_bitmap_1        = (0x0);

         program_property->qual_to_ce_info.qualifier_to_ce_id[0][0] = SOC_IS_JERICHO(unit) ? 8 : 6; 
         program_property->qual_to_ce_info.qualifier_to_ce_id[0][1] = SOC_IS_JERICHO(unit) ? 7 : 5; 

         program_property->qual_to_ce_info.qualifier_to_ce_id[1][0] = SOC_IS_JERICHO(unit) ? 8 : 6; 
         program_property->qual_to_ce_info.qualifier_to_ce_id[1][1] = SOC_IS_JERICHO(unit) ? 6 : 4; 
         break;
     case TST1_VID_DOUBLE_TAG:
         program_property->nof_keys = 3;
         program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_OUTER_INNER_VD_SEM_A);
         program_property->vtt_table_index[1] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_OUTER_VD_SEM_B);
         program_property->vtt_table_index[2] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_DOUBLE_TAG_VRRP_TCAM);
         program_property->pd_bitmap_0        = (0x18); 
         program_property->pd_bitmap_1        = (0x0);

         program_property->qual_to_ce_info.qualifier_to_ce_id[0][0] = SOC_IS_JERICHO(unit) ? 8 : 6; 
         program_property->qual_to_ce_info.qualifier_to_ce_id[0][1] = SOC_IS_JERICHO(unit) ? 6 : 4; 
         program_property->qual_to_ce_info.qualifier_to_ce_id[0][2] = SOC_IS_JERICHO(unit) ? 5 : 3; 

         program_property->qual_to_ce_info.qualifier_to_ce_id[1][0] = SOC_IS_JERICHO(unit) ? 8 : 6; 
         program_property->qual_to_ce_info.qualifier_to_ce_id[1][1] = SOC_IS_JERICHO(unit) ? 6 : 4; 

         break;
     }

    program_property->tcam_profile       = ARAD_PP_ISEM_ACCESS_TCAM_VRRP_VSI_DA_PROFILE;

    program_property->qual_to_ce_info.qualifier_to_ce_id[tcam_indx][0] = SOC_IS_JERICHO(unit) ? 9 : 7; 
    program_property->qual_to_ce_info.qualifier_to_ce_id[tcam_indx][1] = SOC_IS_JERICHO(unit) ? 8 : 6; 
    program_property->qual_to_ce_info.qualifier_to_ce_id[tcam_indx][2] = SOC_IS_JERICHO(unit) ? 5 : 3; 
    program_property->qual_to_ce_info.qualifier_to_ce_id[tcam_indx][3] = SOC_IS_JERICHO(unit) ? 4 : 2; 
    program_property->qual_to_ce_info.qualifier_to_ce_id[tcam_indx][4] = 1;   
    program_property->qual_to_ce_info.qualifier_to_ce_id[tcam_indx][5] = 0;  
}




void arad_pp_dbal_vt_program_exp_null_tst1_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 vid_type)
{

    switch(vid_type)
     {
     case TST1_VID_UNTAGGED:
         program_property->nof_keys = 2;
         program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VD_SEM_A);
         program_property->vtt_table_index[1] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_TST1_MPLS_TCAM);
         program_property->pd_bitmap_0        = (0x10); 
         program_property->pd_bitmap_1        = (0x20);
         break;
     case TST1_VID_ONE_TAG:
         program_property->nof_keys = 3;
         program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_INITIAL_VD_SEM_A);
         program_property->vtt_table_index[1] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_OUTER_VD_SEM_B);
         program_property->vtt_table_index[2] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_TST1_MPLS_TCAM);
         program_property->pd_bitmap_0        = (0x18); 
         program_property->pd_bitmap_1        = (0x20);
         break;
     case TST1_VID_DOUBLE_TAG:
         program_property->nof_keys = 3;
         program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_OUTER_INNER_VD_SEM_A);
         program_property->vtt_table_index[1] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_OUTER_VD_SEM_B);
         program_property->vtt_table_index[2] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_TST1_MPLS_TCAM);
         program_property->pd_bitmap_0        = (0x18); 
         program_property->pd_bitmap_1        = (0x20);
         break;
     }

    program_property->tcam_profile       = ARAD_PP_ISEM_ACCESS_TCAM_MPLS_EXPLICIT_NULL_VT_DB_PROFILE;

    if(!SOC_IS_JERICHO(unit) && (vid_type != TST1_VID_UNTAGGED))
    {
        program_property->implicit_flag = 1;

        program_property->qual_to_ce_info.qualifier_to_ce_id[0][0] = 7;
        program_property->qual_to_ce_info.qualifier_to_ce_id[0][1] = 6;
        program_property->qual_to_ce_info.qualifier_to_ce_id[0][2] = 3;

        program_property->qual_to_ce_info.qualifier_to_ce_id[1][0] = 7;
        program_property->qual_to_ce_info.qualifier_to_ce_id[1][1] = 2;

        program_property->qual_to_ce_info.qualifier_to_ce_id[2][0] = 5;
        program_property->qual_to_ce_info.qualifier_to_ce_id[2][1] = 1;
        program_property->qual_to_ce_info.qualifier_to_ce_id[2][2] = 0;

    }
}

void arad_pp_dbal_vt_program_outer_inner_pcp_tst2_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 tpid)
{
    program_property->nof_keys = 3;
    program_property->vtt_table_index[0] = SW_DB_DBAL2VTT((tpid == TPID1) ? SOC_DPP_DBAL_SW_TABLE_ID_QINQ_COMPRESSED_TPID1_SEM_A : SOC_DPP_DBAL_SW_TABLE_ID_QINQ_COMPRESSED_TPID2_SEM_A);
    program_property->vtt_table_index[1] = SW_DB_DBAL2VTT((tpid == TPID1) ? SOC_DPP_DBAL_SW_TABLE_ID_QINANY_PCP_TPID1_SEM_A : SOC_DPP_DBAL_SW_TABLE_ID_QINANY_PCP_TPID2_SEM_A);
    program_property->vtt_table_index[2] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_TST2_TCAM);
    program_property->tcam_profile       = ARAD_PP_ISEM_ACCESS_TCAM_TST2_PROFILE;

    program_property->pd_bitmap_0              = (0x38); 
    program_property->pd_bitmap_1              = (0x0);


    if(SOC_IS_JERICHO(unit))
    {
        program_property->qual_to_ce_info.use_32_bit_ce[0][0] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[0][1] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[0][2] = 1;
    }
    else
    {
        program_property->implicit_flag = 1;

        program_property->qual_to_ce_info.qualifier_to_ce_id[0][0] = 7;
        program_property->qual_to_ce_info.qualifier_to_ce_id[0][1] = 6;
        program_property->qual_to_ce_info.qualifier_to_ce_id[0][2] = 0;

        program_property->qual_to_ce_info.qualifier_to_ce_id[1][0] = 7;
        program_property->qual_to_ce_info.qualifier_to_ce_id[1][1] = 6;
        program_property->qual_to_ce_info.qualifier_to_ce_id[1][2] = 5;

        program_property->qual_to_ce_info.qualifier_to_ce_id[2][0] = 7; 
        program_property->qual_to_ce_info.qualifier_to_ce_id[2][1] = 6; 
        program_property->qual_to_ce_info.qualifier_to_ce_id[2][2] = 5; 
        program_property->qual_to_ce_info.qualifier_to_ce_id[2][3] = 4; 
        program_property->qual_to_ce_info.qualifier_to_ce_id[2][4] = 1; 
    }
}


void arad_pp_dbal_vt_program_test2_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 tpid)
{
    program_property->nof_keys = 2;
    program_property->vtt_table_index[0]             = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_TEST2_TCAM);
     
    program_property->pd_bitmap_1                    = (0x0);

    program_property->vtt_table_index[1] =  SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_TEST2_SEM_A);

    program_property->pd_bitmap_0          = 0x10;
    
    program_property->tcam_profile       = ARAD_PP_ISEM_ACCESS_TCAM_TEST2_PROFILE;
}

void arad_pp_dbal_vt_program_outer_pcp_tst2_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 tpid)
{
    program_property->nof_keys = 3;
    program_property->vtt_table_index[0] = SW_DB_DBAL2VTT((tpid == TPID1) ? SOC_DPP_DBAL_SW_TABLE_ID_1Q_PCP_COMPRESSED_TPID1_SEM_A : SOC_DPP_DBAL_SW_TABLE_ID_1Q_PCP_COMPRESSED_TPID2_SEM_A);
    program_property->vtt_table_index[1] = SW_DB_DBAL2VTT((tpid == TPID1) ? SOC_DPP_DBAL_SW_TABLE_ID_1Q_COMPRESSED_TPID1_SEM_B : SOC_DPP_DBAL_SW_TABLE_ID_1Q_COMPRESSED_TPID2_SEM_B);
    program_property->vtt_table_index[2] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_TST2_TCAM);
    program_property->tcam_profile       = ARAD_PP_ISEM_ACCESS_TCAM_TST2_PROFILE;

    program_property->pd_bitmap_0              = (0x38); 
    program_property->pd_bitmap_1              = (0x0);


    if(SOC_IS_JERICHO(unit))
    {
        program_property->qual_to_ce_info.use_32_bit_ce[0][0] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[0][1] = 1;
    }
    else
    {
        program_property->implicit_flag = 1;

        program_property->qual_to_ce_info.qualifier_to_ce_id[0][0] = 6; 
        program_property->qual_to_ce_info.qualifier_to_ce_id[0][1] = 5;
        program_property->qual_to_ce_info.qualifier_to_ce_id[0][2] = 4; 

        program_property->qual_to_ce_info.qualifier_to_ce_id[1][0] = 6; 
        program_property->qual_to_ce_info.qualifier_to_ce_id[1][1] = 5;

        program_property->qual_to_ce_info.qualifier_to_ce_id[2][0] = 7; 
        program_property->qual_to_ce_info.qualifier_to_ce_id[2][1] = 6; 
        program_property->qual_to_ce_info.qualifier_to_ce_id[2][2] = 4; 
        program_property->qual_to_ce_info.qualifier_to_ce_id[2][3] = 3; 
        program_property->qual_to_ce_info.qualifier_to_ce_id[2][4] = 1; 
    }
}

void arad_pp_dbal_vt_program_outer_inner_tst2_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 tpid)
{
    program_property->nof_keys = 3;
    program_property->vtt_table_index[0] = SW_DB_DBAL2VTT((tpid == TPID1) ? SOC_DPP_DBAL_SW_TABLE_ID_QINQ_COMPRESSED_TPID1_SEM_A : SOC_DPP_DBAL_SW_TABLE_ID_QINQ_COMPRESSED_TPID2_SEM_A);
    program_property->vtt_table_index[1] = SW_DB_DBAL2VTT((tpid == TPID1) ? SOC_DPP_DBAL_SW_TABLE_ID_QINANY_TPID1_SEM_B : SOC_DPP_DBAL_SW_TABLE_ID_QINANY_TPID2_SEM_B);
    program_property->vtt_table_index[2] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_TST2_TCAM);
    program_property->tcam_profile       = ARAD_PP_ISEM_ACCESS_TCAM_TST2_PROFILE;

    program_property->pd_bitmap_0              = (0x38); 
    program_property->pd_bitmap_1              = (0x0);

    if(SOC_IS_JERICHO(unit))
    {
        program_property->qual_to_ce_info.use_32_bit_ce[0][0] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[0][1] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[0][2] = 1;
    }
    else
    {
        program_property->implicit_flag = 1;

        program_property->qual_to_ce_info.qualifier_to_ce_id[0][0] = 7;
        program_property->qual_to_ce_info.qualifier_to_ce_id[0][1] = 6;
        program_property->qual_to_ce_info.qualifier_to_ce_id[0][2] = 0;

        program_property->qual_to_ce_info.qualifier_to_ce_id[1][0] = 7;
        program_property->qual_to_ce_info.qualifier_to_ce_id[1][1] = 6;

        program_property->qual_to_ce_info.qualifier_to_ce_id[2][0] = 7; 
        program_property->qual_to_ce_info.qualifier_to_ce_id[2][1] = 6; 
        program_property->qual_to_ce_info.qualifier_to_ce_id[2][2] = 5; 
        program_property->qual_to_ce_info.qualifier_to_ce_id[2][3] = 4; 
        program_property->qual_to_ce_info.qualifier_to_ce_id[2][4] = 1; 
    }
}
void arad_pp_dbal_vt_program_outer_tst2_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 tpid)
{
    program_property->nof_keys = 3;
    program_property->vtt_table_index[0] = SW_DB_DBAL2VTT((tpid == TPID1) ? SOC_DPP_DBAL_SW_TABLE_ID_1Q_TPID1_SEM_A : SOC_DPP_DBAL_SW_TABLE_ID_1Q_TPID2_SEM_A);
    program_property->vtt_table_index[1] = SW_DB_DBAL2VTT((tpid == TPID1) ? SOC_DPP_DBAL_SW_TABLE_ID_1Q_COMPRESSED_TPID1_SEM_B : SOC_DPP_DBAL_SW_TABLE_ID_1Q_COMPRESSED_TPID2_SEM_B);
    program_property->vtt_table_index[2] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_TST2_TCAM);
    program_property->tcam_profile       = ARAD_PP_ISEM_ACCESS_TCAM_TST2_PROFILE;

    program_property->pd_bitmap_0              = (0x38); 
    program_property->pd_bitmap_1              = (0x0);

    if(SOC_IS_JERICHO(unit))
    {
        program_property->qual_to_ce_info.use_32_bit_ce[0][0] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[0][1] = 1;
    }
    else
    {
        program_property->implicit_flag = 1;

        program_property->qual_to_ce_info.qualifier_to_ce_id[0][0] = 7;
        program_property->qual_to_ce_info.qualifier_to_ce_id[0][1] = 6;

        program_property->qual_to_ce_info.qualifier_to_ce_id[1][0] = 6; 
        program_property->qual_to_ce_info.qualifier_to_ce_id[1][1] = 0;

        program_property->qual_to_ce_info.qualifier_to_ce_id[2][0] = 7; 
        program_property->qual_to_ce_info.qualifier_to_ce_id[2][1] = 6; 
        program_property->qual_to_ce_info.qualifier_to_ce_id[2][2] = 5; 
        program_property->qual_to_ce_info.qualifier_to_ce_id[2][3] = 4; 
        program_property->qual_to_ce_info.qualifier_to_ce_id[2][4] = 1; 
    }
}
void arad_pp_dbal_vt_program_untagged_tst2_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
    program_property->nof_keys = 2;
    program_property->vtt_table_index[0] = SW_DB_DBAL2VTT( SOC_DPP_DBAL_SW_TABLE_ID_UNTAGGED_SEM_A);
    program_property->vtt_table_index[1] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_TST2_TCAM);
    program_property->tcam_profile       = ARAD_PP_ISEM_ACCESS_TCAM_TST2_PROFILE;

    program_property->pd_bitmap_0              = (0x30); 
    program_property->pd_bitmap_1              = (0x0);

    if(!SOC_IS_JERICHO(unit))
    {

        program_property->implicit_flag = 1;

        program_property->qual_to_ce_info.qualifier_to_ce_id[0][0] = 7;
        program_property->qual_to_ce_info.qualifier_to_ce_id[0][1] = 6;

        program_property->qual_to_ce_info.qualifier_to_ce_id[1][0] = 7; 
        program_property->qual_to_ce_info.qualifier_to_ce_id[1][1] = 6; 
        program_property->qual_to_ce_info.qualifier_to_ce_id[1][2] = 5; 
        program_property->qual_to_ce_info.qualifier_to_ce_id[1][3] = 4; 
        program_property->qual_to_ce_info.qualifier_to_ce_id[1][4] = 1; 
    }
}


void arad_pp_dbal_vt_program_extender_pe_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 vid_type)
{
    program_property->nof_keys = 1;

    if (vid_type == INITIAL_VID) {
        program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_EXTENDER_PE_UT_SEM_A);
    } else {
        program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_EXTENDER_PE_SEM_A);
    }

    program_property->pd_bitmap_0              = (0x10); 
    program_property->pd_bitmap_1              = (0x00);

}


void arad_pp_dbal_vt_program_custom_pp_port_tunnel_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
    program_property->nof_keys = 2;

    program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_CUSTOM_PP_PORT_TUNNEL_SEMA);
    program_property->vtt_table_index[1] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_CUSTOM_PP_PORT_LSP_ECMP_TCAM);
    program_property->qual_to_ce_info.use_32_bit_ce[1][2] = 1;
    program_property->tcam_profile = ARAD_PP_ISEM_ACCESS_TCAM_CUSTOM_PP_LSP_ECMP_PROFILE;

    program_property->pd_bitmap_0              = (0x30); 
    program_property->pd_bitmap_1              = (0x00);

}

void arad_pp_dbal_vt_program_vlan_domain_mpls_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
    program_property->nof_keys = 3;
    program_property->vtt_table_index[0]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_A);
    program_property->vtt_table_index[2]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VLAN_DOMAIN_MPLS_L1_SEMB);

    program_property->tcam_profile         = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->vtt_table_index[1]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_OUTER_VID_TCAM);
    program_property->pd_bitmap_0          = (0x10);  
    program_property->pd_bitmap_1          = (0x8);  
  
    program_property->qual_to_ce_info.use_32_bit_ce[0][0] = 1;
    program_property->qual_to_ce_info.use_32_bit_ce[0][1] = 1;
    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        program_property->qual_to_ce_info.use_32_bit_ce[2][0] = 1;
    }

    if(g_prog_soc_prop[unit].mpls_1_database == ARAD_PP_ISEM_ACCESS_ID_ISEM_A)
    {
        arad_pp_dbal_vtt_program_property_swap( program_property );
        program_property->vtt_table_index[0]			 = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_B);
        program_property->vtt_table_index[2]			 = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VLAN_DOMAIN_MPLS_L1_SEMA);
    }


    if (g_prog_soc_prop->ingress_full_mymac_1) {
        program_property->tcam_profile                   = ARAD_PP_ISEM_ACCESS_TCAM_SRC_PORT_DA_DB_PROFILE;
        program_property->vtt_table_index[1]             = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_SRC_PORT_DA_INITIAL_VID_TCAM);
    }

}

void arad_pp_dbal_vt_program_pon_untagged_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
    program_property->implicit_flag = 1;
    program_property->nof_keys = 0;

    if (g_prog_soc_prop[unit].tls_db_enable)
    {
        program_property->vtt_table_index[program_property->nof_keys]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_PON_VDxTUNNEL_SEM_A);
        program_property->qual_to_ce_info.qualifier_to_ce_id[program_property->nof_keys][0] = SOC_IS_JERICHO(unit) ? 11 : 7; 
        program_property->qual_to_ce_info.qualifier_to_ce_id[program_property->nof_keys][1] = 1; 
        program_property->nof_keys++;
    }

    if (!g_prog_soc_prop[unit].custom_pon_enable)
    {
        program_property->vtt_table_index[program_property->nof_keys]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_PON_VDxTUNNEL_SEM_B);
        program_property->qual_to_ce_info.qualifier_to_ce_id[program_property->nof_keys][0] = SOC_IS_JERICHO(unit) ? 11 : 7; 
        program_property->qual_to_ce_info.qualifier_to_ce_id[program_property->nof_keys][1] = 1; 
        program_property->nof_keys++;
    }

    
    program_property->tcam_profile         = ARAD_PP_ISEM_ACCESS_TCAM_PON_EXTEND_LKP_DB_PROFILE;
    program_property->vtt_table_index[program_property->nof_keys]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_PON_VDxTUNNELxETHTYPE_TCAM);
    program_property->qual_to_ce_info.qualifier_to_ce_id[program_property->nof_keys][0] = SOC_IS_JERICHO(unit) ? 11 : 7; 
    program_property->qual_to_ce_info.qualifier_to_ce_id[program_property->nof_keys][1] = 1; 
    program_property->qual_to_ce_info.qualifier_to_ce_id[program_property->nof_keys][2] = 0; 
    program_property->nof_keys++;

#ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS(unit))
    {
        
        program_property->is_use_strength    = 1;
        if (!g_prog_soc_prop[unit].tls_in_tcam_enable && g_prog_soc_prop[unit].tls_db_enable) {
            program_property->pd_0_tcam_strength = 3;
            program_property->pd_0_isa_strength  = g_prog_soc_prop[unit].custom_pon_enable ? 1 : 2;
            program_property->pd_0_isb_strength  = g_prog_soc_prop[unit].custom_pon_enable ? 2 : 1;
            program_property->pd_1_tcam_strength = 0;
            program_property->pd_1_isa_strength  = 0;
            program_property->pd_1_isb_strength  = 0;
        }
        else
        {
            program_property->pd_0_tcam_strength = 1;
            program_property->pd_0_isa_strength  = g_prog_soc_prop[unit].custom_pon_enable ? 2 : 3;
            program_property->pd_0_isb_strength  = g_prog_soc_prop[unit].custom_pon_enable ? 3 : 2;
            program_property->pd_1_tcam_strength = 0;
            program_property->pd_1_isa_strength  = 0;
            program_property->pd_1_isb_strength  = 0;
        }
    }
    else
#endif
    {
        program_property->pd_bitmap_0          = (g_prog_soc_prop[unit].tls_in_tcam_enable) ? (0x14):((!g_prog_soc_prop[unit].tls_db_enable) ? 0x28:0x38); ; 
        program_property->pd_bitmap_1          = 0;
    }

}

void arad_pp_dbal_vt_program_pon_one_tag_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
    program_property->implicit_flag = 1;
    program_property->nof_keys = 0;

    program_property->vtt_table_index[program_property->nof_keys]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_PON_VDxTUNNELxOTUER_VID_SEM_A);
    program_property->qual_to_ce_info.qualifier_to_ce_id[program_property->nof_keys][0] = SOC_IS_JERICHO(unit) ? 11 : 7; 
    program_property->qual_to_ce_info.qualifier_to_ce_id[program_property->nof_keys][1] = SOC_IS_JERICHO(unit) ? 10 : 6; 
    program_property->qual_to_ce_info.qualifier_to_ce_id[program_property->nof_keys][2] = 1; 
    program_property->nof_keys++;

    if (!g_prog_soc_prop[unit].custom_pon_enable)
    {
        program_property->vtt_table_index[program_property->nof_keys]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_PON_VDxTUNNEL_SEM_B);
        program_property->qual_to_ce_info.qualifier_to_ce_id[program_property->nof_keys][0] = SOC_IS_JERICHO(unit) ? 11 : 7; 
        program_property->qual_to_ce_info.qualifier_to_ce_id[program_property->nof_keys][1] = SOC_IS_JERICHO(unit) ? 10 : 6; 
        program_property->nof_keys++;
    }

    if (!soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "gpon_application_enable", 0)) {
        
        program_property->tcam_profile         = ARAD_PP_ISEM_ACCESS_TCAM_PON_EXTEND_LKP_DB_PROFILE;
        program_property->vtt_table_index[program_property->nof_keys]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_PON_VDxTUNNELxETHTYPExPCPxOUTER_VID_TCAM);
        program_property->qual_to_ce_info.qualifier_to_ce_id[program_property->nof_keys][0] = SOC_IS_JERICHO(unit) ? 11 : 7; 
        program_property->qual_to_ce_info.qualifier_to_ce_id[program_property->nof_keys][1] = SOC_IS_JERICHO(unit) ? 10 : 6; 
        program_property->qual_to_ce_info.qualifier_to_ce_id[program_property->nof_keys][2] = 2; 
        program_property->qual_to_ce_info.qualifier_to_ce_id[program_property->nof_keys][3] = 1; 
        program_property->qual_to_ce_info.qualifier_to_ce_id[program_property->nof_keys][4] = 0; 
        program_property->nof_keys++;
    }

  if (!soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "gpon_application_enable", 0)) {
#ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS(unit))
    {

        
        program_property->is_use_strength = 1;
        if (g_prog_soc_prop[unit].tls_in_tcam_enable) {
            program_property->pd_0_tcam_strength = 1;
            program_property->pd_0_isa_strength = g_prog_soc_prop[unit].custom_pon_enable ? 2 : 3;
            program_property->pd_0_isb_strength = g_prog_soc_prop[unit].custom_pon_enable ? 3 : 2;
        } else {
            program_property->pd_0_tcam_strength = 3;
            program_property->pd_0_isa_strength = g_prog_soc_prop[unit].custom_pon_enable ? 1 : 2;
            program_property->pd_0_isb_strength = g_prog_soc_prop[unit].custom_pon_enable ? 2 : 1;
        }

        program_property->pd_1_tcam_strength = 0;
        program_property->pd_1_isa_strength  = 0;
        program_property->pd_1_isb_strength  = 0;

    }
    else
#endif
    {
        program_property->pd_bitmap_0 = (g_prog_soc_prop[unit].tls_in_tcam_enable ? 0x1c:0x38); 
        program_property->pd_bitmap_1 = (0x0);
    }
  } else {
      program_property->pd_bitmap_0        = (0x18); 
      program_property->pd_bitmap_1        = (0x0);
  }

}

void arad_pp_dbal_vt_program_pon_two_tags_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
    program_property->implicit_flag = 1;
    program_property->nof_keys = 3;

    if (g_prog_soc_prop[unit].custom_pon_enable)
    {
        
        program_property->vtt_table_index[0]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_PON_VDxTUNNELxOTUER_VID_SEM_A);
        program_property->qual_to_ce_info.qualifier_to_ce_id[0][0] = SOC_IS_JERICHO(unit) ? 11 : 7; 
        program_property->qual_to_ce_info.qualifier_to_ce_id[0][1] = SOC_IS_JERICHO(unit) ? 10 : 6; 
        program_property->qual_to_ce_info.qualifier_to_ce_id[0][2] = 2; 

        program_property->vtt_table_index[1]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_PON_VDxTUNNELxOTUER_VIDxINNER_VID_SEM_B);
        program_property->qual_to_ce_info.qualifier_to_ce_id[1][0] = SOC_IS_JERICHO(unit) ? 11 : 7; 
        program_property->qual_to_ce_info.qualifier_to_ce_id[1][1] = SOC_IS_JERICHO(unit) ? 10 : 6; 
        program_property->qual_to_ce_info.qualifier_to_ce_id[1][2] = 2; 
        program_property->qual_to_ce_info.qualifier_to_ce_id[1][3] = 0; 
    }
    else
    {
        program_property->vtt_table_index[0]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_PON_VDxTUNNELxOTUER_VIDxINNER_VID_SEM_A);
        program_property->qual_to_ce_info.qualifier_to_ce_id[0][0] = SOC_IS_JERICHO(unit) ? 11 : 7; 
        program_property->qual_to_ce_info.qualifier_to_ce_id[0][1] = SOC_IS_JERICHO(unit) ? 10 : 6; 
        program_property->qual_to_ce_info.qualifier_to_ce_id[0][2] = 2; 
        program_property->qual_to_ce_info.qualifier_to_ce_id[0][3] = 0; 

        program_property->vtt_table_index[1]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_PON_VDxTUNNELxOTUER_VID_SEM_B);
        program_property->qual_to_ce_info.qualifier_to_ce_id[1][0] = SOC_IS_JERICHO(unit) ? 11 : 7; 
        program_property->qual_to_ce_info.qualifier_to_ce_id[1][1] = SOC_IS_JERICHO(unit) ? 10 : 6; 
        program_property->qual_to_ce_info.qualifier_to_ce_id[1][2] = 2; 
    }
    
    program_property->tcam_profile         = ARAD_PP_ISEM_ACCESS_TCAM_PON_EXTEND_LKP_DB_PROFILE;
    program_property->vtt_table_index[2]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_PON_VDxTUNNELxETHTYPExPCPxOUTER_VIDxINNER_VID_TCAM);
    program_property->qual_to_ce_info.qualifier_to_ce_id[2][0] = SOC_IS_JERICHO(unit) ? 11 : 7; 
    program_property->qual_to_ce_info.qualifier_to_ce_id[2][1] = SOC_IS_JERICHO(unit) ? 10 : 6; 
    program_property->qual_to_ce_info.qualifier_to_ce_id[2][2] = 3; 
    program_property->qual_to_ce_info.qualifier_to_ce_id[2][3] = 2; 
    program_property->qual_to_ce_info.qualifier_to_ce_id[2][4] = 1; 
    program_property->qual_to_ce_info.qualifier_to_ce_id[2][5] = 0; 

#ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS(unit))
    {
        
        program_property->is_use_strength = 1;
        if (g_prog_soc_prop[unit].tls_in_tcam_enable) {
            program_property->pd_0_tcam_strength = 1;
            program_property->pd_0_isa_strength  = g_prog_soc_prop[unit].custom_pon_enable ? 2 : 3;
            program_property->pd_0_isb_strength  = g_prog_soc_prop[unit].custom_pon_enable ? 3 : 2;
        } else {
            program_property->pd_0_tcam_strength = 3;
            program_property->pd_0_isa_strength  = g_prog_soc_prop[unit].custom_pon_enable ? 1 : 2;
            program_property->pd_0_isb_strength  = g_prog_soc_prop[unit].custom_pon_enable ? 2 : 1;
        }

        program_property->pd_1_tcam_strength = 0;
        program_property->pd_1_isa_strength  = 0;
        program_property->pd_1_isb_strength  = 0;
    }
    else
#endif
    {
        program_property->pd_bitmap_0 = (g_prog_soc_prop[unit].tls_in_tcam_enable ? 0x1c:0x38); 
        program_property->pd_bitmap_1 = (0x0);
    }

}

void arad_pp_dbal_vt_program_pon_two_tags_vs_tunnel_id_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
    program_property->implicit_flag = 1;
    program_property->nof_keys = 0;

    
    if(soc_property_suffix_num_get(unit,-1,spn_CUSTOM_FEATURE,"pon_lif_lkup",0))
    {
        program_property->vtt_table_index[program_property->nof_keys]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_PON_VDxTUNNELxOTUER_VID_SEM_A);
        program_property->qual_to_ce_info.qualifier_to_ce_id[program_property->nof_keys][0] = SOC_IS_JERICHO(unit) ? 11 : 7; 
        program_property->qual_to_ce_info.qualifier_to_ce_id[program_property->nof_keys][1] = SOC_IS_JERICHO(unit) ? 10 : 6; 
        program_property->qual_to_ce_info.qualifier_to_ce_id[program_property->nof_keys][2] = 2; 
        program_property->nof_keys++;
    }
    else
    {
        if (!g_prog_soc_prop[unit].custom_pon_enable)
        {
            program_property->vtt_table_index[program_property->nof_keys]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_PON_VDxTUNNELxOTUER_VIDxINNER_VID_SEM_A);
            program_property->qual_to_ce_info.qualifier_to_ce_id[program_property->nof_keys][0] = SOC_IS_JERICHO(unit) ? 11 : 7; 
            program_property->qual_to_ce_info.qualifier_to_ce_id[program_property->nof_keys][1] = SOC_IS_JERICHO(unit) ? 10 : 6; 
            program_property->qual_to_ce_info.qualifier_to_ce_id[program_property->nof_keys][2] = 2; 
            program_property->qual_to_ce_info.qualifier_to_ce_id[program_property->nof_keys][3] = 0; 
            program_property->nof_keys++;
        }
    }

    
    if(soc_property_suffix_num_get(unit,-1,spn_CUSTOM_FEATURE,"pon_lif_lkup",0))
    {
        program_property->vtt_table_index[program_property->nof_keys]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_PON_VDxTUNNEL_SEM_B);
        program_property->qual_to_ce_info.qualifier_to_ce_id[program_property->nof_keys][0] = SOC_IS_JERICHO(unit) ? 11 : 7; 
        program_property->qual_to_ce_info.qualifier_to_ce_id[program_property->nof_keys][1] = SOC_IS_JERICHO(unit) ? 10 : 6; 
    }
    else
    {
        if (g_prog_soc_prop[unit].custom_pon_enable)
        {
            program_property->vtt_table_index[program_property->nof_keys]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_PON_VDxTUNNELxOTUER_VIDxINNER_VID_SEM_B);
            program_property->qual_to_ce_info.qualifier_to_ce_id[program_property->nof_keys][0] = SOC_IS_JERICHO(unit) ? 11 : 7; 
            program_property->qual_to_ce_info.qualifier_to_ce_id[program_property->nof_keys][1] = SOC_IS_JERICHO(unit) ? 10 : 6; 
            program_property->qual_to_ce_info.qualifier_to_ce_id[program_property->nof_keys][2] = 2; 
            program_property->qual_to_ce_info.qualifier_to_ce_id[program_property->nof_keys][3] = 0; 
        }
        else
        {
            program_property->vtt_table_index[program_property->nof_keys]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_PON_VDxTUNNEL_SEM_B);
            program_property->qual_to_ce_info.qualifier_to_ce_id[program_property->nof_keys][0] = SOC_IS_JERICHO(unit) ? 11 : 7; 
            program_property->qual_to_ce_info.qualifier_to_ce_id[program_property->nof_keys][1] = SOC_IS_JERICHO(unit) ? 10 : 6; 
        }
    }
    program_property->nof_keys++;

    
    program_property->tcam_profile         = ARAD_PP_ISEM_ACCESS_TCAM_PON_EXTEND_LKP_DB_PROFILE;
    program_property->vtt_table_index[program_property->nof_keys]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_PON_VDxTUNNELxETHTYPExPCPxOUTER_VIDxINNER_VID_TCAM);
    program_property->qual_to_ce_info.qualifier_to_ce_id[program_property->nof_keys][0] = SOC_IS_JERICHO(unit) ? 11 : 7; 
    program_property->qual_to_ce_info.qualifier_to_ce_id[program_property->nof_keys][1] = SOC_IS_JERICHO(unit) ? 10 : 6; 
    program_property->qual_to_ce_info.qualifier_to_ce_id[program_property->nof_keys][2] = 3; 
    program_property->qual_to_ce_info.qualifier_to_ce_id[program_property->nof_keys][3] = 2; 
    program_property->qual_to_ce_info.qualifier_to_ce_id[program_property->nof_keys][4] = 1; 
    program_property->qual_to_ce_info.qualifier_to_ce_id[program_property->nof_keys][5] = 0; 
    program_property->nof_keys++;

#ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS(unit))
    {
        
        program_property->is_use_strength = 1;
        if(soc_property_suffix_num_get(unit,-1,spn_CUSTOM_FEATURE,"pon_lif_lkup",0)){
             program_property->pd_0_tcam_strength = 3;
             program_property->pd_0_isa_strength = 2;
             program_property->pd_0_isb_strength = 1;
        }else if (g_prog_soc_prop[unit].tls_in_tcam_enable) {
             program_property->pd_0_tcam_strength = 1;
             program_property->pd_0_isa_strength = g_prog_soc_prop[unit].custom_pon_enable ? 2 : 3;
             program_property->pd_0_isb_strength = g_prog_soc_prop[unit].custom_pon_enable ? 3 : 2;
         } else {
             program_property->pd_0_tcam_strength = 3;
             program_property->pd_0_isa_strength = g_prog_soc_prop[unit].custom_pon_enable ? 1 : 2;
             program_property->pd_0_isb_strength = g_prog_soc_prop[unit].custom_pon_enable ? 2 : 1;
         }

        program_property->pd_1_tcam_strength = 0;
        program_property->pd_1_isa_strength  = 0;
        program_property->pd_1_isb_strength  = 0;
    }
    else
#endif
    {
        program_property->pd_bitmap_0 = (g_prog_soc_prop[unit].tls_in_tcam_enable ? 0x1c:0x38); 
        program_property->pd_bitmap_1 = (0x0);
    }

}


void arad_pp_dbal_vt_program_section_oam_one_tag_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
    program_property->implicit_flag = 1;
    program_property->nof_keys = 0;

    
    program_property->nof_keys++;
    program_property->tcam_profile         = ARAD_PP_ISEM_ACCESS_TCAM_SECTION_OAM_PROFILE;
    program_property->vtt_table_index[0]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_SECTION_OAM_VDxOTUER_VIDxETH_TCAM);
    program_property->qual_to_ce_info.qualifier_to_ce_id[0][0] = SOC_IS_JERICHO(unit) ? 11 : 7; 
    program_property->qual_to_ce_info.qualifier_to_ce_id[0][1] = 1; 
    program_property->qual_to_ce_info.qualifier_to_ce_id[0][2] = 0; 
    program_property->pd_bitmap_0 = 0x20; 
    program_property->pd_bitmap_1 = (0x0);

}

void arad_pp_dbal_vt_program_section_oam_two_tags_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{

    program_property->implicit_flag = 1;
    program_property->nof_keys = 1;

    
    program_property->tcam_profile         = ARAD_PP_ISEM_ACCESS_TCAM_SECTION_OAM_DOU_TAGS_PROFILE;
    program_property->vtt_table_index[0]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_SECTION_OAM_VDxOTUER_VIDxINNER_VIDxETH_TCAM);
    program_property->qual_to_ce_info.qualifier_to_ce_id[0][0] =  SOC_IS_JERICHO(unit) ? 11 : 7; 
    program_property->qual_to_ce_info.qualifier_to_ce_id[0][1] = SOC_IS_JERICHO(unit) ? 10 : 6; ; 
    program_property->qual_to_ce_info.qualifier_to_ce_id[0][2] = 1; 
    program_property->qual_to_ce_info.qualifier_to_ce_id[0][3] = 0; 
    program_property->pd_bitmap_0 = 0x20; 
    program_property->pd_bitmap_1 = (0x0);

}

void arad_pp_dbal_vt_program_3mtse_plus_frr_vdxinitialvid_l2l1_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 mpls_label )
{
    program_property->nof_keys = 3;
    if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 20) {
    	program_property->vtt_table_index[0]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxINITIALVID_SEM_A);
        program_property->vtt_table_index[1]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L12IDX_SEM_B);
        program_property->pd_bitmap_0          = (0x10);  
        program_property->pd_bitmap_1          = (0x8);  
   }
    if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 21) {
        program_property->vtt_table_index[0]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxINITIALVID_SEM_B);
        program_property->vtt_table_index[1]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L12IDX_SEM_A);
        program_property->pd_bitmap_0          = (0x08);  
        program_property->pd_bitmap_1          = (0x10);  }

    if(g_prog_soc_prop[unit].fast_reroute_labels_enable) {
    	program_property->tcam_profile         = ARAD_PP_ISEM_ACCESS_TCAM_MPLS_KEY_0_PROFILE;
        program_property->vtt_table_index[2]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_MPLS4_INITIAL_VID_L1_TCAM);
        program_property->qual_to_ce_info.use_32_bit_ce[2][0] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[2][1] = 1;
    }
    else {
        program_property->vtt_table_index[2]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_INITIAL_VID_TCAM);
    }

    program_property->processing_profile = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_3MTSE_PLUS_FRR_TCAM;
}

void arad_pp_dbal_vt_program_3mtse_plus_frr_vdxOuterVid_l2l1_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 mpls_label )
{
    program_property->nof_keys = 3;
    if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 20) {
    	program_property->vtt_table_index[0]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_A);
    	program_property->vtt_table_index[1]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L12IDX_SEM_B);
    	program_property->pd_bitmap_0          = (0x10);  
    	program_property->pd_bitmap_1          = (0x8);  
    }
    if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 21) {
       	program_property->vtt_table_index[0]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_B);
      	program_property->vtt_table_index[1]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L12IDX_SEM_A);
        program_property->pd_bitmap_0          = (0x8);  
        program_property->pd_bitmap_1          = (0x10);  
    }

    if(g_prog_soc_prop[unit].fast_reroute_labels_enable) {
        program_property->tcam_profile         = ARAD_PP_ISEM_ACCESS_TCAM_MPLS_KEY_0_PROFILE;
        program_property->vtt_table_index[2]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_MPLS4_OUTER_VID_L1_TCAM);
        program_property->qual_to_ce_info.use_32_bit_ce[2][0] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[2][1] = 1;
    }
    else {
        program_property->vtt_table_index[2]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_OUTER_VID_TCAM);
    }

    program_property->processing_profile = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_3MTSE_PLUS_FRR_TCAM;
    program_property->key_program_variable = (0x2); 
}

void arad_pp_dbal_vt_program_3mtse_plus_frr_vdxinitialvid_l2_l4b_l5g_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 mpls_label )
{
    program_property->nof_keys = 3;
    if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 20) {
        program_property->vtt_table_index[0]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxINITIALVID_SEM_A);
        program_property->vtt_table_index[1]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L12IDX_SEM_B);
        program_property->pd_bitmap_0          = (0x10);  
        program_property->pd_bitmap_1          = (0x8);  
    }
    if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 21) {

    	program_property->vtt_table_index[0]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxINITIALVID_SEM_B);
        program_property->vtt_table_index[1]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L12IDX_SEM_A);
        program_property->pd_bitmap_0          = (0x8);  
        program_property->pd_bitmap_1          = (0x10);  
    }

    program_property->tcam_profile         = ARAD_PP_ISEM_ACCESS_TCAM_L4B_L5L_PROFILE;
    program_property->vtt_table_index[2]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_INITIAL_VID_L4B_L5G_TCAM);

    program_property->processing_profile = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_3MTSE_PLUS_FRR_L4B_AND_L5G;
}

void arad_pp_dbal_vt_program_3mtse_plus_frr_vdxOuterVid_l2_l4b_l5g_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 mpls_label )
{
    program_property->nof_keys = 3;
    if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 20) {
    	program_property->vtt_table_index[0]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_A);
    	program_property->vtt_table_index[1]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L12IDX_SEM_B);
    	program_property->pd_bitmap_0          = (0x10);  
    	program_property->pd_bitmap_1          = (0x8);  
    }
    if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 21) {

     	program_property->vtt_table_index[0]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_B);
        program_property->vtt_table_index[1]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L12IDX_SEM_A);
        program_property->pd_bitmap_0          = (0x8);  
        program_property->pd_bitmap_1          = (0x10);  
    }

    program_property->tcam_profile         = ARAD_PP_ISEM_ACCESS_TCAM_L4B_L5L_PROFILE;
    program_property->vtt_table_index[2]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_OUTER_VID_L4B_L5G_TCAM);
    program_property->processing_profile = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_3MTSE_PLUS_FRR_L4B_AND_L5G;
    program_property->key_program_variable = (0x2); 
}


void arad_pp_dbal_vt_program_2mtse_plus_frr_vid_frr_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 vid_type)
{
    program_property->nof_keys = 3;

    if(vid_type == INITIAL_VID)
    {
    	if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 22) {
           program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxINITIALVID_SEM_A);
           program_property->vtt_table_index[1] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxINITIALVID_SEM_B);
           program_property->pd_bitmap_0        = (0x10);
       	   program_property->pd_bitmap_1        = (0x20);
        }
        if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 23) {
            program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxINITIALVID_SEM_B);
            program_property->vtt_table_index[1] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxINITIALVID_SEM_A);
            program_property->pd_bitmap_0        = (0x8);
        	program_property->pd_bitmap_1        = (0x20);
        }

        if(g_prog_soc_prop[unit].fast_reroute_labels_enable) {
            program_property->vtt_table_index[2] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_MPLS4_INITIAL_VID_L1_TCAM);
            program_property->tcam_profile       = ARAD_PP_ISEM_ACCESS_TCAM_MPLS_KEY_0_PROFILE;
            program_property->qual_to_ce_info.use_32_bit_ce[2][0] = 1;
            program_property->qual_to_ce_info.use_32_bit_ce[2][1] = 1;
            program_property->qual_to_ce_info.use_32_bit_ce[2][2] = 1;
        }
        else {
        	program_property->vtt_table_index[2] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_INITIAL_VID_TCAM);
        }
    }

    if(vid_type == OUTER_VID)
    {

    	if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 22) {
    		program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_A);
            program_property->vtt_table_index[1] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxINITIALVID_SEM_B);
            program_property->pd_bitmap_0        = (0x10);
        	program_property->pd_bitmap_1        = (0x20);
    	}
    	if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 23) {
       		program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_B);
            program_property->vtt_table_index[1] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxINITIALVID_SEM_A);
            program_property->pd_bitmap_0        = (0x08);
        	program_property->pd_bitmap_1        = (0x20);
    	}

        if(g_prog_soc_prop[unit].fast_reroute_labels_enable) {
            program_property->vtt_table_index[2] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_MPLS4_OUTER_VID_L1_TCAM);
            program_property->tcam_profile       = ARAD_PP_ISEM_ACCESS_TCAM_MPLS_KEY_0_PROFILE;
            program_property->qual_to_ce_info.use_32_bit_ce[2][0] = 1;
            program_property->qual_to_ce_info.use_32_bit_ce[2][1] = 1;
            program_property->qual_to_ce_info.use_32_bit_ce[2][2] = 1;
        }
        else {
           program_property->vtt_table_index[2] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_OUTER_VID_TCAM);
        }
    }

    if(vid_type == OUTER_INNER_VID)
    {
        if(SOC_IS_JERICHO(unit))
        {
        	program_property->pd_bitmap_0        = (0x18);
        	program_property->pd_bitmap_1        = (0x20);

        	if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 22) {
                program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_A);
                program_property->vtt_table_index[1] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTER_INNER_VID_SEM_B);
        	}
        	if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 23) {
                program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_B);
                program_property->vtt_table_index[1] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTER_INNER_VID_SEM_A);
        	}

            if(g_prog_soc_prop[unit].fast_reroute_labels_enable) {
                program_property->vtt_table_index[2] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_MPLS4_OUTER_VID_L1_TCAM);
                program_property->tcam_profile       = ARAD_PP_ISEM_ACCESS_TCAM_MPLS_KEY_0_PROFILE;
                program_property->qual_to_ce_info.use_32_bit_ce[2][0] = 1;
                program_property->qual_to_ce_info.use_32_bit_ce[2][1] = 1;
                program_property->qual_to_ce_info.use_32_bit_ce[2][2] = 1;
            }
            else {
               program_property->vtt_table_index[2] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_OUTER_VID_TCAM);
            }
        }
    }

    if(vid_type == OUTER_INNER_VID_PCP)
        {
            if(SOC_IS_JERICHO(unit))
            {
            	program_property->pd_bitmap_0        = (0x18);
            	program_property->pd_bitmap_1        = (0x20);

                if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 22) {
                	program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_A);
                	program_property->vtt_table_index[1] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTER_INNER_VID_SEM_B);
                }
                if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 23) {
                	program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_B);
                	program_property->vtt_table_index[1] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTER_INNER_VID_SEM_A);
                }

                if(g_prog_soc_prop[unit].fast_reroute_labels_enable) {
                      program_property->vtt_table_index[2] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_MPLS4_OUTER_VID_L1_TCAM);
                      program_property->tcam_profile       = ARAD_PP_ISEM_ACCESS_TCAM_MPLS_KEY_0_PROFILE;
                      program_property->qual_to_ce_info.use_32_bit_ce[2][0] = 1;
                      program_property->qual_to_ce_info.use_32_bit_ce[2][1] = 1;
                      program_property->qual_to_ce_info.use_32_bit_ce[2][2] = 1;
                }
                else {
                    program_property->vtt_table_index[2] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_OUTER_VID_TCAM);
                }
            }
        }

    program_property->key_program_variable = (0x2); 
    program_property->processing_profile = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_2MTSE_PLUS_FRR_TCAM;
}


void arad_pp_dbal_vt_program_generalized_rch_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 is_rch0)
{
    program_property->nof_keys = 1;

    
    program_property->tcam_profile         = ARAD_PP_ISEM_ACCESS_TCAM_GENERALIZED_RCH_PROFILE;
    program_property->vtt_table_index[0]   = is_rch0 ? SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_GENERALIZED_RCH0_TCAM) 
                                                     : SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_GENERALIZED_RCH1_TCAM);
    program_property->pd_bitmap_0 = 0x20; 
    program_property->pd_bitmap_1 = (0x0);
}



uint32  arad_pp_dbal_tt_program_set(int unit){

    ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  program_property;
    ARAD_PP_DBAL_VTT_SW_DB_PROPERTY    sw_db_property;
    SOC_DPP_DBAL_KEY_TO_TABLE          keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] = {{0}};

    int sw_db_index;
    int res;
    int i,j;
    int ati_for_dbal = SOC_DPP_DBAL_ATI_NONE;
    int tcam_db_id = SOC_DPP_DBAL_ATI_NONE;
    int nof_keys_to_create;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    for(i=0; i<ARAD_PP_DBAL_TT_NOF_PROGRAMS; i++)
    {
        
        if(tt_dbal_programs[i].prog_used[unit] != -1)
        {
            
            PROGRAM_PROPERTY_CLEAR(&program_property);
            
            (tt_dbal_programs[i].vtt_program_property_set)(unit, &program_property,tt_dbal_programs[i].additional_info);

            if (program_property.tcam_profile == ARAD_PP_TCAM_DYNAMIC_ACCESS_ID_BASE) {
                
                ati_for_dbal = SOC_DPP_DBAL_ATI_NONE;
                tcam_db_id = 0;                
            }else if(program_property.tcam_profile != ARAD_TCAM_ACCESS_PROFILE_INVALID)
            {
                ati_for_dbal = SOC_DPP_DBAL_ATI_TCAM_STATIC;
                tcam_db_id = program_property.tcam_profile;                 
            }else{
                ati_for_dbal = SOC_DPP_DBAL_ATI_TCAM_DUMMY;
            }

            if(program_property.nof_dbs == 0)
            {
               nof_keys_to_create = program_property.nof_keys;
            }
            else
            {
               nof_keys_to_create = program_property.nof_dbs;
            }

            
            for(j=0; j<nof_keys_to_create; j++)
            {
                
                sw_db_index = program_property.vtt_table_index[j];

                
                if(!(vtt_tables[sw_db_index].valid_indication[unit]))
                {
                    
                    SW_DB_PROPERTY_CLEAR(&sw_db_property);
                    sw_db_property.prefix_len = ARAD_PP_ISEM_ACCESS_NOF_BITS_PREFIX; 

                    
                    vtt_tables[sw_db_index].valid_indication[unit] = 1;
                    
                    vtt_tables[sw_db_index].vtt_table_property_set(unit, &sw_db_property,vtt_tables[sw_db_index].table_additional_info);

                    if (vtt_tables[sw_db_index].mem_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_TCAM) {
                        if(ati_for_dbal == SOC_DPP_DBAL_ATI_TCAM_STATIC){
                            sw_db_property.prefix = tcam_db_id;                            
                        }
                    }

                    
                    res = arad_pp_dbal_table_create(unit, vtt_tables[sw_db_index].dbal_sw_db_id,
                                                          sw_db_property.prefix,
                                                          sw_db_property.prefix_len,
                                                          vtt_tables[sw_db_index].mem_type,
                                                          sw_db_property.nof_qulifiers,
                                                          ati_for_dbal,
                                                          sw_db_property.qual_info,
                                                          vtt_tables[sw_db_index].name);
                   SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);
                }

                
                keys_to_table_id[j].key_id      = MEM_TYPE_TO_KEY(vtt_tables[sw_db_index].mem_type);
                keys_to_table_id[j].sw_table_id = vtt_tables[sw_db_index].dbal_sw_db_id;
            }

            if(program_property.nof_keys != 0)
            {
                if(program_property.implicit_flag)
                {
                    
                    res = arad_pp_dbal_program_to_tables_associate_implicit(unit,
                                                                            tt_dbal_programs[i].prog_used[unit],
                                                                            SOC_PPC_FP_DATABASE_STAGE_INGRESS_TT,
                                                                            keys_to_table_id,
                                                                            program_property.qual_to_ce_info.qualifier_to_ce_id,
                                                                            program_property.nof_keys);
                }
                else
                {
                    
                    res = arad_pp_dbal_program_to_tables_associate(unit,
                                                                   tt_dbal_programs[i].prog_used[unit],
                                                                   SOC_PPC_FP_DATABASE_STAGE_INGRESS_TT,
                                                                   keys_to_table_id,
                                                                   program_property.qual_to_ce_info.use_32_bit_ce,
                                                                   program_property.nof_keys);
                }
                SOC_SAND_CHECK_FUNC_RESULT(res, 200 + i, exit);
            }

            
            res = arad_pp_dbal_tt_program_property_set(unit, &program_property, tt_dbal_programs[i].prog_used[unit]);
            SOC_SAND_CHECK_FUNC_RESULT(res, 300 + i, exit);

        }

    }
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_dbal_tt_program_set()", 0, 0);
}


uint32 arad_pp_dbal_tt_program_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 program )
{
    uint32 res;
    uint32 mem_tfl = 0;
    uint32 temp_tfl = 0;

    ARAD_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_DATA  ihp_vtt2nd_key_construction_tbl_data;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    ARAD_CLEAR(&ihp_vtt2nd_key_construction_tbl_data, ARAD_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_DATA, 1);

    ihp_vtt2nd_key_construction_tbl_data.dbal = 1;
    res = arad_pp_ihp_vtt2nd_key_construction_tbl_get_unsafe(unit, program, &ihp_vtt2nd_key_construction_tbl_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, program*10, exit);

	
    ihp_vtt2nd_key_construction_tbl_data.termination_0_pd_bitmap = program_property->pd_bitmap_0;
    ihp_vtt2nd_key_construction_tbl_data.termination_1_pd_bitmap = program_property->pd_bitmap_1;
    ihp_vtt2nd_key_construction_tbl_data.processing_profile = program_property->processing_profile;
    ihp_vtt2nd_key_construction_tbl_data.key_program_variable = program_property->key_program_variable;

    ihp_vtt2nd_key_construction_tbl_data.isa_key_initial_from_vt  = program_property->isa_key_initial_from_vt;
    ihp_vtt2nd_key_construction_tbl_data.isb_key_initial_from_vt  = program_property->isb_key_initial_from_vt;
    ihp_vtt2nd_key_construction_tbl_data.tcam_key_initial_from_vt = program_property->tcam_key_initial_from_vt;
    ihp_vtt2nd_key_construction_tbl_data.processing_code       = program_property->processing_code;
    ihp_vtt2nd_key_construction_tbl_data.result_to_use_0         = program_property->result_to_use_0;
    ihp_vtt2nd_key_construction_tbl_data.result_to_use_1         = program_property->result_to_use_1;
    ihp_vtt2nd_key_construction_tbl_data.result_to_use_2         = program_property->result_to_use_2;
    ihp_vtt2nd_key_construction_tbl_data.second_stage_parsing    = program_property->second_stage_parsing;

    
    if (SOC_IS_JERICHO(unit) && (program_property->terminate_first_label)) {
        res = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_mpls_termination_first_label_bit_map_get,(unit, &mem_tfl)));
        SOC_SAND_CHECK_FUNC_RESULT(res, program*11, exit);
        temp_tfl = (0x1 << program);
        mem_tfl = mem_tfl | temp_tfl;
        res = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_mpls_termination_first_label_bit_map_set,(unit, mem_tfl)));
        SOC_SAND_CHECK_FUNC_RESULT(res, program*11+1, exit);
    }

    ihp_vtt2nd_key_construction_tbl_data.dbal = 1;
    res = arad_pp_ihp_vtt2nd_key_construction_tbl_set_unsafe( unit, program, &ihp_vtt2nd_key_construction_tbl_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, program*10+1, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_dbal_tt_program_property_set()", 0, 0);
}

void arad_pp_dbal_tt_program_tm_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused )
{
    program_property->nof_keys = 0;
    program_property->processing_code    = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_TM;
    program_property->processing_profile = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE;
    program_property->tcam_profile       = ARAD_TCAM_ACCESS_PROFILE_INVALID;

}

void arad_pp_dbal_tt_program_oam_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
    program_property->nof_keys = 2;

    program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_OAM_STAT_SEM_A);
    program_property->vtt_table_index[1] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_OAM_STAT_SEM_B);

    program_property->qual_to_ce_info.use_32_bit_ce[0][0] = 1;
    program_property->qual_to_ce_info.use_32_bit_ce[1][0] = 1;


    program_property->tcam_profile = ARAD_TCAM_ACCESS_PROFILE_INVALID;

    program_property->pd_bitmap_0 = (0x10);
    program_property->pd_bitmap_1 = (0x8);

    program_property->processing_code = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_TM;
    program_property->processing_profile = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE;
}

void arad_pp_dbal_tt_program_bfd_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
    program_property->nof_keys = 2;

    program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_BFD_STAT_SEM_A);
    program_property->vtt_table_index[1] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_BFD_STAT_SEM_B);
    program_property->tcam_profile = ARAD_TCAM_ACCESS_PROFILE_INVALID;

    program_property->pd_bitmap_0 = (0x10);
    program_property->pd_bitmap_1 = (0x8);

    program_property->processing_code = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_TM;
    program_property->processing_profile = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE;
}

void arad_pp_dbal_tt_program_unindexed_mpls_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 mpls_label )
{
    program_property->nof_keys = 1;

    program_property->vtt_table_index[0] = (mpls_label == MPLS_LABEL_1) ? SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_UNINDEXED_SEM_B) : SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_UNINDEXED_SEM_B);
    program_property->pd_bitmap_0     = 0x8;
    program_property->pd_bitmap_1     = 0x0;
    program_property->tcam_profile    = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->processing_code = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_MPLS;
    program_property->result_to_use_0 = 1;
    program_property->result_to_use_1 = 2;
    program_property->result_to_use_2 = 0;
    program_property->second_stage_parsing = 1;
    program_property->processing_profile   = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE;

    if(g_prog_soc_prop[unit].mpls_1_database == ARAD_PP_ISEM_ACCESS_ID_ISEM_A)
    {
        arad_pp_dbal_vtt_program_property_swap(program_property );
        program_property->vtt_table_index[0]    -= 1;
    }

}

void arad_pp_dbal_tt_program_mldp_overlay_after_rcy_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused )
{
    program_property->nof_keys = 1;

    program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_DUMMY_UNINDEXED_SEM_A) ;
    program_property->pd_bitmap_0     = 0x10;
    program_property->pd_bitmap_1     = 0x0;
    program_property->tcam_profile    = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->processing_code = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_MPLS;
    program_property->result_to_use_0 = 2;
    program_property->result_to_use_1 = 0;
    program_property->result_to_use_2 = 0;
    program_property->second_stage_parsing = 1;
    program_property->processing_profile   = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE;

}


void arad_pp_dbal_tt_program_indexed_mpls_l1_l1_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 is_gal)
{

    program_property->nof_keys = 2;
    program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L1IDX_SEM_A);
    program_property->vtt_table_index[0] += 2*(g_prog_soc_prop[unit].mpls_1_namespace);

    if(is_gal)
        program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_GAL_SEM_B);
    else
    {
        program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L1IDX_SEM_B);
        program_property->vtt_table_index[1] += 2*(g_prog_soc_prop[unit].mpls_2_namespace);
    }

    program_property->tcam_profile             = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->pd_bitmap_0              = (0x18);
    program_property->pd_bitmap_1              = (0x0);
    program_property->isa_key_initial_from_vt  = (0x0);
    program_property->isb_key_initial_from_vt  = (0x0);
    program_property->tcam_key_initial_from_vt = (0x0);

    program_property->processing_code          = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_MPLS;
    program_property->result_to_use_0          = (2);
    program_property->result_to_use_1          = (0);
    program_property->result_to_use_2          = (0);
    program_property->second_stage_parsing     = 1;
    program_property->processing_profile       = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE;

}

void arad_pp_dbal_tt_program_indexed_mpls_l1_l2_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 is_gal)
{
    program_property->nof_keys = 2;
    program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L1IDX_SEM_A);
    program_property->vtt_table_index[0] += 2*(g_prog_soc_prop[unit].mpls_1_namespace);

    if(is_gal)
        program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_GAL_SEM_B);
    else
    {
        program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L1IDX_SEM_B);
        program_property->vtt_table_index[1] += 2*(g_prog_soc_prop[unit].mpls_2_namespace);
    }

    program_property->tcam_profile             = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->pd_bitmap_0              = (0x10);
    program_property->pd_bitmap_1              = (0x8);
    program_property->isa_key_initial_from_vt  = (0x0);
    program_property->isb_key_initial_from_vt  = (0x0);
    program_property->tcam_key_initial_from_vt = (0x0);

    program_property->processing_code          = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_MPLS;
    program_property->result_to_use_0          = (2);
    program_property->result_to_use_1          = (3);
    program_property->result_to_use_2          = (0);
    program_property->second_stage_parsing     = 1;
    program_property->processing_profile       = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE;
}


void arad_pp_dbal_tt_program_indexed_mpls_l2_l2_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
    program_property->nof_keys = 2;
    if (SOC_IS_ARADPLUS_AND_BELOW(unit)){
        program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L1IDX_SEM_A);
        program_property->vtt_table_index[0] += 2*(g_prog_soc_prop[unit].mpls_1_namespace);

        program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L1IDX_SEM_B);
        program_property->vtt_table_index[1] += 2*(g_prog_soc_prop[unit].mpls_2_namespace);

        program_property->result_to_use_0          = (1);
        program_property->result_to_use_1          = (2);
        program_property->result_to_use_2          = (0);
    } else if (SOC_IS_JERICHO(unit)) {
        program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L1IDX_SEM_A);
        program_property->vtt_table_index[0] += 2*(g_prog_soc_prop[unit].mpls_1_namespace);

        program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L2IDX_SEM_B);
        program_property->vtt_table_index[1] += 2*(g_prog_soc_prop[unit].mpls_2_namespace);

        program_property->result_to_use_0          = (2);
        program_property->result_to_use_1          = (0);
        program_property->result_to_use_2          = (0);
	}

    program_property->tcam_profile             = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->pd_bitmap_0              = (0x18);
    program_property->pd_bitmap_1              = (0x0);
    program_property->isa_key_initial_from_vt  = (0x0);
    program_property->isb_key_initial_from_vt  = (0x0);
    program_property->tcam_key_initial_from_vt = (0x0);

    program_property->processing_code          = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_MPLS;
    program_property->second_stage_parsing     = 1;
    program_property->processing_profile       = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE;
}


void arad_pp_dbal_tt_program_indexed_mpls_l2_l3_gal_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
    program_property->nof_keys = 2;
    program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L1IDX_SEM_A);
    program_property->vtt_table_index[0] += 2*(g_prog_soc_prop[unit].mpls_1_namespace);

    program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_GAL_SEM_B);

    program_property->tcam_profile             = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->pd_bitmap_0              = (0x10);
    program_property->pd_bitmap_1              = (0x8);
    program_property->isa_key_initial_from_vt  = (0x0);
    program_property->isb_key_initial_from_vt  = (0x0);
    program_property->tcam_key_initial_from_vt = (0x0);

    program_property->processing_code          = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_MPLS;
    program_property->result_to_use_0          = (1);
    program_property->result_to_use_1          = (2);
    program_property->result_to_use_2          = (3);
    program_property->second_stage_parsing     = 1;
    program_property->processing_profile       = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE;
}

void arad_pp_dbal_tt_program_indexed_mpls_l1_gal_only_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 is_gal)
{

    program_property->nof_keys = 1;
    program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L1IDX_SEM_B);
    program_property->vtt_table_index[0] += 2*(g_prog_soc_prop[unit].mpls_1_namespace);


    program_property->tcam_profile             = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->pd_bitmap_0              = (0x18);
    program_property->pd_bitmap_1              = (0x0);
    program_property->isa_key_initial_from_vt  = (0x0);
    program_property->isb_key_initial_from_vt  = (0x0);
    program_property->tcam_key_initial_from_vt = (0x0);

    program_property->processing_code          = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_MPLS;
    program_property->result_to_use_0          = (1);
    program_property->result_to_use_1          = (0);
    program_property->result_to_use_2          = (0);
    program_property->second_stage_parsing     = 1;
    program_property->processing_profile       = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE;

}

void arad_pp_dbal_tt_program_indexed_mpls_l2_gal_only_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{

    uint32 database_l2,namespace_l2;
    uint32 database_l3,namespace_l3;

    program_property->nof_keys = 1;
    program_property->vtt_table_index[0]  = ( g_prog_soc_prop[unit].is_bos_in_key_enable) ? SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_BOS_L1IDX_SEM_A) : SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L1IDX_SEM_A);

    arad_pp_dbal_vtt_find_namespace_database(unit, (g_prog_soc_prop[unit].tunnel_termination_in_tt_only) ? SOC_PPC_MPLS_TERM_NAMESPACE_L1 : SOC_PPC_MPLS_TERM_NAMESPACE_L2, &namespace_l2, &database_l2);
    arad_pp_dbal_vtt_find_namespace_database(unit, (g_prog_soc_prop[unit].tunnel_termination_in_tt_only) ? SOC_PPC_MPLS_TERM_NAMESPACE_L2 : SOC_PPC_MPLS_TERM_NAMESPACE_L3, &namespace_l3, &database_l3);

    program_property->vtt_table_index[0] += 2*namespace_l2;

    program_property->tcam_profile         = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->pd_bitmap_0          = 0x8;
    program_property->pd_bitmap_1          = 0x10;
    program_property->processing_code      = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_MPLS;
    program_property->result_to_use_0      = 1;
    program_property->result_to_use_1      = 3;
    program_property->result_to_use_2      = 2;
    program_property->second_stage_parsing = 1;
    program_property->processing_profile   = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE;

    if(!(g_prog_soc_prop[unit].tunnel_termination_in_tt_only) && database_l3 == ARAD_PP_ISEM_ACCESS_ID_ISEM_A)
    {
        arad_pp_dbal_vtt_program_property_swap( program_property );
        program_property->vtt_table_index[0] += 1;
    }

    if(g_prog_soc_prop[unit].is_bos_in_key_enable)
    {
        program_property->qual_to_ce_info.use_32_bit_ce[0][0] = 1;
    }

}


void arad_pp_dbal_tt_program_unindexed_mpls_l2_l3_eli_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{

    program_property->nof_keys = 1;
    program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_ELI_UNINDEXED_SEM_B);
    program_property->tcam_profile        = ARAD_TCAM_ACCESS_PROFILE_INVALID;

    program_property->isa_key_initial_from_vt  = (0x0);
    program_property->isb_key_initial_from_vt  = (0x0);
    program_property->tcam_key_initial_from_vt = (0x0);
    program_property->pd_bitmap_0              = (0x8);
    program_property->pd_bitmap_1              = (0x0);
    program_property->processing_code          = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_MPLS;
    program_property->result_to_use_0          = (1);
    program_property->result_to_use_1          = (2);
    program_property->result_to_use_2          = (0);
    program_property->second_stage_parsing     = 1;
    program_property->processing_profile       = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE;

    if(g_prog_soc_prop[unit].mpls_1_database == ARAD_PP_ISEM_ACCESS_ID_ISEM_A)
    {
        arad_pp_dbal_vtt_program_property_swap(program_property );
        program_property->vtt_table_index[0]    = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_ELI_UNINDEXED_SEM_B);
    }

}


void arad_pp_dbal_tt_program_indexed_mpls_l2_l3_eli_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
    uint32 database_l2,namespace_l2;

    program_property->nof_keys = 1;
    program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_ELI_L1IDX_SEM_A);

    arad_pp_dbal_vtt_find_namespace_database(unit, SOC_PPC_MPLS_TERM_NAMESPACE_L2, &namespace_l2, &database_l2);

    program_property->vtt_table_index[0] += 2*namespace_l2;

    program_property->tcam_profile         = ARAD_TCAM_ACCESS_PROFILE_INVALID;

    program_property->isa_key_initial_from_vt  = (0x0);
    program_property->isb_key_initial_from_vt  = (0x0);
    program_property->tcam_key_initial_from_vt = (0x0);
    program_property->pd_bitmap_0              = 0x0;
    program_property->pd_bitmap_1              = 0x10;
    program_property->processing_code          = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_MPLS;
    program_property->result_to_use_0          = (1);
    program_property->result_to_use_1          = (3);
    program_property->result_to_use_2          = (0);
    program_property->second_stage_parsing     = 1;
    program_property->processing_profile       = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE;

    if(database_l2 == ARAD_PP_ISEM_ACCESS_ID_ISEM_B)
    {
        arad_pp_dbal_vtt_program_property_swap( program_property );
        program_property->vtt_table_index[0] += 1;
    }
}

void arad_pp_dbal_tt_program_l1_l2_inrif_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
    program_property->nof_keys = 2;

    program_property->vtt_table_index[0]   = SW_DB_DBAL2VTT(( g_prog_soc_prop[unit].is_bos_in_key_enable ? SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_BOS_L1IDX_SEM_A : SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L1IDX_SEM_A));
    program_property->vtt_table_index[0] += 2*(g_prog_soc_prop[unit].mpls_2_namespace);

    program_property->vtt_table_index[1]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_L1IDX_SEM_B);
    program_property->vtt_table_index[1] += 2*(g_prog_soc_prop[unit].mpls_1_namespace);

    program_property->isa_key_initial_from_vt  = (0x0);
    program_property->isb_key_initial_from_vt  = (0x0);
    program_property->tcam_key_initial_from_vt = (0x0);
    program_property->pd_bitmap_0              = (0x8); 
    program_property->pd_bitmap_1              = (0x10); 
    program_property->tcam_profile             = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->processing_code          = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_MPLS;
    program_property->result_to_use_0          = (2);
    program_property->result_to_use_1          = (3);
    program_property->result_to_use_2          = (1);
    program_property->second_stage_parsing     = 1;
    program_property->processing_profile       = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE;

    if(g_prog_soc_prop[unit].mpls_1_database == ARAD_PP_ISEM_ACCESS_ID_ISEM_A)
    {
        arad_pp_dbal_vtt_program_property_swap(program_property );
        program_property->vtt_table_index[0]    += 1;
        program_property->vtt_table_index[1]    -= 1;
    }

    if(!SOC_IS_JERICHO(unit))
    {
        if(g_prog_soc_prop[unit].is_bos_in_key_enable)
        {
            program_property->qual_to_ce_info.use_32_bit_ce[0][0] = 1;
            program_property->qual_to_ce_info.use_32_bit_ce[1][1] = 1;
        }

        program_property->qual_to_ce_info.use_32_bit_ce[1][0] = 1;
    }
}

void arad_pp_dbal_tt_program_l1_l2eli_inrif_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
    program_property->nof_keys = 1;

    program_property->vtt_table_index[0]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_ELI_L1IDX_SEM_B);
    program_property->vtt_table_index[0] += 2*(g_prog_soc_prop[unit].mpls_1_namespace);

    program_property->isa_key_initial_from_vt  = (0x0);
    program_property->isb_key_initial_from_vt  = (0x0);
    program_property->tcam_key_initial_from_vt = (0x0);
    program_property->pd_bitmap_0              = (0x8);
    program_property->pd_bitmap_1              = (0x0);
    program_property->tcam_profile             = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->processing_code          = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_MPLS;
    program_property->result_to_use_0          = (2);
    program_property->result_to_use_1          = (0);
    program_property->result_to_use_2          = (0);
    program_property->second_stage_parsing     = 1;
    program_property->processing_profile       = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE;

    if(g_prog_soc_prop[unit].mpls_1_database == ARAD_PP_ISEM_ACCESS_ID_ISEM_A)
    {
        arad_pp_dbal_vtt_program_property_swap(program_property );
        program_property->vtt_table_index[0]    -= 1;
    }
}

void arad_pp_dbal_tt_program_l1_l2_l3eli_inrif_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
    program_property->nof_keys = 2;

    program_property->vtt_table_index[0]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_ELI_L1IDX_SEM_A);
    program_property->vtt_table_index[0] += 2*(g_prog_soc_prop[unit].mpls_2_namespace);

    program_property->vtt_table_index[1]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_RIF_MPLS_L1_L1IDX_SEM_B);
    program_property->vtt_table_index[1] += 2*(g_prog_soc_prop[unit].mpls_1_namespace);

    program_property->isa_key_initial_from_vt  = (0x0);
    program_property->isb_key_initial_from_vt  = (0x0);
    program_property->tcam_key_initial_from_vt = (0x0);
    program_property->pd_bitmap_0              = (0x8); 
    program_property->pd_bitmap_1              = (0x10); 
    program_property->tcam_profile             = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->processing_code          = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_MPLS;
    program_property->result_to_use_0          = (2);
    program_property->result_to_use_1          = (3);
    program_property->result_to_use_2          = (0);
    program_property->second_stage_parsing     = 1;
    program_property->processing_profile       = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE;

    if(g_prog_soc_prop[unit].mpls_1_database == ARAD_PP_ISEM_ACCESS_ID_ISEM_A)
    {
        arad_pp_dbal_vtt_program_property_swap(program_property );
        program_property->vtt_table_index[0]    += 1;
        program_property->vtt_table_index[1]    -= 1;
    }

    if(!SOC_IS_JERICHO(unit))
    {
        if(g_prog_soc_prop[unit].is_bos_in_key_enable)
        {
            program_property->qual_to_ce_info.use_32_bit_ce[0][0] = 1;
            program_property->qual_to_ce_info.use_32_bit_ce[1][1] = 1;
        }

        program_property->qual_to_ce_info.use_32_bit_ce[1][0] = 1;
    }

}

void arad_pp_dbal_tt_program_indexed_mpls_l3_l4_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{

    uint32 database_l3,namespace_l3;
    uint32 database_l4,namespace_l4;

    program_property->nof_keys = 2;
    program_property->vtt_table_index[0]  = ( g_prog_soc_prop[unit].is_bos_in_key_enable) ? SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_BOS_L1IDX_SEM_A) : SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_L1IDX_SEM_A);
    program_property->vtt_table_index[1]  = ( g_prog_soc_prop[unit].is_bos_in_key_enable) ? SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_BOS_L1IDX_SEM_B) : SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_L1IDX_SEM_B);

    arad_pp_dbal_vtt_find_namespace_database(unit, SOC_PPC_MPLS_TERM_NAMESPACE_L2, &namespace_l3, &database_l3); 
    arad_pp_dbal_vtt_find_namespace_database(unit, SOC_PPC_MPLS_TERM_NAMESPACE_L3, &namespace_l4, &database_l4); 

    program_property->vtt_table_index[0] += 2*namespace_l3;
    program_property->vtt_table_index[1] += 2*namespace_l4;

    program_property->tcam_profile         = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->pd_bitmap_0          = 0x8;
    program_property->pd_bitmap_1          = 0x10;
    program_property->processing_code      = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_MPLS;
    program_property->result_to_use_0      = 1;
    program_property->result_to_use_1      = 3;
    program_property->result_to_use_2      = 2;
    program_property->second_stage_parsing = 1;
    program_property->processing_profile   = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE;

    if(database_l4 == ARAD_PP_ISEM_ACCESS_ID_ISEM_A)
    {
        arad_pp_dbal_vtt_program_property_swap( program_property );
        program_property->vtt_table_index[0] += 1;
        program_property->vtt_table_index[1] -= 1;
    }

    if(!SOC_IS_JERICHO(unit))
    {
        if(g_prog_soc_prop[unit].is_bos_in_key_enable)
        {
            program_property->qual_to_ce_info.use_32_bit_ce[0][0] = 1;
            program_property->qual_to_ce_info.use_32_bit_ce[1][0] = 1;
        }
    }

}


void arad_pp_dbal_tt_program_indexed_mpls_l3_l2_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 is_mpls4)
{

    uint32 database_l2,namespace_l2;
    uint32 database_l3,namespace_l3;

    program_property->nof_keys = 2;
    if (is_mpls4) {
        program_property->vtt_table_index[0]  = ( g_prog_soc_prop[unit].is_bos_in_key_enable) ? SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_BOS_L1IDX_SEM_A) : SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_L1IDX_SEM_A);
        program_property->vtt_table_index[1]  = ( g_prog_soc_prop[unit].is_bos_in_key_enable) ? SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_BOS_L1IDX_SEM_B) : SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_L1IDX_SEM_B);
    } else {
        program_property->vtt_table_index[0]  = ( g_prog_soc_prop[unit].is_bos_in_key_enable) ? SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_BOS_L1IDX_SEM_A) : SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L1IDX_SEM_A);
        program_property->vtt_table_index[1]  = ( g_prog_soc_prop[unit].is_bos_in_key_enable) ? SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_BOS_L1IDX_SEM_B) : SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_L1IDX_SEM_B);

    }

    arad_pp_dbal_vtt_find_namespace_database(unit, (g_prog_soc_prop[unit].tunnel_termination_in_tt_only) ? SOC_PPC_MPLS_TERM_NAMESPACE_L1 : SOC_PPC_MPLS_TERM_NAMESPACE_L2, &namespace_l2, &database_l2);
    arad_pp_dbal_vtt_find_namespace_database(unit, (g_prog_soc_prop[unit].tunnel_termination_in_tt_only) ? SOC_PPC_MPLS_TERM_NAMESPACE_L2 : SOC_PPC_MPLS_TERM_NAMESPACE_L3, &namespace_l3, &database_l3);

    program_property->vtt_table_index[0] += 2*namespace_l2;
    program_property->vtt_table_index[1] += 2*namespace_l3;

    program_property->tcam_profile         = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->pd_bitmap_0          = 0x8;
    program_property->pd_bitmap_1          = 0x10;
    program_property->processing_code      = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_MPLS;
    program_property->result_to_use_0      = 1;
    program_property->result_to_use_1      = 3;
    program_property->result_to_use_2      = 2;
    program_property->second_stage_parsing = 1;
    program_property->processing_profile   = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE;

    if(!(g_prog_soc_prop[unit].tunnel_termination_in_tt_only) && database_l3 == ARAD_PP_ISEM_ACCESS_ID_ISEM_A)
    {
        arad_pp_dbal_vtt_program_property_swap( program_property );
        program_property->vtt_table_index[0] += 1;
        program_property->vtt_table_index[1] -= 1;
    }

    if(g_prog_soc_prop[unit].is_bos_in_key_enable)
    {
        program_property->qual_to_ce_info.use_32_bit_ce[0][0] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[1][0] = 1;
    }

}





void arad_pp_dbal_tt_program_mpls_port_termination_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
    
    uint32 database_l2,namespace_l2;
    uint32 database_l3,namespace_l3;
    program_property->nof_keys = 1;
    program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_PORT_L3_L1IDX_SEM_B);
    program_property->pd_bitmap_0          = 0x10;
    program_property->pd_bitmap_1          = 0x8;

    if(g_prog_soc_prop[unit].mpls_1_database == ARAD_PP_ISEM_ACCESS_ID_ISEM_A)
    {
        program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_PORT_L3_L1IDX_SEM_A);
        program_property->pd_bitmap_0          = 0x8;
        program_property->pd_bitmap_1          = 0x10;

    }

    arad_pp_dbal_vtt_find_namespace_database(unit, (g_prog_soc_prop[unit].tunnel_termination_in_tt_only) ? SOC_PPC_MPLS_TERM_NAMESPACE_L1 : SOC_PPC_MPLS_TERM_NAMESPACE_L2, &namespace_l2, &database_l2);
    arad_pp_dbal_vtt_find_namespace_database(unit, (g_prog_soc_prop[unit].tunnel_termination_in_tt_only) ? SOC_PPC_MPLS_TERM_NAMESPACE_L2 : SOC_PPC_MPLS_TERM_NAMESPACE_L3, &namespace_l3, &database_l3);


    program_property->tcam_profile         = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->processing_code      = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_MPLS;
    program_property->result_to_use_0      = 1;
    program_property->result_to_use_1      = 3;
    program_property->result_to_use_2      = 2;
    program_property->second_stage_parsing = 1;
    program_property->processing_profile   = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE;


    if(g_prog_soc_prop[unit].is_bos_in_key_enable)
    {
        program_property->qual_to_ce_info.use_32_bit_ce[0][0] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[1][0] = 1;
    }

}

void arad_pp_dbal_tt_program_arp_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
    program_property->nof_keys = 0;

    program_property->tcam_profile    = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->processing_code = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_ARP;
}

void arad_pp_dbal_tt_program_bridge_property_set (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{

    if(g_prog_soc_prop[unit].compression_spoof_ip6_enable)
    {
        program_property->nof_keys = 1;
        program_property->tcam_profile        = ARAD_PP_ISEM_ACCESS_TCAM_TT_IPV6_SPOOF_COMPRESSION;
        program_property->pd_bitmap_1         = (0x20);
        program_property->processing_code     = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_BRIDGE;
        program_property->processing_profile  = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE;
        program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_SPOOFv6_COMPRESSED_TCAM) ;
    }
    else
    {
        program_property->nof_keys = 0;
        program_property->pd_bitmap_0 = (0x28);
        program_property->processing_code     = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_BRIDGE;
        program_property->processing_profile  = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE;

        if(g_prog_soc_prop[unit].spoof_ipv6_enable)
        {
            program_property->tcam_profile = ARAD_PP_ISEM_ACCESS_TCAM_IPV6_SPOOF_DB_PROFILE;
            program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_SPOOFv6_TCAM) ;
            program_property->nof_keys += 1;
        }
        else
            program_property->tcam_profile = ARAD_TCAM_ACCESS_PROFILE_INVALID;

        if(g_prog_soc_prop[unit].spoof_enable)
        {
            program_property->vtt_table_index[1]            = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_SPOOFv4_SEM_B);
            program_property->nof_keys += 1;
        }
    }
}

void arad_pp_dbal_tt_program_uknown_l3_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
    program_property->nof_keys = 0;

    program_property->tcam_profile        = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->processing_code     = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_UNKNOWN_L3;
    program_property->processing_profile  = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE;

}

void arad_pp_dbal_tt_program_trill_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 nof_tags     )
{
    program_property->nof_keys = 3;

    program_property->vtt_table_index[0] =  SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_TRILL_ING_NICK_SEM_A);
    program_property->vtt_table_index[1] = (nof_tags == TRILL_ONE_TAG) ? SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_TRILL_UC_ONE_TAG_SEM_B) : SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_TRILL_UC_TWO_TAG_SEM_B) ;
    program_property->vtt_table_index[2] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_TRILL_TCAM);
    program_property->tcam_profile       = ARAD_PP_ISEM_ACCESS_TCAM_TT_TRILL_TRANSPARENT_SERVICE;

    program_property->key_program_variable = (0x2); 

    program_property->pd_bitmap_0          = 0x10;
    program_property->pd_bitmap_1          = 0xc;
    program_property->tcam_profile         = ARAD_PP_ISEM_ACCESS_TCAM_TT_TRILL_TRANSPARENT_SERVICE;
    program_property->processing_code      = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_TRILL;
    program_property->result_to_use_0      = 2;
    program_property->result_to_use_1      = 0;
    program_property->result_to_use_2      = 0;
    program_property->second_stage_parsing = 0;
    program_property->processing_profile   = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE;

    if(!SOC_IS_JERICHO(unit))
    {
        program_property->qual_to_ce_info.use_32_bit_ce[0][0] = 1;
        program_property->qual_to_ce_info.use_32_bit_ce[2][0] = 1;
    }


}
void arad_pp_dbal_tt_program_trill_trap_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused     )
{
    program_property->nof_keys = 0;
    program_property->processing_code    = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_TRILL_DESIGNATED_VLAN;
    program_property->processing_profile = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE;
}

void arad_pp_dbal_tt_program_fc_with_vft_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY *program_property, uint32 vsan_source)
{
    program_property->nof_keys = 1;

    if(vsan_source == VSAN_FROM_VFT)
    {
        program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_FC_WITH_VFT_VSAN_VFT_SEMB);
    }

    if(vsan_source == VSAN_FROM_VSI)
    {
        program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_FC_WITH_VFT_VSAN_VSI_SEMB);
    }

    if(vsan_source == VSAN_INIT)
    {
        program_property->nof_dbs = 2;
        program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_FC_WITH_VFT_VSAN_VFT_SEMB);
        program_property->vtt_table_index[1] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_FC_WITH_VFT_VSAN_VSI_SEMB);
    }

    program_property->pd_bitmap_0             = (0x8);
    program_property->isb_key_initial_from_vt = (0x0);
    program_property->tcam_profile            = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->result_to_use_0         = (0);
    program_property->result_to_use_1         = (0);
    program_property->result_to_use_2         = (0);
    program_property->second_stage_parsing    = (0x0);
    program_property->processing_code         = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_CUSTOM_1;
    program_property->processing_profile      = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_FC;
}

void arad_pp_dbal_tt_program_fc_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY *program_property, uint32 vsan_source)
{
    program_property->nof_keys = 1;

    if(vsan_source == VSAN_FROM_VFT)
    {
        program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_FC_VSAN_VFT_SEMB);
    }

    if(vsan_source == VSAN_FROM_VSI)
    {
        program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_FC_VSAN_VSI_SEMB);
    }

    if(vsan_source == VSAN_INIT)
    {
        program_property->nof_dbs = 2;
        program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_FC_VSAN_VFT_SEMB);
        program_property->vtt_table_index[1] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_FC_VSAN_VSI_SEMB);
    }

    program_property->pd_bitmap_0             = (0x8);
    program_property->isb_key_initial_from_vt = (0x0);
    program_property->tcam_profile            = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->processing_code         = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_CUSTOM_1;
    program_property->processing_profile      = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_FC;
}

uint32 arad_pp_dbal_vtt_fc_vsan_mode_update(uint32 unit, uint32 is_vsan_from_vsi)
{
    int res;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (!is_vsan_from_vsi) 
    {
        res = _arad_pp_dbal_vtt_fc_vsan_mode_update(unit, ARAD_PP_DBAL_TT_PROG_FC_WITH_VFT, SOC_DPP_DBAL_SW_TABLE_ID_FC_WITH_VFT_VSAN_VSI_SEMB);
        SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);

        res = _arad_pp_dbal_vtt_fc_vsan_mode_update(unit, ARAD_PP_DBAL_TT_PROG_FC, SOC_DPP_DBAL_SW_TABLE_ID_FC_VSAN_VSI_SEMB);
        SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
    }
    else                   
    {
        res = _arad_pp_dbal_vtt_fc_vsan_mode_update(unit, ARAD_PP_DBAL_TT_PROG_FC_WITH_VFT, SOC_DPP_DBAL_SW_TABLE_ID_FC_WITH_VFT_VSAN_VFT_SEMB);
        SOC_SAND_CHECK_FUNC_RESULT(res, 21, exit);

        res = _arad_pp_dbal_vtt_fc_vsan_mode_update(unit, ARAD_PP_DBAL_TT_PROG_FC, SOC_DPP_DBAL_SW_TABLE_ID_FC_VSAN_VFT_SEMB);
        SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_dbal_vtt_fc_vsan_mode_update", 0, 0);
}

uint32 _arad_pp_dbal_vtt_fc_vsan_mode_update(uint32 unit, uint32 prog_id, uint32 disassociate_table_id)
{
    int sw_db_index;
    SOC_DPP_DBAL_KEY_TO_TABLE          keys_to_table_id[SOC_DPP_DBAL_PROGRAM_NOF_KEYS] = {{0}};
    SOC_DPP_DBAL_TABLE_INFO table;
    ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  program_property;
    uint32 vsan_new_source;
    int res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if(disassociate_table_id == SOC_DPP_DBAL_SW_TABLE_ID_FC_WITH_VFT_VSAN_VSI_SEMB ||
       disassociate_table_id == SOC_DPP_DBAL_SW_TABLE_ID_FC_VSAN_VSI_SEMB)
    {
        vsan_new_source = VSAN_FROM_VFT;
    }
    else
    {
       
        vsan_new_source = VSAN_FROM_VSI;
    }

    res = sw_state_access[unit].dpp.soc.arad.pp.dbal_info.dbal_tables.get(unit, disassociate_table_id, &table);
    SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);

    res = arad_pp_dbal_program_table_disassociate(unit,
                                                  table.table_programs[0].program_id,
                                                  table.table_programs[0].stage, disassociate_table_id);
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

    
    PROGRAM_PROPERTY_CLEAR(&program_property);
    
    (tt_dbal_programs[prog_id].vtt_program_property_set)(unit, &program_property, vsan_new_source);

     sw_db_index = program_property.vtt_table_index[0];

     keys_to_table_id[0].key_id                 = MEM_TYPE_TO_KEY(vtt_tables[sw_db_index].mem_type);
     keys_to_table_id[0].sw_table_id            = vtt_tables[sw_db_index].dbal_sw_db_id;

     res = arad_pp_dbal_program_to_tables_associate(unit,
                                                    tt_dbal_programs[prog_id].prog_used[unit],
                                                    SOC_PPC_FP_DATABASE_STAGE_INGRESS_TT,
                                                    keys_to_table_id,
                                                    program_property.qual_to_ce_info.use_32_bit_ce,
                                                    program_property.nof_keys);
     SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in _arad_pp_dbal_vtt_fc_vsan_mode_update", 0, 0);
}


uint32
  arad_pp_dbal_vtt_fcoe_npv_mode_no_my_mac_set(int unit, int enable)
{
    uint32 res, i;
    ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA tt_prog_selection;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    for (i = 0; i < DPP_PP_ISEM_PROG_SEL_1ST_CAM_DEF_OFFSET(unit); i++)
    {
        ARAD_CLEAR(&tt_prog_selection, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA, 1);
        res = arad_pp_ihp_isem_2nd_program_selection_cam_tbl_get_unsafe(unit, i, &tt_prog_selection);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        if(tt_prog_selection.llvp_prog_sel == tt_dbal_programs[tt_dbal_program_selection[ARAD_PP_DBAL_TT_PROG_SELECTION_FC_WITH_VFT].prog_name[unit]].prog_used[unit] ||
           tt_prog_selection.llvp_prog_sel == tt_dbal_programs[tt_dbal_program_selection[ARAD_PP_DBAL_TT_PROG_SELECTION_FC].prog_name[unit]].prog_used[unit])
        {
            tt_prog_selection.valid = enable;
            res = arad_pp_ihp_isem_2nd_program_selection_cam_tbl_set_unsafe(unit, i, &tt_prog_selection);
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_isem_access_fcoe_npv_mode_set", 0, 0);
}

void arad_pp_dbal_tt_program_mac_in_mac_property_set        (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 mac_in_mac_type )
{
    program_property->nof_keys = 1;

    program_property->vtt_table_index[0] =  SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_ETHERNET_HEADER_ISID_SEM_A);
    program_property->tcam_profile        = ARAD_TCAM_ACCESS_PROFILE_INVALID;

    program_property->pd_bitmap_0          = 0x10;

    if(mac_in_mac_type == MAC_IN_MAC_WITH_BTAG)
        program_property->processing_code = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_MIM;
    else
        program_property->processing_code = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_MY_B_MAC_MC_BRIDGE;

    program_property->result_to_use_0      = 2;
    program_property->second_stage_parsing = 0;
    program_property->processing_profile   = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_MAC_IN_MAC;

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mim_spb_enable", 0)) {
        program_property->nof_keys           = 2;
        program_property->vtt_table_index[1] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_ETHERNET_HEADER_BSA_ISID_SEM_B);
        program_property->pd_bitmap_0        = 0x18;
    }

}

void arad_pp_dbal_tt_program_ipv4_router_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
    program_property->nof_keys = 0;

    program_property->pd_bitmap_0              = (0x00);
    program_property->pd_bitmap_1              = (0x0);

    program_property->isa_key_initial_from_vt  = (0x0);
    program_property->isb_key_initial_from_vt  = (0x0);
    program_property->tcam_key_initial_from_vt = (0x0);
    program_property->tcam_profile             = ARAD_TCAM_ACCESS_PROFILE_INVALID;

    program_property->processing_code          = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_IP;

    program_property->result_to_use_0          = (0);
    program_property->result_to_use_1          = (0);
    program_property->result_to_use_2          = (0);
    program_property->second_stage_parsing     = (0x0);

    program_property->processing_profile       = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE;
}
void arad_pp_dbal_tt_program_ipv4_router_pwe_gre_dip_found_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{

    if(g_prog_soc_prop[unit].ipv4_term_dip_sip_enable)
    {
        program_property->nof_keys = 1;
        program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_TUNNEL_SEM_B);

    }
    else
    {
        program_property->nof_keys = 2;
        program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_SIPv4_SEM_A);
        program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_TUNNEL_SEM_B);

    }

    program_property->tcam_profile             =  ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->pd_bitmap_0              = (0x10);
    program_property->pd_bitmap_1              = (0x8);
    program_property->isa_key_initial_from_vt  = (0x0);
    program_property->isb_key_initial_from_vt  = (0x0);
    program_property->tcam_key_initial_from_vt = (0x0);
    program_property->processing_code          = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_IP;
    program_property->result_to_use_0          = (1); 
    program_property->result_to_use_1          = (0);
    program_property->result_to_use_2          = (0);
    program_property->second_stage_parsing     = (0x0);
    program_property->processing_profile       = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_PWEoGRE;
}


void arad_pp_dbal_tt_program_ipv4_router_dip_found_port_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 tunnel_type) {
    program_property->nof_keys = 1;

    if (tunnel_type == L2_GRE) {
        program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_L2GRE_ID_TUNNEL_SEM_B);
    } else {
        program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_VXLAN_ID_TUNNEL_SEM_B);
    }
    program_property->tcam_profile         =  ARAD_TCAM_ACCESS_PROFILE_INVALID;

    program_property->pd_bitmap_0              = (0x14); 
    program_property->pd_bitmap_1              = (0x8);  
    program_property->isa_key_initial_from_vt  = (0x0);
    program_property->isb_key_initial_from_vt  = (0x0);
    program_property->tcam_key_initial_from_vt = (0x0);
    program_property->processing_code          = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_IP;
    program_property->result_to_use_0      = (1); 
    program_property->result_to_use_1          = (0); 
    program_property->result_to_use_2          = (0); 
    program_property->second_stage_parsing     = (0x0);
    program_property->processing_profile       = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE;

}

void arad_pp_dbal_tt_program_ipv4_router_dip_found_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 is_recycle)
{
   program_property->nof_keys = 0;

   if(!g_prog_soc_prop[unit].ipv4_term_dip_sip_enable)
   {
       program_property->nof_keys = 1;
       program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_SIPv4_SEM_A);
   }

   if (SOC_IS_JERICHO(unit) && (g_prog_soc_prop[unit].ipv4_term_dip_sip_enable == 4) && (!is_recycle)) {
       program_property->nof_keys = 2; 
       program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MY_VTEP_INDEX_SIP_VRF_SEM_A); 
       program_property->vtt_table_index[1] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_DIP_SIP_VRF_TCAM); 
       
   }


   if(g_prog_soc_prop[unit].e_o_ip_enable | g_prog_soc_prop[unit].nvgre_enable | g_prog_soc_prop[unit].vxlan_enable)
   {
       program_property->vtt_table_index[program_property->nof_keys] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_TUNNEL_SEM_B);
       program_property->nof_keys++;
   }

   if (SOC_IS_JERICHO(unit) && (g_prog_soc_prop[unit].ipv4_term_dip_sip_enable == 4) && (!is_recycle)) {
       program_property->tcam_profile         =  ARAD_PP_ISEM_ACCESS_TCAM_DIP_SIP_VRF_PROFILE;
   } else {
       program_property->tcam_profile         =  ARAD_TCAM_ACCESS_PROFILE_INVALID;
   }
   program_property->pd_bitmap_0              = (0x14); 
   program_property->pd_bitmap_1              = (0x8);   
   program_property->isa_key_initial_from_vt  = (0x0);
   program_property->isb_key_initial_from_vt  = (0x0);
   program_property->tcam_key_initial_from_vt = (0x0);
   program_property->processing_code          = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_IP;
   if (SOC_IS_JERICHO(unit) && (g_prog_soc_prop[unit].ipv4_term_dip_sip_enable == 4) && (!is_recycle)) {
       program_property->result_to_use_0      = (2);  
   } else {
       program_property->result_to_use_0      = (1); 
   }
   program_property->result_to_use_1          = (0); 
   program_property->result_to_use_2          = (0); 
   program_property->second_stage_parsing     = (0x0);
   program_property->processing_profile       = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE;


 



}
void arad_pp_dbal_tt_program_ipv4_router_uc_dip_not_found_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
      program_property->nof_keys = 0;

      program_property->pd_bitmap_0              = (0x00);
      program_property->pd_bitmap_1              = (0x0);

      program_property->isa_key_initial_from_vt  = (0x0);
      program_property->isb_key_initial_from_vt  = (0x0);
      program_property->tcam_key_initial_from_vt = (0x0);
      program_property->tcam_profile             = ARAD_TCAM_ACCESS_PROFILE_INVALID;

      program_property->processing_code          = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_IP;

      program_property->result_to_use_0          = (1); 
      program_property->result_to_use_1          = (0);
      program_property->result_to_use_2          = (0);
      program_property->second_stage_parsing     = (0x0);

      program_property->processing_profile       = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE;

}
void arad_pp_dbal_tt_program_ipv4_router_mc_dip_not_found_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
    program_property->nof_keys           = 1;
    program_property->pd_bitmap_1        = (0x0);
    program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MC_DIPv4_RIF_SEM_A);

    if(g_prog_soc_prop[unit].e_o_ip_enable | g_prog_soc_prop[unit].nvgre_enable | g_prog_soc_prop[unit].vxlan_enable)
    {
        program_property->nof_keys            = 2;
        program_property->vtt_table_index[1] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_TUNNEL_SEM_B);
        program_property->pd_bitmap_1        = (0x8);
    }

    program_property->tcam_profile             =  ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->pd_bitmap_0              = (0x10);
    program_property->isa_key_initial_from_vt  = (0x0);
    program_property->isb_key_initial_from_vt  = (0x0);
    program_property->tcam_key_initial_from_vt = (0x0);
    program_property->processing_code          = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_COMPATIBLE_MC;
    program_property->result_to_use_0          = (1); 
    program_property->result_to_use_1          = (0);
    program_property->result_to_use_2          = (0);
    program_property->second_stage_parsing     = (0x0);
    program_property->processing_profile       = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE;

}
void arad_pp_dbal_tt_program_ipv4_router_compatible_mc_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
    program_property->nof_keys    = 0;
    program_property->pd_bitmap_1 = (0x0);

    if(g_prog_soc_prop[unit].e_o_ip_enable | g_prog_soc_prop[unit].nvgre_enable | g_prog_soc_prop[unit].vxlan_enable)
    {
        program_property->nof_keys            = 1;
        program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_TUNNEL_SEM_B);
        program_property->pd_bitmap_1        = (0x8);
    }

     program_property->tcam_profile         = ARAD_TCAM_ACCESS_PROFILE_INVALID;
     program_property->pd_bitmap_0          = (0x0);
     program_property->second_stage_parsing = (0x0);
     program_property->processing_code      = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_COMPATIBLE_MC;
     program_property->processing_profile   = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE;

}
void arad_pp_dbal_tt_program_ipv6_router_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 is_recycle)
{
    int dipv6_index = 0;
    int tunnel_index = 1;
    program_property->nof_keys = 1;

    if (is_recycle) {
        program_property->vtt_table_index[0]       = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_DIPv6_AFTER_RECYCLE_TCAM);
    } else {
        program_property->vtt_table_index[0]       = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_DIPv6_TCAM);
    }

    if (SOC_IS_JERICHO_PLUS(unit) && 
        (g_prog_soc_prop[unit].e_o_ip_enable | g_prog_soc_prop[unit].nvgre_enable | g_prog_soc_prop[unit].vxlan_enable)) {
        program_property->vtt_table_index[1] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_TUNNEL_SEM_B);
        program_property->nof_keys++;
    }

    if (is_recycle) {
        program_property->tcam_profile  = ARAD_PP_ISEM_ACCESS_TCAM_IPV6_AFTER_RECYCLE_TT_DB_PROFILE;
    } else {
        program_property->tcam_profile  = ARAD_PP_ISEM_ACCESS_TCAM_IPV6_TT_DB_PROFILE;
    }
    program_property->pd_bitmap_0              = (0x20);
    if (SOC_IS_JERICHO_PLUS(unit) && 
        (g_prog_soc_prop[unit].e_o_ip_enable | g_prog_soc_prop[unit].nvgre_enable | g_prog_soc_prop[unit].vxlan_enable)) {
        program_property->pd_bitmap_1              = (0x8);
    }
    program_property->isa_key_initial_from_vt  = (0x0);
    program_property->isb_key_initial_from_vt  = (0x0);
    program_property->tcam_key_initial_from_vt = (0x0);
    program_property->result_to_use_0          = (2);
    program_property->result_to_use_1          = (0);
    program_property->result_to_use_2          = (0);
    program_property->processing_code          = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_IP;
    program_property->second_stage_parsing     = (0x0);
    program_property->processing_profile       = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE;

    if (g_prog_soc_prop[unit].ipv6_term_dip_sip_enable) {
        program_property->implicit_flag = 1;
        program_property->qual_to_ce_info.qualifier_to_ce_id[dipv6_index][0] = 10;  
        program_property->qual_to_ce_info.qualifier_to_ce_id[dipv6_index][1] = 9;    
        program_property->qual_to_ce_info.qualifier_to_ce_id[dipv6_index][2] = 8;   
        program_property->qual_to_ce_info.qualifier_to_ce_id[dipv6_index][3] = 5;   
        program_property->qual_to_ce_info.qualifier_to_ce_id[dipv6_index][4] = 4;   
        program_property->qual_to_ce_info.qualifier_to_ce_id[dipv6_index][5] = 3;   
        program_property->qual_to_ce_info.qualifier_to_ce_id[dipv6_index][6] = 2;           
        program_property->qual_to_ce_info.qualifier_to_ce_id[dipv6_index][7] = 1;   

        if(g_prog_soc_prop[unit].nvgre_enable) {
            program_property->qual_to_ce_info.qualifier_to_ce_id[tunnel_index][0] = 11; 
            program_property->qual_to_ce_info.qualifier_to_ce_id[tunnel_index][1] = 7;  
            if (SOC_IS_L2GRE_VDC_ENABLE(unit)) {
                program_property->qual_to_ce_info.qualifier_to_ce_id[tunnel_index][2] = 6; 
            }
        }        
        if(g_prog_soc_prop[unit].vxlan_enable )
        {
            program_property->qual_to_ce_info.qualifier_to_ce_id[tunnel_index][0] = 11;  
            if (SOC_IS_VXLAN_VDC_ENABLE(unit)) {
                program_property->qual_to_ce_info.qualifier_to_ce_id[tunnel_index][1] = 7; 
            }
        }        
        if(g_prog_soc_prop[unit].e_o_ip_enable)
        {
            program_property->qual_to_ce_info.qualifier_to_ce_id[tunnel_index][0] = 11;  
        }
    }    
}

void arad_pp_dbal_tt_program_dipv6_compressed_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
    program_property->nof_keys = 1;

    program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_DIPv6_COMPRESSED_TCAM);

    program_property->tcam_profile            =  ARAD_PP_ISEM_ACCESS_TCAM_TT_DIP_COMPRESSION_DB_PROFILE;
    program_property->pd_bitmap_0             = (0x0);
    program_property->pd_bitmap_1             = (0x20);
    program_property->isb_key_initial_from_vt = (0x0);
    program_property->processing_code         = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_BRIDGE;
    program_property->processing_profile      = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE;
}

void arad_pp_dbal_tt_program_extender_untag_check_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 extender_type)
{
    program_property->nof_keys = 2;

    program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_EXTENDER_CHANNEL_REG_SEM_B);
    program_property->vtt_table_index[1] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_EXTENDER_UNTAG_CHECK_SEM_A);

    program_property->pd_bitmap_0              = (0x08); 
    program_property->pd_bitmap_1              = (0x10); 

    switch (extender_type) 
    {
    case EXTENDER_IP_UC:
        program_property->processing_code          = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_IP;
        break;
    case EXTENDER_IP_MC:
        program_property->processing_code          = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_COMPATIBLE_MC;
        break;
    case EXTENDER_ETH:
    default:
       program_property->processing_code           = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_BRIDGE;
       break;
    }

    program_property->processing_profile       = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE;
}

void arad_pp_dbal_tt_program_extender_cb_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 extender_type)
{
    program_property->nof_keys = 1;

    program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_EXTENDER_CHANNEL_REG_SEM_B);
    program_property->pd_bitmap_0              = (0x08); 

    switch (extender_type) 
    {
    case EXTENDER_IP_UC:
        program_property->processing_code          = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_IP;
        break;
    case EXTENDER_IP_MC:
        program_property->processing_code          = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_COMPATIBLE_MC;
        break;
    case EXTENDER_ETH:
    default:
       program_property->processing_code           = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_BRIDGE;
       break;
    }

    program_property->processing_profile       = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE;
}

void arad_pp_dbal_tt_program_ipv4_router_gre_port_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
    program_property->nof_keys = 1;
    program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_GRE_DUMMY_SEM_B);
    program_property->pd_bitmap_0 = (0x8);  
    program_property->pd_bitmap_1 = (0x0);
    program_property->tcam_profile         = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->processing_code          = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_IP;
    program_property->isa_key_initial_from_vt  = (0x0);
    program_property->isb_key_initial_from_vt  = (0x0);
    program_property->tcam_key_initial_from_vt = (0x0);
    program_property->result_to_use_0          = (2);                                        
    program_property->result_to_use_1          = (0);                                        
    program_property->result_to_use_2          = (0);                                        
    program_property->second_stage_parsing     = (0x0);                                    
    program_property->processing_profile       = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE; 

}


void arad_pp_dbal_tt_program_ipv4_router_dip_sip_vrf_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
   program_property->nof_keys = 0;

   
   if (SOC_IS_JERICHO(unit) && g_prog_soc_prop[unit].ipv4_term_dip_sip_enable == 3) 
   {
       program_property->nof_keys = 1;
       program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_DIP_SIP_VRF_TCAM);
   }

    program_property->tcam_profile             =  ARAD_PP_ISEM_ACCESS_TCAM_DIP_SIP_VRF_PROFILE;
    program_property->pd_bitmap_0              = (0x20);
    program_property->pd_bitmap_1              = (0x0);
    program_property->isa_key_initial_from_vt  = (0x0);
    program_property->isb_key_initial_from_vt  = (0x0);
    program_property->tcam_key_initial_from_vt = (0x0);
    program_property->processing_code          = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_IP;
    program_property->result_to_use_0          = (2);                                        
    program_property->result_to_use_1          = (0);                                        
    program_property->result_to_use_2          = (0);                                        
    program_property->second_stage_parsing     = (0x0);                                    
    program_property->processing_profile       = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE; 
}

void arad_pp_dbal_tt_program_ipv4_for_explicit_mpls_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
	program_property->nof_keys = 1;
    program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_DIPv4_EXPLICIT_NULL_DUMMY_SEM_A);
    program_property->pd_bitmap_0              = (0x10);
    program_property->pd_bitmap_1              = (0x0);
    program_property->tcam_profile         = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->isa_key_initial_from_vt  = (0x0);
    program_property->isb_key_initial_from_vt  = (0x0);
    program_property->tcam_key_initial_from_vt = (0x0);
    program_property->processing_code          = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_IP;
    program_property->result_to_use_0          = (2);                                        
    program_property->result_to_use_1          = (0);                                        
    program_property->result_to_use_2          = (0);                                        
    program_property->second_stage_parsing     = (0x0);                                    
    program_property->processing_profile       = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_MPLS_EXP_NULL; 

    if(g_prog_soc_prop[unit].mpls_1_database == ARAD_PP_ISEM_ACCESS_ID_ISEM_B)
    {
        arad_pp_dbal_vtt_program_property_swap( program_property );
        program_property->vtt_table_index[0]       = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_DIPv4_EXPLICIT_NULL_DUMMY_SEM_B);        
    }
}

void arad_pp_dbal_tt_program_ip_disable_on_lif_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
    program_property->nof_keys = 0;
    program_property->pd_bitmap_0              = (0x10);
    program_property->pd_bitmap_1              = (0x0);
    program_property->tcam_profile         = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->isa_key_initial_from_vt  = (0x0);
    program_property->isb_key_initial_from_vt  = (0x0);
    program_property->tcam_key_initial_from_vt = (0x0);
    program_property->processing_code          = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_BRIDGE;
    program_property->result_to_use_0          = (1);                                        
    program_property->result_to_use_1          = (0);                                        
    program_property->result_to_use_2          = (0);                                        
    program_property->second_stage_parsing     = (0x0);                                    
    program_property->processing_profile       = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE; 

}

void arad_pp_dbal_tt_program_tunnel_two_explicit_mpls_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
    uint32 database_l2,namespace_l2;

    program_property->nof_keys = 1;

    program_property->pd_bitmap_0     = 0x8;
    program_property->pd_bitmap_1     = 0x0;
    program_property->tcam_profile    = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->processing_code = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_MPLS;
    program_property->result_to_use_0 = 2;
    program_property->result_to_use_1 = 0;
    program_property->result_to_use_2 = 0;
    program_property->second_stage_parsing = 1;
    program_property->processing_profile   = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE;

    if (SOC_DPP_CONFIG(unit)->pp.mpls_label_index_enable) {
        program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L1IDX_SEM_B);
        arad_pp_dbal_vtt_find_namespace_database(unit, SOC_PPC_MPLS_TERM_NAMESPACE_L1, &namespace_l2, &database_l2);
        program_property->vtt_table_index[0] += 2*namespace_l2;
        if(database_l2 == ARAD_PP_ISEM_ACCESS_ID_ISEM_A)
        {
            arad_pp_dbal_vtt_program_property_swap(program_property );
            program_property->vtt_table_index[0]    -= 1;
        }
    } else {
        program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_UNINDEXED_SEM_B);
        if(g_prog_soc_prop[unit].mpls_1_database == ARAD_PP_ISEM_ACCESS_ID_ISEM_A)
        {
            arad_pp_dbal_vtt_program_property_swap(program_property );
            program_property->vtt_table_index[0]    -= 1;
        }
    }
}

void arad_pp_dbal_tt_program_vlan_domain_mpls_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
    program_property->nof_keys = 1;
    program_property->vtt_table_index[0]   = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_VLAN_DOMAIN_MPLS_L2_SEMB);

    program_property->tcam_profile             = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->pd_bitmap_0              = (0x18);
    program_property->pd_bitmap_1              = (0x0);
    program_property->isa_key_initial_from_vt  = (0x0);
    program_property->isb_key_initial_from_vt  = (0x0);
    program_property->tcam_key_initial_from_vt = (0x0);

    program_property->processing_code          = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_MPLS;
    program_property->result_to_use_0          = (1);
    program_property->result_to_use_1          = (2);
    program_property->result_to_use_2          = (0);
    program_property->second_stage_parsing     = 1;
    program_property->processing_profile       = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE;

	if(g_prog_soc_prop[unit].mpls_1_database == ARAD_PP_ISEM_ACCESS_ID_ISEM_A)
	{
		arad_pp_dbal_vtt_program_property_swap(program_property );
		program_property->vtt_table_index[0]	-= 1;
	}


}

void arad_pp_dbal_tt_program_index_as_order_l2_l3_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
    program_property->nof_keys = 2;
    if (soc_property_get(unit, spn_BCM886XX_MPLS_TERMINATION_DATABASE_MODE, 2) == 12) {
        program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L12IDX_SEM_A);
        
        program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_L3IDX_SEM_B);

        program_property->pd_bitmap_0              = (0x10);
        program_property->pd_bitmap_1              = (0x8);

    } else {
        program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L2IDX_SEM_B);
        
        program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_L13IDX_SEM_A);

        program_property->pd_bitmap_0              = (0x8);
        program_property->pd_bitmap_1              = (0x10);
    }
    program_property->tcam_profile             = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->isa_key_initial_from_vt  = (0x0);
    program_property->isb_key_initial_from_vt  = (0x0);
    program_property->tcam_key_initial_from_vt = (0x0);

    program_property->processing_code          = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_MPLS;
    program_property->result_to_use_0          = (1);
    program_property->result_to_use_1          = (2);
    program_property->result_to_use_2          = (3);
    program_property->second_stage_parsing     = 1;
    program_property->processing_profile       = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE;

}

void arad_pp_dbal_tt_program_index_as_order_l2_l2_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
    program_property->nof_keys = 2;

    if (soc_property_get(unit, spn_BCM886XX_MPLS_TERMINATION_DATABASE_MODE, 2) == 12) {
        program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L12IDX_SEM_A);

        program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L3IDX_SEM_B);
    }else {
        program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L2IDX_SEM_B);

        program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L13IDX_SEM_A);
    }
    program_property->tcam_profile             = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->pd_bitmap_0              = (0x18);
    program_property->pd_bitmap_1              = (0x0);
    program_property->isa_key_initial_from_vt  = (0x0);
    program_property->isb_key_initial_from_vt  = (0x0);
    program_property->tcam_key_initial_from_vt = (0x0);

    program_property->processing_code          = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_MPLS;
    program_property->result_to_use_0          = (1);
    program_property->result_to_use_1          = (2);
    program_property->result_to_use_2          = (0);
    program_property->second_stage_parsing     = 1;
    program_property->processing_profile       = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE;

}

void arad_pp_dbal_tt_program_index_as_order_l1_l2_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
    if (soc_property_get(unit, spn_BCM886XX_MPLS_TERMINATION_DATABASE_MODE, 2) == 12) {
        program_property->nof_keys = 0;
        program_property->result_to_use_0          = (0);
        program_property->result_to_use_1          = (0);
        program_property->result_to_use_2          = (0);

    } else {
        program_property->nof_keys = 2;
        program_property->result_to_use_0          = (2);
        program_property->result_to_use_1          = (3);
        program_property->result_to_use_2          = (0);

    }
    program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L2IDX_SEM_B);

    program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L13IDX_SEM_A);

    program_property->tcam_profile             = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->pd_bitmap_0              = (0x8);
    program_property->pd_bitmap_1              = (0x10);
    program_property->isa_key_initial_from_vt  = (0x0);
    program_property->isb_key_initial_from_vt  = (0x0);
    program_property->tcam_key_initial_from_vt = (0x0);

    program_property->processing_code          = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_MPLS;
    program_property->second_stage_parsing     = 1;
    program_property->processing_profile       = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE;

}

void arad_pp_dbal_tt_program_index_as_order_l1_l1_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
    program_property->nof_keys = 1;
    if (soc_property_get(unit, spn_BCM886XX_MPLS_TERMINATION_DATABASE_MODE, 2) == 12) {
        program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L3IDX_SEM_B);

    }else {
        program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L2IDX_SEM_B);

    }

    program_property->tcam_profile             = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->pd_bitmap_0              = (0x8);
    program_property->isa_key_initial_from_vt  = (0x0);
    program_property->isb_key_initial_from_vt  = (0x0);
    program_property->tcam_key_initial_from_vt = (0x0);

    program_property->processing_code          = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_MPLS;
    program_property->result_to_use_0          = (2);
    program_property->result_to_use_1          = (0);
    program_property->result_to_use_2          = (0);
    program_property->second_stage_parsing     = 1;
    program_property->processing_profile       = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE;

}

void arad_pp_dbal_tt_program_index_as_order_nop_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
    program_property->nof_keys = 0;

    program_property->tcam_profile             = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->pd_bitmap_0              = (0x18);
    program_property->isa_key_initial_from_vt  = (0x0);
    program_property->isb_key_initial_from_vt  = (0x0);
    program_property->tcam_key_initial_from_vt = (0x0);

    program_property->processing_code          = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_MPLS;
    program_property->result_to_use_0          = (1);
    program_property->result_to_use_1          = (0);
    program_property->result_to_use_2          = (0);
    program_property->second_stage_parsing     = 1;
    program_property->processing_profile       = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE;

}

void arad_pp_dbal_tt_program_mpls4_nop_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
    program_property->nof_keys = 0;

    program_property->tcam_profile             = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->pd_bitmap_0              = (0x18);
    program_property->isa_key_initial_from_vt  = (0x0);
    program_property->isb_key_initial_from_vt  = (0x0);
    program_property->tcam_key_initial_from_vt = (0x0);

    program_property->processing_code          = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_MPLS;
    program_property->result_to_use_0          = (0);
    program_property->result_to_use_1          = (0);
    program_property->result_to_use_2          = (0);
    program_property->second_stage_parsing     = 1;
    program_property->processing_profile       = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE;

}


void arad_pp_dbal_tt_program_ipv6_router_comp_dest_found_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
    int sip_vrf_com_dest_index = 0;
    int tunnel_index = 1;
    
    program_property->nof_keys = 1;
    program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_IPV6_SIP_VRF_COMP_DEST_TCAM);
    program_property->tcam_profile        =  ARAD_PP_ISEM_ACCESS_TCAM_IPV6_CASCADE_TT_DB_PROFILE;    

    if(g_prog_soc_prop[unit].e_o_ip_enable | g_prog_soc_prop[unit].nvgre_enable | g_prog_soc_prop[unit].vxlan_enable)
    {
        program_property->vtt_table_index[1] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_TUNNEL_SEM_B);
        program_property->nof_keys++;
    }

    program_property->pd_bitmap_0              = (0x20); 
    if(g_prog_soc_prop[unit].e_o_ip_enable | g_prog_soc_prop[unit].nvgre_enable | g_prog_soc_prop[unit].vxlan_enable) {
        program_property->pd_bitmap_1              = (0x8);   
    }
    program_property->isa_key_initial_from_vt  = (0x0);
    program_property->isb_key_initial_from_vt  = (0x0);
    program_property->tcam_key_initial_from_vt = (0x0);
    program_property->processing_code          = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_IP;
    program_property->result_to_use_0          = (2);  
    program_property->result_to_use_1          = (0); 
    program_property->result_to_use_2          = (0); 
    program_property->second_stage_parsing     = (0x0);
    program_property->processing_profile       = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE;

    if (g_prog_soc_prop[unit].ipv6_term_dip_sip_enable) {
        program_property->implicit_flag = 1;
        program_property->qual_to_ce_info.qualifier_to_ce_id[sip_vrf_com_dest_index][0] = 10;  
        program_property->qual_to_ce_info.qualifier_to_ce_id[sip_vrf_com_dest_index][1] = 9;    
        program_property->qual_to_ce_info.qualifier_to_ce_id[sip_vrf_com_dest_index][2] = 8;   
        program_property->qual_to_ce_info.qualifier_to_ce_id[sip_vrf_com_dest_index][3] = 5;   
        program_property->qual_to_ce_info.qualifier_to_ce_id[sip_vrf_com_dest_index][4] = 4;   
        program_property->qual_to_ce_info.qualifier_to_ce_id[sip_vrf_com_dest_index][5] = 3;   
        program_property->qual_to_ce_info.qualifier_to_ce_id[sip_vrf_com_dest_index][6] = 2;           
        program_property->qual_to_ce_info.qualifier_to_ce_id[sip_vrf_com_dest_index][7] = 1;   

        if(g_prog_soc_prop[unit].nvgre_enable) {
            program_property->qual_to_ce_info.qualifier_to_ce_id[tunnel_index][0] = 11; 
            program_property->qual_to_ce_info.qualifier_to_ce_id[tunnel_index][1] = 7;  
            if (SOC_IS_L2GRE_VDC_ENABLE(unit)) {
                program_property->qual_to_ce_info.qualifier_to_ce_id[tunnel_index][2] = 6; 
            }
        }        
        if(g_prog_soc_prop[unit].vxlan_enable )
        {
            program_property->qual_to_ce_info.qualifier_to_ce_id[tunnel_index][0] = 11;  
            if (SOC_IS_VXLAN_VDC_ENABLE(unit)) {
                program_property->qual_to_ce_info.qualifier_to_ce_id[tunnel_index][1] = 7; 
            }
        }        
        if(g_prog_soc_prop[unit].e_o_ip_enable)
        {
            program_property->qual_to_ce_info.qualifier_to_ce_id[tunnel_index][0] = 11;  
        }
    } 
}


void arad_pp_dbal_tt_program_3mtse_plus_frr_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 vtt_3mtse_frr_type)
{
	program_property->tcam_profile             = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->processing_profile       = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE;
	program_property->nof_keys                 = 2;
	program_property->processing_code          = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_MPLS;
	program_property->second_stage_parsing     = 1;

	switch(vtt_3mtse_frr_type)
	{
	case VTT_3MTSE_FRR_L1_L1:

		program_property->pd_bitmap_0          = 0x18;
		program_property->result_to_use_0      = 2;

		if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 20) {
	        program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L3IDX_SEM_A);
	        program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L12IDX_SEM_B);
	    }
		if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 21) {
			program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L3IDX_SEM_B);
			program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L12IDX_SEM_A);
		}
		break;

	case VTT_3MTSE_FRR_L3_L3:
		program_property->pd_bitmap_0          = 0x18;
		program_property->result_to_use_0      = 1;
		program_property->result_to_use_1      = 2;
		program_property->terminate_first_label = 1;
		if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 20) {
	        program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_L3IDX_SEM_A);
            program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_L12IDX_SEM_B);
		}
		if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 21) {
			program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_L3IDX_SEM_B);
			program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_L12IDX_SEM_A);
		}
		break;

	case VTT_3MTSE_FRR_L3_L1:

		program_property->result_to_use_0      = 2;
		program_property->result_to_use_1      = 1;
		program_property->result_to_use_2      = 3;
		if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 20) {
	        program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_L3IDX_SEM_A);
	        program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L12IDX_SEM_B);
			program_property->pd_bitmap_0          = 0x08;
			program_property->pd_bitmap_1          = 0x10;
		}
		if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 21) {
			program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_L3IDX_SEM_B);
			program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L12IDX_SEM_A);
			program_property->pd_bitmap_0          = 0x10;
			program_property->pd_bitmap_1          = 0x8;
		}
		break;

	case VTT_3MTSE_FRR_L4_L3:
		program_property->result_to_use_0      = 1;
		program_property->result_to_use_1      = 2;
		program_property->result_to_use_2      = 3;
		program_property->terminate_first_label = 1;
		if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 20) {
	        program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_L3IDX_SEM_A);
	        program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_L12IDX_SEM_B);
			program_property->pd_bitmap_0          = 0x8;
			program_property->pd_bitmap_1          = 0x10;
		}
		if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 21) {
			program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L4_L3IDX_SEM_B);
			program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_L12IDX_SEM_A);
			program_property->pd_bitmap_0          = 0x10;
			program_property->pd_bitmap_1          = 0x8;
		}
		break;

	case VTT_3MTSE_FRR_L1:

		program_property->result_to_use_0      = 2;
		program_property->result_to_use_1      = 1;

		if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 20) {
			program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L3IDX_SEM_A);
			program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L12IDX_SEM_B);
			program_property->pd_bitmap_0          = 0x8;
		}
		if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 21) {
			program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L3IDX_SEM_B);
		    program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L12IDX_SEM_A);
			program_property->pd_bitmap_0          = 0x10;
		}

		break;

	case VTT_3MTSE_FRR_L2_L1:

		program_property->result_to_use_0      = 2;
		program_property->result_to_use_1      = 3;

		if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 20) {
			program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L3IDX_SEM_A);
			program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L12IDX_SEM_B);
			program_property->pd_bitmap_0          = 0x8;
			program_property->pd_bitmap_1          = 0x10;
		}
		if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 21) {
			program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L3IDX_SEM_B);
			program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L12IDX_SEM_A);
			program_property->pd_bitmap_0          = 0x10;
			program_property->pd_bitmap_1          = 0x8;
		}

		break;

	default :
		break;
	}


}

void arad_pp_dbal_tt_program_2mtse_plus_frr_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 vtt_2mtse_frr_type)
{
	program_property->tcam_profile             = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->processing_profile       = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE;
	program_property->nof_keys                 = 2;
	program_property->processing_code          = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_MPLS;
	program_property->second_stage_parsing     = 1;

	switch(vtt_2mtse_frr_type)
	{
	case VTT_2MTSE_FRR_L1_L1:

		program_property->pd_bitmap_0          = 0x18;
		program_property->result_to_use_0      = 2;

		if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 22) {
	        program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L2IDX_SEM_A);
	        program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L1IDX_SEM_B);
	    }
		if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 23) {
			program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L2IDX_SEM_B);
			program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L1IDX_SEM_A);
		}
		break;

	case VTT_2MTSE_FRR_L2_L2:
		program_property->pd_bitmap_0          = 0x18;
		program_property->result_to_use_0      = 1;
		program_property->result_to_use_1      = 2;
		if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 22) {
	        program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L2IDX_SEM_A);
            program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L1IDX_SEM_B);
		}
		if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 23) {
			program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L2IDX_SEM_B);
			program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L1IDX_SEM_A);
		}
		break;

	case VTT_2MTSE_FRR_L2_L1:
		program_property->result_to_use_0      = 2;
		program_property->result_to_use_1      = 3;
		if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 22) {
	        program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L2IDX_SEM_A);
	        program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L1IDX_SEM_B);
			program_property->pd_bitmap_0          = 0x08;
			program_property->pd_bitmap_1          = 0x10;
		}
		if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 23) {
			program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L2IDX_SEM_B);
			program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_L1IDX_SEM_A);
			program_property->pd_bitmap_0          = 0x10;
			program_property->pd_bitmap_1          = 0x08;
		}
		break;

	case VTT_2MTSE_FRR_L3_L2:
		program_property->result_to_use_0      = 1;
		program_property->result_to_use_1      = 2;
		program_property->result_to_use_2      = 3;
		if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 22) {
	        program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_L2IDX_SEM_A);
	        program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L1IDX_SEM_B);
			program_property->pd_bitmap_0          = 0x8;
			program_property->pd_bitmap_1          = 0x10;
		}
		if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 23) {
			program_property->vtt_table_index[0]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L3_L2IDX_SEM_B);
			program_property->vtt_table_index[1]  = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L2_L1IDX_SEM_A);
			program_property->pd_bitmap_0          = 0x10;
			program_property->pd_bitmap_1          = 0x08;
		}
		break;

	default :
		break;
	}

}


void arad_pp_dbal_tt_program_generalized_rch_property_mpls_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
    program_property->nof_keys = 2;
    program_property->vtt_table_index[0]             = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_UNINDEXED_SEM_A);
    program_property->vtt_table_index[1]             = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_MPLS_L1_UNINDEXED_SEM_B);
    program_property->tcam_profile                   = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->pd_bitmap_0                    = (0x18);  

    program_property->tcam_profile             = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->isa_key_initial_from_vt  = (0x0);
    program_property->isb_key_initial_from_vt  = (0x0);
    program_property->tcam_key_initial_from_vt = (0x0);

    program_property->processing_code          = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_MPLS;
    program_property->result_to_use_0          = (2);
    program_property->result_to_use_1          = (0);
    program_property->result_to_use_2          = (0);
    program_property->second_stage_parsing     = 1;
    program_property->processing_profile       = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_NONE;

}

void arad_pp_dbal_tt_program_mymac_ip_disable_trap_property_set (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
    program_property->nof_keys            = 0;
    program_property->pd_bitmap_0         = (0x28);
    program_property->processing_code     = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_IP;
    program_property->processing_profile  = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_MYMAC_IP_DISABLED_TRAP;
    program_property->tcam_profile        = ARAD_TCAM_ACCESS_PROFILE_INVALID;
}

void arad_pp_dbal_tt_program_pppoe_property_set(int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
	program_property->nof_keys			 = 1;
	program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_PPPOE_ISEM_A);
	program_property->tcam_profile		 = ARAD_TCAM_ACCESS_PROFILE_INVALID;
	program_property->pd_bitmap_0		 = 0x10;
	

	program_property->result_to_use_0	 = 2;
	program_property->result_to_use_1	 = 2;
	program_property->result_to_use_2	 = 2;

    program_property->processing_code          = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_IP;
    program_property->processing_profile       = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_PPPoE;

	
}

void arad_pp_dbal_tt_program_l2tp_property_set (int unit, ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property, uint32 unused)
{
	program_property->nof_keys           = 1;
	program_property->vtt_table_index[0] = SW_DB_DBAL2VTT(SOC_DPP_DBAL_SW_TABLE_ID_L2TP_ISEM_A);
	program_property->tcam_profile       = ARAD_TCAM_ACCESS_PROFILE_INVALID;
    program_property->pd_bitmap_0		 = 0x10;
    

	program_property->result_to_use_0    = 2;
	program_property->result_to_use_1    = 2;
	program_property->result_to_use_2    = 2;

    program_property->processing_code    = ARAD_PP_ISEM_ACCESS_PROCESSING_CODE_TYPE_IP;
    program_property->processing_profile = ARAD_PP_IHP_VTT_TT_PROCESSING_PROFILE_L2TP;

	
}









uint32 arad_pp_dbal_vtt_cam_set(int unit)
{

    ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA    vt_prog_selection;
    ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA    tt_prog_selection;
    uint32 res;
    int next_line,current_line;
    int i;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    for(i=0; i < ARAD_PP_DBAL_VT_NOF_PROGRAM_SELECTION; i++)
    {
        next_line = 0;
        current_line = 0;

        
        if(vt_dbal_program_selection[i].prog_name[unit] != ARAD_PP_DBAL_VT_PROG_INVALID)
        {
            while(next_line != -1)
            {
                ARAD_CLEAR(&vt_prog_selection, ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA, 1);

                res = arad_pp_ihp_isem_1st_program_selection_cam_tbl_get_unsafe(unit, vt_dbal_program_selection[i].priority[unit] + current_line, &vt_prog_selection);
                SOC_SAND_CHECK_FUNC_RESULT(res, 100 + i, exit);

                vt_prog_selection.llvp_prog_sel   = vt_dbal_programs[vt_dbal_program_selection[i].prog_name[unit]].prog_used[unit];
                vt_prog_selection.valid = 0x1;
                next_line = (vt_dbal_program_selection[i].vt_program_selection_set)(unit, &vt_prog_selection, next_line, vt_dbal_program_selection[i].additional_info);

                res = arad_pp_ihp_isem_1st_program_selection_cam_tbl_set_unsafe(unit, vt_dbal_program_selection[i].priority[unit] + current_line, &vt_prog_selection);
                SOC_SAND_CHECK_FUNC_RESULT(res, 200 + i, exit);

                current_line = next_line;

            }

        }
    }

    
    for(i=0; i < ARAD_PP_DBAL_TT_NOF_PROGRAM_SELECTION; i++)
    {
        next_line = 0;
        current_line=0;

        
        if(tt_dbal_program_selection[i].prog_name[unit] != ARAD_PP_DBAL_TT_PROG_INVALID)
        {
            while(next_line != -1)
            {
                ARAD_CLEAR(&tt_prog_selection, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA, 1);

                res = arad_pp_ihp_isem_2nd_program_selection_cam_tbl_get_unsafe(unit, tt_dbal_program_selection[i].priority[unit] + current_line, &tt_prog_selection);
                SOC_SAND_CHECK_FUNC_RESULT(res, 300 + i, exit);
                tt_prog_selection.llvp_prog_sel = tt_dbal_programs[tt_dbal_program_selection[i].prog_name[unit]].prog_used[unit];
                tt_prog_selection.valid = 0x1;
                
                next_line = (tt_dbal_program_selection[i].tt_program_selection_set)(unit, &tt_prog_selection, next_line, tt_dbal_program_selection[i].additional_info);

                res = arad_pp_ihp_isem_2nd_program_selection_cam_tbl_set_unsafe(unit, tt_dbal_program_selection[i].priority[unit] + current_line, &tt_prog_selection);
                SOC_SAND_CHECK_FUNC_RESULT(res, 400 + i, exit);

                current_line = next_line;
            }
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_dbal_vtt_cam_set", 0, 0);

}


int32 arad_pp_dbal_vt_cam_tm_set(int unit, ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused)
{
    prog_selection->packet_format_code       = ARAD_PARSER_PFC_RAW_AND_FTMH;  
    prog_selection->packet_format_code_mask  = ARAD_PARSER_PFC_MATCH_ONE;
    prog_selection->parser_leaf_context      = ARAD_PARSER_PLC_TM;
    prog_selection->parser_leaf_context_mask = ARAD_PARSER_PLC_MATCH_ONE;

    return -1;
}

int32 arad_pp_dbal_vt_cam_vdxinitialvid_set(int unit, ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 port_profile)
{

    if(port_profile == SOC_PPC_NOF_PORT_DEFINED_VT_PROFILES)
    {
        prog_selection->in_pp_port_vt_profile           = (0x0);
        prog_selection->in_pp_port_vt_profile_mask      = (ARAD_PP_ISEM_ACCESS_VT_PROFILE_MASK);
    }
    else
    {
        prog_selection->in_pp_port_vt_profile         = (port_profile);
        prog_selection->in_pp_port_vt_profile_mask    = (ARAD_PP_ISEM_ACCESS_VT_PROFILE_MASK_INITIAL_VID);
    }

    prog_selection->packet_format_code              = ARAD_PARSER_PFC_ETH;
    prog_selection->packet_format_code_mask         = ARAD_PARSER_PFC_MATCH_TYPE;


    if(next_line == 0)
    {
        
        prog_selection->packet_format_qualifier_1       = ARAD_PP_ISEM_ACCESS_QLFR_ETH_NULL;
        prog_selection->packet_format_qualifier_1_mask  = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID);

        return 1;
    }

    if(next_line == 1)
    {
        
        prog_selection->packet_format_qualifier_1       = ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_PCP;
        prog_selection->packet_format_qualifier_1_mask  = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_INNER_TPID);
        return -1;
    }

    return -1;
}


int32 arad_pp_dbal_vt_cam_vdxinitialvid_l1_set(int unit, ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 port_profile)
{

	if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 22 || SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 23)
    {
	    prog_selection->in_pp_port_vt_profile           = SOC_PPC_PORT_DEFINED_VT_PROFILE_DOUBLE_TAG_PRIORITY;
        prog_selection->in_pp_port_vt_profile_mask      = 0x0;
    }
    else if(port_profile == SOC_PPC_NOF_PORT_DEFINED_VT_PROFILES)
    {
		prog_selection->in_pp_port_vt_profile           = (0x0);
        prog_selection->in_pp_port_vt_profile_mask      = (ARAD_PP_ISEM_ACCESS_VT_PROFILE_MASK);
    }
    else {
        prog_selection->in_pp_port_vt_profile         = (port_profile);
        prog_selection->in_pp_port_vt_profile_mask    = (ARAD_PP_ISEM_ACCESS_VT_PROFILE_MASK_INITIAL_VID);
    }

    prog_selection->packet_format_code              = ARAD_PARSER_PFC_ETH;
    prog_selection->packet_format_code_mask         = ARAD_PARSER_PFC_MATCH_TYPE;

    if (SOC_IS_JERICHO(unit))
    {
        if(next_line == 0)
        {
            
            prog_selection->packet_format_qualifier_1       = ARAD_PP_ISEM_ACCESS_QLFR_ETH_NEXT_PROTOCOL_MPLS;
            prog_selection->packet_format_qualifier_1_mask  = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID);

            return 1;
        }

        if(next_line == 1)
        {
            
            prog_selection->packet_format_qualifier_1       = ARAD_PP_ISEM_ACCESS_QLFR_ETH_NEXT_PROTOCOL_MPLS | ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_PCP;
            prog_selection->packet_format_qualifier_1_mask  = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_INNER_TPID);
            return -1;
        }
    }
    else
    {
        if(next_line == 0)
        {
            
            prog_selection->packet_format_qualifier_1		= ARAD_PP_ISEM_ACCESS_QLFR_ETH_NULL;
            prog_selection->packet_format_qualifier_1_mask	= (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID);
            return 1;
        }

        if(next_line == 1)
        {
            
            prog_selection->packet_format_qualifier_1       = ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_PCP;
            prog_selection->packet_format_qualifier_1_mask  = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_INNER_TPID);
            return -1;
        }
    }

    return -1;
}

int32 arad_pp_dbal_vt_cam_vdxinitialvid_mpls4_set(int unit, ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 port_profile)
{

    if(port_profile == SOC_PPC_NOF_PORT_DEFINED_VT_PROFILES)
    {
        prog_selection->in_pp_port_vt_profile           = (0x0);
        prog_selection->in_pp_port_vt_profile_mask      = (ARAD_PP_ISEM_ACCESS_VT_PROFILE_MASK);
    }
    else
    {
        prog_selection->in_pp_port_vt_profile         = (port_profile);
        prog_selection->in_pp_port_vt_profile_mask    = (ARAD_PP_ISEM_ACCESS_VT_PROFILE_MASK_INITIAL_VID);
    }

    prog_selection->packet_format_code              = ARAD_PARSER_PFC_MPLS3_ETH;
    prog_selection->packet_format_code_mask         = 0;
    prog_selection->packet_format_qualifier_2 = 0;
    prog_selection->packet_format_qualifier_2_mask = 0x7fe;


    if(next_line == 0)
    {
        
        prog_selection->packet_format_qualifier_1       = ARAD_PP_ISEM_ACCESS_QLFR_ETH_NULL;
        prog_selection->packet_format_qualifier_1_mask  = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID);

        return 1;
    }

    if(next_line == 1)
    {
        
        prog_selection->packet_format_qualifier_1       = ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_PCP;
        prog_selection->packet_format_qualifier_1_mask  = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_INNER_TPID);
        return -1;
    }

    return -1;
}

int32 arad_pp_dbal_vt_cam_vdxoutervid_mpls4_set(int unit,ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int line, uint32 unused)
{

    prog_selection->packet_format_code              = ARAD_PARSER_PFC_MPLS3_ETH;
    prog_selection->packet_format_code_mask         = 0;
    prog_selection->packet_format_qualifier_2 = 0;
    prog_selection->packet_format_qualifier_2_mask = 0x7fe;

    prog_selection->in_pp_port_vt_profile           = (0x0);
    prog_selection->in_pp_port_vt_profile_mask      = (ARAD_PP_ISEM_ACCESS_VT_PROFILE_MASK_EXCEPT_INITIAL_VID);

    if(line==0)
    {
        
        prog_selection->packet_format_qualifier_1       = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_TPID1);
        prog_selection->packet_format_qualifier_1_mask  = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP);
        return 1;
    }

    if(line==1)
    {
        
        prog_selection->packet_format_qualifier_1       = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_TPID2);
        prog_selection->packet_format_qualifier_1_mask  = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP);
        return -1;
    }

    return -1;
}



int32 arad_pp_dbal_vt_cam_mpls_port_termination_set(int unit, ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int line, uint32 unused) {
    
    prog_selection->in_pp_port_vt_profile =  (SOC_PPC_PORT_DEFINED_VT_PORT_TERMINATION);
    prog_selection->in_pp_port_vt_profile_mask = (0x0);
    prog_selection->packet_format_qualifier_1       = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_NEXT_PROTOCOL_MPLS);
    prog_selection->packet_format_qualifier_1_mask  = (0x7F);
    return -1;

}


int32 arad_pp_dbal_vt_cam_vdxoutervid_set(int unit,ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int line, uint32 unused)
{

    prog_selection->packet_format_code              = ARAD_PARSER_PFC_ETH;
    prog_selection->packet_format_code_mask         = ARAD_PARSER_PFC_MATCH_TYPE;
    prog_selection->in_pp_port_vt_profile           = (0x0);
    prog_selection->in_pp_port_vt_profile_mask      = (ARAD_PP_ISEM_ACCESS_VT_PROFILE_MASK_EXCEPT_INITIAL_VID);

    if(line==0)
    {
        
        prog_selection->packet_format_qualifier_1       = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_TPID1);
        prog_selection->packet_format_qualifier_1_mask  = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP);
        return 1;
    }

    if(line==1)
    {
        
        prog_selection->packet_format_qualifier_1       = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_TPID2);
        prog_selection->packet_format_qualifier_1_mask  = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP);
        return -1;
    }

    return -1;
}


int32 arad_pp_dbal_vt_cam_vdxoutervid_l1_set(int unit,ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int line, uint32 unused)
{

    prog_selection->packet_format_code              = ARAD_PARSER_PFC_ETH;
    prog_selection->packet_format_code_mask         = ARAD_PARSER_PFC_MATCH_TYPE;

    if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 22 || SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 23)
    {  prog_selection->in_pp_port_vt_profile           = SOC_PPC_PORT_DEFINED_VT_PROFILE_DOUBLE_TAG_PRIORITY;
       prog_selection->in_pp_port_vt_profile_mask      = 0x0;
    }
    else {
    	prog_selection->in_pp_port_vt_profile           = (0x0);
    	prog_selection->in_pp_port_vt_profile_mask      = (ARAD_PP_ISEM_ACCESS_VT_PROFILE_MASK_EXCEPT_INITIAL_VID);
    }

    if (SOC_IS_JERICHO(unit))
    {
        if(line==0)
        {
            
            prog_selection->packet_format_qualifier_1       = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_NEXT_PROTOCOL_MPLS | ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_TPID1);
            prog_selection->packet_format_qualifier_1_mask  =  ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP;
            return 1;
        }

        if(line==1)
        {
            
            prog_selection->packet_format_qualifier_1       = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_NEXT_PROTOCOL_MPLS | ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_TPID2);
            prog_selection->packet_format_qualifier_1_mask  = ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP;
            return -1;
        }
    }
    else
    {
        if(line==0)
        {
            
            prog_selection->packet_format_qualifier_1       = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_TPID1);
            prog_selection->packet_format_qualifier_1_mask  = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP);
            return 1;
        }

        if(line==1)
        {
            
            prog_selection->packet_format_qualifier_1       = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_TPID2);
            prog_selection->packet_format_qualifier_1_mask  = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP);
            return -1;
        }
    }

    return -1;
}

int32 arad_pp_dbal_vt_cam_vdxouter_inner_vid_set(int unit, ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int line, uint32 unused)
{
    
    prog_selection->packet_format_qualifier_1      = (0x0); 
    prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_INNER_TPID | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_PCP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID);
    prog_selection->packet_format_code              = ARAD_PARSER_PFC_ETH;
    prog_selection->packet_format_code_mask         = ARAD_PARSER_PFC_MATCH_TYPE;
    prog_selection->in_pp_port_vt_profile          = (0x0);
    prog_selection->in_pp_port_vt_profile_mask     = (ARAD_PP_ISEM_ACCESS_VT_PROFILE_MASK);

    return -1;
}

int32 arad_pp_dbal_vt_cam_vdxouter_inner_vid_or_outer_vid_set(int unit, ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int line, uint32 unused)
{
    prog_selection->packet_format_code              = ARAD_PARSER_PFC_ETH;
    prog_selection->packet_format_code_mask         = ARAD_PARSER_PFC_MATCH_TYPE;
    prog_selection->in_pp_port_vt_profile          = SOC_PPC_PORT_DEFINED_VT_PROFILE_DOUBLE_TAG_PRIORITY;
    prog_selection->in_pp_port_vt_profile_mask     = (0x0);


    if(line==0)
    {
        prog_selection->packet_format_qualifier_1      =  (ARAD_PP_ISEM_ACCESS_QLFR_ETH_INNER_TPID1);
        prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID);
        return 1;
    }


    if(line==1)
    {
        
        prog_selection->packet_format_qualifier_1      =  (ARAD_PP_ISEM_ACCESS_QLFR_ETH_INNER_TPID2);
        prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID);
        return -1;
    }
    return -1;
}





int32 arad_pp_dbal_vt_cam_initialvid_l1frr_set(int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused)
{

    prog_selection->packet_format_qualifier_2      = (0x0);
    prog_selection->packet_format_qualifier_2_mask = (ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L1 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L2 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L3 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_BOS_NEXT_PROT);
    prog_selection->packet_format_code              = ARAD_PARSER_PFC_ETH;
    prog_selection->packet_format_code_mask         = ARAD_PARSER_PFC_MATCH_TYPE;
    prog_selection->in_pp_port_vt_profile          = SOC_PPC_PORT_DEFINED_VT_PROFILE_FRR_COUPLING; 
    prog_selection->in_pp_port_vt_profile_mask     = (ARAD_PP_ISEM_ACCESS_VT_PROFILE_MASK_INITIAL_VID | ARAD_PP_ISEM_ACCESS_VT_PROFILE_MASK_DOUBLE_TAG);

    if(next_line == 0)
    {
        
        prog_selection->packet_format_qualifier_1      = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_NEXT_PROTOCOL_MPLS | ARAD_PP_ISEM_ACCESS_QLFR_ETH_NULL);
        prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID);
        return 1;
    }

    if(next_line == 1)
    {
        
        prog_selection->packet_format_qualifier_1      = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_NEXT_PROTOCOL_MPLS | ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_PCP);
        prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_INNER_TPID);
        return -1;
    }
    return -1;
}


int32 arad_pp_dbal_vt_cam_outervid_l1frr_set(int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused)
{
    prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP);
    prog_selection->packet_format_qualifier_2      = (0x0);
    prog_selection->packet_format_qualifier_2_mask = (ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L1 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L2 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L3 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_BOS_NEXT_PROT);
    prog_selection->packet_format_code              = ARAD_PARSER_PFC_ETH;
    prog_selection->packet_format_code_mask         = ARAD_PARSER_PFC_MATCH_TYPE;
    prog_selection->in_pp_port_vt_profile          = SOC_PPC_PORT_DEFINED_VT_PROFILE_FRR_COUPLING;
    prog_selection->in_pp_port_vt_profile_mask     = (0x0);

    if(next_line == 0)
    {
        prog_selection->packet_format_qualifier_1 = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_NEXT_PROTOCOL_MPLS | ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_TPID1);
        return 1;
    }

    if(next_line == 1)
    {
        prog_selection->packet_format_qualifier_1 = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_NEXT_PROTOCOL_MPLS | ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_TPID2);
        return -1;
    }
    return -1;
}

int32 arad_pp_dbal_vt_cam_vdxinitialvid_l1l2_set(int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused)
{
    prog_selection->packet_format_qualifier_2      = (0x0);
    prog_selection->packet_format_qualifier_2_mask = (ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L1 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L2 |ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L3 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_BOS_NEXT_PROT);
    prog_selection->packet_format_code              = ARAD_PARSER_PFC_ETH;
    prog_selection->packet_format_code_mask         = ARAD_PARSER_PFC_MATCH_TYPE;
    prog_selection->in_pp_port_vt_profile          = (SOC_PPC_PORT_DEFINED_VT_PROFILE_FRR_COUPLING); 
    prog_selection->in_pp_port_vt_profile_mask     = (ARAD_PP_ISEM_ACCESS_VT_PROFILE_MASK_INITIAL_VID);

    if(next_line == 0)
    {
        prog_selection->packet_format_qualifier_1      = ARAD_PP_ISEM_ACCESS_QLFR_ETH_NULL;
        prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID);
        return 1;
    }
    if(next_line == 1)
    {
        prog_selection->packet_format_qualifier_1      = ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_PCP;
        prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_INNER_TPID);
        return -1;
    }
    return -1;
}


int32 arad_pp_dbal_vt_cam_vdxoutervid_l1l2_set(int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused)
{
    prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP);
    prog_selection->packet_format_qualifier_2     = (0x0);
    prog_selection->packet_format_qualifier_2_mask = (ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L1 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L2 |ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L3 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_BOS_NEXT_PROT);
    prog_selection->packet_format_code              = ARAD_PARSER_PFC_ETH;
    prog_selection->packet_format_code_mask         = ARAD_PARSER_PFC_MATCH_TYPE;
    prog_selection->in_pp_port_vt_profile         = (SOC_PPC_PORT_DEFINED_VT_PROFILE_FRR_COUPLING);
    prog_selection->in_pp_port_vt_profile_mask     = (0x0);

    if(next_line == 0)
    {
        prog_selection->packet_format_qualifier_1     = ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_TPID1;
        return 1;
    }
    if(next_line == 1)
    {
        prog_selection->packet_format_qualifier_1     = ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_TPID2;
        return -1;
    }
    return -1;
}

int32 arad_pp_dbal_vt_cam_vdxinitialvid_trill_set(int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused )
{
    prog_selection->packet_format_code              = ARAD_PARSER_PFC_ETH;
    prog_selection->packet_format_code_mask         = ARAD_PARSER_PFC_MATCH_TYPE;
    prog_selection->in_pp_port_vt_profile          = (SOC_PPC_PORT_DEFINED_VT_PROFILE_TRILL); 
    prog_selection->in_pp_port_vt_profile_mask     = (ARAD_PP_ISEM_ACCESS_VT_PROFILE_MASK_INITIAL_VID);

    if(next_line==0)
    {
        
        prog_selection->packet_format_qualifier_1      = ARAD_PP_ISEM_ACCESS_QLFR_ETH_NULL;
        prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID);
        return 1;
    }

    if(next_line==1)
    {

        
        prog_selection->packet_format_qualifier_1      = ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_PCP;
        prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_INNER_TPID);
        return -1;
    }
    return -1;
}

int32 arad_pp_dbal_vt_cam_vdxoutervid_trill_set(int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused )
{
    prog_selection->packet_format_code              = ARAD_PARSER_PFC_ETH;
    prog_selection->packet_format_code_mask         = ARAD_PARSER_PFC_MATCH_TYPE;
    prog_selection->in_pp_port_vt_profile          = (SOC_PPC_PORT_DEFINED_VT_PROFILE_TRILL);
    prog_selection->in_pp_port_vt_profile_mask     = (0x0);

    if(next_line==0)
    {
        
        prog_selection->packet_format_qualifier_1      = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_TPID1);
        prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_INNER_TPID);
        return 1;
    }

    if(next_line==1)
    {
        
        prog_selection->packet_format_qualifier_1      = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_TPID2);
        prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_INNER_TPID);
        return -1;
    }
    return -1;

}

int32 arad_pp_dbal_vt_cam_vdxouter_inner_vid_trill_set(int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused )
{

    prog_selection->packet_format_qualifier_1      = 0x0; 
    prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_PCP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID);
    prog_selection->packet_format_code              = ARAD_PARSER_PFC_ETH;
    prog_selection->packet_format_code_mask         = ARAD_PARSER_PFC_MATCH_TYPE;
    prog_selection->in_pp_port_vt_profile          = (SOC_PPC_PORT_DEFINED_VT_PROFILE_TRILL); 
    prog_selection->in_pp_port_vt_profile_mask     = (0x1); 
    return -1;
}

int32 arad_pp_dbal_vt_cam_vd_designated_vid_trill_set(int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused )
{
    prog_selection->packet_format_code             = ARAD_PARSER_PFC_ETH_TRILL_ETH;
    prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ONE;
    prog_selection->in_pp_port_vt_profile          = (SOC_PPC_PORT_DEFINED_VT_PROFILE_TRILL);
    prog_selection->in_pp_port_vt_profile_mask     = (0x0);

    if(next_line==0)
    {
        
        prog_selection->packet_format_qualifier_1      = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_TPID1);
        prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_INNER_TPID);
        return 1;
    }

    if(next_line==1)
    {
        
        prog_selection->packet_format_qualifier_1      = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_TPID2);
        prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_INNER_TPID);
        return -1;
    }
    return -1;
}

int32 arad_pp_dbal_vt_cam_ipv4_vdxinitialvid_set(int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused)
{
    prog_selection->packet_format_code           = ARAD_PARSER_PFC_IPV4_ETH;
    prog_selection->packet_format_code_mask      = ARAD_PARSER_PFC_MATCH_ANY_ENCAP_ANY_TYPE; 

    if(next_line == 0)
    {
        
        prog_selection->packet_format_qualifier_1      = ARAD_PP_ISEM_ACCESS_QLFR_ETH_NULL; 
        prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID);
        return 1;
    }

    if(next_line == 1)
    {
        
        prog_selection->packet_format_qualifier_1     = ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_PCP; 
        prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_INNER_TPID);
        return -1;
    }
    return -1;
}

int32 arad_pp_dbal_vt_cam_ipv4_port_termination_set(int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused)
{
    prog_selection->packet_format_code         = ARAD_PARSER_PFC_ETH_IPV4_ETH;
    prog_selection->packet_format_code_mask    = ARAD_PARSER_PFC_MATCH_ONE;
    prog_selection->in_pp_port_vt_profile      = (SOC_PPC_PORT_DEFINED_VT_PORT_TERMINATION);
    prog_selection->in_pp_port_vt_profile_mask =  0;

    return -1;
}

int32 arad_pp_dbal_vt_cam_ipv4_vdxoutervid_set(int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused)
{
    prog_selection->packet_format_code         = ARAD_PARSER_PFC_IPV4_ETH;  
    prog_selection->packet_format_code_mask    = ARAD_PARSER_PFC_MATCH_ANY_ENCAP_ANY_TYPE; 
    prog_selection->in_pp_port_vt_profile      = (0x0);
    prog_selection->in_pp_port_vt_profile_mask =  ARAD_PP_ISEM_ACCESS_VT_PROFILE_MASK_EXCEPT_INITIAL_VID;

    if(next_line == 0)
    {
        prog_selection->packet_format_qualifier_1      = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_TPID1); 
        prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP);
        return 1;
    }

    if(next_line == 1)
    {
        prog_selection->packet_format_qualifier_1      = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_TPID2); 
        prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP);
        return -1;
    }
    return -1;
}


int32 arad_pp_dbal_vt_cam_ipv6_vdxinitialvid_set(int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 is_recycle)
{
    prog_selection->packet_format_code          = ARAD_PARSER_PFC_IPV6_ETH;
    prog_selection->packet_format_code_mask     = ARAD_PARSER_PFC_MATCH_ANY_ENCAP_ANY_TYPE; 
    prog_selection->in_pp_port_vt_profile       = SOC_PPC_PORT_DEFINED_VT_PORT_TERMINATION_CASCADE;
    prog_selection->in_pp_port_vt_profile_mask  =  0;
    if (is_recycle) 
    {
        prog_selection->ptc_vt_profile             = ARAD_PORTS_VT_PROFILE_OVERLAY_RCY;
        prog_selection->ptc_vt_profile_mask        = 0x0;
    }

    if(next_line == 0)
    {
        
        prog_selection->packet_format_qualifier_1      = ARAD_PP_ISEM_ACCESS_QLFR_ETH_NULL; 
        prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID);
        return 1;
    }

    if(next_line == 1)
    {
        
        prog_selection->packet_format_qualifier_1     = ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_PCP; 
        prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_INNER_TPID);
        return -1;
    }
    return -1;
}

int32 arad_pp_dbal_vt_cam_ipv6_vdxoutervid_set(int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 is_recycle)
{
    prog_selection->packet_format_code         = ARAD_PARSER_PFC_IPV6_ETH;  
    prog_selection->packet_format_code_mask    = ARAD_PARSER_PFC_MATCH_ANY_ENCAP_ANY_TYPE; 
    prog_selection->in_pp_port_vt_profile      = SOC_PPC_PORT_DEFINED_VT_PORT_TERMINATION_CASCADE;
    prog_selection->in_pp_port_vt_profile_mask =  0xE;
    if (is_recycle) 
    {
        prog_selection->ptc_vt_profile             = ARAD_PORTS_VT_PROFILE_OVERLAY_RCY;
        prog_selection->ptc_vt_profile_mask        = 0x0;
    }

    if(next_line == 0)
    {
        prog_selection->packet_format_qualifier_1      = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_TPID1); 
        prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP);
        return 1;
    }

    if(next_line == 1)
    {
        prog_selection->packet_format_qualifier_1      = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_TPID2); 
        prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP);
        return -1;
    }
    return -1;
}

int32 arad_pp_dbal_vt_cam_evb_set(int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 vid_type)
{
    prog_selection->packet_format_code         = ARAD_PARSER_PFC_ETH;
    prog_selection->packet_format_code_mask    = ARAD_PARSER_PFC_MATCH_TYPE;
    prog_selection->in_pp_port_vt_profile      = (SOC_PPC_PORT_DEFINED_VT_PROFILE_EVB); 
    prog_selection->in_pp_port_vt_profile_mask = 0x0;

    if(vid_type == OUTER_VID)
    {
        
        prog_selection->packet_format_qualifier_1      = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_NULL); 
        prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_PCP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID);
    }
    else
    {
        prog_selection->packet_format_qualifier_1      = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_NULL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_TPID1); 
        prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_PCP);
    }
    return -1;
}

int32 arad_pp_dbal_vt_cam_double_tag_pri_set(int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused)
{
    prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID);
    prog_selection->packet_format_code              = ARAD_PARSER_PFC_ETH;
    prog_selection->packet_format_code_mask         = ARAD_PARSER_PFC_MATCH_TYPE;
    prog_selection->in_pp_port_vt_profile          = SOC_PPC_PORT_DEFINED_VT_PROFILE_DOUBLE_TAG_PRIORITY_INITIAL_VID;
    prog_selection->in_pp_port_vt_profile_mask     = (0x0);

    if(next_line == 0)
    {
        prog_selection->packet_format_qualifier_1     = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_INNER_TPID1); 
        return 1;
    }

    if(next_line == 1)
    {
        prog_selection->packet_format_qualifier_1     = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_INNER_TPID2); 
        return -1;
    }
    return -1;
}

int32 arad_pp_dbal_vt_cam_double_tag_set(int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused)
{
    prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID);
    prog_selection->packet_format_code              = ARAD_PARSER_PFC_ETH;
    prog_selection->packet_format_code_mask         = ARAD_PARSER_PFC_MATCH_TYPE;
    prog_selection->in_pp_port_vt_profile          = SOC_PPC_PORT_DEFINED_VT_PROFILE_USE_INITIAL_VID;
    prog_selection->in_pp_port_vt_profile_mask     = (0x0);

    if(next_line == 0)
    {
        prog_selection->packet_format_qualifier_1     = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_INNER_TPID1); 
        return 1;
    }

    if(next_line == 1)
    {
        prog_selection->packet_format_qualifier_1     = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_INNER_TPID2); 
        return -1;
    }
    return -1;
}

int32 arad_pp_dbal_vt_cam_ipv4_vdxinitialvid_recycle_set(int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused)
{
    prog_selection->packet_format_code             = ARAD_PARSER_PFC_IPV4_ETH;  
    prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP_ANY_TYPE; 
    prog_selection->ptc_vt_profile                 = (ARAD_PORTS_VT_PROFILE_OVERLAY_RCY);
    prog_selection->ptc_vt_profile_mask            = (0x0);

    if(next_line == 0)
    {
        
        prog_selection->packet_format_qualifier_1      = ARAD_PP_ISEM_ACCESS_QLFR_ETH_NULL; 
        prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID);
        return 1;
    }

    if(next_line == 1)
    {
        
        prog_selection->packet_format_qualifier_1      = ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_PCP; 
        prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_INNER_TPID);
        return -1;
    }

    return -1;
}
int32 arad_pp_dbal_vt_cam_ipv4_vdxoutervid_recycle_set(int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused)
{
    prog_selection->packet_format_code             = ARAD_PARSER_PFC_IPV4_ETH;  
    prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP_ANY_TYPE; 
    prog_selection->in_pp_port_vt_profile          = (0x0);
    prog_selection->in_pp_port_vt_profile_mask     =  ARAD_PP_ISEM_ACCESS_VT_PROFILE_MASK_EXCEPT_INITIAL_VID;
    prog_selection->ptc_vt_profile                 = (ARAD_PORTS_VT_PROFILE_OVERLAY_RCY);
    prog_selection->ptc_vt_profile_mask            = (0x0);

    if(next_line == 0)
    {
        prog_selection->packet_format_qualifier_1      = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_TPID1); 
        prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP);
        return 1;
    }

    if(next_line == 1)
    {
        prog_selection->packet_format_qualifier_1      = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_TPID2); 
        prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP);
        return -1;
    }

    return -1;
}

int32 arad_pp_dbal_vt_cam_5_tuple_qinq_set(int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 vid_type)
{
    prog_selection->packet_format_qualifier_1      = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_NULL);
    prog_selection->packet_format_code             = ARAD_PARSER_PFC_IPV4_ETH;  
    prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP_ANY_TYPE; 
    prog_selection->in_pp_port_vt_profile          = (SOC_PPC_PORT_DEFINED_VT_PROFILE_FLEXIBLE_Q_IN_Q);
    prog_selection->in_pp_port_vt_profile_mask     = 0x0;

    if(vid_type == TST1_VID_DOUBLE_TAG)
        prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL |
                                                          ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP        |
                                                          ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_PCP    |
                                                          ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID   |
                                                          ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_INNER_TPID);
    else
        prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL |
                                                          ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP        |
                                                          ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_PCP    |
                                                          ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID);
    return -1;
}

int32 arad_pp_dbal_vt_cam_untagged_tst1_set(int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused)
{
    prog_selection->packet_format_qualifier_1      = (0x0); 
    prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP);
    prog_selection->packet_format_code             = ARAD_PARSER_PFC_ETH;
    prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_TYPE;
    prog_selection->in_pp_port_vt_profile          = (0x0);
    prog_selection->in_pp_port_vt_profile_mask     = (ARAD_PP_ISEM_ACCESS_VT_PROFILE_MASK);

    return -1;
}
int32 arad_pp_dbal_vt_cam_one_tag_tst1_set(int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused)
{
    
    prog_selection->packet_format_qualifier_1      = (0x0); 
    prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL |
                                                      ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP        |
                                                      ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID   |
                                                      ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_PCP    |
                                                      ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_INNER_TPID);
    prog_selection->packet_format_code             = ARAD_PARSER_PFC_ETH;
    prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_TYPE;
    prog_selection->in_pp_port_vt_profile          = (0x0);
    prog_selection->in_pp_port_vt_profile_mask     = (ARAD_PP_ISEM_ACCESS_VT_PROFILE_MASK);

    return -1;

}
int32 arad_pp_dbal_vt_cam_double_tag_tst1_set(int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused)
{
    prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL |
                                                      ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP        |
                                                      ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_PCP    |
                                                      ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID);
    prog_selection->packet_format_code             = ARAD_PARSER_PFC_ETH;
    prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_TYPE;
    prog_selection->in_pp_port_vt_profile          = (SOC_PPC_PORT_DEFINED_VT_PROFILE_DEFAULT);
    prog_selection->in_pp_port_vt_profile_mask     = (ARAD_PP_ISEM_ACCESS_VT_PROFILE_MASK);

    if(next_line == 0)
    {
        
        prog_selection->packet_format_qualifier_1       = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_INNER_TPID1);
        return 1;
    }

    if(next_line==1)
    {
        prog_selection->packet_format_qualifier_1       = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_INNER_TPID2);
        return -1;
    }

    return -1;
}


int32 arad_pp_dbal_vt_cam_untagged_vrrp_set(int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused)
{
    prog_selection->packet_format_qualifier_1      = (0x0);
    prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP);
    prog_selection->packet_format_code             = ARAD_PARSER_PFC_IPV4_ETH;  
    prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP_ANY_TYPE_ANY_IP;
    if (SOC_IS_JERICHO(unit)) {
        prog_selection->in_pp_port_vt_profile      = 11;
    }
    else {
        prog_selection->in_pp_port_vt_profile      = 5;
    }
    prog_selection->in_pp_port_vt_profile_mask     = 0x0;

    return -1;
}
int32 arad_pp_dbal_vt_cam_one_tag_vrrp_set(int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused)
{
    
    prog_selection->packet_format_qualifier_1      = (0x0); 
    prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL |
                                                      ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP        |
                                                      ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID   |
                                                      ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_PCP    |
                                                      ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_INNER_TPID);
    prog_selection->packet_format_code             = ARAD_PARSER_PFC_IPV4_ETH;   
    prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP_ANY_TYPE_ANY_IP;
    if (SOC_IS_JERICHO(unit)) {
        prog_selection->in_pp_port_vt_profile      = 11;
    }
    else {
        prog_selection->in_pp_port_vt_profile      = 5;
    }
    prog_selection->in_pp_port_vt_profile_mask     = (0x0);

    return -1;

}
int32 arad_pp_dbal_vt_cam_double_tag_vrrp_set(int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused)
{
    prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL |
                                                      ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP        |
                                                      ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_PCP    |
                                                      ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID);
    prog_selection->packet_format_code             = ARAD_PARSER_PFC_IPV4_ETH;
    prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP_ANY_TYPE_ANY_IP;
    if (SOC_IS_JERICHO(unit)) {
        prog_selection->in_pp_port_vt_profile      = 11;
    }
    else {
        prog_selection->in_pp_port_vt_profile      = 5;
    }

    prog_selection->in_pp_port_vt_profile_mask     = (0x0);

    if(next_line == 0)
    {
        
        prog_selection->packet_format_qualifier_1       = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_INNER_TPID1);
        return 1;
    }

    if(next_line==1)
    {
        prog_selection->packet_format_qualifier_1       = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_INNER_TPID2);
        return -1;
    }

    return -1;
}


int32 arad_pp_dbal_vt_cam_untagged_exp_null_tst1_set(int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused)
{
    
    prog_selection->packet_format_qualifier_1      = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_NEXT_PROTOCOL_MPLS); 
    prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP);
    prog_selection->packet_format_code             = ARAD_PARSER_PFC_ETH;
    prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_TYPE;
    prog_selection->in_pp_port_vt_profile_mask     = (0x0);

    if(next_line == 0)
    {
        prog_selection->in_pp_port_vt_profile      = (SOC_PPC_PORT_DEFINED_VT_PROFILE_EXPLICIT_NULL);
        return 1;
    }

    if(next_line==1)
    {
        
        prog_selection->in_pp_port_vt_profile      = (SOC_PPC_PORT_DEFINED_VT_PROFILE_IGNORE_2ND_TAG_EXPLICIT_NULL);
        return -1;
    }

    return -1;
}
int32 arad_pp_dbal_vt_cam_one_tag_exp_null_tst1_set(int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused)
{
    
    prog_selection->packet_format_qualifier_1      = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_NEXT_PROTOCOL_MPLS); 
    prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP      |
                                                      ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID |
                                                      ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_PCP  |
                                                      ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_INNER_TPID);
    prog_selection->packet_format_code              = ARAD_PARSER_PFC_ETH;
    prog_selection->packet_format_code_mask         = ARAD_PARSER_PFC_MATCH_TYPE;
    prog_selection->in_pp_port_vt_profile_mask     = (0x0);

    if(next_line == 0)
    {
        prog_selection->in_pp_port_vt_profile      = (SOC_PPC_PORT_DEFINED_VT_PROFILE_EXPLICIT_NULL);
        return 1;
    }
    if(next_line==1)
    {
        
        prog_selection->in_pp_port_vt_profile      = (SOC_PPC_PORT_DEFINED_VT_PROFILE_IGNORE_2ND_TAG_EXPLICIT_NULL);
        return -1;
    }

    return -1;
}
int32 arad_pp_dbal_vt_cam_double_tag_exp_null_tst1_set(int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused)
{
    
     prog_selection->packet_format_qualifier_1_mask  = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP     |
                                                        ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_PCP |
                                                        ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID);
     prog_selection->packet_format_code              = ARAD_PARSER_PFC_ETH;
     prog_selection->packet_format_code_mask         = ARAD_PARSER_PFC_MATCH_TYPE;
     prog_selection->in_pp_port_vt_profile           = (SOC_PPC_PORT_DEFINED_VT_PROFILE_EXPLICIT_NULL);
     prog_selection->in_pp_port_vt_profile_mask      = (0x0);

     if(next_line == 0)
     {
         prog_selection->packet_format_qualifier_1       = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_NEXT_PROTOCOL_MPLS | ARAD_PP_ISEM_ACCESS_QLFR_ETH_INNER_TPID1);
         return 1;
     }
     if(next_line==1)
     {
         prog_selection->packet_format_qualifier_1       = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_NEXT_PROTOCOL_MPLS | ARAD_PP_ISEM_ACCESS_QLFR_ETH_INNER_TPID2);
         return -1;
     }

     return -1;
}

int32 arad_pp_dbal_vt_cam_outer_inner_pcp_tst2_set(int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int line, uint32 tpid)
{
    prog_selection->packet_format_code              = ARAD_PARSER_PFC_ETH;
    prog_selection->packet_format_code_mask         = ARAD_PARSER_PFC_MATCH_TYPE;
    prog_selection->in_pp_port_vt_profile          = SOC_PPC_PORT_DEFINED_VT_PROFILE_PCP_ENABLE;
    prog_selection->in_pp_port_vt_profile_mask     = (0x0);

    if(line==0)
    {
        prog_selection->packet_format_qualifier_1      = (((tpid == TPID1) ? ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_TPID1 : ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_TPID2) | ARAD_PP_ISEM_ACCESS_QLFR_ETH_INNER_TPID1 );
        prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP |  ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_PCP);
        return 1;
    }
    if(line==1)
    {
        prog_selection->packet_format_qualifier_1      = (((tpid == TPID1) ? ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_TPID1 : ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_TPID2) | ARAD_PP_ISEM_ACCESS_QLFR_ETH_INNER_TPID2 );
        prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP |  ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_PCP);
        return -1;
    }
    return -1;
}




int32 arad_pp_dbal_vt_cam_vdxiniitialvid_l1_l2eli_set(int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused )
{
    prog_selection->packet_format_qualifier_2      = (ARAD_PP_ISEM_ACCESS_QLFR_MPLS_L2ELI | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_L1);
    prog_selection->packet_format_qualifier_2_mask = (ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L3 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_BOS_NEXT_PROT);
    prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS2_ETH; 
    prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_AT_LEAST_MPLS2;
    prog_selection->in_pp_port_vt_profile          = (0x0);
    prog_selection->in_pp_port_vt_profile_mask     = (ARAD_PP_ISEM_ACCESS_VT_PROFILE_MASK);

    if(next_line == 0)
    {
        
        prog_selection->packet_format_qualifier_1      = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_NEXT_PROTOCOL_MPLS | ARAD_PP_ISEM_ACCESS_QLFR_ETH_NULL);
        prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID);
        return 1;
    }

    if(next_line == 1)
    {
        prog_selection->packet_format_qualifier_1      = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_NEXT_PROTOCOL_MPLS | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_PCP);
        prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_INNER_TPID);
        return -1;
    }

    return -1;
}


int32 arad_pp_dbal_vt_cam_vdxoutervid_l1_l2eli_set(int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused )
{
    prog_selection->packet_format_qualifier_2      = (ARAD_PP_ISEM_ACCESS_QLFR_MPLS_L2ELI | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_L1);
    prog_selection->packet_format_qualifier_2_mask = (ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L3 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_BOS_NEXT_PROT);
    prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS2_ETH; 
    prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_AT_LEAST_MPLS2;
    prog_selection->in_pp_port_vt_profile          = (0x0);
    prog_selection->in_pp_port_vt_profile_mask     = (ARAD_PP_ISEM_ACCESS_VT_PROFILE_MASK_EXCEPT_INITIAL_VID);

    if(next_line == 0)
    {
        
        prog_selection->packet_format_qualifier_1      = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_NEXT_PROTOCOL_MPLS | ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_TPID1);
        prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP);
        return 1;
    }
    if(next_line == 1)
    {
        
        prog_selection->packet_format_qualifier_1      = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_NEXT_PROTOCOL_MPLS | ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_TPID2);
        prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP);
        return -1;
    }
    return -1;
}
int32 arad_pp_dbal_vt_cam_outer_pcp_tst2_set(int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int line, uint32 tpid)
{
    prog_selection->packet_format_qualifier_1      = ((tpid == TPID1) ? ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_TPID1 : ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_TPID2);
    prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP |  ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_PCP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_INNER_TPID);
    prog_selection->packet_format_code             = ARAD_PARSER_PFC_ETH;
    prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_TYPE;
    prog_selection->in_pp_port_vt_profile          = SOC_PPC_PORT_DEFINED_VT_PROFILE_PCP_ENABLE;
    prog_selection->in_pp_port_vt_profile_mask     = (0x0);
    return -1;
}
int32 arad_pp_dbal_vt_cam_outer_inner_tst2_set(int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int line, uint32 tpid)
{
    prog_selection->packet_format_code             = ARAD_PARSER_PFC_ETH;
    prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_TYPE;
    prog_selection->in_pp_port_vt_profile          = SOC_PPC_PORT_DEFINED_VT_PROFILE_DEFAULT;
    prog_selection->in_pp_port_vt_profile_mask     = ARAD_PP_ISEM_ACCESS_VT_PROFILE_MASK;

    if(line==0)
    {
        prog_selection->packet_format_qualifier_1      = (((tpid == TPID1) ? ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_TPID1 : ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_TPID2) | ARAD_PP_ISEM_ACCESS_QLFR_ETH_INNER_TPID1 );
        prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP |  ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_PCP);
        return 1;
    }
    if(line==1)
    {
        prog_selection->packet_format_qualifier_1      = (((tpid == TPID1) ? ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_TPID1 : ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_TPID2) | ARAD_PP_ISEM_ACCESS_QLFR_ETH_INNER_TPID2 );
        prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP |  ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_PCP);
        return -1;
    }
    return -1;
}

int32 arad_pp_dbal_vt_cam_outer_tst2_set(int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int line, uint32 tpid)
{
    prog_selection->packet_format_qualifier_1      = ((tpid == TPID1) ? ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_TPID1 : ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_TPID2);
    prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP |  ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_PCP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_INNER_TPID);
    prog_selection->packet_format_code             = ARAD_PARSER_PFC_ETH;
    prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_TYPE;
    prog_selection->in_pp_port_vt_profile          = SOC_PPC_PORT_DEFINED_VT_PROFILE_DEFAULT;
    prog_selection->in_pp_port_vt_profile_mask     = ARAD_PP_ISEM_ACCESS_VT_PROFILE_MASK;
    return -1;
}

int32 arad_pp_dbal_vt_cam_untagged_tst2_set(int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int line, uint32 unused)
{
    prog_selection->packet_format_qualifier_1      = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_NULL);
    prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP |  ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_PCP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_INNER_TPID);
    prog_selection->packet_format_code              = ARAD_PARSER_PFC_ETH;
    prog_selection->packet_format_code_mask         = ARAD_PARSER_PFC_MATCH_TYPE;
    prog_selection->in_pp_port_vt_profile_mask     = (ARAD_PP_ISEM_ACCESS_VT_PROFILE_MASK);
    return -1;
}

int32 arad_pp_dbal_vt_cam_test2_set(int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int line, uint32 unused)
{
    prog_selection->packet_format_qualifier_1 = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_NEXT_PROTOCOL_MAC_IN_MAC);
    prog_selection->packet_format_qualifier_1_mask = (0x7F);
    prog_selection->packet_format_code              = ARAD_PARSER_PFC_ETH;
    prog_selection->packet_format_code_mask         = ARAD_PARSER_PFC_MATCH_TYPE;
   
    return -1;
}

int32 arad_pp_dbal_vt_cam_extender_pe_set(int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 vid_type)
{
    prog_selection->packet_format_code              = ARAD_PARSER_PFC_ETH;
    prog_selection->packet_format_code_mask         = ARAD_PARSER_PFC_MATCH_TYPE;
    
    prog_selection->in_pp_port_vt_profile          = SOC_PPC_PORT_DEFINED_VT_PROFILE_EXTENDER_PE;
    prog_selection->in_pp_port_vt_profile_mask     = (0x0);

    if(vid_type == INITIAL_VID) {
        
        prog_selection->packet_format_qualifier_1      = ARAD_PP_ISEM_ACCESS_QLFR_ETH_NULL;
        prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID |ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_INNER_TPID);
    }else{
        if(next_line==0)
        {
            prog_selection->packet_format_qualifier_1      = ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_TPID2;
            prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP |  ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_PCP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_INNER_TPID);
            return 1;
        }
        if(next_line==1)
        {
            prog_selection->packet_format_qualifier_1      = ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_TPID1;
            prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP |  ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_PCP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_INNER_TPID);
            return -1;
        }
    }

    return -1;
}

int32 arad_pp_dbal_vt_cam_custom_pp_port_tunnel_set(int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int line, uint32 unused)
{
    
    prog_selection->ptc_vt_profile = (ARAD_PORTS_VT_PROFILE_CUSTOM_PP);
    prog_selection->ptc_vt_profile_mask = 0;
   
    return -1;
}

int32 arad_pp_dbal_vt_cam_vlan_domain_mpls_l1_set(int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused)
{

    prog_selection->in_pp_port_vt_profile = SOC_PPC_PORT_DEFINED_VT_PROFILE_VLAN_DOMAIN_MPLS;
    prog_selection->in_pp_port_vt_profile_mask = 0;

    prog_selection->packet_format_code      = ARAD_PARSER_PFC_MPLS4P_ETH;
    prog_selection->packet_format_code_mask = ARAD_PARSER_PFC_MATCH_ANY_MPLS;
   
    return -1;
}

int32 arad_pp_dbal_vt_cam_pon_all_progs_set(int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 prog_sel_pon_type)
{
    int ret = (-1);

    prog_selection->packet_format_code              = ARAD_PARSER_PFC_ETH;
    prog_selection->packet_format_code_mask         = ARAD_PARSER_PFC_MATCH_TYPE;

    switch(prog_sel_pon_type)
    {
        case PROG_SEL_PON_UNTAGGED:
            prog_selection->packet_format_qualifier_1      = ARAD_PP_ISEM_ACCESS_QLFR_ETH_NULL; 
            prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP);
            prog_selection->in_pp_port_vt_profile          = (SOC_PPC_PORT_DEFINED_VT_PROFILE_PON_DEFAULT); 
            prog_selection->in_pp_port_vt_profile_mask     = 0x3;
            break;
        case PROG_SEL_PON_PRIORITY_TAG:
            prog_selection->packet_format_qualifier_1      = ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_PCP; 
            prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_INNER_TPID);
            prog_selection->in_pp_port_vt_profile          = (SOC_PPC_PORT_DEFINED_VT_PROFILE_PON_DEFAULT); 
            prog_selection->in_pp_port_vt_profile_mask     = 0x3;
            break;
        case PROG_SEL_PON_ONE_TAG_1:
            prog_selection->packet_format_qualifier_1      = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_NULL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_TPID1); 
            prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_PCP);
            if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "gpon_application_enable", 0)) {
	         prog_selection->packet_format_qualifier_1_mask |= ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_INNER_TPID;
	    }
            prog_selection->in_pp_port_vt_profile          = (SOC_PPC_PORT_DEFINED_VT_PROFILE_PON_DEFAULT); 
            prog_selection->in_pp_port_vt_profile_mask     = 0x3;
            break;
        case PROG_SEL_PON_ONE_TAG_2:
            prog_selection->packet_format_qualifier_1      = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_NULL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_TPID2); 
            prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_PCP);
            if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "gpon_application_enable", 0)) {
	         prog_selection->packet_format_qualifier_1_mask |= ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_INNER_TPID;
	    }
            prog_selection->in_pp_port_vt_profile          = (SOC_PPC_PORT_DEFINED_VT_PROFILE_PON_DEFAULT); 
            prog_selection->in_pp_port_vt_profile_mask     = 0x3;
            break;
        case PROG_SEL_PON_TWO_TAGS:
            prog_selection->packet_format_qualifier_1      = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_NULL); 
            prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | 
                                                              ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_PCP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID | 
                                                              ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_INNER_TPID ); 
            prog_selection->in_pp_port_vt_profile          = (SOC_PPC_PORT_DEFINED_VT_PROFILE_PON_DEFAULT); 
            prog_selection->in_pp_port_vt_profile_mask     = (0x0);
            break;
        case PROG_SEL_PON_TWO_TAGS_VS_TUNNEL_ID:
            prog_selection->packet_format_qualifier_1      = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_NULL); 
            prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | 
                                                              ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_PCP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID | 
                                                              ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_INNER_TPID ); 
            prog_selection->in_pp_port_vt_profile          = (SOC_PPC_PORT_DEFINED_VT_PROFILE_PON_TUNNEL_ID); 
            prog_selection->in_pp_port_vt_profile_mask     = (0x0);
            break;
        case PROG_SEL_PON_IGNORE_2ND_TAG:
            prog_selection->packet_format_qualifier_1      = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_NULL); 
            prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | 
                                                              ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_PCP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID | 
                                                              ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_INNER_TPID ); 
            prog_selection->in_pp_port_vt_profile          = (SOC_PPC_PORT_DEFINED_VT_PROFILE_IGNORE_2ND_TAG); 
            prog_selection->in_pp_port_vt_profile_mask     = (0x0);

            break;
        default:
            break;
    }

    return ret;
}


int32 arad_pp_dbal_vt_cam_section_oam_all_progs_set(int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 prog_sel_pon_type)
{
    int ret = (-1);

    prog_selection->packet_format_code             = ARAD_PARSER_PFC_ETH;  
    prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_TYPE;

    switch(prog_sel_pon_type)
    {
        case PROG_SEL_SECTION_OAM_ONE_TAG_1:
            prog_selection->packet_format_qualifier_1      = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_TPID1); 
            prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP);
            break;
        case PROG_SEL_SECTION_OAM_ONE_TAG_2:
            prog_selection->packet_format_qualifier_1      = ( ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_TPID2); 
            prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP);
            break;
        case PROG_SEL_SECTION_OAM_TWO_TAGS:
            prog_selection->packet_format_qualifier_1      = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_NULL); 
            prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | 
                                                              ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_PCP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID | 
                                                              ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_INNER_TPID ); 
            break;
        default:
            break;
    }

    return ret;
}


int32 arad_pp_dbal_vt_cam_3mtse_plus_frr_vdxinitialvid_set(int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 prog_sel_3mtse_plus_frr_type)
{

   switch(prog_sel_3mtse_plus_frr_type)
    {
        case PROG_SEL_3MTSE_PLUS_FRR_UNTAGGED_M3_PLUS_ELI:
            prog_selection->packet_format_qualifier_2      = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_L3ELI;
            prog_selection->packet_format_qualifier_2_mask = (ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L1 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L2 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_NEXT_PROT);
            prog_selection->in_pp_port_vt_profile           = (0x0);
            prog_selection->in_pp_port_vt_profile_mask      = (ARAD_PP_ISEM_ACCESS_VT_PROFILE_MASK);
            prog_selection->packet_format_code              = ARAD_PARSER_PFC_MPLS3_ETH;
            prog_selection->packet_format_code_mask         = ARAD_PARSER_PFC_MATCH_ANY_ENCAP;
            break;

        case PROG_SEL_3MTSE_PLUS_FRR_UNTAGGED_M2_PLUS:
            prog_selection->in_pp_port_vt_profile           = (0x0);
            prog_selection->in_pp_port_vt_profile_mask      = (ARAD_PP_ISEM_ACCESS_VT_PROFILE_MASK);
            prog_selection->packet_format_code              = ARAD_PARSER_PFC_MPLS2_ETH;
            prog_selection->packet_format_code_mask         = ARAD_PARSER_PFC_MATCH_AT_LEAST_MPLS2;
            break;
        case PROG_SEL_3MTSE_PLUS_FRR_UNTAGGED:
            prog_selection->in_pp_port_vt_profile           = (0x0);
            prog_selection->in_pp_port_vt_profile_mask      = (ARAD_PP_ISEM_ACCESS_VT_PROFILE_MASK);
           break;

        default:
            break;
    }

   if(next_line == 0)
   {
       
       prog_selection->packet_format_qualifier_1      = ARAD_PP_ISEM_ACCESS_QLFR_ETH_NULL; 
       prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID);
       return 1;
   }

   if(next_line == 1)
   {
       
       prog_selection->packet_format_qualifier_1     = ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_PCP; 
       prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_INNER_TPID);
       return -1;
   }

    return -1;
}

int32 arad_pp_dbal_vt_cam_3mtse_plus_frr_vdxoutervid_set(int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 prog_sel_3mtse_plus_frr_type)
{

    switch(prog_sel_3mtse_plus_frr_type)
    {

        case PROG_SEL_3MTSE_PLUS_FRR_TAG_1_M3_PLUS_ELI:
            prog_selection->packet_format_qualifier_2      = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_L3ELI;
            prog_selection->packet_format_qualifier_2_mask = (ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L1 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L2 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_NEXT_PROT);
            prog_selection->in_pp_port_vt_profile           = (0x0);
            prog_selection->in_pp_port_vt_profile_mask      = (ARAD_PP_ISEM_ACCESS_VT_PROFILE_MASK);
            prog_selection->packet_format_code              = ARAD_PARSER_PFC_MPLS3_ETH;
            prog_selection->packet_format_code_mask         = ARAD_PARSER_PFC_MATCH_ANY_ENCAP;
            break;

        case PROG_SEL_3MTSE_PLUS_FRR_TAG_1_M2_PLUS:
            prog_selection->in_pp_port_vt_profile           = (0x0);
            prog_selection->in_pp_port_vt_profile_mask      = (ARAD_PP_ISEM_ACCESS_VT_PROFILE_MASK);
            prog_selection->packet_format_code              = ARAD_PARSER_PFC_MPLS2_ETH;
            prog_selection->packet_format_code_mask         = ARAD_PARSER_PFC_MATCH_AT_LEAST_MPLS2;
            break;

        case PROG_SEL_3MTSE_PLUS_FRR_TAG_1:
            prog_selection->in_pp_port_vt_profile           = (0x0);
            prog_selection->in_pp_port_vt_profile_mask      = (ARAD_PP_ISEM_ACCESS_VT_PROFILE_MASK);
            break;

        default:
            break;
    }

    if(next_line == 0)
    {
        prog_selection->packet_format_qualifier_1      = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_TPID1); 
        prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP);
        return 1;
    }

    if(next_line == 1)
    {
        prog_selection->packet_format_qualifier_1      = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_TPID2); 
        prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP);
        return -1;
    }

    return -1;
}

int32 arad_pp_dbal_vt_cam_2mtse_plus_frr_vdxouter_inner_vid_l1_set(int unit, ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int line, uint32 unused)
{
    prog_selection->packet_format_code              = ARAD_PARSER_PFC_ETH;
    prog_selection->packet_format_code_mask         = ARAD_PARSER_PFC_MATCH_TYPE;
    prog_selection->in_pp_port_vt_profile          = SOC_PPC_PORT_DEFINED_VT_PROFILE_DOUBLE_TAG_PRIORITY;
    prog_selection->in_pp_port_vt_profile_mask     = (0x0);

    if(line==0)
    {
        prog_selection->packet_format_qualifier_1      =  (ARAD_PP_ISEM_ACCESS_QLFR_ETH_NEXT_PROTOCOL_MPLS | ARAD_PP_ISEM_ACCESS_QLFR_ETH_INNER_TPID1);
        
        prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_PCP);
        return 1;
    }


    if(line==1)
    {
        
        prog_selection->packet_format_qualifier_1      =  (ARAD_PP_ISEM_ACCESS_QLFR_ETH_NEXT_PROTOCOL_MPLS | ARAD_PP_ISEM_ACCESS_QLFR_ETH_INNER_TPID2);
        
        prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_PCP);
        return -1;
    }
    return -1;
}

int32 arad_pp_dbal_vt_cam_2mtse_plus_frr_vdxouter_inner_vid_pcp_l1_set(int unit, ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int line, uint32 unused)
{
    prog_selection->packet_format_code              = ARAD_PARSER_PFC_ETH;
    prog_selection->packet_format_code_mask         = ARAD_PARSER_PFC_MATCH_TYPE;
    prog_selection->in_pp_port_vt_profile          = SOC_PPC_PORT_DEFINED_VT_PROFILE_DOUBLE_TAG_PRIORITY;
    prog_selection->in_pp_port_vt_profile_mask     = (0x0);

    if(line==0)
    {
        prog_selection->packet_format_qualifier_1      =  (ARAD_PP_ISEM_ACCESS_QLFR_ETH_NEXT_PROTOCOL_MPLS | ARAD_PP_ISEM_ACCESS_QLFR_ETH_INNER_TPID1 | ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_PCP);
        
        prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID);
        return 1;
    }


    if(line==1)
    {
        
        prog_selection->packet_format_qualifier_1      =  (ARAD_PP_ISEM_ACCESS_QLFR_ETH_NEXT_PROTOCOL_MPLS | ARAD_PP_ISEM_ACCESS_QLFR_ETH_INNER_TPID2 | ARAD_PP_ISEM_ACCESS_QLFR_ETH_OUTER_PCP);
        
        prog_selection->packet_format_qualifier_1_mask = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID);
        return -1;
    }
    return -1;
}
int32 arad_pp_dbal_vt_cam_generalized_rch_set(int unit,  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 is_rch0)
{
    prog_selection->in_pp_port_vt_profile      = is_rch0? SOC_PPC_PORT_DEFINED_VT_PROFILE_GENERALIZED_RCH0:SOC_PPC_PORT_DEFINED_VT_PROFILE_GENERALIZED_RCH1;
    prog_selection->in_pp_port_vt_profile_mask =  0;

    return -1;
}


int32 arad_pp_dbal_tt_cam_tm_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused)
{
    prog_selection->packet_format_code      = ARAD_PARSER_PFC_RAW_AND_FTMH;
    prog_selection->packet_format_code_mask = ARAD_PARSER_PFC_MATCH_TYPE;
    prog_selection->enable_routing_mask     = 1;
    return -1;

}

int32 arad_pp_dbal_tt_cam_oam_bfd_stat_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA *prog_selection, int next_line, uint32 stat_type)
{
    prog_selection->packet_format_code      = ARAD_PARSER_PFC_RAW_AND_FTMH;
    prog_selection->packet_format_code_mask = ARAD_PARSER_PFC_MATCH_TYPE;
    prog_selection->enable_routing_mask     = 1;

    if(stat_type == IS_OAM_STAT)
    {
        prog_selection->isa_lookup_found = (0x1);
        prog_selection->isa_lookup_found_mask = (0);
    }
    else if (stat_type == IS_BFD_STAT)
    {
        prog_selection->isb_lookup_found = (0x1);
        prog_selection->isb_lookup_found_mask = (0);
    }

    return -1;
}



int32 arad_pp_dbal_tt_cam_mpls_port_l3_l2_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int line, uint32 unused) {
    
    prog_selection->in_pp_port_tt_profile =  (SOC_PPC_PORT_DEFINED_TT_PROFILE_PORT_TERMINATION);
    prog_selection->in_pp_port_tt_profile_mask = (0x0);
    prog_selection->packet_format_qualifier_1       = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_NEXT_PROTOCOL_MPLS);
    prog_selection->packet_format_qualifier_1_mask  = (0x7F);
    return -1;
}




int32 arad_pp_dbal_tt_cam_mpls2_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int line, uint32 unused)
{
    int delta = g_prog_soc_prop->ingress_full_mymac_1 ? 1 : 0 ;

    prog_selection->packet_format_qualifier_1       = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_NEXT_PROTOCOL_MPLS);
    prog_selection->packet_format_qualifier_1_mask  = (0x7F);
    prog_selection->enable_routing_mask             = (0x1);

    if(line == 0)
    {
        prog_selection->packet_format_code              = ARAD_PARSER_PFC_ETH;
        prog_selection->packet_format_code_mask         = ARAD_PARSER_PFC_MATCH_TYPE;
        prog_selection->my_mac                          = (0x1);
        prog_selection->my_mac_mask                     = (0x0);
        prog_selection->enable_routing                  = (0x1);
        prog_selection->enable_routing_mask             = (0x0);
        if( g_prog_soc_prop->ingress_full_mymac_1 ||  g_prog_soc_prop[unit].mpls_tp_mac_address || g_prog_soc_prop[unit].port_raw_mpls)
            return 1;
        return -1;
    }

    if (g_prog_soc_prop->ingress_full_mymac_1 && line == 1) {
        prog_selection->packet_format_code              = ARAD_PARSER_PFC_ETH;
        prog_selection->packet_format_code_mask         = ARAD_PARSER_PFC_MATCH_TYPE;
        prog_selection->my_mac_mask                     = (0x1); 
        prog_selection->enable_routing_mask             = (0x1); 
        prog_selection->tcam_lookup_match               = (0x1);
        prog_selection->tcam_lookup_match_mask          = (0x0);
        if (g_prog_soc_prop[unit].mpls_tp_mac_address || g_prog_soc_prop[unit].port_raw_mpls) 
            return 2;
        return -1;
    }

    if(line == (1+delta))
    {
        if(g_prog_soc_prop[unit].mpls_tp_mac_address)
        {
            prog_selection->packet_format_code       = ARAD_PARSER_PFC_ETH;
            prog_selection->packet_format_code_mask  = ARAD_PARSER_PFC_MATCH_TYPE;
            prog_selection->da_is_all_r_bridges      = (0x1);
            prog_selection->da_is_all_r_bridges_mask = (0x0);

            if( g_prog_soc_prop[unit].port_raw_mpls)
                return (2+delta);
            return -1;
        }
        else
        {
            prog_selection->in_pp_port_tt_profile      =  SOC_PPC_PORT_DEFINED_TT_PROFILE_FORCE_MY_MAC;
            prog_selection->in_pp_port_tt_profile_mask =  (0x0);
            return -1;
        }
    }

    if(line == (2+delta))
    {
        prog_selection->in_pp_port_tt_profile      =  SOC_PPC_PORT_DEFINED_TT_PROFILE_FORCE_MY_MAC;
        prog_selection->in_pp_port_tt_profile_mask =  (0x0);
        return -1;
    }
    return -1;
}

int32 arad_pp_dbal_tt_cam_mpls_l3_l2_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int line, uint32 unused)
{

    if(!g_prog_soc_prop[unit].tunnel_termination_in_tt_only)
    {
        return arad_pp_dbal_tt_cam_mpls2_set(unit, prog_selection, line, unused);
    }
    else
    {
        prog_selection->packet_format_qualifier_1      = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_NEXT_PROTOCOL_MPLS);
        prog_selection->packet_format_qualifier_1_mask = (0x7F);
        prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS3_ETH; 
        prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP; 
        prog_selection->in_pp_port_tt_profile          = (SOC_PPC_PORT_DEFINED_TT_PROFILE_EXPLICIT_NULL);
        prog_selection->in_pp_port_tt_profile_mask     = (0x0);
        prog_selection->tcam_lookup_match              = (0x1);
        prog_selection->tcam_lookup_match_mask         = (0x0);
        prog_selection->enable_routing_mask            = (0x1);

        if(line == 0)
        {
            prog_selection->my_mac                          = (0x1);
            prog_selection->my_mac_mask                     = (0x0);
            prog_selection->enable_routing                  = (0x1);
            prog_selection->enable_routing_mask             = (0x0);
            if( g_prog_soc_prop[unit].mpls_tp_mac_address || g_prog_soc_prop[unit].port_raw_mpls)
                return 1;
            return -1;
        }

        if(line == 1)
        {
            if(g_prog_soc_prop[unit].mpls_tp_mac_address)
            {
                prog_selection->da_is_all_r_bridges      = (0x1);
                prog_selection->da_is_all_r_bridges_mask = (0x0);

                if( g_prog_soc_prop[unit].port_raw_mpls)
                    return 2;
                return -1;
            }
            else
                return -1;
        }
        return -1;
    }
}

int32 arad_pp_dbal_tt_cam_mpls4_l3_l4_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int line, uint32 sem_miss)
{
    uint32 database, namespace;

    prog_selection->packet_format_code              = ARAD_PARSER_PFC_MPLS3_ETH;
    prog_selection->packet_format_code_mask         = 0;
    prog_selection->packet_format_qualifier_2 = 0;
    prog_selection->packet_format_qualifier_2_mask = 0x7fe;

    prog_selection->my_mac                          = (0x1);
    prog_selection->my_mac_mask                     = (0x0);
    prog_selection->enable_routing                  = (0x1);
    prog_selection->enable_routing_mask             = (0x0);

    prog_selection->tcam_lookup_match          = sem_miss?(0x0):(0x1);
    prog_selection->tcam_lookup_match_mask = sem_miss?(0x1):(0x0);

    arad_pp_dbal_vtt_find_namespace_database(unit, SOC_PPC_MPLS_TERM_NAMESPACE_L1, &namespace, &database);
    if(database == ARAD_PP_ISEM_ACCESS_ID_ISEM_A) {
        prog_selection->isa_lookup_found=                 sem_miss? (0x0):(0x1);        
        prog_selection->isa_lookup_found_mask       = (0x0);        
    } else {
        prog_selection->isb_lookup_found=                  sem_miss?(0x0):(0x1);        
        prog_selection->isb_lookup_found_mask       = (0x0);        

    }


    return -1;

}

int32 arad_pp_dbal_tt_cam_mpls3_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int line, uint32 unused)
{

    prog_selection->packet_format_qualifier_1      = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_NEXT_PROTOCOL_MPLS);
    prog_selection->packet_format_qualifier_1_mask = (0x7F);
    prog_selection->packet_format_code             = ARAD_PARSER_PFC_ETH;
    prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_TYPE;
    prog_selection->in_pp_port_tt_profile          = SOC_PPC_PORT_DEFINED_TT_PROFILE_FRR_COUPLING;
    prog_selection->in_pp_port_tt_profile_mask     = (0x0);
    prog_selection->tcam_lookup_match              = (0x1); 
    prog_selection->tcam_lookup_match_mask         = (0x0);
    prog_selection->enable_routing_mask            = (0x1);

    if(line == 0)
    {
        prog_selection->my_mac                     = (0x1);
        prog_selection->my_mac_mask                = (0x0);
        prog_selection->enable_routing             = (0x1);
        prog_selection->enable_routing_mask        = (0x0);
        if (g_prog_soc_prop[unit].mpls_tp_mac_address)
            return 1;
        return -1;
    }

    if(line == 1)
    {
        prog_selection->da_is_all_r_bridges        = (0x1);
        prog_selection->da_is_all_r_bridges_mask   = (0x0);
        return -1;
    }
    return -1;
}

int32 arad_pp_dbal_tt_cam_mpls_l1_l1_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 is_gal)
{
    prog_selection->packet_format_qualifier_1      = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_NEXT_PROTOCOL_MPLS);
    prog_selection->packet_format_qualifier_1_mask = (0x7F);
    prog_selection->enable_routing_mask            = (0x1);

    if(is_gal)
    {
        prog_selection->packet_format_qualifier_2      = (ARAD_PP_ISEM_ACCESS_QLFR_MPLS_L1GAL);
        prog_selection->packet_format_qualifier_2_mask = (ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L1 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L3 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_BOS_NEXT_PROT);
        prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS2_ETH; 
        prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP; 
    }
    else
    {
        prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS1_ETH; 
        prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP; 
    }

    return arad_pp_dbal_tt_cam_mpls_mac_and_raw_mpls_set(unit, prog_selection, next_line);
}

int32 arad_pp_dbal_tt_cam_extender_untag_check_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 extender_type)
{
    prog_selection->enable_routing_mask         = 0x1;

    if(extender_type > EXTENDER_ETH)  
    {
        prog_selection->packet_format_code          = ARAD_PARSER_PFC_IPV4_ETH;  
        prog_selection->packet_format_code_mask     = ARAD_PARSER_PFC_MATCH_ANY_ENCAP_ANY_TYPE; 
        if (extender_type == EXTENDER_IP_UC) 
        {
            prog_selection->my_mac                  = 0x1;  
            prog_selection->my_mac_mask             = 0x0;
            prog_selection->enable_routing          = 0x1;
            prog_selection->enable_routing_mask     = 0x0;
        } 
        else 
        {
            prog_selection->packet_is_compatible_mc      = 0x1;
            prog_selection->packet_is_compatible_mc_mask = 0x0;
        }
    }
    prog_selection->in_pp_port_tt_profile                   = SOC_PPC_PORT_DEFINED_TT_PROFILE_PORT_EXTENDER_UNTAG_CB;
    prog_selection->in_pp_port_tt_profile_mask              = 0x0;
    return -1;
}


int32 arad_pp_dbal_tt_cam_extender_cb_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 extender_type)
{
    prog_selection->enable_routing_mask         = 0x1;

    if(extender_type > EXTENDER_ETH)  
    {
        prog_selection->packet_format_code          = ARAD_PARSER_PFC_IPV4_ETH;  
        prog_selection->packet_format_code_mask     = ARAD_PARSER_PFC_MATCH_ANY_ENCAP_ANY_TYPE; 
        if (extender_type == EXTENDER_IP_UC) 
        {
            prog_selection->my_mac                  = 0x1;  
            prog_selection->my_mac_mask             = 0x0;
            prog_selection->enable_routing          = 0x1;
            prog_selection->enable_routing_mask     = 0x0;
        } 
        else 
        {
            prog_selection->packet_is_compatible_mc      = 0x1;
            prog_selection->packet_is_compatible_mc_mask = 0x0;
        }
    }
    prog_selection->in_pp_port_tt_profile                   = SOC_PPC_PORT_DEFINED_TT_PROFILE_PORT_EXTENDER_CB;
    prog_selection->in_pp_port_tt_profile_mask              = 0x0;
    return -1;
}



int32 arad_pp_dbal_tt_cam_mpls_l1_l2_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 is_gal)
{
    prog_selection->packet_format_qualifier_1      = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_NEXT_PROTOCOL_MPLS);
    prog_selection->packet_format_qualifier_1_mask = (0x7F);
    prog_selection->enable_routing_mask            = (0x1);

    if(is_gal)
    {
        prog_selection->packet_format_qualifier_2      = (ARAD_PP_ISEM_ACCESS_QLFR_MPLS_L2GAL);
        prog_selection->packet_format_qualifier_2_mask = (ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L1 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L2 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_BOS_NEXT_PROT);
        prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS3_ETH;
        prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP;
    }
    else
    {
        prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS2_ETH; 
        prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_AT_LEAST_MPLS2;
    }

    return arad_pp_dbal_tt_cam_mpls_mac_and_raw_mpls_set(unit, prog_selection, next_line);
}
int32 arad_pp_dbal_tt_cam_mpls_l1_l3_eli_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 is_gal)
{
    prog_selection->packet_format_qualifier_1      = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_NEXT_PROTOCOL_MPLS);
    prog_selection->packet_format_qualifier_1_mask = (0x7F);
    prog_selection->packet_format_qualifier_2      = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_L3ELI;
    prog_selection->packet_format_qualifier_2_mask = (ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L1 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L2 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_BOS_NEXT_PROT);
    prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS3_ETH;
    prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP;
    prog_selection->enable_routing_mask            = (0x1);

    return arad_pp_dbal_tt_cam_mpls_mac_and_raw_mpls_set(unit, prog_selection, next_line);
}


int32 arad_pp_dbal_tt_cam_mpls_l1_l2__eli_inrif_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused)
{
    prog_selection->packet_format_qualifier_1      = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_NEXT_PROTOCOL_MPLS);
    prog_selection->packet_format_qualifier_1_mask = (0x7F);
    prog_selection->packet_format_qualifier_2      = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_L2ELI;
    prog_selection->packet_format_qualifier_2_mask = (ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L1 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L3 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_BOS_NEXT_PROT);

    prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS2_ETH; 
    prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_AT_LEAST_MPLS2;
    prog_selection->in_pp_port_tt_profile          = (0x0);
    prog_selection->in_pp_port_tt_profile_mask     = (0x0);
    prog_selection->enable_routing_mask            = (0x1);

    return arad_pp_dbal_tt_cam_mpls_mac_and_raw_mpls_set(unit, prog_selection, next_line);
}
int32 arad_pp_dbal_tt_cam_mpls_l2_l2_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused)
{
    prog_selection->packet_format_qualifier_1      = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_NEXT_PROTOCOL_MPLS);
    prog_selection->packet_format_qualifier_1_mask = (0x7F);
    prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS2_ETH; 
    prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP;
    if (SOC_IS_ARADPLUS_AND_BELOW(unit)){
        prog_selection->tcam_lookup_match              = (0x1); 
        prog_selection->tcam_lookup_match_mask         = (0x0);
        prog_selection->in_pp_port_tt_profile          = (SOC_PPC_PORT_DEFINED_TT_PROFILE_EXPLICIT_NULL);
        prog_selection->in_pp_port_tt_profile_mask     = (0x0);
        prog_selection->enable_routing_mask            = (0x1);
    } else if(SOC_IS_JERICHO(unit)){
        prog_selection->packet_format_qualifier_2      = 0x0;
        prog_selection->packet_format_qualifier_2_mask = 0x7FB;
    }
    return arad_pp_dbal_tt_cam_mpls_mac_set(unit, prog_selection, next_line);

}
int arad_pp_dbal_tt_cam_mldp_overlay_after_recycle_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused)
{
     prog_selection->enable_routing_mask              = 1;
     prog_selection->ptc_tt_profile                   = ARAD_PORTS_TT_PROFILE_OVERLAY_RCY;
     prog_selection->ptc_tt_profile_mask              = 0x0;
     prog_selection->my_mac                           = 0x1;
     prog_selection->my_mac_mask                      = 0x0;
     prog_selection->packet_format_code_mask          = ARAD_PARSER_PFC_MATCH_ANY_ENCAP_ANY_TYPE;

     if(next_line == 0) {
         prog_selection->packet_format_code = ARAD_PARSER_PFC_MPLS1_ETH;
         return 1;
     } else
         prog_selection->packet_format_code = ARAD_PARSER_PFC_MPLS2_ETH;
     return -1;
}

int32 arad_pp_dbal_tt_cam_mpls_l2_l3_gal_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused)
{
    prog_selection->packet_format_qualifier_1                     = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_NEXT_PROTOCOL_MPLS);
    prog_selection->packet_format_qualifier_1_mask                = (0x7F);
    prog_selection->packet_format_qualifier_2                     = (0x0);
    prog_selection->packet_format_qualifier_2_mask                = (ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L1 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L2 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L3 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_NEXT_PROT);
    prog_selection->packet_format_qualifier_3_outer_vid_bits      = (0x1); 
    prog_selection->packet_format_qualifier_3_outer_vid_bits_mask = (0x6);
    prog_selection->packet_format_code                            = ARAD_PARSER_PFC_MPLS3_ETH; 
    prog_selection->packet_format_code_mask                       = ARAD_PARSER_PFC_MATCH_ANY_ENCAP; 
    prog_selection->in_pp_port_tt_profile                         = (SOC_PPC_PORT_DEFINED_TT_PROFILE_EXPLICIT_NULL);
    prog_selection->in_pp_port_tt_profile_mask                    = (0x0);
    prog_selection->tcam_lookup_match                             = (0x1);
    prog_selection->tcam_lookup_match_mask                        = (0x0);
    prog_selection->enable_routing_mask                           = (0x1);

    return arad_pp_dbal_tt_cam_mpls_mac_set(unit, prog_selection, next_line);
}



int32 arad_pp_dbal_tt_cam_mpls_l1_gal_exactly_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 is_gal)
{
    prog_selection->packet_format_qualifier_1      = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_NEXT_PROTOCOL_MPLS);
    prog_selection->packet_format_qualifier_1_mask = (0x7F);
    prog_selection->my_mac                          = (0x1);
    prog_selection->my_mac_mask                     = (0x0);

    
    prog_selection->packet_format_qualifier_2      = (ARAD_PP_ISEM_ACCESS_QLFR_MPLS_L1GAL);
    prog_selection->packet_format_qualifier_2_mask = (ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L1 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L3 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_BOS_NEXT_PROT);
    prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS2_ETH;
    prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP;

    return -1;
}


int32 arad_pp_dbal_tt_cam_mpls_l2_gal_exactly_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused)
{

     prog_selection->packet_format_qualifier_1      = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_NEXT_PROTOCOL_MPLS);
     prog_selection->packet_format_qualifier_1_mask = (0x7F);
    prog_selection->my_mac                          = (0x1);
    prog_selection->my_mac_mask                     = (0x0);

   
    prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS3_ETH;
    prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP;
    prog_selection->packet_format_qualifier_2      = (ARAD_PP_ISEM_ACCESS_QLFR_MPLS_L2GAL);
    prog_selection->packet_format_qualifier_2_mask = (ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L1 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L2 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_BOS_NEXT_PROT);
     return -1;

}

int32 arad_pp_dbal_tt_cam_mpls_index_as_order_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 function)
{
    prog_selection->packet_format_qualifier_1      = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_NEXT_PROTOCOL_MPLS);
    prog_selection->packet_format_qualifier_1_mask = (0x7F);

    switch(function)
    {
        case 1:
            prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS3_ETH; 
            prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP; 
            prog_selection->isa_lookup_found              = (0x1); 
            prog_selection->isa_lookup_found_mask    = (0x0);
            break;
        case 2:
            prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS2_ETH; 
            prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP; 
            prog_selection->isa_lookup_found              = (0x1);
            prog_selection->isa_lookup_found_mask    = (0x0);
            break;
        case 3:
            prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS2_ETH; 
            prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_AT_LEAST_MPLS2; 
            prog_selection->isa_lookup_found              = (0x0);
            prog_selection->isa_lookup_found_mask    = (0x0);
            break;
        case 4:
            prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS1_ETH; 
            prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP; 
            prog_selection->isa_lookup_found              = (0x0);
            prog_selection->isa_lookup_found_mask    = (0x0);
            break;
        case 5:
            prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS1_ETH; 
            prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP; 
            prog_selection->isa_lookup_found              = (0x1);
            prog_selection->isa_lookup_found_mask    = (0x0);
            break;

        default:
        break;
    }

    if (!g_prog_soc_prop->ingress_full_mymac_1) {
        return arad_pp_dbal_tt_cam_mpls_mac_and_raw_mpls_set(unit, prog_selection, next_line);
    }

    if (next_line == 0) {
        prog_selection->my_mac                          = (0x1);
        prog_selection->my_mac_mask                     = (0x0);
        prog_selection->enable_routing                          = (0x1);
        prog_selection->enable_routing_mask        = (0x0);
        return 1;
    }
    if (next_line == 1) {
        prog_selection->my_mac_mask                     = (0x1); 
        prog_selection->enable_routing_mask             = (0x1); 
        prog_selection->tcam_lookup_match               = (0x1);
        prog_selection->tcam_lookup_match_mask          = (0x0);
        if( g_prog_soc_prop[unit].mpls_tp_mac_address || g_prog_soc_prop[unit].port_raw_mpls)
            return 2;
        return -1;
   }

    if(next_line == 2)
    {
        if(g_prog_soc_prop[unit].mpls_tp_mac_address)
        {
            prog_selection->da_is_all_r_bridges      = (0x1);
            prog_selection->da_is_all_r_bridges_mask = (0x0);

            if( g_prog_soc_prop[unit].port_raw_mpls)
                return 3;
            return -1;
        }
        else
        {
            prog_selection->in_pp_port_tt_profile      = SOC_PPC_PORT_DEFINED_TT_PROFILE_FORCE_MY_MAC;
            prog_selection->in_pp_port_tt_profile_mask = (0x0);
            return -1;
        }
    }

    if(next_line == 3)
    {
        prog_selection->in_pp_port_tt_profile      = SOC_PPC_PORT_DEFINED_TT_PROFILE_FORCE_MY_MAC;
        prog_selection->in_pp_port_tt_profile_mask = (0x0);
        return -1;
    }

    return -1;
}

int32 arad_pp_dbal_tt_cam_mpls_index_as_order_l1_l2_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 label_num)
{
    prog_selection->packet_format_qualifier_1      = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_NEXT_PROTOCOL_MPLS);
    prog_selection->packet_format_qualifier_1_mask = (0x7F);
    if (label_num == 2) {
        prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS2_ETH; 
        prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP; 

    } else  if(label_num == 1) {
        prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS2_ETH; 
        prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP; 
    }

    prog_selection->my_mac                          = (0x0);
    prog_selection->my_mac_mask                     = (0x0);
    prog_selection->enable_routing                          = (0x1);
    prog_selection->enable_routing_mask        = (0x0);
    prog_selection->isb_lookup_found              = (0x1);
    prog_selection->isb_lookup_found_mask    = (0x0);

    return -1;
}


int32 arad_pp_dbal_tt_cam_arp_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused)
{
    prog_selection->packet_format_code              = ARAD_PARSER_PFC_ETH;
    prog_selection->packet_format_code_mask         = ARAD_PARSER_PFC_MATCH_ONE;
    prog_selection->packet_format_qualifier_2       = (0x0);
    prog_selection->packet_format_qualifier_2_mask  = (0x7FF);
    prog_selection->my_mac                          = (0x1);
    prog_selection->my_mac_mask                     = (0x0);
    prog_selection->enable_routing                  = (0x0);
    prog_selection->enable_routing_mask             = (0x0);
    prog_selection->packet_format_qualifier_1       = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_NEXT_PROTOCOL_ARP);
    prog_selection->packet_format_qualifier_1_mask  = (0x7F);

    return -1;
}

int32 arad_pp_dbal_tt_cam_pppoe_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused)
{
    prog_selection->my_mac                          = (0x1);
    prog_selection->my_mac_mask                     = (0x0);
    prog_selection->packet_is_compatible_mc         = 0x0;
    prog_selection->packet_is_compatible_mc_mask    = 0x0;
    
    prog_selection->enable_routing_mask             = 0x1;
    prog_selection->packet_format_qualifier_1       = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_NEXT_PPPOE_ENCAP);
    prog_selection->packet_format_qualifier_1_mask  = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_NXT_PROTOCOL | 
                                                       ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP_PPPOE|
                                                       ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID|
                                                       ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_PCP|
                                                       ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_INNER_TPID);

    return -1;
}

int32 arad_pp_dbal_tt_cam_l2tp_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused)
{
	
	prog_selection->packet_format_code		= ARAD_PARSER_PFC_IPV4_ETH;
	prog_selection->packet_format_code_mask = ARAD_PARSER_PFC_MATCH_ONE;
	prog_selection->parser_leaf_context = ARAD_PARSER_PLC_PP_L4;
	prog_selection->parser_leaf_context_mask = ARAD_PARSER_PLC_MATCH_ONE;
	prog_selection->packet_format_qualifier_2 =  0x0080;
	prog_selection->packet_format_qualifier_2_mask = 0;

	
	prog_selection->my_mac = 1;
	prog_selection->my_mac_mask =  0;

	
    if(soc_property_get(unit, spn_BCM886XX_IP4_TUNNEL_TERMINATION_MODE, 3) == 2) {
		prog_selection->isb_lookup_found = 1;
		prog_selection->isb_lookup_found_mask =  0;
    }
 
    
    if(soc_property_get(unit, spn_BCM886XX_IP4_TUNNEL_TERMINATION_MODE, 3) == 1) {
		prog_selection->tcam_lookup_match = (0x1);
		prog_selection->tcam_lookup_match_mask = (0x0);
    }
	 
	
 	prog_selection->enable_routing_mask = 1;

	
	prog_selection->packet_format_qualifier_3_outer_vid_bits_mask = 1;

    return -1;
}

int32 arad_pp_dbal_tt_cam_bridge_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 bridge_cause)
{
    if(bridge_cause != BRIDGE_OAM){
        if (g_prog_soc_prop[unit].compression_spoof_ip6_enable || g_prog_soc_prop[unit].compression_ip6_enable)
        {
            prog_selection->ptc_tt_profile       = ARAD_PORTS_TT_PROFILE_PON;
            prog_selection->ptc_tt_profile_mask  = 0x0;
        }

        prog_selection->packet_format_code      = ARAD_PARSER_PFC_ETH;
        prog_selection->packet_format_code_mask = ARAD_PARSER_PFC_MATCH_TYPE;
        prog_selection->my_mac_mask             = (0x0);

        if(bridge_cause == BRIDGE_ROUTE_DISABLE) {
            prog_selection->my_mac                  = (0x1);
            prog_selection->enable_routing          = (0x0);
            prog_selection->enable_routing_mask     = (0x0);
        } 
        else if (bridge_cause == BRIDGE_ROUTE_DISABLE_IP) {
            prog_selection->my_mac                  = (0x1);
            prog_selection->enable_routing          = (0x0);
            prog_selection->enable_routing_mask     = (0x0);
            
            prog_selection->packet_format_code      = ARAD_PARSER_PFC_IPV4_ETH;
            prog_selection->packet_format_code_mask = ARAD_PARSER_PFC_MATCH_IPVx;
        }
        else {
            prog_selection->my_mac                  = (0x0);
            prog_selection->enable_routing_mask     = (0x1);
        }
    } else {
        
        prog_selection->packet_format_code      = ARAD_PARSER_PFC_ETH;
        prog_selection->packet_format_code_mask = ARAD_PARSER_PFC_MATCH_ONE;
        prog_selection->my_mac       = (0x1);
        prog_selection->my_mac_mask  = (0x0);
        prog_selection->packet_format_qualifier_1       = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_NEXT_PROTOCOL_CFM);
        prog_selection->packet_format_qualifier_1_mask  = (0x7F);
        prog_selection->enable_routing_mask     = (0x0);
        prog_selection->enable_routing          = (0x1);
    }

    return -1;
}

int32 arad_pp_dbal_tt_cam_uknown_l3_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int line, uint32 unused)
{
    prog_selection->packet_format_code               = ARAD_PARSER_PFC_ETH;
    prog_selection->packet_format_code_mask          = ARAD_PARSER_PFC_MATCH_ONE;
    prog_selection->packet_format_qualifier_1        = ARAD_PP_ISEM_ACCESS_QLFR_ETH_NEXT_PROTOCOL_MISS; 
    prog_selection->packet_format_qualifier_1_mask   = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_ENCAP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_TPID | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_OUTER_PCP | ARAD_PP_ISEM_ACCESS_QLFR_ETH_MASK_INNER_TPID );
    prog_selection->my_mac                           = 0x1;
    prog_selection->my_mac_mask                      = 0x0;
    prog_selection->enable_routing                   = 0x1;
    prog_selection->enable_routing_mask              = 0x0;

    return -1;
}

int32 arad_pp_dbal_tt_cam_trill_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 nof_tags )
{
    prog_selection->packet_format_code       = ARAD_PARSER_PFC_ETH_TRILL_ETH;
    prog_selection->packet_format_code_mask  = ARAD_PARSER_PFC_MATCH_ONE;
    prog_selection->vlan_is_designated       = (0x1);
    prog_selection->vlan_is_designated_mask  = (g_prog_soc_prop[unit].trill_disable_designated_vlan_check) ? (0x1) : (0x0);
    prog_selection->isa_lookup_found         = (nof_tags == TRILL_ONE_TAG) ? (0x0) : (0x1);
    prog_selection->isa_lookup_found_mask    = (0x0);
    prog_selection->enable_routing_mask      = (0x1);

    if(next_line == 0)
    {
        prog_selection->my_mac                   = (0x1);
        prog_selection->my_mac_mask              = (0x0);
        prog_selection->enable_routing           = (0x1);
        prog_selection->enable_routing_mask      = (0x0);
        return 1;
    }

    if(next_line == 1)
    {
        prog_selection->da_is_all_r_bridges      = (0x1);
        prog_selection->da_is_all_r_bridges_mask = (0x0);
        return -1;
    }

    return -1;
}
int32 arad_pp_dbal_tt_cam_trill_trap_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused)
{
    prog_selection->packet_format_code       = ARAD_PARSER_PFC_ETH_TRILL_ETH;
    prog_selection->packet_format_code_mask  = ARAD_PARSER_PFC_MATCH_ONE;
    prog_selection->enable_routing_mask      = (0x1);

    return -1;
}

int32 arad_pp_dbal_tt_cam_fc_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA *prog_selection, int next_line, uint32 vft_stat)
{
    prog_selection->enable_routing_mask = 1;
    prog_selection->parser_leaf_context = ARAD_PARSER_PLC_FCOE_VFT;

    if(vft_stat == NO_VFT)
    {
        prog_selection->parser_leaf_context_mask    = ARAD_PARSER_PLC_MATCH_ANY_FCOE;
        prog_selection->packet_format_code          = ARAD_PARSER_PFC_FCOE_ENCAP_ETH;
        prog_selection->packet_format_code_mask     = ARAD_PARSER_PFC_MATCH_ANY_FCOE;
    }

    if(vft_stat == WITH_VFT)
    {
        prog_selection->parser_leaf_context_mask    = ARAD_PARSER_PLC_MATCH_ONE;
        prog_selection->packet_format_code          = ARAD_PARSER_PFC_FCOE_STD_ETH;
        prog_selection->packet_format_code_mask     = ARAD_PARSER_PFC_MATCH_ONE;
    }

    if(next_line == 0)
    {
        prog_selection->my_mac      = (0x1);
        prog_selection->my_mac_mask = (0x0);

        if(soc_property_get(unit, spn_FCOE_NPV_BRIDGE_MODE, 0))
        {
           return 1;
        }
        else
        {
           return -1;
        }
    }

    if(next_line == 1)
    {
           prog_selection->in_pp_port_tt_profile       = SOC_PPC_PORT_DEFINED_TT_PROFILE_FORCE_MY_MAC;
           prog_selection->in_pp_port_tt_profile_mask  = (0x0);
           prog_selection->valid = 0x0;
    }

    return -1;
}

int32 arad_pp_dbal_tt_cam_mac_in_mac_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 mac_in_mac_type)
{
    prog_selection->packet_format_code             = ARAD_PARSER_PFC_ETH;
    prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_TYPE;
    prog_selection->packet_format_qualifier_1      = (ARAD_PP_ISEM_ACCESS_QLFR_ETH_NEXT_PROTOCOL_MAC_IN_MAC);
    prog_selection->packet_format_qualifier_1_mask = (0x7F);
    prog_selection->enable_routing_mask            = (0x1);

    if(mac_in_mac_type == MAC_IN_MAC_MC)
    {
        prog_selection->my_b_mac_mc                = (0x1);
        prog_selection->my_b_mac_mc_mask           = (0x0);
        prog_selection->ptc_tt_profile             = (ARAD_PORTS_TT_PROFILE_NONE);
        prog_selection->ptc_tt_profile_mask        = (0x0);
    }

    if (SOC_DPP_CONFIG(unit)->pp.test2) {
        if (next_line == 0) {
            prog_selection->tcam_lookup_match                         = (0x1);
            prog_selection->tcam_lookup_match_mask                    = (0x0);
            if (mac_in_mac_type == MAC_IN_MAC_MC)
                return -1;
            return 1;
        } else { 
            prog_selection->ptc_tt_profile                 = ARAD_PORTS_TT_PROFILE_OVERLAY_RCY;
            prog_selection->ptc_tt_profile_mask            = 0x0;
            return -1;
        }
    } else {
        if (next_line == 0) {
            prog_selection->my_mac                         = (0x1);
            prog_selection->my_mac_mask                    = (0x0);
            prog_selection->enable_routing                 = (0x1);
            prog_selection->enable_routing_mask            = (0x0);
            if (mac_in_mac_type == MAC_IN_MAC_MC)
                return -1;
            return 1;
        } else { 
            prog_selection->ptc_tt_profile                 = ARAD_PORTS_TT_PROFILE_OVERLAY_RCY;
            prog_selection->ptc_tt_profile_mask            = 0x0;
            return -1;
        }
    }
    return -1;
}

int32 arad_pp_dbal_tt_cam_ipv4_router_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused)
{
    prog_selection->packet_format_code      = ARAD_PARSER_PFC_IPV4_ETH;
    prog_selection->packet_format_code_mask = ARAD_PARSER_PFC_MATCH_ONE;
    prog_selection->my_mac                  = 0x1; 
    prog_selection->my_mac_mask             = 0x0;
    prog_selection->enable_routing          = 0x1;
    prog_selection->enable_routing_mask     = 0x0;

    if (g_prog_soc_prop[unit].ipv6_term_enable)
        prog_selection->packet_format_code_mask  = ARAD_PARSER_PFC_MATCH_ONE;
    else
        prog_selection->packet_format_code_mask  = ARAD_PARSER_PFC_MATCH_IPVx;

    
    if (SOC_DPP_CONFIG(unit)->pp.custom_pon_ipmc) {
        prog_selection->ptc_tt_profile       = 0;
        prog_selection->ptc_tt_profile_mask  = 0x0;
    }

    if ((SOC_DPP_CONFIG(unit)->pp.custom_feature_vrrp_scaling_tcam || (SOC_DPP_CONFIG(unit)->pp.vrrp_scaling_tcam)) && next_line == 0) {
        prog_selection->my_mac_mask                  = 0x1; 
        prog_selection->enable_routing_mask          = 0x1; 
        prog_selection->vt_processing_profile        = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_VRRP;
        prog_selection->vt_processing_profile_mask   = 0x0;
        prog_selection->tcam_lookup_match_mask       = 0x0; 
        prog_selection->tcam_lookup_match            = 0x1;

        return 1;
    }

    return -1;
}

int32 arad_pp_dbal_tt_cam_ipv4_router_tunnels_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused)
{
    prog_selection->packet_format_code      = ARAD_PARSER_PFC_IPV4_ETH;
    prog_selection->packet_format_code_mask = ARAD_PARSER_PFC_MATCH_ONE;
    prog_selection->my_mac                  = 0x1; 
    prog_selection->my_mac_mask             = 0x0;
    prog_selection->enable_routing          = 0x1;
    prog_selection->enable_routing_mask     = 0x0;

    if (g_prog_soc_prop[unit].ipv6_term_enable)
        prog_selection->packet_format_code_mask = ARAD_PARSER_PFC_MATCH_ONE;
    else
        
        prog_selection->packet_format_code_mask  = ARAD_PARSER_PFC_MATCH_ANY_ENCAP_ANY_IP;

    
    if (SOC_DPP_CONFIG(unit)->pp.custom_pon_ipmc) {
        prog_selection->ptc_tt_profile       = 0;
        prog_selection->ptc_tt_profile_mask  = 0x0;
    }

    if ((SOC_DPP_CONFIG(unit)->pp.custom_feature_vrrp_scaling_tcam || (SOC_DPP_CONFIG(unit)->pp.vrrp_scaling_tcam)) && next_line == 0) {
        prog_selection->my_mac_mask                  = 0x1; 
        prog_selection->enable_routing_mask          = 0x1; 
        prog_selection->vt_processing_profile        = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_VRRP;
        prog_selection->vt_processing_profile_mask   = 0x0;
        prog_selection->tcam_lookup_match_mask       = 0x0; 
        prog_selection->tcam_lookup_match            = 0x1;

        return 1;
    }

    return -1;
}
int32 arad_pp_dbal_tt_cam_ipv4_router_ipmc_after_rcy_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 is_vpls) {
    prog_selection->packet_format_code             = ARAD_PARSER_PFC_IPV4_ETH;
    if(g_prog_soc_prop[unit].ipv4mc_bidir_enable)
        prog_selection->packet_format_code_mask    = ARAD_PARSER_PFC_MATCH_ANY_ENCAP_ANY_TYPE;
    else
        prog_selection->packet_format_code_mask    = ARAD_PARSER_PFC_MATCH_ANY_ENCAP_ANY_TYPE_ANY_IP;
    prog_selection->packet_is_compatible_mc        = 0x1; 
    prog_selection->packet_is_compatible_mc_mask   = 0x0; 
    prog_selection->ptc_tt_profile                 = ARAD_PORTS_TT_PROFILE_OVERLAY_RCY;
    prog_selection->ptc_tt_profile_mask            = 0x0;
    prog_selection->enable_routing_mask            = 0x1;

    if (is_vpls) {
        prog_selection->packet_format_qualifier_2      = ARAD_PP_ISEM_ACCESS_QLFR_IP_NEXT_PROTOCOL_MPLS;
        prog_selection->packet_format_qualifier_2_mask = (0x7F);
    }

    return -1;
}


int32 arad_pp_dbal_tt_cam_ipv4_port_router_dip_found_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 tunnel_type) {
    prog_selection->packet_format_code      = ARAD_PARSER_PFC_ETH_IPV4_ETH;
    prog_selection->packet_format_code_mask = ARAD_PARSER_PFC_MATCH_ONE;
    prog_selection->in_pp_port_tt_profile =  (SOC_PPC_PORT_DEFINED_TT_PROFILE_PORT_TERMINATION);
    prog_selection->in_pp_port_tt_profile_mask = (0x0);

    if (tunnel_type == VXLAN) {
        prog_selection->parser_leaf_context = ARAD_PARSER_PLC_PP_L4;
        prog_selection->parser_leaf_context_mask = ARAD_PARSER_PLC_MATCH_ONE;
    } else {
        prog_selection->parser_leaf_context = ARAD_PARSER_PLC_PP;
        prog_selection->parser_leaf_context_mask = ARAD_PARSER_PLC_MATCH_ONE;
    }
    if ((g_prog_soc_prop[unit].mpls_1_database == ARAD_PP_ISEM_ACCESS_ID_ISEM_B)) {
        prog_selection->isb_lookup_found        = (0x1);
        prog_selection->isb_lookup_found_mask   = (g_prog_soc_prop[unit].ipv4mc_bidir_enable) ? 0x0 : 0x1; 
    } else {
        if ( (g_prog_soc_prop[unit].mpls_1_database == ARAD_PP_ISEM_ACCESS_ID_ISEM_A)) {
            prog_selection->isa_lookup_found        = (0x1);
            prog_selection->isa_lookup_found_mask   =  (g_prog_soc_prop[unit].ipv4mc_bidir_enable) ? 0x0 : 0x1; 
        } else {
            prog_selection->tcam_lookup_match        = (0x1);
            prog_selection->tcam_lookup_match_mask   = (g_prog_soc_prop[unit].ipv4mc_bidir_enable) ? 0x0 : 0x1;  
        }
    }

    
    if (SOC_DPP_CONFIG(unit)->pp.custom_pon_ipmc) {
        prog_selection->ptc_tt_profile       = 0;
        prog_selection->ptc_tt_profile_mask  = 0x0;
    }

    return -1;
}


int32 arad_pp_dbal_tt_cam_ipv4_router_dip_found_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 is_vpls)
{
    prog_selection->packet_format_code      = ARAD_PARSER_PFC_IPV4_ETH;
    prog_selection->packet_format_code_mask = ARAD_PARSER_PFC_MATCH_ANY_ENCAP_ANY_TYPE; 
    prog_selection->my_mac                  = (0x1);
    prog_selection->my_mac_mask             = (0x0);
    prog_selection->enable_routing          = (0x1);
    prog_selection->enable_routing_mask     = (0x0);

    if ((g_prog_soc_prop[unit].ipv4_term_dip_sip_enable == 0) && (g_prog_soc_prop[unit].mpls_1_database == ARAD_PP_ISEM_ACCESS_ID_ISEM_B))
    {
      prog_selection->isb_lookup_found        = (0x1);
      prog_selection->isb_lookup_found_mask   = (g_prog_soc_prop[unit].ipv4mc_bidir_enable) ? 0x0:0x1; 
    }
    else
    {
        if ((g_prog_soc_prop[unit].ipv4_term_dip_sip_enable == 0) && (g_prog_soc_prop[unit].mpls_1_database == ARAD_PP_ISEM_ACCESS_ID_ISEM_A))
        {
            prog_selection->isa_lookup_found        = (0x1);
            prog_selection->isa_lookup_found_mask   = (g_prog_soc_prop[unit].ipv4mc_bidir_enable) ? 0x0:0x1; 
        }
        else
        {
            prog_selection->tcam_lookup_match        = (0x1);
            prog_selection->tcam_lookup_match_mask   = (g_prog_soc_prop[unit].ipv4mc_bidir_enable) ? 0x0:0x1; 
        }
    }

    if(is_vpls)
    {
        prog_selection->packet_format_qualifier_2        = ARAD_PP_ISEM_ACCESS_QLFR_IP_NEXT_PROTOCOL_MPLS;
        prog_selection->packet_format_qualifier_2_mask   = (0x7F);
    }

    
    if (SOC_DPP_CONFIG(unit)->pp.custom_pon_ipmc) {
        prog_selection->ptc_tt_profile       = 0;
        prog_selection->ptc_tt_profile_mask  = 0x0;
    }

    if (SOC_DPP_CONFIG(unit)->pp.custom_feature_vrrp_scaling_tcam && next_line == 0) {
        prog_selection->my_mac_mask                  = 0x1; 
        prog_selection->enable_routing_mask          = 0x1; 
        prog_selection->vt_processing_profile        = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_VRRP;
        prog_selection->vt_processing_profile_mask   = 0x0;
        prog_selection->tcam_lookup_match_mask       = 0x0; 
        prog_selection->tcam_lookup_match            = 0x1;
        return 1;
    }

    return -1;
}

int32 arad_pp_dbal_tt_cam_ipv4_router_uc_dip_not_found_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused)
{
    prog_selection->packet_format_code           = ARAD_PARSER_PFC_IPV4_ETH;
    prog_selection->packet_format_code_mask      = ARAD_PARSER_PFC_MATCH_ANY_ENCAP_ANY_TYPE; 
    prog_selection->packet_is_compatible_mc      = 0x0;
    prog_selection->packet_is_compatible_mc_mask = 0x0;
    prog_selection->my_mac                       = 0x1;
    prog_selection->my_mac_mask                  = 0x0;
    prog_selection->enable_routing               = 0x1;
    prog_selection->enable_routing_mask          = 0x0;

    if (g_prog_soc_prop[unit].mpls_1_database == ARAD_PP_ISEM_ACCESS_ID_ISEM_B)
    {
      prog_selection->isb_lookup_found      = 0x0;
      prog_selection->isb_lookup_found_mask = 0x0;
    }
    else
    {
      prog_selection->isa_lookup_found      = 0x0;
      prog_selection->isa_lookup_found_mask = 0x0;
    }

    return -1;
}
int32 arad_pp_dbal_tt_cam_ipv4_router_mc_dip_not_found_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused)
{
    prog_selection->packet_format_code           = ARAD_PARSER_PFC_IPV4_ETH;
    prog_selection->packet_format_code_mask      = ARAD_PARSER_PFC_MATCH_ANY_ENCAP_ANY_TYPE; 
    prog_selection->packet_is_compatible_mc      = 0x1;
    prog_selection->packet_is_compatible_mc_mask = 0x0;
    prog_selection->enable_routing_mask          = 0x1;

    if (g_prog_soc_prop[unit].mpls_1_database == ARAD_PP_ISEM_ACCESS_ID_ISEM_B)
    {
      prog_selection->isb_lookup_found      = 0x0;
      prog_selection->isb_lookup_found_mask = 0x0;
    }
    else
    {
      prog_selection->isa_lookup_found      = 0x0;
      prog_selection->isa_lookup_found_mask = 0x0;
    }

    return -1;
}

int32 arad_pp_dbal_tt_cam_ipv4_router_compatible_mc_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused)
{
    prog_selection->packet_format_code           = ARAD_PARSER_PFC_IPV6_ETH;
    if(g_prog_soc_prop[unit].ipv4mc_bidir_enable)
        prog_selection->packet_format_code_mask  = ARAD_PARSER_PFC_MATCH_ANY_ENCAP_ANY_TYPE;
    else
        prog_selection->packet_format_code_mask  = ARAD_PARSER_PFC_MATCH_ANY_ENCAP_ANY_TYPE_ANY_IP;
    prog_selection->packet_is_compatible_mc      = 0x1;
    prog_selection->packet_is_compatible_mc_mask = 0x0;
    prog_selection->enable_routing_mask          = 0x1;

    
    if (SOC_DPP_CONFIG(unit)->pp.custom_pon_ipmc) {
        prog_selection->ptc_tt_profile       = 0;
        prog_selection->ptc_tt_profile_mask  = 0x0;
    }

    return -1;
}

int32 arad_pp_dbal_tt_cam_pon_ipv4_bridge_compatible_mc_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused)
{
    prog_selection->packet_format_code           = ARAD_PARSER_PFC_IPV6_ETH;
    if(g_prog_soc_prop[unit].ipv4mc_bidir_enable)
        prog_selection->packet_format_code_mask  = ARAD_PARSER_PFC_MATCH_ANY_ENCAP_ANY_TYPE;
    else
        prog_selection->packet_format_code_mask  = ARAD_PARSER_PFC_MATCH_ANY_ENCAP_ANY_TYPE_ANY_IP;
    prog_selection->packet_is_compatible_mc      = 0x1;
    prog_selection->packet_is_compatible_mc_mask = 0x0;
    prog_selection->enable_routing_mask          = 0x1;

    
    if (SOC_DPP_CONFIG(unit)->pp.custom_pon_ipmc) {
        prog_selection->ptc_tt_profile       = ARAD_PORTS_TT_PROFILE_PON;
        prog_selection->ptc_tt_profile_mask  = 0x0;
    }

    return -1;
}

int32 arad_pp_dbal_tt_cam_pon_ipv4_bridge_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused)
{
    prog_selection->packet_format_code      = ARAD_PARSER_PFC_IPV4_ETH;
    prog_selection->packet_format_code_mask = ARAD_PARSER_PFC_MATCH_ONE;
    prog_selection->my_mac                  = 0x1; 
    prog_selection->my_mac_mask             = 0x0;
    prog_selection->enable_routing          = 0x1;
    prog_selection->enable_routing_mask     = 0x0;

    if (g_prog_soc_prop[unit].ipv6_term_enable) {
        prog_selection->packet_format_code_mask = ARAD_PARSER_PFC_MATCH_ONE;
    }
    else {
        prog_selection->packet_format_code_mask = ARAD_PARSER_PFC_MATCH_IPVx;
    }

    
    if (SOC_DPP_CONFIG(unit)->pp.custom_pon_ipmc) {
        prog_selection->ptc_tt_profile       = ARAD_PORTS_TT_PROFILE_PON;
        prog_selection->ptc_tt_profile_mask  = 0x0;
    }

    return -1;
}

int32 arad_pp_dbal_tt_cam_pon_ipv6_bridge_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused)
{
    prog_selection->packet_format_code      = ARAD_PARSER_PFC_IPV6_ETH;
    prog_selection->packet_format_code_mask = ARAD_PARSER_PFC_MATCH_ANY_ENCAP_ANY_TYPE; 
    prog_selection->my_mac                  = 0x1;
    prog_selection->my_mac_mask             = 0x0;
    prog_selection->enable_routing          = 0x1;
    prog_selection->enable_routing_mask     = 0x0;

    
    if (SOC_DPP_CONFIG(unit)->pp.custom_pon_ipmc) {
        prog_selection->ptc_tt_profile       = ARAD_PORTS_TT_PROFILE_PON;
        prog_selection->ptc_tt_profile_mask  = 0x0;
    }

    return -1;
}

int32 arad_pp_dbal_tt_cam_pon_ipv4_bridge_dip_found_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 is_vpls)
{
    prog_selection->packet_format_code      = ARAD_PARSER_PFC_IPV4_ETH;
    prog_selection->packet_format_code_mask = ARAD_PARSER_PFC_MATCH_ANY_ENCAP_ANY_TYPE; 
    prog_selection->my_mac                  = (0x1);
    prog_selection->my_mac_mask             = (0x0);
    prog_selection->enable_routing          = (0x1);
    prog_selection->enable_routing_mask     = (0x0);

    if ((g_prog_soc_prop[unit].ipv4_term_dip_sip_enable == 0) && (g_prog_soc_prop[unit].mpls_1_database == ARAD_PP_ISEM_ACCESS_ID_ISEM_B))
    {
      prog_selection->isb_lookup_found        = (0x1);
      prog_selection->isb_lookup_found_mask   = (g_prog_soc_prop[unit].ipv4mc_bidir_enable) ? 0x0:0x1; 
    }
    else
    {
        if ((g_prog_soc_prop[unit].ipv4_term_dip_sip_enable == 0) && (g_prog_soc_prop[unit].mpls_1_database == ARAD_PP_ISEM_ACCESS_ID_ISEM_A))
        {
            prog_selection->isa_lookup_found        = (0x1);
            prog_selection->isa_lookup_found_mask   = (g_prog_soc_prop[unit].ipv4mc_bidir_enable) ? 0x0:0x1; 
        }
        else
        {
            prog_selection->tcam_lookup_match        = (0x1);
            prog_selection->tcam_lookup_match_mask   = (g_prog_soc_prop[unit].ipv4mc_bidir_enable) ? 0x0:0x1; 
        }
    }

    if(is_vpls)
    {
        prog_selection->packet_format_qualifier_2        = ARAD_PP_ISEM_ACCESS_QLFR_IP_NEXT_PROTOCOL_MPLS;
        prog_selection->packet_format_qualifier_2_mask   = (0x7F);
    }

    
    if (SOC_DPP_CONFIG(unit)->pp.custom_pon_ipmc) {
        prog_selection->ptc_tt_profile       = ARAD_PORTS_TT_PROFILE_PON;
        prog_selection->ptc_tt_profile_mask  = 0x0;
    }

    return -1;
}

int32 arad_pp_dbal_tt_cam_ipv6_router_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 is_recycle)
{
    prog_selection->packet_format_code      = ARAD_PARSER_PFC_IPV6_ETH;
    prog_selection->packet_format_code_mask = ARAD_PARSER_PFC_MATCH_ANY_ENCAP_ANY_TYPE; 
    if (is_recycle) 
    {
        prog_selection->packet_is_compatible_mc          = 0x1;
        prog_selection->packet_is_compatible_mc_mask     = 0x0;
        prog_selection->ptc_tt_profile          = ARAD_PORTS_TT_PROFILE_OVERLAY_RCY;
        prog_selection->ptc_tt_profile_mask     = 0x0;
        prog_selection->enable_routing_mask     = 0x1;
    } else {        
        prog_selection->my_mac                  = 0x1;
        prog_selection->my_mac_mask             = 0x0;
        prog_selection->enable_routing          = 0x1;
        prog_selection->enable_routing_mask     = 0x0;
    }

    
    if (SOC_DPP_CONFIG(unit)->pp.custom_pon_ipmc) {
        prog_selection->ptc_tt_profile       = 0;
        prog_selection->ptc_tt_profile_mask  = 0x0;
    }

    if (g_prog_soc_prop[unit].ipv6_term_dip_sip_enable) {
      prog_selection->tcam_lookup_match        = (0x0);
      prog_selection->tcam_lookup_match_mask   = 0; 
    }

    if ((SOC_DPP_CONFIG(unit)->pp.vrrp_scaling_tcam) && next_line == 0) {
        prog_selection->my_mac_mask                  = 0x1; 
        prog_selection->enable_routing_mask          = 0x1; 
        prog_selection->vt_processing_profile        = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_VRRP;
        prog_selection->vt_processing_profile_mask   = 0x0;
        prog_selection->tcam_lookup_match_mask       = 0x0; 
        prog_selection->tcam_lookup_match            = 0x1;

        return 1;
    }

    return -1;
}

int32 arad_pp_dbal_tt_cam_dipv6_compressed_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused )
{
    prog_selection->ptc_tt_profile          = SOC_TMC_PORTS_TT_PROFILE_NONE;
    prog_selection->ptc_tt_profile_mask     = 0x0;
    prog_selection->packet_format_code      = ARAD_PARSER_PFC_ETH;
    prog_selection->packet_format_code_mask = ARAD_PARSER_PFC_MATCH_TYPE;
    prog_selection->my_mac                  = (0x0);
    prog_selection->my_mac_mask             = (0x0);
    prog_selection->enable_routing_mask     = (0x1);

    return -1;
}

int32 arad_pp_dbal_tt_cam_ipv6_router_comp_dest_found_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 is_recycle)
{
    prog_selection->packet_format_code      = ARAD_PARSER_PFC_IPV6_ETH;
    prog_selection->packet_format_code_mask = ARAD_PARSER_PFC_MATCH_ANY_ENCAP_ANY_TYPE; 
    prog_selection->vt_processing_profile        = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_IPV6_CASCADE;
    prog_selection->vt_processing_profile_mask   = 0x0;    
    prog_selection->tcam_lookup_match            = 0x1;
    prog_selection->tcam_lookup_match_mask       = 0x0; 
    if (is_recycle) 
    {
        prog_selection->packet_is_compatible_mc          = 0x1;
        prog_selection->packet_is_compatible_mc_mask     = 0x0;
        prog_selection->ptc_tt_profile    	= ARAD_PORTS_TT_PROFILE_OVERLAY_RCY;
        prog_selection->ptc_tt_profile_mask = 0x0;
        prog_selection->enable_routing_mask = 0x1;
    } else {
        prog_selection->my_mac                  = 0x1;
        prog_selection->my_mac_mask             = 0x0;
        prog_selection->enable_routing          = 0x1;
        prog_selection->enable_routing_mask     = 0x0;
    }

    return -1;
}

int32 arad_pp_dbal_tt_cam_mpls_mac_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line )
{
    prog_selection->enable_routing_mask     = (0x1);

    if(next_line == 0)
    {
        prog_selection->my_mac              = (0x1);
        prog_selection->my_mac_mask         = (0x0);
        prog_selection->enable_routing      = (0x1);
        prog_selection->enable_routing_mask = (0x0);

        if( g_prog_soc_prop[unit].mpls_tp_mac_address)
            return 1;
        return -1;
    }

    if(next_line == 1)
    {
        prog_selection->da_is_all_r_bridges      = (0x1);
        prog_selection->da_is_all_r_bridges_mask = (0x0);
        return -1;
    }

    return -1;
}

int32 arad_pp_dbal_tt_cam_mpls_mac_and_raw_mpls_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line)
{
    prog_selection->enable_routing_mask                 = (0x1);

    if(next_line == 0)
    {
        prog_selection->my_mac                          = (0x1);
        prog_selection->my_mac_mask                     = (0x0);
        prog_selection->enable_routing                  = (0x1);
        prog_selection->enable_routing_mask             = (0x0);

        if( g_prog_soc_prop[unit].mpls_tp_mac_address || g_prog_soc_prop[unit].port_raw_mpls)
            return 1;
        return -1;
    }

    if(next_line == 1)
    {
        if(g_prog_soc_prop[unit].mpls_tp_mac_address)
        {
            prog_selection->da_is_all_r_bridges      = (0x1);
            prog_selection->da_is_all_r_bridges_mask = (0x0);

            if( g_prog_soc_prop[unit].port_raw_mpls)
                return 2;
            return -1;
        }
        else
        {
            prog_selection->in_pp_port_tt_profile      = SOC_PPC_PORT_DEFINED_TT_PROFILE_FORCE_MY_MAC;
            prog_selection->in_pp_port_tt_profile_mask = (0x0);
            return -1;
        }
    }

    if(next_line == 2)
    {
        prog_selection->in_pp_port_tt_profile      = SOC_PPC_PORT_DEFINED_TT_PROFILE_FORCE_MY_MAC;
        prog_selection->in_pp_port_tt_profile_mask = (0x0);
        return -1;
    }
    return -1;
}


int32 arad_pp_dbal_tt_cam_ipv4_router_dip_sip_vrf_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection,int next_line, uint32 dummy)
{
    prog_selection->packet_format_code      = ARAD_PARSER_PFC_IPV4_ETH;
    prog_selection->packet_format_code_mask = ARAD_PARSER_PFC_MATCH_ANY_ENCAP_ANY_TYPE; 
    
    if (dummy) {
        prog_selection->in_pp_port_tt_profile =  (SOC_PPC_PORT_DEFINED_TT_PROFILE_PORT_TERMINATION);
        prog_selection->in_pp_port_tt_profile_mask = (0x0);

    } else {
        prog_selection->my_mac                  = (0x1);
        prog_selection->my_mac_mask             = (0x0);
        prog_selection->enable_routing          = (0x1);
        prog_selection->enable_routing_mask     = (0x0);
    }

    

    
    if (next_line == 0) {
        prog_selection->packet_format_qualifier_2      = (0x700);
        prog_selection->packet_format_qualifier_2_mask = (0xFF);
        return 1;  
    }
    
    else if (next_line == 1) {
        prog_selection->packet_format_qualifier_2      = (0x680);
        prog_selection->packet_format_qualifier_2_mask = (0x7F);
        return -1; 
    }
  
    return -1;
}

int32 arad_pp_dbal_tt_cam_ipv4_for_explicit_null_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection,int next_line, uint32 unused)
{
    prog_selection->my_mac                  = (0x1);
    prog_selection->my_mac_mask             = (0x0);
    prog_selection->enable_routing          = (0x1);
    prog_selection->enable_routing_mask     = (0x0);

    if (next_line == 0)
    {
        prog_selection->packet_format_code      = ARAD_PARSER_PFC_MPLS1_ETH;
        prog_selection->packet_format_code_mask = ARAD_PARSER_PFC_MATCH_ANY_ENCAP;
        prog_selection->packet_format_qualifier_2       = (0x600); 
        prog_selection->packet_format_qualifier_2_mask  = (0x1FB);
        return 1;    	  
    }
    
    if (next_line == 1)
    {
        prog_selection->packet_format_code      = ARAD_PARSER_PFC_MPLS2_ETH;
        prog_selection->packet_format_code_mask = ARAD_PARSER_PFC_MATCH_ANY_ENCAP;
        prog_selection->packet_format_qualifier_2       = (0x600); 
        prog_selection->packet_format_qualifier_2_mask  = (0x1EB);
        return 2;    	  
    }
    
    
    if (next_line == 2)
    {
        prog_selection->packet_format_code      = ARAD_PARSER_PFC_MPLS3_ETH;
        prog_selection->packet_format_code_mask = ARAD_PARSER_PFC_MATCH_ANY_ENCAP;
        prog_selection->packet_format_qualifier_2       = (0x601); 
        prog_selection->packet_format_qualifier_2_mask  = (0x1AC);
        return -1;    	  
    }
    
    return -1; 
}


int32 arad_pp_dbal_tt_cam_tunnel_for_two_explicit_null_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection,int next_line, uint32 unused)
{
    prog_selection->my_mac                  = (0x1);
    prog_selection->my_mac_mask             = (0x0);
    prog_selection->enable_routing          = (0x1);
    prog_selection->enable_routing_mask     = (0x0);
    prog_selection->packet_format_code      = ARAD_PARSER_PFC_MPLS3_ETH;
    prog_selection->packet_format_code_mask = ARAD_PARSER_PFC_MATCH_ANY_ENCAP_ANY_TYPE; 
	prog_selection->packet_format_qualifier_2       = (0x0); 
	prog_selection->packet_format_qualifier_2_mask  = (0x7EB); 

	
    return -1; 
}

int32 arad_pp_dbal_tt_cam_ip_disable_on_lif_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection,int next_line, uint32 type)
{
    uint32 lif_disable_bits = soc_property_get(unit, spn_L3_DISABLED_BIT_ON_LIF, 0);
    uint32 v4_mask,v6_mask;

    v4_mask = 1<<(lif_disable_bits&3);
    v6_mask =  1<<((lif_disable_bits/4)&3);


    prog_selection->my_mac                  = (0x1);
    prog_selection->my_mac_mask             = (0x0);
    prog_selection->enable_routing          = (0x1);
    prog_selection->enable_routing_mask     = (0x0);

    if (type == 1) {
        prog_selection->vt_in_lif_profile_mask = 0xF&(~v4_mask);
        prog_selection->vt_in_lif_profile          = v4_mask;

        if (next_line == 0) {
            prog_selection->packet_format_code      = ARAD_PARSER_PFC_IPV4_ETH;
            prog_selection->packet_format_code_mask = ARAD_PARSER_PFC_MATCH_ANY_ENCAP_ANY_IP; 
            return 1;
        }

        if (next_line == 1) {
            prog_selection->packet_format_code      = ARAD_PARSER_PFC_MPLS1_ETH;  
            prog_selection->packet_format_code_mask = ARAD_PARSER_PFC_MATCH_ANY_ENCAP;
            prog_selection->packet_format_qualifier_2       = (0x600); 
            prog_selection->packet_format_qualifier_2_mask  = (0x1FB);

            return 2; 
        }

        if (next_line == 2) {
            prog_selection->packet_format_code      = ARAD_PARSER_PFC_MPLS2_ETH;  
            prog_selection->packet_format_code_mask = ARAD_PARSER_PFC_MATCH_ANY_ENCAP;
            prog_selection->packet_format_qualifier_2       = (0x600); 
            prog_selection->packet_format_qualifier_2_mask  = (0x1EB);

            return 3; 
        }

        if (next_line == 3) {
            prog_selection->packet_format_code      = ARAD_PARSER_PFC_MPLS3_ETH;  
            prog_selection->packet_format_code_mask = ARAD_PARSER_PFC_MATCH_ANY_ENCAP;
            prog_selection->packet_format_qualifier_2       = (0x601); 
            prog_selection->packet_format_qualifier_2_mask  = (0x1AC);
            return -1; 
        }


    } else if(type==2){
        prog_selection->vt_in_lif_profile_mask = 0xF&(~v4_mask);
        prog_selection->vt_in_lif_profile          = v4_mask;

        if (next_line == 0) {
            prog_selection->packet_format_code      = ARAD_PARSER_PFC_IPV4_ETH;
            prog_selection->packet_format_code_mask = ARAD_PARSER_PFC_MATCH_ANY_ENCAP; 

            return 1;
        }
        
        if (next_line == 1) {
            prog_selection->packet_format_code      = ARAD_PARSER_PFC_MPLS1_ETH;  
            prog_selection->packet_format_code_mask = ARAD_PARSER_PFC_MATCH_ANY_ENCAP;
            prog_selection->packet_format_qualifier_2       = (0x680); 
            prog_selection->packet_format_qualifier_2_mask  = (0x7B);
        
            return 2; 
        }
        
        if (next_line == 2) {
            prog_selection->packet_format_code      = ARAD_PARSER_PFC_MPLS2_ETH;  
            prog_selection->packet_format_code_mask = ARAD_PARSER_PFC_MATCH_ANY_ENCAP;
            prog_selection->packet_format_qualifier_2       = (0x680); 
            prog_selection->packet_format_qualifier_2_mask  = (0x6B);
        
            return 3; 
        }
        
        if (next_line == 3) {
            prog_selection->packet_format_code      = ARAD_PARSER_PFC_MPLS3_ETH;  
            prog_selection->packet_format_code_mask = ARAD_PARSER_PFC_MATCH_ANY_ENCAP;
            prog_selection->packet_format_qualifier_2       = (0x681); 
            prog_selection->packet_format_qualifier_2_mask  = (0x2C);
            return -1; 
        }




    } else {
        prog_selection->vt_in_lif_profile_mask = 0xF&(~v6_mask);
        prog_selection->vt_in_lif_profile          = v6_mask;

        if (next_line == 0) {
            prog_selection->packet_format_code      = ARAD_PARSER_PFC_IPV6_ETH;
            prog_selection->packet_format_code_mask = ARAD_PARSER_PFC_MATCH_ANY_ENCAP;
        
            return 1;
        }
        
        if (next_line == 1) {
            prog_selection->packet_format_code      = ARAD_PARSER_PFC_MPLS1_ETH;  
            prog_selection->packet_format_code_mask = ARAD_PARSER_PFC_MATCH_ANY_ENCAP;
            prog_selection->packet_format_qualifier_2       = (0x700); 
            prog_selection->packet_format_qualifier_2_mask  = (0x7B);
        
            return 2; 
        }
        
        if (next_line == 2) {
            prog_selection->packet_format_code      = ARAD_PARSER_PFC_MPLS2_ETH;  
            prog_selection->packet_format_code_mask = ARAD_PARSER_PFC_MATCH_ANY_ENCAP;
            prog_selection->packet_format_qualifier_2       = (0x700); 
            prog_selection->packet_format_qualifier_2_mask  = (0x6B);
        
            return 3; 
        }
        
        if (next_line == 3) {
            prog_selection->packet_format_code      = ARAD_PARSER_PFC_MPLS3_ETH;  
            prog_selection->packet_format_code_mask = ARAD_PARSER_PFC_MATCH_ANY_ENCAP;
            prog_selection->packet_format_qualifier_2       = (0x701); 
            prog_selection->packet_format_qualifier_2_mask  = (0x2C);
            return -1; 
        }

    }
    return -1;
}


int32 arad_pp_dbal_tt_cam_vlan_domain_mpls_l2_set(int unit,  ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int line, uint32 unused)
{

    prog_selection->in_pp_port_tt_profile = SOC_PPC_PORT_DEFINED_TT_PROFILE_VLAN_DOMAIN_MPLS;
    prog_selection->in_pp_port_tt_profile_mask = 0;

    prog_selection->packet_format_code      = ARAD_PARSER_PFC_MPLS4P_ETH;
    prog_selection->packet_format_code_mask = ARAD_PARSER_PFC_MATCH_ANY_MPLS;
   
    if (line == 0){
        prog_selection->my_mac                          = (0x1);
        prog_selection->my_mac_mask                     = (0x0);
        prog_selection->enable_routing             = (0x1);
        prog_selection->enable_routing_mask        = (0x0);
        if (g_prog_soc_prop->ingress_full_mymac_1) {
            return 1;
        }
        return -1;
    }

    if (g_prog_soc_prop->ingress_full_mymac_1 && line == 1) {
        prog_selection->my_mac_mask                     = (0x1); 
        prog_selection->enable_routing_mask             = (0x1); 
        prog_selection->tcam_lookup_match               = (0x1);
        prog_selection->tcam_lookup_match_mask          = (0x0);
        return -1;
    }

    return -1;
}

int32 arad_pp_dbal_tt_cam_3mtse_plus_frr_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 vtt_3mtse_plus_frr_type)
{

    switch(vtt_3mtse_plus_frr_type)
    {
        case VTT_3MTSE_PLUS_FRR_M1:
            prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS1_ETH;
            prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP; 
            prog_selection->vt_processing_profile          = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_3MTSE_PLUS_FRR_NO_MPLS;
            prog_selection->vt_processing_profile_mask     = 0x0;
            prog_selection->in_pp_port_tt_profile          =  (SOC_PPC_PORT_DEFINED_TT_PROFILE_DEFAULT);
            prog_selection->in_pp_port_tt_profile_mask     = (0x0);

            break;
        case VTT_3MTSE_PLUS_FRR_M2_HIT_SEM:
            prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS2_ETH; 
            prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP; 
            prog_selection->in_pp_port_tt_profile          =  (SOC_PPC_PORT_DEFINED_TT_PROFILE_DEFAULT);
            prog_selection->in_pp_port_tt_profile_mask     = (0x0);
            prog_selection->vt_processing_profile          = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_3MTSE_PLUS_FRR_TCAM;
            prog_selection->vt_processing_profile_mask     = 0x0;
            prog_selection->packet_format_qualifier_2      = (0x01E);
            prog_selection->packet_format_qualifier_2_mask = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_R1_V | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L3 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_BOS_NEXT_PROT;
            if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 20) {
            	prog_selection->isb_lookup_found               = (0x1);
            	prog_selection->isb_lookup_found_mask          = (0x0);
            }
            if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 21) {
            	prog_selection->isa_lookup_found               = (0x1);
                prog_selection->isa_lookup_found_mask          = (0x0);
            }
            break;
        case VTT_3MTSE_PLUS_FRR_M2_MISS_SEM:
        	prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS2_ETH; 
        	prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP; 
            prog_selection->in_pp_port_tt_profile          =  (SOC_PPC_PORT_DEFINED_TT_PROFILE_DEFAULT);
            prog_selection->in_pp_port_tt_profile_mask     = (0x0);
            prog_selection->vt_processing_profile          = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_3MTSE_PLUS_FRR_TCAM;
        	prog_selection->vt_processing_profile_mask     = 0x0;
        	prog_selection->packet_format_qualifier_2      = (0x01E);
        	prog_selection->packet_format_qualifier_2_mask = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_R1_V | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_R2_W | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L3 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_BOS_NEXT_PROT;
            if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 20) {
            	prog_selection->isb_lookup_found               = (0x0);
            	prog_selection->isb_lookup_found_mask          = (0x0);
            }
            if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 21) {
            	prog_selection->isa_lookup_found               = (0x0);
                prog_selection->isa_lookup_found_mask          = (0x0);
            }
            break;

        case VTT_3MTSE_PLUS_FRR_M2:
        	prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS2_ETH; 
        	prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP; 
            prog_selection->in_pp_port_tt_profile          =  (SOC_PPC_PORT_DEFINED_TT_PROFILE_DEFAULT);
            prog_selection->in_pp_port_tt_profile_mask     = (0x0);
            prog_selection->vt_processing_profile          = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_3MTSE_PLUS_FRR_TCAM;
        	prog_selection->vt_processing_profile_mask     = 0x0;

            break;

        case VTT_3MTSE_PLUS_FRR_M3_PLUS_ELI_IML_SEM_HIT:
            prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS3_ETH; 
            prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP; 
            prog_selection->in_pp_port_tt_profile          =  (SOC_PPC_PORT_DEFINED_TT_PROFILE_DEFAULT);
            prog_selection->in_pp_port_tt_profile_mask     = (0x0);
            prog_selection->vt_processing_profile          = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_3MTSE_PLUS_FRR_TCAM;
            prog_selection->vt_processing_profile_mask     = 0x0;
            prog_selection->packet_format_qualifier_2      = (0x60);
            prog_selection->packet_format_qualifier_2_mask = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L2  | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_NEXT_PROT;
            prog_selection->packet_format_qualifier_3      = (0x6);
            prog_selection->packet_format_qualifier_3_mask = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_R1_W | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L2  | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L3 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_BOS_NEXT_PROT;
            if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 20) {
                prog_selection->isb_lookup_found               = (0x1);
                prog_selection->isb_lookup_found_mask          = (0x0);
            }
            if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 21) {
                prog_selection->isa_lookup_found               = (0x1);
                prog_selection->isa_lookup_found_mask          = (0x0);
            }
            break;

        case VTT_3MTSE_PLUS_FRR_M3_PLUS_ELI_IML_SEM_MISS:
            prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS3_ETH; 
            prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP; 
            prog_selection->in_pp_port_tt_profile          =  (SOC_PPC_PORT_DEFINED_TT_PROFILE_DEFAULT);
            prog_selection->in_pp_port_tt_profile_mask     = (0x0);
            prog_selection->vt_processing_profile          = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_3MTSE_PLUS_FRR_TCAM;
            prog_selection->vt_processing_profile_mask     = 0x0;
            prog_selection->packet_format_qualifier_2      = (0x60);
            prog_selection->packet_format_qualifier_2_mask = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L2  | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_NEXT_PROT;
            prog_selection->packet_format_qualifier_3      = (0x6);
            prog_selection->packet_format_qualifier_3_mask = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_R1_W | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L2  | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L3 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_BOS_NEXT_PROT;
            if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 20) {
                prog_selection->isb_lookup_found               = (0x0);
                prog_selection->isb_lookup_found_mask          = (0x0);
            }
            if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 21) {
                prog_selection->isa_lookup_found               = (0x0);
                prog_selection->isa_lookup_found_mask          = (0x0);
            }
            break;


        case VTT_3MTSE_PLUS_FRR_M3_PLUS_ELI:
        	prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS3_ETH; 
        	prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP; 
            prog_selection->in_pp_port_tt_profile          =  (SOC_PPC_PORT_DEFINED_TT_PROFILE_DEFAULT);
            prog_selection->in_pp_port_tt_profile_mask     = (0x0);
            prog_selection->vt_processing_profile          = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_3MTSE_PLUS_FRR_TCAM;
        	prog_selection->vt_processing_profile_mask     = 0x0;
        	prog_selection->packet_format_qualifier_2      = (0x0);
        	prog_selection->packet_format_qualifier_2_mask = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L2 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L3 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_BOS_NEXT_PROT;
            break;

        case VTT_3MTSE_PLUS_FRR_M3_ELI:
        	prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS3_ETH; 
        	prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP; 
            prog_selection->in_pp_port_tt_profile          =  (SOC_PPC_PORT_DEFINED_TT_PROFILE_DEFAULT);
            prog_selection->in_pp_port_tt_profile_mask     = (0x0);
            prog_selection->vt_processing_profile          = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_3MTSE_PLUS_FRR_TCAM;
        	prog_selection->vt_processing_profile_mask     = 0x0;
        	prog_selection->packet_format_qualifier_2      = (0x1);
        	prog_selection->packet_format_qualifier_2_mask = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L1 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L3 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_NEXT_PROT;
            break;

        case VTT_3MTSE_PLUS_FRR_M3_HIT_SEM_AND_TCAM:
        	prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS3_ETH; 
        	prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP; 
            prog_selection->in_pp_port_tt_profile          =  (SOC_PPC_PORT_DEFINED_TT_PROFILE_DEFAULT);
            prog_selection->in_pp_port_tt_profile_mask     = (0x0);
            prog_selection->vt_processing_profile          = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_3MTSE_PLUS_FRR_TCAM;
        	prog_selection->vt_processing_profile_mask     = 0x0;
        	prog_selection->packet_format_qualifier_2      = (0x7F);
        	prog_selection->packet_format_qualifier_2_mask = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_R1_V | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_R2_V | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_R3_W | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_NEXT_PROT;
        	prog_selection->tcam_lookup_match               = (0x1);
        	prog_selection->tcam_lookup_match_mask          = (0x0);
			if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 20) {
            	prog_selection->isb_lookup_found               = (0x1);
            	prog_selection->isb_lookup_found_mask          = (0x0);
            }
            if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 21) {
            	prog_selection->isa_lookup_found               = (0x1);
                prog_selection->isa_lookup_found_mask          = (0x0);
            }
            break;

        case VTT_3MTSE_PLUS_FRR_M3_DC_SEM_AND_TCAM:
        	prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS3_ETH; 
        	prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP; 
            prog_selection->in_pp_port_tt_profile          =  (SOC_PPC_PORT_DEFINED_TT_PROFILE_DEFAULT);
            prog_selection->in_pp_port_tt_profile_mask     = (0x0);
            prog_selection->vt_processing_profile          = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_3MTSE_PLUS_FRR_TCAM;
        	prog_selection->vt_processing_profile_mask     = 0x0;
        	prog_selection->packet_format_qualifier_2      = (0x7F);
        	prog_selection->packet_format_qualifier_2_mask = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_R1_V | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_R2_V | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_R3_W | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_NEXT_PROT;
            break;

        case VTT_3MTSE_PLUS_FRR_M3_HIT_SEM:
        	prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS3_ETH; 
        	prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP; 
            prog_selection->in_pp_port_tt_profile          =  (SOC_PPC_PORT_DEFINED_TT_PROFILE_DEFAULT);
            prog_selection->in_pp_port_tt_profile_mask     = (0x0);
            prog_selection->vt_processing_profile          = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_3MTSE_PLUS_FRR_TCAM;
        	prog_selection->vt_processing_profile_mask     = 0x0;
        	prog_selection->packet_format_qualifier_2      = (0x7);
        	prog_selection->packet_format_qualifier_2_mask = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_R1_V | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L2 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L3 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_NEXT_PROT;
            if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 20) {
            	prog_selection->isb_lookup_found               = (0x1);
            	prog_selection->isb_lookup_found_mask          = (0x0);
            }
            if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 21) {
            	prog_selection->isa_lookup_found               = (0x1);
                prog_selection->isa_lookup_found_mask          = (0x0);
            }
			break;

        case VTT_3MTSE_PLUS_FRR_M3_MISS_SEM:
        	prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS3_ETH; 
        	prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP; 
            prog_selection->in_pp_port_tt_profile          =  (SOC_PPC_PORT_DEFINED_TT_PROFILE_DEFAULT);
            prog_selection->in_pp_port_tt_profile_mask     = (0x0);
            prog_selection->vt_processing_profile          = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_3MTSE_PLUS_FRR_TCAM;
        	prog_selection->vt_processing_profile_mask     = 0x0;
        	prog_selection->packet_format_qualifier_2      = (0x7);
        	prog_selection->packet_format_qualifier_2_mask = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_R1_V | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L2 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L3 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_NEXT_PROT;
            if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 20) {
            	prog_selection->isb_lookup_found               = (0x0);
            	prog_selection->isb_lookup_found_mask          = (0x0);
            }
            if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 21) {
            	prog_selection->isa_lookup_found               = (0x0);
                prog_selection->isa_lookup_found_mask          = (0x0);
            }
            break;

        case VTT_3MTSE_PLUS_FRR_M3_PLUS_HIT_SEM_AND_TCAM:
        	prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS3_ETH;
        	prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP; 
            prog_selection->in_pp_port_tt_profile          =  (SOC_PPC_PORT_DEFINED_TT_PROFILE_DEFAULT);
            prog_selection->in_pp_port_tt_profile_mask     = (0x0);
            prog_selection->vt_processing_profile          = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_3MTSE_PLUS_FRR_L4B_AND_L5G;
        	prog_selection->vt_processing_profile_mask     = 0x0;
			prog_selection->packet_format_qualifier_2      = (0x0);
        	prog_selection->packet_format_qualifier_2_mask = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L1 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L2 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L3 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_NEXT_PROT;
			prog_selection->tcam_lookup_match               = (0x1);
        	prog_selection->tcam_lookup_match_mask          = (0x0);
            if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 20) {
            	prog_selection->isb_lookup_found               = (0x1);
            	prog_selection->isb_lookup_found_mask          = (0x0);
            }
            if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 21) {
            	prog_selection->isa_lookup_found               = (0x1);
                prog_selection->isa_lookup_found_mask          = (0x0);
            }
            break;

        case VTT_3MTSE_PLUS_FRR_M3_PLUS_HIT_TCAM:
        	prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS3_ETH;
        	prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP; 
            prog_selection->in_pp_port_tt_profile          =  (SOC_PPC_PORT_DEFINED_TT_PROFILE_DEFAULT);
            prog_selection->in_pp_port_tt_profile_mask     = (0x0);
            prog_selection->vt_processing_profile          = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_3MTSE_PLUS_FRR_L4B_AND_L5G;
        	prog_selection->vt_processing_profile_mask     = 0x0;
        	prog_selection->packet_format_qualifier_2      = (0x0);
        	prog_selection->packet_format_qualifier_2_mask = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L1 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L2 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L3 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_NEXT_PROT;
			prog_selection->tcam_lookup_match              = (0x1);
        	prog_selection->tcam_lookup_match_mask         = (0x0);
            break;

        case VTT_3MTSE_PLUS_FRR_M3_PLUS_MISS_TCAM:
        	prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS3_ETH; 
        	prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP; 
            prog_selection->in_pp_port_tt_profile          =  (SOC_PPC_PORT_DEFINED_TT_PROFILE_DEFAULT);
            prog_selection->in_pp_port_tt_profile_mask     = (0x0);
            prog_selection->vt_processing_profile          = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_3MTSE_PLUS_FRR_L4B_AND_L5G;
        	prog_selection->vt_processing_profile_mask     = 0x0;
        	prog_selection->tcam_lookup_match              = (0x0);
        	prog_selection->tcam_lookup_match_mask         = (0x0);
        	prog_selection->packet_format_qualifier_2      = (0x0);
        	prog_selection->packet_format_qualifier_2_mask = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L1 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L2 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L3 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_NEXT_PROT;
        	if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 20) {
        	    prog_selection->isb_lookup_found               = (0x1);
        	    prog_selection->isb_lookup_found_mask          = (0x0);
        	}
        	if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 21) {
        	    prog_selection->isa_lookup_found               = (0x1);
        	    prog_selection->isa_lookup_found_mask          = (0x0);
        	}
            break;

        case VTT_3MTSE_PLUS_FRR_M3_PLUS_MISS_SEM_AND_TCAM:
        	prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS3_ETH;
        	prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP; 
            prog_selection->in_pp_port_tt_profile          =  (SOC_PPC_PORT_DEFINED_TT_PROFILE_DEFAULT);
            prog_selection->in_pp_port_tt_profile_mask     = (0x0);
            prog_selection->vt_processing_profile          = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_3MTSE_PLUS_FRR_L4B_AND_L5G;
        	prog_selection->vt_processing_profile_mask     = 0x0;
        	prog_selection->tcam_lookup_match              = (0x0);
        	prog_selection->tcam_lookup_match_mask         = (0x0);
			prog_selection->packet_format_qualifier_2      = (0x0);
        	prog_selection->packet_format_qualifier_2_mask = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L1 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L2 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L3 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_NEXT_PROT;
            if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 20) {
            	prog_selection->isb_lookup_found               = (0x0);
            	prog_selection->isb_lookup_found_mask          = (0x0);
            }
            if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 21) {
            	prog_selection->isa_lookup_found               = (0x0);
                prog_selection->isa_lookup_found_mask          = (0x0);
            }
            break;

        case VTT_3MTSE_PLUS_FRR_M3_PLUS_ELI_GAL:
        	prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS3_ETH;
        	prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP; 
            prog_selection->in_pp_port_tt_profile          =  (SOC_PPC_PORT_DEFINED_TT_PROFILE_DEFAULT);
            prog_selection->in_pp_port_tt_profile_mask     = (0x0);
            prog_selection->vt_processing_profile          = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_3MTSE_PLUS_FRR_TCAM;
        	prog_selection->vt_processing_profile_mask     = 0x0;
        	prog_selection->packet_format_qualifier_2      = (0x0);
        	prog_selection->packet_format_qualifier_2_mask = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L1 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L3 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_NEXT_PROT;
        	prog_selection->packet_format_qualifier_3      = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_GAL;
        	prog_selection->packet_format_qualifier_3_mask = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L2 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L3 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_BOS_NEXT_PROT;
            break;

        case VTT_3MTSE_PLUS_FRR_M3_PLUS_ELI_HIT_SEM:
        	prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS3_ETH;
        	prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP; 
            prog_selection->in_pp_port_tt_profile          =  (SOC_PPC_PORT_DEFINED_TT_PROFILE_DEFAULT);
            prog_selection->in_pp_port_tt_profile_mask     = (0x0);
            prog_selection->vt_processing_profile          = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_3MTSE_PLUS_FRR_TCAM;
        	prog_selection->vt_processing_profile_mask     = 0x0;
        	prog_selection->packet_format_qualifier_2      = (0x0);
        	prog_selection->packet_format_qualifier_2_mask = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L1 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L3 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_NEXT_PROT;
            if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 20) {
            	prog_selection->isb_lookup_found               = (0x1);
            	prog_selection->isb_lookup_found_mask          = (0x0);
            }
            if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 21) {
            	prog_selection->isa_lookup_found               = (0x1);
                prog_selection->isa_lookup_found_mask          = (0x0);
            }
            break;

        case VTT_3MTSE_PLUS_FRR_M3_PLUS_ELI_MISS_SEM:
        	prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS3_ETH;
        	prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP; 
            prog_selection->in_pp_port_tt_profile          =  (SOC_PPC_PORT_DEFINED_TT_PROFILE_DEFAULT);
            prog_selection->in_pp_port_tt_profile_mask     = (0x0);
            prog_selection->vt_processing_profile          = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_3MTSE_PLUS_FRR_TCAM;
        	prog_selection->vt_processing_profile_mask     = 0x0;
        	prog_selection->packet_format_qualifier_2      = (0x0);
        	prog_selection->packet_format_qualifier_2_mask = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L1 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L3 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_NEXT_PROT;
            if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 20) {
            	prog_selection->isb_lookup_found               = (0x0);
            	prog_selection->isb_lookup_found_mask          = (0x0);
            }
            if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 21) {
            	prog_selection->isa_lookup_found               = (0x0);
                prog_selection->isa_lookup_found_mask          = (0x0);
            }
            break;

        case VTT_3MTSE_PLUS_FRR_M3_PLUS_ELI_GAL_HIT_SEM_AND_TCAM:
         	prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS3_ETH;
         	prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP; 
            prog_selection->in_pp_port_tt_profile          =  (SOC_PPC_PORT_DEFINED_TT_PROFILE_DEFAULT);
            prog_selection->in_pp_port_tt_profile_mask     = (0x0);
            prog_selection->vt_processing_profile          = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_3MTSE_PLUS_FRR_TCAM;
         	prog_selection->vt_processing_profile_mask     = 0x0;
         	prog_selection->packet_format_qualifier_2      = (0x0);
         	prog_selection->packet_format_qualifier_2_mask = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L1 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L2 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_NEXT_PROT;
         	prog_selection->tcam_lookup_match              = (0x1);
         	prog_selection->tcam_lookup_match_mask         = (0x0);
        	prog_selection->packet_format_qualifier_3      = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_L2GAL;
        	prog_selection->packet_format_qualifier_3_mask = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L1 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L2 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_BOS_NEXT_PROT;
        	if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 20) {
            	prog_selection->isb_lookup_found               = (0x1);
            	prog_selection->isb_lookup_found_mask          = (0x0);
            }
            if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 21) {
            	prog_selection->isa_lookup_found               = (0x1);
                prog_selection->isa_lookup_found_mask          = (0x0);
            }
			break;

        case VTT_3MTSE_PLUS_FRR_M3_PLUS_ELI_GAL_DC_SEM_AND_TCAM:
         	prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS3_ETH;
         	prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP; 
            prog_selection->in_pp_port_tt_profile          =  (SOC_PPC_PORT_DEFINED_TT_PROFILE_DEFAULT);
            prog_selection->in_pp_port_tt_profile_mask     = (0x0);
            prog_selection->vt_processing_profile          = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_3MTSE_PLUS_FRR_TCAM;
         	prog_selection->vt_processing_profile_mask     = 0x0;
         	prog_selection->packet_format_qualifier_2      = (0x0);
         	prog_selection->packet_format_qualifier_2_mask = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L1 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L2 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_NEXT_PROT;
         	prog_selection->packet_format_qualifier_3      = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_L2GAL;
        	prog_selection->packet_format_qualifier_3_mask = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L1 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L2 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_BOS_NEXT_PROT;
        	break;

        case VTT_3MTSE_PLUS_FRR_M3_PLUS_NO_RANGE_HIT_TCAM:
         	prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS3_ETH;
         	prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP; 
            prog_selection->in_pp_port_tt_profile          =  (SOC_PPC_PORT_DEFINED_TT_PROFILE_DEFAULT);
            prog_selection->in_pp_port_tt_profile_mask     = (0x0);
            prog_selection->vt_processing_profile          = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_3MTSE_PLUS_FRR_TCAM;
         	prog_selection->vt_processing_profile_mask     = 0x0;
         	prog_selection->packet_format_qualifier_2      = (0x7E);
         	prog_selection->packet_format_qualifier_2_mask = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_R1_V | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_R2_V | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_R3_V | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_NEXT_PROT;
         	prog_selection->tcam_lookup_match              = (0x1);
         	prog_selection->tcam_lookup_match_mask         = (0x0);
        	prog_selection->packet_format_qualifier_3      = (0x6);
        	prog_selection->packet_format_qualifier_3_mask = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_R1_W | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L2 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L3 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_BOS_NEXT_PROT;
        	break;

        case VTT_3MTSE_PLUS_FRR_M3_PLUS_NO_RANGE_MISS_TCAM:
         	prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS3_ETH;
         	prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP; 
            prog_selection->in_pp_port_tt_profile          =  (SOC_PPC_PORT_DEFINED_TT_PROFILE_DEFAULT);
            prog_selection->in_pp_port_tt_profile_mask     = (0x0);
            prog_selection->vt_processing_profile          = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_3MTSE_PLUS_FRR_TCAM;
         	prog_selection->vt_processing_profile_mask     = 0x0;
         	prog_selection->packet_format_qualifier_2      = (0x7E);
         	prog_selection->packet_format_qualifier_2_mask = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_R1_V | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_R2_V | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_R3_V | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_NEXT_PROT;
         	prog_selection->tcam_lookup_match              = (0x0);
         	prog_selection->tcam_lookup_match_mask         = (0x0);
        	prog_selection->packet_format_qualifier_3      = (0x6);
        	prog_selection->packet_format_qualifier_3_mask = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_R1_W | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L2 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L3 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_BOS_NEXT_PROT;
        	break;

        case VTT_3MTSE_PLUS_FRR_M3_PLUS_DFLT_HIT_SEM_AND_TCAM:
         	prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS3_ETH;
         	prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP; 
            prog_selection->in_pp_port_tt_profile          =  (SOC_PPC_PORT_DEFINED_TT_PROFILE_DEFAULT);
            prog_selection->in_pp_port_tt_profile_mask     = (0x0);
            prog_selection->vt_processing_profile          = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_3MTSE_PLUS_FRR_TCAM;
         	prog_selection->vt_processing_profile_mask     = 0x0;
         	prog_selection->packet_format_qualifier_2      = (0x0);
         	prog_selection->packet_format_qualifier_2_mask = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L1 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L2 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L3 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_NEXT_PROT;
        	prog_selection->tcam_lookup_match              = (0x1);
         	prog_selection->tcam_lookup_match_mask         = (0x0);
        	if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 20) {
            	prog_selection->isb_lookup_found               = (0x1);
            	prog_selection->isb_lookup_found_mask          = (0x0);
            }
            if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 21) {
            	prog_selection->isa_lookup_found               = (0x1);
                prog_selection->isa_lookup_found_mask          = (0x0);
            }
			break;

        case VTT_3MTSE_PLUS_FRR_M3_PLUS_DFLT_DC_SEM_AND_TCAM:
         	prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS3_ETH;
         	prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP; 
            prog_selection->in_pp_port_tt_profile          =  (SOC_PPC_PORT_DEFINED_TT_PROFILE_DEFAULT);
            prog_selection->in_pp_port_tt_profile_mask     = (0x0);
            prog_selection->vt_processing_profile          = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_3MTSE_PLUS_FRR_TCAM;
         	prog_selection->vt_processing_profile_mask     = 0x0;
           	prog_selection->packet_format_qualifier_2      = (0x0);
         	prog_selection->packet_format_qualifier_2_mask = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L1 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L2 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L3 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_NEXT_PROT;
     		break;

        default:
        break;
    }
    prog_selection->ptc_tt_profile             = (ARAD_PORTS_TT_PROFILE_NONE);
    prog_selection->ptc_tt_profile_mask        = (0x0);
    prog_selection->my_mac                     = (0x1);
    prog_selection->my_mac_mask                = (0x0); 
    prog_selection->enable_routing             = (0x1);
    prog_selection->enable_routing_mask        = (0x0);
    return -1;

}

int32 arad_pp_dbal_tt_cam_2mtse_plus_frr_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 vtt_2mtse_plus_frr_type)
{

    switch(vtt_2mtse_plus_frr_type)
    {
        case VTT_2MTSE_PLUS_FRR_M1:
            prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS1_ETH;
            prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP; 
            prog_selection->vt_processing_profile          = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_2MTSE_PLUS_FRR_TCAM;
            prog_selection->vt_processing_profile_mask     = 0x0;
            prog_selection->in_pp_port_tt_profile          =  (SOC_PPC_PORT_DEFINED_TT_PROFILE_TERMINATION_2MTSE_PLUS_FRR);
            prog_selection->in_pp_port_tt_profile_mask     = (0x0);
            break;

        case VTT_2MTSE_PLUS_FRR_M3_PLUS_R1_ELI:
        	prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS3_ETH; 
        	prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP; 
            prog_selection->in_pp_port_tt_profile          =  (SOC_PPC_PORT_DEFINED_TT_PROFILE_TERMINATION_2MTSE_PLUS_FRR);
            prog_selection->in_pp_port_tt_profile_mask     = (0x0);
            prog_selection->vt_processing_profile          = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_2MTSE_PLUS_FRR_TCAM;
        	prog_selection->vt_processing_profile_mask     = 0x0;
        	prog_selection->packet_format_qualifier_2      = (0x0);
        	prog_selection->packet_format_qualifier_2_mask = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L2 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L3 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_BOS_NEXT_PROT;
            break;

        case VTT_2MTSE_PLUS_FRR_M2_HIT_TCAM:
        	prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS2_ETH; 
        	prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP;
            prog_selection->in_pp_port_tt_profile          =  (SOC_PPC_PORT_DEFINED_TT_PROFILE_TERMINATION_2MTSE_PLUS_FRR);
            prog_selection->in_pp_port_tt_profile_mask     = (0x0);
            prog_selection->vt_processing_profile          = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_2MTSE_PLUS_FRR_TCAM;
        	prog_selection->vt_processing_profile_mask     = 0x0;
        	prog_selection->packet_format_qualifier_2      = (0x1F);
        	prog_selection->packet_format_qualifier_2_mask = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_R2_W | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L3 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_NEXT_PROT;
        	prog_selection->tcam_lookup_match               = (0x1);
        	prog_selection->tcam_lookup_match_mask          = (0x0);
            break;

        case VTT_2MTSE_PLUS_FRR_M2_MISS_TCAM:
        	prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS2_ETH; 
        	prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP;
            prog_selection->in_pp_port_tt_profile          =  (SOC_PPC_PORT_DEFINED_TT_PROFILE_TERMINATION_2MTSE_PLUS_FRR);
            prog_selection->in_pp_port_tt_profile_mask     = (0x0);
            prog_selection->vt_processing_profile          = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_2MTSE_PLUS_FRR_TCAM;
        	prog_selection->vt_processing_profile_mask     = 0x0;
        	prog_selection->packet_format_qualifier_2      = (0x1F);
        	prog_selection->packet_format_qualifier_2_mask = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_R2_W | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L3 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_NEXT_PROT;
        	prog_selection->tcam_lookup_match               = (0x0);
        	prog_selection->tcam_lookup_match_mask          = (0x0);
            break;

        case VTT_2MTSE_PLUS_FRR_M3_PLUS_R3_GAL_HIT_TCAM:
        	prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS3_ETH;
        	prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP;
            prog_selection->in_pp_port_tt_profile          =  (SOC_PPC_PORT_DEFINED_TT_PROFILE_TERMINATION_2MTSE_PLUS_FRR);
            prog_selection->in_pp_port_tt_profile_mask     = (0x0);
            prog_selection->vt_processing_profile          = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_2MTSE_PLUS_FRR_TCAM;
        	prog_selection->vt_processing_profile_mask     = 0x0;
         	prog_selection->packet_format_qualifier_2      = (0x5F);
         	prog_selection->packet_format_qualifier_2_mask = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_NEXT_PROT;
        	prog_selection->tcam_lookup_match               = (0x1);
        	prog_selection->tcam_lookup_match_mask          = (0x0);
            break;

        case VTT_2MTSE_PLUS_FRR_M3_PLUS_R3_GAL_MISS_TCAM:
        	prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS3_ETH;
        	prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP;
            prog_selection->in_pp_port_tt_profile          =  (SOC_PPC_PORT_DEFINED_TT_PROFILE_TERMINATION_2MTSE_PLUS_FRR);
            prog_selection->in_pp_port_tt_profile_mask     = (0x0);
            prog_selection->vt_processing_profile          = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_2MTSE_PLUS_FRR_TCAM;
        	prog_selection->vt_processing_profile_mask     = 0x0;
         	prog_selection->packet_format_qualifier_2      = (0x5F);
         	prog_selection->packet_format_qualifier_2_mask = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_NEXT_PROT;
        	prog_selection->tcam_lookup_match               = (0x0);
        	prog_selection->tcam_lookup_match_mask          = (0x0);
            break;

        case VTT_2MTSE_PLUS_FRR_M3_R2_IML:
        	prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS3_ETH;
        	prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP;
            prog_selection->in_pp_port_tt_profile          = (SOC_PPC_PORT_DEFINED_TT_PROFILE_TERMINATION_2MTSE_PLUS_FRR);
            prog_selection->in_pp_port_tt_profile_mask     = (0x0);
            prog_selection->vt_processing_profile          = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_2MTSE_PLUS_FRR_TCAM;
        	prog_selection->vt_processing_profile_mask     = 0x0;
         	prog_selection->packet_format_qualifier_2      = (0x09);
         	prog_selection->packet_format_qualifier_2_mask = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L1 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L3 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_NEXT_PROT;
            break;

        case VTT_2MTSE_PLUS_FRR_M2:
        	prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS2_ETH; 
        	prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP; 
            prog_selection->in_pp_port_tt_profile          =  (SOC_PPC_PORT_DEFINED_TT_PROFILE_TERMINATION_2MTSE_PLUS_FRR);
            prog_selection->in_pp_port_tt_profile_mask     = (0x0);
            prog_selection->vt_processing_profile          = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_2MTSE_PLUS_FRR_TCAM;
        	prog_selection->vt_processing_profile_mask     = 0x0;
            break;

        case VTT_2MTSE_PLUS_FRR_M3_ELI:
        	prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS3_ETH; 
        	prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP; 
            prog_selection->in_pp_port_tt_profile          =  (SOC_PPC_PORT_DEFINED_TT_PROFILE_TERMINATION_2MTSE_PLUS_FRR);
            prog_selection->in_pp_port_tt_profile_mask     = (0x0);
            prog_selection->vt_processing_profile          = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_2MTSE_PLUS_FRR_TCAM;
        	prog_selection->vt_processing_profile_mask     = 0x0;
        	prog_selection->packet_format_qualifier_2      = (0x1);
        	prog_selection->packet_format_qualifier_2_mask = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L1 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L3 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_NEXT_PROT;
            break;

        case VTT_2MTSE_PLUS_FRR_M3_PLUS_ELI_GAL:
         	prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS3_ETH; 
         	prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP; 
            prog_selection->in_pp_port_tt_profile          =  (SOC_PPC_PORT_DEFINED_TT_PROFILE_TERMINATION_2MTSE_PLUS_FRR);
            prog_selection->in_pp_port_tt_profile_mask     = (0x0);
            prog_selection->vt_processing_profile          = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_2MTSE_PLUS_FRR_TCAM;
         	prog_selection->vt_processing_profile_mask     = 0x0;
         	prog_selection->packet_format_qualifier_2      = (0x0);
         	prog_selection->packet_format_qualifier_2_mask = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L1 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L3 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_NEXT_PROT;
         	prog_selection->packet_format_qualifier_3      = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_GAL;
         	prog_selection->packet_format_qualifier_3_mask = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L2 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L3 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_BOS_NEXT_PROT;
            break;

        case VTT_2MTSE_PLUS_FRR_M3_PLUS_ELI_HIT_TCAM:
         	prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS3_ETH; 
         	prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP; 
            prog_selection->in_pp_port_tt_profile          =  (SOC_PPC_PORT_DEFINED_TT_PROFILE_TERMINATION_2MTSE_PLUS_FRR);
            prog_selection->in_pp_port_tt_profile_mask     = (0x0);
            prog_selection->vt_processing_profile          = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_2MTSE_PLUS_FRR_TCAM;
         	prog_selection->vt_processing_profile_mask     = 0x0;
         	prog_selection->packet_format_qualifier_2      = (0x0);
         	prog_selection->packet_format_qualifier_2_mask = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L1 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L2 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_NEXT_PROT;
         	prog_selection->tcam_lookup_match              = (0x1);
         	prog_selection->tcam_lookup_match_mask         = (0x0);
            break;

        case VTT_2MTSE_PLUS_FRR_M3_PLUS_ELI_MISS_TCAM:
         	prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS3_ETH; 
         	prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP; 
            prog_selection->in_pp_port_tt_profile          =  (SOC_PPC_PORT_DEFINED_TT_PROFILE_TERMINATION_2MTSE_PLUS_FRR);
            prog_selection->in_pp_port_tt_profile_mask     = (0x0);
            prog_selection->vt_processing_profile          = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_2MTSE_PLUS_FRR_TCAM;
         	prog_selection->vt_processing_profile_mask     = 0x0;
         	prog_selection->packet_format_qualifier_2      = (0x0);
         	prog_selection->packet_format_qualifier_2_mask = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L1 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L2 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_NEXT_PROT;
         	prog_selection->tcam_lookup_match              = (0x0);
         	prog_selection->tcam_lookup_match_mask         = (0x0);
            break;

        case VTT_2MTSE_PLUS_FRR_M3_PLUS_R2_ELI:
        	prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS3_ETH; 
        	prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP; 
            prog_selection->in_pp_port_tt_profile          =  (SOC_PPC_PORT_DEFINED_TT_PROFILE_TERMINATION_2MTSE_PLUS_FRR);
            prog_selection->in_pp_port_tt_profile_mask     = (0x0);
            prog_selection->vt_processing_profile          = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_2MTSE_PLUS_FRR_TCAM;
        	prog_selection->vt_processing_profile_mask     = 0x0;
        	prog_selection->packet_format_qualifier_2      = (0x0);
        	prog_selection->packet_format_qualifier_2_mask = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L1 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L3 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_NEXT_PROT;
            break;

        case VTT_2MTSE_PLUS_FRR_M3_PLUS_NO_RANGE_HIT_TCAM:
         	prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS3_ETH; 
         	prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP; 
            prog_selection->in_pp_port_tt_profile          =  (SOC_PPC_PORT_DEFINED_TT_PROFILE_TERMINATION_2MTSE_PLUS_FRR);
            prog_selection->in_pp_port_tt_profile_mask     = (0x0);
            prog_selection->vt_processing_profile          = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_2MTSE_PLUS_FRR_TCAM;
         	prog_selection->vt_processing_profile_mask     = 0x0;
         	prog_selection->packet_format_qualifier_2      = (0x1E);
         	prog_selection->packet_format_qualifier_2_mask = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L3 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_BOS_NEXT_PROT;
         	prog_selection->tcam_lookup_match              = (0x1);
         	prog_selection->tcam_lookup_match_mask         = (0x0);
        	break;

        case VTT_2MTSE_PLUS_FRR_M3_PLUS_NO_RANGE_MISS_TCAM:
         	prog_selection->packet_format_code             = ARAD_PARSER_PFC_MPLS3_ETH; 
         	prog_selection->packet_format_code_mask        = ARAD_PARSER_PFC_MATCH_ANY_ENCAP; 
            prog_selection->in_pp_port_tt_profile          =  (SOC_PPC_PORT_DEFINED_TT_PROFILE_TERMINATION_2MTSE_PLUS_FRR);
            prog_selection->in_pp_port_tt_profile_mask     = (0x0);
            prog_selection->vt_processing_profile          = ARAD_PP_IHP_VTT_VT_PROCESSING_PROFILE_2MTSE_PLUS_FRR_TCAM;
         	prog_selection->vt_processing_profile_mask     = 0x0;
         	prog_selection->packet_format_qualifier_2      = (0x1E);
         	prog_selection->packet_format_qualifier_2_mask = ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_L3 | ARAD_PP_ISEM_ACCESS_QLFR_MPLS_MASK_BOS_NEXT_PROT;
         	prog_selection->tcam_lookup_match              = (0x0);
         	prog_selection->tcam_lookup_match_mask         = (0x0);
        	break;
        default:
        break;
    }

    prog_selection->ptc_tt_profile             = (ARAD_PORTS_TT_PROFILE_NONE);
    prog_selection->ptc_tt_profile_mask        = (0x0);
    prog_selection->my_mac                     = (0x1);
    prog_selection->my_mac_mask                = (0x0);
    prog_selection->enable_routing             = (0x1);
    prog_selection->enable_routing_mask        = (0x0);
    return -1;

}

int32 arad_pp_dbal_tt_cam_generalized_rch_ip_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused)
{
    prog_selection->in_pp_port_tt_profile              = SOC_PPC_PORT_DEFINED_TT_PROFILE_GENERALIZED_RCH;
    prog_selection->in_pp_port_tt_profile_mask         = 0;
    prog_selection->packet_format_code              = ARAD_PARSER_PFC_IPV4_ETH;
    prog_selection->packet_format_code_mask         = ARAD_PARSER_PFC_MATCH_IPVx;
    prog_selection->my_mac                          = (0x0);
    prog_selection->my_mac_mask                     = (0x1);
    prog_selection->enable_routing          = 0x1;
    prog_selection->enable_routing_mask     = 0x0;

    return -1;
}

int32 arad_pp_dbal_tt_cam_generalized_rch_mpls_set(int unit, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA   *prog_selection, int next_line, uint32 unused)
{
    prog_selection->in_pp_port_tt_profile              = SOC_PPC_PORT_DEFINED_TT_PROFILE_GENERALIZED_RCH;
    prog_selection->in_pp_port_tt_profile_mask         = 0;
    prog_selection->packet_format_code              = ARAD_PARSER_PFC_MPLS4P_ETH;
    prog_selection->packet_format_code_mask         = ARAD_PARSER_PFC_MATCH_ANY_MPLS;
    prog_selection->my_mac                          = (0x0);
    prog_selection->my_mac_mask                     = (0x1);
    prog_selection->enable_routing          = 0x1;
    prog_selection->enable_routing_mask     = 0x0;

    return -1;
}









uint32 arad_pp_dbal_vtt_sw_db_get(uint32 unit, ARAD_PP_ISEM_ACCESS_KEY   *isem_key,
                                               uint32                    *nof_tables,
                                               SOC_DPP_DBAL_SW_TABLE_IDS  dbal_tables_id[3],
                                               uint8 *is_duplicate)
{
    int i;
    uint8  duplicate = 0;
    uint32 res;
    uint32 tables_mem[ARAD_PP_ISEM_ACCESS_NOF_TABLES];

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    *nof_tables = 0;

    switch(isem_key->key_type)
    {
    case ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD_VID:
        dbal_tables_id[0] = SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_A;
        dbal_tables_id[1] = SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_A;
        break;

    case ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD_INITIAL_VID:
        dbal_tables_id[0] = SOC_DPP_DBAL_SW_TABLE_ID_VDxINITIALVID_SEM_A;
        dbal_tables_id[1] = SOC_DPP_DBAL_SW_TABLE_ID_VDxINITIALVID_SEM_A;
        break;

    case ARAD_PP_ISEM_ACCESS_KEY_TYPE_VD_VID_VID:
        dbal_tables_id[0] = SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTER_INNER_VID_SEM_A;
        dbal_tables_id[1] = SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTER_INNER_VID_SEM_A;
        break;
    case SOC_PPC_L2_LIF_AC_MAP_KEY_TYPE_TEST2:
        dbal_tables_id[0] = SOC_DPP_DBAL_SW_TABLE_ID_TEST2_SEM_A;
        dbal_tables_id[1] = SOC_DPP_DBAL_SW_TABLE_ID_TEST2_SEM_A;
        *nof_tables=1;
        break;
    case ARAD_PP_ISEM_ACCESS_KEY_TYPE_EXTENDER_UNTAG_CHECK:
        dbal_tables_id[0] = SOC_DPP_DBAL_SW_TABLE_ID_EXTENDER_UNTAG_CHECK_SEM_A;
        dbal_tables_id[1] = SOC_DPP_DBAL_SW_TABLE_ID_EXTENDER_UNTAG_CHECK_SEM_A;
        break;

    case ARAD_PP_ISEM_ACCESS_KEY_TYPE_EXTENDER_PE:
        dbal_tables_id[0] = SOC_DPP_DBAL_SW_TABLE_ID_EXTENDER_PE_SEM_A;
        dbal_tables_id[1] = SOC_DPP_DBAL_SW_TABLE_ID_EXTENDER_PE_UT_SEM_A;
        break;

    case ARAD_PP_ISEM_ACCESS_KEY_TYPE_CUSTOM_PP_PORT_TUNNEL:
        dbal_tables_id[0] = SOC_DPP_DBAL_SW_TABLE_ID_CUSTOM_PP_PORT_TUNNEL_SEMA;
        dbal_tables_id[1] = SOC_DPP_DBAL_SW_TABLE_ID_CUSTOM_PP_PORT_TUNNEL_SEMA;
        break;

    case ARAD_PP_ISEM_ACCESS_KEY_TYPE_CUSTOM_PP_LSP_ECMP:
        dbal_tables_id[0] = SOC_DPP_DBAL_SW_TABLE_ID_CUSTOM_PP_PORT_LSP_ECMP_TCAM;
        dbal_tables_id[1] = SOC_DPP_DBAL_SW_TABLE_ID_CUSTOM_PP_PORT_LSP_ECMP_TCAM;
        break;
    case ARAD_PP_ISEM_ACCESS_KEY_TYPE_SECTION_OAM_PORT_ETH_VID:
        dbal_tables_id[0] = SOC_DPP_DBAL_SW_TABLE_ID_SECTION_OAM_VDxOTUER_VIDxETH_TCAM;
        break;
    case ARAD_PP_ISEM_ACCESS_KEY_TYPE_SECTION_OAM_PORT_ETH_VID_VID:
        dbal_tables_id[0] = SOC_DPP_DBAL_SW_TABLE_ID_SECTION_OAM_VDxOTUER_VIDxINNER_VIDxETH_TCAM;
        break;
	case ARAD_PP_ISEM_ACCESS_KEY_TYPE_L2TP_TUNNEL_SESSION:
		dbal_tables_id[0] = SOC_DPP_DBAL_SW_TABLE_ID_L2TP_ISEM_A;
		break;
	case ARAD_PP_ISEM_ACCESS_KEY_TYPE_PPPOE_SESSION:
		dbal_tables_id[0] = SOC_DPP_DBAL_SW_TABLE_ID_PPPOE_ISEM_A;
		break;
    default:
        dbal_tables_id[0]  = SOC_DPP_DBAL_SW_TABLE_ID_INVALID;
        break;
    }


    if (dbal_tables_id[0] != SOC_DPP_DBAL_SW_TABLE_ID_INVALID) {
        if ((dbal_tables_id[0] == SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTERVID_SEM_A)
            || (dbal_tables_id[0] == SOC_DPP_DBAL_SW_TABLE_ID_VDxINITIALVID_SEM_A)
            || (dbal_tables_id[0] == SOC_DPP_DBAL_SW_TABLE_ID_VDxOUTER_INNER_VID_SEM_A)) {
            res = arad_pp_isem_access_sem_tables_get(unit, isem_key, nof_tables, tables_mem, &duplicate);
            SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
            for (i = 0; i < *nof_tables; i++) {
                if (tables_mem[i] != ARAD_PP_ISEM_ACCESS_ID_TCAM) dbal_tables_id[i] += tables_mem[i];
                if (!(vtt_tables[SW_DB_DBAL2VTT(dbal_tables_id[i])].valid_indication[unit])) {
                    *nof_tables = 0;
                }
            }
        } else {
            *nof_tables = 1;
        }
    }

   *is_duplicate = duplicate;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_dbal_vtt_table_id_get()", 0, 0);

}


#define ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY_SWAP_FIELD(field_1,field_2) \
{ \
  uint32 __tmp; \
  __tmp = field_1; \
  field_1 = field_2; \
  field_2 = __tmp; \
}

#define ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY_SWAP_BITS(new,old,value_1,value_2) \
{ \
  if (old & value_1) \
  { \
    new |= value_2; \
    new &= ~value_1; \
  } \
  if (old & value_2) \
  { \
    new |= value_1; \
    new &= ~value_2; \
  } \
}

void arad_pp_dbal_vtt_program_property_swap(ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY  *program_property )
{
    uint32 tmp;

    ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY_SWAP_FIELD(program_property->isa_key_initial_from_vt, program_property->isb_key_initial_from_vt);

    tmp = program_property->pd_bitmap_0;
    ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY_SWAP_BITS(program_property->pd_bitmap_0,tmp,0x2,0x1);
    ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY_SWAP_BITS(program_property->pd_bitmap_0,tmp,0x10,0x8);
    tmp = program_property->pd_bitmap_1;
    ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY_SWAP_BITS(program_property->pd_bitmap_1,tmp,0x2,0x1);
    ARAD_PP_DBAL_VTT_PROGRAM_PROPERTY_SWAP_BITS(program_property->pd_bitmap_1,tmp,0x10,0x8);

}

void arad_pp_dbal_vtt_find_namespace_database(int unit, SOC_PPC_MPLS_TERM_NAMESPACE_TYPE namespace_required,
                                                        SOC_PPC_MPLS_TERM_NAMESPACE_TYPE *namespace,
                                                        uint32 *database)
{

  *namespace = g_prog_soc_prop[unit].mpls_3_namespace;
  *database  = g_prog_soc_prop[unit].mpls_3_database;

  if (namespace_required == SOC_PPC_MPLS_TERM_NAMESPACE_L1)
  {
    if (ARAD_PP_VTT_FIND_NAMESPACE_L1(g_prog_soc_prop[unit].mpls_1_namespace)) {
      *namespace = g_prog_soc_prop[unit].mpls_1_namespace;
      *database  = g_prog_soc_prop[unit].mpls_1_database;
    } else if (ARAD_PP_VTT_FIND_NAMESPACE_L1(g_prog_soc_prop[unit].mpls_2_namespace)) {
      *namespace = g_prog_soc_prop[unit].mpls_2_namespace;
      *database  = g_prog_soc_prop[unit].mpls_2_database;
    } 
  }

  if (namespace_required == SOC_PPC_MPLS_TERM_NAMESPACE_L2)
  {
    if (ARAD_PP_VTT_FIND_NAMESPACE_L2(g_prog_soc_prop[unit].mpls_1_namespace))
    {
      *namespace = g_prog_soc_prop[unit].mpls_1_namespace;
      *database  = g_prog_soc_prop[unit].mpls_1_database;
    }
    else if (ARAD_PP_VTT_FIND_NAMESPACE_L2(g_prog_soc_prop[unit].mpls_2_namespace))
    {
      *namespace = g_prog_soc_prop[unit].mpls_2_namespace;
      *database  = g_prog_soc_prop[unit].mpls_2_database;
    } 
  }

  if (namespace_required == SOC_PPC_MPLS_TERM_NAMESPACE_L3)
  {
    if (ARAD_PP_VTT_FIND_NAMESPACE_L3(g_prog_soc_prop[unit].mpls_1_namespace))
    {
      *namespace = g_prog_soc_prop[unit].mpls_1_namespace;
      *database  = g_prog_soc_prop[unit].mpls_1_database;
    }
    else if (ARAD_PP_VTT_FIND_NAMESPACE_L3(g_prog_soc_prop[unit].mpls_2_namespace))
    {
      *namespace = g_prog_soc_prop[unit].mpls_2_namespace;
      *database  = g_prog_soc_prop[unit].mpls_2_database;
    } 
  }
}

