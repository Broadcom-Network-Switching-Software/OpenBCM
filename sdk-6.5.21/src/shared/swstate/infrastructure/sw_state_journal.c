/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * INFO: this module implement a Roll-Back Journal for the SW State infrastructure layer,
 *       the roll-back is needed in order to implement Crash Recovery (and generic Error Recovery)
 */

#include <soc/types.h>
#include <soc/error.h>

#ifdef CRASH_RECOVERY_SUPPORT
#include <shared/swstate/sw_state_utils.h>
#include <shared/swstate/sw_state_defs.h>
#include <shared/swstate/access/sw_state_access.h>
#include <shared/swstate/sw_state_journal.h>
#include <soc/dcmn/dcmn_crash_recovery_utils.h>
#define SW_STATE_JOURNAL_SAFETY_MARGIN (32)

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_LS_SHARED_SWSTATE


#define VERIFY_SW_STATE_JOURNAL_INITIALIZED(unit) \
    do { \
      if (!sw_state_journal[(unit)]) { \
          _SOC_EXIT_WITH_ERR(_SHR_E_INTERNAL, \
                       (BSL_META_U(unit, \
                          "unit:%d SW State Journal is not initialized.\n"), unit)); \
      } \
    } while(0)


sw_state_journal_t *sw_state_journal[SOC_MAX_NUM_DEVICES];
#ifdef CRASH_RECOVERY_SUPPORT
extern soc_dcmn_cr_t *dcmn_cr_info[SOC_MAX_NUM_DEVICES];
uint8 dcmn_cr_after_roll_back[SOC_MAX_NUM_DEVICES] = {FALSE};
#endif

int sw_state_journal_init(int unit, uint32 size){
    int                                rc = SOC_E_NONE;
    soc_scache_handle_t                wb_handle_orig = 0;
    int                                scache_flags=0;
    int                                already_exists;
    uint8                             *scache_ptr = NULL;
    uint32                             total_buffer_size = 0;
    sw_state_journal_node_t           *tmp_node;
    SOC_INIT_FUNC_DEFS;

    SHR_SW_STATE_SCACHE_HANDLE_SET(wb_handle_orig, unit, SHR_SW_STATE_SCACHE_HANDLE_JOURNAL);

    total_buffer_size = size; 

    /* allocate (or reallocate) new scache buffer */
    rc = shr_sw_state_scache_ptr_get(unit, 
                                     wb_handle_orig, 
                                     (SOC_WARM_BOOT(unit) ? socSwStateScacheRetreive : socSwStateScacheCreate), 
                                     scache_flags,
                                     &total_buffer_size, &scache_ptr, &already_exists);
    _SOC_IF_ERR_EXIT(rc);
    SOC_NULL_CHECK(scache_ptr);

    sw_state_journal[unit] = (sw_state_journal_t *) scache_ptr;

    if (!SOC_WARM_BOOT(unit)) {
        /*init the journal to be all zeros*/
        sal_memset(scache_ptr, 0x0, total_buffer_size);

        /* init the Journal,
           the first node (bottom of stack) get 0 values and no data attached to it */
        tmp_node = (sw_state_journal_node_t *) ((uint8 *) sw_state_journal[unit] + sizeof(sw_state_journal_t));
        tmp_node->offset = 0;
        tmp_node->length = 0;
        tmp_node->is_ptr = FALSE;
        sw_state_journal[unit]->last_node_offset = sizeof(sw_state_journal_t);
        sw_state_journal[unit]->max_size = total_buffer_size - sizeof(sw_state_journal_t) - sizeof(sw_state_journal_node_t);
        sw_state_journal[unit]->size_left = sw_state_journal[unit]->max_size - SW_STATE_JOURNAL_SAFETY_MARGIN;
        sw_state_journal[unit]->nof_log_entries = 0;
    }

exit:
    SOC_FUNC_RETURN;
}

int sw_state_journal_insert(int unit, uint32 offset, uint32 length, uint8 *data, uint8 is_ptr){
    soc_scache_handle_t                wb_handle_orig = 0;
    sw_state_journal_node_t           *tmp_node;
    int                                buff_offset;
    DCMN_TRANSACTION_MODE              tr_mode;

    SOC_INIT_FUNC_DEFS;

    /* exit if not the journaling thread */
    DCMN_CR_EXIT_IF_NOT_JOURNALING_THREAD(unit);
    VERIFY_SW_STATE_JOURNAL_INITIALIZED(unit);


    /* check if logging is enabled */
    if(!soc_dcmn_cr_utils_is_logging(unit)) {
        SOC_EXIT;
    }

    /* no journalling during warm reboot */
    if (SOC_WARM_BOOT(unit)) {
        SOC_EXIT;
    }

    /* if not inside a transaction stop */
    tr_mode = dcmn_cr_info[unit]->transaction_mode;
    if (tr_mode != DCMN_TRANSACTION_MODE_LOGGING) {
        SOC_EXIT;
    }

    /* verify there is enough space left in the journal */
    if (sw_state_journal[unit]->size_left < length + sizeof(sw_state_journal_node_t)) {
        _SOC_EXIT_WITH_ERR(SOC_E_MEMORY,
                           (BSL_META_U(unit,
                              "unit:%d SW State Journal is full.\n"), unit));
    }

    /* prepare the new node's location in the journal */
    tmp_node = (sw_state_journal_node_t * ) 
                ((uint8 *) sw_state_journal[unit] + 
                 sw_state_journal[unit]->last_node_offset + 
                 sizeof(sw_state_journal_node_t) + length);

    /* copy the given data into the journal */
    sal_memcpy(((uint8 *) tmp_node) - length, data, length);

    /* save the needed information for restoring the data */
    tmp_node->length = length;
    tmp_node->offset = offset;
    tmp_node->is_ptr = is_ptr;

    /* update the journal's header */
    sw_state_journal[unit]->last_node_offset += length + sizeof(sw_state_journal_node_t);
    sw_state_journal[unit]->size_left -= length + sizeof(sw_state_journal_node_t);

    /* update the counter */
    sw_state_journal[unit]->nof_log_entries++;

    /* autosync journal only if autosync is enabled */
    /* autosync must be enabled for journaling (until working over shared memory is implemented) */
    if (SOC_AUTOSYNC_IS_ENABLE(unit)) {
        /* get the wb buffer handle */
        SHR_SW_STATE_SCACHE_HANDLE_SET(wb_handle_orig, unit, SHR_SW_STATE_SCACHE_HANDLE_JOURNAL);

        /* get buff_offset */
        buff_offset = (sw_state_journal[unit]->last_node_offset - length); 

        /* perform auto sync on the specific var that has just been set */
        _SOC_IF_ERR_EXIT(shr_sw_state_scache_sync(unit, wb_handle_orig, buff_offset, length + sizeof(sw_state_journal_node_t)));
        
        /* perform auto sync on the header */
        _SOC_IF_ERR_EXIT(shr_sw_state_scache_sync(unit, wb_handle_orig, 0, sizeof(sw_state_journal_node_t))); 
    }

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

int sw_state_journal_roll_back(int unit)
{
    sw_state_journal_node_t           *tmp_node;
    uint8                             *block;

    SOC_INIT_FUNC_DEFS;

    VERIFY_SW_STATE_JOURNAL_INITIALIZED(unit);

    block = (uint8*) shr_sw_state_data_block_header[unit];

    /* start with last node in the journal */
    tmp_node = (sw_state_journal_node_t *) 
                ((uint8 *) sw_state_journal[unit] + 
                 sw_state_journal[unit]->last_node_offset);

    /* roll back the entire journal */
    while (tmp_node->length != 0 && tmp_node->offset != 0) {
        /* check if the data is ptr and not equals to NULL */
        if (tmp_node->is_ptr && *(uint8**)(((uint8 *) tmp_node) - tmp_node->length) != NULL ) {
            (*(uint8**)(((uint8 *) tmp_node) - tmp_node->length)) += sw_state_journal[unit]->ptr_offset;
        }
        /* copy the Node's data into it's offset in the block */
        sal_memcpy(block + tmp_node->offset, ((uint8 *) tmp_node) - tmp_node->length, tmp_node->length);
        tmp_node = (sw_state_journal_node_t *) (((uint8 *) tmp_node) - tmp_node->length - sizeof(sw_state_journal_node_t));
    }

    sw_state_journal_is_after_roll_back_set(unit, TRUE);
    /* it is the caller responsibility to clear the log */

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

int sw_state_journal_clear(int unit)
{
    SOC_INIT_FUNC_DEFS;

    VERIFY_SW_STATE_JOURNAL_INITIALIZED(unit);

    sal_memset(((uint8 *) (sw_state_journal[unit])) + sizeof(sw_state_journal_t) + sizeof(sw_state_journal_node_t),
              0x0,
              sw_state_journal[unit]->max_size - sw_state_journal[unit]->size_left);

    sw_state_journal[unit]->nof_log_entries = 0;
    sw_state_journal[unit]->last_node_offset = sizeof(sw_state_journal_t);
    sw_state_journal[unit]->size_left = sw_state_journal[unit]->max_size - SW_STATE_JOURNAL_SAFETY_MARGIN;

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

int sw_state_journal_update_ptr_offset(int unit, uint32 ptr_offset) {
    SOC_INIT_FUNC_DEFS;

    VERIFY_SW_STATE_JOURNAL_INITIALIZED(unit);

    sw_state_journal[unit]->ptr_offset = ptr_offset;

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

int sw_state_journal_is_after_roll_back_get(int unit, uint8* result) {

    SOC_INIT_FUNC_DEFS;

    *result = dcmn_cr_after_roll_back[unit];

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

int sw_state_journal_is_after_roll_back_set(int unit, uint8 value) {

    SOC_INIT_FUNC_DEFS;

    dcmn_cr_after_roll_back[unit] = value;

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

#endif
