/** \file kbp_connectivity.h
 *
 * Functions for handling jericho2 kbp connectivity.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#if defined(INCLUDE_KBP)
/*
 * {
 */
#ifndef __KBP_CONNECTIVITY_INCLUDED__
/*
 * {
 */
#define __KBP_CONNECTIVITY_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (Jr2) family only!"
#endif

/*************
 * INCLUDES  *
 */
/*
 * {
 */
#include <soc/kbp/alg_kbp/include/db.h>
#include <soc/kbp/alg_kbp/include/default_allocator.h>
#include <soc/kbp/alg_kbp/include/device.h>
#include <soc/kbp/alg_kbp/include/key.h>
#include <soc/kbp/alg_kbp/include/instruction.h>
#include <soc/kbp/alg_kbp/include/errors.h>
#include <soc/kbp/alg_kbp/include/ad.h>
#include <soc/kbp/alg_kbp/include/kbp_legacy.h>
#include <soc/kbp/alg_kbp/include/init.h>
#include <soc/kbp/alg_kbp/include/kbp_pcie.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_elk.h>
#include <bcm/port.h>
/*
 * }
 */

/*************
 * DEFINES   *
 */
/*
 * {
 */

#define DNX_KBP_MDIO_CLAUSE (45)
/*
 * Opcode
 */

#define DNX_KBP_OPCODE_NUM_MAX              256
/*
 * Lookup Table Write
 */
#define DNX_KBP_CPU_WR_LUT_OPCODE           255
/*
 * Lookup Table Read
 */
#define DNX_KBP_CPU_RD_LUT_OPCODE           254
/*
 * PIO Write
 */
#define DNX_KBP_CPU_PIOWR_OPCODE            253
/*
 * PIO Read Database-X,Register, and User Data Array
 */
#define DNX_KBP_CPU_PIORDX_OPCODE           252
/*
 * PIO Read Database-Y
 */
#define DNX_KBP_CPU_PIORDY_OPCODE           251
/*
 * ROP Error Record
 */
#define DNX_KBP_CPU_ERR_OPCODE              250
/*
 * Database Block Copy
 */
#define DNX_KBP_CPU_BLK_COPY_OPCODE         249
/*
 * Database Block Move
 */
#define DNX_KBP_CPU_BLK_MOVE_OPCODE         248
/*
 * Database Block Clear
 */
#define DNX_KBP_CPU_BLK_CLR_OPCODE          247
/*
 * Database Block Invalidate
 */
#define DNX_KBP_CPU_BLK_EV_OPCODE           246

/**
 * \brief
 *  define the first opcode in KBP management group
 */
#define DNX_KBP_CPU_MANAGMENT_OPCODE_FIRST  200
/*
 * Context Buffer Write
 */
#define DNX_KBP_CPU_CTX_BUFF_WRITE_OPCODE   10
/*
 * PAD OPCODE
 */
#define DNX_KBP_CPU_PAD_OPCODE              0

/*
 * Size for Request
 */
#define DNX_KBP_CPU_WR_LUT_REQ_SIZE       4
#define DNX_KBP_CPU_RD_LUT_REQ_SIZE       1
#define DNX_KBP_CPU_PIOWR_REQ_SIZE        24
#define DNX_KBP_CPU_PIORDX_REQ_SIZE       4
#define DNX_KBP_CPU_PIORDY_REQ_SIZE       4
#define DNX_KBP_CPU_BLK_COPY_REQ_SIZE     8
#define DNX_KBP_CPU_BLK_MOVE_REQ_SIZE     8
#define DNX_KBP_CPU_BLK_CLR_REQ_SIZE      5
#define DNX_KBP_CPU_BLK_EV_REQ_SIZE       5

/*
 * Size for Reply
 */
#define DNX_KBP_CPU_WR_LUT_REP_SIZE       1
#define DNX_KBP_CPU_RD_LUT_REP_SIZE       4
#define DNX_KBP_CPU_PIOWR_REP_SIZE        1
#define DNX_KBP_CPU_PIORDX_REP_SIZE       11
#define DNX_KBP_CPU_PIORDY_REP_SIZE       11
#define DNX_KBP_CPU_BLK_COPY_REP_SIZE     1
#define DNX_KBP_CPU_BLK_MOVE_REP_SIZE     1
#define DNX_KBP_CPU_BLK_CLR_REP_SIZE      1
#define DNX_KBP_CPU_BLK_EV_REP_SIZE       1

/*
 * Opcode type
 */
#define DNX_KBP_OPCODE_TYPE_REQUEST       1
#define DNX_KBP_OPCODE_TYPE_REPLY         3

/*
 * Number of kbp instance
 */
#define DNX_KBP_MAX_NOF_INST     (DNX_DATA_MAX_ELK_GENERAL_MAX_KBP_INSTANCE)
/*
 * Second KBP exists or not
 * This indication is true when:
 * 1. KBP connectivity properties are configured with second KBP
 * or
 * 2. when KBP blackhole mode indicates 2 KBP instances
 */
#define DNX_KBP_IS_SECOND_KBP_EXIST  \
        ((dnx_data_elk.connectivity.topology_get(unit)->start_lane[2] >= \
         dnx_data_elk.general.max_lane_id_get(unit)) || \
            (dnx_data_elk.connectivity.blackhole_get(unit) == 2))

#define DNX_KBP_INST_ITER(unit, kbp_inst) \
        for (kbp_inst = 0; kbp_inst < (DNX_KBP_IS_SECOND_KBP_EXIST ? DNX_KBP_MAX_NOF_INST : 1); kbp_inst++)

#define DNX_KBP_INST_MASTER 0
#define DNX_KBP_INST_SLAVE  1

#define DNX_KBP_OP_IS_DUAL_MODE(unit)  ((dnx_data_elk.connectivity.connect_mode_get(unit) == \
                                         DNX_NIF_ELK_TCAM_CONNECT_MODE_DUAL_SHARED) || \
                                       (dnx_data_elk.connectivity.connect_mode_get(unit) == \
                                        DNX_NIF_ELK_TCAM_CONNECT_MODE_DUAL_SMT))

#define DNX_KBP_OP2_IS_SINGLE_PORT_MODE(unit)  (dnx_data_elk.connectivity.connect_mode_get(unit) == \
                                         DNX_NIF_ELK_TCAM_CONNECT_MODE_SINGLE_PORT)

#define DNX_KBP_VALID_KBP_PORTS_NUM    (2)

#define DNX_KBP_USED_KBP_PORTS_NUM    (DNX_KBP_OP2_IS_SINGLE_PORT_MODE(unit) ? 1 : DNX_KBP_VALID_KBP_PORTS_NUM)

/*
 * }
 */

/*************
 * MACROS    *
 */
/*
 * {
 */

/*
 * }
 */

/*************
 * TYPE DEFS *
 */
/*
 * {
 */

typedef enum
{
    /*
     *  OP Single Host Dual Port mode
     */
    DNX_NIF_ELK_TCAM_CONNECT_MODE_SINGLE = 0,
    /*
     *  OP Dual Host Quad Port Shared mode
     */
    DNX_NIF_ELK_TCAM_CONNECT_MODE_DUAL_SHARED = 1,
    /*
     *  OP Dual Host Quad Port SMT mode
     */
    DNX_NIF_ELK_TCAM_CONNECT_MODE_DUAL_SMT = 2,
    /*
     *  OP2 Single Port mode
     */
    DNX_NIF_ELK_TCAM_CONNECT_MODE_SINGLE_PORT = 3,
    /*
     *  Number of types in ARAD_NIF_ELK_TCAM_DEV_TYPE
     */
    DNX_NIF_ELK_TCAM_NOF_CONNECT_MODE = 4
} DNX_NIF_ELK_TCAM_CONNECT_MODE;

typedef struct dnx_kbp_user_data_s
{
    int device;
    uint32 kbp_mdio_id;
} dnx_kbp_user_data_t;

typedef struct dnx_kbp_opcode_map_s
{
    uint8 opcode;
    /*
     * Reply size, 1-based
     */
    uint32 rx_size;
    uint32 rx_type;
    /*
     * Response size, 1-based
     */
    uint32 tx_size;
    uint32 tx_type;
} dnx_kbp_opcode_map_t;
/*
 * }
 */

/*************
 * GLOBALS   *
 */
/*
 * {
 */

/*
 * }
 */

/*************
 * FUNCTIONS *
 */
/*
 * {
 */
int32_t dnx_kbp_callback_usleep(
    void *handle,
    uint32_t usec);

int32_t dnx_kbp_callback_mdio_read(
    void *handle,
    int32_t chip_no,
    uint8_t dev,
    uint16_t reg,
    uint16_t * value);

int32_t dnx_kbp_callback_mdio_write(
    void *handle,
    int32_t chip_no,
    uint8_t dev,
    uint16_t reg,
    uint16_t value);

int32_t dnx_kbp_callback_reset_device(
    void *handle,
    int32_t s_reset_low,
    int32_t c_reset_low);

shr_error_e dnx_kbp_init_device_type_get(
    int unit,
    enum kbp_device_type *device_type);

shr_error_e dnx_kbp_init_user_data_get(
    int unit,
    int kbp_inst,
    dnx_kbp_user_data_t * user_data);

shr_error_e dnx_kbp_ilkn_interface_param_get(
    int unit,
    bcm_core_t core,
    bcm_port_t * ilkn_port,
    int *ilkn_num_lanes,
    int *ilkn_rate,
    int *ilkn_metaframe);

shr_error_e dnx_kbp_init_config_param_get(
    int unit,
    int kbp_inst,
    bcm_core_t core,
    uint32 *flags,
    struct kbp_device_config *kbp_config);

shr_error_e dnx_kbp_opcode_map_set(
    int unit,
    bcm_core_t core,
    dnx_kbp_opcode_map_t * opcode_map);

shr_error_e dnx_kbp_opcode_map_get(
    int unit,
    bcm_core_t core,
    dnx_kbp_opcode_map_t * opcode_map);

shr_error_e dnx_kbp_interface_init(
    int unit);

shr_error_e dnx_kbp_init_reset(
    int unit);

shr_error_e dnx_kbp_init_kbp_stat_interface(
    int unit);

shr_error_e dnx_kbp_stat_interface_enable_get(
    int unit,
    int *enable);

/*
 * }
 */

/*
 * }
 */
#endif /* __KBP_CONNECTIVITY_INCLUDED__ */
/*
 * }
 */
#endif /* defined(INCLUDE_KBP) */
