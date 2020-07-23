/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BFCMAP_H
#define BFCMAP_H

#include <shared/fcmap.h>

/*
 * Initilaize the BFCMAP module.
 */
extern int bfcmap_init(void);

/*
 * Attach console/debug functions to BFCMAP module.
 */
typedef _shr_bfcmap_print_fn bfcmap_print_fn;

#define BFCMAP_PRINT_DEBUG      BPRINT_FN_DEBUG
#define BFCMAP_PRINT_PRINTF     BPRINT_FN_PRINTF

/*
 * The following function sets/gets the print function pointers for
 * BFCMAP module. If the flag to this function is Zero, no printf are
 * done. If the flag is BFCMAP_PRINT_DEBUG, it sets the print function
 * pointer for the BFCMAP driver and API module.
 * If the flag is BFCMAP_PRINT_PRINTF, it sets the function pointer
 * for all other non-debug printf.
 */
extern int 
bfcmap_config_print_set(buint32_t flag, bfcmap_print_fn print_cb_fn);

extern int 
bfcmap_config_print_get(buint32_t *flag, bfcmap_print_fn *print_fn_cb);


/*
 * Enum for traffic direction. 
 * Ingress traffic is defined as the one coming from the Line side.
 * Egress traffic is defined as coming from the switch/ASIC side.
 */
typedef enum {
        BFCMAP_DIR_EGRESS  = _SHR_BFCMAP_DIR_EGRESS ,
        BFCMAP_DIR_INGRESS = _SHR_BFCMAP_DIR_INGRESS
} bfcmap_dir_t;

/*
 * Identifiers for BFCMAP core type.
 */
typedef enum {
    BFCMAP_CORE_UNKNOWN = _SHR_BFCMAP_CORE_UNKNOWN,
    BFCMAP_CORE_OCTAL_GIG = _SHR_BFCMAP_CORE_OCTAL_GIG,
    BFCMAP_CORE_BCM5458X = _SHR_BFCMAP_CORE_BCM5458X,
    BFCMAP_CORE_BCM5458X_B0 = _SHR_BFCMAP_CORE_BCM5458X_B0,
    BFCMAP_CORE_BCM8729 = _SHR_BFCMAP_CORE_BCM8729,
    BFCMAP_CORE_BCM8483X = _SHR_BFCMAP_CORE_BCM8483X,
    BFCMAP_CORE_BCM5438X = _SHR_BFCMAP_CORE_BCM5438X,
    BFCMAP_CORE_BCM84756 = _SHR_BFCMAP_CORE_BCM84756,
    BFCMAP_CORE_BCM84756_C0 = _SHR_BFCMAP_CORE_BCM84756_C0,
    BFCMAP_CORE_BCM88060_A0 = _SHR_BFCMAP_CORE_BCM88060_A0,
    BFCMAP_CORE_BCM88061_A0 = _SHR_BFCMAP_CORE_BCM88061_A0
} bfcmap_core_t;




/* BFCMAP port is Enabled. Default is disabled. */
#define BFCMAP_PORT_ENABLE                               0x0000001

/* Single secure channel i.e. point-to-point configuration or LAN mode */
#define BFCMAP_PORT_SINGLE_CHANNEL                       0x0000002

/* Ingress Store-forward mode. Default is cut-through mode. 
 * In cut-through mode bfcmap errors are indicated to switch by corrupted CRC,
 * whereas in store-forward mode, error indication can be provided in special 
 * VLAN header. */
#define BFCMAP_PORT_INGRESS_STORE_FORWARD                0x0000004

/* Egress store-forward mode */
#define BFCMAP_PORT_EGRESS_STORE_FORWARD                 0x0000008



/* Port Mode */
typedef enum bfcmap_port_mode_s {
    BFCMAP_FCOE_TO_FC_MODE=_SHR_BFCMAP_FCOE_TO_FC_MODE,
    BFCMAP_FCOE_TO_FCOE_MODE=_SHR_BFCMAP_FCOE_TO_FCOE_MODE
} bfcmap_port_mode_t;

/* Port Speed */
typedef enum bfcmap_port_speed_s  {
    BFCMAP_PORT_SPEED_AN = _SHR_BFCMAP_PORT_SPEED_AN,
    BFCMAP_PORT_SPEED_2GBPS =_SHR_BFCMAP_PORT_SPEED_2GBPS,
    BFCMAP_PORT_SPEED_4GBPS =_SHR_BFCMAP_PORT_SPEED_4GBPS,
    BFCMAP_PORT_SPEED_8GBPS =_SHR_BFCMAP_PORT_SPEED_8GBPS,
    BFCMAP_PORT_SPEED_16GBPS = _SHR_BFCMAP_PORT_SPEED_16GBPS,
    BFCMAP_PORT_SPEED_32GBPS = _SHR_BFCMAP_PORT_SPEED_32GBPS,
    BFCMAP_PORT_SPEED_AN_2GBPS =_SHR_BFCMAP_PORT_SPEED_AN_2GBPS,
    BFCMAP_PORT_SPEED_AN_4GBPS =_SHR_BFCMAP_PORT_SPEED_AN_4GBPS,
    BFCMAP_PORT_SPEED_AN_8GBPS =_SHR_BFCMAP_PORT_SPEED_AN_8GBPS,
    BFCMAP_PORT_SPEED_AN_16GBPS = _SHR_BFCMAP_PORT_SPEED_AN_16GBPS,
    BFCMAP_PORT_SPEED_AN_32GBPS = _SHR_BFCMAP_PORT_SPEED_AN_32GBPS,
    BFCMAP_PORT_SPEED_MAX_COUNT=_SHR_BFCMAP_PORT_SPEED_MAX_COUNT
} bfcmap_port_speed_t;

/* Port State */
typedef enum bfcmap_port_state_s  {
    BFCMAP_PORT_STATE_INIT  = _SHR_BFCMAP_PORT_STATE_INIT,
    BFCMAP_PORT_STATE_RESET = _SHR_BFCMAP_PORT_STATE_RESET,
    BFCMAP_PORT_STATE_ACTIVE = _SHR_BFCMAP_PORT_STATE_ACTIVE,
    BFCMAP_PORT_STATE_LINKDOWN = _SHR_BFCMAP_PORT_STATE_LINKDOWN,
    BFCMAP_PORT_STATE_DISABLE = _SHR_BFCMAP_PORT_STATE_DISABLE,
    BFCMAP_PORT_STATE_MAX_COUNT = _SHR_BFCMAP_PORT_STATE_MAX_COUNT
} bfcmap_port_state_t;

typedef enum {
    BFCMAP_ENCAP_FCOE_FPMA             = 0,/* FPMA, use prefix and Node
                                             ID to construct FCoE MAC Address */
    BFCMAP_ENCAP_FCOE_ETH_ADDRESS_NULL = 1,/* Null address for FCOE MAC */
    BFCMAP_ENCAP_FCOE_ETH_ADDRESS_USER = 2/* User Provided Address */
} bfcmap_encap_mac_address_t;


typedef enum bfcmap_8g_fw_on_active_s {
    BFCMAP_8G_FW_ON_ACTIVE_ARBFF = _SHR_BFCMAP_8G_FW_ON_ACTIVE_ARBFF,
    BFCMAP_8G_FW_ON_ACTIVE_IDLE =  _SHR_BFCMAP_8G_FW_ON_ACTIVE_IDLE  
} bfcmap_8g_fw_on_active_t;


typedef enum bfcmap_map_table_input_e {
    BFCMAP_MAP_TABLE_INPUT_VID = _SHR_BFCMAP_MAP_TABLE_INPUT_VID, 
    BFCMAP_MAP_TABLE_INPUT_VFID = _SHR_BFCMAP_MAP_TABLE_INPUT_VFID 
} bfcmap_map_table_input_t;

typedef enum bfcmap_fc_crc_mode_e {
    BFCMAP_FC_CRC_MODE_NORMAL = _SHR_BFCMAP_FC_CRC_MODE_NORMAL, 
    BFCMAP_FC_CRC_MODE_NO_CRC_CHECK = _SHR_BFCMAP_FC_CRC_MODE_NO_CRC_CHECK 
} bfcmap_fc_crc_mode_t;

typedef enum bfcmap_vfthdr_proc_mode_e {
    BFCMAP_VFTHDR_PRESERVE = _SHR_BFCMAP_VFTHDR_PRESERVE, 
    BFCMAP_VFTHDR_INSERT = _SHR_BFCMAP_VFTHDR_INSERT, 
    BFCMAP_VFTHDR_DELETE = _SHR_BFCMAP_VFTHDR_DELETE 
} bfcmap_vfthdr_proc_mode_t;

typedef enum bfcmap_vlantag_proc_mode_e {
    BFCMAP_VLANTAG_PRESERVE = _SHR_BFCMAP_VLANTAG_PRESERVE, 
    BFCMAP_VLANTAG_INSERT = _SHR_BFCMAP_VLANTAG_INSERT, 
    BFCMAP_VLANTAG_DELETE = _SHR_BFCMAP_VLANTAG_DELETE 
} bfcmap_vlantag_proc_mode_t;

typedef enum bfcmap_vfid_mapsrc_e {
    BFCMAP_VFID_MACSRC_PASSTHRU = _SHR_BFCMAP_VFID_MACSRC_PASSTHRU, 
    BFCMAP_VFID_MACSRC_PORT_DEFAULT = _SHR_BFCMAP_VFID_MACSRC_PORT_DEFAULT, 
    BFCMAP_VFID_MACSRC_VID = _SHR_BFCMAP_VFID_MACSRC_VID, 
    BFCMAP_VFID_MACSRC_MAPPER = _SHR_BFCMAP_VFID_MACSRC_MAPPER,
    BFCMAP_VFID_MACSRC_DISABLE = _SHR_BFCMAP_VFID_MACSRC_DISABLE
} bfcmap_vfid_mapsrc_t;

typedef enum bfcmap_vid_mapsrc_e {
    BFCMAP_VID_MACSRC_PASSTHRU = _SHR_BFCMAP_VID_MACSRC_PASSTHRU, 
    BFCMAP_VID_MACSRC_PORT_DEFAULT = _SHR_BFCMAP_VID_MACSRC_PORT_DEFAULT, 
    BFCMAP_VID_MACSRC_VFID = _SHR_BFCMAP_VID_MACSRC_VFID, 
    BFCMAP_VID_MACSRC_MAPPER = _SHR_BFCMAP_VID_MACSRC_MAPPER 
} bfcmap_vid_mapsrc_t;

typedef enum bfcmap_vlan_pri_map_mode_e {
    BFCMAP_VLAN_PRI_MAP_MODE_PORT_DEFAULT = _SHR_BFCMAP_VLAN_PRI_MAP_MODE_PORT_DEFAULT, 
    BFCMAP_VLAN_PRI_MAP_MODE_PASSTHRU = _SHR_BFCMAP_VLAN_PRI_MAP_MODE_PASSTHRU 
} bfcmap_vlan_pri_map_mode_t;

typedef enum bfcmap_hopcnt_check_mode_e {
    BFCMAP_HOPCNT_CHECK_MODE_NO_CHK = _SHR_BFCMAP_HOPCNT_CHECK_MODE_NO_CHK,
    BFCMAP_HOPCNT_CHECK_MODE_FAIL_DROP = _SHR_BFCMAP_HOPCNT_CHECK_MODE_FAIL_DROP,
    BFCMAP_HOPCNT_CHECK_MODE_FAIL_EOFNI = _SHR_BFCMAP_HOPCNT_CHECK_MODE_FAIL_EOFNI
}  bfcmap_hopcnt_check_mode_t;


/* action_mask/action_mask2 field in  bfcmap_port_config_t */
#define BFCMAP_ATTR_PORT_MODE_MASK                  _SHR_FCMAP_ATTR_PORT_MODE_MASK 
#define BFCMAP_ATTR_SPEED_MASK                      _SHR_FCMAP_ATTR_SPEED_MASK 
#define BFCMAP_ATTR_TX_BB_CREDITS_MASK              _SHR_FCMAP_ATTR_TX_BB_CREDITS_MASK 
#define BFCMAP_ATTR_RX_BB_CREDITS_MASK              _SHR_FCMAP_ATTR_RX_BB_CREDITS_MASK 
#define BFCMAP_ATTR_MAX_FRAME_LENGTH_MASK           _SHR_FCMAP_ATTR_MAX_FRAME_LENGTH_MASK 
#define BFCMAP_ATTR_BB_SC_N_MASK                    _SHR_FCMAP_ATTR_BB_SC_N_MASK 
#define BFCMAP_ATTR_PORT_STATE_MASK                 _SHR_FCMAP_ATTR_PORT_STATE_MASK 
#define BFCMAP_ATTR_R_T_TOV_MASK                    _SHR_FCMAP_ATTR_R_T_TOV_MASK 
#define BFCMAP_ATTR_INTERRUPT_ENABLE_MASK           _SHR_FCMAP_ATTR_INTERRUPT_ENABLE_MASK 
#define BFCMAP_ATTR_FW_ON_ACTIVE_8G_MASK            _SHR_FCMAP_ATTR_FW_ON_ACTIVE_8G_MASK 
#define BFCMAP_ATTR_SRC_MAC_CONSTRUCT_MASK          _SHR_FCMAP_ATTR_SRC_MAC_CONSTRUCT_MASK 
#define BFCMAP_ATTR_DST_MAC_CONSTRUCT_MASK          _SHR_FCMAP_ATTR_DST_MAC_CONSTRUCT_MASK 
#define BFCMAP_ATTR_VLAN_TAG_MASK                   _SHR_FCMAP_ATTR_VLAN_TAG_MASK 
#define BFCMAP_ATTR_VFT_TAG_MASK                    _SHR_FCMAP_ATTR_VFT_TAG_MASK 
#define BFCMAP_ATTR_MAPPER_LEN_MASK                 _SHR_FCMAP_ATTR_MAPPER_LEN_MASK 
#define BFCMAP_ATTR_INGRESS_MAPPER_BYPASS_MASK      _SHR_FCMAP_ATTR_INGRESS_MAPPER_BYPASS_MASK 
#define BFCMAP_ATTR_EGRESS_MAPPER_BYPASS_MASK       _SHR_FCMAP_ATTR_EGRESS_MAPPER_BYPASS_MASK 
#define BFCMAP_ATTR_INGRESS_MAP_TABLE_INPUT_MASK    _SHR_FCMAP_ATTR_INGRESS_MAP_TABLE_INPUT_MASK 
#define BFCMAP_ATTR_EGRESS_MAP_TABLE_INPUT_MASK     _SHR_FCMAP_ATTR_EGRESS_MAP_TABLE_INPUT_MASK 
#define BFCMAP_ATTR_INGRESS_FC_CRC_MODE_MASK        _SHR_FCMAP_ATTR_INGRESS_FC_CRC_MODE_MASK 
#define BFCMAP_ATTR_EGRESS_FC_CRC_MODE_MASK         _SHR_FCMAP_ATTR_EGRESS_FC_CRC_MODE_MASK 
#define BFCMAP_ATTR_INGRESS_VFTHDR_PROC_MODE_MASK   _SHR_FCMAP_ATTR_INGRESS_VFTHDR_PROC_MODE_MASK 
#define BFCMAP_ATTR_EGRESS_VFTHDR_PROC_MODE_MASK    _SHR_FCMAP_ATTR_EGRESS_VFTHDR_PROC_MODE_MASK 
#define BFCMAP_ATTR_INGRESS_VLANTAG_PROC_MODE_MASK  _SHR_FCMAP_ATTR_INGRESS_VLANTAG_PROC_MODE_MASK 
#define BFCMAP_ATTR_EGRESS_VLANTAG_PROC_MODE_MASK   _SHR_FCMAP_ATTR_EGRESS_VLANTAG_PROC_MODE_MASK 
#define BFCMAP_ATTR_INGRESS_VFID_MAPSRC_MASK        _SHR_FCMAP_ATTR_INGRESS_VFID_MAPSRC_MASK 
#define BFCMAP_ATTR_EGRESS_VFID_MAPSRC_MASK         _SHR_FCMAP_ATTR_EGRESS_VFID_MAPSRC_MASK 
#define BFCMAP_ATTR_INGRESS_VID_MAPSRC_MASK         _SHR_FCMAP_ATTR_INGRESS_VID_MAPSRC_MASK 
#define BFCMAP_ATTR_EGRESS_VID_MAPSRC_MASK          _SHR_FCMAP_ATTR_EGRESS_VID_MAPSRC_MASK 
#define BFCMAP_ATTR_INGRESS_VLAN_PRI_MAP_MODE_MASK  _SHR_FCMAP_ATTR_INGRESS_VLAN_PRI_MAP_MODE_MASK 
#define BFCMAP_ATTR_EGRESS_VLAN_PRI_MAP_MODE_MASK   _SHR_FCMAP_ATTR_EGRESS_VLAN_PRI_MAP_MODE_MASK 
#define BFCMAP_ATTR_INGRESS_HOPCNT_CHECK_MODE_MASK  _SHR_FCMAP_ATTR_INGRESS_HOPCNT_CHECK_MODE_MASK 
#define BFCMAP_ATTR2_EGRESS_HOPCNT_DEC_ENABLE_MASK  _SHR_FCMAP_ATTR2_EGRESS_HOPCNT_DEC_ENABLE_MASK
#define BFCMAP_ATTR2_USE_TTS_PCS_16G_MASK           _SHR_FCMAP_ATTR2_USE_TTS_PCS_16G_MASK
#define BFCMAP_ATTR2_USE_TTS_PCS_32G_MASK           _SHR_FCMAP_ATTR2_USE_TTS_PCS_32G_MASK
#define BFCMAP_ATTR2_TRAINING_ENABLE_16G_MASK       _SHR_FCMAP_ATTR2_TRAINING_ENABLE_16G_MASK
#define BFCMAP_ATTR2_TRAINING_ENABLE_32G_MASK       _SHR_FCMAP_ATTR2_TRAINING_ENABLE_32G_MASK
#define BFCMAP_ATTR2_FEC_ENABLE_16G_MASK            _SHR_FCMAP_ATTR2_FEC_ENABLE_16G_MASK
#define BFCMAP_ATTR2_FEC_ENABLE_32G_MASK            _SHR_FCMAP_ATTR2_FEC_ENABLE_32G_MASK
#define BFCMAP_ATTR2_INGRESS_FCS_CRRPT_EOF_ENABLE_MASK     _SHR_FCMAP_ATTR2_INGRESS_FCS_CRRPT_EOF_ENABLE_MASK
#define BFCMAP_ATTR2_INGRESS_VLANTAG_PRESENCE_ENABLE_MASK  _SHR_FCMAP_ATTR2_INGRESS_VLANTAG_PRESENCE_ENABLE_MASK
#define BFCMAP_ATTR2_EGRESS_HOPCNT_CHECK_MODE_MASK         _SHR_FCMAP_ATTR2_EGRESS_HOPCNT_CHECK_MODE_MASK
#define BFCMAP_ATTR2_EGRESS_PASS_CTRL_FRAME_ENABLE_MASK    _SHR_FCMAP_ATTR2_EGRESS_PASS_CTRL_FRAME_ENABLE_MASK
#define BFCMAP_ATTR2_EGRESS_PASS_PFC_FRAME_ENABLE_MASK     _SHR_FCMAP_ATTR2_EGRESS_PASS_PFC_FRAME_ENABLE_MASK
#define BFCMAP_ATTR2_EGRESS_PASS_PAUSE_FRAME_ENABLE_MASK   _SHR_FCMAP_ATTR2_EGRESS_PASS_PAUSE_FRAME_ENABLE_MASK
#define BFCMAP_ATTR2_EGRESS_FCOE_VER_CHK_DISABLE_MASK      _SHR_FCMAP_ATTR2_EGRESS_FCOE_VER_CHK_DISABLE_MASK
#define BFCMAP_ATTR2_EGRESS_DEFAULT_COS_VALUE_MASK         _SHR_FCMAP_ATTR2_EGRESS_DEFAULT_COS_VALUE_MASK
#define BFCMAP_ATTR2_EGRESS_USE_IP_COS_MAP_MASK            _SHR_FCMAP_ATTR2_EGRESS_USE_IP_COS_MAP_MASK
#define BFCMAP_ATTR2_INGRESS_HOPCNT_DEC_ENABLE_MASK        _SHR_FCMAP_ATTR2_INGRESS_HOPCNT_DEC_ENABLE_MASK
#define BFCMAP_ATTR2_SCRAMBLING_ENABLE_MASK                _SHR_FCMAP_ATTR2_SCRAMBLING_ENABLE_MASK
#define BFCMAP_ATTR2_EGRESS_PAUSE_ENABLE_MASK              _SHR_FCMAP_ATTR2_EGRESS_PAUSE_ENABLE_MASK
#define BFCMAP_ATTR2_EGRESS_PFC_ENABLE_MASK                _SHR_FCMAP_ATTR2_EGRESS_PFC_ENABLE_MASK
#define BFCMAP_ATTR2_STAT_INTERVAL_MASK                    _SHR_FCMAP_ATTR2_STAT_INTERVAL_MASK
#define BFCMAP_ATTR2_TRCM_ATTRIBS_MASK                     _SHR_FCMAP_ATTR2_TRCM_ATTRIBS_MASK
#define BFCMAP_ATTR2_COS_TO_PRI_MASK                       _SHR_FCMAP_ATTR2_COS_TO_PRI_MASK

#define BFCMAP_ATTR_ALL_MASK                        _SHR_FCMAP_ATTR_ALL_MASK
#define BFCMAP_ATTR2_ALL_MASK                       _SHR_FCMAP_ATTR2_ALL_MASK


/*
 * BFCMAP port configuration structure.
 */
typedef _shr_bfcmap_port_config_t  bfcmap_port_config_t;

/*
 * Set BFCMAP Port configuration.
 */
extern int
bfcmap_port_config_set(bfcmap_port_t p, bfcmap_port_config_t *cfg);

/*
 * Get current BFCMAP Port configuration.
 */
extern int
bfcmap_port_config_get(bfcmap_port_t p, bfcmap_port_config_t *cfg);

/*
 * Set BFCMAP Port configuration based on bitmask
 */
extern int
bfcmap_port_config_selective_set(bfcmap_port_t p, bfcmap_port_config_t *cfg);

/*
 * Get BFCMAP Port configuration based on bitmask
 */
extern int
bfcmap_port_config_selective_get(bfcmap_port_t p, bfcmap_port_config_t *cfg);


/*
 * Prototype for Port interation callback function.
 */
typedef int (*bfcmap_port_traverse_cb)(bfcmap_port_t p, 
                                        bfcmap_core_t dev_core, 
                                        bfcmap_dev_addr_t dev_addr, 
                                        int dev_port, 
                                        bfcmap_dev_io_f devio_f, 
                                        void *user_data);

/*
 * Iterates over all the BFCMAP ports and calls user provided callback
 * function for each port.
 */
extern int bfcmap_port_traverse(bfcmap_port_traverse_cb callbk, 
                                void *user_data);

/*
 * BFCMAP VLAN - VSAN Mapping
 */
typedef _shr_bfcmap_vlan_vsan_map_t  bfcmap_vlan_vsan_map_t;

/*
 * Add entry into the VLAN to VSAN table for BFCMAP port
 */
extern int
bfcmap_vlan_map_add(bfcmap_port_t p, bfcmap_vlan_vsan_map_t *vlan);

/*
 * Get entry from the VLAN to VSAN table for BFCMAP port
 */
extern int
bfcmap_vlan_map_get(bfcmap_port_t p, bfcmap_vlan_vsan_map_t *vlan);

/*
 * Delete entry from the VLAN to VSAN table for BFCMAP port
 */
extern int
bfcmap_vlan_map_delete(bfcmap_port_t p, bfcmap_vlan_vsan_map_t *vlan);


/*
 * BFCMAP Events that are triggered by the BFCMAP driver.
 */
typedef enum {
    BFCMAP_EVENT_FC_LINK_INIT = _SHR_BFCMAP_EVENT_FC_LINK_INIT,
    BFCMAP_EVENT_FC_LINK_RESET = _SHR_BFCMAP_EVENT_FC_LINK_RESET,
    BFCMAP_EVENT_FC_LINK_DOWN = _SHR_BFCMAP_EVENT_FC_LINK_DOWN,
    BFCMAP_EVENT_FC_R_T_TIMEOUT = _SHR_BFCMAP_EVENT_FC_R_T_TIMEOUT,
    BFCMAP_EVENT_FC_E_D_TIMEOUT = _SHR_BFCMAP_EVENT_FC_E_D_TIMEOUT,

    BFCMAP_EVENT__COUNT = _SHR_BFCMAP_EVENT__COUNT
} bfcmap_event_t;

/*
 * BFCMAP event handler entry function. This function is/should be called
 * by the user either periodically or when when an interrupt is triggered.
 * This function will collect the pending events reported by the HW and call
 * the user provided callbacks for vaious events.
 */
int bfcmap_event_handler(bfcmap_dev_addr_t dev_addr);

/*
 * Prototype for the event callback function.
 */
typedef int (*bfcmap_event_cb_fn)(bfcmap_port_t p,      /* Port ID          */
                                  bfcmap_event_t event, /* Event            */
                                  void *user_data);         /* secure assoc ID  */

/*
 * Register user event handler callback function.
 */
extern int 
bfcmap_event_register(bfcmap_event_cb_fn cb, void *user_data);
                               
/*
 * Unregister user event handler callabck function.
 */
extern int 
bfcmap_event_unregister(bfcmap_event_cb_fn cb);

/*
 * Enables/Diable handling of the BFCMAP event.
 */
extern int bfcmap_event_enable_set(bfcmap_event_t event, int enable);

/*
 * Return the current event hanler status for specified event.
 */
extern int bfcmap_event_enable_get(bfcmap_event_t event, int *enable);

/*
 * The following enums define the stats/couter types.
 */
typedef enum {
        fc_rxdebug0=_shr_fc_rxdebug0,
        fc_rxdebug1=_shr_fc_rxdebug1,
        fc_rxunicastpkts=_shr_fc_rxunicastpkts,
        fc_rxgoodframes=_shr_fc_rxgoodframes,
        fc_rxbcastpkts=_shr_fc_rxbcastpkts,
        fc_rxbbcredit0=_shr_fc_rxbbcredit0,
        fc_rxinvalidcrc=_shr_fc_rxinvalidcrc,
        fc_rxframetoolong=_shr_fc_rxframetoolong,
        fc_rxtruncframes=_shr_fc_rxtruncframes,
        fc_rxdelimitererr=_shr_fc_rxdelimitererr,
        fc_rxothererr=_shr_fc_rxothererr,
        fc_rxruntframes=_shr_fc_rxruntframes,
        fc_rxlipcount=_shr_fc_rxlipcount,
        fc_rxnoscount=_shr_fc_rxnoscount,
        fc_rxerrframes=_shr_fc_rxerrframes,
        fc_rxdropframes=_shr_fc_rxdropframes,
        fc_rxlinkfail=_shr_fc_rxlinkfail,
        fc_rxlosssync=_shr_fc_rxlosssync,
        fc_rxlosssig=_shr_fc_rxlosssig,
        fc_rxprimseqerr=_shr_fc_rxprimseqerr,
        fc_rxinvalidword=_shr_fc_rxinvalidword,
        fc_rxinvalidset=_shr_fc_rxinvalidset,
        fc_rxencodedisparity=_shr_fc_rxencodedisparity,
        fc_rxbyt=_shr_fc_rxbyt,
        fc_txdebug0=_shr_fc_txdebug0,
        fc_txdebug1=_shr_fc_txdebug1,
        fc_txunicastpkts=_shr_fc_txunicastpkts,
        fc_txbcastpkts=_shr_fc_txbcastpkts,
        fc_txbbcredit0=_shr_fc_txbbcredit0,
        fc_txgoodframes=_shr_fc_txgoodframes,
        fc_txfifounderrun=_shr_fc_txfifounderrun,
        fc_txdropframes=_shr_fc_txdropframes,
        fc_txbyt=_shr_fc_txbyt,

        fc_class2_rxgoodframes=_shr_fc_class2_rxgoodframes, 
        fc_class2_rxinvalidcrc=_shr_fc_class2_rxinvalidcrc, 
        fc_class2_rxframetoolong=_shr_fc_class2_rxframetoolong, 
        fc_class3_rxgoodframes=_shr_fc_class3_rxgoodframes, 
        fc_class3_rxinvalidcrc=_shr_fc_class3_rxinvalidcrc, 
        fc_class3_rxframetoolong=_shr_fc_class3_rxframetoolong, 
        fc_classf_rxgoodframes=_shr_fc_classf_rxgoodframes, 
        fc_classf_rxinvalidcrc=_shr_fc_classf_rxinvalidcrc, 
        fc_classf_rxframetoolong=_shr_fc_classf_rxframetoolong, 
        fc_rxbbc0drop=_shr_fc_rxbbc0drop, 
        fc_rxsyncfail=_shr_fc_rxsyncfail, 
        fc_rxbadxword=_shr_fc_rxbadxword, 
        fc_class2_rxruntframes=_shr_fc_class2_rxruntframes, 
        fc_class3_rxruntframes=_shr_fc_class3_rxruntframes, 
        fc_classf_rxruntframes=_shr_fc_classf_rxruntframes, 
        fc_class2_rxbyt=_shr_fc_class2_rxbyt, 
        fc_class3_rxbyt=_shr_fc_class3_rxbyt, 
        fc_classf_rxbyt=_shr_fc_classf_rxbyt, 
        fc_class2_txframes=_shr_fc_class2_txframes, 
        fc_class3_txframes=_shr_fc_class3_txframes, 
        fc_classf_txframes=_shr_fc_classf_txframes, 
        fc_txframes=_shr_fc_txframes, 
        fc_class2_txoversized_frames=_shr_fc_class2_txoversized_frames, 
        fc_class3_txoversized_frames=_shr_fc_class3_txoversized_frames, 
        fc_classf_txoversized_frames=_shr_fc_classf_txoversized_frames, 
        fc_txoversized_frames=_shr_fc_txoversized_frames, 
        fc_class2_txbyt=_shr_fc_class2_txbyt, 
        fc_class3_txbyt=_shr_fc_class3_txbyt, 
        fc_classf_txbyt=_shr_fc_classf_txbyt, 

	bfcmap_stat__count = _shr_bfcmap_stat__count
} bfcmap_stat_t;

/*
 * Crear all the stats for the specified port.
 */
extern int bfcmap_stat_clear(bfcmap_port_t p);

/*
 * Return the current statistics for the specified counter.
 */
extern int bfcmap_stat_get(bfcmap_port_t p, bfcmap_stat_t stat, buint64_t *val);

/*
 * Return the current statistics for the specified counter.
 */
extern int bfcmap_stat_get32(bfcmap_port_t p, bfcmap_stat_t stat,
                             buint32_t *val);


/* FC link failure trigger type */
typedef enum bfcmap_lf_tr_e {
    BFCMAP_LF_TR_NONE = _SHR_BFCMAP_LF_TR_NONE,
    BFCMAP_LF_TR_PORT_INIT = _SHR_BFCMAP_LF_TR_PORT_INIT,
    BFCMAP_LF_TR_OPEN_LINK = _SHR_BFCMAP_LF_TR_OPEN_LINK,
    BFCMAP_LF_TR_LINK_FAILURE = _SHR_BFCMAP_LF_TR_LINK_FAILURE,
    BFCMAP_LF_TR_OLS_RCVD = _SHR_BFCMAP_LF_TR_OLS_RCVD,
    BFCMAP_LF_TR_NOS_RCVD = _SHR_BFCMAP_LF_TR_NOS_RCVD,
    BFCMAP_LF_TR_SYNC_LOSS = _SHR_BFCMAP_LF_TR_SYNC_LOSS,
    BFCMAP_LF_TR_BOUCELINK_FROM_ADMIN = _SHR_BFCMAP_LF_TR_BOUCELINK_FROM_ADMIN,
    BFCMAP_LF_TR_CHGSPEED_FROM_ADMIN = _SHR_BFCMAP_LF_TR_CHGSPEED_FROM_ADMIN,
    BFCMAP_LF_TR_DISABLE_FROM_ADMIN = _SHR_BFCMAP_LF_TR_DISABLE_FROM_ADMIN,
    BFCMAP_LF_TR_RESET_FROM_ADMIN = _SHR_BFCMAP_LF_TR_RESET_FROM_ADMIN,
    BFCMAP_LF_TR_LR_RCVD = _SHR_BFCMAP_LF_TR_LR_RCVD,
    BFCMAP_LF_TR_LRR_RCVD = _SHR_BFCMAP_LF_TR_LRR_RCVD,
    BFCMAP_LF_TR_ED_TOV = _SHR_BFCMAP_LF_TR_ED_TOV
}  bfcmap_lf_tr_t;


/* FC link failure reason code */
typedef enum bfcmap_lf_rc_e {
    BFCMAP_LF_RC_NONE = _SHR_BFCMAP_LF_RC_NONE,
    BFCMAP_LF_RC_PORT_INIT = _SHR_BFCMAP_LF_RC_PORT_INIT,
    BFCMAP_LF_RC_OPEN_LINK = _SHR_BFCMAP_LF_RC_OPEN_LINK,
    BFCMAP_LF_RC_LINK_FAILURE = _SHR_BFCMAP_LF_RC_LINK_FAILURE,
    BFCMAP_LF_RC_OLS_RCVD = _SHR_BFCMAP_LF_RC_OLS_RCVD,
    BFCMAP_LF_RC_NOS_RCVD = _SHR_BFCMAP_LF_RC_NOS_RCVD,
    BFCMAP_LF_RC_SYNC_LOSS = _SHR_BFCMAP_LF_RC_SYNC_LOSS,
    BFCMAP_LF_RC_BOUCELINK_FROM_ADMIN = _SHR_BFCMAP_LF_RC_BOUCELINK_FROM_ADMIN,
    BFCMAP_LF_RC_CHGSPEED_FROM_ADMIN = _SHR_BFCMAP_LF_RC_CHGSPEED_FROM_ADMIN,
    BFCMAP_LF_RC_DISABLE_FROM_ADMIN = _SHR_BFCMAP_LF_RC_DISABLE_FROM_ADMIN,
    BFCMAP_LF_RC_RESET_FAILURE = _SHR_BFCMAP_LF_RC_RESET_FAILURE
}  bfcmap_lf_rc_t;


extern int bfcmap_port_linkfault_trigger_rc_get(bfcmap_port_t p, bfcmap_lf_tr_t *lf_trigger, bfcmap_lf_rc_t *lf_rc);


typedef enum bfcmap_diag_code_e {
    BFCMAP_DIAG_OK = _SHR_BFCMAP_DIAG_OK,
    BFCMAP_DIAG_PORT_INIT = _SHR_BFCMAP_DIAG_PORT_INIT,
    BFCMAP_DIAG_OPEN_LINK = _SHR_BFCMAP_DIAG_OPEN_LINK,
    BFCMAP_DIAG_LINK_FAILURE = _SHR_BFCMAP_DIAG_LINK_FAILURE,
    BFCMAP_DIAG_OLS_RCVD = _SHR_BFCMAP_DIAG_OLS_RCVD,
    BFCMAP_DIAG_NOS_RCVD = _SHR_BFCMAP_DIAG_NOS_RCVD,
    BFCMAP_DIAG_SYNC_LOSS = _SHR_BFCMAP_DIAG_SYNC_LOSS,
    BFCMAP_DIAG_BOUCELINK_FROM_ADMIN = _SHR_BFCMAP_DIAG_BOUCELINK_FROM_ADMIN,
    BFCMAP_DIAG_CHGSPEED_FROM_ADMIN = _SHR_BFCMAP_DIAG_CHGSPEED_FROM_ADMIN,
    BFCMAP_DIAG_DISABLE_FROM_ADMIN = _SHR_BFCMAP_DIAG_DISABLE_FROM_ADMIN,
    BFCMAP_DIAG_AN_NO_SIGNAL = _SHR_BFCMAP_DIAG_AN_NO_SIGNAL,
    BFCMAP_DIAG_AN_TIMEOUT = _SHR_BFCMAP_DIAG_AN_TIMEOUT,
    BFCMAP_DIAG_PROTO_TIMEOUT = _SHR_BFCMAP_DIAG_PROTO_TIMEOUT
}  bfcmap_diag_code_t;


extern int bfcmap_port_diag_get(bfcmap_port_t p, bfcmap_diag_code_t *diag);

extern int bfcmap_port_private_data_set(bfcmap_port_t p, buint8_t *data, int len);

/*
 * BFCMAP Port create.
 * This API creates a BFCMAP port and binds the bfcmap_port_t
 * to particular bfcmap device/port.
 */
extern int 
bfcmap_port_create(bfcmap_port_t p,             /* port identifier    */
                    bfcmap_core_t dev_core,      /* device core type   */
                    bfcmap_dev_addr_t dev_addr,  /* device address     */
                    bfcmap_dev_addr_t uc_dev_addr,  /* uc device address */
                    int dev_port,                 /* device port index  */
                    bfcmap_dev_io_f devio_f,      /* device I/O callback*/
                    void *fcmap_port_cfg);        /* port config */

/*
 * BFCMAP Port destroy.
 */
extern int bfcmap_port_destroy(bfcmap_port_t p);

/*
 * BFCMAP Port reset.
 * The FC link reset protocol perform LINK reset when protocol errors
 * are detected. The FC-2 protocol errors,
 * e.g. Receive frame when no B2B credits exist,
 *  timeout conditions need FC Link reset. 
 */
extern int bfcmap_port_reset(bfcmap_port_t p);

/*
 * BFCMAP Port shutdown.
 * Initiates FC link Online to Offline protocol on the FC port
 */
extern int bfcmap_port_shutdown(bfcmap_port_t p);

/*
 * BFCMAP Port Enable.
 * Enables FC link to Online from Offline
 */
extern int bfcmap_port_link_enable(bfcmap_port_t p);

/*
 * BFCMAP Port Bounce link.
 * Bounce FC link 
 */
extern int bfcmap_port_bounce(bfcmap_port_t p);

/*
 * BFCMAP Port Speed.
 * Sets FC link speed
 */
extern int bfcmap_port_speed_set(bfcmap_port_t p, bfcmap_port_speed_t speed);

typedef _shr_bfcmap_port_ability_t bfcmap_port_ability_t;

/* port_ability_speed field values in bfcmap_port_ability_t */
#define BFCMAP_PORT_ABILITY_SPEED_AN     _SHR_FCMAP_PORT_ABILITY_SPEED_AN       
#define BFCMAP_PORT_ABILITY_SPEED_2GBPS  _SHR_FCMAP_PORT_ABILITY_SPEED_2GBPS
#define BFCMAP_PORT_ABILITY_SPEED_4GBPS  _SHR_FCMAP_PORT_ABILITY_SPEED_4GBPS
#define BFCMAP_PORT_ABILITY_SPEED_8GBPS  _SHR_FCMAP_PORT_ABILITY_SPEED_8GBPS
#define BFCMAP_PORT_ABILITY_SPEED_16GBPS _SHR_FCMAP_PORT_ABILITY_SPEED_16GBPS
#define BFCMAP_PORT_ABILITY_SPEED_32GBPS _SHR_FCMAP_PORT_ABILITY_SPEED_32GBPS

/*
 * BFCMAP Port Speed Ability Set
 * Sets discrete port speeds for AN or single forced port speed
 */

extern int bfcmap_port_ability_advert_set(bfcmap_port_t port, bfcmap_port_ability_t *ability_mask);

/*
 * BFCMAP Port Speed Ability Get
 * Retrieves supported port speeds 
 */

extern int bfcmap_port_ability_advert_get(bfcmap_port_t port, bfcmap_port_ability_t *ability_mask);

#define BFCMAP_PORT_SCRAMBLING_SPEED_2GBPS  _SHR_FCMAP_PORT_SCRAMBLING_SPEED_2GBPS 
#define BFCMAP_PORT_SCRAMBLING_SPEED_4GBPS  _SHR_FCMAP_PORT_SCRAMBLING_SPEED_4GBPS 
#define BFCMAP_PORT_SCRAMBLING_SPEED_8GBPS  _SHR_FCMAP_PORT_SCRAMBLING_SPEED_8GBPS 
#define BFCMAP_PORT_SCRAMBLING_SPEED_16GBPS _SHR_FCMAP_PORT_SCRAMBLING_SPEED_16GBPS 
#define BFCMAP_PORT_SCRAMBLING_SPEED_32GBPS _SHR_FCMAP_PORT_SCRAMBLING_SPEED_32GBPS 

#define BFCMAP_PORT_MODULE_NO_FW_I2C     _SHR_FCMAP_PORT_MODULE_NO_FW_I2C 
#define BFCMAP_PORT_MODULE_IS_COPPER     _SHR_FCMAP_PORT_MODULE_IS_COPPER 
#define BFCMAP_PORT_MODULE_SPEED_CAP32G  _SHR_FCMAP_PORT_MODULE_SPEED_CAP32G 
#define BFCMAP_PORT_MODULE_SPEED_CAP16G  _SHR_FCMAP_PORT_MODULE_SPEED_CAP16G 
#define BFCMAP_PORT_MODULE_SPEED_CAP8G   _SHR_FCMAP_PORT_MODULE_SPEED_CAP8G 
#define BFCMAP_PORT_MODULE_SPEED_CAP4G   _SHR_FCMAP_PORT_MODULE_SPEED_CAP4G 
#define BFCMAP_PORT_MODULE_FEC_CAP32G    _SHR_FCMAP_PORT_MODULE_FEC_CAP32G 
#define BFCMAP_PORT_MODULE_FEC_CAP16G    _SHR_FCMAP_PORT_MODULE_FEC_CAP16G 
#define BFCMAP_PORT_MODULE_IS_QSFP       _SHR_FCMAP_PORT_MODULE_IS_QSFP 
#define BFCMAP_PORT_MODULE_SUPP_RS       _SHR_FCMAP_PORT_MODULE_SUPP_RS 
#define BFCMAP_PORT_MODULE_RS_HRD_RX_CUTOFF _SHR_FCMAP_PORT_MODULE_RS_HRD_RX_CUTOFF 
#define BFCMAP_PORT_MODULE_RS_HRD_TX_CUTOFF _SHR_FCMAP_PORT_MODULE_RS_HRD_TX_CUTOFF 

/* Internal routines to malloc and free 8-byte aligned memory */
extern void* bfcmap_int_dword_malloc(unsigned int size, char *s);
extern void bfcmap_int_dword_free(void *addr);

#endif /* BFCMAP_H */

