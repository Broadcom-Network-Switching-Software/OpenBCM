/*----------------------------------------------------------------------
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Broadcom Corporation
 * Proprietary and Confidential information
 * All rights reserved
 * This source file is the property of Broadcom Corporation, and
 * may not be copied or distributed in any isomorphic form without the
 * prior written consent of Broadcom Corporation.
 *---------------------------------------------------------------------
 * File       : sc_field_defines.h
 * Description: 
 *---------------------------------------------------------------------*/

/*##############################################################################
############################################################################# */
#define TEMOD2PLL_PLL_MODE_DIV_40       2
#define TEMOD2PLL_PLL_MODE_DIV_42       3
#define TEMOD2PLL_PLL_MODE_DIV_48       4
#define TEMOD2PLL_PLL_MODE_DIV_50       5
#define TEMOD2PLL_PLL_MODE_DIV_52       6
#define TEMOD2PLL_PLL_MODE_DIV_54       7
#define TEMOD2PLL_PLL_MODE_DIV_60       8
#define TEMOD2PLL_PLL_MODE_DIV_64       9
#define TEMOD2PLL_PLL_MODE_DIV_66       10
#define TEMOD2PLL_PLL_MODE_DIV_68       11
#define TEMOD2PLL_PLL_MODE_DIV_70       12
#define TEMOD2PLL_PLL_MODE_DIV_80       13
#define TEMOD2PLL_PLL_MODE_DIV_92       14
#define TEMOD2PLL_PLL_MODE_DIV_100      15
#define TEMOD2PLL_PLL_MODE_DIV_82P5     16
#define TEMOD2PLL_PLL_MODE_DIV_87P5     17
#define TEMOD2PLL_PLL_MODE_ILLEGAL      0 

/*############################################################################# */
#define TEMOD2PLL_PMA_OS_MODE_1         0 
#define TEMOD2PLL_PMA_OS_MODE_2         1 
#define TEMOD2PLL_PMA_OS_MODE_3         2 
#define TEMOD2PLL_PMA_OS_MODE_3_3       3 
#define TEMOD2PLL_PMA_OS_MODE_4         4 
#define TEMOD2PLL_PMA_OS_MODE_5         5 
#define TEMOD2PLL_PMA_OS_MODE_7_5       6 
#define TEMOD2PLL_PMA_OS_MODE_8         7 
#define TEMOD2PLL_PMA_OS_MODE_8_25      8 
#define TEMOD2PLL_PMA_OS_MODE_10        9 
#define TEMOD2PLL_PMA_OS_MODE_ILLEGAL   15

/*############################################################################# */
#define TEMOD2PLL_SCR_MODE_BYPASS       0
#define TEMOD2PLL_SCR_MODE_66B          1
#define TEMOD2PLL_SCR_MODE_80B          2
#define TEMOD2PLL_SCR_MODE_64B          3

/*############################################################################# */
#define TEMOD2PLL_ENCODE_MODE_NONE        0
#define TEMOD2PLL_ENCODE_MODE_CL48        1
#define TEMOD2PLL_ENCODE_MODE_CL48_2_LANE 2
#define TEMOD2PLL_ENCODE_MODE_CL36        3
#define TEMOD2PLL_ENCODE_MODE_CL82        4
#define TEMOD2PLL_ENCODE_MODE_CL49        5
#define TEMOD2PLL_ENCODE_MODE_BRCM        6
#define TEMOD2PLL_ENCODE_MODE_ILLEGAL     7


/*############################################################################# */
#define TEMOD2PLL_CL48_CHECK_END_OFF       0
#define TEMOD2PLL_CL48_CHECK_END_ON        1

/*############################################################################# */
#define TEMOD2PLL_SIGDET_FILTER_NONCX4     0
#define TEMOD2PLL_SIGDET_FILTER_CX4        1

/*############################################################################# */
#define TEMOD2PLL_BLOCKSYNC_MODE_NONE      0
#define TEMOD2PLL_BLOCKSYNC_MODE_CL49      1
#define TEMOD2PLL_BLOCKSYNC_MODE_CL82      2
#define TEMOD2PLL_BLOCKSYNC_MODE_8B10B     3
#define TEMOD2PLL_BLOCKSYNC_MODE_FEC       4
#define TEMOD2PLL_BLOCKSYNC_MODE_BRCM      5
#define TEMOD2PLL_BLOCKSYNC_MODE_ILLEGAL   7

/*############################################################################# */
#define TEMOD2PLL_R_REORDER_MODE_NONE      0
#define TEMOD2PLL_R_REORDER_MODE_CL48      1
#define TEMOD2PLL_R_REORDER_MODE_CL36      2
#define TEMOD2PLL_R_REORDER_MODE_CL36_CL48 3

/*############################################################################# */
#define TEMOD2PLL_CL36_DISABLE             0
#define TEMOD2PLL_CL36_ENABLE              1

/*############################################################################# */
#define TEMOD2PLL_R_DESCR1_MODE_BYPASS     0
#define TEMOD2PLL_R_DESCR1_MODE_66B        1
#define TEMOD2PLL_R_DESCR1_MODE_10B        2
#define TEMOD2PLL_R_DESCR1_MODE_ILLEGAL    3


/*############################################################################# */
#define TEMOD2PLL_DECODER_MODE_NONE       0
#define TEMOD2PLL_DECODER_MODE_CL49       1
#define TEMOD2PLL_DECODER_MODE_BRCM       2     
#define TEMOD2PLL_DECODER_MODE_ALU        3   
#define TEMOD2PLL_DECODER_MODE_CL48       4   
#define TEMOD2PLL_DECODER_MODE_CL36       5 
#define TEMOD2PLL_DECODER_MODE_CL82       6
#define TEMOD2PLL_DECODER_MODE_ILLEGAL    7

/*############################################################################# */
#define TEMOD2PLL_R_DESKEW_MODE_BYPASS        0
#define TEMOD2PLL_R_DESKEW_MODE_8B_10B        1
#define TEMOD2PLL_R_DESKEW_MODE_BRCM_66B      2
#define TEMOD2PLL_R_DESKEW_MODE_CL82_66B      3
#define TEMOD2PLL_R_DESKEW_MODE_CL36_10B      4
#define TEMOD2PLL_R_DESKEW_MODE_ILLEGAL       7

/*#############################################################################*/
#define TEMOD2PLL_DESC2_MODE_NONE             0
#define TEMOD2PLL_DESC2_MODE_CL49             1
#define TEMOD2PLL_DESC2_MODE_BRCM             2
#define TEMOD2PLL_DESC2_MODE_ALU              3
#define TEMOD2PLL_DESC2_MODE_CL48             4
#define TEMOD2PLL_DESC2_MODE_CL36             5
#define TEMOD2PLL_DESC2_MODE_CL82             6
#define TEMOD2PLL_DESC2_MODE_ILLEGAL          7


/*#############################################################################*/

#define TEMOD2PLL_R_DESC2_BYTE_DELETION_100M     0
#define TEMOD2PLL_R_DESC2_BYTE_DELETION_10M      1
#define TEMOD2PLL_R_DESC2_BYTE_DELETION_NONE     2

/*############################################################################*/

#define TEMOD2PLL_R_DEC1_DESCR_MODE_NONE         0
#define TEMOD2PLL_R_DEC1_DESCR_MODE_BRCM64B66B   1

/*#############################################################################*/

#define RTL_SPEED_100G_CR10                         0x29
#define RTL_SPEED_40G_CR4                           0x28
#define RTL_SPEED_40G_KR4                           0x27
#define RTL_SPEED_40G_X4                            0x1C
#define RTL_SPEED_32p7G_X4                          0x21
#define RTL_SPEED_31p5G_X4                          0x20
#define RTL_SPEED_25p45G_X4                         0x14
#define RTL_SPEED_21G_X4                            0x13
#define RTL_SPEED_20G_CR2                           0x3A
#define RTL_SPEED_20G_KR2                           0x39
#define RTL_SPEED_20G_X2                            0x1D
#define RTL_SPEED_20G_X4                            0x22
#define RTL_SPEED_20G_CX2                           0x1E
#define RTL_SPEED_20G_CX4                           0x12
#define RTL_SPEED_16G_X4                            0x0C
#define RTL_SPEED_15p75G_X2                         0x2C
#define RTL_SPEED_15G_X4                            0x0B
#define RTL_SPEED_13G_X4                            0x0A
#define RTL_SPEED_12p7G_X2                          0x19
#define RTL_SPEED_12p5G_X4                          0x09
#define RTL_SPEED_12G_X4                            0x08
#define RTL_SPEED_10p5G_X2                          0x17
#define RTL_SPEED_10G_KR1                           0x0F
#define RTL_SPEED_10G_CX2                           0x24
#define RTL_SPEED_10G_X2                            0x23
#define RTL_SPEED_10G_KX4                           0x0E
#define RTL_SPEED_10G_CX4                           0x07
#define RTL_SPEED_10G_X4                            0x06
#define RTL_SPEED_10G_CX1                           0x33
#define RTL_SPEED_6G_X4                             0x05
#define RTL_SPEED_5G_X4                             0x04
#define RTL_SPEED_5G_X2                             0x2A
#define RTL_SPEED_2p5G_X1                           0x03
#define RTL_SPEED_1G_KX1                            0x0D
#define RTL_SPEED_1000M                             0x02
#define RTL_SPEED_1G_CX1                            0x34
#define RTL_SPEED_100M                              0x01
#define RTL_SPEED_10M                               0x00
#define RTL_SPEED_5G_X1                             0x10
#define RTL_SPEED_6p36G_X1                          0x11
#define RTL_SPEED_10G_X2_NOSCRAMBLE                 0x15
#define RTL_SPEED_10G_CX2_NOSCRAMBLE                0x16
#define RTL_SPEED_10p5G_CX2_NOSCRAMBLE              0x18
#define RTL_SPEED_12p7G_CX2                         0x1A
#define RTL_SPEED_10G_X1                            0x1B
#define RTL_SPEED_10G_SFI                           0x1F
#define RTL_SPEED_12G_CISCO_R2                      0x25
#define RTL_SPEED_10G_CISCO_X2                      0x26
#define RTL_SPEED_3p125G_IL                         0x3C
#define RTL_SPEED_6p25_IL                           0x3D
#define RTL_SPEED_10p31G_IL                         0x3E
#define RTL_SPEED_10p96G_IL                         0x3F
#define RTL_SPEED_12p5G_IL                          0x40
#define RTL_SPEED_11p5G_IL                          0x41
#define RTL_SPEED_1p25G_IL                          0x42
#define RTL_SPEED_2G_FC                             0x2E
#define RTL_SPEED_4G_FC                             0x2F
#define RTL_SPEED_8G_FC                             0x30
#define RTL_SPEED_1G_CL73                           0x3B
#define RTL_SPEED_1000M_SGMII                       0x2D
#define RTL_SPEED_NONE                              0x2B


/*#############################################################################*/
