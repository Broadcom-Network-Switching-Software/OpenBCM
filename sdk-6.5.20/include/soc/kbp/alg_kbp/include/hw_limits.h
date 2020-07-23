/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#ifndef __HW_LIMITS_H
#define __HW_LIMITS_H

#ifdef __cplusplus
extern "C" {
#endif




#define ALIGN_UP(value, align) (((value) + ((align) - 1)) & ~((align) - 1))





#define KBP_BITS_IN_BYTE (8)



#define KBP_HW_MAX_KPUS (4)



#define KBP_HW_MAX_SEARCH_DB (8)



#define KBP_MAX_INSTRUCTION_CYCLES (2)


#define KBP_MAX_CASCADED_DEVICES (4)


#define KBP_MAX_BROADCAST_DEVICES (8)



#define KBP_MAX_PARITY_FIFO_DEPTH (16)


#define KBP_OP_MAX_PARITY_FIFO_DEPTH (32)


#define KBP_MAX_PLACE_HOLDER_PARITY_FIFO_DEPTH (512)


#define KBP_MAX_INTF_ERROR_FIFO_DEPTH (485)



#define KBP_NUMBER_OF_INDEX_BITS_TO_SHIFT (512)






#define KBP_HW_MAX_RANGE_COMPARES (4)



#define KBP_HW_MAX_DBA_WIDTH_1 (640)



#define KBP_HW_MAX_DBA_WIDTH_8 (KBP_HW_MAX_DBA_WIDTH_1 / KBP_BITS_IN_BYTE)




#define KBP_HW_MIN_DBA_WIDTH_1 (80)



#define KBP_HW_MIN_DBA_WIDTH_8 (KBP_HW_MIN_DBA_WIDTH_1 / KBP_BITS_IN_BYTE)




#define KBP_HW_MAX_AB_DEPTH (4096)


#define KBP_HW_MAX_UDA_WIDTH_1 (256)


#define KBP_HW_MAX_UDA_WIDTH_8 (KBP_HW_MAX_UDA_WIDTH_1 / KBP_BITS_IN_BYTE)


#define KBP_HW_MIN_UDA_WIDTH_1 (32)


#define KBP_HW_MIN_UDA_WIDTH_8 (KBP_HW_MIN_UDA_WIDTH_1 / KBP_BITS_IN_BYTE)



#define KBP_HW_MAX_DBA_BLOCKS (256)



#define KBP_HW_NUM_SB_PER_UDM (4)


#define KBP_HW_MINIMUM_PRIORITY (0x3FFFFF)


#define KBP_HW_PRIORITY_SIZE_1 (24)


#define KBP_HW_PRIORITY_SIZE_8 (KBP_HW_PRIORITY_SIZE_1 / KBP_BITS_IN_BYTE)



#define KBP_HW_MAX_AB (256)



#define KBP_HW_MAX_DBA_SB (64)



#define KBP_HW_MAX_LTRS (128)


#define KBP_HW_MAX_LTR_DUAL_PORT (64)


#define KBP_HW_MAX_UDA_SB (256)


#define KBP_12K_HW_MAX_UDA_SB (64)


#define KBP_LPM_KEY_MAX_WIDTH_1 (160)


#define KBP_LPM_KEY_MAX_WIDTH_8 (20)


#define KBP_HW_MAX_SEARCH_KEY_WIDTH_1 (1280)

#define KBP_HW_MAX_SEARCH_KEY_WIDTH_8 (160)



#define KBP_HW_HIT_INDEX_WIDTH_1 (24)


#define HW_MAX_RANGE_UNITS (16)


#define KBP_HW_MAX_UDM_SIZE_MB (8)


#define KBP_HW_MAX_UDA_SB_SIZE_MB (2)


 #define KBP_MAX_BMR_USED_FOR_LARGER_WIDTH (3)

 
 #define KBP_MAX_META_PRIORITY_VALUE (3)

 
 #define KBP_NUM_BYTES_IN_SB ((KBP_HW_MAX_UDA_SB_SIZE_MB * 1024 * 1024) / 8)

 
 #define KBP_NUM_AD_WORD_IN_SB (KBP_NUM_BYTES_IN_SB / KBP_HW_MIN_UDA_WIDTH_8)


 

 #define KBP_MAX_NUM_OF_SEGMENTS_PER_KEY   (15)



 #define KBP_MAX_NUM_HB_BLOCKS              (28)




#ifdef __cplusplus
}
#endif
#endif                          
