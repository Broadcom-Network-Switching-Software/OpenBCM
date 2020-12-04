/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_HASH_H__
#define __BCM_HASH_H__

#include <bcm/types.h>

/* Flex bin selection APIs valid options. */
#define BCM_HASH_FLEX_BIN_OPTION_REPLACE    (1 << 0)   /* Indicate a replace
                                                          operation. */
#define BCM_HASH_FLEX_BIN_OPTION_WITH_ID    (1 << 1)   /* Write to the entry
                                                          pointed to by the
                                                          given entry_id. */
#define BCM_HASH_FLEX_BIN_OPTION_SIZE_8BIT  (1 << 2)   /* Select 8bit instead of
                                                          16bit per bin command */

/* Hash flex bin selection command object offset. */
typedef enum bcm_hash_flex_bin_object_offset_e {
    BCM_HASH_OBJ_OFFSET_BIT0 = 0,       /* From start of the object. */
    BCM_HASH_OBJ_OFFSET_BIT4 = 1,       /* Start at bit4 of the object. */
    BCM_HASH_OBJ_OFFSET_BIT8 = 2,       /* Start at bit8 of the object. */
    BCM_HASH_OBJ_OFFSET_BIT12 = 3,      /* Start at bit12 of the object. */
    BCM_HASH_OBJ_OFFSET_BIT16 = 4,      /* Start at bit16 of the object. */
    BCM_HASH_OBJ_OFFSET_BIT20 = 5,      /* Start at bit20 of the object. */
    BCM_HASH_OBJ_OFFSET_BIT24 = 6,      /* Start at bit24 of the object. */
    BCM_HASH_OBJ_OFFSET_BIT28 = 7,      /* Start at bit28 of the object. */
    BCM_HASH_OBJ_OFFSET_BIT32 = 8,      /* Start at bit32 of the object. */
    BCM_HASH_OBJ_OFFSET_BIT36 = 9,      /* Start at bit36 of the object. */
    BCM_HASH_OBJ_OFFSET_BIT40 = 10,     /* Start at bit40 of the object. */
    BCM_HASH_OBJ_OFFSET_BIT44 = 11,     /* Start at bit44 of the object. */
    BCM_HASH_OBJ_OFFSET_BIT48 = 12,     /* Start at bit48 of the object. */
    BCM_HASH_OBJ_OFFSET_BIT52 = 13,     /* Start at bit52 of the object. */
    BCM_HASH_OBJ_OFFSET_BIT56 = 14,     /* Start at bit56 of the object. */
    BCM_HASH_OBJ_OFFSET_BIT60 = 15,     /* Start at bit60 of the object. */
    BCM_HASH_OBJ_OFFSET_BIT64 = 16,     /* Start at bit64 of the object. */
    BCM_HASH_OBJ_OFFSET_BIT68 = 17,     /* Start at bit68 of the object. */
    BCM_HASH_OBJ_OFFSET_BIT72 = 18,     /* Start at bit72 of the object. */
    BCM_HASH_OBJ_OFFSET_BIT76 = 19,     /* Start at bit76 of the object. */
    BCM_HASH_OBJ_OFFSET_BIT80 = 20,     /* Start at bit80 of the object. */
    BCM_HASH_OBJ_OFFSET_BIT84 = 21,     /* Start at bit84 of the object. */
    BCM_HASH_OBJ_OFFSET_BIT88 = 22,     /* Start at bit88 of the object. */
    BCM_HASH_OBJ_OFFSET_BIT92 = 23,     /* Start at bit92 of the object. */
    BCM_HASH_OBJ_OFFSET_BIT96 = 24,     /* Start at bit96 of the object. */
    BCM_HASH_OBJ_OFFSET_BIT100 = 25,    /* Start at bit100 of the object. */
    BCM_HASH_OBJ_OFFSET_BIT104 = 26,    /* Start at bit104 of the object. */
    BCM_HASH_OBJ_OFFSET_BIT108 = 27,    /* Start at bit108 of the object. */
    BCM_HASH_OBJ_OFFSET_BIT112 = 28,    /* Start at bit112 of the object. */
    BCM_HASH_OBJ_OFFSET_BIT116 = 29,    /* Start at bit116 of the object. */
    BCM_HASH_OBJ_OFFSET_BIT120 = 30,    /* Start at bit120 of the object. */
    BCM_HASH_OBJ_OFFSET_BIT124 = 31,    /* Start at bit124 of the object. */
    BCM_HASH_OBJ_OFFSET_MAX = 32        /* Must be last. */
} bcm_hash_flex_bin_object_offset_t;

/* Hash flex bin selection command bin ID. */
typedef enum bcm_hash_flex_bin_id_e {
    BCM_HASH_BIN_A_0 = 0,               /* ID of the BIN_A 0. */
    BCM_HASH_BIN_A_1 = 1,               /* ID of the BIN_A 1. */
    BCM_HASH_BIN_A_2 = 2,               /* ID of the BIN_A 2. */
    BCM_HASH_BIN_A_3 = 3,               /* ID of the BIN_A 3. */
    BCM_HASH_BIN_A_4 = 4,               /* ID of the BIN_A 4. */
    BCM_HASH_BIN_A_5 = 5,               /* ID of the BIN_A 5. */
    BCM_HASH_BIN_A_6 = 6,               /* ID of the BIN_A 6. */
    BCM_HASH_BIN_A_7 = 7,               /* ID of the BIN_A 7. */
    BCM_HASH_BIN_A_8 = 8,               /* ID of the BIN_A 8. */
    BCM_HASH_BIN_A_9 = 9,               /* ID of the BIN_A 9. */
    BCM_HASH_BIN_A_10 = 10,             /* ID of the BIN_A 10. */
    BCM_HASH_BIN_A_11 = 11,             /* ID of the BIN_A 11. */
    BCM_HASH_BIN_A_12 = 12,             /* ID of the BIN_A 12. */
    BCM_HASH_BIN_A_MAX = 12,            /* Max bin number in BIN_A set. */
    BCM_HASH_BIN_A_0_UPPER_HALF = 50,   /* ID for the upper half of the BIN_A 0. */
    BCM_HASH_BIN_A_1_UPPER_HALF = 51,   /* ID for the upper half of the BIN_A 1. */
    BCM_HASH_BIN_A_2_UPPER_HALF = 52,   /* ID for the upper half of the BIN_A 2. */
    BCM_HASH_BIN_A_3_UPPER_HALF = 53,   /* ID for the upper half of the BIN_A 3. */
    BCM_HASH_BIN_A_4_UPPER_HALF = 54,   /* ID for the upper half of the BIN_A 4. */
    BCM_HASH_BIN_A_5_UPPER_HALF = 55,   /* ID for the upper half of the BIN_A 5. */
    BCM_HASH_BIN_A_6_UPPER_HALF = 56,   /* ID for the upper half of the BIN_A 6. */
    BCM_HASH_BIN_A_7_UPPER_HALF = 57,   /* ID for the upper half of the BIN_A 7. */
    BCM_HASH_BIN_A_8_UPPER_HALF = 58,   /* ID for the upper half of the BIN_A 8. */
    BCM_HASH_BIN_A_9_UPPER_HALF = 59,   /* ID for the upper half of the BIN_A 9. */
    BCM_HASH_BIN_A_10_UPPER_HALF = 60,  /* ID for the upper half of the BIN_A
                                           10. */
    BCM_HASH_BIN_A_11_UPPER_HALF = 61,  /* ID for the upper half of the BIN_A
                                           11. */
    BCM_HASH_BIN_A_12_UPPER_HALF = 62,  /* ID for the upper half of the BIN_A
                                           12. */
    BCM_HASH_BIN_A_UPPER_HALF_MAX = 62, /* Max upper half bin number in the
                                           BIN_A set. */
    BCM_HASH_BIN_B_0 = 100,             /* ID of the BIN_B 0. */
    BCM_HASH_BIN_B_1 = 101,             /* ID of the BIN_B 1. */
    BCM_HASH_BIN_B_2 = 102,             /* ID of the BIN_B 2. */
    BCM_HASH_BIN_B_3 = 103,             /* ID of the BIN_B 3. */
    BCM_HASH_BIN_B_4 = 104,             /* ID of the BIN_B 4. */
    BCM_HASH_BIN_B_5 = 105,             /* ID of the BIN_B 5. */
    BCM_HASH_BIN_B_6 = 106,             /* ID of the BIN_B 6. */
    BCM_HASH_BIN_B_7 = 107,             /* ID of the BIN_B 7. */
    BCM_HASH_BIN_B_8 = 108,             /* ID of the BIN_B 8. */
    BCM_HASH_BIN_B_9 = 109,             /* ID of the BIN_B 9. */
    BCM_HASH_BIN_B_10 = 110,            /* ID of the BIN_B 10. */
    BCM_HASH_BIN_B_11 = 111,            /* ID of the BIN_B 11. */
    BCM_HASH_BIN_B_12 = 112,            /* ID of the BIN_B 12. */
    BCM_HASH_BIN_B_MAX = 112,           /* Max bin number in BIN_B set. */
    BCM_HASH_BIN_B_0_UPPER_HALF = 150,  /* ID for the upper half of the BIN_B 0. */
    BCM_HASH_BIN_B_1_UPPER_HALF = 151,  /* ID for the upper half of the BIN_B 1. */
    BCM_HASH_BIN_B_2_UPPER_HALF = 152,  /* ID for the upper half of the BIN_B 2. */
    BCM_HASH_BIN_B_3_UPPER_HALF = 153,  /* ID for the upper half of the BIN_B 3. */
    BCM_HASH_BIN_B_4_UPPER_HALF = 154,  /* ID for the upper half of the BIN_B 4. */
    BCM_HASH_BIN_B_5_UPPER_HALF = 155,  /* ID for the upper half of the BIN_B 5. */
    BCM_HASH_BIN_B_6_UPPER_HALF = 156,  /* ID for the upper half of the BIN_B 6. */
    BCM_HASH_BIN_B_7_UPPER_HALF = 157,  /* ID for the upper half of the BIN_B 7. */
    BCM_HASH_BIN_B_8_UPPER_HALF = 158,  /* ID for the upper half of the BIN_B 8. */
    BCM_HASH_BIN_B_9_UPPER_HALF = 159,  /* ID for the upper half of the BIN_B 9. */
    BCM_HASH_BIN_B_10_UPPER_HALF = 160, /* ID for the upper half of the BIN_B
                                           10. */
    BCM_HASH_BIN_B_11_UPPER_HALF = 161, /* ID for the upper half of the BIN_B
                                           11. */
    BCM_HASH_BIN_B_12_UPPER_HALF = 162, /* ID for the upper half of the BIN_B
                                           12. */
    BCM_HASH_BIN_B_UPPER_HALF_MAX = 162 /* Max upper half bin number in the
                                           BIN_B set. */
} bcm_hash_flex_bin_id_t;

/* Hash flex bin selection command object IDs. */
typedef enum bcm_hash_flex_bin_object_id_e {
    BCM_HASH_OBJ_SGPP = 0,              /* Source Global Physical Port. */
    BCM_HASH_OBJ_SVP = 1,               /* Source VP. */
    BCM_HASH_OBJ_OUTER_PROTO = 2,       /* Outer header IP protocol. */
    BCM_HASH_OBJ_HG_LBID = 3,           /* HG Load Balance ID. */
    BCM_HASH_OBJ_OUTER_SIP = 4,         /* Outer IP source address. */
    BCM_HASH_OBJ_OUTER_DIP = 5,         /* Outer IP destination address. */
    BCM_HASH_OBJ_OUTER_SIP_NORMALIZED = 6, /* Outer SIP normalized. */
    BCM_HASH_OBJ_OUTER_DIP_NORMALIZED = 7, /* Outer DIP normalized. */
    BCM_HASH_OBJ_INNER_SIP = 8,         /* Inner SIP. */
    BCM_HASH_OBJ_INNER_DIP = 9,         /* Inner DIP. */
    BCM_HASH_OBJ_INNER_SIP_NORMALIZED = 10, /* Inner SIP normalized. */
    BCM_HASH_OBJ_INNER_DIP_NORMALIZED = 11, /* Inner DIP normalized. */
    BCM_HASH_OBJ_OUTER_L4_SRC_PORT = 12, /* Outer L4 source port. */
    BCM_HASH_OBJ_OUTER_L4_DEST_PORT = 13, /* Outer L4 dest port. */
    BCM_HASH_OBJ_INNER_L4_SRC_PORT = 14, /* Inner L4 source port. */
    BCM_HASH_OBJ_INNER_L4_DEST_PORT = 15, /* Inner L4 dest port. */
    BCM_HASH_OBJ_OUTER_L4_SRC_PORT_NORMALIZED = 16, /* Outer L4 source port normalized. */
    BCM_HASH_OBJ_OUTER_L4_DEST_PORT_NORMALIZED = 17, /* Outer L4 dest port normalized. */
    BCM_HASH_OBJ_INNER_L4_SRC_PORT_NORMALIZED = 18, /* Inner L4 source port normalized. */
    BCM_HASH_OBJ_INNER_L4_DEST_PORT_NORMALIZED = 19, /* Inner L4 dest port normalized. */
    BCM_HASH_OBJ_INNER_PROTO = 20,      /* Inner IP protocol. */
    BCM_HASH_OBJ_MPLS_LABEL_1 = 21,     /* MPLS label 1. */
    BCM_HASH_OBJ_MPLS_LABEL_2 = 22,     /* MPLS label 2. */
    BCM_HASH_OBJ_MPLS_LABEL_3 = 23,     /* MPLS label 3. */
    BCM_HASH_OBJ_MPLS_LABEL_4 = 24,     /* MPLS label 4. */
    BCM_HASH_OBJ_MPLS_LABEL_5 = 25,     /* MPLS label 5. */
    BCM_HASH_OBJ_OUTER_MAC_SA = 26,     /* MAC source address. */
    BCM_HASH_OBJ_OUTER_MAC_DA = 27,     /* MAC destination address. */
    BCM_HASH_OBJ_OUTER_VNTAG_ETAG = 28, /* VNTAG. */
    BCM_HASH_OBJ_OUTER_OTAG = 29,       /* OTAG. */
    BCM_HASH_OBJ_OUTER_ITAG = 30,       /* ITAG. */
    BCM_HASH_OBJ_OUTER_CNTAG = 31,      /* CNTAG. */
    BCM_HASH_OBJ_OUTER_ETHTYPE = 32,    /* ETHTYPE. */
    BCM_HASH_OBJ_OUTER_VXLAN = 33,      /* VXLAN 8 bytes. */
    BCM_HASH_OBJ_OUTER_IPV4_HEADER = 34, /* The first 12 bytes of IPv4 header. */
    BCM_HASH_OBJ_OUTER_IPV4_SIP = 35,   /* The IPv4 source address. */
    BCM_HASH_OBJ_OUTER_IPV4_DIP = 36,   /* The IPv4 destination address. */
    BCM_HASH_OBJ_OUTER_IPV6_DIP = 37,   /* IPv6 destination address. */
    BCM_HASH_OBJ_OUTER_IPV6_SIP = 38,   /* IPv6 source address. */
    BCM_HASH_OBJ_OUTER_IPV6_HEADER1 = 39, /* The first 4 bytes of IPv6 header. */
    BCM_HASH_OBJ_OUTER_IPV6_HEADER2 = 40, /* The second 4 bytes of IPv6 header. */
    BCM_HASH_OBJ_OUTER_GRE_HEADER = 41, /* GRE header. */
    BCM_HASH_OBJ_OUTER_L4 = 42,         /* 8 bytes of L4 field. */
    BCM_HASH_OBJ_OUTER_L5 = 43,         /* 8 bytes of L5 field. */
    BCM_HASH_OBJ_OUTER_UDF0_0 = 44,     /* first 2 byte of UDF0. */
    BCM_HASH_OBJ_OUTER_UDF0_1 = 45,     /* 2 byte UDF0. */
    BCM_HASH_OBJ_OUTER_UDF0_2 = 46,     /* 2 byte UDF0. */
    BCM_HASH_OBJ_OUTER_UDF0_3 = 47,     /* 2 byte UDF0. */
    BCM_HASH_OBJ_OUTER_UDF0_4 = 48,     /* 2 byte UDF0. */
    BCM_HASH_OBJ_OUTER_UDF0_5 = 49,     /* 2 byte UDF0. */
    BCM_HASH_OBJ_OUTER_UDF0_6 = 50,     /* 2 byte UDF0. */
    BCM_HASH_OBJ_OUTER_UDF0_7 = 51,     /* 2 byte UDF0. */
    BCM_HASH_OBJ_OUTER_UDF1_0 = 52,     /* first 2 byte of UDF1. */
    BCM_HASH_OBJ_OUTER_UDF1_1 = 53,     /* 2 byte UDF1. */
    BCM_HASH_OBJ_OUTER_UDF1_2 = 54,     /* 2 byte UDF1. */
    BCM_HASH_OBJ_OUTER_UDF1_3 = 55,     /* 2 byte UDF1. */
    BCM_HASH_OBJ_OUTER_UDF1_4 = 56,     /* 3 byte UDF1. */
    BCM_HASH_OBJ_OUTER_UDF1_5 = 57,     /* 3 byte UDF1. */
    BCM_HASH_OBJ_OUTER_UDF1_6 = 58,     /* 4 byte UDF1. */
    BCM_HASH_OBJ_OUTER_UDF1_7 = 59,     /* 4 byte UDF1. */
    BCM_HASH_OBJ_OUTER_TOS = 60,        /* IP header Type Of Service. */
    BCM_HASH_OBJ_OUTER_TTL = 61,        /* IP header TTL. */
    BCM_HASH_OBJ_INNER_MAC_SA = 62,     /* MAC source address. */
    BCM_HASH_OBJ_INNER_MAC_DA = 63,     /* MAC destination address. */
    BCM_HASH_OBJ_INNER_VNTAG_ETAG = 64, /* VNTAG. */
    BCM_HASH_OBJ_INNER_OTAG = 65,       /* OTAG. */
    BCM_HASH_OBJ_INNER_ITAG = 66,       /* ITAG. */
    BCM_HASH_OBJ_INNER_CNTAG = 67,      /* CNTAG. */
    BCM_HASH_OBJ_INNER_ETHTYPE = 68,    /* ETHTYPE. */
    BCM_HASH_OBJ_INNER_VXLAN = 69,      /* VXLAN 8 bytes. */
    BCM_HASH_OBJ_INNER_IPV4_HEADER = 70, /* The first 12 bytes of IPv4 header. */
    BCM_HASH_OBJ_INNER_IPV4_SIP = 71,   /* The IPv4 source address. */
    BCM_HASH_OBJ_INNER_IPV4_DIP = 72,   /* The IPv4 destination address. */
    BCM_HASH_OBJ_INNER_IPV6_DIP = 73,   /* IPv6 destination address. */
    BCM_HASH_OBJ_INNER_IPV6_SIP = 74,   /* IPv6 source address. */
    BCM_HASH_OBJ_INNER_IPV6_HEADER1 = 75, /* The first 4 bytes of IPv6 header. */
    BCM_HASH_OBJ_INNER_IPV6_HEADER2 = 76, /* The second 4 bytes of IPv6 header. */
    BCM_HASH_OBJ_INNER_GRE_HEADER = 77, /* GRE header. */
    BCM_HASH_OBJ_INNER_L4 = 78,         /* 8 bytes of Layer 4 field. */
    BCM_HASH_OBJ_INNER_L5 = 79,         /* 8 bytes of Layer 5 field. */
    BCM_HASH_OBJ_INNER_UDF0_0 = 80,     /* first 2 byte of UDF0. */
    BCM_HASH_OBJ_INNER_UDF0_1 = 81,     /* 2 byte UDF0. */
    BCM_HASH_OBJ_INNER_UDF0_2 = 82,     /* 2 byte UDF0. */
    BCM_HASH_OBJ_INNER_UDF0_3 = 83,     /* 2 byte UDF0. */
    BCM_HASH_OBJ_INNER_UDF0_4 = 84,     /* 2 byte UDF0. */
    BCM_HASH_OBJ_INNER_UDF0_5 = 85,     /* 2 byte UDF0. */
    BCM_HASH_OBJ_INNER_UDF0_6 = 86,     /* 2 byte UDF0. */
    BCM_HASH_OBJ_INNER_UDF0_7 = 87,     /* 2 byte UDF0. */
    BCM_HASH_OBJ_INNER_UDF1_0 = 88,     /* first 2 byte of UDF1. */
    BCM_HASH_OBJ_INNER_UDF1_1 = 89,     /* 2 byte UDF1. */
    BCM_HASH_OBJ_INNER_UDF1_2 = 90,     /* 2 byte UDF1. */
    BCM_HASH_OBJ_INNER_UDF1_3 = 91,     /* 2 byte UDF1. */
    BCM_HASH_OBJ_INNER_UDF1_4 = 92,     /* 3 byte UDF1. */
    BCM_HASH_OBJ_INNER_UDF1_5 = 93,     /* 3 byte UDF1. */
    BCM_HASH_OBJ_INNER_UDF1_6 = 94,     /* 4 byte UDF1. */
    BCM_HASH_OBJ_INNER_UDF1_7 = 95,     /* 4 byte UDF1. */
    BCM_HASH_OBJ_INNER_TOS = 96,        /* IP header Type Of Service. */
    BCM_HASH_OBJ_INNER_TTL = 97,        /* IP header TTL. */
    BCM_HASH_OBJ_SUBPORT_TAG = 98,      /* SUBPORT_TAG. */
    BCM_HASH_OBJ_ID_MAX = 99,           /* Must be last. */
    BCM_HASH_OBJ_ID_INVALID = BCM_HASH_OBJ_ID_MAX /* Same as the BCM_HASH_OBJ_ID_MAX. */
} bcm_hash_flex_bin_object_id_t;

/* Hash flex bin selection packet types. */
typedef enum bcm_hash_pkt_type_e {
    BCM_HASH_PKT_FLOW_TYPE_FRONT_PANEL = 0, /* Packet flow type: front panel port */
    BCM_HASH_PKT_FLOW_TYPE_FRONT_PANEL_SYSTEM_HDR_PROXY = 1, /* Packet flow type: front panel port,
                                           system header proxy */
    BCM_HASH_PKT_FLOW_TYPE_FRONT_PANEL_LOOPBACK_GENERIC = 2, /* Packet flow type: front panel port,
                                           loopback generic */
    BCM_HASH_PKT_FLOW_TYPE_FRONT_PANEL_LOOPBACK_TUNNEL_TERM = 3, /* Packet flow type: front panel port,
                                           loopback tunnel termination */
    BCM_HASH_PKT_FLOW_TYPE_SYSTEM_HDR = 4, /* Packet flow type: system header */
    BCM_HASH_PKT_FLOW_TYPE_SYSTEM_HDR_CONTROL = 5, /* Packet flow type: system header
                                           control */
    BCM_HASH_PKT_FLOW_TYPE_SYSTEM_HDR_UNICAST = 6, /* Packet flow type: system header
                                           unicast */
    BCM_HASH_PKT_FLOW_TYPE_SYSTEM_HDR_FLOOD = 7, /* Packet flow type: system header flood */
    BCM_HASH_PKT_FLOW_TYPE_SYSTEM_HDR_L2_MULTICAST = 8, /* Packet flow type: system header L2
                                           multicast */
    BCM_HASH_PKT_FLOW_TYPE_SYSTEM_HDR_L3_MULTICAST = 9, /* Packet flow type: system header L3
                                           multicast */
    BCM_HASH_PKT_FLOW_TYPE_TUNNEL_VXLAN = 10, /* Packet flow tunnel type: VXLAN. */
    BCM_HASH_PKT_FLOW_TYPE_TUNNEL_NVGRE = 11, /* Packet flow tunnel type: NVGRE. */
    BCM_HASH_PKT_FLOW_TYPE_TUNNEL_MIM = 12, /* Packet flow tunnel type: MIM. */
    BCM_HASH_PKT_FLOW_TYPE_TUNNEL_IPINIP = 13, /* Packet flow tunnel type: IPINIP. */
    BCM_HASH_PKT_FLOW_TYPE_TUNNEL_MPLS = 14, /* Packet flow tunnel type: MPLS. */
    BCM_HASH_PKT_FLOW_TYPE_LAYER_L2 = 15, /* Packet flow type: L2. */
    BCM_HASH_PKT_FLOW_TYPE_LAYER_L3 = 16, /* Packet flow type: L3. */
    BCM_HASH_PKT_TYPE_MAX = 17,         /* The current max number of packet
                                           types. */
    BCM_HASH_PKT_TYPE_INVALID = 128     /* The number of packet types cannot
                                           exceed this number. */
} bcm_hash_pkt_type_t;

/* Hash flex bin selection packet processing status. */
typedef enum bcm_hash_pkt_status_e {
    BCM_HASH_PKT_STATUS_GTP_ENDPOINT_ID = 0, /* GTP endpoint id */
    BCM_HASH_PKT_STATUS_L3_FORWARDED = 1, /* L3 forward status */
    BCM_HASH_PKT_STATUS_TUNNEL_TERM = 2, /* Tunnel termination status */
    BCM_HASH_PKT_STATUS_MAX = 3,        /* The current number of packet status. */
    BCM_HASH_PKT_STATUS_INVALID = 64    /* The number of status cannot exceed
                                           this number. */
} bcm_hash_pkt_status_t;

/* A list of flex objects. Do not change. Add new ones at end. */
typedef enum bcm_hash_flex_object_e {
    BCM_HASH_FLEX_OBJ_MATCH_ID_ZONE0 = 0, /* flex object match_id zone0 */
    BCM_HASH_FLEX_OBJ_MATCH_ID_ZONE1 = 1, /* flex object match_id zone1 */
    BCM_HASH_FLEX_OBJ_MATCH_ID_ZONE2 = 2, /* flex object match_id zone2 */
    BCM_HASH_FLEX_OBJ_MATCH_ID_ZONE3 = 3, /* flex object match_id zone3 */
    BCM_HASH_FLEX_OBJ_MATCH_ID_ZONE4 = 4, /* flex object match_id zone4 */
    BCM_HASH_FLEX_OBJ_FLOW_ID_0 = 5,    /* flex object flow id 0 */
    BCM_HASH_FLEX_OBJ_FLOW_ID_1 = 6,    /* flex object flow id 1 */
    BCM_HASH_FLEX_OBJ_FLOW_ID_2 = 7,    /* flex object flow id 2 */
    BCM_HASH_FLEX_OBJ_HVE_CONTROL_1 = 8, /* flex object HVE_CONTROL_1. */
    BCM_HASH_FLEX_OBJ_HVE_CONTROL_2 = 9, /* flex object HVE_CONTROL_2. */
    BCM_HASH_FLEX_OBJ_MAX = 10          /* Number of flex objects. Must be last. */
} bcm_hash_flex_object_t;

/* Size constants for array. */
#define BCM_HASH_PKT_TYPE_BIT_MAX   128        
#define BCM_HASH_PKT_STATUS_BIT_MAX 64         

/* Hash flex bin selection command */
typedef struct bcm_hash_flex_bin_cmd_s {
    bcm_hash_flex_bin_id_t bin_id;      /* The bin number for the selected
                                           object. */
    bcm_hash_flex_bin_object_id_t obj_id; /* The selected object. */
    bcm_hash_flex_bin_object_offset_t obj_offset; /* The starting bit of the object. */
    uint16 bin_mask;                    /* Mask applied on the selected bin. */
} bcm_hash_flex_bin_cmd_t;

/* Hash flex bin selection packet type configuration bitmap */
typedef struct bcm_hash_pkt_type_set_s {
    SHR_BITDCL q_bit[_SHR_BITDCLSIZE(BCM_HASH_PKT_TYPE_BIT_MAX)]; /* Packet type configuration bitmap. */
} bcm_hash_pkt_type_set_t;

/* Hash flex bin selection packet status configuration bitmap */
typedef struct bcm_hash_pkt_status_set_s {
    SHR_BITDCL s_bit[_SHR_BITDCLSIZE(BCM_HASH_PKT_STATUS_BIT_MAX)]; /* Packet processing status
                                           configuration bitmap. */
} bcm_hash_pkt_status_set_t;

#define BCM_HASH_PKT_TYPE_SET_ADD(_set, _q)  SHR_BITSET(((_set).q_bit), (_q)) 

#define BCM_HASH_PKT_TYPE_SET_REMOVE(_set, _q)  SHR_BITCLR(((_set).q_bit), (_q)) 

#define BCM_HASH_PKT_TYPE_SET_TEST(_set, _q)  SHR_BITGET(((_set).q_bit), (_q)) 

#define BCM_HASH_PKT_STATUS_SET_ADD(_set, _q)  SHR_BITSET(((_set).s_bit), (_q)) 

#define BCM_HASH_PKT_STATUS_SET_REMOVE(_set, _q)  SHR_BITCLR(((_set).s_bit), (_q)) 

#define BCM_HASH_PKT_STATUS_SET_TEST(_set, _q)  SHR_BITGET(((_set).s_bit), (_q)) 

/* Hash flex field */
typedef struct bcm_hash_flex_field_s {
    uint32 field_id;    /* Flex field ID. */
    uint32 value;       /* Flex field value. */
    uint32 mask;        /* Flex field mask. */
} bcm_hash_flex_field_t;

/* Hash flex bin selection configuration */
typedef struct bcm_hash_flex_bin_config_s {
    bcm_hash_pkt_type_set_t tset;       /* Packet type set bitmap. */
    bcm_hash_pkt_status_set_t sset;     /* Packet processing status set bitmap. */
    bcm_hash_pkt_status_set_t sset_mask; /* Packet processing status set mask
                                           bitmap. */
    uint16 sgpp_ctrl_id;                /* SGPP hash ctrl id. */
    uint16 svp_ctrl_id;                 /* SVP hash ctrl id. */
    uint16 l3iif_ctrl_id;               /* L3_IIF hash ctrl id. */
    uint16 vfi_ctrl_id;                 /* VFI hash ctrl id. */
    uint16 vlan_ctrl_id;                /* VLAN hash ctrl id. */
    uint16 sgpp_ctrl_id_mask;           /* sgpp_ctrl_id mask. */
    uint16 svp_ctrl_id_mask;            /* svp_ctrl_id mask. */
    uint16 l3iif_ctrl_id_mask;          /* l3iif_ctrl_id mask. */
    uint16 vfi_ctrl_id_mask;            /* vfi_ctrl_id mask. */
    uint16 vlan_ctrl_id_mask;           /* vlan_ctrl_id mask. */
    int entry_id;                       /* Higher entry ID has high priority. */
    uint32 option;                      /* Bitwise flags for operation options. */
} bcm_hash_flex_bin_config_t;

/* bcm_hash_flex_bin_traverse callback */
typedef int (*bcm_hash_flex_bin_traverse_cb)(
    int unit, 
    bcm_hash_flex_bin_config_t *cfg, 
    int num_bins, 
    bcm_hash_flex_bin_cmd_t *bin_cmd, 
    int num_fields, 
    bcm_hash_flex_field_t *flex_field, 
    void *user_data);

/* Initialize the bcm_hash_flex_bin_config_t structure. */
extern void bcm_hash_flex_bin_config_t_init(
    bcm_hash_flex_bin_config_t *cfg);

#ifndef BCM_HIDE_DISPATCHABLE

/* Initialize hash module. */
extern int bcm_hash_init(
    int unit);

/* Add a hash flex bin selection rule. */
extern int bcm_hash_flex_bin_add(
    int unit, 
    bcm_hash_flex_bin_config_t *cfg, 
    int num_bins, 
    bcm_hash_flex_bin_cmd_t *bin_cmd, 
    int num_fields, 
    bcm_hash_flex_field_t *flex_field);

/* Delete a flex bin selection rule for the given entry_id. */
extern int bcm_hash_flex_bin_delete(
    int unit, 
    int entry_id);

/* Delete all flex bin selection rules. */
extern int bcm_hash_flex_bin_delete_all(
    int unit);

/* Get the flex bin selection rule for the given entry_id. */
extern int bcm_hash_flex_bin_get(
    int unit, 
    bcm_hash_flex_bin_config_t *cfg, 
    int num_bins, 
    bcm_hash_flex_bin_cmd_t *bin_cmd, 
    int num_fields, 
    bcm_hash_flex_field_t *flex_field);

/* Traverse flex bin selection entries. */
extern int bcm_hash_flex_bin_traverse(
    int unit, 
    uint32 option, 
    bcm_hash_flex_bin_traverse_cb cb, 
    void *user_data);

/* Get the field id from the field name for the given object. */
extern int bcm_hash_flex_field_id_get(
    int unit, 
    bcm_hash_flex_object_t object, 
    const char *field_name, 
    uint32 *field_id);

/* Get the field name and the flex object for the given field id. */
extern int bcm_hash_flex_field_name_get(
    int unit, 
    uint32 field_id, 
    bcm_hash_flex_object_t *object, 
    int size, 
    char *field_name);

#endif /* BCM_HIDE_DISPATCHABLE */

#endif /* __BCM_HASH_H__ */
