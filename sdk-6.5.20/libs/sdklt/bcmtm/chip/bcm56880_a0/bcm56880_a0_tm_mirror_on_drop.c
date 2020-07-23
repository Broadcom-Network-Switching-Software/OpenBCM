/*! \file bcm56880_a0_tm_mirror_on_drop.c
 *
 * TM Mirror-on-drop chip specific functionality implementation.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/chip/bcm56880_a0_enum.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_types.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include <bcmtm/chip/bcm56880_a0_tm.h>
#include "bcm56880_a0_tm_mirror_on_drop.h"
#include <bsl/bsl.h>
#include <sal/sal_sleep.h>

#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmdrd/bcmdrd_dev.h>

#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/imm/bcmtm_imm_defines_internal.h>
#include <bcmltd/chip/bcmltd_tm_constants.h>
#include <bcmtm/sched_shaper/bcmtm_scheduler_internal.h>
#include "bcm56880_a0_tm_scheduler_shaper.h"
#include <bcmlrd/bcmlrd_map.h>
#include <bcmtm/mod/bcmtm_mirror_on_drop_internal.h>
#include <bcmtm/chip/bcm56880_a0_tm_thd_internal.h>

/*******************************************************************************
 * Local definitions
 */

/* TM_MIRROR_DROP_DESTINATION_ENTRY_STATE_T */
#define MOD_MRDP_VALID                      0
#define MOD_MRDP_PORT_INFO_UNAVAILABLE      1
#define MOD_MRDP_INCORRECT_QUEUE            2

#define BSL_LOG_MODULE  BSL_LS_BCMTM_CHIP

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief Enable the MoD in H/W.

 * \param [in] unit Unit number.
 * \param [in] ltid Logical table identifier.
 * \param [in] enable Set to enable the MoD.
 * \param [out] enable_opr Operating value of enable.

 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Error when writing config to H/W.
 */
static int
bcm56880_a0_tm_mod_enable_hw_write(int unit,
                                   bcmltd_sid_t ltid,
                                   uint32_t enable,
                                   uint32_t *enable_opr)
{
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_sid_t ptid = MMU_THDO_MIRROR_ON_DROP_LIMIT_CONFIGr;
    uint32_t ltmbuf[BCMTM_MAX_THD_ENTRY_WSIZE] = {0};

    SHR_FUNC_ENTER(unit);
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);

    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, ltid, &pt_dyn_info,
                               ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, ptid, MIRROR_ON_DROP_ENABLEf,
                         ltmbuf, &enable));

    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, ptid, ltid,
                                (void*)&pt_dyn_info, ltmbuf));

    if (enable_opr) {
        *enable_opr = enable;
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Program the MoD buffer limit in H/W.

 * \param [in] unit Unit number.
 * \param [in] ltid Logical table identifier.
 * \param [in] buf_limit MoD buffer size in bytes.
 * \param [out] buf_limit_por MoD buffer size operating value.

 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Error when writing config to H/W.
 */
static int
bcm56880_a0_tm_mod_buffer_limit_hw_write(int unit,
                                   bcmltd_sid_t ltid,
                                   uint32_t buf_limit,
                                   uint32_t *buf_limit_opr)
{
    long int delta;
    bcmtm_pt_info_t pt_dyn_info = {0};
    bcmdrd_sid_t ptid = MMU_THDO_MIRROR_ON_DROP_LIMIT_CONFIGr;
    uint32_t cell_limit;
    uint32_t ltmbuf[BCMTM_MAX_THD_ENTRY_WSIZE] = {0};
    bool write_flag;


    SHR_FUNC_ENTER(unit);

    write_flag = TRUE;

    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, ltid, &pt_dyn_info,
                               ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, ptid, MIRROR_ON_DROP_CELL_LIMITf,
                         ltmbuf, &cell_limit));

    if (buf_limit_opr) {
        *buf_limit_opr = cell_limit;
    }

    /*
     * Calculate delta of available shared size.
     * delta of shared_size always equals to -(delta of reserved_size).
     */
    delta = - ((long int) buf_limit - cell_limit);
    /* Check if both ITM have enough buffer to allocate the delta. */
    if (SHR_SUCCESS
            (bcm56880_a0_tm_shared_buffer_update(unit, 0, -1,
                                                 delta, RESERVED_BUFFER_UPDATE,
                                                 TRUE)) &&
             SHR_SUCCESS
                 (bcm56880_a0_tm_shared_buffer_update(unit, 1, -1,
                                                delta, RESERVED_BUFFER_UPDATE,
                                                TRUE))) {
        /* Update the service pool size in both ITM. */
        SHR_IF_ERR_EXIT
            (bcm56880_a0_tm_shared_buffer_update(unit, 0, -1, delta,
                                                 RESERVED_BUFFER_UPDATE,
                                                 FALSE));
       SHR_IF_ERR_EXIT
           (bcm56880_a0_tm_shared_buffer_update(unit, 1, -1, delta,
                                                RESERVED_BUFFER_UPDATE,
                                                FALSE));
     } else {
         write_flag = FALSE;
     }

     if (write_flag) {
         BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
         SHR_IF_ERR_EXIT
             (bcmtm_field_set(unit, ptid, MIRROR_ON_DROP_CELL_LIMITf,
                              ltmbuf, &buf_limit));
         SHR_IF_ERR_EXIT
             (bcmtm_pt_indexed_write(unit, ptid, ltid,
                                     (void*)&pt_dyn_info, ltmbuf));
         if (buf_limit_opr) {
             *buf_limit_opr = buf_limit;
         }
     }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_tm_mod_dest_delete_hw(int unit,
                                  bcmltd_sid_t sid)
{
    bcmdrd_sid_t ptid;
    uint32_t ltmbuf[BCMTM_MAX_THD_ENTRY_WSIZE] = {0};
    bcmtm_pt_info_t pt_dyn_info = {0};
    int32_t inst, pipe = 0;
    uint32_t pipe_map;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmdrd_dev_valid_pipes_get(unit, &pipe_map));

    /* THDO */
    ptid =  MMU_THDO_MIRROR_ON_DROP_DESTINATION_CONFIGr;
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, ptid, sid,
                               (void*)&pt_dyn_info, ltmbuf));

    /* TOQ */
    ptid =  MMU_TOQ_CQEB_MIRROR_ON_DROP_DESTINATION_CONFIGr;
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, ptid, sid,
                                (void*)&pt_dyn_info, ltmbuf));

    ptid =  MMU_TOQ_MIRROR_ON_DROP_DESTINATION_CONFIGr;
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, ptid, sid,
                                (void*)&pt_dyn_info, ltmbuf));

    /* QSCH */
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    ptid = MMU_QSCH_MIRROR_ON_DROP_DESTINATION_CONFIGr;
    for (pipe = 0; pipe < TD4_PIPES_PER_DEV; pipe++) {
        if (pipe_map & (1 << pipe)) {
            inst = pipe;
            BCMTM_PT_DYN_INFO(pt_dyn_info, 0, inst);
            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit, ptid, sid,
                                       (void*)&pt_dyn_info, ltmbuf));
        }
    }

    /* EBCTM */
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    ptid = MMU_EBCTM_EPIPE_CT_CFGr;
    for (pipe = 0; pipe < TD4_PIPES_PER_DEV; pipe++) {
        if (pipe_map & (1 << pipe)) {
            inst = pipe;
            BCMTM_PT_DYN_INFO(pt_dyn_info, 0, inst);

            SHR_IF_ERR_EXIT
                (bcmtm_pt_indexed_write(unit, ptid, sid,
                                        (void*)&pt_dyn_info, ltmbuf));
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_tm_mod_dest_delete(int unit,
                               bcmltd_sid_t sid)

{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcm56880_a0_tm_mod_dest_delete_hw(unit, sid));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_tm_mod_dest_set_hw(int unit,
                               bcmltd_sid_t sid,
                               uint32_t lport,
                               uint32_t queue)
{
    bcmdrd_sid_t ptid;
    int32_t mport;
    uint32_t lcl_mmu_port;
    uint32_t ltmbuf[BCMTM_MAX_THD_ENTRY_WSIZE] = {0};
    bcmtm_pt_info_t pt_dyn_info = {0};
    int32_t inst, pipe;
    uint32_t enable = 1;
    #define TD4_MMU_PORT_PIPE_OFFSET 32
    SHR_FUNC_ENTER(unit);

    /* THDO */
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    SHR_IF_ERR_EXIT(bcmtm_lport_mport_get(unit, lport, &mport));

    SHR_IF_ERR_EXIT(bcmtm_lport_pipe_get(unit, lport, &pipe));

    ptid =  MMU_THDO_MIRROR_ON_DROP_DESTINATION_CONFIGr;
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT
         (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info,
                                ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, ptid, MIRROR_ON_DROP_DESTINATION_PORTf,
                         ltmbuf, (uint32_t *)&mport));
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, ptid, MIRROR_ON_DROP_DESTINATION_QUEUEf,
                         ltmbuf, &queue));
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, ptid, sid,
                                   (void*)&pt_dyn_info, ltmbuf));

    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    ptid =  MMU_TOQ_MIRROR_ON_DROP_DESTINATION_CONFIGr;
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT
         (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info,
                                ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, ptid, DESTINATION_PORTf,
                         ltmbuf, (uint32_t *)&mport));
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, ptid, DESTINATION_QUEUEf,
                         ltmbuf, &queue));
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, ptid, VALIDf,
                         ltmbuf, &enable));
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, ptid, sid,
                                (void*)&pt_dyn_info, ltmbuf));

    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    ptid =  MMU_TOQ_CQEB_MIRROR_ON_DROP_DESTINATION_CONFIGr;
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT
         (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info,
                                ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, ptid, DESTINATION_PORTf,
                         ltmbuf, (uint32_t *)&mport));
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, ptid, DESTINATION_QUEUEf,
                         ltmbuf, &queue));
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, ptid, VALIDf,
                         ltmbuf, &enable));
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, ptid, sid,
                                (void*)&pt_dyn_info, ltmbuf));


    /* QSCH */
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));

    ptid = MMU_QSCH_MIRROR_ON_DROP_DESTINATION_CONFIGr;
    inst = pipe;
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, inst);
    lcl_mmu_port = mport % TD4_MMU_PORT_PIPE_OFFSET;

    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));

    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, ptid, DEST_PORTf, ltmbuf, &lcl_mmu_port));
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, ptid, DEST_QUEUEf, ltmbuf, &queue));
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, ptid, ACTIVEf, ltmbuf, &enable));

    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, ptid, sid,
                               (void*)&pt_dyn_info, ltmbuf));

    /* EBCTM */
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));

    ptid = MMU_EBCTM_EPIPE_CT_CFGr;
    inst = pipe;
    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, inst);

    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, sid, &pt_dyn_info, ltmbuf));

    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, ptid, MIRROR_ON_DROP_LOCAL_DEST_PORTf,
                         ltmbuf, &lcl_mmu_port));
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, ptid, MIRROR_ON_DROP_DEST_PORT_ACTIVEf,
                         ltmbuf, &enable));
    SHR_IF_ERR_EXIT
       (bcmtm_pt_indexed_write(unit, ptid, sid,
                              (void*)&pt_dyn_info, ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Program the MoD destination in H/W.

 * \param [in] unit Unit number.
 * \param [in] ltid Logical table identifier.
 * \param [in] dest_id MoD destination id.
 * \param [in] dest_info MoD destination information.
 * \param [in] action specificies internal action.
 * \param [out] opcode operational status of the entry.
 * \param [in] opr_old current operational status of the entry.

 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Error when writing config to H/W.
 */
static int
bcm56880_a0_tm_mod_dest_set(int unit,
                   bcmltd_sid_t ltid,
                   uint32_t dest_id,
                   bcmtm_mod_dest_entry_info_t dest_info,
                   uint8_t action,
                   uint32_t *opcode,
                   uint32_t opr_old)
{
    bcmdrd_dev_type_t dev_type;
    int32_t q_out = -1, mport = -1;
    int32_t port_valid = 1;
    int32_t queue_valid = 1;
    int num_ucq = 0, num_mcq =0;
    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    if ((action == ACTION_INSERT) || (action == ACTION_UPDATE)) {
        if (SHR_FAILURE(bcmtm_lport_mport_get(unit, dest_info.lport, &mport))) {
            port_valid = 0;
        }
    }
    if ((action == ACTION_INSERT) || (action == ACTION_UPDATE) ||
         (action == ACTION_PORT_UPDATE_INT)) {

         if (SHR_FAILURE(bcmtm_lport_num_ucq_get(unit, dest_info.lport,
                            &num_ucq))) {
             num_ucq = 0;
         }

         if (SHR_FAILURE(bcmtm_lport_num_mcq_get(unit, dest_info.lport,
                            &num_mcq))) {
             num_mcq = 0;
         }

        if (dest_info.ucast == 1) {
            if (dest_info.uc_q_id >= (uint32_t)num_ucq) {
                queue_valid = 0;
            }
        } else {
            if (dest_info.mc_q_id >= (uint32_t)num_mcq) {
                queue_valid = 0;
            }
        }

        if ((!port_valid) || (!queue_valid)) {
             /* Delete the old MRDP. */
             if ((action == ACTION_UPDATE) && (opr_old == MOD_MRDP_VALID)) {
                    SHR_IF_ERR_EXIT
                        (bcm56880_a0_tm_mod_dest_delete(unit, ltid));
             }
        }

        if (!port_valid) {
            *opcode = MOD_MRDP_PORT_INFO_UNAVAILABLE;
        } else if (!queue_valid) {
            *opcode = MOD_MRDP_INCORRECT_QUEUE;
        } else {
            if (dest_info.ucast == 1) {
                q_out = dest_info.uc_q_id;
            } else {
                q_out = num_ucq + dest_info.mc_q_id;
            }

            /* Program the new MRDP in TOQ, QSCH, EBCTM. */
            SHR_IF_ERR_EXIT
                (bcm56880_a0_tm_mod_dest_set_hw(unit, ltid, dest_info.lport, q_out));
            *opcode = MOD_MRDP_VALID;
        }
        SHR_EXIT();
    } else if ( action == ACTION_DELETE) {
        SHR_IF_ERR_EXIT
            (bcm56880_a0_tm_mod_dest_delete_hw(unit, ltid));
    } else if (action == ACTION_PORT_DELETE_INT) {
        SHR_IF_ERR_EXIT
            (bcm56880_a0_tm_mod_dest_delete_hw(unit, ltid));
        *opcode = MOD_MRDP_PORT_INFO_UNAVAILABLE;
    }
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Program the MoD encap profile in H/W.

 * \param [in] unit Unit number.
 * \param [in] ltid Logical table identifier.
 * \param [in] encap_prof_id MoD encap profile id.
 * \param [in] encap_prof_info MoD encap profile information.
 * \param [in] action specificies internal action.

* \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Error when writing config to H/W.
 */
static int
bcm56880_a0_tm_mod_encap_set(int unit,
                             bcmltd_sid_t ltid,
                             uint32_t encap_prof_id,
                             bcmtm_mod_encap_prof_entry_info_t encap_prof_info,
                             uint8_t action)
{
    bcmdrd_sid_t ptid;
    uint32_t ltmbuf[BCMTM_MAX_THD_ENTRY_WSIZE] = {0};
    bcmtm_pt_info_t pt_dyn_info = {0};
    uint32_t mirror_instance_id = encap_prof_info.mirror_instance_id;

    SHR_FUNC_ENTER(unit);

    BCMTM_PT_DYN_INFO(pt_dyn_info, 0, 0);
    sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    ptid = MMU_TOQ_CQEB_MIRROR_ON_DROP_MIRR_ENCAP_INDEX_CONFIGr;
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, ptid, ltid, &pt_dyn_info,
                               ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, ptid, MIRROR_ENCAP_INDEXf,
                         ltmbuf, (uint32_t *)&mirror_instance_id));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, ptid, ltid,
                                (void*)&pt_dyn_info, ltmbuf));

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public Functions
 */
int
bcm56880_a0_tm_mod_drv_get(int unit, void *mod_drv)
{
    bcmtm_mod_drv_t bcm56880_a0_mod_drv = {
        .enable_set = bcm56880_a0_tm_mod_enable_hw_write,
        .buffer_set = bcm56880_a0_tm_mod_buffer_limit_hw_write,
        .dest_set = bcm56880_a0_tm_mod_dest_set,
        .encap_set = bcm56880_a0_tm_mod_encap_set,
    };
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(mod_drv, SHR_E_INTERNAL);
    *((bcmtm_mod_drv_t *)mod_drv) = bcm56880_a0_mod_drv;
exit:
    SHR_FUNC_EXIT();
}


