/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cpudb.h
 * Purpose:     
 */

#ifndef   _CPUDB_H_
#define   _CPUDB_H_

#include <sdk_config.h>
#include <sal/core/libc.h>
#include <sal/core/sync.h>

#include <bcm/types.h>
#include <bcm/rx.h>

#include <appl/cpudb/brd_ident.h>

/*
 * CPU Database Array Sizes
 *
 * The current CPU database array sizes determine the maximum
 * number of stacking elements as follows,
 *     
 *   CPUDB_CPU_MAX        Maximum number of stacking CPU systems
 *   CPUDB_UNITS_MAX      Maximum number of stacking units per CPU entry
 *   CPUDB_STK_PORTS_MAX  Maximum number of stack ports per CPU entry
 */

#ifndef CPUDB_CPU_MAX
#define CPUDB_CPU_MAX        64                /* Max CPUs */
#endif
#ifndef CPUDB_UNITS_MAX
#define CPUDB_UNITS_MAX      BCM_MAX_NUM_UNITS /* Max units per CPU */
#endif
#ifndef CPUDB_CXN_MAX
#define CPUDB_CXN_MAX        32                /* Max stack ports per CPU */
#endif
#ifndef CPUDB_STK_PORTS_MAX
#define CPUDB_STK_PORTS_MAX  CPUDB_CXN_MAX     /* Max stack ports per CPU (same
                                                  as above, name more clear) */
#endif

/*
 * CPUDB Key Definition.
 *
 * Current implementation is MAC as key to database; If this
 * is changed, the following macros need to be updated.
 *
 *     cpudb_key_t                  The key data type
 *     CPUDB_KEY_BYTES              Number of bytes when packed in buffer
 *     CPUDB_KEY_PACK(buf, _key)    Pack the key into packet data
 *     CPUDB_KEY_UNPACK(buf, _key)  Unpack the key from packet data
 *     CPUDB_KEY_COMPARE(k1, k2)    Compare like memcmp
 *     CPUDB_KEY_COPY(dst, src)     Copy src key to dst key
 *     CPUDB_KEY_HASH(_key)         Returns hash value between 0 and
 *                                  CPUDB_HASH_ENTRY_COUNT - 1
 *     CPUDB_KEY_SEARCH(db_ref, _key, entry)
 *                                  Search for key; if found entry is set;
 *                                  Otherwise, entry is set to NULL.
 */

typedef struct cpudb_key_s {
    bcm_mac_t key;
} cpudb_key_t;

#define CPUDB_KEY_BYTES             sizeof(cpudb_key_t)
#define CPUDB_KEY_PACK(buf, _key)   sal_memcpy(buf, &(_key), CPUDB_KEY_BYTES)
#define CPUDB_KEY_UNPACK(buf, _key) sal_memcpy(&(_key), buf, CPUDB_KEY_BYTES)

/* Compare must have same semantics as memcmp and be 0 when == */
#define CPUDB_KEY_COMPARE(k1, k2) sal_memcmp(&(k1), &(k2), CPUDB_KEY_BYTES)
#define CPUDB_KEY_EQUAL(k1, k2)	  (CPUDB_KEY_COMPARE(k1,k2) == 0)
#define CPUDB_KEY_COPY(dst, src)  (dst) = (src)

/* Hash function for keys.  Testing indicates reasonable distribution */
#define CPUDB_HASH_ENTRY_COUNT      31  /* prime */
#define CPUDB_KEY_HASH(_key)         \
    ((((    ((_key).key[2] << 8) | (_key).key[3])) ^      \
      ((    ((_key).key[4] << 8) | (_key).key[5]))) % CPUDB_HASH_ENTRY_COUNT)

/* max length of string formatted key (see key_format) */
#ifndef CPUDB_KEY_STRING_LEN
#define	CPUDB_KEY_STRING_LEN		18
#endif

/* These are used in print statements as follows:
 *     DEBUGK((DK_VERBOSE, "src key: " CPUDB_KEY_FMT,
 *             CPUDB_KEY_DISP(src_key)));
 */

#ifndef CPUDB_KEY_FMT
#define CPUDB_KEY_FMT "%x:%x"
#endif
#ifndef CPUDB_KEY_FMT_EOLN
#define CPUDB_KEY_FMT_EOLN "%x:%x\n"
#endif
#ifndef CPUDB_KEY_DISP
#define CPUDB_KEY_DISP(_key) (_key).key[4], (_key).key[5]
#endif

/*
 * Two keys must be reserved as special values for transports:
 *     Broadcast key:  For broadcasting packets.
 *     Neighbor key:   For sending pkts to a neighbor before the
 *                     key of that neighbor is known.
 */
extern cpudb_key_t cpudb_bcast_key;
#ifndef CPUDB_BCAST_KEY_DECLARATION
#define CPUDB_BCAST_KEY_DECLARATION \
    cpudb_key_t cpudb_bcast_key = {{0xff, 0xff, 0xff, 0xff, 0xff, 0xff}}
#endif

#define CPUDB_KEY_BCAST_SET(_key)     CPUDB_KEY_COPY(_key, cpudb_bcast_key)
#define CPUDB_KEY_BCAST_COMPARE(_key) CPUDB_KEY_COMPARE(_key, cpudb_bcast_key)
#define CPUDB_KEY_IS_BCAST(_key)      CPUDB_KEY_EQUAL(_key, cpudb_bcast_key)
#define CPUDB_KEY_BCAST_PACK(buf)     CPUDB_KEY_PACK(buf, cpudb_bcast_key)

extern cpudb_key_t cpudb_neighbor_key;
#ifndef CPUDB_NEIGHBOR_KEY_DECLARATION
#define CPUDB_NEIGHBOR_KEY_DECLARATION \
    cpudb_key_t cpudb_neighbor_key = {{0xff, 0xff, 0xff, 0xff, 0xff, 0xfe}}
#endif

#define CPUDB_KEY_NEIGHBOR_SET(_key)      \
    CPUDB_KEY_COPY(_key, cpudb_neighbor_key)
#define CPUDB_KEY_NEIGHBOR_COMPARE(_key)  \
    CPUDB_KEY_COMPARE(_key, cpudb_neighbor_key)
#define CPUDB_KEY_NEIGHBOR_PACK(buf)     \
    CPUDB_KEY_PACK(buf, cpudb_neighbor_key)

/*
 * The base CPU DB entry provides a free format buffer of
 * data for applications to use, especially for discovery and
 * master election.  CPUDB_APP_DATA_BYTES determines the size of
 * this buffer.
 */
#ifndef CPUDB_APP_DATA_BYTES
#define CPUDB_APP_DATA_BYTES 8
#endif

/* Forward type defs */
typedef struct cpudb_s cpudb_t;
typedef cpudb_t *cpudb_ref_t;
#define CPUDB_REF_NULL NULL

/*
 * Stack port information structure; see discovery.txt
 *
 *        flags           Prefixed by CPUDB_SPF_
 *            NO_LINK              Port should be ignored as it doesn't
 *                                 have link.
 *            DUPLEX               Is this link (physically) duplex?  This may
 *                                 be relayed to next hop for efficiency.
 *            TX_RESOLVED          Used in discovery process
 *            RX_RESOLVED          Used in discovery process
 *            ON_BOARD             Does the port connect to a local device?
 *                                 Does not support split on/off board ports.
 *            The following flags are used by topology:
 *            TX_ENABLED           Port is used for TX; (force enable)
 *            RX_ENABLED           Port is used for RX; (force enable)
 *            TX_DISABLE_FORCE     Force the TX of the port disabled;
 *            RX_DISABLE_FORCE     Force the RX of the port disabled;
 *            INACTIVE             Link may be up, but no pkts seen during
 *                                 discovery.
 *            CUT_PORT             See topology.c for more details.
 *                                 A spanning tree is generated and those
 *                                 edges not in the tree are marked as cut
 *                                 ports.  These are used to block BC/MC.
 *            TOPO1                Reserved for topo processing.
 *        tx_cpu_idx      What is the index (in this DB) of the CPU
 *                            subsystem that will receive packets sent
 *                            on this stack link?
 *        tx_stk_idx      What is the stack port index (in this DB) for
 *                            the destination subsystem.
 *        rx_cpu_idx      What is the index (in this DB) of the CPU
 *                            subsystem from which packets are received
 *                            on this link.  Ignored if the DUPLEX flag
 *                            is set.
 *        rx_stk_idx      What is the stack port index (in this DB) for
 *                            the rx_cpu_idx entry?
 */

typedef struct cpudb_stk_port_s {
    uint32 flags;               /* See CPUDB_SPF* below */
    cpudb_key_t tx_cpu_key;
    int tx_stk_idx;
    cpudb_key_t rx_cpu_key;
    int rx_stk_idx;
} cpudb_stk_port_t;

#define CPUDB_SPF_NO_LINK           0x1
#define CPUDB_SPF_DUPLEX            0x2
#define CPUDB_SPF_TX_RESOLVED       0x4
#define CPUDB_SPF_RX_RESOLVED       0x8
#define CPUDB_SPF_ON_BOARD          0x10
#define CPUDB_SPF_TX_ENABLED        0x20
#define CPUDB_SPF_RX_ENABLED        0x40
#define CPUDB_SPF_TX_DISABLE_FORCE  0x80
#define CPUDB_SPF_RX_DISABLE_FORCE  0x100
#define CPUDB_SPF_INACTIVE          0x200
#define CPUDB_SPF_CUT_PORT          0x400
#define CPUDB_SPF_TOPO1             0x800
#define CPUDB_SPF_ETHERNET          0x1000
#define CPUDB_SPF_TX_FORWARDED      0x2000

/* A stack port is "on board" if the tx cxn CPU is the same as it's own CPU */
#define CPUDB_ONBOARD_CXN(_sp) \
    (((_sp)->flags & CPUDB_SPF_ON_BOARD) != 0)


/*
 * Base information for a CPU DB entry.  This is the information
 * that needs to be filled out before discovery can start.
 *
 * Each stack port has information stored in a cpudb_unit_port_t
 * structure including:
 *     unit/port         - The local, physical unit and port number
 *                         of the stack port.
 *     weight            - May be used in topology calculations.  Frequently
 *                         this is just the speed.  Use of this is to
 *                         be implemented.
 *     bflags            - (Base) Flags related to the port state
 *        DISABLE_IF_CUT   Cut ports may require different treatment
 *                         depending on the system's characteristics.
 *                         This flag indicates that if a port is 
 *                         marked as a CUT port, then it should not
 *                         be allowed to convey anything except
 *                         stacking management traffic.
 *
 * Historical note:  unit_port_t started as a structure containing only the
 * unit and port.  It was necessary to add the other members to this structure
 * to ensure they are conveyed during discovery.
 *
 * The other base information is:
 *
 *     key               - Unique key identifying the entry
 *     mac               - MAC address of CPU entry
 *     dseq_num;         - Discovery sequence number
 *
 *     slot_id           - For chassis; can be used for stack order (optional)
 *     master_pri        - Master priority 
 *     app_data          - A string of application data
 *
 *     num_units         - Number of physical units (devices) controlled
 *     dest_unit         - Reach this CPU addressing this unit.  The actual
 *                         mod id to use is the base mod id for this unit.
 *     dest_port         - Reach this CPU addressing this physical port.
 *                         This is the absolute port reference for the
 *                         CPU port on dest_unit.  It may be remapped
 *                         in entry->dest_port.
 *
 *     num_stk_ports     - How many external stack ports in subsystem
 *
 *              variable length info
 *     stk_ports         - Physical unit/ports for stack ports.
 *     pref_mod_id       - Per device, what is preferred mod id.
 *     mod_ids_req       - Per device, how many mod ids required
 *     board_id          - Board identifier
 *     flags             - Application flags
 */

typedef struct cpudb_unit_port_s {
    int unit;
    bcm_port_t port;
    uint32 weight;
    uint32 bflags;
} cpudb_unit_port_t;

#define CPUDB_UPF_DISABLE_IF_CUT    0x1

typedef struct cpudb_base_s cpudb_base_t;
struct cpudb_base_s {
    cpudb_key_t key;
    bcm_mac_t mac;
    volatile int dseq_num;      /* Discovery sequence number */

    int slot_id;          /* For chassis; can be used for stack order */
    int master_pri;       /* Master priority */
    uint8 app_data[CPUDB_APP_DATA_BYTES];   /* Application discovery data */

    int num_units;        /* Number of physical units controlled */
    int dest_unit;
    int dest_port;

    /* Stack port info; also part of BASE INIT */
    int num_stk_ports;

    /* Keep variable length data at end for convenience */
    /* sp_u_p:  Stack-port unit/port; Local only */
    cpudb_unit_port_t stk_ports[CPUDB_CXN_MAX];  

    int pref_mod_id[BCM_MAX_NUM_UNITS];
    int mod_ids_req[BCM_MAX_NUM_UNITS];
    CPUDB_BOARD_ID board_id;
    uint32 flags;
};

#define CPUDB_BASE_F_CHASSIS         0x1    /* Are we in a chassis? */
#define CPUDB_BASE_F_LOAD_BALANCE    0x2    /* Should load bal be done ? */
#define CPUDB_BASE_F_CHASSIS_10SLOT  0x4    /* 10 Slot chassis backplane */
#define CPUDB_BASE_F_CHASSIS_AST     0x8    /* Use asymmetric trunks */

/*
 * A CPU entry
 *     base              - Base information above,
 *     stk_ports         - Current stack port information
 *     sysid             - System layer index (post discovery); may be index
 *
 *     tx_unit           - Send packets destined for this CPU to the
 *     tx_port           -    local SOC port (tx_unit, tx_port).
 *
 *     flags             - Prefixed by CPUDB_F_
 *         LOCAL_COMPLETE      Base info filled in for all entries
 *         GLOBAL_COMPLETE     All entries report local complete
 *
 *         BASE_INIT_DONE      For discovery; 
 *         DEST_KNOWN          Are dest_mod/port known?
 *         SYSID_KNOWN         Is sysid set?
 *         TX_KNOWN            tx_unit/port known?
 *         IS_LOCAL            Is this entry the local CPU's entry?
 *         IS_MASTER           Is this entry the master CPU's entry?
 *
 *     mod_ids           - Array indexed by device of base mod ids
 *                         Length of array is num_units.
 */

typedef int cpudb_mod_list_t[BCM_MAX_NUM_UNITS];
typedef cpudb_stk_port_t cpudb_sp_list_t[CPUDB_CXN_MAX];

typedef struct cpudb_entry_s cpudb_entry_t;
struct cpudb_entry_s {
    cpudb_base_t base;

    cpudb_sp_list_t sp_info; /* cpudb_stk_port_t[CPUDB_CXN_MAX] */

    /* Applications that modify DB entries should update flags */
    volatile uint32 flags; /* See CPUDB_F_* below */

    /* SYSID member */
    void *sysid;

    /* TX members */
    int tx_unit;           /* Local info */
    int tx_port;

    /* Destination module ID/port to use to address this CPU */
    int dest_mod;
    int dest_port;    /* May be remapped for mod-id remapping */

    /* Other info, post discovery */
    cpudb_mod_list_t mod_ids; /* int[BCM_MAX_NUM_UNITS] */

    void *user_cookie;         /* Application cookie */

    /* Administrivia */
    cpudb_ref_t db_ref;        /* To which DB do we belong? */
    cpudb_entry_t *next;
    cpudb_entry_t *prev;       /* For easier removal */
    cpudb_entry_t *h_next;     /* Hash chain linkage */
    cpudb_entry_t *h_prev;
    bcm_trans_ptr_t *trans_ptr;   /* Mainly used by c2c for setup, tx */
    int topo_idx;            /* Internal, local index for topo, 0..n-1; */
};

#define CPUDB_F_LOCAL_COMPLETE    0x1    /* For discovery; stk ports rslvd */
#define CPUDB_F_GLOBAL_COMPLETE   0x2    /* For discovery; all cpus cmplt */
#define CPUDB_F_FORWARD_MASK      0xff   /* Only these flags are forwarded */

    /* The following have local significance */
#define CPUDB_F_BASE_INIT_DONE    0x100  /* For discovery; most basic info */
#define CPUDB_F_DEST_KNOWN        0x200  /* Are dest_mod/port known? */
#define CPUDB_F_SYSID_KNOWN       0x400  /* Is sysid set? */
#define CPUDB_F_TX_KNOWN          0x800  /* tx_unit/port known? */
#define CPUDB_F_IS_LOCAL          0x1000 /* Marks the local entry */
#define CPUDB_F_IS_MASTER         0x2000 /* Marks the master entry */
#define CPUDB_F_INACTIVE_MARKED   0x4000 /* Have inactive ports been marked? */
#define CPUDB_F_CONFIG_IN         0x8000 /* Config pkt in from this CPU? */
#define CPUDB_F_TOPO1             0x10000 /* Reserved for topo processing */
#define CPUDB_F_TOPO2             0x20000 /* Reserved for topo processing */

/* Use the reserved TOPO2 flags in the CPU DB */
#define CPUDB_TOPO_VISITED(entry) ((entry)->flags & CPUDB_F_TOPO2)
#define CPUDB_TOPO_VISITED_SET(entry) ((entry)->flags |= CPUDB_F_TOPO2)
#define CPUDB_TOPO_NOT_VISITED_SET(entry) ((entry)->flags &= ~CPUDB_F_TOPO2)

/* Constants for unit modid preferences */
#define CPUDB_TOPO_MODID_ANY -1
#define CPUDB_TOPO_MODID_EVEN -2

extern cpudb_ref_t cpudb_create(void);
extern cpudb_ref_t cpudb_copy(const cpudb_ref_t src_db);
extern int cpudb_clear(cpudb_ref_t db_ref, int keep_local);
extern int cpudb_destroy(cpudb_ref_t db_ref);
extern int cpudb_valid(cpudb_ref_t db_ref);

extern int cpudb_entry_copy(cpudb_entry_t *dest, const cpudb_entry_t *src);

extern cpudb_entry_t *cpudb_entry_create(cpudb_ref_t db_ref,
                                         const cpudb_key_t key,
                                         int is_local);
extern int cpudb_local_base_info_set(cpudb_ref_t db_ref,
                                     cpudb_base_t *local_base);

extern cpudb_entry_t *cpudb_mac_lookup(cpudb_ref_t db_ref,
                                       const bcm_mac_t mac);
extern cpudb_entry_t *cpudb_sysid_lookup(cpudb_ref_t db_ref,
                                               void *sysid);
extern int cpudb_sysid_set(cpudb_ref_t db_ref, cpudb_key_t key, void *sysid,
                           int overwrite);

extern int cpudb_master_set(cpudb_ref_t db_ref, const cpudb_key_t key);
extern cpudb_entry_t *cpudb_master_get(cpudb_ref_t db_ref);
extern int cpudb_entry_count_get(cpudb_ref_t db_ref);

extern int cpudb_entry_remove(cpudb_ref_t db_ref, const cpudb_key_t key);

extern int cpudb_key_format(cpudb_key_t key, char *buf, int len);
extern int cpudb_key_parse(char *buf, cpudb_key_t *keyp);

extern int cpudb_board_id_get(const char *idstring, CPUDB_BOARD_ID *board_id);
extern int cpudb_board_idstr_get(CPUDB_BOARD_ID board_id, char **board_idstr);

extern int cpudb_sp_idx_to_slot(const cpudb_ref_t db_ref,
                                const cpudb_entry_t *entry,
                                int sp_idx, cpudb_entry_t **out_entry);

extern cpudb_entry_t *cpudb_key_lookup(cpudb_ref_t db_ref,
                                       const cpudb_key_t key);

/* An instance of a database */
struct cpudb_s {
    int num_cpus;                  /* How many active */
    cpudb_entry_t *entries;
    cpudb_entry_t *local_entry;    /* Set prior to discovery */
    cpudb_entry_t *master_entry;   /* Set by discovery */

    /* Hash for key lookups */
    cpudb_entry_t *key_hash[CPUDB_HASH_ENTRY_COUNT];

    /* Administrivia */
    void *topo_cookie;             /* Cookie for topology results */
    uint32 magic;                  /* For verifying valid */
    cpudb_ref_t old_db;            /* Previous DB in system */
};

/* Quick "get entry" from sys id; treat as const */
#define CPUDB_ENTRY_COUNT_GET(_db_ref)  (_db_ref->num_cpus)

#define CPUDB_FOREACH_ENTRY(_db_ref, _entry) \
    for (_entry = _db_ref->entries; _entry != NULL; _entry = _entry->next)

#define CPUDB_KEY_SEARCH(_db_ref, _key, _entry)                            \
    do {                                                                   \
        int _cnt;                                                          \
        for (_entry = _db_ref->key_hash[CPUDB_KEY_HASH(_key)], _cnt = 0;   \
		_entry != NULL && _cnt < CPUDB_CPU_MAX;                    \
                _entry = _entry->h_next, _cnt++) {                         \
            if (CPUDB_KEY_EQUAL(_key, _entry->base.key)) {                 \
                break;         /* Found it */                              \
            }                                                              \
        }                                                                  \
        if (_entry != NULL && !CPUDB_KEY_EQUAL(_key, _entry->base.key)) {  \
            _entry = NULL;                                                 \
        }                                                                  \
    } while (0)

#endif /* _CPUDB_H_ */
