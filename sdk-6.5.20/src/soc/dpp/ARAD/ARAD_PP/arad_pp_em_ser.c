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
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_em_ser.h>



#define ARAD_PP_EM_SER_LEM_KEYT_BASE_ADDR       (0x500000)
#define ARAD_PP_EM_SER_LEM_PLDT_BASE_ADDR       (0x590000)
#define ARAD_PP_EM_SER_LEM_NOF_LINES            (256*1024)

#define ARAD_PP_EM_SER_ISA_KEYT_BASE_ADDR       (0x7A0000)
#define ARAD_PP_EM_SER_ISA_PLDT_BASE_ADDR       (0x830000)
#define ARAD_PP_EM_SER_ISA_NOF_LINES            (32*1024)

#define ARAD_PP_EM_SER_ISB_KEYT_BASE_ADDR       (0xA10000)
#define ARAD_PP_EM_SER_ISB_PLDT_BASE_ADDR       (0xAA0000)
#define ARAD_PP_EM_SER_ISB_NOF_LINES            (32*1024)

#define ARAD_PP_EM_SER_ESEM_KEYT_BASE_ADDR      (0x520000)
#define ARAD_PP_EM_SER_ESEM_PLDT_BASE_ADDR      (0x5B0000)
#define ARAD_PP_EM_SER_ESEM_NOF_LINES           (16*1024)

#define ARAD_PP_EM_SER_OEMA_KEYT_BASE_ADDR      (0xEA0000)
#define ARAD_PP_EM_SER_OEMA_PLDT_BASE_ADDR      (0xF30000)
#define ARAD_PP_EM_SER_OEMA_NOF_LINES           (16*1024)

#define ARAD_PP_EM_SER_OEMB_KEYT_BASE_ADDR      (0x14A0000)
#define ARAD_PP_EM_SER_OEMB_PLDT_BASE_ADDR      (0x1530000)
#define ARAD_PP_EM_SER_OEMB_NOF_LINES           (8*1024)

#define ARAD_PP_EM_SER_RMAPEM_KEYT_BASE_ADDR    (0x100000)
#define ARAD_PP_EM_SER_RMAPEM_PLDT_BASE_ADDR    (0x190000)
#define ARAD_PP_EM_SER_RMAPEM_NOF_LINES         (16*1024)

#define ARAD_PP_EM_SER_NOF_HASH_RESULTS             8
#define ARAD_PP_EM_SER_EM_AUX_CAM_SIZE             32

#define ARAD_PP_EM_SER_LEM_VERIFIER_LEN             59
#define ARAD_PP_EM_SER_LEM_VERIFIER_LEN_IN_UINT32   (ARAD_PP_EM_SER_LEM_VERIFIER_LEN / SOC_SAND_NOF_BITS_IN_UINT32 + 1)

#define ARAD_PP_EM_SER_LEM_NOF_FIDS                 (32 * 1024)

#define LEM_ERR_MEM_MASK_REGISTER       IHP_PAR_0_ERR_MEM_MASKr
#define ISA_ERR_MEM_MASK_REGISTER       IHP_PAR_1_ERR_MEM_MASKr
#define ISB_ERR_MEM_MASK_REGISTER       IHP_PAR_3_ERR_MEM_MASKr
#define ESEM_ERR_MEM_MASK_REGISTER      EPNI_PAR_ERR_MEM_MASKr
#define OEMA_ERR_MEM_MASK_REGISTER      IHB_PAR_ERR_MEM_MASKr
#define OEMB_ERR_MEM_MASK_REGISTER      IHB_PAR_ERR_MEM_MASKr
#define RMAPEM_ERR_MEM_MASK_REGISTER    OAMP_PAR_0_ERR_MONITOR_MEM_MASKr

#define ARAD_PP_EM_SER_ISA_REQUEST_KEY_START_BIT        11
#define ARAD_PP_EM_SER_ISB_REQUEST_KEY_START_BIT        11
#define ARAD_PP_EM_SER_ESEM_REQUEST_KEY_START_BIT       11
#define ARAD_PP_EM_SER_OEMA_REQUEST_KEY_START_BIT       11
#define ARAD_PP_EM_SER_OEMB_REQUEST_KEY_START_BIT       11
#define ARAD_PP_EM_SER_RMAPEM_REQUEST_KEY_START_BIT     11

#define ARAD_PP_EM_SER_ISA_REQUEST_PAYLOAD_START_BIT        52
#define ARAD_PP_EM_SER_ISB_REQUEST_PAYLOAD_START_BIT        52
#define ARAD_PP_EM_SER_ESEM_REQUEST_PAYLOAD_START_BIT       48
#define ARAD_PP_EM_SER_OEMA_REQUEST_PAYLOAD_START_BIT       28
#define ARAD_PP_EM_SER_OEMB_REQUEST_PAYLOAD_START_BIT       32
#define ARAD_PP_EM_SER_RMAPEM_REQUEST_PAYLOAD_START_BIT     40

#define ARAD_PP_EM_SER_ISA_REQUEST_KEY_SIZE        41
#define ARAD_PP_EM_SER_ISB_REQUEST_KEY_SIZE        41
#define ARAD_PP_EM_SER_ESEM_REQUEST_KEY_SIZE       37
#define ARAD_PP_EM_SER_OEMA_REQUEST_KEY_SIZE       17
#define ARAD_PP_EM_SER_OEMB_REQUEST_KEY_SIZE       21
#define ARAD_PP_EM_SER_RMAPEM_REQUEST_KEY_SIZE     29

#define ARAD_PP_EM_SER_ISA_REQUEST_PAYLOAD_SIZE        16
#define ARAD_PP_EM_SER_ISB_REQUEST_PAYLOAD_SIZE        16
#define ARAD_PP_EM_SER_ESEM_REQUEST_PAYLOAD_SIZE       16
#define ARAD_PP_EM_SER_OEMA_REQUEST_PAYLOAD_SIZE       31
#define ARAD_PP_EM_SER_OEMB_REQUEST_PAYLOAD_SIZE       17
#define ARAD_PP_EM_SER_RMAPEM_REQUEST_PAYLOAD_SIZE     14

#define ARAD_PP_EM_SER_ISA_READ_RESULT_LEN        42
#define ARAD_PP_EM_SER_ISB_READ_RESULT_LEN        42
#define ARAD_PP_EM_SER_ESEM_READ_RESULT_LEN       38
#define ARAD_PP_EM_SER_OEMA_READ_RESULT_LEN       18
#define ARAD_PP_EM_SER_OEMB_READ_RESULT_LEN       22
#define ARAD_PP_EM_SER_RMAPEM_READ_RESULT_LEN     30

#define ARAD_PP_EM_SER_ISA_LOOKUP_RESULT_LEN        22
#define ARAD_PP_EM_SER_ISB_LOOKUP_RESULT_LEN        22
#define ARAD_PP_EM_SER_ESEM_LOOKUP_RESULT_LEN       22
#define ARAD_PP_EM_SER_OEMA_LOOKUP_RESULT_LEN       37
#define ARAD_PP_EM_SER_OEMB_LOOKUP_RESULT_LEN       23
#define ARAD_PP_EM_SER_RMAPEM_LOOKUP_RESULT_LEN     20

#define ARAD_PP_EM_SER_REQUEST_TYPE_START_BIT  0
#define ARAD_PP_EM_SER_REQUEST_TYPE_SIZE       3
#define ARAD_PP_EM_SER_REQUEST_TYPE_INSERT_VAL 1
#define ARAD_PP_EM_SER_REQUEST_TYPE_DELETE_VAL 0





#define ARAD_PP_EM_SER_LEM_TRVRS_ITER_BLK_SIZE      (130)



#define MANAGEMENT_UNIT_CONFIGURATION_REGISTER(block,db)    block##_##db##_MANAGEMENT_UNIT_CONFIGURATION_REGISTERr
#define MNGMNT_UNIT_ENABLE_FIELD(db)                        db##_MNGMNT_UNIT_ENABLEf

#define PARITY_ERR_CNT_REGISTER(block)                  block##_PARITY_ERR_CNTr
#define ENTRIES_COUNTER_REGISTER(block, db)             block##_##db##_ENTRIES_COUNTERr
#define DIAGNOSTICS_REGISTER(block, db)                 block##_##db##_DIAGNOSTICSr
#define DIAGNOSTICS_INDEX_REGISTER(block, db)           block##_##db##_DIAGNOSTICS_INDEXr
#define DIAGNOSTICS_KEY_REGISTER(block, db)             block##_##db##_DIAGNOSTICS_KEYr
#define DIAGNOSTICS_READ_RESULT_REGISTER(block, db)     block##_##db##_DIAGNOSTICS_READ_RESULTr
#define DIAGNOSTICS_LOOKUP_RESULT_REGISTER(block, db)   block##_##db##_DIAGNOSTICS_LOOKUP_RESULTr
#define DIAGNOSTICS_READ_FIELD(db)                      db##_DIAGNOSTICS_READf
#define DIAGNOSTICS_LOOKUP_FIELD(db)                    db##_DIAGNOSTICS_LOOKUPf

#define MASK_SER_INTERRUPTS_32B_SET(db, mask_register, reg, val) \
    soc_reg_field_set(unit, mask_register, reg, db##_KEYT_H_0_PARITY_ERR_MASKf, val);   \
    soc_reg_field_set(unit, mask_register, reg, db##_KEYT_H_1_PARITY_ERR_MASKf, val);   \
    soc_reg_field_set(unit, mask_register, reg, db##_KEYT_H_2_PARITY_ERR_MASKf, val);   \
    soc_reg_field_set(unit, mask_register, reg, db##_KEYT_H_3_PARITY_ERR_MASKf, val);   \
    soc_reg_field_set(unit, mask_register, reg, db##_KEYT_H_4_PARITY_ERR_MASKf, val);   \
    soc_reg_field_set(unit, mask_register, reg, db##_KEYT_H_5_PARITY_ERR_MASKf, val);   \
    soc_reg_field_set(unit, mask_register, reg, db##_KEYT_H_6_PARITY_ERR_MASKf, val);   \
    soc_reg_field_set(unit, mask_register, reg, db##_KEYT_H_7_PARITY_ERR_MASKf, val);   \
    soc_reg_field_set(unit, mask_register, reg, db##_PLDT_H_0_PARITY_ERR_MASKf, val);   \
    soc_reg_field_set(unit, mask_register, reg, db##_PLDT_H_1_PARITY_ERR_MASKf, val);   \
    soc_reg_field_set(unit, mask_register, reg, db##_PLDT_H_2_PARITY_ERR_MASKf, val);   \
    soc_reg_field_set(unit, mask_register, reg, db##_PLDT_H_3_PARITY_ERR_MASKf, val);   \
    soc_reg_field_set(unit, mask_register, reg, db##_PLDT_H_4_PARITY_ERR_MASKf, val);   \
    soc_reg_field_set(unit, mask_register, reg, db##_PLDT_H_5_PARITY_ERR_MASKf, val);   \
    soc_reg_field_set(unit, mask_register, reg, db##_PLDT_H_6_PARITY_ERR_MASKf, val);   \
    soc_reg_field_set(unit, mask_register, reg, db##_PLDT_H_7_PARITY_ERR_MASKf, val) 

#define MASK_SER_INTERRUPTS_64B_SET(db, mask_register, reg, val) \
    soc_reg64_field_set(unit, mask_register, reg, db##_KEYT_H_0_PARITY_ERR_MASKf, val);   \
    soc_reg64_field_set(unit, mask_register, reg, db##_KEYT_H_1_PARITY_ERR_MASKf, val);   \
    soc_reg64_field_set(unit, mask_register, reg, db##_KEYT_H_2_PARITY_ERR_MASKf, val);   \
    soc_reg64_field_set(unit, mask_register, reg, db##_KEYT_H_3_PARITY_ERR_MASKf, val);   \
    soc_reg64_field_set(unit, mask_register, reg, db##_KEYT_H_4_PARITY_ERR_MASKf, val);   \
    soc_reg64_field_set(unit, mask_register, reg, db##_KEYT_H_5_PARITY_ERR_MASKf, val);   \
    soc_reg64_field_set(unit, mask_register, reg, db##_KEYT_H_6_PARITY_ERR_MASKf, val);   \
    soc_reg64_field_set(unit, mask_register, reg, db##_KEYT_H_7_PARITY_ERR_MASKf, val);   \
    soc_reg64_field_set(unit, mask_register, reg, db##_PLDT_H_0_PARITY_ERR_MASKf, val);   \
    soc_reg64_field_set(unit, mask_register, reg, db##_PLDT_H_1_PARITY_ERR_MASKf, val);   \
    soc_reg64_field_set(unit, mask_register, reg, db##_PLDT_H_2_PARITY_ERR_MASKf, val);   \
    soc_reg64_field_set(unit, mask_register, reg, db##_PLDT_H_3_PARITY_ERR_MASKf, val);   \
    soc_reg64_field_set(unit, mask_register, reg, db##_PLDT_H_4_PARITY_ERR_MASKf, val);   \
    soc_reg64_field_set(unit, mask_register, reg, db##_PLDT_H_5_PARITY_ERR_MASKf, val);   \
    soc_reg64_field_set(unit, mask_register, reg, db##_PLDT_H_6_PARITY_ERR_MASKf, val);   \
    soc_reg64_field_set(unit, mask_register, reg, db##_PLDT_H_7_PARITY_ERR_MASKf, val) 

#define MASK_SER_INTERRUPTS_ABOVE_64B_SET(db, mask_register, reg, val) \
    soc_reg_above_64_field_set(unit, mask_register, reg, db##_KEYT_H_0_PARITY_ERR_MASKf, val);   \
    soc_reg_above_64_field_set(unit, mask_register, reg, db##_KEYT_H_1_PARITY_ERR_MASKf, val);   \
    soc_reg_above_64_field_set(unit, mask_register, reg, db##_KEYT_H_2_PARITY_ERR_MASKf, val);   \
    soc_reg_above_64_field_set(unit, mask_register, reg, db##_KEYT_H_3_PARITY_ERR_MASKf, val);   \
    soc_reg_above_64_field_set(unit, mask_register, reg, db##_KEYT_H_4_PARITY_ERR_MASKf, val);   \
    soc_reg_above_64_field_set(unit, mask_register, reg, db##_KEYT_H_5_PARITY_ERR_MASKf, val);   \
    soc_reg_above_64_field_set(unit, mask_register, reg, db##_KEYT_H_6_PARITY_ERR_MASKf, val);   \
    soc_reg_above_64_field_set(unit, mask_register, reg, db##_KEYT_H_7_PARITY_ERR_MASKf, val);   \
    soc_reg_above_64_field_set(unit, mask_register, reg, db##_PLDT_H_0_PARITY_ERR_MASKf, val);   \
    soc_reg_above_64_field_set(unit, mask_register, reg, db##_PLDT_H_1_PARITY_ERR_MASKf, val);   \
    soc_reg_above_64_field_set(unit, mask_register, reg, db##_PLDT_H_2_PARITY_ERR_MASKf, val);   \
    soc_reg_above_64_field_set(unit, mask_register, reg, db##_PLDT_H_3_PARITY_ERR_MASKf, val);   \
    soc_reg_above_64_field_set(unit, mask_register, reg, db##_PLDT_H_4_PARITY_ERR_MASKf, val);   \
    soc_reg_above_64_field_set(unit, mask_register, reg, db##_PLDT_H_5_PARITY_ERR_MASKf, val);   \
    soc_reg_above_64_field_set(unit, mask_register, reg, db##_PLDT_H_6_PARITY_ERR_MASKf, val);   \
    soc_reg_above_64_field_set(unit, mask_register, reg, db##_PLDT_H_7_PARITY_ERR_MASKf, val) 
    



typedef enum
{
    ARAD_PP_EM_SER_DB_TYPE_LEM,
    ARAD_PP_EM_SER_DB_TYPE_ISA,
    ARAD_PP_EM_SER_DB_TYPE_ISB,
    ARAD_PP_EM_SER_DB_TYPE_ESEM,
    ARAD_PP_EM_SER_DB_TYPE_OEMA,
    ARAD_PP_EM_SER_DB_TYPE_OEMB,
    ARAD_PP_EM_SER_DB_TYPE_RMAPEM,
    ARAD_PP_EM_SER_DB_TYPE_NOF_TYPES
}ARAD_PP_EM_SER_DB_TYPE;

typedef struct
{
    ARAD_PP_EM_SER_DB_TYPE db_type;

    
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

    soc_mem_t keyt_mem;
    soc_mem_t pldt_mem;
    soc_mem_t aux_cam_mem;
    soc_reg_t counter_reg;
    uint32 db_nof_lines;

}ARAD_PP_SER_EM_TYPE_INFO;



int fix_global_counters=1;
int fix_fid_counters=1;

uint32 p1, p2, p3, p4, p5;
SOC_PPC_FRWRD_MACT_ENTRY_KEY    l2_traverse_mact_keys[ARAD_PP_EM_SER_LEM_TRVRS_ITER_BLK_SIZE];
SOC_PPC_FRWRD_MACT_ENTRY_VALUE  l2_traverse_mact_vals[ARAD_PP_EM_SER_LEM_TRVRS_ITER_BLK_SIZE];
uint16 lem_fid_lif_counters[ARAD_PP_EM_SER_LEM_NOF_FIDS]={0};


uint32 arad_entry_index[ARAD_PP_EM_SER_DB_TYPE_NOF_TYPES][ARAD_PP_EM_SER_NOF_HASH_RESULTS] = 
            {{16375, 5903, 22433, 27896, 9645, 5150, 26058, 16115}, 
             {1957, 1828, 50, 887, 3229, 70, 2892, 603},            
             {1957, 1828, 50, 887, 3229, 70, 2892, 603},            
             {1451, 487, 1382, 1807, 1605, 179, 1995, 1228},        
             {1852, 875, 646, 39, 334, 6, 1808, 1753},              
             {902, 35, 610, 708, 197, 518, 722, 315},               
             {124, 652, 406, 97, 1140, 1467, 587, 1481}             
            };
uint32 arad_plus_entry_index[ARAD_PP_EM_SER_DB_TYPE_NOF_TYPES][ARAD_PP_EM_SER_NOF_HASH_RESULTS] = 
            {{23527, 2702, 17517, 20818, 25905, 29029, 4039, 2007}, 
             {2702, 3139, 2818, 4028, 3663, 37, 3232, 1956},        
             {2702, 3139, 2818, 4028, 3663, 37, 3232, 1956},        
             {586, 1978, 1381, 1105, 1808, 35, 853, 1135},          
             {895, 1553, 1832, 1577, 1944, 1884, 14, 275},          
             {376, 16, 260, 91, 672, 450, 495, 572},                
             {565, 729, 1369, 1405, 1281, 185, 682, 350}            
            };



uint32 arad_pp_ser_em_type_info_get(int unit, ARAD_PP_EM_SER_TYPE em_ser_type, ARAD_PP_SER_EM_TYPE_INFO *ser_info)
{

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    switch (em_ser_type) {
        case ARAD_PP_EM_SER_TYPE_ISA_KEYT:
        case ARAD_PP_EM_SER_TYPE_ISA_PLDT:
            ser_info->db_type = ARAD_PP_EM_SER_DB_TYPE_ISA;
            ser_info->req_mem = IHP_ISA_MANAGEMENT_REQUESTm;
            ser_info->key_start_bit = ARAD_PP_EM_SER_ISA_REQUEST_KEY_START_BIT;
            ser_info->key_size = ARAD_PP_EM_SER_ISA_REQUEST_KEY_SIZE;
            ser_info->payload_start_bit = ARAD_PP_EM_SER_ISA_REQUEST_PAYLOAD_START_BIT;
            ser_info->payload_size = ARAD_PP_EM_SER_ISA_REQUEST_PAYLOAD_SIZE;
            ser_info->shadow_key_size = ARAD_CHIP_SIM_ISEM_A_KEY;
            ser_info->shadow_payload_size = ARAD_CHIP_SIM_ISEM_A_PAYLOAD;
            ser_info->shadow_base = ARAD_CHIP_SIM_ISEM_A_BASE;
            ser_info->db_nof_lines = ARAD_PP_EM_SER_ISA_NOF_LINES;
            ser_info->diagnostics_reg = DIAGNOSTICS_REGISTER(IHP, ISA);
            ser_info->diagnostics_key_reg = DIAGNOSTICS_KEY_REGISTER(IHP, ISA);
            ser_info->diagnostics_lookup_result_reg = DIAGNOSTICS_LOOKUP_RESULT_REGISTER(IHP, ISA);
            ser_info->diagnostics_lookup_field = DIAGNOSTICS_LOOKUP_FIELD(ISA);
            ser_info->diagnostics_read_result_reg = DIAGNOSTICS_READ_RESULT_REGISTER(IHP, ISA);
            ser_info->diagnostics_read_field = DIAGNOSTICS_READ_FIELD(ISA);
            ser_info->diagnostics_index_reg = DIAGNOSTICS_INDEX_REGISTER(IHP, ISA);
            ser_info->keyt_mem = IHP_MEM_7A0000m;
            ser_info->pldt_mem = IHP_MEM_830000m;
            ser_info->aux_cam_mem = IHP_MEM_820000m;
            ser_info->counter_reg = ENTRIES_COUNTER_REGISTER(IHP, ISA);
            break;

        case ARAD_PP_EM_SER_TYPE_ISB_KEYT:
        case ARAD_PP_EM_SER_TYPE_ISB_PLDT:
            ser_info->db_type = ARAD_PP_EM_SER_DB_TYPE_ISB;
            ser_info->req_mem = IHP_ISB_MANAGEMENT_REQUESTm;
            ser_info->key_start_bit = ARAD_PP_EM_SER_ISB_REQUEST_KEY_START_BIT;
            ser_info->key_size = ARAD_PP_EM_SER_ISB_REQUEST_KEY_SIZE;
            ser_info->payload_start_bit = ARAD_PP_EM_SER_ISB_REQUEST_PAYLOAD_START_BIT;
            ser_info->payload_size = ARAD_PP_EM_SER_ISB_REQUEST_PAYLOAD_SIZE;
            ser_info->shadow_key_size = ARAD_CHIP_SIM_ISEM_B_KEY;
            ser_info->shadow_payload_size = ARAD_CHIP_SIM_ISEM_B_PAYLOAD;
            ser_info->shadow_base = ARAD_CHIP_SIM_ISEM_B_BASE;
            ser_info->db_nof_lines = ARAD_PP_EM_SER_ISB_NOF_LINES;
            ser_info->diagnostics_reg = DIAGNOSTICS_REGISTER(IHP, ISB);
            ser_info->diagnostics_key_reg = DIAGNOSTICS_KEY_REGISTER(IHP, ISB);
            ser_info->diagnostics_lookup_result_reg = DIAGNOSTICS_LOOKUP_RESULT_REGISTER(IHP, ISB);
            ser_info->diagnostics_lookup_field = DIAGNOSTICS_LOOKUP_FIELD(ISB);
            ser_info->diagnostics_read_result_reg = DIAGNOSTICS_READ_RESULT_REGISTER(IHP, ISB);
            ser_info->diagnostics_read_field = DIAGNOSTICS_READ_FIELD(ISB);
            ser_info->diagnostics_index_reg = DIAGNOSTICS_INDEX_REGISTER(IHP, ISB);
            ser_info->keyt_mem = IHP_MEM_A10000m;
            ser_info->pldt_mem = IHP_MEM_AA0000m;
            ser_info->aux_cam_mem = IHP_MEM_A90000m;
            ser_info->counter_reg = ENTRIES_COUNTER_REGISTER(IHP, ISB);
            break;

        case ARAD_PP_EM_SER_TYPE_ESEM_KEYT:
        case ARAD_PP_EM_SER_TYPE_ESEM_PLDT:
            ser_info->db_type = ARAD_PP_EM_SER_DB_TYPE_ESEM;
            ser_info->req_mem = EPNI_ESEM_MANAGEMENT_REQUESTm;
            ser_info->key_start_bit = ARAD_PP_EM_SER_ESEM_REQUEST_KEY_START_BIT;
            ser_info->key_size = ARAD_PP_EM_SER_ESEM_REQUEST_KEY_SIZE;
            ser_info->payload_start_bit = ARAD_PP_EM_SER_ESEM_REQUEST_PAYLOAD_START_BIT;
            ser_info->payload_size = ARAD_PP_EM_SER_ESEM_REQUEST_PAYLOAD_SIZE;
            ser_info->shadow_key_size = ARAD_CHIP_SIM_ESEM_KEY;
            ser_info->shadow_payload_size = ARAD_CHIP_SIM_ESEM_PAYLOAD;
            ser_info->shadow_base = ARAD_CHIP_SIM_ESEM_BASE;
            ser_info->db_nof_lines = ARAD_PP_EM_SER_ESEM_NOF_LINES;
            ser_info->diagnostics_reg = DIAGNOSTICS_REGISTER(EPNI, ESEM);
            ser_info->diagnostics_key_reg = DIAGNOSTICS_KEY_REGISTER(EPNI, ESEM);
            ser_info->diagnostics_lookup_result_reg = DIAGNOSTICS_LOOKUP_RESULT_REGISTER(EPNI, ESEM);
            ser_info->diagnostics_lookup_field = DIAGNOSTICS_LOOKUP_FIELD(ESEM);
            ser_info->diagnostics_read_result_reg = DIAGNOSTICS_READ_RESULT_REGISTER(EPNI, ESEM);
            ser_info->diagnostics_read_field = DIAGNOSTICS_READ_FIELD(ESEM);
            ser_info->diagnostics_index_reg = DIAGNOSTICS_INDEX_REGISTER(EPNI, ESEM);
            ser_info->keyt_mem = EPNI_MEM_520000m;
            ser_info->pldt_mem = EPNI_MEM_5B0000m;
            ser_info->aux_cam_mem = EPNI_MEM_5A0000m;
            ser_info->counter_reg = ENTRIES_COUNTER_REGISTER(EPNI, ESEM);
            break;

        case ARAD_PP_EM_SER_TYPE_OEMA_KEYT:
        case ARAD_PP_EM_SER_TYPE_OEMA_PLDT:
            ser_info->db_type = ARAD_PP_EM_SER_DB_TYPE_OEMA;
            ser_info->req_mem = IHB_OEMA_MANAGEMENT_REQUESTm;
            ser_info->key_start_bit = ARAD_PP_EM_SER_OEMA_REQUEST_KEY_START_BIT;
            ser_info->key_size = ARAD_PP_EM_SER_OEMA_REQUEST_KEY_SIZE;
            ser_info->payload_start_bit = ARAD_PP_EM_SER_OEMA_REQUEST_PAYLOAD_START_BIT;
            ser_info->payload_size = ARAD_PP_EM_SER_OEMA_REQUEST_PAYLOAD_SIZE;
            ser_info->shadow_key_size = ARAD_CHIP_SIM_OEMA_KEY;
            ser_info->shadow_payload_size = ARAD_CHIP_SIM_OEMA_PAYLOAD;
            ser_info->shadow_base = ARAD_CHIP_SIM_OEMA_BASE;
            ser_info->db_nof_lines = ARAD_PP_EM_SER_OEMA_NOF_LINES;
            ser_info->diagnostics_reg = DIAGNOSTICS_REGISTER(IHB, OEMA);
            ser_info->diagnostics_key_reg = DIAGNOSTICS_KEY_REGISTER(IHB, OEMA);
            ser_info->diagnostics_lookup_result_reg = DIAGNOSTICS_LOOKUP_RESULT_REGISTER(IHB, OEMA);
            ser_info->diagnostics_lookup_field = DIAGNOSTICS_LOOKUP_FIELD(OEMA);
            ser_info->diagnostics_read_result_reg = DIAGNOSTICS_READ_RESULT_REGISTER(IHB, OEMA);
            ser_info->diagnostics_read_field = DIAGNOSTICS_READ_FIELD(OEMA);
            ser_info->diagnostics_index_reg = DIAGNOSTICS_INDEX_REGISTER(IHB, OEMA);
            ser_info->keyt_mem = IHB_MEM_EA0000m;
            ser_info->pldt_mem = IHB_MEM_F30000m;
            ser_info->aux_cam_mem = IHB_MEM_F20000m;
            ser_info->counter_reg = ENTRIES_COUNTER_REGISTER(IHB, OEMA);
            break;

        case ARAD_PP_EM_SER_TYPE_OEMB_KEYT:
        case ARAD_PP_EM_SER_TYPE_OEMB_PLDT:
            ser_info->db_type = ARAD_PP_EM_SER_DB_TYPE_OEMB;
            ser_info->req_mem = IHB_OEMB_MANAGEMENT_REQUESTm;
            ser_info->key_start_bit = ARAD_PP_EM_SER_OEMB_REQUEST_KEY_START_BIT;
            ser_info->key_size = ARAD_PP_EM_SER_OEMB_REQUEST_KEY_SIZE;
            ser_info->payload_start_bit = ARAD_PP_EM_SER_OEMB_REQUEST_PAYLOAD_START_BIT;
            ser_info->payload_size = ARAD_PP_EM_SER_OEMB_REQUEST_PAYLOAD_SIZE;
            ser_info->shadow_key_size = ARAD_CHIP_SIM_OEMB_KEY;
            ser_info->shadow_payload_size = ARAD_CHIP_SIM_OEMB_PAYLOAD;
            ser_info->shadow_base = ARAD_CHIP_SIM_OEMB_BASE;
            ser_info->db_nof_lines = ARAD_PP_EM_SER_OEMB_NOF_LINES;
            ser_info->diagnostics_reg = DIAGNOSTICS_REGISTER(IHB, OEMB);
            ser_info->diagnostics_key_reg = DIAGNOSTICS_KEY_REGISTER(IHB, OEMB);
            ser_info->diagnostics_lookup_result_reg = DIAGNOSTICS_LOOKUP_RESULT_REGISTER(IHB, OEMB);
            ser_info->diagnostics_lookup_field = DIAGNOSTICS_LOOKUP_FIELD(OEMB);
            ser_info->diagnostics_read_result_reg = DIAGNOSTICS_READ_RESULT_REGISTER(IHB, OEMB);
            ser_info->diagnostics_read_field = DIAGNOSTICS_READ_FIELD(OEMB);
            ser_info->diagnostics_index_reg = DIAGNOSTICS_INDEX_REGISTER(IHB, OEMB);
            ser_info->keyt_mem = IHB_MEM_14A0000m;
            ser_info->pldt_mem = IHB_MEM_1530000m;
            ser_info->aux_cam_mem = IHB_MEM_1520000m;
            ser_info->counter_reg = ENTRIES_COUNTER_REGISTER(IHB, OEMB);
            break;

        case ARAD_PP_EM_SER_TYPE_RMAPEM_KEYT:
        case ARAD_PP_EM_SER_TYPE_RMAPEM_PLDT:
            ser_info->db_type = ARAD_PP_EM_SER_DB_TYPE_RMAPEM;
            ser_info->req_mem = OAMP_RMAPEM_MANAGEMENT_REQUESTm;
            ser_info->key_start_bit = ARAD_PP_EM_SER_RMAPEM_REQUEST_KEY_START_BIT;
            ser_info->key_size = ARAD_PP_EM_SER_RMAPEM_REQUEST_KEY_SIZE;
            ser_info->payload_start_bit = ARAD_PP_EM_SER_RMAPEM_REQUEST_PAYLOAD_START_BIT;
            ser_info->payload_size = ARAD_PP_EM_SER_RMAPEM_REQUEST_PAYLOAD_SIZE;
            ser_info->shadow_key_size = ARAD_CHIP_SIM_RMAPEM_KEY;
            ser_info->shadow_payload_size = ARAD_CHIP_SIM_RMAPEM_PAYLOAD;
            ser_info->shadow_base = ARAD_CHIP_SIM_RMAPEM_BASE;
            ser_info->db_nof_lines = ARAD_PP_EM_SER_RMAPEM_NOF_LINES;
            ser_info->diagnostics_reg = DIAGNOSTICS_REGISTER(OAMP, RMAPEM);
            ser_info->diagnostics_key_reg = DIAGNOSTICS_KEY_REGISTER(OAMP, RMAPEM);
            ser_info->diagnostics_lookup_result_reg = DIAGNOSTICS_LOOKUP_RESULT_REGISTER(OAMP, RMAPEM);
            ser_info->diagnostics_lookup_field = DIAGNOSTICS_LOOKUP_FIELD(RMAPEM);
            ser_info->diagnostics_read_result_reg = DIAGNOSTICS_READ_RESULT_REGISTER(OAMP, RMAPEM);
            ser_info->diagnostics_read_field = DIAGNOSTICS_READ_FIELD(RMAPEM);
            ser_info->diagnostics_index_reg = DIAGNOSTICS_INDEX_REGISTER(OAMP, RMAPEM);
            ser_info->keyt_mem = OAMP_MEM_100000m;
            ser_info->pldt_mem = OAMP_MEM_190000m;
            ser_info->aux_cam_mem = IHB_MEM_1520000m;
            ser_info->counter_reg = ENTRIES_COUNTER_REGISTER(OAMP, RMAPEM);
            break;

        default:
            break;
    }

    SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

uint32 arad_pp_em_ser_mask_interrupt_set(int unit,
                                         ARAD_PP_EM_SER_TYPE em_ser_type, 
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

    switch (em_ser_type) {
        case ARAD_PP_EM_SER_TYPE_LEM_KEYT:
        case ARAD_PP_EM_SER_TYPE_LEM_PLDT:
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHP_PAR_0_ERR_MEM_MASKr(unit, &reg_32_val));
            MASK_SER_INTERRUPTS_32B_SET(MACT, LEM_ERR_MEM_MASK_REGISTER, &reg_32_val, mask_val_32);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_IHP_PAR_0_ERR_MEM_MASKr(unit, reg_32_val));
            break;

        case ARAD_PP_EM_SER_TYPE_ISA_KEYT:
        case ARAD_PP_EM_SER_TYPE_ISA_PLDT:
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_IHP_PAR_1_ERR_MEM_MASKr(unit, &reg_32_val));
            MASK_SER_INTERRUPTS_32B_SET(ISA, ISA_ERR_MEM_MASK_REGISTER, &reg_32_val, mask_val_32);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, WRITE_IHP_PAR_1_ERR_MEM_MASKr(unit, reg_32_val));
            break;

        case ARAD_PP_EM_SER_TYPE_ISB_KEYT:
        case ARAD_PP_EM_SER_TYPE_ISB_PLDT:
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_IHP_PAR_3_ERR_MEM_MASKr(unit, &reg_32_val));
            MASK_SER_INTERRUPTS_32B_SET(ISB, ISB_ERR_MEM_MASK_REGISTER, &reg_32_val, mask_val_32);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, WRITE_IHP_PAR_3_ERR_MEM_MASKr(unit, reg_32_val));

            break;
        case ARAD_PP_EM_SER_TYPE_ESEM_KEYT:
        case ARAD_PP_EM_SER_TYPE_ESEM_PLDT:
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, READ_EPNI_PAR_ERR_MEM_MASKr(unit, 0, reg_above_64_val));
            MASK_SER_INTERRUPTS_ABOVE_64B_SET(ESEM, ESEM_ERR_MEM_MASK_REGISTER, reg_above_64_val, mask_val_above_64);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, WRITE_EPNI_PAR_ERR_MEM_MASKr(unit, 0, reg_above_64_val));
            break;

        case ARAD_PP_EM_SER_TYPE_OEMA_KEYT:
        case ARAD_PP_EM_SER_TYPE_OEMA_PLDT:
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 70, exit, READ_IHB_PAR_ERR_MEM_MASKr(unit, 0, reg_above_64_val));
            MASK_SER_INTERRUPTS_ABOVE_64B_SET(OEMA, OEMA_ERR_MEM_MASK_REGISTER, reg_above_64_val, mask_val_above_64);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 80, exit, WRITE_IHB_PAR_ERR_MEM_MASKr(unit, 0, reg_above_64_val));
            break;

        case ARAD_PP_EM_SER_TYPE_OEMB_KEYT:
        case ARAD_PP_EM_SER_TYPE_OEMB_PLDT:
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 90, exit, READ_IHB_PAR_ERR_MEM_MASKr(unit, 0, reg_above_64_val));
            MASK_SER_INTERRUPTS_ABOVE_64B_SET(OEMB, OEMB_ERR_MEM_MASK_REGISTER, reg_above_64_val, mask_val_above_64);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 100, exit, WRITE_IHB_PAR_ERR_MEM_MASKr(unit, 0, reg_above_64_val));
            break;

        case ARAD_PP_EM_SER_TYPE_RMAPEM_KEYT:
        case ARAD_PP_EM_SER_TYPE_RMAPEM_PLDT:
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, READ_OAMP_PAR_0_ERR_MONITOR_MEM_MASKr(unit, &reg_64_val));
            MASK_SER_INTERRUPTS_64B_SET(RMAPEM, RMAPEM_ERR_MEM_MASK_REGISTER, &reg_64_val, mask_val_64);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 120, exit, WRITE_OAMP_PAR_0_ERR_MONITOR_MEM_MASKr(unit, reg_64_val));
            break;

        default:
            res = SOC_SAND_ERR;
            break;
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

uint32 arad_pp_em_ser_management_enable_set(int unit,
                                            ARAD_PP_EM_SER_DB_TYPE db_type, 
                                            int val)
{
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    switch (db_type) {
       case ARAD_PP_EM_SER_DB_TYPE_LEM:
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, MANAGEMENT_UNIT_CONFIGURATION_REGISTER(IHP,MACT), REG_PORT_ANY, 0, MNGMNT_UNIT_ENABLE_FIELD(MACT),  val));
            break;

       case ARAD_PP_EM_SER_DB_TYPE_ISA:
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, MANAGEMENT_UNIT_CONFIGURATION_REGISTER(IHP,ISA), REG_PORT_ANY, 0, MNGMNT_UNIT_ENABLE_FIELD(ISA),  val));
            break;

       case ARAD_PP_EM_SER_DB_TYPE_ISB:
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, MANAGEMENT_UNIT_CONFIGURATION_REGISTER(IHP,ISB), REG_PORT_ANY, 0, MNGMNT_UNIT_ENABLE_FIELD(ISB),  val));
            break;
       case ARAD_PP_EM_SER_DB_TYPE_ESEM:
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, MANAGEMENT_UNIT_CONFIGURATION_REGISTER(EPNI,ESEM), REG_PORT_ANY, 0, MNGMNT_UNIT_ENABLE_FIELD(ESEM),  val));
            break;

       case ARAD_PP_EM_SER_DB_TYPE_OEMA:
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, MANAGEMENT_UNIT_CONFIGURATION_REGISTER(IHB,OEMA), REG_PORT_ANY, 0, MNGMNT_UNIT_ENABLE_FIELD(OEMA),  val));
            break;

       case ARAD_PP_EM_SER_DB_TYPE_OEMB:
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, MANAGEMENT_UNIT_CONFIGURATION_REGISTER(IHB,OEMB), REG_PORT_ANY, 0, MNGMNT_UNIT_ENABLE_FIELD(OEMB),  val));
            break;

       case ARAD_PP_EM_SER_DB_TYPE_RMAPEM:
           SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, MANAGEMENT_UNIT_CONFIGURATION_REGISTER(OAMP,RMAPEM), REG_PORT_ANY, 0, MNGMNT_UNIT_ENABLE_FIELD(RMAPEM),  val));
            break;

        default:
            res = SOC_SAND_ERR;
            break;
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}
int arad_pp_em_ser_parity_error_info_get(int unit,
                                         ARAD_PP_EM_SER_BLOCK block,
                                         uint32 *address,
                                         uint32 *address_valid,
                                         uint32 *counter,
                                         uint32 *counter_overflow)
{
    int res;
    uint64 reg_val;
    soc_reg_t parity_err_reg;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(address);
    SOC_SAND_CHECK_NULL_INPUT(address_valid);
    SOC_SAND_CHECK_NULL_INPUT(counter);
    SOC_SAND_CHECK_NULL_INPUT(counter_overflow); 

    switch (block) {
       case ARAD_PP_EM_SER_BLOCK_IHP:
           parity_err_reg = PARITY_ERR_CNT_REGISTER(IHP);
           break;

       case ARAD_PP_EM_SER_BLOCK_IHB:
           parity_err_reg = PARITY_ERR_CNT_REGISTER(IHB);
           break;

       case ARAD_PP_EM_SER_BLOCK_EPNI:
           parity_err_reg = PARITY_ERR_CNT_REGISTER(EPNI);
           break;

       case ARAD_PP_EM_SER_BLOCK_OAMP:
           parity_err_reg = PARITY_ERR_CNT_REGISTER(OAMP);
           break;

       default:
           res = SOC_SAND_ERR;
           goto exit;
    }
    res = soc_reg_get(unit, parity_err_reg, REG_PORT_ANY, 0, &reg_val);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

    
    *address_valid = soc_reg64_field32_get(unit, parity_err_reg, reg_val, PARITY_ERR_ADDR_VALIDf);
  
    
    *address = soc_reg64_field32_get(unit, parity_err_reg, reg_val, PARITY_ERR_ADDRf);

     
    *counter_overflow = soc_reg64_field32_get(unit, parity_err_reg, reg_val, PARITY_ERR_CNT_OVERFLOWf);

    
    *counter = soc_reg64_field32_get(unit, parity_err_reg, reg_val, PARITY_ERR_CNTf);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}


uint32 arad_pp_em_ser_type_get(int unit, uint32 address, ARAD_PP_EM_SER_BLOCK block, ARAD_PP_EM_SER_TYPE *em_ser_type, soc_mem_t *mem, uint32 *base_address)  
{

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    *em_ser_type = ARAD_PP_EM_SER_TYPE_LAST;

    if (block == ARAD_PP_EM_SER_BLOCK_IHP) {
        if ((address >= ARAD_PP_EM_SER_LEM_KEYT_BASE_ADDR) &&
            (address <= (ARAD_PP_EM_SER_LEM_KEYT_BASE_ADDR + ARAD_PP_EM_SER_LEM_NOF_LINES))) {
            *em_ser_type = ARAD_PP_EM_SER_TYPE_LEM_KEYT;
            *mem = IHP_MEM_500000m;
            *base_address = ARAD_PP_EM_SER_LEM_KEYT_BASE_ADDR;
        }
        else if ((address >= ARAD_PP_EM_SER_LEM_PLDT_BASE_ADDR) &&
                 (address <= (ARAD_PP_EM_SER_LEM_PLDT_BASE_ADDR + ARAD_PP_EM_SER_LEM_NOF_LINES))) {
            *em_ser_type = ARAD_PP_EM_SER_TYPE_LEM_PLDT;
            *mem = IHP_MEM_590000m;
            *base_address = ARAD_PP_EM_SER_LEM_PLDT_BASE_ADDR;
        }
        else if ((address >= ARAD_PP_EM_SER_ISA_KEYT_BASE_ADDR) &&
            (address <= (ARAD_PP_EM_SER_ISA_KEYT_BASE_ADDR + ARAD_PP_EM_SER_ISA_NOF_LINES))) {
            *em_ser_type = ARAD_PP_EM_SER_TYPE_ISA_KEYT;
            *mem = IHP_MEM_7A0000m;
            *base_address = ARAD_PP_EM_SER_ISA_KEYT_BASE_ADDR;
        }
        else if ((address >= ARAD_PP_EM_SER_ISA_PLDT_BASE_ADDR) &&
                 (address <= (ARAD_PP_EM_SER_ISA_PLDT_BASE_ADDR + ARAD_PP_EM_SER_ISA_NOF_LINES))) {
            *em_ser_type = ARAD_PP_EM_SER_TYPE_ISA_PLDT;
            *mem = IHP_MEM_830000m;
            *base_address = ARAD_PP_EM_SER_ISA_PLDT_BASE_ADDR;
        }
        else if ((address >= ARAD_PP_EM_SER_ISB_KEYT_BASE_ADDR) &&
            (address <= (ARAD_PP_EM_SER_ISB_KEYT_BASE_ADDR + ARAD_PP_EM_SER_ISB_NOF_LINES))) {
            *em_ser_type = ARAD_PP_EM_SER_TYPE_ISB_KEYT;
            *mem = IHP_MEM_A10000m;
            *base_address = ARAD_PP_EM_SER_ISB_KEYT_BASE_ADDR;
        }
        else if ((address >= ARAD_PP_EM_SER_ISB_PLDT_BASE_ADDR) &&
                 (address <= (ARAD_PP_EM_SER_ISB_PLDT_BASE_ADDR + ARAD_PP_EM_SER_ISB_NOF_LINES))) {
            *em_ser_type = ARAD_PP_EM_SER_TYPE_ISB_PLDT;
            *mem = IHP_MEM_AA0000m;
            *base_address = ARAD_PP_EM_SER_ISB_PLDT_BASE_ADDR;
        }
    }
    else if (block == ARAD_PP_EM_SER_BLOCK_EPNI) {
        if ((address >= ARAD_PP_EM_SER_ESEM_KEYT_BASE_ADDR) &&
            (address <= (ARAD_PP_EM_SER_ESEM_KEYT_BASE_ADDR + ARAD_PP_EM_SER_ESEM_NOF_LINES))) {
            *em_ser_type = ARAD_PP_EM_SER_TYPE_ESEM_KEYT;
            *mem = EPNI_MEM_520000m;
            *base_address = ARAD_PP_EM_SER_ESEM_KEYT_BASE_ADDR;
        }
        else if ((address >= ARAD_PP_EM_SER_ESEM_PLDT_BASE_ADDR) &&
                 (address <= (ARAD_PP_EM_SER_ESEM_PLDT_BASE_ADDR + ARAD_PP_EM_SER_ESEM_NOF_LINES))) {
            *em_ser_type = ARAD_PP_EM_SER_TYPE_ESEM_PLDT;
            *mem = EPNI_MEM_5B0000m;
            *base_address = ARAD_PP_EM_SER_ESEM_PLDT_BASE_ADDR;
        }
    }
    else if (block == ARAD_PP_EM_SER_BLOCK_IHB) {
        if ((address >= ARAD_PP_EM_SER_OEMA_KEYT_BASE_ADDR) &&
            (address <= (ARAD_PP_EM_SER_OEMA_KEYT_BASE_ADDR + ARAD_PP_EM_SER_OEMA_NOF_LINES))) {
            *em_ser_type = ARAD_PP_EM_SER_TYPE_OEMA_KEYT;
            *mem = IHB_MEM_EA0000m;
            *base_address = ARAD_PP_EM_SER_OEMA_KEYT_BASE_ADDR;
        }
        else if ((address >= ARAD_PP_EM_SER_OEMA_PLDT_BASE_ADDR) &&
                 (address <= (ARAD_PP_EM_SER_OEMA_PLDT_BASE_ADDR + ARAD_PP_EM_SER_OEMA_NOF_LINES))) {
            *em_ser_type = ARAD_PP_EM_SER_TYPE_OEMA_PLDT;
            *mem = IHB_MEM_F30000m;
            *base_address = ARAD_PP_EM_SER_OEMA_PLDT_BASE_ADDR;
        }
        else if ((address >= ARAD_PP_EM_SER_OEMB_KEYT_BASE_ADDR) &&
            (address <= (ARAD_PP_EM_SER_OEMB_KEYT_BASE_ADDR + ARAD_PP_EM_SER_OEMB_NOF_LINES))) {
            *em_ser_type = ARAD_PP_EM_SER_TYPE_OEMB_KEYT;
            *mem = IHB_MEM_14A0000m;
            *base_address = ARAD_PP_EM_SER_OEMB_KEYT_BASE_ADDR;
        }
        else if ((address >= ARAD_PP_EM_SER_OEMB_PLDT_BASE_ADDR) &&
                 (address <= (ARAD_PP_EM_SER_OEMB_PLDT_BASE_ADDR + ARAD_PP_EM_SER_OEMB_NOF_LINES))) {
            *em_ser_type = ARAD_PP_EM_SER_TYPE_OEMB_PLDT;
            *mem = IHB_MEM_1530000m;
            *base_address = ARAD_PP_EM_SER_OEMB_PLDT_BASE_ADDR;
        }
    }
    else if (block == ARAD_PP_EM_SER_BLOCK_OAMP) {
        if ((address >= ARAD_PP_EM_SER_RMAPEM_KEYT_BASE_ADDR) &&
            (address <= (ARAD_PP_EM_SER_RMAPEM_KEYT_BASE_ADDR + ARAD_PP_EM_SER_RMAPEM_NOF_LINES))) {
            *em_ser_type = ARAD_PP_EM_SER_TYPE_RMAPEM_KEYT;
            *mem = OAMP_MEM_100000m;
            *base_address = ARAD_PP_EM_SER_RMAPEM_KEYT_BASE_ADDR;
        }
        else if ((address >= ARAD_PP_EM_SER_RMAPEM_PLDT_BASE_ADDR) &&
                 (address <= (ARAD_PP_EM_SER_RMAPEM_PLDT_BASE_ADDR + ARAD_PP_EM_SER_RMAPEM_NOF_LINES))) {
            *em_ser_type = ARAD_PP_EM_SER_TYPE_RMAPEM_PLDT;
            *mem = OAMP_MEM_190000m;
            *base_address = ARAD_PP_EM_SER_RMAPEM_PLDT_BASE_ADDR;
        }
    }

    SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}


uint32 arad_pp_em_ser_address_get(int unit, soc_mem_t mem, uint32 array_index, uint32 index, uint32 *address, uint8 *is_em_ser)  
{

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    *is_em_ser = 1;

    switch(mem)
    {
       case IHP_MEM_500000m:
           *address = ARAD_PP_EM_SER_LEM_KEYT_BASE_ADDR + (ARAD_PP_EM_SER_LEM_NOF_LINES/ARAD_PP_EM_SER_NOF_HASH_RESULTS) * array_index + index;
           break;

       case IHP_MEM_590000m:
           *address = ARAD_PP_EM_SER_LEM_PLDT_BASE_ADDR + (ARAD_PP_EM_SER_LEM_NOF_LINES/ARAD_PP_EM_SER_NOF_HASH_RESULTS) * array_index + index;
           break;

       case IHP_MEM_7A0000m:
           *address = ARAD_PP_EM_SER_ISA_KEYT_BASE_ADDR + (ARAD_PP_EM_SER_ISA_NOF_LINES/ARAD_PP_EM_SER_NOF_HASH_RESULTS) * array_index + index;
           break;

       case IHP_MEM_830000m:
           *address = ARAD_PP_EM_SER_ISA_PLDT_BASE_ADDR + (ARAD_PP_EM_SER_ISA_NOF_LINES/ARAD_PP_EM_SER_NOF_HASH_RESULTS) * array_index + index;
           break;

       case IHP_MEM_A10000m:
           *address = ARAD_PP_EM_SER_ISB_KEYT_BASE_ADDR + (ARAD_PP_EM_SER_ISB_NOF_LINES/ARAD_PP_EM_SER_NOF_HASH_RESULTS) * array_index + index;
           break;

       case IHP_MEM_AA0000m:
           *address = ARAD_PP_EM_SER_ISB_PLDT_BASE_ADDR + (ARAD_PP_EM_SER_ISB_NOF_LINES/ARAD_PP_EM_SER_NOF_HASH_RESULTS) * array_index + index;
           break;

       case EPNI_MEM_520000m:
           *address = ARAD_PP_EM_SER_ESEM_KEYT_BASE_ADDR + (ARAD_PP_EM_SER_ESEM_NOF_LINES/ARAD_PP_EM_SER_NOF_HASH_RESULTS) * array_index + index;
           break;

       case EPNI_MEM_5B0000m:
           *address = ARAD_PP_EM_SER_ESEM_PLDT_BASE_ADDR + (ARAD_PP_EM_SER_ESEM_NOF_LINES/ARAD_PP_EM_SER_NOF_HASH_RESULTS) * array_index + index;
           break;

       case IHB_MEM_EA0000m:
           *address = ARAD_PP_EM_SER_OEMA_KEYT_BASE_ADDR + (ARAD_PP_EM_SER_OEMA_NOF_LINES/ARAD_PP_EM_SER_NOF_HASH_RESULTS) * array_index + index;
           break;

       case IHB_MEM_F30000m:
           *address = ARAD_PP_EM_SER_OEMA_PLDT_BASE_ADDR + (ARAD_PP_EM_SER_OEMA_NOF_LINES/ARAD_PP_EM_SER_NOF_HASH_RESULTS) * array_index + index;
           break;

       case IHB_MEM_14A0000m:
           *address = ARAD_PP_EM_SER_OEMB_KEYT_BASE_ADDR + (ARAD_PP_EM_SER_OEMB_NOF_LINES/ARAD_PP_EM_SER_NOF_HASH_RESULTS) * array_index + index;
           break;

       case IHB_MEM_1530000m:
           *address = ARAD_PP_EM_SER_OEMB_PLDT_BASE_ADDR + (ARAD_PP_EM_SER_OEMB_NOF_LINES/ARAD_PP_EM_SER_NOF_HASH_RESULTS) * array_index + index;
           break;

       case OAMP_MEM_100000m:
           *address = ARAD_PP_EM_SER_RMAPEM_KEYT_BASE_ADDR + (ARAD_PP_EM_SER_RMAPEM_NOF_LINES/ARAD_PP_EM_SER_NOF_HASH_RESULTS) * array_index + index;
           break;

       case OAMP_MEM_190000m:
           *address = ARAD_PP_EM_SER_RMAPEM_PLDT_BASE_ADDR + (ARAD_PP_EM_SER_RMAPEM_NOF_LINES/ARAD_PP_EM_SER_NOF_HASH_RESULTS) * array_index + index;
           break;

       default:
           *is_em_ser = 0;
           break;
    }

    SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}


uint32 arad_pp_em_lem_counter_increment(int unit, int inc_val)
{
    uint32 i, j, is_found=0;
    uint32 key_data[SOC_DPP_DEFS_MAX_LEM_WIDTH_IN_UINT32S]; 
    uint32 verifier_data[ARAD_PP_EM_SER_LEM_VERIFIER_LEN_IN_UINT32] = {0};     
    soc_reg_above_64_val_t reg_above_64_val, request_val, fld_value, tmp, dummy_key_value;
    uint32 *entry_index, entry_location;
    uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
    SOC_REG_ABOVE_64_CLEAR(fld_value);
    SOC_REG_ABOVE_64_CLEAR(dummy_key_value);
    SOC_REG_ABOVE_64_CLEAR(tmp);
    SOC_REG_ABOVE_64_CLEAR(request_val);

    if (SOC_IS_ARADPLUS(unit)) {
        entry_index = arad_plus_entry_index[ARAD_PP_EM_SER_DB_TYPE_LEM];
    }
    else {
        entry_index = arad_entry_index[ARAD_PP_EM_SER_DB_TYPE_LEM];
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
        ARAD_FLD_TO_REG_ABOVE_64(IHP_MACT_CPU_REQUEST_REQUESTr, MACT_REQ_MFF_IS_KEYf, fld_value, request_val,  15, exit);
        ARAD_FLD_TO_REG_ABOVE_64(IHP_MACT_CPU_REQUEST_REQUESTr, MACT_REQ_MFF_KEYf, dummy_key_value, request_val,  15, exit);
        ARAD_FLD_TO_REG_ABOVE_64(IHP_MACT_CPU_REQUEST_REQUESTr, MACT_REQ_COMMANDf, fld_value, request_val,  15, exit);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_IHP_MACT_CPU_REQUEST_REQUESTr(unit, request_val));

        
        for (j = 0; j < ARAD_PP_EM_SER_NOF_HASH_RESULTS; j++) {
             
            entry_location = entry_index[j] + (ARAD_PP_EM_SER_LEM_NOF_LINES/ARAD_PP_EM_SER_NOF_HASH_RESULTS) * j;
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, WRITE_IHP_MACT_DIAGNOSTICS_INDEXr(unit, entry_location));
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR, soc_reg_field32_modify(unit, PPDB_B_LARGE_EM_DIAGNOSTICSr, REG_PORT_ANY, MACT_DIAGNOSTICS_READf, 1));

            
                res = arad_polling(
                      unit,
                      ARAD_TIMEOUT,
                      ARAD_MIN_POLLS,
                      IHP_MACT_DIAGNOSTICSr,
                      REG_PORT_ANY,
                      0,
                      MACT_DIAGNOSTICS_READf,
                      0
                    );
                SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

            SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, READ_IHP_MACT_DIAGNOSTICS_READ_RESULTr(unit, reg_above_64_val));

            key_data[0] = 0;
            key_data[1] = 0;
            key_data[2] = 0;

            
            res = soc_sand_bitstream_get_any_field(
                  reg_above_64_val,
                  1, 
                  74, 
                  key_data
                );
            SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);


            if ((key_data[0] == dummy_key_value[0]) && 
                (key_data[1] == dummy_key_value[1]) &&
                (key_data[2] == dummy_key_value[2])) {
                
                is_found = 1;

                
                
                res = soc_mem_array_write(unit,
                                          IHP_MEM_500000m,
                                          j, 
                                          MEM_BLOCK_ANY,             
                                          entry_index[j],
                                          verifier_data);
                SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

                break;
            }   
        }
        if (!is_found) {
             
            for (j=0; j < ARAD_PP_EM_SER_EM_AUX_CAM_SIZE; j++) {
                res = soc_mem_read(unit,
                                   IHP_MEM_580000m,
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
                                    IHP_MEM_580000m,
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


uint32 arad_pp_em_em_counter_increment(int unit, ARAD_PP_SER_EM_TYPE_INFO *ser_info, int inc_val)
{
    uint32 i, j, is_found=0;
    uint32 key_data[2] = {0}; 
    uint32 verifier_data[2] = {0};     
    soc_reg_above_64_val_t reg_above_64_val, tmp, dummy_key_value, read_result;
    uint32 *entry_index, entry_location, temp, buffer[3] = {0};
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
    SOC_REG_ABOVE_64_CLEAR(dummy_key_value);
    SOC_REG_ABOVE_64_CLEAR(tmp);
    SOC_REG_ABOVE_64_CLEAR(read_result);

    
    if (SOC_IS_ARADPLUS(unit)) {
        entry_index = arad_plus_entry_index[ser_info->db_type];
    }
    else {
        entry_index = arad_entry_index[ser_info->db_type];
    }

    
    sal_memset(tmp, 0xff, sizeof(soc_reg_above_64_val_t));
    res = soc_sand_bitstream_set_any_field(
          tmp,
          0,
          ser_info->key_size,
          dummy_key_value
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    for (i = 0; i < inc_val; i++) {
        

        
        temp = ARAD_PP_EM_SER_REQUEST_TYPE_INSERT_VAL;
        res = soc_sand_bitstream_set_any_field(
              &temp,
              ARAD_PP_EM_SER_REQUEST_TYPE_START_BIT,
              ARAD_PP_EM_SER_REQUEST_TYPE_SIZE,
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

        
        res = soc_mem_write(unit, ser_info->req_mem, MEM_BLOCK_ALL, 0, buffer);
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

        
        for (j = 0; j < ARAD_PP_EM_SER_NOF_HASH_RESULTS; j++) {
             
            entry_location = entry_index[j] + ((ser_info->db_nof_lines)/ARAD_PP_EM_SER_NOF_HASH_RESULTS) * j;
            res = soc_reg32_set(unit, ser_info->diagnostics_index_reg, REG_PORT_ANY, 0, entry_location);
            SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ser_info->diagnostics_reg, REG_PORT_ANY, 0, ser_info->diagnostics_read_field, 1));

            
                res = arad_polling(
                      unit,
                      ARAD_TIMEOUT,
                      ARAD_MIN_POLLS,
                      ser_info->diagnostics_reg,
                      REG_PORT_ANY,
                      0,
                      ser_info->diagnostics_read_field,
                      0
                    );
                SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
            res = soc_reg_above_64_get(unit, ser_info->diagnostics_read_result_reg, REG_PORT_ANY, 0, read_result);
            SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

            
            res = soc_sand_bitstream_get_any_field(
                  read_result,
                  1, 
                  ser_info->key_size, 
                  key_data
                );
            SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);


            if ((key_data[0] == dummy_key_value[0]) && (key_data[1] == dummy_key_value[1])) {
                is_found = 1;

                
                res = soc_mem_write(unit,
                                ser_info->keyt_mem,
                                MEM_BLOCK_ANY,             
                                entry_location,
                                verifier_data);
                SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

                break;
            }   
        }
        if (!is_found) {
             
            for (j=0; j < ARAD_PP_EM_SER_EM_AUX_CAM_SIZE; j++) {
                res = soc_mem_read(unit,
                                   ser_info->aux_cam_mem,
                                   MEM_BLOCK_ANY,             
                                   j,
                                   reg_above_64_val);
                SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

                if ((reg_above_64_val[0] == dummy_key_value[0]) && 
                    (reg_above_64_val[1] == dummy_key_value[1])) {
                    
                    is_found = 1;
                    key_data[0] = 0;
                    key_data[1] = 0;

                    
                    res = soc_mem_write(unit,
                                    ser_info->aux_cam_mem,
                                    MEM_BLOCK_ANY,             
                                    j, 
                                    key_data);
                    SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
                    break;
                }
            }
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}


uint32 arad_pp_em_lem_counter_decrement(int unit, int dec_val)
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
        
        for (j=0; j < ARAD_PP_EM_SER_EM_AUX_CAM_SIZE; j++) {
            res = soc_mem_read(unit,
                               IHP_MEM_580000m,
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

        if (j == ARAD_PP_EM_SER_EM_AUX_CAM_SIZE) {
            
            res = SOC_SAND_ERR;
            goto exit;;
        }

        
        res = soc_mem_write(unit,
                            IHP_MEM_580000m,
                            MEM_BLOCK_ANY,             
                            j, 
                            dummy_key_value_valid);
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

        
        fld_value[0] = 1;
        ARAD_FLD_TO_REG_ABOVE_64(IHP_MACT_CPU_REQUEST_REQUESTr, MACT_REQ_MFF_IS_KEYf, fld_value, request_val,  60, exit);
        ARAD_FLD_TO_REG_ABOVE_64(IHP_MACT_CPU_REQUEST_REQUESTr, MACT_REQ_MFF_KEYf, dummy_key_value, request_val,  70, exit);
        fld_value[0] = 0;
        ARAD_FLD_TO_REG_ABOVE_64(IHP_MACT_CPU_REQUEST_REQUESTr, MACT_REQ_COMMANDf, fld_value, request_val,  80, exit);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 1000, exit, WRITE_IHP_MACT_CPU_REQUEST_REQUESTr(unit, request_val));    
    }
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}


uint32 arad_pp_em_em_counter_decrement(int unit, ARAD_PP_SER_EM_TYPE_INFO *ser_info, int dec_val)
{
    uint32 res = SOC_SAND_OK;
    uint32 is_valid, temp, buffer[3] = {0};
    int i,j;
    soc_reg_above_64_val_t reg_above_64_val, request_val, tmp;
    soc_reg_above_64_val_t dummy_key_value, dummy_key_value_valid;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
    SOC_REG_ABOVE_64_CLEAR(dummy_key_value);
    SOC_REG_ABOVE_64_CLEAR(dummy_key_value_valid);
    SOC_REG_ABOVE_64_CLEAR(request_val);
    SOC_REG_ABOVE_64_CLEAR(tmp);

    
    sal_memset(tmp, 0xff, sizeof(soc_reg_above_64_val_t));
    res = soc_sand_bitstream_set_any_field(
          tmp,
          0,
          ser_info->key_size + 1, 
          dummy_key_value_valid
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    SOC_REG_ABOVE_64_CLEAR(dummy_key_value);
    res = soc_sand_bitstream_set_any_field(
          tmp,
          0,
          ser_info->key_size,
          dummy_key_value
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

    for (i = 0; i < dec_val; i++) {
        
        for (j=0; j < ARAD_PP_EM_SER_EM_AUX_CAM_SIZE; j++) {
            res = soc_mem_read(unit,
                               ser_info->aux_cam_mem,
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

        if (j == ARAD_PP_EM_SER_EM_AUX_CAM_SIZE) {
            
            res = SOC_SAND_ERR;
            goto exit;
        }

        
        res = soc_mem_write(unit,
                            ser_info->aux_cam_mem,
                            MEM_BLOCK_ANY,             
                            j, 
                            dummy_key_value_valid);
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

        
        
        temp = ARAD_PP_EM_SER_REQUEST_TYPE_DELETE_VAL;
        res = soc_sand_bitstream_set_any_field(
              &temp,
              ARAD_PP_EM_SER_REQUEST_TYPE_START_BIT,
              ARAD_PP_EM_SER_REQUEST_TYPE_SIZE,
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

        
        res = soc_mem_write(unit, ser_info->req_mem, MEM_BLOCK_ALL, 0, buffer);
        SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);  
    }
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

uint32 arad_pp_em_ser_mact_counter_fix(int unit)
{
    uint32 res = SOC_SAND_OK;
    ARAD_PP_LEM_ACCESS_KEY                          rule_key, rule_key_mask;
    SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE    rule_val;
    SOC_SAND_TABLE_BLOCK_RANGE                      block_range;
    SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION              action;
    uint32 nof_matched_mact_entries;  

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_PP_LEM_ACCESS_KEY_clear(&rule_key);
    ARAD_PP_LEM_ACCESS_KEY_clear(&rule_key_mask);
    SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE_clear(&rule_val);
    SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_clear(&action);
    soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&block_range);

    block_range.entries_to_scan = SOC_SAND_TBL_ITER_SCAN_ALL;
    rule_key.type = ARAD_PP_LEM_ACCESS_KEY_TYPE_MAC;
    rule_key.prefix.nof_bits = ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_MAC;
    rule_key.prefix.value = ARAD_PP_LEM_ACCESS_KEY_PREFIX_FOR_MAC(unit);

    rule_key_mask.type = ARAD_PP_LEM_ACCESS_KEY_TYPE_MAC;
    rule_key_mask.prefix.nof_bits = ARAD_PP_LEM_ACCESS_KEY_PREFIX_SIZE_IN_BITS_FOR_MAC;
    rule_key_mask.prefix.value = 0xf;

    action.type = SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_NONE; 

    
    res = arad_pp_frwrd_lem_traverse_internal_unsafe(
            unit,
            &rule_key,
            &rule_key_mask,
            &rule_val,
            &block_range,
            &action,
            TRUE, 
            &nof_matched_mact_entries
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_IHP_MACT_COUNTER_LIMIT_MACT_DB_ENTRIES_COUNTr(unit, nof_matched_mact_entries));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}


uint32 arad_pp_em_ser_lem_counter_diff_get(int unit, int *lem_counter_diff)
{
    uint32 res = SOC_SAND_OK;
    uint32 nof_matched_lem_entries, nof_lem_entries; 
    ARAD_PP_LEM_ACCESS_KEY                          rule_key, rule_key_mask;
    SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE    rule_val;
    SOC_SAND_TABLE_BLOCK_RANGE                      block_range;
    SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION              action; 

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_PP_LEM_ACCESS_KEY_clear(&rule_key);
    ARAD_PP_LEM_ACCESS_KEY_clear(&rule_key_mask);
    SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_VALUE_RULE_clear(&rule_val);
    SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_clear(&action);
    soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&block_range);

    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHP_MACT_ENTRIES_COUNTERr(unit, &nof_lem_entries));

    block_range.entries_to_scan = SOC_SAND_TBL_ITER_SCAN_ALL;
    rule_key.prefix.value = ARAD_PP_LEM_ACCESS_KEY_PREFIX_FOR_MAC(unit);
    action.type = SOC_PPC_FRWRD_MACT_TRAVERSE_ACTION_TYPE_NONE; 

    
    res = arad_pp_frwrd_lem_traverse_internal_unsafe(
            unit,
            &rule_key,
            &rule_key_mask,
            &rule_val,
            &block_range,
            &action,
            TRUE, 
            &nof_matched_lem_entries
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    *lem_counter_diff = nof_matched_lem_entries - nof_lem_entries; 

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}


uint32 arad_pp_em_ser_em_counter_diff_get(int unit, ARAD_PP_SER_EM_TYPE_INFO *ser_info, int *em_counter_diff)
{
    uint32 res = SOC_SAND_OK;
    uint32 nof_valid_entries=0, nof_em_entries, is_valid, entry_index, array_index;
    uint32 key_data[SOC_DPP_DEFS_MAX_LEM_WIDTH_IN_UINT32S] = {0}; 

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    res = soc_reg32_get(unit, ser_info->counter_reg, REG_PORT_ANY, 0, &nof_em_entries);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


    for (array_index = 0; array_index < ARAD_PP_EM_SER_NOF_HASH_RESULTS; array_index++) {
        for (entry_index=0; entry_index < (ser_info->db_nof_lines/ARAD_PP_EM_SER_NOF_HASH_RESULTS); entry_index++) {
            is_valid = 0;
            res = soc_mem_array_read(unit,
                                     ser_info->keyt_mem,
                                     array_index,
                                     MEM_BLOCK_ANY,            
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



uint32 arad_pp_em_ser_lem_fid_counters_update(int unit)
{
    uint32 res = SOC_SAND_OK;
    uint32 nof_entries, indx, fid;  
    uint32 tbl_data[1];
    SOC_SAND_TABLE_BLOCK_RANGE                      block_range;
    SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE          rule;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&block_range);
    SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE_clear(&rule);

    sal_memset(lem_fid_lif_counters, 0, sizeof(uint16)*ARAD_PP_EM_SER_LEM_NOF_FIDS);

    
    SOC_SAND_TBL_ITER_SET_BEGIN(&(block_range.iter));
    block_range.entries_to_act = ARAD_PP_EM_SER_LEM_TRVRS_ITER_BLK_SIZE;
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

    
    for (indx = 0; indx < ARAD_PP_EM_SER_LEM_NOF_FIDS; ++indx) {
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


uint32 arad_pp_em_ser_lem_lif_counters_update(int unit)
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

    sal_memset(lem_fid_lif_counters, 0, sizeof(uint16)*ARAD_PP_EM_SER_LEM_NOF_FIDS);

    
    SOC_SAND_TBL_ITER_SET_BEGIN(&(block_range.iter));
    block_range.entries_to_act = ARAD_PP_EM_SER_LEM_TRVRS_ITER_BLK_SIZE;
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
            lif_indx = ARAD_PP_EM_SER_LEM_NOF_FIDS;
            if (l2_traverse_mact_vals[indx].frwrd_info.forward_decision.additional_info.outlif.type != SOC_PPC_OUTLIF_ENCODE_TYPE_NONE) {
                
                lif = l2_traverse_mact_vals[indx].frwrd_info.forward_decision.additional_info.outlif.val;

                
                if ((lif >= range0_min) && (lif < range0_max)) {
                    lif_indx = lif;
                }
                else if ((lif >= range1_min) && (lif < range1_max)) {
                    lif_indx = lif + ARAD_PP_FRWRD_MACT_LIMIT_RANGE_MAP_SIZE;
                }
            }
            
            if (lif_indx < ARAD_PP_EM_SER_LEM_NOF_FIDS) {
                lem_fid_lif_counters[lif_indx]++;
            }
        }
    }

    
    for (indx = 0; indx < ARAD_PP_EM_SER_LEM_NOF_FIDS; ++indx) {
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

uint32 arad_pp_em_ser_fix_lem_fid_lif_counters(int unit, ARAD_PP_EM_SER_TYPE em_ser_type)
{
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if ((em_ser_type == ARAD_PP_EM_SER_TYPE_LEM_KEYT) && 
        (SOC_PPC_FRWRD_MACT_LEARN_LIMIT_MODE == SOC_PPC_FRWRD_MACT_LEARN_LIMIT_TYPE_VSI)) {
        res = arad_pp_em_ser_lem_fid_counters_update(unit);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }
    else if ((em_ser_type == ARAD_PP_EM_SER_TYPE_LEM_PLDT) && 
        (SOC_PPC_FRWRD_MACT_LEARN_LIMIT_MODE == SOC_PPC_FRWRD_MACT_LEARN_LIMIT_TYPE_LIF)) {
        res = arad_pp_em_ser_lem_lif_counters_update(unit);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);

}

uint32 arad_pp_em_ser_lem_fix_counters(int unit, ARAD_PP_EM_SER_TYPE em_ser_type)
{
    uint32 res = SOC_SAND_OK;
    int lem_counter_diff = 0;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (fix_global_counters) {
        
        res = arad_pp_em_ser_lem_counter_diff_get(unit, &lem_counter_diff);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        
        res = arad_pp_em_ser_mact_counter_fix(unit);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }

    if (fix_fid_counters) {
        
        res = arad_pp_em_ser_fix_lem_fid_lif_counters(unit, em_ser_type);
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    }

    
    res = arad_pp_em_ser_management_enable_set(unit, ARAD_PP_EM_SER_DB_TYPE_LEM, 1);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    
    if (lem_counter_diff > 0) { 
        res = arad_pp_em_lem_counter_increment(unit, lem_counter_diff);
        SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    }
    else if (lem_counter_diff < 0) {
        res = arad_pp_em_lem_counter_decrement(unit, lem_counter_diff*(-1));
        SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

uint32 arad_pp_em_ser_em_fix_counters(int unit, ARAD_PP_EM_SER_TYPE em_ser_type, ARAD_PP_SER_EM_TYPE_INFO* ser_info)
{
    uint32 res = SOC_SAND_OK;
    int em_counter_diff = 0;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (fix_global_counters) {
        
        res = arad_pp_em_ser_em_counter_diff_get(unit, ser_info, &em_counter_diff);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }

    
    res = arad_pp_em_ser_management_enable_set(unit, ser_info->db_type, 1);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    
    if (em_counter_diff > 0) { 
        res = arad_pp_em_em_counter_increment(unit, ser_info, em_counter_diff);
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    }
    else if (em_counter_diff < 0) {
        res = arad_pp_em_em_counter_decrement(unit, ser_info, em_counter_diff*(-1));
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

uint32 arad_pp_em_ser_align_shadow_and_hw_lem(int unit)
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

    ARAD_PP_LEM_ACCESS_PAYLOAD_clear(&payload);
    ARAD_PP_LEM_ACCESS_KEY_clear(&key);
    ARAD_PP_LEM_ACCESS_REQUEST_clear(&request);
    ARAD_PP_LEM_ACCESS_ACK_STATUS_clear(&ack);
    ARAD_PP_LEM_ACCESS_KEY_ENCODED_clear(&key_in_buffer);
    soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&block_range);
    SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE_clear(&rule);

    
    for(entry_index = 0; entry_index < ARAD_CHIP_SIM_LEM_TABLE_SIZE; entry_index++) {
        soc_sand_os_memset(key_out, 0x0, ARAD_CHIP_SIM_LEM_KEY) ;   
        soc_sand_os_memset(data_out, 0x0, ARAD_CHIP_SIM_LEM_PAYLOAD) ;

        res = soc_sand_exact_match_entry_get_by_index_unsafe(
              unit,
              ARAD_CHIP_SIM_LEM_BASE,
              entry_index,
              key_out,
              ARAD_CHIP_SIM_LEM_KEY,
              data_out,
              ARAD_CHIP_SIM_LEM_PAYLOAD,
              &is_valid
            );
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        if (is_valid) {
            
            is_found = 0;
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_IHP_MACT_DIAGNOSTICS_KEYr(unit, key_out));

            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHP_MACT_DIAGNOSTICSr, REG_PORT_ANY, 0, MACT_DIAGNOSTICS_LOOKUPf, 1));

            
                res = arad_polling(
                      unit,
                      ARAD_TIMEOUT,
                      ARAD_MIN_POLLS,
                      IHP_MACT_DIAGNOSTICSr,
                      REG_PORT_ANY,
                      0,
                      MACT_DIAGNOSTICS_LOOKUPf,
                      0
                    );
                SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
            
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,READ_PPDB_B_LARGE_EM_DIAGNOSTICS_LOOKUP_RESULTr(unit, &reg_val1));

            ARAD_FLD_FROM_REG64(IHP_MACT_DIAGNOSTICS_LOOKUP_RESULTr, MACT_ENTRY_FOUNDf, fld_val64, reg_val1, 50, exit);
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
    block_range.entries_to_act = ARAD_PP_EM_SER_LEM_TRVRS_ITER_BLK_SIZE;
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
                  ARAD_CHIP_SIM_LEM_BASE,
                  key_in_buffer.buffer,
                  ARAD_CHIP_SIM_LEM_KEY,
                  data_out,
                  ARAD_CHIP_SIM_LEM_PAYLOAD,
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

uint32 arad_pp_em_ser_align_shadow_and_hw_em(int unit, ARAD_PP_SER_EM_TYPE_INFO* ser_info)
{
    uint32 res = SOC_SAND_OK;
    uint8  is_valid, is_found;
    uint32 entry_index, temp, entry_is_valid;
    soc_reg_above_64_val_t reg_val, fld_val, read_result, key_data, key_out, data_out, buffer;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

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
            res = soc_reg_above_64_set(unit, ser_info->diagnostics_key_reg, REG_PORT_ANY, 0, key_out);
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ser_info->diagnostics_reg, REG_PORT_ANY, 0, ser_info->diagnostics_lookup_field, 1));

            
                res = arad_polling(
                      unit,
                      ARAD_TIMEOUT,
                      ARAD_MIN_POLLS,
                      ser_info->diagnostics_reg,
                      REG_PORT_ANY,
                      0,
                      ser_info->diagnostics_lookup_field,
                      0
                    );
                SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
            
            res= soc_reg_above_64_get(unit, ser_info->diagnostics_lookup_result_reg, REG_PORT_ANY, 0, reg_val);
            SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

            
            is_found = reg_val[0] & 1;

            if ((is_found) == FALSE) {
                
                
                temp = ARAD_PP_EM_SER_REQUEST_TYPE_INSERT_VAL;
                res = soc_sand_bitstream_set_any_field(
                      &temp,
                      ARAD_PP_EM_SER_REQUEST_TYPE_START_BIT,
                      ARAD_PP_EM_SER_REQUEST_TYPE_SIZE,
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

                
                res = soc_mem_write(unit, ser_info->req_mem, MEM_BLOCK_ALL, 0, buffer);
                SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
            }
        }
    }

    

    for (entry_index = 0; entry_index < ser_info->db_nof_lines; entry_index++) {
        
        res = soc_reg32_set(unit, ser_info->diagnostics_index_reg, REG_PORT_ANY, 0, entry_index);
        SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  110,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ser_info->diagnostics_reg, REG_PORT_ANY, 0, ser_info->diagnostics_read_field, 1));

        
        res = arad_polling(
              unit,
              ARAD_TIMEOUT,
              ARAD_MIN_POLLS,
              ser_info->diagnostics_reg,
              REG_PORT_ANY,
              0,
              ser_info->diagnostics_read_field,
              0
            );
        SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
        res = soc_reg_above_64_get(unit, ser_info->diagnostics_read_result_reg, REG_PORT_ANY, 0, read_result);
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
                

                
                temp = ARAD_PP_EM_SER_REQUEST_TYPE_DELETE_VAL;
                res = soc_sand_bitstream_set_any_field(
                      &temp,
                      ARAD_PP_EM_SER_REQUEST_TYPE_START_BIT,
                      ARAD_PP_EM_SER_REQUEST_TYPE_SIZE,
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

                
                res = soc_mem_write(unit, ser_info->req_mem, MEM_BLOCK_ALL, 0, buffer);
                SOC_SAND_CHECK_FUNC_RESULT(res, 190, exit);  

            }
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

uint32 arad_pp_em_ser_is_dynamic_get(uint32 unit, uint32 *key_data, uint32 *payload_data, uint8 *is_dynamic)
{
    ARAD_PP_LEM_ACCESS_KEY_ENCODED key_in_buffer;
    ARAD_PP_LEM_ACCESS_KEY key;
    uint32 entry_type;
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_PP_LEM_ACCESS_KEY_ENCODED_clear(&key_in_buffer);
    ARAD_PP_LEM_ACCESS_KEY_clear(&key);

    key_in_buffer.buffer[0] = key_data[0];
    key_in_buffer.buffer[1] = key_data[1];
    key_in_buffer.buffer[2] = key_data[2];

    res = arad_pp_lem_key_encoded_parse(
      unit,
      &key_in_buffer,
      &(key)
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    entry_type = arad_pp_lem_access_get_entry_type_from_key_unsafe(unit, &(key));

    res = arad_pp_lem_access_is_dynamic_get(unit, 
                                          payload_data, 
                                          entry_type, 
                                          is_dynamic);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

uint32 arad_pp_em_ser_is_dynamic_set(uint32 unit, uint32 *key_data, uint8 is_dynamic, uint32 *payload_data)
{
    ARAD_PP_LEM_ACCESS_KEY_ENCODED key_in_buffer;
    ARAD_PP_LEM_ACCESS_KEY key;
    uint32 entry_type;
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_PP_LEM_ACCESS_KEY_ENCODED_clear(&key_in_buffer);
    ARAD_PP_LEM_ACCESS_KEY_clear(&key);

    key_in_buffer.buffer[0] = key_data[0];
    key_in_buffer.buffer[1] = key_data[1];
    key_in_buffer.buffer[2] = key_data[2];

    res = arad_pp_lem_key_encoded_parse(
      unit,
      &key_in_buffer,
      &(key)
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    entry_type = arad_pp_lem_access_get_entry_type_from_key_unsafe(unit, &(key));

    res = arad_pp_lem_access_is_dynamic_set(unit, entry_type, is_dynamic, payload_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

uint32 arad_pp_em_ser_is_valid_get(int unit , soc_mem_t mem, uint32 entry_offset, uint32 array_index, uint32 *is_valid) {

    uint32 res = SOC_SAND_OK;
    soc_reg_above_64_val_t key_data;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_REG_ABOVE_64_CLEAR(key_data);

    
    res = soc_mem_array_read(unit,
                             mem,
                             array_index,
                             MEM_BLOCK_ANY,            
                             entry_offset,
                             key_data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

    
    res = soc_sand_bitstream_get_any_field(
          key_data,
          0, 
          1, 
          is_valid
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);


exit:
    SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void arad_pp_em_ser_lem_recover(void *p1, void *p2, void *p3, void *p4, void *p5)
{
    uint32 key_data[SOC_DPP_DEFS_MAX_LEM_WIDTH_IN_UINT32S] = {0},
           payload_data[ARAD_PP_LEM_ACCESS_PAYLOAD_IN_UINT32S] = {0};
    ARAD_PP_LEM_ACCESS_KEY_ENCODED key_in_buffer;
    ARAD_PP_LEM_ACCESS_KEY key;
    soc_reg_above_64_val_t reg_above_64_val;
    uint32 is_valid;
    uint8 is_dynamic = 0, is_found;
    int unit = *(int*)p1;
    ARAD_PP_EM_SER_TYPE em_ser_type = *(ARAD_PP_EM_SER_TYPE*)p2;
    uint32 entry_offset = *(uint32*)p3;
    uint32 is_ser_exist = *(uint32*)p4;
    uint32 array_index = *(uint32*)p5;
    uint32 entry_location;
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    ARAD_PP_LEM_ACCESS_KEY_ENCODED_clear(&key_in_buffer);
    ARAD_PP_LEM_ACCESS_KEY_clear(&key);
    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);

    if (!is_ser_exist) {
        
        res = arad_pp_em_ser_lem_fix_counters(unit, em_ser_type);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        
        res = arad_pp_em_ser_align_shadow_and_hw_lem(unit);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }
    else {

        
        entry_location = array_index * (ARAD_PP_EM_SER_LEM_NOF_LINES/ARAD_PP_EM_SER_NOF_HASH_RESULTS) + entry_offset;
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, WRITE_IHP_MACT_DIAGNOSTICS_INDEXr(unit, entry_location));
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IHP_MACT_DIAGNOSTICSr, REG_PORT_ANY, 0, MACT_DIAGNOSTICS_READf, 1));

        
            res = arad_polling(
                  unit,
                  ARAD_TIMEOUT,
                  ARAD_MIN_POLLS,
                  IHP_MACT_DIAGNOSTICSr,
                  REG_PORT_ANY,
                  0,
                  MACT_DIAGNOSTICS_READf,
                  0
                );
            SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

        SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, READ_IHP_MACT_DIAGNOSTICS_READ_RESULTr(unit, reg_above_64_val));

        
        res = soc_sand_bitstream_get_any_field(
              reg_above_64_val,
              1, 
              74, 
              key_data
            );
        SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

        if (em_ser_type == ARAD_PP_EM_SER_TYPE_LEM_KEYT) {
            
            res = soc_mem_array_read(unit,
                                     IHP_MEM_590000m,
                                     array_index,
                                     MEM_BLOCK_ANY,             
                                     entry_offset,
                                     payload_data);
            SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

            arad_pp_em_ser_is_dynamic_get(unit, key_data, payload_data, &is_dynamic);

            key_data[0] = 0;
            key_data[1] = 0;
            key_data[2] = 0;
            
            
            res = soc_mem_array_write(unit,
                                      IHP_MEM_500000m,
                                      array_index,
                                      MEM_BLOCK_ANY,             
                                      entry_offset,
                                      key_data);
            SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

            
            res = arad_pp_em_ser_lem_fix_counters(unit, em_ser_type);
            SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

            if (!is_dynamic) {

                
                res = arad_pp_em_ser_align_shadow_and_hw_lem(unit);
                SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
            }
        }
        else if (em_ser_type == ARAD_PP_EM_SER_TYPE_LEM_PLDT){

            
            res = arad_pp_em_ser_is_valid_get(unit, IHP_MEM_500000m, entry_offset, array_index, &is_valid);
            SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);

            if (is_valid) {
                is_found = 0;
                
                res = chip_sim_exact_match_entry_get_unsafe(
                      unit,
                      ARAD_CHIP_SIM_LEM_BASE,
                      key_data,
                      ARAD_CHIP_SIM_LEM_KEY,
                      payload_data,
                      ARAD_CHIP_SIM_LEM_PAYLOAD,
                      &is_found
                    );
                SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);

                if (is_found) {
                    
                    res = soc_mem_array_write(unit,
                                              IHP_MEM_590000m,
                                              array_index,
                                              MEM_BLOCK_ANY,             
                                              entry_offset,
                                              payload_data);
                    SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);

                    
                    res = arad_pp_em_ser_management_enable_set(unit, ARAD_PP_EM_SER_DB_TYPE_LEM, 1);
                    SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

                }
                else { 

                    
                    res = soc_mem_array_read(unit,
                                             IHP_MEM_590000m,
                                             array_index,
                                             MEM_BLOCK_ANY,             
                                             entry_offset,
                                             payload_data);
                    SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);

                    res = arad_pp_em_ser_is_dynamic_get(unit, key_data, payload_data, &is_dynamic);
                    SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);

                    if (!is_dynamic) {
                        
                        res = arad_pp_em_ser_is_dynamic_set(unit, key_data, 1, payload_data);
                        SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);

                        
                        res = soc_mem_array_write(unit,
                                                  IHP_MEM_590000m,
                                                  array_index,
                                                  MEM_BLOCK_ANY,             
                                                  entry_offset,
                                                  payload_data);
                        SOC_SAND_CHECK_FUNC_RESULT(res, 190, exit);

                        
                        res = arad_pp_em_ser_management_enable_set(unit, ARAD_PP_EM_SER_DB_TYPE_LEM, 1);
                        SOC_SAND_CHECK_FUNC_RESULT(res, 195, exit);
                    }
                    else { 
                        key_data[0] = 0;
                        key_data[1] = 0;
                        key_data[2] = 0;
                        
                        res = soc_mem_array_write(unit,
                                                  IHP_MEM_500000m,
                                                  array_index,
                                                  MEM_BLOCK_ANY,             
                                                  entry_offset,
                                                  key_data);
                        SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);

                        payload_data[0] = 0;
                        payload_data[1] = 0;

                        
                        res = soc_mem_array_write(unit,
                                                  IHP_MEM_590000m,
                                                  array_index,
                                                  MEM_BLOCK_ANY,             
                                                  entry_offset,
                                                  payload_data);
                        SOC_SAND_CHECK_FUNC_RESULT(res, 210, exit);

                        
                        res = arad_pp_em_ser_lem_fix_counters(unit, em_ser_type);
                        SOC_SAND_CHECK_FUNC_RESULT(res, 220, exit);
                    }
                }
            }
            else { 

                key_data[0] = 0;
                key_data[1] = 0;
                key_data[2] = 0;
                
                res = soc_mem_array_write(unit,
                                          IHP_MEM_500000m,
                                          array_index,
                                          MEM_BLOCK_ANY,             
                                          entry_offset,
                                          key_data);
                SOC_SAND_CHECK_FUNC_RESULT(res, 230, exit);

                
                res = soc_mem_array_write(unit,
                                          IHP_MEM_590000m,
                                          array_index,
                                          MEM_BLOCK_ANY,             
                                          entry_offset,
                                          payload_data);
                SOC_SAND_CHECK_FUNC_RESULT(res, 240, exit);

                
                res = arad_pp_em_ser_management_enable_set(unit, ARAD_PP_EM_SER_DB_TYPE_LEM, 1);
                SOC_SAND_CHECK_FUNC_RESULT(res, 250, exit);
            }
        }
    }

exit:
        
    arad_pp_em_ser_mask_interrupt_set(unit, em_ser_type, 1);
}

uint32 arad_pp_ser_em_keyt_mem_get(int unit, ARAD_PP_EM_SER_TYPE em_ser_type ,soc_mem_t *mem) 
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    switch (em_ser_type) {
    case ARAD_PP_EM_SER_TYPE_ISA_KEYT:
    case ARAD_PP_EM_SER_TYPE_ISA_PLDT:
        *mem = IHP_MEM_7A0000m;
        break;

    case ARAD_PP_EM_SER_TYPE_ISB_KEYT:
    case ARAD_PP_EM_SER_TYPE_ISB_PLDT:
        *mem = IHP_MEM_A10000m;
        break;

    case ARAD_PP_EM_SER_TYPE_ESEM_KEYT:
    case ARAD_PP_EM_SER_TYPE_ESEM_PLDT:
        *mem = EPNI_MEM_520000m;
        break;

    case ARAD_PP_EM_SER_TYPE_OEMA_KEYT:
    case ARAD_PP_EM_SER_TYPE_OEMA_PLDT:
        *mem = IHB_MEM_EA0000m;
        break;

    case ARAD_PP_EM_SER_TYPE_OEMB_KEYT:
    case ARAD_PP_EM_SER_TYPE_OEMB_PLDT:
        *mem = IHB_MEM_14A0000m;
        break;

    case ARAD_PP_EM_SER_TYPE_RMAPEM_KEYT:
    case ARAD_PP_EM_SER_TYPE_RMAPEM_PLDT:
        *mem = OAMP_MEM_100000m;
        break;

    default:
        break;
    }

    SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void arad_pp_em_ser_em_recover(void *p1, void *p2, void *p3, void *p4, void *p5)
{
    int unit = *(int*)p1;
    ARAD_PP_EM_SER_TYPE em_ser_type = *(ARAD_PP_EM_SER_TYPE*)p2;
    uint32 entry_location = *(uint32*)p3;
    uint32 is_valid, array_index, entry_offset;
    uint8 is_found;
    soc_reg_above_64_val_t key_data, payload_data, read_result;
    ARAD_PP_SER_EM_TYPE_INFO ser_info;
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_REG_ABOVE_64_CLEAR(key_data);
    SOC_REG_ABOVE_64_CLEAR(payload_data);
    SOC_REG_ABOVE_64_CLEAR(read_result);
    sal_memset(&ser_info, 0, sizeof(ARAD_PP_SER_EM_TYPE_INFO));

    res = arad_pp_ser_em_type_info_get(unit, em_ser_type, &ser_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    entry_offset = (entry_location) % (ser_info.db_nof_lines/ARAD_PP_EM_SER_NOF_HASH_RESULTS);
    array_index  = (entry_location) / (ser_info.db_nof_lines/ARAD_PP_EM_SER_NOF_HASH_RESULTS);

    if ((em_ser_type == ARAD_PP_EM_SER_TYPE_ISA_KEYT) ||
        (em_ser_type == ARAD_PP_EM_SER_TYPE_ISB_KEYT) ||
        (em_ser_type == ARAD_PP_EM_SER_TYPE_ESEM_KEYT) ||
        (em_ser_type == ARAD_PP_EM_SER_TYPE_OEMA_KEYT) ||
        (em_ser_type == ARAD_PP_EM_SER_TYPE_OEMB_KEYT) ||
        (em_ser_type == ARAD_PP_EM_SER_TYPE_RMAPEM_KEYT)) 
    {
        
        
        res = soc_mem_array_write(unit,
                                  ser_info.keyt_mem,
                                  array_index,
                                  MEM_BLOCK_ANY,             
                                  entry_offset,
                                  key_data);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

        
        res = arad_pp_em_ser_em_fix_counters(unit, em_ser_type, &ser_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

        
        res = arad_pp_em_ser_align_shadow_and_hw_em(unit, &ser_info);
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }
    else { 

        res = arad_pp_em_ser_is_valid_get(unit, ser_info.keyt_mem, entry_offset, array_index, &is_valid);
        SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

        if (is_valid) {
            
            res = soc_reg32_set(unit, ser_info.diagnostics_index_reg, REG_PORT_ANY, 0, entry_location);
            SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ser_info.diagnostics_reg, REG_PORT_ANY, 0, ser_info.diagnostics_read_field, 1));

            
                res = arad_polling(
                      unit,
                      ARAD_TIMEOUT,
                      ARAD_MIN_POLLS,
                      ser_info.diagnostics_reg,
                      REG_PORT_ANY,
                      0,
                      ser_info.diagnostics_read_field,
                      0
                    );
                SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
            res = soc_reg_above_64_get(unit, ser_info.diagnostics_read_result_reg, REG_PORT_ANY, 0, read_result);
            SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

            
            res = soc_sand_bitstream_get_any_field(
                  read_result,
                  1, 
                  ser_info.key_size, 
                  key_data
                );
            SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

            is_found = 0;
            
            res = chip_sim_exact_match_entry_get_unsafe(
                  unit,
                  ser_info.shadow_base,
                  key_data,
                  ser_info.shadow_key_size,
                  payload_data,
                  ser_info.shadow_payload_size,
                  &is_found
                );
            SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

            if (is_found) {
                
                res = soc_mem_array_write(unit,
                                          ser_info.pldt_mem,
                                          array_index,
                                          MEM_BLOCK_ANY,             
                                          entry_offset,
                                          payload_data);
                SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
            }
            else {
                
            }
        }
        else {
            
            res = soc_mem_array_write(unit,
                                      ser_info.pldt_mem,
                                      array_index,
                                      MEM_BLOCK_ANY,            
                                      entry_offset,
                                      payload_data);
            SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
        }

        
        res = arad_pp_em_ser_management_enable_set(unit, ser_info.db_type, 1);
        SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);
    }
exit:
        
    arad_pp_em_ser_mask_interrupt_set(unit, em_ser_type, 1);
}

int arad_pp_em_ser_lem(int unit, soc_mem_t mem, uint32 base_addr, uint32 addr, ARAD_PP_EM_SER_TYPE em_ser_type)
{
    uint32 entry_offset, counter, counter_overflow, address_valid, address, array_index;
    uint32 data[2];
    uint32 res = SOC_SAND_OK;
    uint32 is_ser_exist;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    res = arad_pp_em_ser_management_enable_set(unit, ARAD_PP_EM_SER_DB_TYPE_LEM, 0);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    
    entry_offset = (addr - base_addr) % (ARAD_PP_EM_SER_LEM_NOF_LINES/ARAD_PP_EM_SER_NOF_HASH_RESULTS);
    array_index  = (addr - base_addr) / (ARAD_PP_EM_SER_LEM_NOF_LINES/ARAD_PP_EM_SER_NOF_HASH_RESULTS);
    res = soc_mem_array_read(unit,
                             mem,
                             array_index,
                             MEM_BLOCK_ANY,            
                             entry_offset,
                             data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    
	res = arad_pp_em_ser_parity_error_info_get(unit, ARAD_PP_EM_SER_BLOCK_IHP, &address, &address_valid, &counter, &counter_overflow);
 	SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

    if ((counter > 0) && (address == addr)) {
        
        is_ser_exist = 1;
    }
    else {
        
        is_ser_exist = 0;
    }
    
        
    p1 = unit;
    p2 = em_ser_type;
    p3 = entry_offset;
    p4 = is_ser_exist;
    p5 = array_index;

    
    sal_dpc(arad_pp_em_ser_lem_recover, (void*)&p1, (void*)&p2, (void*)&p3, (void*)&p4, (void*)&p5);

    
    arad_pp_em_ser_mask_interrupt_set(unit, em_ser_type, 0);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}


int arad_pp_em_ser_em(int unit, soc_mem_t mem, uint32 base_addr, uint32 addr, ARAD_PP_EM_SER_TYPE em_ser_type)
{
    uint32 entry_offset;
    ARAD_PP_SER_EM_TYPE_INFO ser_info;
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = arad_pp_ser_em_type_info_get(unit, em_ser_type, &ser_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    
    res = arad_pp_em_ser_management_enable_set(unit, ser_info.db_type, 0);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
 
    entry_offset = addr - base_addr;
       
        
    p1 = unit;
    p2 = em_ser_type;
    p3 = entry_offset;

    
    sal_dpc(arad_pp_em_ser_em_recover, (void*)&p1, (void*)&p2, (void*)&p3, NULL, NULL);

    
    arad_pp_em_ser_mask_interrupt_set(unit, em_ser_type, 0);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_em_ser_em()", 0, 0);
}


int arad_pp_em_ser(int unit, uint32* addr, ARAD_PP_EM_SER_BLOCK block)
{
    uint32 base_addr;
    uint32 res = SOC_SAND_OK;
    ARAD_PP_EM_SER_TYPE em_ser_type;
    soc_mem_t mem;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(addr);

    res = arad_pp_em_ser_type_get(unit, *addr, block, &em_ser_type, &mem, &base_addr);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if ((em_ser_type == ARAD_PP_EM_SER_TYPE_LEM_KEYT) || (em_ser_type == ARAD_PP_EM_SER_TYPE_LEM_PLDT)) {
        
        if (SOC_DPP_IS_LEM_SIM_ENABLE(unit)) {
            
            res = arad_pp_em_ser_lem(unit, mem, base_addr, *addr, em_ser_type);
            SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
        }
    }
    else if (em_ser_type != ARAD_PP_EM_SER_TYPE_LAST) {
        
        res = arad_pp_em_ser_em(unit, mem, base_addr, *addr, em_ser_type);
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    }
     
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR(0, 0, 0);
}


#include <soc/dpp/SAND/Utils/sand_footer.h>


