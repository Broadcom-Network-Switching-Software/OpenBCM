/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef PEM_DEBUG_USER_INTERFACE_H
#define PEM_DEBUG_USER_INTERFACE_H



int pem_debug_print_pem_access_all_inputs(int stage_num);
int pem_debug_print_pem_access_all_outputs(int stage_num);


int pem_debug_print_pem_access_relevant_inputs(int stage_num);
int pem_debug_print_pem_access_relevant_outputs(int stage_num);


int pem_debug_print_pem_access_single_input(int stage_num, const char* field_name);
int pem_debug_print_pem_access_single_output(int stage_num, const char* field_name);


int pem_debug_print_pem_access_packet_in_fifo(int stage_num, int packet_backwards_offset);  



int pem_debug_print_available_fields(int stage_num);





#endif

