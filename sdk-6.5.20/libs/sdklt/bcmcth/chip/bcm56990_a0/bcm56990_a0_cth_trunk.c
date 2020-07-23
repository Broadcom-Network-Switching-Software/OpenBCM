/*! \file bcm56990_a0_cth_trunk.c
 *
 * Chip stub for BCMCTH TRUNK.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>

#include <bcmdrd/chip/bcm56990_a0_defs.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmcth/bcmcth_trunk_util.h>
#include <bcmcth/bcmcth_trunk_drv.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_TRUNK

/*******************************************************************************
 * Local definitions
 */

#define BCM56990_FAST_TRUNK_SIZE 64

/*******************************************************************************
 * Typedefs
 */


/*******************************************************************************
 * Private variables
 */

static bcmdrd_fid_t mem_fids[BCM56990_FAST_TRUNK_SIZE] = {
    TRUNK_MEMBER_0f, TRUNK_MEMBER_1f, TRUNK_MEMBER_2f, TRUNK_MEMBER_3f,
    TRUNK_MEMBER_4f, TRUNK_MEMBER_5f, TRUNK_MEMBER_6f, TRUNK_MEMBER_7f,
    TRUNK_MEMBER_8f, TRUNK_MEMBER_9f, TRUNK_MEMBER_10f, TRUNK_MEMBER_11f,
    TRUNK_MEMBER_12f, TRUNK_MEMBER_13f, TRUNK_MEMBER_14f, TRUNK_MEMBER_15f,
    TRUNK_MEMBER_16f, TRUNK_MEMBER_17f, TRUNK_MEMBER_18f, TRUNK_MEMBER_19f,
    TRUNK_MEMBER_20f, TRUNK_MEMBER_21f, TRUNK_MEMBER_22f, TRUNK_MEMBER_23f,
    TRUNK_MEMBER_24f, TRUNK_MEMBER_25f, TRUNK_MEMBER_26f, TRUNK_MEMBER_27f,
    TRUNK_MEMBER_28f, TRUNK_MEMBER_29f, TRUNK_MEMBER_30f, TRUNK_MEMBER_31f,
    TRUNK_MEMBER_32f, TRUNK_MEMBER_33f, TRUNK_MEMBER_34f, TRUNK_MEMBER_35f,
    TRUNK_MEMBER_36f, TRUNK_MEMBER_37f, TRUNK_MEMBER_38f, TRUNK_MEMBER_39f,
    TRUNK_MEMBER_40f, TRUNK_MEMBER_41f, TRUNK_MEMBER_42f, TRUNK_MEMBER_43f,
    TRUNK_MEMBER_44f, TRUNK_MEMBER_45f, TRUNK_MEMBER_46f, TRUNK_MEMBER_47f,
    TRUNK_MEMBER_48f, TRUNK_MEMBER_49f, TRUNK_MEMBER_50f, TRUNK_MEMBER_51f,
    TRUNK_MEMBER_52f, TRUNK_MEMBER_53f, TRUNK_MEMBER_54f, TRUNK_MEMBER_55f,
    TRUNK_MEMBER_56f, TRUNK_MEMBER_57f, TRUNK_MEMBER_58f, TRUNK_MEMBER_59f,
    TRUNK_MEMBER_60f, TRUNK_MEMBER_61f, TRUNK_MEMBER_62f, TRUNK_MEMBER_63f
};


/*******************************************************************************
 * Private Functions
 */

static int
bcm56990_a0_trunk_block_mask_set(int unit,
                                 const bcmltd_op_arg_t *op_arg,
                                 bcmltd_sid_t lt_id,
                                 bcmdrd_pbmp_t old_trunk_bmp,
                                 bcmdrd_pbmp_t new_trunk_bmp,
                                 uint16_t new_cnt,
                                 uint16_t *new_modports)
{
    bcmdrd_pbmp_t pbmp;
    int  tbl_sz, region_sz, ent_sz, idx;
    uint32_t *dma_buf = NULL, *ent_buf = NULL;
    bcmdrd_sid_t pt_id = NONUCAST_TRUNK_BLOCK_MASKm;

    SHR_FUNC_ENTER(unit);

    /*
     * The NONUCAST_TRUNK_BLOCK_MASK table is divided into 4 regions:
     * IPMC, L2MC, broadcast, and unknown unicast/multicast. Same datas
     * are set to 4 regions.
     */
    tbl_sz = bcmdrd_pt_index_max(unit, pt_id)
             - bcmdrd_pt_index_min(unit, pt_id) + 1;
    region_sz = tbl_sz / 4;
    ent_sz = bcmdrd_pt_entry_wsize(unit, NONUCAST_TRUNK_BLOCK_MASKm);
    SHR_ALLOC(dma_buf, ent_sz * tbl_sz * sizeof(uint32_t), "bcmcthTrunkBlockBuf");
    SHR_NULL_CHECK(dma_buf, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT
        (bcmcth_trunk_hw_read_dma(unit, op_arg, lt_id,
                                  NONUCAST_TRUNK_BLOCK_MASKm,
                                  0, tbl_sz, dma_buf));

    BCMDRD_PBMP_CLEAR(pbmp);
    for (idx = 0; idx < region_sz; idx++) {
        ent_buf = dma_buf + idx * ent_sz;
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_get(unit, pt_id, ent_buf, BLOCK_MASKf, pbmp.w));
        BCMDRD_PBMP_REMOVE(pbmp, old_trunk_bmp);
        BCMDRD_PBMP_OR(pbmp, new_trunk_bmp);

        if (new_cnt > 0) {
            BCMDRD_PBMP_PORT_REMOVE(pbmp, new_modports[idx % new_cnt]);
        }

        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, pt_id, ent_buf, BLOCK_MASKf, pbmp.w));
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, pt_id, ent_buf + 1 * region_sz * ent_sz,
                                 BLOCK_MASKf, pbmp.w));
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, pt_id, ent_buf + 2 * region_sz * ent_sz,
                                 BLOCK_MASKf, pbmp.w));
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, pt_id, ent_buf + 3 * region_sz * ent_sz,
                                 BLOCK_MASKf, pbmp.w));
    }

    SHR_IF_ERR_EXIT
        (bcmcth_trunk_hw_write_dma(unit, op_arg, lt_id, pt_id,
                                   0, tbl_sz, dma_buf));

exit:
    SHR_FREE(dma_buf);
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_trunk_fast_grp_populate(int unit, const bcmltd_op_arg_t *op_arg,
                                    bcmltd_sid_t lt_id,
                                    bcmcth_trunk_fast_group_param_t *param)
{
    uint32_t grp_buf1[BCMDRD_MAX_PT_WSIZE];
    uint32_t grp_buf2[BCMDRD_MAX_PT_WSIZE];
    uint32_t *grp_ptr;
    FAST_TRUNK_SIZEm_t size_buf;
    TRUNK_RAND_LB_SEED_INST0r_t rand_seed_inst0_buf;
    TRUNK_RAND_LB_SEED_INST1r_t rand_seed_inst1_buf;
    int trunk_id, idx, seed;
    bcmdrd_sid_t mem_sid;
    uint32_t value;

    SHR_FUNC_ENTER(unit);

    trunk_id =  param->id;
    /* On TH4, fast trunk hw table are split into two tables. */
    sal_memset(grp_buf1, 0, sizeof(grp_buf1));
    sal_memset(grp_buf2, 0, sizeof(grp_buf2));
    mem_sid = FAST_TRUNK_PORTS_1m;
    grp_ptr = grp_buf1;
    for (idx = 0; idx < param->uc_cnt; idx++) {
        if (idx > 31) {
            mem_sid = FAST_TRUNK_PORTS_2m;
            grp_ptr = grp_buf2;
        }
        value = param->uc_modport[idx];
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit, mem_sid, grp_ptr, mem_fids[idx], &value));
    }
    value = param->ing_eflex_action_id;
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_set(unit, FAST_TRUNK_PORTS_1m, grp_buf1,
                             FLEX_CTR_ACTION_SELf, &value));
    value = param->ing_eflex_object;
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_set(unit, FAST_TRUNK_PORTS_1m, grp_buf1,
                             FLEX_CTR_OBJECTf, &value));
    SHR_IF_ERR_EXIT
        (bcmcth_trunk_hw_write(unit, op_arg, lt_id, FAST_TRUNK_PORTS_1m,
                               trunk_id, &grp_buf1));
    SHR_IF_ERR_EXIT
        (bcmcth_trunk_hw_write(unit, op_arg, lt_id, FAST_TRUNK_PORTS_2m,
                               trunk_id, &grp_buf2));

    FAST_TRUNK_SIZEm_CLR(size_buf);
    if (param->uc_cnt > 0) {
        FAST_TRUNK_SIZEm_TG_SIZEf_SET(size_buf, (param->uc_cnt - 1));
    }
    FAST_TRUNK_SIZEm_TRUNK_MODEf_SET(size_buf, param->lb_mode);
    SHR_IF_ERR_EXIT
        (bcmcth_trunk_hw_write(unit, op_arg, lt_id, FAST_TRUNK_SIZEm,
                               trunk_id, &size_buf));

    if (param->lb_mode_valid) {
        if (param->lb_mode == BCMCTH_TRUNK_LB_RANDOM) {
            /* coverity[dont_call] */
            seed = (sal_rand()) % 0xFFFF;

            TRUNK_RAND_LB_SEED_INST0r_CLR(rand_seed_inst0_buf);
            TRUNK_RAND_LB_SEED_INST0r_SEEDf_SET(rand_seed_inst0_buf, seed);
            TRUNK_RAND_LB_SEED_INST1r_CLR(rand_seed_inst1_buf);
            TRUNK_RAND_LB_SEED_INST1r_SEEDf_SET(rand_seed_inst1_buf, seed);
            /* Program the seed to MTPP-2T INST0 and INST1. */
            SHR_IF_ERR_EXIT
                (bcmcth_trunk_hw_write(unit, op_arg, lt_id,
                                       TRUNK_RAND_LB_SEED_INST0r, 0,
                                       &rand_seed_inst0_buf));
            SHR_IF_ERR_EXIT
                (bcmcth_trunk_hw_write(unit, op_arg, lt_id,
                                       TRUNK_RAND_LB_SEED_INST1r, 0,
                                       &rand_seed_inst1_buf));
        }
    }

exit:
    SHR_FUNC_EXIT();
}


static int
bcm56990_a0_trunk_fast_grp_add(int unit,
                               const bcmltd_op_arg_t *op_arg,
                               bcmltd_sid_t lt_id,
                               bcmcth_trunk_fast_group_param_t *param)
{
    TRUNK_BITMAPm_t bitmap_buf;
    bcmdrd_pbmp_t old_trunk_bmp, new_trunk_bmp;
    int trunk_id, px;

    SHR_FUNC_ENTER(unit);

    trunk_id = param->id;
    SHR_IF_ERR_EXIT
        (bcm56990_a0_trunk_fast_grp_populate(unit, op_arg, lt_id, param));

    BCMDRD_PBMP_CLEAR(old_trunk_bmp);
    BCMDRD_PBMP_CLEAR(new_trunk_bmp);

    /* Write TRUNK_BITMAPm. */
    for (px = 0; px < param->uc_cnt; px++) {
        BCMDRD_PBMP_PORT_ADD(new_trunk_bmp, param->uc_modport[px]);
    }
    for (px = 0; px < param->nonuc_cnt; px++) {
        BCMDRD_PBMP_PORT_ADD(new_trunk_bmp, param->nonuc_modport[px]);
    }
    BCMDRD_PBMP_CLEAR(old_trunk_bmp);
    TRUNK_BITMAPm_CLR(bitmap_buf);
    TRUNK_BITMAPm_TRUNK_BITMAPf_SET(bitmap_buf, new_trunk_bmp.w);
    SHR_IF_ERR_EXIT
        (bcmcth_trunk_hw_write(unit, op_arg, lt_id, TRUNK_BITMAPm,
                               trunk_id, &bitmap_buf));

    /* Write NONUCAST_TRUNK_BLOCK_MASKm. */
    if (BCMDRD_PBMP_NOT_NULL(new_trunk_bmp)) {
        SHR_IF_ERR_EXIT
            (bcm56990_a0_trunk_block_mask_set(unit,
                                              op_arg,
                                              lt_id,
                                              old_trunk_bmp,
                                              new_trunk_bmp,
                                              param->nonuc_cnt,
                                              param->nonuc_modport));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_trunk_fast_grp_del(int unit,
                               const bcmltd_op_arg_t *op_arg,
                               bcmltd_sid_t lt_id,
                               bcmcth_trunk_fast_group_param_t *param,
                               bcmcth_trunk_fast_group_t *grp)
{
    FAST_TRUNK_PORTS_1m_t grp_buf1;
    FAST_TRUNK_PORTS_2m_t grp_buf2;
    TRUNK_BITMAPm_t bitmap_buf;
    FAST_TRUNK_SIZEm_t size_buf;
    bcmdrd_pbmp_t old_trunk_bmp, new_trunk_bmp;
    int trunk_id;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(grp);

    trunk_id = param->id;

    FAST_TRUNK_PORTS_1m_CLR(grp_buf1);
    SHR_IF_ERR_EXIT
        (bcmcth_trunk_hw_write(unit, op_arg, lt_id, FAST_TRUNK_PORTS_1m,
                               trunk_id, &grp_buf1));

    FAST_TRUNK_PORTS_2m_CLR(grp_buf2);
    SHR_IF_ERR_EXIT
        (bcmcth_trunk_hw_write(unit, op_arg, lt_id, FAST_TRUNK_PORTS_2m,
                               trunk_id, &grp_buf2));

    FAST_TRUNK_SIZEm_CLR(size_buf);
    SHR_IF_ERR_EXIT
        (bcmcth_trunk_hw_write(unit, op_arg, lt_id, FAST_TRUNK_SIZEm,
                               trunk_id, &size_buf));

    BCMDRD_PBMP_CLEAR(old_trunk_bmp);
    BCMDRD_PBMP_CLEAR(new_trunk_bmp);
    SHR_IF_ERR_EXIT
        (bcmcth_trunk_hw_read(unit, op_arg, lt_id, TRUNK_BITMAPm,
                              trunk_id, &bitmap_buf));
    TRUNK_BITMAPm_TRUNK_BITMAPf_GET(bitmap_buf, old_trunk_bmp.w);
    if (BCMDRD_PBMP_NOT_NULL(old_trunk_bmp)) {
        SHR_IF_ERR_EXIT
            (bcm56990_a0_trunk_block_mask_set(unit, op_arg, lt_id,
                                              old_trunk_bmp, new_trunk_bmp,
                                              0, NULL));
    }

    TRUNK_BITMAPm_CLR(bitmap_buf);
    SHR_IF_ERR_EXIT
        (bcmcth_trunk_hw_write(unit, op_arg, lt_id, TRUNK_BITMAPm,
                               trunk_id, &bitmap_buf));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_trunk_fast_grp_update(int unit,
                                  const bcmltd_op_arg_t *op_arg,
                                  bcmltd_sid_t lt_id,
                                  bcmcth_trunk_fast_group_param_t *param,
                                  bcmcth_trunk_fast_group_t *grp)
{
    TRUNK_BITMAPm_t bitmap_buf;
    bcmdrd_pbmp_t old_trunk_bmp, new_trunk_bmp;
    int trunk_id, px;

    SHR_FUNC_ENTER(unit);

    trunk_id = param->id;
    (void) bcmcth_trunk_fast_param_update(unit, param, grp);
    SHR_IF_ERR_EXIT
        (bcm56990_a0_trunk_fast_grp_populate(unit, op_arg, lt_id, param));

    BCMDRD_PBMP_CLEAR(new_trunk_bmp);
    /* Write TRUNK_BITMAPm. */
    for (px = 0; px < param->uc_cnt; px++) {
        BCMDRD_PBMP_PORT_ADD(new_trunk_bmp, param->uc_modport[px]);
    }
    for (px = 0; px < param->nonuc_cnt; px++) {
        BCMDRD_PBMP_PORT_ADD(new_trunk_bmp, param->nonuc_modport[px]);
    }
    BCMDRD_PBMP_CLEAR(old_trunk_bmp);
    TRUNK_BITMAPm_CLR(bitmap_buf);
    SHR_IF_ERR_EXIT
        (bcmcth_trunk_hw_read(unit, op_arg, lt_id, TRUNK_BITMAPm,
                              trunk_id, &bitmap_buf));
    TRUNK_BITMAPm_TRUNK_BITMAPf_GET(bitmap_buf, old_trunk_bmp.w);

    TRUNK_BITMAPm_TRUNK_BITMAPf_SET(bitmap_buf, new_trunk_bmp.w);
    SHR_IF_ERR_EXIT
        (bcmcth_trunk_hw_write(unit, op_arg, lt_id, TRUNK_BITMAPm,
                               trunk_id, &bitmap_buf));

    /* Write NONUCAST_TRUNK_BLOCK_MASKm. */
    if (BCMDRD_PBMP_NOT_NULL(old_trunk_bmp) ||
        BCMDRD_PBMP_NOT_NULL(new_trunk_bmp)) {
        SHR_IF_ERR_EXIT
            (bcm56990_a0_trunk_block_mask_set(unit,
                                              op_arg,
                                              lt_id,
                                              old_trunk_bmp,
                                              new_trunk_bmp,
                                              param->nonuc_cnt,
                                              param->nonuc_modport));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_trunk_fast_grp_init(int unit, bcmcth_trunk_fgrp_bk_t *fgrp_bk)
{
    SHR_FUNC_ENTER(unit);
    fgrp_bk->max_members = BCM56990_FAST_TRUNK_SIZE;

    SHR_FUNC_EXIT();
}


static int
bcm56990_a0_trunk_failover_add(int unit,
                               const bcmltd_op_arg_t *op_arg,
                               bcmltd_sid_t lt_id,
                               bcmcth_trunk_failover_param_t *param)
{
    PORT_LAG_FAILOVER_SETm_t failover_buf;
    int cnt;

    SHR_FUNC_ENTER(unit);

    PORT_LAG_FAILOVER_SETm_CLR(failover_buf);
    PORT_LAG_FAILOVER_SETm_RTAGf_SET(failover_buf, param->rtag);
    cnt = param->failover_cnt;
    if (cnt > 0 ) {
        PORT_LAG_FAILOVER_SETm_FAILOVER_SET_SIZEf_SET(failover_buf, (cnt - 1));
        /* Every filed should be set even when the cnt is less than 8. */
        PORT_LAG_FAILOVER_SETm_PORT0f_SET(failover_buf,
                                          param->failover_modport[0]);
        PORT_LAG_FAILOVER_SETm_PORT1f_SET(failover_buf,
                                          param->failover_modport[1 % cnt]);
        PORT_LAG_FAILOVER_SETm_PORT2f_SET(failover_buf,
                                          param->failover_modport[2 % cnt]);
        PORT_LAG_FAILOVER_SETm_PORT3f_SET(failover_buf,
                                          param->failover_modport[3 % cnt]);
        PORT_LAG_FAILOVER_SETm_PORT4f_SET(failover_buf,
                                          param->failover_modport[4 % cnt]);
        PORT_LAG_FAILOVER_SETm_PORT5f_SET(failover_buf,
                                          param->failover_modport[5 % cnt]);
        PORT_LAG_FAILOVER_SETm_PORT6f_SET(failover_buf,
                                          param->failover_modport[6 % cnt]);
        PORT_LAG_FAILOVER_SETm_PORT7f_SET(failover_buf,
                                          param->failover_modport[7 % cnt]);
    }

    SHR_IF_ERR_EXIT
        (bcmcth_trunk_hw_write(unit, op_arg, lt_id,
                               PORT_LAG_FAILOVER_SETm,
                               param->id, &failover_buf));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_trunk_failover_del(int unit,
                               const bcmltd_op_arg_t *op_arg,
                               bcmltd_sid_t lt_id,
                               bcmcth_trunk_failover_param_t *param)
{
    PORT_LAG_FAILOVER_SETm_t failover_buf;

    SHR_FUNC_ENTER(unit);

    PORT_LAG_FAILOVER_SETm_CLR(failover_buf);
    SHR_IF_ERR_EXIT
        (bcmcth_trunk_hw_write(unit, op_arg,
                               TRUNK_FAILOVERt, PORT_LAG_FAILOVER_SETm,
                               param->id, &failover_buf));
exit:
   SHR_FUNC_EXIT();
}

static int
bcm56990_a0_trunk_failover_update(int unit,
                                  const bcmltd_op_arg_t *op_arg,
                                  bcmltd_sid_t lt_id,
                                  bcmcth_trunk_failover_param_t *param,
                                  bcmcth_trunk_failover_t *entry)
{
    PORT_LAG_FAILOVER_SETm_t failover_buf;
    int cnt;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmcth_trunk_failover_param_update(unit, param, entry));

    PORT_LAG_FAILOVER_SETm_CLR(failover_buf);
    PORT_LAG_FAILOVER_SETm_RTAGf_SET(failover_buf, param->rtag);
    cnt = param->failover_cnt;
    if (cnt > 0) {
        PORT_LAG_FAILOVER_SETm_FAILOVER_SET_SIZEf_SET(failover_buf, cnt - 1);
        /* Every field should be set even when the cnt is less than 8. */
        PORT_LAG_FAILOVER_SETm_PORT0f_SET(failover_buf,
                                          param->failover_modport[0]);
        PORT_LAG_FAILOVER_SETm_PORT1f_SET(failover_buf,
                                          param->failover_modport[1 % cnt]);
        PORT_LAG_FAILOVER_SETm_PORT2f_SET(failover_buf,
                                          param->failover_modport[2 % cnt]);
        PORT_LAG_FAILOVER_SETm_PORT3f_SET(failover_buf,
                                          param->failover_modport[3 % cnt]);
        PORT_LAG_FAILOVER_SETm_PORT4f_SET(failover_buf,
                                          param->failover_modport[4 % cnt]);
        PORT_LAG_FAILOVER_SETm_PORT5f_SET(failover_buf,
                                          param->failover_modport[5 % cnt]);
        PORT_LAG_FAILOVER_SETm_PORT6f_SET(failover_buf,
                                          param->failover_modport[6 % cnt]);
        PORT_LAG_FAILOVER_SETm_PORT7f_SET(failover_buf,
                                          param->failover_modport[7 % cnt]);
    }

    SHR_IF_ERR_EXIT
        (bcmcth_trunk_hw_write(unit, op_arg, lt_id,
                               PORT_LAG_FAILOVER_SETm,
                               param->id, &failover_buf));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_trunk_failover_init(int unit,
                                bcmcth_trunk_failover_bk_t *failover_bk)
{
    int mem_tbl_size;

    SHR_FUNC_ENTER(unit);

    mem_tbl_size = bcmdrd_pt_index_max(unit, PORT_LAG_FAILOVER_SETm)
                   - bcmdrd_pt_index_min(unit, PORT_LAG_FAILOVER_SETm)
                   + 1;
    if (mem_tbl_size < 0) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
    failover_bk->max_ports = mem_tbl_size;

exit:
    SHR_FUNC_EXIT();
}

static bcmcth_trunk_drv_t bcm56990_a0_cth_trunk_drv = {
    .fn_trunk_mode_set   = NULL,
    .fn_grp_add          = NULL,
    .fn_grp_update       = NULL,
    .fn_grp_del          = NULL,
    .fn_grp_init         = NULL,
    .fn_fast_grp_add     = &bcm56990_a0_trunk_fast_grp_add,
    .fn_fast_grp_del     = &bcm56990_a0_trunk_fast_grp_del,
    .fn_fast_grp_update  = &bcm56990_a0_trunk_fast_grp_update,
    .fn_fast_grp_init    = &bcm56990_a0_trunk_fast_grp_init,
    .fn_failover_add     = &bcm56990_a0_trunk_failover_add,
    .fn_failover_del     = &bcm56990_a0_trunk_failover_del,
    .fn_failover_update  = &bcm56990_a0_trunk_failover_update,
    .fn_failover_init    = &bcm56990_a0_trunk_failover_init
};

/*******************************************************************************
 * Public Functions
 */

bcmcth_trunk_drv_t *
bcm56990_a0_cth_trunk_drv_get(int unit)
{
    return &bcm56990_a0_cth_trunk_drv;
}
