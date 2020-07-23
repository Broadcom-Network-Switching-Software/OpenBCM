/****************************************************************
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    diag_signals_dump.c
 */

#define DSP_KERNEL_DEF

#include <shared/bsl.h>

#ifdef BCM_PETRA_SUPPORT
#include <sal/types.h>
#include <sal/core/libc.h>
#include <sal/core/dpc.h>
#include <sal/appl/sal.h>
#include <sal/appl/io.h>

#include <appl/diag/dcmn/diag_signals_dump.h>
#include <appl/diag/shell.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/diag.h>
#include <appl/diag/dcmn/diag.h>

#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/PPD/ppd_api_diag.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_diag.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>


/********************************************************************
 * this struct holds all data, and is used to output the resolved data
 * based on the resolved_data struct from the adapter
 */
typedef struct  {

  /* [15:   0] */ char      p2_src_system_port[4 + 1];                                              /* addr_high 9 addr_low 0 msb 55 lsb 40 block_id 1  */
  /* [31:  16] */ char      p2_dst_system_port[4 + 1];                                              /* addr_high 9 addr_low 2 msb 157 lsb 142 block_id 1*/
  /* [39:  32] */ char      p2_nwk_headers_size[2 + 1];         /* msb is pad with '0'                 addr_high 9 addr_low 1 msb 64 lsb 58 block_id 1  */
  /* [47:  40] */ char      p2_sys_headers_size[2 + 1];         /* msb is pad with '0'                 addr_high 9 addr_low 0 msb 31 lsb 24 block_id 1  */
  /* [103: 48] */ char      p2_ees_data_action1[14 + 1];        /* 56 bits                             addr_high 9 addr_low 2 msb 140 lsb 85 block_id 1 */
  /* [159:104] */ char      p2_ees_data_action2[14 + 1];        /* 56 bits                             addr_high 9 addr_low 2 msb 84 lsb 29 block_id 1  */
  /* [223:160] */ char      p2_prge_program_variable[16 + 1];                                       
  /* [231:224] */ char      p2_out_tm_port[2 + 1];                                                  /* addr_high 9 addr_low 0 msb 253 lsb 246 block_id 1*/
  /* [239:232] */ char      p2_out_pp_port[2 + 1];                                                  /* addr_high 9 addr_low 0 msb 255 lsb 254 block_id 1 ,  addr_high 9 addr_low 1 msb 5 lsb 0 block_id 1   */
  /* [271:240] */ char      p2_oam_counter_value[8 + 1];                                            /* addr_high 9 addr_low 1 msb 57 lsb 26 block_id 1  */
  /* [291:272] */ char      p2_oam_id[5 + 1];                   /* 20 bits - 3 msb are pad with '0'    addr_high 9 addr_low 1 msb 25 lsb 9 block_id 1   */
  /* [295:292] */ char      oam_sub_type[1 + 1];                /* 4 bits- msb is pad with '0'         addr_high 9 addr_low 1 msb 8 lsb 6 block_id 1    */
  /* [359:296] */ char      term2prge_tod_1588[16 + 1];                                             /* addr_high 6 addr_low 0 msb 127 lsb 64 block_id 1 */
  /* [423:360] */ char      term2prge_tod_ntp[16 + 1];                                              /* addr_high 6 addr_low 0 msb 63 lsb 0 block_id 1   */
  /* [447:424] */ char      p2_vsi_variable[6 + 1];             /* 24 bits                             addr_high 9 addr_low 0 msb 23 lsb 0 block_id 1   */
  /* [455:448] */ char      p2_ace_variable[2 + 1];                                                 /* addr_high 9 addr_low 2 msb 166 lsb 159 block_id 1*/
  /* [471:456] */ char      p2_prge_out_lif[4 + 1];                                                 /* addr_high 8 addr_low 0 msb 65 lsb 50 block_id 1  */
  /* [503:472] */ char      p2_prge_tm_port_var[8 + 1];                                             /* addr_high 9 addr_low 0 msb 98 lsb 67 block_id 1  */
  /* [535:504] */ char      p2_prge_pp_port_var[8 + 1];                                             /* addr_high 9 addr_low 0 msb 134 lsb 103 block_id 1*/
  /* [552:536] */ char      p2_oam_id_and_oam_sub_type[5 + 1];  /* 17+3  bits                          oam_id | (oam_sub_type<<17)                      */
} resolved_data_s ;

/*********************************************************************
 *     Global variables
 */
resolved_data_s
    resolved_data_struct,
    *resolved_data = &resolved_data_struct; 

char 
    buffer[200],                                /*buffer will hold the information printed to the output file*/
    buffer_copy[200];

#ifndef NO_FILEIO
FILE
    *ofp;
#endif

uint32
    regs_val[ARAD_PP_DIAG_DBG_VAL_LEN];

ARAD_PP_EPNI_PRGE_PROGRAM_TBL_DATA
    tbl_data;

/*********************************************************************
 *     Creates a dump file of the packet signals,
 *     parsed to match the verilog format.
 *     Currently set to match ARAD & ARAD+
 */

cmd_result_t
  cmd_dpp_diag_parsed_signals_get(
      int unit,
      int core_id,
      char * filename) 
{
#ifndef NO_FILEIO

  int
      line_index = 298916,
      is_stdout  = 0;
  cmd_result_t
      res = CMD_OK;
#endif

#ifdef NO_FILEIO
    sal_printf("ERROR: I/O is not allowed!\n");
    return CMD_FAIL;
#else
  if ((filename != NULL) && sal_strcmp(filename,"stdout")) {
      ofp = sal_fopen(filename, "w"); 
  }

  else if (filename == NULL) {
      ofp = sal_fopen("diag_parsed_signals.txt", "w"); 
  } 

  else{
      ofp = NULL; 
      is_stdout = 1;
  }

  if ((is_stdout == 0) && (ofp == NULL)){
        res = CMD_FAIL;
        sal_printf("ERROR: Failed to open file.\n");
        return res; 
  }

  res= cmd_dpp_diag_psd_get_table_data_for_signals_dump(unit, core_id);
  IF_CMD_ERROR_GOTO_EXIT(res, "cmd_dpp_diag_psd_get_table_data_for_signals_dump");

  PRINT_TO_TARGET(ofp,("%d: >>>>>>>   Link2Prge packet# 00 \n%d-",line_index, 1+line_index),
                  (ofp,"%d: >>>>>>>   Link2Prge packet# 00 \n%d-",line_index, 1+line_index))
  line_index++;

  res= cmd_dpp_diag_psd_get_registers(unit, core_id, ++line_index );
  IF_CMD_ERROR_GOTO_EXIT(res, "cmd_dpp_diag_psd_get_registers");

  /*unprinted info -start*/
  res= cmd_dpp_diag_psd_parse_signal(core_id, line_index    ,   "term2prge_tod_1588"     , 6, 0, 127, 64, 0);
  IF_CMD_ERROR_GOTO_EXIT(res, "cmd_dpp_diag_psd_parse_signal");
  sal_strncpy(resolved_data->term2prge_tod_1588, buffer, sizeof(resolved_data->term2prge_tod_1588));
  resolved_data->term2prge_tod_1588[sizeof(resolved_data->term2prge_tod_1588) - 1] = '\0';

  res= cmd_dpp_diag_psd_parse_signal(core_id, line_index   ,   "term2prge_tod_ntp"     , 6, 0, 63, 0, 0);
  IF_CMD_ERROR_GOTO_EXIT(res, "cmd_dpp_diag_psd_parse_signal");

  sal_strncpy(resolved_data->term2prge_tod_ntp, buffer, sizeof(resolved_data->term2prge_tod_ntp));
  resolved_data->term2prge_tod_ntp[sizeof(resolved_data->term2prge_tod_ntp) - 1] = '\0';

  /*end*/

  res= cmd_dpp_diag_psd_get_gen_header(core_id, ++line_index);
  IF_CMD_ERROR_GOTO_EXIT(res, "cmd_dpp_diag_psd_get_gen_header");

  res= cmd_dpp_diag_psd_parse_signal(core_id,  ++line_index,   "sys_headers_size" , 9, 0, 31, 25, 1 );
  IF_CMD_ERROR_GOTO_EXIT(res, "cmd_dpp_diag_psd_parse_signal");
  sal_strncpy(resolved_data->p2_sys_headers_size, buffer, sizeof(resolved_data->p2_sys_headers_size));
  resolved_data->p2_sys_headers_size[sizeof(resolved_data->p2_sys_headers_size) - 1] = '\0';

  res= cmd_dpp_diag_psd_parse_signal(core_id,  ++line_index,   "nwk_headers_size" ,9 ,1 ,64, 58, 1  );
  IF_CMD_ERROR_GOTO_EXIT(res, "cmd_dpp_diag_psd_parse_signal");
  sal_strncpy(resolved_data->p2_nwk_headers_size, buffer, sizeof(resolved_data->p2_nwk_headers_size));
  resolved_data->p2_nwk_headers_size[sizeof(resolved_data->p2_nwk_headers_size) - 1] = '\0';

  res= cmd_dpp_diag_psd_parse_signal(core_id,  ++line_index,   "ees_data_action1" , 9, 2, 140, 85, 1);
  IF_CMD_ERROR_GOTO_EXIT(res, "cmd_dpp_diag_psd_parse_signal");
  sal_strncpy(resolved_data->p2_ees_data_action1, buffer, sizeof(resolved_data->p2_ees_data_action1));
  resolved_data->p2_ees_data_action1[sizeof(resolved_data->p2_ees_data_action1) - 1] = '\0';

  res= cmd_dpp_diag_psd_parse_signal(core_id,  ++line_index,   "ees_data_action2" , 9, 2, 84, 29, 1 );
  IF_CMD_ERROR_GOTO_EXIT(res, "cmd_dpp_diag_psd_parse_signal");
  sal_strncpy(resolved_data->p2_ees_data_action2, buffer, sizeof(resolved_data->p2_ees_data_action2));
  resolved_data->p2_ees_data_action2[sizeof(resolved_data->p2_ees_data_action2) - 1] = '\0';

  res= cmd_dpp_diag_psd_parse_signal(core_id,  ++line_index,   "prge_tm_port_var" , 9, 0, 98, 67, 1 );
  IF_CMD_ERROR_GOTO_EXIT(res, "cmd_dpp_diag_psd_parse_signal");
  sal_strncpy(resolved_data->p2_prge_tm_port_var, buffer, sizeof(resolved_data->p2_prge_tm_port_var));
  resolved_data->p2_prge_tm_port_var[sizeof(resolved_data->p2_prge_tm_port_var) - 1] = '\0';

  res= cmd_dpp_diag_psd_parse_signal(core_id,  ++line_index,   "prge_pp_port_var" , 9, 0, 134, 103, 1);
  IF_CMD_ERROR_GOTO_EXIT(res, "cmd_dpp_diag_psd_parse_signal");
  sal_strncpy(resolved_data->p2_prge_pp_port_var, buffer, sizeof(resolved_data->p2_prge_pp_port_var));
  resolved_data->p2_prge_pp_port_var[sizeof(resolved_data->p2_prge_pp_port_var) - 1] = '\0';

  res= cmd_dpp_diag_psd_parse_signal(core_id,  ++line_index,   "post_pro_data"    , 9, 0, 245, 153, 1);
  IF_CMD_ERROR_GOTO_EXIT(res, "cmd_dpp_diag_psd_parse_signal");

  /*garbage start - dumping zeroes */
  res= cmd_dpp_diag_psd_parse_signal(core_id,  ++line_index,   "program_index"    , 0, 0, 0,    1, 1 );
  IF_CMD_ERROR_GOTO_EXIT(res, "cmd_dpp_diag_psd_parse_signal");
  /*garbage end*/

  res= cmd_dpp_diag_psd_get_original_packet( unit, core_id, ++line_index);
  IF_CMD_ERROR_GOTO_EXIT(res, "cmd_dpp_diag_psd_get_original_packet");

  /*garbage start - dumping zeroes */
  res= cmd_dpp_diag_psd_parse_signal(core_id,  ++line_index, "bypass_data"      , 0, 0, 0,   15, 1 );
  IF_CMD_ERROR_GOTO_EXIT(res, "cmd_dpp_diag_psd_parse_signal");
  /*garbage end*/

  res= cmd_dpp_diag_psd_parse_signal(core_id,  ++line_index,   "oam_counter_value", 9, 1, 57, 26, 1 );
  IF_CMD_ERROR_GOTO_EXIT(res, "cmd_dpp_diag_psd_parse_signal");
  sal_strncpy(resolved_data->p2_oam_counter_value, buffer, sizeof(resolved_data->p2_oam_counter_value));
  resolved_data->p2_oam_counter_value[sizeof(resolved_data->p2_oam_counter_value) - 1] = '\0';

  res= cmd_dpp_diag_psd_parse_signal(core_id,  ++line_index,  "dst_system_port"  , 9, 2, 157, 142, 1 );
  IF_CMD_ERROR_GOTO_EXIT(res, "cmd_dpp_diag_psd_parse_signal");
  sal_strncpy(resolved_data->p2_dst_system_port, buffer, sizeof(resolved_data->p2_dst_system_port));
  resolved_data->p2_dst_system_port[sizeof(resolved_data->p2_dst_system_port) - 1] = '\0';

  res= cmd_dpp_diag_psd_parse_signal(core_id,  ++line_index,  "src_system_port"  , 9, 0, 55, 40, 1);
  IF_CMD_ERROR_GOTO_EXIT(res, "cmd_dpp_diag_psd_parse_signal");
  sal_strncpy(resolved_data->p2_src_system_port, buffer, sizeof(resolved_data->p2_src_system_port));
  resolved_data->p2_src_system_port[sizeof(resolved_data->p2_src_system_port) - 1] = '\0';

  res= cmd_dpp_diag_psd_parse_signal(core_id,  ++line_index, "out_tm_port"      , 9, 0, 253, 246, 1 );
  IF_CMD_ERROR_GOTO_EXIT(res, "cmd_dpp_diag_psd_parse_signal");
  sal_strncpy(resolved_data->p2_out_tm_port, buffer, sizeof(resolved_data->p2_out_tm_port));
  resolved_data->p2_out_tm_port[sizeof(resolved_data->p2_out_tm_port) - 1] = '\0';

  res= cmd_dpp_diag_psd_get_out_pp_port(core_id, ++line_index);
  IF_CMD_ERROR_GOTO_EXIT(res, "cmd_dpp_diag_psd_get_out_pp_port");
  sal_strncpy(resolved_data->p2_out_pp_port, buffer, sizeof(resolved_data->p2_out_pp_port));
  resolved_data->p2_out_pp_port[sizeof(resolved_data->p2_out_pp_port) - 1] = '\0';

  res= cmd_dpp_diag_psd_parse_signal(core_id,  ++line_index, "oam_id"           , 9, 1, 25, 9, 1 );
  IF_CMD_ERROR_GOTO_EXIT(res, "cmd_dpp_diag_psd_parse_signal");
  sal_strncpy(resolved_data->p2_oam_id, buffer, sizeof(resolved_data->p2_oam_id));
  resolved_data->p2_oam_id[sizeof(resolved_data->p2_oam_id) - 1] = '\0';

  res= cmd_dpp_diag_psd_parse_signal(core_id,  ++line_index, "oam_sub_type"     , 9, 1, 8, 6, 1  );
  IF_CMD_ERROR_GOTO_EXIT(res, "cmd_dpp_diag_psd_parse_signal");
  sal_strncpy(resolved_data->oam_sub_type, buffer, sizeof(resolved_data->oam_sub_type));
  resolved_data->oam_sub_type[sizeof(resolved_data->oam_sub_type) - 1] = '\0';

  res= cmd_dpp_diag_psd_parse_signal(core_id, ++line_index , "ace_variable"     , 9, 2, 166, 159, 1  );
  IF_CMD_ERROR_GOTO_EXIT(res, "cmd_dpp_diag_psd_parse_signal");
  sal_strncpy(resolved_data->p2_ace_variable, buffer, sizeof(resolved_data->p2_ace_variable));
  resolved_data->p2_ace_variable[sizeof(resolved_data->p2_ace_variable) - 1] = '\0';

  res= cmd_dpp_diag_psd_parse_signal(core_id, ++line_index , "vsi_variable"     , 9, 0, 23, 0, 1 );
  IF_CMD_ERROR_GOTO_EXIT(res, "cmd_dpp_diag_psd_parse_signal");
  sal_strncpy(resolved_data->p2_vsi_variable, buffer, sizeof(resolved_data->p2_vsi_variable));
  resolved_data->p2_vsi_variable[sizeof(resolved_data->p2_vsi_variable) - 1] = '\0';

  res= cmd_dpp_diag_psd_parse_signal(core_id, ++line_index , "prge_out_lif"     , 9, 0, 150, 135, 1 );
  IF_CMD_ERROR_GOTO_EXIT(res, "cmd_dpp_diag_psd_parse_signal");
  sal_strncpy(resolved_data->p2_prge_out_lif, buffer, sizeof(resolved_data->p2_prge_out_lif));
  resolved_data->p2_prge_out_lif[sizeof(resolved_data->p2_prge_out_lif) - 1] = '\0';

  cmd_dpp_diag_psd_get_prge_program( unit,++line_index);
  sal_strncpy(resolved_data->p2_prge_program_variable, buffer, sizeof(resolved_data->p2_prge_program_variable));
  resolved_data->p2_prge_program_variable[sizeof(resolved_data->p2_prge_program_variable) - 1] = '\0';

  cmd_dpp_diag_psd_get_p2_oam_id_and_oam_sub_type_again();

  cmd_dpp_diag_psd_print_resolved_data(++line_index);

  PRINT_TO_TARGET(ofp,("\n--"),(ofp,"\n--"))

  sal_printf("Dump finished successfully; See help for location\n");

exit:

  if (is_stdout == 0) {
      sal_fclose(ofp); 
  }

  return res;
#endif
}


#ifndef NO_FILEIO /*goes all the way down*/
/*********************************************************************
 *     Writes parsed EPNI signals to a given file pointer,
 *     in the verilog format.
 *     See H file for more information
 */
cmd_result_t 
cmd_dpp_diag_psd_parse_signal(
    int core_id,
    int line_index,
    char* field_name,
     int prm_addr_msb,
     int prm_addr_lsb,
     int prm_fld_msb,
     int prm_fld_lsb,
     int write_to_file)
{

    int
        num_of_spaces;

    uint32 
        rv;

    sal_strncpy(buffer, "", sizeof(buffer));

    /*print title*/
    if ((write_to_file )) {
        num_of_spaces = 19 - sal_strlen(field_name) ;
        PRINT_TO_TARGET(ofp,("\n%d-%s%*s= 0x", line_index, field_name,num_of_spaces, " "),
                        (ofp, "\n%d-%s%*s= 0x", line_index, field_name,num_of_spaces, " "))
    }

    /*only zerores*/
    if ((prm_addr_msb == 0) &&( prm_addr_lsb == 0) &&( prm_fld_msb == 0)) {
        cmd_dpp_diag_psd_print_zeroes_to_file(prm_fld_lsb, ofp);
        return CMD_OK;
    }

    /*recieving signal raw data - to regs_val*/
    rv = arad_pp_diag_get_raw_signal( core_id, ARAD_EPNI_ID,  prm_addr_msb,  prm_addr_lsb,  prm_fld_msb,  prm_fld_lsb, regs_val);
    IF_ERROR_EXIT(rv,"arad_pp_diag_get_raw_signal");

    cmd_dpp_diag_psd_regs_val_to_str(prm_fld_msb,prm_fld_lsb, buffer);

    if (write_to_file ) {
        PRINT_TO_TARGET(ofp,( buffer),(ofp, buffer))
    }

    return CMD_OK;
}

/**********************************************************************
 * Fills the file with the specified amount of zeroes
 */
void
    cmd_dpp_diag_psd_print_zeroes_to_file(
        int amount,
        FILE *fp)
{

    for (; amount>0 ; amount-- ) {
            PRINT_TO_TARGET(fp,( "%d", 0),(fp, "%d", 0))
        }

}

/**********************************************************************
 * Add the input data (number),as a string in hex format, 
 * to the input str. the data is appended to the str. 
 * hex_digits is the amount of nibbles to be added.
 */
void
    cmd_dpp_diag_psd_add_hex_to_str(
        int hex_digits,
        int data,
        char *str)
{

        char
            curr_line[10],
            hex_digit_res_chr = '0' + hex_digits; /*represent hex_digits (an int), as a char*/

        sal_strncpy(curr_line, "%.", sizeof(curr_line));
        sal_strncat(curr_line, &hex_digit_res_chr ,5);
        sal_strncat(curr_line, "x", 5);
        sal_sprintf(str +sal_strlen(str), curr_line, data); 
}

/**********************************************************************
 * Parses and prints gen_header signal to output file
 */
cmd_result_t
cmd_dpp_diag_psd_get_gen_header(
    int core_id,
    int line_index
    )
{
    int
        full_regs_amount,
        bit_res_old = 0,
        bit_res_amount = 0,
        data
        ;

    uint32 
        rv;

    sal_strncpy(buffer, "", sizeof(buffer));

    PRINT_TO_TARGET(ofp,( "\n%d-gen_header         = 0x", line_index),
                    (ofp, "\n%d-gen_header         = 0x", line_index))

    /*recieving signal raw data - to regs_val*/
    rv = arad_pp_diag_get_raw_signal( core_id, ARAD_EPNI_ID,  10,  0,  255,  166, regs_val);              
    IF_ERROR_EXIT(rv,"arad_pp_diag_get_raw_signal");

    full_regs_amount = ( 255 + 1 - 166 )/32 ;                                               /*there are 26 bits left*/

    cmd_dpp_diag_psd_get_gen_header_line( full_regs_amount, bit_res_old, bit_res_amount ) ;

    sal_strncpy(buffer_copy, buffer, sizeof(buffer_copy));
    buffer_copy[sizeof(buffer_copy) - 1] = '\0';
    data = ( regs_val[full_regs_amount] & 0xffffff);                   /*only 24 first bits*/
    sal_sprintf(buffer ,"%.6x%s",data ,buffer_copy);                   /*there are 2 bits left*/
    bit_res_old=regs_val[full_regs_amount ] >> 24;

    bit_res_amount = 2;

    rv = arad_pp_diag_get_raw_signal( core_id, ARAD_EPNI_ID,  10,  1,  255,  0, regs_val);                
    IF_ERROR_EXIT(rv,"arad_pp_diag_get_raw_signal");
    full_regs_amount = 256 / 32 ;
    bit_res_old = cmd_dpp_diag_psd_get_gen_header_line( full_regs_amount, bit_res_old, bit_res_amount ) ;

    rv = arad_pp_diag_get_raw_signal( core_id, ARAD_EPNI_ID,  10,  2,  255,  0, regs_val);                
    IF_ERROR_EXIT(rv,"arad_pp_diag_get_raw_signal");
    full_regs_amount = 256 / 32 ;
    bit_res_old = cmd_dpp_diag_psd_get_gen_header_line( full_regs_amount, bit_res_old, bit_res_amount ) ;

    rv = arad_pp_diag_get_raw_signal( core_id, ARAD_EPNI_ID,  10,  3,  165,  0, regs_val);                
    IF_ERROR_EXIT(rv,"arad_pp_diag_get_raw_signal");
    full_regs_amount = 166 / 32 ;
    bit_res_old = cmd_dpp_diag_psd_get_gen_header_line( full_regs_amount, bit_res_old, bit_res_amount ) ;


    regs_val[full_regs_amount] = ( regs_val[full_regs_amount] << 2) | bit_res_old;
    sal_strncpy(buffer_copy, buffer, sizeof(buffer_copy));
    buffer_copy[sizeof(buffer_copy) - 1] = '\0';
    sal_sprintf(buffer ,"%.2x%s",regs_val[full_regs_amount],buffer_copy);

    PRINT_TO_TARGET(ofp,(buffer),(ofp, buffer))

    return CMD_OK;


}

/**********************************************************************
 * Writes gen_header given data from one adress to the buffer
 */
int cmd_dpp_diag_psd_get_gen_header_line(
    int full_regs_amount,
    int bit_res_old,
    int bit_res_amount
    ) 
{

    int bit_res_new,
        i;

    for (i = 0; i < full_regs_amount; i++) {
        bit_res_new = regs_val[i] >> (32-bit_res_amount);
        regs_val[i] = (regs_val[i] << bit_res_amount) | bit_res_old;
        bit_res_old = bit_res_new;
        sal_strncpy(buffer_copy, buffer, sizeof(buffer_copy));
        buffer_copy[sizeof(buffer_copy) - 1] = '\0';
        sal_sprintf(buffer, "%.8x%s", regs_val[i], buffer_copy);
    }

    return bit_res_old;
}

/**********************************************************************
 * Prints registers data to output file
 */
cmd_result_t
cmd_dpp_diag_psd_get_registers(
    int unit,
    int core_id,
    int line_index
    )
{
    int
        R0,
        R1,
        R2,
        R3;

    uint32
        rv;

    if (tbl_data.register_src == 1) {   /* PP mode*/
        
        rv = arad_pp_diag_get_raw_signal( core_id, ARAD_EPNI_ID,  9,  1,  71,  65, regs_val);     
        IF_ERROR_EXIT(rv,"arad_pp_diag_get_raw_signal");
        R0=regs_val[0];

        rv = arad_pp_diag_get_raw_signal( core_id, ARAD_EPNI_ID,  9,  0,  62,  56, regs_val);     
        IF_ERROR_EXIT(rv,"arad_pp_diag_get_raw_signal");
        R1=regs_val[0]+R0; /*not concatenation*/

        rv = arad_pp_diag_get_raw_signal( core_id, ARAD_EPNI_ID,  9,  2,  26,  20, regs_val);     
        IF_ERROR_EXIT(rv,"arad_pp_diag_get_raw_signal");
        R2=regs_val[0]+R1; 

        rv = arad_pp_diag_get_raw_signal( core_id, ARAD_EPNI_ID,  9,  2,  19,  13, regs_val);     
        IF_ERROR_EXIT(rv,"arad_pp_diag_get_raw_signal");
        R3=regs_val[0]+R2; 
        }

    else{   /* TM mode*/
        int cfg_ftmh_lb_key_ext_en,
            p0_dsp_ext_exist,
            cfg_ftmh_stacking_ext_enable,
            oam_ts_exists;

        rv = arad_pp_diag_ftmh_cfg_get(unit,&cfg_ftmh_lb_key_ext_en, &cfg_ftmh_stacking_ext_enable);
        IF_ERROR_EXIT(rv,"arad_pp_diag_ftmh_cfg_get");

        rv = arad_pp_diag_get_raw_signal( core_id, ARAD_EPNI_ID,  9,  2,  158,  158, regs_val);   
        IF_ERROR_EXIT(rv,"arad_pp_diag_get_raw_signal");
        p0_dsp_ext_exist = regs_val[0];
        R0 = 9 + cfg_ftmh_lb_key_ext_en + p0_dsp_ext_exist*2 ;

        R1 = R0 + cfg_ftmh_stacking_ext_enable*2 ;

        rv = arad_pp_diag_get_raw_signal( core_id, ARAD_EPNI_ID,  9,  0,  152,  152, regs_val);   
        IF_ERROR_EXIT(rv,"arad_pp_diag_get_raw_signal");
        oam_ts_exists = regs_val[0] ;
        R2 = R1 + oam_ts_exists*6 ;

        rv = arad_pp_diag_get_raw_signal( core_id, ARAD_EPNI_ID,  9,  0,  31,  25, regs_val);     
        IF_ERROR_EXIT(rv,"arad_pp_diag_get_raw_signal");
        R3=regs_val[0];

    }

    PRINT_TO_TARGET(ofp,("\n%d-R0=0x%.4x, R1=0x%.4x, R2=0x%.4x, R3=0x%.4x",line_index, R0, R1, R2, R3),
                    (ofp, "\n%d-R0=0x%.4x, R1=0x%.4x, R2=0x%.4x, R3=0x%.4x",line_index, R0, R1, R2, R3))

    return CMD_OK;

}

/**********************************************************************
 * Insert data to resolved_data->p2_oam_id_and_oam_sub_type
 */
void
cmd_dpp_diag_psd_get_p2_oam_id_and_oam_sub_type_again(void) {

    unsigned long 
        ul=0;

    ul = strtoul(resolved_data->oam_sub_type, NULL, 16);
    ul = ul << 17;
    ul = ul | strtoul(resolved_data->p2_oam_id, NULL, 16);
    sal_sprintf(resolved_data->p2_oam_id_and_oam_sub_type, "%.5x", (int)ul);

}

/**********************************************************************
 * Print resolved data to the signals dump file
 */
void
cmd_dpp_diag_psd_print_resolved_data(
    int line_index
    )
{
    char buffer[1000];

    sal_sprintf(buffer,"\n%d-resolved_data      = 0x%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s", 
                line_index,
                resolved_data->p2_oam_id_and_oam_sub_type,
                resolved_data->p2_prge_pp_port_var,
                resolved_data->p2_prge_tm_port_var,
                resolved_data->p2_prge_out_lif,
                resolved_data->p2_ace_variable,
                resolved_data->p2_vsi_variable,
                resolved_data->term2prge_tod_ntp,
                resolved_data->term2prge_tod_1588,
                resolved_data->oam_sub_type,
                resolved_data->p2_oam_id,
                resolved_data->p2_oam_counter_value,
                resolved_data->p2_out_pp_port,
                resolved_data->p2_out_tm_port,
                resolved_data->p2_prge_program_variable,
                resolved_data->p2_ees_data_action2,
                resolved_data->p2_ees_data_action1,
                resolved_data->p2_sys_headers_size,
                resolved_data->p2_nwk_headers_size,
                resolved_data->p2_dst_system_port,
                resolved_data->p2_src_system_port);

    PRINT_TO_TARGET(ofp,(buffer),(ofp, buffer))
}

/**********************************************************************
 * Print out_pp_port to the signals dump file
 */
cmd_result_t
cmd_dpp_diag_psd_get_out_pp_port(
    int core_id,
    int line_index
    )
{
    int
        lsb,
        msb,
        data
        ;

    uint32
        rv;

    rv = arad_pp_diag_get_raw_signal( core_id, ARAD_EPNI_ID,  9,  0,  255,  254, regs_val); 
    IF_ERROR_EXIT(rv,"arad_pp_diag_get_raw_signal");

    lsb = regs_val[0];

    rv = arad_pp_diag_get_raw_signal( core_id, ARAD_EPNI_ID,  9,  1,  5,  0, regs_val);     
    IF_ERROR_EXIT(rv,"arad_pp_diag_get_raw_signal");

    msb = regs_val[0] << 2 ;

    data =  msb | lsb;

    sal_sprintf(buffer,"%.2x", data);

    PRINT_TO_TARGET(ofp,("\n%d-out_pp_port        = 0x%s", line_index, buffer),
                    (ofp, "\n%d-out_pp_port        = 0x%s", line_index, buffer))

    return CMD_OK;

}

/**********************************************************************
 * Print prge_program to the signals dump file
 */
void
cmd_dpp_diag_psd_get_prge_program(
    int unit,
    int line_index
    )
{

    int
        data_lsbs;

    data_lsbs = ((((((( tbl_data.remove_system_header << 1) | tbl_data.remove_network_header) << 5) 
                    | tbl_data.bytes_to_remove) << 1 ) | tbl_data.register_src) << 4) | tbl_data.program_pointer ;

    sal_sprintf(buffer, "%08x%08x%.3x", COMPILER_64_HI(tbl_data.program_var), COMPILER_64_LO(tbl_data.program_var), data_lsbs);

    PRINT_TO_TARGET(ofp,( "\n%d-prge_program       = 0x%s", line_index, buffer),
                    (ofp, "\n%d-prge_program       = 0x%s", line_index, buffer))

    sal_sprintf(buffer, "%08x%08x", COMPILER_64_HI(tbl_data.program_var), COMPILER_64_LO(tbl_data.program_var));
}

/**********************************************************************
 * Print original_packet to the signals dump file
 */
cmd_result_t
cmd_dpp_diag_psd_get_original_packet(
    int unit,
    int core_id,
    int line_index
    )
{
    int bytes_to_remove,
        bytes_to_add,
        nibble_amt,
        i,
        bits_to_add;

    cmd_result_t
        res;
    SOC_PPC_DIAG_RESULT ret_val;

    char *updated_packet = NULL;
    char *original_packet = NULL;

    SOC_PPC_DIAG_RECEIVED_PACKET_INFO info;
    SOC_PPC_DIAG_BUFFER packet_header;
    uint32 rv;

    rv = soc_ppd_diag_received_packet_info_get(unit, core_id, &info, &ret_val);
    IF_ERROR_EXIT(rv,"soc_ppd_diag_received_packet_info_get");

    if (ret_val != SOC_PPC_DIAG_OK) {
        sal_printf("ERROR: Packet not found\n");
        goto exit;
    }
    packet_header = info.packet_header;

    rv = arad_pp_diag_get_raw_signal( core_id, ARAD_IHB_ID,  12,  2,  252,  246, regs_val); 
    IF_ERROR_EXIT(rv,"arad_pp_diag_get_raw_signal");
    bytes_to_remove = regs_val[0];

    rv = arad_pp_diag_get_raw_signal( core_id, ARAD_IHB_ID,  12,  2,  255,  253, regs_val); 
    IF_ERROR_EXIT(rv,"arad_pp_diag_get_raw_signal");
    bytes_to_add = regs_val[0];
    
    rv = arad_pp_diag_get_raw_signal( core_id, ARAD_IHB_ID,  12,  3,  2,  0, regs_val);     
    IF_ERROR_EXIT(rv,"arad_pp_diag_get_raw_signal");
    bytes_to_add |= ( regs_val[0] << 3) ;
    /*what if it is equal zero*/

    nibble_amt = 2 * ( SOC_PPC_DIAG_BUFF_MAX_SIZE * 4) + 1 ;

    original_packet= (char*) sal_alloc(sizeof(char)*nibble_amt, "");
    IF_ALLOC_ERROR_GOTO_EXIT(original_packet,"sal_alloc");

    sal_strncpy(original_packet, "", nibble_amt);

    for (i = SOC_PPC_DIAG_BUFF_MAX_SIZE - 1 ; i > -1; --i) {
        sal_sprintf(original_packet + sal_strlen(original_packet),"%.8x" ,packet_header.buff[i] );
    }
    nibble_amt = 2 * ( packet_header.buff_len + bytes_to_add - bytes_to_remove ) + 1 ;
    
    if ((nibble_amt < 1)) {
       sal_printf("ERROR: Packet before editor has a non-positive legnth \n");
       goto exit;
    }
    updated_packet = (char *)sal_alloc(sizeof(char) * nibble_amt, "");
    IF_ALLOC_ERROR_GOTO_EXIT(updated_packet,"sal_alloc");

    sal_strncpy(updated_packet, "", nibble_amt); 

    bits_to_add = bytes_to_add * 8;

    if (bits_to_add == 0) {
        ;
    }

    else if (bits_to_add < 247) {
        rv = arad_pp_diag_get_raw_signal( core_id, ARAD_IHB_ID,  12,  2,  245, ( 246 - bits_to_add ) , regs_val);                 
        IF_ERROR_GOTO_EXIT(rv,"arad_pp_diag_get_raw_signal");

        res= cmd_dpp_diag_psd_update_ftmh_base(bits_to_add, core_id);
        IF_CMD_ERROR_GOTO_EXIT(res, "cmd_dpp_diag_psd_update_ftmh_base");

        cmd_dpp_diag_psd_regs_val_to_str(  245, ( 246 - bits_to_add ) , updated_packet);
    }

    else{
        rv = arad_pp_diag_get_raw_signal( core_id, ARAD_IHB_ID,  12,  2,  245, 0 , regs_val);                                     
        IF_ERROR_GOTO_EXIT(rv,"arad_pp_diag_get_raw_signal");

        res= cmd_dpp_diag_psd_update_ftmh_base(bits_to_add, core_id);
        IF_CMD_ERROR_GOTO_EXIT(res, "cmd_dpp_diag_psd_update_ftmh_base");

        cmd_dpp_diag_psd_regs_val_to_str(  245, 0 , updated_packet );      

        rv = arad_pp_diag_get_raw_signal( core_id, ARAD_IHB_ID,  12,  1,  255, ( 256 - ( bits_to_add - 246 )) , regs_val);        
        IF_ERROR_GOTO_EXIT(rv,"arad_pp_diag_get_raw_signal");

        cmd_dpp_diag_psd_regs_val_to_str(  255, ( 256 - ( bits_to_add - 246 )) , updated_packet+ sal_strlen(updated_packet));
    }

    original_packet[2 * packet_header.buff_len ] = '\0';
    sal_strncpy(updated_packet + sal_strlen(updated_packet), original_packet + 2 * bytes_to_remove, nibble_amt - sal_strlen(updated_packet));

    /* the max size for adapter is 384 nibbles*/
    PRINT_TO_TARGET(ofp,("\n%d-original_pkt       = 0x%.384s", line_index, updated_packet),
                    (ofp, "\n%d-original_pkt       = 0x%.384s", line_index, updated_packet))

    /*adding nibbles (to match 384 nibbles) for the adapter to work*/
    for (i = 0 ; i < (384 - sal_strlen(updated_packet)) ; i++) {
        PRINT_TO_TARGET(ofp,( "1"),(ofp, "1"))
    }

    sal_free(updated_packet);
    sal_free(original_packet);

    return CMD_OK;

    exit:

    if (updated_packet != NULL) {
        sal_free(updated_packet); 
    }

    if (original_packet != NULL) {
        sal_free(original_packet); 
    }

    return CMD_FAIL;
}

/**********************************************************************
 * Parses the content of regs_val (a uint32 array) to str, 
 * as a hex number.  parameters prm_fld_lsb, prm_fld_msb represent 
 * the amount of bits in the array. 
 */
void
cmd_dpp_diag_psd_regs_val_to_str(
    int prm_fld_msb,
    int prm_fld_lsb,
    char *str
    )
{
    int
        full_regs_amount,
        hex_digit_res,
        i;

    full_regs_amount = ( prm_fld_msb + 1 - prm_fld_lsb )/32 ;

    /*amount of hex digit remaining in the last register*/
    hex_digit_res = cmd_dpp_diag_psd_ceil_div ((( prm_fld_msb + 1 - prm_fld_lsb ) % 32) , 4);

    if (hex_digit_res != 0) {
        cmd_dpp_diag_psd_add_hex_to_str(  hex_digit_res, regs_val[full_regs_amount ], str);
    }

    for (i = (full_regs_amount - 1 ) ; i > -1  ; i-- ) {
        sal_sprintf(str +sal_strlen(str) ,"%.8x",regs_val[i]);
    }

}

/**********************************************************************
 * Fills the global variable tbl_data with relevant data
 */
cmd_result_t
cmd_dpp_diag_psd_get_table_data_for_signals_dump(
    int unit, 
    int core_id
    )
{

    uint32
        offset,
        rv;

    rv = arad_pp_diag_get_raw_signal( core_id, ARAD_EPNI_ID,  11,  0,  34,  30, regs_val);    
    IF_ERROR_EXIT(rv, "arad_pp_diag_get_raw_signal");

    offset = regs_val[0] / 2;

    rv = arad_pp_diag_epni_prge_program_tbl_get(unit, offset, &tbl_data);
    IF_ERROR_EXIT(rv, "arad_pp_diag_epni_prge_program_tbl_get");

    return CMD_OK;

}

/**********************************************************************
 * Update the original packet with ftmh_base fields
 */
cmd_result_t
cmd_dpp_diag_psd_update_ftmh_base(int bits_to_add, int core_id){
    int
        ftmh_base_offset,

        /*these fields from FTMH base are not updated in the packet and need a seperate update*/
        packet_size,
        pp_dsp,
        tm_action_type;

    uint32
        inner_regs_val[ARAD_PP_DIAG_DBG_VAL_LEN],
        rv;

    ftmh_base_offset = bits_to_add - 72;

    /*14 bits packet_size - 71:58*/
    rv = arad_pp_diag_get_raw_signal( core_id, ARAD_EPNI_ID,  2,  1,  63,  50, inner_regs_val);       
    IF_ERROR_EXIT(rv, "arad_pp_diag_get_raw_signal");
    packet_size = inner_regs_val[0];
    cmd_dpp_diag_psd_update_ftmh_base_field(packet_size, 58, 71, ftmh_base_offset);

    /*8 bits pp_dsp - 38:31*/
    rv = arad_pp_diag_get_raw_signal( core_id, ARAD_EPNI_ID,  2,  1,  30,  23, inner_regs_val);       
    IF_ERROR_EXIT(rv, "arad_pp_diag_get_raw_signal");
    pp_dsp = inner_regs_val[0];
    cmd_dpp_diag_psd_update_ftmh_base_field(pp_dsp, 31, 38, ftmh_base_offset);

    /*2 bits tm_action - 28:27*/
    rv = arad_pp_diag_get_raw_signal( core_id, ARAD_EPNI_ID,  2,  1,  20,  19, inner_regs_val);       
    IF_ERROR_EXIT(rv, "arad_pp_diag_get_raw_signal");
    tm_action_type = inner_regs_val[0];
    cmd_dpp_diag_psd_update_ftmh_base_field(tm_action_type, 27, 28, ftmh_base_offset);

    return CMD_OK;
    
}

/**********************************************************************
 * Update ftmh_base specific field
 */
void
cmd_dpp_diag_psd_update_ftmh_base_field(int data, int bit_start, int bit_end, int ftmh_base_offset){

    int
        reg_num,
        bit_offset,
        num_bits,
        data_left;

    if (bit_end < bit_start) {
        return;
    }

    reg_num = (ftmh_base_offset + bit_start) / 32; 
    bit_offset =  ( ftmh_base_offset + bit_start ) % 32 ;
    num_bits = ((32 - bit_offset) > (bit_end-bit_start+1)) ?  (bit_end-bit_start+1) : (32 - bit_offset);

    data_left = data >> num_bits ;
    data = data << bit_offset ;
    regs_val[reg_num] = regs_val[reg_num] | data;

    cmd_dpp_diag_psd_update_ftmh_base_field( data_left,  bit_start + num_bits, bit_end, ftmh_base_offset);

}

/*****************************
* Dividing the ints, and ceiling the result
 */ 
int
cmd_dpp_diag_psd_ceil_div(int dividend, int divisor) {

    int 
        res = dividend / divisor;

    if ( (dividend % divisor ) == 0) {
        return res;
    }
    return res + 1;
}

#endif /*FOR FILE IO*/
#endif /* BCM_PETRA_SUPPORT */

