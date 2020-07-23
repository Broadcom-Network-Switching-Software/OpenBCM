/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _PEM_MEMINFO_ACCESS_H_
#define _PEM_MEMINFO_ACCESS_H_

#include <stdio.h>






#define UCODE_MAX_NOF_MEM_LINE  128000    
#define UCODE_MAX_NOF_REG_LINE  512      

#define PEM_RANGE_LOW_LIMIT 0
#define PEM_RANGE_HIGH_LIMIT 1

#define DB_ROWS 0
#define DB_COLUMNS 1

#define MAX_NOF_FIELDS   16

#define IN 
#define OUT 



void get_nof_entries(int db_id);
void qax_db_mapping_info_init(int nof_reg);
void qax_reg_mapping_info_init(int nof_dbs);
void qax_tcam_mapping_info_init(int nof_dbs);
void qax_em_mapping_info_init(int nof_dbs);
void qax_lpm_mapping_info_init(int nof_dbs);



void qax_ucode_init();
void qax_ucode_add(int unit, unsigned int mem_block_id, unsigned int mem_addr, unsigned int index, unsigned int *data, unsigned int data_length);
int  qax_ucode_nof_instruction_get();
int  qax_ucode_instruction_get(unsigned int instruction_no, unsigned int *unit, unsigned int *mem_block_id, unsigned int *mem_addr, 
                           unsigned int *row_offset, unsigned int **data, unsigned int *data_length);


#endif 

