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

#ifndef _BCM_INT_DPP_COUNTERS_H_
#define _BCM_INT_DPP_COUNTERS_H_

/**************************************************************
 * **********************INCLUDES******************************
 */

#include <shared/swstate/sw_state.h>
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/TMC/tmc_api_cnt.h>
#include <soc/types.h>
#include <sal/core/thread.h>
#include <sal/core/sync.h>
#include <bcm/stat.h>


/**************************************************************
 * ******************* DEFINES, ENUM, MACROS*******************
 */

/*
 *  Enum for bits in bitmap listing provided stats in current mode, and for the
 *  individual counters within a list of counters to manupulate.
 *
 *  Note that some of these items would be synthetic in various modes, and many
 *  of them will not be supported in all modes.
 *
 *  WARNING: When adding entries here, add both pkts and bytes versions, in
 *  the order as demonstrated.  In several places, it is assumed that one can
 *  add one to a pkts value here to get the same bytes value, or subtract one
 *  from a bytes value to get a pkts value, in this enum.
 *
 *  Some of these values may not be supported.
 */

typedef enum bcm_dpp_counter_e {
    /* forwarded and dropped frames and bytes */
    bcm_dpp_counter_pkts,
    bcm_dpp_counter_bytes,
    bcm_dpp_counter_green_pkts,
    bcm_dpp_counter_green_bytes,
    bcm_dpp_counter_not_green_pkts,
    bcm_dpp_counter_not_green_bytes,
    bcm_dpp_counter_yellow_pkts,
    bcm_dpp_counter_yellow_bytes,
    bcm_dpp_counter_not_yellow_pkts,
    bcm_dpp_counter_not_yellow_bytes,
    bcm_dpp_counter_red_pkts,
    bcm_dpp_counter_red_bytes,
    bcm_dpp_counter_not_red_pkts,
    bcm_dpp_counter_not_red_bytes,
    /* forwarded frames and bytes */
    bcm_dpp_counter_fwd_pkts,
    bcm_dpp_counter_fwd_bytes,
    bcm_dpp_counter_fwd_green_pkts,
    bcm_dpp_counter_fwd_green_bytes,
    bcm_dpp_counter_fwd_not_green_pkts,
    bcm_dpp_counter_fwd_not_green_bytes,
    bcm_dpp_counter_fwd_yellow_pkts,
    bcm_dpp_counter_fwd_yellow_bytes,
    bcm_dpp_counter_fwd_not_yellow_pkts,
    bcm_dpp_counter_fwd_not_yellow_bytes,
    bcm_dpp_counter_fwd_red_pkts,
    bcm_dpp_counter_fwd_red_bytes,
    bcm_dpp_counter_fwd_not_red_pkts,
    bcm_dpp_counter_fwd_not_red_bytes,
    /* discarded frames and bytes */
    bcm_dpp_counter_drop_pkts,
    bcm_dpp_counter_drop_bytes,
    bcm_dpp_counter_drop_green_pkts,
    bcm_dpp_counter_drop_green_bytes,
    bcm_dpp_counter_drop_not_green_pkts,
    bcm_dpp_counter_drop_not_green_bytes,
    bcm_dpp_counter_drop_yellow_pkts,
    bcm_dpp_counter_drop_yellow_bytes,
    bcm_dpp_counter_drop_not_yellow_pkts,
    bcm_dpp_counter_drop_not_yellow_bytes,
    bcm_dpp_counter_drop_red_pkts,
    bcm_dpp_counter_drop_red_bytes,
    bcm_dpp_counter_drop_not_red_pkts,
    bcm_dpp_counter_drop_not_red_bytes,
	/* entry by index */
    bcm_dpp_counter_offset0_pkts,
	bcm_dpp_counter_offset0_bytes,
    bcm_dpp_counter_offset1_pkts,
	bcm_dpp_counter_offset1_bytes,
	bcm_dpp_counter_offset2_pkts,
	bcm_dpp_counter_offset2_bytes,
	bcm_dpp_counter_offset3_pkts,
	bcm_dpp_counter_offset3_bytes,
	bcm_dpp_counter_offset4_pkts,
	bcm_dpp_counter_offset4_bytes,
	bcm_dpp_counter_offset5_pkts,
	bcm_dpp_counter_offset5_bytes,
	bcm_dpp_counter_offset6_pkts,
	bcm_dpp_counter_offset6_bytes,
	bcm_dpp_counter_offset7_pkts,
	bcm_dpp_counter_offset7_bytes,
	/* limit */
    bcm_dpp_counter_count /* NOT A VALID ENTRY; MUST BE LAST */
} bcm_dpp_counter_t;

/* entry bmap- bits mapping*/
#define GREEN_FWD   (1 << SOC_TMC_CNT_BMAP_OFFSET_GREEN_FWD)
#define GREEN_DROP  (1 << SOC_TMC_CNT_BMAP_OFFSET_GREEN_DROP)
#define YELLOW_FWD  (1 << SOC_TMC_CNT_BMAP_OFFSET_YELLOW_FWD)
#define YELLOW_DROP (1 << SOC_TMC_CNT_BMAP_OFFSET_YELLOW_DROP)
#define RED_FWD	    (1 << SOC_TMC_CNT_BMAP_OFFSET_RED_FWD)
#define RED_DROP    (1 << SOC_TMC_CNT_BMAP_OFFSET_RED_DROP)
#define BLACK_FWD   (1 << SOC_TMC_CNT_BMAP_OFFSET_BLACK_FWD)
#define BLACK_DROP  (1 << SOC_TMC_CNT_BMAP_OFFSET_BLACK_DROP)

/*
 *  This is used when querying what stats are available.
 */
typedef SHR_BITDCL bcm_dpp_counter_set_t[_SHR_BITDCLSIZE(bcm_dpp_counter_count)];

/*
 *  Specifies the underlying counter source
 *
 *  This is here only to make debugging easier; it is directly mapped from the
 *  value of the counter_source SOC property.
 */

typedef SOC_TMC_CNT_SRC_TYPE _bcm_dpp_counter_source_t;

/*
 *  Diagnostic information tags
 */
typedef enum bcm_dpp_counter_diag_info_e {
    /* global */
    bcm_dpp_counter_diag_info_num_proc,
    bcm_dpp_counter_diag_info_bg_active,
    bcm_dpp_counter_diag_info_bg_suspend,
    bcm_dpp_counter_diag_info_bg_defer,
    bcm_dpp_counter_diag_info_fg_hit,
    bcm_dpp_counter_diag_info_fifo_read_background,
    bcm_dpp_counter_diag_info_fifo_read_deferred,
    /* per counter processor */
    bcm_dpp_counter_diag_info_fifo_read_passes,
    bcm_dpp_counter_diag_info_fifo_read_fails,
    bcm_dpp_counter_diag_info_fifo_read_items,
    bcm_dpp_counter_diag_info_fifo_read_max,
    bcm_dpp_counter_diag_info_fifo_read_last,
    bcm_dpp_counter_diag_info_direct_read_passes,
    bcm_dpp_counter_diag_info_direct_read_fails,
    bcm_dpp_counter_diag_info_cache_updates,
    bcm_dpp_counter_diag_info_cache_reads,
    bcm_dpp_counter_diag_info_cache_writes,
    bcm_dpp_counter_diag_info_api_reads,
    bcm_dpp_counter_diag_info_api_writes,
    bcm_dpp_counter_diag_info_api_miss_reads,
    bcm_dpp_counter_diag_info_api_miss_writes,
    bcm_dpp_counter_diag_info_source,
    bcm_dpp_counter_diag_info_msb_patten,
    bcm_dpp_counter_diag_info_command_id,
    bcm_dpp_counter_diag_info_stat_tag_low_bit,
    bcm_dpp_counter_diag_info_voq_base,
    bcm_dpp_counter_diag_info_voq_per_set,
    bcm_dpp_counter_diag_info_format,
    bcm_dpp_counter_diag_info_counters,
    bcm_dpp_counter_diag_info_sets,
    bcm_dpp_counter_diag_info_alloc_local,
    bcm_dpp_counter_diag_info_alloc_inuse,
    bcm_dpp_counter_diag_info_mode,
    bcm_dpp_counter_diag_info_range_start,
    bcm_dpp_counter_diag_info_range_end,
    /* limit */
    bcm_dpp_counter_diag_info_count /* NOT A VALID ENTRY; MUST BE LAST */
} bcm_dpp_counter_diag_info_t;

/*
 *  Counter format
 */
typedef enum bcm_dpp_counter_format_e {
    bcm_dpp_counter_format_packets,
    bcm_dpp_counter_format_bytes,
    bcm_dpp_counter_format_packets_and_bytes,
    bcm_dpp_counter_format_max_q_size
} bcm_dpp_counter_format_t;

/*
 *  Counter format
 */
typedef enum bcm_dpp_counter_replicated_pkts_e {
    bcm_dpp_counter_replicated_pkts_all,
    bcm_dpp_counter_replicated_pkts_frwrd_and_mc,
    bcm_dpp_counter_replicated_pkts_frwrd
} bcm_dpp_counter_replicated_pkts_t;
/*
 *  Flags for allocation of counters
 *
 *  WITH_ID indicates both processor and set ID are specified
 *  WITH_PROC indicates processor ID is specified
 */
#define BCM_DPP_COUNTER_WITH_ID   0x00000001
#define BCM_DPP_COUNTER_WITH_PROC 0x00000002


/*
 *  Subcounter index -- each counter is a group, consisting of sub counters.
 */
#define _SUB_PKTS 0  /* sub counter for frames */
#define _SUB_BYTES 1 /* sub counter for bytes */
#define _SUB_COUNT 2 /* number of sub counters */

/* packets and packets mode Subcounter indexes*/
#define _SUB_FWD_PKTS   _SUB_BYTES /* sub counter for forward packets */
#define _SUB_DROP_PKTS  _SUB_PKTS  /* sub counter for droped packets */


#define UNSUPPORTED_COUNTER_OFFSET (256)

#define BCM_DPP_COUNTER_PROC_INFO_VALID 0x1
#define BCM_DPP_COUNTER_PROC_INFO_CONFIGURED_DYNAMICALLY 0x2

/* set of defines to select which section of SW DB to fill/update*/
#define COUNTER_UPDATE_SELECT_RESERVED                  0x1
#define COUNTER_UPDATE_SELECT_DIAGS                     0x2
#define COUNTER_UPDATE_SELECT_PROC_INFO_ALL             0x4
#define COUNTER_UPDATE_SELECT_PROC_INFO_SPECIFIC_DATA   0x8
#define COUNTER_UPDATE_SELECT_BACKROUND_INFO            0x10
#define COUNTER_UPDATE_SELECT_LIF_INFO                  0x20
#define COUNTER_UPDATE_SELECT_FIFO_INFO                 0x40
#define COUNTER_UPDATE_SELECT_ALL (COUNTER_UPDATE_SELECT_RESERVED      | \
                                   COUNTER_UPDATE_SELECT_DIAGS         | \
                                   COUNTER_UPDATE_SELECT_PROC_INFO_ALL | \
                                   COUNTER_UPDATE_SELECT_BACKROUND_INFO| \
                                   COUNTER_UPDATE_SELECT_LIF_INFO      | \
                                   COUNTER_UPDATE_SELECT_FIFO_INFO)

#define COUNTER_OUT_LIF_SOURCE_INDEX (0)
#define COUNTER_IN_LIF_SOURCE_INDEX (1)


/**************************************************************
 * *****************STRUCTURES*********************************
 */

/* per proc statistics and diagnostics */
typedef struct _bcm_dpp_counter_proc_diag_s {
    unsigned int direct_read_passes;        /* direct read successes */
    unsigned int direct_read_fails;         /* direct read errors */
    unsigned int cache_updates;             /* cache updates (by bg/direct) */
    unsigned int cache_reads;               /* cache reads */
    unsigned int cache_writes;              /* cache writes */
    unsigned int api_reads;                 /* API requested reads */
    unsigned int api_writes;                /* API requested writes */
    unsigned int api_miss_reads;            /* API req reads to invalid ctr */
    unsigned int api_miss_writes;           /* SPI req writes to invalid ctr */
} _bcm_dpp_counter_proc_diag_t;

typedef struct _bcm_dpp_counter_proc_field_info_s {
    unsigned int allocated;                     /* number of allocated sets */
    PARSER_HINT_ARR uint8 *inUse;               /* bits indicating in use sets */
} _bcm_dpp_counter_proc_field_info_t;

/*
 *  Per counter processor information
 *
 *  The term 'counter pair' here refers to the bytes+frames counter pair per
 *  element in the hardware, not some larger scale pairing, such as the ingress
 *  colour disabled mode; these larger groupings are covered by the format.
 *  The term 'counter set' describes such larger grouping.
 *
 *  Certain sources (such as VOQ or VSI) have implied assignments of the
 *  counter sets; those sources will not allocate any in use bits.  Sources
 *  that do not imply counter set assignments (such as PP or STAG) will
 *  allocate bits for tracking which sets are in use.
 */
typedef struct _bcm_dpp_counter_proc_info_s {
    uint8 valid;                                 /* bit1 - valid,  bit 2 - if configure via API */
    SOC_TMC_CNT_PROCESSOR_ID proc_id;            /* this counter processor ID */
    SOC_TMC_CNT_COUNTERS_INFO mode;              /* this counter processor mode */
    /* supported counters - native */
    SHR_BITDCL native[_SHR_BITDCLSIZE(bcm_dpp_counter_count)];
    /* each entry can be unsupported val - unsupported counter/ offset - for native counter/ offsets bmap - for simulated counter*/
    uint32 avail_offsets[bcm_dpp_counter_count];   
    PARSER_HINT_ALLOW_WB_ACCESS _bcm_dpp_counter_proc_diag_t diag;  /* per proc statistics and diagnostics */
    _bcm_dpp_counter_proc_field_info_t field;                       /* field allocation - used for PMF */
} _bcm_dpp_counter_proc_info_t;

/* per fifo statistics and diagnostics */
typedef struct _bcm_dpp_counter_fifo_info_s {
    unsigned int fifo_read_passes;          /* FIFO read successes */
    unsigned int fifo_read_fails;           /* FIFO read errors */
    unsigned int fifo_read_items;           /* total counters in FIFO reads */
    unsigned int fifo_read_max;             /* largest single FIFO read */
    unsigned int fifo_read_last;            /* last counter read by FIFO */
    uint8 ref_count;                        /* How many engines are enabled and need DMA-Channel (Narro-format)*/
} _bcm_dpp_counter_fifo_info_t;

typedef struct _bcm_dpp_counter_global_info_s {
    uint8 haveStatTag;
    /*
     *  cache_only controls whether the stat code uses the cached or direct version of the functions.
     *  It is controlled by the field control bcmFieldControlStatSyncEnable.
     *  When stat sync enabled (this flag is FALSE, the default condition),
     *  the field APIs will use uncached access to the counter processors,
     *  enabling reads and writes to apply immediately.
     *  When stat sync disabled (this flag is TRUE), the field APIs will only use
     *  the cached access to the counter processors, which means reads apply to the
     *  time of last ejection for a counter, and writes also apply to the time of
     *  last ejection for a counter.
     */
    uint8 cache_only;
} _bcm_dpp_counter_global_info_t;

    /* statistics and diagnostics */
typedef struct _bcm_dpp_counter_diag_s {
    /* statistics and diagnostics */
    unsigned int fifo_read_background;      /* background FIFO attempts */
    unsigned int fifo_read_deferred;        /* deferred FIFO attempts */
} _bcm_dpp_counter_diag_t;


typedef struct _bcm_dpp_counter_lif_ranges_s {
    int counting_profile;
    int lif_stack_priority[BCM_STAT_COUNT_LIF_NUMBER_OF_STACK_IDS];
} _bcm_dpp_counter_lif_ranges_t;

/* default values of the QP, TC, Cast masks and shifts*/
#define QUEUE_PAIR_MASK         255
#define TRAFFIC_CLASS_MASK      7
#define CAST_MASK               1
#define QUEUE_PAIR_SHIFT        4
#define TRAFFIC_CLASS_SHIFT     1
#define CAST_SHIFT              0

typedef struct _bcm_dpp_egress_receive_tm_pointer_format_s {
    uint32 queue_pair_mask;     /* Which bits to consider from the queue pair
                                   field when building the counter pointer. */
    uint32 traffic_class_mask;  /* Which bits to consider from the traffic class
                                   field when building the counter pointer. */
    uint32 cast_mask;           /* Consider (or not) the cast bit when building
                                   the counter pointer. */
    uint32 queue_pair_shift;
    uint32 traffic_class_shift;
    uint32 cast_shift;
} _bcm_dpp_egress_receive_tm_pointer_format_t;

/*
 *  Per unit information
 */
typedef struct _bcm_dpp_counter_state_s {
    /* per unit information */
    int cunit;                               /* unit number */
    int sampling_interval_configured;        /* is BG thread and DMA configured to be active by SOC or API*/
    unsigned int num_counter_procs;         /* number of counter processors */
    /* interlocking */
    PARSER_HINT_ALLOW_WB_ACCESS int background_defer;                   /* defer background updates */
    PARSER_HINT_ALLOW_WB_ACCESS int background_active;                  /* background update occurring */
    PARSER_HINT_ALLOW_WB_ACCESS int background_disable;                 /* background updates disabled */
    PARSER_HINT_ALLOW_WB_ACCESS int foreground_hit;                     /* a foreground access occurred */
    PARSER_HINT_ALLOW_WB_ACCESS _bcm_dpp_counter_diag_t diag; 
    /* per processor information */
    PARSER_HINT_ALLOW_WB_ACCESS PARSER_HINT_ARR _bcm_dpp_counter_proc_info_t *proc; /* per processor information */
    PARSER_HINT_ALLOW_WB_ACCESS PARSER_HINT_ARR_ARR uint64 **counter;    /* cache of this proc's counters */
    PARSER_HINT_ALLOW_WB_ACCESS _bcm_dpp_counter_fifo_info_t fifo[SOC_DPP_DEFS_MAX(NOF_COUNTER_FIFOS)];
    _bcm_dpp_counter_global_info_t global_info;
    _bcm_dpp_counter_lif_ranges_t lif_ranges[SOC_TMC_CNT_LIF_COUNTING_NOF_SOURCES][SOC_TMC_CNT_LIF_COUNTING_MAX_NOF_RANGES_PER_SOURCE]; 
    _bcm_dpp_egress_receive_tm_pointer_format_t egress_receive_tm_pointer_format;
} _bcm_dpp_counter_state_t;


typedef struct bcm_dpp_counter_bg_thread_and_dma_data {
    int running;                            /* unit background thread enable */
    int bgWait;                             /* delay per bg iter (microsec) */   
    uint8 dma_attached;
    uint8 dma_channel[SOC_DPP_DEFS_MAX(NOF_COUNTER_FIFOS)]; /* DMA-Channel attached to each FIFO*/
} bcm_dpp_counter_bg_thread_and_dma_data_t;


typedef struct bcm_dpp_counter_proc_and_set_s {
    int ctr_proc_id;
    int ctr_set_id;
} bcm_dpp_counter_proc_and_set_t;


typedef struct bcm_dpp_counter_proc_specific_data_s
{
    const bcm_dpp_counter_t *type;
    int                     typeSize;
    unsigned int            procArr[SOC_DPP_DEFS_MAX(NOF_COUNTER_PROCESSORS) + SOC_DPP_DEFS_MAX(NOF_SMALL_COUNTER_PROCESSORS)];
    int                     procArrSize;
}bcm_dpp_counter_proc_specific_data_s;

/**************************************************************
 * *****************GLOBALS************************************
 */

extern const char * const bcm_dpp_counter_t_names[];
extern _bcm_dpp_counter_state_t *_bcm_dpp_counter_state[SOC_MAX_NUM_DEVICES];
extern int _bcm_counter_thread_is_running[SOC_MAX_NUM_DEVICES];



/**************************************************************
 * *****************FUNCTIONS**********************************
 */

/*
 *   Name
 *     bcm_dpp_counter_init
 *   Purpose
 *     Initialise the counter processor handling
 *   Arguments
 *     IN int unit = the unit number on which to operate
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 *   Notes
 */
extern int
bcm_dpp_counter_init(int unit);


/*
 *  Name
 *    bcm_dpp_counter_reserve_dma_channel
 *  Purpose
 *    Make a DMA channel un-allocatable. 
 *  Arguments
 *    int unit = unit number.
 *    uint8 channel - reserved channel.
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 *   Notes
 *     The only DMAs that allowed also by CRPS will be active
 * 
 *     Unit must be initialized.
 */
int bcm_dpp_counter_reserve_dma_channel(
    int unit, 
    uint8 channel);

/*
 *   Name
 *     bcm_dpp_counter_background_collection_enable_set
 *   Purpose
 *     Stop the background thread and disable DMA FIFO
 *   Arguments
 *   Arguments
 *     IN int unit = the unit number on which to operate
 *     IN int enable = enable/disable counter thread and DMA operation
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 *   Notes
 */
extern int 
bcm_dpp_counter_background_collection_enable_set(int unit, int enable);

/*
 *   Name
 *     bcm_dpp_counter_background_collection_enable_get
 *   Purpose
 *     Get the background collection enable/disable mode
 *   Arguments
 *   Arguments
 *     IN int unit = the unit number on which to operate
 *     OUT int enable = counter thread status - enable/disable 
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 *   Notes:
 *   Not check DMA status
 */
extern int 
bcm_dpp_counter_background_collection_enable_get(int unit, int *enable);

/*
 *   Name
 *     bcm_dpp_counter_detach
 *   Purpose
 *     Deinitialise the counter processor handling
 *   Arguments
 *   Arguments
 *     IN int unit = the unit number on which to operate
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 *   Notes
 */
extern int
bcm_dpp_counter_detach(int unit);

/*
 *   Name
 *     bcm_dpp_counter_avail_get
 *   Purpose
 *     Get information about which stats are provided for a given counter set.
 *   Arguments
 *     IN int unit = the unit number on which to operate
 *     IN unsigned int proc = the counter processor on which to operate
 *     OUT bcm_dpp_counter_set_t *avail = where to put available set
 *     OUT bcm_dpp_counter_set_t *native = where to put native set
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 *   Notes
 *     The 'available' set includes all stats that are available in the current
 *     mode, both native and emulated.
 *
 *     The 'native' set includes all stats that are supported natively by the
 *     current hardware mode.
 *
 *     Here 'native' refers to stats that are directly supported by the current
 *     hardware mode, while 'emulated' refers to stats that can be derived by
 *     combining 'native' stats in some way.  For example, if the current
 *     hardware mode provides {drop green, drop yellow, drop red}, it is
 *     possible to emulate {drop all, drop not green, drop not yellow, drop not
 *     red} from the native information.
 *
 *     In error case, both avail and native will be cleared.
 *
 *     Current and next generation (Soc_petra and Arad) do not support multiple
 *     modes per counter processor, so this only takes counter processor as the
 *     argument to determine the mode.  May change in distant future.
 */
extern int
bcm_dpp_counter_avail_get(int unit,
                          unsigned int proc,
                          bcm_dpp_counter_set_t *avail,
                          bcm_dpp_counter_set_t *native);

/*
 *   Name
 *     bcm_dpp_counter_get(_cached)
 *   Purpose
 *     Retrieve a specific statistic from a counter set
 *   Arguments
 *     IN int unit = the unit number on which to operate
 *     IN unsigned int proc = the counter processor on which to operate
 *     IN unsigned int set_id = the ID of the counter set to read
 *     IN bcm_dpp_counter_t type = which stat to retrieve
 *     OUT uint64 *stat = where to put the retrieved stat
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE for success
 *       BCM_E_EMPTY if a requested stat is not available
 *       BCM_E_NOT_FOUND if requested set is not in use
 *       BCM_E_* otherwise as appropriate
 *   Notes
 *     It is far more efficient to use the multt_get if you want more than one.
 *
 *     May return success when getting stats that are not supported in the
 *     current mode; such stats will be returned as zero.
 *
 *     Some stats may be filled in by emulating them (arithmetic on native
 *     stats).
 *
 *     BCM API convention also specs BCM_E_NOT_FOUND if the set_id is invalid.
 *
 *     The _cached version of the call works exclusively with the cache and
 *     does not sync to the hardware.  It should be used in sensitive contexts
 *     (such as IRQ handler) or if speed is substantially more important than
 *     accuracy, but not in other contexts.  Note that in this mode, any data
 *     on the hardware will be ignored, leading to possibly large grained
 *     updates on reads and sets eventually including large possibly stale
 *     values from hardware. The effect will be significantly more pronounced
 *     on slower flows.  Also in this mode, it is possible that a background
 *     update will occur during the access, leading to inconsistent values.
 */
extern int
bcm_dpp_counter_get(int unit,
                    unsigned int proc,
                    unsigned int set_id,
                    bcm_dpp_counter_t type,
                    uint64 *stat);
extern int
bcm_dpp_counter_get_cached(int unit,
                           unsigned int proc,
                           unsigned int set_id,
                           bcm_dpp_counter_t type,
                           uint64 *stat);

/*
 *   Name
 *     bcm_dpp_counter_set(_cached)
 *   Purpose
 *     Updates a specific statistic of a counter set
 *   Arguments
 *     IN int unit = the unit number on which to operate
 *     IN unsigned int proc = the counter processor on which to operate
 *     IN unsigned int set_id = the ID of the counter set to write
 *     IN bcm_dpp_counter_t type = which stat to set
 *     IN uint64 stat = the new value for the statistic
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE for success
 *       BCM_E_EMPTY if a requested stat is not available
 *       BCM_E_NOT_FOUND if requested set is not in use
 *       BCM_E_* otherwise as appropriate
 *   Notes
 *     It is far more efficient to use the multt_set if you want more than one.
 *
 *     May return success even for stats that are not supported in the current
 *     mode; no changes will be made for such stats.
 *
 *     For stats that are emulated by arithmetic, the values of the individual
 *     'native' stats participating in an emulated stat are not guaranteed;
 *     only that the total will be as written.  It is therefore best to only
 *     set native stats or to only set stats to zero.
 *
 *     BCM API convention also specs BCM_E_NOT_FOUND if the set_id is invalid.
 *
 *     The _cached version of the call works exclusively with the cache and
 *     does not sync to the hardware.  It should be used in sensitive contexts
 *     (such as IRQ handler) or if speed is substantially more important than
 *     accuracy, but not in other contexts.  Note that in this mode, any data
 *     on the hardware will be ignored, leading to possibly large grained
 *     updates on reads and sets eventually including large possibly stale
 *     values from hardware. The effect will be significantly more pronounced
 *     on slower flows.  Also in this mode, it is possible that a background
 *     update will occur during the access, leading to inconsistent values.
 */
extern int
bcm_dpp_counter_set(int unit,
                    unsigned int proc,
                    unsigned int set_id,
                    bcm_dpp_counter_t type,
                    uint64 stat);
extern int
bcm_dpp_counter_set_cached(int unit,
                           unsigned int proc,
                           unsigned int set_id,
                           bcm_dpp_counter_t type,
                           uint64 stat);

/*
 *   Name
 *     bcm_dpp_counter_multi_get(_cached)
 *   Purpose
 *     Retrieve specific stats from a counter set
 *   Arguments
 *     IN int unit = the unit number on which to operate
 *     IN unsigned int proc = the counter processor on which to operate
 *     IN unsigned int set_id = the ID of the counter set to read
 *     IN unsigned int count = number of stats to retrieve
 *     IN bcm_dpp_counter_t *type = list of stats to retrieve
 *     OUT uint64 *stats = where to put the retrieved stats
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE for success
 *       BCM_E_EMPTY if a requested stat is not available
 *       BCM_E_NOT_FOUND if requested set is not in use
 *       BCM_E_* otherwise as appropriate
 *   Notes
 *     On success, some stats may not have been supported in the particular
 *     configuration.  These stats will be zeroed.
 *
 *     Some stats may be filled in by emulating them (arithmetic on known
 *     stats).
 *
 *     BCM API convention also specs BCM_E_NOT_FOUND if the set_id is invalid.
 *
 *     type and stat are pointers to the initial element of respective arrays.
 *
 *     The _cached version of the call works exclusively with the cache and
 *     does not sync to the hardware.  It should be used in sensitive contexts
 *     (such as IRQ handler) or if speed is substantially more important than
 *     accuracy, but not in other contexts.  Note that in this mode, any data
 *     on the hardware will be ignored, leading to possibly large grained
 *     updates on reads and sets eventually including large possibly stale
 *     values from hardware. The effect will be significantly more pronounced
 *     on slower flows.  Also in this mode, it is possible that a background
 *     update will occur during the access, leading to inconsistent values.
 */
extern int
bcm_dpp_counter_multi_get(int unit,
                          unsigned int proc,
                          unsigned int set_id,
                          unsigned int count,
                          const bcm_dpp_counter_t *type,
                          uint64 *stat);
extern int
bcm_dpp_counter_multi_get_cached(int unit,
                                 unsigned int proc,
                                 unsigned int set_id,
                                 unsigned int count,
                                 const bcm_dpp_counter_t *type,
                                 uint64 *stat);

/*
 *   Name
 *     bcm_dpp_counter_multi_set(_cached)
 *   Purpose
 *     Set specific stats from a counter set
 *   Arguments
 *     IN int unit = the unit number on which to operate
 *     IN unsigned int proc = the counter processor on which to operate
 *     IN unsigned int set_id = the ID of the counter set to write
 *     IN unsigned int count = number of stats to set
 *     IN bcm_dpp_counter_t *type = list of stats to set
 *     IN uint64 *stats = where to put the retrieved stats
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE for success
 *       BCM_E_EMPTY if a requested stat is not available
 *       BCM_E_NOT_FOUND if requested set is not in use
 *       BCM_E_* otherwise as appropriate
 *   Notes
 *     On success, stats that are not supported in the particular mode were
 *     ignored (no changes made).
 *
 *     For stats that are emulated by arithmetic, the values of the individual
 *     'native' stats participating in an emulated stat are not guaranteed;
 *     only that the total will be as written.  It is therefore best to only
 *     set native stats or to only set stats to zero.
 *
 *     BCM API convention also specs BCM_E_NOT_FOUND if the set_id is invalid.
 *
 *     type and stat are pointers to the initial element of respective arrays.
 *
 *     The _cached version of the call works exclusively with the cache and
 *     does not sync to the hardware.  It should be used in sensitive contexts
 *     (such as IRQ handler) or if speed is substantially more important than
 *     accuracy, but not in other contexts.  Note that in this mode, any data
 *     on the hardware will be ignored, leading to possibly large grained
 *     updates on reads and sets eventually including large possibly stale
 *     values from hardware. The effect will be significantly more pronounced
 *     on slower flows.  Also in this mode, it is possible that a background
 *     update will occur during the access, leading to inconsistent values.
 */
extern int
bcm_dpp_counter_multi_set(int unit,
                          unsigned int proc,
                          unsigned int set_id,
                          unsigned int count,
                          const bcm_dpp_counter_t *type,
                          /*const*/ uint64 *stat);
extern int
bcm_dpp_counter_multi_set_cached(int unit,
                                 unsigned int proc,
                                 unsigned int set_id,
                                 unsigned int count,
                                 const bcm_dpp_counter_t *type,
                                 /*const*/ uint64 *stat);

/*
 *   Name
 *     bcm_dpp_counter_diag_info_get
 *   Purpose
 *     DIAGNOSTICS: Retrieve information about the counter state on a unit
 *   Arguments
 *     IN int unit = the unit number on which to operate
 *     IN bcm_dpp_counter_diag_info_t info = which item to retrieve
 *     IN unsigned int proc = the counter processor on which to operate
 *     OUT unsigned int *value = where to put the retrieved value
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 *   Notes
 *     Some items apply globally to a unit; others only to a single counter
 *     processor.  For those that are for a particular counter processor, the
 *     processor number chooses the counter processor.  For all cases, the
 *     processor number must be valid, so it is best to use zero as the
 *     processor number for any of the global information.
 */
extern int
bcm_dpp_counter_diag_info_get(int unit,
                              bcm_dpp_counter_diag_info_t info,
                              unsigned int proc,
                              unsigned int *value);
/**/
extern int
bcm_dpp_counter_egress_receive_tm_pointer_update( int unit,
                                                  uint32 queue_pair_mask, 
                                                  uint32 traffic_class_mask, 
                                                  uint32 cast_mask, 
                                                  uint32 queue_pair_shift, 
                                                  uint32 traffic_class_shift, 
                                                  uint32 cast_shift);
/*
 *   Name
 *     bcm_dpp_counter_find_egress_que
 *   Purpose
 *     Find the counter processor and set that is used for egress.
 *   Arguments
 *     IN int unit = the unit number on which to operate
 *     IN unsigned int max_set_cnt - Size of result array
 *     IN unsigned int offset = Offset (TC)
 *     IN unsigned int tm_port
 *     IN unsigned int is_mc
 *     IN unsigned int pp_port
 *     OUT unsigned int *proc = the counter processor handlng the results
 *     OUT unsigned int *set_array = the counter set handling the results
 *     OUT unsigned int *set_cnt = the number of actual entries in set_array
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_NOT_FOUND if the VOQ is not handled by any counter processor
 *       BCM_E_* otherwise as appropriate
 *   Notes
 *     Unwanted OUT arguments may be NULL.
 */


extern int
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
                         unsigned int *set_cnt);
 
/*
 *   Name
 *     bcm_dpp_counter_find_voq
 *   Purpose
 *     Find the counter processor and set that is used for a paricular VOQ, and
 *     the range of VOQs associated with that processor and set.
 *   Arguments
 *     IN int unit = the unit number on which to operate
 *     IN unsigned int voq = the VOQ of interest
 *     OUT unsigned int *proc = the counter processor handlng the VOQ
 *     OUT unsigned int *set = the counter set handling the VOQ
 *     OUT unsigned int *first = the first VOQ handled by the set
 *     OUT unsigned int *last = the last VOQ handled by the set
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_NOT_FOUND if the VOQ is not handled by any counter processor
 *       BCM_E_* otherwise as appropriate
 *   Notes
 *     Unwanted OUT arguments may be NULL.
 */
extern int
bcm_dpp_counter_find_voq(int unit,
                         int core_id,
                         unsigned int voq,
                         bcm_dpp_counter_proc_and_set_t *proc_and_set,
                         bcm_dpp_counter_t ctrStat,
                         int* nof_found,
                         unsigned int *first,
                         unsigned int *last);

/*
 *   Name
 *     bcm_dpp_counter_find_stag
 *   Purpose
 *     Find the counter processor and set that is used for a paricular
 *     statistics tag, and the range of statistics tags associated with that
 *     processor and set.
 *   Arguments
 *     IN int unit = the unit number on which to operate
 *     IN unsigned int stag = the stat tag of interest
 *     OUT unsigned int *proc = the counter processor handlng the stat tag
 *     OUT unsigned int *set = the counter set handling the stat tag
 *     OUT unsigned int *first = the first stat tag handled by the set
 *     OUT unsigned int *last = the last stat tag handled by the set
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_NOT_FOUND if the stat tag is not handled by any counter processor
 *       BCM_E_* otherwise as appropriate
 *   Notes
 *     Unwanted OUT arguments may be NULL.
 */
extern int
bcm_dpp_counter_find_stag(int unit,
                          unsigned int stag,
                          unsigned int *proc,
                          unsigned int *set,
                          unsigned int *first,
                          unsigned int *last);

/*
 *   Name
 *     bcm_dpp_counter_find_vsi
 *   Purpose
 *     Find the counter processor and set that is used for a paricular VSI
 *   Arguments
 *     IN int unit = the unit number on which to operate
 *     IN unsigned int vsi = the VSI of interest
 *     OUT unsigned int *proc = the counter processor handlng the VSI
 *     OUT unsigned int *set = the counter set handling the VSI
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_NOT_FOUND if the VSI is not handled by any counter processor
 *       BCM_E_* otherwise as appropriate
 *   Notes
 *     Unwanted OUT arguments may be NULL.
 */
extern int
bcm_dpp_counter_find_vsi(int unit,
                         unsigned int vsi,
                         unsigned int *proc,
                         unsigned int *set);

/*
 *   Name
 *     bcm_dpp_counter_find_outlif
 *   Purpose
 *     Find the counter processor and set that is used for a paricular outlif
 *   Arguments
 *     IN int unit = the unit number on which to operate
 *     IN unsigned int voq = the outlif of interest
 *     OUT unsigned int *proc = the counter processor handlng the outlif
 *     OUT unsigned int *set = the counter set handling the outlif
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_NOT_FOUND if the outlif is not handled by any counter processor
 *       BCM_E_* otherwise as appropriate
 *   Notes
 *     Unwanted OUT arguments may be NULL.
 */
extern int
bcm_dpp_counter_find_outlif(int unit,
                            unsigned int outlif,
                            unsigned int *proc,
                            unsigned int *set);

/*
 *  Name
 *    bcm_dpp_counter_alloc
 *  Purpose
 *    Allocate a counter set from a processor where we are providing allocation
 *    from this module (rather than it being implied by the source type).
 *  Arguments
 *    IN int unit = the unit number on which to operate
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
extern int
bcm_dpp_counter_alloc(int unit,
                      uint32 flags,
                      _bcm_dpp_counter_source_t source,
                      unsigned int *proc,
                      unsigned int *set);

/*
 *  Name
 *    bcm_dpp_counter_free
 *  Purpose
 *    Free a counter set from a processor where we are providing allocation
 *    from this module (rather than it being implied by the source type).
 *  Arguments
 *    IN int unit = the unit number on which to operate
 *    IN unsigned int proc = the counter processor for the counter set
 *    IN unsigned int set_id = the ID of the counter set
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 *  Notes
 */
extern int
bcm_dpp_counter_free(int unit,
                     unsigned int proc,
                     unsigned int set);

/*
 *  Name
 *    bcm_dpp_counter_bg_enable_set
 *  Purpose
 *    Set background thread's access to the counter processor.
 *  Arguments
 *    IN int unit = the unit number on which to operate
 *    IN int enable = TRUE to allow background updates, FALSE to disallow
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 *  Notes
 *    Disabling background updates does not prevent demand accesses reading the
 *    counter hardware, either directly or through the ejection FIFO.
 */
extern int
bcm_dpp_counter_bg_enable_set(int unit,
                              int enable);

/*
 *  Name
 *    bcm_dpp_counter_bg_enable_get
 *  Purpose
 *    Set background thread's access to the counter processor.
 *  Arguments
 *    IN int unit = the unit number on which to operate
 *    OUT int *enable = TRUE to allow background updates, FALSE to disallow
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *       BCM_E_* otherwise as appropriate
 *  Notes
 *    Disabling background updates does not prevent demand accesses reading the
 *    counter hardware, either directly or through the ejection FIFO.
 */
extern int
bcm_dpp_counter_bg_enable_get(int unit,
                              int *enable);
extern int 
bcm_dpp_counter_stag_lsb_get(
        int unit,
        uint32* stag_lsb,
        uint8* enable);

extern int 
bcm_dpp_counter_lif_range_is_allocated(
   int unit, 
   uint8* is_allocated);
extern int 
bcm_dpp_counter_lif_range_source_is_allocated(
   int unit, 
   SOC_TMC_CNT_SRC_TYPE source, 
   int range_id, 
   uint8* is_allocated);

extern int 
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
   );

extern int 
bcm_dpp_counter_lif_range_set(
   int                  unit,
   int                  counting_profile,
   SOC_TMC_CNT_SRC_TYPE source,
   int                  range_id,
   int                  stif_counter_id,   
   int                  is_double_entry,       
   SOC_SAND_U32_RANGE   *range,
   int engine_range_offset
   );
extern int 
bcm_dpp_counter_lif_counting_priority_set (
   int                                  unit, 
   SOC_TMC_CNT_SRC_TYPE                 source,
   int                                  command_id,
   bcm_stat_counter_lif_stack_id_t      lif_stack_level, 
   int                                  priority);

extern int 
bcm_dpp_counter_lif_base_val_get(
   int                  unit,
   SOC_TMC_CNT_SRC_TYPE source,
   int                  command_id,
   int                  *base_val);

extern int 
bcm_dpp_counter_lif_counting_priority_get (
   int                                  unit, 
   SOC_TMC_CNT_SRC_TYPE                 source,
   int                                  command_id,
   bcm_stat_counter_lif_stack_id_t      lif_stack_level, 
   int                                  *priority);

extern int
bcm_dpp_counter_cache_only_set(
    int unit, 
    uint8 cache_only);

extern int
bcm_dpp_counter_cache_only_get(
    int unit, 
    uint8* cache_only);

extern int
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
        int counter_profile_offset);
int
bcm_dpp_counter_config_clear(int unit, soc_port_t crps_index, uint8 clear_cache);
int
bcm_dpp_counter_config_get(
        int unit,
        soc_port_t crps_index,
        uint8* enabled,
        int* src_core,
        SOC_TMC_CNT_SRC_TYPE* src_type,
        int *command_id,
        int *command_base_offset,
        SOC_TMC_CNT_MODE_EG_TYPE* eg_type,
        SOC_TMC_CNT_FORMAT* format,
        SOC_TMC_CNT_COUNTER_WE_BITMAP* we_val,
        SOC_TMC_CNT_CUSTOM_MODE_PARAMS* custom_mode_params,
        SOC_TMC_CNT_Q_SET_SIZE* q_set_size,
        uint32* stag_lsb,
        int* counter_profile_offset);

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern int
_bcm_dpp_counters_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */
         
int
bcm_dpp_counter_state_lock_take(int unit);
int
bcm_dpp_counter_state_lock_give(int unit);
int
bcm_dpp_counter_state_diag_unmask(int unit);
int 
bcm_dpp_counter_state_diag_mask(int unit);

/*
 *  Name
 *    _bcm_dpp_modify_proc_and_index
 *  Purpose
 *    modify the proc and index before reading/writing the counter value from the SW state DB.
 *  Arguments
 *    IN _bcm_dpp_counter_state_t * unitData
 *    OUT unsigned int * proc
 *    OUT unsigned int * finalIndex - the modified index
 *    uint8 subCount - is the function called after subCount was made or not.
 *   Returns
 *     bcm_error_t cast as int
 *       BCM_E_NONE if success
 *  Notes
 *          FOR INGRESS_PP, groupSize=5, num_counters/groupSize is not devided. it cause that the last group in the engine is split between
 *          the current engine and the next one (if defined). therefore, need to modify the index and the proc
 */
int
_bcm_dpp_modify_proc_and_index(_bcm_dpp_counter_state_t * unitData, unsigned int *proc, unsigned int * finalIndex, uint8 subCount);

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
_bcm_dpp_counter_engine_special_condition(int unit, _bcm_dpp_counter_state_t *unitData, int proc, int *specialCondition);
int 
bcm_dpp_counter_lif_local_profile_get(int unit, int counting_profile, uint8 *local_counting_profile_id);

int 
bcm_dpp_counter_stat_id_create(
    int unit,
    bcm_gport_t counter_source_gport,
    uint32 counter_source_id, 
    bcm_stat_counter_source_type_t counter_source_type, 
    int command_id, 
    bcm_core_t core_id,
    int *stat_id
    );

#endif /* ndef BCM_INT_DPP_COUNTERS_H */

