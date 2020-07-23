/*! \file bcmpc_diag.h
 *
 * BCMPC diagnostic APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#ifndef BCMPC_DIAG_H
#define BCMPC_DIAG_H

#include <bcmpc/bcmpc_types_internal.h>
#include <bcmpc/bcmpc_phy.h>

/*!
 * \brief PRBS polynomial type.
 */
typedef enum bcmpc_phy_prbs_poly_e {
    /*! Polynomial of degree 7. */
    BCMPC_PHY_PRBS_POLY_7,

    /*! Polynomial of degree 9. */
    BCMPC_PHY_PRBS_POLY_9,

    /*! Polynomial of degree 11. */
    BCMPC_PHY_PRBS_POLY_11,

    /*! Polynomial of degree 15. */
    BCMPC_PHY_PRBS_POLY_15,

    /*! Polynomial of degree 23. */
    BCMPC_PHY_PRBS_POLY_23,

    /*! Polynomial of degree 31. */
    BCMPC_PHY_PRBS_POLY_31,

    /*! Polynomial of degree 58. */
    BCMPC_PHY_PRBS_POLY_58,

    /*! Polynomial of degree 49. */
    BCMPC_PHY_PRBS_POLY_49,

    /*! Polynomial of degree 10. */
    BCMPC_PHY_PRBS_POLY_10,

    /*! Polynomial of degree 20. */
    BCMPC_PHY_PRBS_POLY_20,

    /*! Polynomial of degree 13. */
    BCMPC_PHY_PRBS_POLY_13,

    /*! Quaternary PRBS13. */
    BCMPC_PHY_PRBS_POLY_Q13,

    /*! Must be the last. */
    BCMPC_PHY_PRBS_POLY_COUNT

} bcmpc_phy_prbs_poly_t;

/*!
 * \brief The bit definition of the PRBS config control flag.
 */
typedef enum bcmpc_phy_prbs_cfg_flag_e {

    /*! Config PRBS for RX direction. */
    BCMPC_PHY_PRBS_CFG_F_RX = 0,

    /*! Config PRBS for TX direction. */
    BCMPC_PHY_PRBS_CFG_F_TX

} bcmpc_phy_prbs_cfg_flag_t;

/*!
 * \brief The bit definition of the PRBS status control flag.
 *
 * This enum defines the bits of the parameter \c flags in
 * \ref bcmpc_phy_prbs_status_get().
 */
typedef enum bcmpc_phy_prbs_status_flag_e {

    /*! Clear error count on read. */
    BCMPC_PHY_PRBS_STATUS_F_CLEAR_ON_READ = 0

} bcmpc_phy_prbs_status_flag_t;

/*!
 * \brief Eyescan mode.
 */
typedef enum bcmpc_phy_eyescan_mode_e {

    /*! Fast eyescan resoultion. */
    BCMPC_PHY_EYESCAN_MODE_FAST = 0,

    /*! Do BER projection. */
    BCMPC_PHY_EYESCAN_MODE_BER_PROJ,

    /*! Must be the last. */
    BCMPC_PHY_EYESCAN_MODE_COUNT

} bcmpc_phy_eyescan_mode_t;

/*!
 * \brief The bit definition of the Eyescan control flag.
 *
 * This enum defines the bits of the parameter \c flags in
 * \ref bcmpc_phy_eyescan_set().
 */
typedef enum bcmpc_phy_eyescan_flag_e {

    /*! Enable eyescan. */
    BCMPC_PHY_EYESCAN_F_ENABLE = 0,

    /*! Process eyescan. */
    BCMPC_PHY_EYESCAN_F_PROCESS,

    /*! Eyescan done. */
    BCMPC_PHY_EYESCAN_F_DONE,

    /*! Don't wait to finish enable operation. */
    BCMPC_PHY_EYESCAN_F_ENABLE_DONT_WAIT

} bcmpc_phy_eyescan_flag_t;

/*!
 * \brief PRBS configuration parameter.
 */
typedef struct bcmpc_phy_prbs_cfg_s {

    /*! Polynomial type. */
    bcmpc_phy_prbs_poly_t poly;

    /*! Invert PRBS data. */
    uint32_t invert;

} bcmpc_phy_prbs_cfg_t;

/*!
 * \brief PRBS status.
 */
typedef struct bcmpc_phy_prbs_status_s {

    /*! Whether PRBS is currently locked. */
    uint32_t prbs_lock;

    /*! Whether PRBS was unlocked since last call. */
    uint32_t prbs_lock_loss;

    /*! PRBS errors count. */
    uint32_t error_count;

} bcmpc_phy_prbs_status_t;

/*!
 * \brief Eyescan configuration.
 */
typedef struct bcmpc_phy_eyescan_options_s {

    /*! Sampling time in millisecond. */
    uint32_t sample_time;

    /*! The maximum horizontal value. */
    int horz_max;

    /*! The minimum horizontal value. */
    int horz_min;

    /*! The horizontal step value. */
    int hstep;

    /*! The maximum vertical value. */
    int vert_max;

    /*! The minimum vertical value. */
    int vert_min;

    /*! The vertical step value. */
    int vstep;

    /*! For BER projection Controls the direction and polarity of the test. */
    uint32_t ber_proj_scan_mode;

    /*! BER projection total measurement time in units of ~1.3 seconds. */
    uint32_t ber_proj_timer_cnt;

    /*!
     * BER projection the error threshold it uses to step to next measurement
     * in units of 16.
     */
    uint32_t ber_proj_err_cnt;

} bcmpc_phy_eyescan_options_t;

/*!
 * BCMPC PHY BER projection stages.
 */
/*! Pre-config stage for optimized errcnt threshold. */
#define BCMPC_PORT_PHY_BER_PROJ_P_PRE           (1 << 0)

/*! Config Aanlyzer per port. */
#define BCMPC_PORT_PHY_BER_PROJ_P_CONFIG        (1 << 1)

/*! Start error accumulation per port. */
#define BCMPC_PORT_PHY_BER_PROJ_P_START         (1 << 2)

/*! Collect error per port. */
#define BCMPC_PORT_PHY_BER_PROJ_P_COLLECT       (1 << 3)

/*! Caculate BER per port. */
#define BCMPC_PORT_PHY_BER_PROJ_P_CALC          (1 << 4)

/*! FEC frame size. */
#define BCMPC_PORT_FEC_FRAME_SIZE_RS_544         5440

/*! FEC corrected counter. */
#define BCMPC_PHY_FEC_CORRECTED_COUNTER       (1 << 0)

/*! FEC uncorrected counter. */
#define BCMPC_PHY_FEC_UNCORRECTED_COUNTER     (1 << 1)

/*!
 * \brief Berproj mode.
 */
typedef enum bcmpc_phy_ber_proj_mode_e {

    /*! Post-fec error projection. */
    BCMPC_PHY_BERPROJ_MODE_POST_FEC,

    /*! Must be the last. */
    BCMPC_PHY_BERPROJ_MODE_COUNT

} bcmpc_phy_ber_proj_mode_t;

/*!
 * \brief Number of PRBS Error Analyzer Counters
 */
#define BCMPC_PRBS_NUM_OF_ERR_ANALYZER_COUNTERS 16
/*! RRBS error count. */
typedef struct bcmpc_phy_prbs_errcnt_s{

    /*! Error count. */
    uint32_t prbs_errcnt[BCMPC_PRBS_NUM_OF_ERR_ANALYZER_COUNTERS];

    /*! Histogram error count threshold. */
    uint32_t start_hist_errcnt_thresh;

    /*! Max error count threshold. */
    uint32_t max_errcnt_thresh;

   /*! projected BER. */
    uint32_t prbs_proj_ber;

} bcmpc_phy_prbs_errcnt_t;

/*!
 * \brief Berproj configuration.
 */
typedef struct bcmpc_phy_ber_proj_options_s {

    /*! Stage flag for BER projection. */
    uint32_t ber_proj_phase;

    /*! FEC frame size in bits. */
    uint32_t ber_proj_fec_size;

    /*! Histogram error threshold. */
    uint32_t ber_proj_hist_errcnt_thresh;

    /*! Measuremnt time that PRBS errors are accumulated. */
    uint32_t ber_proj_timeout;

    /*! Arrays to store PRBS errors. */
    bcmpc_phy_prbs_errcnt_t* ber_proj_prbs_errcnt;

    /*! Flag to avoid printing BER proj results to console. */
    uint32_t without_print;

} bcmpc_phy_ber_proj_options_t;

/*! The types of FEC type. */
typedef enum  bcmpc_cmd_fec_e {

    /*! Disable FEC. */
    BCMPC_CMD_PORT_FEC_NONE = 0,

    /*! BASE-R FEC (IEEE 802.3 clause 74). */
    BCMPC_CMD_PORT_FEC_BASE_R,

    /*! RS 528 FEC. */
    BCMPC_CMD_PORT_FEC_RS_528,

    /*! RS 544 FEC, using 1xN RS FEC architecture. */
    BCMPC_CMD_PORT_FEC_RS_544,

    /*! RS 272 FEC, using 1xN RS FEC architecture. */
    BCMPC_CMD_PORT_FEC_RS_272,

    /*! RS 544 FEC, using 2xN RS FEC architecture. */
    BCMPC_CMD_PORT_FEC_RS_544_2XN,

    /*! RS 272 FEC, using 2xN RS FEC architecture. */
    BCMPC_CMD_PORT_FEC_RS_272_2XN,

    /*! Number of FEC types. */
    BCMPC_CMD_PORT_FEC_COUNT

} bcmpc_cmd_fec_t;


/*! The types of FEC type. */
typedef struct bcmpc_cmd_port_cfg_s {

    /*! Speed. */
    uint32_t speed;

    /*! number of lanes. */
    int lanes;

    /*! FEC type. */
    bcmpc_cmd_fec_t fec_type;

} bcmpc_cmd_port_cfg_t;

/*! The RS FEC symbol error. */
typedef struct bcmpc_phy_rsfec_symb_errcnt_s {

    /*! Max error count. */
    int max_count;

    /*! Actual error count. */
    int actual_count;

    /*! Symbol error count. */
    uint32_t *symbol_errcnt;

} bcmpc_phy_rsfec_symb_errcnt_t;

/*!
 * \brief Set the PRBS configurations.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physial port number.
 * \param [in] lane_idx Logical lane index of the port.
 * \param [in] flags Control flags. \ref bcmpc_phy_prbs_cfg_flag_t.
 * \param [in] prbs PRBS configuration to set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fail to set the configuration.
 */
extern int
bcmpc_phy_prbs_config_set(int unit, bcmpc_pport_t pport, int lane_idx,
                          uint32_t flags, bcmpc_phy_prbs_cfg_t *prbs);

/*!
 * \brief Set the PRBS enable state.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physial port number.
 * \param [in] lane_idx Logical lane index of the port.
 * \param [in] flags Control flags. \ref bcmpc_phy_prbs_cfg_flag_t.
 * \param [in] enable Enable/Disable PRBS.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fail to enable/disable PRBS.
 */
extern int
bcmpc_phy_prbs_enable_set(int unit, bcmpc_pport_t pport, int lane_idx,
                          uint32_t flags, uint32_t enable);

/*!
 * \brief Get the PRBS status.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] lane_idx Logical lane index of the port.
 * \param [in] flags Control flags. \ref bcmpc_phy_prbs_status_flag_t.
 * \param [out] status PRBS status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fail to get the status.
 */
extern int
bcmpc_phy_prbs_status_get(int unit, bcmpc_pport_t  pport, int lane_idx,
                          uint32_t flags, bcmpc_phy_prbs_status_t *status);

/*!
 * \brief Execute eyescan.
 *
 * This funcion would run the eyescan process and display the result when done.
 * The flags is use to control the phases of the eyescan test.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] flags Control flags. \ref bcmpc_phy_eyescan_flag_t.
 * \param [in] mode Eyescan mode.
 * \param [in] option Eyescan options. It it used only when eyescan mode is
               BCMPC_PHY_EYESCAN_MODE_BER_PROJ.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fail to run eyescan.
 */
extern int
bcmpc_phy_eyescan_set(int unit, bcmpc_pport_t pport, uint32_t flags,
                       bcmpc_phy_eyescan_mode_t *mode,
                       bcmpc_phy_eyescan_options_t *option);

/*!
 * \brief Phy diagnostics pmd info dump.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical device port.
 * \param [in] type PHY pmd info type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fail to read data from PHY register.
 */
extern int
bcmpc_phy_pmd_info_dump(int unit, bcmpc_pport_t pport, const char *type);

/*!
 * \brief Phy diagnostics prbs configuration set.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical device port.
 * \param [in] flags prbs configuration flags.
 * \param [in] prbs_cfg prbs configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fail to read data from PHY register.
 */
extern int
bcmpc_phy_prbs_set(int unit, bcmpc_pport_t pport, uint32_t flags,
                   bcmpc_phy_prbs_cfg_t *prbs_cfg);

/*!
 * \brief Phy diagnostics prbs configuration set.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical device port.
 * \param [in] mode BER projection mode.
 * \param [in] proj_cfg BER projection configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fail to read data from PHY register.
 */
extern int
bcmpc_phy_ber_proj(int unit, bcmpc_pport_t pport,
                   bcmpc_phy_ber_proj_mode_t mode,
                   bcmpc_phy_ber_proj_options_t *proj_cfg);

/*!
 * \brief Get number of lanes on the port.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical device port.
 * \param [out] lane_nums Number of lanes.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fail to read data from PHY register.
 */
extern int
bcmpc_phy_lane_num_get(int unit, bcmpc_pport_t pport, int *lane_nums);

/*!
 * \brief Get prbs status.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical device port.
 * \param [in] lane_idx Lane index.
 * \param [out] value Prbs status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fail to read data from PHY register.
 */
extern int
bcmpc_phy_prbs_rx_status_get(int unit, bcmpc_pport_t pport, int lane_idx,
                             uint32_t *value);

/*!
 * \brief Get prbs status.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical device port.
 * \param [out] port_cfg Port configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fail to read data from PHY register.
 */
extern int
bcmpc_phy_port_port_cfg_get(int unit, bcmpc_pport_t pport,
                             bcmpc_cmd_port_cfg_t *port_cfg);

/*!
 * \brief Get FEC corrected/uncorrected counters.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical device port.
 * \param [in] fec_type FEC type.
 * \param [in] flags flags to indicate corrected/uncorrected count.
 * \param [out] value Counter value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fail to read data from PHY register.
 */
extern int
bcmpc_phy_fec_counter_get(int unit, bcmpc_pport_t pport,
                          bcmpc_cmd_fec_t fec_type, uint32_t flags,
                           uint32_t *value);

/*!
 * \brief Get FEC corrected/uncorrected counters.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical device port.
 * \param [out] port_cfg Counter value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fail to read data from PHY register.
 */
extern int
bcmpc_phy_port_port_cfg_get(int unit, bcmpc_pport_t pport,
                            bcmpc_cmd_port_cfg_t *port_cfg);

/*!
 * \brief Get FEC symbol counters.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical device port.
 * \param [out] sym_err_count Symbol error counter value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fail to read data from PHY register.
 */
extern int
bcmpc_phy_rsfec_sym_counter_get(int unit, bcmpc_pport_t pport,
                                bcmpc_phy_rsfec_symb_errcnt_t *sym_err_count);

/*!
 * \brief Channel Analysis Tool collecting data on links
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical device port.
 * \param [in] lane Lane number to operate on
 * \param [in] mode_val Set port mode to TX/RX/LPBK
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Fail to read data from PHY register.
 */
extern int
bcmpc_phy_linkcat(int unit, bcmpc_pport_t pport, int lane, int mode_val);

#endif /* BCMPC_DIAG_H */
