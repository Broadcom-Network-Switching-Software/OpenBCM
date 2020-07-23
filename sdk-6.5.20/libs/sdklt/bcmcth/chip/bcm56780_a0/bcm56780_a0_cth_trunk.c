/*! \file bcm56780_a0_cth_trunk.c
 *
 * Chip specific functions for BCMCTH TRUNK
 *
 * This file contains the chip specific functions for BCMCTH TRUNK
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
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>

#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/chip/bcm56780_a0_enum.h>
#include <bcmdrd/chip/bcm56780_a0_defs.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmcth/bcmcth_trunk_util.h>
#include <bcmcth/bcmcth_trunk_drv.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmptm/bcmptm.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmcth/bcmcth_trunk_vp_db_internal.h>
#include <bcmcth/bcmcth_trunk_vp_lt_utils.h>
#include <bcmcth/bcmcth_trunk_vp_common_imm.h>

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_TRUNK

#define TRUNK_SYSTEM_MAX_CNT 64

#define TRUNK_MAX_LOGICAL_PORTS 160

#define MEMBER0_MAX_FIELD 4

#define MEMBER_LEVEL0 0

#define MEMBER_LEVEL1 1

static const bcmdrd_sid_t cfg_reg[TRUNK_SYSTEM_MAX_CNT] = {
    IPOST_LAG_SYSTEM_LAG_CONFIG_0r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_1r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_2r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_3r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_4r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_5r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_6r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_7r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_8r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_9r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_10r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_11r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_12r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_13r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_14r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_15r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_16r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_17r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_18r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_19r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_20r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_21r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_22r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_23r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_24r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_25r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_26r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_27r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_28r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_29r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_30r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_31r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_32r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_33r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_34r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_35r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_36r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_37r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_38r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_39r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_40r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_41r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_42r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_43r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_44r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_45r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_46r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_47r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_48r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_49r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_50r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_51r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_52r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_53r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_54r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_55r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_56r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_57r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_58r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_59r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_60r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_61r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_62r,
    IPOST_LAG_SYSTEM_LAG_CONFIG_63r
};

static const uint32_t fbmp_mem0_id[MEMBER0_MAX_FIELD] = {
      BCMCTH_TRUNK_VP_LT_FIELD_MEMBER0_FIELD0,
      BCMCTH_TRUNK_VP_LT_FIELD_MEMBER0_FIELD1,
      BCMCTH_TRUNK_VP_LT_FIELD_MEMBER0_FIELD2,
      BCMCTH_TRUNK_VP_LT_FIELD_MEMBER0_FIELD3,
};

/*******************************************************************************
 * Typedefs
 */


/*******************************************************************************
 * Private variables
 */


/*******************************************************************************
 * Private Functions
 */

static int
bcm56780_a0_trunk_localport_get(int unit,
                                const bcmltd_op_arg_t *op_arg,
                                bcmltd_sid_t lt_id,
                                uint16_t system_port,
                                uint8_t *local_port,
                                bool *local_port_type)
{
    IDEV_CONFIG_TABLEm_t buf;
    uint16_t tmp_system_port;
    uint16_t i = 0;

    SHR_FUNC_ENTER(unit);

    *local_port      = 0;
    *local_port_type = FALSE;

    /* Skipping the CPU port. */
    for (i = (IDEV_CONFIG_TABLEm_MIN + 1);
         i < IDEV_CONFIG_TABLEm_MAX;
         i++) {
        IDEV_CONFIG_TABLEm_CLR(buf);
        SHR_IF_ERR_VERBOSE_EXIT
           (bcmcth_trunk_hw_read(unit,
                                 op_arg,
                                 lt_id,
                                 IDEV_CONFIG_TABLEm,
                                 i,
                                 &buf));
        tmp_system_port = IDEV_CONFIG_TABLEm_SYSTEM_PORTf_GET(buf);
        if (tmp_system_port == system_port) {
            *local_port_type = TRUE;
            *local_port      = i;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_trunk_block_mask_set(int unit,
                                 const bcmltd_op_arg_t *op_arg,
                                 bcmltd_sid_t lt_id,
                                 bcmdrd_pbmp_t old_trunk_bmp,
                                 bcmdrd_pbmp_t new_trunk_bmp,
                                 uint16_t  new_cnt,
                                 uint16_t  *nonuc_system_ports,
                                 bool  *nonuc_egr_block)
{
    bcmdrd_pbmp_t pbmp;
    int  tbl_sz, region_sz, ent_sz, idx;
    uint32_t *dma_buf = NULL, *ent_buf = NULL;
    bcmdrd_sid_t pt_id = IPOST_LAG_NONUCAST_LAG_BLOCK_MASKm;
    SHR_BITDCL *my_modid_bmp = NULL;
    uint8_t    *local_ports = NULL;
    uint8_t local_port;
    bool local_port_type;
    uint32_t nonuc_egr_cnt = 0;

    SHR_FUNC_ENTER(unit);

    if (new_cnt > 0) {
        SHR_ALLOC(my_modid_bmp, SHR_BITALLOCSIZE(new_cnt),
                  "bcmcthTrunkDrdPbmp");
        SHR_NULL_CHECK(my_modid_bmp, SHR_E_MEMORY);
        sal_memset(my_modid_bmp, 0, SHR_BITALLOCSIZE(new_cnt));

        SHR_ALLOC(local_ports, (sizeof(uint8_t) * BCMCTH_TRUNK_MAX_MEMBERS),
                  "bcmcthTrunkDrdLogicalPortsArray");
        SHR_NULL_CHECK(local_ports, SHR_E_MEMORY);
        sal_memset(local_ports, 0,
                  (sizeof(uint8_t) * BCMCTH_TRUNK_MAX_MEMBERS));

        for (idx = 0; idx < new_cnt; idx++) {
            local_port_type = FALSE;
            local_port = 0;
            SHR_IF_ERR_VERBOSE_EXIT(bcm56780_a0_trunk_localport_get(
                            unit,
                            op_arg,
                            lt_id,
                            nonuc_system_ports[idx],
                            &local_port, &local_port_type));
            if (local_port_type == TRUE) {
                if (nonuc_egr_block[idx]) {
                    continue;
                } else {
                    SHR_BITSET(my_modid_bmp, nonuc_egr_cnt);
                    local_ports[nonuc_egr_cnt] = local_port;
                    nonuc_egr_cnt++;
                }
            }
        }
    }


    /*
     * The NONUCAST_TRUNK_BLOCK_MASK table is divided into 4 regions:
     * IPMC, L2MC, broadcast, and unknown unicast/multicast. Same datas
     * are set to 4 regions.
     */
    tbl_sz = bcmdrd_pt_index_max(unit, pt_id)
             - bcmdrd_pt_index_min(unit, pt_id) + 1;
    region_sz = tbl_sz / 4;
    ent_sz = bcmdrd_pt_entry_wsize(unit, pt_id);
    SHR_ALLOC(dma_buf,
              BCMDRD_WORDS2BYTES(ent_sz * tbl_sz),
              "bcmcthTrunkBlockBuf");
    SHR_NULL_CHECK(dma_buf, SHR_E_MEMORY);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_trunk_hw_read_dma(unit,
                                  op_arg,
                                  lt_id,
                                  IPOST_LAG_NONUCAST_LAG_BLOCK_MASKm,
                                  0,
                                  tbl_sz,
                                  dma_buf));

    BCMDRD_PBMP_CLEAR(pbmp);
    for (idx = 0; idx < region_sz; idx++) {
        ent_buf = dma_buf + idx * ent_sz;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmdrd_pt_field_get(unit, pt_id, ent_buf, BITMAPf, pbmp.w));
        BCMDRD_PBMP_REMOVE(pbmp, old_trunk_bmp);
        BCMDRD_PBMP_OR(pbmp, new_trunk_bmp);

        if (nonuc_egr_cnt) {
            if (SHR_BITGET(my_modid_bmp, (idx % nonuc_egr_cnt))) {
                BCMDRD_PBMP_PORT_REMOVE(pbmp, local_ports[idx % nonuc_egr_cnt]);
            }
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmdrd_pt_field_set(unit, pt_id, ent_buf, BITMAPf, pbmp.w));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmdrd_pt_field_set(unit, pt_id, ent_buf + 1 * region_sz * ent_sz,
                                 BITMAPf, pbmp.w));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmdrd_pt_field_set(unit, pt_id, ent_buf + 2 * region_sz * ent_sz,
                                 BITMAPf, pbmp.w));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmdrd_pt_field_set(unit, pt_id, ent_buf + 3 * region_sz * ent_sz,
                                 BITMAPf, pbmp.w));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_trunk_hw_write_dma(unit, op_arg, lt_id, pt_id,
                                   0, tbl_sz, dma_buf));

exit:
    if (my_modid_bmp != NULL) {
        SHR_FREE(my_modid_bmp);
        my_modid_bmp = NULL;
    }
    if (local_ports != NULL) {
        SHR_FREE(local_ports);
        local_ports = NULL;
    }
    if (dma_buf != NULL) {
        SHR_FREE(dma_buf);
        dma_buf = NULL;
    }
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_trunk_grp_add(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          bcmltd_sid_t lt_id,
                          bcmcth_trunk_imm_grp_bk_t *grp_bk,
                          bcmcth_trunk_group_param_t *param,
                          uint32_t base_ptr)
{
    IPOST_LAG_LAG_GROUPm_t  grp_buf;
    IPOST_LAG_LAG_MEMBERm_t mem_buf;
    IPOST_LAG_LAG_BITMAPm_t         bitmap_buf;
    bcmdrd_pbmp_t  old_trunk_bmp, new_trunk_bmp;
    int trunk_id, px;
    bcmcth_trunk_imm_group_t *grp;
    uint8_t local_port;
    bool local_port_type = FALSE;
    uint16_t uc_egr_cnt = 0;

    SHR_FUNC_ENTER(unit);

    trunk_id = param->id;
    grp = grp_bk->grp_ptr->group_array + trunk_id;

    BCMDRD_PBMP_CLEAR(old_trunk_bmp);
    BCMDRD_PBMP_CLEAR(new_trunk_bmp);

    uc_egr_cnt = 0;
    if (param->uc_cnt > 0) {
        /* Write IPOST_LAG_LAG_MEMBERm. */
        for (px = 0; px < param->uc_cnt; px++) {
            if (param->uc_egr_block_valid[px]) {
                if (param->uc_egr_block[px]) {
                    continue;
                }
            }
            IPOST_LAG_LAG_MEMBERm_CLR(mem_buf);
            if (param->uc_system_port_valid[px]) {
                IPOST_LAG_LAG_MEMBERm_SYSTEM_DEST_PORTf_SET(
                                      mem_buf,
                                      param->uc_system_port[px]);
            }
            if (param->l2_oif_valid[px]) {
                IPOST_LAG_LAG_MEMBERm_MEMBER_L2OIFf_SET(
                                      mem_buf,
                                      param->l2_oif[px]);
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_trunk_hw_write(unit,
                                       op_arg,
                                       lt_id,
                                       IPOST_LAG_LAG_MEMBERm,
                                       (base_ptr + uc_egr_cnt),
                                       &mem_buf));
            uc_egr_cnt++;
        }
        LOG_INFO(BSL_LOG_MODULE,
              (BSL_META_U(unit,
               "uc_egr_cnt %d"), uc_egr_cnt));

        /* Write IPOST_LAG_LAG_GROUPm. */
        IPOST_LAG_LAG_GROUPm_CLR(grp_buf);
        IPOST_LAG_LAG_GROUPm_LB_MODEf_SET(grp_buf, param->lb_mode);
        IPOST_LAG_LAG_GROUPm_LAG_SIZEf_SET(grp_buf, (uc_egr_cnt - 1));
        IPOST_LAG_LAG_GROUPm_BASE_PTRf_SET(grp_buf, base_ptr);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_trunk_hw_write(unit,
                                   op_arg, lt_id, IPOST_LAG_LAG_GROUPm,
                                   trunk_id, &grp_buf));

        /* Write IPOST_LAG_LAG_BITMAPm. */
        for (px = 0; px < param->uc_cnt; px++) {
            local_port = 0;
            local_port_type = FALSE;
            if (param->uc_system_port_valid[px]) {
                SHR_IF_ERR_VERBOSE_EXIT(bcm56780_a0_trunk_localport_get(
                            unit,
                            op_arg,
                            lt_id,
                            param->uc_system_port[px],
                            &local_port, &local_port_type));
                if (local_port_type == TRUE) {
                    BCMDRD_PBMP_PORT_ADD(new_trunk_bmp,
                                         local_port);
                }
            }
        }
    }
    for (px = 0; px < param->nonuc_cnt; px++) {
        local_port = 0;
        local_port_type = FALSE;
        if (param->nonuc_system_port_valid[px]) {
             SHR_IF_ERR_VERBOSE_EXIT(
                   bcm56780_a0_trunk_localport_get(
                            unit,
                            op_arg,
                            lt_id,
                            param->nonuc_system_port[px],
                            &local_port, &local_port_type));
            if (local_port_type == TRUE) {
               BCMDRD_PBMP_PORT_ADD(new_trunk_bmp,
                                     local_port);
            }
        }
    }
    IPOST_LAG_LAG_BITMAPm_CLR(bitmap_buf);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_trunk_hw_read(unit, op_arg,
               lt_id, IPOST_LAG_LAG_BITMAPm,
                          trunk_id, &bitmap_buf));
    IPOST_LAG_LAG_BITMAPm_BITMAPf_GET(bitmap_buf, old_trunk_bmp.w);

    IPOST_LAG_LAG_BITMAPm_BITMAPf_SET(bitmap_buf, new_trunk_bmp.w);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_trunk_hw_write(unit, op_arg,
            lt_id, IPOST_LAG_LAG_BITMAPm,
                           trunk_id, &bitmap_buf));

    /* Write IPOST_LAG_NONUCAST_LAG_BLOCK_MASKm. */
    if (BCMDRD_PBMP_NOT_NULL(new_trunk_bmp)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_trunk_block_mask_set(unit,
                                              op_arg,
                                              lt_id,
                                              old_trunk_bmp,
                                              new_trunk_bmp,
                                              param->nonuc_cnt,
                                              param->nonuc_system_port,
                                              param->nonuc_egr_block));
    }

    /* Now we are done with all configs, update base_pointer */
    grp->base_ptr = base_ptr;

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_trunk_grp_del(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          bcmltd_sid_t lt_id,
                          bcmcth_trunk_imm_grp_bk_t *grp_bk,
                          bcmcth_trunk_group_param_t *param)
{
    IPOST_LAG_LAG_GROUPm_t  grp_buf;
    IPOST_LAG_LAG_BITMAPm_t bitmap_buf;
    bcmdrd_pbmp_t   old_trunk_bmp, new_trunk_bmp;
    uint32_t trunk_id;
    bcmcth_trunk_imm_group_t *grp;

    SHR_FUNC_ENTER(unit);

    trunk_id = param->id;
    grp = grp_bk->grp_ptr->group_array + trunk_id;

    /* Group is valid in HW when uc_cnt is bigger than 0. */
    if (grp->uc_cnt > 0) {
        IPOST_LAG_LAG_GROUPm_CLR(grp_buf);
        SHR_IF_ERR_VERBOSE_EXIT
           (bcmcth_trunk_hw_write(unit,
                                  op_arg,
                                  lt_id,
                                  IPOST_LAG_LAG_GROUPm,
                                  trunk_id,
                                  &grp_buf));
    }

    BCMDRD_PBMP_CLEAR(old_trunk_bmp);
    BCMDRD_PBMP_CLEAR(new_trunk_bmp);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_trunk_hw_read(unit,
                              op_arg,
                              lt_id,
                              IPOST_LAG_LAG_BITMAPm,
                              trunk_id,
                              &bitmap_buf));
    IPOST_LAG_LAG_BITMAPm_BITMAPf_GET(bitmap_buf, old_trunk_bmp.w);

    if (BCMDRD_PBMP_NOT_NULL(old_trunk_bmp)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_trunk_block_mask_set(unit,
                                              op_arg,
                                              lt_id,
                                              old_trunk_bmp,
                                              new_trunk_bmp,
                                              0,
                                              NULL,
                                              NULL));
    }

    IPOST_LAG_LAG_BITMAPm_CLR(bitmap_buf);
    SHR_IF_ERR_VERBOSE_EXIT
     (bcmcth_trunk_hw_write(unit,
                            op_arg,
                            lt_id,
                            IPOST_LAG_LAG_BITMAPm,
                            trunk_id,
                            &bitmap_buf));

    grp->base_ptr = 0;
    grp->uc_max_members = 0;

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_trunk_grp_move(int unit,
                           const bcmltd_op_arg_t *op_arg,
                           bcmltd_sid_t lt_id,
                           bcmcth_trunk_imm_grp_bk_t *grp_bk,
                           bcmcth_trunk_grp_defrag_t *src,
                           bcmcth_trunk_grp_defrag_t *dest)
{
    IPOST_LAG_LAG_GROUPm_t   grp_buf;
    IPOST_LAG_LAG_MEMBERm_t  mem_buf;
    uint32_t trunk_id, px;
    uint32_t old_cnt, old_base;
    uint32_t new_base;
    bcmcth_trunk_imm_group_t *grp;

    SHR_FUNC_ENTER(unit);

    trunk_id = src->trunk_id;
    old_cnt = src->group_size;
    old_base = src->group_base;
    new_base = dest->group_base;

    grp = grp_bk->grp_ptr->group_array + trunk_id;

    /* Move member block to new base. */
    for (px = 0; px < old_cnt; px++) {
       IPOST_LAG_LAG_MEMBERm_CLR(mem_buf);
       SHR_IF_ERR_EXIT(bcmcth_trunk_hw_read(unit,
                                            op_arg,
                                            lt_id,
                                            IPOST_LAG_LAG_MEMBERm,
                                            (old_base + px),
                                            &mem_buf));
        SHR_IF_ERR_EXIT
            (bcmcth_trunk_hw_write(unit,
                                   op_arg,
                                   lt_id,
                                   IPOST_LAG_LAG_MEMBERm,
                                   (new_base + px),
                                   &mem_buf));
    }

    /* Update IPOST_LAG_LAG_GROUPm table pointer. */
    SHR_IF_ERR_EXIT
        (bcmcth_trunk_hw_read(unit,
                              op_arg,
                              lt_id,
                              IPOST_LAG_LAG_GROUPm,
                              trunk_id,
                              &grp_buf));
    IPOST_LAG_LAG_GROUPm_BASE_PTRf_SET(grp_buf, new_base);

    SHR_IF_ERR_EXIT
        (bcmcth_trunk_hw_write(unit,
                               op_arg,
                               lt_id,
                               IPOST_LAG_LAG_GROUPm,
                               trunk_id,
                               &grp_buf));
    grp->base_ptr = new_base;

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_trunk_grp_update(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             bcmltd_sid_t lt_id,
                             bcmcth_trunk_imm_grp_bk_t *grp_bk,
                             bcmcth_trunk_group_param_t *param,
                             uint32_t new_base)
{
    IPOST_LAG_LAG_GROUPm_t   grp_buf;
    IPOST_LAG_LAG_MEMBERm_t  mem_buf;
    IPOST_LAG_LAG_BITMAPm_t         bitmap_buf;
    bcmdrd_pbmp_t           old_trunk_bmp, new_trunk_bmp;
    uint32_t trunk_id, px;
    uint32_t old_max, old_cnt, old_base;
    uint32_t new_max;
    uint16_t uc_port = 0;
    uint32_t min_cnt;
    uint8_t local_port;
    bcmcth_trunk_imm_group_t *grp;
    bool local_port_type = FALSE;
    uint16_t uc_egr_cnt = 0;

    SHR_FUNC_ENTER(unit);

    trunk_id = param->id;
    grp = grp_bk->grp_ptr->group_array + trunk_id;

    old_cnt  = grp->uc_cnt;
    old_base = grp->base_ptr;
    old_max  = grp->uc_max_members;

    if (param->uc_max_members_valid) {
        new_max  = param->uc_max_members;
    } else {
        new_max = grp->uc_max_members;
    }

    /* First, need to move member_entry if max_member_size is updated. */
    if (new_max != old_max) {
        if (new_base != old_base) {
            for (px = 0; px < old_cnt; px++) {
                if (param->uc_egr_block_valid[px]) {
                    if (param->uc_egr_block[px]) {
                        continue;
                    }
                }
                IPOST_LAG_LAG_MEMBERm_CLR(mem_buf);
                IPOST_LAG_LAG_MEMBERm_SYSTEM_DEST_PORTf_SET(
                                          mem_buf,
                                          grp->uc_system_port[px]);
                IPOST_LAG_LAG_MEMBERm_MEMBER_L2OIFf_SET(
                                          mem_buf,
                                          grp->l2_eif[px]);
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmcth_trunk_hw_write(unit,
                                           op_arg,
                                           lt_id,
                                           IPOST_LAG_LAG_MEMBERm,
                                           (new_base + uc_egr_cnt),
                                           &mem_buf));
                 uc_egr_cnt++;
            }
        }
    }

    uc_egr_cnt = 0;
    /*
     * param->uc_cnt is always valid in update operation
     * either the value is new supplied in update operation
     * or it is copied from the grp_bk information.
     * if 0 means in the recent operation it has been set
     * explicitly to delete the trunk group information.
     */
    if (param->uc_cnt) {
        /* Update IPOST_LAG_LAG_MEMBERm. */
        if (old_cnt < param->uc_cnt) {
            min_cnt = old_cnt;
        } else {
            min_cnt = param->uc_cnt;
        }
        for (px = 0; px < min_cnt; px++) {

            IPOST_LAG_LAG_MEMBERm_CLR(mem_buf);
            /*
             * Clear the member information for the
             * UC_EGRESS_DISABLE.
             */
            SHR_IF_ERR_VERBOSE_EXIT
               (bcmcth_trunk_hw_write(unit,
                                      op_arg,
                                      lt_id,
                                      IPOST_LAG_LAG_MEMBERm,
                                      (new_base + px),
                                      &mem_buf));
            if (param->uc_egr_block_valid[px]) {
                if (param->uc_egr_block[px]) {
                    continue;
                }
            }
            if (param->uc_system_port_valid[px]) {
                IPOST_LAG_LAG_MEMBERm_SYSTEM_DEST_PORTf_SET(
                   mem_buf,
                   param->uc_system_port[px]);
            }

            if (param->l2_oif_valid[px]) {
                IPOST_LAG_LAG_MEMBERm_MEMBER_L2OIFf_SET(
                   mem_buf,
                   param->l2_oif[px]);
            }
            SHR_IF_ERR_VERBOSE_EXIT
               (bcmcth_trunk_hw_write(unit,
                                      op_arg,
                                      lt_id,
                                      IPOST_LAG_LAG_MEMBERm,
                                      (new_base + uc_egr_cnt),
                                      &mem_buf));
            uc_egr_cnt++;
        }

        for (px = min_cnt; px < param->uc_cnt; px++) {
            if (param->uc_egr_block_valid[px]) {
                if (param->uc_egr_block[px]) {
                    continue;
                }
            }
            IPOST_LAG_LAG_MEMBERm_CLR(mem_buf);
            if (param->uc_system_port_valid[px]) {
                IPOST_LAG_LAG_MEMBERm_SYSTEM_DEST_PORTf_SET(
                           mem_buf,
                           param->uc_system_port[px]);
            }
            if (param->l2_oif_valid[px]) {
               IPOST_LAG_LAG_MEMBERm_MEMBER_L2OIFf_SET(
                           mem_buf,
                           param->l2_oif[px]);
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_trunk_hw_write(unit,
                                       op_arg,
                                       lt_id,
                                       IPOST_LAG_LAG_MEMBERm,
                                       (new_base + uc_egr_cnt),
                                       &mem_buf));
            uc_egr_cnt++;
        }
        LOG_INFO(BSL_LOG_MODULE,
              (BSL_META_U(unit,
               "uc_egr_cnt %d"), uc_egr_cnt));

        /* Update IPOST_LAG_LAG_GROUPm. */
        IPOST_LAG_LAG_GROUPm_CLR(grp_buf);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_trunk_hw_read(unit,
                                  op_arg,
                                  lt_id,
                                  IPOST_LAG_LAG_GROUPm,
                                  trunk_id,
                                  &grp_buf));

        if (param->lb_mode_valid) {
            IPOST_LAG_LAG_GROUPm_LB_MODEf_SET(grp_buf, param->lb_mode);
        }

        IPOST_LAG_LAG_GROUPm_BASE_PTRf_SET(grp_buf, new_base);
        /*
         * All the unicast member ports are disabled.
         * Member resources are not released by itbm.
         * but the trunk group is programmed with
         * LAG_SIZE 0. since the re enable of the
         * unicast egress ports for unicast traffic forwarding
         * by resetting the uc_egr_block[x]=0
         * should program the trunk group
         * with LAG_SIZE equal to non zero uc_egr_cnt.
         */
        if (uc_egr_cnt) {
            IPOST_LAG_LAG_GROUPm_LAG_SIZEf_SET(grp_buf, (uc_egr_cnt - 1));
        } else {
            IPOST_LAG_LAG_GROUPm_LAG_SIZEf_SET(grp_buf, 0);
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_trunk_hw_write(unit,
                                   op_arg,
                                   lt_id,
                                   IPOST_LAG_LAG_GROUPm,
                                   trunk_id,
                                   &grp_buf));

    } else {
        /*
         * Need to delete HW info for uc member
         * when uc_cnt is set 0 in update operation.
         */
        IPOST_LAG_LAG_GROUPm_CLR(grp_buf);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_trunk_hw_write(unit,
                                   op_arg,
                                   lt_id,
                                   IPOST_LAG_LAG_GROUPm,
                                   trunk_id,
                                   &grp_buf));
    }

    /* Update IPOST_LAG_LAG_BITMAPm. */
    BCMDRD_PBMP_CLEAR(new_trunk_bmp);
    BCMDRD_PBMP_CLEAR(old_trunk_bmp);

    IPOST_LAG_LAG_BITMAPm_CLR(bitmap_buf);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_trunk_hw_read(unit,
                              op_arg,
                              lt_id,
                              IPOST_LAG_LAG_BITMAPm,
                              trunk_id,
                              &bitmap_buf));
    IPOST_LAG_LAG_BITMAPm_BITMAPf_GET(bitmap_buf, old_trunk_bmp.w);
    for (px = 0; px < param->uc_cnt; px++) {
        local_port = 0;
        local_port_type = FALSE;
        if (param->uc_system_port_valid[px]) {
            uc_port = param->uc_system_port[px];
        }
        if (uc_port) {
            SHR_IF_ERR_VERBOSE_EXIT(bcm56780_a0_trunk_localport_get(
                        unit,
                        op_arg,
                        lt_id,
                        uc_port,
                        &local_port,
                        &local_port_type));
            if (local_port_type == TRUE) {
                BCMDRD_PBMP_PORT_ADD(new_trunk_bmp, local_port);
            }
        }
    }
    for (px = 0; px < param->nonuc_cnt; px++) {
        local_port = 0;
        local_port_type = FALSE;
        if (param->nonuc_system_port_valid[px]) {
            SHR_IF_ERR_VERBOSE_EXIT(bcm56780_a0_trunk_localport_get(
                        unit,
                        op_arg,
                        lt_id,
                        param->nonuc_system_port[px],
                        &local_port,
                        &local_port_type));
            if (local_port_type == TRUE) {
                BCMDRD_PBMP_PORT_ADD(new_trunk_bmp, local_port);
            }
        }
    }
    IPOST_LAG_LAG_BITMAPm_BITMAPf_SET(bitmap_buf, new_trunk_bmp.w);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_trunk_hw_write(unit,
                               op_arg,
                               lt_id,
                               IPOST_LAG_LAG_BITMAPm,
                               trunk_id,
                               &bitmap_buf));


    /* Write IPOST_LAG_NONUCAST_LAG_BLOCK_MASKm. */
    if (BCMDRD_PBMP_NOT_NULL(old_trunk_bmp) ||
        BCMDRD_PBMP_NOT_NULL(new_trunk_bmp)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_trunk_block_mask_set(unit,
                                              op_arg,
                                              lt_id,
                                              old_trunk_bmp,
                                              new_trunk_bmp,
                                              param->nonuc_cnt,
                                              param->nonuc_system_port,
                                              param->nonuc_egr_block));
    }

    /* Now we are done with all configs, update base_pointer */
    grp->base_ptr = new_base;
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_trunk_grp_init(int unit,
                           bcmcth_trunk_imm_grp_bk_t *grp_bk)
{
    int max, min, rv = SHR_E_NONE;
    shr_itbm_list_params_t params;
    shr_itbm_list_hdl_t lag_member_itbm_hdl = NULL;

    SHR_FUNC_ENTER(unit);

    /* Max group count. */
    max = bcmdrd_pt_index_max(unit, IPOST_LAG_LAG_GROUPm);
    if (max < 0) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }
    min = bcmdrd_pt_index_min(unit, IPOST_LAG_LAG_GROUPm);
    grp_bk->max_group_cnt = max - min + 1;

    /* Max member count. */
    max = bcmdrd_pt_index_max(unit, IPOST_LAG_LAG_MEMBERm);
    if (max < 0) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    /* Trunk member group table entry 0 is reserved. */
    min = bcmdrd_pt_index_min(unit, IPOST_LAG_LAG_MEMBERm) + 1;
    grp_bk->max_member_cnt = max - min + 1;

    shr_itbm_list_params_t_init(&params);
    params.unit = unit;
    params.table_id = IPOST_LAG_LAG_MEMBERm;
    params.min = min;
    params.max = max;
    params.first = min;
    params.last  = max;
    params.comp_id = BCMMGMT_TRUNK_COMP_ID;
    params.block_stat = true;

    rv = shr_itbm_list_create(params, "bcmcthTrunkMemberList", &lag_member_itbm_hdl);
    if ((rv== SHR_E_NONE) || (rv == SHR_E_EXISTS)) {
        grp_bk->itbm_hdl     = lag_member_itbm_hdl;
        grp_bk->itbm_support = TRUE;
        LOG_VERBOSE(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                          "ITBM group list creation rv=%d list %p.\n"),
                          rv,
                          ((void *)lag_member_itbm_hdl)));
    } else {
        grp_bk->itbm_support = FALSE;
        LOG_ERROR(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                          "ITBM group list creation failed rv=%d\n"), rv));
    }

exit:
    if ((rv != SHR_E_NONE) && (rv != SHR_E_EXISTS)) {
        if (lag_member_itbm_hdl) {
            shr_itbm_list_destroy(lag_member_itbm_hdl);
        }
    }

    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_sys_trunk_grp_add(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              bcmltd_sid_t lt_id,
                              bcmcth_trunk_sys_grp_bk_t *grp_bk,
                              bcmcth_trunk_sys_grp_param_t *param,
                              uint32_t base_ptr)
{
    IPOST_LAG_SYSTEM_LAG_GROUP_0m_t  grp_buf;
    IPOST_LAG_SYSTEM_LAG_MEMBER_0m_t mem_buf;
    IPOST_LAG_SYSTEM_LAG_BITMAP_0m_t         bitmap_buf;
    bcmdrd_pbmp_t  new_trunk_bmp;
    int trunk_id, px;
    bcmcth_trunk_imm_sys_group_t *grp;
    IPOST_LAG_SYSTEM_LAG_CONFIG_0r_t  cfg_buf;

    SHR_FUNC_ENTER(unit);

    trunk_id = param->id;
    grp = grp_bk->grp_ptr->sys_grp_array + trunk_id;
    grp->base_ptr = base_ptr;

    BCMDRD_PBMP_CLEAR(new_trunk_bmp);

    if (param->cnt > 0) {
        /* Write IPOST_LAG_SYSTEM_LAG_MEMBER_0m. */
        for (px = 0; px < param->cnt; px++) {
            IPOST_LAG_SYSTEM_LAG_MEMBER_0m_CLR(mem_buf);
            if (param->port_valid[px]) {
                IPOST_LAG_SYSTEM_LAG_MEMBER_0m_LOCAL_DESTINATION_PORTf_SET(
                                      mem_buf,
                                      param->port[px]);

                SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_trunk_hw_write(unit,
                                       op_arg,
                                       lt_id,
                                       IPOST_LAG_SYSTEM_LAG_MEMBER_0m,
                                       (base_ptr + px),
                                       &mem_buf));
            }
        }

        /* Write IPOST_LAG_SYSTEM_LAG_GROUP_0m. */
        IPOST_LAG_SYSTEM_LAG_GROUP_0m_CLR(grp_buf);
        IPOST_LAG_SYSTEM_LAG_GROUP_0m_LB_MODEf_SET(grp_buf
                                 , param->lb_mode);
        IPOST_LAG_SYSTEM_LAG_GROUP_0m_SYSTEM_LAG_SIZEf_SET(grp_buf
                                 ,(param->cnt - 1));
        IPOST_LAG_SYSTEM_LAG_GROUP_0m_BASE_PTRf_SET(grp_buf
                                 , base_ptr);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_trunk_hw_write(unit,
                                   op_arg,
                                   lt_id,
                                   IPOST_LAG_SYSTEM_LAG_GROUP_0m,
                                   trunk_id,
                                   &grp_buf));

        /* Write IPOST_LAG_SYSTEM_LAG_BITMAP_0m. */
        for (px = 0; px < param->cnt; px++) {
            if (param->port_valid[px]) {
                BCMDRD_PBMP_PORT_ADD(new_trunk_bmp,
                                     param->port[px]);
            }
        }
    }

    IPOST_LAG_SYSTEM_LAG_BITMAP_0m_CLR(bitmap_buf);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_trunk_hw_read(unit, op_arg,
                          lt_id, IPOST_LAG_SYSTEM_LAG_BITMAP_0m,
                          trunk_id, &bitmap_buf));

    IPOST_LAG_SYSTEM_LAG_BITMAP_0m_BITMAPf_SET(bitmap_buf, new_trunk_bmp.w);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_trunk_hw_write(unit, op_arg,
                           lt_id, IPOST_LAG_SYSTEM_LAG_BITMAP_0m,
                           trunk_id, &bitmap_buf));

    /* Disable the system resolution. */
    if (param->disable_valid) {
        IPOST_LAG_SYSTEM_LAG_CONFIG_0r_CLR(cfg_buf);
        IPOST_LAG_SYSTEM_LAG_CONFIG_0r_SYSTEM_LAG_RES_DISABLEf_SET(
                            cfg_buf,
                            param->disable);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_trunk_hw_write(unit,
                                   op_arg,
                                   lt_id,
                                   cfg_reg[trunk_id],
                                   0,
                                   &cfg_buf));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_sys_trunk_grp_del(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              bcmltd_sid_t lt_id,
                              bcmcth_trunk_sys_grp_bk_t *grp_bk,
                              bcmcth_trunk_sys_grp_param_t *param)
{
    IPOST_LAG_SYSTEM_LAG_GROUP_0m_t  grp_buf;
    IPOST_LAG_SYSTEM_LAG_BITMAP_0m_t bitmap_buf;
    IPOST_LAG_SYSTEM_LAG_CONFIG_0r_t cfg_buf;
    bcmdrd_pbmp_t   old_trunk_bmp, new_trunk_bmp;
    int trunk_id;
    bcmcth_trunk_imm_sys_group_t *grp;

    SHR_FUNC_ENTER(unit);

    trunk_id = param->id;
    grp = grp_bk->grp_ptr->sys_grp_array + trunk_id;

    /* Group is valid in HW when cnt is bigger than 0. */
    if (grp->cnt > 0) {
     IPOST_LAG_SYSTEM_LAG_GROUP_0m_CLR(grp_buf);
     SHR_IF_ERR_VERBOSE_EXIT
         (bcmcth_trunk_hw_write(unit,
                                op_arg,
                                lt_id,
                                IPOST_LAG_SYSTEM_LAG_GROUP_0m,
                                trunk_id, &grp_buf));

    }

    BCMDRD_PBMP_CLEAR(old_trunk_bmp);
    BCMDRD_PBMP_CLEAR(new_trunk_bmp);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_trunk_hw_read(unit,
                              op_arg,
                              lt_id,
                              IPOST_LAG_SYSTEM_LAG_BITMAP_0m,
                              trunk_id,
                              &bitmap_buf));
    IPOST_LAG_SYSTEM_LAG_BITMAP_0m_BITMAPf_GET(bitmap_buf, old_trunk_bmp.w);

    IPOST_LAG_SYSTEM_LAG_BITMAP_0m_CLR(bitmap_buf);
    SHR_IF_ERR_VERBOSE_EXIT
     (bcmcth_trunk_hw_write(unit, op_arg,
                    lt_id, IPOST_LAG_SYSTEM_LAG_BITMAP_0m,
                        trunk_id, &bitmap_buf));


    bcmcth_trunk_member_free(unit, grp_bk->mbmp_ptr->mbmp,
                         grp->base_ptr, grp->max_members);

    /* Disable the system resolution. */
    IPOST_LAG_SYSTEM_LAG_CONFIG_0r_CLR(cfg_buf);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_trunk_hw_write(unit,
                               op_arg,
                               lt_id,
                               cfg_reg[trunk_id],
                               0,
                               &cfg_buf));

    grp->base_ptr = 0;
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_sys_trunk_grp_move(int unit,
                               const bcmltd_op_arg_t *op_arg,
                               bcmltd_sid_t lt_id,
                               bcmcth_trunk_sys_grp_bk_t *grp_bk,
                               bcmcth_trunk_grp_defrag_t *src,
                               bcmcth_trunk_grp_defrag_t *dest)
{
    IPOST_LAG_SYSTEM_LAG_GROUP_0m_t   grp_buf;
    IPOST_LAG_SYSTEM_LAG_MEMBER_0m_t  mem_buf;
    uint32_t trunk_id, px;
    uint32_t old_cnt, old_base;
    uint32_t new_base;
    bcmcth_trunk_imm_sys_group_t *grp;

    SHR_FUNC_ENTER(unit);

    trunk_id = src->trunk_id;
    old_cnt = src->group_size;
    old_base = src->group_base;
    new_base = dest->group_base;

    grp = grp_bk->grp_ptr->sys_grp_array + trunk_id;

    /* Move member block to new base. */
    for (px = 0; px < old_cnt; px++) {
       IPOST_LAG_SYSTEM_LAG_MEMBER_0m_CLR(mem_buf);
       SHR_IF_ERR_EXIT(bcmcth_trunk_hw_read(unit,
                                            op_arg,
                                            lt_id,
                                            IPOST_LAG_SYSTEM_LAG_MEMBER_0m,
                                            (old_base + px),
                                            &mem_buf));
        SHR_IF_ERR_EXIT
            (bcmcth_trunk_hw_write(unit,
                                   op_arg,
                                   lt_id,
                                   IPOST_LAG_SYSTEM_LAG_MEMBER_0m,
                                   (new_base + px),
                                   &mem_buf));
    }

    /* Update IPOST_LAG_SYSTEM_LAG_GROUPm table pointer. */
    SHR_IF_ERR_EXIT
        (bcmcth_trunk_hw_read(unit,
                              op_arg,
                              lt_id,
                              IPOST_LAG_SYSTEM_LAG_GROUP_0m,
                              trunk_id,
                              &grp_buf));
    IPOST_LAG_SYSTEM_LAG_GROUP_0m_BASE_PTRf_SET(grp_buf, new_base);

    SHR_IF_ERR_EXIT
        (bcmcth_trunk_hw_write(unit,
                               op_arg,
                               lt_id,
                               IPOST_LAG_SYSTEM_LAG_GROUP_0m,
                               trunk_id,
                               &grp_buf));
    grp->base_ptr = new_base;

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_sys_trunk_grp_update(int unit,
                                 const bcmltd_op_arg_t *op_arg,
                                 bcmltd_sid_t lt_id,
                                 bcmcth_trunk_sys_grp_bk_t *grp_bk,
                                 bcmcth_trunk_sys_grp_param_t *param,
                                 uint32_t new_base)
{
    IPOST_LAG_SYSTEM_LAG_GROUP_0m_t   grp_buf;
    IPOST_LAG_SYSTEM_LAG_MEMBER_0m_t  mem_buf;
    IPOST_LAG_SYSTEM_LAG_BITMAP_0m_t bitmap_buf;
    IPOST_LAG_SYSTEM_LAG_CONFIG_0r_t cfg_buf;
    bcmdrd_pbmp_t clr_trunk_bmp, new_trunk_bmp;
    bool     port_chg[BCMCTH_TRUNK_SYSTEM_MAX_MEMBERS] = {FALSE};
    uint8_t port_old[BCMCTH_TRUNK_SYSTEM_MAX_MEMBERS] = {0};
    uint32_t trunk_id, px;
    uint32_t old_max, old_cnt, old_base;
    uint32_t new_max;
    uint16_t old_port;
    uint32_t min_cnt;
    bcmcth_trunk_imm_sys_group_t *grp;

    SHR_FUNC_ENTER(unit);

    trunk_id = param->id;
    grp = grp_bk->grp_ptr->sys_grp_array + trunk_id;

    /* Read from the hw */
    old_base = grp->base_ptr;
    old_cnt =  grp->cnt;
    old_max  = grp->max_members;

    if (param->max_members_valid) {
        new_max  = param->max_members;
    } else {
        new_max  = grp->max_members;
    }

    /* First, need to move member_entry if max_member_size is updated. */
    if (new_max != old_max) {
        if (new_base != old_base) {
            for (px = 0; px < old_cnt; px++) {
               IPOST_LAG_SYSTEM_LAG_MEMBER_0m_CLR(mem_buf);
               SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_trunk_hw_read(unit, op_arg,
                        lt_id, IPOST_LAG_SYSTEM_LAG_MEMBER_0m,
                          (old_base + px), &mem_buf));
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmcth_trunk_hw_write(unit,
                                           op_arg,
                                           lt_id,
                                           IPOST_LAG_SYSTEM_LAG_MEMBER_0m,
                                           (new_base + px),
                                           &mem_buf));
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_trunk_member_free(unit,
                                          grp_bk->mbmp_ptr->mbmp,
                                          old_base,
                                          old_max));
        }
    }

    if (param->cnt > 0) {
        /* Update IPOST_LAG_SYSTEM_LAG_MEMBER_0m. */
        if (old_cnt < param->cnt) {
            min_cnt = old_cnt;
        } else {
            min_cnt = param->cnt;
        }
        for (px = 0; px < min_cnt; px++) {
           IPOST_LAG_SYSTEM_LAG_MEMBER_0m_CLR(mem_buf);
           SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_trunk_hw_read(unit,
                                      op_arg,
                                      lt_id,
                                      IPOST_LAG_SYSTEM_LAG_MEMBER_0m,
                                      (new_base + px),
                                      &mem_buf));

            old_port =
            IPOST_LAG_SYSTEM_LAG_MEMBER_0m_LOCAL_DESTINATION_PORTf_GET(
                               mem_buf);

            if (param->port_valid[px] && (param->port[px] != old_port)) {
                port_chg[px] = TRUE;
                port_old[px] = old_port;
                IPOST_LAG_SYSTEM_LAG_MEMBER_0m_LOCAL_DESTINATION_PORTf_SET(
                   mem_buf,param->port[px]);
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmcth_trunk_hw_write(unit,
                                           op_arg,
                                           lt_id,
                                           IPOST_LAG_SYSTEM_LAG_MEMBER_0m,
                                           (new_base + px),
                                           &mem_buf));
            }
        }

        for (px = min_cnt; px < param->cnt; px++) {
            if (param->port_valid[px]) {
                IPOST_LAG_SYSTEM_LAG_MEMBER_0m_CLR(mem_buf);
                IPOST_LAG_SYSTEM_LAG_MEMBER_0m_LOCAL_DESTINATION_PORTf_SET(
                     mem_buf, param->port[px]);
                SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_trunk_hw_write(unit,
                                       op_arg,
                                       lt_id,
                                       IPOST_LAG_SYSTEM_LAG_MEMBER_0m,
                                       (new_base + px),
                                       &mem_buf));
           }
        }

        /* Update IPOST_LAG_SYSTEM_LAG_GROUP_0m. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_trunk_hw_read(unit,
                                  op_arg,
                                  lt_id,
                                  IPOST_LAG_SYSTEM_LAG_GROUP_0m,
                                  trunk_id,
                                  &grp_buf));
        IPOST_LAG_SYSTEM_LAG_GROUP_0m_BASE_PTRf_SET(grp_buf,
                      new_base);
        IPOST_LAG_SYSTEM_LAG_GROUP_0m_LB_MODEf_SET(grp_buf,
                      param->lb_mode);
        IPOST_LAG_SYSTEM_LAG_GROUP_0m_SYSTEM_LAG_SIZEf_SET(grp_buf,
                     (param->cnt - 1));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_trunk_hw_write(unit,
                                   op_arg,
                                   lt_id,
                                   IPOST_LAG_SYSTEM_LAG_GROUP_0m,
                                   trunk_id,
                                   &grp_buf));
    } else {
        /* Need to delete HW info for uc member. */
        IPOST_LAG_SYSTEM_LAG_GROUP_0m_CLR(grp_buf);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_trunk_hw_write(unit,
                                   op_arg,
                                   lt_id,
                                   IPOST_LAG_SYSTEM_LAG_GROUP_0m,
                                   trunk_id,
                                   &grp_buf));
    }

    /* Update IPOST_LAG_SYSTEM_LAG_BITMAP_0m. */
    BCMDRD_PBMP_CLEAR(new_trunk_bmp);
    BCMDRD_PBMP_CLEAR(clr_trunk_bmp);
    IPOST_LAG_SYSTEM_LAG_BITMAP_0m_CLR(bitmap_buf);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_trunk_hw_read(unit,
                              op_arg,
                              lt_id,
                              IPOST_LAG_SYSTEM_LAG_BITMAP_0m,
                              trunk_id,
                              &bitmap_buf));
    IPOST_LAG_SYSTEM_LAG_BITMAP_0m_BITMAPf_GET(bitmap_buf, new_trunk_bmp.w);

    for (px = 0; px < param->cnt; px++) {
        if (param->port_valid[px]) {
            if (param->port[px]) {
                if (port_chg[px] == TRUE) {
                    BCMDRD_PBMP_PORT_REMOVE(new_trunk_bmp,
                                            port_old[px]);
                }
                BCMDRD_PBMP_PORT_ADD(new_trunk_bmp,
                                     param->port[px]);
            } else {
                if (port_chg[px] == TRUE) {
                    BCMDRD_PBMP_PORT_REMOVE(new_trunk_bmp,
                                     port_old[px]);
                }
            }
        }
    }
    if (param->cnt_valid) {
        if (param->cnt) {
            IPOST_LAG_SYSTEM_LAG_BITMAP_0m_BITMAPf_SET(bitmap_buf,
                                                       new_trunk_bmp.w);
        } else {
            IPOST_LAG_SYSTEM_LAG_BITMAP_0m_BITMAPf_SET(bitmap_buf,
                                                       clr_trunk_bmp.w);
        }
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_trunk_hw_write(unit,
                               op_arg,
                               lt_id,
                               IPOST_LAG_SYSTEM_LAG_BITMAP_0m,
                               trunk_id,
                               &bitmap_buf));

   /* Disable the system resolution. */
   if (param->disable_valid) {
        IPOST_LAG_SYSTEM_LAG_CONFIG_0r_CLR(cfg_buf);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_trunk_hw_read(unit,
                                  op_arg,
                                  lt_id,
                                  cfg_reg[trunk_id],
                                  0,
                                  &cfg_buf));

        if (IPOST_LAG_SYSTEM_LAG_CONFIG_0r_SYSTEM_LAG_RES_DISABLEf_GET(
                  cfg_buf) != param->disable) {
            IPOST_LAG_SYSTEM_LAG_CONFIG_0r_SYSTEM_LAG_RES_DISABLEf_SET(
                            cfg_buf,
                            param->disable);
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_trunk_hw_write(unit,
                                       op_arg,
                                       lt_id,
                                       cfg_reg[trunk_id],
                                       0,
                                       &cfg_buf));
        }
    }

    /* Done with all configs, update group base pointer */
    grp->base_ptr = new_base;
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_sys_trunk_grp_init(int unit, bcmcth_trunk_sys_grp_bk_t *grp_bk)
{
    int max, min, rv = SHR_E_NONE;
    shr_itbm_list_params_t params;
    shr_itbm_list_hdl_t lag_member_itbm_hdl = NULL;
    int max_bit_base_ptr, min_bit_base_ptr;

    SHR_FUNC_ENTER(unit);

     /* Max group count. */
    max = bcmdrd_pt_index_max(unit, IPOST_LAG_SYSTEM_LAG_GROUP_0m);
    if (max < 0) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }
    min = bcmdrd_pt_index_min(unit, IPOST_LAG_SYSTEM_LAG_GROUP_0m);
    grp_bk->max_group_cnt = max - min + 1;

    /* Min bit for base pointer in group to derive member count. */
    min_bit_base_ptr = bcmdrd_pt_field_minbit (
                       unit,
                       IPOST_LAG_SYSTEM_LAG_GROUP_0m,
                       BASE_PTRf);
    /* Max bit for base pointer in group to derive member count. */
    max_bit_base_ptr = bcmdrd_pt_field_maxbit (
                       unit,
                       IPOST_LAG_SYSTEM_LAG_GROUP_0m,
                       BASE_PTRf);
    if (max_bit_base_ptr < 0) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    /* System trunk member group table entry 0 is reserved. */
    min = 1;
    grp_bk->max_member_cnt =
       (1 << (max_bit_base_ptr - min_bit_base_ptr + 1));

    /* Max index that can be allocated in member table. */
    max = grp_bk->max_member_cnt - 1;

    shr_itbm_list_params_t_init(&params);
    params.unit = unit;
    params.table_id = IPOST_LAG_SYSTEM_LAG_MEMBER_0m;
    params.min = min;
    params.max = max;
    params.first = min;
    params.last  = max;
    params.comp_id = BCMMGMT_TRUNK_COMP_ID;
    params.block_stat = true;

    rv = shr_itbm_list_create(params, "bcmcthSystemTrunkMemberList", &lag_member_itbm_hdl);
    if ((rv== SHR_E_NONE) || (rv == SHR_E_EXISTS)) {
        grp_bk->itbm_hdl     = lag_member_itbm_hdl;
        grp_bk->itbm_support = TRUE;
        LOG_VERBOSE(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                          "ITBM system trunk group list creation rv=%d list %p.\n"),
                          rv,
                          ((void *)lag_member_itbm_hdl)));
    } else {
        grp_bk->itbm_support = FALSE;
        LOG_ERROR(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                          "ITBM system trunk group list creation failed rv=%d\n"), rv));
        SHR_ERR_EXIT(rv);
    }

exit:

    if ((rv != SHR_E_NONE) && (rv != SHR_E_EXISTS)) {
        if (lag_member_itbm_hdl) {
            shr_itbm_list_destroy(lag_member_itbm_hdl);
        }
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Initalize a bcmcth_trunk_vp_pt_op_info_t structure memory.
 *
 * \param [in] pt_op_info Pointer to bcmcth_trunk_vp_pt_op_info_t structure.
 *
 * \return Nothing.
 */
static void
bcm56780_a0_pt_op_info_t_init(bcmcth_trunk_vp_pt_op_info_t *pt_op_info)
{
    /* Initialize structure member variables. */
    if (pt_op_info) {
        pt_op_info->drd_sid = INVALIDm;
        pt_op_info->req_lt_sid = BCMLTD_SID_INVALID;
        pt_op_info->rsp_lt_sid = BCMLTD_SID_INVALID;
        pt_op_info->req_flags = BCMPTM_REQ_FLAGS_NO_FLAGS;
        pt_op_info->rsp_flags = BCMPTM_REQ_FLAGS_NO_FLAGS;
        pt_op_info->op = BCMPTM_OP_NOP;
        pt_op_info->dyn_info.index = BCMCTH_TRUNK_VP_INVALID;
        pt_op_info->dyn_info.tbl_inst = BCMCTH_TRUNK_VP_ALL_PIPES_INST;
        pt_op_info->wsize = 0;
        pt_op_info->dma = FALSE;
        pt_op_info->ecount = 0;
        pt_op_info->op_arg = NULL;
    }

    return;
}

static int
bcm56780_a0_trunk_vp_get_hw_lb_mode(int unit, bcmcth_trunk_vp_lb_mode_t lb_mode,
                                uint32_t *hw_lb_mode) {
    SHR_FUNC_ENTER(unit);
    switch (lb_mode) {
        case BCMCTH_TRUNK_VP_LB_MODE_REGULAR:
            *hw_lb_mode = 0;
            break;
        case BCMCTH_TRUNK_VP_LB_MODE_RANDOM:
            *hw_lb_mode = 1;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Write an entry or entries to a table given.
 *
 * Write an entry or entries to a table given index or start index and number
 * of entries to write.
 *
 * \param [in] unit Unit number.
 * \param [in] op_info Pointer to bcmcth_trunk_vp_pt_op_info_t structure.
 * \param [in] buf Pointer to hardware table entry write data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Input parameter error.
 * \retval SHR_E_FAIL Hardware write operation unsuccessful.
 * \retval SHR_E_INTERNAL Invalid entry count value when DMA is TRUE.
 */
static int
bcm56780_a0_trunk_vp_pt_write(int unit,
                          bcmcth_trunk_vp_pt_op_info_t *op_info,
                          uint32_t *buf)
{
    uint32_t *rsp_ent_buf = NULL; /* Valid buffer required only for READ. */
    void *ptr_pt_ovr_info = NULL; /* Physical Table Override info pointer. */
    bcmptm_misc_info_t misc_info; /* Miscellaneous DMA oper info.  */
    uint64_t req_flags; /* PTM required flags. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(op_info, SHR_E_PARAM);
    SHR_NULL_CHECK(buf, SHR_E_PARAM);

    /* Initialize the structure and set the values for PTM call. */
    sal_memset(&misc_info, 0, sizeof(bcmptm_misc_info_t));
    if (op_info->dma) {
        if (!op_info->ecount) {
            /* To perform TDMA valid entries count value is required. */
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        misc_info.dma_enable = op_info->dma;
        misc_info.dma_entry_count = op_info->ecount;
    }

    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_info->op_arg->attrib);

    /*
     * Check if Write operation has to be performed using DMA engine. If DMA is
     * enabled, pass valid 'misc_info' argument to PTM function or else pass
     * NULL value as argument to misc_info PTM function input parameter.
     */
    if (misc_info.dma_enable) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_ltm_mreq_indexed_lt(unit,
                                        req_flags,
                                        op_info->drd_sid,
                                        (void *)&(op_info->dyn_info),
                                        ptr_pt_ovr_info,
                                        &misc_info,
                                        op_info->op,
                                        buf,
                                        op_info->wsize,
                                        op_info->req_lt_sid,
                                        op_info->op_arg->trans_id,
                                        NULL,
                                        NULL,
                                        rsp_ent_buf,
                                        &(op_info->rsp_lt_sid),
                                        &(op_info->rsp_flags)));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_ltm_mreq_indexed_lt(unit,
                                        req_flags,
                                        op_info->drd_sid,
                                        (void *)&(op_info->dyn_info),
                                        ptr_pt_ovr_info,
                                        NULL, /* misc_info */
                                        op_info->op,
                                        buf,
                                        op_info->wsize,
                                        op_info->req_lt_sid,
                                        op_info->op_arg->trans_id,
                                        NULL,
                                        NULL,
                                        rsp_ent_buf,
                                        &(op_info->rsp_lt_sid),
                                        &(op_info->rsp_flags)));
    }
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "\tpt_write[drd_sid=%d idx=%d]: trans_id=0x%x "
                          "req_ltid=0x%x rsp_ltid=0x%x\n"),
              op_info->drd_sid,
              op_info->dyn_info.index,
              op_info->op_arg->trans_id,
              op_info->req_lt_sid,
              op_info->rsp_lt_sid));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Read an entry or entries from a table given.
 *
 * Read an entry or entries from a table given index or start index and number
 * of entries to read.
 *
 * \param [in] unit Unit number.
 * \param [in] op_info Pointer to bcmcth_trunk_vp_pt_op_info_t structure.
 * \param [out] buf Pointer to hardware table entry read data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Input parameter error.
 * \retval SHR_E_FAIL Hardware table read operation unsuccessful.
 * \retval SHR_E_INTERNAL Invalid entry count value when DMA is TRUE.
 */
static int
bcm56780_a0_trunk_vp_pt_read(int unit,
                         bcmcth_trunk_vp_pt_op_info_t *op_info,
                         uint32_t *buf)
{
    uint32_t *ent_buf = NULL; /* Valid buffer required only for READ. */
    void *ptr_pt_ovr_info = NULL; /* Physical Table Override info pointer. */
    bcmptm_misc_info_t misc_info; /* Miscellaneous DMA oper info.  */
    uint64_t req_flags; /* PTM required flags. */
    uint32_t flags_0, flags_1;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(op_info, SHR_E_PARAM);
    SHR_NULL_CHECK(buf, SHR_E_PARAM);

    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_info->op_arg->attrib);
    flags_0 = (uint32_t) req_flags;
    flags_1 = (uint32_t) (req_flags >> 32);
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "\tpt_read-in[drd_sid=%d hw_idx=%d]: trans_id=0x%x "
                          " req_flags=0x%x%x req_ltid=0x%x rsp_ltid=0x%x\n"),
              op_info->drd_sid,
              op_info->dyn_info.index,
              op_info->op_arg->trans_id,
              flags_1,
              flags_0,
              op_info->req_lt_sid,
              op_info->rsp_lt_sid));
    /* Initialize the structure and set the values for PTM call. */
    sal_memset(&misc_info, 0, sizeof(bcmptm_misc_info_t));
    if (op_info->dma) {
        if (!op_info->ecount) {
            /* To perform TDMA valid entries count value is required. */
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        misc_info.dma_enable = op_info->dma;
        misc_info.dma_entry_count = op_info->ecount;
    }

    /*
     * Check if Write operation has to be performed using DMA engine. If DMA is
     * enabled, pass valid 'misc_info' argument to PTM function or else pass
     * NULL value as argument to misc_info PTM function input parameter.
     */
    if (misc_info.dma_enable) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_ltm_mreq_indexed_lt(unit,
                                        req_flags,
                                        op_info->drd_sid,
                                        (void *)&(op_info->dyn_info),
                                        ptr_pt_ovr_info,
                                        &misc_info,
                                        op_info->op,
                                        ent_buf,
                                        op_info->wsize,
                                        op_info->req_lt_sid,
                                        op_info->op_arg->trans_id,
                                        NULL,
                                        NULL,
                                        buf,
                                        &(op_info->rsp_lt_sid),
                                        &(op_info->rsp_flags)));

    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmptm_ltm_mreq_indexed_lt(unit,
                                        req_flags,
                                        op_info->drd_sid,
                                        (void *)&(op_info->dyn_info),
                                        ptr_pt_ovr_info,
                                        NULL, /* misc_info */
                                        op_info->op,
                                        ent_buf,
                                        op_info->wsize,
                                        op_info->req_lt_sid,
                                        op_info->op_arg->trans_id,
                                        NULL,
                                        NULL,
                                        buf,
                                        &(op_info->rsp_lt_sid),
                                        &(op_info->rsp_flags)));
    }

    /*
     * PTM Values:
     *  - BCMPTM_LTID_RSP_NOT_SUPPORTED 0xFFFFFFFD
     *  - BCMPTM_LTID_RSP_HW 0xFFFFFFFF
     */
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "\tpt_read-out[drd_sid=%d hw_idx=%d]: trans_id=0x%x "
                          " req_flags=0x%x%x req_ltid=0x%x rsp_ltid=0x%x\n"),
              op_info->drd_sid,
              op_info->dyn_info.index,
              op_info->op_arg->trans_id,
              flags_1,
              flags_0,
              op_info->req_lt_sid,
              op_info->rsp_lt_sid));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initalize a bcmcth_trunk_vp_grp_drd_fields_t structure memory.
 *
 * \param [in] gdrd Pointer to bcmcth_trunk_vp_grp_drd_fields_t structure.
 *
 * \return Nothing.
 */
static void
bcmcth_trunk_vp_grp_drd_fields_t_init(bcmcth_trunk_vp_grp_drd_fields_t *gdrd)
{
    if (gdrd) {
        sal_memset(gdrd, INVALIDf, sizeof(*gdrd));
    }
    return;
}

/*!
 * \brief Initalize a bcmcth_trunk_vp_lt_field_attrs_t structure memory.
 *
 * \param [in] fattrs Pointer to bcmcth_trunk_vp_lt_field_attrs_t structure.
 *
 * \return Nothing.
 */
static void
bcmcth_trunk_vp_lt_field_attrs_t_init(bcmcth_trunk_vp_lt_field_attrs_t *fattrs)
{
    if (fattrs) {
        sal_memset(fattrs, BCMCTH_TRUNK_VP_INVALID, sizeof(*fattrs));
    }
    return;
}

/*!
 * \brief Initialize Trident4 TRUNK_VP tables DRD-SID and LTD-SID information.
 *
 * Initialize DRD-SID and LTD-SID details for TRUNK_VP Group, Member
 * load balancing count tables.
 *
 * \param [in] unit Unit number.
 * \param [in] vp_drv_var VP device variant info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL On invalid TRUNK_VP mode value.
 */
static int
bcm56780_a0_trunk_vp_sids_init(int unit,
                               const bcmcth_trunk_vp_drv_var_t *vp_drv_var)
{
    uint32_t i = 0;
    const bcmcth_trunk_vp_member_info_t *member0_info;
    SHR_FUNC_ENTER(unit);

    BCMCTH_TRUNK_VP_MODE(unit) = BCMCTH_TRUNK_VP_MODE_FULL;
    /* Initialize device TRUNK_VP config register DRD and LTD SID values. */
    BCMCTH_TRUNK_VP_REG_DRD_SID(unit, config) = INVALIDr;

    member0_info = vp_drv_var->member0_info;

    /* Initialize TRUNK_VP_MODE specific DRD + LTD SID values. */
    /* Initialize Overlay TRUNK_VP resolution tables DRD-SID values. */
    BCMCTH_TRUNK_VP_TBL_DRD_SID
        (unit, group,
         BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) = ECMP_GROUP_LEVEL0_GROUP_TABLEm;
    BCMCTH_TRUNK_VP_TBL_DRD_SID
        (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0) =
         ECMP_GROUP_LEVEL0_GROUP_TABLEm;


    if ((member0_info != NULL) &&
        (vp_drv_var->mem0 != NULL)) {

        BCMCTH_TRUNK_VP_TBL_DRD_SID
            (unit, member,
             BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) = vp_drv_var->mem0[i];

        BCMCTH_TRUNK_VP_TBL_DRD_SID
            (unit, member,
             BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0) = vp_drv_var->mem0[i];

        BCMCTH_TRUNK_VP_TBL_DRD_SID
            (unit, member2,
             BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) = INVALIDm;

        BCMCTH_TRUNK_VP_TBL_DRD_SID
            (unit, member2,
             BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0) = INVALIDm;
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Initialize LTD-SID values for Overlay and Underlay LTs. */
    BCMCTH_TRUNK_VP_TBL_LTD_SID
        (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) =
         vp_drv_var->ids->trunk_vp_ltid;
    BCMCTH_TRUNK_VP_TBL_LTD_SID
        (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0) =
         vp_drv_var->ids->trunk_vp_weighted_ltid;
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize TRUNK_VP LT fields LRD and DRD IDs.
 *
 * Initialize TRUNK_VP single TRUNK_VP resolution LT fields LRD and DRD IDs
 * for use duing LT entry INSERT, LOOKUP, UPDATE and DELETE operations.
 *
 * \param [in] unit Unit number.
 * \param [in] map LT fields LRD mapping data.
 * \param [in] map Weighted LT fields LRD mapping data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL Unexpected invalid input parameter.
 * \retval SHR_E_MEMORY Memory allocation failure.
 */
static int
bcm56780_a0_trunk_vp_olay_lt_flds_init(int unit,
                                   const bcmlrd_map_t *map,
                                   const bcmlrd_map_t *wmap,
                                   const bcmcth_trunk_drv_var_ids_t *ids)
{
    /* Group tbl DRD info. */
    bcmcth_trunk_vp_grp_drd_fields_t *ogdrd_fields = NULL;
    /* Overlay LT fields LTD ID. */
    bcmcth_trunk_vp_grp_ltd_fields_t *olt_fids = NULL, *owlt_fids = NULL;
    /* Overlay lt field attrs. */
    bcmcth_trunk_vp_lt_field_attrs_t *ogrp_attrs = NULL, *owgrp_attrs = NULL;
    const bcmlrd_field_data_t *fdata = NULL;     /* LT field data map. */
    uint32_t idx;    /* Enum symbol array index. */
    uint32_t lb_val; /* LB mode value. */
    const char *const lb_name[BCMCTH_TRUNK_VP_LB_MODE_COUNT] =
                         BCMCTH_TRUNK_VP_LB_MODE_NAME;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(map, SHR_E_INTERNAL);

    LOG_INFO(BSL_LOG_MODULE,
                (BSL_META_U(unit, "olay_fields_init")));

    /* Allocate memory for storing Group table entry fields DRD ID values. */
    BCMCTH_TRUNK_VP_ALLOC(ogdrd_fields,
                          sizeof(*ogdrd_fields), "bcmcTrunkVpOlayGdrdFlds");

    /* Initialize Group table entry fields DRD ID values. */
    bcmcth_trunk_vp_grp_drd_fields_t_init(ogdrd_fields);
    ogdrd_fields->lb_mode = LB_MODEf;
    ogdrd_fields->base_ptr = BASE_PTRf;
    ogdrd_fields->count = COUNTf;

    BCMCTH_TRUNK_VP_TBL_DRD_FIELDS
        (unit, member, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) = NULL;
    BCMCTH_TRUNK_VP_TBL_DRD_FIELDS
        (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) = (void *)ogdrd_fields;
    /*
     * Allocate memory for storing TRUNK_VP LT logical fields Min, Max
     * attribute values.
     */
    BCMCTH_TRUNK_VP_ALLOC(ogrp_attrs,
                          sizeof(*ogrp_attrs), "bcmcthTrunkVpOlayGrpAttrs");
    BCMCTH_TRUNK_VP_LT_FIELD_ATTRS_VOID_PTR
        (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) = (void *)ogrp_attrs;
    bcmcth_trunk_vp_lt_field_attrs_t_init(ogrp_attrs);

    /*
     * Initialize Group logical table field identifer values based on the
     * device TRUNK_VP mode.
     */
    BCMCTH_TRUNK_VP_ALLOC(olt_fids,
                          sizeof(*olt_fids), "bcmcthTrunkVpOlayGrpLtdFids");
    BCMCTH_TRUNK_VP_TBL_LTD_FIELDS
        (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) = (void *)olt_fids;
    /* Initialize Overlay TRUNK_VP Group LTD fields information. */
    olt_fids->trunk_vp_id_fid = ids->trunk_vp_trunk_vp_id;
    olt_fids->lb_mode_fid = ids->trunk_vp_lb_mode_id;
    olt_fids->max_members_fid = ids->trunk_vp_max_members_id;
    olt_fids->member_cnt_fid = ids->trunk_vp_member_cnt_id;
    /* Get TRUNK_VP ID min and max values from LRD field data. */
    if (olt_fids->trunk_vp_id_fid != BCMCTH_TRUNK_INVALID) {
        fdata = &(map->field_data->field[olt_fids->trunk_vp_id_fid]);
        BCMCTH_TRUNK_VP_LT_MIN_TRUNK_VP_ID
            (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) = fdata->min->u16;
        BCMCTH_TRUNK_VP_LT_MAX_TRUNK_VP_ID
            (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) = fdata->max->u16;
    }

    /*
    * Get load balancing mode min and max values from LRD field
    * data.
    */
    if (olt_fids->lb_mode_fid != BCMCTH_TRUNK_INVALID) {
        fdata = &(map->field_data->field[olt_fids->lb_mode_fid]);
        BCMCTH_TRUNK_VP_LT_MIN_LB_MODE
            (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) = fdata->min->u32;
        BCMCTH_TRUNK_VP_LT_MAX_LB_MODE
            (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) = fdata->max->u32;

        /*
        * Iterate over lb_mode enum symbols, get enum values and store in
        * SW database.
        */
        for (idx = 0; idx < fdata->edata->num_sym; idx++) {
           /* Get LB_MODE enum value. */
            lb_val = (uint32_t)fdata->edata->sym[idx].val;

            /*
             * Check and confirm LB_MODE enum name matches expected name
             * for this LB_MODE enum value. Return error and exit
             * initialization if invalid/unrecognized LB_MODE enum name is
             * found.
             */
            if (!sal_strcmp(lb_name[lb_val], fdata->edata->sym[idx].name)) {
                /* Initialize supported LB modes for Overlay TRUNK_VP LT. */
                BCMCTH_TRUNK_VP_LT_LB_MODE
                  (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY, lb_val) = lb_val;
            } else {
                LOG_ERROR(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Unsupported supported LB mode %d"), lb_val));

                SHR_ERR_EXIT(SHR_E_INTERNAL);
            }
        }
    }
    /*
     * It's not mandatory that MAX_MEMBERS and MEMBER_CNT values must be in
     * powers-of-two for entries in TRUNK_VP LT for this device.
     */
    BCMCTH_TRUNK_VP_LT_POW_TWO_PATHS
        (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) = FALSE;

    /*
     * Get MAX_MEMBERS logical table LRD field data pointer.
     * Initialize device max_members minimum and maximum values.
     */
    if (olt_fids->max_members_fid != BCMCTH_TRUNK_INVALID) {
        fdata = &(map->field_data->field[olt_fids->max_members_fid]);
        BCMCTH_TRUNK_VP_LT_MIN_MEMBERS
            (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) = fdata->min->u16;
        BCMCTH_TRUNK_VP_LT_MAX_MEMBERS
            (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) = fdata->max->u16;
    }

    /*
     * Get MEMBER_CNT logical table LRD field data pointer.
     * Initialize device member_cnt minimum and maximum values.
     */
    if (olt_fids->member_cnt_fid != BCMCTH_TRUNK_INVALID) {
        fdata = &(map->field_data->field[olt_fids->member_cnt_fid]);
        BCMCTH_TRUNK_VP_LT_MIN_NMEMBERS
            (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) = fdata->min->u16;
        BCMCTH_TRUNK_VP_LT_MAX_NMEMBERS
            (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) = fdata->max->u16;
    }
    BCMCTH_TRUNK_VP_TBL_DRD_FIELDS
        (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0) =
        (void *)ogdrd_fields;

    /*
     * Allocate memory for storing TRUNK_VP LT logical fields Min, Max
     * attribute values.
     */
    BCMCTH_TRUNK_VP_ALLOC(owgrp_attrs,
        sizeof(*owgrp_attrs), "bcmcthTrunkvpWOlayGrpAttrs");
    BCMCTH_TRUNK_VP_LT_FIELD_ATTRS_VOID_PTR
        (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0) =
        (void *)owgrp_attrs;
    bcmcth_trunk_vp_lt_field_attrs_t_init(owgrp_attrs);

    /*
     * Initialize Group logical table field identifer values based on the
     * device ECMP mode.
     */
    BCMCTH_TRUNK_VP_ALLOC(owlt_fids,
        sizeof(*owlt_fids), "bcmcthTrunkVpWOlayGrpLtdFids");
    BCMCTH_TRUNK_VP_TBL_LTD_FIELDS
        (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0) = (void *)owlt_fids;

    /* Initialize TRUNK_VP_WEIGHTED Group LTD fields information. */
    owlt_fids->trunk_vp_id_fid = ids->trunk_vp_weighted_trunk_vp_id;
    owlt_fids->member_cnt_fid = ids->trunk_vp_weighted_member_cnt_id;
    owlt_fids->weighted_size_fid = ids->trunk_vp_weighted_wt_size_id;

    /* Get TRUNK_VP ID min and max values from LRD field data. */
    if (owlt_fids->trunk_vp_id_fid != BCMCTH_TRUNK_INVALID) {
        fdata = &(wmap->field_data->field[owlt_fids->trunk_vp_id_fid]);
        BCMCTH_TRUNK_VP_LT_MIN_TRUNK_VP_ID
            (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0) = fdata->min->u16;
        BCMCTH_TRUNK_VP_LT_MAX_TRUNK_VP_ID
            (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0) = fdata->max->u16;

    }
    /*
    * Get weighted size min and max values from LRD field
    * data.
    */
    if (owlt_fids->weighted_size_fid != BCMCTH_TRUNK_INVALID) {
        fdata = &(wmap->field_data->field[owlt_fids->weighted_size_fid]);
        BCMCTH_TRUNK_VP_LT_MIN_WEIGHTED_SIZE
            (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0) = fdata->min->u32;
        BCMCTH_TRUNK_VP_LT_MAX_WEIGHTED_SIZE
            (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0) = fdata->max->u32;
        BCMCTH_TRUNK_VP_LT_MIN_LB_MODE
            (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0) = fdata->min->u32;
        BCMCTH_TRUNK_VP_LT_MAX_LB_MODE
            (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0) = fdata->max->u32;
        /*
         * Get MAX_PATHS logical table LRD field data pointer.
         * Initialize device max_paths minimum and maximum values.
         */
        BCMCTH_TRUNK_VP_LT_MIN_MEMBERS
            (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0) =
        bcmcth_trunk_vp_weighted_size_convert_to_member_cnt
                        (fdata->min->u32);
        BCMCTH_TRUNK_VP_LT_MAX_MEMBERS
            (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0) =
        bcmcth_trunk_vp_weighted_size_convert_to_member_cnt
                        (fdata->max->u32);
    }

    BCMCTH_TRUNK_VP_LT_POW_TWO_PATHS
        (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0) = TRUE;

    /*
     * Get NUM_PATHS logical table LRD field data pointer.
     * Initialize device num_paths minimum and maximum values.
     */
    if (owlt_fids->member_cnt_fid != BCMCTH_TRUNK_INVALID) {
        fdata = &(wmap->field_data->field[owlt_fids->member_cnt_fid]);
        BCMCTH_TRUNK_VP_LT_MIN_NMEMBERS
            (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0) = fdata->min->u16;
        BCMCTH_TRUNK_VP_LT_MAX_NMEMBERS
            (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0) = fdata->max->u16;
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize Trident4 device specific TRUNK_VP tables fields
 *        information.
 * Initialize TRUNK_VP Group and Member tables fields information.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL Invalid TRUNK_VP mode, invalid LB mode, null pointer.
 */
static int
bcm56780_a0_trunk_vp_fields_init(int unit,
                                 const bcmcth_trunk_drv_var_ids_t *ids)
{
    const bcmlrd_map_t *map = NULL, *wmap = NULL; /* LT map. */
    int rv = SHR_E_INTERNAL; /* Return value variable. */

    SHR_FUNC_ENTER(unit);

    /* Get LRD mapping information for TRUNK_VP LT. */
    rv = bcmlrd_map_get(unit,
                        BCMCTH_TRUNK_VP_TBL_LTD_SID
                            (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY),
                        &map);
    if (SHR_SUCCESS(rv)) {
        /* Get LRD mapping information for TRUNK_VP LT. */
        rv = bcmlrd_map_get(unit,
                        BCMCTH_TRUNK_VP_TBL_LTD_SID
                            (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0),
                            &wmap);
    }

    if (SHR_SUCCESS(rv)) {
        SHR_NULL_CHECK(map, SHR_E_INTERNAL);
        /*
         * Create TRUNK_VP LT related fields database info
         * required for programming LT entries in this LT.
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_trunk_vp_olay_lt_flds_init(unit, map, wmap, ids));

    }
    SHR_IF_ERR_EXIT(rv);
exit:
    SHR_FUNC_EXIT();

}

static int
bcm56780_a0_trunk_vp_itbm_lists_cleanup(int unit)
{
    shr_itbm_list_hdl_t list_hdl;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);
    list_hdl =
      BCMCTH_TRUNK_VP_TBL_ITBM_LIST((BCMCTH_TRUNK_VP_TBL_PTR(unit, group,
             BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY)));
    rv = shr_itbm_list_destroy(list_hdl);
    if (rv != SHR_E_NONE) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                     "Error: RM cleaning up group list.")));
        SHR_ERR_EXIT(rv);
    }
    BCMCTH_TRUNK_VP_TBL_ITBM_LIST((BCMCTH_TRUNK_VP_TBL_PTR(unit, group,
             BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY))) = NULL;
    BCMCTH_TRUNK_VP_TBL_ITBM_LIST((BCMCTH_TRUNK_VP_TBL_PTR(unit, group,
             BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0))) = NULL;

    list_hdl =
      BCMCTH_TRUNK_VP_TBL_ITBM_LIST((BCMCTH_TRUNK_VP_TBL_PTR(unit, member,
             BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY)));
    rv = shr_itbm_list_destroy(list_hdl);
    if (rv != SHR_E_NONE) {
       LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                     "Error: RM cleaning up member list.")));
       SHR_ERR_EXIT(rv);
    }
    BCMCTH_TRUNK_VP_TBL_ITBM_LIST((BCMCTH_TRUNK_VP_TBL_PTR(unit, member,
             BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY))) = NULL;
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56780_a0_trunk_vp_itbm_lists_init(int unit)
{
    shr_itbm_list_params_t params;
    shr_itbm_list_hdl_t grp_list_hdl = NULL;
    shr_itbm_list_hdl_t mem_list_hdl = NULL;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    shr_itbm_list_params_t_init(&params);
    params.unit = unit;
    params.table_id = BCMCTH_TRUNK_VP_TBL_DRD_SID(unit, group,
                                BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY);
    params.min = BCMCTH_TRUNK_VP_TBL_IDX_MIN
        (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY);
    params.max = BCMCTH_TRUNK_VP_TBL_IDX_MAX
        (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) +
        BCMCTH_TRUNK_VP_TBL_IDX_MAX
        (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) + 1;

    params.first = BCMCTH_TRUNK_VP_TBL_IDX_MIN
        (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY);
    params.last = BCMCTH_TRUNK_VP_TBL_IDX_MAX
        (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY);

    params.comp_id = BCMMGMT_TRUNK_COMP_ID;
    params.block_stat = false;

    rv = shr_itbm_list_create(params, "bcmcthTrunkVpGrp0List", &grp_list_hdl);
    if ((rv== SHR_E_NONE) || (rv == SHR_E_EXISTS)) {
        BCMCTH_TRUNK_VP_TBL_ITBM_LIST((BCMCTH_TRUNK_VP_TBL_PTR(unit, group,
             BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY)))
             = grp_list_hdl;
        BCMCTH_TRUNK_VP_TBL_ITBM_LIST((BCMCTH_TRUNK_VP_TBL_PTR(unit, group,
             BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0)))
             = grp_list_hdl;
        LOG_VERBOSE(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                          "ITBM group list creation rv=%d list %p.\n"),
                          rv,
                          ((void *)grp_list_hdl)));
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                          "ITBM group list creation failed rv=%d\n"), rv));
        SHR_ERR_EXIT(rv);
    }
    params.table_id = BCMCTH_TRUNK_VP_TBL_DRD_SID(unit, member,
                                BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY);
    params.min = BCMCTH_TRUNK_VP_TBL_IDX_MIN
        (unit, member, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY);
    params.max = BCMCTH_TRUNK_VP_TBL_IDX_MAX
        (unit, member, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY);
    params.first = BCMCTH_TRUNK_VP_TBL_IDX_MIN
        (unit, member, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY);
    params.last = BCMCTH_TRUNK_VP_TBL_IDX_MAX
        (unit, member, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY);
    params.block_stat = true;

    rv = shr_itbm_list_create(params, "bcmcthTrunkVpMem0List", &mem_list_hdl);
    if ((rv == SHR_E_NONE) || (rv == SHR_E_EXISTS)) {
        BCMCTH_TRUNK_VP_TBL_ITBM_LIST((BCMCTH_TRUNK_VP_TBL_PTR(unit, member,
             BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY)))
             = mem_list_hdl;
        BCMCTH_TRUNK_VP_TBL_ITBM_LIST((BCMCTH_TRUNK_VP_TBL_PTR(unit, member,
             BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0)))
             = mem_list_hdl;
        LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                             "ITBM member list creation rv=%d list %p.\n"),
                             rv,
                             ((void *)mem_list_hdl)));
    } else {
        LOG_ERROR(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                            "ITBM member list creation failed rv=%d\n"),
                            rv));
        SHR_ERR_EXIT(rv);
    }

exit:
    if (SHR_FUNC_ERR()) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "RM cleaning up list.\n")));
        if (grp_list_hdl) {
            BCMCTH_TRUNK_VP_TBL_ITBM_LIST(
                (BCMCTH_TRUNK_VP_TBL_PTR(unit, group,
                                         BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY))) = NULL;
            BCMCTH_TRUNK_VP_TBL_ITBM_LIST(
                (BCMCTH_TRUNK_VP_TBL_PTR(unit, group,
                                         BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0))) = NULL;
            shr_itbm_list_destroy(grp_list_hdl);
        }
        if (mem_list_hdl) {
            BCMCTH_TRUNK_VP_TBL_ITBM_LIST(
                (BCMCTH_TRUNK_VP_TBL_PTR(unit, member,
                                         BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY))) = NULL;
            BCMCTH_TRUNK_VP_TBL_ITBM_LIST(
                (BCMCTH_TRUNK_VP_TBL_PTR(unit, member,
                                         BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0))) = NULL;
            shr_itbm_list_destroy(mem_list_hdl);
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize Trident4 device specfic ECMP tables range information.
 *
 * Initialize ECMP tables size, entry size and valid index range details.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL On invalid ECMP mode value.
 */
static int
bcm56780_a0_trunk_vp_hw_tbls_range_init(int unit)
{
    uint32_t wsize; /* Hardware table entry size in words. */
    int memb_imin;  /* ECMP Member hardware table Min index value. */
    int memb_imax;  /* ECMP Member hardware table Max index value. */
    int grp_imin;   /* ECMP Group hardware table Min index value. */
    int grp_imax;   /* ECMP Group hardware table Max index value. */
    int tbl_sz;     /* Local variable to store calculated table size. */

    SHR_FUNC_ENTER(unit);

    /* Initialize device ECMP config register entry size in words from DRD. */
    BCMCTH_TRUNK_VP_REG_ENT_SIZE(unit, config) = bcmdrd_pt_entry_wsize
                                    (unit,
                                    BCMCTH_TRUNK_VP_REG_DRD_SID(unit, config));

    /*
     * Get Overlay Group table i.e. ECMP_LEVEL0 table entry
     * size in words.
     */
    wsize = bcmdrd_pt_entry_wsize(unit,
                                  BCMCTH_TRUNK_VP_TBL_DRD_SID(unit, group,
                                        BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY));
    BCMCTH_TRUNK_VP_TBL_ENT_SIZE
        (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) = wsize;
    if (wsize > BCMCTH_TRUNK_VP_TBL_ENT_SIZE_MAX(unit)) {
        BCMCTH_TRUNK_VP_TBL_ENT_SIZE_MAX(unit) = wsize;
    }

    wsize = bcmdrd_pt_entry_wsize(unit,
                                  BCMCTH_TRUNK_VP_TBL_DRD_SID
                                        (unit, member,
                                         BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY));
    BCMCTH_TRUNK_VP_TBL_ENT_SIZE
        (unit, member, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) = wsize;
    if (wsize > BCMCTH_TRUNK_VP_TBL_ENT_SIZE_MAX(unit)) {
        BCMCTH_TRUNK_VP_TBL_ENT_SIZE_MAX(unit) = wsize;
    }

    BCMCTH_TRUNK_VP_TBL_ENT_SIZE
        (unit, group,
         BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0) = BCMCTH_TRUNK_VP_TBL_ENT_SIZE
                                            (unit, group,
                                             BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY);

    BCMCTH_TRUNK_VP_TBL_ENT_SIZE
        (unit, member,
         BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0) = BCMCTH_TRUNK_VP_TBL_ENT_SIZE
                                            (unit, member,
                                             BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY);

    /* Get ECMP Member HW table Min and Max index ranges from DRD. */
    memb_imin = bcmdrd_pt_index_min(unit,
                                    BCMCTH_TRUNK_VP_TBL_DRD_SID(unit, member,
                                        BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY));
    memb_imax = bcmdrd_pt_index_max(unit,
                                    BCMCTH_TRUNK_VP_TBL_DRD_SID(unit, member,
                                        BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY));

    /* Get ECMP Group HW table Min and Max index ranges from DRD. */
    grp_imin = bcmdrd_pt_index_min(unit,
                                   BCMCTH_TRUNK_VP_TBL_DRD_SID(unit, group,
                                        BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY));
    grp_imax = bcmdrd_pt_index_max(unit,
                                   BCMCTH_TRUNK_VP_TBL_DRD_SID(unit, group,
                                        BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY));

    /*
     * Initialize Overlay ECMP Member table Min & Max indicies, table
     * size and used indices status.
     * Max index value is half of the hardware table size. Member table
     * is split into two to support Overlay and Underlay ECMP lookups.
     */
    BCMCTH_TRUNK_VP_TBL_IDX_MIN
        (unit, member, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) = memb_imin;
    BCMCTH_TRUNK_VP_TBL_IDX_MAX
        (unit, member, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) = memb_imax;
    tbl_sz = (BCMCTH_TRUNK_VP_TBL_IDX_MAX
                (unit, member, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY)
                - BCMCTH_TRUNK_VP_TBL_IDX_MIN
                    (unit, member, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY)) + 1;
    BCMCTH_TRUNK_VP_TBL_SIZE
        (unit, member, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) = tbl_sz;
    BCMCTH_TRUNK_VP_TBL_SIZE
        (unit, member, BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0) = tbl_sz;
    BCMCTH_TRUNK_VP_TBL_IDX_MAX_USED
        (unit, member, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) = memb_imin;

    /*
     * Overlay ECMP Group HW Table Min & Max indicies, Size and Used
     * indices status intialization.
     */
    BCMCTH_TRUNK_VP_TBL_IDX_MIN
        (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) = grp_imin;
    BCMCTH_TRUNK_VP_TBL_IDX_MAX
       (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) = grp_imax;

    BCMCTH_TRUNK_VP_TBL_IDX_MIN
        (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0) = grp_imin;
    BCMCTH_TRUNK_VP_TBL_IDX_MAX
        (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0) = grp_imax;

    /* Calculate Overlay ECMP Group table size. */
    tbl_sz = (BCMCTH_TRUNK_VP_TBL_IDX_MAX
                (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY)
                - BCMCTH_TRUNK_VP_TBL_IDX_MIN
                    (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY)) + 1;
    BCMCTH_TRUNK_VP_TBL_SIZE
            (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) = tbl_sz;
    BCMCTH_TRUNK_VP_TBL_SIZE
            (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0) = tbl_sz;
    BCMCTH_TRUNK_VP_TBL_IDX_MAX_USED
        (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) = grp_imin;
    BCMCTH_TRUNK_VP_TBL_IDX_MAX_USED
        (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0) = grp_imin;
    BCMCTH_TRUNK_VP_TBL_IDX_MIN
        (unit, member, BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0)
            = BCMCTH_TRUNK_VP_TBL_IDX_MIN
                  (unit, member, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY);
    BCMCTH_TRUNK_VP_TBL_IDX_MAX
        (unit, member, BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0)
            = BCMCTH_TRUNK_VP_TBL_IDX_MAX
                  (unit, member, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY);

    BCMCTH_TRUNK_VP_TBL_IDX_MIN
        (unit, member, BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0)
            = BCMCTH_TRUNK_VP_TBL_IDX_MIN
                  (unit, member, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY);
    BCMCTH_TRUNK_VP_TBL_IDX_MAX
        (unit, member, BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0)
            = BCMCTH_TRUNK_VP_TBL_IDX_MAX
                  (unit, member, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY);

    BCMCTH_TRUNK_VP_TBL_IDX_MAX_USED
        (unit, member, BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0)
        = BCMCTH_TRUNK_VP_TBL_IDX_MAX_USED
              (unit, member, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY);


    bcm56780_a0_trunk_vp_itbm_lists_init(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize Trident4 device specific TRUNK_VP tables information.
 *
 * Initialize TRUNK_VP Group and Member tables information by retrieving details
 * from LRD and DRD.
 *
 * \param [in] unit Unit number.
 * \param [in] vp_drv_var VP device variant info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL On invalid TRUNK_VP mode value, null pointer detected.
 */
static int
bcm56780_a0_trunk_vp_tables_init(int unit,
                                 const bcmcth_trunk_vp_drv_var_t *vp_drv_var)
{

    SHR_FUNC_ENTER(unit);

    /* Initialize TRUNK_VP tables DRD-SID and LTD-SID details. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_trunk_vp_sids_init(unit, vp_drv_var));

    /* Initialize TRUNK_VP hardware tables size and index range info. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_trunk_vp_hw_tbls_range_init(unit));

    /* Initialize TRUNK_VP tables logical and physical field details. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_trunk_vp_fields_init(unit, vp_drv_var->ids));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief This function allocates the memory for storing TRUNK_VP
 *        software state.
 *
 * This function allocates memory to store TRUNK_VP Group tables, Member tables
 * and TRUNK_VP logical table entries.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Table entries memory allocation failed.
 */
static int
bcm56780_a0_trunk_vp_db_init(int unit, bool warm)
{
    bcmcth_trunk_vp_tbl_prop_t *tbl_ptr = NULL; /* Hardware tbl info ptr. */
    size_t tbl_size;            /* TRUNK_VP group or member table size. */
    uint32_t ha_alloc_sz = 0;   /* Size of allocated HA memory block. */
    uint32_t tot_ha_mem = 0;    /* Total HA memory allocated by TRUNK_VP CTH. */
    uint32_t blk_sig = 0x78880000; /* TRUNK_VP HA block signature. */
    bcmcth_trunk_vp_hw_entry_ha_blk_t *hw_array_ptr;
    bcmcth_trunk_vp_grp_ha_blk_t *grp_array_ptr;

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "%s warm = %s.\n"),
              __func__,
              warm ? "TRUE" : "FALSE"));

    /* Allocate memory for Member table entries array. */
    tbl_size = (size_t)(BCMCTH_TRUNK_VP_TBL_SIZE
                            (unit, member, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY));
    /*
     * Allocate memory for active Member Table entries array
     * elements.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_trunk_vp_hw_entry_ha_alloc(
                                   unit,
                                   warm,
                                   tbl_size,
                                   BCMCTH_TRUNK_VP_MEMB_TBL_SUB_COMP_ID,
                                   (blk_sig |
                                   BCMCTH_TRUNK_VP_MEMB_TBL_SUB_COMP_ID),
                                   "bcmcthTrunkVpMemberTableHaInfo",
                                   &hw_array_ptr,
                                   &ha_alloc_sz));
    tbl_ptr = BCMCTH_TRUNK_VP_TBL_PTR
                 (unit, member, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY);
    tbl_ptr->ent_arr = hw_array_ptr->array;
    tot_ha_mem += ha_alloc_sz;
    /*
     * Initialize weighted TRUNK_VP groups member table entries array
     * pointer.
     */
    (BCMCTH_TRUNK_VP_TBL_PTR
        (unit, member,
         BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0))->ent_arr = tbl_ptr->ent_arr;

    /* Allocate memory for Group Table entries arrary. */
    tbl_size = (size_t)(BCMCTH_TRUNK_VP_TBL_SIZE
                            (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_trunk_vp_hw_entry_ha_alloc(
                                   unit,
                                   warm,
                                   tbl_size,
                                   BCMCTH_TRUNK_VP_GRP_TBL_SUB_COMP_ID,
                                   (blk_sig |
                                   BCMCTH_TRUNK_VP_GRP_TBL_SUB_COMP_ID),
                                   "bcmcthTrunkVpGroupTableHaInfo",
                                   &hw_array_ptr,
                                   &ha_alloc_sz));
    tbl_ptr = BCMCTH_TRUNK_VP_TBL_PTR
                (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY);
    tbl_ptr->ent_arr = hw_array_ptr->array;
    tot_ha_mem += ha_alloc_sz;

    /* Initialize weighted TRUNK_VP groups array pointer. */
    (BCMCTH_TRUNK_VP_TBL_PTR
        (unit, group,
         BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0))->ent_arr = tbl_ptr->ent_arr;
    /* Allocate memory for active TRUNK_VP group LT entries array elements. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_trunk_vp_group_attr_ha_alloc(
                                     unit,
                                     warm,
                                     tbl_size,
                                     BCMCTH_TRUNK_VP_GRP_SUB_COMP_ID,
                                     (blk_sig |
                                     BCMCTH_TRUNK_VP_GRP_SUB_COMP_ID),
                                     "bcmcthTrunkVpGroupAttrHaInfo",
                                     &grp_array_ptr,
                                     &ha_alloc_sz));
    BCMCTH_TRUNK_VP_GRP_INFO_PTR(unit) = grp_array_ptr->array;
    tot_ha_mem += ha_alloc_sz;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Total TRUNK_VP HA Mem = %uKB.\n"),
                tot_ha_mem / 1024));
exit:
    if (SHR_FUNC_ERR()) {
        if (BCMCTH_TRUNK_VP_GRP_INFO_PTR(unit)) {
            grp_array_ptr = (bcmcth_trunk_vp_grp_ha_blk_t *)
                ((uint8_t *)BCMCTH_TRUNK_VP_GRP_INFO_PTR(unit)
                 - sizeof(bcmcth_trunk_vp_grp_ha_blk_t));
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "\n\t free grp HA addr=0x%p\n"),
                         (void*)grp_array_ptr));
            shr_ha_mem_free(unit, grp_array_ptr);
            BCMCTH_TRUNK_VP_GRP_INFO_PTR(unit) = NULL;
        }

        tbl_ptr = BCMCTH_TRUNK_VP_TBL_PTR
            (unit, member, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY);
        if (tbl_ptr->ent_arr) {
            hw_array_ptr = (bcmcth_trunk_vp_hw_entry_ha_blk_t *)
                ((uint8_t *)tbl_ptr->ent_arr
                 - sizeof(bcmcth_trunk_vp_hw_entry_ha_blk_t));
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "\n\t free memb hw HA=0x%p\n"),
                         (void*)hw_array_ptr));
            shr_ha_mem_free(unit, hw_array_ptr);
            tbl_ptr->ent_arr = NULL;
        }

        tbl_ptr = BCMCTH_TRUNK_VP_TBL_PTR
            (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY);
        if (tbl_ptr->ent_arr) {
            hw_array_ptr = (bcmcth_trunk_vp_hw_entry_ha_blk_t *)
                ((uint8_t *)tbl_ptr->ent_arr
                 - sizeof(bcmcth_trunk_vp_hw_entry_ha_blk_t));
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "\n\t free grp hw HA=0x%p\n"),
                         (void*)hw_array_ptr));
            shr_ha_mem_free(unit, hw_array_ptr);
            tbl_ptr->ent_arr = NULL;
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief This function allocates the memory for storing TRUNK_VP
 *         software state.
 *
 * This function allocates memory to store TRUNK_VP Group tables, Member tables
 * and TRUNK_VP logical table entries.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Table entries memory allocation failed.
 */
static int
bcm56780_a0_trunk_vp_abort_db_init(int unit, bool warm)
{
    bcmcth_trunk_vp_tbl_prop_t *tbl_ptr = NULL; /* Hardware table info ptr. */
    size_t tbl_size; /* TRUNK_VP group or member table size. */
    uint32_t ha_alloc_sz = 0; /* Size of allocated HA memory block. */
    uint32_t tot_ha_mem = 0; /* Total HA memory allocated by TRUNK_VP CTH. */
    uint32_t blk_sig = 0x66880000; /* TRUNK_VP HA block signature. */
    bcmcth_trunk_vp_hw_entry_ha_blk_t *hw_array_ptr;
    bcmcth_trunk_vp_grp_ha_blk_t *grp_array_ptr;

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "%s warm = %s.\n"),
              __func__,
              warm ? "TRUE" : "FALSE"));

    /* Allocate memory for Member table entries array. */
    tbl_size = (size_t)(BCMCTH_TRUNK_VP_TBL_SIZE
                            (unit, member, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_trunk_vp_hw_entry_ha_alloc(
                                   unit,
                                   warm,
                                   tbl_size,
                                   BCMCTH_TRUNK_VP_MEMB_TBL_BK_SUB_COMP_ID,
                                   (blk_sig |
                                   BCMCTH_TRUNK_VP_MEMB_TBL_BK_SUB_COMP_ID),
                                   "bcmcthTrunkVpMemberTableBkHaInfo",
                                   &hw_array_ptr,
                                   &ha_alloc_sz));
    tbl_ptr = BCMCTH_TRUNK_VP_TBL_PTR(
                unit, member, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY);
    tbl_ptr->ent_bk_arr = hw_array_ptr->array;
    tot_ha_mem += ha_alloc_sz;


    /*
     * Initialize weighted TRUNK_VP groups member table entries array
     * pointer.
     */
    (BCMCTH_TRUNK_VP_TBL_PTR
        (unit, member,
         BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0))->ent_bk_arr =
           tbl_ptr->ent_bk_arr;

    /* Allocate memory for Group Table entries arrary. */
    tbl_size = (size_t)(BCMCTH_TRUNK_VP_TBL_SIZE
                            (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_trunk_vp_hw_entry_ha_alloc(
                                   unit,
                                   warm,
                                   tbl_size,
                                   BCMCTH_TRUNK_VP_GRP_TBL_BK_SUB_COMP_ID,
                                   (blk_sig |
                                   BCMCTH_TRUNK_VP_GRP_TBL_BK_SUB_COMP_ID),
                                   "bcmcthTrunkVpGroupTableBkHaInfo",
                                   &hw_array_ptr,
                                   &ha_alloc_sz));
    tbl_ptr = BCMCTH_TRUNK_VP_TBL_PTR(
                  unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY);
    tbl_ptr->ent_bk_arr = hw_array_ptr->array;
    tot_ha_mem += ha_alloc_sz;

    /* Initialize weighted TRUNK_VP groups array pointer. */
    (BCMCTH_TRUNK_VP_TBL_PTR
        (unit, group,
         BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0))->ent_bk_arr =
         tbl_ptr->ent_bk_arr;

    /* Allocate memory for active TRUNK_VP group LT entries array elements. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_trunk_vp_group_attr_ha_alloc(
                                     unit,
                                     warm,
                                     tbl_size,
                                     BCMCTH_TRUNK_VP_GRP_BK_SUB_COMP_ID,
                                     (blk_sig |
                                     BCMCTH_TRUNK_VP_GRP_BK_SUB_COMP_ID),
                                     "bcmcthTrunkVpGroupAttrBkHaInfo",
                                     &grp_array_ptr,
                                     &ha_alloc_sz));
    BCMCTH_TRUNK_VP_GRP_INFO_BK_PTR(unit) = grp_array_ptr->array;
    tot_ha_mem += ha_alloc_sz;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "Total TRUNK_VP HA BK Mem = %uKB.\n"),
                tot_ha_mem / 1024));
exit:
    if (SHR_FUNC_ERR()) {
        if (BCMCTH_TRUNK_VP_GRP_INFO_BK_PTR(unit)) {
            grp_array_ptr = (bcmcth_trunk_vp_grp_ha_blk_t *)
                ((uint8_t *)BCMCTH_TRUNK_VP_GRP_INFO_BK_PTR(unit)
                 - sizeof(bcmcth_trunk_vp_grp_ha_blk_t));
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "\n\t free bk grp HA addr=0x%p\n"),
                         (void*)grp_array_ptr));
            shr_ha_mem_free(unit, grp_array_ptr);
            BCMCTH_TRUNK_VP_GRP_INFO_BK_PTR(unit) = NULL;
        }

        tbl_ptr = BCMCTH_TRUNK_VP_TBL_PTR
            (unit, member, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY);
        if (tbl_ptr->ent_bk_arr) {
            hw_array_ptr = (bcmcth_trunk_vp_hw_entry_ha_blk_t *)
                ((uint8_t *)tbl_ptr->ent_bk_arr
                 - sizeof(bcmcth_trunk_vp_hw_entry_ha_blk_t));
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "\n\tfree bk memb hw HA=0x%p\n"),
                         (void*)hw_array_ptr));
            shr_ha_mem_free(unit, hw_array_ptr);
            tbl_ptr->ent_bk_arr = NULL;
        }
        tbl_ptr = BCMCTH_TRUNK_VP_TBL_PTR
            (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY);
        if (tbl_ptr->ent_bk_arr) {
            hw_array_ptr = (bcmcth_trunk_vp_hw_entry_ha_blk_t *)
                ((uint8_t *)tbl_ptr->ent_bk_arr
                 - sizeof(bcmcth_trunk_vp_hw_entry_ha_blk_t));
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "\n\tfree bk grp hw HA=0x%p\n"),
                         (void*)hw_array_ptr));
            shr_ha_mem_free(unit, hw_array_ptr);
            tbl_ptr->ent_bk_arr = NULL;
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief TRUNK_VP Group and Member table entries initialization function.
 *
 * Allocates memory to store TRUNK_VP Group and Member table entries usage
 * information.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Table entries memory allocation failed.
 */
static int
bcm56780_a0_trunk_vp_entries_init(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    /*
     * Allocate HA memory for managing TRUNK_VP group and member table
     * entries.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_trunk_vp_db_init(unit, warm));

    /* Initialize atomic transaction related SW database. */
    if (BCMCTH_TRUNK_VP_ATOMIC_TRANS(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_trunk_vp_abort_db_init(unit, warm));
    }
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Trident4 device TRUNK_VP hardware table details initialization
 * function.
 *
 * Initializes TRUNK_VP hardware Tables, Fields and Entries information.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start status.
 * \param [in] drv_var device variant info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56780_a0_trunk_vp_info_init(int unit,
                               bool warm,
                               const bcmcth_trunk_drv_var_t *drv_var)
{
    SHR_FUNC_ENTER(unit);

    /* Initialize TRUNK_VP tables information for Trident4 device. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_trunk_vp_tables_init(unit, drv_var->trunk_vp_var));

    /*
     * Initialize TRUNK_VP Group and Member tables entries array for
     * Overlay group support.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_trunk_vp_entries_init(unit, warm));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Cleanup TRUNK_VP LT fields LRD and DRD data.
 *
 * Cleanup TRUNK_VP single ECMP resolution LT fields LRD and DRD data.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcm56780_a0_trunk_vp_olay_grps_flds_cleanup(int unit)
{
    bcmcth_trunk_vp_grp_drd_fields_t *ogdrd_fields = NULL;
    bcmcth_trunk_vp_grp_ltd_fields_t *olt_fids = NULL;
    bcmcth_trunk_vp_lt_field_attrs_t *ogrp_attrs = NULL;

    SHR_FUNC_ENTER(unit);

    /* Free memory allocated for storing Group tables DRD fields data. */
    ogdrd_fields = (bcmcth_trunk_vp_grp_drd_fields_t *)
                          BCMCTH_TRUNK_VP_TBL_DRD_FIELDS
                                  (unit, group,
                                  BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY);
    BCMCTH_TRUNK_VP_FREE(ogdrd_fields);

    /* Free memory allocated for storing TRUNK_VP LT logical fields data. */
    olt_fids = (bcmcth_trunk_vp_grp_ltd_fields_t *)
                         BCMCTH_TRUNK_VP_TBL_LTD_FIELDS
                                  (unit, group,
                                  BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY);
    BCMCTH_TRUNK_VP_FREE(olt_fids);

    /* Free memory allocated for storing TRUNK_VP LT logical fields data. */
    olt_fids = (bcmcth_trunk_vp_grp_ltd_fields_t *)
                         BCMCTH_TRUNK_VP_TBL_LTD_FIELDS
                                   (unit, group,
                                   BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0);
    BCMCTH_TRUNK_VP_FREE(olt_fids);

    /*
     * Free memory allocated for storing TRUNK_VP LT logical fields
     * attributes information.
     */
    ogrp_attrs = (bcmcth_trunk_vp_lt_field_attrs_t *)
                        BCMCTH_TRUNK_VP_LT_FIELD_ATTRS_VOID_PTR
                                  (unit, group,
                                  BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY);
    BCMCTH_TRUNK_VP_FREE(ogrp_attrs);

    /*
     * Free memory allocated for storing TRUNK_VP_WEIGHTED_0 LT logical fields
     * attributes information.
     */
    ogrp_attrs = (bcmcth_trunk_vp_lt_field_attrs_t *)
                      BCMCTH_TRUNK_VP_LT_FIELD_ATTRS_VOID_PTR
                                   (unit, group,
                                   BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0);
    BCMCTH_TRUNK_VP_FREE(ogrp_attrs);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Cleanup Trident4 device specific TRUNK_VP Table fields information.
 *
 * Cleanup TRUNK_VP Group and Member tables fields information.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcm56780_a0_trunk_vp_fields_cleanup(int unit)
{

    SHR_FUNC_ENTER(unit);

    /* Cleanup TRUNK_VP LT fields information. */
    bcm56780_a0_trunk_vp_olay_grps_flds_cleanup(unit);

    SHR_FUNC_EXIT();
}
/*!
 * \brief TRUNK_VP Group and Member table entries initialization function.
 *
 * Allocates memory to store TRUNK_VP Group and Member table entries usage
 * information.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Table entries memory allocation failed.
 */
static int
bcm56780_a0_trunk_vp_db_clear(int unit)
{
    /* Pointer to TRUNK_VP hardware table info. */
    bcmcth_trunk_vp_tbl_prop_t *tbl_ptr;

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "%s.\n"), __func__));
    /*
     * Don't free the HA memory, initialize all HA pointer to NULL for
     * cleanup.
     */
    /* Clear TRUNK_VP group logical table active array base pointers. */
    BCMCTH_TRUNK_VP_GRP_INFO_PTR(unit) = NULL;


    /* Clear Overlay Member table entries base pointer. */
    tbl_ptr = BCMCTH_TRUNK_VP_TBL_PTR
                 (unit, member, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY);
    tbl_ptr->ent_arr = NULL;

    /* Clear Overlay Group table entries base pointer. */
    tbl_ptr = BCMCTH_TRUNK_VP_TBL_PTR
                  (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY);
    tbl_ptr->ent_arr = NULL;

    /* Clear weighted TRUNK_VP Member table entries base pointer. */
    tbl_ptr = BCMCTH_TRUNK_VP_TBL_PTR
                  (unit, member, BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0);
    tbl_ptr->ent_arr = NULL;

    /* Clear weighted TRUNK_VP Group table entries base pointer. */
    tbl_ptr = BCMCTH_TRUNK_VP_TBL_PTR
                  (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0);
    tbl_ptr->ent_arr = NULL;

    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear device specfic TRUNK_VP tables range information.
 *
 * Clear TRUNK_VP tables size, entry size and valid index range details.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcm56780_a0_trunk_vp_hw_tbls_range_clear(int unit)
{

    SHR_FUNC_ENTER(unit);

    /* Clear device TRUNK_VP config register entry size in words value. */
    BCMCTH_TRUNK_VP_REG_ENT_SIZE(unit, config) = 0;

    /* Clear Max hardware tables entry size info. */
    BCMCTH_TRUNK_VP_TBL_ENT_SIZE_MAX(unit) = 0;

    /* Clear Overlay TRUNK_VP tables entry size value. */
    BCMCTH_TRUNK_VP_TBL_ENT_SIZE
        (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) = 0;
    BCMCTH_TRUNK_VP_TBL_ENT_SIZE
        (unit, member, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) = 0;

    /* Clear Overlay TRUNK_VP Member table hardware data. */
    BCMCTH_TRUNK_VP_TBL_IDX_MIN
        (unit, member, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) =
               BCMCTH_TRUNK_VP_INVALID;
    BCMCTH_TRUNK_VP_TBL_IDX_MAX
        (unit, member, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) =
               BCMCTH_TRUNK_VP_INVALID;
    BCMCTH_TRUNK_VP_TBL_IDX_MAX_USED
        (unit, member, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) =
               BCMCTH_TRUNK_VP_INVALID;
    BCMCTH_TRUNK_VP_TBL_SIZE
        (unit, member, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) = 0;

    /* Clear Overlay TRUNK_VP Group table hardware data. */
    BCMCTH_TRUNK_VP_TBL_IDX_MIN
        (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) =
               BCMCTH_TRUNK_VP_INVALID;
    BCMCTH_TRUNK_VP_TBL_IDX_MAX
        (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) =
               BCMCTH_TRUNK_VP_INVALID;
    BCMCTH_TRUNK_VP_TBL_IDX_MAX_USED
        (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) =
               BCMCTH_TRUNK_VP_INVALID;
    BCMCTH_TRUNK_VP_TBL_SIZE(unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) = 0;

    /* Clear weighted TRUNK_VP tables entry size value. */
    BCMCTH_TRUNK_VP_TBL_ENT_SIZE
        (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0) = 0;
    BCMCTH_TRUNK_VP_TBL_ENT_SIZE
        (unit, member, BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0) = 0;

    /* Clear weighted TRUNK_VP Member table hardware data. */
    BCMCTH_TRUNK_VP_TBL_IDX_MIN
        (unit, member, BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0) =
               BCMCTH_TRUNK_VP_INVALID;
    BCMCTH_TRUNK_VP_TBL_IDX_MAX
        (unit, member, BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0) =
               BCMCTH_TRUNK_VP_INVALID;
    BCMCTH_TRUNK_VP_TBL_IDX_MAX_USED
        (unit, member, BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0) =
               BCMCTH_TRUNK_VP_INVALID;
    BCMCTH_TRUNK_VP_TBL_SIZE(unit,
               member, BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0) =
               0;

    /* Clear weighted TRUNK_VP Group table hardware data. */
    BCMCTH_TRUNK_VP_TBL_IDX_MIN
        (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0) =
               BCMCTH_TRUNK_VP_INVALID;
    BCMCTH_TRUNK_VP_TBL_IDX_MAX
        (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0) =
               BCMCTH_TRUNK_VP_INVALID;
    BCMCTH_TRUNK_VP_TBL_IDX_MAX_USED
        (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0) =
               BCMCTH_TRUNK_VP_INVALID;
    BCMCTH_TRUNK_VP_TBL_SIZE(unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0) =
               0;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_trunk_vp_itbm_lists_cleanup(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear TRUNK_VP tables LRD-SID and DRD-SID information.
 *
 * Clear TRUNK_VP Group, Member and Round-Robin load balancing hardware tables
 * LRD-SID and DRD-SID information.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcm56780_a0_trunk_vp_sids_clear(int unit)
{
    SHR_FUNC_ENTER(unit);

    /* Clear device TRUNK_VP config register DRD and LTD SID values. */
    BCMCTH_TRUNK_VP_REG_DRD_SID(unit, config) = INVALIDr;
    BCMCTH_TRUNK_VP_REG_LTD_SID(unit, config) = BCMLTD_SID_INVALID;

    /*
     * Clear Post-IFP Overlay TRUNK_VP tables DRD-SID and LTD-SID
     * values.
     */
    BCMCTH_TRUNK_VP_TBL_DRD_SID
        (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) = INVALIDm;
    BCMCTH_TRUNK_VP_TBL_DRD_SID
        (unit, member, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) = INVALIDm;
    BCMCTH_TRUNK_VP_TBL_LTD_SID
        (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY) = BCMLTD_SID_INVALID;

    BCMCTH_TRUNK_VP_TBL_DRD_SID
        (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0) = INVALIDm;
    BCMCTH_TRUNK_VP_TBL_DRD_SID
        (unit, member, BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0) = INVALIDm;
    BCMCTH_TRUNK_VP_TBL_LTD_SID
        (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0) = BCMLTD_SID_INVALID;

    SHR_FUNC_EXIT();
}

/*!
 * \brief Clears Trident4 device specific TRUNK_VP tables information.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcm56780_a0_trunk_vp_tables_clear(int unit)
{

    SHR_FUNC_ENTER(unit);

    /* Clear TRUNK_VP tables logical and physical field details. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_trunk_vp_fields_cleanup(unit));

    /* Clear TRUNK_VP hardware tables size and index range info. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_trunk_vp_hw_tbls_range_clear(unit));

    /* Clear TRUNK_VP tables DRD-SID and LTD-SID details. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_trunk_vp_sids_clear(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief TRUNK_VP Group and Member table entries initialization function.
 *
 * Allocates memory to store TRUNK_VP Group and Member table entries usage
 * information.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Table entries memory allocation failed.
 */
static int
bcm56780_a0_trunk_vp_abort_db_clear(int unit)
{
    /* Pointer to TRUNK_VP hardware table info. */
    bcmcth_trunk_vp_tbl_prop_t *tbl_ptr;

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "%s.\n"), __func__));
    /*
     * Don't free the HA memory, initialize all HA pointer to NULL for
     * cleanup.
     */
    /*
     * Clear TRUNK_VP group logical table active
     * and backup arrays base pointers.
     */
    BCMCTH_TRUNK_VP_GRP_INFO_BK_PTR(unit) = NULL;

    /* Clear Overlay Member table entries array base pointer. */
    tbl_ptr = BCMCTH_TRUNK_VP_TBL_PTR
              (unit, member, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY);
    tbl_ptr->ent_bk_arr = NULL;

    /* Clear Overlay Group table entries array base pointer. */
    tbl_ptr = BCMCTH_TRUNK_VP_TBL_PTR
             (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY);
    tbl_ptr->ent_bk_arr = NULL;

    /* Clear weighted TRUNK_VP Member table entries array base pointer. */
    tbl_ptr = BCMCTH_TRUNK_VP_TBL_PTR
             (unit, member, BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0);
    tbl_ptr->ent_bk_arr = NULL;

    /* Clear weighted TRUNK_VP Group table entries array base pointer. */
    tbl_ptr = BCMCTH_TRUNK_VP_TBL_PTR
              (unit, group, BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0);
    tbl_ptr->ent_bk_arr = NULL;

    SHR_FUNC_EXIT();
}

/*!
 * \brief TRUNK_VP Group and Member table entries cleanup in itbm list.
 *
 * Cleanup entries in itbm list for TRUNK_VP Group
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcm56780_a0_trunk_vp_itbm_groups_cleanup(int unit)
{
    bcmcth_trunk_vp_id_t trunk_vp_id = BCMCTH_TRUNK_VP_INVALID;
    bcmcth_trunk_vp_id_t trunk_vp_id_temp = BCMCTH_TRUNK_VP_INVALID;
    bool in_use = FALSE;
    int gtype = BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY;
    uint32_t comp_id = 0;

    SHR_FUNC_ENTER(unit);

    for (trunk_vp_id_temp = BCMCTH_TRUNK_VP_TBL_IDX_MIN(unit, group, gtype);
         trunk_vp_id_temp <= BCMCTH_TRUNK_VP_TBL_IDX_MAX(unit, group, gtype);
         trunk_vp_id_temp++) {
        trunk_vp_id = trunk_vp_id_temp;
        SHR_IF_ERR_VERBOSE_EXIT(
           bcmcth_trunk_vp_grp_in_use(unit,
                                      trunk_vp_id,
                                      gtype,
                                      &in_use,
                                      &comp_id));
        if (in_use && (comp_id == BCMMGMT_TRUNK_COMP_ID)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (shr_itbm_list_block_free(BCMCTH_TRUNK_VP_TBL_ITBM_LIST
                    (BCMCTH_TRUNK_VP_TBL_PTR(unit, member, gtype)),
                    BCMCTH_TRUNK_VP_GRP_MAX_MEMBERS(unit, trunk_vp_id),
                    SHR_ITBM_INVALID,
                    BCMCTH_TRUNK_VP_GRP_MEMB_TBL_START_IDX(unit, trunk_vp_id)));
            SHR_IF_ERR_VERBOSE_EXIT
                (shr_itbm_list_block_free(BCMCTH_TRUNK_VP_TBL_ITBM_LIST
                    (BCMCTH_TRUNK_VP_TBL_PTR(unit, group, gtype)),
                    1,
                    SHR_ITBM_INVALID,
                    trunk_vp_id));
        }
    }
exit:
    if (SHR_FUNC_ERR()) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Error: RM cleaning up groups")));
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief TRUNK_VP Group and Member table entries cleanup function.
 *
 * Cleanup memory allocated to store TRUNK_VP Group and
 * Member table entries usage information.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcm56780_a0_trunk_vp_entries_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(
      bcm56780_a0_trunk_vp_itbm_groups_cleanup(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_trunk_vp_db_clear(unit));

    /* Clear atomic transaction related SW database info. */
    if (BCMCTH_TRUNK_VP_ATOMIC_TRANS(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_trunk_vp_abort_db_clear(unit));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Trident4 device TRUNK_VP hardware table details cleanup function.
 *
 * Cleanup Trident4 TRUNK_VP hardware table information and
 * free memory allocated
 * to store this information.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56780_a0_trunk_vp_info_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    /*
     * Cleanup memory allocated for TRUNK_VP Group and
     * Member table entries info.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_trunk_vp_entries_cleanup(unit));

    /* Clear TRUNK_VP tables information stored in SW. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_trunk_vp_tables_clear(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an TRUNK_VP Group into group and member tables
 * using DMA mechanism.
 *
 * Delete TRUNK_VP Group Logical Table entry in hardware Group and Member
 * tables.
 *
 * \param [in] unit Unit number.
 * \param [in] info Pointer to TRUNK_VP Logical Table entry data.
 * \param [in] vp_drv_var VP device variant info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56780_a0_trunk_vp_grp_dma_del(int unit,
                                 void *info)
{
    bcmcth_trunk_vp_grp_type_t gtype = BCMCTH_TRUNK_VP_GRP_TYPE_COUNT;
    bcmcth_trunk_vp_lt_entry_t *lt_ent = NULL;    /* LT entry data pointer. */
    bcmcth_trunk_vp_pt_op_info_t op_info;   /* Physical Table operation info. */
    uint32_t *pt_entry_buff = NULL; /* Local entry buffer poiner. */
    uint32_t *dma_buf = NULL;       /* Local DMA buffer poiner. */
    uint32_t dma_alloc_sz = 0;      /* Local DMA buffer alloc size. */
    /* Grp start index in Member table. */
    int mstart_idx = BCMCTH_TRUNK_VP_INVALID;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    /* Get LT entry data pointer. */
    lt_ent = (bcmcth_trunk_vp_lt_entry_t *)info;
    /* Get TRUNK_VP Group type. */
    gtype = lt_ent->grp_type;

    /* Allocate hardware entry buffer. */
    BCMCTH_TRUNK_VP_ALLOC(pt_entry_buff,
                  BCMCTH_TRUNK_VP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t),
                  "bcmcthTrunkVBcm56780GrpDmaInsPtEntBuf");

    dma_alloc_sz = (BCMCTH_TRUNK_VP_TBL_ENT_SIZE
                        (unit, member, gtype) * sizeof(uint32_t) *
                         lt_ent->max_members);
    BCMCTH_TRUNK_VP_ALLOC(dma_buf,
          dma_alloc_sz, "bcmcthTrunkVpBcm56780GrpDmaInsPtDmaBuf");

    /* Member Table Group Start Index. */
    mstart_idx = lt_ent->mstart_idx;

    /* Initialize op_info structure member values. */
    bcm56780_a0_pt_op_info_t_init(&op_info);

    /* Prepare for PT entry write in Member table. */
    op_info.ecount = lt_ent->max_members;
    op_info.req_lt_sid = lt_ent->glt_sid;
    op_info.op = BCMPTM_OP_WRITE;
    op_info.dyn_info.index = mstart_idx;
    op_info.op_arg = lt_ent->op_arg;
    op_info.drd_sid = BCMCTH_TRUNK_VP_TBL_DRD_SID(unit, member, gtype);
    op_info.wsize = (BCMCTH_TRUNK_VP_TBL_ENT_SIZE
                     (unit, member, gtype) * op_info.ecount);
    op_info.dma = TRUE;


    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_trunk_vp_pt_write(unit, &op_info, dma_buf));

    /* Initialize op_info structure member values. */
    bcm56780_a0_pt_op_info_t_init(&op_info);

    /* Program TRUNK_VP Group table. */
    op_info.op_arg = lt_ent->op_arg;
    op_info.req_lt_sid = lt_ent->glt_sid;
    op_info.op = BCMPTM_OP_WRITE;
    op_info.dyn_info.index = (lt_ent->trunk_vp_id);
    op_info.wsize = BCMCTH_TRUNK_VP_TBL_ENT_SIZE(unit, group, gtype);
    op_info.drd_sid = BCMCTH_TRUNK_VP_TBL_DRD_SID(unit, group, gtype);

    /* Clear entry buffer. */
    sal_memset(pt_entry_buff, 0,
               BCMCTH_TRUNK_VP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t));

     /* Write entry to TRUNK_VP GROUP table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_trunk_vp_pt_write(unit, &op_info, pt_entry_buff));

exit:
    if (pt_entry_buff != NULL) {
        BCMCTH_TRUNK_VP_FREE(pt_entry_buff);
        pt_entry_buff = NULL;
    }
    if (dma_buf != NULL) {
        BCMCTH_TRUNK_VP_FREE(dma_buf);
        dma_buf = NULL;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an TRUNK_VP Group from group and member tables
 * using PIO mechanism.
 *
 * Deletes an TRUNK_VP Group Logical Table entry from hardware Group and Member
 * tables. Entries are deleted from Post-IFP TRUNK_VP Group and Member Tables.
 *
 * \param [in] unit Unit number.
 * \param [in] info Pointer to TRUNK_VP Logical Table entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56780_a0_trunk_vp_grp_pio_del(int unit,
                             void *info)
{
    bcmcth_trunk_vp_lt_entry_t *lt_ent = NULL; /* LT entry pointer. */
    uint32_t *pt_entry_buff = NULL;    /* Ptr to physical table entry buf. */
    bcmcth_trunk_vp_pt_op_info_t op_info; /* Physical table operation info. */
    uint32_t arr_idx;             /* NHOP array index iterator. */
    int start_idx = BCMCTH_TRUNK_VP_INVALID;   /* Member table start index. */
    uint32_t max_members;                /* Group MAX_MEMBERS value. */
    bcmcth_trunk_vp_grp_type_t gtype = BCMCTH_TRUNK_VP_GRP_TYPE_COUNT;
    uint16_t trunk_vp_id = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcm56780_a0_trunk_vp_grp_pio_del.\n")));

    /* Get LT entry data pointer. */
    lt_ent = (bcmcth_trunk_vp_lt_entry_t *)info;

    /* Get TRUNK_VP Group type. */
    gtype = lt_ent->grp_type;

    /* Allocate hardware entry buffer. */
    BCMCTH_TRUNK_VP_ALLOC(pt_entry_buff,
                  BCMCTH_TRUNK_VP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t),
                  "bcmcthTrunkVpBcm56780GrpPioDelPtBuf");

    /* Get Group start index in TRUNK_VP Member table. */
    start_idx =  lt_ent->mstart_idx;

    /* Get MAX_MEMBERS value of the group. */
    max_members = lt_ent->max_members;

    trunk_vp_id = lt_ent->trunk_vp_id;

    if (start_idx > (int) BCMCTH_TRUNK_VP_TBL_IDX_MAX(unit, member, gtype)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    op_info.drd_sid = BCMCTH_TRUNK_VP_TBL_DRD_SID(unit, member, gtype);

    /* Initialize op_info structure member values. */
    bcm56780_a0_pt_op_info_t_init(&op_info);
    op_info.dyn_info.index = (start_idx);
    op_info.op_arg = lt_ent->op_arg;
    for (arr_idx = 0;
         arr_idx < max_members;
         arr_idx++, op_info.dyn_info.index++) {
        /* Update Post-IFP TRUNK_VP Member table DRD SID value and word size.*/
        op_info.req_lt_sid = lt_ent->glt_sid;
        op_info.op = BCMPTM_OP_WRITE;
        op_info.drd_sid = BCMCTH_TRUNK_VP_TBL_DRD_SID(unit, member, gtype);
        op_info.wsize = BCMCTH_TRUNK_VP_TBL_ENT_SIZE(unit, member, gtype);

        if ((op_info.drd_sid ==
             BCMCTH_TRUNK_VP_TBL_DRD_SID(unit, member, gtype)) &&
            (op_info.dyn_info.index >=
             (int)(BCMCTH_TRUNK_VP_TBL_IDX_MAX(unit, member, gtype)))) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        /* Write entry to Post-IFP Member table. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_trunk_vp_pt_write(unit, &op_info, pt_entry_buff));
    }

    /* Delete Group entry in Post-IFP TRUNK_VP Group Table. */
    op_info.dyn_info.index = trunk_vp_id;
    op_info.drd_sid = BCMCTH_TRUNK_VP_TBL_DRD_SID(unit, group, gtype);
    op_info.wsize = BCMCTH_TRUNK_VP_TBL_ENT_SIZE(unit, group, gtype);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_trunk_vp_pt_write(unit, &op_info, pt_entry_buff));

exit:
    if (pt_entry_buff != NULL) {
        BCMCTH_TRUNK_VP_FREE(pt_entry_buff);
        pt_entry_buff = NULL;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Insert an TRUNK_VP Group into group and member tables
 * using DMA mechanism.
 *
 * Installs an TRUNK_VP Group Logical Table entry into hardware Group and Member
 * tables. Entries are installed in Post-IFP TRUNK_VP Group and Member Tables.
 *
 * \param [in] unit Unit number.
 * \param [in] info Pointer to TRUNK_VP Logical Table entry data.
 * \param [in] vp_drv_var VP device variant info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56780_a0_trunk_vp_grp_dma_ins(int unit,
                                 void *info,
                                 const bcmcth_trunk_vp_drv_var_t *vp_drv_var)
{
    bcmcth_trunk_vp_grp_type_t gtype = BCMCTH_TRUNK_VP_GRP_TYPE_COUNT;
    bcmcth_trunk_vp_lt_entry_t *lt_ent = NULL;    /* LT entry data pointer. */
    bcmcth_trunk_vp_pt_op_info_t op_info;   /* Physical Table operation info. */
    bcmcth_trunk_vp_grp_drd_fields_t *grp_flds;   /* Group table DRD fields. */
    uint32_t *pt_entry_buff = NULL; /* Local entry buffer poiner. */
    uint32_t *dma_buf = NULL;       /* Local DMA buffer poiner. */
    uint32_t dma_alloc_sz = 0;      /* Local DMA buffer alloc size. */
    uint32_t hw_npaths = 0;         /* HW (zero based) member_cnt value. */
    /* Grp start index in Member table. */
    int mstart_idx = BCMCTH_TRUNK_VP_INVALID;
    uint32_t arr_idx;                     /* array index variable. */
    uint32_t val[1] = {0};                     /* Temporary local variable. */
    uint32_t tmp_val = 0;                     /* Temporary local variable. */
    uint32_t ent_size = 0;                /* Table entry size. */
    uint32_t minbit = 0, maxbit = 0;
    int i;
    const bcmcth_trunk_vp_member_info_t *member_info;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    /* Get LT entry data pointer. */
    lt_ent = (bcmcth_trunk_vp_lt_entry_t *)info;
    member_info = vp_drv_var->member0_info;
    /* Get TRUNK_VP Group type. */
    gtype = lt_ent->grp_type;

    /* Allocate hardware entry buffer. */
    BCMCTH_TRUNK_VP_ALLOC(pt_entry_buff,
                  BCMCTH_TRUNK_VP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t),
                  "bcmcthTrunkVBcm56780GrpDmaInsPtEntBuf");

    dma_alloc_sz = (BCMCTH_TRUNK_VP_TBL_ENT_SIZE
                        (unit, member, gtype) * sizeof(uint32_t) *
                         lt_ent->max_members);
    BCMCTH_TRUNK_VP_ALLOC(dma_buf,
          dma_alloc_sz, "bcmcthTrunkVpBcm56780GrpDmaInsPtDmaBuf");

    /*
     * Program the Member table entries in hardware.
     * Post-IFP Table.
     */

    /* Member Table Group Start Index. */
    mstart_idx = lt_ent->mstart_idx;


    if (lt_ent->event == BCMIMM_ENTRY_UPDATE) {
        /* Read the data from the PT into the dma_buffers. */
        /* Initialize op_info structure member values. */
        if (lt_ent->member_cnt) {
            /*
             * Prepare for member table entries read operation
             * BCMPTM_OP_READ for the valid member_cnt.
             * During update operation, use member_cnt supplied by user or
             * backup grp entry to read the member information from HW.
             * During insert operation, use max_members to write into member
             * table and to reset the entries not valid and > member_cnt.
             * For update operation after member_cnt valid member entries
             * are read from hardware the remaining members > member_cnt are
             * reset.
             */
            bcm56780_a0_pt_op_info_t_init(&op_info);
            op_info.op_arg = lt_ent->op_arg;
            op_info.req_lt_sid = lt_ent->glt_sid;
            op_info.op = BCMPTM_OP_READ;
            op_info.drd_sid = BCMCTH_TRUNK_VP_TBL_DRD_SID(unit, member, gtype);
            op_info.dyn_info.index = mstart_idx;
            op_info.dma = TRUE;
            op_info.ecount = lt_ent->member_cnt;
            op_info.wsize = (BCMCTH_TRUNK_VP_TBL_ENT_SIZE
                             (unit, member, gtype) * op_info.ecount);

            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56780_a0_trunk_vp_pt_read(unit, &op_info, dma_buf));

        }
    }
    /* Initialize op_info structure member values. */
    bcm56780_a0_pt_op_info_t_init(&op_info);

    /* member table entry size. */
    ent_size = BCMCTH_TRUNK_VP_TBL_ENT_SIZE(unit, member, gtype);

    for (arr_idx = 0; arr_idx < lt_ent->member_cnt; arr_idx++) {
        /* Set member field values. */
        for (i = 0; i < (member_info->flds_count); ++i) {
            minbit = member_info->flds[i].start_bit;
            maxbit = member_info->flds[i].end_bit;
            val[0] = (uint32_t)lt_ent->member0_field[i][arr_idx];
            if (val[0] != (uint32_t) (BCMCTH_TRUNK_INVALID)) {
                shr_bitop_range_copy(&dma_buf[arr_idx * ent_size],
                   minbit, val, 0, maxbit - minbit + 1);
            }
        }

    }

    /* Prepare for PT entry write in Member table. */
    op_info.ecount = lt_ent->max_members;
    op_info.req_lt_sid = lt_ent->glt_sid;
    op_info.op = BCMPTM_OP_WRITE;
    op_info.dyn_info.index = mstart_idx;
    op_info.op_arg = lt_ent->op_arg;
    op_info.drd_sid = BCMCTH_TRUNK_VP_TBL_DRD_SID(unit, member, gtype);
    op_info.wsize = (BCMCTH_TRUNK_VP_TBL_ENT_SIZE
                     (unit, member, gtype) * op_info.ecount);
    op_info.dma = TRUE;


    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_trunk_vp_pt_write(unit, &op_info, dma_buf));

    /* Initialize op_info structure member values. */
    bcm56780_a0_pt_op_info_t_init(&op_info);

    /* Program TRUNK_VP Group table. */
    op_info.op_arg = lt_ent->op_arg;
    op_info.req_lt_sid = lt_ent->glt_sid;
    op_info.op = BCMPTM_OP_WRITE;
    op_info.dyn_info.index = (lt_ent->trunk_vp_id);
    op_info.wsize = BCMCTH_TRUNK_VP_TBL_ENT_SIZE(unit, group, gtype);
    op_info.drd_sid = BCMCTH_TRUNK_VP_TBL_DRD_SID(unit, group, gtype);

    grp_flds = (bcmcth_trunk_vp_grp_drd_fields_t *)
                      BCMCTH_TRUNK_VP_TBL_DRD_FIELDS
                               (unit, group, gtype);
    SHR_NULL_CHECK(grp_flds, SHR_E_INTERNAL);

    /* Clear entry buffer. */
    sal_memset(pt_entry_buff, 0,
               BCMCTH_TRUNK_VP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t));

    /* Set LB_MODE value. */
    if (BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0 != gtype) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_trunk_vp_get_hw_lb_mode(unit,
              lt_ent->lb_mode, &tmp_val));
    } else {
        tmp_val = lt_ent->weighted_size;
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_pt_field_set(unit,
                             op_info.drd_sid,
                             pt_entry_buff,
                             grp_flds->lb_mode,
                             &(tmp_val)));

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "\n inserted dma at %u %u \n"),
                mstart_idx, lt_ent->mstart_idx));
    /*
     * If member_cnt value is non-zero, decrement it by '1'
     * as hardware count field value is zero base.
     */
    if (lt_ent->member_cnt && BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0 != gtype) {
        hw_npaths = (uint32_t) lt_ent->member_cnt - 1;
    }
    /* Set members COUNT value. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_pt_field_set(unit,
                             op_info.drd_sid,
                             pt_entry_buff,
                             grp_flds->count,
                             &(hw_npaths)));

    /* Set Group's member table base pointer. */
    tmp_val = (uint32_t)mstart_idx;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_pt_field_set(unit,
                             op_info.drd_sid,
                             pt_entry_buff,
                             grp_flds->base_ptr,
                             &(tmp_val)));

    /* Write entry to TRUNK_VP GROUP table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_trunk_vp_pt_write(unit, &op_info, pt_entry_buff));

exit:
    if (pt_entry_buff != NULL) {
        BCMCTH_TRUNK_VP_FREE(pt_entry_buff);
        pt_entry_buff = NULL;
    }
    if (dma_buf != NULL) {
        BCMCTH_TRUNK_VP_FREE(dma_buf);
        dma_buf = NULL;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Insert an TRUNK_VP Group into group and member tables
 * using PIO mechanism.
 *
 * Installs an TRUNK_VP Group Logical Table entry into hardware Group and Member
 * tables. Entries are installed in Post-IFP TRUNK_VP Group and Member Tables.
 *
 * \param [in] unit Unit number.
 * \param [in] info Pointer to TRUNK_VP Logical Table entry data.
 * \param [in] vp_drv_var VP device variant info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56780_a0_trunk_vp_grp_pio_ins(int unit,
                                 void *info,
                                 const bcmcth_trunk_vp_drv_var_t *vp_drv_var)
{
    bcmcth_trunk_vp_grp_type_t gtype = BCMCTH_TRUNK_VP_GRP_TYPE_COUNT;
    bcmcth_trunk_vp_lt_entry_t *lt_ent = NULL;/* LT entry data pointer. */
    bcmcth_trunk_vp_pt_op_info_t op_info;/* Physical Table operation info. */
    bcmcth_trunk_vp_grp_drd_fields_t *grp_flds;/* Group table DRD fields. */
    uint32_t *pt_entry_buff = NULL, *entry_buff = NULL;
    uint32_t hw_npaths = 0; /* HW (zero based) member_cnt value. */
    int mstart_idx = BCMCTH_TRUNK_VP_INVALID;
    uint32_t arr_idx; /* NHOP array index variable. */
    uint32_t val[1] = {0}; /* Temporary local variable. */
    uint32_t tmp_val = 0; /* Temporary local variable. */
    uint32_t minbit = 0, maxbit = 0;
    const bcmcth_trunk_vp_member_info_t *member_info;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(info, SHR_E_PARAM);


    /* Get LT entry data pointer. */
    lt_ent = (bcmcth_trunk_vp_lt_entry_t *)info;
    member_info = vp_drv_var->member0_info;
    /* Get TRUNK_VP Group type. */
    gtype = lt_ent->grp_type;

    /* Allocate hardware entry buffer. */
    BCMCTH_TRUNK_VP_ALLOC(pt_entry_buff,
                  BCMCTH_TRUNK_VP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t),
                  "bcmcthTrunkVpBcm56780GrpPioInsPtBuf");

    BCMCTH_TRUNK_VP_ALLOC(entry_buff,
                  BCMCTH_TRUNK_VP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t),
                  "bcmcthTrunkVpBcm56780GrpPioInsPtBuf");

    /*
     * Program the Member table entries in hardware.
     * 1) Pre-IFP Table.
     * 2) Post-IFP Table.
     */

    /* Member Table Group Start Index. */
    mstart_idx = lt_ent->mstart_idx;

    /* Initialize op_info structure member values. */
    bcm56780_a0_pt_op_info_t_init(&op_info);
    op_info.op_arg = lt_ent->op_arg;

    if (mstart_idx > BCMCTH_TRUNK_VP_TBL_IDX_MAX(unit, member, gtype)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    op_info.drd_sid = BCMCTH_TRUNK_VP_TBL_DRD_SID(unit, member, gtype);
    op_info.dyn_info.index = (mstart_idx);
    for (arr_idx = 0; arr_idx < lt_ent->member_cnt;
         arr_idx++, op_info.dyn_info.index++) {
        int i;
        sal_memset(pt_entry_buff, 0,
                   BCMCTH_TRUNK_VP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t));

        sal_memset(entry_buff, 0,
                   BCMCTH_TRUNK_VP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t));
        for (i = 0; i < (member_info->flds_count); ++i) {
            minbit = member_info->flds[i].start_bit;
            maxbit = member_info->flds[i].end_bit;
            val[0] = (uint32_t)lt_ent->member0_field[i][arr_idx];
            shr_bitop_range_copy(
              entry_buff, minbit, val, 0, maxbit - minbit + 1);
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "\n pio_ins fld %d min %u max %u val %u.\n"),
                     i, minbit, maxbit, val[0]));
        }

        LOG_INFO(BSL_LOG_MODULE,
            (BSL_META_U(unit, "\nWriting %u at %d"), entry_buff[0], arr_idx));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmdrd_pt_field_set(
                             unit,
                             BCMCTH_TRUNK_VP_TBL_DRD_SID(unit, member, gtype),
                             pt_entry_buff,
                             DATAf,
                             (entry_buff)));

        /* Prepare for PT entry write in Member table. */
        op_info.req_lt_sid = lt_ent->glt_sid;
        op_info.op = BCMPTM_OP_WRITE;
        op_info.op_arg = lt_ent->op_arg;
        op_info.wsize = BCMCTH_TRUNK_VP_TBL_ENT_SIZE(unit, member, gtype);

        if ((op_info.drd_sid ==
             BCMCTH_TRUNK_VP_TBL_DRD_SID(unit, member, gtype)) &&
            (op_info.dyn_info.index >
             BCMCTH_TRUNK_VP_TBL_IDX_MAX(unit, member, gtype))) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_trunk_vp_pt_write(unit, &op_info, pt_entry_buff));
    }


    /* Program TRUNK_VP Group table. */
    op_info.dyn_info.index = (lt_ent->trunk_vp_id);
    op_info.drd_sid = BCMCTH_TRUNK_VP_TBL_DRD_SID(unit, group, gtype);
    op_info.wsize = BCMCTH_TRUNK_VP_TBL_ENT_SIZE(unit, group, gtype);
    grp_flds = (bcmcth_trunk_vp_grp_drd_fields_t *)
                  BCMCTH_TRUNK_VP_TBL_DRD_FIELDS
                    (unit, group, gtype);
    SHR_NULL_CHECK(grp_flds, SHR_E_INTERNAL);

    /* Clear entry buffer. */
    sal_memset(pt_entry_buff, 0,
               BCMCTH_TRUNK_VP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t));

    /* Set LB_MODE vlaue. */
    if (BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0 != gtype) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56780_a0_trunk_vp_get_hw_lb_mode(unit,
               lt_ent->lb_mode, &tmp_val));
    } else {
        tmp_val = lt_ent->weighted_size - 1;
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_pt_field_set(unit,
                             op_info.drd_sid,
                             pt_entry_buff,
                             grp_flds->lb_mode,
                             &(tmp_val)));

    /*
     * If member_cnt value is non-zero, decrement it by '1'
     * as hardware count field value is zero base.
     */
    if (lt_ent->member_cnt && (BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0 != gtype)) {
        hw_npaths = (uint32_t) lt_ent->member_cnt - 1;
    }
    /* Set members COUNT value. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_pt_field_set(unit,
                             op_info.drd_sid,
                             pt_entry_buff,
                             grp_flds->count,
                             &(hw_npaths)));

    /* Set Group's member table base pointer. */
    tmp_val = (uint32_t)mstart_idx;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_pt_field_set(unit,
                             op_info.drd_sid,
                             pt_entry_buff,
                             grp_flds->base_ptr,
                             &(tmp_val)));

    /* Write entry to TRUNK_VP table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_trunk_vp_pt_write(unit, &op_info, pt_entry_buff));

exit:
    if (pt_entry_buff != NULL) {
        BCMCTH_TRUNK_VP_FREE(pt_entry_buff);
        pt_entry_buff = NULL;
    }
    if (entry_buff != NULL) {
        BCMCTH_TRUNK_VP_FREE(entry_buff);
        entry_buff = NULL;
    }
    SHR_FUNC_EXIT();
}


/*!
 * \brief Insert TRUNK_VP Group logical table entry into hardware tables.
 *
 * Installs TRUNK_VP Group Logical Table entry into hardware Group and Member
 * tables. Entries are installed in Pre-IFP and Post-IFP TRUNK_VP Group and
 * Member Tables.
 *
 * \param [in] unit Unit number.
 * \param [in] info Pointer to TRUNK_VP Logical Table entry data.
 * \param [in] drv_var device variant info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56780_a0_trunk_vp_grp_ins(int unit,
                             void *info,
                             const bcmcth_trunk_drv_var_t *drv_var)
{

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(info, SHR_E_PARAM);
    if (BCMCTH_TRUNK_VP_USE_DMA(unit)) {
        SHR_ERR_EXIT
            (bcm56780_a0_trunk_vp_grp_dma_ins(
                                unit,
                                info,
                                drv_var->trunk_vp_var));
    } else {
        SHR_ERR_EXIT
            (bcm56780_a0_trunk_vp_grp_pio_ins(
                                unit,
                                info,
                                drv_var->trunk_vp_var));
    }

exit:
    SHR_FUNC_EXIT();
}
/*!
 * \brief Delete TRUNK_VP Group logical table entry from hardware tables.
 *
 * Deletes TRUNK_VP Group Logical Table entry from hardware Group and Member
 * tables. Entries are deleted from Pre-IFP and Post-IFP TRUNK_VP Group and
 * Member Tables.
 *
 * \param [in] unit Unit number.
 * \param [in] info Pointer to TRUNK_VP Logical Table entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56780_a0_trunk_vp_grp_del(int unit,
                         void *info)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit,
                         "bcm56780_a0_trunk_vp_grp_del.\n")));

    if (BCMCTH_TRUNK_VP_USE_DMA(unit)) {
        SHR_ERR_EXIT
            (bcm56780_a0_trunk_vp_grp_dma_del(unit, info));
    } else {
        SHR_ERR_EXIT
            (bcm56780_a0_trunk_vp_grp_pio_del(unit, info));
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Copy TRUNK_VP member table entries from sfrag block to dfrag block.
 *
 * Copy TRUNK_VP member table entries from sfrag block to dfrag block.
 *
 * \param [in] unit Unit number.
 * \param [in] dfrag Pointer to TRUNK_VP Logical Table entry data.
 * \param [in] sfrag Pointer to TRUNK_VP Logical Table entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56780_a0_trunk_vp_memb_dma_cpy(int unit,
                              const void *mdest,
                              const void *msrc)
{
    const bcmcth_trunk_vp_grp_defrag_t *sfrag =
       (bcmcth_trunk_vp_grp_defrag_t *)msrc; /* Source entries block. */
    const bcmcth_trunk_vp_grp_defrag_t *dfrag =
       (bcmcth_trunk_vp_grp_defrag_t *)mdest; /* Destination entries block. */
    uint32_t *dma_buf = NULL;             /* Local DMA buffer poiner. */
    uint32_t dma_alloc_sz = 0;            /* Local DMA buffer alloc size. */
    bcmcth_trunk_vp_grp_type_t gtype = BCMCTH_TRUNK_VP_GRP_TYPE_COUNT;
    bcmcth_trunk_vp_pt_op_info_t op_info;
    bcmcth_trunk_vp_wal_info_t *wali = NULL; /* TRUNK_VP WAL entries info. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(sfrag, SHR_E_PARAM);
    SHR_NULL_CHECK(dfrag, SHR_E_PARAM);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "%s.\n"), __func__));

    /* Get and validate TRUNK_VP WALI pointer. */
    wali = BCMCTH_TRUNK_VP_WALI_PTR(unit);
    SHR_NULL_CHECK(wali, SHR_E_INTERNAL);

    /* Get the group type. */
    gtype = sfrag->gtype;

    /* Allocate the DMA buffer for reading Member table entries. */
    dma_alloc_sz = (BCMCTH_TRUNK_VP_TBL_ENT_SIZE
                        (unit, member, gtype) * sizeof(uint32_t) *
                         sfrag->gsize);
    BCMCTH_TRUNK_VP_ALLOC(dma_buf,
                    dma_alloc_sz, "bcmcthTrunkVp56780L3EcmpDmaCpy");

    /*
     * Prepare for member table entries read operation
     * i.e. BCMPTM_OP_READ.
     */
    bcm56780_a0_pt_op_info_t_init(&op_info);
    op_info.op_arg = sfrag->op_arg;
    op_info.req_lt_sid = sfrag->glt_sid;
    op_info.op = BCMPTM_OP_READ;
    op_info.drd_sid = BCMCTH_TRUNK_VP_TBL_DRD_SID(unit, member, gtype);
    op_info.dyn_info.index = sfrag->mstart_idx;
    op_info.dma = TRUE;
    op_info.ecount = sfrag->gsize;
    op_info.wsize = (BCMCTH_TRUNK_VP_TBL_ENT_SIZE
                     (unit, member, gtype) * op_info.ecount);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_trunk_vp_pt_read(unit, &op_info, dma_buf));

    /* Update WALI member table entries READ info. */
    if (wali->mwsize == 0) {
        wali->mwsize = BCMCTH_TRUNK_VP_TBL_ENT_SIZE(unit, member, gtype);
    }
    wali->mr_ecount += sfrag->gsize;

    /*
     * Prepare for L3_TRUNK_VP member table entries write operation
     * i.e. BCMPTM_OP_WRITE.
     */
    bcm56780_a0_pt_op_info_t_init(&op_info);
    op_info.op_arg = dfrag->op_arg;
    op_info.req_lt_sid = dfrag->glt_sid;
    op_info.op = BCMPTM_OP_WRITE;
    op_info.drd_sid = BCMCTH_TRUNK_VP_TBL_DRD_SID(unit, member, gtype);
    op_info.dyn_info.index = dfrag->mstart_idx;
    op_info.dma = TRUE;
    op_info.ecount = dfrag->gsize;
    op_info.wsize = (BCMCTH_TRUNK_VP_TBL_ENT_SIZE
                     (unit, member, gtype) * op_info.ecount);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_trunk_vp_pt_write(unit, &op_info, dma_buf));

    BCMCTH_TRUNK_VP_FREE(dma_buf);
    /* Update WALI member table entries write count. */
    wali->mw_ecount += dfrag->gsize;

exit:
    if (SHR_FUNC_ERR()) {
        if (dma_buf != NULL) {
            BCMCTH_TRUNK_VP_FREE(dma_buf);
            dma_buf = NULL;
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Copy TRUNK_VP member table entries from sfrag block to dfrag block.
 *
 * Copy TRUNK_VP member table entries from sfrag block to dfrag block.
 *
 * \param [in] unit Unit number.
 * \param [in] dfrag Pointer to TRUNK_VP Logical Table entry data.
 * \param [in] sfrag Pointer to TRUNK_VP Logical Table entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56780_a0_trunk_vp_memb_pio_cpy(int unit,
                              const void *mdest,
                              const void *msrc)
{
    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "%s.\n"), __func__));

    SHR_ERR_EXIT(SHR_E_UNAVAIL);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Copy TRUNK_VP member table entries from sfrag block to dfrag block.
 *
 * Copy TRUNK_VP member table entries from sfrag block to dfrag block.
 *
 * \param [in] unit Unit number.
 * \param [in] dfrag Pointer to TRUNK_VP Logical Table entry data.
 * \param [in] sfrag Pointer to TRUNK_VP Logical Table entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56780_a0_trunk_vp_memb_cpy(int unit,
                          const void *mdest,
                          const void *msrc)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(mdest, SHR_E_PARAM);
    SHR_NULL_CHECK(msrc, SHR_E_PARAM);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "%s.\n"), __func__));
    if (BCMCTH_TRUNK_VP_USE_DMA(unit)) {
        SHR_ERR_EXIT
            (bcm56780_a0_trunk_vp_memb_dma_cpy(unit, mdest, msrc));
    } else {
        SHR_ERR_EXIT
            (bcm56780_a0_trunk_vp_memb_pio_cpy(unit, mdest, msrc));
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear TRUNK_VP member tables.
 *
 * Clear TRUNK_VP member tables.
 *
 * \param [in] unit Unit number.
 * \param [out] info Pointer to TRUNK_VP Logical Table entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56780_a0_trunk_vp_memb_dma_clr(int unit,
                              const void *mfrag)
{
    const bcmcth_trunk_vp_grp_defrag_t *mclr =
             (bcmcth_trunk_vp_grp_defrag_t *)mfrag;
    uint32_t *dma_buf = NULL;          /* Local DMA buffer poiner. */
    uint32_t dma_alloc_sz = 0;         /* Local DMA buffer alloc size. */
    bcmcth_trunk_vp_pt_op_info_t op_info;  /* Physical table operation info. */
    bcmcth_trunk_vp_grp_type_t gtype = BCMCTH_TRUNK_VP_GRP_TYPE_COUNT;
    bcmcth_trunk_vp_wal_info_t *wali = NULL; /* TRUNK_VP WAL entries info. */

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(mclr, SHR_E_PARAM);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "bcm56780_a0_trunk_vp_memb_dma_clr.\n")));

    /* Get and validate TRUNK_VP WALI pointer. */
    wali = BCMCTH_TRUNK_VP_WALI_PTR(unit);
    SHR_NULL_CHECK(wali, SHR_E_INTERNAL);

    /* Get TRUNK_VP Group type. */
    gtype = mclr->gtype;

    /* Allocate buffer for L3_TRUNK_VP member table entries. */
    dma_alloc_sz = (BCMCTH_TRUNK_VP_TBL_ENT_SIZE
                        (unit, member, gtype) * sizeof(uint32_t) *
                         mclr->gsize);
    BCMCTH_TRUNK_VP_ALLOC(dma_buf,
              dma_alloc_sz, "bcmcthTrunkVp56780DmaClr");

    /* Initialize op_info structure member values. */
    bcm56780_a0_pt_op_info_t_init(&op_info);

    /* Update Post-IFP TRUNK_VP Member table DRD SID value and word size.*/
    op_info.req_lt_sid = mclr->glt_sid;
    op_info.op = BCMPTM_OP_WRITE;
    op_info.drd_sid = BCMCTH_TRUNK_VP_TBL_DRD_SID(unit, member, gtype);
    op_info.dyn_info.index = mclr->mstart_idx;
    op_info.dma = TRUE;
    op_info.ecount = mclr->gsize;
    op_info.op_arg = mclr->op_arg;
    op_info.wsize = (BCMCTH_TRUNK_VP_TBL_ENT_SIZE
                     (unit, member, gtype) * op_info.ecount);

    /* Write entry to Post-IFP Member table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_trunk_vp_pt_write(unit, &op_info, dma_buf));

    BCMCTH_TRUNK_VP_FREE(dma_buf);
    /* Update WALI member table entries write count. */
    wali->mclr_ecount += mclr->gsize;

exit:
    if (SHR_FUNC_ERR()) {
        if (dma_buf != NULL) {
            if (dma_buf != NULL) {
                BCMCTH_TRUNK_VP_FREE(dma_buf);
                dma_buf = NULL;
            }
        }
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get RH-TRUNK_VP group LT entry field values from the device tables.
 *
 * Get RH-TRUNK_VP group LT entry field values for the device ECMP group and
 * member tables.
 *
 * \param [in] unit Unit number.
 * \param [out] info Pointer to TRUNK_VP Logical Table entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56780_a0_trunk_vp_memb_pio_clr(int unit,
                             const void *mfrag)
{
    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "%s.\n"), __func__));

    SHR_ERR_EXIT(SHR_E_UNAVAIL);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get RH-TRUNK_VP group LT entry field values from the device tables.
 *
 * Get RH-TRUNK_VP group LT entry field values for the device ECMP group and
 * member tables.
 *
 * \param [in] unit Unit number.
 * \param [out] info Pointer to TRUNK_VP Logical Table entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56780_a0_trunk_vp_memb_clr(int unit,
                          const void *mfrag)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(mfrag, SHR_E_PARAM);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "%s.\n"), __func__));
    if (BCMCTH_TRUNK_VP_USE_DMA(unit)) {
        SHR_ERR_EXIT
            (bcm56780_a0_trunk_vp_memb_dma_clr(unit, mfrag));
    } else {
        SHR_ERR_EXIT
            (bcm56780_a0_trunk_vp_memb_pio_clr(unit, mfrag));
    }
exit:
    SHR_FUNC_EXIT();
}

/* This function is not used. Can be removed. */
/*!
 * \brief Update Group's member table start index in HW.
 *
 * Update Group's member table start index in HW
 *
 * \param [in] unit Unit number.
 * \param [out] info Pointer to TRUNK_VP Logical Table entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm56780_a0_trunk_vp_gmstart_updt(int unit,
                              const void *ginfo)
{

    const bcmcth_trunk_vp_grp_defrag_t *gupdt =
              (bcmcth_trunk_vp_grp_defrag_t *)ginfo;
    bcmcth_trunk_vp_grp_type_t gtype =
              BCMCTH_TRUNK_VP_GRP_TYPE_COUNT; /* Group type. */
    bcmcth_trunk_vp_id_t trunk_vp_id =
             BCMCTH_TRUNK_VP_INVALID; /* Group identifier. */
    bcmcth_trunk_vp_pt_op_info_t op_info; /* Physical Table operation info. */
    bcmcth_trunk_vp_grp_drd_fields_t *grp_flds;   /* Group table DRD fields. */
    uint32_t *pt_entry_buff = NULL;       /* Local entry buffer poiner. */
    uint32_t val = 0;                     /* Temporary local variable. */
    bcmcth_trunk_vp_wal_info_t *wali = NULL; /* TRUNK_VP WAL entries info. */
    bool in_use;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(ginfo, SHR_E_PARAM);

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "%s.\n"), __func__));
    /* Get and validate TRUNK_VP WALI pointer. */
    wali = BCMCTH_TRUNK_VP_WALI_PTR(unit);
    SHR_NULL_CHECK(wali, SHR_E_INTERNAL);

    /* Get the TRUNK_VP group type and validate it. */
    gtype = gupdt->gtype;
    if (gtype < BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY ||
        gtype > BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Invalid GTYPE=%d.\n"), gtype));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Get the TRUNK_VP_ID and validate it. */
    trunk_vp_id = gupdt->trunk_vp_id;

    if (trunk_vp_id < BCMCTH_TRUNK_VP_LT_MIN_TRUNK_VP_ID(unit, group, gtype)
    || trunk_vp_id > BCMCTH_TRUNK_VP_LT_MAX_TRUNK_VP_ID(unit, group, gtype)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Invalid TRUNK_VP_ID=%d.\n"), trunk_vp_id));
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Verify the group is in use. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_trunk_vp_grp_in_use(unit,
         gupdt->trunk_vp_id, gtype, &in_use, NULL));

    if (!in_use) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "TRUNK_VP_ID=%d not in-use.\n"), trunk_vp_id));
        SHR_ERR_EXIT(SHR_E_BADID);
    }

    /* Allocate hardware entry buffer. */
    BCMCTH_TRUNK_VP_ALLOC(pt_entry_buff,
                  BCMCTH_TRUNK_VP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t),
                  "bcmcthTrunkVp56780GrpMstartUpdt");

    /* Prepare for group table entry read operation. */
    bcm56780_a0_pt_op_info_t_init(&op_info);
    op_info.op_arg = gupdt->op_arg;
    op_info.req_lt_sid = gupdt->glt_sid;
    op_info.dyn_info.index = gupdt->trunk_vp_id;
    op_info.drd_sid = BCMCTH_TRUNK_VP_TBL_DRD_SID(unit, group, gtype);
    op_info.wsize = BCMCTH_TRUNK_VP_TBL_ENT_SIZE(unit, group, gtype);
    op_info.op = BCMPTM_OP_READ;

    /* Clear the entry buffer for storing the new group table entry. */
    sal_memset(pt_entry_buff, 0,
               BCMCTH_TRUNK_VP_TBL_ENT_SIZE_MAX(unit) * sizeof(uint32_t));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_trunk_vp_pt_read(unit, &op_info, pt_entry_buff));
    if (wali->gwsize == 0) {
        wali->gwsize = BCMCTH_TRUNK_VP_TBL_ENT_SIZE(unit, group, gtype);
    }
    wali->gr_ecount += 1;

    /* Prepare for table entry write operation. */
    bcm56780_a0_pt_op_info_t_init(&op_info);
    op_info.op_arg = gupdt->op_arg;
    op_info.req_lt_sid = gupdt->glt_sid;
    op_info.op = BCMPTM_OP_WRITE;
    op_info.dyn_info.index = (gupdt->trunk_vp_id);
    op_info.drd_sid = BCMCTH_TRUNK_VP_TBL_DRD_SID(unit, group, gtype);
    op_info.wsize = BCMCTH_TRUNK_VP_TBL_ENT_SIZE(unit, group, gtype);
    grp_flds = (bcmcth_trunk_vp_grp_drd_fields_t *)
                    BCMCTH_TRUNK_VP_TBL_DRD_FIELDS
                           (unit, group, gtype);
    SHR_NULL_CHECK(grp_flds, SHR_E_INTERNAL);

    val = (uint32_t)gupdt->mstart_idx;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_pt_field_set(unit,
                             op_info.drd_sid,
                             pt_entry_buff,
                             grp_flds->base_ptr,
                             &(val)));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_trunk_vp_pt_write(unit, &op_info, pt_entry_buff));
    wali->gw_ecount += 1;

exit:
    if (pt_entry_buff != NULL) {
        sal_free(pt_entry_buff);
        pt_entry_buff = NULL;
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief TRUNK_VP LT entry fields fill routine.
 *
 * Parse TRUNK_VP logical table entry logical fields
 * and store the data in lt_entry structure.
 *
 * \param [in] unit Unit number.
 * \param [in] in imm field informtion.
 * \param [in] vp_drv_var VP device variant info.
 * \param [in,out] lt_entry LT entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval !SHR_E_NONE Failed to convert \c in to \c lt_entry.
 */
static int
bcm56780_a0_trunk_vp_group_lt_fields_fill(
                                     int unit,
                                     const bcmltd_field_t *in,
                                     const bcmcth_trunk_vp_drv_var_t *vp_drv_var,
                                     bcmcth_trunk_vp_lt_entry_t *lt_entry)
{
    int fid;  /* Field identifier value. */
    uint64_t fval; /* Field value. */
    uint32_t idx;
    uint32_t num;
    uint64_t def_val;
    uint32_t i;
    bool match = FALSE;
    const bcmcth_trunk_vp_member_info_t *member_info;
    const bcmcth_trunk_drv_var_ids_t *ids;

    SHR_FUNC_ENTER(unit);

    member_info = vp_drv_var->member0_info;
    ids = vp_drv_var->ids;
    /* Verifiy for null pointer input params and return error. */
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    fid = in->id;
    fval = in->data;
    idx = in->idx;
    if (in->flags & SHR_FMM_FIELD_DEL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_field_default_get(unit, lt_entry->glt_sid, fid,
                                     1, &def_val, &num));
        fval = def_val;
    }

    if (fid == ids->trunk_vp_trunk_vp_id) {
        BCMCTH_TRUNK_VP_LT_FIELD_SET(
        lt_entry->fbmp, BCMCTH_TRUNK_VP_LT_FIELD_TRUNK_VP_ID);
        lt_entry->trunk_vp_id = (bcmcth_trunk_vp_id_t) fval;
    } else if (fid == ids->trunk_vp_lb_mode_id) {
        BCMCTH_TRUNK_VP_LT_FIELD_SET(
        lt_entry->fbmp, BCMCTH_TRUNK_VP_LT_FIELD_LB_MODE);
        lt_entry->lb_mode = (uint32_t) fval;
    } else if (fid == ids->trunk_vp_max_members_id) {
        BCMCTH_TRUNK_VP_LT_FIELD_SET
            (lt_entry->fbmp, BCMCTH_TRUNK_VP_LT_FIELD_MAX_MEMBERS);
        lt_entry->max_members = (uint32_t) fval;
    } else if (fid == ids->trunk_vp_member_cnt_id) {
        BCMCTH_TRUNK_VP_LT_FIELD_SET
            (lt_entry->fbmp, BCMCTH_TRUNK_VP_LT_FIELD_MEMBER_CNT);
        lt_entry->member_cnt = (uint32_t) fval;
    } else {
        for (i = 0 ; i < member_info->flds_count; i++) {
            match = FALSE;
            if (fid == vp_drv_var->flds_mem0[i].grp_fld) {

                BCMCTH_TRUNK_VP_LT_FIELD_SET
                    (lt_entry->fbmp,  fbmp_mem0_id[i]);
                lt_entry->member0_field[i][idx] = (int) fval;
                match = TRUE;
                break;
            }
        }
        if (match == FALSE) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief TRUNK_VP_WEIGHTED LT entry fields fill routine.
 *
 * Parse TRUNK_VP_WEIGHTED logical table entry logical fields
 * and store the data in lt_entry structure.
 *
 * \param [in] unit Unit number.
 * \param [in] in imm field informtion.
 * \param [in] vp_drv_var VP device variant info.
 * \param [in,out] lt_entry LT entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval !SHR_E_NONE Failed to convert \c in to \c lt_entry.
 */
static int
bcm56780_a0_trunk_vp_weighted_group_lt_fields_fill(
                                     int unit,
                                     const bcmltd_field_t *in,
                                     const bcmcth_trunk_vp_drv_var_t *vp_drv_var,
                                     bcmcth_trunk_vp_lt_entry_t *lt_entry)
{
    int fid;  /* Field identifier value. */
    uint64_t fval; /* Field value. */
    uint32_t idx;
    uint32_t num;
    uint64_t def_val;
    uint32_t i;
    bool match = FALSE;
    const bcmcth_trunk_vp_member_info_t *wmember_info;
    const bcmcth_trunk_drv_var_ids_t *ids;

    SHR_FUNC_ENTER(unit);

    /* Verifiy for null pointer input params and return error. */
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    wmember_info = vp_drv_var->wmember0_info;
    fid = in->id;
    fval = in->data;
    idx = in->idx;
    ids = vp_drv_var->ids;
    if (in->flags & SHR_FMM_FIELD_DEL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlrd_field_default_get(unit, lt_entry->glt_sid, fid,
                                     1, &def_val, &num));
        fval = def_val;
    }
    if (fid == ids->trunk_vp_weighted_trunk_vp_id) {
        BCMCTH_TRUNK_VP_LT_FIELD_SET(
        lt_entry->fbmp, BCMCTH_TRUNK_VP_LT_FIELD_TRUNK_VP_ID);
        lt_entry->trunk_vp_id = (bcmcth_trunk_vp_id_t) fval;
    } else if (fid == ids->trunk_vp_weighted_wt_size_id) {
        lt_entry->weighted_size = (uint32_t) fval;
        BCMCTH_TRUNK_VP_LT_FIELD_SET(
        lt_entry->fbmp, BCMCTH_TRUNK_VP_LT_FIELD_WEIGHTED_SIZE);
        lt_entry->lb_mode = (uint32_t) fval;
        BCMCTH_TRUNK_VP_LT_FIELD_SET(
        lt_entry->fbmp, BCMCTH_TRUNK_VP_LT_FIELD_MAX_MEMBERS);
        lt_entry->max_members =
           bcmcth_trunk_vp_weighted_size_convert_to_member_cnt(fval);
    } else if (fid == ids->trunk_vp_weighted_member_cnt_id) {
        BCMCTH_TRUNK_VP_LT_FIELD_SET
            (lt_entry->fbmp, BCMCTH_TRUNK_VP_LT_FIELD_MEMBER_CNT);
        lt_entry->member_cnt = (uint32_t) fval;
    } else {
        for (i = 0 ; i < wmember_info->flds_count; i++) {
            match = FALSE;
            if (fid == vp_drv_var->wflds_mem0[i].grp_fld) {
                BCMCTH_TRUNK_VP_LT_FIELD_SET
                    (lt_entry->fbmp, fbmp_mem0_id[i]);
                lt_entry->member0_field[i][idx] = (int) fval;
                match = TRUE;
                break;
            }
        }
        if (match == FALSE) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief TRUNK_VP/TRUNK_VP_WEIGHTED LT entry fields fill routine.
 *
 * Parse TRUNK_VP logical table entry logical fields
 * and store the data in lt_entry structure.
 *
 * \param [in]  unit Unit number.
 * \param [in]  in field imm field informtion.
 * \param [in]  drv_var device variant info.
 * \param [out] lt_entry lt_entry LT entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval !SHR_E_NONE Failed to convert \c in to \c lt_entry.
 */
static int
bcm56780_a0_trunk_vp_lt_fields_fill(int unit,
                                    const void *in_field,
                                    const bcmcth_trunk_drv_var_t *drv_var,
                                    void *entry)
{
    const bcmltd_field_t *in = in_field;
    bcmcth_trunk_vp_lt_entry_t *lt_entry = entry;
    bcmcth_trunk_vp_grp_type_t gtype = BCMCTH_TRUNK_VP_GRP_TYPE_COUNT;

    SHR_FUNC_ENTER(unit);

    /* Verifiy for null pointer input params and return error. */
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    gtype = lt_entry->grp_type;

    if (gtype == BCMCTH_TRUNK_VP_GRP_TYPE_WEIGHTED_0) {
        SHR_ERR_EXIT
            (bcm56780_a0_trunk_vp_weighted_group_lt_fields_fill
               (unit, in, drv_var->trunk_vp_var, lt_entry));
    } else if (gtype == BCMCTH_TRUNK_VP_GRP_TYPE_OVERLAY){
        SHR_ERR_EXIT
            (bcm56780_a0_trunk_vp_group_lt_fields_fill
               (unit, in, drv_var->trunk_vp_var, lt_entry));
    }  else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}
static bcmcth_trunk_drv_t bcm56780_a0_cth_trunk_drv =  {
    .fn_imm_grp_add          = &bcm56780_a0_trunk_grp_add,
    .fn_imm_grp_update       = &bcm56780_a0_trunk_grp_update,
    .fn_imm_grp_del          = &bcm56780_a0_trunk_grp_del,
    .fn_imm_grp_move         = &bcm56780_a0_trunk_grp_move,
    .fn_imm_grp_init         = &bcm56780_a0_trunk_grp_init,
    .fn_sys_grp_add          = &bcm56780_a0_sys_trunk_grp_add,
    .fn_sys_grp_del          = &bcm56780_a0_sys_trunk_grp_del,
    .fn_sys_grp_update       = &bcm56780_a0_sys_trunk_grp_update,
    .fn_sys_grp_move         = &bcm56780_a0_sys_trunk_grp_move,
    .fn_sys_grp_init         = &bcm56780_a0_sys_trunk_grp_init,
    .info_init               = &bcm56780_a0_trunk_vp_info_init,
    .info_cleanup            = &bcm56780_a0_trunk_vp_info_cleanup,
    .grp_ins                 = &bcm56780_a0_trunk_vp_grp_ins,
    .grp_del                 = &bcm56780_a0_trunk_vp_grp_del,
    .memb_cpy                = &bcm56780_a0_trunk_vp_memb_cpy,
    .memb_clr                = &bcm56780_a0_trunk_vp_memb_clr,
    .gmstart_updt            = &bcm56780_a0_trunk_vp_gmstart_updt,
    .lt_fields_fill          = &bcm56780_a0_trunk_vp_lt_fields_fill
};

/*******************************************************************************
 * Public Functions
 */

bcmcth_trunk_drv_t *
bcm56780_a0_cth_trunk_drv_get(int unit)
{
    return &bcm56780_a0_cth_trunk_drv;
}


