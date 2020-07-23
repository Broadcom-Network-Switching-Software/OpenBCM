/*! \file bcmmgmt_sysm_default.c
 *
 * Contains the default list of system manager components.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmbd/bcmbd_sysm.h>
#include <bcmlt/bcmlt_sysm.h>
#include <bcmtrm/trm_sysm.h>
#include <bcmptm/bcmptm_cth_uft_be_sysm.h>
#include <bcmptm/bcmptm_cth_uft_sysm.h>
#include <bcmptm/bcmptm_cth_alpm_sysm.h>
#include <bcmptm/bcmptm_cth_alpm_be_sysm.h>
#include <bcmptm/bcmptm_rm_hash_sysm.h>
#include <bcmptm/bcmptm_rm_tcam_sysm.h>
#include <bcmptm/bcmptm_rm_alpm_sysm.h>
#include <bcmptm/bcmptm_ctr_flex_sysm.h>
#include <bcmptm/bcmptm_sysm.h>
#include <bcmltm/bcmltm_sysm.h>
#include <bcmfp/bcmfp_sysm.h>
#include <bcmlm/bcmlm_sysm.h>
#include <bcmcfg/bcmcfg_sysm.h>
#include <bcmimm/bcmimm_sysm.h>
#include <bcmecmp/bcmecmp_sysm.h>
#include <bcmltp/bcmltp_sysm.h>
#include <bcmpc/bcmpc_sysm.h>
#include <bcmtm/bcmtm_sysm.h>
#include <bcmcth/bcmcth_meter_sysm.h>
#include <bcmcth/bcmcth_l2_sysm.h>
#include <bcmcth/bcmcth_tnl_sysm.h>
#include <bcmlrd/bcmlrd_sysm.h>
#include <bcmevm/bcmevm_sysm.h>
#include <bcmcth/bcmcth_port_sysm.h>
#include <bcmdi/bcmdi_sysm.h>
#include <bcmcth/bcmcth_agm_sysm.h>
#include <bcmcth/bcmcth_dlb_sysm.h>
#include <bcmcth/bcmcth_ecn_sysm.h>
#include <bcmcth/bcmcth_lb_sysm.h>
#include <bcmcth/bcmcth_mirror_sysm.h>
#include <bcmcth/bcmcth_trunk_sysm.h>
#include <bcmcth/bcmcth_vlan_sysm.h>
#include <bcmissu/bcmissu_sysm.h>
#include <bcmcth/bcmcth_ctr_eflex_sysm.h>
#include <bcmcth/bcmcth_ctr_evict_sysm.h>
#include <bcmcth/bcmcth_ctr_dbgsel_sysm.h>
#include <bcmcth/bcmcth_ldh_sysm.h>
#include <bcmcth/bcmcth_flex_digest_sysm.h>
#include <bcmcth/bcmcth_flex_qos_sysm.h>
#include <bcmmgmt/bcmmgmt_sysm_default.h>
#include <bcmptm/bcmptm_ser_sysm.h>
#include <bcmcth/bcmcth_mon_sysm.h>
#include <bcmcth/bcmcth_l3_sysm.h>
#include <bcmcth/bcmcth_udf_sysm.h>
#include <bcmcth/bcmcth_oam_sysm.h>
#include <bcmcth/bcmcth_ts_sysm.h>
#include <bcmsec/bcmsec_sysm.h>

static bcmmgmt_comp_start_f def_comp_start[] = {
    bcmlrd_start,
    bcmcfg_start,
    bcmbd_start,
    bcmlt_start,
    bcmptm_cth_uft_be_start,
    bcmptm_cth_uft_start,
    bcmptm_cth_alpm_be_start,
    bcmptm_cth_alpm_start,
    bcmptm_rm_hash_start,
    bcmptm_rm_tcam_start,
    bcmptm_rm_alpm_start,
    bcmptm_ctr_flex_start,
    bcmptm_start,
    bcmdi_start,
    bcmltm_start,
    bcmtrm_start,
    bcmltp_start,
    bcmlm_start,
    bcmimm_start,
    bcmecmp_start,
    bcmpc_start,
    bcmtm_start,
    bcmcth_meter_start,
    bcmcth_tnl_start,
    bcmcth_l2_start,
    bcmevm_start,
    bcmcth_port_start,
    bcmcth_agm_start,
    bcmcth_dlb_start,
    bcmcth_ecn_start,
    bcmcth_lb_start,
    bcmcth_mirror_start,
    bcmcth_trunk_start,
    bcmcth_vlan_start,
    bcmissu_start,
    bcmfp_start,
    bcmcth_ctr_evict_start,
    bcmcth_ctr_eflex_start,
    bcmcth_ctr_dbgsel_start,
    bcmcth_ldh_start,
    bcmcth_flex_digest_start,
    bcmcth_udf_start,
    bcmptm_ser_start,
    bcmcth_mon_start,
    bcmcth_l3_start,
    bcmcth_flex_qos_start,
    bcmcth_state_eflex_start,
    bcmcth_oam_start,
    bcmcth_ts_start,
    bcmsec_start,
};

static const bcmmgmt_comp_list_t comp_list = {
    def_comp_start,
    COUNTOF(def_comp_start)
};

const bcmmgmt_comp_list_t *
bcmmgmt_sysm_default_comp_list(void)
{
    return &comp_list;
}
