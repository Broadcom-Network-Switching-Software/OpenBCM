
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <soc/dnxc/swstate/sw_state_features.h>
#include <soc/dnxc/swstate/auto_generated/access/dnxc_module_ids_access.h>
#include <soc/dnxc/swstate/dnxc_sw_state_verifications.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <soc/dnxc/multithread_analyzer.h>
#include <soc/dnxc/drv_dnxc_utils.h>



#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOCDNX_SWSTATEDNX

#ifdef DNX_SW_STATE_VERIFICATIONS

#include <sal/appl/sal.h>
#include <soc/drv.h>
#include <soc/types.h>
#include <soc/error.h>
#include <soc/dnxc/dnxc_ha.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/dnxc_sw_state.h>
#include <soc/dnxc/swstate/dnxc_sw_state_verifications.h>
#include <soc/dnxc/swstate/dnxc_sw_state_dispatcher.h>

#define DNX_SW_STATE_VERF_MATRIX_GENERALLY      (1 << 0)
#define DNX_SW_STATE_VERF_MATRIX_AFTER_INIT     (1 << 1)
#define DNX_SW_STATE_VERF_MATRIX_DEINIT         (1 << 2)
#define DNX_SW_STATE_VERF_MATRIX_WB             (1 << 3)

#define SW_STATE_VERF_MATRIX_INIT(func_id, generally, init, deinit, wb)\
    dnxc_sw_state_verification_mgr[unit].matrix[func_id] =             \
        (generally ? DNX_SW_STATE_VERF_MATRIX_GENERALLY : 0) |         \
        (init ? DNX_SW_STATE_VERF_MATRIX_AFTER_INIT : 0) |             \
        (deinit ? DNX_SW_STATE_VERF_MATRIX_DEINIT : 0) |               \
        (wb ? DNX_SW_STATE_VERF_MATRIX_WB : 0)

#define SW_STATE_VERF_MATRIX_CHECK(condition)\
    (dnxc_sw_state_verification_mgr[unit].matrix[func_type] & condition)

typedef struct dnxc_sw_state_verification_mgr_d {
    uint8 is_assert_disabled;
    uint8 matrix[SW_STATE_FUNC_NOF];
} dnxc_sw_state_verification_mgr_t;

dnxc_sw_state_verification_mgr_t dnxc_sw_state_verification_mgr[SOC_MAX_NUM_DEVICES] = {{0}};


int dnxc_verifications_func_call_allowed(
    int unit,
    uint32 module,
    dnxc_sw_state_function_types_e func_type,
    uint32 flags)
{

#if defined(DNXC_MTA_ENABLED)
    uint8 is_write = 1;
#endif

    DNXC_SW_STATE_INIT_FUNC_DEFS;

     
    if (module == EXAMPLE_MODULE_ID) {
        
        SHR_EXIT();
    }

    if (module == MULTITHREAD_ANALYZER_MODULE_ID) {
         
        SHR_EXIT();
    }

    if (!dnxc_ha_is_access_disabled(unit,UTILEX_SEQ_ALLOW_SW_STATE)) {
        
        SHR_EXIT();
    }

    
    if (SW_STATE_VERF_MATRIX_CHECK(DNX_SW_STATE_VERF_MATRIX_DEINIT) &&
        SW_STATE_VERF_MATRIX_CHECK(DNX_SW_STATE_VERF_MATRIX_WB) &&
        SW_STATE_VERF_MATRIX_CHECK(DNX_SW_STATE_VERF_MATRIX_AFTER_INIT) &&
        SW_STATE_VERF_MATRIX_CHECK(DNX_SW_STATE_VERF_MATRIX_GENERALLY)) {
        DNXC_MTA(is_write = 0);
    }
    DNXC_MTA(SHR_IF_ERR_EXIT(dnxc_multithread_analyzer_log_resource_use(unit, MTA_RESOURCE_SW_STATE, module, is_write)));

    
    if (_SHR_IS_FLAG_SET(flags, DNXC_SW_STATE_ALLOW_WRITES_DURING_ALL))
    {
        SHR_EXIT();
    }

    if (SOC_IS_DETACHING(unit) &&
        !SW_STATE_VERF_MATRIX_CHECK(DNX_SW_STATE_VERF_MATRIX_DEINIT)) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                 "Trying to modify state during deinit\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }

    if ((SOC_WARM_BOOT(unit) &&
        !SW_STATE_VERF_MATRIX_CHECK(DNX_SW_STATE_VERF_MATRIX_WB)) &&
        !_SHR_IS_FLAG_SET(flags, DNXC_SW_STATE_ALLOW_WRITES_DURING_WB)) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                 "Trying to modify state during wb\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }

    if (SOC_IS_DONE_INIT(unit) &&
        !dnxc_sw_state_alloc_during_test_get(unit) &&
        !SW_STATE_VERF_MATRIX_CHECK(DNX_SW_STATE_VERF_MATRIX_AFTER_INIT)) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                 "Trying to create new state after init\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }

    if (!SW_STATE_VERF_MATRIX_CHECK(DNX_SW_STATE_VERF_MATRIX_GENERALLY)) {

        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                             "This sw state function does not exist or is not allowed\n%s%s%s",
                             EMPTY, EMPTY, EMPTY);
    }

    DNXC_SW_STATE_FUNC_RETURN;
}


int dnxc_verifications_module_is_init(
    int unit,
    uint32 module_id,
    void* module_ptr)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (!sw_state_roots_array[unit]) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                 "Trying to access SW State module although sw state was not initialized for the unit (module name=%s id=%d)\n%s",
                                 dnxc_module_id_e_get_name(module_id), module_id, EMPTY);
    }

    if (!module_ptr) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                 "Trying to access SW State module that was'nt initialized (module name=%s id=%d)\n%s",
                                 dnxc_module_id_e_get_name(module_id), module_id, EMPTY);
    }

    DNXC_SW_STATE_FUNC_RETURN;
}


int dnxc_verifications_null_check(
    int unit,
    uint32 module_id,
    void* ptr)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (!ptr) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                 "sw state ERROR: trying to access null pointer (module_id=%d)\n%s%s",
                                 module_id, EMPTY, EMPTY);
    }

    DNXC_SW_STATE_FUNC_RETURN;
}

int dnxc_verifications_family(
    int unit,
    uint32 module_id,
    soc_is_device_e soc_device)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if(!soc_is(unit, soc_device))
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,"sw state ERROR: Trying to access %s only parameter while the unit is not from the same family(module_id=%d)\n%s",
                                         "dnx2", module_id,  EMPTY);
    }

    DNXC_SW_STATE_FUNC_RETURN;
}


int dnxc_sw_state_verifications_deinit(
    int unit){

    int index = 0;

    for (index = 0; index < SW_STATE_FUNC_NOF; index++) {
        SW_STATE_VERF_MATRIX_INIT(index, 0, 0, 0, 0);
    }

    return SOC_E_NONE;
}


int dnxc_sw_state_verifications_init(
    int unit){

    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_VERIFY,                     1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_CREATE,                     1,    0,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_DESTROY,                    0,    0,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_GET_NEXT,                   1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_GET_NEXT_IN_RANGE,          1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_STATUS_SET,                 1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_IS_OCCUPIED,                1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_ALLOC_NEXT,                 1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_CLEAR,                      1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PRINT,                      1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_FIND,                       1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_INSERT,                     1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_DELETE,                     1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_DELETE_ALL,                 1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_ITERATE,                    1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_CREATE_EMPTY,               1,    0,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_NOF_ELEMENTS,               1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_NODE_KEY,                   1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_NODE_VALUE,                 1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_NODE_UPDATE,                1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_NEXT_NODE,                  1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PREVIOUS_NODE,              1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_ADD,                        1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_ADD_FIRST,                  1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_ADD_LAST,                   1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_ADD_BEFORE,                 1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_ADD_AFTER,                  1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_REMOVE_NODE,                1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_GET_LAST,                   1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_GET_FIRST,                  1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_GET_ROOT,                   1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_GET_PARENT,                 1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_GET_LEFT_CHILD,             1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_GET_RIGHT_CHILD,            1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_ADD_ROOT,                   1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_ADD_LEFT_CHILD,             1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_ADD_RIGHT_CHILD,            1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_SET_PARENT,                 1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_SET_LEFT_CHILD,             1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_SET_RIGHT_CHILD,            1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_NODE_FREE,                  1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PIECE_ALLOC,                1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PIECE_FREE,                 1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_TAKE,                       1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_GIVE,                       1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_ALLOC,                      1,    0,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PTR_ALLOC,                  1,    0,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_FREE,                       1,    0,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PTR_FREE,                   1,    0,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_IS_ALLOCATED,               1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_SET,                        1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_GET,                        1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_INC,                        1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_DEC,                        1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_RANGE_READ,                 1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_RANGE_WRITE,                1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_RANGE_FILL,                 1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_ALLOC_BITMAP,               1,    0,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_BIT_SET,                    1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_BIT_CLEAR,                  1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_BIT_GET,                    1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_BIT_RANGE_READ,             1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_BIT_RANGE_WRITE,            1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_BIT_RANGE_AND,              1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_BIT_RANGE_OR,               1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_BIT_RANGE_XOR,              1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_BIT_RANGE_REMOVE,           1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_BIT_RANGE_NEGATE,           1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_BIT_RANGE_CLEAR,            1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_BIT_RANGE_SET,              1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_BIT_RANGE_NULL,             1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_BIT_RANGE_TEST,             1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_BIT_RANGE_EQ,               1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_BIT_RANGE_COUNT,            1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_MEMREAD,                    1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_MEMWRITE,                   1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_MEMCMP,                     1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_MEMSET,                     1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PBMP_NEQ,                   1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PBMP_EQ,                    1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PBMP_MEMBER,                1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PBMP_NOT_NULL,              1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PBMP_IS_NULL,               1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PBMP_COUNT,                 1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PBMP_XOR,                   1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PBMP_REMOVE,                1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PBMP_ASSIGN,                1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PBMP_GET,                   1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PBMP_AND,                   1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PBMP_NEGATE,                1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PBMP_OR,                    1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PBMP_CLEAR,                 1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PBMP_PORT_ADD,              1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PBMP_PORT_FLIP,             1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PBMP_PORT_REMOVE,           1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PBMP_PORT_SET,              1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PBMP_PORTS_RANGE_ADD,       1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PBMP_FMT,                   1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_DUMP,                       1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_INIT,                       1,    0,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_DEINIT,                     0,    0,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_IS_INIT,                    1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_GET_NAME,                   1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_GET_CB,                     1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_REGISTER_CB,                1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_UNREGISTER_CB,              1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_REMOVE_BY_INDEX_MULTIPLE,   1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_REMOVE_BY_INDEX,            1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_REMOVE,                     1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_INSERT_AT_INDEX,            1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_GET_BY_INDEX,               1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_DELETE_BY_INDEX,            1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_ADD_AT_INDEX_MULTIPLE,      1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_ADD_AT_INDEX,               1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_CHECK_ALL,                  1,    1,    1,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_CHECK_ALL_TAG,              1,    1,    1,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_CHECK,                      1,    1,    1,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_TAG_GET,                    1,    1,    1,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_TAG_SET,                    1,    1,    1,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_NOF_FREE_ELEMENTS_GET,      1,    1,    1,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_NOF_USED_ELEMENTS_IN_GRAIN_GET,         1,    1,    1,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_ALLOCATE_SINGLE,                        1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_ALLOCATE_SEVERAL,                       1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_FREE_SINGLE,                            1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_FREE_SEVERAL,                           1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_DUMP_INFO_GET,                          1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_CLEAR_ALL,                              1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_BITMAP_ALLOC,                           1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_BITMAP_FREE,                            1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_EXCHANGE,                               1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_REPLACE_DATA,                           1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PROFILE_DATA_GET,                       1,    1,    1,    1);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_PROFILE_GET,                            1,    1,    1,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_FREE_ALL,                               1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_CREATE_INFO_GET,                        1,    1,    1,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_STRINGNCAT,                             1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_STRINGNCMP,                             1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_STRINGNCPY,                             1,    1,    0,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_STRINGLEN,                              1,    1,    1,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_STRINGGET,                              1,    1,    1,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_NOF_ALLOCATED_ELEMENTS_IN_RANGE_GET,    1,    1,    1,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_ADVANCED_ALGORITHM_INFO_SET,                 1,    1,    1,    0);
    SW_STATE_VERF_MATRIX_INIT(SW_STATE_FUNC_ADVANCED_ALGORITHM_INFO_GET,                 1,    1,    1,    0);
    return SOC_E_NONE;
}


int dnx_sw_state_oob_static_buffer_check(
    int unit,
    uint32 idx,
    uint32 arr_size)
{
    if((idx >= (arr_size)) || (idx < 0))
    {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE, (BSL_META_U(unit, "sw state ERROR: value %u is outside of STATIC BUFFER limit %u.\n"), (idx), (arr_size)));
        return _SHR_E_INTERNAL;
    }
    return _SHR_E_NONE;
}


int dnx_sw_state_oob_static_bitmap_check(
    int unit,
    uint32 idx,
    uint32 arr_size)
{
    if((idx >= ((arr_size)*SHR_BITWID)) || (idx < 0))
    {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE, (BSL_META_U(unit, "sw state ERROR: value %u is outside of STATIC BITMAP limits.\n"), (uint32)(idx)));
        return _SHR_E_INTERNAL;
    }
    return _SHR_E_NONE;
}


int dnx_sw_state_oob_static_array_check(
    int unit,
    uint32 idx,
    uint32 arr_size)
{
    if((idx >= (arr_size)) || (idx < 0)){
        LOG_ERROR(BSL_LS_SHARED_SWSTATE, (BSL_META_U(unit, "sw state ERROR: value %u is outside of STATIC ARRAY limit %u.\n"), idx, arr_size));
        return _SHR_E_INTERNAL;
    }
    return _SHR_E_NONE;
}


int dnx_sw_state_oob_dynamic_array_check(
    int unit,
    uint32 module_id,
    int idx,
    void *ptr,
    uint32 type_size)
{
    uint32 size = 0;
    uint32 nof_elements_retrieved = 0;
    uint32 element_size_retrieved = 0;
    uint32 nof_elements = 0;

    if(DNXC_SW_STATE_ALLOC_SIZE(unit, module_id, (uint8 *)ptr, &nof_elements_retrieved, &element_size_retrieved) != SOC_E_NONE)
    {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META_U(unit, "sw state OUT OF BOUND ERROR: Failed to retrieve size\n")));
        return SOC_E_INTERNAL;
    }

    size = nof_elements_retrieved * element_size_retrieved;
    
    nof_elements = (size / type_size);

    if ((idx < 0) || (idx >= nof_elements)) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META_U(unit, "sw state OUT OF BOUND ERROR: size - %d. nof_elements - %d, idx - %d \n"), size,
                   nof_elements, idx));
        return SOC_E_INTERNAL;
    }

    return SOC_E_NONE;
}


int dnx_sw_state_oob_dynamic_buffer_check(
    int unit,
    uint32 module_id,
    uint32 idx,
    void *ptr_void,
    uint32 type_size)
{
    int rv;

    rv = dnx_sw_state_oob_dynamic_array_check(unit, module_id, idx, ptr_void, type_size);
    if(rv != SOC_E_NONE)
    {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE, (BSL_META_U(unit, "sw state ERROR: value %u is outside of DYNAMIC BUFFER limits.\n"), idx));
        return SOC_E_INTERNAL;
    }
    return SOC_E_NONE;
}


int dnx_sw_state_oob_dynamic_bitmap_check(
    int unit,
    uint32 module_id,
    int bit_idx,
    void *ptr)
{
    uint32 nof_bits = 0;
    uint32 nof_elements_retrieved = 0;
    uint32 element_size_retrieved = 0;

    
    if(DNXC_SW_STATE_ALLOC_SIZE(unit, module_id, (uint8 *)ptr, &nof_elements_retrieved, &element_size_retrieved) != SOC_E_NONE)
    {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META_U(unit, "sw state OUT OF BOUND ERROR: Failed to retrieved size\n")));
        return SOC_E_INTERNAL;
    }

    nof_bits = (nof_elements_retrieved * element_size_retrieved * SHR_BITWID) / sizeof(SHR_BITDCL);

    
    if((bit_idx < 0) || (bit_idx >= nof_bits)) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META_U(unit, "sw state OUT OF BOUND ERROR: nof_bits - %d, idx - %d \n"),
                   nof_bits, bit_idx));
        return SOC_E_INTERNAL;
    }

    return SOC_E_NONE;
}


STATIC int
dnxc_sw_state_size_verify(
    int unit,
    uint32 alloc_size,
    uint32 flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    
    if (!(sw_state_is_flag_on(flags, DNXC_SW_STATE_DNX_DATA_ALLOC)) &&
        !(sw_state_is_flag_on(flags, DNXC_SW_STATE_DNX_DATA_ALLOC_EXCEPTION)) &&
        !(sw_state_is_flag_on(flags, DNXC_SW_STATE_JOURNAL_ROLLING_BACK)) &&
        (sw_state_is_flag_on(flags, DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION)) &&
        (alloc_size > DNXC_SW_STATE_ALLOC_MAX_BYTES))
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                 "sw state ERROR: size allocated is bigger than permitted by DNXC_SW_STATE_ALLOC_MAX_BYTES \n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }

    DNXC_SW_STATE_FUNC_RETURN;
}


int
dnxc_sw_state_alloc_verify_common(
    int unit,
    uint32 module_id,
    uint8 **ptr_location,
    uint32 nof_elements,
    uint32 element_size,
    uint32 flags,
    char *dbg_string)
{
    uint32 size = 0;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_NULL_CHECK(ptr_location, SOC_E_INTERNAL, "sw state alloc ERROR: ptr_location can't be NULL");
    SHR_RANGE_VERIFY(nof_elements, 0, DNXC_SW_STATE_MAX_NOF_ELEMENTS_IN_ARRAY, SOC_E_INTERNAL, "sw state alloc ERROR: nof_elements not in range");
    SHR_RANGE_VERIFY(element_size, 0, DNXC_SW_STATE_MAX_ARRAY_ENTRY_SIZE, SOC_E_INTERNAL, "sw state alloc ERROR: element_size not in range");

    if (!(dnxc_sw_state_alloc_during_test_get(unit)
          || !(SOC_IS_DONE_INIT(unit))
          || (sw_state_is_flag_on(flags, DNXC_SW_STATE_ALLOC_AFTER_INIT_EXCEPTION))
          || (sw_state_is_flag_on(flags, DNXC_SW_STATE_JOURNAL_ROLLING_BACK))))
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state ERROR: trying to allocate data after init\n%s%s%s", EMPTY,
                                 EMPTY, EMPTY);
    }

    size = nof_elements * element_size;
    
    if (nof_elements > 1)
    {
        SHR_IF_ERR_EXIT(dnxc_sw_state_size_verify(unit, size, flags));
    }

    
    if (NULL != *ptr_location)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                 "sw state ERROR: double allocation attempted. Allocation can be performed on NULL pointers only. \n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }

    DNXC_SW_STATE_FUNC_RETURN;
}


int
dnxc_sw_state_free_verify_common(
    int unit,
    uint32 module_id,
    uint8 **ptr_location,
    uint32 flags,
    char *dbg_string)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_NULL_CHECK(ptr_location, SOC_E_INTERNAL, "sw state free ERROR: ptr is an invalid pointer");

    DNXC_SW_STATE_FUNC_RETURN;
}


int
dnxc_sw_state_alloc_size_verify_common(
    int unit,
    uint32 module,
    uint8 *location,
    uint32 *nof_elements,
    uint32 *element_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_NULL_CHECK(location,     SOC_E_INTERNAL, "sw state ERROR: invalid parameter: location");
    SHR_NULL_CHECK(nof_elements, SOC_E_INTERNAL, "sw state ERROR: invalid parameter: nof_elements");
    SHR_NULL_CHECK(element_size, SOC_E_INTERNAL, "sw state ERROR: invalid parameter: element_size");

    DNXC_SW_STATE_FUNC_RETURN;
}


int
dnxc_sw_state_memcpy_verify_common(
    int unit,
    uint32 module_id,
    uint32 size,
    uint8 *dest,
    uint8 *src,
    uint32 flags,
    char *dbg_string)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_NULL_CHECK(dest, SOC_E_INTERNAL, "sw state ERROR: invalid parameter: dest");
    SHR_NULL_CHECK(src,  SOC_E_INTERNAL, "sw state ERROR: invalid parameter: src");

    DNXC_SW_STATE_FUNC_RETURN;
}


int
dnxc_sw_state_memset_verify_common(
    int unit,
    uint32 module_id,
    uint8 *dest,
    uint32 value,
    uint32 size,
    uint32 flags,
    char *dbg_string)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_NULL_CHECK(dest, SOC_E_INTERNAL, "sw state ERROR: invalid parameter: dest");

    DNXC_SW_STATE_FUNC_RETURN;
}

void dnxc_sw_state_err_assert_fail_debug_hook(
    int unit)
{
    LOG_ERROR(BSL_LS_SHARED_SWSTATE,
            (BSL_META_U(unit, "sw state ASSERT FAILURE: asserting to avoid error drop, please refer to error message above for the original error\n")));

    return;
}

int
dnxc_sw_state_err_assert_is_disabled(
    int unit)
{
    return (dnxc_sw_state_verification_mgr[unit].is_assert_disabled) ? TRUE : FALSE;
}

int
dnxc_sw_state_err_assert_state_change(
    int unit,
    uint8 is_disabled)
{
    SHR_FUNC_INIT_VARS(unit);

    dnxc_sw_state_verification_mgr[unit].is_assert_disabled = is_disabled;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

#else



shr_error_e dnx_sw_state_verification_dummy_func_to_make_compiler_happy(int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}


#endif 

#undef _ERR_MSG_MODULE_NAME
