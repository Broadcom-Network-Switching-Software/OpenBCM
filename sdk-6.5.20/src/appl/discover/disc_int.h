/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        disc_int.h
 * Purpose:     Internal header file for discovery
 */

#ifndef   _DISC_INT_H_
#define   _DISC_INT_H_

#include <shared/util.h>
#include <bcm/types.h>

#define PACK_SHORT(buf, val)     _SHR_PACK_SHORT(buf, val)
#define PACK_LONG(buf, val)      _SHR_PACK_LONG(buf, val)
#define UNPACK_SHORT(buf, val)   _SHR_UNPACK_SHORT(buf, val)
#define UNPACK_LONG(buf, val)    _SHR_UNPACK_LONG(buf, val)

#define PACK_U32_INCR(buf, val) do { \
        _SHR_PACK_LONG(buf, val); (buf) += sizeof(uint32); \
    } while (0)

#define UNPACK_U32_INCR(buf, val) do { \
        _SHR_UNPACK_LONG(buf, val); (buf) += sizeof(uint32); \
    } while (0)


/*********************************************************************
 *
 * PACKET FORMAT DESCRIPTIONS
 *
 *  Number indicated on each packet format are in bytes unit.
 *
 *  Discovery Packet Format
 *
 *                       |<---------- Discovery packet --------->|
 *      +----------------+---------------------------------------+
 *      | <cputrans-hdr> | <disc-pkt-hdr> | <disc-pkt-entry> * n |
 *      +----------------+---------------------------------------+
 *                                            <n = ENTRY_COUNT>
 *
 *
 *  <disc-pkt-hdr> -  Discovery packet header for all packet types,
 *                    probe, routing, and config packets.
 *  +-------------------------------------------------------------+
 *  | DISC_VER | RSVD_TTL | PKT_TYPE | ENTRY_COUNT | SRC_DSEQ_NUM |
 *  +-------------------------------------------------------------+
 *       1          1          1            1             4
 *
 *
 *  <disc-pkt-entry> -  There are 3 types of discovery packets,
 *                          Probe   <probe-pkt-entry>
 *                          Routing <route-pkt-entry>
 *                          Config  <cfg-pkt-entry>
 *
 *    The number of entries in the discovery packet is specified
 *    in the ENTRY_COUNT field.
 *
 *    See below for more details on each of the discovery packet types.
 *
 *
 *****************
 *
 *  Probe Packet Entry
 *
 *    <probe-pkt-entry> 
 *    +----------------------------------------+
 *    | KEY | MAC | DSEQ_NUM | RX_IDX | TX_IDX |
 *    +----------------------------------------+
 *       6     6        4        1        1
 *
 *
 *    The source dseq_num in the header is ignored in probe packets.
 *
 *    RX_IDX indicates the port on which the packet was
 *    originally received by the CPU for this entry.  It is ignored
 *    for the first entry.
 *
 *    TX_IDX indicates the stack port on which CPU KEY transmitted this pkt.
 *
 ****************
 *
 *  Routing Packet Entry
 *
 *    The base format is the same as a probe packet (discovery header
 *    is the same).
 *
 *    <route-pkt-entry>                                      cont->
 *    +---------------------------------------------------------
 *    | KEY | MAC | DSEQ_NUM | FLAGS | SLOT_ID | MASTER_PRI | ..
 *    +---------------------------------------------------------
 *       6     6        4        4        4           4
 *
 *      cont->                                                cont->
 *      ---------------------------------------------------------
 *      ..  |  APP_DATA  | NUM_UNITS | DEST_UNIT | DEST_PORT | ..
 *      ---------------------------------------------------------
 *                8           4           4           4 
 *
 *      cont->
 *       ---------------------------------------------------+
 *       .. | STK_COUNT | <mod-entry> * m | <stk-entry> * s |
 *       ---------------------------------------------------+
 *               4        <m = NUM_UNITS>   <s = STK_COUNT>
 *
 *
 *    Unless indicated, the following formats applies to all discovery
 *    versions.
 *
 *      <mod-entry> - Contains mod ID information.  The number of 
 *                    entries is specified in the NUM_UNITS 
 *                    field.  The format for each entry is as follows,
 *
 *      +------------------------+
 *      | MODID_REQ | PREF_MODID |
 *      +------------------------+
 *             4          4
 *
 *      <stk-entry> - Contains stack port information.  The number
 *                    of entries is specified in the STK_COUNT
 *                    field.  The format for each entry is as follows,
 *
 *      discovery-v0
 *      +---------------------------------------------+
 *      | SP_FLAG | TX_KEY | TX_IDX | RX_KEY | RX_IDX |
 *      +---------------------------------------------+
 *           4         6        4        6        4
 *
 *      discovery-v1
 *      +---------------------------------------------------------------+
 *      | SP_FLAG | TX_KEY | TX_IDX | RX_KEY | RX_IDX | WEIGHT | BFLAGS |
 *      +---------------------------------------------------------------+
 *           4        6        4        6        4         4       4
 *
 *       discovery-v2                                                   cont->
 *      +-------------------------------------------------------------------
 *      | SP_FLAG | TX_KEY | TX_IDX | RX_KEY | RX_IDX | WEIGHT | BFLAGS | ..
 *      +-------------------------------------------------------------------
 *          4          6        4        6        4        4       4 
 *
 *        cont->
 *        -----------------+
 *        .. | UNIT | PORT |
 *        -----------------+
 *               4     4
 *
 *  When the compiler flag DISCOVER_APP_DATA_BOARDID is set,
 *  discovery-v2 will use the APP_DATA field to set the board-id
 *  and CPU base flag information as follows,
 *
 *        discovery-v2
 *       ---------------------------------
 *        .. |      APP_DATA       | ..
 *       ---------------------------------
 *        .. | BOARDID | BASE_FLAG | .. (when DISCOVER_APP_DATA_BOARID)
 *       ---------------------------------
 *                4          4
 *
 ****************
 *
 *  Config Packet Entry
 *
 *    <config-pkt-entry> 
 *    +-----------------------+
 *    | MASTER_KEY | MSEQ_NUM |
 *    +-----------------------+
 *           6          4
 *
 *    The config packet:  Confirmation of discovery completion
 *    between the master and each other CPU in the system.  The
 *    master sends one packet to each other CPU and each CPU
 *    responds with a config packet.  The master considers discovery
 *    to be complete when it has received a cfg packet from every other
 *    CPU in the system.  Each other member of the stack considers
 *    discovery complete after it has received the cfg pkt from the
 *    master and responded with its own.
 *
 *    The base format is the same as a probe packet (discovery header
 *    is the same).
 *
 *    The "ENTRY_COUNT" in the discovery header holds the number of
 *    CPUs in the source's database.  The config packet simply holds
 *    the MASTER_KEY and that entry's discovery sequence number as
 *    calculated by the source.
 */


#define DISC_VER_OFS            0
#define DISC_RSVD_OFS           (DISC_VER_OFS + 1)
#define DISC_PROBE_TTL_OFS      DISC_RSVD_OFS
#define LOCAL_PKT_TYPE_OFS      (DISC_RSVD_OFS + 1)
#define ENTRY_COUNT_OFS         (LOCAL_PKT_TYPE_OFS + 1)
#define SRC_DSEQ_NUM_OFS        (ENTRY_COUNT_OFS + 1)
#define ENTRY_START_OFS         (SRC_DSEQ_NUM_OFS + sizeof(uint32))
#define DISC_HEADER_BYTES       ENTRY_START_OFS

/* More in header for config packets. */
#define CFG_MASTER_KEY_OFS      ENTRY_START_OFS
#define CFG_MSEQ_NUM_OFS        (CFG_MASTER_KEY_OFS + CPUDB_KEY_BYTES)
/* CFG_HEADER_BYTES includes discovery header */
#define CFG_HEADER_BYTES        (CFG_MSEQ_NUM_OFS + sizeof(uint32))

/* These are for probe only; relative to start of probe entry */
#define PROBE_KEY_OFS           0
#define PROBE_MAC_OFS           CPUDB_KEY_BYTES
#define PROBE_DSEQ_NUM_OFS      (PROBE_MAC_OFS + sizeof(bcm_mac_t))
#define PROBE_RX_IDX_OFS        (PROBE_DSEQ_NUM_OFS + sizeof(uint32))
#define PROBE_TX_IDX_OFS        (PROBE_RX_IDX_OFS + 1)

#define PROBE_ENTRY_BYTES       (PROBE_TX_IDX_OFS + 1)

/* Offset of entry from start of discovery payload */
#define PROBE_ENTRY_OFS(i)      (ENTRY_START_OFS + (i) * PROBE_ENTRY_BYTES)

/* These are for routing only; relative to start of route entry */
#define ROUTE_KEY_OFS           0
#define ROUTE_MAC_OFS           (ROUTE_KEY_OFS + CPUDB_KEY_BYTES)

#define ROUTE_DSEQ_NUM_OFS      (ROUTE_MAC_OFS + sizeof(bcm_mac_t))
#define ROUTE_FLAGS_OFS         (ROUTE_DSEQ_NUM_OFS + sizeof(uint32))
#define ROUTE_SLOT_ID_OFS       (ROUTE_FLAGS_OFS + sizeof(uint32))
#define ROUTE_MASTER_PRI_OFS    (ROUTE_SLOT_ID_OFS + sizeof(uint32))

#define ROUTE_APP_DATA_OFS      (ROUTE_MASTER_PRI_OFS + sizeof(uint32))

#define ROUTE_NUM_UNITS_OFS     (ROUTE_APP_DATA_OFS + CPUDB_APP_DATA_BYTES)
#define ROUTE_DEST_UNIT_OFS     (ROUTE_NUM_UNITS_OFS + sizeof(uint32))
#define ROUTE_DEST_PORT_OFS     (ROUTE_DEST_UNIT_OFS + sizeof(uint32))
#define ROUTE_STK_COUNT_OFS     (ROUTE_DEST_PORT_OFS + sizeof(uint32))
#define ROUTE_MOD_ID_OFS        (ROUTE_STK_COUNT_OFS + sizeof(uint32))
/* PER-Unit mod_ids_req and pref_mod_id arrays follow here */
#define ROUTE_STK_ENTRY_OFS(num_units) \
    (ROUTE_MOD_ID_OFS + (num_units) * 2 * sizeof(uint32))

#define ROUTE_BASE_BYTES(num_units) \
    (ROUTE_STK_ENTRY_OFS(num_units) - ROUTE_KEY_OFS)

#define STK_ENTRY_FLAG_OFS      0
#define STK_ENTRY_TX_KEY_OFS    (STK_ENTRY_FLAG_OFS + sizeof(uint32))
#define STK_ENTRY_TX_IDX_OFS    (STK_ENTRY_TX_KEY_OFS + CPUDB_KEY_BYTES)
#define STK_ENTRY_RX_KEY_OFS    (STK_ENTRY_TX_IDX_OFS + sizeof(uint32))
#define STK_ENTRY_RX_IDX_OFS    (STK_ENTRY_RX_KEY_OFS + CPUDB_KEY_BYTES)
#define STK_ENTRY_WEIGHT_OFS    (STK_ENTRY_RX_IDX_OFS + sizeof(uint32))
#define STK_ENTRY_BFLAGS_OFS    (STK_ENTRY_WEIGHT_OFS + sizeof(uint32))
#define STK_ENTRY_UNIT_OFS      (STK_ENTRY_BFLAGS_OFS + sizeof(uint32))
#define STK_ENTRY_PORT_OFS      (STK_ENTRY_UNIT_OFS + sizeof(uint32))

/* These are used to pre-allocate packet buffers for routing packets */
#define ROUTE_ENTRIES_PER_PKT_MAX 4    /* Max entries allowed/pkt */
#define ROUTE_PKTS_MAX            CPUDB_CPU_MAX /* Max route pkts for table */
#define ROUTE_PKT_BYTES_MAX    1500    /* Max route pkts for table */

#endif /* _DISC_INT_H_ */
