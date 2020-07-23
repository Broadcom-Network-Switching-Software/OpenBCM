/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include "pem_physical_access.h"
#include "pem_meminfo_access.h"
#include "pem_meminfo_init.h"
#include "pem_logical_access.h"


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

ApiInfo qax_api_info;





void qax_db_mapping_info_init(int nof_dbs) {
  qax_api_info.db_direct_container.nof_direct_dbs = nof_dbs;
  qax_api_info.db_direct_container.db_direct_info_arr = (LogicalDirectInfo*)malloc(nof_dbs * sizeof(LogicalDirectInfo));
}

void qax_reg_mapping_info_init(int nof_reg) {
  qax_api_info.reg_container.nof_registers = nof_reg;
  qax_api_info.reg_container.reg_info_arr = (LogicalRegInfo*)malloc(nof_reg * sizeof(LogicalRegInfo));
}

void qax_tcam_mapping_info_init(int nof_dbs) {
  qax_api_info.db_tcam_container.nof_tcam_dbs = nof_dbs;
  qax_api_info.db_tcam_container.db_tcam_info_arr = (LogicalTcamInfo*)malloc(nof_dbs * sizeof(LogicalTcamInfo));
}

void qax_em_mapping_info_init(int nof_dbs) {
  qax_api_info.db_em_container.nof_em_dbs = nof_dbs;
  qax_api_info.db_em_container.db_em_info_arr = (LogicalEmInfo*)malloc(nof_dbs * sizeof(LogicalEmInfo));
}

void qax_lpm_mapping_info_init(int nof_dbs) {
  qax_api_info.db_lpm_container.nof_lpm_dbs = nof_dbs;
  qax_api_info.db_lpm_container.db_lpm_info_arr = (LogicalLpmInfo*)malloc(nof_dbs * sizeof(LogicalLpmInfo));
}




typedef struct {
  unsigned int  unit;
  unsigned int  mem_block_id;
  unsigned int  mem_addr;
  unsigned int  row_offset;
  unsigned int  *data;
  unsigned int  data_length;
} ucode_write_instructions_t;


static ucode_write_instructions_t ucode[UCODE_MAX_NOF_MEM_LINE + UCODE_MAX_NOF_REG_LINE];

static unsigned int ucode_instruction_counter;

void qax_ucode_init() {
  ucode_instruction_counter = 0;
}

void qax_ucode_add(int unit, unsigned int mem_block_id, unsigned int mem_addr, unsigned int index, unsigned int *data, unsigned int data_length) {

  assert(data_length < MAX_MEM_DATA_LENGTH);

  ucode[ucode_instruction_counter].unit = unit;
  ucode[ucode_instruction_counter].mem_block_id = mem_block_id;
  ucode[ucode_instruction_counter].mem_addr = mem_addr;
  ucode[ucode_instruction_counter].row_offset = index;
  ucode[ucode_instruction_counter].data = data;
  ucode[ucode_instruction_counter].data_length = data_length;

  assert(ucode_instruction_counter < UCODE_MAX_NOF_MEM_LINE + UCODE_MAX_NOF_REG_LINE);
  ucode_instruction_counter = ucode_instruction_counter + 1;
}

int  qax_ucode_nof_instruction_get() {
  return ucode_instruction_counter;
}

int  qax_ucode_instruction_get(unsigned int instruction_no, unsigned int *unit, unsigned int *mem_block_id, unsigned int *mem_addr, unsigned int *row_offset, unsigned int **data, unsigned int *data_length) {
  assert(instruction_no < ucode_instruction_counter);
  *unit = ucode[instruction_no].unit;
  *mem_block_id = ucode[instruction_no].mem_block_id;
  *mem_addr = ucode[instruction_no].mem_addr;
  *row_offset = ucode[instruction_no].row_offset;
  *data_length = ucode[instruction_no].data_length;
  *data = ucode[instruction_no].data;

  return 0;
}


void qax_ucode_upload() {
  unsigned int inst;

  for (inst = 0; inst < ucode_instruction_counter; inst = inst + 1) {

  }

}

