/** \file oam_oamp_interrupts.c
 * 
 *
 * OAMP Interrupt handling procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_OAM

 /*
  * Include files.
  * {
  */
#include <bcm_int/dnx/sat/sat.h>
#include <src/bcm/dnx/oam/oam_oamp.h>
#include <src/bcm/dnx/oam/oam_internal.h>
#include <src/bcm/dnx/bfd/bfd_internal.h>
#include "include/bcm/bfd.h"
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>
#include <bcm_int/dnx/algo/oam/algo_oam.h>
#include <shared/hash_tbl.h>
#include <soc/dnx/dnx_fifodma.h>
#include <soc/dnxc/dnxc_ha.h>
#include <soc/register.h>
/*
 * }
 */

/*
 * DEFINEs
 * {
 */

/*
 * SAT Interrupt Format
 *   0 - 7   - FLOW-ID
 *   8 - 13  - Reserved(0)
 *   15 - 15 - Delay set
 *   16 - 19 - Seq number set
 *   20 - 20 - Default(0)
 *   21 - 22 - Event Type ('11' - SAT)
 */
#define DNX_OAM_OAMP_SAT_FLOW_ID_EVENT_MASK     0x7F
#define DNX_OAM_OAMP_SAT_DELAY_SEQ_NUM_SET_MASK 0x1
#define DNX_OAM_OAMP_SAT_SIZE_MASK              0xf
#define DNX_OAM_OAMP_SAT_FRAME_DEALY_MSB_MASK   0x3FF
#define DNX_OAM_OAMP_SAT_FRAME_DEALY_LSB_MASK   0xffffffff
#define DNX_OAM_OAMP_SAT_DELAY_SET_SHIFT        14
#define DNX_OAM_OAMP_SAT_SEQ_NUM_SET_SHIFT      15
#define DNX_OAM_OAMP_SAT_SIZE_SHIFT             16
#define DNX_OAM_OAMP_SAT_FRAME_DEALY_MSB_SHIFT  23
#define DNX_OAM_OAMP_SAT_TYPE                   3

/** Same for all stat events */
#define DNX_OAM_OAMP_STAT_EVENT_TYPE_MASK       0x3
#define DNX_OAM_OAMP_STAT_EVENT_TYPE_SHIFT      21

/*
 * Report Interrupt Format
 *   0 - 15   - MEP-ID
 *   16 - 19  - Event size
 *   20 - 20  - MEP event type ('0' - LM/'1' - DM)
 *   21 - 22  - Event type ('10' - Report)
 *   23 - 31  - Reserved(0)
 */
#define DNX_OAM_OAMP_REPORT_MEP_ID_MASK         0xFFFF
#define DNX_OAM_OAMP_REPORT_EVENT_SIZE_MASK     0xF
#define DNX_OAM_OAMP_REPORT_MEP_TYPE_MASK       1
#define DNX_OAM_OAMP_REPORT_EVENT_SIZE_SHIFT    16
#define DNX_OAM_OAMP_REPORT_MEP_TYPE_SHIFT      20
#define DNX_OAM_OAMP_REPORT_TYPE                2

/* Maximum number of events that can stored in SW */
#define DNX_OAM_OAMP_FIFO_MAX_NUMBER_OF_EVENTS_IN_SW 1024

/*
 * Max number of JR2 format events is 13 since 640 bits/48 bits = 13.
 */
#define DNX_OAM_OAMP_MAX_JR2_FORMAT_EVENTS 13

/*
 * Size of interrupt register
 */
#define DNX_OAM_OAMP_INTERRUPT_REG_BYTES 80

/*
 * Max number of JR2 format events is 5 and min 2
 * We have total 640 bits
 *         * 32 bits is entry size
 *         * 2, 3, 5 is the event size
 *         * 2 * 32b = 64b - minumum size of event
 *         * 5 * 32b = 160b - maximum size of event
 *         --------------------------------
 *         => 640bits/(2 * 32bits) = 10
 * Event size:
 *       * Normal mode  = lm ? 3 : (one DM ? 5 : 3)
 *       * Compact mode = lm ? 3 : (one DM ? 3 : 2)
 */
#define DNX_OAM_OAMP_MAX_JR2_FORMAT_REPORT 10
/*
 * Max number of words in an report data(read by interrupt handler)
 */
#define DNX_OAM_OAMP_REPORT_MAX_DATA_WORDS 4

/*
 * Max number of words in an event data(read by interrupt handler)
 */
#define DNX_OAM_OAMP_EVENT_MAX_DATA_WORDS 2

/** JR2 EVENT FORMAT */
typedef struct dnx_oam_oamp_interrupt_jr2_event_data_s
{
    /** Pointer to the OAMP_RMEP_DB for the RMEP for which event was raised */
    uint16 rmep_db_ptr;
    /** 00 - No LOC event 01 - LOC event 10 - Almost LOC event 11 - LOC Clear event */
    dbal_enum_value_field_loc_event_e loc_evt;
    /** Indicates RDI set event */
    uint8 rdi_set;
    /** Indicates RDI clear event */
    uint8 rdi_clear;
    /** Indicates there is a change in RMEP state */
    uint8 rmep_state_change;
    /** Indicates if the event is valid or not */
    uint8 is_valid;
    /** If RMEP_STATE_CHANGE is true, this gives the changed RMEP STATE for OAM/BFD */
    union
    {
        dnx_oam_oamp_oam_state_t eth_state;

        dnx_oam_oamp_bfd_state_t bfd_state;
    } rmep_state;
    /** Indicates if the event is oam or not */
    uint8 is_oam;
} dnx_oam_oamp_interrupt_jr2_event_data_t;

/** JR2 REPORT FORMAT */
typedef struct dnx_oam_oamp_interrupt_jr2_report_data_s
{
    /** MEP-ID as received in incoming packet */
    uint16 mep_id;

    /** Indicates if the event is valid or not */
    uint8 is_valid;

    uint8 event_size;
    /** 0-LM, 1-DM */
    uint8 mep_event_type;

    /*
     * Max number of JR2 format report size is 5
     *   * Event size:
     *       * Normal mode  = lm ? 3 : (one DM ? 5 : 3)
     *       * Compact mode = lm ? 3 : (one DM ? 3 : 2)
     */
    uint32 data[DNX_OAM_OAMP_REPORT_MAX_DATA_WORDS];
} dnx_oam_oamp_interrupt_jr2_report_data_t;

/** The interrupt event data. Format and the data array */
typedef struct dnx_oam_interrupt_data_s
{
    union
    {
        /*
         * The actual event data (in JR2 event format). Valid if
         * event format is 0x00. 48 bits size.
         */
        dnx_oam_oamp_interrupt_jr2_event_data_t jr2_event_data[DNX_OAM_OAMP_MAX_JR2_FORMAT_EVENTS];

        /*
         * The actual event data (in JR2 report format). Valid if
         * event format is 0x10. 32 bits size.
         */
        dnx_oam_oamp_interrupt_jr2_report_data_t jr2_report_data[DNX_OAM_OAMP_MAX_JR2_FORMAT_REPORT];

    } interrupt_data;

    union
    {
        /*
         * The actual event data (in JR2 event format). Valid if
         * event format is 0x00. 48 bits size.
         */
        dnx_oam_oamp_interrupt_jr2_event_data_t jr2_event_message;

        /*
         * The actual event data (in JR2 report format). Valid if
         * event format is 0x10. 48 bits size.
         */
        dnx_oam_oamp_interrupt_jr2_report_data_t jr2_report_message;

    } interrupt_entry;

} dnx_oam_interrupt_data_t;

/*Structure preserving all "global" data associated with an interrupt.*/
typedef struct
{
    /*
     * Field represents the freshest copy of the interrupt message. Only this should be processed directly.
     */
    dnx_oam_interrupt_data_t interrupt_message;

   /** Used only when using DMA interrupts */
    /*
     * Upon entering an interrupt a "chunk of memory should be copy from the "host memory" (shared by the DMA) to this
     * memory
     */
    uint32 *buffer_copied_from_dma_host_memory;
    uint8 internal_buffer_is_allocated;
    /*
     * Represents amount of valid entries in buffer_copied_from_dma_host_memory
     */
    int num_entries_available_in_local_buffer;
    /*
     * Pointer to current location in buffer_copied_from_dma_host_memory
     */
    int num_entries_read_in_local_buffer;
} dnx_oam_oamp_interrupt_global_data_t;
/* OAM event types generated by OAMP */
typedef enum
{
    DNX_OAM_OAMP_DMA_EVENT_TYPE_EVENT,
    DNX_OAM_OAMP_DMA_EVENT_TYPE_STAT_EVENT,
    DNX_OAM_OAMP_DMA_EVENT_TYPE_NOF
} dnx_oam_oamp_dma_event_type_t;

/*Used to store data on all events. from the FIFO.*/
typedef struct
{
    uint16 mep_or_rmep_indx;    /* index in the (R)MEP DB (not the actual (R)MEP ID) */
    uint8 is_oam;
    uint8 event_number;         /* number from the bcm_bfd/oam_event_type_t enum */
    uint32 event_data[DNX_OAM_OAMP_REPORT_MAX_DATA_WORDS];      /* Used for statistic interface (LM/DM) */
} dnx_oam_event_and_rmep_info;

/*
The following structures are used for event FIFO implementation.
The implementation is as such:
    The hash table is used to store the multi-event indication and potentially reference counter
    The following linked list is then iterated on in the end.
When an event is encounter, look for it in the hash table.
    if it exists, update the reference counter in the hash table and set the multi event indication
    else insert it into the linked list and the hash table.

In the end the events are popped one by one (FIFO) from the list and the reference counter and multi-event indication is gotten from the hash table
*/
typedef struct dnx_oam_oamp_eventt_Node_s
{
    dnx_oam_event_and_rmep_info eventt;
    struct dnx_oam_oamp_eventt_Node_s *next;
} dnx_oam_oamp_eventt_Node_t;

typedef struct
{
    dnx_oam_oamp_eventt_Node_t *head;
    dnx_oam_oamp_eventt_Node_t *tail;
} dnx_oam_oamp_eventt_list_t;

/*
 * }
 */

/*
 * MACROs
 * {
 */

/*
 * Returns the actual flow-id.
 */
#define DNX_OAM_OAMP_SAT_FLOW_ID_EVENT(event_data) \
            (event_data & DNX_OAM_OAMP_SAT_FLOW_ID_EVENT_MASK)

/*
 * Returns the if delay is set.
 */
#define DNX_OAM_OAMP_SAT_DELAY_SET(event_data) \
            (((event_data) >> DNX_OAM_OAMP_SAT_DELAY_SET_SHIFT)  & DNX_OAM_OAMP_SAT_DELAY_SEQ_NUM_SET_MASK)

/*
 * Returns the if seq number is set.
 */
#define DNX_OAM_OAMP_SAT_SEQ_NUM_SET(event_data) \
            (((event_data) >> DNX_OAM_OAMP_SAT_SEQ_NUM_SET_SHIFT) & DNX_OAM_OAMP_SAT_DELAY_SEQ_NUM_SET_MASK)

/*
 * Returns the actual sat size.
 */
#define DNX_OAM_OAMP_SAT_SIZE(event_data) \
            (((event_data) >> DNX_OAM_OAMP_SAT_SIZE_SHIFT) & DNX_OAM_OAMP_SAT_SIZE_MASK)

/*
 * Returns the actual MSB frame delay.
 */
#define DNX_OAM_OAMP_SAT_FRAME_DEALY_MSB(event_data) \
            (((event_data) >> DNX_OAM_OAMP_SAT_FRAME_DEALY_MSB_SHIFT) & DNX_OAM_OAMP_SAT_FRAME_DEALY_MSB_MASK)

/*
 * Returns the actual event type.
 */
#define DNX_OAM_OAMP_STAT_EVENT_TYPE(event_data) \
            (((event_data) >> DNX_OAM_OAMP_STAT_EVENT_TYPE_SHIFT) & DNX_OAM_OAMP_STAT_EVENT_TYPE_MASK)

/*
 * Returns the report size
 */
#define DNX_OAM_OAMP_REPORT_EVENT_SIZE(event_data) \
            (((event_data) >> DNX_OAM_OAMP_REPORT_EVENT_SIZE_SHIFT) & DNX_OAM_OAMP_REPORT_EVENT_SIZE_MASK)

/*
 * Returns the mep event type (LM/DM).
 */
#define DNX_OAM_OAMP_REPORT_MEP_TYPE(event_data) \
            (((event_data) >> DNX_OAM_OAMP_REPORT_MEP_TYPE_SHIFT) & DNX_OAM_OAMP_REPORT_MEP_TYPE_MASK)

/*
 * Returns the mep id.
 */
#define DNX_OAM_OAMP_REPORT_MEP_ID(event_data) \
            (event_data & DNX_OAM_OAMP_REPORT_MEP_ID_MASK)

/*
 * Returns the actual LSB frame delay.
 */
#define DNX_OAM_OAMP_SAT_FRAME_DEALY_LSB(event_data) \
            ((event_data) & DNX_OAM_OAMP_SAT_FRAME_DEALY_LSB_MASK)

/** Macro to indicate maximum state change events possible in RMEP state */
#define DNX_OAM_OAMP_MAX_STATE_CHANGE_EVENTS_POSSIBLE 3

/** Macros to differentiate BFD and OAM RMEPs based on RMEP name */
#define DNX_BFD_RMEP_SW_INFO_NAME_IS_BFD(rmep_name) (rmep_name == DNX_BFD_RMEP_SW_INFO_NAME_CONST)
#define DNX_BFD_RMEP_SW_INFO_NAME_IS_OAM(rmep_name) (!(DNX_BFD_RMEP_SW_INFO_NAME_IS_BFD(rmep_name)))

/** Macros to parse the RMEP state of OAM/BFD RMEPs from the oamp_event.rmep_state */
#define DNX_OAM_BFD_RMEP_STATE_DIAG_PROF_GET(rmep_state) (rmep_state & 0xf)
#define DNX_OAM_BFD_RMEP_STATE_STATE_GET(rmep_state) ((rmep_state >> 4) & 0x3)
#define DNX_OAM_BFD_RMEP_STATE_FLAGS_PROF_GET(rmep_state) ((rmep_state >> 6) & 0xf)
#define DNX_OAM_BFD_RMEP_STATE_DETECT_MULT_GET(rmep_state) ((rmep_state >> 10) & 0xff)

#define DNX_OAM_BFD_RMEP_STATE_INTF_STATUS_GET(rmep_state) (rmep_state & 0x7)
#define DNX_OAM_BFD_RMEP_STATE_PORT_STATUS_GET(rmep_state) ((rmep_state >> 3) & 0x3)

#define DNX_OAM_OAMP_EVENT_PORT_BLOCKED(_port_status_) \
  ((_port_status_) == 1)

#define DNX_OAM_OAMP_EVENT_PORT_UP(_port_status_) \
  ((_port_status_) == 2)

#define DNX_OAM_OAMP_EVENT_INTERFACE_UP(_intf_status_) \
  ((_intf_status_) == 1)

#define DNX_OAM_OAMP_EVENT_INTERFACE_DOWN(_intf_status_) \
  ((_intf_status_) == 2)

#define DNX_OAM_OAMP_EVENT_INTERFACE_TESTING(_intf_status_) \
  ((_intf_status_) == 3)

#define DNX_OAM_OAMP_EVENT_INTERFACE_UNKNOWN(_intf_status_) \
  ((_intf_status_) == 4)

#define DNX_OAM_OAMP_EVENT_INTERFACE_DORMANT(_intf_status_) \
  ((_intf_status_) == 5)

#define DNX_OAM_OAMP_EVENT_INTERFACE_NOT_PRESENT(_intf_status_) \
  ((_intf_status_) == 6)

#define DNX_OAM_OAMP_EVENT_INTERFACE_LL_DOWN(_intf_status_) \
  ((_intf_status_ & 7) == 7)

/*
 * }
 */

/*
 * Global and Static
 * {
 */

/*
 * }
 */

 /**
 * \brief -  Function that pushes an entry to the end
 *           of the linked list (FIFO).
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] event_list - linked list to be initialized.
 *
 * \return
 *   shr_error_e
 */
void
dnx_oam_oamp_eventt_list_init(
    int unit,
    dnx_oam_oamp_eventt_list_t * event_list)
{
    event_list->head = event_list->tail = NULL;
}

/**
 * \brief -  Function that pushes an entry to the end
 *           of the linked list (FIFO).
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] event_list - linked list where event is pushed.
 * \param [out] event - entry to get pushed.
 *
 * \return
 *   shr_error_e
 */
static shr_error_e
dnx_oam_oamp_eventt_list_push(
    int unit,
    dnx_oam_oamp_eventt_list_t * event_list,
    const dnx_oam_event_and_rmep_info * event)
{
    dnx_oam_oamp_eventt_Node_t *new_node = sal_alloc(sizeof(dnx_oam_oamp_eventt_Node_t), "oam event fifo");
    sal_memset(new_node, 0, sizeof(dnx_oam_oamp_eventt_Node_t));
    if (new_node == NULL)
    {
        return _SHR_E_MEMORY;
    }
    new_node->next = NULL;
    new_node->eventt = *event;
    if (event_list->head != NULL)
    {
        event_list->tail->next = new_node;
        event_list->tail = new_node;
    }
    else
    {
        event_list->head = event_list->tail = new_node;
    }
    return _SHR_E_NONE;
}

/**
 * \brief -  Function that pops an event from the front of
 *           the linked list (FIFO).
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] event_list - linked list where event to be popped.
 * \param [out] event - event is set here.
 *
 * \return
 *   shr_error_e
 */
static shr_error_e
dnx_oam_oamp_eventt_list_pop(
    int unit,
    dnx_oam_oamp_eventt_list_t * event_list,
    dnx_oam_event_and_rmep_info * event)
{
    if (event_list->head)
    {
        dnx_oam_oamp_eventt_Node_t *temp_head = event_list->head;
        *event = event_list->head->eventt;
        if (event_list->head == event_list->tail)
        {
            event_list->head = event_list->tail = NULL;
        }
        else
        {
            event_list->head = event_list->head->next;
        }
        sal_free(temp_head);
        return _SHR_E_NONE;
    }
    else
    {
        return _SHR_E_EMPTY;
    }
}

/**
 * \brief - This function gets the OAMP interrupt data in JR2 event format.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] is_dma_enabled - DMA enabled or not.
 * \param [out] event - Event data information.
 * \param [in,out] valid_event_exist - pointer to event valid existence indication.
 * \param [in] host_entry - Host entry in case of DMA.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   This function gets the interrupt data from OAMP_INTERRUPT_MESSAGE register.
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_interrupt_event_data_jr2_format_get(
    int unit,
    uint8 is_dma_enabled,
    dnx_oam_interrupt_data_t * event,
    uint8 *valid_event_exist,
    void *host_entry)
{
    uint32 entry_handle_id;
    int instance, rv;
    uint32 event_data[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
    uint32 rmep_db_ptr;
    uint32 loc_evt;
    uint32 rdi_set;
    uint32 rdi_clear;
    uint32 rmep_state_change;
    uint32 reserved_1;
    uint32 rmep_state;
    uint32 reserved_2;
    dnx_oam_remote_endpoint_info_t sw_rmep_info;
    int dma_register_size_in_bits;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    *valid_event_exist = 0;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_INTERRUPT_DATA, &entry_handle_id));
    if (!is_dma_enabled)
    {
        /** Read DBAL to get the interrupt message data from OAMP */
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_tables_payload_size_get
                        (unit, DBAL_TABLE_OAMP_INTERRUPT_DATA, 0, &dma_register_size_in_bits));

        /*
         * In OAMP_INTERRUPT_MESSAGEr, there are 13 events, each event has 48bits, totally 640 bits. So 16 bits are invalid
         * The 16 bits locate in event 12 and event 13. Here move the 16 bits.
         */
#if defined(LE_HOST)
        {
            int i = 0;
            for (i = 72; i < 78; i++)
            {
                ((uint8 *) host_entry)[i] = ((uint8 *) host_entry)[i + 2];
            }
            ((uint8 *) host_entry)[78] = 0xff;
            ((uint8 *) host_entry)[79] = 0xff;
        }
#else
        ((uint8 *) host_entry)[74] = ((uint8 *) host_entry)[72];
        ((uint8 *) host_entry)[75] = ((uint8 *) host_entry)[73];
        ((uint8 *) host_entry)[72] = ((uint8 *) host_entry)[78];
        ((uint8 *) host_entry)[73] = ((uint8 *) host_entry)[79];
        ((uint8 *) host_entry)[78] = ((uint8 *) host_entry)[76];
        ((uint8 *) host_entry)[79] = ((uint8 *) host_entry)[77];
        ((uint8 *) host_entry)[76] = 0xff;
        ((uint8 *) host_entry)[77] = 0xff;
#endif /* defined(LE_HOST) */

        /*
         * Update DBAL handle with the event's details including payload and key
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_payload_update
                        (unit, entry_handle_id, NULL, NULL, _SHR_CORE_ALL, dma_register_size_in_bits, host_entry));

    }

    for (instance = 0; instance < DNX_OAM_OAMP_MAX_JR2_FORMAT_EVENTS; instance++)
    {
        /** Update SW structure with OAMP event data */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_JR2_OAMP_EVENT, instance, event_data));
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                        (unit, DBAL_FIELD_JR2_OAMP_EVENT_FORMAT, DBAL_FIELD_RESERVED_1, &(reserved_1), event_data));

        /** reserved_1 is indicator of logical not of valid bit*/
        if (reserved_1)
        {
            continue;
        }

        SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode(unit, DBAL_FIELD_JR2_OAMP_EVENT_FORMAT,
                                                        DBAL_FIELD_RMEP_INDEX, &(rmep_db_ptr), event_data));
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode(unit, DBAL_FIELD_JR2_OAMP_EVENT_FORMAT,
                                                        DBAL_FIELD_LOC_EVENT, &(loc_evt), event_data));
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode(unit, DBAL_FIELD_JR2_OAMP_EVENT_FORMAT,
                                                        DBAL_FIELD_RDI_SET, &(rdi_set), event_data));
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode(unit, DBAL_FIELD_JR2_OAMP_EVENT_FORMAT,
                                                        DBAL_FIELD_RDI_CLEAR, &(rdi_clear), event_data));
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode(unit, DBAL_FIELD_JR2_OAMP_EVENT_FORMAT,
                                                        DBAL_FIELD_RMEP_STATE_CHANGE, &(rmep_state_change),
                                                        event_data));
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                        (unit, DBAL_FIELD_JR2_OAMP_EVENT_FORMAT, DBAL_FIELD_RMEP_STATE, &(rmep_state), event_data));
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                        (unit, DBAL_FIELD_JR2_OAMP_EVENT_FORMAT, DBAL_FIELD_RESERVED_2, &(reserved_2), event_data));

        /** Update RMEP Index in event data */
        event->interrupt_data.jr2_event_data[instance].rmep_db_ptr = rmep_db_ptr;
        sal_memset(&sw_rmep_info, 0, sizeof(sw_rmep_info));
        rv = dnx_oam_sw_state_remote_endpoint_info_get(unit, rmep_db_ptr, &sw_rmep_info);
        if (rv != _SHR_E_NONE)
        {
            /** Don't process the event if SW does not have the EP context */
            /** Update the entry as invalid */
            event->interrupt_data.jr2_event_data[instance].is_valid = 0;
            continue;
        }
        event->interrupt_data.jr2_event_data[instance].loc_evt = loc_evt;
        event->interrupt_data.jr2_event_data[instance].rdi_set = rdi_set;
        event->interrupt_data.jr2_event_data[instance].rdi_clear = rdi_clear;
        event->interrupt_data.jr2_event_data[instance].rmep_state_change = rmep_state_change;
        event->interrupt_data.jr2_event_data[instance].is_valid = !reserved_1;
        /** Update valid entry existence to the caller */
        if (event->interrupt_data.jr2_event_data[instance].is_valid)
        {
            *valid_event_exist = 1;
        }
        /** Using the RMEP name to identify if it is BFD or OAM RMEP */
        if (DNX_BFD_RMEP_SW_INFO_NAME_IS_BFD(sw_rmep_info.rmep_id))
        {   /** Update RMEP state */
            event->interrupt_data.jr2_event_data[instance].rmep_state.bfd_state.flags_profile =
                DNX_OAM_BFD_RMEP_STATE_FLAGS_PROF_GET(rmep_state);
            event->interrupt_data.jr2_event_data[instance].rmep_state.bfd_state.state =
                DNX_OAM_BFD_RMEP_STATE_STATE_GET(rmep_state);
            event->interrupt_data.jr2_event_data[instance].rmep_state.bfd_state.diag_profile =
                DNX_OAM_BFD_RMEP_STATE_DIAG_PROF_GET(rmep_state);
            event->interrupt_data.jr2_event_data[instance].rmep_state.bfd_state.detect_multiplier =
                DNX_OAM_BFD_RMEP_STATE_DETECT_MULT_GET(rmep_state);
            event->interrupt_data.jr2_event_data[instance].is_oam = 0;
        }
        else
        {   /** Update RMEP state */
            event->interrupt_data.jr2_event_data[instance].rmep_state.eth_state.interface_status =
                DNX_OAM_BFD_RMEP_STATE_INTF_STATUS_GET(rmep_state);
            event->interrupt_data.jr2_event_data[instance].rmep_state.eth_state.port_status =
                DNX_OAM_BFD_RMEP_STATE_PORT_STATUS_GET(rmep_state);
            event->interrupt_data.jr2_event_data[instance].is_oam = 1;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function reads data from HW interrupt message and update
 *  software events data structure.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in,out] valid_event_exist - pointer to event valid existence indication.
 * \param [out] event - Event data information.
 * \param [in] host_entry - Host entry in case of DMA.
 *
 * \return
 *   shr_error_e
 */
static shr_error_e
dnx_oam_oamp_event_fifo_read(
    int unit,
    uint8 *valid_event_exist,
    dnx_oam_interrupt_data_t * event,
    void *host_entry)
{
    uint8 oam_use_fifo_dma = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Normal or DMA event
     */
    oam_use_fifo_dma = dnx_data_oam.oamp.oamp_fifo_dma_event_interface_enable_get(unit);
    /*
     * Use DBAL interface to read interrupt message data and update the interrupt_data structure
     */
    /** We currently only support JR2 format. */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_interrupt_event_data_jr2_format_get
                    (unit, oam_use_fifo_dma, event, valid_event_exist, host_entry));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function converts the OAMP event to BCM OAM event type.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] event_data - OAMP event data to which bcm event type has to be derived.
 * \param [in] event_type - Array of bcm event type(s) corresponding to an OAMP event(s).
 *
 * \return
 *   shr_error_e
 */
static shr_error_e
dnx_oam_oamp_event_to_bcm_oam_event_type(
    int unit,
    const dnx_oam_interrupt_data_t * event_data,
    bcm_oam_event_type_t * event_type)
{
    int index = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (event_data->interrupt_entry.jr2_event_message.rdi_set)
    {
        event_type[index] = bcmOAMEventEndpointRemote;
        index++;
    }
    else if (event_data->interrupt_entry.jr2_event_message.rdi_clear)
    {
        event_type[index] = bcmOAMEventEndpointRemoteUp;
        index++;
    }
    else if (event_data->interrupt_entry.jr2_event_message.loc_evt)
    {
        switch (event_data->interrupt_entry.jr2_event_message.loc_evt)
        {
            case DBAL_ENUM_FVAL_LOC_EVENT_LOC_EVENT:
                event_type[index] = bcmOAMEventEndpointCCMTimeout;
                break;
            case DBAL_ENUM_FVAL_LOC_EVENT_LOC_CLEAR_EVENT:
                event_type[index] = bcmOAMEventEndpointCCMTimein;
                break;
            case DBAL_ENUM_FVAL_LOC_EVENT_ALMOST_LOC_EVENT:
                event_type[index] = bcmOAMEventEndpointCCMTimeoutEarly;
                break;
            default:
                SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_PARAM, "Error: Illegal OAM LOC event %d \n%s%s",
                                         event_data->interrupt_entry.jr2_event_message.loc_evt, EMPTY, EMPTY);
        }
        index++;
    }

    if (event_data->interrupt_entry.jr2_event_message.rmep_state_change)
    {
        dnx_oam_oamp_oam_state_t rmep_state;
        rmep_state = event_data->interrupt_entry.jr2_event_message.rmep_state.eth_state;
        /*
         * Check port status
         */
        if (DNX_OAM_OAMP_EVENT_PORT_BLOCKED(rmep_state.port_status))
        {
            event_type[index] = bcmOAMEventEndpointPortDown;
            index++;
        }
        else if (DNX_OAM_OAMP_EVENT_PORT_UP(rmep_state.port_status))
        {
            event_type[index] = bcmOAMEventEndpointPortUp;
            index++;
        }
        /*
         * Check interface status
         */
        if (DNX_OAM_OAMP_EVENT_INTERFACE_UP(rmep_state.interface_status))
        {
            event_type[index] = bcmOAMEventEndpointInterfaceUp;
        }
        else if (DNX_OAM_OAMP_EVENT_INTERFACE_DOWN(rmep_state.interface_status))
        {
            event_type[index] = bcmOAMEventEndpointInterfaceDown;
        }
        else if (DNX_OAM_OAMP_EVENT_INTERFACE_TESTING(rmep_state.interface_status))
        {
            event_type[index] = bcmOAMEventEndpointInterfaceTesting;
        }
        else if (DNX_OAM_OAMP_EVENT_INTERFACE_UNKNOWN(rmep_state.interface_status))
        {
            event_type[index] = bcmOAMEventEndpointInterfaceUnkonwn;
        }
        else if (DNX_OAM_OAMP_EVENT_INTERFACE_DORMANT(rmep_state.interface_status))
        {
            event_type[index] = bcmOAMEventEndpointInterfaceDormant;
        }
        else if (DNX_OAM_OAMP_EVENT_INTERFACE_NOT_PRESENT(rmep_state.interface_status))
        {
            event_type[index] = bcmOAMEventEndpointInterfaceNotPresent;
        }
        else if (DNX_OAM_OAMP_EVENT_INTERFACE_LL_DOWN(rmep_state.interface_status))
        {
            event_type[index] = bcmOAMEventEndpointInterfaceLLDown;
        }
    }

    if (!index && event_type[index] == bcmOAMEventCount)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_PARAM, "Error: Illegal OAMP OAM event \n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function converts the OAMP event to BCM BFD event type.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] event_data - OAMP event data to which bcm BFD event type has to be derived.
 * \param [in] event_type - bcm BFD event type corresponding to an event.
 *
 * \return
 *   shr_error_e
 */
static shr_error_e
dnx_oam_oamp_event_to_bcm_bfd_event_type(
    int unit,
    const dnx_oam_interrupt_data_t * event_data,
    bcm_bfd_event_type_t * event_type)
{
    SHR_FUNC_INIT_VARS(unit);

    if (event_data->interrupt_entry.jr2_event_message.loc_evt)
    {
        switch (event_data->interrupt_entry.jr2_event_message.loc_evt)
        {
            case DBAL_ENUM_FVAL_LOC_EVENT_LOC_EVENT:
                *event_type = bcmBFDEventEndpointTimeout;
                break;
            case DBAL_ENUM_FVAL_LOC_EVENT_LOC_CLEAR_EVENT:
                *event_type = bcmBFDEventEndpointTimein;
                break;
            case DBAL_ENUM_FVAL_LOC_EVENT_ALMOST_LOC_EVENT:
                *event_type = bcmBFDEventEndpointTimeoutEarly;
                break;
            default:
                SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_PARAM, "Error: Illegal BFD LOC event %d \n%s%s",
                                         event_data->interrupt_entry.jr2_event_message.loc_evt, EMPTY, EMPTY);
        }
    }
    else if (event_data->interrupt_entry.jr2_event_message.rmep_state_change)
    {
        *event_type = bcmBFDEventStateChange;
    }
    else
    {
        *event_type = bcmBFDEventCount;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function inserts an event to the event hash table.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] htbl - Pointer to hash table.
 * \param [in] event_list - Pointer to event list.
 * \param [in] eventt - Pointer to event data.
 *
 * \return
 *   shr_error_e
 */
static shr_error_e
dnx_oam_oamp_insert_event_into_htbl_and_fifo(
    int unit,
    shr_htb_hash_table_t htbl,
    dnx_oam_oamp_eventt_list_t * event_list,
    dnx_oam_event_and_rmep_info * eventt)
{
    int rv;
    shr_htb_data_t data;
    SHR_FUNC_INIT_VARS(unit);

    rv = shr_htb_insert(htbl, (shr_htb_key_t) eventt, (shr_htb_data_t) 0);

    if (rv == _SHR_E_EXISTS)
    {
        /** Make sure data is not 0, but the flag */
        SHR_IF_ERR_EXIT(shr_htb_find(htbl, (shr_htb_key_t) eventt, &data, 1 /* remove */ ));
        SHR_IF_ERR_EXIT(shr_htb_insert(htbl, (shr_htb_key_t) eventt, INT_TO_PTR(BCM_OAM_EVENT_FLAGS_MULTIPLE)));
    }
    else
    {
        /*
         *  Insert event to FIFO
         */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_eventt_list_push(unit, event_list, eventt));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function updates the OAMP event into
 *          SW structures(hash and FIFO) maintained
 *          by doing the necessary conversions.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] is_report_mode - if set update the statistics.
 * \param [in] htbl - Pointer to hash table.
 * \param [in] event_list - Pointer to event list.
 * \param [in] event_data - Pointer to event data.
 * \param [in] event_found - Pointer to use in hash table
 *
 * \return
 *   shr_error_e
 */
static shr_error_e
dnx_oam_oamp_event_update_sw(
    int unit,
    uint8 is_report_mode,
    shr_htb_hash_table_t htbl,
    dnx_oam_oamp_eventt_list_t * event_list,
    const dnx_oam_interrupt_data_t * event_data,
    dnx_oam_event_and_rmep_info * event_found)
{
    int event_idx = 0;
    bcm_bfd_event_type_t bfd_event_type = bcmBFDEventCount;
    bcm_oam_event_type_t oam_event_type[DNX_OAM_OAMP_MAX_STATE_CHANGE_EVENTS_POSSIBLE] =
        { bcmOAMEventCount, bcmOAMEventCount, bcmOAMEventCount };
    SHR_FUNC_INIT_VARS(unit);
    sal_memset(event_found, 0, sizeof(dnx_oam_event_and_rmep_info));

    if (is_report_mode)
    {
        event_found->is_oam = 1;
        event_found->mep_or_rmep_indx = event_data->interrupt_entry.jr2_report_message.mep_id;
        event_found->event_number =
            event_data->interrupt_entry.
            jr2_report_message.mep_event_type ? bcmOAMEventEndpointDmStatistics : bcmOAMEventEndpointLmStatistics;
        if (event_data->interrupt_entry.jr2_report_message.event_size == 3)
        {
            /**
            * LM - dFcb
            * DM - 32b lsb
            */
            event_found->event_data[0] = event_data->interrupt_entry.jr2_report_message.data[0];
            /**
            * LM - dFcf
            * DM - 32b msb
            */
            event_found->event_data[1] = event_data->interrupt_entry.jr2_report_message.data[1];
        }
        else if (event_data->interrupt_entry.jr2_report_message.event_size == 2)
        {
            if (event_data->interrupt_entry.jr2_report_message.mep_event_type)
            {
                /**
                * DM Type, Compact Mode.
                * Last Delay = [0-31]
                */
                event_found->event_data[0] = event_data->interrupt_entry.jr2_report_message.data[0];
                event_found->event_data[1] = 0;
            }
            else
            {
                /**
                * LM Type, Compact Mode.
                * delta Fcf = [0-15]
                * delta Fcb = [16-31]
                */
                event_found->event_data[1] = event_data->interrupt_entry.jr2_report_message.data[0] & 0xFFFF;
                event_found->event_data[0] = (event_data->interrupt_entry.jr2_report_message.data[0] >> 0x10) & 0xFFFF;
            }
        }
        else
        {
            if (event_data->interrupt_entry.jr2_report_message.mep_event_type)
            {
                /**
                * Possible only for one way DM. Event size = 5.
                * Push 2 reports to handle the case.
                */
                event_found->event_data[0] = event_data->interrupt_entry.jr2_report_message.data[0];
                event_found->event_data[1] = event_data->interrupt_entry.jr2_report_message.data[1];
                event_found->event_data[2] = event_data->interrupt_entry.jr2_report_message.data[2];
                event_found->event_data[3] = event_data->interrupt_entry.jr2_report_message.data[3];
            }
        }

       /** Add the event to Hash table and event FIFO */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_insert_event_into_htbl_and_fifo(unit, htbl, event_list, event_found));
    }
    else
    {
        event_found->mep_or_rmep_indx = event_data->interrupt_entry.jr2_event_message.rmep_db_ptr;
        if (event_data->interrupt_entry.jr2_event_message.is_oam)
        {
            /*
             * OAM event
             */
            event_found->is_oam = 1;
            /** Convert OAMP event(s) to bcm event type(s) */
            SHR_IF_ERR_EXIT(dnx_oam_oamp_event_to_bcm_oam_event_type(unit, event_data, oam_event_type));

            if (event_data->interrupt_entry.jr2_event_message.rmep_state_change)
            {   /** if there are two status events from OAMP, add both the events */
                for (event_idx = 0; event_idx < DNX_OAM_OAMP_MAX_STATE_CHANGE_EVENTS_POSSIBLE; event_idx++)
                {
                    if (oam_event_type[event_idx] != bcmOAMEventCount)
                    {
                        event_found->event_number = oam_event_type[event_idx];
                        /** Add the event to Hash table and event FIFO */
                        SHR_IF_ERR_EXIT(dnx_oam_oamp_insert_event_into_htbl_and_fifo
                                        (unit, htbl, event_list, event_found));
                    }
                }
            }
            else
            {
                event_found->event_number = oam_event_type[event_idx];
                /** Add the event to Hash table and event FIFO */
                SHR_IF_ERR_EXIT(dnx_oam_oamp_insert_event_into_htbl_and_fifo(unit, htbl, event_list, event_found));
            }
        }
        else
        {       /* BFD event */
            event_found->is_oam = 0;
            /** Convert OAMP event to bcm event type */
            SHR_IF_ERR_EXIT(dnx_oam_oamp_event_to_bcm_bfd_event_type(unit, event_data, &bfd_event_type));
            if (bfd_event_type != bcmBFDEventCount)
            {
                event_found->event_number = (uint8) bfd_event_type;
                /** Add the event to Hash table and event FIFO */
                SHR_IF_ERR_EXIT(dnx_oam_oamp_insert_event_into_htbl_and_fifo(unit, htbl, event_list, event_found));
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function  gets information on each
 * stored event and calls the proper call back with that
 * information.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] event_info - Pointer to event data.
 * \param [in] really_the_flags - "global" data used by soc layer.
 *                       The reason this is not used as a global
 *                       variable is so that the interrupts will
 *                       be reentrable. The data is only global
 *                       per interrupt.
 *
 * \return
 *   shr_error_e
 */
static shr_error_e
dnx_oam_oamp_fifo_interrupt_handle_user_callback(
    int unit,
    dnx_oam_event_and_rmep_info * event_info,
    shr_htb_data_t really_the_flags)
{
    int rmeb_db_ndx = 0, rv;
    dnx_oam_remote_endpoint_info_t rmep_info;
    dnx_oam_endpoint_info_t mep_info;
    int flags = PTR_TO_INT(really_the_flags);
    bcm_bfd_event_types_t event_types;

    SHR_FUNC_INIT_VARS(unit);

    if (event_info->is_oam)
    {
        /*
         * OAM event
         */
        if (_g_oam_event_cb[unit][event_info->event_number])
        {
            rv = dnx_oam_sw_state_remote_endpoint_info_get(unit, event_info->mep_or_rmep_indx, &rmep_info);
            if (rv != _SHR_E_NONE)
            {
                LOG_WARN(BSL_LS_BCM_OAM, (BSL_META_U(unit, "Warning: Remote Endpoint %d"
                                                     " already destroyed.\n"), event_info->mep_or_rmep_indx));
                SHR_EXIT();
            }

            DNX_OAM_REMOTE_MEP_INDEX_FROM_INTERNAL(event_info->mep_or_rmep_indx, rmeb_db_ndx);

            /** Fetch EP information for Group information */
            rv = dnx_oam_sw_state_endpoint_info_get(unit, rmep_info.oam_id, &mep_info);
            if (rv != _SHR_E_NONE)
            {
                LOG_WARN(BSL_LS_BCM_OAM, (BSL_META_U(unit, "Warning: Endpoint %d"
                                                     " already destroyed.\n"), rmep_info.oam_id));
                SHR_EXIT();
            }
            /*
             * finally, the callback
             */
            _g_oam_event_cb[unit][event_info->event_number] (unit, flags, event_info->event_number, mep_info.group,
                                                             rmeb_db_ndx,
                                                             _g_oam_event_ud[unit][event_info->event_number]);
        }
        else
        {
            bcm_oam_performance_event_data_t report_data;
            sal_memset(&report_data, 0, sizeof(bcm_oam_performance_event_data_t));

            /** Fetch EP information for Group information */
            rv = dnx_oam_sw_state_endpoint_info_get(unit, event_info->mep_or_rmep_indx, &mep_info);
            if (rv != _SHR_E_NONE)
            {
                /** Check if lm session exists for this oam id */
                rv = dnx_oam_sw_db_lm_session_oam_id_get(unit, event_info->mep_or_rmep_indx,
                                                         &event_info->mep_or_rmep_indx);
                if (rv != _SHR_E_NONE)
                {
                    LOG_WARN(BSL_LS_BCM_OAM, (BSL_META_U(unit, "Warning: Endpoint %d"
                                                         " already destroyed.\n"), event_info->mep_or_rmep_indx));
                    SHR_EXIT();
                }
                else
                {
                    /** Fetch EP information again if lm session exists */
                    rv = dnx_oam_sw_state_endpoint_info_get(unit, event_info->mep_or_rmep_indx, &mep_info);
                    if (rv != _SHR_E_NONE)
                    {
                        LOG_WARN(BSL_LS_BCM_OAM, (BSL_META_U(unit, "Warning: Endpoint %d"
                                                             " already destroyed.\n"), event_info->mep_or_rmep_indx));
                        SHR_EXIT();
                    }
                }
            }

            if (_g_oam_performance_event_cb[unit][event_info->event_number])
            {
                if (event_info->event_number == bcmOAMEventEndpointLmStatistics)
                {       /* LM */
                    report_data.delta_FCB = event_info->event_data[0];
                    report_data.delta_FCf = event_info->event_data[1];
                }
                else
                {
                /**
                   * DM
                   * OAMP truncates the two lower bits of the nanoseconds.
                   * SW must recompensate
                */
                    COMPILER_64_SET(report_data.last_delay, (event_info->event_data[1]),
                                    (event_info->event_data[0]) <<= 2);
                    COMPILER_64_SET(report_data.last_delay_near, (event_info->event_data[3]),
                                    (event_info->event_data[2]) <<= 2);
                }

                /*
                 * finally, the callback
                 */
                _g_oam_performance_event_cb[unit][event_info->event_number] (unit, event_info->event_number,
                                                                             mep_info.group,
                                                                             event_info->mep_or_rmep_indx, &report_data,
                                                                             _g_oam_event_ud[unit]
                                                                             [event_info->event_number]);
            }

        }
    }
    else
    {   /** bfd event*/
        if (_g_bfd_event_cb[unit][event_info->event_number])
        {

            rv = dnx_oam_sw_state_remote_endpoint_info_get(unit, event_info->mep_or_rmep_indx, &rmep_info);
            if (rv != _SHR_E_NONE)
            {
                LOG_WARN(BSL_LS_BCM_OAM, (BSL_META_U(unit, "Warning: Remote Endpoint %d"
                                                     " already destroyed.\n"), event_info->mep_or_rmep_indx));
                SHR_EXIT();
            }

            BCM_BFD_EVENT_TYPE_CLEAR_ALL(event_types);
            BCM_BFD_EVENT_TYPE_SET(event_types, event_info->event_number);
            /*
             * finally, the callback
             */
            _g_bfd_event_cb[unit][event_info->event_number] (unit, flags, event_types, rmep_info.oam_id,
                                                             _g_bfd_event_ud[unit][event_info->event_number]);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * Struct used to contain data structures used to manage multiple events.
 * In case of DMA usage, in the first time dnx_oam_fifo_interrupt_handler will be called
 * (i.e. when entry_number ==1) the struct will be allocated and filled, in next iterations
 * dnx_oam_fifo_interrupt_handler() will read from the struct provided through user_data.
 * In the last iteration (when entry_number == total_amount_of_entries) everything will be handled and freed.
 */
typedef struct
{
    dnx_oam_oamp_eventt_list_t *event_list;
    shr_htb_hash_table_t event_htbl;
} dma_interrupt_global_data_structures;

/**
 * \brief - This function handles the interrupts generated by OAMP.
 * Should not be called directly, rather through callback in soc
 * layer.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] host_entry - Host entry in case of DMA.
 * \param [in] entry_number - Number of entry in case used in DMA
 * \param [in] total_amount_of_entries - Total number of entries in case of DMA.
 * \param [in,out] user_data - user data in case used in DMA. In the first iteration will point to void * pointer,
 *                             in subsequent iterations will point to a pointer to a dma_interrupt_global_data_structures
 *
 * \return
 *   Error value
 */
static shr_error_e
dnx_oam_fifo_interrupt_handler(
    int unit,
    void *host_entry,
    int entry_number,
    int total_amount_of_entries,
    void **user_data)
{
    int rv;
    uint32 event_index;
    uint8 valid_event_exist = 0;
    dnx_oam_event_and_rmep_info event_found;
    dnx_oam_interrupt_data_t event_data;
    shr_htb_data_t data;
    int fifo_read_counter = 0;
    dnx_oam_interrupt_data_t interrupt_message;
    uint8 oam_use_event_fifo_dma = 0;
    int is_first, is_last;
    dma_interrupt_global_data_structures *int_data = NULL;

    SHR_FUNC_INIT_VARS(unit);
    sal_memset(&event_found, 0, sizeof(dnx_oam_event_and_rmep_info));
    sal_memset(&interrupt_message, 0, sizeof(dnx_oam_interrupt_data_t));

    oam_use_event_fifo_dma = dnx_data_oam.oamp.oamp_fifo_dma_event_interface_enable_get(unit);

    /*
     * In the first run we allocate the event list and hash table.
     * In the last run we process the events as well as free the dynamic memory.
     * Non-DMA has only one run, so those are both first and last.
     */
    is_first = (!oam_use_event_fifo_dma || entry_number == 1);
    is_last = (!oam_use_event_fifo_dma || entry_number == total_amount_of_entries);

    if (is_first)
    {

        SHR_ALLOC_SET_ZERO(int_data, sizeof(dma_interrupt_global_data_structures),
                           "OAM interrupt data bases", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        SHR_ALLOC_SET_ZERO(int_data->event_list, sizeof(dnx_oam_oamp_eventt_list_t),
                           "event list in oamp interrupt", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
        /*
         * Create event_list
         */
        dnx_oam_oamp_eventt_list_init(unit, int_data->event_list);

        SHR_IF_ERR_EXIT(shr_htb_create(&int_data->event_htbl,
                                       DNX_OAM_OAMP_FIFO_MAX_NUMBER_OF_EVENTS_IN_SW,
                                       sizeof(dnx_oam_event_and_rmep_info), "OAM event hash table."));
        if (oam_use_event_fifo_dma)
        {
            *user_data = int_data;
        }
    }
    else
    {
        /*
         * Already allocated in previous runs in the current interrupt.
         */
        int_data = (dma_interrupt_global_data_structures *) * user_data;
    }

    SHR_IF_ERR_EXIT(dnx_oam_oamp_event_fifo_read(unit, &valid_event_exist, &interrupt_message, host_entry));

    while (valid_event_exist)
    {
        for (event_index = 0; event_index < DNX_OAM_OAMP_MAX_JR2_FORMAT_EVENTS; event_index++)
        {
            event_data.interrupt_entry.jr2_event_message = interrupt_message.interrupt_data.jr2_event_data[event_index];
            /** If not valid no need to put in SW database */
            if (event_data.interrupt_entry.jr2_event_message.is_valid)
            {
                rv = dnx_oam_oamp_event_update_sw(unit, 0 /** event interrupt*/ , int_data->event_htbl,
                                                  int_data->event_list, &event_data, &event_found);
                if (rv != _SHR_E_NONE)
                {
                    LOG_WARN(BSL_LS_BCM_OAM, (BSL_META_U(unit, "Warning: Error in updating event SW data.\n")));
                    continue;
                }
            }
        }
        /*
         * Bailout condition - Preventing the while loop to run forever in case of interrupts arriving at a very high
         * rate
         */
        if (++fifo_read_counter >= DNX_OAM_OAMP_FIFO_MAX_NUMBER_OF_EVENTS_IN_SW)
        {
            LOG_WARN(BSL_LS_BCM_OAM, (BSL_META_U(unit, "Warning: Too many OAMP interrupts .\n")));
            break;
        }

        valid_event_exist = 0;
        if (!oam_use_event_fifo_dma)
        {
            sal_memset(&interrupt_message, 0, sizeof(dnx_oam_interrupt_data_t));
            SHR_IF_ERR_EXIT(dnx_oam_oamp_event_fifo_read(unit, &valid_event_exist, &interrupt_message, host_entry));
        }
    }

    /*
     * Empty the data bases where events were stored and call user callbacks.
     */
    if (is_last)
    {
        /*
         * For complete implementation details see comment above dnx_oam_oamp_eventt_Node_t
         */
        rv = dnx_oam_oamp_eventt_list_pop(unit, int_data->event_list, &event_found);
        while (rv != _SHR_E_EMPTY)
        {
            SHR_IF_ERR_EXIT(shr_htb_find(int_data->event_htbl, &event_found, &data, 0 /* don't remove */ ));
            SHR_IF_ERR_EXIT(dnx_oam_oamp_fifo_interrupt_handle_user_callback(unit, &event_found, data));
            rv = dnx_oam_oamp_eventt_list_pop(unit, int_data->event_list, &event_found);
        }
    }
exit:
    if (is_last)
    {
        if (int_data->event_htbl)
        {
            /*
             * Empty event_htbl in case of error
             */
            rv = dnx_oam_oamp_eventt_list_pop(unit, int_data->event_list, &event_found);
            while (rv != BCM_E_EMPTY)
            {
                SHR_IF_ERR_EXIT(shr_htb_find(int_data->event_htbl, &event_found, &data, 1 /* remove */ ));
                rv = dnx_oam_oamp_eventt_list_pop(unit, int_data->event_list, &event_found);
            }
            SHR_IF_ERR_EXIT_WITH_LOG(shr_htb_destroy(&int_data->event_htbl, NULL), "shr_htb_destroy failed\n%s%s%s",
                                     EMPTY, EMPTY, EMPTY);
        }
        SHR_FREE(int_data->event_list);
        SHR_FREE(int_data);
    }
    SHR_FUNC_EXIT;
}

/**
 * See oam_oamp.h
 */
void
dnx_oam_oamp_dma_event_handler(
    int unit,
    void *entry,
    int entry_size,
    int entry_number,
    int total_amount_of_entries,
    void **user_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_oam_fifo_interrupt_handler(unit, entry, entry_number, total_amount_of_entries, user_data));

exit:
    SHR_VOID_FUNC_EXIT;
}

/**
 * See oam_internal.h
 */
void
dnx_oam_oamp_event_handler(
    int unit)
{
    dnx_oam_interrupt_data_t interrupt_message;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&interrupt_message, 0, sizeof(dnx_oam_interrupt_data_t));

    /** DMA event type is dummy in case of normal event interrupts */
    SHR_IF_ERR_EXIT(dnx_oam_fifo_interrupt_handler(unit, NULL, 0, 0, NULL));

exit:
    SHR_VOID_FUNC_EXIT;
}

/**
 * See oam_oamp.h
 */
shr_error_e
dnx_oam_oamp_event_dma_init(
    int unit)
{
    int dma_channel;
    soc_dnx_fifodma_config_t fifodma_info;
    uint32 host_memory_size;
    uint32 entry_size;
    int is_started;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * if we are during warmboot the is_started may be incorrect, it
     * may have been turned off during init, but this information
     * was not synced to the external storage and this info was
     * overwritten with incorrect info during warmboot recovery
     */
    if (!sw_state_is_warm_boot(unit))
    {
        /** Stop and free the channel if it is already started */
        SHR_IF_ERR_EXIT(soc_dnx_fifodma_channel_is_started(unit, soc_dnx_fifodma_src_oam_event, &is_started));

        if (is_started)
        {
            SHR_IF_ERR_EXIT(soc_dnx_fifodma_channel_get(unit, soc_dnx_fifodma_src_oam_event, &dma_channel));
            SHR_IF_ERR_EXIT(soc_dnx_fifodma_channel_stop(unit, dma_channel));
            SHR_IF_ERR_EXIT(soc_dnx_fifodma_channel_free(unit, dma_channel));
        }
    }

    SHR_IF_ERR_EXIT(soc_dnx_fifodma_channel_alloc(unit, soc_dnx_fifodma_src_oam_event, &dma_channel));
    host_memory_size = dnx_data_oam.oamp.oamp_fifo_dma_event_interface_buffer_size_get(unit);

    /** In case no host memory was defined do not start the DMA */
    if (host_memory_size == 0)
    {
        SHR_EXIT();
    }

    entry_size = soc_reg_bytes(unit, OAMP_INTERRUPT_MESSAGEr);

    SHR_IF_ERR_EXIT(soc_dnx_fifodma_config_t_init(unit, &fifodma_info));
    fifodma_info.copyno = REG_PORT_ANY;
    fifodma_info.force_entry_size = 0;
    fifodma_info.is_mem = 0;
    fifodma_info.is_poll = 0;
    /** Round down the number of entries to the closest power of 2 */
    fifodma_info.max_entries = utilex_power_of_2(utilex_log2_round_down(host_memory_size / entry_size));
    fifodma_info.reg = OAMP_INTERRUPT_MESSAGEr;
    fifodma_info.threshold = dnx_data_oam.oamp.oamp_fifo_dma_event_interface_threshold_get(unit);
    fifodma_info.timeout = dnx_data_oam.oamp.oamp_fifo_dma_event_interface_timeout_get(unit);

    SHR_IF_ERR_EXIT(soc_dnx_fifodma_channel_start(unit, dma_channel, &fifodma_info, "dnx_oam_oamp_dma_event_handler"));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See oam_oamp.h
 */
shr_error_e
dnx_oam_oamp_event_dma_deinit(
    int unit)
{
    int oamp_channel;
    int is_started;

    SHR_FUNC_INIT_VARS(unit);

   /** Allow writing to registers and changing SW state for the DMA init time */
    SHR_IF_ERR_CONT(dnxc_ha_tmp_allow_access_enable(unit, UTILEX_SEQ_ALLOW_SCHAN));
    SHR_IF_ERR_CONT(dnxc_ha_tmp_allow_access_enable(unit, UTILEX_SEQ_ALLOW_SW_STATE));

    /** Stop and free the channel if it is already started */
    SHR_IF_ERR_CONT(soc_dnx_fifodma_channel_is_started(unit, soc_dnx_fifodma_src_oam_event, &is_started));

    if (is_started)
    {
        SHR_IF_ERR_CONT(soc_dnx_fifodma_channel_get(unit, soc_dnx_fifodma_src_oam_event, &oamp_channel));

        SHR_IF_ERR_CONT(soc_dnx_fifodma_channel_free(unit, oamp_channel));
    }
    /** Return to warmboot normal mode (re-enable verifications) */
    SHR_IF_ERR_CONT(dnxc_ha_tmp_allow_access_disable(unit, UTILEX_SEQ_ALLOW_SW_STATE));
    SHR_IF_ERR_CONT(dnxc_ha_tmp_allow_access_disable(unit, UTILEX_SEQ_ALLOW_SCHAN));

    SHR_FUNC_EXIT;
}

/**
 * See oam_oamp.h
 */
shr_error_e
dnx_oam_oamp_stat_dma_init(
    int unit)
{
    int dma_channel;
    soc_dnx_fifodma_config_t fifodma_info;
    uint32 host_memory_size;
    uint32 entry_size;
    int is_started;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * if we are during warmboot the is_started may be incorrect, it
     * may have been turned off during init, but this information
     * was not synced to the external storage and this info was
     * overwritten with incorrect info during warmboot recovery
     */
    if (!sw_state_is_warm_boot(unit))
    {
        /** Stop and free the channel if it is already started */
        SHR_IF_ERR_EXIT(soc_dnx_fifodma_channel_is_started(unit, soc_dnx_fifodma_src_oam_status, &is_started));

        if (is_started)
        {
            SHR_IF_ERR_EXIT(soc_dnx_fifodma_channel_get(unit, soc_dnx_fifodma_src_oam_status, &dma_channel));
            SHR_IF_ERR_EXIT(soc_dnx_fifodma_channel_stop(unit, dma_channel));
            SHR_IF_ERR_EXIT(soc_dnx_fifodma_channel_free(unit, dma_channel));
        }
    }

    SHR_IF_ERR_EXIT(soc_dnx_fifodma_channel_alloc(unit, soc_dnx_fifodma_src_oam_status, &dma_channel));
    host_memory_size = dnx_data_oam.oamp.oamp_fifo_dma_report_interface_buffer_size_get(unit);

    /** In case no host memory was defined do not start the DMA */
    if (host_memory_size == 0)
    {
        SHR_EXIT();
    }

    entry_size = soc_reg_bytes(unit, OAMP_STAT_INTERRUPT_MESSAGEr);

    SHR_IF_ERR_EXIT(soc_dnx_fifodma_config_t_init(unit, &fifodma_info));
    fifodma_info.copyno = REG_PORT_ANY;
    fifodma_info.force_entry_size = 0;
    fifodma_info.is_mem = 0;
    fifodma_info.is_poll = 0;
    /** Round down the number of entries to the closest power of 2 */
    fifodma_info.max_entries = utilex_power_of_2(utilex_log2_round_down(host_memory_size / entry_size));
    fifodma_info.reg = OAMP_STAT_INTERRUPT_MESSAGEr;
    fifodma_info.threshold = dnx_data_oam.oamp.oamp_fifo_dma_report_interface_threshold_get(unit);
    fifodma_info.timeout = dnx_data_oam.oamp.oamp_fifo_dma_report_interface_timeout_get(unit);

    SHR_IF_ERR_EXIT(soc_dnx_fifodma_channel_start
                    (unit, dma_channel, &fifodma_info, "dnx_oam_oamp_stat_dma_event_handler"));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See oam_oamp.h
 */
shr_error_e
dnx_oam_oamp_stat_dma_deinit(
    int unit)
{
    int oamp_channel;
    int is_started;

    SHR_FUNC_INIT_VARS(unit);

   /** Allow writing to registers and changing SW state for the DMA init time */
    SHR_IF_ERR_CONT(dnxc_ha_tmp_allow_access_enable(unit, UTILEX_SEQ_ALLOW_SCHAN));
    SHR_IF_ERR_CONT(dnxc_ha_tmp_allow_access_enable(unit, UTILEX_SEQ_ALLOW_SW_STATE));

    /** Stop and free the channel if it is already started */
    SHR_IF_ERR_CONT(soc_dnx_fifodma_channel_is_started(unit, soc_dnx_fifodma_src_oam_status, &is_started));

    if (is_started)
    {
        SHR_IF_ERR_CONT(soc_dnx_fifodma_channel_get(unit, soc_dnx_fifodma_src_oam_status, &oamp_channel));

        SHR_IF_ERR_CONT(soc_dnx_fifodma_channel_free(unit, oamp_channel));
    }
    /** Return to warmboot normal mode (re-enable verifications) */
    SHR_IF_ERR_CONT(dnxc_ha_tmp_allow_access_disable(unit, UTILEX_SEQ_ALLOW_SW_STATE));
    SHR_IF_ERR_CONT(dnxc_ha_tmp_allow_access_disable(unit, UTILEX_SEQ_ALLOW_SCHAN));

    SHR_FUNC_EXIT;
}

/*SAT report handle*/
shr_error_e
dnx_oam_oamp_sat_ctf_report_process(
    int unit,
    dnx_sat_event_data_t * event)
{
    bcm_sat_report_event_data_t report;
    bcm_sat_event_type_t type = bcmSATEventReport;
    SHR_FUNC_INIT_VARS(unit);

    if (!event || (!event->is_valid))
    {
        SHR_IF_ERR_EXIT(BCM_E_PARAM);
    }
    sal_memset(&report, 0, sizeof(bcm_sat_report_event_data_t));

    report.ctf_id = event->flow_id;
    if (event->d_set)
    {
        report.delay = event->frame_delay;
        report.delay_valid = 1;
    }

    if (event->s_set)
    {
        report.sequence_number = event->seq_num;
        report.sequence_number_valid = 1;
    }

    if (_g_dnx_sat_event_cb[unit][type])
    {
        _g_dnx_sat_event_cb[unit][type] (unit, type, (void *) &report, _g_dnx_sat_event_ud[unit][type]);
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function gets the OAMP interrupt data in jr2 event format.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] is_dma_enabled - DMA enabled or not.
 * \param [in,out] valid_event_exist - pointer to event valid existence indication.
 * \param [out] event - Event data information.
 * \param [out] event_type - Report or Sat event.
 * \param [in] host_entry - Host entry in case of DMA.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   This function gets the interrupt data from OAMP_STAT_INTERRUPT_MESSAGE register.
 *
 * \see
 *   * None
 */
shr_error_e
dnx_oam_oamp_stat_event_data_get(
    int unit,
    uint8 is_dma_enabled,
    dnx_oam_interrupt_data_t * event,
    uint8 *valid_event_exist,
    uint8 *event_type,
    void *host_entry)
{
    uint32 entry_handle_id = 0;
    int instance = 0;
    uint32 report_index = 0;
    uint32 stat_event_data[DNX_SAT_MAX_JR2_SAT_EVENTS] = { 0 };
    uint32 frame_delay_msb = 0;
    uint32 frame_delay_lsb = 0;
    uint64 frame_delay;
    uint32 seq_num = 0;
    uint32 event_idx = 0;
    int field_index = 0;
    int dma_register_size_in_bits = 0;
    dnx_sat_event_data_t sat_event;
    uint8 data_index = 0;
    uint8 instance_index = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    *valid_event_exist = 0;
    COMPILER_64_ZERO(frame_delay);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_INTERRUPT_REPORT, &entry_handle_id));
    if (!is_dma_enabled)
    {
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_tables_payload_size_get
                        (unit, DBAL_TABLE_OAMP_INTERRUPT_REPORT, 0, &dma_register_size_in_bits));
        /*
         * Update DBAL handle with the event's details including payload and key
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_payload_update
                        (unit, entry_handle_id, NULL, NULL, _SHR_CORE_ALL, dma_register_size_in_bits, host_entry));
    }

    /** Read DBAL to get the interrupt message data from OAMP */
    for (instance_index = 0; instance_index < DNX_SAT_MAX_JR2_SAT_EVENTS; instance_index++)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_JR2_STAT_EVENT, instance_index,
                         (uint32 *) (stat_event_data + instance_index)));
    }

    /** The first event is going to show what kind of events are stored in the interrupt register for this unit */
    *event_type = DNX_OAM_OAMP_STAT_EVENT_TYPE(stat_event_data[0]);
    /** It's possible to have report and sat event in the STAT FIFO, so check the first event to check which stat event is called */
    if (*event_type == DNX_OAM_OAMP_SAT_TYPE)
    {
        for (instance = 0; instance < DNX_SAT_MAX_JR2_SAT_EVENTS; instance++)
        {
            sat_event.event_type = DNX_OAM_OAMP_STAT_EVENT_TYPE(stat_event_data[instance]);
            /** In case of invalid event or wrong event continue */
            if ((sat_event.event_type != DNX_OAM_OAMP_SAT_TYPE) || (stat_event_data[instance] == 0xffffffff))
            {
                continue;
            }
            /** Update SW structure with OAMP event data */
            sal_memset(&sat_event, 0, sizeof(dnx_sat_event_data_t));

            *valid_event_exist = 1;
            sat_event.is_valid = 1;
            sat_event.flow_id = DNX_OAM_OAMP_SAT_FLOW_ID_EVENT(stat_event_data[instance]);
            sat_event.d_set = DNX_OAM_OAMP_SAT_DELAY_SET(stat_event_data[instance]);
            sat_event.s_set = DNX_OAM_OAMP_SAT_SEQ_NUM_SET(stat_event_data[instance]);
            sat_event.size = DNX_OAM_OAMP_SAT_SIZE(stat_event_data[instance]);
            frame_delay_msb = DNX_OAM_OAMP_SAT_FRAME_DEALY_MSB(stat_event_data[instance]);
            /*
             * To get delay
             */
            if (sat_event.d_set)
            {
                field_index = instance + 1;
                if (field_index < DNX_SAT_MAX_JR2_SAT_EVENTS)
                {
                    /*
                     * skip to next instance to get frame_delay_lsb
                     */
                    frame_delay_lsb = DNX_OAM_OAMP_SAT_FRAME_DEALY_LSB(stat_event_data[field_index]);
                    /*
                     * Frame-Delay [40:32] + Frame-Delay [31:0]
                     */
                    COMPILER_64_SET(frame_delay, frame_delay_msb, frame_delay_lsb);
                    instance++;
                }
            }

            sat_event.frame_delay = COMPILER_64_LO(frame_delay);
            /*
             * To get seq_no
             */
            if (sat_event.s_set)
            {
                field_index = instance + 1;
                if (field_index < DNX_SAT_MAX_JR2_SAT_EVENTS)
                {
                    /*
                     * skip to next instance to get seq_num
                     */
                    seq_num = stat_event_data[field_index];
                    instance++;
                }
            }

            sat_event.seq_num = seq_num;

            /** Pass the event data to SW and process it */
            SHR_IF_ERR_EXIT(dnx_oam_oamp_sat_ctf_report_process(unit, &sat_event));

            event_idx++;
        }
    }
    else if (*event_type == DNX_OAM_OAMP_REPORT_TYPE)
    {
        /*
         * Data pointer always point to the beginning of event Statistic event is 32bits + event size * 32bits
         */
        for (instance = 0; instance < DNX_SAT_MAX_JR2_SAT_EVENTS; instance++)
        {
            /** In case of invalid event or wrong event continue */
            if ((DNX_OAM_OAMP_STAT_EVENT_TYPE(stat_event_data[instance]) != DNX_OAM_OAMP_REPORT_TYPE)
                || (stat_event_data[instance] == 0xffffffff))
            {
                continue;
            }

            *valid_event_exist = 1;
            event->interrupt_data.jr2_report_data[report_index].is_valid = 1;
            event->interrupt_data.jr2_report_data[report_index].mep_id =
                DNX_OAM_OAMP_REPORT_MEP_ID(stat_event_data[instance]);
            event->interrupt_data.jr2_report_data[report_index].event_size =
                DNX_OAM_OAMP_REPORT_EVENT_SIZE(stat_event_data[instance]);
            event->interrupt_data.jr2_report_data[report_index].mep_event_type =
                DNX_OAM_OAMP_REPORT_MEP_TYPE(stat_event_data[instance]);
            /**
            * The event size shows the total size of the event in 32b words,
            * where the first word shows the event information, and the rest is the data.
            * We can have the following sizes:
            *   2: 32b(event info) + 32b(data)
            *   3: 32b(event info) + 32b(data) + 32b(data)
            *   5: 32b(event info) + 32b(data) + 32b(data) + 32b(data) + 32b(data)
            *   9: 32b(event info) + 32b(data) + 32b(data) + 32b(data) + 32b(data) + 32b(data) + 32b(data) + 32b(data) + 32b(data)
            */
            for (data_index = 0; data_index < event->interrupt_data.jr2_report_data[report_index].event_size - 1;
                 ++data_index)
            {
                instance++;
                /** Limit the value of instance to be up to 20 */
                if (instance >= DNX_SAT_MAX_JR2_SAT_EVENTS)
                {
                    continue;
                }
                event->interrupt_data.jr2_report_data[report_index].data[data_index] = stat_event_data[instance];
            }
            report_index++;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function reads data from HW interrupt message and update
 *  software events data structure.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] is_dma_enabled - DMA enabled or not.
 * \param [in,out] valid_event_exist - pointer to event valid existence indication.
 * \param [out] event - Event data information.
 * \param [out] event_type - Report or Sat event.
 * \param [in] host_entry - Host entry in case of DMA.
 *
 * \return
 *   shr_error_e
 */
shr_error_e
dnx_oam_oamp_stat_event_fifo_read(
    int unit,
    uint8 is_dma_enabled,
    uint8 *valid_event_exist,
    dnx_oam_interrupt_data_t * event,
    uint8 *event_type,
    void *host_entry)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Use DBAL interface to read interrupt message data and update the interrupt_data structure
     */
    /** We currently only support JR2 format. */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_stat_event_data_get
                    (unit, is_dma_enabled, event, valid_event_exist, event_type, host_entry));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function handles the interrupts generated by OAMP.
 * Should not be called directly, rather through callback in soc
 * layer.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] host_entry - Host entry in case of DMA.
 *
 * \return
 *   Error value
 */
shr_error_e
dnx_oam_oamp_stat_fifo_interrupt_handler(
    int unit,
    void *host_entry)
{
    uint8 valid_event_exist = 0;
    int fifo_read_counter = 0;
    uint8 use_event_fifo_dma = 0;
    int rv = 0;
    uint32 event_index = 0;
    dnx_oam_event_and_rmep_info event_found;
    dnx_oam_interrupt_data_t report_data;
    shr_htb_data_t data;
    uint8 event_type = 0;
    uint8 empty = 0;
    shr_htb_hash_table_t event_htbl = NULL;
    dnx_oam_oamp_eventt_list_t event_list;
    dnx_oam_interrupt_data_t interrupt_message;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&event_found, 0, sizeof(dnx_oam_event_and_rmep_info));
    sal_memset(&interrupt_message, 0, sizeof(dnx_oam_interrupt_data_t));
    sal_memset(&report_data, 0, sizeof(dnx_oam_interrupt_data_t));

    /** Check if DMA interface is enabled */
    use_event_fifo_dma = dnx_data_oam.oamp.oamp_fifo_dma_report_interface_enable_get(unit);
    /*
     * Create event_list
     */
    dnx_oam_oamp_eventt_list_init(unit, &event_list);

    /** Read the interrupt register(Stat FIFO HEAD) and determine the event type */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_stat_event_fifo_read
                    (unit, use_event_fifo_dma, &valid_event_exist, &interrupt_message, &event_type, host_entry));

    SHR_IF_ERR_EXIT(shr_htb_create(&event_htbl,
                                   DNX_OAM_OAMP_FIFO_MAX_NUMBER_OF_EVENTS_IN_SW,
                                   sizeof(dnx_oam_event_and_rmep_info), "OAM event hash table."));

    /** Read the FIFO until empty head is not empty
     */
    while (valid_event_exist)
    {
        /**
        * Stat interrupt register can hold SAT and REPORT event.
        * In case of SAT, the events are process one by one.
        * In case of REPORT, the events are stored in hash table and then processed.
        */
        if (event_type == DNX_OAM_OAMP_REPORT_TYPE)
        {
            for (event_index = 0; event_index < DNX_OAM_OAMP_MAX_JR2_FORMAT_REPORT; event_index++)
            {
                report_data.interrupt_entry.jr2_report_message =
                    interrupt_message.interrupt_data.jr2_report_data[event_index];
                /** If not valid no need to put in SW database */
                if (report_data.interrupt_entry.jr2_report_message.is_valid)
                {
                    rv = dnx_oam_oamp_event_update_sw(unit, 1 /** report interrupt*/ , event_htbl, &event_list,
                                                      &report_data, &event_found);
                    if (rv != _SHR_E_NONE)
                    {
                        LOG_WARN(BSL_LS_BCM_OAM, (BSL_META_U(unit, "Warning: Error in updating event SW data.\n")));
                        continue;
                    }
                }
            }
        }

        /*
         * Bailout condition - Preventing the while loop to run forever in case of interrupts arriving at a very high
         * rate
         */
        if (++fifo_read_counter >= DNX_SAT_FIFO_MAX_NUMBER_OF_EVENTS_IN_SW)
        {
            LOG_WARN(BSL_LS_BCM_SAT, (BSL_META_U(unit, "Warning: Too many SAT interrupts .\n")));
            break;
        }

        valid_event_exist = 0;
        if (!use_event_fifo_dma)
        {
            sal_memset(&interrupt_message, 0, sizeof(dnx_oam_interrupt_data_t));
            SHR_IF_ERR_EXIT(dnx_oam_oamp_stat_event_fifo_read
                            (unit, use_event_fifo_dma, &valid_event_exist, &interrupt_message, &empty, host_entry));
        }
    }

    if (event_type == DNX_OAM_OAMP_REPORT_TYPE)
    {
        /*
         * Empty the data bases where events were stored and call user callbacks.
         */
        /*
         * For complete implementation details see comment above dnx_oam_oamp_eventt_Node_t
         */
        rv = dnx_oam_oamp_eventt_list_pop(unit, &event_list, &event_found);
        while (rv != _SHR_E_EMPTY)
        {
            SHR_IF_ERR_EXIT(shr_htb_find(event_htbl, &event_found, &data, 0 /* don't remove */ ));
            SHR_IF_ERR_EXIT(dnx_oam_oamp_fifo_interrupt_handle_user_callback(unit, &event_found, data));
            rv = dnx_oam_oamp_eventt_list_pop(unit, &event_list, &event_found);
        }
    }

exit:
    if (event_htbl)
    {
        /*
         * Empty event_htbl in case of error
         */
        rv = dnx_oam_oamp_eventt_list_pop(unit, &event_list, &event_found);
        while (rv != BCM_E_EMPTY)
        {
            SHR_IF_ERR_EXIT(shr_htb_find(event_htbl, &event_found, &data, 1 /* remove */ ));
            rv = dnx_oam_oamp_eventt_list_pop(unit, &event_list, &event_found);
        }
        SHR_IF_ERR_EXIT_WITH_LOG(shr_htb_destroy(&event_htbl, NULL), "shr_htb_destroy failed\n%s%s%s", EMPTY, EMPTY,
                                 EMPTY);
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief - This is the call back function used in sal_dpc call.
 *
 * \param [in] unit - pointer to hardware unit used.
 * \return
 *   void
 */
void
dnx_oam_oamp_stat_event_handler(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_oam_oamp_stat_fifo_interrupt_handler(unit, NULL));
exit:
    SHR_VOID_FUNC_EXIT;
}

/**
 * See oam_oamp.h
 */
void
dnx_oam_oamp_stat_dma_event_handler(
    int unit,
    void *entry,
    int entry_size,
    int entry_number,
    int total_amount_of_entries,
    void **user_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_oam_oamp_stat_fifo_interrupt_handler(unit, entry));
exit:
    SHR_VOID_FUNC_EXIT;
}
