/*! \file bcma_testutil_drv.h
 *
 * Chip-specific driver utility.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTUTIL_DRV_H
#define BCMA_TESTUTIL_DRV_H

#include <bcmbd/bcmbd.h>
#include <bcma/test/util/bcma_testutil_vlan.h>
#include <bcma/test/util/bcma_testutil_l2.h>
#include <bcma/test/util/bcma_testutil_ctr.h>
#include <bcma/test/util/bcma_testutil_flex.h>
#include <bcma/test/util/bcma_testutil_tm.h>
#include <bcma/test/util/bcma_testutil_power.h>
#include <bcma/test/util/bcma_testutil_qos.h>
#include <bcma/test/util/bcma_testutil_pt_iterate.h>

/*!
 * \brief Physical table test type.
 *
 * It is used to indicate which test type the function will handle.
 */
typedef enum bcma_testutil_pt_type_e {
    /*! Test type of register reset. */
    BCMA_TESTUTIL_PT_REG_RESET = 0,

    /*! Test type of register write/read. */
    BCMA_TESTUTIL_PT_REG_RD_WR,

    /*! Test type of memory write/read. */
    BCMA_TESTUTIL_PT_MEM_RD_WR,

    /*! PT does not support SER test TR144. */
    BCMA_TESTUTIL_PT_MEM_TR_144,

    /*! PT does not support SER test TR901. */
    BCMA_TESTUTIL_PT_MEM_TR_901,

    /*! Test type of memory SBUSDMA write/read. */
    BCMA_TESTUTIL_PT_MEM_DMA_RD_WR,

    /*! PT does not support  SER test TR143 */
    BCMA_TESTUTIL_PT_MEM_TR_143,

    /*! Test type of memory reset. */
    BCMA_TESTUTIL_PT_MEM_RESET,
} bcma_testutil_pt_type_t;

/*!\brief Field name and field value used to configure LT to support packet traffic */
typedef struct bcma_testutil_packet_traffic_fld_config_s {
    /*! LT field name */
    const char *field_name;
    /*! LT field value */
    uint64_t field_val;
    /*! LT field string value */
    const char *field_str_val;
    /*! Number of element of array field. 0: not array field */
    uint32_t fld_index_num;
    /*! Start index of array field. -1: not array field */
    uint32_t fld_start_index;
} bcma_testutil_pkt_fld_cfg_t;

/*!\brief Max number of configured field */
#define BCMA_TESTUTIL_CFG_FLD_MAX_NUM  (8)

/*!\brief Configure LT to support packet traffic */
typedef struct bcma_testutil_packet_traffic_lt_config_s {
    /*! LT name */
    const char *table_name;
    /*! LT field name and value */
    bcma_testutil_pkt_fld_cfg_t fld_data[BCMA_TESTUTIL_CFG_FLD_MAX_NUM];
    /*! Operation code */
    int opcode;
} bcma_testutil_pkt_lt_cfg_t;

/*!\brief packet metadata needs to be sent. */
typedef struct bcma_testutil_pkt_data_s {
    /*! TX Packet metadata handle. */
    uint32_t (*txpmd)[3];
    /*! Length of tx packet metadata */
    uint32_t txpmd_len;
    /*! Loopback Header handle. */
    uint32_t *lbhdr;
    /*! Length of loopback header */
    uint32_t lbhdr_len;
    /*! Higig handle. */
    uint32_t *higig;
    /*! Length of higig */
    uint32_t higig_len;
    /*! Raw data. */
    uint8_t *raw_data;
    /*! Length of raw data */
    uint32_t raw_data_byte_len;
} bcma_testutil_pkt_data_t;

/*! Testing information for CAMBIST */
typedef struct bcma_testutil_mem_cambist_entry_s {
    /*! Control register */
    bcmdrd_sid_t control_reg;

    /*!
     * Control register field : enable
     *    Must have, for cambist to select different cams
     */
#define BCMA_TESTUTIL_MEM_CAMBIST_FIELD_ENABLE_MAX  (20)
    /*! array size */
    int          control_field_enable_num;
    /*! array of field ids */
    bcmdrd_fid_t control_field_enable_arr
                    [BCMA_TESTUTIL_MEM_CAMBIST_FIELD_ENABLE_MAX];
    /*! array of count for each field */
    int          control_field_enable_arr_count
                    [BCMA_TESTUTIL_MEM_CAMBIST_FIELD_ENABLE_MAX];

    /*!
     * Control register field : reset
     *    Optional, for CAMBIST reset if need
     */
    bcmdrd_fid_t control_field_reset;

    /*!
     * Control register field : mode
     *    Optional, select operation mode
     */
#define BCMA_TESTUTIL_MEM_CAMBIST_FIELD_MODE_MAX  (10)
    /*! field id */
    bcmdrd_fid_t control_field_mode;
    /*! array size */
    int          control_field_mode_num;
    /*! array of mode value */
    int          control_field_mode_arr_value
                    [BCMA_TESTUTIL_MEM_CAMBIST_FIELD_MODE_MAX];


    /*! Result register */
    bcmdrd_sid_t result_reg;

    /*!
     * Result register field : status
     *    Must have, the status of testing result
     */
    bcmdrd_fid_t result_field_status;

    /*! the expected value of result */
    uint32_t result_field_status_expect;
} bcma_testutil_mem_cambist_entry_t;

/*! Testing information for MBIST */
typedef struct bcma_testutil_mem_mbist_entry_s {
    /*! Test item title */
    const char *name;

    /*! Number of operation code */
    uint32_t op_count;

    /*! Array of operation code */
    const uint32_t *array_opcode;

    /**@{*/
    /*!
     * Array of pattern
     *    This array describe the most frequent data appeared in operation
     *      control_write : the control value
     *      data_write    : the data value
     *      status_verify : the status value for verify
     *                    (the upper 32-bit is value, the lower 32-bit is mask)
     */
    const uint32_t *pattern_control_write;
    const uint32_t *pattern_data_write;
    const uint64_t *pattern_status_verify;
    /**@}*/

    /**@{*/
    /*!
     * Array of left data
     *    This array describe the data appeared in the left operation
     *      control_write : the control value
     *      data_write    : the data value
     *      status_verify : the status value for verify
     *                    (the upper 32-bit is value, the lower 32-bit is mask)
     */
    const uint8_t *array_control_write;
    const uint32_t *array_data_write;
    const uint64_t *array_status_verify;
    /**@}*/
} bcma_testutil_mem_mbist_entry_t;

/*! Testing information for MBIST DMA */
typedef struct bcma_testutil_mem_mbist_dma_entry_s {
    /*! Test item title */
    const char *name;

    /*! Number of data entries */
    uint32_t entry_num;

    /*! Number of operation code */
    uint32_t op_count;

    /*! Array of operation code */
    const uint8_t *array_opcode;

    /*! Array of data */
    const uint32_t *array_data;

    /*! Array of long pattern */
    const uint32_t *array_long_pattern;

    /*! Array of pattern */
    const uint32_t *array_pattern;
} bcma_testutil_mem_mbist_dma_entry_t;

/*!
 * \brief Check whether the physical table should be skipped.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] type Test Type.
 *
 * \retval TRUE The physical table should be skipped.
 * \retval FALSE The physical table should be tested.
 */
typedef bool (*bcma_testutil_drv_pt_skip_f)(int unit, bcmdrd_sid_t sid,
                                            bcma_testutil_pt_type_t type);

/*!
 * \brief Perform conditional initialization for physical entity.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] type Test Type.
 *
 * \retval SHR_E_NONE No error.
 */
typedef int (*bcma_testutil_drv_cond_init_f)(int unit, bcmdrd_sid_t sid,
                                             bcma_testutil_pt_type_t type);

/*!
 * \brief Perform conditional cleanup for physical entity.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] type Test Type.
 *
 * \retval SHR_E_NONE No error.
 */
typedef int (*bcma_testutil_drv_cond_cleanup_f)(int unit, bcmdrd_sid_t sid,
                                                 bcma_testutil_pt_type_t type);

/*!
 * \brief Override physical table mask if needs special handling.
 *
 * \c mask buffer must be large enough to hold a full table entry of the given \c sid.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] index Primary index if the table has more than a single entry.
 * \param [inout] mask Mask of testable bits in table entry.
 * \param [in] wsize Number of 32-bit words in table entry.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int (*bcma_testutil_drv_pt_mask_override_f)(int unit, bcmdrd_sid_t sid,
                                                    int index, uint32_t *mask,
                                                    int wsize);

/*!
 * \brief Override entity access in physical table iteration.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] dyn_info Dynamic address information.
 * \param [in] entity_handle_cb Entity handler function.
 * \param [in] User_data User specified data passed in handler function.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int (*bcma_testutil_drv_pt_entity_override_f)(
                int unit, bcmdrd_sid_t sid,
                bcmbd_pt_dyn_info_t *dyn_info,
                bcma_testutil_pt_entity_handle_f entity_handle_cb,
                void *user_data);

/*!
 * \brief Get configurations of LTs, in order to configure chip to support sending packet traffic.
 * And get metadata of packet which needs to be sent.
 *
 * \param [in] unit Unit number.
 * \param [out] pkt_lt_cfg Configurations of LTs.
 * \param [out] lt_cfg_num Number of configuration of LTs.
 * \param [out] pkt_data Metadata of packet.
 *
 * \retval None.
 */
typedef void (*bcma_testutil_pkt_cfg_data_get_f)(int unit,
                                                 const bcma_testutil_pkt_lt_cfg_t **pkt_lt_cfg,
                                                 int *lt_cfg_num,
                                                 const bcma_testutil_pkt_data_t **pkt_data);

/*!
 * \brief Enable/disable background hardware update for physical table test.
 *
 * Prevent the hardware overwrite the data written by software.
 *
 * \param [in] unit Unit number.
 * \param [in] name Physical table name.
 * \param [in] type Test Type.
 * \param [in] enable Set to false to disable HW background updates.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int (*bcma_testutil_drv_pt_hw_update_f)(int unit,
                                                const char *name,
                                                bcma_testutil_pt_type_t type,
                                                bool enable);

/*!
 * \brief Get related information for CAM built-in self test (CAMBIST)
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [out] indicate this register is for CAMBIST purpose or not
 * \param [out] CAMBIST information of this register
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int (*bcma_testutil_drv_mem_cambist_info_f)(
                int unit, bcmdrd_sid_t sid, bool *is_bist,
                bcma_testutil_mem_cambist_entry_t *info);

/*!
 * \brief Adjust address range for memory test.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [inout] index_start Start address.
 * \param [inout] index_end End address.
 * \param [in] type Test Type.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int (*bcma_testutil_drv_mem_address_adjust_f)(int unit,
                                                      bcmdrd_sid_t sid,
                                                      int *index_start,
                                                      int *index_end,
                                                      bcma_testutil_pt_type_t type);

/*!
 * \brief Adjust entry data for hash memory test.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [inout] data Data of testable bits in table entry.
 * \param [in] wsize Number of 32-bit words in table entry.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int (*bcma_testutil_drv_mem_hash_data_adjust_f)(int unit,
                                                        bcmdrd_sid_t sid,
                                                        uint32_t *data,
                                                        int wsize);

/*!
 * \brief Write memory using DMA mode.
 *
 * Write data from DMA buffer into a range of table entries.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] index_start Index of the first table entry of the write operation.
 * \param [in] index_end Index of the last table entry of the write operation.
 * \param [in] tbl_inst Table instance.
 * \param [in] buf DMA buffer for table entries to be written.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int (*bcma_testutil_drv_mem_range_write_f)(int unit,
                                                   bcmdrd_sid_t sid,
                                                   int index_start,
                                                   int index_end,
                                                   int inst,
                                                   uint64_t buf);

/*!
 * \brief Read memory using DMA mode.
 *
 * Read data from a range of table entries into a DMA buffer.
 * The buffer size is index count * byte size of table entry.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] index_start Index of the first table entry of the read operation.
 * \param [in] index_end Index of the last table entry of the read operation.
 * \param [in] tbl_inst Table instance.
 * \param [out] buf DMA buffer large enough to hold the specified range of table entries.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int (*bcma_testutil_drv_mem_range_read_f)(int unit,
                                                  bcmdrd_sid_t sid,
                                                  int index_start,
                                                  int index_end,
                                                  int inst,
                                                  uint64_t buf);

/*!
 * \brief Required configuration for Memory built-in self test (MBIST)
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int (*bcma_testutil_drv_mem_mbist_preconfig_f)(int unit);

/*!
 * \brief Get related information for Memory built-in self test (MBIST)
 *
 * \param [in] unit Unit number.
 * \param [out] array of MBIST information
 * \param [out] number of MBIST information
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int (*bcma_testutil_drv_mem_mbist_info_f)(
                int unit,
                const bcma_testutil_mem_mbist_entry_t **test_array,
                int *test_num);

/*!
 * \brief Get related information for Memory built-in self test (MBIST) with DMA
 *
 * \param [in] unit Unit number.
 * \param [out] array of MBIST information
 * \param [out] number of MBIST information
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int (*bcma_testutil_drv_mem_mbist_dma_info_f)(
                int unit,
                const bcma_testutil_mem_mbist_dma_entry_t **test_array,
                int *test_num);

/*!
 * \brief Get exepected traffic rate per port
 *
 * \param [in] unit Unit number.
 * \param [in] pbmp Bitmap for all ports
 * \param [in] pbmp_oversub Bitmap for oversubscribed ports
 * \param [in] array_size Array size of ret_exp_rate
 * \param [out] ret_exp_rate Returned expected rate for each port
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int (*bcma_testutil_drv_traffic_get_expeceted_rate_f)(
                int unit, bcmdrd_pbmp_t pbmp, bcmdrd_pbmp_t pbmp_oversub,
                int pkt_size, int array_size, uint64_t *ret_exp_rate);

/*!
 * \brief Get list with fixed packet length for traffic testing
 *
 * \param [in] unit Unit number.
 * \param [in] pkt_len Packet length
 * \param [out] ret_pkt_cnt Returned packet count
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int (*bcma_testutil_drv_traffic_get_fixed_packet_list_f)(
                int unit, uint32_t pkt_len, uint32_t *ret_pkt_cnt);

/*!
 * \brief Get list with random packet length for traffic testing
 *
 * \param [in] unit Unit number.
 * \param [in] cellnums_limit Upper limit of cell numbers per packet
 * \param [in] array_size Array size of ret_pkt_len
 * \param [out] ret_pkt_len Returned array of packet length
 * \param [out] ret_pkt_cnt Returned packet count
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int (*bcma_testutil_drv_traffic_get_random_packet_list_f)(
                int unit, uint32_t cellnums_limit, uint32_t array_size,
                uint32_t *ret_pkt_len, uint32_t *ret_pkt_cnt);

/*!
 * \brief Get packet length in the worst case for traffic testing
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] ret_pkt_len Returned array of packet length
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int (*bcma_testutil_drv_traffic_get_worstcase_pktlen_f)(
                int unit, int port, uint32_t *ret_pkt_len);

/*!
 * \brief Get testing register list for CPU benchmark
 *
 * \param [in] unit Unit number.
 * \param [out] reg_arr Testing register list.
 * \param [out] reg_cnt Number of testing register.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int (*bcma_testutil_drv_benchmark_reglist_get_f)
                (int unit, const bcmdrd_sid_t **reg_arr, int *reg_cnt);

/*!
 * \brief Get testing memory list for CPU benchmark
 *
 * \param [in] unit Unit number.
 * \param [out] mem_arr Testing memory list.
 * \param [out] mem_cnt Number of testing memory.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int (*bcma_testutil_drv_benchmark_memlist_get_f)
                (int unit, const bcmdrd_sid_t **mem_arr, int *mem_cnt);

/*!
 * \brief Get l2 operations handler
 *
 * \param [in] unit Unit number.
 *
 * \return handler of l2 operations.
 */
typedef bcma_testutil_l2_op_t *(*bcma_testutil_drv_l2_op_get_f)(int unit);

/*!
 * \brief Get counter operations handler
 *
 * \param [in] unit Unit number.
 *
 * \return handler of counter operations.
 */
typedef bcma_testutil_ctr_op_t *(*bcma_testutil_drv_ctr_op_get_f)(int unit);

/*!
 * \brief Get vlan operations handler
 *
 * \param [in] unit Unit number.
 *
 * \return handler of vlan operations.
 */
typedef bcma_testutil_vlan_op_t *(*bcma_testutil_drv_vlan_op_get_f)(int unit);

/*!
 * \brief Get flex_lt operations handler
 *
 * \param [in] unit Unit number.
 *
 * \return handler of flex operations on logical tables.
 */
typedef bcma_testutil_flex_lt_op_t *(*bcma_testutil_drv_flex_lt_op_get_f)(
                                        int unit);

/*!
 * \brief Get flex traffic operations handler
 *
 * \param [in] unit Unit number.
 *
 * \return handler of flex operations on traffic test.
 */
typedef bcma_testutil_flex_traffic_op_t
    *(*bcma_testutil_drv_flex_traffic_op_get_f)(int unit);

/*!
 * \brief Get QOS operations handler
 *
 * \param [in] unit Unit number.
 *
 * \return handler of qos operations on logical tables.
 */
typedef bcma_testutil_qos_lt_op_t *(*bcma_testutil_drv_qos_lt_op_get_f)(
                                        int unit);

/*!
 * \brief Get CMC channel information.
 *
 * \param [in] unit Unit number.
 * \param [out] cmc_num CMC number.
 * \param [out] cmc_chan_num Channel number per CMC.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int (*bcma_testutil_drv_cmc_chan_get_f)(int unit, int *cmc_num,
                                                int *cmc_chan_num);

/*!
 * \brief Get block type ID from its name.
 *
 * \param [in] unit Unit number.
 * \param [in] blktype_name Block type name.
 * \param [out] blktype_id Block type ID which match to the name.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int (*bcma_testutil_drv_chip_blktype_get_f)(int unit,
                                                    const char *blktype_name,
                                                    int *blktype_id);

/*!
 * \brief Setup the FIFODMA operation for test instance.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int (*bcma_testutil_drv_fifodma_instance_setup_f)(int unit);

/*!
 * \brief Trigger the FIFODMA operation for test instance.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int (*bcma_testutil_drv_fifodma_instance_trigger_f)(int unit);

/*!
 * \brief Call check health utility for a device.
 *
 * \param [in] unit Unit number.
 * \param [out] test_result Test result.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int (*bcma_testutil_drv_check_health_f)(int unit, bool *test_result);

/*!
 * \brief Write a physical table entry.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] dyn_info Device-specific structure of dynamic address components.
 * \param [in] ovrr_info Optional device-sepcific override structure.
 * \param [in] data Data buffer to write to the table entry.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int (*bcma_testutil_drv_pt_write_f)(int unit,
                                            bcmdrd_sid_t sid,
                                            bcmbd_pt_dyn_info_t *dyn_info,
                                            void *ovrr_info,
                                            uint32_t *data);

/*!
 * \brief Read a physical table entry.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] dyn_info Device-specific structure of dynamic address components.
 * \param [in] ovrr_info Optional device-sepcific override structure.
 * \param [in] data Buffer where to return the table entry contents.
 * \param [in] wsize Size of buffer in units of 32-bit words.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
typedef int (*bcma_testutil_drv_pt_read_f)(int unit,
                                           bcmdrd_sid_t sid,
                                           bcmbd_pt_dyn_info_t *dyn_info,
                                           void *ovrr_info,
                                           uint32_t *data,
                                           size_t wsize);

/*!
 * \brief Get default value for given symbol ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 *
 *  \retval Pointer to default value for given symbol ID, or NULL if not found.
 */
typedef const uint32_t * (*bcma_testutil_drv_pt_default_value_get_f)(int unit,
                                                                     bcmdrd_sid_t sid);

/*!
 * \brief Test utility feature.
 *
 * This feature list defined some special handling
 */
typedef enum bcma_testutil_ft_e {

    /*! Need to setup the l2 multicast group for vlan flooding */
    BCMA_TESTUTIL_FT_NEED_L2_MC_FOR_VLAN_FLOOD = 0,

    /*! Need to setup the tag action for port-based vlan id */
    BCMA_TESTUTIL_FT_NEED_TAG_ACTION_FOR_PVID,

    /*! Need not algorithmic tdm coding */
    BCMA_TESTUTIL_FT_NEED_NOT_TDM_CODING_ALGO,

    /*! Need not test PT lookup because it is not supported */
    BCMA_TESTUTIL_FT_NOT_SUPPORT_PT_LOOKUP,

    /*! Must be the last. */
    BCMA_TESTUTIL_FT_COUNT

} bcma_testutil_ft_t;


/*! \brief Chip-specific driver.
 *
 * These function pointers will be attached to the specific implementation
 * when test componenent initialize.
 */
typedef struct bcma_testutil_drv_s {
    /*! Device type. */
    bcmdrd_dev_type_t dev_type;

    /*! Check whether the physical table should be skipped. */
    bcma_testutil_drv_pt_skip_f pt_skip;

    /*! Perform conditional initalization for given PT. */
    bcma_testutil_drv_cond_init_f cond_init;

    /*! Perform conditional cleanup for given PT. */
    bcma_testutil_drv_cond_cleanup_f cond_cleanup;

    /*! Override physical table mask if needs special handling. */
    bcma_testutil_drv_pt_mask_override_f pt_mask_override;

    /*! Override entity access in physical table iteration */
    bcma_testutil_drv_pt_entity_override_f pt_entity_override;

    /*! Enable/disable background hardware update for physical table test. */
    bcma_testutil_drv_pt_hw_update_f pt_hw_update;

    /*! Get related information for CAM built-in self test (CAMBIST). */
    bcma_testutil_drv_mem_cambist_info_f mem_cambist_info;

    /*! Adjust address range for memory test. */
    bcma_testutil_drv_mem_address_adjust_f mem_address_adjust;

    /*! Adjust entry data for hash memory test. */
    bcma_testutil_drv_mem_hash_data_adjust_f mem_hash_data_adjust;

    /*! Write memory using DMA mode. */
    bcma_testutil_drv_mem_range_write_f mem_range_write;

    /*! Read memory using DMA mode. */
    bcma_testutil_drv_mem_range_read_f mem_range_read;

    /*! Required configuration for Memory built-in self test (MBIST). */
    bcma_testutil_drv_mem_mbist_preconfig_f mem_mbist_preconfig;

    /*! Get related information for Memory built-in self test (MBIST). */
    bcma_testutil_drv_mem_mbist_info_f mem_mbist_info;

    /*! Get related information for Memory built-in self test (MBIST) with DMA. */
    bcma_testutil_drv_mem_mbist_dma_info_f mem_mbist_dma_info;

    /*! Get CMC channel information. */
    bcma_testutil_drv_cmc_chan_get_f cmc_chan_get;

    /*! Get default injecting packet number for traffic testing. */
    uint32_t traffic_default_pktcnt;

    /*! Get expeceted rate for traffic testing. */
    bcma_testutil_drv_traffic_get_expeceted_rate_f traffic_expeceted_rate_get;

    /*! Get fixed packet list for traffic testing. */
    bcma_testutil_drv_traffic_get_fixed_packet_list_f traffic_fixed_packet_list_get;

    /*! Get random packet list for traffic testing. */
    bcma_testutil_drv_traffic_get_random_packet_list_f traffic_random_packet_list_get;

    /*! Get worstcase packet length for traffic testing. */
    bcma_testutil_drv_traffic_get_worstcase_pktlen_f traffic_worstcase_pktlen_get;

    /*! Get testing register list for CPU benchmark */
    bcma_testutil_drv_benchmark_reglist_get_f benchmark_reglist_get;

    /*! Get testing memory list for CPU benchmark */
    bcma_testutil_drv_benchmark_memlist_get_f benchmark_memlist_get;

    /*! VLAN operation functions. */
    bcma_testutil_drv_vlan_op_get_f vlan_op_get;

    /*! L2 operation functions. */
    bcma_testutil_drv_l2_op_get_f l2_op_get;

    /*! CTR operation functions. */
    bcma_testutil_drv_ctr_op_get_f ctr_op_get;

    /*! Power test misc operation functions. */
    bcma_testutil_drv_power_op_t *power_op_get;

    /*! Flex test operation functions on logical table. */
    bcma_testutil_drv_flex_lt_op_get_f flex_lt_op_get;

    /*! Flex test misc operation functions. */
    bcma_testutil_drv_flex_traffic_op_get_f flex_traffic_op_get;

    /*! Get block type ID from its name. */
    bcma_testutil_drv_chip_blktype_get_f chip_blktype_get;

    /*! Get LTs' configurations and packet metadata needs to be sent. */
    bcma_testutil_pkt_cfg_data_get_f pkt_cfg_data_get;

    /*! Setup the FIFODMA operation for test instance. */
    bcma_testutil_drv_fifodma_instance_setup_f fifodma_instance_setup;

    /*! Trigger the FIFODMA operation for test instance. */
    bcma_testutil_drv_fifodma_instance_trigger_f fifodma_instance_trigger;

    /*! Call Check Health routine for device */
    bcma_testutil_drv_check_health_f check_health;

    /*! Write a physical table entry. */
    bcma_testutil_drv_pt_write_f pt_write;

    /*! Read a physical table entry. */
    bcma_testutil_drv_pt_read_f pt_read;

    /*! QOS test operation functions on logical table. */
    bcma_testutil_drv_qos_lt_op_get_f qos_lt_op_get;

    /*! Get default value for a given symbol. */
    bcma_testutil_drv_pt_default_value_get_f pt_default_value_get;

    /*! Feature list */
    SHR_BITDCLNAME(features, BCMA_TESTUTIL_FT_COUNT);
} bcma_testutil_drv_t;

/*!
 * \brief Get pointer to chip-specific driver.
 *
 * \param [in] unit Unit number.
 *
 * \return pointer to chip-specific driver, or NULL if not found.
 */
extern bcma_testutil_drv_t*
bcma_testutil_drv_get(int unit);

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bd##_testutil_drv_attach(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*!
 * \brief Initialize chip-specific driver.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_drv_attach(int unit);

/*!
 * \brief Check whether the physical table should be skipped.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] type Test Type.
 *
 * \retval TRUE The physical table should be skipped.
 * \retval FALSE The physical table should be tested.
 */
extern bool
bcma_testutil_drv_pt_skip(int unit, bcmdrd_sid_t sid,
                          bcma_testutil_pt_type_t type);

/*!
 * \brief Perform conditional initialization for given physical entity.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] type Test Type.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_UNIT Invalid unit
 * \retval SHR_E_INVALID Invalid SID.
 */
extern int
bcma_testutil_cond_init(int unit, bcmdrd_sid_t sid,
                        bcma_testutil_pt_type_t type);

/*!
 * \brief Perform conditional cleanup for given physical entity.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] type Test Type.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_UNIT Invalid unit
 * \retval SHR_E_INVALID Invalid SID.
 */
extern int
bcma_testutil_cond_cleanup(int unit, bcmdrd_sid_t sid,
                           bcma_testutil_pt_type_t type);

/*!
 * \brief Override physical table mask if needs special handling.
 *
 * \c mask buffer must be large enough to hold a full table entry of the given \c sid.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] index Primary index if the table has more than a single entry.
 * \param [inout] mask Mask of testable bits in table entry.
 * \param [in] wsize Number of 32-bit words in table entry.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_drv_pt_mask_override(int unit, bcmdrd_sid_t sid, int index,
                                   uint32_t *mask, int wsize);

/*!
 * \brief Get configurations of LTs, in order to configure chip to support sending packet traffic.
 * And get metadata of packet which needs to be sent.
 *
 * \param [in] unit Unit number.
 * \param [out] pkt_lt_cfg Configurations of LTs.
 * \param [out] lt_cfg_num Number of configuration of LTs.
 * \param [out] pkt_data Metadata of packet.
 *
 * \retval None.
 */
extern void
bcma_testutil_pkt_cfg_data_get(int unit,
                               const bcma_testutil_pkt_lt_cfg_t **pkt_lt_cfg,
                               int *lt_cfg_num,
                               const bcma_testutil_pkt_data_t **pkt_data);

/*!
 * \brief Enable/disable background hardware update for physical table test.
 *
 * Prevent the hardware overwrite the data written by software.
 *
 * \param [in] unit Unit number.
 * \param [in] name Physical table name.
 * \param [in] type Test Type.
 * \param [in] enable Set to false to disable HW background updates.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_drv_pt_hw_update(int unit,
                               const char *name,
                               bcma_testutil_pt_type_t type,
                               bool enable);

/*!
 * \brief Get related information for CAM built-in self test (CAMBIST)
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [out] is_bist indicate this register is for CAMBIST purpose or not
 * \param [out] info CAMBIST information of this register
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_drv_mem_cambist_info(
                int unit, bcmdrd_sid_t sid, bool *is_bist,
                bcma_testutil_mem_cambist_entry_t *info);


/*!
 * \brief Adjust address range for memory test.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [inout] index_start Start address.
 * \param [inout] index_end End address.
 * \param [in] type Test Type.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_drv_mem_address_adjust(int unit, bcmdrd_sid_t sid,
                                     int *index_start, int *index_end,
                                     bcma_testutil_pt_type_t type);

/*!
 * \brief Adjust entry data for hash memory test.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [inout] data Data of testable bits in table entry.
 * \param [in] wsize Number of 32-bit words in table entry.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_drv_mem_hash_data_adjust(int unit, bcmdrd_sid_t sid,
                                       uint32_t *data, int wsize);

/*!
 * \brief Write memory using DMA mode.
 *
 * Write data from DMA buffer into a range of table entries.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] index_start Index of the first table entry of the write operation.
 * \param [in] index_end Index of the last table entry of the write operation.
 * \param [in] tbl_inst Table instance.
 * \param [in] buf DMA buffer for table entries to be written.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_drv_mem_range_write(int unit, bcmdrd_sid_t sid, int index_start,
                                  int index_end, int tbl_inst, uint64_t buf);

/*!
 * \brief Read memory using DMA mode.
 *
 * Read data from a range of table entries into a DMA buffer.
 * The buffer size is index count * byte size of table entry.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] index_start Index of the first table entry of the read operation.
 * \param [in] index_end Index of the last table entry of the read operation.
 * \param [in] tbl_inst Table instance.
 * \param [out] buf DMA buffer large enough to hold the specified range of table entries.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_drv_mem_range_read(int unit, bcmdrd_sid_t sid, int index_start,
                                 int index_end, int tbl_inst, uint64_t buf);

/*!
 * \brief Required configuration for Memory built-in self test (MBIST).
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_drv_mem_mbist_preconfig(int unit);

/*!
 * \brief Get related information for Memory built-in self test (MBIST)
 *
 * \param [in] unit Unit number.
 * \param [out] test_array Array of MBIST information
 * \param [out] test_count Number of MBIST information
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_drv_mem_mbist_info(
                int unit,
                const bcma_testutil_mem_mbist_entry_t **test_array,
                int *test_count);

/*! \cond  Externs for the required chip specific functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bd##_mem_mbist_info(int unit, const bcma_testutil_mem_mbist_entry_t **array, int *array_count);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*!
 * \brief Get related information for Memory built-in self test (MBIST) with DMA
 *
 * \param [in] unit Unit number.
 * \param [out] test_array Array of MBIST information
 * \param [out] test_count Number of MBIST information
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_drv_mem_mbist_dma_info(
                int unit,
                const bcma_testutil_mem_mbist_dma_entry_t **test_array,
                int *test_count);

/*! \cond  Externs for the required chip specific functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bd##_mem_mbist_dma_info(int unit, const bcma_testutil_mem_mbist_dma_entry_t **array, int *array_count);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */


/*!
 * \brief Get CMC channel information.
 *
 * \param [in] unit Unit number.
 * \param [out] cmc_num CMC number.
 * \param [out] cmc_chan_num Channel number per CMC.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
int bcma_testutil_drv_cmc_chan_get(int unit, int *cmc_num, int *cmc_chan_num);

/*!
 * \brief Get default injecting packet number for traffic testing
 *
 * \param [in] unit Unit number.
 * \param [out] ret_pktcnt The default injecting packet number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_drv_traffic_default_pktcnt_get(int unit, uint32_t *ret_pktcnt);

/*!
 * \brief Get exepected traffic rate per port
 *
 * \param [in] unit Unit number.
 * \param [in] pbmp Bitmap for all ports
 * \param [in] pbmp_oversub Bitmap for oversubscribed ports
 * \param [in] pkt_size Packet size.
 * \param [in] array_size Array size of ret_exp_rate
 * \param [out] ret_exp_rate Returned expected rate for each port
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_drv_traffic_expeceted_rate_get(
    int unit, bcmdrd_pbmp_t pbmp, bcmdrd_pbmp_t pbmp_oversub,
    int pkt_size, int array_size, uint64_t *ret_exp_rate);

/*!
 * \brief Get list with fixed packet length for traffic testing
 *
 * \param [in] unit Unit number.
 * \param [in] pkt_len Packet length
 * \param [out] ret_pkt_cnt Returned packet count
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_drv_traffic_fixed_packet_list_get(
    int unit, uint32_t pkt_len, uint32_t *ret_pkt_cnt);

/*!
 * \brief Get list with random packet length for traffic testing
 *
 * \param [in] unit Unit number.
 * \param [in] cellnums_limit Upper limit of cell numbers per packet
 * \param [in] array_size Array size of ret_pkt_len
 * \param [out] ret_pkt_len Returned array of packet length
 * \param [out] ret_pkt_cnt Returned packet count
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_drv_traffic_random_packet_list_get(
    int unit, uint32_t cellnums_limit,
    uint32_t array_size, uint32_t *ret_pkt_len, uint32_t *ret_pkt_cnt);

/*!
 * \brief Get packet length in the worst case for traffic testing
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] ret_pkt_len Returned array of packet length
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_drv_traffic_worstcase_pktlen_get(
    int unit, int port, uint32_t *ret_pkt_len);

/*!
 * \brief Get block type ID from its name.
 *
 * \param [in] unit Unit number.
 * \param [in] blktype_name Block type name.
 * \param [out] blktype_id Block type ID which match to the name.
 *
 * \return -1 on failure, block type ID otherwise.
 */
extern int
bcma_testutil_drv_chip_blktype_get(int unit, const char *blktype_name,
                                   int *blktype_id);

/*!
 * \brief Get testing register list for CPU benchmark
 *
 * \param [in] unit Unit number.
 * \param [out] reg_arr Testing register list.
 * \param [out] reg_cnt Number of testing register.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_drv_benchmark_reglist_get(int unit, const bcmdrd_sid_t **reg_arr,
                                        int *reg_cnt);


 /*!
 * \brief Get testing memory list for CPU benchmark
 *
 * \param [in] unit Unit number.
 * \param [out] mem_arr Testing memory list.
 * \param [out] mem_cnt Number of testing memory.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_drv_benchmark_memlist_get(int unit, const bcmdrd_sid_t **mem_arr,
                                        int *mem_cnt);

/*!
 * \brief Setup the FIFODMA operation for test instance.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_drv_fifodma_instance_setup(int unit);

/*!
 * \brief Trigger the FIFODMA operation for test instance.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_drv_fifodma_instance_trigger(int unit);

/*!
 * \brief Check the specfic feature enabled or not
 *
 * \param [in] unit Unit number.
 * \param [in] feature Feature.
 * \param [out] enabled Returned check result
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_drv_feature_get(int unit, bcma_testutil_ft_t feature,
                              bool *enabled);

/*!
 * \brief Check health of device at test end.
 *
 * \param [in] unit Unit number.
 * \param [out] test_result Check Result.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_drv_check_health(int unit, bool *test_result);

/*!
 * \brief Write a physical table entry.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] dyn_info Device-specific structure of dynamic address components.
 * \param [in] ovrr_info Optional device-sepcific override structure.
 * \param [in] data Data buffer to write to the table entry.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_drv_pt_write(int unit,
                           bcmdrd_sid_t sid,
                           bcmbd_pt_dyn_info_t *dyn_info,
                           void *ovrr_info,
                           uint32_t *data);

/*!
 * \brief Read a physical table entry.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] dyn_info Device-specific structure of dynamic address components.
 * \param [in] ovrr_info Optional device-sepcific override structure.
 * \param [in] data Buffer where to return the table entry contents.
 * \param [in] wsize Size of buffer in units of 32-bit words.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int
bcma_testutil_drv_pt_read(int unit,
                          bcmdrd_sid_t sid,
                          bcmbd_pt_dyn_info_t *dyn_info,
                          void *ovrr_info,
                          uint32_t *data,
                          size_t wsize);

/*!
 * \brief Get default value for given symbol ID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 *
 *  \retval Pointer to default value for given symbol ID, or NULL if not found.
 */
extern const uint32_t *
bcma_testutil_drv_pt_default_value_get(int unit, bcmdrd_sid_t sid);

#endif /* BCMA_TESTUTIL_DRV_H */

