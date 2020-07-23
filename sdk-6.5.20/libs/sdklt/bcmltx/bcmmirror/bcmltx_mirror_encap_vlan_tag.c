/*! \file bcmltx_mirror_encap_vlan_tag.c
 *
 * Mirror Encap VLAN TAG Handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltx/bcmmirror/bcmltx_mirror_encap_vlan_tag.h>
#include <bcmdrd/bcmdrd_field.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_MIRROR

/* VLAN ID mask. */
#define PKT_VLAN_MASK            0xFFF
/* VLAN priority shift. */
#define PKT_PRIO_SHIFT           13
/* VLAN priority mask. */
#define PKT_PRIO_MASK            7
/* helper marco of VLAN priority. */
#define PKT_PRIO(p)              (((p) & PKT_PRIO_MASK) << PKT_PRIO_SHIFT)
/* CFI value mask. */
#define PKT_CFI_MASK             1
/* CFI value shift. */
#define PKT_CFI_SHIFT            12
/* helper marco of CFI value. */
#define PKT_CFI(c)               (((c) & PKT_CFI_MASK) << PKT_CFI_SHIFT)

int
bcmltx_mirror_encap_vlan_tag_rev_transform(int unit,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_transform_arg_t *arg)
{
    uint16_t pri, cfi, vlan_id, tpid;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META(
                "bcmltx_mirror_encap_vlan_tag_rev_transform \n")));

    if ((in->count < 1) || (arg->rfields < 4)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    pri     = (in->field[0]->data >> PKT_PRIO_SHIFT) & PKT_PRIO_MASK;
    cfi     = (in->field[0]->data >> PKT_CFI_SHIFT) & PKT_CFI_MASK;
    vlan_id = in->field[0]->data & 0x0FFF;
    tpid    = in->field[0]->data >> 16;

    out->count = 0;
    out->field[0]->data = pri;
    out->field[0]->id   = arg->rfield[out->count];
    out->field[0]->idx  = 0;
    out->count++;

    out->field[1]->data = cfi;
    out->field[1]->id   = arg->rfield[out->count];
    out->field[1]->idx  = 0;
    out->count++;

    out->field[2]->data = vlan_id;
    out->field[2]->id   = arg->rfield[out->count];
    out->field[2]->idx  = 0;
    out->count++;

    out->field[3]->data = tpid;
    out->field[3]->id   = arg->rfield[out->count];
    out->field[3]->idx  = 0;
    out->count++;

    SHR_EXIT();


 exit:
    SHR_FUNC_EXIT();
    return (0);
}

int
bcmltx_mirror_encap_vlan_tag_transform(int unit,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_transform_arg_t *arg)
{
    uint16_t   vlan, tpid;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META(
                "bcmltx_mirror_encap_vlan_tag_transform \n")));

    if ((in->count < 4) || (arg->rfields < 1)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    vlan    = PKT_PRIO(in->field[0]->data) |
              PKT_CFI(in->field[1]->data) |
              (in->field[2]->data & PKT_VLAN_MASK);
    tpid    = in->field[3]->data;

    out->count = 0;
    out->field[0]->data = ((uint32_t)(tpid << 16) | vlan);
    out->field[0]->id   = arg->rfield[out->count];
    out->field[0]->idx  = 0;
    out->count++;

    SHR_EXIT();
 exit:
    SHR_FUNC_EXIT();
}
