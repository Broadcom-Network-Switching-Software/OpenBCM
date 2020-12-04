/*! \file bcmcfg_read_null.c
 *
 * null YAML file reader
 *
 * These functions are called if some component requests YAML
 * configuration parsing, but a YAML library was not configured.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <shr/shr_debug.h>
#include <bcmcfg/bcmcfg.h>
#include <bcmcfg/bcmcfg_reader.h>
#include <shr/shr_error.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMCFG_READ

/*!
 * \brief BCMCFG File parser stub.
 *
 * YAML is unavailable so return an error.
 *
 * \param [in]  file            File to parse.
 *
 * \retval SHR_E_UNAVAIL
 */
int
bcmcfg_file_parse(const char *file)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_IF_ERR_CONT(SHR_E_UNAVAIL);
    SHR_FUNC_EXIT();
}

/*!
 * \brief BCMCFG String parser stub.
 *
 * YAML is unavailable so return an error.
 *
 * \param [in]  str             String to parse.
 *
 * \retval SHR_E_UNAVAIL
 */
int
bcmcfg_string_parse(const char *str)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_IF_ERR_CONT(SHR_E_UNAVAIL);
    SHR_FUNC_EXIT();
}

/*!
 * \brief BCMCFG Null generic reader initialization.
 *
 * YAML unavailable so nothing to do.
 *
 * \retval SHR_E_NONE
 */
int
bcmcfg_read_gen_init(void)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_FUNC_EXIT();
}

/*!
 * \brief BCMCFG Null generic reader cleanup.
 *
 * YAML unavailable so nothing to do.
 *
 * \retval SHR_E_NONE
 */
int
bcmcfg_read_gen_cleanup(void)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_FUNC_EXIT();
}

/*!
 * \brief BCMCFG Null unit reader initialization.
 *
 * YAML unavailable so nothing to do.
 *
 * \retval SHR_E_NONE
 */
int
bcmcfg_read_unit_init(int unit,
                      bcmcfg_init_stage_t stage)
{
    SHR_FUNC_ENTER(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief BCMCFG Null unit reader cleanup.
 *
 * YAML unavailable so nothing to do.
 *
 * \retval SHR_E_NONE
 */
int
bcmcfg_read_unit_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get read data.
 *
 * YAML unavailable so nothing to do.
 *
 * \retval SHR_E_NONE
 */
int
bcmcfg_read_data_get(const bcmcfg_read_data_t **read)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clean up read data.
 *
 * YAML unavailable so nothing to do.
 *
 * \retval SHR_E_NONE
 */
int
bcmcfg_read_data_cleanup(void)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_FUNC_EXIT();
}
