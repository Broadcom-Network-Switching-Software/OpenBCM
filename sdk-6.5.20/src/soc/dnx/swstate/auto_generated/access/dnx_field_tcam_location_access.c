
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnxc/swstate/dnxc_sw_state_c_includes.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_location_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_field_tcam_location_diagnostic.h>



dnx_field_tcam_location_sw_t* dnx_field_tcam_location_sw_dummy = NULL;



int
dnx_field_tcam_location_sw_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID]),
        "dnx_field_tcam_location_sw[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_location_sw_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        dnx_field_tcam_location_sw_t,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_tcam_location_sw_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID]),
        "dnx_field_tcam_location_sw[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        dnx_field_tcam_location_sw_info,
        DNX_FIELD_TCAM_LOCATION_SW_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(dnx_field_tcam_location_sw_t),
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_location_sw_tcam_handlers_info_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info,
        dnx_field_tcam_location_handler_info_t,
        dnx_data_field.tcam.nof_tcam_handlers_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_tcam_location_sw_tcam_handlers_info_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info,
        "dnx_field_tcam_location_sw[%d]->tcam_handlers_info",
        unit,
        dnx_data_field.tcam.nof_tcam_handlers_get(unit) * sizeof(dnx_field_tcam_location_handler_info_t) / sizeof(*((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        dnx_field_tcam_location_sw_info,
        DNX_FIELD_TCAM_LOCATION_SW_TCAM_HANDLERS_INFO_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_location_sw_tcam_handlers_info_entries_occupation_bitmap_alloc(int unit, uint32 tcam_handlers_info_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info,
        tcam_handlers_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].entries_occupation_bitmap,
        sw_state_occ_bm_t,
        dnx_data_device.general.nof_cores_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_tcam_location_sw_tcam_handlers_info_entries_occupation_bitmap_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].entries_occupation_bitmap,
        "dnx_field_tcam_location_sw[%d]->tcam_handlers_info[%d].entries_occupation_bitmap",
        unit, tcam_handlers_info_idx_0,
        dnx_data_device.general.nof_cores_get(unit) * sizeof(sw_state_occ_bm_t) / sizeof(*((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].entries_occupation_bitmap));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        dnx_field_tcam_location_sw_info,
        DNX_FIELD_TCAM_LOCATION_SW_TCAM_HANDLERS_INFO_ENTRIES_OCCUPATION_BITMAP_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_location_sw_tcam_handlers_info_entries_occupation_bitmap_create(int unit, uint32 tcam_handlers_info_idx_0, uint32 entries_occupation_bitmap_idx_0, sw_state_occ_bitmap_init_info_t * init_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        SW_STATE_FUNC_CREATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].entries_occupation_bitmap);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID);

    SW_STATE_OCC_BM_CREATE(
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].entries_occupation_bitmap[entries_occupation_bitmap_idx_0],
        init_info,
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_OCC_BM_CREATE_LOGGING,
        BSL_LS_SWSTATEDNX_OCC_BMCREATE,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        &((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].entries_occupation_bitmap[entries_occupation_bitmap_idx_0],
        "dnx_field_tcam_location_sw[%d]->tcam_handlers_info[%d].entries_occupation_bitmap[%d]",
        unit, tcam_handlers_info_idx_0, entries_occupation_bitmap_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        dnx_field_tcam_location_sw_info,
        DNX_FIELD_TCAM_LOCATION_SW_TCAM_HANDLERS_INFO_ENTRIES_OCCUPATION_BITMAP_INFO,
        DNX_SW_STATE_DIAG_OCC_BM,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_location_sw_tcam_handlers_info_entries_occupation_bitmap_get_next(int unit, uint32 tcam_handlers_info_idx_0, uint32 entries_occupation_bitmap_idx_0, uint32 *place, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        SW_STATE_FUNC_GET_NEXT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].entries_occupation_bitmap);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID);

    SW_STATE_OCC_BM_GET_NEXT(
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].entries_occupation_bitmap[entries_occupation_bitmap_idx_0],
        place,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_OCC_BM_GET_NEXT_LOGGING,
        BSL_LS_SWSTATEDNX_OCC_BMGET_NEXT,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        &((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].entries_occupation_bitmap[entries_occupation_bitmap_idx_0],
        "dnx_field_tcam_location_sw[%d]->tcam_handlers_info[%d].entries_occupation_bitmap[%d]",
        unit, tcam_handlers_info_idx_0, entries_occupation_bitmap_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        dnx_field_tcam_location_sw_info,
        DNX_FIELD_TCAM_LOCATION_SW_TCAM_HANDLERS_INFO_ENTRIES_OCCUPATION_BITMAP_INFO,
        DNX_SW_STATE_DIAG_OCC_BM,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_location_sw_tcam_handlers_info_entries_occupation_bitmap_get_next_in_range(int unit, uint32 tcam_handlers_info_idx_0, uint32 entries_occupation_bitmap_idx_0, uint32 start, uint32 end, uint8 forward, uint8 val, uint32 *place, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        SW_STATE_FUNC_GET_NEXT_IN_RANGE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].entries_occupation_bitmap);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID);

    SW_STATE_OCC_BM_GET_NEXT_IN_RANGE(
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].entries_occupation_bitmap[entries_occupation_bitmap_idx_0],
        start,
        end,
        forward,
        val,
        place,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_OCC_BM_GET_NEXT_IN_RANGE_LOGGING,
        BSL_LS_SWSTATEDNX_OCC_BMGET_NEXT_IN_RANGE,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        &((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].entries_occupation_bitmap[entries_occupation_bitmap_idx_0],
        "dnx_field_tcam_location_sw[%d]->tcam_handlers_info[%d].entries_occupation_bitmap[%d]",
        unit, tcam_handlers_info_idx_0, entries_occupation_bitmap_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        dnx_field_tcam_location_sw_info,
        DNX_FIELD_TCAM_LOCATION_SW_TCAM_HANDLERS_INFO_ENTRIES_OCCUPATION_BITMAP_INFO,
        DNX_SW_STATE_DIAG_OCC_BM,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_location_sw_tcam_handlers_info_entries_occupation_bitmap_status_set(int unit, uint32 tcam_handlers_info_idx_0, uint32 entries_occupation_bitmap_idx_0, uint32 place, uint8 occupied)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        SW_STATE_FUNC_STATUS_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].entries_occupation_bitmap);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID);

    SW_STATE_OCC_BM_STATUS_SET(
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].entries_occupation_bitmap[entries_occupation_bitmap_idx_0],
        place,
        occupied);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_OCC_BM_STATUS_SET_LOGGING,
        BSL_LS_SWSTATEDNX_OCC_BMSTATUS_SET,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        &((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].entries_occupation_bitmap[entries_occupation_bitmap_idx_0],
        "dnx_field_tcam_location_sw[%d]->tcam_handlers_info[%d].entries_occupation_bitmap[%d]",
        unit, tcam_handlers_info_idx_0, entries_occupation_bitmap_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        dnx_field_tcam_location_sw_info,
        DNX_FIELD_TCAM_LOCATION_SW_TCAM_HANDLERS_INFO_ENTRIES_OCCUPATION_BITMAP_INFO,
        DNX_SW_STATE_DIAG_OCC_BM,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_location_sw_tcam_handlers_info_entries_occupation_bitmap_is_occupied(int unit, uint32 tcam_handlers_info_idx_0, uint32 entries_occupation_bitmap_idx_0, uint32 place, uint8 *occupied)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        SW_STATE_FUNC_IS_OCCUPIED,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].entries_occupation_bitmap);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID);

    SW_STATE_OCC_BM_IS_OCCUPIED(
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].entries_occupation_bitmap[entries_occupation_bitmap_idx_0],
        place,
        occupied);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_OCC_BM_IS_OCCUPIED_LOGGING,
        BSL_LS_SWSTATEDNX_OCC_BMIS_OCCUPIED,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        &((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].entries_occupation_bitmap[entries_occupation_bitmap_idx_0],
        "dnx_field_tcam_location_sw[%d]->tcam_handlers_info[%d].entries_occupation_bitmap[%d]",
        unit, tcam_handlers_info_idx_0, entries_occupation_bitmap_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        dnx_field_tcam_location_sw_info,
        DNX_FIELD_TCAM_LOCATION_SW_TCAM_HANDLERS_INFO_ENTRIES_OCCUPATION_BITMAP_INFO,
        DNX_SW_STATE_DIAG_OCC_BM,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_location_sw_tcam_handlers_info_entries_occupation_bitmap_alloc_next(int unit, uint32 tcam_handlers_info_idx_0, uint32 entries_occupation_bitmap_idx_0, uint32 *place, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        SW_STATE_FUNC_ALLOC_NEXT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].entries_occupation_bitmap);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID);

    SW_STATE_OCC_BM_ALLOC_NEXT(
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].entries_occupation_bitmap[entries_occupation_bitmap_idx_0],
        place,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_OCC_BM_ALLOC_NEXT_LOGGING,
        BSL_LS_SWSTATEDNX_OCC_BMALLOC_NEXT,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        &((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].entries_occupation_bitmap[entries_occupation_bitmap_idx_0],
        "dnx_field_tcam_location_sw[%d]->tcam_handlers_info[%d].entries_occupation_bitmap[%d]",
        unit, tcam_handlers_info_idx_0, entries_occupation_bitmap_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        dnx_field_tcam_location_sw_info,
        DNX_FIELD_TCAM_LOCATION_SW_TCAM_HANDLERS_INFO_ENTRIES_OCCUPATION_BITMAP_INFO,
        DNX_SW_STATE_DIAG_OCC_BM,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_location_sw_tcam_handlers_info_entries_occupation_bitmap_clear(int unit, uint32 tcam_handlers_info_idx_0, uint32 entries_occupation_bitmap_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        SW_STATE_FUNC_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].entries_occupation_bitmap);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID);

    SW_STATE_OCC_BM_CLEAR(
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].entries_occupation_bitmap[entries_occupation_bitmap_idx_0]);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_OCC_BM_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_OCC_BMCLEAR,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        &((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].entries_occupation_bitmap[entries_occupation_bitmap_idx_0],
        "dnx_field_tcam_location_sw[%d]->tcam_handlers_info[%d].entries_occupation_bitmap[%d]",
        unit, tcam_handlers_info_idx_0, entries_occupation_bitmap_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        dnx_field_tcam_location_sw_info,
        DNX_FIELD_TCAM_LOCATION_SW_TCAM_HANDLERS_INFO_ENTRIES_OCCUPATION_BITMAP_INFO,
        DNX_SW_STATE_DIAG_OCC_BM,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_alloc(int unit, uint32 tcam_handlers_info_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info,
        tcam_handlers_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range,
        sw_state_sorted_ll_t,
        dnx_data_device.general.nof_cores_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC_EXCEPTION | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range,
        "dnx_field_tcam_location_sw[%d]->tcam_handlers_info[%d].priorities_range",
        unit, tcam_handlers_info_idx_0,
        dnx_data_device.general.nof_cores_get(unit) * sizeof(sw_state_sorted_ll_t) / sizeof(*((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        dnx_field_tcam_location_sw_info,
        DNX_FIELD_TCAM_LOCATION_SW_TCAM_HANDLERS_INFO_PRIORITIES_RANGE_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_create_empty(int unit, uint32 tcam_handlers_info_idx_0, uint32 priorities_range_idx_0, sw_state_ll_init_info_t *init_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        SW_STATE_FUNC_CREATE_EMPTY,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID);

    SW_STATE_LL_CREATE_EMPTY(
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range[priorities_range_idx_0],
        init_info,
        SW_STATE_LL_SORTED,
        uint32,
        dnx_field_tcam_location_range_t,
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION | DNXC_SW_STATE_DNX_DATA_ALLOC_EXCEPTION);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_CREATE_EMPTY_LOGGING,
        BSL_LS_SWSTATEDNX_LLCREATE_EMPTY,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        &((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range[priorities_range_idx_0],
        "dnx_field_tcam_location_sw[%d]->tcam_handlers_info[%d].priorities_range[%d]",
        unit, tcam_handlers_info_idx_0, priorities_range_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        dnx_field_tcam_location_sw_info,
        DNX_FIELD_TCAM_LOCATION_SW_TCAM_HANDLERS_INFO_PRIORITIES_RANGE_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION | DNXC_SW_STATE_DNX_DATA_ALLOC_EXCEPTION);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_nof_elements(int unit, uint32 tcam_handlers_info_idx_0, uint32 priorities_range_idx_0, uint32 *nof_elements)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        SW_STATE_FUNC_NOF_ELEMENTS,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID);

    SW_STATE_LL_NOF_ELEMENTS(
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range[priorities_range_idx_0],
        nof_elements);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NOF_ELEMENTS_LOGGING,
        BSL_LS_SWSTATEDNX_LLNOF_ELEMENTS,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        &((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range[priorities_range_idx_0],
        "dnx_field_tcam_location_sw[%d]->tcam_handlers_info[%d].priorities_range[%d]",
        unit, tcam_handlers_info_idx_0, priorities_range_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        dnx_field_tcam_location_sw_info,
        DNX_FIELD_TCAM_LOCATION_SW_TCAM_HANDLERS_INFO_PRIORITIES_RANGE_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_node_value(int unit, uint32 tcam_handlers_info_idx_0, uint32 priorities_range_idx_0, sw_state_ll_node_t node, dnx_field_tcam_location_range_t *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        SW_STATE_FUNC_NODE_VALUE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID);

    SW_STATE_LL_NODE_VALUE(
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range[priorities_range_idx_0],
        node,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NODE_VALUE_LOGGING,
        BSL_LS_SWSTATEDNX_LLNODE_VALUE,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        &((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range[priorities_range_idx_0],
        "dnx_field_tcam_location_sw[%d]->tcam_handlers_info[%d].priorities_range[%d]",
        unit, tcam_handlers_info_idx_0, priorities_range_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        dnx_field_tcam_location_sw_info,
        DNX_FIELD_TCAM_LOCATION_SW_TCAM_HANDLERS_INFO_PRIORITIES_RANGE_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_node_update(int unit, uint32 tcam_handlers_info_idx_0, uint32 priorities_range_idx_0, sw_state_ll_node_t node, const dnx_field_tcam_location_range_t *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        SW_STATE_FUNC_NODE_UPDATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID);

    SW_STATE_LL_NODE_UPDATE(
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range[priorities_range_idx_0],
        node,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NODE_UPDATE_LOGGING,
        BSL_LS_SWSTATEDNX_LLNODE_UPDATE,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        &((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range[priorities_range_idx_0],
        "dnx_field_tcam_location_sw[%d]->tcam_handlers_info[%d].priorities_range[%d]",
        unit, tcam_handlers_info_idx_0, priorities_range_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        dnx_field_tcam_location_sw_info,
        DNX_FIELD_TCAM_LOCATION_SW_TCAM_HANDLERS_INFO_PRIORITIES_RANGE_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_next_node(int unit, uint32 tcam_handlers_info_idx_0, uint32 priorities_range_idx_0, sw_state_ll_node_t node, sw_state_ll_node_t *next_node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        SW_STATE_FUNC_NEXT_NODE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID);

    SW_STATE_LL_NEXT_NODE(
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range[priorities_range_idx_0],
        node,
        next_node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NEXT_NODE_LOGGING,
        BSL_LS_SWSTATEDNX_LLNEXT_NODE,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        &((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range[priorities_range_idx_0],
        "dnx_field_tcam_location_sw[%d]->tcam_handlers_info[%d].priorities_range[%d]",
        unit, tcam_handlers_info_idx_0, priorities_range_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        dnx_field_tcam_location_sw_info,
        DNX_FIELD_TCAM_LOCATION_SW_TCAM_HANDLERS_INFO_PRIORITIES_RANGE_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_previous_node(int unit, uint32 tcam_handlers_info_idx_0, uint32 priorities_range_idx_0, sw_state_ll_node_t node, sw_state_ll_node_t *prev_node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        SW_STATE_FUNC_PREVIOUS_NODE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID);

    SW_STATE_LL_PREVIOUS_NODE(
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range[priorities_range_idx_0],
        node,
        prev_node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_PREVIOUS_NODE_LOGGING,
        BSL_LS_SWSTATEDNX_LLPREVIOUS_NODE,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        &((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range[priorities_range_idx_0],
        "dnx_field_tcam_location_sw[%d]->tcam_handlers_info[%d].priorities_range[%d]",
        unit, tcam_handlers_info_idx_0, priorities_range_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        dnx_field_tcam_location_sw_info,
        DNX_FIELD_TCAM_LOCATION_SW_TCAM_HANDLERS_INFO_PRIORITIES_RANGE_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_add_first(int unit, uint32 tcam_handlers_info_idx_0, uint32 priorities_range_idx_0, const uint32 *key, const dnx_field_tcam_location_range_t *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        SW_STATE_FUNC_ADD_FIRST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID);

    SW_STATE_LL_ADD_FIRST(
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range[priorities_range_idx_0],
        key,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_ADD_FIRST_LOGGING,
        BSL_LS_SWSTATEDNX_LLADD_FIRST,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        &((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range[priorities_range_idx_0],
        "dnx_field_tcam_location_sw[%d]->tcam_handlers_info[%d].priorities_range[%d]",
        unit, tcam_handlers_info_idx_0, priorities_range_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        dnx_field_tcam_location_sw_info,
        DNX_FIELD_TCAM_LOCATION_SW_TCAM_HANDLERS_INFO_PRIORITIES_RANGE_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_add_last(int unit, uint32 tcam_handlers_info_idx_0, uint32 priorities_range_idx_0, const uint32 *key, const dnx_field_tcam_location_range_t *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        SW_STATE_FUNC_ADD_LAST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID);

    SW_STATE_LL_ADD_LAST(
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range[priorities_range_idx_0],
        key,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_ADD_LAST_LOGGING,
        BSL_LS_SWSTATEDNX_LLADD_LAST,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        &((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range[priorities_range_idx_0],
        "dnx_field_tcam_location_sw[%d]->tcam_handlers_info[%d].priorities_range[%d]",
        unit, tcam_handlers_info_idx_0, priorities_range_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        dnx_field_tcam_location_sw_info,
        DNX_FIELD_TCAM_LOCATION_SW_TCAM_HANDLERS_INFO_PRIORITIES_RANGE_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_add_before(int unit, uint32 tcam_handlers_info_idx_0, uint32 priorities_range_idx_0, sw_state_ll_node_t node, const uint32 *key, const dnx_field_tcam_location_range_t *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        SW_STATE_FUNC_ADD_BEFORE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID);

    SW_STATE_LL_ADD_BEFORE(
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range[priorities_range_idx_0],
        node,
        key,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_ADD_BEFORE_LOGGING,
        BSL_LS_SWSTATEDNX_LLADD_BEFORE,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        &((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range[priorities_range_idx_0],
        "dnx_field_tcam_location_sw[%d]->tcam_handlers_info[%d].priorities_range[%d]",
        unit, tcam_handlers_info_idx_0, priorities_range_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        dnx_field_tcam_location_sw_info,
        DNX_FIELD_TCAM_LOCATION_SW_TCAM_HANDLERS_INFO_PRIORITIES_RANGE_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_add_after(int unit, uint32 tcam_handlers_info_idx_0, uint32 priorities_range_idx_0, sw_state_ll_node_t node, const uint32 *key, const dnx_field_tcam_location_range_t *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        SW_STATE_FUNC_ADD_AFTER,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID);

    SW_STATE_LL_ADD_AFTER(
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range[priorities_range_idx_0],
        node,
        key,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_ADD_AFTER_LOGGING,
        BSL_LS_SWSTATEDNX_LLADD_AFTER,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        &((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range[priorities_range_idx_0],
        "dnx_field_tcam_location_sw[%d]->tcam_handlers_info[%d].priorities_range[%d]",
        unit, tcam_handlers_info_idx_0, priorities_range_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        dnx_field_tcam_location_sw_info,
        DNX_FIELD_TCAM_LOCATION_SW_TCAM_HANDLERS_INFO_PRIORITIES_RANGE_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_remove_node(int unit, uint32 tcam_handlers_info_idx_0, uint32 priorities_range_idx_0, sw_state_ll_node_t node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        SW_STATE_FUNC_REMOVE_NODE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID);

    SW_STATE_LL_REMOVE_NODE(
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range[priorities_range_idx_0],
        node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_REMOVE_NODE_LOGGING,
        BSL_LS_SWSTATEDNX_LLREMOVE_NODE,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        &((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range[priorities_range_idx_0],
        "dnx_field_tcam_location_sw[%d]->tcam_handlers_info[%d].priorities_range[%d]",
        unit, tcam_handlers_info_idx_0, priorities_range_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        dnx_field_tcam_location_sw_info,
        DNX_FIELD_TCAM_LOCATION_SW_TCAM_HANDLERS_INFO_PRIORITIES_RANGE_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_get_last(int unit, uint32 tcam_handlers_info_idx_0, uint32 priorities_range_idx_0, sw_state_ll_node_t *node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        SW_STATE_FUNC_GET_LAST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID);

    SW_STATE_LL_GET_LAST(
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range[priorities_range_idx_0],
        node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_GET_LAST_LOGGING,
        BSL_LS_SWSTATEDNX_LLGET_LAST,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        &((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range[priorities_range_idx_0],
        "dnx_field_tcam_location_sw[%d]->tcam_handlers_info[%d].priorities_range[%d]",
        unit, tcam_handlers_info_idx_0, priorities_range_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        dnx_field_tcam_location_sw_info,
        DNX_FIELD_TCAM_LOCATION_SW_TCAM_HANDLERS_INFO_PRIORITIES_RANGE_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_get_first(int unit, uint32 tcam_handlers_info_idx_0, uint32 priorities_range_idx_0, sw_state_ll_node_t *node)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        SW_STATE_FUNC_GET_FIRST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID);

    SW_STATE_LL_GET_FIRST(
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range[priorities_range_idx_0],
        node);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_GET_FIRST_LOGGING,
        BSL_LS_SWSTATEDNX_LLGET_FIRST,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        &((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range[priorities_range_idx_0],
        "dnx_field_tcam_location_sw[%d]->tcam_handlers_info[%d].priorities_range[%d]",
        unit, tcam_handlers_info_idx_0, priorities_range_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        dnx_field_tcam_location_sw_info,
        DNX_FIELD_TCAM_LOCATION_SW_TCAM_HANDLERS_INFO_PRIORITIES_RANGE_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_print(int unit, uint32 tcam_handlers_info_idx_0, uint32 priorities_range_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        SW_STATE_FUNC_PRINT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID);

    SW_STATE_LL_PRINT(
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range[priorities_range_idx_0]);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_PRINT_LOGGING,
        BSL_LS_SWSTATEDNX_LLPRINT,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        &((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range[priorities_range_idx_0],
        "dnx_field_tcam_location_sw[%d]->tcam_handlers_info[%d].priorities_range[%d]",
        unit, tcam_handlers_info_idx_0, priorities_range_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        dnx_field_tcam_location_sw_info,
        DNX_FIELD_TCAM_LOCATION_SW_TCAM_HANDLERS_INFO_PRIORITIES_RANGE_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_add(int unit, uint32 tcam_handlers_info_idx_0, uint32 priorities_range_idx_0, const uint32 *key, const dnx_field_tcam_location_range_t *value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        SW_STATE_FUNC_ADD,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID);

    SW_STATE_LL_ADD(
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range[priorities_range_idx_0],
        key,
        value);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_ADD_LOGGING,
        BSL_LS_SWSTATEDNX_LLADD,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        &((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range[priorities_range_idx_0],
        "dnx_field_tcam_location_sw[%d]->tcam_handlers_info[%d].priorities_range[%d]",
        unit, tcam_handlers_info_idx_0, priorities_range_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        dnx_field_tcam_location_sw_info,
        DNX_FIELD_TCAM_LOCATION_SW_TCAM_HANDLERS_INFO_PRIORITIES_RANGE_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_node_key(int unit, uint32 tcam_handlers_info_idx_0, uint32 priorities_range_idx_0, sw_state_ll_node_t node, uint32 *key)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        SW_STATE_FUNC_NODE_KEY,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID);

    SW_STATE_LL_NODE_KEY(
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range[priorities_range_idx_0],
        node,
        key);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_NODE_KEY_LOGGING,
        BSL_LS_SWSTATEDNX_LLNODE_KEY,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        &((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range[priorities_range_idx_0],
        "dnx_field_tcam_location_sw[%d]->tcam_handlers_info[%d].priorities_range[%d]",
        unit, tcam_handlers_info_idx_0, priorities_range_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        dnx_field_tcam_location_sw_info,
        DNX_FIELD_TCAM_LOCATION_SW_TCAM_HANDLERS_INFO_PRIORITIES_RANGE_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_find(int unit, uint32 tcam_handlers_info_idx_0, uint32 priorities_range_idx_0, sw_state_ll_node_t *node, const uint32 *key, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        SW_STATE_FUNC_FIND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID);

    SW_STATE_LL_FIND(
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range[priorities_range_idx_0],
        node,
        key,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_LL_FIND_LOGGING,
        BSL_LS_SWSTATEDNX_LLFIND,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        &((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_handlers_info[tcam_handlers_info_idx_0].priorities_range[priorities_range_idx_0],
        "dnx_field_tcam_location_sw[%d]->tcam_handlers_info[%d].priorities_range[%d]",
        unit, tcam_handlers_info_idx_0, priorities_range_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        dnx_field_tcam_location_sw_info,
        DNX_FIELD_TCAM_LOCATION_SW_TCAM_HANDLERS_INFO_PRIORITIES_RANGE_INFO,
        DNX_SW_STATE_DIAG_LL,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_location_sw_location_priority_arr_set(int unit, uint32 location_priority_arr_idx_0, uint32 location_priority_arr_idx_1, uint32 location_priority_arr)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        location_priority_arr_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->location_priority_arr[location_priority_arr_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->location_priority_arr[location_priority_arr_idx_0],
        location_priority_arr_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->location_priority_arr[location_priority_arr_idx_0][location_priority_arr_idx_1],
        location_priority_arr,
        uint32,
        0,
        "dnx_field_tcam_location_sw_location_priority_arr_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        &location_priority_arr,
        "dnx_field_tcam_location_sw[%d]->location_priority_arr[%d][%d]",
        unit, location_priority_arr_idx_0, location_priority_arr_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        dnx_field_tcam_location_sw_info,
        DNX_FIELD_TCAM_LOCATION_SW_LOCATION_PRIORITY_ARR_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_location_sw_location_priority_arr_get(int unit, uint32 location_priority_arr_idx_0, uint32 location_priority_arr_idx_1, uint32 *location_priority_arr)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        location_priority_arr_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        location_priority_arr);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->location_priority_arr[location_priority_arr_idx_0],
        location_priority_arr_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->location_priority_arr[location_priority_arr_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID);

    *location_priority_arr = ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->location_priority_arr[location_priority_arr_idx_0][location_priority_arr_idx_1];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        &((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->location_priority_arr[location_priority_arr_idx_0][location_priority_arr_idx_1],
        "dnx_field_tcam_location_sw[%d]->location_priority_arr[%d][%d]",
        unit, location_priority_arr_idx_0, location_priority_arr_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        dnx_field_tcam_location_sw_info,
        DNX_FIELD_TCAM_LOCATION_SW_LOCATION_PRIORITY_ARR_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_location_sw_location_priority_arr_alloc(int unit, uint32 location_priority_arr_idx_0)
{
    DNX_SW_STATE_DEFAULT_VALUE_DEFS;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        location_priority_arr_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->location_priority_arr[location_priority_arr_idx_0],
        uint32,
        dnx_data_field.tcam.tcam_banks_size_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_tcam_location_sw_location_priority_arr_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->location_priority_arr[location_priority_arr_idx_0],
        "dnx_field_tcam_location_sw[%d]->location_priority_arr[%d]",
        unit, location_priority_arr_idx_0,
        dnx_data_field.tcam.tcam_banks_size_get(unit) * sizeof(uint32) / sizeof(*((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->location_priority_arr[location_priority_arr_idx_0]));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        dnx_field_tcam_location_sw_info,
        DNX_FIELD_TCAM_LOCATION_SW_LOCATION_PRIORITY_ARR_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_field.tcam.tcam_banks_size_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->location_priority_arr[location_priority_arr_idx_0][def_val_idx[0]],
        DNX_FIELD_TCAM_ENTRY_PRIORITY_INVALID);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_location_sw_tcam_banks_occupation_bitmap_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_banks_occupation_bitmap,
        sw_state_occ_bm_t,
        dnx_data_device.general.nof_cores_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_tcam_location_sw_tcam_banks_occupation_bitmap_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_banks_occupation_bitmap,
        "dnx_field_tcam_location_sw[%d]->tcam_banks_occupation_bitmap",
        unit,
        dnx_data_device.general.nof_cores_get(unit) * sizeof(sw_state_occ_bm_t) / sizeof(*((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_banks_occupation_bitmap));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        dnx_field_tcam_location_sw_info,
        DNX_FIELD_TCAM_LOCATION_SW_TCAM_BANKS_OCCUPATION_BITMAP_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_location_sw_tcam_banks_occupation_bitmap_create(int unit, uint32 tcam_banks_occupation_bitmap_idx_0, sw_state_occ_bitmap_init_info_t * init_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        SW_STATE_FUNC_CREATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_banks_occupation_bitmap);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID);

    SW_STATE_OCC_BM_CREATE(
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_banks_occupation_bitmap[tcam_banks_occupation_bitmap_idx_0],
        init_info,
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_OCC_BM_CREATE_LOGGING,
        BSL_LS_SWSTATEDNX_OCC_BMCREATE,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        &((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_banks_occupation_bitmap[tcam_banks_occupation_bitmap_idx_0],
        "dnx_field_tcam_location_sw[%d]->tcam_banks_occupation_bitmap[%d]",
        unit, tcam_banks_occupation_bitmap_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        dnx_field_tcam_location_sw_info,
        DNX_FIELD_TCAM_LOCATION_SW_TCAM_BANKS_OCCUPATION_BITMAP_INFO,
        DNX_SW_STATE_DIAG_OCC_BM,
        NULL,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_location_sw_tcam_banks_occupation_bitmap_get_next(int unit, uint32 tcam_banks_occupation_bitmap_idx_0, uint32 *place, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        SW_STATE_FUNC_GET_NEXT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_banks_occupation_bitmap);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID);

    SW_STATE_OCC_BM_GET_NEXT(
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_banks_occupation_bitmap[tcam_banks_occupation_bitmap_idx_0],
        place,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_OCC_BM_GET_NEXT_LOGGING,
        BSL_LS_SWSTATEDNX_OCC_BMGET_NEXT,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        &((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_banks_occupation_bitmap[tcam_banks_occupation_bitmap_idx_0],
        "dnx_field_tcam_location_sw[%d]->tcam_banks_occupation_bitmap[%d]",
        unit, tcam_banks_occupation_bitmap_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        dnx_field_tcam_location_sw_info,
        DNX_FIELD_TCAM_LOCATION_SW_TCAM_BANKS_OCCUPATION_BITMAP_INFO,
        DNX_SW_STATE_DIAG_OCC_BM,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_location_sw_tcam_banks_occupation_bitmap_get_next_in_range(int unit, uint32 tcam_banks_occupation_bitmap_idx_0, uint32 start, uint32 end, uint8 forward, uint8 val, uint32 *place, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        SW_STATE_FUNC_GET_NEXT_IN_RANGE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_banks_occupation_bitmap);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID);

    SW_STATE_OCC_BM_GET_NEXT_IN_RANGE(
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_banks_occupation_bitmap[tcam_banks_occupation_bitmap_idx_0],
        start,
        end,
        forward,
        val,
        place,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_OCC_BM_GET_NEXT_IN_RANGE_LOGGING,
        BSL_LS_SWSTATEDNX_OCC_BMGET_NEXT_IN_RANGE,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        &((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_banks_occupation_bitmap[tcam_banks_occupation_bitmap_idx_0],
        "dnx_field_tcam_location_sw[%d]->tcam_banks_occupation_bitmap[%d]",
        unit, tcam_banks_occupation_bitmap_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        dnx_field_tcam_location_sw_info,
        DNX_FIELD_TCAM_LOCATION_SW_TCAM_BANKS_OCCUPATION_BITMAP_INFO,
        DNX_SW_STATE_DIAG_OCC_BM,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_location_sw_tcam_banks_occupation_bitmap_status_set(int unit, uint32 tcam_banks_occupation_bitmap_idx_0, uint32 place, uint8 occupied)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        SW_STATE_FUNC_STATUS_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_banks_occupation_bitmap);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID);

    SW_STATE_OCC_BM_STATUS_SET(
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_banks_occupation_bitmap[tcam_banks_occupation_bitmap_idx_0],
        place,
        occupied);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_OCC_BM_STATUS_SET_LOGGING,
        BSL_LS_SWSTATEDNX_OCC_BMSTATUS_SET,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        &((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_banks_occupation_bitmap[tcam_banks_occupation_bitmap_idx_0],
        "dnx_field_tcam_location_sw[%d]->tcam_banks_occupation_bitmap[%d]",
        unit, tcam_banks_occupation_bitmap_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        dnx_field_tcam_location_sw_info,
        DNX_FIELD_TCAM_LOCATION_SW_TCAM_BANKS_OCCUPATION_BITMAP_INFO,
        DNX_SW_STATE_DIAG_OCC_BM,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_location_sw_tcam_banks_occupation_bitmap_is_occupied(int unit, uint32 tcam_banks_occupation_bitmap_idx_0, uint32 place, uint8 *occupied)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        SW_STATE_FUNC_IS_OCCUPIED,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_banks_occupation_bitmap);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID);

    SW_STATE_OCC_BM_IS_OCCUPIED(
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_banks_occupation_bitmap[tcam_banks_occupation_bitmap_idx_0],
        place,
        occupied);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_OCC_BM_IS_OCCUPIED_LOGGING,
        BSL_LS_SWSTATEDNX_OCC_BMIS_OCCUPIED,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        &((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_banks_occupation_bitmap[tcam_banks_occupation_bitmap_idx_0],
        "dnx_field_tcam_location_sw[%d]->tcam_banks_occupation_bitmap[%d]",
        unit, tcam_banks_occupation_bitmap_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        dnx_field_tcam_location_sw_info,
        DNX_FIELD_TCAM_LOCATION_SW_TCAM_BANKS_OCCUPATION_BITMAP_INFO,
        DNX_SW_STATE_DIAG_OCC_BM,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_location_sw_tcam_banks_occupation_bitmap_alloc_next(int unit, uint32 tcam_banks_occupation_bitmap_idx_0, uint32 *place, uint8 *found)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        SW_STATE_FUNC_ALLOC_NEXT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_banks_occupation_bitmap);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID);

    SW_STATE_OCC_BM_ALLOC_NEXT(
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_banks_occupation_bitmap[tcam_banks_occupation_bitmap_idx_0],
        place,
        found);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_OCC_BM_ALLOC_NEXT_LOGGING,
        BSL_LS_SWSTATEDNX_OCC_BMALLOC_NEXT,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        &((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_banks_occupation_bitmap[tcam_banks_occupation_bitmap_idx_0],
        "dnx_field_tcam_location_sw[%d]->tcam_banks_occupation_bitmap[%d]",
        unit, tcam_banks_occupation_bitmap_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        dnx_field_tcam_location_sw_info,
        DNX_FIELD_TCAM_LOCATION_SW_TCAM_BANKS_OCCUPATION_BITMAP_INFO,
        DNX_SW_STATE_DIAG_OCC_BM,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_location_sw_tcam_banks_occupation_bitmap_clear(int unit, uint32 tcam_banks_occupation_bitmap_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        SW_STATE_FUNC_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_banks_occupation_bitmap);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID);

    SW_STATE_OCC_BM_CLEAR(
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        ((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_banks_occupation_bitmap[tcam_banks_occupation_bitmap_idx_0]);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_OCC_BM_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_OCC_BMCLEAR,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        &((dnx_field_tcam_location_sw_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_LOCATION_MODULE_ID])->tcam_banks_occupation_bitmap[tcam_banks_occupation_bitmap_idx_0],
        "dnx_field_tcam_location_sw[%d]->tcam_banks_occupation_bitmap[%d]",
        unit, tcam_banks_occupation_bitmap_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST_WITH_FLAGS(
        unit,
        DNX_FIELD_TCAM_LOCATION_MODULE_ID,
        dnx_field_tcam_location_sw_info,
        DNX_FIELD_TCAM_LOCATION_SW_TCAM_BANKS_OCCUPATION_BITMAP_INFO,
        DNX_SW_STATE_DIAG_OCC_BM,
        NULL,
        DNXC_SW_STATE_NO_FLAGS);

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_field_tcam_location_sw_cbs dnx_field_tcam_location_sw = 	{
	
	dnx_field_tcam_location_sw_is_init,
	dnx_field_tcam_location_sw_init,
		{
		
		dnx_field_tcam_location_sw_tcam_handlers_info_alloc,
			{
			
			dnx_field_tcam_location_sw_tcam_handlers_info_entries_occupation_bitmap_alloc,
			dnx_field_tcam_location_sw_tcam_handlers_info_entries_occupation_bitmap_create,
			dnx_field_tcam_location_sw_tcam_handlers_info_entries_occupation_bitmap_get_next,
			dnx_field_tcam_location_sw_tcam_handlers_info_entries_occupation_bitmap_get_next_in_range,
			dnx_field_tcam_location_sw_tcam_handlers_info_entries_occupation_bitmap_status_set,
			dnx_field_tcam_location_sw_tcam_handlers_info_entries_occupation_bitmap_is_occupied,
			dnx_field_tcam_location_sw_tcam_handlers_info_entries_occupation_bitmap_alloc_next,
			dnx_field_tcam_location_sw_tcam_handlers_info_entries_occupation_bitmap_clear}
		,
			{
			
			dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_alloc,
			dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_create_empty,
			dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_nof_elements,
			dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_node_value,
			dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_node_update,
			dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_next_node,
			dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_previous_node,
			dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_add_first,
			dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_add_last,
			dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_add_before,
			dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_add_after,
			dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_remove_node,
			dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_get_last,
			dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_get_first,
			dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_print,
			dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_add,
			dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_node_key,
			dnx_field_tcam_location_sw_tcam_handlers_info_priorities_range_find}
		}
	,
		{
		
		dnx_field_tcam_location_sw_location_priority_arr_set,
		dnx_field_tcam_location_sw_location_priority_arr_get,
		dnx_field_tcam_location_sw_location_priority_arr_alloc}
	,
		{
		
		dnx_field_tcam_location_sw_tcam_banks_occupation_bitmap_alloc,
		dnx_field_tcam_location_sw_tcam_banks_occupation_bitmap_create,
		dnx_field_tcam_location_sw_tcam_banks_occupation_bitmap_get_next,
		dnx_field_tcam_location_sw_tcam_banks_occupation_bitmap_get_next_in_range,
		dnx_field_tcam_location_sw_tcam_banks_occupation_bitmap_status_set,
		dnx_field_tcam_location_sw_tcam_banks_occupation_bitmap_is_occupied,
		dnx_field_tcam_location_sw_tcam_banks_occupation_bitmap_alloc_next,
		dnx_field_tcam_location_sw_tcam_banks_occupation_bitmap_clear}
	}
;
#undef BSL_LOG_MODULE
