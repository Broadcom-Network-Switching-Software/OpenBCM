/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_API_DRAM_INCLUDED__

#define __ARAD_API_DRAM_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/TMC/tmc_api_dram.h>
#include <soc/dpp/ARAD/arad_framework.h>




#define ARAD_DRAM_VAL_LSB                                ( 0)
#define ARAD_DRAM_VAL_MSB                                (30)
#define ARAD_DRAM_VAL_SHIFT                              (ARAD_DRAM_VAL_LSB)
#define ARAD_DRAM_VAL_MASK                               (SOC_SAND_BITS_MASK(ARAD_DRAM_VAL_MSB, ARAD_DRAM_VAL_LSB))

#define ARAD_DRAM_VAL_IS_IN_CLOCKS_LSB                   (31)
#define ARAD_DRAM_VAL_IS_IN_CLOCKS_MSB                   (31)
#define ARAD_DRAM_VAL_IS_IN_CLOCKS_SHIFT                 (ARAD_DRAM_VAL_IS_IN_CLOCKS_LSB)
#define ARAD_DRAM_VAL_IS_IN_CLOCKS_MASK                  (SOC_SAND_BITS_MASK(ARAD_DRAM_VAL_IS_IN_CLOCKS_MSB, ARAD_DRAM_VAL_IS_IN_CLOCKS_LSB))


#define  ARAD_DRAM_PATTERN_SIZE_IN_UINT32S (8)

#define ARAD_DRAM_TYPE_DDR3                                SOC_TMC_DRAM_TYPE_DDR3
#define ARAD_DRAM_NOF_TYPES                                SOC_TMC_DRAM_NOF_TYPES
typedef SOC_TMC_DRAM_TYPE                                      ARAD_DRAM_TYPE;

#define ARAD_DRAM_NUM_COLUMNS_256                          SOC_TMC_DRAM_NUM_COLUMNS_256
#define ARAD_DRAM_NUM_COLUMNS_512                          SOC_TMC_DRAM_NUM_COLUMNS_512
#define ARAD_DRAM_NUM_COLUMNS_1024                         SOC_TMC_DRAM_NUM_COLUMNS_1024
#define ARAD_DRAM_NUM_COLUMNS_2048                         SOC_TMC_DRAM_NUM_COLUMNS_2048
#define ARAD_DRAM_NUM_COLUMNS_4096                         SOC_TMC_DRAM_NUM_COLUMNS_4096
#define ARAD_DRAM_NUM_COLUMNS_8192                         SOC_TMC_DRAM_NUM_COLUMNS_8192
#define ARAD_NOF_DRAM_NUMS_COLUMNS                         SOC_TMC_NOF_DRAM_NUMS_COLUMNS
typedef SOC_TMC_DRAM_NUM_COLUMNS                               ARAD_DRAM_NUM_COLUMNS;                              

#define ARAD_DRAM_NUM_ROWS_8192                          SOC_TMC_DRAM_NUM_ROWS_8192
#define ARAD_DRAM_NUM_ROWS_16384                        SOC_TMC_DRAM_NUM_ROWS_16384
#define ARAD_NOF_DRAM_NUMS_ROWS                         SOC_TMC_NOF_DRAM_NUMS_ROWS
typedef SOC_TMC_DRAM_NUM_ROWS                                     ARAD_DRAM_NUM_ROWS;

#define ARAD_DRAM_NUM_BANKS_4                              SOC_TMC_DRAM_NUM_BANKS_4
#define ARAD_DRAM_NUM_BANKS_8                              SOC_TMC_DRAM_NUM_BANKS_8
#define ARAD_NOF_DRAM_NUM_BANKS                            SOC_TMC_NOF_DRAM_NUM_BANKS
typedef SOC_TMC_DRAM_NUM_BANKS                                 ARAD_DRAM_NUM_BANKS;

#define ARAD_DRAM_AP_POSITION_08                           SOC_TMC_DRAM_AP_POSITION_08
#define ARAD_DRAM_AP_POSITION_09                           SOC_TMC_DRAM_AP_POSITION_09
#define ARAD_DRAM_AP_POSITION_10                           SOC_TMC_DRAM_AP_POSITION_10
#define ARAD_DRAM_AP_POSITION_11                           SOC_TMC_DRAM_AP_POSITION_11
#define ARAD_DRAM_AP_POSITION_12                           SOC_TMC_DRAM_AP_POSITION_12
#define ARAD_NOF_DRAM_AP_POSITIONS                         SOC_TMC_NOF_DRAM_AP_POSITIONS
typedef SOC_TMC_DRAM_AP_POSITION                               ARAD_DRAM_AP_POSITION;

#define ARAD_DRAM_BURST_SIZE_16                            SOC_TMC_DRAM_BURST_SIZE_16
#define ARAD_DRAM_BURST_SIZE_32                            SOC_TMC_DRAM_BURST_SIZE_32
#define ARAD_DRAM_NOF_BURST_SIZES                          SOC_TMC_DRAM_NOF_BURST_SIZES
typedef SOC_TMC_DRAM_BURST_SIZE                                ARAD_DRAM_BURST_SIZE;

#define ARAD_DDR_CLAM_SHELL_MODE_DISABLED                  SOC_TMC_DRAM_CLAM_SHELL_MODE_DISABLED
#define ARAD_DDR_CLAM_SHELL_MODE_DRAM_0                    SOC_TMC_DRAM_CLAM_SHELL_MODE_DRAM_0
#define ARAD_DDR_CLAM_SHELL_MODE_DRAM_1                    SOC_TMC_DRAM_CLAM_SHELL_MODE_DRAM_1
#define ARAD_DDR_NOF_CLAM_SHELL_MODES                      SOC_TMC_NOF_DRAM_CLAM_SHELL_MODE
typedef SOC_TMC_DRAM_CLAM_SHELL_MODE                       ARAD_DDR_CLAM_SHELL_MODE;


#define ARAD_DRAM_BIST_NOF_PATTERNS (8)


#define ARAD_DRAM_BIST_CONS_ADDR_8_BANKS            SOC_TMC_DRAM_BIST_FLAGS_CONS_ADDR_8_BANKS 
#define ARAD_DRAM_BIST_ADDRESS_SHIFT_MODE           SOC_TMC_DRAM_BIST_FLAGS_ADDRESS_SHIFT_MODE
#define ARAD_DRAM_BIST_INFINITE                     SOC_TMC_DRAM_BIST_FLAGS_INFINITE 
#define ARAD_DRAM_BIST_ALL_ADDRESS                  SOC_TMC_DRAM_BIST_FLAGS_ALL_ADDRESS
#define ARAD_DRAM_BIST_STOP                         SOC_TMC_DRAM_BIST_FLAGS_STOP
#define ARAD_DRAM_BIST_GET_DATA                     SOC_TMC_DRAM_BIST_FLAGS_GET_DATA 
#define ARAD_DRAM_BIST_TWO_ADDRESS_MODE             SOC_TMC_DRAM_BIST_FLAGS_TWO_ADDRESS_MODE
#define ARAD_DRAM_BIST_BG_INTERLEAVE                SOC_TMC_DRAM_BIST_FLAGS_BG_INTERLEAVE
#define ARAD_DRAM_BIST_SINGLE_BANK_TEST             SOC_TMC_DRAM_BIST_FLAGS_SINGLE_BANK_TEST

#define ARAD_DRAM_BUF_NUM_ALL 0xffffffff
#define ARAD_DRAM_MAX_BUFFERS_IN_ERROR_CNTR 1000






#define ARAD_DRAM_VAL_IN_CLOCKS(val)                 \
          (SOC_SAND_SET_FLD_IN_PLACE(val, ARAD_DRAM_VAL_SHIFT, ARAD_DRAM_VAL_MASK)) | \
          (SOC_SAND_SET_FLD_IN_PLACE(0x1, ARAD_DRAM_VAL_IS_IN_CLOCKS_SHIFT, ARAD_DRAM_VAL_IS_IN_CLOCKS_MASK))






typedef struct arad_api_dram_ddr3_s{

    SOC_SAND_MAGIC_NUM_VAR
    
    uint32 mrs0_wr1;
    
    uint32 mrs0_wr2;
    
    uint32 mrs1_wr1;
    
    uint32 mrs2_wr1;
    
    uint32 mrs3_wr1;
} arad_api_dram_ddr3_t;

typedef union
{
    arad_api_dram_ddr3_t ddr3;
} ARAD_DRAM_MODE_REGS_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  
  uint8 auto_mode;
  
  ARAD_DRAM_NUM_BANKS nof_banks;
  
  ARAD_DRAM_NUM_COLUMNS nof_cols;
  
  ARAD_DRAM_AP_POSITION ap_bit_pos;
  
  ARAD_DRAM_BURST_SIZE burst_size;
  
  uint32 c_cas_latency;
  
  uint32 c_wr_latency;
  
  uint32 t_rc;
  
  uint32 t_rfc;
  
  uint32 t_ras;
  
  uint32 t_faw;
  
  uint32 t_rcd_rd;
  
  uint32 t_rcd_wr;
  
  uint32 t_rrd;
  
  uint32 t_rp;
  
  uint32 t_wr;
  
  uint32 t_ref;
  
  uint32 t_wtr;
  
  uint32 t_rtp;
  
  uint32 jedec;
  
  ARAD_DRAM_MODE_REGS_INFO mode_regs;

} ARAD_DRAM_INFO;


typedef  SOC_TMC_DRAM_BIST_DATA_PATTERN_MODE ARAD_DRAM_BIST_DATA_PATTERN_MODE; 

typedef  SOC_TMC_DRAM_BIST_INFO ARAD_DRAM_BIST_TEST_RUN_INFO; 

typedef struct arad_dram_buffer_info_s{
    uint32 buf_num;
    uint32 bank;
    char channel;
    uint32 err_cntr; 
    uint32 is_deleted; 
} arad_dram_buffer_info_t;





uint32
  arad_dram_info_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 dram_ndx,
    SOC_SAND_IN  uint32                 dram_freq,
    SOC_SAND_IN  ARAD_DRAM_TYPE           dram_type,
    SOC_SAND_IN  ARAD_DRAM_INFO           *info
  );


uint32
  arad_dram_info_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 dram_ndx,
    SOC_SAND_IN  uint32                 dram_freq,
    SOC_SAND_OUT ARAD_DRAM_TYPE           *dram_type,
    SOC_SAND_OUT ARAD_DRAM_INFO           *info
  );


uint32
    arad_dram_bist_test_start(
      SOC_SAND_IN int unit,
      SOC_SAND_IN uint32 dram_ndx,
      SOC_SAND_IN ARAD_DRAM_BIST_TEST_RUN_INFO *info
    );




uint32
    arad_dram_mmu_indirect_get_logical_address_full(
         SOC_SAND_IN int unit,
         SOC_SAND_IN uint32 buf_num,
         SOC_SAND_IN uint32 index,
         SOC_SAND_OUT uint32* addr_full 
    );




uint32
    arad_dram_mmu_indirect_read(
        SOC_SAND_IN int unit,
        SOC_SAND_IN uint32 logical_mod,
        SOC_SAND_IN uint32 addr,
        soc_reg_above_64_val_t* data
    );
    
uint32
    arad_dram_mmu_indirect_write(
        SOC_SAND_IN int unit,
        SOC_SAND_IN uint32 logical_mod,
        SOC_SAND_IN uint32 addr,
        soc_reg_above_64_val_t* data
    );


 
uint32
    arad_dram_crc_delete_buffer_enable(
        SOC_SAND_IN int unit,
        SOC_SAND_IN uint32 fifo_depth);  
 
 
  
uint32
    arad_dram_crc_del_buffer_max_reclaims_set(
        SOC_SAND_IN int unit,
        SOC_SAND_IN uint32 max_err);

uint32
    arad_dram_crc_del_buffer_max_reclaims_get(
        SOC_SAND_IN int unit,
        SOC_SAND_OUT uint32 *max_err);


uint32
    arad_dram_buffer_get_info(
            SOC_SAND_IN int unit,
            SOC_SAND_IN  uint32 buf,
            SOC_SAND_OUT arad_dram_buffer_info_t *buf_info);      
            


uint32
    arad_dram_delete_buffer_read_fifo(
           SOC_SAND_IN int unit, 
           SOC_SAND_IN uint32 del_buf_max, 
           SOC_SAND_OUT uint32 *del_buf_array, 
           SOC_SAND_OUT uint32 *del_buf_count);
                   


uint32
    arad_dram_delete_buffer_test(
           SOC_SAND_IN int unit, 
           SOC_SAND_IN uint32 del_buf, 
           SOC_SAND_OUT uint32 *is_pass);



uint32
    arad_dram_delete_buffer_action(
        SOC_SAND_IN int unit, 
        SOC_SAND_IN uint32 buf, 
        SOC_SAND_IN uint32 should_delete);


uint32
    arad_dram_init_buffer_error_cntr(
        SOC_SAND_IN int unit, 
        SOC_SAND_IN uint32 buf);


uint32
   arad_dram_get_buffer_error_cntr_in_list_index(
        SOC_SAND_IN int unit, 
        SOC_SAND_IN uint32 index_in_list, 
        SOC_SAND_OUT uint32 *is_buf,
        SOC_SAND_OUT uint32 *buf);

void
  arad_ARAD_DRAM_MR_INFO_clear(
    SOC_SAND_OUT ARAD_DRAM_MODE_REGS_INFO  *info
  );

void
  arad_ARAD_DRAM_INFO_clear(
    SOC_SAND_OUT ARAD_DRAM_INFO *info
  );

#if ARAD_DEBUG_IS_LVL1

const char*
  arad_ARAD_DRAM_TYPE_to_string(
    SOC_SAND_IN ARAD_DRAM_TYPE enum_val
  );

const char*
  arad_ARAD_DRAM_BURST_SIZE_to_string(
    SOC_SAND_IN ARAD_DRAM_BURST_SIZE enum_val
  );

const char*
  arad_ARAD_DRAM_NUM_BANKS_to_string(
    SOC_SAND_IN ARAD_DRAM_NUM_BANKS enum_val
  );

const char*
  arad_ARAD_DRAM_NUM_COLUMNS_to_string(
    SOC_SAND_IN ARAD_DRAM_NUM_COLUMNS enum_val
  );

const char*
  arad_ARAD_DRAM_AP_POSITION_to_string(
    SOC_SAND_IN ARAD_DRAM_AP_POSITION enum_val
  );


void
  arad_ARAD_DRAM_MR_INFO_print(
    SOC_SAND_IN ARAD_DRAM_TYPE           dram_type,
    SOC_SAND_IN ARAD_DRAM_MODE_REGS_INFO *info
  );

void
  arad_ARAD_DRAM_INFO_print(
    SOC_SAND_IN ARAD_DRAM_TYPE dram_type,
    SOC_SAND_IN ARAD_DRAM_INFO *info
  );



#endif 

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

