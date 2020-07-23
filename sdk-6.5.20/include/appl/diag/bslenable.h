/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Broadcom System Log State
 */

#ifndef _DIAG_BSLENABLE_H
#define _DIAG_BSLENABLE_H

#include <shared/bslext.h>
#include <sal/core/thread.h>

#define bslmgmt_source_valid    bslenable_source_valid
#define bslmgmt_set             bslenable_set
#define bslmgmt_get             bslenable_get
#define bslmgmt_reset           bslenable_reset

#define MAX_BSLENABLE_THREAD_COUNT  2
extern int
bslenable_source_valid(bsl_layer_t layer, bsl_source_t source);

extern void
bslenable_set(bsl_layer_t layer, bsl_source_t source, bsl_severity_t severity);

extern bsl_severity_t
bslenable_get(bsl_layer_t layer, bsl_source_t source);

extern void
bslenable_reset(bsl_layer_t layer, bsl_source_t source);

extern void
bslenable_reset_all(void);

extern void
bslenable_thread_set(bsl_layer_t layer, bsl_source_t source, int tidx,
                     int thread_id);
extern int
bslenable_thread_get(bsl_layer_t layer, bsl_source_t source, int tidx);
#endif /* !_DIAG_BSLENABLE_H */
