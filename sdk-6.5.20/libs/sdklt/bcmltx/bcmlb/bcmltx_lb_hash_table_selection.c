/*! \file bcmltx_lb_hash_table_selection.c
 *
 * Load balance hash table selection transform.
 *
 * This file contains hash table selection transform information for
 * LB_HASH_TRUNK_UC_OUTPUT_SELECTION LT
 * LB_HASH_TRUNK_NONUC_OUTPUT_SELECTION LT
 * LB_HASH_TRUNK_FAILOVER_OUTPUT_SELECTION LT
 * LB_HASH_TRUNK_SYSTEM_OUTPUT_SELECTION LT
 * LB_HASH_TRUNK_SYSTEM_FAILOVER_OUTPUT_SELECTION LT
 * LB_HASH_DLB_TRUNK_OUTPUT_SELECTION LT
 * LB_HASH_DLB_ECMP_LEVEL0_OUTPUT_SELECTION LT
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltx/bcmlb/bcmltx_lb_hash_table_select_trunk_uc.h>
#include <bcmltx/bcmlb/bcmltx_lb_hash_table_select_trunk_nonuc.h>
#include <bcmltx/bcmlb/bcmltx_lb_hash_table_select_trunk_failover.h>
#include <bcmltx/bcmlb/bcmltx_lb_hash_table_select_trunk_system.h>
#include <bcmltx/bcmlb/bcmltx_lb_hash_table_select_trunk_system_failover.h>
#include <bcmltx/bcmlb/bcmltx_lb_hash_table_select_dlb.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_internal.h>

#define BSL_LOG_MODULE BSL_LS_BCMLTX_LB

/*******************************************************************************
 * Private functions
 */

/*
 * \brief LB_HASH OUTPUT table forward transform
 *  for unicast trunk.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              In field array.
 * \param [out] out             Out field array.
 * \param [in]  arg             Handler arguments.
 *
 * \retval SHR_E_NONE  OK.
 * \retval !SHR_E_NONE ERROR.
 */
static
int bcmltx_lb_hash_table_select_transform(int unit,
                                          const bcmltd_fields_t *in,
                                          bcmltd_fields_t *out,
                                          const bcmltd_transform_arg_t *arg)
{
    uint32_t hash_inst = 0;

    SHR_FUNC_ENTER(unit);

    if (in->count == 0) {
        SHR_EXIT();
    }

    if (in->count != 1) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    hash_inst = in->field[0]->data;

    out->count = 0;

    /* __TABLE_SEL */
    out->field[out->count]->data = hash_inst;
    out->field[out->count]->id = arg->rfield[0];
    out->field[out->count]->idx = 0;
    out->count++;
exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief LB_HASH OUTPUT table reverse transform
 *  for unicast trunk.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              In field array.
 * \param [out] out             Out field array.
 * \param [in]  arg             Handler arguments.
 *
 * \retval SHR_E_NONE  OK.
 * \retval !SHR_E_NONE ERROR.
 */
static int
bcmltx_lb_hash_table_select_rev_transform(int unit,
                                          const bcmltd_fields_t *in,
                                          bcmltd_fields_t *out,
                                          const bcmltd_transform_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    if (in->count == 0) {
        SHR_EXIT();
    }

    /* count includes transform tables.` */
    if (in->count != 1) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    out->count = 0;

    /* TABLE_SEL to Hash instance */
    out->field[out->count]->id = arg->rfield[out->count];
    out->field[out->count]->data = in->field[0]->data;
    out->field[out->count]->idx = 0;
    out->count++;
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

/*
 * \brief LB_HASH OUTPUT table forward transform
 *  for unicast trunk.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              In field array.
 * \param [out] out             Out field array.
 * \param [in]  arg             Handler arguments.
 *
 * \retval SHR_E_NONE  OK.
 * \retval !SHR_E_NONE ERROR.
 */
int
bcmltx_lb_hash_table_select_trunk_uc_transform(
                                          int unit,
                                          const bcmltd_fields_t *in,
                                          bcmltd_fields_t *out,
                                          const bcmltd_transform_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(
       bcmltx_lb_hash_table_select_transform(unit,
                                             in,
                                             out,
                                             arg));
exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief LB_HASH OUTPUT table reverse transform
 *  for unicast trunk.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              In field array.
 * \param [out] out             Out field array.
 * \param [in]  arg             Handler arguments.
 *
 * \retval SHR_E_NONE  OK.
 * \retval !SHR_E_NONE ERROR.
 */
int
bcmltx_lb_hash_table_select_trunk_uc_rev_transform(
                                          int unit,
                                          const bcmltd_fields_t *in,
                                          bcmltd_fields_t *out,
                                          const bcmltd_transform_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(
       bcmltx_lb_hash_table_select_rev_transform(unit,
                                                 in,
                                                 out,
                                                 arg));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief LB_HASH OUTPUT table forward transform
 *  for non-unicast trunk.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              In field array.
 * \param [out] out             Out field array.
 * \param [in]  arg             Handler arguments.
 *
 * \retval SHR_E_NONE  OK.
 * \retval !SHR_E_NONE ERROR.
 */
int
bcmltx_lb_hash_table_select_trunk_nonuc_transform(
                                          int unit,
                                          const bcmltd_fields_t *in,
                                          bcmltd_fields_t *out,
                                          const bcmltd_transform_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(
       bcmltx_lb_hash_table_select_transform(unit,
                                             in,
                                             out,
                                             arg));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief LB_HASH OUTPUT table reverse transform
 *  for non-unicast trunk.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              In field array.
 * \param [out] out             Out field array.
 * \param [in]  arg             Handler arguments.
 *
 * \retval SHR_E_NONE  OK.
 * \retval !SHR_E_NONE ERROR.
 */
int
bcmltx_lb_hash_table_select_trunk_nonuc_rev_transform(
                                          int unit,
                                          const bcmltd_fields_t *in,
                                          bcmltd_fields_t *out,
                                          const bcmltd_transform_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(
       bcmltx_lb_hash_table_select_rev_transform(unit,
                                                 in,
                                                 out,
                                                 arg));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief LB_HASH OUTPUT table forward transform
 *  for trunk failvoer.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              In field array.
 * \param [out] out             Out field array.
 * \param [in]  arg             Handler arguments.
 *
 * \retval SHR_E_NONE  OK.
 * \retval !SHR_E_NONE ERROR.
 */
int
bcmltx_lb_hash_table_select_trunk_failover_transform(
                                          int unit,
                                          const bcmltd_fields_t *in,
                                          bcmltd_fields_t *out,
                                          const bcmltd_transform_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(
       bcmltx_lb_hash_table_select_transform(unit,
                                             in,
                                             out,
                                             arg));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief LB_HASH OUTPUT table reverse transform
 *  for trunk failover.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              In field array.
 * \param [out] out             Out field array.
 * \param [in]  arg             Handler arguments.
 *
 * \retval SHR_E_NONE  OK.
 * \retval !SHR_E_NONE ERROR.
 */
int
bcmltx_lb_hash_table_select_trunk_failover_rev_transform(
                                          int unit,
                                          const bcmltd_fields_t *in,
                                          bcmltd_fields_t *out,
                                          const bcmltd_transform_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(
       bcmltx_lb_hash_table_select_rev_transform(unit,
                                                 in,
                                                 out,
                                                 arg));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief LB_HASH OUTPUT table forward transform
 *  for system trunk.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              In field array.
 * \param [out] out             Out field array.
 * \param [in]  arg             Handler arguments.
 *
 * \retval SHR_E_NONE  OK.
 * \retval !SHR_E_NONE ERROR.
 */
int
bcmltx_lb_hash_table_select_trunk_system_transform(
                                          int unit,
                                          const bcmltd_fields_t *in,
                                          bcmltd_fields_t *out,
                                          const bcmltd_transform_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(
       bcmltx_lb_hash_table_select_transform(unit,
                                             in,
                                             out,
                                             arg));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief LB_HASH OUTPUT table reverse transform
 *  for system trunk.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              In field array.
 * \param [out] out             Out field array.
 * \param [in]  arg             Handler arguments.
 *
 * \retval SHR_E_NONE  OK.
 * \retval !SHR_E_NONE ERROR.
 */
int
bcmltx_lb_hash_table_select_trunk_system_rev_transform(
                                          int unit,
                                          const bcmltd_fields_t *in,
                                          bcmltd_fields_t *out,
                                          const bcmltd_transform_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(
       bcmltx_lb_hash_table_select_rev_transform(unit,
                                                 in,
                                                 out,
                                                 arg));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief LB_HASH OUTPUT table forward transform
 *  for system trunk failover.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              In field array.
 * \param [out] out             Out field array.
 * \param [in]  arg             Handler arguments.
 *
 * \retval SHR_E_NONE  OK.
 * \retval !SHR_E_NONE ERROR.
 */
int
bcmltx_lb_hash_table_select_trunk_system_failover_transform(
                                          int unit,
                                          const bcmltd_fields_t *in,
                                          bcmltd_fields_t *out,
                                          const bcmltd_transform_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(
       bcmltx_lb_hash_table_select_transform(unit,
                                             in,
                                             out,
                                             arg));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief LB_HASH OUTPUT table reverse transform
 *  for system trunk failover.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              In field array.
 * \param [out] out             Out field array.
 * \param [in]  arg             Handler arguments.
 *
 * \retval SHR_E_NONE  OK.
 * \retval !SHR_E_NONE ERROR.
 */
int
bcmltx_lb_hash_table_select_trunk_system_failover_rev_transform(
                                          int unit,
                                          const bcmltd_fields_t *in,
                                          bcmltd_fields_t *out,
                                          const bcmltd_transform_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(
       bcmltx_lb_hash_table_select_rev_transform(unit,
                                                 in,
                                                 out,
                                                 arg));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief LB_HASH OUTPUT table forward transform
 *  for dlb trunk.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              In field array.
 * \param [out] out             Out field array.
 * \param [in]  arg             Handler arguments.
 *
 * \retval SHR_E_NONE  OK.
 * \retval !SHR_E_NONE ERROR.
 */
int
bcmltx_lb_hash_table_select_dlb_transform(int unit,
                                          const bcmltd_fields_t *in,
                                          bcmltd_fields_t *out,
                                          const bcmltd_transform_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(
       bcmltx_lb_hash_table_select_transform(unit,
                                             in,
                                             out,
                                             arg));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief LB_HASH OUTPUT table reverse transform
 *  for dlb trunk.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              In field array.
 * \param [out] out             Out field array.
 * \param [in]  arg             Handler arguments.
 *
 * \retval SHR_E_NONE  OK.
 * \retval !SHR_E_NONE ERROR.
 */
int
bcmltx_lb_hash_table_select_dlb_rev_transform(int unit,
                                              const bcmltd_fields_t *in,
                                              bcmltd_fields_t *out,
                                          const bcmltd_transform_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(
       bcmltx_lb_hash_table_select_rev_transform(unit,
                                                 in,
                                                 out,
                                                 arg));
exit:
    SHR_FUNC_EXIT();
}
