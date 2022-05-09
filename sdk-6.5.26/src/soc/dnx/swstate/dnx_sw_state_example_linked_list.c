
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#if defined(INCLUDE_CTEST)

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnx/swstate/auto_generated/access/example_access.h>

static int
dnx_sw_state_ll_init(
    int unit)
{
    sw_state_ll_init_info_t init_info;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&init_info, 0, sizeof(sw_state_ll_init_info_t));
    init_info.max_nof_elements = 10;

    SHR_IF_ERR_EXIT(example.ll.create_empty(unit, &init_info));

exit:
    SHR_FUNC_EXIT;
}

static int
dnx_sw_state_multihead_ll_init(
    int unit)
{
    sw_state_ll_init_info_t init_info;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&init_info, 0, sizeof(sw_state_ll_init_info_t));
    init_info.max_nof_elements = 15;
    init_info.nof_heads = 2;

    SHR_IF_ERR_EXIT(example.multihead_ll.create_empty(unit, &init_info));

exit:
    SHR_FUNC_EXIT;
}

static int
dnx_sw_state_sorted_ll_init(
    int unit)
{
    sw_state_ll_init_info_t init_info;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&init_info, 0, sizeof(sw_state_ll_init_info_t));
    init_info.max_nof_elements = 12;
    init_info.key_cmp_cb_name = "sw_state_sorted_list_cmp_uint8";

    SHR_IF_ERR_EXIT(example.sorted_ll.create_empty(unit, &init_info));
exit:
    SHR_FUNC_EXIT;
}

static int
dnx_sw_state_sorted_multihead_ll_init(
    int unit)
{
    sw_state_ll_init_info_t init_info;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&init_info, 0, sizeof(sw_state_ll_init_info_t));
    init_info.max_nof_elements = 12;
    init_info.key_cmp_cb_name = "sw_state_sorted_list_cmp_uint32";
    init_info.nof_heads = 3;

    LOG_CLI((BSL_META("Init the sorted multihead linked list\n")));
    SHR_IF_ERR_EXIT(example.sorted_multihead_ll.create_empty(unit, &init_info));

exit:
    SHR_FUNC_EXIT;
}

int
dnx_sw_state_example_linked_list_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(example.init(unit));

    SHR_IF_ERR_EXIT(dnx_sw_state_ll_init(unit));

    SHR_IF_ERR_EXIT(dnx_sw_state_multihead_ll_init(unit));

    SHR_IF_ERR_EXIT(dnx_sw_state_sorted_ll_init(unit));

    SHR_IF_ERR_EXIT(dnx_sw_state_sorted_multihead_ll_init(unit));

exit:
    SHR_FUNC_EXIT;
}

#undef _ERR_MSG_MODULE_NAME

#else
typedef int make_iso_compilers_happy;

#endif
