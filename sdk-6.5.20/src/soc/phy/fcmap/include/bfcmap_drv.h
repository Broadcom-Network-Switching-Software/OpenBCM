/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef BFCMAP_DRV_H
#define BFCMAP_DRV_H

struct bfcmap_chip_info_s;
struct bfcmap_port_ctrl_s;
struct bfcmap_dev_ctrl_s;
struct bfcmap_api_table_s;
struct bfcmap_counter_info_s;
struct bfcmap_counter_cb_s;

typedef int (*msec_unit_init_f)(struct bfcmap_dev_ctrl_s*);
typedef int (*msec_unit_uninit_f)(struct bfcmap_dev_ctrl_s*);
typedef int (*msec_config_set)(struct bfcmap_port_ctrl_s*,bfcmap_port_config_t*);
typedef int (*msec_config_get)(struct bfcmap_port_ctrl_s*,bfcmap_port_config_t*);
/*typedef int (*msec_capability_get)(struct bfcmap_port_ctrl_s*,bfcmap_port_capability_t*);*/
typedef int (*msec_port_init)(struct bfcmap_port_ctrl_s*,bfcmap_port_config_t*);
typedef int (*msec_port_uninit)(struct bfcmap_port_ctrl_s*);
typedef int (*msec_port_reset)(struct bfcmap_port_ctrl_s*);
typedef int (*msec_port_shutdown)(struct bfcmap_port_ctrl_s*);
typedef int (*msec_port_link_enable)(struct bfcmap_port_ctrl_s*);
typedef int (*msec_port_speed_set)(struct bfcmap_port_ctrl_s*, bfcmap_port_speed_t );
typedef int (*msec_port_bounce)(struct bfcmap_port_ctrl_s*);
#if defined (BCM_WARM_BOOT_SUPPORT)  /*  */
typedef int (*msec_vlan_recover)(struct bfcmap_port_ctrl_s*);
#endif /* BCM_WARM_BOOT_SUPPORT */



typedef int (*msec_event_handler)(struct bfcmap_dev_ctrl_s*);

/* diagnostic functions */
typedef int (*msec_cipher_op_encrypt)(struct bfcmap_port_ctrl_s*,
#if 0
                                      bfcmap_crypto_aes128_gcm_t *key,
#endif
                                      buint8_t *protected_data, int pdlen,
                                      buint8_t *confidential_data, int cdlen,
                                      buint8_t *out_data, int outlen);

typedef int (*msec_cipher_op_decrypt)(struct bfcmap_port_ctrl_s*,
#if 0
                                      bfcmap_crypto_aes128_gcm_t *key,
#endif
                                      buint8_t *protected_data, int pdlen,
                                      buint8_t *confidential_data, int cdlen,
                                      buint8_t *icv_data, int icvlen,
                                      buint8_t *out_data, int outlen);

typedef int (*msec_diag_get)(struct bfcmap_port_ctrl_s*,bfcmap_diag_code_t*);
typedef int (*msec_linkfault_trigger_rc_get)(struct bfcmap_port_ctrl_s*,bfcmap_lf_tr_t*, bfcmap_lf_rc_t*);

typedef int (*msec_port_ability_advert_set)(struct bfcmap_port_ctrl_s*, bfcmap_port_ability_t*);
typedef int (*msec_port_ability_advert_get)(struct bfcmap_port_ctrl_s*, bfcmap_port_ability_t*);
typedef int (*msec_private_data_set)(struct bfcmap_port_ctrl_s*,buint8_t *, int);
typedef int (*msec_vlan_map_add)(struct bfcmap_port_ctrl_s*, bfcmap_vlan_vsan_map_t*);
typedef int (*msec_vlan_map_get)(struct bfcmap_port_ctrl_s*, bfcmap_vlan_vsan_map_t*);
typedef int (*msec_vlan_map_delete)(struct bfcmap_port_ctrl_s*, bfcmap_vlan_vsan_map_t*);
typedef int (*msec_stat_get)(struct bfcmap_port_ctrl_s*, bfcmap_stat_t, buint64_t*);
typedef int (*msec_stat_clear)(struct bfcmap_port_ctrl_s*);
typedef struct bfcmap_api_table_s {
    /* Unit APIs */
    msec_unit_init_f        unit_init;
    msec_unit_uninit_f      unit_uninit;

    /* Port based APIs */
    msec_config_set         config_set;
    msec_config_get         config_get;
    /*msec_capability_get     capability_get;*/
    msec_port_init          port_init;
    msec_port_uninit        port_uninit;
    msec_port_reset         port_reset;
    msec_port_shutdown      port_shutdown;
    msec_port_link_enable   port_link_enable;
    msec_port_speed_set     port_speed_set;
    msec_port_bounce        port_bounce;

    msec_event_handler      event_handler;
    msec_diag_get           diag_get;
    msec_linkfault_trigger_rc_get     linkfault_trigger_rc_get;
#if defined (BCM_WARM_BOOT_SUPPORT)  /*  */
    msec_vlan_recover       vlan_recover;
#endif /* BCM_WARM_BOOT_SUPPORT */
    /* diagnostic functions */
    msec_cipher_op_encrypt  cipher_op_encrypt;
    msec_cipher_op_decrypt  cipher_op_decrypt;
    msec_port_ability_advert_set port_ability_advert_set;
    msec_port_ability_advert_get port_ability_advert_get;
    msec_private_data_set   private_data_set;
    msec_vlan_map_add       vlan_map_add;
    msec_vlan_map_get       vlan_map_get;
    msec_vlan_map_delete    vlan_map_delete;
    msec_stat_get           stat_get;
    msec_stat_clear         stat_clear;
} bfcmap_drv_t;

#define BFCMAP_DRV_FN(_mpc,_mf)   (_mpc)->api->_mf

#define _BFCMAP_DRV_CALL(_mpc, _mf, _ma)                     \
        (_mpc == NULL ? BFCMAP_E_PARAM :                     \
         (BFCMAP_DRV_FN(_mpc,_mf) == NULL ?                  \
         BFCMAP_E_UNAVAIL : BFCMAP_DRV_FN(_mpc,_mf) _ma))

#define _BFCMAP_DRV_CALL_1ARG(_mpc, _mf, _ma, _arg)                     \
        (_mpc == NULL ? BFCMAP_E_PARAM :                     \
         (BFCMAP_DRV_FN(_mpc,_mf) == NULL ?                  \
         BFCMAP_E_UNAVAIL : BFCMAP_DRV_FN(_mpc,_mf)(_ma, _arg)))

#define _BFCMAP_DRV_CALL_2ARG(_mpc, _mf, _ma, _arg1, _arg2)                     \
        (_mpc == NULL ? BFCMAP_E_PARAM :                     \
         (BFCMAP_DRV_FN(_mpc,_mf) == NULL ?                  \
         BFCMAP_E_UNAVAIL : BFCMAP_DRV_FN(_mpc,_mf)(_ma, _arg1, _arg2)))

#define BFCMAP_PORT_CONFIG_SET(_mpc, _ma) \
        _BFCMAP_DRV_CALL_1ARG((_mpc), config_set, (_mpc), (_ma))

#define BFCMAP_PORT_CONFIG_GET(_mpc, _ma) \
        _BFCMAP_DRV_CALL_1ARG((_mpc), config_get, (_mpc), (_ma))

#define BFCMAP_PORT_CAPABILITY_GET(_mpc, _ma) \
        _BFCMAP_DRV_CALL((_mpc), capability_get, ((_mpc), (_ma)))

#define BFCMAP_PORT_INIT(_mpc, _ma) \
        _BFCMAP_DRV_CALL((_mpc), port_init, ((_mpc), (_ma)))

#define BFCMAP_PORT_UNINIT(_mpc) \
        _BFCMAP_DRV_CALL((_mpc), port_uninit, ((_mpc)))

#define BFCMAP_PORT_RESET(_mpc) \
        _BFCMAP_DRV_CALL((_mpc), port_reset, ((_mpc)))

#define BFCMAP_PORT_SHUTDOWN(_mpc) \
        _BFCMAP_DRV_CALL((_mpc), port_shutdown, ((_mpc)))

#define BFCMAP_PORT_LINK_ENABLE(_mpc) \
        _BFCMAP_DRV_CALL((_mpc), port_link_enable, ((_mpc)))

#define BFCMAP_PORT_BOUNCE(_mpc) \
        _BFCMAP_DRV_CALL((_mpc), port_bounce, ((_mpc)))

#define BFCMAP_PORT_SPEED_SET(_mpc, _speed) \
        _BFCMAP_DRV_CALL_1ARG((_mpc), port_speed_set, ((_mpc)), (_speed))

#define BFCMAP_DEVICE_INIT(_mdc) \
        _BFCMAP_DRV_CALL((_mdc)->ports, unit_init, ((_mdc)))

#define BFCMAP_DEVICE_UNINIT(_mdc) \
        _BFCMAP_DRV_CALL((_mdc)->ports, unit_uninit, ((_mdc)))

#define BFCMAP_PORT_GET_CFG(_mpc, _ma) \
        _BFCMAP_DRV_CALL((_mpc), get_cfg, ((_mpc), (_ma)))

#define BFCMAP_DEVICE_EVENT_HANDLER(_mdc)      \
        _BFCMAP_DRV_CALL((_mdc)->ports, event_handler, ((_mdc)))

#define BFCMAP_PORT_LINKFAULT_TRIGGER_RC_GET(_mpc, _ma1, _ma2) \
       _BFCMAP_DRV_CALL_2ARG((_mpc), linkfault_trigger_rc_get, (_mpc), (_ma1), (_ma2))

#if defined (BCM_WARM_BOOT_SUPPORT)  /*  */
#define BFCMAP_VLAN_RECOVER(_mpc) \
        _BFCMAP_DRV_CALL((_mpc), vlan_recover, ((_mpc)))
#endif /* BCM_WARM_BOOT_SUPPORT */


#define BFCMAP_PORT_DIAG_GET(_mpc, _ma) \
        _BFCMAP_DRV_CALL_1ARG((_mpc), diag_get, (_mpc), (_ma))

#define BFCMAP_PORT_DIAG_CIPHER_OP_ENCRYPT(_mpc,k,ppd,pdl,pcd,cdl,pout,pol) \
        _BFCMAP_DRV_CALL((_mpc), cipher_op_encrypt, \
                        ((_mpc), (k), (ppd), (pdl), (pcd), (cdl), (pout), (pol)))

#define BFCMAP_PORT_DIAG_CIPHER_OP_DECRYPT(_mpc,k,ppd,pdl,pcd,cdl,picv,icvl,pout,pol) \
        _BFCMAP_DRV_CALL((_mpc), cipher_op_decrypt, \
                        ((_mpc), (k), (ppd), (pdl), (pcd), (cdl), (picv), (icvl), \
                        (pout), (pol)))

#define BFCMAP_PORT_ABILITY_ADVERT_SET(_mpc, _ma) \
        _BFCMAP_DRV_CALL_1ARG((_mpc), port_ability_advert_set, (_mpc), (_ma))

#define BFCMAP_PORT_ABILITY_ADVERT_GET(_mpc, _ma) \
        _BFCMAP_DRV_CALL_1ARG((_mpc), port_ability_advert_get, (_mpc), (_ma))

#define BFCMAP_PORT_PRIVATE_DATA_SET(_mpc, _ma1, _ma2) \
       _BFCMAP_DRV_CALL_2ARG((_mpc), private_data_set, (_mpc), (_ma1), (_ma2))

#define BFCMAP_VLAN_MAP_ADD(_mpc, _ma) \
        _BFCMAP_DRV_CALL_1ARG((_mpc), vlan_map_add, (_mpc), (_ma))

#define BFCMAP_VLAN_MAP_GET(_mpc, _ma) \
        _BFCMAP_DRV_CALL_1ARG((_mpc), vlan_map_get, (_mpc), (_ma))

#define BFCMAP_VLAN_MAP_DELETE(_mpc, _ma) \
        _BFCMAP_DRV_CALL_1ARG((_mpc), vlan_map_delete, (_mpc), (_ma))

#define BFCMAP_STAT_GET(_mpc, _ma1, _ma2) \
       _BFCMAP_DRV_CALL_2ARG((_mpc), stat_get, (_mpc), (_ma1), (_ma2))

#define BFCMAP_STAT_CLEAR(_mpc) \
        _BFCMAP_DRV_CALL((_mpc), stat_clear, ((_mpc)))
#endif /* BFCMAP_DRV_H */

