/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    hla.h
 * Purpose: Header file for HLA module
 */

#ifndef _HLA_H_
#define _HLA_H_

#include <soc/defs.h>
#include <bcm/types.h>

/* Error codes. See shared/hla/hla.h */
#define SOC_HLA_ERR_NONE                 (0x0)
#define SOC_HLA_ERR_INTERNAL             (0x0001)
#define SOC_HLA_ERR_NO_CCF               (0x0002)
#define SOC_HLA_ERR_BOUNDARY_CHK_FAIL    (0x0003)
#define SOC_HLA_ERR_CCF_BOUNDAY_CHK_FAIL (0x0004)
#define SOC_HLA_ERR_CCF_AUTH_FAIL        (0x0005)
#define SOC_HLA_ERR_CDB_NONE             (0x0006)
#define SOC_HLA_ERR_CDB_BOUNDAY_CHK_FAIL (0x0007)
#define SOC_HLA_ERR_CHIPID_MATCH_FAIL    (0x0008)
#define SOC_HLA_ERR_HASH_MATCH_FAIL      (0x0009)

int soc_hla_license_load(int unit,
        const uint8 *data,
        int len,
        uint32 *status);

#endif /* _HLA_H_ */
