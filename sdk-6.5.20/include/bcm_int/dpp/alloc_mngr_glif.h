/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        alloc_mngr_glif.h
 * Purpose:     Resource allocation for gloal lif.
 */

#ifndef  INCLUDE_ALLOC_MNGR_GLIF_H
#define  INCLUDE_ALLOC_MNGR_GLIF_H

#include <bcm_int/dpp/alloc_mngr_lif.h>

/*************
 * INCLUDES  *
 */
/* { */

/* } */
/*************
 * DEFINES   *
 */
/* { */

/* Allocation flags */
#define _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS             (0x1)
#define _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS              (0x2)
#define _BCM_DPP_GLOBAL_LIF_MAPPING_IN_EG_DIRECTIONS    (_BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS | _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS)
#define _BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_TO_LOCAL     (0X4)
#define _BCM_DPP_GLOBAL_LIF_MAPPING_LOCAL_TO_GLOBAL     (0X8)
#define _BCM_DPP_GLOBAL_LIF_MAPPING_GLOB_LOC_DIRECTIONS (_BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_TO_LOCAL | _BCM_DPP_GLOBAL_LIF_MAPPING_LOCAL_TO_GLOBAL)

/* All flags except for direction flags will be used to indicate type */
#define _BCM_DPP_GLOBAL_LIF_MAPPING_DIRECTIONS  (_BCM_DPP_GLOBAL_LIF_MAPPING_GLOB_LOC_DIRECTIONS | _BCM_DPP_GLOBAL_LIF_MAPPING_IN_EG_DIRECTIONS)
#define _BCM_DPP_GLOBAL_LIF_MAPPING_TYPES       (-1 & ~_BCM_DPP_GLOBAL_LIF_MAPPING_DIRECTIONS)

/* Maximum number of virtual lifs */
#define _BCM_DPP_MAX_VIRTUAL_LIF(unit)            (2 * SOC_DPP_DEFS_GET(unit, nof_global_lifs))

/* Maximum number of entries in global lif bank. */
#define _BCM_DPP_AM_GLOBAL_LIF_NOF_ENTRIES_PER_BANK      ((int)ARAD_PP_EG_ENCAP_NOF_ENTRIES_PER_HALF_EEDB_BANK(unit))



/* } */
/*************
 * MACROS    *
 */
/* { */

/* Ingress global lif defines. */
#define _BCM_DPP_AM_NOF_GLOBAL_IN_LIFS(_unit) (SOC_DPP_CONFIG(_unit)->pp.nof_global_in_lifs)

#define _BCM_DPP_AM_GLOBAL_IN_LIF_ID_IS_LEGAL(_unit, _global_lif_id)      \
        (((_global_lif_id) < _BCM_DPP_AM_NOF_GLOBAL_IN_LIFS(_unit))       \
        && ((_global_lif_id) >= 0))

/* Egress global lif defines. */
#define _BCM_DPP_AM_NOF_GLOBAL_OUT_LIFS(_unit) (SOC_DPP_CONFIG(_unit)->pp.nof_global_out_lifs)

#define _BCM_DPP_AM_GLOBAL_OUT_LIF_ID_IS_LEGAL(_unit, _global_lif_id)     \
        (((_global_lif_id) < _BCM_DPP_AM_NOF_GLOBAL_OUT_LIFS(_unit))      \
        && ((_global_lif_id) >= 0))

/* } */
/*************
 * TYPE DEFS *
 */
/* { */

typedef struct {
    PARSER_HINT_ARR int         *ingress_lif_to_global_lif;
                    int         ingress_global_lif_to_lif_htb_handle;
                    int         egress_lif_to_global_lif_htb_handle;
                    SHR_BITDCL  *global_lif_sync_indication;
                    int         global_lif_egress_count;
                    int         global_lif_ingress_count;
} bcm_dpp_alloc_mngr_glif_info_t;

/* } */
/*************
 * GLOBALS   *
 */
/* { */

/* } */
/*************
 * FUNCTIONS *
 */
/* { */

/* Global lif mapping */

/* Init global lif mapping module. */
int
_bcm_dpp_global_lif_mapping_init(int unit);

/* Deinit global lif mapping module. */
int
_bcm_dpp_global_lif_mapping_deinit(int unit);

/* Map between a global lif to a local lif and vice versa */
int
_bcm_dpp_global_lif_mapping_create(int unit, uint32 flags, int global_lif, int local_lif);

/* Remove mapping between a global lif to a local lif and vice versa */
int
_bcm_dpp_global_lif_mapping_remove(int unit, uint32 flags, int global_lif, int local_lif);

/* Find mapping between a local lif to a global lif */
int
_bcm_dpp_global_lif_mapping_local_to_global_get(int unit, uint32 flags, int local_lif, int *global_lif);

/* Find mapping between a global lif to a local lif */
int
_bcm_dpp_global_lif_mapping_global_to_local_get(int unit, int flags, int global_lif, int *local_lif);

/** GLOBAL LIF ALLOCATION - START **/

uint32
_bcm_dpp_am_global_lif_init(int unit);

uint32
_bcm_dpp_am_global_sync_lif_internal_alloc(int unit, uint32 flags, int *global_lif);

uint32
_bcm_dpp_am_global_sync_lif_internal_dealloc(int unit, int global_lif);

uint32
_bcm_dpp_am_global_lif_non_sync_internal_alloc(int unit, uint32 alloc_flags, uint8 is_ingress, int *global_lif);

uint32
_bcm_dpp_am_global_lif_non_sync_internal_dealloc(int unit, int is_ingress, int global_lif);

uint32
_bcm_dpp_am_global_egress_lif_internal_is_allocated(int unit, int global_lif);

uint32
_bcm_dpp_am_global_ingress_lif_internal_is_allocated(int unit, int global_lif);

uint32
_bcm_dpp_am_global_lif_set_direct_bank(int unit, int eedb_bank_id, uint8 is_direct_bank);

/* } */

#endif /* INCLUDE_ALLOC_MNGR_GLIF_H */
