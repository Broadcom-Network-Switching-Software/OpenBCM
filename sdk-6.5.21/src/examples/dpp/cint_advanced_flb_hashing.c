/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
 */

/* This file gives examples on how to configure pmf programs that provide flexible load balancing solutions to different cases.
 * These cases include:
 *      * PROG-MPLS-three-to-eight
 *      * PROG-L4oIPv4-At-H3
 *      * PROG-L4oIPv4-At-H4
 *      * PROG-IPv4-At-BOS
 *      * PROG-IPv6-At-BOS
 *      * PROG-ETH-At-BOS
 *
 * To configure the following programs correctly several steps must be taken beforehand:
 *
 * 1. SoC property parser_mode must be set to 1:
 *          init_soc_properties $unit "parser_mode=0"
 * 2. For all programs (but MPLS-Three-to-eight-labels) speculative parsing for the in-port must be turned on:
 *          rv = bcm_port_control_set( unit, in_port, bcmPortControlMplsSpeculativeParse, 1)
 * 3. For programs with packet types ETHoMPLSoETH speculation of ETH below MPLS for nibble value 0 must be configured:
 *          bcm_switch_control_key_t control_key;
 *          control_key.type = bcmSwitchMplsSpeculativeNibbleMap;
 *          control_key.index = 0;
 *          bcm_switch_control_info_t next_protocol_field;
 *          next_protocol_field.value = bcmSwitchMplsNextProtocolEthernet;
 *          rv = bcm_switch_control_indexed_set(unit, control_key, next_protocol_field);
 * 4. For *ETH2oMPLSoETH (second ETH header is double tagged), TPID value different than 0x8100, must be known:
 *          rv = bcm_port_tpid_add(unit, in_port, tpid_value, BCM_COLOR_PRIORITY);
 */

int presel_id = 0; /* ID from 0 to 11 for Advanced mode */

int program_flb_ecmp_ipv6_mpls_hashing(int unit)
{
    int rv = BCM_E_NONE;
    int group_id = 30;

    if (presel_id != 0)
    {
        presel_id = 0;
    }
    rv= program_mpls_three_to_eight_labels(unit, group_id, group_id+1);
    if(rv != BCM_E_NONE)
    {
        printf("program_mpls_three_to_eight_labels failed : %d\n",rv);
        return rv;
    }
    printf("program_mpls_three_to_eight_labels passed\n");
    group_id += 2;

    rv = program_ipv6_at_header_5(unit, group_id, group_id+1);
    if(rv != BCM_E_NONE)
    {
        printf("program_ipv6_at_header_5 failed : %d\n", rv);
        return rv;

    }
    printf("program_ipv6_at_header_5 passed\n");
    group_id += 2;

    rv = program_ipv6_at_h3_bos(unit, group_id, group_id+1);
    if(rv != BCM_E_NONE)
    {
        printf("program_ipv6_at_h3_bos failed : %d\n",rv);
        return rv;
    }
    printf("program_ipv6_at_h3_bos passed\n");
    group_id += 2;
    rv = program_ipv6_at_h4_bos(unit, group_id, group_id+1);
    if(rv != BCM_E_NONE)
    {
        printf("program_ipv6_at_h4_bos failed : %d\n",rv);
        return rv;
    }
    printf("program_ipv6_at_h4_bos passed\n");
    group_id += 2;

    return rv;
}

int program_flb_ecmp_ipv4_eth_hashing(int unit)
{
	int rv = BCM_E_NONE;
    int group_id = 15;
    if (presel_id != 0)
    {
        presel_id = 0;
    }
    rv = program_l4_ipv4_at_header_3(unit, group_id, group_id+1);
    if(rv != BCM_E_NONE)
    {
        printf("program_l4_ipv4_at_header_3 failed : %d\n",rv);
        return rv;
    }
    printf("program_l4_ipv4_at_header_3 passed\n");
    group_id += 2;

    rv = program_l4_ipv4_at_header_4(unit, group_id, group_id+1);
    if(rv != BCM_E_NONE)
    {
        printf("program_l4_ipv4_at_header_4 failed : %d\n",rv);
        return rv;
    }
    printf("program_l4_ipv4_at_header_4 passed\n");
    group_id += 2;

    rv = program_ipv4_at_h3_h4_bos(unit, group_id, group_id+1);
    if (rv != BCM_E_NONE)
    {
        printf("program_ipv4_at_h3_h4_bos failed : %d\n",rv);
        return rv;
    }
    group_id += 2;
    printf("program_ipv4_at_h3_h4_bos passed\n");

    rv = program_eth_mpls_eth_bos(unit, group_id, group_id+1);
    if(rv != BCM_E_NONE)
    {
        printf("program_eth_mpls_eth_bos failed : %d\n", rv);
        return rv;
    }
    printf("program_eth_mpls_eth_bos passed\n");
    group_id += 2;

    rv = program_ipv4_at_header_5(unit, group_id, group_id+1);
    if(rv != BCM_E_NONE)
    {
        printf("program_ipv4_at_header_5 failed : %d\n", rv);
        return rv;
    }
    printf("program_ipv4_at_header_5 passed\n");
    group_id += 2;

    return rv;
}

/*
 * The aim of this function is to create a program that would to match on packets of type MPLS[3..8]oETH.
 * It uses up to 8 different mpls labels to form the hashing.
 * In case of:
 *          * 3 MPLS headers - key will be comprised of MPLS3(neg)-MPLS2(neg)-MPLS3(neg)-MPLS2(neg)-MPLS1(neg)-MPLS3-MPLS2-MPLS1
 *          * 4 MPLS headers - key will be comprised of MPLS4(neg)-MPLS3(neg)-MPLS4(neg)-MPLS3(neg)-MPLS2(neg)-MPLS3-MPLS2-MPLS1
 *          * 5 MPLS headers - key will be comprised of MPLS5(neg)-MPLS4(neg)-MPLS5(neg)-MPLS4(neg)-MPLS3(neg)-MPLS3-MPLS2-MPLS1
 *          * 6 MPLS headers - key will be comprised of MPLS6(neg)-MPLS5(neg)-MPLS6(neg)-MPLS5(neg)-MPLS4(neg)-MPLS3-MPLS2-MPLS1
 *          * 7 MPLS headers - key will be comprised of MPLS7(neg)-MPLS6(neg)-MPLS6(neg)-MPLS5(neg)-MPLS4(neg)-MPLS3-MPLS2-MPLS1
 *          * 8 MPLS headers - key will be comprised of MPLS8(neg)-MPLS7(neg)-MPLS6(neg)-MPLS5(neg)-MPLS4(neg)-MPLS3-MPLS2-MPLS1
 * This program is designed for 8 MPLs headers and in case of packet with 9
 *          * 9 MPLS headers - key will be comprised of MPLS9(neg)-MPLS8(neg)-MPLS6(neg)-MPLS5(neg)-MPLS4(neg)-MPLS3-MPLS2-MPLS1
 * NOTE that packets with less than three MPLS headers will also fall into this program unless specified programs for
 * bcmFieldHeaderFormatMplsLabel1 and bcmFieldHeaderFormatMplsLabel2 packet header formats exist with a higher priority.
 */
int program_mpls_three_to_eight_labels(int unit, int grp_id_pre, int grp_id_post)
{
    int rv = BCM_E_NONE;
    /*NOTE: Qualifying on packets which have at least three MPLS labels (not the best header type qualifier)*/
    bcm_field_header_format_t preselector_header_format = bcmFieldHeaderFormatMplsAnyLabelAnyL2L3;

    /*Hash (pre hashing) data qualifiers*/
    uint8 hash_dq_nof = 8;
    bcm_field_qualify_t hash_dq_qualifier[8];
    uint32 hash_dq_length[8];
    uint16 hash_dq_offset[8];
    bcm_field_data_qualifier_t hash_dq_data_qual[8];
    int hash_offset_base[8];

    /*Key (post hashing) data qualifiers*/
    uint8 key_dq_nof = 4;
    bcm_field_qualify_t key_dq_qualifier[4];
    uint32 key_dq_length[4];
    uint16 key_dq_offset[4];
    int key_offset_base[4] = {0, 0, 0, 0};
    bcm_field_data_qualifier_t key_dq_data_qual[4];
    int qual_ids[8];

    /*NOTE: Keeping the pre-hashing and post-hashing vector the same while testing*/
    /*Build Hash vector*/
    hash_dq_qualifier[0] = bcmFieldQualifyMplsLabel1Id; /*MPLS Label 1*/
    hash_dq_length[0] = 20;
    hash_dq_offset[0] = 7;
    hash_dq_qualifier[1] = bcmFieldQualifyMplsLabel2Id; /*MPLS Label 2*/
    hash_dq_length[1] = 20;
    hash_dq_offset[1] = 39;
    hash_dq_qualifier[2] = bcmFieldQualifyMplsLabel3Id; /*MPLS Label 3*/
    hash_dq_length[2] = 20;
    hash_dq_offset[2] = 71;
    hash_dq_qualifier[3] = bcmFieldQualifyMplsLabel1Id; /*MPLS Label 4*/
    hash_dq_length[3] = 20;
    hash_dq_offset[3] = 39;
    hash_dq_qualifier[4] = bcmFieldQualifyMplsLabel2Id; /*MPLS Label 5*/
    hash_dq_length[4] = 20;
    hash_dq_offset[4] = 71;
    hash_dq_qualifier[5] = bcmFieldQualifyMplsLabel3Id; /*MPLS Label 6*/
    hash_dq_length[5] = 20;
    hash_dq_offset[5] = 103;
    hash_dq_qualifier[6] = bcmFieldQualifyMplsLabel1Id; /*MPLS Label 7*/
    hash_dq_length[6] = 20;
    hash_dq_offset[6] = 71;
    hash_dq_qualifier[7] = bcmFieldQualifyMplsLabel2Id; /*MPLS Label 8*/
    hash_dq_length[7] = 20;
    hash_dq_offset[7] = 39;

    hash_offset_base[0] = bcmFieldDataOffsetBaseFirstHeader;
    hash_offset_base[1] = bcmFieldDataOffsetBaseFirstHeader;
    hash_offset_base[2] = bcmFieldDataOffsetBaseFirstHeader;
    hash_offset_base[3] = bcmFieldDataOffsetBaseThirdHeader;
    hash_offset_base[4] = bcmFieldDataOffsetBaseThirdHeader;
    hash_offset_base[5] = bcmFieldDataOffsetBaseThirdHeader;
    hash_offset_base[6] = bcmFieldDataOffsetBaseFourthHeader;
    hash_offset_base[7] = bcmFieldDataOffsetBaseFourthHeader;

    /*Build Key, 74 bits*/
    key_dq_qualifier[0] = bcmFieldQualifyHashValue;
    key_dq_length[0] = 16;
    key_dq_offset[0] = 16; /*CRC-C*/
    key_dq_qualifier[1] = bcmFieldQualifyHashValue;
    key_dq_length[1] = 16;
    key_dq_offset[1] = 0; /*CRC-D*/
    key_dq_qualifier[2] = bcmFieldQualifyHashValue;
    key_dq_length[2] = 32;
    key_dq_offset[2] = 64; /*CRC-A+CRC-B*/
    key_dq_qualifier[3] = bcmFieldQualifyHashValue;
    key_dq_length[3] = 10;
    key_dq_offset[3] = 40; /*CRC-B*/

    uint32 dq_ndx;
    /*NOTE: IDs of hash and key field groups - one for hash, one for key*/
    bcm_field_group_t group_id[2];
    group_id[0] = grp_id_pre;
    group_id[1] = grp_id_post;

    /*Create Pre selector*/
    bcm_field_presel_set_t psset;
    rv = field_preselector_create(unit, &psset, preselector_header_format);
    if (rv != BCM_E_NONE)
    {
        printf("Error, field_preselector_create - (%d)\n", rv);
        return rv;
    }

    /*Create data qualifiers for the pre-hashing vector. Create a group for them*/
    for (dq_ndx = 0; dq_ndx < hash_dq_nof; dq_ndx++)
    {
        bcm_field_data_qualifier_t_init(&hash_dq_data_qual[dq_ndx]);
        if (dq_ndx < 3)
        {
            hash_dq_data_qual[dq_ndx].flags = BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
        }
        else
        {
            hash_dq_data_qual[dq_ndx].flags = BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_NEGATIVE;
        }

        hash_dq_data_qual[dq_ndx].offset_base = hash_offset_base[dq_ndx];
        hash_dq_data_qual[dq_ndx].offset = hash_dq_offset[dq_ndx];
        hash_dq_data_qual[dq_ndx].stage = bcmFieldStageHash;
        hash_dq_data_qual[dq_ndx].qualifier = hash_dq_qualifier[dq_ndx];
        hash_dq_data_qual[dq_ndx].length = hash_dq_length[dq_ndx];
        rv = bcm_field_data_qualifier_create(unit, &hash_dq_data_qual[dq_ndx]);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_field_data_qualifier_create %d \n", dq_ndx);
            return rv;
        }
        printf("qualifier ID : %d \n", hash_dq_data_qual[dq_ndx].qual_id);
        qual_ids[dq_ndx] = hash_dq_data_qual[dq_ndx].qual_id;
    }
    rv = field_group_create(unit, group_id[0], hash_dq_nof, qual_ids, 1, psset);
    if (rv != BCM_E_NONE)
    {
        printf("Error field_group_create - (%d)\n", rv);
        return rv;
    }

    for (dq_ndx = 0; dq_ndx < key_dq_nof; dq_ndx++)
    {
        bcm_field_data_qualifier_t_init(&key_dq_data_qual[dq_ndx]);
        key_dq_data_qual[dq_ndx].flags = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
        key_dq_data_qual[dq_ndx].offset_base = key_offset_base[dq_ndx];
        key_dq_data_qual[dq_ndx].stage = bcmFieldStageHash;
        key_dq_data_qual[dq_ndx].qualifier = key_dq_qualifier[dq_ndx];
        key_dq_data_qual[dq_ndx].length = key_dq_length[dq_ndx];
        rv = bcm_field_data_qualifier_create(unit, &key_dq_data_qual[dq_ndx]);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_field_data_qualifier_create %d \n", dq_ndx);
            return rv;
        }
        printf("qualifier ID : %d \n", key_dq_data_qual[dq_ndx].qual_id);
        qual_ids[dq_ndx] = key_dq_data_qual[dq_ndx].qual_id;
    }
    rv = field_group_create(unit, group_id[1], key_dq_nof, qual_ids, 0, psset);
    if (rv != BCM_E_NONE)
    {
        printf("Error field_group_create - (%d)\n", rv);
        return rv;
    }

    return rv;
}

/*
 * To match on packets of type:
 *      * L4oIPv4oMPLS[1..3]oETH
 * It calls program_l4_ipv4_at_header_3_header_4(unit, 1) to create the configuration for IPv4 header at H4.
 */
int program_l4_ipv4_at_header_3(int unit, int grp_id_pre, int grp_id_post)
{
    int rv = BCM_E_NONE;
    rv = program_l4_ipv4_at_header_3_header_4(unit, 1, grp_id_pre, grp_id_post);
    if (rv != BCM_E_NONE)
    {
        printf("Error, program_l4_ipv4_at_header_3_header_4 - (%d)\n", rv);
    }
    return rv;
}

/*
 * To create a program to match on packets of type:
 *      * L4oIPv4oETHoMPLS[1..3]oETH
 * It calls program_l4_ipv4_at_header_3_header_4(unit, 0) to create the configuration for IPv4 header at H4.
 */
int program_l4_ipv4_at_header_4(int unit, int grp_id_pre, int grp_id_post)
{
    int rv = BCM_E_NONE;
    rv = program_l4_ipv4_at_header_3_header_4(unit, 0, grp_id_pre, grp_id_post);
    if (rv != BCM_E_NONE)
    {
        printf("Error, program_l4_ipv4_at_header_3_header_4 - (%d)\n", rv);
    }
    return rv;
}


/* This function creates a program that matches on packets of types:
 *          * L4oIPv4oMPLSoETH - when ipv4_at_l3 is set to 1
 *          * L4oIPv4oETHoMPLSoETH - when ipv4_at_l3 is set to 0.
 * It uses the L4 and IPv4 headers as qualifying headers and uses the fields from them for hashing.
 * The created program extracts L4-src-port, L4-dst-port and IPv4 DIP and uses them to create the key.
 */
int program_l4_ipv4_at_header_3_header_4(int unit, int ipv4_at_l3, int grp_id_pre, int grp_id_post)
{
    int rv = BCM_E_NONE;
    bcm_field_qset_t gr_qset;
    uint32 flags;
    int qual_ids[5];

    /*Hash and key field groups*/
    bcm_field_group_t group_id[2];
    group_id[0] = grp_id_pre;
    group_id[1] = grp_id_post;
    bcm_field_group_config_t group_config[2];

    /*Pre selector*/
    int presel_flags = 0;
    bcm_field_presel_set_t psset;
    bcm_field_header_format_extension_t preselector_header_format;

    /*Hash (pre hashing) data qualifiers*/
    uint8 hash_dq_nof = 4;
    bcm_field_qualify_t hash_dq_qualifier[4];
    uint32 hash_dq_length[4];
    uint16 hash_dq_offset[4];
    int hash_offset_base[4];
    bcm_field_data_qualifier_t hash_dq_data_qual[4];

    /*Key (post hashing) data qualifiers*/
    uint8 key_dq_nof = 5;
    bcm_field_qualify_t key_dq_qualifier[5];
    uint32 key_dq_length[5];
    uint16 key_dq_offset[5];
    int key_offset_base[5] = {0, 0, 0, 0, 0};
    bcm_field_data_qualifier_t key_dq_data_qual[5];

    /*Build Hash vector*/
    hash_dq_qualifier[0] = bcmFieldQualifySrcIp;
    hash_dq_length[0] = 32;
    hash_dq_offset[0] = 71;
    hash_dq_qualifier[1] = bcmFieldQualifyDstIp;
    hash_dq_length[1] = 32;
    hash_dq_offset[1] = 39;
    hash_dq_qualifier[2] = bcmFieldQualifyL4SrcPort;
    hash_dq_length[2] = 16;
    hash_dq_offset[2] = 7;
    hash_dq_qualifier[3] = bcmFieldQualifyL4DstPort;
    hash_dq_length[3] = 16;
    hash_dq_offset[3] = 23;
    if (ipv4_at_l3 == 1)
    {
        hash_offset_base[0] = bcmFieldDataOffsetBaseThirdHeader;
        hash_offset_base[1] = bcmFieldDataOffsetBaseThirdHeader;
        hash_offset_base[2] = bcmFieldDataOffsetBaseThirdHeader;
        hash_offset_base[3] = bcmFieldDataOffsetBaseThirdHeader;
        preselector_header_format = bcmFieldHeaderFormatExtensionMplsL4IPV4isH3;
    }
    else
    {
        hash_offset_base[0] = bcmFieldDataOffsetBaseFourthHeader;
        hash_offset_base[1] = bcmFieldDataOffsetBaseFourthHeader;
        hash_offset_base[2] = bcmFieldDataOffsetBaseFourthHeader;
        hash_offset_base[3] = bcmFieldDataOffsetBaseFourthHeader;
        preselector_header_format = bcmFieldHeaderFormatExtensionMplsL4IPV4isH4;
    }

    key_dq_qualifier[0] = bcmFieldQualifyHashValue;
    key_dq_length[0] = 16;
    key_dq_offset[0] = 0; /*CRC-D*/
    key_dq_qualifier[1] = bcmFieldQualifyHashValue;
    key_dq_length[1] = 16;
    key_dq_offset[1] = 16; /*CRC-C*/
    key_dq_qualifier[2] = bcmFieldQualifyHashValue;
    key_dq_length[2] = 16;
    key_dq_offset[2] = 32; /*CRC-B*/
    key_dq_qualifier[3] = bcmFieldQualifyHashValue;
    key_dq_length[3] = 16;
    key_dq_offset[3] = 48; /*CRC-A*/
    key_dq_qualifier[4] = bcmFieldQualifyHashValue;
    key_dq_length[4] = 10;
    key_dq_offset[4] = 0; /*CRC-D*/

    rv = field_preselector_extension_create(unit, &psset, preselector_header_format);
    if (rv != BCM_E_NONE)
    {
        printf("Error, field_preselector_extension_create - (%d)\n", rv);
        return rv;
    }
    int dq_ndx;
    for (dq_ndx = 0; dq_ndx < hash_dq_nof; dq_ndx++)
    {
        bcm_field_data_qualifier_t_init(&hash_dq_data_qual[dq_ndx]);
        if (dq_ndx > 1)
        {
            hash_dq_data_qual[dq_ndx].flags = BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
        }
        else
        {
            hash_dq_data_qual[dq_ndx].flags = BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_NEGATIVE;
        }
        hash_dq_data_qual[dq_ndx].offset_base = hash_offset_base[dq_ndx];
        hash_dq_data_qual[dq_ndx].offset = hash_dq_offset[dq_ndx];
        hash_dq_data_qual[dq_ndx].stage = bcmFieldStageHash;
        hash_dq_data_qual[dq_ndx].qualifier = hash_dq_qualifier[dq_ndx];
        hash_dq_data_qual[dq_ndx].length = hash_dq_length[dq_ndx];
        rv = bcm_field_data_qualifier_create(unit, &hash_dq_data_qual[dq_ndx]);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_field_data_qualifier_create on ID %d \n", dq_ndx);
            return rv;
        }
        printf("qualifier ID : %d \n", hash_dq_data_qual[dq_ndx].qual_id);
        qual_ids[dq_ndx] = hash_dq_data_qual[dq_ndx].qual_id;
    }

    rv = field_group_create(unit, group_id[0], hash_dq_nof, qual_ids, 1, psset);
    if (rv != BCM_E_NONE)
    {
        printf("Error field_group_create - (%d)\n", rv);
        return rv;
    }

    rv = field_data_qualifier_array_create(unit, key_dq_nof, key_dq_qualifier, key_dq_offset, key_offset_base, key_dq_length,
                 BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES, qual_ids);
    if (rv != BCM_E_NONE)
    {
        printf("Error, field_data_qualifier_array_create - (%d) \n", rv);
        return rv;
    }
    rv = field_group_create(unit, group_id[1], key_dq_nof, qual_ids, 0, psset);
    if (rv != BCM_E_NONE)
    {
        printf("Error field_group_create - (%d)\n", rv);
        return rv;
    }
    return rv;
}

/* The purpose of this function is to create a program that would match on packets of type:
 *          * IPv4oMPLS[1..3]oETH
 *          * IPv4oETHoMPLS[1..3]oETH
 * and extract fields from the IPv4 header which is the qualifying header.
 * It extracts 32 bits of the DIP and 32 bits of the SIP and uses them for hashing.
 */
int program_ipv4_at_h3_h4_bos(int unit, int grp_id_pre, int grp_id_post)
{
    int rv = BCM_E_NONE;
    bcm_field_header_format_extension_t preselector_header_format = bcmFieldHeaderFormatExtensionMplsNonL4IPV4IsAny;
    int dq_ndx;
    uint32 flags;
    int qual_ids[2];

    /*Hash and key field groups*/
    bcm_field_group_t group_id[2];
    group_id[0] = grp_id_pre;
    group_id[1] = grp_id_post;
    bcm_field_group_config_t group_config;
    bcm_field_presel_set_t psset;

    /*Hash (pre hashing) data qualifiers*/
    uint8 hash_dq_nof = 2;
    bcm_field_qualify_t hash_dq_qualifier[2];
    uint32 hash_dq_length[2];
    uint16 hash_dq_offset[2];
    int hash_offset_base[2];
    bcm_field_data_qualifier_t hash_dq_data_qual[2];

    /*Key (post hashing) data qualifiers*/
    uint8 key_dq_nof = 2;
    bcm_field_qualify_t key_dq_qualifier[2];
    uint32 key_dq_length[2];
    uint16 key_dq_offset[2];
    int key_offset_base[2] = {0, 0};
    bcm_field_data_qualifier_t key_dq_data_qual[2];

    /*Build Hash vector*/
    hash_dq_qualifier[0] = bcmFieldQualifySrcIp;
    hash_dq_length[0] = 32;
    hash_dq_offset[0] = 71;
    hash_dq_qualifier[1] = bcmFieldQualifyDstIp;
    hash_dq_length[1] = 32;
    hash_dq_offset[1] = 39;
    hash_offset_base[0] = bcmFieldDataOffsetBaseFourthHeader;
    hash_offset_base[1] = bcmFieldDataOffsetBaseFourthHeader;

    key_dq_qualifier[0] = bcmFieldQualifyHashValue;
    key_dq_length[0] = 32;
    key_dq_offset[0] = 32; /*CRC-B + CRC-A*/
    key_dq_qualifier[1] = bcmFieldQualifyHashValue;
    key_dq_length[1] = 32;
    key_dq_offset[1] = 0; /*CRC-D + CRC-C*/

    rv = field_preselector_extension_create(unit, &psset, preselector_header_format);
    if (rv != BCM_E_NONE)
    {
        printf("Error, field_preselector_extension_create - (%d)\n", rv);
        return rv;
    }
    /*
     * 2. Define all the data qualifiers
     */
    rv = field_data_qualifier_array_create(unit, hash_dq_nof, hash_dq_qualifier, hash_dq_offset, hash_offset_base, hash_dq_length,
                BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_NEGATIVE, qual_ids);
    if (rv != BCM_E_NONE)
    {
        printf("Error, field_data_qualifier_array_create - (%d) \n", rv);
        return rv;
    }
    rv = field_group_create(unit, group_id[0], hash_dq_nof, qual_ids, 1, psset);
    if (rv != BCM_E_NONE)
    {
        printf("Error field_group_create - (%d)\n", rv);
        return rv;
    }

    rv = field_data_qualifier_array_create(unit, key_dq_nof, key_dq_qualifier, key_dq_offset, key_offset_base, key_dq_length,
                BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES , qual_ids);
    if (rv != BCM_E_NONE)
    {
        printf("Error, field_data_qualifier_array_create - (%d) \n", rv);
        return rv;
    }
    rv = field_group_create(unit, group_id[1], key_dq_nof, qual_ids, 0, psset);
    if (rv != BCM_E_NONE)
    {
        printf("Error field_group_create - (%d)\n", rv);
        return rv;
    }
    rv = bcm_switch_control_set(unit, bcmSwitchECMPHashConfig, BCM_HASH_CONFIG_CRC16_0x84a1);
    if (rv != BCM_E_NONE) {
        printf ("bcm_petra_switch_control_set with type bcmSwitchECMPHashConfig failed: %d \n", rv);
    }
    rv = bcm_switch_control_set(unit, bcmSwitchECMPHashOffset, 14);
    if (rv != BCM_E_NONE) {
        printf ("bcm_petra_switch_control_set with type bcmSwitchECMPHashOffset failed: %d \n", rv);
    }

    return rv;
}


/* The aim of this program is to match on packets of type:
 *          * L4oIPv6oMPLS[1..3]oETH,
 *          * IPv6oMPLS[1..3]oETH.
 * To do that, this function calls program_ipv6_at_h3_h4_bos(unit, 1)
 */
int program_ipv6_at_h3_bos(int unit, int grp_id_pre, int grp_id_post)
{
    int rv = BCM_E_NONE;
    int is_h3 = 1;
    rv = program_ipv6_at_h3_h4_bos(unit, is_h3, grp_id_pre, grp_id_post);
    if (rv != BCM_E_NONE)
    {
        printf("Error, program_ipv6_at_h3_h4_bos - (%d)\n", rv);
    }
    return rv;
}

/* The aim of this program is to match on packets of type:
 *          * L4oIPv6oETHoMPLS[1..3]oETH,
 *          * IPv6oETHoMPLS[1..3]oETH,
 * To do that, this function calls program_ipv6_at_h3_h4_bos(unit, 0)
 */
int program_ipv6_at_h4_bos(int unit, int grp_id_pre, int grp_id_post)
{
    int rv = BCM_E_NONE;
    int not_h3 = 0;
    rv = program_ipv6_at_h3_h4_bos(unit, not_h3, grp_id_pre, grp_id_post);
    if (rv != BCM_E_NONE)
    {
        printf("Error, program_ipv6_at_h3_h4_bos - (%d)\n", rv);
    }
    return rv;
}

/* The aim of this function is create a program that would match on packets of type:
 *          * L4oIPv6oETHoMPLS[1..3]oETH,
 *          * IPv6oETHoMPLS[1..3]oETH,
 * if ipv6_at_h3 is set to 0.
 * Or the program can match on:
 *          * L4oIPv6oMPLS[1..3]oETH,
 *          * IPv6oMPLS[1..3]oETH
 * if ipv6_at_h3 is set to 1.
 * If there's a need to combine the two cases, parsing of the L4 header must be turned off using the soc property
 * "custom_feature_udp_parse_disable" set to 1 and the header format should be combining the two cases - bcmFieldHeaderFormatExtensionMplsNonL4IPV6IsAny.
 * For hashing purposes the IPv6 header is used and 128 bits of the SIP and 128 bits of the DIP will be extracted.
 */
int program_ipv6_at_h3_h4_bos(int unit, int ipv6_at_h3, int grp_id_pre, int grp_id_post)
{
    int rv = BCM_E_NONE;
    bcm_field_header_format_extension_t preselector_header_format;
    int dq_ndx;
    int qual_ids[8];

    /*Hash and key field groups*/
    bcm_field_group_t group_id[2];
    group_id[0] = grp_id_pre;
    group_id[1] = grp_id_post;
    bcm_field_group_config_t group_config;
    bcm_field_presel_set_t psset;

    /*Hash (pre hashing) data qualifiers*/
    uint8 hash_dq_nof = 8;
    bcm_field_qualify_t hash_dq_qualifier[8];
    uint32 hash_dq_length[8];
    uint16 hash_dq_offset[8];
    int hash_offset_base[8];
    bcm_field_data_qualifier_t hash_dq_data_qual[8];

    /*Key (post hashing) data qualifiers*/
    uint8 key_dq_nof = 4;
    bcm_field_qualify_t key_dq_qualifier[4];
    uint32 key_dq_length[4];
    uint16 key_dq_offset[4];
    int key_offset_base[4] = {0, 0, 0, 0};
    bcm_field_data_qualifier_t key_dq_data_qual[4];

    /*Build Hash vector*/
    hash_dq_qualifier[0] = bcmFieldQualifyDstIp6Low;
    hash_dq_length[0] = 32;
    hash_dq_offset[0] = 135;
    hash_dq_qualifier[1] = bcmFieldQualifyDstIp6Low;
    hash_dq_length[1] = 32;
    hash_dq_offset[1] = 103;
    hash_dq_qualifier[2] = bcmFieldQualifyDstIp6High;
    hash_dq_length[2] = 32;
    hash_dq_offset[2] = 71;
    hash_dq_qualifier[3] = bcmFieldQualifyDstIp6High;
    hash_dq_length[3] = 32;
    hash_dq_offset[3] = 39;
    hash_dq_qualifier[4] = bcmFieldQualifySrcIp6Low;
    hash_dq_length[4] = 32;
    hash_dq_offset[4] = 263;
    hash_dq_qualifier[5] = bcmFieldQualifySrcIp6Low;
    hash_dq_length[5] = 32;
    hash_dq_offset[5] = 231;
    hash_dq_qualifier[6] = bcmFieldQualifySrcIp6High;
    hash_dq_length[6] = 32;
    hash_dq_offset[6] = 199;
    hash_dq_qualifier[7] = bcmFieldQualifySrcIp6High;
    hash_dq_length[7] = 32;
    hash_dq_offset[7] = 167;
    if (ipv6_at_h3 == 1)
    {
        preselector_header_format = bcmFieldHeaderFormatExtensionMplsAnyL4IPV6IsH3;
        hash_offset_base[0] = bcmFieldDataOffsetBaseThirdHeader;
        hash_offset_base[1] = bcmFieldDataOffsetBaseThirdHeader;
        hash_offset_base[2] = bcmFieldDataOffsetBaseThirdHeader;
        hash_offset_base[3] = bcmFieldDataOffsetBaseThirdHeader;
        hash_offset_base[4] = bcmFieldDataOffsetBaseThirdHeader;
        hash_offset_base[5] = bcmFieldDataOffsetBaseThirdHeader;
        hash_offset_base[6] = bcmFieldDataOffsetBaseThirdHeader;
        hash_offset_base[7] = bcmFieldDataOffsetBaseThirdHeader;
    }
    else
    {
        preselector_header_format = bcmFieldHeaderFormatExtensionMplsAnyL4IPV6IsH4;
        hash_offset_base[0] = bcmFieldDataOffsetBaseFourthHeader;
        hash_offset_base[1] = bcmFieldDataOffsetBaseFourthHeader;
        hash_offset_base[2] = bcmFieldDataOffsetBaseFourthHeader;
        hash_offset_base[3] = bcmFieldDataOffsetBaseFourthHeader;
        hash_offset_base[4] = bcmFieldDataOffsetBaseFourthHeader;
        hash_offset_base[5] = bcmFieldDataOffsetBaseFourthHeader;
        hash_offset_base[6] = bcmFieldDataOffsetBaseFourthHeader;
        hash_offset_base[7] = bcmFieldDataOffsetBaseFourthHeader;
    }

    key_dq_qualifier[0] = bcmFieldQualifyHashValue;
    key_dq_length[0] = 16; /*CRC-D*/
    key_dq_offset[0] = 0;
    key_dq_qualifier[1] = bcmFieldQualifyHashValue;
    key_dq_length[1] = 16;
    key_dq_offset[1] = 16; /*CRC-C*/
    key_dq_qualifier[2] = bcmFieldQualifyHashValue;
    key_dq_length[2] = 32;
    key_dq_offset[2] = 32; /*CRC-A+CRC-B*/
    key_dq_qualifier[3] = bcmFieldQualifyHashValue;
    key_dq_length[3] = 10;
    key_dq_offset[3] = 8; /*CRC-D*/

    rv = field_preselector_extension_create(unit, &psset, preselector_header_format);
    if (rv != BCM_E_NONE)
    {
        printf("Error, field_preselector_extension_create - (%d)\n", rv);
        return rv;
    }
    /*
     * 2. Define all the data qualifiers
     */
    rv = field_data_qualifier_array_create(unit, hash_dq_nof, hash_dq_qualifier, hash_dq_offset, hash_offset_base, hash_dq_length,
                BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_NEGATIVE, qual_ids);
    if (rv != BCM_E_NONE)
    {
        printf("Error, field_data_qualifier_array_create - (%d) \n", rv);
        return rv;
    }
    rv = field_group_create(unit, group_id[0], hash_dq_nof, qual_ids, 1, psset);
    if (rv != BCM_E_NONE)
    {
        printf("Error field_group_create - (%d)\n", rv);
        return rv;
    }

    rv = field_data_qualifier_array_create(unit, key_dq_nof, key_dq_qualifier, key_dq_offset, key_offset_base, key_dq_length,
                BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES, qual_ids);
    if (rv != BCM_E_NONE)
    {
        printf("Error, field_data_qualifier_array_create - (%d) \n", rv);
        return rv;
    }
    rv = field_group_create(unit, group_id[1], key_dq_nof, qual_ids, 0, psset);
    if (rv != BCM_E_NONE)
    {
        printf("Error field_group_create - (%d)\n", rv);
        return rv;
    }
    rv = bcm_switch_control_set(unit, bcmSwitchECMPHashConfig, BCM_HASH_CONFIG_CRC16_0x84a1);
    if (rv != BCM_E_NONE) {
        printf ("bcm_petra_switch_control_set with type bcmSwitchECMPHashConfig failed: %d \n", rv);
    }
    rv = bcm_switch_control_set(unit, bcmSwitchECMPHashOffset, 14);
    if (rv != BCM_E_NONE) {
        printf ("bcm_petra_switch_control_set with type bcmSwitchECMPHashOffset failed: %d \n", rv);
    }
    return rv;
}

/* The aim of this function is to create a program that would match on packets of type ETHAnyoMPLS[1..6]oETH.
 * It would extract different parts of the second ETH header based on the number of VLAN tags as described below:
 *          * ETH0 -                         (DA-DA-DA-DA-DA-   DA-   SA-   SA-  )SA-   SA-   SA-   SA-   Type-Type
 *          * ETH1 -             DA-DA-DA-DA-(DA-DA-SA-SA-SA-   SA-   SA-   SA-  )CVLAN-CVLAN-CVLAN-CVLAN-Type-Type
 *          * ETH2 - DA-DA-DA-DA-DA-DA-SA-SA-(SA-SA-SA-SA-SVLAN-SVLAN-SVLAN-SVLAN)-CVLAN-CVLAN-CVLAN-CVLAN-Type-Type
 *                                           '-----------------v-----------------'
 * The marked parts will be used for hashing purposes. The second ETH header is used as qualifying header.
 */
int program_eth_mpls_eth_bos(int unit, int grp_id_pre, int grp_id_post)
{
    int rv = BCM_E_NONE;
    bcm_field_header_format_extension_t preselector_header_format = bcmFieldHeaderFormatExtensionMplsAnyEthIsH3H4; /*bcmFieldHeaderFormatExtensionMplsETHisH3*/
    int dq_ndx;
    int qual_ids[3];

    /*Hash and key field groups*/
    bcm_field_group_t group_id[2];
    group_id[0] = grp_id_pre;
    group_id[1] = grp_id_post;
    bcm_field_group_config_t group_config;
    bcm_field_presel_set_t psset;

    /*Hash (pre hashing) data qualifiers*/
    uint8 hash_dq_nof = 2;
    bcm_field_qualify_t hash_dq_qualifier[2];
    uint32 hash_dq_length[2];
    uint16 hash_dq_offset[2];
    int hash_offset_base[2];
    bcm_field_data_qualifier_t hash_dq_data_qual[2];

    /*Key (post hashing) data qualifiers*/
    uint8 key_dq_nof = 3;
    bcm_field_qualify_t key_dq_qualifier[3];
    uint32 key_dq_length[3];
    uint16 key_dq_offset[3];
    int key_offset_base[3] = {0, 0, 0};
    bcm_field_data_qualifier_t key_dq_data_qual[3];

    /*Build Hash vector*/
    hash_dq_qualifier[0] = bcmFieldQualifyDstMac;
    hash_dq_length[0] = 32;
    hash_dq_offset[0] = 119;
    hash_dq_qualifier[1] = bcmFieldQualifySrcMac;
    hash_dq_length[1] = 32;
    hash_dq_offset[1] = 87;

    hash_offset_base[0] = bcmFieldDataOffsetBaseFourthHeader;
    hash_offset_base[1] = bcmFieldDataOffsetBaseFourthHeader;

    key_dq_qualifier[0] = bcmFieldQualifyHashValue;
    key_dq_length[0] = 32; /*CRC-D and CRC-C*/
    key_dq_offset[0] = 0;
    key_dq_qualifier[1] = bcmFieldQualifyHashValue;
    key_dq_length[1] = 32;
    key_dq_offset[1] = 32; /*CRC-B and CRC-A*/
    key_dq_qualifier[2] = bcmFieldQualifyHashValue;
    key_dq_length[2] = 10;
    key_dq_offset[2] = 16; /*CRC-C*/

    rv = field_preselector_extension_create(unit, &psset, preselector_header_format);
    if (rv != BCM_E_NONE)
    {
        printf("Error, field_preselector_extension_create - (%d)\n", rv);
        return rv;
    }
    /*
     * 2. Define all the data qualifiers
     */
    rv = field_data_qualifier_array_create(unit, hash_dq_nof, hash_dq_qualifier, hash_dq_offset, hash_offset_base, hash_dq_length,
                BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_NEGATIVE, qual_ids);
    if (rv != BCM_E_NONE)
    {
        printf("Error, field_data_qualifier_array_create - (%d) \n", rv);
        return rv;
    }
    rv = field_group_create(unit, group_id[0], hash_dq_nof, qual_ids, 1, psset);
    if (rv != BCM_E_NONE)
    {
        printf("Error field_group_create - (%d)\n", rv);
        return rv;
    }

    rv = field_data_qualifier_array_create(unit, key_dq_nof, key_dq_qualifier, key_dq_offset, key_offset_base, key_dq_length,
                BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES , qual_ids);
    if (rv != BCM_E_NONE)
    {
        printf("Error, field_data_qualifier_array_create - (%d) \n", rv);
        return rv;
    }
    rv = field_group_create(unit, group_id[1], key_dq_nof, qual_ids, 0, psset);
    if (rv != BCM_E_NONE)
    {
        printf("Error field_group_create - (%d)\n", rv);
        return rv;
    }
    return rv;
}


/* The aim of this function is create a program that would match on packets of type:
 *          * IPv4oETHoMPLS[4..6]oETH,
 *          * IPv4oMPLS[7..9]oETH,
 * It uses the IPv4 fields - SIP and DIP as qualifiers - a total of 64 bits.
 * They are being taken with positive offset from the last header offset base possible.
 */
int program_ipv4_at_header_5(int unit, int grp_id_pre, int grp_id_post)
{
    int rv = BCM_E_NONE;
    bcm_field_header_format_extension_t preselector_header_format = bcmFieldHeaderFormatExtensionMplsAnyL4IPV4IsH5;
    int dq_ndx;
    int qual_ids[3];

    /*Hash and key field groups*/
    bcm_field_group_t group_id[2];
    group_id[0] = grp_id_pre;
    group_id[1] = grp_id_post;
    bcm_field_group_config_t group_config;
    bcm_field_presel_set_t psset;

    /*Hash (pre hashing) data qualifiers*/
    uint8 hash_dq_nof = 2;
    bcm_field_qualify_t hash_dq_qualifier[2];
    uint32 hash_dq_length[2];
    uint16 hash_dq_offset[2];
    int hash_offset_base[2];

    /*Key (post hashing) data qualifiers*/
    uint8 key_dq_nof = 3;
    bcm_field_qualify_t key_dq_qualifier[3];
    uint32 key_dq_length[3];
    uint16 key_dq_offset[3];
    int key_offset_base[3] = {0, 0, 0};

    /*Build Hash vector*/
    hash_dq_qualifier[0] = bcmFieldQualifySrcIp;
    hash_dq_length[0] = 32;
    hash_dq_offset[0] = 103;
    hash_dq_qualifier[1] = bcmFieldQualifyDstIp;
    hash_dq_length[1] = 32;
    hash_dq_offset[1] = 135;
    hash_offset_base[0] = bcmFieldDataOffsetBaseFourthHeader;
    hash_offset_base[1] = bcmFieldDataOffsetBaseFourthHeader;
    
    key_dq_qualifier[0] = bcmFieldQualifyHashValue;;
    key_dq_length[0] = 32; /*CRC-D + CRC-C*/
    key_dq_offset[0] = 0;
    key_dq_qualifier[1] = bcmFieldQualifyHashValue;
    key_dq_length[1] = 32;
    key_dq_offset[1] = 32; /*CRC-A+CRC-B*/
    key_dq_qualifier[2] = bcmFieldQualifyHashValue;
    key_dq_length[2] = 10;
    key_dq_offset[2] = 16; /*CRC-C*/

    rv = field_preselector_extension_create(unit, &psset, preselector_header_format);
    if (rv != BCM_E_NONE)
    {
        printf("Error, field_preselector_extension_create - (%d)\n", rv);
        return rv;
    }
    /*
     * 2. Define all the data qualifiers
     */
    rv = field_data_qualifier_array_create(unit, hash_dq_nof, hash_dq_qualifier, hash_dq_offset, hash_offset_base, hash_dq_length,
                BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES, qual_ids);
    if (rv != BCM_E_NONE)
    {
        printf("Error, field_data_qualifier_array_create - (%d) \n", rv);
        return rv;
    }
    rv = field_group_create(unit, group_id[0], hash_dq_nof, qual_ids, 1, psset);
    if (rv != BCM_E_NONE)
    {
        printf("Error field_group_create - (%d)\n", rv);
        return rv;
    }

    rv = field_data_qualifier_array_create(unit, key_dq_nof, key_dq_qualifier, key_dq_offset, key_offset_base, key_dq_length,
                BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES, qual_ids);
    if (rv != BCM_E_NONE)
    {
        printf("Error, field_data_qualifier_array_create - (%d) \n", rv);
        return rv;
    }
    rv = field_group_create(unit, group_id[1], key_dq_nof, qual_ids, 0, psset);
    if (rv != BCM_E_NONE)
    {
        printf("Error field_group_create - (%d)\n", rv);
        return rv;
    }
    rv = bcm_switch_control_set(unit, bcmSwitchECMPHashConfig, BCM_HASH_CONFIG_CRC16_0x84a1);
    if (rv != BCM_E_NONE) {
        printf ("bcm_petra_switch_control_set with type bcmSwitchECMPHashConfig failed: %d \n", rv);
    }
    rv = bcm_switch_control_set(unit, bcmSwitchECMPHashOffset, 14);
    if (rv != BCM_E_NONE) {
        printf ("bcm_petra_switch_control_set with type bcmSwitchECMPHashOffset failed: %d \n", rv);
    }
    return rv;
}


/* The aim of this function is create a program that would match on packets of type:
 *          * IPv6oETHoMPLS[4..6]oETH,
 *          * IPv6oMPLS[7..9]oETH,
 * It uses the IPv6 fields - SIP and DIP as qualifiers - a total of 256 bits.
 * They are being taken with positive offset from the last header offset base possible.
 */
int program_ipv6_at_header_5(int unit, int grp_id_pre, int grp_id_post)
{
    int rv = BCM_E_NONE;
    bcm_field_header_format_extension_t preselector_header_format = bcmFieldHeaderFormatExtensionMplsAnyL4IPV6IsH5;
    int dq_ndx;
    int qual_ids[8];

    /*Hash and key field groups*/
    bcm_field_group_t group_id[2];
    group_id[0] = grp_id_pre;
    group_id[1] = grp_id_post;
    bcm_field_group_config_t group_config;
    bcm_field_presel_set_t psset;
    /*Hash (pre hashing) data qualifiers*/
    uint8 hash_dq_nof = 8;
    bcm_field_qualify_t hash_dq_qualifier[8];
    uint32 hash_dq_length[8];
    uint16 hash_dq_offset[8];
    int hash_offset_base[8];
    
    /*Key (post hashing) data qualifiers*/
    uint8 key_dq_nof = 3;
    bcm_field_qualify_t key_dq_qualifier[3];
    uint32 key_dq_length[3];
    uint16 key_dq_offset[3];
    int key_offset_base[3] = {0, 0, 0};

    hash_dq_qualifier[0] = bcmFieldQualifyDstIp6Low;
    hash_dq_length[0] = 32;
    hash_dq_offset[0] = 71;
    hash_dq_qualifier[1] = bcmFieldQualifyDstIp6Low;
    hash_dq_length[1] = 32;
    hash_dq_offset[1] = 103;
    hash_dq_qualifier[2] = bcmFieldQualifyDstIp6High;
    hash_dq_length[2] = 32;
    hash_dq_offset[2] = 135;
    hash_dq_qualifier[3] = bcmFieldQualifyDstIp6High;
    hash_dq_length[3] = 32;
    hash_dq_offset[3] = 167;
    hash_dq_qualifier[4] = bcmFieldQualifySrcIp6Low;
    hash_dq_length[4] = 32;
    hash_dq_offset[4] = 199;
    hash_dq_qualifier[5] = bcmFieldQualifySrcIp6Low;
    hash_dq_length[5] = 32;
    hash_dq_offset[5] = 231;
    hash_dq_qualifier[6] = bcmFieldQualifySrcIp6High;
    hash_dq_length[6] = 32;
    hash_dq_offset[6] = 263;
    hash_dq_qualifier[7] = bcmFieldQualifySrcIp6High;
    hash_dq_length[7] = 32;
    hash_dq_offset[7] = 295;
    hash_offset_base[0] = bcmFieldDataOffsetBaseFourthHeader;
    hash_offset_base[1] = bcmFieldDataOffsetBaseFourthHeader;
    hash_offset_base[2] = bcmFieldDataOffsetBaseFourthHeader;
    hash_offset_base[3] = bcmFieldDataOffsetBaseFourthHeader;
    hash_offset_base[4] = bcmFieldDataOffsetBaseFourthHeader;
    hash_offset_base[5] = bcmFieldDataOffsetBaseFourthHeader;
    hash_offset_base[6] = bcmFieldDataOffsetBaseFourthHeader;
    hash_offset_base[7] = bcmFieldDataOffsetBaseFourthHeader;
    
    key_dq_qualifier[0] = bcmFieldQualifyHashValue;
    key_dq_length[0] = 32; /*CRC-D + CRC-C*/
    key_dq_offset[0] = 0;
    key_dq_qualifier[1] = bcmFieldQualifyHashValue;
    key_dq_length[1] = 32;
    key_dq_offset[1] = 32; /*CRC-A+CRC-B*/
    key_dq_qualifier[2] = bcmFieldQualifyHashValue;
    key_dq_length[2] = 10;
    key_dq_offset[2] = 16; /*CRC-C*/
    
    rv = field_preselector_extension_create(unit, &psset, preselector_header_format);
    if (rv != BCM_E_NONE)
    {
        printf("Error, field_preselector_extension_create - (%d)\n", rv);
        return rv;
    }
    /*
     * 2. Define all the data qualifiers
     */
    rv = field_data_qualifier_array_create(unit, hash_dq_nof, hash_dq_qualifier, hash_dq_offset, hash_offset_base, hash_dq_length,
                BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES, qual_ids);
    if (rv != BCM_E_NONE)
    {
        printf("Error, field_data_qualifier_array_create - (%d) \n", rv);
        return rv;
    }
    rv = field_group_create(unit, group_id[0], hash_dq_nof, qual_ids, 1, psset);
    if (rv != BCM_E_NONE)
    {
        printf("Error field_group_create - (%d)\n", rv);
        return rv;
    }

    rv = field_data_qualifier_array_create(unit, key_dq_nof, key_dq_qualifier, key_dq_offset, key_offset_base, key_dq_length,
                BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES, qual_ids);
    if (rv != BCM_E_NONE)
    {
        printf("Error, field_data_qualifier_array_create - (%d) \n", rv);
        return rv;
    }
    rv = field_group_create(unit, group_id[1], key_dq_nof, qual_ids, 0, psset);
    if (rv != BCM_E_NONE)
    {
        printf("Error field_group_create - (%d)\n", rv);
        return rv;
    }
    rv = bcm_switch_control_set(unit, bcmSwitchECMPHashConfig, BCM_HASH_CONFIG_CRC16_0x84a1);
    if (rv != BCM_E_NONE) {
        printf ("bcm_petra_switch_control_set with type bcmSwitchECMPHashConfig failed: %d \n", rv);
    }
    rv = bcm_switch_control_set(unit, bcmSwitchECMPHashOffset, 14);
    if (rv != BCM_E_NONE) {
        printf ("bcm_petra_switch_control_set with type bcmSwitchECMPHashOffset failed: %d \n", rv);
    }
    return rv;
}

/* This function creates a field group using the arguments that have been passed to it.
 * It adds the qualifiers (using their IDs) to the group.
 *          * group_id - the ID of the group that will be created.
 *          * dq_ndx_max - number of data qualifiers that are passed to the function.
 *          * *qual_ids - an array of IDs of the data qualifiers.
 *          * is_trunk_hash - boolean variable that marks if a trunk or ecmp group is going to be created.
 *          * psset - the ID of the preselector that this group is going to belong to.
 */
int field_group_create(int unit, int group_id, int dq_ndx_max, int *qual_id, int is_trunk_hash, bcm_field_presel_set_t psset)
{
    printf("group id = %d \n", group_id);
    int rv = BCM_E_NONE;
    int dq_ndx;
    bcm_field_group_config_t group_config;
    group_config.group = group_id;
    group_config.priority = group_id*2;
    /* Define the QSET */
    BCM_FIELD_QSET_INIT(group_config.qset);
    BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageHash);
    for (dq_ndx = 0; dq_ndx < dq_ndx_max; dq_ndx++) {
        /* Add the Data qualifier to the QSET */
        rv = bcm_field_qset_data_qualifier_add(unit, &group_config.qset, qual_id[dq_ndx]);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_field_qset_data_qualifier_add DQ %d\n", dq_ndx);
            return rv;
        }
    }

    /*
     *  Define the ASET - use counter 0.
     */
    BCM_FIELD_ASET_INIT(group_config.aset);
    /* for pre-hashing vector - bcmFieldActionHashValueSet
     * for post-hashing vector - bcmFieldActionTrunkHashKeySet
     */
    if (is_trunk_hash == 1)
    {
        BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionHashValueSet);
    }
    else
    {
        BCM_FIELD_ASET_ADD(group_config.aset, bcmFieldActionTrunkHashKeySet);
    }

    /*
     *  Create the field group with direct extraction
     */
    group_config.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ID | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET | BCM_FIELD_GROUP_CREATE_WITH_ASET;
    group_config.mode = bcmFieldGroupModeHashing;
    group_config.preselset = psset;
    rv = bcm_field_group_config_create(unit, &group_config);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_field_group_create\n");
        return rv;
    }
    return rv;
}

/* This function creates a preselector for a program in hashing stage.
 * It uses a bcm_field_header_format_t variable to define the header format.
 * It accepts several parameters -
 *          * psset - a callback variable of type bcm_field_presel_set_t that would be used to return the created presel
 *          * preselector_header_format -  a member of the bcm_field_header_format_extension_t enumeration that would
 *                                          determine the wanted header type for the program
 */
int field_preselector_create(int unit, bcm_field_presel_set_t *psset, bcm_field_header_format_t preselector_header_format)
{
    int rv = BCM_E_NONE;
    /*Pre selector*/
    int presel_flags = 0;
    /*
     * Determine if advanced mode is active in order to create a preselector.
     */
    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        presel_flags = BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_HASH;
        rv = bcm_field_presel_create_stage_id(unit, bcmFieldStageHash, presel_id);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_field_presel_create_stage_id\n");
        }
    } else {
        rv = bcm_field_presel_create(unit, &presel_id);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_field_presel_create\n");
            return rv;
        }
    }
    /*
     * Define stage in which the qualification will occur.
     */
    rv = bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldStageHash);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_qualify_Stage\n");
        return rv;
    }

    /*
     * Define the identification of packets based on their header format using a header format extension enum member.
     */
    rv = bcm_field_qualify_HeaderFormat(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, preselector_header_format);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_qualify_HeaderFormat\n");
        return rv;
    }

    /*
     * Initialise the presel_set and fill it with the created presel
     */
    BCM_FIELD_PRESEL_INIT(*psset);
    BCM_FIELD_PRESEL_ADD(*psset, presel_id);
    presel_id++;
    return rv;
}


/* This function creates a preselector for a program in hashing stage.
 * It uses a header format extension to define the header format.
 * It accepts several parameters -
 *          * psset - a callback variable of type bcm_field_presel_set_t that would be used to return the created presel
 *          * preselector_header_format -  a member of the bcm_field_header_format_extension_t enumeration that would
 *                                          determine the wanted header type for the program
 */
int field_preselector_extension_create(int unit, bcm_field_presel_set_t *psset, bcm_field_header_format_extension_t preselector_header_format)
{
    int rv = BCM_E_NONE;
    /*Pre selector*/
    int presel_flags = 0;
    /*
     * Determine if advanced mode is active in order to create a preselector.
     */
    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        presel_flags = BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_HASH;
        rv = bcm_field_presel_create_stage_id(unit, bcmFieldStageHash, presel_id);
        printf("Created presel stage_id = %d \n", presel_id);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_field_presel_create_stage_id\n");
        }
    }
    else
    {
        presel_id = 3;
        rv = bcm_field_presel_create(unit, &presel_id);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_field_presel_create\n");
            return rv;
        }
    }
    /*
     * Define stage in which the qualification will occur.
     */
    rv = bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldStageHash);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_qualify_Stage\n");
        return rv;
    }
        rv = bcm_field_qualify_HeaderFormat(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldHeaderFormatMplsAnyLabelAnyL2L3);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_qualify_HeaderFormat\n");
        return rv;
    }
    /*
     * Define the identification of packets based on their header format using a header format extension enum member.
     */
    rv = bcm_field_qualify_HeaderFormatExtension(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, preselector_header_format);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_qualify_HeaderFormatExtension\n");
        return rv;
    }
    /*
     * Initialise the presel_set and fill it with the created presel
     */
    BCM_FIELD_PRESEL_INIT(*psset);
    BCM_FIELD_PRESEL_ADD(*psset, presel_id);
    presel_id++;
    return rv;
}


/* This function creates an array of data qualifiers that will be used for the creation of a group at a later point.
 * It accepts several parameters:
 *          * nof_dq - number of data qualifiers - each of the other arrays passed to this function should have this number of members defined.
 *          * *dq_qualifiers - an array with the IDs of the DQ that the user wants to create out of the members of bcm_field_qualify_t enum.
 *          * *dq_offsets - an array of uint16 that have a meaning of offset in bits/bytes.
 *          * *dq_offset_base - an array of integers that have a meaning of offset base. If using OFFSET_PREDEFINED flag, need to provide 0 to all members of the array.
 *                              The members of this array should be members of bcm_field_data_offset_base_t enum.
 *          * *dq_length - an array of uint32 whose members have a meaning of the length in bits/bytes that will be taken into account for the DQ.
 *          * flags - flags that will be used when creating the data qualifier.
 *          * *qual_ids - an array of integers that will be returned to the calling function. This array will hold the IDs of the created data qualifiers.
 */
int field_data_qualifier_array_create(int unit, int nof_dq, int *dq_qualifiers, uint16 *dq_offsets, int *dq_offset_base, uint32 *dq_length, uint32 flags, int * qual_ids)
{
    int rv = BCM_E_NONE;
    int dq_ndx;
    bcm_field_data_qualifier_t key_dq_data_qual[nof_dq];
    for (dq_ndx = 0; dq_ndx < nof_dq; dq_ndx++)
    {
        bcm_field_data_qualifier_t_init(&key_dq_data_qual[dq_ndx]);
        key_dq_data_qual[dq_ndx].flags = flags;
        key_dq_data_qual[dq_ndx].offset_base = dq_offset_base[dq_ndx];
        key_dq_data_qual[dq_ndx].offset = dq_offsets[dq_ndx];
        key_dq_data_qual[dq_ndx].stage = bcmFieldStageHash;
        key_dq_data_qual[dq_ndx].qualifier = dq_qualifiers[dq_ndx];
        key_dq_data_qual[dq_ndx].length = dq_length[dq_ndx];
        rv = bcm_field_data_qualifier_create(unit, &key_dq_data_qual[dq_ndx]);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_field_data_qualifier_create on ID %d \n", dq_ndx);
            return rv;
        }
        printf("qualifier ID : %d \n", key_dq_data_qual[dq_ndx].qual_id);
        qual_ids[dq_ndx] = key_dq_data_qual[dq_ndx].qual_id;
    }

    return rv;
}

