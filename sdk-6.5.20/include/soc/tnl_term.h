/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _TUNNEL_TERMINATION_H_
#define _TUNNEL_TERMINATION_H_
#include <shared/l3.h>
#include <soc/mem.h>

#define SOC_TNL_TERM_IPV4_ENTRY_WIDTH  (1) /* 1 table entry required     */
                                           /* for IPv4 tunnel terminator.*/
#define SOC_TNL_TERM_IPV6_ENTRY_WIDTH  (4) /* 4 table entries required   */
                                           /* for IPv6 tunnel terminator.*/

#define SOC_TNL_TERM_ENTRY_MODE_SINGLE  0
#define SOC_TNL_TERM_ENTRY_MODE_DOUBLE  2
#define SOC_TNL_TERM_ENTRY_MODE_QUAD    1

#define SOC_TNL_TERM_ENTRY_WIDTH_SINGLE 1
#define SOC_TNL_TERM_ENTRY_WIDTH_DOUBLE 2
#define SOC_TNL_TERM_ENTRY_WIDTH_QUAD 4


#define SOC_TNL_TERM_IDX_TO_BLOCK_START(_idx_, _block_start_, _block_size_)    \
       (_block_start_) = (_idx_) - ((_idx_) % _block_size_); 

/*
 * Tunnel termination entry structure. 
 * contains up to SOC_TNL_TERM_ENTRY_WIDTH_MAX L3_TUNNEL table entries
 */
/*
 * Tomahawk3 uses generic Hash table with L3_TUNNEL_quad and L3_TUNNEL_DOUBLE
 * for L3 tunnel entries. The sizes of these entries are 48 and 24 bytes, and
 * will fit in this structure, however we may have to handle any changes for
 * future devices.
 */

typedef struct soc_tunnel_term_s {
    tunnel_entry_t entry_arr[SOC_TNL_TERM_IPV6_ENTRY_WIDTH];
} soc_tunnel_term_t;


/* Tunnel terminator hash key. */
typedef union soc_tnl_term_hash_key_u {
    /* IPv4/IPv6 Key fields */
    struct {
        uint8 dip[_SHR_L3_IP6_ADDRLEN];     /* Destination ip address. */
        uint8 sip[_SHR_L3_IP6_ADDRLEN];     /* Source ip address.      */
        uint8 sip_plen;                     /* Source ip prefix length.*/
        uint16 l4_src_port;                 /* L4 source port.         */
        uint16 l4_dst_port;                 /* L4 destination port.    */
        uint16 ip_protocol;                 /* Ip protocol.            */
    } ip_hash_key;

    /* MIM Key Fields */
    struct {
        uint16 sglp;                        /* MIM: SGLP               */
        uint16 bvid;                        /* MIM: BVlan              */
        sal_mac_addr_t bmacsa;              /* MIM: Bmacsa             */
    } mim_hash_key;

    /* MPLS Key Fields */
    struct {
        uint32 mpls_label;                  /* MPLS: Mpls_label        */
        uint16 module_id;                   /* MPLS: Module_id         */
        uint16 port;                        /* MPLS: Port              */
        uint16 trunk_id;                    /* MPLS: Trunk id          */
    } mpls_hash_key;

    struct {
        uint32 entry[SOC_MAX_MEM_WORDS];
    } flex_hash_key;

} soc_tnl_term_hash_key_t;


/*
 * Tunnel termination hash table. 
 */
typedef struct soc_tnl_term_hash_s {
    int         unit;
    int         entry_count;    /* Number entries in hash table */
    int         index_count;    /* Hash index max value + 1     */
    uint16      *table;         /* Hash table with 16 bit index */
    uint16      *link_table;    /* To handle collisions         */
} soc_tnl_term_hash_t;

/* Key bit width for CRC calculation. (words * 32) */
#define SOC_TNL_TERM_HASH_BIT_WIDTH    ((sizeof(soc_tnl_term_hash_key_t)) << 3)

/* 
 * Tunnel termination type tcam indexes. 
 */
typedef struct soc_tnl_term_state_s {
    int start;  /* start index for this entry priority. */
    int end;    /* End index for this entry priority. */
    int prev;   /* Previous (Lo to Hi) priority with non zero entry count. */
    int next;   /* Next (Hi to Lo) priority with non zero entry count. */
    int vent;   /* Number of valid entries. */
    int fent;   /* Number of free entries. */
} soc_tnl_term_state_t, *soc_tnl_term_state_p;

extern int soc_tunnel_term_init(int unit);
extern int soc_tunnel_term_deinit(int unit);
extern int soc_tunnel_term_insert(int unit, soc_tunnel_term_t *entry, uint32 *index);
extern int soc_tunnel_term_delete(int unit, soc_tunnel_term_t *key);
extern int soc_tunnel_term_delete_all(int unit);
extern int soc_tunnel_term_match(int unit, soc_tunnel_term_t *key,
                                   soc_tunnel_term_t *result, int *entry_index);
extern void soc_tunnel_term_sw_dump(int unit);
extern int  soc_tunnel_term_used_get(int unit);

#if defined(BCM_WARM_BOOT_SUPPORT)
extern int soc_tunnel_term_reinit(int unit);
#endif /*  BCM_WARM_BOOT_SUPPORT */

#endif	/* !_TUNNEL_TERMINATION_H_ */
