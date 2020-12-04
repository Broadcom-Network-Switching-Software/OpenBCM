/*
* 
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
*
* INFO:
 */
#ifndef _SHR_SW_STATE_ISSU_DEFS_H
#define _SHR_SW_STATE_ISSU_DEFS_H

#define SW_STATE_CONST_ARR_SIZE 3
#define SW_STATE_CONST_ARR_SIZE_2 2
#ifndef SW_STATE_ISSU_TEST_WARMBOOT
#define SW_STATE_ARR1_SIZE 3
#define SW_STATE_ARR2_SIZE 3
#define SW_STATE_ARR3_SIZE 4
#endif
#ifdef SW_STATE_ISSU_TEST_WARMBOOT
#define SW_STATE_ARR1_SIZE 2
#define SW_STATE_ARR2_SIZE 3
#define SW_STATE_ARR3_SIZE 5
#endif

#ifndef SW_STATE_ISSU_TEST_WARMBOOT /* before ISSU */
typedef uint8 sw_state_uint8_32;
typedef uint32 sw_state_uint32_8;
#else                               /* after ISSU */
typedef uint32 sw_state_uint8_32;
typedef uint8 sw_state_uint32_8;
#endif

/* 
   add_members
   --------------------------------
   before ISSU :     e2,     e4
   after  ISSU : e1, e2, e3, e4, e5
   --------------------------------
*/
typedef struct sw_state_issu_add_members_s {
#ifdef SW_STATE_ISSU_TEST_WARMBOOT
    int e1;
#endif
    int e2;
#ifdef SW_STATE_ISSU_TEST_WARMBOOT
    int e3;
#endif
    int e4;
#ifdef SW_STATE_ISSU_TEST_WARMBOOT
    int e5;
#endif
}sw_state_issu_add_members_t;



/* 
   delete_members
   --------------------------------
   before ISSU : e1, e2, e3, e4, e5
   after  ISSU :     e2,     e4
   --------------------------------
*/
typedef struct sw_state_issu_delete_members_s {
#ifndef SW_STATE_ISSU_TEST_WARMBOOT
    int e1;
#endif
    int e2;
#ifndef SW_STATE_ISSU_TEST_WARMBOOT
    int e3;
#endif
    int e4;
#ifndef SW_STATE_ISSU_TEST_WARMBOOT
    int e5;
#endif

}sw_state_issu_delete_members_t;


/* 
   change_array_size
   --------------------------------
                      arr1, arr2, arr3 
   size before ISSU :   [3],  [3],  [4]
   size after  ISSU :   [2],  [3],  [5]
   --------------------------------
*/


typedef struct sw_state_issu_change_array_size_s {
    int arr1[SW_STATE_ARR1_SIZE];
    int arr2[SW_STATE_ARR2_SIZE];
    int arr3[SW_STATE_ARR3_SIZE];
} sw_state_issu_change_array_size_t;


/* 
   change_elements_order
   --------------------------------
   before ISSU : e1, e2, e3, e4, e5
   after  ISSU : e3, e1, e4, e2, e5
   --------------------------------
*/
/*
    There is a problem with the auto-coder with this test.
    Flags association conflict
    
typedef struct sw_state_issu_change_elements_order_in_struct_s {
#ifndef SW_STATE_ISSU_TEST_WARMBOOT
    int e1;
    int e2;
    int e3;
    int e4;
    int e5;
#endif
#ifdef SW_STATE_ISSU_TEST_WARMBOOT
    int e3;
    int e1;
    int e4;
    int e2;
    int e5;
#endif
} sw_state_issu_change_elements_order_in_struct_t;
*/
/* 
   change_primitive_types
   --------------------------------------------------
   before ISSU : uint8  myUint8  |  uint32 myUint32
   after  ISSU : uint32 myUint8  |  uint8  myUint32
   --------------------------------------------------
*/
typedef struct sw_state_issu_change_primitive_types_s {
    sw_state_uint8_32 myUint8;
    sw_state_uint32_8 myUint32;
    sw_state_uint8_32 myUint8Arr[SW_STATE_CONST_ARR_SIZE];
    sw_state_uint32_8 myUint32Arr[SW_STATE_CONST_ARR_SIZE];
} sw_state_issu_change_primitive_types_t;


/* 
   pointed_value
   ------------------------
   before/after ISSU : int *a 
   ------------------------
   we check that the pointed value doesn't change
*/
typedef struct sw_state_issu_pointed_value_s {
    PARSER_HINT_PTR int *a;
    PARSER_HINT_PTR int *b;
} sw_state_issu_pointed_value_t;


/* 
   pointed_array
   ------------------------
   before/after ISSU : int *a
   ------------------------
   we check that the pointed value doesn't change when a is a pointer to array
*/
typedef struct sw_state_issu_pointed_array_s {
    PARSER_HINT_ARR int *a;
} sw_state_issu_pointed_array_t;



/* 
   dynamic_array_in_static_array
   ------------------------
   before/after ISSU : int *a[]
   ------------------------
   we check that the pointed value doesn't change when a is an array of dynamically allocated arrays
*/
typedef struct sw_state_issu_dynamic_array_in_static_array_s {
    PARSER_HINT_ARR int *a[SW_STATE_CONST_ARR_SIZE];
} sw_state_issu_dynamic_array_in_static_array_t;




/*
typedef struct element_s {
    int a;
    int b;
} element_t;
 
   change_from_primitive_to_struct
   --------------------------------------------------
   before ISSU : int    element
   after  ISSU : struct element_s {
                    int a, int b
                 }      element
   --------------------------------------------------

typedef struct sw_state_issu_change_from_primitive_to_struct_s {
#ifndef SW_STATE_ISSU_TEST_WARMBOOT
    int element;
#endif
#ifdef SW_STATE_ISSU_TEST_WARMBOOT
    element_t element;
#endif
} sw_state_issu_change_from_primitive_to_struct_t;




   change_from_struct_to_primitive
   --------------------------------------------------
   before  ISSU : struct element_s {
                    int a, int b
                 }       element 
   after   ISSU : int    element
   --------------------------------------------------

typedef struct sw_state_issu_change_from_struct_to_primitive_s {
#ifndef SW_STATE_ISSU_TEST_WARMBOOT
    element_t element;
#endif
#ifdef SW_STATE_ISSU_TEST_WARMBOOT
    int element;
#endif
} sw_state_issu_change_from_struct_to_primitive_t;

*/


/* 
   one_dim_array
   ------------------------
   before/after ISSU : int a[]
   ------------------------
*/
typedef struct sw_state_issu_one_dim_array_s {
    int a[SW_STATE_CONST_ARR_SIZE];
} sw_state_issu_one_dim_array_t;


/* 
   two_dim_array
   ------------------------
   before/after ISSU : int a[][]
   ------------------------
*/
typedef struct sw_state_issu_two_dim_array_s {
    int a[SW_STATE_CONST_ARR_SIZE_2][SW_STATE_CONST_ARR_SIZE_2];
} sw_state_issu_two_dim_array_t;



/* 
   pointed_value_in_two_dim_array
   ------------------------
   before/after ISSU : int *a[][]
   ------------------------
*/
typedef struct sw_state_issu_pointed_value_in_two_dim_array_s {
    PARSER_HINT_PTR int *a[SW_STATE_CONST_ARR_SIZE][SW_STATE_CONST_ARR_SIZE_2];
} sw_state_issu_pointed_value_in_two_dim_array_t;



/* 
   to_empty_struct
   --------------------------------------------------
   before  ISSU : to_empty_struct {
                    int a
                 }        
   after   ISSU : to_empty_struct {
                    -----
                 }   
   --------------------------------------------------

typedef struct to_empty_struct_s {
#ifndef SW_STATE_ISSU_TEST_WARMBOOT
    int a;
#endif
} to_empty_struct_t;
*/

/* 
   to_empty_struct
   --------------------------------------------------
   after   ISSU : to_empty_struct {
                    -----
                 }   
   before  ISSU : to_empty_struct {
                    int a
                 }        
   --------------------------------------------------

typedef struct from_empty_struct_s {
#ifdef SW_STATE_ISSU_TEST_WARMBOOT
    int a;
#endif
} from_empty_struct_t;
*/

typedef struct sw_state_issu_test_s {
    sw_state_issu_add_members_t                       *add_members;
    sw_state_issu_delete_members_t                    *delete_members;
    sw_state_issu_change_array_size_t                 *change_array_size;
    sw_state_issu_change_primitive_types_t            *change_primitive_types;
    sw_state_issu_pointed_value_t                     *pointed_value;
    sw_state_issu_pointed_array_t                     *pointed_array;
    sw_state_issu_dynamic_array_in_static_array_t     *dynamic_array_in_static_array;
    sw_state_issu_one_dim_array_t                     *one_dim_array;
    sw_state_issu_two_dim_array_t                     *two_dim_array;
    sw_state_issu_pointed_value_in_two_dim_array_t    *pointed_value_in_two_dim_array;
} sw_state_issu_test_t;

#endif /* _SHR_SW_STATE_ISSU_DEFS_H */
