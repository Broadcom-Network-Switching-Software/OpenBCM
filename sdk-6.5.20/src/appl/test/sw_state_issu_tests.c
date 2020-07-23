/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: sw_state_issu_tests.c
 */
#include <appl/diag/system.h> /* for parse_small_integers */
#include <ibde.h>

#include <shared/bsl.h>

#include <soc/debug.h>
#include <soc/cmext.h>
#ifdef BCM_PETRA_SUPPORT
#include <soc/dpp/dpp_config_defs.h>
#endif /* BCM_PETRA_SUPPORT */
#include <shared/bsl.h>
#include <bcm/stack.h>
#include <bcm/l2.h>
#include <bcm/trunk.h>
#include <bcm/vswitch.h>
#include <bcm/switch.h>
#include <bcm/mpls.h>
#include <bcm/oam.h>
#include <bcm/bfd.h>
#include <bcm/policer.h>
#include <soc/drv.h>
#ifdef BCM_PETRA_SUPPORT
#if defined(INCLUDE_KBP) && !defined(BCM_88030)
#include <soc/dpp/drv.h>
#endif /* defined(INCLUDE_KBP) && !defined(BCM_88030) */
#endif /* BCM_PETRA_SUPPORT */
#include <bcm/init.h>
#include <bcm/error.h>
#include <bcm/link.h>

#include <sal/appl/io.h>
#include <sal/appl/sal.h>

#include <appl/diag/parse.h>
#include <appl/diag/test.h>
#include <appl/diag/shell.h>
#include <appl/diag/sysconf.h>
#include <appl/diag/bslenable.h>
#if (defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT))
#include <appl/diag/dcmn/init_deinit.h>
#endif /* BCM_PETRA_SUPPORT || BCM_DFE_SUPPORT */

#if defined (BCM_PETRA_SUPPORT) && !defined (__KERNEL__)

#include <shared/swstate/access/sw_state_access.h>

extern int init_deinit_test(int u, args_t *a, void *p);
extern shr_sw_state_t *sw_state[BCM_MAX_NUM_UNITS];


#include <stdlib.h>
#define ISSU_TEST_ACCESS sw_state_access[unit].issu_test
#define ISSU_PARSER_TABLE_ADD(member) parse_table_add(&pt, #member, PQ_INT, (void *) 0,\
     &(init_param->member), NULL)

#define ASSERT_ISSU(expr) do { \
	if (expr) { \
		cli_out("\tPASSED!\n"); \
  } else { \
		cli_out("\tFAILED!\n"); \
        rv = _SHR_E_INTERNAL; \
	} \
  } \
	while (0)

char sw_state_issu_usage[] = 
"SW_STATE_ISSU Usage:\n"
"ALL values should be 0 or 1.\n"
"  add_members=<value>.\n"
"  delete_members=<value>.\n"
"  change_array_size=<value>.\n"
"  change_primitive_types=<value>.\n"
"  pointed_value=<value>.\n"
"  pointed_array=<value>.\n"
"  dynamic_array_in_static_array=<value>.\n"
"  one_dim_array=<value>.\n"
"  two_dim_array=<value>.\n"
"  pointed_value_in_two_dim_array=<value>.\n" 
;

typedef struct sw_state_issu_test_init_param_s {
    uint32  unit;
    uint32  add_members;
    uint32  delete_members;
    uint32  change_array_size;
    uint32  change_primitive_types;
    uint32  pointed_value;
    uint32  pointed_array;
    uint32  dynamic_array_in_static_array;
    uint32  one_dim_array;
    uint32  two_dim_array;
    uint32  pointed_value_in_two_dim_array;
} sw_state_issu_test_init_param_t;


STATIC sw_state_issu_test_init_param_t sw_state_issu_test_p[SOC_MAX_NUM_DEVICES];

void cli_out_aligned_issu(char* str){
    cli_out("Test ISSU %-50s",str);
}


int
sw_state_issu_test(int unit, args_t *a, void *p)
{
    int rv = _SHR_E_NONE;
    sw_state_issu_test_init_param_t *init_param = p;
#ifdef SW_STATE_ISSU_TEST_WARMBOOT
    uint32 allocated_size = 0;
#endif
    int e1 = 0, e2 = 0, e3 = 0, e4 = 0 , e5 = 0;
    int i = 0;
    e1 = e1 + 0; e2 = e2 + 0; e3 = e3 + 0; e4 = e4 + 0; e5 = e5 + 0; i = i + 0;
    /* uint32 allocated_size = 0 */
    COMPILER_REFERENCE(a);
    COMPILER_REFERENCE(p);

    if (init_param->add_members) {
#ifndef SW_STATE_ISSU_TEST_WARMBOOT
        ISSU_TEST_ACCESS.add_members.alloc(unit);
        ISSU_TEST_ACCESS.add_members.e2.set(unit, 2);
        ISSU_TEST_ACCESS.add_members.e4.set(unit, 4);
#else
        cli_out_aligned_issu("add members:");
        ISSU_TEST_ACCESS.add_members.e1.get(unit, &e1);
        ISSU_TEST_ACCESS.add_members.e2.get(unit, &e2);
        ISSU_TEST_ACCESS.add_members.e3.get(unit, &e3);
        ISSU_TEST_ACCESS.add_members.e4.get(unit, &e4);
        ISSU_TEST_ACCESS.add_members.e5.get(unit, &e5);
        ASSERT_ISSU((e1 == 0 && e2 == 2 && e3 == 0 && e4 == 4 && e5 == 0));
#endif
    }

    if (init_param->delete_members) {
#ifndef SW_STATE_ISSU_TEST_WARMBOOT
        ISSU_TEST_ACCESS.delete_members.alloc(unit);
        ISSU_TEST_ACCESS.delete_members.e1.set(unit, 1);
        ISSU_TEST_ACCESS.delete_members.e2.set(unit, 2);
        ISSU_TEST_ACCESS.delete_members.e3.set(unit, 3);
        ISSU_TEST_ACCESS.delete_members.e4.set(unit, 4);
        ISSU_TEST_ACCESS.delete_members.e5.set(unit, 5);
#else
        cli_out_aligned_issu("delete members:");
        ISSU_TEST_ACCESS.delete_members.e2.get(unit, &e2);
        ISSU_TEST_ACCESS.delete_members.e4.get(unit, &e4);
        ASSERT_ISSU ((e2 == 2 && e4 == 4));
        shr_sw_state_allocated_size_get(unit,
            (uint8*)sw_state[unit]->issu_test->delete_members, &allocated_size);
        ASSERT_ISSU (allocated_size == sizeof (*(sw_state[unit]->issu_test->delete_members)));

#endif
   }

    if (init_param->change_array_size) {
#ifndef SW_STATE_ISSU_TEST_WARMBOOT
        ISSU_TEST_ACCESS.change_array_size.alloc(unit);
        ISSU_TEST_ACCESS.change_array_size.arr1.set(unit, 2, 2);
        ISSU_TEST_ACCESS.change_array_size.arr2.set(unit, 2, 2);
        ISSU_TEST_ACCESS.change_array_size.arr3.set(unit, 3, 3);
#else
        cli_out_aligned_issu("change array size:");
        ISSU_TEST_ACCESS.change_array_size.arr1.get(unit, 1, &e1);
        ISSU_TEST_ACCESS.change_array_size.arr2.get(unit, 2, &e2);
        ISSU_TEST_ACCESS.change_array_size.arr3.get(unit, 3, &e3);
        ASSERT_ISSU ((e1 == 0 && e2 == 2 && e3 == 3));
#endif        
    }
/*
    if (init_param->change_elements_order_in_struct) {
#ifndef SW_STATE_ISSU_TEST_WARMBOOT
        ISSU_TEST_ACCESS.change_elements_order_in_struct.alloc(unit);
        ISSU_TEST_ACCESS.change_elements_order_in_struct.e1.set(unit, 1);
        ISSU_TEST_ACCESS.change_elements_order_in_struct.e2.set(unit, 2);
        ISSU_TEST_ACCESS.change_elements_order_in_struct.e3.set(unit, 3);
        ISSU_TEST_ACCESS.change_elements_order_in_struct.e4.set(unit, 4);
        ISSU_TEST_ACCESS.change_elements_order_in_struct.e5.set(unit, 5);
#else
        cli_out_aligned_issu("change elements order in struct:");
        ISSU_TEST_ACCESS.change_elements_order_in_struct.e1.get(unit, &e1);
        ISSU_TEST_ACCESS.change_elements_order_in_struct.e2.get(unit, &e2);
        ISSU_TEST_ACCESS.change_elements_order_in_struct.e3.get(unit, &e3);
        ISSU_TEST_ACCESS.change_elements_order_in_struct.e4.get(unit, &e4);
        ISSU_TEST_ACCESS.change_elements_order_in_struct.e5.get(unit, &e5);
        ASSERT_ISSU (e1 == 1 && e2 == 2 && e3 == 3 && e4 == 4 && e5 == 5);
#endif                
    }
*/
    if (init_param->change_primitive_types) {
#ifndef SW_STATE_ISSU_TEST_WARMBOOT
        ISSU_TEST_ACCESS.change_primitive_types.alloc(unit);
        ISSU_TEST_ACCESS.change_primitive_types.myUint8.set(unit, 1 << 7);
        ISSU_TEST_ACCESS.change_primitive_types.myUint32.set(unit, 31);
        for (i=0; i < 3; i++) {
            ISSU_TEST_ACCESS.change_primitive_types.myUint8Arr.set(unit,i,i+1);
            ISSU_TEST_ACCESS.change_primitive_types.myUint32Arr.set(unit,i,i+1);
        }

#else
        cli_out_aligned_issu("change primitive types:");
        {
          uint32 myUint32 = 0;
          uint8 myUint8 = 0;
          uint32 myUint32_1 = 0, myUint32_2 = 0, myUint32_3 = 0;
          uint8 myUint8_1 = 0, myUint8_2 = 0, myUint8_3 = 0;
          ISSU_TEST_ACCESS.change_primitive_types.myUint8.get(unit, &myUint32);
          ISSU_TEST_ACCESS.change_primitive_types.myUint32.get(unit, &myUint8);
          ASSERT_ISSU (((myUint32 == (1 << 7)) && (myUint8 == 31)));

          ISSU_TEST_ACCESS.change_primitive_types.myUint8Arr.get(unit,0,&myUint32_1);
          ISSU_TEST_ACCESS.change_primitive_types.myUint8Arr.get(unit,1,&myUint32_2);
          ISSU_TEST_ACCESS.change_primitive_types.myUint8Arr.get(unit,2,&myUint32_3);
          ASSERT_ISSU (((myUint32_1 == 1 && myUint32_2 == 2 && myUint32_3 == 3)));

          ISSU_TEST_ACCESS.change_primitive_types.myUint32Arr.get(unit,0,&myUint8_1);
          ISSU_TEST_ACCESS.change_primitive_types.myUint32Arr.get(unit,1,&myUint8_2);
          ISSU_TEST_ACCESS.change_primitive_types.myUint32Arr.get(unit,2,&myUint8_3);
          ASSERT_ISSU (((myUint8_1 == 1 && myUint8_2 == 2 && myUint8_3 == 3)));
          /*
          cli_out("\tmyUint32 = %d expected %d\n",myUint32, 1 << 7);
          cli_out("\tmyUint32 = %x expected %x\n",myUint32, 1 << 7);
          cli_out("\tmyUint8 = %d expected %d\n",myUint8, 31);
          cli_out("\tmyUint8 = %x expected %x\n",myUint8, 31); 
          */ 
        
        }
#endif          
    }

    if (init_param->pointed_value) {
#ifndef SW_STATE_ISSU_TEST_WARMBOOT
        ISSU_TEST_ACCESS.pointed_value.alloc(unit);
        ISSU_TEST_ACCESS.pointed_value.a.alloc(unit);
        ISSU_TEST_ACCESS.pointed_value.a.set(unit,11);
        
        ISSU_TEST_ACCESS.pointed_value.b.alloc(unit);
        ISSU_TEST_ACCESS.pointed_value.b.set(unit,-13);
#else
        cli_out_aligned_issu("pointed value:");        
        ISSU_TEST_ACCESS.pointed_value.a.get(unit,&e1);
        ISSU_TEST_ACCESS.pointed_value.b.get(unit,&e2);
        ASSERT_ISSU (e1 == 11 && e2 == -13);
#endif
    }

    if (init_param->pointed_array) {
#ifndef SW_STATE_ISSU_TEST_WARMBOOT        
        ISSU_TEST_ACCESS.pointed_array.alloc(unit);
        ISSU_TEST_ACCESS.pointed_array.a.alloc(unit,3);
        ISSU_TEST_ACCESS.pointed_array.a.set(unit,0,0);
        ISSU_TEST_ACCESS.pointed_array.a.set(unit,1,1);
        ISSU_TEST_ACCESS.pointed_array.a.set(unit,2,2);
#else
        cli_out_aligned_issu("pointed array:");                
        ISSU_TEST_ACCESS.pointed_array.a.get(unit,0,&e1);
        ISSU_TEST_ACCESS.pointed_array.a.get(unit,1,&e2);
        ISSU_TEST_ACCESS.pointed_array.a.get(unit,2,&e3);
        ASSERT_ISSU (e1 == 0 && e2 == 1 && e3 == 2);
#endif
    }

    if (init_param->dynamic_array_in_static_array) {
#ifndef SW_STATE_ISSU_TEST_WARMBOOT        
        ISSU_TEST_ACCESS.dynamic_array_in_static_array.alloc(unit);
        ISSU_TEST_ACCESS.dynamic_array_in_static_array.a.alloc(unit,1,3);
        ISSU_TEST_ACCESS.dynamic_array_in_static_array.a.set(unit,1,0,0);
        ISSU_TEST_ACCESS.dynamic_array_in_static_array.a.set(unit,1,1,1);
        ISSU_TEST_ACCESS.dynamic_array_in_static_array.a.set(unit,1,2,2);
#else
        cli_out_aligned_issu("dynamic array in static array:");     
        ISSU_TEST_ACCESS.dynamic_array_in_static_array.a.get(unit,1,0,&e1);
        ISSU_TEST_ACCESS.dynamic_array_in_static_array.a.get(unit,1,1,&e2);
        ISSU_TEST_ACCESS.dynamic_array_in_static_array.a.get(unit,1,2,&e3);
        ASSERT_ISSU (e1 == 0 && e2 == 1 && e3 == 2);
#endif
    }
/*
    if (init_param->change_from_primitive_to_struct) {
#ifndef SW_STATE_ISSU_TEST_WARMBOOT        
        ISSU_TEST_ACCESS.change_from_primitive_to_struct.alloc(unit);
        ISSU_TEST_ACCESS.change_from_primitive_to_struct.element.set(unit,31);
#else
        cli_out_aligned_issu("change from primitive to struct:");
        ISSU_TEST_ACCESS.change_from_primitive_to_struct.element.a.get(unit,&e1);
        ASSERT_ISSU (e1 == 0);
#endif
        
    }


    if (init_param->change_from_struct_to_primitive) {
#ifndef SW_STATE_ISSU_TEST_WARMBOOT        
        ISSU_TEST_ACCESS.change_from_struct_to_primitive.alloc(unit);
        ISSU_TEST_ACCESS.change_from_struct_to_primitive.element.a.set(unit,31);
#else
        cli_out_aligned_issu("change from struct to primitive:");
        ISSU_TEST_ACCESS.change_from_struct_to_primitive.element.get(unit,&e1);
        ASSERT_ISSU (e1 == 0);
#endif
    }
*/
    if (init_param->one_dim_array) {
#ifndef SW_STATE_ISSU_TEST_WARMBOOT        
        ISSU_TEST_ACCESS.one_dim_array.alloc(unit);
        ISSU_TEST_ACCESS.one_dim_array.a.set(unit,0,0);
        ISSU_TEST_ACCESS.one_dim_array.a.set(unit,1,1);
        ISSU_TEST_ACCESS.one_dim_array.a.set(unit,2,2);
#else
        cli_out_aligned_issu("one dim array:");
        ISSU_TEST_ACCESS.one_dim_array.a.get(unit,0,&e1);
        ISSU_TEST_ACCESS.one_dim_array.a.get(unit,1,&e2);
        ISSU_TEST_ACCESS.one_dim_array.a.get(unit,2,&e3);
        ASSERT_ISSU (e1 == 0 && e2 == 1 && e3 == 2);
#endif
    }

    if (init_param->two_dim_array) {
#ifndef SW_STATE_ISSU_TEST_WARMBOOT        
        ISSU_TEST_ACCESS.two_dim_array.alloc(unit);
        ISSU_TEST_ACCESS.two_dim_array.a.set(unit,0,1,15);
        ISSU_TEST_ACCESS.two_dim_array.a.set(unit,1,0,17);
#else
        cli_out_aligned_issu("two dim array:");
        ISSU_TEST_ACCESS.two_dim_array.a.get(unit,0,1,&e1);
        ISSU_TEST_ACCESS.two_dim_array.a.get(unit,1,0,&e2);
        ASSERT_ISSU (e1 == 15 && e2 == 17);
#endif
    }

    if (init_param->pointed_value_in_two_dim_array) {
#ifndef SW_STATE_ISSU_TEST_WARMBOOT           
        ISSU_TEST_ACCESS.pointed_value_in_two_dim_array.alloc(unit);
        ISSU_TEST_ACCESS.pointed_value_in_two_dim_array.a.alloc(unit,2,1);
        ISSU_TEST_ACCESS.pointed_value_in_two_dim_array.a.set(unit,2,1,70);
#else
        cli_out_aligned_issu("pointed value in two dim array:");       
        ISSU_TEST_ACCESS.pointed_value_in_two_dim_array.a.get(unit,2,1,&e1);
        ASSERT_ISSU (e1 == 70);
#endif
    }
#ifndef SW_STATE_ISSU_TEST_WARMBOOT

{
    int rc = 0;
    rc = bcm_switch_control_set(unit, bcmSwitchControlSync, 1);
    if (rc != BCM_E_NONE) {
        printf("ERROR: in bcm_switch_control_set-bcmSwitchControlSync, failed to sync, rc= %d.\n", rc);
        return(rc);
    }
}

#endif

    return rv;	
}

/*
 * Function:     sw_state_issu_usage_parse
 * Purpose:      sw_state_issu_usage_parse command
 * Parameters:   u - unit number to operate on
 *               a - args (none expected)
 *               init_param - return parsed parameter struct 
 * Returns:      CMD_OK/CMD_FAIL.
 */
cmd_result_t
sw_state_issu_usage_parse(int unit, args_t *a, sw_state_issu_test_init_param_t *init_param)
{
    int rv = CMD_OK;
    int parse_arg = 0;
    parse_table_t pt;

    parse_table_init(unit, &pt);

ISSU_PARSER_TABLE_ADD(add_members);
ISSU_PARSER_TABLE_ADD(delete_members);
ISSU_PARSER_TABLE_ADD(change_array_size);
ISSU_PARSER_TABLE_ADD(change_primitive_types);
ISSU_PARSER_TABLE_ADD(pointed_value);
ISSU_PARSER_TABLE_ADD(pointed_array);
ISSU_PARSER_TABLE_ADD(dynamic_array_in_static_array);
ISSU_PARSER_TABLE_ADD(one_dim_array);
ISSU_PARSER_TABLE_ADD(two_dim_array);
ISSU_PARSER_TABLE_ADD(pointed_value_in_two_dim_array);
      
    parse_arg = parse_arg_eq(a, &pt);
    if (parse_arg <= 0 || parse_arg > 14) {
        cli_out("%s: Invalid option: %s\n",
                ARG_CMD(a), ARG_CUR(a));
        cli_out("%s\n",sw_state_issu_usage);
        rv = CMD_FAIL;
    }
    parse_arg_eq_done(&pt);
    return rv;
}

int
sw_state_issu_test_init(int unit, args_t *a, void **p)
{
    sw_state_issu_test_init_param_t *init_param;
    uint8 is_allocated = 0;
    init_param = &sw_state_issu_test_p[unit];
    sal_memset(init_param, 0x0, sizeof(sw_state_issu_test_init_param_t));

    ISSU_TEST_ACCESS.is_allocated(unit, &is_allocated);
    if (!is_allocated) {
        ISSU_TEST_ACCESS.alloc(unit);
    }

    *p = init_param;
    return sw_state_issu_usage_parse(unit, a, init_param);
}

int
sw_state_issu_test_done(int unit, void *p)
{
    cli_out("SW_STATE_ISSU Test Done\n");
    return 0;
}
#endif
