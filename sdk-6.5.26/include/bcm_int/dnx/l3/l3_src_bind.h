/** \file bcm_int/dnx/l3/l3_src_bind.h
 * 
 * Internal DNX L3 source bind APIs 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#ifndef _L3_SRC_BIND_INCLUDED__
/*
 * { 
 */
#define _L3_SRC_BIND_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif /* */
#include <bcm/types.h>
#include <bcm/l3.h>

/*************
 * INCLUDES  *
 *************/
/* { */

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/* } */
/*************
 * MACROS    *
 *************/
/* { */

/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */
typedef struct
{

    /*
     *  Source MAC address 
     */
    bcm_mac_t smac;             /* Source MAC Address. */

    /*
     *  If set to FALSE then SMAC is masked. 
     */
    uint8 smac_valid;

    /*
     *  SIP (Source IP address).  
     */
    uint32 sip;

    /*
     *  SIP (Source IP address) mask.  
     */
    uint32 sip_mask;

    /*
     *  Logical Interface ID.  
     */
    uint32 in_lif_sav;

} L3_SRC_BIND_IPV4_ENTRY;
typedef struct
{

    /*
     *  Source MAC address 
     */
    bcm_mac_t smac;             /* Source MAC Address. */

    /*
     *  If set to FALSE then SMAC is masked. 
     */
    uint8 smac_valid;

    /*
     *  SIP (Source IPv6 address).  
     */
    bcm_ip6_t sip6;

    /*
     *  SIP (Source IPv6 address) mask.  
     */
    bcm_ip6_t sip6_mask;

    /*
     *  Logical Interface ID.  
     */
    uint32 in_lif_sav;

} L3_SRC_BIND_IPV6_ENTRY;

/* } */
/*************
 * GLOBALS   *
 *************/
/* { */

/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

/*
 * Function:
 *      dnx_l3_src_bind_ipv4_entry_init
 * Purpose:
 *      initiate an IPV4 L3 source binding input parameters.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (OUT) L3 source binding information
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */

static shr_error_e dnx_l3_src_bind_ipv4_entry_init(
    int unit,
    L3_SRC_BIND_IPV4_ENTRY * info);

/*
 * Function:
 *      dnx_l3_src_bind_ipv6_entry_init
 * Purpose:
 *      initiate an IPV6 L3 source binding input parameters.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (OUT) L3 source binding information
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */

static shr_error_e dnx_l3_src_bind_ipv6_entry_init(
    int unit,
    L3_SRC_BIND_IPV6_ENTRY * info);

/*
 * Function:
 *      dnx_l3_source_bind_verify
 * Purpose:
 *      verify an L3 source binding input parameters.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) L3 source binding information
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
static shr_error_e dnx_l3_source_bind_verify(
    int unit,
    bcm_l3_source_bind_t * info);

static shr_error_e dnx_l3_src_bind_ipv4_get(
    int unit,
    L3_SRC_BIND_IPV4_ENTRY * src_bind_info,
    uint32 mac_compress_id);

/*
 * Function:
 *      dnx_l3_src_bind_ipv4_add
 * Purpose:
 *      delete ipv4 sav entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      src_bind_info - (IN) ipv4 information
 *      mac_compress_id - (IN) mac address compression ID
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */

shr_error_e dnx_l3_src_bind_ipv4_add(
    int unit,
    L3_SRC_BIND_IPV4_ENTRY * src_bind_info,
    uint32 mac_compress_id);

/*
 * Function:
 *      dnx_l3_src_bind_ipv4_delete
 * Purpose:
 *      delete ipv4 sav entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      src_bind_info - (IN) ipv4 information
 *      mac_compress_id - (IN) mac address compression ID
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */

shr_error_e dnx_l3_src_bind_ipv4_delete(
    int unit,
    L3_SRC_BIND_IPV4_ENTRY * src_bind_info,
    uint32 mac_compress_id);

/*
 * Function:
 *      dnx_l3_src_bind_ipv6_get
 * Purpose:
 *      find if an ipv6 sav entry exists.
 * Parameters:
 *      unit - (IN) Unit number.
 *      src_bind_info - (IN) ipv6 information
 *      mac_compress_id - (IN) mac address compression ID
 *      ip6_msb32_compress_id - (IN) ip6 address compression ID 
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */

static shr_error_e dnx_l3_src_bind_ipv6_get(
    int unit,
    L3_SRC_BIND_IPV6_ENTRY * src_bind_info,
    uint32 mac_compress_id,
    uint32 ip6_msb32_compress_id);

/*
 * Function:
 *      dnx_l3_src_bind_ipv6_add
 * Purpose:
 *      delete ipv6 sav entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      src_bind_info - (IN) ipv6 information
 *      mac_compress_id - (IN) mac address compression ID
 *      ip6_msb32_compress_id - (IN) ip6 address compression ID 
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */

shr_error_e dnx_l3_src_bind_ipv6_add(
    int unit,
    L3_SRC_BIND_IPV6_ENTRY * src_bind_info,
    uint32 mac_compress_id,
    uint32 ip6_msb32_compress_id);

/*
 * Function:
 *      dnx_l3_src_bind_ipv6_delete
 * Purpose:
 *      delete ipv6 sav entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      src_bind_info - (IN) ipv6 information
 *      mac_compress_id - (IN) mac address compression ID
 *      ip6_msb32_compress_id - (IN) ip6 address compression ID 
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */

shr_error_e dnx_l3_src_bind_ipv6_delete(
    int unit,
    L3_SRC_BIND_IPV6_ENTRY * src_bind_info,
    uint32 mac_compress_id,
    uint32 ip6_msb32_compress_id);

/*
 * Function:
 *      dnx_l3_src_mac_compression_get
 * Purpose:
 *      get mac compression  ID.
 * Parameters:
 *      unit - (IN) Unit number.
 *      mac_addr - (IN) mac address
 *      mac_compress_id - (OUT) mac address compression ID
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
static shr_error_e dnx_l3_src_mac_compression_get(
    int unit,
    bcm_mac_t mac_addr,
    uint32 *mac_compress_id);

/*
 * Function:
 *      dnx_l3_src_mac_compression_add
 * Purpose:
 *      add mac compression entry and return an ID.
 * Parameters:
 *      unit - (IN) Unit number.
 *      mac_addr - (IN) mac address
 *      mac_compress_id - (OUT) mac address compression ID
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */

static shr_error_e dnx_l3_src_mac_compression_add(
    int unit,
    bcm_mac_t mac_addr,
    uint32 *mac_compress_id);

/*
 * Function:
 *      dnx_l3_src_mac_compression_delete
 * Purpose:
 *      delete mac compression ID.
 * Parameters:
 *      unit - (IN) Unit number.
 *      mac_addr - (IN) mac address
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */

shr_error_e dnx_l3_src_mac_compression_delete(
    int unit,
    bcm_mac_t mac_addr);

/*
 * Function:
 *      dnx_l3_src_ip6_msb32_compression_get
 * Purpose:
 *      get ip6_msb32_compression ID.
 * Parameters:
 *      unit - (IN) Unit number.
 *      ip6_addr - (IN) ipv6 address
 *      ip6_msb32_compress_id - (OUT) ipv6 address compression ID
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */

shr_error_e dnx_l3_src_ip6_msb32_compression_get(
    int unit,
    bcm_ip6_t ip6_addr,
    uint32 *ip6_msb32_compress_id);

/*
 * Function:
 *      dnx_l3_src_ip6_msb32_compression_add
 * Purpose:
 *      delete ip6_msb32_compression entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      ip6_addr - (IN) ipv6 address
 *      ip6_msb32_compress_id - (OUT) ipv6 address compression ID
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */

shr_error_e dnx_l3_src_ip6_msb32_compression_add(
    int unit,
    bcm_ip6_t ip6_addr,
    uint32 *ip6_msb32_compress_id);

/*
 * Function:
 *      dnx_l3_src_ip6_msb32_compression_delete
 * Purpose:
 *      delete ip6_msb32_compression entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      ip6_addr - (IN) ipv6 address
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */

shr_error_e dnx_l3_src_ip6_msb32_compression_delete(
    int unit,
    bcm_ip6_t ip6_addr);

/*
 * Function:
 *      dnx_l3_src_ip6_msb32_compression_clear
 * Purpose:
 *      clear ip6_msb32_compression entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      ip6_addr - (IN) ipv6 address
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */

static shr_error_e dnx_l3_src_ip6_msb32_compression_clear(
    int unit);

/*
 * Function:
 *      dnx_l3_src_in_lif_profile_get
 * Purpose:
 *      get an L3 source binding new in_lif_profile and in_lif_profile info.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) L3 source binding information
 *      inlif_dbal_table_id - (IN) AC table id
 *      in_lif_profile - (OUT) in_lif_profile of the gport
 *      in_lif_profile_info - (OUT) in_lif_profile information
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */

shr_error_e dnx_l3_src_in_lif_profile_get(
    int unit,
    bcm_gport_t port,
    dbal_tables_e * inlif_dbal_table_id,
    uint32 *in_lif_sav,
    int *in_lif_profile,
    in_lif_profile_info_t * in_lif_profile_info);

/*
 * Function:
 *      dnx_l3_src_in_lif_profile_set
 * Purpose:
 *      set an L3 source binding cs_profile.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) L3 source binding information
 *      inlif_dbal_table_id - (IN) AC table id
 *      in_lif_profile - (IN) in_lif_profile of the gport
 *      in_lif_profile_info - (IN) in_lif_profile information
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */

shr_error_e dnx_l3_src_in_lif_profile_set(
    int unit,
    bcm_gport_t port,
    dbal_tables_e inlif_dbal_table_id,
    int in_lif_profile,
    in_lif_profile_info_t * in_lif_profile_info);

/*
 * Function:
 *      dnx_l3_src_bind_ipv6_delete_all
 * Purpose:
 *      delete all ipv6 sav table
 * Parameters:
 *      unit - (IN) Unit number.
 *      is_dhcp - (IN) if the table is sav DHCP table
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */

static shr_error_e dnx_l3_src_bind_ipv6_delete_all(
    int unit,
    int is_dhcp);

/*
 * Function:
 *      dnx_l3_src_bind_ipv4_delete_all
 * Purpose:
 *      delete all ipv4 sav table
 * Parameters:
 *      unit - (IN) Unit number.
 *      is_dhcp - (IN) if the table is sav DHCP table
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */

static shr_error_e dnx_l3_src_bind_ipv4_delete_all(
    int unit,
    int is_dhcp);

#endif /* */
