/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Built-in Self Test for ARL, L3, and CBP memories
 */
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_APPL_TESTS

#include <shared/bsl.h>
#include <soc/defs.h>
#include <soc/mem.h>

#if defined(BCM_PETRA_SUPPORT) || defined(BCM_88750_A0)

#include <soc/util.h>
#include <soc/dcmn/dcmn_defs.h>
#ifdef BCM_DFE_SUPPORT
#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dfe/fe1600/fe1600_defs.h>
#include <soc/dfe/cmn/dfe_fabric_cell.h>
#include <soc/dfe/fe1600/fe1600_fabric_cell.h>
#endif /* BCM_DFE_SUPPORT */
#include <appl/diag/test.h>
#include <appl/diag/parse.h>
#include <bcm/types.h>
#include <bcm/fabric.h>
#include <bcm/port.h>
#include "testlist.h"
#include <bcm/stack.h>
#include <bcm/error.h>
#include <sal/appl/io.h>
#include <sal/core/time.h>
#include <bcm_int/common/debug.h>

#ifdef BCM_PETRA_SUPPORT
#include <soc/dpp/JER/jer_ocb_dram_buffers.h>
#include <soc/dpp/mbcm.h>
#endif /* BCM_PETRA_SUPPORT */

#if defined(BCM_PETRA_SUPPORT) && !defined(_ARAD_MC_NO_IMPLEMENTATION_SPECIFIC_TESTS)
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/multicast_imp.h>
#include <soc/dpp/QAX/qax_multicast_imp.h>
#include <shared/swstate/access/sw_state_access.h>
#endif

#ifdef BCM_88750_A0
#endif /* BCM_88750_A0 */

#if defined(BCM_PETRA_SUPPORT)
#include <bcm_int/petra_dispatch.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/error.h>
#include <bcm/multicast.h>
#include <bcm_int/common/multicast.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/ARAD/arad_ports.h>
#include <sal/appl/sal.h>

/* print error message */
#define TEST_PRINT(stuff) do { if (debug_level[unit] == MC_PRINT_ALL) {bsl_printf stuff;} } while (0);
#define INFO_PRINT(stuff) do { if (debug_level[unit] <= MC_PRINT_INFO_AND_ERRORS) {bsl_printf stuff;} } while (0);
#define ERROR_PRINT(stuff) bsl_printf stuff

#define TEST_ASSERT(exp)  do { if (!(exp)) {mc_test_stop(unit, __LINE__, (#exp), 0); } } while (0)

#define MC_TEST_USE_ASSERT_ON_ERRORS
#ifdef MC_TEST_USE_ASSERT_ON_ERRORS
#undef BCM_EXIT
#define BCM_EXIT do {TEST_ASSERT(!_rv); goto exit;} while (0)
#endif /* MC_TEST_USE_ASSERT_ON_ERRORS */

#undef BCMDNX_ERR_EXIT_MSG
#undef _BCM_MSG
#undef _BCM_MSG_NO_UNIT
#undef BCMDNX_ALLOC
#define _BCM_MSG(string) "[%d] unit %d: " string "\n", __LINE__, unit
#define _BCM_MSG_NO_UNIT(string) "[%d]: " string "\n", __LINE__
#define BCMDNX_ERR_EXIT_MSG(_rc, stuff) \
      do { \
            bsl_printf stuff; \
            _rv = _rc; \
            BCM_EXIT; \
      } while (0)
#define BCMDNX_ALLOC(pointer, mem_size, alloc_comment)   \
  do {   \
      if((pointer) == NULL) { \
          (pointer) = sal_alloc((mem_size), (alloc_comment)); \
      } else {\
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BCM_MSG_NO_UNIT("attempted to allocate to a non NULL pointer: %s"), (alloc_comment))); \
      } \
  } while (0)


#define BCM_PETRA_MULTICAST_NOF_BRIDGE_ROUTE    4096
#define _BCM_TRILL_ENCAP_PORT_SHIFT         16
#define _BCM_TRILL_ENCAP_MASK               0xffff

/* from src/soc/dpp/ARAD/arad_multicast_ingress.c */
#define ARAD_MC_ING_DESTINATION_FLD_TYPE_TM_FLOW 0x20000
#define ARAD_MC_ING_DESTINATION_FLD_TYPE_PORT 0x00000
#define ARAD_MC_ING_DESTINATION_FLD_TYPE_MCID 0x10000

#define QAX_MC_ING_DESTINATION_FLD_TYPE_TM_FLOW (0)
#define QAX_MC_ING_DESTINATION_FLD_TYPE_SPA (1 << 16)


#define JER_MC_ING_DESTINATION_FLD_TYPE_TM_FLOW (1 << 17)
#define JER_MC_ING_DESTINATION_FLD_TYPE_SYSPORT (1 << 16)
#define JER_MC_ING_DESTINATION_FLD_TYPE_LAG (3 << 15)

#define DPP_MC_ING_DESTINATION_FLD_TYPE_TM_FLOW (SOC_IS_QAX(unit) ? QAX_MC_ING_DESTINATION_FLD_TYPE_TM_FLOW : (SOC_IS_JERICHO(unit)?JER_MC_ING_DESTINATION_FLD_TYPE_TM_FLOW:ARAD_MC_ING_DESTINATION_FLD_TYPE_TM_FLOW))
#define DPP_MC_ING_DESTINATION_FLD_TYPE_PORT    (SOC_IS_QAX(unit) ? QAX_MC_ING_DESTINATION_FLD_TYPE_SPA     : (SOC_IS_JERICHO(unit)?JER_MC_ING_DESTINATION_FLD_TYPE_SYSPORT:ARAD_MC_ING_DESTINATION_FLD_TYPE_PORT))
#define DPP_MC_ING_DESTINATION_FLD_TYPE_LAG     (SOC_IS_QAX(unit) ? QAX_MC_ING_DESTINATION_FLD_TYPE_SPA     : (SOC_IS_JERICHO(unit)?JER_MC_ING_DESTINATION_FLD_TYPE_LAG:ARAD_MC_ING_DESTINATION_FLD_TYPE_PORT))


/* from include/soc/dpp/ARAD/arad_multicast_linked_list.h */
#define ARAD_MULT_ING_UNOCCUPIED_BASE_QUEUE_INDICATION         0x3FFFF
#define QAX_MULT_ING_UNOCCUPIED_BASE_QUEUE_INDICATION          0x1FFFF

#define _BCM_MAX_ENCAP_VALUE                0xffff

#define ARAD_MC_MAX_GROUP_REPLICATIONS_INGRESS 4094
#define ARAD_MC_MAX_GROUP_REPLICATIONS_EGRESS 4095
#define ARAD_MC_MAX_GROUP_REPLICATIONS_ANY 4095
#define ARAD_MC_VLAN_BITMAP_ENTRY_SIZE 9
#define ARAD_MC_TABLE_ENTRY_SIZE 2
#define ARAD_IRDB_ENTRY_SIZE 2
#define ARAD_IRDB_NOF_ENTRIES 4096 

#define MC_NOF_OUTLIF_TO_PORT 0x10000 /* Number of outlifs in original Arad, in mapping from outlifs to ports */

/* internal testing functions */

/* test the of mcdb swdb free entries coherency, return 0 on success (no errors) */
int _dpp_mcds_test_free_entries( 
    SOC_SAND_IN int unit
);

/* object representing a multicast member */
typedef struct {
    uint32 data0;
    uint32 data1;
} mc_member;

/*
 * date0 bits 0-18: CUD 
 *            19,20,21: flags: TDM, used, is ingress
 *            22-31: destination
 * data1 bits 0-7:   destination 
 *            8-27:  link pointer (NEXT)
 *            28-31: allocation bits
*/
#define MC_MEMBER_CUD_BITS 19 /* number of CUD bits */
#define MC_MEMBER_CUD_MASK ((((uint32)1) << MC_MEMBER_CUD_BITS) - 1)
#define MC_NOF_OUTLIF_IN_FIELD (1 << MC_MEMBER_CUD_BITS)

#define MC_MEMBER_DATA0_IS_TDM_BIT MC_MEMBER_CUD_BITS /* tdm bit in data0: 19 */
#define MC_MEMBER_DATA0_IS_USED_BIT (MC_MEMBER_DATA0_IS_TDM_BIT + 1) /* is used bit in data0: 20 */
#define MC_MEMBER_DATA0_IS_INGRESS_BIT (MC_MEMBER_DATA0_IS_USED_BIT + 1) /* is ingress bit in data0: 21 */

#define MC_MEMBER_DATA0_DEST_START_BIT (MC_MEMBER_DATA0_IS_INGRESS_BIT + 1) /* start bit of dest in data0: 22 */
#define MC_MEMBER_DEST_BITS 18 /* number of dest bits */
#define MC_MEMBER_DEST_OUTLIF_ONLY 0x3ffff
#define MC_MEMBER_DEST_BITMAP_OFFSET 1024
#define MC_MEMBER_DATA0_DEST_BITS (32 - MC_MEMBER_DATA0_DEST_START_BIT) /* number of dest bits in data0: 10 */
#define MC_MEMBER_DATA1_DEST_BITS (MC_MEMBER_DEST_BITS - MC_MEMBER_DATA0_DEST_BITS) /* number of dest bits in data1: 8 */
#define MC_MEMBER_DEST_VAL_OVERFLOW (((uint32)1) << MC_MEMBER_DEST_BITS)
#define MC_MEMBER_DATA0_DEST_MASK ((((uint32)1) << MC_MEMBER_DATA0_DEST_BITS) - 1)
#define MC_MEMBER_DATA1_DEST_MASK ((((uint32)1) << MC_MEMBER_DATA1_DEST_BITS) - 1)

#define MC_MEMBER_NEXT_BITS 20 /* number of bits for next pointer (in data1) */
#define MC_MEMBER_DATA1_NEXT_BIT MC_MEMBER_DATA1_DEST_BITS /* start bit of next pointer in data1: 8 */
#define MC_MEMBER_NEXT_MAX_VAL ((((uint32)1) << MC_MEMBER_NEXT_BITS) - 1)

#define MC_MEMBER_DATA1_ALLOC_BIT (MC_MEMBER_DATA1_NEXT_BIT + MC_MEMBER_NEXT_BITS) /* start bit of allocation number in data1: 28 */
#define MC_MEMBER_ALLOC_BITS (32 - MC_MEMBER_DATA1_ALLOC_BIT) /* number of bits for allocation number in data1: 4 */
#define MC_MEMBER_ALLOC_MASK ((((uint32)1) << MC_MEMBER_ALLOC_BITS) - 1)

#define MC_MEMBER_DATA0_DATA_MASK (~((((uint32)1) << MC_MEMBER_DATA0_IS_TDM_BIT) | (((uint32)1) << MC_MEMBER_DATA0_IS_USED_BIT) | (((uint32)1) << MC_MEMBER_DATA0_IS_INGRESS_BIT)))
#define MC_MEMBER_DATA1_DATA_MASK MC_MEMBER_DATA1_DEST_MASK



#define mc_max(a,b)	((a) > (b) ? (a) : (b))
#define mc_min(a,b)	((a) < (b) ? (a) : (b))

/*
 * These will hold the starting member of each MC group.
 * If the group is not open, it's type (in/egress) will be wrong.
 */
#define MAX_VLANS ARAD_MULT_NOF_MULT_VLAN_ERP_GROUPS
#define UINT32SPER_VLAN 8 /* 256 bit */
#define PORTS_PER_VLAN (UINT32SPER_VLAN * 32)

#define MAX_ADDITIONAL_MEMBERS (3*256*1024) /* max number of non initial members supported by the test (each IRR_MCDB entry may have up to 3 mc_members)*/
#define ADDITIONAL_MEMBERS_PER_ALLOC (12*4096) /* number of non initial members to allocate per allocation */
#define ADDITIONAL_MEMBERS_MAX_ALLOCS (MAX_ADDITIONAL_MEMBERS / ADDITIONAL_MEMBERS_PER_ALLOC) /* number of non initial members to allocate per allocation */

typedef enum { /* which assert technique to use in multicast tests */
  MC_ASSERT_FUCNTION = 1, /* divides be zero */
  MC_ASSERT_NORMAL   = 0, /* regular SDK assert */
  MC_ASSERT_PRINT    = 2  /* print error and continue */
} mc_test_assert_type;

typedef enum { /* print filtering in multicast tests */
  MC_PRINT_ALL = -1,
  MC_PRINT_INFO_AND_ERRORS = 0,
  MC_PRINT_ONLY_ERRORS = 1
} mc_test_debug_level;

mc_test_assert_type assert_type[SOC_MAX_NUM_DEVICES] = {0};
mc_test_debug_level debug_level[SOC_MAX_NUM_DEVICES] = {0};

typedef struct mc_test_data_s {
    mc_member* ingress_mc_groups;
    mc_member* egress_mc_groups;
    uint32* vlan_bitmaps;

    mc_member* additional_members[ADDITIONAL_MEMBERS_MAX_ALLOCS];
    int additional_members_last_alloc;
    unsigned additional_members_left_in_last_alloc;
    unsigned additional_members_in_free_list;
    mc_member* free_list_first;
    mc_member* free_list_last;
    uint32 nof_vlan_egress_mc_groups;
    uint32 nof_egress_bitmaps;
} mc_test_data_t;

mc_test_data_t *test_data[SOC_MAX_NUM_DEVICES] = {0};

void mc_test_set_assert_type(int unit, int atype) {
    assert_type[unit] = (mc_test_assert_type)atype;
}

void mc_test_set_debug_level(int unit, int level) {
    debug_level[unit] = (mc_test_debug_level)level;
}

void mc_test_stop(int unit, unsigned line, const char *expr, int i) { /* function used to handle filed asserts and can be caught the debugger. In gdb use:   b mc_test_stop */
    bsl_printf ("Assertion %s failed, at %s:%d\n",expr, __FILE__, line);
    assert(unit >= 0 && unit < SOC_MAX_NUM_DEVICES);
    /* There is an assertion that the unit is in the required range */
    /* coverity[overrun-local:FALSE] */
    if (assert_type[unit] == MC_ASSERT_NORMAL) { /* regular SDK assert */
        assert(i);
    } else if (assert_type[unit] == MC_ASSERT_FUCNTION) { /* divides be zero ; It's needed for debugging */
        /* coverity[divide_arg] */
        i = i / i;
    } /* else continue */
}


/* 1 bit determining if the member is of a TDM MC group */
unsigned mc_member_is_tdm_get(int unit, const mc_member *member)
{
    TEST_ASSERT(member);
    return (member->data0 >> MC_MEMBER_DATA0_IS_TDM_BIT) & 1;
}
void mc_member_is_tdm_set(int unit, mc_member *member, unsigned value)
{
    TEST_ASSERT(member && value < 2);
    member->data0 &= ~(1 << MC_MEMBER_DATA0_IS_TDM_BIT);
    member->data0 |= ((uint32)value) << MC_MEMBER_DATA0_IS_TDM_BIT;
    TEST_ASSERT(mc_member_is_tdm_get(unit, member) == value);
}

/* 1 bit determining if the member object is used by a MC group or on the free list */
unsigned mc_member_is_used_get(const mc_member *member)
{
    return (member->data0 >> MC_MEMBER_DATA0_IS_USED_BIT) & 1;
}
void mc_member_is_used_set(int unit, mc_member *member, unsigned value)
{
    TEST_ASSERT(member && value < 2);
    member->data0 &= ~(1 << MC_MEMBER_DATA0_IS_USED_BIT);
    member->data0 |= ((uint32)value) << MC_MEMBER_DATA0_IS_USED_BIT;
    TEST_ASSERT(mc_member_is_used_get(member) == value);
}

/* 1 bit determining if the member is of an INGRESS MC group */
unsigned mc_member_is_ingress_get(const mc_member *member)
{
    return (member->data0 >> MC_MEMBER_DATA0_IS_INGRESS_BIT) & 1;
}
void mc_member_is_ingress_set(int unit, mc_member *member, unsigned value)
{
    TEST_ASSERT(member && value < 2);
    member->data0 &= ~(1 << MC_MEMBER_DATA0_IS_INGRESS_BIT);
    member->data0 |= ((uint32)value) << MC_MEMBER_DATA0_IS_INGRESS_BIT;
    TEST_ASSERT(mc_member_is_ingress_get(member) == value);
}
void mc_member_clear_ingress(mc_member *member)
{
    member->data0 = 0;
    member->data1 = 0;
}
void mc_member_clear_egress(mc_member *member)
{
    member->data0 = 0;
    member->data1 = 0;
}

/* 16 bits representing the object's CUD (outlif)  */
uint32 mc_member_cud_get(const mc_member *member)
{
    return member->data0 & MC_MEMBER_CUD_MASK;
}
void mc_member_cud_set(int unit, mc_member *member, uint32 value)
{
    TEST_ASSERT(member && value <= MC_MEMBER_CUD_MASK);
    member->data0 &= ~MC_MEMBER_CUD_MASK;
    member->data0 |= value;
    TEST_ASSERT(mc_member_cud_get(member) == value);
}

/* 20 bits representing the next MC member in the linked list */
uint32 mc_member_nextptr_get(const mc_member *member)
{
    return (member->data1 >> MC_MEMBER_DATA1_NEXT_BIT) & MC_MEMBER_NEXT_MAX_VAL;
}
void mc_member_nextptr_set(int unit, mc_member *member, uint32 value)
{
    TEST_ASSERT(member && value <= MC_MEMBER_NEXT_MAX_VAL);
    member->data1 &= ~(MC_MEMBER_NEXT_MAX_VAL << MC_MEMBER_DATA1_NEXT_BIT);
    member->data1 |= value << MC_MEMBER_DATA1_NEXT_BIT;
    TEST_ASSERT(mc_member_nextptr_get(member) == value);
}

/* 18 bits representing the destination. For Ingress it is an 18 bit destination, for egress it is 8b pp-dsp or 15b bit map pointer list */
uint32 mc_member_dest_get(const mc_member *member)
{
    return (member->data0 >> MC_MEMBER_DATA0_DEST_START_BIT) |
           ((member->data1 & MC_MEMBER_DATA1_DEST_MASK) << MC_MEMBER_DATA0_DEST_BITS);
}
void mc_member_dest_set(int unit, mc_member *member, uint32 value)
{
    TEST_ASSERT(member && value < MC_MEMBER_DEST_VAL_OVERFLOW);
    member->data0 &= ~(MC_MEMBER_DATA0_DEST_MASK << MC_MEMBER_DATA0_DEST_START_BIT);
    member->data0 |= value << MC_MEMBER_DATA0_DEST_START_BIT;
    member->data1 &= ~MC_MEMBER_DATA1_DEST_MASK;
    member->data1 |= value >> MC_MEMBER_DATA0_DEST_BITS;
    TEST_ASSERT(mc_member_dest_get(member) == value);
}

/* 9 bits  (8 of which are used) representing the allocation number containing this object */
uint32 mc_member_alloc_get(const mc_member *member)
{
    return member->data1 >> MC_MEMBER_DATA1_ALLOC_BIT;
}
void mc_member_alloc_set(int unit, mc_member *member, uint32 value)
{
    TEST_ASSERT(member && value <= MC_MEMBER_ALLOC_MASK);
    member->data1 &= ~(MC_MEMBER_ALLOC_MASK << MC_MEMBER_DATA1_ALLOC_BIT);
    member->data1 |= value << MC_MEMBER_DATA1_ALLOC_BIT;
    TEST_ASSERT(mc_member_alloc_get(member) == value);
}

/* copy destination and cud from source to destination */
/*inline*/ void mc_member_copy_dest_cud(mc_member *destination, const mc_member *source)
{
    destination->data0 &= ~(MC_MEMBER_CUD_MASK | (MC_MEMBER_DATA0_DEST_MASK << MC_MEMBER_DATA0_DEST_START_BIT));
    destination->data0 |= source->data0 & (MC_MEMBER_CUD_MASK | (MC_MEMBER_DATA0_DEST_MASK << MC_MEMBER_DATA0_DEST_START_BIT));
    destination->data1 &= ~MC_MEMBER_DATA1_DEST_MASK;
    destination->data1 |= source->data1 & MC_MEMBER_DATA1_DEST_MASK;
}
/* copy source to destination */
/*inline*/ void mc_member_copy(mc_member *destination, const mc_member *source)
{
    destination->data0 = source->data0;
    destination->data1 = source->data1;
}
/* compare the data fields (return 0 if not the same) in member1 and member2 */
/*inline*/ int mc_member_same(const mc_member *member1, const mc_member *member2)
{
    return !(((member1->data0 ^ member2->data0) & MC_MEMBER_DATA0_DATA_MASK) ||
             ((member1->data1 ^ member2->data1) & MC_MEMBER_DATA1_DATA_MASK));
}
/* compare the data fields (return 0 if the same) in member1 and member2 */
/*inline*/ int mc_member_not_same(const mc_member *member1, const mc_member *member2)
{
    return ((member1->data0 ^ member2->data0) & MC_MEMBER_DATA0_DATA_MASK) ||
           ((member1->data1 ^ member2->data1) & MC_MEMBER_DATA1_DATA_MASK);
}

/*
 * Compare the bit value of two mc_members a,b. Returns return a negative, zero or positive value
 * if member1 is less than, equal to, or greater than member2 respectively.
 */
/*inline*/ int mc_member_compare(const mc_member *member1, const mc_member *member2)
{
    return member1->data1 < member2->data1 ? -1 :
           member1->data1 > member2->data1 ?  1 :
           member1->data0 < member2->data0 ? -1 :
           member1->data0 > member2->data0 ?  1 : 0;
}


int mc_member_compare_generic(void* member1,void* member2)
{
  return mc_member_compare(member1, member2);
}

/* * swap the values of two mc_members a,b. */
/*inline*/ void mc_member_swap(mc_member *member1, mc_member *member2)
{
    uint32 temp = member1->data0;
    member1->data0 = member2->data0;
    member2->data0 = temp;
    temp = member1->data1;
    member1->data1 = member2->data1;
    member2->data1 = temp;
}

/* sort the input array of mc_members of size size, using mc_member_compare for comparisons */
/*inline*/ void mc_member_sort(const mc_member *array, uint32 size)
{
    soc_sand_os_qsort((void*)array, size, sizeof(mc_member), mc_member_compare_generic);
}

typedef struct {
    uint32 dest;
    uint32 cud;
    uint32 cud2;
} mc_member_data;

static INLINE int32 mc_member_data_compare(const mc_member_data* member1,const mc_member_data* member2)
{
    int32 res = (int32)member1->dest - (int32)member2->dest;
    return res ? res : (int32)member1->cud - (int32)member2->cud;
}

int mc_member_data_compare_generic(void* member1,void* member2)
{
    int res = (int)((const mc_member_data*)member1)->dest - (int)((const mc_member_data*)member2)->dest;
    return res ? res : (int)((const mc_member_data*)member1)->cud - (int)((const mc_member_data*)member2)->cud;
}

/* sort the input array of mc_member_datas of size size, using mc_member_data_compare_generic for comparisons */
/*inline*/ void mc_member_data_sort(mc_member_data *array, uint32 size)
{
    soc_sand_os_qsort((void*)array, size, sizeof(mc_member_data), mc_member_data_compare_generic);
}


/*
 * Initialize Arad MC testing infrastructure, return 0 on success.
 * supports testing only one device at a time, and no concurrent operations.
 */
int multicast_arad_test_init(int unit)
{
    unsigned i;
    int success = 0;
    mc_test_data_t **datap = test_data + unit;
    mc_test_data_t *data = 0;
    BCMDNX_INIT_FUNC_DEFS;

    TEST_ASSERT(unit >= 0 && unit < SOC_MAX_NUM_DEVICES && !*datap);
    TEST_ASSERT(ADDITIONAL_MEMBERS_MAX_ALLOCS <= MC_MEMBER_ALLOC_MASK + 1); /* verify that allocation numbers fits in bit field */
    BCMDNX_ALLOC(*datap, sizeof(mc_test_data_t), "multicast_test_data");
    data = *datap;
    sal_memset(data, 0, sizeof(mc_test_data_t)) ;
    data->additional_members_last_alloc = -1; /* number of the last allocation of additional members */
    data->nof_egress_bitmaps = 8192; 

    TEST_ASSERT(!data->ingress_mc_groups && !data->egress_mc_groups && data->additional_members_last_alloc == -1 && !*data->additional_members);

    BCMDNX_ALLOC(data->egress_mc_groups, sizeof(mc_member) * SOC_DPP_CONFIG(unit)->tm.nof_mc_ids * SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores, "multicast_egress_groups"); 
    if (!data->egress_mc_groups) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BCM_MSG("Failed allocating test memory for egress multicast groups")));
    }
    for (i = SOC_DPP_CONFIG(unit)->tm.nof_mc_ids * SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores; i ;) { 
        mc_member_clear_egress(data->egress_mc_groups + --i); /* mark as closed */
    }

    {
        BCMDNX_ALLOC(data->ingress_mc_groups, sizeof(mc_member) * SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids, "multicast_ingress_groups");
        if (!data->ingress_mc_groups) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BCM_MSG("Failed allocating test memory for ingress multicast groups")));
        }
        for (i = SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids; i ;) {
            mc_member_clear_ingress(data->ingress_mc_groups + --i); /* mark as closed */
        }

        if (!SOC_IS_QAX(unit)) { /* In QAX we don't have direct bitmap */
          BCMDNX_IF_ERR_EXIT(READ_EGQ_EGRESS_REPLICATION_GENERAL_CONFIGr_REG32(unit, REG_PORT_ANY, &data->nof_vlan_egress_mc_groups)); 
          data->nof_vlan_egress_mc_groups = soc_reg_field_get(unit, EGQ_EGRESS_REPLICATION_GENERAL_CONFIGr, data->nof_vlan_egress_mc_groups, EGRESS_REP_BITMAP_GROUP_VALUE_TOPf);
          TEST_ASSERT(data->nof_vlan_egress_mc_groups <= MAX_VLANS);
          ++data->nof_vlan_egress_mc_groups;
        }
    }

    BCMDNX_ALLOC(data->vlan_bitmaps, sizeof(uint32) * UINT32SPER_VLAN * data->nof_vlan_egress_mc_groups * SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores, "multicast_vlan_bitmaps");
    if (!data->vlan_bitmaps) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BCM_MSG("Failed allocating test memory for vlan bitmaps")));
    }
    for (i = data->nof_vlan_egress_mc_groups * UINT32SPER_VLAN * SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores; i ;) {
        data->vlan_bitmaps[--i] = 0; /* mark as no ports */
    }

    data->additional_members_left_in_last_alloc = 0;
    data->additional_members_in_free_list = 0;
    data->free_list_first = 0;
    data->free_list_last = 0;

    success = 1;
exit:
    if (!success) {
        if (data) {
            BCM_FREE(data->vlan_bitmaps);
            BCM_FREE(data->ingress_mc_groups);
            BCM_FREE(data->egress_mc_groups);
        }
        BCM_FREE(*datap);
    }
    BCMDNX_FUNC_RETURN;
}


/*
 * Un-Initialize Arad MC testing infrastructure, return 0 on success.
 */
int multicast_arad_test_destroy(int unit)
{
    int i;
    mc_test_data_t *data = test_data[unit];
    BCMDNX_INIT_FUNC_DEFS;

    TEST_ASSERT(unit >= 0 && unit < SOC_MAX_NUM_DEVICES && data && data->vlan_bitmaps); 

    BCM_FREE(data->vlan_bitmaps);
    {
        TEST_ASSERT(data->ingress_mc_groups);
    }
    TEST_ASSERT(data->egress_mc_groups);
    BCM_FREE(data->ingress_mc_groups);
    BCM_FREE(data->egress_mc_groups);

    TEST_ASSERT(data->additional_members_last_alloc < ADDITIONAL_MEMBERS_MAX_ALLOCS);
    for (i = 0; i <= data->additional_members_last_alloc; ++i) {
        TEST_ASSERT(data->additional_members[i]);
        BCM_FREE(data->additional_members[i]);
    }
    data->additional_members_last_alloc = -1;
    BCM_FREE(test_data[unit]);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* get the next MC group member pointed to by the given member */
int multicast_arad_test_get_next(int unit, const mc_member* inp_member, const mc_member** out_next_member)
{
    uint32 nextptr;
    mc_test_data_t *data = test_data[unit];
    BCMDNX_INIT_FUNC_DEFS;

    nextptr = mc_member_nextptr_get(inp_member);
    if (nextptr == MC_MEMBER_NEXT_MAX_VAL) {
        *out_next_member = 0;
    } else {
        int alloc_num = nextptr / ADDITIONAL_MEMBERS_PER_ALLOC;
        uint32 alloc_index = nextptr % ADDITIONAL_MEMBERS_PER_ALLOC;
        if (alloc_num < data->additional_members_last_alloc || (alloc_num == data->additional_members_last_alloc && alloc_index >= data->additional_members_left_in_last_alloc)) {
          *out_next_member = data->additional_members[alloc_num] + alloc_index;
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BCM_MSG("Encountered an illegal next pointer value 0x%lx"), (unsigned long)nextptr));
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* set the next MC group member pointed to by the given member */
int multicast_arad_test_set_next(int unit, mc_member* member_to_update, const mc_member* member_to_point_to)
{
    uint32 nextptr;
    uint32 alloc_num;
    mc_test_data_t *data = test_data[unit];
    BCMDNX_INIT_FUNC_DEFS;

    if (member_to_point_to) {
        alloc_num = mc_member_alloc_get(member_to_point_to);
        TEST_ASSERT(alloc_num <= data->additional_members_last_alloc);
        nextptr = member_to_point_to - data->additional_members[alloc_num];
        TEST_ASSERT(nextptr < ADDITIONAL_MEMBERS_PER_ALLOC);
        nextptr += alloc_num * ADDITIONAL_MEMBERS_PER_ALLOC;
    } else {
        nextptr = MC_MEMBER_NEXT_MAX_VAL;
    }
    mc_member_nextptr_set(unit, member_to_update, nextptr);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Given a group (first member of a group), check if it is open
 * Returns non zero if open.
 */
int multicast_arad_test_is_group_open(int unit, const mc_member* group, int* out_is_open)
{
    size_t diff;
    int not_found = 1;
    mc_test_data_t *data = test_data[unit];
    BCMDNX_INIT_FUNC_DEFS;
    if (data->egress_mc_groups) {
        diff = group - data->egress_mc_groups;
        if (diff < SOC_DPP_CONFIG(unit)->tm.nof_mc_ids * SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
            not_found = 0;
            *out_is_open = mc_member_is_ingress_get(group);
        }
    }
    if (data->ingress_mc_groups && not_found) {
        diff = group - data->ingress_mc_groups;
        if (diff < SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids) { 
            not_found = 0;
            *out_is_open = mc_member_is_ingress_get(group);
        }
    }
    if (not_found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BCM_MSG("Multicast group provided is not found")));
    }
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Given a group (first member of a group), assert that it is open
 */
int multicast_arad_test_assert_group_open(int unit, const mc_member* group)
{
    int is_open = 1;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(multicast_arad_test_is_group_open(unit, group, &is_open));
    TEST_ASSERT(is_open);
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Get a free MC member object, perform allocation if needed, remove from free list if needed
 */
int multicast_arad_test_get_free_member(int unit, mc_member** out_member)
{
    mc_test_data_t *data = test_data[unit];
    BCMDNX_INIT_FUNC_DEFS;

    /* First get an allocated but never used before object if exists. */
    if (data->additional_members_left_in_last_alloc) {
        --data->additional_members_left_in_last_alloc; /* objects from an allocation are return from the last to the first */
        *out_member = data->additional_members[data->additional_members_last_alloc] + data->additional_members_left_in_last_alloc;
        mc_member_alloc_set(unit, *out_member, data->additional_members_last_alloc); /* set the allocation number in the object */
        mc_member_is_used_set(unit, *out_member, 0); /* mark the member as unused */

    /* If not, and the free list is not empty, get the first one in the free list */
    } else if (data->free_list_first) {
        *out_member = data->free_list_first;
        TEST_ASSERT(data->additional_members_in_free_list);
        --data->additional_members_in_free_list;
        BCMDNX_IF_ERR_EXIT(multicast_arad_test_get_next(unit, data->free_list_first, (const mc_member**)&data->free_list_first));
        if (!data->free_list_first) {
            TEST_ASSERT(data->free_list_last == *out_member && !data->additional_members_in_free_list);
            data->free_list_last = 0;
        }

    /* If not, allocate more objects */
    } else if (data->additional_members_last_alloc < ADDITIONAL_MEMBERS_MAX_ALLOCS - 1) {

        ++data->additional_members_last_alloc;
        TEST_ASSERT(!data->additional_members[data->additional_members_last_alloc]);
        BCMDNX_ALLOC(data->additional_members[data->additional_members_last_alloc], sizeof(mc_member) * ADDITIONAL_MEMBERS_PER_ALLOC, "multicast_group_members");
        if (!data->additional_members[data->additional_members_last_alloc]) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BCM_MSG("Failed allocating test memory for multicast group members")));
        }
        data->additional_members_left_in_last_alloc = ADDITIONAL_MEMBERS_PER_ALLOC - 1;
        *out_member = data->additional_members[data->additional_members_last_alloc] + data->additional_members_left_in_last_alloc;
        mc_member_alloc_set(unit, *out_member, data->additional_members_last_alloc); /* set the allocation number in the object */
        mc_member_is_used_set(unit, *out_member, 0); /* mark the member as unused */
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_FAIL, (_BCM_MSG("Multicast will not allocate more members memory, already using the max numbner: %u"), MAX_ADDITIONAL_MEMBERS));
    }
            
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Release an MC member object, remove it from its group's linked list and add it to the free list
 * Null prev_member means that the removed member is the first in the group.
 */
int multicast_arad_test_free_member(int unit, mc_member* member_to_remove, mc_member* prev_member)
{
    const mc_member* temp;
    mc_test_data_t *data = test_data[unit];
    BCMDNX_INIT_FUNC_DEFS;

    TEST_ASSERT(member_to_remove && mc_member_is_used_get(member_to_remove));
    if (prev_member) { /* removing an additional (non first) member of the group */
        BCMDNX_IF_ERR_EXIT(multicast_arad_test_get_next(unit, prev_member, &temp));
        TEST_ASSERT(temp == member_to_remove); /* verify that the previous group member is really so */
        BCMDNX_IF_ERR_EXIT(multicast_arad_test_get_next(unit, member_to_remove, &temp)); /* get next member in group */
        BCMDNX_IF_ERR_EXIT(multicast_arad_test_set_next(unit, prev_member, temp)); /* remove member from group linked list */
        /* add the member to the free linked list */
        if (data->free_list_last) {
            TEST_ASSERT(data->free_list_first && data->additional_members_in_free_list);
            BCMDNX_IF_ERR_EXIT(multicast_arad_test_set_next(unit, data->free_list_last, member_to_remove));
        } else {
            TEST_ASSERT(!data->free_list_first && !data->additional_members_in_free_list);
            data->free_list_first = member_to_remove ;
        }
        BCMDNX_IF_ERR_EXIT(multicast_arad_test_set_next(unit, member_to_remove, 0));
        data->free_list_last = member_to_remove;
        ++data->additional_members_in_free_list;
        mc_member_is_used_set(unit, member_to_remove, 0);

    } else { /* removing the first member of the group */

        BCMDNX_IF_ERR_EXIT(multicast_arad_test_assert_group_open(unit, member_to_remove));
        BCMDNX_IF_ERR_EXIT(multicast_arad_test_get_next(unit, member_to_remove, &temp));
        if (temp) { /* The group has more members , copy the second member onto the first and remove the second one */
            mc_member_copy_dest_cud(member_to_remove, temp);
            BCMDNX_IF_ERR_EXIT(multicast_arad_test_free_member(unit, (mc_member*)temp, member_to_remove));
        } else { /* The only member of the group is being removed */
            mc_member_is_used_set(unit, member_to_remove, 0);
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Given a group (first member of a group), get its group ID, and if it is in/egress
 */
int multicast_arad_test_get_group_id(int unit, const mc_member* group, SOC_TMC_MULT_ID* out_id, int *out_is_ingress)
{
    size_t diff = 0;
    int not_found = 1;
    mc_test_data_t *data = test_data[unit];
    BCMDNX_INIT_FUNC_DEFS;
    if (data->egress_mc_groups) {
        diff = group - data->egress_mc_groups;
        if (diff < SOC_DPP_CONFIG(unit)->tm.nof_mc_ids * SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
            not_found = 0;
            *out_is_ingress = 0;
        }
    }
    if (data->ingress_mc_groups && not_found) {
        diff = group - data->ingress_mc_groups;
        if (diff < SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids) {
            not_found = 0;
            *out_is_ingress = 1;
        }
    }
    if (not_found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BCM_MSG("Multicast group not found")));
    }
    *out_id = diff;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Find a member in the given MC group.
 * Returns in result the matching group member if found or the last member in the group.
 * If the group is empty, group_to_search is returned.
 * If found, found will be set to 1, otherwise 0.
 * Group must be open.
 */
int multicast_arad_test_find_member(int unit, const mc_member* group_to_search, const mc_member* member_to_find, const mc_member** result, int* found)
{
    BCMDNX_INIT_FUNC_DEFS;
    TEST_ASSERT(group_to_search && member_to_find && result && found);
    BCMDNX_IF_ERR_EXIT(multicast_arad_test_assert_group_open(unit, group_to_search));
    *found = 0;
    if (mc_member_is_used_get(group_to_search)) {
        const mc_member* i = group_to_search;
        for (;;) { /* loop over all members */
            *result = i;
            if (mc_member_same(member_to_find, i)) {
                *found = 1;
                break;
            }
            BCMDNX_IF_ERR_EXIT(multicast_arad_test_get_next(unit, i, &i));
            if (!i) {
                break;
            }
        }
    } else {
        *result = group_to_search;
    }
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * remove a member in the given MC group.
 * if the member was out_found and removed, found will be non zero.
 * If the group is empty, group_to_search is returned.
 * If found, found will be set to 1, otherwise 0.
 * Group must be open.
 */
int multicast_arad_test_remove_member(int unit, mc_member* group_to_search, int is_ingress, const mc_member* member_to_find, int* out_found)
{
    BCMDNX_INIT_FUNC_DEFS;
    *out_found = 0;
    TEST_ASSERT(group_to_search && member_to_find && out_found);
    BCMDNX_IF_ERR_EXIT(multicast_arad_test_assert_group_open(unit, group_to_search));
    if (mc_member_is_used_get(group_to_search)) {
        mc_member* i;
        mc_member* prev = 0;
        for (i = group_to_search; i;) { /* loop over all members */
            if (mc_member_same(member_to_find, i)) {
                *out_found = 1;
                BCMDNX_IF_ERR_EXIT(multicast_arad_test_free_member(unit, i, prev));
                break;
            }
            prev = i;
            BCMDNX_IF_ERR_EXIT(multicast_arad_test_get_next(unit, i, (const mc_member**)&i));
        }
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/* return non zero if the given port is a member of the given VLAN bitmap MC group */
/* does not check input validaty: (mc_id < data->nof_vlan_egress_mc_groups)  && (port < PORTS_PER_VLAN) */
/*inline*/ int is_port_member_of_vlan(int unit, const SOC_TMC_MULT_ID mc_id, const uint32 port)
{
    return (test_data[unit]->vlan_bitmaps[mc_id * UINT32SPER_VLAN + (mc_id / 32)] >> (mc_id % 32)) & 1;
}

/*
 * Add a member to the given MC group if it is not already a member.
 * The group must already be open.
 */
int multicast_arad_test_add_member(int unit, mc_member* group_to_add_to, uint32 cud, uint32 dest)
{
    SOC_TMC_MULT_ID mc_id;
    int is_ingress;
    uint32 tm_port;
    int core;
    uint32 flags = 0;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    mc_test_data_t *data = test_data[unit];
    BCMDNX_INIT_FUNC_DEFS;
    TEST_ASSERT(group_to_add_to);

    BCMDNX_IF_ERR_EXIT(multicast_arad_test_get_group_id(unit, group_to_add_to, &mc_id, &is_ingress));
    TEST_ASSERT(cud <= (is_ingress ? SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_cud :
      SOC_DPP_CONFIG(unit)->tm.egress_mc_max_cud) && dest < MC_MEMBER_DEST_VAL_OVERFLOW);
    if (!is_ingress && mc_id < data->nof_vlan_egress_mc_groups) { /* This is a VLAN bitmap MC group */
        TEST_ASSERT(dest < PORTS_PER_VLAN);
        BCMDNX_IF_ERR_EXIT(bcm_petra_port_get(unit, dest, &flags, &interface_info, &mapping_info));
        tm_port = mapping_info.tm_port;
        core = mapping_info.core;
        data->vlan_bitmaps[(core * data->nof_vlan_egress_mc_groups + mc_id) * UINT32SPER_VLAN + (tm_port / 32)] |= ((uint32)1) << (tm_port % 32); 

    } else { /* regular MC group */
        int found;
        mc_member member_value = {0, 0};
        mc_member* found_member;
        mc_member_cud_set(unit, &member_value, cud);
        mc_member_dest_set(unit, &member_value, dest);

        BCMDNX_IF_ERR_EXIT(multicast_arad_test_find_member(unit, group_to_add_to, &member_value, (const mc_member**)&found_member, &found));
        if (!found) { /* if member not found, add it */
            if (!mc_member_is_used_get(found_member)) { /* the group is empty, use its first (non additional) member */
                TEST_ASSERT(found_member == group_to_add_to);
                mc_member_cud_set(unit, group_to_add_to, cud);
                mc_member_dest_set(unit, group_to_add_to, dest);
                mc_member_nextptr_set(unit, group_to_add_to, MC_MEMBER_NEXT_MAX_VAL);
                mc_member_is_used_set(unit, group_to_add_to, 1);
            } else { /* add an additional member to the end of the group */
                mc_member* new_member;
                TEST_ASSERT(mc_member_nextptr_get(found_member) == MC_MEMBER_NEXT_MAX_VAL);
                BCMDNX_IF_ERR_EXIT(multicast_arad_test_get_free_member(unit, &new_member)); /* get a new member to add */
                /* fill the fields of the new member */
                mc_member_cud_set(unit, new_member, cud);
                mc_member_dest_set(unit, new_member, dest);
                mc_member_nextptr_set(unit, new_member, MC_MEMBER_NEXT_MAX_VAL);
                mc_member_is_used_set(unit, new_member, 1);
                mc_member_is_ingress_set(unit, new_member, mc_member_is_ingress_get(group_to_add_to));
                BCMDNX_IF_ERR_EXIT(multicast_arad_test_set_next(unit, found_member, new_member)); /* add new member to the end of the group */
            }
        }
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/* is multicast testing initialized for the given unit, return non zero if so */
int multicast_is_initialized(int unit)
{
    return unit >= 0 && unit < SOC_MAX_NUM_DEVICES && test_data[unit] && test_data[unit]->egress_mc_groups;
}


/*
 * This section contains wrappers for BCM multicast APIs.
 * The wrappers both calls the BCM API and updates the test data structures.
 */


/* Initialize device tables and driver structures for multicast management functions. */
int test_bcm_multicast_init(int unit)
{
    bcm_multicast_t multicast_id;
    mc_test_data_t *data = test_data[unit];
    
    BCMDNX_INIT_FUNC_DEFS;
    if (data && (data->egress_mc_groups || data->ingress_mc_groups)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BCM_MSG("Multicast test already initialized")));
    }
    bcm_multicast_init(unit); /* ignore errors */
    if (SOC_DPP_PP_ENABLE(unit)) {
        int range_high, range_low;
        /* Multicast egress groups 0-4K bitmap types are opened by default since they are reserved for vlan(VSI) 0-4K */
        /* They are removed to match the test data structures */

        range_low = SOC_DPP_CONFIG(unit)->tm.multicast_egress_bitmap_group_range.mc_id_low; 
        range_high = SOC_DPP_CONFIG(unit)->tm.multicast_egress_bitmap_group_range.mc_id_high;

        for (multicast_id = 0; multicast_id < BCM_PETRA_MULTICAST_NOF_BRIDGE_ROUTE; ++multicast_id) {     
            if (!SAL_BOOT_QUICKTURN || (multicast_id < range_low || multicast_id > range_high)) {
                /* In case of link list needs to call to regular open sequence */
                bcm_petra_multicast_destroy(unit, multicast_id); /* ignore errors */
            } else {
                /* In case of vlan membership, only needs to call to allocation management */
                bcm_dpp_am_mc_dealloc(unit, multicast_id, TRUE); /* ignore errors */
            }      
        }              
        /* remove group crated for OLP */
        bcm_petra_multicast_destroy(unit, SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids - 1); /* ignore errors */
    }

    BCMDNX_IF_ERR_EXIT(multicast_arad_test_init(unit));

exit:
    BCMDNX_FUNC_RETURN;
}

/* Detach the BCM Multicast subsystem. Allocated resources will be released and internal driver state for the multicast module will be lost. */
int test_bcm_multicast_detach(int unit)
{
    BCMDNX_INIT_FUNC_DEFS;
    if (!multicast_is_initialized(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BCM_MSG("Multicast test not initialized to this unit")));
    }
    BCMDNX_IF_ERR_EXIT(multicast_arad_test_destroy(unit));
    /* BCMDNX_IF_ERR_EXIT(bcm_multicast_detach(unit)); not yet implemented */
    BCMDNX_IF_ERR_EXIT(bcm_multicast_init(unit));
exit:
    BCMDNX_FUNC_RETURN;
}


/* Detach the BCM Multicast subsystem when a test stopped in the middle. Destroy crated multicast groups.
   Allocated resources will be released and internal driver state for the multicast module will be lost. */
int test_bcm_multicast_detach_dirty(int unit)
  /* res = arad_mult_rplct_tbl_entry_unoccupied_set_all(unit); */
{
    SOC_TMC_MULT_ID i;
    mc_test_data_t *data = test_data[unit];

    BCMDNX_INIT_FUNC_DEFS;
    if (!multicast_is_initialized(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BCM_MSG("Multicast test not initialized to this unit, can not cleanup after the test")));
    }

    /* verify egress VLAN bitmap MC groups */
#ifdef _ARAD_MC_BETTER_TEST_VLANS_0
    for (i = 0; i < data->nof_vlan_egress_mc_groups; ++i) { /* first test VLAN bitmap egress Mc groups */
        uint32* entry = vlan_table + (i * ARAD_MC_VLAN_BITMAP_ENTRY_SIZE);
        uint32* vlan_bits = data->vlan_bitmaps + (i * UINT32SPER_VLAN);
        group_ptr = data->egress_mc_groups + i;
        is_open = mc_member_is_ingress_get(group_ptr); /* group marked open in data structure */
    }
#endif

    /* verify egress regular MC groups */
    for (i = 0; i < SOC_DPP_CONFIG(unit)->tm.nof_mc_ids; ++i) {
        /* check if the ingress/egress group is open in the test data structures */
        if ((data->ingress_mc_groups && i < ARAD_MULT_NOF_MULTICAST_GROUPS && mc_member_is_ingress_get(data->ingress_mc_groups + i)) || 
            (data->egress_mc_groups && i >= data->nof_vlan_egress_mc_groups && (mc_member_is_ingress_get(data->egress_mc_groups + i)))) {
                bcm_multicast_destroy(unit, i); /* try to close it, ignoring errors */
        }
    }

    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_detach(unit));
exit:
    BCMDNX_FUNC_RETURN;
}

#define TEST_FULL_BCM_IF_ERR_EXIT(_rc) \
      do { \
        int __err__rc = _rc; \
        if (__err__rc != BCM_E_NONE) { \
            _rv = __err__rc; \
            if (may_run_out_of_space && __err__rc == BCM_E_FULL) { \
                goto exit; \
            } else { \
                LOG_ERROR(BSL_LS_APPL_TESTS, \
                          (BSL_META_U(unit, \
                                      "%s\n"), \
                           bcm_errmsg(__err__rc))); \
                BCM_EXIT; \
            } \
        } \
      } while(0)

/* Create a multicast group for packet replication. */
int test_bcm_multicast_create(int unit, uint32 flags, bcm_multicast_t *group, int may_run_out_of_space)
{
    SOC_TMC_MULT_ID multicast_id;
    mc_test_data_t *data = test_data[unit];
    int32 core;
    BCMDNX_INIT_FUNC_DEFS;
    TEST_ASSERT(multicast_is_initialized(unit) && flags & BCM_MULTICAST_WITH_ID);
    TEST_FULL_BCM_IF_ERR_EXIT(bcm_multicast_create(unit, flags, group));
    BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_group_to_id(*group, &multicast_id));

    if (!(flags & BCM_MULTICAST_TYPE_TRILL) && ((flags & BCM_MULTICAST_INGRESS_GROUP) || (~flags & BCM_MULTICAST_EGRESS_GROUP))) {
        /* open an ingress MC group */
        TEST_ASSERT(!mc_member_is_ingress_get(data->ingress_mc_groups + multicast_id)); /* should not already be open */
        mc_member_is_ingress_set(unit, data->ingress_mc_groups + multicast_id, 1); /* mark group as open */
        mc_member_is_used_set(unit, data->ingress_mc_groups + multicast_id, 0); /* mark group as empty */
    }
    if ((flags & (BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_TYPE_TRILL)) || (~flags & BCM_MULTICAST_INGRESS_GROUP)) {
        /* open an egress MC group */
        SOC_DPP_CORES_ITER(SOC_DPP_CORE_ALL(unit), core){ /*for each of the cores*/
            TEST_ASSERT(!mc_member_is_ingress_get(data->egress_mc_groups + (core * SOC_DPP_CONFIG(unit)->tm.nof_mc_ids) + multicast_id)); /* should not already be open */
            mc_member_is_ingress_set(unit, data->egress_mc_groups + (core * SOC_DPP_CONFIG(unit)->tm.nof_mc_ids) + multicast_id, 1); /* mark group as open */
            mc_member_is_used_set(unit, data->egress_mc_groups + (core * SOC_DPP_CONFIG(unit)->tm.nof_mc_ids) + multicast_id, 0); /* mark group as empty */
        }
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* remove all members from the given ingress multicast group */
int static test_bcm_multicast_remove_ingress_group_members(int unit, SOC_TMC_MULT_ID multicast_id, int close_group)
{
    mc_member* group = test_data[unit]->ingress_mc_groups + multicast_id;
    mc_member* next_member;
    BCMDNX_INIT_FUNC_DEFS;

    TEST_ASSERT(mc_member_is_ingress_get(group)); /* assert that ingress group is open */
    if (close_group) {
        mc_member_is_ingress_set(unit, group, 0); /* mark group as closed */
    }
    if (mc_member_is_used_get(group)) {
        for (;;) { /* remove all members */
            BCMDNX_IF_ERR_EXIT(multicast_arad_test_get_next(unit, group, (const mc_member**)&next_member));
            if (!next_member) {
                break;
            }
            BCMDNX_IF_ERR_EXIT(multicast_arad_test_free_member(unit, next_member, group));
        }
        mc_member_is_used_set(unit, group, 0); /* mark group as empty */
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/* remove all members from the given egress multicast group */
int static test_bcm_multicast_remove_egress_group_members(int unit, SOC_TMC_MULT_ID multicast_id, int close_group)
{
    int core;
    mc_test_data_t *data = test_data[unit];
    mc_member* group;
    mc_member* next_member;
    BCMDNX_INIT_FUNC_DEFS;
    SOC_DPP_CORES_ITER(SOC_DPP_CORE_ALL(unit), core){
        group = data->egress_mc_groups + (core * SOC_DPP_CONFIG(unit)->tm.nof_mc_ids) + multicast_id;
        TEST_ASSERT(mc_member_is_ingress_get(group)); /* assert that egress group is open */
        if (close_group) {
            mc_member_is_ingress_set(unit, group, 0); /* mark group as closed */
        }
        if (multicast_id < data->nof_vlan_egress_mc_groups) { /* This is a VLAN bitmap MC group */
            int i;
            uint32* vlan_bits = data->vlan_bitmaps + (UINT32SPER_VLAN * (multicast_id + (core * data->nof_vlan_egress_mc_groups)));
            for (i = UINT32SPER_VLAN;  i; --i) { /* clear bitmap */
                *(vlan_bits++) = 0;
            }
        } else {
            if (mc_member_is_used_get(group)) {
                for (;;) { /* remove all members */
                    BCMDNX_IF_ERR_EXIT(multicast_arad_test_get_next(unit, group, (const mc_member**)&next_member));
                    if (!next_member) {
                        break;
                    }
                    BCMDNX_IF_ERR_EXIT(multicast_arad_test_free_member(unit, next_member, group));
                }
            }
        }
        mc_member_is_used_set(unit, group, 0); /* mark group as empty */
    }
    

exit:
    BCMDNX_FUNC_RETURN;
}

/* Destroy a multicast group. */
int test_bcm_multicast_destroy(int unit, bcm_multicast_t group)
{
    SOC_TMC_MULT_ID multicast_id;
    int is_ingress_open = 0, is_egress_open = 0;
    mc_test_data_t *data = test_data[unit];
    BCMDNX_INIT_FUNC_DEFS;
    TEST_ASSERT(multicast_is_initialized(unit));
    BCMDNX_IF_ERR_EXIT(bcm_multicast_destroy(unit, group));
    BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_group_to_id(group, &multicast_id));

    if (multicast_id < SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids) {
        is_ingress_open = mc_member_is_ingress_get(data->ingress_mc_groups + multicast_id); /* is ingress group open? */
    }
    if (multicast_id < SOC_DPP_CONFIG(unit)->tm.nof_mc_ids) {
        is_egress_open = mc_member_is_ingress_get(data->egress_mc_groups + multicast_id); /* is egress group open? */
    }
    TEST_ASSERT(is_ingress_open || is_egress_open);

    if (is_ingress_open) {
        BCMDNX_IF_ERR_EXIT(test_bcm_multicast_remove_ingress_group_members(unit, multicast_id, 1));
    }
    if (is_egress_open) {
        BCMDNX_IF_ERR_EXIT(test_bcm_multicast_remove_egress_group_members(unit, multicast_id, 1));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* the encapsulation commands are not yet supported for Arad */

/* Get the multicast encapsulation ID (CUD) for L3 replication. The encapsulation ID is an opaque object passed in to bcm_multicast_egress_* APIs to when adding/removing ports to a multicast group.*/
int test_bcm_multicast_l3_encap_get(int unit, bcm_multicast_t group,
                                    bcm_gport_t port, 
                                    bcm_if_t intf, bcm_if_t *encap_id)
{
    return bcm_multicast_l3_encap_get(unit, group, port, intf, encap_id);
}

/* Get the multicast encapsulation ID (CUD) for L2 replication. The encapsulation ID is an opaque object passed in to bcm_multicast_egress_* APIs when adding/removing ports to a multicast group. */
int test_bcm_multicast_l2_encap_get(int unit, bcm_multicast_t group,
                                   bcm_gport_t port, 
                                   bcm_vlan_t vlan, bcm_if_t *encap_id)
{
    return bcm_multicast_l2_encap_get(unit, group, port, vlan, encap_id);
}

/* Get the multicast encapsulation ID (CUD) for VPLS replication. The encapsulation ID is an opaque object passed in to bcm_multicast_egress_* APIs when adding/removing ports to a multicast group. */
int test_bcm_multicast_vpls_encap_get(int unit, bcm_multicast_t group,
                                     bcm_gport_t port, 
                                     bcm_gport_t mpls_port_id,
                                     bcm_if_t *encap_id)
{
    return bcm_multicast_vpls_encap_get(unit, group, port, mpls_port_id, encap_id);
}

/* Get the multicast encapsulation ID (CUD) for SUBPORT replication. The encapsulation ID is an opaque object passed in to bcm_multicast_egress_* APIs when adding/removing ports to a multicast group. */
int test_bcm_multicast_subport_encap_get(int unit, bcm_multicast_t group,
                                         bcm_gport_t port, bcm_gport_t subport,
                                         bcm_if_t *encap_id)
{
    return bcm_multicast_subport_encap_get(unit, group, port, subport, encap_id);
}

/* Get the multicast encapsulation ID (CUD) for MAC-in-MAC replication. The encapsulation ID is an opaque object passed in to bcm_multicast_egress_* APIs when adding/removing ports to a multicast group. */
int test_bcm_multicast_mim_encap_get(int unit, bcm_multicast_t group,
                                    bcm_gport_t port, 
                                    bcm_gport_t mim_port_id,
                                    bcm_if_t *encap_id)
{
    return bcm_multicast_mim_encap_get(unit, group, port, mim_port_id, encap_id);
}

/* Get the multicast encapsulation ID (CUD) for WLAN replication. The encapsulation ID is an opaque object passed in to bcm_multicast_egress_* APIs when adding/removing ports to a multicast group. */
int test_bcm_multicast_wlan_encap_get(int unit, bcm_multicast_t group,
                                     bcm_gport_t port, 
                                     bcm_gport_t wlan_port_id,
                                     bcm_if_t *encap_id)
{
    return bcm_multicast_wlan_encap_get(unit, group, port, wlan_port_id, encap_id);
}

/* Get the multicast encapsulation ID (CUD) for layer 2 logical port replication. The encapsulation ID is an opaque object passed in to bcm_multicast_egress_* APIs when adding/removing ports to a multicast group. */

int test_bcm_multicast_vlan_encap_get(int unit, bcm_multicast_t group,
                                     bcm_gport_t port, 
                                     bcm_gport_t vlan_port_id,
                                     bcm_if_t *encap_id)
{
    return bcm_multicast_vlan_encap_get(unit, group, port, vlan_port_id, encap_id);
}

/* Get the multicast encapsulation ID (CUD) for NIV replication. The encapsulation ID is an opaque object passed in to bcm_multicast_egress_* APIs when adding/removing ports to a multicast group. */
int test_bcm_multicast_niv_encap_get(int unit, bcm_multicast_t group,
                                     bcm_gport_t port, 
                                     bcm_gport_t niv_port_id,
                                     bcm_if_t *encap_id)
{
    return bcm_multicast_niv_encap_get(unit, group, port, niv_port_id, encap_id);
}

/* Get the multicast encapsulation ID (CUD) for Egress Object type replication. The encapsulation ID is an opaque object passed in to bcm_multicast_egress_* APIs when adding/removing ports to a multicast group. */
int test_bcm_multicast_egress_object_encap_get(int unit, bcm_multicast_t group,
                                     bcm_if_t intf,
                                     bcm_if_t *encap_id)
{
    return bcm_multicast_egress_object_encap_get(unit, group, intf, encap_id);
}

/* Add a port to a multicast group's replication list. The port is the physical port where replicated packets will be sent. The encap_id is an opaque encapsulation ID returned from the bcm_multicast_*_encap_get APIs. The subscriber_queue is an opaque subscriber queue ID. The subscriber form is only available on certain devices in specific modes; if the non-subscriber form is used in these modes, it will assume zero as queue group. */
int test_bcm_multicast_egress_add(int unit, bcm_multicast_t group, 
                                 bcm_gport_t port, bcm_if_t encap_id, int may_run_out_of_space)
{
    SOC_TMC_MULT_ID multicast_id;
    int is_bitmap_group;
    mc_member* testgroup;
    mc_test_data_t *data = test_data[unit];
    int32 core;
    uint32 flags = 0;
    uint8 cud_core;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    BCMDNX_INIT_FUNC_DEFS;
    TEST_ASSERT(multicast_is_initialized(unit));
    TEST_FULL_BCM_IF_ERR_EXIT(bcm_multicast_egress_add(unit, group, port, encap_id));

    BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_group_to_id(group, &multicast_id));
    SOC_DPP_CORES_ITER(SOC_DPP_CORE_ALL(unit), core){
        testgroup = data->egress_mc_groups + core * SOC_DPP_CONFIG(unit)->tm.nof_mc_ids + multicast_id;
        is_bitmap_group =
          (_BCM_MULTICAST_TYPE_GET(group) == _BCM_MULTICAST_TYPE_PORTS_GROUP ||
          multicast_id <= SOC_DPP_CONFIG(unit)->tm.multicast_egress_bitmap_group_range.mc_id_high);

        if (!is_bitmap_group || multicast_id <= SOC_DPP_CONFIG(unit)->tm.multicast_egress_bitmap_group_range.mc_id_high) {
            BCMDNX_IF_ERR_EXIT(multicast_arad_test_assert_group_open(unit, testgroup));
        }

        if (!is_bitmap_group && port == BCM_GPORT_INVALID ) { /* outlif only replication (port mapped from outlif) */
            if (SOC_IS_JERICHO(unit)) {
                DPP_CUD2CORE_GET_CORE(unit, encap_id, cud_core);
                cud_core = (cud_core == core) ? TRUE : FALSE; /*check if this is the correct core*/
            } else { 
                cud_core = TRUE; /*ARAD has only one core*/
            }
            if (cud_core) {
                BCMDNX_IF_ERR_EXIT(multicast_arad_test_add_member(unit, testgroup, encap_id, MC_MEMBER_DEST_OUTLIF_ONLY));
            }
        } else if (!is_bitmap_group && BCM_GPORT_IS_MCAST(port)) { /* bitmap replication */
            BCMDNX_IF_ERR_EXIT(multicast_arad_test_add_member(unit, testgroup, encap_id, BCM_GPORT_MCAST_GET(port) + MC_MEMBER_DEST_BITMAP_OFFSET));
        } else {
            uint32 local_port;
            bcm_pbmp_t pbmp_local_ports;
            bcm_if_t port_encap = encap_id; 
            /* Derive local ports */
            if (BCM_GPORT_IS_TRILL_PORT(port)) {
                /*extract port and real encap id from trill encap*/
                port_encap &= _BCM_TRILL_ENCAP_MASK;
                BCM_PBMP_CLEAR(pbmp_local_ports);
                BCM_PBMP_PORT_ADD(pbmp_local_ports,(encap_id >> _BCM_TRILL_ENCAP_PORT_SHIFT ));
            } else { /* not a trill port */
                _bcm_dpp_gport_info_t gport_info;
                BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info)); 
                BCM_PBMP_ASSIGN(pbmp_local_ports, gport_info.pbmp_local_ports);
            }
            BCM_PBMP_ITER(pbmp_local_ports,local_port) {
                BCMDNX_IF_ERR_EXIT(bcm_petra_port_get(unit, local_port, &flags, &interface_info, &mapping_info));
                if (core == mapping_info.core) {
                    BCMDNX_IF_ERR_EXIT(multicast_arad_test_add_member(unit, testgroup, port_encap, local_port)); 
                }
            }
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* not supported */
int test_bcm_multicast_egress_subscriber_add(int unit, bcm_multicast_t group,
                                            bcm_gport_t port,
                                            bcm_if_t encap_id,
                                            bcm_gport_t subscriber_queue)

{
    return bcm_multicast_egress_subscriber_add(unit, group, port, encap_id, subscriber_queue);
}

int _multicast_arad_test_remove_eg_member(int unit, mc_member* group, mc_member* member_to_find, bcm_if_t port_encap, uint32 local_port, int* out_found)
{
    mc_test_data_t *data = test_data[unit];
    BCMDNX_INIT_FUNC_DEFS;
    /* Find the member and remove it */
    mc_member_cud_set(unit, member_to_find, port_encap);
    mc_member_dest_set(unit, member_to_find, local_port);
    BCMDNX_IF_ERR_EXIT(multicast_arad_test_remove_member(unit, group, 0, member_to_find, out_found));
    if (!*out_found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BCM_MSG("could not find member dest=0x%lx cud=0x%lx to remove in group 0x%lx"), (unsigned long)local_port, (unsigned long)port_encap, (unsigned long)(group - data->egress_mc_groups)));
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/* Delete a port from a multicast group's replication list. The port is the physical port where replicated packets will be sent. The encap_id is an opaque encapsulation ID returned from the bcm_multicast_*_encap_get APIs. The subscriber_queue is an opaque subscriber queue ID. The subscriber form is only available on certain devices in specific modes; if the non-subscriber form is used in these modes, it will assume zero as queue group. */
int test_bcm_multicast_egress_delete(int unit, bcm_multicast_t group, 
                                    bcm_gport_t port, bcm_if_t encap_id, int may_run_out_of_space)
{
    SOC_TMC_MULT_ID multicast_id;
    mc_member member_to_find;
    qax_mcds_t *mcds = dpp_get_mcds(unit);
    mc_member* testgroup;
    int found, is_bitmap_group;
    int32 core;
    int32 mc_id;
    uint8 cud_core;
    uint32 flags = 0;
    bcm_gport_t bm_port ;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    BCMDNX_INIT_FUNC_DEFS;
    TEST_ASSERT(multicast_is_initialized(unit));
    TEST_FULL_BCM_IF_ERR_EXIT(bcm_multicast_egress_delete(unit, group, port, encap_id));

    BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_group_to_id(group, &multicast_id));
    mc_id = (int32)multicast_id;
    is_bitmap_group = _BCM_MULTICAST_TYPE_GET(group) == _BCM_MULTICAST_TYPE_PORTS_GROUP ||
      mc_id <= SOC_DPP_CONFIG(unit)->tm.multicast_egress_bitmap_group_range.mc_id_high;

    SOC_DPP_CORES_ITER(SOC_DPP_CORE_ALL(unit), core){
        testgroup = test_data[unit]->egress_mc_groups + core * SOC_DPP_CONFIG(unit)->tm.nof_mc_ids + multicast_id;
        if (!is_bitmap_group || mc_id <= SOC_DPP_CONFIG(unit)->tm.multicast_egress_bitmap_group_range.mc_id_high) {
            BCMDNX_IF_ERR_EXIT(multicast_arad_test_assert_group_open(unit, testgroup));
        }
        if (!is_bitmap_group && port == BCM_GPORT_INVALID ) { /* outlif only replication (port mapped from outlif) */
            if (SOC_IS_JERICHO(unit)) {
                    DPP_CUD2CORE_GET_CORE(unit, encap_id, cud_core);
                    cud_core = (cud_core == core) ? TRUE : FALSE; /*check if this is the correct core*/
                } else {
                    cud_core = TRUE; /*Arad has only one core*/
                }
                if (cud_core) {
                    BCMDNX_IF_ERR_EXIT(_multicast_arad_test_remove_eg_member(unit, testgroup, &member_to_find, encap_id, MC_MEMBER_DEST_OUTLIF_ONLY, &found));
                }
        } else if (!is_bitmap_group && BCM_GPORT_IS_MCAST(port)) { /* bitmap replication */
            bm_port = (!SOC_IS_QAX(unit)) ? BCM_GPORT_MCAST_GET(port) + MC_MEMBER_DEST_BITMAP_OFFSET 
                : mcds->egress_bitmap_start + BCM_GPORT_MCAST_GET(port)*SOC_DPP_CONFIG(unit)->qax->nof_egress_bitmap_bytes + MC_MEMBER_DEST_BITMAP_OFFSET;
            BCMDNX_IF_ERR_EXIT(_multicast_arad_test_remove_eg_member(unit, testgroup, &member_to_find, encap_id, bm_port, &found));
        } else {
            uint32 local_port;
            bcm_pbmp_t pbmp_local_ports;
            bcm_if_t port_encap = encap_id; 
            /* Derive local ports */
            if (BCM_GPORT_IS_TRILL_PORT(port)) {
                /*extract port and real encap id from trill encap*/
                port_encap &= _BCM_TRILL_ENCAP_MASK;
                BCM_PBMP_CLEAR(pbmp_local_ports);
                BCM_PBMP_PORT_ADD(pbmp_local_ports,(encap_id >> _BCM_TRILL_ENCAP_PORT_SHIFT ));
            } else { /* not a trill port */
                _bcm_dpp_gport_info_t gport_info;
                BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));
                BCM_PBMP_ASSIGN(pbmp_local_ports, gport_info.pbmp_local_ports);
            }
            /* loop over the pbmp_local_ports bitmap */
            BCM_PBMP_ITER(pbmp_local_ports,local_port) {
                BCMDNX_IF_ERR_EXIT(bcm_petra_port_get(unit, local_port, &flags, &interface_info, &mapping_info));
                if (core == mapping_info.core) {
                    BCMDNX_IF_ERR_EXIT(_multicast_arad_test_remove_eg_member(unit, testgroup, &member_to_find, port_encap, local_port, &found));
                }
            }
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* not supported */
int test_bcm_multicast_egress_subscriber_delete(int unit, bcm_multicast_t group,
                                               bcm_gport_t port,
                                               bcm_if_t encap_id,
                                               bcm_gport_t subscriber_queue)
{
    return bcm_multicast_egress_subscriber_delete(unit, group, port, encap_id, subscriber_queue);
}


/* Delete all replications for the specified multicast index. */
int test_bcm_multicast_egress_delete_all(int unit, bcm_multicast_t group)
{
    SOC_TMC_MULT_ID multicast_id;
    BCMDNX_INIT_FUNC_DEFS;

    TEST_ASSERT(multicast_is_initialized(unit));
    BCMDNX_IF_ERR_EXIT(bcm_multicast_egress_delete_all(unit, group));

    BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_group_to_id(group, &multicast_id));
    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_remove_egress_group_members(unit, multicast_id, 0));
exit:
    BCMDNX_FUNC_RETURN;
}


/* Assign a set of encapsulated interfaces on specific ports as a multicast group's replication list. The port_array contains physical ports where replicated packets will be sent. The encap_id_array contains the opaque encapsulation IDs returned from the bcm_multicast_*_encap_get APIs. The subscriber_queue is an array of opaque subscriber queue IDs. The subscriber form is only available on certain devices in specific modes; if the non-subscriber form is used in these modes, it will assume zeros as all queue groups. */
int test_bcm_multicast_egress_set(int unit, bcm_multicast_t group,
                                  int port_count, 
                                  bcm_gport_t *port_array,
                                  bcm_if_t *encap_id_array,
                                  int may_run_out_of_space)
{
    SOC_TMC_MULT_ID multicast_id;
    qax_mcds_t *mcds = dpp_get_mcds(unit);
    int i, is_bitmap_group, dest;
    mc_member* testgroup;
    mc_test_data_t *data = test_data[unit];
    int32 core;
    uint8 cud_core;
    uint32 flags = 0;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    int32 mc_id ;
    BCMDNX_INIT_FUNC_DEFS;  
    TEST_ASSERT(multicast_is_initialized(unit) && port_count>= 0 && (port_count == 0 || (port_array && encap_id_array)));
    TEST_FULL_BCM_IF_ERR_EXIT(bcm_multicast_egress_set(unit, group, port_count, port_array, encap_id_array));

    BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_group_to_id(group, &multicast_id));
    mc_id = (int32) multicast_id;

    is_bitmap_group = _BCM_MULTICAST_TYPE_GET(group) == _BCM_MULTICAST_TYPE_PORTS_GROUP ||
      mc_id <= SOC_DPP_CONFIG(unit)->tm.multicast_egress_bitmap_group_range.mc_id_high;

    if (is_bitmap_group && mc_id > SOC_DPP_CONFIG(unit)->tm.multicast_egress_bitmap_group_range.mc_id_high) {
        BCM_EXIT;
    }
    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_remove_egress_group_members(unit, multicast_id, 0)); /* remove previous members */
    SOC_DPP_CORES_ITER(SOC_DPP_CORE_ALL(unit), core){
        testgroup = data->egress_mc_groups + core * SOC_DPP_CONFIG(unit)->tm.nof_mc_ids + multicast_id;
        BCMDNX_IF_ERR_EXIT(multicast_arad_test_assert_group_open(unit, testgroup));
        /* add members to group */
        /* Resolve GPORT to local ports bitmap and local ports arr + encap */
        for (i = 0; i < port_count ; ++i) {
            if (!is_bitmap_group && port_array[i] == BCM_GPORT_INVALID ) { /* outlif only replication (port mapped from outlif) */
                if (SOC_IS_JERICHO(unit)) {
                    DPP_CUD2CORE_GET_CORE(unit, encap_id_array[i], cud_core);
                    cud_core = (cud_core == core) ? TRUE : FALSE; /*check if this is the correct core*/
                } else {
                    cud_core = TRUE; /*Arad has only one core*/
                }
                if (cud_core) {
                    BCMDNX_IF_ERR_EXIT(multicast_arad_test_add_member(unit, testgroup, encap_id_array[i], MC_MEMBER_DEST_OUTLIF_ONLY)); 
                }
            } else if (!is_bitmap_group && BCM_GPORT_IS_MCAST(port_array[i])) { /* bitmap replication */
                dest = SOC_IS_QAX(unit) ? (mcds->egress_bitmap_start + BCM_GPORT_MCAST_GET(port_array[i])*SOC_DPP_CONFIG(unit)->qax->nof_egress_bitmap_bytes + MC_MEMBER_DEST_BITMAP_OFFSET) 
                : (BCM_GPORT_MCAST_GET(port_array[i]) + MC_MEMBER_DEST_BITMAP_OFFSET);
                
                BCMDNX_IF_ERR_EXIT(multicast_arad_test_add_member(unit, testgroup, encap_id_array[i], dest));
            } else {
                uint32 local_port;
                bcm_pbmp_t pbmp_local_ports;
                bcm_if_t port_encap = encap_id_array[i]; 
                /* Derive local ports */
                if (BCM_GPORT_IS_TRILL_PORT(port_array[i])) {
                    /*extract port and real encap id from trill encap*/
                    port_encap &= _BCM_TRILL_ENCAP_MASK;
                    BCM_PBMP_CLEAR(pbmp_local_ports);
                    BCM_PBMP_PORT_ADD(pbmp_local_ports,(encap_id_array[i] >> _BCM_TRILL_ENCAP_PORT_SHIFT ));
                } else { /* not a trill port */
                    _bcm_dpp_gport_info_t gport_info;
                    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port_array[i], _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));
                    BCM_PBMP_ASSIGN(pbmp_local_ports, gport_info.pbmp_local_ports);
                }
                BCM_PBMP_ITER(pbmp_local_ports,local_port) {
                    BCMDNX_IF_ERR_EXIT(bcm_petra_port_get(unit, local_port, &flags, &interface_info, &mapping_info));
                    if (core == mapping_info.core) {
                        BCMDNX_IF_ERR_EXIT(multicast_arad_test_add_member(unit, testgroup, port_encap, local_port)); 
                    }
                }
            }
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* not supported */
int test_bcm_multicast_egress_subscriber_set(int unit, bcm_multicast_t group,
                                            int port_count,
                                            bcm_gport_t *port_array,
                                            bcm_if_t *encap_id_array,
                                            bcm_gport_t *subscriber_queue_array)
{
    return bcm_multicast_egress_subscriber_set(unit, group, port_count, port_array, encap_id_array, subscriber_queue_array);
}


#define _COMPARE_TEMP_SIZE 256
#define _MAX_MEMBER_COMPARE_ERRORS 5
/*
 * Compare the members in two input arrays representing groups, by first sorting them (changing the input arrays).
 * nof_errors is both input and output - number of errors found
 */
int test_bcm_multicast_compare_group_member_data(int unit, mc_member_data* group1, const unsigned group1_size,
                                                 mc_member_data* group2, const unsigned group2_size,
                                                 const SOC_TMC_MULT_ID multicast_id, unsigned* nof_errors)
{
    unsigned i1, i2;
    int comp;
    BCMDNX_INIT_FUNC_DEFS;
    TEST_ASSERT(nof_errors && group1 && group2);

    if (group1_size != group2_size) {
        ++*nof_errors;
        ERROR_PRINT((_BCM_MSG("MC test found group 0x%lx to have a size %u which is different than the expected size %u"), (unsigned long)multicast_id, group2_size, group1_size));
    }

    mc_member_data_sort(group1, group1_size); /* sort the members of group1 */
    mc_member_data_sort(group2, group2_size); /* sort the members of group2 */

    /* compare the two sorted arrays */
    for (i1 = i2 = 0; ; ++i1, ++i2) {
        /* First check if we reached the end of one of the two groups */
        if (i1 >= group1_size) { /* reached end of group1 */
            for (;i2 < group2_size; ++i2) { /* if not also reached end of group2, its remains are unique */
                ERROR_PRINT((_BCM_MSG("MC test found dest=0x%lx cud=0x%lx only at the BCM API output at index %u in group 0x%lx"), (unsigned long)group2[i2].dest, (unsigned long)group2[i2].cud, i2, (unsigned long)multicast_id));
                if (++*nof_errors >= _MAX_MEMBER_COMPARE_ERRORS) {
                    break;
                }
            }
            break;
        } else if (i2 >= group2_size) { /* reached end of group2, and not the end of group1 whose remains are unique */
            for (;i1 < group1_size; ++i1) {
                ERROR_PRINT((_BCM_MSG("MC test found dest=0x%lx cud=0x%lx only at the test data structure at index %u in group 0x%lx"), (unsigned long)group1[i1].dest, (unsigned long)group1[i1].cud, i1, (unsigned long)multicast_id));
                if (++*nof_errors >= _MAX_MEMBER_COMPARE_ERRORS) {
                    break;
                }
            }
            break;
        }

        /* compare the next member in each arrays */
        comp = mc_member_data_compare(group1 + i1, group2 + i2);
        if (comp) { /* the two members are different */
            if (comp < 0) { /* group1[i1] < group2[i2] */
                ERROR_PRINT((_BCM_MSG("MC test found dest=0x%lx cud=0x%lx only at the test data structure at index %u in group 0x%lx"), (unsigned long)group1[i1].dest, (unsigned long)group1[i1].cud, i1, (unsigned long)multicast_id));
                --i2;
            } else { /* group1[i1] > group2[i2] */
                ERROR_PRINT((_BCM_MSG("MC test found dest=0x%lx cud=0x%lx only at the BCM API output at index %u in group 0x%lx"), (unsigned long)group2[i2].dest, (unsigned long)group2[i2].cud, i2, (unsigned long)multicast_id));
                --i1;
            }
            if (++*nof_errors >= _MAX_MEMBER_COMPARE_ERRORS) {
                break;
            }
        }
    }

    if (*nof_errors) {
        if (*nof_errors >= _MAX_MEMBER_COMPARE_ERRORS) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BCM_MSG("stopping MC group comparison after too many errors")));
        }
      BCM_ERR_EXIT_NO_MSG(BCM_E_INTERNAL);
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Compare the members of a non VLAN bitmap open MC group mc_group with the members in port_array[size], encap_id_array[size].
 * Assumes that the members of mc_group have no duplicates.
 * This is fine since multicast_arad_test_add_member() adds no duplicates
 */
int test_bcm_multicast_compare_member_list(int unit, const mc_member* mc_group, SOC_TMC_MULT_ID multicast_id,
                                           const bcm_gport_t *port_array, const bcm_if_t *encap_id_array, const unsigned size)
{
    unsigned mc_group_size = 0, bcm_buf_i;
    unsigned buffer_size = _COMPARE_TEMP_SIZE;
    const mc_member* i = mc_group;
    int is_open;
    unsigned errors_found = 0;
    uint8 is_egress = 0;
    uint32 core = 0;
    mc_member_data *stack_buffer = NULL;
    mc_member_data* bcm_buffer;
    mc_member_data* buffer = NULL;
    mc_test_data_t *data = test_data[unit];
    BCMDNX_INIT_FUNC_DEFS;
    TEST_ASSERT(multicast_is_initialized(unit));

    BCMDNX_IF_ERR_EXIT(multicast_arad_test_is_group_open(unit, mc_group, &is_open));
    BCMDNX_ALLOC(stack_buffer, sizeof(mc_member_data) * _COMPARE_TEMP_SIZE * 2, "test_bcm_multicast_compare_member_list.stack_buffer");
    if (!stack_buffer) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BCM_MSG("Memory allocation failure")));
    }
    bcm_buffer = stack_buffer + _COMPARE_TEMP_SIZE;
    buffer = stack_buffer;

    if (is_open && size > _COMPARE_TEMP_SIZE) { /* allocate bigger buffer if needed */
        buffer = NULL;
        buffer_size = size;
        BCMDNX_ALLOC(buffer, (sizeof(mc_member_data) * 2) * size, "multicast_group_comparison");
        if (!buffer) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BCM_MSG("Failed allocating comparison memory for multicast group of %d members"), size));
        }
        bcm_buffer = buffer + size;
    }
    /* copy port_array & encap_id_array into bcm_buffer */
    if (mc_group - data->egress_mc_groups >= 0 && mc_group - data->egress_mc_groups < SOC_DPP_CONFIG(unit)->tm.nof_mc_ids) { /* egress MC group */
        is_egress = 1;
        for (bcm_buf_i = 0; bcm_buf_i < size ; ++bcm_buf_i) {
            bcm_buffer[bcm_buf_i].cud = encap_id_array[bcm_buf_i];
            if (port_array[bcm_buf_i] == BCM_GPORT_INVALID ) { /* outlif only replication (port mapped from outlif) */
                bcm_buffer[bcm_buf_i].dest = MC_MEMBER_DEST_OUTLIF_ONLY;
            } else if (BCM_GPORT_IS_MCAST(port_array[bcm_buf_i])) { /* bitmap replication */
                bcm_buffer[bcm_buf_i].dest = BCM_GPORT_MCAST_GET(port_array[bcm_buf_i]) + MC_MEMBER_DEST_BITMAP_OFFSET;
            } else {
                /* Derive local ports */
                if (BCM_GPORT_IS_TRILL_PORT(port_array[bcm_buf_i])) {
                    bcm_buffer[bcm_buf_i].dest = bcm_buffer[bcm_buf_i].cud >> _BCM_TRILL_ENCAP_PORT_SHIFT;
                    bcm_buffer[bcm_buf_i].cud &= _BCM_TRILL_ENCAP_MASK;
                } else { /* not a trill port */
                    uint32 ports_count;
                    _bcm_dpp_gport_info_t gport_info;
                    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port_array[bcm_buf_i], _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info)); 
                    BCM_PBMP_COUNT(gport_info.pbmp_local_ports, ports_count);
                    if(ports_count > 1) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BCM_MSG("can not have a LAG here, gport=0x%lx"),(unsigned long)port_array[bcm_buf_i]));
                    }
                    bcm_buffer[bcm_buf_i].dest = gport_info.local_port;
                }
            }
        }
    } else { /* ingress MC group */
        for (bcm_buf_i = 0; bcm_buf_i < size ; ++bcm_buf_i) {
            uint32 dest;
            SOC_TMC_DEST_INFO destination;
            /* Convert GPORT to TM dest */
            /* SOC_TMC_DEST_INFO_clear(&destination); */
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_tm_dest_info(unit, port_array[bcm_buf_i], &destination));
            if (destination.type == SOC_TMC_DEST_TYPE_QUEUE)
            { /* direct Queue_id */
                dest = destination.id | DPP_MC_ING_DESTINATION_FLD_TYPE_TM_FLOW;
            } else if (destination.type == SOC_TMC_DEST_TYPE_MULTICAST) {
                /* multicast_id */
                if (SOC_IS_JERICHO(unit)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BCM_MSG("Wrong type for Jericho")));
                }
                dest = destination.id | ARAD_MC_ING_DESTINATION_FLD_TYPE_MCID; 
            } else { /* sys_port_id */
                if (destination.type == SOC_TMC_DEST_TYPE_LAG) {
                  BCM_SAND_IF_ERR_EXIT(arad_ports_logical_sys_id_build(unit, TRUE, destination.id, 0, 0, &dest));
                  dest |= DPP_MC_ING_DESTINATION_FLD_TYPE_LAG;
                } else { /* destination-type == sys_phy_port*/
                  BCM_SAND_IF_ERR_EXIT(arad_ports_logical_sys_id_build(unit, FALSE, 0, 0, destination.id, &dest));
                  dest |= DPP_MC_ING_DESTINATION_FLD_TYPE_PORT;
                }
            }
            bcm_buffer[bcm_buf_i].dest = dest;
            bcm_buffer[bcm_buf_i].cud = encap_id_array[bcm_buf_i];
        }
    }
    if (is_open) {
        SOC_DPP_CORES_ITER(SOC_DPP_CORE_ALL(unit), core){
            if (!is_egress && core != 0) { /*if this is an ingress group, we only have one core to collect it's replications*/
                break;
            }
            i = mc_group + core * SOC_DPP_CONFIG(unit)->tm.nof_mc_ids;
            if (!mc_member_is_used_get(i)) { /*make sure the member is used (might not be used when all replications are at one core)*/
                continue;
            }
            /* copy mc_group to buffer */
            for (; ;) {
                
                if (mc_member_dest_get(i) != MC_MEMBER_DEST_OUTLIF_ONLY && mc_member_dest_get(i) > MC_MEMBER_DEST_BITMAP_OFFSET && core != 0) {
                    /*if this is a bitmap replication, the API returns one copy per replications and not per core, so we need to skip it*/
                    BCMDNX_IF_ERR_EXIT(multicast_arad_test_get_next(unit, i, &i)); /* get next member */
                    if (!i) { /* no next member */
                        break;
                    }
                    continue;
                }
                buffer[mc_group_size].dest = mc_member_dest_get(i); 
                buffer[mc_group_size].cud = mc_member_cud_get(i);
                ++mc_group_size;
                BCMDNX_IF_ERR_EXIT(multicast_arad_test_get_next(unit, i, &i)); /* get next member */
                if (!i) { /* no next member */
                    break;
                } else if (mc_group_size == buffer_size) {
                    errors_found = 1;
                    ERROR_PRINT((_BCM_MSG("MC test will not read all the members to prevent buffer overflow for group 0x%lx, compare may not be accurate"), (unsigned long)multicast_id));
                    break;
                }
            }
        }
    }

    /* compare buffer with bcm_buffer */
    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_compare_group_member_data(unit, buffer, mc_group_size, bcm_buffer, size, multicast_id, &errors_found));

exit:
    if (buffer != stack_buffer) {
        BCM_FREE(buffer);
    }
    BCM_FREE(stack_buffer);
    BCMDNX_FUNC_RETURN;
}



/* Get the set of encapsulated interfaces on specific ports which comprises a multicast group's replication list. The port_array contains physical ports where replicated packets will be sent. The encap_id_array contains the opaque encapsulation IDs returned from the bcm_multicast_*_encap_get APIs. The subscriber_queue is an array of opaque subscriber queue IDs. The subscriber form is only available on certain devices in specific modes; if the non-subscriber form is used in these modes, it will not return the subscriber queue group IDs. If port_max = 0, port_array and encap_id_array must be NULL, and the function will return in port_count the number of ports in the given multicast group. */
int test_bcm_multicast_egress_get(int unit, bcm_multicast_t group, int port_max, 
                                  bcm_gport_t *port_array,
                                  bcm_if_t *encap_id_array, 
                                  int *port_count)
{
    SOC_TMC_MULT_ID multicast_id;
    mc_member* testgroup;
    mc_test_data_t *data = test_data[unit];
    BCMDNX_INIT_FUNC_DEFS;
    TEST_ASSERT(multicast_is_initialized(unit) && port_count && port_max >= 0 && (port_max == 0 || (port_array && encap_id_array)));

    BCMDNX_IF_ERR_EXIT(bcm_multicast_egress_get(unit, group, port_max, port_array, encap_id_array, port_count));
    TEST_ASSERT(*port_count <= port_max && *port_count >= 0);

    BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_group_to_id(group, &multicast_id));
    testgroup = data->egress_mc_groups + multicast_id;
    BCMDNX_IF_ERR_EXIT(multicast_arad_test_assert_group_open(unit, testgroup));

    if (multicast_id < data->nof_vlan_egress_mc_groups) { /* This is a VLAN bitmap MC group */
        
        int i;
        uint32 entry[ARAD_MC_VLAN_BITMAP_ENTRY_SIZE]; /* 256 port bits and 8 bits parity */
        uint32* vlan_bits = data->vlan_bitmaps + (UINT32SPER_VLAN * multicast_id);
        BCMDNX_IF_ERR_EXIT(READ_EGQ_VLAN_TABLEm(unit, MEM_BLOCK_ANY, multicast_id, entry)); /* read the vlan bitmap from the chip */
        for (i = 0;  i < UINT32SPER_VLAN; ++i) { /* loop over bitmap ports, assumes BCM output is sorted */
            if (vlan_bits[i] != entry[i]) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BCM_MSG("MC test found different bits in VLAN 0x%lx at index %d hardware:0x%lx expected:0x%lx"), (unsigned long)multicast_id, i, (unsigned long)entry[i], (unsigned long)vlan_bits[i]));
            }
        }
    } else { /* regular MC group, compare it using the compare function */
        BCMDNX_IF_ERR_EXIT(test_bcm_multicast_compare_member_list(unit, testgroup, multicast_id, port_array, encap_id_array, *port_count));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* not supported */
int test_bcm_multicast_egress_subscriber_get(int unit, bcm_multicast_t group,
                                            int port_max,
                                            bcm_gport_t *port_array,
                                            bcm_if_t *encap_id_array,
                                            bcm_gport_t *subscriber_queue_array,
                                            int *port_count)
{
    return bcm_multicast_egress_subscriber_get(unit, group, port_max, port_array, encap_id_array, subscriber_queue_array, port_count);
}

/* Given a multicast group previously reserved by bcm_multicast_create , this function will return the flags necessary to create the same group on a different unit. That is, the returned flags values will be the value of BCM_MULTICAST_TYPE_* with which the group was created combined with the BCM_MULTICAST_WITH_ID flag. The returned flags will include BCM_MULTICAST_WITH_ID even if the group was not originally created via this flag. */
int test_bcm_multicast_group_get(int unit, bcm_multicast_t group, uint32 *flags)
{
    return bcm_multicast_group_get(unit, group, flags);
}


#ifdef _ARAD_MC_TESTS_SUPPORT_UNSUPPORTED_APIS_0
/* This API associates the given Multicast group ID to the given fabric distribution ID. The Fabric Distribution ID is effectively used to derive the base QID of the queue group that is used for packets hitting the specified Multicast Group ID. */
/* not supported */
int test_bcm_multicast_fabric_distribution_set(int unit, bcm_multicast_t group, 
                                              bcm_fabric_distribution_t ds_id);
/* Get the configured Fabric Distribution ID for the given Multicast group ID. */
/* not supported */
int test_bcm_multicast_fabric_distribution_get(int unit, bcm_multicast_t group, 
                                              bcm_fabric_distribution_t *ds_id);

/* Assign a set of VLANs as selected port's replication list for the chosen multicast group. A new set of VLANs will replace the previous set for a particular (mc_index, port) pair. An empty VLAN set will erase all replications for the (mc_index, port) pair.*/
/* not supported */
int test_bcm_multicast_repl_set(int unit, int mc_index, bcm_port_t port,
                               bcm_vlan_vector_t vlan_vec);

/* Return the set of VLANs selected for a port's replication list for the chosen multicast group. */
/* not supported */
int test_bcm_multicast_repl_get(int unit, int index, bcm_port_t port,
                          bcm_vlan_vector_t vlan_vec);

/* Iterates over created multicast groups and executes user callback function for each valid entry. The flags parameter specifies the multicast types which should be included. For all types, use BCM_MULTICAST_TYPE_MASK.*/
/* not supported */
int test_bcm_multicast_group_traverse(int unit,
                                     bcm_multicast_group_traverse_cb_t trav_fn,
                                     uint32 flags, void *user_data);

/* Sets/gets miscellaneous per multicast group controls. The controls are defined in the bcm_multicast_control_t enumerated type Cross-Reference 1699 (multicast_control) . */
/* not supported */
int test_bcm_multicast_control_set(int unit, bcm_multicast_t group,
                                  bcm_multicast_control_t type, int arg);
/* not supported */
int test_bcm_multicast_control_get(int unit, bcm_multicast_t group,
                                  bcm_multicast_control_t type, int *arg);
#endif /* 0 */

/* Add a port to a multicast group's replication list in device Ingress. The port is the physical port where replicated packets will be sent. The encap_id is an opaque encapsulation ID returned from the bcm_multicast_*_encap_get APIs. */
int test_bcm_multicast_ingress_add(
        int unit, 
        bcm_multicast_t group, 
        bcm_gport_t port, 
        bcm_if_t encap_id,
        int may_run_out_of_space)
{
    SOC_TMC_MULT_ID multicast_id;
    uint32 dest;
    mc_member* testgroup;
    SOC_TMC_DEST_INFO destination;
    mc_test_data_t *data = test_data[unit];
    BCMDNX_INIT_FUNC_DEFS;
    TEST_ASSERT(multicast_is_initialized(unit));
    TEST_FULL_BCM_IF_ERR_EXIT(bcm_multicast_ingress_add(unit, group, port, encap_id)); /* call the BCM API */

    /* Convert GPORT to TM dest */
    SOC_TMC_DEST_INFO_clear(&destination);
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_tm_dest_info(unit, port, &destination));

    /* calculate dest */
    if (destination.type == SOC_TMC_DEST_TYPE_QUEUE)
    { /* direct Queue_id */
        dest = destination.id | DPP_MC_ING_DESTINATION_FLD_TYPE_TM_FLOW;
    } else if (destination.type == SOC_TMC_DEST_TYPE_MULTICAST) {   
        /* multicast_id */
        if (SOC_IS_JERICHO(unit)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BCM_MSG("Wrong type for Jericho")));
        }
        dest = destination.id | ARAD_MC_ING_DESTINATION_FLD_TYPE_MCID; 
    } else { /* sys_port_id */
        if (destination.type == SOC_TMC_DEST_TYPE_LAG) {
          BCM_SAND_IF_ERR_EXIT(arad_ports_logical_sys_id_build(unit, TRUE, destination.id, 0, 0, &dest));
          dest |= DPP_MC_ING_DESTINATION_FLD_TYPE_LAG;
        } else { /* destination-type == sys_phy_port*/
          BCM_SAND_IF_ERR_EXIT(arad_ports_logical_sys_id_build(unit, FALSE, 0, 0, destination.id, &dest));
          dest |= DPP_MC_ING_DESTINATION_FLD_TYPE_PORT;
        }
    }

    BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_group_to_id(group, &multicast_id));
    testgroup = data->ingress_mc_groups + multicast_id;
    BCMDNX_IF_ERR_EXIT(multicast_arad_test_assert_group_open(unit, testgroup));

    /* In Arad, an encap-id of 0 for a non ERP system port needs to be changed */
    if (!encap_id) {
        if (destination.type == SOC_TMC_DEST_TYPE_SYS_PHY_PORT) {
            uint32    fap_id = 0;
            uint32    fap_port_id = 0;
            /* get module+port from system port, in order to check if this is an ERP port*/
            BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_sys_phys_to_local_port_map_get,(unit, destination.id, &fap_id, &fap_port_id)));
            BCM_DPP_UNIT_CHECK(unit);
            if (BCM_SAND_FAP_PORT_ID_TO_BCM_PORT(fap_port_id) != SOC_TMC_FAP_EGRESS_REPLICATION_IPS_PORT_ID) {
                encap_id = SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_cud;
            }
        } else {
            encap_id = SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_cud;
        }
    }

    BCMDNX_IF_ERR_EXIT(multicast_arad_test_add_member(unit, testgroup, encap_id, dest));

exit:
    BCMDNX_FUNC_RETURN;
}

/* Delete a port from multicast group's replication list in device Ingress. The port is the physical port where replicated packets will be sent. The encap_id is an opaque encapsulation ID returned from the bcm_multicast_*_encap_get APIs. */
int test_bcm_multicast_ingress_delete(
        int unit, 
        bcm_multicast_t group, 
        bcm_gport_t port, 
        bcm_if_t encap_id)
{
    SOC_TMC_MULT_ID multicast_id;
    SOC_TMC_DEST_INFO destination;
    uint32 dest;
    mc_member* testgroup;
    mc_member member_to_find = {0,0};
    int found;
    mc_test_data_t *data = test_data[unit];
    BCMDNX_INIT_FUNC_DEFS;

    TEST_ASSERT(multicast_is_initialized(unit));
    BCMDNX_IF_ERR_EXIT(bcm_multicast_ingress_delete(unit, group, port, encap_id)); /* call the BCM API */

    /* Convert GPORT to TM dest */
    SOC_TMC_DEST_INFO_clear(&destination);
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_tm_dest_info(unit, port, &destination));

    /* calculate dest */
    if (destination.type == SOC_TMC_DEST_TYPE_QUEUE)
    { /* direct Queue_id */
        dest = destination.id | DPP_MC_ING_DESTINATION_FLD_TYPE_TM_FLOW;
    } else if (destination.type == SOC_TMC_DEST_TYPE_MULTICAST) {
        /* multicast_id */
        if (SOC_IS_JERICHO(unit)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BCM_MSG("Wrong type for Jericho")));
        }
        dest = destination.id | ARAD_MC_ING_DESTINATION_FLD_TYPE_MCID;
    } else { /* sys_port_id */
        if (destination.type == SOC_TMC_DEST_TYPE_LAG) {
          BCM_SAND_IF_ERR_EXIT(arad_ports_logical_sys_id_build(unit, TRUE, destination.id, 0, 0, &dest));
          dest |= DPP_MC_ING_DESTINATION_FLD_TYPE_LAG;
        } else { /* destination-type == sys_phy_port*/
          BCM_SAND_IF_ERR_EXIT(arad_ports_logical_sys_id_build(unit, FALSE, 0, 0, destination.id, &dest));
          dest |= DPP_MC_ING_DESTINATION_FLD_TYPE_PORT;
        }
    }

    BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_group_to_id(group, &multicast_id));
    testgroup = data->ingress_mc_groups + multicast_id;
    BCMDNX_IF_ERR_EXIT(multicast_arad_test_assert_group_open(unit, testgroup));

    /* In Arad, an encap-id of 0 for a non ERP system port needs to be changed */
    if (!encap_id) {
        if (destination.type == SOC_TMC_DEST_TYPE_SYS_PHY_PORT) {
            uint32    fap_id;
            uint32    fap_port_id = 0;
            /* get module+port from system port, in order to check if this is an ERP port*/
            BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_sys_phys_to_local_port_map_get,(unit, destination.id, &fap_id, &fap_port_id)));
            BCM_DPP_UNIT_CHECK(unit);
            if (BCM_SAND_FAP_PORT_ID_TO_BCM_PORT(fap_port_id) != SOC_TMC_FAP_EGRESS_REPLICATION_IPS_PORT_ID) {
                encap_id = SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_cud;
            }
        } else {
            encap_id = SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_cud;
        }
    }

    /* Find the member and remove it */
    mc_member_cud_set(unit, &member_to_find, encap_id);
    mc_member_dest_set(unit, &member_to_find, dest);
    BCMDNX_IF_ERR_EXIT(multicast_arad_test_remove_member(unit, testgroup, 1, &member_to_find, &found));
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BCM_MSG("could not find member dest=0x%lx cud=0x%lx to remove in group 0x%lx"), (unsigned long)dest, (unsigned long)encap_id, (unsigned long)multicast_id));
    }
exit:
    BCMDNX_FUNC_RETURN;
}



/* Remove all ports from Multicast replication list from device Ingress. */
int test_bcm_multicast_ingress_delete_all(
        int unit, 
        bcm_multicast_t group)
{
    SOC_TMC_MULT_ID multicast_id;
    BCMDNX_INIT_FUNC_DEFS;

    TEST_ASSERT(multicast_is_initialized(unit));
    BCMDNX_IF_ERR_EXIT(bcm_multicast_ingress_delete_all(unit, group));

    BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_group_to_id(group, &multicast_id));
    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_remove_ingress_group_members(unit, multicast_id, 0));
exit:
    BCMDNX_FUNC_RETURN;
}

/* Add a set of ports to a multicast group's replication list in device Ingress. The port_array contains physical ports where replicated packets will be sent. The encap_id_array contains the opaque encapsulation IDs returned from the bcm_multicast_*_encap_get APIs. */
int test_bcm_multicast_ingress_set(int unit, bcm_multicast_t group, int port_count,
                             bcm_gport_t *port_array, bcm_if_t *encap_id_array, int may_run_out_of_space)
{
    SOC_TMC_MULT_ID multicast_id;
    int i;
    mc_member* testgroup;
    mc_test_data_t *data = test_data[unit];
    BCMDNX_INIT_FUNC_DEFS;
    TEST_ASSERT(multicast_is_initialized(unit) && port_count>= 0 && (port_count == 0 || (port_array && encap_id_array)));
    TEST_FULL_BCM_IF_ERR_EXIT(bcm_multicast_ingress_set(unit, group, port_count, port_array, encap_id_array));

    BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_group_to_id(group, &multicast_id));
    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_remove_ingress_group_members(unit, multicast_id, 0)); /* remove previous members */
    testgroup = data->ingress_mc_groups + multicast_id;
    BCMDNX_IF_ERR_EXIT(multicast_arad_test_assert_group_open(unit, testgroup));

    for (i = 0; i < port_count ; ++i) { /* add members to group */
        SOC_TMC_DEST_INFO destination;
        uint32 dest;
        bcm_if_t encap_id = encap_id_array[i];
        /* Convert GPORT to TM dest */
        SOC_TMC_DEST_INFO_clear(&destination);
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_tm_dest_info(unit, port_array[i], &destination));

        /* calculate dest */
        if (destination.type == SOC_TMC_DEST_TYPE_QUEUE)
        { /* direct Queue_id */
            dest = destination.id | DPP_MC_ING_DESTINATION_FLD_TYPE_TM_FLOW;
        } else if (destination.type == SOC_TMC_DEST_TYPE_MULTICAST) {
            /* multicast_id */
            if (SOC_IS_JERICHO(unit)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BCM_MSG("Wrong type for Jericho")));
            }
            dest = destination.id | ARAD_MC_ING_DESTINATION_FLD_TYPE_MCID;
        } else { /* sys_port_id */
            if (destination.type == SOC_TMC_DEST_TYPE_LAG) {
              BCM_SAND_IF_ERR_EXIT(arad_ports_logical_sys_id_build(unit, TRUE, destination.id, 0, 0, &dest));
              dest |= DPP_MC_ING_DESTINATION_FLD_TYPE_LAG;
            } else { /* destination-type == sys_phy_port*/
              BCM_SAND_IF_ERR_EXIT(arad_ports_logical_sys_id_build(unit, FALSE, 0, 0, destination.id, &dest));
              dest |= DPP_MC_ING_DESTINATION_FLD_TYPE_PORT;
            }
        }

        /* In Arad, an encap-id of 0 for a non ERP system port needs to be changed */
        if (!encap_id) {
            if (destination.type == SOC_TMC_DEST_TYPE_SYS_PHY_PORT) {
                uint32    fap_id;
                uint32    fap_port_id = 0;
                /* get module+port from system port, in order to check if this is an ERP port*/
                BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_sys_phys_to_local_port_map_get,(unit, destination.id, &fap_id, &fap_port_id)));
                BCM_DPP_UNIT_CHECK(unit);
                if (BCM_SAND_FAP_PORT_ID_TO_BCM_PORT(fap_port_id) != SOC_TMC_FAP_EGRESS_REPLICATION_IPS_PORT_ID) {
                    encap_id = SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_cud;
                }
            } else {
                encap_id = SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_cud;
            }
        }

        BCMDNX_IF_ERR_EXIT(multicast_arad_test_add_member(unit, testgroup, encap_id, dest)); /* add member */
    }
exit:
    BCMDNX_FUNC_RETURN;
}


/* Get the ports in a multicast group's replication list from device Ingress. The port_array contains physical ports where replicated packets will be sent. The encap_id_array contains the opaque encapsulation IDs returned from the bcm_multicast_*_encap_get APIs. If port_max = 0, port_array and encap_id_array must be NULL, and the function will return in port_count the number of ports in the given multicast group. */
int test_bcm_multicast_ingress_get(
        int unit, 
        bcm_multicast_t group, 
        int port_max, 
        bcm_gport_t *port_array, 
        bcm_if_t *encap_id_array, 
        int *port_count)
{
    SOC_TMC_MULT_ID multicast_id;
    mc_member* testgroup;
    mc_test_data_t *data = test_data[unit];
    BCMDNX_INIT_FUNC_DEFS;
    TEST_ASSERT(multicast_is_initialized(unit) && port_count && port_max >= 0 && (port_max == 0 || (port_array && encap_id_array)));

    BCMDNX_IF_ERR_EXIT(bcm_multicast_ingress_get(unit, group, port_max, port_array, encap_id_array, port_count));
    TEST_ASSERT(*port_count <= port_max && *port_count >= 0);

    BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_group_to_id(group, &multicast_id));
    testgroup = data->ingress_mc_groups + multicast_id;
    BCMDNX_IF_ERR_EXIT(multicast_arad_test_assert_group_open(unit, testgroup));

    /* compare the group output of the BCM API to the test data structure using the compare function */
    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_compare_member_list(unit, testgroup, multicast_id, port_array, encap_id_array, *port_count));

exit:
    BCMDNX_FUNC_RETURN;
}

/* check that the ingress/egress MC group open status according to BCM APIs is the same as expected_open */
int test_bcm_multicast_check_group_open_by_bcm_api(int unit, SOC_TMC_MULT_ID multicast_id_ndx, const uint8 is_egress, const unsigned expected_open)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_is_group_exist(unit,
      is_egress ? BCM_DPP_MULTICAST_TYPE_EGRESS_ONLY : BCM_DPP_MULTICAST_TYPE_INGRESS_ONLY,
      multicast_id_ndx, expected_open ? 1 : 0));
exit:
    BCMDNX_FUNC_RETURN;
}

/* Add egress replications to an array */
int test_bcm_multicast_add_to_array(int unit, mc_member_data* buffer, unsigned* current_buf_size, uint32 dest, uint32 cud)
{
    BCMDNX_INIT_FUNC_DEFS;
    if (*current_buf_size == ARAD_MC_MAX_GROUP_REPLICATIONS_EGRESS) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BCM_MSG("MCDB egress group contains more than the maximum number of members")));
    }
    buffer[*current_buf_size].dest = dest;
    buffer[*current_buf_size].cud = cud;
    ++*current_buf_size;
exit:
    BCMDNX_FUNC_RETURN;
}

#ifdef BCM_88470_A0
/* compare the whole chip MC+vlan tables to the test data structures */
int test_bcm_qax_multicast_verify_all(int unit)
{
    uint32* alloced_mem = NULL; /* regular memory allocated */
    uint32* alloced_pci_mem = NULL; /* memory allocated for DMA access */
    uint32* mcdb; /* TAR_MCDB read from chip */
    uint32* irdb; /* CGM_IS_ING_MC read from chip */
    uint32* used_bitmap; /* bitmap to mark if an MCDB entry was already used for a verified group */
    int core = 0;
    soc_port_t local_port;
    mc_member_data *mcg_buf1, *mcg_buf2; /* buffers for group sorting & comparison*/
    unsigned mcg_size1, mcg_size2; /* the sizes of mcg_buf1, mcg_buf2 */
    unsigned max_ingr_reps;
    const mc_member* group_ptr;
    uint32 used_entries = 0, rep_entries = 0; /* rep_entries will hold number of replications (bitmap counts a one) + number of open empty groups */
    int32 nof_egr_bitmap_entries = (SOC_DPP_CONFIG(unit)->qax->nof_egress_bitmaps * SOC_DPP_CONFIG(unit)->qax->nof_egress_bitmap_bytes);
#ifndef _ARAD_MC_NO_IMPLEMENTATION_SPECIFIC_TESTS
    uint32 group_entries1 = 0, group_entries2 = 0;
    qax_mcds_t *mcds = dpp_get_mcds(unit);
#endif
    int32 free_entries =  QAX_LAST_MCDB_ENTRY(unit) + 1, free_edge_entries = 0;
    SOC_TMC_MULT_ID i, j, next_entry;
    unsigned nof_uint32s, nof_errors = 0;
    unsigned is_open;
    mc_test_data_t *data = test_data[unit];
    BCMDNX_INIT_FUNC_DEFS;


    TEST_ASSERT(multicast_is_initialized(unit));
    TEST_PRINT((_BCM_MSG("starting to verify MCDB")));

    max_ingr_reps = 4096 - 2;

    TEST_ASSERT(data->ingress_mc_groups);

    TEST_ASSERT(data->egress_mc_groups);
    TEST_ASSERT(!(SOC_DPP_CONFIG(unit)->tm.nof_mc_ids % 32));

    
    /* allocate the buffers for this function in one allocation */
    if (soc_mem_dmaable(unit, TAR_MCDBm, SOC_MEM_BLOCK_ANY(unit, TAR_MCDBm))) { /* use DMA, separate allocation for memory to be filled by DMA */
        nof_uint32s = (QAX_LAST_MCDB_ENTRY(unit) + 1) * QAX_MC_ENTRY_SIZE + /* MCDB */
                      ((SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids + QAX_NOF_GROUPS_PER_IRDB_ENTRY -1) / QAX_NOF_GROUPS_PER_IRDB_ENTRY) * QAX_CGM_TABLE_ENTRY_WORDS; /* IRDB */
        alloced_pci_mem = soc_cm_salloc(unit, 4 * nof_uint32s, "mc_verify_dma");
        if (alloced_pci_mem == NULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BCM_MSG("Failed allocating dma memory for test_bcm_multicast_verify_all")));
        }

        nof_uint32s = ((QAX_LAST_MCDB_ENTRY(unit) + 1) + 31) / 32;

        BCMDNX_ALLOC(alloced_mem, 4 * nof_uint32s + 
                     ARAD_MC_MAX_GROUP_REPLICATIONS_ANY * 2 * sizeof(mc_member_data), "mc_verify_all");
        if (!alloced_mem) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BCM_MSG("Failed allocating test memory for test_bcm_multicast_verify_all")));
        }
        mcdb = alloced_pci_mem;
        irdb = mcdb + (QAX_LAST_MCDB_ENTRY(unit) + 1) * QAX_MC_ENTRY_SIZE;
        used_bitmap = alloced_mem;
        mcg_buf1 = (mc_member_data*)(alloced_mem + nof_uint32s);
        mcg_buf2 = mcg_buf1 + ARAD_MC_MAX_GROUP_REPLICATIONS_ANY;
    } else { /* do not use DMA, everything in the same memory allocation */
        nof_uint32s = (QAX_LAST_MCDB_ENTRY(unit) + 1) * QAX_MC_ENTRY_SIZE + /* MCDB */
                      ((SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids + QAX_NOF_GROUPS_PER_IRDB_ENTRY -1) / QAX_NOF_GROUPS_PER_IRDB_ENTRY) * IRDB_TABLE_ENTRY_WORDS; /* IRDB */
        BCMDNX_ALLOC(alloced_mem, 4 * nof_uint32s + ARAD_MC_MAX_GROUP_REPLICATIONS_ANY * 2 * sizeof(mc_member_data), "mc_verify_all");
        if (!alloced_mem) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BCM_MSG("Failed allocating test memory for test_bcm_multicast_verify_all")));
        }
        mcdb = alloced_mem;
        irdb = mcdb + (QAX_LAST_MCDB_ENTRY(unit) + 1) * QAX_MC_ENTRY_SIZE;
        used_bitmap = irdb + ((SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids + QAX_NOF_GROUPS_PER_IRDB_ENTRY -1) / QAX_NOF_GROUPS_PER_IRDB_ENTRY) * IRDB_TABLE_ENTRY_WORDS;
        mcg_buf1 = (mc_member_data*)(mcdb + nof_uint32s);
        TEST_ASSERT(!(((((uint32*)mcg_buf1) - used_bitmap) - SOC_DPP_CONFIG(unit)->tm.nof_mc_ids / 32 ) & ~1));
        mcg_buf2 = mcg_buf1 + ARAD_MC_MAX_GROUP_REPLICATIONS_ANY;
    }
    
    for (i = (((QAX_LAST_MCDB_ENTRY(unit) + 1) + 31) / 32); i;) { /* init used_ingr_bitmap to 0 */
        used_bitmap[--i] = 0;
    }

    /* read the multicast database table, IRDB, and the vlan bitmap table from the chip */
    BCMDNX_IF_ERR_EXIT(soc_mem_read_range(unit, TAR_MCDBm, MEM_BLOCK_ANY, 0, QAX_LAST_MCDB_ENTRY(unit), mcdb));
    BCMDNX_IF_ERR_EXIT(soc_mem_read_range(unit, CGM_IS_ING_MCm, MEM_BLOCK_ANY, 0, ((SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids + QAX_NOF_GROUPS_PER_IRDB_ENTRY -1) / QAX_NOF_GROUPS_PER_IRDB_ENTRY) - 1, irdb));

#ifndef _ARAD_MC_NO_IMPLEMENTATION_SPECIFIC_TESTS
    {
        qax_mcdb_entry_t *entry = QAX_GET_MCDB_ENTRY(mcds, 0);
        uint32* mcdb_ptr = mcdb;
        /* mark all free ingress entries */
        for (i = SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids; i; --i, ++entry, mcdb_ptr += QAX_MC_ENTRY_SIZE) { 
            TEST_ASSERT(!QAX_MCDS_ENTRY_GET_TEST_BIT(entry));
            if (DPP_MCDS_TYPE_IS_FREE(QAX_MCDS_ENTRY_GET_TYPE(entry))) {
                if (!(mcdb_ptr[0] == mcds->free_value.word0 && mcdb_ptr[1] == mcds->free_value.word1 && !((mcdb_ptr[2] ^ mcds->free_value.word2) & QAX_MC_ENTRY_MASK_VAL))) {
                    bsl_printf ("i=%d read:%x %x %x entry:%x %x %x free_value:%x %x %x\n", i, mcdb_ptr[0], mcdb_ptr[1], mcdb_ptr[2], entry->word0, entry->word1, entry->word2, mcds->free_value.word0,  mcds->free_value.word1, mcds->free_value.word2);}
                TEST_ASSERT(mcdb_ptr[0] == mcds->free_value.word0 && mcdb_ptr[1] == mcds->free_value.word1 && !((mcdb_ptr[2] ^ mcds->free_value.word2) & QAX_MC_ENTRY_MASK_VAL));
            } else {
                TEST_ASSERT(mcdb_ptr[0] == entry->word0 && mcdb_ptr[1] == entry->word1 && !((mcdb_ptr[2] ^ entry->word2) & QAX_MC_ENTRY_MASK_VAL));
                ++group_entries1;
                QAX_MCDS_ENTRY_SET_TEST_BIT_ON(entry);
            }
        }
        /* mark all free egress entries */
        for (i = (QAX_LAST_MCDB_ENTRY(unit) + 1 - SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids - nof_egr_bitmap_entries); i; --i, ++entry, mcdb_ptr += QAX_MC_ENTRY_SIZE) { 
            TEST_ASSERT(!QAX_MCDS_ENTRY_GET_TEST_BIT(entry));
            if (DPP_MCDS_TYPE_IS_FREE(QAX_MCDS_ENTRY_GET_TYPE(entry))) {
                TEST_ASSERT(mcdb_ptr[0] == mcds->free_value.word0 && mcdb_ptr[1] == mcds->free_value.word1 && !((mcdb_ptr[2] ^ mcds->free_value.word2) & QAX_MC_ENTRY_MASK_VAL));
            } else {
                TEST_ASSERT(mcdb_ptr[0] == entry->word0 && mcdb_ptr[1] == entry->word1 && !((mcdb_ptr[2] ^ entry->word2) & QAX_MC_ENTRY_MASK_VAL));
                ++group_entries1;
                QAX_MCDS_ENTRY_SET_TEST_BIT_ON(entry);
            }
        }
    }
#endif

    /* verify ingress MC groups */
    irdb -=2;
    if (data->ingress_mc_groups) {
        for (i = 0; i < SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids; ++i) {
            group_ptr = data->ingress_mc_groups + i;
            is_open = mc_member_is_ingress_get(group_ptr); /* group marked open in data structure */
            /* Check that group open state is the same in BCM and in test data structure */
            BCMDNX_IF_ERR_EXIT(test_bcm_multicast_check_group_open_by_bcm_api(unit, i, FALSE, is_open));

            if (is_open) { /* if group is open compare its members */
                /* read MC group from group_ptr into mcg_buf1 */
                mcg_size1 = 0;
                if (mc_member_is_used_get(group_ptr)) {
                    for (; group_ptr;) {
                        TEST_ASSERT(mc_member_is_used_get(group_ptr));
                        TEST_ASSERT(mc_member_is_ingress_get(group_ptr));
                        if (mc_member_is_used_get(group_ptr)) {
                            if (mcg_size1 == ARAD_MC_MAX_GROUP_REPLICATIONS_INGRESS) {
                                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BCM_MSG("ingress group 0x%lx data structure contains more than the maximum number of members"),(unsigned long)i));
                            }
                            mcg_buf1[mcg_size1].dest = mc_member_dest_get(group_ptr);
                            mcg_buf1[mcg_size1].cud = mc_member_cud_get(group_ptr);
                            ++mcg_size1;
                        }
                        BCMDNX_IF_ERR_EXIT(multicast_arad_test_get_next(unit, group_ptr, &group_ptr));
                    }
                }

                /* read MC group from mcdb into mcg_buf2 */
                mcg_size2 = 0;
                for (j = i; j != MCDS_INGRESS_LINK_END(mcds);) {
                    uint32 *mcdb_ptr = mcdb + j * QAX_MC_ENTRY_SIZE;
                    uint32* used_bitmap_ptr = used_bitmap + (j / 32);
                    uint32 used_bitmap_mask = 1 << (j % 32);
                    uint32 format = soc_mem_field32_get(unit, TAR_MCDB_SINGLE_REPLICATIONm, mcdb_ptr, FORMATf);
                    uint32 cud0 = soc_mem_field32_get(unit, TAR_MCDB_SINGLE_REPLICATIONm, mcdb_ptr, CUD_0f);
                    uint32 cud1;
                    uint32 destination0;
                    uint32 destination1;
#ifndef _ARAD_MC_NO_IMPLEMENTATION_SPECIFIC_TESTS
                    qax_mcdb_entry_t *entry = QAX_GET_MCDB_ENTRY(mcds, j);
#endif

                    TEST_ASSERT(j < MCDS_INGRESS_LINK_END(mcds));
                    if (mcg_size2 >= ARAD_MC_MAX_GROUP_REPLICATIONS_INGRESS) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BCM_MSG("MCDB ingress group 0x%lx contains more than the maximum number of members"),(unsigned long)i));
                    }
                    if (used_bitmap_mask & *used_bitmap_ptr) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BCM_MSG("MCDB entry 0x%lx for ingress group 0x%lx is used by a different group"),(unsigned long)j, (unsigned long)i));
                    }
                    *used_bitmap_ptr |= used_bitmap_mask;
                    ++used_entries;
#ifndef _ARAD_MC_NO_IMPLEMENTATION_SPECIFIC_TESTS
                    {
                        TEST_ASSERT(QAX_MCDS_ENTRY_GET_TEST_BIT(entry));
                        QAX_MCDS_ENTRY_SET_TEST_BIT_OFF(entry);
                        ++group_entries2;
                    }
#endif
                    switch (format) {
                    case 1: /*single replication*/
                      cud1 = soc_mem_field32_get(unit, TAR_MCDB_SINGLE_REPLICATIONm, mcdb_ptr, CUD_1f);
                      destination0 = soc_mem_field32_get(unit, TAR_MCDB_SINGLE_REPLICATIONm, mcdb_ptr, DESTINATIONf);
                      if (destination0 != QAX_MULT_ING_UNOCCUPIED_BASE_QUEUE_INDICATION) { /* occupied entry, fill entry in array */
                          if (mcg_size2 == ARAD_MC_MAX_GROUP_REPLICATIONS_INGRESS) {
                              BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BCM_MSG("MCDB ingress group 0x%lx contains more than the maximum number of members"),(unsigned long)i));
                          }
                          mcg_buf2[mcg_size2].dest = destination0;
                          mcg_buf2[mcg_size2].cud = cud0;
                          mcg_buf2[mcg_size2].cud2 = cud1;
                          ++mcg_size2;
                      }
                      j = soc_mem_field32_get(unit, TAR_MCDB_SINGLE_REPLICATIONm, mcdb_ptr, LINK_PTRf);
                      break;
                    case 2: /*double replication*/
                    case 3: /*double replication*/
                      destination0 = soc_mem_field32_get(unit, TAR_MCDB_DOUBLE_REPLICATIONm, mcdb_ptr, DESTINATION_0f);
                      cud1 = soc_mem_field32_get(unit, TAR_MCDB_DOUBLE_REPLICATIONm, mcdb_ptr, CUD_1f);
                      destination1 = soc_mem_field32_get(unit, TAR_MCDB_DOUBLE_REPLICATIONm, mcdb_ptr, DESTINATION_1f);
                      if (destination0 != QAX_MULT_ING_UNOCCUPIED_BASE_QUEUE_INDICATION) {
                          mcg_buf2[mcg_size2].dest = destination0;
                          mcg_buf2[mcg_size2].cud = cud0;
                          ++mcg_size2;
                      }
                      if (destination1 != QAX_MULT_ING_UNOCCUPIED_BASE_QUEUE_INDICATION) {
                          mcg_buf2[mcg_size2].dest = destination1;
                          mcg_buf2[mcg_size2].cud = cud1;
                          ++mcg_size2;
                      }
                      j++;
                      if (format==3) {
                        j = MCDS_INGRESS_LINK_END(mcds);
                      }
                      break; 
                    }
                    TEST_ASSERT(j <= MCDS_INGRESS_LINK_END(mcds));
                }
                rep_entries += mcg_size2 ? mcg_size2 : 1;
                TEST_ASSERT(mcg_size2 <= max_ingr_reps);

                /* compare the group output of the BCM API to the test data structure using the compare function */
                BCMDNX_IF_ERR_EXIT(test_bcm_multicast_compare_group_member_data(unit, mcg_buf1, mcg_size1, mcg_buf2, mcg_size2, i , &nof_errors));
                mcg_size1 = 1; /* expected value in IRDB */
            } else {
                mcg_size1 = 0; /* expected value in IDR_IRDB */
            }

            {
                unsigned irdb_bit_offset = (i % 32); /* bit offset in IDR_IRDB entry */
                if (!irdb_bit_offset) { /* read the IDR_IRDB entry the first time it is used */
                    irdb +=2;
                }
                TEST_ASSERT((((*irdb) >> irdb_bit_offset) & 1) == mcg_size1); /* verify the correct IRDB fields for the current group */
            }
        }
    }


    /* verify egress regular MC groups */
    for (i = 0; i < SOC_DPP_CONFIG(unit)->tm.nof_mc_ids; ++i) {
        group_ptr = data->egress_mc_groups + i;
        is_open = mc_member_is_ingress_get(group_ptr); /* group marked open in data structure */
        /* Check that group open state is the same in BCM and in test data structure */
        BCMDNX_IF_ERR_EXIT(test_bcm_multicast_check_group_open_by_bcm_api(unit, i, TRUE, is_open));

        if (is_open) { /* if group is open compare its members */
            /* read MC group from group_ptr into mcg_buf1 */
            mcg_size1 = 0;
            if (mc_member_is_used_get(group_ptr)) {
                for (; group_ptr;) {
                    TEST_ASSERT(mc_member_is_used_get(group_ptr));
                    TEST_ASSERT(mc_member_is_ingress_get(group_ptr));
                    if (mc_member_is_used_get(group_ptr)) {
                        if (mcg_size1 == ARAD_MC_MAX_GROUP_REPLICATIONS_EGRESS) {
                            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BCM_MSG("egress group 0x%lx data structure contains more than the maximum number of members"),(unsigned long)i));
                        }
                        mcg_buf1[mcg_size1].dest = mc_member_dest_get(group_ptr);
                        mcg_buf1[mcg_size1].cud = mc_member_cud_get(group_ptr);
                        ++mcg_size1;
                    }
                    BCMDNX_IF_ERR_EXIT(multicast_arad_test_get_next(unit, group_ptr, &group_ptr));
                }
            }
            /* read MC group from mcdb into mcg_buf2 */
            mcg_size2 = 0;
            for (j = QAX_MCDS_GET_EGRESS_GROUP_START(mcds, i) ; j != DPP_MC_EGRESS_LINK_PTR_END;) {
                soc_mem_t mem = 0; /*will be set in Jericho Egress formats whenever a format with LinkPTR is in use*/
                uint32* used_bitmap_ptr = used_bitmap + (j / 32);
                uint32 used_bitmap_mask = 1 << (j % 32);
#ifndef _ARAD_MC_NO_IMPLEMENTATION_SPECIFIC_TESTS
                qax_mcdb_entry_t *entry = QAX_GET_MCDB_ENTRY(mcds, j);
#endif
                TEST_ASSERT(j <= MCDS_INGRESS_LINK_END(mcds));
                if (used_bitmap_mask & *used_bitmap_ptr) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BCM_MSG("MCDB entry 0x%lx for egress group 0x%lx is used by a different group"),(unsigned long)j, (unsigned long)i));
                }
                *used_bitmap_ptr |= used_bitmap_mask;
                ++used_entries;
#ifndef _ARAD_MC_NO_IMPLEMENTATION_SPECIFIC_TESTS
                {
                    TEST_ASSERT(QAX_MCDS_ENTRY_GET_TEST_BIT(entry));
                    QAX_MCDS_ENTRY_SET_TEST_BIT_OFF(entry);
                    ++group_entries2;
                }
#endif
                {
                      /* QAX Egress multicast formats */
                      uint32 tm_port;
                      uint32 format_type = soc_mem_field32_get(unit, TAR_MCDB_EGRESS_TWO_COPIES_FORMATm, entry, ENTRY_FORMATf);
                      uint32 cud = soc_mem_field32_get(unit, TAR_MCDB_EGRESS_TWO_COPIES_FORMATm, entry, OUTLIF_Af);
                      next_entry = DPP_MC_EGRESS_LINK_PTR_END;
                      switch (format_type) { /* select memory format based on the format type */
                        case 0: /*TAR_MCDB_EGRESS_BITMAP_POINTER_FORMAT*/
                          {
                              const uint32 bitmap = soc_mem_field32_get(unit, (mem = TAR_MCDB_EGRESS_BITMAP_POINTER_FORMATm), entry, BMP_PTRf);
                              if (bitmap != DPP_MC_EGR_BITMAP_DISABLED) { /* IRR_MCDB_EGRESS_FORMAT_2 - replication not disabled for this entry */
                                  BCMDNX_IF_ERR_EXIT(test_bcm_multicast_add_to_array(unit, mcg_buf2, &mcg_size2, bitmap + MC_MEMBER_DEST_BITMAP_OFFSET, cud));
                              }
                          }
                          break;

                        case 1: /*TAR_MCDB_EGRESS_TWO_COPIES_FORMAT*/
                          {
                              tm_port = soc_mem_field32_get(unit, (mem = TAR_MCDB_EGRESS_TWO_COPIES_FORMATm), entry, PP_DSP_Af);
                              BCMDNX_IF_ERR_EXIT(soc_port_sw_db_tm_to_local_port_get(unit, core, tm_port, &local_port));
                              if (tm_port != DPP_MULT_EGRESS_PORT_INVALID) { /* replication not disabled for this entry part */
                                  BCMDNX_IF_ERR_EXIT(test_bcm_multicast_add_to_array(unit, mcg_buf2, &mcg_size2, local_port, cud));
                              }
                              tm_port = soc_mem_field32_get(unit, TAR_MCDB_EGRESS_TWO_COPIES_FORMATm, entry, PP_DSP_Bf);
                              BCMDNX_IF_ERR_EXIT(soc_port_sw_db_tm_to_local_port_get(unit, core, tm_port, &local_port));
                              cud = soc_mem_field32_get(unit, TAR_MCDB_EGRESS_TWO_COPIES_FORMATm, entry, OUTLIF_Bf);
                              if (tm_port != DPP_MULT_EGRESS_PORT_INVALID) { /* replication not disabled for this entry part */
                                  BCMDNX_IF_ERR_EXIT(test_bcm_multicast_add_to_array(unit, mcg_buf2, &mcg_size2, local_port, cud));
                              }
                          }
                          break;

                        case 2: /*TAR_MCDB_EGRESS_DOUBLE_CUD_FORMAT*/
                          {
                            tm_port = soc_mem_field32_get(unit, (mem = TAR_MCDB_EGRESS_DOUBLE_CUD_FORMATm), entry, PP_DSP_1f);
                            BCMDNX_IF_ERR_EXIT(soc_port_sw_db_tm_to_local_port_get(unit, core, tm_port, &local_port));
                            cud = soc_mem_field32_get(unit, TAR_MCDB_EGRESS_TWO_COPIES_FORMATm, entry, OUTLIF_Af);
                            if (tm_port != DPP_MULT_EGRESS_PORT_INVALID) { /* replication not disabled for this entry part */
                                BCMDNX_IF_ERR_EXIT(test_bcm_multicast_add_to_array(unit, mcg_buf2, &mcg_size2, local_port, cud));
                            }
                          }
                          break;

                        case 5: /*TAR_MCDB_EGRESS_TDM_FORMATm*/
                          {
                            cud = soc_mem_field32_get(unit, TAR_MCDB_EGRESS_TDM_FORMATm, entry, OUTLIFf);
                            tm_port = soc_mem_field32_get(unit, TAR_MCDB_EGRESS_TDM_FORMATm, entry, PP_DSP_1f); /* set the four destination ports */
                            if (tm_port != DPP_MULT_EGRESS_PORT_INVALID) { /* replication not disabled for this entry part */
                                BCMDNX_IF_ERR_EXIT(test_bcm_multicast_add_to_array(unit, mcg_buf2, &mcg_size2, tm_port, cud));
                            }
                            tm_port = soc_mem_field32_get(unit, TAR_MCDB_EGRESS_TDM_FORMATm, entry, PP_DSP_2f);
                            if (tm_port != DPP_MULT_EGRESS_PORT_INVALID) { /* replication not disabled for this entry part */
                                BCMDNX_IF_ERR_EXIT(test_bcm_multicast_add_to_array(unit, mcg_buf2, &mcg_size2, tm_port, cud));
                            }
                            tm_port = soc_mem_field32_get(unit, TAR_MCDB_EGRESS_TDM_FORMATm, entry, PP_DSP_3f);
                            if (tm_port != DPP_MULT_EGRESS_PORT_INVALID) { /* replication not disabled for this entry part */
                                BCMDNX_IF_ERR_EXIT(test_bcm_multicast_add_to_array(unit, mcg_buf2, &mcg_size2, tm_port, cud));
                            }
                            tm_port = soc_mem_field32_get(unit, TAR_MCDB_EGRESS_TDM_FORMATm, entry, PP_DSP_4f);
                            if (tm_port != DPP_MULT_EGRESS_PORT_INVALID) { /* replication not disabled for this entry part */
                                BCMDNX_IF_ERR_EXIT(test_bcm_multicast_add_to_array(unit, mcg_buf2, &mcg_size2, tm_port, cud));
                            }
                            next_entry = soc_mem_field32_get(unit, TAR_MCDB_EGRESS_TDM_FORMATm, entry, LINK_PTRf);
                          }
                          break;

                        default:
                          BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BCM_MSG("unsupported format in MCTB entry 0x%lx | format : %d "),(unsigned long)j , format_type));
                      } /* end of the switch */
                      if (mem) {
                          next_entry = soc_mem_field32_get(unit, mem, entry, LINK_PTRf);
                      }
                      j = next_entry;
                }
                TEST_ASSERT(j <= MCDS_INGRESS_LINK_END(mcds));
            }
            rep_entries += mcg_size2 ? mcg_size2 : 1;
            /* compare the group output of the BCM API to the test data structure using the compare function */
            BCMDNX_IF_ERR_EXIT(test_bcm_multicast_compare_group_member_data(unit, mcg_buf1, mcg_size1, mcg_buf2, mcg_size2, i , &nof_errors));
        }
    }
    

    free_entries -= used_entries;
    if (!(used_bitmap[DPP_MC_EGRESS_LINK_PTR_END / 32] & (1 << DPP_MC_EGRESS_LINK_PTR_END % 32))) {
        ++free_edge_entries;
    }
    TEST_ASSERT(free_entries >= free_edge_entries && used_entries <= QAX_LAST_MCDB_ENTRY(unit)+1);
#ifndef _ARAD_MC_NO_IMPLEMENTATION_SPECIFIC_TESTS
    {
        TEST_ASSERT(free_entries - nof_egr_bitmap_entries - free_edge_entries == qax_mcds_unoccupied_get(mcds)); /* This test is implementation defined */
        TEST_ASSERT(!qax_mcds_test_free_entries(unit));
        TEST_ASSERT(group_entries1 == group_entries2 && group_entries1 <= QAX_LAST_MCDB_ENTRY(unit) + 1);
        TEST_ASSERT(group_entries1 == QAX_LAST_MCDB_ENTRY(unit) + 1 - free_entries);
    }
#endif

    TEST_PRINT((_BCM_MSG("verified the whole MCDB successfully, %lu replications and %lu free entries found"), (unsigned long)rep_entries, (unsigned long)(free_entries - free_edge_entries)));
exit:
    BCM_FREE(alloced_mem);
    if (alloced_pci_mem) {
        soc_cm_sfree(unit, alloced_pci_mem);
    }
    BCMDNX_FUNC_RETURN;
}
#endif

/* compare the whole chip MC+vlan tables to the test data structures */
int test_bcm_multicast_verify_all(int unit)
{
    uint32* alloced_mem = NULL; /* regular memory allocated */
    uint32* alloced_pci_mem = NULL; /* memory allocated for DMA access */
    uint32* mcdb; /* IRR_MCDB read from chip */
    uint32* irdb; /* IDR_IRDB read from chip */
    uint32* vlan_table; /* EGQ_VLAN_TABLE read from chip */
    uint32* used_bitmap; /* bitmap to mark if an MCDB entry was already used for a verified group */
    mc_member_data *mcg_buf1, *mcg_buf2; /* buffers for group sorting & comparison*/
    unsigned mcg_size1, mcg_size2; /* the sizes of mcg_buf1, mcg_buf2 */
    unsigned /*max_ingr_reps_in_mini,*/ max_ingr_reps;
    const mc_member* group_ptr;
    uint32 temp, used_entries = 0, rep_entries = 0; /* rep_entries will hold number of replications (bitmap counts a one) + number of open empty groups */
#ifndef _ARAD_MC_NO_IMPLEMENTATION_SPECIFIC_TESTS
    uint32 group_entries1 = 0, group_entries2 = 0;
    dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
#endif
    int32 free_entries =  DPP_LAST_MCDB_ENTRY(mcds) + 1, free_edge_entries = 0;
    int32 core;
    SOC_TMC_MULT_ID i, j, next_entry;
    unsigned nof_uint32s, nof_errors = 0;
    unsigned is_open;
    mc_test_data_t *data = test_data[unit];
    BCMDNX_INIT_FUNC_DEFS;


#ifdef BCM_88470_A0
    if (SOC_IS_QAX(unit)) {
        return test_bcm_qax_multicast_verify_all(unit);
    }
#endif

    TEST_ASSERT(multicast_is_initialized(unit));
    TEST_PRINT((_BCM_MSG("starting to verify MCDB")));

    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      switch (SOC_DPP_CONFIG(unit)->arad->init.dram.fmc_dbuff_mode) {
        case ARAD_INIT_FMC_4K_REP_64K_DBUFF_MODE:
          max_ingr_reps = 4096 - 2;
          break;
        case ARAD_INIT_FMC_64_REP_128K_DBUFF_MODE:
          max_ingr_reps = 64 - 2;
          break;
        default:
          SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("invalid buffer mode")));
      }
/*      max_ingr_reps_in_mini = 1;*/
    } else {
        switch (SOC_DPP_CONFIG(unit)->arad->init.dram.fmc_dbuff_mode) {
        case JERICHO_INIT_FMC_64_REP_512K_DBUFF_MODE:
          max_ingr_reps = 64 - 2;
          break;
        default: /*for JERICHO_INIT_FMC_4K_REP_256K_DBUFF_MODE and JERICHO_INIT_FMC_NO_REP_DBUFF_MODE modes, should test for invalid mode?*/
          max_ingr_reps = 4096 - 2;
          break;
      }
/*      max_ingr_reps_in_mini = 8 - 2;*/
    }

    BCMDNX_IF_ERR_EXIT(READ_EGQ_EGRESS_REPLICATION_GENERAL_CONFIGr_REG32(unit, REG_PORT_ANY, &temp));
    /* verify that data->nof_vlan_egress_mc_groups matches the chip value */

    TEST_ASSERT(data->ingress_mc_groups);
    TEST_ASSERT(data->egress_mc_groups);
    {
        TEST_ASSERT(data->nof_vlan_egress_mc_groups == soc_reg_field_get(unit, EGQ_EGRESS_REPLICATION_GENERAL_CONFIGr,
                    temp, EGRESS_REP_BITMAP_GROUP_VALUE_TOPf) + 1);
    }

    TEST_ASSERT(!(SOC_DPP_CONFIG(unit)->tm.nof_mc_ids % 32));
    
    /* allocate the buffers for this function in one allocation */
    if (soc_mem_dmaable(unit, IRR_MCDBm, SOC_MEM_BLOCK_ANY(unit, IRR_MCDBm))) { /* use DMA, separate allocation for memory to be filled by DMA */
        nof_uint32s = (MCDS_INGRESS_LINK_END(mcds) + 1) * ARAD_MC_TABLE_ENTRY_SIZE + /* MCDB */
                      ARAD_IRDB_NOF_ENTRIES * ARAD_IRDB_ENTRY_SIZE * (SOC_IS_JERICHO(unit) ? 2 : 1) + /* IDR_IRDB - Jericho has x2 entries */
                      ((ARAD_MC_VLAN_BITMAP_ENTRY_SIZE * data->nof_egress_bitmaps * (SOC_IS_JERICHO(unit) ? 2 : 1) /* VLAN table - nof_egress_bitmaps already initialized to include both cores*/
                      + 1) / 2 * 2); /* add padding to 8 byte boundary */
        alloced_pci_mem = soc_cm_salloc(unit, 4 * nof_uint32s, "mc_verify_dma");
        if (alloced_pci_mem == NULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BCM_MSG("Failed allocating dma memory for test_bcm_multicast_verify_all")));
        }

        nof_uint32s = ((MCDS_INGRESS_LINK_END(mcds) + 1) + 31) / 32;

        BCMDNX_ALLOC(alloced_mem, 4 * nof_uint32s + 
                     ARAD_MC_MAX_GROUP_REPLICATIONS_ANY * 2 * sizeof(mc_member_data), "mc_verify_all");
        if (!alloced_mem) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BCM_MSG("Failed allocating test memory for test_bcm_multicast_verify_all")));
        }
        mcdb = alloced_pci_mem;
        irdb = mcdb + (MCDS_INGRESS_LINK_END(mcds) + 1) * ARAD_MC_TABLE_ENTRY_SIZE;
        vlan_table = irdb + ARAD_IRDB_NOF_ENTRIES * ARAD_IRDB_ENTRY_SIZE * (SOC_IS_JERICHO(unit) ? 2 : 1);
        used_bitmap = alloced_mem;
        mcg_buf1 = (mc_member_data*)(alloced_mem + nof_uint32s);
        mcg_buf2 = mcg_buf1 + ARAD_MC_MAX_GROUP_REPLICATIONS_ANY;
    } else { /* do not use DMA, everything in the same memory allocation */
        nof_uint32s = (MCDS_INGRESS_LINK_END(mcds) + 1) * ARAD_MC_TABLE_ENTRY_SIZE + /* MCDB */
                      ARAD_IRDB_NOF_ENTRIES * ARAD_IRDB_ENTRY_SIZE * (SOC_IS_JERICHO(unit) ? 2 : 1) + /* IDR_IRDB */
                      ((ARAD_MC_VLAN_BITMAP_ENTRY_SIZE * data->nof_egress_bitmaps * (SOC_IS_JERICHO(unit) ? 2 : 1) + /* VLAN table */
                      ((SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids + SOC_DPP_CONFIG(unit)->tm.nof_mc_ids * SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores + 31) / 32)
                      + 1) / 2 * 2); /* add padding to 8 byte boundary */
        BCMDNX_ALLOC(alloced_mem, 4 * nof_uint32s + ARAD_MC_MAX_GROUP_REPLICATIONS_ANY * 2 * sizeof(mc_member_data), "mc_verify_all");
        if (!alloced_mem) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BCM_MSG("Failed allocating test memory for test_bcm_multicast_verify_all")));
        }
        mcdb = alloced_mem;
        irdb = mcdb + (MCDS_INGRESS_LINK_END(mcds) + 1) * ARAD_MC_TABLE_ENTRY_SIZE;
        vlan_table = irdb + ARAD_IRDB_NOF_ENTRIES * ARAD_IRDB_ENTRY_SIZE * (SOC_IS_JERICHO(unit) ? 2 : 1);
        used_bitmap = vlan_table + ARAD_MC_VLAN_BITMAP_ENTRY_SIZE * data->nof_egress_bitmaps * (SOC_IS_JERICHO(unit) ? 2 : 1);
        mcg_buf1 = (mc_member_data*)(mcdb + nof_uint32s);
        TEST_ASSERT(!(((((uint32*)mcg_buf1) - used_bitmap) - SOC_DPP_CONFIG(unit)->tm.nof_mc_ids / 32 ) & ~1));
        mcg_buf2 = mcg_buf1 + ARAD_MC_MAX_GROUP_REPLICATIONS_ANY;
    }
    
    for (i = (((MCDS_INGRESS_LINK_END(mcds) + 1) + 31) / 32); i;) { /* init used_ingr_bitmap to 0 */
        used_bitmap[--i] = 0;
    }

    /* read the multicast database table, IRDB, and the vlan bitmap table from the chip */
    BCMDNX_IF_ERR_EXIT(soc_mem_read_range(unit, IRR_MCDBm, MEM_BLOCK_ANY, 0, MCDS_INGRESS_LINK_END(mcds), mcdb));
    BCMDNX_IF_ERR_EXIT(soc_mem_read_range(unit, IDR_IRDBm, MEM_BLOCK_ANY, 0, ARAD_IRDB_NOF_ENTRIES * (SOC_IS_JERICHO(unit) ? 2 : 1) - 1, irdb));
    if (data->nof_egress_bitmaps){
        BCMDNX_IF_ERR_EXIT(soc_mem_read_range(unit, EGQ_VLAN_TABLEm, EGQ_BLOCK(unit, 0), 0, data->nof_egress_bitmaps - 1, vlan_table));
        if (SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores > 1) {
            BCMDNX_IF_ERR_EXIT(soc_mem_read_range(unit, EGQ_VLAN_TABLEm, EGQ_BLOCK(unit, 1), 0, data->nof_egress_bitmaps - 1, vlan_table+data->nof_egress_bitmaps));
        }
    }

#ifndef _ARAD_MC_NO_IMPLEMENTATION_SPECIFIC_TESTS
    {
        arad_mcdb_entry_t *entry = MCDS_GET_MCDB_ENTRY(mcds, 0);
        uint32* mcdb_ptr = mcdb;
        /* mark all free ingress entries */
        for (i = SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids; i; --i, ++entry, mcdb_ptr += ARAD_MC_TABLE_ENTRY_SIZE) { 
            TEST_ASSERT(!DPP_MCDS_ENTRY_GET_TEST_BIT(entry));
            if (DPP_MCDS_TYPE_IS_FREE(DPP_MCDS_ENTRY_GET_TYPE(entry))) {
                if (!(mcdb_ptr[0] == mcds->free_value[0] && !((mcdb_ptr[1] ^ mcds->free_value[1]) & ARAD_MC_ENTRY_MASK_VAL))) {
                    bsl_printf ("i=%d read:%x %x  entry:%x %x free_value:%x %x\n", i, mcdb_ptr[0], mcdb_ptr[1], entry->word0, entry->word1, mcds->free_value[0],  mcds->free_value[1]);}
                TEST_ASSERT(mcdb_ptr[0] == mcds->free_value[0] && (mcdb_ptr[1] & ARAD_MC_ENTRY_MASK_VAL) == mcds->free_value[1]);
            } else {
                TEST_ASSERT(mcdb_ptr[0] == entry->word0 && !((mcdb_ptr[1] ^ entry->word1) & ARAD_MC_ENTRY_MASK_VAL));
                ++group_entries1;
                DPP_MCDS_ENTRY_SET_TEST_BIT_ON(entry);
            }
        }
        /* mark all free egress entries */
        for (i = MCDS_INGRESS_LINK_END(mcds) + 1 - SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids; i; --i, ++entry, mcdb_ptr += ARAD_MC_TABLE_ENTRY_SIZE) { 
            TEST_ASSERT(!DPP_MCDS_ENTRY_GET_TEST_BIT(entry));
            if (DPP_MCDS_TYPE_IS_FREE(DPP_MCDS_ENTRY_GET_TYPE(entry))) {
                TEST_ASSERT(mcdb_ptr[0] == mcds->free_value[0] && !((mcdb_ptr[1] ^ mcds->free_value[1]) & ARAD_MC_ENTRY_MASK_VAL));
            } else {
                TEST_ASSERT(mcdb_ptr[0] == entry->word0 && !((mcdb_ptr[1] ^ entry->word1) & ARAD_MC_ENTRY_MASK_VAL));
                ++group_entries1;
                DPP_MCDS_ENTRY_SET_TEST_BIT_ON(entry);
            }
        }
    }
#endif

    /* verify ingress MC groups */
    irdb -=2;
    if (data->ingress_mc_groups) {
        for (i = 0; i < SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids; ++i) {
            group_ptr = data->ingress_mc_groups + i;
            is_open = mc_member_is_ingress_get(group_ptr); /* group marked open in data structure */
            /* Check that group open state is the same in BCM and in test data structure */
            BCMDNX_IF_ERR_EXIT(test_bcm_multicast_check_group_open_by_bcm_api(unit, i, FALSE, is_open));

            if (is_open) { /* if group is open compare its members */
                /* read MC group from group_ptr into mcg_buf1 */
                mcg_size1 = 0;
                if (mc_member_is_used_get(group_ptr)) {
                    for (; group_ptr;) {
                        TEST_ASSERT(mc_member_is_used_get(group_ptr));
                        TEST_ASSERT(mc_member_is_ingress_get(group_ptr));
                        if (mc_member_is_used_get(group_ptr)) {
                            if (mcg_size1 == ARAD_MC_MAX_GROUP_REPLICATIONS_INGRESS) {
                                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BCM_MSG("ingress group 0x%lx data structure contains more than the maximum number of members"),(unsigned long)i));
                            }
                            mcg_buf1[mcg_size1].dest = mc_member_dest_get(group_ptr);
                            mcg_buf1[mcg_size1].cud = mc_member_cud_get(group_ptr);
                            ++mcg_size1;
                        }
                        BCMDNX_IF_ERR_EXIT(multicast_arad_test_get_next(unit, group_ptr, &group_ptr));
                    }
                }

                /* read MC group from mcdb into mcg_buf2 */
                mcg_size2 = 0;
                for (j = i; j != MCDS_INGRESS_LINK_END(mcds);) {
                    uint32* mcdb_ptr = mcdb + j * ARAD_MC_TABLE_ENTRY_SIZE;
                    uint32* used_bitmap_ptr = used_bitmap + (j / 32);
                    uint32 used_bitmap_mask = 1 << (j % 32);
                    uint32 out_lif = soc_mem_field32_get(unit, IRR_MCDBm, mcdb_ptr, OUT_LIFf);
                    uint32 destination = soc_mem_field32_get(unit, IRR_MCDBm, mcdb_ptr, DESTINATIONf);
#ifndef _ARAD_MC_NO_IMPLEMENTATION_SPECIFIC_TESTS
                    arad_mcdb_entry_t *entry = MCDS_GET_MCDB_ENTRY(mcds, j);
#endif

                    TEST_ASSERT(j < MCDS_INGRESS_LINK_END(mcds));
                    if (used_bitmap_mask & *used_bitmap_ptr) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BCM_MSG("MCDB entry 0x%lx for ingress group 0x%lx is used by a different group"),(unsigned long)j, (unsigned long)i));
                    }
                    *used_bitmap_ptr |= used_bitmap_mask;
                    ++used_entries;
#ifndef _ARAD_MC_NO_IMPLEMENTATION_SPECIFIC_TESTS
                    {
                        TEST_ASSERT(DPP_MCDS_ENTRY_GET_TEST_BIT(entry));
                        DPP_MCDS_ENTRY_SET_TEST_BIT_OFF(entry);
                        ++group_entries2;
                    }
#endif

                    if (destination != ARAD_MULT_ING_UNOCCUPIED_BASE_QUEUE_INDICATION) { /* occupied entry, fill entry in array */
                        if (mcg_size2 == ARAD_MC_MAX_GROUP_REPLICATIONS_INGRESS) {
                            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BCM_MSG("MCDB ingress group 0x%lx contains more than the maximum number of members"),(unsigned long)i));
                        }
                        switch (SOC_DPP_CONFIG(unit)->tm.mc_ing_encoding_mode) {
                            case 0:
                                mcg_buf2[mcg_size2].dest = destination;
                                mcg_buf2[mcg_size2].cud = out_lif;
                                break;
                            case 1:
                                mcg_buf2[mcg_size2].dest = destination & ~(1 << 16);
                                mcg_buf2[mcg_size2].cud = out_lif | (destination & (1 << 16));
                                break;
                            case 2:
                                if (destination & (1 << 17)) {
                                    mcg_buf2[mcg_size2].dest = destination & 0x27fff;
                                    mcg_buf2[mcg_size2].cud = out_lif | ((destination & (3 << 15)) << 1);
                                } else {
                                    mcg_buf2[mcg_size2].dest = destination & 0x0bfff;
                                    mcg_buf2[mcg_size2].cud = out_lif | ((destination & (1 << 16)) << 1) | ((destination & (1 << 14)) << 2);
                                }
                                break;
                            case 3:
                                mcg_buf2[mcg_size2].dest = destination | (1 << 17);
                                mcg_buf2[mcg_size2].cud = out_lif | ((destination & (1 <<17)) >> 1);
                                break;
                            default:
                                BCMDNX_IF_ERR_EXIT(BCM_E_INTERNAL);
                        }
                        ++mcg_size2;
                    }
                    j = soc_mem_field32_get(unit, IRR_MCDBm, mcdb_ptr, LINK_PTRf);
                    TEST_ASSERT(j <= MCDS_INGRESS_LINK_END(mcds));
                }
                rep_entries += mcg_size2 ? mcg_size2 : 1;
                TEST_ASSERT(mcg_size2 <= max_ingr_reps);

                /* compare the group output of the BCM API to the test data structure using the compare function */
                BCMDNX_IF_ERR_EXIT(test_bcm_multicast_compare_group_member_data(unit, mcg_buf1, mcg_size1, mcg_buf2, mcg_size2, i , &nof_errors));
                /* expected value in IRDB - FMC allways used */
                mcg_size1 = 1; /* mcg_size2 > max_ingr_reps_in_mini ? 1 : 3;*/
            } else {
                mcg_size1 = 0; /* expected value in IDR_IRDB */
            }

            {
                unsigned irdb_bit_offset = 2 * (i % 16); /* bit offset in IDR_IRDB entry */
                if (!irdb_bit_offset) { /* read the IDR_IRDB entry the first time it is used */
                    irdb +=2;
                }
                TEST_ASSERT((((*irdb) >> irdb_bit_offset) & 3) == mcg_size1); /* verify the correct IRDB fields for the current group */
            }
        }
    }

    SOC_DPP_CORES_ITER(SOC_DPP_CORE_ALL(unit), core){
        /* verify egress VLAN bitmap MC groups */
        for (i = 0; i < data->nof_vlan_egress_mc_groups; ++i) { /* first test VLAN bitmap egress Mc groups */
            uint32 entry[UINT32SPER_VLAN];
            uint32* vlan_bits = data->vlan_bitmaps + ((i + core * data->nof_vlan_egress_mc_groups) * UINT32SPER_VLAN);
            uint32 entry_mem[ARAD_MC_VLAN_BITMAP_ENTRY_SIZE];
            SOCDNX_IF_ERR_EXIT(READ_EGQ_VLAN_TABLEm(unit, EGQ_BLOCK(unit, core), i, entry_mem));
            soc_mem_field_get(unit,EGQ_VLAN_TABLEm, entry_mem, VLAN_MEMBERSHIPf, entry);


            group_ptr = data->egress_mc_groups + i + (core * SOC_DPP_CONFIG(unit)->tm.nof_mc_ids);
            is_open = mc_member_is_ingress_get(group_ptr); /* group marked open in data structure */
            /* Check that group open state is the same in BCM and in test data structure */
            BCMDNX_IF_ERR_EXIT(test_bcm_multicast_check_group_open_by_bcm_api(unit, i, TRUE, is_open));
            for (j = 0;  j < UINT32SPER_VLAN; ++j) { /* loop over bitmap port words comparing the bitmap */
                if (vlan_bits[j] != entry[j]) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BCM_MSG("MC test found different bits in VLAN egress MC group 0x%lx at index %d hardware:0x%lx expected:0x%lx"), (unsigned long)i, j, (unsigned long)entry[j], (unsigned long)vlan_bits[j]));
                }
            }
        }
    

        /* verify egress regular MC groups */
        for (; i < SOC_DPP_CONFIG(unit)->tm.nof_mc_ids; ++i) {
            group_ptr = data->egress_mc_groups + core * SOC_DPP_CONFIG(unit)->tm.nof_mc_ids + i;
            is_open = mc_member_is_ingress_get(group_ptr); /* group marked open in data structure */
            /* Check that group open state is the same in BCM and in test data structure */
            BCMDNX_IF_ERR_EXIT(test_bcm_multicast_check_group_open_by_bcm_api(unit, i, TRUE, is_open));

            if (is_open) { /* if group is open compare its members */
                /* read MC group from group_ptr into mcg_buf1 */
                mcg_size1 = 0;
                if (mc_member_is_used_get(group_ptr)) {
                    for (; group_ptr;) {
                        TEST_ASSERT(mc_member_is_used_get(group_ptr));
                        TEST_ASSERT(mc_member_is_ingress_get(group_ptr));
                        if (mc_member_is_used_get(group_ptr)) {
                            if (mcg_size1 == ARAD_MC_MAX_GROUP_REPLICATIONS_EGRESS) {
                                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BCM_MSG("egress group 0x%lx data structure contains more than the maximum number of members"),(unsigned long)i));
                            }
                            mcg_buf1[mcg_size1].dest = mc_member_dest_get(group_ptr);
                            mcg_buf1[mcg_size1].cud = mc_member_cud_get(group_ptr);
                            ++mcg_size1;
                        }
                        BCMDNX_IF_ERR_EXIT(multicast_arad_test_get_next(unit, group_ptr, &group_ptr));
                    }
                }
                /* read MC group from mcdb into mcg_buf2 */
                mcg_size2 = 0;
                for (j = DPP_MCDS_GET_EGRESS_GROUP_START(mcds, i, core) ; j != DPP_MC_EGRESS_LINK_PTR_END;) {
                    soc_mem_t mem = 0; /*will be set in Jericho Egress formats whenever a format with LinkPTR is in use*/
                    uint32* mcdb_ptr = mcdb + (j * ARAD_MC_TABLE_ENTRY_SIZE);
                    uint32* used_bitmap_ptr = used_bitmap + (j / 32);
                    uint32 used_bitmap_mask = 1 << (j % 32);
                    uint32 cud_f;
#ifndef _ARAD_MC_NO_IMPLEMENTATION_SPECIFIC_TESTS
                    arad_mcdb_entry_t *entry = MCDS_GET_MCDB_ENTRY(mcds, j);
#endif
                    TEST_ASSERT(j <= MCDS_INGRESS_LINK_END(mcds));
                    if (used_bitmap_mask & *used_bitmap_ptr) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BCM_MSG("MCDB entry 0x%lx for egress group 0x%lx is used by a different group"),(unsigned long)j, (unsigned long)i));
                    }
                    *used_bitmap_ptr |= used_bitmap_mask;
                    ++used_entries;
#ifndef _ARAD_MC_NO_IMPLEMENTATION_SPECIFIC_TESTS

                    {
                        TEST_ASSERT(DPP_MCDS_ENTRY_GET_TEST_BIT(entry));
                        DPP_MCDS_ENTRY_SET_TEST_BIT_OFF(entry);
                        ++group_entries2;
                    }
#endif
                    if (SOC_IS_JERICHO(unit)) { /*Jericho Egress multicast formats*/
                        uint32 format_type = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_1m, entry, ENTRY_FORMATf);
                        uint32 cud = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_1m, entry, OUTLIF_1f);
                        next_entry = DPP_MC_EGRESS_LINK_PTR_END;
                        switch (format_type) { /* select memory format based on the format type */
                          case 0: /*IRR_MCDB_EGRESS_FORMAT_1*/
                            {
                                uint32 port = soc_mem_field32_get(unit, (mem = IRR_MCDB_EGRESS_FORMAT_1m), entry, PP_DSP_1Af);
                                if (port != DPP_MULT_EGRESS_PORT_INVALID) { /* replication not disabled for this entry part */
                                    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_add_to_array(unit, mcg_buf2, &mcg_size2, port, cud));
                                }
                                port = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_1m, entry, PP_DSP_1Bf);
                                if (port != DPP_MULT_EGRESS_PORT_INVALID) { /* replication not disabled for this entry part */
                                    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_add_to_array(unit, mcg_buf2, &mcg_size2, port, cud));
                                }
                            }
                            break;

                          case 1: /*IRR_MCDB_EGRESS_FORMAT_2*/
                            {
                                const uint32 bitmap = soc_mem_field32_get(unit, (mem = IRR_MCDB_EGRESS_FORMAT_2m), entry, BMP_PTRf);
                                if (soc_mem_field32_get(unit, mem, entry, NEW_FORMATf)) { /*IRR_MCDB_EGRESS_FORMAT_8 or IRR_MCDB_EGRESS_FORMAT_9*/
                                    DPP_MC_ASSERT(0); /* PP formats not yet supported */
                                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("unhandled case")));
                                } else if (bitmap != DPP_MC_EGR_BITMAP_DISABLED) { /* IRR_MCDB_EGRESS_FORMAT_2 - replication not disabled for this entry */
                                    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_add_to_array(unit, mcg_buf2, &mcg_size2, bitmap + MC_MEMBER_DEST_BITMAP_OFFSET, cud));
                                }
                            }
                            break;

                          case 3: /*IRR_MCDB_EGRESS_FORMAT_5*/
                            next_entry = j + 1; /*difference between format 5 and 4 is that 5's pointer is incremented, and 4's is the lists end*/
                          case 2: /*IRR_MCDB_EGRESS_FORMAT_4*/
                            {
                                uint32 port = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_4m, entry, PP_DSP_1f);
                                if (port != DPP_MULT_EGRESS_PORT_INVALID) { /* replication not disabled for this entry part */
                                    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_add_to_array(unit, mcg_buf2, &mcg_size2, port, cud));
                                }
                                port = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_4m, entry, PP_DSP_2f);
                                cud = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_4m, entry, OUTLIF_2f);
                                if (port != DPP_MULT_EGRESS_PORT_INVALID) { /* replication not disabled for this entry part */
                                    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_add_to_array(unit, mcg_buf2, &mcg_size2, port, cud));
                                }
                            }
                            break;

                          default:
                            if (format_type < 8) { /* IRR_MCDB_EGRESS_FORMAT_3 - two CUDs and a link pointer */

                                if (cud != DPP_MC_EGR_OUTLIF_DISABLED) { /* replication not disabled for this entry part */
                                    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_add_to_array(unit, mcg_buf2, &mcg_size2, MC_MEMBER_DEST_OUTLIF_ONLY, cud));
                                }
                                cud = soc_mem_field32_get(unit, (mem = IRR_MCDB_EGRESS_FORMAT_3m), entry, OUTLIF_2f);
                                if (cud != DPP_MC_EGR_OUTLIF_DISABLED) { /* replication not disabled for this entry part */
                                    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_add_to_array(unit, mcg_buf2, &mcg_size2, MC_MEMBER_DEST_OUTLIF_ONLY, cud));
                                }

                            } else { /* three CUDs, no link pointer */

                                if (format_type & 4) { /*IRR_MCDB_EGRESS_FORMAT_7*/
                                    next_entry = j + 1; 
                                }
                                /*IRR_MCDB_EGRESS_FORMAT_6 + IRR_MCDB_EGRESS_FORMAT_7 - format 6 is lists end, format 7 pointer is implicit to next entry*/
                                if (cud != DPP_MC_EGR_OUTLIF_DISABLED) { /* replication not disabled for this entry part */
                                    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_add_to_array(unit, mcg_buf2, &mcg_size2, MC_MEMBER_DEST_OUTLIF_ONLY, cud));
                                }
                                cud = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_6m, entry, OUTLIF_2f);
                                if (cud != DPP_MC_EGR_OUTLIF_DISABLED) { /* replication not disabled for this entry part */
                                    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_add_to_array(unit, mcg_buf2, &mcg_size2, MC_MEMBER_DEST_OUTLIF_ONLY, cud));
                                }
                                cud = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_6m, entry, OUTLIF_3f);
                                if (cud != DPP_MC_EGR_OUTLIF_DISABLED) { /* replication not disabled for this entry part */
                                    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_add_to_array(unit, mcg_buf2, &mcg_size2, MC_MEMBER_DEST_OUTLIF_ONLY, cud));
                                }
                            }
                        } /* end of the switch */
                        if (mem) {
                            next_entry = soc_mem_field32_get(unit, mem, entry, LINK_PTRf);
                        }
                        j = next_entry;
                    } else { /*Arad Egress multicast formats*/
                        ++j; /* used for entry formats 5,7 */
                        /* switch based on entry format */ 
                        switch (soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_0m, mcdb_ptr, ENTRY_FORMATf)) {
                        case 0:
                            cud_f = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_0m, mcdb_ptr, OUTLIF_1f);
                            if ((temp = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_0m, mcdb_ptr, PP_DSP_1Af)) != 255) {
                                if (SOC_DPP_CONFIG(unit)->tm.mc_mode & DPP_MC_EGR_17B_CUDS_127_PORTS_MODE) {
                                    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_add_to_array(unit, mcg_buf2, &mcg_size2, temp & ~(1 << 7), cud_f | ((temp & (1 << 7)) << 9)));
                                } else {
                                    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_add_to_array(unit, mcg_buf2, &mcg_size2, temp, cud_f));
                                }
                            }
                            if ((temp = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_0m, mcdb_ptr, PP_DSP_1Bf)) != 127) {
                                BCMDNX_IF_ERR_EXIT(test_bcm_multicast_add_to_array(unit, mcg_buf2, &mcg_size2,
                                  temp, cud_f));
                            }
                            j = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_0m, mcdb_ptr, LINK_PTRf);
                            break;

                        case 1:
                            if ((temp = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_1m, mcdb_ptr, BMP_PTRf)) != 0) {
                                cud_f = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_1m, mcdb_ptr, OUTLIF_1f);
                                if (SOC_DPP_CONFIG(unit)->tm.mc_mode & DPP_MC_EGR_17B_CUDS_127_PORTS_MODE) {
                                    cud_f |= (temp & (1 << 14)) << 2;
                                    temp &= ~(1 << 14);
                                }
                                TEST_ASSERT(temp < data->nof_vlan_egress_mc_groups);
                                BCMDNX_IF_ERR_EXIT(test_bcm_multicast_add_to_array(unit, mcg_buf2, &mcg_size2,
                                  temp + MC_MEMBER_DEST_BITMAP_OFFSET, cud_f));
                            }
                            j = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_1m, mcdb_ptr, LINK_PTRf);
                            break;

                        case 2:
                        case 3:
                            TEST_ASSERT((SOC_DPP_CONFIG(unit)->tm.mc_mode & DPP_MC_EGR_17B_CUDS_127_PORTS_MODE) == 0);
                            if ((temp = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_2m, mcdb_ptr, OUTLIF_1f)) != 0) {
                                TEST_ASSERT(temp <= SOC_DPP_CONFIG(unit)->tm.egress_mc_max_cud);
                                BCMDNX_IF_ERR_EXIT(test_bcm_multicast_add_to_array(unit, mcg_buf2, &mcg_size2,
                                  MC_MEMBER_DEST_OUTLIF_ONLY, temp));
                            }
                            if ((temp = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_2m, mcdb_ptr, OUTLIF_2f)) != 0) {
                                TEST_ASSERT(temp <= SOC_DPP_CONFIG(unit)->tm.egress_mc_max_cud);
                                BCMDNX_IF_ERR_EXIT(test_bcm_multicast_add_to_array(unit, mcg_buf2, &mcg_size2,
                              MC_MEMBER_DEST_OUTLIF_ONLY, temp));
                        }
                        j = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_2m, mcdb_ptr, LINK_PTRf);
                        break;

                        case 4:
                            j = DPP_MC_EGRESS_LINK_PTR_END;
                            /* No missing break here. It is Ok.*/
                            /* coverity[fallthrough:FALSE] */
                        case 5:
                            if ((temp = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_4m, mcdb_ptr, PP_DSP_1f)) != 255) {
                                cud_f = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_4m, mcdb_ptr, OUTLIF_1f);
                                if (SOC_DPP_CONFIG(unit)->tm.mc_mode & DPP_MC_EGR_17B_CUDS_127_PORTS_MODE) {
                                    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_add_to_array(unit, mcg_buf2, &mcg_size2, temp & ~(1 << 7), cud_f | ((temp & (1 << 7)) << 9)));
                                } else {
                                    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_add_to_array(unit, mcg_buf2, &mcg_size2, temp, cud_f));
                                }
                            }
                            if ((temp = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_4m, mcdb_ptr, PP_DSP_2f)) != 255) {
                                cud_f = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_4m, mcdb_ptr, OUTLIF_2f);
                                if (SOC_DPP_CONFIG(unit)->tm.mc_mode & DPP_MC_EGR_17B_CUDS_127_PORTS_MODE) {
                                    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_add_to_array(unit, mcg_buf2, &mcg_size2, temp & ~(1 << 7), cud_f | ((temp & (1 << 7)) << 9)));
                                } else {
                                    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_add_to_array(unit, mcg_buf2, &mcg_size2, temp, cud_f));
                                }
                            }
                            break;

                        case 6:
                            j = DPP_MC_EGRESS_LINK_PTR_END;
                            /* No missing break here. It is Ok.*/
                            /* coverity[fallthrough:FALSE] */
                        case 7:
                            TEST_ASSERT((SOC_DPP_CONFIG(unit)->tm.mc_mode & DPP_MC_EGR_17B_CUDS_127_PORTS_MODE) == 0);
                            if ((temp = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_6m, mcdb_ptr, OUTLIF_1f)) != 0) {
                                TEST_ASSERT(temp <= SOC_DPP_CONFIG(unit)->tm.egress_mc_max_cud);
                                BCMDNX_IF_ERR_EXIT(test_bcm_multicast_add_to_array(unit, mcg_buf2, &mcg_size2,
                                  MC_MEMBER_DEST_OUTLIF_ONLY, temp));
                            }
                            if ((temp = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_6m, mcdb_ptr, OUTLIF_2f)) != 0) {
                                TEST_ASSERT(temp <= SOC_DPP_CONFIG(unit)->tm.egress_mc_max_cud);
                                BCMDNX_IF_ERR_EXIT(test_bcm_multicast_add_to_array(unit, mcg_buf2, &mcg_size2,
                                  MC_MEMBER_DEST_OUTLIF_ONLY, temp));
                            }
                            if ((temp = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_6m, mcdb_ptr, OUTLIF_3f)) != 0) {
                                TEST_ASSERT(temp <= SOC_DPP_CONFIG(unit)->tm.egress_mc_max_cud);
                                BCMDNX_IF_ERR_EXIT(test_bcm_multicast_add_to_array(unit, mcg_buf2, &mcg_size2,
                                  MC_MEMBER_DEST_OUTLIF_ONLY, temp));
                            }
                            break;

                        default:
                            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BCM_MSG("illegal format in MCTB entry 0x%lx"),(unsigned long)j));
                        }

                    }
                    TEST_ASSERT(j <= MCDS_INGRESS_LINK_END(mcds));
                }
                rep_entries += mcg_size2 ? mcg_size2 : 1;
                /* compare the group output of the BCM API to the test data structure using the compare function */
                BCMDNX_IF_ERR_EXIT(test_bcm_multicast_compare_group_member_data(unit, mcg_buf1, mcg_size1, mcg_buf2, mcg_size2, i , &nof_errors));
            }
        }
    }

    free_entries -= used_entries;
    if (!(used_bitmap[DPP_MC_EGRESS_LINK_PTR_END / 32] & (1 << DPP_MC_EGRESS_LINK_PTR_END % 32))) {
        ++free_edge_entries;
    }
    if (!(used_bitmap[DPP_LAST_MCDB_ENTRY(mcds) / 32] & (1 << DPP_LAST_MCDB_ENTRY(mcds) % 32))) {
        ++free_edge_entries;
    }
    TEST_ASSERT(free_entries >= free_edge_entries && used_entries <= MCDS_INGRESS_LINK_END(mcds));
#ifndef _ARAD_MC_NO_IMPLEMENTATION_SPECIFIC_TESTS
    {
        TEST_ASSERT(free_entries - free_edge_entries == dpp_mcds_unoccupied_get(mcds)); /* This test is implementation defined */
        TEST_ASSERT(!_dpp_mcds_test_free_entries(unit));
        TEST_ASSERT(group_entries1 == group_entries2 && group_entries1 <= MCDS_INGRESS_LINK_END(mcds) + 1);
        TEST_ASSERT(group_entries1 == MCDS_INGRESS_LINK_END(mcds) + 1 - free_entries);
    }
#endif

    TEST_PRINT((_BCM_MSG("verified the whole MCDB successfully, %lu replications and %lu free entries found"), (unsigned long)rep_entries, (unsigned long)(free_entries - free_edge_entries)));
exit:
    BCM_FREE(alloced_mem);
    if (alloced_pci_mem) {
        soc_cm_sfree(unit, alloced_pci_mem);
    }
    BCMDNX_FUNC_RETURN;
}

typedef 
enum {
  MC_INGRESS,
  MC_EGRESS,
  MC_BOTH
} mc_group_type;

/* open a group for basic test function */
int test_bcm_mc_arad_group_create(int unit, uint32 group_id, mc_group_type group_type, int may_run_out_of_space)
{
    const uint32 flags = BCM_MULTICAST_WITH_ID | (
      group_type == MC_INGRESS ? BCM_MULTICAST_INGRESS_GROUP :
      group_type == MC_EGRESS ? BCM_MULTICAST_EGRESS_GROUP :
      (BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_EGRESS_GROUP));
    bcm_multicast_t group = group_id;
    BCMDNX_INIT_FUNC_DEFS;
    if (group_type == MC_INGRESS) {
        TEST_ASSERT(group_id < SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids); 
    } else if (group_type == MC_EGRESS) {
        TEST_ASSERT(group_id < SOC_DPP_CONFIG(unit)->tm.nof_mc_ids); 
    } else {
        TEST_ASSERT(group_id < mc_min(SOC_DPP_CONFIG(unit)->tm.nof_mc_ids, SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids)); 
    }
    TEST_FULL_BCM_IF_ERR_EXIT(test_bcm_multicast_create(unit, flags, &group, may_run_out_of_space)); 
    TEST_ASSERT(group == group_id);
exit:
    BCMDNX_FUNC_RETURN;
}




/* basic test fucntion */
int test_bcm_mc_arad_basic_open(int unit)
{
    int initialized = 0;
    const bcm_multicast_t i_group = 10;
    const bcm_multicast_t e_group = 5;
    const bcm_multicast_t both_group = 0xfffe;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_init(unit));
    initialized = 1;
    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_verify_all(unit));

    BCMDNX_IF_ERR_EXIT(test_bcm_mc_arad_group_create(unit, i_group, MC_INGRESS, 0));
    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_verify_all(unit));
    BCMDNX_IF_ERR_EXIT(test_bcm_mc_arad_group_create(unit, e_group, MC_EGRESS, 0));
    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_verify_all(unit));
    BCMDNX_IF_ERR_EXIT(test_bcm_mc_arad_group_create(unit, both_group, MC_BOTH, 0));
    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_verify_all(unit));

    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_destroy(unit, i_group));
    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_verify_all(unit));
    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_destroy(unit, e_group));
    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_verify_all(unit));
    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_destroy(unit, both_group));
    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_verify_all(unit));

    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_detach(unit));
    initialized = 0;
exit:
    if (initialized) {
        test_bcm_multicast_detach_dirty(unit);
    }
    BCMDNX_FUNC_RETURN;
}

/* future test fucntion */
int test_bcm_mc_arad_1(int unit)
{
    BCMDNX_INIT_FUNC_DEFS;
    INFO_PRINT((_BCM_MSG("test message=0x%lx"), (unsigned long)1));
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


/* get the (sorted) ids of up to port_num ports, return 0 on success */
int get_n_ports(int unit, int port_num, bcm_port_t *out_ports, int *out_port_num) {
    bcm_port_config_t c;
    bcm_port_t p;

    BCMDNX_INIT_FUNC_DEFS;

    if (!out_ports || !out_port_num || port_num <= 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BCM_MSG("illegal arguments")));
    }
    BCMDNX_IF_ERR_EXIT(bcm_port_config_get(unit, &c));
    *out_port_num = 0;
    BCM_PBMP_ITER(c.all, p) {
        if (p >= 0 && p < 255) {
            out_ports[*out_port_num] = p;
            ++*out_port_num;
            if (*out_port_num >= port_num) { break; }
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Test MC by adding all the ingress and all the egress groups, each with a single replication.
 * The groups are added and deleted in random order.
 */
int test_bcm_mc_arad_basic(int unit)
{
    unsigned i, initialized = 0;
    bcm_gport_t port;
    SOC_TMC_MULT_ID *ids = NULL;
    mc_test_data_t *data;
    BCMDNX_INIT_FUNC_DEFS;


    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_init(unit));
    initialized = 1;
    sal_srand(0x1f73b06f);
    data = test_data[unit];
    BCMDNX_ALLOC(ids, sizeof(SOC_TMC_MULT_ID)*(mc_max(SOC_DPP_CONFIG(unit)->tm.nof_mc_ids, SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids)), "test_bcm_mc_arad_basic.ids");
        
    if (data->ingress_mc_groups) { /* create and set ingress MC groups */
        for (i = 0; i < SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids; ++i) { /* init random group id selection array */ 
            ids[i] = i;
        }
        if (SOC_DPP_CONFIG(unit)->tm.mc_ing_encoding_mode == 3) { /* does not support (system) port destinations) */
            BCM_GPORT_UNICAST_QUEUE_GROUP_SET(port, 0x12345); /* use the same queue for all groups */
        } else {
            BCM_GPORT_LOCAL_SET(port, 1); /* use the same port */
        }
        for (; i;) { /* for each group in random order, add one member with a random CUD */
            unsigned j = sal_rand() % i;
            bcm_if_t encap = sal_rand() % SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_cud;
            TEST_PRINT((_BCM_MSG("adding in-group 0x%lx  %u"), (unsigned long)ids[j], i));
            BCMDNX_IF_ERR_EXIT(test_bcm_mc_arad_group_create(unit, ids[j], MC_INGRESS, 0));
            if (i%2) { /* either add a port or set the group to the port */
                BCMDNX_IF_ERR_EXIT(test_bcm_multicast_ingress_add(unit, ids[j], port, encap, 0));
            } else {
                BCMDNX_IF_ERR_EXIT(test_bcm_multicast_ingress_set(unit, ids[j], 1, &port, &encap, 0));
            }
            ids[j] = ids[--i];
            if (!(i%32768 /*8192*/)) {
                BCMDNX_IF_ERR_EXIT(test_bcm_multicast_verify_all(unit));
            }
        }
    }

    /* create and set egress MC groups */
    for (i = 0; i < SOC_DPP_CONFIG(unit)->tm.nof_mc_ids; ++i) { /* init random group id selection array */
        ids[i] = i;
    }
    BCM_GPORT_LOCAL_SET(port, 0); /* use the same port */
    for (; i;) { /* for each group in random order, add one member with a random CUD */
        unsigned j = sal_rand() % i;
        bcm_if_t encap = sal_rand() % SOC_DPP_CONFIG(unit)->tm.egress_mc_max_cud;
        TEST_PRINT((_BCM_MSG("adding e-group 0x%lx  %u"), (unsigned long)ids[j], i));
        BCMDNX_IF_ERR_EXIT(test_bcm_mc_arad_group_create(unit, ids[j], MC_EGRESS, 0));
        if (ids[j] >= data->nof_vlan_egress_mc_groups) { /* not a vlan bitmap, add port */
        }
        if (i%2) { /* either add a port or set the group to the port */
            BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_add(unit, ids[j], port, encap, 0));
        } else {
            BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_set(unit, ids[j], 1, &port, &encap, 0));
        }
        ids[j] = ids[--i];
        if (!(i%32768)) {
            BCMDNX_IF_ERR_EXIT(test_bcm_multicast_verify_all(unit));
        }
    }

    for (i = 0; i < mc_max(SOC_DPP_CONFIG(unit)->tm.nof_mc_ids, SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids); ++i) { /* init random group id selection array */ 
        ids[i] = i;
    }
    for (; i;) { /* destroy all groups in random order */
        unsigned j = sal_rand() % i;
        TEST_PRINT((_BCM_MSG("destroying group 0x%lx  %u"), (unsigned long)ids[j], i));
        BCMDNX_IF_ERR_EXIT(test_bcm_multicast_destroy(unit, ids[j]));
        ids[j] = ids[--i];
        if (!(i%32768 /*8192*/)) {
            BCMDNX_IF_ERR_EXIT(test_bcm_multicast_verify_all(unit));
        }
    }
    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_detach(unit));
    initialized = 0;

exit:
    BCM_FREE(ids);
    if (initialized) {
        test_bcm_multicast_detach_dirty(unit);
    }
    BCMDNX_FUNC_RETURN;
}

typedef struct {
  int group_size; /* Group size number of replications. Negative value marks not used/end of array of structs. */
  unsigned nof_groups;
  int is_ingress;
  unsigned increase_port_skip; /* after how many replications, to increase the port. */
  uint32 first_port;
  uint32 first_encap;
} test_iter_info; /* information on a test iteration */

/*
 * Test MC by adding ingress and egress groups with many replication.
 * The groups are added and deleted in random order.
 */
#define TEST_MAX_PORTS 20
#define TEST_MIN_PORTS 4
int test_bcm_mc_arad_mid_size(int unit)
{
    unsigned i, j, ids_left, initialized = 0;
    bcm_gport_t tmp_port;
    bcm_if_t tmp_encap;
    SOC_TMC_MULT_ID* ids = NULL;
    SOC_TMC_MULT_ID tmp_id, *remaining_ids;
    bcm_gport_t *replications_port = NULL;
    bcm_if_t *replications_encap = NULL;
    int ports_number, egress_ports_num;
    bcm_port_t ports[TEST_MAX_PORTS];

     test_iter_info iters[] = {
        {   2, 1538, 0,    1, 2, 0x259d}, /* egress */
        {   1,  701, 1,    1, 2, 0xc0f6}, /* ingress */
        {  17,  279, 0,    4, 1, 0xc39d}, /* egress */
        {   5,  311, 1,    2, 7, 0xe216}, /* ingress */
        { 801,    5, 0,  115, 0, 0xc57d}, /* egress */
        { 128,   19, 1,    2, 0, 0xe3e5}, /* ingress */
        { 132,   17, 0,    4, 0, 0xc597}, /* egress */
        {4094,    3, 1, 9999, 5, 0xe3f1}, /* ingress */
        {   1, 1474, 0,    4, 0, 0xc29d}, /* egress */
        {  17,  203, 1,    6, 1, 0x81f6}, /* ingress */
        {2311,    4, 0,  463, 1, 0xc59d}, /* egress */
        {1325,    7, 1,  280, 9, 0x01f6}, /* ingress */
        {4095,    2, 0,  205, 0, 0xc59d}, /* egress */
        {   2, 1429, 1,    1, 8, 0x10f6}, /* ingress */
        {-1, 0, 0, 0, 0, 0}};

    
    const test_iter_info* iter_i = iters;
    mc_test_data_t *data;
   
    BCMDNX_INIT_FUNC_DEFS;

    if(SOC_IS_QUX(unit) ) {
        /* DB is smaller in QUX*/
        iters[10].group_size = -1;
    }

    BCMDNX_IF_ERR_EXIT(get_n_ports(unit, TEST_MAX_PORTS, ports, &ports_number));
    if (ports_number < TEST_MIN_PORTS) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_FAIL, (_BCM_MSG("found only %d ports which are less than the needed %d ports for the test"), ports_number, TEST_MIN_PORTS));
    }
    if (SOC_DPP_CONFIG(unit)->tm.mc_mode & DPP_MC_EGR_17B_CUDS_127_PORTS_MODE) {
        for (egress_ports_num = 0; egress_ports_num < ports_number && ports[egress_ports_num] < 127; ++egress_ports_num);
        if (egress_ports_num < TEST_MIN_PORTS) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_FAIL, (_BCM_MSG("found only %d ports suitable for egress multicast, which are less than the needed %d ports for the test"), egress_ports_num, TEST_MIN_PORTS));
        }

    } else {
        egress_ports_num = ports_number;
    }
    
    for (i = 0; iters[i].group_size > 0; ++i) {
        int port_index = (i+2) % (iters[i].is_ingress ? ports_number : egress_ports_num);
        int consequtive_ports_num = 1;
        iters[i].first_port = ports[port_index];
        for (++port_index; port_index < (iters[i].is_ingress ? ports_number : egress_ports_num) &&
             ports[port_index] == iters[i].first_port + consequtive_ports_num; ++consequtive_ports_num) {
            ++port_index;
        }
        iters[i].increase_port_skip = (iters[i].group_size - 1) / consequtive_ports_num + 1;
    }

    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_init(unit));
    initialized = 1;
    BCMDNX_ALLOC(ids, sizeof(SOC_TMC_MULT_ID) * SOC_DPP_CONFIG(unit)->tm.nof_mc_ids, "mc_ids");
    if (!ids) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BCM_MSG("Failed allocating test memory for MC IDs")));
    }
    BCMDNX_ALLOC(replications_port, sizeof(bcm_gport_t) * ARAD_MC_MAX_GROUP_REPLICATIONS_EGRESS, "mc_replication_ports");
    BCMDNX_ALLOC(replications_encap, sizeof(bcm_if_t) * ARAD_MC_MAX_GROUP_REPLICATIONS_EGRESS, "mc_replication_encaps");
    if (!replications_port || !replications_encap) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BCM_MSG("Failed allocating test memory for MC replications")));
    }

    sal_srand(0x6d73a26e);
    data = test_data[unit];
    /* create and populate MC groups, no group will be used for both ingress & egress */
    for (i = 0; i < SOC_DPP_CONFIG(unit)->tm.nof_mc_ids; ++i) { /* init random group id selection array */
        ids[i] = i;
    }
    { /* test VLANs */
#ifdef _ARAD_MC_BETTER_TEST_VLANS_0
        for (ids_left = data->nof_vlan_egress_mc_groups; ids_left;) { /* for each vlan bitmap group in random order */
            unsigned mask = 128; /* used to get port bits from ids_left */
            i = sal_rand() % ids_left;
            tmp_id = ids[i];
            TEST_PRINT((_BCM_MSG("adding vlan bitmap group 0x%lx  %u"), (unsigned long)tmp_id, ids_left));
            BCMDNX_IF_ERR_EXIT(test_bcm_mc_arad_group_create(unit, tmp_id, MC_EGRESS, 0));
            for (j = 8; j; mask >>= 1) { /*add possibly 8 ports to the bitmap based on lsb bits of ids_left */
                --j;
                if (ids_left & mask) {
                    BCM_GPORT_LOCAL_SET(tmp_port, j); /* use the same port */
                    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_add(unit, tmp_id, tmp_port, 0x1f36, 0));
                }
            }
            ids[i] = ids[--ids_left];
            ids[ids_left] = tmp_id;
        }
        BCMDNX_IF_ERR_EXIT(test_bcm_multicast_verify_all(unit));
#endif
    }

    /* test non VLAN MC groups according to iterations */
    remaining_ids = ids + data->nof_vlan_egress_mc_groups;
    ids_left = SOC_DPP_CONFIG(unit)->tm.nof_mc_ids - data->nof_vlan_egress_mc_groups;
    for (; iter_i->group_size >= 0; ++iter_i) { /* loop through test iterations */
        unsigned groups_left = iter_i->nof_groups;
        TEST_PRINT((_BCM_MSG("replications=%d groups=%u %s ports from %ld encap from 0x%lx port skip=%u"),
          iter_i->group_size, groups_left, iter_i->is_ingress ? "ingress" : "egress",
          (unsigned long)iter_i->first_port, (unsigned long)iter_i->first_encap, iter_i->increase_port_skip));
        TEST_ASSERT(iter_i->group_size <= (iter_i->is_ingress ? ARAD_MC_MAX_GROUP_REPLICATIONS_INGRESS : ARAD_MC_MAX_GROUP_REPLICATIONS_EGRESS));

        TEST_ASSERT(ids_left >= groups_left);
        for (; groups_left; --groups_left) { /* loop over the MC groups of the iteration */
            uint32 port_id = iter_i->first_port;
            bcm_if_t encap = iter_i->first_encap;
            unsigned mc_id_index = sal_rand() % ids_left;
            /* every i-th replication will be to a queue (and not to a port) */
            i = !iter_i->is_ingress  ? 5000 : /* egress mc does not support queue destinations */
              (SOC_DPP_CONFIG(unit)->tm.mc_ing_encoding_mode == 3 || mc_id_index % 5 == 0 ? 1 : 11);
                
            for (j = 0; j < iter_i->group_size;) { /* init replication arrays according to iteration */
                unsigned j_plus = j + 1;
                if (j_plus % i) {
                    BCM_GPORT_LOCAL_SET(replications_port[j], port_id);
                } else {
                    BCM_GPORT_UNICAST_QUEUE_GROUP_SET(replications_port[j], SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_queue * j_plus / iter_i->group_size);
                }
                replications_encap[j] = encap++;
                j = j_plus;
                if (j_plus % iter_i->increase_port_skip == 0) {
                    ++port_id;
                }
            }
            for (j = iter_i->group_size; j > 1;) { /* randomize the order in the replication arrays */
                i = sal_rand() % j;
                --j;
                tmp_port = replications_port[i];
                tmp_encap = replications_encap[i];
                replications_port[i] = replications_port[j];
                replications_encap[i] = replications_encap[j];
                replications_port[j] = tmp_port;
                replications_encap[j] = tmp_encap;
            }

            /* create the MC groups of the iteration, with their replications */
            tmp_id = remaining_ids[mc_id_index];
            /*TEST_PRINT((_BCM_MSG("creating group 0x%lx   %u"), (unsigned long)tmp_id, groups_left));*/
            if (iter_i->is_ingress) {
                BCMDNX_IF_ERR_EXIT(test_bcm_mc_arad_group_create(unit, tmp_id, MC_INGRESS, 0));
            } else { /* egress MC group */
                BCMDNX_IF_ERR_EXIT(test_bcm_mc_arad_group_create(unit, tmp_id, MC_EGRESS, 0));
            }
            if (groups_left % 2) { /* add or set members depending on groups_left being even */
                if (iter_i->is_ingress) {
                    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_ingress_set(unit, tmp_id, iter_i->group_size, replications_port, replications_encap, 0));
                } else { /* egress MC group */
                    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_set(unit, tmp_id, iter_i->group_size, replications_port, replications_encap, 0));
                } 
            } else {
                for (j = 0; j < iter_i->group_size; j++) { /* add each replication separately */
                    if (iter_i->is_ingress) {
                        BCMDNX_IF_ERR_EXIT(test_bcm_multicast_ingress_add(unit, tmp_id, replications_port[j], replications_encap[j], 0));
                    } else { /* egress MC group */
                        BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_add (unit, tmp_id, replications_port[j], replications_encap[j], 0));
                    }
                }
            }
    
            remaining_ids[mc_id_index] = remaining_ids[--ids_left]; /* move used MC ID to end of array */
            remaining_ids[ids_left] = tmp_id;
        } /* end of MC groups loop */
        BCMDNX_IF_ERR_EXIT(test_bcm_multicast_verify_all(unit));
    } /* end of iterations loop */

    /* close all groups, assumes no group is both ingress & egress */
#ifdef _ARAD_MC_BETTER_TEST_VLANS_0
    { /* close VLAN MC groups in random order */
        for (j = data->nof_vlan_egress_mc_groups; j;) { /* for each vlan bitmap group in random order */
            i = sal_rand() % j;
            tmp_id = ids[i];
            /* encap = sal_rand() % SOC_DPP_CONFIG(unit)->tm.egress_mc_max_cud; */
            TEST_PRINT((_BCM_MSG("Removing vlan bitmap group 0x%lx  %u"), (unsigned long)tmp_id, j));
            BCMDNX_IF_ERR_EXIT(test_bcm_multicast_destroy(unit, tmp_id));
            ids[i] = ids[--j];
        }
        BCMDNX_IF_ERR_EXIT(test_bcm_multicast_verify_all(unit));
    }

#endif /* 0 */

    /* close non VLAN MC groups in random order */
    TEST_ASSERT(SOC_DPP_CONFIG(unit)->tm.nof_mc_ids > ids_left + data->nof_vlan_egress_mc_groups );
    remaining_ids += ids_left;
    for (ids_left = SOC_DPP_CONFIG(unit)->tm.nof_mc_ids - data->nof_vlan_egress_mc_groups - ids_left;
         ids_left;) { /* for each vlan bitmap group in random order */
        i = sal_rand() % ids_left;
        tmp_id = remaining_ids[i];
        TEST_PRINT((_BCM_MSG("Removing MC group 0x%lx  %u"), (unsigned long)tmp_id, ids_left));
        BCMDNX_IF_ERR_EXIT(test_bcm_multicast_destroy(unit, tmp_id));
        remaining_ids[i] = remaining_ids[--ids_left];
        if (!(ids_left%4096)) {
            BCMDNX_IF_ERR_EXIT(test_bcm_multicast_verify_all(unit));
        }
    }

    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_detach(unit));
    initialized = 0;

exit:
    BCM_FREE(replications_encap);
    BCM_FREE(replications_port);
    BCM_FREE(ids);
    if (initialized) {
        test_bcm_multicast_detach_dirty(unit);
    }
    BCMDNX_FUNC_RETURN;
}

/* test new replication types */
#define REP_TEST_IS_INGRESS(rand) ((rand) % 97 < 30)
#define REP_TEST_IS_INGRESS_Q_DEST_ONLY(rand) (SOC_DPP_CONFIG(unit)->tm.mc_ing_encoding_mode == 3 || rand % 101 == 0)
#define REP_TEST_Q_NUM(rand, i, mcid) \
  ((((rand_val % SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_queue) + (i + mcid) * 41) * 7) % \
  (SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_queue + 1))
#define REP_TEST_GET_EGRESS_MCID(rand) (((rand) % (SOC_DPP_CONFIG(unit)->tm.nof_mc_ids - nof_bitmap_groups)) + nof_bitmap_groups)
#define REP_TEST_GET_INGRESS_MCID(rand) REP_TEST_GET_EGRESS_MCID(rand)
#ifdef _ARAD_MC_OLD_GROUP_SIZES_IN_ITERATIONS_TEST_0
#define REP_TEST_GET_INGRESS_REPS_NUM(rand) (((rand) % 19 <= 5) ? ((rand) % ARAD_MC_MAX_GROUP_REPLICATIONS_INGRESS) : ((rand) % 16))
#define REP_TEST_GET_EGRESS_REPS_NUM(rand) (((rand) % 19 <= 5) ? ((rand) % 4000) : ((rand) % 16) + 1)
#else
#define REP_TEST_GET_INGRESS_REPS_NUM(rand) ((rand) % 19 ? (((rand) % 19 <= 14) ? ((rand) % 16) : (((rand) % 254) + 2)) : ((rand) % ARAD_MC_MAX_GROUP_REPLICATIONS_INGRESS))
#define REP_TEST_GET_EGRESS_REPS_NUM(rand) ((rand) % 19 ? (((rand) % 19 <= 14) ? (((rand) % 16) + 1) : (((rand) % 254) + 2)) : ((rand) % 4000))
#endif
#define REP_TEST_GET_EGRESS_BITMAP_REPS_NUM(unit, rand, reps) (SOC_IS_QAX(unit) ? 0 : ((reps) > 14 ? (rand) % 7 : 0))
#define REP_TEST_GET_EGRESS_PORT_CUD_REPS_NUM(unit, rand, reps, bitmap_reps) (SOC_IS_QAX(unit) ? reps : ((((reps) - (bitmap_reps)) * ((rand) % 53 + 48)) / 100))
#define REP_TEST_GET_EGRESS_CUD_REPS_NUM(rand, reps, bitmap_reps, port_cud_reps) ((reps) - ((bitmap_reps) + (port_cud_reps)))
#define REP_TEST_GET_USE_ADD(rand) ((rand) % 37 <= 9)
#define REP_TEST_GET_USE_EGRESS_ADD(rand) ((rand) % 37 == 0)
#define IS_INGRESS_GROUP_USED(mcid) ((ingress_groups[(mcid)>>5] >> ((mcid) & 31)) & 1)
#define IS_EGRESS_GROUP_USED(mcid) ((egress_groups[(mcid)>>5] >> ((mcid) & 31)) & 1)
#define SET_INGRESS_GROUP_USED(mcid) do {ingress_groups[(mcid)>>5] |= 1 << ((mcid) & 31);} while (0)
#define SET_INGRESS_GROUP_UNUSED(mcid) do {ingress_groups[(mcid)>>5] &= ~(1 << ((mcid) & 31));} while (0)
#define SET_EGRESS_GROUP_USED(mcid) do {egress_groups[(mcid)>>5] |= 1 << ((mcid) & 31);} while (0)
#define SET_EGRESS_GROUP_UNUSED(mcid) do {egress_groups[(mcid)>>5] &= ~(1 << ((mcid) & 31));} while (0)
#define REP_TEST_STOP_HERE (MCDS_INGRESS_LINK_END(mcds) - (ARAD_MC_MAX_GROUP_REPLICATIONS_ANY+1)) /* stop assuming that new groups can be created after this number of replications */
#define REP_TEST_IF_ERR_FAIL(_rc) \
      do { \
        int __err__rc = _rc; \
        if (__err__rc != BCM_E_NONE) { \
            if (may_fail && __err__rc == BCM_E_FULL) { \
                full = 1; \
            } else { \
                LOG_ERROR(BSL_LS_APPL_TESTS, \
                          (BSL_META_U(unit, \
                                      "%s\n"), \
                           bcm_errmsg(__err__rc))); \
                _rv = __err__rc; \
                BCM_EXIT; \
            } \
        } \
      } while(0)
int test_bcm_mc_arad_replications(int unit, unsigned iters_num)
{
    unsigned i, j, k, initialized = 0;
    dpp_mcds_base_t *mcds = dpp_get_mcds(unit);
    uint32 highest_bitmap_group = SOC_DPP_CONFIG(unit)->tm.multicast_egress_bitmap_group_range.mc_id_high;
    uint32 nof_bitmap_groups = highest_bitmap_group + 1;
    bcm_gport_t port;
    int ports_number, egress_ports_num;
    bcm_port_t ports[TEST_MAX_PORTS];
    bcm_port_t bm_port_1, bm_port_2;
    bcm_gport_t *port_array = NULL;
    bcm_if_t *cud_array = NULL;
    int *rands = NULL, rands_num = 0;
    uint32 *egress_groups = NULL, *ingress_groups = NULL; /* keep if a linked list group is open */
    uint32 rep_entries = 0; /* estimated number of used mcdb entries */
    SOC_TMC_MULT_ID mcid;
    int iteration;
    sal_usecs_t start_time = sal_time_usecs(), print_time;
    BCMDNX_INIT_FUNC_DEFS;


    BCMDNX_IF_ERR_EXIT(get_n_ports(unit, TEST_MAX_PORTS, ports, &ports_number));
    if (ports_number < TEST_MIN_PORTS) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_FAIL, (_BCM_MSG("found only %d ports which are less than the needed %d ports for the test"), ports_number, TEST_MIN_PORTS));
    }
    if (SOC_DPP_CONFIG(unit)->tm.mc_mode & DPP_MC_EGR_17B_CUDS_127_PORTS_MODE) {
        for (egress_ports_num = 0; egress_ports_num < ports_number && ports[egress_ports_num] < 127; ++egress_ports_num);
        if (egress_ports_num < TEST_MIN_PORTS) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_FAIL, (_BCM_MSG("found only %d ports suitable for egress multicast, which are less than the needed %d ports for the test"), egress_ports_num, TEST_MIN_PORTS));
        }

    } else {
        egress_ports_num = ports_number;
    }

    if (SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit)) { /* QAX does not support CUD only replication */
        for (i = ARAD_MC_16B_MAX_EGR_CUD; i; --i) {
            bcm_port_encap_map_set(unit, 0, i, ports[i % egress_ports_num]);
        }
    }

    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_init(unit));
    initialized = 1;

    /* create and set all the bitmap groups */
    bm_port_1 = ports[egress_ports_num - 1];
    bm_port_2 = ports[0];

    BCMDNX_ALLOC(port_array, sizeof(bcm_gport_t)*4095, "test_bcm_mc_arad_replications.port_array");
    if (port_array == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BCM_MSG("failed to allocate memory")));
    }
    BCMDNX_ALLOC(cud_array, sizeof(bcm_if_t)*4095, "test_bcm_mc_arad_replications.cud_array");
    if (cud_array == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BCM_MSG("failed to allocate memory")));
    }

    port_array[0] = bm_port_2;
    port_array[1] = bm_port_1;
    cud_array[0] = 0x9876;
    cud_array[1] = 0;
    for (i = 0; i < nof_bitmap_groups; ++i) { /* Set the group to two ports using one of a few techniques */
        BCMDNX_IF_ERR_EXIT(test_bcm_mc_arad_group_create(unit, i, MC_EGRESS, 0));
        if (i%5 == 0) {
            BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_add(unit, i, bm_port_1, 7, 0));
            BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_add(unit, i, bm_port_2, 7777, 0));
        } else if (i%5 == 1) {
            BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_set(unit, i, 2, port_array, cud_array, 0));
        } else if (i%5 == 2) {
            BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_set(unit, i, 2, port_array, cud_array, 0));
            BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_delete_all(unit, i));
            BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_add(unit, i, bm_port_2, 0xfe61, 0));
            BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_add(unit, i, bm_port_1, 1, 0));
        } else if (i%5 == 3) {
            BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_set(unit, i, 1, port_array, cud_array, 0));
            BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_add(unit, i, bm_port_1, 0, 0));
        } else {
            BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_add(unit, i, bm_port_1, 0, 0));
            BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_set(unit, i, 1, port_array, cud_array, 0));
            BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_add(unit, i, bm_port_1, 0, 0));
        }
    }

    sal_srand(0x3d73ef7d + iters_num);
    BCMDNX_ALLOC(rands, sizeof(int) * (MCDS_INGRESS_LINK_END(mcds) + 1), "test_bcm_mc_arad_replications.rands");
    BCMDNX_ALLOC(ingress_groups, sizeof(uint32) * (MCDS_INGRESS_LINK_END(mcds) + 1) / 32, "test_bcm_mc_arad_replications.groups_used");
    for (i=0; i <= MCDS_INGRESS_LINK_END(mcds) / 32; ++i) {
        ingress_groups[i]=0; /* zero the memory */
    }
    egress_groups = ingress_groups + (SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids/32);

    for (iteration = 1; iteration <= iters_num; ++iteration) { /* start a test iteration */
        int group_num = 0, full = 0, initial_groups_num = rands_num;
        unsigned extra_groups = 0;
        sal_usecs_t iter_start_time = sal_time_usecs();
        TEST_PRINT((_BCM_MSG("iteration %d/%u starting"), iteration, iters_num));

        /* 1. Add new MC groups and fill them */
        while (!full) { /* while we think we should not run out of mcdb entries */
            int may_fail = rep_entries > REP_TEST_STOP_HERE;
            int rand_val = sal_rand();
            rands[rands_num] = rand_val;
            if (REP_TEST_IS_INGRESS(rand_val)) { /* create and fill an ingress group */
                unsigned reps = REP_TEST_GET_INGRESS_REPS_NUM(rand_val); /* the size the group will have minus one */
                int only_queue_destinations = REP_TEST_IS_INGRESS_Q_DEST_ONLY(rand_val);
                mcid = REP_TEST_GET_INGRESS_MCID(rand_val);
                if (IS_INGRESS_GROUP_USED(mcid) || IS_EGRESS_GROUP_USED(mcid)) continue;
                TEST_PRINT((_BCM_MSG("i%d: %s ingress group %u  size=%u  rand %d    rep_entries=%d only_q_dests=%d"),
                  iteration, may_fail ? "trying to add" : "adding", mcid, reps, rand_val, rep_entries, only_queue_destinations));
                REP_TEST_IF_ERR_FAIL(test_bcm_mc_arad_group_create(unit, mcid, MC_INGRESS, may_fail));
                if (full) break;
                i = 0;
                if (!REP_TEST_GET_USE_ADD(rand_val)) {
                    for ( ; i + 1 < reps; ++i) {
                        if (only_queue_destinations || (i & 127) == 3) {
                            BCM_GPORT_UNICAST_QUEUE_GROUP_SET(port_array[i], REP_TEST_Q_NUM(rand_val, i, mcid));
                        } else {
                            BCM_GPORT_LOCAL_SET(port_array[i], ports[(i + mcid) % ports_number]);
                        }
                        cud_array[i] = (rand_val+i) & SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_cud;
                    }
                    REP_TEST_IF_ERR_FAIL(test_bcm_multicast_ingress_set(unit, mcid, reps ? reps - 1 : 0, port_array, cud_array, may_fail));
                }
                /* add the remaining members (only the last member or all the members) */
                for (; i < reps && !full; ++i) {
                    if (only_queue_destinations || (i & 127) == 3) {
                        BCM_GPORT_UNICAST_QUEUE_GROUP_SET(port, REP_TEST_Q_NUM(rand_val, i, mcid));
                    } else {
                        BCM_GPORT_LOCAL_SET(port, ports[(i + mcid) % ports_number]);
                    }
                    REP_TEST_IF_ERR_FAIL(test_bcm_multicast_ingress_add(unit, mcid, port, (rand_val+i) & SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_cud, may_fail));
                }
                if (full) {
                    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_destroy(unit, mcid));
                } else {
                    SET_INGRESS_GROUP_USED(mcid);
                    rep_entries += reps ? reps : 1;
                }
            } else { /* create and fill an egress group */
                unsigned reps = REP_TEST_GET_EGRESS_REPS_NUM(rand_val);
                unsigned bitmap_reps = REP_TEST_GET_EGRESS_BITMAP_REPS_NUM(unit,rand_val, reps);
                unsigned port_cud_reps = REP_TEST_GET_EGRESS_PORT_CUD_REPS_NUM(unit, rand_val, reps, bitmap_reps);
                unsigned cud_reps = REP_TEST_GET_EGRESS_CUD_REPS_NUM(rand_val, reps, bitmap_reps, port_cud_reps);
                if (SOC_DPP_CONFIG(unit)->tm.mc_mode & DPP_MC_EGR_17B_CUDS_127_PORTS_MODE) {
                    port_cud_reps += cud_reps;
                    cud_reps = 0; /* This mode does not support outlif only replications */
                }
                TEST_ASSERT(reps == bitmap_reps + port_cud_reps + cud_reps);
                mcid = REP_TEST_GET_EGRESS_MCID(rand_val);
                if (IS_INGRESS_GROUP_USED(mcid) || IS_EGRESS_GROUP_USED(mcid)) continue; /* we do not use the same mcid for both ingress & egress to make group destructions simple */

                TEST_PRINT((_BCM_MSG("i%d: %s egress group %u size=%u(%u,%u,%u) rand %d    rep_entries=%d"), iteration,
                  may_fail ? "trying to add" : "adding", mcid, reps, port_cud_reps, cud_reps, bitmap_reps, rand_val, rep_entries));
                REP_TEST_IF_ERR_FAIL(test_bcm_mc_arad_group_create(unit, mcid, MC_EGRESS, may_fail));
                if (full) break;
                j = 0;
                if (!REP_TEST_GET_USE_EGRESS_ADD(rand_val)) {
                    for (i = 0; i < bitmap_reps; ++i, ++j) {
                        BCM_GPORT_MCAST_SET(port_array[j], (i+mcid) % highest_bitmap_group + 1); /* bitmap may not be 0 */
                        cud_array[j] = (rand_val+i) & SOC_DPP_CONFIG(unit)->tm.egress_mc_max_cud;
                    }
                    for (i = 0; i < port_cud_reps; ++i, ++j) {
                        BCM_GPORT_LOCAL_SET(port_array[j], ports[(i+mcid) % egress_ports_num]);
                        cud_array[j] = (rand_val+i) & SOC_DPP_CONFIG(unit)->tm.egress_mc_max_cud;
                    }
                    for (i = 0; i < cud_reps; ++i, ++j) {
                        port_array[j] = BCM_GPORT_INVALID;
                        cud_array[j] = (rand_val+i) % ARAD_MC_16B_MAX_EGR_CUD + 1; /* outlif may not be 0 */
                    }
                    REP_TEST_IF_ERR_FAIL(test_bcm_multicast_egress_set(unit, mcid, reps, port_array, cud_array, may_fail));
                } else {
                    for (i = 0; i < bitmap_reps && !full; ++i, ++j) {
                        BCM_GPORT_MCAST_SET(port, (i+mcid) % highest_bitmap_group + 1); /* bitmap may not be 0 */
                        REP_TEST_IF_ERR_FAIL(test_bcm_multicast_egress_add(unit, mcid, port, (rand_val+i) & SOC_DPP_CONFIG(unit)->tm.egress_mc_max_cud, may_fail));
                    }
                    for (i = 0; i < port_cud_reps && !full; ++i, ++j) {
                        BCM_GPORT_LOCAL_SET(port, ports[(i+mcid) % egress_ports_num]);
                        REP_TEST_IF_ERR_FAIL(test_bcm_multicast_egress_add(unit, mcid, port, (rand_val+i) & SOC_DPP_CONFIG(unit)->tm.egress_mc_max_cud, may_fail));
                    }
                    for (i = 0; i < cud_reps && !full; ++i, ++j) {
                        REP_TEST_IF_ERR_FAIL(test_bcm_multicast_egress_add(unit, mcid, BCM_GPORT_INVALID, (rand_val+i) % ARAD_MC_16B_MAX_EGR_CUD + 1, may_fail));
                    }
                }
                if (full) {
                    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_destroy(unit, mcid));
                } else {
                    TEST_ASSERT(reps == j);
                    SET_EGRESS_GROUP_USED(mcid);
                    rep_entries += reps ? reps : 1;
                }
            }
            if (!full) {
                ++rands_num;
                if (may_fail) {
                    ++extra_groups;
                }
                if (++group_num % 170 == 0) {
                    TEST_PRINT((_BCM_MSG("iteration %d group %u"), iteration, group_num));
                    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_verify_all(unit));
                }
            }
        }
        INFO_PRINT((_BCM_MSG("iteration %d/%u finished adding %d groups, out of which %u groups were added when the mcdb could have overflown"), iteration, iters_num, rands_num - initial_groups_num, extra_groups));
        BCMDNX_IF_ERR_EXIT(test_bcm_multicast_verify_all(unit));

        j = rands_num * 2 / 5;
        /* 2. Delete groups created from rands[j .. j*2-1] */
        for (k = j * 2 - 1; k >= j; --k) {
            int rand_val = rands[k];

            if (REP_TEST_IS_INGRESS(rand_val)) { /* delete an ingress group */
                unsigned reps = REP_TEST_GET_INGRESS_REPS_NUM(rand_val); /* the size the group will have */
                int only_queue_destinations = REP_TEST_IS_INGRESS_Q_DEST_ONLY(rand_val);
                mcid = REP_TEST_GET_INGRESS_MCID(rand_val);
                TEST_PRINT((_BCM_MSG("destroying ingress group %u     rep_entries=%d"), mcid, rep_entries));
                TEST_ASSERT(IS_INGRESS_GROUP_USED(mcid));
                if (rand_val % 49 < 9) {
                    for (i = 0 ; i < reps; i += 2) {
                    if (only_queue_destinations || (i & 127) == 3) {
                        BCM_GPORT_UNICAST_QUEUE_GROUP_SET(port, REP_TEST_Q_NUM(rand_val, i, mcid));
                    } else {
                        BCM_GPORT_LOCAL_SET(port, ports[(i + mcid) % ports_number]);
                    }
                        BCMDNX_IF_ERR_EXIT(test_bcm_multicast_ingress_delete(unit, mcid, port, (rand_val+i) & SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_cud));
                    }
                }
                if (rand_val & 1) {
                    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_ingress_delete_all(unit, mcid));
                }
                BCMDNX_IF_ERR_EXIT(test_bcm_multicast_destroy(unit, mcid));

                SET_INGRESS_GROUP_UNUSED(mcid);
                if (!reps) reps = 1;
                TEST_ASSERT(rep_entries >= reps);
                rep_entries -= reps;
            } else { /* delete an egress group */
                unsigned reps = REP_TEST_GET_EGRESS_REPS_NUM(rand_val);
                unsigned bitmap_reps = REP_TEST_GET_EGRESS_BITMAP_REPS_NUM(unit,rand_val, reps);
                unsigned port_cud_reps = REP_TEST_GET_EGRESS_PORT_CUD_REPS_NUM(unit, rand_val, reps, bitmap_reps);
                unsigned cud_reps = REP_TEST_GET_EGRESS_CUD_REPS_NUM(rand_val, reps, bitmap_reps, port_cud_reps);
                if (SOC_DPP_CONFIG(unit)->tm.mc_mode & DPP_MC_EGR_17B_CUDS_127_PORTS_MODE) {
                    port_cud_reps += cud_reps;
                    cud_reps = 0; /* This mode does not support outlif only replications */
                }
                mcid = REP_TEST_GET_EGRESS_MCID(rand_val);
                TEST_PRINT((_BCM_MSG("destroying egress group %u     rep_entries=%d"), mcid, rep_entries));
                TEST_ASSERT(reps == bitmap_reps + port_cud_reps + cud_reps && IS_EGRESS_GROUP_USED(mcid));

                if (rand_val % 49 < 9) {
                    for (i = 0; i < bitmap_reps && !full; i += 2) {
                        BCM_GPORT_MCAST_SET(port, (i+mcid) % highest_bitmap_group + 1); /* bitmap may not be 0 */
                        BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_delete(unit, mcid, port, (rand_val+i) & SOC_DPP_CONFIG(unit)->tm.egress_mc_max_cud, 0));
                    }
                    for (i = 0; i < port_cud_reps && !full; i += 2) {
                        BCM_GPORT_LOCAL_SET(port, ports[(i+mcid) % egress_ports_num]);
                        BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_delete(unit, mcid, port, (rand_val + i) & SOC_DPP_CONFIG(unit)->tm.egress_mc_max_cud, 0));
                    }
                    for (i = 0; i < cud_reps && !full; i += 2) {
                        BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_delete(unit, mcid, BCM_GPORT_INVALID, (rand_val + i) % ARAD_MC_16B_MAX_EGR_CUD + 1, 0));
                    }
                }
                if (rand_val & 1) {
                    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_delete_all(unit, mcid));
                }
                BCMDNX_IF_ERR_EXIT(test_bcm_multicast_destroy(unit, mcid));

                SET_EGRESS_GROUP_UNUSED(mcid);
                if (!reps) reps = 1;
                TEST_ASSERT(rep_entries >= reps);
                rep_entries -= reps;
            }
#ifdef _ARAD_MC_TEST_HIGHER_VERIFICATION_RESOLUTION_0
            if ((k-j) % 100 == 99) {
                TEST_PRINT((_BCM_MSG("iteration %d k %d    j %d"), iteration, k, j));
                BCMDNX_IF_ERR_EXIT(test_bcm_multicast_verify_all(unit));
            }
#endif
        }
        TEST_PRINT((_BCM_MSG("iteration %d finished destroying groups"), iteration));
        BCMDNX_IF_ERR_EXIT(test_bcm_multicast_verify_all(unit));

        /* 3. For each group from rands[0 .. j-1], add members and delete members */
        for (; k < j; --k) {
            int rand_val = rands[k];
            unsigned reps, orig_reps;

            if (REP_TEST_IS_INGRESS(rand_val)) { /* ingress group */
                int reps_to_add = rand_val % 29 + 1;
                int only_queue_destinations = REP_TEST_IS_INGRESS_Q_DEST_ONLY(rand_val);
                orig_reps = reps = REP_TEST_GET_INGRESS_REPS_NUM(rand_val); /* the current size of the group */
                mcid = REP_TEST_GET_INGRESS_MCID(rand_val);
                if (reps + reps_to_add > ARAD_MC_MAX_GROUP_REPLICATIONS_INGRESS) {
                    reps_to_add = ARAD_MC_MAX_GROUP_REPLICATIONS_INGRESS - reps;
                }
                TEST_ASSERT(IS_INGRESS_GROUP_USED(mcid) && reps_to_add >=0);

                TEST_PRINT((_BCM_MSG("adding %d replications to ingress group %u before its deletion  orig_size:%u   rep_entries=%d"), reps_to_add, mcid, orig_reps, rep_entries));
                if (rand_val % 7 < 4) {
                    int found_size;
                    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_ingress_get(unit, mcid, ARAD_MC_MAX_GROUP_REPLICATIONS_INGRESS, port_array, cud_array, &found_size));
                    TEST_ASSERT(found_size == reps);
                    reps += reps_to_add;
                    for (i = found_size; i < reps; ++i) {
                        if (only_queue_destinations || (i & 127) == 3) {
                            BCM_GPORT_UNICAST_QUEUE_GROUP_SET(port_array[i], REP_TEST_Q_NUM(rand_val, i, mcid));
                        } else {
                            BCM_GPORT_LOCAL_SET(port_array[i], ports[(i + mcid) % ports_number]);
                        }
                        cud_array[i] = (rand_val+i) & SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_cud;
                    }
                    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_ingress_set(unit, mcid, reps, port_array, cud_array, 0));
                } else {
                    i = reps;
                    for (reps += reps_to_add; i < reps; ++i) {
                        if (only_queue_destinations || (i & 127) == 3) {
                            BCM_GPORT_UNICAST_QUEUE_GROUP_SET(port, REP_TEST_Q_NUM(rand_val, i, mcid));
                        } else {
                            BCM_GPORT_LOCAL_SET(port, ports[(i + mcid) % ports_number]);
                        }
                        cud_array[i] = (rand_val+i) & SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_cud;
                        BCMDNX_IF_ERR_EXIT(test_bcm_multicast_ingress_add(unit, mcid, port, (rand_val+i) & SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_cud, 0));
                    }
                }
                /* delete up to five replications */
                if (reps > 5) {
                    reps = 5;
                }
                for (i = 0 ; i < reps;  i+=2) {
                    if (only_queue_destinations || (i & 127) == 3) {
                        BCM_GPORT_UNICAST_QUEUE_GROUP_SET(port, REP_TEST_Q_NUM(rand_val, i, mcid));
                    } else {
                        BCM_GPORT_LOCAL_SET(port, ports[(i + mcid) % ports_number]);
                    }
                    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_ingress_delete(unit, mcid, port, (rand_val+i) & SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_cud));
                }

            } else { /* egress group */
                unsigned bitmap_reps, port_cud_reps, cud_reps;
                unsigned add_bitmap = 0, add_port_cud = 0, add_cud = 0;
                orig_reps = reps = REP_TEST_GET_EGRESS_REPS_NUM(rand_val);
                bitmap_reps = REP_TEST_GET_EGRESS_BITMAP_REPS_NUM(unit,rand_val, reps);
                port_cud_reps = REP_TEST_GET_EGRESS_PORT_CUD_REPS_NUM(unit, rand_val, reps, bitmap_reps);
                cud_reps = REP_TEST_GET_EGRESS_CUD_REPS_NUM(rand_val, reps, bitmap_reps, port_cud_reps);
                if (SOC_DPP_CONFIG(unit)->tm.mc_mode & DPP_MC_EGR_17B_CUDS_127_PORTS_MODE) {
                    port_cud_reps += cud_reps;
                    cud_reps = 0; /* This mode does not support outlif only replications */
                }
                mcid = REP_TEST_GET_EGRESS_MCID(rand_val);
                TEST_ASSERT(reps == bitmap_reps + port_cud_reps + cud_reps && IS_EGRESS_GROUP_USED(mcid));
                if (reps < ARAD_MC_MAX_GROUP_REPLICATIONS_EGRESS) {
                    add_port_cud = 1;
                    ++reps;
                    if (!SOC_IS_QAX(unit) && (reps < ARAD_MC_MAX_GROUP_REPLICATIONS_EGRESS)) {
                        if ((SOC_DPP_CONFIG(unit)->tm.mc_mode & DPP_MC_EGR_17B_CUDS_127_PORTS_MODE) == 0) {
                            add_cud = 1;
                            ++reps;
                        }
                        if (reps < ARAD_MC_MAX_GROUP_REPLICATIONS_EGRESS - 4) {
                            add_bitmap = 1;
                            ++reps;
                        }
                    }
                }

                TEST_PRINT((_BCM_MSG("adding %u,%u,%u replications to egress group %u before its deletion   size:%u->%u    rep_entries=%d"), add_port_cud, add_cud, add_bitmap, mcid, orig_reps, reps, rep_entries));
                if (rand_val % 7 < 4) {
                    int found_size;
                    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_get(unit, mcid, ARAD_MC_MAX_GROUP_REPLICATIONS_EGRESS, port_array, cud_array, &found_size));
                    TEST_ASSERT(found_size == orig_reps);
                    for (i = 0; i < add_bitmap; ++i, ++orig_reps) {
                        BCM_GPORT_MCAST_SET(port_array[orig_reps], (i + mcid + bitmap_reps) % highest_bitmap_group + 1); /* bitmap may not be 0 */
                        cud_array[orig_reps] = (rand_val + bitmap_reps + i) & SOC_DPP_CONFIG(unit)->tm.egress_mc_max_cud;
                    }
                    for (i = 0; i < add_port_cud; ++i, ++orig_reps) {
                        BCM_GPORT_LOCAL_SET(port_array[orig_reps], ports[(i + mcid + port_cud_reps) % egress_ports_num]);
                        cud_array[orig_reps] = (rand_val + port_cud_reps + i) & SOC_DPP_CONFIG(unit)->tm.egress_mc_max_cud;
                    }
                    for (i = 0; i < add_cud; ++i, ++orig_reps) {
                        port_array[orig_reps] = BCM_GPORT_INVALID;
                        cud_array[orig_reps] = (rand_val + cud_reps + i) % ARAD_MC_16B_MAX_EGR_CUD + 1; /* outlif may not be 0 */
                    }
                    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_set(unit, mcid, reps, port_array, cud_array, 0));
                } else {
                    for (i = 0; i < add_bitmap; ++i) {
                        BCM_GPORT_MCAST_SET(port, (i + mcid + bitmap_reps) % highest_bitmap_group + 1); /* bitmap may not be 0 */
                        BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_add(unit, mcid, port, (rand_val + bitmap_reps + i) & SOC_DPP_CONFIG(unit)->tm.egress_mc_max_cud, 0));
                    }
                    for (i = 0; i < add_port_cud; ++i) {
                        BCM_GPORT_LOCAL_SET(port, ports[(i + mcid + port_cud_reps) % egress_ports_num]);
                        BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_add(unit, mcid, port, (rand_val + port_cud_reps + i) & SOC_DPP_CONFIG(unit)->tm.egress_mc_max_cud, 0));
                    }
                    for (i = 0; i < add_cud; ++i) {
                        BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_add(unit, mcid, BCM_GPORT_INVALID, (rand_val + cud_reps + i) % ARAD_MC_16B_MAX_EGR_CUD + 1, 0)); /* outlif may not be 0 */
                    }
                }

                /* delete replications */
                add_bitmap += bitmap_reps;
                if (add_bitmap > 1) add_bitmap = 1;
                add_port_cud += port_cud_reps;
                if (add_port_cud > 2) add_port_cud = 2;
                add_cud += cud_reps;
                if (add_cud > 2) add_cud = 2;
                for (i = 0; i < add_bitmap; ++i) {
                    BCM_GPORT_MCAST_SET(port, (i + mcid) % highest_bitmap_group + 1); /* bitmap may not be 0 */
                    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_delete(unit, mcid, port, (rand_val + i) & SOC_DPP_CONFIG(unit)->tm.egress_mc_max_cud, 0));
                }
                for (i = 0; i < add_port_cud; ++i) {
                    BCM_GPORT_LOCAL_SET(port, ports[(i + mcid) % egress_ports_num]);
                    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_delete(unit, mcid, port, (rand_val + i) & SOC_DPP_CONFIG(unit)->tm.egress_mc_max_cud, 0));
                }
                for (i = 0; i < add_cud; ++i) {
                    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_delete(unit, mcid, BCM_GPORT_INVALID, (rand_val + i) % ARAD_MC_16B_MAX_EGR_CUD + 1, 0)); /* outlif may not be 0 */
                }
                orig_reps = REP_TEST_GET_EGRESS_REPS_NUM(rand_val);

            }
            if (!orig_reps) orig_reps = 1;
            TEST_ASSERT(rep_entries >= orig_reps);
            rep_entries -= orig_reps;

#ifdef _ARAD_MC_TEST_HIGHER_VERIFICATION_RESOLUTION_0
            if (k % 100 == 99) {
                TEST_PRINT((_BCM_MSG("deleting replications  iteration %d k %d    j %d"), iteration, k, j));
                BCMDNX_IF_ERR_EXIT(test_bcm_multicast_verify_all(unit));
            }
#endif
        }
        TEST_PRINT((_BCM_MSG("iteration %d finished adding replications"), iteration));
        BCMDNX_IF_ERR_EXIT(test_bcm_multicast_verify_all(unit));

        /* 4. Destroy groups from rands[0 .. j-1] */
        for (k = j - 1; k < j; --k) {
            int rand_val = rands[k];

            mcid = REP_TEST_IS_INGRESS(rand_val) ?
              REP_TEST_GET_INGRESS_MCID(rand_val) :
              REP_TEST_GET_EGRESS_MCID(rand_val);
            BCMDNX_IF_ERR_EXIT(test_bcm_multicast_destroy(unit, mcid));
        }
        TEST_PRINT((_BCM_MSG("iteration %d finished deleting the groups added to"), iteration));
        BCMDNX_IF_ERR_EXIT(test_bcm_multicast_verify_all(unit));

        /* 5. move remaining groups rands[j*2 .. rands_num-1] to rands[0 ..rands_num-1-j*2 */
        i = j * 2;
        for (k = i; k < rands_num; ++k) {
            rands[k-i] = rands[k];
        }
        rands_num -= i;

        print_time = (sal_time_usecs() - iter_start_time) / 1000;
        k = print_time / 1000;
        INFO_PRINT((_BCM_MSG("iteration %d finished in %u:%.2u.%.3u m:s"), iteration, k / 60, k % 60, print_time % 1000));
    }

    TEST_PRINT((_BCM_MSG("destroying remaining %d groups after the end of the iterations"), rands_num));
    /* Destroy remaining groups after iterations */
    for (k = 0; k < rands_num; ++k) {
        int rand_val = rands[k];
        mcid = REP_TEST_IS_INGRESS(rand_val) ?
          REP_TEST_GET_INGRESS_MCID(rand_val) :
          REP_TEST_GET_EGRESS_MCID(rand_val);
        BCMDNX_IF_ERR_EXIT(test_bcm_multicast_destroy(unit, mcid));
    }

    for (i = 0; i < nof_bitmap_groups; ++i) { /* Set the group to two ports using one of a few techniques */
        BCMDNX_IF_ERR_EXIT(test_bcm_multicast_destroy(unit, i));
    }

    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_verify_all(unit));

    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_detach(unit));
    print_time = (sal_time_usecs() - start_time) / 1000;
    k = print_time / 1000;
    INFO_PRINT((_BCM_MSG("test finished successfully in %u:%.2u.%.3u m:s"), k / 60, k % 60, print_time % 1000));
    initialized = 0;

exit:
    BCM_FREE(rands);
    BCM_FREE(ingress_groups);
    BCM_FREE(port_array);
    BCM_FREE(cud_array);
    if (initialized) {
        test_bcm_multicast_detach_dirty(unit);
    }
    BCMDNX_FUNC_RETURN;
}

/*
 * Test replication deletion from an egress group, when the MCDB is full, and the group can not be reconstructed.
 * Test using all egress replication types.
 * This test strongly depends on the Arad multicast implementation.
 */
#define MY_NOF_EGRESS_GROUP_REPS (8 *TEST_MIN_PORTS +  12)
#define QAX_MY_NOF_EGRESS_GROUP_REPS (8 * TEST_MIN_PORTS)/* cud only replication does not exist for qax*/
int test_bcm_mc_arad_delete_egress_replications(int unit)
{
    unsigned i, eg_reps = 0, ing_groups, initialized = 0, ing_mc_groups_created;
    int ports_number, egress_ports_num, full = 0, may_fail = 0;
    bcm_port_t ports[TEST_MAX_PORTS];
    bcm_gport_t port_array[MY_NOF_EGRESS_GROUP_REPS], gport;
    bcm_if_t cud_array[MY_NOF_EGRESS_GROUP_REPS];
    bcm_if_t cuds_port_reps[TEST_MIN_PORTS] = {0x1234, 0x2345, 0xabcd, 0xfedb};
    bcm_if_t cuds_outlif_reps[12] = {0x1357, 0x2468, 0x9bdf, 0x8ace, 0x1358, 0x2469, 0x9be0, 0x8acf, 0x1359, 0x246a, 0x9be1, 0x8ad0};
    bcm_if_t cuds_bitmap_reps[TEST_MIN_PORTS] = {0x0369, 0x147a, 0x258b, 0x369c};
    SOC_TMC_MULT_ID mcid = (SOC_IS_QAX(unit) ? 0x3ffe : 0xfffe);
    int max_nof_ingr_groups_in_mcdb;
    BCMDNX_INIT_FUNC_DEFS;

    if (SOC_IS_QAX(unit)) {
        /*
          QAX - Table size 96K, 16K taken for egress bitmap, and 16K ing groups by default. In order to feel the table with ing entries each group need to occupy minimum 5 entries
          QUX - Table size 48K, 16K taken for egress bitmap, and 16K ing groups by default. In order to feel the table with ing entries each group need to occupy minimum 3 entries
        */
        max_nof_ingr_groups_in_mcdb = (QAX_LAST_MCDB_ENTRY(unit) + 1 - (SOC_DPP_CONFIG(unit)->qax->nof_egress_bitmaps * SOC_DPP_CONFIG(unit)->qax->nof_egress_bitmap_bytes)) / (SOC_IS_QUX(unit) ? 3 : 5);
    } else {
        max_nof_ingr_groups_in_mcdb = (DPP_LAST_MCDB_ENTRY(dpp_get_mcds(unit)) + 1) / (SOC_IS_JERICHO(unit) ? 4 : 2); 
    }


    BCMDNX_IF_ERR_EXIT(get_n_ports(unit, TEST_MAX_PORTS, ports, &ports_number));
    if (ports_number < TEST_MIN_PORTS) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_FAIL, (_BCM_MSG("found only %d ports which are less than the needed %d ports for the test"), ports_number, TEST_MIN_PORTS));
    }
    if (SOC_DPP_CONFIG(unit)->tm.mc_mode & DPP_MC_EGR_17B_CUDS_127_PORTS_MODE) {
        for (egress_ports_num = 0; egress_ports_num < ports_number && ports[egress_ports_num] < 127; ++egress_ports_num);
        if (egress_ports_num < TEST_MIN_PORTS) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_FAIL, (_BCM_MSG("found only %d ports suitable for egress multicast, which are less than the needed %d ports for the test"), egress_ports_num, TEST_MIN_PORTS));
        }

    } else {
        egress_ports_num = ports_number;
    }

    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_init(unit));
    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_verify_all(unit));
    initialized = 1;

    /* create and fill an egress group */
    for (i = 0 ; i < TEST_MIN_PORTS; ++i) {
        BCM_GPORT_LOCAL_SET(port_array[eg_reps], ports[i]);
        cud_array[eg_reps++] = cuds_port_reps[0] + i; /* will make format 4/5 entries if space is left in consecutive entries block */
        BCM_GPORT_LOCAL_SET(port_array[eg_reps], ports[i]);
        cud_array[eg_reps++] = cuds_port_reps[1] + i; /* will make format 4/5 entries if space is left in consecutive entries bloc */
        BCM_GPORT_LOCAL_SET(port_array[eg_reps], ports[i]);
        cud_array[eg_reps++] = cuds_port_reps[2]; /* will make format 0 entries */
        BCM_GPORT_LOCAL_SET(port_array[eg_reps], ports[i]);
        cud_array[eg_reps++] = cuds_port_reps[3]; /* will make format 0 entries */
        
        BCM_GPORT_MCAST_SET(port_array[eg_reps], 1); /* bitmap may not be 0 */
        cud_array[eg_reps++] = cuds_bitmap_reps[i];
        BCM_GPORT_MCAST_SET(port_array[eg_reps], 2);
        cud_array[eg_reps++] = cuds_bitmap_reps[i];
        BCM_GPORT_MCAST_SET(port_array[eg_reps], 3);
        cud_array[eg_reps++] = cuds_bitmap_reps[i];
        BCM_GPORT_MCAST_SET(port_array[eg_reps], 16);
        cud_array[eg_reps++] = cuds_bitmap_reps[i];
    }

    if (!SOC_IS_QAX(unit)){ /* CUD only replication is not supported for QAX */
        for (i = 0; i < 12; ++i) {
            port_array[eg_reps] = BCM_GPORT_INVALID;
            cud_array[eg_reps++] = cuds_outlif_reps[i];
            if (SOC_IS_JERICHO(unit)) { /* delete the CUD to port mapping if it previously existed */
                REP_TEST_IF_ERR_FAIL(bcm_port_encap_map_set(unit, BCM_PORT_ENCAP_MAP_ALLOW_CORE_CHANGE, cuds_outlif_reps[i], BCM_GPORT_INVALID));
            }
            /* map the CUD to a port for using CUD only replications */
            REP_TEST_IF_ERR_FAIL(bcm_port_encap_map_set(unit, 0, cuds_outlif_reps[i], ports[i % ports_number]));
        }
        TEST_ASSERT(eg_reps == MY_NOF_EGRESS_GROUP_REPS);
    } else{
      TEST_ASSERT(eg_reps == QAX_MY_NOF_EGRESS_GROUP_REPS);
    }

    REP_TEST_IF_ERR_FAIL(test_bcm_mc_arad_group_create(unit, mcid, MC_EGRESS, may_fail));
    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_set(unit, mcid, eg_reps, port_array, cud_array, may_fail));
    TEST_PRINT((_BCM_MSG("created egress group 0x%x to be tested"), mcid));
    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_verify_all(unit)); 

    /* add ingress groups till MCDB is totally full */
    may_fail = TRUE;
    for (ing_groups = 0; !full; ++ing_groups) {
        bcm_gport_t ing_dest;
        BCM_GPORT_LOCAL_SET(ing_dest, ports[0]);
        REP_TEST_IF_ERR_FAIL(test_bcm_mc_arad_group_create(unit, ing_groups, MC_INGRESS, may_fail));
        if (!full) {
            may_fail = FALSE;
            REP_TEST_IF_ERR_FAIL(test_bcm_multicast_ingress_add(unit, ing_groups, ing_dest, 0xf372, may_fail));
            may_fail = TRUE;
            BCM_GPORT_UNICAST_QUEUE_GROUP_SET(ing_dest, 1024);
            REP_TEST_IF_ERR_FAIL(test_bcm_multicast_ingress_add(unit, ing_groups, ing_dest, 0x16e9, may_fail));
            if (SOC_IS_JERICHO(unit)) {
                REP_TEST_IF_ERR_FAIL(test_bcm_multicast_ingress_add(unit, ing_groups, ing_dest, 0x16ea, may_fail));
                REP_TEST_IF_ERR_FAIL(test_bcm_multicast_ingress_add(unit, ing_groups, ing_dest, 0x16eb, may_fail));
            }
            if (SOC_IS_QAX(unit) && !SOC_IS_QUX(unit)) /* QAX has 96K entries 16K taken for egress bitmap, and 16K ing groups by default. In order to feel the table with ing entries each group need to occupy minimum 5 entries */ {
                REP_TEST_IF_ERR_FAIL(test_bcm_multicast_ingress_add(unit, ing_groups, ing_dest, 0x16ec, may_fail));
                REP_TEST_IF_ERR_FAIL(test_bcm_multicast_ingress_add(unit, ing_groups, ing_dest, 0x16ed, may_fail));
                REP_TEST_IF_ERR_FAIL(test_bcm_multicast_ingress_add(unit, ing_groups, ing_dest, 0x16ee, may_fail));
                REP_TEST_IF_ERR_FAIL(test_bcm_multicast_ingress_add(unit, ing_groups, ing_dest, 0x16ef, may_fail));
                REP_TEST_IF_ERR_FAIL(test_bcm_multicast_ingress_add(unit, ing_groups, ing_dest, 0x16fc, may_fail));
            }
        } else {
          --ing_groups;
        }
    }
    TEST_PRINT((_BCM_MSG("created %d ingress groups"), ing_groups));
    if (SOC_IS_QAX(unit)) {
      /* QAX uses consecutive formats and therfore the precise number of the ing groups can not be predicted. We are enforcing 95 percent of maximum capacity */
      TEST_ASSERT(ing_groups < max_nof_ingr_groups_in_mcdb - 1 && ing_groups > (max_nof_ingr_groups_in_mcdb * 95 / 100) - QAX_MY_NOF_EGRESS_GROUP_REPS ); 
    } else {
      TEST_ASSERT(ing_groups < max_nof_ingr_groups_in_mcdb - 8 && ing_groups > max_nof_ingr_groups_in_mcdb - MY_NOF_EGRESS_GROUP_REPS); 
    }
        
    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_verify_all(unit)); 
    may_fail = FALSE;
    if (!SOC_IS_QAX(unit)){ /* CUD only replication is not supported for QAX */
      /* delete three entries in the consecutive entries block, where an entry can be deleted */
      BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_delete(unit, mcid, BCM_GPORT_INVALID, cuds_outlif_reps[5], may_fail));
      BCMDNX_IF_ERR_EXIT(test_bcm_multicast_verify_all(unit));
      BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_delete(unit, mcid, BCM_GPORT_INVALID, cuds_outlif_reps[9], may_fail));
      BCMDNX_IF_ERR_EXIT(test_bcm_multicast_verify_all(unit)); 
      BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_delete(unit, mcid, BCM_GPORT_INVALID, cuds_outlif_reps[1], may_fail));
      BCMDNX_IF_ERR_EXIT(test_bcm_multicast_verify_all(unit));
      BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_delete(unit, mcid, BCM_GPORT_INVALID, cuds_outlif_reps[0], may_fail));
      /* BCMDNX_IF_ERR_EXIT(test_bcm_multicast_verify_all(unit)); */
      BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_delete(unit, mcid, BCM_GPORT_INVALID, cuds_outlif_reps[4], may_fail));
      BCMDNX_IF_ERR_EXIT(test_bcm_multicast_verify_all(unit)); 
      BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_delete(unit, mcid, BCM_GPORT_INVALID, cuds_outlif_reps[8], may_fail));
      BCMDNX_IF_ERR_EXIT(test_bcm_multicast_verify_all(unit));
      BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_delete(unit, mcid, BCM_GPORT_INVALID, cuds_outlif_reps[10], may_fail));
      /* BCMDNX_IF_ERR_EXIT(test_bcm_multicast_verify_all(unit)); */
      BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_delete(unit, mcid, BCM_GPORT_INVALID, cuds_outlif_reps[2], may_fail));
      BCMDNX_IF_ERR_EXIT(test_bcm_multicast_verify_all(unit)); 
      BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_delete(unit, mcid, BCM_GPORT_INVALID, cuds_outlif_reps[6], may_fail));
      BCMDNX_IF_ERR_EXIT(test_bcm_multicast_verify_all(unit)); 

      BCM_GPORT_MCAST_SET(gport, 3);
      BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_delete(unit, mcid, gport, cuds_bitmap_reps[1], may_fail));
    }

    BCM_GPORT_LOCAL_SET(gport, ports[3]);
    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_delete(unit, mcid, gport, cuds_port_reps[3], may_fail));
    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_verify_all(unit)); 
    BCM_GPORT_LOCAL_SET(gport, ports[0]);
    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_delete(unit, mcid, gport, cuds_port_reps[3], may_fail));
    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_verify_all(unit));
    BCM_GPORT_LOCAL_SET(gport, ports[2]);
    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_delete(unit, mcid, gport, cuds_port_reps[3], may_fail));
    BCM_GPORT_LOCAL_SET(gport, ports[1]);
    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_delete(unit, mcid, gport, cuds_port_reps[3], may_fail));
    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_verify_all(unit));

    /* delete the first entry in the consecutive entries block, and a middle block */
    BCM_GPORT_LOCAL_SET(gport, ports[3]);
    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_delete(unit, mcid, gport, cuds_port_reps[1]+3, may_fail));
    BCM_GPORT_LOCAL_SET(gport, ports[2]);
    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_delete(unit, mcid, gport, cuds_port_reps[0]+2, may_fail));
    BCM_GPORT_LOCAL_SET(gport, ports[3]);
    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_delete(unit, mcid, gport, cuds_port_reps[0]+3, may_fail));
    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_verify_all(unit));
    BCM_GPORT_LOCAL_SET(gport, ports[2]);
    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_delete(unit, mcid, gport, cuds_port_reps[1]+2, may_fail));
    /* delete the entry before the last one in the consecutive entries block, and a middle block */
    BCM_GPORT_LOCAL_SET(gport, ports[1]);
    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_delete(unit, mcid, gport, cuds_port_reps[0]+1, may_fail));
    BCM_GPORT_LOCAL_SET(gport, ports[0]);
    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_delete(unit, mcid, gport, cuds_port_reps[0], may_fail));
    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_verify_all(unit));

    /* create an egress group with two entries */
    REP_TEST_IF_ERR_FAIL(test_bcm_mc_arad_group_create(unit, mcid - 1, MC_EGRESS, may_fail));
    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_set(unit, mcid - 1, 2, port_array + 4, cud_array + 4, may_fail));
    TEST_PRINT((_BCM_MSG("created egress group 0x%x to be tested"), mcid - 1));

    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_verify_all(unit));
    /* fill the table completely by adding replications to ingress group 0 */
    full = 0;
    BCM_GPORT_UNICAST_QUEUE_GROUP_SET(gport, 1024); 
    REP_TEST_IF_ERR_FAIL(test_bcm_mc_arad_group_create(unit, ing_groups, MC_INGRESS, may_fail));
    may_fail = TRUE;
    for (i = 0, full = 0; !full; ++i) {
        REP_TEST_IF_ERR_FAIL(test_bcm_multicast_ingress_add(unit, ing_groups, gport, 0xe100 + i, may_fail));
    }
    ing_groups++;
    may_fail = FALSE;
    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_verify_all(unit));
    TEST_PRINT((_BCM_MSG("created a 2 replications group and filled MCDB again")));
    TEST_ASSERT(i <= 9);

    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_delete(unit, mcid - 1, port_array[4], cud_array[4], may_fail));
    REP_TEST_IF_ERR_FAIL(test_bcm_mc_arad_group_create(unit, ing_groups, MC_INGRESS, may_fail));
    REP_TEST_IF_ERR_FAIL(test_bcm_multicast_ingress_add(unit, ing_groups, gport, 0xe100 + i, may_fail)); /* occupy the deleted entry */
    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_verify_all(unit));
    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_egress_delete(unit, mcid - 1, port_array[5], cud_array[5], may_fail));
    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_verify_all(unit));
    if (!SOC_IS_QAX(unit)) {
      may_fail = TRUE; 
      full = 0;
      REP_TEST_IF_ERR_FAIL(test_bcm_multicast_ingress_add(unit, ing_groups, gport, 0xe100 + ++i, may_fail)); /* occupy the deleted entry */
      BCMDNX_IF_ERR_EXIT(test_bcm_multicast_verify_all(unit));
    }
    if (SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit)) {
        TEST_ASSERT(!full);
    } else {
        TEST_ASSERT(full);
    }
    may_fail = FALSE;

    ing_mc_groups_created = ing_groups; 
    /* Destroy ingress groups */
    while (ing_groups > 0) {
        --ing_groups;
        BCMDNX_IF_ERR_EXIT(test_bcm_multicast_destroy(unit, ing_groups));
    }
    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_verify_all(unit));
    if (ing_mc_groups_created <= mcid) {
        BCMDNX_IF_ERR_EXIT(test_bcm_multicast_destroy(unit, mcid)); 
    }
    if (ing_mc_groups_created <= mcid -1) {
        BCMDNX_IF_ERR_EXIT(test_bcm_multicast_destroy(unit, mcid - 1));
    }
    BCMDNX_IF_ERR_EXIT(test_bcm_multicast_verify_all(unit));

    if (SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit)) { /* delete the CUD to port mapping */
        for (i = 0 ; i < 12; ++i) {
            REP_TEST_IF_ERR_FAIL(bcm_port_encap_map_set(unit, 0, cuds_outlif_reps[i], BCM_GPORT_INVALID));
        }
    }

exit:
    if (initialized) {
        test_bcm_multicast_detach_dirty(unit);
    }
    BCMDNX_FUNC_RETURN;
}


#endif /* BCM_PETRA_SUPPORT */

#endif /* defined(BCM_PETRA_SUPPORT) || defined(BCM_88750_A0) */
