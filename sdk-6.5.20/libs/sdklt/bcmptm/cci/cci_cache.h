/*! \file cci_cache.h
 *
 * Interface functions for Counter Cache
 *
 * This file contains implementation of counter cache (SW) for CCI
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef CCI_CACHE_H
#define CCI_CACHE_H

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmptm/bcmptm_cci_internal.h>
#include <bcmptm/bcmptm_cci.h>
#include "cci_internal.h"

/*******************************************************************************
 * Defines
 */

/*!
 * \brief Information of counter cache object
 */
typedef struct bcmptm_cci_cache_ctr_info_s {
    bcmdrd_sid_t sid;       /*!< sid symbol ID */
    uint32_t     offset;       /*!< Fixed part of symbol address */
    int          blktype;      /*! Symbol blktype */
    uint32_t     pipe_map;     /*! Symbol pipe map */
    int          tbl_inst;     /*!< Number of table instances */
    int          ctrtype;      /*!<Counter Type */
    bool         is_mem;       /*!< Sym is memory */
    bool         enable;       /*!< enable to update */
    uint32_t     index_min;    /*! Minimum valid index */
    uint32_t     index_max;    /*! Maximum valid index */
    int          field_num;            /*!< field_num, of filed ID's */
    uint32_t     pt_entry_wsize;       /*!< physical entry size */
} bcmptm_cci_cache_ctr_info_t;


/*!
 * \brief Counter map id number
 * value (-1) indicated all counters for given typeid
 */
typedef uint32_t bcmptm_cci_ctr_map_id;

/*!
 * \brief Description  Parameter structure for Non DMA access
 */

typedef struct bcmptm_cci_cache_pt_param_s {
    bcmptm_cci_ctr_info_t *info; /*!< info, Counter SID Information */
    uint32_t *buf;               /*!< buf, Data buffer, PT format */
    size_t size;                 /*!<size, buffer size */
    uint32_t *read_cache;       /* !< read_cache, Read data, PT format */
} bcmptm_cci_cache_pt_param_t;

/*!
 * \brief Description  Paramter structure for Non DMA access
 */

typedef int (*bcmptm_cci_cache_hw_op_f)(int unit,
                                        cci_handle_t handle,
                                        void *param,
                                        bool cache_update);

/*!
 * \brief Initialize counter cache
 * \n Must be called every time a new unit is attached
 *
 * \param [in] unit Logical device id
 * \param [in] warm  warm boot
 * \param [in] cci_con  context of cci
 * \param [out] handle of the current cache Instance
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_cache_init(int unit,
                      bool warm,
                      const cci_context_t  *cci_con,
                      cci_handle_t *handle);


/*!
 * \brief De-Initialize counter cache
 *
 * \param [in] unit Logical device id
 * \param [in] handle of the current cache Instance
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_cache_cleanup(int unit,
                         cci_handle_t handle);

/*!
 * \brief   Add the counter cache object
 *
 * \param [in] unit Logical device id
 * \param [in] handle of the current cache Instance
 * \param [in] sid symbol ID.
  * \param [in] map_id counter map ID.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_cache_ctr_add(int unit,
                         cci_handle_t handle,
                         bcmdrd_sid_t sid,
                         uint32_t *map_id,
                         bool warm);


/*!
 * \brief   Read single or multiple counter values
 * \n Given a counter blk_instance and port number(tbl_instance)
 * \n read 64-bit software-accumulated counter value
 *
 * \param [in] unit Logical device id
 * \param [in] handle of the current cache Instance
 * \param [in] info Counter memory/register  information
 * \param [out] buf Buffer to read counter values, LT format
 * \param [in] size of the buffer in words
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_cache_read(int unit,
                      cci_handle_t handle,
                      bcmptm_cci_ctr_info_t *info,
                      uint32_t *buf,
                      size_t size);

/*!
 * \brief  Write single or multiple counter values
 * \n Given a counter blk_instance and port number(tbl_instance),
 * \n write 64-bit counter value to software-accumulated and hardware
 *
 * \param [in] unit Logical device id
 * \param [in] handle of the current cache Instance
 * \param [in] info Counter information
 * \param [in] buf Buffer to write counter values, LT Format
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_cache_write(int unit,
                       cci_handle_t handle,
                       bcmptm_cci_ctr_info_t *info,
                       uint32_t *buf);

/*!
 * \brief  Update single or multiple counter values from HW to Cache
 * \n Given a counter blk_instance and port number(tbl_instance),
 * \n write 64-bit counter value to software-accumulated and hardware
 *
 * \param [in] unit Logical device id
 * \param [in] handle of the current cache Instance
 * \param [in] info Counter information
 * \param [in] buf Buffer to write counter values, PT Format
 * \param [in] size of the buffer in words
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_cache_update(int unit,
                        cci_handle_t handle,
                        bcmptm_cci_ctr_info_t *info,
                        uint32_t *buf,
                        size_t size);


/*!
 * \brief  sync the counter values with Hardware
 * \n Given a counter blk_instance and port number(tbl_instance),
 * \n write 64-bit counter value to software-accumulated and hardware
 *
 * \param [in] unit Logical device id
 * \param [in] handle of the current cache Instance
 * \param [in] hw_op function pointer for hardware opearation
 * \param [in] param parameter for hardware opeartion function
 * \param [in] update_cache If cache need to be updated
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_cache_hw_sync(int unit,
                         cci_handle_t handle,
                         bcmptm_cci_cache_hw_op_f hw_op,
                         void *param,
                         bool update_cache);

/*!
 * \brief  Write to physical Table, Reset Cache request by SER event (SCHAN PIO)
 *
 * \param [in] unit Logical device id
 * \param [in] handle of the current cache Instance
 * \param [in] param, pointer to structure cci_pt_param_t
 * \param [in] update_cache, If cache need to be updated
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_cache_pt_req_ser_clear(int unit,
                                  cci_handle_t handle,
                                  void *param,
                                  bool update_cache);

/*!
 * \brief Read, Modify Write to physical Table, Reset Cache (SCHAN PIO)
 *
 * \param [in] unit Logical device id
 * \param [in] handle of the current cache Instance
 * \param [in] param, pointer to structure cci_pt_param_t
 * \param [in] update_cache, If cache need to be updated
 *
 * \retval SHR_E_NONE Success
 */

extern int
bcmptm_cci_cache_pt_req_write(int unit,
                              cci_handle_t handle,
                              void *param,
                              bool update_cache);

/*!
 * \brief Clear of the counters in cache and HW for given Symbol
 *
 * \param [in] unit Logical device id.
 * \param [in] handle of the current cache Instance.
 * \param [in] param, pointer to structure cci_pt_param_t.
 * \param [in] update_cache, If cache need to be updated.
 *
 * \retval SHR_E_NONE Success
 */

extern int
bcmptm_cci_cache_pt_req_all_clear(int unit,
                                  cci_handle_t handle,
                                  void *param,
                                  bool update_cache);

/*!
 * \brief Write to physical Table, Reset Cache (SCHAN PIO)
 *
 * \param [in] unit Logical device id
 * \param [in] handle of the current cache Instance
 * \param [in] param, pointer to structure cci_pt_param_t
 * \param [in] cache, If cache need to be updated
 *
 * \retval SHR_E_NONE Success
 */

extern int
bcmptm_cci_cache_pt_req_passthru_write(int unit,
                                       cci_handle_t handle,
                                       void *param,
                                       bool update_cache);


/*!
 * \brief Read from physical Table and update Cache (SCHAN PIO)
 *
 * \param [in] unit Logical device id
 * \param [in] handle of the current cache Instance
 * \param [in] param, pointer to structure cci_pt_param_t
 * \param [in] cache, If cache need to be updated
 *
 * \retval SHR_E_NONE Success
 */

extern int
bcmptm_cci_cache_pt_req_read(int unit,
                             cci_handle_t handle,
                             void *param,
                             bool update_cache);


/*!
 * \brief   Get maxiimum size of map table
 *
 * \param [in] unit Logical device id
 * \param [in] handle of the current cache Instance
 *
 * \retval counter map id on Success
 */
extern int
bcmptm_cci_cache_ctr_map_size(int unit,
                              cci_handle_t handle,
                              bcmptm_cci_ctr_map_id *max);


/*!
 * \brief   Get information the counter object
 *
 * \param [in] unit Logical device id
 * \param [in] handle of the current cache Instance
 * \param [out] mapid  Counter Map id
 * \param [out] cinfo Counter object information.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_cache_ctr_info_get(int unit,
                              cci_handle_t handle,
                              bcmptm_cci_ctr_map_id mapid,
                              bcmptm_cci_cache_ctr_info_t *cinfo);

/*!
 * \brief Configure Counter Cache
 * \n Called during System Manager CONFIG state for each unit
 *
 * \param [in] unit Logical device id
 * \param [in] handle of the current cache Instance
 * \param [in] warm  warm boot
 * \param [in] phase configuration phase (1,2,3)
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_cache_ctr_config(int unit,
                            cci_handle_t handle,
                            bool warm,
                            bcmptm_sub_phase_t phase);

/*!
 * \brief Get symbol ID from counter map id
 *
 * \param [in] unit Logical device id
 * \param [in] handle of the current cache Instance
 * \param [in] counter map id
 * \param [out] sid Symbol ID
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_cache_mapid_to_sid(int unit,
                              cci_handle_t handle,
                              uint32_t mapid,
                              bcmdrd_sid_t *sid);

/*!
 * \brief Check if index is valid
 *
 * \param [in] unit Logical device id
 * \param [in] handle of the current cache Instance
 * \param [in] counter map id
 * \param [in] index
 * \param [in] table inst
 * \param [out] sid Symbol ID
 *
 * \retval SHR_E_NONE Success
 */
extern bool
bcmptm_cci_cache_mapid_index_valid(int unit,
                              cci_handle_t handle,
                              uint32_t mapid,
                              uint32_t index,
                              int tbl_inst);

/*!
 * \brief Transform logical port to physical port
 *
 * \param [in] unit Logical device id
 * \param [in] handle of the current cache Instance
 * \param [in, out] Counter information
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_cache_lport_to_pport(int unit,
                                cci_handle_t handle,
                                bcmptm_cci_ctr_info_t *info);

/*!
 * \brief Set eviction control mode.
 *
 * \param [in] unit Unit number
 * \param [in] handle Current cache Instance
 * \param [in] mapid Counter mapid
 * \param [in] mode Eviction mode
 *
 * \retval SHR_E_NONE No errors
 */
extern int
bcmptm_cci_cache_evict_control_set(int unit,
                                   cci_handle_t handle,
                                   uint32_t mapid,
                                   bcmptm_cci_ctr_evict_mode_t mode);

/*!
 * \brief Configure, update clear_on_read attribute on specified sid.
 * \n And force reading and updating cache when clear_on_read is TRUE.
 *
 * \param [in] unit Unit number
 * \param [in] handle Current cache Instance
 * \param [in] sid Symbol ID
 * \param [in] clear_on_read Eviction mode
 *
 * \retval SHR_E_NONE No errors
 */
extern int
bcmptm_cci_cache_pt_req_clear_on_read(int unit,
                                      cci_handle_t handle,
                                      void *sid,
                                      bool clear_on_read);
/*!
 * \brief Set counter mode.
 * \n Called by Flex counter module
 *
 * \param [in] unit Logical device id.
 * \param [in] sid Enum to specify reg, mem.
 * \param [in] index_min start index.
 * \param [in] index_max end index.
 * \param [in] ctr_mode-counter mode.
 * \param [in] update_mode-counter update mode buffer pointer.
 * \param [in] size-number of update mode elements.
 *
 * \retval SHR_E_NONE Success
 */

extern int
bcmptm_cci_cache_counter_mode_set(int unit,
                                 cci_handle_t handle,
                                 uint32_t mapid,
                                 uint32_t index_min,
                                 uint32_t index_max,
                                 int tbl_inst,
                                 bcmptm_cci_ctr_mode_t ctr_mode,
                                 bcmptm_cci_update_mode_t *update_mode,
                                 size_t size);

/*!
 * \brief Get counter mode.
 * \n Called by Flex counter module
 *
 * \param [in] unit Logical device id.
 * \param [in] sid Enum to specify reg, mem.
 * \param [in] index index of table.
 * \param [out] ctr_mode-counter mode.
 * \param [out] update_mode-counter update mode buffer pointer.
 * \param [in, out] size-number of update mode elements.
 *
 * \retval SHR_E_NONE Success
 */

extern int
bcmptm_cci_cache_counter_mode_get(int unit,
                                 cci_handle_t handle,
                                 uint32_t mapid,
                                 uint32_t index,
                                 int tbl_inst,
                                 bcmptm_cci_ctr_mode_t *ctr_mode,
                                 bcmptm_cci_update_mode_t *update_mode,
                                 size_t *size);


/*!
 * \brief   Read single slim counter value
 *
 * \param [in] unit Logical device id.
 * \param [in] handle of the current cache Instance.
 * \param [in] info Counter memory/register  information.
 * \param [out] buf Buffer to read counter values, LT format.
 * \param [in] size of the buffer in words.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_slim_cache_read(int unit,
                           cci_handle_t handle,
                           bcmptm_cci_ctr_info_t *info,
                           uint32_t *buf,
                           size_t size);

/*!
 * \brief   Write single slim counter value to cache
 *
 * \param [in] unit Logical device id.
 * \param [in] handle of the current cache Instance.
 * \param [in] info Counter information.
 * \param [in] buf Buffer to write counter values, LT Format.
 *
 * info->txfm_fld will have the field number.
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_slim_cache_write(int unit,
                       cci_handle_t handle,
                       bcmptm_cci_ctr_info_t *info,
                       uint32_t *buf);

/*!
 * \brief   write single slim counter to cache and HW.
 *
 * \param [in] unit Logical device id.
 * \param [in] handle of the current cache Instance.
 * \param [in] param Counter memory/register  information.
 * \param [in] buf Buffer to read counter values, LT format.
 * \param [in] update_cache TRUE, if cache need to be updated.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_slim_cache_pt_req_write(int unit,
                                   cci_handle_t handle,
                                   void *param,
                                   bool update_cache);
/*!
 * \brief   Read single normal double counter value
 *
 * \param [in] unit Logical device id.
 * \param [in] handle of the current cache Instance.
 * \param [in] info Counter memory/register  information.
 * \param [out] buf Buffer to read counter values, LT format.
 * \param [in] size of the buffer in words.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_normal_double_cache_read(int unit,
                           cci_handle_t handle,
                           bcmptm_cci_ctr_info_t *info,
                           uint32_t *buf,
                           size_t size);

/*!
 * \brief   Write normal double counter value to cache
 *
 * \param [in] unit Logical device id.
 * \param [in] handle of the current cache Instance.
 * \param [in] info Counter information.
 * \param [in] buf Buffer to write counter values, LT Format.
 *
 * info->txfm_fld will have the field number.
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_normal_double_cache_write(int unit,
                       cci_handle_t handle,
                       bcmptm_cci_ctr_info_t *info,
                       uint32_t *buf);

/*!
 * \brief   write normal doubble counter to cache and HW.
 *
 * \param [in] unit Logical device id.
 * \param [in] handle of the current cache Instance.
 * \param [in] param Counter memory/register  information.
 * \param [in] buf Buffer to read counter values, LT format.
 * \param [in] update_cache TRUE, if cache need to be updated.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_cci_normal_double_cache_pt_req_write(int unit,
                                   cci_handle_t handle,
                                   void *param,
                                   bool update_cache);

/*!
 * \brief Set state of the flex counter pool cache.
 *
 * \param [in] unit Logical device id
 * \param [in] handle of the current cache Instance.
 * \param [in] sid Enum to specify reg, mem
 * \param [in] state disable, enable, shadow
 *
 * \retval SHR_E_NONE Success
 */

extern int
bcmptm_cci_cache_flex_counter_pool_set_state(int unit,
                                             cci_handle_t handle,
                                             uint32_t map_id,
                                             bcmptm_cci_pool_state_t state);

/*!
 * \brief Get state of the flex counter pool cache.
 *
 * \param [in] unit Logical device id
 * \param [in] handle of the current cache Instance.
 * \param [in] sid Enum to specify reg, mem
 * \param [out] state disable, enable, shadow
 *
 * \retval SHR_E_NONE Success
 */

extern int
bcmptm_cci_cache_flex_counter_pool_get_state(int unit,
                                             cci_handle_t handle,
                                             uint32_t map_id,
                                             bcmptm_cci_pool_state_t *state);

/*!
 * \brief Get clear on read sbus property.
 *
 * \param [in] unit Logical device id.
 * \param [in] handle of the current cache Instance.
 * \param [in] map id in counter cache.
 * \param [out] clear of read.
 *
 * \retval SHR_E_NONE Success
 */

extern int
bcmptm_cci_cache_clearon_read_get(int unit,
                                  cci_handle_t handle,
                                  uint32_t map_id,
                                  bool *cor);

/*!
 * \brief Get counter type.
 *
 * \param [in] unit Logical device id.
 * \param [in] handle of the current cache Instance.
 * \param [in] map id in counter cache.
 * \param [out] counter type.
 *
 * \retval SHR_E_NONE Success
 */

extern int
bcmptm_cci_cache_counter_type_get(int unit,
                                  cci_handle_t handle,
                                  uint32_t map_id,
                                  cci_ctr_type_id *type);

/*!
 * \brief Write single or multiple counter values
 * to counter cache. Cache will be locked during update.
 *
 * \param [in] unit Logical device id
 * \param [in] handle of the current cache Instance
 * \param [in] param, pointer to structure cci_pt_param_t
 * \param [in] slim, If counter mode is slim.
 *
 * \retval SHR_E_NONE Success
 */

extern int
bcmptm_cci_cache_lock_write(int unit,
                            cci_handle_t handle,
                            void *param,
                            bool slim);
#endif /* CCI_CACHE_H */
