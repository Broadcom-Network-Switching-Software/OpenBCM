/** \file oam_oamp_v1_interrupts.c
 * 
 *
 * OAMP Interrupt handling procedures for DNX version 1.
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_OAM
#ifdef BCM_DNX2_SUPPORT
 /*
  * Include files.
  * {
  */
#include <soc/sand/shrextend/shrextend_debug.h>
#include <src/bcm/dnx/oam/oamp_v1/oam_oamp_v1_interrupts.h>
#include <src/bcm/dnx/oam/oam_internal.h>
#include <src/bcm/dnx/bfd/bfd_internal.h>

/*
 * }
 */

/*
 * MACROs
 * {
 */
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

/*
 * }
 */

/*
 * Inner functions
 * {
 */

/*
 * See prototype definition for function description in .h file
 */
shr_error_e
dnx_oam_oamp_v1_interrupt_event_data_get(
    int unit,
    uint8 is_dma_enabled,
    dnx_oam_oamp_interrupt_event_data_t * event,
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
    uint32 max_format_events;

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

        if (dnx_data_oam.oamp.event_fifo_slicing_size_offset_get(unit) != 0)
        {
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
        }
        /*
         * Update DBAL handle with the event's details including payload and key
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_payload_update
                        (unit, entry_handle_id, NULL, NULL, _SHR_CORE_ALL, dma_register_size_in_bits, host_entry));

    }
    max_format_events = dnx_data_oam.oamp.max_number_of_events_get(unit);
    for (instance = 0; instance < max_format_events; instance++)
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
        event[instance].rmep_db_ptr = rmep_db_ptr;
        sal_memset(&sw_rmep_info, 0, sizeof(sw_rmep_info));
        rv = dnx_oam_sw_state_remote_endpoint_info_get(unit, rmep_db_ptr, &sw_rmep_info);
        if (rv != _SHR_E_NONE)
        {
            /** Don't process the event if SW does not have the EP context */
            /** Update the entry as invalid */
            event[instance].is_valid = 0;
            continue;
        }
        event[instance].loc_evt = loc_evt;
        event[instance].rdi_set = rdi_set;
        event[instance].rdi_clear = rdi_clear;
        event[instance].rmep_state_change = rmep_state_change;
        event[instance].is_valid = !reserved_1;
        /** Update valid entry existence to the caller */
        if (event[instance].is_valid)
        {
            *valid_event_exist = 1;
        }
        /** Using the RMEP name to identify if it is BFD or OAM RMEP */
        if (DNX_BFD_RMEP_SW_INFO_NAME_IS_BFD(sw_rmep_info.rmep_id))
        {   /** Update RMEP state */
            event[instance].rmep_state.bfd_state.flags = DNX_OAM_BFD_RMEP_STATE_FLAGS_PROF_GET(rmep_state);
            event[instance].rmep_state.bfd_state.state = DNX_OAM_BFD_RMEP_STATE_STATE_GET(rmep_state);
            event[instance].rmep_state.bfd_state.diag_profile = DNX_OAM_BFD_RMEP_STATE_DIAG_PROF_GET(rmep_state);
            event[instance].rmep_state.bfd_state.detect_multiplier = DNX_OAM_BFD_RMEP_STATE_DETECT_MULT_GET(rmep_state);
            event[instance].is_oam = 0;
        }
        else
        {   /** Update RMEP state */
            event[instance].rmep_state.eth_state.interface_status = DNX_OAM_BFD_RMEP_STATE_INTF_STATUS_GET(rmep_state);
            event[instance].rmep_state.eth_state.port_status = DNX_OAM_BFD_RMEP_STATE_PORT_STATUS_GET(rmep_state);
            event[instance].is_oam = 1;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

#endif
