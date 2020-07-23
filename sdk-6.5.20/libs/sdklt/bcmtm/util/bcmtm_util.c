/*! \file bcmtm_util.c
 *
 * TM Driver utility functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_pb.h>

#include <bcmdrd/bcmdrd_internal.h>
#include <bcmbd/bcmbd_mmu_intr.h>
#include <bcmtm/bcmtm_types.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmtm/multicast/bcmtm_mc_group.h>
#include <bcmtm/obm/bcmtm_obm_internal.h>
#include <bcmtm/imm/bcmtm_imm_defines_internal.h>

#define BSL_LOG_MODULE  BSL_LS_BCMTM_UTIL

/*******************************************************************************
 * Local definitions
 */

/*! TM port operation handler function. */
typedef int
(*tm_port_op_f)(int unit, bcmtm_pport_t pport, uint32_t param);

/*! This structure is used to define the operation for the TM. */
typedef struct tm_port_op_hdl_s {

    /*! Operation name. */
    char *name;

    /*! Operation handler. */
    tm_port_op_f func;

} tm_port_op_hdl_t;

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief Convert the union value bcmlt_field_data_t to uint64_t.
 *
 * \param [in] width Field width in bits.
 * \param [in] field_data Field map data which includes the max and min field
 *                        value in bcmlt_field_data_t.
 * \param [out] min Minimum field value in uint64_t.
 * \param [out] max Maximum field value in uint64_t.
 */
static void
bcmtm_lt_field_data_to_u64(uint32_t width,
                           const bcmlrd_field_data_t *field_data,
                           uint64_t *min, uint64_t *max, uint64_t *def)
{

    if (width == 1) {
        *min = (uint64_t)field_data->min->is_true;
        *max = (uint64_t)field_data->max->is_true;
        *def = (uint64_t)field_data->def->is_true;
    } else if (width <= 8) {
        *min = (uint64_t)field_data->min->u8;
        *max = (uint64_t)field_data->max->u8;
        *def = (uint64_t)field_data->def->u8;
    } else if (width <= 16) {
        *min = (uint64_t)field_data->min->u16;
        *max = (uint64_t)field_data->max->u16;
        *def = (uint64_t)field_data->def->u8;
    } else if (width <= 32) {
        *min = (uint64_t)field_data->min->u32;
        *max = (uint64_t)field_data->max->u32;
        *def = (uint64_t)field_data->def->u8;
    } else {
        *min = (uint64_t)field_data->min->u64;
        *max = (uint64_t)field_data->max->u64;
        *def = (uint64_t)field_data->def->u8;
    }
}

/*!
 * \brief Checks the field value passed is within the allowed range.
 *
 * \param fval [in] Field value to be verified.
 * \param fdata[in] Field data.
 *
 * \return 1 The field value is within the allowed range.
 * \return 0 The field value is not in permitted range.
 */
static int
bcmtm_fval_range_check(uint64_t fval,
                       const bcmlrd_field_data_t *fdata,
                       uint32_t width)
{
    uint64_t min, max, def;
    bcmtm_lt_field_data_to_u64(width, fdata, &min, &max, &def);
    return (((fval >= min) && (fval <= max)) ? 1: 0);
}

/*!
 * \brief Execute ingress path up/down sequence for a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Physical port number.
 * \param [in] up Execute 'up' sequence if non-zero, otherwise execute 'down'
 *                sequence.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
tm_port_ing_up(int unit, bcmtm_pport_t port, uint32_t up)
{
    if (up) {
        return bcmtm_port_ing_up(unit, port);
    }

    return bcmtm_port_ing_down(unit, port);
}

/*!
 * \brief Execute MMU path up/down sequence for a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Physical port number.
 * \param [in] up Execute 'up' sequence if non-zero, otherwise execute 'down'
 *                sequence.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
tm_port_mmu_up(int unit, bcmtm_pport_t port, uint32_t up)
{
    if (up) {
        return bcmtm_port_mmu_up(unit, port);
    }

    return bcmtm_port_mmu_down(unit, port);
}

/*!
 * \brief Execute egress path up/down sequence for a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Physical port number.
 * \param [in] up Execute 'up' sequence if non-zero, otherwise execute 'down'
 *                sequence.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
tm_port_egr_up(int unit, bcmtm_pport_t port, uint32_t up)
{
    if (up) {
        return bcmtm_port_egr_up(unit, port);
    }

    return bcmtm_port_egr_down(unit, port);
}

/*!
 * \brief Egress port credit reset for port up/down sequence.
 *
 * \param [in] unit Unit number.
 * \param [in] port Physical port number.
 * \param [in] up Execute 'up' sequence if non-zero, otherwise execute 'down'
 *             sequence.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
tm_port_egr_credit_reset(int unit, bcmtm_pport_t port, uint32_t up)
{
    return bcmtm_port_egr_credit_reset_clr(unit, port);
}

/*!
 * \brief Egress forwarding enable/disable for port up/down sequence.
 *
 * \param [in] unit Unit number.
 * \param [in] port Physical port number.
 * \param [in] up Execute 'up' sequence if non-zero, otherwise execute 'down'
 *             sequence.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
tm_port_egr_fwd_enable(int unit, bcmtm_pport_t port, uint32_t up)
{
    if (up) {
        return bcmtm_port_egr_fwd_enable(unit, port);
    }
    return bcmtm_port_egr_fwd_disable(unit, port);
}

/*!
 * \brief Check if logical port is a spare port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 *
 * \retval TRUE Logical port is spare.
 * \retval FALSE Logical port is not spare.
 */
static int
bcmtm_lport_is_spare(int unit, bcmtm_lport_t lport)
{
    if (bcmdrd_lport_type_get(unit, lport) == BCMDRD_PORT_TYPE_RSVD) {
        return true;
    }
    return false;
}

/*!
 * \brief BCMTM port map reset.
 * This reset all the port mapping for a given port.
 *
 * \param [in] unit Logical unit number.
 * \param [in] pport Physical port number.
 */
static void
bcmtm_port_map_reset (int unit,
                      int pport)
{
    bcmtm_port_map_info_t *port_map;
    int lport, mport;

    bcmtm_port_map_get(unit, &port_map);

    lport = port_map->pport_map[pport].lport;
    mport = port_map->pport_map[pport].mport;

    sal_memset(&(port_map->pport_map[pport]), -1, sizeof(bcmtm_pport_map_t));
    sal_memset(&(port_map->lport_map[lport]), -1, sizeof(bcmtm_lport_map_t));
    sal_memset(&(port_map->mport_map[mport]), -1, sizeof(bcmtm_mport_map_t));
}


/*******************************************************************************
 * Public functions
 */

bcmdrd_sid_t
bcmtm_pt_sid_unique_xpe_pipe(int unit, bcmdrd_sid_t sid, int xpe, int pipe)
{
    bcmtm_drv_t *tm_drv;

    (void)(bcmtm_drv_get(unit, &tm_drv));

    return (tm_drv->pt_sid_uniq_acc(unit, sid, xpe, pipe));
}

int
bcmtm_lport_num_mcq_get(int unit,
                       bcmtm_lport_t lport,
                       int *num_mcq)
{
    bcmtm_port_map_info_t *port_map;

    SHR_FUNC_ENTER(unit);
    bcmtm_port_map_get(unit, &port_map);
    *num_mcq = port_map->lport_map[lport].num_mcq;
    if (*num_mcq == -1) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_lport_num_ucq_get(int unit,
                        bcmtm_lport_t lport,
                        int *num_ucq)
{
    bcmtm_port_map_info_t *port_map;

    SHR_FUNC_ENTER(unit);
    bcmtm_port_map_get(unit, &port_map);
    *num_ucq = port_map->lport_map[lport].num_ucq;
    if (*num_ucq == -1) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_itm_valid_get(int unit, int itm)
{
    bcmtm_drv_t *tm_drv;

    SHR_FUNC_ENTER(unit);

    (void)(bcmtm_drv_get(unit, &tm_drv));

    if (!tm_drv->itm_valid_get) {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT
        (tm_drv->itm_valid_get(unit, itm));

exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_lport_mcq_base_get(int unit,
                        bcmtm_lport_t lport,
                        int *mcq_base)
{
    bcmtm_port_map_info_t *port_map;

    SHR_FUNC_ENTER(unit);
    bcmtm_port_map_get(unit, &port_map);
    *mcq_base = port_map->lport_map[lport].base_mcq;
    if (*mcq_base == -1) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_lport_ucq_base_get(int unit,
                         bcmtm_lport_t lport,
                         int *ucq_base)
{
    bcmtm_port_map_info_t *port_map;

    SHR_FUNC_ENTER(unit);
    bcmtm_port_map_get(unit, &port_map);
    *ucq_base = port_map->lport_map[lport].base_ucq;
    if (*ucq_base == -1) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_lport_is_cpu(int unit, bcmtm_lport_t lport)
{
    if (bcmdrd_lport_type_get(unit, lport) == BCMDRD_PORT_TYPE_CPU) {
        return true;
    }
    return false;
}

int
bcmtm_lport_is_lb(int unit, bcmtm_lport_t lport)
{
    if (bcmdrd_lport_type_get(unit, lport) == BCMDRD_PORT_TYPE_LB) {
        return true;
    }
    return false;
}

int bcmtm_lport_is_rdb(int unit, bcmtm_lport_t lport)
{
    if (bcmdrd_lport_type_get(unit, lport) == BCMDRD_PORT_TYPE_RDB) {
        return true;
    }
    return false;
}

int
bcmtm_lport_is_mgmt(int unit, bcmtm_lport_t lport)
{
    if (bcmdrd_lport_type_get(unit, lport) == BCMDRD_PORT_TYPE_MGMT) {
        return true;
    }
    return false;
}

int
bcmtm_lport_is_fp(int unit, bcmtm_lport_t lport)
{
    if ((bcmtm_lport_is_mgmt(unit,  lport)) ||
            (bcmtm_lport_is_lb(unit, lport)) ||
            (bcmtm_lport_is_cpu(unit, lport)) ||
            (bcmtm_lport_is_rdb(unit, lport)) ||
            (bcmtm_lport_is_spare(unit, lport))) {
        return false;
    }
    return true;
}

int
bcmtm_lport_mport_get(int unit,
                         bcmtm_lport_t lport,
                         int *mmu_port)
{
    bcmtm_port_map_info_t *port_map;

    SHR_FUNC_ENTER(unit);
    bcmtm_port_map_get(unit, &port_map);
    *mmu_port = port_map->lport_map[lport].mport;
    if (*mmu_port == -1) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_lport_mchip_port_get(int unit,
                              bcmtm_lport_t lport,
                              int *mmu_chip_port)
{
    bcmtm_port_map_info_t *port_map;

    SHR_FUNC_ENTER(unit);
    bcmtm_port_map_get(unit, &port_map);
    *mmu_chip_port = port_map->lport_map[lport].mchip_port;
    if (*mmu_chip_port == -1) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_pport_lport_get(int unit,
                      bcmtm_pport_t pport,
                      int *lport)
{
    bcmtm_port_map_info_t *port_map;

    SHR_FUNC_ENTER(unit);
    bcmtm_port_map_get(unit, &port_map);
    *lport = port_map->pport_map[pport].lport;
    if (*lport == -1) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_lport_pport_get(int unit,
                      bcmtm_lport_t lport,
                      int *pport)
{
    bcmtm_port_map_info_t *port_map;

    SHR_FUNC_ENTER(unit);
    bcmtm_port_map_get(unit, &port_map);
    *pport = port_map->lport_map[lport].pport;
    if (*pport == -1) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_lport_max_speed_get(int unit,
                          bcmtm_lport_t lport,
                          int *max_port_speed)
{
    bcmtm_port_map_info_t *port_map;
    int pport;

    SHR_FUNC_ENTER(unit);
    bcmtm_port_map_get(unit, &port_map);
    pport = port_map->lport_map[lport].pport;
    if (pport == -1) {
        *max_port_speed = -1;
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }
    *max_port_speed = port_map->lport_map[lport].max_speed;
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_lport_pipe_get(int unit,
                    bcmtm_lport_t lport,
                    int *pipe)
{
    bcmtm_port_map_info_t *port_map;

    SHR_FUNC_ENTER(unit);
    bcmtm_port_map_get(unit, &port_map);
    *pipe = port_map->lport_map[lport].pipe;
    if (*pipe == -1) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_lport_mmu_local_port_get(int unit,
                               bcmtm_lport_t lport,
                               int *mmu_lport)
{
    bcmtm_port_map_info_t *port_map;

    SHR_FUNC_ENTER(unit);
    bcmtm_port_map_get(unit, &port_map);
    *mmu_lport = port_map->lport_map[lport].mlocal_port;
    if (*mmu_lport == -1) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_mport_lport_get(int unit,
                      int mport,
                      int *lport)
{
    bcmtm_port_map_info_t *port_map;

    SHR_FUNC_ENTER(unit);
    bcmtm_port_map_get(unit, &port_map);
    *lport = port_map->mport_map[mport].lport;
    if (*lport == -1) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_mchip_port_lport_get(int unit,
                           int mchip_port,
                           int *lport)
{
    bcmtm_drv_t *drv;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmtm_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv, SHR_E_PARAM);
    SHR_NULL_CHECK(drv->mchip_port_lport_get, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(drv->mchip_port_lport_get(unit, mchip_port, lport));
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_port_ing_down(int unit, bcmtm_pport_t port)
{
    bcmtm_drv_t *tm_drv;

    SHR_FUNC_ENTER(unit);

    (void)(bcmtm_drv_get(unit, &tm_drv));

    if (!tm_drv->port_ing_down) {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT
        (tm_drv->port_ing_down(unit, port));

exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_port_ing_up(int unit, bcmtm_pport_t port)
{
    bcmtm_drv_t *tm_drv;

    SHR_FUNC_ENTER(unit);

    (void)(bcmtm_drv_get(unit, &tm_drv));

    if (!tm_drv->port_ing_up) {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT
        (tm_drv->port_ing_up(unit, port));

exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_port_mmu_down(int unit, bcmtm_pport_t port)
{
    bcmtm_drv_t *tm_drv;

    SHR_FUNC_ENTER(unit);

    (void)(bcmtm_drv_get(unit, &tm_drv));

    if (!tm_drv->port_mmu_down) {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT
        (tm_drv->port_mmu_down(unit, port));

exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_port_mmu_up(int unit, bcmtm_pport_t port)
{
    bcmtm_drv_t *tm_drv;

    SHR_FUNC_ENTER(unit);

    (void)(bcmtm_drv_get(unit, &tm_drv));

    if (!tm_drv->port_mmu_up) {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT
        (tm_drv->port_mmu_up(unit, port));

exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_port_egr_down(int unit, bcmtm_pport_t port)
{
    bcmtm_drv_t *tm_drv;

    SHR_FUNC_ENTER(unit);

    (void)(bcmtm_drv_get(unit, &tm_drv));

    if (!tm_drv->port_egr_down) {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT
        (tm_drv->port_egr_down(unit, port));

exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_port_egr_up(int unit, bcmtm_pport_t port)
{
    bcmtm_drv_t *tm_drv;

    SHR_FUNC_ENTER(unit);

    (void)(bcmtm_drv_get(unit, &tm_drv));

    if (!tm_drv->port_egr_up) {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT
        (tm_drv->port_egr_up(unit, port));

exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_port_egr_credit_reset_clr(int unit, bcmtm_pport_t port)
{
    bcmtm_drv_t *tm_drv;

    SHR_FUNC_ENTER(unit);

    (void)(bcmtm_drv_get(unit, &tm_drv));

    if (!tm_drv->port_egr_credit_reset) {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT
        (tm_drv->port_egr_credit_reset(unit, port));
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_port_egr_fwd_enable(int unit, bcmtm_pport_t port)
{
    bcmtm_drv_t *tm_drv;

    SHR_FUNC_ENTER(unit);

    (void)(bcmtm_drv_get(unit, &tm_drv));

    if (!tm_drv->port_egr_fwd_enable) {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT
        (tm_drv->port_egr_fwd_enable(unit, port));
exit:
    SHR_FUNC_EXIT();

}

int
bcmtm_port_egr_fwd_disable(int unit, bcmtm_pport_t port)
{
    bcmtm_drv_t *tm_drv;

    SHR_FUNC_ENTER(unit);

    (void)(bcmtm_drv_get(unit, &tm_drv));

    if (!tm_drv->port_egr_fwd_disable) {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT
        (tm_drv->port_egr_fwd_disable(unit, port));
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_port_op_exec(int unit, bcmpc_pport_t pport,
                   char *op_name, uint32_t op_param)
{
    static tm_port_op_hdl_t port_ops[] = {
        { "ing_up", tm_port_ing_up },
        { "mmu_up", tm_port_mmu_up },
        { "egr_up", tm_port_egr_up },
        { "ep_credit", tm_port_egr_credit_reset },
        { "fwd_enable", tm_port_egr_fwd_enable},
    };
    tm_port_op_hdl_t *op_hdl;
    int i;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < COUNTOF(port_ops); i++) {
        op_hdl = &port_ops[i];

        if (sal_strcmp(op_name, op_hdl->name) != 0) {
            continue;
        }

        SHR_IF_ERR_EXIT
            (op_hdl->func(unit, pport, op_param));

        SHR_EXIT();
    }

    LOG_ERROR(BSL_LOG_MODULE,
              (BSL_META_U(unit, "Operation not supported[%s]\n"), op_name));

    SHR_ERR_EXIT(SHR_E_UNAVAIL);

exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_fval_get_from_field_array (int unit,
                                 bcmlrd_fid_t fid,
                                 uint32_t idx,
                                 const bcmltd_field_t *field_array,
                                 uint64_t *fval)
{
    bcmltd_field_t *field = (bcmltd_field_t *)field_array;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(field, SHR_E_NOT_FOUND);

    while (field) {
        if ((field->id == fid) && (field->idx == idx)){
            *fval = field->data;
            break;
        }
        field =  field->next;
    }
    if (!field) {
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT();
}

void
bcmtm_field_list_free(bcmltd_fields_t *flist)
{
    if (flist) {
        if (flist->field) {
            SHR_FREE(flist->field[0]);
        }
        SHR_FREE(flist->field);
        flist = NULL;
    }
}

int
bcmtm_field_list_alloc(int unit,
                       size_t num_fields,
                       bcmltd_fields_t *flist)
{
    bcmltd_field_t *mem = NULL;
    uint32_t count = 0;

    SHR_FUNC_ENTER(unit);

    if (!num_fields) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    flist->count = num_fields;
    flist->field = NULL;
    SHR_ALLOC(flist->field, num_fields * sizeof(bcmltd_field_t *), "bcmtmLtFieldList");
    SHR_NULL_CHECK( flist->field, SHR_E_MEMORY);

    SHR_ALLOC(mem, num_fields * sizeof(bcmltd_field_t), "bcmtmLtFieldList");
    SHR_NULL_CHECK( mem, SHR_E_MEMORY);
    sal_memset( mem, 0, (num_fields * sizeof(bcmltd_field_t)));

    for (count = 0; count < num_fields; count++) {
        flist->field[count] = mem + count;
        if (count > 0) {
            flist->field[count-1]->next = flist->field[count];
        }
    }
exit:
    if (SHR_FUNC_VAL_IS(SHR_E_MEMORY)) {
        SHR_FREE(flist->field);
    }
    SHR_FUNC_EXIT();
}

void
bcmtm_field_linked_list_free(bcmltd_field_t *head)
{
    bcmltd_field_t *tmp;

    while (head != NULL)
    {
       tmp = head;
       head = head->next;
       SHR_FREE(tmp);
    }
}

int
bcmtm_field_list_set(int unit,
                     bcmltd_fields_t *flist,
                     bcmlrd_fid_t fid,
                     uint32_t idx,
                     uint64_t val)
{
    size_t count;

    SHR_FUNC_ENTER(unit);

    for (count = 0; count < flist->count; count++) {
        if ((flist->field[count]->id == fid) &&
            (flist->field[count]->idx == idx)) {
            flist->field[count]->data = val;
            flist->field[count]->flags = 0;
            SHR_EXIT();
        }
    }
    if (!flist->field[count]) {
        SHR_ERR_EXIT(SHR_E_FULL);
    }
    flist->field[count]->id = fid;
    flist->field[count]->data = val;
    flist->field[count]->idx = idx;
    flist->field[count]->flags = 0;
    flist->count++;
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_field_list_get(int unit,
                     bcmltd_fields_t *flist,
                     bcmlrd_fid_t fid,
                     uint32_t idx,
                     uint64_t *val)
{
    size_t count;
    for (count = 0; count < flist->count; count++) {
        if ((flist->field[count]->id == fid) &&
            (flist->field[count]->idx == idx)) {
            *val = flist->field[count]->data;
            return SHR_E_NONE;
        }
    }
    return SHR_E_NOT_FOUND;
}

int
bcmtm_default_entry_array_construct(int unit, bcmlrd_sid_t sid,
                                    bcmltd_fields_t *entry)
{
    uint64_t fval;
    uint32_t fsize = 1;
    size_t num_fid, idx;
    bcmlrd_fid_t *all_fields = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get all the fields from LRD. */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, sid, &num_fid));
    all_fields = sal_alloc(num_fid * sizeof(bcmlrd_fid_t), "bcmtmLtFieldList");
    sal_memset(all_fields, 0, num_fid * sizeof(bcmlrd_fid_t));
    SHR_IF_ERR_EXIT
        (bcmlrd_field_list_get(unit, sid, num_fid, all_fields, &num_fid));

    /*
     * Populate default entry (value fields) with HW default values.
     * Key fields must be included.
     */
    for (idx = 0; idx < num_fid; idx++) {
        SHR_IF_ERR_EXIT
            (bcmlrd_field_default_get(unit, sid, all_fields[idx], fsize, &fval,
                                      &fsize));
        entry->field[idx]->id   = all_fields[idx];
        entry->field[idx]->data = fval;
        entry->field[idx]->idx = 0;
    }

exit:
    SHR_FREE(all_fields);
    SHR_FUNC_EXIT();
}

int
bcmtm_default_entry_ll_construct(int unit, bcmlrd_sid_t sid,
                                 bcmltd_field_t **data_fields)
{
    uint64_t fval;
    uint32_t fsize = 1;
    size_t num_fid, idx;
    bcmlrd_fid_t *all_fields = NULL;
    bcmltd_field_t *tmp = NULL;
    bcmltd_field_t *data_ptr;

    SHR_FUNC_ENTER(unit);

    /* Get all the fields from LRD. */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, sid, &num_fid));
    all_fields = sal_alloc(num_fid * sizeof(bcmlrd_fid_t), "bcmtmLtFieldList");
    sal_memset(all_fields, 0, num_fid * sizeof(bcmlrd_fid_t));
    SHR_IF_ERR_EXIT
        (bcmlrd_field_list_get(unit, sid, num_fid, all_fields, &num_fid));

    /* Create data_fields with default entries. */
    tmp = sal_alloc(num_fid * sizeof(bcmltd_field_t), "bcmtmLtFieldList");
    data_ptr = tmp;
    for (idx = 0; idx < num_fid; idx++) {
        if (idx > 0) {
            data_ptr->next = data_ptr + 1;
            data_ptr = data_ptr->next;
        }
        SHR_IF_ERR_EXIT
            (bcmlrd_field_default_get(unit, sid, all_fields[idx], fsize, &fval,
                                      &fsize));
        data_ptr->id   = all_fields[idx];
        data_ptr->data = fval;
    }
    data_ptr->next = NULL;
    *data_fields = tmp;

exit:
    SHR_FREE(all_fields);
    SHR_FUNC_EXIT();
}

int
bcmtm_field_list_size_check(int unit, bcmltd_fields_t *flist, size_t size)
{
    if (flist->count < size) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "output_fields allocated by IMM callback is too "
                              "small to hold the desired fields.\n")));
        return SHR_E_MEMORY;
    }
    return SHR_E_NONE;
}


typedef struct tm_port_ops_s {
    int port_add;
    int port_delete;
} tm_port_ops_t;

#define MAX_PHYS_PORTS 512
int
bcmtm_pc_tm_configure(int unit,
                      size_t num_ports,
                      const bcmpc_mmu_port_cfg_t *old_cfg,
                      const bcmpc_mmu_port_cfg_t *new_cfg)
{
    bcmtm_port_map_info_t *port_map;
    int pport, lport, old_lport, idx;
    bcmtm_drv_t *tm_drv;
    int old_speed, new_speed;
    tm_port_ops_t port_ops[MAX_PHYS_PORTS];
    bool add_flag = false;

    SHR_FUNC_ENTER(unit);

    bcmtm_port_map_get(unit, &port_map);
    SHR_IF_ERR_EXIT(bcmtm_drv_get(unit, &tm_drv));

    for (idx = 0; idx < (int)num_ports; idx++) {
        pport = new_cfg[idx].pport;
        if (pport == BCMPC_INVALID_LPORT) {
            continue;
        }
        port_ops[pport].port_add = 0;
        port_ops[pport].port_delete = 0;
    }

    for (idx = 0; idx < (int)num_ports; idx++) {
        pport = new_cfg[idx].pport;
        if (pport == BCMPC_INVALID_LPORT) {
            continue;
        }
        lport = new_cfg[idx].lport;
        old_lport = (bcmpc_lport_t)port_map->pport_map[pport].lport;
        if ((old_lport == BCMPC_INVALID_LPORT) &&
                (lport == BCMPC_INVALID_LPORT)) {
            /* No change in port state; Do nothing */
            continue;
        } else if ((old_lport != BCMPC_INVALID_LPORT) &&
            (lport == BCMPC_INVALID_LPORT)) {
            /* Deleted port */
             port_ops[pport].port_delete = 1;
        } else if ((old_lport == BCMPC_INVALID_LPORT) &&
            (lport != BCMPC_INVALID_LPORT)) {
            /* Newly added port */
            port_ops[pport].port_add = 1;
        } else {
            old_speed = port_map->lport_map[lport].cur_speed;
            new_speed = new_cfg[idx].speed_cur;
            if (old_speed != new_speed) {
                /* Port speed change */
                if (new_speed == 0) {
                    /* Port link down */
                    continue;
                }
                if (!bcmtm_lport_is_fp(unit, lport)) {
                    /* Speed change not supported in TM for CPU, LB and MGMT
                     * ports */
                    continue;
                }
                port_ops[pport].port_add = 1;
                port_ops[pport].port_delete = 1;
            }
        }
    }
    /* Call only for newly disabled ports */
    /* Assume port down is called by PC directly before PC TM configure */
    for (idx = 0; idx < (int)num_ports; idx++) {
        pport = new_cfg[idx].pport;
        if (pport == BCMPC_INVALID_LPORT) {
            continue;
        }
        if (port_ops[pport].port_delete == 1) {
            lport = new_cfg[idx].lport;
            old_lport = (bcmpc_lport_t)port_map->pport_map[pport].lport;
            if (tm_drv->tm_port_delete) {
                if (tm_drv->update_unicast_drop_config) {
                    SHR_IF_ERR_EXIT
                    (tm_drv->update_unicast_drop_config(unit,
                                                        pport,
                                                        old_lport,
                                                        false));
                }
                SHR_IF_ERR_EXIT(tm_drv->tm_port_delete(unit, pport));
            }
            /* Delete ports from IMM before re-populating drv_info */
            if (tm_drv->imm_update) {
                SHR_IF_ERR_EXIT
                    (tm_drv->imm_update(unit, old_lport, ACTION_PORT_DELETE_INT));
            }
            /* Delete removed ports from drv_info */
            bcmtm_port_map_reset(unit, pport);
        }
    }


    /* Update drv_info for any changes to existing ports or new ports added */
    if (tm_drv->portmap_update) {
        tm_drv->portmap_update(unit, num_ports, old_cfg, new_cfg);
    }

    bcmtm_drv_info_print(unit);

    /* Call only for newly enabled  or speed-changed ports */
    for (idx = 0; idx < (int)num_ports; idx++) {
        pport = new_cfg[idx].pport;
        if (pport == BCMPC_INVALID_LPORT) {
            continue;
        }
        if (port_ops[pport].port_add == 1) {
            lport = new_cfg[idx].lport;
            /* New port addition */
            if (tm_drv->tm_port_add) {
                add_flag = true;
                SHR_IF_ERR_EXIT(tm_drv->tm_port_add(unit, pport));
            }
            /* Add ports from IMM after re-populating drv_info */
            if (tm_drv->imm_update) {
                SHR_IF_ERR_EXIT
                    (tm_drv->imm_update(unit, lport, ACTION_PORT_UPDATE_INT));
            }

            if (bcmtm_lport_is_cpu(unit, lport) ||
                bcmtm_lport_is_lb(unit, lport)) {
                if (tm_drv->port_egr_up) {
                    tm_drv->port_egr_up(unit, pport);
                }
                if (tm_drv->port_ing_up) {
                    tm_drv->port_ing_up(unit, pport);
                }
                if (tm_drv->port_egr_fwd_enable) {
                    tm_drv->port_egr_fwd_enable(unit, pport);
                }
            }
        }
    }

    /* Call only if there was port add operation performed. */
    if (tm_drv->update_unicast_drop_config) {
        if (add_flag) {
            SHR_IF_ERR_EXIT
               (tm_drv->update_unicast_drop_config(unit,
                                                   0,
                                                   0,
                                                   add_flag));
        }
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_lt_field_validate(int unit,
        bcmlt_opcode_t opcode,
        bcmlrd_sid_t sid,
        const bcmltd_fields_t *in)
{
    const bcmlrd_field_data_t *fdata;
    const bcmlrd_table_rep_t *tbl;
    bcmlrd_fid_t fid = 0;
    size_t count = 0;
    uint16_t input = 0;
    uint16_t key_field = 0;

    SHR_FUNC_ENTER(unit);
    tbl = bcmlrd_table_get(sid);
    SHR_NULL_CHECK(tbl, SHR_E_PARAM);

    while (count < in->count) {
        fid = in->field[count]->id;
        input |= (1 << fid);
        SHR_IF_ERR_EXIT(bcmlrd_field_get(unit, sid, fid, &fdata));
        /*check for max and min values for the fields */
        if(!bcmtm_fval_range_check(in->field[count]->data,
                                   fdata,
                                   tbl->field[fid].width)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        /* check for read only fields in "in data" for insert and update only */
        if ((opcode == BCMLT_OPCODE_INSERT)|| (opcode == BCMLT_OPCODE_UPDATE)){
            if (fdata->flags & BCMLRD_FIELD_F_READ_ONLY){
                SHR_ERR_EXIT(SHR_E_ACCESS);
            }
        }
        count++;
    }
    /* verify whether all key fields are present */
    count = 0;
    while (count < tbl->fields) {
        if (tbl->field[count].flags & BCMLRD_FIELD_F_KEY) {
            key_field |= ( 1 << count);
        }
        count++;
    }
    if ((input & key_field) != key_field) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_field_width_get(int unit,
                      bcmlrd_sid_t sid,
                      bcmlrd_fid_t fid,
                      size_t *width)
{
    const bcmlrd_field_data_t *field_data;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlrd_field_get(unit, sid, fid, &field_data));

    *width = field_data->width;
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_field_value_range_get(int unit,
                            bcmlrd_sid_t sid,
                            uint32_t fid,
                            uint64_t *min, uint64_t *max)
{
    const bcmlrd_field_data_t *field_data;
    const bcmlrd_table_rep_t *tab_md;
    const bcmltd_field_rep_t *field_md;
    uint64_t def;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlrd_field_get(unit, sid, fid, &field_data));

    tab_md = bcmlrd_table_get(sid);

    SHR_NULL_CHECK(tab_md, SHR_E_FAIL);
    if (fid >= tab_md->fields) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    field_md = &(tab_md->field[fid]);
    bcmtm_lt_field_data_to_u64(field_md->width, field_data, min, max, &def);

exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_field_default_value_get(int unit, bcmlrd_sid_t sid, uint32_t fid,
                                uint64_t *def)
{
    const bcmlrd_field_data_t *field_data;
    const bcmlrd_table_rep_t *tab_md;
    const bcmltd_field_rep_t *field_md;
    uint64_t min, max;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlrd_field_get(unit, sid, fid, &field_data));

    tab_md = bcmlrd_table_get(sid);

    SHR_NULL_CHECK(tab_md, SHR_E_FAIL);
    if (fid >= tab_md->fields) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    field_md = &(tab_md->field[fid]);
    bcmtm_lt_field_data_to_u64(field_md->width, field_data, &min, &max, def);

exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_in_field_parser (int unit,
        const bcmltd_fields_t *in,
        bcmlrd_sid_t sid,
        size_t num_fid,
        uint32_t *in_data)
{
    size_t count = 0;
    uint32_t in_list_cnt = 0;
    const bcmlrd_field_data_t *field;

    SHR_FUNC_ENTER(unit);
    /*populate the values from in list */
    for (count = 0; count < num_fid; count++) {
        /* populate default value if any*/
        SHR_IF_ERR_EXIT(bcmlrd_field_get(unit, sid, count, &field));
        if (field->def->u64) {
            in_data[count] = field->def->u64;
        }

        if (in_list_cnt < in->count) {
            if (in->field[in_list_cnt]->id == count) {
                in_data[count] = in->field[in_list_cnt]->data;
                in_list_cnt++;
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_chipq_from_mmuq_get(int unit,
                          uint32_t lport,
                          uint32_t mmu_q,
                          uint32_t *chip_q)
{
    bcmtm_drv_t  *tm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_drv_get(unit, &tm_drv));

    SHR_NULL_CHECK(tm_drv, SHR_E_PARAM);

    SHR_NULL_CHECK(tm_drv->chip_q_get, SHR_E_PARAM);
    SHR_IF_ERR_EXIT
        (tm_drv->chip_q_get(unit, lport, mmu_q, chip_q));
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_noncpu_chipq_from_mmuq_get(int unit,
                          uint32_t lport,
                          uint32_t mmu_q,
                          uint32_t *global_q)
{
    bcmtm_drv_t  *tm_drv = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmtm_drv_get(unit, &tm_drv));

    SHR_NULL_CHECK(tm_drv, SHR_E_PARAM);

    SHR_NULL_CHECK(tm_drv->noncpu_chip_q_get, SHR_E_PARAM);
    SHR_IF_ERR_EXIT
        (tm_drv->noncpu_chip_q_get(unit, lport, mmu_q, global_q));
exit:
    SHR_FUNC_EXIT();
}


static void
bcmtm_drv_info_dump(int unit, shr_pb_t *pb)
{
    bcmtm_drv_info_t *drv_info;
    bcmdrd_pbmp_t pbmp;
    bcmtm_port_map_info_t *port_map;
    int lport, pport, mmu_port, local_mmu_port, mmu_chip_port, pipe, speed_max,
        speed_cur, num_uc_q, num_mc_q, uc_base, mc_base;
    char *sys_port;

    bcmtm_drv_info_get(unit, &drv_info);
    bcmtm_port_map_get(unit, &port_map);

    if (SHR_FAILURE(bcmdrd_dev_valid_ports_get(unit, &pbmp))) {
        return;
    }

    shr_pb_printf(pb, "\n");
    shr_pb_printf(pb, "frequency            : %0d\n", drv_info->frequency);
    shr_pb_printf(pb, "fabric_port_enable   : %0d\n", drv_info->fabric_port_enable);
    shr_pb_printf(pb, "dpr_clock_frequency  : %0d\n", drv_info->dpr_clock_frequency);
    shr_pb_printf(pb, "oobfc_clock_ns       : %0d\n", drv_info->oobfc_clock_ns);
    shr_pb_printf(pb, "flexport             : %0d\n", drv_info->flexport);
    shr_pb_printf(pb, "lossless             : %0d\n", drv_info->lossless);
    shr_pb_printf(pb, "----------------------------------------------------------------------------------\n");
    shr_pb_printf(pb, "Logical Physical MMU  Local MMU         Speed   Speed  Num  Num  UCQ   MCQ  System\n");
    shr_pb_printf(pb, " Port     Port   Port  MMU  Chip Pipe    Max     Cur   UCQ  MCQ  base  base  Port \n");
    shr_pb_printf(pb, "----------------------------------------------------------------------------------\n");
    /* Output format: " xxx      xxx    xxx   xxx  xxx  xxx   xxxxxx  xxxxxx  xxx  xxx  xxxx  xxxx  xxxx \n" */

    BCMDRD_PBMP_ITER(pbmp, pport) {
        lport = port_map->pport_map[pport].lport;
        if (lport == -1) {
            continue;
        }
        mmu_port       = port_map->lport_map[lport].mport;
        local_mmu_port = port_map->lport_map[lport].mlocal_port;
        mmu_chip_port  = port_map->lport_map[lport].mchip_port;
        pipe           = port_map->lport_map[lport].pipe;
        speed_max      = port_map->lport_map[lport].max_speed;
        speed_cur      = port_map->lport_map[lport].cur_speed;
        num_uc_q       = port_map->lport_map[lport].num_ucq;
        num_mc_q       = port_map->lport_map[lport].num_mcq;
        uc_base        = port_map->lport_map[lport].base_ucq;
        mc_base        = port_map->lport_map[lport].base_mcq;
        if (bcmtm_lport_is_cpu(unit, lport)) {
            sys_port = "CPU";
        } else if (bcmtm_lport_is_lb(unit, lport)) {
            sys_port = "LPBK";
        } else if (bcmtm_lport_is_mgmt(unit, lport)) {
            sys_port = "MGMT";
        } else if (bcmtm_lport_is_rdb(unit, lport)) {
            sys_port = "RDB";
        } else {
            sys_port = "";
        }
        shr_pb_printf(pb, " %3d      %3d    %3d   %3d  %3d  %3d   %6d  %6d  %3d  %3d  %4d  %4d  %4s\n",
                      lport, pport, mmu_port, local_mmu_port, mmu_chip_port, pipe,
                      speed_max, speed_cur, num_uc_q, num_mc_q, uc_base, mc_base,
                      sys_port);
    }
    shr_pb_printf(pb, "----------------------------------------------------------------------------------\n");
    shr_pb_printf(pb, "\n");
}

void
bcmtm_drv_info_print(int unit)
{
    shr_pb_t *pb;

    if (!BSL_LOG_CHECK_VERBOSE(BSL_LS_BCMTM_INIT)) {
        return;
    }

    pb = shr_pb_create();
    bcmtm_drv_info_dump(unit, pb);

    LOG_VERBOSE(BSL_LS_BCMTM_INIT,
                (BSL_META_U(unit, "%s\n"), shr_pb_str(pb)));
    shr_pb_destroy(pb);
}

int
bcmtm_is_flexport(int unit)
{
    bcmtm_drv_info_t *drv_info;
    bcmtm_drv_info_get(unit, &drv_info);
    return drv_info->flexport;
}

int
bcmtm_interrupt_enable(int unit,
                       int intr_num,
                       bcmbd_intr_f intr_func,
                       bcmtm_intr_enable_t enable)
{
    SHR_FUNC_ENTER(unit);

    switch (enable) {
        case INTR_ENABLE:
            /* Enable the interrupt and register the
             * interrupt callback function.
             */
            SHR_IF_ERR_EXIT
                (bcmbd_mmu_intr_func_set(unit, intr_num, intr_func, intr_num));
            SHR_IF_ERR_EXIT
                (bcmbd_mmu_intr_enable(unit, intr_num));
            break;

        case INTR_DISABLE:
            /* Disable the interrupt */
            SHR_IF_ERR_EXIT
                (bcmbd_mmu_intr_clear(unit, intr_num));
            SHR_IF_ERR_EXIT
                (bcmbd_mmu_intr_disable(unit, intr_num));
            break;

        case INTR_CLEAR:
            SHR_IF_ERR_EXIT
                (bcmbd_mmu_intr_clear(unit, intr_num));
            break;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_mmui_interrupt_enable(int unit,
                            int intr_num,
                            bcmbd_intr_f intr_func,
                            bcmtm_intr_enable_t enable)
{
    SHR_FUNC_ENTER(unit);

    switch (enable) {
        case INTR_ENABLE:
            /* Enable the interrupt and register the
             * interrupt callback function.
             */
            SHR_IF_ERR_EXIT
                (bcmbd_mmui_intr_func_set(unit, intr_num,
                                                    intr_func, intr_num));
            SHR_IF_ERR_EXIT
                (bcmbd_mmui_intr_enable(unit, intr_num));
            break;

        case INTR_DISABLE:
            /* Disable the interrupt */
            SHR_IF_ERR_EXIT
                (bcmbd_mmui_intr_clear(unit, intr_num));
            SHR_IF_ERR_EXIT
                (bcmbd_mmui_intr_disable(unit, intr_num));
            break;

        case INTR_CLEAR:
            SHR_IF_ERR_EXIT
                (bcmbd_mmui_intr_clear(unit, intr_num));
            break;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_mc_free_resource_count_get (int unit, uint64_t *free_resource) {

    bcmtm_mc_repl_list_free_count fn;
    bcmtm_drv_t *drv;
    bcmtm_mc_drv_t mc_drv;
    SHR_FUNC_ENTER(unit);

    sal_memset(&mc_drv, 0, sizeof(bcmtm_mc_drv_t));
    SHR_IF_ERR_EXIT(bcmtm_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv->mc_drv_get, SHR_E_UNAVAIL);
    SHR_IF_ERR_EXIT(drv->mc_drv_get(unit, &mc_drv));

    fn = mc_drv.mc_repl_list_free_count;
    if (fn) {
        SHR_IF_ERR_EXIT(fn(unit, free_resource));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_port_is_higig3 (int unit, bcmtm_lport_t lport, int *hg3)
{
    bcmtm_port_map_info_t *port_map;

    SHR_FUNC_ENTER(unit);
    bcmtm_port_map_get(unit, &port_map);
    *hg3 = 0;
    if (port_map->lport_map[lport].flags & BCMTM_PORT_IS_HG3) {
        *hg3 = 1;
    }
    SHR_FUNC_EXIT();
}

int
bcmtm_obm_port_validation(int unit, bcmtm_lport_t lport)
{
    bcmtm_drv_t *drv;
    bcmtm_obm_drv_t obm_drv;
    int rv;

    sal_memset(&obm_drv, 0, sizeof(bcmtm_obm_drv_t));
    rv = bcmtm_drv_get(unit, &drv);
    if (SHR_FAILURE(rv)) {
        return FALSE;
    }

    if (!drv->obm_drv_get) {
        return FALSE;
    }

    rv = drv->obm_drv_get(unit, &obm_drv);
    if (SHR_FAILURE(rv)) {
        return FALSE;
    }
    if (!obm_drv.port_validation) {
        return FALSE;
    }

    return obm_drv.port_validation(unit, lport);
}

int
bcmtm_num_ucq_get(int unit, uint8_t *num_ucq)
{
    bcmtm_drv_t *drv;

    SHR_FUNC_ENTER(unit);

    *num_ucq = 0;
    SHR_IF_ERR_EXIT
        (bcmtm_drv_get(unit, &drv));
    if (drv->num_uc_q_cfg_get) {
        *num_ucq = (uint8_t)(drv->num_uc_q_cfg_get(unit));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_num_mcq_get(int unit, uint8_t *num_mcq)
{
    bcmtm_drv_t *drv;

    SHR_FUNC_ENTER(unit);

    *num_mcq = 0;
    SHR_IF_ERR_EXIT
        (bcmtm_drv_get(unit, &drv));
    if (drv->num_mc_q_cfg_get) {
        *num_mcq = (uint8_t)(drv->num_mc_q_cfg_get(unit));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_check_ep_mmu_credit(int unit, int lport)
{
    bcmtm_drv_t *tm_drv;

    SHR_FUNC_ENTER(unit);

    (void)(bcmtm_drv_get(unit, &tm_drv));

    if (!tm_drv->check_ep_mmu_credit) {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT
        (tm_drv->check_ep_mmu_credit(unit, lport));

exit:
    SHR_FUNC_EXIT();
}

int
bcmtm_check_port_ep_credit(int unit, int lport)
{
    bcmtm_drv_t *tm_drv;

    SHR_FUNC_ENTER(unit);

    (void)(bcmtm_drv_get(unit, &tm_drv));

    if (!tm_drv->check_port_ep_credit) {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT
        (tm_drv->check_port_ep_credit(unit, lport));

exit:
    SHR_FUNC_EXIT();
}
