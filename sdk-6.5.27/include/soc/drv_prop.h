/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#ifndef _SOC_DRV_PROP_H
#define _SOC_DRV_PROP_H

#include <shared/pbmp.h>
#include <soc/types.h>

#define SOC_PROPERTY_NAME_MAX   128
#define SOC_PROPERTY_VALUE_MAX  64

extern void soc_family_suffix_update(int unit);

extern char *soc_property_get_str(int unit, const char *name);
extern uint32 soc_property_get(int unit, const char *name, uint32 defl);
#if defined(SRC_SOC_COMMON_ACTIVATE_UNUSED) || defined(BCM_ESW_SUPPORT)
/* { */
extern uint32 soc_property_obj_attr_get(int unit, const char *prefix,
                                        const char *obj, int index,
                                        const char *attr, int scale,
					char *suffix, uint32 defl);
/* } */
#endif
extern pbmp_t soc_property_get_pbmp(int unit, const char *name,
                                    int defneg);
extern pbmp_t soc_property_get_pbmp_default(int unit, const char *name,
                                            pbmp_t def_pbmp);
#if defined(SRC_SOC_COMMON_ACTIVATE_UNUSED) || defined(BCM_DFE_SUPPORT)
/* { */
extern pbmp_t soc_property_suffix_num_pbmp_get(int unit, int num, const char *name,
                            const char *suffix, soc_pbmp_t pbmp_def);

extern void soc_property_get_bitmap_default(int unit, const char *name,
                              uint32 *bitmap, int max_words, uint32 *def_bitmap);
/* } */
#endif
extern int soc_property_get_csv(int unit, const char *name,
                                     int val_max, int *val_array);
extern char *soc_property_port_get_str(int unit, soc_port_t port,
                                       const char *name);
extern uint32 soc_property_port_get(int unit, soc_port_t port,
                                    const char *name, uint32 defl);
#if defined(SRC_SOC_COMMON_ACTIVATE_UNUSED) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
/* { */
extern char *soc_property_port_num_get_str(int unit, soc_port_t port,
                                        const char *name);
extern uint32 soc_property_port_num_get(int unit, soc_port_t port,
					const char *name, uint32 defl);
/* } */
#endif
extern char *soc_property_phy_get_str(int unit, soc_port_t port,
                                      int phy_num, int phy_port, int lane,
                                      const char *name);
#if defined(SRC_SOC_COMMON_ACTIVATE_UNUSED) || defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
/* { */
extern uint32 soc_property_phy_get(int unit, soc_port_t port,
				   int phy_num, int phy_port, int lane,
				   const char *name, uint32 defl);
/* } */
#endif
extern uint32 soc_property_phys_port_get(int unit, soc_port_t port,
					 const char *name, uint32 defl);
#if defined(SRC_SOC_COMMON_ACTIVATE_UNUSED) || defined(BCM_ESW_SUPPORT)
/* { */
extern uint32 soc_property_port_obj_attr_get(int unit, soc_port_t port,
                                             const char *prefix,
                                             const char *obj, int index,
                                             const char *attr, int scale,
					     char *suffix, uint32 defl);
/* } */
#endif
#if defined(SRC_SOC_COMMON_ACTIVATE_UNUSED) || defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
/* { */
extern int soc_property_port_get_csv(int unit, soc_port_t port,
                                     const char *name, int val_max,
				     int *val_array);
/* } */
#endif

extern uint32 soc_property_suffix_num_get(int unit, int num,
                                          const char *name,
                                          const char *suffix, uint32 defl);

extern uint32 soc_property_suffix_num_get_only_suffix(int unit, int num, const char *name,
                            const char *suffix, uint32 defl);

extern char* soc_property_suffix_num_str_get(int unit, int num, const char *name,
                            const char *suffix);
extern uint32 soc_property_port_suffix_num_get(int unit, soc_port_t port,
                                               int num, const char *name,
                                          const char *suffix, uint32 defl);
#if defined(SRC_SOC_COMMON_ACTIVATE_UNUSED) || defined(BCM_PETRA_SUPPORT)
/* { */
extern uint32 soc_property_port_suffix_num_match_port_get(int unit, soc_port_t port,
                                                   int num, const char *name,
                                                   const char *suffix, uint32 defl);
/* } */
#endif
extern char *soc_property_port_suffix_num_get_str(int unit, soc_port_t port,
                                               int num, const char *name,
                                          const char *suffix);
extern uint32 soc_property_cos_get(int unit, soc_cos_t cos,
                                    const char *name, uint32 defl);
extern uint32 soc_property_uc_get(int unit, int uc,
                                   const char *name, uint32 defl);
#if defined(SRC_SOC_COMMON_ACTIVATE_UNUSED) || defined(BCM_PETRA_SUPPORT)
/* { */
extern uint32 soc_property_ci_get(int unit, int ci,
                                   const char *name, uint32 defl);
/* } */
#endif
#if defined(SRC_SOC_COMMON_ACTIVATE_UNUSED) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DDR3_SUPPORT)
/* { */
extern int soc_property_ci_get_csv(int unit, int ci,
                                     const char *name, int val_max,
                                     int *val_array);
/* } */
#endif

extern char* soc_property_suffix_num_only_suffix_str_get(int unit, int num, const char *name,
                                                            const char *suffix);

#endif
