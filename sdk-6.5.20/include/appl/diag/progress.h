/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Progress Report module: diag/progress.c
 */

#include <sal/types.h>

void progress_init(uint32 total_count, int start_seconds, int disable);
void progress_status(char *status_str);
void progress_report(uint32 count_incr);
void progress_done(void);
