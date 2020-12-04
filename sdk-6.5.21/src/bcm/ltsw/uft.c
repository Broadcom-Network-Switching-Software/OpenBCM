/*! \file uft.c
 *
 * Hash Emulator above SDKLT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/uft.h>
#include <bcm_int/ltsw/mbcm/uft.h>

#include <shr/shr_debug.h>
#include <sal/sal_types.h>

/******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCM_UFT

/******************************************************************************
 * Private functions
 */

/*******************************************************************************
 * Public Internal HSDK functions
 */

int
bcmint_uft_var_db_get(
    int unit,
    bcmint_uft_var_db_t **uft_var_db)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_uft_var_db_get(unit, uft_var_db));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initializes the UFT module.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_uft_init(int unit)
{

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_uft_init(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief De-initializes the uft module.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_uft_detach(int unit)
{

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_uft_detach(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the UFT mode.
 *
 * \param [in] unit Unit number.
 * \param [out] uft_mode UFT mode.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcmi_ltsw_uft_mode_get(int unit, bcmi_ltsw_uft_mode_t *uft_mode)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_uft_mode_get(unit, uft_mode));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the maximum number of entry moves for hash reordering.
 *
 * \param [in] unit Unit Number.
 * \param [in] type Move depth switch control type.
 * \param [in] arg Move depth value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_uft_multi_move_depth_set(
    int unit,
    bcm_switch_control_t type,
    int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_uft_multi_move_depth_set(unit, type, arg));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the maximum number of entry moves for hash reordering.
 *
 * \param [in] unit Unit Number.
 * \param [in] type Move depth switch control type.
 * \param [out] arg Move depth value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_uft_multi_move_depth_get(
    int unit,
    bcm_switch_control_t type,
    int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_uft_multi_move_depth_get(unit, type, arg));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get information about a hash bank.
 *
 * \param [in] unit Unit Number.
 * \param [in/out] bank_info Hash bank information structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_uft_bank_info_get(
    int unit,
    bcm_switch_hash_bank_info_t *bank_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_uft_bank_info_get(unit, bank_info));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the hash banks assigned to a hash table.
 *
 * \param [in] unit Unit Number.
 * \param [in] hash_table Hash table.
 * \param [in] array_size Array size of bank_array.
 * \param [out] bank_array Hash bank array.
 * \param [out] num_banks Actual number of hash banks returned in bank_array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_uft_bank_multi_get(
    int unit,
    bcm_switch_hash_table_t hash_table,
    int array_size,
    bcm_switch_hash_bank_t *bank_array,
    int *num_banks)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_uft_bank_multi_get(unit, hash_table, array_size, bank_array,
                                      num_banks));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the hash tables to which a given hash bank is assigned.
 *
 * \param [in] unit Unit Number.
 * \param [in] bank_num Hash bank number.
 * \param [in] array_size Array size of table_array.
 * \param [out] table_array Hash table array.
 * \param [out] num_tables Actual number of hash tables returned in table_array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_uft_table_multi_get(
    int unit,
    bcm_switch_hash_bank_t bank_num,
    int array_size,
    bcm_switch_hash_table_t *table_array,
    int *num_tables)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_uft_table_multi_get(unit, bank_num, array_size, table_array,
                                       num_tables));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief UFT bank control set function.
 *
 * \param [in] unit Unit Number.
 * \param [in] bank_control Hash bank control structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_uft_bank_control_set(
    int unit,
    bcm_switch_hash_bank_control_t *bank_control)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_uft_bank_control_set(unit, bank_control));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief UFT bank control get function.
 *
 * \param [in] unit Unit Number.
 * \param [in/out] bank_control Hash bank control structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_uft_bank_control_get(
    int unit,
    bcm_switch_hash_bank_control_t *bank_control)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_uft_bank_control_get(unit, bank_control));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Hash bank config set function.
 *
 * \param [in] unit Unit number.
 * \param [in] hash_table Hash table.
 * \param [in] bank_num Bank number.
 * \param [in] hash_type Hash type.
 * \param [in] hash_offset Hash offset.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_uft_bank_config_set(
    int unit,
    bcm_switch_hash_table_t hash_table,
    uint32_t bank_num,
    int hash_type,
    uint32_t hash_offset)

{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_uft_bank_config_set(unit, hash_table, bank_num, hash_type,
                                       hash_offset));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief UFT bank config get function.
 *
 * \param [in] unit Unit number.
 * \param [in] hash_table Hash table.
 * \param [in] bank_num Bank number.
 * \param [out] hash_type Hash type.
 * \param [out] hash_offset Hash offset.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_uft_bank_config_get(
    int unit,
    bcm_switch_hash_table_t hash_table,
    uint32_t bank_num,
    int *hash_type,
    uint32_t *hash_offset)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_uft_bank_config_get(unit, hash_table,bank_num, hash_type,
                                       hash_offset));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief UFT bank max num get function.
 *
 * \param [in] unit Unit number.
 * \param [in] hash_table Hash table.
 * \param [out] bank_count Bank count.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_uft_bank_max_get(
    int unit,
    bcm_switch_hash_table_t hash_table,
    uint32_t *bank_count)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_uft_bank_max_get(unit, hash_table, bank_count));

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public Functions
 */

