/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _SOC_SAND_MEM_H
#define _SOC_SAND_MEM_H
#include <soc/mcm/allenum.h>

#define SAND_MAX_U32S_IN_MEM_ENTRY 20


uint32 sand_alloc_mem(
    const int unit,
    void      *mem_ptr,        
    const unsigned size,       
    const char     *alloc_name 
);


uint32 sand_free_mem(
    const int unit,
    void **mem_ptr 
);


void sand_free_mem_if_not_null(
    const int unit,
    void **mem_ptr 
);




uint32 sand_alloc_dma_mem(
    const int unit,
    const uint8     is_slam,    
    void            **mem_ptr,  
    const unsigned  size,       
    const char      *alloc_name 
);


uint32 sand_free_dma_mem(
    const int unit,
    const uint8 is_slam, 
    void  **mem_ptr      
);





uint32 sand_init_fill_table(
    const  int unit
);


uint32 sand_deinit_fill_table(
    const  int unit
);


uint32 sand_fill_table_with_entry(
    const int       unit,
    const soc_mem_t mem,        
    const int       copyno,     
    const void      *entry_data 
  );


uint32 sand_fill_partial_table_with_entry(
    const int       unit,
    const soc_mem_t mem,               
    const unsigned  array_index_start, 
    const unsigned  array_index_end,   
    const int       copyno,            
    const int       index_start,       
    const int       index_end,         
    const void      *entry_data        
  );


uint32 sand_fill_table_with_entry_skip_emul_err(
    const int       unit,
    const soc_mem_t mem,        
    const int       copyno,     
    const void      *entry_data 
    );


int sand_mem_array_wide_access(
    int unit,
    uint32 flags,
    soc_mem_t mem,
    unsigned array_index,
    int copyno,
    int index,
    uint32 *entry_data,
    unsigned do_read  
  );



int sand_tbl_is_dynamic(int unit,soc_mem_t mem);


int sand_mem_contain_one_of_the_fields(int unit,const soc_mem_t mem,soc_field_t *fields);


int soc_sand_cache_table_update_all(int unit);


int sand_do_read_table(int unit, soc_mem_t mem, unsigned array_index,
                  int index, int count, uint32 *entry_ptr);


int soc_sand_control_dynamic_mem_writes(
    const int unit,
    const soc_reg_t *regs, 
    const uint32 val); 

int soc_sand_mem_is_in_soc_property(int unit, soc_mem_t mem, int en);


int sand_tbl_mem_cache_mem_set(int unit, soc_mem_t mem, void* en);


int
sand_mem_indirect_poll_done(
        int unit,
        int blk_id,
        int time_out);

int
sand_mem_indirect_reset_write(
        int unit,
        soc_mem_t mem,
        int blk_id,
        uint32 address,
        int nof_entries,
        uint32 *value);


int
sand_fill_mem_indirect_reset_write(
        const int unit,
        const soc_mem_t mem,
        const int copyno,     
        uint32 array_index_start,
        uint32 array_index_end,
        uint32 index_start,
        uint32 index_end,
        const uint32 *entry);


int soc_sand_indirect_mem_access_init(int unit);


int soc_sand_indirect_mem_access_deinit(int unit);

#endif 
