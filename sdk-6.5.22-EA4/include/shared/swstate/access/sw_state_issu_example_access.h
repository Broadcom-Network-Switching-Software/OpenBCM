/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 * 
 * DO NOT EDIT THIS FILE!
 * This file is auto-generated.
 * Edits to this file will be lost when it is regenerated.
 * search for 'sw_state_cbs_t' for the root of the struct
 */

#ifndef _SHR_SW_STATE_ISSU_EXAMPLE_ACCESS_H_
#define _SHR_SW_STATE_ISSU_EXAMPLE_ACCESS_H_

/********************************* access calbacks definitions *************************************/
/* this set of callbacks, are the callbacks used in the access calbacks struct 'sw_state_cbs_t' to */
/* access the data in 'sw_state_t'.                                                                */
/* the calbacks are inserted into the access struct by 'sw_state_access_cb_init'.                  */
/***************************************************************************************************/

#ifdef _SHR_SW_STATE_EXM
/* implemented by: sw_state_issu_example_set */
typedef int (*sw_state_issu_example_set_cb)(
    int unit, CONST sw_state_issu_example_t *issu_example);
#endif /* _SHR_SW_STATE_EXM*/ 

#ifdef _SHR_SW_STATE_EXM
/* implemented by: sw_state_issu_example_get */
typedef int (*sw_state_issu_example_get_cb)(
    int unit, sw_state_issu_example_t *issu_example);
#endif /* _SHR_SW_STATE_EXM*/ 

#ifdef _SHR_SW_STATE_EXM
/* implemented by: sw_state_issu_example_alloc */
typedef int (*sw_state_issu_example_alloc_cb)(
    int unit);
#endif /* _SHR_SW_STATE_EXM*/ 

#ifdef _SHR_SW_STATE_EXM
/* implemented by: sw_state_issu_example_is_allocated */
typedef int (*sw_state_issu_example_is_allocated_cb)(
    int unit, uint8 *is_allocated);
#endif /* _SHR_SW_STATE_EXM*/ 

#ifdef _SHR_SW_STATE_EXM
/* implemented by: sw_state_issu_example_free */
typedef int (*sw_state_issu_example_free_cb)(
    int unit);
#endif /* _SHR_SW_STATE_EXM*/ 

#ifdef _SHR_SW_STATE_EXM
/* implemented by: sw_state_issu_example_verify */
typedef int (*sw_state_issu_example_verify_cb)(
    int unit);
#endif /* _SHR_SW_STATE_EXM*/ 

#ifdef _SHR_SW_STATE_EXM
/* implemented by: sw_state_issu_example_o1234567890123456789012345678901234567890123456789_set */
typedef int (*sw_state_issu_example_o1234567890123456789012345678901234567890123456789_set_cb)(
    int unit, uint8 o1234567890123456789012345678901234567890123456789);
#endif /* _SHR_SW_STATE_EXM*/ 

#ifdef _SHR_SW_STATE_EXM
/* implemented by: sw_state_issu_example_o1234567890123456789012345678901234567890123456789_get */
typedef int (*sw_state_issu_example_o1234567890123456789012345678901234567890123456789_get_cb)(
    int unit, uint8 *o1234567890123456789012345678901234567890123456789);
#endif /* _SHR_SW_STATE_EXM*/ 

#ifdef _SHR_SW_STATE_EXM
/* implemented by: sw_state_issu_example_o1234567890123456789012345678901234567890123456789_verify */
typedef int (*sw_state_issu_example_o1234567890123456789012345678901234567890123456789_verify_cb)(
    int unit);
#endif /* _SHR_SW_STATE_EXM*/ 

#ifdef _SHR_SW_STATE_EXM
/* implemented by: sw_state_issu_example_myUint8_set */
typedef int (*sw_state_issu_example_myUint8_set_cb)(
    int unit, uint8 myUint8);
#endif /* _SHR_SW_STATE_EXM*/ 

#ifdef _SHR_SW_STATE_EXM
/* implemented by: sw_state_issu_example_myUint8_get */
typedef int (*sw_state_issu_example_myUint8_get_cb)(
    int unit, uint8 *myUint8);
#endif /* _SHR_SW_STATE_EXM*/ 

#ifdef _SHR_SW_STATE_EXM
/* implemented by: sw_state_issu_example_myUint8_verify */
typedef int (*sw_state_issu_example_myUint8_verify_cb)(
    int unit);
#endif /* _SHR_SW_STATE_EXM*/ 

#ifdef _SHR_SW_STATE_EXM
/* implemented by: sw_state_issu_example_myUint16_set */
typedef int (*sw_state_issu_example_myUint16_set_cb)(
    int unit, uint16 myUint16);
#endif /* _SHR_SW_STATE_EXM*/ 

#ifdef _SHR_SW_STATE_EXM
/* implemented by: sw_state_issu_example_myUint16_get */
typedef int (*sw_state_issu_example_myUint16_get_cb)(
    int unit, uint16 *myUint16);
#endif /* _SHR_SW_STATE_EXM*/ 

#ifdef _SHR_SW_STATE_EXM
/* implemented by: sw_state_issu_example_myUint16_verify */
typedef int (*sw_state_issu_example_myUint16_verify_cb)(
    int unit);
#endif /* _SHR_SW_STATE_EXM*/ 

#ifdef _SHR_SW_STATE_EXM
/* implemented by: sw_state_issu_example_myArray_set */
typedef int (*sw_state_issu_example_myArray_set_cb)(
    int unit, int myArray_idx_0, uint32 myArray);
#endif /* _SHR_SW_STATE_EXM*/ 

#ifdef _SHR_SW_STATE_EXM
/* implemented by: sw_state_issu_example_myArray_get */
typedef int (*sw_state_issu_example_myArray_get_cb)(
    int unit, int myArray_idx_0, uint32 *myArray);
#endif /* _SHR_SW_STATE_EXM*/ 

#ifdef _SHR_SW_STATE_EXM
/* implemented by: sw_state_issu_example_myArray_verify */
typedef int (*sw_state_issu_example_myArray_verify_cb)(
    int unit, int myArray_idx_0);
#endif /* _SHR_SW_STATE_EXM*/ 

#ifdef _SHR_SW_STATE_EXM
/* implemented by: sw_state_issu_example_myArray2D_set */
typedef int (*sw_state_issu_example_myArray2D_set_cb)(
    int unit, int myArray2D_idx_0, int myArray2D_idx_1, uint32 myArray2D);
#endif /* _SHR_SW_STATE_EXM*/ 

#ifdef _SHR_SW_STATE_EXM
/* implemented by: sw_state_issu_example_myArray2D_get */
typedef int (*sw_state_issu_example_myArray2D_get_cb)(
    int unit, int myArray2D_idx_0, int myArray2D_idx_1, uint32 *myArray2D);
#endif /* _SHR_SW_STATE_EXM*/ 

#ifdef _SHR_SW_STATE_EXM
/* implemented by: sw_state_issu_example_myArray2D_verify */
typedef int (*sw_state_issu_example_myArray2D_verify_cb)(
    int unit, int myArray2D_idx_0, int myArray2D_idx_1);
#endif /* _SHR_SW_STATE_EXM*/ 

#ifndef BCM_ISSU_TEST_AFTER_ISSU
#ifdef _SHR_SW_STATE_EXM
/* implemented by: sw_state_issu_example_myUint32_set */
typedef int (*sw_state_issu_example_myUint32_set_cb)(
    int unit, uint32 myUint32);
#endif /* _SHR_SW_STATE_EXM*/ 
#endif /* BCM_ISSU_TEST_AFTER_ISSU*/ 

#ifndef BCM_ISSU_TEST_AFTER_ISSU
#ifdef _SHR_SW_STATE_EXM
/* implemented by: sw_state_issu_example_myUint32_get */
typedef int (*sw_state_issu_example_myUint32_get_cb)(
    int unit, uint32 *myUint32);
#endif /* _SHR_SW_STATE_EXM*/ 
#endif /* BCM_ISSU_TEST_AFTER_ISSU*/ 

#ifndef BCM_ISSU_TEST_AFTER_ISSU
#ifdef _SHR_SW_STATE_EXM
/* implemented by: sw_state_issu_example_myUint32_verify */
typedef int (*sw_state_issu_example_myUint32_verify_cb)(
    int unit);
#endif /* _SHR_SW_STATE_EXM*/ 
#endif /* BCM_ISSU_TEST_AFTER_ISSU*/ 

#ifdef BCM_ISSU_TEST_AFTER_ISSU
#ifdef _SHR_SW_STATE_EXM
/* implemented by: sw_state_issu_example_myUint64_set */
typedef int (*sw_state_issu_example_myUint64_set_cb)(
    int unit, uint64 myUint64);
#endif /* _SHR_SW_STATE_EXM*/ 
#endif /* BCM_ISSU_TEST_AFTER_ISSU*/ 

#ifdef BCM_ISSU_TEST_AFTER_ISSU
#ifdef _SHR_SW_STATE_EXM
/* implemented by: sw_state_issu_example_myUint64_get */
typedef int (*sw_state_issu_example_myUint64_get_cb)(
    int unit, uint64 *myUint64);
#endif /* _SHR_SW_STATE_EXM*/ 
#endif /* BCM_ISSU_TEST_AFTER_ISSU*/ 

#ifdef BCM_ISSU_TEST_AFTER_ISSU
#ifdef _SHR_SW_STATE_EXM
/* implemented by: sw_state_issu_example_myUint64_verify */
typedef int (*sw_state_issu_example_myUint64_verify_cb)(
    int unit);
#endif /* _SHR_SW_STATE_EXM*/ 
#endif /* BCM_ISSU_TEST_AFTER_ISSU*/ 

/*********************************** access calbacks struct ****************************************/
/* this set of structs, rooted at 'sw_state_cbs_t' define the access layer for the entire SW state.*/
/* use this tree to alloc/free/set/get fields in the sw state rooted at 'sw_state_t'.              */
/* NOTE: 'sw_state_t' data should not be accessed directly.                                        */
/***************************************************************************************************/

#ifdef _SHR_SW_STATE_EXM
typedef struct sw_state_issu_example_o1234567890123456789012345678901234567890123456789_cbs_s {
    sw_state_issu_example_o1234567890123456789012345678901234567890123456789_set_cb set;
    sw_state_issu_example_o1234567890123456789012345678901234567890123456789_get_cb get;
    sw_state_issu_example_o1234567890123456789012345678901234567890123456789_verify_cb verify;
} sw_state_issu_example_o1234567890123456789012345678901234567890123456789_cbs_t;
#endif /* _SHR_SW_STATE_EXM*/ 

#ifdef _SHR_SW_STATE_EXM
typedef struct sw_state_issu_example_myUint8_cbs_s {
    sw_state_issu_example_myUint8_set_cb set;
    sw_state_issu_example_myUint8_get_cb get;
    sw_state_issu_example_myUint8_verify_cb verify;
} sw_state_issu_example_myUint8_cbs_t;
#endif /* _SHR_SW_STATE_EXM*/ 

#ifdef _SHR_SW_STATE_EXM
typedef struct sw_state_issu_example_myUint16_cbs_s {
    sw_state_issu_example_myUint16_set_cb set;
    sw_state_issu_example_myUint16_get_cb get;
    sw_state_issu_example_myUint16_verify_cb verify;
} sw_state_issu_example_myUint16_cbs_t;
#endif /* _SHR_SW_STATE_EXM*/ 

#ifdef _SHR_SW_STATE_EXM
typedef struct sw_state_issu_example_myArray_cbs_s {
    sw_state_issu_example_myArray_set_cb set;
    sw_state_issu_example_myArray_get_cb get;
    sw_state_issu_example_myArray_verify_cb verify;
} sw_state_issu_example_myArray_cbs_t;
#endif /* _SHR_SW_STATE_EXM*/ 

#ifdef _SHR_SW_STATE_EXM
typedef struct sw_state_issu_example_myArray2D_cbs_s {
    sw_state_issu_example_myArray2D_set_cb set;
    sw_state_issu_example_myArray2D_get_cb get;
    sw_state_issu_example_myArray2D_verify_cb verify;
} sw_state_issu_example_myArray2D_cbs_t;
#endif /* _SHR_SW_STATE_EXM*/ 

#ifndef BCM_ISSU_TEST_AFTER_ISSU
#ifdef _SHR_SW_STATE_EXM
typedef struct sw_state_issu_example_myUint32_cbs_s {
#ifndef BCM_ISSU_TEST_AFTER_ISSU
    sw_state_issu_example_myUint32_set_cb set;
#endif /* BCM_ISSU_TEST_AFTER_ISSU*/ 
#ifndef BCM_ISSU_TEST_AFTER_ISSU
    sw_state_issu_example_myUint32_get_cb get;
#endif /* BCM_ISSU_TEST_AFTER_ISSU*/ 
#ifndef BCM_ISSU_TEST_AFTER_ISSU
    sw_state_issu_example_myUint32_verify_cb verify;
#endif /* BCM_ISSU_TEST_AFTER_ISSU*/ 
} sw_state_issu_example_myUint32_cbs_t;
#endif /* _SHR_SW_STATE_EXM*/ 
#endif /* BCM_ISSU_TEST_AFTER_ISSU*/ 

#ifdef BCM_ISSU_TEST_AFTER_ISSU
#ifdef _SHR_SW_STATE_EXM
typedef struct sw_state_issu_example_myUint64_cbs_s {
#ifdef BCM_ISSU_TEST_AFTER_ISSU
    sw_state_issu_example_myUint64_set_cb set;
#endif /* BCM_ISSU_TEST_AFTER_ISSU*/ 
#ifdef BCM_ISSU_TEST_AFTER_ISSU
    sw_state_issu_example_myUint64_get_cb get;
#endif /* BCM_ISSU_TEST_AFTER_ISSU*/ 
#ifdef BCM_ISSU_TEST_AFTER_ISSU
    sw_state_issu_example_myUint64_verify_cb verify;
#endif /* BCM_ISSU_TEST_AFTER_ISSU*/ 
} sw_state_issu_example_myUint64_cbs_t;
#endif /* _SHR_SW_STATE_EXM*/ 
#endif /* BCM_ISSU_TEST_AFTER_ISSU*/ 

#ifdef _SHR_SW_STATE_EXM
typedef struct sw_state_issu_example_cbs_s {
#ifdef _SHR_SW_STATE_EXM
    sw_state_issu_example_set_cb set;
#endif /* _SHR_SW_STATE_EXM*/ 
#ifdef _SHR_SW_STATE_EXM
    sw_state_issu_example_get_cb get;
#endif /* _SHR_SW_STATE_EXM*/ 
#ifdef _SHR_SW_STATE_EXM
    sw_state_issu_example_alloc_cb alloc;
#endif /* _SHR_SW_STATE_EXM*/ 
#ifdef _SHR_SW_STATE_EXM
    sw_state_issu_example_is_allocated_cb is_allocated;
#endif /* _SHR_SW_STATE_EXM*/ 
#ifdef _SHR_SW_STATE_EXM
    sw_state_issu_example_free_cb free;
#endif /* _SHR_SW_STATE_EXM*/ 
#ifdef _SHR_SW_STATE_EXM
    sw_state_issu_example_verify_cb verify;
#endif /* _SHR_SW_STATE_EXM*/ 
    sw_state_issu_example_o1234567890123456789012345678901234567890123456789_cbs_t o1234567890123456789012345678901234567890123456789;
    sw_state_issu_example_myUint8_cbs_t myUint8;
    sw_state_issu_example_myUint16_cbs_t myUint16;
    sw_state_issu_example_myArray_cbs_t myArray;
    sw_state_issu_example_myArray2D_cbs_t myArray2D;
#ifndef BCM_ISSU_TEST_AFTER_ISSU
    sw_state_issu_example_myUint32_cbs_t myUint32;
#endif /* BCM_ISSU_TEST_AFTER_ISSU*/ 
#ifdef BCM_ISSU_TEST_AFTER_ISSU
    sw_state_issu_example_myUint64_cbs_t myUint64;
#endif /* BCM_ISSU_TEST_AFTER_ISSU*/ 
} sw_state_issu_example_cbs_t;

#endif /* _SHR_SW_STATE_EXM*/ 

int sw_state_issu_example_access_cb_init(int unit);

#endif /* _SHR_SW_STATE_ISSU_EXAMPLE_ACCESS_H_ */
