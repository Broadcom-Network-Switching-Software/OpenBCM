/* 
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <shared/bsl.h>
#include <bcm/error.h>

#ifndef AREFHELPER_H
#define AREFHELPER_H


#define APICALLCHK(cmd, arg) { int rv; if ( (rv = (cmd arg)) < 0 ) { \
                               bsl_printf("Error %d : %s %s at file %s line %d\n", rv, #cmd, bcm_errmsg(rv), __FILE__, __LINE__); \
                               return rv; } }

#define APICALLCHK_NR(cmd, arg) { int rv; if ( (rv = (cmd arg)) < 0 ) { \
                               bsl_printf("Error %d : %s at file %s line %d\n", rv, #cmd,__FILE__, __LINE__); return; } }



#endif
