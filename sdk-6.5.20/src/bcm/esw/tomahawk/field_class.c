/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:       field_class.c
 * Purpose:    BCM56960 Field Processor Group management functions.
 */

#include <soc/defs.h>
#if defined(BCM_TOMAHAWK_SUPPORT) && defined(BCM_FIELD_SUPPORT)
#include <shared/bsl.h>
#include <soc/mem.h>
#include <soc/drv.h>
#include <bcm_int/esw/tomahawk.h>

#if defined(BCM_TOMAHAWK3_SUPPORT)
#include <bcm_int/esw/tomahawk3.h>
#endif

#include <bcm/field.h>
#include <bcm_int/esw/field.h>

/* Local functions prototypes. */
STATIC int
_field_th_class_group_validate(int unit,
                               _field_group_t *fg);
STATIC int
_field_th_class_group_sort(int unit,
                           _field_group_t *fg,
                           _field_class_type_t ctype);
STATIC void
_field_th_class_entry_key_show(int unit,
                                   _field_entry_t *class_ent,
                                   _field_class_type_t ctype,
                                   uint32 *cdata);
STATIC int
_field_th_class_hwindex_get(int unit,
                            _field_entry_t *entry,
                            _field_class_type_t ctype,
                            int *hw_index);
STATIC int
_field_th_class_ethertype_install(int unit,
                                  _field_entry_t *entry);
STATIC int
_field_th_class_ethertype_read(int unit,
                               _field_entry_t *entry,
                               uint32 *databuf);
STATIC int
_field_th_class_l4srcport_install(int unit,
                                  _field_entry_t *entry);
STATIC int
_field_th_class_l4srcport_read(int unit,
                               _field_entry_t *entry,
                               uint32 *databuf);
STATIC int
_field_th_class_l4dstport_install(int unit,
                                  _field_entry_t *entry);
STATIC int
_field_th_class_l4dstport_read(int unit,
                               _field_entry_t *entry,
                               uint32 *databuf);
STATIC int
_field_th_class_srccompression_install(int unit,
                                       _field_entry_t *entry);
STATIC int
 _field_th_class_srccompression_read(int unit,
                                     _field_entry_t *entry,
                                     uint32 *databuf);
STATIC int
_field_th_class_dstcompression_install(int unit,
                                       _field_entry_t *entry);
STATIC int
_field_th_class_dstcompression_read(int unit,
                                    _field_entry_t *entry,
                                    uint32 *databuf);
STATIC int
_field_th_class_ttl_install(int unit,
                            _field_entry_t *entry);
STATIC int
_field_th_class_ttl_read(int unit,
                         _field_entry_t *entry,
                         uint32 *databuf);
STATIC int
_field_th_class_tos_install(int unit,
                            _field_entry_t *entry);
STATIC int
_field_th_class_tos_read(int unit,
                         _field_entry_t *entry,
                         uint32 *databuf);
STATIC int
_field_th_class_ipproto_install(int unit,
                                _field_entry_t *entry);
STATIC int
_field_th_class_ipproto_read(int unit,
                             _field_entry_t *entry,
                             uint32 *databuf);
STATIC int
_field_th_class_tcp_install(int unit,
                            _field_entry_t *entry);
STATIC int
_field_th_class_tcp_read(int unit,
                         _field_entry_t *entry,
                         uint32 *databuf);

STATIC int
_field_th_class_iptunnelttl_install(int unit,
                            _field_entry_t *entry);
STATIC int
_field_th_class_iptunnelttl_read(int unit,
                         _field_entry_t *entry,
                         uint32 *databuf);

/*
 * Function:
 *    _bcm_field_th_class_type_qset_get
 * Purpose:
 *    Get Field Class Type from qset.
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     qset     - (IN) Field Qualifier Set.
 *     ctype    - (OUT) Field Class type.
 * Returns:
 *     BCM_E_XXX
 * Notes:
 *     This api assumes that only valid set of qset is passed
 *     for class type.
 */
int
_bcm_field_th_class_type_qset_get(int unit, bcm_field_qset_t *qset,
                                            _field_class_type_t *ctype)
{
    int rv = BCM_E_NONE;    /* Operation Return Status */

    /* Input parameters check */
    if (qset == NULL || ctype == NULL) {
        return (BCM_E_INTERNAL);
    }

    /* Retreive class type from provided qset */
    if (BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyEtherType)) {
        *ctype = _FieldClassEtherType;
    } else if (BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyTtl)) {
        *ctype = _FieldClassTtl;
    } else if (BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyTos)) {
        *ctype = _FieldClassToS;
    } else if (BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyIpProtocol)) {
        *ctype = _FieldClassIpProto;
    } else if (BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyL4SrcPort) ||
              (BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyFcoeOxID))) {
        *ctype = _FieldClassL4SrcPort;
    } else if (BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyL4DstPort) ||
              (BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyFcoeRxID))) {
        *ctype = _FieldClassL4DstPort;
    } else if (BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyTcpControl)) {
        *ctype = _FieldClassTcp;
    } else if (BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifySrcIp) ||
               BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifySrcIp6) ||
               BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyFibreChanSrcId)) {
        *ctype = _FieldClassSrcCompression;
    } else if (BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyDstIp) ||
               BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyDstIp6) ||
               BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyFibreChanDstId)) {
        *ctype = _FieldClassDstCompression;
    } else if (BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyIpTunnelTtl)) {
        *ctype = _FieldClassIpTunnelTtl;
    } else {
        rv = BCM_E_PARAM;
    }

    return (rv);
}

/*
 * Function:
 *    _bcm_field_th_class_ctype_supported
 * Purpose:
 *    Check if the class type is supported for the device.
 * Parameters:
 *     unit        - (IN) BCM Device number.
 *     ctype       - (IN) Field Class Type.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_th_class_ctype_supported(int unit,
                                 _field_class_type_t ctype)
{
    int rv = BCM_E_NONE;

#if defined (BCM_TOMAHAWK3_SUPPORT)
    if ((soc_feature(unit, soc_feature_th3_style_fp))) {
        return _field_th3_class_ctype_supported (unit, ctype);
    }
#endif

    switch(ctype) {
        case _FieldClassEtherType:
        case _FieldClassTtl:
        case _FieldClassToS:
        case _FieldClassIpProto:
        case _FieldClassL4SrcPort:
        case _FieldClassL4DstPort:
        case _FieldClassTcp:
        case _FieldClassSrcCompression:
        case _FieldClassDstCompression:
              rv = BCM_E_NONE;
              break;
        default:
              rv = BCM_E_UNAVAIL;
              break;
    }

    return rv;
}

/*
 * Function:
 *    _field_th_class_max_entries
 * Purpose:
 *    Get Field Class max entries supported for
 *    provide class pipe and type.
 * Parameters:
 *     unit        - (IN) BCM Device number.
 *     cpipe       - (IN) Field Class Pipe.
 *     ctype       - (IN) Field Class Type.
 *     max_entries - (OUT) Max Entries Supported.
 * Returns:
 *     BCM_E_XXX
 */
int
_field_th_class_max_entries(int unit, int cpipe,
                                      _field_class_type_t ctype,
                                      int *max_entries)
{
    int rv = BCM_E_NONE;    /* Operation Return Status */

    /* Validate class pipe */
    if (cpipe < 0 || cpipe >= _FP_MAX_NUM_PIPES) {
        return (BCM_E_INTERNAL);
    }

    /* Validate and fill as per class type */
    switch(ctype) {
        case _FieldClassEtherType:
            *max_entries = _FP_MAX_NUM_CLASS_ETHERTYPE;
            break;
        case _FieldClassTtl:
            *max_entries = _FP_MAX_NUM_CLASS_TTL;
            break;
        case _FieldClassToS:
            *max_entries = _FP_MAX_NUM_CLASS_TOS;
            break;
        case _FieldClassIpProto:
            *max_entries = _FP_MAX_NUM_CLASS_IP_PROTO;
            break;
        case _FieldClassL4SrcPort:
            *max_entries = _FP_MAX_NUM_CLASS_L4_SRC_PORT;
            break;
        case _FieldClassL4DstPort:
            *max_entries = _FP_MAX_NUM_CLASS_L4_DST_PORT;
            break;
        case _FieldClassTcp:
            *max_entries = _FP_MAX_NUM_CLASS_TCP;
            break;
        case _FieldClassSrcCompression:
            *max_entries = _FP_MAX_NUM_CLASS_SRC_COMPRESSION;
            break;
        case _FieldClassDstCompression:
            *max_entries = _FP_MAX_NUM_CLASS_DST_COMPRESSION;
            break;
        case _FieldClassIpTunnelTtl:
            *max_entries = _FP_MAX_NUM_CLASS_IPTUNNELTTL;
            break;
        default:
            rv = BCM_E_PARAM;
    }

    return (rv);
}

/*
 * Function:
 *    _field_th_class_group_validate
 * Purpose:
 *    Validate a field processor group for stage class.
 * Parameters:
 *    unit       - (IN) BCM Device Number.
 *    class_fg   - (IN/OUT) Field Class Group.
 * Returns:
 *    BCM_E_XXX.
 * Notes:
 *    This function is used to validate
 *    qset, aset , pbmp for field class group
 *    and update instance information.
 */
STATIC int
_field_th_class_group_validate(int unit, _field_group_t *class_fg)
{
    int rv = BCM_E_NONE;    /* Operation Return Status */
    int stage_cnt = 0, class_cnt = 0;
                            /* Stage and Class Count */
    _field_stage_t *stage_fc = NULL;
                            /* Field Stage Control */
    _field_control_t *fc = NULL;
                            /* Field Control Structure */
    _field_group_t *fg = NULL;
                            /* Field Group Control */
    int pipe;               /* Device pipe iterator */
    bcm_pbmp_t t_pbmp;      /* Pipe Port bitmap */
    bcm_field_qset_t *qset = NULL;
                            /* Field Qualifier Set */
    bcm_field_qset_t qset_supported;
                            /* Field Class Qualifier Set */
    _bcm_field_aset_t *aset = NULL;
                            /* Field Action Set */
    bcm_field_qset_t qset_fg;
                            /* Field Group Qualifier Set */
    bcm_pbmp_t         all_pbmp;   /* Group port bitmap. */

    /* Input parameters check. */
    if ((NULL == class_fg) ||
        (class_fg->stage_id != _BCM_FIELD_STAGE_CLASS)) {
        return (BCM_E_INTERNAL);
    }

    /* Get Field Control */
    rv = _field_control_get(unit, &fc);
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    /* Get Field Class Stage Control */
    rv = _field_stage_control_get(unit, class_fg->stage_id, &stage_fc);
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    /* Assign qset and aset */
    qset = &class_fg->qset;
    aset = &class_fg->aset;

    /* Set Supported QSet for Stage Class */
    BCM_FIELD_QSET_INIT(qset_supported);
    BCM_FIELD_QSET_ADD(qset_supported, bcmFieldQualifyStageClass);
    BCM_FIELD_QSET_ADD(qset_supported, bcmFieldQualifyStageClassExactMatch);
    BCM_FIELD_QSET_ADD(qset_supported, bcmFieldQualifyEtherType);
    BCM_FIELD_QSET_ADD(qset_supported, bcmFieldQualifyTtl);
    BCM_FIELD_QSET_ADD(qset_supported, bcmFieldQualifyTos);
    BCM_FIELD_QSET_ADD(qset_supported, bcmFieldQualifyIpProtocol);
    BCM_FIELD_QSET_ADD(qset_supported, bcmFieldQualifyTcpControl);

    BCM_FIELD_QSET_ADD(qset_supported, bcmFieldQualifyVrf);

    if (SOC_IS_TOMAHAWK3(unit)) {
        BCM_FIELD_QSET_ADD(qset_supported, bcmFieldQualifyIpTunnelTtl);
    } else {
        BCM_FIELD_QSET_ADD(qset_supported, bcmFieldQualifyL4SrcPort);
        BCM_FIELD_QSET_ADD(qset_supported, bcmFieldQualifyL4DstPort);
        BCM_FIELD_QSET_ADD(qset_supported, bcmFieldQualifySrcIp);
        BCM_FIELD_QSET_ADD(qset_supported, bcmFieldQualifySrcIp6);
        BCM_FIELD_QSET_ADD(qset_supported, bcmFieldQualifyDstIp);
        BCM_FIELD_QSET_ADD(qset_supported, bcmFieldQualifyDstIp6);
        BCM_FIELD_QSET_ADD(qset_supported, bcmFieldQualifyFcoeOxID);
        BCM_FIELD_QSET_ADD(qset_supported, bcmFieldQualifyFcoeRxID);
        BCM_FIELD_QSET_ADD(qset_supported, bcmFieldQualifyFibreChanSrcId);
        BCM_FIELD_QSET_ADD(qset_supported, bcmFieldQualifyFibreChanDstId);
    }

    /* Validate Provided qset is supported */
    if(!(_field_qset_is_subset(&class_fg->qset, &qset_supported))) {
        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                  "FP(unit %d) Error: Un-Supported Qualifier in qset "
                  "for stage class.\n"),
                  unit));
        return (BCM_E_UNAVAIL);
    }

    /*
     * First Check only one stage is passed either
     * it is StageClass or StageClassExactMatch
     */
    if (BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyStageClass))
        stage_cnt++;
    if (BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyStageClassExactMatch))
        stage_cnt++;

    if (stage_cnt != 1) {
        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                  "FP(unit %d) Error: StageClass Parameter not specified "
                  "or is not valid.\n"),
                  unit));
        return (BCM_E_PARAM);
    }

    /* Check only one class is specified */
    if (BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyEtherType))
        class_cnt++;
    if (BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyTtl))
        class_cnt++;
    if (BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyTos))
        class_cnt++;
    if (BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyIpProtocol))
        class_cnt++;
    if (BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyL4SrcPort) ||
        BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyFcoeOxID))
        class_cnt++;
    if (BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyL4DstPort) ||
        BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyFcoeRxID))
        class_cnt++;
    if (BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyTcpControl))
        class_cnt++;
    if (BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifySrcIp) ||
        BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifySrcIp6) ||
        BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyFibreChanSrcId))
        class_cnt++;
    if (BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyDstIp) ||
        BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyDstIp6) ||
        BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyFibreChanDstId))
        class_cnt++;
    if (BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyIpTunnelTtl))
        class_cnt++;

    if (class_cnt != 1) {
        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                  "FP(unit %d) Error: Un-Supported combination of class "
                  "qualifier.\n"),
                  unit));
        return (BCM_E_PARAM);
    }

    /* Check if Class action provided, if not return error */
    if (!(BCM_FIELD_ASET_TEST(*aset, bcmFieldActionClassZero) ||
                BCM_FIELD_ASET_TEST(*aset, bcmFieldActionClassOne))) {
        LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                     "FP(unit %d) Error: Stage Action Paramter not "
                     "specified.\n"),
                    unit));
        return (BCM_E_PARAM);
    }

    /* ClassOne Action Validation */
    if (BCM_FIELD_ASET_TEST(*aset, bcmFieldActionClassOne)) {
        if (!(BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyTtl) ||
              BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyTos) ||
              BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyTcpControl) ||
              BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyIpTunnelTtl))) {
            LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                      "FP(unit %d) Error: ClassOne Action unsupported for "
                      "provided class qualfier.\n"),
                      unit));
            return (BCM_E_PARAM);
        }
    }

    /* Stage Class Validation */
    if (BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyStageClass)) {
        if (!(BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifySrcIp) ||
              BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifySrcIp6) ||
              BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyFibreChanSrcId) ||
              BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyDstIp) ||
              BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyDstIp6) ||
              BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyFibreChanDstId))) {
            LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                      "FP(unit %d) Error: Qualifier unsupported for given "
                      "stage class.\n"),
                      unit));
            return (BCM_E_PARAM);
        }
    }

    if (BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyStageClassExactMatch)) {
        if (!(BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyEtherType) ||
              BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyTtl) ||
              BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyTos) ||
              BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyIpProtocol) ||
              BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyL4SrcPort) ||
              BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyFcoeOxID) ||
              BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyL4DstPort) ||
              BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyFcoeRxID) ||
              BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyTcpControl) ||
              BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyIpTunnelTtl))) {
            LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                      "FP(unit %d) Error: Qualifier unsupported for given "
                      "stage class exact match.\n"),
                      unit));
            return (BCM_E_PARAM);
        }
    }

    /* VRF Check */
    if (BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyVrf)) {
        if (!(BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifySrcIp) ||
              BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifySrcIp6) ||
              BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyFibreChanSrcId) ||
              BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyDstIp) ||
              BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyDstIp6) ||
              BCM_FIELD_QSET_TEST(*qset, bcmFieldQualifyFibreChanDstId))) {
            LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                      "FP(unit %d) Error: Un-Supported Qualifier set "
                      "for VRF.\n"),
                      unit));
            return (BCM_E_PARAM);
        }
    }

    /* Read device port configuration. */ 
    BCM_PBMP_CLEAR(all_pbmp);
    BCM_IF_ERROR_RETURN(_bcm_field_valid_pbmp_get(unit, &all_pbmp));

    /* Verify pbmp based on stage operational mode */
    if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
        /*
         * Validate user supplied IPBM value.For global mode,
         * all ports must be part of IPBM value, if supplied.
         */
        if (BCM_PBMP_NEQ(all_pbmp, class_fg->pbmp)) {
            /* If port configuration get fails, go to end. */
            LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                      "FP(unit %d) Error: Global Mode - "
                      "Port Bitmap not valid.\n"),
                      unit));
            return (BCM_E_PARAM);
        }

        /* For group global operational mode, use the default instance.  */
        class_fg->instance = _FP_DEF_INST;
    } else {
        /*
         * For PipeLocal group oper mode, derive the
         * instance information from group's PBM value.
         */
        for (pipe = 0; pipe < NUM_PIPE(unit); pipe++){
            BCM_PBMP_ASSIGN(t_pbmp, PBMP_PIPE(unit,pipe));
            BCM_PBMP_REMOVE(t_pbmp, PBMP_LB(unit));
            /*
             * User must use bcm_port_config_get API to retrieve Pipe PBMP value
             * and pass this value in Group's pbmp parameter when creating
             * PipeLocal groups.
             */
            if (BCM_PBMP_EQ(t_pbmp, class_fg->pbmp)) {
                /*
                 * Pipe PBM matches user supplied PBM, update field group
                 * instance.
                 */
                class_fg->instance = pipe;
                break;
            }
        }

        /*
         * Portbit map value supplied by the user not matching any of the pipe's
         * port bitmap mask value, hence return input paramter error.
         */
        if (NUM_PIPE(unit) == pipe) {
            LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                      "FP(unit %d) Error: Per Pipe Mode - "
                      "Port Bitmap not valid.\n"),
                      unit));
            return (BCM_E_PARAM);
        }
    }

    /*
     * Only one class group is valid per compression
     * table. Validate if it exist already in system.
     */
    fg = fc->groups;
    while (fg != NULL) {
        /* Skip non class groups */
        if (fg->stage_id != _BCM_FIELD_STAGE_CLASS) {
            fg = fg->next;
            continue;
        }
        /*
         * Check overlapping qualifier additionally.
         * SrcIp and DstIp compression groups are superset of
         * IPv6 ,V6 and FCoE entries. Hence compare subset
         * with superset qset than actual group qset.
         */
        BCM_FIELD_QSET_INIT(qset_fg);
        qset_fg = class_fg->qset;
        if (BCM_FIELD_QSET_TEST(qset_fg, bcmFieldQualifyL4SrcPort) ||
            BCM_FIELD_QSET_TEST(qset_fg, bcmFieldQualifyFcoeOxID)) {
            BCM_FIELD_QSET_ADD(qset_fg, bcmFieldQualifyL4SrcPort);
            BCM_FIELD_QSET_ADD(qset_fg, bcmFieldQualifyFcoeOxID);
        }
        if (BCM_FIELD_QSET_TEST(qset_fg, bcmFieldQualifyL4DstPort) ||
            BCM_FIELD_QSET_TEST(qset_fg, bcmFieldQualifyFcoeRxID)) {
            BCM_FIELD_QSET_ADD(qset_fg, bcmFieldQualifyL4DstPort);
            BCM_FIELD_QSET_ADD(qset_fg, bcmFieldQualifyFcoeRxID);
        }
        if (BCM_FIELD_QSET_TEST(qset_fg, bcmFieldQualifySrcIp) ||
            BCM_FIELD_QSET_TEST(qset_fg, bcmFieldQualifySrcIp6) ||
            BCM_FIELD_QSET_TEST(qset_fg, bcmFieldQualifyFibreChanSrcId)) {
            BCM_FIELD_QSET_ADD(qset_fg, bcmFieldQualifySrcIp);
            BCM_FIELD_QSET_ADD(qset_fg, bcmFieldQualifySrcIp6);
            BCM_FIELD_QSET_ADD(qset_fg, bcmFieldQualifyFibreChanSrcId);
        }
        if (BCM_FIELD_QSET_TEST(qset_fg, bcmFieldQualifyDstIp) ||
            BCM_FIELD_QSET_TEST(qset_fg, bcmFieldQualifyDstIp6) ||
            BCM_FIELD_QSET_TEST(qset_fg, bcmFieldQualifyFibreChanDstId)) {
            BCM_FIELD_QSET_ADD(qset_fg, bcmFieldQualifyDstIp);
            BCM_FIELD_QSET_ADD(qset_fg, bcmFieldQualifyDstIp6);
            BCM_FIELD_QSET_ADD(qset_fg, bcmFieldQualifyFibreChanDstId);
        }

        if (_field_qset_is_subset(&fg->qset, &qset_fg)) {

            if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
                LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                          "FP(unit %d) Error: Global Mode - "
                          "Field Group Exist Already.\n"),
                          unit));
                return (BCM_E_PARAM);
            } else {
                if (BCM_PBMP_EQ(fg->pbmp, class_fg->pbmp)) {
                    LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                              "FP(unit %d) Error: Per Pipe Mode - "
                              "Field Group Exist Already.\n"),
                              unit));
                    return (BCM_E_PARAM);
                }
            }
        }
        fg = fg->next;
    }

    return (rv);
}

/*
 * Function:
 *  _bcm_field_th_class_size_get
 * Purpose:
 *  Get Field Class size supported for
 *  provide ctype.
 * Parameters:
 *  unit        - (IN) BCM Device number.
 *  ctype       - (IN) Field Class Type.
 *  memsize     - (OUT) Max Class Size.
 * Returns:
 *  BCM_E_XXX
 */
int
_bcm_field_th_class_size_get(int unit, _field_class_type_t ctype,
                                                    int *memsize)
{
    int rv = BCM_E_NONE; /* Operation Return Status */

    /* Validate and fill as per class type */
    switch(ctype) {
        case _FieldClassEtherType:
            *memsize = _FP_SIZE_CLASS_ETHERTYPE;
            break;
        case _FieldClassTtl:
            *memsize = _FP_SIZE_CLASS_TTL;
            break;
        case _FieldClassToS:
            *memsize = _FP_SIZE_CLASS_TOS;
            break;
        case _FieldClassIpProto:
            *memsize = _FP_SIZE_CLASS_IP_PROTO;
            break;
        case _FieldClassL4SrcPort:
            *memsize = _FP_SIZE_CLASS_L4_SRC_PORT;
            break;
        case _FieldClassL4DstPort:
            *memsize = _FP_SIZE_CLASS_L4_DST_PORT;
            break;
        case _FieldClassTcp:
            *memsize = _FP_SIZE_CLASS_TCP;
            break;
        case _FieldClassSrcCompression:
            *memsize = _FP_SIZE_CLASS_SRC_COMPRESSION;
            break;
        case _FieldClassDstCompression:
            *memsize = _FP_SIZE_CLASS_DST_COMPRESSION;
            break;
        case _FieldClassIpTunnelTtl:
            *memsize = _FP_SIZE_CLASS_IPTUNNELTTL;
            break;
        default:
            rv = BCM_E_PARAM;
    }

    return (rv);
}

/*
 * Function:
 *  _bcm_field_th_class_stage_add
 * Purpose:
 *  Add Pseudo Class Stage.
 * Parameters:
 *  unit     - (IN) BCM device number.
 *  fc       - (IN) Field control structure.
 *  stage_fc - (IN) Stage field control structure.
 * Returns:
 *  BCM_E_XXX
 */
int
_bcm_field_th_class_stage_add(int unit, _field_control_t *fc,
                                     _field_stage_t *stage_fc)
{
    int rv = BCM_E_NONE;    /* Operation Return Status */

    /* Input parameters check. */
    if ((fc == NULL) ||
        (stage_fc == NULL)) {
        return (BCM_E_INTERNAL);
    }

    /* Add stage to Field Control structure. */
    FP_LOCK(unit);

    stage_fc->next = fc->stages;
    fc->stages = stage_fc;

    stage_fc->num_pipes = NUM_PIPE(unit);

    if (bcmFieldGroupOperModePipeLocal == stage_fc->oper_mode) {
        /*
         * Initialize number of FP instances to number of pipes supported in
         * hardware.
         */
        stage_fc->num_instances = stage_fc->num_pipes;
    } else {
        /* Initialize to legacy TD2 mode - Global mode (single instance). */
        stage_fc->num_instances = 1;
    }

    /* Initialize Class Stage Tracking Structure. */
    rv = _bcm_field_th_class_stage_status_init(unit,stage_fc);

    FP_UNLOCK(unit);

    return (rv);
}

/*
 * Function:
 *  _bcm_field_th_class_stage_status_init
 * Purpose:
 *  Initialize Class Stage Tracking Structure.
 * Parameters:
 *  unit     - (IN) BCM device number.
 *  stage_fc - (IN) Stage field control structure.
 * Returns:
 *  BCM_E_XXX
 */
int
_bcm_field_th_class_stage_status_init(int unit, _field_stage_t *stage_fc)
{
    int rv = BCM_E_NONE;    /* Operation Return Status */
    _field_class_type_t ctype;
                            /* Field Class Type */
    _field_class_info_t **class_status_arr;
                            /* Field Per Pipe Class Status Array */
    _field_class_info_t *class_status_info = NULL;
                            /* Field Per Compression table status */
    int alloc_sz;           /* Memory allocation size. */
    int cpipe = 0, max_entries = 0,size = 0;
                            /* Misc Var */

    /* Input parameters check */
    if (stage_fc == NULL) {
        return (BCM_E_INTERNAL);
    }

    /* Skip this routine for non-class stages. */
    if (_BCM_FIELD_STAGE_CLASS != stage_fc->stage_id) {
        return (BCM_E_NONE);
    }

    /* Allocate Per-Pipe Class Stage Pointer Arrays */
    for (cpipe = 0; cpipe < _FP_MAX_NUM_PIPES; cpipe++) {
        if (stage_fc->class_info_arr[cpipe] == NULL) {
            /* Allocate memory for stage class status. */
            _FP_XGS3_ALLOC(stage_fc->class_info_arr[cpipe],
                  (_FP_MAX_NUM_CLASS_TYPES * sizeof(_field_class_info_t *)),
                    "Field Class Status");
            if (NULL == stage_fc->class_info_arr[cpipe]) {
                LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                          "FP(unit %d) Error: Allocation failure for field "
                          "class status arr info.\n"),
                          unit));
                return (BCM_E_MEMORY);
            }
        } else {
            LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                      "FP(unit %d) Error: Allocation already present for "
                      "field class status arr info.\n"),
                      unit));
            return (BCM_E_INTERNAL);
        }
    }

    /* Allocate Class Stage Tracking Strucutre for all classes per pipe */
    for (cpipe = 0; cpipe < _FP_MAX_NUM_PIPES; cpipe++) {
        class_status_arr = stage_fc->class_info_arr[cpipe];
        for (ctype = 0; ctype < _FP_MAX_NUM_CLASS_TYPES; ctype++) {
            /* Check the class types supported */
            rv = _bcm_field_th_class_ctype_supported(unit, ctype);
            if (BCM_SUCCESS(rv)) {
                class_status_info = class_status_arr[ctype];
                if (class_status_info == NULL) {
                    /* Allocate field class status info struct. */
                    alloc_sz = sizeof(_field_class_info_t);
                    _FP_XGS3_ALLOC(class_status_info, alloc_sz,
                                                        "FP Class Status info");
                    if (NULL == class_status_info) {
                        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                  "FP(unit %d) Error: Allocation failure for "
                                  "field class status info.\n"),
                                  unit));
                        return (BCM_E_MEMORY);
                    }
                    /* Update Class Status Info */
                    class_status_info->classtype = ctype;
                    class_status_info->total_entries_used = 0;

                    /* Retreive Max Entries for class type */
                    rv = _field_th_class_max_entries(unit,cpipe,
                                                          ctype,&max_entries);

                    if (BCM_SUCCESS(rv)) {
                        class_status_info->total_entries_available = max_entries;
                        size = SHR_BITALLOCSIZE(max_entries << 1);
                        class_status_info->class_bmp.w =
                                                   sal_alloc(size, "class_bmap");
                        sal_memset(class_status_info->class_bmp.w, 0, size);
                    } else {
                        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                  "FP(unit %d) Error: Retreiving max entries for "
                                  "class type %d.\n"),
                                  unit,ctype));
                        sal_free(class_status_info);
                        return (BCM_E_INTERNAL);
                    }
                    class_status_arr[ctype] = class_status_info;

                } else {
                        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                  "FP(unit %d) Error: Allocation already present "
                                  "for field class status info.\n"),
                                  unit));
                        return (BCM_E_MEMORY);
                }
            }
        }
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *  _bcm_field_th_class_stage_status_deinit
 * Purpose:
 *  De-Initialize Class Stage Tracking Structure.
 * Parameters:
 *  unit     - (IN) BCM device number.
 *  stage_fc - (IN) Stage field control structure.
 * Returns:
 *  BCM_E_XXX
 */
int
_bcm_field_th_class_stage_status_deinit(int unit, _field_stage_t *stage_fc)
{
    int rv = BCM_E_NONE;        /* Operation Return Status */
    _field_class_type_t ctype;  /* Field Class Type */
    _field_class_info_t **class_status_arr;
                                /* Field Per Pipe Class Status Array */
    _field_class_info_t *class_status_info;
                                /* Field Per Compression table status */
    int cpipe = 0;              /* Per Pipe Iterator */

    /* Input parameters check. */
    if (stage_fc == NULL) {
        return (BCM_E_INTERNAL);
    }

    /* Skip this routine for non-class stages. */
    if (_BCM_FIELD_STAGE_CLASS != stage_fc->stage_id) {
        return (BCM_E_NONE);
    }

    /* Free Memory Allocated for Class Status Structure */
    for (cpipe = 0; cpipe < _FP_MAX_NUM_PIPES; cpipe++) {
        class_status_arr = stage_fc->class_info_arr[cpipe];
        if (class_status_arr != NULL) {
            for (ctype = 0; ctype < _FP_MAX_NUM_CLASS_TYPES; ctype++) {
                /* Check the class types supported */
                rv = _bcm_field_th_class_ctype_supported(unit, ctype);
                if (BCM_SUCCESS(rv)) {
                    class_status_info = class_status_arr[ctype];
                    if (class_status_info != NULL) {
                        if (class_status_info->class_bmp.w != NULL) {
                            sal_free(class_status_info->class_bmp.w);
                            class_status_info->class_bmp.w = NULL;
                        }
                        sal_free(class_status_info);
                        class_status_info = NULL;
                        class_status_arr[ctype] = class_status_info;
                    }
                }
            }
            sal_free(class_status_arr);
            class_status_arr = NULL;
            stage_fc->class_info_arr[cpipe] = class_status_arr;
        }
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *  _bcm_field_th_class_group_add
 * Purpose:
 *  Create a field processor group for stage class.
 * Parameters:
 *  unit      - (IN) BCM device number.
 *  fsm_ptr   - (IN) State machine tracking structure.
 * Returns:
 *  BCM_E_XXX
 */
int
_bcm_field_th_class_group_add(int unit, _field_group_add_fsm_t *fsm_ptr,
                                             bcm_field_aset_t aset_class)
{
    int rv = BCM_E_NONE;        /* Operation Return Status */
    int mem_sz;                 /* Memory size to be allocated */
    _field_group_t *fg = NULL;  /* New group information structure */
    _field_group_t *fg_temp;    /* Pointer to an existing Field Group */
    _field_class_info_t **class_status_arr;
                                /* Field Per Pipe Class Status Array */
    _field_class_info_t *class_status_info;
                                /* Field Per Compression table status */
    _field_class_type_t ctype;
                                /* Field Class Type */
    int idx;

    /* Input parameters check. */
    if (NULL == fsm_ptr) {
        return (BCM_E_INTERNAL);
    }

    /* Validate Group ID Passed */
    if (BCM_SUCCESS(_field_group_get(unit, fsm_ptr->group_id, &fg_temp))) {
        LOG_ERROR(BSL_LS_BCM_FP,
                 (BSL_META_U(unit, "FP(unit %d) Error: group=%d already "
                 "exists.\n"),
                 unit, fsm_ptr->group_id));
        return (BCM_E_EXISTS);
    }

    /* Get Field Control */
    rv = _field_control_get(unit, &fsm_ptr->fc);
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    /* Get Field Stage Class Control */
    rv = _field_stage_control_get(unit, _BCM_FIELD_STAGE_CLASS,
                                             &fsm_ptr->stage_fc);
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    /* Determine field group structure buffer size. */
    mem_sz = sizeof(_field_group_t);

    /* Allocate and initialize memory for Field Group. */
    _FP_XGS3_ALLOC(fg, mem_sz, "field group");

    if (NULL == fg) {
        LOG_ERROR(BSL_LS_BCM_FP, (BSL_META_U(unit,
                  "FP(unit %d) Error:Allocation failure for "
                  "_field_group_t\n"), unit));
        return (BCM_E_MEMORY);
    }

    /* Initialize group structure. */
    fg->gid            = fsm_ptr->group_id;
    fg->stage_id       = fsm_ptr->stage_fc->stage_id;
    fg->qset           = fsm_ptr->qset;
    fg->pbmp           = fsm_ptr->pbmp;
    fg->priority       = fsm_ptr->priority;
    sal_memcpy(&fg->aset, &aset_class, sizeof(bcm_field_aset_t));

    /* Initialize the action profile index's to -1 */
    for (idx=0; idx < MAX_ACT_PROF_ENTRIES_PER_GROUP; idx++) {
        fg->action_profile_idx[idx] =  -1;
    }

    /*
     * Based on the Stage Group Operational mode,
     * set the Field Group's instance information.
     */
    rv = _field_th_class_group_validate(unit, fg);
    if (BCM_FAILURE(rv)) {
        sal_free(fg);
        return rv;
    }

    /* Initialize group status. */
    bcm_field_group_status_t_init(&fg->group_status);

    /* Retreive Class Type from entry qset */
    rv = _bcm_field_th_class_type_qset_get(unit, &fg->qset, &ctype);
    if (BCM_FAILURE(rv)) {
        sal_free(fg);
        return rv;
    }

    /* Update Class Info Status Structure */
    class_status_arr = fsm_ptr->stage_fc->class_info_arr[fg->instance];
    if (class_status_arr == NULL) {
        sal_free(fg);
        return (BCM_E_INTERNAL);
    }
    class_status_info = class_status_arr[ctype];
    if (class_status_info == NULL) {
        sal_free(fg);
        return (BCM_E_INTERNAL);
    }

    fg->group_status.entry_count =
                            class_status_info->total_entries_used;
    fg->group_status.entries_total =
                            class_status_info->total_entries_available;
    fg->group_status.entries_free = fg->group_status.entries_total -
                                    fg->group_status.entry_count;

    /* By default mark group as active. */
    fg->flags |= _FP_GROUP_LOOKUP_ENABLED;

    /* Set allocated pointer to FSM structure. */
    fsm_ptr->fg = fg;

    rv = _bcm_field_group_linked_list_insert(unit, fsm_ptr);
    if (BCM_FAILURE(rv)) {
        sal_free(fg);
        return (rv);
    }

    return (rv);
}

/*
 * Function:
 *  _field_classtype_name
 * Purpose:
 *  Translate classtype to a text string.
 * Parameters:
 *  class_type stage field class type.
 * Returns:
 *  Text name of indicated class type enum value.
 */
STATIC char *
_field_classtype_name(_field_class_type_t class_type)
{
    static char *stage_text[] = _BCM_FIELD_CLASS_TYPE_STRINGS;

    if (class_type >= _FP_MAX_NUM_CLASS_TYPES) {
        return "??";
    }
    return stage_text[class_type];
}

/*
 * Function:
 *  _bcm_field_th_class_stage_dump
 * Purpose:
 *  Dump field stage class info entries.
 * Parameters:
 *  unit      - (IN) BCM device number.
 *  stage_fc  - (IN) Field Class Stage Structure.
 * Returns:
 */
void
_bcm_field_th_class_stage_dump(int unit, _field_stage_t *stage_fc)
{
    _field_class_type_t ctype;
                        /* Operation Return Status */
    _field_class_info_t **class_status_arr;
                        /* Field Per Pipe Class Status Array */
    _field_class_info_t *class_status_info;
                        /* Field Per Compression table status */
    int cpipe = 0;      /* Class Pipe Iterator */
    int rv = BCM_E_NONE;/* Return Value */

    /* Input parameters check. */
    if ((stage_fc != NULL) &&
        (stage_fc->stage_id == _BCM_FIELD_STAGE_CLASS)) {

        LOG_CLI((BSL_META_U(unit,
                        "ClassPipe  ClassType        Flags  "
                        "Entries Used  Entries Available\n")));
        LOG_CLI((BSL_META_U(unit,
                        "---------  ---------------  -----  "
                        "------------  -----------------\n")));

        if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
            /* Global Mode - Display only default instance */
            class_status_arr = stage_fc->class_info_arr[_FP_DEF_INST];
            if (class_status_arr != NULL) {
                for (ctype = 0; ctype < _FP_MAX_NUM_CLASS_TYPES; ctype++) {
                    /* Check the class types supported */
                    rv = _bcm_field_th_class_ctype_supported(unit, ctype);
                    if (BCM_SUCCESS(rv)) {
                        class_status_info = class_status_arr[ctype];
                        if (class_status_info != NULL) {
                            LOG_CLI((BSL_META_U(unit,
                                     "%9s  %-14s  %#5x  %12d  %17d\n"),
                                     "Default",_field_classtype_name(ctype),
                                     class_status_info->flags,
                                     class_status_info->total_entries_used,
                                     class_status_info->total_entries_available));
                        }
                    }
                }
            }
        } else {
            /* Per Pipe Mode - Display all pipe instances */
            for (cpipe = 0; cpipe < _FP_MAX_NUM_PIPES; cpipe++) {
                class_status_arr = stage_fc->class_info_arr[cpipe];
                if (class_status_arr != NULL) {
                    for (ctype = 0; ctype < _FP_MAX_NUM_CLASS_TYPES; ctype++) {
                        /* Check the class types supported */
                        rv = _bcm_field_th_class_ctype_supported(unit, ctype);
                        if (BCM_SUCCESS(rv)) {
                            class_status_info = class_status_arr[ctype];
                            if (class_status_info != NULL) {
                                LOG_CLI((BSL_META_U(unit,
                                       "%9d  %-14s  %#5x  %12d  %17d\n"),
                                       cpipe,_field_classtype_name(ctype),
                                       class_status_info->flags,
                                       class_status_info->total_entries_used,
                                       class_status_info->total_entries_available));

                            }
                        }
                    }
                }
            }
        }
    }
}

/*
 * Function:
 *  _bcm_field_th_class_group_dump
 * Purpose:
 *  Dump field stage class group.
 * Parameters:
 *  unit      - (IN) BCM device number.
 *  class_fg  - (IN) Field Class Stage Group Structure.
 * Returns:
 *  BCM_E_XXX
 */
int
_bcm_field_th_class_group_dump(int unit, _field_group_t *class_fg)
{
    int rv = BCM_E_NONE;        /* Operation Return Status */
    _field_stage_t *stage_fc = NULL;
                                /* Field Class Stage info */
    int enable = 0;             /* Group Enable */
    int entry_count = 0;        /* Field Entry Count */
    int entry_index;            /* Field Entry Index */
    bcm_field_entry_t *entry_ids = NULL;
                                /* Field Entry ID Array */
    char buf[100];              /* Character Buffer */

    /* Input parameters check. */
    if (class_fg == NULL) {
        return (BCM_E_INTERNAL);
    }

    /* Skip if it is not stage class */
    if (class_fg->stage_id != _BCM_FIELD_STAGE_CLASS) {
        return (BCM_E_NONE);
    }

    /* Retreive Field Stage Control */
    rv = _field_stage_control_get(unit, class_fg->stage_id, &stage_fc);
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    /* Dump Field Class Group Information */
    LOG_CLI((BSL_META_U(unit,
             "GID %10d: gid=0x%x, instance=%d, flags %#x, stage=%d "),
             class_fg->gid, class_fg->gid, class_fg->instance, class_fg->flags,
             class_fg->stage_id));
    bcm_esw_field_group_enable_get(unit, class_fg->gid, &enable);
    if (enable) {
        LOG_CLI((BSL_META_U(unit,"lookup=Enabled, ")));
    } else {
        LOG_CLI((BSL_META_U(unit,"lookup=Disabled, ")));
    }
    LOG_CLI((BSL_META_U(unit,
                    "pbmp={%s},\n"), SOC_PBMP_FMT(class_fg->pbmp, buf)));
    _field_qset_dump("         qset=", class_fg->qset, ",\n");
    _field_aset_dump("         aset=", class_fg->aset, ",\n");
    LOG_CLI((BSL_META_U(unit,"         group_status=")));
    LOG_CLI((BSL_META("{entries_total=%d,"),
                      class_fg->group_status.entries_total));
    LOG_CLI((BSL_META(" entries_free=%d,"),
                      class_fg->group_status.entries_free));
    LOG_CLI((BSL_META(" entries_count=%d}"),
                      class_fg->group_status.entry_count));
    LOG_CLI((BSL_META_U(unit,"\n")));

    /* Print group's entries */
    rv = bcm_esw_field_entry_multi_get(unit, class_fg->gid, 0,
                                            NULL, &entry_count);

    if (BCM_SUCCESS(rv))
    {
        entry_ids = sal_alloc(entry_count * sizeof (bcm_field_entry_t),
                "Entry ID array");

        if (entry_ids == NULL)
        {
            rv = BCM_E_MEMORY;
        }
        else
        {
            rv = bcm_esw_field_entry_multi_get(unit, class_fg->gid,
                                               entry_count,
                                               entry_ids, &entry_count);

            if (BCM_SUCCESS(rv))
            {
                for (entry_index = 0; entry_index < entry_count;
                        ++entry_index)
                {
                    _bcm_esw_field_entry_dump(unit, entry_ids[entry_index], -1);
                }
            }

            sal_free(entry_ids);
        }
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *  _bcm_field_th_class_group_brief_dump
 * Purpose:
 *  Dump field stage class group brief.
 * Parameters:
 *  unit      - (IN) BCM device number.
 *  class_fg  - (IN) Field Class Stage Group Structure.
 * Returns:
 *  BCM_E_XXX
 */
int
_bcm_field_th_class_group_brief_dump(int unit, _field_group_t *class_fg)
{
    int rv = BCM_E_NONE;        /* Operation Return Status */
    _field_stage_t *stage_fc = NULL;
                                /* Field Class Stage info */
    int enable = 0;             /* Group Enable */
    char buf[100];              /* Character Buffer */

    /* Input parameters check. */
    if (class_fg == NULL) {
        return (BCM_E_INTERNAL);
    }

    /* Skip if it is not stage class */
    if (class_fg->stage_id != _BCM_FIELD_STAGE_CLASS) {
        return (BCM_E_NONE);
    }

    /* Retreive Field Stage Control */
    rv = _field_stage_control_get(unit, class_fg->stage_id, &stage_fc);
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    /* Dump Field Class Group Information */
    LOG_CLI((BSL_META_U(unit,
             "GID %10d: gid=0x%x, instance=%d, flags %#x, stage=%d "),
             class_fg->gid,
             class_fg->gid, class_fg->instance, class_fg->flags,
             class_fg->stage_id));
    bcm_esw_field_group_enable_get(unit, class_fg->gid, &enable);
    if (enable) {
        LOG_CLI((BSL_META_U(unit,"lookup=Enabled, ")));
    } else {
        LOG_CLI((BSL_META_U(unit,"lookup=Disabled, ")));
    }

    LOG_CLI((BSL_META_U(unit,
                    "pbmp={%s},\n"), SOC_PBMP_FMT(class_fg->pbmp, buf)));
    _field_qset_dump("         qset=", class_fg->qset, ",\n");
    LOG_CLI((BSL_META_U(unit,"         group_status=")));
    LOG_CLI((BSL_META("{entries_total=%d,"),
             class_fg->group_status.entries_total));
    LOG_CLI((BSL_META(" entries_free=%d,"),
             class_fg->group_status.entries_free));
    LOG_CLI((BSL_META(" entries_count=%d}"),
             class_fg->group_status.entry_count));
    LOG_CLI((BSL_META_U(unit,"\n")));

    return (BCM_E_NONE);
}

/*
 * Function:
 *  _bcm_field_th_class_entry_dump
 * Purpose:
 *  Dump field stage class entry.
 * Parameters:
 *  unit       - (IN) BCM device number.
 *  class_ent  - (IN) Field Class Entry Structure.
 * Returns:
 *  BCM_E_XXX
 */
int
_bcm_field_th_class_entry_dump(int unit, _field_entry_t *class_ent)
{
    int rv = BCM_E_NONE;            /* Operation Return Status */
    _field_group_t *fg = NULL;      /* Field group info */
    _field_class_type_t ctype;      /* Field Class Type */
    _field_action_t *fa = NULL;     /* Field action info */
    uint32 *databuf = 0;            /* Data Buffer */
    int tcam_sz = 0;

    /* Input parameters check. */
    if (class_ent == NULL ||
        class_ent->group == NULL) {
        return (BCM_E_INTERNAL);
    }

    /* Retreive Field Group Info */
    fg = class_ent->group;

    /* Skip if stage is not class */
    if (fg->stage_id != _BCM_FIELD_STAGE_CLASS) {
        return (BCM_E_NONE);
    }

    /* Dump Field Class Group Info */
    LOG_CLI((BSL_META_U(unit,
             "EID 0x%08x: gid=0x%x,\n"),
             class_ent->eid, fg->gid));
    LOG_CLI((BSL_META_U(unit,
             "         prio=%#x, flags=%#x, "),
             class_ent->prio, class_ent->flags));
    if (class_ent->slice_idx != -1) {
        LOG_CLI((BSL_META_U(unit,
             "hw_index=%#x, "),
             class_ent->slice_idx));
    }
    if (!(class_ent->flags & _FP_ENTRY_INSTALLED)) {
        LOG_CLI((BSL_META_U(unit,
                        "Not installed\n")));
    } else {
        LOG_CLI((BSL_META_U(unit,
                        "Installed")));
        if (class_ent->flags & _FP_ENTRY_DIRTY) {
            LOG_CLI((BSL_META_U(unit,
                            ", Dirty\n")));
        }
        else {
            LOG_CLI((BSL_META_U(unit,
                            "\n")));
        }
    }

    /* Retreive Class Type from entry qset */
    rv = _bcm_field_th_class_type_qset_get(unit, &fg->qset, &ctype);
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    /* Get key_size from class type */
    rv = _bcm_field_th_class_size_get(unit, ctype, &tcam_sz);
    if (BCM_FAILURE(rv)) {
        return (rv);
    }
    class_ent->tcam.key_size = tcam_sz;

    /* Allocate memory for class data */
    _FP_XGS3_ALLOC(databuf, class_ent->tcam.key_size, "field class data buf");
    if (databuf == NULL) {;
        LOG_ERROR(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                 "FP(unit %d) Error: allocation failure for "
                 "field class data buf\n"),
                 unit));
        return (BCM_E_MEMORY);
    }

    LOG_CLI((BSL_META_U(unit,
             "         Compression Type: %-14s\n"),
             _field_classtype_name(ctype)));

    if (class_ent->tcam.key != NULL) {
        /* Entry is in tcam data buffer , read it from buffer */
        sal_memcpy(databuf, class_ent->tcam.key, class_ent->tcam.key_size);
    } else {
        /* Entry is in hardware , read it from hardware */
        rv = _bcm_field_th_class_entry_hwread(unit, class_ent,
                                                ctype, databuf);
        if (BCM_FAILURE(rv)) {
            sal_free(databuf);
            return (rv);
        }
    }

    /* Dispaly Entry Key */
    _field_th_class_entry_key_show(unit, class_ent, ctype, databuf);

    /* Show Actions attached to entry */
    LOG_CLI((BSL_META_U(unit,"         action=")));

    fa = class_ent->actions;
    while (fa != NULL) {
        LOG_CLI((BSL_META("{act=%s, class=0x%x%08x}"),
                _field_action_name(fa->action), fa->param[1], fa->param[0]));
        fa = fa->next;
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n")));

    /* Free Entry Buffer */
    sal_free(databuf);

    return (BCM_E_NONE);
}

/*
 * Function:
 *  _field_th_class_entry_key_show.
 * Purpose:
 *  Read Class Entry From Hardware.
 * Parameters:
 *  unit     - (IN) BCM device number.
 *  entry    - (IN) Field Entry Info.
 *  ctype    - (IN) Field Class Type.
 *  cdata    - (IN) Field Class Entry DataBuffer.
 * Returns:
 */
STATIC void
_field_th_class_entry_key_show(int unit, _field_entry_t *class_ent,
                               _field_class_type_t ctype,
                               uint32 *cdata)
{
    soc_reg_t reg;              /* SOC Register */
    soc_mem_t mem;              /* SOC Memory */
    uint32 keytype = 0;         /* Class Entry Type */
    uint32 validbit = 0;        /* Entry Valid Bit  */
    uint32 cval = 0;            /* Class Entry Value */
    uint32 cmask = 0;           /* Class Entry Mask */
    uint32 cip6_field[4];       /* Class IP6 Field */
    uint32 cip6_field_mask[4];  /* Class IP6 Field Mask */
    bcm_ip6_t cip6_addr;        /* Class IP6 Address */
    bcm_ip6_t cip6_addr_mask;   /* Class IP6 Address Mask */
    int i = 0;                  /* Iterator */

    LOG_CLI((BSL_META_U(unit,"         {")));

    /* Show Key Based on Class Type */
    switch(ctype) {
        case _FieldClassEtherType:
            reg = ETHERTYPE_MAPr;
            cval = soc_reg_field_get(unit, reg, *cdata, ETHERTYPEf);
            LOG_CLI((BSL_META_U(unit,
                     "EtherType=0x%x"),
                     cval));
            break;
        case _FieldClassTtl:
            LOG_CLI((BSL_META_U(unit,
                     "TTL=%d"),
                     class_ent->tcam.ip_type));
            break;
        case _FieldClassToS:
            LOG_CLI((BSL_META_U(unit,
                     "TOS=%d"),
                     class_ent->tcam.ip_type));
            break;
        case _FieldClassIpProto:
            LOG_CLI((BSL_META_U(unit,
                     "IP Protocol=0x%x"),
                     class_ent->tcam.ip_type));
            break;
        case _FieldClassL4SrcPort:
            reg = L4_SRC_PORT_MAPr;
            keytype = soc_reg_field_get(unit, reg, *cdata, MATCH_TYPEf);
            cval = soc_reg_field_get(unit, reg, *cdata, L4_SRC_PORTf);
            if (keytype == _FP_MATCH_L4_PORT_TYPE) {
                LOG_CLI((BSL_META_U(unit,
                         "L4 SRC Port=%d"),
                         cval));
            } else if (keytype == _FP_MATCH_EXCHANGE_ID_TYPE) {
                LOG_CLI((BSL_META_U(unit,
                         "FCoE OxID=0x%x"),
                         cval));
            } else {
                LOG_CLI((BSL_META_U(unit,
                         "Unknown")));
            }
            break;
        case _FieldClassL4DstPort:
            reg = L4_DST_PORT_MAPr;
            keytype = soc_reg_field_get(unit, reg, *cdata, MATCH_TYPEf);
            cval = soc_reg_field_get(unit, reg , *cdata, L4_DST_PORTf);
            if (keytype == _FP_MATCH_L4_PORT_TYPE) {
                LOG_CLI((BSL_META_U(unit,
                         "L4 Dst Port=%d"),
                         cval));
            } else if (keytype == _FP_MATCH_EXCHANGE_ID_TYPE) {
                LOG_CLI((BSL_META_U(unit,
                         "FCoE RxID=0x%x"),
                         cval));
            } else {
                LOG_CLI((BSL_META_U(unit,
                         "Unknown")));
            }
            break;
        case _FieldClassTcp:
            LOG_CLI((BSL_META_U(unit,
                     "TCP=%d"),
                     class_ent->tcam.ip_type));
            break;
        case _FieldClassSrcCompression:
            mem = SRC_COMPRESSIONm;
            soc_mem_field_get(unit, mem, cdata, KEY_TYPEf, &keytype);
            soc_mem_field_get(unit, mem, cdata, VALIDf, &validbit);
            if (keytype == _FP_KEY_IPv4_TYPE) {
                soc_mem_field_get(unit, mem, cdata,
                                        IPV4__SIP_V4f, &cval);
                soc_mem_field_get(unit, mem, cdata,
                                        IPV4__SIP_V4_MASKf, &cmask);
                LOG_CLI((BSL_META_U(unit,"Source IPv4=0x%x, "),cval));
                LOG_CLI((BSL_META_U(unit,"IPv4 Mask=0x%x, "),cmask));
            } else if (keytype == _FP_KEY_FCoE_TYPE) {
                soc_mem_field_get(unit, mem, cdata,
                                        FCOE__S_IDf, &cval);
                soc_mem_field_get(unit, mem, cdata,
                                        FCOE__S_ID_MASKf, &cmask);
                LOG_CLI((BSL_META_U(unit,"FCoE S_ID=0x%x, "),cval));
                LOG_CLI((BSL_META_U(unit,"S_ID Mask=0x%x, "),cmask));
            } else if (keytype == _FP_KEY_IPv6_TYPE) {
                soc_mem_field_get(unit, mem, cdata,
                                        IPV6__SIP_V6f, cip6_field);
                soc_mem_field_get(unit, mem, cdata,
                                        IPV6__SIP_V6_MASKf, cip6_field_mask);
                SAL_IP6_ADDR_FROM_UINT32(cip6_addr, cip6_field);
                SAL_IP6_ADDR_FROM_UINT32(cip6_addr_mask, cip6_field_mask);
                LOG_CLI((BSL_META_U(unit,"Source Ipv6= ")));
                LOG_CLI((BSL_META("0x")));
                for (i = 0; i < sizeof(bcm_ip6_t) - 1; i++) {
                    LOG_CLI((BSL_META("%02x:"), cip6_addr[i]));
                }
                LOG_CLI((BSL_META("%02x"), cip6_addr[i]));
                LOG_CLI((BSL_META(", ")));
                LOG_CLI((BSL_META_U(unit,"Ipv6 Mask=")));
                LOG_CLI((BSL_META("0x")));
                for (i = 0; i < sizeof(bcm_ip6_t); i++) {
                    LOG_CLI((BSL_META("%02x:"), cip6_addr_mask[i]));
                }
                LOG_CLI((BSL_META(", ")));
            } else {
                LOG_CLI((BSL_META_U(unit,"Unknown, ")));
            }
            soc_mem_field_get(unit, mem, cdata, VRFf, &cval);
            soc_mem_field_get(unit, mem, cdata, VRF_MASKf, &cmask);

            LOG_CLI((BSL_META_U(unit,"VRF=0x%x, "),cval));
            LOG_CLI((BSL_META_U(unit,"VRF Mask=0x%x, "),cmask));
            LOG_CLI((BSL_META_U(unit,"Valid=%d"),validbit));
            break;
        case _FieldClassDstCompression:
            mem = DST_COMPRESSIONm;
            soc_mem_field_get(unit, mem, cdata, KEY_TYPEf, &keytype);
            soc_mem_field_get(unit, mem, cdata, VALIDf, &validbit);
            if (keytype == _FP_KEY_IPv4_TYPE) {
                soc_mem_field_get(unit, mem, cdata,
                                        IPV4__DIP_V4f, &cval);
                soc_mem_field_get(unit, mem, cdata,
                                        IPV4__DIP_V4_MASKf, &cmask);
                LOG_CLI((BSL_META_U(unit,"Destination IPv4=0x%x, "),cval));
                LOG_CLI((BSL_META_U(unit,"IPv4 Mask=0x%x, "),cmask));
            } else if (keytype == _FP_KEY_FCoE_TYPE) {
                soc_mem_field_get(unit, mem, cdata,
                                        FCOE__D_IDf, &cval);
                soc_mem_field_get(unit, mem, cdata,
                                        FCOE__D_ID_MASKf, &cmask);
                LOG_CLI((BSL_META_U(unit,"FCoE D_ID=0x%x, "),cval));
                LOG_CLI((BSL_META_U(unit,"D_ID Mask=0x%x, "),cmask));
            } else if (keytype == _FP_KEY_IPv6_TYPE) {
                soc_mem_field_get(unit, mem, cdata,
                                        IPV6__DIP_V6f, cip6_field);
                soc_mem_field_get(unit, mem, cdata,
                                        IPV6__DIP_V6_MASKf, cip6_field_mask);
                SAL_IP6_ADDR_FROM_UINT32(cip6_addr, cip6_field);
                SAL_IP6_ADDR_FROM_UINT32(cip6_addr_mask, cip6_field_mask);
                LOG_CLI((BSL_META_U(unit,"Destination Ipv6= ")));
                LOG_CLI((BSL_META("0x")));
                for (i = 0; i < sizeof(bcm_ip6_t) - 1; i++) {
                    LOG_CLI((BSL_META("%02x:"), cip6_addr[i]));
                }
                LOG_CLI((BSL_META("%02x"), cip6_addr[i]));
                LOG_CLI((BSL_META(", ")));
                LOG_CLI((BSL_META_U(unit,"Ipv6 Mask=")));
                LOG_CLI((BSL_META("0x")));
                for (i = 0; i < sizeof(bcm_ip6_t); i++) {
                    LOG_CLI((BSL_META("%02x:"), cip6_addr_mask[i]));
                }
                LOG_CLI((BSL_META(", ")));
            } else {
                LOG_CLI((BSL_META_U(unit,"Unknown, ")));
            }
            soc_mem_field_get(unit, mem, cdata, VRFf, &cval);
            soc_mem_field_get(unit, mem, cdata, VRF_MASKf, &cmask);

            LOG_CLI((BSL_META_U(unit,"VRF=0x%x, "),cval));
            LOG_CLI((BSL_META_U(unit,"VRF Mask=0x%x, "),cmask));
            LOG_CLI((BSL_META_U(unit,"Valid=%d"),validbit));
            break;
#if defined(BCM_TOMAHAWK_SUPPORT)
        case _FieldClassIpTunnelTtl:
            LOG_CLI((BSL_META_U(unit,
                     "TTL=%d"),
                     class_ent->tcam.ip_type));
            break;
#endif
        default:
            LOG_CLI((BSL_META_U(unit,"Unknown")));
    }
    LOG_CLI((BSL_META_U(unit,"}\n")));
}


/*
 * Function:
 *  _bcm_field_th_class_entry_create
 * Purpose:
 *  Create Field Class Entry.
 * Parameters:
 *  unit      - (IN) BCM device number.
 *  group     - (IN) Field Group Id.
 *  entry     - (IN) Field Entry.
 * Returns:
 *  BCM_E_XXX
 */
int
_bcm_field_th_class_entry_create(int unit, bcm_field_group_t group,
                                            bcm_field_entry_t entry)
{
    int rv = BCM_E_NONE;        /* Operation Return Status */
    _field_stage_t *stage_fc = NULL;
                                /* Stage field control info */
    _field_group_t *fg = NULL;
                                /* Field group info */
    _field_entry_t *f_ent = NULL;
                                /* Field entry info */
    int mem_sz = 0;             /* Memory allocation size */
    _field_class_info_t **class_status_arr;
                                /* Field Per Pipe Class Status Array */
    _field_class_info_t *class_status_info;
                                /* Field Per Compression table status */
    _field_class_type_t ctype;
                                /* Field Class Type */
    int tcam_sz = 0;            /* TCAM data size */

    /* Confirm that 'entry' is not already used on unit */
    if (BCM_SUCCESS(_field_entry_get(unit, entry, _FP_ENTRY_PRIMARY, &f_ent))) {
        LOG_ERROR(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                 "FP(unit %d) Error: entry=(%d) already exists.\n"),
                 unit, entry));
        return (BCM_E_EXISTS);
    }

    /* Get group control structure. */
    rv = _field_group_get(unit, group, &fg);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                 "FP(unit %d) Error: group=(%d) group get failure.\n"),
                 unit, group));
        return (rv);
    }

    /* Get group stage control structure. */
    rv = _field_stage_control_get(unit, fg->stage_id, &stage_fc);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                 "FP(unit %d) Error: Stage (%d) control get failure.\n"),
                 unit, fg->stage_id));
        return (rv);
    }

    /* Allocate and zero memory for field entry. */
    mem_sz = sizeof(_field_entry_t);

    _FP_XGS3_ALLOC(f_ent, mem_sz, "field entry");

    if (f_ent == NULL) {
        LOG_ERROR(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                 "FP(unit %d) Error: allocation failure for "
                 "class field_entry.\n"),
                 unit));
        return (BCM_E_MEMORY);
    }

    f_ent->eid = entry;
    f_ent->group = fg;

    /* Retreive Class Type from entry qset */
    rv = _bcm_field_th_class_type_qset_get(unit, &fg->qset, &ctype);
    if (BCM_FAILURE(rv)) {
        sal_free(f_ent);
        return rv;
    }

    /* Update Class Info Status Structure */
    class_status_arr = stage_fc->class_info_arr[fg->instance];
    if (class_status_arr == NULL) {
        sal_free(f_ent);
        return (BCM_E_INTERNAL);
    }
    class_status_info = class_status_arr[ctype];
    if (class_status_info == NULL) {
        sal_free(f_ent);
        return (BCM_E_INTERNAL);
    }

    /* Check if resources are present */
    if ((class_status_info->total_entries_used + 1) >
        (class_status_info->total_entries_available)) {
        sal_free(f_ent);
        LOG_DEBUG(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                 "FP(unit %d) Error: no resource available in "
                 "hardware.\n"),
                 unit));
        return (BCM_E_RESOURCE);
    }

    /* Allocate memory for tcam key and mask */
    rv = _bcm_field_th_class_size_get(unit, ctype, &tcam_sz);
    if (BCM_FAILURE(rv)) {
        sal_free(f_ent);
        return (rv);
    }

    /* Check Tcam Key and Size */
    if ((f_ent->tcam.key != NULL) ||
        (tcam_sz == 0)) {
        sal_free(f_ent);
        return (BCM_E_INTERNAL);
    }

    /* Allocate memory for field class entry */
    if (tcam_sz != 0) {
        _FP_XGS3_ALLOC(f_ent->tcam.key, tcam_sz, "field class entry key");
        if (f_ent->tcam.key == NULL) {
            LOG_ERROR(BSL_LS_BCM_FP,
                     (BSL_META_U(unit,
                     "FP(unit %d) Error: allocation failure for "
                     "class entry key\n"),
                     unit));
            sal_free(f_ent);
            return (BCM_E_MEMORY);
        }
    }

    /* Add field entry in group structure */
    rv = _field_group_entry_add(unit, fg, f_ent);
    if (BCM_FAILURE(rv)) {
        sal_free(f_ent);
        return (rv);
    }

    /* Initialize slice idx */
    f_ent->slice_idx = -1;

    /* Set TCAM Key Size */
    f_ent->tcam.key_size = tcam_sz;

    /* Increment Field Entry Count */
    fg->group_status.entries_free =
                            fg->group_status.entries_total -
                            fg->group_status.entry_count;
    class_status_info->total_entries_used++;

    return (rv);
}

/*
 * Function:
 *  _bcm_field_th_class_entry_destroy
 * Purpose:
 *  Delete Field Class Entry.
 * Parameters:
 *  unit      - (IN) BCM device number.
 *  entryr    - (IN) Field Class Entry.
 * Returns:
 *  BCM_E_XXX
 * Notes:
 *  This api should not delete entry if install flag
 *  is set. This check happens in calling function.
 */
int
_bcm_field_th_class_entry_destroy(int unit, _field_entry_t *entry)
{
    int rv = BCM_E_NONE;    /* Operation Return Status */
    _field_stage_t *stage_fc;
                            /* Stage field control info */
    _field_group_t *fg;
                            /* Field group info */
    _field_entry_t *f_ent;
                            /* Field Entry info */
    _field_class_info_t **class_status_arr;
                            /* Field Per Pipe Class Status Array */
    _field_class_info_t *class_status_info;
                            /* Field Per Compression table status */
    _field_class_type_t ctype;
                            /* Field Class Type */

    /* Input paramter check */
    if ((entry == NULL) ||
        (entry->group == NULL)) {
        return (BCM_E_INTERNAL);
    }

    /* Assign Entry and Group Info */
    f_ent = entry;
    fg = f_ent->group;

    /* Get group stage control structure. */
    rv = _field_stage_control_get(unit, fg->stage_id, &stage_fc);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                 "FP(unit %d) Error: Stage (%d) control get failure.\n"),
                 unit, fg->stage_id));
        return (rv);
    }

    /* Retreive Class Type from entry qset */
    rv = _bcm_field_th_class_type_qset_get(unit, &fg->qset, &ctype);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Remove entry from hw before destroying sw image. */
    if (f_ent->flags & _FP_ENTRY_INSTALLED) {
        BCM_IF_ERROR_RETURN(
           _bcm_field_th_class_entry_remove(unit, entry->eid));
    }

    /* Update Class status info structure */
    class_status_arr = stage_fc->class_info_arr[fg->instance];
    if (class_status_arr == NULL) {
        return (BCM_E_INTERNAL);
    }
    class_status_info = class_status_arr[ctype];
    if (class_status_info == NULL) {
        return (BCM_E_INTERNAL);
    }

    /* Free Entry Data */
    if (f_ent->tcam.key != NULL) {
        sal_free(f_ent->tcam.key);
    }

    /* Delete Entry From Linked List */
    rv = _field_group_entry_delete(unit, fg, f_ent);
    if (BCM_FAILURE(rv)) {
        sal_free(f_ent);
        return (rv);
    }

    /* Free Field Entry Resource */
    sal_free(f_ent);

    /* Decrement Field Entry Used Count */
    fg->group_status.entries_free = fg->group_status.entries_total -
                                    fg->group_status.entry_count;
    class_status_info->total_entries_used--;

    return (rv);
}

/*
 * Function:
 *  _bcm_field_th_class_entry_prio_set
 * Purpose:
 *  Set Priority for Field Class Entry.
 * Parameters:
 *  unit      - (IN) BCM device number.
 *  entry     - (IN) Field Entry.
 *  prio      - (IN) New Field Entry Priority.
 * Returns:
 *  BCM_E_XXX
 */
int
_bcm_field_th_class_entry_prio_set(int unit, _field_entry_t *entry, int prio)
{
    int rv = BCM_E_NONE;        /* Operation Return Status */
    _field_group_t *fg;         /* Field Group Info        */
    _field_class_type_t ctype;  /* Field Class Type        */
    int old_prio = 0;           /* Field Entry Priority    */

    /* Input paramter check */
    if ((entry == NULL) ||
        (entry->group == NULL)) {
        return (BCM_E_INTERNAL);
    }

    /* Assign Group Info */
    fg = entry->group;

    /* Retreive Class Type from entry qset */
    rv = _bcm_field_th_class_type_qset_get(unit, &fg->qset, &ctype);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Modify entry priority. */
    old_prio = entry->prio;
    entry->prio = prio;

    /* Set and Sort entry priority. */
    switch (ctype) {
        case _FieldClassEtherType:
        case _FieldClassTtl:
        case _FieldClassToS:
        case _FieldClassIpProto:
        case _FieldClassL4SrcPort:
        case _FieldClassL4DstPort:
        case _FieldClassTcp:
             rv = BCM_E_UNAVAIL;
             break;
        case _FieldClassIpTunnelTtl:
             if ((soc_feature(unit, soc_feature_th3_style_fp))) {
                 rv = BCM_E_NONE;
             } else {
                 rv = BCM_E_UNAVAIL;
             }
             break;
        case _FieldClassSrcCompression:
        case _FieldClassDstCompression:
             if (entry->slice_idx != -1) {
                 rv = _field_th_class_group_sort(unit, fg, ctype);
                 if (BCM_SUCCESS(rv)) {
                     /* coverity[end_of_scope] */
                     return (rv);
                 }
             }
             break;
        default:
             rv = BCM_E_PARAM;
    }

    /* Restore old value. */
    if (BCM_FAILURE(rv)) {
        entry->prio = old_prio;
    }

    return (rv);
}

/*
 * Function:
 *  _field_th_class_ent_cmp
 * Purpose:
 *  Compare class entries based on priority.
 * Parameters:
 *  b - (IN) first compared field entry.
 *  a - (IN) second compared field entry.
 * Returns:
 *  a<=>b
 */
static INLINE int
_field_th_class_ent_cmp(void *a, void *b)
{
    _field_class_cmp_info_t *first;
                            /* First Class Entry Configuration.   */
    _field_class_cmp_info_t *second;
                            /* Second Class Entry Configuration.  */

    first = (_field_class_cmp_info_t *)a;
    second = (_field_class_cmp_info_t *)b;

    if (first->entry_prio < second->entry_prio) {
        return (1);
    } else if (first->entry_prio > second->entry_prio) {
        return (-1);
    }

    return (0);
}

/*
 * Function:
 *  _field_th_class_valid_set
 * Purpose:
 *  Set valid bit of given entry.
 * Parameters:
 *  unit           - (IN) Unit Number.
 *  entry          - (IN) Field Entry Info.
 *  ctype          - (IN) Field Class Type.
 *  valid_bit      - (IN) Valid Bit.
 * Returns:
 */
STATIC int
_field_th_class_valid_set(int unit,
                          _field_entry_t *entry,
                          _field_class_type_t ctype,
                          uint32 valid_bit)
{
    int rv = BCM_E_NONE;        /* Operation Return Status.           */
    _field_stage_t *stage_fc = NULL;
                                /* Stage field control info.          */
    _field_group_t *fg = NULL;  /* Field Group Info.                  */
    soc_mem_t mem;              /* SOC Memory.                        */
    uint32 cdata[SOC_MAX_MEM_FIELD_WORDS] = {0};
                                /* Entry buffer.                      */

    /* Input paramter check */
    if ((entry == NULL) ||
        (entry->group == NULL)) {
        return (BCM_E_INTERNAL);
    }

    /* Check Input Valid Bit. */
    if (valid_bit > 1) {
        return (BCM_E_PARAM);
    }

#ifdef BCM_TOMAHAWK_SUPPORT
     /* VALIDf in SRC_COMPRESSIONm and DST_COMPRESSIONm
      * is two bit wide on TH2 and TD3.*/
    if (valid_bit == 1
        && (SOC_IS_TOMAHAWK2(unit)
        || soc_feature(unit, soc_feature_td3_style_fp))) {
        valid_bit = 3;
    }
#endif /*BCM_TOMAHAWK2_SUPPORT*/

    /* Assign Entry and Group Info */
    fg = entry->group;

    /* Get group stage control structure. */
    rv = _field_stage_control_get(unit, fg->stage_id, &stage_fc);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                 "FP(unit %d) Error: Stage (%d) control get failure.\n"),
                 unit, fg->stage_id));
        return (rv);
    }

    /* Get Memory based on compression and stage mode. */
    switch(ctype) {
        case _FieldClassSrcCompression:
            if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
                mem = SRC_COMPRESSIONm;
            } else {
                BCM_IF_ERROR_RETURN(_bcm_field_mem_instance_get(unit,
                                                       SRC_COMPRESSIONm,
                                                       fg->instance,
                                                       &mem));
            }
            break;
        case _FieldClassDstCompression:
            if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
                mem = DST_COMPRESSIONm;
            } else {
                BCM_IF_ERROR_RETURN(_bcm_field_mem_instance_get(unit,
                                                       DST_COMPRESSIONm,
                                                       fg->instance,
                                                       &mem));
            }
            break;
        default:
            rv = BCM_E_PARAM;
    }

    /* Check Compression Type. */
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    /* Read Memory */
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                      entry->slice_idx, cdata);
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    /* Set valid bit. */
    soc_mem_field_set(unit, mem, cdata,
                      VALIDf, &valid_bit);

    /* Write Memory. */
    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                       entry->slice_idx, cdata);

    return (rv);
}

/*
 * Function:
 *  _field_th_class_entry_copy
 * Purpose:
 *  Copy Entry from old index to new index.
 * Parameters:
 *  unit           - (IN) Unit Number.
 *  mem            - (IN) SoC Memory Identifier.
 *  old_entry_idx  - (IN) Old Entry Index.
 *  new_entry_idx  - (IN) New Entry Index.
 * Returns:
 */
STATIC int
_field_th_class_entry_copy(int unit,
                           soc_mem_t mem,
                           uint32 old_entry_idx,
                           uint32 new_entry_idx)
{
    uint32 cdata[SOC_MAX_MEM_FIELD_WORDS] = {0}; /* Entry buffer. */
    int rv = BCM_E_NONE;

    /* Read Memory */
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                            old_entry_idx, cdata);
    if (BCM_FAILURE(rv)) {
        return (rv);
    }
    /* Write Memory. */
    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                             new_entry_idx, cdata);
    return (rv);
}

/*
 * Function:
 *  _field_th_class_entry_del
 * Purpose:
 *  Delete entry from provided index.
 * Parameters:
 *  unit      - (IN) Unit Number.
 *  mem       - (IN) SoC Memory Identifier.
 *  entry_idx - (IN) Entry Index.
 * Returns:
 */
STATIC int
_field_th_class_entry_del(int unit,
                          soc_mem_t mem,
                          uint32 entry_idx)
{
    uint32 cdata[SOC_MAX_MEM_FIELD_WORDS] = {0}; /* Entry buffer. */
    int rv = BCM_E_NONE;

    /* Write Memory. */
    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                             entry_idx, cdata);
    return rv;
}

/*
 * Function:
 *  _field_th_class_entry_update
 * Purpose:
 *  Update Class entry old entry idx.
 * Parameters:
 *  unit           - (IN) Unit Number.
 *  class_cmp_info - (IN/OUT) - Class Entry Struct.
 *  valid_entries  - (IN) - Valid entries in array.
 *  old_idx        - (IN) - Old Entry Index.
 *  new_idx        - (IN) - New Entry Index.
 * Returns:
 */
STATIC void
_field_th_class_entry_update(int unit,
                             _field_class_cmp_info_t *class_cmp_info,
                             int valid_entries,
                             uint32 old_idx, uint32 new_idx)
{
    int idx = 0;            /* Index Iterator.   */
    _field_class_cmp_info_t *class_cmp_temp = NULL;
                            /* Class Entry temp. */
    int flag = 0;           /* Flag var.         */

    /* Find entry in array. */
    for(idx = 0; idx < valid_entries; idx++) {
        class_cmp_temp = class_cmp_info + idx;
        if (class_cmp_temp->cur_entry_idx == old_idx) {
                flag = 1;
                break;
        }
    }

    /* Update idx if entry is found. */
    if(flag) {
        class_cmp_temp->cur_entry_idx = new_idx;
    }
}

/*
 * Function:
 *  _field_th_class_entry_shift
 * Purpose:
 *  Shift Class entry old slice idx.
 * Parameters:
 *  unit           - (IN) Unit Number.
 *  mem            - (IN) SoC Memory Identifier.
 *  class_cmp_info - (IN/OUT) - Class Entry Struct.
 *  valid_entries  - (IN) - Valid entries in array.
 *  old_idx        - (IN) - Old Entry Index.
 *  new_idx        - (IN) - New Entry Index.
 *  Returns:
 *  Notes:
 *    This function shifts entry in hardware from new_idx
 *    to free_idx and shift all entries down by 1.
 */
STATIC int
_field_th_class_entry_shift(int unit,
                            soc_mem_t mem,
                           _field_class_cmp_info_t *class_cmp_info,
                           _field_class_info_t *class_status_info,
                           int valid_entries,
                           uint32 new_idx, uint32 free_idx)
{
    uint32 idx = 0;            /* Index Iterator.            */
    uint32 ent_num = 0;        /* No. of entries to shift.   */
    uint32 cur_shift_idx = 0;  /* Current entry Shift Index. */
    uint32 cur_free_idx = 0;   /* Current free Index.        */
    int rv = BCM_E_NONE;       /* Return Status */

    /* Initialise Variables. */
    ent_num = free_idx - new_idx;
    cur_free_idx = free_idx;

    /* Shift Entries. */
    if ((class_cmp_info != NULL) &&
        (class_status_info != NULL) &&
        (ent_num > 0)) {

        for(idx = 0; idx < ent_num; idx++) {

            cur_shift_idx = free_idx - idx - 1;

            if (_FP_CLASS_TABLE_BMP_TEST(class_status_info->class_bmp,
                                                        cur_shift_idx)) {

                /* Copy entry from new index to free index. */
                rv = _field_th_class_entry_copy(unit, mem,
                                           cur_shift_idx, cur_free_idx);
                BCM_IF_ERROR_RETURN(rv);

                _field_th_class_entry_update(unit,
                                             class_cmp_info,
                                             valid_entries,
                                             cur_shift_idx, cur_free_idx);
                _FP_CLASS_TABLE_BMP_ADD(class_status_info->class_bmp,
                                                            cur_free_idx);
                _FP_CLASS_TABLE_BMP_REMOVE(class_status_info->class_bmp,
                                                            cur_shift_idx);
            }
            cur_free_idx--;
        }
    }
    return rv;
}

/*
 * Function:
 *  _field_th_class_group_sort
 * Purpose:
 *  Sort Provided Class Field Group Entries based on priority.
 * Parameters:
 *  unit  - (IN) Unit.
 *  fg    - (IN/OUT) Field Group Structure.
 *  ctype - (IN) Compression Type.
 * Returns:
 *  BCM_E_XXX
 * Notes:
 *  This api will sort Src/Dst Compression entries
 *  based on priority. It can sort entries only
 *  if there is one free index available in hardware
 *  otherwide it will return error. Also it will do hitless
 *  sort which means order of entry prio does not
 *  change during sorting.
 */
STATIC int
_field_th_class_group_sort(int unit,
                           _field_group_t *fg,
                           _field_class_type_t ctype)
{
    int rv = BCM_E_NONE;        /* Operation Return Status.           */
    _field_stage_t *stage_fc = NULL;
                                /* Stage field control info.          */
    _field_class_info_t **class_status_arr;
                                /* Field Per Pipe Class Status Array. */
    _field_class_info_t *class_status_info;
                                /* Field Per Compression table status.*/
    _field_entry_t *f_ent = NULL;
                                /* Field Entry Structure.             */
    _field_class_cmp_info_t *class_cmp_info = NULL;
                                /* Class Entry Compare Info Struct.   */
    _field_class_cmp_info_t *class_cmp_temp = NULL;
                                /* Class Entry Compare Info Temp.     */
    int idx, valid_entries = 0; /* Index and Valid Entries var.       */
    int entry_count = 0;        /* Entry Count in Software.           */
    int e_idx = 0;              /* Entry Iteration Index.             */
    bcm_field_entry_t *entry_ids = NULL;
                                /* Entry Id. Array.                   */
                                /* SOC Per Pipe Memory.               */
    soc_mem_t mem;              /* SOC Memory.                        */
    int free_idx = 0;           /* Free Index in Bitmap..             */

    /* Input paramter check */
    if (fg == NULL) {
        return (BCM_E_INTERNAL);
    }

    /* Get group stage control structure. */
    rv = _field_stage_control_get(unit, fg->stage_id, &stage_fc);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                 "FP(unit %d) Error: Stage (%d) control get failure.\n"),
                 unit, fg->stage_id));
        return (rv);
    }

    /* Update Class Info Status Structure */
    class_status_arr = stage_fc->class_info_arr[fg->instance];
    if (class_status_arr == NULL) {
        return (BCM_E_INTERNAL);
    }
    class_status_info = class_status_arr[ctype];
    if (class_status_info == NULL) {
        return (BCM_E_INTERNAL);
    }

    /* Check if free entry available in hardware. */
    if(class_status_info->total_entries_used ==
       class_status_info->total_entries_available) {
        return (BCM_E_PARAM);
    }

    /* Get Memory based on compression and stage mode. */
    switch(ctype) {
        case _FieldClassSrcCompression:
            if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
                mem = SRC_COMPRESSIONm;
            } else {
                BCM_IF_ERROR_RETURN(_bcm_field_mem_instance_get(unit,
                                                      SRC_COMPRESSIONm,
                                                      fg->instance,
                                                      &mem));
            }
            break;
        case _FieldClassDstCompression:
            if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
                mem = DST_COMPRESSIONm;
            } else {
                BCM_IF_ERROR_RETURN(_bcm_field_mem_instance_get(unit,
                                                      DST_COMPRESSIONm,
                                                      fg->instance,
                                                      &mem));
            }
            break;
        default:
            rv = BCM_E_PARAM;
    }

    /* Check Compression Type. */
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    /* Count number of valid entries present in hardware. */
    for (idx = 0; idx < class_status_info->total_entries_available; idx++) {
        if (_FP_CLASS_TABLE_BMP_TEST(class_status_info->class_bmp, idx)) {
            valid_entries++;
        }
    }

    /* No valid entry found in Hardware. */
    if (!valid_entries) {
        return (BCM_E_NONE);
    }

    /* Allocate Memory. */
    _FP_XGS3_ALLOC(class_cmp_info,
                  (valid_entries * sizeof(_field_class_cmp_info_t)),
                  "Field Class Status");
    if (class_cmp_info == NULL) {
        return (BCM_E_MEMORY);
    }

    /* Get group's entries */
    entry_count = fg->group_status.entry_count;
    _FP_XGS3_ALLOC(entry_ids,
                  (entry_count * sizeof(bcm_field_entry_t)),
                  "Entry ID array");
    if (entry_ids == NULL) {
        sal_free(class_cmp_info);
        return (BCM_E_MEMORY);
    }

    rv = bcm_esw_field_entry_multi_get(unit, fg->gid, entry_count,
                                            entry_ids, &entry_count);

    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Copy entry info into class cmp struct. */
    class_cmp_temp = class_cmp_info;
    for (e_idx = 0; e_idx < entry_count; ++e_idx)
    {
        rv = _bcm_field_entry_get_by_id(unit, entry_ids[e_idx], &f_ent);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
        /* Copy only if entry is installed. */
        if (f_ent->slice_idx != -1) {
            class_cmp_temp->f_ent = f_ent;
            class_cmp_temp->cur_entry_idx = f_ent->slice_idx;
            class_cmp_temp->entry_prio  = f_ent->prio;
            class_cmp_temp++;
        }
    }

    /* Sort entries based on priority. */
    _shr_sort(class_cmp_info, valid_entries, sizeof(_field_class_cmp_info_t),
                                                     _field_th_class_ent_cmp);

    /* Move entries in Hardware. */
    for (e_idx = 0; e_idx< valid_entries; e_idx++) {

        /* Update new slice idx. */
        class_cmp_temp = class_cmp_info + e_idx;
        class_cmp_temp->new_entry_idx = e_idx;
        class_cmp_temp->f_ent->slice_idx = class_cmp_temp->new_entry_idx;

        /* Check if new index is free. If yes then move entry from
         * old index to new and mark old as free. */

        if (!(_FP_CLASS_TABLE_BMP_TEST(class_status_info->class_bmp, e_idx))) {

            /* Copy entry from old index to new index. */
            rv = _field_th_class_entry_copy(unit, mem,
                                   class_cmp_temp->cur_entry_idx, e_idx);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }

            _FP_CLASS_TABLE_BMP_ADD(class_status_info->class_bmp, e_idx);

            /* Delete entry from old index. */
            rv = _field_th_class_entry_del(unit, mem,
                                      class_cmp_temp->cur_entry_idx);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }

            _FP_CLASS_TABLE_BMP_REMOVE(class_status_info->class_bmp,
                                       class_cmp_temp->cur_entry_idx);
            class_cmp_temp->cur_entry_idx = e_idx;
        } else {

            /* New index is not free, do something only if
             * new index is different from old index. */
            if (class_cmp_temp->cur_entry_idx != e_idx) {

                /* Find free index, it has to be greater than new index. */
                for (idx = 0; idx < class_status_info->total_entries_available;
                                                                        idx++) {
                    if (0 == _FP_CLASS_TABLE_BMP_TEST(
                                           class_status_info->class_bmp, idx)) {
                        free_idx = idx;
                        break;
                    }
                }
                if (free_idx > e_idx) {

                    /* Shift entries down starting from e_idx to free_idx and
                     * mark e_idx as free. */
                    rv = _field_th_class_entry_shift(unit, mem,
                                                class_cmp_info,
                                                class_status_info,
                                                valid_entries,
                                                e_idx, free_idx);
                    if (BCM_FAILURE(rv)) {
                        goto cleanup;
                    }
                    /* Now copy entry to new idx. */
                    rv = _field_th_class_entry_copy(unit, mem,
                                               class_cmp_temp->cur_entry_idx,
                                               e_idx);
                    if (BCM_FAILURE(rv)) {
                        goto cleanup;
                    }

                    _FP_CLASS_TABLE_BMP_ADD(class_status_info->class_bmp, e_idx);

                    /* Delete entry from old index. */
                    rv = _field_th_class_entry_del(unit, mem,
                                              class_cmp_temp->cur_entry_idx);
                    if (BCM_FAILURE(rv)) {
                        goto cleanup;
                    }

                    _FP_CLASS_TABLE_BMP_REMOVE(class_status_info->class_bmp,
                                               class_cmp_temp->cur_entry_idx);
                    class_cmp_temp->cur_entry_idx = e_idx;
                }
            }
        }
    }

cleanup:

    /* Free Resources. */
    sal_free(entry_ids);
    sal_free(class_cmp_info);

    return (rv);
}

/*
 * Function:
 *  _bcm_field_th_class_entry_prio_get
 * Purpose:
 *  Get Priority for Field Class Entry.
 * Parameters:
 *  unit      - (IN) BCM device number.
 *  entry     - (IN) Field Entry.
 * Returns:
 *  BCM_E_XXX
 */
int
_bcm_field_th_class_entry_prio_get(int unit, _field_entry_t *entry)
{
    int rv = BCM_E_NONE;        /* Operation Return Status */
    _field_group_t *fg;         /* Field Group Info        */
    _field_class_type_t ctype;  /* Field Class Type        */

    /* Input paramter check */
    if ((entry == NULL) ||
        (entry->group == NULL)) {
        return (BCM_E_INTERNAL);
    }

    /* Assign Entry and Group Info */
    fg = entry->group;

    /* Retreive Class Type from entry qset */
    rv = _bcm_field_th_class_type_qset_get(unit, &fg->qset, &ctype);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Set and Sort entry priority. */
    switch (ctype) {
        case _FieldClassEtherType:
        case _FieldClassTtl:
        case _FieldClassToS:
        case _FieldClassIpProto:
        case _FieldClassL4SrcPort:
        case _FieldClassL4DstPort:
        case _FieldClassTcp:
             rv = BCM_E_UNAVAIL;
             break;
        case _FieldClassIpTunnelTtl:
             if ((soc_feature(unit, soc_feature_th3_style_fp))) {
                 rv = BCM_E_NONE;
             } else {
                 rv = BCM_E_UNAVAIL;
             }
             break;
        case _FieldClassSrcCompression:
        case _FieldClassDstCompression:
             rv = BCM_E_NONE;
             break;
        default:
             rv = BCM_E_PARAM;
    }

    return (rv);
}

/*
 * Function:
 *  _bcm_field_th_class_qualify_set
 * Purpose:
 *  Qualify Field Class Entry.
 * Parameters:
 *  unit      - (IN) BCM device number.
 *  entry     - (IN) Field Class Entry Id.
 *  qual      - (IN) Field Qualifier.
 *  data      - (IN) Qualify Data Value.
 *  mask      - (IN) Qualify Data Mask.
 * Returns:
 *  BCM_E_XXX
 */
int
_bcm_field_th_class_qualify_set(int unit, bcm_field_entry_t entry,
                                    int qual, uint32 *data, uint32 *mask)
{
    int rv = BCM_E_NONE;        /* Operation Return Status */
    uint32 *cdata = NULL;       /* Field Class Data */
    bcm_field_qset_t cqset;     /* Class Qset */
    _field_class_type_t ctype;  /* Field Class Type */
    soc_reg_t reg;              /* SOC Register */
    soc_mem_t mem;              /* SOC Memory */
    _field_entry_t *class_ent = NULL;
                                /* Field Class Entry */
    uint32 keytype = 0;         /* Field Class Key Type */
    uint32 keymask = 0;         /* Field Class Key Type Mask */
    uint32 validen = 0;         /* Valid Enable */

    /* Retrieve the field entry. */
    rv = _field_entry_get(unit, entry, _FP_ENTRY_PRIMARY, &class_ent);
    BCM_IF_ERROR_RETURN(rv);

    /* Initialize Class Qset */
    BCM_FIELD_QSET_INIT(cqset);

    /* Input Parameter Check */
    if (class_ent->group == NULL) {
        return (BCM_E_INTERNAL);
    }

    /* Input Group Stage Check */
    if (class_ent->group->stage_id !=
                _BCM_FIELD_STAGE_CLASS) {
        return (BCM_E_INTERNAL);
    }

    /* Add qual in class qset */
    BCM_FIELD_QSET_ADD(cqset, qual);

    /*
     * Check if qual is present in entry group qset
     */
    if(!(_field_qset_is_subset(&cqset, &class_ent->group->qset))) {
        LOG_DEBUG(BSL_LS_BCM_FP,
                (BSL_META_U(unit,
                 "FP(unit %d) Error: Qual=%s not in group=%d "
                 "qset.\n"),
                 unit, _field_qual_name(qual),class_ent->group->gid));
        return (BCM_E_PARAM);
    }

    /* Retreive Class Type from entry qset */
    rv = _bcm_field_th_class_type_qset_get(unit,
                                           &class_ent->group->qset,
                                           &ctype);
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    /* Assign class data buffer */
    if (class_ent->tcam.key == NULL) {
        if ((class_ent->flags & _FP_ENTRY_INSTALLED) &&
            (class_ent->slice_idx != -1)) {
            /* Allocate memory for class data */
            _FP_XGS3_ALLOC(class_ent->tcam.key,
                           class_ent->tcam.key_size, "field class data buf");
            if (class_ent->tcam.key == NULL) {
                LOG_ERROR(BSL_LS_BCM_FP,
                        (BSL_META_U(unit,
                         "FP(unit %d) Error: allocation failure for "
                         "field class data buf\n"),
                         unit));
                return (BCM_E_MEMORY);
            }
            /* Entry is in hardware , read it from hardware */
            rv = _bcm_field_th_class_entry_hwread(unit, class_ent,
                                                  ctype, class_ent->tcam.key);
            if (BCM_FAILURE(rv)) {
                sal_free(class_ent->tcam.key);
                return (rv);
            }
        } else {
            return (BCM_E_INTERNAL);
        }
    }

    cdata = class_ent->tcam.key;

    switch(ctype) {
        case _FieldClassEtherType:
            reg = ETHERTYPE_MAPr;
            soc_reg_field_set(unit, reg, cdata,
                                    ETHERTYPEf, *data);
            break;
        case _FieldClassTtl:
            class_ent->tcam.ip_type = *data;
            break;
        case _FieldClassToS:
            class_ent->tcam.ip_type = *data;
            break;
        case _FieldClassIpProto:
            class_ent->tcam.ip_type = *data;
            break;
        case _FieldClassL4SrcPort:
            /*
             * Validate L4 Src Port,should not be
             * more than 16 bits
             */
            if (*data & ~(0xFFFF)) {
                return (BCM_E_PARAM);
            }
            reg = L4_SRC_PORT_MAPr;
            if (qual == bcmFieldQualifyL4SrcPort) {
                keytype = _FP_MATCH_L4_PORT_TYPE;
                soc_reg_field_set(unit, reg, cdata,
                                        MATCH_TYPEf, keytype);
                soc_reg_field_set(unit, reg, cdata,
                                        L4_SRC_PORTf, *data);
            } else if (qual == bcmFieldQualifyFcoeOxID) {
                keytype = _FP_MATCH_EXCHANGE_ID_TYPE;
                soc_reg_field_set(unit, reg, cdata,
                                        MATCH_TYPEf, keytype);
                soc_reg_field_set(unit, reg, cdata,
                                        L4_SRC_PORTf, *data);
            } else {
                rv = BCM_E_PARAM;
            }
            break;
        case _FieldClassL4DstPort:
            /*
             * Validate L4 Dst Port,should not be
             * more than 16 bits
             */
            if (*data & ~(0xFFFF)) {
                return (BCM_E_PARAM);
            }
            reg = L4_DST_PORT_MAPr;
            if (qual == bcmFieldQualifyL4DstPort) {
                keytype = _FP_MATCH_L4_PORT_TYPE;
                soc_reg_field_set(unit, reg, cdata,
                                        MATCH_TYPEf, keytype);
                soc_reg_field_set(unit, reg, cdata,
                                        L4_DST_PORTf, *data);
            } else if (qual == bcmFieldQualifyFcoeRxID) {
                keytype = _FP_MATCH_EXCHANGE_ID_TYPE;
                soc_reg_field_set(unit, reg, cdata,
                                        MATCH_TYPEf, keytype);
                soc_reg_field_set(unit, reg, cdata,
                                        L4_DST_PORTf, *data);
            } else {
                rv = BCM_E_PARAM;
            }
            break;
        case _FieldClassTcp:
            class_ent->tcam.ip_type = *data;
            break;
        case _FieldClassSrcCompression:
            mem = SRC_COMPRESSIONm;
            validen = 0;
            if (qual == bcmFieldQualifySrcIp) {

                keytype = _FP_KEY_IPv4_TYPE;
                keymask = _FP_KEY_TYPE_MASK;

                soc_mem_field_set(unit, mem, cdata,
                                             KEY_TYPEf, &keytype);
                soc_mem_field_set(unit, mem, cdata,
                                             KEY_TYPE_MASKf, &keymask);
                soc_mem_field_set(unit, mem, cdata,
                                             VALIDf, &validen);
                soc_mem_field_set(unit, mem, cdata,
                                             IPV4__SIP_V4f, data);
                soc_mem_field_set(unit, mem, cdata,
                                             IPV4__SIP_V4_MASKf, mask);

            } else if (qual == bcmFieldQualifyFibreChanSrcId) {
                /*
                 * Validate Fibre Chan Src ID,
                 * should not be more than 24 bits.
                 */
                if ((*data & ~(0xFFFFFF)) ||
                    (*mask & ~(0xFFFFFF))) {
                    return (BCM_E_PARAM);
                }
                keytype = _FP_KEY_FCoE_TYPE;
                keymask = _FP_KEY_TYPE_MASK;

                soc_mem_field_set(unit, mem, cdata,
                                             KEY_TYPEf, &keytype);
                soc_mem_field_set(unit, mem, cdata,
                                             KEY_TYPE_MASKf, &keymask);
                soc_mem_field_set(unit, mem, cdata,
                                             VALIDf, &validen);
                soc_mem_field_set(unit, mem, cdata,
                                             FCOE__S_IDf, data);
                soc_mem_field_set(unit, mem, cdata,
                                             FCOE__S_ID_MASKf, mask);

            } else if (qual == bcmFieldQualifySrcIp6) {

                keytype = _FP_KEY_IPv6_TYPE;
                keymask = _FP_KEY_TYPE_MASK;

                soc_mem_field_set(unit, mem, cdata,
                                             KEY_TYPEf, &keytype);
                soc_mem_field_set(unit, mem, cdata,
                                             KEY_TYPE_MASKf, &keymask);
                soc_mem_field_set(unit, mem, cdata,
                                             VALIDf, &validen);
                soc_mem_field_set(unit, mem, cdata,
                                             IPV6__SIP_V6f, data);
                soc_mem_field_set(unit, mem, cdata,
                                             IPV6__SIP_V6_MASKf, mask);

            } else if (qual == bcmFieldQualifyVrf) {
                /*
                 * Validate VRF Id,
                 * should not be more than 11 bits for TH/TH2.
                 * should not be more than 12 bits for TD3.
                 */
                if (SOC_IS_TOMAHAWKX(unit)
                    && ((*data & ~(0x7FF)) || (*mask & ~(0x7FF)))) {
                    return (BCM_E_PARAM);
                }
#if defined(BCM_TRIDENT3_SUPPORT)
                else {
                    if (soc_feature(unit, soc_feature_td3_style_fp)
                        && ((*data & ~(0xFFF)) || (*mask & ~(0xFFF)))) {
                        return (BCM_E_PARAM);
                    }
                }
#endif
                soc_mem_field_set(unit, mem, cdata,
                                             VRFf, data);
                soc_mem_field_set(unit, mem, cdata,
                                             VRF_MASKf, mask);
            } else {
                rv = BCM_E_PARAM;
            }
            break;
        case _FieldClassDstCompression:
            mem = DST_COMPRESSIONm;
            validen = 0;

            if (qual == bcmFieldQualifyDstIp) {

                keytype = _FP_KEY_IPv4_TYPE;
                keymask = _FP_KEY_TYPE_MASK;

                soc_mem_field_set(unit, mem, cdata,
                                             KEY_TYPEf, &keytype);
                soc_mem_field_set(unit, mem, cdata,
                                             KEY_TYPE_MASKf, &keymask);
                soc_mem_field_set(unit, mem, cdata,
                                             VALIDf, &validen);
                soc_mem_field_set(unit, mem, cdata,
                                             IPV4__DIP_V4f, data);
                soc_mem_field_set(unit, mem, cdata,
                                             IPV4__DIP_V4_MASKf, mask);

            } else if (qual == bcmFieldQualifyFibreChanDstId) {
                /*
                 * Validate Fibre Chan Dst ID,
                 * should not be more than 24 bits.
                 */
                if ((*data & ~(0xFFFFFF)) ||
                    (*mask & ~(0xFFFFFF))) {
                    return (BCM_E_PARAM);
                }

                keytype  = _FP_KEY_FCoE_TYPE;
                keymask = _FP_KEY_TYPE_MASK;

                soc_mem_field_set(unit, mem, cdata,
                                             KEY_TYPEf, &keytype);
                soc_mem_field_set(unit, mem, cdata,
                                             KEY_TYPE_MASKf, &keymask);
                soc_mem_field_set(unit, mem, cdata,
                                             VALIDf, &validen);
                soc_mem_field_set(unit, mem, cdata,
                                             FCOE__D_IDf, data);
                soc_mem_field_set(unit, mem, cdata,
                                             FCOE__D_ID_MASKf, mask);

            } else if (qual == bcmFieldQualifyDstIp6) {

                keytype = _FP_KEY_IPv6_TYPE;
                keymask = _FP_KEY_TYPE_MASK;

                soc_mem_field_set(unit, mem, cdata,
                                             KEY_TYPEf, &keytype);
                soc_mem_field_set(unit, mem, cdata,
                                             KEY_TYPE_MASKf, &keymask);
                soc_mem_field_set(unit, mem, cdata,
                                             VALIDf, &validen);
                soc_mem_field_set(unit, mem, cdata,
                                             IPV6__DIP_V6f, data);
                soc_mem_field_set(unit, mem, cdata,
                                             IPV6__DIP_V6_MASKf, mask);

            } else if (qual == bcmFieldQualifyVrf) {
                /*
                 * Validate VRF Id,
                 * should not be more than 11 bits for TH/TH2.
                 * should not be more than 12 bits for TD3.
                 */
                if (SOC_IS_TOMAHAWKX(unit)
                    && ((*data & ~(0x7FF)) || (*mask & ~(0x7FF)))) {
                    return (BCM_E_PARAM);
                }
#if defined(BCM_TRIDENT3_SUPPORT)
                else {
                    if (soc_feature(unit, soc_feature_td3_style_fp)
                        && ((*data & ~(0xFFF)) || (*mask & ~(0xFFF)))) {
                        return (BCM_E_PARAM);
                    }
                }
#endif
                soc_mem_field_set(unit, mem, cdata,
                                             VRFf, data);
                soc_mem_field_set(unit, mem, cdata,
                                             VRF_MASKf, mask);
            } else {
                rv = BCM_E_PARAM;
            }
            break;
        case _FieldClassIpTunnelTtl:
            class_ent->tcam.ip_type = *data;
            break;
        default:
            rv = BCM_E_PARAM;
    }

    /* Mark entry as dirty. */
    class_ent->flags |= _FP_ENTRY_DIRTY;

    return (rv);
}

/*
 * Function:
 *  _bcm_field_th_class_entry_qualifier_key_get
 * Purpose:
 *  Get Qualify Field Class Entry.
 * Parameters:
 *  unit      - (IN) BCM device number.
 *  entry     - (IN) Field Class Entry ID.
 *  qual_id   - (IN) Field Qualifier ID.
 *  q_data    - (OUT) Qualifier Data Array
 *  q_mask    - (OUT) Qualifier Mask Array.
 * Returns:
 *  BCM_E_XXX
 */
int
_bcm_field_th_class_entry_qualifier_key_get(int unit,
                                            bcm_field_entry_t entry,
                                            int qual_id,
                                            _bcm_field_qual_data_t q_data,
                                            _bcm_field_qual_data_t q_mask)
{
    int rv = BCM_E_NONE;                /* Operation Return Status */
    _field_group_t *fg = NULL;          /* Field group info */
    _field_entry_t *class_ent = NULL;   /* Field entry info */
    _field_class_type_t ctype;          /* Field Class Type */
    uint32 *databuf = 0;                /* Data Buffer */
    soc_reg_t reg;                      /* SOC Register */
    soc_mem_t mem;                      /* SOC Memory */
    uint32 keytype = 0;                 /* Class Entry Type */
    bcm_field_qset_t cqset;             /* Class Qset */

    /* Confirm that 'entry' is  present on unit */
    if (BCM_FAILURE(_field_entry_get(unit, entry, _FP_ENTRY_PRIMARY,
                                                        &class_ent))) {
        LOG_ERROR(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                 "FP(unit %d) Error: entry=(%d) does not exists.\n"),
                 unit, entry));
        return (BCM_E_BADID);
    }

    /* Retreive Field Group Info */
    fg = class_ent->group;

    /* Skip if stage is not class */
    if (fg->stage_id != _BCM_FIELD_STAGE_CLASS) {
        return (BCM_E_NONE);
    }

    /* Initialize Class Qset */
    BCM_FIELD_QSET_INIT(cqset);

    /* Add qual in class qset */
    BCM_FIELD_QSET_ADD(cqset, qual_id);

    /*
     * Check if qual is present in entry group
     * qset.
     */
    if(!(_field_qset_is_subset(&cqset, &class_ent->group->qset))) {
        LOG_DEBUG(BSL_LS_BCM_FP,
                (BSL_META_U(unit,
                    "FP(unit %d) Error: Qual=%s not in group=%d "
                    "qset.\n"),
                 unit, _field_qual_name(qual_id),class_ent->group->gid));
        return (BCM_E_NOT_FOUND);
    }

    /* Retreive Class Type from entry qset */
    rv = _bcm_field_th_class_type_qset_get(unit, &fg->qset, &ctype);
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    /* Allocate memory for class data */
    _FP_XGS3_ALLOC(databuf, class_ent->tcam.key_size, "field class data buf");
    if (databuf == NULL) {;
        LOG_ERROR(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                 "FP(unit %d) Error: allocation failure for "
                 "field class data buf\n"),
                 unit));
        return (BCM_E_MEMORY);
    }

    if (class_ent->tcam.key != NULL) {
        /* Entry is in tcam data buffer , read it from buffer */
        sal_memcpy(databuf, class_ent->tcam.key, class_ent->tcam.key_size);
    } else {
        /* Entry is in hardware , read it from hardware */
        rv = _bcm_field_th_class_entry_hwread(unit, class_ent,
                                              ctype, databuf);
        if (BCM_FAILURE(rv)) {
            sal_free(databuf);
            return (rv);
        }
    }

    switch(ctype) {
        case _FieldClassEtherType:
            reg = ETHERTYPE_MAPr;
            if (qual_id == bcmFieldQualifyEtherType) {
                q_data[0] = soc_reg_field_get(unit, reg, *databuf,
                                                    ETHERTYPEf);
            }
            break;
        case _FieldClassTtl:
            if (qual_id == bcmFieldQualifyTtl) {
                q_data[0] = class_ent->tcam.ip_type;
            }
            break;
        case _FieldClassToS:
            if (qual_id == bcmFieldQualifyTos) {
                q_data[0] = class_ent->tcam.ip_type;
            }
            break;
        case _FieldClassIpProto:
            if (qual_id == bcmFieldQualifyIpProtocol) {
                q_data[0] = class_ent->tcam.ip_type;
            }
            break;
        case _FieldClassL4SrcPort:
            reg = L4_SRC_PORT_MAPr;
            keytype = soc_reg_field_get(unit, reg, *databuf,
                                              MATCH_TYPEf);
            if ((keytype == _FP_MATCH_L4_PORT_TYPE) &&
                (qual_id == bcmFieldQualifyL4SrcPort)) {
                q_data[0] = soc_reg_field_get(unit, reg, *databuf,
                                                    L4_SRC_PORTf);
            }
            if ((keytype == _FP_MATCH_EXCHANGE_ID_TYPE) &&
                (qual_id == bcmFieldQualifyFcoeOxID)) {
                q_data[0] = soc_reg_field_get(unit, reg, *databuf,
                                                    L4_SRC_PORTf);
            }
            break;
        case _FieldClassL4DstPort:
            reg = L4_DST_PORT_MAPr;
            keytype = soc_reg_field_get(unit, reg, *databuf,
                                              MATCH_TYPEf);
            if ((keytype == _FP_MATCH_L4_PORT_TYPE) &&
                (qual_id == bcmFieldQualifyL4DstPort)) {
                q_data[0] = soc_reg_field_get(unit, reg, *databuf,
                                                    L4_DST_PORTf);
            }
            if ((keytype == _FP_MATCH_EXCHANGE_ID_TYPE) &&
                (qual_id == bcmFieldQualifyFcoeRxID)) {
                q_data[0] = soc_reg_field_get(unit, reg, *databuf,
                                                    L4_DST_PORTf);
            }
            break;
        case _FieldClassTcp:
            if (qual_id == bcmFieldQualifyTcpControl) {
                q_data[0] = class_ent->tcam.ip_type;
            }
            break;
        case _FieldClassSrcCompression:
            mem = SRC_COMPRESSIONm;
            soc_mem_field_get(unit, mem, databuf,
                                    KEY_TYPEf, &keytype);
            if (keytype == _FP_KEY_IPv4_TYPE &&
                (qual_id == bcmFieldQualifySrcIp)) {
                soc_mem_field_get(unit, mem, databuf,
                                        IPV4__SIP_V4f, q_data);
                soc_mem_field_get(unit, mem, databuf,
                                        IPV4__SIP_V4_MASKf, q_mask);
            } else if (keytype == _FP_KEY_FCoE_TYPE &&
                (qual_id == bcmFieldQualifyFibreChanSrcId)) {
                soc_mem_field_get(unit, mem, databuf,
                                        FCOE__S_IDf, q_data);
                soc_mem_field_get(unit, mem, databuf,
                                        FCOE__S_ID_MASKf, q_mask);
            } else if (keytype == _FP_KEY_IPv6_TYPE &&
                (qual_id == bcmFieldQualifySrcIp6)) {
                soc_mem_field_get(unit, mem, databuf,
                                        IPV6__SIP_V6f, q_data);
                soc_mem_field_get(unit, mem, databuf,
                                        IPV6__SIP_V6_MASKf, q_mask);
            } else {
                /* Do Nothing */
            }
            if (qual_id == bcmFieldQualifyVrf) {
                soc_mem_field_get(unit, mem, databuf,
                                        VRFf, q_data);
                soc_mem_field_get(unit, mem, databuf,
                                        VRF_MASKf, q_mask);
            }
            break;
        case _FieldClassDstCompression:
            mem = DST_COMPRESSIONm;
            soc_mem_field_get(unit, mem, databuf,
                                    KEY_TYPEf, &keytype);
            if ((keytype == _FP_KEY_IPv4_TYPE) &&
                (qual_id == bcmFieldQualifyDstIp)) {

                soc_mem_field_get(unit, mem, databuf,
                                        IPV4__DIP_V4f, q_data);
                soc_mem_field_get(unit, mem, databuf,
                                        IPV4__DIP_V4_MASKf, q_mask);
            } else if ((keytype == _FP_KEY_FCoE_TYPE) &&
                       (qual_id == bcmFieldQualifyFibreChanDstId)) {

                soc_mem_field_get(unit, mem, databuf,
                                        FCOE__D_IDf, q_data);
                soc_mem_field_get(unit, mem, databuf,
                                        FCOE__D_ID_MASKf, q_mask);
            } else if ((keytype == _FP_KEY_IPv6_TYPE) &&
                       (qual_id == bcmFieldQualifyDstIp6)) {
                soc_mem_field_get(unit, mem, databuf,
                                        IPV6__DIP_V6f, q_data);
                soc_mem_field_get(unit, mem, databuf,
                                        IPV6__DIP_V6_MASKf, q_mask);
            } else {
                /* Do Nothing */
            }
            if (qual_id == bcmFieldQualifyVrf) {
                soc_mem_field_get(unit, mem, databuf,
                                        VRFf, q_data);
                soc_mem_field_get(unit, mem, databuf,
                                        VRF_MASKf, q_mask);
            }
            break;
        case _FieldClassIpTunnelTtl:
            if (qual_id == bcmFieldQualifyIpTunnelTtl) {
                q_data[0] = class_ent->tcam.ip_type;
            }
            break;
        default:
            rv = BCM_E_NONE;
    }

    /* Free Entry Buffer */
    sal_free(databuf);

    return (BCM_E_NONE);
}

/*
 * Function:
 *  _bcm_field_th_class_qualify_clear
 * Purpose:
 *  Clear Field Class Qualifiers.
 * Parameters:
 *  unit      - (IN) BCM device number.
 *  entry     - (IN) Field Entry Id.
 * Returns:
 *  BCM_E_XXX
 */
int
_bcm_field_th_class_qualify_clear(int unit, bcm_field_entry_t entry)
{
    int rv = BCM_E_NONE;        /* Operation Return Status */
    _field_entry_t *class_ent = NULL;
                                /* Field Class Entry */
    _field_group_t *fg = NULL;
                                /* Field group info */
    int q = 0;                  /* Qset iterator */
    _bcm_field_qual_data_t  q_data;
                                /* Qualifier match data */
    _bcm_field_qual_data_t  q_mask;
                                /* Qualifier match mask */

    /* Retrieve the field entry. */
    rv = _field_entry_get(unit, entry, _FP_ENTRY_PRIMARY, &class_ent);
    BCM_IF_ERROR_RETURN(rv);

    /* Parameter Check */
    if (class_ent->group == NULL) {
        return (BCM_E_INTERNAL);
    }

    /* Input Group Stage Check */
    if (class_ent->group->stage_id != _BCM_FIELD_STAGE_CLASS) {
        return (BCM_E_INTERNAL);
    }

    /* Retreive Field Group from entry */
    fg = class_ent->group;

    /* Initialize qdata and qmask */
    sal_memset(q_data, 0, sizeof(_bcm_field_qual_data_t));
    sal_memset(q_mask, 0, sizeof(_bcm_field_qual_data_t));

    /* Clear all qualifier in entry group */
    for (q = 0; q < (int)bcmFieldQualifyCount; q++) {
        if (BCM_FIELD_QSET_TEST(fg->qset, q)) {
            if ((q != bcmFieldQualifyStageClass) &&
                (q != bcmFieldQualifyStageClassExactMatch)) {
                    rv = _bcm_field_th_class_qualify_set(unit, entry,
                                                         q, q_data, q_mask);
                    if (BCM_FAILURE(rv)) {
                        return (rv);
                }
            }
        }
    }

    return (rv);
}

/*
 * Function:
 *  _bcm_field_th_class_action_set
 * Purpose:
 *  Set Field Class Entry Action.
 * Parameters:
 *  unit        - (IN) BCM device number.
 *  entry       - (IN) Field Entry Id.
 *  class_info  - (IN) Field Class Info Structure.
 * Returns:
 *  BCM_E_XXX
 */
int
_bcm_field_th_class_action_set(int unit, bcm_field_entry_t entry,
                               bcm_field_class_info_t *class_info)
{
    int rv = BCM_E_NONE;        /* Operation Return Status */
    _field_group_t *fg = NULL;
                                /* Field group info */
    _field_entry_t *f_ent = NULL;
                                /* Field entry info */
    _field_action_t *fa = NULL;
                                /* Field action info */
    _field_action_t *faiter = NULL;
                                /* Field action iterator */
    uint32 *cdata;              /* Class Data */
    _field_class_type_t ctype;  /* Field Class Type */
    soc_reg_t reg;              /* SOC Register */
    soc_mem_t mem;              /* SOC Memory */

    /* Confirm that 'entry' is  present on unit */
    if (BCM_FAILURE(_field_entry_get(unit, entry, _FP_ENTRY_PRIMARY, &f_ent))) {
        LOG_ERROR(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                 "FP(unit %d) Error: entry=(%d) does not exists.\n"),
                 unit, entry));
        return (BCM_E_NOT_FOUND);
    }

    /* Retreive Field Group */
    fg = f_ent->group;

    /* Entry Group Check */
    if (fg == NULL) {
        return (BCM_E_INTERNAL);
    }

    /* Class Info Check */
    if (class_info == NULL) {
        return (BCM_E_PARAM);
    }

    /* Skip if stage is not class */
    if (fg->stage_id != _BCM_FIELD_STAGE_CLASS) {
        return (BCM_E_PARAM);
    }

    /* Confirm that action is in aset. */
    if (SHR_BITGET(fg->aset.w, class_info->action) == 0) {
        LOG_DEBUG(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                 "FP(unit %d) Error: provided action not present in entry "
                 "group aset.\n"),
                 unit));
        return (BCM_E_PARAM);
    }

    /* Check action is not set already in entry */
    faiter = f_ent->actions;
    while (faiter != NULL) {
        if (faiter->action == class_info->action) {
            LOG_DEBUG(BSL_LS_BCM_FP,
                    (BSL_META_U(unit,
                     "FP(unit %d) Error: provided action already present"
                     " in field entry.\n"),
                     unit));
            return (BCM_E_EXISTS);
        }
        faiter = faiter->next;
    }

    fa = sal_alloc(sizeof (_field_action_t), "field_action");
    if (fa == NULL) {
        LOG_ERROR(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                 "FP(unit %d) Error: allocation failure for field_action\n"),
                 unit));
        return (BCM_E_MEMORY);
    }

    sal_memset(fa, 0, sizeof (_field_action_t));

    /* Assign and store class action values */
    fa->action = class_info->action;
    COMPILER_64_TO_32_LO(fa->param[0], class_info->class_id);
    COMPILER_64_TO_32_HI(fa->param[1], class_info->class_id);

    /* Retreive Class Type from entry qset */
    rv = _bcm_field_th_class_type_qset_get(unit,
                                        &f_ent->group->qset, &ctype);
    if (BCM_FAILURE(rv)) {
        sal_free(fa);
        return (rv);
    }

    /* Assign class data buffer */
    if (f_ent->tcam.key == NULL) {
        if ((f_ent->flags & _FP_ENTRY_INSTALLED) &&
                (f_ent->slice_idx != -1)) {
            /* Allocate memory for class data */
            _FP_XGS3_ALLOC(f_ent->tcam.key,
                           f_ent->tcam.key_size, "field class data buf");
            if (f_ent->tcam.key == NULL) {
                LOG_ERROR(BSL_LS_BCM_FP,
                         (BSL_META_U(unit,
                         "FP(unit %d) Error: allocation failure for "
                         "field class data buf\n"),
                         unit));
                sal_free(fa);
                return (BCM_E_MEMORY);
            }
            /* Entry is in hardware , read it from hardware */
            rv = _bcm_field_th_class_entry_hwread(unit, f_ent,
                                                  ctype, f_ent->tcam.key);
            if (BCM_FAILURE(rv)) {
                sal_free(f_ent->tcam.key);
                sal_free(fa);
                return (rv);
            }
        } else {
            sal_free(fa);
            return (BCM_E_INTERNAL);
        }
    }

    /* Assign class data and mask buffer */
    cdata = f_ent->tcam.key;

    switch(ctype) {
        case _FieldClassEtherType:
            reg = ETHERTYPE_MAPr;
            if ((fa->param[0] & ~(0xF)) ||
                (fa->param[1] & ~(0))) {
                rv = BCM_E_PARAM;
            } else {
                soc_reg_field_set(unit, reg, cdata,
                                        C_ETHERTYPEf, fa->param[0]);
            }
            break;
        case _FieldClassTtl:
            mem = TTL_FNm;
            if ((fa->param[0] & ~(0xFF)) ||
                (fa->param[1] & ~(0))) {
                rv = BCM_E_PARAM;
            } else {
                if (class_info->action == bcmFieldActionClassZero) {
                    soc_mem_field_set(unit, mem, cdata,
                                            FN0f, &fa->param[0]);
                } else {
                    soc_mem_field_set(unit, mem, cdata,
                                            FN1f, &fa->param[0]);
                }
            }
            break;
        case _FieldClassToS:
            mem = TOS_FNm;
            if ((fa->param[0] & ~(0xFF)) ||
                (fa->param[1] & ~(0))) {
                rv = BCM_E_PARAM;
            } else {
                if (class_info->action == bcmFieldActionClassZero) {
                    soc_mem_field_set(unit, mem, cdata,
                                            FN0f, &fa->param[0]);
                } else {
                    soc_mem_field_set(unit, mem, cdata,
                                            FN1f, &fa->param[0]);
                }
            }
            break;
        case _FieldClassIpProto:
            mem = IP_PROTO_MAPm;
            if ((fa->param[0] & ~(0xF)) ||
                (fa->param[1] & ~(0))) {
                rv = BCM_E_PARAM;
            } else {
                soc_mem_field_set(unit, mem, cdata,
                                        C_IP_PROTOCOLf, &fa->param[0]);
            }
            break;
        case _FieldClassL4SrcPort:
            reg = L4_SRC_PORT_MAPr;
            if ((fa->param[0] & ~(0xF)) ||
                (fa->param[1] & ~(0))) {
                rv = BCM_E_PARAM;
            } else {
                soc_reg_field_set(unit, reg, cdata,
                                        C_L4_SRC_PORTf, fa->param[0]);
            }
            break;
        case _FieldClassL4DstPort:
            reg = L4_DST_PORT_MAPr;
            if ((fa->param[0] & ~(0xF)) ||
                (fa->param[1] & ~(0))) {
                rv = BCM_E_PARAM;
            } else {
                soc_reg_field_set(unit, reg, cdata,
                                        C_L4_DST_PORTf, fa->param[0]);
            }
            break;
        case _FieldClassTcp:
            mem = TCP_FNm;
            if ((fa->param[0] & ~(0xFF)) ||
                (fa->param[1] & ~(0))) {
                rv = BCM_E_PARAM;
            } else {
                if (class_info->action == bcmFieldActionClassZero) {
                    soc_mem_field_set(unit, mem, cdata,
                                            FN0f, &fa->param[0]);
                } else {
                    soc_mem_field_set(unit, mem, cdata,
                                            FN1f, &fa->param[0]);
                }
            }
            break;
        case _FieldClassSrcCompression:
            mem = SRC_COMPRESSIONm;
            if (fa->param[1] & ~(0xF)) {
                rv = BCM_E_PARAM;
            } else {
                soc_mem_field_set(unit, mem, cdata,
                                        C_SRCf, &fa->param[0]);
            }
            break;
        case _FieldClassDstCompression:
            mem = DST_COMPRESSIONm;
            if (fa->param[1] & ~(0xF)) {
                rv = BCM_E_PARAM;
            } else {
                soc_mem_field_set(unit, mem, cdata,
                                        C_DSTf, &fa->param[0]);
            }
            break;
        case _FieldClassIpTunnelTtl:
            mem = ALT_TTL_FNm;
            if ((fa->param[0] & ~(0xFF)) ||
                (fa->param[1] & ~(0))) {
                rv = BCM_E_PARAM;
            } else {
                if (class_info->action == bcmFieldActionClassZero) {
                    soc_mem_field_set(unit, mem, cdata,
                                            FN0f, &fa->param[0]);
                } else {
                    rv = BCM_E_PARAM;
                }
            }
            break;
        default:
            rv = BCM_E_PARAM;
    }

    if (BCM_FAILURE(rv)) {
        sal_free(fa);
        return (rv);
    }

    /* Add action to front of entry's linked-list. */
    fa->next = f_ent->actions;
    f_ent->actions  = fa;

    /* Mark entry as dirty. */
    f_ent->flags |= _FP_ENTRY_DIRTY;

    return (rv);
}

/*
 * Function:
 *  _bcm_field_th_class_action_delete
 * Purpose:
 *  Delete Field Class Entry Action.
 * Parameters:
 *  unit        - (IN) BCM device number.
 *  entry       - (IN) Field Entry Id.
 *  action      - (IN) Field Action info.
 * Returns:
 *  BCM_E_XXX
 */
int
_bcm_field_th_class_action_delete(int unit, bcm_field_entry_t entry,
                                           bcm_field_action_t action)
{
    int rv = BCM_E_NONE;        /* Operation Return Status */
    _field_group_t *fg = NULL;  /* Field group info */
    _field_entry_t *f_ent = NULL;
                                /* Field entry info */
    _field_action_t *fa = NULL;
                                /* Field action info */
    _field_action_t *fa_prev = NULL;
                                /* Field action info previous */
    uint32 *cdata = NULL;       /* Class Data */
    _field_class_type_t ctype;  /* Field Class Type */
    soc_reg_t reg;              /* SOC Register */
    soc_mem_t mem;              /* SOC Memory */
    uint32 valid = 0;           /* Valid */
    uint32 action_clear[2] = {0, 0};
                                /* Class Src/Dst */

    /* Confirm that 'entry' is  present on unit */
    if (BCM_FAILURE(_field_entry_get(unit, entry, _FP_ENTRY_PRIMARY, &f_ent))) {
        LOG_ERROR(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                 "FP(unit %d) Error: entry=(%d) does not exists.\n"),
                 unit, entry));
        return (BCM_E_NOT_FOUND);
    }

    /* Retreive Field Group */
    fg = f_ent->group;

    /* Entry Group Check */
    if (fg == NULL) {
        return (BCM_E_INTERNAL);
    }

    /* Find the action in the entry */
    fa = f_ent->actions; /* start at head */

    while (fa != NULL) {
        if (fa->action != action) {
            fa_prev = fa;
            fa      = fa->next;
            continue;
        }
        break;
    }

    if (NULL == fa) {
        LOG_ERROR(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                 "FP(unit %d) Error: action=(%d) is not present in entry.\n"),
                 unit, action));
        return (BCM_E_NOT_FOUND);
    }

    /* Retreive Class Type from entry qset */
    rv = _bcm_field_th_class_type_qset_get(unit,
                                       &f_ent->group->qset,
                                       &ctype);
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    /* Assign class data buffer */
    if (f_ent->tcam.key == NULL) {
        if ((f_ent->flags & _FP_ENTRY_INSTALLED) &&
            (f_ent->slice_idx != -1)) {
            /* Allocate memory for class data */
            _FP_XGS3_ALLOC(f_ent->tcam.key,
                    f_ent->tcam.key_size, "field class data buf");
            if (f_ent->tcam.key == NULL) {
                LOG_ERROR(BSL_LS_BCM_FP,
                         (BSL_META_U(unit,
                         "FP(unit %d) Error: allocation failure for "
                         "field class data buf\n"),
                         unit));
                return (BCM_E_MEMORY);
            }
            /* Entry is in hardware , read it from hardware */
            rv = _bcm_field_th_class_entry_hwread(unit, f_ent,
                                                  ctype, f_ent->tcam.key);
            if (BCM_FAILURE(rv)) {
                sal_free(f_ent->tcam.key);
                return (rv);
            }
        } else {
            return (BCM_E_INTERNAL);
        }
    }

    /* Assign class data and mask buffer */
    cdata = f_ent->tcam.key;

    switch(ctype) {
        case _FieldClassEtherType:
            reg = ETHERTYPE_MAPr;
            soc_reg_field_set(unit, reg, cdata, C_ETHERTYPEf, valid);
            break;
        case _FieldClassTtl:
            mem = TTL_FNm;
            if (fa->action == bcmFieldActionClassZero) {
                soc_mem_field_set(unit, mem, cdata, FN0f, &valid);
            } else {
                soc_mem_field_set(unit, mem, cdata, FN1f, &valid);
            }
            break;
        case _FieldClassToS:
            mem = TOS_FNm;
            if (fa->action == bcmFieldActionClassZero) {
                soc_mem_field_set(unit, mem, cdata, FN0f, &valid);
            } else {
                soc_mem_field_set(unit, mem, cdata, FN1f, &valid);
            }
            break;
        case _FieldClassIpProto:
            mem = IP_PROTO_MAPm;
            soc_mem_field_set(unit, mem, cdata, C_IP_PROTOCOLf, &valid);
            break;
        case _FieldClassL4SrcPort:
            reg = L4_SRC_PORT_MAPr;
            soc_reg_field_set(unit, reg , cdata, C_L4_SRC_PORTf, valid);
            break;
        case _FieldClassL4DstPort:
            reg = L4_DST_PORT_MAPr;
            soc_reg_field_set(unit, reg , cdata, C_L4_DST_PORTf, valid);
            break;
        case _FieldClassTcp:
            mem = TCP_FNm;
            if (fa->action == bcmFieldActionClassZero) {
                soc_mem_field_set(unit, mem, cdata, FN0f, &valid);
            } else {
                soc_mem_field_set(unit, mem, cdata, FN1f, &valid);
            }
            break;
        case _FieldClassSrcCompression:
            mem = SRC_COMPRESSIONm;
            soc_mem_field_set(unit, mem, cdata, C_SRCf, action_clear);
            break;
        case _FieldClassDstCompression:
            mem = DST_COMPRESSIONm;
            soc_mem_field_set(unit, mem, cdata, C_DSTf, action_clear);
            break;
        case _FieldClassIpTunnelTtl:
            mem = ALT_TTL_FNm;
            if (fa->action == bcmFieldActionClassZero) {
                soc_mem_field_set(unit, mem, cdata, FN0f, &valid);
            } else {
                rv = BCM_E_PARAM;
            }
            break;
        default:
            rv = BCM_E_PARAM;
    }

    if (fa_prev != NULL) {
        fa_prev->next = fa->next;
    } else { /* matched head of list */
        f_ent->actions = fa->next;
    }

    /* okay to free action */
    sal_free(fa);
    fa = NULL;

    /* Mark Entry as Dirty */
    f_ent->flags |= _FP_ENTRY_DIRTY;

    return rv;
}

/*
 * Function:
 *  _bcm_field_th_class_action_get
 * Purpose:
 *  Get Field Class Entry Action.
 * Parameters:
 *  unit        - (IN) BCM device number.
 *  entry       - (IN) Field Entry Id.
 *  class_info  - (INOUT) Field Class Info Structure.
 * Returns:
 *  BCM_E_XXX
 */
int
_bcm_field_th_class_action_get(int unit, bcm_field_entry_t entry,
                                bcm_field_class_info_t *class_info)
{
    int rv = BCM_E_NONE;        /* Operation Return Status */
    _field_group_t *fg = NULL;
                                /* Field group info */
    _field_entry_t *f_ent = NULL;
                                /* Field entry info */
    _field_action_t *fa = NULL;
                                /* Field action info */

    /* Confirm that 'entry' is  present on unit */
    if (BCM_FAILURE(_field_entry_get(unit, entry, _FP_ENTRY_PRIMARY, &f_ent))) {
        LOG_ERROR(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                 "FP(unit %d) Error: entry=(%d) does not exists.\n"),
                 unit, entry));
        return (BCM_E_BADID);
    }

    /* Retreive Field Group */
    fg = f_ent->group;

    /* Entry Group Check */
    if (fg == NULL) {
        return (BCM_E_INTERNAL);
    }

    /* Class Info Check */
    if (class_info == NULL) {
        return (BCM_E_PARAM);
    }

    /* Skip if stage is not class */
    if (fg->stage_id != _BCM_FIELD_STAGE_CLASS) {
        return (BCM_E_PARAM);
    }

    /* Check action is not set already in entry */
    fa = f_ent->actions;
    while (fa != NULL) {
        if (fa->action == class_info->action) {
            break;
        }
        fa = fa->next;
    }

    if (NULL == fa) {
        LOG_ERROR(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                 "FP(unit %d) Error: action=(%d) is not present in entry.\n"),
                 unit, class_info->action));
        return (BCM_E_NOT_FOUND);
    }

    COMPILER_64_SET(class_info->class_id, fa->param[1], fa->param[0]);

    return (rv);
}

/*
 * Function:
 *  _bcm_field_th_qual_class_size_get
 * Purpose:
 *  Get Field Qualifier Class Size
 * Parameters:
 *  unit        - (IN) BCM device number.
 *  qual        - (IN) Field Qualifier.
 *  class_size  - (OUT) Field Qualifier Class Size.
 * Returns:
 *  BCM_E_XXX
 * Notes:
 *  This api returns class size for provided qualifier
 *  Size provided is in bits.
 */
int
_bcm_field_th_qual_class_size_get(int unit, bcm_field_qualify_t qual,
                                                   uint16 *class_size)
{
    int rv = BCM_E_NONE;        /* Operation Return Status */

#if defined (BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(unit)) {
        return _bcm_field_th3_qual_class_size_get(unit, qual, class_size);
    }
#endif

    switch (qual) {
        case bcmFieldQualifyEtherType:
            *class_size = _FP_QUAL_CLASS_SIZE_ETHERTYPE;
            break;
        case bcmFieldQualifyTtl:
            *class_size = _FP_QUAL_CLASS_SIZE_TTL;
            break;
        case bcmFieldQualifyTos:
            *class_size = _FP_QUAL_CLASS_SIZE_TOS;
            break;
        case bcmFieldQualifyIpProtocol:
            *class_size = _FP_QUAL_CLASS_SIZE_IP_PROTO;
            break;
        case bcmFieldQualifyL4SrcPort:
            *class_size = _FP_QUAL_CLASS_SIZE_L4_SRC_PORT;
            break;
        case bcmFieldQualifyFcoeOxID:
            *class_size = _FP_QUAL_CLASS_SIZE_L4_SRC_PORT;
            break;
        case bcmFieldQualifyL4DstPort:
            *class_size = _FP_QUAL_CLASS_SIZE_L4_DST_PORT;
            break;
        case bcmFieldQualifyFcoeRxID:
            *class_size = _FP_QUAL_CLASS_SIZE_L4_DST_PORT;
            break;
        case bcmFieldQualifyTcpControl:
            *class_size = _FP_QUAL_CLASS_SIZE_TCP;
            break;
        case bcmFieldQualifySrcIp:
            *class_size = _FP_QUAL_CLASS_SIZE_SRC_COMPRESSION;
            break;
        case bcmFieldQualifySrcIp6:
            *class_size = _FP_QUAL_CLASS_SIZE_SRC_COMPRESSION;
            break;
        case bcmFieldQualifyFibreChanSrcId:
            *class_size = _FP_QUAL_CLASS_SIZE_SRC_COMPRESSION;
            break;
        case bcmFieldQualifyDstIp:
            *class_size = _FP_QUAL_CLASS_SIZE_DST_COMPRESSION;
            break;
        case bcmFieldQualifyDstIp6:
            *class_size = _FP_QUAL_CLASS_SIZE_DST_COMPRESSION;
            break;
        case bcmFieldQualifyFibreChanDstId:
            *class_size = _FP_QUAL_CLASS_SIZE_DST_COMPRESSION;
            break;
        case bcmFieldQualifyVrf:
            *class_size = _FP_QUAL_CLASS_SIZE_SRC_COMPRESSION;
            break;
        default:
            rv = BCM_E_PARAM;
    }

    return (rv);
}

/*
 * Function:
 *  _bcm_field_th_class_entry_install
 * Purpose:
 *  Install Field Class Entry.
 * Parameters:
 *  unit        - (IN) BCM device number.
 *  entry       - (IN) Field Entry Id.
 * Returns:
 *  BCM_E_XXX
 */
int
_bcm_field_th_class_entry_install(int unit, bcm_field_entry_t entry)
{
    int rv = BCM_E_NONE;            /* Operation Return Status */
    _field_group_t *fg = NULL;      /* Field group info */
    _field_entry_t *f_ent = NULL;   /* Field entry info */
    _field_class_type_t ctype;      /* Field Class Type */
    int hw_index = 0;               /* Hardware Index */
    _field_class_info_t **class_status_arr;
                                    /* Field Per Pipe Class Status Array */
    _field_class_info_t *class_status_info;
                                    /* Field Per Compression table status */
    _field_stage_t *stage_fc = NULL;
                                    /* Stage field control info */

    /* Confirm that 'entry' is  present on unit */
    if (BCM_FAILURE(_field_entry_get(unit, entry, _FP_ENTRY_PRIMARY, &f_ent))) {
        LOG_ERROR(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                 "FP(unit %d) Error: entry=(%d) does not exists.\n"),
                 unit, entry));
        return (BCM_E_BADID);
    }

    /* Retreive Field Group */
    fg = f_ent->group;

    /* Check if group is associated */
    if (fg == NULL) {
        return (BCM_E_INTERNAL);
    }

    /* Get group stage control structure. */
    rv = _field_stage_control_get(unit, fg->stage_id, &stage_fc);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                 "FP(unit %d) Error: Stage (%d) control get failure.\n"),
                 unit, fg->stage_id));
        return (rv);
    }

    /* Check if entry is installed and not dirty */
    if ((f_ent->flags & _FP_ENTRY_INSTALLED) &&
        (!(f_ent->flags & _FP_ENTRY_DIRTY))) {
        /* Nothing to do - entry is installed and clean */
        return (BCM_E_NONE);
    }

    /* Check if data and action exist */
    if ((f_ent->tcam.key == NULL) ||
        (f_ent->actions == NULL)) {
        LOG_DEBUG(BSL_LS_BCM_FP,
                (BSL_META_U(unit,
                 "FP(unit %d) Error: entry=(%d) data or action is not qualified.\n"),
                 unit, entry));
        return (BCM_E_PARAM);
    }

    /* Retreive Class Type from entry qset */
    rv = _bcm_field_th_class_type_qset_get(unit, &fg->qset, &ctype);
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    /* Update Class Status Info Structure */
    class_status_arr = stage_fc->class_info_arr[fg->instance];
    if (class_status_arr == NULL) {
        return (BCM_E_INTERNAL);
    }
    class_status_info = class_status_arr[ctype];
    if (class_status_info == NULL) {
        return (BCM_E_INTERNAL);
    }

    /* Allocate HwIndex Only if index is not allocate previously */
    if (f_ent->slice_idx == -1) {
        rv = _field_th_class_hwindex_get(unit, f_ent, ctype, &hw_index);
        if (BCM_FAILURE(rv)) {
            return (BCM_E_RESOURCE);
        }
        f_ent->slice_idx = hw_index;
    }

    /* Install Entry in Hardware */
    rv = _bcm_field_th_class_entry_hwinstall(unit, f_ent, ctype);
    if (BCM_FAILURE(rv)) {
        f_ent->slice_idx = -1;
        return (rv);
    }

    sal_free(f_ent->tcam.key);
    f_ent->tcam.key = NULL;
    f_ent->flags |= _FP_ENTRY_INSTALLED;
    f_ent->flags &= ~_FP_ENTRY_DIRTY;
    _FP_CLASS_TABLE_BMP_ADD(class_status_info->class_bmp, f_ent->slice_idx);

    /* Sorted Insert for Src/Dst Compression. */
    if ((ctype == _FieldClassSrcCompression) ||
        (ctype == _FieldClassDstCompression)) {
        if (class_status_info->total_entries_used !=
            class_status_info->total_entries_available) {
            rv = _bcm_field_th_class_entry_prio_set(unit, f_ent, f_ent->prio);
            if (BCM_FAILURE(rv)) {
                return (rv);
            }
        }
        rv = _field_th_class_valid_set(unit, f_ent, ctype, 1);
    }

    return (rv);
}

/*
 * Function:
 *  _field_th_class_hwindex_get
 * Purpose:
 *  Get Field Class Entry Hw Index.
 * Parameters:
 *  unit        - (IN) BCM device number.
 *  entry       - (IN) Field Entry Info.
 *  ctype       - (IN) Field Class Type.
 *  hw_index    - (OUT) Field Entry Hardware Index.
 * Returns:
 *  BCM_E_XXX
 */
STATIC int
_field_th_class_hwindex_get(int unit, _field_entry_t *entry,
                            _field_class_type_t ctype, int *hw_index)
{
    int rv = BCM_E_PARAM;           /* Operation Return Status */
    _field_group_t *fg = NULL;      /* Field group info */
    _field_class_info_t **class_status_arr;
                                    /* Field Per Pipe Class Status Array */
    _field_class_info_t *class_status_info;
                                    /* Field Per Compression table status */
    _field_stage_t *stage_fc = NULL;
                                    /* Stage field control info */
    int idx = 0;                    /* Index Iterator */

    /* Input Parameters Check */
    if (entry == NULL) {
        return (BCM_E_INTERNAL);
    }

    /* Retreive Field Group */
    fg = entry->group;

    /* Get group stage control structure. */
    rv = _field_stage_control_get(unit, fg->stage_id, &stage_fc);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                 "FP(unit %d) Error: Stage (%d) control get failure.\n"),
                 unit, fg->stage_id));
        return (rv);
    }

    /* Update Class Info Status Structure */
    class_status_arr = stage_fc->class_info_arr[fg->instance];
    if (class_status_arr == NULL) {
        return (BCM_E_INTERNAL);
    }
    class_status_info = class_status_arr[ctype];
    if (class_status_info == NULL) {
        return (BCM_E_INTERNAL);
    }

    switch (ctype) {
        case _FieldClassEtherType:
        case _FieldClassL4SrcPort:
        case _FieldClassL4DstPort:
        case _FieldClassSrcCompression:
        case _FieldClassDstCompression:
            for (idx = 0; idx < class_status_info->total_entries_available;
                                                                    idx++) {
                if (0 == _FP_CLASS_TABLE_BMP_TEST(
                                       class_status_info->class_bmp, idx)) {
                    *hw_index = idx;
                    rv = BCM_E_NONE;
                    break;
                }
            }
            break;
        case _FieldClassTtl:
        case _FieldClassToS:
        case _FieldClassIpProto:
        case _FieldClassTcp:
        case _FieldClassIpTunnelTtl:
            *hw_index = entry->tcam.ip_type;
            rv = BCM_E_NONE;
            break;
        default:
            rv = BCM_E_PARAM;
    }

    return rv;
}

/*
 * Function:
 *  _bcm_field_th_class_entry_hwinstall
 * Purpose:
 *  Install Class Entry In Hardware.
 * Parameters:
 *  unit        - (IN) BCM device number.
 *  entry       - (IN) Field Entry Info.
 *  ctype       - (IN) Field Class Type.
 * Returns:
 *  BCM_E_XXX
 */
int
_bcm_field_th_class_entry_hwinstall(int unit, _field_entry_t *entry,
                                           _field_class_type_t ctype)
{
    int rv = BCM_E_NONE;        /* Operation Return Status */

    /* Input Parameter Check */
    if ((entry == NULL) ||
        (entry->group == NULL) ||
        (entry->tcam.key == NULL)) {
        return (BCM_E_INTERNAL);
    }

    switch (ctype) {
        case _FieldClassEtherType:
            rv = _field_th_class_ethertype_install(unit,entry);
            break;
        case _FieldClassL4SrcPort:
            rv = _field_th_class_l4srcport_install(unit,entry);
            break;
        case _FieldClassL4DstPort:
            rv = _field_th_class_l4dstport_install(unit,entry);
            break;
        case _FieldClassSrcCompression:
            rv = _field_th_class_srccompression_install(unit,entry);
            break;
        case _FieldClassDstCompression:
            rv = _field_th_class_dstcompression_install(unit,entry);
            break;
        case _FieldClassTtl:
            rv = _field_th_class_ttl_install(unit,entry);
            break;
        case _FieldClassToS:
            rv = _field_th_class_tos_install(unit,entry);
            break;
        case _FieldClassIpProto:
            rv = _field_th_class_ipproto_install(unit,entry);
            break;
        case _FieldClassTcp:
            rv = _field_th_class_tcp_install(unit,entry);
            break;
        case _FieldClassIpTunnelTtl:
            rv = _field_th_class_iptunnelttl_install(unit,entry);
            break;
        default:
            rv = BCM_E_PARAM;
    }

    return rv;
}

/*
 * Function:
 *  _bcm_field_th_class_entry_hwread.
 * Purpose:
 *  Read Class Entry From Hardware.
 * Parameters:
 *  unit        - (IN) BCM device number.
 *  entry       - (IN) Field Entry Info.
 *  ctype       - (IN) Field Class Type.
 *  cdata       - (OUT) Field Class Entry DataBuffer.
 * Returns:
 *  BCM_E_XXX
 */
int
_bcm_field_th_class_entry_hwread(int unit, _field_entry_t *entry,
                                       _field_class_type_t ctype,
                                                   uint32 *cdata)
{
    int rv = BCM_E_NONE;        /* Operation Return Status */

    /* Input Parameter Check */
    if ((entry == NULL) ||
        (entry->group == NULL) ||
        (cdata == NULL)) {
        return (BCM_E_INTERNAL);
    }

    /* Read based on class type */
    switch (ctype) {
        case _FieldClassEtherType:
            rv = _field_th_class_ethertype_read(unit,entry,cdata);
            break;
        case _FieldClassL4SrcPort:
            rv = _field_th_class_l4srcport_read(unit,entry,cdata);
            break;
        case _FieldClassL4DstPort:
            rv = _field_th_class_l4dstport_read(unit,entry,cdata);
            break;
        case _FieldClassSrcCompression:
            rv = _field_th_class_srccompression_read(unit,entry,cdata);
            break;
        case _FieldClassDstCompression:
            rv = _field_th_class_dstcompression_read(unit,entry,cdata);
            break;
        case _FieldClassTtl:
            rv = _field_th_class_ttl_read(unit,entry,cdata);
            break;
        case _FieldClassToS:
            rv = _field_th_class_tos_read(unit,entry,cdata);
            break;
        case _FieldClassIpProto:
            rv = _field_th_class_ipproto_read(unit,entry,cdata);
            break;
        case _FieldClassTcp:
            rv = _field_th_class_tcp_read(unit,entry,cdata);
            break;
        case _FieldClassIpTunnelTtl:
            rv = _field_th_class_iptunnelttl_read(unit,entry,cdata);
            break;
        default:
            rv = BCM_E_PARAM;
    }

    return rv;
}

/*
 * Function:
 *  _field_th_class_ethertype_install
 * Purpose:
 *  Install EtherType Class Entry In Hardware.
 * Parameters:
 *  unit        - (IN) BCM device number.
 *  entry       - (IN) Field Entry Info.
 * Returns:
 *  BCM_E_XXX:
 * Notes:
 *  No Null Check for entry, group and key.
 *  Done by calling function _bcm_field_th_class_entry_hwinstall
 *  entry->slice_idx contains index to write
 *  entry->tcam.key contains data to write
 */
STATIC int
_field_th_class_ethertype_install(int unit, _field_entry_t *entry)
{
    int rv = BCM_E_NONE;        /* Operation Return Status */
    _field_group_t *fg = NULL;
                                /* Field group info */
    _field_stage_t *stage_fc = NULL;
                                /* Stage field control info */
    uint32 rval = 0;            /* Register Value */
    soc_reg_t reg;              /* SOC Register */
    uint32 *cdata = NULL;       /* Class Data */

    /* Get class data key and group info */
    cdata = entry->tcam.key;
    fg = entry->group;

    /* Get Class Data Value */
    rval = *cdata;

    /* Get group stage control structure. */
    rv = _field_stage_control_get(unit, fg->stage_id, &stage_fc);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                 "FP(unit %d) Error: Stage (%d) control get failure.\n"),
                 unit, fg->stage_id));
        return (rv);
    }

    /* Decide register based on operational mode */
    if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
        reg = ETHERTYPE_MAPr;
    } else {
        BCM_IF_ERROR_RETURN(_bcm_field_reg_instance_get(unit,
                                                           ETHERTYPE_MAPr,
                                                           fg->instance,
                                                           &reg));
    }

    /* Write Register */
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg,
                                REG_PORT_ANY, entry->slice_idx, rval));

    return (rv);
}

/*
 * Function:
 *  _field_th_class_ethertype_read
 * Purpose:
 *  Read EtherType Class Entry From Hardware.
 * Parameters:
 *  unit        - (IN)  BCM device number.
 *  entry       - (IN)  Field Entry Info.
 *  cdata       - (OUT) Field Class Entry DataBuffer.
 * Returns:
 *    BCM_E_XXX
 * Notes:
 *  No Null Check for entry, group and classdata buffer.
 *  Done by calling function _bcm_field_th_class_entry_hwread
 *  entry->slice_idx contains index to read
 *  cdata contains data read from hardware.
 */
STATIC int
_field_th_class_ethertype_read(int unit, _field_entry_t *entry,
                                                  uint32 *cdata)
{
    int rv = BCM_E_NONE;        /* Operation Return Status */
    _field_group_t *fg = NULL;
                                /* Field group info */
    _field_stage_t *stage_fc = NULL;
                                /* Stage field control info */
    soc_reg_t reg;              /* SOC Register */

    /* Get class group info */
    fg = entry->group;

    /* Get group stage control structure. */
    rv = _field_stage_control_get(unit, fg->stage_id, &stage_fc);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                 "FP(unit %d) Error: Stage (%d) control get failure.\n"),
                 unit, fg->stage_id));
        return (rv);
    }

    /* Decide register based on operational mode */
    if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
        reg = ETHERTYPE_MAPr;
    } else {
        BCM_IF_ERROR_RETURN(_bcm_field_reg_instance_get(unit,
                                                           ETHERTYPE_MAPr,
                                                           fg->instance,
                                                           &reg));
    }

    /* Read Register */
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg,
                                REG_PORT_ANY, entry->slice_idx, cdata));

    return (rv);
}

/*
 * Function:
 *  _field_th_class_l4srcport_install
 * Purpose:
 *  Install L4 Src port Class Entry In Hardware.
 * Parameters:
 *  unit        - (IN) BCM device number.
 *  entry       - (IN) Field Entry Info.
 * Returns:
 *  BCM_E_XXX
 * Notes:
 *  No Null Check for entry, group and key.
 *  Done by calling function _bcm_field_th_class_entry_hwinstall
 *  entry->slice_idx contains index to write
 *  entry->tcam.key contains data to write
 */
STATIC int
_field_th_class_l4srcport_install(int unit, _field_entry_t *entry)
{
    int rv = BCM_E_NONE;        /* Operation Return Status */
    _field_group_t *fg = NULL;
                                /* Field group info */
    _field_stage_t *stage_fc = NULL;
                                /* Stage field control info */
    uint32 rval = 0;            /* Register Value */
    soc_reg_t reg;              /* SOC Register */
    uint32 *cdata = NULL;       /* Class Data */

    /* Get class data key and group info */
    cdata = entry->tcam.key;
    fg = entry->group;

    /* Get Class Data Value */
    rval = *cdata;

    /* Get group stage control structure. */
    rv = _field_stage_control_get(unit, fg->stage_id, &stage_fc);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                 "FP(unit %d) Error: Stage (%d) control get failure.\n"),
                 unit, fg->stage_id));
        return (rv);
    }

    /* Decide register based on operational mode */
    if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
        reg = L4_SRC_PORT_MAPr;
    } else {
        BCM_IF_ERROR_RETURN(_bcm_field_reg_instance_get(unit,
                                                           L4_SRC_PORT_MAPr,
                                                           fg->instance,
                                                           &reg));

    }

    /* Write Register */
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY,
                                                entry->slice_idx, rval));

    return (rv);
}

/*
 * Function:
 *  _field_th_class_l4srcport_read
 * Purpose:
 *  Read L4 Src port Class Entry From Hardware.
 * Parameters:
 *  unit        - (IN) BCM device number.
 *  entry       - (IN) Field Entry Info.
 *  cdata       - (OUT) Field Class Entry DataBuffer.
 * Returns:
 *    BCM_E_XXX
 * Notes:
 *  No Null Check for entry, group and classdata buffer.
 *  Done by calling function _bcm_field_th_class_entry_hwread
 *  entry->slice_idx contains index to read
 *  cdata contains data read from hardware.
 */
STATIC int
_field_th_class_l4srcport_read(int unit, _field_entry_t *entry,
                                                  uint32 *cdata)
{
    int rv = BCM_E_NONE;        /* Operation Return Status */
    _field_group_t *fg = NULL;
                                /* Field group info */
    _field_stage_t *stage_fc = NULL;
                                /* Stage field control info */
    soc_reg_t reg;              /* SOC Register */


    /* Get class group info */
    fg = entry->group;

    /* Get group stage control structure. */
    rv = _field_stage_control_get(unit, fg->stage_id, &stage_fc);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                 "FP(unit %d) Error: Stage (%d) control get failure.\n"),
                 unit, fg->stage_id));
        return (rv);
    }

    /* Decide register based on operational mode */
    if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
        reg = L4_SRC_PORT_MAPr;
    } else {
        BCM_IF_ERROR_RETURN(_bcm_field_reg_instance_get(unit,
                                                           L4_SRC_PORT_MAPr,
                                                           fg->instance,
                                                           &reg));

    }

    /* Read Register */
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY,
                                        entry->slice_idx, cdata));

    return (rv);
}

/*
 * Function:
 *  _field_th_class_l4dstport_install
 * Purpose:
 *  Install L4 Dst Port Class Entry In Hardware.
 * Parameters:
 *  unit        - (IN) BCM device number.
 *  entry       - (IN) Field Entry Info.
 * Returns:
 *  BCM_E_XXX
 * Notes:
 *  No Null Check for entry , group and key.
 *  Done by calling function _bcm_field_th_class_entry_hwinstall
 *  entry->slice_idx contains index to write
 *  entry->tcam.key contains data to write
 */
STATIC int
_field_th_class_l4dstport_install(int unit, _field_entry_t *entry)
{
    int rv = BCM_E_NONE;        /* Operation Return Status */
    _field_group_t *fg = NULL;
                                /* Field group info */
    _field_stage_t *stage_fc = NULL;
                                /* Stage field control info */
    uint32 rval = 0;            /* Register Value */
    soc_reg_t reg;              /* SOC Register */
    uint32 *cdata = NULL;       /* Class Data */

    /* Get class data key and group info */
    cdata = entry->tcam.key;
    fg = entry->group;

    /* Get Class Data Value */
    rval = *cdata;

    /* Get group stage control structure. */
    rv = _field_stage_control_get(unit, fg->stage_id, &stage_fc);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                 "FP(unit %d) Error: Stage (%d) control get failure.\n"),
                 unit, fg->stage_id));
        return (rv);
    }

    /* Decide register based on operational mode */
    if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
        reg = L4_DST_PORT_MAPr;
    } else {
        BCM_IF_ERROR_RETURN(_bcm_field_reg_instance_get(unit,
                                                           L4_DST_PORT_MAPr,
                                                           fg->instance,
                                                           &reg));
    }

    /* Write Register */
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY,
                                                  entry->slice_idx, rval));

    return (rv);
}

/*
 * Function:
 *  _field_th_class_l4dstport_read
 * Purpose:
 *  Read L4 Dst Port Class Entry From Hardware.
 * Parameters:
 *  unit        - (IN) BCM device number.
 *  entry       - (IN) Field Entry Info.
 *  cdata       - (OUT) Field Class Entry DataBuffer.
 * Returns:
 *  BCM_E_XXX
 * Notes:
 *  No Null Check for entry, group and classdata buffer.
 *  Done by calling function _bcm_field_th_class_entry_hwread
 *  entry->slice_idx contains index to read
 *  cdata contains data read from hardware.
 */
 STATIC int
_field_th_class_l4dstport_read(int unit, _field_entry_t *entry,
                                                 uint32 *cdata)
{
    int rv = BCM_E_NONE;        /* Operation Return Status */
    _field_group_t *fg = NULL;
                                /* Field group info */
    _field_stage_t *stage_fc = NULL;
                                /* Stage field control info */
    soc_reg_t reg;              /* SOC Register */

    /* Get class group info */
    fg = entry->group;

    /* Get group stage control structure. */
    rv = _field_stage_control_get(unit, fg->stage_id, &stage_fc);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                 "FP(unit %d) Error: Stage (%d) control get failure.\n"),
                 unit, fg->stage_id));
        return (rv);
    }

    /* Decide register based on operational mode */
    if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
        reg = L4_DST_PORT_MAPr;
    } else {
        BCM_IF_ERROR_RETURN(_bcm_field_reg_instance_get(unit,
                                                           L4_DST_PORT_MAPr,
                                                           fg->instance,
                                                           &reg));
    }

    /* Read Register */
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY,
                                            entry->slice_idx, cdata));

    return (rv);
}

/*
 * Function:
 *  _field_th_class_srccompression_install
 * Purpose:
 *  Install Src Compression Class Entry In Hardware.
 * Parameters:
 *  unit        - (IN) BCM device number.
 *  entry       - (IN) Field Entry Info.
 * Returns:
 *  BCM_E_XXX
 * Notes:
 *  No Null Check for entry , group and key.
 *  Done by calling function _bcm_field_th_class_entry_hwinstall
 *  entry->slice_idx contains index to write
 *  entry->tcam.key contains data to write
 */
STATIC int
_field_th_class_srccompression_install(int unit, _field_entry_t *entry)
{
    int rv = BCM_E_NONE;        /* Operation Return Status */
    _field_group_t *fg = NULL;
                                /* Field group info */
    _field_stage_t *stage_fc = NULL;
                                /* Stage field control info */
    soc_mem_t mem;              /* SOC Memory */

    /* Get class group info */
    fg = entry->group;

    /* Get group stage control structure. */
    rv = _field_stage_control_get(unit, fg->stage_id, &stage_fc);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                 "FP(unit %d) Error: Stage (%d) control get failure.\n"),
                 unit, fg->stage_id));
        return (rv);
    }

    /* Decide register based on operational mode */
    if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
        mem = SRC_COMPRESSIONm;
    } else {
        BCM_IF_ERROR_RETURN(_bcm_field_mem_instance_get(unit,
                                               SRC_COMPRESSIONm,
                                               fg->instance,
                                               &mem));
    }

    /* Write Memory */
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                                        entry->slice_idx, entry->tcam.key));

    return (rv);
}

/*
 * Function:
 *  _field_th_class_srccompression_read
 * Purpose:
 *  Read Src Compression Class Entry From Hardware.
 * Parameters:
 *  unit        - (IN) BCM device number.
 *  entry       - (IN) Field Entry Info.
 *  cdata       - (OUT) Field Class Entry DataBuffer.
 * Returns:
 *  BCM_E_XXX
 * Notes:
 *  No Null Check for entry, group and classdata buffer.
 *  Done by calling function _bcm_field_th_class_entry_hwread
 *  entry->slice_idx contains index to read
 *  cdata contains data read from hardware.
 */
STATIC int
_field_th_class_srccompression_read(int unit, _field_entry_t *entry,
                                                      uint32 *cdata)
{
    int rv = BCM_E_NONE;        /* Operation Return Status */
    _field_group_t *fg = NULL;
                                /* Field group info */
    _field_stage_t *stage_fc = NULL;
                                /* Stage field control info */
    soc_mem_t mem;              /* SOC Memory */

    /* Get class group info */
    fg = entry->group;

    /* Get group stage control structure. */
    rv = _field_stage_control_get(unit, fg->stage_id, &stage_fc);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                 "FP(unit %d) Error: Stage (%d) control get failure.\n"),
                 unit, fg->stage_id));
        return (rv);
    }

    /* Decide register based on operational mode */
    if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
        mem = SRC_COMPRESSIONm;
    } else {
        BCM_IF_ERROR_RETURN(_bcm_field_mem_instance_get(unit,
                                               SRC_COMPRESSIONm,
                                               fg->instance,
                                               &mem));
    }

    /* Read Memory */
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                                                entry->slice_idx, cdata));

    return (rv);
}

/*
 * Function:
 *  _field_th_class_dstcompression_install
 * Purpose:
 *  Install Dst Compression Class Entry In Hardware.
 * Parameters:
 *  unit        - (IN) BCM device number.
 *  entry       - (IN) Field Entry Info.
 * Returns:
 *  BCM_E_XXX
 * Notes:
 *  No Null Check for entry , group and key.
 *  Done by calling function _bcm_field_th_class_entry_hwinstall
 *  entry->slice_idx contains index to write
 *  entry->tcam.key contains data to write
 */
STATIC int
_field_th_class_dstcompression_install(int unit, _field_entry_t *entry)
{
    int rv = BCM_E_NONE;        /* Operation Return Status */
    _field_group_t *fg = NULL;
                                /* Field group info */
    _field_stage_t *stage_fc = NULL;
                                /* Stage field control info */
    soc_mem_t mem;              /* SOC Memory */

    /* Get class group info */
    fg = entry->group;

    /* Get group stage control structure. */
    rv = _field_stage_control_get(unit, fg->stage_id, &stage_fc);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                 "FP(unit %d) Error: Stage (%d) control get failure.\n"),
                 unit, fg->stage_id));
        return (rv);
    }

    /* Decide register based on operational mode */
    if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
        mem = DST_COMPRESSIONm;
    } else {
        BCM_IF_ERROR_RETURN(_bcm_field_mem_instance_get(unit,
                                               DST_COMPRESSIONm,
                                               fg->instance,
                                               &mem));
    }

    /* Write Memory */
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                                      entry->slice_idx, entry->tcam.key));

    return (rv);
}

/*
 * Function:
 *  _field_th_class_dstcompression_read
 * Purpose:
 *  Read Dst Compression Class Entry From Hardware.
 * Parameters:
 *  unit        - (IN) BCM device number.
 *  entry       - (IN) Field Entry Info.
 *  cdata       - (OUT) Field Class Entry DataBuffer.
 * Returns:
 *  BCM_E_XXX
 * Notes:
 *  No Null Check for entry, group and classdata buffer.
 *  Done by calling function _bcm_field_th_class_entry_hwread
 *  entry->slice_idx contains index to read
 *  cdata contains data read from hardware.
 */
STATIC int
_field_th_class_dstcompression_read(int unit, _field_entry_t *entry,
                                                       uint32 *cdata)
{
    int rv = BCM_E_NONE;        /* Operation Return Status */
    _field_group_t *fg = NULL;
                                /* Field group info */
    _field_stage_t *stage_fc = NULL;
                                /* Stage field control info */
    soc_mem_t mem;              /* SOC Memory */

    /* Get class group info */
    fg = entry->group;

    /* Get group stage control structure. */
    rv = _field_stage_control_get(unit, fg->stage_id, &stage_fc);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                 "FP(unit %d) Error: Stage (%d) control get failure.\n"),
                 unit, fg->stage_id));
        return (rv);
    }

    /* Decide register based on operational mode */
    if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
        mem = DST_COMPRESSIONm;
    } else {
        BCM_IF_ERROR_RETURN(_bcm_field_mem_instance_get(unit,
                                               DST_COMPRESSIONm,
                                               fg->instance,
                                               &mem));
    }

    /* Read Memory */
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                                     entry->slice_idx, cdata));

    return (rv);
}

/*
 * Function:
 *  _field_th_class_ttl_install
 * Purpose:
 *  Install TTL Class Entry In Hardware.
 * Parameters:
 *  unit        - (IN) BCM device number.
 *  entry       - (IN) Field Entry Info.
 * Returns:
 *  BCM_E_XXX
 * Notes:
 *  No Null Check for entry, group and key.
 *  Done by calling function _bcm_field_th_class_entry_hwinstall
 *  entry->slice_idx contains index to write
 *  entry->tcam.key contains data to write
 */
STATIC int
_field_th_class_ttl_install(int unit, _field_entry_t *entry)
{
    int rv = BCM_E_NONE;        /* Operation Return Status */
    _field_group_t *fg = NULL;
                                /* Field group info */
    _field_stage_t *stage_fc = NULL;
                                /* Stage field control info */
    soc_mem_t mem;              /* SOC Memory */

    /* Get class group info */
    fg = entry->group;

    /* Get group stage control structure. */
    rv = _field_stage_control_get(unit, fg->stage_id, &stage_fc);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                 "FP(unit %d) Error: Stage (%d) control get failure.\n"),
                 unit, fg->stage_id));
        return (rv);
    }

    /* Decide register based on operational mode */
    if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
        mem = TTL_FNm;
    } else {
        BCM_IF_ERROR_RETURN(_bcm_field_mem_instance_get(unit,
                                               TTL_FNm,
                                               fg->instance,
                                               &mem));
    }

    /* Write Memory */
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                                      entry->slice_idx, entry->tcam.key));

    return (rv);
}

/*
 * Function:
 *  _field_th_class_ttl_read
 * Purpose:
 *  Read TTL Class Entry From Hardware.
 * Parameters:
 *  unit        - (IN) BCM device number.
 *  entry       - (IN) Field Entry Info.
 *  cdata       - (OUT) Field Class Entry DataBuffer.
 * Returns:
 *  BCM_E_XXX
 * Notes:
 *  No Null Check for entry, group and classdata buffer.
 *  Done by calling function _bcm_field_th_class_entry_hwread
 *  entry->slice_idx contains index to read
 *  cdata contains data read from hardware.
 */
STATIC int
_field_th_class_ttl_read(int unit, _field_entry_t *entry,
                                            uint32 *cdata)
{
    int rv = BCM_E_NONE;        /* Operation Return Status */
    _field_group_t *fg = NULL;
                                /* Field group info */
    _field_stage_t *stage_fc = NULL;
                                /* Stage field control info */
    soc_mem_t mem;              /* SOC Memory */

    /* Get class group info */
    fg = entry->group;

    /* Get group stage control structure. */
    rv = _field_stage_control_get(unit, fg->stage_id, &stage_fc);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                 "FP(unit %d) Error: Stage (%d) control get failure.\n"),
                 unit, fg->stage_id));
        return (rv);
    }

    /* Decide register based on operational mode */
    if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
        mem = TTL_FNm;
    } else {
        BCM_IF_ERROR_RETURN(_bcm_field_mem_instance_get(unit,
                                               TTL_FNm,
                                               fg->instance,
                                               &mem));
    }

    /* Read Memory */
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                                     entry->slice_idx, cdata));

    return (rv);
}

/*
 * Function:
 *  _field_th_class_tos_install
 * Purpose:
 *  Install TOS Class Entry In Hardware.
 * Parameters:
 *  unit        - (IN) BCM device number.
 *  entry       - (IN) Field Entry Info.
 * Returns:
 *  BCM_E_XXX
 * Notes:
 *  No Null Check for entry, group and key.
 *  Done by calling function _bcm_field_th_class_entry_hwinstall
 *  entry->slice_idx contains index to write
 *  entry->tcam.key contains data to write
 */
STATIC int
_field_th_class_tos_install(int unit, _field_entry_t *entry)
{
    int rv = BCM_E_NONE;        /* Operation Return Status */
    _field_group_t *fg = NULL;
                                /* Field group info */
    _field_stage_t *stage_fc = NULL;
                                /* Stage field control info */
    soc_mem_t mem;              /* SOC Memory */

    /* Get class group info */
    fg = entry->group;

    /* Get group stage control structure. */
    rv = _field_stage_control_get(unit, fg->stage_id, &stage_fc);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                 "FP(unit %d) Error: Stage (%d) control get failure.\n"),
                 unit, fg->stage_id));
        return (rv);
    }

    /* Decide register based on operational mode */
    if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
        mem = TOS_FNm;
    } else {
        BCM_IF_ERROR_RETURN(_bcm_field_mem_instance_get(unit,
                                               TOS_FNm,
                                               fg->instance,
                                               &mem));
    }

    /* Write Memory */
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                                      entry->slice_idx, entry->tcam.key));

    return (rv);
}

/*
 * Function:
 *  _field_th_class_tos_read
 * Purpose:
 *  Read TOS Class Entry From Hardware.
 * Parameters:
 *  unit        - (IN) BCM device number.
 *  entry       - (IN) Field Entry Info.
 *  cdata       - (OUT) Field Class Entry DataBuffer.
 * Returns:
 *  BCM_E_XXX
 * Notes:
 *  No Null Check for entry, group and classdata buffer.
 *  Done by calling function _bcm_field_th_class_entry_hwread
 *  entry->slice_idx contains index to read
 *  cdata contains data read from hardware.
 */
STATIC int
_field_th_class_tos_read(int unit, _field_entry_t *entry,
                                            uint32 *cdata)
{
    int rv = BCM_E_NONE;        /* Operation Return Status */
    _field_group_t *fg = NULL;
                                /* Field group info */
    _field_stage_t *stage_fc = NULL;
                                /* Stage field control info */
    soc_mem_t mem;              /* SOC Memory */

    /* Get class group info */
    fg = entry->group;

    /* Get group stage control structure. */
    rv = _field_stage_control_get(unit, fg->stage_id, &stage_fc);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                 "FP(unit %d) Error: Stage (%d) control get failure.\n"),
                 unit, fg->stage_id));
        return (rv);
    }

    /* Decide register based on operational mode */
    if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
        mem = TOS_FNm;
    } else {
        BCM_IF_ERROR_RETURN(_bcm_field_mem_instance_get(unit,
                                               TOS_FNm,
                                               fg->instance,
                                               &mem));

    }
    /* Read Memory */
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                                     entry->slice_idx, cdata));

    return (rv);
}

/*
 * Function:
 *  _field_th_class_ipproto_install
 * Purpose:
 *  Install IP Protocol Class Entry In Hardware.
 * Parameters:
 *  unit        - (IN) BCM device number.
 *  entry       - (IN) Field Entry Info.
 * Returns:
 *  BCM_E_XXX
 * Notes:
 *  No Null Check for entry , group and key.
 *  Done by calling function _bcm_field_th_class_entry_hwinstall
 *  entry->slice_idx contains index to write
 *  entry->tcam.key contains data to write
 */
STATIC int
_field_th_class_ipproto_install(int unit, _field_entry_t *entry)
{
    int rv = BCM_E_NONE;        /* Operation Return Status */
    _field_group_t *fg = NULL;
                                /* Field group info */
    _field_stage_t *stage_fc = NULL;
                                /* Stage field control info */
    soc_mem_t mem;              /* SOC Memory */

    /* Get class group info */
    fg = entry->group;

    /* Get group stage control structure. */
    rv = _field_stage_control_get(unit, fg->stage_id, &stage_fc);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                 "FP(unit %d) Error: Stage (%d) control get failure.\n"),
                 unit, fg->stage_id));
        return (rv);
    }

    /* Decide register based on operational mode */
    if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
        mem = IP_PROTO_MAPm;
    } else {
        BCM_IF_ERROR_RETURN(_bcm_field_mem_instance_get(unit,
                                               IP_PROTO_MAPm,
                                               fg->instance,
                                               &mem));
    }

    /* Write Memory */
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                                      entry->slice_idx, entry->tcam.key));

    return (rv);
}

/*
 * Function:
 *  _field_th_class_ipproto_read
 * Purpose:
 *  Read IP Protocol Class Entry From Hardware.
 * Parameters:
 *  unit        - (IN) BCM device number.
 *  entry       - (IN) Field Entry Info.
 *  cdata       - (OUT) Field Class Entry DataBuffer.
 * Returns:
 *  BCM_E_XXX
 * Notes:
 *  No Null Check for entry, group and classdata buffer.
 *  Done by calling function _bcm_field_th_class_entry_hwread
 *  entry->slice_idx contains index to read
 *  cdata contains data read from hardware.
 */
 STATIC int
_field_th_class_ipproto_read(int unit, _field_entry_t *entry,
                                                uint32 *cdata)
{
    int rv = BCM_E_NONE;        /* Operation Return Status */
    _field_group_t *fg = NULL;
                                /* Field group info */
    _field_stage_t *stage_fc = NULL;
                                /* Stage field control info */
    soc_mem_t mem;              /* SOC Memory */

    /* Get class group info */
    fg = entry->group;

    /* Get group stage control structure. */
    rv = _field_stage_control_get(unit, fg->stage_id, &stage_fc);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                 "FP(unit %d) Error: Stage (%d) control get failure.\n"),
                 unit, fg->stage_id));
        return (rv);
    }

    /* Decide register based on operational mode */
    if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
        mem = IP_PROTO_MAPm;
    } else {
        BCM_IF_ERROR_RETURN(_bcm_field_mem_instance_get(unit,
                                               IP_PROTO_MAPm,
                                               fg->instance,
                                               &mem));
    }

    /* Read Memory */
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                                     entry->slice_idx, cdata));

    return (rv);
}

/*
 * Function:
 *  _field_th_class_tcp_install
 * Purpose:
 *  Install TCP Class Entry In Hardware.
 * Parameters:
 *  unit        - (IN) BCM device number.
 *  entry       - (IN) Field Entry Info.
 * Returns:
 *  BCM_E_XXX
 * Notes:
 *  No Null Check for entry , group and key.
 *  Done by calling function _bcm_field_th_class_entry_hwinstall
 *  entry->slice_idx contains index to write
 *  entry->tcam.key contains data to write
 */
STATIC int
_field_th_class_tcp_install(int unit, _field_entry_t *entry)
{
    int rv = BCM_E_NONE;        /* Operation Return Status */
    _field_group_t *fg = NULL;
                                /* Field group info */
    _field_stage_t *stage_fc = NULL;
                                /* Stage field control info */
    soc_mem_t mem;              /* SOC Memory */

    /* Get class group info */
    fg = entry->group;

    /* Get group stage control structure. */
    rv = _field_stage_control_get(unit, fg->stage_id, &stage_fc);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                 "FP(unit %d) Error: Stage (%d) control get failure.\n"),
                 unit, fg->stage_id));
        return (rv);
    }

    /* Decide register based on operational mode */
    if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
        mem = TCP_FNm;
    } else {
        BCM_IF_ERROR_RETURN(_bcm_field_mem_instance_get(unit,
                                               TCP_FNm,
                                               fg->instance,
                                               &mem));
    }

    /* Write Memory */
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                                      entry->slice_idx, entry->tcam.key));

    return (rv);
}

/*
 * Function:
 *  _field_th_class_tcp_read
 * Purpose:
 *  Read TCP Class Entry From Hardware.
 * Parameters:
 *  unit        - (IN) BCM device number.
 *  entry       - (IN) Field Entry Info.
 *  cdata       - (OUT) Field Class Entry DataBuffer.
 * Returns:
 *  BCM_E_XXX
 * Notes:
 *  No Null Check for entry, group and classdata buffer.
 *  Done by calling function _bcm_field_th_class_entry_hwread
 *  entry->slice_idx contains index to read
 *  cdata contains data read from hardware.
 */
STATIC int
_field_th_class_tcp_read(int unit, _field_entry_t *entry,
                                            uint32 *cdata)
{
    int rv = BCM_E_NONE;        /* Operation Return Status */
    _field_group_t *fg = NULL;
                                /* Field group info */
    _field_stage_t *stage_fc = NULL;
                                /* Stage field control info */
    soc_mem_t mem;              /* SOC Memory */

    /* Get class group info */
    fg = entry->group;

    /* Get group stage control structure. */
    rv = _field_stage_control_get(unit, fg->stage_id, &stage_fc);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                 "FP(unit %d) Error: Stage (%d) control get failure.\n"),
                 unit, fg->stage_id));
        return (rv);
    }

    /* Decide register based on operational mode */
    if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
        mem = TCP_FNm;
    } else {
        BCM_IF_ERROR_RETURN(_bcm_field_mem_instance_get(unit,
                                               TCP_FNm,
                                               fg->instance,
                                               &mem));
    }

    /* Read Memory */
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                                     entry->slice_idx, cdata));

    return (rv);
}
/*
 * Function:
 *  _field_th_class_iptunnelttl_install
 * Purpose:
 *  Install IP Tunnel TTL Class Entry In Hardware.
 * Parameters:
 *  unit        - (IN) BCM device number.
 *  entry       - (IN) Field Entry Info.
 * Returns:
 *  BCM_E_XXX
 * Notes:
 *  No Null Check for entry, group and key.
 *  Done by calling function _bcm_field_th_class_entry_hwinstall
 *  entry->slice_idx contains index to write
 *  entry->tcam.key contains data to write
 */
STATIC int
_field_th_class_iptunnelttl_install(int unit, _field_entry_t *entry)
{
    int rv = BCM_E_NONE;        /* Operation Return Status */
    _field_group_t *fg = NULL;
                                /* Field group info */
    _field_stage_t *stage_fc = NULL;
                                /* Stage field control info */
    soc_mem_t mem;              /* SOC Memory */

    /* Get class group info */
    fg = entry->group;

    /* Get group stage control structure. */
    rv = _field_stage_control_get(unit, fg->stage_id, &stage_fc);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                 "FP(unit %d) Error: Stage (%d) control get failure.\n"),
                 unit, fg->stage_id));
        return (rv);
    }

    /* Decide register based on operational mode */
    if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
        mem = ALT_TTL_FNm;
    } else {
        BCM_IF_ERROR_RETURN(_bcm_field_mem_instance_get(unit,
                                               ALT_TTL_FNm,
                                               fg->instance,
                                               &mem));
    }

    /* Write Memory */
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                                      entry->slice_idx, entry->tcam.key));

    return (rv);
}

/*
 * Function:
 *  _field_th_class_iptunnelttl_read
 * Purpose:
 *  Read Ip Tunnel TTL Class Entry From Hardware.
 * Parameters:
 *  unit        - (IN) BCM device number.
 *  entry       - (IN) Field Entry Info.
 *  cdata       - (OUT) Field Class Entry DataBuffer.
 * Returns:
 *  BCM_E_XXX
 * Notes:
 *  No Null Check for entry, group and classdata buffer.
 *  Done by calling function _bcm_field_th_class_entry_hwread
 *  entry->slice_idx contains index to read
 *  cdata contains data read from hardware.
 */
STATIC int
_field_th_class_iptunnelttl_read(int unit, _field_entry_t *entry,
                                            uint32 *cdata)
{
    int rv = BCM_E_NONE;        /* Operation Return Status */
    _field_group_t *fg = NULL;
                                /* Field group info */
    _field_stage_t *stage_fc = NULL;
                                /* Stage field control info */
    soc_mem_t mem;              /* SOC Memory */

    /* Get class group info */
    fg = entry->group;

    /* Get group stage control structure. */
    rv = _field_stage_control_get(unit, fg->stage_id, &stage_fc);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                 "FP(unit %d) Error: Stage (%d) control get failure.\n"),
                 unit, fg->stage_id));
        return (rv);
    }

    /* Decide register based on operational mode */
    if (stage_fc->oper_mode == bcmFieldGroupOperModeGlobal) {
        mem = ALT_TTL_FNm;
    } else {
        BCM_IF_ERROR_RETURN(_bcm_field_mem_instance_get(unit,
                                               ALT_TTL_FNm,
                                               fg->instance,
                                               &mem));
    }

    /* Read Memory */
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                                     entry->slice_idx, cdata));

    return (rv);
}


/*
 * Function:
 *  _bcm_field_th_class_entry_remove
 * Purpose:
 *  Remove Field Class Entry In Hardware.
 * Parameters:
 *  unit     - (IN) BCM device number.
 *  entry    - (IN) Field Entry Id.
 * Returns:
 *  BCM_E_XXX
 */
int
_bcm_field_th_class_entry_remove(int unit, bcm_field_entry_t entry)
{
    int rv = BCM_E_NONE;            /* Operation Return Status */
    _field_group_t *fg = NULL;      /* Field group info */
    _field_entry_t *f_ent = NULL;   /* Field entry info */
    _field_class_type_t ctype;      /* Field Class Type */
    _field_class_info_t **class_status_arr;
                                    /* Field Per Pipe Class Status Array */
    _field_class_info_t *class_status_info;
                                    /* Field Per Compression table status */
    _field_stage_t *stage_fc = NULL;
                                    /* Stage field control info */

    /* Confirm that 'entry' is  present on unit */
    if (BCM_FAILURE(_field_entry_get(unit, entry, _FP_ENTRY_PRIMARY, &f_ent))) {
        LOG_ERROR(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                 "FP(unit %d) Error: entry=(%d) already exists.\n"),
                 unit, entry));
        return (BCM_E_EXISTS);
    }

    /* Retreive Field Group and Action */
    fg = f_ent->group;

    /* Check if group is associated */
    if (fg == NULL) {
        return (BCM_E_INTERNAL);
    }

    /* Get group stage control structure. */
    rv = _field_stage_control_get(unit, fg->stage_id, &stage_fc);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                 "FP(unit %d) Error: Stage (%d) control get failure.\n"),
                 unit, fg->stage_id));
        return (rv);
    }

    /* Entry not installed in hardware to perform remove operation. */
    if (!(f_ent->flags & _FP_ENTRY_INSTALLED)) {
        return (BCM_E_NONE);
    }

    /* Entry hardware index not present */
    if (f_ent->slice_idx == -1) {
        return (BCM_E_INTERNAL);
    }

    /* Retreive Class Type from entry qset */
    rv = _bcm_field_th_class_type_qset_get(unit, &fg->qset, &ctype);
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    if (f_ent->tcam.key != NULL) {
        sal_memset(f_ent->tcam.key, 0, f_ent->tcam.key_size);
    } else {
        _FP_XGS3_ALLOC(f_ent->tcam.key,
                       f_ent->tcam.key_size, "field class entry key");
        if (f_ent->tcam.key == NULL) {
            LOG_ERROR(BSL_LS_BCM_FP,
                     (BSL_META_U(unit,
                     "FP(unit %d) Error: allocation failure for "
                     "class entry key.\n"),
                     unit));
            return (BCM_E_MEMORY);
        }
    }

    /* Update Class Status Info Structure */
    class_status_arr = stage_fc->class_info_arr[fg->instance];
    if (class_status_arr == NULL) {
        return (BCM_E_INTERNAL);
    }
    class_status_info = class_status_arr[ctype];
    if (class_status_info == NULL) {
        return (BCM_E_INTERNAL);
    }

    rv = _bcm_field_th_class_entry_hwinstall(unit, f_ent, ctype);
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    _FP_CLASS_TABLE_BMP_REMOVE(class_status_info->class_bmp, f_ent->slice_idx);
    f_ent->slice_idx = -1;
    f_ent->flags &= ~(_FP_ENTRY_INSTALLED);

    return (rv);
}

/*
 * Function:
 *  _bcm_field_th_class_qual_support_check
 * Purpose:
 *  Check if the qualifier is supported in the stage class
 * Parameters:
 *  unit - (IN) BCM device number.
 *  qual - (IN) Qualifier to check.
 * Returns:
 *  TRUE - Qualifier is supported by the stage
 *  FALSE - Qualifier is NOT supported by the stage
 */
int
_bcm_field_th_class_qual_support_check(int unit, int qual)
{
    switch(qual)
    {
        case bcmFieldQualifyStageClass:
        case bcmFieldQualifyIpProtocolClass:
        case bcmFieldQualifyEtherTypeClass:
        case bcmFieldQualifyL4SrcPortClass:
        case bcmFieldQualifyL4DstPortClass:
        case bcmFieldQualifySrcIpClass:
        case bcmFieldQualifySrcIpClassMsbNibble:
        case bcmFieldQualifySrcIpClassLower:
        case bcmFieldQualifySrcIpClassUpper:
        case bcmFieldQualifySrcIp6Class:
        case bcmFieldQualifySrcIp6ClassMsbNibble:
        case bcmFieldQualifySrcIp6ClassLower:
        case bcmFieldQualifySrcIp6ClassUpper:
        case bcmFieldQualifyFcoeOxIDClass:
        case bcmFieldQualifyDstIpClassMsbNibble:
        case bcmFieldQualifyDstIpClass:
        case bcmFieldQualifyDstIpClassLower:
        case bcmFieldQualifyDstIpClassUpper:
        case bcmFieldQualifyDstIp6Class:
        case bcmFieldQualifyDstIp6ClassMsbNibble:
        case bcmFieldQualifyDstIp6ClassLower:
        case bcmFieldQualifyDstIp6ClassUpper:
        case bcmFieldQualifyFcoeRxIDClass:
        case bcmFieldQualifyFibreChanSrcIdClass:
        case bcmFieldQualifyFibreChanSrcIdClassMsbNibble:
        case bcmFieldQualifyFibreChanSrcIdClassLower:
        case bcmFieldQualifyFibreChanSrcIdClassUpper:
        case bcmFieldQualifyFibreChanDstIdClass:
        case bcmFieldQualifyFibreChanDstIdClassMsbNibble:
        case bcmFieldQualifyFibreChanDstIdClassLower:
        case bcmFieldQualifyFibreChanDstIdClassUpper:
        case bcmFieldQualifyTcpClassZero:
        case bcmFieldQualifyTosClassZero:
        case bcmFieldQualifyTtlClassZero:
        case bcmFieldQualifyIpTunnelTtlClassZero:
        case bcmFieldQualifyTcpClassOne:
        case bcmFieldQualifyTosClassOne:
        case bcmFieldQualifyTtlClassOne:
            return TRUE;
        default:
            ;
    }
    return FALSE;
}

#else /* BCM_TOMAHAWK_SUPPORT && BCM_FIELD_SUPPORT */
typedef int _th_field_not_empty; /* Make ISO compilers happy. */
#endif /* !BCM_TOMAHAWK_SUPPORT && !BCM_FIELD_SUPPORT */
