
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Module: internal counter processor work
 *
 * Purpose:
 *     Access to counters hosted by Soc_petra counter processors
 */


#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_CNT

#include <shared/bsl.h>

#include <soc/dpp/drv.h>

#include "soc/drv.h"
#include "soc/dpp/mbcm.h"
#include "bcm_int/dpp/error.h"
#include "bcm_int/dpp/utils.h"
#include "bcm_int/dpp/counters.h"
#include "bcm_int/common/debug.h"
#include "bcm_int/dpp/alloc_mngr_local_lif.h"

#include <shared/swstate/access/sw_state_access.h>
#include <bcm_int/dpp/counters.h>
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/TMC/tmc_api_cnt.h>
#ifdef  BCM_ARAD_SUPPORT   
#include <soc/dpp/ARAD/arad_api_egr_queuing.h>
#include <soc/dpp/ARAD/arad_egr_queuing.h>
#include <soc/dpp/ARAD/arad_cnt.h>
#include <soc/dpp/port_sw_db.h>
#include <shared/swstate/sw_state_sync_db.h>
#endif

STATIC int _bcm_dpp_counter_dma_fifo_dettach(_bcm_dpp_counter_state_t *unitData, int fifo);


/****************************************************************************
 *
 *  Internal implementation
 */

/*
 *  With the _SATURATE_INSTEAD_OF_OVERFLOW option TRUE, counters will saturate
 *  (all bits one) instead of overflow.  With it false, counters will overflow.
 *  The overflow case is a bit faster and slightly improves coherency (not to
 *  mention reduces stack depth by a couple octbytes here and there).
 *
 *  Note that emulated counters also honor this setting -- if saturation, and
 *  the components of an emulated counter would be too big, the emulated
 *  counter will be saturated.  Otherwise it will overflow.
 */
#define _SATURATE_INSTEAD_OF_OVERFLOW TRUE

/*
 *  With the _TRACK_DIAGS_IN_WARMBOOT_ARENA option TRUE, the counter
 *  diagnostics will be kept up to date in the warmboot backing store.  This is
 *  rather expensive, since there are a lot of updates by the background thread
 *  and by directed access (the background thread alone updates numerous
 *  statistics and can run as many as 100 times per second in the default
 *  configuration).
 */
/*
 *  _BACKGROUND_THREAD_ITER_MIN/MAX are the limits on the automatic delay
 *  adjustment that will be used to try to keep the background counter thread
 *  processor use reasonable, while avoiding over-running the FIFO.
 *
 *  _BACKGROUND_THREAD_ITER_DEFAULT is where the background thread starts.  It
 *  will increase the delay as long as the delay is below MAX and the deepest
 *  FIFO during the iteration was below _FIFO_DEPTH_MIN.  It will decrease the
 *  delay as long as the delay is above MIN and the deepest FIFO during the
 *  iteration was above _FIFO_DEPTH_PREF.  DEFAULT should generally be equal to
 *  MAX, since it will otherwise quickly settle to MAX during startup.
 *
 *  _BACKGROUND_THREAD_ITER_ADJUST is the amount of adjustment that will be
 *  applied when making adjustments, with the exception that if the background
 *  thread ever sees a read go above the _FIFO_DEPTH_MAX, and the current value
 *  is greater than MIN, it will bet set to MIN.
 *
 *  These are all expressed in microseconds, but note that different operating
 *  systems have different timer resolution, and the accuracy is guaranteed to
 *  be no tighter than half a timer tic.  Also note that this is delay between
 *  the end of one poll and the beginning of the next -- it is not measured
 *  between the same point on adjacent polls.
 */
#define _FIFO_DEPTH_MIN   2
#define _FIFO_DEPTH_MAX(unit)   (SOC_DPP_DEFS_GET(unit, counter_fifo_depth) - 1)
#define _FIFO_DEPTH_PREF(unit)  ((_FIFO_DEPTH_MIN + _FIFO_DEPTH_MAX(unit)) / 2)
#define  _BCM_COUNTER_THREAD_WAIT_TIMEOUT    (SOC_AUTOSYNC_IS_ENABLE(unit)? 300000000: 1000)
/* Maximum delay access: 100 ms in Petra-B, 1 sec in Arad. Possible enlargement in Arad to up to 3 minutes  */
#define _BACKGROUND_THREAD_ITER_MIN      10000
#define _BACKGROUND_THREAD_ITER_MAX(unit)     (_BACKGROUND_THREAD_ITER_MIN * 100)
#define _BACKGROUND_THREAD_ITER_DEFAULT(unit) (_BACKGROUND_THREAD_ITER_MAX(unit))
/* Reach min <-> max in 20 steps max */
#define _BACKGROUND_THREAD_ITER_ADJUST(unit)    ((_BACKGROUND_THREAD_ITER_MAX(unit) + _BACKGROUND_THREAD_ITER_MIN) / 20) 

/* check if one of the new offset based counters */
#define IS_OFFSET_BASED_COUNTER(counter)  (((counter) >= bcm_dpp_counter_offset0_pkts) && ((counter) <= bcm_dpp_counter_offset7_bytes))
#define OFFSET_BASED_COUNTER_OFFSET_GET(counter)  (((counter) - bcm_dpp_counter_offset0_pkts)/2)

/* check lsb/msb counter*/
#define IS_STR(propval, string) (sal_strcmp(propval, (string)) == 0)
#define IS_STR_LSB0(propval, string) ((sal_strcmp(propval, (string)) == 0) || (sal_strcmp(propval, (string "_0")) == 0) || (sal_strcmp(propval, (string "_LSB_0")) == 0)  || (sal_strcmp(propval, (string "_LSB")) == 0))
#define IS_STR_LSB1(propval, string) ((sal_strcmp(propval, (string "_1")) == 0) || (sal_strcmp(propval, (string "_LSB_1")) == 0))
#define IS_STR_MSB0(propval, string) ((sal_strcmp(propval, (string "_MSB_0")) == 0) || (sal_strcmp(propval, (string "_MSB"))== 0))
#define IS_STR_MSB1(propval, string) ((sal_strcmp(propval, (string "_MSB_1")) == 0))

#define NUM_OF_COUNTER_COMMANDS (2)

/*define for each counter(except the offset based counters) in bcm_dpp_counter_t what is it count: 
  e.g fwd_all = fwd_green, fwd_yellow, fwd_red, fwd_black*/
uint32 counters_def[bcm_dpp_counter_count] = {0};




/*
 *  The hardware contains multiple counter processors; each one contains a
 *  number of counter pairs.  Unhappily, the hardware counter pairs only
 *  contain a 32b byte counter and a 25b frame counter.  These both are rather
 *  too small for the device, based upon its maximum bandwidth, and so there
 *  is a mechanism that places counters that are 'nearly ready to overflow'
 *  into a FIFO, which must be cleared regularly (at least 10Hz[?]).
 *
 *  While just reading the FIFO is enough to maintain things, it is not enough
 *  when the application needs to read or write counters.  There are several
 *  situations where the mechanism is not sufficient, but most of them relate
 *  to situations where data rate is not near the maximum:
 *
 *  * Since the counter 'algorithmic' mechanism only ejects counters that are
 *    near to full, low-rate counters will have long latencies between updates,
 *    and the update granularity will not reflect the actual rate except over
 *    extremely long intervals.
 *
 *  In order to get counters to be as accurate as possible, we want to ensure a
 *  counter that is about to be read is updated, and that a counter that is
 *  about to be written does not have residue values, either in the hardware
 *  counter buffers or in the FIFO.  In either location it could have up to
 *  2^25-1 frames and 2^32-1 bytes -- no small discrepancy.
 *
 *  We don't want to block the background task on semaphore, nor do we want
 *  every counter access to touch a semaphore; mostly this is because of the
 *  overhead of dealing with them on many operating systems.
 *
 *  Instead of mucking about with semaphores, there is a simple locking
 *  mechanism designed into the local cache tables.  The background thread sets
 *  a flag (background_active) when it is updating the cache, and clears it
 *  when it is done.  If another flag, background_defer, is set, it will skip
 *  the current cycle.  When accessing counters on behalf of an API caller, we
 *  set the background_defer flag and then wait for the background_active flag
 *  to become zero.  Once this is done, we clear the FIFO and update the
 *  counter(s) that were requested, and then set background_defer to zero.
 */

/*
 *  Names for the various statistics
 */
CONST char* CONST bcm_dpp_counter_t_names[bcm_dpp_counter_count + 1] = {
    /* forwarded and dropped frames and bytes */
    "packets",
    "bytes",
    "green packets",
    "green bytes",
    "not_green packets",
    "not_green bytes",
    "yellow packets",
    "yellow bytes",
    "not_yellow packets",
    "not_yellow bytes",
    "red packets",
    "red bytes",
    "not_red packets",
    "not_red bytes",
    /* forwarded frames and bytes */
    "forwarded packets",
    "forwarded bytes",
    "forwarded green packets",
    "forwarded green bytes",
    "forwarded not_green packets",
    "forwarded not_green bytes",
    "forwarded yellow packets",
    "forwarded yellow bytes",
    "forwarded not_yellow packets",
    "forwarded not_yellow bytes",
    "forwarded red packets",
    "forwarded red bytes",
    "forwarded not_red packets",
    "forwarded not_red bytes",
    /* discarded frames and bytes */
    "dropped packets",
    "dropped bytes",
    "dropped green packets",
    "dropped green bytes",
    "dropped not_green packets",
    "dropped not_green bytes",
    "dropped yellow packets",
    "dropped yellow bytes",
    "dropped not_yellow packets",
    "dropped not_yellow bytes",
    "dropped red packets",
    "dropped red bytes",
    "dropped not_red packets",
    "dropped not_red bytes",
    /* entry by index */
    "offset0 packets",
    "offset0 bytes",
    "offset1 packets",
    "offset1 bytes",
    "offset2 packets",
    "offset2_bytes",
    "offset3 packets",
    "offset3 bytes",
    "offset4 packets",
    "offset4 bytes",
    "offset5 packets",
    "offset5 bytes",
    "offset6 packets",
    "offset6 bytes",
    "offset7 packets",
    "offset7 bytes",
    /* limit */
    "unknown statistic" /* NOT A VALID ENTRY; MUST BE LAST */
};

/*
 *  Handle the counter state for multiple units.
 */
typedef struct _bcm_dpp_counter_state_buffer_s {
    _bcm_dpp_counter_state_t *state;
    uint8 dirty_bitmap;
} _bcm_dpp_counter_state_buffer_t;

_bcm_dpp_counter_state_buffer_t _bcm_dpp_counter_state_buffer[SOC_MAX_NUM_DEVICES];
#define _COUNTER_UNIT_CHECK(_u,_ud) \
    if ((0 > (_u)) || (SOC_MAX_NUM_DEVICES <= (_u))) { \
        LOG_ERROR(BSL_LS_BCM_CNT, \
                  (BSL_META_U(unit, \
                              "unit %d is not valid\n"), \
                   _u)); \
        BCM_RETURN_VAL_EXIT(BCM_E_UNIT); \
    } \
    if (NULL == ((_ud) = _bcm_dpp_counter_state_buffer[unit].state)) { \
        LOG_ERROR(BSL_LS_BCM_CNT, \
                  (BSL_META_U(unit, \
                              "unit %d does not have DPP counters" \
                              " inited\n"), \
                   _u)); \
        BCM_RETURN_VAL_EXIT(BCM_E_INIT); \
    }

SOC_TMC_CNT_RESULT_ARR _fifo_results[SOC_MAX_NUM_DEVICES];
int _bcm_counter_thread_is_running[SOC_MAX_NUM_DEVICES];

/*
 *  Offsets into the cache based upon the color and mode
 */
#define _OFS_E_CN 0
#define _OFS_E_CY_GREEN 0
#define _OFS_E_CY_YELLOW _SUB_COUNT
/*
 *  Some of the code assumes there will be no more than 32 sub-counters
 *  in a counter set.  Right now, the maximum is 10, so hopefully that will
 *  hold true for a while yet.  Considering G,Y,R,K for both forward and drop
 *  of bytes and frames comes to sixteen total, it should work.
 *
 *  Of course, if we add support for black, a lot of the names (particularly
 *  the negative ones) in bcm_dpp_counter_e probably need to change.
 */
#define _OFS_COUNT (_SUB_COUNT * 8)

/*Compare a source */
#define BCM_DPP_COUNTER_SOURCE_MATCH(unitData, crps_index, _src_core, _src_type, _command_id, _eg_type)       \
        if ((unitData->proc[crps_index].mode.src_type == _src_type) &&                                                     \
            (unitData->proc[crps_index].mode.command_id == _command_id) &&                                                 \
            (unitData->proc[crps_index].mode.src_core == _src_core || _src_core == BCM_CORE_ALL) &&                        \
            (unitData->proc[crps_index].mode.mode_eg.type == _eg_type || _eg_type == SOC_TMC_CNT_NOF_MODE_EG_TYPES_ARAD) &&\
            (unitData->proc[crps_index].valid & BCM_DPP_COUNTER_PROC_INFO_VALID)) \



#define COUNTERS_ACCESS sw_state_access[unit].dpp.bcm.counter
#define COUNTERS_PROC_ACCESS sw_state_access[unit].dpp.bcm.counter.proc
#define COUNTERS_FIFO_ACCESS sw_state_access[unit].dpp.bcm.counter.fifo
#define COUNTERS_SYNC_ACCESS sw_state_sync_db[unit].dpp.counter

bcm_dpp_counter_bg_thread_and_dma_data_t glb_bg_thread_and_dma_data[SOC_MAX_NUM_DEVICES];
/* lif profile table is assistance array to find quickly the right profile ID as stored in the LIF-MGMT inlif/outLif DBs*/
/* key is user profile ID, value is Lif-MGMT profile ID */
int lif_profile_mapping_table[SOC_MAX_NUM_DEVICES][(SOC_TMC_CNT_LIF_COUNTING_MAX_NOF_RANGES_PER_SOURCE + SOC_TMC_CNT_LIF_COUNTING_MIN_NOF_RANGES_PER_SOURCE)];

/* get number of counters per entry. 2 for double entry mode, 1 for all the rest */
#define BCM_DPP_NOF_COUNTERS_PER_ENTRY_GET(_unit, _format) \
        ((SOC_IS_ARADPLUS(_unit)) && (_format == ARAD_CNT_FORMAT_PKTS_AND_PKTS)) ?  2 : 1;


STATIC int 
_bcm_dpp_counter_proc_select_update(int unit, int engine, uint32 select_bitmap, _bcm_dpp_counter_proc_info_t* proc_info, int indexForAvailOffset) {
    int offset;
    int rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    if ((select_bitmap & COUNTER_UPDATE_SELECT_PROC_INFO_ALL) || (select_bitmap & COUNTER_UPDATE_SELECT_PROC_INFO_SPECIFIC_DATA))
    {
        rv = COUNTERS_PROC_ACCESS.valid.set(unit, engine, proc_info->valid);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = COUNTERS_PROC_ACCESS.proc_id.set(unit, engine, proc_info->proc_id);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = COUNTERS_PROC_ACCESS.mode.set(unit, engine, &proc_info->mode);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = COUNTERS_PROC_ACCESS.native.bit_range_write(unit, engine, 0, 0, bcm_dpp_counter_count, proc_info->native);
        BCMDNX_IF_ERR_EXIT(rv);
        if (select_bitmap & COUNTER_UPDATE_SELECT_PROC_INFO_ALL)
        {
            for (offset = 0; offset < bcm_dpp_counter_count; offset++)
            {
                rv = COUNTERS_PROC_ACCESS.avail_offsets.set(unit, engine, offset, proc_info->avail_offsets[offset]);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
        else
        {
            rv = COUNTERS_PROC_ACCESS.avail_offsets.set(unit, engine, indexForAvailOffset, proc_info->avail_offsets[indexForAvailOffset]);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }
    if (select_bitmap & COUNTER_UPDATE_SELECT_DIAGS) {
        rv = COUNTERS_PROC_ACCESS.diag.set(unit, engine, &proc_info->diag);
        BCMDNX_IF_ERR_EXIT(rv);
    }
exit:
    BCMDNX_FUNC_RETURN;
}
/*
int 
_bcm_dpp_counter_proc_update(int unit, 
                             int engine, 
                             _bcm_dpp_counter_proc_info_t* proc_info) {
    int rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    rv = _bcm_dpp_counter_proc_select_update(unit, engine, COUNTER_UPDATE_SELECT_ALL, proc_info);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}
*/
STATIC int 
_bcm_dpp_counter_unit_select_update(int unit, uint32 select_bitmap, _bcm_dpp_counter_state_t *unitData, bcm_dpp_counter_proc_specific_data_s * specific_proc_data) {
    int rv = BCM_E_NONE;
    int engine;
    int fifo;
    int range_id;
    int is_ingress;
    int indexProc, index;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    if (!SOC_WARM_BOOT(unit)) {
        unitData->cunit = unit;
        rv = COUNTERS_ACCESS.cunit.set(unit, unit);
        BCMDNX_IF_ERR_EXIT(rv);

        if (select_bitmap & COUNTER_UPDATE_SELECT_BACKROUND_INFO) {
            rv = COUNTERS_ACCESS.sampling_interval_configured.set(unit, unitData->sampling_interval_configured);      
            BCMDNX_IF_ERR_EXIT(rv);         
            rv = COUNTERS_ACCESS.background_defer.set(unit, unitData->background_defer);
            BCMDNX_IF_ERR_EXIT(rv);
            rv = COUNTERS_ACCESS.background_active.set(unit, unitData->background_active);
            BCMDNX_IF_ERR_EXIT(rv);
            rv = COUNTERS_ACCESS.background_disable.set(unit, unitData->background_disable);
            BCMDNX_IF_ERR_EXIT(rv);
            rv = COUNTERS_ACCESS.foreground_hit.set(unit, unitData->foreground_hit);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        if ((select_bitmap & COUNTER_UPDATE_SELECT_PROC_INFO_ALL) || (select_bitmap & COUNTER_UPDATE_SELECT_PROC_INFO_SPECIFIC_DATA))
        {
            rv = COUNTERS_ACCESS.num_counter_procs.set(unit, unitData->num_counter_procs);
            BCMDNX_IF_ERR_EXIT(rv);
            rv = COUNTERS_ACCESS.global_info.set(unit, &unitData->global_info);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        if (select_bitmap & COUNTER_UPDATE_SELECT_DIAGS) {
            rv = COUNTERS_ACCESS.diag.set(unit, &unitData->diag);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        if (select_bitmap & COUNTER_UPDATE_SELECT_PROC_INFO_ALL)
        {
            for (engine = 0; engine < (SOC_DPP_DEFS_GET(unit, nof_counter_processors) + SOC_DPP_DEFS_GET(unit, nof_small_counter_processors)); engine++)
            {
                rv = _bcm_dpp_counter_proc_select_update(unit, engine, select_bitmap, &(unitData->proc[engine]), 0);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
        if (select_bitmap & COUNTER_UPDATE_SELECT_PROC_INFO_SPECIFIC_DATA)
        {
            if (specific_proc_data == NULL)
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("can't update specific engine data because specific_proc_data == NULL")));
            }
            for (indexProc = 0; indexProc < specific_proc_data->procArrSize; indexProc++)
            {
                for (index=0; index<specific_proc_data->typeSize; index++)
                {
                    rv = _bcm_dpp_counter_proc_select_update(unit, specific_proc_data->procArr[indexProc], select_bitmap,
                            &(unitData->proc[specific_proc_data->procArr[indexProc]]), specific_proc_data->type[index]);
                    BCMDNX_IF_ERR_EXIT(rv);
                }
            }
        }

        if (select_bitmap & COUNTER_UPDATE_SELECT_DIAGS) {
            for (fifo = 0; fifo < SOC_DPP_DEFS_GET(unit, nof_counter_fifos); fifo++) {
                rv = COUNTERS_FIFO_ACCESS.fifo_read_passes.set(unit, fifo, unitData->fifo[fifo].fifo_read_passes);
                BCMDNX_IF_ERR_EXIT(rv);
                rv = COUNTERS_FIFO_ACCESS.fifo_read_items.set(unit, fifo, unitData->fifo[fifo].fifo_read_items);
                BCMDNX_IF_ERR_EXIT(rv);
                rv = COUNTERS_FIFO_ACCESS.fifo_read_last.set(unit, fifo, unitData->fifo[fifo].fifo_read_last);
                BCMDNX_IF_ERR_EXIT(rv);
                rv = COUNTERS_FIFO_ACCESS.fifo_read_max.set(unit, fifo, unitData->fifo[fifo].fifo_read_max);
                BCMDNX_IF_ERR_EXIT(rv);
                rv = COUNTERS_FIFO_ACCESS.fifo_read_fails.set(unit, fifo, unitData->fifo[fifo].fifo_read_fails);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
        if (select_bitmap & COUNTER_UPDATE_SELECT_FIFO_INFO) {
            for (fifo = 0; fifo < SOC_DPP_DEFS_GET(unit, nof_counter_fifos); fifo++) {
                rv = COUNTERS_FIFO_ACCESS.ref_count.set(unit, fifo, unitData->fifo[fifo].ref_count);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
        if (select_bitmap & COUNTER_UPDATE_SELECT_LIF_INFO) {
            for (is_ingress = 0; is_ingress < SOC_TMC_CNT_LIF_COUNTING_NOF_SOURCES; is_ingress++) {
                for (range_id = 0; range_id < SOC_TMC_CNT_LIF_COUNTING_MAX_NOF_RANGES_PER_SOURCE; range_id++) {
                    rv = COUNTERS_ACCESS.lif_ranges.set(unit, is_ingress, range_id, &unitData->lif_ranges[is_ingress][range_id]);
                    BCMDNX_IF_ERR_EXIT(rv);
                }
            }
        }
        /*Clear the updated buffers*/
        _bcm_dpp_counter_state_buffer[unit].dirty_bitmap &= ~select_bitmap;
    }
    else
    {
        _bcm_dpp_counter_state_buffer[unit].dirty_bitmap |= select_bitmap; /*Cannnot update - mark as dirty*/
    }
exit:
      BCMDNX_FUNC_RETURN;
}

STATIC int 
_bcm_dpp_counter_unit_update(int unit, _bcm_dpp_counter_state_t *unitData, uint32 select_bitmap, bcm_dpp_counter_proc_specific_data_s * specific_proc_data) {
    int rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    rv = _bcm_dpp_counter_unit_select_update(unit, select_bitmap, unitData, specific_proc_data);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int 
_bcm_dpp_counter_proc_fill(int unit, int engine, _bcm_dpp_counter_proc_info_t* proc_info, int indexForAvailOffset, uint32 select_bitmap) {
    int offset;
    int rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    if ((select_bitmap & COUNTER_UPDATE_SELECT_PROC_INFO_ALL) || (select_bitmap & COUNTER_UPDATE_SELECT_PROC_INFO_SPECIFIC_DATA))
    {
        rv = COUNTERS_PROC_ACCESS.valid.get(unit, engine, &proc_info->valid);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = COUNTERS_PROC_ACCESS.proc_id.get(unit, engine, &proc_info->proc_id);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = COUNTERS_PROC_ACCESS.mode.get(unit, engine, &proc_info->mode);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = COUNTERS_PROC_ACCESS.native.bit_range_read(unit, engine, 0, 0, bcm_dpp_counter_count, proc_info->native);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    if(select_bitmap & COUNTER_UPDATE_SELECT_PROC_INFO_ALL)
    {
        for (offset = 0; offset < bcm_dpp_counter_count; offset++) {
            rv = COUNTERS_PROC_ACCESS.avail_offsets.get(unit, engine, offset, &proc_info->avail_offsets[offset]);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }
    if(select_bitmap & COUNTER_UPDATE_SELECT_PROC_INFO_SPECIFIC_DATA)
    {
        rv = COUNTERS_PROC_ACCESS.avail_offsets.get(unit, engine, indexForAvailOffset, &proc_info->avail_offsets[indexForAvailOffset]);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    if (select_bitmap & COUNTER_UPDATE_SELECT_DIAGS)
    {
        rv = COUNTERS_PROC_ACCESS.diag.get(unit, engine, &proc_info->diag);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int 
_bcm_dpp_counter_unit_fill(int unit, _bcm_dpp_counter_state_t **unitDataPtr, uint32 select_bitmap, bcm_dpp_counter_proc_specific_data_s * specific_proc_data) {
    _bcm_dpp_counter_state_t *unitData = *unitDataPtr;
    int rv = BCM_E_NONE;
    int engine;
    int fifo;
    uint8 is_allocated;
    int range_id;
    int is_ingress;
    int index, indexProc;
    BCMDNX_INIT_FUNC_DEFS;

    rv = COUNTERS_ACCESS.is_allocated(unit, &is_allocated);
    BCMDNX_IF_ERR_EXIT(rv);

    if(!is_allocated){
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Counter data state is not allocated\n")));
    }
    unitData->cunit = unit;
    if ((select_bitmap & COUNTER_UPDATE_SELECT_PROC_INFO_ALL) || (select_bitmap & COUNTER_UPDATE_SELECT_PROC_INFO_SPECIFIC_DATA))
    {          
        rv = COUNTERS_ACCESS.num_counter_procs.get(unit, &unitData->num_counter_procs);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = COUNTERS_ACCESS.global_info.haveStatTag.get(unit, &unitData->global_info.haveStatTag);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = COUNTERS_ACCESS.global_info.cache_only.get(unit, &unitData->global_info.cache_only);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    if (select_bitmap & COUNTER_UPDATE_SELECT_BACKROUND_INFO)
    {
        rv = COUNTERS_ACCESS.sampling_interval_configured.get(unit, &unitData->sampling_interval_configured);        
        BCMDNX_IF_ERR_EXIT(rv);     
        rv = COUNTERS_ACCESS.background_defer.get(unit, &unitData->background_defer);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = COUNTERS_ACCESS.background_active.get(unit, &unitData->background_active);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = COUNTERS_ACCESS.background_disable.get(unit, &unitData->background_disable);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = COUNTERS_ACCESS.foreground_hit.get(unit, &unitData->foreground_hit);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    if (select_bitmap & COUNTER_UPDATE_SELECT_DIAGS)
    {
        rv = COUNTERS_ACCESS.diag.get(unit, &unitData->diag);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    if (select_bitmap & COUNTER_UPDATE_SELECT_PROC_INFO_ALL)
    {
        for (engine = 0; engine < (SOC_DPP_DEFS_GET(unit, nof_counter_processors) + SOC_DPP_DEFS_GET(unit, nof_small_counter_processors)); engine++) {
            rv = _bcm_dpp_counter_proc_fill(unit, engine, &(unitData->proc[engine]), 0, select_bitmap);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }
    if (select_bitmap & COUNTER_UPDATE_SELECT_PROC_INFO_SPECIFIC_DATA)
    {
        if (specific_proc_data == NULL)
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("can't fill specific engine data because specific_proc_data == NULL")));
        }
        for (indexProc = 0; indexProc < specific_proc_data->procArrSize; indexProc++)
        {
            for (index=0; index<specific_proc_data->typeSize; index++)
            {
                rv = _bcm_dpp_counter_proc_fill(unit, specific_proc_data->procArr[indexProc], &(unitData->proc[specific_proc_data->procArr[indexProc]]), specific_proc_data->type[index], select_bitmap);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
    }
    if (select_bitmap & COUNTER_UPDATE_SELECT_DIAGS)
    {
        for (fifo = 0; fifo < SOC_DPP_DEFS_GET(unit, nof_counter_fifos); fifo++) {
            rv = COUNTERS_FIFO_ACCESS.get(unit, fifo, &unitData->fifo[fifo]);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }
    if (select_bitmap & COUNTER_UPDATE_SELECT_LIF_INFO)
    {
        for (is_ingress = 0; is_ingress < SOC_TMC_CNT_LIF_COUNTING_NOF_SOURCES; is_ingress++) {
            for (range_id = 0; range_id < SOC_TMC_CNT_LIF_COUNTING_MAX_NOF_RANGES_PER_SOURCE; range_id++) {
                rv = COUNTERS_ACCESS.lif_ranges.get(unit, is_ingress, range_id, &unitData->lif_ranges[is_ingress][range_id]);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int 
_bcm_dpp_counter_unit_check_and_fill(int unit, _bcm_dpp_counter_state_t **unitData, uint32 select_bitmap,  bcm_dpp_counter_proc_specific_data_s * specific_proc_data) {
    int rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;
    _COUNTER_UNIT_CHECK(unit, *unitData);

    if (!SOC_WARM_BOOT(unit)) {
        if (_bcm_dpp_counter_state_buffer[unit].dirty_bitmap) {
            rv = _bcm_dpp_counter_unit_select_update(unit, _bcm_dpp_counter_state_buffer[unit].dirty_bitmap, *unitData, NULL);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        rv = _bcm_dpp_counter_unit_fill(unit, unitData, select_bitmap, specific_proc_data);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}
int
bcm_dpp_counter_state_lock_take(int unit) {
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    if (0 != sal_mutex_take(COUNTERS_SYNC_ACCESS.cacheLock, sal_mutex_FOREVER)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_TIMEOUT, (_BSL_BCM_MSG("unable to take unit %d counter mutex"), unit));
    }
exit:
    BCMDNX_FUNC_RETURN;
}
int
bcm_dpp_counter_state_lock_give(int unit) {
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    if (sal_mutex_give(COUNTERS_SYNC_ACCESS.cacheLock)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("unable to give unit %d counter mutex"), unit));
    }
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_counter_state_set_all(_bcm_dpp_counter_state_t *unitData, int proc, uint64 counter) {
    int ofs;
    int rv = BCM_E_NONE;
    int unit = unitData->cunit;  
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(unitData);

    if (unitData->num_counter_procs <= proc) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d invalid counter processor ID %u"), unit, proc));
    }
    for (ofs = 0; ofs < unitData->proc[proc].mode.num_counters*_SUB_COUNT; ofs += _SUB_COUNT) {
  
        rv = COUNTERS_ACCESS.counter.set(unit, proc, ofs + _SUB_PKTS, counter);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = COUNTERS_ACCESS.counter.set(unit, proc, ofs + _SUB_BYTES, counter);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    
exit:
    BCMDNX_FUNC_RETURN;
}


int
bcm_dpp_counter_state_diag_unmask(int unit) {
    _bcm_dpp_counter_state_t *unitData;
    int rv = BCM_E_NONE;
    int ofs;
    int proc;
    BCMDNX_INIT_FUNC_DEFS;
    _COUNTER_UNIT_CHECK(unit, unitData);

    /*restore data*/
    rv = _bcm_dpp_counter_unit_select_update(unit, COUNTER_UPDATE_SELECT_BACKROUND_INFO | COUNTER_UPDATE_SELECT_DIAGS, unitData, NULL);
    BCMDNX_IF_ERR_EXIT(rv);
    _bcm_dpp_counter_state_buffer[unit].dirty_bitmap &= ~(COUNTER_UPDATE_SELECT_BACKROUND_INFO | COUNTER_UPDATE_SELECT_DIAGS);
    if (unitData->counter) {
        for (proc = 0; proc < unitData->num_counter_procs; proc++) {
            if (!(unitData->proc[proc].valid & BCM_DPP_COUNTER_PROC_INFO_VALID)) {
                continue;
            }
            for (ofs = 0; ofs < SOC_TMC_NOF_COUNTERS_IN_ENGINE(unit, proc); ofs += _SUB_COUNT) {
                rv = COUNTERS_ACCESS.counter.set(unit, proc, ofs + _SUB_PKTS, unitData->counter[proc][ofs + _SUB_PKTS]);
                BCMDNX_IF_ERR_EXIT(rv);

                rv = COUNTERS_ACCESS.counter.set(unit, proc, ofs + _SUB_BYTES, unitData->counter[proc][ofs + _SUB_BYTES]);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
    }
    rv = bcm_dpp_counter_state_lock_give(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    if (unitData->counter) {
        for (proc = 0; proc < unitData->num_counter_procs; proc++) {
            if (unitData->counter[proc]) {
                BCM_FREE(unitData->counter[proc]);
            }
        }
        BCM_FREE(unitData->counter);
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_dpp_counter_state_diag_mask(int unit) {
    _bcm_dpp_counter_state_t *zeroUnitData, *unitData;
    int rv = BCM_E_NONE;
    int alloc_size;
    int ofs;
    int proc;
    uint64 counter;
    BCMDNX_INIT_FUNC_DEFS;

    sal_memset(&zeroUnitData, 0, sizeof(zeroUnitData));
    BCMDNX_ALLOC(zeroUnitData, sizeof(*zeroUnitData), "Unit info buffer");
    if (zeroUnitData == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failure")));
    }
    sal_memset(zeroUnitData, 0, sizeof(*zeroUnitData));

    BCMDNX_ALLOC(zeroUnitData->proc, (sizeof(*(zeroUnitData->proc))) * (SOC_DPP_DEFS_GET(unit, nof_counter_processors) + SOC_DPP_DEFS_GET(unit, nof_small_counter_processors)), "Proc info buffer");
    if (zeroUnitData->proc == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failure")));
    }
    sal_memset(zeroUnitData->proc, 0, (sizeof(*(zeroUnitData->proc))) * (SOC_DPP_DEFS_GET(unit, nof_counter_processors) + SOC_DPP_DEFS_GET(unit, nof_small_counter_processors)));

    _COUNTER_UNIT_CHECK(unit, unitData);

    rv = _bcm_dpp_counter_unit_fill(unit, &unitData, COUNTER_UPDATE_SELECT_ALL, NULL); /*save data*/
    BCMDNX_IF_ERR_EXIT(rv);
    /*Allocate a buffer for SW cache*/
    alloc_size = sizeof(uint64*) * (SOC_DPP_DEFS_GET(unit, nof_counter_processors) + SOC_DPP_DEFS_GET(unit, nof_small_counter_processors));
    unitData->counter = NULL;
    BCMDNX_ALLOC(unitData->counter, alloc_size, "Unit info buffer");
    if (unitData->counter == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failure")));
    }
    sal_memset(unitData->counter, 0, alloc_size);
    for (proc = 0; proc < unitData->num_counter_procs; proc++) {
        if (proc < SOC_DPP_DEFS_GET(unit, nof_counter_processors)) {
            alloc_size = sizeof(uint64) * _SUB_COUNT * SOC_DPP_DEFS_GET(unit, counters_per_counter_processor);
        } else {
            alloc_size = sizeof(uint64) * _SUB_COUNT * SOC_DPP_DEFS_GET(unit, counters_per_small_counter_processor);
        }
        BCMDNX_ALLOC(unitData->counter[proc], alloc_size, "Unit info buffer");
        if (unitData->counter[proc] == NULL) {
            int proc_ndx;
            for(proc_ndx = 0; proc_ndx < proc; ++proc_ndx) {
                BCM_FREE(unitData->counter[proc_ndx]);
            }
            BCM_FREE(unitData->counter);
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failure")));
        }
        sal_memset(unitData->counter[proc], 0, alloc_size);
    }

    rv = bcm_dpp_counter_state_lock_take(unit);
    BCMDNX_IF_ERR_CONT(rv);

    COMPILER_64_ZERO(counter);
    for (proc = 0; proc < unitData->num_counter_procs; proc++) {
        if (!(unitData->proc[proc].valid & BCM_DPP_COUNTER_PROC_INFO_VALID)) {
            continue;
        }
        for (ofs = 0; ofs < SOC_TMC_NOF_COUNTERS_IN_ENGINE(unit, proc) ; ofs += _SUB_COUNT) {
            rv = COUNTERS_ACCESS.counter.get(unit, proc, ofs + _SUB_PKTS, &unitData->counter[proc][ofs + _SUB_PKTS]);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = COUNTERS_ACCESS.counter.set(unit, proc, ofs + _SUB_PKTS, counter);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = COUNTERS_ACCESS.counter.get(unit, proc, ofs + _SUB_BYTES, &unitData->counter[proc][ofs + _SUB_BYTES]);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = COUNTERS_ACCESS.counter.set(unit, proc, ofs + _SUB_BYTES, counter);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }
    /*write zero data*/
    rv = _bcm_dpp_counter_unit_select_update(unit, COUNTER_UPDATE_SELECT_BACKROUND_INFO | COUNTER_UPDATE_SELECT_DIAGS, zeroUnitData, NULL);
    BCMDNX_IF_ERR_EXIT(rv);
    _bcm_dpp_counter_state_buffer[unit].dirty_bitmap |= COUNTER_UPDATE_SELECT_BACKROUND_INFO | COUNTER_UPDATE_SELECT_DIAGS;

exit:
    if (zeroUnitData) {
        BCM_FREE(zeroUnitData->proc);
        BCM_FREE(zeroUnitData);
    }

    BCMDNX_FUNC_RETURN;
}

/*
 *   Name
 *     _bcm_dpp_counters_per_set
 *   Purpose
 *     Get the number of counters in a set
 *   Arguments
 *     IN/OUT _bcm_dpp_counter_state_t *unitData = info for the unit
 *     IN unsigned int proc = which counter processor
 *     OUT unsigned int *counters = where to put counters per set
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 *   Notes
 */
STATIC int
_bcm_dpp_counters_per_set(_bcm_dpp_counter_state_t *unitData,
                          unsigned int proc,
                          unsigned int *counters)
{
    int unit = unitData->cunit;
    BCMDNX_INIT_FUNC_DEFS;
    if (unitData->num_counter_procs <= proc) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("unit %d invalid counter"
                                       "processor ID %u"),
                                       unit,
                                       proc));
    }
    switch(unitData->proc[proc].mode.mode_statistics){
    case SOC_TMC_CNT_MODE_STATISTICS_ALL_NO_COLOR:
    case SOC_TMC_CNT_MODE_STATISTICS_FWD_NO_COLOR:
    case SOC_TMC_CNT_MODE_STATISTICS_DROP_NO_COLOR:
        *counters = 1;
        break;
    case SOC_TMC_CNT_MODE_STATISTICS_NO_COLOR:
        *counters =  2;
        break;
    case SOC_TMC_CNT_MODE_STATISTICS_COLOR_RES_ENQ_HI:
        *counters = 2;
        break;
    case SOC_TMC_CNT_MODE_STATISTICS_COLOR_RES_LOW:
        *counters =  4;
        break;

    case SOC_TMC_CNT_MODE_STATISTICS_COLOR_RES_HI:
        *counters =  5;
        break;
#ifdef BCM_88660_A0
    case SOC_TMC_CNT_MODE_STATISTICS_FWD_SIMPLE_COLOR:
    case SOC_TMC_CNT_MODE_STATISTICS_DROP_SIMPLE_COLOR: /*slide*/
        if(SOC_IS_ARAD_B1_AND_BELOW(unit)){
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("unit %d invalid counter mode %d"),
             unit,
             unitData->proc[proc].mode.mode_statistics));
        }
        *counters = 2;
        break;
    case SOC_TMC_CNT_MODE_STATISTICS_CONFIGURABLE_OFFSETS:
        if(SOC_IS_ARAD_B1_AND_BELOW(unit)){
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("unit %d invalid counter mode %d"),
             unit,
             unitData->proc[proc].mode.mode_statistics));
        }
        *counters = unitData->proc[proc].mode.custom_mode_params.set_size;
        break;
#endif
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("unit %d invalid counter mode %d"),
                     unit,
                     unitData->proc[proc].mode.mode_statistics));
    } /* switch (unitData->proc[proc].format) */
    BCM_EXIT;
exit:
      BCMDNX_FUNC_RETURN;
}


void _bcm_dpp_counters_def_init(void){
    counters_def[bcm_dpp_counter_pkts]              = counters_def[bcm_dpp_counter_bytes]                 = GREEN_FWD | GREEN_DROP | YELLOW_FWD | YELLOW_DROP | RED_FWD | RED_DROP | BLACK_FWD | BLACK_DROP;
    counters_def[bcm_dpp_counter_green_pkts]        = counters_def[bcm_dpp_counter_green_bytes]           = GREEN_FWD | GREEN_DROP;
    counters_def[bcm_dpp_counter_not_green_pkts]    = counters_def[bcm_dpp_counter_not_green_bytes]       = YELLOW_FWD | YELLOW_DROP | RED_FWD | RED_DROP | BLACK_FWD | BLACK_DROP ;
    counters_def[bcm_dpp_counter_yellow_pkts]       = counters_def[bcm_dpp_counter_yellow_bytes]          = YELLOW_FWD | YELLOW_DROP | RED_FWD | RED_DROP;
    counters_def[bcm_dpp_counter_not_yellow_pkts]   = counters_def[bcm_dpp_counter_not_yellow_bytes]      = GREEN_FWD | GREEN_DROP | BLACK_FWD | BLACK_DROP;
    counters_def[bcm_dpp_counter_red_pkts]          = counters_def[bcm_dpp_counter_red_bytes]             = BLACK_FWD | BLACK_DROP;
    counters_def[bcm_dpp_counter_not_red_pkts]      = counters_def[bcm_dpp_counter_not_red_bytes]         = GREEN_FWD | GREEN_DROP | YELLOW_FWD | YELLOW_DROP | RED_FWD | RED_DROP;
    counters_def[bcm_dpp_counter_fwd_pkts]           = counters_def[bcm_dpp_counter_fwd_bytes]            = GREEN_FWD | YELLOW_FWD | RED_FWD | BLACK_FWD;
    counters_def[bcm_dpp_counter_fwd_green_pkts]     = counters_def[bcm_dpp_counter_fwd_green_bytes]      = GREEN_FWD;
    counters_def[bcm_dpp_counter_fwd_not_green_pkts] = counters_def[bcm_dpp_counter_fwd_not_green_bytes]  = YELLOW_FWD | RED_FWD | BLACK_FWD;
    counters_def[bcm_dpp_counter_fwd_yellow_pkts]    = counters_def[bcm_dpp_counter_fwd_yellow_bytes]     = YELLOW_FWD | RED_FWD;
    counters_def[bcm_dpp_counter_fwd_not_yellow_pkts]= counters_def[bcm_dpp_counter_fwd_not_yellow_bytes] = GREEN_FWD | BLACK_FWD;
    counters_def[bcm_dpp_counter_fwd_red_pkts]       = counters_def[bcm_dpp_counter_fwd_red_bytes]        = BLACK_FWD;
    counters_def[bcm_dpp_counter_fwd_not_red_pkts]   = counters_def[bcm_dpp_counter_fwd_not_red_bytes]    = GREEN_FWD | YELLOW_FWD | RED_FWD;
    counters_def[bcm_dpp_counter_drop_pkts]          = counters_def[bcm_dpp_counter_drop_bytes]           = GREEN_DROP | YELLOW_DROP | RED_DROP | BLACK_DROP;
    counters_def[bcm_dpp_counter_drop_green_pkts]    = counters_def[bcm_dpp_counter_drop_green_bytes]     = GREEN_DROP;
    counters_def[bcm_dpp_counter_drop_not_green_pkts]= counters_def[bcm_dpp_counter_drop_not_green_bytes] = YELLOW_DROP | RED_DROP | BLACK_DROP;
    counters_def[bcm_dpp_counter_drop_yellow_pkts]   = counters_def[bcm_dpp_counter_drop_yellow_bytes]    = YELLOW_DROP | RED_DROP;
    counters_def[bcm_dpp_counter_drop_not_yellow_pkts]= counters_def[bcm_dpp_counter_drop_not_yellow_bytes]= GREEN_DROP | BLACK_DROP;
    counters_def[bcm_dpp_counter_drop_red_pkts]      = counters_def[bcm_dpp_counter_drop_red_bytes]       = BLACK_DROP;
    counters_def[bcm_dpp_counter_drop_not_red_pkts]  = counters_def[bcm_dpp_counter_drop_not_red_bytes]   = GREEN_DROP | YELLOW_DROP | RED_DROP;
}


int
_bcm_dpp_counters_is_byte_format_get(bcm_dpp_counter_t counter, uint8 *is_byte_format){
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
    switch (counter){
    case bcm_dpp_counter_pkts:
    case bcm_dpp_counter_green_pkts:
    case bcm_dpp_counter_not_green_pkts:
    case bcm_dpp_counter_yellow_pkts:
    case bcm_dpp_counter_not_yellow_pkts:
    case bcm_dpp_counter_red_pkts:
    case bcm_dpp_counter_not_red_pkts:
    case bcm_dpp_counter_fwd_pkts:
    case bcm_dpp_counter_fwd_green_pkts:
    case bcm_dpp_counter_fwd_not_green_pkts:
    case bcm_dpp_counter_fwd_yellow_pkts:
    case bcm_dpp_counter_fwd_not_yellow_pkts:
    case bcm_dpp_counter_fwd_red_pkts:
    case bcm_dpp_counter_fwd_not_red_pkts:
    case bcm_dpp_counter_drop_pkts:
    case bcm_dpp_counter_drop_green_pkts:
    case bcm_dpp_counter_drop_not_green_pkts:
    case bcm_dpp_counter_drop_yellow_pkts:
    case bcm_dpp_counter_drop_not_yellow_pkts:
    case bcm_dpp_counter_drop_red_pkts:
    case bcm_dpp_counter_drop_not_red_pkts:
    case bcm_dpp_counter_offset0_pkts:
    case bcm_dpp_counter_offset1_pkts:
    case bcm_dpp_counter_offset2_pkts:
    case bcm_dpp_counter_offset3_pkts:
    case bcm_dpp_counter_offset4_pkts:
    case bcm_dpp_counter_offset5_pkts:
    case bcm_dpp_counter_offset6_pkts:
    case bcm_dpp_counter_offset7_pkts:
        *is_byte_format = FALSE;
        break;
    case bcm_dpp_counter_bytes:
    case bcm_dpp_counter_green_bytes:
    case bcm_dpp_counter_not_green_bytes:
    case bcm_dpp_counter_yellow_bytes:
    case bcm_dpp_counter_not_yellow_bytes:
    case bcm_dpp_counter_red_bytes:
    case bcm_dpp_counter_not_red_bytes:
    case bcm_dpp_counter_fwd_bytes:
    case bcm_dpp_counter_fwd_green_bytes:
    case bcm_dpp_counter_fwd_not_green_bytes:
    case bcm_dpp_counter_fwd_yellow_bytes:
    case bcm_dpp_counter_fwd_not_yellow_bytes:
    case bcm_dpp_counter_fwd_red_bytes:
    case bcm_dpp_counter_fwd_not_red_bytes:
    case bcm_dpp_counter_drop_bytes:
    case bcm_dpp_counter_drop_green_bytes:
    case bcm_dpp_counter_drop_not_green_bytes:
    case bcm_dpp_counter_drop_yellow_bytes:
    case bcm_dpp_counter_drop_not_yellow_bytes:
    case bcm_dpp_counter_drop_red_bytes:
    case bcm_dpp_counter_drop_not_red_bytes:
    case bcm_dpp_counter_offset0_bytes:
    case bcm_dpp_counter_offset1_bytes:
    case bcm_dpp_counter_offset2_bytes:
    case bcm_dpp_counter_offset3_bytes:
    case bcm_dpp_counter_offset4_bytes:
    case bcm_dpp_counter_offset5_bytes:
    case bcm_dpp_counter_offset6_bytes:
    case bcm_dpp_counter_offset7_bytes:
        *is_byte_format = TRUE;
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG_NO_UNIT("format %d not supported "), counter));
    }
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 *   Name
 *     _bcm_dpp_counters_calc_unavail_formats
 *   Purpose
 *     get list of unavailable formats.
 *   Arguments
 *     IN nof_entries the number of counters
 *     IN entries_bmaps - bmap by color and drop/fwd for each counter
 *     OUT unavail_counters - the counters we cant count
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 *   Notes:
 *   We assume that each dp_fwd/drop counter can be counted in one entry.
 *   From that assumption we define conflict counters:
 *   counter that contains part of the counter definition but also contains bits that are not part of it
 */
void
_bcm_dpp_counters_calc_unavail_formats(
   SOC_TMC_CNT_SRC_TYPE src_type, 
   SOC_TMC_CNT_MODE_EG_TYPE cnt_mode, 
   uint32 nof_entries, 
   uint32 *entries_bmaps, 
   bcm_dpp_counter_set_t unavail_counters){
    uint32 i, j, counter_entry[1], do_not_count_entry[1], tmp_counter_bmap = 0;
    
    SHR_BITCLR_RANGE(unavail_counters, 0, bcm_dpp_counter_count);
    /*invalid offset based counters*/
    for(i = 0 ; i < bcm_dpp_counter_count; i++) {
        if((IS_OFFSET_BASED_COUNTER(i)) && (OFFSET_BASED_COUNTER_OFFSET_GET(i) >= nof_entries)){
            SHR_BITSET(unavail_counters, i);
        }
    }
    /*look for conflicts*/
    for(i = 0 ; i < nof_entries ; i++) {
        if(entries_bmaps[i] == 0) { /*possible just in configurable mode*/
            continue;
        }
        *counter_entry = entries_bmaps[i];
        for(j = 0 ; j < bcm_dpp_counter_count; j++){
            if((IS_OFFSET_BASED_COUNTER(j)) || /*Offset based counters were removed above*/
               (*counter_entry == counters_def[j]) || /*Entry is supported*/
               (SOC_TMC_CNT_SRC_IS_EGRESS_TM(src_type, cnt_mode)) /*Egress-TM src-support geting color using the counter pointer*/) {
                continue;
            }
            SHR_BITNEGATE_RANGE(&counters_def[j], 0, SOC_TMC_CNT_BMAP_OFFSET_COUNT, &tmp_counter_bmap);
            /*check if there is conflict between the entry and the counter definition. see the function notes*/
            if(((*counter_entry & counters_def[j]) != 0) && ((*counter_entry & tmp_counter_bmap) != 0)){
                SHR_BITSET(unavail_counters, j);
            }
        }
    }
    /*remove counters that we don't count at all*/
    *counter_entry = 0;
    for(i = 0 ; i < nof_entries ; i++){
        *counter_entry |= entries_bmaps[i];
    }
    SHR_BITNEGATE_RANGE(counter_entry, 0, SOC_TMC_CNT_BMAP_OFFSET_COUNT, do_not_count_entry);
    for(i = 0 ; i < bcm_dpp_counter_count; i++){
        if((!IS_OFFSET_BASED_COUNTER(i)) && ((counters_def[i] & *do_not_count_entry) != 0)){
            SHR_BITSET(unavail_counters, i);
        }
    }
}


/*
 *   Name
 *     _bcm_dpp_counters_calc_formats
 *   Purpose
 *     calc the native and the available counters without considering the counting format mode(Packets , Bytes, etc...)
 *   Arguments
 *     IN nof_entries the number of counters
 *     IN entries_bmaps - bmap by color and drop/fwd for each counter
 *     OUT native_formats - bmap that the i bit represent whether the i counter is native or not
 *     OUT avail_offsets - array if the value is not UNSUPPORTED_COUNTER_OFFSET. its an available counter and we will use the value
 *     to read/write the counter value to the processor counters array.
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 *   Notes
 *   Doesn't support packets and packets because the entries bmaps are not defined well for this format
 */
int
_bcm_dpp_counters_calc_formats(
   SOC_TMC_CNT_SRC_TYPE src_type, 
   SOC_TMC_CNT_MODE_EG_TYPE cnt_mode, 
   uint32 nof_entries, 
   uint32 *entries_bmaps,
   bcm_dpp_counter_set_t native_formats, 
   uint32 *avail_offsets)
{
    uint32 i, j;
    bcm_dpp_counter_set_t unavail_formats;
    uint8 is_bytes_format;
    int rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    _bcm_dpp_counters_calc_unavail_formats(src_type, cnt_mode, nof_entries, entries_bmaps, unavail_formats);
    /*calc the offsets*/
    for(j = 0; j < bcm_dpp_counter_count ; j++){
        if(SHR_BITGET(unavail_formats, j)){
            continue;
        }
        rv = _bcm_dpp_counters_is_byte_format_get(j, &is_bytes_format);
        BCMDNX_IF_ERR_EXIT(rv);
        avail_offsets[j] = 0;
        for(i = 0 ; i < nof_entries; i++){
            /*native format*/
            if(entries_bmaps[i] == counters_def[j] || ((IS_OFFSET_BASED_COUNTER(j))&& (OFFSET_BASED_COUNTER_OFFSET_GET(j) == i))) {
                if(is_bytes_format){
                    avail_offsets[j] = i*_SUB_COUNT + _SUB_BYTES;
                }
                else{
                    avail_offsets[j] = i*_SUB_COUNT + _SUB_PKTS;
                }
                SHR_BITSET(native_formats, j);
                continue;
            }
            /*if the entry is not part of the counter*/
            /* j is limited to bcm_dpp_counter_count - 1 according to the for look. The nof entries of counters_def is counters_def - no overrun*/
            /* coverity[overrun-local:FALSE] */
            if((entries_bmaps[i] & counters_def[j]) == 0){
                continue;
            }
            /*avail format- we use bmap*/
            if(is_bytes_format){
                 SHR_BITSET(&avail_offsets[j], (_SUB_COUNT*i + _SUB_BYTES));
            } else {
                 SHR_BITSET(&avail_offsets[j], (_SUB_COUNT*i + _SUB_PKTS));
            }
        }
    }
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 *   Name
 *     _bcm_dpp_counter_fifo_process
 *   Purpose
 *     Process the 'algorithmic' FIFO to adjust the cache accordingly
 *   Arguments
 *     IN/OUT _bcm_dpp_counter_state_t *unitData = info for the unit
 *     OUT unsigned int *depth = where to put greatest FIFO depth this pass
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 *   Notes
 *     This is called by both the background task and the demand access
 *     functions, so it does not itself handle the locking protocol.
 *
 *     Because the calls to soc_pb_cnt_algorithmic_read from the background process
 *     and API caller may collide, or may collide with the direct read, we
 *     don't do any background updates while performing updates in preparation
 *     for foreground access. This means that in both cases, we need to run the
 *     FIFOs on all of the counter processors.
 */

STATIC int
_bcm_dpp_counter_fifo_process(_bcm_dpp_counter_state_t *unitData,
                              unsigned int proc_mask,
                              unsigned int *depth)
{
    uint32 soc_sandResult;
    unsigned int index;
    unsigned int ofs;
    unsigned int proc;
    unsigned int ctrs = 0;
    SOC_TMC_CNT_RESULT_ARR *data_p = NULL;
    int unit = unitData->cunit;
    int result = BCM_E_NONE;
    int fifo_dma_index = 0, fifo_dma_index_old = -1, cache_length;
    uint32 fifo_dma_offset;
    uint32 engine_id, counter_id;
    int nof_procs; /* processor descriptor */
    SOC_TMC_CNT_COUNTERS_INFO *counter_info_array = NULL;

#if _SATURATE_INSTEAD_OF_OVERFLOW
    uint64 oldval;
#endif /* _SATURATE_INSTEAD_OF_OVERFLOW */
    uint64 counter;
    _bcm_dpp_counter_proc_info_t *info, *info_array;
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

    nof_procs = (SOC_DPP_DEFS_GET(unit, nof_counter_processors) + SOC_DPP_DEFS_GET(unit, nof_small_counter_processors)); /* processor descriptor */
    BCMDNX_ALLOC(counter_info_array, (sizeof(SOC_TMC_CNT_COUNTERS_INFO) * nof_procs), "DPP prcc counter info array");
    if (!counter_info_array) {
        BCMDNX_IF_ERR_EXIT(BCM_E_MEMORY);
    }
    /*build info array*/
    sal_memset(counter_info_array, 0x0, sizeof(*counter_info_array) * nof_procs);
    for (proc = 0; proc < unitData->num_counter_procs && proc < nof_procs; proc++) {
        sal_memcpy(&(counter_info_array[proc]), &(unitData->proc[proc].mode), sizeof(unitData->proc[proc].mode));
    }

    data_p = &_fifo_results[unit];
    for (proc = 0; proc < unitData->num_counter_procs; proc++) {
        if(0 == (proc_mask & (1 << proc))) {
           /* if not in the mask skip*/
           continue;
        }

        info = &(unitData->proc[proc]);
        if (!(info->valid & BCM_DPP_COUNTER_PROC_INFO_VALID)) {
            continue;
        }
        info_array = unitData->proc; 

        /* if wide mode do not collect from the DMA*/
        if(SOC_TMC_CNT_FORMAT_IS_WIDE(info->mode.format, info->mode.src_type))
        {
            continue;
        }
        SOCDNX_SAND_IF_ERR_RETURN(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_cnt_engine_to_fifo_dma_index, (unit, info->proc_id, &fifo_dma_index, &cache_length)));
        if (fifo_dma_index == fifo_dma_index_old) {
            /*allready read this procs data*/
            continue;
        } 
        if (glb_bg_thread_and_dma_data[unit].dma_channel[fifo_dma_index] == SOC_TMC_CNT_INVALID_DMA_CHANNEL) {
            continue; /*No DMA attached*/
        }
        fifo_dma_index_old = fifo_dma_index;
        SOCDNX_SAND_IF_ERR_RETURN(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_cnt_fifo_dma_offset_in_engine, (unit, info->proc_id, &fifo_dma_offset)));


        /* prep */
        SOC_TMC_CNT_RESULT_ARR_clear(data_p);
        /* get the data from the algorithmic read FIFO */
        soc_sandResult = (MBCM_DPP_DRIVER_CALL((unit), mbcm_dpp_cnt_algorithmic_read, (unit, fifo_dma_index, glb_bg_thread_and_dma_data[unit].dma_channel[fifo_dma_index], counter_info_array, data_p)));
        result = handle_sand_result(soc_sandResult);

        BCM_DPP_UNIT_CHECK(unit);
        if (result != BCM_E_NONE) {
            /* accounting -- read pass failed */
            unitData->fifo[fifo_dma_index].fifo_read_fails++;
            BCMDNX_IF_ERR_EXIT(result);
        } 

        /* accounting -- read pass successful */
        unitData->fifo[fifo_dma_index].fifo_read_passes++;
        if (data_p->nof_counters > unitData->fifo[fifo_dma_index].fifo_read_max) {
            unitData->fifo[fifo_dma_index].fifo_read_max = data_p->nof_counters;
        }

        /* accounting -- largest FIFO read set this time */
        if (data_p->nof_counters > ctrs) {
            ctrs = data_p->nof_counters;
        }
        
        /* while there are counters to update, update them */
        for (index = 0; index < data_p->nof_counters; index++) {
            counter_id = data_p->cnt_result[index].counter_id;
            engine_id = data_p->cnt_result[index].engine_id;
            ofs = counter_id * _SUB_COUNT;
            unitData->fifo[fifo_dma_index].fifo_read_last = counter_id;
            unitData->fifo[fifo_dma_index].fifo_read_items++;
            info_array[engine_id].diag.cache_updates++;
#if _SATURATE_INSTEAD_OF_OVERFLOW
            result = COUNTERS_ACCESS.counter.get(unit, engine_id, ofs + _SUB_PKTS, &oldval);
            BCMDNX_IF_ERR_EXIT(result);
            COMPILER_64_ZERO(counter);
            COMPILER_64_ADD_64(counter, oldval);
            COMPILER_64_ADD_64(counter, data_p->cnt_result[index].pkt_cnt);
            if (COMPILER_64_LT(counter, oldval)) {
                /* dont allow wrap; saturate instead */
                COMPILER_64_ALLONES(counter);
            }
            result = COUNTERS_ACCESS.counter.set(unit, engine_id, ofs + _SUB_PKTS, counter);
            BCMDNX_IF_ERR_EXIT(result);

            result = COUNTERS_ACCESS.counter.get(unit, engine_id, ofs + _SUB_BYTES, &oldval);
            BCMDNX_IF_ERR_EXIT(result);
            COMPILER_64_ZERO(counter);
            COMPILER_64_ADD_64(counter, oldval);
            COMPILER_64_ADD_64(counter, data_p->cnt_result[index].byte_cnt);
            if (COMPILER_64_LT(counter, oldval)) {
                /* dont allow wrap; saturate instead */
                COMPILER_64_ALLONES(counter);
            }
            result = COUNTERS_ACCESS.counter.set(unit, engine_id, ofs + _SUB_BYTES, counter);
            BCMDNX_IF_ERR_EXIT(result);

#else /* _SATURATE_INSTEAD_OF_OVERFLOW */
            result = COUNTERS_ACCESS.counter.get(unit, engine_id, ofs + _SUB_PKTS, &counter);
            BCMDNX_IF_ERR_EXIT(result);

            COMPILER_64_ADD_64(counter, data_p->cnt_result[index].pkt_cnt);

            result = COUNTERS_ACCESS.counter.set(unit, engine_id, ofs + _SUB_PKTS, counter);
            BCMDNX_IF_ERR_EXIT(result);

            result = COUNTERS_ACCESS.counter.get(unit, engine_id, ofs + _SUB_BYTES, &counter);
            BCMDNX_IF_ERR_EXIT(result);

            COMPILER_64_ADD_64(counter, data_p->cnt_result[index].byte_cnt);

            result = COUNTERS_ACCESS.counter.set(unit, engine_id, ofs + _SUB_BYTES, counter);
            BCMDNX_IF_ERR_EXIT(result);
#endif /* _SATURATE_INSTEAD_OF_OVERFLOW */
            /*update counters*/
        }
    } /* for (proc = 0; proc < unitData->num_counter_procs; proc++) */
    if (depth) {
        *depth = ctrs;
    }
    BCMDNX_IF_ERR_EXIT(result);
exit:
    if (counter_info_array) {
        BCM_FREE(counter_info_array);
    }
    BCMDNX_FUNC_RETURN;
}

/*
 *   Name
 *     _bcm_dpp_counter_set_process
 *   Purpose
 *     Ensure specific counters are updated before they are accessed by
 *     updating the specific counters.
 *   Arguments
 *     IN/OUT _bcm_dpp_counter_state_t *unitData = info for the unit
 *     IN unsigned int proc = the counter processor on which to operate
 *     IN unsigned int index = the index of the first counter to process
 *     IN unsigned int num = the number of counters to process
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 *   Notes
 *     This is called when about to read or write a counter.  It ensures the
 *     cache is up-to-date with the current value, so that either small
 *     differences can be seen, or so that sets apply to the moment of the
 *     request rather than some unknown point in time before the request.
 */
STATIC int
_bcm_dpp_counter_set_process(_bcm_dpp_counter_state_t *unitData,
                             unsigned int procBase,
                             unsigned int index,
                             unsigned int num,
                             unsigned int * finalProc)
{
    uint32 soc_sandResult;
    SOC_TMC_CNT_RESULT data;
    unsigned int ofs;
    int unit = unitData->cunit;
    int result = BCM_E_NONE;
    unsigned int proc = procBase;
#if _SATURATE_INSTEAD_OF_OVERFLOW
    uint64 oldval;
#endif /* _SATURATE_INSTEAD_OF_OVERFLOW */
    _bcm_dpp_counter_proc_info_t *info;
    uint64 counter;
    BCMDNX_INIT_FUNC_DEFS;
    while (num > 0) {
        result = _bcm_dpp_modify_proc_and_index(unitData, &proc, &index, FALSE);
        if (result != BCM_E_NONE) {
            BCMDNX_IF_ERR_EXIT(result);
        }
        info = &(unitData->proc[proc]);
        if (unitData->num_counter_procs <= proc){
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d invalid counter processor ID %u"), unit, proc));
        }

        soc_sandResult = (MBCM_DPP_DRIVER_CALL((unit), mbcm_dpp_cnt_direct_read, (unit, info->proc_id, &(info->mode), index, &data)));
           result = handle_sand_result(soc_sandResult);
        if (result != BCM_E_NONE) {
            /* accounting -- read pass failure */
            info->diag.direct_read_fails++;
            BCMDNX_IF_ERR_EXIT(result);
        }
        /* accounting -- read pass successful */
        info->diag.direct_read_passes++;
        info->diag.cache_updates++;
        /* update this counter */
        ofs = index * _SUB_COUNT;

#if _SATURATE_INSTEAD_OF_OVERFLOW
        if (info->mode.format == SOC_TMC_CNT_FORMAT_MAX_QUEUE_SIZE) {
            /* The MBCM_DPP_DRIVER_CALL initializing the required varible */ 
            /* coverity[uninit_use:FALSE] */
            /* coverity[overrun-local:FALSE] */
            result = COUNTERS_ACCESS.counter.get(unit, proc, ofs + _SUB_BYTES, &counter);
            BCMDNX_IF_ERR_EXIT(result);
            if (COMPILER_64_LT(counter, data.byte_cnt)) {
                /* 
                 * For Max-Queue-Size, returns always the absolute maximum. 
                 * If needed, the user can zero the counter 
                 */ 
                result = COUNTERS_ACCESS.counter.set(unit, proc, ofs + _SUB_BYTES, data.byte_cnt);
                BCMDNX_IF_ERR_EXIT(result);
            }
        }
        else if (info->mode.format == SOC_TMC_CNT_FORMAT_IPT_LATENCY){
            result = COUNTERS_ACCESS.counter.get(unit, proc, ofs + _SUB_PKTS, &counter);
            BCMDNX_IF_ERR_EXIT(result);
            if (COMPILER_64_LT(counter, data.pkt_cnt)) {
               /* 
                         * For Max-latency, returns always the absolute maximum. 
                         * If needed, the user can zero the counter 
                         */ 
                result = COUNTERS_ACCESS.counter.set(unit, proc, ofs + _SUB_PKTS, data.pkt_cnt);
                BCMDNX_IF_ERR_EXIT(result);
            }
        }
        else {
            result = COUNTERS_ACCESS.counter.get(unit, proc, ofs + _SUB_PKTS, &oldval);
            BCMDNX_IF_ERR_EXIT(result);
            result = COUNTERS_ACCESS.counter.get(unit, proc, ofs + _SUB_PKTS, &counter);
            BCMDNX_IF_ERR_EXIT(result);
            /* The MBCM_DPP_DRIVER_CALL initializing the required varible */ 
            /* coverity[uninit_use:FALSE] */
            COMPILER_64_ADD_64(counter, data.pkt_cnt);
            if (COMPILER_64_LT(counter, oldval)) {
                /* don't allow wrap; saturate instead */
                COMPILER_64_ALLONES(counter);
            }
            result = COUNTERS_ACCESS.counter.set(unit, proc, ofs + _SUB_PKTS, counter);
            BCMDNX_IF_ERR_EXIT(result);

            result = COUNTERS_ACCESS.counter.get(unit, proc, ofs + _SUB_BYTES, &oldval);
            BCMDNX_IF_ERR_EXIT(result);
            result = COUNTERS_ACCESS.counter.get(unit, proc, ofs + _SUB_BYTES, &counter);
            BCMDNX_IF_ERR_EXIT(result);
            /* The MBCM_DPP_DRIVER_CALL initializing the required varible */ 
            /* coverity[uninit_use:FALSE] */
            COMPILER_64_ADD_64(counter, data.byte_cnt);
            if (COMPILER_64_LT(counter, oldval)) {
                /* don't allow wrap; saturate instead */
                COMPILER_64_ALLONES(counter);
            }
            result = COUNTERS_ACCESS.counter.set(unit, proc, ofs + _SUB_BYTES, counter);
            BCMDNX_IF_ERR_EXIT(result);
        }
#else /* _SATURATE_INSTEAD_OF_OVERFLOW */
       if (info->mode.format == SOC_TMC_CNT_FORMAT_MAX_QUEUE_SIZE){
           result = COUNTERS_ACCESS.counter.get(unit, proc, ofs + _SUB_BYTES, &counter);
           BCMDNX_IF_ERR_EXIT(result);
           if (COMPILER_64_LT(counter, data.byte_cnt)) {
               /* 
                * For Max-Queue-Size, returns always the absolute maximum. 
                * If needed, the user can zero the counter 
                */ 
               result = COUNTERS_ACCESS.counter.set(unit, proc, ofs + _SUB_BYTES, new_val);
               BCMDNX_IF_ERR_EXIT(result);
           }
        }
        else if (info->mode.format == SOC_TMC_CNT_FORMAT_IPT_LATENCY){
            result = COUNTERS_ACCESS.counter.get(unit, proc, ofs + _SUB_PKTS, &counter);
            BCMDNX_IF_ERR_EXIT(result);
            if (COMPILER_64_LT(counter, data.pkt_cnt)) {
               /* 
                         * For Max-latency, returns always the absolute maximum. 
                         * If needed, the user can zero the counter 
                         */ 
                result = COUNTERS_ACCESS.counter.set(unit, proc, ofs + _SUB_PKTS, data.pkt_cnt);
                BCMDNX_IF_ERR_EXIT(result);
            }          
       } 
       else {
           result = COUNTERS_ACCESS.counter.set(unit, proc, ofs + _SUB_PKTS, data.pkt_cnt);
           BCMDNX_IF_ERR_EXIT(result);
           result = COUNTERS_ACCESS.counter.set(unit, proc, ofs + _SUB_BYTES, data.byte_cnt);

           BCMDNX_IF_ERR_EXIT(result);
       }

#endif /* _SATURATE_INSTEAD_OF_OVERFLOW */

        num--;
        index++;
    } /* while (num > 0) */
    BCMDNX_IF_ERR_EXIT(result);
    *finalProc = proc;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *   Name
 *     _bcm_dpp_counter_bg
 *   Purpose
 *     Background handler (one thread per unit)
 *   Arguments
 *     IN/OUT _bcm_dpp_counter_state_t *unitData = info for the unit
 *   Returns
 *     nothing
 *   Notes
 *     This sets unitData->running to TRUE to indicate it has started.
 *
 *     This thread will terminate itself and free the unit descriptor during
 *     the next iteration if unitData->running is set to FALSE.  This allows
 *     the destroy function to safely shut down the background thread by simply
 *     detaching it from the globals and setting the running flag to FALSE.
 */
void _bcm_dpp_counter_bg(void *unitPtr) {
    _bcm_dpp_counter_state_t *unitData = (_bcm_dpp_counter_state_t*) unitPtr;
    unsigned int depth;
    int semTaken = FALSE;
    int mutexTaken;
    unsigned int proc_mask = 0;
    int rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
    if (unitData) {
        unit = unitData->cunit;

        rv = _bcm_dpp_counter_unit_check_and_fill(unit, &unitData, COUNTER_UPDATE_SELECT_ALL, NULL);
        BCMDNX_IF_ERR_EXIT(rv);
        /* mark unit as running */

        glb_bg_thread_and_dma_data[unit].running = TRUE;        
        /* loop until told to go away */
        while (glb_bg_thread_and_dma_data[unit].running) {
            /*
             *  This should wait for the specified interval and then give up,
             *  but we don't care whether it gets triggered or it gives up, so
             *  just discard the result deliberately.  Note that this mechanism
             *  can also be used to force an immediate trigger, such as when
             *  destroying a unit.
             */
            semTaken = (!sal_sem_take(COUNTERS_SYNC_ACCESS.bgSem, glb_bg_thread_and_dma_data[unit].bgWait));
            /* indicate background process is active */
            unitData->background_active = TRUE;
            
            if ((!unitData->diag.fifo_read_deferred) && (!unitData->background_disable)) {
                /* background processing is not deferred or disabled */
                /* interlock using mutex instead of spin loops */
                mutexTaken = (0 == sal_mutex_take(COUNTERS_SYNC_ACCESS.cacheLock, sal_mutex_FOREVER));
                if (!mutexTaken) {
                    /* background processing is deferred */
                    /* accounting -- background FIFO deferral */
                    unitData->diag.fifo_read_deferred++;
                    unitData->background_active = FALSE;

                    rv = _bcm_dpp_counter_unit_select_update(unit, COUNTER_UPDATE_SELECT_DIAGS | COUNTER_UPDATE_SELECT_BACKROUND_INFO, unitData, NULL);
                    BCMDNX_IF_ERR_EXIT(rv);
                    continue;
                }
                /* accounting -- background FIFO read attempt */
                unitData->diag.fifo_read_background++;
                /* process the FIFO */
                /* process for all processors*/
                SHR_BITSET_RANGE(&proc_mask, 0, unitData->num_counter_procs);
                rv = _bcm_dpp_counter_fifo_process(unitData, proc_mask, &depth);
                if(rv != BCM_E_NONE) {
                    LOG_ERROR(BSL_LS_BCM_CNT,
                              (BSL_META_U(unit,
                                          "unit %d error processing counter fifo:  _bcm_dpp_counter_fifo_process \n"),
                               unit));
                }
                /*
                 *  Automatically adjust delay based upon how busy.  Tries to
                 *  reduce this thread's overhead on the system while avoiding
                 *  conditions that result in missed counter updates.
                 *
                 *  Basically, hardware docs say should read every 100ms or so
                 *  as long as we process 10 counters per FIFO read.  This
                 *  thread prefers to keep it to fewer than 8 counters per FIFO
                 *  read and so will back out to 100ms if it can, but if it
                 *  starts to get more, will reduce update delay until it hits
                 *  40ms or 8 counters per FIFO.
                 *
                 *  Note that if a foreground read occurred, it is probable that
                 *  it occurred since the last time based poll, and will
                 *  therefore skew the depth downward.  We will ignore the
                 *  adjustment to lengthen the interval in this case, to avoid
                 *  falsely making the interval longer.
                 */
                if (!(unitData->foreground_hit)) {
                    if ((_FIFO_DEPTH_MIN > depth) &&
                        (_BACKGROUND_THREAD_ITER_MAX(unit) > glb_bg_thread_and_dma_data[unit].bgWait)) {
                        glb_bg_thread_and_dma_data[unit].bgWait += _BACKGROUND_THREAD_ITER_ADJUST(unit);
                        LOG_DEBUG(BSL_LS_BCM_CNT, (BSL_META_U(unit,"%s unit %d counter background thread delay to %dus\n"),
                                   "increasing",
                                   unit,
                                   glb_bg_thread_and_dma_data[unit].bgWait));
                    }
                }
                if ((_FIFO_DEPTH_MAX(unit) < depth) &&
                    (_BACKGROUND_THREAD_ITER_MIN < glb_bg_thread_and_dma_data[unit].bgWait)) {
                    glb_bg_thread_and_dma_data[unit].bgWait = _BACKGROUND_THREAD_ITER_MIN;
                    LOG_DEBUG(BSL_LS_BCM_CNT, (BSL_META_U(unit, "%s unit %d counter background thread delay to %dus\n"),
                               "decreasing",
                               unit,
                               glb_bg_thread_and_dma_data[unit].bgWait));
                } else if ((_FIFO_DEPTH_PREF(unit) < depth) &&
                    (_BACKGROUND_THREAD_ITER_MIN < glb_bg_thread_and_dma_data[unit].bgWait)) {
                    glb_bg_thread_and_dma_data[unit].bgWait -= _BACKGROUND_THREAD_ITER_ADJUST(unit);
                    LOG_DEBUG(BSL_LS_BCM_CNT, (BSL_META_U(unit, "%s unit %d counter background thread delay to %dus\n"),
                               "decreasing",
                               unit,
                               glb_bg_thread_and_dma_data[unit].bgWait));
                }
                /* no foreground hit since this FIFO run (yet) */
                unitData->foreground_hit = FALSE;
                /* allow foreground access again */
                sal_mutex_give(COUNTERS_SYNC_ACCESS.cacheLock);
            } else { /* if (!(unitData->background_defer)) */
                /* background processing is deferred */
                /* accounting -- background FIFO deferral */
                unitData->diag.fifo_read_deferred++;
            } /* if (!(unitData->background_defer)) */
            unitData->background_active = FALSE;
            rv = _bcm_dpp_counter_unit_select_update(unit, COUNTER_UPDATE_SELECT_DIAGS | COUNTER_UPDATE_SELECT_BACKROUND_INFO, unitData, NULL);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* dispose of the unit information */
        if (semTaken) {
            /*
             *  If we have it, then release it.  If not, nobody should have it
             *  now since it was released by the detach code, which is also
             *  where running was set to FALSE.
             */
            sal_sem_give(COUNTERS_SYNC_ACCESS.bgSem);
        }

        rv = _bcm_dpp_counter_unit_update(unit, unitData, COUNTER_UPDATE_SELECT_ALL, NULL);
        BCMDNX_IF_ERR_EXIT(rv);
    }
exit:
    if(unitData) {
        _bcm_counter_thread_is_running[unit] = FALSE;
    }
    BCMDNX_FUNC_RETURN_VOID_NO_UNIT;
}

/*
 *   Name
 *     _bcm_dpp_counter_update_process
 *   Purpose
 *     Prepare for application access to a specific counter set by ensuring the
 *     specific counters are updated in the cache, and running the FIFO, to
 *     ensure that none of the specific counters were in there.
 *   Arguments
 *     IN/OUT _bcm_dpp_counter_state_t *unitData = info for the unit
 *     IN unsigned int proc = the counter processor on which to operate
 *     IN unsigned int index = the index of the first counter to process
 *     IN unsigned int num = the number of counters to process
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 *   Notes
 *     This must be done before both read and set, in order to ensure that the
 *     operation applies at the time of the call rather than to some point in
 *     time an unknown interval before the call.
 *
 *     This handles the protocol between demand access and the background
 *     update thread.
 */
STATIC int
_bcm_dpp_counter_update_process(_bcm_dpp_counter_state_t *unitData,
                                unsigned int proc,
                                unsigned int index,
                                unsigned int num)
{
    int unit = unitData->cunit;
    int result;
    unsigned int finalProc = proc;
    int lock_taken = FALSE;
    BCMDNX_INIT_FUNC_DEFS;
    /* request background process deferral */
    unitData->background_defer = TRUE;

    result = _bcm_dpp_counter_unit_select_update(unit, COUNTER_UPDATE_SELECT_BACKROUND_INFO, unitData, NULL);
    BCMDNX_IF_ERR_EXIT(result);

    result = bcm_dpp_counter_state_lock_take(unit);
    BCMDNX_IF_ERR_EXIT(result);
    lock_taken = TRUE;
    /* background process is deferred now; process the counters */
    result = _bcm_dpp_counter_set_process(unitData, proc, index, num, &finalProc);
    if (BCM_E_NONE == result) {
        /* successfully processed the counter set; process the FIFO */
        result = _bcm_dpp_counter_fifo_process(unitData, (1 << proc), NULL);
        /*case that two procs were process from FIFO, need to collect also the secon proc (called finalProc)*/
        if (finalProc != proc)
        {
            result = _bcm_dpp_counter_fifo_process(unitData, (1 << finalProc), NULL);
        }
        BCMDNX_IF_ERR_EXIT(result);
        unitData->foreground_hit = TRUE;    
    }
    result = bcm_dpp_counter_state_lock_give(unit);
    BCMDNX_IF_ERR_EXIT(result);        
    lock_taken = FALSE;  
    /* allow background process to resume */
    unitData->background_defer = FALSE;
    BCMDNX_IF_ERR_EXIT(result);

    result = _bcm_dpp_counter_unit_select_update(unit, COUNTER_UPDATE_SELECT_BACKROUND_INFO, unitData, NULL);
    BCMDNX_IF_ERR_EXIT(result);
    
exit:
    if(lock_taken == TRUE)
    {
        bcm_dpp_counter_state_lock_give(unit);
    }
    BCMDNX_FUNC_RETURN;
}

/*
 *   Name
 *     _bcm_dpp_counter_single_emulated
 *   Purpose
 *     Manipulate a set of sub-counters to, in effect, manipulate either a
 *     native or emulated set of counters.
 *   Arguments
 *     IN/OUT _bcm_dpp_counter_state_t *unitData = info for the unit
 *     IN unsigned int proc = the counter processor on which to operate
 *     IN unsigned int index = the base counter of the set on which to operate
 *     IN uint32 offsets = bitmap of participating sub-counter offsets
 *     IN int update = FALSE to read, TRUE to write
 *     IN/OUT uint64 counter = where to put/get value
 *   Returns
 *     no return value
 *   Notes
 *     If emulating writes, it writes the entire value to the first sub counter
 *     it encounters, and then zeroes the rest of them.  This probably has the
 *     desired effect only in the case of zeroing an emulated counter, so it is
 *     best to only write to known 'native' counters.
 *
 *     If emulating reads, it will saturate rather than overflow when the total
 *     would not fit in 64b.
 *
 *     This is not a particularly efficient way to deal with native counters.
 */
STATIC int
_bcm_dpp_counter_single_emulated(_bcm_dpp_counter_state_t *unitData,
                                 unsigned int procBase,
                                 unsigned int index,
                                 uint32 offsets,
                                 int update,
                                 uint64 *counter)
{
    int result = BCM_E_NONE;
    int unit = unitData->cunit;
    uint64 accum;
    unsigned int proc = procBase;

#if _SATURATE_INSTEAD_OF_OVERFLOW
    uint64 prev;
#endif /* _SATURATE_INSTEAD_OF_OVERFLOW */
    unsigned int ofs, finalIndex;
    int done;
    uint64 counter_accum;
    BCMDNX_INIT_FUNC_DEFS;
    if (update) {
        done = FALSE;
        for (ofs = 0; ofs < _OFS_COUNT; ofs++) {
            if (offsets & (1 << ofs)) {
                if (done) {
                    COMPILER_64_ZERO(counter_accum);
                    finalIndex = index + ofs;
                    proc = procBase;
                    result = _bcm_dpp_modify_proc_and_index(unitData, &proc, &finalIndex, TRUE);
                    BCMDNX_IF_ERR_EXIT(result);
                    result = COUNTERS_ACCESS.counter.set(unit, proc, finalIndex, counter_accum);
                    BCMDNX_IF_ERR_EXIT(result);                   
                } else {
                    finalIndex = index + ofs;
                    proc = procBase;
                    result = _bcm_dpp_modify_proc_and_index(unitData, &proc, &finalIndex, TRUE);
                    BCMDNX_IF_ERR_EXIT(result);
                    result = COUNTERS_ACCESS.counter.set(unit, proc, finalIndex, *counter);
                    BCMDNX_IF_ERR_EXIT(result);                   
                    done = TRUE;
                }
                unitData->proc[proc].diag.cache_writes++;
            } /* if (offsets & (1 << ofs)) */
        } /* for (ofs = 0; ofs <= _OFS_MAX; ofs++) */
    } else { /* if (update) */
        COMPILER_64_ZERO(accum);
#if _SATURATE_INSTEAD_OF_OVERFLOW
        prev = accum;
#endif /* _SATURATE_INSTEAD_OF_OVERFLOW */
        for (ofs = 0; ofs < _OFS_COUNT; ofs++) {
            if (offsets & (1 << ofs)) {
                finalIndex = index + ofs;
                proc = procBase;
                result = _bcm_dpp_modify_proc_and_index(unitData, &proc, &finalIndex, TRUE);
                BCMDNX_IF_ERR_EXIT(result);

                result = COUNTERS_ACCESS.counter.get(unit, proc, finalIndex, &counter_accum);
                BCMDNX_IF_ERR_EXIT(result);                   

                unitData->proc[proc].diag.cache_reads++;
                COMPILER_64_ADD_64(accum, counter_accum);
#if _SATURATE_INSTEAD_OF_OVERFLOW
                if (COMPILER_64_LT(accum, prev)) {
                    /* overflow */
                    COMPILER_64_ALLONES(accum);
                }
                prev = accum;
#endif /* _SATURATE_INSTEAD_OF_OVERFLOW */
            } /* if (offsets & (1 << ofs)) */
        } /* for (ofs = 0; ofs <= _OFS_MAX; ofs++) */
        *counter = accum;

    } /* if (update) */
exit:
    BCMDNX_FUNC_RETURN;
}



STATIC int
_bcm_dpp_counter_native(_bcm_dpp_counter_state_t *unitData,
                                 unsigned int procBase,
                                 unsigned int index,
                                 uint32 offset,
                                 int update,
                                 uint64 *counter){
    int result;
    int unit = unitData->cunit;
    unsigned int finalIndex;
    unsigned int proc = procBase;
    BCMDNX_INIT_FUNC_DEFS;

    finalIndex = index + offset;
    proc = procBase;
    result = _bcm_dpp_modify_proc_and_index(unitData, &proc, &finalIndex, TRUE);
    BCMDNX_IF_ERR_EXIT(result);
    if (update) {
        unitData->proc[proc].diag.cache_writes++;
        result = COUNTERS_ACCESS.counter.set(unit, proc, finalIndex, *counter);
        BCMDNX_IF_ERR_EXIT(result);
    } else {
        unitData->proc[proc].diag.cache_reads++;
        result = COUNTERS_ACCESS.counter.get(unit, proc, finalIndex, counter);
        BCMDNX_IF_ERR_EXIT(result);
    } 
exit:
    BCMDNX_FUNC_RETURN;
}
STATIC int
_bcm_dpp_counters_set_id_inside_lif_range(_bcm_dpp_counter_state_t *unitData, unsigned int proc, int set_id) {
    int result = BCM_E_NONE;
    int unit = unitData->cunit;
    int command_id = 0;
    int is_ingress;
    SOC_SAND_U32_RANGE range;
    int counting_profile;
    BCMDNX_INIT_FUNC_DEFS;

    /* check that this local lif is associated with the relevant command id */
    if(!SOC_IS_JERICHO_PLUS(unit) || unitData->proc[proc].mode.src_type != SOC_TMC_CNT_SRC_TYPE_EPNI)
    {
        if (SOC_IS_ARADPLUS_AND_BELOW(unit) ||
            (unitData->proc[proc].mode.src_type != SOC_TMC_CNT_SRC_TYPE_ING_PP &&
             unitData->proc[proc].mode.src_type != SOC_TMC_CNT_SRC_TYPE_EPNI) || 
            unitData->proc[proc].mode.mode_eg.type != SOC_TMC_CNT_MODE_EG_TYPE_OUTLIF) {
            BCM_EXIT; /*check LIFs on the IRPP and EPNI only, for jericho only*/
        }
        command_id = unitData->proc[proc].mode.command_id;
        is_ingress = (unitData->proc[proc].mode.src_type == SOC_TMC_CNT_SRC_TYPE_ING_PP);
        result = COUNTERS_ACCESS.lif_ranges.counting_profile.get(unit, is_ingress, command_id, &counting_profile);
        BCMDNX_IF_ERR_EXIT(result);

        if (counting_profile == BCM_STAT_LIF_COUNTING_PROFILE_NONE) {
            BCM_EXIT; /*If there is no LIF-counting-profile attached - don't check if set-ID is in LIF-ranges.*/
        }

        result = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_cnt_lif_counting_range_get, (unit, unitData->proc[proc].mode.src_type, command_id, &range));
        BCMDNX_IF_ERR_EXIT(result);
        if (set_id < range.start || set_id > range.end) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Cannnot access LIF-set %d, since it is outside the LIF-counting-range %d-%d"), set_id, range.start, range.end));
        }        
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Name
 *  _bcm_dpp_counter_engine_special_condition
 *  Purpose
 *      When engine map size is 3/5/6/7 counters should be splitted between two consequtive engines
 *      Verifies that the next engine is with the correct configuration and can be used
 *  Arguments
 *      IN int unit
 *      IN _bcm_dpp_counter_state_t *unitData = info for the unit
 *      IN int proc = the counter processor on which to operate
 *      OUT int specialCondition = if it meets the criteria - TRUE, if not - FALSE
*/
int
_bcm_dpp_counter_engine_special_condition(int unit,
                                          _bcm_dpp_counter_state_t *unitData,
                                          int proc,
                                          int *specialCondition)
{
    int result = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;
    if( ((unitData->proc[proc+1].valid & BCM_DPP_COUNTER_PROC_INFO_VALID) == TRUE) &&
                    ((unitData->proc[proc].mode.we_val + 1) == unitData->proc[proc+1].mode.we_val) &&
                    (unitData->proc[proc].mode.command_id == unitData->proc[proc+1].mode.command_id) &&
                    (unitData->proc[proc].mode.src_type == unitData->proc[proc+1].mode.src_type) &&
                    (unitData->proc[proc].mode.mode_eg.type == unitData->proc[proc+1].mode.mode_eg.type)){
        *specialCondition = TRUE;
    }
    else {
        *specialCondition = FALSE;
    }
    BCMDNX_IF_ERR_EXIT(result);
exit:
      BCMDNX_FUNC_RETURN;
}

/*
 *   Name
 *     _bcm_dpp_counter_access
 *   Purpose
 *     Manipulate (read or write) a group of counters within a set.
 *   Arguments
 *     IN/OUT _bcm_dpp_counter_state_t *unitData = info for the unit
 *     IN int proc = the counter processor on which to operate
 *     IN int set_id = the ID of the set on which to operate
 *     IN int update = FALSE to read, TRUE to write
 *     IN int cache_only = TRUE to only use cache values, FALSE to sync first
 *     IN _bcm_dpp_counter_format_t format = format of counter set
 *     IN unsigned int num_counters = number of counters to access
 *     IN bcm_dpp_counter_t *type = type to access (per counter)
 *     IN/OUT uint64 *counter = where to put/get value (per counter)
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 *   Notes
 *     This will update the cache so it is closer to being in sync with the
 *     hardware, shortly before it performs the requested operation.  The
 *     actual operation is performed against the cache.
 *
 *     When writing, a write of zero to an emulated value will write zero to
 *     all participating values.  However, a write of nonzero to an emulated
 *     value may split the value across the participants and the exact values
 *     of each participant is not guaranteed.
 *
 *     Despite the effort to keep the cache in sync with the hardware, there is
 *     still some lag.  High traffic rates will tend to make it obvious.
 *
 *     If reading a counter that is unsupported (either natively or via some
 *     emulation work), the value retrieved will be zero.  If writing such a
 *     counter, the operation is ignored quietly.
 *
 *     Because certain things are unsafe (such as yielding in an interrupt
 *     handler), this function can bypass the hardware sync and manipulate the
 *     cache only.  WARNING: in this mode, coherence of emulated counters is
 *     NOT guaranteed, nor is the point in time at which the read or write will
 *     effectively take place -- it is very possible that there is long-stale
 *     data sitting on the device that should affect the access but which will
 *     be ignored in this mode.  Set cache_only TRUE to enable this mode, but
 *     keep cache_only FALSE under normal conditions.
 */
STATIC int
_bcm_dpp_counter_access(int unit,
                        unsigned int proc,
                        unsigned int set_id,
                        int update,
                        int cache_only,
                        unsigned int num_counters,
                        const bcm_dpp_counter_t *type,
                        uint64 *counter)
{
    _bcm_dpp_counter_state_t *unitData;
    int result, result_sum = BCM_E_NONE;
    unsigned int index;
    unsigned int base;
    unsigned int base_limit_up;
    unsigned int base_limit_down;
    unsigned int num;
    unsigned int lfirst;
    int specialCondition = FALSE;
    int bit_count;
    int select_bitmap = 0;
    bcm_dpp_counter_proc_specific_data_s proc_specific_data;
    int nof_counters_per_entry;
    uint32 offset;
    int lock_taken = FALSE;
    BCMDNX_INIT_FUNC_DEFS;

    select_bitmap = COUNTER_UPDATE_SELECT_PROC_INFO_SPECIFIC_DATA | COUNTER_UPDATE_SELECT_DIAGS;
    proc_specific_data.procArr[0] = proc;
    proc_specific_data.procArrSize = 1;
    proc_specific_data.type = type;
    proc_specific_data.typeSize = num_counters;
    result = _bcm_dpp_counter_unit_check_and_fill(unit, &unitData, select_bitmap, &proc_specific_data);
    BCMDNX_IF_ERR_EXIT(result);
    _COUNTER_UNIT_CHECK(unit, unitData);
    if (!(unitData->proc[proc].valid & BCM_DPP_COUNTER_PROC_INFO_VALID)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d Cannot accees processor ID %u, it is not configured."),
                          unit,
                          proc));
    }
    if (unitData->num_counter_procs <= proc) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d invalid counter processor ID %u"),
                          unit,
                          proc));
    }
    if ((!counter) || (!type)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("obligatory pointer arguments must"
                                           " not be NULL")));
    }

    /*check that all counters are llegal*/
    for (index = 0; index < num_counters; index++) {
        if(unitData->proc[proc].avail_offsets[type[index]] == UNSUPPORTED_COUNTER_OFFSET){
             BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d counter processor %u unavailable counter type %s "),
                           unit, proc, bcm_dpp_counter_t_names[type[index]]));
        }
    }
    /*Validate that we are in counting range for LIFs - in case the engine counts LIFs*/
    result = _bcm_dpp_counters_set_id_inside_lif_range(unitData, proc, set_id);
    BCMDNX_IF_ERR_EXIT(result);

    /* how many counters to sync with hardware, according to format */
    result = _bcm_dpp_counters_per_set(unitData, proc, &num);
    BCMDNX_IF_ERR_EXIT(result);

    /* scaling is applied by hardware; we must apply it as well */
    if (unitData->proc[proc].mode.src_type == SOC_TMC_CNT_SRC_TYPE_VOQ) {
        lfirst = (unitData->proc[proc].mode.mode_eg.base_val / unitData->proc[proc].mode.q_set_size);
    } else {
        lfirst = unitData->proc[proc].mode.mode_eg.base_val;
    }
    /* ingress oam when working in double entry mode, need to add an offset to the set_id (same as was added to he range configuration */
    /* this is because the HW set the MSB bit (direction bit) in the counter pointer */
    if (unitData->proc[proc].mode.src_type == SOC_TMC_CNT_SRC_TYPE_INGRESS_OAM && 
        unitData->proc[proc].mode.format == SOC_TMC_CNT_FORMAT_PKTS_AND_PKTS)
        {
            set_id += SOC_DPP_DEFS_GET(unit, counters_per_counter_processor)*SOC_DPP_DEFS_GET(unit, nof_counter_processors);
        }
    nof_counters_per_entry = BCM_DPP_NOF_COUNTERS_PER_ENTRY_GET(unit, unitData->proc[proc].mode.format);        
    base = ((set_id - lfirst) * num) / nof_counters_per_entry;  
    base_limit_down = (unitData->proc[proc].mode.num_counters * unitData->proc[proc].mode.we_val);
    base_limit_up = (unitData->proc[proc].mode.num_counters * (unitData->proc[proc].mode.we_val + 1));

    /*We keep this condition as backward compatible - if the counter was already scaled.*/
    if (!((base + num - 1) < (unitData->proc[proc].mode.num_counters)) && !((base >= base_limit_down) && ((base + num - 1) < base_limit_up)))
    {
        /* return if num is power of 2 or not - if = 0 - num is power of 2 size */
        bit_count = num & (num - 1);
        /* FOR INGRESS_PP/ETPP, groupSize=5, num_counters/groupSize is not devided. it cause that the last group in the engine is split between
           the current engine and the next one (if defined). it cause to the next engine to be shifted.
           for example: num_counters=32K, engine_0 - we_val=0, engine_1 - we_val=1 group 6553 is mapped to the last 3 counters in the engine_0
           and the first 2 counters in engine_1. group 6554 is mapped to counters 2..6 in engine_1 etc etc..
           Here, we check that next engine is defined in the same configuration as the current one just with we_val++.
         */
        if ((base < unitData->proc[proc].mode.num_counters) || ((base >= base_limit_down) && (base < base_limit_up)))
        {
            if ( ((proc+1) < (SOC_DPP_DEFS_GET(unit, nof_counter_processors))) && 
                (unitData->proc[proc].mode.src_type == SOC_TMC_CNT_SRC_TYPE_ING_PP || unitData->proc[proc].mode.src_type == SOC_TMC_CNT_SRC_TYPE_EPNI ||
                 unitData->proc[proc].mode.src_type == SOC_TMC_CNT_SRC_TYPE_EGR_PP) && (bit_count != 0) )
            {
                _bcm_dpp_counter_engine_special_condition(unit,unitData,proc,&specialCondition);
            }
        }
        if(specialCondition == TRUE)
        {
            LOG_DEBUG(BSL_LS_BCM_CNT, (BSL_META_U(unit, "unit %d access to set %u will be split between proc %u and proc %u \n"), unit, set_id, proc, (proc+1)));
        }
        else
        {
            /* for counter_set_size=3/5/6/7, it is impossible to talk in sets units becuase it is not devided fully in an engine. */
            /* therefore, the error print is in counters entries and not counter_sets */            
            if((bit_count != 0))
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d counter processor %u set %u, counters %u..%u, invalid. The valid range of counters is %u..%u."),
                                  unit,
                                  proc,
                                  set_id,
                                  base,
                                  base + num - 1,
                                  (unitData->proc[proc].mode.num_counters * unitData->proc[proc].mode.we_val),
                                  (unitData->proc[proc].mode.num_counters * (unitData->proc[proc].mode.we_val + 1) - 1)));            
            }
            else
            {
               BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d counter processor %u set %u, counters %u..%u, invalid. The valid range of sets is %u..%u"),
                                unit,
                                proc,
                                set_id,
                                base,
                                base + num - 1,
                                (unitData->proc[proc].mode.num_sets * unitData->proc[proc].mode.we_val),
                                (unitData->proc[proc].mode.num_sets *(unitData->proc[proc].mode.we_val + 1)-1)));            
            }
        }
    }

    /* not sure why doing it, it should be already aligned */
    base %= unitData->proc[proc].mode.num_counters;  

    /* if base > unitData->proc[proc].mode.num_counters --> add to proc+1 */
    if (!cache_only || SOC_TMC_CNT_FORMAT_IS_WIDE(unitData->proc[proc].mode.format, unitData->proc[proc].mode.src_type)) {
        /* sync requested counter set with hardware */	
        result = _bcm_dpp_counter_update_process(unitData, proc, base, num);
        BCMDNX_IF_ERR_EXIT(result);
    }

    /* now scale base so it aligns to the cache */
    base *= _SUB_COUNT;
    result = bcm_dpp_counter_state_lock_take(unit);
    BCMDNX_IF_ERR_EXIT(result);   
    lock_taken = TRUE;
    for (index = 0; index < num_counters; index++) {
        /*take SW cache mutex*/

        /*if native*/
        if(SHR_BITGET(unitData->proc[proc].native, type[index])){
            /* only if double entry and the pointer value is even, need to read the bytes counter */
            if ((unitData->proc[proc].mode.format == ARAD_CNT_FORMAT_PKTS_AND_PKTS) &&
                 (unitData->proc[proc].mode.custom_mode_params.set_size == 1) && 
                 (type[index] == bcm_dpp_counter_offset0_pkts) &&
                 ((((set_id - lfirst) * num) % 2) == 0))
            {
                offset = _OFS_E_CN + _SUB_BYTES;
            }   
            else
            {
                offset = unitData->proc[proc].avail_offsets[type[index]];
            }            
            result = _bcm_dpp_counter_native(unitData,
                            proc,
                            base,
                            offset,
                            update,
                            &(counter[index]));
        }
        else{
            result = _bcm_dpp_counter_single_emulated(unitData,
                             proc,
                             base,
                             unitData->proc[proc].avail_offsets[type[index]],
                             update,
                             &(counter[index]));
        }

        if (BCM_E_CONFIG == result) {
            /* encountered a counter not supported in this configuration */
            COMPILER_64_ZERO(counter[index]);
            if (update) {
                unitData->proc[proc].diag.api_miss_writes++;
            } else {
                unitData->proc[proc].diag.api_miss_reads++;
            }
            BCMDNX_IF_ERR_EXIT(result);
        } else {
            if (update) {
                unitData->proc[proc].diag.api_writes++;
            } else {
                unitData->proc[proc].diag.api_reads++;
            }
            result_sum = result;
        }
        BCMDNX_IF_ERR_CONT(result);
    } /* for (index = 0; index < num_counters; index++) */
    /*release sw cache mutex - even if read/write failed*/ 
    result = bcm_dpp_counter_state_lock_give(unit);
    BCMDNX_IF_ERR_EXIT(result);    
    lock_taken = FALSE;
    result = _bcm_dpp_counter_unit_update(unit, unitData, select_bitmap, &proc_specific_data);
    BCMDNX_IF_ERR_EXIT(result);
    BCMDNX_IF_ERR_EXIT(result_sum);
exit:
    if(lock_taken == TRUE)
    {
        bcm_dpp_counter_state_lock_give(unit);
    }
    BCMDNX_FUNC_RETURN;
}

/*
 *   Name
 *     _bcm_dpp_counter_clear
 *   Purpose
 *     Clear a set of counters, for allocation or similar reason.
 *   Arguments
 *     IN/OUT _bcm_dpp_counter_state_t *unitData = info for the unit
 *     IN unsigned int proc = the counter processor on which to operate
 *     IN unsigned int index = the index of the first counter to process
 *     IN unsigned int num = the number of counters to process
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 *   Notes
 *     This is done before returning an allocated counter set.
 */
STATIC int
_bcm_dpp_counter_clear(_bcm_dpp_counter_state_t *unitData,
                       unsigned int proc,
                       unsigned int set_id)
{
    int unit = unitData->cunit;
    int result;
    unsigned int base;
    unsigned int num;
    uint64 counter;
    BCMDNX_INIT_FUNC_DEFS;

    result = _bcm_dpp_counters_per_set(unitData, proc, &num);
    BCMDNX_IF_ERR_EXIT(result);
    base = set_id * num;
    result = _bcm_dpp_counter_update_process(unitData, proc, base, num);
    BCMDNX_IF_ERR_EXIT(result);

    num *= _SUB_COUNT;
    base *= _SUB_COUNT;

    while (num > 0) {
        num--;
        COMPILER_64_ZERO(counter);
        result = COUNTERS_ACCESS.counter.set(unit, proc, base + num, counter);
        BCMDNX_IF_ERR_EXIT(result);                   
        unitData->proc[proc].diag.cache_writes++;
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Name
 *    _bcm_dpp_counter_alloc
 *  Purpose
 *    Allocate a counter set from a processor where we are providing allocation
 *    from this module (rather than it being implied by the source type).
 *  Arguments
 *    IN/OUT _bcm_dpp_counter_state_t *unitData = info for the unit
 *    IN uint32 flags = flags for allocation control
 *    IN _bcm_dpp_counter_source_t source = source for the allocated set
 *    IN/OUT int *proc = the counter processor for the counter set
 *    IN/OUT int *set_id = the ID of the counter set
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_NOT_FOUND if no available proc using the source
 *       BCM_E_CONFIG if we don't allocate that source here
 *       BCM_E_RESOURCE if there is no available set for the requested source
 *       BCM_E_EXSISTS if we alloc and WITH_ID and it is already in use
 *       BCM_E_* otherwise as appropriate
 *  Notes
 *    If WITH_PROC is specified instead of WITH_ID, will search for a free set
 *    on the speicied processor.
 *
 *    If WITH_ID is specified instead of WITH_PROC, will only try to allocate
 *    the specified set on the specified processor.
 *
 *    If neither WITH_ID nor WITH_PROC is specified, will search for a free set
 *    on any processor(s) that are using the requested source.
 */
STATIC int
_bcm_dpp_counter_alloc(_bcm_dpp_counter_state_t *unitData,
                       uint32 flags,
                       _bcm_dpp_counter_source_t source,
                       unsigned int *proc,
                       unsigned int *set_id)
{
    int unit = unitData->cunit;
    unsigned int pIndex;
    unsigned int pLimit;
    unsigned int sIndex;
    unsigned int sLimit;
    int result = BCM_E_NOT_FOUND; /* assume no proc using the req source */
    uint8 mask;
    uint8 is_alloc;
    uint8 inUse;
    unsigned int allocated;

    BCMDNX_INIT_FUNC_DEFS;
    if ((!proc) || (!set_id)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("NULL may not be user for obligatory OUT arguments")));
    }
    if (flags & BCM_DPP_COUNTER_WITH_ID) {
        if (*proc >= unitData->num_counter_procs) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d counter processor %u invalid"), unit, *proc));
        }
        if (*set_id >= unitData->proc[*proc].mode.num_sets) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d counter processor %u does not support set %u in current config"), unit, *proc, *set_id));
        }
        BCMDNX_IF_ERR_EXIT(COUNTERS_PROC_ACCESS.field.inUse.is_allocated(unit, *proc, &is_alloc));

        if (!is_alloc) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("unit %d counter processor %u sets use implied allocation by source"), unit, *proc));
        }
        sIndex = (*set_id) >> 3;
        mask = 1 << ((*set_id) & 7);
        result = COUNTERS_PROC_ACCESS.field.inUse.get(unit, *proc, sIndex, &inUse);
        BCMDNX_IF_ERR_EXIT(result);

        if (inUse & mask) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("unit %d counter processor %d set %d is already in use"), unit, *proc, *set_id));
        }
        inUse |= mask;
        result = COUNTERS_PROC_ACCESS.field.inUse.set(unit, *proc, sIndex, inUse);
        BCMDNX_IF_ERR_EXIT(result);

        result = COUNTERS_PROC_ACCESS.field.allocated.get(unit, *proc, &allocated);
        BCMDNX_IF_ERR_EXIT(result);
        allocated++;
        result = COUNTERS_PROC_ACCESS.field.allocated.set(unit, *proc, allocated);
        BCMDNX_IF_ERR_EXIT(result);
        BCM_EXIT;
    } else { /* if (flags & BCM_DPP_COUNTER_WITH_ID) */
        if (flags & BCM_DPP_COUNTER_WITH_PROC) {
            pIndex = *proc;
            pLimit = *proc + 1;
        } else {
            pIndex = 0;
            pLimit = unitData->num_counter_procs;
        }
        for (; pIndex < pLimit; pIndex++) {
            if (source == unitData->proc[pIndex].mode.src_type) {
                result = COUNTERS_PROC_ACCESS.field.inUse.is_allocated(unit, pIndex, &is_alloc);
                BCMDNX_IF_ERR_EXIT(result);
                if (is_alloc) {
                        sLimit = (unitData->proc[pIndex].mode.num_sets + 7) >> 3;
                        for (sIndex = 0; sIndex < sLimit; sIndex++) {
                            /* just iterate until either end or find a zero bit */
                            result = COUNTERS_PROC_ACCESS.field.inUse.get(unit, pIndex, sIndex, &inUse);
                            BCMDNX_IF_ERR_EXIT(result);

                            if (255 != inUse){
                                break;
                            }
                        }
                        if (sIndex < sLimit) {
                            sLimit = sIndex << 3;

                            result = COUNTERS_PROC_ACCESS.field.inUse.get(unit, pIndex, sIndex, &inUse);
                            BCMDNX_IF_ERR_EXIT(result);

                            for (mask = 1; 0 != mask; mask = mask << 1, sLimit++) {
                                /* just iterate until we find a zero bit */
                                if (0 == (mask & inUse)){
                                    break;
                                }
                            }
                            if (sLimit < unitData->proc[pIndex].mode.num_sets) {
                                /* mark it as in use */
                                inUse |= mask;
                                result = COUNTERS_PROC_ACCESS.field.inUse.set(unit, pIndex, sIndex, inUse);
                                BCMDNX_IF_ERR_EXIT(result);

                                result = COUNTERS_PROC_ACCESS.field.allocated.get(unit, pIndex, &allocated);
                                BCMDNX_IF_ERR_EXIT(result);
                                allocated++;
                                result = COUNTERS_PROC_ACCESS.field.allocated.set(unit, pIndex, allocated);
                                BCMDNX_IF_ERR_EXIT(result);

                                /* return it now */
                                *proc = pIndex;
                                *set_id = sLimit;
                                BCM_EXIT;
                            } else {
                                /* fell off the end; no available sets */
                                result = BCM_E_RESOURCE;
                            }
                        } else { /* if (sIndex < sLimit) */
                            /* fell off the end; no available sets */
                            result = BCM_E_RESOURCE;
                        } /* if (sIndex < sLimit) */
                } else { /* if (is_alloc) */
                    /* we do not allocate this source here */
                    result = BCM_E_CONFIG;
                } /* if (is_alloc) */
            } /* if (source == unitData->proc[pIndex].mode.src_type) */
        } /* for (; pIndex < pLimit; pIndex++) */
    } /* if (flags & BCM_DPP_COUNTER_WITH_ID) */
    if (flags & BCM_DPP_COUNTER_WITH_ID)
    {
        LOG_ERROR(BSL_LS_BCM_CNT, (BSL_META_U(unit, "unit %d unable to alloc a counter set for source %d: %d (%s)\n"), unit, source, result, _SHR_ERRMSG(result)));
    }
    else
    {
        LOG_VERBOSE(BSL_LS_BCM_CNT, (BSL_META_U(unit, "unit %d unable to alloc a counter set for source %d: %d (%s)\n"), unit, source, result, _SHR_ERRMSG(result)));
    }
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Name
 *    _bcm_dpp_counter_free
 *  Purpose
 *    Free a counter set from a processor where we are providing allocation
 *    from this module (rather than it being implied by the source type).
 *  Arguments
 *    IN/OUT _bcm_dpp_counter_state_t *unitData = info for the unit
 *    IN/OUT int *proc = the counter processor for the counter set
 *    IN/OUT int *set_id = the ID of the counter set
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 *  Notes
 */
STATIC int
_bcm_dpp_counter_free(_bcm_dpp_counter_state_t *unitData,
                      unsigned int proc,
                      unsigned int set_id)
{
    int unit = unitData->cunit;
    unsigned int sIndex;
    uint8 mask;
    uint8 is_alloc = 0;
    uint8 inUse;
    unsigned int allocated;
    BCMDNX_INIT_FUNC_DEFS;
    if (proc >= unitData->num_counter_procs) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d counter processor %u invalid"), unit, proc));
    }
    if (set_id >= unitData->proc[proc].mode.num_sets) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d counter processor %u does not support set %u in current config"), unit, proc, set_id));
    }

    BCMDNX_IF_ERR_EXIT(COUNTERS_PROC_ACCESS.field.inUse.is_allocated(unit, proc, &is_alloc));

    if (!is_alloc) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("unit %d counter processor %u sets use implied allocation by source"), unit, proc));
    }
    sIndex = set_id >> 3;
    mask = 1 << (set_id & 7);

    BCMDNX_IF_ERR_EXIT(COUNTERS_PROC_ACCESS.field.inUse.get(unit, proc, sIndex, &inUse));
    if (0 == (inUse & mask)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("unit %d counter processor %d set %d is not in use"), unit, proc, set_id));
    }
    inUse &= (~mask);
    BCMDNX_IF_ERR_EXIT(COUNTERS_PROC_ACCESS.field.inUse.set(unit, proc, sIndex, inUse));

    BCMDNX_IF_ERR_EXIT(COUNTERS_PROC_ACCESS.field.allocated.get(unit, proc, &allocated));
    allocated--;
    BCMDNX_IF_ERR_EXIT(COUNTERS_PROC_ACCESS.field.allocated.set(unit, proc, allocated));
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC
int _bcm_dpp_counter_bg_thread_attach(_bcm_dpp_counter_state_t *unit_data){
    int unit = unit_data->cunit;
    BCMDNX_INIT_FUNC_DEFS;
    if(COUNTERS_SYNC_ACCESS.bgSem != NULL){
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("unit %d bg sem already exist\n"), unit));
    }
    if(glb_bg_thread_and_dma_data[unit].running){
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("unit %d bg thread already run\n"), unit));
    }
    COUNTERS_SYNC_ACCESS.bgSem = sal_sem_create("dpp_counters_wait", sal_sem_BINARY, 1);
    if (!COUNTERS_SYNC_ACCESS.bgSem) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("unit %d unable to create bg sem\n"), unit));
    }

    if (COUNTERS_SYNC_ACCESS.background != NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("unit %d bg thread already exist\n"), unit));
    }
    COUNTERS_SYNC_ACCESS.background = sal_thread_create("dpp_counter_bg", SAL_THREAD_STKSZ, 50, _bcm_dpp_counter_bg, unit_data);
    if (SAL_THREAD_ERROR == COUNTERS_SYNC_ACCESS.background) {
        sal_sem_destroy(COUNTERS_SYNC_ACCESS.bgSem);
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("unit %d unable to create counter background thread\n"), unit));
    }
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 *  Name
 *    soc_dpp_counter_bg_thread_detach
 *  Purpose
 *    Disconnect counter support on a unit
 *  Arguments
 *    int unit = unit number to detach
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 *   Notes
 *     This only signals to the background thread to stop.
 * 
 *     Unit must be initialized.
 */
STATIC
int _bcm_dpp_counter_bg_thread_detach(_bcm_dpp_counter_state_t *unit_data){
    soc_timeout_t         to;
    int unit = unit_data->cunit;
    BCMDNX_INIT_FUNC_DEFS;

    if (glb_bg_thread_and_dma_data[unit].running){
        _bcm_counter_thread_is_running[unit] = TRUE;
        glb_bg_thread_and_dma_data[unit].running = FALSE;
        sal_sem_give(COUNTERS_SYNC_ACCESS.bgSem); 
        soc_timeout_init(&to, _BCM_COUNTER_THREAD_WAIT_TIMEOUT, 0);
        while (_bcm_counter_thread_is_running[unit] == TRUE) {
            if (soc_timeout_check(&to)) {
                if (_bcm_counter_thread_is_running[unit] == TRUE) {
                    _bcm_counter_thread_is_running[unit] = FALSE;
                    /*we cannot release the bgsem because we are not sure in this case what is the thread status*/
                    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("unit %d _bcm_dpp_counter_bg thread is not responding\n"), unit));
                }
            }
            sal_usleep(1000000);
        }
        sal_sem_destroy(COUNTERS_SYNC_ACCESS.bgSem);
        /*sal_thread_destroy(COUNTERS_SYNC_ACCESS.background);*/
        COUNTERS_SYNC_ACCESS.bgSem = NULL;
        sal_thread_destroy(COUNTERS_SYNC_ACCESS.background);
        COUNTERS_SYNC_ACCESS.background = NULL;
        LOG_DEBUG(BSL_LS_BCM_CNT, (BSL_META_U(unit, "unit %d counter background thread exit\n"), unit));
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Name
 *    bcm_dpp_counter_dma_attach
 *  Purpose
 *    Find a free DMA channel to allocate. 
 *  Arguments
 *    unitData
 *    uint8 reserved_channel - a chennel that was changed and cannot be taken right now.
 *    Out channel - free channel
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 * 
 *     Unit must be initialized.
 */
STATIC
int _bcm_dpp_counter_dma_find_free_channel(
    _bcm_dpp_counter_state_t    *unitData,
    uint8                       reserved_channel,
    uint8                       *channel)
{
    int unit = unitData->cunit;
    int result = BCM_E_NONE;
    int channel_number;
    uint8 fifo, fifo_get = 0;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(unitData);
    SOCDNX_NULL_CHECK(channel);
    BCM_DPP_UNIT_CHECK(unit);

    /*1. free uneeded channels*/
    for (fifo = 0; fifo < SOC_DPP_DEFS_GET(unit, nof_counter_fifos); fifo++) {
        if (unitData->fifo[fifo].ref_count == 0 && 
            glb_bg_thread_and_dma_data[unit].dma_channel[fifo] != SOC_TMC_CNT_INVALID_DMA_CHANNEL) {
            /*Have a channel and  don't need it.*/
            
            result = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_cnt_channel_to_fifo_mapping_get, (unit, glb_bg_thread_and_dma_data[unit].dma_channel[fifo], &fifo_get));
            BCMDNX_IF_ERR_EXIT(result);
            if (fifo_get != fifo) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, 
                                    (_BSL_BCM_MSG("unit %d channel %d is mapped to fifo %d in SW, but mapped to fifo %d in HW\n"), unit, fifo, glb_bg_thread_and_dma_data[unit].dma_channel[fifo], fifo, fifo_get));
            }
            result = _bcm_dpp_counter_dma_fifo_dettach(unitData, fifo);
            BCM_SAND_IF_ERR_EXIT(result);
            break;
        }
    }

    if(soc_feature(unit, soc_feature_cmicm_multi_dma_cmc) && SOC_PCI_CMCS_NUM(unit) > 1)
    {
        result = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_mgmt_dma_fifo_channel_free_find, (unit, TRUE, &channel_number));
    } else {
        result = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_mgmt_dma_fifo_channel_free_find, (unit, FALSE, &channel_number));
    }

    BCMDNX_IF_ERR_EXIT(result);

    if (channel_number == -1) {
        /*Return an error - all channels are taken.*/
        BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("unit %d Cannot find free DMA-channel."), unit));
    }
    *channel = channel_number;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Name
 *    _bcm_dpp_counter_dma_fifo_attach
 *  Purpose
 *    Find a free DMA channel to and attach it to CRPS-fifo. 
 *  Arguments
 *    unitData
 *    fifo - the CRPS-fifo to to be attached
 *    uint8 reserved_channel - a channel that was changed and cannot be taken right now.
 *  Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 * 
 *     Unit must be initialized.
 */
STATIC
int _bcm_dpp_counter_dma_fifo_attach(_bcm_dpp_counter_state_t *unitData, int fifo, uint8 reserved_channel) {
    int unit = unitData->cunit;
    uint32 sand_rv;
    int result = BCM_E_NONE;
    uint8 free_channel = 0;
    int channel_number = -1;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(unitData);

    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        /*In Arad-there is one-to-one mapping between FIFO and channel*/
        free_channel = fifo;
    } else {
        /* No channel - no peace */
        result = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_mgmt_dma_fifo_channel_get, (unit, (dma_fifo_channel_src_t)fifo, &channel_number));
        BCMDNX_IF_ERR_EXIT(result);   
        /* in case that the fifo DMA already configure in HW, no need to find a new channel*/
        if(channel_number == -1)
        {
            result = _bcm_dpp_counter_dma_find_free_channel(unitData, reserved_channel, &free_channel);
            BCMDNX_IF_ERR_EXIT(result);        
        }
        else
        {
            free_channel = (uint8)channel_number;
        }
    }

    /*Start DMA on a new channel*/
    sand_rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_cnt_dma_set, (unit, fifo, free_channel, SOC_DPP_DEFS_GET(unit, counter_fifo_depth)));
    BCM_SAND_IF_ERR_EXIT(sand_rv);

    /*Update the DMA select*/
    result = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_cnt_channel_to_fifo_mapping_set, (unit, free_channel, fifo));
    BCMDNX_IF_ERR_EXIT(result);
    glb_bg_thread_and_dma_data[unit].dma_channel[fifo] = free_channel;
exit:
    BCMDNX_FUNC_RETURN;
}
/*
 *  Name
 *    _bcm_dpp_counter_dma_fifo_dettach
 *  Purpose
 *    Free DMA channel attached to CRPS-fifo. 
 *  Arguments
 *    unitData
 *    fifo - the CRPS-fifo to to be attached
 *  Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 * 
 *     Unit must be initialized.
 */
STATIC
int _bcm_dpp_counter_dma_fifo_dettach(_bcm_dpp_counter_state_t *unitData, int fifo){
    int unit = unitData->cunit;
    int result = BCM_E_NONE;
    uint32 sand_rv;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(unitData);

    sand_rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_cnt_dma_unset, (unit, fifo, glb_bg_thread_and_dma_data[unit].dma_channel[fifo]));
    BCM_SAND_IF_ERR_EXIT(sand_rv);

    result = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_cnt_channel_to_fifo_mapping_set, (unit, glb_bg_thread_and_dma_data[unit].dma_channel[fifo], SOC_TMC_CNT_INVALID_FIFO));
    BCMDNX_IF_ERR_EXIT(result);

    glb_bg_thread_and_dma_data[unit].dma_channel[fifo] = SOC_TMC_CNT_INVALID_DMA_CHANNEL;

exit:
    BCMDNX_FUNC_RETURN;
}
/*
 *  Name
 *    _bcm_dpp_counter_dma_fifo_update_state
 *  Purpose
 *    After engine configuration, check if the it's FIFO needs to be attached or detached. 
 *  Arguments
 *    unitData
 *    fifo - the CRPS-fifo to to be attached
 *  Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 * 
 *     Unit must be initialized.
 */
STATIC
int _bcm_dpp_counter_dma_fifo_update_state(_bcm_dpp_counter_state_t *unitData, int fifo, uint8 reserved_channel) {
    int unit = unitData->cunit;
    int result = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(unitData);
    /*need to stop DMA*/
    if ((!glb_bg_thread_and_dma_data[unit].dma_attached || unitData->fifo[fifo].ref_count == 0) && 
         glb_bg_thread_and_dma_data[unit].dma_channel[fifo] != SOC_TMC_CNT_INVALID_DMA_CHANNEL) {
        result = _bcm_dpp_counter_dma_fifo_dettach(unitData, fifo);
        BCMDNX_IF_ERR_EXIT(result);
    } 
    if (glb_bg_thread_and_dma_data[unit].dma_attached && 
        (unitData->fifo[fifo].ref_count != 0 && glb_bg_thread_and_dma_data[unit].dma_channel[fifo] == SOC_TMC_CNT_INVALID_DMA_CHANNEL)) {
        result = _bcm_dpp_counter_dma_fifo_attach(unitData, fifo, reserved_channel);
        BCMDNX_IF_ERR_EXIT(result);
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Name
 *    bcm_dpp_counter_dma_attach
 *  Purpose
 *    Enable the DMA from the CMIC mechanism. 
 *  Arguments
 *    int unit - unit number to detach
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 *       
 *   Notes
 *     The only DMAs that allowed also by CRPS will be active
 * 
 *     Unit must be initialized.
 */
STATIC
int _bcm_dpp_counter_dma_attach(_bcm_dpp_counter_state_t *unitData){
    int unit = unitData->cunit;
    int fifo;
    int result = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(unitData);
    BCM_DPP_UNIT_CHECK(unit);
    if (!glb_bg_thread_and_dma_data[unit].dma_attached) {
        /*Allocate new chnells*/
        for (fifo = 0; fifo < SOC_DPP_DEFS_GET(unit, nof_counter_fifos); fifo++) {
            if (unitData->fifo[fifo].ref_count == 0) {
                continue; /*You don't need a channel, don't be greedy*/
            }
            if (glb_bg_thread_and_dma_data[unit].dma_channel[fifo] != SOC_TMC_CNT_INVALID_DMA_CHANNEL) {
                continue; /*You already have a channel, don't be greedy*/
            }
            result = _bcm_dpp_counter_dma_fifo_attach(unitData, fifo, SOC_TMC_CNT_INVALID_FIFO);
            BCMDNX_IF_ERR_EXIT(result);
        }
        glb_bg_thread_and_dma_data[unit].dma_attached = 1;
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Name
 *    bcm_dpp_counter_dma_detach
 *  Purpose
 *    Stop the DMA from the CMIC mechanism. Still enabled by CRPS.
 *    to re-enable call bcm_dpp_counter_dma_attach
 *  Arguments
 *    int unit = unit number to detach
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 *   Notes
 *     This is not the reverse of attach -
 *     attach updates the DMA state if the DMA is attached
 *     detach clears it.
 *     Unit must be initialized.
 */
STATIC
int _bcm_dpp_counter_dma_detach(_bcm_dpp_counter_state_t *unitData) {
    int unit = unitData->cunit;
    int fifo;
    int result = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(unitData);
    BCM_DPP_UNIT_CHECK(unit);
    if (glb_bg_thread_and_dma_data[unit].dma_attached) {
        for (fifo = 0; fifo < SOC_DPP_DEFS_GET(unit, nof_counter_fifos); fifo++) {
            if (unitData->fifo[fifo].ref_count == 0) {
                continue; /*You don't have a channel*/
            }
            if (glb_bg_thread_and_dma_data[unit].dma_channel[fifo] != SOC_TMC_CNT_INVALID_DMA_CHANNEL) {
                result = _bcm_dpp_counter_dma_fifo_dettach(unitData, fifo);
                BCMDNX_IF_ERR_EXIT(result);
            }
        }
        glb_bg_thread_and_dma_data[unit].dma_attached = 0;
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Name
 *    bcm_dpp_counter_reserve_dma_channel
 *  Purpose
 *    Make move the CRPS-FIFO to another DMA-channel if it is attached to given channel. 
 *  Arguments
 *    int unit = unit number.
 *    uint8 channel - reserved channel.
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 *   Notes
 *     The only DMAs that allowed also by CRPS will be active
 *     Unit must be initialized.
 */
int bcm_dpp_counter_reserve_dma_channel(
    int unit, 
    uint8 channel) {
    int result = BCM_E_NONE;
    _bcm_dpp_counter_state_t *unitData;
    uint32      sand_rv;
    uint8 fifo;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    if (channel >= SOC_DPP_DEFS_GET(unit, nof_counter_fifos)) {
        BCMDNX_IF_ERR_EXIT(BCM_E_PARAM); /*illegal DMA channel*/
    }

    result = _bcm_dpp_counter_unit_check_and_fill(unit, &unitData, COUNTER_UPDATE_SELECT_ALL, NULL);
    BCMDNX_IF_ERR_EXIT(result);

    result = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_cnt_channel_to_fifo_mapping_get, (unit, channel , &fifo));
    BCMDNX_IF_ERR_EXIT(result);

    if (fifo < SOC_DPP_DEFS_GET(unit, nof_counter_fifos)) {
        unsigned int proc_mask = 0;
        int nof_procs_per_fifo = SOC_DPP_DEFS_GET(unit, nof_counter_processors) / SOC_DPP_DEFS_GET(unit, nof_counter_fifos);
        SHR_BITSET_RANGE(&proc_mask, (fifo * nof_procs_per_fifo), nof_procs_per_fifo);

        /*Drain-DMA if needed*/
        result = _bcm_dpp_counter_fifo_process(unitData, proc_mask,  NULL);
        BCMDNX_IF_ERR_EXIT(result);

        /*Stop DMA*/
        sand_rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_cnt_dma_unset, (unit, fifo, channel));
        BCM_SAND_IF_ERR_EXIT(sand_rv);
        /*Attach the FIFO*/
        result = _bcm_dpp_counter_dma_fifo_attach(unitData, fifo, channel);
        BCMDNX_IF_ERR_EXIT(result);
        /*Update Unit data*/
        result = _bcm_dpp_counter_unit_update(unit, unitData, COUNTER_UPDATE_SELECT_ALL, NULL);
        BCMDNX_IF_ERR_EXIT(result);
    }

exit:
    BCMDNX_FUNC_RETURN;
}


int 
bcm_dpp_counter_background_collection_enable_set(int unit, int enable){
    _bcm_dpp_counter_state_t *unitData;
    int channel_number = -1;
    int fifo;    
    BCMDNX_INIT_FUNC_DEFS;
    if ((0 > unit) || (SOC_MAX_NUM_DEVICES <= unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNIT, (_BSL_BCM_MSG("unit is not valid\n")));
    }
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_counter_unit_check_and_fill(unit, &unitData, COUNTER_UPDATE_SELECT_ALL, NULL));
    unitData->sampling_interval_configured = enable;
    if(enable){
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_counter_dma_attach(unitData));
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_counter_bg_thread_attach(unitData));
    } else{
        if(SOC_IS_JERICHO(unit))
        {
            for (fifo = 0; fifo < SOC_DPP_DEFS_GET(unit, nof_counter_fifos); fifo++) 
            {
                /* get the DMA channel number that each FIFO belong to. */
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_mgmt_dma_fifo_channel_get, (unit, (dma_fifo_channel_src_t)fifo, &channel_number)));
                if(channel_number != -1)
                {
                    /* Disonnect DMA engine from the CRPS. */
                    BCMDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_mgmt_dma_fifo_channel_set, (unit, channel_number, dma_fifo_channel_src_reserved)));                 
                }
            }
            /* give 2 more seconds to the BG thread to empty the FIFO */
            sal_usleep(2000000);            
        }    
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_counter_bg_thread_detach(unitData));
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_counter_dma_detach(unitData));
    }
    
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_counter_unit_update(unit, unitData, COUNTER_UPDATE_SELECT_ALL, NULL));
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_counter_background_collection_enable_get(int unit, int *enable){
    BCMDNX_INIT_FUNC_DEFS;
    if ((0 > unit) || (SOC_MAX_NUM_DEVICES <= unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNIT, (_BSL_BCM_MSG("unit is not valid\n")));
    }
    BCMDNX_NULL_CHECK(enable);
    /*Get if the BG thread already stoped*/
    * enable = glb_bg_thread_and_dma_data[unit].running;
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 *  Name
 *    _bcm_dpp_counter_attach_init
 *  Purpose
 *    call DMA and Bg thread init if required. In addition, update DB
 *  Arguments
 *    int unit - unit number to detach
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 * 
 *     Unit must be initialized.
 */
STATIC
int _bcm_dpp_counter_attach_init(_bcm_dpp_counter_state_t *unitData){
    int unit = unitData->cunit;
    int result = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(unitData);
    BCM_DPP_UNIT_CHECK(unit);

    if (unitData->sampling_interval_configured) {
        result = _bcm_dpp_counter_dma_attach(unitData);
        BCMDNX_IF_ERR_EXIT(result);
    }
    glb_bg_thread_and_dma_data[unit].bgWait = _BACKGROUND_THREAD_ITER_DEFAULT(unitData->cunit);
    BCMDNX_IF_ERR_EXIT(result);

    if(unitData->sampling_interval_configured) {
        LOG_DEBUG(BSL_LS_BCM_CNT,(BSL_META_U(unit,"unit %d counter background thread init\n"),unit));
        result = _bcm_dpp_counter_bg_thread_attach(unitData);
        BCMDNX_IF_ERR_EXIT(result);
    }

exit:
    BCMDNX_FUNC_RETURN;
}



STATIC int
_soc_dpp_str_prop_parse_counter_source(int unit, soc_port_t port, int *src_core, SOC_TMC_CNT_SRC_TYPE *src_type, int *command_id, SOC_TMC_CNT_MODE_EG_TYPE *eg_type, SOC_TMC_CNT_COUNTER_WE_BITMAP* we_val, uint8* valid)
{
    char *propkey, propval[256], propval_parse[256], *tmp_propval, *tok;
    char             *end_c;
    char             *tokstr;
    int propval_len = 0, propval_index;
    int nof_ints, ints[3], max_nof_ints = 3;
    BCMDNX_INIT_FUNC_DEFS;

    propkey = spn_COUNTER_ENGINE_SOURCE;
    
  
    *src_type = SOC_TMC_CNT_SRC_TYPE_ING_PP;
    *eg_type = SOC_TMC_CNT_NOF_MODE_EG_TYPES_ARAD;

    tmp_propval = soc_property_port_get_str(unit, port, propkey);

    if (tmp_propval == NULL) {
        *valid = FALSE;
        BCM_EXIT;
    }
    propval_len = sal_strlen(tmp_propval);
    for (propval_index = 0; propval_index < propval_len; propval_index++) {
        if (tmp_propval[propval_index] == '.') {
            break;
        }
    }
    if (propval_index < propval_len) {
        *src_core = _shr_ctoi(tmp_propval + propval_index + 1);
    } else {
        *src_core = 0;
    }     
    sal_strncpy(propval, tmp_propval, propval_index);
    propval[propval_index] = '\0';
    sal_strcpy(propval_parse, propval);

    *valid = BCM_DPP_COUNTER_PROC_INFO_VALID;
    if (port >= SOC_DPP_DEFS_GET(unit, nof_counter_processors)) {
        if (port == SOC_DPP_DEFS_GET(unit, nof_counter_processors)) {
            *src_core = 0;
        } else {
            *src_core = 1;
        }
        *src_type = SOC_TMC_CNT_SRC_TYPES_EGQ_TM;
        *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_TM;
        if ((IS_STR_LSB0(propval, "EGRESS_RECEIVE_TM")) || (IS_STR_LSB1(propval, "EGRESS_RECEIVE_TM"))) { 
            *we_val = 0;
        } else if ((IS_STR_MSB0(propval, "EGRESS_RECEIVE_TM")) || (IS_STR_MSB1(propval, "EGRESS_RECEIVE_TM"))) {
            *we_val = 1;
        } else {
            LOG_WARN(BSL_LS_BCM_CNT,(BSL_META_U(unit, "unit %d counter processor %u Source should not be configured\n"), unit, port));
            *we_val = 0;
        }       
        BCM_EXIT;
    }

    /*Step 1: find what format is it*/
    for (nof_ints = 0, tok = sal_strtok_r(propval_parse, "_", &tokstr); 
         nof_ints < max_nof_ints;
         tok = sal_strtok_r(NULL, "_", &tokstr)) {
        if (tok) {
            if (isdigit((int)(tok[0]))) {
                ints[nof_ints] = sal_ctoi(tok, &end_c);
                nof_ints++;
            }
        } else {
            break;
        }
    }
    if (nof_ints > 3) {
        /*error*/
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("Unexpected property value format (\"%s\") for %s\n\r"),propval, propkey));
    } else if (nof_ints == 3) { /*New Jerciho compatibale format*/
        *src_core = ints[0];
        *command_id = ints[1];
        *we_val = ints[2];
        for (propval_index = 0; propval_index < propval_len - 1; propval_index++) {
            if (propval[propval_index] == '_' && isdigit((int)(propval[propval_index + 1]))) {
                break;
            }
        }
        if (propval_index < propval_len) {
            propval[propval_index] = '\0';
        }
        if (IS_STR(propval,"INGRESS_FIELD")) {
            *src_type = SOC_TMC_CNT_SRC_TYPE_ING_PP;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_PMF;
        } else if (IS_STR(propval,"INGRESS_VSI")) {
            *src_type = SOC_TMC_CNT_SRC_TYPE_ING_PP;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_VSI;
        } else if (IS_STR(propval,"INGRESS_IN_LIF")) {
            *src_type = SOC_TMC_CNT_SRC_TYPE_ING_PP;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_OUTLIF;
        } else if (IS_STR(propval, "INGRESS_VOQ")) {
            *src_type = SOC_TMC_CNT_SRC_TYPE_VOQ;
        } else if (IS_STR(propval, "INGRESS_EXT_STAT")) {
            *src_type = SOC_TMC_CNT_SRC_TYPE_INGRESS_EXT_STAT;          
        } else if (IS_STR(propval, "INGRESS_STAG")) {
            *src_type = SOC_TMC_CNT_SRC_TYPE_STAG;
        } else if (IS_STR(propval, "INGRESS_VSQ")) {
            *src_type = SOC_TMC_CNT_SRC_TYPE_VSQ;
        } else if (IS_STR(propval, "INGRESS_CNM")) {
            *src_type  = SOC_TMC_CNT_SRC_TYPE_CNM_ID;
        } else if (IS_STR(propval, "EGRESS_OAM")) {
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EGRESS_OAM;
            if(!SOC_IS_QAX(unit)){
                *command_id = 0;
            }
        } else if (IS_STR(propval, "INGRESS_OAM")) {
            *src_type  = SOC_TMC_CNT_SRC_TYPE_INGRESS_OAM;
            if(!SOC_IS_QAX(unit)){
                *command_id = 1;  
            }
        } else if (IS_STR(propval, "EGRESS_VSI")) {
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EPNI;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_VSI;
        } else if (IS_STR(propval, "EGRESS_OUTLIF") || IS_STR(propval, "EGRESS_OUT_LIF")) {
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EPNI;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_OUTLIF;
        } else if (IS_STR(propval, "EGRESS_TM")) {
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EPNI;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_TM;       
        } else if (IS_STR(propval, "EGRESS_TM_PORT")) {
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EPNI;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_TM_PORT;
        } else if (IS_STR(propval, "EGRESS_RECEIVE_VSI")) {
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EGR_PP;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_VSI;
        } else if (IS_STR(propval, "EGRESS_RECEIVE_OUTLIF") || IS_STR(propval, "EGRESS_RECEIVE_OUT_LIF")) {
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EGR_PP;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_OUTLIF;
        } else if (IS_STR(propval, "EGRESS_RECEIVE_TM")) {
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EGR_PP;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_TM;
        } else if (IS_STR(propval, "EGRESS_RECEIVE_TM_PORT")) {
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EGR_PP;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_TM_PORT;
        } else if (IS_STR(propval, "EGRESS_FIELD")) {
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EGR_PP;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_PMF;
        } else if (IS_STR(propval, "INGRESS_LATENCY")) {
            *src_type  = SOC_TMC_CNT_SRC_TYPES_IPT_LATENCY;
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("Unexpected property value (\"%s\") for %s\n\r"),propval, propkey));
        }
    } else if (isdigit((int)(propval[0]))) { /*PetraB compatibale format*/
        *src_type = sal_ctoi(propval, &end_c);

        switch (*src_type)  {
        case SOC_TMC_CNT_SRC_TYPE_ING_PP:
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_PMF;
             break;
        case SOC_TMC_CNT_SRC_TYPE_VOQ:
        case SOC_TMC_CNT_SRC_TYPE_INGRESS_EXT_STAT:			
        case SOC_TMC_CNT_SRC_TYPE_STAG:
        case SOC_TMC_CNT_SRC_TYPE_VSQ:
        case SOC_TMC_CNT_SRC_TYPE_CNM_ID:
        case SOC_TMC_CNT_SRC_TYPE_INGRESS_OAM:
        case SOC_TMC_CNT_SRC_TYPE_EGRESS_OAM:            
            *eg_type = SOC_TMC_CNT_NOF_MODE_EG_TYPES_ARAD;
             break;
        case SOC_TMC_CNT_SRC_TYPE_EGR_PP:
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_TM;
            break;
        default:
            if (*src_type == 6) {
                *src_type = SOC_TMC_CNT_SRC_TYPE_EGR_PP;
                *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_VSI;
            } else if (*src_type == 7) {
                *src_type = SOC_TMC_CNT_SRC_TYPE_EGR_PP;
                *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_OUTLIF;
            } else if (*src_type == 8) {
                *src_type = SOC_TMC_CNT_SRC_TYPE_EGR_PP;
                *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_PMF;
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("Unexpected property value (\"%d\") for %s\n\r"),*src_type, propkey));  
            }       
        }
    } else {
        if ((IS_STR_LSB0(propval, "INGRESS_FIELD")) || (IS_STR_LSB0(propval, "INGRESS_VSI"))||
           (IS_STR_LSB0(propval, "INGRESS_IN_LIF")) || (IS_STR_LSB0(propval, "INGRESS_TM"))){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_ING_PP;
            *command_id = 0;
            *we_val = 0;
        }
        else if ((IS_STR_LSB1(propval, "INGRESS_FIELD")) || (IS_STR_LSB1(propval, "INGRESS_VSI"))||
           (IS_STR_LSB1(propval, "INGRESS_IN_LIF")) || (IS_STR_LSB1(propval, "INGRESS_TM"))){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_ING_PP;
            *command_id = 1;
            *we_val = 0;
        }
        else if ((IS_STR_MSB0(propval, "INGRESS_FIELD")) || (IS_STR_MSB0(propval, "INGRESS_VSI"))||
           (IS_STR_MSB0(propval, "INGRESS_IN_LIF")) || (IS_STR_MSB0(propval, "INGRESS_TM"))){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_ING_PP;
            *command_id = 0;
            *we_val = 1;
        }
        else if ((IS_STR_MSB1(propval, "INGRESS_FIELD")) || (IS_STR_MSB1(propval, "INGRESS_VSI"))||
           (IS_STR_MSB1(propval, "INGRESS_IN_LIF")) || (IS_STR_MSB1(propval, "INGRESS_TM"))){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_ING_PP;
            *command_id = 1;
            *we_val = 1;
        }
        else if (IS_STR_LSB0(propval, "INGRESS_VOQ")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_VOQ;
            *command_id = 0;
            *we_val = 0;
        } 
        else if (SOC_IS_JERICHO(unit) && IS_STR_LSB1(propval, "INGRESS_VOQ")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_VOQ;
            *command_id = 1;
            *we_val = 0;
        } 
        else if (SOC_IS_JERICHO(unit) && IS_STR_MSB0(propval, "INGRESS_VOQ")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_VOQ;
            *command_id = 0;
            *we_val = 1;
        } 
        else if (SOC_IS_JERICHO(unit) && IS_STR_MSB1(propval, "INGRESS_VOQ")){
            *src_type = SOC_TMC_CNT_SRC_TYPE_VOQ;
            *command_id = 1;
            *we_val = 1;
        } 
        else if (IS_STR_LSB0(propval, "INGRESS_EXT_STAT")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_INGRESS_EXT_STAT;
            *command_id = 0;
            *we_val = 0;
        } 
        else if (SOC_IS_JERICHO(unit) && IS_STR_LSB1(propval, "INGRESS_EXT_STAT")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_INGRESS_EXT_STAT;
            *command_id = 1;
            *we_val = 0;
        } 
        else if (SOC_IS_JERICHO(unit) && IS_STR_MSB0(propval, "INGRESS_EXT_STAT")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_INGRESS_EXT_STAT;
            *command_id = 0;
            *we_val = 1;
        } 
        else if (SOC_IS_JERICHO(unit) && IS_STR_MSB1(propval, "INGRESS_EXT_STAT")){
            *src_type = SOC_TMC_CNT_SRC_TYPE_INGRESS_EXT_STAT;
            *command_id = 1;
            *we_val = 1;
        } 		
        else if (IS_STR_LSB0(propval, "INGRESS_STAG")){
            *src_type = SOC_TMC_CNT_SRC_TYPE_STAG;
            *command_id = 0;
            *we_val = 0;
        }
        else if (SOC_IS_JERICHO(unit) && IS_STR_LSB1(propval, "INGRESS_STAG")){
            *src_type = SOC_TMC_CNT_SRC_TYPE_STAG;
            *command_id = 1;
            *we_val = 0;
        }
        else if (SOC_IS_JERICHO(unit) && IS_STR_MSB0(propval, "INGRESS_STAG")){
            *src_type = SOC_TMC_CNT_SRC_TYPE_STAG;
            *command_id = 0;
            *we_val = 1;
        }
        else if (SOC_IS_JERICHO(unit) && IS_STR_MSB1(propval, "INGRESS_STAG")){
            *src_type = SOC_TMC_CNT_SRC_TYPE_STAG;
            *command_id = 1;
            *we_val = 1;
        }
        else if (IS_STR_LSB0(propval, "INGRESS_VSQ")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_VSQ;
            *command_id = 0;
            *we_val = 0;
        }
        else if (SOC_IS_JERICHO(unit) && IS_STR_LSB1(propval, "INGRESS_VSQ")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_VSQ;
            *command_id = 1;
            *we_val = 0;
        }
        else if (SOC_IS_JERICHO(unit) && IS_STR_MSB0(propval, "INGRESS_VSQ")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_VSQ;
            *command_id = 0;
            *we_val = 1;
        }
        else if (SOC_IS_JERICHO(unit) && IS_STR_MSB1(propval, "INGRESS_VSQ")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_VSQ;
            *command_id = 1;
            *we_val = 1;
        }
        else if (IS_STR_LSB0(propval, "INGRESS_CNM")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_CNM_ID;
            *command_id = 0;
            *we_val = 0;
        }
        else if (SOC_IS_JERICHO(unit) && IS_STR_LSB1(propval, "INGRESS_CNM")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_CNM_ID;
            *command_id = 1;
            *we_val = 0;
        }
        else if (SOC_IS_JERICHO(unit) && IS_STR_MSB0(propval, "INGRESS_CNM")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_CNM_ID;
            *command_id = 0;
            *we_val = 1;
        }
        else if (SOC_IS_JERICHO(unit) && IS_STR_MSB1(propval, "INGRESS_CNM")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_CNM_ID;
            *command_id = 1;
            *we_val = 1;
        }
        /*OAM*/
        else if (IS_STR_LSB0(propval, "EGRESS_OAM") && !SOC_IS_QAX(unit)){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EGRESS_OAM;
            *command_id = 0;
            *we_val = 0;
        }
        else if (IS_STR_MSB0(propval, "EGRESS_OAM") && !SOC_IS_QAX(unit)){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EGRESS_OAM;
            *command_id = 0;
            *we_val = 1;
        }
        else if (IS_STR_LSB0(propval, "INGRESS_OAM")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_INGRESS_OAM;
            *command_id = SOC_IS_QAX(unit) ? 0 : 1;
            *we_val = 0;
        }
        else if (IS_STR_MSB0(propval, "INGRESS_OAM")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_INGRESS_OAM;
            *command_id = SOC_IS_QAX(unit) ? 0 : 1;
            *we_val = 1;
        }
        else if (IS_STR_LSB1(propval, "INGRESS_OAM")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_INGRESS_OAM;
            *command_id = 1;
            *we_val = 0;
        }
        else if (IS_STR_MSB1(propval, "INGRESS_OAM")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_INGRESS_OAM;
            *command_id = 1;
            *we_val = 1;
        }        
        /*EPNI*/
        else if (IS_STR_LSB0(propval, "EGRESS_VSI")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EPNI;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_VSI;
            *command_id = 0;
            *we_val = 0;
        }
        else if (IS_STR_MSB0(propval, "EGRESS_VSI")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EPNI;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_VSI;
            *command_id = 0;
            *we_val = 1;
        }
        else if (IS_STR_LSB1(propval, "EGRESS_VSI")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EPNI;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_VSI;
            *command_id = 1;
            *we_val = 0;
        }
        else if (IS_STR_MSB1(propval, "EGRESS_VSI")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EPNI;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_VSI;
            *command_id = 1;
            *we_val = 1;
        }
        else if (IS_STR_LSB0(propval, "EGRESS_OUTLIF") || IS_STR_LSB0(propval, "EGRESS_OUT_LIF")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EPNI;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_OUTLIF;
            *command_id = 0;
            *we_val = 0;
        }
        else if (IS_STR_MSB0(propval, "EGRESS_OUTLIF") || IS_STR_MSB0(propval, "EGRESS_OUT_LIF")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EPNI;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_OUTLIF;
            *command_id = 0;
            *we_val = 1;
        }
        else if (IS_STR_LSB1(propval, "EGRESS_OUTLIF") || IS_STR_LSB1(propval, "EGRESS_OUT_LIF")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EPNI;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_OUTLIF;
            *command_id = 1;
            *we_val = 0;
        }
        else if (IS_STR_MSB1(propval, "EGRESS_OUTLIF") || IS_STR_MSB1(propval, "EGRESS_OUT_LIF")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EPNI;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_OUTLIF;
            *command_id = 1;
            *we_val = 1;
        }           
        else if (IS_STR_LSB0(propval, "EGRESS_TM")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EPNI;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_TM;
            *command_id = 0;
            *we_val = 0;
        }
        else if (IS_STR_MSB0(propval, "EGRESS_TM")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EPNI;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_TM;
            *command_id = 0;
            *we_val = 1;
        }
        else if (IS_STR_LSB1(propval, "EGRESS_TM")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EPNI;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_TM;
            *command_id = 1;
            *we_val = 0;
        }
        else if (IS_STR_MSB1(propval, "EGRESS_TM")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EPNI;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_TM;
            *command_id = 1;
            *we_val = 1;
        }
        else if (IS_STR_LSB0(propval, "EGRESS_TM_PORT")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EPNI;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_TM_PORT;
            *command_id = 0;
            *we_val = 0;
        }
        else if (IS_STR_MSB0(propval, "EGRESS_TM_PORT")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EPNI;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_TM_PORT;
            *command_id = 0;
            *we_val = 1;
        }
        else if (IS_STR_LSB1(propval, "EGRESS_TM_PORT")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EPNI;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_TM_PORT;
            *command_id = 1;
            *we_val = 0;
        }
        else if (IS_STR_MSB1(propval, "EGRESS_TM_PORT")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EPNI;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_TM_PORT;
            *command_id = 1;
            *we_val = 1;
        }
        /*ERPP*/
        else if (IS_STR_LSB0(propval, "EGRESS_RECEIVE_VSI")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EGR_PP;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_VSI;
            *command_id = 0;
            *we_val = 0;
        }
        else if (IS_STR_MSB0(propval, "EGRESS_RECEIVE_VSI")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EGR_PP;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_VSI;
            *command_id = 0;
            *we_val = 1;
        }
        else if (IS_STR_LSB0(propval, "EGRESS_RECEIVE_OUTLIF") || IS_STR_LSB0(propval, "EGRESS_RECEIVE_OUT_LIF")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EGR_PP;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_OUTLIF;
            *command_id = 0;
            *we_val = 0;
        }
        else if (IS_STR_MSB0(propval, "EGRESS_RECEIVE_OUTLIF") || IS_STR_MSB0(propval, "EGRESS_RECEIVE_OUT_LIF")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EGR_PP;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_OUTLIF;
            *command_id = 0;
            *we_val = 1;
        }
        else if (IS_STR_LSB0(propval, "EGRESS_RECEIVE_TM")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EGR_PP;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_TM;
            *command_id = 0;
            *we_val = 0;
        }
        else if (IS_STR_MSB0(propval, "EGRESS_RECEIVE_TM")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EGR_PP;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_TM;
            *command_id = 0;
            *we_val = 1;
        }
        else if (IS_STR_LSB0(propval, "EGRESS_RECEIVE_TM_PORT")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EGR_PP;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_TM_PORT;
            *command_id = 0;
            *we_val = 0;
        }
        else if (IS_STR_MSB0(propval, "EGRESS_RECEIVE_TM_PORT")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EGR_PP;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_TM_PORT;
            *command_id = 0;
            *we_val = 1;
        }
        else if (IS_STR_LSB0(propval, "EGRESS_FIELD")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EGR_PP;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_PMF;
            *command_id = 0;
            *we_val = 0;
        }
        else if (IS_STR_MSB0(propval, "EGRESS_FIELD")){
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EGR_PP;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_PMF;
            *command_id = 0;
            *we_val = 1;
        }
        else if (IS_STR_LSB1(propval, "EGRESS_FIELD")) {
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EGR_PP;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_PMF;
            *command_id = 1;
            *we_val = 0;
        }
        else if (IS_STR_MSB1(propval, "EGRESS_FIELD")) {
            *src_type  = SOC_TMC_CNT_SRC_TYPE_EGR_PP;
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_PMF;
            *command_id = 1;
            *we_val = 1;
        }
        else if (SOC_IS_JERICHO(unit) && IS_STR_LSB0(propval, "INGRESS_LATENCY")){
            *src_type  = SOC_TMC_CNT_SRC_TYPES_IPT_LATENCY;
            *command_id = 0;
            *we_val = 0;
        }
        else if (SOC_IS_JERICHO(unit) && IS_STR_MSB0(propval, "INGRESS_LATENCY")){
            *src_type  = SOC_TMC_CNT_SRC_TYPES_IPT_LATENCY;
            *command_id = 0;
            *we_val = 1;
        }        
        else  {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("Unexpected property value (\"%s\") for %s\n\r"),propval, propkey));  
        }

        if ((IS_STR_LSB0(propval, "INGRESS_FIELD")) || (IS_STR_LSB1(propval, "INGRESS_FIELD")) || 
            (IS_STR_MSB0(propval, "INGRESS_FIELD")) || (IS_STR_MSB1(propval, "INGRESS_FIELD"))){
            *eg_type  = SOC_TMC_CNT_MODE_EG_TYPE_PMF;
        } 
        if ((IS_STR_LSB0(propval, "INGRESS_VSI")) || (IS_STR_LSB1(propval, "INGRESS_VSI")) || 
            (IS_STR_MSB0(propval, "INGRESS_VSI")) || (IS_STR_MSB1(propval, "INGRESS_VSI"))){
            *eg_type = SOC_TMC_CNT_MODE_EG_TYPE_VSI;
        }
        if ((IS_STR_LSB0(propval, "INGRESS_IN_LIF")) || (IS_STR_LSB1(propval, "INGRESS_IN_LIF")) || 
            (IS_STR_MSB0(propval, "INGRESS_IN_LIF")) || (IS_STR_MSB1(propval, "INGRESS_IN_LIF"))){
            *eg_type  = SOC_TMC_CNT_MODE_EG_TYPE_OUTLIF;
        } 
    }
    
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_soc_dpp_str_prop_parse_counter_format (int unit, soc_port_t port, SOC_TMC_CNT_FORMAT *p_val)
{
    char *propkey, *propval;
    BCMDNX_INIT_FUNC_DEFS;

    propkey = spn_COUNTER_ENGINE_FORMAT;
    *p_val = SOC_TMC_CNT_FORMAT_PKTS_AND_BYTES;

    propval = soc_property_port_get_str(unit, port, propkey);    
    if (propval == NULL) {
        BCM_EXIT;
    }

    if ((!propval) || (sal_strcmp(propval, "PACKETS") == 0)) {
        *p_val = SOC_TMC_CNT_FORMAT_PKTS;
    } else if (sal_strcmp(propval, "BYTES") == 0) {
        *p_val = SOC_TMC_CNT_FORMAT_BYTES;
    } else if (sal_strcmp(propval, "PACKETS_AND_BYTES") == 0) {
        *p_val = SOC_TMC_CNT_FORMAT_PKTS_AND_BYTES;
    } else if (sal_strcmp(propval, "MAX_QUEUE_SIZE") == 0) {
        *p_val = SOC_TMC_CNT_FORMAT_MAX_QUEUE_SIZE;
    } else if (sal_strcmp(propval, "LATENCY") == 0) {
        *p_val = SOC_TMC_CNT_FORMAT_IPT_LATENCY;        
    }else if ((SOC_IS_ARADPLUS(unit)) && (sal_strcmp(propval, "PACKETS_AND_PACKETS") == 0)){
        *p_val = SOC_TMC_CNT_FORMAT_PKTS_AND_PKTS;
    } else  {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("Unexpected property value (\"%s\") for %s\n\r"),propval, propkey));

    }
exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_soc_dpp_str_prop_parse_counter_mode(int unit, soc_port_t port, SOC_TMC_CNT_SRC_TYPE src_type, SOC_TMC_CNT_MODE_STATISTICS *stat)
{
   char *propkey, *propval;
   char             *end_c;


    BCMDNX_INIT_FUNC_DEFS;

    propkey = spn_COUNTER_ENGINE_STATISTICS;
  
    *stat = (SOC_TMC_CNT_SRC_TYPE_EGR_PP == src_type) ? SOC_TMC_CNT_MODE_STATISTICS_FWD_NO_COLOR : SOC_TMC_CNT_MODE_STATISTICS_NO_COLOR;



    propval = soc_property_port_get_str(unit, port, propkey);    

    if (propval == NULL) {
        BCM_EXIT;
    }

    if (isdigit((int)(propval[0]))) {
        *stat = sal_ctoi(propval, &end_c);

        if (*stat > SOC_TMC_CNT_NOF_MODE_INGS_ARAD) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("Unexpected property value (\"%d\") for %s\n\r"),*stat, propkey));  

        }   
    } else {
        if ((sal_strcmp(propval, "FWD_COLOR") == 0))  { /* backward support */
            *stat = SOC_TMC_CNT_MODE_STATISTICS_COLOR_RES_ENQ_HI;
        } else if ((sal_strcmp(propval, "SIMPLE_COLOR") == 0))  {
            *stat = SOC_TMC_CNT_MODE_STATISTICS_COLOR_RES_ENQ_HI;
        } else if (sal_strcmp(propval, "FWD_DROP") == 0) {
            *stat = SOC_TMC_CNT_MODE_STATISTICS_NO_COLOR;
        } else if (sal_strcmp(propval, "GREEN_NOT_GREEN") == 0) {
            *stat = SOC_TMC_CNT_MODE_STATISTICS_COLOR_RES_LOW;
#ifdef BCM_88650_B0
        } else if ((sal_strcmp(propval, "FWD") == 0) && (SOC_IS_ARAD_B0_AND_ABOVE(unit))) {
            *stat = SOC_TMC_CNT_MODE_STATISTICS_FWD_NO_COLOR;
        } else if ((sal_strcmp(propval, "DROP") == 0) && (SOC_IS_ARAD_B0_AND_ABOVE(unit))) {
            *stat = SOC_TMC_CNT_MODE_STATISTICS_DROP_NO_COLOR;
#endif
        } else if (sal_strcmp(propval, "ALL") == 0) {
            *stat = SOC_TMC_CNT_MODE_STATISTICS_ALL_NO_COLOR;
        } else if (sal_strcmp(propval, "FULL_COLOR") == 0) {
            *stat = SOC_TMC_CNT_MODE_STATISTICS_COLOR_RES_HI;
        }
#ifdef BCM_88660_A0
        else if ((sal_strcmp(propval, "SIMPLE_COLOR_FWD") == 0) && (SOC_IS_ARADPLUS(unit))){
            *stat = SOC_TMC_CNT_MODE_STATISTICS_FWD_SIMPLE_COLOR;
        } else if ((sal_strcmp(propval, "SIMPLE_COLOR_DROP") == 0) && (SOC_IS_ARADPLUS(unit))) {
            *stat = SOC_TMC_CNT_MODE_STATISTICS_DROP_SIMPLE_COLOR;
        } else if ((sal_strcmp(propval, "CONFIGURABLE") == 0) && (SOC_IS_ARADPLUS(unit))) {
            *stat = SOC_TMC_CNT_MODE_STATISTICS_CONFIGURABLE_OFFSETS;
        }
#endif
        else  {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNIT, (_BSL_BCM_MSG("Unexpected statistics mode value (\"%s\") for %s\n\r"),propval, propkey));
        }
    }
exit:
    BCMDNX_FUNC_RETURN;
}


#ifdef BCM_88660_A0
STATIC int
_soc_dpp_str_prop_parse_counter_mapping(int unit, soc_port_t crps_index, uint32 *nof_counters, uint32 *entries_bmaps)
{
    uint32 propval;
    uint32 i;
    const char *offsets_propkeys[] = {spn_COUNTER_ENGINE_MAP_FWD_GREEN_OFFSET, spn_COUNTER_ENGINE_MAP_DROP_GREEN_OFFSET,
    spn_COUNTER_ENGINE_MAP_FWD_YELLOW_OFFSET, spn_COUNTER_ENGINE_MAP_DROP_YELLOW_OFFSET, spn_COUNTER_ENGINE_MAP_FWD_RED_OFFSET,
    spn_COUNTER_ENGINE_MAP_DROP_RED_OFFSET, spn_COUNTER_ENGINE_MAP_FWD_BLACK_OFFSET, spn_COUNTER_ENGINE_MAP_DROP_BLACK_OFFSET};
    const char *propkey;
    BCMDNX_INIT_FUNC_DEFS;


    /*verify map enabled*/
    propkey = spn_COUNTER_ENGINE_MAP_ENABLE;
    propval = soc_property_port_get(unit, crps_index, propkey, FALSE);
    if(!propval){
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("Unexpected value (\"%u\") for %s in configurable stat mode\n\r"),propval, propkey));
    }
    /*read num of entries*/
    propkey = spn_COUNTER_ENGINE_MAP_SIZE;
    propval = soc_property_port_get(unit, crps_index, propkey, 0);
    if (SOC_IS_JERICHO(unit)) {
        /*valid propval is 1-8*/
        if(propval > 8 || propval < 1) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("Unexpected value (\"%u\") for %s\n\r"),propval, propkey));
        }
    } else {
        if((propval != 1) && (propval != 2) && (propval != 4) && (propval != 5)){
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("Unexpected value (\"%u\") for %s\n\r"),propval, propkey));
        }
    }
    *nof_counters = propval;

    /*read the counters configuration*/
    for( i = 0 ; i < sizeof(offsets_propkeys)/sizeof(offsets_propkeys[0]); i++)
    {
       propval = soc_property_port_get(unit, crps_index, offsets_propkeys[i], SOC_TMC_CNT_BMAP_OFFSET_INVALID_VAL);    
       if ((propval >= *nof_counters) && (propval !=  SOC_TMC_CNT_BMAP_OFFSET_INVALID_VAL)){
           BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("Unexpected value (\"%u\") for %s\n\r"),propval, propkey));
       }
       SHR_BITSET(&(entries_bmaps[propval]), i) ;
   }

exit:
    BCMDNX_FUNC_RETURN;    
}
#endif

/****************************************************************************
 *
 *  External interface. It is used in the deinit procedure.
 * the flow of detaching is as follow:
    1. disconnect CRPS FIFO from DMA. this ensure that no more counters will be sent to DMA
    2. allow BG thread to collect the last counters that was copied to CPU. here assuming that all counters were copied to CPU.
    3. disable BG thread.
    4. stop CMIC-DMA work.
 */
int
bcm_dpp_counter_detach(int unit) {
    int rv;
    uint8  unit_allocated = FALSE;
    _bcm_dpp_counter_state_t *unitData;
    int channel_number = -1;
    int fifo;
    BCMDNX_INIT_FUNC_DEFS;

    if ((0 > unit) || (SOC_MAX_NUM_DEVICES <= unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNIT, (_BSL_BCM_MSG("unit is not valid")));
    }
    rv = COUNTERS_ACCESS.is_allocated(unit, &unit_allocated);
    BCMDNX_IF_ERR_CONT(rv);

    if (unit_allocated) {
        rv = _bcm_dpp_counter_unit_check_and_fill(unit, &unitData, COUNTER_UPDATE_SELECT_ALL, NULL);
        BCMDNX_IF_ERR_CONT(rv);
        if (rv == BCM_E_NONE) {
            
            if(SOC_IS_JERICHO(unit))
            {
                for (fifo = 0; fifo < SOC_DPP_DEFS_GET(unit, nof_counter_fifos); fifo++) 
                {
                    rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_mgmt_dma_fifo_channel_get, (unit, (dma_fifo_channel_src_t)fifo, &channel_number));
                    BCMDNX_IF_ERR_EXIT(rv);   
                    if(channel_number != -1)
                    {
                        /* Disonnect DMA engine from the CRPS. */
                        rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_mgmt_dma_fifo_channel_set, (unit, channel_number, dma_fifo_channel_src_reserved));                 
                        BCMDNX_IF_ERR_EXIT(rv); 
                    }
                }
                /* give 2 more seconds to the BG thread to empty the FIFO */
                sal_usleep(2000000);            
            }
            /* disable BG thread */
            rv = _bcm_dpp_counter_bg_thread_detach(unitData);
            BCMDNX_IF_ERR_CONT(rv);
            /* detach the DMA channel from CMIC */
            rv = _bcm_dpp_counter_dma_detach(unitData);
            BCMDNX_IF_ERR_CONT(rv);      
            
            if (COUNTERS_SYNC_ACCESS.cacheLock) {
                sal_mutex_destroy(COUNTERS_SYNC_ACCESS.cacheLock);
                COUNTERS_SYNC_ACCESS.cacheLock = NULL;
            }            
        }
    }

    if (_fifo_results[unit].cnt_result) {
        BCM_FREE(_fifo_results[unit].cnt_result);
    }

    if (_bcm_dpp_counter_state_buffer[unit].state) {
        if (_bcm_dpp_counter_state_buffer[unit].state->proc) {
            BCM_FREE(_bcm_dpp_counter_state_buffer[unit].state->proc);
        }
        BCM_FREE(_bcm_dpp_counter_state_buffer[unit].state);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
bcm_dpp_statistic_interface_config(int unit){
    char *propkey, *propval;
    SOC_TMC_CNT_MODE_EG_TYPE eg_type;
    SOC_TMC_CNT_SRC_TYPE src_type;
    uint32 soc_sandRes, command_id;
    int result = BCM_E_NONE;
    int core_index;
    BCMDNX_INIT_FUNC_DEFS;

    propkey = spn_COUNTER_ENGINE_SOURCE;

    /* read for each command id the ETPP source mode */
    for(command_id = 0; command_id < NUM_OF_COUNTERS_CMDS; command_id++) {
        /* read the key string and get the value string */
        propval = soc_property_suffix_num_str_get(unit, command_id, propkey, "egress_pp_stat");
        /* support old format */
        if (propval == NULL) {
            propval = soc_property_suffix_num_str_get(unit,command_id, propkey, "stat");                                  
            if(propval == NULL) {
                continue;
            }
        }        
        
        if (sal_strcmp(propval, "EGRESS_VSI") == 0) 
        {
            eg_type = SOC_TMC_CNT_MODE_EG_TYPE_VSI;
        } 
        else if (sal_strcmp(propval, "EGRESS_OUT_LIF") == 0)
        { 
            eg_type = SOC_TMC_CNT_MODE_EG_TYPE_OUTLIF;
        } 
        else if (sal_strcmp(propval, "EGRESS_TM") == 0)
        {
            eg_type = SOC_TMC_CNT_MODE_EG_TYPE_TM;
        } 
        else if (sal_strcmp(propval, "EGRESS_TM_PORT") == 0) 
        {
            eg_type = SOC_TMC_CNT_MODE_EG_TYPE_TM_PORT;
        } 
        else
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("Unexpected property value (\"%s\") for %s\n\r"),propval, propkey));  
        }       

        src_type = SOC_TMC_CNT_SRC_TYPE_EPNI;  

        BCM_DPP_CORES_ITER(BCM_CORE_ALL, core_index) {
            soc_sandRes = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_cnt_epni_regs_set,(unit, src_type, core_index, eg_type, command_id, FALSE));            
            result = handle_sand_result(soc_sandRes);
            BCMDNX_IF_ERR_EXIT(result);  
            /* unit check was added to satisfy the coverity */        
            BCM_DPP_UNIT_CHECK(unit);              
        }
    }       

    
    /* read for each command id the IRPP source mode*/
    for(command_id = 0; command_id < NUM_OF_COUNTERS_CMDS; command_id++) {
        /* read the key string and get the value string */
        propval = soc_property_suffix_num_str_get(unit, command_id, propkey, "ingress_pp_stat");   
        if(NULL == propval) {
            continue;
        }
        
        if (sal_strcmp(propval, "INGRESS_VSI") == 0)
        {
            eg_type = SOC_TMC_CNT_MODE_EG_TYPE_VSI;
        }
        else if (sal_strcmp(propval, "INGRESS_IN_LIF") == 0)
        {
            eg_type = SOC_TMC_CNT_MODE_EG_TYPE_OUTLIF;
        }
        else if (sal_strcmp(propval, "INGRESS_TM") == 0)
        {
            eg_type = SOC_TMC_CNT_MODE_EG_TYPE_TM;
        }
        else
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("Unexpected property value (\"%s\") for %s\n\r"),propval, propkey));  
        }        
        result = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_cnt_stif_ingress_pp_source_set,(unit, command_id, eg_type, 0));
        BCMDNX_IF_ERR_EXIT(result);    
    }


exit:
    BCMDNX_FUNC_RETURN;
}
STATIC int 
_soc_dpp_str_prop_parse_counter_voq(
       _bcm_dpp_counter_state_t *unitData, 
       soc_port_t crps_index,
       bcm_core_t src_core,
       SOC_TMC_CNT_SRC_TYPE src_type, 
       uint32 (*lastVoqMin)[MAX_NUM_OF_CORES], 
       uint32* lastVoqSize, 
       int *command_base_offset,
       SOC_TMC_CNT_Q_SET_SIZE *q_set_size)
{
    int unit = unitData->cunit;
    int result = BCM_E_NONE;
    unsigned int temp = 0;
    SOC_TMC_CNT_FORMAT lastVoqMinId;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(unitData);
    BCMDNX_NULL_CHECK(command_base_offset);
    BCMDNX_NULL_CHECK(q_set_size);
    
    *q_set_size = SOC_TMC_CNT_NOF_Q_SET_SIZES;
    *command_base_offset = 0;
    lastVoqMinId = unitData->proc[crps_index].mode.format;
    temp = soc_property_port_get(unit, crps_index, spn_COUNTER_ENGINE_VOQ_MIN_QUEUE, lastVoqMin[lastVoqMinId][src_core]);

    if ((lastVoqMinId != SOC_TMC_CNT_FORMAT_MAX_QUEUE_SIZE) && 
        ((temp < lastVoqMin[lastVoqMinId][src_core]) || (temp < lastVoqMin[SOC_TMC_CNT_FORMAT_PKTS_AND_BYTES][src_core]))) {
        LOG_ERROR(BSL_LS_BCM_CNT,
                  (BSL_META_U(unit, "unit %d counter processor %u VOQ start queue %u overlaps prior counter processor VOQ space\n"),
                                    unit, crps_index, temp));
        BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);
    } else if (SOC_DPP_DEFS_GET(unit, nof_queues) > temp) {
        lastVoqMin[lastVoqMinId][src_core] = temp;
        *command_base_offset = temp;
        temp = soc_property_port_get(unit, crps_index, spn_COUNTER_ENGINE_VOQ_QUEUE_SET_SIZE, lastVoqSize[src_core]);

        switch (temp) {
            case 1:
                *q_set_size = SOC_TMC_CNT_Q_SET_SIZE_1_Q;
                break;
            case 2:
                *q_set_size = SOC_TMC_CNT_Q_SET_SIZE_2_Q;
                break;
            case 3:
                LOG_WARN(BSL_LS_BCM_CNT,(BSL_META_U(unit, "unit %d counter processor %u VOQ queues per group %u is not valid; adjusting to %u\n"),
                                                    unit, crps_index, temp, 4));
                temp = 4;
                /* fallthrough intentional */
            case 4:
                *q_set_size  = SOC_TMC_CNT_Q_SET_SIZE_4_Q;
                break;
            case 5:
            case 6:
            case 7:
                LOG_WARN(BSL_LS_BCM_CNT, (BSL_META_U(unit,"unit %d counter processor %d VOQ queues per group %d is not valid; adjusting to %d\n"),
                                            unit, crps_index, temp, 8));
                temp = 8;
                /* fallthrough intentional */
            case 8:
                *q_set_size = SOC_TMC_CNT_Q_SET_SIZE_8_Q;
                break;
            default:
                LOG_ERROR(BSL_LS_BCM_CNT, (BSL_META_U(unit,"unit %d counter processor %u VOQ queues per group %u is not valid (max 8)\n"),
                                           unit, crps_index, temp));
                BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);
        }
        BCMDNX_IF_ERR_EXIT(result);
        
        lastVoqSize[src_core] = temp;
        LOG_DEBUG(BSL_LS_BCM_CNT, (BSL_META_U(unit,"unit %d proc %d VOQ start %u, queues per set %d\n"),
                                            unit, crps_index, *command_base_offset, temp));       
    } else {
        LOG_ERROR(BSL_LS_BCM_CNT, (BSL_META_U(unit, "unit %d counter processor %u VOQ start queue %u invalid\n"),
                                            unit, crps_index, temp));
        BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);
    }

    /* compute where this processor's queus space ends if VOQ mode */
    lastVoqMin[lastVoqMinId][src_core] += (unitData->proc[crps_index].mode.num_sets * (lastVoqSize[src_core]));
    lastVoqMin[SOC_TMC_CNT_FORMAT_PKTS_AND_BYTES][src_core] = 
        SOC_SAND_MAX(lastVoqMin[SOC_TMC_CNT_FORMAT_PKTS_AND_BYTES][src_core], SOC_SAND_MIN(lastVoqMin[SOC_TMC_CNT_FORMAT_PKTS][src_core], lastVoqMin[SOC_TMC_CNT_FORMAT_BYTES][src_core]));
    if (lastVoqMin[lastVoqMinId][src_core] > (SOC_DPP_DEFS_GET(unit, nof_queues) - 1)) {
        LOG_WARN(BSL_LS_BCM_CNT, (BSL_META_U(unit, "unit %d counter processor %u VOQ max %u exceeds supported hardware range; counters will be wasted.\n"),
                                  unit, crps_index, lastVoqMin[lastVoqMinId][src_core]));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int 
_soc_dpp_str_prop_parse_counter_stag(
   _bcm_dpp_counter_state_t *unitData, 
   soc_port_t crps_index, 
   SOC_TMC_CNT_SRC_TYPE src_type, 
   uint32* stag_lsb)
{
    int unit = unitData->cunit;
    unsigned int temp = 0;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(unitData);
    BCMDNX_NULL_CHECK(stag_lsb);
    
    if (unitData->global_info.haveStatTag) {
        LOG_ERROR(BSL_LS_BCM_CNT, (BSL_META_U(unit, "unit %d already has at least one processor in stat tag mode\n"),unit));
        BCMDNX_IF_ERR_EXIT(BCM_E_CONFIG);
    }
    LOG_DEBUG(BSL_LS_BCM_CNT,(BSL_META_U(unit, "unit %d proc %u source %s\n"), unit, crps_index, "SOC_TMC_CNT_SRC_TYPE_STAG"));
    temp = soc_property_port_get(unit, crps_index, spn_COUNTER_ENGINE_STAG_LOW_BIT, 0);
    if (temp >= 16) {
        LOG_ERROR(BSL_LS_BCM_CNT, (BSL_META_U(unit, "unit %d counter processor %u statistics tag lsb %u not valid\n"), unit, crps_index, temp));
        BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);
    }
    *stag_lsb = temp;
    LOG_DEBUG(BSL_LS_BCM_CNT, (BSL_META_U(unit, "unit %d proc %d stag lsb %u\n"), unit, crps_index, temp));        
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *     bcm_dpp_counter_lif_local_profile_get
 * Purpose:
 *      Get the profile id as stored in LIF-MGMT DB according to the user profile id
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     int counting_profile     - (IN) user profile id
 *     unit8 * local_counting_profile_id - (OUT) LIF-MGMT profile id
 * Returns: 
 *     BCM_E_NONE       - If was succesful.
 *     BCM_E_*          - Otherwise.
 */
int 
bcm_dpp_counter_lif_local_profile_get(int unit, int counting_profile, uint8 *local_counting_profile_id)
{
    BCMDNX_INIT_FUNC_DEFS

    if(counting_profile >= (SOC_TMC_CNT_LIF_COUNTING_MAX_NOF_RANGES_PER_SOURCE + SOC_TMC_CNT_LIF_COUNTING_MIN_NOF_RANGES_PER_SOURCE))
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("counting_profile=%d out of range\n"), counting_profile));
    }
    if(counting_profile == BCM_STAT_LIF_COUNTING_PROFILE_NONE)
    {
        *local_counting_profile_id = BCM_STAT_LIF_COUNTING_PROFILE_NONE;
    }
    else
    {
        if(lif_profile_mapping_table[unit][counting_profile] == BCM_STAT_LIF_COUNTING_PROFILE_NONE)
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("counting_profile=%d could not found in DB\n"), counting_profile));    
        }
        else
        {
            * local_counting_profile_id = lif_profile_mapping_table[unit][counting_profile];
        }    
    }
exit:
    BCMDNX_FUNC_RETURN;       
}


/*
 * Function:
 *     bcm_dpp_counter_lif_range_is_allocated
 * Purpose:
 *      Get if a counting profile was allocated 
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     is_allocated     - (OUT) if a counting profile was allocated
 * Returns: 
 *     BCM_E_NONE       - If was succesful.
 *     BCM_E_*          - Otherwise.
 */
int 
bcm_dpp_counter_lif_range_is_allocated(int unit, uint8* is_allocated) {
    int range_id = 0;
    int is_ingress = 0;
    int result = BCM_E_NONE;
    _bcm_dpp_counter_state_t *unitData;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(is_allocated);
    *is_allocated = FALSE;
    if (!_bcm_dpp_counter_state_buffer[unit].state) {
        /*If the module is not allocated: Don't return of print an error - return *is_allocated = FALSE;*/
        BCM_EXIT;
    }
    result = _bcm_dpp_counter_unit_check_and_fill(unit, &unitData, COUNTER_UPDATE_SELECT_ALL, NULL);
    /*If result != BCM_E_NONE: Don't return an error - return *is_allocated = FALSE;*/
    if (result != BCM_E_NONE) {
        BCM_EXIT;
    }
    for (is_ingress = 0; is_ingress < SOC_TMC_CNT_LIF_COUNTING_NOF_SOURCES && *is_allocated != TRUE; is_ingress++) {
        for (range_id = 0; range_id < SOC_TMC_CNT_LIF_COUNTING_NOF_RANGES_PER_SRC(unit) && *is_allocated != TRUE; range_id++) {
            if (unitData->lif_ranges[is_ingress][range_id].counting_profile != BCM_STAT_LIF_COUNTING_PROFILE_NONE) {
                *is_allocated = TRUE;
            }
        }
    }
exit:
    BCMDNX_FUNC_RETURN;
}
/*
 * Function:
 *     bcm_dpp_counter_lif_range_source_is_allocated
 * Purpose:
 *      Get if a specific rangeXsource for LIF-counting was allocated 
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     source           - (IN) Block - IRPP or ETPP
 *     range_id         - (IN) LIF-counting-range in the block
 *     is_allocated     - (OUT) if a counting profile was allocated
 * Returns: 
 *     BCM_E_NONE       - If was succesful.
 *     BCM_E_*          - Otherwise.
 */
int 
bcm_dpp_counter_lif_range_source_is_allocated(int unit, SOC_TMC_CNT_SRC_TYPE source, int range_id, uint8* is_allocated) {
    int is_ingress = (source == SOC_TMC_CNT_SRC_TYPE_ING_PP);
    _bcm_dpp_counter_state_t *unitData;
    int result = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(is_allocated);
    if (source != SOC_TMC_CNT_SRC_TYPE_ING_PP && source != SOC_TMC_CNT_SRC_TYPE_EPNI) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid source type %d."), source));
    }
    if (range_id < 0 || range_id >= SOC_TMC_CNT_LIF_COUNTING_NOF_RANGES_PER_SRC(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid command ID %d."), range_id));
    }
    result = _bcm_dpp_counter_unit_check_and_fill(unit, &unitData, COUNTER_UPDATE_SELECT_ALL, NULL);
    BCMDNX_IF_ERR_EXIT(result);
    *is_allocated = (unitData->lif_ranges[is_ingress][range_id].counting_profile != BCM_STAT_LIF_COUNTING_PROFILE_NONE);

exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * Function:
 *      bcm_dpp_counter_lif_range_get
 * Purpose:
 *      Get A LIF-counting-profile mapping to source X command-ID, and LIF-range.
 */
int 
bcm_dpp_counter_lif_range_get(
   int                  unit,
   int                  counting_profile,
   SOC_TMC_CNT_SRC_TYPE *source,
   int                  *command_id,
   int                  *stif_id,
   SOC_SAND_U32_RANGE   *range,
   uint32               *offset,
   int                  *is_double_entry,
   int                  *engine_range_offset
   ) 
{
    int result = BCM_E_NONE;
    _bcm_dpp_counter_state_t *unitData;
    int range_index = 0, size = 0, base = 0;
    int ptr_base = 0;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(range);
    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unsupported API for device type.")));
    }

    if (counting_profile < 0 || counting_profile >= SOC_TMC_CNT_LIF_COUNTING_NOF_PROFILES(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid LIF counting profile %d."), counting_profile));
    }

    result = _bcm_dpp_counter_unit_check_and_fill(unit, &unitData, COUNTER_UPDATE_SELECT_ALL, NULL);
    BCMDNX_IF_ERR_EXIT(result);

    if(lif_profile_mapping_table[unit][counting_profile] == BCM_STAT_LIF_COUNTING_PROFILE_NONE)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("LIF counting profile %d not configured."), counting_profile));    
    }
    
    if(counting_profile == unitData->lif_ranges[COUNTER_OUT_LIF_SOURCE_INDEX][lif_profile_mapping_table[unit][counting_profile]].counting_profile)
    {
        *source = SOC_TMC_CNT_SRC_TYPE_EPNI;   
        range_index = lif_profile_mapping_table[unit][counting_profile];              
    }
    else if((lif_profile_mapping_table[unit][counting_profile] < SOC_TMC_CNT_LIF_COUNTING_MIN_NOF_RANGES_PER_SOURCE) && 
            (counting_profile == unitData->lif_ranges[COUNTER_IN_LIF_SOURCE_INDEX][lif_profile_mapping_table[unit][counting_profile]].counting_profile))
    {
        *source = SOC_TMC_CNT_SRC_TYPE_ING_PP;    
        range_index = lif_profile_mapping_table[unit][counting_profile];              
    }            
    else
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("LIF counting profile %d not configured."), counting_profile));                
    }

    *command_id = range_index;
    *offset = 0;
    *engine_range_offset = 0;    
    /* get the range from DB and not from HW, according to the range, we get the other parameters */
   
    if(*source == SOC_TMC_CNT_SRC_TYPE_EPNI && SOC_IS_JERICHO_PLUS(unit))
    {
        *stif_id = -1;    
        result = _bcm_dpp_am_local_outlif_counting_profile_get(unit, lif_profile_mapping_table[unit][counting_profile], &base, &size, is_double_entry);
        BCMDNX_IF_ERR_EXIT(result);        
        range->start = (uint32)base;
        range->end = (uint32)(base + size - 1);
        result = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_cnt_out_lif_counting_range_get, (unit, range, offset, command_id, stif_id, is_double_entry));
        BCMDNX_IF_ERR_EXIT(result);   

        /* in qax, need to update the engine_range_offset, which align the profile range to the engine range. */
        /* first calc the base counter pointer and than, find the offset between the base pointer and the profile start range */
        if(SOC_IS_QAX(unit))
        {
            ptr_base = ((range->start>>(*is_double_entry)) + (*offset)) % (1<<SOC_TMC_CNT_QAX_OUT_LIF_CNT_PTR_SIZE);
            *engine_range_offset = ptr_base - range->start;
        }
    }
    else
    {
        result = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_cnt_lif_counting_range_get, (unit, *source, range_index, range));
        BCMDNX_IF_ERR_EXIT(result);        
    }
exit:
    BCMDNX_FUNC_RETURN;
}


/***********************************************************************************************************************
 * Function:
 *      bcm_dpp_counter_out_lif_engine_range_check
 * Purpose:
 *     verify that the range of the profile has corresponding range in the counter processor. (we check only the start and the end points)
 * Parameters:
 *      unit -  unit number
 *      command_id
 *      is_double_entry
 *      range
 *      engine_range_offset (negative or positive offset to adjust the profile range to the engine range)
 * Returns:
 *      SOC_E_XXX
 * Notes:
 */
STATIC int 
bcm_dpp_counter_out_lif_engine_range_check(
   _bcm_dpp_counter_state_t *unitData,
   int                  command_id,
   int                  is_double_entry,      
   SOC_SAND_U32_RANGE   *range,
   int engine_range_offset
   ) 
{
    int unit = unitData->cunit;  
    int crps_index, core_id, start_range_match, end_range_match, size, base;
    uint32 start_engine_counter, end_engine_counter;
    int engine_configured;
    int valid_check;
    BCMDNX_INIT_FUNC_DEFS;

    if(command_id != -1)
    {
        start_range_match = FALSE;
        end_range_match = FALSE;    
        valid_check = TRUE;        
        engine_configured = FALSE;        
        SOC_DPP_CORES_ITER(SOC_CORE_ALL, core_id)
        {
            for(crps_index = 0; crps_index < unitData->num_counter_procs; crps_index++)
            {      
                BCM_DPP_COUNTER_SOURCE_MATCH(unitData, crps_index, core_id, SOC_TMC_CNT_SRC_TYPE_EPNI, command_id, SOC_TMC_CNT_MODE_EG_TYPE_OUTLIF)                
                {                    
                    engine_configured = TRUE;                
                    start_engine_counter = (unitData->proc[crps_index].mode.we_val*
                                            SOC_TMC_NOF_COUNTERS_IN_ENGINE(unit, unitData->proc[crps_index].proc_id))*
                                            ((unitData->proc[crps_index].mode.format == SOC_TMC_CNT_FORMAT_PKTS_AND_PKTS) ? 2 : 1);
                    end_engine_counter = ((unitData->proc[crps_index].mode.we_val + 1)*
                                            SOC_TMC_NOF_COUNTERS_IN_ENGINE(unit, unitData->proc[crps_index].proc_id))*
                                            ((unitData->proc[crps_index].mode.format == SOC_TMC_CNT_FORMAT_PKTS_AND_PKTS) ? 2 : 1) - 1;
                    /* find engine that its range match to the start range of the profile + engine_range_offset */
                    base = (range->start + engine_range_offset)*unitData->proc[crps_index].mode.custom_mode_params.set_size ;
                    if((base >= start_engine_counter) &&
                       (base <= end_engine_counter))
                    {
                       start_range_match = TRUE;
                    }
                    /* find engine that its range match to the end range of the profile */    
                    /* if double entry, the range will be short by half */
                    size = ((range->end - range->start)>>is_double_entry)*unitData->proc[crps_index].mode.custom_mode_params.set_size;
                    if(((base + size) >= start_engine_counter) &&
                       (((base + size) + unitData->proc[crps_index].mode.custom_mode_params.set_size - 1) <= end_engine_counter))
                    {
                       end_range_match = TRUE;
                    }                   
                    /** if the engines were configured by SOc properties, the check is invalid, 
                        because the engines offset will be change according to this api range */
                    if (!(unitData->proc[crps_index].valid & BCM_DPP_COUNTER_PROC_INFO_CONFIGURED_DYNAMICALLY)) 
                    {    
                        valid_check = FALSE;
                    }
                }                
            }
        }
        /* if at least one engine was configure to count out lif, we verify that the range is match */
        if((valid_check == TRUE) && (engine_configured == TRUE) && 
           (start_range_match == FALSE || end_range_match == FALSE))
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, 
                (_BSL_BCM_MSG("OUT LIF profile range {%d..%d}, command_id=%d, is_double_entry=%d has no corresponding range\n"), 
                range->start, range->end, command_id, is_double_entry));          
        }
        /* if not even one engine configure to count out lif - error */
        if(engine_configured == FALSE)
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, 
                (_BSL_BCM_MSG("OUT LIF profile set called, but no engine was configure to count out lifs")));                      
        }
    }
   
exit:
    BCMDNX_FUNC_RETURN;
}


/* find the next available place in the lif_ranges table for out lif */
/* in case that profile exist, need to use the same index */
STATIC int _bcm_dpp_outlif_range_id_set (int unit, _bcm_dpp_counter_state_t * unit_data, int counting_profile, int * range_id)
{
    int i;
    BCMDNX_INIT_FUNC_DEFS;
    for(i = 0; i < SOC_TMC_CNT_LIF_COUNTING_MAX_NOF_RANGES_PER_SOURCE; i++)
    {
        if(unit_data->lif_ranges[COUNTER_OUT_LIF_SOURCE_INDEX][i].counting_profile == BCM_STAT_LIF_COUNTING_PROFILE_NONE)
        {
            break;
        }
        else if(unit_data->lif_ranges[COUNTER_OUT_LIF_SOURCE_INDEX][i].counting_profile == counting_profile)
        {
            break;
        }
    }
    if(i == SOC_TMC_CNT_LIF_COUNTING_MAX_NOF_RANGES_PER_SOURCE)
    {
        LOG_ERROR(BSL_LS_BCM_CNT, (BSL_META_U(unit, "can't allocate new outlif profile.\n")));
        BCMDNX_IF_ERR_EXIT(BCM_E_RESOURCE);   
    }
    else
    {
        *range_id = i;
    }
    
exit:
    BCMDNX_FUNC_RETURN;

}


/* 
 *  
 * Function:
 *      bcm_petra_stat_lif_counting_profile_set
 * Purpose:
 *      Set A LIF-counting-profile mapping to source X command-ID, and LIF-range.
 */
int 
bcm_dpp_counter_lif_range_set(
   int                  unit,
   int                  counting_profile,
   SOC_TMC_CNT_SRC_TYPE source,
   int                  command_id,
   int                  stif_counter_id,   
   int                  is_double_entry,      
   SOC_SAND_U32_RANGE   *range,
   int engine_range_offset
   ) {
    int result = BCM_E_NONE;
    _bcm_dpp_counter_state_t *unitData;
    int range_index, index;
    soc_port_t crps_index;
    SOC_SAND_U32_RANGE other_counting_range = {0};
    int is_ingress = (source == SOC_TMC_CNT_SRC_TYPE_ING_PP);
    int nof_resereved_lifs;
    int range_id = 0, base, size, temp, in_lif_profile;
    uint32 nof_local_lifs;
    BCMDNX_INIT_FUNC_DEFS;

    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unsupported API for device type.")));
    }
    if (source != SOC_TMC_CNT_SRC_TYPE_ING_PP && source != SOC_TMC_CNT_SRC_TYPE_EPNI) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid source type %d."), source));
    }
    if (counting_profile < 0 || counting_profile >= SOC_TMC_CNT_LIF_COUNTING_NOF_PROFILES(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid LIF counting profile %d."), counting_profile));
    }
    BCMDNX_NULL_CHECK(range);

    
    if(is_ingress)
    {
        nof_resereved_lifs = BCM_DPP_AM_LOCAL_IN_LIF_NOF_RESERVED_LIFS;
        nof_local_lifs = SOC_DPP_DEFS_GET(unit, nof_local_lifs);        
    }
    else
    {
        uint32 bank_id;
        nof_resereved_lifs = SOC_IS_JERICHO_PLUS(unit) ? 0 : SOC_DPP_CONFIG(unit)->l3.nof_rifs;
        /*
         * due to the rserved bits, local outlif value might bigger than physical (eedb lines * 2)
         * get maximum local outlif per sw state bank id and bank offset
         ------------------------------------------------------------------
         |                 |            |              |                  |
         |  bank_id[5:1]   |  rsv bits  |  bank_id[0]  |   offset         |
         |                 |            |              |                  |
         ------------------------------------------------------------------
         */
        /* get maximum sw_state bank_id */
        bank_id = _BCM_DPP_AM_EGRESS_LIF_NOF_EEDB_HALF_BANKS(unit);
        /* get maximum local outlif value */
        nof_local_lifs = ((bank_id >> 1) << (SOC_DPP_DEFS_GET(unit, nof_eg_encap_rsvs) + SOC_DPP_DEFS_GET(unit, nof_eg_encap_lsbs) + 1));
        nof_local_lifs += ((bank_id & 1) <<  SOC_DPP_DEFS_GET(unit, nof_eg_encap_lsbs));

        /* from JER+, Out-RIF DB is moved out of EDB_EEDB_BANK, but still share the same index space, so take consideration of nof_rifs here */
        if (SOC_IS_JERICHO_PLUS(unit)) {
            nof_local_lifs += SOC_DPP_CONFIG(unit)->l3.nof_rifs;
        }
    }
    if ((range->start > range->end) || 
        (range->start < nof_resereved_lifs) || 
        (range->end >= nof_local_lifs)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid LIF-counting range %d-%d."), range->start, range->end));
    }
    result = _bcm_dpp_counter_unit_check_and_fill(unit, &unitData, COUNTER_UPDATE_SELECT_ALL, NULL);
    BCMDNX_IF_ERR_EXIT(result);

    /** due to SW limitation, counter_set_size=3,5,6,7 is not supported for lif counting. the reason for that is that LIF start
        range is not zero. and it change the range of the engine.
        THe driver don't know to handle start!=0 and counter_set=3,5,6,7. 
        The reason for it is that driver, will add ofsset (base_val) to the ptr and increament the HW we_val. 
         but the SW we_val remain the same. Once the engine_size%counter_set_size !=0, there will be a set that split between two 
         consecutive engine and the first set in the next engine is shifted and it depend on the we_val value. 
         Therefore, when reading the counter we do not aware on the exact shift 
         In addition, the first set that the user wants to count will not be fully counted. since we using regular arithmetic operations,
         the first full set in the engine is represent start+1.*/    
    for (crps_index = 0; crps_index < unitData->num_counter_procs; crps_index++) 
    {
        BCM_DPP_COUNTER_SOURCE_MATCH(unitData, crps_index, BCM_CORE_ALL, source, command_id, SOC_TMC_CNT_MODE_EG_TYPE_OUTLIF) 
        {
            if( (unitData->proc[crps_index].mode.custom_mode_params.set_size == 3 || 
                  unitData->proc[crps_index].mode.custom_mode_params.set_size == 5 ||
                  unitData->proc[crps_index].mode.custom_mode_params.set_size == 6 || 
                  unitData->proc[crps_index].mode.custom_mode_params.set_size == 7) &&
                  range->start != 0)
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("LIF range.start=%d, counter_set_size=%d. counter_set_size=%d is allowed only when range.start=0 \n"),
                    range->start, unitData->proc[crps_index].mode.custom_mode_params.set_size, unitData->proc[crps_index].mode.custom_mode_params.set_size));
            }
        }
    }

    if(SOC_IS_JERICHO_PLUS(unit) && (is_ingress == FALSE))
    {
         BCMDNX_IF_ERR_EXIT(_bcm_dpp_outlif_range_id_set(unit, unitData, counting_profile, &range_id));
    }
    else
    {
        range_id = command_id;
    }

    /** coverity issue */
    if(range_id < 0 || is_ingress < 0)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("one of the array indexes is negative. range_id=%d, is_ingress=%d"), range_id, is_ingress));        
    }

    /* verify user input parameters are correct */
    if(is_ingress == FALSE && SOC_IS_JERICHO_PLUS(unit))
    {
        /* check that the range starts in 4K resolution */
        if(range->start % 4096 != 0)
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("range= {%d,%d}, must be in 4K resolution"), range->start, range->end));            
        }
        /* check that range is not overlapping with existing range (unless it is the same profile - allowed run over)*/
        for(range_index = 0; range_index < (SOC_TMC_CNT_LIF_COUNTING_MAX_NOF_RANGES_PER_SOURCE + SOC_TMC_CNT_LIF_COUNTING_MIN_NOF_RANGES_PER_SOURCE); range_index++)
        {
            in_lif_profile = FALSE;
            if ((lif_profile_mapping_table[unit][range_index] != BCM_STAT_LIF_COUNTING_PROFILE_NONE) && (range_index != counting_profile))
            {
                /* We need to check only overlapping between outlif profiles. check that it is not profile that belong to in lif.*/
                for (index = 0; index < SOC_TMC_CNT_LIF_COUNTING_MIN_NOF_RANGES_PER_SOURCE; index++) 
                {       
                    if(unitData->lif_ranges[COUNTER_IN_LIF_SOURCE_INDEX][index].counting_profile == range_index)
                    {         
                        in_lif_profile = TRUE;
                        break;
                    }
                }
                if(in_lif_profile == FALSE)
                {
                    result = _bcm_dpp_am_local_outlif_counting_profile_get(unit, lif_profile_mapping_table[unit][range_index], &base, &size, &temp);
                    BCMDNX_IF_ERR_EXIT(result);    
                    if( (base >= range->start && base <= range->end) || 
                        ((base + size - 1) >= range->start && (base + size - 1) <= range->end) ||
                        (range->start >= base && range->start <= (base + size - 1)) ||
                        (range->end >= base && range->end <= (base + size - 1)) )
                    {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("overlapping range between profile %d {range:%d..%d} and profile %d {range:%d..%d}"), 
                            range_index, base, (base + size - 1), counting_profile, range->start, range->end));
                    }                    
                }
            }
        }
    }
    else
    {        
        /*Check that the range is not in use*/
        if (unitData->lif_ranges[is_ingress][range_id].counting_profile != BCM_STAT_LIF_COUNTING_PROFILE_NONE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("LIF counting range %d for source %s is allready in use."), is_ingress, SOC_TMC_CNT_SRC_TYPE_to_string(source)));
        }     
        /*Check that the range does not overlap with the other range, if is enabled. */
        if (unitData->lif_ranges[is_ingress][1 - range_id].counting_profile != BCM_STAT_LIF_COUNTING_PROFILE_NONE) {
            result = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_cnt_lif_counting_range_get, (unit, source, 1 - range_id, &other_counting_range));        
            BCMDNX_IF_ERR_EXIT(result);
        
            if ((other_counting_range.start <= range->start && other_counting_range.end >= range->start) || 
                (range->start <= other_counting_range.start && range->end >= other_counting_range.start)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Cannnot set LIF-counting range %d-%d, since it overlapps with the other range %d-%d"), 
                                                                range->start, range->end, other_counting_range.start,other_counting_range.end));
            }
        }        
        /*Check that the profile is not in use in other ranges*/
        for (range_index = 0; range_index < SOC_TMC_CNT_LIF_COUNTING_NOF_RANGES_PER_SRC(unit); range_index++) {
            if (counting_profile == unitData->lif_ranges[is_ingress][range_index].counting_profile) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("LIF counting profile %d for source %s is already in use."), counting_profile, SOC_TMC_CNT_SRC_TYPE_to_string(source)));
            }
        }        
    }

    /*LIF-allocation configuration -*/
    if (is_ingress) {
        result = _bcm_dpp_am_local_inlif_counting_profile_set(unit, range_id, range->start, (range->end - range->start) + 1);
        BCMDNX_IF_ERR_EXIT(result);
    } else {
        result = _bcm_dpp_am_local_outlif_counting_profile_set(unit, range_id, range->start, (range->end - range->start) + 1, is_double_entry);
        BCMDNX_IF_ERR_EXIT(result);        
    }

    /*Attach the counting profile to a LIF-counting-source*/    
    unitData->lif_ranges[is_ingress][range_id].counting_profile = counting_profile;
    lif_profile_mapping_table[unit][counting_profile] = range_id;

    if(SOC_IS_QAX(unit) && source == SOC_TMC_CNT_SRC_TYPE_EPNI)
    {
        BCMDNX_IF_ERR_EXIT(bcm_dpp_counter_out_lif_engine_range_check(unitData, command_id, is_double_entry, range, engine_range_offset));               
    }

    result = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_cnt_lif_counting_range_set, (unit, source, command_id, range, stif_counter_id, is_double_entry, engine_range_offset));
    BCMDNX_IF_ERR_EXIT(result);

    if(is_ingress == TRUE || !SOC_IS_JERICHO_PLUS(unit))
    {
        /*Update the LIF base vals*/
        for (crps_index = 0; crps_index < unitData->num_counter_procs; crps_index++) {
            BCM_DPP_COUNTER_SOURCE_MATCH(unitData, crps_index, BCM_CORE_ALL, source, range_id /*command_id == range_id*/, SOC_TMC_CNT_MODE_EG_TYPE_OUTLIF) {
                /* 
                 * When configuring the counter-engine dynamically we define a start and end of counter-sets. 
                 * So we need to update the we_val (MSB-pattern acording to the new base-value.
                 * When configuring the counter-engine using SOC properties we specify the we_val so there is no need to update it.
                 */
                if (unitData->proc[crps_index].valid & BCM_DPP_COUNTER_PROC_INFO_CONFIGURED_DYNAMICALLY) {
                    int base_delta = range->start - unitData->proc[crps_index].mode.mode_eg.base_val;
                    int we_val_delta = (base_delta / unitData->proc[crps_index].mode.num_sets);
                    /*Update we_val*/
                    if (we_val_delta > unitData->proc[crps_index].mode.we_val) {
                        BCMDNX_ERR_EXIT_MSG(
                           BCM_E_INTERNAL, (_BSL_BCM_MSG("LIF counting profile %d for source %s, engine %d misconfiguration, MSB-pattern %d."), 
                                            counting_profile, SOC_TMC_CNT_SRC_TYPE_to_string(source), crps_index, (int)unitData->proc[crps_index].mode.we_val - we_val_delta));
                    }
                    unitData->proc[crps_index].mode.we_val -= we_val_delta;
                }
                /*Update lif base val*/
                unitData->proc[crps_index].mode.mode_eg.base_val = range->start;
                result = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_cnt_base_val_set, (unit, crps_index, &unitData->proc[crps_index].mode));
                BCMDNX_IF_ERR_EXIT(result);
            }
        }    
    }
    result = _bcm_dpp_counter_unit_select_update(unit, COUNTER_UPDATE_SELECT_LIF_INFO | COUNTER_UPDATE_SELECT_PROC_INFO_ALL, unitData, NULL);
    BCMDNX_IF_ERR_EXIT(result);
    
exit:
    BCMDNX_FUNC_RETURN;
}

/*Get the LIF base val from HW - the range start */
int 
bcm_dpp_counter_lif_base_val_get(
   int                  unit,
   SOC_TMC_CNT_SRC_TYPE source,
   int                  command_id,
   int                  *base_val) 
{
    int result = BCM_E_NONE;
    _bcm_dpp_counter_state_t *unitData;
    SOC_SAND_U32_RANGE range;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(base_val);
    if (source != SOC_TMC_CNT_SRC_TYPE_ING_PP && source != SOC_TMC_CNT_SRC_TYPE_EPNI) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid source type %d."), source));
    }
    if (command_id < 0 || command_id >= SOC_TMC_CNT_LIF_COUNTING_NOF_RANGES_PER_SRC(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid command ID %d."), command_id));
    }

    result = _bcm_dpp_counter_unit_check_and_fill(unit, &unitData, COUNTER_UPDATE_SELECT_ALL, NULL);
    BCMDNX_IF_ERR_EXIT(result);

    result = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_cnt_lif_counting_range_get, (unit, source, command_id, &range));
    BCMDNX_IF_ERR_EXIT(result);
    *base_val = range.start;

exit:
    BCMDNX_FUNC_RETURN;
}
/*Get the LIF-stack-priority from SW*/
int 
bcm_dpp_counter_lif_counting_priority_get (
   int                                  unit, 
   SOC_TMC_CNT_SRC_TYPE                 source,
   int                                  range_id,
   bcm_stat_counter_lif_stack_id_t      lif_stack_level, 
   int                                  *priority)

{
    _bcm_dpp_counter_state_t *unitData;
    int is_ingress = (source == SOC_TMC_CNT_SRC_TYPE_ING_PP);
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(priority);
    if (source != SOC_TMC_CNT_SRC_TYPE_ING_PP && source != SOC_TMC_CNT_SRC_TYPE_EPNI) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid source type %d."), source));
    }
    if (range_id < 0 || range_id >=SOC_TMC_CNT_LIF_COUNTING_NOF_RANGES_PER_SRC(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid command ID %d."), range_id));
    }
    if (lif_stack_level >= BCM_STAT_COUNT_LIF_NUMBER_OF_STACK_IDS) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid LIF stack level %d."), lif_stack_level));
    }
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_counter_unit_check_and_fill(unit, &unitData, COUNTER_UPDATE_SELECT_ALL, NULL));
    *priority = unitData->lif_ranges[is_ingress][range_id].lif_stack_priority[lif_stack_level];

exit:
    BCMDNX_FUNC_RETURN;
}
/* 
 * Update what LIF-stack-to-count according to priorities define in SW
 */
STATIC int 
_bcm_dpp_counter_lif_counting_stack_level_map_update(
   _bcm_dpp_counter_state_t             *unitData,
   SOC_TMC_CNT_SRC_TYPE                 source,
   int                                  range_id) 
{
    int unit = unitData->cunit;
    int result = BCM_E_NONE;
    uint32 lif_counting_mask;
    int stack_level_to_count = bcmStatCounterLifStackId0, stack_level;
    int is_ingress = (source == SOC_TMC_CNT_SRC_TYPE_ING_PP);
    int max_lif_stack_to_count = SOC_TMC_CNT_LIF_COUNTING_NUMBER_OF_STACK_IDS(unitData->cunit, source);
    uint8 found = 0;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(unitData);
    if (SOC_IS_ARDON(unit) && source == SOC_TMC_CNT_SRC_TYPE_ING_PP) {
        BCM_EXIT;
    }
    if (source != SOC_TMC_CNT_SRC_TYPE_ING_PP && source != SOC_TMC_CNT_SRC_TYPE_EPNI) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid source type %d."), source));
    }
    if (range_id < 0 || range_id >=SOC_TMC_CNT_LIF_COUNTING_NOF_RANGES_PER_SRC(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid command ID %d."), range_id));
    }
    /*Configure HW according to the configured priority*/
    for (found = 0, stack_level_to_count = bcmStatCounterLifStackId0, lif_counting_mask = 0; 
         lif_counting_mask <= SOC_TMC_CNT_LIF_COUNTING_MASK_MAX(unitData->cunit, source);
         found = 0, stack_level_to_count = bcmStatCounterLifStackId0, lif_counting_mask++) {
        for (stack_level = 0; stack_level < max_lif_stack_to_count; stack_level++) {
            if (SOC_TMC_CNT_LIF_COUNTING_MASK_TO_FIELD_VAL(range_id) == SOC_TMC_CNT_LIF_COUNTING_MASK_FIELD_VAL_GET(unit, source, lif_counting_mask, stack_level)) {
                if (!found || 
                    (unitData->lif_ranges[is_ingress][range_id].lif_stack_priority[stack_level] >
                     unitData->lif_ranges[is_ingress][range_id].lif_stack_priority[stack_level_to_count])) {
                    stack_level_to_count = stack_level; /*If priority higher - replace LIF to count.*/
                }
                found = 1;
            }
        }
        if (!found) {
            stack_level_to_count = bcmStatCounterLifStackId0; /*If there is no match pass the outtermost LIF*/
        }
        if((!SOC_IS_QAX(unit) && !SOC_IS_JERICHO_PLUS_A0(unit)) || (source == SOC_TMC_CNT_SRC_TYPE_ING_PP))
        {
            result = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_cnt_lif_counting_set, (unit, source, range_id, lif_counting_mask, stack_level_to_count)); 
            BCMDNX_IF_ERR_EXIT(result);            
        }
    }
exit:
    BCMDNX_FUNC_RETURN;
}
/* 
 * Set the LIF-stack-priority from SW 
 * Update what LIF-stack-to-count acording to prioritoes define in SW
 */
int 
bcm_dpp_counter_lif_counting_priority_set(
   int                                  unit, 
   SOC_TMC_CNT_SRC_TYPE                 source,
   int                                  range_id,
   bcm_stat_counter_lif_stack_id_t      lif_stack_level, 
   int                                  priority)
{
    int result = BCM_E_NONE;
    _bcm_dpp_counter_state_t *unitData;
    int is_ingress = (source == SOC_TMC_CNT_SRC_TYPE_ING_PP);
    BCMDNX_INIT_FUNC_DEFS;
    if (source != SOC_TMC_CNT_SRC_TYPE_ING_PP && 
        source != SOC_TMC_CNT_SRC_TYPE_EPNI) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid source type %d."), source));
    }
    if (range_id < 0 || range_id >=SOC_TMC_CNT_LIF_COUNTING_NOF_RANGES_PER_SRC(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid command ID %d."), range_id));
    }
    if (lif_stack_level >= BCM_STAT_COUNT_LIF_NUMBER_OF_STACK_IDS) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid LIF stack level %d."), lif_stack_level));
    }
    /* for Jer A0 and below, an HW prevent counting Lif4 in EPNI */
    if (SOC_IS_JERICHO_B0_AND_ABOVE (unit) == FALSE)
    {
        if (source == SOC_TMC_CNT_SRC_TYPE_EPNI && lif_stack_level == bcmStatCounterLifStackId3) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Egress sources cannot configure priority for LIF-stack-3.")));
        }
    }
    if (priority < 0 || priority >= BCM_STAT_COUNT_LIF_NUMBER_OF_STACK_IDS) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid priority %d."), priority));
    }
    result = _bcm_dpp_counter_unit_check_and_fill(unit, &unitData, COUNTER_UPDATE_SELECT_ALL, NULL);
    BCMDNX_IF_ERR_EXIT(result);

    /*Update LIF priority*/
    unitData->lif_ranges[is_ingress][range_id].lif_stack_priority[lif_stack_level] = priority;

    /*Update HW*/
    result = _bcm_dpp_counter_lif_counting_stack_level_map_update(unitData, source, range_id);
    BCMDNX_IF_ERR_EXIT(result);

    result = _bcm_dpp_counter_unit_select_update(unit, COUNTER_UPDATE_SELECT_LIF_INFO, unitData, NULL);
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}


/* 
 * update profile_mapping_table accoring to the lif_range table.
 * the profile mapping table is array that the key is the profike ID and the value is the profile ID as stored in the LIF-MGMT DB.
 * MGMT DB hold one DB for inlif and one for out lif. 
 */
STATIC int 
_bcm_dpp_counter_profile_mapping_table_init(_bcm_dpp_counter_state_t *unitData) 
{
    int index;
    int unit = unitData->cunit;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(unitData);

    memset(lif_profile_mapping_table[unit], BCM_STAT_LIF_COUNTING_PROFILE_NONE, 
        (SOC_TMC_CNT_LIF_COUNTING_MAX_NOF_RANGES_PER_SOURCE + SOC_TMC_CNT_LIF_COUNTING_MIN_NOF_RANGES_PER_SOURCE)*sizeof(int));
    for (index = 0; index < SOC_TMC_CNT_LIF_COUNTING_MIN_NOF_RANGES_PER_SOURCE; index++) 
    {       
       if(unitData->lif_ranges[COUNTER_IN_LIF_SOURCE_INDEX][index].counting_profile != BCM_STAT_LIF_COUNTING_PROFILE_NONE)
       {
           if(unitData->lif_ranges[COUNTER_IN_LIF_SOURCE_INDEX][index].counting_profile >= (SOC_TMC_CNT_LIF_COUNTING_MAX_NOF_RANGES_PER_SOURCE + SOC_TMC_CNT_LIF_COUNTING_MIN_NOF_RANGES_PER_SOURCE))
           {
               LOG_ERROR(BSL_LS_BCM_CNT, (BSL_META_U(unit, "Lif profile out of range.\n")));
               BCMDNX_IF_ERR_EXIT(BCM_E_INTERNAL);   
           }
           else
           {
               lif_profile_mapping_table[unit][unitData->lif_ranges[COUNTER_IN_LIF_SOURCE_INDEX][index].counting_profile] = index; 
           }
       }
    }
    for (index = 0; index < SOC_TMC_CNT_LIF_COUNTING_MAX_NOF_RANGES_PER_SOURCE; index++) 
    {       
       if(unitData->lif_ranges[COUNTER_OUT_LIF_SOURCE_INDEX][index].counting_profile != BCM_STAT_LIF_COUNTING_PROFILE_NONE)
       {
           if(unitData->lif_ranges[COUNTER_OUT_LIF_SOURCE_INDEX][index].counting_profile >= (SOC_TMC_CNT_LIF_COUNTING_MAX_NOF_RANGES_PER_SOURCE + SOC_TMC_CNT_LIF_COUNTING_MIN_NOF_RANGES_PER_SOURCE))
           {
               LOG_ERROR(BSL_LS_BCM_CNT, (BSL_META_U(unit, "Lif profile out of range.\n")));
               BCMDNX_IF_ERR_EXIT(BCM_E_INTERNAL);           
           }
           else
           {
               lif_profile_mapping_table[unit][unitData->lif_ranges[COUNTER_OUT_LIF_SOURCE_INDEX][index].counting_profile] = index; 
           }           
       }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * Set all LIF-stack-priorities to be 0 
 * Set counting profile to none.
 * Update what LIF-stack-to-count to take outter-most-LIF
 */
STATIC int 
_bcm_dpp_counter_lif_counting_priority_default_set(_bcm_dpp_counter_state_t *unitData) {
    int unit = unitData->cunit;
    int range_id, is_ingress;
    int result = BCM_E_NONE;
    SOC_TMC_CNT_SRC_TYPE sources[SOC_TMC_CNT_LIF_COUNTING_NOF_SOURCES] = {SOC_TMC_CNT_SRC_TYPE_EPNI, SOC_TMC_CNT_SRC_TYPE_ING_PP};
    bcm_stat_counter_lif_stack_id_t lif_stack_level;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(unitData);

    for (is_ingress = 0; is_ingress < SOC_TMC_CNT_LIF_COUNTING_NOF_SOURCES; is_ingress++) {
        for (range_id = 0; range_id < SOC_TMC_CNT_LIF_COUNTING_MAX_NOF_RANGES_PER_SOURCE; range_id++) {
            for (lif_stack_level = 0; lif_stack_level < BCM_STAT_COUNT_LIF_NUMBER_OF_STACK_IDS; lif_stack_level++) {
                unitData->lif_ranges[is_ingress][range_id].lif_stack_priority[lif_stack_level] = 0; /*Set All priorities to 0 by default*/
            }
            unitData->lif_ranges[is_ingress][range_id].counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE;
        }
    }
    /*Set HW LIF-stack-level-mapping-allways take outtermost*/
    for (is_ingress = 0; is_ingress < SOC_TMC_CNT_LIF_COUNTING_NOF_SOURCES; is_ingress++) {
        for (range_id = 0; range_id < SOC_TMC_CNT_LIF_COUNTING_MIN_NOF_RANGES_PER_SOURCE; range_id++){
            result = _bcm_dpp_counter_lif_counting_stack_level_map_update(unitData, sources[is_ingress], range_id);
            BCMDNX_IF_ERR_EXIT(result);
        }
    }
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int 
_bcm_dpp_counter_source_config_get(
       _bcm_dpp_counter_state_t *unitData,
       soc_port_t crps_index, 
       int *src_core,
       SOC_TMC_CNT_SRC_TYPE* src_type, 
       int *command_id,
       int *command_base_offset, 
       SOC_TMC_CNT_MODE_EG_TYPE* eg_type,
       SOC_TMC_CNT_COUNTER_WE_BITMAP* we_val,
       SOC_TMC_CNT_Q_SET_SIZE* q_set_size) {
    int unit = unitData->cunit;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(unitData);
    BCMDNX_NULL_CHECK(src_type);
    BCMDNX_NULL_CHECK(src_core);
    BCMDNX_NULL_CHECK(command_id);
    BCMDNX_NULL_CHECK(command_base_offset);
    BCMDNX_NULL_CHECK(eg_type);
    BCMDNX_NULL_CHECK(we_val);
    BCMDNX_NULL_CHECK(q_set_size);

    *src_type = unitData->proc[crps_index].mode.src_type;
    *eg_type =  unitData->proc[crps_index].mode.mode_eg.type;
    *src_core = unitData->proc[crps_index].mode.src_core;
    *command_id = unitData->proc[crps_index].mode.command_id;
    *command_base_offset = unitData->proc[crps_index].mode.mode_eg.base_val;
    *we_val = unitData->proc[crps_index].mode.we_val;
    if (unitData->proc[crps_index].mode.src_type == SOC_TMC_CNT_SRC_TYPE_VOQ) {
        *q_set_size = unitData->proc[crps_index].mode.q_set_size;
    } else {
        *q_set_size = 1;
    }
exit:
    BCMDNX_FUNC_RETURN;
}
int 
bcm_dpp_counter_stag_lsb_get(
    int unit,
    uint32* stag_lsb,
    uint8* enable) {
    int result = BCM_E_NONE;
    _bcm_dpp_counter_state_t *unitData;
    soc_port_t crps_index = 0;
    BCMDNX_INIT_FUNC_DEFS;

    result = _bcm_dpp_counter_unit_check_and_fill(unit, &unitData, COUNTER_UPDATE_SELECT_ALL, NULL);
    BCMDNX_IF_ERR_EXIT(result);
    BCMDNX_NULL_CHECK(enable);
    BCMDNX_NULL_CHECK(stag_lsb);
    *enable = unitData->global_info.haveStatTag;
    if (*enable) {
        for (crps_index = 0; crps_index < unitData->num_counter_procs; crps_index++) {
            if (unitData->proc[crps_index].mode.src_type == SOC_TMC_CNT_SRC_TYPE_STAG) {
                *stag_lsb = unitData->proc[crps_index].mode.stag_lsb;
                break;
            }
        }
    }
    if (crps_index == unitData->num_counter_procs) {
        LOG_ERROR(BSL_LS_BCM_CNT, (BSL_META_U(unit, "We have Stag but no counter source configures it.\n")));
        BCMDNX_IF_ERR_EXIT(BCM_E_CONFIG);
    }
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int 
_bcm_dpp_counter_source_config(
       _bcm_dpp_counter_state_t *unitData,
       soc_port_t crps_index, 
       int src_core,
       SOC_TMC_CNT_SRC_TYPE src_type, 
       int command_id,
       int command_base_offset,
       SOC_TMC_CNT_MODE_EG_TYPE eg_type,
       SOC_TMC_CNT_COUNTER_WE_BITMAP we_val,
       SOC_TMC_CNT_Q_SET_SIZE q_set_size,
       uint32 stag_lsb) {
    int unit = unitData->cunit;
    int result = BCM_E_NONE;
    int index = crps_index;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(unitData);
    

    unitData->proc[crps_index].mode.src_core = src_core;
    unitData->proc[crps_index].mode.src_type = src_type;
    unitData->proc[crps_index].mode.command_id = command_id;
    unitData->proc[crps_index].mode.mode_eg.base_val = command_base_offset;
    unitData->proc[crps_index].mode.we_val = we_val;
    switch (src_type) {
        case SOC_TMC_CNT_SRC_TYPE_ING_PP:
        case SOC_TMC_CNT_SRC_TYPE_VSQ:
        case SOC_TMC_CNT_SRC_TYPE_INGRESS_EXT_STAT:			
        case SOC_TMC_CNT_SRC_TYPE_CNM_ID:
        case SOC_TMC_CNT_SRC_TYPE_INGRESS_OAM:
        case SOC_TMC_CNT_SRC_TYPE_EGRESS_OAM:            
            unitData->proc[index].mode.mode_eg.type = eg_type;
            LOG_DEBUG(BSL_LS_BCM_CNT,
                      (BSL_META_U(unit,
                                  "unit %d proc %u source %s\n"),
                       unit,
                       index,
                       SOC_TMC_CNT_SRC_TYPE_to_string(unitData->proc[index].mode.src_type)));
            break;
        case SOC_TMC_CNT_SRC_TYPE_VOQ:
            unitData->proc[index].mode.q_set_size = q_set_size;
            LOG_DEBUG(BSL_LS_BCM_CNT, (BSL_META_U(unit, "unit %d proc %u source %s\n"), unit, index, "SOC_TMC_CNT_SRC_TYPE_VOQ"));
            break;
        case SOC_TMC_CNT_SRC_TYPE_STAG:
            if (unitData->global_info.haveStatTag) {
                LOG_ERROR(BSL_LS_BCM_CNT, (BSL_META_U(unit, "unit %d already has at least one processor in stat tag mode\n"),unit));
                BCMDNX_IF_ERR_EXIT(BCM_E_CONFIG);
            }  
            unitData->proc[index].mode.stag_lsb = stag_lsb;
            unitData->global_info.haveStatTag = TRUE;           
            break;
        case SOC_TMC_CNT_SRC_TYPE_EGR_PP:         
        case SOC_TMC_CNT_SRC_TYPES_EGQ_TM:
            SOC_TMC_CNT_MODE_EG_clear(&(unitData->proc[index].mode.mode_eg));
            unitData->proc[index].mode.mode_eg.type = eg_type;
             /*not allow EGRESS_FIELD on command 1*/
             if (unitData->proc[index].mode.command_id == 1 && unitData->proc[index].mode.src_type == SOC_TMC_CNT_SRC_TYPE_EGR_PP && unitData->proc[index].mode.mode_eg.type ==  SOC_TMC_CNT_MODE_EG_TYPE_PMF) 
             {
                 LOG_ERROR(BSL_LS_BCM_CNT,
                           (BSL_META_U(unit,
                                       "unit %d  not allowed EGRESS_FIELD - on command_id=1 - source %s, proc %d \n"),
                                   unit,
                                   SOC_TMC_CNT_SRC_TYPE_to_string(unitData->proc[crps_index].mode.src_type),
                                   crps_index));
                        BCMDNX_IF_ERR_EXIT(BCM_E_CONFIG);
             }
            LOG_DEBUG(BSL_LS_BCM_CNT,
                      (BSL_META_U(unit,
                                  "unit %d proc %u source %s egress type %s\n"),
                       unit,
                       index,
                       SOC_TMC_CNT_SRC_TYPE_to_string(unitData->proc[index].mode.src_type),
                       SOC_TMC_CNT_MODE_EG_TYPE_to_string(unitData->proc[index].mode.mode_eg.type)));
            break;
#ifdef BCM_ARAD_SUPPORT
        case SOC_TMC_CNT_SRC_TYPE_EPNI:
#endif          
            unitData->proc[index].mode.mode_eg.type = eg_type;
            LOG_DEBUG(BSL_LS_BCM_CNT,
                      (BSL_META_U(unit,
                                  "unit %d proc %u source %s egress type %s\n"),
                       unit,
                       index,
                       SOC_TMC_CNT_SRC_TYPE_to_string(unitData->proc[index].mode.src_type),
                       SOC_TMC_CNT_MODE_EG_TYPE_to_string(unitData->proc[index].mode.mode_eg.type)));
            break;    
        case SOC_TMC_CNT_SRC_TYPES_IPT_LATENCY:        
            break;
        default:
            LOG_ERROR(BSL_LS_BCM_CNT,
                      (BSL_META_U(unit,
                                  "unit %d counter processor %u"
                                  " invalid source %u\n"),
                       unit,
                       index,
                       src_type));
            result = BCM_E_PARAM;
    }
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int 
_bcm_dpp_counter_proc_inUse_config(_bcm_dpp_counter_state_t *unitData, soc_port_t crps_index){
    int unit = unitData->cunit;
    int result = BCM_E_NONE;
    size_t allocSize = 0;
    uint8 is_alloc = 0;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(unitData);
    
    result = COUNTERS_PROC_ACCESS.field.inUse.is_allocated(unit, crps_index, &is_alloc);
    BCMDNX_IF_ERR_EXIT(result);
    if (is_alloc) {
        BCM_EXIT;    
    }
    switch (unitData->proc[crps_index].mode.src_type) {
        case SOC_TMC_CNT_SRC_TYPE_ING_PP:
        case SOC_TMC_CNT_SRC_TYPE_EGR_PP:
            /* we manage these locally */
            allocSize = ((unitData->proc[crps_index].mode.num_sets + 7) / 8) * sizeof(uint32);
            result = COUNTERS_PROC_ACCESS.field.inUse.alloc(unit, crps_index, allocSize);
            BCMDNX_IF_ERR_EXIT(result);
            break;
        default:
            /* these are implied or managed elsewhere */
            break;
        }
exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int 
_bcm_dpp_counter_mode_config(_bcm_dpp_counter_state_t *unitData, soc_port_t crps_index, SOC_TMC_CNT_MODE_STATISTICS stat) {
    int unit = unitData->cunit;
    int result = BCM_E_NONE;
    unsigned int index2;
    uint32 i, sub_index=0;
    
    uint32 *entries_bmaps = unitData->proc[crps_index].mode.custom_mode_params.entries_bmaps;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(unitData);
    
    switch(unitData->proc[crps_index].mode.src_type){
        case SOC_TMC_CNT_SRC_TYPE_INGRESS_OAM:
        case SOC_TMC_CNT_SRC_TYPE_EGRESS_OAM:            
            if (stat != SOC_TMC_CNT_MODE_STATISTICS_ALL_NO_COLOR && stat != SOC_TMC_CNT_MODE_STATISTICS_CONFIGURABLE_OFFSETS){
                   LOG_ERROR(BSL_LS_BCM_CNT,(BSL_META_U(unit,"unit %d counter processor %u invalid stat %s for OAM\n"),
                                             unit,crps_index, SOC_TMC_CNT_MODE_STATISTICS_to_string(stat)));
                   BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);
            }
            break;
        case SOC_TMC_CNT_SRC_TYPE_CNM_ID:
            if (stat != SOC_TMC_CNT_MODE_STATISTICS_NO_COLOR && stat != SOC_TMC_CNT_MODE_STATISTICS_CONFIGURABLE_OFFSETS){
                LOG_ERROR(BSL_LS_BCM_CNT,(BSL_META_U(unit,"unit %d counter processor %u invalid stat %s for CNM\n"),
                           unit,crps_index,SOC_TMC_CNT_MODE_STATISTICS_to_string(stat)));
                BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);
            }
            break;
        case SOC_TMC_CNT_SRC_TYPE_EPNI:
        case SOC_TMC_CNT_SRC_TYPE_EGR_PP:
            if(unitData->proc[crps_index].mode.mode_eg.type == SOC_TMC_CNT_MODE_EG_TYPE_TM || 
               unitData->proc[crps_index].mode.mode_eg.type == SOC_TMC_CNT_MODE_EG_TYPE_TM_PORT){
                if((stat != SOC_TMC_CNT_MODE_STATISTICS_DROP_NO_COLOR) && 
                   (stat != SOC_TMC_CNT_MODE_STATISTICS_FWD_NO_COLOR) &&
                   (stat != SOC_TMC_CNT_MODE_STATISTICS_NO_COLOR) && 
                   (stat != SOC_TMC_CNT_MODE_STATISTICS_ALL_NO_COLOR) &&
                   (stat != SOC_TMC_CNT_MODE_STATISTICS_CONFIGURABLE_OFFSETS)){
                    LOG_ERROR(BSL_LS_BCM_CNT,(BSL_META_U(unit,"unit %d counter processor %u invalid stat %s for %s\n"),
                                unit,crps_index, SOC_TMC_CNT_MODE_STATISTICS_to_string(stat), SOC_TMC_CNT_SRC_TYPE_to_string(unitData->proc[crps_index].mode.src_type)));
                    BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);
                }
            }
            break;
        default:
            break;
    }
            
    if (SOC_IS_JERICHO(unit)) {
        unitData->proc[crps_index].mode.mode_statistics = SOC_TMC_CNT_MODE_STATISTICS_CONFIGURABLE_OFFSETS;
    } else {
        unitData->proc[crps_index].mode.mode_statistics = stat;
    }
    /* configure things according to the specified stats format */
    switch (stat) {
        case SOC_TMC_CNT_MODE_STATISTICS_COLOR_RES_ENQ_HI: /*SIMPLE_COLOR*/
            unitData->proc[crps_index].mode.mode_eg.resolution = SOC_TMC_CNT_MODE_EG_RES_COLOR;
            unitData->proc[crps_index].mode.custom_mode_params.set_size = 2;
            unitData->proc[crps_index].mode.num_sets = unitData->proc[crps_index].mode.num_counters / 2;
            entries_bmaps[0] = GREEN_FWD | GREEN_DROP;
            entries_bmaps[1] = YELLOW_FWD | YELLOW_DROP | RED_FWD | RED_DROP | BLACK_FWD | BLACK_DROP;
            LOG_DEBUG(BSL_LS_BCM_CNT,
                      (BSL_META_U(unit,
                                  "unit %d proc %u mode %s,"
                                  " sets %u\n"),
                       unit,
                       crps_index,
                       "SOC_TMC_CNT_MODE_EG_RES_COLOR",
                       unitData->proc[crps_index].mode.num_sets));
            break;
#ifdef BCM_88660_A0
        case SOC_TMC_CNT_MODE_STATISTICS_CONFIGURABLE_OFFSETS: /*CONFIGURABLE: */
            unitData->proc[crps_index].mode.num_sets = unitData->proc[crps_index].mode.num_counters / unitData->proc[crps_index].mode.custom_mode_params.set_size;
            LOG_DEBUG(BSL_LS_BCM_CNT,(BSL_META_U(unit,"unit %d proc %u mode %s, sets %u\n"),
                       unit,crps_index,"SOC_TMC_CNT_MODE_STATISTICS_CONFIGURABLE_OFFSETS",unitData->proc[crps_index].mode.num_sets));
#ifdef BCM_88660_A0            
            if((SOC_IS_ARADPLUS(unit) || SOC_IS_JERICHO(unit)) &&
                (unitData->proc[crps_index].mode.format == SOC_TMC_CNT_FORMAT_PKTS_AND_PKTS)) {
                if (unitData->proc[crps_index].mode.custom_mode_params.set_size == 1){
                    unitData->proc[crps_index].mode.num_sets = unitData->proc[crps_index].mode.num_sets * 2;
                    SHR_BITSET(unitData->proc[crps_index].native, bcm_dpp_counter_offset0_pkts);
                    unitData->proc[crps_index].avail_offsets[bcm_dpp_counter_offset0_pkts] = _OFS_E_CN + _SUB_PKTS;
                } else if ((unitData->proc[crps_index].mode.custom_mode_params.set_size == 2) && SOC_IS_JERICHO(unit)) {
                    /* set number keep unchange*/
                    for (i = 0; i < unitData->proc[crps_index].mode.custom_mode_params.set_size; i++) {
                        SHR_BITSET(unitData->proc[crps_index].native, (bcm_dpp_counter_offset0_pkts+i*2));
                        if (entries_bmaps[i] & (YELLOW_FWD | RED_FWD | BLACK_FWD | GREEN_FWD)) {
                            sub_index = _SUB_FWD_PKTS;
                        } else if (entries_bmaps[i] & (YELLOW_DROP | RED_DROP | BLACK_DROP | GREEN_DROP)) {
                            sub_index = _SUB_DROP_PKTS;
                        }
                        unitData->proc[crps_index].avail_offsets[bcm_dpp_counter_offset0_pkts+i*2] = _OFS_E_CN + sub_index;
                    }
                } else {
                    LOG_ERROR(BSL_LS_BCM_CNT,(BSL_META_U(unit, "unit %d, proc=%d, set size should illegal\n"),
                               unit, crps_index));
                    BCMDNX_IF_ERR_EXIT(BCM_E_CONFIG); 
                }
            }
#endif              
            break;
        case SOC_TMC_CNT_MODE_STATISTICS_FWD_SIMPLE_COLOR: /*SIMPLE_COLOR_FWD:*/
            unitData->proc[crps_index].mode.custom_mode_params.set_size = 2;
            unitData->proc[crps_index].mode.num_sets = unitData->proc[crps_index].mode.num_counters / 2;
            entries_bmaps[0] = GREEN_FWD;
            entries_bmaps[1] = YELLOW_FWD | RED_FWD | BLACK_FWD;
            entries_bmaps[SOC_TMC_CNT_BMAP_OFFSET_INVALID_VAL] = GREEN_DROP | YELLOW_DROP | RED_DROP | BLACK_DROP;
            LOG_DEBUG(BSL_LS_BCM_CNT,
                      (BSL_META_U(unit,
                                  "unit %d proc %u mode %s,"
                                  " sets %u\n"),
                       unit,
                       crps_index,
                       "SOC_TMC_CNT_MODE_STATISTICS_FWD_SIMPLE_COLOR",
                       unitData->proc[crps_index].mode.num_sets));
            break;
        case SOC_TMC_CNT_MODE_STATISTICS_DROP_SIMPLE_COLOR: /*SIMPLE_COLOR_DROP:*/
            unitData->proc[crps_index].mode.custom_mode_params.set_size = 2;
            unitData->proc[crps_index].mode.num_sets = unitData->proc[crps_index].mode.num_counters / 2;
            entries_bmaps[0] = GREEN_DROP;
            entries_bmaps[1] = YELLOW_DROP | RED_DROP | BLACK_DROP;
            entries_bmaps[SOC_TMC_CNT_BMAP_OFFSET_INVALID_VAL] = GREEN_FWD | YELLOW_FWD | RED_FWD | BLACK_FWD;
            LOG_DEBUG(BSL_LS_BCM_CNT,
                      (BSL_META_U(unit,
                                  "unit %d proc %u mode %s,"
                                  " sets %u\n"),
                       unit,
                       crps_index,
                       "SOC_TMC_CNT_MODE_STATISTICS_DROP_SIMPLE_COLOR",
                       unitData->proc[crps_index].mode.num_sets));
            break;
#endif
        case SOC_TMC_CNT_MODE_STATISTICS_NO_COLOR: /*FWD_DROP*/
            unitData->proc[crps_index].mode.custom_mode_params.set_size = 2;
            unitData->proc[crps_index].mode.num_sets = unitData->proc[crps_index].mode.num_counters / 2;
            entries_bmaps[0] = GREEN_FWD | YELLOW_FWD| RED_FWD | BLACK_FWD;
            entries_bmaps[1] =  GREEN_DROP | YELLOW_DROP  | RED_DROP | BLACK_DROP;
#ifdef BCM_88660_A0
            if((SOC_IS_ARADPLUS(unit) || SOC_IS_JERICHO(unit)) && (unitData->proc[crps_index].mode.format == SOC_TMC_CNT_FORMAT_PKTS_AND_PKTS)){
                unitData->proc[crps_index].mode.num_sets = unitData->proc[crps_index].mode.num_counters;
                /*packet and packet is only fwd_drop. and we use implict decleartion of native and avail formats*/
                SHR_BITSET(unitData->proc[crps_index].native, bcm_dpp_counter_fwd_pkts);
                SHR_BITSET(unitData->proc[crps_index].native, bcm_dpp_counter_drop_pkts);
                unitData->proc[crps_index].avail_offsets[bcm_dpp_counter_pkts] = ((1 << (_OFS_E_CN + _SUB_FWD_PKTS)) |
                                                                                  (1 << (_OFS_E_CN + _SUB_DROP_PKTS)));
                unitData->proc[crps_index].avail_offsets[bcm_dpp_counter_fwd_pkts] = _OFS_E_CN + _SUB_FWD_PKTS;
                unitData->proc[crps_index].avail_offsets[bcm_dpp_counter_drop_pkts] =  _OFS_E_CN + _SUB_DROP_PKTS;
            }
#endif
            LOG_DEBUG(BSL_LS_BCM_CNT,
                      (BSL_META_U(unit,
                                  "unit %d proc %u mode %s,"
                                  " sets %u\n"),
                       unit,
                       crps_index,
                       "SOC_TMC_CNT_MODE_STATISTICS_NO_COLOR",
                       unitData->proc[crps_index].mode.num_sets));
            break;
        case SOC_TMC_CNT_MODE_STATISTICS_COLOR_RES_LOW: /*GREEN_NOT_GREEN:*/
            unitData->proc[crps_index].mode.custom_mode_params.set_size = 4;
            unitData->proc[crps_index].mode.num_sets = unitData->proc[crps_index].mode.num_counters / 4;
            entries_bmaps[0] = GREEN_FWD;
            entries_bmaps[1] = GREEN_DROP;
            entries_bmaps[2] = YELLOW_FWD | RED_FWD | BLACK_FWD;
            entries_bmaps[3] = YELLOW_DROP | RED_DROP | BLACK_DROP;   
            LOG_DEBUG(BSL_LS_BCM_CNT,
                      (BSL_META_U(unit,
                                  "unit %d proc %u mode %s,"
                                  " sets %u\n"),
                       unit,
                       crps_index,
                       "SOC_TMC_CNT_MODE_STATISTICS_COLOR_RES_LOW",
                       unitData->proc[crps_index].mode.num_sets));
            break;
        case SOC_TMC_CNT_MODE_STATISTICS_COLOR_RES_HI: /*FULL_COLOR*/
            unitData->proc[crps_index].mode.custom_mode_params.set_size = 5;
            unitData->proc[crps_index].mode.num_sets = unitData->proc[crps_index].mode.num_counters / 5;
            entries_bmaps[0] = GREEN_FWD;
            entries_bmaps[1] = GREEN_DROP;
            entries_bmaps[2] = YELLOW_FWD | RED_FWD | BLACK_FWD;
            entries_bmaps[3] = YELLOW_DROP | RED_DROP;  
            entries_bmaps[4] = BLACK_DROP; 
            LOG_DEBUG(BSL_LS_BCM_CNT,
                      (BSL_META_U(unit,
                                  "unit %d proc %u mode %s,"
                                  " sets %u\n"),
                       unit,
                       crps_index,
                       "SOC_TMC_CNT_MODE_STATISTICS_COLOR_RES_HI",
                       unitData->proc[crps_index].mode.num_sets));
            break;
#ifdef BCM_88650_B0
        case SOC_TMC_CNT_MODE_STATISTICS_FWD_NO_COLOR: /*FWD:*/
            if (SOC_IS_ARAD_B0_AND_ABOVE(unit)) {
                unitData->proc[crps_index].mode.custom_mode_params.set_size = 1;
                unitData->proc[crps_index].mode.num_sets = unitData->proc[crps_index].mode.num_counters;
                entries_bmaps[0] = GREEN_FWD | YELLOW_FWD | RED_FWD | BLACK_FWD;
                entries_bmaps[SOC_TMC_CNT_BMAP_OFFSET_INVALID_VAL] = GREEN_DROP | YELLOW_DROP | RED_DROP | BLACK_DROP;
                LOG_DEBUG(BSL_LS_BCM_CNT,
                          (BSL_META_U(unit,
                                      "unit %d proc %u mode %s,"
                                      " sets %u\n"),
                           unit,
                           crps_index,
                           "SOC_TMC_CNT_MODE_STATISTICS_NO_COLOR_ONE_ENTRY",
                           unitData->proc[crps_index].mode.num_sets));
            }
            break;
        case SOC_TMC_CNT_MODE_STATISTICS_DROP_NO_COLOR: /*DROP:*/ 
            if (SOC_IS_ARAD_B0_AND_ABOVE(unit)) {
                unitData->proc[crps_index].mode.custom_mode_params.set_size = 1;
                unitData->proc[crps_index].mode.num_sets = unitData->proc[crps_index].mode.num_counters;
                entries_bmaps[0] = GREEN_DROP | YELLOW_DROP | RED_DROP | BLACK_DROP;
                entries_bmaps[SOC_TMC_CNT_BMAP_OFFSET_INVALID_VAL] = GREEN_FWD | YELLOW_FWD | RED_FWD | BLACK_FWD;
                LOG_DEBUG(BSL_LS_BCM_CNT,
                          (BSL_META_U(unit,
                                      "unit %d proc %u mode %s,"
                                      " sets %u\n"),
                           unit,
                           crps_index,
                           "SOC_TMC_CNT_MODE_STATISTICS_NO_COLOR_ONE_ENTRY",
                           unitData->proc[crps_index].mode.num_sets));
            }
            break;
#endif
        case SOC_TMC_CNT_MODE_STATISTICS_ALL_NO_COLOR: /*ALL:*/
            unitData->proc[crps_index].mode.custom_mode_params.set_size = 1;
            unitData->proc[crps_index].mode.num_sets = unitData->proc[crps_index].mode.num_counters;
            entries_bmaps[0] = GREEN_FWD | YELLOW_FWD | RED_FWD | BLACK_FWD | GREEN_DROP | YELLOW_DROP | RED_DROP | BLACK_DROP;
            LOG_DEBUG(BSL_LS_BCM_CNT,
                      (BSL_META_U(unit,
                                  "unit %d proc %u mode %s,"
                                  " sets %u\n"),
                       unit,
                       crps_index,
                       "SOC_TMC_CNT_MODE_STATISTICS_NO_COLOR_ONE_ENTRY",
                       unitData->proc[crps_index].mode.num_sets));
            break;

        default:
            /* this was already handled above */
            break;
    }
    BCMDNX_IF_ERR_EXIT(result);

    /*except the PetraB and the packets and packets mode we calc the avail and native format by the entries bmap */
    if (unitData->proc[crps_index].mode.format != SOC_TMC_CNT_FORMAT_PKTS_AND_PKTS) {
        result = _bcm_dpp_counters_calc_formats(
                   unitData->proc[crps_index].mode.src_type,
                   unitData->proc[crps_index].mode.mode_eg.type,
                   unitData->proc[crps_index].mode.num_counters/unitData->proc[crps_index].mode.num_sets, /*unitData->proc[crps_index].mode.custom_mode_params.set_size,*/
                   entries_bmaps, 
                   unitData->proc[crps_index].native, 
                   unitData->proc[crps_index].avail_offsets);
        BCMDNX_IF_ERR_EXIT(result);
    }

    /* Remove from native and available in case of packet-only or byte-only the other counters */
    /*remove all the packets formats*/
    if (unitData->proc[crps_index].mode.format == SOC_TMC_CNT_FORMAT_BYTES) {
        uint8 is_bytes_format;
        for (index2 = 0; index2 < bcm_dpp_counter_count ; index2++) {
            result = _bcm_dpp_counters_is_byte_format_get(index2, &is_bytes_format);
            BCMDNX_IF_ERR_EXIT(result);

            if(!is_bytes_format){
                SHR_BITCLR(unitData->proc[crps_index].native, index2);
                unitData->proc[crps_index].avail_offsets[index2] = UNSUPPORTED_COUNTER_OFFSET;
            }
        }
    } else if ((unitData->proc[crps_index].mode.format == SOC_TMC_CNT_FORMAT_PKTS) || 
               (unitData->proc[crps_index].mode.format == SOC_TMC_CNT_FORMAT_PKTS_AND_PKTS)) { /*remove bytes formats*/
        uint8 is_bytes_format;
        for (index2 = 0; index2 < bcm_dpp_counter_count; index2++) {
            result = _bcm_dpp_counters_is_byte_format_get(index2, &is_bytes_format);
            BCMDNX_IF_ERR_EXIT(result);

            if(is_bytes_format){
                SHR_BITCLR(unitData->proc[crps_index].native, index2);
                unitData->proc[crps_index].avail_offsets[index2] = UNSUPPORTED_COUNTER_OFFSET;
            }
        }
    }
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int 
_bcm_dpp_counter_unit_state_verify(_bcm_dpp_counter_state_t *unitData, soc_port_t crps_index) {
    int unit = unitData->cunit;
    unsigned int j;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(unitData);
    
    /* relevant only to Arad. In Arad the source mapping is hard coded */
    if (SOC_IS_ARADPLUS_AND_BELOW(unitData->cunit))
    {
        if (unitData->proc[crps_index].mode.src_type == SOC_TMC_CNT_SRC_TYPE_VOQ)
        {
            BCM_EXIT;
        }
    }
    /* in QAX/JER_PLUS - for SRC EPNI the base val must be in 2K resolution. see register EPNI_COUNTERS_BASE */
    if(SOC_IS_JERICHO_PLUS(unit))
    {
        if ((unitData->proc[crps_index].mode.src_type == SOC_TMC_CNT_SRC_TYPE_EPNI) && ((unitData->proc[crps_index].mode.mode_eg.base_val % 2048) != 0))
        {
            LOG_ERROR(BSL_LS_BCM_CNT,(BSL_META_U(unit, "unit %d, proc=%d, source EPNI, base_val=%d is not allowed for QAX. must be in 2K jumps \n"),
                       unit, crps_index, unitData->proc[crps_index].mode.mode_eg.base_val));
             BCMDNX_IF_ERR_EXIT(BCM_E_CONFIG);            
        }
    }    
    /* run over all counters except the current engine (crps_index)
     * verify that the other engine source configuration match to the current engine source */
    for (j = 0; (j < unitData->num_counter_procs); j++)
    {
        if (j == crps_index)
        {
            continue;
        }

        /* case engine not valid - continue */
        if (!(unitData->proc[j].valid & BCM_DPP_COUNTER_PROC_INFO_VALID))
        {
            continue;
        }
        /* case that the source is the same */
        if (unitData->proc[crps_index].mode.src_type   == unitData->proc[j].mode.src_type &&
            unitData->proc[crps_index].mode.src_core   == unitData->proc[j].mode.src_core &&
            unitData->proc[crps_index].mode.command_id == unitData->proc[j].mode.command_id)
        {
            /* must have same statistic mode */
            /* one exception to the check: CONFIGRABLE mode can be used only in ARAD_PLUS and above. For ARAD_PLUS, it is not mandatory, therefore, there can be a miss-match
             * For example, if soc configured one engine as FULL_COLOR and API configure different engine as CONFIGURABLE,
             * the mode_statistic will be different, but it still might be OK. (if not OK, the custom_mode_params check will find it later in this function)
             */
            if( (unitData->proc[crps_index].mode.mode_statistics != unitData->proc[j].mode.mode_statistics) &&
                    !(SOC_IS_ARADPLUS_AND_BELOW(unit) &&
                     ((unitData->proc[crps_index].mode.mode_statistics == ARAD_CNT_MODE_STATISTICS_CONFIGURABLE_OFFSETS) ||
                      (unitData->proc[j].mode.mode_statistics == ARAD_CNT_MODE_STATISTICS_CONFIGURABLE_OFFSETS)) ) )
            {

                LOG_ERROR(BSL_LS_BCM_CNT,
                          (BSL_META_U(unit,
                                      "unit %d different ingress mode defined for source %s proc %d: %s , proc %d: %s\n"),
                           unit,
                           SOC_TMC_CNT_SRC_TYPE_to_string(unitData->proc[crps_index].mode.src_type),
                           crps_index,
                           SOC_TMC_CNT_MODE_STATISTICS_to_string(unitData->proc[crps_index].mode.mode_statistics),
                           j,
                           SOC_TMC_CNT_MODE_STATISTICS_to_string(unitData->proc[j].mode.mode_statistics)));
                BCMDNX_IF_ERR_EXIT(BCM_E_CONFIG);
            }
            else
            {
                /* only for Arad  - if the formats are the same, they must be different from  PKTS_AND_PKTS */
                if (SOC_IS_ARADPLUS_AND_BELOW(unitData->cunit))
                {
                    if((unitData->proc[crps_index].mode.format != unitData->proc[j].mode.format) &&
                       (unitData->proc[crps_index].mode.format == SOC_TMC_CNT_FORMAT_PKTS_AND_PKTS || unitData->proc[j].mode.format == SOC_TMC_CNT_FORMAT_PKTS_AND_PKTS))
                    {
                        LOG_ERROR(BSL_LS_BCM_CNT,
                                  (BSL_META_U(unit,
                                              "unit %d can't use packet and packet and another format for the same source type %s proc %d: %s , proc %d: %s\n"),
                                   unit,
                                   SOC_TMC_CNT_SRC_TYPE_to_string(unitData->proc[crps_index].mode.src_type),
                                   crps_index,
                                   SOC_TMC_CNT_FORMAT_to_string(unitData->proc[crps_index].mode.format),
                                   j,
                                   SOC_TMC_CNT_FORMAT_to_string(unitData->proc[j].mode.format)));
                        BCMDNX_IF_ERR_EXIT(BCM_E_CONFIG);
                    }
                }
                if(unitData->proc[crps_index].mode.mode_statistics == SOC_TMC_CNT_MODE_STATISTICS_CONFIGURABLE_OFFSETS &&
                   (sal_memcmp(&(unitData->proc[crps_index].mode.custom_mode_params), &(unitData->proc[j].mode.custom_mode_params) , sizeof(unitData->proc[crps_index].mode.custom_mode_params))!= 0))
                {
                    LOG_ERROR(BSL_LS_BCM_CNT,
                              (BSL_META_U(unit,
                                          "unit %d different configurable mode params for source %s proc %d and proc %d\n"),
                               unit,
                               SOC_TMC_CNT_SRC_TYPE_to_string(unitData->proc[crps_index].mode.src_type),
                               crps_index,
                               j));
                    BCMDNX_IF_ERR_EXIT(BCM_E_CONFIG);
                }


                /* check for none IQM src */
                if (unitData->proc[crps_index].mode.src_type != SOC_TMC_CNT_SRC_TYPE_VOQ &&
                    unitData->proc[crps_index].mode.src_type != SOC_TMC_CNT_SRC_TYPE_STAG &&
                    unitData->proc[crps_index].mode.src_type != SOC_TMC_CNT_SRC_TYPE_VSQ &&
                    unitData->proc[crps_index].mode.src_type != SOC_TMC_CNT_SRC_TYPE_CNM_ID)
                {
                    if (unitData->proc[crps_index].mode.mode_eg.type != unitData->proc[j].mode.mode_eg.type)
                    {
                        /*the mode_eg must be the same, i.e. we must count the same entity on the same command id*/
                        LOG_ERROR(BSL_LS_BCM_CNT,
                                  (BSL_META_U(unit,
                                              "unit %d different counting modes type for the same command_id params for source %s proc %d and proc %d\n"),
                                   unit,
                                   SOC_TMC_CNT_SRC_TYPE_to_string(unitData->proc[crps_index].mode.src_type),
                                   crps_index,
                                   j));
                        BCMDNX_IF_ERR_EXIT(BCM_E_CONFIG);

                    }
                }
                else /* for IQM src */
                {
                    /* q_set_size must be the same */
                    if (unitData->proc[crps_index].mode.q_set_size != unitData->proc[j].mode.q_set_size)
                    {

                        LOG_ERROR(BSL_LS_BCM_CNT,
                                  (BSL_META_U(unit,
                                              "unit %d different q_set_size for the same command_id params for source %s, proc %d and proc %d\n"),
                                   unit,
                                   SOC_TMC_CNT_SRC_TYPE_to_string(unitData->proc[crps_index].mode.src_type),
                                   crps_index,
                                   j));
                        BCMDNX_IF_ERR_EXIT(BCM_E_CONFIG);

                    }
                }
                /* verify that the base value is the same per source (except Egress_PP which base is not relevant for) */
                if (unitData->proc[crps_index].mode.src_type != SOC_TMC_CNT_SRC_TYPE_EGR_PP)
                {
                    if (unitData->proc[crps_index].mode.mode_eg.base_val != unitData->proc[j].mode.mode_eg.base_val)
                    {
                        LOG_ERROR(BSL_LS_BCM_CNT,
                                  (BSL_META_U(unit,
                                              "unit %d different counting base_val for the same command_id params for source %s proc %d and proc %d\n"),
                                   unit,
                                   SOC_TMC_CNT_SRC_TYPE_to_string(unitData->proc[crps_index].mode.src_type),
                                   crps_index,
                                   j));
                        BCMDNX_IF_ERR_EXIT(BCM_E_CONFIG);
                    }
                }
            }
        }
        /*case that the source is different*/
        else
        {
            /*not allowed two different types from ERPP*/
            if(unitData->proc[crps_index].mode.src_type   == unitData->proc[j].mode.src_type &&
               unitData->proc[crps_index].mode.command_id != unitData->proc[j].mode.command_id &&
               unitData->proc[crps_index].mode.src_core == unitData->proc[j].mode.src_core)
            {
                if(unitData->proc[crps_index].mode.src_type == SOC_TMC_CNT_SRC_TYPE_EGR_PP && (unitData->proc[crps_index].mode.mode_eg.type != unitData->proc[j].mode.mode_eg.type))
                {
                    LOG_ERROR(BSL_LS_BCM_CNT,
                                  (BSL_META_U(unit,
                                              "unit %d  not allowed two different comands in ERPP with different counter profile - source %s, proc %d and proc %d\n"),
                                   unit,
                                   SOC_TMC_CNT_SRC_TYPE_to_string(unitData->proc[crps_index].mode.src_type),
                                   crps_index,
                                   j));
                        BCMDNX_IF_ERR_EXIT(BCM_E_CONFIG);
                }
            }
        }
    }
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int 
_bcm_dpp_counter_oam_verify(_bcm_dpp_counter_state_t *unitData) {
    unsigned int index;
    int prev_oam_ing_proc = -1;
    int prev_oam_eng_proc = -1;
    int unit = unitData->cunit;
    int result = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(unitData);
    for (index = 0; (index < unitData->num_counter_procs); index++) {
        if ((unitData->proc[index].mode.src_type == SOC_TMC_CNT_SRC_TYPE_INGRESS_OAM) || (unitData->proc[index].mode.src_type == SOC_TMC_CNT_SRC_TYPE_EGRESS_OAM)) {
            if (SOC_IS_ARADPLUS_AND_BELOW(unitData->cunit)) {
                if (unitData->proc[index].mode.command_id == 0) {
                    if(prev_oam_eng_proc == -1){
                        prev_oam_eng_proc = index;
                    }
                    if ((index >= 2) && (prev_oam_eng_proc < 2)) {
                        LOG_ERROR(BSL_LS_BCM_CNT,
                                  (BSL_META_U(unit,
                                              "unit %d Both OAM EG counter engines must be {0,1} or {2,3}. Your configuration {%d,%d}"),
                                   unit, prev_oam_eng_proc , index));
                        result = BCM_E_CONFIG;
                        break; 
                    }
                } else if (unitData->proc[index].mode.command_id == 1) {
                    if(prev_oam_ing_proc == -1) {
                        prev_oam_ing_proc = index;
                    }
                    if ((index >= 2) && (prev_oam_ing_proc < 2)) {
                        LOG_ERROR(BSL_LS_BCM_CNT,
                                  (BSL_META_U(unit,
                                              "unit %d Both OAM ING counter engines must be {0,1} or {2,3}. Your configuration {%d,%d}"),
                                   unit, prev_oam_ing_proc, index));
                        result = BCM_E_CONFIG;
                        break;
                    }
                }
        }
        /* For Jericho and above - no need of consecutive limitation, but ingress engine ids should be smaller than the egress engine ids*/
        else {
            if (unitData->proc[index].mode.src_type == SOC_TMC_CNT_SRC_TYPE_INGRESS_OAM) {
                 if (prev_oam_eng_proc != -1 && prev_oam_eng_proc < index) {
                     LOG_ERROR(BSL_LS_BCM_CNT,
                                  (BSL_META_U(unit,
                                              "unit %d OAM ingress counter engines must be with lower ID than OAM egress counter engines. Your configuration ingress,egress {%d,%d}"),
                                   unit, index, prev_oam_eng_proc));
                     result = BCM_E_CONFIG;
                     break;
                    }
                } 
            else {
                prev_oam_eng_proc = index;
                }
            }
        }
    }
    BCMDNX_IF_ERR_EXIT(result);

exit:
    BCMDNX_FUNC_RETURN;
}
int
bcm_dpp_counter_cache_only_set(
    int unit, 
    uint8 cache_only){
    int result = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    result = COUNTERS_ACCESS.global_info.cache_only.set(unit, cache_only ? TRUE : FALSE);
    BCMDNX_IF_ERR_EXIT(result);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_counter_cache_only_get(
    int unit, 
    uint8* cache_only){
    int result = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(cache_only);

    result = COUNTERS_ACCESS.global_info.cache_only.get(unit, cache_only);
    BCMDNX_IF_ERR_EXIT(result);

exit:
    BCMDNX_FUNC_RETURN;
}
int
bcm_dpp_counter_config_set(
        int unit,
        soc_port_t crps_index,
        int src_core,
        SOC_TMC_CNT_SRC_TYPE src_type,
        int command_id,
        int command_base_offset,
        SOC_TMC_CNT_MODE_EG_TYPE eg_type,
        SOC_TMC_CNT_FORMAT format,
        SOC_TMC_CNT_COUNTER_WE_BITMAP we_val,
        SOC_TMC_CNT_CUSTOM_MODE_PARAMS* custom_mode_params,
        SOC_TMC_CNT_Q_SET_SIZE q_set_size,
        uint32 stag_lsb,
        uint8 clear_cache,
        uint32 multiple_sources,
        int counter_profile_offset) {
    char *propkey, *propval;
    _bcm_dpp_counter_state_t *unitData;
    soc_dpp_config_arad_t *dpp_arad;
    int valid = 0, stif_enabled;
    int result = BCM_E_NONE;
    uint32 soc_sandRes, stif_cmd_id;
    int index;
    uint64 counter;
    uint8 used_dma, using_dma;
    int fifo = 0, cache_length = 0;
    BCMDNX_INIT_FUNC_DEFS;
    result = _bcm_dpp_counter_unit_check_and_fill(unit, &unitData, COUNTER_UPDATE_SELECT_ALL, NULL);
    BCMDNX_IF_ERR_EXIT(result);

    BCMDNX_NULL_CHECK(custom_mode_params);
    dpp_arad = SOC_DPP_CONFIG(unit)->arad;
    if (crps_index >= SOC_DPP_DEFS_GET(unit, nof_counter_processors)) {
        if (src_type != SOC_TMC_CNT_SRC_TYPES_EGQ_TM) {
            LOG_ERROR(BSL_LS_BCM_CNT,(BSL_META_U(unit,"unit %d counter %d the last counter engines must be configured for EGQ-TM\n"),unit, crps_index));
            BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);
        }
        if ((src_core != 0) && (crps_index == SOC_DPP_DEFS_GET(unit, nof_counter_processors))) {
            LOG_ERROR(BSL_LS_BCM_CNT,(BSL_META_U(unit,"unit %d counter %d must be configured for core 0\n"),unit, crps_index));
            BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);
        } 
        if ((src_core != 1) && (crps_index == SOC_DPP_DEFS_GET(unit, nof_counter_processors) + 1)) {
            LOG_ERROR(BSL_LS_BCM_CNT,(BSL_META_U(unit,"unit %d counter %d must be configured for core 1\n"),unit, crps_index));
            BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);                   
        }
    }
    used_dma = ((unitData->proc[crps_index].valid & BCM_DPP_COUNTER_PROC_INFO_VALID) && 
                (!SOC_TMC_CNT_FORMAT_IS_WIDE(unitData->proc[crps_index].mode.format, unitData->proc[crps_index].mode.src_type)));    

    if (SOC_IS_JERICHO(unit) && 
        unitData->proc[crps_index].mode.mode_eg.type == SOC_TMC_CNT_MODE_EG_TYPE_OUTLIF) {
        /*In jericho - we configure the base value for LIFs in bcm_dpp_counter_lif_range_set() - Use old value*/
        command_base_offset = unitData->proc[crps_index].mode.mode_eg.base_val;
    }
    SOC_TMC_CNT_COUNTERS_INFO_clear(unit, &(unitData->proc[crps_index].mode));
    unitData->proc[crps_index].mode.num_counters = SOC_TMC_NOF_COUNTERS_IN_ENGINE(unit, crps_index);
    /* get counter processor source */
    /* all but last default to ingress PP, last to egress PP */
    unitData->proc[crps_index].mode.format = format;
    using_dma = !SOC_TMC_CNT_FORMAT_IS_WIDE(format, unitData->proc[crps_index].mode.src_type);    

    result = _bcm_dpp_counter_source_config(unitData, crps_index, src_core, src_type, command_id, command_base_offset, eg_type, we_val, q_set_size, stag_lsb);
    BCMDNX_IF_ERR_EXIT(result);
    
    sal_memcpy(unitData->proc[crps_index].mode.custom_mode_params.entries_bmaps, custom_mode_params->entries_bmaps, sizeof(uint32) * SOC_TMC_CNT_BMAP_OFFSET_COUNT);
    unitData->proc[crps_index].mode.custom_mode_params.set_size = custom_mode_params->set_size;
    if (multiple_sources == BCM_STAT_COUNTER_MULTIPLE_SOURCES_PER_ENGINE) {
        unitData->proc[crps_index].mode.multiple_sources = TRUE; 
    }
    else {
        unitData->proc[crps_index].mode.multiple_sources = FALSE; 
    }

    result = _bcm_dpp_counter_mode_config(unitData, crps_index, SOC_TMC_CNT_MODE_STATISTICS_CONFIGURABLE_OFFSETS);
    BCMDNX_IF_ERR_EXIT(result);

    result = _bcm_dpp_counter_proc_inUse_config(unitData, crps_index);
    BCMDNX_IF_ERR_EXIT(result);

    result = _bcm_dpp_counter_unit_state_verify(unitData, crps_index);
    BCMDNX_IF_ERR_EXIT(result);

    /* check oam configuration is valid*/
    result = _bcm_dpp_counter_oam_verify(unitData);
    BCMDNX_IF_ERR_EXIT(result);

    /* configure counter profile offsets for ERPP sources - update the register */
    if(src_type == SOC_TMC_CNT_SRC_TYPE_EGR_PP) {
        result = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_stat_counter_erpp_offset_set, (unit, src_type, eg_type, counter_profile_offset));
        BCMDNX_IF_ERR_EXIT(result);
    }

    /* disable epni statistics in case there is not yet an epni source */
    if(SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit)) {
        /* get if STIF is enabled and if it is using ETPP mode - if STIF is using ETPP the EPNI source should not be disabled */
        stif_enabled = dpp_arad->init.stat_if.stat_if_enable;
        propkey = spn_COUNTER_ENGINE_SOURCE;
        /* read for each command id the ETPP source mode */
        for(stif_cmd_id = 0; stif_cmd_id < NUM_OF_COUNTERS_CMDS; stif_cmd_id++) {
            propval = soc_property_suffix_num_str_get(unit, stif_cmd_id, propkey, "egress_pp_stat");
            if (propval != NULL) {
                break;
            }
        }
        /* check if EPNI is used for STIF*/
        if (stif_enabled == TRUE && propval != NULL) {
            valid = 1;
        }
        /* check if EPNI is used for CRPS*/
        else {
            for (index = 0; index < unitData->num_counter_procs; index++) {
                if(((unitData->proc[index].valid & BCM_DPP_COUNTER_PROC_INFO_VALID) == TRUE) && unitData->proc[index].mode.src_type == SOC_TMC_CNT_SRC_TYPE_EPNI) {
                    valid = 1;
                    break;
                }
            }
        }
        /* check if no EPNI source - disable EPNI statistic */
        if(valid == 0) {
            soc_sandRes = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_cnt_pni_statistics_enable,(unit, FALSE)));
        }
    }
    /*Commit to HW*/
    soc_sandRes = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_cnt_counters_set,(unit, unitData->proc[crps_index].proc_id, &(unitData->proc[crps_index].mode))));
    result = handle_sand_result(soc_sandRes);
    BCMDNX_IF_ERR_EXIT(result);

    soc_sandRes = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_cnt_engine_to_fifo_dma_index, (unit, crps_index, &fifo, &cache_length));

    BCM_SAND_IF_ERR_EXIT(soc_sandRes);
    if (using_dma) {
        unitData->fifo[fifo].ref_count++;
    }
    if (used_dma) {
        if(unitData->fifo[fifo].ref_count > 0) {
            unitData->fifo[fifo].ref_count--;
        }
    }
    /*Update DMA state id needed*/
    result = _bcm_dpp_counter_dma_fifo_update_state(unitData, fifo, SOC_TMC_CNT_INVALID_DMA_CHANNEL);
    BCMDNX_IF_ERR_EXIT(result);

    unitData->proc[crps_index].valid = BCM_DPP_COUNTER_PROC_INFO_VALID | BCM_DPP_COUNTER_PROC_INFO_CONFIGURED_DYNAMICALLY;

    /*Update per source info - for all sources*/
    for (index = 0; index < unitData->num_counter_procs; index++) {
        BCM_DPP_COUNTER_SOURCE_MATCH(unitData, index, src_core, src_type, command_id, SOC_TMC_CNT_NOF_MODE_EG_TYPES_ARAD) {
            unitData->proc[index].mode.mode_eg.base_val = command_base_offset;
            unitData->proc[index].mode.q_set_size = q_set_size;
        }
    }
    if (clear_cache) {
        COMPILER_64_ZERO(counter);
        result = _bcm_dpp_counter_state_set_all(unitData, crps_index, counter);
        BCMDNX_IF_ERR_EXIT(result);
    }

    result = _bcm_dpp_counter_unit_select_update(unit, COUNTER_UPDATE_SELECT_PROC_INFO_ALL | COUNTER_UPDATE_SELECT_FIFO_INFO, unitData, NULL);
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}
int
bcm_dpp_counter_config_get(
        int unit,
        soc_port_t crps_index,
        uint8* enabled,
        int* src_core,
        SOC_TMC_CNT_SRC_TYPE* src_type,
        int* command_id,
        int* command_base_offset,
        SOC_TMC_CNT_MODE_EG_TYPE* eg_type,
        SOC_TMC_CNT_FORMAT* format,
        SOC_TMC_CNT_COUNTER_WE_BITMAP* we_val,
        SOC_TMC_CNT_CUSTOM_MODE_PARAMS* custom_mode_params,
        SOC_TMC_CNT_Q_SET_SIZE *q_set_size,
        uint32* stag_lsb,
        int* counter_profile_offset){
    int result = BCM_E_NONE;
    _bcm_dpp_counter_state_t *unitData;
    /*uint8 is_alloc;*/
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(enabled);
    BCMDNX_NULL_CHECK(src_core);
    BCMDNX_NULL_CHECK(src_type);
    BCMDNX_NULL_CHECK(eg_type);
    BCMDNX_NULL_CHECK(we_val);
    BCMDNX_NULL_CHECK(q_set_size);
    BCMDNX_NULL_CHECK(custom_mode_params);
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_counter_unit_check_and_fill(unit, &unitData, COUNTER_UPDATE_SELECT_ALL, NULL));
    *enabled = unitData->proc[crps_index].valid;
    if (!(unitData->proc[crps_index].valid & BCM_DPP_COUNTER_PROC_INFO_VALID)) {
        BCM_EXIT;
    }
    result = _bcm_dpp_counter_source_config_get(unitData, crps_index, src_core, src_type, command_id, command_base_offset, eg_type, we_val, q_set_size);
    BCMDNX_IF_ERR_EXIT(result);

    custom_mode_params->set_size = unitData->proc[crps_index].mode.custom_mode_params.set_size;
    sal_memcpy(custom_mode_params->entries_bmaps, 
               unitData->proc[crps_index].mode.custom_mode_params.entries_bmaps, 
               sizeof(uint32) * SOC_TMC_CNT_BMAP_OFFSET_COUNT);
    *format = unitData->proc[crps_index].mode.format;
    /* get the configuration of the counter profile offset for ERPP sources */
    if(*src_type == SOC_TMC_CNT_SRC_TYPE_EGR_PP) {
        result = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_stat_counter_erpp_offset_get, (unit, *src_type, *eg_type, counter_profile_offset));
    }
    else{
        *counter_profile_offset = 0;
    }
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_counter_config_clear(int unit, soc_port_t crps_index, uint8 clear_cache){
    _bcm_dpp_counter_state_t *unitData;
    int result = BCM_E_NONE;
    uint64 counter;
    uint8 used_dma;
    int fifo = 0, cache_length;
    BCMDNX_INIT_FUNC_DEFS;

    result = _bcm_dpp_counter_unit_check_and_fill(unit, &unitData, COUNTER_UPDATE_SELECT_ALL, NULL);
    BCMDNX_IF_ERR_EXIT(result);
    used_dma = ((unitData->proc[crps_index].valid & BCM_DPP_COUNTER_PROC_INFO_VALID)  && 
                (!SOC_TMC_CNT_FORMAT_IS_WIDE(unitData->proc[crps_index].mode.format, unitData->proc[crps_index].mode.src_type)));    
    if (clear_cache) {
        COMPILER_64_ZERO(counter);
        result = _bcm_dpp_counter_state_set_all(unitData, crps_index, counter);
        BCMDNX_IF_ERR_EXIT(result);
    }

    if (!(unitData->proc[crps_index].valid & BCM_DPP_COUNTER_PROC_INFO_VALID)) {
        BCM_EXIT;
    }
    if (unitData->proc[crps_index].mode.src_type == SOC_TMC_CNT_SRC_TYPE_STAG) {
        unitData->global_info.haveStatTag = FALSE;
    }
    sal_memset(&unitData->proc[crps_index], 0, sizeof(unitData->proc[crps_index]));
    SOC_TMC_CNT_COUNTERS_INFO_clear(unit, &unitData->proc[crps_index].mode);
    unitData->proc[crps_index].proc_id = crps_index;
    unitData->proc[crps_index].valid = BCM_DPP_COUNTER_PROC_INFO_CONFIGURED_DYNAMICALLY;

    BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_cnt_engine_to_fifo_dma_index, (unit, crps_index, &fifo, &cache_length)));
    if (used_dma) {
        unitData->fifo[fifo].ref_count--;
    }
    /*Update DMA state id needed*/
    result = _bcm_dpp_counter_dma_fifo_update_state(unitData, fifo, SOC_TMC_CNT_INVALID_DMA_CHANNEL);
    BCMDNX_IF_ERR_EXIT(result);

    result = _bcm_dpp_counter_unit_select_update(unit, COUNTER_UPDATE_SELECT_DIAGS | COUNTER_UPDATE_SELECT_PROC_INFO_ALL | COUNTER_UPDATE_SELECT_FIFO_INFO, unitData, NULL);
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_counter_init(int unit)
{
    _bcm_dpp_counter_state_t *tempUnit = NULL;
    int result = BCM_E_NONE;
    uint32 soc_sandRes;
    unsigned int index;
    uint32 entries_bmaps[SOC_TMC_CNT_BMAP_OFFSET_COUNT]; 
    /* 0-Regular 1 - Packet-only, 2 - Byte only, 3-Queue-size 4-unused 5- Packet-and-Packet*/
    unsigned int lastVoqMin[SOC_TMC_CNT_NOF_FORMATS_JERICHO][MAX_NUM_OF_CORES] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};
    unsigned int lastVoqSize[MAX_NUM_OF_CORES] = {1, 1};
    SOC_TMC_CNT_MODE_EG_TYPE eg_type;
    SOC_TMC_CNT_SRC_TYPE src_type;
    SOC_TMC_CNT_Q_SET_SIZE q_set_size;
    SOC_TMC_CNT_MODE_STATISTICS stat;
    SOC_TMC_CNT_COUNTER_WE_BITMAP we_val = 0;
    int src_core = 0;
    int command_id = 0;
    int command_base_offset = 0;
    uint8 valid = FALSE;
    uint32 stag_lsb = 0;
    uint8 is_alloc, is_allocated;
    size_t nof_procs;

    BCMDNX_INIT_FUNC_DEFS;
    _bcm_dpp_counters_def_init();

    if ((0 > unit) || (SOC_MAX_NUM_DEVICES <= unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNIT, (_BSL_BCM_MSG("unit is not valid")));
    }

    /* compute total size of counter state for this unit */               
    nof_procs = (SOC_DPP_DEFS_GET(unit, nof_counter_processors) + SOC_DPP_DEFS_GET(unit, nof_small_counter_processors));

    /*Sandbox*/
    /*Array of the cache counters. Only the first 'counter_fifo_depth' counters are relevant.*/
    _fifo_results[unit].nof_counters = 0;    /* buffer for the data read from fifo by the background thread*/

    sal_memset(&_fifo_results[unit].cnt_result, 0, sizeof(_fifo_results[unit].cnt_result));
    BCMDNX_ALLOC(_fifo_results[unit].cnt_result, (sizeof(*(_fifo_results[unit].cnt_result)) * SOC_DPP_DEFS_GET(unit, counter_fifo_depth)), "FIFO results buffer");
    sal_memset(_fifo_results[unit].cnt_result, 0, (sizeof(*(_fifo_results[unit].cnt_result)) * SOC_DPP_DEFS_GET(unit, counter_fifo_depth)));

    sal_memset(&tempUnit, 0, sizeof(tempUnit));
    BCMDNX_ALLOC(tempUnit, sizeof(*tempUnit), "Unit info buffer");
    sal_memset(tempUnit, 0, sizeof(*tempUnit));

    BCMDNX_ALLOC(tempUnit->proc, (sizeof(*(tempUnit->proc))) * nof_procs, "Proc info buffer");
    sal_memset(tempUnit->proc, 0, (sizeof(*(tempUnit->proc))) * nof_procs);
  
    _bcm_dpp_counter_state_buffer[unit].dirty_bitmap = COUNTER_UPDATE_SELECT_ALL;
    _bcm_dpp_counter_state_buffer[unit].state = NULL;
    /* init bg thread_dma db. each init, driver will configure it from scratch*/
    memset(&glb_bg_thread_and_dma_data[unit], 0, sizeof(bcm_dpp_counter_bg_thread_and_dma_data_t));	
    for (index = 0; index < SOC_DPP_DEFS_GET(unit, nof_counter_fifos); index++) {
        glb_bg_thread_and_dma_data[unit].dma_channel[index] = SOC_TMC_CNT_INVALID_DMA_CHANNEL;
    }    
    
    /* set up initial state */
    if (!SOC_WARM_BOOT(unit)) {
        result = COUNTERS_ACCESS.is_allocated(unit, &is_allocated);
        BCMDNX_IF_ERR_EXIT(result);

        if (is_allocated) {
            /* unit is already inited, detach first then reinit */
            result = bcm_dpp_counter_detach(unit);
            BCMDNX_IF_ERR_EXIT(result);
        }

        result = COUNTERS_ACCESS.alloc(unit);
        BCMDNX_IF_ERR_EXIT(result);

        result = COUNTERS_PROC_ACCESS.alloc(unit, nof_procs);
        BCMDNX_IF_ERR_EXIT(result);

        result = COUNTERS_ACCESS.counter.ptr_alloc(unit, nof_procs);
        BCMDNX_IF_ERR_EXIT(result);

        for (index = 0; index < SOC_DPP_DEFS_GET(unit, nof_counter_processors); index++) {
            result = COUNTERS_ACCESS.counter.alloc(unit, index, SOC_DPP_DEFS_GET(unit, counters_per_counter_processor) * _SUB_COUNT);
            BCMDNX_IF_ERR_EXIT(result);
        }
        for (; index < SOC_DPP_DEFS_GET(unit, nof_counter_processors) + SOC_DPP_DEFS_GET(unit, nof_small_counter_processors); index++) {
            result = COUNTERS_ACCESS.counter.alloc(unit, index, SOC_DPP_DEFS_GET(unit, counters_per_small_counter_processor) * _SUB_COUNT);
            BCMDNX_IF_ERR_EXIT(result);
        }
        for (index = 0; index < SOC_DPP_DEFS_GET(unit, nof_counter_fifos); index++) {
            tempUnit->fifo[index].ref_count = 0;
        }
    } else {
        result = _bcm_dpp_counter_unit_fill(unit, &tempUnit, COUNTER_UPDATE_SELECT_ALL, NULL);
        BCMDNX_IF_ERR_EXIT(result);
    }
    tempUnit->cunit = unit;
    tempUnit->num_counter_procs = nof_procs;
    tempUnit->global_info.cache_only = FALSE;
    /* point to processor descriptor array */
    if (!SOC_WARM_BOOT(unit)) {
        /* set up each processor */
        for (index = 0; index < tempUnit->num_counter_procs; index++) {
            uint32 i;
            sal_memset(entries_bmaps, 0 , sizeof(uint32)*SOC_TMC_CNT_BMAP_OFFSET_COUNT);
            SOC_TMC_CNT_COUNTERS_INFO_clear(unit, &(tempUnit->proc[index].mode));
            SOC_TMC_CNT_MODE_EG_clear(&(tempUnit->proc[index].mode.mode_eg));

            tempUnit->proc[index].proc_id = SOC_TMC_CNT_PROCESSOR_ID_A + index;
            tempUnit->proc[index].mode.num_counters = SOC_TMC_NOF_COUNTERS_IN_ENGINE(unit, index);
            for (i = 0 ; i < bcm_dpp_counter_count ; i++){
                tempUnit->proc[index].avail_offsets[i] = UNSUPPORTED_COUNTER_OFFSET;
            }
            /* get counter processor source */
            /* all but last default to ingress PP, last to egress PP */
            result = _soc_dpp_str_prop_parse_counter_source(unit, index, &src_core, &src_type, &command_id, &eg_type, &we_val, &valid);
            BCMDNX_IF_ERR_EXIT(result);
            if (!(valid & BCM_DPP_COUNTER_PROC_INFO_VALID)) {
                continue;
            }
            result = _soc_dpp_str_prop_parse_counter_format(unit, index, &(tempUnit->proc[index].mode.format));
            if (result != BCM_E_NONE) {
                LOG_ERROR(BSL_LS_BCM_CNT,(BSL_META_U(unit,"unit %d counter %d failed to read counter format property. processors init failed\n"),unit, index));
                BCMDNX_IF_ERR_EXIT(result);
            }

            q_set_size = SOC_TMC_CNT_NOF_Q_SET_SIZES;
            command_base_offset = 0;
            if (src_type == SOC_TMC_CNT_SRC_TYPE_VOQ) {
                result = _soc_dpp_str_prop_parse_counter_voq(
                           tempUnit,
                           index,
                           src_core,
                           src_type,
                           lastVoqMin,
                           lastVoqSize,
                           &command_base_offset,
                           &q_set_size);
                BCMDNX_IF_ERR_EXIT(result);
            }
            if (src_type == SOC_TMC_CNT_SRC_TYPE_STAG) {
                result = _soc_dpp_str_prop_parse_counter_stag(tempUnit, index, src_type, &stag_lsb);
                BCMDNX_IF_ERR_EXIT(result);
            }

            /*config mode by SOC properties*/
            result = _soc_dpp_str_prop_parse_counter_mode(unit, index, src_type, &stat);
            BCMDNX_IF_ERR_EXIT(result);

            /*get the counters bmap from soc properties*/
            if (stat == SOC_TMC_CNT_MODE_STATISTICS_CONFIGURABLE_OFFSETS) {
                result = _soc_dpp_str_prop_parse_counter_mapping(unit, index, &(tempUnit->proc[index].mode.custom_mode_params.set_size), tempUnit->proc[index].mode.custom_mode_params.entries_bmaps);
                BCMDNX_IF_ERR_EXIT(result);
            }
            tempUnit->proc[index].valid = valid;

            result = _bcm_dpp_counter_source_config(tempUnit, index, src_core, src_type, command_id, command_base_offset, eg_type, we_val, q_set_size, stag_lsb);
            BCMDNX_IF_ERR_EXIT(result);

            result = _bcm_dpp_counter_mode_config(tempUnit, index, stat);
            BCMDNX_IF_ERR_EXIT(result);

            result = _bcm_dpp_counter_proc_inUse_config(tempUnit, index);
            BCMDNX_IF_ERR_EXIT(result);

            if (!SOC_TMC_CNT_FORMAT_IS_WIDE(tempUnit->proc[index].mode.format, tempUnit->proc[index].mode.src_type)) {
                int fifo = 0, cache_length = 0;
                BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_cnt_engine_to_fifo_dma_index, (unit, index, &fifo, &cache_length)));
                tempUnit->fifo[fifo].ref_count++;
            }
        } /* for (all counter processors as long as no errors) */
    }
    
    /* Verify conflicts in setting */
    for (index = 0; (index < tempUnit->num_counter_procs); index++) {
        if (tempUnit->proc[index].valid & BCM_DPP_COUNTER_PROC_INFO_VALID)
        {
            result = _bcm_dpp_counter_unit_state_verify(tempUnit, index);
            BCMDNX_IF_ERR_EXIT(result);
        }        
    }
    /* check oam configuration is valid*/
    result = _bcm_dpp_counter_oam_verify(tempUnit);
    BCMDNX_IF_ERR_EXIT(result);

    if (!SOC_WARM_BOOT(unit)) {
        /* only modify hardware state if not warm boot */

        result = bcm_dpp_statistic_interface_config(unit);
        BCMDNX_IF_ERR_EXIT(result);        
        
        for (index = 0; (index < tempUnit->num_counter_procs); index++) {
            if (!(tempUnit->proc[index].valid & BCM_DPP_COUNTER_PROC_INFO_VALID)) {
                continue;
            }
            /* configure the counter processor */
            LOG_DEBUG(BSL_LS_BCM_CNT,(BSL_META_U(unit,"configure unit %d counter proc %u hardware\n"),unit,index));
            soc_sandRes = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_cnt_counters_set,(unit, tempUnit->proc[index].proc_id, &(tempUnit->proc[index].mode))));
            result = handle_sand_result(soc_sandRes);
            BCMDNX_IF_ERR_EXIT(result);
        } /* for (all counter processors as long as no errors) */

        /*LIF-Counting-mapping-set*/
        result = _bcm_dpp_counter_lif_counting_priority_default_set(tempUnit);
        BCMDNX_IF_ERR_EXIT(result);
    }
    COUNTERS_SYNC_ACCESS.cacheLock = sal_mutex_create("dpp cache interlock"); 
    if (!COUNTERS_SYNC_ACCESS.cacheLock) {
         LOG_ERROR(BSL_LS_BCM_CNT,(BSL_META_U(unit,"unable to create unit %d cache"" interlock mutex\n"),unit));
         result = BCM_E_INTERNAL;
         BCMDNX_IF_ERR_EXIT(result);
    }

    /* lif profile mapping init */
    result = _bcm_dpp_counter_profile_mapping_table_init(tempUnit);
    BCMDNX_IF_ERR_EXIT(result);

    /* update if  bg thread and attach to DMA configured */
    if (!SOC_WARM_BOOT(unit))
    {
        tempUnit->sampling_interval_configured = soc_property_get(unit, spn_COUNTER_ENGINE_SAMPLING_INTERVAL, 1);
    }

    if (!SOC_WARM_BOOT(unit))
    {
        /* Set to the SW DB default values for the egress receive tm pointer */
        result = bcm_dpp_counter_egress_receive_tm_pointer_update(unit,QUEUE_PAIR_MASK,TRAFFIC_CLASS_MASK,CAST_MASK,QUEUE_PAIR_SHIFT,TRAFFIC_CLASS_SHIFT,CAST_SHIFT);
        BCMDNX_IF_ERR_EXIT(result);
    }
    _bcm_dpp_counter_state_buffer[unit].state = tempUnit;
    result = _bcm_dpp_counter_unit_update(unit, tempUnit, COUNTER_UPDATE_SELECT_ALL, NULL);
    BCMDNX_IF_ERR_EXIT(result);

    COUNTERS_SYNC_ACCESS.bgSem = NULL;

    /* DMA and Bg thread attach */
    SOC_DPP_ALLOW_WARMBOOT_WRITE(_bcm_dpp_counter_attach_init(tempUnit), result);
    if (result != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_CNT,
                  (BSL_META_U(unit,
                              "unit %d, failed in sched init, error 0x%x\n"), unit, result));
        BCMDNX_IF_ERR_EXIT(result);
    }    
   
    LOG_DEBUG(BSL_LS_BCM_CNT, (BSL_META_U(unit, "unit %d counter processors ready\n"), unit));

    
exit:
    if (BCM_E_NONE != result) {
        /* something went wrong */
        LOG_ERROR(BSL_LS_BCM_CNT,(BSL_META_U(unit,"unit %d counter processors init failed\n"),unit));
        if (!SOC_WARM_BOOT(unit)) {
            COUNTERS_ACCESS.is_allocated(unit, &is_allocated);
            if (is_allocated) {
                for (index = 0; index < tempUnit->num_counter_procs; index++) {
                    COUNTERS_PROC_ACCESS.field.inUse.is_allocated(unit, index, &is_alloc);
                    if (is_alloc) {
                        COUNTERS_PROC_ACCESS.field.inUse.free(unit, index);
                    }
                }
                /* destroy the background thread */
                _bcm_dpp_counter_bg_thread_detach(tempUnit);
                if (COUNTERS_SYNC_ACCESS.cacheLock) {
                    sal_mutex_destroy(COUNTERS_SYNC_ACCESS.cacheLock);
                    COUNTERS_SYNC_ACCESS.cacheLock = NULL;
                }
               COUNTERS_ACCESS.free(unit);
            } /* if (tempUnit) */
        }
        if (tempUnit) {
            if (tempUnit->proc) {
                BCM_FREE(tempUnit->proc);
            }
            BCM_FREE(tempUnit);
        }
        if (_fifo_results[unit].cnt_result) {
            BCM_FREE(_fifo_results[unit].cnt_result);
        }
    } /* if (BCM_E_NONE == result) */
    /*    coverity[leaked_storage : FALSE]    */
    BCMDNX_FUNC_RETURN;
}


int
bcm_dpp_counter_avail_get(int unit,
                          unsigned int proc,
                          bcm_dpp_counter_set_t *avail,
                          bcm_dpp_counter_set_t *native)
{
    _bcm_dpp_counter_state_t *unitData;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_counter_unit_check_and_fill(unit, &unitData, COUNTER_UPDATE_SELECT_ALL, NULL));
    if (unitData->num_counter_procs <= proc) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("invalid counter processor ID %u"),
                          proc));
    }
    if (avail) {
        uint32 i;
        SHR_BITCLR_RANGE(*avail , 0, bcm_dpp_counter_count);
        for( i = 0; i < bcm_dpp_counter_count; i++){
            if(unitData->proc[proc].avail_offsets[i] != UNSUPPORTED_COUNTER_OFFSET){
                SHR_BITSET(*avail , i);
            }
        }
    }
    if (native) {
        sal_memcpy(native, &(unitData->proc[proc].native), sizeof(*native));
    }
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_counter_get(int unit,
                         unsigned int proc,
                         unsigned int set_id,
                         bcm_dpp_counter_t type,
                         uint64 *stat)
{
    
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_counter_access(unit,
                                   proc,
                                   set_id,
                                   FALSE /* update */,
                                   FALSE /* cache only */,
                                   1 /* num_counters */,
                                   &type,
                                   stat));
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_counter_get_cached(int unit,
                           unsigned int proc,
                           unsigned int set_id,
                           bcm_dpp_counter_t type,
                           uint64 *stat)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_counter_access(unit,
                                   proc,
                                   set_id,
                                   FALSE /* update */,
                                   TRUE /* cache only */,
                                   1 /* num_counters */,
                                   &type,
                                   stat));
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_counter_set(int unit,
                    unsigned int proc,
                    unsigned int set_id,
                    bcm_dpp_counter_t type,
                    uint64 stat)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_counter_access(unit,
                                   proc,
                                   set_id,
                                   TRUE /* update */,
                                   FALSE /* cache only */,
                                   1 /* num_counters */,
                                   &type,
                                   &stat));
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_counter_set_cached(int unit,
                           unsigned int proc,
                           unsigned int set_id,
                           bcm_dpp_counter_t type,
                           uint64 stat)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_counter_access(unit,
                                   proc,
                                   set_id,
                                   TRUE /* update */,
                                   TRUE /* cache only */,
                                   1 /* num_counters */,
                                   &type,
                                   &stat));
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_counter_multi_get(int unit,
                          unsigned int proc,
                          unsigned int set_id,
                          unsigned int count,
                          const bcm_dpp_counter_t *type,
                          uint64 *stat)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_counter_access(unit,
                                   proc,
                                   set_id,
                                   FALSE /* update */,
                                   FALSE /* cache only */,
                                   count,
                                   type,
                                   stat));
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_counter_multi_get_cached(int unit,
                                 unsigned int proc,
                                 unsigned int set_id,
                                 unsigned int count,
                                 const bcm_dpp_counter_t *type,
                                 uint64 *stat)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_counter_access(unit,
                                   proc,
                                   set_id,
                                   FALSE /* update */,
                                   TRUE /* cache only */,
                                   count,
                                   type,
                                   stat));
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_counter_multi_set(int unit,
                          unsigned int proc,
                          unsigned int set_id,
                          unsigned int count,
                          const bcm_dpp_counter_t *type,
                          /*const*/ uint64 *stat)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_counter_access(unit,
                                   proc,
                                   set_id,
                                   TRUE /* update */,
                                   FALSE /* cache only */,
                                   count,
                                   type,
                                   stat));
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_counter_multi_set_cached(int unit,
                                 unsigned int proc,
                                 unsigned int set_id,
                                 unsigned int count,
                                 const bcm_dpp_counter_t *type,
                                 /*const*/ uint64 *stat)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_counter_access(unit,
                                   proc,
                                   set_id,
                                   TRUE /* update */,
                                   TRUE /* cache only */,
                                   count,
                                   type,
                                   stat));
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_counter_diag_info_get(int unit,
                              bcm_dpp_counter_diag_info_t info,
                              unsigned int proc,
                              unsigned int *value)
{
    _bcm_dpp_counter_state_t *unitData;
    uint8 is_alloc = 0;
    unsigned int allocated = 0;
    int fifo = 0, cache_length = 0;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_counter_unit_check_and_fill(unit, &unitData, COUNTER_UPDATE_SELECT_ALL, NULL));
    if (unitData->num_counter_procs <= proc) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("counter processor %u is not valid"),
                          proc));
    }
    if (!value) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("obligatory out arg must not be NULL")));
    }
    SOCDNX_SAND_IF_ERR_RETURN(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_cnt_engine_to_fifo_dma_index, (unit, proc, &fifo, &cache_length)));
    switch (info) {
    /* global */
    case bcm_dpp_counter_diag_info_num_proc:
        *value = unitData->num_counter_procs;
        break;
    case bcm_dpp_counter_diag_info_bg_active:
        *value = unitData->background_active;
        break;
    case bcm_dpp_counter_diag_info_bg_suspend:
        *value = unitData->background_disable;
        break;
    case bcm_dpp_counter_diag_info_bg_defer:
        *value = unitData->background_defer;
        break;
    case bcm_dpp_counter_diag_info_fg_hit:
        *value = unitData->foreground_hit;
        break;
    case bcm_dpp_counter_diag_info_fifo_read_background:
        *value = unitData->diag.fifo_read_background;
        break;
    case bcm_dpp_counter_diag_info_fifo_read_deferred:
        *value = unitData->diag.fifo_read_deferred;
        break;
    /* per processor */
    case bcm_dpp_counter_diag_info_source:
        *value = unitData->proc[proc].mode.src_type;
        break;
    case bcm_dpp_counter_diag_info_msb_patten:
        *value = unitData->proc[proc].mode.we_val;
        break;
    case bcm_dpp_counter_diag_info_command_id:
        *value = unitData->proc[proc].mode.command_id;
        break;
    case bcm_dpp_counter_diag_info_stat_tag_low_bit:
        if (SOC_TMC_CNT_SRC_TYPE_STAG == unitData->proc[proc].mode.src_type) {
            *value = unitData->proc[proc].mode.stag_lsb;
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                             (_BSL_BCM_MSG("stat_tag_low_bit not supported for"
                                       " processor %u source %s"),
                              proc,
                              SOC_TMC_CNT_SRC_TYPE_to_string(unitData->proc[proc].mode.src_type)));
        }
        break;
    case bcm_dpp_counter_diag_info_voq_base:
        if (SOC_TMC_CNT_SRC_TYPE_VOQ == unitData->proc[proc].mode.src_type) {
            *value = unitData->proc[proc].mode.mode_eg.base_val;
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                             (_BSL_BCM_MSG("voq_base not supported for"
                                       " processor %u source %s"),
                              proc,
                              SOC_TMC_CNT_SRC_TYPE_to_string(unitData->proc[proc].mode.src_type)));
        }
        break;
    case bcm_dpp_counter_diag_info_voq_per_set:
        if (SOC_TMC_CNT_SRC_TYPE_VOQ == unitData->proc[proc].mode.src_type) {
            *value = unitData->proc[proc].mode.q_set_size;
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                             (_BSL_BCM_MSG("voq_per_set not supported for"
                                       " processor %u source %s"),
                              proc,
                              SOC_TMC_CNT_SRC_TYPE_to_string(unitData->proc[proc].mode.src_type)));
        }
        break;
    case bcm_dpp_counter_diag_info_fifo_read_passes:
        *value = unitData->fifo[fifo].fifo_read_passes;
        break;
    case bcm_dpp_counter_diag_info_fifo_read_fails:
        *value = unitData->fifo[fifo].fifo_read_fails;
        break;
    case bcm_dpp_counter_diag_info_fifo_read_items:
        *value = unitData->fifo[fifo].fifo_read_items;
        break;
    case bcm_dpp_counter_diag_info_fifo_read_max:
        *value = unitData->fifo[fifo].fifo_read_max;
        break;
    case bcm_dpp_counter_diag_info_fifo_read_last:
        *value = unitData->fifo[fifo].fifo_read_last;
        break;
    case bcm_dpp_counter_diag_info_direct_read_passes:
        *value = unitData->proc[proc].diag.direct_read_passes;
        break;
    case bcm_dpp_counter_diag_info_direct_read_fails:
        *value = unitData->proc[proc].diag.direct_read_fails;
        break;
    case bcm_dpp_counter_diag_info_cache_updates:
        *value = unitData->proc[proc].diag.cache_updates;
        break;
    case bcm_dpp_counter_diag_info_cache_reads:
        *value = unitData->proc[proc].diag.cache_reads;
        break;
    case bcm_dpp_counter_diag_info_cache_writes:
        *value = unitData->proc[proc].diag.cache_writes;
        break;
    case bcm_dpp_counter_diag_info_api_reads:
        *value = unitData->proc[proc].diag.api_reads;
        break;
    case bcm_dpp_counter_diag_info_api_writes:
        *value = unitData->proc[proc].diag.api_writes;
        break;
    case bcm_dpp_counter_diag_info_api_miss_reads:
        *value = unitData->proc[proc].diag.api_miss_reads;
        break;
    case bcm_dpp_counter_diag_info_api_miss_writes:
        *value = unitData->proc[proc].diag.api_miss_writes;
        break;
    case bcm_dpp_counter_diag_info_format:
        *value = unitData->proc[proc].mode.mode_statistics;
        break;
    case bcm_dpp_counter_diag_info_counters:
        *value = unitData->proc[proc].mode.num_counters;
        break;
    case bcm_dpp_counter_diag_info_sets:
        *value = unitData->proc[proc].mode.num_sets;
        break;
    case bcm_dpp_counter_diag_info_alloc_local:
        BCMDNX_IF_ERR_EXIT(COUNTERS_PROC_ACCESS.field.inUse.is_allocated(unit, proc, &is_alloc));
        *value = is_alloc;
        break;
    case bcm_dpp_counter_diag_info_alloc_inuse:
        BCMDNX_IF_ERR_EXIT(COUNTERS_PROC_ACCESS.field.allocated.get(unit, proc, &allocated));
        *value = allocated;
        break;
    case bcm_dpp_counter_diag_info_mode:
    	*value = unitData->proc[proc].mode.mode_eg.type;
    	break;
    case bcm_dpp_counter_diag_info_range_start:
    	/* range_start=numOfSets*MSBs+Base */
    	*value = unitData->proc[proc].mode.num_sets*unitData->proc[proc].mode.we_val + unitData->proc[proc].mode.mode_eg.base_val;
    	break;
    case bcm_dpp_counter_diag_info_range_end:
    	/* range_end=numOfSets*(MSBs+1)+Base -1 */
    	*value = (unitData->proc[proc].mode.num_sets*(unitData->proc[proc].mode.we_val + 1) + unitData->proc[proc].mode.mode_eg.base_val) - 1;
    	break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("invalid info item %d"),
                          info));
    }
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_counter_match_stat_dp (_bcm_dpp_counter_state_t *unitData,
                                int proc,
                                bcm_dpp_counter_t ctrStat, 
                                unsigned int dp, 
                                unsigned int *match,
                                unsigned int *use_color /* If True, the color of stat was used */
                                )
{
    int unit = unitData->cunit;
    unsigned int dp_match = 0;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(unitData);
    BCMDNX_NULL_CHECK(match);
    BCMDNX_NULL_CHECK(use_color);
    
    switch (ctrStat) {
    case bcm_dpp_counter_green_pkts:
    case bcm_dpp_counter_green_bytes:
    case bcm_dpp_counter_fwd_green_bytes:
    case bcm_dpp_counter_fwd_green_pkts:
    case bcm_dpp_counter_drop_green_pkts:
    case bcm_dpp_counter_drop_green_bytes:
        /* Green only */
        dp_match = (dp == 0);
        break;
    case bcm_dpp_counter_yellow_pkts:
    case bcm_dpp_counter_yellow_bytes:
    case bcm_dpp_counter_fwd_yellow_pkts:
    case bcm_dpp_counter_fwd_yellow_bytes:
    case bcm_dpp_counter_drop_yellow_pkts:
    case bcm_dpp_counter_drop_yellow_bytes:
        dp_match = (dp == 1) || (dp == 2);
        break;
    case bcm_dpp_counter_red_pkts:
    case bcm_dpp_counter_red_bytes:
    case bcm_dpp_counter_fwd_red_pkts:
    case bcm_dpp_counter_fwd_red_bytes:
    case bcm_dpp_counter_drop_red_pkts:
    case bcm_dpp_counter_drop_red_bytes:
        dp_match = (dp == 3);
        break;
    case bcm_dpp_counter_not_green_pkts:
    case bcm_dpp_counter_not_green_bytes:
    case bcm_dpp_counter_fwd_not_green_pkts:
    case bcm_dpp_counter_fwd_not_green_bytes:
    case bcm_dpp_counter_drop_not_green_pkts:
    case bcm_dpp_counter_drop_not_green_bytes:
        dp_match = (dp != 0);
        break;
    case bcm_dpp_counter_drop_bytes:
    case bcm_dpp_counter_drop_pkts:
    case bcm_dpp_counter_fwd_bytes:
    case bcm_dpp_counter_fwd_pkts:
    case bcm_dpp_counter_bytes:
    case bcm_dpp_counter_pkts:
        dp_match = 1;
        break;
    default:
        dp_match = 0;
        break;
    }


    
    /*the all no color is blocked in the init because there are missing enum value for yellow1, yellow2 and red */
    switch (unitData->proc[proc].mode.mode_statistics) {
    case SOC_TMC_CNT_MODE_STATISTICS_ALL_NO_COLOR: 
    case SOC_TMC_CNT_MODE_STATISTICS_NO_COLOR: 
    case SOC_TMC_CNT_MODE_STATISTICS_DROP_NO_COLOR:
    case SOC_TMC_CNT_MODE_STATISTICS_FWD_NO_COLOR:
        *use_color = 1;
        *match = dp_match;
        break;
    case SOC_TMC_CNT_MODE_STATISTICS_CONFIGURABLE_OFFSETS:
        if (unitData->proc[proc].avail_offsets[ctrStat] == UNSUPPORTED_COUNTER_OFFSET) {
            *use_color = 0;
            *match = 0;
        } else {
            *use_color = 1;
            *match = dp_match;
        }
        break;
    default:
        *match = 0;
        *use_color = 0;
        break;
    }
exit:
    BCMDNX_FUNC_RETURN;
}
/*When bcm_petra_stat_egress_receive_tm_pointer_format_set is called - the DB is updated*/
int
bcm_dpp_counter_egress_receive_tm_pointer_update( int unit,
                                                  uint32 queue_pair_mask,
                                                  uint32 traffic_class_mask,
                                                  uint32 cast_mask,
                                                  uint32 queue_pair_shift,
                                                  uint32 traffic_class_shift,
                                                  uint32 cast_shift){
    int rv=BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    rv = COUNTERS_ACCESS.egress_receive_tm_pointer_format.queue_pair_mask.set(unit,queue_pair_mask);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = COUNTERS_ACCESS.egress_receive_tm_pointer_format.traffic_class_mask.set(unit,traffic_class_mask);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = COUNTERS_ACCESS.egress_receive_tm_pointer_format.cast_mask.set(unit,cast_mask);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = COUNTERS_ACCESS.egress_receive_tm_pointer_format.queue_pair_shift.set(unit,queue_pair_shift);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = COUNTERS_ACCESS.egress_receive_tm_pointer_format.traffic_class_shift.set(unit,traffic_class_shift);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = COUNTERS_ACCESS.egress_receive_tm_pointer_format.cast_shift.set(unit,cast_shift);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_counter_egress_receive_tm_build_pointer(_bcm_dpp_counter_state_t *unitData,
                                                 uint32 out_port_field,
                                                 int cosq,
                                                 int priority,
                                                 int is_mc,
                                                 unsigned int *lval) {
    int rv=BCM_E_NONE;
    int unit = unitData->cunit;
    bcm_stat_egress_receive_tm_pointer_format_t pointer_format;
    uint32 queue_pair_shift, traffic_class_shift, cast_shift;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(unitData);

    rv = COUNTERS_ACCESS.egress_receive_tm_pointer_format.queue_pair_mask.get(unit,&pointer_format.queue_pair_mask);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = COUNTERS_ACCESS.egress_receive_tm_pointer_format.traffic_class_mask.get(unit,&pointer_format.traffic_class_mask);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = COUNTERS_ACCESS.egress_receive_tm_pointer_format.cast_mask.get(unit,&pointer_format.cast_mask);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = COUNTERS_ACCESS.egress_receive_tm_pointer_format.queue_pair_shift.get(unit,&queue_pair_shift);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = COUNTERS_ACCESS.egress_receive_tm_pointer_format.traffic_class_shift.get(unit,&traffic_class_shift);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = COUNTERS_ACCESS.egress_receive_tm_pointer_format.cast_shift.get(unit,&cast_shift);
    BCMDNX_IF_ERR_EXIT(rv);

    /* default value : lval = (((out_port_field + cosq) & 0xff) << 4) | ((priority & 0x7) << 1) | is_mc; */
    *lval = (((out_port_field + cosq) & pointer_format.queue_pair_mask) << queue_pair_shift) | 
                 ((priority & pointer_format.traffic_class_mask)  << traffic_class_shift) | 
                 ((is_mc & pointer_format.cast_mask) << cast_shift);    

exit:
    BCMDNX_FUNC_RETURN;
}
int
_bcm_dpp_counter_egress_que_build_pointer(_bcm_dpp_counter_state_t *unitData,
                                          int max_set_cnt,
                                          int proc,
                                          unsigned int tm_port,
                                          unsigned int pp_port,
                                          int core_id,
                                          unsigned int priority,
                                          unsigned int system_mc,
                                          unsigned int is_mc,
                                          unsigned int dp,
                                          bcm_dpp_counter_proc_and_set_t *proc_set_array,
                                          unsigned int *set_cnt
                                          ) {
    int cosq, i;
    int nof_counters_per_entry;
    bcm_error_t result;
    uint32 soc_sand_rc;
    int unit = unitData->cunit;
    unsigned int lval;
    unsigned int counters_in_set;
    unsigned int base;
    uint32 out_port_field;
    uint32 base_q_pair = 0;
    uint32 nof_priorities;
    soc_port_t logical_port;    
    unsigned int counter_ptr_arr[8];
    
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(unitData);
    BCMDNX_NULL_CHECK(proc_set_array);
    BCMDNX_NULL_CHECK(set_cnt);

    sal_memset(counter_ptr_arr, -1, sizeof(unsigned int)*8);
    
    result = _bcm_dpp_counters_per_set(unitData, proc, &counters_in_set);
    BCMDNX_IF_ERR_EXIT(result);

    if (unitData->proc[proc].mode.mode_eg.type == SOC_TMC_CNT_MODE_EG_TYPE_TM) {
        soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_egr_dsp_pp_to_base_q_pair_get, (unit, unitData->proc[proc].mode.src_core, tm_port, &base_q_pair));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rc); 

        BCM_DPP_UNIT_CHECK(unit);

        out_port_field = base_q_pair;
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_tm_to_local_port_get(unit, core_id, tm_port, &logical_port));
        result = soc_port_sw_db_local_to_out_port_priority_get(unit, logical_port, &nof_priorities);
        BCMDNX_IF_ERR_EXIT(result);

    } else if (unitData->proc[proc].mode.mode_eg.type == SOC_TMC_CNT_MODE_EG_TYPE_TM_PORT){
        out_port_field = pp_port;
        nof_priorities = 1;
    } else {
        LOG_ERROR(BSL_LS_BCM_CNT, (BSL_META_U(unit, "Src mode 0x%x is not of an egress queue\n"), unitData->proc[proc].mode.mode_eg.type));
        BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);
    }
    for (cosq = 0 ; cosq < nof_priorities; cosq++) {
        if (max_set_cnt < *set_cnt) {
            LOG_ERROR(BSL_LS_BCM_CNT, (BSL_META_U(unit, "More counters %d than counter limit %d\n"), *set_cnt, max_set_cnt));
            BCMDNX_IF_ERR_EXIT(BCM_E_INTERNAL);
        }
        /*Calculate the number of counters per set*/
        if (unitData->proc[proc].mode.src_type == SOC_TMC_CNT_SRC_TYPES_EGQ_TM) {
            /* 
             * EGQ TM Macro build - engines 16,17
             */ 
            result = _bcm_dpp_counter_egress_receive_tm_build_pointer(unitData,out_port_field,cosq,priority,is_mc,&lval);
            BCMDNX_IF_ERR_EXIT(result);

            /** add each counter pointer once. since the q_pair mask can remove the 3 lsb bits, */
            /*   there might be a situation that for different cosqs, the counter pointer will be the same. */
            counter_ptr_arr[cosq] = lval;
            for(i=0; i < cosq; i++)
            {
                if(counter_ptr_arr[i] == lval)
                {
                    break;
                }
            }
            if(i < cosq) continue;
            
        } else if (unitData->proc[proc].mode.src_type == SOC_TMC_CNT_SRC_TYPE_EGR_PP) {
            /* 
             * EGQ TM Macro build 
             */ 
            lval = (((out_port_field + cosq) & 0xff) << 6) | ((priority & 0x7) << 3 ) | ((dp & 0x3) << 1) | (is_mc & 0x1);
        } else if (unitData->proc[proc].mode.src_type == SOC_TMC_CNT_SRC_TYPE_EPNI){
            /* 
             * EPNI TM Macro build 
             */ 
             if(SOC_IS_JERICHO_PLUS(unit)) /* include QAX */
             {
                /* from spec - 7.19.1.3.1.3	 Create Port-Counter-Pointer */
                lval = (((out_port_field + cosq) & 0xff) << 4) | ((priority & 0x7) << 1) | (is_mc & 0x1);                
             }
             else
             {
                lval = (((is_mc & 0x1) << 14) | (system_mc & 0x1) << 13) | (((out_port_field + cosq) & 0xff) << 5) | ((priority & 0x7) << 2) | (dp & 0x3);                
             }
        } else {
            LOG_ERROR(BSL_LS_BCM_CNT, (BSL_META_U(unit, "Src type 0x%x is not of an egress queue\n"), unitData->proc[proc].mode.src_type));
            BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);
        }
        /*validate that the counter is in range.*/
        nof_counters_per_entry = BCM_DPP_NOF_COUNTERS_PER_ENTRY_GET(unit, unitData->proc[proc].mode.format);        
        base = lval * counters_in_set;
        if (((base >= (unitData->proc[proc].mode.num_counters * nof_counters_per_entry * unitData->proc[proc].mode.we_val)) && 
               ((base + counters_in_set - 1) < (unitData->proc[proc].mode.num_counters * nof_counters_per_entry * (unitData->proc[proc].mode.we_val + 1))))) {
            proc_set_array[*set_cnt].ctr_proc_id = proc;
            proc_set_array[*set_cnt].ctr_set_id = lval;
            (*set_cnt)++;
        }
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_counter_find_egress_que(int unit,
                         unsigned int max_set_cnt,
                         unsigned int offset,
                         int core_id,
                         unsigned int tm_port,
                         unsigned int is_mc,
                         unsigned int pp_port,
                         bcm_dpp_counter_t ctrStat,
                         unsigned int *use_color, /* If True, the color of stat was used */
                         bcm_dpp_counter_proc_and_set_t *proc_set_array,
                         unsigned int *set_cnt)
{
    _bcm_dpp_counter_state_t *unitData;
    unsigned int dp;
    unsigned int system_mc;
    unsigned int proc_index, match;
    unsigned int set_index = 0;
    int result = BCM_E_NOT_FOUND, found_result = BCM_E_NOT_FOUND;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(use_color);
    BCMDNX_NULL_CHECK(proc_set_array);
    BCMDNX_NULL_CHECK(set_cnt);
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_counter_unit_check_and_fill(unit, &unitData, COUNTER_UPDATE_SELECT_ALL, NULL));
    *set_cnt = 0;

    for (proc_index = 0; proc_index < unitData->num_counter_procs; proc_index++) {
        if (!(unitData->proc[proc_index].valid & BCM_DPP_COUNTER_PROC_INFO_VALID)) {
            continue;
        }
        if (unitData->proc[proc_index].mode.mode_eg.type == SOC_TMC_CNT_MODE_EG_TYPE_TM || 
            unitData->proc[proc_index].mode.mode_eg.type == SOC_TMC_CNT_MODE_EG_TYPE_TM_PORT) {
            if (unitData->proc[proc_index].mode.src_core != core_id) {
                continue;
            }
            /* how many counters to sync with hardware, according to format */
            /* the system multicast are in the upper 4k ID - which means that they are in the MSB - have the check reflecting that*/
            /* Choose out_port according to queue pair or pp port */
            /* if system_mc :loop over all entries and create with mc bit (14) =  0/1
            otherwise only entries with mc=0 are created. */               
            for (system_mc = 0; system_mc < 2; system_mc++) {
                if ((unitData->proc[proc_index].mode.src_type == SOC_TMC_CNT_SRC_TYPE_EGR_PP || 
                     unitData->proc[proc_index].mode.src_type == SOC_TMC_CNT_SRC_TYPES_EGQ_TM ||
                     SOC_IS_JERICHO(unit) /*In jericho we mask out system_mc*/) && system_mc) {
                    /*No system mc for EGQ source type*/
                    continue;
                }
                for (dp = 0; dp < SOC_TMC_NOF_DROP_PRECEDENCE; dp++) {
                    if (unitData->proc[proc_index].mode.src_type == SOC_TMC_CNT_SRC_TYPES_EGQ_TM && dp) {
                        /*No dp usage for EGQ-TM 16,17 source types*/
                        break;
                    }
                    result = _bcm_dpp_counter_match_stat_dp(unitData, proc_index, ctrStat, dp, &match, use_color);
                    BCMDNX_IF_ERR_EXIT(result);
                    if ((SOC_IS_JERICHO_PLUS(unit) == TRUE) && (unitData->proc[proc_index].mode.src_type == SOC_TMC_CNT_SRC_TYPE_EPNI) && (dp != 0))
                    {                       
                        /*From Spec: No need for DP, DP is part of the counter-set*/
                        break;
                    }   
                    /* since DP is part of the counter_set, there is no meaing to check the if dp match. we can consider it as always a match */
                    if((SOC_IS_JERICHO_PLUS(unit) == TRUE) && (unitData->proc[proc_index].mode.src_type == SOC_TMC_CNT_SRC_TYPE_EPNI))
                    {
                        match = TRUE;
                    }

                    if (match) {
                        set_index = 0;
                        result = _bcm_dpp_counter_egress_que_build_pointer(
                                    unitData, 
                                    max_set_cnt - (*set_cnt), 
                                    proc_index, 
                                    tm_port, 
                                    pp_port, 
                                    core_id,
                                    offset, 
                                    system_mc, 
                                    is_mc, 
                                    dp, 
                                    &proc_set_array[*set_cnt],
                                    &set_index);
                        BCMDNX_IF_ERR_EXIT(result);
                        /*not matched - return error*/
                        if(set_index == 0) {
                            continue;
                        }
                        if (set_cnt) {
                            *set_cnt += set_index;
                        }
                        /*We had a match*/
                        found_result = BCM_E_NONE;
                    }
                }
            }
        }
    } /* for (index = 0; index < unitData->num_counter_procs; index++) */
    BCMDNX_IF_ERR_EXIT(found_result);
exit:
      BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_counter_find_voq(int unit,
                         int core_id,
                         unsigned int voq,
                         bcm_dpp_counter_proc_and_set_t *proc_and_set,
                         bcm_dpp_counter_t ctrStat,
                         int* nof_found,
                         unsigned int *first,
                         unsigned int *last)
{
    _bcm_dpp_counter_state_t *unitData;
    unsigned int lfirst;
    unsigned int lcount;
    unsigned int lscale;
    unsigned int lindex;
    unsigned int index;
    int result = BCM_E_NOT_FOUND;
    int core_index;
    int current_proc_and_set_index = 0;
    BCMDNX_INIT_FUNC_DEFS;
    _COUNTER_UNIT_CHECK(unit, unitData);
    BCMDNX_NULL_CHECK(proc_and_set);
    BCMDNX_NULL_CHECK(nof_found);

    *nof_found = 0;
    BCM_DPP_CORES_ITER(core_id, core_index) {
        for (index = 0; index < unitData->num_counter_procs; index++) {
            if (SOC_TMC_CNT_SRC_TYPE_VOQ == unitData->proc[index].mode.src_type && unitData->proc[index].mode.src_core == core_index) {
                lscale = unitData->proc[index].mode.q_set_size;
                lcount = lscale * unitData->proc[index].mode.num_sets;
                lfirst = unitData->proc[index].mode.mode_eg.base_val + (lcount * unitData->proc[index].mode.we_val);

                if ((voq >= lfirst) && (voq < (lfirst + lcount))) {
                    /* Check also the Counter type matches, except for bcm_dpp_counter_count: bypass for enable_set/get */
                    if ((ctrStat == bcm_dpp_counter_count)
                        || SHR_BITGET(unitData->proc[index].native, ctrStat) || unitData->proc[index].avail_offsets[ctrStat] != UNSUPPORTED_COUNTER_OFFSET
                        ) {
                        /* found it */
                        lindex = voq / lscale;
                        if (proc_and_set) {
                            proc_and_set[current_proc_and_set_index].ctr_proc_id = index;
                            proc_and_set[current_proc_and_set_index].ctr_set_id = lindex;
                            ++current_proc_and_set_index;
                            *nof_found += 1;
                        }
                        if (first) {
                            *first = (lindex * lscale);
                        }
                        if (last) {
                            *last = (lindex * lscale) + (lscale - 1);
                        }
                        result = BCM_E_NONE;
                        break;

                    }
                } /* if ((voq >= lfirst) && (voc < (lfirst + lcount)) */
            } /* if (proc is sourced from ingress voq) */
        } /* for (index = 0; index < unitData->num_counter_procs; index++) */
    }
    BCMDNX_IF_ERR_EXIT(result);
exit:
      BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_counter_find_stag(int unit,
                          unsigned int stag,
                          unsigned int *proc,
                          unsigned int *set,
                          unsigned int *first,
                          unsigned int *last)
{
    _bcm_dpp_counter_state_t *unitData;
    unsigned int lfirst;
    unsigned int lcount;
    unsigned int lscale;
    unsigned int lindex;
    unsigned int index;
    int result = BCM_E_NOT_FOUND;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_counter_unit_check_and_fill(unit, &unitData, COUNTER_UPDATE_SELECT_ALL, NULL));
    BCMDNX_NULL_CHECK(proc);
    BCMDNX_NULL_CHECK(set);
    for (index = 0; index < unitData->num_counter_procs; index++) {
        if (SOC_TMC_CNT_SRC_TYPE_STAG == unitData->proc[index].mode.src_type) {
            lscale = 1 << unitData->proc[index].mode.stag_lsb;
            lcount = lscale * unitData->proc[index].mode.num_sets;
            lfirst = unitData->proc[index].mode.mode_eg.base_val + (lcount * unitData->proc[index].mode.we_val);
            if ((stag >= lfirst) && (stag < (lfirst + lcount))) {
                /* found it */
                lindex = stag / lscale;
                *proc = index;
                *set = lindex;
                if (first) {
                    *first = (lindex * lscale);
                }
                if (last) {
                    *last = (lindex * lscale) + (lscale - 1);
                }
                result = BCM_E_NONE;
                break;
            } /* if ((voq >= lfirst) && (voc < (lfirst + lcount)) */
        } /* if (proc is sourced from ingress voq) */
    } /* for (index = 0; index < unitData->num_counter_procs; index++) */
    BCMDNX_IF_ERR_EXIT(result);
exit:
      BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_counter_find_vsi(int unit,
                         unsigned int vsi,
                         unsigned int *proc,
                         unsigned int *set)
{
    _bcm_dpp_counter_state_t *unitData;
    unsigned int index;
    unsigned int lfirst;
    int result = BCM_E_NOT_FOUND;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_counter_unit_check_and_fill(unit, &unitData, COUNTER_UPDATE_SELECT_ALL, NULL));
    BCMDNX_NULL_CHECK(proc);
    BCMDNX_NULL_CHECK(set);
    for (index = 0; index < unitData->num_counter_procs; index++) {
        /*if (_bcm_dpp_counter_source_egress_vsi == unitData->proc[index].mode.src_type) {*/
        if ((SOC_TMC_CNT_SRC_TYPE_EGR_PP == unitData->proc[index].mode.src_type) 
             && 
             ( unitData->proc[index].mode.mode_eg.type == SOC_TMC_CNT_MODE_EG_TYPE_VSI)) {
            lfirst = unitData->proc[index].mode.mode_eg.base_val + (unitData->proc[index].mode.num_sets * unitData->proc[index].mode.we_val);
            if ((vsi >= lfirst) &&
                (vsi < (lfirst + unitData->proc[index].mode.num_sets))) {
                /* found it */
                *proc = index;
                *set = vsi;
                result = BCM_E_NONE;
                break;
            } /* if (vsi is in range supported by this processor) */
        } /* if (proc is sourced from egress vsi) */
    } /* for (index = 0; index < unitData->num_counter_procs; index++) */
    BCMDNX_IF_ERR_EXIT(result);
exit:
      BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_counter_find_outlif(int unit,
                            unsigned int outlif,
                            unsigned int *proc,
                            unsigned int *set)
{
    _bcm_dpp_counter_state_t *unitData;
    unsigned int lfirst;
    unsigned int index;
    int result = BCM_E_NOT_FOUND;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_counter_unit_check_and_fill(unit, &unitData, COUNTER_UPDATE_SELECT_ALL, NULL));
    BCMDNX_NULL_CHECK(proc);
    BCMDNX_NULL_CHECK(set);
    for (index = 0; index < unitData->num_counter_procs; index++) {
        /*if (_bcm_dpp_counter_source_egress_outlif == unitData->proc[index].mode.src_type) {*/
        if ((SOC_TMC_CNT_SRC_TYPE_EGR_PP == unitData->proc[index].mode.src_type) 
             && 
             ( unitData->proc[index].mode.mode_eg.type == SOC_TMC_CNT_MODE_EG_TYPE_OUTLIF)) {
            lfirst = unitData->proc[index].mode.mode_eg.base_val + (unitData->proc[index].mode.num_sets * unitData->proc[index].mode.we_val);
            if ((outlif >= lfirst) &&
                (outlif < (lfirst + unitData->proc[index].mode.num_sets))) {
                /* found it */
                *proc = index;
                *set = outlif;
                result = BCM_E_NONE;
                break;
            } /* if (outlif is in range supported by this processor) */
        } /* if (proc is sourced from egress outlif) */
    } /* for (index = 0; index < unitData->num_counter_procs; index++) */
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_counter_alloc(int unit,
                      uint32 flags,
                      _bcm_dpp_counter_source_t source,
                      unsigned int *proc,
                      unsigned int *set)
{
    _bcm_dpp_counter_state_t *unitData;
    unsigned int xProc = ~0;
    unsigned int xSet = ~0;
    int result;
    int auxRes;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_counter_unit_check_and_fill(unit, &unitData, COUNTER_UPDATE_SELECT_ALL, NULL));

    if (flags & BCM_DPP_COUNTER_WITH_PROC) {
        xProc = *proc;
    }
    if (flags & BCM_DPP_COUNTER_WITH_ID) {
        xProc = *proc;
        xSet = *set;
    }
    result = _bcm_dpp_counter_alloc(unitData, flags, source, &xProc, &xSet);
    if (BCM_E_NONE == result) {
        /* make sure counter set is cleared when allocating */
        result = _bcm_dpp_counter_clear(unitData, xProc, xSet);
        if (BCM_E_NONE == result) {
            *proc = xProc;
            *set = xSet;
        } else {
            auxRes = _bcm_dpp_counter_free(unitData, xProc, xSet);
            BCMDNX_IF_ERR_EXIT(auxRes);
        }
    }
    if (flags & BCM_DPP_COUNTER_WITH_ID)
    {
        BCMDNX_IF_ERR_EXIT(result);
    }
    else
    {
        BCMDNX_IF_ERR_VERB_EXIT(result);
    }

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_counter_unit_update(unit, unitData, COUNTER_UPDATE_SELECT_ALL, NULL));

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_counter_free(int unit,
                     unsigned int proc,
                     unsigned int set)
{
    _bcm_dpp_counter_state_t *unitData;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_counter_unit_check_and_fill(unit, &unitData, COUNTER_UPDATE_SELECT_ALL, NULL));
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_counter_free(unitData, proc, set));

    /* make sure counter set is cleared after freeing */
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_counter_clear(unitData, proc, set));
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_counter_unit_update(unit, unitData, COUNTER_UPDATE_SELECT_ALL, NULL));

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_counter_bg_enable_set(int unit, int enable)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(COUNTERS_ACCESS.background_disable.set(unit, !enable));
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_dpp_counter_bg_enable_get(int unit,
                              int *enable)
{
    int neg_enable;
    BCMDNX_INIT_FUNC_DEFS;
    if (!enable) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("obligatory out arg must not be NULL")));
    }
    BCMDNX_IF_ERR_EXIT(COUNTERS_ACCESS.background_disable.get(unit, &neg_enable));
    *enable = !neg_enable;
exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_dpp_modify_proc_and_index(_bcm_dpp_counter_state_t * unitData, unsigned int *proc, unsigned int * finalIndex, uint8 subCount)
{
    unsigned int groupSize;
    int size = (subCount == TRUE) ? (_SUB_COUNT * unitData->proc[*proc].mode.num_counters) : (unitData->proc[*proc].mode.num_counters);
    int unit = unitData->cunit;
    int result;
    int bit_count;
    BCMDNX_INIT_FUNC_DEFS;

    result = _bcm_dpp_counters_per_set(unitData, *proc, &groupSize);
    BCMDNX_IF_ERR_EXIT(result);

    /*return if groupSize is power of 2 or not - if  =0 - groupSize is power of 2 */
    bit_count = groupSize & (groupSize - 1);
    /* modification of the ptr and proc should be made only if the source is ING_PP/EPNI/EGR_PP/EGQ and the group size=3/5/6/7 and the we_val !=0 */
    if ((unitData->proc[*proc].mode.src_type != SOC_TMC_CNT_SRC_TYPE_ING_PP && unitData->proc[*proc].mode.src_type != SOC_TMC_CNT_SRC_TYPE_EPNI
         && unitData->proc[*proc].mode.src_type != SOC_TMC_CNT_SRC_TYPE_EGR_PP) || (bit_count == 0))
    {
        SOC_EXIT;
    }

    /* if the index didn't cross the engine limit, therefore, no need to do anything */
    if (*finalIndex < size)
    {
        SOC_EXIT;
    }
    else
    {
        /* verify that that the next engine has the same configuration as the current one */
        if( ((*proc+1) < (SOC_DPP_DEFS_GET(unit, nof_counter_processors))) &&
            ((unitData->proc[*proc+1].valid & BCM_DPP_COUNTER_PROC_INFO_VALID) == TRUE) &&
            ((unitData->proc[*proc].mode.we_val + 1) == unitData->proc[*proc+1].mode.we_val) &&
            (unitData->proc[*proc].mode.command_id == unitData->proc[*proc+1].mode.command_id) &&
            (unitData->proc[*proc].mode.src_type == unitData->proc[*proc+1].mode.src_type) &&
            (unitData->proc[*proc].mode.mode_eg.type == unitData->proc[*proc+1].mode.mode_eg.type))
        {
            *finalIndex = *finalIndex % size;
            *proc = *proc + 1;
        }
        else
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("trying to modify the proc from proc_%d to proc_%d, but the is no matching engine in proc_%d"),
                    *proc, *proc+1, *proc+1));
        }
    }


exit:
    BCMDNX_FUNC_RETURN;
}




/***********************************************************************************************************************
 * Function:
 *      bcm_dpp_counter_stat_id_create
 * Purpose:
 *     create stat_id according to the command id, core, src_type and counter id.
 * Parameters:
 * int unit,
 * bcm_gport_t counter_source_gport,
 * uint32 counter_source_id,
 * bcm_stat_counter_source_type_t counter_source_type, 
 * int command_id, 
 * bcm_core_t core_id
 * int * stat_id
 * Returns:
 *      SOC_E_XXX
 * Notes:
 */
int bcm_dpp_counter_stat_id_create(
    int unit,
    bcm_gport_t counter_source_gport,
    uint32 counter_source_id, 
    bcm_stat_counter_source_type_t counter_source_type, 
    int command_id,
    bcm_core_t core_id,
    int * stat_id
    )
{
    _bcm_dpp_counter_state_t     *unitData;
    int proc, cntr, start_engine_counter, end_engine_counter;
    SOC_TMC_CNT_SRC_TYPE src_type;
    SOC_TMC_CNT_MODE_EG_TYPE eg_type;
    SOC_SAND_U32_RANGE range;
    uint32 offset;
    int double_entry;
    bcm_gport_t gport;
    BCMDNX_INIT_FUNC_DEFS;

    _COUNTER_UNIT_CHECK(unit, unitData);

    switch(counter_source_type)
    {
        case bcmStatCounterSourceIngressInlif:
            src_type = SOC_TMC_CNT_SRC_TYPE_ING_PP;
            eg_type = SOC_TMC_CNT_MODE_EG_TYPE_OUTLIF;
            /* allowing the GPORT In-Lif value to be placed in counter_source_id or counter_source_gport. 
               Altghough it is make sense that counter_source_gport will hold the gport. we allow it due to backword compatability requrements */
            if(_SHR_GPORT_IS_SET(counter_source_id) == FALSE && _SHR_GPORT_IS_SET(counter_source_gport) == FALSE)
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("counter_source_id=%d or counter_source_gport=%d, must be GPORT for bcmStatCounterSourceIngressInlif\n"),
                                counter_source_id, counter_source_gport));
            }
            else if(_SHR_GPORT_IS_SET(counter_source_id) == TRUE)
            {
                gport = (bcm_gport_t)counter_source_id;
            }
            else
            {
                gport = counter_source_gport;
            }
            BCMDNX_IF_ERR_EXIT(bcm_petra_port_encap_local_get(unit, gport, BCM_PORT_ENCAP_LOCAL_INGRESS, &cntr));
            break;            
        case bcmStatCounterSourceEgressTransmitOutlif: 
            src_type = SOC_TMC_CNT_SRC_TYPE_EPNI;
            eg_type = SOC_TMC_CNT_MODE_EG_TYPE_OUTLIF;
            /* counter_source_gport - used for Lifs*/
            if(counter_source_gport != -1) {
                if(_SHR_GPORT_IS_SET(counter_source_gport) == FALSE)
                {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("counter_source_gport=%d, must be GPORT for bcmStatCounterSourceEgressTransmitOutlif\n"),counter_source_gport));   
                }           
                BCMDNX_IF_ERR_EXIT(bcm_petra_port_encap_local_get(unit, counter_source_gport, BCM_PORT_ENCAP_LOCAL_EGRESS, &cntr));
            }
            /* counter_source_id - used for Out Rif*/
            else {
                cntr = counter_source_id;
            }
            if(SOC_IS_JERICHO_PLUS(unit))
            {
                range.start = cntr;
                range.end = cntr;
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL
                    (unit, mbcm_dpp_cnt_out_lif_counting_range_get, (unit, &range, &offset, NULL, NULL, &double_entry)));
                if(SOC_IS_QAX(unit))
                {
                    cntr = ((cntr>>double_entry) + offset) % (1<<SOC_TMC_CNT_QAX_OUT_LIF_CNT_PTR_SIZE);                
                }
                else
                {
                    cntr = ((cntr>>double_entry) + offset);
                }
            }
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("stat id create is not supported for src_type=%d\n"),counter_source_type));            
            break;
    }

    /* create stat id */
    /* find the engine that its configuration match to the desired counter */
    /* check if the counter is in the engine reange and create stat_id */
    for (proc = 0; proc < unitData->num_counter_procs; proc++) 
    {    
        BCM_DPP_COUNTER_SOURCE_MATCH(unitData, proc, core_id, src_type, command_id, eg_type)                
        {
            start_engine_counter = (unitData->proc[proc].mode.we_val*
                                    SOC_TMC_NOF_COUNTERS_IN_ENGINE(unit, unitData->proc[proc].proc_id))*
                                    ((unitData->proc[proc].mode.format == SOC_TMC_CNT_FORMAT_PKTS_AND_PKTS) ? 2 : 1);
            end_engine_counter = ((unitData->proc[proc].mode.we_val + 1)*
                                    SOC_TMC_NOF_COUNTERS_IN_ENGINE(unit, unitData->proc[proc].proc_id) - 1)*
                                    ((unitData->proc[proc].mode.format == SOC_TMC_CNT_FORMAT_PKTS_AND_PKTS) ? 2 : 1);
                                    
            if(cntr < unitData->proc[proc].mode.mode_eg.base_val)
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("cntr to read is %d, smaller than the base val (%d) in engine number %d.\n"), 
                cntr, unitData->proc[proc].mode.mode_eg.base_val, proc));                        
            }
            /* need to consider the base val when searching for the valid engine */            
            if(((cntr-unitData->proc[proc].mode.mode_eg.base_val)*unitData->proc[proc].mode.custom_mode_params.set_size >= start_engine_counter) &&
               ((cntr-unitData->proc[proc].mode.mode_eg.base_val)*unitData->proc[proc].mode.custom_mode_params.set_size <= end_engine_counter))
            {
               BCM_FIELD_STAT_ID_SET(*stat_id, proc, cntr);
               break;
            }                                    
        }
    }

    if(*stat_id == -1)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("stat id failed to create. could not find match engine to the input counter\n")));            
    }
exit:
    BCMDNX_FUNC_RETURN;

}


#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
int
_bcm_dpp_counters_sw_dump(int unit)
{
    uint32                        proc;
    uint32                        cntr;
    uint32                        i;
    uint32                        sLimit;
    _bcm_dpp_counter_state_t     *unitData;
    _bcm_dpp_counter_proc_info_t *info;
    uint8                        inUse = 0;
    uint8                        is_inUse_alloc = 0;
    unsigned int                 allocated = 0;
    int fifo = 0, cache_length = 0;
    uint64 counter;
    BCMDNX_INIT_FUNC_DEFS;

    /* Make sure the counters module is initialized */
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_counter_unit_check_and_fill(unit, &unitData, COUNTER_UPDATE_SELECT_ALL, NULL));

    LOG_CLI((BSL_META_U(unit,
                        "\nCOUNTERS:")));
    LOG_CLI((BSL_META_U(unit,
                        "\n---------")));

    LOG_CLI((BSL_META_U(unit,
                        "\nbackground_disable: %d\n"), unitData->background_disable));

    LOG_CLI((BSL_META_U(unit,
                        "\nfifo_read_background: %u\n"), unitData->diag.fifo_read_background));

    LOG_CLI((BSL_META_U(unit,
                        "\nfifo_read_deferred: %d\n"), unitData->diag.fifo_read_deferred));

    for (proc = 0; proc < unitData->num_counter_procs; proc++) {

        LOG_CLI((BSL_META_U(unit,
                            "\n\nproc: %d\n"), proc));

        info = &(unitData->proc[proc]);
        SOCDNX_SAND_IF_ERR_RETURN(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_cnt_engine_to_fifo_dma_index, (unit, proc, &fifo, &cache_length)));

        LOG_CLI((BSL_META_U(unit, "fifo_read_passes:   %u\n"), unitData->fifo[fifo].fifo_read_passes));
        LOG_CLI((BSL_META_U(unit, "fifo_read_fails:    %u\n"), unitData->fifo[fifo].fifo_read_fails));
        LOG_CLI((BSL_META_U(unit, "fifo_read_items:    %u\n"), unitData->fifo[fifo].fifo_read_items));
        LOG_CLI((BSL_META_U(unit, "fifo_read_max:      %u\n"), unitData->fifo[fifo].fifo_read_max));
        LOG_CLI((BSL_META_U(unit, "fifo_read_last:     %u\n"), unitData->fifo[fifo].fifo_read_last));
        LOG_CLI((BSL_META_U(unit, "direct_read_passes: %u\n"), info->diag.direct_read_passes));
        LOG_CLI((BSL_META_U(unit, "direct_read_fails:  %u\n"), info->diag.direct_read_fails));
        LOG_CLI((BSL_META_U(unit, "cache_updates:      %u\n"), info->diag.cache_updates));
        LOG_CLI((BSL_META_U(unit, "cache_reads:        %u\n"), info->diag.cache_reads));
        LOG_CLI((BSL_META_U(unit, "cache_writes:       %u\n"), info->diag.cache_writes));
        LOG_CLI((BSL_META_U(unit, "api_reads:          %u\n"), info->diag.api_reads));
        LOG_CLI((BSL_META_U(unit, "api_writes:         %u\n"), info->diag.api_writes));
        LOG_CLI((BSL_META_U(unit, "api_miss_reads:     %u\n"), info->diag.api_miss_reads));
        LOG_CLI((BSL_META_U(unit, "api_miss_writes:    %u\n"), info->diag.api_miss_writes));

        BCMDNX_IF_ERR_EXIT(COUNTERS_PROC_ACCESS.field.allocated.get(unit, proc, &allocated));
        LOG_CLI((BSL_META_U(unit, "allocated:          %u\n"), allocated));
        for (cntr = 0; cntr < unitData->proc[proc].mode.num_counters; cntr++) {
            for (i = 0; i < _SUB_COUNT; i++) {
                BCMDNX_IF_ERR_EXIT(COUNTERS_ACCESS.counter.get(unit, proc, (cntr * _SUB_COUNT) + i, &counter));
                if(counter != 0) {
                    LOG_CLI((BSL_META_U(unit, "counter (%05d, %d):   0x%x%x\n"), cntr, i, COMPILER_64_HI(counter), COMPILER_64_LO(counter)));
                }
            }
        }
        BCMDNX_IF_ERR_EXIT(COUNTERS_PROC_ACCESS.field.inUse.is_allocated(unit, proc, &is_inUse_alloc));
        if(is_inUse_alloc) {
            sLimit = (unitData->proc[proc].mode.num_sets + 7) >> 3;
            for (i = 0; i < sLimit; i++) {
                BCMDNX_IF_ERR_EXIT(COUNTERS_PROC_ACCESS.field.inUse.get(unit, proc, i, &inUse));
                if(0 != inUse) {
                    LOG_CLI((BSL_META_U(unit, "inUse (%05d):   0x%02x\n"), i, inUse));
                }
            }
        }
    }
    LOG_CLI((BSL_META_U(unit, "\n")));
exit:
    BCMDNX_FUNC_RETURN;
}

#endif /* def BCM_WARM_BOOT_SUPPORT_SW_DUMP */
