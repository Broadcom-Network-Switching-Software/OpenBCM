/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        tlv_msg.h
 * Purpose:     TLV Message Utility
 */

#ifndef   _TLV_MSG_H_
#define   _TLV_MSG_H_

#include <sal/types.h>
#include <bcm/types.h>


/*
 *    TLV MESSAGE
 *
 *   +--------------+-----------------------------------------+--------------+
 *   |    HEADER    |                  TLV(s)                 | TLV_TYPE_END |
 *   +--------------+-----------------------------------------+--------------+
 *
 *
 *   TLV
 *    0    7 8           23 24                     
 *   +------+--------------+-----------------------------------+
 *   | TYPE |    LENGTH    |              VALUE                |
 *   +------+--------------+-----------------------------------+
 *                         |<------------- LENGTH------------->|
 *
 *
 *   TLV Message
 *   - The TLV message header is optional and is defined by the application.
 *   - A TLV message contains 0 or more TLV elements.
 *   - A TLV message is terminated by a special TLV of type TLV_TYPE_END,
 *     and there is no length field.
 *
 *   TLV Fields
 *   Type   - indicates type of information, defined as "tlv_msg_type_t".
 *            Type 0 is reserved.
 *   Length - is the size, in bytes, of the data to follow for this element,
 *            defined as "tlv_msg_length_t".
 *   Value  - is the variable sized set of bytes which contains
 *            data for this element.
 *
 *   NOTES:
 *   - Application must know the header format and 'get' all values
 *     expected in the header before retrieving the TLVs.
 *   - Application does not need to 'get' all values in the TLV in order
 *     to move to the next TLV.
 */

typedef uint8  tlv_msg_type_t;
typedef uint16 tlv_msg_length_t;

/* TLV type to indicates end of message */
#define TLV_TYPE_END            0

typedef struct tlv_msg_s {
    uint8            *start;          /* Start of message in buffer */
    uint8            *end;            /* End of message in buffer */
    uint8            *buffer_end;     /* End of buffer */
    uint8            *cur_ptr;        /* Current ptr in buffer to write/read */
    uint8            *tlv_length_ptr; /* Ptr to current TLV length in buffer */
    tlv_msg_length_t  tlv_length;     /* Length for current TLV */
    int               tlv_left;       /* Bytes left to read in current TLV */
} tlv_msg_t;


/* TLV message struct initializer */
extern void tlv_msg_t_init(tlv_msg_t *msg);

/* TLV message buffer */
extern int tlv_msg_buffer_set(tlv_msg_t *msg, uint8 *buffer, int length);

/* TLVs */
extern int tlv_msg_add(tlv_msg_t *msg, tlv_msg_type_t type);
extern int tlv_msg_get(tlv_msg_t *msg, tlv_msg_type_t *type,
                       tlv_msg_length_t *length);

/*
 * Value adders
 *
 * Adds values into a TLV message.  If there are multiple values associated
 * with a TLV message header or a TLV type, the routine of the correct
 * type must be called for each one.
 */
extern int tlv_msg_uint8_add(tlv_msg_t *msg, uint8 value);
extern int tlv_msg_uint16_add(tlv_msg_t *msg, uint16 value);
extern int tlv_msg_uint32_add(tlv_msg_t *msg, uint32 value);
extern int tlv_msg_uint64_add(tlv_msg_t *msg, uint64 value);
extern int tlv_msg_string_add(tlv_msg_t *msg, const char *value);

/*
 * Value getters
 *
 * Gets values from a TLV message.  If there are multiple values
 * associated with a TLV message header or a TLV type, the
 * getter must be called in the same order and use the same type
 * as the corresponding adder.
 */
extern int tlv_msg_uint8_get(tlv_msg_t *msg, uint8 *value);
extern int tlv_msg_uint16_get(tlv_msg_t *msg, uint16 *value);
extern int tlv_msg_uint32_get(tlv_msg_t *msg, uint32 *value);
extern int tlv_msg_uint64_get(tlv_msg_t *msg, uint64 *value);
extern int tlv_msg_string_get(tlv_msg_t *msg, int value_max, char *value);

extern int tlv_msg_length(tlv_msg_t *msg, int *length);

extern int tlv_msg_resize(tlv_msg_t *msg, uint8 *buffer, int length);


#endif /* _TLV_MSG_H_ */
