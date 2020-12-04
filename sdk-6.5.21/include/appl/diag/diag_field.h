/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        diag_field.h
 * Purpose:     
 */

#ifndef   _DIAG_FIELD_H_
#define   _DIAG_FIELD_H_

#include <appl/diag/shell.h>
#include <appl/diag/parse.h>

cmd_result_t
cmd_dpp_diag_field(int unit, args_t* a); 

void
print_field_usage(int unit);



cmd_result_t
cmd_dpp_diag_dbal(int unit, args_t* a); 

void
print_dbal_usage(int unit);

#endif /* _DIAG_FIELD_H_ */

