/*! \file bcmsec_util.c
 *
 * SEC Driver utility functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_pb.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/bcmdrd_internal.h>
#include <bcmbd/bcmbd_sec_intr.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmsec/bcmsec_drv.h>
#include <bcmsec/bcmsec_utils_internal.h>
#include <bcmsec/bcmsec_utils.h>
#include <bcmsec/generated/bcmsec_ha.h>
#include <bcmsec/bcmsec_ha_internal.h>
#include <bcmsec/imm/bcmsec_imm_defines_internal.h>
#include <bcmissu/issu_api.h>

#define BSL_LOG_MODULE  BSL_LS_BCMSEC_UTIL

/*******************************************************************************
* Local definitions
 */
static bcmsec_event_info_t *bcmsec_event[BCMDRD_CONFIG_MAX_UNITS];
static bcmsec_dev_info_t *bcmsec_dev_info[BCMDRD_CONFIG_MAX_UNITS];
static bcmsec_pport_info_t *bcmsec_pport_info[BCMDRD_CONFIG_MAX_UNITS];
static bcmsec_lport_info_t *bcmsec_lport_info[BCMDRD_CONFIG_MAX_UNITS];

/*! TM port operation handler function. */
typedef int
(*sec_port_op_f)(int unit, bcmsec_pport_t pport, uint32_t param);

/*! This structure is used to define the operation for SEC. */
typedef struct sec_port_op_hdl_s {

    /*! Operation name. */
    char *name;

    /*! Operation handler. */
    sec_port_op_f func;

} sec_port_op_hdl_t;

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
bcmsec_lt_field_data_to_u64(uint32_t width,
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
 * \brief Check if logical port is a spare port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 *
 * \retval TRUE Logical port is spare.
 * \retval FALSE Logical port is not spare.
 */
static int
bcmsec_lport_is_spare(int unit, bcmsec_lport_t lport)
{
    if (bcmdrd_lport_type_get(unit, lport) == BCMDRD_PORT_TYPE_RSVD) {
        return true;
    }
    return false;
}

/*!
 * \brief Execute ingress down sequence for a port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Physical port number.
 * \param [in] up Execute 'up' sequence if non-zero, otherwise execute 'down'
 *                sequence. Ignored.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
sec_rx_down(int unit, bcmsec_pport_t port, uint32_t up)
{
    bcmsec_drv_t *sec_drv;

    SHR_FUNC_ENTER(unit);

    (void)(bcmsec_drv_get(unit, &sec_drv));

    SHR_NULL_CHECK
        (sec_drv->port_ing_down, SHR_E_NONE);
    SHR_ERR_EXIT
        (sec_drv->port_ing_down(unit, port));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Execute egress up/down sequence for a port.
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
sec_tx_up(int unit, bcmsec_pport_t port, uint32_t up)
{
    bcmsec_drv_t *sec_drv;

    SHR_FUNC_ENTER(unit);

    (void)(bcmsec_drv_get(unit, &sec_drv));

    if (up) {
        SHR_NULL_CHECK
            (sec_drv->port_egr_up, SHR_E_NONE);
        SHR_ERR_EXIT
            (sec_drv->port_egr_up(unit, port));
    } else {
        SHR_NULL_CHECK
            (sec_drv->port_egr_down, SHR_E_NONE);
        SHR_ERR_EXIT
            (sec_drv->port_egr_down(unit, port));
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
* Public functions
 */
int
bcmsec_fval_get_from_field_array (int unit,
        bcmlrd_fid_t fid,
        uint32_t idx,
        const bcmltd_field_t *field_array,
        uint64_t *fval)
{
    bcmltd_field_t *field = (bcmltd_field_t *)field_array;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(field, SHR_E_NOT_FOUND);

    while(field) {
        if ((field->id == fid) && (field->idx == idx)){
            *fval = field->data;
            break;
        }
        field =  field->next;
    }
    if (!field) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmsec_field_list_set(int unit,
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
            SHR_EXIT();
        }
    }
    if (!flist->field[count]) {
        SHR_ERR_EXIT(SHR_E_FULL);
    }
    flist->field[count]->id = fid;
    flist->field[count]->data = val;
    flist->field[count]->idx = idx;
    flist->count++;
exit:
    SHR_FUNC_EXIT();
}

int
bcmsec_field_list_get(int unit,
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
bcmsec_port_is_cpu(int unit, bcmsec_lport_t lport)
{
    if (bcmdrd_lport_type_get(unit, lport) == BCMDRD_PORT_TYPE_CPU) {
        return true;
    }
    return false;
}

int
bcmsec_port_is_lb(int unit, bcmsec_lport_t lport)
{
    if (bcmdrd_lport_type_get(unit, lport) == BCMDRD_PORT_TYPE_LB) {
        return true;
    }
    return false;
}

int
bcmsec_port_is_rdb(int unit, bcmsec_lport_t lport)
{
    if (bcmdrd_lport_type_get(unit, lport) == BCMDRD_PORT_TYPE_RDB) {
        return true;
    }
    return FALSE;
}

int
bcmsec_port_is_mgmt(int unit, bcmsec_lport_t lport)
{
    if (bcmdrd_lport_type_get(unit, lport) == BCMDRD_PORT_TYPE_MGMT) {
        return true;
    }
    return false;
}

int
bcmsec_port_is_fp(int unit, bcmsec_lport_t lport)
{
    if ((bcmsec_port_is_mgmt(unit,  lport)) ||
            (bcmsec_port_is_lb(unit, lport)) ||
            (bcmsec_port_is_cpu(unit, lport)) ||
            (bcmsec_port_is_rdb(unit, lport)) ||
            (bcmsec_lport_is_spare(unit, lport))) {
        return FALSE;
    }
    return TRUE;
}

void
bcmsec_dev_info_portmap_reset(int unit)
{
    bcmsec_dev_info_t *dev_info;
    bcmsec_pport_info_t *pport_info, *pinfo;
    bcmsec_lport_info_t *lport_info, *linfo;
    int port;

    bcmsec_dev_info_get(unit, &dev_info);
    bcmsec_pport_info_get(unit, &pport_info);
    bcmsec_lport_info_get(unit, &lport_info);

    for (port = 0; port < dev_info->num_pport; port++) {
        pinfo = &pport_info[port];
        pinfo->port_p2l_mapping = -1;
    }
    for (port = 0; port < dev_info->num_lport; port++) {
        linfo = &lport_info[port];
        linfo->port_speed_cur = -1;
        linfo->port_num_lanes = -1;
        linfo->port_l2p_mapping = -1;
    }
}

int
bcmsec_port_op_exec(int unit, bcmsec_pport_t pport,
                    char *op_name, uint32_t op_param)
{
    static sec_port_op_hdl_t port_ops[] = {
        { "rx_down", sec_rx_down},
        { "tx_up", sec_tx_up},
    };
    sec_port_op_hdl_t *op_hdl;
    int i;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < COUNTOF(port_ops); i++) {
        op_hdl = &port_ops[i];

        if (sal_strcmp(op_name, op_hdl->name) != 0) {
            continue;
        }

        SHR_ERR_EXIT
            (op_hdl->func(unit, pport, op_param));
    }

    LOG_ERROR(BSL_LOG_MODULE,
              (BSL_META_U(unit, "Operation not supported[%s]\n"), op_name));

    SHR_ERR_EXIT(SHR_E_UNAVAIL);

exit:
    SHR_FUNC_EXIT();
}

int
bcmsec_field_list_alloc(int unit,
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
    SHR_ALLOC(flist->field, num_fields * sizeof(bcmltd_field_t *), "bcmsecLtFieldList");
    SHR_NULL_CHECK( flist->field, SHR_E_MEMORY);

    SHR_ALLOC(mem, num_fields * sizeof(bcmltd_field_t), "bcmsecLtFieldList");
    SHR_NULL_CHECK( mem, SHR_E_MEMORY);
    sal_memset(mem, 0, (num_fields * sizeof(bcmltd_field_t)));

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
bcmsec_field_list_free(bcmltd_fields_t *flist)
{
    if (flist) {
        if (flist->field) {
            SHR_FREE(flist->field[0]);
        }
        SHR_FREE(flist->field);
        flist = NULL;
    }
}

/*
 * Update the existing field list(parameter ext_fld) with the new list
 * of the logical fields(parameter new_fld).
 */
int
sec_fields_update_and_link (int unit,
                            bcmltd_field_t *new_fld,
                            bcmltd_fields_t * ext_fld)
{
    uint32_t i;

    /* update the existing fields with the new ones */
    while (new_fld) {
        for (i = 0; i < ext_fld->count; i++) {
            if ((new_fld->id == ext_fld->field[i]->id) &&
                (new_fld->idx == ext_fld->field[i]->idx)) {
                ext_fld->field[i]->data = new_fld->data;
                break;
            }
        }
        /* not found, new field to add */
        if (i == ext_fld->count) {
            ext_fld->field[ext_fld->count]->id  = new_fld->id;
            ext_fld->field[ext_fld->count]->idx = new_fld->idx;
            ext_fld->field[ext_fld->count]->data = new_fld->data;
            ext_fld->field[ext_fld->count]->next = NULL;
            ext_fld->count++;
        }
        new_fld = new_fld->next;
    }

    /* Link all bcmltd_fields. */
    for (i = 0; i < ext_fld->count; i++) {
        if (i == (ext_fld->count - 1)) {
            ext_fld->field[i]->next = NULL;
        } else {
            ext_fld->field[i]->next = ext_fld->field[i + 1];
        }
    }
    return SHR_E_NONE;
}

int
bcmsec_field_value_range_get(int unit,
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
    bcmsec_lt_field_data_to_u64(field_md->width, field_data, min, max, &def);

exit:
    SHR_FUNC_EXIT();
}

int
bcmsec_port_phys_port_get(int unit,
        bcmsec_lport_t lport,
        int *pport)
{
    bcmsec_lport_info_t *lport_info, *linfo;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmsec_lport_info_get(unit, &lport_info));

    linfo = &lport_info[lport];
    *pport = linfo->port_l2p_mapping;
    if (*pport == -1) {
        LOG_VERBOSE(BSL_LOG_MODULE,(BSL_META_U(unit,
                "Unable to get pport for logical port %d. "
                "Check if SEC_PC_PM is configured in YAML file prior to PC_PORT.\n"),
                lport));
        SHR_VERBOSE_EXIT(SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmsec_imm_port_update (int unit, int lport,
                        bcmsec_port_internal_update_t port_op)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmsec_port_control_internal_update(unit, lport, port_op));
exit:
    SHR_FUNC_EXIT();
}

int
bcmsec_interrupt_enable(int unit,
                        int intr_num,
                        bcmbd_intr_f intr_func,
                        bcmsec_intr_enable_t enable)
{
    SHR_FUNC_ENTER(unit);

    switch (enable) {
        case INTR_ENABLE:
            /*
             * Enable the interrupt and register the
             * interrupt callback function.
             */
            SHR_IF_ERR_EXIT
                (bcmbd_sec_intr_func_set(unit, intr_num, intr_func, intr_num));
            SHR_IF_ERR_EXIT
                (bcmbd_sec_intr_enable(unit, intr_num));
            break;

        case INTR_DISABLE:
            /* Disable the interrupt */
            SHR_IF_ERR_EXIT
                (bcmbd_sec_intr_clear(unit, intr_num));
            SHR_IF_ERR_EXIT
                (bcmbd_sec_intr_disable(unit, intr_num));
            break;

        case INTR_CLEAR:
            SHR_IF_ERR_EXIT
                (bcmbd_sec_intr_clear(unit, intr_num));
            break;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmsec_post_event (int unit, bcmsec_event_trigger_t *event)
{
    bcmsec_event_info_t *info;
    SHR_FUNC_ENTER(unit);

    bcmsec_event_info_get(unit, &info);
    event->type = SEC_TRIG_NOTIF;
    (void) sal_msgq_post (info->bcmsec_event_q, event,
                          SAL_MSGQ_HIGH_PRIORITY, SAL_MSGQ_FOREVER);
    SHR_FUNC_EXIT();
}

void
bcmsec_event_info_set(int unit,
                      bcmsec_event_info_t *info)
{
    bcmsec_event[unit] = info;
}

void
bcmsec_event_info_get(int unit,
                      bcmsec_event_info_t **info)
{
    *info = bcmsec_event[unit];
}

int
bcmsec_num_sa_per_sc_get (int unit, int blk_id,
                          bool encrypt, int *num_sa)
{
    bcmsec_dev_info_t *dev_info;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmsec_dev_info_get(unit, &dev_info));
    if (encrypt) {
        *num_sa = dev_info->num_sa_per_sc_encrypt[blk_id];
    } else {
        *num_sa = dev_info->num_sa_per_sc_decrypt[blk_id];
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmsec_num_sa_per_sc_set (int unit, int blk_id,
                          bool encrypt, int num_sa)
{
    bcmsec_dev_info_t *dev_info;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmsec_dev_info_get(unit, &dev_info));
    if (encrypt) {
        dev_info->num_sa_per_sc_encrypt[blk_id] = num_sa;
    } else {
        dev_info->num_sa_per_sc_decrypt[blk_id] = num_sa;
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmsec_dev_info_get(int unit, bcmsec_dev_info_t **dev_info)
{
    SHR_FUNC_ENTER(unit);
    *dev_info = bcmsec_dev_info[unit];
    if (*dev_info == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmsec_dev_info_alloc(int unit, bool warm)
{
    bcmsec_dev_info_t *dev_info = NULL;
    uint32_t bcmsec_dev_info_sz = sizeof(bcmsec_dev_info_t);
    int i;

    SHR_FUNC_ENTER(unit);
    dev_info = shr_ha_mem_alloc(unit,
                                BCMMGMT_SEC_COMP_ID,
                                BCMSEC_HA_DEV_INFO,
                                "bcmsecDrvInfo",
                                &bcmsec_dev_info_sz);
    SHR_NULL_CHECK(dev_info, SHR_E_MEMORY);
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "BCMSEC: HA memory for Device drive info "
        "is created with size %0d\n"), bcmsec_dev_info_sz));

    if (!warm) {
        /* memset the allocated memory*/
        sal_memset(dev_info, 0, bcmsec_dev_info_sz);

        bcmissu_struct_info_report(unit, BCMMGMT_SEC_COMP_ID,
                                   BCMSEC_HA_DEV_INFO, 0,
                                   sizeof(bcmsec_dev_info_t), 1,
                                   BCMSEC_DEV_INFO_T_ID);
    }
    for (i = 0; i < MAX_NUM_SEC_BLOCKS; i++) {
        dev_info->num_sa_per_sc_encrypt[i] = SEC_DEFAULT_NUM_SA_ENCRYPT;
        dev_info->num_sa_per_sc_decrypt[i] = SEC_DEFAULT_NUM_SA_DECRYPT;
    }
    bcmsec_dev_info[unit] = dev_info;
exit:
    SHR_FUNC_EXIT();
    return 0;
}

int
bcmsec_pport_info_get(int unit, bcmsec_pport_info_t **port_info)
{
    SHR_FUNC_ENTER(unit);
    *port_info = bcmsec_pport_info[unit];
    if (*port_info == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmsec_lport_info_get(int unit, bcmsec_lport_info_t **port_info)
{
    SHR_FUNC_ENTER(unit);
    *port_info = bcmsec_lport_info[unit];
    if (*port_info == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmsec_port_info_alloc(int unit, bool warm,
                       sec_port_info_alloc_cfg_t *port_cfg)
{
    bcmsec_dev_info_t *dev_info;
    bcmsec_pport_info_t *pport;
    bcmsec_lport_info_t *lport;
    uint32_t size, expected_size;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmsec_dev_info_get(unit, &dev_info));

    size = port_cfg->num_pport * sizeof(bcmsec_pport_info_t);
    expected_size = size;
    pport = shr_ha_mem_alloc(unit,
                            BCMMGMT_SEC_COMP_ID,
                            BCMSEC_HA_PHYSICAL_PORT_INFO,
                            "bcmsecPhyportInfo",
                            &size);
    SHR_NULL_CHECK(pport, SHR_E_MEMORY);
    SHR_IF_ERR_VERBOSE_EXIT
        ((size < expected_size) ? SHR_E_MEMORY : SHR_E_NONE);
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "BCMSEC: HA memory for physical port info "
        "is created with size %0d\n"), size));

    if (!warm) {
        /* memset the allocated memory*/
        sal_memset(pport, 0, size);
        bcmissu_struct_info_report(unit, BCMMGMT_SEC_COMP_ID,
                                   BCMSEC_HA_PHYSICAL_PORT_INFO, 0,
                                   sizeof(bcmsec_pport_info_t),
                                   port_cfg->num_pport,
                                   BCMSEC_PPORT_INFO_T_ID);
    }
    bcmsec_pport_info[unit] = pport;
    dev_info->num_pport = port_cfg->num_pport;

    size = port_cfg->num_lport * sizeof(bcmsec_lport_info_t);
    expected_size = size;
    lport = shr_ha_mem_alloc(unit,
                            BCMMGMT_SEC_COMP_ID,
                            BCMSEC_HA_LOGICAL_PORT_INFO,
                            "bcmsecLogicalportInfo",
                            &size);
    SHR_NULL_CHECK(lport, SHR_E_MEMORY);
    SHR_IF_ERR_VERBOSE_EXIT
        ((size < expected_size) ? SHR_E_MEMORY : SHR_E_NONE);
    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit, "BCMSEC: HA memory for logical port info "
        "is created with size %0d\n"), size));

    if (!warm) {
        /* memset the allocated memory*/
        sal_memset(lport, 0, size);
        bcmissu_struct_info_report(unit, BCMMGMT_SEC_COMP_ID,
                                   BCMSEC_HA_LOGICAL_PORT_INFO, 0,
                                   sizeof(bcmsec_lport_info_t),
                                   port_cfg->num_lport,
                                   BCMSEC_LPORT_INFO_T_ID);

    }
    bcmsec_lport_info[unit] = lport;
    dev_info->num_lport = port_cfg->num_lport;

exit:
    SHR_FUNC_EXIT();
}
