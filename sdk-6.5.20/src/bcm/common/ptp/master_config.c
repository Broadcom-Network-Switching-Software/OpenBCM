/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    master_config.c
 *
 * Purpose: 
 *
 * Functions:
 *      bcm_common_ptp_static_unicast_master_add
 *      bcm_common_ptp_static_unicast_master_list
 *      bcm_common_ptp_static_unicast_master_remove
 *      bcm_common_ptp_static_unicast_master_table_clear
 *      bcm_common_ptp_static_unicast_master_table_size_get
 */

#if defined(INCLUDE_PTP)

#include <soc/defs.h>
#include <soc/drv.h>

#include <bcm/ptp.h>
#include <bcm_int/common/ptp.h>
#include <bcm/error.h>
#include <bcm_int/ptp_common.h>

/* PTP clock unicast master table. */
typedef struct _bcm_ptp_unicast_master_table_s {
    uint16 num_entries;
    bcm_ptp_clock_unicast_master_t entry[PTP_MAX_UNICAST_MASTER_TABLE_ENTRIES];
} _bcm_ptp_unicast_master_table_t;

/* PTP stack unicast master table array. */
typedef struct _bcm_ptp_stack_ucm_array_s {
    _bcm_ptp_memstate_t memstate;
    _bcm_ptp_unicast_master_table_t *table;
} _bcm_ptp_stack_ucm_array_t;

/* Unit unicast master table array(s). */
typedef struct _bcm_ptp_unit_ucm_array_s {
    _bcm_ptp_memstate_t memstate;
    _bcm_ptp_stack_ucm_array_t *stack_array;
} _bcm_ptp_unit_ucm_array_t;

static const bcm_ptp_clock_unicast_master_t ucm_entry_default;
static const _bcm_ptp_unicast_master_table_t ucm_default;
static _bcm_ptp_unit_ucm_array_t unit_ucm_array[BCM_MAX_NUM_UNITS];

static int _bcm_ptp_unicast_master_table_set(int unit, bcm_ptp_stack_id_t ptp_id, int clock_num);

/*
 * Function:
 *      _bcm_ptp_unicast_master_table_init
 * Purpose:
 *      Initialize the unicast master table arrays of a unit.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_ptp_unicast_master_table_init(
    int unit)
{
    int rv = BCM_E_UNAVAIL;
    int i;

    _bcm_ptp_stack_ucm_array_t *stack_p;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, PTP_STACK_ID_DEFAULT,
            PTP_CLOCK_NUMBER_DEFAULT, PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        return rv;
    }
    if (NULL == unit_ucm_array[unit].stack_array) {
        stack_p = sal_alloc(PTP_MAX_STACKS_PER_UNIT*
                            sizeof(_bcm_ptp_stack_ucm_array_t),"Unit UCM arrays");
    } else {
        stack_p = unit_ucm_array[unit].stack_array;
    }
    
    if (!stack_p) {
        unit_ucm_array[unit].memstate = PTP_MEMSTATE_FAILURE;
        return BCM_E_MEMORY;
    }

    unit_ucm_array[unit].stack_array = stack_p;
    unit_ucm_array[unit].memstate = PTP_MEMSTATE_INITIALIZED;

    for (i = 0; i < PTP_MAX_STACKS_PER_UNIT; ++i) {
        unit_ucm_array[unit].stack_array[i].memstate = PTP_MEMSTATE_STARTUP;
    }

    return rv;
}

/*
 * Function:
 *      _bcm_ptp_unicast_master_table_init_detach
 * Purpose:
 *      Shut down the unicast master table arrays of a unit.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_ptp_unicast_master_table_detach(
    int unit)
{
    int i;

    if(unit_ucm_array[unit].stack_array) {
        for (i = 0; i < PTP_MAX_STACKS_PER_UNIT; ++i) {
            _bcm_ptp_unicast_master_table_stack_destroy(unit, i);
        }

        unit_ucm_array[unit].memstate = PTP_MEMSTATE_STARTUP;
        sal_free(unit_ucm_array[unit].stack_array);
        unit_ucm_array[unit].stack_array = NULL;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_ptp_unicast_master_table_stack_create
 * Purpose:
 *      Create the unicast master table array of a PTP stack.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      ptp_id  - (IN) PTP stack ID.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_ptp_unicast_master_table_stack_create(
    int unit,
    bcm_ptp_stack_id_t ptp_id)
{
    int rv = BCM_E_UNAVAIL;

    _bcm_ptp_unicast_master_table_t *table_p;
    int i = 0;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id,
            PTP_CLOCK_NUMBER_DEFAULT, PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        return rv;
    }

    if (unit_ucm_array[unit].memstate != PTP_MEMSTATE_INITIALIZED) {
        return BCM_E_UNAVAIL;
    }

    if (unit_ucm_array[unit].stack_array[ptp_id].memstate == PTP_MEMSTATE_INITIALIZED) {
        return BCM_E_EXISTS;
    }

    table_p = sal_alloc(PTP_MAX_CLOCK_INSTANCES*
                        sizeof(_bcm_ptp_unicast_master_table_t),
                        "PTP stack UCM array");

    if (!table_p) {
            unit_ucm_array[unit].stack_array[ptp_id].memstate =
            PTP_MEMSTATE_FAILURE;
        return BCM_E_MEMORY;
    }

    unit_ucm_array[unit].stack_array[ptp_id].table = table_p;
    unit_ucm_array[unit].stack_array[ptp_id].memstate =
        PTP_MEMSTATE_INITIALIZED;

    /* Set number of entries. */
    for (i = 0; i < PTP_MAX_CLOCK_INSTANCES; ++i) {
        unit_ucm_array[unit].stack_array[ptp_id].table[i].num_entries = 0;
    }

    return rv;
}

/*
 * Function:
 *      _bcm_ptp_unicast_master_table_stack_destroy
 * Purpose:
 *      Destroy the unicast master table array of a PTP stack.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      ptp_id  - (IN) PTP stack ID.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_ptp_unicast_master_table_stack_destroy(
    int unit,
    bcm_ptp_stack_id_t ptp_id)
{
    int rv = BCM_E_NONE;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id,
            PTP_CLOCK_NUMBER_DEFAULT, PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        return rv;
    }

    if (unit_ucm_array[unit].memstate != PTP_MEMSTATE_INITIALIZED) {
        return BCM_E_UNAVAIL;
    }

    if (unit_ucm_array[unit].stack_array[ptp_id].memstate == PTP_MEMSTATE_INITIALIZED) {
        unit_ucm_array[unit].stack_array[ptp_id].memstate =
            PTP_MEMSTATE_STARTUP;
        sal_free(unit_ucm_array[unit].stack_array[ptp_id].table);
        unit_ucm_array[unit].stack_array[ptp_id].table = 0;
    }

    return rv;
}

/*
 * Function:
 *      _bcm_ptp_unicast_master_table_clock_create
 * Purpose:
 *      Create the unicast master table array of a PTP clock.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      ptp_id    - (IN) PTP stack ID.
 *      clock_num - (IN) PTP clock number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
_bcm_ptp_unicast_master_table_clock_create(
    int unit, 
    bcm_ptp_stack_id_t ptp_id,
    int clock_num)
{
    int rv = BCM_E_UNAVAIL;
         
    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num, 
            PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        return rv;   
    }
        
    if ((unit_ucm_array[unit].memstate != PTP_MEMSTATE_INITIALIZED) ||
            (unit_ucm_array[unit].stack_array[ptp_id].memstate != 
            PTP_MEMSTATE_INITIALIZED)) {
        return BCM_E_UNAVAIL;
    }

    unit_ucm_array[unit].stack_array[ptp_id].table[clock_num] = ucm_default;
    
    return rv;
}

/*
 * Function:
 *      bcm_common_ptp_static_unicast_master_table_size_get
 * Purpose:
 *      Get maximum number of unicast master table entries of a PTP clock.
 * Parameters:
 *      unit              - (IN)  Unit number.
 *      ptp_id            - (IN)  PTP stack ID.
 *      clock_num         - (IN)  PTP clock number.
 *      port_num          - (IN)  PTP clock port number.
 *      max_table_entries - (OUT) Maximum number of unicast master table entries.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Ref. IEEE Std. 1588-2008, Chapter 17.5.4.
 */
int 
bcm_common_ptp_static_unicast_master_table_size_get(
    int unit, 
    bcm_ptp_stack_id_t ptp_id, 
    int clock_num, 
    int port_num, 
    int *max_table_entries)
{
    int rv = BCM_E_UNAVAIL;
    
    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num, 
            (uint32)port_num))) {
        return rv;   
    }
        
    *max_table_entries = PTP_MAX_UNICAST_MASTER_TABLE_ENTRIES;
    
    return rv;
}

/*
 * Function:
 *      bcm_common_ptp_static_unicast_master_add
 * Purpose:
 *      Add an entry to the unicast master table of a PTP clock.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      ptp_id      - (IN) PTP stack ID.
 *      clock_num   - (IN) PTP clock number.
 *      port_num    - (IN) PTP clock port number.
 *      master_info - (IN) Unicast master address and granted message intervals.                  
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_common_ptp_static_unicast_master_add(
    int unit, 
    bcm_ptp_stack_id_t ptp_id, 
    int clock_num, 
    int port_num, 
    bcm_ptp_clock_unicast_master_t *master_info)
{
    int rv = BCM_E_UNAVAIL;
    
    _bcm_ptp_unicast_master_table_t *unicast_master_table;
    int i = 0;
    
    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num, 
            (uint32)port_num))) {
        return rv;   
    }
        
    if (master_info == NULL) {
        return BCM_E_PARAM;
    }
    
    if ((unit_ucm_array[unit].memstate != PTP_MEMSTATE_INITIALIZED) ||
            (unit_ucm_array[unit].stack_array[ptp_id].memstate != 
            PTP_MEMSTATE_INITIALIZED)) {
        return BCM_E_UNAVAIL;
    }

    unicast_master_table = &unit_ucm_array[unit].stack_array[ptp_id]
                                                .table[clock_num];

    i = 0;
    while (i < unicast_master_table->num_entries) {
        if (_bcm_ptp_peer_address_compare(&master_info->address,
                &unicast_master_table->entry[i].address)) {
            break;
        }
        ++i;
    }

    if (i >= PTP_MAX_UNICAST_MASTER_TABLE_ENTRIES) {
        return BCM_E_FULL; 
    }

    if (i == unicast_master_table->num_entries) {
        /* New entry. */
        ++unicast_master_table->num_entries;
    }

    unicast_master_table->entry[i].address = 
        master_info->address;
    unicast_master_table->entry[i].log_sync_interval = 
        master_info->log_sync_interval;
    unicast_master_table->entry[i].log_min_delay_request_interval =
        master_info->log_min_delay_request_interval;

    /* Update PTP stack firmware unicast master table. */
    rv = _bcm_ptp_unicast_master_table_set(unit, ptp_id, clock_num);
    
    return rv;
}

/*
 * Function:
 *      bcm_common_ptp_static_unicast_master_list
 * Purpose:
 *      List the unicast master table of a PTP clock.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      ptp_id      - (IN) PTP stack ID.
 *      clock_num   - (IN) PTP clock number.
 *      port_num    - (IN) PTP clock port number.
 *      num_masters - (OUT) Number of unicast master table entries.
 *      master_info - (OUT) Unicast master table.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_common_ptp_static_unicast_master_list(
    int unit,
    bcm_ptp_stack_id_t ptp_id, 
    int clock_num, 
    int port_num, 
    int max_num_masters, 
    int *num_masters, 
    bcm_ptp_clock_peer_address_t *master_info)
{
    int rv = BCM_E_NONE;
    
    _bcm_ptp_unicast_master_table_t *unicast_master_table;
    int i = 0;
    int master_count = 0;
    
    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num, 
            (uint32)port_num))) {
        return rv;   
    }
        
    if ((unit_ucm_array[unit].memstate != PTP_MEMSTATE_INITIALIZED) ||
            (unit_ucm_array[unit].stack_array[ptp_id].memstate != 
            PTP_MEMSTATE_INITIALIZED)) {
        return BCM_E_UNAVAIL;
    }

    unicast_master_table = &unit_ucm_array[unit].stack_array[ptp_id]
                                                .table[clock_num];

    if (unicast_master_table->num_entries == 0) {
        *num_masters = 0;
        return rv;
    }

    
    if (unicast_master_table->num_entries > max_num_masters) {
        *num_masters = 0;
        return BCM_E_RESOURCE;
    }

    /* 
     * Get PTP clock unicast master table.
     * NOTICE: Returns a local, SDK copy of unicast master table 
     *         rather than query PTP stack firmware.
     */
    for (i = 0; i < unicast_master_table->num_entries; ++i) {

        if (port_num == PTP_IEEE1588_ALL_PORTS || _bcm_ptp_signaled_unicast_master_table_search(unit,
                                                            ptp_id, port_num, &unicast_master_table->entry[i].address) == BCM_E_EXISTS) {
            master_info[master_count] = unicast_master_table->entry[i].address;
            master_count++;
        }
    }
    *num_masters = master_count;
    return rv;
}

/*
 * Function:
 *      bcm_common_ptp_static_unicast_master_remove
 * Purpose:
 *      Remove an entry from the unicast master table of a PTP clock.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      ptp_id      - (IN) PTP stack ID.
 *      clock_num   - (IN) PTP clock number.
 *      port_num    - (IN) PTP clock port number.
 *      master_info - (IN) Unicast master address.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_common_ptp_static_unicast_master_remove(
    int unit, 
    bcm_ptp_stack_id_t ptp_id, 
    int clock_num, 
    int port_num, 
    bcm_ptp_clock_peer_address_t *master_info)
{
    int rv = BCM_E_UNAVAIL;
    
    _bcm_ptp_unicast_master_table_t *unicast_master_table;
    int i = 0;
    int k = 0;
    
    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num, 
            (uint32)port_num))) {
        return rv;   
    }
        
    if (master_info == NULL) {
        return BCM_E_PARAM;
    }
    
    if ((unit_ucm_array[unit].memstate != PTP_MEMSTATE_INITIALIZED) ||
            (unit_ucm_array[unit].stack_array[ptp_id].memstate != 
            PTP_MEMSTATE_INITIALIZED)) {
        return BCM_E_UNAVAIL;
    }

    unicast_master_table = &unit_ucm_array[unit].stack_array[ptp_id]
                                                .table[clock_num];

    if (unicast_master_table->num_entries == 0) {
        return BCM_E_EMPTY;
    }

    i = 0; 
    k = 0;
    while (i < unicast_master_table->num_entries) {
        if (_bcm_ptp_peer_address_compare(master_info,
                &unicast_master_table->entry[i].address)) {
            /* 
             * Remove this entry by moving last entry into this slot, and 
             * decrementing number of entries. 
             */
            unicast_master_table->entry[i] = 
                unicast_master_table->entry[--unicast_master_table->num_entries];

            /* Reset unused entry. */
            unicast_master_table->entry[unicast_master_table->num_entries] =
                ucm_entry_default;

            ++k;
        } 
        else 
        {
            ++i;
        }
    }

    if (k <= 0) {
        /* No matching peer address(es) in unicast master table. */
        return BCM_E_NOT_FOUND; 
    }

    /* Update PTP stack firmware unicast master table. */
    rv = _bcm_ptp_unicast_master_table_set(unit, ptp_id, clock_num);
    
    return rv;
}

/*
 * Function:
 *      bcm_common_ptp_static_unicast_master_table_clear
 * Purpose:
 *      Clear (i.e. remove all entries from) the unicast master table 
 *      of a PTP clock.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      ptp_id    - (IN) PTP stack ID.
 *      clock_num - (IN) PTP clock number.
 *      port_num  - (IN) PTP clock port number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_common_ptp_static_unicast_master_table_clear(
    int unit, 
    bcm_ptp_stack_id_t ptp_id, 
    int clock_num, 
    int port_num)
{
    int rv;

    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num,
            (uint32)port_num))) {
        return rv;
    }

    if ((unit_ucm_array[unit].memstate != PTP_MEMSTATE_INITIALIZED) ||
            (unit_ucm_array[unit].stack_array[ptp_id].memstate !=
            PTP_MEMSTATE_INITIALIZED)) {
        return BCM_E_UNAVAIL;
    }

    unit_ucm_array[unit].stack_array[ptp_id].table[clock_num] = ucm_default;

    /* Update PTP stack firmware unicast master table. */
    rv = _bcm_ptp_unicast_master_table_set(unit, ptp_id, clock_num);

    return rv;
}

/*
 * Function:
 *      _bcm_ptp_unicast_master_table_set
 * Purpose:
 *      Set unicast master table of a PTP clock.
 * Parameters:
 *      unit       - (IN)  Unit number.
 *      ptp_id     - (IN)  PTP stack ID.
 *      clock_num  - (IN)  PTP clock number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
static int 
_bcm_ptp_unicast_master_table_set(
    int unit, 
    bcm_ptp_stack_id_t ptp_id, 
    int clock_num)
{
    int rv = BCM_E_UNAVAIL;
    
    uint8 payload[PTP_MGMTMSG_PAYLOAD_MAX_UNICAST_MASTER_TABLE_SIZE_OCTETS] = {0};
    uint8 resp[PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS];
    int resp_len = PTP_MGMTMSG_RESP_MAX_SIZE_OCTETS; 
    
    _bcm_ptp_unicast_master_table_t *unicast_master_table;
    bcm_ptp_port_identity_t portid;     
    int i = 0;
    int k = 0;
    int el = 0;
    int raw_l2_length = 0;
    
    if (BCM_FAILURE(rv = _bcm_ptp_function_precheck(unit, ptp_id, clock_num, 
            PTP_CLOCK_PORT_NUMBER_DEFAULT))) {
        return rv;   
    }
  
    if ((unit_ucm_array[unit].memstate != PTP_MEMSTATE_INITIALIZED) ||
            (unit_ucm_array[unit].stack_array[ptp_id].memstate != 
            PTP_MEMSTATE_INITIALIZED)) {
        return BCM_E_UNAVAIL;
    }

    if (BCM_FAILURE(rv = bcm_common_ptp_clock_port_identity_get(unit, ptp_id, 
            clock_num, 1, &portid))) {
        return rv;
    }

    unicast_master_table = &unit_ucm_array[unit].stack_array[ptp_id]
                                                .table[clock_num];

    /* 
     * Make payload.
     *    Octet 0...1 : Number of unicast master table entries.
     *    Octet 3...* : Unicast master table entries.
     */
    i = 0;
    _bcm_ptp_uint16_write(payload, unicast_master_table->num_entries);  
    i += sizeof(uint16);

    for (el = 0; el < unicast_master_table->num_entries; ++el) {
        _bcm_ptp_uint16_write(&payload[i], 
            unicast_master_table->entry[el].address.addr_type);  
        i += sizeof(uint16);

        switch (unicast_master_table->entry[el].address.addr_type) {
        case bcmPTPUDPIPv4:
            /* Ethernet/UDP/IPv4 address type. */
            _bcm_ptp_uint16_write(&payload[i], PTP_IPV4_ADDR_SIZE_BYTES);  
            i += sizeof(uint16);
            _bcm_ptp_uint32_write(&payload[i], 
                unicast_master_table->entry[el].address.ipv4_addr);  
            i += sizeof(uint32);

            break;

        case bcmPTPUDPIPv6:
            /* Ethernet/UDP/IPv6 address type. */
            _bcm_ptp_uint16_write(&payload[i], PTP_IPV6_ADDR_SIZE_BYTES);  
            i += sizeof(uint16);

            for (k = 0; k < PTP_IPV6_ADDR_SIZE_BYTES; ++k) {
                payload[i++] = unicast_master_table->entry[el]
                               .address.ipv6_addr[k];
            }

            break;

        default:
            _bcm_ptp_uint16_write(&payload[i], 0);  
            i += sizeof(uint16);
        }

        payload[i++] = (uint8)unicast_master_table->entry[el]
                              .log_sync_interval;
        payload[i++] = (uint8)unicast_master_table->entry[el]
                              .log_min_delay_request_interval;

        raw_l2_length = unicast_master_table->entry[el]
                        .address.raw_l2_header_length;
        _bcm_ptp_uint16_write(&payload[i], raw_l2_length);  
        i += sizeof(uint16);

        for (k = 0; k < raw_l2_length; ++k) {
            payload[i++] = unicast_master_table->entry[el]
                           .address.raw_l2_header[k];
        }
    }    

    rv = _bcm_ptp_management_message_send(unit, ptp_id, clock_num, &portid,
            PTP_MGMTMSG_SET, PTP_MGMTMSG_ID_UNICAST_MASTER_TABLE, 
            payload, i, resp, &resp_len);
    
    return rv;
}
#endif /* defined(INCLUDE_PTP)*/
