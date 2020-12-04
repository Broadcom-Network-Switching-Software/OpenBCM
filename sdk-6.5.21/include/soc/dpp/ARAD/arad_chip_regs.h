/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_CHIP_REGS_INCLUDED__

#define __ARAD_CHIP_REGS_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/register.h>
#include <soc/memory.h>






#define  ARAD_NOF_STL_GROUP_CONFIG_REGS          6
#define  ARAD_NOF_STL_GROUP_CONFIG_FLDS          16
#define  ARAD_FAST_REGISTERS_AND_FIELDS_ACCESS

typedef enum
{
  ARAD_REG_NIF_DIRECTION_RX = 0,
  ARAD_REG_NIF_DIRECTION_TX = 1,
  ARAD_REG_NOF_NIF_DIRECTIONS
}ARAD_REG_NIF_DIRECTION;


#define  ARAD_EGQ_CNM_CPID_TO_FC_TYPE_REG_NOF_FLDS 3

#define ARAD_TRANSMIT_DATA_QUEUE_NOF_REGS      7
#define ARAD_REGS_DBUFF_PTR_Q_THRESH_NOF_REGS  8


#define  ARAD_NOF_BIT_PER_LINK_REGS              2

#define ARAD_TIMEOUT                            (20*1000)
#define ARAD_MIN_POLLS                          (100)


#define  ARAD_DRAM_NDX_MAX                      (7)
















uint32
  arad_polling(
    SOC_SAND_IN int   unit,
    SOC_SAND_IN sal_usecs_t time_out,
    SOC_SAND_IN int32    min_polls,
    SOC_SAND_IN soc_reg_t   reg,
    SOC_SAND_IN int32    port,
    SOC_SAND_IN int32    index,
    SOC_SAND_IN soc_field_t field,
    SOC_SAND_IN uint32    expected_value
  );


uint32
  soc_dpp_polling(
    int         unit,
    sal_usecs_t time_out,
    int32       min_polls,
    soc_reg_t   reg,
    int32       port,
    int32       index,
    soc_field_t field,
    uint32      expected_value
  );


uint32
  arad_mem_polling(
    SOC_SAND_IN int   unit,
    SOC_SAND_IN int   core_block,
    SOC_SAND_IN sal_usecs_t time_out,
    SOC_SAND_IN int32    min_polls,
    SOC_SAND_IN soc_mem_t mem,
    SOC_SAND_IN uint32  buff_off,
    SOC_SAND_IN uint32   array_type,
    SOC_SAND_IN soc_field_t field,
    SOC_SAND_IN uint32    expected_value
  );


 


typedef struct
{
  soc_reg_t reg;
  int       reg_addr;
  int       block;
  int       index;
  uint8     at;
} ARAD_FAST_REG_CALL_INFO;


typedef struct
{
  uint16        bp;
  uint16        len;
  soc_field_t   field;
  soc_reg_t     reg;
} ARAD_FAST_FIELD_CALL_INFO;


typedef struct
{
  int            bp;
  int            len;
  soc_field_t    field;
  uint32         flags;
  soc_mem_t      mem;
  soc_mem_info_t *meminfo;
} ARAD_FAST_MEMORY_CALL_INFO;



typedef enum arad_fast_regs_type_e
{
    ARAD_FAST_REG_IHP_MACT_CPU_REQUEST_REQUEST,
    ARAD_FAST_REG_IHP_MACT_REPLY_FIFO_REPLY_FIFO_ENTRY_COUNT,
    ARAD_FAST_REG_IHP_MACT_REPLY,
    ARAD_FAST_REG_IHP_MACT_CPU_REQUEST_TRIGGER,
    ARAD_FAST_REG_IHP_ISA_INTERRUPT_REGISTER,
    ARAD_FAST_REG_IHP_ISB_INTERRUPT_REGISTER,
    ARAD_FAST_REG_IHP_ISA_MANAGEMENT_UNIT_FAILURE,
    ARAD_FAST_REG_IHP_ISB_MANAGEMENT_UNIT_FAILURE,
    
    
    ARAD_FAST_REG_OAMP_IPV4_SRC_ADDR_SELECT, 
    ARAD_FAST_REG_OAMP_IPV4_TOS_TTL_SELECT, 
    ARAD_FAST_REG_OAMP_BFD_TX_RATE , 
    ARAD_FAST_REG_OAMP_BFD_REQ_INTERVAL_POINTER , 
    ARAD_FAST_REG_OAMP_MPLS_PWE_PROFILE , 
    ARAD_FAST_REG_OAMP_BFD_TX_IPV4_MULTI_HOP , 
    ARAD_FAST_REG_OAMP_PR_2_FW_DTC , 

    ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_WORD,
    ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_WORD,
    ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_DATA_0,
    ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_DATA_1,
    ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_DATA_2,
    ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_DATA_3,
    ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_DATA_4,
    ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_DATA_5,
    ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_DATA_6,
    ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_DATA_7,
    ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_DATA_8,
    ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_DATA_9,
    ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_DATA_10,
    ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_DATA_11,
    ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_DATA_12,
    ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_DATA_13,
    ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_DATA_14,
    ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_0,
    ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_1,
    ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_2,
    ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_3,
    ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_4,
    ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_5,
    ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_6,
    ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_7,
    ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_8,
    ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_9,
    ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_10,
    ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_11,
    ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_12,
    ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_13,
    ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_14,

    ARAD_FAST_REG_TYPE_LAST		    
        
} arad_fast_regs_type_t;


typedef enum arad_fast_field_type_e
{
    ARAD_FAST_FIELD_IHP_MACT_CPU_REQUEST_REQUEST_MACT_REQ_QUALIFIER,
    ARAD_FAST_FIELD_IHP_MACT_CPU_REQUEST_REQUEST_MACT_REQ_SELF,
    ARAD_FAST_FIELD_IHP_MACT_CPU_REQUEST_REQUEST_MACT_REQ_AGE_PAYLOAD,
    ARAD_FAST_FIELD_IHP_MACT_CPU_REQUEST_REQUEST_MACT_REQ_PAYLOAD_IS_DYNAMIC,
    ARAD_FAST_FIELD_IHP_MACT_CPU_REQUEST_REQUEST_MACT_REQ_PAYLOAD,
    ARAD_FAST_FIELD_IHP_MACT_CPU_REQUEST_REQUEST_MACT_REQ_STAMP,
    ARAD_FAST_FIELD_IHP_MACT_CPU_REQUEST_REQUEST_MACT_REQ_COMMAND,
    ARAD_FAST_FIELD_IHP_MACT_CPU_REQUEST_REQUEST_MACT_REQ_PART_OF_LAG,
    ARAD_FAST_FIELD_IHP_MACT_CPU_REQUEST_REQUEST_MACT_REQ_MFF_IS_KEY,
    ARAD_FAST_FIELD_IHP_MACT_CPU_REQUEST_REQUEST_MACT_REQ_MFF_KEY,
    ARAD_FAST_FIELD_IHP_MACT_CPU_REQUEST_REQUEST_MACT_REQ_SUCCESS,
    ARAD_FAST_FIELD_IHP_MACT_CPU_REQUEST_REQUEST_MACT_REQ_REASON,
    ARAD_FAST_FIELD_IHP_ISA_INTERRUPT_REGISTER_ONE_ISA_MANAGEMENT_COMPLETED,
    ARAD_FAST_FIELD_IHP_ISB_INTERRUPT_REGISTER_ONE_ISB_MANAGEMENT_COMPLETED,
    ARAD_FAST_FIELD_IHP_ISA_MANAGEMENT_UNIT_FAILURE_ISA_MNGMNT_UNIT_FAILURE_VALID,
    ARAD_FAST_FIELD_IHP_ISB_MANAGEMENT_UNIT_FAILURE_ISB_MNGMNT_UNIT_FAILURE_VALID,
    ARAD_FAST_FIELD_IHP_ISA_MANAGEMENT_UNIT_FAILURE_ISA_MNGMNT_UNIT_FAILURE_REASON,
    ARAD_FAST_FIELD_IHP_ISB_MANAGEMENT_UNIT_FAILURE_ISB_MNGMNT_UNIT_FAILURE_REASON,

    ARAD_FAST_FIELD_IHP_KAPS_IBC_COMMAND_WORD_BLKID,
    ARAD_FAST_FIELD_IHP_KAPS_IBC_COMMAND_WORD_CMD,
    ARAD_FAST_FIELD_IHP_KAPS_IBC_COMMAND_WORD_FUNC,
    ARAD_FAST_FIELD_IHP_KAPS_IBC_COMMAND_WORD_OFFSET,
    ARAD_FAST_FIELD_IHP_KAPS_IBC_RESPONSE_WORD_BLKID,
    ARAD_FAST_FIELD_IHP_KAPS_IBC_RESPONSE_WORD_RSP,
    ARAD_FAST_FIELD_IHP_KAPS_IBC_RESPONSE_WORD_FUNC,
    ARAD_FAST_FIELD_IHP_KAPS_IBC_RESPONSE_WORD_STATUS,

    ARAD_FAST_FIELD_TYPE_LAST		    
        
} arad_fast_field_type_t;

typedef enum arad_fast_mem_field_type_e
{    
    ARAD_FAST_MEM_TYPE_IHP_PP_PORT_INFO_PP_PORT_OUTER_HEADER_START,
    ARAD_FAST_MEM_TYPE_LAST		    
        
} arad_fast_mem_field_type_t;




extern ARAD_FAST_REG_CALL_INFO              g_fast_reg_info_table[BCM_MAX_NUM_UNITS][ARAD_FAST_REG_TYPE_LAST];
extern ARAD_FAST_FIELD_CALL_INFO            g_fast_field_info_table[BCM_MAX_NUM_UNITS][ARAD_FAST_FIELD_TYPE_LAST];
extern ARAD_FAST_MEMORY_CALL_INFO           g_fast_mem_info_table[BCM_MAX_NUM_UNITS][ARAD_FAST_MEM_TYPE_LAST];



int arad_fast_reg_get(int unit, soc_reg_t reg, int acc_type, int addr, int block, soc_reg_above_64_val_t data);
int arad_fast_reg_set(int unit, soc_reg_t reg, int acc_type, int addr, int block, soc_reg_above_64_val_t data);

void    arad_fast_mem_field_set(int bp, int len,uint32 flags, soc_mem_info_t *meminfo, uint32 *entbuf, uint32 *fldbuf);
uint32* arad_fast_mem_field_get(int bp, int len, uint32 flags, soc_mem_info_t *meminfo, const uint32 *entbuf, uint32 *fldbuf);

void arad_fast_regs_and_fields_access_init(int unit);



#define ARAD_FAST_FIELD_GET(field_id, reg_val, fld_value)       SOC_REG_ABOVE_64_CLEAR(fld_value);\
                                                                SHR_BITCOPY_RANGE(fld_value, 0, reg_val, g_fast_field_info_table[unit][field_id].bp,g_fast_field_info_table[unit][field_id].len);

#define ARAD_FAST_FIELD_SET(field_id, reg_val, fld_value)       SHR_BITCOPY_RANGE(reg_val, g_fast_field_info_table[unit][field_id].bp, fld_value, 0, g_fast_field_info_table[unit][field_id].len);


#define ARAD_FAST_REGISER_SET(reg_id, reg_val)                  arad_fast_reg_set(unit, g_fast_reg_info_table[unit][reg_id].reg, g_fast_reg_info_table[unit][reg_id].at,\
                                                                g_fast_reg_info_table[unit][reg_id].reg_addr, g_fast_reg_info_table[unit][reg_id].block, reg_val)

#define ARAD_FAST_REGISER_GET(reg_id, reg_val)                  arad_fast_reg_get(unit, g_fast_reg_info_table[unit][reg_id].reg, g_fast_reg_info_table[unit][reg_id].at,\
                                                                g_fast_reg_info_table[unit][reg_id].reg_addr, g_fast_reg_info_table[unit][reg_id].block, reg_val)



#define ARAD_FAST_MEM_FIELD_SET(mem_fld_id, mem_val, fld_val)   arad_fast_mem_field_set(g_fast_mem_info_table[unit][mem_fld_id].bp, g_fast_mem_info_table[unit][mem_fld_id].len,\
                                                                g_fast_mem_info_table[unit][mem_fld_id].flags, g_fast_mem_info_table[unit][mem_fld_id].meminfo, mem_val, &fld_val);

#define ARAD_FAST_MEM_FIELD_GET(mem_fld_id, mem_val, fld_val)   arad_fast_mem_field_get(g_fast_mem_info_table[unit][mem_fld_id].bp, g_fast_mem_info_table[unit][mem_fld_id].len,\
                                                                g_fast_mem_info_table[unit][mem_fld_id].flags, g_fast_mem_info_table[unit][mem_fld_id].meminfo, mem_val, fld_val);

#include <soc/dpp/SAND/Utils/sand_footer.h>
#endif

