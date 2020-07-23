/*! \file bcm56780_a0_cth_port.c
 *
 * bcm56780 a0 PORT drivers.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <bsl/bsl.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_ha.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmcfg/bcmcfg_lt.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmcth/bcmcth_port_util.h>
#include <bcmcth/bcmcth_port_drv.h>
#include <bcmdrd/chip/bcm56780_a0_enum.h>
#include <bcmdrd/chip/bcm56780_a0_defs.h>

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_PORT

/* Maximum mirror constainer number. */
#define BCMPORT_MIRROR_CONTAINER_MAX (8)

/* Maximum port ID. */
#define BCMPORT_PORT_ID_MAX (160)

#define SYSTEM_PORT_FORMAT_0 0
#define SYSTEM_PORT_FORMAT_1 1
#define SYSTEM_PORT_FORMAT_2 2
#define SYSTEM_PORT_FORMAT_MASK_0 0x0FFF
#define SYSTEM_PORT_FORMAT_MASK_1 0x1F7F
#define SYSTEM_PORT_FORMAT_MASK_2 0x3F3F

#define SYSTEM_PORT_FORMAT_MASK_0_LO 0x00FF
#define SYSTEM_PORT_FORMAT_MASK_0_UP 0x0F00

#define SYSTEM_PORT_FORMAT_MASK_1_LO 0x007F
#define SYSTEM_PORT_FORMAT_MASK_1_UP 0x1F00

#define SYSTEM_PORT_FORMAT_MASK_2_LO  0x003F
#define SYSTEM_PORT_FORMAT_MASK_2_UP  0x3F00
/*******************************************************************************
 * Typedefs
 */
/*! Ingress/egress mirror enable state of ports. */
typedef struct bcmport_mirror_state_s {
    /*! Instance bitmap of egress mirror for UC enabled on a port. */
    uint8_t uc_em[BCMPORT_PORT_ID_MAX];

    /*! Mirror information for UC container. */
    bcmport_mirr_state_t uc_em_state[BCMPORT_PORT_ID_MAX][BCMPORT_MIRROR_CONTAINER_MAX];

} bcmport_mirror_state_t;

/*******************************************************************************
 * Private variables
 */
/*
 * Ingress/egress mirror HA structure.
 * 1. TD4 doesn't have per-port ingress container.
 *    -> im is not used.
 * 2. TD4 doesn't have separated uc and non-uc containers, but one container.
 *    -> take uc_em for recording the container status.
 */
static bcmport_mirror_state_t *port_mstate[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private functions
 */
static int
bcm56780_a0_cth_port_mirror_update(
    int unit,
    const bcmltd_op_arg_t *op_arg,
    bcmltd_sid_t lt_id,
    bool cpu,
    shr_port_t port)
{
    uint32_t *entry_buf = NULL;
    uint32_t entry_sz = 0;
    bcmdrd_sid_t pt_id = IPOST_MIRROR_SAMPLER_CPU_BMPm;
    bcmdrd_fid_t pt_fid = BMPf;
    bcmdrd_pbmp_t pbm_cpu;

    SHR_FUNC_ENTER(unit);

    entry_sz = BCMDRD_WORDS2BYTES(bcmdrd_pt_entry_wsize(unit, pt_id));
    SHR_ALLOC(entry_buf, entry_sz, "bcmcthPortmirrorEntry");
    SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);
    sal_memset(entry_buf, 0, entry_sz);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_port_pt_read(unit, op_arg, lt_id, pt_id, 0, entry_buf));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_pt_field_get(unit,
                             pt_id,
                             entry_buf,
                             pt_fid,
                             (uint32_t *)&pbm_cpu));

    if (cpu) {
        BCMDRD_PBMP_PORT_ADD(pbm_cpu, port);
    } else {
        BCMDRD_PBMP_PORT_REMOVE(pbm_cpu, port);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_pt_field_set(unit,
                             pt_id,
                             entry_buf,
                             pt_fid,
                             (uint32_t *)&pbm_cpu));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_port_pt_write(unit, op_arg, lt_id, pt_id, 0, entry_buf));

exit:
    if (entry_buf) {
        SHR_FREE(entry_buf);
    }
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_port_mirror_set(
    int unit,
    const bcmltd_op_arg_t *op_arg,
    bcmltd_sid_t lt_id,
    bcmcth_port_mirror_op_t op,
    port_mirror_t *entry)
{
    bool cpu;
    shr_port_t port;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    cpu = entry->cpu;
    port = entry->port;

    switch (op) {
        case BCMCTH_PORT_MIRROR_OP_SET:
            SHR_IF_ERR_EXIT
                (bcm56780_a0_cth_port_mirror_update
                    (unit, op_arg, lt_id, cpu, port));
            break;
        case BCMCTH_PORT_MIRROR_OP_DEL:
            SHR_IF_ERR_EXIT
                (bcm56780_a0_cth_port_mirror_update
                    (unit, op_arg, lt_id, 0, port));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();

}

static int
bcm56780_a0_port_mirror_control_set(
    int unit,
    bcmltd_sid_t lt_id,
    const bcmltd_op_arg_t *op_arg,
    shr_port_t port,
    uint32_t container,
    uint32_t session,
    uint32_t mirror_inst,
    bool mirror_enable,
    bool non_uc_em)
{
    uint32_t *entry_buf = NULL;
    uint32_t entry_sz = 0;
    bcmdrd_fid_t pt_fid_inst[] = {
        VALUE_0f,
        VALUE_1f,
        VALUE_2f,
        VALUE_3f,
        VALUE_4f,
        VALUE_5f,
        VALUE_6f,
        VALUE_7f
    };
    bcmdrd_fid_t pt_fid_session[] = {
        CONTAINER_0f,
        CONTAINER_1f,
        CONTAINER_2f,
        CONTAINER_3f,
        CONTAINER_4f,
        CONTAINER_5f,
        CONTAINER_6f,
        CONTAINER_7f
    };
    bcmdrd_fid_t pt_fid_en[] = {
        ENABLE_0f,
        ENABLE_1f,
        ENABLE_2f,
        ENABLE_3f,
        ENABLE_4f,
        ENABLE_5f,
        ENABLE_6f,
        ENABLE_7f
    };
    bcmdrd_sid_t pt_id;
    bcmdrd_fid_t pt_fid;
    uint32_t fval;

    SHR_FUNC_ENTER(unit);

    if (container >= BCMPORT_MIRROR_CONTAINER_MAX) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    pt_id = IPOST_MIRROR_SAMPLER_EMIRROR_CONTROL_0m;

    entry_sz = BCMDRD_WORDS2BYTES(bcmdrd_pt_entry_wsize(unit, pt_id));
    SHR_ALLOC(entry_buf, entry_sz, "bcmcthPortTableEntry");
    SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);
    sal_memset(entry_buf, 0, entry_sz);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_port_pt_read(unit, op_arg, lt_id, pt_id, port, entry_buf));

    /* MIRROR_SESSION_ID */
    pt_fid = pt_fid_session[container];
    fval = session;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_pt_field_set(unit,
                             pt_id,
                             entry_buf,
                             pt_fid,
                             &fval));

    /* MIRROR_INSTANCE_ID */
    pt_fid = pt_fid_inst[container];
    fval = mirror_inst;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_pt_field_set(unit,
                             pt_id,
                             entry_buf,
                             pt_fid,
                             &fval));

    /* MIRROR_ENABLE */
    pt_fid = pt_fid_en[container];
    fval = mirror_enable;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_pt_field_set(unit,
                             pt_id,
                             entry_buf,
                             pt_fid,
                             &fval));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_port_pt_write(unit, op_arg, lt_id, pt_id, port, entry_buf));

exit:
    if (entry_buf) {
        SHR_FREE(entry_buf);
    }
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_port_emirror_control_set(
    int unit,
    bcmltd_sid_t lt_id,
    const bcmltd_op_arg_t *op_arg,
    shr_port_t iport,
    shr_port_t eport,
    uint32_t container,
    bool mirror_enable)
{
    uint32_t *entry_buf = NULL;
    uint32_t entry_sz = 0;
    bcmdrd_sid_t pt_sid[] = {
        IPOST_MIRROR_SAMPLER_EMIRROR_PORTS_0m,
        IPOST_MIRROR_SAMPLER_EMIRROR_PORTS_1m,
        IPOST_MIRROR_SAMPLER_EMIRROR_PORTS_2m,
        IPOST_MIRROR_SAMPLER_EMIRROR_PORTS_3m,
        IPOST_MIRROR_SAMPLER_EMIRROR_PORTS_4m,
        IPOST_MIRROR_SAMPLER_EMIRROR_PORTS_5m,
        IPOST_MIRROR_SAMPLER_EMIRROR_PORTS_6m,
        IPOST_MIRROR_SAMPLER_EMIRROR_PORTS_7m
    };
    bcmdrd_sid_t pt_id;
    bcmdrd_fid_t pt_fid;
    bcmdrd_pbmp_t pbm_eport;

    SHR_FUNC_ENTER(unit);

    if (container >= BCMPORT_MIRROR_CONTAINER_MAX) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    pt_id  = pt_sid[container];
    pt_fid = BITMAPf;

    entry_sz = BCMDRD_WORDS2BYTES(bcmdrd_pt_entry_wsize(unit, pt_id));
    SHR_ALLOC(entry_buf, entry_sz, "bcmcthPortEmirrorControlEntry");
    SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);
    sal_memset(entry_buf, 0, entry_sz);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_port_pt_read(unit, op_arg, lt_id, pt_id, iport, entry_buf));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_pt_field_get(unit,
                             pt_id,
                             entry_buf,
                             pt_fid,
                             (uint32_t *)&pbm_eport));

    if (mirror_enable) {
        BCMDRD_PBMP_PORT_ADD(pbm_eport, eport);
    } else {
        BCMDRD_PBMP_PORT_REMOVE(pbm_eport, eport);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_pt_field_set(unit,
                             pt_id,
                             entry_buf,
                             pt_fid,
                             (uint32_t *)&pbm_eport));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_port_pt_write(unit, op_arg, lt_id, pt_id, iport, entry_buf));


exit:
    if (entry_buf) {
        SHR_FREE(entry_buf);
    }
    SHR_FUNC_EXIT();
}


static int
bcm56780_a0_system_port_table_index_get(int unit,
                                        const bcmltd_op_arg_t *op_arg,
                                        bcmltd_sid_t lt_id,
                                        uint16_t system_port,
                                        int16_t *index)
{
    uint8_t index_sel = 0;
    IPOST_LAG_SYSTEM_PORT_INDEX_SELECT_0r_t format_buf;
    bcmdrd_sid_t pt_id =  IPOST_LAG_SYSTEM_PORT_INDEX_SELECT_0r;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(index, SHR_E_PARAM);

    *index = -1;

    IPOST_LAG_SYSTEM_PORT_INDEX_SELECT_0r_CLR(format_buf);

    SHR_IF_ERR_EXIT
        (bcmcth_port_pt_read(unit,
                             op_arg,
                             lt_id,
                             pt_id,
                             0,
                             &format_buf));

    index_sel =
         IPOST_LAG_SYSTEM_PORT_INDEX_SELECT_0r_SELECTf_GET(format_buf);

    switch (index_sel) {
        case SYSTEM_PORT_FORMAT_0:
            *index = (system_port & SYSTEM_PORT_FORMAT_MASK_0);
            break;
        case SYSTEM_PORT_FORMAT_1:
            *index = (((system_port & SYSTEM_PORT_FORMAT_MASK_1_UP) >> 1) |
                       (system_port & SYSTEM_PORT_FORMAT_MASK_1_LO));
            break;
        case SYSTEM_PORT_FORMAT_2:
            *index = (((system_port & SYSTEM_PORT_FORMAT_MASK_2_UP) >> 2) |
                       (system_port & SYSTEM_PORT_FORMAT_MASK_2));
            break;
        default:
            LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                "Invalid IPOST_LAG_SYSTEM_PORT_INDEX_SELECT" \
                "system port select value %d.\n"),
                 index_sel));
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_port_system_dest_set(int unit, const bcmltd_op_arg_t *op_arg,
                                     bcmltd_sid_t lt_id,
                                     bool set,
                                     port_system_dest_entry_t *entry)
{
    uint32_t *sys_buf = NULL;
    uint32_t entry_sz = 0;
    int16_t  index = -1;
    uint32_t  field_value = 0;
    bcmdrd_sid_t pt_id =  IPOST_LAG_SYSTEM_PORTm;
    bool is_trunk = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    entry_sz = BCMDRD_WORDS2BYTES(bcmdrd_pt_entry_wsize(unit, pt_id));
    SHR_ALLOC(sys_buf, BCMDRD_WORDS2BYTES(entry_sz),
              "bcmcthPortSystemDestTableEntry");
    SHR_NULL_CHECK(sys_buf, SHR_E_MEMORY);

    sal_memset(sys_buf, 0, entry_sz);

    SHR_IF_ERR_EXIT(
        bcm56780_a0_system_port_table_index_get(unit,
                                  op_arg,
                                  lt_id,
                                  entry->port_system_id,
                                  &index));
    if (set) {
        SHR_IF_ERR_EXIT
            (bcmcth_port_pt_read(unit,
                                 op_arg,
                                 lt_id,
                                 pt_id,
                                 index,
                                 sys_buf));
        if (BCMPORT_LT_FIELD_GET(
            entry->fbmp, TRUNK_VALID)) {
            field_value = entry->trunk_valid;
            is_trunk = (bool) field_value;
            SHR_IF_ERR_EXIT
             (bcmdrd_pt_field_set(unit,
                                  pt_id,
                                  sys_buf,
                                  LOCAL_DEST_TYPEf,
                                  &field_value));
        } else {
            /* Read the is_trunk value from hardware */
            SHR_IF_ERR_VERBOSE_EXIT
             (bcmdrd_pt_field_get(unit,
                                  pt_id,
                                  sys_buf,
                                  LOCAL_DEST_TYPEf,
                                  &field_value));
            is_trunk = (bool) field_value;
        }

        if (is_trunk) {
            if (BCMPORT_LT_FIELD_GET(
                entry->fbmp, TRUNK_ID)) {
                field_value = entry->trunk_id;
                SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit,
                                     pt_id,
                                     sys_buf,
                                     LOCAL_DESTINATIONf,
                                     &field_value));
            }
        } else {
            if (BCMPORT_LT_FIELD_GET(
                entry->fbmp, LOCAL_PORT_ID)) {
                field_value = entry->port_id;
                SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit,
                                     pt_id,
                                     sys_buf,
                                     LOCAL_DESTINATIONf,
                                     &field_value));
            }
        }
        if (BCMPORT_LT_FIELD_GET(
            entry->fbmp, DLB_ID_VALID)) {
            field_value = entry->dlb_id_valid;
            SHR_IF_ERR_EXIT
             (bcmdrd_pt_field_set(unit,
                                 pt_id,
                                 sys_buf,
                                 DLB_ID_VALIDf,
                                 &field_value));
        }
        if (BCMPORT_LT_FIELD_GET(
            entry->fbmp, DLB_ID)) {
            field_value = entry->dlb_id;
            SHR_IF_ERR_EXIT
             (bcmdrd_pt_field_set(unit,
                                 pt_id,
                                 sys_buf,
                                 DLB_IDf,
                                 &field_value));
        }
        if (BCMPORT_LT_FIELD_GET(
            entry->fbmp, FLEX_CTR_ACTION)) {
            field_value = entry->flex_ctr_action;
            SHR_IF_ERR_EXIT
             (bcmdrd_pt_field_set(unit,
                                 pt_id,
                                 sys_buf,
                                 COUNTER_ACTIONf,
                                 &field_value));
        }
    }
    SHR_IF_ERR_EXIT
        (bcmcth_port_pt_write(unit, op_arg, lt_id, pt_id, index, sys_buf));

    exit:
    if (sys_buf != NULL) {
        SHR_FREE(sys_buf);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the PORT TRUNK CONTROL hw entry
 *
 * \param [in] unit Unit number.
 * \param [in] entry PORT_TRUNK_PORT_CONTROL LT entry
 *
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL if call fails.
 */
static int
bcm56780_a0_cth_port_trunk_pctl_set(int unit,
                                    const bcmltd_op_arg_t *op_arg,
                                    bcmltd_sid_t lt_id,
                                    port_trunk_pctl_entry_t *entry)
{
    int index = 0;
    IPOST_LAG_SYSTEM_LAG_FAILOVER_ENABLEm_t system_lag_failover;
    bcmdrd_pbmp_t failover_pbmp;
    bcmdrd_sid_t pt_id = IPOST_LAG_SYSTEM_LAG_FAILOVER_ENABLEm;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    /* Read the register from hardware. */
    BCMDRD_PBMP_CLEAR(failover_pbmp);

    index = 0;
    /* Read */
    SHR_IF_ERR_EXIT
        (bcmcth_port_pt_read(unit,
                             op_arg,
                             lt_id,
                             pt_id,
                             index,
                             (uint32_t *) &system_lag_failover));


    SHR_IF_ERR_EXIT(
        bcmdrd_pt_field_get(unit,
                            pt_id,
                            (uint32_t *) &system_lag_failover,
                            SYSTEM_LAG_FAILOVER_ENABLE_BITMAPf,
                            (uint32_t *) &failover_pbmp));

    if (BCMPORT_LT_FIELD_GET(
             entry->fbmp, TRUNK_SYSTEM_FAILOVER)) {

        if (entry->trunk_system_failover) {
            BCMDRD_PBMP_PORT_ADD(failover_pbmp,
                             entry->port_id);
        } else {
            BCMDRD_PBMP_PORT_REMOVE(failover_pbmp,
                             entry->port_id);
        }
    }

    SHR_IF_ERR_EXIT(
        bcmdrd_pt_field_set(unit,
                            pt_id,
                            (uint32_t *) &system_lag_failover,
                            SYSTEM_LAG_FAILOVER_ENABLE_BITMAPf,
                            (uint32_t *) (&failover_pbmp)));

    SHR_IF_ERR_EXIT
        (bcmcth_port_pt_write(unit,
                              op_arg,
                              lt_id,
                              pt_id,
                              index,
                              (uint32_t *) &system_lag_failover));

   exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_port_ingress_mirror_set(int unit,
                                        const bcmltd_op_arg_t *op_arg,
                                        bcmltd_sid_t lt_id,
                                        port_imirror_t *cfg,
                                        bool mirror_enable)
{
    return SHR_E_NONE;
}

static int
bcm56780_a0_cth_port_egress_mirror_set(
    int unit,
    const bcmltd_op_arg_t *op_arg,
    bcmltd_sid_t lt_id,
    port_emirror_t *cfg,
    bool mirror_enable)
{
    shr_port_t iport, eport;
    uint32_t container, sess, inst;
    bool update_imtp = FALSE, update_emtp = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cfg, SHR_E_PARAM);

    iport = cfg->port;
    eport = cfg->egr_port;
    container = cfg->container;
    sess = cfg->session_id;
    inst = cfg->instance_id;

    /* Update mirror state */
    if (mirror_enable) {
        if (port_mstate[unit]->uc_em[iport] & (1 << container) &&
            (port_mstate[unit]->uc_em_state[iport][container].sess != sess ||
             port_mstate[unit]->uc_em_state[iport][container].inst != inst)) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit, "The container(%d) of port(%d) "
                                        "has been occupied.\n"),
                                        container, iport));
            return SHR_E_RESOURCE;
        }
        if (port_mstate[unit]->uc_em[eport] & (1 << container) &&
            (port_mstate[unit]->uc_em_state[eport][container].sess != sess ||
             port_mstate[unit]->uc_em_state[eport][container].inst != inst)) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit, "The container(%d) of port(%d) "
                                        "has been occupied.\n"),
                                        container, eport));
            return SHR_E_RESOURCE;
        }
        port_mstate[unit]->uc_em[iport] |= 1 << container;
        port_mstate[unit]->uc_em[eport] |= 1 << container;
        port_mstate[unit]->uc_em_state[iport][container].sess = sess;
        port_mstate[unit]->uc_em_state[eport][container].sess = sess;
        port_mstate[unit]->uc_em_state[iport][container].inst = inst;
        port_mstate[unit]->uc_em_state[eport][container].inst = inst;
        port_mstate[unit]->uc_em_state[iport][container].ref_cnt++;
        port_mstate[unit]->uc_em_state[eport][container].ref_cnt++;
    } else {
        port_mstate[unit]->uc_em_state[iport][container].ref_cnt--;
        port_mstate[unit]->uc_em_state[eport][container].ref_cnt--;
        if (port_mstate[unit]->uc_em_state[iport][container].ref_cnt == 0) {
            port_mstate[unit]->uc_em[iport] &= ~(1 << container);
            port_mstate[unit]->uc_em_state[iport][container].sess = 0;
            port_mstate[unit]->uc_em_state[iport][container].inst = 0;
            update_imtp = TRUE;
        }
        if (port_mstate[unit]->uc_em_state[eport][container].ref_cnt == 0) {
            port_mstate[unit]->uc_em[eport] &= ~(1 << container);
            port_mstate[unit]->uc_em_state[eport][container].sess = 0;
            port_mstate[unit]->uc_em_state[eport][container].inst = 0;
            update_emtp = TRUE;
        }
    }

    /* Update EMIRROR_CONTROLm */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_port_emirror_control_set(unit,
                                              lt_id,
                                              op_arg,
                                              iport,
                                              eport,
                                              container,
                                              mirror_enable));

    /* Update MIRROR_CONTROLm on ingress port for non-UC */
    if (mirror_enable || update_imtp) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_port_mirror_control_set(unit,
                                                 lt_id,
                                                 op_arg,
                                                 iport,
                                                 container,
                                                 sess,
                                                 inst,
                                                 mirror_enable,
                                                 TRUE));
    }

    /* Update MIRROR_CONTROLm on egress port for UC */
    if (mirror_enable || update_emtp) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_port_mirror_control_set(unit,
                                                 lt_id,
                                                 op_arg,
                                                 eport,
                                                 container,
                                                 sess,
                                                 inst,
                                                 mirror_enable,
                                                 FALSE));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_port_deinit(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (port_mstate[unit] != NULL) {
        port_mstate[unit] = NULL;
    }

    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_port_init(int unit, bool warm)
{
    const bcmlrd_map_t *map = NULL;
    uint32_t ha_alloc_size = 0, ha_req_size = 0;
    int rv;

    SHR_FUNC_ENTER(unit);

    rv = bcmlrd_map_get(unit, PORT_EGR_MIRRORt, &map);
    if (SHR_FAILURE(rv) || !map) {
        SHR_EXIT();
    }

    ha_req_size = sizeof(bcmport_mirror_state_t);
    ha_alloc_size = ha_req_size;
    port_mstate[unit] = shr_ha_mem_alloc(unit,
                                         BCMMGMT_PORT_COMP_ID,
                                         BCMPORT_MIRROR_SUB_COMP_ID,
                                         "bcmcthPortMirrorState",
                                         &ha_alloc_size);
    SHR_NULL_CHECK(port_mstate[unit], SHR_E_MEMORY);
    SHR_IF_ERR_VERBOSE_EXIT
        ((ha_alloc_size < ha_req_size) ? SHR_E_MEMORY : SHR_E_NONE);

    if (!warm) {
        sal_memset(port_mstate[unit], 0, ha_alloc_size);
    }

exit:
    if (SHR_FUNC_ERR()) {
        shr_ha_mem_free(unit, port_mstate[unit]);
        port_mstate[unit] = NULL;
    }
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_cth_port_system_source_get(int unit,
                                       int port,
                                       int *system_source)
{
    bcmbd_pt_dyn_info_t pt_info;
    uint32_t rsp_entry_wsize;
    bcmltd_sid_t rsp_ltid = 0;
    uint32_t rsp_flags = 0;
    bcmdrd_sid_t pt_id = IDEV_CONFIG_TABLEm;
    bcmltd_sid_t lt_id = IDEV_CONFIG_TABLEm;
    IDEV_CONFIG_TABLEm_t buf;
    void *entry_data;

    SHR_FUNC_ENTER(unit);

    entry_data = (void *)&buf;
    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_info.index = port;
    rsp_entry_wsize = bcmdrd_pt_entry_wsize(unit, pt_id);

    SHR_IF_ERR_EXIT
        (bcmptm_ireq_read(unit,
                          BCMPTM_REQ_FLAGS_PASSTHRU,
                          pt_id,
                          &pt_info,
                          NULL,
                          rsp_entry_wsize,
                          lt_id,
                          entry_data,
                          &rsp_ltid,
                          &rsp_flags));

    *system_source = IDEV_CONFIG_TABLEm_SYSTEM_PORTf_GET(buf);

 exit:
    SHR_FUNC_EXIT();
}


static bcmcth_port_drv_t bcm56780_a0_cth_port_drv = {
   .port_init = bcm56780_a0_cth_port_init,
   .port_deinit = bcm56780_a0_cth_port_deinit,
   .port_system_dest_set = bcm56780_a0_cth_port_system_dest_set,
   .port_trunk_pctl_set = bcm56780_a0_cth_port_trunk_pctl_set,
   .port_mirror_set = &bcm56780_a0_cth_port_mirror_set,
   .port_egress_mirror_set = bcm56780_a0_cth_port_egress_mirror_set,
   .port_ingress_mirror_set = bcm56780_a0_cth_port_ingress_mirror_set,
   .port_system_source_get = bcm56780_a0_cth_port_system_source_get
};

/*******************************************************************************
 * Public Functions
 */
bcmcth_port_drv_t *
bcm56780_a0_cth_port_drv_get(int unit)
{
    return &bcm56780_a0_cth_port_drv;
}

