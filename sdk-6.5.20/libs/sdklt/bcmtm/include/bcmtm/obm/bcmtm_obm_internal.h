/*! \file bcmtm_obm_internal.h
 *
 * Definitions for OBM.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMTM_OBM_INTERNAL_H
#define BCMTM_OBM_INTERNAL_H

#include <bcmtm/bcmtm_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/chip/bcmltd_tm_constants.h>
/*! Packet priority from VLAN header. */
#define  OBM_PRIORITY_TYPE_VLAN     0

/*! Packet priority from DSCP. */
#define  OBM_PRIORITY_TYPE_DSCP     1

/*! Packet priority from MPLS EXP bits. */
#define  OBM_PRIORITY_TYPE_MPLS     2

/*! Packet priority from ETAG packet. */
#define  OBM_PRIORITY_TYPE_ETAG     3

/*! Packet priority from HIGIG3 header. */
#define  OBM_PRIORITY_TYPE_HIGIG3   4

/*! Maximum number of TPID supported. */
#define  MAX_TPID_CNT    4

/*!
 * \brief TM OBM flow control configuration values.
 */
typedef struct bcmtm_obm_fc_cfg_s {
    /*! Enable flow control */
    uint8_t fc;

    /*! Flow control message type. */
    uint8_t fc_type;

    /*! Enable flow control for lossless0 packets. */
    uint8_t lossless0_fc;

    /*! Enable flow control for lossless1 packets. */
    uint8_t lossless1_fc;

    /*! COS bitmap enabled for lossless0 packets. */
    uint16_t lossless0_cos_bmap[1];

    /*! COS bitmap enabled for lossless1 packets. */
    uint16_t lossless1_cos_bmap[1];

    /*! Operational status for the entry. */
    uint8_t opcode;

    /*! Action for the entry. */
    uint8_t action;
}bcmtm_obm_fc_cfg_t;

/*!
 * \brief TM OBM policy configuration values.
 */
typedef struct bcmtm_obm_pc_pm_pkt_parse_cfg_s {


    /*! Inner tpid */
    uint16_t  inner_tpid;

    /*! Outer tpid0 */
    uint16_t  outer_tpid[MAX_TPID_CNT];

    /*! Etag parsing enable */
    bool  etag_parsing;

    /*! Etag ethertype */
    uint32_t  etag_ethertype;

    /*! Vntag parsing enable */
    bool  vntag_parsing;

    /*! Vntag ethertype */
    uint32_t  vntag_ethertype;

    /*! gsh ethertype */
    uint32_t  gsh_ethertype;

    /*! gsh ethertype mask */
    uint32_t  gsh_ethertype_mask;

    /*! User protocol matching */
    bool  user_protocol_matching[BCMTM_MAX_OBM_POLICY_DEPTH];

    /*! User defined mac adress */
    uint64_t  mac_addr[BCMTM_MAX_OBM_POLICY_DEPTH];

    /*! User defined mac adress mask */
    uint64_t  mac_addr_mask[BCMTM_MAX_OBM_POLICY_DEPTH];

    /*! User defined ethertype */
    uint32_t  ethertype[BCMTM_MAX_OBM_POLICY_DEPTH];

    /*! User defined mac adress mask */
    uint32_t  ethertype_mask[BCMTM_MAX_OBM_POLICY_DEPTH];

    /*! User defined ob priority value */
    uint8_t   traffic_class;
}bcmtm_obm_pc_pm_pkt_parse_cfg_t;
/*!
 * \brief TM OBM port  control configuration values.
 */
typedef struct bcmtm_obm_port_pkt_parse_cfg_s {
    /*! Header type */
    uint8_t  header_type;

    /*! Enable dscp mapping. */
    bool enable_dscp_mapping;

    /*! Enable mpls mapping. */
    bool enable_mpls_mapping;

    /*! Enable mpls mapping. */
    bool enable_etag_mapping;

    /*! Enable inner tpid. */
    bool enable_inner_tpid;

    /*! Enable outer tpid0. */
    bool enable_outer_tpid[MAX_TPID_CNT];

    /*! Operational status for the entry. */
    uint8_t opcode;

    /*! Action for the entry. */
    uint8_t action;

    /*! default PCP value. */
    uint8_t   default_pri;
}bcmtm_obm_port_pkt_parse_cfg_t;

/*!
 * \brief TM OBM port usage values.
 */
typedef struct bcmtm_obm_usg_cfg_t {

    /*! Current usage in total bytes */
    uint32_t byte;

    /*! Current lossless 0 usage in total bytes */
    uint32_t lossless0_byte;

    /*! Current lossless 1 usage in total bytes */
    uint32_t lossless1_byte;

    /*! Current lossy usage in total bytes */
    uint32_t lossy_byte;

    /*! Max usage in total bytes in history */
    uint32_t max_usage_byte;

    /*! Port operational status. */
    uint8_t status;

}bcmtm_obm_usg_cfg_t;

/*!
 * Device specific function pointers for physical table configurations
 * for obm port flow controls.
 */
typedef int (*bcmtm_obm_port_fc_pt_set_f) (int unit,
                                         bcmltd_sid_t ltid,
                                         bcmtm_lport_t lport,
                                         bcmtm_obm_fc_cfg_t *obm_fc_cfg);
/*!
 * Device specific function pointers for physical table configurations
 * for obm port config .
 */
typedef int (*bcmtm_obm_port_pkt_parse_pt_f) (int unit,
                                         bcmltd_sid_t ltid,
                                         bcmtm_lport_t lport,
                                         bcmtm_obm_port_pkt_parse_cfg_t *obm_pc_cfg);
/*!
 * Device specific function pointers for physical table configurations
 * for obm policy config .
 */
typedef int (*bcmtm_obm_pc_pm_pkt_parse_pt_f) (int unit,
                                         bcmltd_sid_t ltid,
                                         int pmid,
                                         bcmtm_obm_pc_pm_pkt_parse_cfg_t *obm_pc_cfg);

/*!
 * Device specific function pointers for physical table configurations
 * for obm port usage.
 */
typedef int (*bcmtm_obm_port_usage_pt_f) (int unit,
                                         bcmltd_sid_t ltid,
                                         bcmtm_lport_t lport,
                                         uint64_t flags,
                                         bcmtm_obm_usg_cfg_t *obm_usg_cfg);

/*!
 * Device specific function pointers for physical table configurations
 * for obm pkt priority to traffic class mapping.
 */
typedef int (*bcmtm_obm_port_pkt_pri_tc_pt_set_f)(int unit,
                                                bcmltd_sid_t ltid,
                                                bcmtm_lport_t lport,
                                                uint8_t pkt_type,
                                                uint8_t pkt_pri,
                                                uint8_t tc);

/*!
 * Device specific function pointers for port validation for obm logical tables.
 */
typedef int (*bcmtm_obm_port_validation_f)(int unit,
                                           bcmtm_lport_t lport);

/*! Device specific functions. */
typedef struct bcmtm_obm_drv_s {
    /*! OBM flow control physical table configuration. */
    bcmtm_obm_port_fc_pt_set_f fc_set;
    /*! OBM packet priority to traffic class mapping. */
    bcmtm_obm_port_pkt_pri_tc_pt_set_f pkt_pri_tc_set;
    /*! OBM port usage physical table set. */
    bcmtm_obm_port_usage_pt_f port_usg_set;
    /*! OBM port usage get. */
    bcmtm_obm_port_usage_pt_f port_usg_get;
    /*! OBM port packet parser configuration. */
    bcmtm_obm_port_pkt_parse_pt_f port_pkt_parse_set;
    /*! OBM port macro based parser configurations. */
    bcmtm_obm_pc_pm_pkt_parse_pt_f policy_set;
    /*! OBM port validation. */
    bcmtm_obm_port_validation_f port_validation;
} bcmtm_obm_drv_t;

/*!
 * \brief OBM port flow control internal updates.
 *
 * Auto updates the operational state for the configured instructions
 * when there is any port flex operation.
 *
 * \param [in] unit Logical unit number.
 * \param [in] lport Logical port number.
 * \param [in] action Port add/ delete action.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
extern int
bcmtm_obm_port_fc_internal_update(int unit,
                                  bcmtm_lport_t lport,
                                  uint8_t action);

/*!
 * \brief OBM port packet priority traffic class mapping internal updates.
 *
 * Auto updates the operational state for the configured instructions
 * when there is any port flex operation.
 *
 * \param [in] unit Logical unit number.
 * \param [in] lport Logical port number.
 * \param [in] action Port add/ delete action.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
extern int
bcmtm_obm_port_pkt_pri_tc_internal_update(int unit,
                                          bcmtm_lport_t lport,
                                          uint8_t action);

/*!
 * \brief OBM port packet parsing configuration internal updates.
 *
 * Auto updates the operational state for the configured instructions
 * when there is any port flex operation.
 *
 * \param [in] unit Logical unit number.
 * \param [in] lport Logical port number.
 * \param [in] action Port add/ delete action.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
extern int
bcmtm_obm_port_pkt_parse_internal_update(int unit,
                                         bcmtm_lport_t lport,
                                         uint8_t action);


#endif /* BCMTM_OBM_INTERNAL_H */
