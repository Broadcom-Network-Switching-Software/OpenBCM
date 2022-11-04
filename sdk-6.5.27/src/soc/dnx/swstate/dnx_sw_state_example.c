
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#if defined(INCLUDE_CTEST)

#include <soc/sand/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/init/init.h>
#include <soc/dnx/swstate/auto_generated/access/example_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_dev_init.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_infra_tests.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

static int
dnx_sw_state_dnxdata_init(
    int unit)
{
    uint32 nof_elements = 30000;
    uint32 nof_elements_small = 3;
    uint32 idx1 = 0;

    SHR_FUNC_INIT_VARS(unit);

    LOG_CLI((BSL_META("Allocate the DNXData_array\n")));
    SHR_IF_ERR_EXIT(example.DNXData_array.alloc(unit));

    LOG_CLI((BSL_META("Allocate memory for the DNXData_array_table_data\n")));
    SHR_IF_ERR_EXIT(example.DNXData_array_table_data.alloc(unit));

    LOG_CLI((BSL_META("Allocate memory for the DNXData_array_alloc_exception.\n")));
    SHR_IF_ERR_EXIT(example.DNXData_array_alloc_exception.alloc(unit, (int) nof_elements));

    LOG_CLI((BSL_META("Allocate memory for two-dimensional array : [?][DNX_DATA].\n")));
    example.DNXData_array_dyn_dnxdata.alloc(unit, nof_elements_small);

    LOG_CLI((BSL_META("Allocate memory for two-dimensional array : [DNX_DATA][?].\n")));
    example.DNXData_array_dnxdata_dyn.alloc(unit, nof_elements_small);

    LOG_CLI((BSL_META("Allocate memory for two-dimensional array : [DNX_DATA][DNX_DATA].\n")));
    example.DNXData_array_dnxdata_dnxdata.alloc(unit);

    LOG_CLI((BSL_META
             ("Allocate memory for three-dimensional array : [3][DNX_DATA.module_testing.dbal.vrf_field_size][?].\n")));
    for (idx1 = 0; idx1 < 3; idx1++)
    {
        example.DNXData_array_static_dnxdata_dyn.alloc(unit, (int) idx1, nof_elements_small);
    }

    LOG_CLI((BSL_META
             ("Allocate memory for three-dimensional array : [3][?][DNX_DATA.module_testing.dbal.vrf_field_size].\n")));
    for (idx1 = 0; idx1 < 3; idx1++)
    {
        example.DNXData_array_static_dyn_dnxdata.alloc(unit, (int) idx1, nof_elements_small);
    }

    LOG_CLI((BSL_META
             ("Allocate memory for three-dimensional array : [3][DNX_DATA.module_testing.dbal.vrf_field_size][DNX_DATA.module_testing.dbal.vrf_field_size].\n")));
    for (idx1 = 0; idx1 < 3; idx1++)
    {
        example.DNXData_array_static_dnxdata_dnxdata.alloc(unit, (int) idx1);
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static int
dnx_sw_state_arrays_init(
    int unit)
{
    uint32 nof_elements = 10;

    SHR_FUNC_INIT_VARS(unit);

    LOG_CLI((BSL_META("Allocating dynamic array of size 0.\n")));
    SHR_IF_ERR_EXIT(example.dynamic_array_zero_size.alloc(unit, 0));

    LOG_CLI((BSL_META("Allocate the dynamic_array array.\n")));
    SHR_IF_ERR_EXIT(example.dynamic_array.alloc(unit, nof_elements));

    LOG_CLI((BSL_META("Allocate the dynamic_array_static array.\n")));
    SHR_IF_ERR_EXIT(example.dynamic_array_static.alloc(unit, nof_elements));

    LOG_CLI((BSL_META("Allocate the dynamic_dynamic_array array.\n")));
    SHR_IF_ERR_EXIT(example.dynamic_dynamic_array.alloc(unit, nof_elements, nof_elements));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static int
dnx_sw_state_bitmap_init(
    int unit)
{
    uint32 nof_bits_to_alloc = 32;

    SHR_FUNC_INIT_VARS(unit);

    LOG_CLI((BSL_META("Allocate the memory for the dnx data bitmap variable.\n")));
    SHR_IF_ERR_EXIT(example.bitmap_dnx_data.alloc_bitmap(unit));

    LOG_CLI((BSL_META("Allocate the memory for the bitmap variable.\n")));
    SHR_IF_ERR_EXIT(example.bitmap_variable.alloc_bitmap(unit, nof_bits_to_alloc));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static int
dnx_sw_state_default_value_init(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);

    LOG_CLI((BSL_META("alloc memory for DNXdata array\n")));
    SHR_IF_ERR_EXIT(example.default_value_tree.array_dnxdata.alloc(unit));

    LOG_CLI((BSL_META("alloc memory for dynamic array\n")));
    SHR_IF_ERR_EXIT(example.default_value_tree.array_dnxdata.array_dynamic.alloc(unit, 0, 5));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static int
dnx_sw_state_occupation_bitmap_init(
    int unit)
{
    sw_state_occ_bitmap_init_info_t btmp_init_info;
    uint32 num_bits = 64;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sw_state_occ_bm_init_info_clear(&btmp_init_info));

    btmp_init_info.size = num_bits;

    SHR_IF_ERR_EXIT(example.occ.create(unit, &btmp_init_info));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static int
dnx_sw_state_occupation2_bitmap_init(
    int unit)
{
    sw_state_occ_bitmap_init_info_t btmp_init_info;
    uint32 num_bits = 700;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sw_state_occ_bm_init_info_clear(&btmp_init_info));

    btmp_init_info.size = num_bits;

    SHR_IF_ERR_EXIT(example.occ2.create(unit, &btmp_init_info));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static int
dnx_sw_state_bt_init(
    int unit)
{
    sw_state_bt_init_info_t init_info;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&init_info, 0, sizeof(sw_state_bt_init_info_t));
    init_info.max_nof_elements = 10;
    init_info.expected_nof_elements = 10;

    LOG_CLI((BSL_META("init the binary tree\n")));
    SHR_IF_ERR_EXIT(example.bt.create_empty(unit, &init_info));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static int
dnx_sw_state_htb_init(
    int unit)
{
    sw_state_htbl_init_info_t init_info;

    SHR_FUNC_INIT_VARS(unit);

    init_info.max_nof_elements = dnx_data_infra_tests.sw_state.htb_max_nof_elements_get(unit);
    init_info.expected_nof_elements = dnx_data_infra_tests.sw_state.htb_max_nof_elements_get(unit);
    init_info.hash_function = NULL;
    init_info.print_cb_name = "sw_state_htb_example_entry_print";

    LOG_CLI((BSL_META("Init the hash table\n")));
    SHR_IF_ERR_EXIT(example.htb.create(unit, 0, &init_info));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static int
dnx_sw_state_htb_rh_init(
    int unit)
{
    sw_state_htb_create_info_t init_info;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&init_info, 0, sizeof(sw_state_htb_create_info_t));

    init_info.max_nof_elements = dnx_data_infra_tests.sw_state.htb_max_nof_elements_get(unit);
    init_info.hash_func = dnx_data_infra_tests.sw_state.htb_rh_hashing_algorithm_get(unit);
    init_info.flags = dnx_data_infra_tests.sw_state.htb_rh_create_flags_get(unit);
    init_info.print_cb_name = NULL;
    init_info.data_size = sizeof(int);
    LOG_CLI((BSL_META("Init the hash table\n")));
    SHR_IF_ERR_EXIT(example.htb_rh.create(unit, &init_info));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static int
dnx_sw_state_multihead_htb_rh_init(
    int unit)
{
    sw_state_htb_create_info_t init_info;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&init_info, 0, sizeof(sw_state_htb_create_info_t));

    init_info.max_nof_elements = dnx_data_infra_tests.sw_state.htb_max_nof_elements_get(unit);
    init_info.hash_func = dnx_data_infra_tests.sw_state.htb_rh_hashing_algorithm_get(unit);
    init_info.flags = dnx_data_infra_tests.sw_state.htb_rh_create_flags_get(unit);
    init_info.nof_htbs = 255;
    init_info.print_cb_name = NULL;
    LOG_CLI((BSL_META("Init the hash table\n")));
    SHR_IF_ERR_EXIT(example.multihead_htb_rh.create(unit, &init_info, sizeof(int), sizeof(int)));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static int
dnx_sw_state_index_htb_rh_init(
    int unit)
{
    sw_state_htb_create_info_t init_info;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&init_info, 0, sizeof(sw_state_htb_create_info_t));

    init_info.max_nof_elements = dnx_data_infra_tests.sw_state.htb_max_nof_elements_get(unit);
    init_info.hash_func = dnx_data_infra_tests.sw_state.htb_rh_hashing_algorithm_get(unit);
    init_info.flags = dnx_data_infra_tests.sw_state.htb_rh_create_flags_get(unit);
    init_info.print_cb_name = NULL;
    LOG_CLI((BSL_META("Init the hash table\n")));
    SHR_IF_ERR_EXIT(example.index_htb_rh.create(unit, &init_info));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static int
dnx_sw_state_htb_arr_init(
    int unit)
{
    sw_state_htbl_init_info_t init_info0, init_info1;

    SHR_FUNC_INIT_VARS(unit);

    init_info0.max_nof_elements = dnx_data_infra_tests.sw_state.htb_max_nof_elements_get(unit);
    init_info0.expected_nof_elements = dnx_data_infra_tests.sw_state.htb_max_nof_elements_get(unit);

    init_info0.hash_function = NULL;
    init_info0.print_cb_name = "sw_state_htb_example_entry_print";

    init_info1.max_nof_elements = dnx_data_infra_tests.sw_state.htb_max_nof_elements_get(unit);
    init_info1.expected_nof_elements = dnx_data_infra_tests.sw_state.htb_max_nof_elements_get(unit);
    init_info1.hash_function = NULL;
    init_info1.print_cb_name = NULL;

    LOG_CLI((BSL_META("Init the hash tables\n")));
    SHR_IF_ERR_EXIT(example.htb_arr.create(unit, 0, &init_info0, sizeof(int), sizeof(int)));
    SHR_IF_ERR_EXIT(example.htb_arr.create(unit, 1, &init_info1, sizeof(char), sizeof(char)));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static int
dnx_sw_state_index_htb_init(
    int unit)
{
    sw_state_idx_htbl_init_info_t init_info;

    SHR_FUNC_INIT_VARS(unit);

    init_info.max_nof_elements = dnx_data_infra_tests.sw_state.htb_max_nof_elements_index_get(unit);
    init_info.expected_nof_elements = dnx_data_infra_tests.sw_state.htb_max_nof_elements_index_get(unit);
    init_info.hash_function = NULL;
    init_info.print_cb_name = "sw_state_htb_example_entry_print";

    LOG_CLI((BSL_META("Init the index hash table\n")));
    SHR_IF_ERR_EXIT(example.index_htb.create(unit, &init_info));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static int
dnx_sw_state_defragmented_chunk_init(
    int unit)
{
    sw_state_defragmented_chunk_init_info_t init_info;
    int def_cunk_index_part1 = 0;
    int def_cunk_index_part2 = 1;
    int def_cunk_index_part3 = 2;
    int def_cunk_index_part4 = 3;
    int def_cunk_index_part5 = 4;
    int def_cunk_index_part6 = 5;

    SHR_FUNC_INIT_VARS(unit);

    init_info.chunk_size = 64;
    sal_strncpy(init_info.move_function, "sw_state_defragmented_chunk_move_test", SW_STATE_CB_DB_NAME_STR_SIZE - 1);

    LOG_CLI((BSL_META("Create chunk 1.\n")));
    SHR_IF_ERR_EXIT(example.defragmented_chunk_example.create(unit, def_cunk_index_part1, &init_info));

    LOG_CLI((BSL_META("PART 1 - Create and destroy. DONE.\n\n")));

    LOG_CLI((BSL_META("Create chunk 2.\n")));
    SHR_IF_ERR_EXIT(example.defragmented_chunk_example.create(unit, def_cunk_index_part2, &init_info));

    LOG_CLI((BSL_META("Create chunk 3.\n")));
    SHR_IF_ERR_EXIT(example.defragmented_chunk_example.create(unit, def_cunk_index_part3, &init_info));

    LOG_CLI((BSL_META("Create chunk 4.\n")));
    init_info.chunk_size = 17;
    sal_strncpy(init_info.move_function, "sw_state_defragmented_chunk_move_test", SW_STATE_CB_DB_NAME_STR_SIZE - 1);
    SHR_IF_ERR_EXIT(example.defragmented_chunk_example.create(unit, def_cunk_index_part4, &init_info));

    init_info.chunk_size = 64;
    sal_strncpy(init_info.move_function, "sw_state_defragmented_chunk_move_test", SW_STATE_CB_DB_NAME_STR_SIZE - 1);
    LOG_CLI((BSL_META("Create chunk 5.\n")));
    SHR_IF_ERR_EXIT(example.defragmented_chunk_example.create(unit, def_cunk_index_part5, &init_info));

    init_info.chunk_size = 17;
    sal_strncpy(init_info.move_function, "sw_state_defragmented_chunk_move_test", SW_STATE_CB_DB_NAME_STR_SIZE - 1);
    LOG_CLI((BSL_META("Create chunk 6.\n")));
    SHR_IF_ERR_EXIT(example.defragmented_chunk_example.create(unit, def_cunk_index_part6, &init_info));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static int
dnx_sw_state_large_ds_init(
    int unit)
{
    sw_state_htbl_init_info_t htb_init_info;

    SHR_FUNC_INIT_VARS(unit);

    LOG_CLI((BSL_META
             ("The test will try to create large size DSs, but the addional arguments from the xml will overwrite the lagre number of els.\n")));

    sal_memset(&htb_init_info, 0, sizeof(sw_state_htbl_init_info_t));
    htb_init_info.max_nof_elements = 123456789;
    htb_init_info.expected_nof_elements = 10;
    htb_init_info.hash_function = NULL;
    htb_init_info.print_cb_name = "sw_state_htb_example_entry_print";

    LOG_CLI((BSL_META("Create the hash table.\n")));
    SHR_IF_ERR_EXIT(example.large_DS_examples.large_htb.create(unit, 0, &htb_init_info));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

int
dnx_sw_state_example_init(
    int unit)
{
    uint32 nof_buffer_elements = 10;
    bsl_severity_t original_severity_sw_state = BSL_INFO, original_shell_cmd = BSL_INFO;

    SHR_FUNC_INIT_VARS(unit);

    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_sw_state);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, bslSeverityFatal);

    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_APPL_SHELL, original_shell_cmd);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_APPL_SHELL, bslSeverityFatal);

    LOG_CLI((BSL_META("Init example init module.\n")));
    SHR_IF_ERR_EXIT(example.init(unit));

    LOG_CLI((BSL_META("Create new mutex.\n")));
    SHR_IF_ERR_EXIT(example.mutex_test.create(unit));

    LOG_CLI((BSL_META("Create new semaphore.\n")));
    SHR_IF_ERR_EXIT(example.sem_test.create(unit, TRUE, 1));

    SHR_IF_ERR_EXIT(dnx_sw_state_dnxdata_init(unit));

    SHR_IF_ERR_EXIT(dnx_sw_state_arrays_init(unit));

    SHR_IF_ERR_EXIT(dnx_sw_state_bitmap_init(unit));

    LOG_CLI((BSL_META("Allocate the buffer variable.\n")));
    SHR_IF_ERR_EXIT(example.buffer.alloc(unit, nof_buffer_elements));

    SHR_IF_ERR_EXIT(dnx_sw_state_default_value_init(unit));

    SHR_IF_ERR_EXIT(dnx_sw_state_occupation_bitmap_init(unit));

    SHR_IF_ERR_EXIT(dnx_sw_state_occupation2_bitmap_init(unit));

    SHR_IF_ERR_EXIT(dnx_sw_state_bt_init(unit));

    SHR_IF_ERR_EXIT(dnx_sw_state_htb_init(unit));

    SHR_IF_ERR_EXIT(dnx_sw_state_htb_rh_init(unit));

    SHR_IF_ERR_EXIT(dnx_sw_state_multihead_htb_rh_init(unit));

    SHR_IF_ERR_EXIT(dnx_sw_state_index_htb_rh_init(unit));

    SHR_IF_ERR_EXIT(dnx_sw_state_htb_arr_init(unit));

    SHR_IF_ERR_EXIT(dnx_sw_state_index_htb_init(unit));

    SHR_IF_ERR_EXIT(dnx_sw_state_defragmented_chunk_init(unit));

    SHR_IF_ERR_EXIT(dnx_sw_state_large_ds_init(unit));

    SHR_EXIT();
exit:

    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_sw_state);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_APPL_SHELL, original_shell_cmd);
    SHR_FUNC_EXIT;
}

#undef _ERR_MSG_MODULE_NAME

#else
typedef int make_iso_compilers_happy;

#endif
