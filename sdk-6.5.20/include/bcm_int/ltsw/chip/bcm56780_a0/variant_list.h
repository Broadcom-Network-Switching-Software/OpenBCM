/*! \file variant_list.h
 *
 * BCM56780_A0_LTSW_VARIANT_LIST_ENTRY macros for the variant dispatch.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BCM56780_A0_LTSW_VARIANT_LIST_ENTRY
/*
 * BCM56780_A0_LTSW_VARIANT_LIST_ENTRY macros.
 *
 * Before including this file, define BCM56780_A0_LTSW_VARIANT_LIST_ENTRY
 * as a macro to operate on the following parameters:
 *
 *     #define BCM56780_A0_LTSW_VARIANT_LIST_ENTRY(_va,_ve,_vd,_r0)
 *
 *     _va: Variant name (lower case)
 *     _ve: Variant enum symbol (upper case)
 *     _vd: Variant Description
 *     _r0: Reserved
 *
 * BCM56780_A0_LTSW_VARIANT_LIST_ENTRY is undefined at the end of this file.
 */

BCM56780_A0_LTSW_VARIANT_LIST_ENTRY(dna_2_4_13,DNA_2_4_13,NULL,0)
BCM56780_A0_LTSW_VARIANT_LIST_ENTRY(ina_2_4_13,INA_2_4_13,NULL,0)

#undef BCM56780_A0_LTSW_VARIANT_LIST_ENTRY
#endif /* BCM56780_A0_LTSW_VARIANT_LIST_ENTRY */

