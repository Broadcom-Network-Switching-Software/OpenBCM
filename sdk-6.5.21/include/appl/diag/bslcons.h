/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Broadcom System Log State
 */

#ifndef _DIAG_BSLCONS_H
#define _DIAG_BSLCONS_H

int
bslcons_init(void);

int
bslcons_is_enabled(void);

int
bslcons_enable(int enable);

int
bslcons_thread_check_is_enabled(void);

int
bslcons_thread_check_enable(int enable);
#endif /* !_DIAG_BSLCONS_H */
