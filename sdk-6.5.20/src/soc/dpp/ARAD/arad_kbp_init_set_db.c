/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0) && defined(INCLUDE_KBP)

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_TCAM



#include <soc/dpp/SAND/Utils/sand_header.h>
#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/mem.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/drv.h>

#include <soc/dpp/ARAD/arad_kbp.h>
#include <soc/dpp/ARAD/arad_kbp_rop.h>
#include <soc/dpp/ARAD/arad_kbp_xpt.h>
#include <soc/dpp/ARAD/arad_kbp_recover.h>

#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_api_nif.h>
#include <soc/dpp/ARAD/arad_api_ports.h>

#include <soc/dpp/ARAD/arad_chip_regs.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_flp_init.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_ce_instruction.h>


#include <soc/dpp/port_sw_db.h>

#include <soc/dpp/SAND/Management/sand_low_level.h>

#include <soc/scache.h>

#ifdef CRASH_RECOVERY_SUPPORT
#include <soc/ha.h>
#endif




#ifdef CRASH_RECOVERY_SUPPORT
typedef enum {
    HA_KBP_SUB_ID_0 = 0
} HA_KBP_sub_id_tl;
uint8 *kbp_nv_mem_ptr;
#endif

ARAD_KBP_DEV_HANDLERS dev_handlers_info;
ARAD_KBP_DB_TEMP_HANDLERS db_temp_handles_info;









extern arad_kbp_frwd_ltr_db_t arad_kbp_frwd_ltr_db[ARAD_KBP_ROP_LTR_NUM_MAX];
extern uint32 arad_kbp_blk_lut_set(int unit, uint32 core);

static
    ARAD_KBP_TABLE_CONFIG
        Arad_kbp_dummy_table_config_info_static =  {
                FALSE, 
                ARAD_KBP_FRWRD_TBL_ID_DUMMY_0, 
                KBP_DB_INVALID,
                0, 
                NLM_TBL_WIDTH_80,   
                NLM_TBL_ADLEN_32B,  
                0,  
                20, 
                NLMDEV_BANK_0, 
                0, 
                { 
                    0,
                    {{{0,},0,0},}
                },
                ARAD_KBP_FRWRD_IP_NOF_TABLES 
        };

static int Arad_kbp_default_result_sizes_static[ARAD_KBP_MAX_NUM_OF_PARALLEL_SEARCHES] = {
    ARAD_KBP_LUT_AD_TRANSFER_6B,
    ARAD_KBP_LUT_AD_TRANSFER_2B,
    ARAD_KBP_LUT_AD_TRANSFER_3B,
    ARAD_KBP_LUT_AD_TRANSFER_4B
};

static int Arad_kbp_default_result_formats_static[ARAD_KBP_MAX_NUM_OF_PARALLEL_SEARCHES] = {
    NLM_ARAD_INDEX_AND_64B_AD,
    NLM_ARAD_INDEX_AND_32B_AD,
    NLM_ARAD_INDEX_AND_32B_AD,
    NLM_ARAD_INDEX_AND_32B_AD
};

static
    ARAD_KBP_TABLE_CONFIG
        Arad_kbp_table_config_info_static[ARAD_KBP_FRWRD_IP_NOF_TABLES_ARAD] = {

             
            {   
                FALSE, 
                ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0, 
                KBP_DB_INVALID,
                0, 
                NLM_TBL_WIDTH_80,   
                NLM_TBL_ADLEN_48B,  
                0,  
                20, 
                NLMDEV_BANK_0, 
                32, 
                { 
                    2,
                    {{"VRF", 2, KBP_KEY_FIELD_EM},{"DIP", 4, KBP_KEY_FIELD_PREFIX}}
                },
                ARAD_KBP_FRWRD_IP_NOF_TABLES 
            },

            
            {   
                FALSE, 
                ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_1, 
                KBP_DB_INVALID,
                0, 
                NLM_TBL_WIDTH_80,   
                NLM_TBL_ADLEN_24B,  
                0,  
                20, 
                NLMDEV_BANK_0, 
                32, 
                {
                    2,
                    {{"VRF", 2, KBP_KEY_FIELD_EM},{"SIP", 4, KBP_KEY_FIELD_PREFIX}}
                },
                ARAD_KBP_FRWRD_IP_NOF_TABLES 
            },

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TBL_ID_IPV4_MC,  
                KBP_DB_INVALID,
                0, 
                NLM_TBL_WIDTH_160,   
                NLM_TBL_ADLEN_48B,  
                0,  
                20, 
                NLMDEV_BANK_0, 
                32, 
                {
                    4,
                    {{"VRF", 2, KBP_KEY_FIELD_EM},{"DIP", 4, KBP_KEY_FIELD_PREFIX},{"SIP", 4, KBP_KEY_FIELD_PREFIX},{"IN-RIF", 2, KBP_KEY_FIELD_PREFIX}}
                },
                ARAD_KBP_FRWRD_IP_NOF_TABLES 
            },

             
            {
                FALSE, 
                ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_0,  
                KBP_DB_INVALID,
                0, 
                NLM_TBL_WIDTH_160,   
                NLM_TBL_ADLEN_48B,  
                0,  
                20, 
                NLMDEV_BANK_0, 
                128, 
                {
                    2,
                    {{"VRF", 2, KBP_KEY_FIELD_EM},{"DIP", 16, KBP_KEY_FIELD_PREFIX}}
                },
                ARAD_KBP_FRWRD_IP_NOF_TABLES 
            },

             
            {
                FALSE, 
                ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_1,  
                KBP_DB_INVALID,
                0, 
                NLM_TBL_WIDTH_160,   
                NLM_TBL_ADLEN_24B,  
                0,  
                20, 
                NLMDEV_BANK_0, 
                128, 
                {
                    2,
                    {{"VRF", 2, KBP_KEY_FIELD_EM},{"SIP", 16, KBP_KEY_FIELD_PREFIX}}
                },
                ARAD_KBP_FRWRD_IP_NOF_TABLES 
            },

             
            {
                FALSE, 
                ARAD_KBP_FRWRD_TBL_ID_IPV6_MC,  
                KBP_DB_INVALID,
                0, 
                NLM_TBL_WIDTH_320,   
                NLM_TBL_ADLEN_48B,  
                0,  
                20, 
                NLMDEV_BANK_0, 
                0, 
                {
                    4,
                    {{"VRF", 2, KBP_KEY_FIELD_EM},{"DIP", 16, KBP_KEY_FIELD_PREFIX},{"SIP", 16, KBP_KEY_FIELD_PREFIX},{"IN-RIF", 2, KBP_KEY_FIELD_PREFIX}}
                },
                ARAD_KBP_FRWRD_IP_NOF_TABLES 
            },

             
            {
                FALSE, 
                ARAD_KBP_FRWRD_TBL_ID_LSR,  
                KBP_DB_INVALID,
                0, 
                NLM_TBL_WIDTH_80,   
                NLM_TBL_ADLEN_48B,  
                0,  
                20, 
                NLMDEV_BANK_0, 
                0, 
                {
                    1,
                    {{"MPLS-KEY", 6, KBP_KEY_FIELD_PREFIX}}
                },
                ARAD_KBP_FRWRD_IP_NOF_TABLES 
            },

             
            {   
                FALSE, 
                ARAD_KBP_FRWRD_TBL_ID_TRILL_UC,  
                KBP_DB_INVALID,
                0, 
                NLM_TBL_WIDTH_80,   
                NLM_TBL_ADLEN_48B,  
                0,  
                20, 
                NLMDEV_BANK_0, 
                0, 
                {
                    1,
                    {{"EGRESS-NICK", 2, KBP_KEY_FIELD_PREFIX}}
                },
                ARAD_KBP_FRWRD_IP_NOF_TABLES 
            },

             
            {
                FALSE, 
                ARAD_KBP_FRWRD_TBL_ID_TRILL_MC,  
                KBP_DB_INVALID,
                0, 
                NLM_TBL_WIDTH_80,   
                NLM_TBL_ADLEN_48B,  
                0,  
                20, 
                NLMDEV_BANK_0, 
                0, 
                {
                    1,
                    {{"TRILL-MC-KEY", 4, KBP_KEY_FIELD_PREFIX}}
                },
                ARAD_KBP_FRWRD_IP_NOF_TABLES 
            },
        };

static 
    ARAD_KBP_LTR_CONFIG
        Arad_kbp_ltr_config_static[ARAD_KBP_FRWRD_DB_NOF_TYPE_ARAD] = {

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TABLE_OPCODE_IPV4_UC, 
                ARAD_KBP_FRWRD_LTR_IPV4_UC, 
                0x1, 
                {ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0}, 
                NlmFALSE, 
                { 
                {2, {{"VRF", 2, KBP_KEY_FIELD_EM},{"DIP", 4, KBP_KEY_FIELD_PREFIX}}}
                },
                
                {3, {{"VRF", 2, KBP_KEY_FIELD_EM},{"SIP", 4, KBP_KEY_FIELD_PREFIX},{"DIP", 4, KBP_KEY_FIELD_PREFIX}}},
                NULL, 
                NULL, 
            },

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TABLE_OPCODE_IPV4_UC_RPF, 
                ARAD_KBP_FRWRD_LTR_IPV4_UC_RPF, 
                0x3, 
                {ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0, ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_1}, 
                NlmFALSE, 
                { 
                {2, {{"VRF", 2, KBP_KEY_FIELD_EM},{"DIP", 4, KBP_KEY_FIELD_PREFIX}}},
                {2, {{"VRF", 2, KBP_KEY_FIELD_EM},{"SIP", 4, KBP_KEY_FIELD_PREFIX}}}
                },
                
                {3, {{"VRF", 2, KBP_KEY_FIELD_EM},{"SIP", 4, KBP_KEY_FIELD_PREFIX},{"DIP", 4, KBP_KEY_FIELD_PREFIX}}},
                NULL, 
                NULL, 
            },

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TABLE_OPCODE_IPV4_MC_COMP, 
                ARAD_KBP_FRWRD_LTR_IPV4_MC_RPF, 
                0x3, 
                {ARAD_KBP_FRWRD_TBL_ID_IPV4_MC, ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_1}, 
                NlmFALSE, 
                { 
                {4, {{"VRF", 2, KBP_KEY_FIELD_EM}, {"DIP", 4, KBP_KEY_FIELD_PREFIX}, {"SIP", 4, KBP_KEY_FIELD_PREFIX},{"IN-RIF", 2, KBP_KEY_FIELD_PREFIX}}},
                {2, {{"VRF", 2, KBP_KEY_FIELD_EM}, {"SIP", 4, KBP_KEY_FIELD_PREFIX}}}
                },
                
                {4, {{"VRF", 2, KBP_KEY_FIELD_EM}, {"IN-RIF", 2, KBP_KEY_FIELD_PREFIX}, {"SIP", 4, KBP_KEY_FIELD_PREFIX}, {"DIP", 4, KBP_KEY_FIELD_PREFIX}}},
                NULL, 
                NULL, 
            },

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TABLE_OPCODE_IPV6_UC, 
                ARAD_KBP_FRWRD_LTR_IPV6_UC, 
                0x1, 
                {ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_0}, 
                NlmFALSE, 
                { 
                {2, {{"VRF", 2, KBP_KEY_FIELD_EM}, {"DIP", 16, KBP_KEY_FIELD_PREFIX}}}
                },
                
                {3, {{"VRF", 2, KBP_KEY_FIELD_EM}, {"SIP", 16, KBP_KEY_FIELD_PREFIX}, {"DIP", 16, KBP_KEY_FIELD_PREFIX}}},
                NULL, 
                NULL, 
            },

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TABLE_OPCODE_IPV6_UC_RPF_2PASS, 
                ARAD_KBP_FRWRD_LTR_IPV6_UC_RPF_2PASS, 
                0x0, 
                {ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_0}, 
                NlmFALSE, 
                { 
                    {2, {{"VRF", 2}, {"SIP", 16}}},
                    {2, {{"VRF", 2}, {"SIP", 16}}}
                },
                
                {2, {{"VRF", 2}, {"SIP", 16}}},
                NULL, 
                NULL, 
            },

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TABLE_OPCODE_IPV6_UC_RPF, 
                ARAD_KBP_FRWRD_LTR_IPV6_UC_RPF, 
                0x0, 
                {ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_0, ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_1}, 
                NlmFALSE, 
                { 
                    {2, {{"VRF", 2, KBP_KEY_FIELD_EM}, {"DIP", 16, KBP_KEY_FIELD_PREFIX}}},
                    {2, {{"VRF", 2, KBP_KEY_FIELD_EM}, {"SIP", 16, KBP_KEY_FIELD_PREFIX}}}
                },
                
                {3, {{"VRF", 2, KBP_KEY_FIELD_EM}, {"SIP", 16, KBP_KEY_FIELD_PREFIX}, {"DIP", 16, KBP_KEY_FIELD_PREFIX}}},
                NULL, 
                NULL, 
            },

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TABLE_OPCODE_IPV6_MC_RPF, 
                ARAD_KBP_FRWRD_LTR_IPV6_MC_RPF, 
                0x3, 
                {ARAD_KBP_FRWRD_TBL_ID_IPV6_MC, ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_1}, 
                NlmFALSE, 
                { 
                    {4, {{"VRF", 2, KBP_KEY_FIELD_EM}, {"DIP", 16, KBP_KEY_FIELD_PREFIX}, {"SIP", 16, KBP_KEY_FIELD_PREFIX},{"IN-RIF", 2, KBP_KEY_FIELD_PREFIX}}},
                    {2, {{"VRF", 2, KBP_KEY_FIELD_EM}, {"SIP", 16, KBP_KEY_FIELD_PREFIX}}}
                },
                
                {4, {{"VRF", 2, KBP_KEY_FIELD_EM}, {"IN-RIF", 2, KBP_KEY_FIELD_PREFIX}, {"SIP", 16, KBP_KEY_FIELD_PREFIX}, {"DIP", 16, KBP_KEY_FIELD_PREFIX}}},
                NULL, 
                NULL, 
            },

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TABLE_OPCODE_LSR, 
                ARAD_KBP_FRWRD_LTR_LSR, 
                0x1, 
                {ARAD_KBP_FRWRD_TBL_ID_LSR}, 
                NlmFALSE, 
                { 
                {1, {{"MPLS-KEY", 6, KBP_KEY_FIELD_PREFIX}}}
                },
                
                {1, {{"MPLS-KEY", 6, KBP_KEY_FIELD_PREFIX}}},
                NULL, 
                NULL, 
            },

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TABLE_OPCODE_TRILL_UC, 
                ARAD_KBP_FRWRD_LTR_TRILL_UC, 
                0x1, 
                {ARAD_KBP_FRWRD_TBL_ID_TRILL_UC}, 
                NlmFALSE, 
                { 
                {1, {{"EGRESS-NICK", 2, KBP_KEY_FIELD_PREFIX}}}
                },
                
                {1, {{"EGRESS-NICK", 2, KBP_KEY_FIELD_PREFIX}}},
                NULL, 
                NULL, 
            },

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TABLE_OPCODE_TRILL_MC, 
                ARAD_KBP_FRWRD_LTR_TRILL_MC, 
                0x1, 
                {ARAD_KBP_FRWRD_TBL_ID_TRILL_MC}, 
                NlmFALSE, 
                { 
                {1, {{"TRILL-MC-KEY", 4, KBP_KEY_FIELD_PREFIX}}}
                },
                
                {1, {{"TRILL-MC-KEY", 4, KBP_KEY_FIELD_PREFIX}}},
                NULL, 
                NULL, 
            }
        };

static int Arad_plus_kbp_default_result_sizes_static[ARAD_KBP_MAX_NUM_OF_PARALLEL_SEARCHES] = {
    ARAD_KBP_LUT_AD_TRANSFER_6B,
    ARAD_KBP_LUT_AD_TRANSFER_4B,
    ARAD_KBP_LUT_AD_TRANSFER_2B,
    ARAD_KBP_LUT_AD_TRANSFER_3B
};

static int Arad_plus_kbp_default_result_formats_static[ARAD_KBP_MAX_NUM_OF_PARALLEL_SEARCHES] = {
    NLM_ARAD_INDEX_AND_64B_AD,
    NLM_ARAD_INDEX_AND_32B_AD,
    NLM_ARAD_INDEX_AND_32B_AD,
    NLM_ARAD_INDEX_AND_32B_AD
};

#ifdef BCM_88660_A0
static
    ARAD_KBP_TABLE_CONFIG
        Arad_plus_kbp_table_config_info_static[ARAD_KBP_FRWRD_IP_NOF_TABLES_ARAD_PLUS] = {

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0, 
                KBP_DB_INVALID,
                0, 
                NLM_TBL_WIDTH_80,   
                NLM_TBL_ADLEN_48B,  
                0,  
                20, 
                NLMDEV_BANK_0, 
                32, 
                { 
                    2,
                    {{"VRF", 2, KBP_KEY_FIELD_EM},{"DIP", 4, KBP_KEY_FIELD_PREFIX}}
                },
                ARAD_KBP_FRWRD_IP_NOF_TABLES 
            },

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_1, 
                KBP_DB_INVALID,
                0, 
                NLM_TBL_WIDTH_80,   
                NLM_TBL_ADLEN_24B,  
                0,  
                20, 
                NLMDEV_BANK_0, 
                32, 
                {
                    2,
                    {{"VRF", 2, KBP_KEY_FIELD_EM},{"SIP", 4, KBP_KEY_FIELD_PREFIX}}
                },
                ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0 
            },

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TBL_ID_IPV4_MC, 
                KBP_DB_INVALID,
                0, 
                NLM_TBL_WIDTH_160,   
                NLM_TBL_ADLEN_48B,  
                0,  
                20, 
                NLMDEV_BANK_0, 
                32, 
                {
                    4,
                    {{"VRF", 2, KBP_KEY_FIELD_EM},{"DIP", 4, KBP_KEY_FIELD_PREFIX},{"SIP", 4, KBP_KEY_FIELD_PREFIX},{"IN-RIF", 2, KBP_KEY_FIELD_PREFIX}}
                },
                ARAD_KBP_FRWRD_IP_NOF_TABLES 
            },

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_0, 
                KBP_DB_INVALID,
                0, 
                NLM_TBL_WIDTH_160,   
                NLM_TBL_ADLEN_48B,  
                0,  
                20, 
                NLMDEV_BANK_0, 
                128, 
                {
                    2,
                    {{"VRF", 2, KBP_KEY_FIELD_EM},{"DIP", 16, KBP_KEY_FIELD_PREFIX}}
                },
                ARAD_KBP_FRWRD_IP_NOF_TABLES 
            },

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_1, 
                KBP_DB_INVALID,
                0, 
                NLM_TBL_WIDTH_160,   
                NLM_TBL_ADLEN_24B,  
                0,  
                20, 
                NLMDEV_BANK_0, 
                128, 
                {
                    2,
                    {{"VRF", 2, KBP_KEY_FIELD_EM},{"SIP", 16, KBP_KEY_FIELD_PREFIX}}
                },
                ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_0 
            },

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TBL_ID_IPV6_MC, 
                KBP_DB_INVALID,
                0, 
                NLM_TBL_WIDTH_320,   
                NLM_TBL_ADLEN_48B,  
                0,  
                20, 
                NLMDEV_BANK_0, 
                0, 
                {
                    4,
                    {{"VRF", 2, KBP_KEY_FIELD_EM},{"DIP", 16, KBP_KEY_FIELD_PREFIX},{"SIP", 16, KBP_KEY_FIELD_PREFIX},{"IN-RIF", 2, KBP_KEY_FIELD_PREFIX}}
                },
                ARAD_KBP_FRWRD_IP_NOF_TABLES 
            },

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TBL_ID_LSR, 
                KBP_DB_INVALID,
                0, 
                NLM_TBL_WIDTH_80,   
                NLM_TBL_ADLEN_48B,  
                0,  
                20, 
                NLMDEV_BANK_0, 
                0, 
                {
                    1,
                    {{"MPLS-KEY", 6, KBP_KEY_FIELD_PREFIX}}
                },
                ARAD_KBP_FRWRD_IP_NOF_TABLES 
            },

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TBL_ID_TRILL_UC, 
                KBP_DB_INVALID,
                0, 
                NLM_TBL_WIDTH_80,   
                NLM_TBL_ADLEN_48B,  
                0,  
                20, 
                NLMDEV_BANK_0, 
                0, 
                {
                    1,
                    {{"EGRESS-NICK", 2, KBP_KEY_FIELD_PREFIX}}
                },
                ARAD_KBP_FRWRD_IP_NOF_TABLES 
            },

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TBL_ID_TRILL_MC, 
                KBP_DB_INVALID,
                0, 
                NLM_TBL_WIDTH_80,   
                NLM_TBL_ADLEN_48B,  
                0,  
                20, 
                NLMDEV_BANK_0, 
                0, 
                {
                    1,
                    {{"TRILL-MC-KEY", 4, KBP_KEY_FIELD_PREFIX}}
                },
                ARAD_KBP_FRWRD_IP_NOF_TABLES 
            },

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TBL_ID_LSR_IP_SHARED, 
                KBP_DB_INVALID,
                0, 
                NLM_TBL_WIDTH_80,   
                NLM_TBL_ADLEN_128B,  
                0,  
                20, 
                NLMDEV_BANK_0, 
                32, 
                {
                    3,
                    {{"MPLS-EXT-KEY", 3, KBP_KEY_FIELD_PREFIX},{"VRF", 2, KBP_KEY_FIELD_PREFIX},{"DIP", 4, KBP_KEY_FIELD_PREFIX}}
                },
                ARAD_KBP_FRWRD_IP_NOF_TABLES 
            },

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TBL_ID_LSR_IP_SHARED_FOR_IP, 
                KBP_DB_INVALID,
                0, 
                NLM_TBL_WIDTH_80,   
                NLM_TBL_ADLEN_128B,  
                0,  
                20, 
                NLMDEV_BANK_0, 
                32, 
                {
                    3,
                    {{"HOLE", 3, KBP_KEY_FIELD_PREFIX},{"VRF", 2, KBP_KEY_FIELD_PREFIX},{"DIP", 4, KBP_KEY_FIELD_PREFIX}}
                },
                ARAD_KBP_FRWRD_TBL_ID_LSR_IP_SHARED 
            },

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TBL_ID_LSR_IP_SHARED_FOR_LSR, 
                KBP_DB_INVALID,
                0, 
                NLM_TBL_WIDTH_80,   
                NLM_TBL_ADLEN_128B,  
                0,  
                20, 
                NLMDEV_BANK_0, 
                32, 
                {
                    3,
                    {{"MPLS-EXT-KEY", 3, KBP_KEY_FIELD_PREFIX},{"HOLE", 2, KBP_KEY_FIELD_PREFIX},{"HOLE", 4, KBP_KEY_FIELD_PREFIX}}
                },
                ARAD_KBP_FRWRD_TBL_ID_LSR_IP_SHARED 
            },

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TBL_ID_EXTENDED_IPV6, 
                KBP_DB_INVALID,
                0, 
                NLM_TBL_WIDTH_160,   
                NLM_TBL_ADLEN_128B,  
                0,  
                20, 
                NLMDEV_BANK_0, 
                32, 
                {
                    2,
                    {{"VRF", 2, KBP_KEY_FIELD_PREFIX},{"DIP", 16, KBP_KEY_FIELD_PREFIX}}
                },
                ARAD_KBP_FRWRD_IP_NOF_TABLES 
            },

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TBL_ID_EXTENDED_P2P, 
                KBP_DB_INVALID,
                0, 
                NLM_TBL_WIDTH_80,    
                NLM_TBL_ADLEN_128B,  
                0,  
                20, 
                NLMDEV_BANK_0, 
                32, 
                {
                    1,
                    {{"IN-LIF-ID", 2, KBP_KEY_FIELD_PREFIX}}
                },
                ARAD_KBP_FRWRD_IP_NOF_TABLES 
            },

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TBL_ID_INRIF_MAPPING, 
                KBP_DB_INVALID,
                0, 
                NLM_TBL_WIDTH_80,    
                NLM_TBL_ADLEN_128B,  
                0,  
                20, 
                NLMDEV_BANK_0, 
                32, 
                {
                    1,
                    {{"IN-RIF", 2, KBP_KEY_FIELD_PREFIX}}
                },
                ARAD_KBP_FRWRD_IP_NOF_TABLES 
            },

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TBL_ID_IPV4_DC, 
                KBP_DB_INVALID,
                0, 
                NLM_TBL_WIDTH_80,   
                NLM_TBL_ADLEN_48B,  
                0,  
                20, 
                NLMDEV_BANK_0, 
                32, 
                {
                    2,
                    {{"VRF_8B", 1, KBP_KEY_FIELD_EM},{"DIP", 4, KBP_KEY_FIELD_PREFIX}}
                },
                ARAD_KBP_FRWRD_IP_NOF_TABLES 
            },

            
            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_PUBLIC_RPF_0, 
                KBP_DB_INVALID,
                0, 
                NLM_TBL_WIDTH_80,   
                NLM_TBL_ADLEN_48B,  
                0,  
                20, 
                NLMDEV_BANK_0, 
                32, 
                {
                    1,
                    {{"DIP", 4, KBP_KEY_FIELD_EM}}
                },
                ARAD_KBP_FRWRD_IP_NOF_TABLES 
            },

            
            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_PUBLIC_RPF_1, 
                KBP_DB_INVALID,
                0, 
                NLM_TBL_WIDTH_80,   
                NLM_TBL_ADLEN_24B,  
                0,  
                20, 
                NLMDEV_BANK_0, 
                32, 
                {
                    1,
                    {{"SIP", 4, KBP_KEY_FIELD_EM}}
                },
                ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_PUBLIC_RPF_0 
            },

            
            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_PUBLIC_RPF_0, 
                KBP_DB_INVALID,
                0, 
                NLM_TBL_WIDTH_160,   
                NLM_TBL_ADLEN_48B,  
                0,  
                20, 
                NLMDEV_BANK_0, 
                128, 
                {
                    1,
                    {{"DIP", 16, KBP_KEY_FIELD_EM}}
                },
                ARAD_KBP_FRWRD_IP_NOF_TABLES 
            },

            
            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_PUBLIC_RPF_1, 
                KBP_DB_INVALID,
                0, 
                NLM_TBL_WIDTH_160,   
                NLM_TBL_ADLEN_24B,  
                0,  
                20, 
                NLMDEV_BANK_0, 
                128, 
                {
                    1,
                    {{"SIP", 16, KBP_KEY_FIELD_EM}}
                },
                ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_PUBLIC_RPF_0 
            },
            
            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TBL_ID_IPV4_MC_BRIDGE, 
                KBP_DB_INVALID,
                0, 
                NLM_TBL_WIDTH_80,   
                NLM_TBL_ADLEN_24B,  
                0,  
                20, 
                NLMDEV_BANK_0, 
                20, 
                {
                    3,
                    {{"FID", 2, KBP_KEY_FIELD_EM},{"DIP", 4, KBP_KEY_FIELD_PREFIX},{"SIP", 4, KBP_KEY_FIELD_PREFIX}}
                },
                ARAD_KBP_FRWRD_IP_NOF_TABLES 
            },
            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV4_MC, 
                KBP_DB_INVALID,
                0, 
                NLM_TBL_WIDTH_80,   
                NLM_TBL_ADLEN_32B,  
                0,  
                20, 
                NLMDEV_BANK_0, 
                -1, 
                {
                    0,
                    {{{0,},0,0},}
                },
                ARAD_KBP_FRWRD_IP_NOF_TABLES 
            },

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV6_MC, 
                KBP_DB_INVALID,
                0, 
                NLM_TBL_WIDTH_80,   
                NLM_TBL_ADLEN_32B,  
                0,  
                20, 
                NLMDEV_BANK_0, 
                -1, 
                {
                    0,
                    {{{0,},0,0},}
                },
                ARAD_KBP_FRWRD_IP_NOF_TABLES 
            },

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV4_UC, 
                KBP_DB_INVALID,
                0, 
                NLM_TBL_WIDTH_80,   
                NLM_TBL_ADLEN_32B,  
                0,  
                20, 
                NLMDEV_BANK_0, 
                -1, 
                {
                    0,
                    {{{0,},0,0},}
                },
                ARAD_KBP_FRWRD_IP_NOF_TABLES 
            },

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV6_UC, 
                KBP_DB_INVALID,
                0, 
                NLM_TBL_WIDTH_80,   
                NLM_TBL_ADLEN_32B,  
                0,  
                20, 
                NLMDEV_BANK_0, 
                -1, 
                {
                    0,
                    {{{0,},0,0},}
                },
                ARAD_KBP_FRWRD_IP_NOF_TABLES 
            },

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV4_DC,  
                KBP_DB_INVALID,
                0, 
                NLM_TBL_WIDTH_80,   
                NLM_TBL_ADLEN_32B,   
                0,  
                20, 
                NLMDEV_BANK_0, 
                -1, 
                {
                    0,
                    {{{0,},0,0},}
                },
                ARAD_KBP_FRWRD_IP_NOF_TABLES 
            },

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV4_UC_PUBLIC, 
                KBP_DB_INVALID,
                0, 
                NLM_TBL_WIDTH_80,   
                NLM_TBL_ADLEN_32B,  
                0,  
                20, 
                NLMDEV_BANK_0, 
                -1, 
                {
                    0,
                    {{{0,},0,0},}
                },
                ARAD_KBP_FRWRD_IP_NOF_TABLES 
            },

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV6_UC_PUBLIC, 
                KBP_DB_INVALID,
                0, 
                NLM_TBL_WIDTH_80,   
                NLM_TBL_ADLEN_32B,  
                0,  
                20, 
                NLMDEV_BANK_0, 
                -1, 
                {
                    0,
                    {{{0,},0,0},}
                },
                ARAD_KBP_FRWRD_IP_NOF_TABLES 
            },
            
            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TBL_ID_DUMMY_FRWRD,  
                KBP_DB_INVALID,
                0, 
                NLM_TBL_WIDTH_80,    
                NLM_TBL_ADLEN_32B,   
                0,  
                20, 
                NLMDEV_BANK_0, 
                32, 
                {
                    1,
                    {{"DUMMY", 1, KBP_KEY_FIELD_PREFIX}}
                },
                ARAD_KBP_FRWRD_IP_NOF_TABLES 
            },
        };

ARAD_KBP_LTR_SINGLE_SEARCH ipv4_mc_extended_search = 
       {4, {{"VRF", 2, KBP_KEY_FIELD_TERNARY}, {"IN-RIF", 2, KBP_KEY_FIELD_TERNARY}, {"DIP", 4, KBP_KEY_FIELD_TERNARY}, {"SIP", 4, KBP_KEY_FIELD_TERNARY}}};

ARAD_KBP_LTR_SINGLE_SEARCH inrif_mapping_search = 
       {1, { {"IN-RIF", 2, KBP_KEY_FIELD_TERNARY}}} ;

ARAD_KBP_LTR_SINGLE_SEARCH ipv4_mc_extended_search_1 = 
       {2, {{"VRF", 2, KBP_KEY_FIELD_EM}, {"SIP", 4, KBP_KEY_FIELD_PREFIX}}};

static 
    ARAD_KBP_LTR_CONFIG
        Arad_plus_kbp_ltr_config_static[ARAD_KBP_FRWRD_DB_NOF_TYPE_ARAD_PLUS] = {

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TABLE_OPCODE_IPV4_UC, 
                ARAD_KBP_FRWRD_LTR_IPV4_UC, 
                0x1, 
                {ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0}, 
                NlmFALSE, 
                { 
                {2, {{"VRF", 2, KBP_KEY_FIELD_EM},{"DIP", 4, KBP_KEY_FIELD_PREFIX}}}
                },
                
                {3, {{"VRF", 2, KBP_KEY_FIELD_EM},{"SIP", 4, KBP_KEY_FIELD_PREFIX},{"DIP", 4, KBP_KEY_FIELD_PREFIX}}},
                NULL, 
                NULL, 
            },

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TABLE_OPCODE_IPV4_UC_RPF, 
                ARAD_KBP_FRWRD_LTR_IPV4_UC_RPF, 
                0x7, 
                {ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0, ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV4_UC, ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_1}, 
                NlmTRUE, 
                { 
                {2, {{"VRF", 2, KBP_KEY_FIELD_EM},{"DIP", 4, KBP_KEY_FIELD_PREFIX}}},
                {1, {{"VRF", 2, KBP_KEY_FIELD_EM}}},
                {2, {{"VRF", 2, KBP_KEY_FIELD_EM},{"SIP", 4, KBP_KEY_FIELD_PREFIX}}}
                },
                
                {3, {{"VRF", 2, KBP_KEY_FIELD_EM},{"SIP", 4, KBP_KEY_FIELD_PREFIX},{"DIP", 4, KBP_KEY_FIELD_PREFIX}}},
                NULL, 
                NULL, 
            },

            
            {
                
                FALSE, 
                ARAD_KBP_FRWRD_TABLE_OPCODE_IPV4_MC_COMP, 
                ARAD_KBP_FRWRD_LTR_IPV4_MC_RPF, 
                0x7, 
                {ARAD_KBP_FRWRD_TBL_ID_IPV4_MC, ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV4_MC, ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_1}, 
                NlmFALSE, 
                { 
                {4, {{"VRF", 2, KBP_KEY_FIELD_EM}, {"DIP", 4, KBP_KEY_FIELD_PREFIX}, {"SIP", 4, KBP_KEY_FIELD_PREFIX},{"IN-RIF", 2, KBP_KEY_FIELD_PREFIX}}},
                {1, {{"VRF", 2, KBP_KEY_FIELD_EM}}}, 
                {2, {{"VRF", 2, KBP_KEY_FIELD_EM}, {"SIP", 4, KBP_KEY_FIELD_PREFIX}}}
                },
                
                {4, {{"VRF", 2, KBP_KEY_FIELD_EM}, {"IN-RIF", 2, KBP_KEY_FIELD_PREFIX}, {"SIP", 4, KBP_KEY_FIELD_PREFIX}, {"DIP", 4, KBP_KEY_FIELD_PREFIX}}},
                NULL, 
                NULL, 
            },

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TABLE_OPCODE_IPV6_UC, 
                ARAD_KBP_FRWRD_LTR_IPV6_UC, 
                0x1, 
                {ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_0}, 
                NlmFALSE, 
                { 
                {2, {{"VRF", 2, KBP_KEY_FIELD_EM}, {"DIP", 16, KBP_KEY_FIELD_PREFIX}}}
                },
                
                {3, {{"VRF", 2, KBP_KEY_FIELD_EM}, {"SIP", 16, KBP_KEY_FIELD_PREFIX}, {"DIP", 16, KBP_KEY_FIELD_PREFIX}}},
                NULL, 
                NULL, 
            },
            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TABLE_OPCODE_IPV6_UC_RPF_2PASS, 
                ARAD_KBP_FRWRD_LTR_IPV6_UC_RPF_2PASS, 
                0x0, 
                {ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_0}, 
                NlmFALSE, 
                { 
                    {2, {{"VRF", 2}, {"SIP", 16}}},
                    {2, {{"VRF", 2}, {"SIP", 16}}}
                },
                
                {2, {{"VRF", 2}, {"SIP", 16}}},
                NULL, 
                NULL, 
            },
            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TABLE_OPCODE_IPV6_UC_RPF, 
                ARAD_KBP_FRWRD_LTR_IPV6_UC_RPF, 
                0x7, 
                {ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_0,ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV6_UC,ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_1}, 
                NlmTRUE, 
                { 
                    {2, {{"VRF", 2, KBP_KEY_FIELD_EM}, {"DIP", 16, KBP_KEY_FIELD_PREFIX}}},
                    {1, {{"VRF", 2, KBP_KEY_FIELD_EM}}},
                    {2, {{"VRF", 2, KBP_KEY_FIELD_EM}, {"SIP", 16, KBP_KEY_FIELD_PREFIX}}}
                },
                
                {3, {{"VRF", 2, KBP_KEY_FIELD_EM}, {"SIP", 16, KBP_KEY_FIELD_PREFIX}, {"DIP", 16, KBP_KEY_FIELD_PREFIX}}},
                NULL, 
                NULL, 
            },

            
            {
                
                FALSE, 
                ARAD_KBP_FRWRD_TABLE_OPCODE_IPV6_MC_RPF, 
                ARAD_KBP_FRWRD_LTR_IPV6_MC_RPF, 
                0x7, 
                {ARAD_KBP_FRWRD_TBL_ID_IPV6_MC, ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV6_MC, ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_1}, 
                NlmFALSE, 
                { 
                    {4, {{"VRF", 2, KBP_KEY_FIELD_EM}, {"DIP", 16, KBP_KEY_FIELD_PREFIX}, {"SIP", 16, KBP_KEY_FIELD_PREFIX},{"IN-RIF", 2, KBP_KEY_FIELD_PREFIX}}},
                    {1, {{"VRF", 2, KBP_KEY_FIELD_EM}}}, 
                    {2, {{"VRF", 2, KBP_KEY_FIELD_EM}, {"SIP", 16, KBP_KEY_FIELD_PREFIX}}}
                },
                
                {4, {{"VRF", 2, KBP_KEY_FIELD_EM}, {"IN-RIF", 2, KBP_KEY_FIELD_PREFIX}, {"SIP", 16, KBP_KEY_FIELD_PREFIX}, {"DIP", 16, KBP_KEY_FIELD_PREFIX}}},
                NULL, 
                NULL, 
            },

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TABLE_OPCODE_LSR, 
                ARAD_KBP_FRWRD_LTR_LSR, 
                0x1, 
                {ARAD_KBP_FRWRD_TBL_ID_LSR}, 
                NlmFALSE, 
                { 
                {1, {{"MPLS-KEY", 6, KBP_KEY_FIELD_PREFIX}}}
                },
                
                {1, {{"MPLS-KEY", 6, KBP_KEY_FIELD_PREFIX}}},
                NULL, 
                NULL, 
            },

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TABLE_OPCODE_TRILL_UC, 
                ARAD_KBP_FRWRD_LTR_TRILL_UC, 
                0x1, 
                {ARAD_KBP_FRWRD_TBL_ID_TRILL_UC}, 
                NlmFALSE, 
                { 
                {1, {{"EGRESS-NICK", 2, KBP_KEY_FIELD_PREFIX}}}
                },
                
                {1, {{"EGRESS-NICK", 2, KBP_KEY_FIELD_PREFIX}}},
                NULL, 
                NULL, 
            },

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TABLE_OPCODE_TRILL_MC, 
                ARAD_KBP_FRWRD_LTR_TRILL_MC, 
                0x1, 
                {ARAD_KBP_FRWRD_TBL_ID_TRILL_MC}, 
                NlmFALSE, 
                { 
                {1, {{"TRILL-MC-KEY", 4, KBP_KEY_FIELD_PREFIX}}}
                },
                
                {1, {{"TRILL-MC-KEY", 4, KBP_KEY_FIELD_PREFIX}}},
                NULL, 
                NULL, 
            },

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TABLE_OPCODE_IPV4_DC, 
                ARAD_KBP_FRWRD_LTR_IPV4_DC, 
                0x7, 
                {ARAD_KBP_FRWRD_TBL_ID_IPV4_DC, ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV4_DC, ARAD_KBP_FRWRD_TBL_ID_IPV4_DC}, 
                NlmTRUE, 
                { 
                    {2, {{"VRF_8B", 1, KBP_KEY_FIELD_EM}, {"DIP", 4, KBP_KEY_FIELD_PREFIX}}},
                    {1, {{"VRF_8B", 1, KBP_KEY_FIELD_EM}}},
                    {2, {{"VRF_8B", 1, KBP_KEY_FIELD_EM}, {"DIP", 4, KBP_KEY_FIELD_PREFIX}}}
                },
                
                {3, {{"VRF_8B", 1, KBP_KEY_FIELD_EM},{"SIP", 4, KBP_KEY_FIELD_PREFIX},{"DIP", 4, KBP_KEY_FIELD_PREFIX}}},
                NULL, 
                NULL, 
            },

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TABLE_OPCODE_NONE, 
                ARAD_KBP_FRWRD_LTR_IP_LSR_SHARED, 
                0x1, 
                {ARAD_KBP_FRWRD_TBL_ID_LSR_IP_SHARED}, 
                NlmFALSE, 
                { 
                {3, {{"MPLS-EXT-KEY", 3, KBP_KEY_FIELD_TERNARY},{"VRF", 2, KBP_KEY_FIELD_TERNARY},{"DIP", 4, KBP_KEY_FIELD_TERNARY}}}
                },
                
                {3, {{"MPLS-EXT-KEY", 3, KBP_KEY_FIELD_TERNARY},{"VRF", 2, KBP_KEY_FIELD_TERNARY},{"DIP", 4, KBP_KEY_FIELD_TERNARY}}},
                NULL, 
                NULL, 
            },

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TABLE_OPCODE_SHARED_IP_LSR_FOR_IP, 
                ARAD_KBP_FRWRD_LTR_IP_LSR_SHARED_FOR_IP, 
                0x3, 
                {ARAD_KBP_FRWRD_TBL_ID_LSR_IP_SHARED_FOR_IP, ARAD_KBP_FRWRD_TBL_ID_INRIF_MAPPING}, 
                NlmFALSE, 
                { 
                {3, {{"HOLE", 3, KBP_KEY_FIELD_HOLE},{"VRF", 2, KBP_KEY_FIELD_TERNARY},{"DIP", 4, KBP_KEY_FIELD_TERNARY}}},
                {1, {{"IN-RIF", 2, KBP_KEY_FIELD_TERNARY}}}
                },
                
                {4, {{"VRF", 2, KBP_KEY_FIELD_TERNARY},{"SIP", 4, KBP_KEY_FIELD_TERNARY},{"DIP", 4, KBP_KEY_FIELD_TERNARY},{"IN-RIF", 2, KBP_KEY_FIELD_TERNARY}}},
                NULL, 
                NULL, 
            },

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TABLE_OPCODE_SHARED_IP_LSR_FOR_IP_WITH_RPF, 
                ARAD_KBP_FRWRD_LTR_IP_LSR_SHARED_FOR_IP_WITH_RPF, 
                0x7, 
                {ARAD_KBP_FRWRD_TBL_ID_LSR_IP_SHARED_FOR_IP, ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_1, ARAD_KBP_FRWRD_TBL_ID_INRIF_MAPPING}, 
                NlmFALSE, 
                { 
                {3, {{"HOLE", 3, KBP_KEY_FIELD_HOLE},{"VRF", 2, KBP_KEY_FIELD_TERNARY},{"DIP", 4, KBP_KEY_FIELD_TERNARY}}},
                {2, {{"VRF", 2, KBP_KEY_FIELD_EM}, {"SIP", 4, KBP_KEY_FIELD_PREFIX}}},
                {1, {{"IN-RIF", 2, KBP_KEY_FIELD_PREFIX}}}
                },
                
                {4, {{"VRF", 2, KBP_KEY_FIELD_EM},{"SIP", 4, KBP_KEY_FIELD_PREFIX},{"DIP", 4, KBP_KEY_FIELD_PREFIX},{"IN-RIF", 2, KBP_KEY_FIELD_PREFIX}}},
                NULL, 
                NULL, 
            },

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TABLE_OPCODE_SHARED_IP_LSR_FOR_LSR, 
                ARAD_KBP_FRWRD_LTR_IP_LSR_SHARED_FOR_LSR, 
                0x1, 
                {ARAD_KBP_FRWRD_TBL_ID_LSR_IP_SHARED_FOR_LSR}, 
                NlmFALSE, 
                { 
                {3, {{"MPLS-EXT-KEY", 3, KBP_KEY_FIELD_TERNARY}, {"HOLE", 2, KBP_KEY_FIELD_HOLE}, {"HOLE", 4, KBP_KEY_FIELD_HOLE}}}
                },
                
                {1, {{"MPLS-EXT-KEY", 3, KBP_KEY_FIELD_TERNARY}}},
                NULL, 
                NULL, 
            },

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TABLE_OPCODE_EXTENDED_IPV6, 
                ARAD_KBP_FRWRD_LTR_IP_LSR_EXTENDED_IPV6, 
                0x1, 
                {ARAD_KBP_FRWRD_TBL_ID_EXTENDED_IPV6}, 
                NlmFALSE, 
                { 
                {2, {{"VRF", 2, KBP_KEY_FIELD_EM},{"DIP", 16, KBP_KEY_FIELD_PREFIX}}}
                },
                
                {2, {{"VRF", 2, KBP_KEY_FIELD_EM}, {"DIP", 16, KBP_KEY_FIELD_PREFIX}}},
                NULL, 
                NULL, 
            },

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TABLE_OPCODE_EXTENDED_P2P, 
                ARAD_KBP_FRWRD_LTR_IP_LSR_EXTENDED_P2P, 
                0x1, 
                {ARAD_KBP_FRWRD_TBL_ID_EXTENDED_P2P}, 
                NlmFALSE, 
                { 
                {1, {{"IN-LIF-ID", 2, KBP_KEY_FIELD_PREFIX}}}
                },
                
                {1, {{"IN-LIF-ID", 2, KBP_KEY_FIELD_PREFIX}}},
                NULL, 
                NULL, 
            },

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TABLE_OPCODE_IPV4_UC_PUBLIC, 
                ARAD_KBP_FRWRD_LTR_IPV4_UC_PUBLIC, 
                0x1, 
                {ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_PUBLIC_RPF_0}, 
                NlmFALSE, 
                { 
                {1, {{"DUMMY", 1, KBP_KEY_FIELD_PREFIX}}}
                },
                
                {1, {{"DUMMY", 1, KBP_KEY_FIELD_PREFIX}}},
                NULL, 
                NULL, 
            },

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TABLE_OPCODE_IPV4_UC_PUBLIC_RPF, 
                ARAD_KBP_FRWRD_LTR_IPV4_UC_RPF_PUBLIC, 
                0x3, 
                {ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_PUBLIC_RPF_1}, 
                NlmTRUE, 
                { 
                {1, {{"DUMMY", 1, KBP_KEY_FIELD_PREFIX}}}
                },
                
                {1, {{"DUMMY", 1, KBP_KEY_FIELD_PREFIX}}},
                NULL, 
                NULL, 
            },

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TABLE_OPCODE_IPV6_UC_PUBLIC, 
                ARAD_KBP_FRWRD_LTR_IPV6_UC_PUBLIC, 
                0x1, 
                {ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_PUBLIC_RPF_0}, 
                NlmFALSE, 
                { 
                {1, {{"DUMMY", 1, KBP_KEY_FIELD_PREFIX}}}
                },
                
                {1, {{"DUMMY", 1, KBP_KEY_FIELD_PREFIX}}},
                NULL, 
                NULL, 
            },

            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TABLE_OPCODE_IPV6_UC_PUBLIC_RPF, 
                ARAD_KBP_FRWRD_LTR_IPV6_UC_RPF_PUBLIC, 
                0x3, 
                {ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_PUBLIC_RPF_1}, 
                NlmTRUE, 
                { 
                {1, {{"DUMMY", 1, KBP_KEY_FIELD_PREFIX}}}
                },
                
                {1, {{"DUMMY", 1, KBP_KEY_FIELD_PREFIX}}},
                NULL, 
                NULL, 
            },
            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TABLE_OPCODE_IPV4_MC_BRIDGE, 
                ARAD_KBP_FRWRD_LTR_IPV4_MC_BRIDGE, 
                0x1, 
                {ARAD_KBP_FRWRD_TBL_ID_IPV4_MC_BRIDGE}, 
                NlmFALSE, 
                { 
                {3, {{"FID", 2, KBP_KEY_FIELD_EM}, {"DIP", 4, KBP_KEY_FIELD_PREFIX}, {"SIP", 4, KBP_KEY_FIELD_PREFIX}}}
                },
                
                {3, {{"FID", 2, KBP_KEY_FIELD_EM}, {"DIP", 4, KBP_KEY_FIELD_PREFIX}, {"SIP", 4, KBP_KEY_FIELD_PREFIX}}},
                NULL, 
                NULL, 
            },
            
            {
                FALSE, 
                ARAD_KBP_FRWRD_TABLE_OPCODE_NONE, 
                ARAD_KBP_FRWRD_LTR_DUMMY_FRWRD, 
                0x1, 
                {ARAD_KBP_FRWRD_TBL_ID_DUMMY_FRWRD}, 
                NlmFALSE, 
                { 
                {1, {{"DUMMY", 1, KBP_KEY_FIELD_PREFIX}}}
                },
                
                {1, {{"DUMMY", 1, KBP_KEY_FIELD_PREFIX}}},
                NULL, 
                NULL, 
            },
        };

static int Jer_kbp_default_result_sizes_static[ARAD_KBP_MAX_NUM_OF_PARALLEL_SEARCHES] = {
    ARAD_KBP_LUT_AD_TRANSFER_6B,
    ARAD_KBP_LUT_AD_TRANSFER_4B,
    ARAD_KBP_LUT_AD_TRANSFER_3B,
    ARAD_KBP_LUT_AD_TRANSFER_2B,
    ARAD_KBP_LUT_AD_TRANSFER_4B,
    ARAD_KBP_LUT_AD_TRANSFER_4B,
    ARAD_KBP_LUT_AD_TRANSFER_3B,
    ARAD_KBP_LUT_AD_TRANSFER_2B
};

static int Jer_kbp_default_result_formats_static[ARAD_KBP_MAX_NUM_OF_PARALLEL_SEARCHES] = {
    NLM_ARAD_INDEX_AND_64B_AD,
    NLM_ARAD_INDEX_AND_32B_AD,
    NLM_ARAD_INDEX_AND_32B_AD,
    NLM_ARAD_INDEX_AND_32B_AD,
    NLM_ARAD_INDEX_AND_32B_AD,
    NLM_ARAD_INDEX_AND_32B_AD,
    NLM_ARAD_INDEX_AND_32B_AD,
    NLM_ARAD_INDEX_AND_32B_AD
};

static int Jer_kbp_default_result_sizes_ipv4_dc_static[ARAD_KBP_MAX_NUM_OF_PARALLEL_SEARCHES] = {
    ARAD_KBP_LUT_AD_TRANSFER_6B,
    ARAD_KBP_LUT_AD_TRANSFER_1B,
    ARAD_KBP_LUT_AD_TRANSFER_12B,
    ARAD_KBP_LUT_AD_TRANSFER_4B,
    ARAD_KBP_LUT_AD_TRANSFER_4B,
    0
};

static int Jer_kbp_default_result_formats_ipv4_dc_static[ARAD_KBP_MAX_NUM_OF_PARALLEL_SEARCHES] = {
    NLM_ARAD_INDEX_AND_64B_AD,
    NLM_ARAD_INDEX_AND_32B_AD,
    NLM_ARAD_INDEX_AND_128B_AD,
    NLM_ARAD_INDEX_AND_32B_AD,
    NLM_ARAD_INDEX_AND_32B_AD,
    NLM_ARAD_NO_INDEX_NO_AD
};

static int Jer_kbp_default_result_sizes_ipv4_dc_24_static[ARAD_KBP_MAX_NUM_OF_PARALLEL_SEARCHES] = {
    ARAD_KBP_LUT_AD_TRANSFER_6B,
    ARAD_KBP_LUT_AD_TRANSFER_3B,
    ARAD_KBP_LUT_AD_TRANSFER_6B,
    ARAD_KBP_LUT_AD_TRANSFER_5B,
    ARAD_KBP_LUT_AD_TRANSFER_4B,
    ARAD_KBP_LUT_AD_TRANSFER_4B
};

static int Jer_kbp_default_result_formats_ipv4_dc_24_static[ARAD_KBP_MAX_NUM_OF_PARALLEL_SEARCHES] = {
    NLM_ARAD_INDEX_AND_64B_AD,
    NLM_ARAD_INDEX_AND_32B_AD,
    NLM_ARAD_INDEX_AND_64B_AD,
    NLM_ARAD_INDEX_AND_64B_AD,
    NLM_ARAD_INDEX_AND_32B_AD,
    NLM_ARAD_INDEX_AND_32B_AD
};

#endif 

kbp_warmboot_t kbp_warmboot_data[SOC_SAND_MAX_DEVICE];
genericTblMgrAradAppData *AradAppData[SOC_SAND_MAX_DEVICE];







uint32 arad_kbp_alg_kbp_db_get(
    int    unit,
    uint32 frwrd_tbl_id,
    struct kbp_db **db_p)
{
    uint32 res = SOC_SAND_OK;
    *db_p = AradAppData[unit]->g_gtmInfo[frwrd_tbl_id].tblInfo.db_p;
    return res;
}

uint32 arad_kbp_alg_kbp_ad_db_get(
    int    unit,
    uint32 frwrd_tbl_id,
    struct kbp_ad_db **ad_db_p)
{
    uint32 res = SOC_SAND_OK;
    *ad_db_p = AradAppData[unit]->g_gtmInfo[frwrd_tbl_id].tblInfo.ad_db_p;
    return res;
}

uint32 arad_kbp_ltr_get_inst_pointer(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                     ltr_idx,
    SOC_SAND_OUT struct  kbp_instruction    **inst_p)
{
    uint32 res;
    ARAD_KBP_LTR_CONFIG ltr_config_info = {0};

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_KBP_LTR_CONFIG_clear(&ltr_config_info);

    res = KBP_INFO.Arad_kbp_ltr_config.get(unit, ltr_idx, &ltr_config_info); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if(ltr_config_info.valid){
        *inst_p = ltr_config_info.inst_p;
    } else {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                (BSL_META_U(unit,
                        "Error in %s(): Invalid LTR ID %d\n"),
                        FUNCTION_NAME(), ltr_idx));
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 20, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_ltr_get_inst_pointer()", 0, 0);
}

uint32 arad_kbp_gtm_table_info_get(
    int       unit,
    uint32    frwrd_tbl_id,
    tableInfo *tblInfo_p)
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(tblInfo_p);

    if (AradAppData[unit] == NULL) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                (BSL_META_U(unit,
                        "Error in %s(): kbp device is not initialized\n"), 
                        FUNCTION_NAME()));
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
    }
    else {
        sal_memcpy(tblInfo_p, 
                   &(AradAppData[unit]->g_gtmInfo[frwrd_tbl_id].tblInfo), 
                   sizeof(tableInfo));
    }
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_gtm_table_info_get()", frwrd_tbl_id, 0);
}


uint32 arad_kbp_get_device_pointer(
    SOC_SAND_IN  int                unit,
    SOC_SAND_OUT struct kbp_device  **device_p)
{
    uint32 core = 0;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(device_p);

    if(AradAppData[unit]->device_p[core] != NULL) {
        *device_p = AradAppData[unit]->device_p[core];
    }
    else {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                (BSL_META_U(unit,
                        "Error in %s(): KBP device is not initialized\n"), 
                        FUNCTION_NAME()));
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 20, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_get_device_pointer()", 0, 0);
}

uint32 arad_kbp_gtm_ltr_num_get(
   int    unit,
   uint32 frwrd_tbl_id,
   uint8  *ltr_num)
{
    uint32 res = SOC_SAND_OK;
    *ltr_num = AradAppData[unit]->g_gtmInfo[frwrd_tbl_id].ltr_num;
    return res;
}

uint32 arad_kbp_ltr_config_get(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 flp_program,
    SOC_SAND_OUT ARAD_KBP_LTR_CONFIG *config)
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(config);

    LOG_CLI((BSL_META_U(unit, "<UnAvailable> ")));

    ARAD_DO_NOTHING_AND_EXIT;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_ltr_config_get()", 0, 0);
}



uint32 arad_kbp_table_size_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_KBP_FRWRD_IP_TBL_ID table_id,
    SOC_SAND_OUT uint32                  *table_size_in_bytes,
    SOC_SAND_OUT uint32                  *table_payload_in_bytes)
{
    uint32
        res,
        tbl_width_in_bits = 0,
        tbl_payload_in_bits = 0;

    uint8 is_valid;

    ARAD_KBP_DB_HANDLES db_handles_info;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    if(table_id >= ARAD_KBP_MAX_NUM_OF_TABLES) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                (BSL_META_U(unit,
                        "Error in %s(): Table ID %d is not yet created\n"), 
                        FUNCTION_NAME(), table_id));
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
    }
    if(table_id >= ARAD_KBP_ACL_TABLE_ID_OFFSET) {
        
        res = KBP_INFO.db_info.get(unit, table_id,&db_handles_info); 
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
        is_valid = db_handles_info.is_valid;
        if (!is_valid) {
            LOG_ERROR(BSL_LS_SOC_TCAM,
                    (BSL_META_U(unit,
                            "Error in %s(): Table ID %d is not valid\n"), 
                            FUNCTION_NAME(), table_id));
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 60, exit);
        }
        tbl_width_in_bits = db_handles_info.table_width;
        tbl_payload_in_bits = db_handles_info.table_asso_width;

    }
    else {
        res = KBP_INFO.Arad_kbp_table_config_info.tbl_width.get(unit, table_id, &tbl_width_in_bits); 
        SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
        res = KBP_INFO.Arad_kbp_table_config_info.tbl_asso_width.get(unit, table_id, &tbl_payload_in_bits); 
        SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
    }
    *table_size_in_bytes = SOC_SAND_DIV_ROUND_UP(tbl_width_in_bits, SOC_SAND_NOF_BITS_IN_CHAR); 
    *table_payload_in_bytes = SOC_SAND_DIV_ROUND_UP(tbl_payload_in_bits, SOC_SAND_NOF_BITS_IN_CHAR); 

    ARAD_DO_NOTHING_AND_EXIT;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_table_size_get()", table_id, 0x0);
}

uint32 arad_kbp_static_table_size_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_KBP_FRWRD_IP_TBL_ID table_id,
    SOC_SAND_OUT uint32                   *table_size_in_bytes,
    SOC_SAND_OUT uint32                   *table_payload_in_bytes)
{
    uint32
        tbl_width_in_bits = 0,
        tbl_payload_in_bits = 0;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    if(table_id >= ARAD_KBP_FRWRD_IP_NOF_TABLES_ARAD) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                (BSL_META_U(unit,
                        "Error in %s(): Table ID %d not yet created\n"), 
                        FUNCTION_NAME(), table_id));
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
    }

    tbl_width_in_bits = Arad_kbp_table_config_info_static[table_id].tbl_width;
    tbl_payload_in_bits = Arad_kbp_table_config_info_static[table_id].tbl_asso_width;

    *table_size_in_bytes = SOC_SAND_DIV_ROUND_UP(tbl_width_in_bits, SOC_SAND_NOF_BITS_IN_CHAR); 
    *table_payload_in_bytes = SOC_SAND_DIV_ROUND_UP(tbl_payload_in_bits, SOC_SAND_NOF_BITS_IN_CHAR); 

    ARAD_DO_NOTHING_AND_EXIT;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_static_table_size_get()", table_id, 0x0);
}


uint32 arad_kbp_table_ltr_id_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_KBP_FRWRD_IP_TBL_ID table_id,
    SOC_SAND_OUT ARAD_KBP_FRWRD_IP_LTR    *ltr_id)
{
    ARAD_KBP_FRWRD_IP_DB_TYPE
        gtm_db_type_ndx;
    uint32
        srch_ndx, res;
    uint8
        found;

    ARAD_KBP_LTR_CONFIG ltr_config_info = {0};

    int num_of_kbp_searches = ARAD_KBP_NUM_OF_KBP_SEARCHES(unit);

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_KBP_LTR_CONFIG_clear(&ltr_config_info);

    found = FALSE;
    for(gtm_db_type_ndx = 0; (gtm_db_type_ndx < ARAD_KBP_MAX_NUM_OF_FRWRD_DBS) && (!found); gtm_db_type_ndx++)
    {
        res = KBP_INFO.Arad_kbp_ltr_config.get(unit, gtm_db_type_ndx, &ltr_config_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        for(srch_ndx = 0; (srch_ndx < num_of_kbp_searches) && (!found); srch_ndx++)
        {
            if (SHR_BITGET(&(ltr_config_info.parallel_srches_bmp), srch_ndx)) {
                if (ltr_config_info.tbl_id[srch_ndx] == table_id) {
                    found = TRUE;
                    *ltr_id = ltr_config_info.ltr_id;
                }
            }
        }
    }

    
    if (!found) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                (BSL_META_U(unit,
                        "Error in %s(): LTR ID is not found using Table ID %d\n"), 
                        FUNCTION_NAME(), table_id));
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
    }

    ARAD_DO_NOTHING_AND_EXIT;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_table_ltr_id_get()", table_id, 0x0);
}


STATIC uint32 arad_kbp_table_init(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32 table_size,
    SOC_SAND_IN  uint32 table_id)
{
    uint32 res = SOC_SAND_OK;
    ARAD_KBP_TABLE_CONFIG table_config_info = {0};
    enum kbp_db_type db_type;
    ARAD_KBP_DB_HANDLES db_handles_info = {0};
    ARAD_INIT_ELK *elk = &SOC_DPP_CONFIG(unit)->arad->init.elk;
    uint32 core = 0;
    struct kbp_device *arad_kbp_device;
    uint8 need_to_init_table = FALSE;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_KBP_TABLE_CONFIG_clear(&table_config_info);

    if(NULL != AradAppData[unit]->g_gtmInfo[table_id].tblInfo.db_p) {
        ARAD_DO_NOTHING_AND_EXIT;
    }

    res = KBP_INFO.Arad_kbp_table_config_info.get(unit, table_id, &table_config_info); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
    db_type = table_config_info.db_type;

    if (db_type == KBP_DB_INVALID)
    {
        db_type = KBP_DB_ACL;
    }

    
    AradAppData[unit]->g_gtmInfo[table_id].tblInfo.tbl_id        = table_config_info.tbl_id;
    AradAppData[unit]->g_gtmInfo[table_id].tblInfo.tbl_width     = table_config_info.tbl_width;
    AradAppData[unit]->g_gtmInfo[table_id].tblInfo.tbl_size      = table_size;
    AradAppData[unit]->g_gtmInfo[table_id].tblInfo.tbl_assoWidth = table_config_info.tbl_asso_width;
    AradAppData[unit]->g_gtmInfo[table_id].tblInfo.groupId_start = table_config_info.group_id_start;
    AradAppData[unit]->g_gtmInfo[table_id].tblInfo.groupId_end   = table_config_info.group_id_end;

    if(elk->kbp_connect_mode == ARAD_NIF_ELK_TCAM_CONNECT_MODE_DUAL_SMT) {
        arad_kbp_device = AradAppData[unit]->thread_p[core];
    } else {
        arad_kbp_device = AradAppData[unit]->device_p[core];
    }

    if (elk->is_master)
    {
        
        need_to_init_table = TRUE;
    }
    else if(elk->kbp_connect_mode == ARAD_NIF_ELK_TCAM_CONNECT_MODE_DUAL_SHARED)
    {
        
    }
    else
    {
        
        if (table_config_info.clone_of_tbl_id != ARAD_KBP_FRWRD_IP_NOF_TABLES)
        {
            need_to_init_table = TRUE;
        }
        else
        {
            int i = 0;
            for (i=0; i< ARAD_KBP_MAX_NUM_OF_TABLES; i++)
            {
                ARAD_KBP_TABLE_CONFIG clone_table_config_info = { 0 };
                uint8 is_valid;
                res = KBP_INFO.Arad_kbp_table_config_info.get(unit, i, &clone_table_config_info); 
                SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
                res = KBP_INFO.Arad_kbp_table_config_info.valid.get(unit, i, &is_valid); 
                SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
                if ((table_id == clone_table_config_info.clone_of_tbl_id) && (is_valid))
                {
                    need_to_init_table = TRUE;
                    break;
                }
            }
        }
    }

    if (!SOC_WARM_BOOT(unit) && need_to_init_table) {

        
        LOG_VERBOSE(BSL_LS_SOC_TCAM,
                   (BSL_META_U(unit,
                               "%s(): Table [%d] : create DB with width=%d, table-size=%d.\n"),
                               FUNCTION_NAME(),
                    table_config_info.tbl_id,
                    table_config_info.tbl_width,
                    table_config_info.tbl_size));

    
    if (!((soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ext_flexible_mode", 0)) && (AradAppData[unit]->g_gtmInfo[table_id].tblInfo.tbl_id >= ARAD_KBP_FRWRD_IP_NOF_TABLES)))
    {
        if (table_config_info.clone_of_tbl_id == ARAD_KBP_FRWRD_IP_NOF_TABLES) {

                
                res = kbp_db_init(
                        arad_kbp_device, 
                        db_type, 
                        AradAppData[unit]->g_gtmInfo[table_id].tblInfo.tbl_id + ARAD_KBP_DUAL_MODE_QUAD_PORT_TBL_ID_OFFSET,
                        AradAppData[unit]->g_gtmInfo[table_id].tblInfo.tbl_size, 
                        &AradAppData[unit]->g_gtmInfo[table_id].tblInfo.db_p
                  );
            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                LOG_ERROR(BSL_LS_SOC_TCAM,
                        (BSL_META_U(unit,
                                "Error in %s(): kbp_db_init failed: %s, TBL ID %d\n"), 
                                FUNCTION_NAME(), kbp_get_status_string(res), table_id));
                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
            }

            
            if ((AradAppData[unit]->g_gtmInfo[table_id].tblInfo.tbl_assoWidth != NLM_TBL_ADLEN_ZERO)) {
                LOG_VERBOSE(BSL_LS_SOC_TCAM,
                        (BSL_META_U(unit,
                                "%s(): Table [%d] : create AD DB with asso-data=%d.\n"),
                                FUNCTION_NAME(), table_id,
                                ARAD_KBP_AD_WIDTH_TYPE_TO_BITS(AradAppData[unit]->g_gtmInfo[table_id].tblInfo.tbl_assoWidth)));

                    res = kbp_ad_db_init(
                                arad_kbp_device, 
                                AradAppData[unit]->g_gtmInfo[table_id].tblInfo.tbl_id + ARAD_KBP_DUAL_MODE_QUAD_PORT_TBL_ID_OFFSET,
                                AradAppData[unit]->g_gtmInfo[table_id].tblInfo.tbl_size, 
                                ARAD_KBP_AD_WIDTH_TYPE_TO_BITS(AradAppData[unit]->g_gtmInfo[table_id].tblInfo.tbl_assoWidth),
                                &AradAppData[unit]->g_gtmInfo[table_id].tblInfo.ad_db_p);
                if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                    LOG_ERROR(BSL_LS_SOC_TCAM,
                            (BSL_META_U(unit,
                                    "Error in %s(): kbp_ad_db_init failed: %s, TBL ID %d\n"),
                                    FUNCTION_NAME(), kbp_get_status_string(res), AradAppData[unit]->g_gtmInfo[table_id].tblInfo.tbl_id));
                    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 100, exit);
                }


                res = kbp_db_set_ad(AradAppData[unit]->g_gtmInfo[table_id].tblInfo.db_p,
                                      AradAppData[unit]->g_gtmInfo[table_id].tblInfo.ad_db_p);
                if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                    LOG_ERROR(BSL_LS_SOC_TCAM,
                            (BSL_META_U(unit,
                                    "Error in %s(): kbp_db_set_ad failed: %s\n"),
                                    FUNCTION_NAME(), kbp_get_status_string(res)));
                    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 100, exit);
                }

                
                db_handles_info.db_p    = AradAppData[unit]->g_gtmInfo[table_id].tblInfo.db_p;
                db_handles_info.ad_db_p = AradAppData[unit]->g_gtmInfo[table_id].tblInfo.ad_db_p;
                db_handles_info.is_valid = TRUE;
                db_handles_info.table_size = AradAppData[unit]->g_gtmInfo[table_id].tblInfo.tbl_size;
                db_handles_info.table_id = AradAppData[unit]->g_gtmInfo[table_id].tblInfo.tbl_id;
                db_handles_info.table_width = AradAppData[unit]->g_gtmInfo[table_id].tblInfo.tbl_width;
                db_handles_info.table_asso_width = AradAppData[unit]->g_gtmInfo[table_id].tblInfo.tbl_assoWidth;
                if (elk->is_master && elk->kbp_connect_mode == ARAD_NIF_ELK_TCAM_CONNECT_MODE_DUAL_SHARED) {
                    db_temp_handles_info.db_p = db_handles_info.db_p;
                    db_temp_handles_info.ad_db_p = db_handles_info.ad_db_p;
                }

                res = KBP_INFO.db_info.set(unit, table_id, &db_handles_info); 

                SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
            }

            LOG_VERBOSE(BSL_LS_SOC_TCAM,
                    (BSL_META_U(unit,
                            "%s(): Table [%d] : Create new key for each database.\n"),
                            FUNCTION_NAME(), table_config_info.tbl_id));
        }
        else {
            res = kbp_db_clone(AradAppData[unit]->g_gtmInfo[table_config_info.clone_of_tbl_id].tblInfo.db_p,
                         AradAppData[unit]->g_gtmInfo[table_id].tblInfo.tbl_id,
                         &(AradAppData[unit]->g_gtmInfo[table_id].tblInfo.db_p));

            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                LOG_ERROR(BSL_LS_SOC_TCAM,
                        (BSL_META_U(unit,
                                "Error in %s(): kbp_db_clone failed: %s, TBL ID %d\n"),
                                FUNCTION_NAME(), kbp_get_status_string(res), AradAppData[unit]->g_gtmInfo[table_id].tblInfo.tbl_id));
                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 100, exit);
            }

            
            db_handles_info.db_p = AradAppData[unit]->g_gtmInfo[table_id].tblInfo.db_p;
            if (elk->is_master && elk->kbp_connect_mode == ARAD_NIF_ELK_TCAM_CONNECT_MODE_DUAL_SHARED) {
                db_temp_handles_info.db_p = db_handles_info.db_p;
            }
            res = KBP_INFO.db_info.set(unit, table_id, &db_handles_info); 

            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

        }
    }

        res = kbp_key_init(arad_kbp_device, &AradAppData[unit]->g_gtmInfo[table_id].tblInfo.key);

        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            LOG_ERROR(BSL_LS_SOC_TCAM,
                    (BSL_META_U(unit,
                            "Error in %s(): kbp_key_init failed: %s\n"),
                            FUNCTION_NAME(), kbp_get_status_string(res)));
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 100, exit);
        }

        LOG_VERBOSE(BSL_LS_SOC_TCAM,
                (BSL_META_U(unit,
                        "%s(): Table [%d] : Setting ACL database to massively parallel mode.\n"),
                        FUNCTION_NAME(), table_config_info.tbl_id));

        
        if (!((soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ext_flexible_mode", 0)) && (AradAppData[unit]->g_gtmInfo[table_id].tblInfo.tbl_id >= ARAD_KBP_FRWRD_IP_NOF_TABLES)))
        {
            if (!ARAD_KBP_IS_OP_OR_OP2 || (db_type == KBP_DB_ACL) || (ARAD_KBP_IS_OP_OR_OP2 && (db_type == KBP_DB_LPM) && (AradAppData[unit]->g_gtmInfo[table_id].tblInfo.tbl_width > NLM_TBL_WIDTH_160))) {
                res = kbp_db_set_property(AradAppData[unit]->g_gtmInfo[table_id].tblInfo.db_p, KBP_PROP_ALGORITHMIC, 0);
            }
            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                LOG_ERROR(BSL_LS_SOC_TCAM,
                        (BSL_META_U(unit,
                                "Error in %s(): kbp_db_set_property KBP_PROP_ALGORITHMIC failed: %s\n"),
                                FUNCTION_NAME(), kbp_get_status_string(res)));
                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 100, exit);
            }

        

        if (!ARAD_KBP_IS_OP_OR_OP2 && (table_id == ARAD_KBP_FRWRD_TBL_ID_IPV4_DC)) {
            res = kbp_db_set_property(AradAppData[unit]->g_gtmInfo[table_id].tblInfo.db_p,
                                        KBP_PROP_SCALE_UP_CAPACITY, 1);
            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                LOG_ERROR(BSL_LS_SOC_TCAM,
                        (BSL_META_U(unit,
                                "Error in %s(): kbp_db_set_property KBP_PROP_SCALE_UP_CAPACITY failed: %s\n"),
                                FUNCTION_NAME(), kbp_get_status_string(res)));
                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 100, exit);
            }
        }

        if (!ARAD_KBP_IS_OP_OR_OP2 && (table_config_info.min_priority >= 0)) {
            res = kbp_db_set_property(AradAppData[unit]->g_gtmInfo[table_id].tblInfo.db_p, 
                                        KBP_PROP_MIN_PRIORITY, table_config_info.min_priority);
            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                LOG_ERROR(BSL_LS_SOC_TCAM,
                        (BSL_META_U(unit,
                                "Error in %s(): kbp_db_set_property KBP_PROP_MIN_PRIORITY failed: %s\n"),
                                FUNCTION_NAME(), kbp_get_status_string(res)));
                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 100, exit);
            }
        }

        
        if(ARAD_KBP_IS_OP_OR_OP2) {
            int meta_priority = -1;
            if(ARAD_KBP_ENABLE_IPV4_UC_PUBLIC) {
                if(table_id == ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0) {
                    meta_priority = KBP_ENTRY_META_PRIORITY_1;
                } else if(table_id == ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_PUBLIC_RPF_0) {
                    meta_priority = KBP_ENTRY_META_PRIORITY_3;
                }
            }
            if(ARAD_KBP_ENABLE_IPV4_RPF_PUBLIC) {
                if(table_id == ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_1) {
                    meta_priority = KBP_ENTRY_META_PRIORITY_1;
                } else if(table_id == ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_PUBLIC_RPF_1) {
                    meta_priority = KBP_ENTRY_META_PRIORITY_3;
                }
            }
            if(ARAD_KBP_ENABLE_IPV6_UC_PUBLIC) {
                if(table_id == ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_0) {
                    meta_priority = KBP_ENTRY_META_PRIORITY_1;
                } else if(table_id == ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_PUBLIC_RPF_0) {
                    meta_priority = KBP_ENTRY_META_PRIORITY_3;
                }
            }
            if(ARAD_KBP_ENABLE_IPV6_RPF_PUBLIC) {
                if(table_id == ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_1) {
                    meta_priority = KBP_ENTRY_META_PRIORITY_1;
                } else if(table_id == ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_PUBLIC_RPF_1) {
                    meta_priority = KBP_ENTRY_META_PRIORITY_3;
                }
            }

            
            if(meta_priority != -1) {
                res = kbp_db_set_property(AradAppData[unit]->g_gtmInfo[table_id].tblInfo.db_p,
                                            KBP_PROP_ENTRY_META_PRIORITY, meta_priority);
                if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                    LOG_ERROR(BSL_LS_SOC_TCAM,
                            (BSL_META_U(unit,
                                    "Error in %s(): kbp_db_set_property KBP_PROP_ENTRY_META_PRIORITY %d failed: %s\n"),
                                    FUNCTION_NAME(), meta_priority, kbp_get_status_string(res)));
                    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 100, exit);
                }
            }
        }
        

        if (table_id >= ARAD_KBP_ACL_TABLE_ID_OFFSET) {
            res = kbp_db_set_property(AradAppData[unit]->g_gtmInfo[table_id].tblInfo.db_p,
                                        KBP_PROP_MIN_PRIORITY, (4*1024*1024-1));
            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                LOG_ERROR(BSL_LS_SOC_TCAM,
                        (BSL_META_U(unit,
                                "Error in %s(): kbp_db_set_property KBP_PROP_MIN_PRIORITY failed: %s\n"),
                               FUNCTION_NAME(), kbp_get_status_string(res)));
                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 110, exit);
            }
            res = kbp_db_set_property(AradAppData[unit]->g_gtmInfo[table_id].tblInfo.db_p,
                                        KBP_PROP_MAX_PRIORITY, (4*1024*1024-1)-db_handles_info.table_size);
            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                LOG_ERROR(BSL_LS_SOC_TCAM,
                        (BSL_META_U(unit,
                                "Error in %s(): kbp_db_set_property KBP_PROP_MAX_PRIORITY failed: %s\n"),
                               FUNCTION_NAME(), kbp_get_status_string(res)));
                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 120, exit);
            }
        }
    }

        
#ifdef CRASH_RECOVERY_SUPPORT
        if (ARAD_KBP_IS_CR_MODE(unit) == 1) {
            res = kbp_db_set_property(AradAppData[unit]->g_gtmInfo[table_id].tblInfo.db_p, 109, 2);
            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                LOG_ERROR(BSL_LS_SOC_TCAM,
                        (BSL_META_U(unit,
                                "Error in %s(): kbp_db_set_property 109 failed: %s\n"),
                                FUNCTION_NAME(), kbp_get_status_string(res)));
                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 100, exit);
            }
        }
#endif

        ARAD_DO_NOTHING_AND_EXIT;
    }
    else if (!SOC_WARM_BOOT(unit) && !need_to_init_table) {
        
        sal_memcpy(&AradAppData[unit]->g_gtmInfo[table_id], &AradAppData[unit-1]->g_gtmInfo[table_id], sizeof(globalGTMInfo));
        res = KBP_INFO.Arad_kbp_table_config_info.get(unit-1, table_id, &table_config_info); 
        SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
        res = KBP_INFO.Arad_kbp_table_config_info.set(unit, table_id, &table_config_info); 
        SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
        res = KBP_INFO.db_info.get(unit-1, table_id, &db_handles_info); 
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
        res = KBP_INFO.db_info.set(unit, table_id, &db_handles_info); 
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
    }
    else {
        
        res = KBP_INFO.db_info.get(unit, table_id, &db_handles_info);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

        
        if (!((soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ext_flexible_mode", 0)) && (AradAppData[unit]->g_gtmInfo[table_id].tblInfo.tbl_id >= ARAD_KBP_FRWRD_IP_NOF_TABLES)))
        {
            res = kbp_db_refresh_handle(arad_kbp_device, db_handles_info.db_p, &db_handles_info.db_p);
            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                LOG_ERROR(BSL_LS_SOC_TCAM,
                        (BSL_META_U(unit,
                                "Error in %s(): kbp_db_refresh_handle failed: %s\n"),
                                FUNCTION_NAME(), kbp_get_status_string(res)));
                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 100, exit);
            }

            res = kbp_ad_db_refresh_handle(arad_kbp_device, db_handles_info.ad_db_p, &db_handles_info.ad_db_p);
            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                LOG_ERROR(BSL_LS_SOC_TCAM,
                        (BSL_META_U(unit,
                                "Error in %s(): kbp_ad_db_refresh_handle failed: %s\n"),
                                FUNCTION_NAME(), kbp_get_status_string(res)));
                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 100, exit);
            }

            if (elk->is_master) {
                AradAppData[unit]->g_gtmInfo[table_id].tblInfo.db_p = db_handles_info.db_p;
                AradAppData[unit]->g_gtmInfo[table_id].tblInfo.ad_db_p = db_handles_info.ad_db_p;
            } else {
                AradAppData[unit]->g_gtmInfo[table_id].tblInfo.db_p = db_temp_handles_info.db_p;
                AradAppData[unit]->g_gtmInfo[table_id].tblInfo.ad_db_p = db_temp_handles_info.ad_db_p;
            }
            AradAppData[unit]->g_gtmInfo[table_id].tblInfo.tbl_size = db_handles_info.table_size;
            AradAppData[unit]->g_gtmInfo[table_id].tblInfo.tbl_id = db_handles_info.table_id; 
            AradAppData[unit]->g_gtmInfo[table_id].tblInfo.tbl_width = db_handles_info.table_width;
            AradAppData[unit]->g_gtmInfo[table_id].tblInfo.tbl_assoWidth = db_handles_info.table_asso_width;

            res = KBP_INFO.db_info.set(unit, table_id, &db_handles_info);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
        }
    }


exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_table_init()", table_id, table_size);
}

STATIC uint32 arad_kbp_init_static_tables(
    int unit)
{
    int i,tbl_idx;
    int res;
    int num_tbl_valid = 0;

    uint32 table_size, db_type;
    ARAD_KBP_TABLE_CONFIG Arad_kbp_table_config_info = Arad_kbp_dummy_table_config_info_static;

    int num_of_kbp_searches = ARAD_KBP_NUM_OF_KBP_SEARCHES(unit);

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    if (SOC_IS_ARADPLUS(unit)) {
        if(ARAD_KBP_IPV4DC_24BIT_FWD) {
            
            Arad_plus_kbp_table_config_info_static[ARAD_KBP_FRWRD_TBL_ID_IPV4_DC].tbl_asso_width = NLM_TBL_ADLEN_24B;
        }
        for (i = 0; i < ARAD_KBP_FRWRD_IP_NOF_TABLES_ARAD_PLUS; i++) {
            if(ARAD_KBP_IS_OP_OR_OP2 && ARAD_KBP_24BIT_FWD && !ARAD_KBP_ENABLE_IPV4_DC) {
                
                Arad_plus_kbp_table_config_info_static[i].tbl_asso_width = NLM_TBL_ADLEN_24B;
            }
            res = KBP_INFO.Arad_kbp_table_config_info.set(unit, i, &Arad_plus_kbp_table_config_info_static[i]);
            SOC_SAND_CHECK_FUNC_RESULT(res, 200+i, exit);
        }
    }else{
        for (i = 0; i < ARAD_KBP_FRWRD_IP_NOF_TABLES_ARAD; i++) {
            res = KBP_INFO.Arad_kbp_table_config_info.set(unit, i, &Arad_kbp_table_config_info_static[i]);
            SOC_SAND_CHECK_FUNC_RESULT(res, 200+i, exit);
        }
    }

    
    for (i = 0 ; i < num_of_kbp_searches; i++) {
        Arad_kbp_table_config_info.tbl_id = ARAD_KBP_FRWRD_TBL_ID_DUMMY_0+i;
        res = KBP_INFO.Arad_kbp_table_config_info.set(unit, ARAD_KBP_FRWRD_TBL_ID_DUMMY_0+i, &Arad_kbp_table_config_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 200+i, exit);
    }

    
    for ( tbl_idx = 0;tbl_idx < ARAD_KBP_FRWRD_IP_NOF_TABLES; tbl_idx++ )
    {
        
        if(tbl_idx == ARAD_KBP_FRWRD_TBL_ID_DUMMY_FRWRD) {
            continue;
        }

        table_size = SOC_DPP_CONFIG(unit)->arad->init.elk.fwd_table_size[tbl_idx];
        if (table_size > 0x0) 
        {
            
            if (table_size == 1) {
                table_size = 0;
            }

            
            if (SOC_IS_ARADPLUS(unit)) {
                ARAD_KBP_FRWRD_IP_TBL_ID dummy_tbl_to_config = ARAD_KBP_FRWRD_IP_NOF_TABLES_ARAD_PLUS;

                if (tbl_idx == ARAD_KBP_FRWRD_TBL_ID_IPV4_MC) {
                    dummy_tbl_to_config = ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV4_MC;
                }
                if (tbl_idx == ARAD_KBP_FRWRD_TBL_ID_IPV6_MC) {
                    dummy_tbl_to_config = ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV6_MC;
                }
                if (tbl_idx == ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_1) {
                    dummy_tbl_to_config = ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV4_UC;
                }
                if (tbl_idx == ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_1) {
                    dummy_tbl_to_config = ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV6_UC;
                }
                if (tbl_idx == ARAD_KBP_FRWRD_TBL_ID_IPV4_DC) {
                    dummy_tbl_to_config = ARAD_KBP_FRWRD_TBL_ID_DUMMY_IPV4_DC;
                }
                if (dummy_tbl_to_config != ARAD_KBP_FRWRD_IP_NOF_TABLES_ARAD_PLUS) {
                    res = KBP_INFO.Arad_kbp_table_config_info.valid.set(unit, dummy_tbl_to_config, TRUE);
                    SOC_SAND_CHECK_FUNC_RESULT(res, 250, exit);
                    res = KBP_INFO.Arad_kbp_table_config_info.db_type.set(unit, dummy_tbl_to_config, KBP_DB_ACL); 
                    SOC_SAND_CHECK_FUNC_RESULT(res, 250, exit);
                }
            }
            
            res = KBP_INFO.Arad_kbp_table_config_info.valid.set(unit,tbl_idx, TRUE); 
            SOC_SAND_CHECK_FUNC_RESULT(res, 250, exit);
            res = KBP_INFO.Arad_kbp_table_config_info.tbl_size.set(unit, tbl_idx, table_size); 
            SOC_SAND_CHECK_FUNC_RESULT(res, 260, exit);

            if (tbl_idx >= ARAD_KBP_FRWRD_IP_NOF_TABLES || ((tbl_idx >= ARAD_KBP_FRWRD_TBL_ID_DUMMY_0) && (tbl_idx <= ARAD_KBP_FRWRD_TBL_ID_DUMMY_7))) {
                db_type = KBP_DB_ACL; 
            } else if (tbl_idx == ARAD_KBP_FRWRD_TBL_ID_LSR_IP_SHARED) {
                db_type = KBP_DB_ACL;
            } else if (tbl_idx == ARAD_KBP_FRWRD_TBL_ID_IPV4_MC && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ext_ipv4_mc_flexible_fwd_table", 0)) {
                db_type = KBP_DB_ACL;
            } else {
                db_type = KBP_DB_LPM;
            }
            res = KBP_INFO.Arad_kbp_table_config_info.db_type.set(unit, tbl_idx, db_type); 
            SOC_SAND_CHECK_FUNC_RESULT(res, 250, exit);

            
            if (tbl_idx == ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_1) {
                if (soc_property_get(unit, spn_EXT_IP4_FWD_TABLE_SIZE, 0x0) 
                    && (soc_property_get(unit, spn_EXT_IP4_UC_RPF_FWD_TABLE_SIZE, 0x0) == 0)) {
                    res = KBP_INFO.Arad_kbp_table_config_info.tbl_size.set(unit, tbl_idx, 0); 
                    SOC_SAND_CHECK_FUNC_RESULT(res, 270, exit);
                    res = KBP_INFO.Arad_kbp_table_config_info.valid.set(unit, tbl_idx, FALSE); 
                    SOC_SAND_CHECK_FUNC_RESULT(res, 280, exit);
                }
            }
            num_tbl_valid++;
        }
    }

    
    if(num_tbl_valid == 0) {
        res = KBP_INFO.Arad_kbp_table_config_info.valid.set(unit, ARAD_KBP_FRWRD_TBL_ID_DUMMY_FRWRD, TRUE); 
        SOC_SAND_CHECK_FUNC_RESULT(res, 250, exit);
    }

    
    ARAD_KBP_TABLE_CONFIG_clear(&Arad_kbp_table_config_info);
    for ( tbl_idx = ARAD_KBP_ACL_TABLE_ID_OFFSET; tbl_idx < ARAD_KBP_MAX_NUM_OF_TABLES; tbl_idx++) {
        res = KBP_INFO.Arad_kbp_table_config_info.set(unit, tbl_idx, &Arad_kbp_table_config_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 50+tbl_idx, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_init_static_tables()", 0, 0);

}

STATIC uint32 arad_kbp_init_program_db_default_fields(
   int                              unit,
   arad_kbp_lut_data_t              *gtm_lut_info,
   ARAD_KBP_GTM_OPCODE_CONFIG_INFO  *gtm_opcode_config_info)
{

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(gtm_lut_info);
    SOC_SAND_CHECK_NULL_INPUT(gtm_opcode_config_info);

    
    gtm_opcode_config_info->rx_data_type = GTM_OPCODE_CONFIG_DATA_TYPE_RX_REPLY;
    gtm_opcode_config_info->tx_data_type = GTM_OPCODE_CONFIG_DATA_TYPE_TX_REQUEST;

    
    gtm_lut_info->rec_type          = ARAD_KBP_LUT_REC_TYPE_REQUEST;
    gtm_lut_info->rec_is_valid      = 0x1;
    gtm_lut_info->mode              = ARAD_KBP_LUT_INSTR_LUT1_CONTEXID_SEQ_NUM_MODE;
    gtm_lut_info->key_config        = ARAD_KBP_LUT_KEY_CONFIG_SEND_INCOMING_DATA;
    gtm_lut_info->lut_key_data      = 0;
    gtm_lut_info->key_w_cpd_gt_80   = 0;
    gtm_lut_info->copy_data_cfg     = 0;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in kbp_update_db_all_default_fields()", 0,0);

}

STATIC uint32 arad_kbp_init_static_ltr_db(
    int unit)
{
    int res;
    int db_ndx, nof_db, search_ndx;
    int search_to_copy;
    uint8 ltr_valid,tbl_valid;

    ARAD_KBP_LTR_CONFIG  kbp_ltr_config = {0};
    ARAD_KBP_LTR_CONFIG  *kbp_ltr_config_ptr;

    int num_of_kbp_searches = ARAD_KBP_NUM_OF_KBP_SEARCHES(unit);

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (SOC_IS_ARADPLUS(unit)) {
        kbp_ltr_config_ptr = Arad_plus_kbp_ltr_config_static;
        nof_db = ARAD_KBP_FRWRD_DB_NOF_TYPE_ARAD_PLUS;
    }else{
        kbp_ltr_config_ptr = Arad_kbp_ltr_config_static;
        nof_db = ARAD_KBP_FRWRD_DB_NOF_TYPE_ARAD;
    }

    for (db_ndx = 0; db_ndx < nof_db; db_ndx++) {
        kbp_ltr_config = *(kbp_ltr_config_ptr + db_ndx);

        search_to_copy = 0;
        ltr_valid = 1;
        tbl_valid = 0;
        for (search_ndx = 0; search_ndx < num_of_kbp_searches; search_ndx++) {
            
            if (!SHR_BITGET(&(kbp_ltr_config.parallel_srches_bmp), search_ndx)) {
                
                kbp_ltr_config.ltr_search[search_ndx].nof_key_segments = 0;
                kbp_ltr_config.tbl_id[search_ndx] = ARAD_KBP_FRWRD_TBL_ID_DUMMY_0+search_ndx;
            }else{
                
                if ((search_ndx >= ARAD_KBP_CMPR3_FIRST_ACL) && !(SOC_IS_JERICHO(unit))) {
                    SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_MGMT_TCAM_DB_ID_OUT_OF_RANGE_ERR, 20, exit);
                }
                kbp_ltr_config.ltr_search[search_ndx] = kbp_ltr_config_ptr[db_ndx].ltr_search[search_to_copy];
                kbp_ltr_config.tbl_id[search_ndx]     = kbp_ltr_config_ptr[db_ndx].tbl_id[search_to_copy];

                res = KBP_INFO.Arad_kbp_table_config_info.valid.get(unit, kbp_ltr_config_ptr[db_ndx].tbl_id[search_to_copy], &tbl_valid);
                SOC_SAND_CHECK_FUNC_RESULT(res, 290+search_ndx, exit);
                ltr_valid &= tbl_valid;

                search_to_copy++;
            }
        }

        

        
        if (kbp_ltr_config.parallel_srches_bmp == 0) {
            ltr_valid = 0;
        }
        kbp_ltr_config.valid = ltr_valid;

        res = KBP_INFO.Arad_kbp_ltr_config.set(unit, db_ndx, &kbp_ltr_config);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10+db_ndx, exit);
    }

    
    ARAD_KBP_LTR_CONFIG_clear(&kbp_ltr_config);

    for ( db_ndx = ARAD_KBP_FRWRD_DB_ACL_OFFSET; db_ndx < ARAD_KBP_MAX_NUM_OF_FRWRD_DBS; db_ndx++)
    {
        res = KBP_INFO.Arad_kbp_ltr_config.set(unit, db_ndx, &kbp_ltr_config);
        SOC_SAND_CHECK_FUNC_RESULT(res, 50+db_ndx, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_init_static_db()", 0, 0);
}

uint32 arad_kbp_set_all_db_by_ltr(
    int unit,
    int ltr_ndx)
{
    int i, res;
    int db_ndx,search_ndx=0;

    int *result_default_size;
    int *result_default_format;

    int *result_size;
    int *result_format;

    arad_kbp_lut_data_t             kbp_gtm_lut_info = {0};
    ARAD_KBP_LTR_CONFIG             kbp_ltr_config = {0};
    arad_kbp_frwd_ltr_db_t          kbp_gtm_ltr_info = {0};
    ARAD_KBP_GTM_OPCODE_CONFIG_INFO kbp_gtm_opcode_config_info = {0};

    int num_of_kbp_searches = ARAD_KBP_NUM_OF_KBP_SEARCHES(unit);

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (SOC_IS_JERICHO(unit)) {
        result_size   = Jer_kbp_default_result_sizes_static;
        result_format = Jer_kbp_default_result_formats_static;
    }else if (SOC_IS_ARADPLUS(unit)) {
        result_size = Arad_plus_kbp_default_result_sizes_static;
        result_format = Arad_plus_kbp_default_result_formats_static;
    } else {
      result_size = Arad_kbp_default_result_sizes_static;
      result_format = Arad_kbp_default_result_formats_static;
    }

    for (db_ndx = 0 ; db_ndx < ARAD_KBP_MAX_NUM_OF_FRWRD_DBS ; db_ndx++ ) {
        if ((ltr_ndx >= 0) && (db_ndx != ltr_ndx)) {
            continue;
        }
        kbp_gtm_opcode_config_info.rx_data_size = 0;
        kbp_gtm_ltr_info.res_total_data_len = 0;

        res = KBP_INFO.Arad_kbp_ltr_config.get(unit, db_ndx ,&kbp_ltr_config);
        SOC_SAND_CHECK_FUNC_RESULT(res, 290+search_ndx, exit);
        if (kbp_ltr_config.valid == FALSE) {
            if (ltr_ndx == -1) {
                continue;
            }else{
                LOG_ERROR(BSL_LS_SOC_TCAM,
                        (BSL_META_U(unit,
                                "Error in %s(): Trying to set an invalid LTR ID %d\n"), 
                                FUNCTION_NAME(), ltr_ndx));
                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 100, exit);
            }
        }

        if (db_ndx != ARAD_KBP_FRWRD_DB_TYPE_IPV4_DC) {
            result_default_size = result_size;
            result_default_format = result_format;
        }else if (ARAD_KBP_IPV4DC_24BIT_FWD){
            result_default_size = Jer_kbp_default_result_sizes_ipv4_dc_24_static;
            result_default_format = Jer_kbp_default_result_formats_ipv4_dc_24_static;
        }else{
            result_default_size = Jer_kbp_default_result_sizes_ipv4_dc_static;
            result_default_format = Jer_kbp_default_result_formats_ipv4_dc_static;
        }

        for (search_ndx = 0; search_ndx < num_of_kbp_searches; search_ndx++) {
            
            if (!SHR_BITGET(&(kbp_ltr_config.parallel_srches_bmp), search_ndx)) {
                
                kbp_gtm_ltr_info.res_data_len[search_ndx]= SOC_IS_JERICHO(unit) ? result_default_size[search_ndx] : 0;
                kbp_gtm_ltr_info.res_format[search_ndx]  = NLM_ARAD_NO_INDEX_NO_AD;

                
                kbp_gtm_lut_info.result_idx_or_ad[search_ndx]  = ARAD_KBP_LUT_TRANSFER_INDX_ONLY;
                kbp_gtm_lut_info.result_idx_ad_cfg[search_ndx] = ARAD_KBP_LUT_AD_TRANSFER_1B;

            }else{
                
                kbp_gtm_ltr_info.res_data_len[search_ndx] = result_default_size[search_ndx];
                kbp_gtm_ltr_info.res_format[search_ndx]   = result_default_format[search_ndx];
                kbp_gtm_ltr_info.res_total_data_len      += result_default_size[search_ndx];

                
                kbp_gtm_lut_info.result_idx_or_ad[search_ndx]  = ARAD_KBP_LUT_TRANSFER_AD_ONLY;
                kbp_gtm_lut_info.result_idx_ad_cfg[search_ndx] = result_default_size[search_ndx];

                
                kbp_gtm_opcode_config_info.rx_data_size += result_default_size[search_ndx];
            }
        }
        

        
        kbp_gtm_ltr_info.opcode = kbp_ltr_config.opcode;

        if (kbp_gtm_ltr_info.res_total_data_len > 0) {
            kbp_gtm_ltr_info.res_total_data_len += 1;
        }

        
        kbp_gtm_opcode_config_info.tx_data_size = 0;
        for (i=0;i<kbp_ltr_config.master_key_fields.nof_key_segments;i++) {
            kbp_gtm_opcode_config_info.tx_data_size += kbp_ltr_config.master_key_fields.key_segment[i].nof_bytes;
        }
        if (kbp_gtm_opcode_config_info.tx_data_size > 0) {
            kbp_gtm_opcode_config_info.tx_data_size -= 1;
        }

        
        kbp_gtm_lut_info.instr = kbp_ltr_config.is_cmp3_search == NlmTRUE
                                                            ? ARAD_KBP_ROP_LUT_INSTR_CMP3_GET(kbp_ltr_config.ltr_id)
                                                            : ARAD_KBP_ROP_LUT_INSTR_CMP1_GET(kbp_ltr_config.ltr_id) ;

        kbp_gtm_lut_info.rec_size = kbp_gtm_opcode_config_info.tx_data_size + 1;

        res = arad_kbp_init_program_db_default_fields(unit,&kbp_gtm_lut_info,&kbp_gtm_opcode_config_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        res = KBP_INFO.Arad_kbp_gtm_lut_info.set(unit, db_ndx, &kbp_gtm_lut_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 50+db_ndx, exit);
        res = KBP_INFO.Arad_kbp_gtm_ltr_info.set(unit, db_ndx, &kbp_gtm_ltr_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 100+db_ndx, exit);
        res = KBP_INFO.Arad_kbp_gtm_opcode_config_info.set(unit, db_ndx, &kbp_gtm_opcode_config_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 150+db_ndx, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_set_all_db_ltr()", 0, 0);
}


uint32 arad_kbp_sw_init(
    int unit)
{
    int res;
    uint32 db_type,res_idx;
    int num_of_kbp_searches = ARAD_KBP_NUM_OF_KBP_SEARCHES(unit);

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

#ifdef BCM_88660
    if (SOC_IS_ARADPLUS(unit) && (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ext_rpf_fwd_parallel", 0) == 0)){
        if (ARAD_KBP_ENABLE_IPV4_MPLS_EXTENDED) {
            Arad_plus_kbp_table_config_info_static[ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_1].clone_of_tbl_id = ARAD_KBP_FRWRD_IP_NOF_TABLES;
        }
        if (ARAD_KBP_ENABLE_IPV6_EXTENDED){
            Arad_plus_kbp_table_config_info_static[ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_1].clone_of_tbl_id = ARAD_KBP_FRWRD_IP_NOF_TABLES;            
            Arad_plus_kbp_ltr_config_static[ARAD_KBP_FRWRD_DB_TYPE_IPV6_UC].parallel_srches_bmp = 0;
            Arad_plus_kbp_ltr_config_static[ARAD_KBP_FRWRD_DB_TYPE_IPV6_UC_RPF].parallel_srches_bmp = 0;
            Arad_plus_kbp_ltr_config_static[ARAD_KBP_FRWRD_DB_TYPE_IPV6_UC_RPF_2PASS].parallel_srches_bmp = 0x3;
        }

        if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ext_ipv4_mc_flexible_fwd_table", 0)) {
            Arad_plus_kbp_table_config_info_static[ARAD_KBP_FRWRD_TBL_ID_IPV4_MC].tbl_width = NLM_TBL_WIDTH_160;
            Arad_plus_kbp_ltr_config_static[ARAD_KBP_FRWRD_TBL_ID_IPV4_MC].ltr_search[0] = ipv4_mc_extended_search;

            
            Arad_plus_kbp_ltr_config_static[ARAD_KBP_FRWRD_TBL_ID_IPV4_MC].parallel_srches_bmp = 0x7;
            Arad_plus_kbp_ltr_config_static[ARAD_KBP_FRWRD_TBL_ID_IPV4_MC].ltr_search[2] = inrif_mapping_search; 
            Arad_plus_kbp_ltr_config_static[ARAD_KBP_FRWRD_TBL_ID_IPV4_MC].tbl_id[2] = ARAD_KBP_FRWRD_TBL_ID_INRIF_MAPPING;
        }
    }
#endif 

    res = arad_kbp_init_static_tables(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = arad_kbp_init_static_ltr_db(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = arad_kbp_set_all_db_by_ltr(unit, -1);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    sal_memset(arad_kbp_frwd_ltr_db, 0x0, sizeof(arad_kbp_frwd_ltr_db_t) * ARAD_KBP_ROP_LTR_NUM_MAX);

    for ( db_type = 0; db_type < ARAD_KBP_ROP_LTR_NUM_MAX; db_type++) {
        
        for ( res_idx = 0; res_idx < num_of_kbp_searches; res_idx++) {
            arad_kbp_frwd_ltr_db[db_type].res_format[res_idx] = NLM_ARAD_NO_INDEX_NO_AD;
        }
    }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_sw_init()", 0, 0);
}

STATIC uint8 arad_kbp_result_sizes_is_default(
    int                         unit,
    int                         *result_actual_sizes,
    ARAD_KBP_FRWRD_IP_DB_TYPE   db_type)
{
    int i;
    int *result_default_size;

    if (db_type == ARAD_KBP_FRWRD_DB_TYPE_IPV4_DC) {
        if(ARAD_KBP_IPV4DC_24BIT_FWD) {
            result_default_size = Jer_kbp_default_result_sizes_ipv4_dc_24_static;
        } else {
            result_default_size = Jer_kbp_default_result_sizes_ipv4_dc_static;
        }
    } else {
        result_default_size = Jer_kbp_default_result_sizes_static;
    }

    for (i = 0; i < ARAD_PP_FLP_KBP_MAX_NUMBER_OF_RESULTS; i++) {
        if (result_actual_sizes[i] != result_default_size[i]) {
            return FALSE;
        }
    }
    return TRUE;
}


uint32 arad_kbp_result_sizes_configurations_init(
    int                         unit,
    ARAD_KBP_FRWRD_IP_DB_TYPE   db_type,
    int                         results[ARAD_PP_FLP_KBP_MAX_NUMBER_OF_RESULTS])
{
    int i, res;
    int total_size_valid_lookups = 0;
    uint8 is_default, parallel_bitmap_get;
    uint32 parallel_bitmap[1];
    NlmBool is_cmp3;
    int results_size[ARAD_PP_FLP_KBP_MAX_NUMBER_OF_RESULTS];

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (db_type > ARAD_KBP_FRWRD_DB_NOF_TYPES) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                (BSL_META_U(unit,
                        "Error in %s(): invalid db_type %d\n"),
                        FUNCTION_NAME(), db_type));
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 30, exit);
    }

    is_default = arad_kbp_result_sizes_is_default(unit, results, db_type);
    if (is_default) {
        ARAD_DO_NOTHING_AND_EXIT;
    }

    for (i = 0; i < ARAD_PP_FLP_KBP_MAX_NUMBER_OF_RESULTS; i++) {
        if (results[i] > 15) {
            LOG_ERROR(BSL_LS_SOC_TCAM,
                    (BSL_META_U(unit,
                            "Error in %s(): Result_size %d not valid for search %d\n"),
                            FUNCTION_NAME(), results[i], i));
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 30, exit);
        }
    }

    for (i=0;i<ARAD_PP_FLP_KBP_MAX_NUMBER_OF_RESULTS;i++) {
        results_size[i] = results[i];
    }

    if (ARAD_KBP_ENABLE_IPV4_DC) {
        if(db_type == ARAD_KBP_FRWRD_DB_TYPE_IPV4_DC) {
            results_size[1] += ARAD_KBP_IPV4DC_RES1_PAD_BYTES;
            results_size[2] += ARAD_KBP_IPV4DC_RES3_PAD_BYTES;
        } else {
            results_size[0] += ARAD_KBP_IPV4DC_RES1_PAD_BYTES;
            results_size[3] += ARAD_KBP_IPV4DC_RES3_PAD_BYTES;
        }
    }

    res = KBP_INFO.Arad_kbp_ltr_config.parallel_srches_bmp.get(unit,db_type,&parallel_bitmap_get);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    parallel_bitmap[0] = (uint32)parallel_bitmap_get;
    
    for (i = 0; i < ARAD_PP_FLP_KBP_MAX_NUMBER_OF_RESULTS; i++) {
        if (SHR_BITGET(parallel_bitmap, i)) {
            res = KBP_INFO.Arad_kbp_gtm_lut_info.result_idx_ad_cfg.set(unit, db_type, i, results_size[i]);
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
            res = KBP_INFO.Arad_kbp_gtm_lut_info.result_idx_or_ad.set(unit, db_type, i, ARAD_KBP_LUT_TRANSFER_AD_ONLY);
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        }
    }

    for (i = 0; i < ARAD_PP_FLP_KBP_MAX_NUMBER_OF_RESULTS; i++) {
        res = KBP_INFO.Arad_kbp_gtm_ltr_info.res_data_len.set(unit, db_type, i, results_size[i]);
        SOC_SAND_CHECK_FUNC_RESULT(res, 100+i, exit);
        if (SHR_BITGET(parallel_bitmap, i)) {
            total_size_valid_lookups += results_size[i];
            if (results_size[i] <= 4) {
                res = KBP_INFO.Arad_kbp_gtm_ltr_info.res_format.set(unit, db_type, i, NLM_ARAD_INDEX_AND_32B_AD);
                SOC_SAND_CHECK_FUNC_RESULT(res, 100+i, exit);
            } else if (results_size[i] <= 8) {
                res = KBP_INFO.Arad_kbp_gtm_ltr_info.res_format.set(unit, db_type, i, NLM_ARAD_INDEX_AND_64B_AD);
                SOC_SAND_CHECK_FUNC_RESULT(res, 100+i, exit);
            } else {
                res = KBP_INFO.Arad_kbp_gtm_ltr_info.res_format.set(unit, db_type, i, NLM_ARAD_INDEX_AND_128B_AD);
                SOC_SAND_CHECK_FUNC_RESULT(res, 100+i, exit);
            }
        } else {
            res = KBP_INFO.Arad_kbp_gtm_ltr_info.res_format.set(unit, db_type, i, NLM_ARAD_ONLY_INDEX_NO_AD);
            SOC_SAND_CHECK_FUNC_RESULT(res, 100+i, exit);
        }
    }

    res = KBP_INFO.Arad_kbp_gtm_ltr_info.res_total_data_len.set(unit, db_type, (1 + total_size_valid_lookups)); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);

    res = KBP_INFO.Arad_kbp_ltr_config.is_cmp3_search.get(unit,db_type,&is_cmp3);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    
    res = KBP_INFO.Arad_kbp_gtm_opcode_config_info.rx_data_size.set(unit, db_type, total_size_valid_lookups); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_result_sizes_configurations_init", 0, db_type);
}



STATIC int arad_kbp_init_nlm_dev_mgr(
   int unit,
   int core)
{
    int i;
    uint32 flags;
    kbp_warmboot_t *warmboot_data;
    enum kbp_device_type kbp_type;
    int32 res;
    ARAD_INIT_ELK *elk = &SOC_DPP_CONFIG(unit)->arad->init.elk;
    struct kbp_device *arad_kbp_device = NULL;
#ifdef CRASH_RECOVERY_SUPPORT
    unsigned int nv_mem_size = ARAD_KBP_NV_MEMORY_SIZE;
#endif

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    warmboot_data = &kbp_warmboot_data[unit];

    flags = KBP_DEVICE_DEFAULT | KBP_DEVICE_ISSU;
    if (SOC_WARM_BOOT(unit)) {
        flags |= KBP_DEVICE_SKIP_INIT;
#ifdef CRASH_RECOVERY_SUPPORT
        if (ARAD_KBP_IS_CR_MODE(unit) == 1) {
            flags &= ~KBP_DEVICE_ISSU;
        }
#endif
    }
    if (ARAD_KBP_IS_OP_OR_OP2) {
        AradAppData[unit]->app_config_done = FALSE;
        if (core == 1) {
            
            return 0;
        }
        if (ARAD_KBP_IS_OP) {
            kbp_type = KBP_DEVICE_OP;
        } else {
            kbp_type = KBP_DEVICE_OP2;
        }
        if(ARAD_KBP_24BIT_FWD && !ARAD_KBP_ENABLE_IPV4_DC) {
            flags |= KBP_DEVICE_SUPPRESS_INDEX;
        }
        if (elk->kbp_connect_mode == ARAD_NIF_ELK_TCAM_CONNECT_MODE_SINGLE) {
            flags |= KBP_DEVICE_DUAL_PORT;
        } else if(elk->kbp_connect_mode == ARAD_NIF_ELK_TCAM_CONNECT_MODE_DUAL_SHARED){
            flags |= KBP_DEVICE_QUAD_PORT;
        } else { 
            flags |= KBP_DEVICE_QUAD_PORT | KBP_DEVICE_SMT;
        }
    }
    else {
        kbp_type = KBP_DEVICE_12K;
    }
    if (elk->is_master) {
        if(!SAL_BOOT_PLISIM){
            res = kbp_device_init(AradAppData[unit]->dalloc_p[core],
                                kbp_type, 
                                flags, 
                                AradAppData[unit]->alg_kbp_xpt_p[core], 
                                NULL,
                                &AradAppData[unit]->device_p[core]);
        } else{
            res = kbp_device_init(AradAppData[unit]->dalloc_p[core], 
                                    kbp_type, 
                                    flags, 
                                    NULL, 
                                    NULL,
                                    &AradAppData[unit]->device_p[core]);
        }
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK){
            LOG_ERROR(BSL_LS_SOC_TCAM,
                    (BSL_META_U(unit,
                            "Error in %s(): kbp_device_init failed: %s\n"),
                            FUNCTION_NAME(), kbp_get_status_string(res)));
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 100, exit);
        }

        arad_kbp_device = AradAppData[unit]->device_p[core];

        if(ARAD_KBP_OP_IS_DUAL_MODE) {
            AradAppData[unit+1]->device_p[core] = AradAppData[unit]->device_p[core];

            
            if(elk->kbp_connect_mode == ARAD_NIF_ELK_TCAM_CONNECT_MODE_DUAL_SMT) {
                struct kbp_device *tmp_dev_thr = NULL;
                for(i = 0; i < ARAD_KBP_NUM_OF_SMT_THREADS; i++) {
                    res = kbp_device_thread_init(AradAppData[unit]->device_p[core], i, &(tmp_dev_thr));
                    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK){
                        LOG_ERROR(BSL_LS_SOC_TCAM,
                                (BSL_META_U(unit,
                                        "Error in %s(): kbp_device_thread_init thread %d failed: %s\n"),
                                        FUNCTION_NAME(), i, kbp_get_status_string(res)));
                        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 100, exit);
                    }
                    AradAppData[unit+i]->thread_p[core] = tmp_dev_thr;
                }
                arad_kbp_device = AradAppData[unit]->thread_p[core];
            }

        }
    } else {
        if(ARAD_KBP_OP_IS_DUAL_MODE) {
            
            while(AradAppData[unit-1]->app_config_done == FALSE);
            if(elk->kbp_connect_mode == ARAD_NIF_ELK_TCAM_CONNECT_MODE_DUAL_SMT) {
                arad_kbp_device = AradAppData[unit]->thread_p[core];
            } else {
                arad_kbp_device = AradAppData[unit]->device_p[core];
            }
        } else {
            LOG_ERROR(BSL_LS_SOC_TCAM,
                    (BSL_META_U(unit,
                            "Error in %s(): Slave unit in single connect mode\n"),
                            FUNCTION_NAME()));
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 100, exit);
        }
    }

    
    if (!ARAD_KBP_IS_OP_OR_OP2 && SOC_IS_ARADPLUS_AND_BELOW(unit)){
        res = kbp_device_set_property(AradAppData[unit]->device_p[core], KBP_DEVICE_PROP_INST_LATENCY, 1);
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK){
            LOG_ERROR(BSL_LS_SOC_TCAM,
                    (BSL_META_U(unit,
                            "Error in %s(): kbp_device_set_property KBP_DEVICE_PROP_INST_LATENCY failed: %s\n"),
                            FUNCTION_NAME(), kbp_get_status_string(res)));
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 100, exit);
        }
    }

    
    if (!ARAD_KBP_IS_OP_OR_OP2 && ((core > 0) && (SOC_DPP_DEFS_MAX(NOF_CORES) == 2))) {
        res = kbp_device_set_property(AradAppData[unit]->device_p[0], 
                                        KBP_DEVICE_ADD_BROADCAST, 
                                        1, 
                                        AradAppData[unit]->device_p[core]);
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK){
            LOG_ERROR(BSL_LS_SOC_TCAM,
                    (BSL_META_U(unit,
                            "Error in %s(): kbp_device_set_property KBP_DEVICE_ADD_BROADCAST failed: %s\n"),
                            FUNCTION_NAME(), kbp_get_status_string(res)));
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 100, exit);
        }
        
        if (soc_property_get(unit, spn_KBP_MESSAGE_BROADCAST_ENABLE, 0)) {
            res = kbp_device_set_property(AradAppData[unit]->device_p[0],
                                            KBP_DEVICE_BROADCAST_AT_XPT, 1);
            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK){
                LOG_ERROR(BSL_LS_SOC_TCAM,
                        (BSL_META_U(unit,
                                "Error in %s(): kbp_device_set_property KBP_DEVICE_ADD_BROADCAST_AT_XPT failed: %s\n"),
                                FUNCTION_NAME(), kbp_get_status_string(res)));
                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 100, exit);
            }
        }
    }

#ifdef CRASH_RECOVERY_SUPPORT
    if (ARAD_KBP_IS_CR_MODE(unit) == 1) {
        
        if(core == 0) {
            if(ha_mem_alloc(unit, HA_KBP_Mem_Pool, HA_KBP_SUB_ID_0, &nv_mem_size, (void**)&kbp_nv_mem_ptr) != SOC_E_NONE) {
                return SOC_E_INTERNAL;
            }
            SOC_SAND_CHECK_NULL_PTR(kbp_nv_mem_ptr,20,exit);

            if (SOC_WARM_BOOT(unit)){
                res = kbp_device_set_property(arad_kbp_device, KBP_DEVICE_PROP_CRASH_RECOVERY, 0, kbp_nv_mem_ptr, ARAD_KBP_NV_MEMORY_SIZE);
                if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK){
                    LOG_ERROR(BSL_LS_SOC_TCAM,
                            (BSL_META_U(unit,
                                    "Error in %s(): kbp_device_set_property KBP_DEVICE_PROP_CRASH_RECOVERY 0 failed: %s\n"),
                                    FUNCTION_NAME(), kbp_get_status_string(res)));
                    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 90, exit);
                }
                res = kbp_device_restore_state(arad_kbp_device, NULL, NULL, NULL);
                if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK){
                    LOG_ERROR(BSL_LS_SOC_TCAM,
                            (BSL_META_U(unit,
                                    "Error in %s(): kbp_device_restore_state failed: %s\n"),
                                    FUNCTION_NAME(), kbp_get_status_string(res)));
                    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 90, exit);
                }
            } else {
                res = kbp_device_set_property(arad_kbp_device, KBP_DEVICE_PROP_CRASH_RECOVERY, 1, kbp_nv_mem_ptr, ARAD_KBP_NV_MEMORY_SIZE);
                if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK){
                    LOG_ERROR(BSL_LS_SOC_TCAM,
                            (BSL_META_U(unit,
                                    "Error in %s(): kbp_device_set_property KBP_DEVICE_PROP_CRASH_RECOVERY 1 failed: %s\n"),
                                    FUNCTION_NAME(), kbp_get_status_string(res)));
                    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 90, exit);
                }
            }
        }
    } else
#endif
    {
        if (SOC_WARM_BOOT(unit) && (core == 0)) {
            res = kbp_device_restore_state(arad_kbp_device, warmboot_data->kbp_file_read , warmboot_data->kbp_file_write, warmboot_data->kbp_file_fp);
            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                LOG_ERROR(BSL_LS_SOC_TCAM,
                        (BSL_META_U(unit,
                                "Error in %s(): kbp_device_restore_state failed: %s\n"), 
                                FUNCTION_NAME(), kbp_get_status_string(res)));
                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 90, exit);
            }
        }
    }

    ARAD_DO_NOTHING_AND_EXIT;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_init_nlm_dev_mgr()", 0, 0);
}

uint32 arad_kbp_init_nlm_app_set(
   SOC_SAND_IN int unit,
   SOC_SAND_IN uint32 core)
{
    uint32 res;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (ARAD_KBP_IS_OP_OR_OP2) {
        if (core == 1) {
            
            SOC_EXIT;
        }
    }

    
    res = arad_kbp_init_nlm_dev_mgr(unit, core);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_init_nlm_app_set()", 
                                  1, 1);
}

STATIC uint32 arad_kbp_deinit_nlm_app_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 second_kbp_supported,
    int no_sync_flag)
{
    uint core = 0;
    int res;
    ARAD_INIT_ELK *elk = &SOC_DPP_CONFIG(unit)->arad->init.elk;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    if (elk->is_master) {
        if (!no_sync_flag)
        {
            res = kbp_device_destroy(AradAppData[unit]->device_p[core]);
            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                LOG_ERROR(BSL_LS_SOC_TCAM,
                        (BSL_META_U(unit,
                                "Error in %s(): kbp_device_destroy failed: %s\n"), 
                                FUNCTION_NAME(), kbp_get_status_string(res)));
                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 100, exit);
            }
        }
        else
        {
            res = default_allocator_destroy(AradAppData[unit]->dalloc_p[core]);
            if(second_kbp_supported) {
                res |= default_allocator_destroy(AradAppData[unit]->dalloc_p[1]);
            }
            res |= default_allocator_create(&AradAppData[unit]->dalloc_p[core]);
            if(second_kbp_supported) {
                res |= default_allocator_create(&AradAppData[unit]->dalloc_p[1]);
            }
            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                LOG_ERROR(BSL_LS_SOC_TCAM,
                        (BSL_META_U(unit,
                                "Error in %s(): default_allocator_create/destroy failed: %s\n"), 
                                FUNCTION_NAME(), kbp_get_status_string(res)));
                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 100, exit);
            }
        }
    }
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_deinit_nlm_app_set()", 0, 0);
}

STATIC int arad_kbp_device_lock_config(
    SOC_SAND_IN  int unit)
{
    uint32 res = SOC_SAND_OK;
    uint32 core = 0;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = kbp_device_set_property(AradAppData[unit]->device_p[core], KBP_DEVICE_PRE_CLEAR_ABS, 1);
    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                (BSL_META_U(unit,
                        "Error in %s(): kbp_device_set_property KBP_DEVICE_PRE_CLEAR_ABS failed: %s\n"), 
                        FUNCTION_NAME(), kbp_get_status_string(res)));
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 100, exit);
    }

    
    if (!(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ext_flexible_mode", 0))) {

        res = kbp_device_lock(AradAppData[unit]->device_p[core]);
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK)
        {
            LOG_ERROR(BSL_LS_SOC_TCAM,
                    (BSL_META_U(unit,
                            "Error in %s(): kbp_device_lock failed: %s\n"),
                            FUNCTION_NAME(), kbp_get_status_string(res)));
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 100, exit);
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_device_lock_config()", 0, 0);
}

STATIC uint32 arad_kbp_key_init(
    SOC_SAND_IN int                         unit,
    SOC_SAND_IN ARAD_KBP_FRWRD_IP_DB_TYPE   gtm_db_type)
{
    uint32
        srch_ndx,
        table_idx,
        key_ndx,
        table_key_offset;

    uint32_t res;
    uint32 core=0;

    ARAD_KBP_LTR_CONFIG ltr_config_info = {0};
    ARAD_INIT_ELK *elk = &SOC_DPP_CONFIG(unit)->arad->init.elk;
    struct kbp_device *arad_kbp_device = NULL;

    int num_of_kbp_searches = ARAD_KBP_NUM_OF_KBP_SEARCHES(unit);

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_KBP_LTR_CONFIG_clear(&ltr_config_info);

    if(elk->kbp_connect_mode == ARAD_NIF_ELK_TCAM_CONNECT_MODE_DUAL_SMT) {
        arad_kbp_device = AradAppData[unit]->thread_p[core];
    } else {
        arad_kbp_device = AradAppData[unit]->device_p[core];
    }

    if (!SOC_WARM_BOOT(unit)) {

        if(!elk->is_master && (elk->kbp_connect_mode == ARAD_NIF_ELK_TCAM_CONNECT_MODE_DUAL_SHARED)) {
            
            res = KBP_INFO.Arad_kbp_ltr_config.get(unit-1, gtm_db_type, &ltr_config_info); 
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
            res = KBP_INFO.Arad_kbp_ltr_config.set(unit, gtm_db_type, &ltr_config_info); 
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
            ARAD_DO_NOTHING_AND_EXIT;
        }

        res = KBP_INFO.Arad_kbp_ltr_config.get(unit, gtm_db_type, &ltr_config_info); 
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);       

        
        res = kbp_key_init(arad_kbp_device, &ltr_config_info.master_key);
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            LOG_ERROR(BSL_LS_SOC_TCAM,
                    (BSL_META_U(unit,
                            "Error in %s(): kbp_key_init failed: %s\n"), 
                            FUNCTION_NAME(), kbp_get_status_string(res)));
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 90, exit);
        }

        res = KBP_INFO.Arad_kbp_ltr_config.set(unit, gtm_db_type, &ltr_config_info); 
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

        
        if (!((gtm_db_type >= ARAD_KBP_FRWRD_DB_NOF_TYPES) && (ltr_config_info.valid) && (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ext_flexible_mode", 0))))
        {
            for(srch_ndx = 0; srch_ndx < num_of_kbp_searches; srch_ndx++)
            {
                if (SHR_BITGET(&ltr_config_info.parallel_srches_bmp, srch_ndx)) {
                    table_idx = ltr_config_info.tbl_id[srch_ndx];

                    for(key_ndx = 0;
                         key_ndx < ltr_config_info.ltr_search[srch_ndx].nof_key_segments;
                         key_ndx++)
                    {
                        
                        if(ltr_config_info.ltr_search[srch_ndx].key_segment[key_ndx].nof_bytes){
                            if(!(AradAppData[unit]->g_gtmInfo[table_idx].tblInfo.is_key_adde_to_db)){
                                int type = ltr_config_info.ltr_search[srch_ndx].key_segment[key_ndx].type;
                                if(ARAD_KBP_IS_OP_OR_OP2) {
                                    if(type != KBP_KEY_FIELD_RANGE && sal_strcmp("VRF", ltr_config_info.ltr_search[srch_ndx].key_segment[key_ndx].name)) {
                                        type = KBP_KEY_FIELD_EM;
                                    }
                                } else {
                                    if(type != KBP_KEY_FIELD_RANGE && type != KBP_KEY_FIELD_HOLE) {
                                        type = KBP_KEY_FIELD_PREFIX;
                                    }
                                }
                                ltr_config_info.ltr_search[srch_ndx].key_segment[key_ndx].type = type;
                                res = kbp_key_add_field(
                                        AradAppData[unit]->g_gtmInfo[table_idx].tblInfo.key,
                                        ltr_config_info.ltr_search[srch_ndx].key_segment[key_ndx].name,
                                        (ltr_config_info.ltr_search[srch_ndx].key_segment[key_ndx].nof_bytes * 8),
                                        type
                                      );

                                if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                                    if(res != KBP_DUPLICATE_KEY_FIELD) {
                                        LOG_ERROR(BSL_LS_SOC_TCAM,
                                                (BSL_META_U(unit,
                                                        "Error in %s(): DB Key : kbp_key_add_field with failed: %s, LTR ID %d, search %d, Table %d, Segment %s, Bytes %d, Type %d\n"),
                                                        FUNCTION_NAME(), kbp_get_status_string(res), gtm_db_type, srch_ndx, table_idx, ltr_config_info.ltr_search[srch_ndx].key_segment[key_ndx].name, ltr_config_info.ltr_search[srch_ndx].key_segment[key_ndx].nof_bytes, type));
                                        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
                                    }
                                }
                            }
                        }
                    }

                    if(!(AradAppData[unit]->g_gtmInfo[table_idx].tblInfo.is_key_adde_to_db)){


                        res = kbp_db_set_key(
                                AradAppData[unit]->g_gtmInfo[table_idx].tblInfo.db_p,
                                AradAppData[unit]->g_gtmInfo[table_idx].tblInfo.key
                              );

                        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                            LOG_ERROR(BSL_LS_SOC_TCAM,
                                    (BSL_META_U(unit,
                                            "Error in %s(): DB Key : kbp_db_set_key with failed: %s, search %d, TBL ID %d, LTR ID %d\n"), 
                                            FUNCTION_NAME(), kbp_get_status_string(res), srch_ndx, table_idx, gtm_db_type));
                            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 20, exit);
                        }

                        AradAppData[unit]->g_gtmInfo[table_idx].tblInfo.is_key_adde_to_db = 1;
                        AradAppData[unit]->g_gtmInfo[table_idx].tblInfo.dummy_segment.nof_bytes = ltr_config_info.ltr_search[srch_ndx].key_segment[0].nof_bytes;
                        sal_memcpy(
                            AradAppData[unit]->g_gtmInfo[table_idx].tblInfo.dummy_segment.name, 
                            ltr_config_info.ltr_search[srch_ndx].key_segment[0].name, 
                            sizeof(ltr_config_info.ltr_search[srch_ndx].key_segment[0].name)
                        );
                    }
                }
            }
        }

        table_key_offset = 0;
        for(key_ndx = 0;
             key_ndx < ltr_config_info.master_key_fields.nof_key_segments;
             key_ndx++)
        {
            
            if(ltr_config_info.master_key_fields.key_segment[key_ndx].nof_bytes){
                int type = ltr_config_info.master_key_fields.key_segment[key_ndx].type;
                if(ARAD_KBP_IS_OP_OR_OP2) {
                    if(type != KBP_KEY_FIELD_RANGE && sal_strcmp("VRF", ltr_config_info.master_key_fields.key_segment[key_ndx].name)) {
                        type = KBP_KEY_FIELD_EM;
                    }
                } else {
                    if(type != KBP_KEY_FIELD_RANGE && type != KBP_KEY_FIELD_HOLE) {
                        type = KBP_KEY_FIELD_PREFIX;
                    }
                }
                ltr_config_info.master_key_fields.key_segment[key_ndx].type = type;

                
                if (!(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ext_flexible_mode", 0)))
                {
                    res = kbp_key_add_field(
                            ltr_config_info.master_key,
                            ltr_config_info.master_key_fields.key_segment[key_ndx].name,
                            (ltr_config_info.master_key_fields.key_segment[key_ndx].nof_bytes*8),
                            type
                          );
                }

                if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                    if(res != KBP_DUPLICATE_KEY_FIELD) {
                        LOG_ERROR(BSL_LS_SOC_TCAM,
                                (BSL_META_U(unit,
                                        "Error in %s(): DB Master Key Resp Code = %d, kbp_key_add_field failed: %s\n"), 
                                        FUNCTION_NAME(), res, kbp_get_status_string(res)));
                        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 30, exit);
                    }
                }

                table_key_offset += ltr_config_info.master_key_fields.key_segment[key_ndx].nof_bytes;
            }
        }

        if ((table_key_offset % 10) != 0) 
        {
            table_key_offset = (10 - (table_key_offset % 10));

            
            if (!(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ext_flexible_mode", 0)))
            {
                res = kbp_key_add_field(
                        ltr_config_info.master_key,
                        "DUMMY",
                        table_key_offset*8,
                        KBP_KEY_FIELD_PREFIX
                      );
            }

            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                if(res != KBP_DUPLICATE_KEY_FIELD) {
                    LOG_ERROR(BSL_LS_SOC_TCAM,
                            (BSL_META_U(unit,
                                    "Error in %s(): DB Master Key Resp Code = %d, kbp_key_add_field failed: %s\n"), 
                                    FUNCTION_NAME(), res, kbp_get_status_string(res)));
                    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 30, exit);
                }
            }
        }

        for(srch_ndx = 0; srch_ndx < num_of_kbp_searches; srch_ndx++)
        {
            if (SHR_BITGET(&ltr_config_info.parallel_srches_bmp, srch_ndx)) {
                table_idx = ltr_config_info.tbl_id[srch_ndx];
                if ((table_idx >= ARAD_KBP_FRWRD_TBL_ID_DUMMY_0) && (table_idx <= ARAD_KBP_FRWRD_TBL_ID_DUMMY_3)) {
                    

                    
                    if (!(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ext_flexible_mode", 0)))
                    {
                        res = kbp_key_add_field(
                                ltr_config_info.master_key,
                                AradAppData[unit]->g_gtmInfo[table_idx].tblInfo.dummy_segment.name,
                                AradAppData[unit]->g_gtmInfo[table_idx].tblInfo.dummy_segment.nof_bytes*8,
                                KBP_KEY_FIELD_PREFIX
                              );
                    }

                    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                        if(res != KBP_DUPLICATE_KEY_FIELD) {
                            LOG_ERROR(BSL_LS_SOC_TCAM,
                                    (BSL_META_U(unit,
                                            "Error in %s(): DB Master Key Resp Code = %d, kbp_key_add_field failed: %s\n"), 
                                            FUNCTION_NAME(), res, kbp_get_status_string(res)));
                            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 33, exit);
                        }
                    }
                }
            }
        }
    }

    ARAD_DO_NOTHING_AND_EXIT;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_key_init()", gtm_db_type, 0x0);
}

STATIC uint32 arad_kbp_search_init(
    SOC_SAND_IN int                         unit,
    SOC_SAND_IN ARAD_KBP_FRWRD_IP_DB_TYPE   gtm_db_type)
{
    uint32
        srch_ndx, res;
    uint32 core=0;

    ARAD_KBP_LTR_CONFIG ltr_config_info = {0};

    ARAD_KBP_LTR_HANDLES
        ltr_handles_info = {0};

    int num_of_kbp_searches = ARAD_KBP_NUM_OF_KBP_SEARCHES(unit);
    ARAD_INIT_ELK *elk = &SOC_DPP_CONFIG(unit)->arad->init.elk;
    struct kbp_device *arad_kbp_device = NULL;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_KBP_LTR_CONFIG_clear(&ltr_config_info);

    if(elk->kbp_connect_mode == ARAD_NIF_ELK_TCAM_CONNECT_MODE_DUAL_SMT) {
        arad_kbp_device = AradAppData[unit]->thread_p[core];
    } else {
        arad_kbp_device = AradAppData[unit]->device_p[core];
    }


    if (!SOC_WARM_BOOT(unit)) {

        if(!elk->is_master && (elk->kbp_connect_mode == ARAD_NIF_ELK_TCAM_CONNECT_MODE_DUAL_SHARED)) {
            
            res = KBP_INFO.Arad_kbp_ltr_config.get(unit-1, gtm_db_type, &ltr_config_info); 
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
            res = KBP_INFO.Arad_kbp_ltr_config.set(unit, gtm_db_type, &ltr_config_info); 
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
            ARAD_DO_NOTHING_AND_EXIT;
        }

        res = KBP_INFO.Arad_kbp_ltr_config.get(unit, gtm_db_type, &ltr_config_info); 
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        
        LOG_VERBOSE(BSL_LS_SOC_TCAM,
                (BSL_META_U(unit,
                        "%s(): DB Type [%d] : create a new instruction, LTR ID %d.\n"),
                        FUNCTION_NAME(), gtm_db_type, ltr_config_info.ltr_id));

        res = kbp_instruction_init(
                        arad_kbp_device, 
                        ltr_config_info.ltr_id + ARAD_KBP_DUAL_MODE_QUAD_PORT_LTR_ID_OFFSET,
                        ltr_config_info.ltr_id + ARAD_KBP_DUAL_MODE_QUAD_PORT_LTR_ID_OFFSET, 
                        &ltr_config_info.inst_p);

            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                LOG_ERROR(BSL_LS_SOC_TCAM,
                        (BSL_META_U(unit,
                                "Error in %s(): kbp_instruction_init failed: %s, LTR ID %d\n"),
                                FUNCTION_NAME(), kbp_get_status_string(res), ltr_config_info.ltr_id + ARAD_KBP_DUAL_MODE_QUAD_PORT_LTR_ID_OFFSET));
                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 100, exit);
            }

        
        res = KBP_INFO.Arad_kbp_ltr_config.set(unit, gtm_db_type, &ltr_config_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        ltr_handles_info.inst_p = ltr_config_info.inst_p;

        res = KBP_INFO.ltr_info.set(unit, gtm_db_type, &ltr_handles_info); 
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

        
        if (!(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ext_flexible_mode", 0)))
        {
            res = kbp_instruction_set_key(ltr_config_info.inst_p, ltr_config_info.master_key);
            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                LOG_ERROR(BSL_LS_SOC_TCAM,
                        (BSL_META_U(unit,
                                "Error in %s(): kbp_instruction_set_key failed: %s, LTR ID %d\n"),
                                FUNCTION_NAME(), kbp_get_status_string(res), gtm_db_type));
                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 100, exit);
            }
        }

        
        if (!(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ext_flexible_mode", 0)))
        {
            for(srch_ndx = 0; srch_ndx < num_of_kbp_searches; srch_ndx++){
                if (SHR_BITGET(&ltr_config_info.parallel_srches_bmp, srch_ndx)) {
                    res = kbp_instruction_add_db(ltr_config_info.inst_p,
                                                    AradAppData[unit]->g_gtmInfo[ltr_config_info.tbl_id[srch_ndx]].tblInfo.db_p,
                                                    srch_ndx);
                    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                        LOG_ERROR(BSL_LS_SOC_TCAM,
                                (BSL_META_U(unit,
                                        "Error in %s(): kbp_instruction_add_db failed: %s, LTR ID %d, search %d\n"),
                                        FUNCTION_NAME(), kbp_get_status_string(res), gtm_db_type, srch_ndx));
                        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
                    }
                }
            }
        }

        

        
        if (!(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ext_flexible_mode", 0)))
        {
            if(ARAD_KBP_IS_OP_OR_OP2) {
                if(gtm_db_type == ARAD_KBP_FRWRD_DB_TYPE_IPV4_UC_PUBLIC || gtm_db_type == ARAD_KBP_FRWRD_DB_TYPE_IPV4_UC_PUBLIC_RPF) {
                    res = kbp_instruction_add_db(ltr_config_info.inst_p,
                            AradAppData[unit]->g_gtmInfo[ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0].tblInfo.db_p, 0);
                    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                        LOG_ERROR(BSL_LS_SOC_TCAM,
                            (BSL_META_U(unit,
                                    "Error in %s(): kbp_instruction_add_db failed: %s, LTR ID %d, public search %d\n"),
                                    FUNCTION_NAME(), kbp_get_status_string(res), gtm_db_type, 0));
                        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
                    }

                    if(gtm_db_type == ARAD_KBP_FRWRD_DB_TYPE_IPV4_UC_PUBLIC_RPF) {
                        res = kbp_instruction_add_db(ltr_config_info.inst_p,
                                AradAppData[unit]->g_gtmInfo[ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_1].tblInfo.db_p, 2);
                        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                            LOG_ERROR(BSL_LS_SOC_TCAM,
                                (BSL_META_U(unit,
                                        "Error in %s(): kbp_instruction_add_db failed: %s, LTR ID %d, public search %d\n"),
                                        FUNCTION_NAME(), kbp_get_status_string(res), gtm_db_type, 2));
                            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
                        }
                    }
                }

                if(gtm_db_type == ARAD_KBP_FRWRD_DB_TYPE_IPV6_UC_PUBLIC || gtm_db_type == ARAD_KBP_FRWRD_DB_TYPE_IPV6_UC_PUBLIC_RPF) {
                    res = kbp_instruction_add_db(ltr_config_info.inst_p,
                            AradAppData[unit]->g_gtmInfo[ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_0].tblInfo.db_p, 0);
                    if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                        LOG_ERROR(BSL_LS_SOC_TCAM,
                            (BSL_META_U(unit,
                                    "Error in %s(): kbp_instruction_add_db failed: %s, LTR ID %d, public search %d\n"),
                                    FUNCTION_NAME(), kbp_get_status_string(res), gtm_db_type, 0));
                        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
                    }

                    if(gtm_db_type == ARAD_KBP_FRWRD_DB_TYPE_IPV6_UC_PUBLIC_RPF) {
                        res = kbp_instruction_add_db(ltr_config_info.inst_p,
                                AradAppData[unit]->g_gtmInfo[ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_1].tblInfo.db_p, 2);
                        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                            LOG_ERROR(BSL_LS_SOC_TCAM,
                                (BSL_META_U(unit,
                                        "Error in %s(): kbp_instruction_add_db failed: %s, LTR ID %d, public search %d\n"),
                                        FUNCTION_NAME(), kbp_get_status_string(res), gtm_db_type, 2));
                            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
                        }
                    }
                }
            }
        }

        
        if (!(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ext_flexible_mode", 0)))
        {
            res = kbp_instruction_install(ltr_config_info.inst_p);
            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                LOG_ERROR(BSL_LS_SOC_TCAM,
                        (BSL_META_U(unit,
                                "Error in %s(): kbp_instruction_install failed: %s, LTR ID %d\n"),
                                FUNCTION_NAME(), kbp_get_status_string(res), gtm_db_type));
                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
            }
        }
    }
    else { 
        
        res = KBP_INFO.ltr_info.get(unit, gtm_db_type, &ltr_handles_info); 
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

        res = kbp_instruction_refresh_handle(arad_kbp_device, ltr_handles_info.inst_p, &ltr_handles_info.inst_p);
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            LOG_ERROR(BSL_LS_SOC_TCAM,
                    (BSL_META_U(unit,
                            "Error in %s(): kbp_instruction_refresh_handle failed: %s\n"),
                            FUNCTION_NAME(), kbp_get_status_string(res)));
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 100, exit);
        }

        res = KBP_INFO.ltr_info.set(unit, gtm_db_type, &ltr_handles_info); 
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);
    }

    ARAD_DO_NOTHING_AND_EXIT;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_search_init()", gtm_db_type, 0x0);
}


uint32 arad_kbp_frwrd_opcode_set(
    int unit,
    uint8 opcode,
    uint32 tx_data_size, 
    uint32 tx_data_type, 
    uint32 rx_data_size, 
    uint32 rx_data_type)
{
    uint32
        res;
    uint32
        table_entry[1];

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHB_OPCODE_MAP_TXm(unit, MEM_BLOCK_ANY, opcode, table_entry));
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, SIZEf, tx_data_size);
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, TYPEf, tx_data_type);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_IHB_OPCODE_MAP_TXm(unit, MEM_BLOCK_ANY, opcode, table_entry));

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_IHB_OPCODE_MAP_RXm(unit, MEM_BLOCK_ANY, opcode, table_entry));
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, SIZEf, rx_data_size);
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, TYPEf, rx_data_type);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, WRITE_IHB_OPCODE_MAP_RXm(unit, MEM_BLOCK_ANY, opcode, table_entry));


exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_frwrd_opcode_set()", opcode, tx_data_size);
}

uint32 arad_kbp_frwrd_ip_rpf_lut_set(
    int unit,
    ARAD_KBP_INSTRUCTION inst_p,
    uint8 opcode,
    uint32 ltr,
    uint32 gtm_table_id,
    uint32 second_kbp_supported)
{
    uint32
        res = SOC_SAND_OK;
    int tmp_unit;
    arad_kbp_lut_data_t lut_data = {0};
    uint32 core = 0;
    arad_kbp_frwd_ltr_db_t  Arad_kbp_gtm_ltr_info = {0};

    ARAD_INIT_ELK* elk = &SOC_DPP_CONFIG(unit)->arad->init.elk;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    arad_kbp_frwd_ltr_db_clear(&Arad_kbp_gtm_ltr_info);

    
    tmp_unit = (!elk->is_master && (elk->kbp_connect_mode == ARAD_NIF_ELK_TCAM_CONNECT_MODE_DUAL_SHARED)) ? (unit - 1) : unit;
    res = KBP_INFO.Arad_kbp_gtm_lut_info.get(tmp_unit, gtm_table_id, &lut_data); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    res = KBP_INFO.Arad_kbp_gtm_ltr_info.get(tmp_unit, gtm_table_id, &Arad_kbp_gtm_ltr_info); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    

    sal_memcpy( &(arad_kbp_frwd_ltr_db[ltr]), &(Arad_kbp_gtm_ltr_info), sizeof(arad_kbp_frwd_ltr_db_t));

	if (!SOC_WARM_BOOT(unit)) {
		SOC_DPP_CORES_ITER(_SHR_CORE_ALL, core) {
			if ((core > 0) && (!second_kbp_supported)) {
				goto exit;
			}
			if (ARAD_KBP_IS_OP_OR_OP2) {
                
				if (!(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ext_flexible_mode", 0)))
				{
					res = kbp_instruction_set_opcode(inst_p, opcode);
				}
			} else {
                                
				res = arad_kbp_lut_write(unit, core, opcode, &lut_data, NULL);
			}
			SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
			res = KBP_INFO.Arad_kbp_gtm_lut_info.set(unit, gtm_table_id, &lut_data);
			SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
		}
	}

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_frwrd_ip_rpf_lut_set()", opcode, ltr);
}

uint32 arad_kbp_frwrd_ip_opcode_set(
    int unit,
    uint8 opcode,
    uint32 gtm_table_id)
{
    uint32
        res;
    int tmp_unit;

    ARAD_KBP_GTM_OPCODE_CONFIG_INFO Arad_kbp_gtm_opcode_config_info = {0};

    ARAD_INIT_ELK* elk = &SOC_DPP_CONFIG(unit)->arad->init.elk;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    tmp_unit = (!elk->is_master && (elk->kbp_connect_mode == ARAD_NIF_ELK_TCAM_CONNECT_MODE_DUAL_SHARED)) ? (unit - 1) : unit;
    res = KBP_INFO.Arad_kbp_gtm_opcode_config_info.get(tmp_unit, gtm_table_id, &Arad_kbp_gtm_opcode_config_info); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if (!SOC_WARM_BOOT(unit)) {
        
        res = arad_kbp_frwrd_opcode_set(
            unit, 
            opcode,
            Arad_kbp_gtm_opcode_config_info.tx_data_size,
            Arad_kbp_gtm_opcode_config_info.tx_data_type,
            Arad_kbp_gtm_opcode_config_info.rx_data_size,
            Arad_kbp_gtm_opcode_config_info.rx_data_type
            );
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_frwrd_ip4_rpf_opcode_set()", opcode, gtm_table_id);
}

uint32 arad_kbp_init_db_set(
    int unit,
    uint32 second_kbp_supported,
    ARAD_INIT_ELK *elk)
{
    uint32
        tbl_idx,
        db_type,
        res,
        tbl_size,
        tbl_width;

    nlm_u8 clone_of_tbl_id;

#ifdef CRASH_RECOVERY_SUPPORT
    uint32 overall_capacity_taken = 0;
    uint32 max_available_capacity = 1 << 20; 
#endif

    nlm_u8 is_valid, tbl_id;
    uint8 ranges_allocated = FALSE;

    ARAD_KBP_DB_HANDLES db_handles_info;
    ARAD_KBP_LTR_CONFIG ltr_config_info = {0};

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    res = KBP_INFO.Arad_kbp_acl_range.is_allocated(unit, &ranges_allocated);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
    if(!ranges_allocated) {
        int max_num_ranges = soc_property_get(unit, spn_KBP_MAX_NUM_RANGES, 64);
        res = KBP_INFO.Arad_kbp_max_num_ranges.set(unit, max_num_ranges);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
        res = KBP_INFO.Arad_kbp_acl_range.alloc(unit, max_num_ranges);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
    }

    ARAD_KBP_LTR_CONFIG_clear(&ltr_config_info);

    
    for (tbl_idx = 0; tbl_idx < ARAD_KBP_MAX_NUM_OF_TABLES; tbl_idx++) 
    {
        if (SOC_WARM_BOOT(unit)) {
            res = KBP_INFO.db_info.get(unit, tbl_idx, &db_handles_info);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

            is_valid = db_handles_info.is_valid;
            
            tbl_id = tbl_idx;
            tbl_size = db_handles_info.table_size;
        } 
        else {
            
            int tmp_unit = (!elk->is_master && (elk->kbp_connect_mode == ARAD_NIF_ELK_TCAM_CONNECT_MODE_DUAL_SHARED)) ? (unit - 1) : unit;
            res = KBP_INFO.Arad_kbp_table_config_info.valid.get(tmp_unit, tbl_idx, &is_valid); 
            SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
            res = KBP_INFO.Arad_kbp_table_config_info.tbl_id.get(tmp_unit, tbl_idx, &tbl_id); 
            SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
            res = KBP_INFO.Arad_kbp_table_config_info.tbl_size.get(tmp_unit, tbl_idx, &tbl_size); 
            SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);    
            res = KBP_INFO.Arad_kbp_table_config_info.tbl_width.get(tmp_unit, tbl_idx, &tbl_width);
            SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);   
            res = KBP_INFO.Arad_kbp_table_config_info.clone_of_tbl_id.get(tmp_unit, tbl_idx, &clone_of_tbl_id);
            SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);

            res = KBP_INFO.db_info.get(unit, tbl_idx, &db_handles_info);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
        }

        if (is_valid) 
        {
            int i;
            res = arad_kbp_table_init(
                    unit, 
                    tbl_size, 
                    tbl_id
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
            
            res = KBP_INFO.db_info.nof_range_qualifiers.set(unit, tbl_idx, db_handles_info.nof_range_qualifiers);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
            for(i = 0; i < db_handles_info.nof_range_qualifiers; i++) {
                res = KBP_INFO.db_info.range_qualifier.set(unit, tbl_idx, i, &(db_handles_info.range_qualifier[i]));
                SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
            }


#ifdef CRASH_RECOVERY_SUPPORT
            if (ARAD_KBP_IS_CR_MODE(unit) == 1 && clone_of_tbl_id == ARAD_KBP_FRWRD_IP_NOF_TABLES) {
                
                overall_capacity_taken += tbl_size * (tbl_width / NLM_TBL_WIDTH_80);
            }
#endif
        }
    }

#ifdef CRASH_RECOVERY_SUPPORT
    if (ARAD_KBP_IS_CR_MODE(unit) == 1) {
        if(overall_capacity_taken < max_available_capacity) {
            uint32 rem_cap = max_available_capacity - overall_capacity_taken;
            LOG_CLI((BSL_META_U(unit,"Warning: Not all available KBP capacity is utilized. It will remain unusable.\n")));
            LOG_CLI((BSL_META_U(unit,"         Remaining capacity is enough for %d 80b, %d 160b or %d 320b entries.\n"), rem_cap, rem_cap / 2, rem_cap / 4));
        }
    }
#endif

    
    for(db_type = 0; db_type < ARAD_KBP_MAX_NUM_OF_FRWRD_DBS; db_type++) 
    {
        res = KBP_INFO.Arad_kbp_ltr_config.get(unit, db_type, &ltr_config_info); 
        SOC_SAND_CHECK_FUNC_RESULT(res, 500+db_type, exit);
        if (ltr_config_info.valid || (!elk->is_master && (elk->kbp_connect_mode == ARAD_NIF_ELK_TCAM_CONNECT_MODE_DUAL_SHARED))) {
            
            res = arad_kbp_key_init(
                    unit, 
                    db_type
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 100, ltr_print_exit);
        }
    }

    
    for(db_type = 0; db_type < ARAD_KBP_MAX_NUM_OF_FRWRD_DBS; db_type++) 
    {
        res = KBP_INFO.Arad_kbp_ltr_config.get(unit, db_type, &ltr_config_info); 
        SOC_SAND_CHECK_FUNC_RESULT(res, 600+db_type, exit);
        if (ltr_config_info.valid || (!elk->is_master && (elk->kbp_connect_mode == ARAD_NIF_ELK_TCAM_CONNECT_MODE_DUAL_SHARED))) 
        {
            LOG_VERBOSE(BSL_LS_SOC_TCAM,
                    (BSL_META_U(unit,
                            "%s(): Configure DB Type %s, LTR=%d(0x%x), Opcode = %d\n"), 
                            FUNCTION_NAME(), ARAD_KBP_FRWRD_IP_DB_TYPE_to_string(db_type), ltr_config_info.ltr_id, ltr_config_info.ltr_id, ltr_config_info.opcode));

            
            res = arad_kbp_search_init(
                    unit, 
                    db_type
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 100, ltr_print_exit);

            res = KBP_INFO.Arad_kbp_ltr_config.get(unit, db_type, &ltr_config_info); 
            SOC_SAND_CHECK_FUNC_RESULT(res, 600+db_type, exit);

            
            res = arad_kbp_frwrd_ip_rpf_lut_set(
                    unit, 
                    ltr_config_info.inst_p,
                    ltr_config_info.opcode,
                    ltr_config_info.ltr_id,
                    db_type, 
                    second_kbp_supported
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 100, ltr_print_exit);

            
            res = arad_kbp_frwrd_ip_opcode_set(
                    unit, 
                    ltr_config_info.opcode,
                    db_type
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 100, ltr_print_exit);
        }
    }
    if (!SOC_WARM_BOOT(unit) && ARAD_KBP_IS_OP_OR_OP2 && (elk->is_master || (elk->kbp_connect_mode == ARAD_NIF_ELK_TCAM_CONNECT_MODE_DUAL_SMT))) {
        for(db_type = 0; db_type < ARAD_KBP_MAX_NUM_OF_FRWRD_DBS; db_type++) {
            uint32 srch_ndx;
            arad_kbp_lut_data_t kbp_gtm_lut_info = {0};
            int num_of_kbp_searches = ARAD_KBP_NUM_OF_KBP_SEARCHES(unit);

            res = KBP_INFO.Arad_kbp_gtm_lut_info.get(unit, db_type, &kbp_gtm_lut_info);
            SOC_SAND_CHECK_FUNC_RESULT(res, 600+db_type, exit);
            res = KBP_INFO.Arad_kbp_ltr_config.get(unit, db_type, &ltr_config_info);
            SOC_SAND_CHECK_FUNC_RESULT(res, 600+db_type, exit);
            if (ltr_config_info.valid) {
                LOG_VERBOSE(BSL_LS_SOC_TCAM,
                        (BSL_META_U(unit,
                                "%s(): Configure DB Type %s, LTR=%d(0x%x), Opcode = %d\n"),
                                FUNCTION_NAME(), ARAD_KBP_FRWRD_IP_DB_TYPE_to_string(db_type),
                                ltr_config_info.ltr_id, ltr_config_info.ltr_id, ltr_config_info.opcode));

                
                if (!((db_type >= ARAD_KBP_FRWRD_DB_NOF_TYPES) && (ltr_config_info.valid) && (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ext_flexible_mode", 0))))
                {
                    for(srch_ndx = 0; srch_ndx < num_of_kbp_searches; srch_ndx++) {
                        if (SHR_BITGET(&ltr_config_info.parallel_srches_bmp, srch_ndx)) {
                            uint32 ad_size = kbp_gtm_lut_info.result_idx_ad_cfg[srch_ndx];
                            res = kbp_instruction_set_property(ltr_config_info.inst_p, KBP_INST_PROP_RESULT_DATA_LEN, srch_ndx, 0, ad_size);
                            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                                LOG_ERROR(BSL_LS_SOC_TCAM,
                                        (BSL_META_U(unit,
                                                "Error in %s(): kbp_instruction_set_property failed: %s\n"),
                                                FUNCTION_NAME(), kbp_get_status_string(res)));
                                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 100, exit);
                                }
                        }
                    }
                }
            }
        }
    }
    if(ARAD_KBP_OP_IS_DUAL_MODE) {
        int unit_offset;
        AradAppData[unit]->app_config_done = TRUE;
        if (elk->is_master) {
            unit_offset = 1;
        } else {
            unit_offset = -1;
        }
        if(AradAppData[unit + unit_offset]->app_config_done == TRUE) {
            res = arad_kbp_device_lock_config(unit);
            SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
        }
    } else {
        res = arad_kbp_device_lock_config(unit);
        SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
    }

    ARAD_DO_NOTHING_AND_EXIT;

ltr_print_exit:
    ARAD_KBP_LTR_CONFIG_print(unit, &ltr_config_info, 2 );

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_init_db_set()", 0, 0);
}

#ifdef CRASH_RECOVERY_SUPPORT
uint32 arad_kbp_cr_transaction_cmd(
   int unit,
   uint8 is_start)
{
    int core = 0;
    int res = SOC_SAND_OK;
    ARAD_INIT_ELK *elk = &SOC_DPP_CONFIG(unit)->arad->init.elk;
    struct kbp_device *arad_kbp_device = NULL;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if(elk->kbp_connect_mode == ARAD_NIF_ELK_TCAM_CONNECT_MODE_DUAL_SMT) {
        arad_kbp_device = AradAppData[unit]->thread_p[core];
    } else {
        arad_kbp_device = AradAppData[unit]->device_p[core];
    }

    if (arad_kbp_device != NULL) {
        if (is_start) {
            res = kbp_device_start_transaction(arad_kbp_device);
            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                LOG_ERROR(BSL_LS_SOC_TCAM,
                        (BSL_META_U(unit,
                                "Error in %s(): kbp_device_start_transaction failed: %s\n"),
                                FUNCTION_NAME(), kbp_get_status_string(res)));
                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 100, exit);
            }
        } else { 
            res = kbp_device_end_transaction(arad_kbp_device);
            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                LOG_ERROR(BSL_LS_SOC_TCAM,
                        (BSL_META_U(unit,
                                "Error in %s(): kbp_device_end_transaction failed: %s\n"),
                                FUNCTION_NAME(), kbp_get_status_string(res)));
                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 100, exit);
            }
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_cr_transaction_cmd()", 0, 0);
}

uint8 arad_kbp_cr_query_restore_status(
    int unit)
{
    int core = 0;
    int res;
    uint8 result=TRUE;

    enum kbp_restore_status res_status;

    ARAD_INIT_ELK *elk = &SOC_DPP_CONFIG(unit)->arad->init.elk;
    struct kbp_device *arad_kbp_device = NULL;

    if(elk->kbp_connect_mode == ARAD_NIF_ELK_TCAM_CONNECT_MODE_DUAL_SMT) {
        arad_kbp_device = AradAppData[unit]->thread_p[core];
    } else {
        arad_kbp_device = AradAppData[unit]->device_p[core];
    }

    if(arad_kbp_device != NULL){
        res = kbp_device_query_restore_status(arad_kbp_device, &res_status);
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            LOG_ERROR(BSL_LS_SOC_TCAM,
                    (BSL_META_U(unit,
                            "Error in %s(): kbp_device_query_restore_status failed: %s\n"),
                            FUNCTION_NAME(), kbp_get_status_string(res)));
        }
    }
    if (res_status == KBP_RESTORE_CHANGES_ABORTED) {
        result = FALSE;
    }

    return result;
}

uint32 arad_kbp_cr_clear_restore_status(
    int unit)
{
    int core=0;
    int res = SOC_SAND_OK;

    ARAD_INIT_ELK *elk = &SOC_DPP_CONFIG(unit)->arad->init.elk;
    struct kbp_device *arad_kbp_device = NULL;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if(elk->kbp_connect_mode == ARAD_NIF_ELK_TCAM_CONNECT_MODE_DUAL_SMT) {
        arad_kbp_device = AradAppData[unit]->thread_p[core];
    } else {
        arad_kbp_device = AradAppData[unit]->device_p[core];
    }

    if(arad_kbp_device != NULL){
        res = kbp_device_clear_restore_status(arad_kbp_device);
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            LOG_ERROR(BSL_LS_SOC_TCAM,
                      (BSL_META_U(unit, "Error in %s(): kbp_device_clear_restore_status failed: %s\n"),
                      FUNCTION_NAME(), kbp_get_status_string(res)));
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_cr_clear_restore_status()", 0, 0);
}

uint32 arad_kbp_cr_db_commit(
    int    unit,
    uint32 tbl_id)
{
    struct kbp_db *db_p = NULL;
    uint32 res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = arad_kbp_alg_kbp_db_get(unit, tbl_id, &db_p);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    if (db_p != NULL) {
        res = kbp_db_install(db_p);
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
            LOG_ERROR(BSL_LS_SOC_TCAM,
                    (BSL_META_U(unit,
                            "Error in %s(): kbp_db_install failed: %s\n"),
                            FUNCTION_NAME(), kbp_get_status_string(res)));
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 20, exit);
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_db_commit()", 0, 0);
}
#endif

uint32 arad_kbp_db_commit(
   int unit)
{
    struct kbp_db 
        *db_p = NULL;
    uint32
        tbl_idx,
        res;

    nlm_u8 is_valid = FALSE;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    for (tbl_idx = 0; tbl_idx < ARAD_KBP_MAX_NUM_OF_TABLES; tbl_idx++) 
    {
        res = KBP_INFO.Arad_kbp_table_config_info.valid.get(unit, tbl_idx, &is_valid); 
        SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
        if (is_valid) 
        {
            res = arad_kbp_alg_kbp_db_get(unit, tbl_idx, &db_p);
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
#ifdef ARAD_PP_KBP_TIME_MEASUREMENTS
            soc_sand_ll_timer_set("ARAD_KBP_TIMERS_ROUTE_ADD_COMMIT", ARAD_KBP_TIMERS_ROUTE_ADD_COMMIT);
#endif
            res = kbp_db_install(db_p);
#ifdef ARAD_PP_KBP_TIME_MEASUREMENTS
            soc_sand_ll_timer_stop(ARAD_KBP_TIMERS_ROUTE_ADD_COMMIT);
#endif 
            if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK) {
                LOG_ERROR(BSL_LS_SOC_TCAM,
                        (BSL_META_U(unit,
                                "Error in %s(): kbp_db_install failed: %s\n"),
                                FUNCTION_NAME(), kbp_get_status_string(res)));
                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 20, exit);
            }
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_db_commit()", 0, 0);
}


uint32 arad_kbp_init_app(
    int             unit,
    uint32          second_kbp_supported,
    ARAD_INIT_ELK   *elk_ptr)
{
    uint32 res = 0;
    uint32 core;
    uint32 iter_count = (second_kbp_supported ? 2:1);
    ARAD_INIT_ELK *elk;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (elk_ptr == NULL) {
        elk = &SOC_DPP_CONFIG(unit)->arad->init.elk;
    } else {
        elk = elk_ptr;
    }

    if (elk->enable == 0x0) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                (BSL_META_U(unit,
                        "Error in %s(): ELK disabled (ext_tcam_dev_type might be NONE)!!!\n"),
                        FUNCTION_NAME()));
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
    }

    for (core = 0; core < iter_count; core++) {
        
        if(!SAL_BOOT_PLISIM){
            
            if ((ARAD_KBP_IS_OP) && !SOC_WARM_BOOT(unit)) {
                res = arad_kbp_op_link_stability_check(unit, core, elk->kbp_user_data[0].kbp_mdio_id, elk->kbp_recover_iter);
                SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
            }

            if (!ARAD_KBP_IS_OP2) {
                res = arad_kbp_init_rop_test(unit, core);
            }
            
            if (res != 0 && elk->kbp_recover_enable) {
                res = arad_kbp_recover_run_recovery_sequence(unit, core, elk->kbp_user_data, elk->kbp_recover_iter, NULL, 1);
            }
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
        }    

        if (!SOC_WARM_BOOT(unit)) {
            if (!ARAD_KBP_IS_OP2) {
                res = arad_kbp_blk_lut_set(unit, core);
            }
            if (res != 0 && elk->kbp_recover_enable) {
                res = arad_kbp_recover_run_recovery_sequence(unit, core, elk->kbp_user_data, elk->kbp_recover_iter, NULL, 2);
            }
            SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
        }

        
        res = arad_kbp_init_nlm_app_set(unit, core);
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    }


    
    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ext_flexible_mode", 0) && !ARAD_KBP_IS_OP_OR_OP2) {
		LOG_ERROR(BSL_LS_SOC_TCAM,
				(BSL_META_U(unit,
						"Error in %s(): ext_tcam_dev_type has to be BCM52311 or BCM52321\n"),
						FUNCTION_NAME()));
		SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
    }

    
    res = arad_kbp_init_db_set(unit, second_kbp_supported, elk);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_init_app()", elk->enable, elk->tcam_dev_type);
}

uint32 arad_kbp_deinit_app(
    int     unit,
    uint32  second_kbp_supported,
    int     no_sync_flag)
{
    int res;
    ARAD_INIT_ELK *elk = &SOC_DPP_CONFIG(unit)->arad->init.elk;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if(NULL == AradAppData[unit])
    {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                (BSL_META_U(unit,
                        "Error in %s(): kbp device is not initialized\n"),
                        FUNCTION_NAME()));
        ARAD_DO_NOTHING_AND_EXIT;
    }else{

#ifdef CRASH_RECOVERY_SUPPORT
        if (ARAD_KBP_IS_CR_MODE(unit) == 1) {
            res = arad_kbp_deinit_nlm_app_set(unit, second_kbp_supported, no_sync_flag);
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        }
        else
#endif
        {
            res = arad_kbp_deinit_nlm_app_set(unit, second_kbp_supported, 0);
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        }

        if (elk->is_master) {
            sal_memset(AradAppData[unit]->g_gtmInfo, 0x0, ARAD_KBP_MAX_NUM_OF_TABLES * sizeof(globalGTMInfo));
            if (ARAD_KBP_OP_IS_DUAL_MODE && (AradAppData[unit + 1]->g_gtmInfo != NULL)) {
                sal_memset(AradAppData[unit + 1]->g_gtmInfo, 0x0, ARAD_KBP_MAX_NUM_OF_TABLES * sizeof(globalGTMInfo));
            }
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_deinit_app()", unit, 0x0);
}

int arad_kbp_autosync_set(
    int unit,
    int enable)
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in kbp_sync()", 0, 0);
}

int arad_kbp_sync(
    int unit)
{
    kbp_warmboot_t *warmboot_data;
    uint32 core=0;
    int res;
    ARAD_INIT_ELK *elk = &SOC_DPP_CONFIG(unit)->arad->init.elk;
    struct kbp_device *arad_kbp_device = NULL;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if(elk->kbp_connect_mode == ARAD_NIF_ELK_TCAM_CONNECT_MODE_DUAL_SMT) {
        arad_kbp_device = AradAppData[unit]->thread_p[core];
    } else {
        arad_kbp_device = AradAppData[unit]->device_p[core];
    }


#ifdef CRASH_RECOVERY_SUPPORT
    if(!(ARAD_KBP_IS_CR_MODE(unit)))
#endif
    {
        warmboot_data = &kbp_warmboot_data[unit];

        res = kbp_device_save_state_and_continue(arad_kbp_device, warmboot_data->kbp_file_read , warmboot_data->kbp_file_write, warmboot_data->kbp_file_fp);
        if(ARAD_KBP_TO_SOC_RESULT(res) != SOC_SAND_OK){
            LOG_ERROR(BSL_LS_SOC_TCAM,
                    (BSL_META_U(unit,
                            "Error in %s(): kbp_device_save_state_and_continue failed: %s\n"),
                            FUNCTION_NAME(), kbp_get_status_string(res)));
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 100, exit);
        }
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in kbp_sync()", 0, 0);
}

void arad_kbp_warmboot_register(
    int                         unit,
    FILE                        *file_fp,
    kbp_device_issu_read_fn     read_fn,
    kbp_device_issu_write_fn    write_fn)
{
    kbp_warmboot_data[unit].kbp_file_fp = file_fp;
    kbp_warmboot_data[unit].kbp_file_read = read_fn;
    kbp_warmboot_data[unit].kbp_file_write = write_fn;
}


#include <soc/dpp/SAND/Utils/sand_footer.h>
#endif 

