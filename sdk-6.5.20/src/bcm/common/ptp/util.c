/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    util.c
 *
 * Purpose: 
 *
 * Functions:
 *      _bcm_ptp_uint16_read
 *      _bcm_ptp_uint32_read
 *      _bcm_ptp_uint64_read
 *      _bcm_ptp_uint16_write
 *      _bcm_ptp_uint32_write
 *      _bcm_ptp_uint64_write
 *      _bcm_ptp_int64_read
 *      _bcm_ptp_function_precheck
 *      _bcm_ptp_clock_lookup
 *      _bcm_ptp_peer_address_convert
 *      _bcm_ptp_port_address_convert
 *      _bcm_ptp_peer_address_raw_compare
 *      _bcm_ptp_peer_address_compare
 *      _bcm_ptp_port_address_compare
 *      _bcm_ptp_addr_len
 *      _bcm_ptp_is_clockid_null
 *      _bcm_ptp_port_address_cmp
 *      _bcm_ptp_dump_hex
 */

#if defined(INCLUDE_PTP)

#include <shared/bsl.h>

#include <soc/defs.h>
#include <soc/drv.h>
#include <shared/bsl.h>
#include <sal/core/time.h>
#include <sal/core/dpc.h>
#include <shared/bsl.h>
#include <bcm/ptp.h>
#include <bcm_int/common/ptp.h>
#include <bcm/error.h>

#if defined(BCM_PTP_INTERNAL_STACK_SUPPORT)
#ifdef NO_SAL_APPL
#include <string.h>
#else
#include <sal/appl/sal.h>
#endif
#include <include/soc/uc.h>
#endif /* BCM_PTP_INTERNAL_STACK_SUPPORT */
#include <bcm_int/control.h>

#define PTP_UTIL_HEXDUMP_LINE_WIDTH_OCTETS                            (100)


static const bcm_ptp_clock_identity_t BCM_PTP_ALL_CLOCKS = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};

/*
 * Function:
 *      _bcm_ptp_uint16_read
 * Purpose:
 *      Read a 16-bit unsigned integer from a buffer in network byte order.
 * Parameters:
 *      buffer - (IN) Data buffer.
 * Returns:
 *      Result.
 * Notes:
 */
uint16 
_bcm_ptp_uint16_read(
        uint8* buffer)
{   
    return ((((uint16)(buffer[0])) << 8) + (((uint16)(buffer[1]))));
}

/*
 * Function:
 *      _bcm_ptp_uint32_read
 * Purpose:
 *      Read a 32-bit unsigned integer from a buffer in network byte order.
 * Parameters:
 *      buffer - (IN) Data buffer.
 * Returns:
 *      Result.
 * Notes:
 */
uint32 
_bcm_ptp_uint32_read(
    uint8* buffer)
{
    return ((((uint32)(buffer[0])) << 24) + 
            (((uint32)(buffer[1])) << 16) +
            (((uint32)(buffer[2])) << 8)  +
            (((uint32)(buffer[3]))));
}

/*
 * Function:
 *      _bcm_ptp_uint64_read
 * Purpose:
 *      Read a 64-bit unsigned integer from a buffer in network byte order.
 * Parameters:
 *      buffer - (IN) Data buffer.
 * Returns:
 *      Result.
 * Notes:
 */
uint64 
_bcm_ptp_uint64_read(uint8* buffer)
{
    uint64 val64;

    COMPILER_64_SET(val64,
             (buffer[0] << 24) + (buffer[1] << 16) +
             (buffer[2] << 8)  + buffer[3],
             (buffer[4] << 24) + (buffer[5] << 16) + 
             (buffer[6] << 8)  + buffer[7]);
    return val64;
}

/*
 * Function:
 *      _bcm_ptp_uint16_write
 * Purpose:
 *      Write a 16-bit unsigned integer to a buffer in network byte order.
 * Parameters:
 *      value  - (IN)  Data.
 *      buffer - (OUT) Data buffer.
 * Returns:
 *      None.
 * Notes:
 */
void 
_bcm_ptp_uint16_write(
    uint8* buffer, 
    const uint16 value)
{
    buffer[0] = ((value >> 8) & 0xff);
    buffer[1] = ((value) & 0xff);
}

/*
 * Function:
 *      _bcm_ptp_uint32_write
 * Purpose:
 *      Write a 32-bit unsigned integer to a buffer in network byte order.
 * Parameters:
 *      value  - (IN)  Data.
 *      buffer - (OUT) Data buffer.
 * Returns:
 *      None.
 * Notes:
 */
void
_bcm_ptp_uint32_write(
    uint8* buffer, 
    const uint32 value)
{
    buffer[0] = ((value >> 24) & 0xff);
    buffer[1] = ((value >> 16) & 0xff);
    buffer[2] = ((value >> 8) & 0xff);
    buffer[3] = ((value) & 0xff);
}

/*
 * Function:
 *      _bcm_ptp_uint64_write
 * Purpose:
 *      Write a 64-bit unsigned integer to a buffer in network byte order.
 * Parameters:
 *      value  - (IN)  Data.
 *      buffer - (OUT) Data buffer.
 * Returns:
 *      None.
 * Notes:
 */
void 
_bcm_ptp_uint64_write(
    uint8* buffer, 
    const uint64 value)
{
    uint32 low;
    uint32 high;

    COMPILER_64_TO_32_LO(low,value);
    COMPILER_64_TO_32_HI(high,value);

    buffer[0] = ((high >> 24) & 0xff);
    buffer[1] = ((high >> 16) & 0xff);
    buffer[2] = ((high >> 8) & 0xff);
    buffer[3] = ((high) & 0xff);
    buffer[4] = ((low >> 24) & 0xff);
    buffer[5] = ((low >> 16) & 0xff);
    buffer[6] = ((low >> 8) & 0xff);
    buffer[7] = ((low) & 0xff);
}

/*
 * Function:
 *      _bcm_ptp_int64_write
 * Purpose:
 *      Write a 64-bit signed integer to a buffer in network byte order.
 * Parameters:
 *      value  - (IN)  Data.
 *      buffer - (OUT) Data buffer.
 * Returns:
 *      None.
 * Notes:
 *      Casting to and from int64_t is not available on some platforms
 */
void
_bcm_ptp_int64_write(
    uint8* buffer,
    const int64 value)
{
    buffer[0] = (COMPILER_64_HI(value) >> 24) & 0xff;
    buffer[1] = (COMPILER_64_HI(value) >> 16) & 0xff;
    buffer[2] = (COMPILER_64_HI(value) >> 8) & 0xff;
    buffer[3] = (COMPILER_64_HI(value) & 0xff);
    buffer[4] = (COMPILER_64_LO(value) >> 24) & 0xff;
    buffer[5] = (COMPILER_64_LO(value) >> 16) & 0xff;
    buffer[6] = (COMPILER_64_LO(value) >> 8) & 0xff;
    buffer[7] = (COMPILER_64_LO(value) & 0xff);
}


/*
 * Function:
 *      _bcm_ptp_int64_read
 * Purpose:
 *      Read a 64-bit signed integer from a buffer in network byte order.
 * Parameters:
 *      buffer - (IN) Data buffer.
 * Returns:
 *      Result.
 * Notes:
 *      Casting to and from int64_t is not available on some platforms
 */
int64
_bcm_ptp_int64_read(uint8* buffer)
{
    int64 val;
    int64 temp;

    COMPILER_64_SET(temp, buffer[0] << 24, 0);
    val = temp;
    COMPILER_64_SET(temp, buffer[1] << 16, 0);
    COMPILER_64_ADD_64(val, temp);
    COMPILER_64_SET(temp, buffer[2] << 8, 0);
    COMPILER_64_ADD_64(val, temp);
    COMPILER_64_SET(temp, buffer[3], 0);
    COMPILER_64_ADD_64(val, temp);
    COMPILER_64_SET(temp, 0, buffer[4] << 24);
    COMPILER_64_ADD_64(val, temp);
    COMPILER_64_SET(temp, 0, buffer[5] << 16);
    COMPILER_64_ADD_64(val, temp);
    COMPILER_64_SET(temp, 0, buffer[6] << 8);
    COMPILER_64_ADD_64(val, temp);
    COMPILER_64_SET(temp, 0, buffer[7]);
    COMPILER_64_ADD_64(val, temp);

    return val;
}


/*
 * Function:
 *      _bcm_ptp_function_precheck
 * Purpose:
 *      Perform basic argument and PTP module prechecks on a function.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      ptp_id     - (IN) PTP stack ID.
 *      clock_num  - (IN) PTP clock number.
 *      clock_port - (IN) PTP clock port number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
_bcm_ptp_function_precheck(
    int unit, 
    bcm_ptp_stack_id_t ptp_id, 
    int clock_num, 
    uint32 clock_port)
{
    int rv = BCM_E_UNAVAIL;
   
    _bcm_ptp_info_t *ptp_info_p;
    bcm_ptp_clock_info_t ci;    
 
    if ((unit < PTP_UNIT_NUMBER_DEFAULT) ||
        (unit >= BCM_MAX_NUM_UNITS)) {
        return BCM_E_PARAM;
    }
    
    if (soc_feature(unit, soc_feature_ptp)) {                
        SET_PTP_INFO;
     
        if (ptp_info_p->memstate != PTP_MEMSTATE_INITIALIZED) {
            return BCM_E_UNAVAIL;
        }
        
        /* Argument checking and error handling. */
        if ((ptp_id < PTP_STACK_ID_DEFAULT) ||
            (ptp_id >= PTP_MAX_STACKS_PER_UNIT) ||
            (clock_num < PTP_CLOCK_NUMBER_DEFAULT) ||
            (clock_num >= PTP_MAX_CLOCK_INSTANCES)) {
            return BCM_E_PARAM;
        }
        
        if ((clock_port == PTP_IEEE1588_ALL_PORTS) ||
                (clock_port == PTP_CLOCK_PORT_NUMBER_DEFAULT)) {
            return BCM_E_NONE;
        } else {
            if (BCM_FAILURE(rv = _bcm_ptp_clock_cache_info_get(unit, ptp_id, 
                    clock_num, &ci))) {
                return rv;
            }

            if ((clock_port < PTP_CLOCK_PORT_NUMBER_DEFAULT) || 
                    (clock_port > ci.num_ports)) {
                return BCM_E_PORT; 
            }

            switch (ci.type) {
            case bcmPTPClockTypeOrdinary:
            case bcmPTPClockTypeBoundary:
            case bcmPTPClockTypeTransparent:
                break;
            default:
                return BCM_E_PARAM;
            }
        }
        
    }
    
    return rv;
}

/*
 * Function:
 *      _bcm_ptp_clock_lookup
 * Purpose:
 *      Lookup the PTP clock number of a PTP clock for a given unit / PTP stack ID
 *      based on caller-provided clock identity.
 * Parameters:
 *      clock_identity - (IN)  PTP clock identity to lookup.
 *      unit           - (IN) Unit number.
 *      ptp_id         - (IN) PTP stack ID.
 *      clock_num      - (OUT) PTP clock number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int _bcm_ptp_clock_lookup(
    const bcm_ptp_clock_identity_t clock_identity,
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int *clock_num)
{
    bcm_ptp_clock_info_t ci;
    int i;
    
    /* Affiliate "all-clocks" clockIdentity with default stack, unit, and clock number. */
    if (!memcmp(BCM_PTP_ALL_CLOCKS, clock_identity, sizeof(bcm_ptp_clock_identity_t))) {
        /* this should only match if the unit/ptp_id match the defaults */
        if (unit == PTP_UNIT_NUMBER_DEFAULT && ptp_id == PTP_STACK_ID_DEFAULT) {
            *clock_num = PTP_CLOCK_NUMBER_DEFAULT;
            return BCM_E_NONE;
        } else {
            return BCM_E_NOT_FOUND;
        }
    }
            
    if (!BCM_UNIT_VALID(unit)) {
        return BCM_E_NOT_FOUND;
    }

    if ((unit < PTP_UNIT_NUMBER_DEFAULT) ||
        (unit >= BCM_MAX_NUM_UNITS))
    {
       return BCM_E_PARAM;
    }

    for (i = PTP_CLOCK_NUMBER_DEFAULT; i < PTP_MAX_CLOCK_INSTANCES; ++i) {
        if (BCM_FAILURE(_bcm_ptp_clock_cache_info_get(unit, ptp_id, i, &ci))) {
            continue;
        }

        if (!memcmp(ci.clock_identity, clock_identity, sizeof(bcm_ptp_clock_identity_t))) {
            *clock_num = i;
            return BCM_E_NONE;
        }
    }

    return BCM_E_NOT_FOUND;
}

/* Function:
 *      _bcm_ptp_peer_address_convert
 * Purpose:
 *      Convert address from peer format to clock-port format.
 * Parameters:
 *      peer_addr - (IN)  Address (peer format).
 *      port_addr - (OUT) Address (clock-port format).
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
_bcm_ptp_peer_address_convert(
    bcm_ptp_clock_peer_address_t *peer_addr,
    bcm_ptp_clock_port_address_t *port_addr)
{
    port_addr->addr_type = peer_addr->addr_type;

    memset(port_addr->address, 0, BCM_PTP_MAX_NETW_ADDR_SIZE);
    switch (peer_addr->addr_type) {
    case bcmPTPUDPIPv4:
        _bcm_ptp_uint32_write(port_addr->address, peer_addr->ipv4_addr);
        break;

    case bcmPTPUDPIPv6:
        memcpy(port_addr->address, peer_addr->ipv6_addr, PTP_IPV6_ADDR_SIZE_BYTES);
        break;

    default:
        return BCM_E_PARAM;

    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_ptp_port_address_convert
 * Purpose:
 *      Convert address from clock-port format to peer format.
 * Parameters:
 *      port_addr - (IN)  Address (clock-port format).
 *      peer_addr - (OUT) Address (peer format).
 * Returns:
 *      BCM_E_XXX - Function status.
 * Notes:
 */
int
_bcm_ptp_port_address_convert(
     bcm_ptp_clock_port_address_t *port_addr,
     bcm_ptp_clock_peer_address_t *peer_addr)
{
    peer_addr->addr_type = port_addr->addr_type;

    switch (port_addr->addr_type) {
    case bcmPTPUDPIPv4:
        peer_addr->ipv4_addr = _bcm_ptp_uint32_read(port_addr->address);
        break;

    case bcmPTPUDPIPv6:
        memcpy(peer_addr->ipv6_addr, port_addr->address, PTP_IPV6_ADDR_SIZE_BYTES);
        break;

    default:
        return BCM_E_PARAM;

    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_ptp_peer_address_raw_compare
 * Purpose:
 *      Compare addresses.
 *      Address "A" is a peer-address type.
 *      Address "B" is a raw multi-octet buffer.
 * Parameters:
 *      a        - (IN) Address "A".
 *      b        - (IN) Address "B".
 *      protocol - (IN) Address "B" network protocol.
 * Returns:
 *      0: Addresses do not match.
 *      1: Addresses match.
 * Notes:
 */
int 
_bcm_ptp_peer_address_raw_compare(
    const bcm_ptp_clock_peer_address_t *a,
    uint8 *b,
    bcm_ptp_protocol_t protocol)
{
    if (a->addr_type != protocol) {
        return 0;
    }

    switch (protocol) {
    case bcmPTPUDPIPv4:
        return (a->ipv4_addr == _bcm_ptp_uint32_read(b));
    case bcmPTPUDPIPv6:
        return !memcmp(a->ipv6_addr, b, PTP_IPV6_ADDR_SIZE_BYTES);
    default:
        return 0;
    }
}

/*
 * Function:
 *      _bcm_ptp_peer_address_compare
 * Purpose:
 *      Compare addresses.
 * Parameters:
 *      a - (IN) Address "A".
 *      b - (IN) Address "B".
 * Returns:
 *      0: Addresses do not match.
 *      1: Addresses match.
 * Notes:
 */
int 
_bcm_ptp_peer_address_compare(
    const bcm_ptp_clock_peer_address_t *a, 
    const bcm_ptp_clock_peer_address_t *b) 
{
    if (a->addr_type != b->addr_type) {
        return 0;
    }

    switch (a->addr_type) {
    case bcmPTPUDPIPv4:
        return (a->ipv4_addr == b->ipv4_addr);
    case bcmPTPUDPIPv6:
        return !memcmp(a->ipv6_addr, b->ipv6_addr, PTP_IPV6_ADDR_SIZE_BYTES);
    default:
        return 0;
    }
}

/*
 * Function:
 *      _bcm_ptp_peer_portid_compare
 * Purpose:
 *      Compare port Identity.
 * Parameters:
 *      a - (IN) ClockId "A".
 *      b - (IN) ClockId "B".
 * Returns:
 *      0: Not match.
 *      1: Match.
 * Notes:
 */
int
_bcm_ptp_peer_portid_compare(
    const bcm_ptp_port_identity_t *a,
    const bcm_ptp_port_identity_t *b)
{

/*
    LOG_VERBOSE(BSL_LS_BCM_PTP,
            (BSL_META_U(0, " PortId_A[%0x %0x %0x %0x %0x %0x %0x %0x : %d] PortId_B[%0x %0x %0x %0x %0x %0x %0x %0x : %d] \n"),
							 a->clock_identity[0],a->clock_identity[1],a->clock_identity[2],
							 a->clock_identity[3],a->clock_identity[4],a->clock_identity[5],
                             a->clock_identity[6],a->clock_identity[7],
							 a->port_number,
							 b->clock_identity[0],b->clock_identity[1],b->clock_identity[2],
							 b->clock_identity[3],b->clock_identity[4],b->clock_identity[5],
                             b->clock_identity[6],b->clock_identity[7],
							 b->port_number));
*/

    if (a->port_number != b->port_number) {
        return 0;
    }

    return !memcmp(a->clock_identity, b->clock_identity, sizeof(bcm_ptp_clock_identity_t));
}

/*
 * Function:
 *      _bcm_ptp_port_address_compare
 * Purpose:
 *      Compare addresses (clock-port address format).
 * Parameters:
 *      a - (IN) Address "A".
 *      b - (IN) Address "B".
 * Returns:
 *      0: Addresses do not match.
 *      1: Addresses match.
 * Notes:
 */
int
_bcm_ptp_port_address_compare(
     const bcm_ptp_clock_port_address_t *a,
     const bcm_ptp_clock_port_address_t *b)
{
    if (a->addr_type != b->addr_type) {
        return 0;
    }

    return !memcmp(a->address, b->address, _bcm_ptp_addr_len((int)a->addr_type));
}

/*
 * Function:
 *      _bcm_ptp_addr_len
 * Purpose:
 *      Determine address length.
 * Parameters:
 *      addr_type - (IN) Address Type
 * Returns:
 *      0               : Invalid Address Type.
 *      Address Length  : Valid Address Type
 *      
 * Notes:
 */
int _bcm_ptp_addr_len(int addr_type) {
    switch (addr_type) {
    case bcmPTPUDPIPv4:
        return PTP_IPV4_ADDR_SIZE_BYTES;
    case bcmPTPUDPIPv6:
        return PTP_IPV6_ADDR_SIZE_BYTES;
    case bcmPTPIEEE8023:
        return 6;
    default:
        return 0;
    }
}

/*
 * Function:
 *      _bcm_ptp_is_clock_id_null
 * Purpose:
 *      Check if PTP clock identity is all zeros.
 * Parameters:
 *      clockID - (IN) PTP Clock Identity.
 * Returns:
 *      0: Clock Identity is not NULL 
 *      1: Clock Identity is NULL
 * Notes:
 */
int _bcm_ptp_is_clockid_null(bcm_ptp_clock_identity_t clockID)
{
    int i;
    for (i = 0; i < BCM_PTP_CLOCK_EUID_IEEE1588_SIZE; ++i) {
        if (clockID[i] != 0) {
            return 0;
        }
    }
    return 1;
}

/*
 * Function:
 *      _bcm_ptp_port_address_cmp
 * Purpose:
 *      Compare addresses.
 * Parameters:
 *      a - (IN) Address "A".
 *      b - (IN) Address "B".
 * Returns:
 *      0: Addresses match.
 *      Nonzero: Addresses do not match.
 * Notes:
 */
int _bcm_ptp_port_address_cmp(bcm_ptp_clock_port_address_t *a, bcm_ptp_clock_port_address_t *b)
{
    if (a->addr_type > b->addr_type) {
        return 1;
    }
    if (a->addr_type < b->addr_type) {
        return -1;
    }
    return memcmp(a->address, b->address, _bcm_ptp_addr_len(a->addr_type));
}

/*
 * Function:
 *      _bcm_ptp_dump_hex
 * Purpose:
 *      Print hexadecimal buffer.
 * Parameters:
 *      buf - (IN) buffer to be printed
 *      len - (IN) message length
 *      indent - (IN) number of spaces to indent
 * Returns:
 *      none
 * Notes:
 */
void _bcm_ptp_dump_hex(uint8 *buf, int len, int indent)
{
    char line[PTP_UTIL_HEXDUMP_LINE_WIDTH_OCTETS];
    int i,j;
    int linepos = 0;

    /*
     * Limit number of spaces to indent based on line width and the
     * maximum hexadecimal data width (32 x 3 + 1 null terminator).
     */
    indent = indent > (PTP_UTIL_HEXDUMP_LINE_WIDTH_OCTETS - 97) ?
             (PTP_UTIL_HEXDUMP_LINE_WIDTH_OCTETS - 97) : indent;

    for (j = 0; j < indent; j++) {
        sal_sprintf(line+linepos++," ");
    }
    for (i = 0; i < len; ++i) {
        sal_sprintf(line + linepos, "%02x", *buf++);
        linepos += 2;
        sal_sprintf(line + linepos, " ");
        ++linepos;

        if ((i & 0x1f) == 0x1f) {
            LOG_CLI((BSL_META("%s\n"), line));
            line[0] = 0;
            linepos = 0;
            for (j = 0; j < indent; j++) {
                sal_sprintf(line+linepos++," ");
            }
        }
    }

    /* output last line if it wasn't complete */
    if (len & 0x1f) {
        LOG_CLI((BSL_META("%s\n"), line));
    }
}

sal_time_t _bcm_ptp_monotonic_time()
{
    static _bcm_ptp_mutex_t mutex = 0;
    static int32 time_offset = 0;
    static sal_time_t last_time;
    static sal_usecs_t last_usecs;
    sal_time_t this_time = sal_time(), offset_time;
    sal_usecs_t this_usecs = sal_time_usecs();
    int32 usecs_diff, time_diff, time_jump;
    int rv;

    if (!mutex) {
        mutex = _bcm_ptp_mutex_create("ptp_monotonic_time");

        last_time = this_time;
        last_usecs = this_usecs;
    }

    rv = _bcm_ptp_mutex_take(mutex, 1000000); /* one second timeout */

    if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_COMMON,
                  (BSL_META("Failed to get PTP monotonic_time mutex: %d (%s)\n"), rv, bcm_errmsg(rv)));
        return this_time + time_offset;
    }

    time_diff = (int32)(this_time - last_time);
    usecs_diff = (int32)(this_usecs - last_usecs);

    /* if the clock hasn't been adjusted, the difference between these two differences
       should be zero.  If it has been adjusted, the difference is the magnitude of the
       "time jump"
    */
    time_jump = time_diff - (usecs_diff / 1000000);

    if (time_jump < -2 || time_jump > 2) {
        LOG_INFO(BSL_LS_BCM_PTP,
                  (BSL_META("Time jumped by %d seconds \n"), (int)time_jump));

        /* the output of sal_time() evidently jumped, so incorporate that jump into future output */
        time_offset -= time_jump;
    }

    last_time = this_time;
    last_usecs = this_usecs;

    offset_time = this_time + time_offset;

    _bcm_ptp_mutex_give(mutex);

    return offset_time;
}

#if defined(BCM_PTP_INTERNAL_STACK_SUPPORT)
/*
 * Function:
 *      _bcm_ptp_check_firmware_exist
 * Purpose:
 *      Check if the system is running provided firmware
 * Parameters:
 *      fw_str - (IN) firmware string
 * Returns:
 *      BCM_E_XXX;
 * Notes:
 *      Function tells whether the provided firmware
 *      is running on the system, return unavail if not
 */

int
_bcm_ptp_check_firmware_exist (char* fw_str)
{
    int uc;
    char *version = NULL;
    int firmware = 0;
    int unit = 0;

    for(uc=0; uc<SOC_INFO(unit).num_ucs; uc++) {
        if (!soc_uc_in_reset(unit, uc)) {
            version = soc_uc_firmware_version(unit, uc);
            if (version) {
#ifdef NO_SAL_APPL
                if (strstr((char *)version,(char *)fw_str)) {
#else
                if (sal_strcasestr((char *)version,(char *)fw_str)) {
#endif
                    firmware = 1;
                }
                soc_cm_sfree(unit, version);
                if (1 == firmware) {
                    break;
                }
            }
        }
    }
    if (firmware == 0) {
        return BCM_E_UNAVAIL;
    }
    return BCM_E_NONE;
}
#endif /* BCM_PTP_INTERNAL_STACK_SUPPORT */
#endif /* defined(INCLUDE_PTP)*/
