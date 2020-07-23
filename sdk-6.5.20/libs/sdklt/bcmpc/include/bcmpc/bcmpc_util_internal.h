/*! \file bcmpc_util_internal.h
 *
 *  BCMPC internal utilites.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPC_UTIL_INTERNAL_H
#define BCMPC_UTIL_INTERNAL_H

#include <bcmdrd/bcmdrd_dev.h>

#include <bcmpc/bcmpc_topo_internal.h>
#include <bcmpc/bcmpc_drv_internal.h>
#include <bcmpc/bcmpc_util.h>
#include <bcmpc/bcmpc_pm.h>
#include <bcmbd/bcmbd_port_intr.h>
#include <bcmpc/bcmpc_lt_cfg.h>


/*! Interrupt controls. */
typedef enum
bcmpc_intr_enable_e {
    /*! Interrupt enable. */
    INTR_ENABLE = 0,

    /*! Interrupt disable. */
    INTR_DISABLE = 1,

    /*! Clear the interrupts. */
    INTR_CLEAR = 2
} bcmpc_intr_enable_t;

/*!
 * \brief Compare the data of two same-type array variables.
 *
 * \c _var1 and \c _var2 are two array variables with the same data type.
 *
 * \param [in] _var1 The first variable.
 * \param [in] _var2 The second variable.
 *
 * \retval 1 The two arrays are identical.
 * \retval 0 The two arrays are different.
 */
#define BCMPC_ARRAY_EQUAL(_var1, _var2) \
    (sal_memcmp(_var1, _var2, sizeof(_var1)) == 0)

/*!
 * \brief Compare the first number of datas of two same-type array variables.
 *
 * \c _var1 and \c _var2 are two array variables with the same data type.
 * \c _cnt tells the number of datas to compare.
 *
 * \param [in] _var1 The first variable.
 * \param [in] _var2 The second variable.
 * \param [in] _cnt Number of datas to compare.
 *
 * \retval 1 The first number of datas of the two arrays are identical.
 * \retval 0 The first number of datas of the two arrays are different.
 */
#define BCMPC_ARRAY_PARTIAL_EQUAL(_var1, _var2, _cnt) \
    (sal_memcmp(_var1, _var2, _cnt * sizeof(_var1[0])) == 0)

/*!
 * \brief Compare the field value in two same-type variables.
 *
 * \c _var1 and \c _var2 are two variables with the same data type.
 * \c _f is a field name in the data structure.
 *
 * \param [in] _var1 The first variable.
 * \param [in] _var2 The second variable.
 * \param [in] _f field name.
 *
 * \retval 1 The value of field \c _f is same in \c _var1 and \c _var2.
 * \retval 0 The value of \c _f is different.
 */
#define BCMPC_FIELD_VAL_EQUAL(_var1, _var2, _f) \
    ((_var1)->_f == (_var2)->_f)

/*!
 * \brief Return the size of the variable in bits.
 *
 * \param [in] _var The variable to get the size.
 *
 * \return The size of \c _var in bits.
 */
#define BCMPC_SIZEOF_BITS(_var) (sizeof(_var) * 8)

/*!
 * \brief Get absolute value.
 *
 * \param [in] _val The value to get the absolute value.
 *
 * \return The absolute value of \c _val.
 */
#define BCMPC_ABS(_val) ((_val > 0) ? _val : -_val)

/*!
 * \brief Check if the given value is negative.
 *
 * \param [in] _val The value to check.
 *
 * \retval true The given value is negative.
 * \retval false The given value is positive.
 */
#define BCMPC_IS_NEGATIVE(_val) (_val < 0)

/*!
 * \brief Error-exit on PHYMOD operation error.
 *
 * Evaluate an expression of PHYMOD and if it evaluates to a PHYMOD error
 * code, then log a error message and go to the function's single
 * point of exit with a SHR error, SHR_E_FAIL.
 *
 * The log message will use BSL_LOG_MODULE as the log source, so this
 * name must be assigned to a relevant BSL log source in advance.
 *
 * \param [in] _expr Expression to evaluate.
 */
#define PHY_IF_ERR_EXIT(_expr) \
    do {   \
        int _phy_err = _expr; \
        if (_phy_err != PHYMOD_E_NONE) { \
            LOG_WARN(BSL_LOG_MODULE, \
                     (BSL_META_U(_func_unit, "PHYMOD failure[%d]\n"), \
                      _phy_err)); \
            SHR_ERR_EXIT(SHR_E_FAIL); \
        } \
    } while (0)

/*!
 * \brief Error-exit on expression error with exception.
 *
 * Evaluate an expression and if it evaluates to a PHYMOD error code
 * which differs from the exception value, then log a error message
 * and go to the function's single point of exit.
 *
 * The log message will use BSL_LOG_MODULE as the log source, so this
 * name must be assigned to a relevant BSL log source in advance.
 *
 * Typical exception error codes are PHYMOD_E_NOT_FOUND and
 * PHYMOD_E_UNAVAIL, because these error codes may be a simple reflection
 * of the device state or capabilties, which are used to control the
 * logical flow of the function.
 *
 * \param [in] _expr Expression to evaluate.
 * \param [in] _rv_except error code exception.
 */
#define PHY_IF_ERR_EXIT_EXCEPT_IF(_expr, _rv_except)    \
    do {                                                \
        int _phy_err = _expr; \
        if (_phy_err != PHYMOD_E_NONE && _phy_err != _rv_except) { \
            LOG_WARN(BSL_LOG_MODULE, \
                     (BSL_META_U(_func_unit, "PHYMOD failure[%d]\n"), \
                      _phy_err)); \
            SHR_ERR_EXIT(SHR_E_FAIL); \
        } \
    } while(0)

/*!
 * \brief Set a bit in a flag variable.
 *
 * \param _f Flags.
 * \param _b The bit position to set. (First is 0.)
 */
#define BCMPC_BIT_SET(_f, _b) SHR_BITSET(&(_f), _b)

/*!
 * \brief Get a bit in a flag variable.
 *
 * If the \c _b bit is set, this function would return 1; otherwise 0.
 *
 * \param _f Flags.
 * \param _b The bit position to get. (First is 0.)
 */
#define BCMPC_BIT_GET(_f, _b) ((SHR_BITGET(&(_f), _b)) >> _b)

/*!
 * \brief Clear a bit in a flag variable.
 *
 * \param _f Flags.
 * \param _b The bit position to clear. (First is 0.)
 */
#define BCMPC_BIT_CLR(_f, _b) SHR_BITCLR(&(_f), _b)

/*!
 * \brief PM information.
 *
 * The structure describes the resource of a PM which is derived by
 * lookup topology database.
 */
typedef struct bcmpc_pm_info_s {

    /*! Base physical port number. */
    bcmpc_pport_t base_pport;

    /*! Type properities. */
    bcmpc_topo_type_t prop;

} bcmpc_pm_info_t;

/*!
 * \brief Set the PC driver of the device.
 *
 * \param [in] unit Unit number.
 * \param [in] drv PC driver to set.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmpc_drv_set(int unit, bcmpc_drv_t *drv);

/*!
 * \brief Get the PC driver of the device.
 *
 * \param [in] unit Unit number.
 * \param [in] drv PC driver to get.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmpc_drv_get(int unit, bcmpc_drv_t **drv);

/*!
 * \brief Initalize PM resource data structure.
 *
 * \param [in] pm_info Buffer to initalize.
 *
 * \return Nothing.
 */
extern void
bcmpc_pm_info_t_init(bcmpc_pm_info_t *pm_info);

/*!
 * \brief Get the PM resource by a PM ID.
 *
 * This function would lookup topology database to get the PM
 * resource by the PM ID.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_id PM ID.
 * \param [out] pm_info PM resource.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PORT Failed to get the logical PM port resource by \c pm_id.
 */
extern int
bcmpc_pm_info_get(int unit, int pm_id, bcmpc_pm_info_t *pm_info);

/*!
 * \brief Get the PMD common clock for the given physical port.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [out] ref_clk Core reference clock in KHz.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failed to get the core clock.
 */
extern int
bcmpc_pm_pmd_common_clk_get(int unit, bcmpc_pport_t pport, int *ref_clk);


/*!
 * \brief Get the linkscan mode for the given physical port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] lm_mode Logical port linkscan configuration structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failed to get the core clock.
 */
extern int
bcmpc_linkscan_mode_get(int unit, bcmpc_lport_t lport,
                        bcmpc_lm_mode_t *lm_mode);

/*!
 * \brief Get physical port number from PC_PORT_PHYS_MAPt.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [out] pport Physical port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Cannot find the associated \c pport for \c lport.
 */
extern int
bcmpc_port_phys_map_get(int unit, bcmpc_lport_t lport, bcmpc_pport_t *pport);

/*!
 * \brief Initialize the major data of \c phymod_phy_access_t used in BCMPC.
 *
 * The function will use the lane mask of the specifed physical port
 * as the default lane mask in \c phymod_access_t \c access of \c pa.
 * The lane mask of \c phymod_access_t can be override by passing a
 * non-zero value of \c ovrr_lane_mask.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] ovrr_lane_mask Lane mask override value if non-zero.
 * \param [in] phy_data User data for \c phymod_phy_access_t.
 * \param [out] pa Pointer to \c phymod_phy_access_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Argument \c phy_data or \c pa is NULL.
 */
extern int
bcmpc_phymod_phy_access_t_init(int unit, bcmpc_pport_t pport,
                               uint32_t ovrr_lane_mask,
                               bcmpc_phy_access_data_t *phy_data,
                               phymod_phy_access_t *pa);

/*!
 * \brief Initialize the major data of \c phymod_core_access_t used in BCMPC.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 * \param [in] phy_data User data for \c phymod_core_access_t.
 * \param [out] ca Pointer to \c phymod_core_access_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Argument \c phy_data or \c ca is NULL.
 */
extern int
bcmpc_phymod_core_access_t_init(int unit, bcmpc_pport_t pport,
                                bcmpc_phy_access_data_t *phy_data,
                                phymod_core_access_t *ca);

/*!
 * \brief Create a mutex for the given unit.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failed to create the mutex.
 */
extern int
bcmpc_phymod_bus_mutex_init(int unit);

/*!
 * \brief Destroy the mutex of the given unit.
 *
 * \param [in] unit Unit number.
 */
extern void
bcmpc_phymod_bus_mutex_cleanup(int unit);

/*!
 * \brief Common mutex_take() function for PHYMOD bus driver.
 *
 * \param user_acc User data. This function expect to get
 *                 \ref bcmpc_phy_access_data_t.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_phymod_bus_mutex_take(void *user_acc);

/*!
 * \brief Common mutex_give() function for PHYMOD bus driver.
 *
 * \param user_acc User data. This function expect to get
 *                 \ref bcmpc_phy_access_data_t.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
bcmpc_phymod_bus_mutex_give(void *user_acc);

/*!
 * \brief Get the first physical port bitmap of the given block type.
 *
 * This function will return the whole block-port bitmap for the given
 * block type irrespective of the valid port bitmap. In other words,
 * this function will return the same bitmap across all device
 * variants.
 *
 * \param [in] unit Unit number.
 * \param [in] ci Chip info.
 * \param [in] blktype Block type.
 * \param [out] pbmp The first physical port bitmap of \c blktype.
 * \param [out] pbmp_inactive The first physical port bitmap of not
 *              supported \c blktype.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failed to get the first port bitmap of \c blktype.
 */
extern int
bcmpc_block_pbmp_get(int unit, const bcmdrd_chip_info_t *ci, int blktype,
                     bcmdrd_pbmp_t *pbmp, bcmdrd_pbmp_t *pbmp_inactive);

/*!
 * \brief Get the valid first physical port bitmap of the given block type.
 *
 * This function will only return the valid block-port bitmap for the given
 * block type.
 *
 * \param [in] ci Chip info.
 * \param [in] blktype Block type.
 * \param [out] pbmp The first physical port bitmap of \c blktype.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failed to get the first port bitmap of \c blktype.
 */
extern int
bcmpc_valid_block_pbmp_get(const bcmdrd_chip_info_t *ci, int blktype,
                           bcmdrd_pbmp_t *pbmp);

/*!
 * \brief Update port_num_lanes field in bcmpc_topo_t.
 *
 * This function will updaet port_num_lanes field in
 * strcuture bcmpc_topo_t.
 *
 * \param [in] unit Unit number.
 * \param [in] pcfg port configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failed to update port_num_lanes field in bcmpc_topo_t.
 */
extern int
bcmpc_topo_inst_info_update(int unit, bcmpc_port_cfg_t *pcfg);

/*!
 * \brief Set PM type and related parameters in the Port Macro driver database.
 *
 * This function sets PM type with value of enum pm_type_s
 * in \c pm_drv_info->type. It is used by PM for dispatching.
 * The function also sets ref_clk with value of enum phymod_ref_clk_t
 * in \c pm_drv_info->pm_data.ref_clk.
 *
 * \param [in]  unit Unit number.
 * \param [in]  tid Port Macro type ID.
 * \param [out] pm_drv_info Port macro driver information object.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failed to get topo_type by \c tid.
 */
extern int
bcmpc_pm_type_params_set(int unit, int tid, pm_info_t *pm_drv_info);

/*!
 * \brief Get the PortMacro driver resource by a PM ID.
 *
 * This function would lookup topology database to get the PM
 * resource by the PM ID.
 *
 * \param [in] unit Unit number.
 * \param [in] pm_id PM ID.
 * \param [out] pm_info PM resource.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Failed to get the PM resource by \c pm_id.
 */
extern int
bcmpc_pm_drv_info_get(int unit, int pm_id, pm_info_t **pm_info);

/*!
 * \brief BCMPC interrupt control routine.
 *
 * This function is used to enable, disable, clear interrupt or
 * register a callback function for intrrupts.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Device physical port.
 * \param [in] intr_num  PC interrupt type enumeration.
 * \param [in] intr_func Callback functions for interrupts.
 * \param [in] enable Enumeration to specify interrupt operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Failed to get the PM resource by \c pm_id.
 */
extern int
bcmpc_interrupt_enable(int unit,
                       bcmpc_pport_t pport,
                       int intr_num,
                       bcmbd_intr_f intr_func,
                       bcmpc_intr_enable_t enable);

/*!
 * \brief BCMPC interrupt processing routine.
 *
 * This function is used to process interrupts and handle
 * as needed in the driver layer.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Device logical port.
 * \param [in] intr_type  PC interrupt type enumeration.
 * \param [out] is_handled 1 - Interrupt processing successful.
 *                         0 - Interrupt processing failed.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Failed to get the PM resource by \c pm_id.
 */
extern int
bcmpc_interrupt_process(int unit, bcmpc_lport_t lport,
                        bcmpc_intr_type_t intr_type, uint32_t *is_handled);

/*!
 * \brief This function returns complete LT entry with the
 * with user provisioned fields and the default values for
 * the fields not provisioned by user application.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical Table ID.
 * \param [in] key_size key size.
 * \param [in] key Key value.
 * \param [in] data_size Size of data.
 * \param [out] data Structure for data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Input parameter failed validation check.
 */
extern int
bcmpc_lt_entry_get(int unit, bcmltd_sid_t sid,
                   size_t key_size, void *key,
                   size_t data_size, void *data);

/*!
 * \brief Initialize values of all the fields of TX_TAPS lt entry
 * with user provisioned fields and the default values for
 * the fields not provisioned by user application.
 * The caller of this function must have already performed
 * a lookup on the LT. This function checks the field bitmap
 * and populates only the fields which are not provisioned
 * (inicated by the bit not being set) with default value from
 * the LT map file.
 *
 * \param [in] unit Unit number.
 * \param [in] port_lane Key, logical port number and lane index.
 * \param [out] entry PC_TX_TAPs LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Input parameter failed validation check.
 */
extern int
bcmpc_lt_tx_taps_default_values_init(int unit,
                                     bcmpc_port_lane_t *port_lane,
                                     bcmpc_tx_taps_config_t *entry);

/*!
 * Insert the PC_PORT_INFOt LT with the operation status errors
 * from the PM driver.
 *
 * This function is called for each logical port to physical port mapping
 * by traversing the logical port map table.
 *
 * \param [in] unit Unit number.
 * \param [in] key_size Key size, i.e. sizeof(bcmpc_lport_t).
 * \param [in] key The logical port number, bcmpc_lport_t.
 * \param [in] data_size Data size, i.e. sizeof(bcmpc_port_cfg_t).
 * \param [in] data The logical port configuration, bcmpc_port_cfg_t.
 * \param [in] cookie Logical to physical port map configurationi.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
extern int
port_oper_status_table_insert(int unit, size_t key_size, void *key,
                              size_t data_size, void *data, void *cookie);

/*!
 * Populate the PC_PORT_INFOt LT with the operation status errors
 * from the PM driver.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Port configuration.
 * \param [in] op_st Operation status returned for PM driver call.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
port_oper_status_table_populate(int unit, bcmpc_pport_t pport,
                                pm_oper_status_t *op_st);

#endif /* BCMPC_UTIL_INTERNAL_H */
