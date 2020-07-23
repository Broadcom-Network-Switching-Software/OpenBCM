/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    udf.c
 * Purpose: Manages XGS4 UDF_TCAM and UDF_OFFSET tables
 */


#include <shared/bsl.h>

#include <soc/field.h>

#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/error.h>

#include <shared/bitop.h>

#include <bcm/udf.h>
#include <bcm/field.h>
#include <bcm/error.h>
#include <bcm/module.h>

#include <bcm_int/esw/xgs4.h>
#include <bcm_int/esw/switch.h>
#include <bcm_int/esw/field.h>

#define OUTER_IP4_TYPE_VAL(x) (SOC_IS_TD_TT(x) || SOC_IS_TRIUMPH3(x)) ? 1 : 2
#define OUTER_IP6_TYPE_VAL(x) (SOC_IS_TD_TT(x) || SOC_IS_TRIUMPH3(x)) ? 5 : 4

/* WarmBoot Sensitive Macro : Need to bump up scache version if changes */
#define TD2_UDF_MAX_OBJECTS 0xff

#define UDF_MALLOC(_p_, _t_, _sz_, _desc_)                  \
    do {                                                    \
        if (!_p_) {                                         \
            (_p_) = (_t_ *) sal_alloc((_sz_), (_desc_));    \
        }                                                   \
        if (_p_) {                                          \
            sal_memset((_p_), 0, (_sz_));                   \
        }                                                   \
    } while(0)

#define UDF_UNLINK_OFFSET_NODE(_t_) \
    do {                                                    \
        if ((_t_)->prev) {                                   \
            (_t_)->prev->next = (_t_)->next;                 \
        } else {                                            \
            UDF_CTRL(unit)->offset_info_head = (_t_)->next; \
        }                                                   \
        if ((_t_)->next) {                                  \
            ((_t_)->next)->prev = (_t_)->prev;              \
        }                                                   \
    } while(0)

#define UDF_UNLINK_TCAM_NODE(_t_) \
    do {                                                    \
        if ((_t_)->prev) {                                   \
            (_t_)->prev->next = (_t_)->next;                 \
        } else {                                            \
            UDF_CTRL(unit)->tcam_info_head = (_t_)->next;   \
        }                                                   \
        if ((_t_)->next) {                                  \
            ((_t_)->next)->prev = (_t_)->prev;              \
        }                                                   \
    } while(0)

#define BCM_IF_NULL_RETURN_PARAM(_p_) \
    do {                                                    \
        if ((_p_) == NULL) {                                \
            return BCM_E_PARAM;                             \
        }                                                   \
    } while(0)


#define UDF_IF_ERROR_CLEANUP(_rv_) \
    do {                                                    \
        if (BCM_FAILURE(rv)) {                              \
            goto cleanup;                                   \
        }                                                   \
    } while (0)

#define UDF_ID_VALIDATE(_id_) \
    do {                                                    \
        if (((_id_) < BCMI_XGS4_UDF_ID_MIN) ||              \
            ((_id_) > BCMI_XGS4_UDF_ID_MAX)) {              \
            return BCM_E_PARAM;                             \
        }                                                   \
    } while(0)
#define UDF_ID_VALIDATE_UNLOCK(unit, _id_)                  \
    do {                                                    \
        if (((_id_) < BCMI_XGS4_UDF_ID_MIN) ||              \
            ((_id_) > BCMI_XGS4_UDF_ID_MAX)) {              \
            UDF_UNLOCK(unit);                               \
            return BCM_E_PARAM;                             \
        }                                                   \
    } while(0)

#define UDF_PKT_FORMAT_ID_VALIDATE(_id_) \
    do {                                                    \
        if (((_id_) < BCMI_XGS4_UDF_PKT_FORMAT_ID_MIN) ||   \
            ((_id_) > BCMI_XGS4_UDF_PKT_FORMAT_ID_MAX)) {   \
            return BCM_E_PARAM;                             \
        }                                                   \
    } while(0)

#define UDF_PKT_FORMAT_ID_VALIDATE_UNLOCK(unit, _id_)       \
    do {                                                    \
        if (((_id_) < BCMI_XGS4_UDF_PKT_FORMAT_ID_MIN) ||   \
            ((_id_) > BCMI_XGS4_UDF_PKT_FORMAT_ID_MAX)) {   \
            UDF_UNLOCK(unit);                               \
            return BCM_E_PARAM;                             \
        }                                                   \
    } while(0)


/* Global UDF control structure pointers */
bcmi_xgs4_udf_ctrl_t *udf_control[BCM_MAX_NUM_UNITS];

typedef enum {
    bcmiUdfObjectUdf = 1,
    bcmiUdfObjectPktFormat = 2
} bcmiUdfObjectType;

typedef enum bcmi_xgs4_udf_pkt_format_misc_e {
    bcmiUdfPktFormatHigig = 0,
    bcmiUdfPktFormatCntag = 1,
    bcmiUdfPktFormatIcnm = 2,
    bcmiUdfPktFormatVntag = 3,
    bcmiUdfPktFormatEtag = 4,
    bcmiUdfPktFormatSubportTag = 5,
    bcmiUdfPktFormatIntPkt = 6,
    bcmiUdfPktFormatNone
} bcmi_xgs4_udf_pkt_format_misc_t;



/* STATIC function declarations */
STATIC int bcmi_xgs4_udf_ctrl_init(int unit);

STATIC int bcmi_xgs4_udf_hw_init(int unit);

STATIC int bcmi_xgs4_udf_offset_reserve(int unit, int max, int offset[],
                                        int pipe_num);

STATIC int bcmi_xgs4_udf_offset_unreserve(int unit, int max, int offset[],
                                          int pipe_num);

STATIC int bcmi_xgs4_udf_offset_hw_alloc(int unit,
    bcm_udf_alloc_hints_t *hints,
    bcmi_xgs4_udf_offset_info_t *offset_info);

STATIC int bcmi_xgs4_udf_layer_to_offset_base(int unit,
    bcmi_xgs4_udf_offset_info_t *offset_info,
    bcmi_xgs4_udf_tcam_info_t *tcam_info,
    int *base, int *baseoffset);

STATIC int bcmi_xgs4_udf_offset_info_alloc(int unit,
    bcmi_xgs4_udf_offset_info_t **offset_info);

STATIC int bcmi_xgs4_udf_offset_info_add(int unit,
    bcm_udf_t *udf_info, bcmi_xgs4_udf_offset_info_t **offset_info);

STATIC int bcmi_xgs4_udf_offset_node_add(
    int unit, bcmi_xgs4_udf_offset_info_t *new);

STATIC int bcmi_xgs4_udf_offset_node_delete(int unit,
    bcm_udf_id_t udf_id, bcmi_xgs4_udf_offset_info_t **del);

STATIC int bcmi_xgs4_udf_tcam_info_alloc(int unit,
    bcmi_xgs4_udf_tcam_info_t **tcam_info);

STATIC int bcmi_xgs4_udf_tcam_info_add(int unit,
    bcm_udf_pkt_format_info_t *pkt_format,
    bcmi_xgs4_udf_tcam_info_t **tcam_info);

STATIC int bcmi_xgs4_udf_tcam_node_add(
    int unit, bcmi_xgs4_udf_tcam_info_t *new);

STATIC int bcmi_xgs4_udf_tcam_node_delete(int unit,
    bcm_udf_pkt_format_id_t pkt_format_id, bcmi_xgs4_udf_tcam_info_t **del);

STATIC int bcmi_xgs4_udf_id_running_id_alloc(int unit,
    bcmiUdfObjectType type, int *id);

STATIC int bcmi_xgs4_udf_offset_to_hw_field(int unit, int udf_num, int user_num,
    soc_field_t *base_hw_f, soc_field_t *offset_hw_f);

STATIC int bcmi_xgs4_udf_offset_install(int unit,
    int e, uint32 hw_bmap, int base, int offset, int pipe_num, int class_id, uint8 flags);

#if defined (BCM_TOMAHAWK_SUPPORT)
int bcmi_xgs4_udf_ser_init(int unit,
                         bcm_udf_oper_mode_t mode);
#endif
/* tcam help apis */
STATIC int bcmi_xgs4_udf_tcam_entry_move(int unit,
    bcmi_xgs4_udf_tcam_entry_t *tcam_entry_arr, int src, int dst, int pipe_num);

STATIC int bcmi_xgs4_udf_tcam_move_up(int unit,
    bcmi_xgs4_udf_tcam_entry_t *tcam_entry_arr, int dest, int free_slot, int pipe_num);

STATIC int bcmi_xgs4_udf_tcam_move_down(int unit,
    bcmi_xgs4_udf_tcam_entry_t *tcam_entry_arr, int dest, int free_slot, int pipe_num);

STATIC int bcmi_xgs4_udf_tcam_entry_match(int unit,
    bcmi_xgs4_udf_tcam_info_t *new,  bcmi_xgs4_udf_tcam_info_t **match);

STATIC int bcmi_xgs4_udf_tcam_entry_insert(int unit,
    bcmi_xgs4_udf_tcam_info_t *tcam_new);

STATIC int bcmi_xgs4_udf_tcam_entry_vlanformat_parse(int unit,
    uint32 *hw_buf, bcm_udf_pkt_format_info_t *pkt_fmt);

STATIC int bcmi_xgs4_udf_tcam_entry_l2format_parse(int unit,
    uint32 *hw_buf, bcm_udf_pkt_format_info_t *pkt_fmt);

STATIC int bcmi_xgs4_udf_tcam_entry_l3_parse(int unit,
    uint32 *hw_buf, bcm_udf_pkt_format_info_t *pkt_fmt);

STATIC int bcmi_xgs4_udf_tcam_entry_misc_parse(int unit,
    int type, uint32 *hw_buf, uint16 *flags);

STATIC int bcmi_xgs4_udf_tcam_entry_vlanformat_init(int unit,
    bcm_udf_pkt_format_info_t *pkt_fmt, uint32 *hw_buf);

STATIC int bcmi_xgs4_udf_tcam_entry_l2format_init(int unit,
    bcm_udf_pkt_format_info_t *pkt_fmt, uint32 *hw_buf);

STATIC int bcmi_xgs4_udf_tcam_entry_l3_init(int unit,
    bcm_udf_pkt_format_info_t *pkt_fmt, uint32 *hw_buf);

STATIC int bcmi_xgs4_udf_tcam_entry_misc_init(int unit,
    int type, uint16 flags, uint32 *hw_buf);

STATIC int bcmi_xgs4_udf_tcam_entry_parse(int unit,
    uint32 *hw_buf, bcm_udf_pkt_format_info_t *pkt_fmt);

STATIC int bcmi_xgs4_udf_pkt_format_tcam_key_init(int unit,
    bcm_udf_pkt_format_info_t *pkt_format, uint32 *hw_buf);

STATIC int bcmi_xgs4_udf_tcam_misc_format_to_hw_fields(int unit, int type,
    soc_field_t *data_f, soc_field_t *mask_f,
    int *present, int *none, int *any, int *support);

STATIC int bcmi_xgs4_udf_hash_config_set(int unit,
    bcm_udf_hash_config_t *config, bcmi_xgs4_udf_offset_info_t *offset_info);

/* static global variable */
#if defined (BCM_TOMAHAWK_SUPPORT)
STATIC bcm_udf_oper_mode_t bcmi_xgs4_udf_oper_mode[BCM_MAX_NUM_UNITS] = {bcmUdfOperModeGlobal};
#endif

/*
 * Function:
 *      bcmi_xgs4_udf_ctrl_free
 * Purpose:
 *      Frees udf control structure and its members.
 * Parameters:
 *      unit            - (IN)  Unit number.
 *      udfc            - (IN)  UDF Control structure to free.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_xgs4_udf_ctrl_free(int unit, bcmi_xgs4_udf_ctrl_t *udfc)
{
    bcmi_xgs4_udf_tcam_info_t *prv_t, *cur_t;
    bcmi_xgs4_udf_offset_info_t *prv_o, *cur_o;
    int pipe_id = 0;

#if defined (BCM_TOMAHAWK_SUPPORT)
    int pipe;
#endif

    if (NULL == udfc) {
        /* Already freed */
        return BCM_E_NONE;
    }

    /* Free mutex */
    if (NULL != udfc->udf_mutex) {
        sal_mutex_destroy(udfc->udf_mutex);
        udfc->udf_mutex = NULL;
    }

#if defined (BCM_TOMAHAWK_SUPPORT)
    if (soc_feature(unit, soc_feature_udf_multi_pipe_support)) {
        for (pipe = 0; pipe < SOC_MAX_NUM_PIPES; pipe++) {
            if (NULL != udfc->tcam_entry_array_per_pipe[pipe]) {
                sal_free(udfc->tcam_entry_array_per_pipe[pipe]);
                udfc->tcam_entry_array_per_pipe[pipe] = NULL;
            }
            if (NULL != udfc->offset_entry_array_per_pipe[pipe]) {
                sal_free(udfc->offset_entry_array_per_pipe[pipe]);
                udfc->offset_entry_array_per_pipe[pipe] = NULL;
            }
        }
    }
#endif
    /* Free tcam entry array */
    if (NULL != udfc->tcam_entry_array) {
        sal_free(udfc->tcam_entry_array);
        udfc->tcam_entry_array = NULL;
    }

    /* Free offset entry array */
    if (NULL != udfc->offset_entry_array) {
        sal_free(udfc->offset_entry_array);
        udfc->offset_entry_array = NULL;
    }

    /* Free list of pkt format objects */
    cur_o = udfc->offset_info_head;

    while (NULL != cur_o) {
        prv_o = cur_o;
        cur_o = cur_o->next;

        sal_free(prv_o);
    }
    udfc->offset_info_head = NULL;

    /* Free list of offset objects */
    cur_t = udfc->tcam_info_head;

    while (NULL != cur_t) {
        prv_t = cur_t;
        cur_t = cur_t->next;

        sal_free(prv_t);
    }
    udfc->tcam_info_head = NULL;

    /* Clean up udf field module resources */
    if (soc_feature(unit, soc_feature_udf_selector_support)) {
        for (pipe_id = 0; pipe_id < SOC_MAX_NUM_PIPES; pipe_id++) {
            if (NULL != udfc->udf_field_bmp[pipe_id].w) {
                _BCMI_UDF_BMP_FREE(udfc->udf_field_bmp[pipe_id]);
            }
            udfc->max_chunks = 0;
        }
    }
    /* Free the udf control structure */
    sal_free(udfc);

    udf_control[unit] = NULL;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs4_udf_ctrl_init
 * Purpose:
 *      Initialize UDF control and internal data structures.
 * Parameters:
 *      unit           - (IN) bcm device.
 *
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
bcmi_xgs4_udf_ctrl_init(int unit)
{
    int alloc_sz;
    soc_mem_t tcam_mem, offset_mem;
    int nentries, noffsets;
    bcmi_xgs4_udf_ctrl_t *udf_ctrl = NULL;
    int pipe_id = 0;
#if defined (BCM_TOMAHAWK_SUPPORT)
    int pipe;
#endif
    tcam_mem = FP_UDF_TCAMm;
    offset_mem = FP_UDF_OFFSETm;

    /* Detach module if already installed */
    if (udf_control[unit] != NULL) {
        bcmi_xgs4_udf_detach(unit);
    }

    nentries = soc_mem_index_count(unit, tcam_mem);

    /* No. of maximum offset chunks supported */
    if (SOC_MEM_FIELD_VALID(unit, offset_mem, UDF1_OFFSET4f)) {
        noffsets = UDF_OFFSET_CHUNKS16;
    } else {
        noffsets = UDF_OFFSET_CHUNKS8;
    }

    if (udf_control[unit] != NULL) {
        (void)bcmi_xgs4_udf_ctrl_free(unit, udf_control[unit]);
    }

   /* Allocating memory of UDF control structure */
    alloc_sz = sizeof(bcmi_xgs4_udf_ctrl_t);
    UDF_MALLOC(udf_ctrl, bcmi_xgs4_udf_ctrl_t, alloc_sz, "udf control");
    if (udf_ctrl == NULL) {
        return BCM_E_MEMORY;
    }

    udf_control[unit] = udf_ctrl;

    /* Allocate memory for tcam_entry_array structure */
#if defined (BCM_TOMAHAWK_SUPPORT)
    if (soc_feature(unit, soc_feature_udf_multi_pipe_support) &&
       (bcmi_xgs4_udf_oper_mode[unit] == bcmUdfOperModePipeLocal)) {
        sal_memset(udf_ctrl->hw_bmap_per_pipe,   0, sizeof(udf_ctrl->hw_bmap_per_pipe));
        sal_memset(udf_ctrl->byte_bmap_per_pipe, 0, sizeof(udf_ctrl->byte_bmap_per_pipe));

        /* Allocate memory for tcam_entry_array structure */
        alloc_sz = sizeof(bcmi_xgs4_udf_tcam_entry_t) * nentries;
        for (pipe = 0; pipe < SOC_MAX_NUM_PIPES; pipe++) {
            UDF_MALLOC(udf_ctrl->tcam_entry_array_per_pipe[pipe], bcmi_xgs4_udf_tcam_entry_t,
                   alloc_sz, "udf tcam entry array per pipe");
            if (udf_ctrl->tcam_entry_array_per_pipe[pipe] == NULL) {
                /* Free udf control struct */
                (void)bcmi_xgs4_udf_ctrl_free(unit, udf_ctrl);
                return BCM_E_MEMORY;
            }
        }

        /* Allocate memory for offset_entry_array structure */
        alloc_sz = sizeof(bcmi_xgs4_udf_offset_entry_t) * noffsets;
        for (pipe = 0; pipe < SOC_MAX_NUM_PIPES; pipe++) {
            UDF_MALLOC(udf_ctrl->offset_entry_array_per_pipe[pipe], bcmi_xgs4_udf_offset_entry_t,
                   alloc_sz, "udf offset entry array per pipe");

            if (udf_ctrl->offset_entry_array_per_pipe[pipe] == NULL) {
                /* Free udf control struct */
                (void)bcmi_xgs4_udf_ctrl_free(unit, udf_ctrl);
                return BCM_E_MEMORY;
            }
        }
    } else
#endif
    {
        /* Allocate memory for tcam_entry_array structure */
        alloc_sz = sizeof(bcmi_xgs4_udf_tcam_entry_t) * nentries;
        UDF_MALLOC(udf_ctrl->tcam_entry_array, bcmi_xgs4_udf_tcam_entry_t,
               alloc_sz, "udf tcam entry array");
        if (udf_ctrl->tcam_entry_array == NULL) {
            /* Free udf control struct */
            (void)bcmi_xgs4_udf_ctrl_free(unit, udf_ctrl);
            return BCM_E_MEMORY;
        }

        /* Allocate memory for offset_entry_array structure */
        alloc_sz = sizeof(bcmi_xgs4_udf_offset_entry_t) * noffsets;
        UDF_MALLOC(udf_ctrl->offset_entry_array, bcmi_xgs4_udf_offset_entry_t,
                   alloc_sz, "udf offset entry array");
        if (udf_ctrl->offset_entry_array == NULL) {
            /* Free udf control struct */
            (void)bcmi_xgs4_udf_ctrl_free(unit, udf_ctrl);
            return BCM_E_MEMORY;
        }
    }

    /* Create UDF lock */
    if (udf_ctrl->udf_mutex == NULL) {
        udf_ctrl->udf_mutex = sal_mutex_create("udf_mutex");
        if (udf_ctrl->udf_mutex == NULL) {
            /* Free udf control struct */
            (void)bcmi_xgs4_udf_ctrl_free(unit, udf_ctrl);
            return BCM_E_MEMORY;
        }
    }


    udf_ctrl->tcam_mem = tcam_mem;
    udf_ctrl->offset_mem = offset_mem;

    /* Check if GRE options adjust for UDF selection is supported */
    if (SOC_MEM_FIELD_VALID(unit, offset_mem, UDF1_ADD_GRE_OPTIONS0f)) {
        udf_ctrl->flags |= BCMI_XGS4_UDF_CTRL_OFFSET_ADJUST_GRE;
    }

    /* Check if IPV4 options adjust for UDF selection is supported */
    if (SOC_MEM_FIELD_VALID(unit, offset_mem, UDF1_ADD_IPV4_OPTIONS0f)) {
        udf_ctrl->flags |= BCMI_XGS4_UDF_CTRL_OFFSET_ADJUST_IP4;
    }
    if (SOC_MEM_FIELD_VALID(unit, offset_mem, UDF_CLASS_IDf)) {
        udf_ctrl->flags |= BCMI_XGS4_UDF_PKT_FORMAT_CLASS_ID;
    }

    if (SOC_MEM_FIELD_VALID(unit, tcam_mem, HIGIGf)) {
        udf_ctrl->flags |= BCMI_XGS4_UDF_CTRL_TCAM_HIGIG;
    }
    if (SOC_MEM_FIELD_VALID(unit, tcam_mem, VNTAG_PRESENTf)) {
        udf_ctrl->flags |= BCMI_XGS4_UDF_CTRL_TCAM_VNTAG;
    }
    if (SOC_MEM_FIELD_VALID(unit, tcam_mem, ETAG_PACKETf)) {
        udf_ctrl->flags |= BCMI_XGS4_UDF_CTRL_TCAM_ETAG;
    }
    if (SOC_MEM_FIELD_VALID(unit, tcam_mem, CNTAG_PRESENTf)) {
        udf_ctrl->flags |= BCMI_XGS4_UDF_CTRL_TCAM_CNTAG;
    }
    if (SOC_MEM_FIELD_VALID(unit, tcam_mem, ICNM_PACKETf)) {
        udf_ctrl->flags |= BCMI_XGS4_UDF_CTRL_TCAM_ICNM;
    }
    if (SOC_MEM_FIELD_VALID(unit, tcam_mem, SUBPORT_TAG_PRESENTf)) {
        udf_ctrl->flags |= BCMI_XGS4_UDF_CTRL_TCAM_SUBPORT_TAG;
    }
#if defined(BCM_TOMAHAWK_SUPPORT)
    if (SOC_MEM_FIELD_VALID(unit, offset_mem,
                            ENABLE_UDF_CONDITIONAL_CHECKf)) {
        udf_ctrl->flags |= BCMI_XGS4_UDF_CONDITIONAL_CHECK;
    }
#endif

#if defined(BCM_TRIDENT2_SUPPORT)
    /* Check if flex hash is supported */
    if soc_feature(unit, soc_feature_flex_hashing) {
        udf_ctrl->flags |= BCMI_XGS4_UDF_CTRL_FLEXHASH;
    }
    /* Check if udf hash is supported */
    if soc_feature(unit, soc_feature_udf_hashing) {
        udf_ctrl->flags |= BCMI_XGS4_UDF_CTRL_UDFHASH;
    }
#endif

#if defined(BCM_TOMAHAWK_SUPPORT)
    if (SOC_IS_TOMAHAWKX(unit)) {
        /* UDF Range Checker is supported only in Tomahawk. */
        udf_ctrl->flags |= BCMI_XGS4_UDF_CTRL_RANGE_CHECK;
    }
#endif
    if soc_feature(unit, soc_feature_advanced_flex_counter) {
        udf_ctrl->flags |= BCMI_XGS4_UDF_CTRL_FLEX_COUNTER;
    }

    if (SOC_MEM_FIELD_VALID(unit, tcam_mem, L4_DST_PORTf)) {
        udf_ctrl->flags |= BCMI_XGS4_UDF_CTRL_L4_DST_PORT;
    }
    if (SOC_MEM_FIELD_VALID(unit, tcam_mem, OPAQUE_TAG_TYPEf)) {
        udf_ctrl->flags |= BCMI_XGS4_UDF_CTRL_OPAQUE_TAG_TYPE;
    }
    if (SOC_MEM_FIELD_VALID(unit, tcam_mem, INT_PKTf)) {
        udf_ctrl->flags |= BCMI_XGS4_UDF_CTRL_INT_PKT;
    }
    if (SOC_MEM_FIELD_VALID(unit, tcam_mem, SOURCE_PORT_NUMBERf)) {
        udf_ctrl->flags |= BCMI_XGS4_UDF_CTRL_SRC_PORT;
    }
    if (SOC_MEM_FIELD_VALID(unit, tcam_mem, LOOPBACK_PKT_TYPEf)) {
        udf_ctrl->flags |= BCMI_XGS4_UDF_CTRL_LB_PKT_TYPE;
    }

    udf_ctrl->nentries = nentries;
    udf_ctrl->noffsets = noffsets;

    /* restriction merely for warmboot purposes */
    udf_ctrl->max_udfs = TD2_UDF_MAX_OBJECTS;

    /* restriction merely for byte selection */
    udf_ctrl->max_parse_bytes = BCMI_XGS4_UDF_MAX_BYTE_SELECTION;

    /* offset chunk granularity */
    if (SOC_MEM_FIELD_VALID(unit, offset_mem, UDF1_OFFSET4f)) {
        udf_ctrl->gran = UDF_OFFSET_GRAN2;
    } else {
        udf_ctrl->gran = UDF_OFFSET_GRAN4;
    }

    /* Check if policer group is supported */
    if (soc_feature(unit, soc_feature_global_meter)) {
        udf_ctrl->flags |= BCMI_XGS4_UDF_CTRL_POLICER_GROUP;
    }

    /* udf hash chunks */
#if defined(BCM_TRIDENT2_SUPPORT)
    if soc_feature(unit, soc_feature_udf_hashing) {
        udf_ctrl->udf_hash_1 = 6;
        udf_ctrl->udf_hash_2 = 7;
    }
#endif

    if (soc_feature(unit, soc_feature_udf_selector_support)) {
        /* Allocate udf field module related resources */
        for (pipe_id = 0; pipe_id < SOC_MAX_NUM_PIPES; pipe_id++) {
             udf_ctrl->max_chunks = 16;
             udf_ctrl->udf_field_bmp[pipe_id].w = NULL;
             _FP_XGS3_ALLOC(udf_ctrl->udf_field_bmp[pipe_id].w,
                            SHR_BITALLOCSIZE(udf_ctrl->max_chunks),
                            "UDF BMP");
             if (udf_ctrl->udf_field_bmp[pipe_id].w == NULL) {
                 return BCM_E_MEMORY;
             }
        }
    }

    udf_ctrl->udf_used_by_module = UDF_USED_BY_NONE;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs4_udf_hw_init
 * Purpose:
 *      Clears the hardware tables related to UDF.
 * Parameters:
 *      unit            - (IN)  Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_xgs4_udf_hw_init(int unit)
{
    int rv;

    /* Clear UDF_TCAM table */
    rv = soc_mem_clear(unit, UDF_CTRL(unit)->tcam_mem, MEM_BLOCK_ALL, TRUE);
    SOC_IF_ERROR_RETURN(rv);

    /* Clear UDF_OFFSET table */
    rv = soc_mem_clear(unit, UDF_CTRL(unit)->offset_mem, MEM_BLOCK_ALL, TRUE);
    SOC_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}


#if defined (BCM_WARM_BOOT_SUPPORT)
#define BCM_WB_VERSION_1_3     SOC_SCACHE_VERSION(1, 3)
#define BCM_WB_VERSION_1_2     SOC_SCACHE_VERSION(1, 2)
#define BCM_WB_VERSION_1_1     SOC_SCACHE_VERSION(1, 1)
#define BCM_WB_VERSION_1_0     SOC_SCACHE_VERSION(1, 0)
#define BCM_WB_VERSION_1_4     SOC_SCACHE_VERSION(1, 4)
#define BCM_WB_VERSION_1_5     SOC_SCACHE_VERSION(1, 5)
#define BCM_WB_DEFAULT_VERSION BCM_WB_VERSION_1_5

/* Data structure for UDF Offset entry */
struct bcmi_xgs4_wb_offset_entry_1_0 {
    uint8 flags;
    uint8 grp_id;
    uint8 num_pkt_formats;
};

/* Data structure for UDF offset info */
struct bcmi_xgs4_wb_offset_info_1_0 {

    uint8 layer;
    uint8 start;
    uint8 width;
    uint8 flags;

    uint16 num_pkt_formats;

    uint16 id;
    uint32 byte_bmap;
#if defined (BCM_TOMAHAWK_SUPPORT)
    bcm_pbmp_t associated_ports;
    uint32 associated_pipe;
#endif
};

/* Data structure for UDF tcam info */
struct bcmi_xgs4_wb_tcam_info_1_0 {
    uint16 hw_id;
    uint16 num_udfs;

    uint16 id;
    uint16 priority;

    uint16 udf_id_list[MAX_UDF_OFFSET_CHUNKS];
#if defined (BCM_TOMAHAWK_SUPPORT)
    uint32 associated_pipe;
#endif
};

#if defined (BCM_TOMAHAWK_SUPPORT)
struct bcmi_xgs4_wb_tcam_info_1_3 {
    uint16 hw_idx;
    uint32 class_id;
    uint32 associated_pipe;
};

struct bcmi_xgs4_wb_tcam_info_1_4 {
    uint16 hw_idx[SOC_MAX_NUM_PIPES];
    uint16 num_udfs[SOC_MAX_NUM_PIPES];
    uint32 offset_bmap[SOC_MAX_NUM_PIPES];
    uint32 associated_pipes[SOC_MAX_NUM_PIPES];
    uint16 udf_id_list[MAX_UDF_ID_LIST];
};
#endif

struct bcmi_xgs4_wb_tcam_info_1_5 {
    uint32 udf_field_bmp[SOC_MAX_NUM_PIPES];
    int max_chunks;
};

/* Returns required scache size in bytes for version = 1_0 */
STATIC int
bcmi_xgs4_udf_wb_scache_size_get_1_0(int unit, int *req_scache_size)
{
    int alloc_size = 0;
    bcmi_xgs4_udf_ctrl_t *udf_ctrl;

    udf_ctrl = UDF_CTRL(unit);

    /* Number of UDFs */
    alloc_size += sizeof(udf_ctrl->num_udfs);

    /* Number of Packet formats */
    alloc_size += sizeof(udf_ctrl->num_pkt_formats);

#if defined (BCM_TOMAHAWK_SUPPORT)
    if (SOC_IS_TOMAHAWKX(unit)) {
        /* operation mode */
        alloc_size += sizeof(bcmi_xgs4_udf_oper_mode[unit]);
    }
    if ((SOC_IS_TOMAHAWKX(unit)) &&
        (bcmi_xgs4_udf_oper_mode[unit] == bcmUdfOperModePipeLocal)) {

        alloc_size += (udf_ctrl->noffsets * SOC_MAX_NUM_PIPES *
                        sizeof (struct bcmi_xgs4_wb_offset_entry_1_0));
        /* offset nodes */
        alloc_size += (udf_ctrl->max_udfs * SOC_MAX_NUM_PIPES *
                        sizeof(struct bcmi_xgs4_wb_offset_info_1_0));

        /* tcam nodes */
        alloc_size += (udf_ctrl->nentries * SOC_MAX_NUM_PIPES *
                        sizeof(struct bcmi_xgs4_wb_tcam_info_1_0));

    } else
#endif
    {
        /* offset entries */
        alloc_size += (udf_ctrl->noffsets *
                       sizeof (struct bcmi_xgs4_wb_offset_entry_1_0));

        /* offset nodes */
        alloc_size += (udf_ctrl->max_udfs *
                        sizeof(struct bcmi_xgs4_wb_offset_info_1_0));

        /* tcam nodes */
        alloc_size += (udf_ctrl->nentries *
                        sizeof(struct bcmi_xgs4_wb_tcam_info_1_0));
    }

    *req_scache_size = alloc_size;

    return BCM_E_NONE;
}

/* Returns required scache size in bytes for version = 1_1 */
STATIC int
bcmi_xgs4_udf_wb_scache_size_get_1_1(int unit, int *req_scache_size)
{
    int alloc_size = 0;

    /* udf used by module */
    alloc_size += sizeof(UDF_CTRL(unit)->udf_used_by_module);

    *req_scache_size += alloc_size;

    return BCM_E_NONE;
}

#if defined (BCM_TOMAHAWK_SUPPORT)
/* Returns required scache size in bytes for version = 1_3 */
STATIC int
bcmi_xgs4_udf_wb_scache_size_get_1_3(int unit, int *req_scache_size)
{
    int alloc_size = 0;

    alloc_size += (UDF_CTRL(unit)->nentries * SOC_MAX_NUM_PIPES *
                        sizeof(struct bcmi_xgs4_wb_tcam_info_1_3));

    *req_scache_size += alloc_size;

    return BCM_E_NONE;
}

STATIC int
bcmi_xgs4_udf_wb_scache_size_get_1_4(int unit, int *req_scache_size)
{
    int alloc_size = 0;

    alloc_size = (UDF_CTRL(unit)->nentries *
                        sizeof(struct bcmi_xgs4_wb_tcam_info_1_4));

    *req_scache_size += alloc_size;

    return BCM_E_NONE;
}
#endif


STATIC int
bcmi_xgs4_udf_wb_scache_size_get_1_5(int unit, int *req_scache_size)
{

    *req_scache_size += sizeof(struct bcmi_xgs4_wb_tcam_info_1_5);

    return BCM_E_NONE;
}


/* get warmboot recover version */
STATIC int
bcmi_xgs4_udf_recovered_version_get(
    int unit,
    uint16 *recovered_ver)
{
    int     rv = BCM_E_INTERNAL;
    uint8   *scache_ptr;
    soc_scache_handle_t scache_handle;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_UDF, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 0, &scache_ptr,
                                 BCM_WB_DEFAULT_VERSION, recovered_ver);

    if (rv == BCM_E_NOT_FOUND) {
        *recovered_ver = BCM_WB_DEFAULT_VERSION;
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}

/* Recovers offsets info nodes from scache for version = 1_0 */
STATIC int
bcmi_xgs4_udf_wb_offset_info_reinit_1_0(int unit, int num_offset_nodes,
                                        uint8 **scache_ptr)
{
    int rv;
    int i;
    int val = 0;
    uint32 offset_reserve[MAX_UDF_OFFSET_CHUNKS] = {0};
    int gran, max_chunks;
    bcm_udf_t udf_info;
    bcmi_xgs4_udf_offset_info_t *offset_info = NULL;
    bcmi_xgs4_udf_offset_entry_t *offset_entry_array; /* offset entries array.  */
    struct bcmi_xgs4_wb_offset_info_1_0 *scache_offset_p;
    uint8  byte_offset = 0;
    uint16 recovered_ver = BCM_WB_DEFAULT_VERSION;
#if defined (BCM_TOMAHAWK_SUPPORT)
    int offset_num, port;
    soc_info_t  *si;
    si = &SOC_INFO(unit);
#endif

    BCM_IF_ERROR_RETURN(
        bcmi_xgs4_udf_recovered_version_get(
                                        unit,
                                        &recovered_ver));

    gran = BCMI_XGS4_UDF_CTRL_OFFSET_GRAN(unit);
    max_chunks = BCMI_XGS4_UDF_CTRL_MAX_UDF_CHUNKS(unit);

    scache_offset_p = (struct bcmi_xgs4_wb_offset_info_1_0 *)(*scache_ptr);

    /* Recover offset nodes from scache */
    while (num_offset_nodes > 0) {

        offset_info = NULL;
        sal_memset(offset_reserve, 0, sizeof(offset_reserve));

        udf_info.start = BYTES2BITS(scache_offset_p->start);
        udf_info.width = BYTES2BITS(scache_offset_p->width);
        udf_info.layer = scache_offset_p->layer;
#if defined (BCM_TOMAHAWK_SUPPORT)
        if ((soc_feature(unit, soc_feature_udf_multi_pipe_support)) &&
           (recovered_ver >= BCM_WB_VERSION_1_2)) {
            SOC_PBMP_ASSIGN(udf_info.ports, scache_offset_p->associated_ports);
        }
#endif

        rv = bcmi_xgs4_udf_offset_info_add(unit, &udf_info, &offset_info);
        BCM_IF_ERROR_RETURN(rv);

        offset_info->udf_id = scache_offset_p->id;
        offset_info->flags = scache_offset_p->flags;
        offset_info->num_pkt_formats = scache_offset_p->num_pkt_formats;
        offset_info->byte_bmap = scache_offset_p->byte_bmap;

#if defined (BCM_TOMAHAWK_SUPPORT) 
        if ((soc_feature(unit, soc_feature_udf_multi_pipe_support)) && 
           (bcmi_xgs4_udf_oper_mode[unit] == bcmUdfOperModePipeLocal)) {
            SOC_PBMP_ITER(udf_info.ports, port) {
                offset_info->associated_pipe = si->port_pipe[port];
                break;
            }
            offset_entry_array =
                    UDF_CTRL(unit)->offset_entry_array_per_pipe[offset_info->associated_pipe];
        } else
#endif
        {
            offset_entry_array = UDF_CTRL(unit)->offset_entry_array;
        }
        /* Compute byte offset again and store */
        if (gran == UDF_OFFSET_GRAN2) {
            byte_offset = offset_info->start % gran;
        } else {
            if (offset_info->layer == bcmUdfLayerL2Header) {
                byte_offset = (offset_info->start + 2) % gran;
            } else {
                byte_offset = offset_info->start % gran;
            }
        }
        offset_info->byte_offset = byte_offset;

        if (offset_info->udf_id > UDF_CTRL(unit)->udf_id_running) {
            UDF_CTRL(unit)->udf_id_running = offset_info->udf_id;
        }

        for (i = 0; i < max_chunks; i++) {
            SHR_BITTEST_RANGE(&(scache_offset_p->byte_bmap),
                               (i * gran), gran, val);
            if (val) {
                SHR_BITCOPY_RANGE(&offset_reserve[i], 0, &(scache_offset_p->byte_bmap), i*gran, gran);
                SHR_BITSET(&(offset_info->hw_bmap), i);
                offset_info->grp_id = offset_entry_array[i].grp_id;
            }
        }

        /* Mark the global bitmap as used */
        (void) bcmi_xgs4_udf_offset_reserve(unit, max_chunks, (int *)offset_reserve,
                                            offset_info->associated_pipe);

        scache_offset_p++;
        num_offset_nodes--;
    }

    *scache_ptr = (uint8 *)scache_offset_p;

/* clear invalid udf_offset cache recovery */
#if defined (BCM_TOMAHAWK_SUPPORT)
   if (soc_feature(unit, soc_feature_udf_multi_pipe_support)) {
       offset_num = soc_mem_index_count(unit, FP_UDF_OFFSETm);
       for(i=0;i<offset_num;i++){
           if (bcmi_xgs4_udf_oper_mode[unit] == bcmUdfOperModePipeLocal) {
               soc_mem_cache_invalidate(unit, FP_UDF_OFFSETm, MEM_BLOCK_ALL, i);
           }
           else{
               soc_mem_cache_invalidate(unit, FP_UDF_OFFSET_PIPE0m, MEM_BLOCK_ALL, i);
               soc_mem_cache_invalidate(unit, FP_UDF_OFFSET_PIPE1m, MEM_BLOCK_ALL, i);
               soc_mem_cache_invalidate(unit, FP_UDF_OFFSET_PIPE2m, MEM_BLOCK_ALL, i);
               soc_mem_cache_invalidate(unit, FP_UDF_OFFSET_PIPE3m, MEM_BLOCK_ALL, i);
           }
       }
}
#endif
    return BCM_E_NONE;
}

/* Recovers tcam info nodes from scache for version = 1_0 */

STATIC int
bcmi_xgs4_udf_wb_tcam_info_reinit_1_0(int unit, int num_tcam_nodes,
                                      uint8 **scache_ptr)
{
    uint32 *buffer;      /* Hw buffer to dma udf tcam. */
    uint32 *entry_ptr = 0;   /* Tcam entry pointer.    */
    int alloc_size;      /* Memory allocation size.    */
    int entry_size;      /* Single tcam entry size.    */
    soc_mem_t tcam_mem;  /* UDF_TCAM memory            */
    bcmi_xgs4_udf_tcam_entry_t *tcam_entry_arr; /* Tcam entries array.  */
    int idx;             /* Tcam entries iterator.     */
    int rv;              /* Operation return status.   */
    int udf_id;
    bcmi_xgs4_udf_offset_info_t *offset_info;
    bcmi_xgs4_udf_tcam_info_t *tcam_info = NULL;
    fp_udf_tcam_entry_t *tcam_buf;
    struct bcmi_xgs4_wb_tcam_info_1_0 *scache_tcam_p;
    uint16 recovered_ver = BCM_WB_DEFAULT_VERSION;

    BCM_IF_ERROR_RETURN(
        bcmi_xgs4_udf_recovered_version_get(
                                        unit,
                                        &recovered_ver));

    tcam_mem = UDF_CTRL(unit)->tcam_mem;
    tcam_entry_arr = UDF_CTRL(unit)->tcam_entry_array;

    entry_size = sizeof(fp_udf_tcam_entry_t);
    alloc_size = SOC_MEM_TABLE_BYTES(unit, tcam_mem);

    /* Allocate memory buffer. */
    buffer = soc_cm_salloc(unit, alloc_size, "Udf tcam");
    if (buffer == NULL) {
        return (BCM_E_MEMORY);
    }

    sal_memset(buffer, 0, alloc_size);

    /* Read table to the buffer. */
    rv = soc_mem_read_range(unit, tcam_mem, MEM_BLOCK_ANY,
                            soc_mem_index_min(unit, tcam_mem),
                            soc_mem_index_max(unit, tcam_mem), buffer);
    if (BCM_FAILURE(rv)) {
        soc_cm_sfree(unit, buffer);
        return (BCM_E_INTERNAL);
    }



    scache_tcam_p = (struct bcmi_xgs4_wb_tcam_info_1_0 *)(*scache_ptr);

    /* Recover tcam info from scache */
    while (num_tcam_nodes > 0) {

        tcam_info = sal_alloc(sizeof(bcmi_xgs4_udf_tcam_info_t), "tcam info");
        sal_memset(tcam_info, 0, sizeof(bcmi_xgs4_udf_tcam_info_t));

        idx = scache_tcam_p->hw_id;
        tcam_info->hw_idx = idx;
        tcam_info->pkt_format_id = scache_tcam_p->id;
        tcam_info->priority = scache_tcam_p->priority;
        tcam_info->num_udfs = scache_tcam_p->num_udfs;
#if defined (BCM_TOMAHAWK_SUPPORT)
        if ((soc_feature(unit, soc_feature_udf_multi_pipe_support)) &&
           (recovered_ver >= BCM_WB_VERSION_1_2) &&
           (bcmi_xgs4_udf_oper_mode[unit] == bcmUdfOperModePipeLocal)) {
            tcam_info->associated_pipe = scache_tcam_p->associated_pipe;
            if (tcam_info->associated_pipe == BCMI_XGS4_UDF_INVALID_PIPE_NUM) {
                return BCM_E_PARAM;
            }
            tcam_entry_arr = UDF_CTRL(unit)->tcam_entry_array_per_pipe[tcam_info->associated_pipe];
            tcam_mem = SOC_MEM_UNIQUE_ACC(unit, UDF_CTRL(unit)->tcam_mem)[tcam_info->associated_pipe];
        }
#endif
        entry_ptr = soc_mem_table_idx_to_pointer(unit, tcam_mem, uint32 *,
                                                 buffer, idx);

        tcam_buf = (fp_udf_tcam_entry_t *)&(tcam_info->hw_buf);
        sal_memcpy(tcam_buf, entry_ptr, entry_size);

        if (soc_mem_field32_get(unit, tcam_mem, entry_ptr, VALIDf)) {
            tcam_entry_arr[idx].valid = 1;
            tcam_entry_arr[idx].tcam_info = tcam_info;
        }

        /* Not installed entries will have 512 as hw_idx */
        if (idx < MAX_UDF_TCAM_ENTRIES) {

            for (idx = 0; idx < MAX_UDF_OFFSET_CHUNKS; idx++) {

                udf_id = scache_tcam_p->udf_id_list[idx];

                if (udf_id != 0) {
                    rv = bcmi_xgs4_udf_offset_node_get(unit, udf_id,
                                                       &offset_info);
                    /* Continue recovering other udfs added to the tcam entry */
                    if (BCM_SUCCESS(rv)) {
                        tcam_info->offset_info_list[offset_info->grp_id] =
                                                            offset_info;
                        tcam_info->offset_bmap |= offset_info->hw_bmap;
                        tcam_info->udf_id_list[idx] = offset_info->udf_id;
                    }
                }
            }
            if (tcam_info->pkt_format_id >
                UDF_CTRL(unit)->udf_pkt_format_id_running) {

                UDF_CTRL(unit)->udf_pkt_format_id_running =
                    tcam_info->pkt_format_id;
            }

        }

        /* Continue recovering other packet format entries even on failure */
        (void) bcmi_xgs4_udf_tcam_node_add(unit, tcam_info);

        scache_tcam_p++;
        num_tcam_nodes--;
    }

    *scache_ptr = (uint8 *)scache_tcam_p;

    soc_cm_sfree(unit, buffer);

    return BCM_E_NONE;
}


/* Syncs offset nodes info into scache for version = 1_0 */
STATIC int
bcmi_xgs4_udf_wb_offset_info_sync_1_0(int unit, int num_offset_nodes,
                                      uint8 **scache_ptr)
{
    bcmi_xgs4_udf_offset_info_t *offset_info;
    struct bcmi_xgs4_wb_offset_info_1_0 *scache_offset_p;

    scache_offset_p = (struct bcmi_xgs4_wb_offset_info_1_0 *)(*scache_ptr);

    offset_info = UDF_CTRL(unit)->offset_info_head;

    /* Recover offset nodes from scache */
    while (num_offset_nodes > 0) {

        scache_offset_p->layer = offset_info->layer;
        scache_offset_p->start = offset_info->start;
        scache_offset_p->width = offset_info->width;
        scache_offset_p->flags = offset_info->flags;

        scache_offset_p->num_pkt_formats = offset_info->num_pkt_formats;

        scache_offset_p->id = offset_info->udf_id;
        scache_offset_p->byte_bmap = offset_info->byte_bmap;
#if defined (BCM_TOMAHAWK_SUPPORT)
        if (soc_feature(unit, soc_feature_udf_multi_pipe_support)) {
            scache_offset_p->associated_pipe = offset_info->associated_pipe;
            SOC_PBMP_ASSIGN(scache_offset_p->associated_ports, offset_info->associated_ports);
        }
#endif
        num_offset_nodes--;
        scache_offset_p++;
        offset_info = offset_info->next;
    }

    *scache_ptr = (uint8 *)scache_offset_p;

    return BCM_E_NONE;
}

/* Syncs tcam nodes info into scache for version = 1_0 */
STATIC int
bcmi_xgs4_udf_wb_tcam_info_sync_1_0(int unit, int num_tcam_nodes,
                                    uint8 **scache_ptr)
{
    int idx;
    bcmi_xgs4_udf_tcam_info_t *tcam_info;
    struct bcmi_xgs4_wb_tcam_info_1_0 *scache_tcam_p;


    tcam_info = UDF_CTRL(unit)->tcam_info_head;
    scache_tcam_p = (struct bcmi_xgs4_wb_tcam_info_1_0 *)(*scache_ptr);

    /* Sync data to scache */
    while (num_tcam_nodes > 0) {

        idx = scache_tcam_p->hw_id;
        scache_tcam_p->hw_id = tcam_info->hw_idx;
        scache_tcam_p->id = tcam_info->pkt_format_id;
        scache_tcam_p->priority = tcam_info->priority;
        scache_tcam_p->num_udfs = tcam_info->num_udfs;
#if defined (BCM_TOMAHAWK_SUPPORT)
        if (soc_feature(unit, soc_feature_udf_multi_pipe_support)) {
            scache_tcam_p->associated_pipe = tcam_info->associated_pipe;
        }
#endif

        for (idx = 0; idx < MAX_UDF_OFFSET_CHUNKS; idx++) {
            if (tcam_info->offset_info_list[idx] != NULL) {
                scache_tcam_p->udf_id_list[idx] =
                    (tcam_info->offset_info_list[idx])->udf_id;
            } else {
                scache_tcam_p->udf_id_list[idx] = 0;
            }
        }

        scache_tcam_p++;
        num_tcam_nodes--;
        tcam_info = tcam_info->next;
    }

    *scache_ptr = (uint8 *)scache_tcam_p;

    return BCM_E_NONE;
}

#if defined (BCM_TOMAHAWK_SUPPORT) && defined (BCM_WB_VERSION_1_3)
STATIC int
bcmi_xgs4_udf_wb_offset_info_reinit_1_2_to_1_3
                            (int unit, int num_offset_nodes,
                             uint8 **scache_ptr,
                             struct bcmi_xgs4_wb_offset_entry_1_0 *offset_entry_scache_p)
{

    int rv;
    int i;
    int val = 0;
    uint32 offset_reserve[MAX_UDF_OFFSET_CHUNKS] = {0};
    int gran;
    bcm_udf_t udf_info;
    bcmi_xgs4_udf_offset_info_t   *offset_info = NULL;
    bcmi_xgs4_udf_offset_entry_t  *offset_entry_array; /* offset entries array.  */
    struct bcmi_xgs4_wb_offset_entry_1_0 *offset_entry_recover_p;
    struct bcmi_xgs4_wb_offset_info_1_0 *scache_offset_p;
    uint8  byte_offset = 0;
    int offset_num;
    int port = 0, max_chunks = 0;
    soc_info_t  *si;
    si = &SOC_INFO(unit);

    gran = BCMI_XGS4_UDF_CTRL_OFFSET_GRAN(unit);
    max_chunks = BCMI_XGS4_UDF_CTRL_MAX_UDF_CHUNKS(unit);

    offset_entry_recover_p = offset_entry_scache_p;
    for (i = 0; i < max_chunks; i++) {
        /* skip offset entry reinit */
        offset_entry_scache_p++;
    }
    scache_offset_p = (struct bcmi_xgs4_wb_offset_info_1_0 *)offset_entry_scache_p;

    /* Recover offset nodes from scache */
    while (num_offset_nodes > 0) {

        offset_info = NULL;
        sal_memset(offset_reserve, 0, sizeof(offset_reserve));

        udf_info.start = BYTES2BITS(scache_offset_p->start);
        udf_info.width = BYTES2BITS(scache_offset_p->width);
        udf_info.layer = scache_offset_p->layer;

        SOC_PBMP_ASSIGN(udf_info.ports, scache_offset_p->associated_ports);

        rv = bcmi_xgs4_udf_offset_info_add(unit, &udf_info, &offset_info);
        BCM_IF_ERROR_RETURN(rv);

        offset_info->udf_id = scache_offset_p->id;
        offset_info->flags = scache_offset_p->flags;
        offset_info->num_pkt_formats = scache_offset_p->num_pkt_formats;
        offset_info->byte_bmap = scache_offset_p->byte_bmap;

        SOC_PBMP_ITER(udf_info.ports, port) {
            offset_info->associated_pipe = si->port_pipe[port];
            break;
         }

         offset_entry_array =
                    UDF_CTRL(unit)->offset_entry_array_per_pipe[offset_info->associated_pipe];

       /* Compute byte offset again and store */
        if (gran == UDF_OFFSET_GRAN2) {
            byte_offset = offset_info->start % gran;
        } else {
            if (offset_info->layer == bcmUdfLayerL2Header) {
                byte_offset = (offset_info->start + 2) % gran;
            } else {
                byte_offset = offset_info->start % gran;
            }
        }
        offset_info->byte_offset = byte_offset;

        if (offset_info->udf_id > UDF_CTRL(unit)->udf_id_running) {
            UDF_CTRL(unit)->udf_id_running = offset_info->udf_id;
        }

        for (i = 0; i < max_chunks; i++) {
            SHR_BITTEST_RANGE(&(scache_offset_p->byte_bmap),
                               (i * gran), gran, val);
            offset_entry_scache_p = offset_entry_recover_p;
            if (val) {
                SHR_BITCOPY_RANGE(&offset_reserve[i], 0, &(scache_offset_p->byte_bmap), i*gran, gran);
                SHR_BITSET(&(offset_info->hw_bmap), i);

                offset_entry_scache_p += i;
                offset_entry_array[i].flags =  offset_entry_scache_p->flags;
                offset_entry_array[i].grp_id = offset_entry_scache_p->grp_id;
                offset_entry_array[i].num_pkt_formats = offset_entry_scache_p->num_pkt_formats;

                offset_info->grp_id = offset_entry_scache_p->grp_id;

            }
        }

        /* Mark the global bitmap as used */
        (void) bcmi_xgs4_udf_offset_reserve(unit, max_chunks, (int *)offset_reserve,
                                            offset_info->associated_pipe);

        scache_offset_p++;
        num_offset_nodes--;
    }

    *scache_ptr = (uint8 *)scache_offset_p;

/* clear invalid udf_offset cache recovery */
   offset_num = soc_mem_index_count(unit, FP_UDF_OFFSETm);
   for(i=0;i<offset_num;i++){
       if (bcmi_xgs4_udf_oper_mode[unit] == bcmUdfOperModePipeLocal) {
           soc_mem_cache_invalidate(unit, FP_UDF_OFFSETm, MEM_BLOCK_ALL, i);
       }
       else{
           soc_mem_cache_invalidate(unit, FP_UDF_OFFSET_PIPE0m, MEM_BLOCK_ALL, i);
           soc_mem_cache_invalidate(unit, FP_UDF_OFFSET_PIPE1m, MEM_BLOCK_ALL, i);
           soc_mem_cache_invalidate(unit, FP_UDF_OFFSET_PIPE2m, MEM_BLOCK_ALL, i);
           soc_mem_cache_invalidate(unit, FP_UDF_OFFSET_PIPE3m, MEM_BLOCK_ALL, i);
       }
    }

    rv = bcmi_xgs4_udf_wb_tcam_info_reinit_1_0(unit,
                UDF_CTRL(unit)->num_pkt_formats, scache_ptr);
    BCM_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}

/* Recovers UDF control state for offset multipipe support */
STATIC int
bcmi_xgs4_udf_wb_reinit_multipipe
                            (int unit,
                             uint8 **scache_ptr,
                             struct bcmi_xgs4_wb_offset_entry_1_0* offset_entry_scache_p,
                             uint16 recovered_ver, int max_chunks
                            )
{
    int pipe, i, rv;
    bcmi_xgs4_udf_offset_entry_t *offset_entry;

    if (recovered_ver >= BCM_WB_VERSION_1_3) {
        for (pipe = 0; pipe < SOC_MAX_NUM_PIPES; pipe++) {
            for (i = 0; i < max_chunks; i++) {
                offset_entry = &(UDF_CTRL(unit)->offset_entry_array_per_pipe[pipe][i]);
                offset_entry->flags = offset_entry_scache_p->flags;
                offset_entry->grp_id = offset_entry_scache_p->grp_id;
                offset_entry->num_pkt_formats = offset_entry_scache_p->num_pkt_formats;
                offset_entry_scache_p++;
            }
        }
    } else {
        rv = bcmi_xgs4_udf_wb_offset_info_reinit_1_2_to_1_3(unit,
                                          UDF_CTRL(unit)->num_udfs,
                                          scache_ptr, offset_entry_scache_p);
        return rv;
    }
    *scache_ptr = (uint8 *)offset_entry_scache_p;

    rv = bcmi_xgs4_udf_wb_offset_info_reinit_1_0(unit,
                 UDF_CTRL(unit)->num_udfs, scache_ptr);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcmi_xgs4_udf_wb_tcam_info_reinit_1_0(unit,
                UDF_CTRL(unit)->num_pkt_formats, scache_ptr);
    BCM_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;

}
#endif /* (BCM_TOMAHAWK_SUPPORT) && (BCM_WB_VERSION_1_3) */

/* Recovers UDF control state from scache for version = 1_0 */
STATIC int
bcmi_xgs4_udf_wb_reinit_1_0(int unit, uint8 **scache_ptr)
{
    int rv;
    int i;
    int max_chunks;
    bcmi_xgs4_udf_offset_entry_t *offset_entry;
    struct bcmi_xgs4_wb_offset_entry_1_0 *offset_entry_scache_p;
    uint32 *u32_scache_p;
    uint16 recovered_ver = BCM_WB_DEFAULT_VERSION;

    BCM_IF_ERROR_RETURN(
        bcmi_xgs4_udf_recovered_version_get(
                                        unit,
                                        &recovered_ver));

    max_chunks = UDF_CTRL(unit)->noffsets;

    u32_scache_p = (uint32 *)(*scache_ptr);

#if defined (BCM_TOMAHAWK_SUPPORT)
    if ((SOC_IS_TOMAHAWKX(unit)) &&
        (recovered_ver >= BCM_WB_VERSION_1_2)) {
        /* Operation Mode Retrieval */
        bcmi_xgs4_udf_oper_mode_set(unit,u32_scache_p[2]);
    }
#endif

    /* Number of UDFs created */
    UDF_CTRL(unit)->num_udfs = *u32_scache_p;
    u32_scache_p++;

    /* Number of Packet formats created */
    UDF_CTRL(unit)->num_pkt_formats = *u32_scache_p;
    u32_scache_p++;

#if defined (BCM_TOMAHAWK_SUPPORT)
    if ((SOC_IS_TOMAHAWKX(unit)) &&
        (recovered_ver >= BCM_WB_VERSION_1_2)) {
        /* Pass Operation Mode */
        u32_scache_p++;
    }
#endif
    offset_entry_scache_p =
        (struct bcmi_xgs4_wb_offset_entry_1_0 *)u32_scache_p;


#if defined (BCM_TOMAHAWK_SUPPORT) && defined (BCM_WB_VERSION_1_3)
    if ((SOC_IS_TOMAHAWKX(unit)) &&
        (bcmi_xgs4_udf_oper_mode[unit] == bcmUdfOperModePipeLocal)) {
        rv = bcmi_xgs4_udf_wb_reinit_multipipe(unit, scache_ptr, 
                                               offset_entry_scache_p, 
                                               recovered_ver, max_chunks);
        return rv;
    }
#endif
    for (i = 0; i < max_chunks; i++) {
        offset_entry = &(UDF_CTRL(unit)->offset_entry_array[i]);

        offset_entry->flags = offset_entry_scache_p->flags;
        offset_entry->grp_id = offset_entry_scache_p->grp_id;
        offset_entry->num_pkt_formats = offset_entry_scache_p->num_pkt_formats;

        offset_entry_scache_p++;
    }
    *scache_ptr = (uint8 *)offset_entry_scache_p;

    rv = bcmi_xgs4_udf_wb_offset_info_reinit_1_0(unit,
                 UDF_CTRL(unit)->num_udfs, scache_ptr);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcmi_xgs4_udf_wb_tcam_info_reinit_1_0(unit,
                UDF_CTRL(unit)->num_pkt_formats, scache_ptr);
    BCM_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}

/* Recovers UDF control state from scache for version = 1_1 */
STATIC int
bcmi_xgs4_udf_wb_reinit_1_1(int unit, uint8 **scache_ptr)
{
    uint32 *u32_scache_p;

    u32_scache_p = (uint32 *)(*scache_ptr);

    /* Udf resource of used module */
    UDF_CTRL(unit)->udf_used_by_module = *u32_scache_p;
    u32_scache_p++;

    *scache_ptr = (uint8 *)u32_scache_p;

    return BCM_E_NONE;
}

#if defined (BCM_TOMAHAWK_SUPPORT)
/* Recovers UDF control state from scache for version = 1_3 */
STATIC int
bcmi_xgs4_udf_wb_reinit_1_3(int unit, uint8 **scache_ptr)
{
    bcmi_xgs4_udf_tcam_entry_t *tcam_entry_arr; /* Tcam entries array.  */
    int num_tcam_nodes;
    struct bcmi_xgs4_wb_tcam_info_1_3 *scache_tcam_p;

    scache_tcam_p = (struct bcmi_xgs4_wb_tcam_info_1_3 *)(*scache_ptr);
    num_tcam_nodes = UDF_CTRL(unit)->num_pkt_formats;

    while (num_tcam_nodes > 0) {
        if (bcmi_xgs4_udf_oper_mode[unit] == bcmUdfOperModePipeLocal) {
            tcam_entry_arr = 
            UDF_CTRL(unit)->tcam_entry_array_per_pipe[scache_tcam_p->associated_pipe];
        } else {
            tcam_entry_arr = UDF_CTRL(unit)->tcam_entry_array;
        }
        if (tcam_entry_arr[scache_tcam_p->hw_idx].tcam_info != NULL) {
            tcam_entry_arr[scache_tcam_p->hw_idx].tcam_info->class_id = scache_tcam_p->class_id;
        }
        scache_tcam_p++;
        num_tcam_nodes--;
    }

    *scache_ptr = (uint8 *)scache_tcam_p;

    return BCM_E_NONE;
}

STATIC int
bcmi_xgs4_udf_wb_tcam_info_reinit_1_2_to_1_4(int unit)
{
    int idx;
    int pipe;
    bcmi_xgs4_udf_tcam_info_t *tcam_info;

    if (soc_feature(unit, soc_feature_udf_multi_pipe_support) &&
        bcmi_xgs4_udf_oper_mode[unit] == bcmUdfOperModePipeLocal) {
        tcam_info = UDF_CTRL(unit)->tcam_info_head;

        while (tcam_info) {
            for (idx = 0; idx < SOC_MAX_NUM_PIPES; idx++) {
                tcam_info->hw_idx_per_pipe[idx] = MAX_UDF_TCAM_ENTRIES;
                tcam_info->associated_pipes[idx] = BCMI_XGS4_UDF_INVALID_PIPE_NUM;
            }

            pipe = tcam_info->associated_pipe;
            tcam_info->hw_idx_per_pipe[pipe] = tcam_info->hw_idx;
            tcam_info->num_udfs_per_pipe[pipe] = tcam_info->num_udfs;
            tcam_info->offset_bmap_per_pipe[pipe] = tcam_info->offset_bmap;
            tcam_info->associated_pipes[pipe] = pipe;
            tcam_info = tcam_info->next;
        }
    }

    return BCM_E_NONE;
}

STATIC int
bcmi_xgs4_udf_wb_reinit_1_4(int unit, uint8 **scache_ptr)
{
    int idx;
    int pipe;
    int hw_idx;
    soc_mem_t tcam_mem;
    bcmi_xgs4_udf_tcam_info_t *tcam_info;
    bcmi_xgs4_udf_tcam_entry_t *tcam_entry_arr;
    struct bcmi_xgs4_wb_tcam_info_1_4 *scache_tcam_p;

    if (soc_feature(unit, soc_feature_udf_multi_pipe_support) &&
        bcmi_xgs4_udf_oper_mode[unit] == bcmUdfOperModePipeLocal) {
        tcam_info = UDF_CTRL(unit)->tcam_info_head;
        scache_tcam_p = (struct bcmi_xgs4_wb_tcam_info_1_4*)(*scache_ptr);

        while (tcam_info) {
            for (idx = 0; idx < SOC_MAX_NUM_PIPES; idx++) {
                tcam_info->hw_idx_per_pipe[idx] = scache_tcam_p->hw_idx[idx];
                tcam_info->num_udfs_per_pipe[idx] = scache_tcam_p->num_udfs[idx];
                tcam_info->offset_bmap_per_pipe[idx] = scache_tcam_p->offset_bmap[idx];
                tcam_info->associated_pipes[idx] = scache_tcam_p->associated_pipes[idx];

                pipe = tcam_info->associated_pipes[idx];
                hw_idx = tcam_info->hw_idx_per_pipe[idx];
                if (hw_idx == MAX_UDF_TCAM_ENTRIES ||
                    pipe == BCMI_XGS4_UDF_INVALID_PIPE_NUM) {
                    continue;
                }

                tcam_entry_arr = UDF_CTRL(unit)->tcam_entry_array_per_pipe[pipe];
                tcam_mem = SOC_MEM_UNIQUE_ACC(unit, UDF_CTRL(unit)->tcam_mem)[pipe];

                BCM_IF_ERROR_RETURN(
                    soc_mem_read(unit, tcam_mem, MEM_BLOCK_ALL, hw_idx, &tcam_info->hw_buf));

                if (soc_mem_field32_get(unit, tcam_mem, &tcam_info->hw_buf, VALIDf)) {
                    tcam_entry_arr[hw_idx].valid = 1;
                    tcam_entry_arr[hw_idx].tcam_info = tcam_info;
                }
            }

            for (idx = 0; idx < MAX_UDF_ID_LIST; idx++) {
                tcam_info->udf_id_list[idx] = scache_tcam_p->udf_id_list[idx];
            }

            scache_tcam_p++;
            tcam_info = tcam_info->next;
        }

        *scache_ptr = (uint8 *)scache_tcam_p;
    }

    return BCM_E_NONE;
}
#endif

STATIC int
bcmi_xgs4_udf_wb_reinit_1_5(int unit, uint8 **scache_ptr)
{
    int idx;
    struct bcmi_xgs4_wb_tcam_info_1_5 *scache_tcam_p;

    scache_tcam_p = (struct bcmi_xgs4_wb_tcam_info_1_5*)(*scache_ptr);

    for (idx = 0; idx < SOC_MAX_NUM_PIPES; idx++) {
        sal_memcpy(UDF_CTRL(unit)->udf_field_bmp[idx].w,
                   &(scache_tcam_p->udf_field_bmp[idx]),
                   sizeof(uint32));
    }
    UDF_CTRL(unit)->max_chunks = scache_tcam_p->max_chunks;

    scache_tcam_p++;
    *scache_ptr = (uint8 *)scache_tcam_p;


    return BCM_E_NONE;
}

/* Syncs UDF control state into scache for version = 1_0 */
STATIC int
bcmi_xgs4_udf_wb_sync_1_0(int unit, uint8 **scache_ptr)
{
    int rv;
    int i;
    int max_chunks;
    struct bcmi_xgs4_wb_offset_entry_1_0 *offset_entry_scache_p;
    uint32 *u32_scache_p;

#if defined (BCM_TOMAHAWK_SUPPORT)
    int pipe = 0;
#endif
    max_chunks = UDF_CTRL(unit)->noffsets;

    u32_scache_p = (uint32 *)(*scache_ptr);

    /* Number of UDFs created */
    *u32_scache_p = UDF_CTRL(unit)->num_udfs;
    u32_scache_p++;

    /* Number of Packet formats created */
    *u32_scache_p = UDF_CTRL(unit)->num_pkt_formats;
    u32_scache_p++;

#if defined (BCM_TOMAHAWK_SUPPORT)
    if (SOC_IS_TOMAHAWKX(unit)) {
        /* saving operation mode */
        *u32_scache_p = bcmi_xgs4_udf_oper_mode[unit] ;
        u32_scache_p++;
    }
#endif

    offset_entry_scache_p =
        (struct bcmi_xgs4_wb_offset_entry_1_0 *)u32_scache_p;

#if defined (BCM_TOMAHAWK_SUPPORT)
    if ((soc_feature(unit, soc_feature_udf_multi_pipe_support)) &&
        (bcmi_xgs4_udf_oper_mode[unit] == bcmUdfOperModePipeLocal)) {

        for (pipe = 0; pipe < SOC_MAX_NUM_PIPES; pipe++) {
            for (i = 0; i < max_chunks; i++) {
                offset_entry_scache_p->flags =
                    (UDF_CTRL(unit)->offset_entry_array_per_pipe[pipe][i]).flags;
                offset_entry_scache_p->grp_id =
                    (UDF_CTRL(unit)->offset_entry_array_per_pipe[pipe][i]).grp_id;
                offset_entry_scache_p->num_pkt_formats =
                    (UDF_CTRL(unit)->offset_entry_array_per_pipe[pipe][i]).num_pkt_formats;
                offset_entry_scache_p++;
            }
        }
    } else
#endif
    {
        for (i = 0; i < max_chunks; i++) {
            offset_entry_scache_p->flags =
                    (UDF_CTRL(unit)->offset_entry_array[i]).flags;
            offset_entry_scache_p->grp_id =
                    (UDF_CTRL(unit)->offset_entry_array[i]).grp_id;
            offset_entry_scache_p->num_pkt_formats =
                    (UDF_CTRL(unit)->offset_entry_array[i]).num_pkt_formats;

            offset_entry_scache_p++;
        }
    }

    *scache_ptr = (uint8 *)offset_entry_scache_p;

    rv = bcmi_xgs4_udf_wb_offset_info_sync_1_0(unit,
                UDF_CTRL(unit)->num_udfs, scache_ptr);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcmi_xgs4_udf_wb_tcam_info_sync_1_0(unit,
                UDF_CTRL(unit)->num_pkt_formats, scache_ptr);
    BCM_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}

/* Syncs UDF control state into scache for version = 1_1 */
STATIC int
bcmi_xgs4_udf_wb_sync_1_1(int unit, uint8 **scache_ptr)
{
    uint32 *u32_scache_p;

    u32_scache_p = (uint32 *)(*scache_ptr);

    /* Udf resource of used module */
    *u32_scache_p = UDF_CTRL(unit)->udf_used_by_module;
    u32_scache_p++;

    *scache_ptr = (uint8 *)u32_scache_p;

    return BCM_E_NONE;
}

#if defined (BCM_TOMAHAWK_SUPPORT)
/* Syncs UDF control state into scache for version = 1_3 */
STATIC int
bcmi_xgs4_udf_wb_sync_1_3(int unit, uint8 **scache_ptr)
{
    int num_tcam_nodes;
    bcmi_xgs4_udf_tcam_info_t *tcam_info;
    struct bcmi_xgs4_wb_tcam_info_1_3 *scache_tcam_p;

    tcam_info = UDF_CTRL(unit)->tcam_info_head;
    scache_tcam_p = (struct bcmi_xgs4_wb_tcam_info_1_3 *)(*scache_ptr);
    num_tcam_nodes = UDF_CTRL(unit)->num_pkt_formats;

    /* Sync data to scache */
    while (num_tcam_nodes > 0) {
        scache_tcam_p->hw_idx = tcam_info->hw_idx;
        scache_tcam_p->class_id = tcam_info->class_id;
        /* in global mode, this should be always 0 */
        scache_tcam_p->associated_pipe=tcam_info->associated_pipe;
        scache_tcam_p++;
        num_tcam_nodes--;
        tcam_info = tcam_info->next;
    }

    *scache_ptr = (uint8 *)scache_tcam_p;

    return BCM_E_NONE;
}

STATIC int
bcmi_xgs4_udf_wb_sync_1_4(int unit, uint8 **scache_ptr)
{
    int idx;
    bcmi_xgs4_udf_tcam_info_t *tcam_info;
    struct bcmi_xgs4_wb_tcam_info_1_4 *scache_tcam_p;

    if ((soc_feature(unit, soc_feature_udf_multi_pipe_support)) &&
        (bcmi_xgs4_udf_oper_mode[unit] == bcmUdfOperModePipeLocal)) {
        tcam_info = UDF_CTRL(unit)->tcam_info_head;
        scache_tcam_p = (struct bcmi_xgs4_wb_tcam_info_1_4*)(*scache_ptr);

        while (tcam_info) {
            for (idx = 0; idx < SOC_MAX_NUM_PIPES; idx++) {
                scache_tcam_p->hw_idx[idx] = tcam_info->hw_idx_per_pipe[idx];
                scache_tcam_p->num_udfs[idx] = tcam_info->num_udfs_per_pipe[idx];
                scache_tcam_p->offset_bmap[idx] = tcam_info->offset_bmap_per_pipe[idx];
                scache_tcam_p->associated_pipes[idx] = tcam_info->associated_pipes[idx];
            }

            for (idx = 0; idx < MAX_UDF_ID_LIST; idx++) {
                scache_tcam_p->udf_id_list[idx] = tcam_info->udf_id_list[idx];
            }

            scache_tcam_p++;
            tcam_info = tcam_info->next;
        }

        *scache_ptr = (uint8*)scache_tcam_p;
    }

    return BCM_E_NONE;
}
#endif

STATIC int
bcmi_xgs4_udf_wb_sync_1_5(int unit, uint8 **scache_ptr)
{
    int idx;
    struct bcmi_xgs4_wb_tcam_info_1_5 *scache_tcam_p;

    scache_tcam_p = (struct bcmi_xgs4_wb_tcam_info_1_5*)(*scache_ptr);

    for (idx = 0; idx < SOC_MAX_NUM_PIPES; idx++) {
        sal_memcpy(&(scache_tcam_p->udf_field_bmp[idx]),
                   UDF_CTRL(unit)->udf_field_bmp[idx].w,
                   sizeof(uint32));
    }
    scache_tcam_p->max_chunks = UDF_CTRL(unit)->max_chunks;
    scache_tcam_p++;
    *scache_ptr = (uint8*)scache_tcam_p;

    return BCM_E_NONE;
}


/* Returns required scache size for UDF module */
STATIC int
bcmi_xgs4_udf_wb_scache_size_get(int unit, int *req_scache_size)
{
    BCM_IF_ERROR_RETURN(
        bcmi_xgs4_udf_wb_scache_size_get_1_0(unit, req_scache_size));

    BCM_IF_ERROR_RETURN(
        bcmi_xgs4_udf_wb_scache_size_get_1_1(unit, req_scache_size));

#if defined (BCM_TOMAHAWK_SUPPORT)
    BCM_IF_ERROR_RETURN(
        bcmi_xgs4_udf_wb_scache_size_get_1_3(unit, req_scache_size));

    BCM_IF_ERROR_RETURN(
        bcmi_xgs4_udf_wb_scache_size_get_1_4(unit, req_scache_size));
#endif

    if (soc_feature(unit, soc_feature_udf_selector_support)) {
        BCM_IF_ERROR_RETURN(
            bcmi_xgs4_udf_wb_scache_size_get_1_5(unit, req_scache_size));
    }

    return BCM_E_NONE;
}

/* Allocates required scache size for the UDF module recovery */
STATIC int
bcmi_xgs4_udf_wb_alloc(int unit)
{
    int     rv;
    int     req_scache_size;
    uint8   *scache_ptr;
    soc_scache_handle_t scache_handle;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_UDF, 0);

    rv = bcmi_xgs4_udf_wb_scache_size_get(unit, &req_scache_size);
    BCM_IF_ERROR_RETURN(rv);

   rv = _bcm_esw_scache_ptr_get(unit, scache_handle, TRUE,
            req_scache_size, &scache_ptr, BCM_WB_DEFAULT_VERSION, NULL);

    if (BCM_E_NOT_FOUND == rv) {
        /* Proceed with Level 1 Warm Boot */
        rv = BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs4_udf_wb_sync
 * Purpose:
 *      Syncs UDF warmboot state to scache
 * Parameters:
 *      unit            - (IN)  Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_xgs4_udf_wb_sync(int unit)
{
    uint8   *scache_ptr;
    soc_scache_handle_t scache_handle;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_UDF, 0);

    BCM_IF_ERROR_RETURN
        (_bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 0, &scache_ptr,
                                 BCM_WB_DEFAULT_VERSION, NULL));
    if (UDF_CTRL(unit) != NULL) {
        BCM_IF_ERROR_RETURN(bcmi_xgs4_udf_wb_sync_1_0(unit, &scache_ptr));
        BCM_IF_ERROR_RETURN(bcmi_xgs4_udf_wb_sync_1_1(unit, &scache_ptr));
#if defined (BCM_TOMAHAWK_SUPPORT)
        BCM_IF_ERROR_RETURN(bcmi_xgs4_udf_wb_sync_1_3(unit, &scache_ptr));
        BCM_IF_ERROR_RETURN(bcmi_xgs4_udf_wb_sync_1_4(unit, &scache_ptr));
#endif
    }
    if (soc_feature(unit, soc_feature_udf_selector_support)) {
        BCM_IF_ERROR_RETURN(bcmi_xgs4_udf_wb_sync_1_5(unit, &scache_ptr));
    }
    return BCM_E_NONE;
}



/*
 * Function:
 *      bcmi_xgs4_udf_reinit
 * Purpose:
 *      Recovers UDF warmboot state from scache
 * Parameters:
 *      unit            - (IN)  Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_xgs4_udf_reinit(int unit)
{
    int     rv = BCM_E_INTERNAL;
    uint8   *scache_ptr;
    uint16  recovered_ver = 0;
    soc_scache_handle_t scache_handle;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_UDF, 0);

    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 0, &scache_ptr,
                                 BCM_WB_DEFAULT_VERSION, &recovered_ver);

    if (BCM_E_NOT_FOUND == rv) {
        /* Proceed with Level 1 Warm Boot */
        rv = BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN(rv);

    if (recovered_ver >= BCM_WB_VERSION_1_0) {
        BCM_IF_ERROR_RETURN(bcmi_xgs4_udf_wb_reinit_1_0(unit, &scache_ptr));
    }

    if (recovered_ver >= BCM_WB_VERSION_1_1) {
        BCM_IF_ERROR_RETURN(bcmi_xgs4_udf_wb_reinit_1_1(unit, &scache_ptr));
    }

#if defined (BCM_TOMAHAWK_SUPPORT)
    if (recovered_ver >= BCM_WB_VERSION_1_2) {
        BCM_IF_ERROR_RETURN(bcmi_xgs4_udf_wb_tcam_info_reinit_1_2_to_1_4(unit));
    }

    if (recovered_ver >= BCM_WB_VERSION_1_3) {
        BCM_IF_ERROR_RETURN(bcmi_xgs4_udf_wb_reinit_1_3(unit, &scache_ptr));
    }

    if (recovered_ver >= BCM_WB_VERSION_1_4) {
        BCM_IF_ERROR_RETURN(bcmi_xgs4_udf_wb_reinit_1_4(unit, &scache_ptr));
    }
#endif

    if (soc_feature(unit, soc_feature_udf_selector_support)) {
        if (recovered_ver >= BCM_WB_VERSION_1_5) {
            BCM_IF_ERROR_RETURN(bcmi_xgs4_udf_wb_reinit_1_5(unit, &scache_ptr));
        }
    }

    if (BCM_SUCCESS(rv) &&
            (recovered_ver < BCM_WB_DEFAULT_VERSION)) {
        /* This will handle the below cases
          1. When warboot from release which didn't had UDF module in SCACHE or
          2. The current release SCACHE size required is more than preivous version  or
          3. Level 1 warmboot in this case memory will not be allocated. */
        BCM_IF_ERROR_RETURN(bcmi_xgs4_udf_wb_alloc(unit));
    }

    return rv;
}

#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      bcmi_xgs4_udf_offset_reserve
 * Purpose:
 *      Reserves requested offset chunks.
 * Parameters:
 *      unit            - (IN)  Unit number.
 *      max_offsets     - (IN)  Number of offsets to reserve.
 *      offset[]        - (IN)  Array of offset chunks requested.
 *      pipe_num        - (IN)  Pipe Num where offset is reserved
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *  offset[i]: 0x1 means only first half of the chunk is used.
 *  offset[i]: 0x2 means only second half of the chunk is used.
 *  offset[i]: 0x3 means both the chunks are used.
 */
STATIC int
bcmi_xgs4_udf_offset_reserve(int unit, int max_offsets, int offset[], int pipe_num)
{
    int j;
    int first = 1;
    int grp_id = 0;
    int gran = BCMI_XGS4_UDF_CTRL_OFFSET_GRAN(unit);
    bcmi_xgs4_udf_offset_entry_t *offset_entry;
    bcmi_xgs4_udf_offset_entry_t *offset_entry_array;
    SHR_BITDCL *chunk_bmap;
    SHR_BITDCL *byte_bmap;

#if defined (BCM_TOMAHAWK_SUPPORT)
    if (soc_feature(unit, soc_feature_udf_multi_pipe_support) &&
       (bcmi_xgs4_udf_oper_mode[unit] == bcmUdfOperModePipeLocal)) {

        offset_entry_array = UDF_CTRL(unit)->offset_entry_array_per_pipe[pipe_num];
        chunk_bmap = &(UDF_CTRL(unit)->hw_bmap_per_pipe[pipe_num]);
        byte_bmap = &(UDF_CTRL(unit)->byte_bmap_per_pipe[pipe_num]);
    } else
#endif
    {
        offset_entry_array = UDF_CTRL(unit)->offset_entry_array;
        chunk_bmap = &(UDF_CTRL(unit)->hw_bmap);
        byte_bmap = &(UDF_CTRL(unit)->byte_bmap);
    }


    for (j = 0; j < max_offsets; j++) {
        if (offset[j] == 0) {
            continue;
        }

        /* Return BCM_E_RESOURCE if the offset chunk is already in use */
        if (SHR_BITGET(chunk_bmap, j)) {
            return BCM_E_RESOURCE;
        }

        offset_entry = &(offset_entry_array[j]);

#if defined (BCM_TOMAHAWK_SUPPORT)
        if (soc_feature(unit, soc_feature_udf_multi_pipe_support) &&
            (bcmi_xgs4_udf_oper_mode[unit] == bcmUdfOperModePipeLocal)) {
            offset_entry->associated_pipe = pipe_num;
        }
#endif
        offset_entry->num_udfs += 1;

        if (offset_entry_array[j].num_udfs == 1) {
            /* Mark the entries as used */
            SHR_BITSET(chunk_bmap, j);
            *byte_bmap |= offset[j] << (j * gran);
        }

        if (gran == UDF_OFFSET_GRAN2) {
            /* Offset flag updated when gran is 2.
             * offset[i] can be 1,2,3 only in case of gran2
             */
            if ((offset[j] == 0x1) || (offset[j] == 0x2)) {
                offset_entry_array[j].flags |=
                    BCMI_XGS4_UDF_OFFSET_ENTRY_HALF;
            }
        }

        if (first == 1) {
            offset_entry_array[j].flags |=
                BCMI_XGS4_UDF_OFFSET_ENTRY_GROUP;

            offset_entry_array[j].grp_id = j;

            first = 0;
            grp_id = j;
        } else {
            offset_entry_array[j].flags |=
                BCMI_XGS4_UDF_OFFSET_ENTRY_MEMBER;
            offset_entry_array[j].grp_id = grp_id;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcmi_xgs4_th3_qset_udf_offsets_alloc
 * Purpose:
 *      Allocates offset chunks based on the INPUT qset.
 * Parameters:
 *      unit          - (IN)  Unit number.
 *      req_offsets   - (IN)  Number of chunks to be allocated.
 *      offset_array  - (OUT) Offset chunks in the order of availability.
 *      max_chunks    - (OUT) Number of chunks allocated
 *      pipe_num      - (IN)  Pipe Number
 *
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
 _bcmi_xgs4_th3_qset_udf_offsets_alloc(int unit, int req_offsets,
                                       int offset_array[], int *max_chunks,
                                       int pipe_num)
{
    int chunk = 0;
    uint32 hw_bmap[1];
    int num_chunks = 0;
    uint8 use_udf1 = 0, use_udf2 = 0, use_udf12 = 0;
    int pipe = 0;
    int udf1_used_chunks = 0, udf2_used_chunks = 0;

#if defined (BCM_TOMAHAWK_SUPPORT)
    if (soc_feature(unit, soc_feature_udf_multi_pipe_support) &&
       (bcmi_xgs4_udf_oper_mode[unit] == bcmUdfOperModePipeLocal)) {
        hw_bmap[0] = UDF_CTRL(unit)->hw_bmap_per_pipe[pipe_num];
        pipe = pipe_num;
    } else
#endif
    {
        hw_bmap[0] = UDF_CTRL(unit)->hw_bmap;
        pipe = 0;
    }

    /* Count the number of used chunks in UDF 1 and 2 */
    for (chunk = 0; chunk < (UDF_CTRL(unit)->max_chunks);
         chunk++) {
         if (SHR_BITGET(&(hw_bmap[0]), chunk)) {
             if (chunk < (UDF_CTRL(unit)->max_chunks / 2)) {
                 udf1_used_chunks++;
             } else {
                 udf2_used_chunks++;
             }
         }
    }

    /* verify if resources are available in order of udf1 alone,
       udf2 alone, followed by udf1 + udf2 together */

    if (((UDF_CTRL(unit)->max_chunks / 2 )
         - udf1_used_chunks) >= req_offsets ) {
        use_udf1 = 1;
    } else if (((UDF_CTRL(unit)->max_chunks / 2 )
        - udf2_used_chunks) >= req_offsets ) {
        use_udf2 = 1;
    } else if ((UDF_CTRL(unit)->max_chunks
        - (udf1_used_chunks
        + udf2_used_chunks)) >= req_offsets) {
        use_udf12 = 1;
    } else {
        /* No resources available in all cases */
        return (BCM_E_RESOURCE);
    }


    if (use_udf1) {     /* Use UDF1 */
        chunk = 0;
        while ((chunk < (UDF_CTRL(unit)->max_chunks / 2)) && (num_chunks < req_offsets)) {
             if (((_BCMI_UDF_BMP_TEST(UDF_CTRL(unit)->udf_field_bmp[pipe], chunk)) == 0)
                  && (SHR_BITGET(&(hw_bmap[0]), chunk) == 0)) {
                  offset_array[num_chunks] = chunk;
                  udf1_used_chunks++;
                  num_chunks++;
                  _BCMI_UDF_BMP_ADD(UDF_CTRL(unit)->udf_field_bmp[pipe], chunk);
             }
             chunk++;
        }
    } else if (use_udf2) {    /* Use UDF2 */
        chunk = (UDF_CTRL(unit)->max_chunks / 2);

        while ((chunk < UDF_CTRL(unit)->max_chunks) && (num_chunks < req_offsets)) {
             if (((_BCMI_UDF_BMP_TEST(UDF_CTRL(unit)->udf_field_bmp[pipe], chunk)) == 0)
                  && (SHR_BITGET(&(hw_bmap[0]), chunk) == 0)) {
                  offset_array[num_chunks] = chunk;
                  udf2_used_chunks++;
                  num_chunks++;
                  _BCMI_UDF_BMP_ADD(UDF_CTRL(unit)->udf_field_bmp[pipe], chunk);
             }
             chunk++;
        }
    } else if (use_udf12) {    /* Use UDF1 and  UDF2 */
        chunk = 0;
        while ((chunk < UDF_CTRL(unit)->max_chunks) && (num_chunks < req_offsets)) {
             if (((_BCMI_UDF_BMP_TEST(UDF_CTRL(unit)->udf_field_bmp[pipe], chunk)) == 0)
                  && (SHR_BITGET(&(hw_bmap[0]), chunk) == 0)) {
                  offset_array[num_chunks] = chunk;
                  num_chunks++;
                  if (chunk < (UDF_CTRL(unit)->max_chunks / 2)) {
                      udf1_used_chunks++;
                  } else {
                      udf2_used_chunks++;
                  }
                  _BCMI_UDF_BMP_ADD(UDF_CTRL(unit)->udf_field_bmp[pipe], chunk);
             }
             chunk++;
        }
    }

    if (num_chunks < req_offsets) {
        /* Mismatch in internal calculations */
        return (BCM_E_INTERNAL);
    }


    *max_chunks = num_chunks;

    return BCM_E_NONE;
}

int
_bcmi_xgs4_th3_qset_udf_offsets_free(int unit, int chunk, int pipe) {

    _BCMI_UDF_BMP_REMOVE(UDF_CTRL(unit)->udf_field_bmp[pipe], chunk);
    return BCM_E_NONE;

}


/*
 * Function:
 *      bcmi_xgs4_udf_offset_unreserve
 * Purpose:
 *      UnReserves offset chunks.
 * Parameters:
 *      unit            - (IN)  Unit number.
 *      max_offsets     - (IN)  Number of offsets to unreserve.
 *      offset[]        - (IN)  Array of offset chunks to be unreserved.
 *      pipe_num        - (IN)  Pipe Num where offset is unreserved
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_xgs4_udf_offset_unreserve(int unit, int max_offsets, int offset[], int pipe_num)
{
    int j;
    int gran = BCMI_XGS4_UDF_CTRL_OFFSET_GRAN(unit);
    bcmi_xgs4_udf_offset_entry_t *offset_entry;
    bcmi_xgs4_udf_offset_entry_t *offset_entry_array;
    SHR_BITDCL *chunk_bmap;
    SHR_BITDCL *byte_bmap;

#if defined (BCM_TOMAHAWK_SUPPORT)
    if (soc_feature(unit, soc_feature_udf_multi_pipe_support) &&
       (bcmi_xgs4_udf_oper_mode[unit] == bcmUdfOperModePipeLocal)) {

        offset_entry_array = UDF_CTRL(unit)->offset_entry_array_per_pipe[pipe_num];
        chunk_bmap = &(UDF_CTRL(unit)->hw_bmap_per_pipe[pipe_num]);
        byte_bmap = &(UDF_CTRL(unit)->byte_bmap_per_pipe[pipe_num]);
    } else
#endif
    {
        offset_entry_array = UDF_CTRL(unit)->offset_entry_array;
        chunk_bmap = &(UDF_CTRL(unit)->hw_bmap);
        byte_bmap = &(UDF_CTRL(unit)->byte_bmap);
    }

    for (j = 0; j < max_offsets; j++) {
        if (offset[j] == 0) {
            continue;
        }

        offset_entry = &(offset_entry_array[j]);

        /* Decr ref_cnt on the offset entry */
        offset_entry->num_udfs -= 1;

        if (offset_entry->num_udfs == 0) {
            /* Defensive Check: num_pkt_formats must be 0 */
            if (offset_entry->num_pkt_formats != 0) {
                return BCM_E_INTERNAL;
            }

            /* Mark the entries as free */
            SHR_BITCLR(chunk_bmap, j);
            *byte_bmap &= ~(offset[j] << (j * gran));

            if (soc_feature(unit, soc_feature_udf_selector_support)) {
                /* Free the chunk in field module */
                BCM_IF_ERROR_RETURN(_bcmi_xgs4_th3_qset_udf_offsets_free(unit, j, pipe_num));
            }

            /* Clear the UDF Offset entry */
            offset_entry->flags = 0x0;
            offset_entry->grp_id = 0x0;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs4_udf_offset_hw_alloc
 * Purpose:
 *      Allocates offsets for the UDF
 * Parameters:
 *      unit            - (IN)      Unit number.
 *      hints           - (IN)      Hints for allocation of offset chunks.
 *      offset_info     - (INOUT)   UDF Offset info struct
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_xgs4_udf_offset_hw_alloc(int unit, bcm_udf_alloc_hints_t *hints,
                              bcmi_xgs4_udf_offset_info_t *offset_info)
{
    int rv;
    int i, j;
    int gran;
    int grp_id = -1;
    int max_chunks;
    uint32 hw_bmap[1];
    uint32 alloc_bmap = 0;
    uint32 byte_bmap  = 0;
    int *offset_order;
    int offset_array[MAX_UDF_OFFSET_CHUNKS] = {0};
    int offset_order_d[MAX_UDF_OFFSET_CHUNKS] = {0};
    int alloc_cnt = 0, max_offsets = 0, req_offsets = 0;
    uint8 byte_offset = 0;
    int widthcovergbl = 0, widthcoverint = 0;

    /* offset allocation order */
    int bcmi_xgs4_udf_offsets_gran2_default[]  = {2, 3, 6, 7, 0, 1, 4, 5,
                                                  8, 9, 10, 11, 12, 13, 14, 15};
    int bcmi_xgs4_udf_offsets_gran4_default[]  = {1, 3, 0, 2, 4, 5, 6, 7};
    int bcmi_xgs4_udf_offsets_flexhash[] = {12, 13, 14, 15};
    int bcmi_xgs4_udf_offsets_udfhash[]  = {6, 7};
    int bcmi_xgs4_udf_offsets_policer[]  = {0, 1};
    int bcmi_xgs4_udf_offsets_range_checker[] = {5};    /*UDF1 chunk 2: bits (47 - 32 of UDF1)*/
    int bcmi_xgs4_udf_offsets_flex_counter[]  = {0, 1};

    gran = BCMI_XGS4_UDF_CTRL_OFFSET_GRAN(unit);
    max_chunks = BCMI_XGS4_UDF_CTRL_MAX_UDF_CHUNKS(unit);

    /*
     * The 'start' and 'width' need not always align to the offset granularity.
     * Every offset can extract 2 bytes or 4 bytes.
     * Also on devices with gran 4 if layer is L2 Header then offset
     * need to be adjusted with negative offset of 2.
     * This negative offset of 2 for L2 Header start is done because first
     * chunk will pick up B0,B1,B126,B127 bytes of frame.
     * If the user request a UDF that starts/ends on an odd-byte boundary,
     * SDK should allocate extra chunk(s) to facilitate for the requested
     * byte extraction and account for this extra byte in the field APIs
     * (by masking "don'tcare" for those extra bytes.
     */
    if (gran == UDF_OFFSET_GRAN2) {
        byte_offset = offset_info->start % gran;
    } else {
        if (offset_info->layer == bcmUdfLayerL2Header) {
            byte_offset = (offset_info->start + 2) % gran;
        } else {
            byte_offset = offset_info->start % gran;
        }
    }

    offset_info->byte_offset = byte_offset;
    req_offsets = (offset_info->width + byte_offset + gran -1) / gran;

    /*
     * FLEXHASH and IFP/VFP flags can be set at the same time
     * Since FLEXHASH has limited chunks, it is preferred during allocation
     */

    if ((hints != NULL) && (hints->flags & BCM_UDF_CREATE_O_FLEXHASH)) {
        max_offsets = COUNTOF(bcmi_xgs4_udf_offsets_flexhash);
        offset_order = &bcmi_xgs4_udf_offsets_flexhash[0];

    } else if ((hints != NULL) && (hints->flags & BCM_UDF_CREATE_O_UDFHASH)) {
        max_offsets = COUNTOF(bcmi_xgs4_udf_offsets_udfhash);
        offset_order = &bcmi_xgs4_udf_offsets_udfhash[0];

    } else if ((hints != NULL) &&
            (hints->flags & BCM_UDF_CREATE_O_RANGE_CHECK)) {
        max_offsets = COUNTOF(bcmi_xgs4_udf_offsets_range_checker);
        offset_order = &bcmi_xgs4_udf_offsets_range_checker[0];

    } else if ((hints != NULL) &&
               ((hints->flags & BCM_UDF_CREATE_O_FIELD_INGRESS) ||
                (hints->flags & BCM_UDF_CREATE_O_FIELD_LOOKUP))) {

        _field_stage_id_t stage;

        if (hints->flags & BCM_UDF_CREATE_O_FIELD_INGRESS) {
            stage  = _BCM_FIELD_STAGE_INGRESS;
        } else {
            stage  = _BCM_FIELD_STAGE_LOOKUP;
        }

        /* allocate based on qset */
        if (soc_feature(unit, soc_feature_udf_selector_support)) {
            rv = _bcmi_xgs4_th3_qset_udf_offsets_alloc(unit,
                         req_offsets, &offset_order_d[0], &max_offsets,
                         offset_info->associated_pipe);
            BCM_IF_ERROR_RETURN(rv);
        } else {
            rv = _bcm_esw_field_qset_udf_offsets_alloc(unit, stage, hints->qset,
                         req_offsets, &offset_order_d[0], &max_offsets,
                         offset_info->associated_pipe);
            BCM_IF_ERROR_RETURN(rv);
        }


        offset_order = &offset_order_d[0];
    } else if ((hints != NULL) &&
                (hints->flags & BCM_UDF_CREATE_O_POLICER_GROUP)) {
        max_offsets = COUNTOF(bcmi_xgs4_udf_offsets_policer);
        offset_order = &bcmi_xgs4_udf_offsets_policer[0];

    } else if ((hints != NULL) &&
               (hints->flags & BCM_UDF_CREATE_O_FLEX_COUNTER)) {
        max_offsets = COUNTOF(bcmi_xgs4_udf_offsets_flex_counter);
        offset_order = &bcmi_xgs4_udf_offsets_flex_counter[0];

    }  else {
        if (gran == UDF_OFFSET_GRAN2) {
            max_offsets = COUNTOF(bcmi_xgs4_udf_offsets_gran2_default);
            offset_order = &bcmi_xgs4_udf_offsets_gran2_default[0];
        } else {
            max_offsets = COUNTOF(bcmi_xgs4_udf_offsets_gran4_default);
            offset_order = &bcmi_xgs4_udf_offsets_gran4_default[0];
        }
    }

    /* max_chunks cannot be greater than 8 for chipsets with gran 4
     * and greater than 16 for chipsets with gran 2
     */
#if defined (BCM_TOMAHAWK_SUPPORT)
    if (soc_feature(unit, soc_feature_udf_multi_pipe_support) &&
       (bcmi_xgs4_udf_oper_mode[unit] == bcmUdfOperModePipeLocal)) {
        hw_bmap[0] = UDF_CTRL(unit)->hw_bmap_per_pipe[offset_info->associated_pipe];
    } else
#endif
    {
        hw_bmap[0] = UDF_CTRL(unit)->hw_bmap;
    }

    /* All chunks are reserved */
    if ((hw_bmap[0] & 0xffff) == 0xffff) {
        return BCM_E_RESOURCE;
    }

    for (j = 0; j < max_offsets; j++) {
        if ((SHR_BITGET(&(hw_bmap[0]), offset_order[j]))) {
            offset_array[offset_order[j]] = 0;
            continue;
        }

        /* Mark the first offset entry as the grp_id */
        if (grp_id == -1) {
            grp_id = offset_order[j];
        } else if (grp_id > offset_order[j]) {
            grp_id = offset_order[j];
        }

        /* offset_order will have order of chunks to be allocated.
         * offset_array should be filled with 0x1, 0x2 or 0x3 if
         * first half, or second half or full chunk is alloc'ed respy.
         */
        alloc_bmap |= (1 << offset_order[j]);
        if (alloc_cnt == 0) {
            /* First Chunk to be allocated */
            for(i = 0;i< (gran - byte_offset); i++) {
                if (widthcovergbl < offset_info->width) {
                    byte_bmap |= (0x1 << (((offset_order[j]+1) * gran) - byte_offset -i -1));
                    offset_array[offset_order[j]] |= (0x1 << (gran - byte_offset -i -1));
                    widthcovergbl++;
                }
            }
        } else {
            /* Rest all chunk to be allocated */
            widthcoverint = 0;
            for(i = 0;(i <(offset_info->width - widthcovergbl) && (i < gran)); i++){
                byte_bmap |= (0x1 << (((offset_order[j]+1) * gran) -i -1));
                offset_array[offset_order[j]] |= (0x1 << (gran -i -1));
                widthcoverint++;
            }
            widthcovergbl += widthcoverint;
        }

        if (++alloc_cnt == req_offsets) {
            break;
        }
    }

    if (alloc_cnt < req_offsets) {
        return BCM_E_RESOURCE;
    }

    rv = bcmi_xgs4_udf_offset_reserve(unit, max_chunks, offset_array,
                                      offset_info->associated_pipe);
    BCM_IF_ERROR_RETURN(rv);

    /* grp_id is the first chunk number */
    offset_info->grp_id = grp_id;

    offset_info->hw_bmap |= alloc_bmap;
    offset_info->byte_bmap |= byte_bmap;

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcmi_xgs4_udf_layer_to_offset_base
 * Purpose:
 *      Fetches base and offsets values to configure in UDF_OFFSET table.
 * Parameters:
 *      unit            - (IN)      Unit number.
 *      offset_info     - (IN)      UDF Offset info struct
 *      tcam_info       - (IN)      UDF tcam info struct
 *      base            - (OUT)     base_offset to be configured in BASE_OFFSETx
 *      baseoffset      - (OUT)     base offset to be configured in OFFSETx field
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *  Users can implement bcmUdfLayerUserPayload according to the requirement.
 */
STATIC int
bcmi_xgs4_udf_layer_to_offset_base(int unit,
                                   bcmi_xgs4_udf_offset_info_t *offset_info,
                                   bcmi_xgs4_udf_tcam_info_t *tcam_info,
                                   int *base, int *baseoffset)
{
    int gran;

    /* get udf gran from udf control */
    gran = BCMI_XGS4_UDF_CTRL_OFFSET_GRAN(unit);

    /* base offset */
    switch (offset_info->layer) {
        case bcmUdfLayerL2Header:
            *base = BCMI_XGS4_UDF_OFFSET_BASE_START_OF_L2;
            break;
        case bcmUdfLayerL3OuterHeader:
        case bcmUdfLayerTunnelHeader:
            *base = BCMI_XGS4_UDF_OFFSET_BASE_START_OF_OUTER_L3;
            break;
        case bcmUdfLayerL3InnerHeader:
        case bcmUdfLayerL4OuterHeader:
            *base = BCMI_XGS4_UDF_OFFSET_BASE_START_OF_INNER_L3;
            break;
        case bcmUdfLayerL4InnerHeader:
        case bcmUdfLayerTunnelPayload:
            *base = BCMI_XGS4_UDF_OFFSET_BASE_START_OF_L4;
            break;
        case bcmUdfLayerHigigHeader:
        case bcmUdfLayerHigig2Header:
            *base = BCMI_XGS4_UDF_OFFSET_BASE_START_OF_MH;
            break;
        default:
            /* Unknown layer type: Return Error */
            return BCM_E_PARAM;
    }

    /* Base offset value */
    if (gran == UDF_OFFSET_GRAN2) {
        *baseoffset = ((offset_info->start % UDF_PACKET_PARSE_LENGTH)
                        / gran);
    } else {
        if ((offset_info->layer == bcmUdfLayerL2Header) ||
                (offset_info->layer == bcmUdfLayerHigigHeader) ||
                (offset_info->layer == bcmUdfLayerHigig2Header)) {
            *baseoffset = (((offset_info->start + 2) % UDF_PACKET_PARSE_LENGTH)
                            / gran);
        } else {
            *baseoffset = ((offset_info->start % UDF_PACKET_PARSE_LENGTH)
                            / gran);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs4_udf_get
 * Purpose:
 *      To fetch the udf info given the UDF ID.
 * Parameters:
 *      unit            - (IN)      Unit number.
 *      udf_id          - (IN)      UDF Id.
 *      udf_info        - (OUT)     UDF info struct
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_xgs4_udf_get(int unit, bcm_udf_id_t udf_id, bcm_udf_t *udf_info)
{
    int rv;
    bcmi_xgs4_udf_offset_info_t *offset_info = NULL;

    /* UDF Module Init check */
    UDF_INIT_CHECK(unit);

    BCM_IF_NULL_RETURN_PARAM(udf_info);

    /* UDF Module Lock */
    UDF_LOCK(unit);

    rv = bcmi_xgs4_udf_offset_node_get(unit, udf_id, &offset_info);
    if (BCM_FAILURE(rv)) {
        UDF_UNLOCK(unit);
        return rv;
    }

    /* Copy tmp to offset_info */
    udf_info->start = BYTES2BITS(offset_info->start);
    udf_info->width = BYTES2BITS(offset_info->width);
    udf_info->layer = offset_info->layer;
#if defined (BCM_TOMAHAWK_SUPPORT)
    if (soc_feature(unit, soc_feature_udf_multi_pipe_support)) {
        SOC_PBMP_ASSIGN(udf_info->ports, offset_info->associated_ports);
    }
#endif
    /* Not used for XGS4 */
    udf_info->flags = 0;

    /* Release UDF mutex */
    UDF_UNLOCK(unit);

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcmi_xgs4_udf_get_all
 * Purpose:
 *      To fetch all the udfs in the system.
 * Parameters:
 *      unit            - (IN)      Unit number.
 *      max             - (IN)      Number of udf ids to fetch.
 *      udf_id_list     - (OUT)     List of UDF Ids fetched.
 *      actual          - (OUT)     Actual udfs fetched.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_xgs4_udf_get_all(int unit, int max, bcm_udf_id_t *udf_list, int *actual)
{
    bcmi_xgs4_udf_offset_info_t *tmp;

    /* UDF Module Init check */
    UDF_INIT_CHECK(unit);

    BCM_IF_NULL_RETURN_PARAM(actual);

    /* UDF Module Lock */
    UDF_LOCK(unit);

    *actual = 0;
    tmp = UDF_CTRL(unit)->offset_info_head;

    while (tmp != NULL) {
        if ((udf_list != NULL) && (*actual < max)) {
            udf_list[(*actual)] = tmp->udf_id;
        }

        (*actual) += 1;
        tmp = tmp->next;
    }

    /* Release UDF mutex */
    UDF_UNLOCK(unit);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs4_udf_get_all
 * Purpose:
 *      To destroy udf object.
 * Parameters:
 *      unit            - (IN)      Unit number.
 *      udf_id          - (IN)      UDF ID to be destroyed.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_xgs4_udf_destroy(int unit, bcm_udf_id_t udf_id)
{
    int rv;
    int i;
    int max, gran;
    uint32 offset[MAX_UDF_OFFSET_CHUNKS] = {0};
    bcmi_xgs4_udf_offset_info_t *cur = NULL;

    /* UDF Module Init check */
    UDF_INIT_CHECK(unit);

    /* UDF Module Lock */
    UDF_LOCK(unit);

    if (UDF_CTRL(unit)->udf_used_by_module == UDF_USED_BY_FIELD_MODULE) {
        LOG_CLI((BSL_META("Udf resource is used by FIELD module. \n")));
        UDF_UNLOCK(unit);
        return BCM_E_RESOURCE;
    }

    gran = BCMI_XGS4_UDF_CTRL_OFFSET_GRAN(unit);
    max = BCMI_XGS4_UDF_CTRL_MAX_UDF_CHUNKS(unit);;

    rv = bcmi_xgs4_udf_offset_node_get(unit, udf_id, &cur);
    if (BCM_FAILURE(rv)) {
        UDF_UNLOCK(unit);
        return rv;
    }

    /* Matching node not found */
    if (cur == NULL) {
        UDF_UNLOCK(unit);
        return BCM_E_NOT_FOUND;
    }

    if (cur->num_pkt_formats >= 1) {
        UDF_UNLOCK(unit);
        return BCM_E_BUSY;
    }

    for (i = 0; i < max; i++) {
        SHR_BITCOPY_RANGE(&offset[i], 0, &(cur->byte_bmap), i*gran, gran);
    }

    /* unreserve the offset chunks */
    rv = bcmi_xgs4_udf_offset_unreserve(unit, max, (int *)offset,
                                        cur->associated_pipe);
    if (BCM_FAILURE(rv)) {
        UDF_UNLOCK(unit);
        return rv;
    }

    /* Free the cur node */
    UDF_UNLINK_OFFSET_NODE(cur);
    sal_free(cur);

    UDF_CTRL(unit)->num_udfs -= 1;

    if (NULL == UDF_CTRL(unit)->offset_info_head) {
        UDF_CTRL(unit)->udf_used_by_module = UDF_USED_BY_NONE;
    }

    /* Release UDF mutex */
    UDF_UNLOCK(unit);

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcmi_xgs4_udf_offset_info_alloc
 * Purpose:
 *      To allocate memory for UDF offset info struct
 * Parameters:
 *      unit            - (IN)      Unit number.
 *      offset_info     - (OUT)     Pointer to hold the allocated memory.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_xgs4_udf_offset_info_alloc(int unit,
                                bcmi_xgs4_udf_offset_info_t **offset_info)
{
    int mem_sz;

    mem_sz = sizeof(bcmi_xgs4_udf_offset_info_t);

    /* Allocate memory for bcmi_xgs4_udf_offset_info_t structure. */
    UDF_MALLOC((*offset_info), bcmi_xgs4_udf_offset_info_t,
                mem_sz, "udf offset info");

    if (NULL == *offset_info) {
        return (BCM_E_MEMORY);
    }


    return BCM_E_NONE;
}


/*
 * Function:
 *      bcmi_xgs4_udf_offset_info_add
 * Purpose:
 *      To convert the UDF info to offset info and add to the linked list.
 * Parameters:
 *      unit            - (IN)      Unit number.
 *      udf_info        - (IN)      UDF info to be added to linked list.
 *      offset_info     - (INOUT)   Pointer to hold the allocated memory.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_xgs4_udf_offset_info_add(int unit, bcm_udf_t *udf_info,
                              bcmi_xgs4_udf_offset_info_t **offset_info)
{
    int rv;
#if defined (BCM_TOMAHAWK_SUPPORT)
    int port;
    soc_info_t  *si;
    si = &SOC_INFO(unit);
#endif

    /* Allocate memory for bcmi_xgs4_udf_offset_info_t structure. */
    rv = bcmi_xgs4_udf_offset_info_alloc(unit, offset_info);
    BCM_IF_ERROR_RETURN(rv);

    /* Save user inputs into the offset_info struct */
    (*offset_info)->layer = udf_info->layer;
    (*offset_info)->start = BITS2BYTES(udf_info->start);
    (*offset_info)->width = BITS2BYTES(udf_info->width);

#if defined (BCM_TOMAHAWK_SUPPORT)
    if (soc_feature(unit, soc_feature_udf_multi_pipe_support) &&
       (bcmi_xgs4_udf_oper_mode[unit] == bcmUdfOperModePipeLocal)) {

        /* save pbmp ports in order to return in get function */
        SOC_PBMP_ASSIGN((*offset_info)->associated_ports, udf_info->ports);

        /* get the pipe where the entry will be configured */
        SOC_PBMP_ITER(udf_info->ports, port) {
            (*offset_info)->associated_pipe = si->port_pipe[port];
            break;
        }
    }
#endif

    /* Add offset_info item to the linked list */
    rv = bcmi_xgs4_udf_offset_node_add(unit, *offset_info);
    if (BCM_FAILURE(rv)) {
        sal_free(*offset_info);
        *offset_info = NULL;
    }

    return rv;
}

/* Linked list node add: for offset nodes */
STATIC int
bcmi_xgs4_udf_offset_node_add(int unit, bcmi_xgs4_udf_offset_info_t *new)
{
    bcmi_xgs4_udf_offset_info_t *tmp;

    if (new == NULL) {
        return BCM_E_INTERNAL;
    }

    tmp = (UDF_CTRL(unit)->offset_info_head);

    if (NULL == tmp) {
        /* Adding first node */
        (UDF_CTRL(unit)->offset_info_head) = new;
        new->prev = NULL;

    } else {

        /* traverse to the end of the list */
        while (tmp->next != NULL) {
            tmp = tmp->next;
        }

        /* insert new node to the end of the list */
        new->prev = tmp;
        tmp->next = new;

    }

    new->next = NULL;

    return  BCM_E_NONE;
}

/* Linked list node delete: for offset nodes */
STATIC int
bcmi_xgs4_udf_offset_node_delete(int unit, bcm_udf_id_t udf_id,
                                 bcmi_xgs4_udf_offset_info_t **del)
{
    int rv;
    bcmi_xgs4_udf_offset_info_t *cur = NULL;

    rv = bcmi_xgs4_udf_offset_node_get(unit, udf_id, &cur);
    BCM_IF_ERROR_RETURN(rv);

    /* Matching node not found */
    if (cur == NULL) {
        return BCM_E_NOT_FOUND;
    }

    /* unlink cur from the list */
    if (cur->prev) {
        (cur->prev)->next = cur->next;
    } else {
        /* cur node is the head */
        UDF_CTRL(unit)->offset_info_head = cur->next;
    }

    if (cur->next) {
        (cur->next)->prev = cur->prev;
    }

    /* Callee function to free the node */
    *del = cur;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs4_udf_tcam_info_alloc
 * Purpose:
 *      To allocate memory for UDF tcam info struct
 * Parameters:
 *      unit            - (IN)      Unit number.
 *      tcam_info       - (OUT)     Pointer to hold the allocated memory.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_xgs4_udf_tcam_info_alloc(int unit, bcmi_xgs4_udf_tcam_info_t **tcam_info)
{
    int mem_sz;

    mem_sz = sizeof(bcmi_xgs4_udf_tcam_info_t);

    /* Allocate memory for bcmi_xgs4_udf_offset_info_t structure. */
    UDF_MALLOC((*tcam_info), bcmi_xgs4_udf_tcam_info_t,
                mem_sz, "udf tcam info");

    if (NULL == *tcam_info) {
        return (BCM_E_MEMORY);
    }

    /* Mark entry invalid */
    (*tcam_info)->hw_idx = MAX_UDF_TCAM_ENTRIES;

#if defined (BCM_TOMAHAWK_SUPPORT)
    if (soc_feature(unit, soc_feature_udf_multi_pipe_support)) {
        int i;
        /* Mark pipe num invalid */
        (*tcam_info)->associated_pipe = BCMI_XGS4_UDF_INVALID_PIPE_NUM;

        for (i = 0; i < SOC_MAX_NUM_PIPES; i++) {
            (*tcam_info)->hw_idx_per_pipe[i] = MAX_UDF_TCAM_ENTRIES;
            (*tcam_info)->associated_pipes[i] = BCMI_XGS4_UDF_INVALID_PIPE_NUM;
        }
    }
#endif
    return BCM_E_NONE;
}


/*
 * Function:
 *      bcmi_xgs4_udf_tcam_info_add
 * Purpose:
 *      To convert the packet format info to tcam info and add to the list.
 * Parameters:
 *      unit            - (IN)      Unit number.
 *      pkt_format      - (IN)      UDF Packet format to be added to list.
 *      tcam_info       - (INOUT)   Pointer to hold the allocated memory.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_xgs4_udf_tcam_info_add(int unit,
                            bcm_udf_pkt_format_info_t *pkt_format,
                            bcmi_xgs4_udf_tcam_info_t **tcam_info)
{
    int rv;
    fp_udf_tcam_entry_t *tcam_buf;

    /* Allocate memory for bcmi_xgs4_udf_tcam_info_t structure. */
    rv = bcmi_xgs4_udf_tcam_info_alloc(unit, tcam_info);
    BCM_IF_ERROR_RETURN(rv);

    tcam_buf = (fp_udf_tcam_entry_t *)&((*tcam_info)->hw_buf);
    sal_memset(tcam_buf, 0, sizeof(fp_udf_tcam_entry_t));

    /* Initialize packet format entry */
    rv = bcmi_xgs4_udf_pkt_format_tcam_key_init(unit, pkt_format,
                                           (uint32 *)tcam_buf);
    if (BCM_FAILURE(rv)) {
        sal_free(*tcam_info);
        (*tcam_info) = NULL;
        return rv;
    }

    (*tcam_info)->priority = pkt_format->prio;
#if defined (BCM_TOMAHAWK_SUPPORT)
    if (UDF_CTRL(unit)->flags & BCMI_XGS4_UDF_PKT_FORMAT_CLASS_ID) {
        (*tcam_info)->class_id = pkt_format->class_id;
    }
#endif
    /* Add offset_info item to the linked list */
    rv = bcmi_xgs4_udf_tcam_node_add(unit, *tcam_info);
    if (BCM_FAILURE(rv)) {
        sal_free(*tcam_info);
        (*tcam_info) = NULL;
    }

    return rv;
}

/* Linked list node add: for tcam nodes */
STATIC int
bcmi_xgs4_udf_tcam_node_add(int unit, bcmi_xgs4_udf_tcam_info_t *new)
{
    bcmi_xgs4_udf_tcam_info_t *tmp;

    if (new == NULL) {
        return BCM_E_INTERNAL;
    }

    tmp = (UDF_CTRL(unit)->tcam_info_head);

    if (NULL == tmp) {
        /* Adding first node */
        (UDF_CTRL(unit)->tcam_info_head) = new;
        new->prev = NULL;

    } else {

        /* traverse to the end of the list */
        while (tmp->next != NULL) {
            tmp = tmp->next;
        }

        /* insert new node to the end of the list */
        new->prev = tmp;
        tmp->next = new;

    }
    new->next = NULL;

    return  BCM_E_NONE;
}

/* Linked list node delete: for tcam nodes */
STATIC int
bcmi_xgs4_udf_tcam_node_delete(int unit, bcm_udf_pkt_format_id_t pkt_format_id,
                               bcmi_xgs4_udf_tcam_info_t **del)
{
    int rv;
    bcmi_xgs4_udf_tcam_info_t *cur = NULL;

    rv = bcmi_xgs4_udf_tcam_node_get(unit, pkt_format_id, &cur);
    BCM_IF_ERROR_RETURN(rv);

    *del = NULL;

    /* Matching node not found */
    if (cur == NULL) {
        return BCM_E_NOT_FOUND;
    }

    /* unlink cur from the list */
    if (cur->prev) {
        (cur->prev)->next = cur->next;
    } else {
        /* cur node is the head node */
        UDF_CTRL(unit)->tcam_info_head = cur->next;
    }
    if (cur->next) {
        (cur->next)->prev = cur->prev;
    }

    /* Callee function to free the node */
    *del = cur;

    return BCM_E_NONE;
}


/* To allocate running id for UDF and Packet format IDs. */
STATIC int
bcmi_xgs4_udf_id_running_id_alloc(int unit, bcmiUdfObjectType type, int *id)
{
    int rv;
    int ix;
    int min_ix, max_ix;
    bcmi_xgs4_udf_offset_info_t *offset_info = NULL;
    bcmi_xgs4_udf_tcam_info_t *tcam_info = NULL;

    if (type == bcmiUdfObjectUdf) {
        min_ix = BCMI_XGS4_UDF_ID_MIN;
        max_ix = BCMI_XGS4_UDF_ID_MAX;

        ix = BCMI_XGS4_UDF_ID_RUNNING(unit);
    } else {
        min_ix = BCMI_XGS4_UDF_PKT_FORMAT_ID_MIN;
        max_ix = BCMI_XGS4_UDF_PKT_FORMAT_ID_MAX;

        ix = BCMI_XGS4_UDF_PKT_FORMAT_ID_RUNNING(unit);
    }

    if (max_ix < ix) {
        for (ix = min_ix; ix <= max_ix; ix++) {
            if (type == bcmiUdfObjectUdf) {
                rv = bcmi_xgs4_udf_offset_node_get(unit, ix, &offset_info);
                if (BCM_E_NOT_FOUND == rv) {
                    break;
                }
            } else {
                rv = bcmi_xgs4_udf_tcam_node_get(unit, ix, &tcam_info);
                if (BCM_E_NOT_FOUND == rv) {
                    break;
                }
            }
            if (BCM_FAILURE(rv)) {
                break;
            }
        }

        /* All 64k udf ids are used up */
        if (max_ix < ix) {
            return BCM_E_FULL;
        }
    }

    *id = ix;

    return BCM_E_NONE;
}

/* Conveerts chunk ID to OFFSET_xf and BASE_OFFSET_xf fields */
STATIC int
bcmi_xgs4_udf_offset_to_hw_field(int unit, int udf_num, int user_num,
            soc_field_t *base_hw_f, soc_field_t *offset_hw_f)
{
    soc_field_t offset_f[2][8] = {
        {
            UDF1_OFFSET0f,            UDF1_OFFSET1f,
            UDF1_OFFSET2f,            UDF1_OFFSET3f,
            UDF1_OFFSET4f,            UDF1_OFFSET5f,
            UDF1_OFFSET6f,            UDF1_OFFSET7f
        },
        {
            UDF2_OFFSET0f,            UDF2_OFFSET1f,
            UDF2_OFFSET2f,            UDF2_OFFSET3f,
            UDF2_OFFSET4f,            UDF2_OFFSET5f,
            UDF2_OFFSET6f,            UDF2_OFFSET7f
        }
    };

    soc_field_t base_offset_f[2][8] = {
        {
            UDF1_BASE_OFFSET_0f,      UDF1_BASE_OFFSET_1f,
            UDF1_BASE_OFFSET_2f,      UDF1_BASE_OFFSET_3f,
            UDF1_BASE_OFFSET_4f,      UDF1_BASE_OFFSET_5f,
            UDF1_BASE_OFFSET_6f,      UDF1_BASE_OFFSET_7f
        },
        {
            UDF2_BASE_OFFSET_0f,      UDF2_BASE_OFFSET_1f,
            UDF2_BASE_OFFSET_2f,      UDF2_BASE_OFFSET_3f,
            UDF2_BASE_OFFSET_4f,      UDF2_BASE_OFFSET_5f,
            UDF2_BASE_OFFSET_6f,      UDF2_BASE_OFFSET_7f
        }
    };

    *base_hw_f = base_offset_f[udf_num][user_num];
    *offset_hw_f = offset_f[udf_num][user_num];

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs4_udf_offset_uninstall
 * Purpose:
 *      To uninstall the UDF offsets in the hardware
 * Parameters:
 *      unit            - (IN)      Unit number.
 *      e               - (IN)      Entry in the UDF_OFFSET table.
 *      hw_bmap         - (IN)      Offsets to uninstall.
 *      pipe_num        - (IN)      pipe_num (used only in pipeLocal mode)
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_xgs4_udf_offset_uninstall(int unit, int e, uint32 hw_bmap, int pipe_num)
{
    int udf_idx, user_idx;
    int rv;
    int base = 0x0;
    int offset = 0x0;
    soc_mem_t mem;
    soc_field_t base_f, offset_f;
    fp_udf_offset_entry_t offset_buf;

    if ((e < 0) && (e > BCMI_XGS4_UDF_CTRL_MAX_UDF_ENTRIES(unit))) {
        return BCM_E_PARAM;
    }

#if defined (BCM_TOMAHAWK_SUPPORT)
    if (soc_feature(unit, soc_feature_udf_multi_pipe_support) &&
       (bcmi_xgs4_udf_oper_mode[unit] == bcmUdfOperModePipeLocal)) {
         mem = SOC_MEM_UNIQUE_ACC(unit, UDF_CTRL(unit)->offset_mem)[pipe_num];
    } else
#endif
    {
        mem = UDF_CTRL(unit)->offset_mem;
    }

    rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL, e, &offset_buf);
    BCM_IF_ERROR_RETURN(rv);

    for (udf_idx = 0; udf_idx < 2; udf_idx++) {
        for (user_idx = 0; user_idx < (BCMI_XGS4_UDF_CTRL_MAX_UDF_CHUNKS(unit)/2); user_idx++) {
            if (!(hw_bmap & (1 << (udf_idx *
                                (BCMI_XGS4_UDF_CTRL_MAX_UDF_CHUNKS(unit)/2) + user_idx)))) {
                continue;
            }


            rv = bcmi_xgs4_udf_offset_to_hw_field(unit, udf_idx, user_idx, &base_f, &offset_f);
            BCM_IF_ERROR_RETURN(rv);

            soc_mem_field32_set(unit, mem, &offset_buf, base_f, base);
            soc_mem_field32_set(unit, mem, &offset_buf, offset_f, offset);
        }
    }
#if defined (BCM_TOMAHAWK_SUPPORT)
    if (UDF_CTRL(unit)->flags & BCMI_XGS4_UDF_PKT_FORMAT_CLASS_ID) {
        soc_mem_field32_set(unit, mem, &offset_buf, UDF_CLASS_IDf, 0);
    }
#endif

    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, e, &offset_buf);
    BCM_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs4_udf_offset_install
 * Purpose:
 *      To install the UDF offsets in the hardware
 * Parameters:
 *      unit            - (IN)      Unit number.
 *      e               - (IN)      Entry in the UDF_OFFSET table.
 *      hw_bmap         - (IN)      Offsets to uninstall.
 *      base            - (IN)      value to write in BASE_OFFSETx.
 *      offset          - (IN)      Value to write in the OFFSETx.
 *      pipe_num        - (IN)      Pipe number (used only in pipeLocal)
 *      udf_class_id    - (IN)      Class ID used by the IFP Key Selection Mechanism
 *      flags           - (IN)      Flags for offset entry
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
bcmi_xgs4_udf_offset_install(int unit, int e, uint32 hw_bmap,
                             int base, int offset, int pipe_num,
                             int class_id, uint8 flags)
{
    int udf_idx, user_idx;
    int rv, max_offset;
    soc_mem_t mem;
    soc_field_t base_f, offset_f;
    fp_udf_offset_entry_t offset_buf;

    if ((e < 0) && (e > BCMI_XGS4_UDF_CTRL_MAX_UDF_ENTRIES(unit))) {
        return BCM_E_PARAM;
    }
    if (!BCMI_XGS4_UDF_CTRL_OFFSET_GRAN(unit)) {
        return BCM_E_PARAM;
    }
    max_offset = UDF_PACKET_PARSE_LENGTH/BCMI_XGS4_UDF_CTRL_OFFSET_GRAN(unit);

#if defined (BCM_TOMAHAWK_SUPPORT)
    if (soc_feature(unit, soc_feature_udf_multi_pipe_support) &&
       (bcmi_xgs4_udf_oper_mode[unit] == bcmUdfOperModePipeLocal)) {
        mem = SOC_MEM_UNIQUE_ACC(unit, UDF_CTRL(unit)->offset_mem)[pipe_num];
    } else
#endif
    {
        mem = UDF_CTRL(unit)->offset_mem;
    }
    offset = (offset < 0) ? 0 : (uint32)offset;

    rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL, e, &offset_buf);
    BCM_IF_ERROR_RETURN(rv);

    for (udf_idx = 0; udf_idx < 2; udf_idx++) {
        for (user_idx = 0; user_idx < (BCMI_XGS4_UDF_CTRL_MAX_UDF_CHUNKS(unit)/2); user_idx++) {
            if (!(hw_bmap & (1 << (udf_idx *
                    (BCMI_XGS4_UDF_CTRL_MAX_UDF_CHUNKS(unit)/2) + user_idx)))) {
                continue;
            }

            rv = bcmi_xgs4_udf_offset_to_hw_field(unit, udf_idx, user_idx, &base_f, &offset_f);
            BCM_IF_ERROR_RETURN(rv);

            /*handling wrap around*/
            if((offset >= max_offset) || (offset < 0)){
                offset = 0;
            }

            soc_mem_field32_set(unit, mem, &offset_buf, base_f, base);
            soc_mem_field32_set(unit, mem, &offset_buf, offset_f, offset);

            /* Every other offset chunk should have offset incr by 1 */
            offset += 1;
        }
    }

#if defined (BCM_TOMAHAWK_SUPPORT)
    if (UDF_CTRL(unit)->flags & BCMI_XGS4_UDF_PKT_FORMAT_CLASS_ID) {
        soc_mem_field32_set(unit, mem, &offset_buf, UDF_CLASS_IDf, class_id);
    }
    if (UDF_CTRL(unit)->flags & BCMI_XGS4_UDF_CONDITIONAL_CHECK) {
        if (flags & BCMI_XGS4_UDF_OFFSET_FLEXHASH) {
            soc_mem_field32_set(unit, mem, &offset_buf,
                                ENABLE_UDF_CONDITIONAL_CHECKf, 1);
        }
    }
#endif

    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, e, &offset_buf);
    BCM_IF_ERROR_RETURN(rv);


    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_xgs4_udf_tcam_entry_move
 * Purpose:
 *     Move a single entry in  udf tcam and offset tables.
 * Parameters:
 *     unit      - (IN) BCM device number.
 *     tcam_entry_arr - (IN) Data control structure.
 *     src       - (IN) Move from index.
 *     dst       - (IN) Move to index.
 *     pipe      - (IN) Pipe num
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_xgs4_udf_tcam_entry_move(int unit,
                         bcmi_xgs4_udf_tcam_entry_t *tcam_entry_arr,
                         int src, int dst, int pipe_num)
{
    int rv;                                 /* Operation return status.   */
    soc_mem_t offset_mem, tcam_mem;         /* Memory Ids                 */
    uint32 hw_buf[SOC_MAX_MEM_FIELD_WORDS]; /* Buffer to read hw entry.   */

    /* Input parameters check. */
    BCM_IF_NULL_RETURN_PARAM(tcam_entry_arr);

#if defined (BCM_TOMAHAWK_SUPPORT)
    if (soc_feature(unit, soc_feature_udf_multi_pipe_support) &&
       (bcmi_xgs4_udf_oper_mode[unit] == bcmUdfOperModePipeLocal)) {
        if (pipe_num == BCMI_XGS4_UDF_INVALID_PIPE_NUM) {
            return BCM_E_PARAM;
        }
        tcam_mem = SOC_MEM_UNIQUE_ACC(unit, UDF_CTRL(unit)->tcam_mem)[pipe_num];
        offset_mem = SOC_MEM_UNIQUE_ACC(unit, UDF_CTRL(unit)->offset_mem)[pipe_num];
    } else
#endif
    {
        tcam_mem = UDF_CTRL(unit)->tcam_mem;
        offset_mem = UDF_CTRL(unit)->offset_mem;
    }
    if (tcam_mem == INVALIDm || offset_mem == INVALIDm) {
        return BCM_E_INTERNAL;
    }

    rv = soc_mem_read(unit, offset_mem, MEM_BLOCK_ANY, src, hw_buf);
    BCM_IF_ERROR_RETURN(rv);
    rv = soc_mem_write(unit, offset_mem, MEM_BLOCK_ALL, dst, hw_buf);
    BCM_IF_ERROR_RETURN(rv);

    rv = soc_mem_read(unit, tcam_mem, MEM_BLOCK_ANY, src, hw_buf);
    BCM_IF_ERROR_RETURN(rv);
    rv = soc_mem_write(unit, tcam_mem, MEM_BLOCK_ALL, dst, hw_buf);
    BCM_IF_ERROR_RETURN(rv);

    /* Clear entries at old index. */
    rv = soc_mem_write(unit, offset_mem, MEM_BLOCK_ALL, src,
                       soc_mem_entry_null(unit, offset_mem));
    BCM_IF_ERROR_RETURN(rv);

    rv = soc_mem_write(unit, tcam_mem, MEM_BLOCK_ALL, src,
                       soc_mem_entry_null(unit, tcam_mem));
    BCM_IF_ERROR_RETURN(rv);

    /* Update hw_idx in the packet formats */
    tcam_entry_arr[src].tcam_info->hw_idx = dst;

    /* Update sw structure tracking entry use. */
    sal_memcpy(tcam_entry_arr + dst,
               tcam_entry_arr + src,
               sizeof(bcmi_xgs4_udf_tcam_entry_t));

    sal_memset(tcam_entry_arr + src, 0,
               sizeof(bcmi_xgs4_udf_tcam_entry_t));


    return (BCM_E_NONE);
}

/*
 * Function:
 *     bcmi_xgs4_udf_tcam_move_up
 * Purpose:
 *     Moved udf tcam entries up.
 * Parameters:
 *     unit      - (IN) BCM device number.
 *     tcam_entry_arr - (IN) Data control structure.
 *     dest      - (IN) Insertion target index.
 *     free_slot - (IN) Free slot.
 *     pipe_num  - (IN) Pipe num
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_xgs4_udf_tcam_move_up(int unit,
                      bcmi_xgs4_udf_tcam_entry_t *tcam_entry_arr,
                      int dest, int free_slot, int pipe_num)
{
    int idx;          /* Entries iterator.        */
    int rv;           /* Operation return status. */

    for (idx = free_slot; idx > dest; idx --) {
        rv = bcmi_xgs4_udf_tcam_entry_move(unit, tcam_entry_arr,
                                           idx - 1, idx, pipe_num);
        BCM_IF_ERROR_RETURN(rv);
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     bcmi_xgs4_udf_tcam_move_down
 * Purpose:
 *     Moved udf tcam entries down
 * Parameters:
 *     unit      - (IN) BCM device number.
 *     tcam_entry_arr - (IN) Data control structure.
 *     dest      - (IN) Insertion target index.
 *     pipe_num  - (IN) Pipe num
 *     free_slot - (IN) Free slot.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_xgs4_udf_tcam_move_down(int unit,
                        bcmi_xgs4_udf_tcam_entry_t *tcam_entry_arr,
                        int dest, int free_slot, int pipe_num)
{
    int idx;          /* Entries iterator.        */
    int rv;           /* Operation return status. */

    for (idx = free_slot; idx < dest; idx ++) {
        rv = bcmi_xgs4_udf_tcam_entry_move(unit, tcam_entry_arr,
                                           idx + 1, idx, pipe_num);
        BCM_IF_ERROR_RETURN(rv);
    }
    return (BCM_E_NONE);
}

STATIC int
bcmi_xgs4_udf_tcam_entries_compare(int unit,
                                   uint32 *hw_entry, uint32 *sw_entry)
{
    return sal_memcmp(sw_entry, hw_entry, sizeof(fp_udf_tcam_entry_t));
}


/*
 * Function:
 *     bcmi_xgs4_udf_tcam_entry_match
 * Purpose:
 *     Match tcam entry against currently instllaed ones
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     new      - (IN  New Tcam info structure.
 *     match    - (IN  Matched Tcam info structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_xgs4_udf_tcam_entry_match(int unit, bcmi_xgs4_udf_tcam_info_t *new,
                               bcmi_xgs4_udf_tcam_info_t **match)
{
    uint32 *old_buf;   /* Tcam entry pointer.        */
    uint32 *new_buf;   /* Tcam entry pointer.        */
    bcmi_xgs4_udf_tcam_info_t *tmp;

    /* Input parameters check. */
    BCM_IF_NULL_RETURN_PARAM(new);

    new_buf = (uint32 *)&(new->hw_buf);

    tmp = UDF_CTRL(unit)->tcam_info_head;

    *match = NULL;

    while (tmp != NULL) {
        if (tmp == new) {
            tmp = tmp->next;
            continue;
        }

        old_buf = (uint32 *)&(tmp->hw_buf);

        if (!bcmi_xgs4_udf_tcam_entries_compare(unit, old_buf, new_buf)) {
            *match = tmp;
            break;
        }

        tmp = tmp->next;
    }

    if (*match == NULL) {
        return BCM_E_NOT_FOUND;
    }


    return BCM_E_NONE;

}

/*
 * Function:
 *      bcmi_xgs4_udf_tcam_entry_insert
 * Purpose:
 *     To insert an entry in UDF_TCAM at an available index.
 * Parameters:
 *     unit        - (IN) BCM device number.
 *     tcam_new    - (IN) Tcam info to be inserted.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_xgs4_udf_tcam_entry_insert(int unit, bcmi_xgs4_udf_tcam_info_t *tcam_new)
{
    int idx;                       /* FP UDF tcam table interator.    */
    int rv;                        /* Operation return status.        */
    int idx_max;                   /* FP UDF tcam table index max.    */
    int idx_min;                   /* FP UDF tcam table index min.    */
    int range_min;                 /* Index min for entry insertion.  */
    int range_max;                 /* Index max for entry insertion.  */
    int tcam_idx;
    int unused_entry_min;          /* Unused entry before the range.  */
    int unused_entry_max;          /* Unused entry after the range.   */
    bcmi_xgs4_udf_tcam_info_t *tcam_info;
    bcmi_xgs4_udf_tcam_entry_t *tcam_entry_arr; /* Tcam entries array.  */
    soc_mem_t                   tcam_mem; /* tcam memory access */
    int pipe_num = BCMI_XGS4_UDF_INVALID_PIPE_NUM; /* used only in pipeLocal mode */

    /* Initialization. */
    idx_min = soc_mem_index_min(unit, UDF_CTRL(unit)->tcam_mem);
    idx_max = soc_mem_index_max(unit, UDF_CTRL(unit)->tcam_mem);

    if ((idx_min < 0) || (idx_max < 0)) {
        return BCM_E_INTERNAL;
    }

    range_min = idx_min;
    range_max = idx_max;
    unused_entry_min = unused_entry_max = -1;

#if defined (BCM_TOMAHAWK_SUPPORT)
    if (soc_feature(unit, soc_feature_udf_multi_pipe_support) &&
       (bcmi_xgs4_udf_oper_mode[unit] == bcmUdfOperModePipeLocal)) {
        pipe_num = tcam_new->associated_pipe;
        tcam_entry_arr = UDF_CTRL(unit)->tcam_entry_array_per_pipe[pipe_num];
        tcam_mem = SOC_MEM_UNIQUE_ACC(unit, UDF_CTRL(unit)->tcam_mem)[pipe_num];
    } else
#endif
    {
        tcam_entry_arr = UDF_CTRL(unit)->tcam_entry_array;
        tcam_mem = UDF_CTRL(unit)->tcam_mem;
    }

    /* No identical entry found try to allocate an unused entry and
     * reshuffle tcam if necessary to organize entries by priority.
     */
    for (idx = idx_min; idx <= idx_max; idx++) {
        if (0 == tcam_entry_arr[idx].valid) {
            if (idx <= range_max) {
                /* Any free index below range max can be used for insertion. */
                unused_entry_min = idx;
            } else {
                /* There is no point to continue after first
                 * free index above range max.
                 */
                unused_entry_max = idx;
                break;
            }
        } else {
            tcam_info = tcam_entry_arr[idx].tcam_info;
            /* Identify insertion range. */
            /*
            * Principle:
            *    1. Different entries must have different priorities, unless 0
            *    2. Place entry of higher priority to lower index.
            *    3. If priorities are equal (0), place entry of higher
            *       pkt_format_id to lower index.
            *  Process:
            *   If existed tcam priority is explicitly configured (non-zero),
            *      find the minimum entry whose priority is more than new priority,
            *      and the maximum entry priority is less than new priority.
            *   If existed tcam priority is not specified (zero),
            *      if new priority is non-zero, find available place;
            *      if new priority is zero, find available place using pkt_format_id.
            *   If new priority is non-zero and equals to exsited priority,
            *      or new pkt_format_id equals to exsited pkt_format_id,
            *      don't insert, return parameter error.
            */
            if (0 < tcam_info->priority) {
                if (tcam_info->priority > tcam_new->priority) {
                    range_min = idx;
                } else if (tcam_info->priority < tcam_new->priority) {
                    if (idx < range_max) {
                        range_max = idx;
                    }
                } else {
                    return BCM_E_PARAM;
                }
            } else if (0 == tcam_info->priority) {
                if (0 == tcam_new->priority) {
                    if (tcam_info->pkt_format_id > tcam_new->pkt_format_id) {
                        range_min = idx;
                    } else if (tcam_info->pkt_format_id <
                                tcam_new->pkt_format_id) {
                        if (idx < range_max) {
                            range_max = idx;
                        }
                    } else {
                        return BCM_E_PARAM;
                    }
                } else if (0 < tcam_new->priority) {
                    if (idx < range_max) {
                        range_max = idx;
                    }
                } else {
                    return BCM_E_PARAM;
                }
            } else {
                return BCM_E_PARAM;
            }
        }
    }

    /* Check if tcam is completely full. */
    if ((unused_entry_min == -1) && (unused_entry_max == -1)) {
        return (BCM_E_FULL);
    }

    /*  Tcam entries shuffling. */
    if (unused_entry_min > range_min) {
        tcam_idx = unused_entry_min;
    } else if (unused_entry_min == -1) {
        rv = bcmi_xgs4_udf_tcam_move_up(unit, tcam_entry_arr,
                                         range_max, unused_entry_max,
                                         pipe_num);

        BCM_IF_ERROR_RETURN(rv);
        tcam_idx = range_max;
    } else if (unused_entry_max == -1) {
        rv = bcmi_xgs4_udf_tcam_move_down(unit, tcam_entry_arr,
                                           range_min, unused_entry_min,
                                           pipe_num);
        BCM_IF_ERROR_RETURN(rv);
        tcam_idx = range_min;
    } else if ((range_min - unused_entry_min) >
               (unused_entry_max - range_max)) {
        rv = bcmi_xgs4_udf_tcam_move_up(unit, tcam_entry_arr,
                                         range_max, unused_entry_max,
                                         pipe_num);
        BCM_IF_ERROR_RETURN(rv);
        tcam_idx = range_max;
    } else {
        rv = bcmi_xgs4_udf_tcam_move_down(unit, tcam_entry_arr,
                                           range_min, unused_entry_min,
                                           pipe_num);
        BCM_IF_ERROR_RETURN(rv);
        tcam_idx = range_min;
    }

    /* Install the entry into hardware */
    rv = soc_mem_write(unit, tcam_mem,
                       MEM_BLOCK_ALL, tcam_idx, &(tcam_new->hw_buf));
    SOC_IF_ERROR_RETURN(rv);

    /* Index was successfully allocated. */
    tcam_new->hw_idx = tcam_idx;
    tcam_entry_arr[tcam_idx].valid = 1;
    tcam_entry_arr[tcam_idx].tcam_info = tcam_new;

#ifdef BCM_TOMAHAWK_SUPPORT
    if (soc_feature(unit, soc_feature_udf_multi_pipe_support) &&
        (bcmi_xgs4_udf_oper_mode[unit] == bcmUdfOperModePipeLocal)) {
        tcam_new->hw_idx_per_pipe[tcam_new->associated_pipe] = tcam_idx;
    }
#endif

    return (BCM_E_NONE);
}

/*
 * Function:
 *     bcmi_xgs4_udf_tcam_entry_l3_parse
 * Purpose:
 *     Parse udf tcam entry l3 format match key.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     hw_buf   - (IN) Hw buffer.
 *     pkt_fmt  - (OUT) Packet format structure.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_xgs4_udf_tcam_entry_l3_parse(int unit, uint32 *hw_buf,
                                  bcm_udf_pkt_format_info_t *pkt_fmt)
{
    uint32 ethertype = -1;
    uint32 inner_iptype = -1;
    uint32 ethertype_mask = -1;
    uint32 outer_iptype = -1;
    uint32 outer_iptype_ip4_val = -1;
    uint32 outer_iptype_ip6_val = -1;
    uint32 outer_iptype_mask = -1;
    uint32 inner_iptype_mask = -1;
    uint32 l3fields = -1;
    uint32 l3fields_mask = -1;
    soc_mem_t mem = UDF_CTRL(unit)->tcam_mem;
    uint32 iptype_none_val = 0;
    uint8  fc_header_encode = 0;
    uint8  fc_hdr_encode_mask = 0;

    /* Input parameters check. */
    BCM_IF_NULL_RETURN_PARAM(hw_buf);
    BCM_IF_NULL_RETURN_PARAM(pkt_fmt);

    if (SOC_IS_TD_TT(unit)
        || SOC_IS_KATANAX(unit)
        || SOC_IS_TRIUMPH3(unit)) {
        iptype_none_val = 2;
    }
    if ((ethertype_mask =
            soc_mem_field32_get(unit, mem, hw_buf, L2_ETHER_TYPE_MASKf))) {
        ethertype = soc_mem_field32_get(unit, mem, hw_buf, L2_ETHER_TYPEf);
    }

    if ((inner_iptype_mask =
            soc_mem_field32_get(unit, mem, hw_buf, INNER_IP_TYPE_MASKf))) {
        inner_iptype = soc_mem_field32_get(unit, mem, hw_buf, INNER_IP_TYPEf);
    }

    outer_iptype = soc_mem_field32_get(unit, mem, hw_buf, OUTER_IP_TYPEf);
    outer_iptype_mask =  soc_mem_field32_get(unit, mem,
                                          hw_buf, OUTER_IP_TYPE_MASKf);

    if (soc_mem_field32_get(unit, mem, hw_buf, L3_FIELDS_MASKf)) {
        l3fields = soc_mem_field32_get(unit, mem, hw_buf, L3_FIELDSf);
        l3fields_mask = soc_mem_field32_get(unit, mem, hw_buf, L3_FIELDS_MASKf);
        if (0x8847 != ethertype) {
           /* MPLS_ANY has L3_FIELDSf == 0 */
           pkt_fmt->ip_protocol = (l3fields >> 16);
           pkt_fmt->ip_protocol_mask = (l3fields_mask >> 16);
        }
    }

    outer_iptype_ip4_val = OUTER_IP4_TYPE_VAL(unit);
    outer_iptype_ip6_val = OUTER_IP6_TYPE_VAL(unit);

    pkt_fmt->ethertype = ethertype;
    pkt_fmt->ethertype_mask = ethertype_mask;

    /*
     * Logic below implicitly checks Ethertype value for IP, MPLS and FCoE
     * frames.
     * 0x0800 - IPv4, 0x86DD - IPv6, 0x8847 - MPLS , 0x8906 + 0x8914 - FCoE
     */

    if ((ethertype == 0x800) && (outer_iptype == outer_iptype_ip4_val)) {
        pkt_fmt->tunnel = BCM_PKT_FORMAT_TUNNEL_NONE;
        pkt_fmt->outer_ip = BCM_PKT_FORMAT_IP4_WITH_OPTIONS;
        pkt_fmt->inner_ip = BCM_PKT_FORMAT_IP_NONE;
    } else if ((ethertype == 0x86dd) &&
               (outer_iptype == outer_iptype_ip6_val)) {
        pkt_fmt->tunnel = BCM_PKT_FORMAT_TUNNEL_NONE;
        pkt_fmt->outer_ip = BCM_PKT_FORMAT_IP6_WITH_OPTIONS;
        pkt_fmt->inner_ip = BCM_PKT_FORMAT_IP_NONE;
    } else if ((ethertype == 0x800) && (inner_iptype ==  iptype_none_val)) {
        pkt_fmt->tunnel = BCM_PKT_FORMAT_TUNNEL_NONE;
        pkt_fmt->outer_ip = BCM_PKT_FORMAT_IP4;
        pkt_fmt->inner_ip = BCM_PKT_FORMAT_IP_NONE;
    } else if ((ethertype == 0x86dd) && (inner_iptype ==  iptype_none_val)) {
        pkt_fmt->tunnel = BCM_PKT_FORMAT_TUNNEL_NONE;
        pkt_fmt->outer_ip = BCM_PKT_FORMAT_IP6;
        pkt_fmt->inner_ip = BCM_PKT_FORMAT_IP_NONE;
    } else if ((ethertype == 0x800) && (l3fields ==  0x40000)) {
        pkt_fmt->tunnel = BCM_PKT_FORMAT_TUNNEL_IP_IN_IP;
        pkt_fmt->outer_ip = BCM_PKT_FORMAT_IP4;
        pkt_fmt->inner_ip = BCM_PKT_FORMAT_IP4;
    } else if ((ethertype == 0x800) && (l3fields ==  0x290000)) {
        pkt_fmt->tunnel = BCM_PKT_FORMAT_TUNNEL_IP_IN_IP;
        pkt_fmt->outer_ip = BCM_PKT_FORMAT_IP4;
        pkt_fmt->inner_ip = BCM_PKT_FORMAT_IP6;
    } else if ((ethertype == 0x86dd) && (l3fields ==  0x40000)) {
        pkt_fmt->tunnel = BCM_PKT_FORMAT_TUNNEL_IP_IN_IP;
        pkt_fmt->outer_ip = BCM_PKT_FORMAT_IP6;
        pkt_fmt->inner_ip = BCM_PKT_FORMAT_IP4;
    } else if ((ethertype == 0x86dd) && (l3fields ==  0x290000)) {
        pkt_fmt->tunnel = BCM_PKT_FORMAT_TUNNEL_IP_IN_IP;
        pkt_fmt->outer_ip = BCM_PKT_FORMAT_IP6;
        pkt_fmt->inner_ip = BCM_PKT_FORMAT_IP6;
    } else if ((ethertype == 0x800) && (l3fields ==  0x2f0800)) {
        pkt_fmt->tunnel = BCM_PKT_FORMAT_TUNNEL_GRE;
        pkt_fmt->outer_ip = BCM_PKT_FORMAT_IP4;
        pkt_fmt->inner_ip = BCM_PKT_FORMAT_IP4;
    } else if ((ethertype == 0x800) && (l3fields ==  0x2f86dd)) {
        pkt_fmt->tunnel = BCM_PKT_FORMAT_TUNNEL_GRE;
        pkt_fmt->outer_ip = BCM_PKT_FORMAT_IP4;
        pkt_fmt->inner_ip = BCM_PKT_FORMAT_IP6;
    } else if ((ethertype == 0x86dd) && (l3fields ==  0x2f0800)) {
        pkt_fmt->tunnel = BCM_PKT_FORMAT_TUNNEL_GRE;
        pkt_fmt->outer_ip = BCM_PKT_FORMAT_IP6;
        pkt_fmt->inner_ip = BCM_PKT_FORMAT_IP4;
    } else if ((ethertype == 0x86dd) && (l3fields ==  0x2f86dd)) {
        pkt_fmt->tunnel = BCM_PKT_FORMAT_TUNNEL_GRE;
        pkt_fmt->outer_ip = BCM_PKT_FORMAT_IP6;
        pkt_fmt->inner_ip = BCM_PKT_FORMAT_IP6;
    } else if ((ethertype == 0x800) && (l3fields & 0xffff)) {
        pkt_fmt->tunnel = BCM_PKT_FORMAT_TUNNEL_GRE;
        pkt_fmt->outer_ip = BCM_PKT_FORMAT_IP4;
        pkt_fmt->inner_ip = BCM_PKT_FORMAT_IP_NONE;
        pkt_fmt->inner_protocol = (uint16)(l3fields & 0xffff);
        pkt_fmt->inner_protocol_mask = (uint16)(l3fields_mask & 0xffff);
    } else if ((ethertype == 0x86dd) && (l3fields & 0xffff)) {
        pkt_fmt->tunnel = BCM_PKT_FORMAT_TUNNEL_GRE;
        pkt_fmt->outer_ip = BCM_PKT_FORMAT_IP6;
        pkt_fmt->inner_ip = BCM_PKT_FORMAT_IP_NONE;
        pkt_fmt->inner_protocol = (uint16)(l3fields & 0xffff);
        pkt_fmt->inner_protocol_mask = (uint16)(l3fields_mask & 0xffff);
    } else if ((ethertype == 0x8847)) {
        pkt_fmt->tunnel = BCM_PKT_FORMAT_TUNNEL_MPLS;

        switch (l3fields & 0xf) {

            case 1:
                pkt_fmt->mpls = BCM_PKT_FORMAT_MPLS_ONE_LABEL;
                break;
            case 2:
                pkt_fmt->mpls = BCM_PKT_FORMAT_MPLS_TWO_LABELS;
                break;
            case 3:
                pkt_fmt->mpls = BCM_PKT_FORMAT_MPLS_THREE_LABELS;
                break;
            case 4:
                pkt_fmt->mpls = BCM_PKT_FORMAT_MPLS_FOUR_LABELS;
                break;
            case 5:
                pkt_fmt->mpls = BCM_PKT_FORMAT_MPLS_FIVE_LABELS;
                break;
            default :
                pkt_fmt->mpls = BCM_PKT_FORMAT_MPLS_ANY;
                break;
        }
#if defined (BCM_TOMAHAWK3_SUPPORT) || defined (BCM_TOMAHAWK2_SUPPORT)
        if (SOC_IS_TOMAHAWK3(unit) || SOC_IS_TOMAHAWK2(unit)) {
            pkt_fmt->first_2bytes_after_mpls_bos
                 = ((l3fields >> MPLS_LABEL_COUNT_WIDTH_FOR_UDF_L3_FIELDS)
                    & 0xffff);
            pkt_fmt->first_2bytes_after_mpls_bos_mask
                 = ((l3fields_mask
                   >> MPLS_LABEL_COUNT_WIDTH_FOR_UDF_L3_FIELDS)
                   & 0xffff);
        }
#endif

    } else if (ethertype == 0x8906 || ethertype == 0x8914) {
        pkt_fmt->tunnel = (ethertype == 0x8906)
            ? BCM_PKT_FORMAT_TUNNEL_FCOE
            : BCM_PKT_FORMAT_TUNNEL_FCOE_INIT;

        fc_header_encode = soc_mem_field32_get(unit, mem, hw_buf,
                                FC_HDR_ENCODE_1f);
        fc_hdr_encode_mask = soc_mem_field32_get(unit, mem, hw_buf,
                                FC_HDR_ENCODE_1_MASKf);

        if (0 != fc_header_encode && 0 != fc_hdr_encode_mask) {

            switch (fc_header_encode & fc_hdr_encode_mask) {
                case 1:
                    pkt_fmt->fibre_chan_outer
                        = BCM_PKT_FORMAT_FIBRE_CHAN;
                    break;
                case 2:
                    pkt_fmt->fibre_chan_outer
                        = BCM_PKT_FORMAT_FIBRE_CHAN_VIRTUAL;
                    break;
                case 3:
                    pkt_fmt->fibre_chan_outer
                        = BCM_PKT_FORMAT_FIBRE_CHAN_ENCAP;
                    break;
                case 4:
                    pkt_fmt->fibre_chan_outer
                        = BCM_PKT_FORMAT_FIBRE_CHAN_ROUTED;
                    break;
                default:
                    return (BCM_E_INTERNAL);
            }
        }

        fc_header_encode = soc_mem_field32_get(unit, mem, hw_buf,
                                FC_HDR_ENCODE_2f);
        fc_hdr_encode_mask = soc_mem_field32_get(unit, mem, hw_buf,
                                FC_HDR_ENCODE_2_MASKf);
        if (0 != fc_header_encode && 0 != fc_hdr_encode_mask) {

            switch (fc_header_encode & fc_hdr_encode_mask) {
                case 1:
                    pkt_fmt->fibre_chan_inner
                        = BCM_PKT_FORMAT_FIBRE_CHAN;
                    break;
                case 2:
                    pkt_fmt->fibre_chan_inner
                        = BCM_PKT_FORMAT_FIBRE_CHAN_VIRTUAL;
                    break;
                case 3:
                    pkt_fmt->fibre_chan_inner
                        = BCM_PKT_FORMAT_FIBRE_CHAN_ENCAP;
                    break;
                case 4:
                    pkt_fmt->fibre_chan_inner
                        = BCM_PKT_FORMAT_FIBRE_CHAN_ROUTED;
                    break;
                default:
                    return (BCM_E_INTERNAL);
            }
        }
    } else if (outer_iptype_mask == 0x7) {
        /*
         * For Non-IP traffic: OUTER_IP_TYPE_MASK = 0x7 and
         * outer_ip_type_val = 0 (TR2) or 2 (Trident, Katana).
         */
        pkt_fmt->tunnel = BCM_PKT_FORMAT_TUNNEL_NONE;
        pkt_fmt->outer_ip = BCM_PKT_FORMAT_IP_NONE;
        pkt_fmt->inner_ip = BCM_PKT_FORMAT_IP_NONE;
    } else if (0x0 == ethertype_mask && 0x0 == inner_iptype_mask
                && 0x0 == outer_iptype_mask) {
        /*
         * Do not care about IP headers and Tunnel Type.
         */
        pkt_fmt->tunnel = BCM_PKT_FORMAT_TUNNEL_ANY;
        pkt_fmt->outer_ip = BCM_PKT_FORMAT_IP_ANY;
        pkt_fmt->inner_ip = BCM_PKT_FORMAT_IP_ANY;
    } else if (outer_iptype_mask == 0x0) {
        /*
         * Do not care about Outer IP. Inner IP is not valid
         * as its not a Tunnel.
         */
        pkt_fmt->tunnel = BCM_PKT_FORMAT_TUNNEL_NONE;
        pkt_fmt->outer_ip = BCM_PKT_FORMAT_IP_ANY;
        pkt_fmt->inner_ip = BCM_PKT_FORMAT_IP_NONE;
    } else {
        return (BCM_E_INTERNAL);
    }


    return (BCM_E_NONE);
}

/*
 * Function:
 *     bcmi_xgs4_udf_tcam_entry_l3_init
 * Purpose:
 *     Initialize udf tcam entry l2 format match key.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     pkt_fmt  - (IN) Packet format structure.
 *     hw_buf   - (IN/OUT) Hw buffer.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_xgs4_udf_tcam_entry_l3_init(int unit,
                                 bcm_udf_pkt_format_info_t *pkt_fmt,
                                 uint32 *hw_buf)
{
    uint32 iptype_none_val = 0;
    uint32 outer_iptype_ip4_val = 0;
    uint32 outer_iptype_ip6_val = 0;

    uint16 l2_ethertype_val    = 0;
    uint16 l2_ethertype_mask   = 0;
    uint32 l3_fields_val       = 0;
    uint32 l3_fields_mask      = 0;
    uint32 inner_ip_type_val   = 0;
    uint32 inner_ip_type_mask  = 0;
    uint32 outer_ip_type_val   = 0;
    uint32 outer_ip_type_mask  = 0;
    uint32 inner_chn_key       = 0;
    uint32 inner_chn_mask      = 0;
    uint32 outer_chn_key       = 0;
    uint32 outer_chn_mask      = 0;
#if defined (BCM_TOMAHAWK3_SUPPORT) || defined (BCM_TOMAHAWK2_SUPPORT)
    uint16 first_2bytes_after_mpls_bos = 0;
    uint16 first_2bytes_after_mpls_bos_mask = 0;
#endif
    soc_mem_t tcam_mem         = UDF_CTRL(unit)->tcam_mem;


    /* Input parameters check. */
    BCM_IF_NULL_RETURN_PARAM(hw_buf);
    BCM_IF_NULL_RETURN_PARAM(pkt_fmt);

    if (SOC_IS_TD_TT(unit)
        || SOC_IS_KATANAX(unit)
        || SOC_IS_TRIUMPH3(unit)) {
        iptype_none_val = 2;
    }

    outer_iptype_ip4_val = OUTER_IP4_TYPE_VAL(unit);
    outer_iptype_ip6_val = OUTER_IP6_TYPE_VAL(unit);

    if (pkt_fmt->tunnel == BCM_PKT_FORMAT_TUNNEL_NONE) {

        /* inner ip type (none). */
        inner_ip_type_val   = iptype_none_val;
        inner_ip_type_mask  = 0x7;


        switch (pkt_fmt->outer_ip) {

            case BCM_PKT_FORMAT_IP4_WITH_OPTIONS:
                l2_ethertype_val     = 0x800;
                l2_ethertype_mask   = 0xffff;
                outer_ip_type_val = outer_iptype_ip4_val;
                outer_ip_type_mask     = 0x7;
                break;

            case BCM_PKT_FORMAT_IP6_WITH_OPTIONS:
                l2_ethertype_val     = 0x86dd;
                l2_ethertype_mask   = 0xffff;
                outer_ip_type_val = outer_iptype_ip6_val;
                outer_ip_type_mask     = 0x7;
                break;

            case BCM_PKT_FORMAT_IP4:
                l2_ethertype_val = 0x0800;
                l2_ethertype_mask   = 0xffff;
                break;

            case BCM_PKT_FORMAT_IP6:
                l2_ethertype_val = 0x86dd;
                l2_ethertype_mask   = 0xffff;
                break;

            case BCM_PKT_FORMAT_IP_NONE:
                outer_ip_type_val = iptype_none_val;
                outer_ip_type_mask     = 0x7;
                break;

            default:
                outer_ip_type_mask = 0x0;
        }
    } else if (pkt_fmt->tunnel == BCM_PKT_FORMAT_TUNNEL_IP_IN_IP) {
        l2_ethertype_mask = 0xffff;
        l3_fields_mask    = 0xff0000;
        if (pkt_fmt->inner_ip == BCM_PKT_FORMAT_IP4) {
            /* Inner ip protocol v4. */
            l3_fields_val  = 0x40000;
        } else if (pkt_fmt->inner_ip == BCM_PKT_FORMAT_IP6) {
            /* Inner ip protocol v6. */
            l3_fields_val  = 0x290000;
        } else {
            return (BCM_E_UNAVAIL);
        }

        if (pkt_fmt->outer_ip == BCM_PKT_FORMAT_IP4) {
            /* L2 ether type 0x800 */
            l2_ethertype_val     = 0x800;
        } else if (pkt_fmt->outer_ip == BCM_PKT_FORMAT_IP6) {
            /* L2 ether type 0x86dd */
            l2_ethertype_val     = 0x86dd;
        } else {
            return (BCM_E_UNAVAIL);
        }
    } else if (pkt_fmt->tunnel == BCM_PKT_FORMAT_TUNNEL_GRE) {

        l2_ethertype_mask = 0xffff;
        l3_fields_mask    = 0xffffff;
        if (pkt_fmt->inner_ip == BCM_PKT_FORMAT_IP4) {
            /* Inner ip protocol gre, gre ethertype 0x800. */
            l3_fields_val  = 0x2f0800;
        } else if (pkt_fmt->inner_ip == BCM_PKT_FORMAT_IP6) {
            /* Inner ip protocol gre, gre ethertype 0x86dd. */
            l3_fields_val  = 0x2f86dd;
        } else if (pkt_fmt->inner_ip == BCM_PKT_FORMAT_IP_NONE) {
            /*when set inner_ip to IP_NONE, inner_protocal should not be IPV4 and IPV6*/
            if (pkt_fmt->inner_protocol == 0x0800 ||
                pkt_fmt->inner_protocol == 0x86dd) {
                return BCM_E_PARAM;
            }
            l3_fields_val = (pkt_fmt->inner_protocol & 0xffff);
            l3_fields_mask = (pkt_fmt->inner_protocol_mask & 0xffff);
        } else {
            return (BCM_E_UNAVAIL);
        }

        if (pkt_fmt->outer_ip == BCM_PKT_FORMAT_IP4) {
            l2_ethertype_val     = 0x0800;
        } else if (pkt_fmt->outer_ip == BCM_PKT_FORMAT_IP6) {
            /* L2 ether type 0x86dd */
            l2_ethertype_val     = 0x86dd;
        } else {
            return (BCM_E_UNAVAIL);
        }
    } else if (pkt_fmt->tunnel == BCM_PKT_FORMAT_TUNNEL_MPLS) {

        l3_fields_mask = 0x0f;
        switch (pkt_fmt->mpls) {

            case BCM_PKT_FORMAT_MPLS_ONE_LABEL:
                l3_fields_val = 0x01;
                break;

            case BCM_PKT_FORMAT_MPLS_TWO_LABELS:
                l3_fields_val = 0x02;
                break;

#ifdef MPLS_FIVE_LABELS_SUPPORT
            
            case BCM_PKT_FORMAT_MPLS_THREE_LABELS:
                l3_fields_val = 0x03;
                break;

            case BCM_PKT_FORMAT_MPLS_FOUR_LABELS:
                l3_fields_val = 0x04;
                break;

            case BCM_PKT_FORMAT_MPLS_FIVE_LABELS:
                l3_fields_val = 0x05;
                break;
#endif /* MPLS_FIVE_LABELS_SUPPORT */
            case BCM_PKT_FORMAT_MPLS_ANY:
                l3_fields_val = 0x0;
                l3_fields_mask   = 0x0;
                break;

            default:
                return (BCM_E_UNAVAIL);
        }

        l2_ethertype_val = 0x8847;
        l2_ethertype_mask = 0xffff;

#if defined (BCM_TOMAHAWK3_SUPPORT) || defined (BCM_TOMAHAWK2_SUPPORT)
    if (SOC_IS_TOMAHAWK3(unit) || SOC_IS_TOMAHAWK2(unit)) {
        if (pkt_fmt->first_2bytes_after_mpls_bos_mask != 0) {
            first_2bytes_after_mpls_bos = soc_mem_field32_get(unit, tcam_mem,
                                            hw_buf, L3_FIELDSf);
            first_2bytes_after_mpls_bos_mask = soc_mem_field32_get(unit, tcam_mem,
                                                 hw_buf, L3_FIELDS_MASKf);

            first_2bytes_after_mpls_bos
               = ((first_2bytes_after_mpls_bos
                 >> MPLS_LABEL_COUNT_WIDTH_FOR_UDF_L3_FIELDS)
                 & 0xffff);
            first_2bytes_after_mpls_bos_mask
               = ((first_2bytes_after_mpls_bos_mask
                 >> MPLS_LABEL_COUNT_WIDTH_FOR_UDF_L3_FIELDS)
                 & 0xffff);

            if ((first_2bytes_after_mpls_bos_mask != 0) &&
                ((first_2bytes_after_mpls_bos & first_2bytes_after_mpls_bos_mask) !=
                (pkt_fmt->first_2bytes_after_mpls_bos
                & pkt_fmt->first_2bytes_after_mpls_bos_mask))) {
                return BCM_E_CONFIG;

            } else {
                l3_fields_val = (((pkt_fmt->first_2bytes_after_mpls_bos & 0xffff)
                                    << MPLS_LABEL_COUNT_WIDTH_FOR_UDF_L3_FIELDS) |
                                    (l3_fields_val & 0xf));
                l3_fields_mask = (((pkt_fmt->first_2bytes_after_mpls_bos_mask
                                     & 0xffff) << MPLS_LABEL_COUNT_WIDTH_FOR_UDF_L3_FIELDS) |
                                     (l3_fields_mask & 0xf));
            }
        }
    }
#endif

    } else if (pkt_fmt->tunnel == BCM_PKT_FORMAT_TUNNEL_FCOE ||
               pkt_fmt->tunnel == BCM_PKT_FORMAT_TUNNEL_FCOE_INIT) {
        if (!SOC_MEM_FIELD_VALID(unit, tcam_mem, FC_HDR_ENCODE_1f)) {
            return BCM_E_UNAVAIL;
        }

        l2_ethertype_val = ((pkt_fmt->tunnel ==
                            BCM_PKT_FORMAT_TUNNEL_FCOE) ?
                            0x8906 : 0x8914);

        l2_ethertype_mask = 0xffff;

        outer_chn_mask = 0x7;
        switch (pkt_fmt->fibre_chan_outer) {
            case BCM_PKT_FORMAT_FIBRE_CHAN:
                outer_chn_key = 1;
                break;
            case BCM_PKT_FORMAT_FIBRE_CHAN_ENCAP:
                outer_chn_key = 3;
                break;
            case BCM_PKT_FORMAT_FIBRE_CHAN_VIRTUAL:
                outer_chn_key = 2;
                break;
            case BCM_PKT_FORMAT_FIBRE_CHAN_ROUTED:
                outer_chn_key = 4;
                break;
            case BCM_PKT_FORMAT_FIBRE_CHAN_ANY:
                outer_chn_key = 0;
                outer_chn_mask = 0;
                break;
            default:
                return BCM_E_UNAVAIL;
        }

        /* Entry matching on two extended headers has higher priority */
        inner_chn_mask = 0x07;

        switch (pkt_fmt->fibre_chan_inner) {
            case BCM_PKT_FORMAT_FIBRE_CHAN:
                inner_chn_key = 1;
                break;
            case BCM_PKT_FORMAT_FIBRE_CHAN_ENCAP:
                inner_chn_key = 3;
                break;
            case BCM_PKT_FORMAT_FIBRE_CHAN_VIRTUAL:
                inner_chn_key = 2;
                break;
            case BCM_PKT_FORMAT_FIBRE_CHAN_ROUTED:
                inner_chn_key = 4;
                break;
            case BCM_PKT_FORMAT_FIBRE_CHAN_ANY:
                inner_chn_key = 0;
                inner_chn_mask = 0;
                break;
            default:
                return BCM_E_UNAVAIL;
        }

    } else if (BCM_PKT_FORMAT_TUNNEL_ANY == pkt_fmt->tunnel
                && BCM_PKT_FORMAT_IP_ANY == pkt_fmt->outer_ip
                && BCM_PKT_FORMAT_IP_ANY == pkt_fmt->inner_ip) {

        l2_ethertype_val = 0;
        l2_ethertype_mask = 0;
        l3_fields_val = 0;
        l3_fields_mask = 0;
        outer_ip_type_val = 0;
        outer_ip_type_mask = 0;
        inner_ip_type_val = 0;
        inner_ip_type_mask = 0;
        inner_chn_key =0;
        inner_chn_mask =0;
        outer_chn_key =0;
        outer_chn_mask =0;

    } else {
        return (BCM_E_UNAVAIL);
    }

    /* Configure ip_protocol/mask fields */
    if ((pkt_fmt->tunnel != BCM_PKT_FORMAT_TUNNEL_MPLS)) {
        l3_fields_val |= (pkt_fmt->ip_protocol << 16);
        l3_fields_mask |= (pkt_fmt->ip_protocol_mask << 16);
    }

    /* ALL the keys are configured. Now set them in entry */
    soc_mem_field32_set(unit, tcam_mem, hw_buf,
                        L2_ETHER_TYPEf, l2_ethertype_val);
    soc_mem_field32_set(unit, tcam_mem, hw_buf,
                        L2_ETHER_TYPE_MASKf, l2_ethertype_mask);
    soc_mem_field32_set(unit, tcam_mem, hw_buf,
                        L3_FIELDSf, l3_fields_val);
    soc_mem_field32_set(unit, tcam_mem, hw_buf,
                        L3_FIELDS_MASKf, l3_fields_mask);
    soc_mem_field32_set(unit, tcam_mem, hw_buf,
                        OUTER_IP_TYPEf, outer_ip_type_val);
    soc_mem_field32_set(unit, tcam_mem, hw_buf,
                        OUTER_IP_TYPE_MASKf, outer_ip_type_mask);
    soc_mem_field32_set(unit, tcam_mem, hw_buf,
                        INNER_IP_TYPEf, inner_ip_type_val);
    soc_mem_field32_set(unit, tcam_mem, hw_buf,
                        INNER_IP_TYPE_MASKf,inner_ip_type_mask);

    if (SOC_MEM_FIELD_VALID(unit, tcam_mem,FC_HDR_ENCODE_1f)) {
        soc_mem_field32_set(unit, tcam_mem, hw_buf,
                            FC_HDR_ENCODE_1f, outer_chn_key);
    }
    if (SOC_MEM_FIELD_VALID(unit, tcam_mem,FC_HDR_ENCODE_1_MASKf)) {
        soc_mem_field32_set(unit, tcam_mem, hw_buf,
                            FC_HDR_ENCODE_1_MASKf, outer_chn_mask);
    }
    if (SOC_MEM_FIELD_VALID(unit, tcam_mem,FC_HDR_ENCODE_2f)) {
        soc_mem_field32_set(unit, tcam_mem, hw_buf,
                            FC_HDR_ENCODE_2f, inner_chn_key);
    }
    if (SOC_MEM_FIELD_VALID(unit, tcam_mem,FC_HDR_ENCODE_2_MASKf)) {
        soc_mem_field32_set(unit, tcam_mem, hw_buf,
                            FC_HDR_ENCODE_2_MASKf, inner_chn_mask);
    }

    return (BCM_E_NONE);
}


/* Converts the misc packet format flags to fields in the UDF_TCAM */
STATIC int
bcmi_xgs4_udf_tcam_misc_format_to_hw_fields(
    int unit, int type,
    soc_field_t *data_f, soc_field_t *mask_f,
    int *present, int *none, int *any, int *support)
{

    soc_field_t hw_fields_f[2][7] = {
            {
                HIGIGf,        CNTAG_PRESENTf,
                ICNM_PACKETf,  VNTAG_PRESENTf,
                ETAG_PACKETf,  SUBPORT_TAG_PRESENTf,
                INT_PKTf
            },
            {
                HIGIG_MASKf,        CNTAG_PRESENT_MASKf,
                ICNM_PACKET_MASKf,  VNTAG_PRESENT_MASKf,
                ETAG_PACKET_MASKf,  SUBPORT_TAG_PRESENT_MASKf,
                INT_PKT_MASKf
            }
        };
    int sw_flags[3][7] = {
            {
                BCM_PKT_FORMAT_HIGIG_PRESENT,
                BCM_PKT_FORMAT_CNTAG_PRESENT,
                BCM_PKT_FORMAT_ICNM_PRESENT,
                BCM_PKT_FORMAT_VNTAG_PRESENT,
                BCM_PKT_FORMAT_ETAG_PRESENT,
                BCM_PKT_FORMAT_SUBPORT_TAG_PRESENT,
                BCM_PKT_FORMAT_INT_PKT_PRESENT,
            },
            {
                BCM_PKT_FORMAT_HIGIG_NONE,
                BCM_PKT_FORMAT_CNTAG_NONE,
                BCM_PKT_FORMAT_ICNM_NONE,
                BCM_PKT_FORMAT_VNTAG_NONE,
                BCM_PKT_FORMAT_ETAG_NONE,
                BCM_PKT_FORMAT_SUBPORT_TAG_NONE,
                BCM_PKT_FORMAT_INT_PKT_NONE,
            },
            {
                BCM_PKT_FORMAT_HIGIG_ANY,
                BCM_PKT_FORMAT_CNTAG_ANY,
                BCM_PKT_FORMAT_ICNM_ANY,
                BCM_PKT_FORMAT_VNTAG_ANY,
                BCM_PKT_FORMAT_ETAG_ANY,
                BCM_PKT_FORMAT_SUBPORT_TAG_ANY,
                BCM_PKT_FORMAT_INT_PKT_ANY,
            }
        };
    int ctrl_flags[1][7] = {
            {
                BCMI_XGS4_UDF_CTRL_TCAM_HIGIG,
                BCMI_XGS4_UDF_CTRL_TCAM_CNTAG,
                BCMI_XGS4_UDF_CTRL_TCAM_ICNM,
                BCMI_XGS4_UDF_CTRL_TCAM_VNTAG,
                BCMI_XGS4_UDF_CTRL_TCAM_ETAG,
                BCMI_XGS4_UDF_CTRL_TCAM_SUBPORT_TAG,
                BCMI_XGS4_UDF_CTRL_INT_PKT,
            }
        };

    *data_f     = hw_fields_f[0][type];
    *mask_f     = hw_fields_f[1][type];
    *present    = sw_flags[0][type];
    *none       = sw_flags[1][type];
    *any        = sw_flags[2][type];
    *support    = ctrl_flags[0][type];

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_xgs4_udf_tcam_entry_misc_parse
 * Purpose:
 *     Parse udf tcam entry flags format match key.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     hw_buf   - (IN) Hw buffer.
 *     flags    - (OUT) BCM_PKT_FORMAT_F_XXX
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_xgs4_udf_tcam_entry_misc_parse(int unit, int type,
                                    uint32 *hw_buf, uint16 *flags)
{
    int rv;
    soc_mem_t mem;
    uint32 data, mask;
    soc_field_t data_f, mask_f;
    int present, none, any, support;

    /* Input parameters check. */
    BCM_IF_NULL_RETURN_PARAM(hw_buf);
    BCM_IF_NULL_RETURN_PARAM(flags);

    mem = UDF_CTRL(unit)->tcam_mem;

    rv = bcmi_xgs4_udf_tcam_misc_format_to_hw_fields(unit, type,
             &data_f, &mask_f, &present, &none, &any, &support);
    BCM_IF_ERROR_RETURN(rv);

    /* If the field is not supported, return ANY */
    if (!(UDF_CTRL(unit)->flags & support)) {
        *flags = any;
        return BCM_E_NONE;
    }

    if (soc_mem_field32_get(unit, mem, hw_buf, mask_f)) {
        data = soc_mem_field32_get(unit, mem, hw_buf, data_f);
        mask = soc_mem_field32_get(unit, mem, hw_buf, mask_f);
        *flags |= ((mask == 0) ? any : ((data == 1) ? present : none));
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     bcmi_xgs4_udf_tcam_entry_misc_init
 * Purpose:
 *     Initialize udf tcam entry flags match key.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     flags    - (IN) BCM_PKT_FORMAT_F_XXX
 *     hw_buf   - (OUT) Hw buffer.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_xgs4_udf_tcam_entry_misc_init(int unit, int type,
                                   uint16 flags, uint32 *hw_buf)
{
    int rv;
    soc_mem_t mem;
    soc_field_t data_f, mask_f;
    int present, none, any, support;


    /* Input parameters check. */
    BCM_IF_NULL_RETURN_PARAM(hw_buf);

    mem = UDF_CTRL(unit)->tcam_mem;

    rv = bcmi_xgs4_udf_tcam_misc_format_to_hw_fields(unit, type,
             &data_f, &mask_f, &present, &none, &any, &support);
    BCM_IF_ERROR_RETURN(rv);

    /* If the field is not supported, only ANY is allowed,
     * hw_buf should not be set */
    if (!(UDF_CTRL(unit)->flags & support)) {
        if (flags != any) {
            return BCM_E_PARAM;
        } else {
            return BCM_E_NONE;
        }
    }

    /* Both PRESENT and NONE flags cannot be set
     * If it is required to match on all packets, set ANY */
    if ((flags & present) && (flags & none)) {
        return BCM_E_PARAM;
    }

    if ((flags & present) || (flags & none)) {
        soc_mem_field32_set(unit, mem, hw_buf, mask_f, 1);
        soc_mem_field32_set(unit, mem, hw_buf, data_f, (flags & present) ? 1:0);
    } else {
        /* any */
        soc_mem_field32_set(unit, mem, hw_buf, data_f, 0);
        soc_mem_field32_set(unit, mem, hw_buf, mask_f, 0);
    }

    return BCM_E_NONE;

}

/*
 * Function:
 *     bcmi_xgs4_udf_tcam_entry_l2format_init
 * Purpose:
 *     Initialize udf tcam entry l2 format match key.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     l2format - (IN) BCM_PKT_FORMAT_L2_XXX
 *     hw_buf   - (OUT) Hw buffer.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_xgs4_udf_tcam_entry_l2format_init(int unit,
                                       bcm_udf_pkt_format_info_t *pkt_fmt,
                                       uint32 *hw_buf)
{
    uint16 l2format = pkt_fmt->l2;
    soc_mem_t tcam_mem = UDF_CTRL(unit)->tcam_mem;  /* UDF_TCAM memory   */

    /* Input parameters check. */
    BCM_IF_NULL_RETURN_PARAM(hw_buf);
    BCM_IF_NULL_RETURN_PARAM(pkt_fmt);

    /* Translate L2 flag bits to index */
    switch (l2format) {
      case BCM_PKT_FORMAT_L2_ETH_II:
          /* L2 Format . */
          soc_mem_field32_set(unit, tcam_mem, hw_buf,
                              L2_TYPEf, 0);
          soc_mem_field32_set(unit, tcam_mem, hw_buf,
                              L2_TYPE_MASKf, 0x3);

          break;
      case BCM_PKT_FORMAT_L2_SNAP:
          /* L2 Format . */
          soc_mem_field32_set(unit, tcam_mem, hw_buf,
                              L2_TYPEf, 1);
          soc_mem_field32_set(unit, tcam_mem, hw_buf,
                              L2_TYPE_MASKf, 0x3);

          break;
      case BCM_PKT_FORMAT_L2_LLC:
          /* L2 Format . */
          soc_mem_field32_set(unit, tcam_mem, hw_buf,
                              L2_TYPEf, 2);
          soc_mem_field32_set(unit, tcam_mem, hw_buf,
                              L2_TYPE_MASKf, 0x3);

          break;
      case BCM_PKT_FORMAT_L2_ANY:
          /* L2 Format . */
          soc_mem_field32_set(unit, tcam_mem, hw_buf,
                              L2_TYPEf, 0);
          soc_mem_field32_set(unit, tcam_mem, hw_buf,
                              L2_TYPE_MASKf, 0);
          break;
      default:
          return (BCM_E_PARAM);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     bcmi_xgs4_udf_tcam_entry_l2format_parse
 * Purpose:
 *     Parse udf tcam entry l2 format match key.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     hw_buf   - (IN) Hw buffer.
 *     l2format - (OUT) BCM_PKT_FORMAT_L2_XXX
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_xgs4_udf_tcam_entry_l2format_parse(int unit, uint32 *hw_buf,
                                        bcm_udf_pkt_format_info_t *pkt_fmt)
{
    uint32 l2type;
    uint16 *l2format = &pkt_fmt->l2;
    soc_mem_t tcam_mem = UDF_CTRL(unit)->tcam_mem;  /* UDF_TCAM memory   */

    /* Input parameters check. */
    BCM_IF_NULL_RETURN_PARAM(hw_buf);
    BCM_IF_NULL_RETURN_PARAM(pkt_fmt);

    if (soc_mem_field32_get(unit, tcam_mem, hw_buf, L2_TYPE_MASKf)) {
        l2type = soc_mem_field32_get(unit, tcam_mem, hw_buf, L2_TYPEf);
        switch (l2type) {
          case 0:
              *l2format = BCM_PKT_FORMAT_L2_ETH_II;
              break;
          case 1:
              *l2format = BCM_PKT_FORMAT_L2_SNAP;
              break;
          case 2:
              *l2format = BCM_PKT_FORMAT_L2_LLC;
              break;
          default:
              break;
        }
    } else {
        *l2format = BCM_PKT_FORMAT_L2_ANY;
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     bcmi_xgs4_udf_tcam_entry_vlanformat_parse
 * Purpose:
 *     Parse udf tcam entry l2 format match key.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     hw_buf   - (IN) Hw buffer.
 *     vlanformat - (OUT) BCM_PKT_FORMAT_L2_XXX
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_xgs4_udf_tcam_entry_vlanformat_parse(int unit, uint32 *hw_buf,
                                          bcm_udf_pkt_format_info_t *pkt_fmt)
{
    uint32 tag_status;
    uint16 *vlanformat = &pkt_fmt->vlan_tag;
    soc_mem_t tcam_mem = UDF_CTRL(unit)->tcam_mem;  /* UDF_TCAM memory   */

    /* Input parameters check. */
    BCM_IF_NULL_RETURN_PARAM(hw_buf);
    BCM_IF_NULL_RETURN_PARAM(pkt_fmt);

    if (soc_mem_field32_get(unit, tcam_mem, hw_buf, L2_TAG_STATUS_MASKf)) {
        tag_status = soc_mem_field32_get(unit, tcam_mem, hw_buf,
                                         L2_TAG_STATUSf);
        switch (tag_status) {
          case 0:
              *vlanformat = BCM_PKT_FORMAT_VLAN_TAG_NONE;
              break;
          case 1:
              *vlanformat = BCM_PKT_FORMAT_VLAN_TAG_SINGLE;
              break;
          case 2:
              *vlanformat = BCM_PKT_FORMAT_VLAN_TAG_DOUBLE;
              break;
          default:
              break;
        }
    } else {
        *vlanformat = BCM_PKT_FORMAT_VLAN_TAG_ANY;
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     bcmi_xgs4_udf_tcam_entry_vlanformat_init
 * Purpose:
 *     Initialize udf tcam entry vlan tag format match key.
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     vlanformat - (IN) BCM_PKT_FORMAT_L2_XXX
 *     hw_buf     - (IN/OUT) Hw buffer.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_xgs4_udf_tcam_entry_vlanformat_init(int unit,
                                         bcm_udf_pkt_format_info_t *pkt_fmt,
                                         uint32 *hw_buf)
{
    uint16 vlanformat = pkt_fmt->vlan_tag;
    soc_mem_t tcam_mem = UDF_CTRL(unit)->tcam_mem;  /* UDF_TCAM memory   */

    /* Input parameters check. */
    BCM_IF_NULL_RETURN_PARAM(hw_buf);
    BCM_IF_NULL_RETURN_PARAM(pkt_fmt);

    /* Translate L2 flag bits to index */
    switch (vlanformat) {
        case BCM_PKT_FORMAT_VLAN_TAG_NONE:
            /* L2 Format . */
            soc_mem_field32_set(unit, tcam_mem, hw_buf,
                                L2_TAG_STATUSf, 0);
            if (soc_feature(unit, soc_feature_fp_no_inner_vlan_support)) {
                soc_mem_field32_set(unit, tcam_mem, hw_buf,
                                    L2_TAG_STATUS_MASKf, 0x1);
            } else {
                soc_mem_field32_set(unit, tcam_mem, hw_buf,
                                    L2_TAG_STATUS_MASKf, 0x3);
            }

          break;
        case BCM_PKT_FORMAT_VLAN_TAG_SINGLE:
            /* L2 Format . */
            soc_mem_field32_set(unit, tcam_mem, hw_buf,
                                L2_TAG_STATUSf, 1);
            if (soc_feature(unit, soc_feature_fp_no_inner_vlan_support)) {
                soc_mem_field32_set(unit, tcam_mem, hw_buf,
                                    L2_TAG_STATUS_MASKf, 0x1);
            } else {
                soc_mem_field32_set(unit, tcam_mem, hw_buf,
                                    L2_TAG_STATUS_MASKf, 0x3);
            }

        break;
        case BCM_PKT_FORMAT_VLAN_TAG_DOUBLE:
            if (soc_feature(unit, soc_feature_fp_no_inner_vlan_support)) {
                return (BCM_E_UNAVAIL);
            } else {
                /* L2 Format . */
                soc_mem_field32_set(unit, tcam_mem, hw_buf,
                                    L2_TAG_STATUSf, 2);
                soc_mem_field32_set(unit, tcam_mem, hw_buf,
                                    L2_TAG_STATUS_MASKf, 0x3);
            }

        break;
        case BCM_PKT_FORMAT_VLAN_TAG_ANY:
          /* L2 Format . */
            soc_mem_field32_set(unit, tcam_mem, hw_buf,
                                L2_TAG_STATUSf, 0);
            soc_mem_field32_set(unit, tcam_mem, hw_buf,
                                L2_TAG_STATUS_MASKf, 0);
        break;
        default:
            return (BCM_E_PARAM);
    }
    return (BCM_E_NONE);
}


/*
 * Function:
 *     bcmi_xgs4_udf_tcam_entry_lb_pkt_type_parse
 * Purpose:
 *     Parse udf tcam entry loopback pkt type match key.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     hw_buf   - (IN) Hw buffer.
 *     lb_pkt_type - (OUT) BCM_PKT_FORMAT_LB_TYPE_XXX
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_xgs4_udf_tcam_entry_lb_pkt_type_parse(int unit, uint32 *hw_buf,
                                          bcm_udf_pkt_format_info_t *pkt_fmt)
{
    uint32 type;
    uint8 *lb_pkt_type = &pkt_fmt->lb_pkt_type;
    soc_mem_t tcam_mem = UDF_CTRL(unit)->tcam_mem;  /* UDF_TCAM memory   */

    /* Input parameters check. */
    BCM_IF_NULL_RETURN_PARAM(hw_buf);
    BCM_IF_NULL_RETURN_PARAM(pkt_fmt);

    if (soc_mem_field32_get(unit, tcam_mem, hw_buf, LOOPBACK_PKT_TYPE_MASKf)) {
        type = soc_mem_field32_get(unit, tcam_mem, hw_buf,
                                         LOOPBACK_PKT_TYPEf);
        switch (type) {
          case 1:
              if (SOC_IS_TOMAHAWK3(unit)) {
                  *lb_pkt_type = BCM_PKT_FORMAT_LB_TYPE_GENERIC;
              } else {
                  *lb_pkt_type = BCM_PKT_FORMAT_LB_TYPE_MIM;
              }
              break;
          default:
              break;
        }
    } else {
        *lb_pkt_type = BCM_PKT_FORMAT_LB_TYPE_ANY;
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     bcmi_xgs4_udf_tcam_entry_l4_dst_port_parse
 * Purpose:
 *     Parse udf tcam entry loopback pkt type match key.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     hw_buf   - (IN) Hw buffer.
 *     l4_dst_port - (OUT) BCM_PKT_FORMAT_LB_TYPE_XXX
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_xgs4_udf_tcam_entry_l4_dst_port_parse(int unit, uint32 *hw_buf,
                                          bcm_udf_pkt_format_info_t *pkt_fmt)
{
    soc_mem_t tcam_mem = UDF_CTRL(unit)->tcam_mem;  /* UDF_TCAM memory   */

    /* Input parameters check. */
    BCM_IF_NULL_RETURN_PARAM(hw_buf);
    BCM_IF_NULL_RETURN_PARAM(pkt_fmt);

    pkt_fmt->l4_dst_port = soc_mem_field32_get(unit, tcam_mem, hw_buf,
                                               L4_DST_PORTf);
    pkt_fmt->l4_dst_port_mask = soc_mem_field32_get(unit, tcam_mem, hw_buf,
                                               L4_DST_PORT_MASKf);

    return (BCM_E_NONE);

}

/*
 * Function:
 *     bcmi_xgs4_udf_tcam_entry_opaque_tag_type_parse
 * Purpose:
 *     Parse udf tcam entry loopback pkt type match key.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     hw_buf   - (IN) Hw buffer.
 *     opaque_tag_type - (OUT) BCM_PKT_FORMAT_LB_TYPE_XXX
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_xgs4_udf_tcam_entry_opaque_tag_type_parse(int unit, uint32 *hw_buf,
                                          bcm_udf_pkt_format_info_t *pkt_fmt)
{
    soc_mem_t tcam_mem = UDF_CTRL(unit)->tcam_mem;  /* UDF_TCAM memory   */

    /* Input parameters check. */
    BCM_IF_NULL_RETURN_PARAM(hw_buf);
    BCM_IF_NULL_RETURN_PARAM(pkt_fmt);


    pkt_fmt->opaque_tag_type = soc_mem_field32_get(unit, tcam_mem, hw_buf,
                                                   OPAQUE_TAG_TYPEf);
    pkt_fmt->opaque_tag_type_mask = soc_mem_field32_get(unit, tcam_mem, hw_buf,
                                                        OPAQUE_TAG_TYPE_MASKf);

    return (BCM_E_NONE);

}

/*
 * Function:
 *     bcmi_xgs4_udf_tcam_entry_src_port_parse
 * Purpose:
 *     Parse udf tcam entry loopback pkt type match key.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     hw_buf   - (IN) Hw buffer.
 *     src_port - (OUT) BCM_PKT_FORMAT_LB_TYPE_XXX
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_xgs4_udf_tcam_entry_src_port_parse(int unit, uint32 *hw_buf,
                                          bcm_udf_pkt_format_info_t *pkt_fmt)
{
    soc_mem_t tcam_mem = UDF_CTRL(unit)->tcam_mem;  /* UDF_TCAM memory   */

    /* Input parameters check. */
    BCM_IF_NULL_RETURN_PARAM(hw_buf);
    BCM_IF_NULL_RETURN_PARAM(pkt_fmt);

    pkt_fmt->src_port = soc_mem_field32_get(unit, tcam_mem, hw_buf,
                                            SOURCE_PORT_NUMBERf);
    pkt_fmt->src_port_mask = soc_mem_field32_get(unit, tcam_mem, hw_buf,
                                             SOURCE_PORT_NUMBER_MASKf);

    return (BCM_E_NONE);


}

/*
 * Function:
 *     bcmi_xgs4_udf_tcam_entry_parse
 * Purpose:
 *     Parse udf tcam entry key to packet format info
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     hw_buf   - (IN) Hw buffer.
 *     pkt_fmt  - (OUT) Packet format to be filled in.
 * Returns:
 *     BCM_E_XXX
 */

STATIC int
bcmi_xgs4_udf_tcam_entry_parse(int unit, uint32 *hw_buf,
                               bcm_udf_pkt_format_info_t *pkt_fmt)
{
    int rv;
    soc_mem_t tcam_mem = UDF_CTRL(unit)->tcam_mem;  /* UDF_TCAM memory   */

    /* Input parameters check. */
    BCM_IF_NULL_RETURN_PARAM(hw_buf);

    bcm_udf_pkt_format_info_t_init(pkt_fmt);

    /* If entry not valid, return empty pkt_format. */
    if (!soc_mem_field32_get(unit, tcam_mem, hw_buf, VALIDf)) {
        return (BCM_E_NONE);
    }

    /* Parse vlan_tag format.*/
    rv = bcmi_xgs4_udf_tcam_entry_vlanformat_parse(unit, hw_buf, pkt_fmt);
    BCM_IF_ERROR_RETURN(rv);

    /* Parse l2 format.*/
    rv = bcmi_xgs4_udf_tcam_entry_l2format_parse(unit, hw_buf, pkt_fmt);
    BCM_IF_ERROR_RETURN(rv);

    /* Parse l3 fields.*/
    rv = bcmi_xgs4_udf_tcam_entry_l3_parse(unit, hw_buf, pkt_fmt);
    BCM_IF_ERROR_RETURN(rv);

    if (UDF_CTRL(unit)->flags & BCMI_XGS4_UDF_CTRL_TCAM_HIGIG) {
       rv = bcmi_xgs4_udf_tcam_entry_misc_parse(unit, bcmiUdfPktFormatHigig,
                                             hw_buf, (uint16 *)&pkt_fmt->higig);
       BCM_IF_ERROR_RETURN(rv);
    }

    if (UDF_CTRL(unit)->flags & BCMI_XGS4_UDF_CTRL_TCAM_VNTAG) {
       rv = bcmi_xgs4_udf_tcam_entry_misc_parse(unit, bcmiUdfPktFormatVntag,
                                             hw_buf, (uint16 *)&pkt_fmt->vntag);
       BCM_IF_ERROR_RETURN(rv);
    }

    if (UDF_CTRL(unit)->flags & BCMI_XGS4_UDF_CTRL_TCAM_ETAG) {
       rv = bcmi_xgs4_udf_tcam_entry_misc_parse(unit, bcmiUdfPktFormatEtag,
                                             hw_buf, (uint16 *)&pkt_fmt->etag);
       BCM_IF_ERROR_RETURN(rv);
    }

    if (UDF_CTRL(unit)->flags & BCMI_XGS4_UDF_CTRL_TCAM_CNTAG) {
       rv = bcmi_xgs4_udf_tcam_entry_misc_parse(unit, bcmiUdfPktFormatCntag,
                                             hw_buf, (uint16 *)&pkt_fmt->cntag);
       BCM_IF_ERROR_RETURN(rv);
    }

    if (UDF_CTRL(unit)->flags & BCMI_XGS4_UDF_CTRL_TCAM_ICNM) {
       rv = bcmi_xgs4_udf_tcam_entry_misc_parse(unit, bcmiUdfPktFormatIcnm,
                                             hw_buf, (uint16 *)&pkt_fmt->icnm);
       BCM_IF_ERROR_RETURN(rv);
    }

    if (UDF_CTRL(unit)->flags & BCMI_XGS4_UDF_CTRL_TCAM_SUBPORT_TAG) {
       rv = bcmi_xgs4_udf_tcam_entry_misc_parse(unit, bcmiUdfPktFormatSubportTag,
                                             hw_buf, (uint16 *)&pkt_fmt->subport_tag);
       BCM_IF_ERROR_RETURN(rv);
    }

    if (UDF_CTRL(unit)->flags & BCMI_XGS4_UDF_CTRL_INT_PKT) {
       rv = bcmi_xgs4_udf_tcam_entry_misc_parse(unit, bcmiUdfPktFormatIntPkt,
                                                hw_buf, (uint16 *)&pkt_fmt->int_pkt);
       BCM_IF_ERROR_RETURN(rv);
    }

    /* Parse lb_pkt_type fields.*/
    if (UDF_CTRL(unit)->flags & BCMI_XGS4_UDF_CTRL_LB_PKT_TYPE) {
       rv = bcmi_xgs4_udf_tcam_entry_lb_pkt_type_parse(unit, hw_buf, pkt_fmt);
       BCM_IF_ERROR_RETURN(rv);
    }

    /* Parse l4_dst_port fields.*/
    if (UDF_CTRL(unit)->flags & BCMI_XGS4_UDF_CTRL_L4_DST_PORT) {
       rv = bcmi_xgs4_udf_tcam_entry_l4_dst_port_parse(unit, hw_buf, pkt_fmt);
       BCM_IF_ERROR_RETURN(rv);
    }

    /* Parse opaque_tag_type fields.*/
    if (UDF_CTRL(unit)->flags & BCMI_XGS4_UDF_CTRL_OPAQUE_TAG_TYPE) {
       rv = bcmi_xgs4_udf_tcam_entry_opaque_tag_type_parse(unit, hw_buf, pkt_fmt);
       BCM_IF_ERROR_RETURN(rv);
    }

    /* Parse src_port fields.*/
    if (UDF_CTRL(unit)->flags & BCMI_XGS4_UDF_CTRL_SRC_PORT) {
       rv = bcmi_xgs4_udf_tcam_entry_src_port_parse(unit, hw_buf, pkt_fmt);
       BCM_IF_ERROR_RETURN(rv);
    }

    return (BCM_E_NONE);
}

/* Parse ethertype and mask from pkt_format to hw_buf */
STATIC int
bcmi_xgs4_udf_tcam_ethertype_init(int unit,
        bcm_udf_pkt_format_info_t *pkt_format, uint32 *hw_buf)
{
    soc_mem_t mem;
    bcm_ethertype_t ethertype;
    bcm_ethertype_t ethertype_mask;

    mem = UDF_CTRL(unit)->tcam_mem;

    if (pkt_format->ethertype_mask != 0) {
        ethertype = soc_mem_field32_get(unit, mem,
                                        hw_buf, L2_ETHER_TYPEf);
        ethertype_mask = soc_mem_field32_get(unit, mem,
                                             hw_buf, L2_ETHER_TYPE_MASKf);

        if ((ethertype_mask != 0) &&
            ((ethertype & ethertype_mask) !=
             (pkt_format->ethertype & pkt_format->ethertype_mask))) {
            return BCM_E_CONFIG;
        } else {
            soc_mem_field32_set(unit, mem, hw_buf,
                               L2_ETHER_TYPEf, pkt_format->ethertype);
            soc_mem_field32_set(unit, mem, hw_buf,
                               L2_ETHER_TYPE_MASKf, pkt_format->ethertype_mask);
        }
    }

    return BCM_E_NONE;
}

/* Parse l4_dst_port and mask from pkt_format to hw_buf */
STATIC int
bcmi_xgs4_udf_tcam_entry_l4_dst_port_init(int unit,
        bcm_udf_pkt_format_info_t *pkt_format, uint32 *hw_buf)
{
    soc_mem_t mem;
    uint32 l4_dst_port;
    uint32 l4_dst_port_mask;

    mem = UDF_CTRL(unit)->tcam_mem;

    if (pkt_format->l4_dst_port_mask != 0) {
        l4_dst_port = soc_mem_field32_get(unit, mem,
                                        hw_buf, L4_DST_PORTf);
        l4_dst_port_mask = soc_mem_field32_get(unit, mem,
                                             hw_buf, L4_DST_PORT_MASKf);

        if ((l4_dst_port_mask != 0) &&
            ((l4_dst_port & l4_dst_port_mask) !=
             (pkt_format->l4_dst_port & pkt_format->l4_dst_port_mask))) {
            return BCM_E_CONFIG;
        } else {
            soc_mem_field32_set(unit, mem, hw_buf,
                               L4_DST_PORTf, pkt_format->l4_dst_port);
            soc_mem_field32_set(unit, mem, hw_buf,
                               L4_DST_PORT_MASKf, pkt_format->l4_dst_port_mask);
        }
    }

    return BCM_E_NONE;
}

/* Parse opaque_tag_type and mask from pkt_format to hw_buf */
STATIC int
bcmi_xgs4_udf_tcam_entry_opaque_tag_type_init(int unit,
        bcm_udf_pkt_format_info_t *pkt_format, uint32 *hw_buf)
{
    soc_mem_t mem;
    uint8 opaque_tag_type;
    uint8 opaque_tag_type_mask;

    mem = UDF_CTRL(unit)->tcam_mem;

    if (pkt_format->opaque_tag_type_mask != 0) {
        opaque_tag_type = soc_mem_field32_get(unit, mem,
                                        hw_buf, OPAQUE_TAG_TYPEf);
        opaque_tag_type_mask = soc_mem_field32_get(unit, mem,
                                             hw_buf, OPAQUE_TAG_TYPE_MASKf);

        if ((opaque_tag_type_mask != 0) &&
            ((opaque_tag_type & opaque_tag_type_mask) !=
             (pkt_format->opaque_tag_type & pkt_format->opaque_tag_type_mask))) {
            return BCM_E_CONFIG;
        } else {
            soc_mem_field32_set(unit, mem, hw_buf,
                               OPAQUE_TAG_TYPEf, pkt_format->opaque_tag_type);
            soc_mem_field32_set(unit, mem, hw_buf,
                               OPAQUE_TAG_TYPE_MASKf, pkt_format->opaque_tag_type_mask);
        }
    }

    return BCM_E_NONE;
}

/* Parse src_port and mask from pkt_format to hw_buf */
STATIC int
bcmi_xgs4_udf_tcam_entry_src_port_init(int unit,
        bcm_udf_pkt_format_info_t *pkt_format, uint32 *hw_buf)
{
    soc_mem_t mem;
    uint8 src_port;
    uint8 src_port_mask;

    mem = UDF_CTRL(unit)->tcam_mem;

    if (pkt_format->src_port_mask != 0) {
        src_port = soc_mem_field32_get(unit, mem,
                                        hw_buf, SOURCE_PORT_NUMBERf);
        src_port_mask = soc_mem_field32_get(unit, mem,
                                             hw_buf, SOURCE_PORT_NUMBER_MASKf);

        if ((src_port_mask != 0) &&
            ((src_port & src_port_mask) !=
             (pkt_format->src_port & pkt_format->src_port_mask))) {
            return BCM_E_CONFIG;
        } else {
            soc_mem_field32_set(unit, mem, hw_buf,
                               SOURCE_PORT_NUMBERf, pkt_format->src_port);
            soc_mem_field32_set(unit, mem, hw_buf,
                               SOURCE_PORT_NUMBER_MASKf, pkt_format->src_port_mask);
        }
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *     bcmi_xgs4_udf_tcam_entry_lb_pkt_type_init
 * Purpose:
 *     Initialize udf tcam entry loopback packet type match key.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     l2format - (IN) BCM_PKT_FORMAT_LB_TYPE_XXX
 *     hw_buf   - (OUT) Hw buffer.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
bcmi_xgs4_udf_tcam_entry_lb_pkt_type_init(int unit,
                                       bcm_udf_pkt_format_info_t *pkt_fmt,
                                       uint32 *hw_buf)
{
    uint16 type = pkt_fmt->lb_pkt_type;
    soc_mem_t tcam_mem = UDF_CTRL(unit)->tcam_mem;  /* UDF_TCAM memory   */

    /* Input parameters check. */
    BCM_IF_NULL_RETURN_PARAM(hw_buf);
    BCM_IF_NULL_RETURN_PARAM(pkt_fmt);

    /* Translate L2 flag bits to index */
    switch (type) {
      case BCM_PKT_FORMAT_LB_TYPE_MIM:
          /* L2 Format . */
          if (SOC_IS_TOMAHAWK3(unit)) {
              return (BCM_E_PARAM);
          } else {
              soc_mem_field32_set(unit, tcam_mem, hw_buf,
                                  LOOPBACK_PKT_TYPEf, 1);
              soc_mem_field32_set(unit, tcam_mem, hw_buf,
                                  LOOPBACK_PKT_TYPE_MASKf, 0x3);
          }
          break;
      case BCM_PKT_FORMAT_LB_TYPE_GENERIC:
          /* L2 Format . */
          if (SOC_IS_TOMAHAWK3(unit)) {
              soc_mem_field32_set(unit, tcam_mem, hw_buf,
                                  LOOPBACK_PKT_TYPEf, 1);
              soc_mem_field32_set(unit, tcam_mem, hw_buf,
                                  LOOPBACK_PKT_TYPE_MASKf, 0x3);
          } else {
              return (BCM_E_PARAM);
          }
          break;
      case BCM_PKT_FORMAT_LB_TYPE_ANY:
          soc_mem_field32_set(unit, tcam_mem, hw_buf,
                              LOOPBACK_PKT_TYPEf, 0);
          soc_mem_field32_set(unit, tcam_mem, hw_buf,
                              LOOPBACK_PKT_TYPE_MASKf, 0);

          break;
      default:
          return (BCM_E_PARAM);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _field_trx2_data_qualifier_etype_tcam_key_init
 * Purpose:
 *      Initialize ethertype based udf tcam entry.
 * Parameters:
 *      unit       - (IN) bcm device.
 *      pkt_format - (IN) Packet format info
 *      hw_buf     - (OUT) Hardware buffer.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
bcmi_xgs4_udf_pkt_format_tcam_key_init(int unit,
                                       bcm_udf_pkt_format_info_t *pkt_format,
                                       uint32 *hw_buf)
{
    int rv;
    soc_mem_t tcam_mem = UDF_CTRL(unit)->tcam_mem;  /* UDF_TCAM memory   */

    /* Input parameters check. */
    BCM_IF_NULL_RETURN_PARAM(hw_buf);
    BCM_IF_NULL_RETURN_PARAM(pkt_format);

    /* Set valid bit. */
    soc_mem_field32_set(unit, tcam_mem, hw_buf, VALIDf, 1);

    /* Set l2 format. */
    rv = bcmi_xgs4_udf_tcam_entry_l2format_init(unit, pkt_format, hw_buf);
    BCM_IF_ERROR_RETURN(rv);

    /* Set vlan tag format. */
    rv = bcmi_xgs4_udf_tcam_entry_vlanformat_init(unit, pkt_format, hw_buf);
    BCM_IF_ERROR_RETURN(rv);

    /* Set l3 packet format. */
    rv = bcmi_xgs4_udf_tcam_entry_l3_init(unit, pkt_format, hw_buf);
    BCM_IF_ERROR_RETURN(rv);

    /* Set HiGig packet formats */
    if (UDF_CTRL(unit)->flags & BCMI_XGS4_UDF_CTRL_TCAM_HIGIG) {
        rv = bcmi_xgs4_udf_tcam_entry_misc_init(unit, bcmiUdfPktFormatHigig,
                                                pkt_format->higig, hw_buf);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Set VNTAG packet formats */
    if (UDF_CTRL(unit)->flags & BCMI_XGS4_UDF_CTRL_TCAM_VNTAG) {
        rv = bcmi_xgs4_udf_tcam_entry_misc_init(unit, bcmiUdfPktFormatVntag,
                                                pkt_format->vntag, hw_buf);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Set ETAG packet formats */
    if (UDF_CTRL(unit)->flags & BCMI_XGS4_UDF_CTRL_TCAM_ETAG) {
        rv = bcmi_xgs4_udf_tcam_entry_misc_init(unit, bcmiUdfPktFormatEtag,
                                                pkt_format->etag, hw_buf);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Set CNTAG packet formats */
    if (UDF_CTRL(unit)->flags & BCMI_XGS4_UDF_CTRL_TCAM_CNTAG) {
        rv = bcmi_xgs4_udf_tcam_entry_misc_init(unit, bcmiUdfPktFormatCntag,
                                                pkt_format->cntag, hw_buf);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Set ICNM packet formats */
    if (UDF_CTRL(unit)->flags & BCMI_XGS4_UDF_CTRL_TCAM_ICNM) {
        rv = bcmi_xgs4_udf_tcam_entry_misc_init(unit, bcmiUdfPktFormatIcnm,
                                               pkt_format->icnm, hw_buf);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Set Subport tag packet formats */
    if (UDF_CTRL(unit)->flags & BCMI_XGS4_UDF_CTRL_TCAM_SUBPORT_TAG) {
        rv = bcmi_xgs4_udf_tcam_entry_misc_init(unit, bcmiUdfPktFormatSubportTag,
                                                pkt_format->subport_tag, hw_buf);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Set Inband Telemetry Pkt packet formats */
    if (UDF_CTRL(unit)->flags & BCMI_XGS4_UDF_CTRL_INT_PKT) {
        rv = bcmi_xgs4_udf_tcam_entry_misc_init(unit, bcmiUdfPktFormatIntPkt,
                                                pkt_format->int_pkt, hw_buf);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Set L4 Dst Port packet formats */
    if (UDF_CTRL(unit)->flags & BCMI_XGS4_UDF_CTRL_L4_DST_PORT) {
        rv = bcmi_xgs4_udf_tcam_entry_l4_dst_port_init(unit, pkt_format, hw_buf);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Set Opaque Tag Type packet formats */
    if (UDF_CTRL(unit)->flags & BCMI_XGS4_UDF_CTRL_OPAQUE_TAG_TYPE) {
        rv = bcmi_xgs4_udf_tcam_entry_opaque_tag_type_init(unit, pkt_format, hw_buf);
        BCM_IF_ERROR_RETURN(rv);
    }


    /* Set Source Port packet formats */
    if (UDF_CTRL(unit)->flags & BCMI_XGS4_UDF_CTRL_SRC_PORT) {
        rv = bcmi_xgs4_udf_tcam_entry_src_port_init(unit, pkt_format, hw_buf);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Set Loopback Packet Type packet formats */
    if (UDF_CTRL(unit)->flags & BCMI_XGS4_UDF_CTRL_LB_PKT_TYPE) {
        rv = bcmi_xgs4_udf_tcam_entry_lb_pkt_type_init(unit, pkt_format, hw_buf);
        BCM_IF_ERROR_RETURN(rv);
    }


    rv = bcmi_xgs4_udf_tcam_ethertype_init(unit, pkt_format, hw_buf);
    BCM_IF_ERROR_RETURN(rv);

    return (rv);
}

/*
 * Function:
 *      bcmi_xgs4_udf_offset_node_get
 * Purpose:
 *      Fetches UDF offset node given udf id.
 * Parameters:
 *      unit            - (IN)      Unit number.
 *      udf_id          - (IN)      UDF Id.
 *      offset_info     - (INOUT)   Fecthed udf offset info.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_xgs4_udf_offset_node_get(int unit, bcm_udf_id_t udf_id,
                              bcmi_xgs4_udf_offset_info_t **cur)
{
    bcmi_xgs4_udf_offset_info_t *tmp;

    tmp = UDF_CTRL(unit)->offset_info_head;

    while (tmp != NULL) {
        if (tmp->udf_id == udf_id) {
            break;
        }
        tmp = tmp->next;
    }

    /* Reached end of the list */
    if (tmp == NULL) {
        *cur = NULL;
        return BCM_E_NOT_FOUND;
    }

    *cur = tmp;

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcmi_xgs4_udf_tcam_node_get
 * Purpose:
 *      Fetches UDF tcam node given packet format id.
 * Parameters:
 *      unit            - (IN)      Unit number.
 *      pkt_format_id   - (IN)      UDF Packet format Id.
 *      offset_info     - (INOUT)   Fecthed udf tcam info.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_xgs4_udf_tcam_node_get(int unit,
                            bcm_udf_pkt_format_id_t pkt_format_id,
                            bcmi_xgs4_udf_tcam_info_t **cur)
{
    bcmi_xgs4_udf_tcam_info_t *tmp;

    tmp = UDF_CTRL(unit)->tcam_info_head;

    while (tmp != NULL) {
        if (tmp->pkt_format_id == pkt_format_id) {
            break;
        }
        tmp = tmp->next;
    }

    /* Reached end of the list */
    if (tmp == NULL) {
        *cur = NULL;
        return BCM_E_NOT_FOUND;
    }

    *cur = tmp;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs4_udf_detach
 * Purpose:
 *      Detaches udf module and cleans software state.
 * Parameters:
 *      unit            - (IN)      Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_xgs4_udf_detach(int unit)
{
    UDF_INIT_CHECK(unit);

    /* Free resources related to UDF module */
    BCM_IF_ERROR_RETURN(bcmi_xgs4_udf_ctrl_free(unit, UDF_CTRL(unit)));

    /* Reset udf module init state */

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs4_udf_init
 * Purpose:
 *      initializes/ReInitializes the UDF module.
 * Parameters:
 *      unit            - (IN)      Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_xgs4_udf_init(int unit)
{
    int rv;

#if defined (BCM_TOMAHAWK_SUPPORT)
        if (soc_feature(unit, soc_feature_udf_multi_pipe_support)) {
            bcmi_xgs4_udf_oper_mode[unit] = bcmUdfOperModeGlobal;
        }
#endif

    /* Init control structures */
    rv = bcmi_xgs4_udf_ctrl_init(unit);
    BCM_IF_ERROR_RETURN(rv);

#if defined (BCM_WARM_BOOT_SUPPORT)
    if (SOC_WARM_BOOT(unit)) {
        rv = bcmi_xgs4_udf_reinit(unit);
        BCM_IF_ERROR_RETURN(rv);
    } else
#endif /* BCM_WARM_BOOT_SUPPORT */
    {
        rv = bcmi_xgs4_udf_hw_init(unit);
        BCM_IF_ERROR_RETURN(rv);

#if defined (BCM_WARM_BOOT_SUPPORT)
        rv = bcmi_xgs4_udf_wb_alloc(unit);
        BCM_IF_ERROR_RETURN(rv);
#endif /* BCM_WARM_BOOT_SUPPORT */

    }

    return BCM_E_NONE;
}

/* To sanitize udf_info/pkt_format_info input structures */
STATIC int
bcmi_xgs4_udf_sanitize_inputs(int unit, bcm_udf_t *udf_info,
                              bcm_udf_alloc_hints_t *hints,
                              bcm_udf_pkt_format_info_t *pkt_format)
{
#if defined (BCM_TOMAHAWK_SUPPORT)
        int port_count = 0, port = 0, first_pipe = BCMI_XGS4_UDF_INVALID_PIPE_NUM;
        soc_info_t  *si;
        si = &SOC_INFO(unit);
#endif

    if (udf_info != NULL) {

        /* start and width should be at byte boundary */
        if (((udf_info->start % 8) != 0) || ((udf_info->width % 8) != 0)) {
            return BCM_E_PARAM;
        }

        /* start and width should be less than max byte selection */
        if (BITS2BYTES(udf_info->start + udf_info->width) >
            UDF_CTRL(unit)->max_parse_bytes) {
            return BCM_E_PARAM;
        }

        /* flags in udf_info is currently not used; must be 0 */
        if (udf_info->flags != 0) {
            return BCM_E_PARAM;
        }

        /* check if layer is in known range */
        if ((udf_info->layer < bcmUdfLayerL2Header) ||
            (udf_info->layer >= bcmUdfLayerCount)) {
            return BCM_E_PARAM;
        }

        
        if (udf_info->layer == bcmUdfLayerUserPayload) {
            return BCM_E_PARAM;
        }

#if defined (BCM_TOMAHAWK_SUPPORT)
        /* get the pipe where the entry will be configured */
        if (soc_feature(unit, soc_feature_udf_multi_pipe_support) &&
           (bcmi_xgs4_udf_oper_mode[unit] == bcmUdfOperModePipeLocal)) {

            BCM_PBMP_COUNT(udf_info->ports, port_count);
            if (port_count == 0) {
                return BCM_E_PARAM;
            }

            SOC_PBMP_ITER(udf_info->ports, port) {
                if (first_pipe == BCMI_XGS4_UDF_INVALID_PIPE_NUM ) {
                    first_pipe =  si->port_pipe[port];
                } else if (first_pipe != si->port_pipe[port]) {
                    return BCM_E_PARAM;
                }
            }
        }
#endif
    }

    if (hints != NULL) {
        /* check if chip support flex hash */
        if ((hints->flags & BCM_UDF_CREATE_O_FLEXHASH) &&
            (!(UDF_CTRL(unit)->flags & BCMI_XGS4_UDF_CTRL_FLEXHASH))) {
            return BCM_E_PARAM;
        }
        /* check if chip support policer group */
        if ((hints->flags & BCM_UDF_CREATE_O_POLICER_GROUP) &&
            (!(UDF_CTRL(unit)->flags & BCMI_XGS4_UDF_CTRL_POLICER_GROUP))) {
            return BCM_E_PARAM;
        }
        /* check if chip support udf hash */
        if ((hints->flags & BCM_UDF_CREATE_O_UDFHASH) &&
            (!(UDF_CTRL(unit)->flags & BCMI_XGS4_UDF_CTRL_UDFHASH))) {
            return BCM_E_PARAM;
        }
        /* check create udf if withid is used together with replace*/
        if (((hints->flags & BCM_UDF_CREATE_O_WITHID) &&
            !(hints->flags & BCM_UDF_CREATE_O_REPLACE)) ||
            (!(hints->flags & BCM_UDF_CREATE_O_WITHID) &&
            (hints->flags & BCM_UDF_CREATE_O_REPLACE))) {
            return BCM_E_PARAM;
        }
        /* shared udf id should be valid */
        if ((hints->flags & BCM_UDF_CREATE_O_SHARED_HWID) &&
            ((hints->shared_udf < BCMI_XGS4_UDF_ID_MIN) ||
             (hints->shared_udf > BCMI_XGS4_UDF_ID_MAX))) {
            return BCM_E_PARAM;
        }
        /* check if chip supports Range Check */
        if ((hints->flags & BCM_UDF_CREATE_O_RANGE_CHECK) &&
           (!(UDF_CTRL(unit)->flags & BCMI_XGS4_UDF_CTRL_RANGE_CHECK))) {
            return BCM_E_PARAM;
        }
        /* check if chip support flex counter */
        if ((hints->flags & BCM_UDF_CREATE_O_FLEX_COUNTER) &&
            (!(UDF_CTRL(unit)->flags & BCMI_XGS4_UDF_CTRL_FLEX_COUNTER))) {
            return BCM_E_PARAM;
        }
    }

    if (pkt_format != NULL) {

        if ((pkt_format->prio < 0) || (pkt_format->prio > 0xffff)) {
            /* Soft restriction - prio maintained as 'short' integer */
            return BCM_E_PARAM;
        }
        if ((!(UDF_CTRL(unit)->flags & BCMI_XGS4_UDF_CTRL_TCAM_HIGIG) &&
             (pkt_format->higig > 0)) ||
             (!(UDF_CTRL(unit)->flags & BCMI_XGS4_UDF_CTRL_TCAM_VNTAG) &&
             (pkt_format->vntag > 0)) ||
             (!(UDF_CTRL(unit)->flags & BCMI_XGS4_UDF_CTRL_TCAM_ETAG) &&
             (pkt_format->etag > 0)) ||
             (!(UDF_CTRL(unit)->flags & BCMI_XGS4_UDF_CTRL_TCAM_ICNM) &&
             (pkt_format->icnm > 0)) ||
             (!(UDF_CTRL(unit)->flags & BCMI_XGS4_UDF_CTRL_TCAM_CNTAG) &&
             (pkt_format->cntag > 0)) ||
             (!(UDF_CTRL(unit)->flags & BCMI_XGS4_UDF_CTRL_TCAM_SUBPORT_TAG) &&
             (pkt_format->subport_tag > 0)) ||
             (!(UDF_CTRL(unit)->flags & BCMI_XGS4_UDF_CTRL_L4_DST_PORT) &&
             (pkt_format->l4_dst_port > 0)) ||
             (!(UDF_CTRL(unit)->flags & BCMI_XGS4_UDF_CTRL_OPAQUE_TAG_TYPE) &&
             (pkt_format->opaque_tag_type > 0)) ||
             (!(UDF_CTRL(unit)->flags & BCMI_XGS4_UDF_CTRL_INT_PKT) &&
             (pkt_format->int_pkt > 0)) ||
             (!(UDF_CTRL(unit)->flags & BCMI_XGS4_UDF_CTRL_SRC_PORT) &&
             (pkt_format->src_port > 0)) ||
             (!(UDF_CTRL(unit)->flags & BCMI_XGS4_UDF_CTRL_LB_PKT_TYPE) &&
             (pkt_format->lb_pkt_type > 0))) {
            return BCM_E_UNAVAIL;
        }
#if defined (BCM_TOMAHAWK_SUPPORT)
        if (UDF_CTRL(unit)->flags & BCMI_XGS4_UDF_PKT_FORMAT_CLASS_ID) {
            if ((pkt_format->class_id < 0) || (pkt_format->class_id > 255)) {
                return BCM_E_PARAM;
            }
        }
#endif
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_udf_create
 * Purpose:
 *      Creates a UDF object
 * Parameters:
 *      unit - (IN) Unit number.
 *      hints - (IN) Hints to UDF allocator
 *      udf_info - (IN) UDF structure
 *      udf_id - (IN/OUT) UDF ID
 * Returns:
 *      BCM_E_NONE          UDF created successfully.
 *      BCM_E_EXISTS        Entry already exists.
 *      BCM_E_FULL          UDF table full.
 *      BCM_E_UNIT          Invalid BCM Unit number.
 *      BCM_E_INIT          UDF module not initialized.
 *      BCM_E_UNAVAIL       Feature not supported.
 *      BCM_E_XXX           Standard error code.
 * Notes:
 */
int
bcmi_xgs4_udf_create(
    int unit,
    bcm_udf_alloc_hints_t *hints,
    bcm_udf_t *udf_info,
    bcm_udf_id_t *udf_id)
{
    int i;
    int gran;
    int max_chunks;
    int id_running_allocated = 0;
    int rv = BCM_E_NONE;
    bcmi_xgs4_udf_offset_info_t *offset_del = NULL;
    bcmi_xgs4_udf_offset_info_t *offset_info = NULL;
    bcmi_xgs4_udf_offset_entry_t *offset_entry_array;
    uint32 offset[MAX_UDF_OFFSET_CHUNKS] = {0};

    /* UDF Module Init check */
    UDF_INIT_CHECK(unit);

    /* NULL check; hints can be NULL */
    BCM_IF_NULL_RETURN_PARAM(udf_id);
    BCM_IF_NULL_RETURN_PARAM(udf_info);

    gran = BCMI_XGS4_UDF_CTRL_OFFSET_GRAN(unit);

    /* Sanitize input parameters */
    rv = bcmi_xgs4_udf_sanitize_inputs(unit, udf_info, hints, NULL);
    BCM_IF_ERROR_RETURN(rv);

    /* UDF Module Lock */
    UDF_LOCK(unit);

    if (UDF_CTRL(unit)->udf_used_by_module == UDF_USED_BY_FIELD_MODULE) {
        LOG_CLI((BSL_META("Udf resource is used by FIELD module. \n")));
        UDF_UNLOCK(unit);
        return BCM_E_RESOURCE;
    }

    max_chunks = BCMI_XGS4_UDF_CTRL_MAX_UDF_CHUNKS(unit);

    /* Check if similar entry already existing */
    if ((hints != NULL) && (hints->flags & BCM_UDF_CREATE_O_WITHID)) {

        UDF_ID_VALIDATE_UNLOCK(unit, *udf_id);

        /* Fetch the udf offset info;  get should be successful */
        rv = bcmi_xgs4_udf_offset_node_get(unit, *udf_id, &offset_info);

        if ((BCM_FAILURE(rv)) || (offset_info == NULL)) {
            UDF_UNLOCK(unit);
            return BCM_E_NOT_FOUND;
        }

        if ((hints->flags & BCM_UDF_CREATE_O_REPLACE)) {
            if (offset_info->num_pkt_formats >= 1) {
                /* UDF already in use */
                UDF_UNLOCK(unit);
                return BCM_E_CONFIG;
            }

            for (i = 0; i < max_chunks; i++) {
                SHR_BITCOPY_RANGE(&offset[i], 0, &(offset_info->byte_bmap),
                                  i*gran, gran);
            }

            /* unreserve the offset chunks */
            rv = bcmi_xgs4_udf_offset_unreserve(unit, max_chunks, (int *)offset,
                                                offset_info->associated_pipe);
            if (BCM_FAILURE(rv)) {
                UDF_UNLOCK(unit);
                return rv;
            }

            /* Delete the UDF entry and recreate one */
            rv = bcmi_xgs4_udf_offset_node_delete(unit, *udf_id, &offset_del);
            if (BCM_FAILURE(rv)) {
                UDF_UNLOCK(unit);
                return rv;
            }
        } else {
            UDF_UNLOCK(unit);
            return BCM_E_PARAM;
        }
    } else {
        /* Fresh udf creation */

        /* Allocate udf id */
        rv = bcmi_xgs4_udf_id_running_id_alloc(unit, bcmiUdfObjectUdf, udf_id);
        if (BCM_FAILURE(rv)) {
            UDF_UNLOCK(unit);
            return rv;
        } else {
            id_running_allocated = 1;
        }
    }

    /* Add udf info structure to linked list */
    rv = bcmi_xgs4_udf_offset_info_add(unit, udf_info, &offset_info);
    if ((BCM_FAILURE(rv)) || (offset_info == NULL)) {
        UDF_UNLOCK(unit);
        return BCM_E_MEMORY;
    }

    if ((UDF_CTRL(unit)->num_udfs + 1) == UDF_CTRL(unit)->max_udfs) {
        rv = BCM_E_RESOURCE;
    }
    UDF_IF_ERROR_CLEANUP(rv);

    offset_entry_array = UDF_CTRL(unit)->offset_entry_array;
#if defined (BCM_TOMAHAWK_SUPPORT)
    if (soc_feature(unit, soc_feature_udf_multi_pipe_support) &&
       (bcmi_xgs4_udf_oper_mode[unit] == bcmUdfOperModePipeLocal)) {
        offset_entry_array =
            UDF_CTRL(unit)->offset_entry_array_per_pipe[offset_info->associated_pipe];
    }
#endif

    /* For shared hwid, just copy the hw details */
    if ((hints != NULL) && (hints->flags & BCM_UDF_CREATE_O_SHARED_HWID)) {
        bcmi_xgs4_udf_offset_info_t *offset_shared;

        rv = bcmi_xgs4_udf_offset_node_get(unit, hints->shared_udf,
                                           &offset_shared);
        UDF_IF_ERROR_CLEANUP(rv);

        /* A shared udf should also should use the same width */
        if (offset_info->width != offset_shared->width) {
            rv = BCM_E_CONFIG;
        }

        /* Shared offset should 'start' at the same byte boundary
         * Otherwise the chunks allocated may not suffice the requirement
         */
        if ((offset_info->start % gran) != (offset_shared->start % gran)) {
            rv = BCM_E_CONFIG;
        }

        /* In Old devices with gran 4, if base offset is packet start
         * then extra check is need for layer also as bcmUdfLayerL2Header
         * have negative offset adjustment of 2 */
        if (gran == UDF_OFFSET_GRAN4) {
            if (offset_info->start == bcmUdfLayerL2Header) {
                if (offset_shared->start != bcmUdfLayerL2Header) {
                    rv = BCM_E_CONFIG;
                }
            } else {
                if(offset_shared->start == bcmUdfLayerL2Header) {
                    rv = BCM_E_CONFIG;
                }
            }
        }

        UDF_IF_ERROR_CLEANUP(rv);

        /* Since shared, copy the same chunk bitmap */
        offset_info->grp_id = offset_shared->grp_id;
        offset_info->hw_bmap = offset_shared->hw_bmap;
        offset_info->byte_bmap = offset_shared->byte_bmap;
        offset_info->byte_offset = offset_shared->byte_offset;

        offset_info->flags |= BCMI_XGS4_UDF_OFFSET_INFO_SHARED;
        offset_shared->flags |= BCMI_XGS4_UDF_OFFSET_INFO_SHARED;

        /* Mark the entry as shared */
        for (i = 0; i < max_chunks; i ++) {
            if (SHR_BITGET(&(offset_info->hw_bmap), i)) {
                offset_entry_array[i].grp_id = offset_info->grp_id;
                offset_entry_array[i].num_udfs += 1;
                offset_entry_array[i].flags |=
                    BCMI_XGS4_UDF_OFFSET_ENTRY_SHARED;
            }
        }
    } else {

        /* allocate udf chunks */
        rv = bcmi_xgs4_udf_offset_hw_alloc(unit, hints, offset_info);
        UDF_IF_ERROR_CLEANUP(rv);

    }

    /* update udf_id in the offset_info */
    offset_info->udf_id = *udf_id;

    if (hints != NULL) {
        if (hints->flags & BCM_UDF_CREATE_O_FLEXHASH) {
            offset_info->flags |= BCMI_XGS4_UDF_OFFSET_FLEXHASH;
        }
        if (hints->flags & BCM_UDF_CREATE_O_FIELD_INGRESS) {
            offset_info->flags |= BCMI_XGS4_UDF_OFFSET_IFP;
        }
        if (hints->flags & BCM_UDF_CREATE_O_FIELD_LOOKUP) {
            offset_info->flags |= BCMI_XGS4_UDF_OFFSET_VFP;
        }
        if (hints->flags & BCM_UDF_CREATE_O_POLICER_GROUP) {
            offset_info->flags |= BCMI_XGS4_UDF_OFFSET_POLICER;
        }
        if (hints->flags & BCM_UDF_CREATE_O_UDFHASH) {
            offset_info->flags |= BCMI_XGS4_UDF_OFFSET_UDFHASH;
        }
        if (hints->flags & BCM_UDF_CREATE_O_RANGE_CHECK) {
            offset_info->flags |= BCMI_XGS4_UDF_OFFSET_RANGE_CHECK;
        }
    }

    /* Increment num udfs */
    if ((hints == NULL) || !(hints->flags & BCM_UDF_CREATE_O_REPLACE)) {
        UDF_CTRL(unit)->num_udfs += 1;
    }

    UDF_CTRL(unit)->udf_used_by_module = UDF_USED_BY_UDF_MODULE;

cleanup:


    if (BCM_FAILURE(rv)) {

        if ((id_running_allocated == 1) &&
            (UDF_CTRL(unit)->udf_id_running != BCMI_XGS4_UDF_ID_MAX)) {
            /* Decrement the running udf id */
            UDF_CTRL(unit)->udf_id_running -= 1;
        }

        /* Delete the offset node */
        if (offset_info != NULL) {
            UDF_UNLINK_OFFSET_NODE(offset_info);
            sal_free(offset_info);
        }
    }

    /* Release UDF mutex */
    UDF_UNLOCK(unit);


    return rv;
}

/*
 * Function:
 *      bcm_udf_pkt_format_create
 * Purpose:
 *      Create a packet format entry
 * Parameters:
 *      unit - (IN) Unit number.
 *      options - (IN) API Options.
 *      pkt_format - (IN) UDF packet format info structure
 *      pkt_format_id - (OUT) Packet format ID
 * Returns:
 *      BCM_E_NONE          UDF packet format entry created
 *                          successfully.
 *      BCM_E_UNIT          Invalid BCM Unit number.
 *      BCM_E_INIT          UDF module not initialized.
 *      BCM_E_UNAVAIL       Feature not supported.
 *      BCM_E_XXX           Standard error code.
 * Notes:
 */
int
bcmi_xgs4_udf_pkt_format_create(
    int unit,
    bcm_udf_pkt_format_options_t options,
    bcm_udf_pkt_format_info_t *pkt_format_info,
    bcm_udf_pkt_format_id_t *pkt_format_id)
{
    int id_running_allocated = 0;
    bcmi_xgs4_udf_tcam_info_t *tcam_info_match = NULL;
    bcmi_xgs4_udf_tcam_info_t *tcam_info_local = NULL;

    int rv = BCM_E_NONE;

    /* UDF Module Init check */
    UDF_INIT_CHECK(unit);


    /* Input parameters check. */
    BCM_IF_NULL_RETURN_PARAM(pkt_format_id);
    BCM_IF_NULL_RETURN_PARAM(pkt_format_info);

    /* Sanitize input parameters */
    rv = bcmi_xgs4_udf_sanitize_inputs(unit, NULL, NULL, pkt_format_info);
    BCM_IF_ERROR_RETURN(rv);

    /* UDF Module Lock */
    UDF_LOCK(unit);

    /* Check if similar entry already existing */
    if (options & BCM_UDF_PKT_FORMAT_CREATE_O_WITHID) {

        UDF_PKT_FORMAT_ID_VALIDATE_UNLOCK(unit, *pkt_format_id);

        /* Fetch the tcam info;  get should be successful */
        rv = bcmi_xgs4_udf_tcam_node_get(unit, *pkt_format_id,
                                         &tcam_info_local);

        if (BCM_FAILURE(rv)) {
            UDF_UNLOCK(unit);
            return rv;
        }

        if ((options & BCM_UDF_PKT_FORMAT_CREATE_O_REPLACE)) {
            if (tcam_info_local->num_udfs >= 1) {
                /* UDF Pkt format entry already in use */
                UDF_UNLOCK(unit);
                return BCM_E_CONFIG;
            }
            /* Delete the UDF pkt format entry and recreate one */
            rv = bcmi_xgs4_udf_tcam_node_delete(unit, *pkt_format_id,
                                                &tcam_info_match);
            if (BCM_FAILURE(rv)) {
                UDF_UNLOCK(unit);
                return rv;
            }

            /* Free tcam node */
            sal_free(tcam_info_match);
        }
    } else {

        /* Allocate new id */
        rv = bcmi_xgs4_udf_id_running_id_alloc(unit, bcmiUdfObjectPktFormat,
                                               pkt_format_id);
        if (BCM_FAILURE(rv)) {
            UDF_UNLOCK(unit);
            return rv;
        } else {
            id_running_allocated = 1;
        }
    }

    /* Insert the packet format entry in the linked list */
    rv = bcmi_xgs4_udf_tcam_info_add(unit, pkt_format_info, &tcam_info_local);
    UDF_IF_ERROR_CLEANUP(rv);

    if ((UDF_CTRL(unit)->num_pkt_formats + 1) ==
         BCMI_XGS4_UDF_CTRL_MAX_UDF_ENTRIES(unit)) {
        rv = BCM_E_RESOURCE;
    }
    UDF_IF_ERROR_CLEANUP(rv);


    /*
     * The packet format matching hardware is a tcam and hence
     * there is no point in allowing to create more than one entry
     * of similar configuration. Search for a matching entry in
     * list of already installed objects and return error appropriately.
     */

    rv = bcmi_xgs4_udf_tcam_entry_match(unit,
                                        tcam_info_local, &tcam_info_match);
    if (BCM_SUCCESS(rv)) {
        if (tcam_info_match->priority == pkt_format_info->prio) {
            rv = BCM_E_EXISTS;
        } else {
            rv = BCM_E_RESOURCE;
        }
    } else {
        if (rv == BCM_E_NOT_FOUND) {
            rv = BCM_E_NONE;
        }

    }
    UDF_IF_ERROR_CLEANUP(rv);

    /* Increment num udfs */
    UDF_CTRL(unit)->num_pkt_formats += 1;

    /* update pkt_format_id in the tcam_info */
    tcam_info_local->pkt_format_id = *pkt_format_id;

cleanup:

    if (BCM_FAILURE(rv)) {
        if ((id_running_allocated) &&
            (UDF_CTRL(unit)->udf_pkt_format_id_running !=
             BCMI_XGS4_UDF_PKT_FORMAT_ID_MAX)) {
            /* Decrement the running pkt format id */
            UDF_CTRL(unit)->udf_pkt_format_id_running -= 1;
        }

        /* Delete the offset node */
        if (tcam_info_local != NULL) {
            UDF_UNLINK_TCAM_NODE(tcam_info_local);
            sal_free(tcam_info_local);
        }
    }

    /* Release UDF mutex */
    UDF_UNLOCK(unit);


    return rv;
}


/*
 * Function:
 *      bcm_udf_pkt_format_destroy
 * Purpose:
 *      Destroy existing packet format entry
 * Parameters:
 *      unit - (IN) Unit number.
 *      pkt_format_id - (IN) Packet format ID
 *      pkt_format - (IN) UDF packet format info structure
 * Returns:
 *      BCM_E_NONE          Destroy packet format entry.
 *      BCM_E_NOT_FOUND     Packet format ID does not exist.
 *      BCM_E_UNIT          Invalid BCM Unit number.
 *      BCM_E_INIT          UDF module not initialized.
 *      BCM_E_UNAVAIL       Feature not supported.
 *      BCM_E_XXX           Standard error code.
 * Notes:
 */
int
bcmi_xgs4_udf_pkt_format_destroy(int unit,
                                 bcm_udf_pkt_format_id_t pkt_format_id)
{
    int rv;
    bcmi_xgs4_udf_tcam_info_t *tcam_info = NULL;

    /* UDF Module Init check */
    UDF_INIT_CHECK(unit);

    /* UDF Module Lock */
    UDF_LOCK(unit);

    /* Retrieve the node indexed by pkt_format_id */
    rv = bcmi_xgs4_udf_tcam_node_get(unit, pkt_format_id, &tcam_info);
    if (BCM_FAILURE(rv)) {
        UDF_UNLOCK(unit);
        return rv;
    }

    /* Delete the software object */
    if (tcam_info->num_udfs >= 1) {
        UDF_UNLOCK(unit);
        return BCM_E_BUSY;
    } else {
        UDF_UNLINK_TCAM_NODE(tcam_info);

        /* Decrement num pkt_formats */
        UDF_CTRL(unit)->num_pkt_formats -= 1;

        /* Free the tcam node */
        sal_free(tcam_info);
    }

    /* Release UDF mutex */
    UDF_UNLOCK(unit);

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_udf_pkt_format_info_get
 * Purpose:
 *      Retrieve packet format info given the packet format Id
 * Parameters:
 *      unit - (IN) Unit number.
 *      pkt_format_id - (IN) Packet format ID
 *      pkt_format - (OUT) UDF packet format info structure
 * Returns:
 *      BCM_E_NONE          Packet format get successful.
 *      BCM_E_NOT_FOUND     Packet format entry does not exist.
 *      BCM_E_UNIT          Invalid BCM Unit number.
 *      BCM_E_INIT          UDF module not initialized.
 *      BCM_E_UNAVAIL       Feature not supported.
 *      BCM_E_XXX           Standard error code.
 * Notes:
 */
int
bcmi_xgs4_udf_pkt_format_info_get(
    int unit,
    bcm_udf_pkt_format_id_t pkt_format_id,
    bcm_udf_pkt_format_info_t *pkt_format_info)
{
    int rv;
    bcmi_xgs4_udf_tcam_info_t *tcam_info = NULL;

    /* UDF Module Init check */
    UDF_INIT_CHECK(unit);

    BCM_IF_NULL_RETURN_PARAM(pkt_format_info);

    /* UDF Module Lock */
    UDF_LOCK(unit);

    /* Retrieve the node indexed by pkt_format_id */
    rv = bcmi_xgs4_udf_tcam_node_get(unit, pkt_format_id, &tcam_info);
    if (BCM_FAILURE(rv)) {
        UDF_UNLOCK(unit);
        return rv;
    }

    /* Fetch the tcam entry info and convert to user-readable form */
    rv = bcmi_xgs4_udf_tcam_entry_parse(unit,
             (uint32 *)&(tcam_info->hw_buf), pkt_format_info);
    if (BCM_FAILURE(rv)) {
        UDF_UNLOCK(unit);
        return rv;
    }

    pkt_format_info->prio = tcam_info->priority;

#if defined (BCM_TOMAHAWK_SUPPORT)
    if (UDF_CTRL(unit)->flags & BCMI_XGS4_UDF_PKT_FORMAT_CLASS_ID) {
        pkt_format_info->class_id = tcam_info->class_id;
    }
#endif

    /* Release UDF mutex */
    UDF_UNLOCK(unit);


    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_udf_pkt_format_add
 * Purpose:
 *      Adds packet format entry to UDF object
 * Parameters:
 *      unit - (IN) Unit number.
 *      udf_id - (IN) UDF ID
 *      pkt_format_id - (IN) Packet format ID
 * Returns:
 *      BCM_E_NONE          Packet format entry added to UDF
 *                          successfully.
 *      BCM_E_NOT_FOUND     UDF Id or packet format entry does not
 *                          exist.
 *      BCM_E_UNIT          Invalid BCM Unit number.
 *      BCM_E_INIT          UDF module not initialized.
 *      BCM_E_UNAVAIL       Feature not supported.
 *      BCM_E_XXX           Standard error code.
 * Notes:
 */
int
bcmi_xgs4_udf_pkt_format_add(
    int unit,
    bcm_udf_id_t udf_id,
    bcm_udf_pkt_format_id_t pkt_format_id)
{
    int rv;
    int idx;
    int base = 0;
    int offset = 0;
    int num_udfs = 0;
    uint32 offset_bmap;
    bcmi_xgs4_udf_offset_entry_t *offset_entry;
    bcmi_xgs4_udf_offset_info_t *offset_info = NULL;
    bcmi_xgs4_udf_tcam_info_t *tcam_info = NULL;

    /* UDF Module Init check */
    UDF_INIT_CHECK(unit);

    /* Sanitize input params */
    UDF_ID_VALIDATE(udf_id);
    UDF_PKT_FORMAT_ID_VALIDATE(pkt_format_id);

    /* UDF Module Lock */
    UDF_LOCK(unit);

    /* Retrieve the node indexed by pkt_format_id */
    rv = bcmi_xgs4_udf_tcam_node_get(unit, pkt_format_id, &tcam_info);
    if (BCM_FAILURE(rv)) {
        UDF_UNLOCK(unit);
        return rv;
    }

    /* Read UDF info */
    rv = bcmi_xgs4_udf_offset_node_get(unit, udf_id, &offset_info);
    if (BCM_FAILURE(rv)) {
        UDF_UNLOCK(unit);
        return rv;
    }

    /* Convert udf layer+start to udf_offset info */
    rv = bcmi_xgs4_udf_layer_to_offset_base(unit, offset_info,
                                            tcam_info, &base, &offset);
    if (BCM_FAILURE(rv)) {
        UDF_UNLOCK(unit);
        return rv;
    }

    if ((offset_info->num_pkt_formats + 1) >= MAX_UDF_TCAM_ENTRIES) {
        UDF_UNLOCK(unit);
        return  BCM_E_RESOURCE;
    }

#ifdef BCM_TOMAHAWK_SUPPORT
    if (soc_feature(unit, soc_feature_udf_multi_pipe_support) &&
       (bcmi_xgs4_udf_oper_mode[unit] == bcmUdfOperModePipeLocal)) {
        if (offset_info->associated_pipe >= SOC_MAX_NUM_PIPES) {
            UDF_UNLOCK(unit);
            return BCM_E_PARAM;
        }

        tcam_info->associated_pipe = offset_info->associated_pipe;
        tcam_info->associated_pipes[offset_info->associated_pipe] =
            offset_info->associated_pipe;

        num_udfs = tcam_info->num_udfs_per_pipe[tcam_info->associated_pipe];
        offset_bmap = tcam_info->offset_bmap_per_pipe[tcam_info->associated_pipe];
    } else
#endif
    {
        num_udfs = tcam_info->num_udfs;
        offset_bmap = tcam_info->offset_bmap;
    }

    if (offset_bmap & offset_info->hw_bmap) {
        /* Packet format object is already using the bmap */
        UDF_UNLOCK(unit);
        return BCM_E_EXISTS;
    }

    if ((num_udfs + 1) > MAX_UDF_OFFSET_CHUNKS) {
        UDF_UNLOCK(unit);
        return BCM_E_RESOURCE;
    }

    /* Packet format is associated with the first UDF, install it */
    if (num_udfs == 0) {
        rv = bcmi_xgs4_udf_tcam_entry_insert(unit, tcam_info);
        if (BCM_FAILURE(rv)) {
            UDF_UNLOCK(unit);
            return rv;
        }
    }

#ifdef BCM_TOMAHAWK_SUPPORT
    if (soc_feature(unit, soc_feature_udf_multi_pipe_support) &&
        (bcmi_xgs4_udf_oper_mode[unit] == bcmUdfOperModePipeLocal)) {
        tcam_info->hw_idx =
            tcam_info->hw_idx_per_pipe[tcam_info->associated_pipe];
    }
#endif

    /* Add the udf offset info to the UDF_OFFSET entry */
    rv = bcmi_xgs4_udf_offset_install(unit, tcam_info->hw_idx,
                                      offset_info->hw_bmap, base, offset,
                                      tcam_info->associated_pipe,
                                      tcam_info->class_id,
                                      offset_info->flags);

    if (BCM_FAILURE(rv)) {
        UDF_UNLOCK(unit);
        return rv;
    }

    /* set the offset_bmap */
    tcam_info->num_udfs++;
    offset_info->num_pkt_formats++;
    tcam_info->offset_info_list[offset_info->grp_id] = offset_info;

    for (idx = 0; idx < MAX_UDF_ID_LIST; idx++) {
        if (tcam_info->udf_id_list[idx] == 0) {
            tcam_info->udf_id_list[idx] = udf_id;
            break;
        }
    }

#ifdef BCM_TOMAHAWK_SUPPORT
    if (soc_feature(unit, soc_feature_udf_multi_pipe_support) &&
        (bcmi_xgs4_udf_oper_mode[unit] == bcmUdfOperModePipeLocal)) {
        tcam_info->num_udfs_per_pipe[tcam_info->associated_pipe]++;
        tcam_info->offset_bmap_per_pipe[tcam_info->associated_pipe] |= offset_info->hw_bmap;
        offset_entry = &(UDF_CTRL(unit)->offset_entry_array_per_pipe[offset_info->associated_pipe][(offset_info->grp_id)]);
    } else
#endif
    {
        tcam_info->offset_bmap |= (offset_info->hw_bmap);
        offset_entry = &(UDF_CTRL(unit)->offset_entry_array[(offset_info->grp_id)]);
    }

    offset_entry->num_pkt_formats++;

    /* Release UDF mutex */
    UDF_UNLOCK(unit);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_udf_pkt_format_delete
 * Purpose:
 *      Deletes packet format spec associated with the UDF
 * Parameters:
 *      unit - (IN) Unit number.
 *      udf_id - (IN) UDF ID
 *      pkt_format_id - (IN) Packet format ID
 * Returns:
 *      BCM_E_NONE          Packet format configuration successfully
 *                          deleted from UDF.
 *      BCM_E_NOT_FOUND     UDF Id or packet format entry does not
 *                          exist.
 *      BCM_E_UNIT          Invalid BCM Unit number.
 *      BCM_E_INIT          UDF module not initialized.
 *      BCM_E_UNAVAIL       Feature not supported.
 *      BCM_E_XXX           Standard error code.
 * Notes:
 */
int
bcmi_xgs4_udf_pkt_format_delete(
    int unit,
    bcm_udf_id_t udf_id,
    bcm_udf_pkt_format_id_t pkt_format_id)
{
    int rv;
    int idx;
    int hw_idx;
    int *num_udfs;
    uint32 *offset_bmap;
    soc_mem_t tcam_mem;
    bcmi_xgs4_udf_tcam_entry_t *tcam_entry;
    bcmi_xgs4_udf_offset_entry_t *offset_entry;
    bcmi_xgs4_udf_offset_info_t *offset_info = NULL;
    bcmi_xgs4_udf_tcam_info_t *tcam_info = NULL;

#if defined (BCM_TOMAHAWK_SUPPORT)
    int pipe_num = BCMI_XGS4_UDF_INVALID_PIPE_NUM;
#endif

    /* UDF Module Init check */
    UDF_INIT_CHECK(unit);

    /* Sanitize input params */
    UDF_ID_VALIDATE(udf_id);
    UDF_PKT_FORMAT_ID_VALIDATE(pkt_format_id);

    /* UDF Module Lock */
    UDF_LOCK(unit);

    /* Retrieve the node indexed by pkt_format_id */
    rv = bcmi_xgs4_udf_tcam_node_get(unit, pkt_format_id, &tcam_info);
    if (BCM_FAILURE(rv)) {
        UDF_UNLOCK(unit);
        return rv;
    }

    /* Read UDF info */
    rv = bcmi_xgs4_udf_offset_node_get(unit, udf_id, &offset_info);
    if (BCM_FAILURE(rv)) {
        UDF_UNLOCK(unit);
        return rv;
    }

    /* Either of the entry is not installed to the hardware yet */
    if (offset_info->num_pkt_formats == 0) {
        UDF_UNLOCK(unit);
        return BCM_E_CONFIG;
    }

#if defined (BCM_TOMAHAWK_SUPPORT)
    if (soc_feature(unit, soc_feature_udf_multi_pipe_support) &&
       (bcmi_xgs4_udf_oper_mode[unit] == bcmUdfOperModePipeLocal)) {
        if (offset_info->associated_pipe >= SOC_MAX_NUM_PIPES) {
            UDF_UNLOCK(unit);
            return BCM_E_PARAM;
        }

        pipe_num = tcam_info->associated_pipes[offset_info->associated_pipe];
        if (pipe_num == BCMI_XGS4_UDF_INVALID_PIPE_NUM) {
            UDF_UNLOCK(unit);
            return BCM_E_PARAM;
        }

        hw_idx = tcam_info->hw_idx_per_pipe[pipe_num];
        num_udfs = &(tcam_info->num_udfs_per_pipe[pipe_num]);
        offset_bmap = &(tcam_info->offset_bmap_per_pipe[pipe_num]);
    } else
#endif
    {
        hw_idx = tcam_info->hw_idx;
        num_udfs = &(tcam_info->num_udfs);
        offset_bmap = &(tcam_info->offset_bmap);
    }

    /* Unrelated packet format id and udf id */
    if (!(*offset_bmap & offset_info->hw_bmap)) {
        UDF_UNLOCK(unit);
        return BCM_E_CONFIG;
    }

    if (*num_udfs == 0) {
        UDF_UNLOCK(unit);
        return BCM_E_CONFIG;
    }

#if defined (BCM_TOMAHAWK_SUPPORT)
    if (soc_feature(unit, soc_feature_udf_multi_pipe_support) &&
       (bcmi_xgs4_udf_oper_mode[unit] == bcmUdfOperModePipeLocal)) {
        tcam_mem = SOC_MEM_UNIQUE_ACC(unit, UDF_CTRL(unit)->tcam_mem)[pipe_num];
        tcam_entry = &(UDF_CTRL(unit)->tcam_entry_array_per_pipe[pipe_num][hw_idx]);
        offset_entry = &(UDF_CTRL(unit)->offset_entry_array_per_pipe[pipe_num][offset_info->grp_id]);
        tcam_info->num_udfs--;
    } else
#endif
    {
        tcam_mem   = UDF_CTRL(unit)->tcam_mem;
        tcam_entry = &(UDF_CTRL(unit)->tcam_entry_array[hw_idx]);
        offset_entry = &(UDF_CTRL(unit)->offset_entry_array[(offset_info->grp_id)]);
    }

    *num_udfs = *num_udfs - 1;
    offset_info->num_pkt_formats -= 1;
    offset_entry->num_pkt_formats -= 1;
    tcam_info->offset_info_list[offset_info->grp_id] = NULL;

    for (idx = 0; idx < MAX_UDF_ID_LIST; idx++) {
        if (tcam_info->udf_id_list[idx] == udf_id) {
            tcam_info->udf_id_list[idx] = 0;
            break;
        }
    }

    /* Packet format is dissociated with the first UDF, uninstall it */
    if (*num_udfs == 0) {

        /* Install the entry into hardware */
        rv = soc_mem_write(unit, tcam_mem,
                           MEM_BLOCK_ALL, hw_idx,
                           soc_mem_entry_null(unit, UDF_CTRL(unit)->tcam_mem));

        if (SOC_FAILURE(rv)) {
            UDF_UNLOCK(unit);
            return rv;
        }

        tcam_entry->valid = 0;
        tcam_entry->tcam_info = NULL;
    }

    /* clear the offset_bmap */
    *offset_bmap &= ~(offset_info->hw_bmap);

    /* Remove the udf offset info to the UDF_OFFSET entry */
    rv = bcmi_xgs4_udf_offset_uninstall(unit, hw_idx,
                                        offset_info->hw_bmap,
                                        offset_info->associated_pipe);
    if (BCM_FAILURE(rv)) {
        UDF_UNLOCK(unit);
        return rv;
    }

    /* Release UDF mutex */
    UDF_UNLOCK(unit);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_udf_pkt_format_get_all
 * Purpose:
 *      Retrieves the user defined format specification configuration
 *      from UDF
 * Parameters:
 *      unit - (IN) Unit number.
 *      udf_id - (IN) UDF ID
 *      max - (IN) Max Packet formats attached to a UDF object
 *      pkt_format_id_list - (OUT) List of packet format entries added to udf id
 *      actual - (OUT) Actual number of Packet formats retrieved
 * Returns:
 *      BCM_E_NONE          Success.
 *      BCM_E_NOT_FOUND     Either the UDF or packet format entry does
 *                          not exist.
 *      BCM_E_UNIT          Invalid BCM Unit number.
 *      BCM_E_INIT          UDF module not initialized.
 *      BCM_E_UNAVAIL       Feature not supported.
 *      BCM_E_XXX           Standard error code.
 * Notes:
 */
int
bcmi_xgs4_udf_pkt_format_get_all(
    int unit,
    bcm_udf_id_t udf_id,
    int max,
    bcm_udf_pkt_format_id_t *pkt_format_id_list,
    int *actual)
{
    int i;
    int rv;
    bcmi_xgs4_udf_offset_info_t *offset_info = NULL;
    bcmi_xgs4_udf_tcam_info_t *tcam_info = NULL;

    /* UDF Module Init check */
    UDF_INIT_CHECK(unit);

    BCM_IF_NULL_RETURN_PARAM(actual);

    /* Sanitize input params */
    UDF_ID_VALIDATE(udf_id);

    /* UDF Module Lock */
    UDF_LOCK(unit);

    /* Read UDF info */
    rv = bcmi_xgs4_udf_offset_node_get(unit, udf_id, &offset_info);
    if (BCM_FAILURE(rv)) {
        UDF_UNLOCK(unit);
        return rv;
    }

    if ((max == 0) || (pkt_format_id_list == NULL)) {
        *actual = offset_info->num_pkt_formats;
        UDF_UNLOCK(unit);
        return BCM_E_NONE;
    }

    *actual = 0;
    tcam_info = UDF_CTRL(unit)->tcam_info_head;
    while (tcam_info) {
        for (i = 0; i < MAX_UDF_ID_LIST; i++) {
            if (tcam_info->udf_id_list[i] == udf_id) {
                if (*actual < max) {
                    pkt_format_id_list[*actual] = tcam_info->pkt_format_id;
                }

                (*actual) += 1;
                break;
            }
        }

        tcam_info = tcam_info->next;
    }

    /* Release UDF mutex */
    UDF_UNLOCK(unit);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_udf_pkt_format_delete_all
 * Purpose:
 *      Deletes all packet format specs associated with the UDF
 * Parameters:
 *      unit - (IN) Unit number.
 *      udf_id - (IN) UDF ID
 * Returns:
 *      BCM_E_NONE          Deletes all packet formats associated with
 *                          the UDF.
 *      BCM_E_NOT_FOUND     UDF Id does not exist.
 *      BCM_E_UNIT          Invalid BCM Unit number.
 *      BCM_E_INIT          UDF module not initialized.
 *      BCM_E_UNAVAIL       Feature not supported.
 *      BCM_E_XXX           Standard error code.
 * Notes:
 */
int
bcmi_xgs4_udf_pkt_format_delete_all(
    int unit, bcm_udf_id_t udf_id)
{
    int i;
    int rv;
    bcmi_xgs4_udf_tcam_info_t *tcam_info = NULL;

    /* UDF Module Init check */
    UDF_INIT_CHECK(unit);

    /* Sanitize input params */
    UDF_ID_VALIDATE(udf_id);

    /* UDF Module Lock */
    UDF_LOCK(unit);

    tcam_info = UDF_CTRL(unit)->tcam_info_head;
    while (tcam_info) {
        for (i = 0; i < MAX_UDF_ID_LIST; i++) {
            if (tcam_info->udf_id_list[i] == udf_id) {
                rv = bcmi_xgs4_udf_pkt_format_delete(unit, udf_id,
                                                     tcam_info->pkt_format_id);

                if (BCM_FAILURE(rv)) {
                    UDF_UNLOCK(unit);
                    return rv;
                }
                break;
            }
        }

        tcam_info = tcam_info->next;
    }

    /* Release UDF mutex */
    UDF_UNLOCK(unit);

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_udf_pkt_format_get
 * Purpose:
 *      Fetches all UDFs that share the common packet format entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      pkt_format_id - (IN) Packet format ID.
 *      max - (IN) Max number of UDF IDs
 *      udf_id_list - (OUT) List of UDF IDs
 *      actual - (OUT) Actual udfs retrieved
 * Returns:
 *      BCM_E_NONE          Success.
 *      BCM_E_UNIT          Invalid BCM Unit number.
 *      BCM_E_INIT          UDF module not initialized.
 *      BCM_E_UNAVAIL       Feature not supported.
 *      BCM_E_XXX           Standard error code.
 * Notes:
 */
int
bcmi_xgs4_udf_pkt_format_get(
    int unit,
    bcm_udf_pkt_format_id_t pkt_format_id,
    int max,
    bcm_udf_id_t *udf_id_list,
    int *actual)
{
    int i;
    int rv;
    uint16 udf_id;
    bcmi_xgs4_udf_tcam_info_t *tcam_info = NULL;

    /* UDF Module Init check */
    UDF_INIT_CHECK(unit);

    BCM_IF_NULL_RETURN_PARAM(actual);

    /* Sanitize input params */
    UDF_PKT_FORMAT_ID_VALIDATE(pkt_format_id);

    /* UDF Module Lock */
    UDF_LOCK(unit);


    /* Read UDF info */
    rv = bcmi_xgs4_udf_tcam_node_get(unit, pkt_format_id, &tcam_info);
    if (BCM_FAILURE(rv)) {
        UDF_UNLOCK(unit);
        return rv;
    }

    if ((udf_id_list == NULL) || (max == 0)) {
        *actual = tcam_info->num_udfs;
        UDF_UNLOCK(unit);
        return rv;
    }

    *actual = 0;
    for (i = 0; i < MAX_UDF_ID_LIST; i++) {
        udf_id = tcam_info->udf_id_list[i];
        if (udf_id > 0) {
            if (*actual < max) {
                udf_id_list[*actual] = udf_id;
            }

            (*actual) += 1;
        }
    }

    /* Release UDF mutex */
    UDF_UNLOCK(unit);

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcmi_xgs4_udf_hash_config_add
 * Purpose:
 *      Add UDF id into hashing list and configure it
 * Parameters:
 *      unit            (IN) BCM unit number
 *      options         (IN) See BCM_UDF_HASH_ADD_O_xxx.
 *                           Currently not supported,
 *                           reserved for future usage.
 *      config          (IN) UDF hashing configuration info
 * Returns:
 *      BCM_E_NONE      UDF hashing added successfully
 *      BCM_E_XXX       Standard error code
 * Notes:
 */
int
bcmi_xgs4_udf_hash_config_add(
    int unit,
    uint32 options,
    bcm_udf_hash_config_t *config)
{
    int rv;
    bcmi_xgs4_udf_offset_info_t *offset_info = NULL;

    /* UDF Module Init check */
    UDF_INIT_CHECK(unit);

    /* UDF Module Lock */
    UDF_LOCK(unit);

    if ((config == NULL)) {
        UDF_UNLOCK(unit);
        return BCM_E_PARAM;
    }

    rv = bcmi_xgs4_udf_offset_node_get(unit, config->udf_id, &offset_info);
    if (BCM_FAILURE(rv)) {
        UDF_UNLOCK(unit);
        return BCM_E_PARAM;
    }

    rv = bcmi_xgs4_udf_hash_config_set(unit, config, offset_info);
    if (BCM_FAILURE(rv)) {
        UDF_UNLOCK(unit);
        return rv;
    }

    /* Set flag to indicate the object is in the list that
       participates in hash calculation */
    offset_info->flags |= BCMI_XGS4_UDF_OFFSET_UDFHASH_CONFIG;

    /* Release UDF mutex */
    UDF_UNLOCK(unit);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs4_udf_hash_config_delete
 * Purpose:
 *      Delete UDF id from hashing list
 * Parameters:
 *      unit            (IN) BCM unit number
 *      config          (IN) UDF hashing configuration info
 * Returns:
 *      BCM_E_NONE      UDF hashing added successfully
 *      BCM_E_XXX       Standard error code
 * Notes:
 */
int
bcmi_xgs4_udf_hash_config_delete(
    int unit,
    bcm_udf_hash_config_t *config)
{
    int rv;
    bcmi_xgs4_udf_offset_info_t *offset_info = NULL;

    /* UDF Module Init check */
    UDF_INIT_CHECK(unit);

    /* UDF Module Lock */
    UDF_LOCK(unit);

    if ((config == NULL)) {
        return BCM_E_PARAM;
        UDF_UNLOCK(unit);
    }

    rv = bcmi_xgs4_udf_offset_node_get(unit, config->udf_id, &offset_info);
    if (BCM_FAILURE(rv)) {
        UDF_UNLOCK(unit);
        return BCM_E_BADID;
    }

    /* set mask to zero */
    sal_memset(config->hash_mask, 0, sizeof(config->hash_mask));
    config->mask_length = offset_info->width;
    rv = bcmi_xgs4_udf_hash_config_set(unit, config, offset_info);
    if (BCM_FAILURE(rv)) {
        UDF_UNLOCK(unit);
        return rv;
    }

    /* Clear flag */
    offset_info->flags &= ~BCMI_XGS4_UDF_OFFSET_UDFHASH_CONFIG;

    /* Release UDF mutex */
    UDF_UNLOCK(unit);

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcmi_xgs4_udf_hash_config_delete_all
 * Purpose:
 *      Delete all UDF id from hashing list
 * Parameters:
 *      unit            (IN) BCM unit number
 * Returns:
 *      BCM_E_NONE      UDF hashing added successfully
 *      BCM_E_XXX       Standard error code
 * Notes:
 */
int
bcmi_xgs4_udf_hash_config_delete_all(
    int unit)
{
    bcmi_xgs4_udf_offset_info_t *tmp;
    bcm_udf_hash_config_t config;
    int rv;

    /* UDF Module Init check */
    UDF_INIT_CHECK(unit);

    tmp = UDF_CTRL(unit)->offset_info_head;

    while (tmp != NULL) {
        if (tmp->flags & BCMI_XGS4_UDF_OFFSET_UDFHASH_CONFIG) {
            /* the udf id is in the list, set mask to zero */
            sal_memset((&config)->hash_mask, 0, sizeof((&config)->hash_mask));
            (&config)->mask_length = tmp->width;
            (&config)->udf_id = tmp->udf_id;
            rv = bcmi_xgs4_udf_hash_config_set(unit, &config, tmp);
            if (BCM_FAILURE(rv)) {
                UDF_UNLOCK(unit);
                return rv;
            }

            /* Clear flag */
            tmp->flags &= ~BCMI_XGS4_UDF_OFFSET_UDFHASH_CONFIG;
        }

        tmp = tmp->next;
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcmi_xgs4_udf_hash_config_set
 * Purpose:
 *      Set udf hash configuration to hw
 * Parameters:
 *      unit            (IN) BCM unit number
 *      config          (IN) UDF hashing configuration info
 *      offset_info     (IN) Offset info node in offset link
 * Returns:
 *      BCM_E_NONE      UDF hashing added successfully
 *      BCM_E_BADID     UDF id does not exist or is not for UDF hashing usage
 *      BCM_E_XXX       Standard error code
 * Notes:
 */
STATIC int
bcmi_xgs4_udf_hash_config_set(
    int unit,
    bcm_udf_hash_config_t *config,
    bcmi_xgs4_udf_offset_info_t *offset_info)
{
    int i, j, rv;
    bcmi_xgs4_udf_offset_info_t *offset_match = NULL;
    bcmi_xgs4_udf_tcam_info_t *tcam_info = NULL;
    soc_mem_t mem;
    fp_udf_offset_entry_t offset_buf;
    uint8 udf_hash_1, udf_hash_2;
    uint32 mask;
    uint16 mask_1, mask_2, mask_en_1, mask_en_2;
    int byte_map[] = {13, 12, 15, 14};

    if ((config == NULL)
        || (config->mask_length > BCM_UDF_HASH_MAX_LENGTH)) {
        return BCM_E_PARAM;
    }

    udf_hash_1 = UDF_CTRL(unit)->udf_hash_1;
    udf_hash_2 = UDF_CTRL(unit)->udf_hash_2;
    if (!(offset_info->flags & BCMI_XGS4_UDF_OFFSET_UDFHASH) ||
        !(offset_info->hw_bmap & ((1 << udf_hash_1) | (1 << udf_hash_2))) ||
        (offset_info->width != config->mask_length)) {
        return BCM_E_PARAM;
    }

    mask = 0;
    /* mask[0]~[3] reflect to the order in byte_map */
    for (i = 0, j = 0; i < BCM_UDF_HASH_MAX_LENGTH; i++) {
        if (offset_info->byte_bmap & (1 << byte_map[i])) {
            mask |= (config->hash_mask[j++] << (8 * (3 - i)));
        }
    }
    mask_1 = (mask & 0xFFFF0000) >> 16;
    mask_2 = mask & 0x0000FFFF;
    mask_en_1 = (offset_info->hw_bmap & (1 << udf_hash_1)) ? 1 : 0;
    mask_en_2 = (offset_info->hw_bmap & (1 << udf_hash_2)) ? 1 : 0;

    tcam_info = UDF_CTRL(unit)->tcam_info_head;
    mem = UDF_CTRL(unit)->offset_mem;

    while (tcam_info != NULL) {
        for (i = 0; i < BCMI_XGS4_UDF_CTRL_MAX_UDF_CHUNKS(unit); i++) {
            offset_match = tcam_info->offset_info_list[i];
            /* Set same mask to all entries related to the UDF id */
            if (offset_info == offset_match) {
                rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                    tcam_info->hw_idx, &offset_buf);
                if (BCM_FAILURE(rv)) {
                    return rv;
                }

                if (mask_en_1) {
                    soc_mem_field32_set(unit, mem, &offset_buf,
                        HASH_MASK_1f, mask_1);
                }
                if (mask_en_2) {
                    soc_mem_field32_set(unit, mem, &offset_buf,
                        HASH_MASK_2f, mask_2);
                }

                rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                    tcam_info->hw_idx, &offset_buf);
                if (BCM_FAILURE(rv)) {
                    return rv;
                }
                break;
            }
        }

        tcam_info = tcam_info->next;
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcmi_xgs4_udf_hash_config_get
 * Purpose:
 *      Get UDF hashing configuration of a certain id
 * Parameters:
 *      unit            (IN) BCM unit number
 *      config          (INOUT) UDF hashing configuration info
 * Returns:
 *      BCM_E_NONE      UDF hashing added successfully
 *      BCM_E_NOT_FOUND Requested UDF hashing info does not exist
 *      BCM_E_XXX       Standard error code
 * Notes:
 */
int
bcmi_xgs4_udf_hash_config_get(
    int unit,
    bcm_udf_hash_config_t *config)
{
    int i, j, found = 0, rv;
    bcmi_xgs4_udf_offset_info_t *offset_info = NULL;
    bcmi_xgs4_udf_offset_info_t *offset_match = NULL;
    bcmi_xgs4_udf_tcam_info_t *tcam_info = NULL;
    soc_mem_t mem;
    fp_udf_offset_entry_t offset_buf;
    uint8 udf_hash_1, udf_hash_2;
    uint32 mask, mask_1 = 0, mask_2 = 0;
    int byte_map[] = {13, 12, 15, 14};

    /* UDF Module Init check */
    UDF_INIT_CHECK(unit);

    /* UDF Module Lock */
    UDF_LOCK(unit);

    if ((config == NULL)) {
        UDF_UNLOCK(unit);
        return BCM_E_PARAM;
    }

    rv = bcmi_xgs4_udf_offset_node_get(unit, config->udf_id, &offset_info);
    if (BCM_FAILURE(rv)) {
        UDF_UNLOCK(unit);
        return rv;
    }

    udf_hash_1 = UDF_CTRL(unit)->udf_hash_1;
    udf_hash_2 = UDF_CTRL(unit)->udf_hash_2;
    if (!(offset_info->flags & BCMI_XGS4_UDF_OFFSET_UDFHASH) ||
        !(offset_info->flags & BCMI_XGS4_UDF_OFFSET_UDFHASH_CONFIG) ||
        !(offset_info->hw_bmap & ((1 << udf_hash_1) | (1 << udf_hash_2)))) {
        UDF_UNLOCK(unit);
        return BCM_E_PARAM;
    }

    mem = UDF_CTRL(unit)->offset_mem;

    tcam_info = UDF_CTRL(unit)->tcam_info_head;

    while (tcam_info != NULL) {
        /* Get the first entry related to the UDF id */
        for (i = 0; i < BCMI_XGS4_UDF_CTRL_MAX_UDF_CHUNKS(unit); i++) {
            offset_match = tcam_info->offset_info_list[i];
            if (offset_info == offset_match) {
                rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                    tcam_info->hw_idx, &offset_buf);
                if (BCM_FAILURE(rv)) {
                    UDF_UNLOCK(unit);
                    return rv;
                }

                mask_1 = soc_mem_field32_get(unit, mem, &offset_buf,
                    HASH_MASK_1f);
                mask_2 = soc_mem_field32_get(unit, mem, &offset_buf,
                    HASH_MASK_2f);

                found = 1;
                break;
            }
        }

        if (found) {
            break;
        }
        tcam_info = tcam_info->next;
    }

    mask = (mask_1 << 16) | (mask_2 & 0xFFFF);

    /* mask[0]~[3] reflect to the order in byte_bmap */
    for (i = 0, j = 0; i < BCM_UDF_HASH_MAX_LENGTH; i++) {
        if (offset_info->byte_bmap & (1 << byte_map[i])) {
            config->hash_mask[j++] = (mask >> (8 * (3 - i))) & 0xFF;
        }
    }
    config->mask_length = offset_info->width;
    config->flags = 0;

    /* Release UDF mutex */
    UDF_UNLOCK(unit);

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcmi_xgs4_udf_hash_config_get_all
 * Purpose:
 *      Get all added UDF ids from list
 * Parameters:
 *      unit                  (IN) BCM unit number
 *      max                   (IN) Max count of UDF ids in the list.
 *      udf_hash_config_list  (OUT) List of UDF hashing configuration
 *      actual                (OUT) Actual count of UDF ids
 * Returns:
 *      BCM_E_NONE      UDF hashing added successfully
 *      BCM_E_XXX       Standard error code
 * Notes:
 */
int
bcmi_xgs4_udf_hash_config_get_all(
    int unit,
    int max,
    bcm_udf_hash_config_t *udf_hash_config_list,
    int *actual)
{
    bcmi_xgs4_udf_offset_info_t *tmp;
    bcm_udf_hash_config_t config;
    int rv;

    /* UDF Module Init check */
    UDF_INIT_CHECK(unit);

    BCM_IF_NULL_RETURN_PARAM(actual);

    /* UDF Module Lock */
    UDF_LOCK(unit);

    *actual = 0;
    tmp = UDF_CTRL(unit)->offset_info_head;

    while (tmp != NULL) {
        /* the udf id is in the list, get its config */
        if (tmp->flags & BCMI_XGS4_UDF_OFFSET_UDFHASH_CONFIG) {
            if ((*actual < max) && (udf_hash_config_list != NULL)) {
                (&config)->udf_id = tmp->udf_id;
                rv = bcmi_xgs4_udf_hash_config_get(unit, &config);
                if (BCM_FAILURE(rv)) {
                    UDF_UNLOCK(unit);
                    return rv;
                }
                sal_memcpy((udf_hash_config_list + (*actual)), &config,
                    sizeof(bcm_udf_hash_config_t));
            }

            (*actual) += 1;
        }

        tmp = tmp->next;
    }

    /* Release UDF mutex */
    UDF_UNLOCK(unit);

    return BCM_E_NONE;
}

#if defined (BCM_TOMAHAWK_SUPPORT)
/*
 * Function:
 *      bcmi_xgs4_udf_oper_mode_set
 * Purpose:
 *      Configure udf operation mode
 * Parameters:
 *      unit - (IN) Unit number.
 *      oper_mode - (IN) operation mode.
 * Returns:
 *      BCM_E_NONE - Operation successful
 *      BCM_E_PARAM - Invalid operational mode
 *      BCM_E_INIT - BCM unit not initialized
 *      BCM_E_BUSY - UDF tcam or offset memory has a valid entry configured
 * Notes:
 */
int
bcmi_xgs4_udf_oper_mode_set(
    int unit,
    bcm_udf_oper_mode_t mode)
{
    if (!(soc_feature(unit, soc_feature_udf_multi_pipe_support))) {
        return BCM_E_UNAVAIL;
    }

    /* UDF Module Init check */
    UDF_INIT_CHECK(unit);

    if ((mode != bcmUdfOperModeGlobal) &&
       (mode != bcmUdfOperModePipeLocal)) {
        return BCM_E_PARAM;
    }

    if (bcmi_xgs4_udf_oper_mode[unit] == mode) {
        return BCM_E_NONE;
    }

    if (UDF_CTRL(unit)->num_udfs > 0 ||
        UDF_CTRL(unit)->num_pkt_formats > 0) {
        return BCM_E_BUSY;
    }

    if (UDF_CTRL(unit)->udf_used_by_module == UDF_USED_BY_FIELD_MODULE) {
        return BCM_E_BUSY;
    }

    bcmi_xgs4_udf_oper_mode[unit] = mode;

    BCM_IF_ERROR_RETURN(bcmi_xgs4_udf_ser_init(unit, mode));

    BCM_IF_ERROR_RETURN(bcmi_xgs4_udf_ctrl_init(unit));

    UDF_CTRL(unit)->udf_used_by_module = UDF_USED_BY_UDF_MODULE;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs4_udf_oper_mode_get
 * Purpose:
 *      Retrieve udf operation mode
 * Parameters:
 *      unit - (IN) Unit number.
 *      oper_mode - (INOUT) operation mode.
 * Returns:
 *      BCM_E_NONE - Operation successful
 *      BCM_E_INIT - BCM unit not initialized
 *      BCM_E_UNAVAIL - Feature is not available
* Notes:
 */
int
bcmi_xgs4_udf_oper_mode_get(
    int unit,
    bcm_udf_oper_mode_t *mode)
{
    if (!(soc_feature(unit, soc_feature_udf_multi_pipe_support))) {
        return BCM_E_UNAVAIL;
    }
    /* UDF Module Init check */
    UDF_INIT_CHECK(unit);

    *mode = bcmi_xgs4_udf_oper_mode[unit];

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs4_udf_ser_init
 * Purpose:
 *      Initialize the SER for the given operational mode
 *
 * Parameters:
 *      unit    - (IN) BCM Device number.
 *      mode    - (IN) UDF Operational Mode enum value.
 * Returns:
 *      BCM_E_NONE   - Operation successful.
 *      BCM_E_PARAM  - Invalid operational mode.
 *      BCM_E_XXX    - For others.
 *
 * Notes:
 */
int
bcmi_xgs4_udf_ser_init(
    int unit,
    bcm_udf_oper_mode_t mode)
{
    int               ser_mode;     /* SER mode */
    int               rv;           /* Return Value */

    /* Validate input parameters */
    if ((mode != bcmUdfOperModeGlobal) &&
        (mode != bcmUdfOperModePipeLocal)) {
        return BCM_E_PARAM;
    }

    /* Update the SER mode */
    ser_mode = (mode == bcmUdfOperModeGlobal) ?
               _SOC_SER_MEM_MODE_GLOBAL:_SOC_SER_MEM_MODE_PIPE_UNIQUE;

    SOC_IF_ERROR_RETURN(soc_generic_ser_mem_scan_stop(unit));
    rv = soc_generic_ser_mem_update(unit, FP_UDF_TCAMm, 7, ser_mode);
    if (SOC_FAILURE(rv)) {
        LOG_CLI((BSL_META("Udf TCAM SER memory update fail \n")));
        return rv;
    }

    rv = soc_generic_ser_mem_update(unit, FP_UDF_OFFSETm, -1, ser_mode);
    if (SOC_FAILURE(rv)) {
        LOG_CLI((BSL_META("Udf OFFSET SER memory update fail \n")));
        return rv;
    }
    SOC_IF_ERROR_RETURN(soc_generic_ser_mem_scan_start(unit));

   return BCM_E_NONE;
}

#endif
