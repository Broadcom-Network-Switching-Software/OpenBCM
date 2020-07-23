/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        diag_signals_dump.h
 * Purpose:     
 */

#ifndef   _DIAG_SIGNALS_DUMP_H_
#define   _DIAG_SIGNALS_DUMP_H_

#include <appl/diag/shell.h>
#include <appl/diag/parse.h>

#ifdef BCM_PETRA_SUPPORT
/*This macro handles an allocation error*/
#define IF_ALLOC_ERROR_EXIT(rv,func)                       \
    if (rv == NULL) {                                      \
        sal_printf("ERROR: %s function failed!\n", func);  \
        return CMD_FAIL;                                   \
    }      

/*This macro handles an allocation error*/
#define IF_ALLOC_ERROR_GOTO_EXIT(rv,func)                  \
    if (rv == NULL) {                                      \
        sal_printf("ERROR: %s function failed!\n", func);  \
        goto exit;                                         \
    }      

/*This macro handles an error represented as uint32*/
#define IF_ERROR_EXIT(rv,func)                             \
    if (soc_sand_get_error_code_from_error_word(rv) != 0) {\
        sal_printf("ERROR: %s function failed!\n", func);  \
        return CMD_FAIL;                                   \
    }                                                      \

/*This macro handles an error represented as uint32*/
#define IF_ERROR_GOTO_EXIT(rv,func)                        \
    if (soc_sand_get_error_code_from_error_word(rv) != 0) {\
        sal_printf("ERROR: %s function failed!\n", func);  \
        goto exit;                                         \
    }                                                      \

/*This macro handles an error of type cmd_result_t*/
#define IF_CMD_ERROR_EXIT(res,func)                        \
    if (res == CMD_FAIL) {                                 \
        sal_printf("ERROR: %s function failed!\n", func);  \
        return CMD_FAIL;                                   \
    }                                                      \

/*This macro handles an error of type cmd_result_t*/
#define IF_CMD_ERROR_GOTO_EXIT(res,func)                   \
    if (res == CMD_FAIL) {                                 \
        sal_printf("ERROR: %s function failed!\n", func);  \
        goto exit;                                         \
    }                                                      \

/*This macro handles a print request 
  If filename is NULL - print to the screen*/
#define PRINT_TO_TARGET(file_dsc,cli_out_args, sal_fprintf_args)    \
    if (file_dsc == NULL) {                                         \
        bsl_printf cli_out_args;                                    \
    } else{                                                         \
        sal_fprintf sal_fprintf_args;                               \
    }                                                               \

/*********************************************************************
 *     Creates a dump file of the packet signals,
 *     parsed to match the verilog format.
 *     Currently set to match ARAD & ARAD+
 */
cmd_result_t 
  cmd_dpp_diag_parsed_signals_get(
        int unit,
        int core_id,
        char * filename);

#ifndef NO_FILEIO /*If no IO - block all inner functions - they are not relevant*/
/*********************************************************************
 *     Writes parsed signals to a buffer or to the output file
 *     (or both), in the verilog format.
 *  
 *     This funtion should be used only for a signal that is stored in
 *     a single address
 *  
 *     buffer is almost always filled. special cases below.
 *  
 *     Parameters:
 *  
 *          line_index  - the line number to be writen at the beginning
 *          of the line.
 *  
 *          prm_*       - these parameters refer to the location of the
 *          raw signal data.
 *  
 *          write_to_file -
 *              0 : in this option you choose to not print to file.
 *              1 : this is the regular case, when the program writes a
 *                  new signal to the file
 *  
 *     Note: if you want the data to be zeroes, assign
 *     (prm_addr_msb == 0) &&( prm_addr_lsb == 0) &&( prm_fld_msb == 0)
 *     prm_fld_lsb == amount of zeroes.
 *     for this option the buffer will not be filled.
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
     int write_to_file );
    


/**********************************************************************
 * Fills the file with the specified amount of zeroes
 */
void
    cmd_dpp_diag_psd_print_zeroes_to_file(
        int amount, FILE *fp);


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
;

/**********************************************************************
 * Parses and prints gen_header signal to output file
 */
cmd_result_t
cmd_dpp_diag_psd_get_gen_header(
    int core_id,
    int line_index
    )
;

/**********************************************************************
 * Writes gen_header given data from one adress to the buffer
 */
int 
cmd_dpp_diag_psd_get_gen_header_line(
    int full_regs_amount,
    int bit_res_old,
    int bit_res_amount
    ) 
;

/**********************************************************************
 * Prints registers data to output file
 */
cmd_result_t
cmd_dpp_diag_psd_get_registers(
    int unit,
    int core_id,
    int line_index
    )
;

/**********************************************************************
 * Insert data to resolved_data->p2_oam_id_and_oam_sub_type
 */
void
cmd_dpp_diag_psd_get_p2_oam_id_and_oam_sub_type_again(void);

/**********************************************************************
 * Print resolved data to the signals dump file
 */
void
cmd_dpp_diag_psd_print_resolved_data(
    int line_index
    )
;

/**********************************************************************
 * Print out_pp_port to the signals dump file
 */
cmd_result_t
cmd_dpp_diag_psd_get_out_pp_port(
     int core_id,
    int line_index
    )
;

/**********************************************************************
 * Print prge_program to the signals dump file
 */
void
cmd_dpp_diag_psd_get_prge_program(
    int unit,
    int line_index
    )
;

/**********************************************************************
 * Print original_packet to the signals dump file
 */
cmd_result_t
cmd_dpp_diag_psd_get_original_packet(
    int unit,
    int core_id,
    int line_index
    )
;

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
;

/**********************************************************************
 * Fills the global variable tbl_data with relevant data
 */
cmd_result_t
cmd_dpp_diag_psd_get_table_data_for_signals_dump(
    int unit, 
    int core_id);

/**********************************************************************
 * Update the original packet with ftmh_base fields
 */
cmd_result_t
cmd_dpp_diag_psd_update_ftmh_base(
    int bits_to_add,
    int core_id);

/**********************************************************************
 * Update ftmh_base specific field
 */
void
cmd_dpp_diag_psd_update_ftmh_base_field(
    int data, 
    int bit_start, 
    int bit_end, 
    int ftmh_base_offset
    )
;

/*****************************
* Dividing the ints, and ceiling the result
 */ 
int
cmd_dpp_diag_psd_ceil_div(int dividend, int divisor) ;
#endif /*FOR FILE IO*/
#endif /* BCM_PETRA_SUPPORT */
#endif /* _DIAG_SIGNALS_DUMP_H_ */

