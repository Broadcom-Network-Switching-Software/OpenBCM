/*! \file bcmcth_ts_bs_uc.h
 *
 * BCMCTH TS BS uC interface APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_TS_BS_UC_H
#define BCMCTH_TS_BS_UC_H

#include <sal/sal_types.h>
#include <shr/shr_bitop.h>
#include <shr/shr_util_pack.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/chip/bcmltd_common_enum_ctype.h>

/*! Expand shorthand enums to LTD enum naming style. */
#define LTD_ENUM(_type_, _member_) \
    BCMLTD_COMMON_DEVICE_TS_BROADSYNC_ ## _type_ ## _T_T_ ## _member_

/*! Shorthand macro for the LTD enum. */
#define OPERATIONAL_STATE(_state_) LTD_ENUM(OPERATIONAL_STATE, _state_)

/*! Data structure for DEVICE_TS_BROADSYNC_CONTROL LT. */
typedef struct bcmcth_ts_bs_control_s {
    /*! Corresponds to BROADSYNC field in the LT. */
    bool broadsync;

    /*! Corresponds to OPERATIONAL_STATE field in the LT. */
    bcmltd_common_device_ts_broadsync_operational_state_t_t state;
} bcmcth_ts_bs_control_t;

/*! Definition for time spec.  */
typedef struct time_spec_s {
    /*! Sign identifier. */
    bool sign;

    /*! Seconds absolute value. */
    uint64_t sec;

    /*! Nanoseconds absolute value. */
    uint32_t nsec;
} time_spec_t;

/*! Data structure for DEVICE_TS_BROADSYNC_INTERFACE LT. */
typedef struct bcmcth_ts_bs_interface_s {
    /*! Bitmap of modified field Ids. */
    SHR_BITDCLNAME(fbmp, DEVICE_TS_BROADSYNC_INTERFACEt_FIELD_COUNT);

    /*! Interface Id. {sid, intf_id} form an unique pair. */
    uint32_t intf_id;

    /*! BS mode output/input */
    bcmltd_common_device_ts_broadsync_interface_mode_t_t mode;

    /*! BS bitclock freq in Hz. */
    uint32_t bitclk_hz;

    /*! BS heartbeat freq in Hz. */
    uint32_t hb_hz;

    /*! BS time. */
    time_spec_t time;

    /*! phase adjustment. */
    time_spec_t phase_adj;

    /*! frequency adjustment. */
    time_spec_t freq_adj;

    /*! Corresponds to OPERATIONAL_STATE field in the LT. */
    bcmltd_common_device_ts_broadsync_operational_state_t_t state;
} bcmcth_ts_bs_interface_t;

/*! Data structure for DEVICE_TS_BROADSYNC_STATUS LT. */
typedef struct bcmcth_ts_bs_interface_status_s {
    /*! Interface Id. */
    uint32_t intf_id;

    /*! Status */
    bcmltd_common_device_ts_broadsync_state_t_t status;
} bcmcth_ts_bs_interface_status_t;

/*! Data structure for DEVICE_TS_BROADSYNC_SIGNAL LT. */
typedef struct bcmcth_ts_bs_signal_s {
    /*! Signal Id. */
    uint32_t signal_id;

    /*! Frequency. */
    uint32_t frequency;

    /*! Corresponds to OPERATIONAL_STATE field in the LT. */
    bcmltd_common_device_ts_broadsync_operational_state_t_t state;
} bcmcth_ts_bs_signal_t;

/*! Data structure for DEVICE_TS_BROADSYNC_LOG_CONTROL LT. */
typedef struct bcmcth_ts_bs_logctrl_s {
    /*! Debug log mask. */
    uint32_t debug_mask;

    /*! UDP log mask. */
    uint64_t udp_log_mask;

    /*! Source MAC address. */
    shr_mac_t src_mac;

    /*! Destination MAC address. */
    shr_mac_t dst_mac;

    /*! Tag Protocol Identifier. */
    uint16_t tpid;

    /*! VLAN identifier. */
    uint16_t vlan_id;

    /*! Time to Live. */
    uint8_t ttl;

    /*! Source IP address. */
    uint32_t src_ip;

    /*! Destination IP address. */
    uint32_t dst_ip;

    /*! Source port number. */
    uint16_t src_port;

    /*! Destination port number. */
    uint16_t dst_port;

    /*! Corresponds to OPERATIONAL_STATE field in the LT. */
    bcmltd_common_device_ts_broadsync_operational_state_t_t state;
} bcmcth_ts_bs_logctrl_t;

/*!
 * \brief Alloc memory required for the BS EApp.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Flag indicating whether it is a cold or a warm boot.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */

extern int
bcmcth_ts_bs_uc_alloc(int unit, bool warm);

/*!
 * \brief Free memory used by the BS EApp.
 *
 * \param [in] unit Unit number.
 */
extern int
bcmcth_ts_bs_uc_free(int unit);

/*!
 * \brief TS_BROADSYNC_CONTROL LT insert handler.
 *
 * This function handles the INSERT opcode for the TS_BROADSYNC_CONTROL LT. This
 * function will inform the app about the changes by sending a message to it.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Structure representation of the TS BS control entry. The
 *                   fields not filled in by user are populated with the default
 *                   values.
 * \param [out] oper_state operational state that must be
 *              updated in IMM.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_ts_bs_control_insert(int unit,
    bcmcth_ts_bs_control_t *entry,
    bcmltd_common_device_ts_broadsync_operational_state_t_t *oper_state);

/*!
 * \brief TS_BROADSYNC_CONTROL LT update handler.
 *
 * This function handles the UPDATE opcode for the TS_BROADSYNC_CONTROL LT. This
 * function will inform the app about the changes by sending a message to it.
 *
 * \param [in] unit Unit number.
 * \param [in] cur Structure representing the view of the IMM entry after the
 *                 update operation has completed.
 * \param [in] prev Structure representing the current view of the IMM entry
 *                  with defaults populated for the unfilled values.
 * \param [out] oper_state operational state that must be
 *              updated in IMM.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_ts_bs_control_update(int unit,
              bcmcth_ts_bs_control_t *cur,
              bcmcth_ts_bs_control_t *prev,
              bcmltd_common_device_ts_broadsync_operational_state_t_t *oper_state);

/*!
 * \brief TS_BROADSYNC_CONTROL LT delete handler.
 *
 * This function handles the DELETE opcode for the TS_BROADSYNC_CONTROL LT. This
 * function will inform the app about the changes by sending a message to it.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_ts_bs_control_delete(int unit);

/*!
 * \brief TS_BROADSYNC_INTERFACE LT insert handler.
 *
 * This function handles the INSERT opcode for the TS_BROADSYNC_INTERFACE LT.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Structure representation of the TS BS interface entry. The
 *                   fields not filled in by user are populated with the default
 *                   values.
 * \param [out] oper_state operational state that must be
 *              updated in IMM.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_ts_bs_interface_insert(int unit,
    bcmcth_ts_bs_interface_t *entry,
    bcmltd_common_device_ts_broadsync_operational_state_t_t *oper_state);

/*!
 * \brief TS_BROADSYNC_INTERFACE LT update handler.
 *
 * This function handles the UPDATE opcode for the TS_BROADSYNC_INTERFACE LT.
 *
 * \param [in] unit Unit number.
 * \param [in] cur Structure representing the view of the IMM entry after the
 *                 update operation has completed.
 * \param [in] prev Structure representing the current view of the IMM entry
 *                  with defaults populated for the unfilled values.
 * \param [out] oper_state operational state that must be
 *              updated in IMM.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_ts_bs_interface_update(int unit,
              bcmcth_ts_bs_interface_t *cur,
              bcmcth_ts_bs_interface_t *prev,
              bcmltd_common_device_ts_broadsync_operational_state_t_t *oper_state);

/*!
 * \brief TS_BROADSYNC_INTERFACE LT lookup handler.
 *
 * This function handles the LOOKUP opcode for Broadsync interface
 * LT. This function will retreive the time from the app by sending a message
 * to it.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id interface ID.
 * \param [out] entry Structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_ts_bs_interface_lookup(int unit,
                           uint32_t intf_id,
                           bcmcth_ts_bs_interface_t *entry);


/*!
 * \brief TS_BROADSYNC_INTERFACE LT delete handler.
 *
 * This function handles the DELETE opcode for the TS_BROADSYNC_INTERFACE LT.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */

extern int
bcmcth_ts_bs_interface_delete(int unit);

/*!
 * \brief TS_BROADSYNC_SIGNAL_OUTPUT LT insert handler.
 *
 * This function handles the INSERT opcode for the TS_BROADSYNC_SIGNAL_OUTPUT LT.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Structure representation of the TS BS signal output entry. The
 *                   fields not filled in by user are populated with the default
 *                   values.
 * \param [out] oper_state operational state that must be
 *              updated in IMM.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_ts_bs_signal_insert(int unit,
    bcmcth_ts_bs_signal_t *entry,
    bcmltd_common_device_ts_broadsync_operational_state_t_t *oper_state);

/*!
 * \brief TS_BROADSYNC_SIGNAL_OUTPUT LT update handler.
 *
 * This function handles the UPDATE opcode for the TS_BROADSYNC_SIGNAL_OUTPUT LT.
 *
 * \param [in] unit Unit number.
 * \param [in] cur Structure representing the view of the IMM entry after the
 *                 update operation has completed.
 * \param [in] prev Structure representing the current view of the IMM entry
 *                  with defaults populated for the unfilled values.
 * \param [out] oper_state operational state that must be
 *              updated in IMM.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_ts_bs_signal_update(int unit,
              bcmcth_ts_bs_signal_t *cur,
              bcmcth_ts_bs_signal_t *prev,
              bcmltd_common_device_ts_broadsync_operational_state_t_t *oper_state);

/*!
 * \brief TS_BROADSYNC_SIGNAL_OUTPUT LT delete handler.
 *
 * This function handles the DELETE opcode for the TS_BROADSYNC_SIGNAL_OUTPUT LT.
 *
 * \param [in] unit Unit number.
 *
 * \param [in] signal Signal number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */

extern int
bcmcth_ts_bs_signal_delete(int unit, uint32_t signal);

/*!
 * \brief TS_BROADSYNC_LOG_CONTROL LT insert handler.
 *
 * This function handles the INSERT opcode for the TS_BROADSYNC_LOG_CONTROL LT.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Structure representation of the TS BS log control entry. The
 *                   fields not filled in by user are populated with the default
 *                   values.
 * \param [out] oper_state operational state that must be
 *              updated in IMM.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_ts_bs_logctrl_insert(int unit,
    bcmcth_ts_bs_logctrl_t *entry,
    bcmltd_common_device_ts_broadsync_operational_state_t_t *oper_state);

/*!
 * \brief TS_BROADSYNC_LOG_CONTROL LT update handler.
 *
 * This function handles the UPDATE opcode for the TS_BROADSYNC_LOG_CONTROL LT.
 *
 * \param [in] unit Unit number.
 * \param [in] cur Structure representing the view of the IMM entry after the
 *                 update operation has completed.
 * \param [in] prev Structure representing the current view of the IMM entry
 *                  with defaults populated for the unfilled values.
 * \param [out] oper_state operational state that must be
 *              updated in IMM.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_ts_bs_logctrl_update(int unit,
              bcmcth_ts_bs_logctrl_t *cur,
              bcmcth_ts_bs_logctrl_t *prev,
              bcmltd_common_device_ts_broadsync_operational_state_t_t *oper_state);

/*!
 * \brief TS_BROADSYNC_LOG_CONTROL LT delete handler.
 *
 * This function handles the DELETE opcode for the TS_BROADSYNC_LOG_CONTROL LT.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */

extern int
bcmcth_ts_bs_logctrl_delete(int unit);

/*!
 * \brief TS_BROADSYNC_LOG_CONTROL LT lookup handler.
 *
 * This function handles the LOOKUP opcode for BROADSYNC_LOG_CONTROL LT.
 *
 * \param [in] unit Unit number.
 * \param [out] oper_state operational state that must be updated in IMM.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_ts_bs_logctrl_lookup(int unit,
              bcmltd_common_device_ts_broadsync_operational_state_t_t *oper_state);

/*!
 * \brief TS_BROADSYNC_STATUS LT lookup handler.
 *
 * This function handles the LOOKUP opcode for Broadsync interface status
 * LTs. This function will retreive the status from the app by sending a message
 * to it.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id interface ID.
 * \param [out] entry Structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_ts_bs_status_lookup(int unit,
                           uint32_t intf_id,
                           bcmcth_ts_bs_interface_status_t *entry);

#endif /* BCMCTH_TS_BS_UC_H */
