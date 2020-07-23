/*! \file bcmpc_imm_mac_control.c
 *
 * PC_MAC_CONTROLt IMM handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_table.h>


#include <bcmpc/bcmpc_imm_internal.h>
#include <bcmpc/bcmpc_lport.h>
#include <bcmpc/bcmpc_lport_internal.h>
#include <bcmpc/bcmpc_pmgr_internal.h>
#include <bcmpc/bcmpc_pm_internal.h>
#include <bcmpc/bcmpc_cfg_internal.h>

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_IMM

/*! Logical table ID for this handler. */
#define IMM_SID PC_MAC_CONTROLt

/*! The data structure for PC_MAC_CONTROLt entry. */
typedef struct pc_mac_control_entry_s {
    bcmpc_lport_t key;
    bcmpc_mac_control_t data;
} pc_mac_control_entry_t;


/*******************************************************************************
 * Private Functions
 */

 /*!
  * \brief Initialize pc_mac_control_entry_t data buffer.
  *
  * \param [in] entry entry buffer.
  *
  * \return Nothing.
  */
static void
pc_mac_control_entry_t_init(pc_mac_control_entry_t *entry)
{
    entry->key = BCMPC_INVALID_LPORT;
    bcmpc_mac_control_t_init(&entry->data);
}

/*!
 * \brief Initialize RSV (Receive Statistic Vector) fields value
 * as SW default value.
 *
 * This initialization is only used for Insert operation.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry PC_MAC_CONTROL LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Input parameter failed validation check.
 */
static int
pc_mac_ctrl_lt_rsv_default_values_init(int unit,
                                       pc_mac_control_entry_t *lt_entry)
{
    uint64_t def_val = 0;
    uint32_t num;
    bcmpc_mac_control_t *mac_ctrl = NULL;
    SHR_FUNC_ENTER(unit);

    /* validate input parameter. */
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    mac_ctrl = &(lt_entry->data);

    if (!BCMPC_LT_FIELD_GET(mac_ctrl->rsv_fbmp,
                            BCMPC_MAC_RSV_WRONG_SA_PKTS)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_MAC_CONTROLt,
                                     PC_MAC_CONTROLt_PURGE_WRONG_SAf,
                                     1, &def_val, &num));
        mac_ctrl->mac_rsv_ctrl |=  (def_val << BCMPC_MAC_RSV_WRONG_SA_PKTS);
    }

    if (!BCMPC_LT_FIELD_GET(mac_ctrl->rsv_fbmp,
                            BCMPC_MAC_RSV_STACK_VLAN_PKTS)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_MAC_CONTROLt,
                                     PC_MAC_CONTROLt_PURGE_STACK_VLAN_FRAMESf,
                                     1, &def_val, &num));
        mac_ctrl->mac_rsv_ctrl |=  (def_val << BCMPC_MAC_RSV_STACK_VLAN_PKTS);
    }

    if (!BCMPC_LT_FIELD_GET(mac_ctrl->rsv_fbmp,
                            BCMPC_MAC_RSV_UNSUPPORTED_PAUSE_PFC_DA_PKTS)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_MAC_CONTROLt,
                            PC_MAC_CONTROLt_PURGE_UNSUPPORTED_PAUSE_PFC_DAf,
                            1, &def_val, &num));
        mac_ctrl->mac_rsv_ctrl |=
                      (def_val << BCMPC_MAC_RSV_UNSUPPORTED_PAUSE_PFC_DA_PKTS);
    }

    if (!BCMPC_LT_FIELD_GET(mac_ctrl->rsv_fbmp,
                            BCMPC_MAC_RSV_RX_CODE_ERROR_PKTS)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_MAC_CONTROLt,
                                  PC_MAC_CONTROLt_PURGE_RX_CODE_ERROR_FRAMESf,
                                  1, &def_val, &num));
        mac_ctrl->mac_rsv_ctrl |= (def_val << BCMPC_MAC_RSV_RX_CODE_ERROR_PKTS);
    }

    if (!BCMPC_LT_FIELD_GET(mac_ctrl->rsv_fbmp,
                            BCMPC_MAC_RSV_CRC_ERROR_PKTS)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_MAC_CONTROLt,
                                  PC_MAC_CONTROLt_PURGE_CRC_ERROR_FRAMESf,
                                  1, &def_val, &num));
        mac_ctrl->mac_rsv_ctrl |= (def_val << BCMPC_MAC_RSV_CRC_ERROR_PKTS);
    }

    if (!BCMPC_LT_FIELD_GET(mac_ctrl->rsv_fbmp,
                            BCMPC_MAC_RSV_LENGTH_CHECK_FAIL_PKTS)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_MAC_CONTROLt,
                              PC_MAC_CONTROLt_PURGE_LENGTH_CHECK_FAIL_FRAMESf,
                              1, &def_val, &num));
        mac_ctrl->mac_rsv_ctrl |=
                             (def_val << BCMPC_MAC_RSV_LENGTH_CHECK_FAIL_PKTS);
    }

    if (!BCMPC_LT_FIELD_GET(mac_ctrl->rsv_fbmp,
                            BCMPC_MAC_RSV_TRUNCATED_PKTS)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_MAC_CONTROLt,
                                     PC_MAC_CONTROLt_PURGE_TRUNCATED_FRAMESf,
                                     1, &def_val, &num));
        mac_ctrl->mac_rsv_ctrl |= (def_val << BCMPC_MAC_RSV_TRUNCATED_PKTS);
    }

    if (!BCMPC_LT_FIELD_GET(mac_ctrl->rsv_fbmp,
                            BCMPC_MAC_RSV_GOOD_PKTS)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_MAC_CONTROLt,
                                     PC_MAC_CONTROLt_PURGE_GOOD_FRAMESf,
                                     1, &def_val, &num));
        mac_ctrl->mac_rsv_ctrl |= (def_val << BCMPC_MAC_RSV_GOOD_PKTS);
    }

    if (!BCMPC_LT_FIELD_GET(mac_ctrl->rsv_fbmp,
                            BCMPC_MAC_RSV_MULTICAST_PKTS)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_MAC_CONTROLt,
                                     PC_MAC_CONTROLt_PURGE_MULTICAST_FRAMESf,
                                     1, &def_val, &num));
        mac_ctrl->mac_rsv_ctrl |= (def_val << BCMPC_MAC_RSV_MULTICAST_PKTS);
    }

    if (!BCMPC_LT_FIELD_GET(mac_ctrl->rsv_fbmp,
                            BCMPC_MAC_RSV_BROADCAST_PKTS)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_MAC_CONTROLt,
                                     PC_MAC_CONTROLt_PURGE_BROADCAST_FRAMESf,
                                     1, &def_val, &num));
        mac_ctrl->mac_rsv_ctrl |= (def_val << BCMPC_MAC_RSV_BROADCAST_PKTS);
    }

    if (!BCMPC_LT_FIELD_GET(mac_ctrl->rsv_fbmp,
                            BCMPC_MAC_RSV_PROMISCUOUS_PKTS)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_MAC_CONTROLt,
                                     PC_MAC_CONTROLt_PURGE_PROMISCUOUS_FRAMESf,
                                     1, &def_val, &num));
        mac_ctrl->mac_rsv_ctrl |= (def_val << BCMPC_MAC_RSV_PROMISCUOUS_PKTS);
    }

    if (!BCMPC_LT_FIELD_GET(mac_ctrl->rsv_fbmp,
                            BCMPC_MAC_RSV_CONTROL_PKTS)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_MAC_CONTROLt,
                                     PC_MAC_CONTROLt_PURGE_CONTROL_FRAMESf,
                                     1, &def_val, &num));
        mac_ctrl->mac_rsv_ctrl |= (def_val << BCMPC_MAC_RSV_CONTROL_PKTS);
    }

    if (!BCMPC_LT_FIELD_GET(mac_ctrl->rsv_fbmp,
                            BCMPC_MAC_RSV_PAUSE_PKTS)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_MAC_CONTROLt,
                                     PC_MAC_CONTROLt_PURGE_PAUSE_FRAMESf,
                                     1, &def_val, &num));
        mac_ctrl->mac_rsv_ctrl |= (def_val << BCMPC_MAC_RSV_PAUSE_PKTS);
    }

    if (!BCMPC_LT_FIELD_GET(mac_ctrl->rsv_fbmp,
                            BCMPC_MAC_RSV_BAD_OPCODE_PKTS)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_MAC_CONTROLt,
                                     PC_MAC_CONTROLt_PURGE_BAD_OPCODE_FRAMESf,
                                     1, &def_val, &num));
        mac_ctrl->mac_rsv_ctrl |= (def_val << BCMPC_MAC_RSV_BAD_OPCODE_PKTS);
    }

    if (!BCMPC_LT_FIELD_GET(mac_ctrl->rsv_fbmp,
                            BCMPC_MAC_RSV_VLAN_TAGGED_PKTS)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_MAC_CONTROLt,
                                     PC_MAC_CONTROLt_PURGE_VLAN_TAGGED_FRAMESf,
                                     1, &def_val, &num));
        mac_ctrl->mac_rsv_ctrl |= (def_val << BCMPC_MAC_RSV_VLAN_TAGGED_PKTS);
    }

    if (!BCMPC_LT_FIELD_GET(mac_ctrl->rsv_fbmp,
                            BCMPC_MAC_RSV_UNICAST_PKTS)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_MAC_CONTROLt,
                                     PC_MAC_CONTROLt_PURGE_UNICAST_FRAMESf,
                                     1, &def_val, &num));
        mac_ctrl->mac_rsv_ctrl |= (def_val << BCMPC_MAC_RSV_UNICAST_PKTS);
    }

    if (!BCMPC_LT_FIELD_GET(mac_ctrl->rsv_fbmp,
                            BCMPC_MAC_RSV_RX_FIFO_FULL)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_MAC_CONTROLt,
                                     PC_MAC_CONTROLt_RX_FIFO_FULLf,
                                     1, &def_val, &num));
        mac_ctrl->mac_rsv_ctrl |= (def_val << BCMPC_MAC_RSV_RX_FIFO_FULL);
    }

    if (!BCMPC_LT_FIELD_GET(mac_ctrl->rsv_fbmp,
                            BCMPC_MAC_RSV_RUNT_PKTS)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_MAC_CONTROLt,
                                     PC_MAC_CONTROLt_PURGE_RUNT_FRAMESf,
                                     1, &def_val, &num));
        mac_ctrl->mac_rsv_ctrl |= (def_val << BCMPC_MAC_RSV_RUNT_PKTS);
    }

    if (!BCMPC_LT_FIELD_GET(mac_ctrl->rsv_fbmp,
                            BCMPC_MAC_RSV_PFC_PKTS)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_MAC_CONTROLt,
                                     PC_MAC_CONTROLt_PURGE_PFC_FRAMESf,
                                     1, &def_val, &num));
        mac_ctrl->mac_rsv_ctrl |= (def_val << BCMPC_MAC_RSV_PFC_PKTS);
    }

    if (!BCMPC_LT_FIELD_GET(mac_ctrl->rsv_fbmp,
                            BCMPC_MAC_RSV_SCH_CRC_ERROR)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_MAC_CONTROLt,
                                     PC_MAC_CONTROLt_PURGE_SCH_CRC_ERRORf,
                                     1, &def_val, &num));
        mac_ctrl->mac_rsv_ctrl |= (def_val << BCMPC_MAC_RSV_SCH_CRC_ERROR);
    }

    if (!BCMPC_LT_FIELD_GET(mac_ctrl->rsv_fbmp,
                            BCMPC_MAC_RSV_MACSEC_PKTS)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_MAC_CONTROLt,
                                     PC_MAC_CONTROLt_PURGE_MACSEC_FRAMESf,
                                     1, &def_val, &num));
        mac_ctrl->mac_rsv_ctrl |= (def_val << BCMPC_MAC_RSV_MACSEC_PKTS);
    }

    if (!BCMPC_LT_FIELD_GET(mac_ctrl->rsv_fbmp,
                            BCMPC_MAC_RSV_DRIBBLE_NIBBLE_ERROR)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_MAC_CONTROLt,
                          PC_MAC_CONTROLt_PURGE_DRIBBLE_NIBBLE_ERROR_FRAMESf,
                          1, &def_val, &num));
        mac_ctrl->mac_rsv_ctrl |=
                             (def_val << BCMPC_MAC_RSV_DRIBBLE_NIBBLE_ERROR);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Fill PC structure RSV element according to the given LT field value.
 *
 * \param [in] unit Unit number.
 * \param [in] fid Field ID.
 * \param [in] fval Field value.
 * \param [in,out] entry Entry buffer.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int
pc_mac_control_rsv_entry_set(int unit, uint32_t fid, uint64_t fval,
                             pc_mac_control_entry_t *entry)
{
    bcmpc_mac_control_t *config = &entry->data;

    SHR_FUNC_ENTER(unit);

    switch (fid) {
        case PC_MAC_CONTROLt_PURGE_DRIBBLE_NIBBLE_ERROR_FRAMESf:
            config->mac_rsv_ctrl |=
                                 (fval << BCMPC_MAC_RSV_DRIBBLE_NIBBLE_ERROR);
            BCMPC_LT_FIELD_SET(config->rsv_fbmp,
                               BCMPC_MAC_RSV_DRIBBLE_NIBBLE_ERROR);
            break;
        case PC_MAC_CONTROLt_PURGE_MACSEC_FRAMESf:
            config->mac_rsv_ctrl |= (fval << BCMPC_MAC_RSV_MACSEC_PKTS);
            BCMPC_LT_FIELD_SET(config->rsv_fbmp, BCMPC_MAC_RSV_MACSEC_PKTS);
            break;
        case PC_MAC_CONTROLt_PURGE_SCH_CRC_ERRORf:
            config->mac_rsv_ctrl |= (fval << BCMPC_MAC_RSV_SCH_CRC_ERROR);
            BCMPC_LT_FIELD_SET(config->rsv_fbmp, BCMPC_MAC_RSV_SCH_CRC_ERROR);
            break;
        case PC_MAC_CONTROLt_PURGE_PFC_FRAMESf:
            config->mac_rsv_ctrl |= (fval << BCMPC_MAC_RSV_PFC_PKTS);
            BCMPC_LT_FIELD_SET(config->rsv_fbmp, BCMPC_MAC_RSV_PFC_PKTS);
            break;
        case PC_MAC_CONTROLt_PURGE_RUNT_FRAMESf:
            config->mac_rsv_ctrl |= (fval << BCMPC_MAC_RSV_RUNT_PKTS);
            BCMPC_LT_FIELD_SET(config->rsv_fbmp, BCMPC_MAC_RSV_RUNT_PKTS);
            break;
        case PC_MAC_CONTROLt_RX_FIFO_FULLf:
            config->mac_rsv_ctrl |= (fval << BCMPC_MAC_RSV_RX_FIFO_FULL);
            BCMPC_LT_FIELD_SET(config->rsv_fbmp, BCMPC_MAC_RSV_RX_FIFO_FULL);
            break;
        case PC_MAC_CONTROLt_PURGE_UNICAST_FRAMESf:
            config->mac_rsv_ctrl |= (fval << BCMPC_MAC_RSV_UNICAST_PKTS);
            BCMPC_LT_FIELD_SET(config->rsv_fbmp, BCMPC_MAC_RSV_UNICAST_PKTS);
            break;
        case PC_MAC_CONTROLt_PURGE_VLAN_TAGGED_FRAMESf:
            config->mac_rsv_ctrl |= (fval << BCMPC_MAC_RSV_VLAN_TAGGED_PKTS);
            BCMPC_LT_FIELD_SET(config->rsv_fbmp,
                               BCMPC_MAC_RSV_VLAN_TAGGED_PKTS);
            break;
        case PC_MAC_CONTROLt_PURGE_BAD_OPCODE_FRAMESf:
            config->mac_rsv_ctrl |= (fval << BCMPC_MAC_RSV_BAD_OPCODE_PKTS);
            BCMPC_LT_FIELD_SET(config->rsv_fbmp,
                               BCMPC_MAC_RSV_BAD_OPCODE_PKTS);
            break;
        case PC_MAC_CONTROLt_PURGE_PAUSE_FRAMESf:
            config->mac_rsv_ctrl |= (fval << BCMPC_MAC_RSV_PAUSE_PKTS);
            BCMPC_LT_FIELD_SET(config->rsv_fbmp, BCMPC_MAC_RSV_PAUSE_PKTS);
            break;
        case PC_MAC_CONTROLt_PURGE_CONTROL_FRAMESf:
            config->mac_rsv_ctrl |= (fval << BCMPC_MAC_RSV_CONTROL_PKTS);
            BCMPC_LT_FIELD_SET(config->rsv_fbmp, BCMPC_MAC_RSV_CONTROL_PKTS);
            break;
        case PC_MAC_CONTROLt_PURGE_PROMISCUOUS_FRAMESf:
            config->mac_rsv_ctrl |= (fval << BCMPC_MAC_RSV_PROMISCUOUS_PKTS);
            BCMPC_LT_FIELD_SET(config->rsv_fbmp,
                               BCMPC_MAC_RSV_PROMISCUOUS_PKTS);
            break;
        case PC_MAC_CONTROLt_PURGE_BROADCAST_FRAMESf:
            config->mac_rsv_ctrl |= (fval << BCMPC_MAC_RSV_BROADCAST_PKTS);
            BCMPC_LT_FIELD_SET(config->rsv_fbmp, BCMPC_MAC_RSV_BROADCAST_PKTS);
            break;
        case PC_MAC_CONTROLt_PURGE_MULTICAST_FRAMESf:
            config->mac_rsv_ctrl |= (fval << BCMPC_MAC_RSV_MULTICAST_PKTS);
            BCMPC_LT_FIELD_SET(config->rsv_fbmp, BCMPC_MAC_RSV_MULTICAST_PKTS);
            break;
        case PC_MAC_CONTROLt_PURGE_GOOD_FRAMESf:
            config->mac_rsv_ctrl |= (fval << BCMPC_MAC_RSV_GOOD_PKTS);
            BCMPC_LT_FIELD_SET(config->rsv_fbmp, BCMPC_MAC_RSV_GOOD_PKTS);
            break;
        case PC_MAC_CONTROLt_PURGE_TRUNCATED_FRAMESf:
            config->mac_rsv_ctrl |= (fval << BCMPC_MAC_RSV_TRUNCATED_PKTS);
            BCMPC_LT_FIELD_SET(config->rsv_fbmp, BCMPC_MAC_RSV_TRUNCATED_PKTS);
            break;
        case PC_MAC_CONTROLt_PURGE_LENGTH_CHECK_FAIL_FRAMESf:
            config->mac_rsv_ctrl |=
                             (fval << BCMPC_MAC_RSV_LENGTH_CHECK_FAIL_PKTS);
            BCMPC_LT_FIELD_SET(config->rsv_fbmp,
                               BCMPC_MAC_RSV_LENGTH_CHECK_FAIL_PKTS);
            break;
        case PC_MAC_CONTROLt_PURGE_CRC_ERROR_FRAMESf:
            config->mac_rsv_ctrl |= (fval << BCMPC_MAC_RSV_CRC_ERROR_PKTS);
            BCMPC_LT_FIELD_SET(config->rsv_fbmp, BCMPC_MAC_RSV_CRC_ERROR_PKTS);
            break;
        case PC_MAC_CONTROLt_PURGE_RX_CODE_ERROR_FRAMESf:
            config->mac_rsv_ctrl |= (fval << BCMPC_MAC_RSV_RX_CODE_ERROR_PKTS);
            BCMPC_LT_FIELD_SET(config->rsv_fbmp,
                               BCMPC_MAC_RSV_RX_CODE_ERROR_PKTS);
            break;
        case PC_MAC_CONTROLt_PURGE_UNSUPPORTED_PAUSE_PFC_DAf:
            config->mac_rsv_ctrl |=
                        (fval << BCMPC_MAC_RSV_UNSUPPORTED_PAUSE_PFC_DA_PKTS);
            BCMPC_LT_FIELD_SET(config->rsv_fbmp,
                               BCMPC_MAC_RSV_UNSUPPORTED_PAUSE_PFC_DA_PKTS);
            break;
        case PC_MAC_CONTROLt_PURGE_STACK_VLAN_FRAMESf:
            config->mac_rsv_ctrl |= (fval << BCMPC_MAC_RSV_STACK_VLAN_PKTS);
            BCMPC_LT_FIELD_SET(config->rsv_fbmp,
                               BCMPC_MAC_RSV_STACK_VLAN_PKTS);
            break;
        case PC_MAC_CONTROLt_PURGE_WRONG_SAf:
            config->mac_rsv_ctrl |= (fval << BCMPC_MAC_RSV_WRONG_SA_PKTS);
            BCMPC_LT_FIELD_SET(config->rsv_fbmp, BCMPC_MAC_RSV_WRONG_SA_PKTS);
            break;
        default:
            SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Fill PC structure according to the given LT field value.
 *
 * \param [in] unit Unit number.
 * \param [in] fid Field ID.
 * \param [in] fval Field value.
 * \param [in,out] entry Entry buffer.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int
pc_mac_control_entry_set(int unit, uint32_t fid, uint64_t fval,
                                pc_mac_control_entry_t *entry)
{
    bcmpc_mac_control_t *config = &entry->data;

    SHR_FUNC_ENTER(unit);

    switch (fid) {
        case PC_MAC_CONTROLt_PORT_IDf:
            entry->key = fval;
            break;
        case PC_MAC_CONTROLt_OVERSIZE_PKTf:
            config->oversize_pkt = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_MAC_CTRL_LT_FLD_OVERSIZE_PKT);
            break;
        case PC_MAC_CONTROLt_RX_ENABLE_AUTOf:
            config->rx_enable_auto = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_MAC_CTRL_LT_FLD_RX_ENABLE_AUTO);
            break;
        case PC_MAC_CONTROLt_RX_ENABLEf:
            config->rx_enable = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_MAC_CTRL_LT_FLD_RX_ENABLE);
            break;
        case PC_MAC_CONTROLt_TX_ENABLE_AUTOf:
            config->tx_enable_auto = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_MAC_CTRL_LT_FLD_TX_ENABLE_AUTO);
            break;
        case PC_MAC_CONTROLt_TX_ENABLEf:
            config->tx_enable = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_MAC_CTRL_LT_FLD_TX_ENABLE);
            break;
        case PC_MAC_CONTROLt_RUNT_THRESHOLD_AUTOf:
            config->runt_threshold_auto = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_MAC_CTRL_LT_FLD_RUNT_THRESHOLD_AUTO);
            break;
        case PC_MAC_CONTROLt_RUNT_THRESHOLDf:
            config->runt_threshold = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_MAC_CTRL_LT_FLD_RUNT_THRESHOLD);
            break;
        case PC_MAC_CONTROLt_LOCAL_FAULT_DISABLEf:
            config->local_fault_dis = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_MAC_CTRL_LT_FLD_LOCAL_FAULT_DISABLE);
            break;
        case PC_MAC_CONTROLt_REMOTE_FAULT_DISABLEf:
            config->rmt_fault_dis = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_MAC_CTRL_LT_FLD_REMOTE_FAULT_DISABLE);
            break;
        case PC_MAC_CONTROLt_INTER_FRAME_GAP_AUTOf:
            config->ifg_auto = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_MAC_CTRL_LT_FLD_INTER_FRAME_GAP_AUTO);
            break;
        case PC_MAC_CONTROLt_INTER_FRAME_GAPf:
            config->ifg = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_MAC_CTRL_LT_FLD_INTER_FRAME_GAP);
            break;
        case PC_MAC_CONTROLt_PAUSE_ADDRf:
            sal_memcpy(config->pause_addr, &fval, sizeof(config->pause_addr));
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_MAC_CTRL_LT_FLD_PAUSE_ADDR);
            break;
        case PC_MAC_CONTROLt_PAUSE_TXf:
            config->pause_tx = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_MAC_CTRL_LT_FLD_PAUSE_TX);
            break;
        case PC_MAC_CONTROLt_PAUSE_RXf:
            config->pause_rx = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_MAC_CTRL_LT_FLD_PAUSE_RX);
            break;
        case PC_MAC_CONTROLt_PAUSE_PASSf:
            config->pause_pass = fval;
            BCMPC_LT_FIELD_SET(config->fbmp, BCMPC_MAC_CTRL_LT_FLD_PAUSE_PASS);
            break;
        case PC_MAC_CONTROLt_CONTROL_PASSf:
            config->control_pass = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_MAC_CTRL_LT_FLD_CONTROL_PASS);
            break;
        case PC_MAC_CONTROLt_MAC_ECC_INTR_ENABLEf:
            config->mac_ecc_intr = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_MAC_CTRL_LT_FLD_MAC_ECC_INTR_ENABLE);
            break;
        case PC_MAC_CONTROLt_STALL_TXf:
            config->stall_tx = fval;
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_MAC_CTRL_LT_FLD_STALL_TX);
            break;
        case PC_MAC_CONTROLt_OPERATIONAL_STATEf:
            config->oper_state = fval;
            break;
        case PC_MAC_CONTROLt_PURGE_DRIBBLE_NIBBLE_ERROR_FRAMESf:
        case PC_MAC_CONTROLt_PURGE_MACSEC_FRAMESf:
        case PC_MAC_CONTROLt_PURGE_SCH_CRC_ERRORf:
        case PC_MAC_CONTROLt_PURGE_PFC_FRAMESf:
        case PC_MAC_CONTROLt_PURGE_RUNT_FRAMESf:
        case PC_MAC_CONTROLt_RX_FIFO_FULLf:
        case PC_MAC_CONTROLt_PURGE_UNICAST_FRAMESf:
        case PC_MAC_CONTROLt_PURGE_VLAN_TAGGED_FRAMESf:
        case PC_MAC_CONTROLt_PURGE_BAD_OPCODE_FRAMESf:
        case PC_MAC_CONTROLt_PURGE_PAUSE_FRAMESf:
        case PC_MAC_CONTROLt_PURGE_CONTROL_FRAMESf:
        case PC_MAC_CONTROLt_PURGE_PROMISCUOUS_FRAMESf:
        case PC_MAC_CONTROLt_PURGE_BROADCAST_FRAMESf:
        case PC_MAC_CONTROLt_PURGE_MULTICAST_FRAMESf:
        case PC_MAC_CONTROLt_PURGE_GOOD_FRAMESf:
        case PC_MAC_CONTROLt_PURGE_TRUNCATED_FRAMESf:
        case PC_MAC_CONTROLt_PURGE_LENGTH_CHECK_FAIL_FRAMESf:
        case PC_MAC_CONTROLt_PURGE_CRC_ERROR_FRAMESf:
        case PC_MAC_CONTROLt_PURGE_RX_CODE_ERROR_FRAMESf:
        case PC_MAC_CONTROLt_PURGE_UNSUPPORTED_PAUSE_PFC_DAf:
        case PC_MAC_CONTROLt_PURGE_STACK_VLAN_FRAMESf:
        case PC_MAC_CONTROLt_PURGE_WRONG_SAf:
            BCMPC_LT_FIELD_SET(config->fbmp,
                               BCMPC_MAC_CTRL_LT_FLD_MAC_RSV_CONTROL);
            SHR_IF_ERR_EXIT
                (pc_mac_control_rsv_entry_set(unit, fid, fval, entry));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief IMM input LT fields parsing.
 *
 * Parse the input LT fields and save the data to pc_mac_control_entry_t.
 *
 * \param [in] unit Unit number.
 * \param [in] key_flds IMM input key field array.
 * \param [in] data_flds IMM input data field array.
 * \param [out] entry Entry buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to convert \c in to \c pdata.
 */
static int
pc_lt_mac_control_flds_parse(int unit,
                                    const bcmltd_field_t *key_flds,
                                    const bcmltd_field_t *data_flds,
                                    pc_mac_control_entry_t *entry)
{
    const bcmltd_field_t *fld;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    /* Parse key fields. */
    fld = key_flds;
    while (fld) {
        fid = fld->id;
        fval = fld->data;

        SHR_IF_ERR_VERBOSE_EXIT
            (pc_mac_control_entry_set(unit, fid, fval, entry));

        fld = fld->next;
    }

    /* Parse data fields. */
    fld = data_flds;
    while (fld) {
        fid = fld->id;
        fval = fld->data;

        SHR_IF_ERR_VERBOSE_EXIT
            (pc_mac_control_entry_set(unit, fid, fval, entry));

        fld = fld->next;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize all fields value as SW default value.
 *
 * This initialization is only used for Insert operation.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry PC_MAC_CONTROL LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Input parameter failed validation check.
 */
static int
pc_mac_ctrl_lt_default_values_init(int unit, pc_mac_control_entry_t *lt_entry)
{
    uint64_t def_val = 0;
    uint32_t num;
    bcmpc_mac_control_t *mac_ctrl = NULL;
    SHR_FUNC_ENTER(unit);

    /* validate input parameter. */
    SHR_NULL_CHECK(lt_entry, SHR_E_PARAM);

    mac_ctrl = &(lt_entry->data);

    if (!BCMPC_LT_FIELD_GET(mac_ctrl->fbmp,
                            BCMPC_MAC_CTRL_LT_FLD_OVERSIZE_PKT)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_MAC_CONTROLt,
                                     PC_MAC_CONTROLt_OVERSIZE_PKTf,
                                     1, &def_val, &num));
        mac_ctrl->oversize_pkt = (uint32_t)def_val;
    }

    /*
     * Check if RX_ENABLE_AUTO is configured by the user,
     * if unset (0), then get the default value of MAC RX enable
     * if RX_ENABLEf is not configured by the user.
     * If set (1), then MAC RX enable is auto-configured
     * based on the PC_PORTt.ENABLEf and also the link status
     * of the port.
     * The default value for MAC RX enable is always 0 if user
     * controlled, if it is in auto-configure mode, the MAC RX
     * enable setting depends on the PC_PORTt.ENABLEf.
     */
    if (!BCMPC_LT_FIELD_GET(mac_ctrl->fbmp,
                            BCMPC_MAC_CTRL_LT_FLD_RX_ENABLE_AUTO)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_MAC_CONTROLt,
                                     PC_MAC_CONTROLt_RX_ENABLE_AUTOf,
                                     1, &def_val, &num));
        mac_ctrl->rx_enable_auto = (uint8_t)def_val;
    } else {
        /*
         * rx_enable_auto field is configured by user
         * if 1 - rx_enable is set by software, get the
         *        default setting for rx_enable.
         * if 0 - rx_enable is set by user, get default
         *        setting if not configured by the user.
         */
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_MAC_CONTROLt,
                                     PC_MAC_CONTROLt_RX_ENABLEf,
                                     1, &def_val, &num));
        if (mac_ctrl->rx_enable_auto) {
            mac_ctrl->rx_enable = (uint8_t)def_val;
        } else {
            if (!BCMPC_LT_FIELD_GET(mac_ctrl->fbmp,
                                       BCMPC_MAC_CTRL_LT_FLD_RX_ENABLE)) {
                mac_ctrl->rx_enable = (uint8_t)def_val;
            }
        }
    }

    /*
     * Check if TX_ENABLE_AUTO is configured by the user,
     * if unset (0), then get the default value of MAC TX enable
     * if TX_ENABLEf is not configured by the user.
     * If set (1), then MAC TX enable is auto-configured
     * based on the PC_PORTt.ENABLEf and also the link status
     * of the port.
     * The default value for MAC TX enable is always 1.
     * if it is in auto-configure mode, the MAC RX
     * enable setting depends on the PC_PORTt.ENABLEf.
     */
    if (!BCMPC_LT_FIELD_GET(mac_ctrl->fbmp,
                            BCMPC_MAC_CTRL_LT_FLD_TX_ENABLE_AUTO)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_MAC_CONTROLt,
                                     PC_MAC_CONTROLt_TX_ENABLE_AUTOf,
                                     1, &def_val, &num));
        mac_ctrl->tx_enable_auto = (uint32_t)def_val;
    } else {
        /*
         * tx_enable_auto field is configured by user
         * if 1 - tx_enable is set by software, get the
         *        default setting for rx_enable.
         * if 0 - tx_enable is set by user, get default
         *        setting if not configured by the user.
         */
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_MAC_CONTROLt,
                                     PC_MAC_CONTROLt_TX_ENABLEf,
                                     1, &def_val, &num));
        if (mac_ctrl->tx_enable_auto) {
            mac_ctrl->tx_enable = (uint8_t)def_val;
        } else {
            if (!BCMPC_LT_FIELD_GET(mac_ctrl->fbmp,
                                       BCMPC_MAC_CTRL_LT_FLD_TX_ENABLE)) {
                mac_ctrl->tx_enable = (uint8_t)def_val;
            }
        }
    }

    /*
     * Check if RUNT_THRESHOLD_AUTO is configured by the user,
     * if unset (0), then get the default value of runt threshold
     * if runt threshol;d is not configured by the user.
     * If set (1), then runt threshold value is auto-configured
     * based on the logical port to physical port map or in
     * other workds based on the Port Macro type to which the
     * logical port belongs.
     */
    if (!BCMPC_LT_FIELD_GET(mac_ctrl->fbmp,
                            BCMPC_MAC_CTRL_LT_FLD_RUNT_THRESHOLD_AUTO)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_MAC_CONTROLt,
                                     PC_MAC_CONTROLt_RUNT_THRESHOLD_AUTOf,
                                     1, &def_val, &num));
        mac_ctrl->runt_threshold_auto = (uint8_t)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(mac_ctrl->fbmp,
                            BCMPC_MAC_CTRL_LT_FLD_RUNT_THRESHOLD)) {
        SHR_IF_ERR_EXIT(
        bcmlrd_field_default_get(unit, PC_MAC_CONTROLt,
                                 PC_MAC_CONTROLt_RUNT_THRESHOLDf,
                                 1, &def_val, &num));
        mac_ctrl->runt_threshold = (uint8_t)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(mac_ctrl->fbmp,
                            BCMPC_MAC_CTRL_LT_FLD_LOCAL_FAULT_DISABLE)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_MAC_CONTROLt,
                                     PC_MAC_CONTROLt_LOCAL_FAULT_DISABLEf,
                                     1, &def_val, &num));
        mac_ctrl->local_fault_dis = (bool)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(mac_ctrl->fbmp,
                            BCMPC_MAC_CTRL_LT_FLD_REMOTE_FAULT_DISABLE)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_MAC_CONTROLt,
                                     PC_MAC_CONTROLt_REMOTE_FAULT_DISABLEf,
                                     1, &def_val, &num));
        mac_ctrl->rmt_fault_dis = (bool)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(mac_ctrl->fbmp,
                            BCMPC_MAC_CTRL_LT_FLD_INTER_FRAME_GAP_AUTO)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_MAC_CONTROLt,
                                     PC_MAC_CONTROLt_INTER_FRAME_GAP_AUTOf,
                                     1, &def_val, &num));
        mac_ctrl->ifg_auto = (uint32_t)def_val;
    } else {
        /*
         * runt_threshold_auto field is configured by user
         * if 1 - runt_threshold is set by software, get the
         *        default setting.
         * if 0 - runt_threshold is set by user, get default
         *        setting if not configured by the user.
         */
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_MAC_CONTROLt,
                                     PC_MAC_CONTROLt_INTER_FRAME_GAPf,
                                     1, &def_val, &num));
        if (mac_ctrl->ifg_auto) {
            mac_ctrl->ifg = (uint32_t)def_val;
        } else {
            if (!BCMPC_LT_FIELD_GET(mac_ctrl->fbmp,
                                    BCMPC_MAC_CTRL_LT_FLD_INTER_FRAME_GAP)) {
                mac_ctrl->ifg = (uint32_t)def_val;
            }
        }
    }

    if (!BCMPC_LT_FIELD_GET(mac_ctrl->fbmp, BCMPC_MAC_CTRL_LT_FLD_PAUSE_TX)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_MAC_CONTROLt,
                                     PC_MAC_CONTROLt_PAUSE_TXf,
                                     1, &def_val, &num));
        mac_ctrl->pause_tx = (uint32_t)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(mac_ctrl->fbmp, BCMPC_MAC_CTRL_LT_FLD_PAUSE_RX)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_MAC_CONTROLt,
                                     PC_MAC_CONTROLt_PAUSE_RXf,
                                     1, &def_val, &num));
        mac_ctrl->pause_rx = (uint32_t)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(mac_ctrl->fbmp,
                            BCMPC_MAC_CTRL_LT_FLD_PAUSE_ADDR)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_MAC_CONTROLt,
                                     PC_MAC_CONTROLt_PAUSE_ADDRf,
                                     1, &def_val, &num));
        sal_memcpy(mac_ctrl->pause_addr, &def_val,
                   sizeof(mac_ctrl->pause_addr));
    }

    if (!BCMPC_LT_FIELD_GET(mac_ctrl->fbmp,
                            BCMPC_MAC_CTRL_LT_FLD_PAUSE_PASS)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_MAC_CONTROLt,
                                     PC_MAC_CONTROLt_PAUSE_PASSf,
                                     1, &def_val, &num));
        mac_ctrl->pause_pass = (bool)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(mac_ctrl->fbmp,
                            BCMPC_MAC_CTRL_LT_FLD_CONTROL_PASS)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_MAC_CONTROLt,
                                     PC_MAC_CONTROLt_CONTROL_PASSf,
                                     1, &def_val, &num));
        mac_ctrl->control_pass = (bool)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(mac_ctrl->fbmp,
                            BCMPC_MAC_CTRL_LT_FLD_MAC_ECC_INTR_ENABLE)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_MAC_CONTROLt,
                                     PC_MAC_CONTROLt_MAC_ECC_INTR_ENABLEf,
                                     1, &def_val, &num));
        mac_ctrl->mac_ecc_intr = (bool)def_val;
    }

    if (!BCMPC_LT_FIELD_GET(mac_ctrl->fbmp, BCMPC_MAC_CTRL_LT_FLD_STALL_TX)) {
        SHR_IF_ERR_EXIT(
            bcmlrd_field_default_get(unit, PC_MAC_CONTROLt,
                                     PC_MAC_CONTROLt_STALL_TXf,
                                     1, &def_val, &num));
        mac_ctrl->stall_tx = (uint32_t)def_val;
    }

    SHR_IF_ERR_EXIT(pc_mac_ctrl_lt_rsv_default_values_init(unit, lt_entry));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Logical port MAC configuration validation function.
 *
 * \param [in] unit This is device unit number.
 * \param [in] key Key value for TX_TAPS LT.
 * \param [out] cfg_valid Set to true if config is valid. else false.
 * \param [out] cfg_hw_apply Set to true if config is valid. else false.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
pc_mac_control_config_validate(int unit, bcmpc_lport_t key,
                               bool *cfg_valid, bool *cfg_hw_apply)
{
    bcmpc_drv_t *pc_drv = NULL;
    bcmpc_pport_t pport = BCMPC_INVALID_PPORT;
    bool is_internal = FALSE;
    int rv = SHR_E_NONE;
    bcmpc_port_cfg_t pcfg;

    SHR_FUNC_ENTER(unit);

    /*
     * Check if logical to physical port map is available.
     * Do not populate the special (CPU, internal loopback)
     * ports.
     * Return success even if the logical to physical port map
     * is not available or pport is invalid, because the logcial to
     * physical port configuratin can be done later, so the
     * tx_taps configuration should not be blocked.
     * set cfg_hw_apply = false and return success
     * Indicate the operational state of the port in pc_port_status.
     */
    rv = bcmpc_port_phys_map_get(unit, key, &pport);

    if ((rv == SHR_E_NOT_FOUND) || (pport == BCMPC_INVALID_PPORT)) {
        *cfg_valid = FALSE;
        SHR_EXIT();
    }

    /* Return error for internal (CPU, LOOPBACK) ports. */
    SHR_IF_ERR_EXIT(bcmpc_drv_get(unit, &pc_drv));

    SHR_IF_ERR_EXIT(pc_drv->is_internal_port(unit, pport, &is_internal));

    if (is_internal == TRUE) {
        *cfg_valid = FALSE;
        *cfg_hw_apply = FALSE;
        SHR_EXIT();
    }

    /*
     * Check if PC_PORT LT has entry for this logical port.
     * If not present,
     * set cfg_hw_apply = false, cfg_valid = true and return success
     * If present,
     * number of lanes is 0, means logcial port is not configured
     * set cfg_hw_apply = false, cfg_valid = true and return success
     */
    rv = bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&key),
                                   P(&pcfg));

    if (rv == SHR_E_NOT_FOUND) {
        *cfg_valid = TRUE;
        *cfg_hw_apply = FALSE;
        SHR_EXIT();
    }

    if (pcfg.num_lanes == 0) {
        *cfg_valid = TRUE;
        *cfg_hw_apply = FALSE;
        SHR_EXIT();
    }

    *cfg_valid = TRUE;
    *cfg_hw_apply = TRUE;

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * IMM event handler
 */

/*!
 * \brief Stage callback function of IMM event handler (bcmimm_lt_cb_t).
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] event This is the reason for the entry event.
 * \param [in] key_flds This is a linked list of the key fields identifying
 *                        the entry.
 * \param [in] data_flds This is a linked list of the data fields in the
 *                        modified entry.
 * \param [in] context Is a pointer that was given during registration.
 *                     The callback can use this pointer to retrieve some
 *                     context.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
pc_mac_control_stage(int unit,
                     bcmltd_sid_t sid,
                     uint32_t trans_id,
                     bcmimm_entry_event_t event,
                     const bcmltd_field_t *key_flds,
                     const bcmltd_field_t *data_flds,
                     void *context,
                     bcmltd_fields_t *output_fields)
{
    pc_mac_control_entry_t entry;
    pc_mac_control_entry_t default_entry;
    bool port_based_cfg_enable = FALSE;
    bool config_valid = FALSE;
    bool config_hw_apply = FALSE;

    SHR_FUNC_ENTER(unit);

    pc_mac_control_entry_t_init(&entry);
    pc_mac_control_entry_t_init(&default_entry);

    if (output_fields) {
        output_fields->count = 0;
    }

    /*
     * The MAC_CONTROL staging function is invoked for any operation on
     * PC_MAC_CONTROL LT. During pre-config stage of System Manager
     * initialization, the staging function gets invoked for any
     * PC_MAC_CONTROL LT entry in the config.yml. Logical port
     * hardware resources are configured and applied at a lateri
     * stage during the run stage of the System Manager initialization.
     *
     * During the system initialization, the PC omcponent is in
     * bulk configuration mode since the configuration is applied
     * run-stage. After the configuration is applied, the PC
     * configuration is switched to per-port configuration mode.
     *
     * During pre-config stage, since the PC component is in bulk
     * configuration mode, validation on the user input is not done
     * in this stage and only done during run-stage when the
     * configuration is being applied to the hardware.
     *
     * So, if PC compoenent is in bulk configuration mode, and if
     * MAC_CONTROL LT staging function gets called indicating an
     * LT entry in the config.yml, return success to populate the
     * IMM DB with the user provisioned values.
     */

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pc_port_based_config_get(unit, &port_based_cfg_enable));

    if (!port_based_cfg_enable) {
        SHR_EXIT();
    }

    /*
     * If the code reaches this point, it means the user is trying to
     * update the MAC_CONTROL LT entry and the system is run-stage.
     * Do not validate the user configured input, process it for
     * configuring the hardware. Reject the LT operation on invalid
     * input. Do not reject or fail the operation on this LT due to
     * dependency on other LT's.
     * For example:
     * The system could have boot without providing a config.yml
     * or PC component configuration. All the PC LT's should
     * support any provisioning sequence and there should not be
     * any restriction on the user to provision the PC component
     * in a sequence.
     *
     * On MAC_CONTROL LT operation, validate if the config can be
     * applied to the hardware due to dependencies on other LTs
     * provisioning. If this check fails, return success and
     * update the operational state of the MAC_CONTROL LT entry
     * as not valid.
     */

    SHR_IF_ERR_VERBOSE_EXIT
        (pc_lt_mac_control_flds_parse(unit, key_flds, NULL, &entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (pc_mac_control_config_validate(unit, entry.key, &config_valid,
                                        &config_hw_apply));

    if (config_hw_apply == FALSE || config_valid == FALSE) {
        SHR_EXIT();
    }

    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            SHR_IF_ERR_VERBOSE_EXIT
                (pc_lt_mac_control_flds_parse(unit, NULL, data_flds, &entry));
            SHR_IF_ERR_VERBOSE_EXIT
                (pc_mac_ctrl_lt_default_values_init(unit, &entry));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_internal_mac_control_insert(unit, entry.key,
                                                       &entry.data));
            break;
        case BCMIMM_ENTRY_UPDATE:
            SHR_IF_ERR_VERBOSE_EXIT
                (pc_lt_mac_control_flds_parse(unit, NULL, data_flds, &entry));
            SHR_IF_ERR_VERBOSE_EXIT
                (pc_mac_ctrl_lt_default_values_init(unit, &default_entry));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_internal_mac_control_update(unit, entry.key,
                                                   &entry.data, default_entry.data));
            break;
        case BCMIMM_ENTRY_DELETE:
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Convert the PC data structure to the LT fields.
 *
 * This is the fill callback function of IMM DB schema handler, \ref
 * bcmpc_db_imm_schema_handler_t.
 *
 * Convert an integer port id \c key and a bcmpc_mac_control_t \c data to
 * bcmltd_fields_t array \c flds.
 *
 * \param [in] unit Unit number.
 * \param [in] key_size Key size.
 * \param [in] key Key value.
 * \param [in] data_size Data size.
 * \param [in] data Data value.
 * \param [out] flds LT field array.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int
pc_mac_control_oper_flds_fill(int unit,
                              bcmpc_mac_control_t *data,
                              bcmltd_fields_t *flds)
{
    uint32_t i = 0;
    bcmpc_mac_control_t *config = data;

    SHR_FUNC_ENTER(unit);

    if (!data) {
        SHR_EXIT();
    }

    /* update the opeartional fields in the LT. */
    for (i = 0; i < flds->count; i++) {
        switch (flds->field[i]->id) {
            case PC_MAC_CONTROLt_RX_ENABLE_OPERf:
                flds->field[i]->data =  config->rx_enable;
                break;
            case PC_MAC_CONTROLt_TX_ENABLE_OPERf:
                flds->field[i]->data =  config->tx_enable;
                break;
            case PC_MAC_CONTROLt_RUNT_THRESHOLD_OPERf:
                flds->field[i]->data =  config->runt_threshold;
                break;
            case PC_MAC_CONTROLt_INTER_FRAME_GAP_OPERf:
                flds->field[i]->data =  config->ifg;
                break;
            case PC_MAC_CONTROLt_PAUSE_TX_OPERf:
                flds->field[i]->data =  config->pause_tx;
                break;
            case PC_MAC_CONTROLt_PAUSE_RX_OPERf:
                flds->field[i]->data =  config->pause_rx;
                break;
            case PC_MAC_CONTROLt_STALL_TX_OPERf:
                flds->field[i]->data =  config->stall_tx_status;
                break;
            default:
                break;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Callback function for MAC CONTROL LT lookup.
 *
 *  The callback function will be called during normal lookup,
 * traverse or when required to read the content from hardware.
 * The purpose of this call is to allow the component to overwrite
 * read-only operational fields which are dynamic in nature.
 *
 * The lookup operations defined by \ref bcmimm_lookup_type_t can be classified
 * into two operation categories, lookup and traverse.
 *
 * For the lookup category the key fields will be found in the \c in parameter
 * while for the traverse category the key fields will be found in the \c out
 * parameter.
 *
 * In all the operations the \c out field will contain all the data fields and
 * their associated values as maintained by the IMM or default values
 * (for fields that were not yet set).
 * The component may only change the data portions of the data fields.
 * For improved performance the fields will be sorted based on
 * their field ID. Array fields will be sorted based on their index.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 * \param [in] lkup_type Indicates the type of lookup that is being performed.
 * \param [in] in Is a structure which contains the key fields but may include
 * also other fields. The component should only focus on the key fields.
 * \param [in,out] out this structure contains all the fields of the
 * table, so that the component should not delete or add any field to this
 * structure. The data values of the field were set by the IMM so the component
 * may only overwrite the data section of the fields.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
pc_mac_control_lookup(int unit,
                      bcmltd_sid_t sid,
                      uint32_t trans_id,
                      void *context,
                      bcmimm_lookup_type_t lkup_type,
                      const bcmltd_fields_t *in,
                      bcmltd_fields_t *out)
{

    pc_mac_control_entry_t entry;
    const bcmltd_field_t *key_flds = NULL;
    bool port_based_cfg_enable = FALSE;
    bcmpc_pport_t pport = BCMPC_INVALID_PPORT;
    bcmpc_mac_control_t mcfg;

    SHR_FUNC_ENTER(unit);

    pc_mac_control_entry_t_init(&entry);

    /* key fields are in the "out" parameter for traverse operations */
    key_flds = (lkup_type == BCMIMM_TRAVERSE)? *(out->field): *(in->field);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pc_port_based_config_get(unit, &port_based_cfg_enable));

    if (!port_based_cfg_enable) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (pc_lt_mac_control_flds_parse(unit, NULL, key_flds, &entry));

    pport = bcmpc_lport_to_pport(unit, entry.key);

    /*
     * Defensive check, pport should be valid to retrieve the operational
     * values from the hardware.
     */
    if (pport == BCMPC_INVALID_PPORT) {
        SHR_EXIT();
    }

    /*
     * Retrieve hardware settings for MAC to update the operational values.
     */
    sal_memset(&mcfg, 0, sizeof(mcfg));
    SHR_IF_ERR_EXIT
        (bcmpc_pmgr_mac_control_get(unit, entry.key, pport, &mcfg));

    /* Update the read-only operational values in the MAC_CONTROL LT. */
    SHR_IF_ERR_EXIT
        (pc_mac_control_oper_flds_fill(unit, &mcfg, out));

exit:
    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * DB schema handler
 */

/*!
 * \brief Convert the LT fields to the PC data structure.
 *
 * This is the parse callback function of IMM DB schema handler, \ref
 * bcmpc_db_imm_schema_handler_t.
 *
 * Convert bcmltd_fields_t array \c flds to an integer port id \c key and a
 * bcmpc_mac_control_t \c data.
 *
 * \param [in] unit Unit number.
 * \param [in] flds LT field array.
 * \param [in] key_size Key size.
 * \param [out] key Key value from \c flds.
 * \param [in] data_size Data size.
 * \param [out] data Data value from \c flds.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int
pc_mac_control_entry_parse(int unit, const bcmltd_fields_t *flds,
                                  size_t key_size, void *key,
                                  size_t data_size, void *data)
{
    size_t i;
    uint32_t fid;
    uint64_t fval;
    pc_mac_control_entry_t entry;

    SHR_FUNC_ENTER(unit);

    pc_mac_control_entry_t_init(&entry);

    for (i = 0; i < flds->count; i++) {
        fid = flds->field[i]->id;
        fval = flds->field[i]->data;
        SHR_IF_ERR_VERBOSE_EXIT
            (pc_mac_control_entry_set(unit, fid, fval, &entry));
    }

    if (key) {
        sal_memcpy(key, &entry.key, key_size);
    }

    if (data) {
        sal_memcpy(data, &entry.data, data_size);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Convert the PC data structure to the LT fields.
 *
 * This is the fill callback function of IMM DB schema handler, \ref
 * bcmpc_db_imm_schema_handler_t.
 *
 * Convert an integer port id \c key and a bcmpc_mac_control_t \c data to
 * bcmltd_fields_t array \c flds.
 *
 * \param [in] unit Unit number.
 * \param [in] key_size Key size.
 * \param [in] key Key value.
 * \param [in] data_size Data size.
 * \param [in] data Data value.
 * \param [out] flds LT field array.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
static int
pc_mac_control_entry_fill(int unit,
                                 size_t key_size, void *key,
                                 size_t data_size, void *data,
                                 bcmltd_fields_t *flds)
{
    size_t fcnt = 0;
    bcmpc_lport_t *port_id = key;
    bcmpc_mac_control_t *config = data;

    SHR_FUNC_ENTER(unit);

    if (key) {
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_MAC_CONTROLt_PORT_IDf, 0, *port_id, fcnt);
    }

    if (data) {
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_MAC_CONTROLt_RX_ENABLE_AUTOf, 0,
             config->rx_enable_auto, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_MAC_CONTROLt_RX_ENABLEf, 0, config->rx_enable, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_MAC_CONTROLt_TX_ENABLE_AUTOf, 0,
             config->tx_enable_auto, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_MAC_CONTROLt_TX_ENABLEf, 0, config->tx_enable, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_MAC_CONTROLt_OVERSIZE_PKTf, 0,
             config->oversize_pkt, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_MAC_CONTROLt_RUNT_THRESHOLD_AUTOf, 0,
            config->runt_threshold_auto, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_MAC_CONTROLt_RUNT_THRESHOLDf, 0,
            config->runt_threshold, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_MAC_CONTROLt_LOCAL_FAULT_DISABLEf, 0,
            config->local_fault_dis, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_MAC_CONTROLt_REMOTE_FAULT_DISABLEf, 0,
            config->rmt_fault_dis, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_MAC_CONTROLt_INTER_FRAME_GAP_AUTOf, 0,
             config->ifg_auto, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_MAC_CONTROLt_INTER_FRAME_GAPf, 0, config->ifg, fcnt);
        BCMPC_LT_FIELD_DATA_ADD
            (flds, PC_MAC_CONTROLt_PAUSE_ADDRf, 0, &config->pause_addr, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_MAC_CONTROLt_PAUSE_TXf, 0, config->pause_tx, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_MAC_CONTROLt_PAUSE_RXf, 0, config->pause_rx, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_MAC_CONTROLt_PAUSE_PASSf, 0,
             ((config->pause_pass)? TRUE: FALSE), fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_MAC_CONTROLt_CONTROL_PASSf, 0,
             ((config->control_pass)? TRUE: FALSE), fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_MAC_CONTROLt_OPERATIONAL_STATEf, 0,
             config->oper_state, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_MAC_CONTROLt_MAC_ECC_INTR_ENABLEf, 0,
            config->mac_ecc_intr, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_MAC_CONTROLt_STALL_TXf, 0, config->stall_tx, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_MAC_CONTROLt_PURGE_DRIBBLE_NIBBLE_ERROR_FRAMESf,
             0, (config->mac_rsv_ctrl &
             (1 << BCMPC_MAC_RSV_DRIBBLE_NIBBLE_ERROR))? TRUE: FALSE, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_MAC_CONTROLt_PURGE_MACSEC_FRAMESf,
             0, (config->mac_rsv_ctrl &
             (1 << BCMPC_MAC_RSV_MACSEC_PKTS))? TRUE: FALSE, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_MAC_CONTROLt_PURGE_PFC_FRAMESf,
             0, (config->mac_rsv_ctrl &
             (1 << BCMPC_MAC_RSV_PFC_PKTS))? TRUE: FALSE, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_MAC_CONTROLt_PURGE_RUNT_FRAMESf,
             0, (config->mac_rsv_ctrl &
             (1 << BCMPC_MAC_RSV_RUNT_PKTS))? TRUE: FALSE, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_MAC_CONTROLt_RX_FIFO_FULLf,
             0, (config->mac_rsv_ctrl &
             (1 << BCMPC_MAC_RSV_RX_FIFO_FULL))? TRUE: FALSE, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_MAC_CONTROLt_PURGE_UNICAST_FRAMESf,
             0, (config->mac_rsv_ctrl &
             (1 << BCMPC_MAC_RSV_UNICAST_PKTS))? TRUE: FALSE, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_MAC_CONTROLt_PURGE_VLAN_TAGGED_FRAMESf,
             0, (config->mac_rsv_ctrl &
             (1 << BCMPC_MAC_RSV_VLAN_TAGGED_PKTS))? TRUE: FALSE, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_MAC_CONTROLt_PURGE_BAD_OPCODE_FRAMESf,
             0, (config->mac_rsv_ctrl &
             (1 << BCMPC_MAC_RSV_BAD_OPCODE_PKTS))? TRUE: FALSE, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_MAC_CONTROLt_PURGE_PAUSE_FRAMESf,
             0, (config->mac_rsv_ctrl &
             (1 << BCMPC_MAC_RSV_PAUSE_PKTS))? TRUE: FALSE, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_MAC_CONTROLt_PURGE_CONTROL_FRAMESf,
             0, (config->mac_rsv_ctrl &
             (1 << BCMPC_MAC_RSV_CONTROL_PKTS))? TRUE: FALSE, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_MAC_CONTROLt_PURGE_PROMISCUOUS_FRAMESf,
             0, (config->mac_rsv_ctrl &
             (1 << BCMPC_MAC_RSV_PROMISCUOUS_PKTS))? TRUE: FALSE, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_MAC_CONTROLt_PURGE_BROADCAST_FRAMESf,
             0, (config->mac_rsv_ctrl &
             (1 << BCMPC_MAC_RSV_BROADCAST_PKTS))? TRUE: FALSE, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_MAC_CONTROLt_PURGE_MULTICAST_FRAMESf,
             0, (config->mac_rsv_ctrl &
             (1 << BCMPC_MAC_RSV_MULTICAST_PKTS))? TRUE: FALSE, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_MAC_CONTROLt_PURGE_GOOD_FRAMESf,
             0, (config->mac_rsv_ctrl &
             (1 << BCMPC_MAC_RSV_GOOD_PKTS))? TRUE: FALSE, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_MAC_CONTROLt_PURGE_TRUNCATED_FRAMESf,
             0, (config->mac_rsv_ctrl &
             (1 << BCMPC_MAC_RSV_TRUNCATED_PKTS))? TRUE: FALSE, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_MAC_CONTROLt_PURGE_LENGTH_CHECK_FAIL_FRAMESf,
             0, (config->mac_rsv_ctrl &
             (1 << BCMPC_MAC_RSV_LENGTH_CHECK_FAIL_PKTS))? TRUE: FALSE, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_MAC_CONTROLt_PURGE_CRC_ERROR_FRAMESf,
             0, (config->mac_rsv_ctrl &
             (1 << BCMPC_MAC_RSV_CRC_ERROR_PKTS))? TRUE: FALSE, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_MAC_CONTROLt_PURGE_RX_CODE_ERROR_FRAMESf,
             0, (config->mac_rsv_ctrl &
             (1 << BCMPC_MAC_RSV_RX_CODE_ERROR_PKTS))? TRUE: FALSE, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_MAC_CONTROLt_PURGE_UNSUPPORTED_PAUSE_PFC_DAf,
             0, (config->mac_rsv_ctrl &
             (1 << BCMPC_MAC_RSV_UNSUPPORTED_PAUSE_PFC_DA_PKTS))? TRUE: FALSE,
             fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_MAC_CONTROLt_PURGE_STACK_VLAN_FRAMESf,
             0, (config->mac_rsv_ctrl &
             (1 << BCMPC_MAC_RSV_STACK_VLAN_PKTS))? TRUE: FALSE, fcnt);
        BCMPC_LT_FIELD_VAL_ADD
            (flds, PC_MAC_CONTROLt_PURGE_WRONG_SAf,
             0, (config->mac_rsv_ctrl &
             (1 << BCMPC_MAC_RSV_WRONG_SA_PKTS))? TRUE: FALSE, fcnt);
    }

    flds->count = fcnt;

    SHR_FUNC_EXIT();
}


/*******************************************************************************
 * Public Functions
 */

int
bcmpc_imm_mac_control_register(int unit)
{
    bcmpc_db_imm_schema_handler_t db_hdl;
    bcmimm_lt_cb_t event_hdl;

    SHR_FUNC_ENTER(unit);

    db_hdl.entry_parse = pc_mac_control_entry_parse;
    db_hdl.entry_fill = pc_mac_control_entry_fill;

    sal_memset(&event_hdl, 0, sizeof(event_hdl));
    event_hdl.stage = pc_mac_control_stage;
    event_hdl.lookup = pc_mac_control_lookup;

    /* Register the handlers to the table. */
    SHR_IF_ERR_EXIT
        (bcmpc_imm_tbl_register(unit, IMM_SID, &db_hdl, &event_hdl));

exit:
    SHR_FUNC_EXIT();
}

/*
 * This function should be invoked when a new logical port is
 * added to the system to prepopultate the PC_MAC_CONTROL LT
 * with the default values for the port based on the in
 * PC_PORT LT. The logical to physical port map must be
 * available when calling this function.
 */
int
bcmpc_mac_control_config_prepopulate(int unit, bcmpc_lport_t lport,
                                      bcmpc_port_cfg_t *pcfg, bool null_entry)
{
    bcmpc_drv_t *pc_drv = NULL;
    bcmpc_mac_control_t mcfg;
    pc_mac_control_entry_t entry;
    bool is_internal = FALSE;
    int rv = SHR_E_NONE;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    pc_mac_control_entry_t_init(&entry);

    /* Skip CPU port. */
    if (lport == 0) {
        SHR_EXIT();
    }

    if (!null_entry) {
        /*
         * Check if logical to physical port map is available.
         * Do not populate the special (CPU, internal loopback)
         * ports.
         */
        if (pcfg->pport == BCMPC_INVALID_PPORT) {
            SHR_EXIT();
        }

        /* Do not populate the LT for the internal (CPU, LOOPBACK) ports. */
        SHR_IF_ERR_EXIT(bcmpc_drv_get(unit, &pc_drv));

        SHR_IF_ERR_EXIT(pc_drv->is_internal_port(unit, pcfg->pport, &is_internal));

        if (is_internal == TRUE) {
            SHR_EXIT();
        }
    }

    /* Get the default MAC settings for the logical port. */
    fid = PC_MAC_CONTROLt_PORT_IDf;
    fval = lport;

    SHR_IF_ERR_VERBOSE_EXIT
        (pc_mac_control_entry_set(unit, fid, fval, &entry));

    SHR_IF_ERR_VERBOSE_EXIT
        (pc_mac_ctrl_lt_default_values_init(unit, &entry));

    /* Copy the default values into the mac_control structure. */
    sal_memcpy(&mcfg, &entry.data, sizeof(bcmpc_mac_control_t));

    /*
     * Check if PC_MAC_CONTROL LT for this logical port
     * is already present in the DB.
     * If not present, insert the default values based on the
     * current port configuration.
     * If present, then skip this operation.
     */

    rv = bcmpc_db_imm_entry_lookup(unit, IMM_SID,
                                   P(&entry.key), P(&entry.data));

    if (rv == SHR_E_NOT_FOUND) {

        if (!null_entry) {
            /* Get MAC hardware configuration and update the DB. */
            SHR_IF_ERR_EXIT
                (bcmpc_pmgr_mac_control_get(unit, lport, pcfg->pport, &mcfg));
        }

        SHR_IF_ERR_EXIT
            (bcmpc_db_imm_entry_insert(unit, IMM_SID,
                                      sizeof(bcmpc_lport_t), &lport,
                                      sizeof(bcmpc_mac_control_t),
                                      &mcfg));
    } else {
        /* Check the operational satet of the entry and mark it as valid. */
        if (entry.data.oper_state != BCMPC_VALID) {
            entry.data.oper_state = BCMPC_VALID;

            SHR_IF_ERR_EXIT
                (bcmpc_db_imm_entry_update(unit, IMM_SID,
                                           sizeof(bcmpc_lport_t), &lport,
                                           sizeof(bcmpc_mac_control_t),
                                           &entry.data));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * This function should be invoked when a new logical port is
 * deleted from the system to delete the associated PC_MAC_CONTROL LT
 * entry for the logcial port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] pcfg Port confgiration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
int
bcmpc_mac_control_config_delete(int unit, bcmpc_lport_t lport)
{
    pc_mac_control_entry_t entry;
    int rv = SHR_E_NONE;
    uint32_t fid;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    pc_mac_control_entry_t_init(&entry);

    fid = PC_MAC_CONTROLt_PORT_IDf;
    fval = lport;

    SHR_IF_ERR_VERBOSE_EXIT
        (pc_mac_control_entry_set(unit, fid, fval, &entry));

    rv = bcmpc_db_imm_entry_lookup(unit, IMM_SID,
                                   P(&entry.key), P(&entry.data));

    if (rv != SHR_E_NOT_FOUND) {
        entry.data.oper_state = BCMPC_PORT_UNKNOWN;

        SHR_IF_ERR_EXIT
            (bcmpc_db_imm_entry_update(unit, IMM_SID,
                                       sizeof(bcmpc_lport_t), &lport,
                                       sizeof(bcmpc_mac_control_t),
                                       &entry.data));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * This function is invoked during the pre_config stage
 * to pre-populate all the PC_MAC_CONTROL LT entries
 * with null entries.
 */
int
bcmpc_mac_control_prepopulate_all(int unit)
{
    uint64_t lport_min = 0, lport_max = 0, lport = 0;
    bcmpc_mac_control_t mcfg;
    pc_mac_control_entry_t entry;
    bcmpc_lport_t key;
    uint32_t num;

    SHR_FUNC_ENTER(unit);

    pc_mac_control_entry_t_init(&entry);
    /* Get the min and max values for the keys. */
    SHR_IF_ERR_EXIT
        (bcmlrd_field_min_get(unit, PC_MAC_CONTROLt, PC_MAC_CONTROLt_PORT_IDf,
                              1, &lport_min, &num));

    SHR_IF_ERR_EXIT
        (bcmlrd_field_max_get(unit, PC_MAC_CONTROLt, PC_MAC_CONTROLt_PORT_IDf,
                              1, &lport_max, &num));

    sal_memset(&mcfg, 0, sizeof(mcfg));

    /* Get the software defaults from the map table. */
    SHR_IF_ERR_VERBOSE_EXIT
        (pc_mac_ctrl_lt_default_values_init(unit, &entry));

    /* Copy the default values into the mac_control structure. */
    sal_memcpy(&mcfg, &entry.data, sizeof(bcmpc_mac_control_t));


    mcfg.oper_state = BCMPC_PORT_UNKNOWN;

    for (lport = lport_min; lport <= lport_max; lport++) {
         key = (bcmpc_lport_t) lport;
         SHR_IF_ERR_EXIT
             (bcmpc_db_imm_entry_insert(unit, IMM_SID,
                                        sizeof(bcmpc_lport_t), &key,
                                        sizeof(bcmpc_mac_control_t),
                                        &mcfg));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * This function should be invoked when a new logical port is
 * added to the system to update the PC_MAC_CONTROL LT
 * entry OPERATIONAL_STATE.
 */
int
bcmpc_mac_control_oper_state_update(int unit, bcmpc_lport_t lport,
                                    bcmpc_pport_t pport,
                                    bcmpc_entry_state_t oper_state)
{
    bcmpc_drv_t *pc_drv = NULL;
    pc_mac_control_entry_t entry;
    bool is_internal = FALSE;

    SHR_FUNC_ENTER(unit);

    pc_mac_control_entry_t_init(&entry);

    /* Skip CPU port. */
    if (lport == 0) {
        SHR_EXIT();
    }

    /*
     * Check if logical to physical port map is available.
     * Do not populate the special (CPU, internal loopback)
     * ports.
     */
    if (pport == BCMPC_INVALID_PPORT) {
        SHR_EXIT();
    }

    /* Do not populate the LT for the internal (CPU, LOOPBACK) ports. */
    SHR_IF_ERR_EXIT(bcmpc_drv_get(unit, &pc_drv));

    SHR_IF_ERR_EXIT(pc_drv->is_internal_port(unit, pport, &is_internal));

    if (is_internal == TRUE) {
        SHR_EXIT();
    }

    /* Update the operational state of the entry. */
    SHR_IF_ERR_EXIT
        (bcmpc_db_imm_entry_lookup(unit, IMM_SID,
                                   sizeof(bcmpc_lport_t), &lport,
                                   sizeof(bcmpc_mac_control_t), &entry.data));

    entry.data.oper_state = oper_state;

    SHR_IF_ERR_EXIT
        (bcmpc_db_imm_entry_update(unit, IMM_SID,
                                   sizeof(bcmpc_lport_t), &lport,
                                   sizeof(bcmpc_mac_control_t),
                                   &entry.data));

exit:
    SHR_FUNC_EXIT();
}
