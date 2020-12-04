/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        field.h
 * Purpose:     Common Internal Field Processor data structure definitions for the
 *              BCM library.
 */

#ifndef _BCM_COMMON_FIELD_H
#define _BCM_COMMON_FIELD_H

#include <shared/bsl.h>

#include <soc/defs.h>	
#include <bcm/debug.h>

#ifdef BCM_FIELD_SUPPORT

#define FP_VERB_CHECK()        LOG_CHECK(BSL_LS_BCM_FP | BSL_VERBOSE)

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

/*
 * Typedef:
 *     _stage_id_t
 * Purpose:
 *     Holds format info for pipline stage id. 
 */
typedef int _field_stage_id_t;

/* Pipeline stages for packet processing. */
#define _BCM_FIELD_STAGE_INGRESS    (0)  
#define _BCM_FIELD_STAGE_LOOKUP     (1)  
#define _BCM_FIELD_STAGE_EGRESS     (2)  
#define _BCM_FIELD_STAGE_EXTERNAL   (3)
#define _BCM_FIELD_STAGE_EXACTMATCH (4)
#define _BCM_FIELD_STAGE_PRESEL     (_BCM_FIELD_STAGE_INGRESS)
#define _BCM_FIELD_STAGE_CLASS      (5)
#define _BCM_FIELD_STAGE_FLOWTRACKER (6)
#define _BCM_FIELD_STAGE_AMFTFP     (7)
#define _BCM_FIELD_STAGE_AEFTFP     (8)

#define _BCM_FIELD_STAGE_STRINGS \
    {"Ingress",            \
     "Lookup",             \
     "Egress",             \
     "External",           \
     "ExactMatch",         \
     "Class",              \
     "Flowtracker",        \
     "AMFTFP",             \
     "AEFTFP"}

#endif /* BCM_FIELD_SUPPORT */
#endif /* _BCM_COMMON_FIELD_H */
