/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DIAG_INFIX_H
#define _DIAG_INFIX_H

#include <sal/types.h>

#define INFIX_ASTK_SIZE		16
#define INFIX_OSTK_SIZE		6
#define INFIX_TYPE		uint32

int infix_eval(char *s, INFIX_TYPE *v);

#endif /* _DIAG_INFIX_H */
