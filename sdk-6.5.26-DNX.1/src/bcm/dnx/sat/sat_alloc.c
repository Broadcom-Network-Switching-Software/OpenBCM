
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_SAT

#include <soc/sand/shrextend/shrextend_debug.h>
#include <bcm/sat.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sat.h>
#include <bcm_int/dnx/sat/sat.h>
#include <soc/dnxc/error.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_sat_access.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>

shr_error_e
dnx_am_template_sat_pkt_header_create(
    int unit,
    int default_profile,
    dnx_sat_gtf_pkt_hdr_tbl_info_t * header_info)
{
    dnx_algo_template_create_data_t create_info;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&create_info, 0, sizeof(dnx_algo_template_create_data_t));
    create_info.flags = DNX_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE;
    create_info.first_profile = DNX_AM_TEMPLATE_SAT_PKT_HEADER_LOW_ID;
    create_info.nof_profiles = DNX_AM_TEMPLATE_SAT_PKT_HEADER_COUNT;
    create_info.max_references = DNX_AM_TEMPLATE_SAT_PKT_HEADER_MAX_ENTRIES(unit);
    create_info.default_profile = default_profile;
    create_info.data_size = DNX_AM_TEMPLATE_SAT_PKT_HEADER_SIZE;
    create_info.default_data = header_info;
    sal_strncpy(create_info.name, DNX_ALGO_TEMPLATE_SAT_PKT_HEADER, DNX_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_sat_db.sat_pkt_header.create(unit, &create_info, NULL));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_am_template_sat_pkt_header_exchange(
    int unit,
    int flags,
    dnx_sat_gtf_pkt_hdr_tbl_info_t * header_info,
    int old_header_template,
    int *is_last,
    int *new_header_template,
    int *is_allocated)
{
    int rc = _SHR_E_NONE;
    uint8 temp_is_last, temp_is_alloc;

    SHR_FUNC_INIT_VARS(unit);

    rc = algo_sat_db.sat_pkt_header.exchange
        (unit, flags, header_info, old_header_template, NULL, new_header_template, &temp_is_alloc, &temp_is_last);
    SHR_IF_ERR_EXIT(rc);

    if (is_last != NULL)
    {
        (*is_last) = temp_is_last ? 1 : 0;
    }

    if (is_allocated != NULL)
    {
        (*is_allocated) = temp_is_alloc ? 1 : 0;
    }

    SHR_IF_ERR_EXIT(rc);
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_am_template_sat_pkt_header_length_get(
    int unit,
    int header_template,
    uint32 *length)
{
    int rc = _SHR_E_NONE;
    int ref_count;
    dnx_sat_gtf_pkt_hdr_tbl_info_t profile_data;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&profile_data, 0, sizeof(profile_data));

    rc = algo_sat_db.sat_pkt_header.profile_data_get(unit, header_template, &ref_count, &profile_data);
    SHR_IF_ERR_EXIT(rc);

    *length = profile_data.pkt_header_len;

exit:
    SHR_FUNC_EXIT;
}

void
dnx_algo_sat_pkt_header_print_cb(
    int unit,
    const void *data)
{
    DNX_SW_STATE_PRINT(unit, "SAT packet header profile: %d", *(int *) data);
    return;
}

shr_error_e
dnx_sat_gtf_id_allocate(
    int unit,
    uint32 flags,
    bcm_sat_gtf_t * gtf_id)
{

    uint32 algo_flags;
    shr_error_e ret;

    SHR_FUNC_INIT_VARS(unit);

    algo_flags = 0;
    if (flags & BCM_SAT_GTF_WITH_ID)
    {
        algo_flags = DNX_ALGO_RES_ALLOCATE_WITH_ID;
    }

    ret = algo_sat_db.sat_res_alloc_gtf_id.allocate_single(unit, algo_flags, NULL, gtf_id);
    if ((ret == _SHR_E_RESOURCE) || (ret == _SHR_E_FULL))
    {
        ret = _SHR_E_RESOURCE;
    }
    SHR_IF_ERR_EXIT(ret);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_sat_gtf_id_free(
    int unit,
    bcm_sat_gtf_t gtf_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_sat_db.sat_res_alloc_gtf_id.free_single(unit, gtf_id, NULL));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_sat_ctf_id_allocate(
    int unit,
    uint32 flags,
    bcm_sat_ctf_t * ctf_id)
{

    uint32 algo_flags;
    shr_error_e ret;

    SHR_FUNC_INIT_VARS(unit);

    algo_flags = 0;
    if (flags & BCM_SAT_CTF_WITH_ID)
    {
        algo_flags = DNX_ALGO_RES_ALLOCATE_WITH_ID;
    }

    ret = algo_sat_db.sat_res_alloc_ctf_id.allocate_single(unit, algo_flags, NULL, ctf_id);
    if ((ret == _SHR_E_RESOURCE) || (ret == _SHR_E_FULL))
    {
        ret = _SHR_E_RESOURCE;
    }
    SHR_IF_ERR_EXIT(ret);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_sat_ctf_id_free(
    int unit,
    bcm_sat_ctf_t ctf_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_sat_db.sat_res_alloc_ctf_id.free_single(unit, ctf_id, NULL));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_sat_ctf_trap_entry_allocate(
    int unit,
    int *trap_entry_idx)
{
    shr_error_e ret;

    SHR_FUNC_INIT_VARS(unit);

    ret = algo_sat_db.sat_res_alloc_trap_data.allocate_single(unit, 0, NULL, trap_entry_idx);
    if ((ret == _SHR_E_RESOURCE) || (ret == _SHR_E_FULL))
    {
        ret = _SHR_E_FULL;
    }
    SHR_IF_ERR_EXIT(ret);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_sat_ctf_trap_entry_free(
    int unit,
    int trap_entry_idx)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_sat_db.sat_res_alloc_trap_data.free_single(unit, trap_entry_idx, NULL));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_sat_ctf_trap_id_allocate(
    int unit,
    int *trap_idx)
{
    shr_error_e ret;

    SHR_FUNC_INIT_VARS(unit);

    ret = algo_sat_db.sat_res_alloc_trap_id.allocate_single(unit, 0, NULL, trap_idx);

    if ((ret == _SHR_E_RESOURCE) || (ret == _SHR_E_FULL))
    {
        ret = _SHR_E_RESOURCE;
    }
    SHR_IF_ERR_EXIT(ret);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_sat_ctf_trap_id_free(
    int unit,
    int trap_idx)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_sat_db.sat_res_alloc_trap_id.free_single(unit, trap_idx, NULL));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_sat_pkt_header_mem_allocate(
    int unit,
    int nof_dwords,
    int *pkt_header_base)
{
    shr_error_e ret;

    SHR_FUNC_INIT_VARS(unit);

    ret = algo_sat_db.sat_res_alloc_pkt_header_mem.allocate_several(unit, 0, nof_dwords, NULL, pkt_header_base);

    if ((ret == _SHR_E_RESOURCE) || (ret == _SHR_E_FULL))
    {
        ret = _SHR_E_RESOURCE;
    }
    SHR_IF_ERR_EXIT(ret);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_sat_pkt_header_mem_free(
    int unit,
    int nof_dwords,
    int pkt_header_base)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_sat_db.sat_res_alloc_pkt_header_mem.free_several(unit, nof_dwords, pkt_header_base, NULL));

exit:
    SHR_FUNC_EXIT;
}
