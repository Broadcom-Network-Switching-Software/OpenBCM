/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Broadcom System Log State
 */

#ifndef _DIAG_BSLFILE_H
#define _DIAG_BSLFILE_H

#ifndef NO_FILEIO
#include <stdio.h>

FILE *
bslfile_fp(void);

#endif /* NO_FILEIO */

int
bslfile_init(void);

char *
bslfile_name(void);

int
bslfile_close(void);

int
bslfile_open(char *filename, int append);

int
bslfile_is_enabled(void);

int
bslfile_enable(int enable);

#endif /* !_DIAG_BSLFILE_H */
