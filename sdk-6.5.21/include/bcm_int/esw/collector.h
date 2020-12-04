/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        collector.h
 * Purpose:     
 */
#ifndef _BCM_INT_COLLECTOR_H_
#define _BCM_INT_COLLECTOR_H_

#if defined(BCM_COLLECTOR_SUPPORT)

#include <bcm/collector.h>
#include <shared/idxres_fl.h>
#include <soc/shared/shr_pkt.h>


/* Maximum number of collector export profiles. */
#define BCM_INT_COLLECTOR_MAX_EXPORT_PROFILES 32

/* Collector Export profile Start Id */
#define BCM_INT_COLLECTOR_EXPORT_PROFILE_START_ID 0

/* Collector End Id */
#define BCM_INT_COLLECTOR_EXPORT_PROFILE_END_ID \
    (BCM_INT_COLLECTOR_EXPORT_PROFILE_START_ID + \
     BCM_INT_COLLECTOR_MAX_EXPORT_PROFILES - 1)

/* Invalid export profile Id */
#define BCM_INT_COLLECTOR_INVALID_EXPORT_PROFILE_ID (-1)

/* Default max export length is 1500 bytes */
#define BCM_INT_COLLECTOR_DEF_MAX_EXPORT_LENGTH 1500

/* Max export length supported is 9K - Jumbo */
#define BCM_INT_COLLECTOR_MAX_EXPORT_LENGTH 9000

/* Maximum number of collectors. */
#define BCM_INT_COLLECTOR_MAX_COLLECTORS 32

/* Collector Start Id */
#define BCM_INT_COLLECTOR_COLLECTOR_START_ID 0

/* Collector End Id */
#define BCM_INT_COLLECTOR_COLLECTOR_END_ID \
    (BCM_INT_COLLECTOR_COLLECTOR_START_ID + BCM_INT_COLLECTOR_MAX_COLLECTORS - 1)

/* Invalid collector Id */
#define BCM_INT_COLLECTOR_INVALID_COLLECTOR_ID (-1)

/* Default value of export interval. 100 ms */
#define BCM_INT_COLLECTOR_DEF_EXPORT_INTERVAL_MSECS (100)

/* Minimum length of the collecotor encap
 * UDP_HDR  =  8 bytes
 * IPv4_HDR = 20 bytes
 * ETH_HDR  = 14 bytes
 * L2_CRC   =  4 bytes
 * 8 + 20 + 14 + 4 = 46
 */
#define BCM_INT_COLLECTOR_MIN_ENCAP_LENGTH 46

/* Minimum export length required */
#define BCM_INT_COLLECTOR_MIN_EXPORT_LENGTH 128

/*
 * Macro:
 *     _BCM_COLLECTOR_ALLOC
 * Purpose:
 *      Generic memory allocation routine.
 * Parameters:
 *    _ptr_     - Pointer to allocated memory.
 *    _ptype_   - Pointer type.
 *    _size_    - Size of heap memory to be allocated.
 *    _descr_   - Information about this memory allocation.
 */
#define _BCM_COLLECTOR_ALLOC(_ptr_,_ptype_,_size_,_descr_)               \
            do {                                                         \
                if (NULL == (_ptr_)) {                                   \
                   (_ptr_) = (_ptype_ *) sal_alloc((_size_), (_descr_)); \
                }                                                        \
                if((_ptr_) != NULL) {                                    \
                    sal_memset((_ptr_), 0, (_size_));                    \
                }  else {                                                \
                    LOG_ERROR(BSL_LS_BCM_COLLECTOR, \
                              (BSL_META("Collector Error: Allocation"    \
                              " failure %s\n"),                          \
                               (_descr_)));                              \
                }                                                        \
            } while (0)

#define BCM_INT_COLLECTOR_INFO_FLAGS_FLOW_START_TS (1 << 0)

/* Internal data structure to store
 * collector export profile information.
 */
typedef struct _bcm_int_export_profile_info_s {

    /* flags, See BCM_INT_COLLECTOR_INFO_FLAGS_XXX */
    uint32 flags;

    /* wire format of export profile */
    bcm_collector_wire_format_t wire_format;    

    /* Export interval (usecs) */
    uint32 export_interval;

    /* The maximum possible export packet size
     * used to send stats info to collector
     */
    uint16 max_export_pkt_length;

} _bcm_int_export_profile_info_t;

/* Internal data structure to store
 * collector global information.
 */
typedef struct _bcm_collector_global_info_s {

    /* flags, See BCM_INT_COLLECTOR_INFO_FLAGS_XXX */
    uint32 flags;

    /* Export profile ID pool */
    shr_idxres_list_handle_t export_profile_pool;

    /* Collector ID pool */
    shr_idxres_list_handle_t collector_pool;

} _bcm_collector_global_info_t;

/* Internal data structure to store collector export profile information
 */
typedef struct _bcm_collector_export_profile_info_s {
    /* Software ID of the collector export profile*/
    int export_profile_id;
    /* Collector export profile information structure */
    bcm_collector_export_profile_t export_profile_info;

    /* Num users for this export profile */
    int ref_count;
} _bcm_collector_export_profile_info_t;

/* Internal enumeration for the users of common collector
 * infrastructure.
 */
typedef enum _bcm_collector_export_app_e {
    _BCM_COLLECTOR_EXPORT_APP_NONE = 0,
    _BCM_COLLECTOR_EXPORT_APP_FT_IPFIX = 1,
    _BCM_COLLECTOR_EXPORT_APP_FT_PROTOBUF = 2,
    _BCM_COLLECTOR_EXPORT_APP_ST_PROTOBUF = 3,
    _BCM_COLLECTOR_EXPORT_APP_IFA2_IPFIX = 4
} _bcm_collector_export_app_t;


/* Internal data structure to store per collector information
 */
typedef struct _bcm_collector_info_s {
    /* Software ID of the collector */
    bcm_collector_t collector_id;
    /* Collector information structure */
    bcm_collector_info_t collector_info;

    /* Num users for this collectors */
    int ref_count;

    _bcm_collector_export_app_t user;
} _bcm_collector_info_t;


/* Global data structures extern declarations */
extern _bcm_collector_global_info_t *collector_global_info[BCM_MAX_NUM_UNITS];

/* Utility functions */
extern void _bcm_esw_collector_sw_dump(int unit);

/* Gets the COLLECTOR_GLOBAL_INFO struct for the unit */
#define COLLECTOR_GLOBAL_INFO_GET(_u) collector_global_info[(_u)]

/* Collector module APIs */
extern int _bcm_xgs5_collector_udp_header_get(int unit,
                bcm_collector_info_t *collector_info,
                shr_udp_header_t *udp);
extern int _bcm_xgs5_collector_ipv4_header_get(int unit, uint8 protocol,
                              uint8 tos, uint8 ttl,
                              bcm_ip_t src_ip_addr, bcm_ip_t dst_ip_addr,
                              shr_ipv4_header_t *ip);
extern int _bcm_xgs5_collector_ip_headers_get(int unit, 
                             bcm_collector_info_t *collector_info,
                             uint16 *etype,
                             shr_ipv4_header_t *ipv4);
extern int _bcm_xgs5_collector_l2_header_get(int unit, 
                           bcm_collector_info_t *collector_info,
                           uint16 etype, shr_l2_header_t *l2);
extern int _bcm_xgs5_collector_init(int unit);
extern int _bcm_xgs5_collector_detach(int unit);
extern int _bcm_xgs5_collector_create(int unit,
                                uint32 options,
                                bcm_collector_t *id,
                                bcm_collector_info_t *collector_info);
extern int _bcm_xgs5_collector_get(int unit,
                             bcm_collector_t id,
                             bcm_collector_info_t *collector_info);
extern int _bcm_xgs5_collector_destroy(int unit, bcm_collector_t id);
extern int _bcm_xgs5_collector_ids_get_all(int unit, int max_size,
                           bcm_collector_t *collector_list,
                           int *list_size);
extern int _bcm_xgs5_collector_export_profile_create(int unit,
                          uint32 options, int *id,
                          bcm_collector_export_profile_t *export_profile_info);
extern int _bcm_xgs5_collector_export_profile_get(int unit, int id,
                          bcm_collector_export_profile_t *export_profile_info);
extern int _bcm_xgs5_collector_export_profile_ids_get_all(int unit,
                           int max_size,
                           int *collector_list,
                           int *list_size);
extern int _bcm_xgs5_collector_export_profile_destroy(int unit, int id);

extern int _bcm_xgs5_collector_export_record_register(
    int unit,
    bcm_collector_t collector_id,
    bcm_collector_callback_options_t callback_options,
    bcm_collector_export_record_cb_f callback_fn,
    void *userdata);
extern int _bcm_xgs5_collector_export_record_unregister(
    int unit,
    bcm_collector_t collector_id,
    bcm_collector_callback_options_t callback_options,
    bcm_collector_export_record_cb_f callback_fn);

extern int _bcm_esw_collector_sync(int unit);
extern int _bcm_xgs5_collector_sync(int unit);
extern void _bcm_esw_collector_sw_dump(int unit);
extern void _bcm_xgs5_collector_sw_dump(int unit);
extern int _bcm_esw_collector_export_profile_ref_count_update(int unit,
                                                              int id,
                                                              int update);
extern int _bcm_xgs5_collector_export_profile_ref_count_update(int unit,
                                                               int id,
                                                               int update);
extern int _bcm_esw_collector_ref_count_update(int unit,
                                               bcm_collector_t id,
                                               int update);
extern int _bcm_xgs5_collector_ref_count_update(int unit,
                                                bcm_collector_t id,
                                                int update);
extern int _bcm_esw_collector_user_update(int unit,
                                               bcm_collector_t id,
                                               _bcm_collector_export_app_t user);
extern int _bcm_xgs5_collector_user_update(int unit,
                                               bcm_collector_t id,
                                               _bcm_collector_export_app_t user);
extern uint8 _bcm_esw_collector_check_user_is_other(int unit,
                                                    bcm_collector_t id,
                                                    _bcm_collector_export_app_t my_user); 
extern uint8 _bcm_xgs5_collector_check_user_is_other(int unit,
                                                    bcm_collector_t id,
                                                    _bcm_collector_export_app_t my_user); 
extern int _bcm_esw_collector_ref_count_get(int unit,
                                               bcm_collector_t id);
extern int _bcm_xgs5_collector_ref_count_get(int unit,
                                                bcm_collector_t id);
#endif /* BCM_COLLECTOR_SUPPORT */
extern int _bcm_esw_collector_sync(int unit);

#endif  /* !_BCM_INT_COLLECTOR_H_ */
