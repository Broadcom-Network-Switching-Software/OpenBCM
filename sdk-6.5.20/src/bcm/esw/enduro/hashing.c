/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        hashing.c
 * Purpose:     Hash calcualtions for trunk and ECMP packets.
 */

#include <soc/debug.h>
#include <soc/drv.h>
#include <soc/hash.h>
#include <bcm/switch.h>
#include <bcm/debug.h>
#include <bcm/error.h>

#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/switch.h>

typedef struct bcm_rtag7_base_hash_s {
    uint32 rtag7_hash16_value_a_0;      /* hash a0 result */
    uint32 rtag7_hash16_value_a_1;      /* hash a1 result */
    uint32 rtag7_hash16_value_b_0;      /* hash b0 result */
    uint32 rtag7_hash16_value_b_1;      /* hash b1 result */
    uint32 rtag7_macro_flow_id;         /* hash micro flow result */
    uint32 rtag7_port_lbn;     	        /* port LBN value from the port table*/
    uint32 rtag7_lbid_hash;             /* LBID hash calculation*/
    bcm_port_t dev_src_port;            /* Local source port */
    bcm_port_t src_port;                /* System source port */
    bcm_module_t src_modid;             /* System source modid */
    uint8 is_nonuc;                     /* Non-unicast */
    uint8 hash_a_valid;                 /* Hash A result use valid input */
    uint8 hash_b_valid;                 /* Hash B result use valid input */
    uint8 lbid_hash_valid;              /* LBID Hash value is valid */
} bcm_rtag7_base_hash_t;

#define   ETHERTYPE_IPV6 0x86dd /* ipv6 ethertype */
#define   ETHERTYPE_IPV4 0x0800 /* ipv4 ethertype */
#define   ETHERTYPE_MIN  0x0600 /* minimum ethertype for hashing */

#define   IP_PROT_TCP 0x6  /* TCP protocol number */
#define   IP_PROT_UDP 0x11 /* TCP protocol number */

#define RTAG7_L2_ONLY         0x0
#define RTAG7_UNKNOWN_HIGIG   0x1
#define RTAG7_MPLS            0x2
#define RTAG7_MIM             0x3
#define RTAG7_IPV4            0x4
#define RTAG7_IPV6            0x5
#define RTAG7_FCOE            0x6
#define RTAG7_TRILL           0x7

/* fields from TRUNK_GROUPm */
STATIC soc_field_t      _xgs_portf[8] = {
    PORT0f, PORT1f, PORT2f, PORT3f,
    PORT4f, PORT5f, PORT6f, PORT7f
};

STATIC soc_field_t      _xgs_modulef[8] = {
    MODULE0f, MODULE1f, MODULE2f, MODULE3f,
    MODULE4f, MODULE5f, MODULE6f, MODULE7f
};

STATIC soc_field_t      _xgs_hg_portf[4] = {
    HIGIG_TRUNK_PORT0f, HIGIG_TRUNK_PORT1f,
    HIGIG_TRUNK_PORT2f, HIGIG_TRUNK_PORT3f
};

STATIC int
main__en_do_rtag7_hashing(int unit,
                       bcm_switch_pkt_info_t *pkt_info,
                       bcm_rtag7_base_hash_t *hash_res)
{
    /* regular var declaration */
    uint32		hash_src_port;
    uint32		hash_src_modid;
    uint32		rtag7_a_sel;
    bcm_ip_t	        sip_a;               /* Source IPv4 address. */
    bcm_ip_t	        dip_a;               /* Destination IPv4 address. */
    uint32		rtag7_b_sel;
    bcm_ip_t	        sip_b;               /* Source IPv4 address. */
    bcm_ip_t	        dip_b;               /* Destination IPv4 address. */
    uint32		hash_field_bmap_a;   /* block A chosen bitmap*/
    uint32		hash_field_bmap_b;   /* block B chosen bitmap*/
	
    uint32 hash_word[7];
    uint32 hash[13];
    int    elem, elem_bit;
    uint32 hash_element_valid_bits;
    /* int    ip6_addrs = FALSE, ip4_addrs = FALSE; */
    int    sip_valid = FALSE, dip_valid = FALSE;
	
    uint32 hash_crc16_bisync;
    uint32 hash_xor8;
    uint32 hash_xor4;
    uint32 hash_xor2;
    uint32 hash_xor1;
    uint32 xor32;

    /* register var declaration */
    uint8 rtag7_disable_hash_ipv4_a;
    uint8 rtag7_disable_hash_ipv6_a; 
   
    uint8 rtag7_disable_hash_ipv4_b;
    uint8 rtag7_disable_hash_ipv6_b;
    uint32 rtag7_hash_seed_a;
    uint32 rtag7_hash_seed_b;	
    uint32 rtag7_hash_a0_function_select;
    uint32 rtag7_hash_b0_function_select;
	
    uint64 rtag7_hash_control;
    uint32 rtag7_hash_seed_a_reg, rtag7_hash_seed_b_reg;
    soc_reg_t sc_reg;
    soc_field_t sc_field;
    uint32 sc_val;

    SOC_IF_ERROR_RETURN
        (READ_RTAG7_HASH_CONTROLr(unit, &rtag7_hash_control));
    SOC_IF_ERROR_RETURN
        (READ_RTAG7_HASH_SEED_Ar(unit, &rtag7_hash_seed_a_reg));
    SOC_IF_ERROR_RETURN
        (READ_RTAG7_HASH_SEED_Br(unit, &rtag7_hash_seed_b_reg));

    /* RTAG7 hash registers */
    rtag7_disable_hash_ipv4_a =
        soc_reg64_field32_get(unit, RTAG7_HASH_CONTROLr, rtag7_hash_control,
                          DISABLE_HASH_IPV4_Af);
    rtag7_disable_hash_ipv6_a =
        soc_reg64_field32_get(unit, RTAG7_HASH_CONTROLr, rtag7_hash_control,
                          DISABLE_HASH_IPV6_Af);
    rtag7_disable_hash_ipv4_b =
        soc_reg64_field32_get(unit, RTAG7_HASH_CONTROLr, rtag7_hash_control,
                          DISABLE_HASH_IPV4_Bf);
    rtag7_disable_hash_ipv6_b =
        soc_reg64_field32_get(unit, RTAG7_HASH_CONTROLr, rtag7_hash_control,
                          DISABLE_HASH_IPV6_Bf);

    rtag7_hash_a0_function_select =
        soc_reg64_field32_get(unit, RTAG7_HASH_CONTROLr, rtag7_hash_control,
                          HASH_MODE_Af);
    rtag7_hash_b0_function_select =
        soc_reg64_field32_get(unit, RTAG7_HASH_CONTROLr, rtag7_hash_control,
                          HASH_MODE_Bf);

    rtag7_hash_seed_a =
        soc_reg_field_get(unit, RTAG7_HASH_SEED_Ar, rtag7_hash_seed_a_reg,
                          HASH_SEED_Af);
    rtag7_hash_seed_b =
        soc_reg_field_get(unit, RTAG7_HASH_SEED_Br, rtag7_hash_seed_b_reg,
                          HASH_SEED_Bf);

    /* RTAG7 tables */
    /* the first step is to choose the paket type and to fold the ipv6
     * addresses in case of IPv6 packet*/	

    /* option A ip folding for rtag 7 */
    if ((_BCM_SWITCH_PKT_INFO_FLAG_TEST(pkt_info, ETHERTYPE) &&
         (pkt_info->ethertype == ETHERTYPE_IPV6)) &&
               (rtag7_disable_hash_ipv6_a == 0)) {

        /* ip6_addrs = TRUE; */
        rtag7_a_sel = RTAG7_IPV6;
        
        /* This section will fold the Ipv6 address to single 32 bit address
         * by using of of the method of xor */
        if (_BCM_SWITCH_PKT_INFO_FLAG_TEST(pkt_info, SRC_IPV6)) {
            sip_a =
                ((pkt_info->sip6[12] << 24) |
                 (pkt_info->sip6[13] << 16) |
                 (pkt_info->sip6[14] << 8) |
                 (pkt_info->sip6[15])) ^
                ((pkt_info->sip6[8] << 24) |
                 (pkt_info->sip6[9] << 16) |
                 (pkt_info->sip6[10] << 8) |
                 (pkt_info->sip6[11])) ^
                ((pkt_info->sip6[4] << 24) |
                 (pkt_info->sip6[5] << 16) |
                 (pkt_info->sip6[6] << 8) |
                 (pkt_info->sip6[7])) ^
                ((pkt_info->sip6[0] << 24) |
                 (pkt_info->sip6[1] << 16) |
                 (pkt_info->sip6[2] << 8) |
                 (pkt_info->sip6[3]));
            sip_valid = TRUE;
        } else {
            sip_a = 0;
        }

        if (_BCM_SWITCH_PKT_INFO_FLAG_TEST(pkt_info, DST_IPV6)) {
            dip_a =
                ((pkt_info->dip6[12] << 24) |
                 (pkt_info->dip6[13] << 16) |
                 (pkt_info->dip6[14] << 8) |
                 (pkt_info->dip6[15])) ^
                ((pkt_info->dip6[8] << 24) |
                 (pkt_info->dip6[9] << 16) |
                 (pkt_info->dip6[10] << 8) |
                 (pkt_info->dip6[11])) ^
                ((pkt_info->dip6[4] << 24) |
                 (pkt_info->dip6[5] << 16) |
                 (pkt_info->dip6[6] << 8) |
                 (pkt_info->dip6[7])) ^
                ((pkt_info->dip6[0] << 24) |
                 (pkt_info->dip6[1] << 16) |
                 (pkt_info->dip6[2] << 8) |
                 (pkt_info->dip6[3]));
            dip_valid = TRUE;
        } else {
            dip_a = 0;
        }
    } else if ((_BCM_SWITCH_PKT_INFO_FLAG_TEST(pkt_info, ETHERTYPE) &&
                (pkt_info->ethertype == ETHERTYPE_IPV4)) && 
               (rtag7_disable_hash_ipv4_a == 0)) {

        /* ip4_addrs = TRUE; */
        rtag7_a_sel = RTAG7_IPV4;

        /* only one IPv4 hdr */
        if (_BCM_SWITCH_PKT_INFO_FLAG_TEST(pkt_info, SRC_IP)) {
            sip_a = pkt_info->sip;
            sip_valid = TRUE;
        } else {
            sip_a = 0;
        }
        if (_BCM_SWITCH_PKT_INFO_FLAG_TEST(pkt_info, DST_IP)) {
            dip_a = pkt_info->dip;
            dip_valid = TRUE;
        } else {
            dip_a = 0;
        }
    } else {
        rtag7_a_sel = RTAG7_L2_ONLY;
        sip_a = dip_a = 0;
    }

    /* End of option A ip folding for rtag 7 */

    /* option B ip folding for rtag 7 */
    if ((_BCM_SWITCH_PKT_INFO_FLAG_TEST(pkt_info, ETHERTYPE) &&
         (pkt_info->ethertype == ETHERTYPE_IPV6)) &&
               (rtag7_disable_hash_ipv6_b == 0)) {

        /* ip6_addrs = TRUE; */
        rtag7_b_sel = RTAG7_IPV6;
        
        /* This section will fold the Ipv6 address to single 32 bit address
         * by using of of the method of xor */
        if (_BCM_SWITCH_PKT_INFO_FLAG_TEST(pkt_info, SRC_IPV6)) {
            sip_b =
                ((pkt_info->sip6[12] << 24) |
                 (pkt_info->sip6[13] << 16) |
                 (pkt_info->sip6[14] << 8) |
                 (pkt_info->sip6[15])) ^
                ((pkt_info->sip6[8] << 24) |
                 (pkt_info->sip6[9] << 16) |
                 (pkt_info->sip6[10] << 8) |
                 (pkt_info->sip6[11])) ^
                ((pkt_info->sip6[4] << 24) |
                 (pkt_info->sip6[5] << 16) |
                 (pkt_info->sip6[6] << 8) |
                 (pkt_info->sip6[7])) ^
                ((pkt_info->sip6[0] << 24) |
                 (pkt_info->sip6[1] << 16) |
                 (pkt_info->sip6[2] << 8) |
                 (pkt_info->sip6[3]));
            sip_valid = TRUE;
        } else {
            sip_b = 0;
        }

        if (_BCM_SWITCH_PKT_INFO_FLAG_TEST(pkt_info, DST_IPV6)) {
            dip_b =
                ((pkt_info->dip6[12] << 24) |
                 (pkt_info->dip6[13] << 16) |
                 (pkt_info->dip6[14] << 8) |
                 (pkt_info->dip6[15])) ^
                ((pkt_info->dip6[8] << 24) |
                 (pkt_info->dip6[9] << 16) |
                 (pkt_info->dip6[10] << 8) |
                 (pkt_info->dip6[11])) ^
                ((pkt_info->dip6[4] << 24) |
                 (pkt_info->dip6[5] << 16) |
                 (pkt_info->dip6[6] << 8) |
                 (pkt_info->dip6[7])) ^
                ((pkt_info->dip6[0] << 24) |
                 (pkt_info->dip6[1] << 16) |
                 (pkt_info->dip6[2] << 8) |
                 (pkt_info->dip6[3]));
            dip_valid = TRUE;
        } else {
            dip_b = 0;
        }
    } else if ((_BCM_SWITCH_PKT_INFO_FLAG_TEST(pkt_info, ETHERTYPE) &&
                (pkt_info->ethertype == ETHERTYPE_IPV4)) && 
               (rtag7_disable_hash_ipv4_b == 0)) {
			   
       /* ip4_addrs = TRUE; */
        rtag7_b_sel = RTAG7_IPV4;

        /* only one IPv4 hdr */
        if (_BCM_SWITCH_PKT_INFO_FLAG_TEST(pkt_info, SRC_IP)) {
            sip_b = pkt_info->sip;
            sip_valid = TRUE;
        } else {
            sip_b = 0;
        }
        if (_BCM_SWITCH_PKT_INFO_FLAG_TEST(pkt_info, DST_IP)) {
            dip_b = pkt_info->dip;
            dip_valid = TRUE;
        } else {
            dip_b = 0;
        }
    } else {	
        rtag7_b_sel = RTAG7_L2_ONLY;
        sip_b = dip_b = 0;
    }
    /* End of option B ip folding for rtag 7 */

    /*
     * =============
     * RTAG7 HASHING 
     * =============
     */

    /* The inputs to the RTAG7 hash function were reduced in the begining
     * of this function.  
     * This logic takes the hash inputs and computes both A and B RTAG7
     * hash values.
     */

    /* Change source modid/port to PORT32 space for hashing */
    hash_src_port  = hash_res->src_port;
    hash_src_modid = hash_res->src_modid;

    /* Only if the packet was sourced by this chip, make sure to use
     * PORT32 space modid and port if dual modid is enabled
     * Need to do this because of front panel ports.
     *
     * Already handled by API gport resolve */

    /* initialize the variables */   
    sal_memset(hash,0x0,(sizeof(uint32))*13);
    sal_memset(hash_word,0x0,(sizeof(uint32))*7);
	
    hash_crc16_bisync = 0;
    hash_xor8       = 0;
    hash_xor4       = 0;
    hash_xor2       = 0;
    hash_xor1       = 0;
    xor32           = 0;

    hash[12] = 0;	
    hash[3] = hash_src_port;
    hash[2] = hash_src_modid & 0xff;
    hash[1] = 0;
    hash[0] = 0;

    hash_element_valid_bits = 0x1fff; /* Init to valid fields above */

    /* option A hash calculation for rtag 7 */
    
    if ((rtag7_a_sel == RTAG7_IPV6) || (rtag7_a_sel == RTAG7_IPV4)) {

        if (sip_valid) {
            hash[11] = (sip_a >> 16) & 0xffff;
            hash[10] =  sip_a & 0xffff;
        } else {
            hash_element_valid_bits &= ~0xc00;
        }
        if (dip_valid) {
            hash[9]  = (dip_a >> 16) & 0xffff;
            hash[8]  =  dip_a & 0xffff;
        } else {
            hash_element_valid_bits &= ~0x300;
        }

        if (_BCM_SWITCH_PKT_INFO_FLAG_TEST(pkt_info, VLAN)) {
            hash[7]  = pkt_info->vid & 0xfff;
        } else {
            hash_element_valid_bits &= ~0x80;
        }
        if (_BCM_SWITCH_PKT_INFO_FLAG_TEST(pkt_info, SRC_L4_PORT)) {
            hash[6]  = pkt_info->src_l4_port;
        } else {
            hash_element_valid_bits &= ~0x40;
        }
        if (_BCM_SWITCH_PKT_INFO_FLAG_TEST(pkt_info, DST_L4_PORT)) {
            hash[5]  = pkt_info->dst_l4_port;
        } else {
            hash_element_valid_bits &= ~0x20;
        }
        if (_BCM_SWITCH_PKT_INFO_FLAG_TEST(pkt_info, PROTOCOL)) {
            hash[4]  = pkt_info->protocol;
        } else {
            hash_element_valid_bits &= ~0x10;
        }

        if (rtag7_a_sel == RTAG7_IPV4) {  /* Use IPv4 BITMAPs */
            sc_reg = RTAG7_HASH_FIELD_BMAP_1r;
            sc_field = IPV4_FIELD_BITMAP_Af;
            LOG_VERBOSE(BSL_LS_BCM_COMMON,
                            (BSL_META_U(unit,
                                        "Unit %d - Hash calculation: Bitmap is block A IPv4 \n"),
                             unit));
        } else { /* Use IPv6 BITMAPs */
            sc_reg = RTAG7_HASH_FIELD_BMAP_2r;
            sc_field = IPV6_FIELD_BITMAP_Af;
            LOG_VERBOSE(BSL_LS_BCM_COMMON,
                            (BSL_META_U(unit,
                                        "Unit %d - Hash calculation: Bitmap is block A IPv6 \n"),
                             unit));
        }
        
    } else { /* Catch-all for RTAG7_L2_ONLY */

        /* L2 only hash key */
        if (_BCM_SWITCH_PKT_INFO_FLAG_TEST(pkt_info, SRC_MAC)) {
            hash[11] = (pkt_info->src_mac[0] << 8) | pkt_info->src_mac[1];
            hash[10] = (pkt_info->src_mac[2] << 8) | pkt_info->src_mac[3];
            hash[9] = (pkt_info->src_mac[4] << 8) | pkt_info->src_mac[5];
        } else {
            hash_element_valid_bits &= ~0xe00;
        }
        if (_BCM_SWITCH_PKT_INFO_FLAG_TEST(pkt_info, DST_MAC)) {
            hash[8] = (pkt_info->dst_mac[0] << 8) | pkt_info->dst_mac[1];
            hash[7] = (pkt_info->dst_mac[2] << 8) | pkt_info->dst_mac[3];
            hash[6] = (pkt_info->dst_mac[4] << 8) | pkt_info->dst_mac[5];
        } else {
            hash_element_valid_bits &= ~0x1c0;
        }

        if (_BCM_SWITCH_PKT_INFO_FLAG_TEST(pkt_info, ETHERTYPE)) {
            hash[5] = (pkt_info->ethertype >= ETHERTYPE_MIN) ?
                pkt_info->ethertype : 0;
        } else {
            hash_element_valid_bits &= ~0x20;
        }

        if (_BCM_SWITCH_PKT_INFO_FLAG_TEST(pkt_info, VLAN)) {
            hash[4] = pkt_info->vid & 0xfff;
        } else {
            hash_element_valid_bits &= ~0x10;
        }

        sc_reg = RTAG7_HASH_FIELD_BMAP_3r;
        sc_field = L2_FIELD_BITMAP_Af;
        LOG_VERBOSE(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "Unit %d - Hash calculation: Bitmap is block A L2\n"),
                     unit));
    }

    BCM_IF_ERROR_RETURN
        (soc_reg32_get(unit, sc_reg, REG_PORT_ANY, 0, &sc_val));
    hash_field_bmap_a = soc_reg_field_get(unit, sc_reg, sc_val, sc_field);

    hash_word[6] = rtag7_hash_seed_a;

    hash_res->hash_a_valid = TRUE;
    for (elem = 11; elem >= 0; elem--) {
        elem_bit = 1 << elem;
        if (0 != (hash_field_bmap_a & elem_bit)) {
            if (0 != (hash_element_valid_bits & elem_bit)) {
                hash_word[elem/2] |=
                    ((hash[elem] & 0xffff) << (16 * (elem % 2)));
            } else {
                hash_res->hash_a_valid = FALSE;
                break;
            }
        }
    }

    /*Calculation*/
    xor32 = hash_word[6] ^ hash_word[5] ^ hash_word[4] ^ hash_word[3] ^
        hash_word[2] ^ hash_word[1] ^ hash_word[0];

    hash_xor8 = (xor32 & 0xff) ^ ((xor32 >> 8) & 0xff) ^ ((xor32 >> 16) & 0xff) ^ ((xor32 >> 24) & 0xff);
    hash_xor4 = (hash_xor8 & 0xf) ^ ((hash_xor8 >> 4) & 0xf);
    hash_xor2 = (hash_xor4 & 0x3) ^ ((hash_xor4 >> 2) & 0x3);
    hash_xor1 = (hash_xor2 & 0x1) ^ ((hash_xor2 >> 1) & 0x1);


    hash_crc16_bisync = _shr_crc16_draco_array(hash_word, 28);

    switch (rtag7_hash_a0_function_select) {
    case 0: /* reserved */
        hash_res->rtag7_hash16_value_a_0 = 0;
        break;
    case 1: /* reserved */
        hash_res->rtag7_hash16_value_a_0 = 0;
        break;
    case 2: /* reserved */
        hash_res->rtag7_hash16_value_a_0 = 0;
        break;
    case 3: /* CRC16 BISYNC */
        hash_res->rtag7_hash16_value_a_0 = hash_crc16_bisync;
        break;
    case 4: /* CRC16_XOR1 */ 
        hash_res->rtag7_hash16_value_a_0 = hash_xor1 & 0x1;
        hash_res->rtag7_hash16_value_a_0 |= (((hash_crc16_bisync >> 8) & 0xff) << 8);
        break;
    case 5: /* CRC16_XOR2 */
        hash_res->rtag7_hash16_value_a_0 = hash_xor2 & 0x3;
        hash_res->rtag7_hash16_value_a_0 |= (((hash_crc16_bisync >> 8) & 0xff) << 8);
        break;
    case 6: /* CRC16_XOR4 */ 
        hash_res->rtag7_hash16_value_a_0 = hash_xor4 & 0xf;
        hash_res->rtag7_hash16_value_a_0 |= (((hash_crc16_bisync >> 8) & 0xff) << 8);
        break;
    case 7: /* CRC16_XOR8 */
        hash_res->rtag7_hash16_value_a_0 = hash_xor8 & 0xff;
        hash_res->rtag7_hash16_value_a_0 |= (((hash_crc16_bisync >> 8) & 0xff) << 8);
        break;
    default: /* reserved */
        hash_res->rtag7_hash16_value_a_0 = 0;
        break;
    }

    hash_res->rtag7_hash16_value_a_1 = 0;

    /* End of option A hash calculation for rtag 7 */

    hash_res->rtag7_macro_flow_id = 0;

    /* option B hash calculation for rtag 7 */

    /* initialize the variables */   
	
    sal_memset(hash,0x0,(sizeof(uint32))*13);
    sal_memset(hash_word,0x0,(sizeof(uint32))*7);
	
    hash_crc16_bisync = 0;
    hash_xor8       = 0;
    hash_xor4       = 0;
    hash_xor2       = 0;
    hash_xor1       = 0;
    xor32           = 0;
 
    hash[12] = 0;	
    hash[3] = hash_src_port;
    hash[2] = hash_src_modid & 0xff;
    hash[1] = 0;
    hash[0] = 0; 

    hash_element_valid_bits = 0x1fff; /* Init to valid fields above */

    /* option B hash calculation for rtag 7 */
    
    if ((rtag7_b_sel == RTAG7_IPV6) || (rtag7_b_sel == RTAG7_IPV4)) {
        if (sip_valid) {
            hash[11] = (sip_b >> 16) & 0xffff;
            hash[10] =  sip_b & 0xffff;
        } else {
            hash_element_valid_bits &= ~0xc00;
        }
        if (dip_valid) {
            hash[9]  = (dip_b >> 16) & 0xffff;
            hash[8]  =  dip_b & 0xffff;
        } else {
            hash_element_valid_bits &= ~0x300;
        }

        if (_BCM_SWITCH_PKT_INFO_FLAG_TEST(pkt_info, VLAN)) {
            hash[7]  = pkt_info->vid & 0xfff;
        } else {
            hash_element_valid_bits &= ~0x80;
        }
        if (_BCM_SWITCH_PKT_INFO_FLAG_TEST(pkt_info, SRC_L4_PORT)) {
            hash[6]  = pkt_info->src_l4_port;
        } else {
            hash_element_valid_bits &= ~0x40;
        }
        if (_BCM_SWITCH_PKT_INFO_FLAG_TEST(pkt_info, DST_L4_PORT)) {
            hash[5]  = pkt_info->dst_l4_port;
        } else {
            hash_element_valid_bits &= ~0x20;
        }
        if (_BCM_SWITCH_PKT_INFO_FLAG_TEST(pkt_info, PROTOCOL)) {
            hash[4]  = pkt_info->protocol;
        } else {
            hash_element_valid_bits &= ~0x10;
        }

        if (rtag7_b_sel == RTAG7_IPV4) {  /* Use IPv4 BITMAPs */
            sc_reg = RTAG7_HASH_FIELD_BMAP_1r;
            sc_field = IPV4_FIELD_BITMAP_Bf;
            LOG_VERBOSE(BSL_LS_BCM_COMMON,
                            (BSL_META_U(unit,
                                        "Unit %d - Hash calculation: Bitmap is block B IPv4\n"),
                             unit));
        } else { /* Use IPv6 BITMAPs */
            sc_reg = RTAG7_HASH_FIELD_BMAP_2r;
            sc_field = IPV6_FIELD_BITMAP_Bf;
            LOG_VERBOSE(BSL_LS_BCM_COMMON,
                            (BSL_META_U(unit,
                                        "Unit %d - Hash calculation: Bitmap is block B IPv6\n"),
                             unit));
        }
    } else { /* Catch-all for RTAG7_L2_ONLY */

        /* L2 only hash key */
        if (_BCM_SWITCH_PKT_INFO_FLAG_TEST(pkt_info, SRC_MAC)) {
            hash[11] = (pkt_info->src_mac[0] << 8) | pkt_info->src_mac[1];
            hash[10] = (pkt_info->src_mac[2] << 8) | pkt_info->src_mac[3];
            hash[9] = (pkt_info->src_mac[4] << 8) | pkt_info->src_mac[5];
        } else {
            hash_element_valid_bits &= ~0xe00;
        }
        if (_BCM_SWITCH_PKT_INFO_FLAG_TEST(pkt_info, DST_MAC)) {
            hash[8] = (pkt_info->dst_mac[0] << 8) | pkt_info->dst_mac[1];
            hash[7] = (pkt_info->dst_mac[2] << 8) | pkt_info->dst_mac[3];
            hash[6] = (pkt_info->dst_mac[4] << 8) | pkt_info->dst_mac[5];
        } else {
            hash_element_valid_bits &= ~0x1c0;
        }

        if (_BCM_SWITCH_PKT_INFO_FLAG_TEST(pkt_info, ETHERTYPE)) {
            hash[5] = (pkt_info->ethertype >= ETHERTYPE_MIN) ?
                pkt_info->ethertype : 0;
        } else {
            hash_element_valid_bits &= ~0x20;
        }

        if (_BCM_SWITCH_PKT_INFO_FLAG_TEST(pkt_info, VLAN)) {
            hash[4] = pkt_info->vid & 0xfff;
        } else {
            hash_element_valid_bits &= ~0x10;
        }

        sc_reg = RTAG7_HASH_FIELD_BMAP_3r;
        sc_field = L2_FIELD_BITMAP_Bf;
        LOG_VERBOSE(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "Unit %d - Hash calculation: Bitmap is block B L2\n"),
                     unit));
    }

    BCM_IF_ERROR_RETURN
        (soc_reg32_get(unit, sc_reg, REG_PORT_ANY, 0, &sc_val));
    hash_field_bmap_b = soc_reg_field_get(unit, sc_reg, sc_val, sc_field);

    hash_word[6] = rtag7_hash_seed_b;

    hash_res->hash_b_valid = TRUE;
    for (elem = 11; elem >= 0; elem--) {
        elem_bit = 1 << elem;
        if (0 != (hash_field_bmap_b & elem_bit)) {
            if (0 != (hash_element_valid_bits & elem_bit)) {
                hash_word[elem/2] |=
                    ((hash[elem] & 0xffff) << (16 * (elem % 2)));
            } else {
                hash_res->hash_b_valid = FALSE;
                break;
            }
        }
    }

    xor32 = hash_word[6] ^ hash_word[5] ^ hash_word[4] ^ hash_word[3] ^
        hash_word[2] ^ hash_word[1] ^ hash_word[0];

    hash_xor8 = (xor32 & 0xff) ^ ((xor32 >> 8) & 0xff) ^ ((xor32 >> 16) & 0xff) ^ ((xor32 >> 24) & 0xff);
    hash_xor4 = (hash_xor8 & 0xf) ^ ((hash_xor8 >> 4) & 0xf);
    hash_xor2 = (hash_xor4 & 0x3) ^ ((hash_xor4 >> 2) & 0x3);
    hash_xor1 = (hash_xor2 & 0x1) ^ ((hash_xor2 >> 1) & 0x1);

    hash_crc16_bisync = _shr_crc16_draco_array(hash_word, 28);

    switch (rtag7_hash_b0_function_select) {
    case 0: /* reserved */
        hash_res->rtag7_hash16_value_b_0 = 0;
        break;
    case 1: /* reserved */
        hash_res->rtag7_hash16_value_b_0 = 0;
        break;
    case 2: /* reserved */
        hash_res->rtag7_hash16_value_b_0 = 0;
        break;
    case 3: /* CRC16 BISYNC */
        hash_res->rtag7_hash16_value_b_0 = hash_crc16_bisync;
        break;
    case 4: /* CRC16_XOR1 */ 
        hash_res->rtag7_hash16_value_b_0 = hash_xor1 & 0x1;
        hash_res->rtag7_hash16_value_b_0 |= (((hash_crc16_bisync >> 8) & 0xff) << 8);
        break;
    case 5: /* CRC16_XOR2 */
        hash_res->rtag7_hash16_value_b_0 = hash_xor2 & 0x3;
        hash_res->rtag7_hash16_value_b_0 |= (((hash_crc16_bisync >> 8) & 0xff) << 8);
        break;
    case 6: /* CRC16_XOR4 */ 
        hash_res->rtag7_hash16_value_b_0 = hash_xor4 & 0xf;
        hash_res->rtag7_hash16_value_b_0 |= (((hash_crc16_bisync >> 8) & 0xff) << 8);
        break;
    case 7: /* CRC16_XOR8 */
        hash_res->rtag7_hash16_value_b_0 = hash_xor8 & 0xff;
        hash_res->rtag7_hash16_value_b_0 |= (((hash_crc16_bisync >> 8) & 0xff) << 8);
        break;
    default: /* reserved */
        hash_res->rtag7_hash16_value_b_0 = 0;
        break;
    }

    hash_res->rtag7_hash16_value_b_1 = 0;

    /* End of option B hash calculation for rtag 7 */
    return BCM_E_NONE;
}

/*----------------------------------------------------------------*/



STATIC int
select_en_hash_subfield(int hash_sub_sel, uint32 *selected_subfield,
                     bcm_rtag7_base_hash_t *hash_Base)
{
    int rv = BCM_E_NONE;
    *selected_subfield = 0;

    switch (hash_sub_sel) {
        case 0: /* select pkt hash A */
            *selected_subfield = hash_Base->rtag7_hash16_value_a_0;
            if (!hash_Base->hash_a_valid) {
                /* Missing paramter in input */
                rv = BCM_E_PARAM;
            }
            break;

        case 1: /* select pkt hash B */
            *selected_subfield = hash_Base->rtag7_hash16_value_b_0;
            if (!hash_Base->hash_b_valid) {
                /* Missing paramter in input */
                rv = BCM_E_PARAM;
            }
            break;

        case 2: /* select port LBN */
            *selected_subfield = hash_Base->rtag7_port_lbn;
            break;

        case 3: /* select destination port id or hash A */
            
            *selected_subfield = hash_Base->rtag7_hash16_value_a_0;
            if (!hash_Base->hash_a_valid) {
                /* Missing paramter in input */
                rv = BCM_E_PARAM;
            }
            break;

        case 4: /* select LBID from module header or from SW1 stage */ 
            *selected_subfield = hash_Base->rtag7_lbid_hash; 
            if (!hash_Base->lbid_hash_valid) {
                /* LBID setting isn't RTAG7 */
                rv = BCM_E_CONFIG;
            }
            break;

        case 5: /* select LBID from SW1 stage */
            *selected_subfield = hash_Base->rtag7_lbid_hash;
            if (!hash_Base->lbid_hash_valid) {
                /* LBID setting isn't RTAG7 */
                rv = BCM_E_CONFIG;
            }
            break;

        case 6:
        case 7:
            break;
    }

    return rv;
}


STATIC int
main__en_compute_lbid(int unit, bcm_rtag7_base_hash_t *hash_Base)
{
    /*regular var declaration*/
    uint32 lbid_hash_sub_sel;
    uint32 lbid_hash_offset;
    uint32 lbid_hash_value = 0;

    /*register var declaration*/
    int lbid_rtag;
    uint8 rtag7_hash_cfg_sel_lbid;
    uint32 rtag7_hash_lbid_reg;
    int pc_out, sc_val, lbid_idx, rv = BCM_E_NONE;
	
    /*register read*/
    BCM_IF_ERROR_RETURN
        (bcm_esw_port_control_get(unit, hash_Base->dev_src_port,
                                  bcmPortControlLoadBalancingNumber,
                                  &pc_out)); 
    rtag7_hash_cfg_sel_lbid = pc_out;
    BCM_IF_ERROR_RETURN
        (bcm_esw_switch_control_get(unit, bcmSwitchLoadBalanceHashSelect,
                                    &sc_val));
    lbid_rtag = sc_val;
	
    /* ******************* LBID computation ************************* */

    if (lbid_rtag == 7) {
        lbid_idx = (0 != rtag7_hash_cfg_sel_lbid) ? 1 : 0;
        SOC_IF_ERROR_RETURN
            (READ_RTAG7_HASH_LBIDr(unit, lbid_idx, &rtag7_hash_lbid_reg));
        if (hash_Base->is_nonuc) {
            lbid_hash_sub_sel =
                soc_reg_field_get(unit, RTAG7_HASH_LBIDr,
                                  rtag7_hash_lbid_reg, SUB_SEL_NONUCf);
            lbid_hash_offset =
                soc_reg_field_get(unit, RTAG7_HASH_LBIDr,
                                  rtag7_hash_lbid_reg, OFFSET_NONUCf);
        } else {
            lbid_hash_sub_sel =
                soc_reg_field_get(unit, RTAG7_HASH_LBIDr,
                                  rtag7_hash_lbid_reg, SUB_SEL_UCf);
            lbid_hash_offset =
                soc_reg_field_get(unit, RTAG7_HASH_LBIDr,
                                  rtag7_hash_lbid_reg, OFFSET_UCf);
        }		
        /* Only UC hash selects apply to ECMP */
        switch (lbid_hash_sub_sel) {
		
        case 0: /* use pkt hash A */
            lbid_hash_value = hash_Base->rtag7_hash16_value_a_0;
            if (!hash_Base->hash_a_valid) {
                /* Missing paramter in input */
                rv = BCM_E_PARAM;
            }
            break;
		
        case 1: /* use pkt hash B */
            lbid_hash_value = hash_Base->rtag7_hash16_value_b_0;
            if (!hash_Base->hash_b_valid) {
                /* Missing paramter in input */
                rv = BCM_E_PARAM;
            }
            break;
		
        case 2: /* Use LBN */
            lbid_hash_value = hash_Base->rtag7_port_lbn;
            break;
		
        case 3: /* Use Destination PORTID or hash A */
            lbid_hash_value = hash_Base->rtag7_hash16_value_a_0;
            if (!hash_Base->hash_a_valid) {
                /* Missing paramter in input */
                rv = BCM_E_PARAM;
            }
            break;
		
        case 4: /* Use LBID */
            /* Higig lookup use mh_higig2_lbid */
				
            lbid_hash_value = 0;
            break;
		
        case 5:
        case 6:
        case 7:
            lbid_hash_value = 0;
            break;
        }
		
        /*
         * set up the hash value so that the LSB bits are
         * barrel shifted in case offset > 8.
         */
        lbid_hash_value |= ((lbid_hash_value & 0xffff) << 16);
		
        /* Only UC hash offsets apply to ECMP */
        lbid_hash_value = (lbid_hash_value >> lbid_hash_offset);
		
        hash_Base->rtag7_lbid_hash = (lbid_hash_value & 0xff);
        hash_Base->lbid_hash_valid = TRUE;

    } else { /* LBID rtag is 0-6 */        
        /* this function not support the rtag 0-6 */
		
        LOG_VERBOSE(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "Unit %d - Hash calculation: This function doesn't support rtag 0 6 pls change register ING_CONFIG.LBID_RTAG to value 7\n"),
                     unit));
        hash_Base->rtag7_lbid_hash = 0;
        hash_Base->lbid_hash_valid = FALSE;
    }
    return rv;
    /* ******************* END of LBID computation ************************* */
}

#ifdef INCLUDE_L3
STATIC int
compute_en_ecmp_hash(int unit, bcm_rtag7_base_hash_t *hash_Base,
                  uint32 *hash_val)
{
    /*regular var declaration*/
    uint32 hash_sub_sel;
    uint32 hash_offset;
    uint32 hash_subfield;
    uint32 rtag7_ecmp_hash_value;
    uint8 rtag7_ecmp_use_macro_flow_hash;
    uint8 ecmp_hash_use_rtag7;
    uint32 rtag7_hash_ecmp;
    uint32 hash_control;
    int pc_out, ecmp_idx;
    rtag7_flow_based_hash_entry_t rt7_fbh_entry;

    /*register read*/
    BCM_IF_ERROR_RETURN
        (bcm_esw_port_control_get(unit, hash_Base->dev_src_port,
                                  bcmPortControlECMPHashSet,
                                  &pc_out));
    ecmp_idx = (0 != pc_out) ? 1 : 0;

    SOC_IF_ERROR_RETURN
        (READ_RTAG7_HASH_ECMPr(unit, ecmp_idx, &rtag7_hash_ecmp));
    if (soc_reg_field_valid(unit, RTAG7_HASH_ECMPr, USE_FLOW_HASHf)) {
        rtag7_ecmp_use_macro_flow_hash =
            soc_reg_field_get(unit, RTAG7_HASH_ECMPr,
                              rtag7_hash_ecmp, USE_FLOW_HASHf);
    } else {
        /* A0 bincomp value */
        rtag7_ecmp_use_macro_flow_hash = 0;
    }

    SOC_IF_ERROR_RETURN
        (READ_HASH_CONTROLr(unit, &hash_control));
    ecmp_hash_use_rtag7 =
        soc_reg_field_get(unit, HASH_CONTROLr, hash_control,
                          ECMP_HASH_USE_RTAG7f);

    /* RTAG7 hash computation */

    if (rtag7_ecmp_use_macro_flow_hash) {
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, RTAG7_FLOW_BASED_HASHm, MEM_BLOCK_ANY,
                          (hash_Base->rtag7_macro_flow_id & 0xff),
                          &rt7_fbh_entry));

        /* Generating RTAG7 Macro Flow Based Hash Subsel and Offset for use in RSEL1 Stage */
        /* For future designs this logic can be placed in RSEL1 */

        hash_sub_sel = soc_mem_field32_get(unit, RTAG7_FLOW_BASED_HASHm,
                                           &rt7_fbh_entry, SUB_SEL_ECMPf);
        hash_offset  = soc_mem_field32_get(unit, RTAG7_FLOW_BASED_HASHm,
                                           &rt7_fbh_entry, OFFSET_ECMPf);

    } else if (SOC_REG_IS_VALID(unit, RTAG7_HASH_ECMPr)) {
        hash_sub_sel =
            soc_reg_field_get(unit, RTAG7_HASH_ECMPr,
                              rtag7_hash_ecmp, SUB_SELf);
        hash_offset =
            soc_reg_field_get(unit, RTAG7_HASH_ECMPr,
                              rtag7_hash_ecmp, OFFSETf);
    } else {
        /* A0 bincomp values */
        hash_sub_sel = 0;
        hash_offset  = 0;
    }

    BCM_IF_ERROR_RETURN
        (select_en_hash_subfield(hash_sub_sel, &hash_subfield, hash_Base));

    /* Barrel shift the hash subfield, then take the LSB 5 bits */
    rtag7_ecmp_hash_value = ((hash_subfield >> hash_offset) | (hash_subfield << (16 - hash_offset))) & 0x1f;
	
    /* Select between non-RTAG7 hash value and RTAG7 hash value */

    if (ecmp_hash_use_rtag7 == 0) {
        LOG_VERBOSE(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "Unit %d - Hash calculation:  non rtag7 calc not supported\n"),
                     unit));
        *hash_val =  0;
    } else {
        *hash_val =  rtag7_ecmp_hash_value;
    }

    return BCM_E_NONE;
    /* ***************** END of ECMP HASH CALCULATIONS.  ************************ */
}
#endif /* INCLUDE_L3 */

STATIC int
compute_en_rtag7_hash_trunk(int unit , bcm_rtag7_base_hash_t *hash_Base,
                         uint32 *hash_value)
{
    uint32 hash_sub_sel;
    uint32 hash_offset;    
    uint32 hash_subfield;
    uint32 rtag7_hash_trunk;
    uint32 rtag7_hash_trunk_value;
    int offset_shift;
    int pc_out, trunk_idx;
    uint32 hash_control;
    uint32 nuc_trunk_hash_use_rtag7;

    BCM_IF_ERROR_RETURN
        (bcm_esw_port_control_get(unit, hash_Base->dev_src_port,
                                  bcmPortControlTrunkHashSet,
                                  &pc_out));
    trunk_idx = (0 != pc_out) ? 1 : 0;
    SOC_IF_ERROR_RETURN
        (READ_RTAG7_HASH_TRUNKr(unit, trunk_idx, &rtag7_hash_trunk));

    /* Compute RTAG7 hash for trunking */
    if(hash_Base->is_nonuc) {
        hash_sub_sel =
            soc_reg_field_get(unit, RTAG7_HASH_TRUNKr,
                              rtag7_hash_trunk, SUB_SEL_NONUCf);
        hash_offset =
            soc_reg_field_get(unit, RTAG7_HASH_TRUNKr,
                              rtag7_hash_trunk, OFFSET_NONUCf);
        offset_shift = 0xff;
    } else {
        hash_sub_sel =
            soc_reg_field_get(unit, RTAG7_HASH_TRUNKr,
                              rtag7_hash_trunk, SUB_SEL_UCf);
        hash_offset =
            soc_reg_field_get(unit, RTAG7_HASH_TRUNKr,
                              rtag7_hash_trunk, OFFSET_UCf);
        offset_shift = 0xff;
    }        

    BCM_IF_ERROR_RETURN
        (select_en_hash_subfield(hash_sub_sel, &hash_subfield, hash_Base));

    /* Barrel shift the hash subfield, then take the LSB 8 bits */
    rtag7_hash_trunk_value = ((hash_subfield >> hash_offset) | (hash_subfield << (16 - hash_offset))) & offset_shift; 

    /* Compute the trunk_member_table index
     *  trunk_index = trunk_hash % (trunk_group_size + 1);
     *   Irsel2_rsel2__trunk_member_table_index =
     *        (trunk_base_ptr + trunk_index) & 0x7ff;
     */

    *hash_value = rtag7_hash_trunk_value;

    LOG_VERBOSE(BSL_LS_BCM_COMMON,
                (BSL_META_U(unit,
                            "Unit %d - Trunk hash_value=%d\n"),
                 unit, *hash_value));

    BCM_IF_ERROR_RETURN
        (READ_HASH_CONTROLr(unit, &hash_control));
    nuc_trunk_hash_use_rtag7 =
        soc_reg_field_get(unit, HASH_CONTROLr, hash_control,
                          NON_UC_TRUNK_HASH_USE_RTAG7f);

    if (hash_Base->is_nonuc && nuc_trunk_hash_use_rtag7 == 0) {
        LOG_VERBOSE(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "Unit %d - NonUC trunk Hash calculation:  non rtag7 calc not supported\n"),
                     unit));
        *hash_value = 0;
    }

    return BCM_E_NONE;
}
 
STATIC int
get_en_hash_trunk(int unit, int tgid, uint32 hash_index, bcm_gport_t *dst_gport)
{
    uint32 trunk_group_size;    
    uint32 rtag;
    uint32 trunk_index;
    bcm_module_t mod_id;
    bcm_port_t port_id;
    int mod_is_local;
    trunk_group_entry_t  tg_entry;
    _bcm_gport_dest_t   dest;
    
    BCM_IF_ERROR_RETURN
        (READ_TRUNK_GROUPm(unit, MEM_BLOCK_ANY, tgid, &tg_entry));

    trunk_group_size = soc_TRUNK_GROUPm_field32_get(unit, &tg_entry,
                                                    TG_SIZEf);
    rtag = soc_TRUNK_GROUPm_field32_get(unit, &tg_entry, RTAGf);
                
    if (rtag != 7){
        LOG_VERBOSE(BSL_LS_BCM_COMMON,
                     (BSL_META_U(unit,
                                 "Unit %d - Hash calculation: uport only RTAG7 calc no support for rtag %d\n"),
                      unit, rtag));
    }

    trunk_index = hash_index % (trunk_group_size + 1);

   LOG_VERBOSE(BSL_LS_BCM_COMMON,
                (BSL_META_U(unit,
                            "\tTrunk HW index 0x%08x\n"),
                 trunk_index));
    LOG_VERBOSE(BSL_LS_BCM_COMMON,
                (BSL_META_U(unit,
                            "\tTrunk group size 0x%08x\n"),
                 trunk_group_size));

    mod_id = soc_TRUNK_GROUPm_field32_get(unit, &tg_entry,
                                          _xgs_modulef[trunk_index]);
    port_id = soc_TRUNK_GROUPm_field32_get(unit, &tg_entry,
                                           _xgs_portf[trunk_index]);

    BCM_IF_ERROR_RETURN
        (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET, mod_id, port_id,
                                        &(dest.modid), &(dest.port)));
    dest.gport_type = _SHR_GPORT_TYPE_MODPORT;

    BCM_IF_ERROR_RETURN
        (_bcm_esw_modid_is_local(unit, dest.modid,
                                 &mod_is_local));
    if (mod_is_local) {
        if (IS_ST_PORT(unit, dest.port)) {
            dest.gport_type = _SHR_GPORT_TYPE_DEVPORT;
        }
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_gport_construct(unit, &dest, dst_gport));

    return BCM_E_NONE;  
}

/*
 * Function:
 *     get_en_hash_trunk_nuc
 * Description:
 *     Compute non-unicast destination trunk member port 
 * Parameters:
 *  unit - StrataSwitch PCI device unit number (driver internal).
 *  tgid - Trunk group id
 *  fwd_reason - Flow foward reason. unicast, ipmc, l2mc, broadcast, dlf.
 *  hash_index - Hash value calculated based on RTAG7
 *  dst_gport  - Where egress member port id is stored
 *
 * Returns:
 *      BCM_E_xxxx
 */
STATIC int
get_en_hash_trunk_nuc(int unit, int tgid, 
                   bcm_switch_pkt_hash_info_fwd_reason_t fwd_reason, 
                   uint32 hash_index, bcm_gport_t *dst_gport)
{
    int nuc_type;
    int nonuc_trunk_block_mask_index;
    int region_size, i, all_local;
    int member_index, member_count;
    int modid, modid_tmp, port, tid, id, mod_is_local;
    int port_index, count;
    bcm_pbmp_t pbmp, local_pbmp;
    bcm_trunk_info_t   t_add_info;
    bcm_trunk_member_t member_array[BCM_TRUNK_MAX_PORTCNT];
    trunk_bitmap_entry_t trunk_bitmap_entry;

    switch(fwd_reason) {
    case bcmSwitchPktHashInfoFwdReasonIpmc:
        nuc_type = TRUNK_BLOCK_MASK_TYPE_IPMC;
        break;
    case bcmSwitchPktHashInfoFwdReasonL2mc:
        nuc_type = TRUNK_BLOCK_MASK_TYPE_L2MC;
        break;
    case bcmSwitchPktHashInfoFwdReasonBcast:
        nuc_type = TRUNK_BLOCK_MASK_TYPE_BCAST;
        break;
    case bcmSwitchPktHashInfoFwdReasonDlf:
        nuc_type = TRUNK_BLOCK_MASK_TYPE_DLF;
        break;
    default:
        return BCM_E_PARAM;
    }
    nonuc_trunk_block_mask_index = (nuc_type << 4) | (hash_index & 0xf);

    LOG_VERBOSE(BSL_LS_BCM_COMMON,
                (BSL_META_U(unit,
                            "Unit %d - Nonuc-trunk table index = %d\n"), 
                 unit, nonuc_trunk_block_mask_index));

    region_size = soc_mem_index_count(unit, NONUCAST_TRUNK_BLOCK_MASKm) / 4;

    BCM_IF_ERROR_RETURN(
        bcm_esw_trunk_get(unit, tgid, &t_add_info, BCM_TRUNK_MAX_PORTCNT,
            member_array, &member_count));

    BCM_IF_ERROR_RETURN(
        READ_TRUNK_BITMAPm(unit, MEM_BLOCK_ANY, tgid, &trunk_bitmap_entry));
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(local_pbmp);
    soc_mem_pbmp_field_get(unit, TRUNK_BITMAPm, &trunk_bitmap_entry,
                                    TRUNK_BITMAPf, &local_pbmp);
    BCM_PBMP_COUNT(local_pbmp, count);
    all_local = FALSE;
    if (count == member_count) {
        all_local = TRUE;
    }
    if (all_local || member_count > BCM_SWITCH_TRUNK_MAX_PORTCNT) {
        member_index = 
            (nonuc_trunk_block_mask_index % region_size) % member_count;
    } else {
        member_index = 
            (nonuc_trunk_block_mask_index % BCM_XGS3_TRUNK_MAX_PORTCNT) % member_count;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, member_array[member_index].gport,
                                &modid, &port, &tid, &id));
    port_index = 0;
    for (i = 0; i < member_count; i++) {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_gport_resolve(unit, member_array[i].gport,
                                &modid_tmp, &port, &tid, &id));
        if (modid_tmp == modid) {
            if (i <= member_index) {
                port_index++;
            }
            BCM_PBMP_PORT_ADD(pbmp, port);
        }
    }

    count = 0;
    BCM_PBMP_ITER(pbmp, port) {
        count++;
        if (count == port_index) {
            break;
        }
    }

    if (count != port_index) {
        return BCM_E_FAIL;
    }
    
    BCM_IF_ERROR_RETURN
        (_bcm_esw_modid_is_local(unit, modid, &mod_is_local));

    if (mod_is_local && IS_ST_PORT(unit, port)) {
        BCM_GPORT_DEVPORT_SET(*dst_gport, unit, port);
    } else {
        BCM_GPORT_MODPORT_SET(*dst_gport, modid, port);
    }

    return BCM_E_NONE;
}

#ifdef INCLUDE_L3
STATIC int
get_en_hash_ecmp(int unit, int ecmp_group, uint32 hash_index, bcm_if_t *nh_id)
{
    ecmp_count_entry_t ecmpc_entry;
    ecmp_entry_t ecmp_entry;
    uint32 ecmp_ptr = 0;
    uint32 ecmp_count = 0;
                
    uint32 ecmp_mask;
    uint32 ecmp_offset;
    uint32 ecmp_index;

    /* pick up the ecmp count */
    BCM_IF_ERROR_RETURN
        (READ_L3_ECMP_COUNTm(unit, MEM_BLOCK_ANY, ecmp_group, &ecmpc_entry));

    /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
     * A ECMP_COUNT value of 0 implies 1 entry 
     * a value of 1 implies 2 entries etc...
     * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
     */
    ecmp_mask = 0x7ff;
    ecmp_count =
        soc_L3_ECMP_COUNTm_field32_get(unit, &ecmpc_entry,
                                           COUNTf);
    ecmp_offset = (hash_index % (ecmp_count + 1)) & 0x1F;
    LOG_VERBOSE(BSL_LS_BCM_COMMON,
                (BSL_META_U(unit,
                            "\tECMP offset 0x%08x\n"),
                 ecmp_offset));
    if (!BCM_XGS3_L3_EGRESS_MODE_ISSET(unit)) {
        ecmp_group *= BCM_XGS3_L3_ECMP_MAX_PATHS(unit);
    }
    ecmp_ptr = (ecmp_group & ecmp_mask);
    ecmp_index = (ecmp_ptr + ecmp_offset) & ecmp_mask;
    BCM_IF_ERROR_RETURN
        (READ_L3_ECMPm(unit, MEM_BLOCK_ANY, ecmp_index, &ecmp_entry));

    *nh_id =
        soc_L3_ECMPm_field32_get(unit, &ecmp_entry,
                                 NEXT_HOP_INDEXf) & 0x3fff;
    LOG_VERBOSE(BSL_LS_BCM_COMMON,
                (BSL_META_U(unit,
                            "\tECMP next hop HW index 0x%08x\n"),
                 *nh_id));

    return BCM_E_NONE;  
}
#endif /* INCLUDE_L3 */

/*
 * Function:
 *          compute_en_rtag7_hash_hg_trunk
 * Description:
 *     get HGT hash value from RTAG7_PORT_BASED_HASHm 
 *     if unicast, SUB_SEL_TRUNK_UCf and OFFSET_TRUNK_UCf are used 
 *     if not unicast, SUB_SEL_TRUNK_NONUCf and OFFSET_TRUNK_NONUCf
 *     if flow hash is used, get subsel and offset from RTAG7_FLOW_BASED_HASH 
 * Parameters:
 *          unit - StrataSwitch PCI device unit number (driver internal).
 *          hash_Base - internal data where trunk hash value is storead
 *          hash_value- result param 
 * Returns:
 *      BCM_E_xxxx
 */
STATIC int
compute_en_rtag7_hash_hg_trunk(int unit , bcm_rtag7_base_hash_t *hash_Base,
                         uint32 *hash_value)
{
    uint32 hash_sub_sel;
    uint32 hash_offset;    
    uint32 hash_subfield;
    uint32 rtag7_hash_hg_trunk;
    uint32 rtag7_hash_hg_trunk_value;
    int offset_shift;
    int pc_out, trunk_idx;

    BCM_IF_ERROR_RETURN
        (bcm_esw_port_control_get(unit, hash_Base->dev_src_port,
                                  bcmPortControlFabricTrunkHashSet,
                                  &pc_out));
    trunk_idx = (0 != pc_out) ? 1 : 0;
    SOC_IF_ERROR_RETURN
        (READ_RTAG7_HASH_HG_TRUNKr(unit, trunk_idx, &rtag7_hash_hg_trunk));

    /* Compute RTAG7 hash for trunking */
    if(hash_Base->is_nonuc) {
        hash_sub_sel =
            soc_reg_field_get(unit, RTAG7_HASH_HG_TRUNKr,
                              rtag7_hash_hg_trunk, SUB_SEL_NONUCf);
        hash_offset =
            soc_reg_field_get(unit, RTAG7_HASH_HG_TRUNKr,
                              rtag7_hash_hg_trunk, OFFSET_NONUCf);
        offset_shift = 0xff;
    } else {
        hash_sub_sel =
            soc_reg_field_get(unit, RTAG7_HASH_TRUNKr,
                              rtag7_hash_hg_trunk, SUB_SEL_UCf);
        hash_offset =
            soc_reg_field_get(unit, RTAG7_HASH_TRUNKr,
                              rtag7_hash_hg_trunk, OFFSET_UCf);
        offset_shift = 0xff;
    } 

    BCM_IF_ERROR_RETURN
        (select_en_hash_subfield(hash_sub_sel, &hash_subfield, hash_Base));

    /* Barrel shift the hash subfield, then take the LSB 8 bits */
    rtag7_hash_hg_trunk_value = ((hash_subfield >> hash_offset) | (hash_subfield << (16 - hash_offset))) & offset_shift; 

    /* Compute the trunk_member_table index
     *  trunk_index = trunk_hash % (trunk_group_size + 1);
     *   Irsel2_rsel2__trunk_member_table_index =
     *        (trunk_base_ptr + trunk_index) & 0x7ff;
     */

    *hash_value = rtag7_hash_hg_trunk_value;

    LOG_VERBOSE(BSL_LS_BCM_COMMON,
                (BSL_META_U(unit,
                            "Unit %d - HG Trunk hash_value=%d\n"),
                 unit, *hash_value));

    return BCM_E_NONE;
}

STATIC int
get_en_hash_hg_trunk(int unit, int tgid, uint32 hash_index, bcm_gport_t *dst_gport)
{
    uint64 hg_trunk_group;
    uint32 trunk_group_size;    
    uint32 rtag;
    uint32 trunk_index;
    bcm_module_t mod_id;
    bcm_port_t port_id;
    _bcm_gport_dest_t   dest;

    COMPILER_64_ZERO(hg_trunk_group);
    BCM_IF_ERROR_RETURN
        (READ_HG_TRUNK_GROUPr(unit, tgid, &hg_trunk_group));

    trunk_group_size = soc_reg64_field32_get(unit, HG_TRUNK_GROUPr,
                                             hg_trunk_group,
                                             HIGIG_TRUNK_SIZEf);

    rtag = soc_reg64_field32_get(unit, HG_TRUNK_GROUPr,
                                 hg_trunk_group,
                                 HIGIG_TRUNK_RTAGf);

    if (rtag != 7){
        LOG_VERBOSE(BSL_LS_BCM_COMMON,
                     (BSL_META_U(unit,
                                 "Unit %d - Hash calculation: support only RTAG7 calc no support for rtag %d\n"),
                      unit, rtag));
    }

    trunk_index = hash_index % (trunk_group_size + 1);

    LOG_VERBOSE(BSL_LS_BCM_COMMON,
                (BSL_META_U(unit,
                            "\tTrunk HW index 0x%08x\n"),
                 trunk_index));
    LOG_VERBOSE(BSL_LS_BCM_COMMON,
                (BSL_META_U(unit,
                            "\tTrunk group size 0x%08x\n"),
                 trunk_group_size));

    port_id = soc_reg64_field32_get(unit, HG_TRUNK_GROUPr,
                                    hg_trunk_group,
                                    _xgs_hg_portf[trunk_index]);

    if (BCM_FAILURE(bcm_esw_stk_my_modid_get(unit, &mod_id))) {
        mod_id = 0;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET, mod_id, port_id,
                                        &(dest.modid), &(dest.port)));
    dest.gport_type = _SHR_GPORT_TYPE_DEVPORT;

    BCM_IF_ERROR_RETURN(_bcm_esw_gport_construct(unit, &dest, dst_gport));

    return BCM_E_NONE;  
}

int
_bcm_en_switch_pkt_info_hash_get(int unit,
                                 bcm_switch_pkt_info_t *pkt_info,
                                 bcm_gport_t *dst_gport,
                                 bcm_if_t *dst_intf)
{
    bcm_rtag7_base_hash_t hash_res;
    int pc_out;
    bcm_gport_t     port;
    bcm_trunk_t     tid;
    int             id;
    uint32          hash_value;

    if (pkt_info == NULL) {
        return BCM_E_PARAM;
    }

    if (!_BCM_SWITCH_PKT_INFO_FLAG_TEST(pkt_info, SRC_GPORT)) {
        LOG_VERBOSE(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "Unit %d - Hash calculation: source gport value missing\n"),
                     unit));
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, pkt_info->src_gport,
                                &(hash_res.src_modid),
                                &(hash_res.src_port), 
                                &tid, &id));
    if ((-1 != id) || (-1 != tid)) {
        /* Must be single system port */
        return BCM_E_PORT;
    }

    /* Load balancing retrieval */
    BCM_IF_ERROR_RETURN
        (bcm_esw_port_local_get(unit, pkt_info->src_gport, &port));
    hash_res.dev_src_port = port;
    BCM_IF_ERROR_RETURN
        (bcm_esw_port_control_get(unit, port,
                                  bcmPortControlLoadBalancingNumber,
                                  &pc_out)); 
    hash_res.rtag7_port_lbn = pc_out;

    /* check if the foward reason of packet is all non-uc packet or bit 40 in the mac DA is one. */
    hash_res.is_nonuc = pkt_info->fwd_reason || BCM_MAC_IS_MCAST(pkt_info->dst_mac);

    BCM_IF_ERROR_RETURN
        (main__en_do_rtag7_hashing(unit, pkt_info, &hash_res));
    BCM_IF_ERROR_RETURN
        (main__en_compute_lbid(unit, &hash_res));

    LOG_VERBOSE(BSL_LS_BCM_COMMON,
                (BSL_META_U(unit,
                            "Unit %d - Hash status: \n"),
                 unit));
    if (hash_res.hash_a_valid) {
        LOG_VERBOSE(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "\tRTAG7 A0 0x%08x\n"),
                     hash_res.rtag7_hash16_value_a_0));
        LOG_VERBOSE(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "\tRTAG7 A1 0x%08x\n"),
                     hash_res.rtag7_hash16_value_a_1));
    } else {
        LOG_VERBOSE(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "\tRTAG7 A hashes invalid due to missing packet info\n")));
    }
    if (hash_res.hash_b_valid) {
        LOG_VERBOSE(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "\tRTAG7 B0 0x%08x\n"),
                     hash_res.rtag7_hash16_value_b_0));
        LOG_VERBOSE(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "\tRTAG7 B1 0x%08x\n"),
                     hash_res.rtag7_hash16_value_b_1));
    } else {
        LOG_VERBOSE(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "\tRTAG7 B hashes invalid due to missing packet info\n")));
    }
    LOG_VERBOSE(BSL_LS_BCM_COMMON,
                (BSL_META_U(unit,
                            "\tRTAG7 LBN 0x%08x\n"),
                 hash_res.rtag7_port_lbn));
    if (hash_res.lbid_hash_valid){
        LOG_VERBOSE(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "\tRTAG7 LBID 0x%08x\n"),
                     hash_res.rtag7_lbid_hash));
    } else {
        LOG_VERBOSE(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "\tRTAG7 LBID not valid due to non-RTAG7 configuration\n")));
    }

#ifdef INCLUDE_L3
    if (0 != (pkt_info->flags & BCM_SWITCH_PKT_INFO_HASH_MULTIPATH)) {
        bcm_if_t        nh_id;

        if (dst_intf == NULL) {
            return BCM_E_PARAM;
        }
        
        BCM_IF_ERROR_RETURN
            (compute_en_ecmp_hash(unit, &hash_res, &hash_value));
        LOG_VERBOSE(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "\tECMP Hash value 0x%08x\n"),
                     hash_value));

        BCM_IF_ERROR_RETURN
            (get_en_hash_ecmp(unit,
                           pkt_info->mpintf - BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit),
                           hash_value, &nh_id));
        /* Convert to egress object format */
        *dst_intf = nh_id + BCM_XGS3_EGRESS_IDX_MIN(unit);
    } else
#endif /* INCLUDE_L3 */
    if (0 != (pkt_info->flags & BCM_SWITCH_PKT_INFO_HASH_TRUNK)) {
        bcm_trunk_t trunk = BCM_TRUNK_INVALID;
        int member_count;
        bcm_trunk_chip_info_t   ti;

        if (dst_gport == NULL) {
            return BCM_E_PARAM;
        }
        
        if (!BCM_GPORT_IS_TRUNK(pkt_info->trunk_gport)) {
            return BCM_E_PORT;
        }
        trunk = BCM_GPORT_TRUNK_GET(pkt_info->trunk_gport);
        BCM_IF_ERROR_RETURN
            (bcm_esw_trunk_get(unit, trunk, NULL, 0, NULL, &member_count));
        if (0 == member_count) {
            /* Return failure for no trunk members */
            return BCM_E_FAIL;
        }

        BCM_IF_ERROR_RETURN(bcm_esw_trunk_chip_info_get(unit, &ti));

        if (trunk >= ti.trunk_id_min && trunk <= ti.trunk_id_max) {
            BCM_IF_ERROR_RETURN
                (compute_en_rtag7_hash_trunk(unit, &hash_res, &hash_value));
            LOG_VERBOSE(BSL_LS_BCM_COMMON,
                        (BSL_META_U(unit,
                                    "\tTrunk Hash value 0x%08x\n"),
                         hash_value));
            if (hash_res.is_nonuc) {
                BCM_IF_ERROR_RETURN
                    (get_en_hash_trunk_nuc(unit, trunk, pkt_info->fwd_reason, 
                                hash_value, dst_gport));
            } else {
                BCM_IF_ERROR_RETURN
                    (get_en_hash_trunk(unit, trunk,
                                hash_value, dst_gport));
            }
        } else if (trunk >= ti.trunk_fabric_id_min && trunk <= ti.trunk_fabric_id_max) {

            BCM_IF_ERROR_RETURN
                (compute_en_rtag7_hash_hg_trunk(unit, &hash_res, &hash_value));
            LOG_VERBOSE(BSL_LS_BCM_COMMON,
                        (BSL_META_U(unit,
                                    "\tHG-Trunk Hash value 0x%08x\n"),
                         hash_value));

            BCM_IF_ERROR_RETURN
                (get_en_hash_hg_trunk(unit, trunk - ti.trunk_fabric_id_min,
                            hash_value, dst_gport));
        } else {
            return BCM_E_UNAVAIL;
        }
    } else if (pkt_info->flags & BCM_SWITCH_PKT_INFO_HASH_LBID) {
        /* LBID hashing resolution */
        if (dst_intf == NULL || hash_res.lbid_hash_valid == 0) {
            return BCM_E_FAIL;
        } else {
            *dst_intf = hash_res.rtag7_lbid_hash;
        }
    } else {
        return BCM_E_PARAM;
    }
	
    return BCM_E_NONE;
}

