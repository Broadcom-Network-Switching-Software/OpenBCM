/**
 * \file diag_flexe_adapt.h Contains all of the flexE diag&test declarations for external usage
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DIAG_FLEXE_ADAPT_H_
#define _DIAG_FLEXE_ADAPT_H_

/*************
 * DEFINES   *
 */
#ifdef INCLUDE_FLEXE_DBG
#define DIAG_FLEXE_CORE_DRIVER_AVALIABLE    1
#endif

#ifdef DIAG_FLEXE_CORE_DRIVER_AVALIABLE
#else
#define TOP_BASE_ADDR 0x0
#define TABLE_BASE_ADDR 0x20000 /* change for asic */
#define TABLE_BASE_ADDR_FOR_FPGA 0x20000        /* outside of the core */
#define MCMACRX_BASE_ADDR 0x8000        /* change for asic */
#define MCMACTX_BASE_ADDR 0x8100        /* change for asic */
#define B66SAR_RX_BASE_ADDR 0x8400      /* change for asic */
#define B66SAR_TX_BASE_ADDR 0x8500      /* change for asic */
#define B66SAR_RX_311M_BASE_ADDR 0x8600 /* change for asic */
#define B66SAR_TX_311M_BASE_ADDR 0x8700 /* change for asic */
#define CPB_EGRESS_BASE_ADDR 0x8800     /* change for asic */
#define FLEXE_OH_RX_BASE_ADDR 0x14000   /* change for asic */
#define FLEXE_OH_TX_BASE_ADDR 0x14200   /* change for asic */
#define FLEXE_OFFSET 0x40
#define FLEXE_MULTI_DESKEW_BASE_ADDR 0x14400    /* change for asic */
#define FLEXE_MUX_BASE_ADDR 0xc200      /* change for asic */
#define FLEXE_DEMUX_BASE_ADDR 0xc400    /* change for asic */
#define RATEADP_BASE_ADDR 0xc600        /* change for asic */
#define ADJ_BASE_ADDR 0xc700    /* change for asic */
#define CHANNELIZE_BASE_ADDR 0xc800     /* change for asic */
#define FLEXE_MACRX_BASE_ADDR 0xca00    /* change for asic */
#define FLEXE_OAM_RX_BASE_ADDR 0xcc00   /* change for asic */
#define FLEXE_OAM_TX_BASE_ADDR 0xcd00   /* change for asic */
#define OAM_RX_BASE_ADDR 0x8c00 /* change for asic */
#define OAM_TX_BASE_ADDR 0x8d00 /* change for asic */
#define MAC_ADAPT_BASE_ADDR 0x9000      /* change for asic */
#define MAC_ADJ_BASE_ADDR 0x9100        /* change for asic */
#define OHOAM_EF_BASE_ADDR 0xe000       /* change for asic */
#define OHOAM_EF_ASIC_BASE_ADDR 0xe080  /* change for asic */
#define B66SWITCH_BASE_ADDR 0xa200      /* change for asic */
#define CPU_BASE_ADDR 0x16200   /* change for asic */
#define CCU_CFG_BASE_ADDR 0x16400       /* change for asic */
#define CCU_RX_BASE_ADDR 0x16480        /* change for asic */
#define CCU_TX_BASE_ADDR 0x16500        /* change for asic */
#define GLOBAL_IEEE1588_BASE_ADDR 0x16000       /* change for asic */
#define PTP_TX_OH_BASE_ADDR 0x16010     /* change for asic */
#define PTP_TX_OH_OFFSET 0x10
#define PTP_RX_OH_BASE_ADDR 0x16090     /* change for asic */
#define PTP_RX_OH_OFFSET 0x10
#define PPS_TOD_GLOBAL_TS_BASE_ADDR 0x16600     /* change for asic */
#define INF_CH_ADP_RX_BASE_ADDR 0x18000 /* change for asic */
#define INF_CH_ADP_TX_BASE_ADDR 0x18080 /* change for asic */
#define FLEXE_ENV_BASE_ADDR 0x18100     /* change for asic */
#define REQ_GEN_BASE_ADDR 0x18200       /* change for asic */
#endif

typedef struct
{
    /*
     * sar_oam_per_cnt_get 
     */
    uint32 sar_bip8_cnt;
    uint32 sar_bei_cnt;
    /*
     * flexe_oam_per_cnt_get 
     */
    uint32 flexe_bip8_cnt;
    uint32 flexe_bei_cnt;

    /*
     * sar_oam_cnt_get 
     */
    uint32 sar_packet_cnt;
    /*
     * flexe_oam_cnt_get 
     */
    uint32 flexe_packet_cnt;

    /*
     * sar_oam_bas_cnt_get 
     */
    uint32 sar_bas_packet_cnt;
    /*
     * flexe_oam_bas_cnt_get 
     */
    uint32 flexe_bas_packet_cnt;
} diag_flexe_oam_cnt_t;

/*******************
 * ADAPT FUNCTIONS *
 */
shr_error_e dnx_diag_flexe_core_run_debug_function(
    int unit,
    uint32 debug_idx,
    uint32 *debug_value,
    char *debug_string,
    sh_sand_control_t * sand_control);

/**
 * \brief
 *   wrapper function regp_read
 * \param [in] unit - The unit number.
 * \param [in] address - register address.
 * \param [out] value - register value read result.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e diag_regp_read(
    int unit,
    int address,
    uint32 *value);

/**
 * \brief
 *   wrapper function to function ram_read
 * \param [in] unit - The unit number.
 * \param [in] address - memory address.
 * \param [in] index - memory entry index. 
 * \param [in] width - width of memory.
 * \param [out] value - memory value read result.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e diag_ram_read(
    int unit,
    int address,
    int index,
    int width,
    uint32 *value);

/**
 * \brief
 *   wrapper function to function regp_write
 * \param [in] unit - The unit number.
 * \param [in] address - register address.
 * \param [out] value - register value to be write.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e diag_regp_write(
    int unit,
    int address,
    uint32 value);

/**
 * \brief
 *   wrapper function to read flexE core memory
 * \param [in] unit - The unit number.
 * \param [in] address - memory address.
 * \param [in] index - memory entry index. 
 * \param [in] width - width of memory.
 * \param [in] value - memory value to be write.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e diag_ram_write(
    int unit,
    int address,
    int index,
    int width,
    uint32 *value);

shr_error_e diag_flexe_core_oam_cnt_get(
    int unit,
    int channel,
    diag_flexe_oam_cnt_t * oam_cnt);

#endif
