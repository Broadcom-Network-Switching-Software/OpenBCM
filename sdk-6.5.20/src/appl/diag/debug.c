/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * socdiag debug commands
 */
#include <shared/bsl.h>
#include <appl/diag/system.h>
#include <soc/mem.h>

#include <soc/devids.h>
#include <soc/debug.h>

#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)
#ifdef BROADCOM_DEBUG

/*
 * INTERNAL: Slot header format, for chip debug only
 */
typedef struct slot_header_s {
#if defined(LE_HOST)
    uint32 timestamp:14,        /* Timestamp for this Packet */
           ipx:1,               /* IPX Packet */
           ip:1,                /* IP Packet */
           BCxMC:1,             /* This is a Broadcast or Multicast Packet */
           O:2,                 /* Router help bits */
           cell_len:6,          /* How many bytes are in this cell */
           crc:2,               /* Tells Egress how to treat CRC */
           lc:1,                /* Last Cell of Packet */
           fc:1,                /* First Cell of Packet */
           purge:1,             /* Purge this packet */
           ncl:2;               /* Next Cell Length */
    uint32 cpu_opcode:4,        /* Opcode to help out the CPU */
           _pad0:1,             /* Reserved */
           slot_size:6,         /* Slot Size */
           copy_count:5,        /* How many copies of this slot are valid */
           nsp:16;              /* Next Slot Pointer */
    uint32 ls:1,                /* Last Slot in Chain */
           src_port:5,          /* The source port for this packet */
           _pad1:7,             /* Reserved */
           slot_csum_lo:19;     /* Slot Checksum (lower bits) */
    uint32 slot_csum_hi;        /* Slot Checksum (upper bits) */
#else /* BE host */
    uint32 ncl:2,
           purge:1,
           fc:1,
           lc:1,
           crc:2,
           cell_len:6,
           O:2,
           BCxMC:1,
           ip:1,
           ipx:1,
           timestamp:14;
    uint32 nsp:16,
           copy_count:5,
           slot_size:6,
           _pad0:1,
           cpu_opcode:4;
    uint32 slot_csum_lo:19,
           _pad1:7,
           src_port:5,
           ls:1;
    uint32 slot_csum_hi;
#endif
} slot_header_t;

#endif /* BROADCOM_DEBUG */
#endif /* BCM_ESW_SUPPORT || BCM_SAND_SUPPORT */

char bkpmon_usage[] =
"Parameters: none\n\t"
"Monitors for backpressure discard messages.\n\t"
"Displays the gcccount registers when changes occur.\n";

cmd_result_t
dbg_bkpmon(int unit, args_t *a)
{
           return CMD_NOTIMPL;
}
