/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * Broadcom System Log Error
 */

#ifndef _DIAG_BSLERROR_H
#define _DIAG_BSLERROR_H

int
bslerror_init(void);

int
bslerror_enable_set(int enable);

int
bslerror_enable_restore(void);
#endif /* !_DIAG_BSLERROR_H */
