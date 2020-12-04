/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains STAT definitions internal to the BCM library.
 */

#ifndef _BCM_INT_STAT_H
#define _BCM_INT_STAT_H

#include <bcm/stat.h>
#include <soc/counter.h>

/*
 * Utility routines for statistics accumulation
 */

/* The REG_* macros require the following declaration in any function which
 * uses them:
 */

#define REG_MATH_DECL \
        uint64 reg_val

#define REG_ADD(unit, port, reg, val)					   \
    if (SOC_REG_IS_VALID(unit, reg) && SOC_REG_IS_COUNTER(unit, reg)) {	   \
        if (SOC_CONTROL(unit)->counter_interval == 0 /*counter thread is off*/) { \
            SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, port, 0, &reg_val));      \
        } else {                                                            \
            SOC_IF_ERROR_RETURN(soc_counter_get(unit, port, reg,            \
                                            0, &reg_val));                  \
        }                                                                   \
	COMPILER_64_ADD_64(val, reg_val);				                        \
    }


#define REG_SUB(unit, port, reg, val)					   \
    if (SOC_REG_IS_VALID(unit, reg) && SOC_REG_IS_COUNTER(unit, reg)) {	   \
        if (SOC_CONTROL(unit)->counter_interval == 0 /*counter thread is off*/) { \
            SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, port, 0, &reg_val));      \
        } else {                                                            \
            SOC_IF_ERROR_RETURN(soc_counter_get(unit, port, reg,            \
                                            0, &reg_val));                  \
        }                                                                  \
        if (COMPILER_64_GT(val, reg_val)) {                                \
	    COMPILER_64_SUB_64(val, reg_val);				   \
        } else {                                                           \
            COMPILER_64_ZERO(val);                                         \
        }                                                                  \
    }

/* Library-private functions exported from stat_generic.c */
extern int _bcm_petra_stat_generic_get(int unit, bcm_port_t port,
                                 bcm_stat_val_t type, uint64 *val);

extern int _bcm_petra_stat_ge_get(int unit, bcm_port_t port,
                                 bcm_stat_val_t type, uint64 *val);

extern int _bcm_petra_stat_mxq_ge_get(int unit, bcm_port_t port,
                                 bcm_stat_val_t type, uint64 *val);

int _bcm_petra_stat_counter_non_dma_extra_get(int unit,
                                  soc_counter_non_dma_id_t non_dma_id,
                                                    bcm_port_t port,
                                                    uint64 *val);
extern int
_soc_controlled_counter_get_info(int unit, soc_port_t port, soc_reg_t ctr_reg,
                            int *base_index, int *num_entries, char **cname);

#ifdef BCM_ARAD_SUPPORT
int _bcm_petra_stat_recover(int unit, bcm_pbmp_t okay_nif_ports, uint32 counter_flags, int counter_interval, bcm_pbmp_t counter_pbmp);
#endif

#if defined(BCM_DPP_SUPPORT)
int 
_bcm_petra_stat_counter_lif_counting_get(
    int unit, 
    uint32 flags, 
    bcm_stat_counter_source_t *source, 
    bcm_stat_counter_lif_mask_t *counting_mask, 
    bcm_stat_counter_lif_stack_id_t *lif_stack_id_to_count);
#endif /* defined(BCM_DPP_SUPPORT) */

#endif /* !_BCM_INT_STAT_H */
