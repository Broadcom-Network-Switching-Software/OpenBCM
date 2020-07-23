/*! \file bcmpc_imm_internal.h
 *
 * BCMPC in-memory table (IMM) utilities.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPC_IMM_INTERNAL_H
#define BCMPC_IMM_INTERNAL_H

#include <sal/sal_assert.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmpc/bcmpc_db_internal.h>

/*!
 * \brief Add a field with data value to the field array.
 *
 * \param [in] _flds Field array, refer to bcmltd_fields_t.
 * \param [in] _fid Field ID, refer to bcmltd_field_t.
 * \param [in] _fidx Field Index, refer to bcmltd_field_t.
 * \param [in] _fdata Pointer to the field data, refer to bcmltd_field_t.
 * \param [in,out] _fcnt Fields count. The \c _fcnt will increase 1 after the
 *                       field is added.
 */
#define BCMPC_LT_FIELD_DATA_ADD(_flds, _fid, _fidx, _fdata, _fcnt) \
            do { \
                assert(_flds->count > fcnt); \
                _flds->field[_fcnt]->id = _fid; \
                _flds->field[_fcnt]->idx = _fidx; \
                sal_memcpy(&(_flds->field[_fcnt]->data), _fdata, \
                           sizeof(*_fdata)); \
                _fcnt++; \
            } while (0);

/*!
 * \brief Add a field with primitive type or constant value to the field array.
 *
 * \param [in] _flds Field array, refer to bcmltd_fields_t.
 * \param [in] _fid Field ID, refer to bcmltd_field_t.
 * \param [in] _fidx Field Index, refer to bcmltd_field_t.
 * \param [in] _fval Field value, refer to bcmltd_field_t.
 * \param [in,out] _fcnt Fields count. The \c _fcnt will increase 1 after the
 *                       field is added.
 */
#define BCMPC_LT_FIELD_VAL_ADD(_flds, _fid, _fidx, _fval, _fcnt) \
            do { \
                assert(_flds->count > fcnt); \
                _flds->field[_fcnt]->id = _fid; \
                _flds->field[_fcnt]->idx = _fidx; \
                _flds->field[_fcnt]->data = (uint64_t)_fval; \
                _fcnt++; \
            } while (0);

/*!
 * \brief This macro is used to set a Field present indicator bit status.
 *
 * Sets the Field \c _f present bit indicator in the Field Bitmap \c _m.
 *
 * \param [in] _m Field bitmap.
 * \param [in] _f Field position in the bitmap.
 *
 * \returns Nothing.
 */
#define BCMPC_LT_FIELD_SET(_m, _f)    \
            do {                      \
                SHR_BITSET(_m, _f);   \
            } while (0)

/*!
 * \brief This macro is used to get the Field present indicator bit status.
 *
 * Returns the Field \c _f present bit indicator status from the Field
 * Bitmap \c _m.
 *
 * \param [in] _m Field bitmap.
 * \param [in] _f Field position in the bitmap.
 *
 * \returns Field present indicator bit status from the Field bitmap.
 */
#define BCMPC_LT_FIELD_GET(_m, _f)  (SHR_BITGET(_m, _f))

/*!
 * \brief This macro is used to clear the Field bitmap.
 *
 * Returns the cleared field bitmap.
 *
 * \param [in] _m Field bitmap.
 * \param [in] _l Length of the field bitmap.
 *
 * \returns cleared field bitmap.
 */
#define BCMPC_LT_FILED_BMP_CLR(_m, _l) (SHR_BITCLR_RANGE(_m, 0, _l))

/*!
 * \brief Get the value of a field from input array.
 *
 * \param [in] unit Unit number.
 * \param [in] flds LT fields.
 * \param [in] fid Field ID.
 * \param [out] fval Field value.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmpc_imm_lt_field_get(int unit, const bcmltd_field_t *flds, uint32_t fid,
                       uint64_t *fval);

/*!
 * \brief Register the IMM DB and event handler to the table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] db_hdl Database handler.
 * \param [in] event_hdl Event handler.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmpc_imm_tbl_register(int unit, bcmltd_sid_t sid,
                       bcmpc_db_imm_schema_handler_t *db_hdl,
                       bcmimm_lt_cb_t *event_hdl);

/*!
 * \brief Initialize the IMM interface for all PC LTs.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmpc_imm_init(int unit);

/*!
 * \brief Initialize the IMM interface for PC_PM_TX_LANE_PROFILEt.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmpc_imm_pm_tx_lane_profile_register(int unit);

/*!
 * \brief Initialize the IMM interface for PC_PM_RX_LANE_PROFILEt.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmpc_imm_pm_rx_lane_profile_register(int unit);

/*!
 * \brief Initialize the IMM interface for PC_PORTt.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmpc_imm_port_register(int unit);

/*!
 * \brief Initialize the IMM interface for PC_PORT_PHYS_MAPt.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmpc_imm_port_phys_map_register(int unit);

/*!
 * \brief Initialize the IMM interface for PC_PMt.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmpc_imm_pm_register(int unit);

/*!
 * \brief Initialize the IMM interface for PC_PM_COREt.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmpc_imm_pm_core_register(int unit);

/*!
 * \brief Initialize the IMM interface for PC_PM_LANEt.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmpc_imm_pm_lane_register(int unit);

/*!
 * \brief Initialize the IMM interface for PC_AUTONEG_PROFILEt.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmpc_imm_autoneg_profile_register(int unit);

/*!
 * \brief Initialize the IMM interface for PC_PFC_PROFILEt.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmpc_imm_pfc_profile_register(int unit);

/*!
 * \brief Initialize the IMM interface for LM_CONTROLt.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmpc_imm_lm_ctrl_register(int unit);

/*!
 * \brief Initialize the IMM interface for LM_PORT_CONTROLt.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmpc_imm_lm_port_ctrl_register(int unit);

/*!
 * \brief Initialize the IMM interface for PC_PMD_FIRMWAREt.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmpc_imm_pmd_firmware_config_register(int unit);

/*!
 * \brief Initialize the IMM interface for PC_TX_TAPSt.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmpc_imm_tx_taps_config_register(int unit);
/*!
 * \brief Initialize the IMM interface for PC_PHY_CONTROL_PROFILEt.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmpc_imm_phy_control_register(int unit);

/*!
 * \brief Initialize the IMM interface for PC_MAC_CONTROLt.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmpc_imm_mac_control_register(int unit);

/*!
 * \brief Initialize the IMM interface for PC_PFCt.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmpc_imm_pfc_register(int unit);

/*!
 * \brief Initialize the IMM interface for PC_PORT_TIMESYNCt.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmpc_imm_port_timesync_register(int unit);

/*!
 * \brief Initialize the IMM interface for PC_PORT_INFOt.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmpc_imm_port_info_register(int unit);

/*!
 * \brief Initialize the IMM interface for
 * PC_FDR_CONTROLt.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmpc_imm_fdr_port_control_register(int unit);

/*!
 * \brief Initialize the IMM interface for
 * PC_FDR_STATSt.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmpc_imm_port_fdr_stats_register(int unit);

/*!
 * \brief Initialize the IMM interface for
 * PC_PORT_MONITORt.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcmpc_imm_port_monitor_register(int unit);


#endif /* BCMPC_IMM_INTERNAL_H */
