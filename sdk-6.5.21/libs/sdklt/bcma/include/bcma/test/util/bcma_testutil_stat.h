/*! \file bcma_testutil_stat.h
 *
 * Statistic operation utility.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTUTIL_STAT_H
#define BCMA_TESTUTIL_STAT_H

#include <sal/sal_types.h>

/*!
 * \brief Clear port counter.
 *
 * \param [in] unit Unit number.
 * \param [in] port The port number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_stat_clear(int unit, int port);

/*! Statistic type */
typedef enum bcma_testutil_stat_type_e {
    SNMP_IF_IN_UCAST_OCTETS = 0,           /* RFC 1213 */
    SNMP_IF_IN_UCAST_PKTS = 1,             /* RFC 1213 */
    SNMP_IF_OUT_UCAST_OCTETS = 6,          /* RFC 1213 */
    SNMP_IF_OUT_UCAST_PKTS = 7,            /* RFC 1213 */
    SNMP_IF_OUT_ERRORS = 10,               /* RFC 1213 */
    SNMP_IF_DOT1P_IN_FRAMES = 18,          /* RFC 1493 */
    SNMP_IF_DOT1P_OUT_FRAMES = 19,         /* RFC 1493 */
    SNMP_ETHER_STATS_UNDERSIZE_PKTS = 24,  /* RFC 1757 */
    SNMP_ETHER_STATS_FRAGMENTS = 25,       /* RFC 1757 */
    SNMP_ETHER_STATS_JABBERS = 35,         /* RFC 1757 */
    SNMP_ETHER_STATS_CRC_ALIGN_ERRORS = 39,/* RFC 1757 */
    /* Additional error stats */
    SNMP_IF_OUT_STATS_JABBERS = 60,
    SNMP_IF_OUT_STATS_FCS_ERR_PKTS = 61,
    SNMP_IF_OUT_STATS_UFL_PKTS = 62,
    SNMP_IF_OUT_OVERSZ_PKTS = 63,
    SNMP_ETHER_STATS_FCS_ERR_PKTS = 64,
    SNMP_ETHER_STATS_OVRSZ_PKTS = 65,
    SNMP_ETHER_STATS_MTU_ERR_PKTS = 66,
    SNMP_ETHER_STATS_LEN_OORANGE_PKTS = 67,
    SNMP_ETHER_CODE_ERR_PKTS = 68,
    SNMP_ETHER_MC_SA_PKTS = 69,
    SNMP_ETHER_PROMISCUOUS_PKTS = 70,
} bcma_testutil_stat_type_t;


/*! Statistic id for counters from CTR_MAC */
typedef enum bcma_testutil_stat_mac_e {
    RX_UC_PKT,
    TX_UC_PKT,
    RX_FRAGMENT_PKT,
    RX_BYTES,
    TX_BYTES,
    RX_PKT,
    TX_PKT,
    STAT_ID_MAC_MAX
} bcma_testutil_stat_mac_t;

/*! Statistic id for counters from CTR_MAC_ERR */
typedef enum bcma_testutil_stat_macerr_e {
    TX_ERR_PKT,
    TX_JABBER_PKT,
    TX_FCS_ERR_PKT,
    TX_FIFO_UNDER_RUN_PKT,
    TX_OVER_SIZE_PKT,
    RX_UNDER_SIZE_PKT,
    RX_JABBER_PKT,
    RX_ALIGN_ERR_PKT,
    RX_FCS_ERR_PKT,
    RX_OVER_SIZE_PKT,
    RX_MTU_CHECK_ERR_PKT,
    RX_LEN_OUT_OF_RANGE_PKT,
    RX_CODE_ERR_PKT,
    RX_MC_SA_PKT,
    RX_PROMISCUOUS_PKT,
    STAT_ID_MACERR_MAX
} bcma_testutil_stat_macerr_t;

/*!
 * \brief Statistic field array type.
 *
 * The type is used to return field array for getting stat in batch mode.
 */
typedef struct bcma_testutil_stat_s {
    /*! Stat id. */
    uint32_t id;

    /*! Stat name. */
    char *name;

    /*! Stat value. */
    uint64_t value;

} bcma_testutil_stat_t;

/*!
 * \brief Get port counter.
 *
 * \param [in] unit Unit number.
 * \param [in] port The port number.
 * \param [in] stat_type The counter field id.
 * \param [out] value The counter field value.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_stat_get(int unit, int port, bcma_testutil_stat_type_t stat_type,
                       uint64_t *value);

/*!
 * \brief Get port mac counter in batch mode.
 *
 * \param [in] unit Unit number.
 * \param [in] port The port number.
 * \param [out] stat The counter field array.
 * \param [out] size The counter field array size.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_stat_mac_get(int unit, int port, bcma_testutil_stat_t **stat,
                           int *size);

/*!
 * \brief Get port mac error counter in batch mode.
 *
 * \param [in] unit Unit number.
 * \param [in] port The port number.
 * \param [out] stat The counter field array.
 * \param [out] size The counter field array size.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_stat_macerr_get(int unit, int port, bcma_testutil_stat_t **stat,
                              int *size);

/*!
 * \brief Show 64-bit number as Kilo, Mega, Giga or Tera
 *
 * \param [in] numer 64-bit numerator
 * \param [in] denom 64-bit denominator
 * \param [in] need_suffix Display suffix (ex. K/M/G/T) or not
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern void
bcma_testutil_stat_show(uint64_t numer, uint64_t denom, bool need_suffix);

/*!
 * \brief Enable port counter.
 *
 * \param [in] unit Unit number.
 * \param [in] port The port number.
 * \param [in] en Enable or disable port counter.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_stat_enable(int unit, int port, bool en);

#endif /* BCMA_TESTUTIL_STAT_H */
