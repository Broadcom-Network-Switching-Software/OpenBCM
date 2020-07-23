/*! \file bcmltm_ha_internal.h
 *
 * Logical Table Manager High Availability Internal Definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_HA_INTERNAL_H
#define BCMLTM_HA_INTERNAL_H

#include <shr/shr_types.h>
#include <shr/shr_bitop.h>
#include <shr/shr_ha.h>
#include <shr/shr_pb_local.h>
#include <bcmltm/generated/bcmltm_ha.h>


/*!
 * \brief HA block types.
 *
 * The High Availability persistent memory block categories.
 */
typedef enum bcmltm_ha_block_type_e {
    /*! Invalid block number to simplify bad HA pointer detection. */
    BCMLTM_HA_BLOCK_TYPE_INVALID = 0,

    /*! Same-size bcmltm_lt_state_t structures for all LTs. */
    BCMLTM_HA_BLOCK_TYPE_STATE = 1,

    /*! Various-sized bcmltm_lt_state_data_t structures for all LTs. */
    BCMLTM_HA_BLOCK_TYPE_STATE_DATA = 2,

    /*! LTM Transaction Status for current modeled transaction. */
    BCMLTM_HA_BLOCK_TYPE_TRANS_STATUS = 3,

    /*! Rollback copy of base state for LTs in the current transaction. */
    BCMLTM_HA_BLOCK_TYPE_ROLLBACK_STATE = 4,

    /*! Rollback copy of state data for LTs in the current transaction.  */
    BCMLTM_HA_BLOCK_TYPE_ROLLBACK_STATE_DATA = 5,

    /*! Total HA block type count. */
    BCMLTM_HA_BLOCK_TYPE_COUNT = 6
} bcmltm_ha_block_type_t;

/*!
 * HA offset + subblock encoded pointer.
 *
 * Encoding is:
 * ha_ptr[31:24] = subblock type
 * ha_ptr[23:0] = block offset in bytes
 */
typedef shr_ha_ptr_t bcmltm_ha_ptr_t;

/*! Extract HA subblock from encoded pointer. */
#define BCMLTM_HA_PTR_TO_BLOCK_TYPE(_ha_ptr) \
    SHR_HA_PTR_SUB((_ha_ptr))

/*! Extract HA block offset from encoded pointer. */
#define BCMLTM_HA_PTR_OFFSET(_ha_ptr) \
    SHR_HA_PTR_OFFSET((_ha_ptr))

/*! Encode HA block offset and subblock into HA pointer. */
#define BCMLTM_HA_PTR(_bt, _offset)                              \
    SHR_HA_PTR_CONSTRUCT((_bt), (_offset))

/*! An invalid LTM HA pointer value. */
#define BCMLTM_HA_PTR_INVALID 0

/*! Test for invalid LTM HA pointer. */
#define BCMLTM_HA_PTR_IS_INVALID(_ha_ptr) \
    ((_ha_ptr) == BCMLTM_HA_PTR_INVALID)

/*! Set LTM HA pointer to the invalid value. */
#define BCMLTM_HA_PTR_INVALID_SET(_ha_ptr) \
    (_ha_ptr) = BCMLTM_HA_PTR_INVALID

/*! LTM HA block signature base. */
#define BCMLTM_BLOCK_SIGNATURE_BASE   0xd1d54200

/*! HA subblock encoding mask. */
#define BCMLTM_HA_SIGNATURE_BLOCK_TYPE_MASK     0xff

/*! LTM HA block signature for a block type. */
#define BCMLTM_BLOCK_SIGNATURE(_bt)   \
    (((uint32_t)((_bt) & BCMLTM_HA_SIGNATURE_BLOCK_TYPE_MASK)) | \
     BCMLTM_BLOCK_SIGNATURE_BASE)

/*! LTM HA block signature for a block type. */
#define BCMLTM_BLOCK_SIGNATURE_TYPE(_sig)   \
    ((bcmltm_ha_block_type_t)((_sig) & BCMLTM_HA_SIGNATURE_BLOCK_TYPE_MASK))


/*!
 * \brief HA LTM state size values.
 */
typedef struct bcmltm_ha_state_sizes_s {
    /*!
     * Total size of all LTs bcmltm_lt_state_t structures.
     * This should be the size of the STATE HA block.
     */
    uint32_t total_state_size;

    /*!
     * Total size of all LTs state data.
     * This should be the size of the STATE_DATA HA block.
     */
    uint32_t total_state_data_size;

    /*!
     * Maximum size of a single LT bcmltm_lt_state_t structure.
     * This is included in calculating the size of the
     * ROLLBACK_STATE HA block.
     */
    uint32_t maximum_state_size;

    /*!
     * Maximum size of a single LT's combined state data.
     * This is included in calculating the size of the
     * ROLLBACK_STATE_DATA HA block.
     */
    uint32_t maximum_state_data_size;
} bcmltm_ha_state_sizes_t;

/*!
 * \brief LTM HA block information.
 */
typedef struct bcmltm_ha_block_info_s {
    /*!
     * LTM bcmltm_ha_block_type_t identifying the purpose of this block.
     */
    bcmltm_ha_block_type_t block_type;

    /*!
     * Pointer to the HA memory space of this block.
     */
    bcmltm_ha_block_header_t *ha_block_ptr;
} bcmltm_ha_block_info_t;


/*!
 * \brief Indicate ISSU recovery to BCMLTM module.
 *
 * Signal the LTM module that ISSU recovery is configured for this
 * initialization sequence.
 *
 * \param [in] unit Logical device id.
 *
 * \retval None.
 */
extern void
bcmltm_issu_enable(int unit);

/*!
 * \brief Retrieve the maximum number of LTs per transaction.
 *
 * The number of Logical Table state records maintained for rollback
 * during an Atomic Transaction may be configured during initialization.
 * This function returns that number to the LTM elements which
 * depend upon this value.
 *
 * \param [in] unit Logical device id.
 *
 * \retval Number of LTs permitted per transaction.
 */
extern uint8_t
bcmltm_ha_transaction_max(int unit);

/*!
 * \brief Initialize LTM High Availability blocks.
 *
 * When the LTM is initialized, the HA block must be created with the
 * necessary sizes to handle the information to be stored in each block.
 * First, sufficient examination of the LTs configured in this run
 * must be completed to determine the size of each block.
 * Next, the HA regions are either initialized or recovered.
 *
 * \param [in] unit Logical device id.
 * \param [in] warm Indicates if this is cold or warm boot.
 * \param [in] ha_state_sizes Pointer to HA state sizes record.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ha_init(int unit,
               bool warm,
               bcmltm_ha_state_sizes_t *ha_state_sizes);

/*!
 * \brief Cleanup LTM High Availability blocks.
 *
 * Erase and release the LTM High Availability blocks.
 *
 * \param [in] unit Logical device id
 *
 * \retval None
 */
extern void
bcmltm_ha_cleanup(int unit);

/*!
 * \brief Allocate memory from High Availability blocks.
 *
 * The LTM's High Availability blocks are used for storing LT state,
 * transaction status, and LT rollback state.  During metadata
 * initialization, the core LT state structures are allocated from
 * some of these blocks.
 *
 * This function verifies that sufficient memory to provide the
 * requested byte size remains in the HA block, reserves that amount
 * after word-alignment, and updates the record of space allocated.
 *
 * \param [in] unit Logical device id.
 * \param [in] block_type The HA block type from which to allocate memory.
 * \param [in] size Size in bytes of desired memory segment.
 *
 * \retval Encoded HA ptr to HA memory location of specified size.
 * \retval BCMLTM_HA_PTR_INVALID on failure due to insufficient memory.
 */
extern bcmltm_ha_ptr_t
bcmltm_ha_alloc(int unit,
                bcmltm_ha_block_type_t block_type,
                uint32_t size);

/*!
 * \brief Recover memory from High Availability blocks.
 *
 * The LTM's High Availability blocks are used for storing LT state,
 * transaction status, and LT rollback state.  During metadata
 * initialization, the core LT state structures are allocated from
 * some of these blocks.
 *
 * This function recovers the HA memory pointer of the specified block
 * during any Warm Boot/ISSU/HA sequence.
 *
 * \param [in] unit Logical device id.
 * \param [in] block_type The HA block type from which to allocate memory.
 *
 * \retval Encoded HA ptr to HA memory location of specified size.
 * \retval BCMLTM_HA_PTR_INVALID on failure due to insufficient memory.
 */
extern bcmltm_ha_ptr_t
bcmltm_ha_blk_get(int unit,
                  bcmltm_ha_block_type_t block_type);

/*!
 * \brief Retrieve memory for High Availability blocks.
 *
 * This function searched for an existing high availabiliy memory block
 * matching the requested block type.  If found, the HA encoded pointer
 * is returned.  Otherwise, BCMLTM_HA_PTR_INVALID is returned.
 *
 * \param [in] unit Logical device id.
 * \param [in] block_type The HA block type for which to retrieve memory.
 *
 * \retval Encoded HA ptr to HA memory location of specified block_type.
 * \retval BCMLTM_HA_PTR_INVALID when matching HA pointer not found.
 */
extern bcmltm_ha_ptr_t
bcmltm_ha_blk_get(int unit,
                  bcmltm_ha_block_type_t block_type);

/*!
 * \brief Clear High Availability records.
 *
 * This function erases all of the information within the Logical
 * Table Manager High Availability blocks.
 *
 * \param [in] unit Logical device id.
 *
 * \retval None
 */
extern void
bcmltm_ha_reset(int unit);

/*!
 * \brief Convert HA encoded pointer to physical memory pointer.
 *
 * This function converts the HA encoded (subblock, offset) format
 * of a LTM HA pointer into the physical memory required for further
 * processing.  If the HA pointer is invalid, it returns NULL.
 *
 * \param [in] unit Logical device id.
 * \param [in] ha_ptr LTM HA encoded pointer.
 *
 * \retval Physical memory pointer if HA pointer is valid.
 *         NULL if HA pointer is invalid.
 */
extern void *
bcmltm_ha_ptr_to_mem_ptr(int unit,
                         bcmltm_ha_ptr_t ha_ptr);

/*!
 * \brief Increment HA encoded pointer by given amount
 *
 * This function adds the provided byte increment value,
 * with word-alignment, to the byte offset element of the
 * HA encoded (subblock, offset) format pointer.
 * If insufficient space remains for this block, then the returned
 * HA pointer value is BCMLTM_HA_PTR_INVALID.
 *
 * \param [in] unit Logical device id.
 * \param [in] ha_ptr LTM HA encoded pointer.
 * \param [in] increment Byte count to increase LTM HA encoded pointer.
 *
 * \retval New HA pointer.
 */
extern bcmltm_ha_ptr_t
bcmltm_ha_ptr_increment(int unit,
                        bcmltm_ha_ptr_t ha_ptr,
                        uint32_t increment);

/*!
 * \brief Modify LTM HA state during ISSU.
 *
 * This function analyzes the LTM HA state records from a previous
 * SDKLT version and modifies them during the ISSU step of a warm boot
 * recovery.  The particular actions required are:
 *  1) convert LTID and field ID values from the prior version
 *     to the new enumerations.
 *  2) Eliminate LT records for LTs which are removed from the LRD.
 *  3) Create record space for new LTs in the LRD.  These records are
 *     constructed during metadata init as they would be during cold boot.
 *
 * \param [in] unit Logical device id.
 * \param [in] ha_state_sizes Pointer to HA state sizes record.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_unit_state_upgrade(int unit,
                          bcmltm_ha_state_sizes_t *ha_state_sizes);

#endif /* BCMLTM_HA_INTERNAL_H */
