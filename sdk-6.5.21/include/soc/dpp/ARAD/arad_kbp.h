/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#if defined(INCLUDE_KBP) && !defined(__ARAD_KBP_INCLUDED__)

#define __ARAD_KBP_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>



#include <shared/swstate/sw_state_workarounds.h>

#include <soc/kbp/alg_kbp/include/db.h>
#include <soc/kbp/alg_kbp/include/default_allocator.h>
#include <soc/kbp/alg_kbp/include/device.h>
#include <soc/kbp/alg_kbp/include/key.h>
#include <soc/kbp/alg_kbp/include/instruction.h>
#include <soc/kbp/alg_kbp/include/errors.h>
#include <soc/kbp/alg_kbp/include/ad.h>
#include <soc/kbp/alg_kbp/include/kbp_legacy.h>
#include <soc/kbp/alg_kbp/include/init.h>
#include <soc/kbp/alg_kbp/include/kbp_pcie.h>
#include <soc/hwstate/hw_log.h>

#include <soc/dpp/ARAD/arad_api_nif.h>
#include <soc/dpp/ARAD/arad_kbp_rop.h>
#include <soc/dpp/ARAD/arad_sw_db_tcam_mgmt.h>

#include <soc/dpp/TMC/tmc_api_tcam.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_flp_init.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_fp.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_fp_key.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_dbal.h>
#include <soc/dpp/PPC/ppc_api_fp.h>







#define ARAD_KBP_NL_88650_MAX_NOF_ENTRIES (SOC_TMC_TCAM_NL_88650_MAX_NOF_ENTRIES)


#define KBP_PORT_STABLE_TIMEOUT 2000000

#define ARAD_KBP_ENABLE_FRWRD_TABLE(frwrd_table)     \
    (SOC_DPP_CONFIG(unit)->arad->init.elk.enable   \
     && (0 != SOC_DPP_CONFIG(unit)->arad->init.elk.frwrd_table)) 

#define arad_kbp_alloc(s,x) sal_alloc((s*x),__FILE__)
#define arad_kbp_free(s) sal_free(s)

#define ARAD_KBP_ENABLE_IPV4_UC             ARAD_KBP_ENABLE_FRWRD_TABLE(fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_0])
#define ARAD_KBP_ENABLE_IPV4_MC             ARAD_KBP_ENABLE_FRWRD_TABLE(fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV4_MC])
#define ARAD_KBP_ENABLE_IPV6_UC             ARAD_KBP_ENABLE_FRWRD_TABLE(fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_0])
#define ARAD_KBP_ENABLE_IPV6_MC             ARAD_KBP_ENABLE_FRWRD_TABLE(fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV6_MC])
#define ARAD_KBP_ENABLE_TRILL_UC            ARAD_KBP_ENABLE_FRWRD_TABLE(fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_TRILL_UC])
#define ARAD_KBP_ENABLE_TRILL_MC            ARAD_KBP_ENABLE_FRWRD_TABLE(fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_TRILL_MC])
#define ARAD_KBP_ENABLE_MPLS                ARAD_KBP_ENABLE_FRWRD_TABLE(fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_LSR])
#define ARAD_KBP_ENABLE_IPV4_MPLS_EXTENDED  ARAD_KBP_ENABLE_FRWRD_TABLE(fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_LSR_IP_SHARED])
#define ARAD_KBP_ENABLE_IPV6_EXTENDED       ARAD_KBP_ENABLE_FRWRD_TABLE(fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_EXTENDED_IPV6])
#define ARAD_KBP_ENABLE_P2P_EXTENDED        ARAD_KBP_ENABLE_FRWRD_TABLE(fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_EXTENDED_P2P])
#define ARAD_KBP_ENABLE_INRIF_MAPPING       ARAD_KBP_ENABLE_FRWRD_TABLE(fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_INRIF_MAPPING])

#define ARAD_KBP_ENABLE_IPV4_RPF            ARAD_KBP_ENABLE_FRWRD_TABLE(fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_RPF_1])
#define ARAD_KBP_ENABLE_IPV6_RPF            ARAD_KBP_ENABLE_FRWRD_TABLE(fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_RPF_1])
#define ARAD_KBP_ENABLE_IPV4_DC             ARAD_KBP_ENABLE_FRWRD_TABLE(fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV4_DC])

#define ARAD_KBP_ENABLE_IPV4_UC_PUBLIC      ARAD_KBP_ENABLE_FRWRD_TABLE(fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_PUBLIC_RPF_0])
#define ARAD_KBP_ENABLE_IPV4_RPF_PUBLIC     ARAD_KBP_ENABLE_FRWRD_TABLE(fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV4_UC_PUBLIC_RPF_1])
#define ARAD_KBP_ENABLE_IPV6_UC_PUBLIC      ARAD_KBP_ENABLE_FRWRD_TABLE(fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_PUBLIC_RPF_0])
#define ARAD_KBP_ENABLE_IPV6_RPF_PUBLIC     ARAD_KBP_ENABLE_FRWRD_TABLE(fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV6_UC_PUBLIC_RPF_1])

#define ARAD_KBP_ENABLE_IPV4_MC_BRIDGE      ARAD_KBP_ENABLE_FRWRD_TABLE(fwd_table_size[ARAD_KBP_FRWRD_TBL_ID_IPV4_MC_BRIDGE])

#define ARAD_KBP_ENABLE_ANY_IPV4UC_PROGRAM  (ARAD_KBP_ENABLE_IPV4_UC || ARAD_KBP_ENABLE_IPV4_MPLS_EXTENDED || ARAD_KBP_ENABLE_IPV4_DC || ARAD_KBP_ENABLE_IPV4_UC_PUBLIC)
#define ARAD_KBP_ENABLE_ANY_IPV6UC_PROGRAM  (ARAD_KBP_ENABLE_IPV6_UC || ARAD_KBP_ENABLE_IPV6_EXTENDED || ARAD_KBP_ENABLE_IPV6_UC_PUBLIC)

#define ARAD_KBP_PROG_IS_L2_SHARING(opcode) ((opcode == PROG_FLP_ETHERNET_ING_LEARN)|| (opcode == PROG_FLP_P2P))

#define ARAD_KBP_MASTER_KEY_MAX_LENGTH          (NLM_TBL_WIDTH_640)
#define ARAD_KBP_MASTER_KEY_MAX_LENGTH_BYTES    (ARAD_KBP_MASTER_KEY_MAX_LENGTH>>3)
#define ARAD_KBP_RESULT_MAX_LENGTH_BYTES        (32)

#define NLM_ARAD_MAX_NUM_RESULTS_PER_INST       ARAD_KBP_MAX_NUM_OF_PARALLEL_SEARCHES
#define ARAD_KBP_CMPR3_SKIPPED_SEARCH           (1)
#define ARAD_KBP_CMPR3_FIRST_ACL                (4)
#define ARAD_KBP_NUM_OF_KBP_SEARCHES(unit)      (SOC_IS_JERICHO(unit) ? ((ARAD_KBP_IS_OP_OR_OP2) ? 8 : 6) : 4)

#define ARAD_KBP_IS_IN_USE                 ((SOC_DPP_CONFIG(unit)->arad->init.elk.tcam_dev_type != ARAD_NIF_ELK_TCAM_DEV_TYPE_NONE) ? 1 : 0)
#define ARAD_KBP_IS_OP                     ((SOC_DPP_CONFIG(unit)->arad->init.elk.tcam_dev_type == ARAD_NIF_ELK_TCAM_DEV_TYPE_BCM52311) ? 1 : 0)
#define ARAD_KBP_IS_OP2                    ((SOC_DPP_CONFIG(unit)->arad->init.elk.tcam_dev_type == ARAD_NIF_ELK_TCAM_DEV_TYPE_BCM52321) ? 1 : 0)
#define ARAD_KBP_IS_OP_OR_OP2              (ARAD_KBP_IS_OP || ARAD_KBP_IS_OP2)
#define ARAD_KBP_OP_IS_DUAL_MODE           ((SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_connect_mode == ARAD_NIF_ELK_TCAM_CONNECT_MODE_DUAL_SHARED) || \
                                            (SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_connect_mode == ARAD_NIF_ELK_TCAM_CONNECT_MODE_DUAL_SMT) ? 1 : 0)
#define ARAD_KBP_MAX_KBP_PORT              (ARAD_KBP_OP_IS_DUAL_MODE ? 4 : 2)

#define ARAD_KBP_24BIT_FWD                 (SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_24bit_fwd)
#define ARAD_KBP_24BIT_FWD_RES_SIZE        (24)
#define ARAD_KBP_IPV4DC_24BIT_FWD          (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ext_tcam_dc_24bit_result", 0) ? 1 : 0)
#define ARAD_KBP_IPV4DC_RES1_PAD_BYTES     (ARAD_KBP_IPV4DC_24BIT_FWD ? 3 : 0)
#define ARAD_KBP_IPV4DC_RES3_PAD_BYTES     (ARAD_KBP_IPV4DC_24BIT_FWD ? 3 : 6)
#define ARAD_KBP_IPV6MC_INVERT_GROUP_SOURCE ((SOC_DPP_CONFIG(unit)->arad->init.elk.kbp_ipv6_mc_invert_group_source == 1) ? 1 : 0)

#define ARAD_KBP_MAX_NOF_RANGES_IN_TABLE   (2)

#define ARAD_KBP_NOF_VALID_DYNAMIC_LTRS    (2)



#define ARAD_KBP_FLP_PROG_TO_OPCODE(program) (program)
#define ARAD_KBP_OPCODE_TO_FLP_PROG(opcode) (opcode)

#define ARAD_KBP_TABLE_INDX_TO_DUMMY_TABLE_ID(_index)    (ARAD_KBP_FRWRD_TBL_ID_DUMMY_0 + _index)



#define ARAD_KBP_IPV4_LOOKUP_MODEL_1_IS_IN_USE(_unit_) (SOC_DPP_CONFIG(_unit_)->pp.ipmc_lookup_model == 1)

#define ARAD_KBP_IPV6_LOOKUP_MODEL_1_IS_IN_USE(_unit_) (SOC_DPP_CONFIG(_unit_)->pp.ipmc_lookup_model == 1)

#ifdef CRASH_RECOVERY_SUPPORT

#define ARAD_KBP_IS_CR_MODE(unit) (0) 
#define ARAD_KBP_NV_MEMORY_SIZE (50*1024*1024)
#endif

#define ARAD_KBP_OPCODE_NUM_MAX 256

#define ARAD_KBP_CPU_WR_LUT_OPCODE 255
#define ARAD_KBP_CPU_RD_LUT_OPCODE 254
#define ARAD_KBP_CPU_PIOWR_OPCODE 253
#define ARAD_KBP_CPU_PIORDX_OPCODE 252
#define ARAD_KBP_CPU_PIORDY_OPCODE 251
#define ARAD_KBP_CPU_ERR_OPCODE 250
#define ARAD_KBP_CPU_BLK_COPY_OPCODE 249
#define ARAD_KBP_CPU_BLK_MOVE_OPCODE 248
#define ARAD_KBP_CPU_BLK_CLR_OPCODE 247
#define ARAD_KBP_CPU_BLK_EV_OPCODE 246
#define ARAD_KBP_CPU_CTX_BUFF_WRITE_OPCODE 10
#define ARAD_KBP_CPU_4B_AD_OPCODE 220
#define ARAD_KBP_CPU_8B_AD_OPCODE 221
#define ARAD_KBP_CPU_16B_AD_OPCODE 222
#define ARAD_KBP_CPU_32B_AD_OPCODE 223
#define ARAD_KBP_CPU_UDA_OPERATION1_OPCODE 231
#define ARAD_KBP_CPU_UDA_OPERATION2_OPCODE 233
#define ARAD_KBP_CPU_PAD_OPCODE 0


#define ARAD_KBP_ROP_LTR_NUM_MAX            256
#define ARAD_KBP_MAX_NOF_KEY_SEGMENTS       10
#define ARAD_KBP_MAX_SEGMENT_LENGTH_BYTES   16


#define ARAD_KBP_CPU_BLK_COPY_INSTRUCTION 8
#define ARAD_KBP_CPU_BLK_MOVE_INSTRUCTION 9
#define ARAD_KBP_CPU_BLK_CLR_INSTRUCTION 10
#define ARAD_KBP_CPU_BLK_EV_INSTRUCTION 11
#define ARAD_KBP_CPU_AD_WRITE_INSTRUCTION 12
#define ARAD_KBP_CPU_UDA_OPERATION1_INSTRUCTION 13
#define ARAD_KBP_CPU_UDA_OPERATION2_INSTRUCTION 14


#define ARAD_KBP_CPU_BLK_COPY_REC_SIZE 8
#define ARAD_KBP_CPU_BLK_MOVE_REC_SIZE 8
#define ARAD_KBP_CPU_BLK_CLR_REC_SIZE 5
#define ARAD_KBP_CPU_BLK_EV_REC_SIZE 5
#define ARAD_KBP_CPU_4B_AD_REC_SIZE 12
#define ARAD_KBP_CPU_8B_AD_REC_SIZE 16
#define ARAD_KBP_CPU_16B_AD_REC_SIZE 24
#define ARAD_KBP_CPU_32B_AD_REC_SIZE 40
#define ARAD_KBP_CPU_UDA_OPERATION1_REC_SIZE 40
#define ARAD_KBP_CPU_UDA_OPERATION2_REC_SIZE 8


#define ARAD_KBP_CPU_CTX_BUFF_WRITE_LTR 11


#define ARAD_KBP_APP_NUM_OF_FRWRD_TABLES 2



#define ARAD_KBP_TABLE_ALLOC_CHECK_ONLY     ARAD_PP_FP_KEY_ALLOC_CHECK_ONLY


#define ARAD_KBP_TO_SOC_RESULT(result)     ((result == 0) ? SOC_SAND_OK : SOC_SAND_ERR)
#define ARAD_KBP_CHECK_FUNC_RESULT(f_res,err_num,err_exit_label)\
  if(soc_sand_update_error_code(ARAD_KBP_TO_SOC_RESULT(f_res), &ex ) != no_err)         \
  {                                                         \
    exit_place = err_num;                                   \
    FUNC_RESULT_PRINT   \
    goto err_exit_label;                                    \
  }


#define GTM_OPCODE_CONFIG_DATA_TYPE_TX_REQUEST (1)
#define GTM_OPCODE_CONFIG_DATA_TYPE_RX_REPLY   (3)

#define ARAD_KBP_NUM_OF_SMT_THREADS (2)
#define ARAD_KBP_DUAL_MODE_QUAD_PORT_LTR_ID_OFFSET ((SOC_DPP_CONFIG(unit)->arad->init.elk.is_master) ? 0 : 1)
#define ARAD_KBP_DUAL_MODE_QUAD_PORT_TBL_ID_OFFSET ((SOC_DPP_CONFIG(unit)->arad->init.elk.is_master) ? 0 : 64)

#define KBP_INFO sw_state_access[unit].dpp.soc.arad.tm.kbp_info







typedef enum
{
    ARAD_KBP_ACL_IN_MASTER_KEY_LSB_ONLY      = 0, 
    ARAD_KBP_ACL_IN_MASTER_KEY_MSB_ONLY      = 1, 
    ARAD_KBP_ACL_IN_MASTER_KEY_LSB_MSB       = 2, 
    ARAD_KBP_ACL_IN_MASTER_KEY_NOT_REQUIRED  = 3  
} ARAD_KBP_ACL_IN_MASTER_KEY_TYPE;

typedef enum
{
    
    ARAD_KBP_FRWRD_DB_TYPE_IPV4_UC,
    ARAD_KBP_FRWRD_DB_TYPE_IPV4_UC_RPF,
    ARAD_KBP_FRWRD_DB_TYPE_IPV4_MC_RPF,
    
    ARAD_KBP_FRWRD_DB_TYPE_IPV6_UC,
    ARAD_KBP_FRWRD_DB_TYPE_IPV6_UC_RPF_2PASS,
    ARAD_KBP_FRWRD_DB_TYPE_IPV6_UC_RPF,
    ARAD_KBP_FRWRD_DB_TYPE_IPV6_MC_RPF,
    
    ARAD_KBP_FRWRD_DB_TYPE_LSR,
    ARAD_KBP_FRWRD_DB_TYPE_TRILL_UC,
    ARAD_KBP_FRWRD_DB_TYPE_TRILL_MC,

    ARAD_KBP_FRWRD_DB_NOF_TYPE_ARAD,

    
    ARAD_KBP_FRWRD_DB_TYPE_IPV4_DC = ARAD_KBP_FRWRD_DB_NOF_TYPE_ARAD,
    ARAD_KBP_FRWRD_DB_TYPE_IP_LSR_SHARED,
    ARAD_KBP_FRWRD_DB_TYPE_IP_LSR_SHARED_FOR_IP,
    ARAD_KBP_FRWRD_DB_TYPE_IP_LSR_SHARED_FOR_IP_WITH_RPF,
    ARAD_KBP_FRWRD_DB_TYPE_IP_LSR_SHARED_FOR_LSR,
    ARAD_KBP_FRWRD_DB_TYPE_EXTENDED_IPv6,
    ARAD_KBP_FRWRD_DB_TYPE_EXTENDED_P2P,
    ARAD_KBP_FRWRD_DB_TYPE_IPV4_UC_PUBLIC,
    ARAD_KBP_FRWRD_DB_TYPE_IPV4_UC_PUBLIC_RPF,
    ARAD_KBP_FRWRD_DB_TYPE_IPV6_UC_PUBLIC,
    ARAD_KBP_FRWRD_DB_TYPE_IPV6_UC_PUBLIC_RPF,
    ARAD_KBP_FRWRD_DB_TYPE_IPV4_MC_BRIDGE,
    ARAD_KBP_FRWRD_DB_TYPE_DUMMY_FRWRD,
    ARAD_KBP_FRWRD_DB_NOF_TYPE_ARAD_PLUS,

    ARAD_KBP_FRWRD_DB_NOF_TYPES   = ARAD_KBP_FRWRD_DB_NOF_TYPE_ARAD_PLUS,
    ARAD_KBP_FRWRD_DB_ACL_OFFSET  = ARAD_KBP_FRWRD_DB_NOF_TYPES,
    ARAD_KBP_MAX_NUM_OF_FRWRD_DBS = SOC_DPP_DEFS_MAX(NOF_FLP_PROGRAMS)
} ARAD_KBP_FRWRD_IP_DB_TYPE;

typedef enum
{
    ARAD_KBP_LTR_ID_FIRST                            = 0,
    
    ARAD_KBP_FRWRD_LTR_IPV4_UC                       = ARAD_KBP_LTR_ID_FIRST, 
    ARAD_KBP_FRWRD_LTR_IPV4_UC_RPF                   = 2,
    ARAD_KBP_FRWRD_LTR_IPV4_MC_RPF                   = 4,
    
    ARAD_KBP_FRWRD_LTR_IPV6_UC                       = 6,
    ARAD_KBP_FRWRD_LTR_IPV6_UC_RPF_2PASS             = 8,
    ARAD_KBP_FRWRD_LTR_IPV6_UC_RPF                   = 10,
    ARAD_KBP_FRWRD_LTR_IPV6_MC_RPF                   = 12,
    
    ARAD_KBP_FRWRD_LTR_LSR                           = 14,
    ARAD_KBP_FRWRD_LTR_TRILL_UC                      = 16,
    ARAD_KBP_FRWRD_LTR_TRILL_MC                      = 18,
    
    ARAD_KBP_FRWRD_LTR_IPV4_UC_PUBLIC                = 20,
    ARAD_KBP_FRWRD_LTR_IPV4_UC_RPF_PUBLIC            = 22,
    ARAD_KBP_FRWRD_LTR_IPV6_UC_PUBLIC                = 24,
    ARAD_KBP_FRWRD_LTR_IPV6_UC_RPF_PUBLIC            = 26,
    ARAD_KBP_FRWRD_LTR_IPV4_MC_BRIDGE                = 28,
    ARAD_KBP_FRWRD_LTR_IPV4_DC                       = 30,
    ARAD_KBP_FRWRD_LTR_IP_LSR_SHARED                 = 32,
    ARAD_KBP_FRWRD_LTR_IP_LSR_SHARED_FOR_IP          = 33,
    ARAD_KBP_FRWRD_LTR_IP_LSR_SHARED_FOR_IP_WITH_RPF = 34,
    ARAD_KBP_FRWRD_LTR_IP_LSR_SHARED_FOR_LSR         = 35,
    ARAD_KBP_FRWRD_LTR_IP_LSR_EXTENDED_IPV6          = 36,
    ARAD_KBP_FRWRD_LTR_IP_LSR_EXTENDED_P2P           = 37,

    
    ARAD_KBP_FRWRD_LTR_DUMMY_FRWRD                   = 38,
    ARAD_KBP_MAX_FRWRD_LTR_ID                        = ARAD_KBP_FRWRD_LTR_DUMMY_FRWRD,

    ARAD_KBP_ACL_LTR_ID_OFFSET                       = 40,

    
    ARAD_KBP_MAX_ACL_LTR_ID = ARAD_KBP_LTR_ID_FIRST + 2*ARAD_KBP_MAX_NUM_OF_FRWRD_DBS,

} ARAD_KBP_FRWRD_IP_LTR;

typedef enum
{
    ARAD_KBP_FRWRD_TABLE_OPCODE_NONE          = PROG_FLP_PPPOE_IPV4UC, 

    ARAD_KBP_FRWRD_TABLE_OPCODE_IPV4_UC_RPF  = PROG_FLP_IPV4UC_RPF,         
    ARAD_KBP_FRWRD_TABLE_OPCODE_IPV4_UC      = PROG_FLP_IPV4UC,             
    ARAD_KBP_FRWRD_TABLE_OPCODE_IPV4_MC_COMP = PROG_FLP_IPV4COMPMC_WITH_RPF,
    ARAD_KBP_FRWRD_TABLE_OPCODE_IPV4_MC_BRIDGE = PROG_FLP_IPV4MC_BRIDGE,    

    ARAD_KBP_FRWRD_TABLE_OPCODE_IPV6_UC      = PROG_FLP_IPV6UC,             
    ARAD_KBP_FRWRD_TABLE_OPCODE_EXTENDED_P2P = PROG_FLP_P2P,                
    ARAD_KBP_FRWRD_TABLE_OPCODE_IPV6_MC_RPF  = PROG_FLP_IPV6MC,             

    ARAD_KBP_FRWRD_TABLE_OPCODE_LSR          = PROG_FLP_LSR,                
    ARAD_KBP_FRWRD_TABLE_OPCODE_TRILL_UC     = PROG_FLP_TRILL_UC,           
    ARAD_KBP_FRWRD_TABLE_OPCODE_TRILL_MC     = PROG_FLP_TRILL_MC_ONE_TAG,   

    ARAD_KBP_FRWRD_TABLE_OPCODE_IPV4_DC      = PROG_FLP_IPV4_DC,            

    ARAD_KBP_FRWRD_TABLE_OPCODE_SHARED_IP_LSR_FOR_IP           = 12,
    ARAD_KBP_FRWRD_TABLE_OPCODE_SHARED_IP_LSR_FOR_IP_WITH_RPF  = 13,
    ARAD_KBP_FRWRD_TABLE_OPCODE_SHARED_IP_LSR_FOR_LSR          = 14,
    ARAD_KBP_FRWRD_TABLE_OPCODE_EXTENDED_IPV6                  = 15,

    ARAD_KBP_FRWRD_TABLE_OPCODE_IPV6_UC_RPF        = PROG_FLP_IPV6UC_RPF, 
    ARAD_KBP_FRWRD_TABLE_OPCODE_IPV6_UC_RPF_2PASS  = PROG_FLP_IPV6UC_WITH_RPF_2PASS, 

    ARAD_KBP_FRWRD_TABLE_OPCODE_IPV4_UC_PUBLIC     = PROG_FLP_IPV4UC_PUBLIC, 
    ARAD_KBP_FRWRD_TABLE_OPCODE_IPV4_UC_PUBLIC_RPF = PROG_FLP_IPV4UC_PUBLIC_RPF, 

    ARAD_KBP_FRWRD_TABLE_OPCODE_IPV6_UC_PUBLIC     = PROG_FLP_IPV6UC_PUBLIC, 
    ARAD_KBP_FRWRD_TABLE_OPCODE_IPV6_UC_PUBLIC_RPF = PROG_FLP_IPV6UC_PUBLIC_RPF, 
} ARAD_KBP_FRWRD_IP_OPCODE;

typedef enum
{
    ARAD_KBP_MAX_AD_TABLE_SIZE_64,
    ARAD_KBP_MAX_AD_TABLE_SIZE_48,
    ARAD_KBP_MAX_AD_TABLE_SIZE_32,
    ARAD_KBP_MAX_AD_TABLE_SIZE_24,
    
    ARAD_KBP_MAX_NUM_OF_AD_TABLES,

} ARAD_KBP_AD_TABLE_INDEX;

typedef enum alg_kbp_tbl_width
{
    NLM_TBL_WIDTH_80  = 80,
    NLM_TBL_WIDTH_160 = 160,
    NLM_TBL_WIDTH_320 = 320,
    NLM_TBL_WIDTH_480 = 480,
    NLM_TBL_WIDTH_640 = 640,

    NLM_TBL_WIDTH_END
}alg_kbp_tbl_width;

typedef enum alg_kbp_ad_width
{
    NLM_TBL_ADLEN_ZERO = 0,
    NLM_TBL_ADLEN_24B  = 24,
    NLM_TBL_ADLEN_32B  = 32,
    NLM_TBL_ADLEN_48B  = 48,
    NLM_TBL_ADLEN_64B  = 64,
    NLM_TBL_ADLEN_128B = 128,
    NLM_TBL_ADLEN_256B = 256,

    NLM_TBL_ADLEN_END

} alg_kbp_ad_width;

#define ARAD_KBP_AD_WIDTH_TYPE_TO_BITS(width)    \
     ((width == NLM_TBL_ADLEN_24B)  ? 24  : \
     ((width == NLM_TBL_ADLEN_32B)  ? 32  : \
     ((width == NLM_TBL_ADLEN_48B)  ? 48  : \
     ((width == NLM_TBL_ADLEN_64B)  ? 64  : \
     ((width == NLM_TBL_ADLEN_128B) ? 128 : \
     0)))))

#define ARAD_KBP_AD_WIDTH_TO_AD_TABLE_IDX(width)    \
     ((width == NLM_TBL_ADLEN_24B) ? ARAD_KBP_MAX_AD_TABLE_SIZE_24 : \
     ((width == NLM_TBL_ADLEN_32B) ? ARAD_KBP_MAX_AD_TABLE_SIZE_32 : \
     ((width == NLM_TBL_ADLEN_48B) ? ARAD_KBP_MAX_AD_TABLE_SIZE_48 : \
     ((width == NLM_TBL_ADLEN_64B) ? ARAD_KBP_MAX_AD_TABLE_SIZE_64 : \
     ARAD_KBP_MAX_NUM_OF_AD_TABLES))))

#define ARAD_KBP_AD_TABLE_IDX_TO_AD_WIDTH(_index)    \
     ((_index == ARAD_KBP_MAX_AD_TABLE_SIZE_24) ? NLM_TBL_ADLEN_24B : \
     ((_index == ARAD_KBP_MAX_AD_TABLE_SIZE_32) ? NLM_TBL_ADLEN_32B : \
     ((_index == ARAD_KBP_MAX_AD_TABLE_SIZE_48) ? NLM_TBL_ADLEN_48B : \
     ((_index == ARAD_KBP_MAX_AD_TABLE_SIZE_64) ? NLM_TBL_ADLEN_64B : \
     0))))











typedef kbp_instruction_t*      ARAD_KBP_INSTRUCTION;
typedef kbp_key_t*              ARAD_KBP_KEY;



typedef struct
{
    
    char name[20];

    
    uint8 nof_bytes;

    
    int type;

} ARAD_KBP_KEY_SEGMENT;

typedef struct
{
    int     nof_segments;
    ARAD_KBP_KEY_SEGMENT key_segment[ARAD_KBP_MAX_NOF_KEY_SEGMENTS];

}ARAD_KBP_DIAG_ENTRY_PARSING_TABLE;


typedef struct 
{
    
    uint8 valid;

     
    nlm_u8 tbl_id;

     
    kbp_db_type_t db_type;

     
    uint32 tbl_size;

    
    alg_kbp_tbl_width tbl_width;

    
    alg_kbp_ad_width tbl_asso_width;

    
    uint16 group_id_start;

    
    uint16 group_id_end;

    
    NlmBankNum bankNum;

    
    int32 min_priority;

    
    ARAD_KBP_DIAG_ENTRY_PARSING_TABLE entry_key_parsing;

    
    nlm_u8 clone_of_tbl_id;

} ARAD_KBP_TABLE_CONFIG;


typedef struct 
{
    
    uint32 nof_key_segments;

    
    ARAD_KBP_KEY_SEGMENT key_segment[ARAD_KBP_MAX_NOF_KEY_SEGMENTS];

} ARAD_KBP_LTR_SINGLE_SEARCH;


typedef struct 
{
    
    uint8 valid;

    
    uint8 opcode;

    
    ARAD_KBP_FRWRD_IP_LTR ltr_id;

    
    uint8 parallel_srches_bmp;

    
    ARAD_KBP_FRWRD_IP_TBL_ID tbl_id[ARAD_KBP_MAX_NUM_OF_PARALLEL_SEARCHES];

    
    NlmBool is_cmp3_search;

    
    ARAD_KBP_LTR_SINGLE_SEARCH ltr_search[ARAD_KBP_MAX_NUM_OF_PARALLEL_SEARCHES];

    
    ARAD_KBP_LTR_SINGLE_SEARCH master_key_fields;


    
    ARAD_KBP_INSTRUCTION inst_p;

    
    ARAD_KBP_KEY master_key;

} ARAD_KBP_LTR_CONFIG;

typedef struct
{
    
    uint32 tx_data_size;

    
    uint32 tx_data_type;

    
    uint32 rx_data_size;

    
    uint32 rx_data_type;

} ARAD_KBP_GTM_OPCODE_CONFIG_INFO;

typedef struct alg_kbp_rec
{
    uint8    *m_data; 
    uint8    *m_mask; 
    uint16    m_len;  
} alg_kbp_rec;

typedef struct tableRecordInfo
{
    alg_kbp_rec   record;
    uint16        groupId;
    uint16        priority;
    uint32        index;
    uint8         assoData[8]; 
} tableRecordInfo;

typedef struct tableInfo
{
    uint8               tbl_id;
    alg_kbp_tbl_width   tbl_width;
    alg_kbp_ad_width    tbl_assoWidth;
    uint32              tbl_size;
    uint32              max_recCount;
    uint32              rec_count;
    uint16              groupId_start;
    uint16              groupId_end;
    tableRecordInfo     *tblRecordInfo_p;

    
    struct kbp_db       *db_p;
    struct kbp_ad_db    *ad_db_p;
    struct kbp_key      *key;
    uint8               is_key_adde_to_db;
    ARAD_KBP_KEY_SEGMENT dummy_segment;
} tableInfo;

typedef struct globalGTMInfo_s
{
    tableInfo tblInfo;

    
    uint8 ltr_num;

} globalGTMInfo;


typedef struct arad_kbp_rslt_s
{
    uint8   m_resultValid[NLM_MAX_NUM_RESULTS];
    uint8   m_hitOrMiss[NLM_MAX_NUM_RESULTS];
    uint8   m_respType[NLM_MAX_NUM_RESULTS];

    uint8   m_hitDevId[NLM_MAX_NUM_RESULTS];
    uint32  m_hitIndex[NLM_MAX_NUM_RESULTS];

    uint8   m_AssocData[NLM_MAX_NUM_RESULTS][NLMDEV_MAX_AD_LEN_IN_BYTES];

} arad_kbp_rslt;

typedef struct ad_info
{
    struct kbp_ad_db *ad_db_p;
    uint32 ad_table_size;

} ad_info;

typedef struct genericTblMgrAradAppData
{
    globalGTMInfo g_gtmInfo[ARAD_KBP_MAX_NUM_OF_TABLES];
    
    NlmCmAllocator  alloc_bdy[SOC_DPP_DEFS_MAX(NOF_CORES)];
    NlmCmAllocator *alloc_p[SOC_DPP_DEFS_MAX(NOF_CORES)];

    

    struct kbp_allocator *dalloc_p[SOC_DPP_DEFS_MAX(NOF_CORES)];
    void *alg_kbp_xpt_p[SOC_DPP_DEFS_MAX(NOF_CORES)];
    struct kbp_device *device_p[SOC_DPP_DEFS_MAX(NOF_CORES)];
    struct kbp_device *thread_p[SOC_DPP_DEFS_MAX(NOF_CORES)];
    struct kbp_device_config device_config[SOC_DPP_DEFS_MAX(NOF_CORES)];
    uint8 app_config_done;

} genericTblMgrAradAppData;


typedef struct arad_kbp_frwd_ltr_db_s
{
    uint32 opcode;
    uint32 res_data_len[NLM_ARAD_MAX_NUM_RESULTS_PER_INST];
    NlmAradCompareResponseFormat res_format[NLM_ARAD_MAX_NUM_RESULTS_PER_INST];
    uint32 res_total_data_len;
} arad_kbp_frwd_ltr_db_t;


typedef kbp_db_t*  kbp_db_handle;
typedef kbp_ad_db_t* kbp_ad_handle;
typedef struct 
{
    kbp_db_handle       db_p;
    kbp_ad_handle       ad_db_p;
    uint8               is_valid;
    uint32              table_size;
    uint32              table_id;
    uint32              table_width;
    uint32              table_asso_width;
    uint32              nof_range_qualifiers;
    ARAD_PP_FP_RANGE_QUAL_INFO range_qualifier[ARAD_KBP_MAX_NOF_RANGES_IN_TABLE];

} ARAD_KBP_DB_HANDLES;


typedef struct 
{
    ARAD_KBP_INSTRUCTION inst_p;

} ARAD_KBP_LTR_HANDLES;


typedef struct
{
    void    *xpt_p[SOC_DPP_DEFS_MAX(NOF_CORES)];
    struct kbp_device *dev_p[SOC_DPP_DEFS_MAX(NOF_CORES)];
}  ARAD_KBP_DEV_HANDLERS;


typedef struct
{
    kbp_db_handle       db_p;
    kbp_ad_handle       ad_db_p;
}  ARAD_KBP_DB_TEMP_HANDLERS;



typedef  int32_t (*kbp_reset_f)(void *handle, int32_t s_reset_low, int32_t c_reset_low);

#ifndef KBP_WARMBOOT_TYPE_DEFINED
#define KBP_WARMBOOT_TYPE_DEFINED

typedef struct kbp_warmboot_s{
    FILE *kbp_file_fp;
    kbp_device_issu_read_fn  kbp_file_read;
    kbp_device_issu_write_fn kbp_file_write;
}kbp_warmboot_t;
#endif 


typedef struct kbp_acl_range_s {
    uint16 low;
    uint16 high;
    uint32 flags;
    uint8 in_use;
} kbp_acl_range_t;

typedef struct {
    ARAD_KBP_DB_HANDLES db_info[ARAD_KBP_MAX_NUM_OF_TABLES];
    ARAD_KBP_LTR_HANDLES ltr_info[ARAD_KBP_MAX_NUM_OF_FRWRD_DBS];
    SOC_SAND_HASH_TABLE_PTR hash_table[ARAD_KBP_MAX_NUM_OF_FRWRD_DBS];


    ARAD_KBP_TABLE_CONFIG           Arad_kbp_table_config_info[ARAD_KBP_MAX_NUM_OF_TABLES];
    ARAD_KBP_LTR_CONFIG             Arad_kbp_ltr_config[ARAD_KBP_MAX_NUM_OF_FRWRD_DBS];
    arad_kbp_lut_data_t             Arad_kbp_gtm_lut_info[ARAD_KBP_MAX_NUM_OF_FRWRD_DBS];
    arad_kbp_frwd_ltr_db_t          Arad_kbp_gtm_ltr_info[ARAD_KBP_MAX_NUM_OF_FRWRD_DBS];
    ARAD_KBP_GTM_OPCODE_CONFIG_INFO Arad_kbp_gtm_opcode_config_info[ARAD_KBP_MAX_NUM_OF_FRWRD_DBS];

    
    uint8                                 Arad_pmf_ce_dynamic_kbp_qualifiers_enabled;
    ARAD_PMF_CE_IRPP_QUALIFIER_ATTRIBUTES Arad_pmf_ce_dynamic_kbp_qualifiers_values[ARAD_KBP_MAX_NUM_OF_PARALLEL_SEARCHES];

    uint32                                Arad_kbp_max_num_ranges;
    PARSER_HINT_ARR kbp_acl_range_t       *Arad_kbp_acl_range;

} ARAD_KBP_INFO;







uint32 arad_kbp_test_ip4_rpf_NlmGenericTableManager(
    int unit,
    uint32 num_entries,
    uint32 *record_base_tbl,
    uint32 *ad_val_tbl);


uint32 arad_kbp_alg_kbp_db_get(
   int unit,
   uint32 frwrd_tbl_id,
   struct kbp_db **db_p);


uint32 arad_kbp_gtm_table_info_get(
   int unit,
   uint32 frwrd_tbl_id,
   tableInfo *tblInfo_p);

uint32 arad_kbp_alg_kbp_ad_db_get(
   int unit,
   uint32 frwrd_tbl_id,
   struct kbp_ad_db **db_p);


uint32 arad_kbp_gtm_ltr_num_get(
   int unit,
   uint32 frwrd_tbl_id,
   uint8 *ltr_num);

uint32 
    arad_kbp_get_device_pointer(
        SOC_SAND_IN  int                unit,
        SOC_SAND_OUT struct kbp_device  **device_p
    );
uint32 
    arad_kbp_ltr_get_inst_pointer(
        SOC_SAND_IN  int                        unit,
        SOC_SAND_IN  uint32                     ltr_idx,
        SOC_SAND_OUT struct  kbp_instruction    **inst_p
    );
uint32 
    arad_kbp_opcode_to_db_type(
       SOC_SAND_IN  int                      unit,
       SOC_SAND_IN  uint8                       opcode,
       SOC_SAND_OUT uint32                      *db_type,
       SOC_SAND_OUT uint32                      *ltr_id,
       SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success
   );



uint32
    arad_kbp_set_all_db_by_ltr(int unit, int ltr_ndx);

uint32 arad_kbp_init_kbp_interface( 
    int unit,
    uint32 core);

uint32 arad_kbp_init_arad_interface( 
    int unit);

uint32 arad_kbp_init_app(
    int unit,
    uint32 second_kbp_supported,
    ARAD_INIT_ELK *elk);

uint32 arad_kbp_deinit_app(
    int unit,
    uint32 second_kbp_supported,
    int no_sync_flag);

uint32 arad_kbp_init_kbp_reset(
    int unit,
    uint32 core);


void
  ARAD_KBP_GTM_LUT_clear(
    SOC_SAND_OUT arad_kbp_lut_data_t *info
  );
void
  arad_kbp_frwd_ltr_db_clear(
    SOC_SAND_OUT arad_kbp_frwd_ltr_db_t *info
  );
void
  ARAD_KBP_GTM_OPCODE_CONFIG_INFO_clear(
    SOC_SAND_OUT ARAD_KBP_GTM_OPCODE_CONFIG_INFO *info
  );
void
  ARAD_KBP_LTR_CONFIG_clear(
    SOC_SAND_OUT ARAD_KBP_LTR_CONFIG *info
  );
void
  ARAD_KBP_LTR_SINGLE_SEARCH_clear(
    SOC_SAND_OUT ARAD_KBP_LTR_SINGLE_SEARCH *info
  );
void
  ARAD_KBP_TABLE_CONFIG_clear(
    SOC_SAND_OUT ARAD_KBP_TABLE_CONFIG *info
  );
void
  ARAD_KBP_KEY_SEGMENT_clear(
    SOC_SAND_OUT ARAD_KBP_KEY_SEGMENT *info
  );


void
  arad_kbp_frwd_lut_db_print(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  arad_kbp_lut_data_t *info
  );
void
  arad_kbp_frwd_ltr_db_print(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  arad_kbp_frwd_ltr_db_t *info,
    SOC_SAND_IN  int print_mode
  );
void
  ARAD_KBP_GTM_OPCODE_CONFIG_INFO_print(
    SOC_SAND_IN  ARAD_KBP_GTM_OPCODE_CONFIG_INFO *info
  );
void
  ARAD_KBP_LTR_CONFIG_print(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  ARAD_KBP_LTR_CONFIG *info,
    SOC_SAND_IN  int print_mode
  );
void
  ARAD_KBP_LTR_SINGLE_SEARCH_print(
    SOC_SAND_IN  ARAD_KBP_LTR_SINGLE_SEARCH *info,
    SOC_SAND_IN  int print_mode
  );
void
  ARAD_KBP_TABLE_CONFIG_print(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  ARAD_KBP_TABLE_CONFIG *info
  );
void
  ARAD_KBP_KEY_SEGMENT_print(
    SOC_SAND_IN  ARAD_KBP_KEY_SEGMENT *info,
    SOC_SAND_IN  int print_mode
  );
const char *
  ARAD_KBP_KEY_FIELD_TYPE_to_string(
    SOC_SAND_IN enum kbp_key_field_type enum_val
  );

const char*
  NlmAradCompareResponseFormat_to_string(
    SOC_SAND_IN  NlmAradCompareResponseFormat enum_val
  );
const char*
  ARAD_KBP_FRWRD_IP_DB_TYPE_to_string(
    SOC_SAND_IN  ARAD_KBP_FRWRD_IP_DB_TYPE enum_val
  );

uint32
    arad_kbp_result_sizes_configurations_init(int unit, ARAD_KBP_FRWRD_IP_DB_TYPE db_type, int results[ARAD_PP_FLP_KBP_MAX_NUMBER_OF_RESULTS]);

uint32
  arad_kbp_update_master_key(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN int                     prog_id,
    SOC_SAND_IN SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX],
    SOC_SAND_IN int                     nof_qualifiers);

uint32
  arad_kbp_update_lookup(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN int                     table_id,
    SOC_SAND_IN int                     prog_id,
    SOC_SAND_IN int                     search_id,
    SOC_SAND_IN SOC_DPP_DBAL_QUAL_INFO  qual_info[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX],
    SOC_SAND_IN int                     nof_qualifiers);

uint32
    arad_kbp_sw_init(
       SOC_SAND_IN  int  unit
    );

uint32 
    arad_kbp_add_acl(
       SOC_SAND_IN  int                         unit,
       SOC_SAND_IN  uint32                      table_id,
       SOC_SAND_IN  uint32                      search_id,
       SOC_SAND_IN  uint32                      pgm_bmp_used,
       SOC_SAND_IN  uint32                      pgm_ndx_min,
       SOC_SAND_IN  uint32                      pgm_ndx_max,
       SOC_SAND_IN  uint32                      key_size_in_bits_lsb,
       SOC_SAND_IN  uint32                      key_size_in_bits_msb,
       SOC_SAND_IN  uint32                      flags,
       SOC_SAND_IN  uint32                      min_priority,
       uint32                                   nof_shared_quals,
       ARAD_PP_FP_SHARED_QUAL_INFO              shared_quals_info[MAX_NOF_SHARED_QUALIFIERS_PER_PROGRAM],
       uint32                                   nof_range_quals,
       ARAD_PP_FP_RANGE_QUAL_INFO               range_quals_info[ARAD_KBP_MAX_NOF_RANGES_IN_TABLE],
       SOC_SAND_OUT uint32                      *pgm_bmp_new,
       SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success
    );


uint32 arad_kbp_cpu_ctxb_write_cmp(
    int unit,
    uint8 opcode,
    soc_reg_above_64_val_t data);


uint32
  arad_kbp_table_size_get(
    SOC_SAND_IN int                      unit,
    SOC_SAND_IN ARAD_KBP_FRWRD_IP_TBL_ID table_id,
    SOC_SAND_OUT uint32                  *table_size_in_bytes,
    SOC_SAND_OUT uint32                  *table_payload_in_bytes
  );


uint32
  arad_kbp_static_table_size_get(
    SOC_SAND_IN int                      unit,
    SOC_SAND_IN ARAD_KBP_FRWRD_IP_TBL_ID table_id,
    SOC_SAND_OUT uint32                  *table_size_in_bytes,
    SOC_SAND_OUT uint32                  *table_payload_in_bytes
  );



uint32
  arad_kbp_table_ltr_id_get(
    SOC_SAND_IN int                   unit, 
    SOC_SAND_IN ARAD_KBP_FRWRD_IP_TBL_ID table_id,
    SOC_SAND_OUT ARAD_KBP_FRWRD_IP_LTR   *ltr_id
  );


uint32 arad_kbp_ilkn_interface_param_get( 
    int         unit,
    uint32      core,
    soc_port_t *ilkn_port,
    uint32     *ilkn_num_lanes,
    int        *ilkn_rate,
    uint32     *ilkn_metaframe);


uint32
    arad_kbp_ltr_config_get(
        SOC_SAND_IN  int unit,
        SOC_SAND_IN  uint32 flp_program,
        SOC_SAND_OUT ARAD_KBP_LTR_CONFIG *config
    );

#ifdef CRASH_RECOVERY_SUPPORT

uint32 arad_kbp_cr_transaction_cmd(int unit,
                                   uint8 is_start);
uint8 arad_kbp_cr_query_restore_status(int unit);
uint32 arad_kbp_cr_clear_restore_status(int unit);
uint32 arad_kbp_cr_db_commit(int unit,
                             uint32 tbl_id);
#endif

uint32 arad_kbp_db_commit(
   int unit);


int arad_kbp_autosync_set(int unit, int enable);


int arad_kbp_sync(int unit);


void 
  arad_kbp_warmboot_register(int unit,
                             FILE *file_fp,
                             kbp_device_issu_read_fn read_fn, 
                             kbp_device_issu_write_fn write_fn);

void 
    arad_kbp_sw_config_print(
       SOC_SAND_IN  int  unit,
       SOC_SAND_IN  int  print_mode,
       SOC_SAND_IN  int  print_ltr
    );

void
    arad_kbp_device_print(
       SOC_SAND_IN  int   unit,
       SOC_SAND_IN  char* file_name
    );

void
    arad_kbp_device_dump(
       SOC_SAND_IN  int   unit,
       SOC_SAND_IN  char* file_name
   );

int
  arad_kbp_print_diag_entry_added(
     SOC_SAND_IN    int       unit,
     SOC_SAND_IN    tableInfo *tbl_info,
     SOC_SAND_IN    uint8     *data,
     SOC_SAND_IN    uint32    prefix_len,
     SOC_SAND_IN    uint8     *mask,
     SOC_SAND_IN    uint8     *ad_data);

int
  arad_kbp_print_diag_all_entries(int unit, int iterate_table);

int
  arad_kbp_print_diag_last_packet(int unit, int core, int flp_program);

int
  arad_kbp_print_search_result(int unit, uint32 ltr_idx, uint8 *master_key_buffer, struct kbp_search_result *search_rslt);

int32_t
  arad_kbp_callback_mdio_write(void *handle, int32_t chip_no, uint8_t dev, uint16_t reg, uint16_t value);

int32_t
  arad_kbp_callback_mdio_read(void *handle, int32_t chip_no, uint8_t dev, uint16_t reg, uint16_t *value);

uint32 arad_kbp_dynamic_ltr_validate(int unit, int ltr_db_type);

uint32
  arad_kbp_acl_range_get_by_range_and_flags(int unit, uint32 flags, uint32 *range_id, uint32 emin, uint32 emax, int *result);

uint32
  arad_kbp_acl_range_get_by_id(int unit, uint32 *flags, uint32 range_id, uint32 *emin, uint32 *emax, int *result);

uint32
  arad_kbp_acl_range_create(int unit, uint32 flags, uint32 *range_index, uint32 emin, uint32 emax, uint8 is_id_api, int *result);

uint32
  arad_kbp_acl_range_qualify_add(void *unitDataIn, bcm_field_qualify_t qual, bcm_field_entry_t entry, bcm_field_range_t range_id, int *result);

int32_t arad_kbp_usleep_callback(void *handle, uint32_t usec);

int arad_kbp_init_rop_test(int unit, uint32 core);

uint32 arad_kbp_init_gpio_config(int unit);

uint32 arad_kbp_init_device_config_init(int unit, uint32 core, uint32 kbp_ilkn_rev, kbp_reset_f kbp_reset);
#include <soc/dpp/SAND/Utils/sand_footer.h>



#endif 
