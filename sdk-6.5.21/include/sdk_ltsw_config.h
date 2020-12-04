/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Headerfile barrier for integrating SDKLT into SDK6.
 */

#ifndef __SDK_LTSW_CONFIG_H__
#define __SDK_LTSW_CONFIG_H__

#ifdef SHR_BITDCL
#undef SHR_BITDCL
#endif

#ifdef SHR_BITALLOCSIZE
#undef SHR_BITALLOCSIZE
#endif

#ifdef SHR_BITDCLNAME
#undef SHR_BITDCLNAME
#endif

#ifdef SHR_BITGET
#undef SHR_BITGET
#endif

#ifdef SHR_BITSET
#undef SHR_BITSET
#endif

#ifdef SHR_BITCLR
#undef SHR_BITCLR
#endif

#ifdef SHR_BITSET_RANGE
#undef SHR_BITSET_RANGE
#endif

#ifdef SHR_BITCLR_RANGE
#undef SHR_BITCLR_RANGE
#endif

#ifdef SHR_BITCOPY_RANGE
#undef SHR_BITCOPY_RANGE
#endif

#ifdef SHR_BITAND_RANGE
#undef SHR_BITAND_RANGE
#endif

#ifdef SHR_BITOR_RANGE
#undef SHR_BITOR_RANGE
#endif

#ifdef SHR_BITXOR_RANGE
#undef SHR_BITXOR_RANGE
#endif

#ifdef SHR_BITREMOVE_RANGE
#undef SHR_BITREMOVE_RANGE
#endif

#ifdef SHR_BITNEGATE_RANGE
#undef SHR_BITNEGATE_RANGE
#endif

#ifdef SHR_BITNULL_RANGE
#undef SHR_BITNULL_RANGE
#endif

#ifdef SHR_BITEQ_RANGE
#undef SHR_BITEQ_RANGE
#endif

#ifdef SHR_BITCOUNT_RANGE
#undef SHR_BITCOUNT_RANGE
#endif

#ifdef SHR_BIT_ITER
#undef SHR_BIT_ITER
#endif

#ifdef INVALIDr
#undef INVALIDr
#endif

#ifdef INVALIDm
#undef INVALIDm
#endif

#ifdef INVALIDf
#undef INVALIDf
#endif

#ifdef RAND_MAX
#undef RAND_MAX
#endif

#ifdef _SAL_TIME_H
#define SAL_TIME_H
#endif

#endif /* __SDK_LTSW_CONFIG_H__ */
