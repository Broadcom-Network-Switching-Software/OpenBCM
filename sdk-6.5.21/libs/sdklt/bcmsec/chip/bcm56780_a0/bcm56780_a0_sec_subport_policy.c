/*! \file bcm56780_a0_sec_subport_policy.c
 *
 * File containing subport policy related defines and internal function for
 * bcm56780_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmbd/bcmbd.h>

#include <bcmdrd/bcmdrd_types.h>

#include <bcmsec/generated/bcmsec_ha.h>
#include <bcmsec/bcmsec_types.h>
#include <bcmsec/bcmsec_pt_internal.h>
#include <bcmsec/chip/bcm56780_a0_sec_subport_policy.h>
#include <bcmdrd/chip/bcm56780_a0_enum.h>
#include <bcmltd/chip/bcmltd_id.h>


#define BSL_LOG_MODULE  BSL_LS_BCMSEC_SUBPORT
/*******************************************************************************
 * Local definitions
 */

/*******************************************************************************
 * Private functions
 */

/*******************************************************************************
 * Public functions
 */

int
bcm56780_a0_subport_policy_macsec_decrypt_set(int unit, bcmltd_sid_t ltid,
                                   bcmsec_pt_info_t *pt_dyn_info,
                                   bcmltd_field_t *in)
{
    int size;
    bcmdrd_sid_t sid = SUB_PORT_POLICY_TABLEm;
    bcmdrd_fid_t fid;
    uint32_t *fval = NULL, *val = NULL;
    uint32_t data;
    uint32_t enable = 0, forward = 0;
    bool set0, set1, set2, set3, set4;

    SHR_FUNC_ENTER(unit);

    set0 = false; /* ESPHDR_OFFSETf, SECTAG_OFFSETf */
    set1 = false; /* INNER_L2_OFFSETf, OUTER_IP_OFFSETf */
    set2 = false; /* INNER_L2_VALIDf, OUTER_IP_VALIDf */
    set3 = false; /* ESPHDR_TAG_LBL_ADJ_ENf, SECTAG_TAG_LBL_ADJ_ENf */
    set4 = false; /* INNERL2_TAG_LBL_ADJ_ENf, OUTERIP_TAG_LBL_ADJ_ENf */
    size = BCMSEC_MAX_ENTRY_WSIZE * sizeof(uint32_t);
    SHR_ALLOC(fval, size, "bcmsecSubportPolicyPtEntry");
    SHR_NULL_CHECK(fval, SHR_E_MEMORY);
    sal_memset(fval, 0, size);

    size = BCMSEC_MAX_FIELD_WSIZE * sizeof(uint32_t);
    SHR_ALLOC(val, size, "bcmsecSubportPolicyPtField");
    SHR_NULL_CHECK(val, SHR_E_MEMORY);
    sal_memset(val, 0, size);

    if (in && (pt_dyn_info->flags & BCMSEC_PT_DYN_FLAGS_UPDATE)) {
        SHR_IF_ERR_EXIT
            (bcmsec_pt_indexed_read(unit, sid, ltid,
                                    pt_dyn_info, fval));
    }

    while (in) {
        data = (uint32_t)in->data;
        val[0] = data;
        fid = INVALIDf;
        switch (in->id) {
            case SEC_DECRYPT_SUBPORT_POLICYt_UNSECURED_CONTROL_PORT_MODEf:
                switch (val[0]) {
                    case 0:
                        enable = 0; forward = 0;
                        break;
                    case 1:
                        enable = 1; forward = 0;
                        break;
                    case 2:
                        enable = 1; forward = 1;
                        break;
                    default:
                        enable = 0; forward = 0;
                        break;
                }
                in = in->next;
                continue;
            case SEC_DECRYPT_SUBPORT_POLICYt_SECURED_CONTROL_PORTf:
                fid = TAG_CTRL_PORT_ENf;
                break;
            case SEC_DECRYPT_SUBPORT_POLICYt_TAG_VALIDATEf:
                fid = TAG_VALIDATEFRAMESf;
                break;
            case SEC_DECRYPT_SUBPORT_POLICYt_DROP_IPV4_AND_MPLS_FAILURESf:
                fid = DROP_IPV4_CHKCUM_FAIL_AND_MPLS_BOS_MISSf;
                break;
            case SEC_DECRYPT_SUBPORT_POLICYt_POINT_TO_POINTf:
                fid = OPERPOINTTOPOINTMACf;
                break;
            case SEC_DECRYPT_SUBPORT_POLICYt_SCIf:
                fid = SCIf;
                break;
            case SEC_DECRYPT_SUBPORT_POLICYt_MTUf:
                fid = MTUf;
                break;
            case SEC_DECRYPT_SUBPORT_POLICYt_CUSTOM_PROTOCOLf:
                fid = CUSTOM_PROTO_SPf;
                break;
            case SEC_DECRYPT_SUBPORT_POLICYt_SECTAG_OFFSET_BYTESf:
                fid = SECTAG_OFFSETf;
                if (val[0]) {
                    if (set0) {
                        LOG_DEBUG(BSL_LOG_MODULE,
                            (BSL_META("ESP header and SECTAG offset conflict.\n")));
                        SHR_ERR_EXIT(SHR_E_PARAM);
                    }
                    set0 = true;
                }
                break;
            case SEC_DECRYPT_SUBPORT_POLICYt_SEC_MACSEC_SECTAG_ETHERTYPE_IDf:
                fid = SECTAG_ETYPE_SELf;
                break;
            case SEC_DECRYPT_SUBPORT_POLICYt_INNER_DST_MAC_OFFSET_BYTESf:
                fid = INNER_L2_OFFSETf;
                if (val[0]) {
                    if (set1) {
                        LOG_DEBUG(BSL_LOG_MODULE,
                            (BSL_META("Inner L2 and Outer IP offset conflict.\n")));
                        SHR_ERR_EXIT(SHR_E_PARAM);
                    }
                    set1 = true;
                }
                break;
            case SEC_DECRYPT_SUBPORT_POLICYt_INNER_DST_MACf:
                fid = INNER_L2_VALIDf;
                if (val[0]) {
                    if (set2) {
                        LOG_DEBUG(BSL_LOG_MODULE,
                            (BSL_META("Inner L2 and Outer IP valid conflict.\n")));
                        SHR_ERR_EXIT(SHR_E_PARAM);
                    }
                    set2 = true;
                }
                break;
            case SEC_DECRYPT_SUBPORT_POLICYt_ADJUST_SECTAG_OFFSETf:
                fid = SECTAG_TAG_LBL_ADJ_ENf;
                if (val[0]) {
                    if (set3) {
                        LOG_DEBUG(BSL_LOG_MODULE,
                            (BSL_META("ESP Tag and SecTAG tag conflict.\n")));
                        SHR_ERR_EXIT(SHR_E_PARAM);
                    }
                    set3 = true;
                }
                break;
            case SEC_DECRYPT_SUBPORT_POLICYt_ADJUST_INNER_DST_MAC_OFFSETf:
                fid = INNERL2_TAG_LBL_ADJ_ENf;
                if (val[0]) {
                    if (set4) {
                        LOG_DEBUG(BSL_LOG_MODULE,
                            (BSL_META("Inner L2 tag and outer IP tag conflict.\n")));
                        SHR_ERR_EXIT(SHR_E_PARAM);
                    }
                    set4 = true;
                }
                break;
            case SEC_DECRYPT_SUBPORT_POLICYt_ESP_OFFSET_BYTESf:
                fid = ESPHDR_OFFSETf;
                if (val[0]) {
                    if (set0) {
                        LOG_DEBUG(BSL_LOG_MODULE,
                            (BSL_META("ESP header and SECTAG offset conflict.\n")));
                        SHR_ERR_EXIT(SHR_E_PARAM);
                    }
                    set0 = true;
                }
                break;
            case SEC_DECRYPT_SUBPORT_POLICYt_OUTER_IPf:
                fid = OUTER_IP_VALIDf;
                if (val[0]) {
                    if (set2) {
                        LOG_DEBUG(BSL_LOG_MODULE,
                            (BSL_META("Inner L2 and Outer IP valid conflict.\n")));
                        SHR_ERR_EXIT(SHR_E_PARAM);
                    }
                    set2 = true;
                }
                break;
            case SEC_DECRYPT_SUBPORT_POLICYt_OUTER_IP_OFFSET_BYTESf:
                fid = OUTER_IP_OFFSETf;
                if (val[0]) {
                    if (set1) {
                        LOG_DEBUG(BSL_LOG_MODULE,
                            (BSL_META("Inner L2 and Outer IP offset conflict.\n")));
                        SHR_ERR_EXIT(SHR_E_PARAM);
                    }
                    set1 = true;
                }
                break;
            case SEC_DECRYPT_SUBPORT_POLICYt_ADJUST_ESP_OFFSETf:
                fid = ESPHDR_TAG_LBL_ADJ_ENf;
                if (val[0]) {
                    if (set3) {
                        LOG_DEBUG(BSL_LOG_MODULE,
                            (BSL_META("ESP Tag and SecTAG tag conflict.\n")));
                        SHR_ERR_EXIT(SHR_E_PARAM);
                    }
                    set3 = true;
                }
                break;
            case SEC_DECRYPT_SUBPORT_POLICYt_ADJUST_OUTER_IP_OFFSETf:
                fid = OUTERIP_TAG_LBL_ADJ_ENf;
                if (val[0]) {
                    if (set4) {
                        LOG_DEBUG(BSL_LOG_MODULE,
                            (BSL_META("Inner L2 tag and outer IP tag conflict.\n")));
                        SHR_ERR_EXIT(SHR_E_PARAM);
                    }
                    set4 = true;
                }
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
        SHR_IF_ERR_EXIT
            (bcmsec_field_set(unit, sid, fid, fval, val));
        in = in->next;
    }
    SHR_IF_ERR_EXIT
        (bcmsec_field_set(unit, sid, UNTAG_CTRL_PORT_ENf, fval, &enable));
    SHR_IF_ERR_EXIT
        (bcmsec_field_set(unit, sid, UNTAG_FORWARDFRAMESf, fval, &forward));
    SHR_IF_ERR_EXIT
        (bcmsec_pt_indexed_write(unit, sid, ltid, pt_dyn_info, fval));
exit:
    if (val) {
        SHR_FREE(val);
    }
    if (fval) {
        SHR_FREE(fval);
    }
    SHR_FUNC_EXIT();
}

