#include <soc/mcm/memregs.h>
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
 */
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_INTR



#ifdef BCM_DPP_SUPPORT
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_mact.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_mact_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lem_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ipv4.h>
#include <soc/dpp/drv.h>
#include <soc/mem.h>
#include <sal/core/dpc.h>
#include <soc/dpp/ARAD/arad_sim_em.h>
#include <soc/dcmn/dcmn_pp_em_ser.h>
#include <soc/dcmn/dcmn_intr_corr_act_func.h>


#define JER_CHIP_SIM_LEM_KEY                  ARAD_CHIP_SIM_LEM_KEY
#define JER_CHIP_SIM_LEM_PAYLOAD              ARAD_CHIP_SIM_LEM_PAYLOAD
#define JER_CHIP_SIM_LEM_BASE                 ARAD_CHIP_SIM_LEM_BASE
#define JER_CHIP_SIM_LEM_TABLE_SIZE           ARAD_CHIP_SIM_LEM_TABLE_SIZE

#define JER_CHIP_SIM_ISEM_A_KEY               ARAD_CHIP_SIM_ISEM_A_KEY
#define JER_CHIP_SIM_ISEM_A_PAYLOAD           ARAD_CHIP_SIM_ISEM_A_PAYLOAD
#define JER_CHIP_SIM_ISEM_A_BASE              ARAD_CHIP_SIM_ISEM_A_BASE
#define JER_CHIP_SIM_ISEM_A_TABLE_SIZE        ARAD_CHIP_SIM_ISEM_A_TABLE_SIZE

#define JER_CHIP_SIM_ISEM_B_KEY               ARAD_CHIP_SIM_ISEM_B_KEY
#define JER_CHIP_SIM_ISEM_B_PAYLOAD           ARAD_CHIP_SIM_ISEM_B_PAYLOAD
#define JER_CHIP_SIM_ISEM_B_BASE              ARAD_CHIP_SIM_ISEM_B_BASE
#define JER_CHIP_SIM_ISEM_B_TABLE_SIZE        ARAD_CHIP_SIM_ISEM_B_TABLE_SIZE

#define JER_CHIP_SIM_GLEM_KEY                 ARAD_CHIP_SIM_GLEM_KEY
#define JER_CHIP_SIM_GLEM_PAYLOAD             ARAD_CHIP_SIM_GLEM_PAYLOAD
#define JER_CHIP_SIM_GLEM_BASE                ARAD_CHIP_SIM_GLEM_BASE
#define JER_CHIP_SIM_GLEM_TABLE_SIZE          ARAD_CHIP_SIM_GLEM_TABLE_SIZE

#define JER_CHIP_SIM_ESEM_KEY                 ARAD_CHIP_SIM_ESEM_KEY
#define JER_CHIP_SIM_ESEM_PAYLOAD             ARAD_CHIP_SIM_ESEM_PAYLOAD
#define JER_CHIP_SIM_ESEM_BASE                ARAD_CHIP_SIM_ESEM_BASE
#define JER_CHIP_SIM_ESEM_TABLE_SIZE          ARAD_CHIP_SIM_ESEM_TABLE_SIZE

#define JER_CHIP_SIM_RMAPEM_KEY               ARAD_CHIP_SIM_RMAPEM_KEY
#define JER_CHIP_SIM_RMAPEM_PAYLOAD           ARAD_CHIP_SIM_RMAPEM_PAYLOAD
#define JER_CHIP_SIM_RMAPEM_BASE              ARAD_CHIP_SIM_RMAPEM_BASE
#define JER_CHIP_SIM_RMAPEM_TABLE_SIZE        ARAD_CHIP_SIM_RMAPEM_TABLE_SIZE

#define JER_CHIP_SIM_OEMA_KEY                 ARAD_CHIP_SIM_OEMA_KEY
#define JER_CHIP_SIM_OEMA_PAYLOAD             ARAD_CHIP_SIM_OEMA_PAYLOAD
#define JER_CHIP_SIM_OEMA_BASE                ARAD_CHIP_SIM_OEMA_BASE
#define JER_CHIP_SIM_OEMA_TABLE_SIZE          ARAD_CHIP_SIM_OEMA_TABLE_SIZE

#define JER_CHIP_SIM_OEMB_KEY                 ARAD_CHIP_SIM_OEMB_KEY
#define JER_CHIP_SIM_OEMB_PAYLOAD             ARAD_CHIP_SIM_OEMB_PAYLOAD
#define JER_CHIP_SIM_OEMB_BASE                ARAD_CHIP_SIM_OEMB_BASE
#define JER_CHIP_SIM_OEMB_TABLE_SIZE          ARAD_CHIP_SIM_OEMB_TABLE_SIZE


#define JER_PP_EM_SER_NOF_HASH_RESULTS             8
#define JER_PP_EM_SER_EM_AUX_CAM_SIZE              32

#define JER_PP_EM_SER_LEM_VERIFIER_LEN             63
#define JER_PP_EM_SER_LEM_VERIFIER_LEN_IN_UINT32   (JER_PP_EM_SER_LEM_VERIFIER_LEN / SOC_SAND_NOF_BITS_IN_UINT32 + 1)

#define JER_PP_EM_SER_LEM_NOF_FIDS                 (32 * 1024)

#define LEM_ERR_MEM_MASK_REGISTER       PPDB_B_ECC_ERR_2B_MONITOR_MEM_MASKr
#define ISEM_ERR_MEM_MASK_REGISTER      IHB_ECC_ERR_2B_MONITOR_MEM_MASKr
#define PPDB_A_ERR_MEM_MASK_REGISTER    PPDB_A_ECC_ERR_2B_MONITOR_MEM_MASKr
#define PPDB_B_ERR_MEM_MASK_REGISTER    PPDB_B_ECC_ERR_2B_MONITOR_MEM_MASKr
#define GLEM_ERR_MEM_MASK_REGISTER      EDB_ECC_ERR_2B_MONITOR_MEM_MASKr
#define ESEM_ERR_MEM_MASK_REGISTER      EDB_ECC_ERR_2B_MONITOR_MEM_MASKr
#define OEMA_ERR_MEM_MASK_REGISTER      PPDB_A_ECC_ERR_2B_MONITOR_MEM_MASKr
#define OEMB_ERR_MEM_MASK_REGISTER      PPDB_A_ECC_ERR_2B_MONITOR_MEM_MASKr
#define RMAPEM_ERR_MEM_MASK_REGISTER    OAMP_ECC_ERR_2B_MONITOR_MEM_MASKr


#define JER_PP_EM_SER_REQUEST_TYPE_START_BIT  0
#define JER_PP_EM_SER_REQUEST_TYPE_SIZE       3
#define JER_PP_EM_SER_REQUEST_TYPE_INSERT_VAL 1
#define JER_PP_EM_SER_REQUEST_TYPE_DELETE_VAL 0


#define JER_PP_EM_SER_LEM_TRVRS_ITER_BLK_SIZE      (130)

#define JER_PP_EM_SER_LEM_FLU_MACHINE_HIT_COUNTER_SIZE      17



#define MANAGEMENT_UNIT_CONFIGURATION_REGISTER(block,db)    block##_##db##_MANAGEMENT_UNIT_CONFIGURATION_REGISTERr
#define MNGMNT_UNIT_ENABLE_FIELD(db)                        db##_MNGMNT_UNIT_ENABLEf

#define ECC_2B_ERR_CNT_REGISTER(block)                      block##_ECC_2B_ERR_CNTr
#define MANAGEMENT_REQUEST_MEM(block, db)                   block##_##db##_MANAGEMENT_REQUESTm
#define KEY_FIELD(db)                                       db##_KEYf
#define PAYLOAD_FIELD(db)                                   db##_PAYLOADf
#define KEYT_PLDT_MEM(block, db)                            block##_##db##_KEYT_PLDT_Hm
#define KEYT_AUX_MEM(block, db)                             block##_##db##_KEYT_AUXm
#define PLDT_AUX_MEM(block, db)                             block##_##db##_PLDT_AUXm
#define ENTRIES_COUNTER_REGISTER(block, db)                 block##_##db##_ENTRIES_COUNTERr
#define DIAGNOSTICS_REGISTER(block, db)                     block##_##db##_DIAGNOSTICSr
#define DIAGNOSTICS_INDEX_REGISTER(block, db)               block##_##db##_DIAGNOSTICS_INDEXr
#define DIAGNOSTICS_KEY_REGISTER(block, db)                 block##_##db##_DIAGNOSTICS_KEYr
#define DIAGNOSTICS_READ_RESULT_REGISTER(block, db)         block##_##db##_DIAGNOSTICS_READ_RESULTr
#define DIAGNOSTICS_LOOKUP_RESULT_REGISTER(block, db)       block##_##db##_DIAGNOSTICS_LOOKUP_RESULTr
#define DIAGNOSTICS_READ_FIELD(db)                          db##_DIAGNOSTICS_READf
#define DIAGNOSTICS_LOOKUP_FIELD(db)                        db##_DIAGNOSTICS_LOOKUPf

#define MASK_SER_INTERRUPTS_32B_SET(db, mask_register, reg, val) \
    if(soc_reg_field_valid(unit,mask_register,db##_KEYT_PLDT_H_0_ECC_2B_ERR_MASKf)) soc_reg_field_set(unit, mask_register, reg, db##_KEYT_PLDT_H_0_ECC_2B_ERR_MASKf, val);   \
    if(soc_reg_field_valid(unit,mask_register,db##_KEYT_PLDT_H_1_ECC_2B_ERR_MASKf)) soc_reg_field_set(unit, mask_register, reg, db##_KEYT_PLDT_H_1_ECC_2B_ERR_MASKf, val);   \
    if(soc_reg_field_valid(unit,mask_register,db##_KEYT_PLDT_H_2_ECC_2B_ERR_MASKf)) soc_reg_field_set(unit, mask_register, reg, db##_KEYT_PLDT_H_2_ECC_2B_ERR_MASKf, val);   \
    if(soc_reg_field_valid(unit,mask_register,db##_KEYT_PLDT_H_3_ECC_2B_ERR_MASKf)) soc_reg_field_set(unit, mask_register, reg, db##_KEYT_PLDT_H_3_ECC_2B_ERR_MASKf, val);   \
    if(soc_reg_field_valid(unit,mask_register,db##_KEYT_PLDT_H_4_ECC_2B_ERR_MASKf)) soc_reg_field_set(unit, mask_register, reg, db##_KEYT_PLDT_H_4_ECC_2B_ERR_MASKf, val);   \
    if(soc_reg_field_valid(unit,mask_register,db##_KEYT_PLDT_H_5_ECC_2B_ERR_MASKf)) soc_reg_field_set(unit, mask_register, reg, db##_KEYT_PLDT_H_5_ECC_2B_ERR_MASKf, val);   \
    if(soc_reg_field_valid(unit,mask_register,db##_KEYT_PLDT_H_6_ECC_2B_ERR_MASKf)) soc_reg_field_set(unit, mask_register, reg, db##_KEYT_PLDT_H_6_ECC_2B_ERR_MASKf, val);   \
    if(soc_reg_field_valid(unit,mask_register,db##_KEYT_PLDT_H_7_ECC_2B_ERR_MASKf)) soc_reg_field_set(unit, mask_register, reg, db##_KEYT_PLDT_H_7_ECC_2B_ERR_MASKf, val)

#define MASK_SER_INTERRUPTS_64B_SET(db, mask_register, reg, val) \
    if(soc_reg_field_valid(unit,mask_register,db##_KEYT_PLDT_H_0_ECC_2B_ERR_MASKf)) soc_reg64_field_set(unit, mask_register, reg, db##_KEYT_PLDT_H_0_ECC_2B_ERR_MASKf, val);   \
    if(soc_reg_field_valid(unit,mask_register,db##_KEYT_PLDT_H_1_ECC_2B_ERR_MASKf)) soc_reg64_field_set(unit, mask_register, reg, db##_KEYT_PLDT_H_1_ECC_2B_ERR_MASKf, val);   \
    if(soc_reg_field_valid(unit,mask_register,db##_KEYT_PLDT_H_2_ECC_2B_ERR_MASKf)) soc_reg64_field_set(unit, mask_register, reg, db##_KEYT_PLDT_H_2_ECC_2B_ERR_MASKf, val);   \
    if(soc_reg_field_valid(unit,mask_register,db##_KEYT_PLDT_H_3_ECC_2B_ERR_MASKf)) soc_reg64_field_set(unit, mask_register, reg, db##_KEYT_PLDT_H_3_ECC_2B_ERR_MASKf, val);   \
    if(soc_reg_field_valid(unit,mask_register,db##_KEYT_PLDT_H_4_ECC_2B_ERR_MASKf)) soc_reg64_field_set(unit, mask_register, reg, db##_KEYT_PLDT_H_4_ECC_2B_ERR_MASKf, val);   \
    if(soc_reg_field_valid(unit,mask_register,db##_KEYT_PLDT_H_5_ECC_2B_ERR_MASKf)) soc_reg64_field_set(unit, mask_register, reg, db##_KEYT_PLDT_H_5_ECC_2B_ERR_MASKf, val);   \
    if(soc_reg_field_valid(unit,mask_register,db##_KEYT_PLDT_H_6_ECC_2B_ERR_MASKf)) soc_reg64_field_set(unit, mask_register, reg, db##_KEYT_PLDT_H_6_ECC_2B_ERR_MASKf, val);   \
    if(soc_reg_field_valid(unit,mask_register,db##_KEYT_PLDT_H_7_ECC_2B_ERR_MASKf)) soc_reg64_field_set(unit, mask_register, reg, db##_KEYT_PLDT_H_7_ECC_2B_ERR_MASKf, val)

#define MASK_SER_INTERRUPTS_ABOVE_64B_SET(db, mask_register, reg, val) \
   if(soc_reg_field_valid(unit,mask_register,db##_KEYT_PLDT_H_0_ECC_2B_ERR_MASKf)) soc_reg_above_64_field_set(unit, mask_register, reg, db##_KEYT_PLDT_H_0_ECC_2B_ERR_MASKf, val);   \
   if(soc_reg_field_valid(unit,mask_register,db##_KEYT_PLDT_H_1_ECC_2B_ERR_MASKf)) soc_reg_above_64_field_set(unit, mask_register, reg, db##_KEYT_PLDT_H_1_ECC_2B_ERR_MASKf, val);   \
   if(soc_reg_field_valid(unit,mask_register,db##_KEYT_PLDT_H_2_ECC_2B_ERR_MASKf)) soc_reg_above_64_field_set(unit, mask_register, reg, db##_KEYT_PLDT_H_2_ECC_2B_ERR_MASKf, val);   \
   if(soc_reg_field_valid(unit,mask_register,db##_KEYT_PLDT_H_3_ECC_2B_ERR_MASKf)) soc_reg_above_64_field_set(unit, mask_register, reg, db##_KEYT_PLDT_H_3_ECC_2B_ERR_MASKf, val);   \
   if(soc_reg_field_valid(unit,mask_register,db##_KEYT_PLDT_H_4_ECC_2B_ERR_MASKf)) soc_reg_above_64_field_set(unit, mask_register, reg, db##_KEYT_PLDT_H_4_ECC_2B_ERR_MASKf, val);   \
   if(soc_reg_field_valid(unit,mask_register,db##_KEYT_PLDT_H_5_ECC_2B_ERR_MASKf)) soc_reg_above_64_field_set(unit, mask_register, reg, db##_KEYT_PLDT_H_5_ECC_2B_ERR_MASKf, val);   \
   if(soc_reg_field_valid(unit,mask_register,db##_KEYT_PLDT_H_6_ECC_2B_ERR_MASKf)) soc_reg_above_64_field_set(unit, mask_register, reg, db##_KEYT_PLDT_H_6_ECC_2B_ERR_MASKf, val);   \
   if(soc_reg_field_valid(unit,mask_register,db##_KEYT_PLDT_H_7_ECC_2B_ERR_MASKf)) soc_reg_above_64_field_set(unit, mask_register, reg, db##_KEYT_PLDT_H_7_ECC_2B_ERR_MASKf, val)





typedef enum
{
    JER_PP_EM_SER_DB_TYPE_LEM,
    JER_PP_EM_SER_DB_TYPE_ISEM_A,
    JER_PP_EM_SER_DB_TYPE_ISEM_B,
    JER_PP_EM_SER_DB_TYPE_GLEM,
    JER_PP_EM_SER_DB_TYPE_ESEM_0,
    JER_PP_EM_SER_DB_TYPE_ESEM_1,
    JER_PP_EM_SER_DB_TYPE_OEMA,
    JER_PP_EM_SER_DB_TYPE_OEMB,
    JER_PP_EM_SER_DB_TYPE_RMAPEM,
    JER_PP_EM_SER_DB_TYPE_NOF_TYPES
}JER_PP_EM_SER_DB_TYPE;

typedef struct
{
    JER_PP_EM_SER_DB_TYPE db_type;

    
    soc_mem_t req_mem;
    uint32 key_start_bit;
    uint32 key_size;
    uint32 payload_start_bit;
    uint32 payload_size;

    
    uint32 shadow_key_size;
    uint32 shadow_payload_size;
    uint32 shadow_base;

    
    soc_reg_t diagnostics_reg;
    soc_reg_t diagnostics_key_reg;
    soc_reg_t diagnostics_lookup_result_reg;
    soc_reg_t diagnostics_read_result_reg;
    soc_reg_t diagnostics_index_reg;
    soc_field_t diagnostics_lookup_field;
    soc_field_t diagnostics_read_field;

    soc_mem_t keyt_pldt_mem;
    soc_mem_t keyt_aux_mem;
    soc_mem_t pldt_aux_mem;
    soc_reg_t counter_reg;
    uint32 db_nof_lines;

    uint32 block_instance;
    soc_reg_t completed_reg;
    soc_field_t completed_field;
    soc_reg_t enable_dynamic_reg;

}JER_PP_SER_EM_TYPE_INFO;



static int fix_global_counters=1;
static int fix_fid_counters=1;

static uint32 p1, p2, p3, p4, p5;
static SOC_PPC_FRWRD_MACT_ENTRY_KEY    l2_traverse_mact_keys[JER_PP_EM_SER_LEM_TRVRS_ITER_BLK_SIZE];
static SOC_PPC_FRWRD_MACT_ENTRY_VALUE  l2_traverse_mact_vals[JER_PP_EM_SER_LEM_TRVRS_ITER_BLK_SIZE];
static uint16 lem_fid_lif_counters[JER_PP_EM_SER_LEM_NOF_FIDS]={0};


uint32 jer_entry_index[JER_PP_EM_SER_DB_TYPE_NOF_TYPES][JER_PP_EM_SER_NOF_HASH_RESULTS] =
            {{92753, 15569, 49009, 78651, 123267, 4338, 0, 0},          
             {821, 6634, 822, 1382, 4701, 7246, 7023, 717},             
             {821, 6634, 822, 1382, 4701, 7246, 7023, 717},             
             {593, 1641, 1853, 3317, 1860, 678, 0, 0},                  
             {3910, 3792, 273, 6760, 7293, 3294, 5933, 7039},           
             {0, 0, 0, 0, 0, 0, 0, 0},            
             {2709, 2060, 161, 3043, 995, 2681, 751, 163},              
             {369, 1668, 1984, 597, 216, 560, 533, 1027},               
             {244, 2478, 2267, 2496, 3832, 3368, 3196, 3394}            
            };
uint32 jerp_entry_index[JER_PP_EM_SER_DB_TYPE_NOF_TYPES][JER_PP_EM_SER_NOF_HASH_RESULTS] =
            {{92753, 15569, 49009, 78651, 123267, 4338, 0, 0},          
             {821, 6634, 822, 1382, 4701, 7246, 7023, 717},             
             {821, 6634, 822, 1382, 4701, 7246, 7023, 717},             
             {5653, 4653, 4985, 6321, 4864, 5858, 3329, 0},             
             {3910, 3792, 273, 6760, 7293, 3294, 5933, 7039},           
             {3910, 3792, 273, 6760, 7293, 3294, 5933, 7039},           
             {2709, 2060, 161, 3043, 995, 2681, 751, 163},              
             {369, 1668, 1984, 597, 216, 560, 533, 1027},               
             {244, 2478, 2267, 2496, 3832, 3368, 3196, 3394}            
            };
uint32 qax_entry_index[JER_PP_EM_SER_DB_TYPE_NOF_TYPES][JER_PP_EM_SER_NOF_HASH_RESULTS] =
            {{2352, 17143, 22949, 17636, 31168, 313, 19078, 10223},     
             {3866, 2766, 3930, 3964, 2000, 3622, 479, 1070},           
             {3866, 2766, 3930, 3964, 2000, 3622, 479, 1070},           
             {1436, 4883, 4954, 1186, 4990, 741, 1429, 818},            
             {2569, 1186, 2567, 2809, 3404, 3942, 14, 2378},            
             {0, 0, 0, 0, 0, 0, 0, 0},            
             {1984, 1098, 746, 1823, 1520, 1472, 498, 136},             
             {156, 974, 771, 464, 231, 642, 560, 765},                  
             {939, 1012, 1664, 2478, 821, 1991, 0, 0}                   
            };
uint32 qux_entry_index[JER_PP_EM_SER_DB_TYPE_NOF_TYPES][JER_PP_EM_SER_NOF_HASH_RESULTS] =
            {{7319, 6683, 5632, 4190, 7807, 1451, 8058, 7054},          
             {1056, 1826, 617, 980, 581, 1736, 0, 0},                   
             {1056, 1826, 617, 980, 581, 1736, 0, 0},                   
             {2712, 1548, 1121, 1204, 2451, 556, 0, 0},                 
             {1536, 1202, 1744, 586, 2040, 402, 307, 397},              
             {0, 0, 0, 0, 0, 0, 0, 0},            
             {209, 53, 490, 401, 62, 443, 507, 89},                     
             {239, 4, 228, 124, 104, 158, 207, 183},                    
             {42, 962, 288, 1009, 518, 787, 0, 0}                       
            };



uint32 jer_pp_ser_em_type_info_get(int unit, uint32 block_instance, soc_mem_t mem, JER_PP_SER_EM_TYPE_INFO *ser_info)
{

    uint32 res = SOC_SAND_OK;
    soc_field_info_t   fld;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ser_info->block_instance = block_instance;
    switch (mem) {
        case KEYT_PLDT_MEM(IHB, ISEM):
            if (block_instance == 1) {
                ser_info->shadow_key_size = JER_CHIP_SIM_ISEM_B_KEY;
                ser_info->shadow_payload_size = JER_CHIP_SIM_ISEM_B_PAYLOAD;
                ser_info->shadow_base = JER_CHIP_SIM_ISEM_B_BASE;
                ser_info->db_type = JER_PP_EM_SER_DB_TYPE_ISEM_B;
            } else {
                ser_info->shadow_key_size = JER_CHIP_SIM_ISEM_A_KEY;
                ser_info->shadow_payload_size = JER_CHIP_SIM_ISEM_A_PAYLOAD;
                ser_info->shadow_base = JER_CHIP_SIM_ISEM_A_BASE;
                ser_info->db_type = JER_PP_EM_SER_DB_TYPE_ISEM_A;
            }
            ser_info->req_mem = MANAGEMENT_REQUEST_MEM(IHB, ISEM);
            ARAD_TBL_REF(unit, ser_info->req_mem, KEY_FIELD(ISEM), &fld);
            ser_info->key_start_bit = ARAD_FLD_LSB(fld);
            ser_info->key_size = ARAD_FLD_NOF_BITS(fld);
            ARAD_TBL_REF(unit, ser_info->req_mem, PAYLOAD_FIELD(ISEM), &fld);
            ser_info->payload_start_bit = ARAD_FLD_LSB(fld);
            ser_info->payload_size = ARAD_FLD_NOF_BITS(fld);
            ser_info->db_nof_lines = soc_mem_index_count(unit, mem) * SOC_MEM_NUMELS(unit, mem);
            ser_info->diagnostics_reg = DIAGNOSTICS_REGISTER(IHB, ISEM);
            ser_info->diagnostics_key_reg = DIAGNOSTICS_KEY_REGISTER(IHB, ISEM);
            ser_info->diagnostics_lookup_result_reg = DIAGNOSTICS_LOOKUP_RESULT_REGISTER(IHB, ISEM);
            ser_info->diagnostics_lookup_field = DIAGNOSTICS_LOOKUP_FIELD(ISEM);
            ser_info->diagnostics_read_result_reg = DIAGNOSTICS_READ_RESULT_REGISTER(IHB, ISEM);
            ser_info->diagnostics_read_field = DIAGNOSTICS_READ_FIELD(ISEM);
            ser_info->diagnostics_index_reg = DIAGNOSTICS_INDEX_REGISTER(IHB, ISEM);
            ser_info->keyt_aux_mem = KEYT_AUX_MEM(IHB, ISEM);
            ser_info->pldt_aux_mem = PLDT_AUX_MEM(IHB, ISEM);
            ser_info->keyt_pldt_mem = mem;
            ser_info->counter_reg = ENTRIES_COUNTER_REGISTER(IHB, ISEM);

            ser_info->completed_reg = IHB_ISEM_INTERRUPT_REGISTER_ONEr;
            ser_info->completed_field = ISEM_MANAGEMENT_COMPLETEDf;
            ser_info->enable_dynamic_reg = IHB_ENABLE_DYNAMIC_MEMORY_ACCESSr;
            break;

        case KEYT_PLDT_MEM(PPDB_A, ISEM):
            ser_info->db_type = JER_PP_EM_SER_DB_TYPE_ISEM_A;
            ser_info->req_mem = MANAGEMENT_REQUEST_MEM(PPDB_A, ISEM);
            ARAD_TBL_REF(unit, ser_info->req_mem, KEY_FIELD(ISEM), &fld);
            ser_info->key_start_bit = ARAD_FLD_LSB(fld);
            ser_info->key_size = ARAD_FLD_NOF_BITS(fld);
            ARAD_TBL_REF(unit, ser_info->req_mem, PAYLOAD_FIELD(ISEM), &fld);
            ser_info->payload_start_bit = ARAD_FLD_LSB(fld);
            ser_info->payload_size = ARAD_FLD_NOF_BITS(fld);
            ser_info->shadow_key_size = JER_CHIP_SIM_ISEM_A_KEY;
            ser_info->shadow_payload_size = JER_CHIP_SIM_ISEM_A_PAYLOAD;
            ser_info->shadow_base = JER_CHIP_SIM_ISEM_A_BASE;
            ser_info->db_nof_lines = soc_mem_index_count(unit, mem) * SOC_MEM_NUMELS(unit, mem);
            ser_info->diagnostics_reg = DIAGNOSTICS_REGISTER(PPDB_A, ISEM);
            ser_info->diagnostics_key_reg = DIAGNOSTICS_KEY_REGISTER(PPDB_A, ISEM);
            ser_info->diagnostics_lookup_result_reg = DIAGNOSTICS_LOOKUP_RESULT_REGISTER(PPDB_A, ISEM);
            ser_info->diagnostics_lookup_field = DIAGNOSTICS_LOOKUP_FIELD(ISEM);
            ser_info->diagnostics_read_result_reg = DIAGNOSTICS_READ_RESULT_REGISTER(PPDB_A, ISEM);
            ser_info->diagnostics_read_field = DIAGNOSTICS_READ_FIELD(ISEM);
            ser_info->diagnostics_index_reg = DIAGNOSTICS_INDEX_REGISTER(PPDB_A, ISEM);
            ser_info->keyt_aux_mem = KEYT_AUX_MEM(PPDB_A, ISEM);
            ser_info->pldt_aux_mem = PLDT_AUX_MEM(PPDB_A, ISEM);
            ser_info->keyt_pldt_mem = mem;
            ser_info->counter_reg = ENTRIES_COUNTER_REGISTER(PPDB_A, ISEM);

            ser_info->completed_reg = PPDB_A_ISEM_INTERRUPT_REGISTER_ONEr;
            ser_info->completed_field = ISEM_MANAGEMENT_COMPLETEDf;
            ser_info->enable_dynamic_reg = PPDB_A_ENABLE_DYNAMIC_MEMORY_ACCESSr;
            break;

        case KEYT_PLDT_MEM(PPDB_B, ISEM):
            ser_info->db_type = JER_PP_EM_SER_DB_TYPE_ISEM_B;
            ser_info->req_mem = MANAGEMENT_REQUEST_MEM(PPDB_B, ISEM);
            ARAD_TBL_REF(unit, ser_info->req_mem, KEY_FIELD(ISEM), &fld);
            ser_info->key_start_bit = ARAD_FLD_LSB(fld);
            ser_info->key_size = ARAD_FLD_NOF_BITS(fld);
            ARAD_TBL_REF(unit, ser_info->req_mem, PAYLOAD_FIELD(ISEM), &fld);
            ser_info->payload_start_bit = ARAD_FLD_LSB(fld);
            ser_info->payload_size = ARAD_FLD_NOF_BITS(fld);
            ser_info->shadow_key_size = JER_CHIP_SIM_ISEM_B_KEY;
            ser_info->shadow_payload_size = JER_CHIP_SIM_ISEM_B_PAYLOAD;
            ser_info->shadow_base = JER_CHIP_SIM_ISEM_B_BASE;
            ser_info->db_nof_lines = soc_mem_index_count(unit, mem) * SOC_MEM_NUMELS(unit, mem);
            ser_info->diagnostics_reg = DIAGNOSTICS_REGISTER(PPDB_B, ISEM);
            ser_info->diagnostics_key_reg = DIAGNOSTICS_KEY_REGISTER(PPDB_B, ISEM);
            ser_info->diagnostics_lookup_result_reg = DIAGNOSTICS_LOOKUP_RESULT_REGISTER(PPDB_B, ISEM);
            ser_info->diagnostics_lookup_field = DIAGNOSTICS_LOOKUP_FIELD(ISEM);
            ser_info->diagnostics_read_result_reg = DIAGNOSTICS_READ_RESULT_REGISTER(PPDB_B, ISEM);
            ser_info->diagnostics_read_field = DIAGNOSTICS_READ_FIELD(ISEM);
            ser_info->diagnostics_index_reg = DIAGNOSTICS_INDEX_REGISTER(PPDB_B, ISEM);
            ser_info->keyt_aux_mem = KEYT_AUX_MEM(PPDB_B, ISEM);
            ser_info->pldt_aux_mem = PLDT_AUX_MEM(PPDB_B, ISEM);
            ser_info->keyt_pldt_mem = mem;
            ser_info->counter_reg = ENTRIES_COUNTER_REGISTER(PPDB_B, ISEM);

            ser_info->completed_reg = PPDB_B_ISEM_INTERRUPT_REGISTER_ONEr;
            ser_info->completed_field = ISEM_MANAGEMENT_COMPLETEDf;
            ser_info->enable_dynamic_reg = PPDB_B_ENABLE_DYNAMIC_MEMORY_ACCESSr;
            break;

        case KEYT_PLDT_MEM(EDB, GLEM):
            ser_info->db_type = JER_PP_EM_SER_DB_TYPE_GLEM;
            ser_info->req_mem = MANAGEMENT_REQUEST_MEM(EDB, GLEM);
            ARAD_TBL_REF(unit, ser_info->req_mem, KEY_FIELD(GLEM), &fld);
            ser_info->key_start_bit = ARAD_FLD_LSB(fld);
            ser_info->key_size = ARAD_FLD_NOF_BITS(fld);
            ARAD_TBL_REF(unit, ser_info->req_mem, PAYLOAD_FIELD(GLEM), &fld);
            ser_info->payload_start_bit = ARAD_FLD_LSB(fld);
            ser_info->payload_size = ARAD_FLD_NOF_BITS(fld);
            ser_info->shadow_key_size = JER_CHIP_SIM_GLEM_KEY;
            ser_info->shadow_payload_size = JER_CHIP_SIM_GLEM_PAYLOAD;
            ser_info->shadow_base = JER_CHIP_SIM_GLEM_BASE;
            ser_info->db_nof_lines = soc_mem_index_count(unit, mem) * SOC_MEM_NUMELS(unit, mem);
            ser_info->diagnostics_reg = DIAGNOSTICS_REGISTER(EDB, GLEM);
            ser_info->diagnostics_key_reg = DIAGNOSTICS_KEY_REGISTER(EDB, GLEM);
            ser_info->diagnostics_lookup_result_reg = DIAGNOSTICS_LOOKUP_RESULT_REGISTER(EDB, GLEM);
            ser_info->diagnostics_lookup_field = DIAGNOSTICS_LOOKUP_FIELD(GLEM);
            ser_info->diagnostics_read_result_reg = DIAGNOSTICS_READ_RESULT_REGISTER(EDB, GLEM);
            ser_info->diagnostics_read_field = DIAGNOSTICS_READ_FIELD(GLEM);
            ser_info->diagnostics_index_reg = DIAGNOSTICS_INDEX_REGISTER(EDB, GLEM);
            ser_info->keyt_aux_mem = KEYT_AUX_MEM(EDB, GLEM);
            ser_info->pldt_aux_mem = PLDT_AUX_MEM(EDB, GLEM);
            ser_info->keyt_pldt_mem = mem;
            ser_info->counter_reg = ENTRIES_COUNTER_REGISTER(EDB, GLEM);

            ser_info->completed_reg = EDB_GLEM_INTERRUPT_REGISTER_ONEr;
            ser_info->completed_field = GLEM_MANAGEMENT_COMPLETEDf;
            ser_info->enable_dynamic_reg = EDB_ENABLE_DYNAMIC_MEMORY_ACCESSr;
            break;

        case KEYT_PLDT_MEM(EDB, ESEM):
            ser_info->db_type = JER_PP_EM_SER_DB_TYPE_ESEM_0;
            ser_info->req_mem = MANAGEMENT_REQUEST_MEM(EDB, ESEM);
            ARAD_TBL_REF(unit, ser_info->req_mem, KEY_FIELD(ESEM), &fld);
            ser_info->key_start_bit = ARAD_FLD_LSB(fld);
            ser_info->key_size = ARAD_FLD_NOF_BITS(fld);
            ARAD_TBL_REF(unit, ser_info->req_mem, PAYLOAD_FIELD(ESEM), &fld);
            ser_info->payload_start_bit = ARAD_FLD_LSB(fld);
            ser_info->payload_size = ARAD_FLD_NOF_BITS(fld);
            ser_info->shadow_key_size = JER_CHIP_SIM_ESEM_KEY;
            ser_info->shadow_payload_size = JER_CHIP_SIM_ESEM_PAYLOAD;
            ser_info->shadow_base = JER_CHIP_SIM_ESEM_BASE;
            ser_info->db_nof_lines = soc_mem_index_count(unit, mem) * SOC_MEM_NUMELS(unit, mem);
            ser_info->diagnostics_reg = DIAGNOSTICS_REGISTER(EDB, ESEM);
            ser_info->diagnostics_key_reg = DIAGNOSTICS_KEY_REGISTER(EDB, ESEM);
            ser_info->diagnostics_lookup_result_reg = DIAGNOSTICS_LOOKUP_RESULT_REGISTER(EDB, ESEM);
            ser_info->diagnostics_lookup_field = DIAGNOSTICS_LOOKUP_FIELD(ESEM);
            ser_info->diagnostics_read_result_reg = DIAGNOSTICS_READ_RESULT_REGISTER(EDB, ESEM);
            ser_info->diagnostics_read_field = DIAGNOSTICS_READ_FIELD(ESEM);
            ser_info->diagnostics_index_reg = DIAGNOSTICS_INDEX_REGISTER(EDB, ESEM);
            ser_info->keyt_aux_mem = KEYT_AUX_MEM(EDB, ESEM);
            ser_info->pldt_aux_mem = PLDT_AUX_MEM(EDB, ESEM);
            ser_info->keyt_pldt_mem = mem;
            ser_info->counter_reg = ENTRIES_COUNTER_REGISTER(EDB, ESEM);

            ser_info->completed_reg = EDB_ESEM_INTERRUPT_REGISTER_ONEr;
            ser_info->completed_field = ESEM_MANAGEMENT_COMPLETEDf;
            ser_info->enable_dynamic_reg = EDB_ENABLE_DYNAMIC_MEMORY_ACCESSr;
            break;

        case KEYT_PLDT_MEM(EDB, ESEM_0):
            ser_info->db_type = JER_PP_EM_SER_DB_TYPE_ESEM_0;
            ser_info->req_mem = MANAGEMENT_REQUEST_MEM(EDB, ESEM_0);
            ARAD_TBL_REF(unit, ser_info->req_mem, KEY_FIELD(ESEM_0), &fld);
            ser_info->key_start_bit = ARAD_FLD_LSB(fld);
            ser_info->key_size = ARAD_FLD_NOF_BITS(fld);
            ARAD_TBL_REF(unit, ser_info->req_mem, PAYLOAD_FIELD(ESEM_0), &fld);
            ser_info->payload_start_bit = ARAD_FLD_LSB(fld);
            ser_info->payload_size = ARAD_FLD_NOF_BITS(fld);
            ser_info->shadow_key_size = JER_CHIP_SIM_ESEM_KEY;
            ser_info->shadow_payload_size = JER_CHIP_SIM_ESEM_PAYLOAD;
            ser_info->shadow_base = JER_CHIP_SIM_ESEM_BASE;
            ser_info->db_nof_lines = soc_mem_index_count(unit, mem) * SOC_MEM_NUMELS(unit, mem);
            ser_info->diagnostics_reg = DIAGNOSTICS_REGISTER(EDB, ESEM_0);
            ser_info->diagnostics_key_reg = DIAGNOSTICS_KEY_REGISTER(EDB, ESEM_0);
            ser_info->diagnostics_lookup_result_reg = DIAGNOSTICS_LOOKUP_RESULT_REGISTER(EDB, ESEM_0);
            ser_info->diagnostics_lookup_field = DIAGNOSTICS_LOOKUP_FIELD(ESEM_0);
            ser_info->diagnostics_read_result_reg = DIAGNOSTICS_READ_RESULT_REGISTER(EDB, ESEM_0);
            ser_info->diagnostics_read_field = DIAGNOSTICS_READ_FIELD(ESEM_0);
            ser_info->diagnostics_index_reg = DIAGNOSTICS_INDEX_REGISTER(EDB, ESEM_0);
            ser_info->keyt_aux_mem = KEYT_AUX_MEM(EDB, ESEM_0);
            ser_info->pldt_aux_mem = PLDT_AUX_MEM(EDB, ESEM_0);
            ser_info->keyt_pldt_mem = mem;
            ser_info->counter_reg = ENTRIES_COUNTER_REGISTER(EDB, ESEM_0);

            ser_info->completed_reg = EDB_ESEM_0_INTERRUPT_REGISTER_ONEr;
            ser_info->completed_field = ESEM_0_MANAGEMENT_COMPLETEDf;
            ser_info->enable_dynamic_reg = EDB_ENABLE_DYNAMIC_MEMORY_ACCESSr;
            break;

        case KEYT_PLDT_MEM(EDB, ESEM_1):
            ser_info->db_type = JER_PP_EM_SER_DB_TYPE_ESEM_1;
            ser_info->req_mem = MANAGEMENT_REQUEST_MEM(EDB, ESEM_1);
            ARAD_TBL_REF(unit, ser_info->req_mem, KEY_FIELD(ESEM_1), &fld);
            ser_info->key_start_bit = ARAD_FLD_LSB(fld);
            ser_info->key_size = ARAD_FLD_NOF_BITS(fld);
            ARAD_TBL_REF(unit, ser_info->req_mem, PAYLOAD_FIELD(ESEM_1), &fld);
            ser_info->payload_start_bit = ARAD_FLD_LSB(fld);
            ser_info->payload_size = ARAD_FLD_NOF_BITS(fld);
            ser_info->shadow_key_size = JER_CHIP_SIM_ESEM_KEY;
            ser_info->shadow_payload_size = JER_CHIP_SIM_ESEM_PAYLOAD;
            ser_info->shadow_base = JER_CHIP_SIM_ESEM_BASE;
            ser_info->db_nof_lines = soc_mem_index_count(unit, mem) * SOC_MEM_NUMELS(unit, mem);
            ser_info->diagnostics_reg = DIAGNOSTICS_REGISTER(EDB, ESEM_1);
            ser_info->diagnostics_key_reg = DIAGNOSTICS_KEY_REGISTER(EDB, ESEM_1);
            ser_info->diagnostics_lookup_result_reg = DIAGNOSTICS_LOOKUP_RESULT_REGISTER(EDB, ESEM_1);
            ser_info->diagnostics_lookup_field = DIAGNOSTICS_LOOKUP_FIELD(ESEM_1);
            ser_info->diagnostics_read_result_reg = DIAGNOSTICS_READ_RESULT_REGISTER(EDB, ESEM_1);
            ser_info->diagnostics_read_field = DIAGNOSTICS_READ_FIELD(ESEM_1);
            ser_info->diagnostics_index_reg = DIAGNOSTICS_INDEX_REGISTER(EDB, ESEM_1);
            ser_info->keyt_aux_mem = KEYT_AUX_MEM(EDB, ESEM_1);
            ser_info->pldt_aux_mem = PLDT_AUX_MEM(EDB, ESEM_1);
            ser_info->keyt_pldt_mem = mem;
            ser_info->counter_reg = ENTRIES_COUNTER_REGISTER(EDB, ESEM_1);

            ser_info->completed_reg = EDB_ESEM_1_INTERRUPT_REGISTER_ONEr;
            ser_info->completed_field = ESEM_1_MANAGEMENT_COMPLETEDf;
            ser_info->enable_dynamic_reg = EDB_ENABLE_DYNAMIC_MEMORY_ACCESSr;
            break;

        case KEYT_PLDT_MEM(PPDB_A, OEMA):
            ser_info->db_type = JER_PP_EM_SER_DB_TYPE_OEMA;
            ser_info->req_mem = MANAGEMENT_REQUEST_MEM(PPDB_A, OEMA);
            ARAD_TBL_REF(unit, ser_info->req_mem, KEY_FIELD(OEMA), &fld);
            ser_info->key_start_bit = ARAD_FLD_LSB(fld);
            ser_info->key_size = ARAD_FLD_NOF_BITS(fld);
            ARAD_TBL_REF(unit, ser_info->req_mem, PAYLOAD_FIELD(OEMA), &fld);
            ser_info->payload_start_bit = ARAD_FLD_LSB(fld);
            ser_info->payload_size = ARAD_FLD_NOF_BITS(fld);
            ser_info->shadow_key_size = JER_CHIP_SIM_OEMA_KEY;
            ser_info->shadow_payload_size = JER_CHIP_SIM_OEMA_PAYLOAD;
            ser_info->shadow_base = JER_CHIP_SIM_OEMA_BASE;
            ser_info->db_nof_lines = soc_mem_index_count(unit, mem) * SOC_MEM_NUMELS(unit, mem);
            ser_info->diagnostics_reg = DIAGNOSTICS_REGISTER(PPDB_A, OEMA);
            ser_info->diagnostics_key_reg = DIAGNOSTICS_KEY_REGISTER(PPDB_A, OEMA);
            ser_info->diagnostics_lookup_result_reg = DIAGNOSTICS_LOOKUP_RESULT_REGISTER(PPDB_A, OEMA);
            ser_info->diagnostics_lookup_field = DIAGNOSTICS_LOOKUP_FIELD(OEMA);
            ser_info->diagnostics_read_result_reg = DIAGNOSTICS_READ_RESULT_REGISTER(PPDB_A, OEMA);
            ser_info->diagnostics_read_field = DIAGNOSTICS_READ_FIELD(OEMA);
            ser_info->diagnostics_index_reg = DIAGNOSTICS_INDEX_REGISTER(PPDB_A, OEMA);
            ser_info->keyt_aux_mem = KEYT_AUX_MEM(PPDB_A, OEMA);
            ser_info->pldt_aux_mem = PLDT_AUX_MEM(PPDB_A, OEMA);
            ser_info->keyt_pldt_mem = mem;
            ser_info->counter_reg = ENTRIES_COUNTER_REGISTER(PPDB_A, OEMA);

            ser_info->completed_reg = PPDB_A_OEMA_INTERRUPT_REGISTER_ONEr;
            ser_info->completed_field = OEMA_MANAGEMENT_COMPLETEDf;
            ser_info->enable_dynamic_reg = PPDB_A_ENABLE_DYNAMIC_MEMORY_ACCESSr;
            break;

        case KEYT_PLDT_MEM(PPDB_A, OEMB):
            ser_info->db_type = JER_PP_EM_SER_DB_TYPE_OEMB;
            ser_info->req_mem = MANAGEMENT_REQUEST_MEM(PPDB_A, OEMB);
            ARAD_TBL_REF(unit, ser_info->req_mem, KEY_FIELD(OEMB), &fld);
            ser_info->key_start_bit = ARAD_FLD_LSB(fld);
            ser_info->key_size = ARAD_FLD_NOF_BITS(fld);
            ARAD_TBL_REF(unit, ser_info->req_mem, PAYLOAD_FIELD(OEMB), &fld);
            ser_info->payload_start_bit = ARAD_FLD_LSB(fld);
            ser_info->payload_size = ARAD_FLD_NOF_BITS(fld);
            ser_info->shadow_key_size = JER_CHIP_SIM_OEMB_KEY;
            ser_info->shadow_payload_size = JER_CHIP_SIM_OEMB_PAYLOAD;
            ser_info->shadow_base = JER_CHIP_SIM_OEMB_BASE;
            ser_info->db_nof_lines = soc_mem_index_count(unit, mem) * SOC_MEM_NUMELS(unit, mem);
            ser_info->diagnostics_reg = DIAGNOSTICS_REGISTER(PPDB_A, OEMB);
            ser_info->diagnostics_key_reg = DIAGNOSTICS_KEY_REGISTER(PPDB_A, OEMB);
            ser_info->diagnostics_lookup_result_reg = DIAGNOSTICS_LOOKUP_RESULT_REGISTER(PPDB_A, OEMB);
            ser_info->diagnostics_lookup_field = DIAGNOSTICS_LOOKUP_FIELD(OEMB);
            ser_info->diagnostics_read_result_reg = DIAGNOSTICS_READ_RESULT_REGISTER(PPDB_A, OEMB);
            ser_info->diagnostics_read_field = DIAGNOSTICS_READ_FIELD(OEMB);
            ser_info->diagnostics_index_reg = DIAGNOSTICS_INDEX_REGISTER(PPDB_A, OEMB);
            ser_info->keyt_aux_mem = KEYT_AUX_MEM(PPDB_A, OEMB);
            ser_info->pldt_aux_mem = PLDT_AUX_MEM(PPDB_A, OEMB);
            ser_info->keyt_pldt_mem = mem;
            ser_info->counter_reg = ENTRIES_COUNTER_REGISTER(PPDB_A, OEMB);

            ser_info->completed_reg = PPDB_A_OEMB_INTERRUPT_REGISTER_ONEr;
            ser_info->completed_field = OEMB_MANAGEMENT_COMPLETEDf;
            ser_info->enable_dynamic_reg = PPDB_A_ENABLE_DYNAMIC_MEMORY_ACCESSr;
            break;

        case KEYT_PLDT_MEM(OAMP, REMOTE_MEP_EXACT_MATCH):
            ser_info->db_type = JER_PP_EM_SER_DB_TYPE_RMAPEM;
            ser_info->req_mem = MANAGEMENT_REQUEST_MEM(OAMP, RMAPEM);
            ARAD_TBL_REF(unit, ser_info->req_mem, KEY_FIELD(RMAPEM), &fld);
            ser_info->key_start_bit = ARAD_FLD_LSB(fld);
            ser_info->key_size = ARAD_FLD_NOF_BITS(fld);
            ARAD_TBL_REF(unit, ser_info->req_mem, PAYLOAD_FIELD(RMAPEM), &fld);
            ser_info->payload_start_bit = ARAD_FLD_LSB(fld);
            ser_info->payload_size = ARAD_FLD_NOF_BITS(fld);
            ser_info->shadow_key_size = JER_CHIP_SIM_RMAPEM_KEY;
            ser_info->shadow_payload_size = JER_CHIP_SIM_RMAPEM_PAYLOAD;
            ser_info->shadow_base = JER_CHIP_SIM_RMAPEM_BASE;
            ser_info->db_nof_lines = soc_mem_index_count(unit, mem) * SOC_MEM_NUMELS(unit, mem);
            ser_info->diagnostics_reg = DIAGNOSTICS_REGISTER(OAMP, REMOTE_MEP_EXACT_MATCH);
            ser_info->diagnostics_key_reg = DIAGNOSTICS_KEY_REGISTER(OAMP, REMOTE_MEP_EXACT_MATCH);
            ser_info->diagnostics_lookup_result_reg = DIAGNOSTICS_LOOKUP_RESULT_REGISTER(OAMP, REMOTE_MEP_EXACT_MATCH);
            ser_info->diagnostics_lookup_field = DIAGNOSTICS_LOOKUP_FIELD(REMOTE_MEP_EXACT_MATCH);
            ser_info->diagnostics_read_result_reg = DIAGNOSTICS_READ_RESULT_REGISTER(OAMP, REMOTE_MEP_EXACT_MATCH);
            ser_info->diagnostics_read_field = DIAGNOSTICS_READ_FIELD(REMOTE_MEP_EXACT_MATCH);
            ser_info->diagnostics_index_reg = DIAGNOSTICS_INDEX_REGISTER(OAMP, REMOTE_MEP_EXACT_MATCH);
            ser_info->keyt_aux_mem = KEYT_AUX_MEM(OAMP, REMOTE_MEP_EXACT_MATCH);
            ser_info->pldt_aux_mem = PLDT_AUX_MEM(OAMP, REMOTE_MEP_EXACT_MATCH);
            ser_info->keyt_pldt_mem = mem;
            ser_info->counter_reg = ENTRIES_COUNTER_REGISTER(OAMP, REMOTE_MEP_EXACT_MATCH);

            ser_info->completed_reg = OAMP_REMOTE_MEP_EXACT_MATCH_INTERRUPT_REGISTER_ONEr;
            ser_info->completed_field = RMAPEM_MANAGEMENT_COMPLETEDf;
            ser_info->enable_dynamic_reg = OAMP_ENABLE_DYNAMIC_MEMORY_ACCESSr;
            break;

        default:
            break;
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

uint32 jer_pp_em_ser_mask_interrupt_set(int unit,
                                         soc_mem_t mem,
                                         uint32 val)
{
    uint32 res = SOC_SAND_OK;
    soc_reg_above_64_val_t reg_above_64_val, mask_val_above_64;
    uint64 reg_64_val, mask_val_64;
    uint32 reg_32_val, mask_val_32;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
    SOC_REG_ABOVE_64_CLEAR(mask_val_above_64);
    mask_val_32 = val;
    COMPILER_64_SET(mask_val_64,0,val);
    mask_val_above_64[0] = val;

    switch (mem) {
        case KEYT_PLDT_MEM(PPDB_B, LARGE_EM):
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_PPDB_B_ECC_ERR_2B_MONITOR_MEM_MASKr(unit, &reg_32_val));
            MASK_SER_INTERRUPTS_32B_SET(MACT, LEM_ERR_MEM_MASK_REGISTER, &reg_32_val, mask_val_32);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_PPDB_B_ECC_ERR_2B_MONITOR_MEM_MASKr(unit, reg_32_val));
            break;

        case KEYT_PLDT_MEM(IHB, ISEM):
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_IHB_ECC_ERR_2B_MONITOR_MEM_MASKr(unit, 0, &reg_32_val));
            MASK_SER_INTERRUPTS_32B_SET(ISEM, ISEM_ERR_MEM_MASK_REGISTER, &reg_32_val, mask_val_32);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, WRITE_IHB_ECC_ERR_2B_MONITOR_MEM_MASKr(unit, 0, reg_32_val));
            break;

        case KEYT_PLDT_MEM(PPDB_A, ISEM):
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_PPDB_A_ECC_ERR_2B_MONITOR_MEM_MASKr(unit, &reg_64_val));
            MASK_SER_INTERRUPTS_64B_SET(ISEM, PPDB_A_ERR_MEM_MASK_REGISTER, &reg_64_val, mask_val_64);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, WRITE_PPDB_A_ECC_ERR_2B_MONITOR_MEM_MASKr(unit, reg_64_val));
            break;

        case KEYT_PLDT_MEM(PPDB_B, ISEM):
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_PPDB_B_ECC_ERR_2B_MONITOR_MEM_MASKr(unit, &reg_32_val));
            MASK_SER_INTERRUPTS_32B_SET(ISEM, PPDB_B_ERR_MEM_MASK_REGISTER, &reg_32_val, mask_val_32);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, WRITE_PPDB_B_ECC_ERR_2B_MONITOR_MEM_MASKr(unit, reg_32_val));
            break;

        case KEYT_PLDT_MEM(EDB, GLEM):
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_EDB_ECC_ERR_2B_MONITOR_MEM_MASKr(unit, reg_above_64_val));
            MASK_SER_INTERRUPTS_ABOVE_64B_SET(GLEM, GLEM_ERR_MEM_MASK_REGISTER, reg_above_64_val, mask_val_above_64);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, WRITE_EDB_ECC_ERR_2B_MONITOR_MEM_MASKr(unit, reg_above_64_val));
            break;

        case KEYT_PLDT_MEM(EDB, ESEM):
        case KEYT_PLDT_MEM(EDB, ESEM_0):
        case KEYT_PLDT_MEM(EDB, ESEM_1):
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, READ_EDB_ECC_ERR_2B_MONITOR_MEM_MASKr(unit, reg_above_64_val));
            MASK_SER_INTERRUPTS_ABOVE_64B_SET(ESEM, ESEM_ERR_MEM_MASK_REGISTER, reg_above_64_val, mask_val_above_64);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, WRITE_EDB_ECC_ERR_2B_MONITOR_MEM_MASKr(unit, reg_above_64_val));
            break;

        case KEYT_PLDT_MEM(PPDB_A, OEMA):
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 70, exit, READ_PPDB_A_ECC_ERR_2B_MONITOR_MEM_MASKr(unit, &reg_64_val));
            MASK_SER_INTERRUPTS_64B_SET(OEMA, OEMA_ERR_MEM_MASK_REGISTER, &reg_64_val, mask_val_64);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 80, exit, WRITE_PPDB_A_ECC_ERR_2B_MONITOR_MEM_MASKr(unit, reg_64_val));
            break;

        case KEYT_PLDT_MEM(PPDB_A, OEMB):
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 90, exit, READ_PPDB_A_ECC_ERR_2B_MONITOR_MEM_MASKr(unit, &reg_64_val));
            MASK_SER_INTERRUPTS_64B_SET(OEMB, OEMB_ERR_MEM_MASK_REGISTER, &reg_64_val, mask_val_64);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 100, exit, WRITE_PPDB_A_ECC_ERR_2B_MONITOR_MEM_MASKr(unit, reg_64_val));
            break;

        case KEYT_PLDT_MEM(OAMP, REMOTE_MEP_EXACT_MATCH):
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, READ_OAMP_ECC_ERR_2B_MONITOR_MEM_MASKr_REG64(unit, &reg_64_val));
            MASK_SER_INTERRUPTS_64B_SET(RMAPEM, RMAPEM_ERR_MEM_MASK_REGISTER, &reg_64_val, mask_val_64);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 120, exit, WRITE_OAMP_ECC_ERR_2B_MONITOR_MEM_MASKr_REG64(unit, reg_64_val));
            break;

        default:
            res = SOC_SAND_ERR;
            break;
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

uint32 jer_pp_em_ser_management_enable_set(int unit,
                                            soc_mem_t mem,
                                            uint32 block_instance,
                                            int val)
{
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    switch (mem) {
        case KEYT_PLDT_MEM(PPDB_B, LARGE_EM):
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, MANAGEMENT_UNIT_CONFIGURATION_REGISTER(PPDB_B,LARGE_EM), block_instance, 0, MNGMNT_UNIT_ENABLE_FIELD(LARGE_EM),  val));
            break;

        case KEYT_PLDT_MEM(IHB, ISEM):
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, MANAGEMENT_UNIT_CONFIGURATION_REGISTER(IHB,ISEM), block_instance, 0, MNGMNT_UNIT_ENABLE_FIELD(ISEM),  val));
            break;

        case KEYT_PLDT_MEM(PPDB_A, ISEM):
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, MANAGEMENT_UNIT_CONFIGURATION_REGISTER(PPDB_A,ISEM), block_instance, 0, MNGMNT_UNIT_ENABLE_FIELD(ISEM),  val));
            break;

        case KEYT_PLDT_MEM(PPDB_B, ISEM):
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, MANAGEMENT_UNIT_CONFIGURATION_REGISTER(PPDB_B,ISEM), block_instance, 0, MNGMNT_UNIT_ENABLE_FIELD(ISEM),  val));
            break;

        case KEYT_PLDT_MEM(EDB, GLEM):
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, MANAGEMENT_UNIT_CONFIGURATION_REGISTER(EDB,GLEM), block_instance, 0, MNGMNT_UNIT_ENABLE_FIELD(GLEM),  val));
            break;

        case KEYT_PLDT_MEM(EDB, ESEM):
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, MANAGEMENT_UNIT_CONFIGURATION_REGISTER(EDB,ESEM), block_instance, 0, MNGMNT_UNIT_ENABLE_FIELD(ESEM),  val));
            break;

        case KEYT_PLDT_MEM(EDB, ESEM_0):
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, MANAGEMENT_UNIT_CONFIGURATION_REGISTER(EDB,ESEM_0), block_instance, 0, MNGMNT_UNIT_ENABLE_FIELD(ESEM_0),  val));
            break;

        case KEYT_PLDT_MEM(EDB, ESEM_1):
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, MANAGEMENT_UNIT_CONFIGURATION_REGISTER(EDB,ESEM_1), block_instance, 0, MNGMNT_UNIT_ENABLE_FIELD(ESEM_1),  val));
            break;

        case KEYT_PLDT_MEM(PPDB_A, OEMA):
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  90,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, MANAGEMENT_UNIT_CONFIGURATION_REGISTER(PPDB_A,OEMA), block_instance, 0, MNGMNT_UNIT_ENABLE_FIELD(OEMA),  val));
            break;

        case KEYT_PLDT_MEM(PPDB_A, OEMB):
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  100,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, MANAGEMENT_UNIT_CONFIGURATION_REGISTER(PPDB_A,OEMB), block_instance, 0, MNGMNT_UNIT_ENABLE_FIELD(OEMB),  val));
            break;

        case KEYT_PLDT_MEM(OAMP, REMOTE_MEP_EXACT_MATCH):
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  110,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, MANAGEMENT_UNIT_CONFIGURATION_REGISTER(OAMP,REMOTE_MEP_EXACT_MATCH), block_instance, 0, MNGMNT_UNIT_ENABLE_FIELD(REMOTE_MEP_EXACT_MATCH),  val));
            break;

        default:
            res = SOC_SAND_ERR;
            break;
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}
int jer_pp_em_ser_ecc_error_info_get(int unit,
                                         JER_PP_EM_SER_BLOCK block,
                                         uint32 *address,
                                         uint32 *address_valid,
                                         uint32 *counter,
                                         uint32 *counter_overflow)
{
    int res;
    uint64 reg_val;
    soc_reg_t ecc_err_reg;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(address);
    SOC_SAND_CHECK_NULL_INPUT(address_valid);
    SOC_SAND_CHECK_NULL_INPUT(counter);
    SOC_SAND_CHECK_NULL_INPUT(counter_overflow);

    switch (block) {
       case JER_PP_EM_SER_BLOCK_IHB:
           ecc_err_reg = ECC_2B_ERR_CNT_REGISTER(IHB);
           break;

       case JER_PP_EM_SER_BLOCK_PPDB_A:
           ecc_err_reg = ECC_2B_ERR_CNT_REGISTER(PPDB_A);
           break;

       case JER_PP_EM_SER_BLOCK_PPDB_B:
           ecc_err_reg = ECC_2B_ERR_CNT_REGISTER(PPDB_B);
           break;

       case JER_PP_EM_SER_BLOCK_EDB:
           ecc_err_reg = ECC_2B_ERR_CNT_REGISTER(EDB);
           break;

       case JER_PP_EM_SER_BLOCK_OAMP:
           ecc_err_reg = ECC_2B_ERR_CNT_REGISTER(OAMP);
           break;

       default:
           res = SOC_SAND_ERR;
           goto exit;
    }
    res = soc_reg_get(unit, ecc_err_reg, REG_PORT_ANY, 0, &reg_val);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

    
    *address_valid = soc_reg64_field32_get(unit, ecc_err_reg, reg_val, ECC_2B_ERR_ADDR_VALIDf);

    
    *address = soc_reg64_field32_get(unit, ecc_err_reg, reg_val, ECC_2B_ERR_ADDRf);

     
    *counter_overflow = soc_reg64_field32_get(unit, ecc_err_reg, reg_val, ECC_2B_ERR_CNT_OVERFLOWf);

    
    *counter = soc_reg64_field32_get(unit, ecc_err_reg, reg_val, ECC_2B_ERR_CNTf);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}


uint32 jer_pp_em_lem_counter_increment(int unit, int inc_val)
{
    uint32 i, j, is_found=0;
    uint32 key_data[SOC_DPP_DEFS_MAX_LEM_WIDTH_IN_UINT32S];
    uint32 verifier_data[JER_PP_EM_SER_LEM_VERIFIER_LEN_IN_UINT32] = {0};
    soc_reg_above_64_val_t reg_above_64_val, request_val, fld_value, tmp, dummy_key_value;
    uint32 *entry_index, entry_location;
    uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
    SOC_REG_ABOVE_64_CLEAR(fld_value);
    SOC_REG_ABOVE_64_CLEAR(dummy_key_value);
    SOC_REG_ABOVE_64_CLEAR(tmp);
    SOC_REG_ABOVE_64_CLEAR(request_val);

    if (SOC_IS_QUX(unit)) {
        entry_index = qux_entry_index[JER_PP_EM_SER_DB_TYPE_LEM];
    }
    else if (SOC_IS_QAX(unit)) {
        entry_index = qax_entry_index[JER_PP_EM_SER_DB_TYPE_LEM];
    }
    else if (SOC_IS_JERICHO_PLUS(unit)) {
        entry_index = jerp_entry_index[JER_PP_EM_SER_DB_TYPE_LEM];
    }
    else {
        entry_index = jer_entry_index[JER_PP_EM_SER_DB_TYPE_LEM];
    }

    sal_memset(tmp, 0xff, sizeof(soc_reg_above_64_val_t));
    res = soc_sand_bitstream_set_any_field(
          tmp,
          0,
          SOC_DPP_DEFS_MAX(LEM_WIDTH),
          dummy_key_value
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    for (i = 0; i < inc_val; i++) {
        
        fld_value[0] = 1;
        ARAD_FLD_TO_REG_ABOVE_64(PPDB_B_LARGE_EM_CPU_REQUEST_REQUESTr, LARGE_EM_REQ_MFF_IS_KEYf, fld_value, request_val,  15, exit);
        ARAD_FLD_TO_REG_ABOVE_64(PPDB_B_LARGE_EM_CPU_REQUEST_REQUESTr, LARGE_EM_REQ_MFF_KEYf, dummy_key_value, request_val,  15, exit);
        ARAD_FLD_TO_REG_ABOVE_64(PPDB_B_LARGE_EM_CPU_REQUEST_REQUESTr, LARGE_EM_REQ_COMMANDf, fld_value, request_val,  15, exit);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_PPDB_B_LARGE_EM_CPU_REQUEST_REQUESTr(unit, request_val));

        
        for (j = 0; j < SOC_DPP_DEFS_GET(unit, nof_lem_banks); j++) {
            
            entry_location = entry_index[j] + (JER_CHIP_SIM_LEM_TABLE_SIZE/SOC_DPP_DEFS_GET(unit, nof_lem_banks)) * j;
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, WRITE_PPDB_B_LARGE_EM_DIAGNOSTICS_INDEXr(unit, entry_location));
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR, soc_reg_field32_modify(unit, PPDB_B_LARGE_EM_DIAGNOSTICSr, REG_PORT_ANY, LARGE_EM_DIAGNOSTICS_READf, 1));

            
            res = arad_polling(
                      unit,
                      ARAD_TIMEOUT,
                      ARAD_MIN_POLLS,
                      PPDB_B_LARGE_EM_DIAGNOSTICSr,
                      REG_PORT_ANY,
                      0,
                      LARGE_EM_DIAGNOSTICS_READf,
                      0
                    );
            SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, READ_PPDB_B_LARGE_EM_DIAGNOSTICS_READ_RESULTr(unit, reg_above_64_val));

            key_data[0] = 0;
            key_data[1] = 0;
            key_data[2] = 0;

            
            res = soc_sand_bitstream_get_any_field(
                  reg_above_64_val,
                  1, 
                  SOC_DPP_DEFS_MAX(LEM_WIDTH), 
                  key_data
                );
            SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);


            if ((key_data[0] == dummy_key_value[0]) &&
                (key_data[1] == dummy_key_value[1]) &&
                (key_data[2] == dummy_key_value[2])) {
                
                is_found = 1;

                
                res = soc_mem_array_write(unit,
                                KEYT_PLDT_MEM(PPDB_B, LARGE_EM),
                                j,
                                MEM_BLOCK_ANY,
                                entry_index[j],
                                verifier_data);
                SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

                break;
            }
        }
        if (!is_found) {
            
            for (j=0; j < JER_PP_EM_SER_EM_AUX_CAM_SIZE; j++) {
                res = soc_mem_read(unit,
                                   KEYT_AUX_MEM(PPDB_B, LARGE_EM),
                                   MEM_BLOCK_ANY,
                                   j,
                                   reg_above_64_val);
                SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

                if ((reg_above_64_val[0] == dummy_key_value[0]) &&
                    (reg_above_64_val[1] == dummy_key_value[1]) &&
                    (reg_above_64_val[2] == dummy_key_value[2])) {
                    
                    is_found = 1;
                    key_data[0] = 0;
                    key_data[1] = 0;
                    key_data[2] = 0;

                    
                    res = soc_mem_write(unit,
                                    KEYT_AUX_MEM(PPDB_B, LARGE_EM),
                                    MEM_BLOCK_ANY,
                                    j,
                                    key_data);
                    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
                    break;
                }
            }
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}


uint32 jer_pp_em_em_counter_increment(int unit, JER_PP_SER_EM_TYPE_INFO *ser_info, int inc_val)
{
    uint32 i, j, is_found=0;
    uint32 key_data[2] = {0};
    uint32 verifier_data[2] = {0};
    soc_reg_above_64_val_t reg_above_64_val, tmp, dummy_key_value, read_result;
    uint32 *entry_index, entry_location, temp, buffer[3] = {0};
    uint32 res = SOC_SAND_OK;
    uint32 hash, enable_dynamic = 0, dynamic_flag = 0;
    int mem_block;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    hash = SOC_MEM_NUMELS(unit, ser_info->keyt_pldt_mem);

    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
    SOC_REG_ABOVE_64_CLEAR(dummy_key_value);
    SOC_REG_ABOVE_64_CLEAR(tmp);
    SOC_REG_ABOVE_64_CLEAR(read_result);

    
    if (SOC_IS_QUX(unit)) {
        entry_index = qux_entry_index[ser_info->db_type];
    }
    else if (SOC_IS_QAX(unit)) {
        entry_index = qax_entry_index[ser_info->db_type];
    }
    else if (SOC_IS_JERICHO_PLUS(unit)) {
        entry_index = jerp_entry_index[ser_info->db_type];
    }
    else {
        entry_index = jer_entry_index[ser_info->db_type];
    }

    
    res = soc_reg32_get(unit, ser_info->enable_dynamic_reg, ser_info->block_instance, 0, &enable_dynamic);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);
    dynamic_flag = 1;

    res = soc_reg32_set(unit, ser_info->enable_dynamic_reg, ser_info->block_instance, 0, 1);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 8, exit);

    
    sal_memset(tmp, 0xff, sizeof(soc_reg_above_64_val_t));
    res = soc_sand_bitstream_set_any_field(
          tmp,
          0,
          ser_info->key_size,
          dummy_key_value
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    for (i = 0; i < inc_val; i++) {
        

        
        temp = JER_PP_EM_SER_REQUEST_TYPE_INSERT_VAL;
        res = soc_sand_bitstream_set_any_field(
              &temp,
              JER_PP_EM_SER_REQUEST_TYPE_START_BIT,
              JER_PP_EM_SER_REQUEST_TYPE_SIZE,
              buffer
            );
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

        
        res = soc_sand_bitstream_set_any_field(
              dummy_key_value,
              ser_info->key_start_bit,
              ser_info->key_size,
              buffer
            );
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

        
        mem_block = ser_info->block_instance + SOC_MEM_BLOCK_MIN(unit, ser_info->req_mem);
        res = soc_mem_write(unit, ser_info->req_mem, mem_block, 0, buffer);
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
        res = arad_polling(
                      unit,
                      ARAD_TIMEOUT,
                      ARAD_MIN_POLLS,
                      ser_info->completed_reg,
                      ser_info->block_instance,
                      0,
                      ser_info->completed_field,
                      1
                    );
        SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

        res = soc_reg_above_64_field32_modify(unit, ser_info->completed_reg, ser_info->block_instance, 
                                          0, ser_info->completed_field, 1);
        SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

        
        for (j = 0; j < hash; j++) {
            
            entry_location = entry_index[j] + ((ser_info->db_nof_lines)/hash) * j;
            res = soc_reg32_set(unit, ser_info->diagnostics_index_reg, ser_info->block_instance, 0, entry_location);
            SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

            res = soc_reg_above_64_field32_modify(unit, ser_info->diagnostics_reg, ser_info->block_instance, 
                                                  0, ser_info->diagnostics_read_field, 1);
            SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

            
            res = arad_polling(
                      unit,
                      ARAD_TIMEOUT,
                      ARAD_MIN_POLLS,
                      ser_info->diagnostics_reg,
                      ser_info->block_instance,
                      0,
                      ser_info->diagnostics_read_field,
                      0
                    );
            SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
            res = soc_reg_above_64_get(unit, ser_info->diagnostics_read_result_reg, ser_info->block_instance, 0, read_result);
            SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

            
            res = soc_sand_bitstream_get_any_field(
                  read_result,
                  1, 
                  ser_info->key_size, 
                  key_data
                );
            SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);


            if ((key_data[0] == dummy_key_value[0]) && (key_data[1] == dummy_key_value[1])) 
            {
                is_found = 1;

                
                mem_block = ser_info->block_instance + SOC_MEM_BLOCK_MIN(unit, ser_info->keyt_pldt_mem);
                res = soc_mem_array_write(unit,
                                ser_info->keyt_pldt_mem,
                                j,
                                mem_block,
                                entry_index[j],
                                verifier_data);
                SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
                break;
            }
        }
        if (!is_found) {
            
            for (j=0; j < JER_PP_EM_SER_EM_AUX_CAM_SIZE; j++) 
            {
                mem_block = ser_info->block_instance + SOC_MEM_BLOCK_MIN(unit, ser_info->keyt_aux_mem);
                res = soc_mem_read(unit,
                                   ser_info->keyt_aux_mem,
                                   mem_block,
                                   j,
                                   reg_above_64_val);
                SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

                if ((reg_above_64_val[0] == dummy_key_value[0]) &&
                    (reg_above_64_val[1] == dummy_key_value[1])) {
                    
                    is_found = 1;
                    key_data[0] = 0;
                    key_data[1] = 0;

                    
                    res = soc_mem_write(unit,
                                    ser_info->keyt_aux_mem,
                                    mem_block,
                                    j,
                                    key_data);
                    SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
                    break;
                }
            }
        }
    }

exit:
    
    if (dynamic_flag == 1)
    {
        dynamic_flag = 0;
        res = soc_reg32_set(unit, ser_info->enable_dynamic_reg, ser_info->block_instance, 0, enable_dynamic);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 130, exit);
    }

    SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}


uint32 jer_pp_em_lem_counter_decrement(int unit, int dec_val)
{
    uint32 res = SOC_SAND_OK;
    uint32 is_valid;
    int i,j;
    soc_reg_above_64_val_t reg_above_64_val, request_val, fld_value, tmp;
    soc_reg_above_64_val_t dummy_key_value, dummy_key_value_valid;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
    SOC_REG_ABOVE_64_CLEAR(dummy_key_value);
    SOC_REG_ABOVE_64_CLEAR(dummy_key_value_valid);
    SOC_REG_ABOVE_64_CLEAR(request_val);
    SOC_REG_ABOVE_64_CLEAR(fld_value);
    SOC_REG_ABOVE_64_CLEAR(tmp);

    
    sal_memset(tmp, 0xff, sizeof(soc_reg_above_64_val_t));
    res = soc_sand_bitstream_set_any_field(
          tmp,
          0,
          SOC_DPP_DEFS_MAX(LEM_WIDTH) + 1, 
          dummy_key_value_valid
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    res = soc_sand_bitstream_set_any_field(
          tmp,
          0,
          SOC_DPP_DEFS_MAX(LEM_WIDTH),
          dummy_key_value
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

    for (i = 0; i < dec_val; i++) {
        
        for (j=0; j < JER_PP_EM_SER_EM_AUX_CAM_SIZE; j++) {
            res = soc_mem_read(unit,
                               KEYT_AUX_MEM(PPDB_B, LARGE_EM),
                               MEM_BLOCK_ANY,
                               j,
                               reg_above_64_val);
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

            res = soc_sand_bitstream_get_any_field(
                  reg_above_64_val,
                  0, 
                  1, 
                  &is_valid);
            SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

            if (!is_valid) {
                
                break;
            }
        }

        if (j == JER_PP_EM_SER_EM_AUX_CAM_SIZE) {
            
            res = SOC_SAND_ERR;
            goto exit;;
        }

        
        res = soc_mem_write(unit,
                            KEYT_AUX_MEM(PPDB_B, LARGE_EM),
                            MEM_BLOCK_ANY,
                            j, 
                            dummy_key_value_valid);
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

        
        fld_value[0] = 1;
        ARAD_FLD_TO_REG_ABOVE_64(PPDB_B_LARGE_EM_CPU_REQUEST_REQUESTr, LARGE_EM_REQ_MFF_IS_KEYf, fld_value, request_val,  60, exit);
        ARAD_FLD_TO_REG_ABOVE_64(PPDB_B_LARGE_EM_CPU_REQUEST_REQUESTr, LARGE_EM_REQ_MFF_KEYf, dummy_key_value, request_val,  70, exit);
        fld_value[0] = 0;
        ARAD_FLD_TO_REG_ABOVE_64(PPDB_B_LARGE_EM_CPU_REQUEST_REQUESTr, LARGE_EM_REQ_COMMANDf, fld_value, request_val,  80, exit);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 1000, exit, WRITE_PPDB_B_LARGE_EM_CPU_REQUEST_REQUESTr(unit, request_val));
    }
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}


uint32 jer_pp_em_em_counter_decrement(int unit, JER_PP_SER_EM_TYPE_INFO *ser_info, int dec_val)
{
    uint32 res = SOC_SAND_OK;
    uint32 is_valid, temp, buffer[3] = {0};
    int i,j;
    soc_reg_above_64_val_t reg_above_64_val, request_val, tmp;
    soc_reg_above_64_val_t dummy_key_value, dummy_key_value_valid;
    int mem_block;
    uint32 enable_dynamic = 0, dynamic_flag = 0;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
    SOC_REG_ABOVE_64_CLEAR(dummy_key_value);
    SOC_REG_ABOVE_64_CLEAR(dummy_key_value_valid);
    SOC_REG_ABOVE_64_CLEAR(request_val);
    SOC_REG_ABOVE_64_CLEAR(tmp);

    
    res = soc_reg32_get(unit, ser_info->enable_dynamic_reg, ser_info->block_instance, 0, &enable_dynamic);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);
    dynamic_flag = 1;

    res = soc_reg32_set(unit, ser_info->enable_dynamic_reg, ser_info->block_instance, 0, 1);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 8, exit);

    
    sal_memset(tmp, 0xff, sizeof(soc_reg_above_64_val_t));
    res = soc_sand_bitstream_set_any_field(
          tmp,
          0,
          ser_info->key_size + 1, 
          dummy_key_value_valid
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    res = soc_sand_bitstream_set_any_field(
          tmp,
          0,
          ser_info->key_size,
          dummy_key_value
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

    for (i = 0; i < dec_val; i++) {
        
        for (j=0; j < JER_PP_EM_SER_EM_AUX_CAM_SIZE; j++) 
        {
            mem_block = ser_info->block_instance + SOC_MEM_BLOCK_MIN(unit, ser_info->keyt_aux_mem);
            res = soc_mem_read(unit,
                               ser_info->keyt_aux_mem,
                               mem_block,
                               j,
                               reg_above_64_val);
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

            res = soc_sand_bitstream_get_any_field(
                  reg_above_64_val,
                  0, 
                  1, 
                  &is_valid);
            SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

            if (!is_valid) {
                
                break;
            }
        }

        if (j == JER_PP_EM_SER_EM_AUX_CAM_SIZE) {
            
            goto exit;
        }

        
        mem_block = ser_info->block_instance + SOC_MEM_BLOCK_MIN(unit, ser_info->keyt_aux_mem);
        res = soc_mem_write(unit,
                            ser_info->keyt_aux_mem,
                            mem_block,
                            j, 
                            dummy_key_value_valid);
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

        
        
        temp = JER_PP_EM_SER_REQUEST_TYPE_DELETE_VAL;
        res = soc_sand_bitstream_set_any_field(
              &temp,
              JER_PP_EM_SER_REQUEST_TYPE_START_BIT,
              JER_PP_EM_SER_REQUEST_TYPE_SIZE,
              buffer
            );
        SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

        
        res = soc_sand_bitstream_set_any_field(
              dummy_key_value,
              ser_info->key_start_bit,
              ser_info->key_size,
              buffer
            );
        SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

        
        mem_block = ser_info->block_instance + SOC_MEM_BLOCK_MIN(unit, ser_info->req_mem);
        res = soc_mem_write(unit, ser_info->req_mem, mem_block, 0, buffer);
        SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
        res = arad_polling(
                      unit,
                      ARAD_TIMEOUT,
                      ARAD_MIN_POLLS,
                      ser_info->completed_reg,
                      ser_info->block_instance,
                      0,
                      ser_info->completed_field,
                      1
                    );
        SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

        res = soc_reg_above_64_field32_modify(unit, ser_info->completed_reg, ser_info->block_instance, 
                                          0, ser_info->completed_field, 1);
        SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
    }

exit:
    if (dynamic_flag == 1)
    {
        
        dynamic_flag = 0;
        res = soc_reg32_set(unit, ser_info->enable_dynamic_reg, ser_info->block_instance, 0, enable_dynamic);
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 110, exit);
    }

    SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

uint32 jer_pp_em_ser_mact_counter_fix(int unit, uint32 nof_mact_matched_entries)
{
    uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    res = WRITE_PPDB_B_LARGE_EM_COUNTER_LIMIT_LARGE_EM_DB_ENTRIES_COUNTr(unit, nof_mact_matched_entries);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}


uint32 jer_pp_em_ser_lem_counter_diff_get(int unit, int *lem_counter_diff, uint32* nof_matched_entrie)
{
    uint32 res = SOC_SAND_OK;
    uint32 nof_matched_lem_entries, nof_lem_entries, temp_nof_matched_lem_entries;
    SOC_SAND_TABLE_BLOCK_RANGE                      block_range;
    SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION              action;
    SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE          rule;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_clear(&action);
    soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&block_range);
    SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE_clear(&rule);
    nof_matched_lem_entries = nof_lem_entries = temp_nof_matched_lem_entries = 0;

    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_PPDB_B_LARGE_EM_COUNTER_LIMIT_LARGE_EM_DB_ENTRIES_COUNTr(unit, &nof_lem_entries));

    SOC_SAND_TBL_ITER_SET_BEGIN(&(block_range.iter));
    if (DPP_PP_LEM_NOF_ENTRIES(unit) < (1 << JER_PP_EM_SER_LEM_FLU_MACHINE_HIT_COUNTER_SIZE))
    {
        block_range.entries_to_act = DPP_PP_LEM_NOF_ENTRIES(unit);
    }
    else
    {
        block_range.entries_to_act = ((1 << JER_PP_EM_SER_LEM_FLU_MACHINE_HIT_COUNTER_SIZE) - 1);
    }
    block_range.entries_to_scan = SOC_SAND_TBL_ITER_SCAN_ALL;
    rule.key_type = SOC_PPC_FRWRD_MACT_KEY_TYPE_MAC_ADDR;

    while(!SOC_SAND_TBL_ITER_IS_END(&(block_range.iter))) {
        
        res = arad_pp_frwrd_mact_get_counter_by_block_unsafe(unit,
                                                            &rule,
                                                            &block_range,
                                                            &temp_nof_matched_lem_entries);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        nof_matched_lem_entries += temp_nof_matched_lem_entries;
    }

    *lem_counter_diff = nof_matched_lem_entries - nof_lem_entries;
    *nof_matched_entrie = nof_matched_lem_entries;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}


uint32 jer_pp_em_ser_em_counter_diff_get(int unit, JER_PP_SER_EM_TYPE_INFO *ser_info, int *em_counter_diff)
{
    uint32 res = SOC_SAND_OK;
    uint32 nof_valid_entries=0, nof_em_entries, is_valid, entry_index, array_index;
    uint32 key_data[SOC_DPP_DEFS_MAX_LEM_WIDTH_IN_UINT32S] = {0};
    uint32 hash;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    hash = SOC_MEM_NUMELS(unit, ser_info->keyt_pldt_mem);

    
    res = soc_reg32_get(unit, ser_info->counter_reg, ser_info->block_instance, 0, &nof_em_entries);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    for (array_index = SOC_MEM_FIRST_ARRAY_INDEX(unit, ser_info->keyt_pldt_mem); array_index < hash; array_index++) {
        for (entry_index=0; entry_index < (ser_info->db_nof_lines/hash); entry_index++) {
            is_valid = 0;

            res = soc_mem_array_read(unit,
                                     ser_info->keyt_pldt_mem,
                                     array_index,
                                     ser_info->block_instance + SOC_MEM_BLOCK_MIN(unit, ser_info->keyt_pldt_mem),
                                     entry_index,
                                     &key_data);
            SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

            
            res = soc_sand_bitstream_get_any_field(
                  key_data,
                  0, 
                  1, 
                  &is_valid
                );
            SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

            if (is_valid) {
                nof_valid_entries++;
            }
        }
    }

    *em_counter_diff = nof_valid_entries - nof_em_entries;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}



uint32 jer_pp_em_ser_lem_fid_counters_update(int unit)
{
    uint32 res = SOC_SAND_OK;
    uint32 nof_entries, indx, fid;
    uint32 tbl_data[1];
    SOC_SAND_TABLE_BLOCK_RANGE                      block_range;
    SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE          rule;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&block_range);
    SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE_clear(&rule);

    sal_memset(lem_fid_lif_counters, 0, sizeof(uint16)*JER_PP_EM_SER_LEM_NOF_FIDS);

    
    SOC_SAND_TBL_ITER_SET_BEGIN(&(block_range.iter));
    block_range.entries_to_act = JER_PP_EM_SER_LEM_TRVRS_ITER_BLK_SIZE;
    block_range.entries_to_scan = SOC_SAND_TBL_ITER_SCAN_ALL;

    rule.key_type = SOC_PPC_FRWRD_MACT_KEY_TYPE_MAC_ADDR;

    while(!SOC_SAND_TBL_ITER_IS_END(&(block_range.iter))) {
        
        res = soc_ppd_frwrd_mact_get_block(unit,
                                           &rule,
                                           SOC_PPC_FRWRD_MACT_TABLE_SW_HW,
                                           &block_range,
                                           l2_traverse_mact_keys,
                                           l2_traverse_mact_vals,
                                           &nof_entries);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        if(nof_entries == 0) {
            break;
        }
        for (indx = 0; indx < nof_entries; ++indx) {
            fid = l2_traverse_mact_keys[indx].key_val.mac.fid;
            
            lem_fid_lif_counters[fid]++;
        }
    }

    
    for (indx = 0; indx < JER_PP_EM_SER_LEM_NOF_FIDS; ++indx) {
        tbl_data[0] = lem_fid_lif_counters[indx];
        res = soc_mem_write(unit,
                            PPDB_B_LARGE_EM_FID_COUNTER_DBm,
                            MEM_BLOCK_ANY,
                            indx,
                            tbl_data);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}


uint32 jer_pp_em_ser_lem_lif_counters_update(int unit)
{
    uint32 res = SOC_SAND_OK;
    SOC_SAND_TABLE_BLOCK_RANGE                      block_range;
    uint32 nof_entries, indx, lif_indx, lif, range0_min, range0_max, range1_min, range1_max;
    uint32 tbl_data[1];
    SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE          rule;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&block_range);
    SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE_clear(&rule);

    
    range0_min = SOC_PPC_FRWRD_MACT_LEARN_LIF_RANGE_BASE(0);
    range0_max = range0_min + ARAD_PP_FRWRD_MACT_LIMIT_RANGE_MAP_SIZE;
    range1_min = SOC_PPC_FRWRD_MACT_LEARN_LIF_RANGE_BASE(1);
    range1_max = range1_min + ARAD_PP_FRWRD_MACT_LIMIT_RANGE_MAP_SIZE;

    sal_memset(lem_fid_lif_counters, 0, sizeof(uint16)*JER_PP_EM_SER_LEM_NOF_FIDS);

    
    SOC_SAND_TBL_ITER_SET_BEGIN(&(block_range.iter));
    block_range.entries_to_act = JER_PP_EM_SER_LEM_TRVRS_ITER_BLK_SIZE;
    block_range.entries_to_scan = SOC_SAND_TBL_ITER_SCAN_ALL;

    rule.key_type = SOC_PPC_FRWRD_MACT_KEY_TYPE_MAC_ADDR;

    while(!SOC_SAND_TBL_ITER_IS_END(&(block_range.iter))) {
        
        res = soc_ppd_frwrd_mact_get_block(unit,
                                           &rule,
                                           SOC_PPC_FRWRD_MACT_TABLE_SW_HW,
                                           &block_range,
                                           l2_traverse_mact_keys,
                                           l2_traverse_mact_vals,
                                           &nof_entries);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        if(nof_entries == 0) {
            break;
        }
        for (indx = 0; indx < nof_entries; ++indx) {
            lif_indx = JER_PP_EM_SER_LEM_NOF_FIDS;
            if (l2_traverse_mact_vals[indx].frwrd_info.forward_decision.additional_info.outlif.type != SOC_PPC_OUTLIF_ENCODE_TYPE_NONE) {
                
                lif = l2_traverse_mact_vals[indx].frwrd_info.forward_decision.additional_info.outlif.val;

                
                if ((lif >= range0_min) && (lif < range0_max)) {
                    lif_indx = lif;
                }
                else if ((lif >= range1_min) && (lif < range1_max)) {
                    lif_indx = lif + ARAD_PP_FRWRD_MACT_LIMIT_RANGE_MAP_SIZE;
                }
            }
            
            if (lif_indx < JER_PP_EM_SER_LEM_NOF_FIDS) {
                lem_fid_lif_counters[lif_indx]++;
            }
        }
    }

    
    for (indx = 0; indx < JER_PP_EM_SER_LEM_NOF_FIDS; ++indx) {
        tbl_data[0] = lem_fid_lif_counters[indx];
        res = soc_mem_write(unit,
                            PPDB_B_LARGE_EM_FID_COUNTER_DBm,
                            MEM_BLOCK_ANY,
                            indx,
                            tbl_data);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

uint32 jer_pp_em_ser_fix_lem_fid_lif_counters(int unit)
{
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (SOC_PPC_FRWRD_MACT_LEARN_LIMIT_MODE == SOC_PPC_FRWRD_MACT_LEARN_LIMIT_TYPE_VSI) {
        res = jer_pp_em_ser_lem_fid_counters_update(unit);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }
    else if (SOC_PPC_FRWRD_MACT_LEARN_LIMIT_MODE == SOC_PPC_FRWRD_MACT_LEARN_LIMIT_TYPE_LIF) {
        res = jer_pp_em_ser_lem_lif_counters_update(unit);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);

}

uint32 jer_pp_em_ser_lem_fix_counters(int unit)
{
    uint32 res = SOC_SAND_OK;
    int lem_counter_diff = 0;
    uint32 lem_mact_counter;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (fix_global_counters) {
        
        res = jer_pp_em_ser_lem_counter_diff_get(unit, &lem_counter_diff, &lem_mact_counter);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        
        
        res = jer_pp_em_ser_mact_counter_fix(unit, lem_mact_counter);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }

    if (fix_fid_counters) {
        
        res = jer_pp_em_ser_fix_lem_fid_lif_counters(unit);
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    }

    
    res = jer_pp_em_ser_management_enable_set(unit, KEYT_PLDT_MEM(PPDB_B, LARGE_EM), REG_PORT_ANY, 1);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    
    if (lem_counter_diff > 0) {
        res = jer_pp_em_lem_counter_increment(unit, lem_counter_diff);
        SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    }
    else if (lem_counter_diff < 0) {
        res = jer_pp_em_lem_counter_decrement(unit, lem_counter_diff*(-1));
        SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

uint32 jer_pp_em_ser_em_fix_counters(int unit, JER_PP_SER_EM_TYPE_INFO* ser_info)
{
    uint32 res = SOC_SAND_OK;
    int em_counter_diff = 0;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (fix_global_counters) {
        
        res = jer_pp_em_ser_em_counter_diff_get(unit, ser_info, &em_counter_diff);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }

    
    res = jer_pp_em_ser_management_enable_set(unit, ser_info->keyt_pldt_mem, ser_info->block_instance, 1);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    
    if (em_counter_diff > 0) {
        res = jer_pp_em_em_counter_increment(unit, ser_info, em_counter_diff);
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    }
    else if (em_counter_diff < 0) {
        res = jer_pp_em_em_counter_decrement(unit, ser_info, em_counter_diff*(-1));
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

uint32 jer_pp_em_ser_align_shadow_and_hw_lem(int unit)
{
    uint32 res = SOC_SAND_OK;
    uint32 key_out[SOC_DPP_DEFS_MAX_LEM_WIDTH_IN_UINT32S],
           data_out[ARAD_PP_LEM_ACCESS_PAYLOAD_IN_UINT32S];
    uint8  is_valid, is_found;
    uint32 entry_index, indx, nof_entries;
    uint64 fld_val64, reg_val1;
    ARAD_PP_LEM_ACCESS_PAYLOAD payload;
    ARAD_PP_LEM_ACCESS_KEY     key;
    ARAD_PP_LEM_ACCESS_REQUEST request;
    ARAD_PP_LEM_ACCESS_KEY_ENCODED key_in_buffer;
    ARAD_PP_LEM_ACCESS_ACK_STATUS ack;
    SOC_SAND_TABLE_BLOCK_RANGE                      block_range;
    SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE          rule;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (!SOC_DPP_IS_LEM_SIM_ENABLE(unit)) {
        LOG_ERROR(BSL_LS_BCM_INTR,
            (BSL_META_U(unit,
            "%s"),
            "LEM shadow must be enabled for the recovery, please configure property exact_match_tables_shadow_enable."));
        SOC_SAND_EXIT_NO_ERROR;
    }

    ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
    ARAD_PP_LEM_ACCESS_KEY_clear(&key);
    ARAD_PP_LEM_ACCESS_REQUEST_clear(&request);
    ARAD_PP_LEM_ACCESS_ACK_STATUS_clear(&ack);
    ARAD_PP_LEM_ACCESS_KEY_ENCODED_clear(&key_in_buffer);
    soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&block_range);
    SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE_clear(&rule);

    
    for(entry_index = 0; entry_index < JER_CHIP_SIM_LEM_TABLE_SIZE; entry_index++) {
        soc_sand_os_memset(key_out, 0x0, JER_CHIP_SIM_LEM_KEY) ;
        soc_sand_os_memset(data_out, 0x0, JER_CHIP_SIM_LEM_PAYLOAD) ;

        res = soc_sand_exact_match_entry_get_by_index_unsafe(
              unit,
              JER_CHIP_SIM_LEM_BASE,
              entry_index,
              key_out,
              JER_CHIP_SIM_LEM_KEY,
              data_out,
              JER_CHIP_SIM_LEM_PAYLOAD,
              &is_valid
            );
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        if (is_valid) {
            
            is_found = 0;
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_PPDB_B_LARGE_EM_DIAGNOSTICS_KEYr(unit, key_out));

            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, PPDB_B_LARGE_EM_DIAGNOSTICSr, REG_PORT_ANY, 0, LARGE_EM_DIAGNOSTICS_LOOKUPf, 1));

            
                res = arad_polling(
                      unit,
                      ARAD_TIMEOUT,
                      ARAD_MIN_POLLS,
                      PPDB_B_LARGE_EM_DIAGNOSTICSr,
                      REG_PORT_ANY,
                      0,
                      LARGE_EM_DIAGNOSTICS_LOOKUPf,
                      0
                    );
                SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
            
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,READ_PPDB_B_LARGE_EM_DIAGNOSTICS_LOOKUP_RESULTr(unit, &reg_val1));

            ARAD_FLD_FROM_REG64(PPDB_B_LARGE_EM_DIAGNOSTICS_LOOKUP_RESULTr, LARGE_EM_ENTRY_FOUNDf, fld_val64, reg_val1, 50, exit);
            is_found = SOC_SAND_NUM2BOOL(COMPILER_64_LO(fld_val64));

            if ((is_found) == FALSE) {
                
                request.command = ARAD_PP_LEM_ACCESS_CMD_INSERT;
                key_in_buffer.buffer[0] = key_out[0];
                key_in_buffer.buffer[1] = key_out[1];
                key_in_buffer.buffer[2] = key_out[2];

                res = arad_pp_lem_key_encoded_parse(
                  unit,
                  &key_in_buffer,
                  &(request.key)
                  );
                SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

                res = arad_pp_lem_access_payload_parse(unit,
                                                       data_out,
                                                       request.key.type,
                                                       &payload);
                SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

                res = arad_pp_lem_access_entry_add_unsafe(
                        unit,
                        &request,
                        &payload,
                        &ack);
                SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
            }
        }
    }

    

    SOC_SAND_TBL_ITER_SET_BEGIN(&(block_range.iter));
    block_range.entries_to_act = JER_PP_EM_SER_LEM_TRVRS_ITER_BLK_SIZE;
    block_range.entries_to_scan = SOC_SAND_TBL_ITER_SCAN_ALL;

    rule.key_type = SOC_PPC_FRWRD_MACT_KEY_TYPE_MAC_ADDR;

    while(!SOC_SAND_TBL_ITER_IS_END(&(block_range.iter))) {
        
        res = soc_ppd_frwrd_mact_get_block(unit,
                                           &rule,
                                           SOC_PPC_FRWRD_MACT_TABLE_SW_HW,
                                           &block_range,
                                           l2_traverse_mact_keys,
                                           l2_traverse_mact_vals,
                                           &nof_entries);
        SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
        if(nof_entries == 0) {
            break;
        }
        for (indx = 0; indx < nof_entries; ++indx) {

            res = arad_pp_frwrd_mact_key_convert(
                  unit,
                  &l2_traverse_mact_keys[indx],
                  &key
                );
            SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

            res = arad_pp_lem_key_encoded_build(
                   unit,
                   &(key),
                   0,
                   &key_in_buffer
            );

            
            is_found = 0;
            res = chip_sim_exact_match_entry_get_unsafe(
                  unit,
                  JER_CHIP_SIM_LEM_BASE,
                  key_in_buffer.buffer,
                  JER_CHIP_SIM_LEM_KEY,
                  data_out,
                  JER_CHIP_SIM_LEM_PAYLOAD,
                  &is_found
                );
            SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

            if ((is_found) == FALSE) {
                

                request.command = ARAD_PP_LEM_ACCESS_CMD_DELETE;

                res = arad_pp_lem_key_encoded_parse(
                  unit,
                  &key_in_buffer,
                  &(request.key)
                  );
                SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);

                res = arad_pp_lem_access_entry_remove_unsafe(
                      unit,
                      &request,
                      &ack);
                SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);
            }
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

uint32 jer_pp_em_ser_align_shadow_and_hw_em(int unit, int block_instance, JER_PP_SER_EM_TYPE_INFO* ser_info)
{
    uint32 res = SOC_SAND_OK;
    uint8  is_valid, is_found;
    uint32 entry_index, temp, entry_is_valid;
    soc_reg_above_64_val_t reg_val, fld_val, read_result, key_data, key_out, data_out, buffer;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (!SOC_DPP_IS_EM_SIM_ENABLE(unit)) {
        LOG_ERROR(BSL_LS_BCM_INTR,
            (BSL_META_U(unit,
            "%s"),
            "EM shadow must be enabled for the recovery, please configure property exact_match_tables_shadow_enable."));
        SOC_SAND_EXIT_NO_ERROR;
    }

    SOC_REG_ABOVE_64_CLEAR(buffer);
    SOC_REG_ABOVE_64_CLEAR(reg_val);
    SOC_REG_ABOVE_64_CLEAR(fld_val);
    SOC_REG_ABOVE_64_CLEAR(read_result);
    SOC_REG_ABOVE_64_CLEAR(key_data);
    SOC_REG_ABOVE_64_CLEAR(key_out);
    SOC_REG_ABOVE_64_CLEAR(data_out);
    
    for(entry_index = 0; entry_index < ser_info->db_nof_lines; entry_index++) {

        res = soc_sand_exact_match_entry_get_by_index_unsafe(
              unit,
              ser_info->shadow_base,
              entry_index,
              key_out,
              ser_info->shadow_key_size,
              data_out,
              ser_info->shadow_payload_size,
              &is_valid
            );
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        if (is_valid) {
            
            is_found = 0;
            res = soc_reg_above_64_set(unit, ser_info->diagnostics_key_reg, block_instance, 0, key_out);
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ser_info->diagnostics_reg, REG_PORT_ANY, 0, ser_info->diagnostics_lookup_field, 1));

            
            res = arad_polling(
                      unit,
                      ARAD_TIMEOUT,
                      ARAD_MIN_POLLS,
                      ser_info->diagnostics_reg,
                      block_instance,
                      0,
                      ser_info->diagnostics_lookup_field,
                      0
                    );
            SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

            
            res= soc_reg_above_64_get(unit, ser_info->diagnostics_lookup_result_reg, block_instance, 0, reg_val);
            SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

            
            is_found = reg_val[0] & 1;

            if ((is_found) == FALSE) {
                
                
                temp = JER_PP_EM_SER_REQUEST_TYPE_INSERT_VAL;
                res = soc_sand_bitstream_set_any_field(
                      &temp,
                      JER_PP_EM_SER_REQUEST_TYPE_START_BIT,
                      JER_PP_EM_SER_REQUEST_TYPE_SIZE,
                      buffer
                    );
                SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

                
                res = soc_sand_bitstream_set_any_field(
                      key_out,
                      ser_info->key_start_bit,
                      ser_info->key_size,
                      buffer
                    );
                SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

                
                res = soc_sand_bitstream_set_any_field(
                      data_out,
                      ser_info->payload_start_bit,
                      ser_info->payload_size,
                      buffer
                    );
                SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

                
                res = soc_mem_write(unit, ser_info->req_mem, SOC_MEM_BLOCK_MIN(unit, ser_info->keyt_pldt_mem) + block_instance, 0, buffer);
                SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
            }
        }
    }

    

    for (entry_index = 0; entry_index < ser_info->db_nof_lines; entry_index++) {
        
        res = soc_reg32_set(unit, ser_info->diagnostics_index_reg, block_instance, 0, entry_index);
        SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  110,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ser_info->diagnostics_reg, REG_PORT_ANY, 0, ser_info->diagnostics_read_field, 1));

        
        res = arad_polling(
              unit,
              ARAD_TIMEOUT,
              ARAD_MIN_POLLS,
              ser_info->diagnostics_reg,
              block_instance,
              0,
              ser_info->diagnostics_read_field,
              0
            );
        SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
        res = soc_reg_above_64_get(unit, ser_info->diagnostics_read_result_reg, block_instance, 0, read_result);
        SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);

        
        res = soc_sand_bitstream_get_any_field(
              read_result,
              0, 
              1, 
              &entry_is_valid
            );
        SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);

        if (entry_is_valid) {
            
            res = soc_sand_bitstream_get_any_field(
                  read_result,
                  1, 
                  ser_info->key_size, 
                  key_data
                );
            SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

            
            is_found = 0;
            res = chip_sim_exact_match_entry_get_unsafe(
                  unit,
                  ser_info->shadow_base,
                  key_data,
                  ser_info->shadow_key_size,
                  data_out,
                  ser_info->shadow_payload_size,
                  &is_found
                );
            SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);

            if ((is_found) == FALSE) {
                

                
                temp = JER_PP_EM_SER_REQUEST_TYPE_DELETE_VAL;
                res = soc_sand_bitstream_set_any_field(
                      &temp,
                      JER_PP_EM_SER_REQUEST_TYPE_START_BIT,
                      JER_PP_EM_SER_REQUEST_TYPE_SIZE,
                      buffer
                    );
                SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);

                
                res = soc_sand_bitstream_set_any_field(
                      key_data,
                      ser_info->key_start_bit,
                      ser_info->key_size,
                      buffer
                    );
                SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);

                
                res = soc_mem_write(unit, ser_info->req_mem, SOC_MEM_BLOCK_MIN(unit, ser_info->keyt_pldt_mem) + block_instance, 0, buffer);
                SOC_SAND_CHECK_FUNC_RESULT(res, 190, exit);

            }
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void jer_pp_em_ser_lem_recover(void *p1, void *p2, void *p3, void *p4, void *p5)
{
    uint32 key_pld_data[4] = {0};
    soc_reg_above_64_val_t reg_above_64_val;
    int unit = *(int*)p1;
    uint32 array_index = *(uint32*)p3;
    uint32 entry_offset = *(uint32*)p4;
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);

    
    res = soc_mem_array_write(unit,
                              KEYT_PLDT_MEM(PPDB_B, LARGE_EM),
                              array_index,
                              MEM_BLOCK_ANY,
                              entry_offset,
                              key_pld_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    
    res = jer_pp_em_ser_lem_fix_counters(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    
    res = jer_pp_em_ser_align_shadow_and_hw_lem(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
    
    jer_pp_em_ser_mask_interrupt_set(unit, KEYT_PLDT_MEM(PPDB_B, LARGE_EM), 1);
}

void jer_pp_em_ser_em_recover(void *p1, void *p2, void *p3, void *p4, void *p5)
{
    int unit = *(int*)p1;
    soc_mem_t mem = *(soc_mem_t*)p2;
    uint32 array_index = *(uint32*)p3;
    uint32 entry_offset = *(uint32*)p4;
    int block_instance = *(int*)p5;
    soc_reg_above_64_val_t key_pld_data;
    JER_PP_SER_EM_TYPE_INFO ser_info;
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);


    SOC_REG_ABOVE_64_CLEAR(key_pld_data);
    sal_memset(&ser_info, 0, sizeof(JER_PP_SER_EM_TYPE_INFO));

    res = jer_pp_ser_em_type_info_get(unit, block_instance, mem, &ser_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);


    
    res = soc_mem_array_write(unit,
                              ser_info.keyt_pldt_mem,
                              array_index,
                              block_instance + SOC_MEM_BLOCK_MIN(unit, ser_info.keyt_pldt_mem),
                              entry_offset,
                              key_pld_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    
    res = jer_pp_em_ser_em_fix_counters(unit, &ser_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    
    res = jer_pp_em_ser_align_shadow_and_hw_em(unit, block_instance, &ser_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
exit:
    
    jer_pp_em_ser_mask_interrupt_set(unit, mem, 1);
}

int jer_pp_em_ser_lem(int unit, soc_mem_t mem, uint32 array_index, uint32 entry_offset)
{
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    res = jer_pp_em_ser_management_enable_set(unit, mem, REG_PORT_ANY, 0);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    
    p1 = unit;
    p2 = 0;
    p3 = array_index;
    p4 = entry_offset;

    
    jer_pp_em_ser_mask_interrupt_set(unit, mem, 0);

    
    res = sal_dpc(jer_pp_em_ser_lem_recover, (void*)&p1, (void*)&p2, (void*)&p3, (void*)&p4, NULL);
    if (res)
    {
        LOG_ERROR(BSL_LS_BCM_INTR,
                (BSL_META_U(unit,
                    "sal_dpc failed to queue jer_pp_em_ser_lem_recover !!\n")));
        
        jer_pp_em_ser_mask_interrupt_set(unit, mem, 1);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}


int jer_pp_em_ser_em(int unit, int block_instance, soc_mem_t mem, uint32 array_index, uint32 entry_offset)
{
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    res = jer_pp_em_ser_management_enable_set(unit, mem, block_instance, 0);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    
    p1 = unit;
    p2 = mem;
    p3 = array_index;
    p4 = entry_offset;
    p5 = block_instance;

    
    jer_pp_em_ser_mask_interrupt_set(unit, mem, 0);

    
    res = sal_dpc(jer_pp_em_ser_em_recover, (void*)&p1, (void*)&p2, (void*)&p3, (void*)&p4, (void*)&p5);
    if (res) {
        LOG_ERROR(BSL_LS_BCM_INTR,
                (BSL_META_U(unit,
                    "sal_dpc failed to queue jer_pp_em_ser_em_recover !!\n")));
        
        jer_pp_em_ser_mask_interrupt_set(unit, mem, 1);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in jer_pp_em_ser_em()", 0, 0);
}



int dcmn_pp_em_ser(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    dcmn_interrupt_mem_err_info* shadow_correct_info_p,
    char* msg)
{
    uint32 res = SOC_SAND_OK;
    soc_mem_t mem;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);


    mem = shadow_correct_info_p->mem;

    if (SOC_MEM_TYPE(unit,mem) == SOC_MEM_TYPE_XOR) {
        
        res = jer_pp_em_ser_management_enable_set(unit, mem, block_instance, 0);
        SOC_SAND_CHECK_FUNC_RESULT(res, 0, exit);

        
        res = dcmn_interrupt_handles_corrective_action_for_xor(unit, block_instance, interrupt_id, shadow_correct_info_p, msg, 1, 0);
        SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

        
        res = jer_pp_em_ser_management_enable_set(unit, mem, block_instance, 1);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }


    if (mem == KEYT_PLDT_MEM(PPDB_B, LARGE_EM)) {
        
        res = jer_pp_em_ser_lem(unit, mem, shadow_correct_info_p->array_index, shadow_correct_info_p->min_index);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }
    else {
        
        res = jer_pp_em_ser_em(unit, block_instance, mem, shadow_correct_info_p->array_index, shadow_correct_info_p->min_index);
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

int
dcmn_interrupt_handles_corrective_action_for_em_ecc_1b(
    int unit,
    int block_instance,
    uint32 interrupt_id,
    dcmn_interrupt_mem_err_info* ecc_1b_correct_info_p,
    char* msg)
{
    int rc;
    soc_mem_t mem;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(ecc_1b_correct_info_p);

    mem = ecc_1b_correct_info_p->mem;

    
    rc = jer_pp_em_ser_management_enable_set(unit, mem, block_instance, 0);
    SOCDNX_IF_ERR_EXIT(rc);

    if (SOC_MEM_TYPE(unit,mem) == SOC_MEM_TYPE_XOR) {
        rc = dcmn_interrupt_handles_corrective_action_for_xor(unit, block_instance, interrupt_id, ecc_1b_correct_info_p, msg, 0, 1);
        SOCDNX_IF_ERR_EXIT(rc);

    } else {
        rc = dcmn_interrupt_handles_corrective_action_for_ecc_1b(unit, block_instance, interrupt_id, ecc_1b_correct_info_p, msg);
        SOCDNX_IF_ERR_EXIT(rc);
    }

    
    rc = jer_pp_em_ser_management_enable_set(unit, mem, block_instance, 1);
    SOCDNX_IF_ERR_EXIT(rc);


exit:
    SOCDNX_FUNC_RETURN;
}
#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif

