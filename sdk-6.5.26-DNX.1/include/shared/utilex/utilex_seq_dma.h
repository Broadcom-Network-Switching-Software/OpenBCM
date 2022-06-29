/** \file utilex_seq_dma.h
 *
 * Init sequence DMA support.
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef UTILEX_SEQ_DMA_H_INCLUDED
#define UTILEX_SEQ_DMA_H_INCLUDED

#ifdef BCM_DNX_SUPPORT
#include <shared/utilex/utilex_seq.h>
#include <bcm_int/dnx/init/init.h>
#include <soc/dnx/utils/dnx_sbusdma_desc.h>

#define UTILEX_SEQ_SBUSDMA_DESC_FLUSH(unit)\
    if(dnx_sbusdma_desc_init_step_enabled_counter_return(unit)) \
    {                                                           \
        SHR_IF_ERR_EXIT(dnx_sbusdma_desc_wait_done(unit));      \
    }

#define UTILEX_SEQ_SBUSDMA_DESC_ADD_MEM(unit, mem, array_index, copyno, index, entry_data)  \
    if(dnx_sbusdma_desc_init_step_enabled_counter_return(unit))                             \
    {                                                                                       \
        rv = dnx_sbusdma_desc_add_mem(unit, mem, array_index, copyno, index, entry_data);   \
        return rv;                                                                          \
    }

#define UTILEX_SEQ_SBUSDMA_DESC_ADD_REG(unit, reg, port, index, data)   \
    if(dnx_sbusdma_desc_init_step_enabled_counter_return(unit))         \
    {                                                                   \
        rv = dnx_sbusdma_desc_add_reg(unit, reg, port, index, data);    \
        return rv;                                                      \
    }

#define UTILEX_SEQ_SBUSDMA_DESC_SUPPRESS(unit)\
    dnx_sbusdma_desc_init_suppress_counter_inc(unit);

#define UTILEX_SEQ_SBUSDMA_DESC_UNSUPPRESS(unit)\
    dnx_sbusdma_desc_init_suppress_counter_dec(unit);

#else /* BCM_DNX_SUPPORT */

#define UTILEX_SEQ_SBUSDMA_DESC_FLUSH(unit)
#define UTILEX_SEQ_SBUSDMA_DESC_ADD_MEM(unit, mem, array_index, copyno, index, entry_data)
#define UTILEX_SEQ_SBUSDMA_DESC_ADD_REG(unit, reg, port, index, data)
#define UTILEX_SEQ_SBUSDMA_DESC_SUPPRESS(unit)
#define UTILEX_SEQ_SBUSDMA_DESC_UNSUPPRESS(unit)

#endif /* BCM_DNX_SUPPORT */
#endif /* UTILEX_SEQ_DMA_H_INCLUDED */
