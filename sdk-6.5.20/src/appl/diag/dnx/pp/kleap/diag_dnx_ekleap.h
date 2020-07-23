/**
 * \file diag_dnx_ekleap.h
 * Purpose: External declarations for command functions and
 *          their associated usage strings.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DIAG_DNX_EKLEAP_H_INCLUDED
/* { */
#define DIAG_DNX_EKLEAP_H_INCLUDED

/*************
 * INCLUDES  *
 */

/*
 * DEFINES
 * {
 */
/** \brief Keyword that's used as stage selection argument  */
#define EKLEAP_ARG_KW_STAGE "Stage"
/** \brief Keyword that's used to get full details in case stage isn't specified */
#define EKLEAP_ARG_KW_EXTENDED "Full"
/** \brief Slash separated list for stages handled in this diag  */
#define EKLEAP_STAGE_LIST_STR "Parser/PRP/Term/Fwd/Enc1(2,3,4,5)/Trap/Btc"
/** \brief Stage information separator in printed tables */
#define EKLEAP_PRT_STAGE_SEP PRT_ROW_SEP_UNDERSCORE_BEFORE
/** \brief Max # of global out-lif pointers */
#define EKLEAP_GLOBAL_OUT_LIF_MAX_NOF 4
/** \brief Max # of ETPS entries to read */
#define EKLEAP_ETPS_ENTRIES_MAX 10
/** \brief # ESEM lookups in Term stage */
#define EKLEAP_TERM_ESEM_ACC_NOF 2
/** \brief # Gen Header MAX size in bytes */
#define EKLEAP_GEN_HEADER_MAX_VISIBLE_SIZE_BYTES 160

/*
 * }
 */

/*
 * TYPES
 * {
 */
/**
 * \brief
 *   Stage enumeration for the sake of eKLeaP command
 */
typedef enum
{
    /*
     * Must be first
     */
    EKLEAP_STAGE_FIRST = 0,

    EKLEAP_STAGE_PARSER = EKLEAP_STAGE_FIRST,
    EKLEAP_STAGE_PRP1,
    EKLEAP_STAGE_PRP2,
    EKLEAP_STAGE_TERM,
    EKLEAP_STAGE_FWD,
    EKLEAP_STAGE_ENC1,
    EKLEAP_STAGE_ENC2,
    EKLEAP_STAGE_ENC3,
    EKLEAP_STAGE_ENC4,
    EKLEAP_STAGE_ENC5,
    EKLEAP_STAGE_TRAP,
    EKLEAP_STAGE_BTC,
    EKLEAP_STAGE_OUT_PORT,
    /*
     * Must be last
     */
    EKLEAP_STAGE_NOF
} ekleap_stage_e;


/**
 * \brief
 *   ETPS entry extended signal information
 */
typedef struct
{
    signal_output_t *entry_sig;
    signal_output_t *data_sig;
} etps_entry_info_t;
/**
 * \brief
 *   ETPS entry data for analysis and display
 */
typedef struct
{
    rhlist_t *sig_list;
    etps_entry_info_t entry_signals[EKLEAP_ETPS_ENTRIES_MAX];
    int entries_nof;
} ekleap_encap_stack_signal_data_t;
/**
 * \brief
 *   Shared data required for all stages.
 */
typedef struct
{
    /** \brief Valid ETPS entries signals at FWD stage */
    ekleap_encap_stack_signal_data_t etps_data;
    /** \brief ETPS shift counter */
    int shifted_etps;
} ekleap_shared_data_t;
/**
 * \brief
 *   Generic stage data container
 */
typedef struct
{
    ekleap_stage_e stage_id;
    char *stage_name;
    dbal_tables_e ctx_table_id;
    dbal_fields_e ctx_field_id;
    dbal_fields_e etps_shift_field_id;
    uint32 etps_shift;
} ekleap_stage_info_t;
/*
 * }
 */

/*
 * Utilities
 * {
 */

/**
 * \brief - get context related information per given egress stage 
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] stage_id - stage id
 *   \param [out] stage_info - stage information.
 *
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e ekleap_stage_info_get(
    int unit,
    ekleap_stage_e stage_id,
    ekleap_stage_info_t * stage_info);

/**
 * \brief - get and store etps information
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core - core id
 *   \param [out] data - etps data.
 *
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
shr_error_e ekleap_shared_data_get(
    int unit,
    int core,
    ekleap_shared_data_t * data);

/*
 * }
 */

/* } */
#endif /* DIAG_DNX_EKLEAP_H_INCLUDED */
