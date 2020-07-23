/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose:    Implement header for ser correction.
 */

#ifndef _SAND_SER_CORRECTION_H_
#define _SAND_SER_CORRECTION_H_


#include <soc/defs.h>
#include <shared/shrextend/shrextend_debug.h>


shr_error_e calc_ecc(
        int unit,
        const uint32 data_len_bits,
        const uint32 *data,
        uint32 *ecc_data);

shr_error_e ecc_correction(
        int unit,
        soc_mem_t mem,
        const uint32 data_len_bits,
        int xor_flag,
        uint32 *data, 
        uint32 *ecc);

shr_error_e sand_mdb_ecc_correction(
    int unit,
    const uint32 data_len_bits,
    uint32* data,
    uint32* ecc);

#endif 

