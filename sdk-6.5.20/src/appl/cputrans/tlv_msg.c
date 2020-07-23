/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        tlv_msg.c
 * Purpose:     TLV Message Utility
 */

#include <sal/core/libc.h>
#include <bcm/error.h>
#include <appl/cputrans/tlv_msg.h>

#define UINT8_SIZE     (sizeof(uint8))
#define UINT16_SIZE    (sizeof(uint16))
#define UINT32_SIZE    (sizeof(uint32))
#define UINT64_SIZE    (sizeof(uint64))

#define TLV_TYPE_SIZE           UINT8_SIZE
#define TLV_LENGTH_SIZE         UINT16_SIZE
#define TLV_HEADER_SIZE        (TLV_TYPE_SIZE + TLV_LENGTH_SIZE)


#define TLV_END_SIZE            TLV_TYPE_SIZE

#define TLV_MSG_END_CHECK(msg)                   \
    if (msg->cur_ptr >= msg->buffer_end) { return BCM_E_NOT_FOUND; }

#define PARAM_NULL_CHECK(arg)  \
    if ((arg) == NULL) { return BCM_E_PARAM; }

/* Function pointer types for pack/unpack */
typedef void (*_tlv_msg_value_pack_fp)(void *buffer, const void *value);
typedef void (*_tlv_msg_value_unpack_fp)(const void *buffer, void *value);


/*
 * Function:
 *     _tlv_msg_uint8_pack
 *     _tlv_msg_uint16_pack
 *     _tlv_msg_uint32_pack
 *     _tlv_msg_string_pack
 * Purpose:
 *     Packs given value into buffer.
 *     Numbers are packed in network byte order.
 *     Strings must be null-terminated.
 * Parameters:
 *     buffer - (OUT) Buffer where to write to
 *     value  - Value to pack
 * Returns:
 *     None
 * Notes:
 *     Assumes enough space in buffer.
 */
STATIC void
_tlv_msg_uint8_pack(void *buffer, const void *value)
{
    *((uint8 *)buffer) = *((uint8 *)value);

    return;
}

STATIC void
_tlv_msg_uint16_pack(void *buffer, const void *value)
{
    uint16  raw;

    raw = bcm_htons(*((uint16 *)value));
    sal_memcpy(buffer, (const void *)&raw, UINT16_SIZE);

    return;
}

STATIC void
_tlv_msg_uint32_pack(void *buffer, const void *value)
{
    uint32  raw;

    raw = bcm_htonl(*((uint32 *)value));
    sal_memcpy(buffer, (const void *)&raw, UINT32_SIZE);

    return;
}

STATIC void
_tlv_msg_uint64_pack(void *buffer, const void *value)
{
    sal_memcpy(buffer, value, UINT64_SIZE);

    return;
}

STATIC void
_tlv_msg_string_pack(void *buffer, const void *value)
{
    int len_value = 0;
    
    len_value = sal_strlen((const char*)value);
    sal_strncpy((char *)buffer, (const char *)value, len_value);
    if(len_value)
        *((char*)buffer+ len_value) = '\0';

    return;
}

/*
 * Function:
 *     _tlv_msg_uint8_unpack
 *     _tlv_msg_uint16_unpack
 *     _tlv_msg_uint32_unpack
 *     _tlv_msg_string_unpack
 * Purpose:
 *     Unpacks value from given buffer.
 * Parameters:
 *     buffer - Buffer where to read data from
 *     value  - (OUT) Returns unpacked value
 * Returns:
 *     None
 * Notes:
 *     Assumes available space in value.
 */
STATIC void
_tlv_msg_uint8_unpack(const void *buffer, void *value)
{
    *((uint8 *)value) = *((uint8 *)buffer);

    return;
}

STATIC void
_tlv_msg_uint16_unpack(const void *buffer, void *value)
{
    uint16  raw;

    sal_memcpy((void *)&raw, buffer, UINT16_SIZE);
    *((uint16 *)value) = bcm_ntohs(raw);

    return;
}

STATIC void
_tlv_msg_uint32_unpack(const void *buffer, void *value)
{
    uint32  raw;

    sal_memcpy((void *)&raw, buffer, UINT32_SIZE);
    *((uint32 *)value) = bcm_ntohl(raw);

    return;
}

STATIC void
_tlv_msg_uint64_unpack(const void *buffer, void *value)
{
    sal_memcpy(value, buffer, UINT64_SIZE);

    return;
}

STATIC void
_tlv_msg_string_unpack(const void *buffer, void *value)
{
    int len_buffer = 0;
    
    len_buffer = sal_strlen((const char*)buffer);
    sal_strncpy((char *)value, (const char *)buffer, len_buffer);
    if (len_buffer)
        *((char*)value + len_buffer) = '\0';

    return;
}

/*
 * Function:
 *     _tlv_msg_end
 * Purpose:
 *     Marks the end of message by setting the End-TLV.
 * Parameters:
 *     msg - (IN/OUT) TLV message object to set End-TLV
 * Returns:
 *     BCM_E_NONE
 * Notes:
 *     Assumes caller has checked for available space in message buffer.
 */
STATIC int
_tlv_msg_end(tlv_msg_t *msg)
{
    uint8 type = TLV_TYPE_END;

    _tlv_msg_uint8_pack((void *)msg->cur_ptr, (const void *)&type);
    msg->end = msg->cur_ptr + TLV_END_SIZE;

    return BCM_E_NONE;
}

/*
 * Function:
 *     _tlv_msg_length_check
 * Purpose:
 *     Checks that there is available space in message for given
 *     value size.
 * Parameters:
 *     msg  - TLV message object to check available space
 *     size - Size in bytes
 * Returns:
 *     BCM_E_NONE
 *     BCM_E_MEMORY - Not enough space left in buffer
 */
STATIC int
_tlv_msg_length_check(tlv_msg_t *msg, int size)
{
    /* Add End-TLV size (only once) */
    if (msg->start == msg->end) {
        size += TLV_END_SIZE;
    }

    if ((msg->buffer_end - msg->end) < size) {
        return BCM_E_MEMORY;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _tlv_msg_value_add
 * Purpose:
 *     Adds value for given message object.
 * Parameters:
 *     msg        - (IN/OUT) Message object where to add value
 *     value      - Value to add
 *     value_size - Size of value, in bytes
 *     value_pack - Pointer to packer routine
 * Returns:
 *     BCM_E_NONE   - Success
 *     BCM_E_PARAM  - Null pointer
 *     BCM_E_MEMORY - Not enough space left in buffer
 *     BCM_E_XXX    - Failure
 */
STATIC int
_tlv_msg_value_add(tlv_msg_t *msg, const void *value, int value_size,
                   _tlv_msg_value_pack_fp value_pack)
{
    PARAM_NULL_CHECK(msg);
    PARAM_NULL_CHECK(value);

    /* Check available space */
    BCM_IF_ERROR_RETURN(_tlv_msg_length_check(msg, value_size));

    /* Write value */
    value_pack((void *)msg->cur_ptr, value);
    msg->cur_ptr += value_size;
    
    /* Update TLV length */
    if (msg->tlv_length_ptr != NULL) {
        msg->tlv_length += value_size;
        _tlv_msg_uint16_pack((void *)msg->tlv_length_ptr,
                             (const void *)&msg->tlv_length);
    }

    /* End-TLV */
    _tlv_msg_end(msg);

    return BCM_E_NONE;
}

/*
 * Function:
 *     _tlv_msg_value_get
 * Purpose:
 *     Gets value from given message object.
 * Parameters:
 *     msg            - (IN/OUT) Message object where to read value from
 *     value          - (OUT) Value read
 *     value_size     - Size of value, in bytes
 *     value_size_max - If >= 0, indicates max size for returning value
 *                      (e.g. for strings)
 *                      If < 0, ignore
 *     value_unpack   - Pointer to unpacker routine
 * Returns:
 *     BCM_E_NONE      - Success
 *     BCM_E_PARAM     - Null pointer
 *     BCM_E_NOT_FOUND - No more data to read
 *     BCM_E_FAIL      - Size of data left to read is smaller than expected
 *     BCM_E_XXX       - Failure
 */
STATIC int
_tlv_msg_value_get(tlv_msg_t *msg, void *value, int value_size,
                   int value_size_max, _tlv_msg_value_unpack_fp value_unpack)
{
    int  length_left;

    PARAM_NULL_CHECK(msg);
    PARAM_NULL_CHECK(value);

    TLV_MSG_END_CHECK(msg);

    /*
     * Get length of remaining data to read
     *
     * If currently reading a TLV, get unread number of bytes in TLV;
     * else, get unread number of bytes in message.
     */
    if (msg->tlv_length_ptr != NULL) {
        length_left = msg->tlv_left;
    } else {
        length_left = msg->buffer_end - msg->cur_ptr;
    }
    if (length_left <= 0) {
        return BCM_E_NOT_FOUND;    /* No more data to read */
    }   
    if (length_left < value_size) {
        return BCM_E_FAIL;         /* Bytes left to read smaller than size */
    }

    /* If max size is specified, check that value fits */
    if (value_size_max >= 0) {
        if (value_size_max < value_size) {
            return BCM_E_MEMORY;
        }
    }
    value_unpack((const void *)msg->cur_ptr, value);
    msg->cur_ptr += value_size;

    if (msg->tlv_length_ptr != NULL) {
        msg->tlv_left -= value_size;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     tlv_msg_t_init
 * Purpose:
 *     Initializes a TLV message structure.
 * Parameters:
 *     msg    - (IN/OUT) TLV message structure to initialize
 * Returns:
 *     None
 */
void
tlv_msg_t_init(tlv_msg_t *msg)
{
    if (msg != NULL) {
        sal_memset(msg, 0, sizeof(*msg));
    }

    return;
}

/*
 * Function:
 *     tlv_msg_buffer_set
 * Purpose:
 *     Sets the buffer for given TLV message object with given buffer.
 *     This routine must be called prior to setting or getting
 *     a TLV message.
 * Parameters:
 *     msg    - (IN/OUT) TLV message object
 *     buffer - (IN/OUT) Buffer where to write or read message
 *     length - Buffer size
 * Returns:
 *     BCM_E_NONE   - Success
 *     BCM_E_PARAM  - Null TLV message pointer
 */
int
tlv_msg_buffer_set(tlv_msg_t *msg, uint8 *buffer, int length)
{
    PARAM_NULL_CHECK(msg);

    if (buffer == NULL) {
        length = 0;
    }

    /* Set pointers to buffer */
    msg->start          = msg->end = msg->cur_ptr = buffer;
    msg->buffer_end     = buffer + length;
    msg->tlv_length_ptr = NULL;
    msg->tlv_length     = 0;
    msg->tlv_left       = 0;

    return BCM_E_NONE;
}

/*
 * Function:
 *     tlv_msg_add
 * Purpose:
 *     Adds a new TLV to current message object.
 * Parameters:
 *     msg  - (IN/OUT) Message object where to add TLV to
 *     type - TLV type
 * Returns:
 *     BCM_E_NONE   - Success
 *     BCM_E_PARAM  - Null pointer
 *     BCM_E_MEMORY - Not enough space left in buffer
 *     BCM_E_XXX    - Failure, other
 */
int
tlv_msg_add(tlv_msg_t *msg, tlv_msg_type_t type)
{
    PARAM_NULL_CHECK(msg);

    /* Check available space */
    BCM_IF_ERROR_RETURN(_tlv_msg_length_check(msg, TLV_HEADER_SIZE));

    /* Set TLV type */
    _tlv_msg_uint8_pack((void *)msg->cur_ptr, (const void *)&type);
    msg->cur_ptr += UINT8_SIZE;

    /* Set TLV length */
    msg->tlv_length_ptr = msg->cur_ptr;
    msg->tlv_length = 0;
    _tlv_msg_uint16_pack((void *)msg->cur_ptr, (const void *)&msg->tlv_length);
    msg->cur_ptr += UINT16_SIZE;

    /* End-TLV */
    _tlv_msg_end(msg);

    return BCM_E_NONE;
}

/*
 * Function:
 *     tlv_msg_get
 * Purpose:
 *     Gets next TLV type and length from given message object.
 * Parameters:
 *     msg    - (IN/OUT) Message object where to get TLV
 *     type   - (OUT) Returns TLV field type
 *     length - (OUT) If non-null, returns TLV length
 * Returns:
 *     BCM_E_NONE      - Success
 *     BCM_E_PARAM     - Null pointer
 *     BCM_E_NOT_FOUND - End of message, no more TLV elements
 *     BCM_E_XXX       - Failure
 */
int
tlv_msg_get(tlv_msg_t *msg, tlv_msg_type_t *type, tlv_msg_length_t *length)
{
    PARAM_NULL_CHECK(msg);
    PARAM_NULL_CHECK(type);

    TLV_MSG_END_CHECK(msg);

    /* This skips to next TLV if in the middle of reading a TLV */
    msg->cur_ptr += msg->tlv_left;

    msg->tlv_length_ptr = NULL;
    msg->tlv_length = 0;
    msg->tlv_left = 0;

    /* TLV type */
    _tlv_msg_uint8_unpack((const void *)msg->cur_ptr, (void *)type);
    msg->cur_ptr += UINT8_SIZE;
    if (*type == TLV_TYPE_END) {
        if (length != NULL) {
            *length = 0;
        }
        return BCM_E_NOT_FOUND;
    }

    /* TLV length */
    msg->tlv_length_ptr = msg->cur_ptr;
    _tlv_msg_uint16_unpack((const void *)msg->cur_ptr,
                           (void *)&msg->tlv_length);
    msg->cur_ptr += UINT16_SIZE;
    msg->tlv_left = msg->tlv_length;
    if (length != NULL) {
        *length = msg->tlv_length;
    }

    return BCM_E_NONE;
}

/*
 * Value adders
 *
 * Adds values into a TLV message.  If there are multiple values associated
 * with a TLV message header or a TLV type, the routine of the correct
 * type must be called for each one.
 */
/*
 * Function:
 *     tlv_msg_uint8_add
 *     tlv_msg_uint16_add
 *     tlv_msg_uint32_add
 * Purpose:
 *     Adds a value for given message object.
 * Parameters:
 *     msg   - (IN/OUT) Message object where to add value
 *     value - Value to add
 * Returns:
 *     BCM_E_NONE   - Success
 *     BCM_E_MEMORY - Not enough space left in buffer
 *     BCM_E_XXX    - Failure other
 */
int
tlv_msg_uint8_add(tlv_msg_t *msg, uint8 value)
{
    return(_tlv_msg_value_add(msg, (const void *)&value, sizeof(value),
                              _tlv_msg_uint8_pack)); 
}

int
tlv_msg_uint16_add(tlv_msg_t *msg, uint16 value)
{
    return(_tlv_msg_value_add(msg, (const void *)&value, sizeof(value),
                              _tlv_msg_uint16_pack)); 
}

int
tlv_msg_uint32_add(tlv_msg_t *msg, uint32 value)
{
    return(_tlv_msg_value_add(msg, (const void *)&value, sizeof(value),
                              _tlv_msg_uint32_pack));
}

int
tlv_msg_uint64_add(tlv_msg_t *msg, uint64 value)
{
    return(_tlv_msg_value_add(msg, (const void *)&value, sizeof(value),
                              _tlv_msg_uint64_pack));
}

/*
 * Function:
 *     tlv_msg_string_add
 * Purpose:
 *     Adds a string value for given message object,
 *     including the null-terminator.
 * Parameters:
 *     msg   - (IN/OUT) Message object where to add string
 *     value - String value to add
 * Returns:
 *     BCM_E_NONE   - Success
 *     BCM_E_PARAM  - Null pointer
 *     BCM_E_MEMORY - Not enough space left in buffer
 *     BCM_E_XXX    - Failure other
 */
int
tlv_msg_string_add(tlv_msg_t *msg, const char *value)
{
    return(_tlv_msg_value_add(msg, (const void *)value, strlen(value) + 1,
                              _tlv_msg_string_pack));
}

/*
 * Value getters
 *
 * Gets values from a TLV message.  If there are multiple values
 * associated with a TLV message header or a TLV type, the
 * getter must be called in the same order and use the same type
 * as the corresponding adder.
 */
/*
 * Function:
 *     tlv_msg_uint8_get
 *     tlv_msg_uint16_get
 *     tlv_msg_uint32_get
 * Purpose:
 *     Gets a value from given message object.
 * Parameters:
 *     msg   - (IN/OUT) Message object where to get value
 *     value - (OUT) Value
 * Returns:
 *     BCM_E_NONE      - Success
 *     BCM_E_PARAM     - Null pointer
 *     BCM_E_NOT_FOUND - No more data to read
 *     BCM_E_FAIL      - Size of data left to read is smaller than expected
 *     BCM_E_XXX       - Failure
 */
int
tlv_msg_uint8_get(tlv_msg_t *msg, uint8 *value)
{
    return (_tlv_msg_value_get(msg, (void *)value, sizeof(*value), -1,
                               _tlv_msg_uint8_unpack));
}

int
tlv_msg_uint16_get(tlv_msg_t *msg, uint16 *value)
{
    return (_tlv_msg_value_get(msg, (void *)value, sizeof(*value), -1,
                               _tlv_msg_uint16_unpack));
}

int
tlv_msg_uint32_get(tlv_msg_t *msg, uint32 *value)
{
    return (_tlv_msg_value_get(msg, (void *)value, sizeof(*value), -1,
                               _tlv_msg_uint32_unpack));
}

int
tlv_msg_uint64_get(tlv_msg_t *msg, uint64 *value)
{
    return (_tlv_msg_value_get(msg, (void *)value, sizeof(*value), -1,
                               _tlv_msg_uint64_unpack));
}

/*
 * Function:
 *     tlv_msg_string_get
 * Purpose:
 *     Gets a string value from given message object.
 * Parameters:
 *     msg       - (IN/OUT) Message object where to get string from
 *     value_max - Size of array
 *     value     - (OUT) String value
 * Returns:
 *     BCM_E_NONE - Success
 *     BCM_E_XXX  - Failure
 */
int
tlv_msg_string_get(tlv_msg_t *msg, int value_max, char *value)
{
    return (_tlv_msg_value_get(msg, (void *)value,
                               sal_strlen((const char *) msg->cur_ptr) + 1,
                               value_max,
                               _tlv_msg_string_unpack));
}

/*
 * Function:
 *     tlv_msg_length
 * Purpose:
 *     Gets the TLV message length, including the End-TLV marker.
 *     Only valid when setting TLV message.
 * Parameters:
 *     msg    - Message object
 *     length - (OUT) Message length
 * Returns:
 *     BCM_E_NONE  - Success
 *     BCM_E_PARAM - Null pointer
 *     BCM_E_XXX   - Failure
 */
int
tlv_msg_length(tlv_msg_t *msg, int *length)
{
    PARAM_NULL_CHECK(msg);
    PARAM_NULL_CHECK(length);

    *length = msg->end - msg->start;

    return BCM_E_NONE;
}

/*
 * Function:
 *     tlv_msg_resize
 * Purpose:
 *     Copies contents of the old message buffer into the
 *     new buffer and updates message object to use new buffer.
 *     It is an error if the new buffer is smaller than what the message
 *     currently requires.
 *
 *     The caller is responsible for releasing the previous buffer
 *     back to whatever allocator was used.
 * Parameters:
 *     msg    - (IN/OUT) Message object to resize
 *     buffer - (OUT) New buffer to be used by message
 *     length - Buffer size
 * Returns:
 *     BCM_E_NONE   - Success
 *     BCM_E_PARAM  - Null pointer
 *     BCM_E_MEMORY - New buffer size is too small
 *     BCM_E_XXX    - Failure
 */
int
tlv_msg_resize(tlv_msg_t *msg, uint8 *buffer, int length)
{
    int    msg_size;
    uint8  *buffer_old;

    PARAM_NULL_CHECK(msg);
    PARAM_NULL_CHECK(buffer);

    /* Check that new buffer is not smaller than message current length */
    BCM_IF_ERROR_RETURN(tlv_msg_length(msg, &msg_size));
    if (length < msg_size) {
        return BCM_E_MEMORY;
    }

    /* Copy buffer data */
    buffer_old = msg->start;
    sal_memcpy(buffer, buffer_old, msg_size);

    /* Update buffer information */
    msg->start       = buffer;
    msg->buffer_end  = buffer + length;
    msg->end         = buffer + (msg->end - buffer_old);
    msg->cur_ptr     = buffer + (msg->cur_ptr - buffer_old);
    if (msg->tlv_length_ptr != NULL) {
        msg->tlv_length_ptr = buffer + (msg->tlv_length_ptr - buffer_old);
    }

    return BCM_E_NONE;
}
