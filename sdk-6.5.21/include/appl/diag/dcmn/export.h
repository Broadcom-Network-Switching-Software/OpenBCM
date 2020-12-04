/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 * DNX Export
 */


#ifndef __DIAG_EXPORT_
#define __DIAG_EXPORT_

#include <appl/diag/shell.h>
#include <appl/diag/parse.h>

/********************************* 
 * if res is not CMD_OK,
 * go to the exit label
 */
#define ASSERT_EXIT(res, diag_cmd)                                      \
    if (res != CMD_OK) {                                                \
        sal_printf("ERROR: export_dump: %s failed!\n", diag_cmd);      \
        goto exit;                                                      \
    }                                                                   \

#endif /*__DIAG_EXPORT_ */
