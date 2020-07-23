/** \file include/bcm_int/dnx/cosq/flow_control/cosq_fc.h
 * 
 *
 * Flow control API functions for DNX. \n
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _SRC_DNX_FC_H_INCLUDED_
/** { */
#define _SRC_DNX_FC_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/cosq.h>

 /*
  * Typedefs
  * {
  */

/**
 * \brief - Type definition for the functions in the generation/reception matrix
 */
typedef shr_error_e(
    *fc_cb_t) (
    int,
    int,
    bcm_cosq_fc_endpoint_t *,
    bcm_cosq_fc_endpoint_t *);

/**
 * \brief - Type definition for the generation/reception matrix
 */
typedef struct fc_matrix
{
    fc_cb_t map;
    fc_cb_t verify;
} fc_matrix_t;

/**
 * \brief - Enumerator for the OOB port modes
 */
typedef enum
{
        /** Disabled */
    DNX_FC_CAL_MODE_DISABLE = 0,
        /** RX enabled */
    DNX_FC_CAL_MODE_RX_ENABLE = 1,
        /** TX enabled */
    DNX_FC_CAL_MODE_TX_ENABLE = 2,
        /** Both RX and TX enabled */
    DNX_FC_CAL_MODE_TX_RX_ENABLE = 3,
        /** Number of OOB port modes */
    DNX_FC_CAL_MODES
} dnx_fc_cal_medes_t;

/**
 * \brief - Enumerator for the Inband ILKN LLFC modes
 */
typedef enum
{
        /** Disabled */
    DNX_FC_INB_ILKN_CAL_LLFC_MODE_DISABLE = 0,
        /** LLFC on channel 0 of the ILKN */
    DNX_FC_INB_ILKN_CAL_LLFC_MODE_CH_0_ONLY = 1,
        /** LLFC on every 16th channel of the ILKN */
    DNX_FC_INB_ILKN_CAL_LLFC_MODE_EVERY_16_CH = 2,
        /** LLFC on all channels of the ILKN */
    DNX_FC_INB_ILKN_CAL_LLFC_MODE_ALL_CH = 3,
        /** Number of Inband ILKN LLFC modes */
    DNX_FC_INB_ILKN_CAL_LLFC_MODES
} dnx_fc_inb_ilkn_cal_llfc_medes_t;

/**
 * \brief - Enumerator for Flow control generation sources
 */
typedef enum
{
        /** Global resources */
    DNX_FC_GEN_SRC_GLB_RES = 0,
        /** LLFC VSQs */
    DNX_FC_GEN_SRC_LLFC_VSQ = 1,
        /** PFC VSQ */
    DNX_FC_GEN_SRC_PFC_VSQ = 2,
        /** Category 2 and Traffic Class */
    DNX_FC_GEN_SRC_CAT2_TC_VSQ = 3,
        /** VSQs from A to D */
    DNX_FC_GEN_SRC_VSQ_A_D = 4,
        /** Almost full NIF */
    DNX_FC_GEN_SRC_ALMOST_FULL_NIF = 5,
        /** Number of generation sources */
    DNX_FC_GENERATION_SOURCES
} dnx_fc_gen_src_t;

/**
 * \brief - Enumerator for Flow control generation destinations
 */
typedef enum
{
        /** LLFC */
    DNX_FC_GEN_DEST_LLFC = 0,
        /** PFC */
    DNX_FC_GEN_DEST_PFC = 1,
        /** Calendar */
    DNX_FC_GEN_DEST_CAL = 2,
        /** Number of generation destinations */
    DNX_FC_GENERATION_DESTINATIONS
} dnx_fc_gen_dest_t;

/**
 * \brief - Enumerator for Flow control reception sources
 */
typedef enum
{
        /** LLFC */
    DNX_FC_REC_SRC_LLFC = 0,
        /** PFC */
    DNX_FC_REC_SRC_PFC = 1,
        /** Calendar(OOB, COE) */
    DNX_FC_REC_SRC_CAL = 2,
        /** Number of reception sources */
    DNX_FC_RECEPTION_SOURCES
} dnx_fc_rec_src_t;

/**
 * \brief - Enumerator for Flow control reception reaction points
 */
typedef enum
{
        /** Interface */
    DNX_FC_REC_REACTION_POINT_INTERFACE = 0,
        /** Port */
    DNX_FC_REC_REACTION_POINT_PORT = 1,
        /** Port + COSQ */
    DNX_FC_REC_REACTION_POINT_PORT_COSQ = 2,
        /** Number of reception reaction points */
    DNX_FC_RECEPTION_REACTION_POINTS
} dnx_fc_rec_reaction_point_t;

/**
 * \brief - Enumerator for Flow control COE calendar modes
 */
typedef enum
{
    DNX_FC_COE_MODE_PAUSE = 0,
    DNX_FC_COE_MODE_PFC = 1,
    DNX_FC_NOF_COE_MODES = 2
} DNX_FC_COE_MODE;

 /*
  * }
  */

#endif /** _SRC_DNX_FC_H_INCLUDED_ */
