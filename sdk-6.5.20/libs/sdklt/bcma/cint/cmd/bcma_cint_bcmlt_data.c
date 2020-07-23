/*
 * $Id: bcma_cint_data.c
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * bcma_cint_data.c
 */

#include <sal_config.h>
#include "../core/cint_config.h"
#include "../core/cint_types.h"
#include "../core/cint_porting.h"
#include <sal/sal_libc.h>
#include <bcmlt/bcmlt.h>
#include <bcmltd/bcmltd_types.h>

CINT_FWRAPPER_CREATE2_RP3(int, int, 0, 0,
                          bcmlt_entry_commit, \
                          bcmlt_entry_handle_t,bcmlt_entry_handle_t,entry_hdl,0,0,CINT_PARAM_IN,
                          bcmlt_opcode_t,bcmlt_opcode_t,opcode,0,0,CINT_PARAM_IN,
                          bcmlt_priority_level_t,bcmlt_priority_level_t,priority,0,0,CINT_PARAM_IN);
CINT_FWRAPPER_CREATE2_RP3(int, int, 0, 0,
                          bcmlt_pt_entry_commit, \
                          bcmlt_entry_handle_t,bcmlt_entry_handle_t,entry_hdl,0,0,CINT_PARAM_IN,
                          bcmlt_pt_opcode_t,bcmlt_pt_opcode_t,opcode,0,0,CINT_PARAM_IN,
                          bcmlt_priority_level_t,bcmlt_priority_level_t,priority,0,0,CINT_PARAM_IN);
CINT_FWRAPPER_CREATE2_RP3(int, int, 0, 0,
                          bcmlt_table_names_get_first, \
                          int,int,unit,0,0,CINT_PARAM_IN,
                          uint32_t,uint32_t,flags,0,0,CINT_PARAM_IN,
                          char**,char,name,2,0,CINT_PARAM_OUT);
CINT_FWRAPPER_CREATE2_RP3(int, int, 0, 0,
                          bcmlt_table_names_get_next, \
                          int,int,unit,0,0,CINT_PARAM_IN,
                          uint32_t,uint32_t,flags,0,0,CINT_PARAM_IN,
                          char**,char,name,2,0,CINT_PARAM_OUT);
CINT_FWRAPPER_CREATE2_RP3(int, int, 0, 0,
                          bcmlt_table_desc_get, \
                          int,int,unit,0,0,CINT_PARAM_IN,
                          const char *,char,tbl_name,1,0,CINT_PARAM_IN,
                          const char**,char,tbl_desc,2,0,CINT_PARAM_OUT);
CINT_FWRAPPER_CREATE2_RP5(int, int, 0, 0,
                          bcmlt_table_field_defs_get, \
                          int,int,unit,0,0,CINT_PARAM_IN,
                          const char *,char,table_name,1,0,CINT_PARAM_IN,
                          uint32_t,uint32_t,fields_array_size,0,0,CINT_PARAM_IN|CINT_PARAM_VL,
                          bcmlt_field_def_t *,bcmlt_field_def_t,field_defs_array,1,0,CINT_PARAM_OUT|CINT_PARAM_VP|3,
                          uint32_t *,uint32_t,require_array_size,1,0,CINT_PARAM_OUT|CINT_PARAM_VL);
CINT_FWRAPPER_CREATE2_RP2(int, int, 0, 0,
                          bcmlt_device_attach, \
                          int,int,unit,0,0,CINT_PARAM_IN,
                          bool,bool,warm,0,0,CINT_PARAM_IN);
CINT_FWRAPPER_CREATE2_RP1(int, int, 0, 0,
                          bcmlt_device_detach, \
                          int,int,unit,0,0,CINT_PARAM_IN);
CINT_FWRAPPER_CREATE2_RP1(bool, bool, 0, 0,
                          bcmlt_device_attached, \
                          int,int,unit,0,0,CINT_PARAM_IN);
CINT_FWRAPPER_CREATE2_RP1(int, int, 0, 0,
                          bcmlt_api_clear, \
                          int,int,unit,0,0,CINT_PARAM_IN);
CINT_FWRAPPER_CREATE2_RP3(int, int, 0, 0,
                          bcmlt_entry_field_add, \
                          bcmlt_entry_handle_t,bcmlt_entry_handle_t,entry_hdl,0,0,CINT_PARAM_IN,
                          const char *,char,field_name,1,0,CINT_PARAM_IN,
                          uint64_t,uint64_t,data,0,0,CINT_PARAM_IN);
CINT_FWRAPPER_CREATE2_RP3(int, int, 0, 0,
                          bcmlt_entry_field_add_by_id, \
                          bcmlt_entry_handle_t,bcmlt_entry_handle_t,entry_hdl,0,0,CINT_PARAM_IN,
                          uint32_t,uint32_t,fid,0,0,CINT_PARAM_IN,
                          uint64_t,uint64_t,data,0,0,CINT_PARAM_IN);
CINT_FWRAPPER_CREATE2_RP2(int, int, 0, 0,
                          bcmlt_entry_field_unset,\
                          bcmlt_entry_handle_t, bcmlt_entry_handle_t, entry_hdl, 0, 0, CINT_PARAM_IN,
                          const char *, char, field_name, 1, 0, CINT_PARAM_IN);
CINT_FWRAPPER_CREATE2_RP3(int, int, 0, 0,
                          bcmlt_entry_field_symbol_add, \
                          bcmlt_entry_handle_t,bcmlt_entry_handle_t,entry_hdl,0,0,CINT_PARAM_IN,
                          const char *,char,field_name,1,0,CINT_PARAM_IN,
                          const char *,char,data,1,0,CINT_PARAM_IN);
CINT_FWRAPPER_CREATE2_RP3(int, int, 0, 0,
                          bcmlt_entry_field_symbol_add_by_id,
                          bcmlt_entry_handle_t,bcmlt_entry_handle_t,entry_hdl,0,0,CINT_PARAM_IN,
                          uint32_t,uint32_t,fid,0,0,CINT_PARAM_IN,
                          const char *,char,data,1,0,CINT_PARAM_IN);
CINT_FWRAPPER_CREATE2_RP3(int, int, 0, 0,
                          bcmlt_entry_field_get, \
                          bcmlt_entry_handle_t,bcmlt_entry_handle_t,entry_hdl,0,0,CINT_PARAM_IN,
                          const char *,char,field_name,1,0,CINT_PARAM_IN,
                          uint64_t *,uint64_t,data,1,0,CINT_PARAM_OUT);
CINT_FWRAPPER_CREATE2_RP3(int, int, 0, 0,
                          bcmlt_entry_field_get_by_id, \
                          bcmlt_entry_handle_t,bcmlt_entry_handle_t,entry_hdl,0,0,CINT_PARAM_IN,
                          uint32_t,uint32_t,fid,0,0,CINT_PARAM_IN,
                          uint64_t *,uint64_t,data,1,0,CINT_PARAM_OUT);
CINT_FWRAPPER_CREATE2_RP3(int, int, 0, 0,
                          bcmlt_entry_field_symbol_get, \
                          bcmlt_entry_handle_t,bcmlt_entry_handle_t,entry_hdl,0,0,CINT_PARAM_IN,
                          const char *,char,field_name,1,0,CINT_PARAM_IN,
                          const char **,char,data,2,0,CINT_PARAM_OUT);
CINT_FWRAPPER_CREATE2_RP3(int, int, 0, 0,
                          bcmlt_entry_field_symbol_get_by_id,
                          bcmlt_entry_handle_t,bcmlt_entry_handle_t,entry_hdl,0,0,CINT_PARAM_IN,
                          uint32_t,uint32_t,fid,0,0,CINT_PARAM_IN,
                          const char **,char,data,2,0,CINT_PARAM_OUT);
CINT_FWRAPPER_CREATE2_RP5(int, int, 0, 0,
                          bcmlt_entry_field_array_add, \
                          bcmlt_entry_handle_t,bcmlt_entry_handle_t,entry_hdl,0,0,CINT_PARAM_IN,
                          const char *,char,field_name,1,0,CINT_PARAM_IN,
                          uint32_t,uint32_t,start_idx,0,0,CINT_PARAM_IN,
                          uint64_t*,uint64_t,data,1,0,CINT_PARAM_IN|CINT_PARAM_VP|5,
                          uint32_t,uint32_t,num_of_elem,0,0,CINT_PARAM_IN|CINT_PARAM_VL);
CINT_FWRAPPER_CREATE2_RP5(int, int, 0, 0,
                          bcmlt_entry_field_array_add_by_id, \
                          bcmlt_entry_handle_t,bcmlt_entry_handle_t,entry_hdl,0,0,CINT_PARAM_IN,
                          uint32_t,uint32_t,fid,0,0,CINT_PARAM_IN,
                          uint32_t,uint32_t,start_idx,0,0,CINT_PARAM_IN,
                          uint64_t*,uint64_t,data,1,0,CINT_PARAM_IN|CINT_PARAM_VP|5,
                          uint32_t,uint32_t,num_of_elem,0,0,CINT_PARAM_IN|CINT_PARAM_VL);
CINT_FWRAPPER_CREATE2_RP4(int, int, 0, 0,
                          bcmlt_entry_field_array_remove, \
                          bcmlt_entry_handle_t,bcmlt_entry_handle_t,entry_hdl,0,0,CINT_PARAM_IN,
                          const char *,char,field_name,1,0,CINT_PARAM_IN,
                          uint32_t,uint32_t,start_idx,0,0,CINT_PARAM_IN,
                          uint32_t,uint32_t,num_of_elem,0,0,CINT_PARAM_IN|CINT_PARAM_VL);
CINT_FWRAPPER_CREATE2_RP5(int, int, 0, 0,
                          bcmlt_entry_field_array_symbol_add, \
                          bcmlt_entry_handle_t,bcmlt_entry_handle_t,entry_hdl,0,0,CINT_PARAM_IN,
                          const char *,char,field_name,1,0,CINT_PARAM_IN,
                          uint32_t,uint32_t,start_idx,0,0,CINT_PARAM_IN,
                          const char **,char,data,2,0,CINT_PARAM_IN|CINT_PARAM_VP|5,
                          uint32_t,uint32_t,num_of_elem,0,0,CINT_PARAM_IN|CINT_PARAM_VL);
CINT_FWRAPPER_CREATE2_RP5(int, int, 0, 0,
                          bcmlt_entry_field_array_sym_add_by_id, \
                          bcmlt_entry_handle_t,bcmlt_entry_handle_t,entry_hdl,0,0,CINT_PARAM_IN,
                          uint32_t,uint32_t,fid,0,0,CINT_PARAM_IN,
                          uint32_t,uint32_t,start_idx,0,0,CINT_PARAM_IN,
                          const char **,char,data,2,0,CINT_PARAM_IN|CINT_PARAM_VP|5,
                          uint32_t,uint32_t,num_of_elem,0,0,CINT_PARAM_IN|CINT_PARAM_VL);
CINT_FWRAPPER_CREATE2_RP6(int, int, 0, 0,
                          bcmlt_entry_field_array_get_by_id, \
                          bcmlt_entry_handle_t,bcmlt_entry_handle_t,entry_hdl,0,0,CINT_PARAM_IN,
                          uint32_t,uint32_t,fid,0,0,CINT_PARAM_IN,
                          uint32_t,uint32_t,start_idx,0,0,CINT_PARAM_IN,
                          uint64_t*,uint64_t,data,1,0,CINT_PARAM_OUT|CINT_PARAM_VP|5,
                          uint32_t,uint32_t,num_of_elem,0,0,CINT_PARAM_IN|CINT_PARAM_VL,
                          uint32_t *,uint32_t,r_elem_cnt,1,0,CINT_PARAM_OUT|CINT_PARAM_VL);
CINT_FWRAPPER_CREATE2_RP6(int, int, 0, 0,
                          bcmlt_entry_field_array_get, \
                          bcmlt_entry_handle_t,bcmlt_entry_handle_t,entry_hdl,0,0,CINT_PARAM_IN,
                          const char *,char,field_name,1,0,CINT_PARAM_IN,
                          uint32_t,uint32_t,start_idx,0,0,CINT_PARAM_IN,
                          uint64_t*,uint64_t,data,1,0,CINT_PARAM_OUT|CINT_PARAM_VP|5,
                          uint32_t,uint32_t,num_of_elem,0,0,CINT_PARAM_IN|CINT_PARAM_VL,
                          uint32_t *,uint32_t,r_elem_cnt,1,0,CINT_PARAM_OUT|CINT_PARAM_VL);
CINT_FWRAPPER_CREATE2_RP6(int, int, 0, 0,
                          bcmlt_entry_field_array_symbol_get, \
                          bcmlt_entry_handle_t,bcmlt_entry_handle_t,entry_hdl,0,0,CINT_PARAM_IN,
                          const char *,char,field_name,1,0,CINT_PARAM_IN,
                          uint32_t,uint32_t,start_idx,0,0,CINT_PARAM_IN,
                          const char **,char,data,2,0,CINT_PARAM_OUT|CINT_PARAM_VP|5,
                          uint32_t,uint32_t,num_of_elem,0,0,CINT_PARAM_IN|CINT_PARAM_VL,
                          uint32_t *,uint32_t,r_elem_cnt,1,0,CINT_PARAM_OUT|CINT_PARAM_VL);
CINT_FWRAPPER_CREATE2_RP6(int, int, 0, 0,
                          bcmlt_entry_field_array_sym_get_by_id, \
                          bcmlt_entry_handle_t,bcmlt_entry_handle_t,entry_hdl,0,0,CINT_PARAM_IN,
                          uint32_t,uint32_t,fid,0,0,CINT_PARAM_IN,
                          uint32_t,uint32_t,start_idx,0,0,CINT_PARAM_IN,
                          const char **,char,data,2,0,CINT_PARAM_OUT|CINT_PARAM_VP|5,
                          uint32_t,uint32_t,num_of_elem,0,0,CINT_PARAM_IN|CINT_PARAM_VL,
                          uint32_t *,uint32_t,r_elem_cnt,1,0,CINT_PARAM_OUT|CINT_PARAM_VL);
CINT_FWRAPPER_CREATE2_RP4(int, int, 0, 0,
                          bcmlt_entry_field_array_unset, \
                          bcmlt_entry_handle_t,bcmlt_entry_handle_t,entry_hdl,0,0,CINT_PARAM_IN,
                          const char *,char,field_name,1,0,CINT_PARAM_IN,
                          uint32_t,uint32_t,start_idx,0,0,CINT_PARAM_IN,
                          uint32_t,uint32_t,num_of_elem,0,0,CINT_PARAM_IN|CINT_PARAM_VL);
CINT_FWRAPPER_CREATE2_RP2(int, int, 0, 0,
                          bcmlt_entry_field_remove, \
                          bcmlt_entry_handle_t,bcmlt_entry_handle_t,entry_hdl,0,0,CINT_PARAM_IN,
                          const char *,char,field_name,1,0,CINT_PARAM_IN);
CINT_FWRAPPER_CREATE2_RP5(int, int, 0, 0,
                          bcmlt_entry_field_symbol_info_get, \
                          bcmlt_entry_handle_t,bcmlt_entry_handle_t,entry_hdl,0,0,CINT_PARAM_IN,
                          const char *,char,field_name,1,0,CINT_PARAM_IN,
                          uint32_t,uint32_t,sym_names_size,0,0,CINT_PARAM_IN|CINT_PARAM_VL,
                          const char **,char,sym_names,2,0,CINT_PARAM_OUT|CINT_PARAM_VP|3,
                          uint32_t *,uint32_t,actual_size,1,0,CINT_PARAM_OUT|CINT_PARAM_VL);
CINT_FWRAPPER_CREATE2_RP2(int, int, 0, 0,
                          bcmlt_entry_info_get, \
                          bcmlt_entry_handle_t,bcmlt_entry_handle_t,entry_hdl,0,0,CINT_PARAM_IN,
                          bcmlt_entry_info_t *,bcmlt_entry_info_t,p_entry_info,1,0,CINT_PARAM_OUT);
CINT_FWRAPPER_CREATE2_RP3(int, int, 0, 0,
                          bcmlt_entry_allocate, \
                          int,int,unit,0,0,CINT_PARAM_IN,
                          const char *,char,table_name,1,0,CINT_PARAM_IN,
                          bcmlt_entry_handle_t*,bcmlt_entry_handle_t,entry_hdl,1,0,CINT_PARAM_OUT);
CINT_FWRAPPER_CREATE2_RP4(int, int, 0, 0,
                          bcmlt_entry_allocate_by_id, \
                          int,int,unit,0,0,CINT_PARAM_IN,
                          uint32_t,uint32_t,tid,0,0,CINT_PARAM_IN,
                          uint32_t,uint32_t,field_cnt,0,0,CINT_PARAM_IN,
                          bcmlt_entry_handle_t*,bcmlt_entry_handle_t,entry_hdl,1,0,CINT_PARAM_OUT);
CINT_FWRAPPER_CREATE2_RP1(int, int, 0, 0,
                          bcmlt_entry_free, \
                          bcmlt_entry_handle_t,bcmlt_entry_handle_t,entry_hdl,0,0,CINT_PARAM_IN);
CINT_FWRAPPER_CREATE2_RP1(int, int, 0, 0,
                          bcmlt_entry_clear,\
                          bcmlt_entry_handle_t, bcmlt_entry_handle_t, entry_hdl, 0, 0, CINT_PARAM_IN);
CINT_FWRAPPER_CREATE2_RP2(int, int, 0, 0,
                          bcmlt_entry_attrib_set, \
                          bcmlt_entry_handle_t,bcmlt_entry_handle_t,entry_hdl,0,0,CINT_PARAM_IN,
                          uint32_t,uint32_t,attrib,0,0,CINT_PARAM_IN);
CINT_FWRAPPER_CREATE2_RP2(int, int, 0, 0,
                          bcmlt_entry_attrib_get, \
                          bcmlt_entry_handle_t,bcmlt_entry_handle_t,entry_hdl,0,0,CINT_PARAM_IN,
                          uint32_t *,uint32_t,attrib,1,0,CINT_PARAM_OUT);
CINT_FWRAPPER_CREATE2_RP2(int, int, 0, 0,
                          bcmlt_entry_attrib_clear, \
                          bcmlt_entry_handle_t,bcmlt_entry_handle_t,entry_hdl,0,0,CINT_PARAM_IN,
                          uint32_t,uint32_t,attrib,0,0,CINT_PARAM_IN);
CINT_FWRAPPER_CREATE2_RP4(int, int, 0, 0,
                          bcmlt_table_subscribe, \
                          int,int,unit,0,0,CINT_PARAM_IN,
                          const char *,char,table_name,1,0,CINT_PARAM_IN,
                          bcmlt_table_sub_cb,bcmlt_table_sub_cb,callback,0,0,CINT_PARAM_IN,
                          void *,void,user_data,1,0,CINT_PARAM_IN);
CINT_FWRAPPER_CREATE2_RP2(int, int, 0, 0,
                          bcmlt_table_unsubscribe, \
                          int,int,unit,0,0,CINT_PARAM_IN,
                          const char *,char,table_name,1,0,CINT_PARAM_IN);
#if 0 /* Only declaration present in bcmlt.h but no definition */
CINT_FWRAPPER_CREATE2_RP3(int, int, 0, 0,
                          bcmlt_table_num_of_entries_get, \
                          int,int,unit,0,0,CINT_PARAM_IN,
                          const char*,char,table_name,1,0,CINT_PARAM_IN,
                          size_t*,size_t,num,0,0,CINT_PARAM_OUT);
#endif
CINT_FWRAPPER_CREATE2_RP2(int, int, 0, 0,
                          bcmlt_transaction_allocate, \
                          bcmlt_trans_type_t,bcmlt_trans_type_t,type,0,0,CINT_PARAM_IN,
                          bcmlt_transaction_hdl_t *,bcmlt_transaction_hdl_t,trans_hdl_p,1,0,CINT_PARAM_OUT);
CINT_FWRAPPER_CREATE2_RP1(int, int, 0, 0,
                          bcmlt_transaction_free, \
                          bcmlt_transaction_hdl_t,bcmlt_transaction_hdl_t,trans_hdl,0,0,CINT_PARAM_IN);
CINT_FWRAPPER_CREATE2_RP3(int, int, 0, 0,
                          bcmlt_transaction_entry_add, \
                          bcmlt_transaction_hdl_t,bcmlt_transaction_hdl_t,trans_hdl,0,0,CINT_PARAM_IN,
                          bcmlt_opcode_t,bcmlt_opcode_t,opcode,0,0,CINT_PARAM_IN,
                          bcmlt_entry_handle_t,bcmlt_entry_handle_t,entry_hdl,0,0,CINT_PARAM_IN);
CINT_FWRAPPER_CREATE2_RP3(int, int, 0, 0,
                          bcmlt_transaction_pt_entry_add, \
                          bcmlt_transaction_hdl_t,bcmlt_transaction_hdl_t,trans_hdl,0,0,CINT_PARAM_IN,
                          bcmlt_pt_opcode_t,bcmlt_pt_opcode_t,opcode,0,0,CINT_PARAM_IN,
                          bcmlt_entry_handle_t,bcmlt_entry_handle_t,entry_hdl,0,0,CINT_PARAM_IN);
CINT_FWRAPPER_CREATE2_RP2(int, int, 0, 0,
                          bcmlt_transaction_entry_delete, \
                          bcmlt_transaction_hdl_t,bcmlt_transaction_hdl_t,trans_hdl,0,0,CINT_PARAM_IN,
                          bcmlt_entry_handle_t,bcmlt_entry_handle_t,entry_hdl,0,0,CINT_PARAM_IN);
CINT_FWRAPPER_CREATE2_RP2(int, int, 0, 0,
                          bcmlt_transaction_info_get, \
                          bcmlt_transaction_hdl_t,bcmlt_transaction_hdl_t,trans_hdl,0,0,CINT_PARAM_IN,
                          bcmlt_transaction_info_t*,bcmlt_transaction_info_t,trans_info_p,1,0,CINT_PARAM_OUT);
CINT_FWRAPPER_CREATE2_RP2(int, int, 0, 0,
                          bcmlt_transaction_commit, \
                          bcmlt_transaction_hdl_t,bcmlt_transaction_hdl_t,trans_hdl,0,0,CINT_PARAM_IN,
                          bcmlt_priority_level_t,bcmlt_priority_level_t,priority,0,0,CINT_PARAM_IN);
CINT_FWRAPPER_CREATE2_RP3(int, int, 0, 0,
                          bcmlt_transaction_entry_num_get, \
                          bcmlt_transaction_hdl_t,bcmlt_transaction_hdl_t,trans_hdl,0,0,CINT_PARAM_IN,
                          uint32_t,uint32_t,entry_num,0,0,CINT_PARAM_IN,
                          bcmlt_entry_info_t*,bcmlt_entry_info_t,entry_info_p,1,0,CINT_PARAM_OUT);
CINT_FWRAPPER_CREATE2_RP2(int, int, 0, 0,
                          bcmlt_entry_oper_get, \
                          bcmlt_entry_handle_t,bcmlt_entry_handle_t,entry_hdl,0,0,CINT_PARAM_IN,
                          bcmlt_unified_opcode_t*,bcmlt_unified_opcode_t,uni_opcode,1,0,CINT_PARAM_OUT);
CINT_FWRAPPER_CREATE2_RP2(int, int, 0, 0,
                          bcmlt_entry_field_count, \
                          bcmlt_entry_handle_t,bcmlt_entry_handle_t,entry_hdl,0,0,CINT_PARAM_IN,
                          uint32_t*,uint32_t,field_count,1,0,CINT_PARAM_OUT);
CINT_FWRAPPER_CREATE2_RP3(int, int, 0, 0,
                          bcmlt_table_pt_name_max_num_get,
                          int,int,unit,0,0,CINT_PARAM_IN,
                          const char *,char,tbl_name,1,0,CINT_PARAM_IN,
                          uint32_t *,uint32_t,max_pt_count,1,0,CINT_PARAM_OUT);
CINT_FWRAPPER_CREATE2_RP5(int, int, 0, 0,
                          bcmlt_table_pt_name_get,
                          int,int,unit,0,0,CINT_PARAM_IN,
                          const char *,char,tbl_name,1,0,CINT_PARAM_IN,
                          uint32_t,uint32_t,pt_max_count,0,0,CINT_PARAM_IN,
                          const char **,char,pt_names,2,0,CINT_PARAM_OUT,
                          uint32_t *,uint32_t,actual_pt_count,1,0,CINT_PARAM_OUT);
CINT_FWRAPPER_CREATE2_RP5(int, int, 0, 0,
                          bcmlt_table_field_id_get_by_name,
                          int,int,unit,0,0,CINT_PARAM_IN,
                          const char *,char,tbl_name,1,0,CINT_PARAM_IN,
                          const char *,char,fld_name,1,0,CINT_PARAM_IN,
                          uint32_t *,uint32_t,ltid,1,0,CINT_PARAM_OUT,
                          uint32_t *,uint32_t,lfid,1,0,CINT_PARAM_OUT);

static cint_function_t bcma_cint_functions[] =
{
    CINT_FWRAPPER_ENTRY(bcmlt_entry_commit),
    CINT_FWRAPPER_ENTRY(bcmlt_pt_entry_commit),
    CINT_FWRAPPER_ENTRY(bcmlt_table_names_get_first),
    CINT_FWRAPPER_ENTRY(bcmlt_table_names_get_next),
    CINT_FWRAPPER_ENTRY(bcmlt_table_desc_get),
    CINT_FWRAPPER_ENTRY(bcmlt_table_field_defs_get),
    CINT_FWRAPPER_ENTRY(bcmlt_device_attach),
    CINT_FWRAPPER_ENTRY(bcmlt_device_detach),
    CINT_FWRAPPER_ENTRY(bcmlt_api_clear),
    CINT_FWRAPPER_ENTRY(bcmlt_device_attached),
    CINT_FWRAPPER_ENTRY(bcmlt_entry_field_add),
    CINT_FWRAPPER_ENTRY(bcmlt_entry_field_add_by_id),
    CINT_FWRAPPER_ENTRY(bcmlt_entry_field_unset),
    CINT_FWRAPPER_ENTRY(bcmlt_entry_field_symbol_add),
    CINT_FWRAPPER_ENTRY(bcmlt_entry_field_symbol_add_by_id),
    CINT_FWRAPPER_ENTRY(bcmlt_entry_field_get),
    CINT_FWRAPPER_ENTRY(bcmlt_entry_field_get_by_id),
    CINT_FWRAPPER_ENTRY(bcmlt_entry_field_symbol_get),
    CINT_FWRAPPER_ENTRY(bcmlt_entry_field_symbol_get_by_id),
    CINT_FWRAPPER_ENTRY(bcmlt_entry_field_array_add),
    CINT_FWRAPPER_ENTRY(bcmlt_entry_field_array_add_by_id),
    CINT_FWRAPPER_ENTRY(bcmlt_entry_field_array_remove),
    CINT_FWRAPPER_ENTRY(bcmlt_entry_field_array_symbol_add),
    CINT_FWRAPPER_ENTRY(bcmlt_entry_field_array_sym_add_by_id),
    CINT_FWRAPPER_ENTRY(bcmlt_entry_field_array_get),
    CINT_FWRAPPER_ENTRY(bcmlt_entry_field_array_get_by_id),
    CINT_FWRAPPER_ENTRY(bcmlt_entry_field_array_symbol_get),
    CINT_FWRAPPER_ENTRY(bcmlt_entry_field_array_sym_get_by_id),
    CINT_FWRAPPER_ENTRY(bcmlt_entry_field_array_unset),
    CINT_FWRAPPER_ENTRY(bcmlt_entry_field_remove),
    CINT_FWRAPPER_ENTRY(bcmlt_entry_field_symbol_info_get),
    CINT_FWRAPPER_ENTRY(bcmlt_entry_info_get),
    CINT_FWRAPPER_ENTRY(bcmlt_entry_allocate),
    CINT_FWRAPPER_ENTRY(bcmlt_entry_allocate_by_id),
    CINT_FWRAPPER_ENTRY(bcmlt_entry_free),
#if 0 /* Only declaration present in bcmlt.h but no definition */
    CINT_FWRAPPER_ENTRY(bcmlt_entry_field_info_get),
#endif
    CINT_FWRAPPER_ENTRY(bcmlt_entry_clear),
    CINT_FWRAPPER_ENTRY(bcmlt_entry_attrib_get),
    CINT_FWRAPPER_ENTRY(bcmlt_entry_attrib_set),
    CINT_FWRAPPER_ENTRY(bcmlt_entry_attrib_clear),
    CINT_FWRAPPER_ENTRY(bcmlt_table_subscribe),
    CINT_FWRAPPER_ENTRY(bcmlt_table_unsubscribe),
#if 0 /* Only declaration present in bcmlt.h but no definition */
    CINT_FWRAPPER_ENTRY(bcmlt_table_num_of_entries_get),
#endif
    CINT_FWRAPPER_ENTRY(bcmlt_transaction_allocate),
    CINT_FWRAPPER_ENTRY(bcmlt_transaction_free),
    CINT_FWRAPPER_ENTRY(bcmlt_transaction_entry_add),
    CINT_FWRAPPER_ENTRY(bcmlt_transaction_pt_entry_add),
    CINT_FWRAPPER_ENTRY(bcmlt_transaction_entry_delete),
    CINT_FWRAPPER_ENTRY(bcmlt_transaction_info_get),
    CINT_FWRAPPER_ENTRY(bcmlt_transaction_commit),
    CINT_FWRAPPER_ENTRY(bcmlt_transaction_entry_num_get),
    CINT_FWRAPPER_ENTRY(bcmlt_entry_oper_get),
    CINT_FWRAPPER_ENTRY(bcmlt_entry_field_count),
    CINT_FWRAPPER_ENTRY(bcmlt_table_pt_name_max_num_get),
    CINT_FWRAPPER_ENTRY(bcmlt_table_pt_name_get),
    CINT_FWRAPPER_ENTRY(bcmlt_table_field_id_get_by_name),
    { NULL }
};

static void*
bcma_cint_maddr_bcmlt_field_def_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    bcmlt_field_def_t* s = (bcmlt_field_def_t*) p;

    switch(mnum)
    {
        case 0: rv = &(s->name); break;
        case 1: rv = &(s->access); break;
        case 2: rv = &(s->min); break;
        case 3: rv = &(s->max); break;
        case 4: rv = &(s->def); break;
        case 5: rv = &(s->dtag); break;
        case 6: rv = &(s->width); break;
        case 7: rv = &(s->depth); break;
        case 8: rv = &(s->elements); break;
        case 9: rv = &(s->key); break;
        case 10: rv = &(s->symbol); break;
        default: rv = NULL; break;
    }

    return rv;
}

static void*
bcma_cint_maddr_bcmltd_binary_field_data_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    bcmltd_binary_field_data_t* s = (bcmltd_binary_field_data_t*) p;

    switch(mnum)
    {
        case 0: rv = &(s->num_of_bits); break;
        case 1: rv = &(s->bytes); break;
        default: rv = NULL; break;
    }

    return rv;
}

static void*
bcma_cint_maddr_bcmlt_entry_info_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    bcmlt_entry_info_t* s = (bcmlt_entry_info_t*) p;

    switch(mnum)
    {
        case 0: rv = &(s->unit); break;
        case 1: rv = &(s->table_name); break;
        case 2: rv = &(s->entry_hdl); break;
        case 3: rv = &(s->notif_opt); break;
        case 4: rv = &(s->status); break;
        default: rv = NULL; break;
    }

    return rv;
}

static void*
bcma_cint_maddr_bcmlt_table_notif_info_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    bcmlt_table_notif_info_t* s = (bcmlt_table_notif_info_t*) p;

    switch(mnum)
    {
        case 0: rv = &(s->unit); break;
        case 1: rv = &(s->table_name); break;
        case 2: rv = &(s->entry_hdl); break;
        default: rv = NULL; break;
    }

    return rv;
}

static void*
bcma_cint_maddr_bcmlt_transaction_info_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    bcmlt_transaction_info_t* s = (bcmlt_transaction_info_t*) p;

    switch(mnum)
    {
        case 0: rv = &(s->trans_hdl); break;
        case 1: rv = &(s->type); break;
        case 2: rv = &(s->notif_opt); break;
        case 3: rv = &(s->status); break;
        case 4: rv = &(s->num_entries); break;
        default: rv = NULL; break;
    }

    return rv;
}

static void*
bcma_cint_maddr_bcmlt_unified_opcode_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    bcmlt_unified_opcode_t* s = (bcmlt_unified_opcode_t*) p;

    switch(mnum)
    {
        case 0: rv = &(s->pt); break;
        case 1: rv = &(s->opcode); break;
        default: rv = NULL; break;
    }

    return rv;
}

static cint_parameter_desc_t bcma_cint_struct_members_bcmlt_field_def_t[] =
{
    {
        "char",
        "name",
        1,
        0
    },
    {
        "bcmlt_field_acc_t",
        "access",
        0,
        0
    },
    {
        "uint64_t",
        "min",
        0,
        0
    },
    {
        "uint64_t",
        "max",
        0,
        0
    },
    {
        "uint64_t",
        "def",
        0,
        0
    },
    {
        "bcmlt_field_data_tag_t",
        "dtag",
        0,
        0
    },
    {
        "uint32_t",
        "width",
        0,
        0
    },
    {
        "uint32_t",
        "depth",
        0,
        0
    },
    {
        "uint32_t",
        "elements",
        0,
        0
    },
    {
        "bool",
        "key",
        0,
        0
    },
    {
        "bool",
        "array",
        0,
        0
    },
    {
        "bool",
        "symbol",
        0,
        0
    },
    { NULL }
};

static cint_parameter_desc_t bcma_cint_struct_members_bcmltd_binary_field_data_t[] =
{
    {
        "uint32_t",
        "num_of_bits",
        0,
        0
    },
    {
        "uint8_t",
        "bytes",
        1,
        0
    },
    { NULL }
};

static cint_parameter_desc_t bcma_cint_struct_members_bcmlt_entry_info_t[] =
{
    {
        "int",
        "unit",
        0,
        0
    },
    {
        "char",
        "table_name",
        1,
        0
    },
    {
        "bcmlt_entry_handle_t",
        "entry_hdl",
        0,
        0
    },
    {
        "bcmlt_notif_option_t",
        "notif_opt",
        0,
        0
    },
    {
        "shr_error_t",
        "status",
        0,
        0
    },
    { NULL }
};

static cint_parameter_desc_t bcma_cint_struct_members_bcmlt_table_notif_info_t[] =
{
    {
        "int",
        "unit",
        0,
        0
    },
    {
        "char",
        "table_name",
        1,
        0
    },
    {
        "bcmlt_entry_handle_t",
        "entry_hdl",
        0,
        0
    },
    { NULL }
};

static cint_parameter_desc_t bcma_cint_struct_members_bcmlt_transaction_info_t[] =
{
    {
        "bcmlt_transaction_hdl_t",
        "trans_hdl",
        0,
        0
    },
    {
        "bcmlt_trans_type_t",
        "type",
        0,
        0
    },
    {
        "bcmlt_notif_option_t",
        "notif_opt",
        0,
        0
    },
    {
        "shr_error_t",
        "status",
        0,
        0
    },
    {
        "uint32_t",
        "num_entries",
        0,
        0
    },
    { NULL }
};

static cint_parameter_desc_t bcma_cint_struct_members_bcmlt_unified_opcode_t[] =
{
    {
        "_Bool",
        "pt",
        0,
        0
    },
    {
        "int",
        "opcode",
        0,
        0
    },
    { NULL }
};

static cint_struct_type_t bcma_cint_structures[] =
{
    {
        "bcmlt_field_def_t",
        sizeof(bcmlt_field_def_t),
        bcma_cint_struct_members_bcmlt_field_def_t,
        bcma_cint_maddr_bcmlt_field_def_t
    },
    {
        "bcmltd_binary_field_data_t",
        sizeof(bcmltd_binary_field_data_t),
        bcma_cint_struct_members_bcmltd_binary_field_data_t,
        bcma_cint_maddr_bcmltd_binary_field_data_t
    },
    {
        "bcmlt_entry_info_t",
        sizeof(bcmlt_entry_info_t),
        bcma_cint_struct_members_bcmlt_entry_info_t,
        bcma_cint_maddr_bcmlt_entry_info_t
    },
    {
        "bcmlt_table_notif_info_t",
        sizeof(bcmlt_table_notif_info_t),
        bcma_cint_struct_members_bcmlt_table_notif_info_t,
        bcma_cint_maddr_bcmlt_table_notif_info_t
    },
    {
        "bcmlt_transaction_info_t",
        sizeof(bcmlt_transaction_info_t),
        bcma_cint_struct_members_bcmlt_transaction_info_t,
        bcma_cint_maddr_bcmlt_transaction_info_t
    },
    {
        "bcmlt_unified_opcode_t",
        sizeof(bcmlt_unified_opcode_t),
        bcma_cint_struct_members_bcmlt_unified_opcode_t,
        bcma_cint_maddr_bcmlt_unified_opcode_t
    },
    { NULL }
};

static cint_enum_map_t bcma_cint_enum_map_bcmltd_field_data_tag_t[] =
{
    { "BCMLT_FIELD_DATA_TAG_UNDEFINED", BCMLT_FIELD_DATA_TAG_UNDEFINED },
    { "BCMLT_FIELD_DATA_TAG_BOOL", BCMLT_FIELD_DATA_TAG_BOOL },
    { "BCMLT_FIELD_DATA_TAG_U8", BCMLT_FIELD_DATA_TAG_U8 },
    { "BCMLT_FIELD_DATA_TAG_U16", BCMLT_FIELD_DATA_TAG_U16 },
    { "BCMLT_FIELD_DATA_TAG_U32", BCMLT_FIELD_DATA_TAG_U32 },
    { "BCMLT_FIELD_DATA_TAG_U64", BCMLT_FIELD_DATA_TAG_U64 },
    { "BCMLT_FIELD_DATA_TAG_RAW", BCMLT_FIELD_DATA_TAG_RAW },
    { "BCMLT_FIELD_DATA_TAG_NUM", BCMLT_FIELD_DATA_TAG_NUM },
    { NULL }
};

static cint_enum_map_t bcma_cint_enum_map_bcmltd_field_acc_t[] =
{
    { "BCMLT_FIELD_ACCESS_READ_WRITE", BCMLT_FIELD_ACCESS_READ_WRITE },
    { "BCMLT_FIELD_ACCESS_READ_ONLY", BCMLT_FIELD_ACCESS_READ_ONLY },
    { NULL }
};

static cint_enum_map_t bcma_cint_enum_map_bcmlt_notif_option_t[] =
{
    { "BCMLT_NOTIF_OPTION_NO_NOTIF", BCMLT_NOTIF_OPTION_NO_NOTIF },
    { "BCMLT_NOTIF_OPTION_COMMIT", BCMLT_NOTIF_OPTION_COMMIT },
    { "BCMLT_NOTIF_OPTION_HW", BCMLT_NOTIF_OPTION_HW },
    { "BCMLT_NOTIF_OPTION_ALL", BCMLT_NOTIF_OPTION_ALL },
    { "BCMLT_NOTIF_OPTION_NUM", BCMLT_NOTIF_OPTION_NUM },
    { NULL }
};

static cint_enum_map_t bcma_cint_enum_map_shr_error_t[] =
{
    { "SHR_E_NONE",  SHR_E_NONE },
    { "SHR_E_INTERNAL",  SHR_E_INTERNAL },
    { "SHR_E_MEMORY",  SHR_E_MEMORY },
    { "SHR_E_UNIT",  SHR_E_UNIT },
    { "SHR_E_PARAM",  SHR_E_PARAM },
    { "SHR_E_EMPTY",  SHR_E_EMPTY },
    { "SHR_E_FULL",  SHR_E_FULL },
    { "SHR_E_NOT_FOUND",  SHR_E_NOT_FOUND },
    { "SHR_E_EXISTS",  SHR_E_EXISTS },
    { "SHR_E_TIMEOUT",  SHR_E_TIMEOUT },
    { "SHR_E_BUSY",  SHR_E_BUSY },
    { "SHR_E_FAIL",  SHR_E_FAIL },
    { "SHR_E_DISABLED",  SHR_E_DISABLED },
    { "SHR_E_BADID",  SHR_E_BADID },
    { "SHR_E_RESOURCE",  SHR_E_RESOURCE },
    { "SHR_E_CONFIG",  SHR_E_CONFIG },
    { "SHR_E_UNAVAIL",  SHR_E_UNAVAIL },
    { "SHR_E_INIT",  SHR_E_INIT },
    { "SHR_E_PORT",  SHR_E_PORT },
    { "SHR_E_IO",  SHR_E_IO },
    { "SHR_E_ACCESS",  SHR_E_ACCESS },
    { "SHR_E_NO_HANDLER",  SHR_E_NO_HANDLER },
    { "SHR_E_PARTIAL",  SHR_E_PARTIAL },

    { "SHR_E_LIMIT",  SHR_E_LIMIT },
    { NULL }
};

static cint_enum_map_t bcma_cint_enum_map_bcmlt_priority_level_t[] =
{
    { "BCMLT_PRIORITY_NORMAL", BCMLT_PRIORITY_NORMAL },
    { "BCMLT_PRIORITY_HIGH", BCMLT_PRIORITY_HIGH },
    { NULL }
};

static cint_enum_map_t bcma_cint_enum_map_bcmlt_opcode_t[] =
{
    { "BCMLT_OPCODE_NOP", BCMLT_OPCODE_NOP },
    { "BCMLT_OPCODE_INSERT", BCMLT_OPCODE_INSERT },
    { "BCMLT_OPCODE_LOOKUP", BCMLT_OPCODE_LOOKUP },
    { "BCMLT_OPCODE_DELETE", BCMLT_OPCODE_DELETE },
    { "BCMLT_OPCODE_UPDATE", BCMLT_OPCODE_UPDATE },
    { "BCMLT_OPCODE_TRAVERSE", BCMLT_OPCODE_TRAVERSE },
    { "BCMLT_OPCODE_NUM", BCMLT_OPCODE_NUM },
    { NULL }
};

static cint_enum_map_t bcma_cint_enum_map_bcmlt_pt_opcode_t[] =
{
    { "BCMLT_PT_OPCODE_NOP", BCMLT_PT_OPCODE_NOP },
    { "BCMLT_PT_OPCODE_FIFO_POP", BCMLT_PT_OPCODE_FIFO_POP },
    { "BCMLT_PT_OPCODE_FIFO_PUSH", BCMLT_PT_OPCODE_FIFO_PUSH },
    { "BCMLT_PT_OPCODE_SET", BCMLT_PT_OPCODE_SET },
    { "BCMLT_PT_OPCODE_MODIFY", BCMLT_PT_OPCODE_MODIFY },
    { "BCMLT_PT_OPCODE_GET", BCMLT_PT_OPCODE_GET },
    { "BCMLT_PT_OPCODE_CLEAR", BCMLT_PT_OPCODE_CLEAR },
    { "BCMLT_PT_OPCODE_INSERT", BCMLT_PT_OPCODE_INSERT },
    { "BCMLT_PT_OPCODE_DELETE", BCMLT_PT_OPCODE_DELETE },
    { "BCMLT_PT_OPCODE_LOOKUP", BCMLT_PT_OPCODE_LOOKUP },
    { "BCMLT_PT_OPCODE_NUM", BCMLT_PT_OPCODE_NUM },
    { NULL }
};

static cint_enum_map_t bcma_cint_enum_map_bcmlt_trans_type_t[] =
{
    { "BCMLT_TRANS_TYPE_ATOMIC", BCMLT_TRANS_TYPE_ATOMIC },
    { "BCMLT_TRANS_TYPE_BATCH", BCMLT_TRANS_TYPE_BATCH },
    { "BCMLT_TRANS_TYPE_NUM", BCMLT_TRANS_TYPE_NUM },
    { NULL }
};

static cint_enum_map_t bcma_cint_enum_map_bcmlt_status_t[] =
{
    { "BCMLT_STATUS_NOT_HANDLED", BCMLT_STATUS_NOT_HANDLED },
    { "BCMLT_STATUS_SUCCESS", BCMLT_STATUS_SUCCESS },
    { "BCMLT_STATUS_PART_SUCCESS", BCMLT_STATUS_PART_SUCCESS },
    { "BCMLT_STATUS_NOTALLOWED", BCMLT_STATUS_NOTALLOWED },
    { "BCMLT_STATUS_NORESOURCE", BCMLT_STATUS_NORESOURCE },
    { "BCMLT_STATUS_INVALID", BCMLT_STATUS_INVALID },
    { "BCMLT_STATUS_HWERROR", BCMLT_STATUS_HWERROR },
    { "BCMLT_STATUS_NUM", BCMLT_STATUS_NUM},
    { NULL }
};

static cint_enum_type_t bcma_cint_enums[] =
{
    { "bcmltd_field_data_tag_t", bcma_cint_enum_map_bcmltd_field_data_tag_t },
    { "bcmltd_field_acc_t", bcma_cint_enum_map_bcmltd_field_acc_t },
    { "bcmlt_notif_option_t", bcma_cint_enum_map_bcmlt_notif_option_t },
    { "shr_error_t", bcma_cint_enum_map_shr_error_t },
    { "bcmlt_priority_level_t", bcma_cint_enum_map_bcmlt_priority_level_t },
    { "bcmlt_opcode_t", bcma_cint_enum_map_bcmlt_opcode_t },
    { "bcmlt_pt_opcode_t", bcma_cint_enum_map_bcmlt_pt_opcode_t },
    { "bcmlt_trans_type_t", bcma_cint_enum_map_bcmlt_trans_type_t },
    { "bcmlt_status_t", bcma_cint_enum_map_bcmlt_status_t },
    { NULL }
};

static cint_parameter_desc_t bcma_cint_typedefs[] =
{
    {
        "uint32_t",
        "bcmlt_entry_handle_t",
        0,
        0
    },
    {
        "void",
        "bcmlt_transaction_hdl_t",
        1,
        0
    },
    {
        "bcmltd_field_data_tag_t",
        "bcmlt_field_data_tag_t",
        0,
        0
    },
    {
        "bcmltd_field_acc_t",
        "bcmlt_field_acc_t",
        0,
        0
    },
    { NULL }
};

static cint_constants_t bcma_cint_constants[] =
{
    { "BCMLT_FIELD_ACCESS_READ_WRITE", BCMLT_FIELD_ACCESS_READ_WRITE },
    { "BCMLT_FIELD_ACCESS_READ_ONLY", BCMLT_FIELD_ACCESS_READ_ONLY },
    { "BCMLT_ENT_ATTR_GET_FROM_HW", BCMLT_ENT_ATTR_GET_FROM_HW },
    { "BCMLT_ENT_ATTR_FILTER_DEFAULTS", BCMLT_ENT_ATTR_FILTER_DEFAULTS },
    { "BCMLT_TABLE_SEARCH_PT", BCMLT_TABLE_SEARCH_PT },
    { "BCMLT_TABLE_SEARCH_LT", BCMLT_TABLE_SEARCH_LT },
    { "BCMLT_TABLE_SEARCH_INTERACTIVE", BCMLT_TABLE_SEARCH_INTERACTIVE },
    { "BCMLT_INVALID_HDL", BCMLT_INVALID_HDL },
    { NULL }
};

static cint_function_pointer_t bcma_cint_function_pointers[2];

static void
bcma_cint_fpointer_bcmlt_table_sub_cb(bcmlt_table_notif_info_t *table_notify_info,
        void *user_data)
{
    cint_interpreter_callback(bcma_cint_function_pointers+0, 2, 0, &table_notify_info, &user_data);

}

static cint_parameter_desc_t bcma_cint_parameters_bcmlt_table_sub_cb[] =
{
    { "void", "r", 0, 0 },
    {
        "bcmlt_table_notif_info_t",
        "table_notify_info",
        1,
        0
    },
    {
        "void",
        "user_data",
        1,
        0
    },
    { NULL }
};

static cint_function_pointer_t bcma_cint_function_pointers[] =
{
    {
        "bcmlt_table_sub_cb",
        (cint_fpointer_t) bcma_cint_fpointer_bcmlt_table_sub_cb,
        bcma_cint_parameters_bcmlt_table_sub_cb,
    },
    { NULL }
};

cint_data_t bcma_cint_data =
{
    NULL,
    bcma_cint_functions,
    bcma_cint_structures,
    bcma_cint_enums,
    bcma_cint_typedefs,
    bcma_cint_constants,
    bcma_cint_function_pointers,
};

