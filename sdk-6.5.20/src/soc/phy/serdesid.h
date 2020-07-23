/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        serdesid.h
 * Purpose:     Defines useful SerDes ID fields .
 */
#ifndef _PHY_SERDESID_H
#define _PHY_SERDESID_H

#define SERDES_ID0_REV_LETTER_A                      (0 << 14)
#define SERDES_ID0_REV_LETTER_B                      (1 << 14)
#define SERDES_ID0_REV_LETTER_C                      (2 << 14)
#define SERDES_ID0_REV_LETTER_D                      (3 << 14)

#define SERDES_ID0_REV_NUMBER_0                      (0 << 11)
#define SERDES_ID0_REV_NUMBER_1                      (1 << 11)
#define SERDES_ID0_REV_NUMBER_2                      (2 << 11)
#define SERDES_ID0_REV_NUMBER_3                      (3 << 11)
#define SERDES_ID0_REV_NUMBER_4                      (4 << 11)
#define SERDES_ID0_REV_NUMBER_5                      (5 << 11)
#define SERDES_ID0_REV_NUMBER_6                      (6 << 11)
#define SERDES_ID0_REV_NUMBER_7                      (7 << 11)

#define SERDES_ID0_BONDING_WIRE                      (0 << 9)
#define SERDES_ID0_BONDING_FLIP_CHIP                 (1 << 9)

#define SERDES_ID0_TECH_PROC_90NM                    (0 << 6)
#define SERDES_ID0_TECH_PROC_65NM                    (1 << 6)

#define SERDES_ID0_MODEL_NUMBER_SERDES_CL73          (0x00 << 0)
#define SERDES_ID0_MODEL_NUMBER_XGXS_16G             (0x01 << 0)
#define SERDES_ID0_MODEL_NUMBER_HYPERCORE            (0x02 << 0)
#define SERDES_ID0_MODEL_NUMBER_HYPERLITE            (0x03 << 0) 
#define SERDES_ID0_MODEL_NUMBER_PCIE                 (0x04 << 0)
#define SERDES_ID0_MODEL_NUMBER_1P25GBD_SERDES       (0x05 << 0)
#define SERDES_ID0_MODEL_NUMBER_XGXS_CL73_90NM       (0x1d << 0)
#define SERDES_ID0_MODEL_NUMBER_SERDES_CL73_90NM     (0x1e << 0)

#define SERDES_ID1_MULTIPLICITY_1              (1 << 12) 
#define SERDES_ID1_MULTIPLICITY_2              (2 << 12)
#define SERDES_ID1_MULTIPLICITY_3              (3 << 12)
#define SERDES_ID1_MULTIPLICITY_4              (4 << 12) 
#define SERDES_ID1_MULTIPLICITY_5              (5 << 12)
#define SERDES_ID1_MULTIPLICITY_6              (6 << 12)
#define SERDES_ID1_MULTIPLICITY_7              (7 << 12)
#define SERDES_ID1_MULTIPLICITY_8              (8 << 12)
#define SERDES_ID1_MULTIPLICITY_9              (9 << 12)
#define SERDES_ID1_MULTIPLICITY_10             (10 << 12)
#define SERDES_ID1_MULTIPLICITY_11             (11 << 12)
#define SERDES_ID1_MULTIPLICITY_12             (12 << 12)

#endif /* _PHY_SERDESID_H */

