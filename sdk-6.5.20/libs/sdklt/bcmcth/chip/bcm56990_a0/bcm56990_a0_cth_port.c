/*! \file bcm56990_a0_cth_port.c
 *
 * bcm56990_a0 PORT drivers.
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
#include <bcmcth/bcmcth_port_drv.h>
#include <bcmdrd/chip/bcm56990_a0_enum.h>
#include <bcmdrd/chip/bcm56990_a0_defs.h>
#include <bcmcth/bcmcth_util.h>
#include <bcmcth/generated/port_ha.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmissu/issu_api.h>
#include <bcmltd/bcmltd_handler.h>

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_PORT

/* Maximum mirror constainer number. */
#define BCMPORT_MIRROR_CONTAINER_MAX (4)

/* Maximum port ID. */
#define BCMPORT_PORT_ID_MAX (272)

/*! Install hardware entry in all pipe instances. */
#define BCMPORT_ALL_PIPES_INST (-1)

/* Every TH4 port mirror reported block carries this signature */
#define BCM_CTH_PORT_BLK_SIGNATURE  0x96990ABC
/*******************************************************************************
 * Typedefs
 */

/*******************************************************************************
 * Private variables
 */
/* Ingress/egress mirror HA structure. */
static bcmcth_port_mirror_ha_blk_t *port_mstate[BCMDRD_CONFIG_MAX_UNITS];

/*! Instance bitmap of ingress mirror enabled on a port. */
#define BCM_CTH_PORT_IM_BMP(_u, _p) (port_mstate[_u]->state_array[_p].im)

/*! Instance bitmap of egress mirror for UC enabled on a port. */
#define BCM_CTH_PORT_UC_EM_BMP(_u, _p) (port_mstate[_u]->state_array[_p].uc_em)

/*! Instance bitmap of egress mirror for non-UC enabled on a port. */
#define BCM_CTH_PORT_NUC_EM_BMP(_u, _p) (port_mstate[_u]->state_array[_p].nuc_em)

/*! Mirror information for UC container. */
#define BCM_CTH_PORT_UC_EM_STATE(_u, _p, _c) \
            (port_mstate[_u]->state_array[_p].uc_em_state[_c])

/*! Mirror information for non-UC container. */
#define BCM_CTH_PORT_NUC_EM_STATE(_u, _p, _c) \
            (port_mstate[_u]->state_array[_p].nuc_em_state[_c])
/*******************************************************************************
 * Private functions
 */
static void
bcm56990_a0_cth_pt_op_info_t_init(bcmcth_pt_op_info_t *pt_op_info)
{
    if (pt_op_info) {
        pt_op_info->drd_sid = BCM56990_A0_ENUM_COUNT;
        pt_op_info->trans_id = -1;
        pt_op_info->req_lt_sid = BCM56990_A0_ENUM_COUNT;
        pt_op_info->rsp_lt_sid = BCM56990_A0_ENUM_COUNT;
        pt_op_info->req_flags = BCMPTM_REQ_FLAGS_NO_FLAGS;
        pt_op_info->rsp_flags = BCMPTM_REQ_FLAGS_NO_FLAGS;
        pt_op_info->op = BCMPTM_OP_NOP;
        pt_op_info->dyn_info.index = -1;
        pt_op_info->dyn_info.tbl_inst = BCMPORT_ALL_PIPES_INST;
        pt_op_info->wsize = 0;
    }
    return;
}

static int
bcm56990_a0_port_mirror_generic_port_tab_pt_write(int unit,
                                                  bool is_port_tab,
                                                  bcmltd_sid_t lt_id,
                                                  const bcmltd_op_arg_t *op_arg,
                                                  int tbl_inst,
                                                  uint16_t index,
                                                  uint32_t container,
                                                  bool mirror_enable)
{
    uint32_t *entry_buf = NULL;
    uint32_t entry_sz = 0;
    uint32_t mbmp = 0;
    bcmdrd_sid_t pt_id;
    bcmdrd_fid_t pt_fid;
    bcmcth_pt_op_info_t pt_op_info;

    SHR_FUNC_ENTER(unit);

    if (is_port_tab){
        pt_id = PORT_TABm;
    } else {
        pt_id = LPORT_TABm;
    }

    pt_fid = MIRRORf;

    entry_sz = BCMDRD_WORDS2BYTES(bcmdrd_pt_entry_wsize(unit, pt_id));
    SHR_ALLOC(entry_buf, entry_sz, "bcmcthPortTableEntry");
    SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);
    sal_memset(entry_buf, 0, entry_sz);

    bcm56990_a0_cth_pt_op_info_t_init(&pt_op_info);
    pt_op_info.drd_sid = pt_id;
    pt_op_info.trans_id = op_arg->trans_id;
    pt_op_info.req_lt_sid = lt_id;
    pt_op_info.op = BCMPTM_OP_READ;
    pt_op_info.wsize = bcmdrd_pt_entry_wsize(unit, pt_id);
    pt_op_info.req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);
    pt_op_info.dyn_info.tbl_inst = tbl_inst;
    pt_op_info.dyn_info.index = index;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_pt_read(unit, &pt_op_info, entry_buf));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_pt_field_get(unit, pt_id, entry_buf, pt_fid, &mbmp));

    if (mirror_enable) {
        mbmp |= 1 << container;
    } else {
        mbmp &= ~(1 << container);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_pt_field_set(unit, pt_id, entry_buf, pt_fid, &mbmp));

    bcm56990_a0_cth_pt_op_info_t_init(&pt_op_info);
    pt_op_info.drd_sid = pt_id;
    pt_op_info.trans_id = op_arg->trans_id;
    pt_op_info.req_lt_sid = lt_id;
    pt_op_info.op = BCMPTM_OP_WRITE;
    pt_op_info.wsize = bcmdrd_pt_entry_wsize(unit, pt_id);

    /* Set up hardware table index to write. */
    pt_op_info.dyn_info.tbl_inst = tbl_inst;
    pt_op_info.dyn_info.index = index;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_pt_write(unit, &pt_op_info, entry_buf));

exit:
    if (entry_buf) {
        SHR_FREE(entry_buf);
    }
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_port_mirror_port_tab_pt_write(int unit,
                                          bcmltd_sid_t lt_id,
                                          const bcmltd_op_arg_t *op_arg,
                                          shr_port_t port,
                                          uint32_t container,
                                          bool mirror_enable)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56990_a0_port_mirror_generic_port_tab_pt_write(unit,
                                                           TRUE,
                                                           lt_id,
                                                           op_arg,
                                                           BCMPORT_ALL_PIPES_INST,
                                                           port,
                                                           container,
                                                           mirror_enable));
exit:
    SHR_FUNC_EXIT();

}

static int
bcm56990_a0_port_mirror_lport_tab_pt_write(int unit,
                                           bcmltd_sid_t lt_id,
                                           const bcmltd_op_arg_t *op_arg,
                                           int tbl_inst,
                                           uint16_t port_system_profile_id,
                                           uint32_t container,
                                           bool mirror_enable)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56990_a0_port_mirror_generic_port_tab_pt_write(
             unit,
             FALSE,
             lt_id,
             op_arg,
             tbl_inst,
             port_system_profile_id,
             container,
             mirror_enable));

exit:
    SHR_FUNC_EXIT();

}

static int
bcm56990_a0_port_i_mirror_control_pt_write(int unit,
                                           bcmltd_sid_t lt_id,
                                           const bcmltd_op_arg_t *op_arg,
                                           shr_port_t port,
                                           uint32_t container,
                                           uint32_t inst_id,
                                           bool mirror_enable,
                                           bool non_uc_em)
{
    uint32_t *entry_buf = NULL;
    uint32_t entry_sz = 0;
    bcmdrd_fid_t non_uc_mtp[] = {NON_UC_EM_MTP_INDEX0f,
                                 NON_UC_EM_MTP_INDEX1f,
                                 NON_UC_EM_MTP_INDEX2f,
                                 NON_UC_EM_MTP_INDEX3f};
    bcmdrd_fid_t uc_mtp[] = {MTP_INDEX0f,
                             MTP_INDEX1f,
                             MTP_INDEX2f,
                             MTP_INDEX3f};
    bcmdrd_sid_t pt_id;
    bcmdrd_fid_t pt_fid_mtp;
    uint32_t fval;

    SHR_FUNC_ENTER(unit);

    if (container >= BCMPORT_MIRROR_CONTAINER_MAX) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    pt_id = I_MIRROR_CONTROLm;

    if (non_uc_em) {
        pt_fid_mtp = non_uc_mtp[container];
    } else {
        pt_fid_mtp = uc_mtp[container];
    }

    entry_sz = BCMDRD_WORDS2BYTES(bcmdrd_pt_entry_wsize(unit, pt_id));
    SHR_ALLOC(entry_buf, entry_sz, "bcmcthPortTableEntry");
    SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);
    sal_memset(entry_buf, 0, entry_sz);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_port_pt_read(unit, op_arg, lt_id, pt_id, port, entry_buf));

    /* MTP_INDEX */
    fval = mirror_enable ? inst_id : 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_pt_field_set(unit, pt_id, entry_buf, pt_fid_mtp, &fval));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_port_pt_write(unit, op_arg, lt_id, pt_id, port, entry_buf));

exit:
    if (entry_buf) {
        SHR_FREE(entry_buf);
    }
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_port_e_mirror_control_pt_write(int unit,
                                           bcmltd_sid_t lt_id,
                                           const bcmltd_op_arg_t *op_arg,
                                           shr_port_t port,
                                           uint32_t container,
                                           uint32_t inst_id,
                                           bool mirror_enable)
{
    uint32_t *entry_buf = NULL;
    uint32_t entry_sz = 0;
    bcmdrd_fid_t uc_mtp[] = {MTP_INDEX0f,
                             MTP_INDEX1f,
                             MTP_INDEX2f,
                             MTP_INDEX3f};
    bcmdrd_sid_t pt_id;
    bcmdrd_fid_t pt_fid_mtp;
    uint32_t fval;

    SHR_FUNC_ENTER(unit);

    if (container >= BCMPORT_MIRROR_CONTAINER_MAX) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    pt_id = E_MIRROR_CONTROLm;

    pt_fid_mtp = uc_mtp[container];

    entry_sz = BCMDRD_WORDS2BYTES(bcmdrd_pt_entry_wsize(unit, pt_id));
    SHR_ALLOC(entry_buf, entry_sz, "bcmcthPortTableEntry");
    SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);
    sal_memset(entry_buf, 0, entry_sz);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_port_pt_read(unit, op_arg, lt_id, pt_id, port, entry_buf));

    /* MTP_INDEX */
    fval = mirror_enable ? inst_id : 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_pt_field_set(unit, pt_id, entry_buf, pt_fid_mtp, &fval));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_port_pt_write(unit, op_arg, lt_id, pt_id, port, entry_buf));

exit:
    if (entry_buf) {
        SHR_FREE(entry_buf);
    }
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_port_emirror_control_pt_write(int unit,
                                          bcmltd_sid_t lt_id,
                                          const bcmltd_op_arg_t *op_arg,
                                          shr_port_t iport,
                                          shr_port_t eport,
                                          uint32_t container,
                                          bool mirror_enable)
{
    uint32_t *entry_buf = NULL;
    uint32_t entry_sz = 0;
    bcmdrd_sid_t pt_id_array[] = {EMIRROR_CONTROL_0m,
                                  EMIRROR_CONTROL_1m,
                                  EMIRROR_CONTROL_2m,
                                  EMIRROR_CONTROL_3m};
    bcmdrd_sid_t pt_id;
    bcmdrd_fid_t pt_fid;
    bcmdrd_pbmp_t pbm_eport;

    SHR_FUNC_ENTER(unit);

    if (container >= BCMPORT_MIRROR_CONTAINER_MAX) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    pt_id  = pt_id_array[container];
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
bcm56990_a0_port_tab_fields_set(int unit,
                                const bcmltd_op_arg_t *op_arg,
                                bcmltd_sid_t lt_id, shr_port_t port,
                                port_membership_policy_info_t *lt_info)
{
    PORT_TABm_t port_tab_buf;
    uint32_t field_value = 0;
    uint32_t *buf_ptr = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_info, SHR_E_PARAM);

    sal_memset(&port_tab_buf, 0, sizeof(port_tab_buf));

    /* Read original entry. */
    buf_ptr = (uint32_t *)&port_tab_buf;
    SHR_IF_ERR_EXIT
        (bcmcth_port_pt_read(unit, op_arg, lt_id,
                             PORT_TABm, port, buf_ptr));

    /* Set ing_vlan_membership_check. */
    if (BCMPORT_LT_FIELD_GET(
            lt_info->fbmp, ING_VLAN_MEMBERSHIP_CHECK)) {
        field_value = lt_info->ing_vlan_membership_check;
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit,
                                 PORT_TABm,
                                 buf_ptr,
                                 EN_IFILTERf,
                                 &field_value));
    }

    /* Set up hardware table index to write. */
    SHR_IF_ERR_EXIT
        (bcmcth_port_pt_write(unit, op_arg, lt_id,
                              PORT_TABm, port, buf_ptr));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_egr_port_fields_set(int unit,
                                const bcmltd_op_arg_t *op_arg,
                                bcmltd_sid_t lt_id, shr_port_t port,
                                port_membership_policy_info_t *lt_info)
{
    EGR_PORTm_t egr_port_tab_buf;
    uint32_t field_value = 0;
    uint32_t *buf_ptr = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_info, SHR_E_PARAM);

    sal_memset(&egr_port_tab_buf, 0, sizeof(egr_port_tab_buf));

    /* Read original entry. */
    buf_ptr = (uint32_t *)&egr_port_tab_buf;
    SHR_IF_ERR_EXIT
        (bcmcth_port_pt_read(unit, op_arg, lt_id,
                             EGR_PORTm, port, buf_ptr));

    /* Set egr_vlan_membership_check. */
    if (BCMPORT_LT_FIELD_GET(
            lt_info->fbmp, EGR_VLAN_MEMBERSHIP_CHECK)) {
        field_value = lt_info->egr_vlan_membership_check;
        SHR_IF_ERR_EXIT
            (bcmdrd_pt_field_set(unit,
                                 EGR_PORTm,
                                 buf_ptr,
                                 EN_EFILTERf,
                                 &field_value));
    }
    /* Set up hardware table index to write. */
    SHR_IF_ERR_EXIT
        (bcmcth_port_pt_write(unit, op_arg, lt_id,
                              EGR_PORTm, port, buf_ptr));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ing_en_efilter_bmp_fields_set(int unit,
                                          const bcmltd_op_arg_t *op_arg,
                                          bcmltd_sid_t lt_id, shr_port_t port,
                                          port_membership_policy_info_t *lt_info)
{
    ING_EN_EFILTER_BITMAPm_t ing_en_efilter_bmp_buf;
    bcmdrd_pbmp_t pbmp;
    uint32_t * buf_ptr = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_info, SHR_E_PARAM);

    sal_memset(&ing_en_efilter_bmp_buf, 0, sizeof(ing_en_efilter_bmp_buf));

    /* Read original entry. */
    buf_ptr = (uint32_t *)&ing_en_efilter_bmp_buf;
    SHR_IF_ERR_EXIT
        (bcmcth_port_pt_read(unit, op_arg, lt_id,
                             ING_EN_EFILTER_BITMAPm, 0, buf_ptr));

    ING_EN_EFILTER_BITMAPm_BITMAPf_GET(ing_en_efilter_bmp_buf, pbmp.w);

    if (BCMPORT_LT_FIELD_GET(
            lt_info->fbmp, EGR_VLAN_MEMBERSHIP_CHECK)) {
        if (lt_info->egr_vlan_membership_check) {
            BCMDRD_PBMP_PORT_ADD(pbmp, lt_info->port_id);
        } else {
            BCMDRD_PBMP_PORT_REMOVE(pbmp, lt_info->port_id);
        }
    }

    /* Set egr_vlan_membership_check. */
    ING_EN_EFILTER_BITMAPm_BITMAPf_SET(ing_en_efilter_bmp_buf, pbmp.w);

    SHR_IF_ERR_EXIT
        (bcmcth_port_pt_write(unit, op_arg, lt_id,
                              ING_EN_EFILTER_BITMAPm, 0, buf_ptr));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_cth_port_membership_policy_get(
    int unit,
    const bcmltd_op_arg_t *op_arg,
    bcmltd_sid_t lt_id, shr_port_t port,
    port_membership_policy_info_t *lt_info)
{
    PORT_TABm_t port_tab_buf;
    EGR_PORTm_t egr_port_tab_buf;
    uint32_t *buf_ptr = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_info, SHR_E_PARAM);

    sal_memset(&port_tab_buf, 0, sizeof(port_tab_buf));
    sal_memset(&egr_port_tab_buf, 0, sizeof(egr_port_tab_buf));

    buf_ptr = (uint32_t*)&port_tab_buf;
    SHR_IF_ERR_EXIT
        (bcmcth_port_pt_read(unit, op_arg, lt_id,
                             PORT_TABm, port, buf_ptr));
    lt_info->ing_vlan_membership_check =
        PORT_TABm_EN_IFILTERf_GET(port_tab_buf);

    buf_ptr = (uint32_t*)&egr_port_tab_buf;
    SHR_IF_ERR_EXIT
        (bcmcth_port_pt_read(unit, op_arg, lt_id,
                             EGR_PORTm, port, buf_ptr));
    lt_info->egr_vlan_membership_check =
        EGR_PORTm_EN_EFILTERf_GET(egr_port_tab_buf);

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_cth_port_membership_policy_set(
    int unit,
    const bcmltd_op_arg_t *op_arg,
    bcmltd_sid_t lt_id, shr_port_t port,
    port_membership_policy_info_t *lt_info)
{
    bool port_tab_update = false;
    bool ing_en_efilter_bitmap_update = false;
    bool egr_port_update = false;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_info, SHR_E_PARAM);

    if (BCMPORT_LT_FIELD_GET(
            lt_info->fbmp, EGR_VLAN_MEMBERSHIP_CHECK)) {
        ing_en_efilter_bitmap_update = true;
        egr_port_update = true;
    }
    if (BCMPORT_LT_FIELD_GET(
            lt_info->fbmp, ING_VLAN_MEMBERSHIP_CHECK)) {
        port_tab_update = true;
    }

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "bcm56990_a0_cth_port_membership_policy_set.\n")));

    if (port_tab_update) {
        SHR_IF_ERR_EXIT
            (bcm56990_a0_port_tab_fields_set(unit,
                                             op_arg,
                                             lt_id,
                                             port,
                                             lt_info));
    }

    if (egr_port_update) {
        SHR_IF_ERR_EXIT
            (bcm56990_a0_egr_port_fields_set(unit,
                                             op_arg,
                                             lt_id,
                                             port,
                                             lt_info));
    }

    if (ing_en_efilter_bitmap_update) {
        SHR_IF_ERR_EXIT
            (bcm56990_a0_ing_en_efilter_bmp_fields_set(unit,
                                                       op_arg,
                                                       lt_id,
                                                       port,
                                                       lt_info));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_cth_port_bridge_get(int unit,
                                const bcmltd_op_arg_t *op_arg,
                                bcmltd_sid_t lt_id,
                                shr_port_t port,
                                bool *bridge)
{
    PORT_TABm_t port_tab_buf;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(bridge, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(
        bcmcth_port_pt_read(unit, op_arg, lt_id,
                            PORT_TABm, port, &port_tab_buf));
    *bridge = PORT_TABm_PORT_BRIDGEf_GET(port_tab_buf);

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_cth_port_bridge_set(int unit,
                                const bcmltd_op_arg_t *op_arg,
                                bcmltd_sid_t lt_id,
                                shr_port_t port,
                                bool bridge)
{
    PORT_TABm_t port_tab_buf;
    PORT_BRIDGE_BMAPm_t bridge_bmap_buf;
    bcmdrd_pbmp_t pbmp;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT(
        bcmcth_port_pt_read(unit, op_arg, lt_id,
                            PORT_TABm, port, &port_tab_buf));

    PORT_TABm_PORT_BRIDGEf_SET(port_tab_buf, bridge ? 1 : 0);
    SHR_IF_ERR_EXIT(
        bcmcth_port_pt_write(unit, op_arg, lt_id,
                             PORT_TABm, port, &port_tab_buf));

    SHR_IF_ERR_EXIT(
        bcmcth_port_pt_read(unit, op_arg, lt_id,
                            PORT_BRIDGE_BMAPm, 0, &bridge_bmap_buf));
    PORT_BRIDGE_BMAPm_BITMAPf_GET(bridge_bmap_buf, pbmp.w);
    if (bridge) {
        BCMDRD_PBMP_PORT_ADD(pbmp, port);
    } else {
        BCMDRD_PBMP_PORT_REMOVE(pbmp, port);
    }

    PORT_BRIDGE_BMAPm_BITMAPf_SET(bridge_bmap_buf, pbmp.w);
    SHR_IF_ERR_EXIT(
        bcmcth_port_pt_write(unit, op_arg, lt_id,
                             PORT_BRIDGE_BMAPm, 0, &bridge_bmap_buf));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_cth_port_ingress_mirror_set(int unit,
                                        const bcmltd_op_arg_t *op_arg,
                                        bcmltd_sid_t lt_id,
                                        port_imirror_t *cfg,
                                        bool mirror_enable)
{
    shr_port_t iport;
    uint32_t container, inst_id;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cfg, SHR_E_PARAM);

    iport = cfg->port;
    container = cfg->container;
    inst_id = cfg->instance_id;

    /* Update mirror state */
    if (mirror_enable) {
        BCM_CTH_PORT_IM_BMP(unit, iport) |= 1 << container;
    } else {
        BCM_CTH_PORT_IM_BMP(unit, iport) &= ~(1 << container);
    }

    /* Update PORT_TABm */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56990_a0_port_mirror_port_tab_pt_write(unit,
                                                   lt_id,
                                                   op_arg,
                                                   iport,
                                                   container,
                                                   mirror_enable));

    /* Update I_MIRROR_CONTROLm */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56990_a0_port_i_mirror_control_pt_write(unit,
                                                    lt_id,
                                                    op_arg,
                                                    iport,
                                                    container,
                                                    inst_id,
                                                    mirror_enable,
                                                    FALSE));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_cth_port_egress_mirror_set(int unit,
                                       const bcmltd_op_arg_t *op_arg,
                                       bcmltd_sid_t lt_id,
                                       port_emirror_t *cfg,
                                       bool mirror_enable)
{
    shr_port_t iport, eport;
    uint32_t container, inst_id;
    bool update_imtp = FALSE, update_emtp = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cfg, SHR_E_PARAM);

    iport = cfg->port;
    eport = cfg->egr_port;
    container = cfg->container;
    inst_id = cfg->instance_id;

    /* Update mirror state */
    if (mirror_enable) {
        if (BCM_CTH_PORT_UC_EM_BMP(unit, eport) & (1 << container) &&
            BCM_CTH_PORT_UC_EM_STATE(unit, eport, container).inst != inst_id) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit, "The UC container(%d) of port(%d) "
                                        "has been occupied.\n"),
                                        container, eport));
            return SHR_E_RESOURCE;
        }
        if (BCM_CTH_PORT_NUC_EM_BMP(unit, iport) & (1 << container) &&
            BCM_CTH_PORT_NUC_EM_STATE(unit, iport, container).inst != inst_id) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit, "The non-UC container(%d) of port(%d) "
                                        "has been occupied.\n"),
                                        container, iport));
            return SHR_E_RESOURCE;
        }
        BCM_CTH_PORT_UC_EM_BMP(unit, eport) |= 1 << container;
        BCM_CTH_PORT_UC_EM_STATE(unit, eport, container).inst = inst_id;
        BCM_CTH_PORT_UC_EM_STATE(unit, eport, container).ref_cnt++;
        BCM_CTH_PORT_NUC_EM_BMP(unit, iport) |= 1 << container;
        BCM_CTH_PORT_NUC_EM_STATE(unit, iport, container).inst = inst_id;
        BCM_CTH_PORT_NUC_EM_STATE(unit, iport, container).ref_cnt++;
    } else {
        BCM_CTH_PORT_UC_EM_STATE(unit, eport, container).ref_cnt--;
        if (BCM_CTH_PORT_UC_EM_STATE(unit, eport, container).ref_cnt == 0) {
            BCM_CTH_PORT_UC_EM_BMP(unit, eport) &= ~(1 << container);
            BCM_CTH_PORT_UC_EM_STATE(unit, eport, container).inst = 0;
            update_emtp = TRUE;
        }

        BCM_CTH_PORT_NUC_EM_STATE(unit, iport, container).ref_cnt--;
        if (BCM_CTH_PORT_NUC_EM_STATE(unit, iport, container).ref_cnt == 0) {
            BCM_CTH_PORT_NUC_EM_BMP(unit, iport) &= ~(1 << container);
            BCM_CTH_PORT_NUC_EM_STATE(unit, iport, container).inst = 0;
            update_imtp = TRUE;
        }
    }

    /* Update EMIRROR_CONTROLm */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56990_a0_port_emirror_control_pt_write(unit,
                                                   lt_id,
                                                   op_arg,
                                                   iport,
                                                   eport,
                                                   container,
                                                   mirror_enable));

    /* Update I_MIRROR_CONTROLm on ingress port for non-UC */
    if (mirror_enable || update_imtp) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56990_a0_port_i_mirror_control_pt_write(unit,
                                                        lt_id,
                                                        op_arg,
                                                        iport,
                                                        container,
                                                        inst_id,
                                                        mirror_enable,
                                                        TRUE));
    }

    /* Update E_MIRROR_CONTROLm on egress port for UC */
    if (mirror_enable || update_emtp) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm56990_a0_port_e_mirror_control_pt_write(unit,
                                                        lt_id,
                                                        op_arg,
                                                        eport,
                                                        container,
                                                        inst_id,
                                                        mirror_enable));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_cth_port_system_ing_mirror_profile_set(
    int unit,
    const bcmltd_op_arg_t *op_arg,
    bcmltd_sid_t lt_id,
    int tbl_inst,
    uint16_t port_system_profile_id,
    uint32_t container,
    bool mirror_enable)
{
    SHR_FUNC_ENTER(unit);

    /* Update LPORT_TABm */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56990_a0_port_mirror_lport_tab_pt_write(unit,
                                                    lt_id,
                                                    op_arg,
                                                    tbl_inst,
                                                    port_system_profile_id,
                                                    container,
                                                    mirror_enable));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_cth_port_deinit(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (port_mstate[unit] != NULL) {
        port_mstate[unit] = NULL;
    }

    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_cth_port_init(int unit, bool warm)
{
    const bcmlrd_map_t *map = NULL;
    uint32_t ha_alloc_size = 0, ha_req_size = 0;
    int rv;

    SHR_FUNC_ENTER(unit);

    rv = bcmlrd_map_get(unit, PORT_ING_MIRRORt, &map);
    if (SHR_FAILURE(rv) || !map) {
        rv = bcmlrd_map_get(unit, PORT_EGR_MIRRORt, &map);
    }
    if (SHR_FAILURE(rv) || !map) {
        SHR_EXIT();
    }

    /* compiler treat bcmcth_port_mirror_ha_blk_t array as empty array(size=0) */
    ha_req_size = sizeof(bcmcth_port_mirror_state_t) * BCMPORT_PORT_ID_MAX +
                  sizeof(bcmcth_port_mirror_ha_blk_t);
    ha_alloc_size = ha_req_size;
    port_mstate[unit] = shr_ha_mem_alloc(unit,
                                         BCMMGMT_PORT_COMP_ID,
                                         BCMPORT_MIRROR_SUB_COMP_ID,
                                         "bcmcthPortMirrorState",
                                         &ha_alloc_size);
    SHR_NULL_CHECK(port_mstate[unit], SHR_E_MEMORY);
    SHR_NULL_CHECK(port_mstate[unit]->state_array, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT
        ((ha_alloc_size < ha_req_size) ? SHR_E_MEMORY : SHR_E_NONE);

    if (!warm) {
        sal_memset(port_mstate[unit], 0, ha_alloc_size);
        port_mstate[unit]->signature = BCM_CTH_PORT_BLK_SIGNATURE;
        port_mstate[unit]->array_size = BCMPORT_PORT_ID_MAX;
        SHR_IF_ERR_EXIT
            (bcmissu_struct_info_report(unit, BCMMGMT_PORT_COMP_ID,
                                        BCMPORT_MIRROR_SUB_COMP_ID, 0,
                                        ha_req_size, 1,
                                        BCMCTH_PORT_MIRROR_HA_BLK_T_ID));
    } else {
        SHR_IF_ERR_MSG_EXIT
            ((port_mstate[unit]->signature != BCM_CTH_PORT_BLK_SIGNATURE
              ? SHR_E_FAIL : SHR_E_NONE),
              (BSL_META_U(unit,
                          "\n\tWB hw enty: Signature mismatch "
                          "exp=0x%-8x act=0x%-8x\n"),
               BCM_CTH_PORT_BLK_SIGNATURE,
               port_mstate[unit]->signature));
    }

exit:
    if (SHR_FUNC_ERR()) {
        bcmissu_struct_info_clear(unit,
                                  BCMCTH_PORT_MIRROR_HA_BLK_T_ID,
                                  BCMMGMT_PORT_COMP_ID,
                                  BCMPORT_MIRROR_SUB_COMP_ID);
        shr_ha_mem_free(unit, port_mstate[unit]);
        port_mstate[unit] = NULL;
    }
    SHR_FUNC_EXIT();
}

static bcmcth_port_drv_t
bcm56990_a0_cth_port_drv = {
   .port_init = bcm56990_a0_cth_port_init,
   .port_deinit = bcm56990_a0_cth_port_deinit,
   .port_bridge_get = bcm56990_a0_cth_port_bridge_get,
   .port_bridge_set = bcm56990_a0_cth_port_bridge_set,
   .port_membership_policy_get = bcm56990_a0_cth_port_membership_policy_get,
   .port_membership_policy_set = bcm56990_a0_cth_port_membership_policy_set,
   .port_egress_mirror_set = bcm56990_a0_cth_port_egress_mirror_set,
   .port_ingress_mirror_set = bcm56990_a0_cth_port_ingress_mirror_set,
   .port_modid_get = NULL,
   .port_system_ing_mirror_profile_set =
       bcm56990_a0_cth_port_system_ing_mirror_profile_set
};

/*******************************************************************************
 * Public Functions
 */
bcmcth_port_drv_t *
bcm56990_a0_cth_port_drv_get(int unit)
{
    return &bcm56990_a0_cth_port_drv;
}
