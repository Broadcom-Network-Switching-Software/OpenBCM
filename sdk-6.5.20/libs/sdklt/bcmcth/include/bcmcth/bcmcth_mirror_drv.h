/*! \file bcmcth_mirror_drv.h
 *
 * BCMCTH Mirroring driver object.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_MIRROR_DRV_H
#define BCMCTH_MIRROR_DRV_H

#include <sal/sal_types.h>
#include <shr/shr_bitop.h>
#include <shr/shr_util.h>
#include <bcmcth/generated/mirror_ha.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmlrd/bcmlrd_conf.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmpkt/bcmpkt_lbhdr.h>

/*! The maximum field ID of MIRROR logical tables */
#define BCMCTH_MIRROR_FIELD_ID_MAX (64)

/*! The maximum flexhdr size (bytes). */
#define BCMCTH_MIRROR_FLEXHDR_SIZE_BYTES \
    (SHR_WORDS2BYTES(BCMCTH_MIRROR_FLEXHDR_SIZE_WORD))

/*! Maximum number of counter fields. */
#define BCMCTH_MIRROR_SFLOW_CTR_FIELD_MAX (16)

/*!
 * \brief callback of set value into a GENERIC_LOOPBACK_T field.
 *
 * \param [in] variant Variant type.
 * \param [in,out] generic_loopback_t GENERIC_LOOPBACK_T handle.
 * \param [in] fid field ID.
 * \param [in] val Set value.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_UNAVAIL Not support the field.
 */
typedef int (*bcmpkt_generic_loopback_t_field_set_f)(
    bcmlrd_variant_t variant,
    uint32_t *generic_loopback_t,
    int fid, uint32_t val);

/*!
 * \brief callback of get value from a GENERIC_LOOPBACK_T field.
 *
 * \param [in] variant Variant type.
 * \param [in] generic_loopback_t GENERIC_LOOPBACK_T handle.
 * \param [in] fid field ID.
 * \param [out] val Field value.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_UNAVAIL Not support the field.
 */
typedef int (*bcmpkt_generic_loopback_t_field_get_f)(
    bcmlrd_variant_t variant,
    uint32_t *generic_loopback_t,
    int fid,
    uint32_t *val);

/*! Structure for MIRROR flexhdr. */
typedef struct bcmcth_mirror_flexhdr_info_s {
    /*! Logical table ID. */
    bcmdrd_sid_t                          sid;
    /*! Logical table key field ID. */
    bcmdrd_fid_t                          key_fid;
    /*! Logical table enable field ID. */
    bcmdrd_fid_t                          enable_fid;
    /*! Logical table flexhdr field name. */
    const char                            *fname;
    /*! Logical table get value from a field. */
    bcmpkt_generic_loopback_t_field_set_f set;
    /*! Logical table set value from a field. */
    bcmpkt_generic_loopback_t_field_get_f get;
    /*! Logical table header size in bytes. */
    uint32_t                              size;
} bcmcth_mirror_flexhdr_info_t;

/*! Opcodes for writing MIRROR_ENCAP_XXXt hardware entries. */
typedef enum bcmcth_mirror_encap_op_e {

    /*! Update mirror encap content. */
    BCMCTH_MIRROR_ENCAP_OP_SET,

    /*! Delete mirror encap content. */
    BCMCTH_MIRROR_ENCAP_OP_DEL,

    /*! Must be the last. */
    BCMCTH_MIRROR_ENCAP_OP_COUNT

} bcmcth_mirror_encap_op_t;

/*! The data structure for MIRROR_ENCAP_XXXXt entry. */
typedef struct bcmcth_mirror_encap_s {
    /*! Bitmap of Fields to be operated. */
    SHR_BITDCLNAME(fbmp, BCMCTH_MIRROR_FIELD_ID_MAX);

    /*! Transaction ID. */
    uint32_t trans_id;

    /*! Logical tabld ID. */
    bcmltd_sid_t lt_id;

    /*! Mirror encap type. */
    bcmcth_mirror_encap_type_t type;

    /*! Mirror encap ID. */
    uint32_t encap_id;

    /*! Enable to add an outer VLAN tag into the encapsulation header. */
    uint32_t outer_vlan;

    /*! Remove the outer VLAN tag from the mirrored packet. */
    uint32_t untag;

    /*! VLAN priority to insert into the mirrored packet. */
    uint8_t pri;

    /*! CFI value to insert into the mirrored packet. */
    uint8_t cfi;

    /*! VLAN ID to insert into the mirrored packet. */
    uint32_t vlan_id;

    /*! TPID to insert into the mirrored packet. */
    uint32_t tpid;

    /*! Source MAC address. */
    uint64_t src_mac;

    /*! Destination MAC address. */
    uint64_t dst_mac;

    /*! Source IPv4 address. */
    uint32_t src_ipv4;

    /*! Destination IPv4 address. */
    uint32_t dst_ipv4;

    /*! IPv4 type of service/IPv6 traffic class. */
    uint32_t tos_tc;

    /*! IPv4 time to live/IPv6 hop limit. */
    uint32_t ttl_hl;

    /*! IPv4 protocol type/IPv6 next header. */
    uint32_t proto_nh;

    /*! IPv4 don't fragment flag. */
    uint32_t df;

    /*! Ethertype. */
    uint32_t ethertype;

    /*! L3 GRE header protocol. */
    uint32_t gre_header;

    /*! Session ID. */
    uint32_t session_id;

    /*! Switch ID. */
    uint32_t switch_id;

    /*! Source IPv6 address. */
    uint64_t src_ipv6[2];

    /*! Destination IPv6 address. */
    uint64_t dst_ipv6[2];

    /*! IPv6 flow label. */
    uint32_t fl;

    /*! UDP source port. */
    uint16_t udp_src_port;

    /*! UDP destination port. */
    uint16_t udp_dst_port;

    /*! Enable sequence number. */
    bool sq;

    /*! Observation domain for IP flow information export (IPFIX) header. */
    uint32_t od;

    /*! Template identifier for IPFIX header. */
    uint16_t ti;

    /*! Specifies the type of metadata. */
    uint16_t meta_type;

    /*! Encapsulation header metadata. */
    uint16_t meta_data;

    /*! Packet sampling (PSAMP) IP flow information. */
    uint32_t ipfix_ver;

    /*! Packet sampling (PSAMP) epoch / revision number. */
    uint16_t ipfix_epoch;

    /*! Indicate variable length / Varible flag. */
    uint8_t vf;

    /*! Padding. */
    uint8_t pad;

    /*! Combination of erspan3_fixed_hdr.p_ft_hwid_d_gra_o. */
    uint16_t p_ft_hwid_d_gra_o;

    /*!
     * Enable to add Virtual Network Tag (VN-Tag) in Network Interface
     * Virtualization (NIV) mirror tunneling.
     */
    uint16_t vntag;

    /*! Source Virtual Interface of NIV tag. */
    uint16_t niv_src_vif;

    /*! Destination Virtual Interface of NIV tag. */
    uint16_t niv_dst_vif;

    /*! Enable HiGig3 Base Header encapsulation. */
    bool higig3;

    /*! HiGig3 Format - Ethertype. */
    uint16_t higig3_ethertype;

    /*! HiGig3 Format - Base Header. (6-Bytes) */
    uint64_t higig3_basehdr;

    /*!
     * Source of VNID of VXLAN header.
     * TRUE  : VNID from vni.
     * FALSE :
     *   VNID is written with value from Virtual Forwarding Instance(VFI)
     *   attributes table.
     */
    bool vni_valid;

    /*!
     * 24-Bit Virtual Network Interface ID.
     * Applicable only if vni_valid == TRUE.
     */
    uint32_t vni;

    /*! Encapsulation mode. */
    uint8_t encap_mode;

    /*! Enable to insert flexhdr ahead of mirror encapsulation header. */
    bool flexhdr_encap;

    /*! flexhdr data buffer. */
    uint32_t flexhdr[BCMCTH_MIRROR_FLEXHDR_SIZE_WORD];

    /*! size of flexhdr data buffer. */
    uint32_t flexhdr_size;

    /*! IFA1 Probe Marker 1 */
    uint32_t ifa_1_probe_marker1;

    /*! IFA1 Probe Marker 2 */
    uint32_t ifa_1_probe_marker2;

    /*! IFA1 version */
    uint8_t ifa_1_version;

    /*! IFA1 message type */
    uint8_t ifa_1_msg_type;

    /*! IFA1 Request Vector */
    uint16_t ifa_1_request_vector;

    /*! IFA1 Action Vector */
    uint16_t ifa_1_action_vector;

    /*! IFA1 Hop Limit */
    uint8_t ifa_1_hop_limit;

    /*! IFA1 max length */
    uint16_t ifa_1_max_length;

    /*! Mirror-on-drop mode. */
    uint8_t mod_mode;

} bcmcth_mirror_encap_t;

/*! The data structure for MIRROR_ENCAP_INSTANCE_ATTRt entry. */
typedef struct bcmcth_mirror_encap_ins_attr_s {
    /*! Bitmap of Fields to be operated. */
    SHR_BITDCLNAME(fbmp, BCMCTH_MIRROR_FIELD_ID_MAX);

    /*! Transaction ID. */
    uint32_t trans_id;

    /*! Logical tabld ID. */
    bcmltd_sid_t lt_id;

    /*! Mirror encap ID. */
    uint32_t encap_id;

    /*! Destination system port. */
    uint16_t system_port;

    /*! Enable truncate. */
    bool truncate_en;

    /*! Truncate profile index */
    uint8_t truncate_ptr;

} bcmcth_mirror_encap_ins_attr_t;

/*!
 * \brief Flex sFlow stats info structure.
 */
typedef struct bcmcth_mirror_flex_sflow_stat_s {
    /*! LT entry attributes. */
    uint32_t attrib;

    /*! Flex sFlow profile index. */
    uint32_t flex_sflow_id;

    /*! Pipe index. */
    int      pipe_idx;

    /*! New SAMPLING_ELIGIBLE_PKT value. */
    uint64_t sampling_eligible_pkt;

    /*! New SAMPLED_PKT value. */
    uint64_t sampled_pkt;

    /*! New ELIGIBLE_PKT_SNAPSHOT value. */
    uint64_t eligible_pkt_snapshot;

    /*! Update SAMPLING_ELIGIBLE_PKT value. */
    bool     upd_sampling_eligible_pkt;

    /*! Update SAMPLED_PKT value. */
    bool     upd_sampled_pkt;

    /*! Update ELIGIBLE_PKT_SNAPSHOT value. */
    bool     upd_eligible_pkt_snapshot;
} bcmcth_mirror_flex_sflow_stat_t;

/*!
 * \brief LT counter field info structure.
 */
typedef struct ctr_sflow_field_s {
    /*! First bit of this field. */
    uint16_t minbit;

    /*! Last bit of this field. */
    uint16_t maxbit;
} ctr_sflow_field_t;

/*!
 * \brief Flex sFlow stats control structure
 */
typedef struct bcmcth_mirror_flex_sflow_control_s {
    /*! sFlow entry size in words. */
    uint32_t entry_wsize;

    /*! Number of LT whose field info is saved. */
    uint32_t lt_info_num;

    /*! LT counter field info */
    ctr_sflow_field_t lt_field[BCMCTH_MIRROR_SFLOW_CTR_FIELD_MAX];
} bcmcth_mirror_flex_sflow_control_t;

/*!
 * \brief Mirror Encap edit tcam info structure
 */
typedef struct bcmcth_mirror_encap_edit_tcam_data_s {

    /*! EDIT TCAM pid for encap. */
    bcmdrd_sid_t pid;

    /*! EDIT TCAM index for encap . */
    int tcam_index;

} bcmcth_mirror_encap_edit_tcam_data_t;

/*!
 * \brief Mirror Encap info structure
 */
typedef struct bcmcth_mirror_encap_data_s {

    /*! EDIT_ID for the mirror encap. */
    int edit_id;

    /*!
     * Number of EDIT TCAM entries associated
     * with a mirror encap.
     */
    int edit_tcam_count;

    /*! EDIT TCAM pid and index. */
    const bcmcth_mirror_encap_edit_tcam_data_t *edit_tcam_data;

} bcmcth_mirror_encap_data_t;

/*!
 * \brief Mirror EDIT CTRL ID info structure
 */
typedef struct bcmcth_mirror_encap_edit_ctrl_id_data_s {

    /*! EDIT_CTRL_ID for RSPAN. */
    int edit_ctrl_id_rspan;

    /*! EDIT_CTRL_ID for ERSPAN_1. */
    int edit_ctrl_id_erspan_1;

    /*! EDIT_CTRL_ID for PSAMP. */
    int edit_ctrl_id_psamp;

    /*! EDIT_CTRL_ID for SFLOW. */
    int edit_ctrl_id_sflow;

    /*! EDIT_CTRL_ID for ERSPAN_3. */
    int edit_ctrl_id_erspan_3;

    /*! EDIT_CTRL_ID for MIRROR_ON_DROP. */
    int edit_ctrl_id_mirror_on_drop;

    /*! EDIT_CTRL_ID for VXLAN. */
    int edit_ctrl_id_vxlan;

    /*! EDIT_CTRL_ID for NO_ENCAP_LPBK. */
    int edit_ctrl_id_no_encap_lb;

    /*! EDIT_CTRL_ID for IFA_1_PROBE. */
    int edit_ctrl_id_ifa_1_probe;

    /*! EDIT_CTRL_ID for IFA_LBK. */
    int edit_ctrl_id_ifa_lb;

} bcmcth_mirror_encap_edit_ctrl_id_data_t;

/*!
 * \brief MIRROR data object
 *
 * MIRROR data is used to provide the variant specific information.
 *
 * Each device should provide its own instance to BCMCTH MIRROR module by
 * \ref bcmcth_mirror_drv_init() from the top layer. BCMCTH MIRROR internally
 * will use this interface to get the corresponding information.
 */
typedef struct bcmcth_mirror_data_s {

    /*! Header ID for MIRROR transport. */
    int hdr_id_mirror_transport;

    /*! Egr Mirror seq 0 container number. */
    int seq_num_0_cont_num;

    /*! Egr Mirror seq 0 bus number. */
    int seq_num_0_src_sel;

    /*! Egr Mirror seq 1 container number. */
    int seq_num_1_cont_num;

    /*! Egr Mirror seq 1 bus number. */
    int seq_num_1_src_sel;

    /*! Base EDIT_ID for MIRROR encapsulation profile. */
    int edit_id_base;

    /*! Default truncate pointer for MIRROR. */
    int default_truncate_ptr;

    /*! Number of Mirror encap. */
    int num_encap;

    /*! Mirror encap info. */
    const bcmcth_mirror_encap_data_t *encap_data;

    /*! EDIT CTRL ID for all the ENCAP modes. */
    const bcmcth_mirror_encap_edit_ctrl_id_data_t *edit_ctrl_id_data;

    /*! Pointer to device variant flexhdr info. */
    const bcmcth_mirror_flexhdr_info_t *flexhdr;

    /*! Number of flexhdr info. */
    const uint32_t flexhdr_num;
} bcmcth_mirror_data_t;

/*!
 * \brief Initialize the MIRROR.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot indicator.
 * \param [in] data Warmboot indicator.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_mirror_init_f)(int unit,
                                    int warm,
                                    const bcmcth_mirror_data_t *data);

/*!
 * \brief De-initialize the MIRROR.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_mirror_deinit_f)(int unit);

/*!
 * \brief Update MIRROR_ENCAP_XXXt hardware register(s).
 *
 * This device-specific function is called by the IMM callback handler
 * in order to update the actual hardware registers associated with
 * the MIRROR_ENCAP_XXXt table.
 *
 * \param [in] unit Unit number.
 * \param [in] encap_op Type of operation (see \ref bcmcth_mirror_encap_op_t).
 * \param [in] encap Encap content of mirror-to port (MTP) to update.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO I/O access error.
 */
typedef int (*bcmcth_mirror_encap_op_f)(int unit,
                                        bcmcth_mirror_encap_op_t encap_op,
                                        bcmcth_mirror_encap_t *encap);


/*!
 * \brief Update MIRROR_ENCAP_INSTANCE_ATTRt hardware register(s).
 *
 * This device-specific function is called by the IMM callback handler
 * in order to update the actual hardware registers associated with
 * the MIRROR_ENCAP_XXXt table.
 *
 * \param [in] unit Unit number.
 * \param [in] encap_op Type of operation (see \ref bcmcth_mirror_encap_op_t).
 * \param [in] encap Encap attribute content to update.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO I/O access error.
 */
typedef int (*bcmcth_mirror_encap_ins_attr_op_f)(int unit,
                                        bcmcth_mirror_encap_op_t encap_op,
                                        bcmcth_mirror_encap_ins_attr_t *encap);


/*!
 * \brief Write flex sFlow stats
 *
 * \param [in] unit    Unit number.
 * \param [in] entry   Flex sFlow stats info.
 *
 * \return SHR_E_NONE Success.
 */
typedef int (*bcmcth_mirror_ctr_flex_sflow_write_f)(
    int unit,
    bcmcth_mirror_flex_sflow_stat_t *info);

/*!
 * \brief Read flex sFlow stats
 *
 * \param [in] unit    Unit number.
 * \param [inout] entry    Flex sFlow stats info.
 *
 * \return SHR_E_NONE Success.
 */
typedef int (*bcmcth_mirror_ctr_flex_sflow_read_f)(
    int unit,
    bcmcth_mirror_flex_sflow_stat_t *info);

/*!
 * \brief MIRROR driver object
 *
 * MIRROR driver is used to provide the chip specific information.
 *
 * Each device should provide its own instance to BCMCTH MIRROR module by
 * \ref bcmcth_mirror_drv_init() from the top layer. BCMCTH MIRROR internally
 * will use this interface to get the corresponding information.
 */
typedef struct bcmcth_mirror_drv_s {
    /*! Init the MIRROR feature. */
    bcmcth_mirror_init_f init;

    /*! De-init the MIRROR feature. */
    bcmcth_mirror_deinit_f deinit;

    /*! Get MIRROR session type register information. */
    bcmcth_mirror_encap_op_f encap_op;

    /*! Get MIRROR instance attr register information. */
    bcmcth_mirror_encap_ins_attr_op_f encap_ins_attr_op;

    /*! MIRROR data for current unit */
    const bcmcth_mirror_data_t *data;

    /*! Write flex sFlow stats. */
    bcmcth_mirror_ctr_flex_sflow_write_f ctr_flex_sflow_write;

    /*! Read flex sFlow stats. */
    bcmcth_mirror_ctr_flex_sflow_read_f ctr_flex_sflow_read;

} bcmcth_mirror_drv_t;

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern bcmcth_mirror_drv_t *_bc##_cth_mirror_drv_get(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*! Signature of MIRROR data attach function */
typedef int (*cth_mirror_data_attach_f)(bcmcth_mirror_drv_t *drv);

/*! \cond  Externs for variant attach. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
extern int _bd##_vu##_va##_cth_mirror_data_attach(bcmcth_mirror_drv_t *drv);
#define BCMLTD_VARIANT_OVERRIDE
#include <bcmlrd/chip/bcmlrd_variant.h>
/*! \endcond */

/*!
 * \brief Get the MIRROR driver of the device.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_mirror_drv_init(int unit);

/*!
 * \brief Initialize the MIRROR driver.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot indicator.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_mirror_init(int unit, bool warm);

/*!
 * \brief De-initialize the MIRROR driver.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_mirror_deinit(int unit);

/*!
 * \brief Update MIRROR_ENCAP_XXXt hardware register(s).
 *
 * This device-specific function is called by the IMM callback handler
 * in order to update the actual hardware registers associated with
 * the MIRROR_ENCAP_XXXt table.
 *
 * \param [in] unit Unit number.
 * \param [in] encap_op Type of operation (see \ref bcmcth_mirror_encap_op_t).
 * \param [in] encap Encap content of mirror-to port (MTP) to update.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO I/O access error.
 */
extern int
bcmcth_mirror_encap_op(
    int unit,
    bcmcth_mirror_encap_op_t encap_op,
    bcmcth_mirror_encap_t *encap);

/*!
 * \brief Update MIRROR_ENCAP_INSTANCE_ATTRt hardware register(s).
 *
 * This device-specific function is called by the IMM callback handler
 * in order to update the actual hardware registers associated with
 * the MIRROR_ENCAP_INSTANCE_ATTRt table.
 *
 * \param [in] unit Unit number.
 * \param [in] encap_op Type of operation (see \ref bcmcth_mirror_encap_op_t).
 * \param [in] encap Encap attribute content to update.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO I/O access error.
 */
extern int
bcmcth_mirror_encap_ins_attr_op(
    int unit,
    bcmcth_mirror_encap_op_t encap_op,
    bcmcth_mirror_encap_ins_attr_t *encap);

/*!
 * \brief Retrieve MIRROR data for the input unit.
 *
 * This function is used to retrieve the MIRROR data for currently configured
 * device variant of an unit.
 *
 * \param [in] unit Unit number.
 * \param [out] data MIRROR data for input unit.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO I/O access error.
 */
extern int
bcmcth_mirror_data_get(
    int unit,
    const bcmcth_mirror_data_t **data);

/*!
 * \brief Initialize IMM callbacks.
 *
 * The MIRROR custom table handling is done via the IMM component,
 * which needs a callback interface to commit changes to hardware.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_mirror_imm_init(int unit, bool warm);

/*!
 * \brief Deinitialize IMM callbacks.
 *
 * The MIRROR custom table handling is done via the IMM component,
 * which needs a callback interface to commit changes to hardware.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_mirror_imm_deinit(int unit);

/*!
 * \brief Write flex sFlow stats
 *
 * \param [in] unit    Unit number.
 * \param [in] info   Flex sFlow stats info.
 *
 * \return SHR_E_NONE Success.
 */
extern int
bcmcth_mirror_ctr_flex_sflow_write(int unit,
                                   bcmcth_mirror_flex_sflow_stat_t *info);

/*!
 * \brief Read flex sFlow stats
 *
 * \param [in] unit    Unit number.
 * \param [inout] info    Flex sFlow stats info.
 *
 * \return SHR_E_NONE Success.
 */
extern int
bcmcth_mirror_ctr_flex_sflow_read(int unit,
                                  bcmcth_mirror_flex_sflow_stat_t *info);

#endif /* BCMCTH_MIRROR_DRV_H */
