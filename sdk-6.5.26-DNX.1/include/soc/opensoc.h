/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _BCM_OPENSOC_H
#define _BCM_OPENSOC_H

/* From: $SDK/include/soc/defs.h */
#ifndef SOC_MAX_NUM_SWITCH_DEVICES
#define SOC_MAX_NUM_SWITCH_DEVICES  16
#endif
/* max number of ethernet devices */
#ifndef SOC_MAX_NUM_ETHER_DEVICES
#define SOC_MAX_NUM_ETHER_DEVICES   2
#endif

/* max number of all kinds of directly attached devices */
#define SOC_MAX_NUM_DEVICES         (SOC_MAX_NUM_SWITCH_DEVICES +\
                     SOC_MAX_NUM_ETHER_DEVICES)
/* End: $SDK/include/soc/defs.h */

/* From: $SDK/include/soc/drv.h */
#ifndef _SOC_DRV_H
typedef struct soc_chip_info_vectors_s {
    int (*icid_get)(int unit, uint8 *data, int len);
} soc_chip_info_vectors_t;
extern int      soc_all_ndev;

extern int
soc_chip_info_vect_config(soc_chip_info_vectors_t *vect);

extern int soc_reset(int unit);
extern int soc_init(int unit);
extern int soc_reset_init(int unit);
extern int soc_misc_init(int unit);
extern int soc_mmu_init(int unit);
extern uint32 soc_property_get(int unit, const char *name, uint32 defl);
extern int soc_detach(int unit);
extern int soc_shutdown(int unit);

#if defined(BCM_WARM_BOOT_SUPPORT)
/*
 * BCM Warm Boot Support
 *
 * Purpose:  Indicates whether the device is currently in reload
 *           state (performing Warm boot) or not.
 *
 *           If finer granularity is needed in the future, additional
 *           flags can be defined to control specific hardware accesses.
 */
extern uint32    soc_state[SOC_MAX_NUM_DEVICES];
#define SOC_ST_F_RELOADING         0x1    /* Warm boot in progress, device is reloading */

#define SOC_WARM_BOOT(unit)       (soc_state[unit] & SOC_ST_F_RELOADING)
#define SOC_WARM_BOOT_START(unit) (soc_state[unit] |= SOC_ST_F_RELOADING)
#define SOC_WARM_BOOT_DONE(unit)  (soc_state[unit] = soc_state[unit] & ~SOC_ST_F_RELOADING)
#endif /* BCM_WARM_BOOT_SUPPORT */

#endif /* _SOC_DRV_H */
/* End: $SDK/include/soc/drv.h */

/* From: $SDK/include/soc/scache.h */
#ifndef _SOC_SCACHE_H_
#define SOC_DEFAULT_LVL2_STABLE_SIZE (1000 * 3072)

typedef int (*soc_read_func_t) (int unit, uint8 *buf, int offset, int nbytes);
typedef int (*soc_write_func_t) (int unit, uint8 *buf, int offset, int nbytes);
typedef void *(*soc_alloc_func_t) (uint32 sz);
typedef void  (*soc_free_func_t) (void *p);
extern int soc_stable_set(int unit, int arg, uint32 flags);
extern int soc_stable_size_set(int unit, int arg);
extern int soc_switch_stable_register(int unit, soc_read_func_t rf,
                                      soc_write_func_t wf,
                                      soc_alloc_func_t alloc_f,
                                      soc_free_func_t free_f);
#endif /* _SOC_SCACHE_H_ */
/* End: $SDK/include/soc/scache.h */

/* From: $SDK/include/soc/types.h */
#ifndef _SOC_TYPES_H
typedef int soc_cos_t;
typedef int soc_port_t;
typedef int soc_block_t;
#endif /* _SOC_TYPES_H */
/* End: $SDK/include/soc/types.h */

/* From: $SDK/include/soc/mcm/allenum.h */
#ifndef _SOC_ALLENUM_H
typedef int soc_reg_t;
typedef int soc_field_t;
#endif /* _SOC_ALLENUM_H */
/* End: $SDK/include/soc/mcm/allenum.h */

/* From: $SDK/include/soc/register.h */
#ifndef _SOC_REGISTER_H
typedef struct soc_regaddrinfo_t {
    uint32          addr;
    int             valid;
    soc_reg_t       reg;        /* INVALIDr if not used */
    int             idx;        /* If array, this is an index */
    soc_block_t     block;      /* SOC_BLK_NONE if not used */
    soc_port_t      port;       /* -1 if not used */
    soc_cos_t       cos;        /* -1 if not used */
    int             is_custom;  /* custom regs use soc_custom_reg_access_t API's instead of standart soc_reg_set/get */
    uint8           acc_type;
    soc_field_t     field;      /* INVALIDf for entire reg */
} soc_regaddrinfo_t;
#endif /* _SOC_REGISTER_H */
/* End: $SDK/include/soc/register.h */

/* From: $SDK/include/soc/cmic.h */
#ifndef _SOC_CMIC_H
extern int soc_anyreg_read(int unit, soc_regaddrinfo_t *ainfo, uint64 *data);
extern int soc_cmicx_miim_divider_set_ring(int unit, int ring_index,
                                           int int_divider, int ext_divider,
                                           int out_delay);
extern int soc_miim_write(int unit, uint32 phy_id,
                          uint8 phy_reg_addr, uint16 phy_wr_data);

/*
 * -----------------------------------------------------------------------
 * LED Microcontroller Registers
 * -----------------------------------------------------------------------
 */

#define CMIC_LED_CTRL                   0x00001000
#define CMIC_LED_STATUS                 0x00001004
#define CMIC_LED_PROGRAM_RAM_BASE       0x00001800
#define CMIC_LED_DATA_RAM_BASE          0x00001c00
#define CMIC_LED_PROGRAM_RAM(_a)        (CMIC_LED_PROGRAM_RAM_BASE + 4 * (_a))
#define CMIC_LED_PROGRAM_RAM_SIZE       0x100
#define CMIC_LED_DATA_RAM(_a)           (CMIC_LED_DATA_RAM_BASE + 4 * (_a))
#define CMIC_LED_DATA_RAM_SIZE          0x100
#define CMIC_LED_REG_SIZE               4

/* Trident Switch introduces two LED processors. LED0 handles first 36 ports
 * LED1 handles the remaining 36 ports.
 */
#define CMICE_LEDUP0_CTRL               0x00001000
#define CMICE_LEDUP0_STATUS             0x00001004
#define CMICE_LEDUP0_PROGRAM_RAM_BASE   0x00001800
#define CMICE_LEDUP0_DATA_RAM_BASE      0x00001400
#define CMICE_LEDUP1_CTRL               0x00002000
#define CMICE_LEDUP1_STATUS             0x00002004
#define CMICE_LEDUP1_PROGRAM_RAM_BASE   0x00002800
#define CMICE_LEDUP1_DATA_RAM_BASE      0x00002400

#define LC_LED_ENABLE                   0x1     /* Enable */

#define LS_LED_INIT                     0x200   /* Initializing */
#define LS_LED_RUN                      0x100   /* Running */
#define LS_LED_PC                       0xff    /* Current PC */

/* This is the offset from the data ram base that is scanned for */
/* link status.  One bit per port, 0-7 in 0x80, etc.  4 bytes.  */
#define LS_LED_DATA_OFFSET              0x80
#define LS_LED_DATA_OFFSET_A0           0xa0

#endif /* _SOC_CMIC_H */
/* End: $SDK/include/soc/cmic.h */

/* From: $SDK/include/soc/mcm/cmicm.h */
/*
 * NOTE: These are legacy LED control registers that are only
 * supported on certain pre-2016 Broadcom devices. LED processor
 * is generally highly device-specific.
 */
#define CMIC_LEDUP0_CTRL_OFFSET                       (0x20000)
#define CMIC_LEDUP0_DATA_RAM_OFFSET                   (0x20400)
#define CMIC_LEDUP0_PROGRAM_RAM_OFFSET                (0x20800)
#define CMIC_LEDUP1_CTRL_OFFSET                       (0x21000)
#define CMIC_LEDUP1_DATA_RAM_OFFSET                   (0x21400)
#define CMIC_LEDUP1_PROGRAM_RAM_OFFSET                (0x21800)
/* End: $SDK/include/soc/mcm/cmicm.h */

/* From: $SDK/include/soc/cm.h */
#ifndef _SOC_CM_H
/* Trident 2 */
#define BCM56850_DEVICE_ID      0xb850
#define BCM56850_A0_REV_ID      1
#define BCM56850_A1_REV_ID      2
#define BCM56850_A2_REV_ID      3
#define BCM56851_DEVICE_ID      0xb851
#define BCM56851_A0_REV_ID      1
#define BCM56851_A1_REV_ID      2
#define BCM56851_A2_REV_ID      3
#define BCM56852_DEVICE_ID      0xb852
#define BCM56852_A0_REV_ID      1
#define BCM56852_A1_REV_ID      2
#define BCM56852_A2_REV_ID      3
#define BCM56853_DEVICE_ID      0xb853
#define BCM56853_A0_REV_ID      1
#define BCM56853_A1_REV_ID      2
#define BCM56853_A2_REV_ID      3
#define BCM56854_DEVICE_ID      0xb854
#define BCM56854_A0_REV_ID      1
#define BCM56854_A1_REV_ID      2
#define BCM56854_A2_REV_ID      3
#define BCM56855_DEVICE_ID      0xb855
#define BCM56855_A0_REV_ID      1
#define BCM56855_A1_REV_ID      2
#define BCM56855_A2_REV_ID      3
#define BCM56834_DEVICE_ID      0xb834
#define BCM56834_A0_REV_ID      1
#define BCM56834_A1_REV_ID      2
#define BCM56834_A2_REV_ID      3

/* Tomahawk */
#define BCM56960_DEVICE_ID      0xb960
#define BCM56960_A0_REV_ID      1
#define BCM56960_B0_REV_ID      0x11
#define BCM56960_B1_REV_ID      0x12
#define BCM56961_DEVICE_ID      0xb961
#define BCM56961_A0_REV_ID      1
#define BCM56961_B0_REV_ID      0x11
#define BCM56961_B1_REV_ID      0x12
#define BCM56962_DEVICE_ID      0xb962
#define BCM56962_A0_REV_ID      1
#define BCM56962_B0_REV_ID      0x11
#define BCM56962_B1_REV_ID      0x12
#define BCM56963_DEVICE_ID      0xb963
#define BCM56963_A0_REV_ID      1
#define BCM56963_B0_REV_ID      0x11
#define BCM56963_B1_REV_ID      0x12
#define BCM56930_DEVICE_ID      0xb930
#define BCM56930_A0_REV_ID      1
#define BCM56930_B0_REV_ID      0x11
#define BCM56930_B1_REV_ID      0x12
#define BCM56930_C0_REV_ID      0x21
#define BCM56931_DEVICE_ID      0xb931
#define BCM56931_A0_REV_ID      1
#define BCM56931_B0_REV_ID      0x11
#define BCM56931_C0_REV_ID      0x21
#define BCM56935_DEVICE_ID      0xb935
#define BCM56935_A0_REV_ID      1
#define BCM56935_B0_REV_ID      0x11
#define BCM56935_C0_REV_ID      0x21
#define BCM56936_DEVICE_ID      0xb936
#define BCM56936_A0_REV_ID      1
#define BCM56936_B0_REV_ID      0x11
#define BCM56936_C0_REV_ID      0x21
#define BCM56939_DEVICE_ID      0xb939
#define BCM56939_A0_REV_ID      1
#define BCM56939_B0_REV_ID      0x11
#define BCM56939_C0_REV_ID      0x21

/* Tomahawk 3 */
#define BCM56980_DEVICE_ID_MASK 0xFFF0
#define BCM56980_DEVICE_ID      0xb980
#define BCM56980_A0_REV_ID      1
#define BCM56980_B0_REV_ID      0x11
#define BCM56981_DEVICE_ID      0xb981
#define BCM56981_A0_REV_ID      1
#define BCM56981_B0_REV_ID      0x11
#define BCM56982_DEVICE_ID      0xb982
#define BCM56982_A0_REV_ID      1
#define BCM56982_B0_REV_ID      0x11
#define BCM56983_DEVICE_ID      0xb983
#define BCM56983_A0_REV_ID      1
#define BCM56983_B0_REV_ID      0x11
#define BCM56984_DEVICE_ID      0xb984
#define BCM56984_A0_REV_ID      1
#define BCM56984_B0_REV_ID      0x11

extern int soc_cm_dev_num_get(int dev, int *dev_num);
extern int soc_cm_get_id(int dev, uint16 *dev_id, uint8 *rev_id);
extern int soc_cm_get_id_driver(uint16 dev_id, uint8 rev_id,
                                uint16 *dev_id_driver,
                                uint16 *rev_id_driver);
extern const char *soc_cm_get_name(int dev);
extern uint32 soc_cm_get_dev_type(int dev);
extern const char *soc_cm_get_device_name(uint16 dev_id, uint8 rev_id);
extern void *soc_cm_salloc(int unit, int size, const char *name);
extern void soc_cm_sfree(int unit, void *ptr);
#endif /* _SOC_CM_H */
/* End: $SDK/include/soc/cm.h */

/* From: $SDK/include/sal/core/boot.h */
#ifndef _SAL_BOOT_H
extern int sal_core_init(void);
#endif /* _SAL_BOOT_H */
/* End: $SDK/include/sal/core/boot.h */

/* From: $SDK/include/sal/core/thread.h */
#ifndef _SAL_THREAD_H
typedef struct sal_thread_s{
    char thread_opaque_type;
} *sal_thread_t;
extern sal_thread_t sal_thread_self(void);
extern void sal_thread_main_set(sal_thread_t thread);
extern sal_thread_t sal_thread_main_get(void);
extern char* sal_thread_name(sal_thread_t thread,
                             char *thread_name, int thread_name_size);
extern int sal_thread_destroy(sal_thread_t);
#endif /* _SAL_THREAD_H */
/* End: $SDK/include/sal/core/thread.h */

/* From: $SDK/include/soc/error.h */
#ifndef _SOC_ERROR_H
#include <shared/error.h>
/*
 * Defines:	SOC_E_XXX
 * Purpose:	SOC API error codes
 * Notes:
 *      An error code may be converted to a string by passing
 *      the code to soc_errmsg().
 */

typedef enum {
    SOC_E_NONE                  = _SHR_E_NONE,
    SOC_E_INTERNAL              = _SHR_E_INTERNAL,
    SOC_E_MEMORY                = _SHR_E_MEMORY,
    SOC_E_UNIT                  = _SHR_E_UNIT,
    SOC_E_PARAM                 = _SHR_E_PARAM,
    SOC_E_EMPTY                 = _SHR_E_EMPTY,
    SOC_E_FULL                  = _SHR_E_FULL,
    SOC_E_NOT_FOUND             = _SHR_E_NOT_FOUND,
    SOC_E_EXISTS                = _SHR_E_EXISTS,
    SOC_E_TIMEOUT               = _SHR_E_TIMEOUT,
    SOC_E_BUSY                  = _SHR_E_BUSY,
    SOC_E_FAIL                  = _SHR_E_FAIL,
    SOC_E_DISABLED              = _SHR_E_DISABLED,
    SOC_E_BADID                 = _SHR_E_BADID,
    SOC_E_RESOURCE              = _SHR_E_RESOURCE,
    SOC_E_CONFIG                = _SHR_E_CONFIG,
    SOC_E_UNAVAIL               = _SHR_E_UNAVAIL,
    SOC_E_INIT                  = _SHR_E_INIT,
    SOC_E_PORT                  = _SHR_E_PORT,
    SOC_E_IO                    = _SHR_E_IO,
    SOC_E_ACCESS                = _SHR_E_ACCESS,
    SOC_E_NO_HANDLER            = _SHR_E_NO_HANDLER,
    SOC_E_PARTIAL               = _SHR_E_PARTIAL,
    SOC_E_LIMIT                 = _SHR_E_LIMIT
} soc_error_t;

#define SOC_SUCCESS(rv)         _SHR_E_SUCCESS(rv)
#define SOC_FAILURE(rv)         _SHR_E_FAILURE(rv)

/* Top level parity error info returned in event callbacks */
typedef enum {
    SOC_SWITCH_EVENT_DATA_ERROR_PARITY = 1,
    SOC_SWITCH_EVENT_DATA_ERROR_ECC,
    SOC_SWITCH_EVENT_DATA_ERROR_UNSPECIFIED,
    SOC_SWITCH_EVENT_DATA_ERROR_FATAL,
    SOC_SWITCH_EVENT_DATA_ERROR_CORRECTED,
    SOC_SWITCH_EVENT_DATA_ERROR_LOG,
    SOC_SWITCH_EVENT_DATA_ERROR_UNCORRECTABLE,
    SOC_SWITCH_EVENT_DATA_ERROR_AUTO_CORRECTED,
    SOC_SWITCH_EVENT_DATA_ERROR_FAILEDTOCORRECT,
    SOC_SWITCH_EVENT_DATA_ERROR_ECC_1BIT_CORRECTED, /* Single-bit ECC corrected, no traffic loss */
    SOC_SWITCH_EVENT_DATA_ERROR_ECC_2BIT_CORRECTED, /* Double-bit ECC corrected, potential traffic loss */
    SOC_SWITCH_EVENT_DATA_ERROR_PARITY_CORRECTED, /* Parity error correction */
    SOC_SWITCH_EVENT_DATA_ERROR_CFAP_MEM_FAIL
} soc_switch_data_error_t;
#endif  /* !_SOC_ERROR_H */
/* End: $SDK/include/soc/error.h */

#endif /* ! defined(_BCM_OPENSOC_H) */
