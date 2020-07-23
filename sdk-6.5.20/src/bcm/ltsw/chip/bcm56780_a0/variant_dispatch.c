/*! \file variant_dispatch.c
 *
 * BCM56780_A0 variant dispatch functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/chip/bcm56780_a0/variant_dispatch.h>

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_str.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_COMMON

/* Declare variant specific attach functions. */
#define BCM56780_A0_LTSW_VARIANT_LIST_ENTRY(_va,_ve,_vd,_r0) \
extern int bcm56780_a0_##_va##_ltsw_feature_drv_attach(int unit); \
extern int bcm56780_a0_##_va##_ltsw_sbr_drv_attach(int unit);     \
extern int bcm56780_a0_##_va##_ltsw_dev_misc_drv_attach(int unit);\
extern int bcm56780_a0_##_va##_ltsw_l3_fib_drv_attach(int unit);  \
extern int bcm56780_a0_##_va##_ltsw_l2_drv_attach(int unit); \
extern int bcm56780_a0_##_va##_ltsw_stat_drv_attach(int unit); \
extern int bcm56780_a0_##_va##_ltsw_pktio_drv_attach(int unit); \
extern int bcm56780_a0_##_va##_ltsw_cosq_drv_attach(int unit); \
extern int bcm56780_a0_##_va##_ltsw_flexctr_drv_attach(int unit); \
extern int bcm56780_a0_##_va##_ltsw_flexstate_drv_attach(int unit); \
extern int bcm56780_a0_##_va##_ltsw_switch_drv_attach(int unit); \
extern int bcm56780_a0_##_va##_ltsw_mpls_drv_attach(int unit); \
extern int bcm56780_a0_##_va##_ltsw_l3_intf_drv_attach(int unit); \
extern int bcm56780_a0_##_va##_ltsw_port_drv_attach(int unit); \
extern int bcm56780_a0_##_va##_ltsw_uft_drv_attach(int unit); \
extern int bcm56780_a0_##_va##_ltsw_vlan_drv_attach(int unit); \
extern int bcm56780_a0_##_va##_ltsw_rx_drv_attach(int unit); \
extern int bcm56780_a0_##_va##_ltsw_field_drv_attach(int unit); \
extern int bcm56780_a0_##_va##_ltsw_qos_drv_attach(int unit); \
extern int bcm56780_a0_##_va##_ltsw_flow_drv_attach(int unit); \
extern int bcm56780_a0_##_va##_ltsw_ifa_drv_attach(int unit); \
extern int bcm56780_a0_##_va##_ltsw_range_drv_attach(int unit); \
extern int bcm56780_a0_##_va##_ltsw_mirror_drv_attach(int unit);

#include <bcm_int/ltsw/chip/bcm56780_a0/variant_list.h>

/* Attach function types for each module. */
typedef int (*variant_attach_f)(int unit);

/*
 * Array of variant specific attach functions.
 * Please define a new module into bcm56780_a0_ltsw_variant_mod_t when adding
 * a new element into attach array.
 */
#define BCM56780_A0_LTSW_VARIANT_LIST_ENTRY(_va,_ve,_vd,_r0) \
    {{ \
        NULL, \
        bcm56780_a0_##_va##_ltsw_feature_drv_attach, \
        bcm56780_a0_##_va##_ltsw_sbr_drv_attach, \
        bcm56780_a0_##_va##_ltsw_dev_misc_drv_attach, \
        bcm56780_a0_##_va##_ltsw_l3_fib_drv_attach, \
        bcm56780_a0_##_va##_ltsw_l2_drv_attach, \
        bcm56780_a0_##_va##_ltsw_stat_drv_attach, \
        bcm56780_a0_##_va##_ltsw_pktio_drv_attach, \
        bcm56780_a0_##_va##_ltsw_cosq_drv_attach, \
        bcm56780_a0_##_va##_ltsw_flexctr_drv_attach, \
        bcm56780_a0_##_va##_ltsw_flexstate_drv_attach, \
        bcm56780_a0_##_va##_ltsw_switch_drv_attach, \
        bcm56780_a0_##_va##_ltsw_mpls_drv_attach, \
        bcm56780_a0_##_va##_ltsw_l3_intf_drv_attach, \
        bcm56780_a0_##_va##_ltsw_port_drv_attach, \
        bcm56780_a0_##_va##_ltsw_uft_drv_attach, \
        bcm56780_a0_##_va##_ltsw_vlan_drv_attach, \
        bcm56780_a0_##_va##_ltsw_rx_drv_attach, \
        bcm56780_a0_##_va##_ltsw_field_drv_attach, \
        bcm56780_a0_##_va##_ltsw_qos_drv_attach, \
        bcm56780_a0_##_va##_ltsw_flow_drv_attach, \
        bcm56780_a0_##_va##_ltsw_ifa_drv_attach, \
        bcm56780_a0_##_va##_ltsw_range_drv_attach, \
        bcm56780_a0_##_va##_ltsw_mirror_drv_attach, \
    }},
static struct {
    variant_attach_f drv_attach[BCM56780_A0_LTSW_VARIANT_MAX];
} variant_list_attach[] = {
#include <bcm_int/ltsw/chip/bcm56780_a0/variant_list.h>
    {{ 0 }} /* end-of-list */
};

/* Array of variant name. */
#define BCM56780_A0_LTSW_VARIANT_LIST_ENTRY(_va,_ve,_vd,_r0) \
#_ve,

static const char* variant_name[] = {
#include <bcm_int/ltsw/chip/bcm56780_a0/variant_list.h>
    NULL,
};

/* Variant information. */
typedef struct variant_info_s {
    /* Index in variant list. */
    int index;

    /* Base variant module. */
    bcm56780_a0_ltsw_variant_mod_t base_module;
} variant_info_t;

static variant_info_t variant_info[BCM_MAX_NUM_UNITS] = {{ 0 }};

/******************************************************************************
 * Private functions
 */
/*!
 * \brief Retrieves the variant index in arrary.
 *
 * \param [in] unit Unit number.
 * \param [out] variant Variant index in array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
variant_get(int unit, int *variant)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    const char *vn = NULL;
    int dunit, i;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(variant, SHR_E_PARAM);

    /* Get variant name. */
    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, DEVICE_CONFIGs, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit,
                              entry_hdl,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(entry_hdl, VARIANTs, &vn));


    /* Get array index based on variant name. */
    for(i = 0; i < COUNTOF(variant_name); i++) {
        if((variant_name[i] != NULL) &&
            !sal_strcmp(variant_name[i], vn)) {
            break;
        }
    }

    if (i == COUNTOF(variant_name)) {
        SHR_IF_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    *variant = i;

exit:
    if (BCMLT_INVALID_HDL != entry_hdl) {
        (void) bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*!
* \brief Retrieves the index in variant arrary.
*
* \param [in] unit Unit number.
* \param [in] module module .
* \param [out] index Variant array index.
*
* \retval SHR_E_NONE No errors.
* \retval !SHR_E_NONE Failure.
*/
static int
variant_index_get(int unit, bcm56780_a0_ltsw_variant_mod_t module, int *index)
{
    variant_info_t *vi = &variant_info[unit];

    SHR_FUNC_ENTER(unit);

    if (vi->base_module == BCM56780_A0_LTSW_VARIANT_INVALID ||
        vi->base_module == module) {

        SHR_IF_ERR_VERBOSE_EXIT
            (variant_get(unit, &vi->index));

        vi->base_module = module;
    }

    *index = vi->index;

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public functions
 */
int
bcm56780_a0_variant_get(int unit, int *variant)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(variant, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (variant_get(unit, variant));

exit:
    SHR_FUNC_EXIT();
}

int
bcm56780_a0_ltsw_variant_drv_attach(int unit,
                                    bcm56780_a0_ltsw_variant_mod_t module)
{
    int index = 0;

    SHR_FUNC_ENTER(unit);

    if (module == BCM56780_A0_LTSW_VARIANT_INVALID ||
        module == BCM56780_A0_LTSW_VARIANT_MAX) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (variant_index_get(unit, module, &index));

    SHR_IF_ERR_VERBOSE_EXIT
        (variant_list_attach[index].drv_attach[module](unit));

exit:
    SHR_FUNC_EXIT();
}
