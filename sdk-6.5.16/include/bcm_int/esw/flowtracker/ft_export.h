/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        ft_export.h
 * Purpose:     Function declarations for flowtracker Export routines.
 */

#ifndef _BCM_INT_FT_EXPORT_H_
#define _BCM_INT_FT_EXPORT_H_

#include <soc/defs.h>
#include <shared/bsl.h>

#if defined(BCM_FLOWTRACKER_SUPPORT)

#include <shared/bitop.h>

#include <soc/helix5.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/shared/shr_pkt.h>

#include <soc/mem.h>
#include <soc/format.h>

#include <bcm/module.h>
#include <soc/scache.h>

#include <bcm/error.h>
#include <bcm/flowtracker.h>
#include <bcm/collector.h>

#include <bcm_int/esw/collector.h>


/*
 * Global defines.
 */
#define BCMI_FT_MAX_COLLECTORS        8
#define BCMI_FT_COLLECTOR_ID_INVALID  -1

#define BCMI_FT_IPFIX_PACKET_BUILD_MAX_SIZE  128
#define BCMI_FT_IPFIX_MAX_RECORDS_PER_PACKET 128
#define BCMI_FT_IPFIX_RECORD_SIZE            (1024 / 8)

#define BCMI_FT_MAX_TEMPLATES    1024
#define BCMI_FT_TEMPLATE_ID_INVALID  -1
#define BCMI_FT_TEMPLATE_MAX_ELEMS 128

#define BCMI_FT_CB_MAX 3

/* Maximum name length of export elements. */
#define BCMI_FT_EXPORT_ELEMENT_NAME_LEN_MAX       30

/* Structures */

typedef struct bcmi_ft_collector_info_s {

    bcm_collector_t cmn_coll_id;
    int             cmn_export_prof_id;
    int             num_records;

    /* Software handle for collector. */
    bcm_flowtracker_collector_t coll_hw_id;

    /* Number of groups associated with collector. */
    int in_use;

    /* Collector information structure. */
    bcm_flowtracker_collector_info_t collector_info;

    /* IPFIX header */
    int ipfix_hdr_use_custom;
    bcm_collector_ipfix_header_t ipfix_hdr;

} bcmi_ft_collector_info_t;

typedef struct bcmi_ft_template_info_s {
    /* Template ID.  */
    bcm_flowtracker_export_template_t template_id;

    /* Number of groups associated with template. */
    int in_use;

    /* IPFIX Template Set Id. */
    uint16 set_id;

    /* Num elements.  */
    int num_elements;

    /* List of export elements.  */
    bcm_flowtracker_export_element_info_t export_elements[BCMI_FT_TEMPLATE_MAX_ELEMS];

} bcmi_ft_template_info_t;


/* Index into array of profile structures */
typedef enum bcmi_ft_export_cb_state_e {
    BCMI_FT_CALLBACK_STATE_INVALID = 0,
    BCMI_FT_CALLBACK_STATE_ACTIVE,
    BCMI_FT_CALLBACK_STATE_REGISTERED,
    BCMI_FT_CALLBACK_STATE_UNREGISTERED
} bcmi_ft_export_cb_state_t;

typedef struct bcmi_ft_cb_entry_s {
    VOL bcmi_ft_export_cb_state_t state;

    bcm_flowtracker_export_record_cb_f callback;
    bcm_flowtracker_collector_callback_options_t callback_options;

    void *userdata;
} bcmi_ft_cb_entry_t;

typedef struct bcmi_ft_export_cb_ctrl_s {
    /* per unit profiles array is indexed by _bcm_ipfix_profile_id_t */
    bcmi_ft_cb_entry_t callback_entry[BCMI_FT_CB_MAX];
    sal_thread_t pid;          /* export fifo processing thread process id */
    VOL sal_usecs_t interval;  /* export fifo processing polling interval */
    int dma_chan;           /* export fifo DMA channel number */

    int bad_handled;  /* Records not properly handled.  */
    int owned;        /* Records owned succesfully.  */
} bcmi_ft_export_cb_ctrl_t;



/*
 * FT Export (IPFIX) Packet build
 */
typedef struct bcmi_ft_pkt_build_s {
    /* Flags */
    uint32 flags;

    /* Checksum of IP header excluding length */
    uint32 ip_base_checksum;

    /* UDP psuedo header checksum */
    uint32 udp_base_checksum;

    /* Software handle for collector */
    uint16 id;

    /* Length of the IPFIX encapsulation */
    uint16 encap_length;

    /* Max size of the packet that can be sent to collector */
    uint16 mtu;

    /* Offset to start of IP header in the encap */
    uint16 ip_offset;

    /* Offset to start of UDP header in the encap */
    uint16 udp_offset;

    /* Offset to start of IPFIX header in the encap */
    uint16 ipfix_offset;

    /* IPFIX encapsulation */
    uint8 encap[BCMI_FT_IPFIX_PACKET_BUILD_MAX_SIZE];
} bcmi_ft_pkt_build_t;


typedef struct bcmi_ft_export_satet_s {
    /* IPFIX export Record length (bytes). */
    int record_length;

    /* Max collectors supported by device. */
    int max_collectors;

    /* Max templates. */
    int max_templates;

    /* Max information elements in a template. */
    int max_elements;

    /* Template valid bitmap.  */
    SHR_BITDCL template_bitmap[SHR_BITALLOCSIZE(BCMI_FT_MAX_TEMPLATES)];

    /* Per collector info. */
    bcmi_ft_collector_info_t collector_info[BCMI_FT_MAX_COLLECTORS];

    /* Templates info. */
    bcmi_ft_template_info_t *template_info[BCMI_FT_MAX_TEMPLATES];

    /* DMA Collector Id. */
    bcm_flowtracker_collector_t dma_collector_id;

    /* Export Callback functions' state */
    bcmi_ft_export_cb_ctrl_t *export_cb_ctrl;

    /* IPFIX observation domain id. */
    int ipfix_observation_domain_id;

} bcmi_ft_export_state_t;




/* Export software state. */
extern bcmi_ft_export_state_t *bcmi_ft_export_state[BCM_MAX_NUM_UNITS];




/*
 * Function Macros
 */
#define BCMI_FT_EXPORT_INFO(unit) \
            (bcmi_ft_export_state[unit])

#define BCMI_FT_COLLECTOR_INFO(unit, id) \
            &(BCMI_FT_EXPORT_INFO(unit)->collector_info[id])

#define BCMI_FT_TEMPLATE_BITMAP(unit) \
            (BCMI_FT_EXPORT_INFO(unit)->template_bitmap)

#define BCMI_FT_TEMPLATE_INFO(unit, id) \
            (BCMI_FT_EXPORT_INFO(unit)->template_info[id])

#define BCMI_FT_CB_CTRL(unit) \
            (BCMI_FT_EXPORT_INFO(unit) == NULL ? NULL : (BCMI_FT_EXPORT_INFO(unit)->export_cb_ctrl))





/* Function Declarations */

extern void
bcma_ft_dump_export_entry(int unit,
                          bcm_flowtracker_export_record_t *data);

extern int
bcma_ft_export_fifo_control(int unit, sal_usecs_t interval);

extern int
bcma_ft_export_dma_collector_get(int unit,
                                 bcm_collector_t *id);

extern int
bcma_ft_setid_template_id_get(int unit, uint16 setid,
                              bcm_flowtracker_export_template_t *template_id);
extern int
bcmi_ft_export_init(int unit);

extern int
bcmi_ft_export_cleanup(int unit);

/*
 * Function:
 *     bcmi_ft_collector_create
 * Purpose:
 *     Create flowtracker collector id.
 * Description :
 *     Routine to create a flowtracker collector
 * Parameters:
 *     unit - (IN) BCM device id
 *     options - (IN) options for group creation
 *     collector_id   - (OUT) FT collector id.
 *     collector_info - (IN) Collector info.
 *
 * Returns:
 *     BCM_E_XXX
 */
extern int
bcmi_ft_collector_create(
    int unit,
    uint32 options,
    bcm_flowtracker_collector_t *collector_id,
    bcm_flowtracker_collector_info_t *collector_info);


/*
 * Function:
 *     bcmi_ft_collector_get
 * Purpose:
 *     Fetch flowtracker collector info.
 * Description :
 *     Routine to create a flowtracker collector
 * Parameters:
 *     unit - (IN) BCM device id
 *     collector_id   - (OUT) FT collector id.
 *     collector_info - (IN) Collector info.
 *
 * Returns:
 *     BCM_E_XXX
 */
extern int
bcmi_ft_collector_get(
    int unit,
    bcm_flowtracker_collector_t collector_d,
    bcm_flowtracker_collector_info_t *collector_info);

/*
 * Function:
 *     bcmi_ft_collector_get_all
 * Purpose:
 *     Fetch all flowtracker collectors info.
 * Description :
 *     Routine to create a flowtracker collector
 * Parameters:
 *     unit - (IN) BCM device id
 *     max_size   - (OUT) Max collectors to read.
 *     collector_list - (OUT) Collector id list.
 *     list_size (OUT) - Num of collectors read.
 *
 * Returns:
 *     BCM_E_XXX
 */
extern int
bcmi_ft_collector_get_all(
    int unit,
    int max_size,
    bcm_flowtracker_collector_t *collector_list,
    int *list_size);

/*
 * Function:
 *     bcmi_ft_collector_destroy
 * Purpose:
 *     Destroy flowtracker collector.
 * Description :
 *     Routine to destroy a flowtracker collector
 * Parameters:
 *     unit - (IN) BCM device id
 *     collector_d   - (OUT) FT collector id.
 *
 * Returns:
 *     BCM_E_XXX
 */
extern int
bcmi_ft_collector_destroy(
    int unit,
    bcm_flowtracker_collector_t collector_d);

/*
 * Function:
 *      bcm_flowtracker_export_template_validate
 * Purpose:
 *      Validate the template and return the list supported by the
 *      device.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flow_group_id - (IN) Flowtracker flow group software ID.
 *      max_in_export_elements - (IN) Max number of export elements intended in the tempate.
 *      in_export_elements - (IN) List of export elements intended to be in the template.
 *      max_out_export_elements - (IN) Max number of export elements in the supportable tempate.
 *      out_export_elements - (OUT) List of export elements in the template supportable by hardware.
 *      actual_out_export_elements - (OUT) Actual number of export elements in the supportable tempate.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
extern int
bcmi_ft_export_template_validate(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    int max_in_export_elements,
    bcm_flowtracker_export_element_info_t *in_export_elements,
    int max_out_export_elements,
    bcm_flowtracker_export_element_info_t *out_export_elements,
    int *actual_out_export_elements);
/*
 * Function:
 *     bcmi_ft_template_create
 * Purpose:
 *     Create flowtracker export template.
 * Description :
 *     Routine to create a flowtracker template
 * Parameters:
 *     unit - (IN) BCM device id
 *     options - (IN) options for template creation
 *     template_id   - (OUT) FT template id.
 *     set_id - (IN) IPFIX template set id
 *     num_export_elements - (IN) Number of information elements.
 *     list_of_export_elements - (IN) List of IPFIX information elems.
 *
 * Returns:
 *     BCM_E_XXX
 */
extern int
bcmi_ft_template_create(
    int unit,
    uint32 options,
    bcm_flowtracker_export_template_t *template_id,
    uint16 set_id,
    int num_export_elements,
    bcm_flowtracker_export_element_info_t *list_of_export_elements);


/*
 * Function:
 *     bcmi_ft_template_get
 * Purpose:
 *     Ftech flowtracker export template.
 * Description :
 *     Routine to get flowtracker template info.
 * Parameters:
 *     unit - (IN) BCM device id
 *     template_id   - (IN) FT template id.
 *     *set_id - (OUT) IPFIX template set id
 *     max_size - (IN) Max num of elements
 *     list_of_export_elements - (OUT) List of IPFIX information elems.
 *     num_export_elements - (OUT) Actual Number of information elements.
 *
 * Returns:
 *     BCM_E_XXX
 */
extern int
bcmi_ft_template_get(
    int unit,
    bcm_flowtracker_export_template_t template_id,
    uint16 *set_id,
    int max_elems,
    bcm_flowtracker_export_element_info_t *list_of_elems,
    int *num_elems);


/*
 * Function:
 *     bcmi_ft_template_destroy
 * Purpose:
 *     Destroy flowtracker export template.
 * Description :
 *     Routine to destroy a flowtracker template
 * Parameters:
 *     unit - (IN) BCM device id
 *     template_id   - (OUT) FT template id.
 *
 * Returns:
 *     BCM_E_XXX
 */
extern int
bcmi_ft_template_destroy(
    int unit,
    bcm_flowtracker_export_template_t template_id);


/*
 * Function:
 *     bcmi_ft_flow_group_collector_add
 * Purpose:
 *     Attach a collector a flow group.
 * Description :
 *     Routine to associate a collector to an flowtracker group.
 * Parameters:
 *     unit - (IN) BCM device id
 *     group_id- (IN) FT group Id.
 *     collector_id   - (OUT) FT Collector Id.
 *     template_id - (IN) IPFIX template Id.
 *
 * Returns:
 *     BCM_E_XXX
 */
extern int
bcmi_ft_flow_group_collector_add(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    bcm_flowtracker_collector_t collector_id,
    bcm_flowtracker_export_template_t template_id);


/*
 * Function:
 *     bcmi_ft_flow_group_collector_delete
 * Purpose:
 *     Detach a collector a flow group.
 * Description :
 *     Routine to dissociate a collector to an flowtracker group.
 * Parameters:
 *     unit - (IN) BCM device id
 *     group_id- (IN) FT group Id.
 *     collector_id   - (OUT) FT Collector Id.
 *     template_id - (IN) IPFIX template Id.
 *
 * Returns:
 *     BCM_E_XXX
 */
extern int
bcmi_ft_flow_group_collector_delete(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    bcm_flowtracker_collector_t collector_id,
    bcm_flowtracker_export_template_t template_id);


/*
 * Function:
 *     bcmi_ft_flow_group_collector_get_all
 * Purpose:
 *     Get collector attached to a flow group.
 * Description :
 *     Routine to fetch collector and template id
 *     associated a flowtracker group.
 * Parameters:
 *     unit                 - (IN)  BCM device id
 *     group_id             - (IN)  FT group Id.
 *     max_list_size        - (IN)  max list of collectors
 *     list_of_collectors   - (OUT) FT Collector Id list.
 *     list_of_templates    - (OUT) FT Template Id list.
 *     list_size            - (IN)  Actual list size.
 *
 * Returns:
 *     BCM_E_XXX
 */
extern int
bcmi_ft_flow_group_collector_get_all(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    int max_list_size, 
    bcm_flowtracker_collector_t *list_of_collectors, 
    bcm_flowtracker_export_template_t *list_of_templates, 
    int *list_size);

 
/*
 * Function:
 *      bcmi_ft_group_collector_attach
 * Purpose:
 *      Associate flow group to a collector with an export template.
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      flow_group_id   - (IN)  Flow group Id
 *      collector_id    - (IN)  Collector Id
 *      template_id     - (IN)  Template Id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
extern int
bcmi_ft_group_collector_attach(
        int unit,
        bcm_flowtracker_group_t flow_group_id,
        bcm_flowtracker_collector_t collector_id,
        int export_profile_id,
        bcm_flowtracker_export_template_t template_id);

/*
 * Function:
 *      bcmi_ft_group_collector_detach
 * Purpose:
 *      Dis-associate flow group from a collector with an export template.
 * Parameters:
 *      unit            - (IN)  BCM device number
 *      flow_group_id   - (IN)  Flow group Id
 *      collector_id    - (IN)  Collector Id
 *      template_id     - (IN)  Template Id
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
extern int
bcmi_ft_group_collector_detach(
        int unit,
        bcm_flowtracker_group_t flow_group_id,
        bcm_flowtracker_collector_t collector_id,
        int export_profile_id,
        bcm_flowtracker_export_template_t template_id);

/*
 * Function:
 *      bcmi_ft_group_collector_attach_get_all
 * Purpose:
 *      Get list of all collectors, templates  attached to a flow group
 * Parameters:
 *      unit                - (IN)  BCM device number
 *      flow_group_id       - (IN)  Flow group Id
 *      max_list_size       - (IN)  Size of the list arrays
 *      list_of_collectors  - (OUT) Collector list
 *      list_of_templates   - (OUT) Template list
 *      list_size           - (OUT) Number of elements in the lists
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
extern int
bcmi_ft_group_collector_attach_get_all(
    int unit, 
    bcm_flowtracker_group_t flow_group_id, 
    int max_list_size, 
    bcm_flowtracker_collector_t *list_of_collectors, 
    int *list_of_export_profile_ids,
    bcm_flowtracker_export_template_t *list_of_templates, 
    int *list_size);


extern int
bcmi_ft_export_record_register(
    int unit,
    bcm_flowtracker_collector_t collector_id, 
    bcm_flowtracker_collector_callback_options_t callback_options, 
    bcm_flowtracker_export_record_cb_f callback,
    void *userdata);

extern int
bcmi_ft_export_record_unregister(
    int unit, 
    bcm_flowtracker_collector_t collector_id, 
    bcm_flowtracker_collector_callback_options_t callback_options, 
    bcm_flowtracker_export_record_cb_f callback_fn);


#ifdef BCM_WARM_BOOT_SUPPORT

extern int
bcmi_ft_export_warmboot_alloc(int unit, soc_scache_handle_t scache_handle);

extern int
bcmi_ft_export_warmboot_sync(int unit, soc_scache_handle_t scache_handle);

extern int
bcmi_ft_export_warmboot_recover(int unit, soc_scache_handle_t scache_handle);

#endif /* BCM_WARM_BOOT_SUPPORT */


#endif /*  BCM_FLOWTRACKER_SUPPORT */

#endif /*  _BCM_INT_FT_GROUP_H_ */
