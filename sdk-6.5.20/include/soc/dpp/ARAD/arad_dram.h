/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_DRAM_INCLUDED__

#define __ARAD_DRAM_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/arad_api_dram.h>
#include <soc/dpp/ARAD/arad_api_mgmt.h>




#define SOC_ARAD_DRAM_USER_BUFFER_FLAGS_LOGICAL2PHY_TRANS 0x1


#define SOC_DPP_DRAM_MMU_IND_ACCESS_MAX_BUFF_SIZE 4











typedef struct
{
  
  uint32  dram_deleted_buff_list[ARAD_DRAM_MAX_BUFFERS_IN_ERROR_CNTR];

} ARAD_DRAM;












uint32 arad_mgmt_dram_init_drc_soft_init(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 *is_valid,
    SOC_SAND_IN uint32  init);

uint32 arad_dram_read_data_dly_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32  dram_freq);


uint32
  arad_dram_info_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 dram_ndx,
    SOC_SAND_IN  uint32                 dram_freq,
    SOC_SAND_IN  ARAD_DRAM_TYPE           dram_type,
    SOC_SAND_IN  ARAD_DRAM_INFO           *info
  );


uint32
  arad_dram_info_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 dram_freq,
    SOC_SAND_IN  ARAD_DRAM_TYPE           dram_type,
    SOC_SAND_IN  ARAD_DRAM_INFO           *info
  );


uint32
  arad_dram_info_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 dram_ndx,
    SOC_SAND_IN  uint32                 dram_freq,
    SOC_SAND_OUT ARAD_DRAM_TYPE           *dram_type,
    SOC_SAND_OUT ARAD_DRAM_INFO           *info
  );


uint32 arad_dram_init_drc_phy_register_set(int unit, ARAD_DRAM_TYPE dram_type, uint32 dram_freq, ARAD_DRAM_INFO t_info);



uint32 arad_dram_rbus_write(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 dram_ndx,
    SOC_SAND_IN uint32 addr,
    SOC_SAND_IN uint32 data
  );



uint32 arad_dram_rbus_write_br(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 last_dram_ndx,
    SOC_SAND_IN uint32 addr,
    SOC_SAND_IN uint32 data
  );


uint32 arad_dram_rbus_read(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 dram_ndx,
    SOC_SAND_IN  uint32 addr,
    SOC_SAND_OUT uint32 *data
  );


uint32 arad_dram_rbus_modify(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 dram_ndx,
    SOC_SAND_IN  uint32 addr,
    SOC_SAND_IN  uint32 data,
    SOC_SAND_IN  uint32 mask
);



uint32
  arad_dram_configure_bist(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 dram_ndx,
    SOC_SAND_IN uint32 write_weight,
    SOC_SAND_IN uint32 read_weight,
    SOC_SAND_IN uint32 pattern_bit_mode,
    SOC_SAND_IN uint32 two_addr_mode,
    SOC_SAND_IN uint32 prbs_mode,
    SOC_SAND_IN uint32 cons_addr_8_banks,
    SOC_SAND_IN uint32 address_shift_mode,
    SOC_SAND_IN uint32 data_shift_mode,
    SOC_SAND_IN uint32 data_addr_mode,
    SOC_SAND_IN uint32 bist_num_actions,
    SOC_SAND_IN uint32 bist_start_address,
    SOC_SAND_IN uint32 bist_end_address,
    SOC_SAND_IN uint32 bist_pattern[ARAD_DRAM_BIST_NOF_PATTERNS]
  );

uint32
  arad_dram_bist_atomic_action(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 dram_ndx,
    SOC_SAND_IN uint8  is_infinite,
    SOC_SAND_IN uint8  stop
  );

uint32
  arad_dram_bist_atomic_action_start(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 dram_ndx,
    uint8 start
  );

uint32
  arad_dram_bist_atomic_action_polling(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 dram_ndx
  );

uint32
  arad_dram_read_bist_err_cnt(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 dram_ndx,
    SOC_SAND_OUT uint32 *bist_err_occur,
    SOC_SAND_OUT uint32 *bist_full_err_cnt,
    SOC_SAND_OUT uint32 *bist_single_err_cnt,
    SOC_SAND_OUT uint32 *bist_global_err_cnt
  );

uint32
    arad_dram_bist_test_start_verify(
        SOC_SAND_IN int unit,
        SOC_SAND_IN uint32 dram_ndx,
        SOC_SAND_IN ARAD_DRAM_BIST_TEST_RUN_INFO *info
    );

uint32
    arad_dram_bist_test_start_unsafe(
      SOC_SAND_IN int unit,
      SOC_SAND_IN uint32 dram_ndx,
      SOC_SAND_IN ARAD_DRAM_BIST_TEST_RUN_INFO *info
    );


uint32
    arad_dram_init_ddr(
        SOC_SAND_IN int                unit,
        SOC_SAND_IN ARAD_INIT_DDR         *init
    );



uint32 arad_dram_init_power_down_unused_dprcs(
        SOC_SAND_IN int unit,
        SOC_SAND_IN uint8* is_valid_dram
    );



uint32
    arad_dram_mmu_indirect_get_logical_address_full_unsafe(
         SOC_SAND_IN int unit,
         SOC_SAND_IN uint32 buf_num,
         SOC_SAND_IN uint32 index,
         SOC_SAND_OUT uint32* addr_full 
    );

uint32 
arad_dram_logical2physical_addr_mapping(
    int unit, 
    int buf_num,
    int index,
    uint32 *phy_addr); 




uint32
    arad_dram_mmu_indirect_read_unsafe(
        SOC_SAND_IN int unit,
        SOC_SAND_IN uint32 logical_mod,
        SOC_SAND_IN uint32 addr,
        soc_reg_above_64_val_t* data
    );
    
uint32
    arad_dram_mmu_indirect_write_unsafe(
        SOC_SAND_IN int unit,
        SOC_SAND_IN uint32 logical_mod,
        SOC_SAND_IN uint32 addr,
        soc_reg_above_64_val_t* data
    );
  
uint32
soc_arad_user_buffer_dram_access(
     SOC_SAND_IN        int unit, 
     SOC_SAND_IN        uint32 flags, 
     SOC_SAND_IN        uint32 access_type, 
     SOC_SAND_INOUT     uint8 *buf, 
     SOC_SAND_IN        int offset, 
     SOC_SAND_IN        int nbytes);

uint32
soc_arad_user_buffer_dram_write(
    SOC_SAND_IN         int unit, 
    SOC_SAND_IN         uint32 flags, 
    SOC_SAND_INOUT      uint8 *buf, 
    SOC_SAND_IN         int offset, 
    SOC_SAND_IN         int nbytes);
  
uint32
soc_arad_user_buffer_dram_read(
    SOC_SAND_IN         int unit, 
    SOC_SAND_IN         uint32 flags, 
    SOC_SAND_INOUT      uint8 *buf, 
    SOC_SAND_IN         int offset, 
    SOC_SAND_IN         int nbytes);
  

 
uint32
    arad_dram_crc_delete_buffer_enable_unsafe(
        SOC_SAND_IN int unit,
        SOC_SAND_IN uint32 fifo_depth);   
  
 
 
  
uint32   
    arad_dram_crc_del_buffer_max_reclaims_set_unsafe(
        SOC_SAND_IN int unit,
        SOC_SAND_IN uint32 max_err);  
  
uint32   
    arad_dram_crc_del_buffer_max_reclaims_get_unsafe(
        SOC_SAND_IN int unit,
        SOC_SAND_OUT uint32 *max_err);
 

uint32
    arad_dram_buffer_get_info_unsafe(
            SOC_SAND_IN int unit,
            SOC_SAND_IN  uint32 buf,
            SOC_SAND_OUT arad_dram_buffer_info_t *buf_info);
       
uint32
    arad_dram_buffer_get_info_verify(
            SOC_SAND_IN int unit,
            SOC_SAND_IN  uint32 buf,
            SOC_SAND_OUT arad_dram_buffer_info_t *buf_info);       
         


uint32
    arad_dram_delete_buffer_read_fifo_unsafe(
           SOC_SAND_IN int unit, 
           SOC_SAND_IN uint32 del_buf_max, 
           SOC_SAND_OUT uint32 *del_buf_array, 
           SOC_SAND_OUT uint32 *del_buf_count);

uint32
    arad_dram_delete_buffer_read_fifo_verify(
           SOC_SAND_IN int unit, 
           SOC_SAND_IN uint32 del_buf_max,
           SOC_SAND_OUT uint32 *del_buf_array, 
           SOC_SAND_OUT uint32 *del_buf_count);
                   


uint32
    arad_dram_delete_buffer_test_unsafe(
           SOC_SAND_IN int unit, 
           SOC_SAND_IN uint32 del_buf, 
           SOC_SAND_OUT uint32 *is_pass);

uint32
    arad_dram_delete_buffer_test_verify(
           SOC_SAND_IN int unit, 
           SOC_SAND_IN uint32 del_buf,
           SOC_SAND_OUT uint32 *is_pass);



uint32
    arad_dram_delete_buffer_action_unsafe(
        SOC_SAND_IN int unit, 
        SOC_SAND_IN uint32 buf, 
        SOC_SAND_IN uint32 should_delete);

uint32
    arad_dram_delete_buffer_action_verify(
        SOC_SAND_IN int unit, 
        SOC_SAND_IN uint32 buf);


uint32
    arad_dram_init_buffer_error_cntr_unsafe(
        SOC_SAND_IN int unit, 
        SOC_SAND_IN uint32 buf);

uint32
    arad_dram_init_buffer_error_cntr_verify(
        SOC_SAND_IN int unit, 
        SOC_SAND_IN uint32 buf);


uint32
    arad_dram_get_buffer_error_cntr_in_list_index_unsafe(
        SOC_SAND_IN int unit, 
        SOC_SAND_IN uint32 index_in_list, 
        SOC_SAND_OUT uint32 *is_buf,
        SOC_SAND_OUT uint32 *buf );

uint32
    arad_dram_get_buffer_error_cntr_in_list_index_verify(
        SOC_SAND_IN int unit, 
        SOC_SAND_IN uint32 index_in_list,
        SOC_SAND_OUT uint32 *is_buf,
        SOC_SAND_OUT uint32 *buf);

int soc_arad_validate_dram_address (int unit, uint32 addr);


int soc_ardon_dram_info_verify(int unit, soc_dpp_drc_combo28_info_t *drc_info);
int soc_dpp_drc_combo28_dram_init(int unit, soc_dpp_drc_combo28_info_t *drc_info);

#include <soc/dpp/SAND/Utils/sand_footer.h>



#endif
