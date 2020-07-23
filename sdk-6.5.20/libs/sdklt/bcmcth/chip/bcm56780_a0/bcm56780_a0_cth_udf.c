/*! \file bcm56780_a0_cth_udf.c
 *
 * Chip stub for BCMCTH UDF.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_ha.h>
#include <bcmcth/bcmcth_udf_drv.h>
#include <bcmcth/bcmcth_imm_util.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmdrd/chip/bcm56780_a0_defs.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm.h>
#include <bcmcth/bcmcth_udf_internal.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmissu/issu_api.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_UDF

/*******************************************************************************
 * Typedefs
 */
#define HFE_POLICY_TABLE_MAX                    (12)
#define HFE_POLICY_TABLE_MAX_COMMAND_PER_INDEX  (20)
#define HFE_POLICY_TABLE_INDEX_MAX              (64)
#define MAX_FIELD_MUX_PROFILE                   (3)
#define CONTAINER_2B_OFFSET                     (16)
#define CONTAINER_1B_OFFSET                     (40)
#define IP0_BUS_SCG_MAX                         (149)
#define MAX_CONT_COUNT                          (48)
#define CONT_TYPE                               (3)

typedef struct hfe_cmd_fmt_s {
    bcmdrd_fid_t valid;
    bcmdrd_fid_t dest_cont_id;
    bcmdrd_fid_t src_pkt_offset;
} hfe_cmd_fmt_t;

/*! Enum defines container types available. */
typedef enum container_type_s {
    /*! 1 byte container  */
    CONTAINER_1_BYTE  = 1,

    /*! 2 byte container  */
    CONTAINER_2_BYTE  = 2,

    /*! 4 byte container  */
    CONTAINER_4_BYTE  = 4,

} container_type_t;

/*******************************************************************************
 * Local definitions
 */
/*
 * UDF variant information.
 */
static bcmcth_udf_var_info_t *bcmcth_udf_var_info[BCMDRD_CONFIG_MAX_UNITS];

/*
 * UDF packet header format HW information.
 */
static udf_control_t *udf_control[BCMDRD_CONFIG_MAX_UNITS];

/*
 * Macro to retrieve pointer to udf_control_t structure.
 */
#define UDF_CONTROL(_u_)   \
                 udf_control[_u_]

/*
 * Macro to retrieve pointer to udf_hdr_fmt_hw_info_t
 * structure for a given pkt hdr format.
 */
#define UDF_HDR_FMT_HW_INFO_GET(_u_, _hdr_fmt_)   \
                 udf_control[_u_]->udf_hdr_fmt_ptr[_hdr_fmt_]

#define UDF_HDR_FMT_COUNT_GET(_u_)   \
    bcmcth_udf_var_info[_u_]->udf_hdr_format_last_count

static const bcmdrd_sid_t hfe_policy_table[HFE_POLICY_TABLE_MAX] = {
    /* IPARSER0 HFE POLICY TABLE 0  */
    IPARSER0_HFE_POLICY_TABLE_0m,

    /* IPARSER1 HFE POLICY TABLE 0  */
    IPARSER1_HFE_POLICY_TABLE_0m,

    /* IPARSER1 HFE POLICY TABLE 1  */
    IPARSER1_HFE_POLICY_TABLE_1m,

    /* IPARSER1 HFE POLICY TABLE 2  */
    IPARSER1_HFE_POLICY_TABLE_2m,

    /* IPARSER1 HFE POLICY TABLE 3  */
    IPARSER1_HFE_POLICY_TABLE_3m,

    /* IPARSER1 HFE POLICY TABLE 4  */
    IPARSER1_HFE_POLICY_TABLE_4m,

    /* IPARSER1 HFE POLICY TABLE 5  */
    IPARSER1_HFE_POLICY_TABLE_5m,

    /* IPARSER2 HFE POLICY TABLE 0  */
    IPARSER2_HFE_POLICY_TABLE_0m,

    /* IPARSER2 HFE POLICY TABLE 1  */
    IPARSER2_HFE_POLICY_TABLE_1m,

    /* IPARSER2 HFE POLICY TABLE 2  */
    IPARSER2_HFE_POLICY_TABLE_2m,

    /* IPARSER2 HFE POLICY TABLE 3  */
    IPARSER2_HFE_POLICY_TABLE_3m,

    /* IPARSER2 HFE POLICY TABLE 4  */
    IPARSER2_HFE_POLICY_TABLE_4m,
};
static const bcmdrd_sid_t bus_scg_overwrite[IP0_BUS_SCG_MAX] = {
      PBLOCK_MISC_IP0_BUS_SCG0r,
      PBLOCK_MISC_IP0_BUS_SCG1r,
      PBLOCK_MISC_IP0_BUS_SCG2r,
      PBLOCK_MISC_IP0_BUS_SCG3r,
      PBLOCK_MISC_IP0_BUS_SCG4r,
      PBLOCK_MISC_IP0_BUS_SCG5r,
      PBLOCK_MISC_IP1_BUS_SCG0r,
      PBLOCK_MISC_IP1_BUS_SCG1r,
      PBLOCK_MISC_IP1_BUS_SCG2r,
      PBLOCK_MISC_IP1_BUS_SCG3r,
      PBLOCK_MISC_IP1_BUS_SCG4r,
      PBLOCK_MISC_IP1_BUS_SCG5r,
      PBLOCK_MISC_IP1_BUS_SCG6r,
      PBLOCK_MISC_IP1_BUS_SCG7r,
      PBLOCK_MISC_IP1_BUS_SCG8r,
      PBLOCK_MISC_IP1_BUS_SCG9r,
      PBLOCK_MISC_IP1_BUS_SCG10r,
      PBLOCK_MISC_IP1_BUS_SCG11r,
      PBLOCK_MISC_IP1_BUS_SCG12r,
      PBLOCK_MISC_IP1_BUS_SCG13r,
      PBLOCK_MISC_IP2_0_BUS_SCG0r,
      PBLOCK_MISC_IP2_0_BUS_SCG1r,
      PBLOCK_MISC_IP2_0_BUS_SCG2r,
      PBLOCK_MISC_IP2_0_BUS_SCG3r,
      PBLOCK_MISC_IP2_0_BUS_SCG4r,
      PBLOCK_MISC_IP2_0_BUS_SCG5r,
      PBLOCK_MISC_IP2_0_BUS_SCG6r,
      PBLOCK_MISC_IP2_0_BUS_SCG7r,
      PBLOCK_MISC_IP2_0_BUS_SCG8r,
      PBLOCK_MISC_IP2_1_BUS_SCG0r,
      PBLOCK_MISC_IP2_1_BUS_SCG1r,
      PBLOCK_MISC_IP2_1_BUS_SCG2r,
      PBLOCK_MISC_IP2_1_BUS_SCG3r,
      PBLOCK_MISC_IP2_1_BUS_SCG4r,
      PBLOCK_MISC_IP2_1_BUS_SCG5r,
      PBLOCK_MISC_IP3_0_BUS_SCG0r,
      PBLOCK_MISC_IP3_0_BUS_SCG1r,
      PBLOCK_MISC_IP3_0_BUS_SCG2r,
      PBLOCK_MISC_IP3_0_BUS_SCG3r,
      PBLOCK_MISC_IP3_0_BUS_SCG4r,
      PBLOCK_MISC_IP3_0_BUS_SCG5r,
      PBLOCK_MISC_IP3_0_BUS_SCG6r,
      PBLOCK_MISC_IP3_0_BUS_SCG7r,
      PBLOCK_MISC_IP3_0_BUS_SCG8r,
      PBLOCK_MISC_IP3_0_BUS_SCG9r,
      PBLOCK_MISC_IP3_0_BUS_SCG10r,
      PBLOCK_MISC_IP3_0_BUS_SCG11r,
      PBLOCK_MISC_IP3_0_BUS_SCG12r,
      PBLOCK_MISC_IP3_0_BUS_SCG13r,
      PBLOCK_MISC_IP3_0_BUS_SCG14r,
      PBLOCK_MISC_IP3_0_BUS_SCG15r,
      PBLOCK_MISC_IP3_0_BUS_SCG16r,
      PBLOCK_MISC_IP3_0_BUS_SCG17r,
      PBLOCK_MISC_IP3_0_BUS_SCG18r,
      PBLOCK_MISC_IP3_0_BUS_SCG19r,
      PBLOCK_MISC_IP3_0_BUS_SCG20r,
      PBLOCK_MISC_IP3_0_BUS_SCG21r,
      PBLOCK_MISC_IP3_1_BUS_SCG0r,
      PBLOCK_MISC_IP3_1_BUS_SCG1r,
      PBLOCK_MISC_IP3_1_BUS_SCG2r,
      PBLOCK_MISC_IP3_1_BUS_SCG3r,
      PBLOCK_MISC_IP3_1_BUS_SCG4r,
      PBLOCK_MISC_IP3_1_BUS_SCG5r,
      PBLOCK_MISC_IP3_1_BUS_SCG6r,
      PBLOCK_MISC_IP3_1_BUS_SCG7r,
      PBLOCK_MISC_IP5_BUS_SCG0r,
      PBLOCK_MISC_IP5_BUS_SCG1r,
      PBLOCK_MISC_IP5_BUS_SCG2r,
      PBLOCK_MISC_IP5_BUS_SCG3r,
      PBLOCK_MISC_IP5_BUS_SCG4r,
      PBLOCK_MISC_IP5_BUS_SCG5r,
      PBLOCK_MISC_IP5_BUS_SCG6r,
      PBLOCK_MISC_IP5_BUS_SCG7r,
      PBLOCK_MISC_IP5_BUS_SCG8r,
      PBLOCK_MISC_IP5_BUS_SCG9r,
      PBLOCK_MISC_IP5_BUS_SCG10r,
      PBLOCK_MISC_IP5_BUS_SCG11r,
      PBLOCK_MISC_IP5_BUS_SCG12r,
      PBLOCK_MISC_IP5_BUS_SCG13r,
      PBLOCK_MISC_IP5_BUS_SCG14r,
      PBLOCK_MISC_IP5_BUS_SCG15r,
      PBLOCK_MISC_IP5_BUS_SCG16r,
      PBLOCK_MISC_IP5_BUS_SCG17r,
      PBLOCK_MISC_IP5_BUS_SCG18r,
      PBLOCK_MISC_IP5_BUS_SCG19r,
      PBLOCK_MISC_IP5_BUS_SCG20r,
      PBLOCK_MISC_IP5_BUS_SCG21r,
      PBLOCK_MISC_IP5_BUS_SCG22r,
      PBLOCK_MISC_IP5_BUS_SCG23r,
      PBLOCK_MISC_IP5_BUS_SCG24r,
      PBLOCK_MISC_IP5_BUS_SCG25r,
      PBLOCK_MISC_IP5_BUS_SCG26r,
      PBLOCK_MISC_IP5_BUS_SCG27r,
      PBLOCK_MISC_IP5_BUS_SCG28r,
      PBLOCK_MISC_IP5_BUS_SCG29r,
      PBLOCK_MISC_IP5_BUS_SCG30r,
      PBLOCK_MISC_IP5_BUS_SCG31r,
      PBLOCK_MISC_IP5_BUS_SCG32r,
      PBLOCK_MISC_IP5_BUS_SCG33r,
      PBLOCK_MISC_IP6_0_BUS_SCG0r,
      PBLOCK_MISC_IP6_0_BUS_SCG1r,
      PBLOCK_MISC_IP6_0_BUS_SCG2r,
      PBLOCK_MISC_IP6_0_BUS_SCG3r,
      PBLOCK_MISC_IP6_0_BUS_SCG4r,
      PBLOCK_MISC_IP6_0_BUS_SCG5r,
      PBLOCK_MISC_IP6_0_BUS_SCG6r,
      PBLOCK_MISC_IP6_0_BUS_SCG7r,
      PBLOCK_MISC_IP6_1_BUS_SCG0r,
      PBLOCK_MISC_IP6_1_BUS_SCG1r,
      PBLOCK_MISC_IP6_1_BUS_SCG2r,
      PBLOCK_MISC_IP6_1_BUS_SCG3r,
      PBLOCK_MISC_IP6_1_BUS_SCG4r,
      PBLOCK_MISC_IP7_BUS_SCG0r,
      PBLOCK_MISC_IP7_BUS_SCG1r,
      PBLOCK_MISC_IP7_BUS_SCG2r,
      PBLOCK_MISC_IP7_BUS_SCG3r,
      PBLOCK_MISC_IP7_BUS_SCG4r,
      PBLOCK_MISC_IP7_BUS_SCG5r,
      PBLOCK_MISC_IP7_BUS_SCG6r,
      PBLOCK_MISC_IP7_BUS_SCG7r,
      PBLOCK_MISC_IP8_0_BUS_SCG0r,
      PBLOCK_MISC_IP8_0_BUS_SCG1r,
      PBLOCK_MISC_IP8_0_BUS_SCG2r,
      PBLOCK_MISC_IP8_0_BUS_SCG3r,
      PBLOCK_MISC_IP8_1_BUS_SCG0r,
      PBLOCK_MISC_IP8_1_BUS_SCG1r,
      PBLOCK_MISC_IP8_1_BUS_SCG2r,
      PBLOCK_MISC_IP8_1_BUS_SCG3r,
      PBLOCK_MISC_IP8_1_BUS_SCG4r,
      PBLOCK_MISC_IP8_1_BUS_SCG5r,
      PBLOCK_MISC_IP8_1_BUS_SCG6r,
      PBLOCK_MISC_IP8_1_BUS_SCG7r,
      PBLOCK_MISC_IP8_1_BUS_SCG8r,
      PBLOCK_MISC_IP8_1_BUS_SCG9r,
      PBLOCK_MISC_IP9_0_BUS_SCG0r,
      PBLOCK_MISC_IP9_0_BUS_SCG1r,
      PBLOCK_MISC_IP9_0_BUS_SCG2r,
      PBLOCK_MISC_IP9_0_BUS_SCG3r,
      PBLOCK_MISC_IP9_1_BUS_SCG0r,
      PBLOCK_MISC_IP9_1_BUS_SCG1r,
      PBLOCK_MISC_IP9_1_BUS_SCG2r,
      PBLOCK_MISC_IP9_2_BUS_SCG0r,
      PBLOCK_MISC_IP9_2_BUS_SCG1r,
      PBLOCK_MISC_IP9_2_BUS_SCG2r,
      PBLOCK_MISC_IP9_2_BUS_SCG3r,
      PBLOCK_MISC_IP9_2_BUS_SCG4r,
      PBLOCK_MISC_IP9_2_BUS_SCG5r,
      PBLOCK_MISC_IP9_2_BUS_SCG6r,
      PBLOCK_MISC_IP9_2_BUS_SCG7r,
};

static const hfe_cmd_fmt_t valid_cmds[HFE_POLICY_TABLE_MAX_COMMAND_PER_INDEX] = {
    {CMD_C_0_VALIDf, CMD_C_0_DEST_CONT_IDf, CMD_C_0_SRC_PKT_OFFSETf},
    {CMD_C_1_VALIDf, CMD_C_1_DEST_CONT_IDf, CMD_C_1_SRC_PKT_OFFSETf},
    {CMD_C_2_VALIDf, CMD_C_2_DEST_CONT_IDf, CMD_C_2_SRC_PKT_OFFSETf},
    {CMD_C_3_VALIDf, CMD_C_3_DEST_CONT_IDf, CMD_C_3_SRC_PKT_OFFSETf},
    {CMD_C_4_VALIDf, CMD_C_4_DEST_CONT_IDf, CMD_C_4_SRC_PKT_OFFSETf},
    {CMD_C_5_VALIDf, CMD_C_5_DEST_CONT_IDf, CMD_C_5_SRC_PKT_OFFSETf},
    {CMD_C_6_VALIDf, CMD_C_6_DEST_CONT_IDf, CMD_C_6_SRC_PKT_OFFSETf},
    {CMD_C_7_VALIDf, CMD_C_7_DEST_CONT_IDf, CMD_C_7_SRC_PKT_OFFSETf},
    {CMD_C_8_VALIDf, CMD_C_8_DEST_CONT_IDf, CMD_C_8_SRC_PKT_OFFSETf},
    {CMD_C_9_VALIDf,  CMD_C_9_DEST_CONT_IDf, CMD_C_9_SRC_PKT_OFFSETf},
    {CMD_C_10_VALIDf, CMD_C_10_DEST_CONT_IDf, CMD_C_10_SRC_PKT_OFFSETf},
    {CMD_C_11_VALIDf, CMD_C_11_DEST_CONT_IDf, CMD_C_11_SRC_PKT_OFFSETf},
    {CMD_C_12_VALIDf, CMD_C_12_DEST_CONT_IDf, CMD_C_12_SRC_PKT_OFFSETf},
    {CMD_C_13_VALIDf, CMD_C_13_DEST_CONT_IDf, CMD_C_13_SRC_PKT_OFFSETf},
    {CMD_C_14_VALIDf, CMD_C_14_DEST_CONT_IDf, CMD_C_14_SRC_PKT_OFFSETf},
    {CMD_C_15_VALIDf, CMD_C_15_DEST_CONT_IDf, CMD_C_15_SRC_PKT_OFFSETf},
    {CMD_C_16_VALIDf, CMD_C_16_DEST_CONT_IDf, CMD_C_16_SRC_PKT_OFFSETf},
    {CMD_C_17_VALIDf, CMD_C_17_DEST_CONT_IDf, CMD_C_17_SRC_PKT_OFFSETf},
    {CMD_C_18_VALIDf, CMD_C_18_DEST_CONT_IDf, CMD_C_18_SRC_PKT_OFFSETf},
    {CMD_C_19_VALIDf, CMD_C_19_DEST_CONT_IDf, CMD_C_19_SRC_PKT_OFFSETf}
};

/* Local Structure to hold command information assigned to UDF object. */
typedef struct udf_obj_hdr_fmt_hw_cfg_s {
   /* CMD Bitmap for each stage */
   uint32_t  cmd_bmap[UDF_MAX_PARSER_STAGES]
                     [HFE_POLICY_TABLE_INDEX_MAX];
   uint8_t   num_4b_cont; /* Number of 4 byte container needed for UDF policy*/
   uint8_t   num_2b_cont; /* Number of 2 byte container needed for UDF policy*/
   uint8_t   num_1b_cont; /* Number of 1 byte container needed for UDF policy*/
} udf_obj_hdr_fmt_hw_cfg_t;


/*******************************************************************************
 * Local Functions
 */
/*!
 * \brief Get UDF anchor information.
 *
 * \param [in] unit Unit number.
 * \param [in] hdr_fmt UDF header format
 *
 * \retval SHR_E_NONE No errors.
 */
static const udf_anchor_t
*bcm56780_a0_udf_anchor_info_get(int unit, int hdr_fmt)
{

    if (bcmcth_udf_var_info[unit]->udf_anchor) {
        return bcmcth_udf_var_info[unit]->udf_anchor[hdr_fmt];
    } else {
        return NULL;
    }

}

/*!
 * \brief This routine clears all PBLOCK_MISC_IP*_*_BUS_SCG registers.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction ID for current operation
 * \param [in] ltid Logical Table enum for current operation
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Hardware write operation unsuccessful.
 */
static int
bcm56780_a0_bus_scg_register_clear(int unit,
                                   uint32_t trans_id,
                                   bcmltd_sid_t lt_sid)
{

    int i;
    bcmdrd_sid_t         pt_id;
    uint32_t             rsp_entry_wsize;
    bcmltd_sid_t         rsp_ltid = 0;
    uint32_t             rsp_flags = 0;
    PBLOCK_MISC_IP0_BUS_SCG0r_t ip_bus_scg_entry;
    uint32_t *buf_ptr    = NULL;
    bcmbd_pt_dyn_info_t  pt_info;
    bcmdrd_fid_t         pt_fid;
    uint32_t             fld_val[2];
    SHR_FUNC_ENTER(unit);
    buf_ptr = (uint32_t *)(&ip_bus_scg_entry);
    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_fid = DATAf;
    pt_id = PBLOCK_MISC_IP0_BUS_SCG0r;
    rsp_entry_wsize = bcmdrd_pt_entry_wsize(unit, pt_id);
    for (i = 0; i < IP0_BUS_SCG_MAX; i++) {
        pt_id = bus_scg_overwrite[i];
        SHR_IF_ERR_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit, 0, pt_id, &pt_info,
                                    NULL, NULL, /* misc_info */
                                    BCMPTM_OP_READ,
                                    NULL,
                                    rsp_entry_wsize,
                                    lt_sid,
                                    trans_id,
                                    NULL, NULL,
                                    buf_ptr,
                                    &rsp_ltid,
                                    &rsp_flags));
        fld_val[0] = 0;
        fld_val[1] = 0;
        bcmdrd_pt_field_set(unit, pt_id, buf_ptr, pt_fid,
                            fld_val);
        SHR_IF_ERR_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit, 0, pt_id, &pt_info,
                                NULL, NULL, /* misc_info */
                                BCMPTM_OP_WRITE,
                                buf_ptr,
                                0,
                                lt_sid,
                                trans_id,
                                NULL, NULL, NULL,
                                &rsp_ltid,
                                &rsp_flags));
    }
exit:
    SHR_FUNC_EXIT();

}

/*!
 * \brief Check if HFE commands are avaible at give index.
 *
 * \param [in] unit Unit number.
 * \param [in] hfe_policy_table HFE policy table
 * \param [in] hfe_index HFE policy table index.
 * \param [in] hfe_command Command number at given HFE policy table index
 *
 * \retval SHR_E_NONE No errors.
 */
static bool
bcm56780_a0_udf_is_hfe_index_available(int unit,
                                       udf_hfe_policy_table_t udf_hfe_policy_table,
                                       uint32_t hfe_index,
                                       uint32_t hfe_command)
{

    if ((udf_hfe_policy_table < HFE_POLICY_TABLE_MAX) &&
        (hfe_index < HFE_POLICY_TABLE_INDEX_MAX) &&
        (hfe_command < HFE_POLICY_TABLE_MAX_COMMAND_PER_INDEX)) {
        if (bcmcth_udf_var_info[unit]->hfe_policy_table_index_get) {
            return !(bcmcth_udf_var_info[unit]->
                     hfe_policy_table_index_get(udf_hfe_policy_table,
                                                hfe_index,
                                                hfe_command));
        }
    }

    return SHR_E_NONE;

}


/*!
 * \brief This routine configures field bus containers for UDF extraction.
 *
 * \param [in] unit Unit number.
 * \param [in] container_list list of avaiable 4 byte containers
 * \param [in] container_list list of avaiable 2 byte containers
 * \param [in] container_list list of avaiable 1 byte containers
 * \param [in] trans_id Transaction ID for current operation
 * \param [in] ltid Logical Table enum for current operation
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Input parameter error.
 * \retval SHR_E_FAIL Hardware write operation unsuccessful.
 */
static int
configure_field_mux_cont(int unit, uint32_t cont_4B_bmp,
                         uint32_t cont_2B_bmp,
                         uint32_t cont_1B_bmp,
                         uint32_t trans_id,
                         bcmltd_sid_t lt_sid)
{

    bcmdrd_sid_t         pt_id;
    uint32_t             rsp_entry_wsize;
    uint32_t             index;
    bcmltd_sid_t         rsp_ltid = 0;
    uint32_t             rsp_flags = 0;
    FIELD_MUX_PROFILEm_t fld_mux_entry;
    uint32_t *buf_ptr    = NULL;
    bcmbd_pt_dyn_info_t  pt_info;
    bcmdrd_fid_t         pt_fid;
    uint32_t             fld_val[2];

    SHR_FUNC_ENTER(unit);

    buf_ptr = (uint32_t *)(&fld_mux_entry);
    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_id = FIELD_MUX_PROFILEm;
    pt_fid = MUX_SEL_FWDf;
    rsp_entry_wsize = bcmdrd_pt_entry_wsize(unit, pt_id);

    for (index = 0; index < MAX_FIELD_MUX_PROFILE; index++) {
        pt_info.index = index;

        /* Read PT table entry */
        SHR_IF_ERR_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit, 0, pt_id, &pt_info,
                                    NULL, NULL, /* misc_info */
                                    BCMPTM_OP_READ,
                                    NULL,
                                    rsp_entry_wsize,
                                    lt_sid,
                                    trans_id,
                                    NULL, NULL,
                                    buf_ptr,
                                    &rsp_ltid,
                                    &rsp_flags));

        SHR_IF_ERR_VERBOSE_EXIT(
            bcmdrd_pt_field_get(unit, pt_id, buf_ptr, pt_fid, fld_val));
        fld_val[0] |= cont_4B_bmp;
        fld_val[0] |= (cont_2B_bmp & ((1 << UDF_4_BYTE_CONTAINER_MAX) - 1))
                      << UDF_4_BYTE_CONTAINER_MAX;
        fld_val[1] |= cont_2B_bmp >> UDF_4_BYTE_CONTAINER_MAX;
        fld_val[1] |= cont_1B_bmp <<
                      ((UDF_4_BYTE_CONTAINER_MAX + UDF_2_BYTE_CONTAINER_MAX)
                      - (sizeof(uint32_t) * 8));
        bcmdrd_pt_field_set(unit, pt_id, buf_ptr, pt_fid,
                            fld_val);

        /* Write PT table entry */
        SHR_IF_ERR_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit, 0, pt_id, &pt_info,
                                    NULL, NULL, /* misc_info */
                                    BCMPTM_OP_WRITE,
                                    buf_ptr,
                                    0,
                                    lt_sid,
                                    trans_id,
                                    NULL, NULL, NULL,
                                    &rsp_ltid,
                                    &rsp_flags));
    }

exit:
    SHR_FUNC_EXIT();

}


/*!
 * \brief This routine programs specified forward container to extract
 *  tunnel header data during tunnel termination profile.
 *
 * \param [in] unit Unit number.
 * \param [in] container_list list of avaiable 4 byte containers
 * \param [in] container_list list of avaiable 2 byte containers
 * \param [in] container_list list of avaiable 1 byte containers
 * \param [in] trans_id Transaction ID for current operation
 * \param [in] ltid Logical Table enum for current operation
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Input parameter error.
 * \retval SHR_E_FAIL Hardware write operation unsuccessful.
 */
static int
configure_field_mux_profile(int unit, uint32_t cont_4B_bmp,
                            uint32_t cont_2B_bmp,
                            uint32_t cont_1B_bmp,
                            int profile_id,
                            uint32_t trans_id,
                            bcmltd_sid_t lt_sid,
                            int event)
{

    bcmdrd_sid_t         pt_id;
    uint32_t             rsp_entry_wsize;
    bcmltd_sid_t         rsp_ltid = 0;
    uint32_t             rsp_flags = 0;
    FIELD_MUX_PROFILEm_t fld_mux_entry;
    uint32_t *buf_ptr    = NULL;
    bcmbd_pt_dyn_info_t  pt_info;
    bcmdrd_fid_t         pt_fid;
    uint32_t             fld_val[2];

    SHR_FUNC_ENTER(unit);

    buf_ptr = (uint32_t *)(&fld_mux_entry);
    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    pt_id = FIELD_MUX_PROFILEm;
    pt_fid = MUX_SEL_FWDf;
    rsp_entry_wsize = bcmdrd_pt_entry_wsize(unit, pt_id);

    pt_info.index = profile_id;

    /* Read PT table entry */
    SHR_IF_ERR_EXIT
    (bcmptm_ltm_mreq_indexed_lt(unit, 0, pt_id, &pt_info,
                                NULL, NULL, /* misc_info */
                                BCMPTM_OP_READ,
                                NULL,
                                rsp_entry_wsize,
                                lt_sid,
                                trans_id,
                                NULL, NULL,
                                buf_ptr,
                                &rsp_ltid,
                                &rsp_flags));

    SHR_IF_ERR_VERBOSE_EXIT(
        bcmdrd_pt_field_get(unit, pt_id, buf_ptr, pt_fid, fld_val));

    if (event) {
        /* handles case for container profile create and update */
        fld_val[0] &= ~cont_4B_bmp;
        fld_val[0] &= ~((cont_2B_bmp & ((1 << UDF_4_BYTE_CONTAINER_MAX) - 1))
                      << UDF_4_BYTE_CONTAINER_MAX);
        fld_val[1] &= ~(cont_2B_bmp >> UDF_4_BYTE_CONTAINER_MAX);
        fld_val[1] &= ~(cont_1B_bmp <<
                      ((UDF_4_BYTE_CONTAINER_MAX + UDF_2_BYTE_CONTAINER_MAX)
                      - (sizeof(uint32_t) * 8)));
    } else {
        /* handles case for container profile delete */
        fld_val[0] |= cont_4B_bmp;
        fld_val[0] |= (cont_2B_bmp & ((1 << UDF_4_BYTE_CONTAINER_MAX) - 1))
                      << UDF_4_BYTE_CONTAINER_MAX;
        fld_val[1] |= cont_2B_bmp >> UDF_4_BYTE_CONTAINER_MAX;
        fld_val[1] |= cont_1B_bmp <<
                      ((UDF_4_BYTE_CONTAINER_MAX + UDF_2_BYTE_CONTAINER_MAX)
                      - (sizeof(uint32_t) * 8));
    }
    bcmdrd_pt_field_set(unit, pt_id, buf_ptr, pt_fid,
                        fld_val);

    /* Write PT table entry */
    SHR_IF_ERR_EXIT
    (bcmptm_ltm_mreq_indexed_lt(unit, 0, pt_id, &pt_info,
                                NULL, NULL, /* misc_info */
                                BCMPTM_OP_WRITE,
                                buf_ptr,
                                0,
                                lt_sid,
                                trans_id,
                                NULL, NULL, NULL,
                                &rsp_ltid,
                                &rsp_flags));


exit:
    SHR_FUNC_EXIT();

}

/*!
 * \brief This routine configures destination container ID needed for
 *  UDF extraction.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to bcmcth_udf_policy_info_t structure.
 * \param [in] stage Packet parsing stage.
 * \param [in] hfe_cnt Count of HFE index that need to be updated for UDF policy.
 * \param [in] cont_gran Container granularity 4B/2B/1B.
 * \param [in] cmd Command ID that need to be updated for HFE table index
 * \param [in] cmd_valid Command is avaiable or not for UDF extraction.
 * \param [in] container_list list of avaiable containers
 * \param [in] udf_anchor_info Pointer to udf_anchor_t structure
 * \param [in] hdr_fmt_hw_info Pointer to udf_hdr_fmt_hw_info_t structure
 * \param [in] trans_id Transaction ID for current operation
 * \param [in] ltid Logical Table enum for current operation
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Input parameter error.
 * \retval SHR_E_FAIL Hardware write operation unsuccessful.
 */
static int
configure_dest_cont(int unit, bcmcth_udf_policy_info_t *lt_entry,
                    int stage, int hfe_idx, int cont_gran,
                    uint32_t *cmd,
                    bool *cmd_valid,
                    int *container_list,
                    const udf_anchor_t *udf_anchor_info,
                    udf_hdr_fmt_hw_info_t *hdr_fmt_hw_info,
                    uint32_t trans_id,
                    bcmltd_sid_t lt_sid)
{

    uint32_t fld_val;
    uint8_t tbl_type;
    uint32_t index;
    hfe_cmd_fmt_t hfe_cmd;
    bcmdrd_sid_t pt_id;
    uint32_t            alloc_sz;
    bcmltd_sid_t        rsp_ltid = 0;
    uint32_t            rsp_flags = 0;
    uint32_t offset;
    uint32_t *buf_ptr = NULL;
    bcmbd_pt_dyn_info_t pt_info;

    SHR_FUNC_ENTER(unit);

    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));
    index = stage * HFE_POLICY_TABLE_INDEX_MAX;
    pt_info.index =hdr_fmt_hw_info->hfe_profile_entry[index + hfe_idx];
    pt_info.tbl_inst = -1;

    if (hdr_fmt_hw_info->current_offset) {
        offset = hdr_fmt_hw_info->current_offset;
    } else {
        offset = lt_entry->offset +
                 hdr_fmt_hw_info->hfe_profile_index[index + hfe_idx].base_offset;
    }

    tbl_type = udf_anchor_info->table[stage]->tbl_type;
    pt_id = hfe_policy_table[tbl_type];

    /* Allocate buffer to read/write to HFE POLICY table. */
    alloc_sz = BCMDRD_WORDS2BYTES(bcmdrd_pt_entry_wsize(unit, pt_id));
    SHR_ALLOC(buf_ptr, alloc_sz, "bcmcthUDFEntry");
    SHR_NULL_CHECK(buf_ptr, SHR_E_MEMORY);
    sal_memset(buf_ptr, 0, alloc_sz);

    /* Read PT table entry */
    SHR_IF_ERR_EXIT
    (bcmptm_ltm_mreq_indexed_lt(unit, 0, pt_id, &pt_info,
                                NULL, NULL, /* misc_info */
                                BCMPTM_OP_READ,
                                NULL,
                                alloc_sz,
                                lt_sid,
                                trans_id,
                                NULL, NULL,
                                buf_ptr,
                                &rsp_ltid,
                                &rsp_flags));

    while (*cmd_valid) {
        hfe_cmd = valid_cmds[*cmd];

        /* mark the command as valid */
        fld_val = 1;
        bcmdrd_pt_field_set(unit, pt_id, buf_ptr, hfe_cmd.valid,
                            &fld_val);

        /* Set destination container ID for UDF extraction */
        fld_val = *container_list;
        bcmdrd_pt_field_set(unit, pt_id, buf_ptr, hfe_cmd.dest_cont_id,
                            &fld_val);
        container_list++;

        /* Set offset for UDF extraction */
        fld_val = offset;
        bcmdrd_pt_field_set(unit, pt_id, buf_ptr, hfe_cmd.src_pkt_offset,
                            &fld_val);

        /* Write PT table entry */
        SHR_IF_ERR_EXIT
        (bcmptm_ltm_mreq_indexed_lt(unit, 0, pt_id, &pt_info,
                                    NULL, NULL, /* misc_info */
                                    BCMPTM_OP_WRITE,
                                    buf_ptr,
                                    0,
                                    lt_sid,
                                    trans_id,
                                    NULL, NULL, NULL,
                                    &rsp_ltid,
                                    &rsp_flags));
        cmd++;
        cmd_valid++;
        offset +=cont_gran; /* offset advanced by container size granularity */

    }

    hdr_fmt_hw_info->current_offset = offset;

exit:
    SHR_FREE(buf_ptr);
    SHR_FUNC_EXIT();

}

/*!
 * \brief This routine list of avaiable containers for UDF extraction.
 * \param [in]  unit Unit number.
 * \param [in]  parser parser0/1/2.
 * \param [in]  cont_type container size type 1byte/2byte/4byte.
 * \param [in]  num_cont Number containers required.
 * \param [out] cont_list list contains number of available containers.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
udf_available_container_get(int unit, int parser,
                            container_type_t cont_type,
                            uint8_t num_cont,
                            int *cont_list)
{
    int rv = SHR_E_NONE;
    size_t cont_size = 0, i;
    const udf_cont_info_t *container_list = NULL;

    SHR_FUNC_ENTER(unit);

    switch (parser) {
        case 1:
            switch (cont_type) {
                case 1:
                    bcmcth_udf_var_info[unit]->iparser1_1_byte_cont_get
                                               (&container_list, &cont_size);
                    break;
                case 2:
                    bcmcth_udf_var_info[unit]->iparser1_2_byte_cont_get
                                               (&container_list, &cont_size);
                    break;
                case 4:
                    bcmcth_udf_var_info[unit]->iparser1_4_byte_cont_get
                                               (&container_list, &cont_size);
                    break;
                default:
                    break;
            }
            break;
        case 2:
            switch (cont_type) {
                case 1:
                    bcmcth_udf_var_info[unit]->iparser2_1_byte_cont_get
                                               (&container_list, &cont_size);
                    break;
                case 2:
                    bcmcth_udf_var_info[unit]->iparser2_2_byte_cont_get
                                              (&container_list, &cont_size);
                    break;
                case 4:
                    bcmcth_udf_var_info[unit]->iparser2_4_byte_cont_get
                                              (&container_list, &cont_size);
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }

    for (i = 0; i < cont_size; i++) {
        cont_list[i] = container_list[i].cont_id;
    }

    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT();

}


/*!
 * \brief This routine calculates number of commands needed per HFE index to
 *  create a UDF policy.
 * \param [in]  unit Unit number.
 * \param [in] lt_entry Pointer to bcmcth_udf_policy_info_t structure.
 * \param [out] num_4b_cont Number of 4 Byte container required.
 * \param [out] num_2B_cont Number of 2 Byte container required.
 * \param [out] num_1B_cont Number of 1 Byte container required.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
num_cmd_per_udf_policy(int unit,
                       bcmcth_udf_policy_info_t *lt_entry,
                       int *num_4B_cont,
                       int *num_2B_cont,
                       int *num_1B_cont)
{

    int i;
    int size_4B_arr;
    int size_2B_arr;
    int size_1B_arr;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    size_4B_arr = sizeof(lt_entry->container_4_byte)
                  /sizeof(lt_entry->container_4_byte[0]);
    size_2B_arr = sizeof(lt_entry->container_2_byte)
                  /sizeof(lt_entry->container_2_byte[0]);
    size_1B_arr = sizeof(lt_entry->container_1_byte)
                  /sizeof(lt_entry->container_1_byte[0]);
    for (i = 0; i < size_4B_arr; i++) {
        if (lt_entry->container_4_byte[i]) {
            *num_4B_cont+=1;
        }
    }
    for (i = 0; i < size_2B_arr; i++) {
        if (lt_entry->container_2_byte[i]) {
            *num_2B_cont+=1;
        }
    }
    for (i = 0; i < size_1B_arr; i++) {
        if (lt_entry->container_1_byte[i]) {
            *num_1B_cont+=1;
        }
    }
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT();

}

static int
udf_policy_lt_fields_free(int unit,
                          int num_fields,
                          bcmltd_fields_t *in)
{
    int i;

    SHR_FUNC_ENTER(unit);

    if (in->field) {
        for (i = 0; i < num_fields; i++) {
            if (in->field[i]) {
                SHR_FREE(in->field[i]);
            }
        }
        SHR_FREE(in->field);
    }

    SHR_FUNC_EXIT();
}

static int
udf_policy_lt_fields_allocate (int unit,
                               bcmltd_fields_t *in,
                               size_t num_fields)
{
    uint32_t i;
    size_t alloc_sz;

    SHR_FUNC_ENTER(unit);

    /* Allocate fields buffers */
    alloc_sz = sizeof(bcmltd_field_t *) * num_fields;
    SHR_ALLOC(in->field, alloc_sz, "bcmcthUdfEntryFields");
    SHR_NULL_CHECK(in->field, SHR_E_MEMORY);
    sal_memset(in->field, 0, alloc_sz);

    in->count = num_fields;

    for (i = 0; i < num_fields; i++) {
        SHR_ALLOC(in->field[i], sizeof(bcmltd_field_t), "bcmcthUdfEntryField");
        SHR_NULL_CHECK(in->field[i], SHR_E_MEMORY);
        sal_memset(in->field[i], 0, sizeof(bcmltd_field_t));
    }

    SHR_FUNC_EXIT();

exit:
    udf_policy_lt_fields_free(unit, num_fields, in);
    SHR_FUNC_EXIT();
}


/*!
 * \brief Uninstall HW policy commands used for UDF object.
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction ID for current operation
 * \param [in] ltid Logical Table enum for current operation
 * \param [in] lt_entry Pointer to bcmcth_udf_policy_info_t structure.

 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Input parameter error.
 * \retval SHR_E_FAIL Hardware write operation unsuccessful.
 */
static int
bcm56780_a0_udf_policy_cmd_uninstall(int unit,
                                     uint32_t trans_id,
                                     bcmltd_sid_t lt_sid,
                                     bcmcth_udf_policy_info_t *lt_entry)
{
    uint8_t stage;
    uint8_t hfe_cnt;
    uint32_t fld_val, cmd;
    bcmdrd_sid_t pt_id;
    uint8_t tbl_type;
    hfe_cmd_fmt_t hfe_cmd;
    uint32_t cmd_bmap;
    uint32_t index;
    const udf_anchor_t *udf_anchor_info = NULL;
    udf_hdr_fmt_hw_info_t *hdr_fmt_hw_info = NULL;
    IPARSER2_HFE_POLICY_TABLE_0m_t hfe_policy_entry;
    uint32_t *buf_ptr = NULL;
    bcmbd_pt_dyn_info_t pt_info;
    bcmltd_sid_t        rsp_ltid = 0;
    uint32_t            rsp_flags = 0;
    uint32_t            rsp_entry_wsize;
    bcmltd_fields_t key_fields = {0};
    bcmltd_fields_t imm_fields = {0};
    size_t num_fields, i;
    bcmltd_fields_t *flist;
    bcmltd_field_t *data_field;
    uint32_t fid;
    uint32_t idx;

    SHR_FUNC_ENTER(unit);

    /* Allocate key field */
    SHR_IF_ERR_EXIT
        (udf_policy_lt_fields_allocate(unit,
                                       &key_fields,
                                       1));

    key_fields.field[0]->id   = UDF_POLICYt_UDF_POLICY_IDf;
    key_fields.field[0]->data = lt_entry->udf_policy_id;

    /* Allocate imm fields */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, lt_sid, &num_fields));

    num_fields += (MAX_CONT_COUNT - CONT_TYPE);

    SHR_IF_ERR_EXIT
        (udf_policy_lt_fields_allocate(unit,
                                       &imm_fields,
                                       num_fields));

    /* Lookup IMM table, error out if entry is not found */
    SHR_IF_ERR_EXIT
        (bcmimm_entry_lookup(unit,
                             lt_sid,
                             &key_fields,
                             &imm_fields));

    flist = &imm_fields;

    /* Parse IMM table data fields */
    for (i = 0; i < flist->count; i++) {
        data_field = flist->field[i];
        fid = data_field->id;
        idx = data_field->idx;

        switch (fid) {
            case UDF_POLICYt_OFFSETf:
                lt_entry->offset = data_field->data;
                break;
            case UDF_POLICYt_PACKET_HEADERf:
                lt_entry->packet_header = data_field->data;
                break;
            case UDF_POLICYt_CONTAINER_4_BYTEf:
                lt_entry->container_4_byte[idx] = data_field->data;
                break;
            case UDF_POLICYt_CONTAINER_2_BYTEf:
                lt_entry->container_2_byte[idx] = data_field->data;
                break;
            case UDF_POLICYt_CONTAINER_1_BYTEf:
                lt_entry->container_1_byte[idx] = data_field->data;
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    udf_anchor_info = bcm56780_a0_udf_anchor_info_get(unit, lt_entry->packet_header);
    if (udf_anchor_info == NULL) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "UDF feature not initialized for configured variant!\n")));
        SHR_ERR_EXIT(SHR_E_INIT);
    }
    hdr_fmt_hw_info = UDF_HDR_FMT_HW_INFO_GET(unit, lt_entry->packet_header);
    SHR_NULL_CHECK(hdr_fmt_hw_info, SHR_E_MEMORY);

    buf_ptr = (uint32_t *)(&hfe_policy_entry);
    sal_memset(&pt_info, 0, sizeof(bcmbd_pt_dyn_info_t));

    for (stage = 0; stage < udf_anchor_info->table_num; stage++) {
        index = stage * HFE_POLICY_TABLE_INDEX_MAX;
        tbl_type = udf_anchor_info->table[stage]->tbl_type;
        pt_id = hfe_policy_table[tbl_type];
        rsp_entry_wsize = bcmdrd_pt_entry_wsize(unit, pt_id);

        for (hfe_cnt = 0; hfe_cnt < udf_anchor_info->table[stage]->num_entries; hfe_cnt++) {
            pt_info.index =hdr_fmt_hw_info->hfe_profile_entry[index + hfe_cnt];

            /* Read PT table entry */
            SHR_IF_ERR_EXIT
            (bcmptm_ltm_mreq_indexed_lt(unit, 0, pt_id, &pt_info,
                                        NULL, NULL, /* misc_info */
                                        BCMPTM_OP_READ,
                                        NULL,
                                        rsp_entry_wsize,
                                        lt_sid,
                                        trans_id,
                                        NULL, NULL,
                                        buf_ptr,
                                        &rsp_ltid,
                                        &rsp_flags));

            cmd_bmap = hdr_fmt_hw_info->valid_cmd_bmap[index + pt_info.index];

            for (cmd = 0; cmd < HFE_POLICY_TABLE_MAX_COMMAND_PER_INDEX; cmd++) {
                if (cmd_bmap & (1 << cmd)) {
                    hfe_cmd = valid_cmds[cmd];

                    /* mark the command as invalid */
                    fld_val = 0;
                    bcmdrd_pt_field_set(unit, pt_id, buf_ptr, hfe_cmd.valid,
                                        &fld_val);

                    /* Clear destination container ID used for UDF extraction */
                    bcmdrd_pt_field_set(unit, pt_id, buf_ptr, hfe_cmd.dest_cont_id,
                                        &fld_val);

                    /* Clear offset used for UDF extraction */
                    bcmdrd_pt_field_set(unit, pt_id, buf_ptr, hfe_cmd.src_pkt_offset,
                                        &fld_val);

                    /* Write PT table entry */
                    SHR_IF_ERR_EXIT
                    (bcmptm_ltm_mreq_indexed_lt(unit, 0, pt_id, &pt_info,
                                                NULL, NULL, /* misc_info */
                                                BCMPTM_OP_WRITE,
                                                buf_ptr,
                                                0,
                                                lt_sid,
                                                trans_id,
                                                NULL, NULL, NULL,
                                                &rsp_ltid,
                                                &rsp_flags));
                }

            }

        }
    }

    /* clear SW state for specified UDF policy ID */
    hdr_fmt_hw_info->current_offset = 0;
    sal_memset( hdr_fmt_hw_info->valid_cmd_bmap, 0, sizeof( hdr_fmt_hw_info->valid_cmd_bmap));
    sal_memset(hdr_fmt_hw_info->cont_used_1b, 0, sizeof(hdr_fmt_hw_info->cont_used_1b));
    sal_memset(hdr_fmt_hw_info->cont_used_2b, 0, sizeof(hdr_fmt_hw_info->cont_used_2b));
    sal_memset(hdr_fmt_hw_info->cont_used_4b, 0, sizeof(hdr_fmt_hw_info->cont_used_4b));

exit:
    udf_policy_lt_fields_free(unit, 1, &key_fields);
    udf_policy_lt_fields_free(unit, num_fields, &imm_fields);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Install HW policy commands for UDF object.
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction ID for current operation
 * \param [in] ltid Logical Table enum for current operation
 * \param [in] lt_entry Pointer to bcmcth_udf_policy_info_t structure.
 * \param [in] cfg_data Pointer to udf_obj_hdr_fmt_hw_cfg_t structure.

 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Input parameter error.
 * \retval SHR_E_FAIL Hardware write operation unsuccessful.
 */
static int
bcm56780_a0_udf_policy_cmd_install(int unit,
                                   uint32_t trans_id,
                                   bcmltd_sid_t lt_sid,
                                   bcmcth_udf_policy_info_t *lt_entry,
                                   udf_obj_hdr_fmt_hw_cfg_t *cfg_data,
                                   uint32_t update)
{
    uint8_t stage;
    uint8_t hfe_cnt;
    uint8_t table_index = 0;
    uint32_t index = 0;
    uint8_t cmd_max = HFE_POLICY_TABLE_MAX_COMMAND_PER_INDEX;
    int idx = 0, i = 0;
    int cont_gran;
    int parser;
    int cmd_4B = 0, cmd_2B = 0, cmd_1B = 0;
    int num_4B_cont = 0, num_2B_cont = 0, num_1B_cont = 0;
    int container_list_4B[UDF_4_BYTE_CONTAINER_MAX] = {0};
    int container_list_2B[UDF_2_BYTE_CONTAINER_MAX] = {0};
    int container_list_1B[UDF_1_BYTE_CONTAINER_MAX] = {0};
    int cont_used_4B[UDF_4_BYTE_CONTAINER_MAX] = {0};
    int cont_used_2B[UDF_2_BYTE_CONTAINER_MAX] = {0};
    int cont_used_1B[UDF_1_BYTE_CONTAINER_MAX] = {0};
    uint32_t cmd_list_4B[HFE_POLICY_TABLE_MAX_COMMAND_PER_INDEX] = {0};
    uint32_t cmd_list_2B[HFE_POLICY_TABLE_MAX_COMMAND_PER_INDEX] = {0};
    uint32_t cmd_list_1B[HFE_POLICY_TABLE_MAX_COMMAND_PER_INDEX] = {0};
    bool cmd_valid_4B[HFE_POLICY_TABLE_MAX_COMMAND_PER_INDEX] = {0};
    bool cmd_valid_2B[HFE_POLICY_TABLE_MAX_COMMAND_PER_INDEX] = {0};
    bool cmd_valid_1B[HFE_POLICY_TABLE_MAX_COMMAND_PER_INDEX] = {0};
    uint32_t cont_4B_bmp = 0, cont_2B_bmp = 0, cont_1B_bmp = 0, cont_idx;
    container_type_t container_type;
    const udf_anchor_t *udf_anchor_info = NULL;
    udf_hdr_fmt_hw_info_t *hdr_fmt_hw_info = NULL;

    SHR_FUNC_ENTER(unit);


    udf_anchor_info = bcm56780_a0_udf_anchor_info_get(unit, lt_entry->packet_header);
    if (udf_anchor_info == NULL) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "UDF feature not initialized for configured variant!\n")));
        SHR_ERR_EXIT(SHR_E_INIT);
    }
    hdr_fmt_hw_info = UDF_HDR_FMT_HW_INFO_GET(unit, lt_entry->packet_header);
    SHR_NULL_CHECK(hdr_fmt_hw_info, SHR_E_MEMORY);

    parser = hdr_fmt_hw_info->parser;
    if (cfg_data->num_4b_cont) {
        container_type = CONTAINER_4_BYTE;
        num_4B_cont = cfg_data->num_4b_cont;
        SHR_IF_ERR_VERBOSE_EXIT(
        udf_available_container_get(unit, parser, container_type,
                                    num_4B_cont,
                                    container_list_4B));
        for (cont_idx = 0; (cont_idx < UDF_4_BYTE_CONTAINER_MAX &&
              num_4B_cont > 0); cont_idx++) {
            if ((container_list_4B[cont_idx]) &&
                (lt_entry->container_4_byte[cont_idx]) &&
                (!hdr_fmt_hw_info->cont_used_4b[cont_idx])) {
                BCMCTH_UDF_LT_FIELD_SET(&cont_4B_bmp, container_list_4B[cont_idx]);
                num_4B_cont--;
                cont_used_4B[i] = container_list_4B[cont_idx];
                hdr_fmt_hw_info->cont_used_4b[cont_idx] = container_list_4B[cont_idx];
                i++;
            }
        }
        if (!cont_4B_bmp && !update) {
            SHR_ERR_EXIT(SHR_E_RESOURCE);
        }
    }
    if (cfg_data->num_2b_cont) {
        container_type = CONTAINER_2_BYTE;
        num_2B_cont = cfg_data->num_2b_cont;
        i = 0;
        SHR_IF_ERR_VERBOSE_EXIT(
        udf_available_container_get(unit, parser, container_type,
                                    cfg_data->num_2b_cont,
                                    container_list_2B));
        for (cont_idx = 0; (cont_idx < UDF_2_BYTE_CONTAINER_MAX &&
              num_2B_cont > 0); cont_idx++) {
            if ((container_list_2B[cont_idx]) &&
                (lt_entry->container_2_byte[cont_idx]) &&
                (!hdr_fmt_hw_info->cont_used_2b[cont_idx])) {
                BCMCTH_UDF_LT_FIELD_SET(&cont_2B_bmp,
                    (container_list_2B[cont_idx] - UDF_4_BYTE_CONTAINER_MAX));
                num_2B_cont--;
                cont_used_2B[i] = container_list_2B[cont_idx];
                hdr_fmt_hw_info->cont_used_2b[cont_idx] = container_list_2B[cont_idx];
                i++;
            }
        }
        if (!cont_2B_bmp && !update) {
            SHR_ERR_EXIT(SHR_E_RESOURCE);
        }
    }
    if (cfg_data->num_1b_cont) {
        container_type = CONTAINER_1_BYTE;
        num_1B_cont = cfg_data->num_1b_cont;
        i = 0;
        SHR_IF_ERR_VERBOSE_EXIT(
        udf_available_container_get(unit, parser, container_type,
                                    cfg_data->num_1b_cont,
                                    container_list_1B));
        for (cont_idx = 0; (cont_idx < UDF_1_BYTE_CONTAINER_MAX &&
              num_1B_cont > 0); cont_idx++) {
            if ((container_list_1B[cont_idx]) &&
                (lt_entry->container_1_byte[cont_idx]) &&
                (!hdr_fmt_hw_info->cont_used_1b[cont_idx])) {
                BCMCTH_UDF_LT_FIELD_SET(&cont_1B_bmp, (container_list_1B[cont_idx]
                    - (UDF_4_BYTE_CONTAINER_MAX + UDF_2_BYTE_CONTAINER_MAX)));
                num_1B_cont--;
                cont_used_1B[i] = container_list_1B[cont_idx];
                hdr_fmt_hw_info->cont_used_1b[cont_idx] = container_list_1B[cont_idx];
                i++;
            }
        }
        if (!cont_1B_bmp && !update) {
            SHR_ERR_EXIT(SHR_E_RESOURCE);
        }
    }
    for (stage = 0; stage < udf_anchor_info->table_num; stage++) {
        index = stage * HFE_POLICY_TABLE_INDEX_MAX;
        for (hfe_cnt = 0; hfe_cnt < udf_anchor_info->table[stage]->num_entries; hfe_cnt++) {
            num_4B_cont = cfg_data->num_4b_cont;
            num_2B_cont = cfg_data->num_2b_cont;
            num_1B_cont = cfg_data->num_1b_cont;
            cont_gran = 4;
            table_index = hdr_fmt_hw_info->hfe_profile_entry[index + hfe_cnt];
            hdr_fmt_hw_info->current_offset = 0;
            if (num_4B_cont) {
                for (cmd_4B = 0; (cmd_4B < cmd_max && num_4B_cont > 0); cmd_4B++) {
                    if (cfg_data->cmd_bmap[stage][table_index] & 1 << cmd_4B) {
                         cmd_list_4B[idx] = cmd_4B;
                         cmd_valid_4B[idx] = TRUE;
                         num_4B_cont --;
                         idx++;
                    }
                }
                SHR_IF_ERR_VERBOSE_EXIT
                (configure_dest_cont(unit, lt_entry, stage,
                                     hfe_cnt, cont_gran,
                                     cmd_list_4B, cmd_valid_4B,
                                     cont_used_4B,
                                     udf_anchor_info,
                                     hdr_fmt_hw_info,
                                     trans_id, lt_sid));
            }
            if (num_2B_cont) {
                idx = 0;
                cont_gran = 2;
                for (cmd_2B = cmd_4B; (cmd_2B < cmd_max && num_2B_cont > 0); cmd_2B++) {
                    if (cfg_data->cmd_bmap[stage][table_index] & 1 << cmd_2B) {
                         cmd_list_2B[idx] = cmd_2B;
                         cmd_valid_2B[idx] = TRUE;
                         num_2B_cont --;
                         idx++;
                    }
                }
                SHR_IF_ERR_VERBOSE_EXIT
                (configure_dest_cont(unit, lt_entry, stage,
                                     hfe_cnt, cont_gran,
                                     cmd_list_2B,
                                     cmd_valid_2B,
                                     cont_used_2B,
                                     udf_anchor_info,
                                     hdr_fmt_hw_info,
                                     trans_id, lt_sid));
            }
            if (num_1B_cont) {
                idx = 0;
                cont_gran = 1;
                for (cmd_1B = cmd_2B; (cmd_1B < cmd_max && num_1B_cont > 0); cmd_1B++) {
                    if (cfg_data->cmd_bmap[stage][table_index] & 1 << cmd_1B) {
                        cmd_list_1B[idx] = cmd_1B;
                        cmd_valid_1B[idx] = TRUE;
                        num_1B_cont --;
                        idx++;
                    }
                }
                SHR_IF_ERR_VERBOSE_EXIT
                (configure_dest_cont(unit, lt_entry, stage,
                                     hfe_cnt, cont_gran,
                                     cmd_list_1B,
                                     cmd_valid_1B,
                                     cont_used_1B,
                                     udf_anchor_info,
                                     hdr_fmt_hw_info,
                                     trans_id, lt_sid));
            }

            hdr_fmt_hw_info->valid_cmd_bmap[index + table_index] |=
                cfg_data->cmd_bmap[stage][table_index];
        }
    }
    /* If UDF anchor is processed using parser1, then FIELD_MUX_PROFILE
     * table profile are programmed by default to select parser1 containers.
     * If UDF anchor is processed using parser2, then FIELD_MUX_PROFILE
     * table profile should be explicitly progrogramed to select parser2
     * containers.
     */
     if (parser == 2) {
        SHR_IF_ERR_VERBOSE_EXIT
        (configure_field_mux_cont(unit, cont_4B_bmp, cont_2B_bmp,
                                  cont_1B_bmp, trans_id, lt_sid));
     }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief This routine allocate HFE table commands for UDF policy
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry Pointer to bcmcth_udf_policy_info_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_RESOURCE Not enough hw resources available.
 * \retval SHR_E_PARAM Input parameter error.
 * \retval SHR_E_FAIL Hardware write operation unsuccessful.
 */
static int
bcm56780_a0_udf_policy_cmd_allocate(int unit,
                                    bcmcth_udf_policy_info_t *lt_entry,
                                    udf_obj_hdr_fmt_hw_cfg_t *cfg_data)
{
    uint8_t stage, hfe_cnt, num_cmds_req, num_cmds_remaining;
    uint8_t table_index;
    uint32_t index;
    uint8_t cmd_max = HFE_POLICY_TABLE_MAX_COMMAND_PER_INDEX;
    int cmd;
    int num_4B_cont = 0;
    int num_2B_cont = 0;
    int num_1B_cont = 0;
    const udf_anchor_t *udf_anchor_info;
    udf_hdr_fmt_hw_info_t *hdr_fmt_hw_info = NULL;

    SHR_FUNC_ENTER(unit);

    udf_anchor_info = bcm56780_a0_udf_anchor_info_get(unit, lt_entry->packet_header);
    if (udf_anchor_info == NULL) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "UDF feature not initialized for configured variant!\n")));
        SHR_ERR_EXIT(SHR_E_INIT);
    }
    hdr_fmt_hw_info = UDF_HDR_FMT_HW_INFO_GET(unit, lt_entry->packet_header);
    SHR_NULL_CHECK(hdr_fmt_hw_info, SHR_E_MEMORY);

    SHR_IF_ERR_EXIT
        (num_cmd_per_udf_policy(unit, lt_entry, &num_4B_cont,
                                &num_2B_cont, &num_1B_cont));
    num_cmds_req = num_4B_cont + num_2B_cont + num_1B_cont;
    cfg_data->num_4b_cont = num_4B_cont;
    cfg_data->num_2b_cont = num_2B_cont;
    cfg_data->num_1b_cont = num_1B_cont;

    for (stage = 0; stage < hdr_fmt_hw_info->num_stages; stage++) {
        index = stage * HFE_POLICY_TABLE_INDEX_MAX;
        for (hfe_cnt = 0; hfe_cnt < hdr_fmt_hw_info->hfe_index_count[stage]; hfe_cnt++) {
            num_cmds_remaining = num_cmds_req;
            table_index = hdr_fmt_hw_info->hfe_profile_entry[index + hfe_cnt];
            for (cmd = 0; (cmd < cmd_max && num_cmds_remaining > 0); cmd++) {
                if (bcm56780_a0_udf_is_hfe_index_available
                    (unit, udf_anchor_info->table[stage]->tbl_type,
                    udf_anchor_info->table[stage]->hfe_index_info[hfe_cnt].index,
                    cmd)) {
                    /* Allocate CMD if the bit is not set */
                    if ((hdr_fmt_hw_info->valid_cmd_bmap[index + table_index] &
                        (1 << cmd)) == 0) {
                        cfg_data->cmd_bmap[stage][table_index] |= (1 << cmd);
                        num_cmds_remaining--;
                    }
                }
            }
            /* num_cmds_required !=0 indicates no policy table commands are
             * available for udf container programming.
             */
            if (num_cmds_remaining) {
                SHR_ERR_EXIT(SHR_E_RESOURCE);
            }
            hdr_fmt_hw_info->valid_cmd_bmap[index + table_index] |=
                cfg_data->cmd_bmap[stage][table_index];
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Creates UDF policy
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction ID for current operation
 * \param [in] ltid Logical Table enum for current operation
 * \param [in] lt_entry Pointer to bcmcth_udf_policy_info_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_RESOURCE Not enough hw resources available.
 * \retval SHR_E_PARAM Input parameter error.
 * \retval SHR_E_FAIL Hardware write operation unsuccessful.
 */
static int
bcm56780_a0_cth_udf_policy_create(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t lt_sid,
    bcmcth_udf_policy_info_t *lt_entry,
    uint32_t update)
{
    udf_obj_hdr_fmt_hw_cfg_t *cfg_data = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    SHR_ALLOC(cfg_data, sizeof(udf_obj_hdr_fmt_hw_cfg_t), "bcmCthUDFCfgDataBuf");
    SHR_NULL_CHECK(cfg_data, SHR_E_MEMORY);
    sal_memset(cfg_data, 0, sizeof(udf_obj_hdr_fmt_hw_cfg_t));

    /* Allocate HFE table commands to configure UDF policy */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_udf_policy_cmd_allocate(unit,
                                             lt_entry,
                                             cfg_data));
    /* Install UDF entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_udf_policy_cmd_install(unit,
                                            trans_id,
                                            lt_sid,
                                            lt_entry,
                                            cfg_data,
                                            update));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_bus_scg_register_clear(unit,trans_id, lt_sid));


exit:
    SHR_FREE(cfg_data);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Deletes UDF policy
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction ID for current operation
 * \param [in] ltid Logical Table enum for current operation
 * \param [in] lt_entry Pointer to bcmcth_udf_policy_info_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_RESOURCE Not enough hw resources available.
 * \retval SHR_E_PARAM Input parameter error.
 * \retval SHR_E_FAIL Hardware write operation unsuccessful.
 */
static int
bcm56780_a0_cth_udf_policy_delete(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t lt_sid,
    bcmcth_udf_policy_info_t *lt_entry)
{

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    /* Uninstall UDF entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56780_a0_udf_policy_cmd_uninstall(unit,
                                              trans_id,
                                              lt_sid,
                                              lt_entry));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Deinitialize UDF module.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
bcm56780_a0_cth_udf_deinit(int unit)
{

    SHR_FUNC_ENTER(unit);
    if (udf_control[unit] != NULL) {
        SHR_FREE(udf_control[unit]);
        udf_control[unit] = NULL;
    }
    SHR_FUNC_EXIT();
}

static int
bcmcth_udf_policy_info_flds_free(int unit, bcmltd_fields_t *in)
{
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    if (in->field) {
        for (i = 0; i < in->count; i++) {
            if (in->field[i]) {
                SHR_FREE(in->field[i]);
            }
        }
        SHR_FREE(in->field);
    }

    SHR_FUNC_EXIT();
}

static int
bcmcth_udf_policy_info_flds_allocate(int unit, bcmltd_fields_t *in, size_t num_fields)
{
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    /* Allocate fields buffers */
    SHR_ALLOC(in->field, sizeof(bcmltd_field_t *) * num_fields, "bcmcthUDF");

    SHR_NULL_CHECK(in->field, SHR_E_MEMORY);

    sal_memset(in->field, 0, sizeof(bcmltd_field_t *) * num_fields);

    in->count = num_fields;

    for (i = 0; i < num_fields; i++) {
        SHR_ALLOC(in->field[i], sizeof(bcmltd_field_t), "bcmcthUDF");
        SHR_NULL_CHECK(in->field[i], SHR_E_MEMORY);
        sal_memset(in->field[i], 0, sizeof(bcmltd_field_t));
    }

    SHR_FUNC_EXIT();

exit:
    bcmcth_udf_policy_info_flds_free(unit, in);

    SHR_FUNC_EXIT();
}


/*!
 * \brief Get UDF anchor info.
 *
 * \param [in] unit Unit number.
 * \param [in] bool Indicates cold or warm start status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Input parameter error.
 * \retval SHR_E_FAIL Hardware write operation unsuccessful.
 */
static int
bcm56780_a0_cth_udf_policy_info_get(int unit, bool warm)
{
    bcmltd_fields_t info_lt_flds = {0};
    bcmlrd_table_attrib_t t_attrib;
    size_t num_fields = 0;
    size_t f_cnt;
    uint8_t anchor;
    udf_hdr_fmt_hw_info_t *anchor_info = NULL;
    int rv;
    int hdr_fmt_last_count;

    SHR_FUNC_ENTER(unit);

    rv = bcmlrd_table_attributes_get(unit,
                                     "UDF_POLICY_INFO",
                                     &t_attrib);

    if (rv != SHR_E_NONE) {
        SHR_EXIT();
    }

    num_fields = t_attrib.number_of_fields;

    SHR_IF_ERR_EXIT
        (bcmcth_udf_policy_info_flds_allocate(unit, &info_lt_flds, num_fields));

    hdr_fmt_last_count = UDF_HDR_FMT_COUNT_GET(unit);

    for (anchor = 0; anchor < hdr_fmt_last_count; anchor++) {
        anchor_info = UDF_HDR_FMT_HW_INFO_GET(unit, anchor);
        f_cnt = 0;

        info_lt_flds.field[f_cnt]->id   = UDF_POLICY_INFOt_UDF_POLICY_INFO_IDf;
        info_lt_flds.field[f_cnt]->data = anchor;
        f_cnt++;

        info_lt_flds.field[f_cnt]->id   = UDF_POLICY_INFOt_MAX_CONTAINERf;
        info_lt_flds.field[f_cnt]->data = anchor_info->max_commands;
        f_cnt++;

        info_lt_flds.field[f_cnt]->id   = UDF_POLICY_INFOt_NUM_CONTAINER_1_BYTEf;
        info_lt_flds.field[f_cnt]->data = anchor_info->num_1_byte_cont;
        f_cnt++;

        info_lt_flds.field[f_cnt]->id   = UDF_POLICY_INFOt_NUM_CONTAINER_2_BYTEf;
        info_lt_flds.field[f_cnt]->data = anchor_info->num_2_byte_cont;
        f_cnt++;

        info_lt_flds.field[f_cnt]->id   = UDF_POLICY_INFOt_NUM_CONTAINER_4_BYTEf;
        info_lt_flds.field[f_cnt]->data = anchor_info->num_4_byte_cont;
        f_cnt++;

        info_lt_flds.count = f_cnt;
        if (!warm) {
            SHR_IF_ERR_EXIT
                (bcmimm_entry_insert(unit,
                                     UDF_POLICY_INFOt,
                                     &info_lt_flds));
        }

    }

exit:
    bcmcth_udf_policy_info_flds_free(unit, &info_lt_flds);

    SHR_FUNC_EXIT();

}


/*!
 * \brief Configure UDF header format with default values.
 *
 * \param [in] unit Unit number.
 * \param [in] bool Indicates cold or warm start status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Memory allocation failure.
 */
static int
bcm56780_a0_udf_hdr_format_info_config(int unit, bool warm)
{
    uint16_t hdr_fmt;
    uint32_t index;
    uint8_t stage, hfe_cnt, cmd, max_commands = 0, prev_max_commands = 0;
    const udf_anchor_t *udf_anchor_info = NULL;
    udf_hdr_fmt_hw_info_t *hdr_fmt_hw_info = NULL;
    int hdr_fmt_last_count;
    const udf_cont_info_t *container_list_parser1_4B = NULL;
    const udf_cont_info_t *container_list_parser1_2B = NULL;
    const udf_cont_info_t *container_list_parser1_1B = NULL;
    const udf_cont_info_t *container_list_parser2_4B = NULL;
    const udf_cont_info_t *container_list_parser2_2B = NULL;
    const udf_cont_info_t *container_list_parser2_1B = NULL;
    size_t cont_parser1_1B_size = 0;
    size_t cont_parser1_2B_size = 0;
    size_t cont_parser1_4B_size = 0;
    size_t cont_parser2_1B_size = 0;
    size_t cont_parser2_2B_size = 0;
    size_t cont_parser2_4B_size = 0;

    SHR_FUNC_ENTER(unit);

    hdr_fmt_last_count = UDF_HDR_FMT_COUNT_GET(unit);
    for (hdr_fmt = 0; hdr_fmt < hdr_fmt_last_count; hdr_fmt++) {
        hdr_fmt_hw_info = UDF_HDR_FMT_HW_INFO_GET(unit, hdr_fmt);
        SHR_NULL_CHECK(hdr_fmt_hw_info, SHR_E_MEMORY);
        udf_anchor_info = bcm56780_a0_udf_anchor_info_get(unit, hdr_fmt);
        if (udf_anchor_info == NULL) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "UDF feature not initialized for configured variant!\n")));
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        /* populate hdr_fmt_hw_info structure with default udf anchor data */
        hdr_fmt_hw_info->num_stages = udf_anchor_info->table_num;
        prev_max_commands = 0;
        for (stage = 0; stage < hdr_fmt_hw_info->num_stages; stage++) {
            index = stage * HFE_POLICY_TABLE_INDEX_MAX;
            hdr_fmt_hw_info->parser = udf_anchor_info->table[stage]->parser;
            hdr_fmt_hw_info->hfe_policy_mem[stage] =
                udf_anchor_info->table[stage]->tbl_type;
            hdr_fmt_hw_info->hfe_index_count[stage] =
                udf_anchor_info->table[stage]->num_entries;
            for (hfe_cnt = 0;
                 hfe_cnt < hdr_fmt_hw_info->hfe_index_count[stage];
                 hfe_cnt++) {
                /* Store each hfe index for a parser stage */
                hdr_fmt_hw_info->hfe_profile_entry[index + hfe_cnt] =
                udf_anchor_info->table[stage]->hfe_index_info[hfe_cnt].index;

                /* Store each hfe index and base offset pair for a parser stage */
                hdr_fmt_hw_info->hfe_profile_index[index + hfe_cnt].index =
                udf_anchor_info->table[stage]->hfe_index_info[hfe_cnt].index;
                hdr_fmt_hw_info->hfe_profile_index[index + hfe_cnt].base_offset =
                udf_anchor_info->table[stage]->hfe_index_info[hfe_cnt].base_offset;

                max_commands = 0;
                for (cmd = 0; cmd < HFE_POLICY_TABLE_MAX_COMMAND_PER_INDEX; cmd++) {
                    if (bcm56780_a0_udf_is_hfe_index_available
                        (unit, udf_anchor_info->table[stage]->tbl_type,
                        udf_anchor_info->table[stage]->hfe_index_info[hfe_cnt].index,
                        cmd)) {
                        max_commands++;
                    }
                }

                if ((!prev_max_commands) || (prev_max_commands > max_commands)) {
                    prev_max_commands = max_commands;
                }
            }
        }

        hdr_fmt_hw_info->max_commands = prev_max_commands;

        bcmcth_udf_var_info[unit]->iparser1_1_byte_cont_get
                                   (&container_list_parser1_1B, &cont_parser1_1B_size);
        bcmcth_udf_var_info[unit]->iparser2_1_byte_cont_get
                                   (&container_list_parser2_1B, &cont_parser2_1B_size);
        hdr_fmt_hw_info->num_1_byte_cont = (cont_parser1_1B_size < cont_parser2_1B_size) ?
                                            cont_parser1_1B_size : cont_parser2_1B_size;

        bcmcth_udf_var_info[unit]->iparser1_2_byte_cont_get
                                   (&container_list_parser1_2B, &cont_parser1_2B_size);
        bcmcth_udf_var_info[unit]->iparser2_2_byte_cont_get
                                   (&container_list_parser2_2B, &cont_parser2_2B_size);
        hdr_fmt_hw_info->num_2_byte_cont = (cont_parser1_2B_size < cont_parser2_2B_size) ?
                                            cont_parser1_2B_size : cont_parser2_2B_size;

        bcmcth_udf_var_info[unit]->iparser1_4_byte_cont_get
                                   (&container_list_parser1_4B, &cont_parser1_4B_size);
        bcmcth_udf_var_info[unit]->iparser2_4_byte_cont_get
                                   (&container_list_parser2_4B, &cont_parser2_4B_size);
        hdr_fmt_hw_info->num_4_byte_cont = (cont_parser1_4B_size < cont_parser2_4B_size) ?
                                            cont_parser1_4B_size : cont_parser2_4B_size;
    }

    exit:
        SHR_FUNC_EXIT();

}

/*!
 * \brief Initialize UDF header format.
 *
 * \param [in] unit Unit number.
 * \param [in] bool Indicates cold or warm start status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Memory allocation failure.
 */
static int
bcm56780_a0_udf_hdr_format_init(int unit, bool warm)
{
    uint32_t ha_alloc_sz = 0; /* Size of allocated HA memory block. */
    uint32_t req_alloc_sz = 0; /* Size of allocated HA memory block. */
    uint32_t offset = 0;
    udf_hdr_fmt_ha_info_t *hdr_fmt_ha_ptr_start = NULL;
    udf_hdr_fmt_hw_info_t *hdr_fmt_ptr_start = NULL;
    udf_control_t *udf_control_desc;
    uint32_t i;
    uint32_t hdr_fmt_last_count;

    SHR_FUNC_ENTER(unit);

    /* allocate device descriptor */
    if (udf_control[unit] != NULL) {
        SHR_FREE(udf_control[unit]);
        udf_control[unit] = NULL;
    }
    udf_control_desc = NULL;
    SHR_ALLOC(udf_control_desc, sizeof(udf_control_t), "bcmCthUDFDescBuf");
    SHR_NULL_CHECK(udf_control_desc, SHR_E_MEMORY);
    sal_memset(udf_control_desc, 0, sizeof(udf_control_t));
    udf_control[unit] = udf_control_desc;

    hdr_fmt_last_count = UDF_HDR_FMT_COUNT_GET(unit);
    req_alloc_sz = sizeof(udf_hdr_fmt_ha_info_t) +
                   (sizeof(udf_hdr_fmt_hw_info_t) * hdr_fmt_last_count);
    ha_alloc_sz = req_alloc_sz;
    hdr_fmt_ha_ptr_start =
       shr_ha_mem_alloc(unit,
                        BCMMGMT_UDF_COMP_ID,
                        BCMCTH_UDF_POLICY_SUB_COMP_ID,
                        "bcmcthUdfHdrFmtInit",
                        &ha_alloc_sz);
    SHR_NULL_CHECK(hdr_fmt_ha_ptr_start, SHR_E_MEMORY);

    if (ha_alloc_sz < req_alloc_sz) {
        if (warm) {
            if (hdr_fmt_ha_ptr_start->max_hdr_count < hdr_fmt_last_count) {
                /* If number of element changes due to NPL change, resize is done below */
                hdr_fmt_ha_ptr_start = shr_ha_mem_realloc(unit,
                                                       hdr_fmt_ha_ptr_start,
                                                       req_alloc_sz);
                SHR_NULL_CHECK(hdr_fmt_ha_ptr_start, SHR_E_MEMORY);
                hdr_fmt_ha_ptr_start->max_hdr_count = hdr_fmt_last_count;
            }
        } else {
            SHR_VERBOSE_EXIT(SHR_E_MEMORY);
        }
    }

    if (!warm) {
        sal_memset(hdr_fmt_ha_ptr_start, 0, ha_alloc_sz);
        hdr_fmt_ha_ptr_start->max_hdr_count = hdr_fmt_last_count;
        SHR_IF_ERR_EXIT
            (bcmissu_struct_info_report(unit, BCMMGMT_UDF_COMP_ID,
                                             BCMCTH_UDF_POLICY_SUB_COMP_ID, 0,
                                             req_alloc_sz,
                                             1,
                                             UDF_HDR_FMT_HW_INFO_T_ID));
    } else {
        SHR_IF_ERR_EXIT
            (bcmissu_struct_info_get(unit,
                                     UDF_HDR_FMT_HW_INFO_T_ID,
                                     BCMMGMT_UDF_COMP_ID,
                                     BCMCTH_UDF_POLICY_SUB_COMP_ID,
                                     &offset));
        hdr_fmt_ha_ptr_start = (udf_hdr_fmt_ha_info_t *)
                               ((uint8_t *)hdr_fmt_ha_ptr_start + offset);
    }

    hdr_fmt_ptr_start = hdr_fmt_ha_ptr_start->udf_hdr_fmt_array;

    for (i = 0; i < hdr_fmt_last_count; i++) {
        udf_control[unit]->udf_hdr_fmt_ptr[i] = hdr_fmt_ptr_start++;
    }

    SHR_FUNC_EXIT();

exit:
    SHR_FREE(udf_control[unit]);
    udf_control[unit] = NULL;
    SHR_FUNC_EXIT();

}

/*!
 * \brief Initialize UDF module.
 *
 * \param [in] unit Unit number.
 * \param [in] bool Indicates cold or warm start status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Input parameter error.
 * \retval SHR_E_FAIL Hardware write operation unsuccessful.
 */
static int
bcm56780_a0_cth_udf_init(int unit,bool warm)
{

    SHR_FUNC_ENTER(unit);

    /*
     * Allocated memory for UDF packet header.
     */
    SHR_IF_ERR_EXIT(bcm56780_a0_udf_hdr_format_init(unit, warm));

    /*
     * Initialize udf header formats with offset and default values.
     */
    SHR_IF_ERR_EXIT(bcm56780_a0_udf_hdr_format_info_config(unit, warm));

exit:
    if (SHR_FUNC_ERR()) {
        if (udf_control[unit] != NULL) {
            shr_ha_mem_free(unit, udf_control[unit]);
            udf_control[unit] = NULL;
        }
    }
    SHR_FUNC_EXIT();

}

/*!
 * \brief Initialize UDF variant anchor info.
 *
 * \param [in] unit Unit number.
 * \param [in] bool Indicates cold or warm start status.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcm56780_a0_cth_udf_var_info_init(int unit,
                                  bool warm,
                                  bcmcth_udf_var_info_t *var_info)
{

    bcmcth_udf_var_info[unit] = var_info;

    return SHR_E_NONE;
}

/*!
 * \brief DeInitialize UDF variant anchor info.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcm56780_a0_cth_udf_var_info_deinit(int unit)
{

    bcmcth_udf_var_info[unit] = NULL;

    return SHR_E_NONE;

}

/*!
 * \brief Select UDF field mux profile.
 *
 * For tunnel termination case, default behavior is to populate containers
 * from tunnel payload (parser2). If user need data from tunnel header, then
 * field mux profile should be programmed to select parser1 container.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id LT Transaction Id
 * \param [in] lt_sid This is the logical table ID.
 * \param [in] lt_entry Pointer to LT entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_FULL Requested hardware resources unavailable..
 * \return SHR_E_EXISTS Entry already exists in hardware.
 */
static int
bcm56780_a0_cth_udf_field_mux_select_create(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t lt_sid,
    bcmcth_udf_field_mux_select_info_t *lt_entry)
{
    uint32_t cont_4B_bmp = 0, cont_2B_bmp = 0, cont_1B_bmp = 0;
    int index;
    const udf_cont_info_t *container_list_4B = NULL;
    const udf_cont_info_t *container_list_2B = NULL;
    const udf_cont_info_t *container_list_1B = NULL;
    size_t cont_1B_size = 0;
    size_t cont_2B_size = 0;
    size_t cont_4B_size = 0;
    int create = 1;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    bcmcth_udf_var_info[unit]->iparser2_1_byte_cont_get
                               (&container_list_1B, &cont_1B_size);
    bcmcth_udf_var_info[unit]->iparser2_2_byte_cont_get
                               (&container_list_2B, &cont_2B_size);
    bcmcth_udf_var_info[unit]->iparser2_4_byte_cont_get
                               (&container_list_4B, &cont_4B_size);


    if (container_list_4B != NULL) {
        for (index = 0; index < UDF_4_BYTE_CONTAINER_MAX; index++) {
            if (lt_entry->container_fwd_4_byte[index]) {
                cont_4B_bmp |= (1 << container_list_4B[index].cont_id);
            }
        }
    }

    if (container_list_2B != NULL) {
        for (index = 0; index < UDF_2_BYTE_CONTAINER_MAX; index++) {
            if (lt_entry->container_fwd_2_byte[index]) {
                cont_2B_bmp |= (1 << (container_list_2B[index].cont_id - UDF_4_BYTE_CONTAINER_MAX));
            }
        }
    }

    if (container_list_1B != NULL) {
        for (index = 0; index < UDF_1_BYTE_CONTAINER_MAX; index++) {
            if (lt_entry->container_fwd_1_byte[index]) {
                cont_1B_bmp |= (1 << (container_list_1B[index].cont_id -
                               (UDF_4_BYTE_CONTAINER_MAX + UDF_2_BYTE_CONTAINER_MAX)));
            }
        }
    }


    SHR_IF_ERR_VERBOSE_EXIT
    (configure_field_mux_profile(unit, cont_4B_bmp, cont_2B_bmp,
                                 cont_1B_bmp, lt_entry->udf_field_mux_select_id,
                                 trans_id, lt_sid, create));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete UDF field mux profile selection.
 *
 * For tunnel termination case, default behavior is to populate containers
 * from tunnel payload (parser2). If user need data from tunnel header, then
 * field mux profile should be programmed to select parser1 container. By deleting
 * selected profile, default behavior is restored.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id LT Transaction Id
 * \param [in] lt_sid This is the logical table ID.
 * \param [in] lt_entry Pointer to LT entry data.
 *
 * \return SHR_E_NONE No errors.
 * \return SHR_E_PARAM Input parameter error.
 * \return SHR_E_FULL Requested hardware resources unavailable..
 * \return SHR_E_EXISTS Entry already exists in hardware.
 */
static int
bcm56780_a0_cth_udf_field_mux_select_delete(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t lt_sid,
    bcmcth_udf_field_mux_select_info_t *lt_entry)
{

    uint32_t cont_4B_bmp = 0, cont_2B_bmp = 0, cont_1B_bmp = 0;
    int index;
    const udf_cont_info_t *container_list_4B = NULL;
    const udf_cont_info_t *container_list_2B = NULL;
    const udf_cont_info_t *container_list_1B = NULL;
    size_t cont_1B_size = 0;
    size_t cont_2B_size = 0;
    size_t cont_4B_size = 0;
    int delete = 0;
    bcmltd_fields_t key_fields = {0};
    bcmltd_fields_t imm_fields = {0};
    size_t num_fields, i;
    bcmltd_fields_t *flist;
    bcmltd_field_t *data_field;
    uint32_t fid;
    uint32_t idx;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    bcmcth_udf_var_info[unit]->iparser2_1_byte_cont_get
                               (&container_list_1B, &cont_1B_size);
    bcmcth_udf_var_info[unit]->iparser2_2_byte_cont_get
                               (&container_list_2B, &cont_2B_size);
    bcmcth_udf_var_info[unit]->iparser2_4_byte_cont_get
                               (&container_list_4B, &cont_4B_size);

    /* Allocate key field */
    SHR_IF_ERR_EXIT
        (udf_policy_lt_fields_allocate(unit,
                                       &key_fields,
                                       1));

    key_fields.field[0]->id   = UDF_FIELD_MUX_SELECTt_UDF_FIELD_MUX_SELECT_IDf;
    key_fields.field[0]->data = lt_entry->udf_field_mux_select_id;

    /* Allocate imm fields */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, lt_sid, &num_fields));

    num_fields += (MAX_CONT_COUNT - CONT_TYPE);

    SHR_IF_ERR_EXIT
        (udf_policy_lt_fields_allocate(unit,
                                       &imm_fields,
                                       num_fields));

    /* Lookup IMM table, error out if entry is not found */
    SHR_IF_ERR_EXIT
        (bcmimm_entry_lookup(unit,
                             lt_sid,
                             &key_fields,
                             &imm_fields));

    flist = &imm_fields;

    /* Parse IMM table data fields */
    for (i = 0; i < flist->count; i++) {
        data_field = flist->field[i];
        fid = data_field->id;
        idx = data_field->idx;

        switch (fid) {
            case UDF_FIELD_MUX_SELECTt_FORWARDING_CONTAINER_4_BYTEf:
                lt_entry->container_fwd_4_byte[idx] = data_field->data;
                break;
            case UDF_FIELD_MUX_SELECTt_FORWARDING_CONTAINER_2_BYTEf:
                lt_entry->container_fwd_2_byte[idx] = data_field->data;
                break;
            case UDF_FIELD_MUX_SELECTt_FORWARDING_CONTAINER_1_BYTEf:
                lt_entry->container_fwd_1_byte[idx] = data_field->data;
                break;
            case UDF_FIELD_MUX_SELECTt_ALTERNATE_CONTAINER_2_BYTEf:
                lt_entry->container_alt_2_byte[idx] = data_field->data;
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    if (container_list_4B != NULL) {
        for (index = 0; index < UDF_4_BYTE_CONTAINER_MAX; index++) {
            if (lt_entry->container_fwd_4_byte[index]) {
                cont_4B_bmp |= (1 << container_list_4B[index].cont_id);
            }
        }
    }

    if (container_list_2B != NULL) {
        for (index = 0; index < UDF_2_BYTE_CONTAINER_MAX; index++) {
            if (lt_entry->container_fwd_2_byte[index]) {
                cont_2B_bmp |= (1 << (container_list_2B[index].cont_id - UDF_4_BYTE_CONTAINER_MAX));
            }
        }
    }

    if (container_list_1B != NULL) {
        for (index = 0; index < UDF_1_BYTE_CONTAINER_MAX; index++) {
            if (lt_entry->container_fwd_1_byte[index]) {
                cont_1B_bmp |= (1 << (container_list_1B[index].cont_id -
                               (UDF_4_BYTE_CONTAINER_MAX + UDF_2_BYTE_CONTAINER_MAX)));
            }
        }
    }


    SHR_IF_ERR_VERBOSE_EXIT
    (configure_field_mux_profile(unit, cont_4B_bmp, cont_2B_bmp,
                                 cont_1B_bmp, lt_entry->udf_field_mux_select_id,
                                 trans_id, lt_sid, delete));

exit:
    udf_policy_lt_fields_free(unit, 1, &key_fields);
    udf_policy_lt_fields_free(unit, num_fields, &imm_fields);
    SHR_FUNC_EXIT();

}

static bcmcth_udf_drv_t bcm56780_a0_cth_udf_drv = {
   .drv_init = &bcm56780_a0_cth_udf_init,
   .drv_policy_create = &bcm56780_a0_cth_udf_policy_create,
   .drv_policy_delete = &bcm56780_a0_cth_udf_policy_delete,
   .drv_deinit = &bcm56780_a0_cth_udf_deinit,
   .drv_policy_info_get = &bcm56780_a0_cth_udf_policy_info_get,
   .var_info_init = &bcm56780_a0_cth_udf_var_info_init,
   .var_info_deinit = &bcm56780_a0_cth_udf_var_info_deinit,
   .drv_field_mux_select_create = &bcm56780_a0_cth_udf_field_mux_select_create,
   .drv_field_mux_select_update = &bcm56780_a0_cth_udf_field_mux_select_create,
   .drv_field_mux_select_delete = &bcm56780_a0_cth_udf_field_mux_select_delete,
};

/*******************************************************************************
 * Public functions
 */

bcmcth_udf_drv_t *
bcm56780_a0_cth_udf_drv_get(int unit)
{
    return &bcm56780_a0_cth_udf_drv;
}
