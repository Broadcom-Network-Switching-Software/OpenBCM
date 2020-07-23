/*! \file bcmtm_tdm_vmap.c
 *
 * TDM vmap functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef _TDM_STANDALONE
    #include <bcmtm_tdm_top.h>
#else
    #include <bcmtm/tdm/bcmtm_tdm_top.h>
#endif


/****************************************************************************
 * Defines.
 */
#define TDM_VMAP_TOKEN_UNUSED 0
#define TDM_VMAP_PM_LANES     32
#define TDM_VMAP_R_A_ARR_LEN  2

/****************************************************************************
 * Typedefs.
 */

/*!
 * \brief TDM vmap pm info.
 */
typedef struct tdm_vmap_pm_s {
    /*!< PM enable. */
    int pm_en;

    /*!< PM number/index. */
    int pm_num;

    /*!< Reserved PM slots. */
    int pm_slots;

    /*!< PM mode. */
    int pm_mode;

    /*!< Active subport count. */
    int subport_cnt;

    /*!< Subport buffer.  */
    int subport_phy[TDM_VMAP_PM_LANES];

    /*!< Required subport slots. */
    int subport_slot_req[TDM_VMAP_PM_LANES];

    /*!< Reserved subport slots. */
    int subport_slot_rsv[TDM_VMAP_PM_LANES];
} tdm_vmap_pm_t;

/*!
 * \brief TDM vmap pmlist info.
 */
typedef struct tdm_vmap_pmlist_s {
    /*!< Number of actual vmap PMs. */
    int num_pms;

    /*!< Number of max vmap PMs. */
    int num_pms_limit;

    /*!< Pointer of vmpa pms. */
    tdm_vmap_pm_t *pms;
} tdm_vmap_pmlist_t;


/*!
 * \brief TDM vmap calendar info.
 */
typedef struct tdm_vmap_s{
    /*!< Number of actual columns in vmap. */
    int wid;

    /*!< Number of actual rows in vmap. */
    int len;

    /*!< Number of max columns in vmap. */
    int wid_max;

    /*!< Number of max rows in vmap. */
    int len_max;

    /*!< Pionter of vmap table. */
    int **tbl;

    /*!< Pointer of r_a_array. */
    int **r_a_arr;
} tdm_vmap_t;


/****************************************************************************
 * Private functions.
 */
/*!
 * \brief Roundup division.
 *
 * \param [in] a Numerator.
 * \param [in] b Denominator.
 *
 * \retval Quotient.
 */
static int
bcmtm_tdm_math_div_ceil(int a, int b)
{
    return ((a > 0 && b > 0) ? (a / b + ((a % b) != 0)) : 0);
}

/*!
 * \brief Roundown division.
 *
 * \param [in] a Numerator.
 * \param [in] b Denominator.
 *
 * \retval Quotient.
 */
static int
bcmtm_tdm_math_div_floor(int a, int b)
{
	return ((a > 0 && b > 0) ? (a / b) : 0);
}

/*!
 * \brief Print vmap.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] vmap Pointer of vmap structure.
 *
 * \return Nothing.
 */
static void
bcmtm_tdm_vmap_print(tdm_mod_t *tdm, tdm_vmap_t *vmap)
{
    int i, j, cfg_token_empty, slot_cnt, slot_cnt_sum = 0;

    /* Print vmap. */
    TDM_PRINT0("\t TDM VMAP \n");
    TDM_PRINT2("%6s\t%6s\t", " ", " ");
    for (i = 0; i < vmap->wid; i++) TDM_PRINT1("%3d\t", i);
    TDM_PRINT0("\n");
    cfg_token_empty = bcmtm_tdm_token_empty_get(tdm);
    for (j = 0; j < vmap->len; j++) {
        slot_cnt = 0;
        for (i = 0; i < vmap->wid; i++) {
            if (vmap->tbl[i][j] != cfg_token_empty) slot_cnt++;
        }
        if (slot_cnt > 1) {
            TDM_PRINT1("%6s\t", "[TDM_FAIL]");
        } else {
            TDM_PRINT1("%6s\t", " ");
        }

        TDM_PRINT1("%3d : \t", j);
        for (i = 0; i < vmap->wid; i++) {
            if (vmap->tbl[i][j] != cfg_token_empty) {
                TDM_PRINT1("%3d\t", vmap->tbl[i][j]);
            } else {
                TDM_PRINT0("---\t");
            }
        }
        TDM_PRINT0("\n");
        slot_cnt_sum += slot_cnt;
    }

    /* Print overall allocated slots. */
    TDM_PRINT2("\nVMAP slot summary: allocated %3d (limit %3d)\n",
               slot_cnt_sum, vmap->len);
    TDM_SML_BAR
}

/*!
 * \brief Print pmlist.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] pmlist Pointer of vmap pmlist structure.
 *
 * \return Nothing.
 */
static void
bcmtm_tdm_pmlist_print(tdm_mod_t *tdm, tdm_vmap_pmlist_t *pmlist)
{
    int i, j, num_pms;
    tdm_vmap_pm_t *pms = NULL;

    if (tdm != NULL && pmlist != NULL) {
        pms = pmlist->pms;
        num_pms = pmlist->num_pms_limit;
        for (i = 0; i < num_pms; i++) {
            if (pms[i].pm_en == 0) break;
            TDM_PRINT3("TDM: PM[%2d] %2d, port_cnt %d, port [",
                       i, pms[i].pm_num, pms[i].subport_cnt);
            for (j = 0; j < pms[i].subport_cnt; j++) {
                TDM_PRINT1("%3d, ",  pms[i].subport_phy[j]);
            }
            TDM_PRINT0("], speed [");
            for (j = 0; j < pms[i].subport_cnt; j++) {
                TDM_PRINT1("%3d, ", (bcmtm_tdm_cfg_port_speed_get(tdm,
                           pms[i].subport_phy[j], 0) / 1000));
            }
            TDM_PRINT0("]\n");
        }
        TDM_SML_BAR
    }
}

/*!
 * \brief Print pipe info.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \return Nothing.
 */
static void
bcmtm_tdm_pipe_print(tdm_mod_t *tdm)
{
    int lr_slot_cnt = 0, os_slot_cnt = 0, lr_prt_cnt = 0, os_prt_cnt = 0;
    int cfg_slot_unit, cfg_pipe_id, cfg_lr_limit, cfg_ancl_num, cfg_cal_len;

    /* Get parameters. */
    cfg_slot_unit = bcmtm_tdm_pipe_slot_unit_get(tdm);
    cfg_pipe_id = bcmtm_tdm_pipe_id_get(tdm);
    cfg_lr_limit = bcmtm_tdm_pipe_lr_limit_get(tdm);
    cfg_ancl_num = bcmtm_tdm_pipe_ancl_limit_get(tdm);
    cfg_cal_len = bcmtm_tdm_lr_cal_len_get(tdm, cfg_pipe_id, 0);

    /* Calculate total number of slots required by linerate ports. */
    if (bcmtm_tdm_pipe_lr_en_get(tdm) == TDM_TRUE) {
        int i = 0;
        int *lr_buff;

        lr_prt_cnt = bcmtm_tdm_pipe_lr_cnt_get(tdm);
        lr_buff = bcmtm_tdm_pipe_lr_buff_get(tdm);
        for (i = 0; i < lr_prt_cnt; i++) {
            lr_slot_cnt += bcmtm_tdm_port_slots_get(tdm, lr_buff[i], 0);
        }
    }

    /* Calculate total number of slots available for oversub ports. */
    if (bcmtm_tdm_pipe_os_en_get(tdm) == TDM_TRUE) {
        os_slot_cnt = cfg_lr_limit - lr_slot_cnt;
        os_prt_cnt = bcmtm_tdm_pipe_os_cnt_get(tdm);
    }

    /* Print pipe bandwidth info. */
    TDM_BIG_BAR
    TDM_PRINT1("TDM: Populating VMAP for PIPE %0d\n\n", cfg_pipe_id);
    TDM_PRINT1("\t [SLOT_UNIT: %0dMbps]\n", cfg_slot_unit);
    TDM_PRINT1("\t -- number of lineRate ports: %3d\n", lr_prt_cnt);
    TDM_PRINT1("\t -- number of OverSub  ports: %3d\n\n", os_prt_cnt);
    TDM_PRINT1("\t -- cal_len : %3d\n", cfg_cal_len);
    TDM_PRINT1("\t\t -- ancl_limit : %3d\n", cfg_ancl_num);
    TDM_PRINT1("\t\t -- lr_os_limit: %3d\n", cfg_lr_limit);
    TDM_PRINT1("\t\t\t -- number of linerate slots: %3d\n", lr_slot_cnt);
    TDM_PRINT1("\t\t\t -- number of oversub  slots: %3d\n", os_slot_cnt);
    TDM_SML_BAR

    /* Check io bandwidth against core bandwidth. */
    if (lr_slot_cnt > cfg_lr_limit) {
        TDM_WARN3("TDM: %s, [lr_slot_req %d, lr_slot_limit %d]\n",
                  "Linerate bandwidth overflow in vmap allocation",
                  lr_slot_cnt, cfg_lr_limit);
    }
}

/*!
 * \brief Vmap singularity check.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] vmap Pointer of vmap structure.
 *
 * \retval TDM_PASS Vmap singularity check passed.
 * \retval TDM_FAIL Vmap singularity check failed.
 */
static int
bcmtm_tdm_vmap_singularity_chk(tdm_mod_t *tdm, tdm_vmap_t *vmap)
{
    int result = TDM_PASS;

    if (bcmtm_tdm_cfg_chk_en_get(tdm)) {
        int i, j, cfg_token_empty, slot_cnt;
        TDM_PRINT0("TDM: Check VMAP singularity\n\n");
        cfg_token_empty = bcmtm_tdm_token_empty_get(tdm);
        for (j = 0; j < vmap->len; j++) {
            slot_cnt = 0;
            for (i = 0; i < vmap->wid; i++) {
                if (vmap->tbl[i][j] != cfg_token_empty) slot_cnt++;
            }
            if (slot_cnt > 1) {
                TDM_ERROR1("FAILED in vmap singularity check, row %03d\n", j);
                result = TDM_FAIL;
            }
        }
        TDM_SML_BAR
    }

    return result;
}

/*!
 * \brief Calendar port space check.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal Pointer of linerate calendar array.
 * \param [in] cal_len length of linerate calendar array.
 * \param [in] espace Extra space.
 * \param [in] pos Slot position.
 * \param [in] dir Moving direction.
 *
 * \retval TDM_PASS Port space check passed.
 * \retval TDM_FAIL Port space check failed.
 */
static int
bcmtm_tdm_cal_port_space_chk(tdm_mod_t *tdm, int *cal, int cal_len,
                             int espace, int pos, int dir)
{
    int d1, d2, result = TDM_PASS;

    if (tdm != NULL && cal != NULL && pos >= 0 && pos < cal_len) {
        if (bcmtm_tdm_fport_chk(tdm, cal[pos])) {
            d1 = bcmtm_tdm_slot_same_dist_get(tdm, cal, cal_len,
                                                   espace, pos, dir);
            d2 = bcmtm_tdm_slot_sister_dist_get(tdm, cal, cal_len,
                                                     espace, pos, dir);
            if (d1 != 0 || d2 != 0) result = TDM_FAIL;
        }
    }

    return result;
}

/*!
 * \brief Calendar sister port space check.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval TDM_PASS Sister port space check passed.
 * \retval TDM_FAIL Sister port space check failed.
 */
static int
bcmtm_tdm_cal_sister_chk(tdm_mod_t *tdm)
{
    int result = TDM_PASS;
    int cfg_space_sister, cfg_lr_en;

    cfg_lr_en = bcmtm_tdm_pipe_lr_en_get(tdm);
    cfg_space_sister = bcmtm_tdm_min_sister_space_get(tdm, 0);

    TDM_PRINT1("TDM: Check VMAP sister port spacing (1-D): limit %d\n\n",
               cfg_space_sister);
    if (cfg_lr_en == TDM_TRUE && cfg_space_sister > 0) {
        int i, j, dist, pipe_id, cal_len, *cal_main;

        pipe_id = bcmtm_tdm_pipe_id_get(tdm);
        cal_main = bcmtm_tdm_lr_cal_get(tdm, pipe_id, 0);
        cal_len = bcmtm_tdm_lr_cal_len_get(tdm, pipe_id, 0);
        for (i = 0; i < cal_len; i++) {
            if (bcmtm_tdm_fport_chk(tdm, cal_main[i])) {
                dist = bcmtm_tdm_slot_sister_dist_get(tdm, cal_main,
                            cal_len, 0, i, TDM_DIR_DN);
                if (dist != 0) {
                    result = TDM_FAIL;
                    j = (i + dist) % cal_len;
                    TDM_PRINT5("%s slot [%03d|%03d], port [%3d|%3d]\n",
                               "[sister-port-space violation]",
                               i, j, cal_main[i], cal_main[j]);
                }
            }
        }
    }
    TDM_SML_BAR

    return result;
}

/*!
 * \brief Calendar same port space check.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval TDM_PASS Same port space check passed.
 * \retval TDM_FAIL Same port space check failed.
 */
static int
bcmtm_tdm_cal_same_chk(tdm_mod_t *tdm)
{
    int result = TDM_PASS;
    int cfg_space_same, cfg_lr_en;

    cfg_lr_en = bcmtm_tdm_pipe_lr_en_get(tdm);
    cfg_space_same = bcmtm_tdm_min_same_space_get(tdm, 0);

    TDM_PRINT1("TDM: Check VMAP same port spacing (1-D): limit %0d\n\n",
               cfg_space_same);
    if (cfg_lr_en == TDM_TRUE && cfg_space_same > 0) {
        int i, dist, pipe_id, cal_len, *cal_main;

        pipe_id = bcmtm_tdm_pipe_id_get(tdm);
        cal_len = bcmtm_tdm_lr_cal_len_get(tdm, pipe_id, 0);
        cal_main = bcmtm_tdm_lr_cal_get(tdm, pipe_id, 0);
        if (cal_main == NULL || cal_len == 0) {
            return TDM_PASS;
        }

        for (i = 0; i < cal_len; i++) {
            if (bcmtm_tdm_fport_chk(tdm, cal_main[i])) {
                dist = bcmtm_tdm_slot_same_dist_get(tdm, cal_main,
                            cal_len, 0, i, TDM_DIR_DN);
                if (dist != 0) {
                    result = TDM_FAIL;
                    TDM_PRINT4("%s, slot [%03d|%03d], port %3d\n",
                               "[same-port-space violation]",
                               i, ((i + dist) % cal_len),
                               cal_main[i]);
                }
            }
        }
    }
    TDM_SML_BAR

    return result;
}

/*!
 * \brief Vmap sister port space check at one slot position.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] vmap Pointer of vmap structure.
 * \param [in] col Slot column number in vmap.
 * \param [in] row Slot row number in vmap.
 *
 * \retval TDM_PASS Sister port space check passed.
 * \retval TDM_FAIL Sister port space check failed.
 */
static int
bcmtm_tdm_vmap_slot_sister_chk(tdm_mod_t *tdm, tdm_vmap_t *vmap,
                               int col, int row)
{
    int i, j, k, port, pm, pm_tmp, min_sister_space,
        result = TDM_PASS;
    int vmap_wid, vmap_len;

    if (tdm == NULL || vmap == NULL) {
        return result;
    }
    vmap_wid = vmap->wid;
    vmap_len = vmap->len;

    if (col >= 0 && col < vmap_wid && row >= 0 && row < vmap_len) {
        port = vmap->tbl[col][row];
        if (bcmtm_tdm_fport_chk(tdm, port)) {
            pm = bcmtm_tdm_port_pm_get(tdm, port);
            min_sister_space = bcmtm_tdm_min_sister_space_get(tdm, port);
            for (i = 0; i < vmap_wid; i++) {
                for (k = 1; k < min_sister_space; k++) {
                    /* TDM_DIR_DN. */
                    j = (row + k) % vmap_len;
                    pm_tmp = bcmtm_tdm_port_pm_get(tdm, vmap->tbl[i][j]);
                    if (pm_tmp == pm) {
                        result = TDM_FAIL;
                        break;
                    }

                    /* TDM_DIR_UP */
                    j = (vmap_len + row - k) % vmap_len;
                    pm_tmp = bcmtm_tdm_port_pm_get(tdm, vmap->tbl[i][j]);
                    if (pm_tmp == pm) {
                        result = TDM_FAIL;
                        break;
                    }
                }
                if (result == TDM_FAIL) break;
            }
        }
    }

    return result;
}

/*!
 * \brief Vmap sister port space check in one column.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] vmap Pointer of vmap structure.
 * \param [in] col Column number in vmap.
 *
 * \retval TDM_PASS Sister port space check passed.
 * \retval TDM_FAIL Sister port space check failed.
 */
static int
bcmtm_tdm_vmap_col_sister_chk(tdm_mod_t *tdm, tdm_vmap_t *vmap, int col)
{
    int row, result = TDM_PASS;

    if (tdm == NULL || vmap == NULL) {
        return result;
    }
    if (col >= 0 && col < vmap->wid) {
        for (row = 0; row < vmap->len; row++) {
            if (bcmtm_tdm_fport_chk(tdm, vmap->tbl[col][row])) {
                if (bcmtm_tdm_vmap_slot_sister_chk(tdm, vmap, col, row) !=
                    TDM_PASS) {
                    result = TDM_FAIL;
                    break;
                }
            }
        }
    }

    return result;
}


/*!
 * \brief Vmap sister port space check.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] vmap Pointer of vmap structure.
 *
 * \retval TDM_PASS Sister port space check passed.
 * \retval TDM_FAIL Sister port space check failed.
 */
static int
bcmtm_tdm_vmap_sister_chk(tdm_mod_t *tdm, tdm_vmap_t *vmap)
{
    int i, j, col, row, pos, pm, result = TDM_PASS;
    int *cal_tmp;
    int cfg_space_sister, cfg_token_empty;

    cfg_space_sister= bcmtm_tdm_min_sister_space_get(tdm, 0);
    cfg_token_empty = bcmtm_tdm_token_empty_get(tdm);

    TDM_PRINT0("TDM: Check VMAP sister port spacing (2-D)\n\n");
    if (cfg_space_sister > 0) {
        int vmap_wid, vmap_len;
        vmap_wid = vmap->wid;
        vmap_len = vmap->len;
        cal_tmp = TDM_ALLOC(vmap_len * sizeof(int), "bcmtmTdmCalTemp");

        TDM_ARRSET(cal_tmp, cfg_token_empty, vmap_len);

        for (row = 0; row < vmap_len; row++) {
            for (col = 0; col < vmap_wid; col++) {
                if (bcmtm_tdm_fport_chk(tdm, vmap->tbl[col][row])) {
                    cal_tmp[row] = vmap->tbl[col][row];
                    break;
                }
            }
        }
        for (i = 0; i < vmap_len; i++) {
            if (bcmtm_tdm_fport_chk(tdm, cal_tmp[i])) {
                pm = bcmtm_tdm_port_pm_get(tdm, cal_tmp[i]);
                for (j = 1; j < cfg_space_sister; j++) {
                    pos = (i + j) % vmap_len;
                    if (cal_tmp[i] != cal_tmp[pos] &&
                        pm == bcmtm_tdm_port_pm_get(tdm, cal_tmp[pos])) {
                        result = TDM_FAIL;
                        break;
                    }
                }
                if (result == TDM_FAIL) break;
            }
        }
        TDM_FREE(cal_tmp);
    }
    TDM_SML_BAR

    return result;
}

/*!
 * \brief Initialize vmap resource.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] vmap Pointer of vmap structure.
 * \param [in] wid_max Max width of vmap.
 * \param [in] len_ma Max length of vmap.
 *
 * \retval TDM_PASS Vmap init sequence passed.
 * \retval TDM_FAIL Vmap init sequence failed.
 */
static int
bcmtm_tdm_vmap_tbl_init(tdm_mod_t *tdm, tdm_vmap_t *vmap, int wid_max,
                        int len_max)
{
    int i, j, cfg_token_empty;
    int **tbl = NULL;
    int **r_a_arr = NULL;

    TDM_MEMSET(vmap, 0, sizeof(tdm_vmap_t));

    /* Allocate mem. */
    tbl = (int **) TDM_ALLOC(wid_max * sizeof(int *), "bcmtmTdmVmapTblWidth");
    for (i = 0; i < wid_max; i++) {
        tbl[i] = (int *) TDM_ALLOC(len_max * sizeof(int), "bcmtmTdmVmapTblLen");
    }
    r_a_arr = (int **) TDM_ALLOC(wid_max * sizeof(int *), "bcmtmTdmRaArrayWidth");
    for (i = 0; i < wid_max; i++) {
        r_a_arr[i] = (int *) TDM_ALLOC(TDM_VMAP_R_A_ARR_LEN * sizeof(int),
                                       "bcmtmTdmRaArrayLen");
    }

    /* Initialize mem. */
    cfg_token_empty = bcmtm_tdm_token_empty_get(tdm);
    for (i = 0; i < wid_max; i++) {
        for (j = 0; j < len_max; j++) {
            tbl[i][j] = cfg_token_empty;
        }
        for (j = 0; j < TDM_VMAP_R_A_ARR_LEN; j++) {
            r_a_arr[i][j] = 0;
        }
    }

    /* Initialize vmap */
    vmap->wid = wid_max;
    vmap->len = len_max;
    vmap->wid_max = wid_max;
    vmap->len_max = len_max;
    vmap->tbl = tbl;
    vmap->r_a_arr = r_a_arr;

    return TDM_PASS;
}

/*!
 * \brief Free vmap resource.
 *
 * \param [in] vmap Pointer of vmap structure.
 */
static void
bcmtm_tdm_vmap_tbl_free(tdm_vmap_t *vmap)
{
    int wid;

    if (vmap) {
        if (vmap->tbl) {
            for (wid = 0; wid < vmap->wid_max; wid++) {
                TDM_FREE(vmap->tbl[wid]);
                TDM_FREE(vmap->r_a_arr[wid]);
            }
        }
    }
}

/*!
 * \brief Initialize pmlist resource.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] pmlist Pointer of pmlist structure.
 * \param [in] size Max number of PMs in vmaplist.
 *
 * \retval TDM_PASS Pmlist init sequence passed.
 * \retval TDM_FAIL Pmlist init sequence failed.
 */
static int
bcmtm_tdm_pmlist_init(tdm_mod_t *tdm, tdm_vmap_pmlist_t *pmlist, int size)
{
    tdm_vmap_pm_t *pms = NULL;

    if (pmlist != NULL && size > 0) {
        TDM_MEMSET(pmlist, 0, sizeof(tdm_vmap_pmlist_t));

        pms =  TDM_ALLOC(size * sizeof(tdm_vmap_pm_t), "bcmtmTdmPmList");
        if (pms != NULL) {
            TDM_MEMSET(pms, 0, size * sizeof(tdm_vmap_pm_t));
            pmlist->num_pms = size;
            pmlist->num_pms_limit = size;
            pmlist->pms = pms;
        }
    }

    return TDM_PASS;
}

/*!
 * \brief Free pmlist resource.
 *
 * \param [in] pmlist Pointer of pmlist structure.
 */
static void
bcmtm_tdm_pmlist_free(tdm_vmap_pmlist_t *pmlist)
{
    if (pmlist) {
        if (pmlist->pms) {
            TDM_FREE(pmlist->pms);
        }
    }
}

/*!
 * \brief Reset PM.
 *
 * \param [in] vmap Pointer of vmap structure.
 * \param [in] pm Pointer of vmap pm structure.
 */
static void
bcmtm_tdm_vmap_pm_reset(tdm_mod_t *tdm, tdm_vmap_pm_t *pm)
{
    if (pm != NULL) {
        TDM_MEMSET(pm, 0, sizeof(tdm_vmap_pm_t));
    }
}

/*!
 * \brief Update PM info.
 *
 * \param [in] vmap Pointer of vmap structure.
 * \param [in] pm Pointer of vmap pm structure.
 */
static void
bcmtm_tdm_vmap_pm_update(tdm_mod_t *tdm, tdm_vmap_pm_t *pm)
{
    int j, subport_cnt;

    if (tdm != NULL && pm != NULL) {
        if ((*pm).pm_en) {
            /* Update subport_slot_rsv. */
            subport_cnt = (*pm).subport_cnt;
            for (j = 0; j < TDM_VMAP_PM_LANES && j < subport_cnt; j++) {
                if ((*pm).subport_slot_rsv[j] < (*pm).subport_slot_req[j]) {
                    (*pm).subport_slot_rsv[j] = (*pm).subport_slot_req[j];
                }
            }
            /* Update pm_num. */
            (*pm).pm_num = bcmtm_tdm_port_pm_get(tdm, (*pm).subport_phy[0]);
            /* Update pm_slots. */
            (*pm).pm_slots = 0;
            for (j = 0; j < TDM_VMAP_PM_LANES && j < subport_cnt; j++) {
                (*pm).pm_slots += (*pm).subport_slot_rsv[j];
            }
        } else {
            bcmtm_tdm_vmap_pm_reset(tdm, pm);
        }
    }
}

/*!
 * \brief Copy source PM to destination PM.
 *
 * \param [in] dst_pm Pointer of destination vmap pm.
 * \param [in] src_pm Pointer of source vmap pm.
 */
static void
bcmtm_tdm_vmap_pm_cpy(tdm_vmap_pm_t *dst_pm, tdm_vmap_pm_t *src_pm)
{
    int n;

    if (dst_pm != NULL && src_pm != NULL && dst_pm != src_pm) {
        dst_pm->pm_en = src_pm->pm_en;
        dst_pm->pm_num = src_pm->pm_num;
        dst_pm->pm_slots = src_pm->pm_slots;
        dst_pm->pm_mode = src_pm->pm_mode;
        dst_pm->subport_cnt = src_pm->subport_cnt;
        for (n = 0; n < TDM_VMAP_PM_LANES; n++) {
            dst_pm->subport_phy[n] = src_pm->subport_phy[n];
            dst_pm->subport_slot_req[n] = src_pm->subport_slot_req[n];
            dst_pm->subport_slot_rsv[n] = src_pm->subport_slot_rsv[n];
        }
    }
}

/*!
 * \brief Swap two PMs.
 *
 * \param [in] pm1 Pointer of destination vmap pm.
 * \param [in] pm2 Pointer of source vmap pm.
 */
static void
bcmtm_tdm_vmap_pm_swap(tdm_vmap_pm_t *pm1, tdm_vmap_pm_t *pm2)
{

    if (pm1 != NULL && pm2 != NULL && pm1 != pm2) {
        tdm_vmap_pm_t tmp;
        TDM_MEMSET(&tmp, 0, sizeof(tdm_vmap_pm_t));
        bcmtm_tdm_vmap_pm_cpy(&tmp, pm1);
        bcmtm_tdm_vmap_pm_cpy(pm1, pm2);
        bcmtm_tdm_vmap_pm_cpy(pm2, &tmp);
    }
}

/*!
 * \brief Add port into PM.
 *
 * \param [in] dst_pm Pointer of destination vmap pm.
 * \param [in] src_pm Pointer of source vmap pm.
 */
static int
bcmtm_tdm_vmap_pm_port_add(tdm_mod_t *tdm, tdm_vmap_pm_t *pm,
                       int port_token, int port_slots)
{
    int result = TDM_PASS;
    int subport_idx;

    if (tdm != NULL && pm != NULL) {
        subport_idx = pm->subport_cnt;
        if (subport_idx < TDM_VMAP_PM_LANES) {
            pm->pm_en = 1;
            pm->subport_cnt++;
            pm->subport_phy[subport_idx] = port_token;
            pm->subport_slot_req[subport_idx] = port_slots;
            pm->subport_slot_rsv[subport_idx] = port_slots;
            bcmtm_tdm_vmap_pm_update(tdm, pm);
        } else {
            result = TDM_FAIL;
            TDM_ERROR5("%s, pm %0d, port %0d, cnt %d, size %0d\n",
                "Number of PM_lanes overflow in vmap allocation",
                bcmtm_tdm_port_pm_get(tdm, port_token), port_token,
                subport_idx, TDM_VMAP_PM_LANES);
        }
    }

    return result;
}

/*!
 * \brief Construct pmlist.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] pmlist Pointer of pmlist structure.
 *
 * \retval TDM_PASS Pmlist construction completed successfully.
 * \retval TDM_FAIL Pmlist construction failed.
 */
static int
bcmtm_tdm_pmlist_gen(tdm_mod_t *tdm, tdm_vmap_pmlist_t *pmlist)
{
    int i, j, prt_tmp, slots1, slots2, result = TDM_PASS;
    int port, pm, port_slots, pm_idx, pm_cnt, pm_existed;
    int cfg_token_empty;
    int *buff = NULL;
    int buff_size;
    tdm_vmap_pm_t *pms = NULL;
    int num_pms;

    TDM_PRINT0("TDM: Construct pmlist for linerate ports\n\n");
    if (tdm != NULL && pmlist != NULL) {
        pms = pmlist->pms;
        num_pms = pmlist->num_pms_limit;
        cfg_token_empty = bcmtm_tdm_token_empty_get(tdm);
        buff = bcmtm_tdm_pipe_lr_buff_get(tdm);
        buff_size = bcmtm_tdm_pipe_lr_cnt_get(tdm);
        /* Step 1: sort ports in buff by speed from highest to lowest. */
        for (i = 0; i < buff_size; i++) {
            if (buff[i] == cfg_token_empty) break;
            for (j = buff_size - 1; j > i; j--) {
                if (buff[j] == cfg_token_empty) continue;
                slots1 = bcmtm_tdm_port_slots_get(tdm, buff[j-1], 0);
                slots2 = bcmtm_tdm_port_slots_get(tdm, buff[j], 0);
                if (slots1 > 0 && slots1 < slots2) {
                    prt_tmp = buff[j-1];
                    buff[j-1] = buff[j];
                    buff[j] = prt_tmp;
                }
            }
        }
        /* Step 2: construct pmlist based on buff. */
        pm_cnt = 0;
        for (i = 0; i < buff_size; i++) {
            port = buff[i];
            if (port == cfg_token_empty) break;
            /* Determine pm_idx for current port. */
            pm = bcmtm_tdm_port_pm_get(tdm, port);
            pm_existed = TDM_FALSE;
            for (j = 0; j < num_pms && j < pm_cnt; j++) {
                if (pms[j].pm_num == pm) {
                    pm_existed = TDM_TRUE;
                    break;
                }
            }
            pm_idx = (pm_existed == TDM_FALSE) ? (pm_cnt++) : (j);
            /* Add port into the selected pm in pm list. */
            if (pm_idx < num_pms) {
                port_slots = bcmtm_tdm_port_slots_get(tdm, port, 0);
                if (bcmtm_tdm_vmap_pm_port_add(tdm, &(pms[pm_idx]),
                    port, port_slots) != TDM_PASS) {
                    result = TDM_FAIL;
                }
            } else {
                result = TDM_FAIL;
                TDM_ERROR4("%s, port %0d, pm_idx %d, pm_size %0d\n",
                    "Number of PMs overflowed in vmap allocation",
                    port, pm_idx, num_pms);
            }
        }

        /* Print pmlist. */
        bcmtm_tdm_pmlist_print(tdm, pmlist);
    }

    return result;
}


/*!
 * \brief Sort pmlist.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] pmlist Pointer of pmlist structure.
 *
 * \return Nothing.
 */
static void
bcmtm_tdm_pmlist_sort(tdm_mod_t *tdm, tdm_vmap_pmlist_t *pmlist)
{
    int i, j, swap_happen;

    TDM_PRINT0("TDM: Sort pmlist\n\n");
    if (tdm != NULL && pmlist != NULL) {
        tdm_vmap_pm_t *pms;
        int num_pms;

        pms = pmlist->pms;
        num_pms = pmlist->num_pms_limit;
        /* Rule 0: sort PMs by pm bandwidth from highest to lowest. */
        for (i = 0; i < num_pms; i++) {
            if (pms[i].pm_en == 0) break;
            swap_happen = TDM_FALSE;
            for (j = num_pms - 1; j > i; j--) {
                if (pms[j].pm_en == 0) continue;
                if (pms[j].pm_slots >
                    pms[j-1].pm_slots) {
                    bcmtm_tdm_vmap_pm_swap(&(pms[j]), &(pms[j-1]));
                    swap_happen = TDM_TRUE;
                }
            }
            if (swap_happen == TDM_FALSE) break;
        }

        /* Rule 1: for PMs with the same pm bandwidth, sort PMs by
         *  port speed from highest to lowest
         */
        for (i = 0; i < num_pms; i++) {
            if (pms[i].pm_en == 0) break;
            swap_happen = TDM_FALSE;
            for (j = num_pms - 1; j > i; j--) {
                if (pms[j].pm_en == 0) continue;
                if (pms[j].pm_slots == pms[j-1].pm_slots &&
                    pms[j].subport_slot_req[0] >
                    pms[j-1].subport_slot_req[0]) {
                    bcmtm_tdm_vmap_pm_swap(&(pms[j]), &(pms[j-1]));
                    swap_happen = TDM_TRUE;
                }
            }
            if (swap_happen == TDM_FALSE) break;
        }

        /* Print pmlist. */
        bcmtm_tdm_pmlist_print(tdm, pmlist);
    }
}

/*!
 * \brief Shift pmlist at given position.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] pmlist Pointer of pmlist structure.
 * \param [in] pm_idx Index of PM in pmlist.
 *
 * \return Nothing.
 */
static void
bcmtm_tdm_pmlist_shift(tdm_mod_t *tdm, tdm_vmap_pmlist_t *pmlist, int pm_idx)
{
    int i, num_pms, idx_max;

    if (tdm != NULL && pmlist != NULL) {
        /* Find the last active pm in pmlist. */
        num_pms = pmlist->num_pms_limit;
        idx_max = num_pms - 1;
        while (!pmlist->pms[idx_max].pm_en && idx_max > 0) idx_max--;

        /* Shift pm[pm_idx,...,idx_max] forward by 1 step. */
        for (i = idx_max; i < num_pms && i >= pm_idx && i > 0 ; i--) {
            bcmtm_tdm_vmap_pm_cpy(&(pmlist->pms[i+1]), &(pmlist->pms[i]));
        }

        /* Clear pm at pm_idx. */
        if (pm_idx < num_pms) {
            bcmtm_tdm_vmap_pm_reset(tdm, &(pmlist->pms[pm_idx]));
        }
    }
}

/*!
 * \brief Adjust pmlist for lr ports belonging to 4-lane PM.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] pmlist Pointer of pmlist structure.
 *
 * \retval TDM_PASS Pmlist adjust completed successfully.
 * \retval TDM_FAIL Pmlist adjust failed.
 */
static int
bcmtm_tdm_pmlist_lr_4lanepm_adjust(tdm_mod_t *tdm, tdm_vmap_pmlist_t *pmlist)
{
    int i, j, subport_cnt = 0, pm_cnt = 0;
    int slot_lr, slot_emp_dual, slot_emp_tri, slot_emp_quad;
    int *s, s_e;
    int split_en_dual = TDM_TRUE, split_en_tri = TDM_TRUE,
        split_en_quad = TDM_TRUE;
    int cfg_lr_limit;
    int num_pms;
    tdm_vmap_pm_t *pms;

    if (tdm == NULL || pmlist == NULL) {
        return TDM_FAIL;
    }

    pms = pmlist->pms;
    num_pms = pmlist->num_pms_limit;
    cfg_lr_limit = bcmtm_tdm_pipe_lr_limit_get(tdm);

    TDM_PRINT0("TDM: Adjust pm list for special mode\n\n");
    /* Calculate requested slots: slot_lr. */
    slot_lr = 0;
    for (i = 0; i < num_pms; i++) {
        if (pms[i].pm_en == 0) break;
        for (j = 0; j < pms[i].subport_cnt && j < TDM_VMAP_PM_LANES; j++) {
            slot_lr += pms[i].subport_slot_req[j];
        }
        pm_cnt++;
    }

    /* Calculate speculatively inserted empty slots. */
    slot_emp_dual = 0;
    slot_emp_tri = 0;
    slot_emp_quad = 0;
    for (i = 0; i < num_pms; i++) {
        subport_cnt = pms[i].subport_cnt;
        if (subport_cnt == 0) break;
        s = pms[i].subport_slot_req;
        /* Special dual. */
        if (subport_cnt==2 && !(s[0]==s[1])) {
            /* Dual: x_y_0_0, where x!=y. */
            slot_emp_dual += (s[0] - s[1]);
        }
        /* Special tri. */
        else if (subport_cnt==3 && !(s[0]==2*s[1] && s[1]==s[2])) {
            /* Tri: x_x_x_0. */
            if (s[0]==s[1] && s[1]==s[2]) {
                slot_emp_tri += s[0];
            }
            /* Tri: x_y_z_0, where x!= 2*y and/or x!=2*z. */
            else {
                slot_emp_tri += s[0] - s[1];
                slot_emp_tri += s[0] - s[2];
            }
        }
        /* Special quad. */
        else if (subport_cnt==4 && !(s[0]==s[1] && s[1]==s[2] && s[2]==s[3])) {
            /* Quad: x_y_z_w, where x!=y or x!=z or x!=w. */
            slot_emp_quad += s[0] - s[1];
            slot_emp_quad += s[0] - s[2];
            slot_emp_quad += s[0] - s[3];
        }
    }
    if (slot_emp_dual > 0) {
        TDM_PRINT0("TDM: Detect special DUAL PM mode\n");
    }
    if (slot_emp_tri > 0) {
        TDM_PRINT0("TDM: Detect special TRI  PM mode\n");
    }
    if (slot_emp_quad > 0) {
        TDM_PRINT0("TDM: Detect special QUAD PM mode\n");
    }

    /* Determine split or not. */
    if (slot_emp_dual > 0 && (slot_emp_dual + slot_lr) <= cfg_lr_limit) {
        split_en_dual = TDM_FALSE;
        slot_lr += slot_emp_dual;
    }
    if (slot_emp_tri > 0 && (slot_emp_tri + slot_lr) <= cfg_lr_limit) {
        split_en_tri = TDM_FALSE;
        slot_lr += slot_emp_tri;
    }
    if (slot_emp_quad > 0 && (slot_emp_quad + slot_lr) <= cfg_lr_limit) {
        split_en_quad = TDM_FALSE;
        slot_lr += slot_emp_quad;
    }

    /* Adjust special dual/tri/quad PMs.
     *   -- case 1: insert empty slots to convert special mode PM to standard
     *   -- case 2: split a special mode PM into two or more standard mode PMs
     */
    /* Special dual. */
    if (slot_emp_dual > 0) {
        for (i = 0; i < num_pms; i++) {
            subport_cnt = pms[i].subport_cnt;
            if (subport_cnt == 0) break;
            s = pms[i].subport_slot_req;
            if (subport_cnt==2 && !(s[0]==s[1])) {
                /* Case 1: split. */
                if (split_en_dual == TDM_TRUE) {
                    bcmtm_tdm_vmap_pm_port_add(tdm, &(pms[pm_cnt]),
                                pms[i].subport_phy[1], s[1]);
                    pms[i].subport_cnt -= 1;
                    pms[i].pm_slots -= s[1];
                    pm_cnt++;
                    TDM_PRINT4("%s, from PM[%2d] to PM[%2d], PM[%2d]\n",
                               "TDM: [special dual] Split PM", i, i, pm_cnt-1);
                }
                /* Case 2: insert, x_y_0_0 -> x_x_0_0, where x>y. */
                else {
                    s_e = s[0] - s[1];
                    pms[i].subport_slot_rsv[1] = s[0];
                    pms[i].pm_slots += s_e;
                    TDM_PRINT5("%s, PM[%2d] %2d, port %3d, emp_slot %0d\n",
                               "TDM: [special dual] Insert empty slots",
                               i, pms[i].pm_num,
                               pms[i].subport_phy[1], s_e);
                }
            }
        }
    }
    /* Special tri. */
    if (slot_emp_tri > 0) {
        for (i = 0; i < num_pms; i++) {
            subport_cnt = pms[i].subport_cnt;
            if (subport_cnt == 0) break;
            s = pms[i].subport_slot_req;
            if (subport_cnt==3 && !(s[0]==2*s[1] && s[1]==s[2])) {
                /* Case 1: split. */
                if (split_en_tri == TDM_TRUE) {
                    if (s[0]>2*s[1] && s[1]==s[2]) {
                        bcmtm_tdm_vmap_pm_port_add(tdm, &(pms[pm_cnt]),
                                    pms[i].subport_phy[1], s[1]);
                        bcmtm_tdm_vmap_pm_port_add(tdm, &(pms[pm_cnt]),
                                    pms[i].subport_phy[2], s[2]);
                        pms[i].subport_cnt -= 2;
                        pms[i].pm_slots -= (s[1] + s[2]);
                        pm_cnt++;
                        TDM_PRINT4("%s, from PM[%2d] to PM[%2d], PM[%2d]\n",
                                   "TDM: [special tri] Split PM",
                                   i, i, pm_cnt-1);
                    }
                }
                /* Case 2: insert.
                 *    -- 1. x_x_x_0    -> 2x_x_x_0
                 *    -- 2. 2p5x_x_x_0 -> 2p5x_1p25x_1p25x_0
                 *    -- 3. 4x_x_x_0   -> 4x_2x_2x_0
                 */
                else {
                    if (s[0]==s[1] && s[1]==s[2]) {
                        s_e = s[0];
                        pms[i].subport_slot_rsv[0] += s_e;
                        pms[i].pm_slots += s_e;
                        TDM_PRINT5("%s, PM[%2d] %2d, port %3d, emp_slot %0d\n",
                                   "TDM: [special tri] Insert empty slots",
                                   i, pms[i].pm_num,
                                   pms[i].subport_phy[0], s_e);
                    } else if (s[0]>2*s[1] && s[1]==s[2]) {
                        s_e = (s[0] - s[1] - s[2]) / 2;
                        for (j = 1; j < subport_cnt; j++) {
                            pms[i].subport_slot_rsv[j] += s_e;
                            pms[i].pm_slots += s_e;
                            TDM_PRINT5("%s, PM[%2d] %2d, port %3d, slot %0d\n",
                                     "TDM: [special tri] Insert empty slots",
                                     i, pms[i].pm_num,
                                     pms[i].subport_phy[j], s_e);
                        }
                    }
                }
            }
        }
    }
    /* Special quad. */
    if (slot_emp_quad > 0) {
        for (i = 0; i < num_pms; i++) {
            subport_cnt = pms[i].subport_cnt;
            if (subport_cnt == 0) break;
            s = pms[i].subport_slot_req;
            if (subport_cnt==4 && !(s[0]==s[1] && s[1]==s[2] && s[2]==s[3])) {
                /* Case 1: split. */
                if (split_en_quad == TDM_TRUE) {
                    for (j = 1; j < subport_cnt; j++) {
                        if (s[0] != s[j]) {
                            bcmtm_tdm_vmap_pm_port_add(tdm, &(pms[pm_cnt]),
                                        pms[i].subport_phy[j], s[j]);
                            pms[i].subport_cnt -= 1;
                            pms[i].pm_slots -= s[j];
                        }
                    }
                    pm_cnt++;
                    TDM_PRINT4("%s, from PM[%2d] to PM[%2d], PM[%2d]\n",
                               "TDM: [special quad] Split PM", i, i, pm_cnt-1);
                }
                /* Case 2: insert, x_y_z_w -> x_x_x_x */
                else {
                    for (j = 1; j < subport_cnt; j++) {
                        if (s[0] != s[j]) {
                            s_e = (s[0] - s[j]);
                            pms[i].subport_slot_rsv[j] += s_e;
                            pms[i].pm_slots += s_e;
                            TDM_PRINT5("%s, PM[%2d] %2d, port %3d, slot %0d\n",
                                    "TDM: [special quad] Insert empty slots",
                                    i, pms[i].pm_num,
                                    pms[i].subport_phy[j], s_e);
                        }
                    }
                }
            }
        }
    }

    /* Print pm list. */
    bcmtm_tdm_pmlist_print(tdm, pmlist);

    return TDM_PASS;
}


/*!
 * \brief Adjust pmlist for linerate ports.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] pmlist Pointer of pmlist structure.
 *
 * \retval TDM_PASS Pmlist adjust completed successfully.
 * \retval TDM_FAIL Pmlist adjust failed.
 */
static int
bcmtm_tdm_pmlist_lr_adjust(tdm_mod_t *tdm, tdm_vmap_pmlist_t *pmlist)
{
    int cfg_num_pm_lanes;

    if (tdm == NULL || pmlist == NULL) {
        return TDM_FAIL;
    }
    cfg_num_pm_lanes= bcmtm_tdm_num_pm_lanes_get(tdm);

    /* Case 1: 4-lane PM. */
    bcmtm_tdm_pmlist_lr_4lanepm_adjust(tdm, pmlist);

    /* Case 2: 8-lane PM. */
    if (cfg_num_pm_lanes == 8) {
        /* Add further optimization for 8-lane PM */
    }

    return TDM_PASS;
}

/*!
 * \brief Adjust pmlist for oversub ports belonging to 4-lane PM.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] pmlist Pointer of pmlist structure.
 *
 * \retval TDM_PASS Pmlist adjust completed successfully.
 * \retval TDM_FAIL Pmlist adjust failed.
 *
 * Create pseudo pm(s) for EMPTY slots. The purpose is to evenly distribute
 * oversub slots among linerate slots.
 */
static int
bcmtm_tdm_pmlist_os_4lanepm_adjust(tdm_mod_t *tdm, tdm_vmap_pmlist_t *pmlist)
{
    int i, n, result = TDM_PASS;
    int lr_slot_num, empty_slots, empty_slots_left, empty_pms;
    int os_2_lr_r, lr_2_os_r, abs_r, abs_n, empty_slot_allc, slot_allc;
    int pm_slots, pm_slot_sum;
    int fac; /* correction factor */
    int cfg_cal_len, cfg_token_unused;
    int pipe_id;
    tdm_vmap_pm_t *pms;
    int num_pms;

    if (tdm == NULL || pmlist == NULL) {
        return TDM_FAIL;
    }
    pms = pmlist->pms;
    num_pms = pmlist->num_pms_limit;
    pipe_id = bcmtm_tdm_pipe_id_get(tdm);
    cfg_cal_len = bcmtm_tdm_lr_cal_len_get(tdm, pipe_id, 0);
    cfg_token_unused= TDM_VMAP_TOKEN_UNUSED;

    /* Calculate lr_slot_num and empty_slots. */
    lr_slot_num = 0;
    for (i = 0; i < num_pms; i++) {
        if (pms[i].subport_cnt > 0) {
            lr_slot_num += pms[i].pm_slots;
        }
    }

    empty_slots = cfg_cal_len - lr_slot_num;
    empty_slots_left= empty_slots;

    if (empty_slots > 0) {
        /* Calculate bandwidth ratio between linerate and oversub. */
        lr_2_os_r = 0;
        os_2_lr_r = 0;
        fac = 105; /* margin 5% */
        if (empty_slots > lr_slot_num) {
            if (lr_slot_num > 0) {
                os_2_lr_r = (empty_slots * fac) / (lr_slot_num * 100);
            }
        } else {
            if (empty_slots > 0) {
                lr_2_os_r = (lr_slot_num * fac) / (empty_slots * 100);
            }
        }

        /* Print bandwidth ratio. */
        TDM_PRINT0("TDM: Insert pseudo PM for non-linerate slots\n\n");
        TDM_PRINT1("\t-- num of linerate slots    : %3d\n", lr_slot_num);
        TDM_PRINT1("\t-- num of non-linerate slots: %3d\n", empty_slots);
        if (empty_slots > lr_slot_num) {
            TDM_PRINT0("\t-- Linerate type   : Minority\n");
            TDM_PRINT1("\t-- empty_2_lr_ratio: %3d\n", os_2_lr_r);
        }
        else {
            TDM_PRINT0("\t-- Linerate type   : Majority\n");
            TDM_PRINT1("\t-- lr_2_empty_ratio: %3d\n", lr_2_os_r);
        }
        TDM_PRINT0("\n");

        /* Case 1: bw_os > bw_lr. */
        if (os_2_lr_r > 0) {
            for (i = 0; i < (num_pms - 1); i++) {
                if (pms[i].subport_cnt == 0) break;
                if (empty_slots_left <= 0) break;
                pm_slots = pms[i].pm_slots;
                if (pm_slots > 0 && empty_slots_left > 0) {
                    abs_n = pm_slots * os_2_lr_r;
                    abs_n = (abs_n > 0) ? abs_n : 1;
                    slot_allc = abs_n <= empty_slots_left ?
                                abs_n : empty_slots_left;
                    empty_pms = bcmtm_tdm_math_div_ceil(slot_allc, pm_slots);
                    for (n = 0; n < empty_pms && i < (num_pms - 1); n++) {
                        empty_slot_allc = (pm_slots <= slot_allc) ?
                                          pm_slots : slot_allc;
                        if (empty_slot_allc < pm_slots) break;
                        bcmtm_tdm_pmlist_shift(tdm, pmlist, i+1);
                        bcmtm_tdm_vmap_pm_port_add(tdm, &(pms[i+1]),
                                        cfg_token_unused, empty_slot_allc);
                        slot_allc -= empty_slot_allc;
                        empty_slots_left -= empty_slot_allc;
                        i++;
                        TDM_PRINT6("%s[%2d], port %0d, slots %0d (%s=%0d)\n",
                                   "TDM: Insert pseudo pm, PM",
                                   i,
                                   pms[i].subport_phy[0],
                                   pms[i].subport_slot_req[0],
                                   "empty_slots_left", empty_slots_left);
                    }
                }
            }
        }
        /* Case 2: bw_lr > bw_os. */
        else if (lr_2_os_r > 0) {
            pm_slot_sum = 0;
            for (i = 0; i < (num_pms - 1); i++) {
                if (pms[i].subport_cnt == 0) break;
                if (empty_slots_left <= 0) break;
                pm_slots = pms[i].pm_slots;
                pm_slot_sum += pm_slots;
                if (pm_slot_sum > 0 && empty_slots_left > 0) {
                    abs_r = lr_slot_num / pm_slot_sum;
                    abs_n = empty_slots / abs_r;
                    abs_n = (lr_2_os_r == 1) ? pm_slot_sum : abs_n;
                    abs_n = (abs_n < pm_slots) ? abs_n : pm_slots;
                    if (pm_slot_sum / lr_2_os_r >= pm_slots) {
                        empty_slot_allc = (abs_n <= empty_slots_left) ?
                                          abs_n : empty_slots_left;
                        if (empty_slot_allc < pm_slots) continue;
                        bcmtm_tdm_pmlist_shift(tdm, pmlist, i+1);
                        bcmtm_tdm_vmap_pm_port_add(tdm, &(pms[i+1]),
                                        cfg_token_unused, empty_slot_allc);
                        empty_slots_left -= empty_slot_allc;
                        i++;
                        pm_slot_sum = 0;
                        TDM_PRINT6("%s[%2d], port %0d, slots %0d (%s=%0d)\n",
                                   "TDM: Insert pseudo pm, PM",
                                   i,
                                   pms[i].subport_phy[0],
                                   pms[i].subport_slot_req[0],
                                   "empty_slots_left", empty_slots_left);
                    }
                }
            }
            /* Improve the last pseudo PM. */
            if (empty_slots_left > 0) {
                /* Desc: if
                 *    1) last active PM (pms[i]) is a pseudo PM, and
                 *    2) second-last PM (pms[i-1])is not a pseudo PM, and
                 *    3) pms[i].pm_slots >= pms[i-1].pm_slots, and
                 *    4) empty_slots_left >= pms[i].pm_slots * fac
                 * then swap the the last two PMs.
                 */
                fac = 150;
                for (i = num_pms - 1; i > 0; i--) {
                    if (pms[i].subport_cnt != 0) break;
                }
                if (pms[i].subport_phy[0] == cfg_token_unused && i > 1) {
                    if (pms[i-1].subport_phy[0] != cfg_token_unused &&
                        pms[i].pm_slots >= pms[i-1].pm_slots &&
                        empty_slots_left * fac >= pms[i].pm_slots * 100) {
                        bcmtm_tdm_vmap_pm_swap(&(pms[i]), &(pms[i-1]));
                        TDM_PRINT3("%s, PM[%2d] to PM[%2d]\n",
                                   "TDM: Migrate pseudo pm",
                                   i, i-1);
                    }
                }
            }
        }

        /* Check validity. */
        if (empty_slots_left < 0) {
            /* Abnormal result. */
            result = TDM_FAIL;
            TDM_ERROR1("TDM: %d extra EMPTY slots allocated\n",
                       (-empty_slots_left));
        }
        /* Print pm list */
        bcmtm_tdm_pmlist_print(tdm, pmlist);
    }

    return result;
}

/*!
 * \brief Adjust pmlist for oversub ports.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] pmlist Pointer of pmlist structure.
 *
 * \retval TDM_PASS Pmlist adjust completed successfully.
 * \retval TDM_FAIL Pmlist adjust failed.
 */
static int
bcmtm_tdm_pmlist_os_adjust(tdm_mod_t *tdm, tdm_vmap_pmlist_t *pmlist)
{
    int cfg_num_pm_lanes;

    if (tdm == NULL || pmlist == NULL) {
        return TDM_FAIL;
    }
    cfg_num_pm_lanes= bcmtm_tdm_num_pm_lanes_get(tdm);

    /* Case 1: 4-lane PM. */
    bcmtm_tdm_pmlist_os_4lanepm_adjust(tdm, pmlist);

    /* Case 2: 8-lane PM. */
    if (cfg_num_pm_lanes == 8) {
        /* add further optimization for 8-lane PM */
    }

    return TDM_PASS;
}

/*!
 * \brief Update actual vmap size.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] vmap Pointer of vmap structure.
 *
 * \return Nothing.
 */
static void
bcmtm_tdm_vmap_size_update(tdm_mod_t *tdm, tdm_vmap_t *vmap)
{
    int i, j, vmap_wid = 0, cfg_token_empty, vmap_wid_max, vmap_len_max;

    cfg_token_empty = bcmtm_tdm_token_empty_get(tdm);
    vmap_wid_max = vmap->wid_max;
    vmap_len_max = vmap->len_max;

    for (i = vmap_wid_max - 1; i > 0; i--) {
        for (j = 0; j < vmap_len_max; j++) {
            if (vmap->tbl[i][j] != cfg_token_empty) {
                vmap_wid = i + 1;
                break;
            }
        }
        if (vmap_wid > 0) break;
    }
    vmap->wid = vmap_wid;
    vmap->len = vmap_len_max;
}

/*!
 * \brief Switch two columns in vmap.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] vmap Pointer of vmap structure.
 * \param [in] col_x Column number.
 * \param [in] col_y Column number.
 *
 * \retval TDM_PASS Switching completed successfully.
 * \retval TDM_FAIL Switching failed.
 *
 * Switch couterpart non-empty slots between column x and y if number
 * of physical ports for column x and y is equivalent.
 */
static int
bcmtm_tdm_vmap_col_switch(tdm_mod_t *tdm, tdm_vmap_t *vmap,
int col_x, int col_y)
{
    int j, row_x, row_y, y_prev, slot_cnt_x, slot_cnt_y,
        token_tmp, result = TDM_FAIL;
    int cfg_token_empty, cfg_vmap_wid, cfg_vmap_len;

    cfg_vmap_wid = vmap->wid_max;
    cfg_vmap_len = vmap->len_max;
    if (col_x >= 0 && col_x < cfg_vmap_wid &&
        col_y >= 0 && col_y < cfg_vmap_wid) {
        slot_cnt_x = 0;
        slot_cnt_y = 0;
        cfg_token_empty = bcmtm_tdm_token_empty_get(tdm);
        for (j = 0; j < cfg_vmap_len; j++) {
            if (vmap->tbl[col_x][j] != cfg_token_empty) slot_cnt_x++;
            if (vmap->tbl[col_y][j] != cfg_token_empty) slot_cnt_y++;
        }
        if (slot_cnt_x == slot_cnt_y && slot_cnt_x > 0) {
            result = TDM_PASS;
            y_prev = 0;
            for (row_x = 0; row_x < cfg_vmap_len; row_x++) {
                if (vmap->tbl[col_x][row_x] != cfg_token_empty) {
                    for (row_y = y_prev; row_y < cfg_vmap_len; row_y++) {
                        if (vmap->tbl[col_y][row_y] != cfg_token_empty) {
                            token_tmp = vmap->tbl[col_x][row_x];
                            vmap->tbl[col_x][row_x] = vmap->tbl[col_y][row_y];
                            vmap->tbl[col_y][row_y] = token_tmp;
                            y_prev = row_y + 1;
                            break;
                        }
                    }
                }
            }
        }
    }

    return result;
}

/*!
 * \brief Rotate vmap.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] vmap Pointer of vmap structure.
 *
 * \return Nothing.
 */
static int
bcmtm_tdm_vmap_rotate(tdm_mod_t *tdm, tdm_vmap_t *vmap)
{
    int i, j, vmap_wid, chk_i, chk_j, result = TDM_PASS;

    TDM_PRINT0("TDM: Rotate VMAP to remove sister spacing violation\n\n");
    vmap_wid = vmap->wid;
    for (i = vmap_wid - 1; i > 0; i--) {
        if (bcmtm_tdm_vmap_col_sister_chk(tdm, vmap, i) != TDM_PASS) {
            TDM_PRINT1("TDM: detect sister port violation at column %d\n", i);
            for (j = i - 1; j >= 0; j--) {
                if (bcmtm_tdm_vmap_col_switch(tdm, vmap, i, j) == TDM_PASS) {
                    chk_i = bcmtm_tdm_vmap_col_sister_chk(tdm, vmap, i);
                    chk_j = bcmtm_tdm_vmap_col_sister_chk(tdm, vmap, j);
                    if (chk_i != TDM_PASS) {
                        /* Switch back column i with j. */
                        bcmtm_tdm_vmap_col_switch(tdm, vmap, i, j);
                    } else {
                        if (chk_j == TDM_PASS) {
                            TDM_PRINT2("\t\t(a) switch column %d with %d\n",
                                       i, j);
                            break;
                        } else {
                            TDM_PRINT2("\t\t(b) switch column %d with %d\n",
                                       i, j);
                            i = j;
                        }
                    }
                }
            }
        }
    }
    TDM_SML_BAR

    return result;
}

/*!
 * \brief Shift port in calendar array.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port_token Port token.
 *
 * \retval TDM_PASS Shifting completed successfully.
 * \retval TDM_FAIL Shifting failed.
 */
static int
bcmtm_tdm_cal_port_shift(tdm_mod_t *tdm, int port_token)
{
    int i, j, n, x1, x2, d_dn, d_up, d_x1, d_x2,
        min_sister_space, shift_happen;
    int *cal_main;
    int cal_len;
    int pipe_id;

    min_sister_space = bcmtm_tdm_min_sister_space_get(tdm, port_token);
    pipe_id = bcmtm_tdm_pipe_id_get(tdm);
    cal_len = bcmtm_tdm_lr_cal_len_get(tdm, pipe_id, 0);
    cal_main = bcmtm_tdm_lr_cal_get(tdm, pipe_id, 0);
    if (cal_main == NULL || cal_len == 0) {
        return TDM_PASS;
    }

    for (n = 0; n < min_sister_space; n++) {
        shift_happen = TDM_FALSE;
        for (i = 0; i < cal_len; i++) {
            if (cal_main[i] == port_token) {
                d_dn = bcmtm_tdm_slot_sister_dist_get(tdm, cal_main,
                        cal_len, 0, i, TDM_DIR_DN);
                d_up = bcmtm_tdm_slot_sister_dist_get(tdm, cal_main,
                        cal_len, 0, i, TDM_DIR_UP);
                /* Shift up. */
                if (d_dn > 0) {
                    for (j = 0; j < cal_len; j++) {
                        if (cal_main[j] == port_token) {
                            x1 = j;
                            x2 = (cal_len + j - 1) % cal_len;
                            d_x1 = bcmtm_tdm_slot_sister_dist_get(tdm,
                                        cal_main, cal_len, 0,
                                        x1, TDM_DIR_UP);
                            d_x2 = bcmtm_tdm_slot_sister_dist_get(tdm,
                                        cal_main, cal_len, 0,
                                        x2, TDM_DIR_DN);
                            if (d_x1 == 0 && d_x2 == 0) {
                                cal_main[x1] = cal_main[x2];
                                cal_main[x2] = port_token;
                                shift_happen = TDM_TRUE;
                                TDM_PRINT4("%s, port %3d, slot %03d to %03d\n",
                                           "TDM: shift port   UP",
                                           port_token, x1, x2 );
                            }
                        }
                    }
                }
                /* Shift down. */
                else if (d_up > 0) {
                    for (j = 0; j < cal_len; j++) {
                        if (cal_main[j] == port_token) {
                            x1 = j;
                            x2 = (j + 1) % cal_len;
                            d_x1 = bcmtm_tdm_slot_sister_dist_get(tdm,
                                        cal_main, cal_len, 0,
                                        x1, TDM_DIR_DN);
                            d_x2 = bcmtm_tdm_slot_sister_dist_get(tdm,
                                        cal_main, cal_len, 0,
                                        x2, TDM_DIR_UP);
                            if (d_x1 == 0 && d_x2 == 0) {
                                cal_main[x1] = cal_main[x2];
                                cal_main[x2] = port_token;
                                shift_happen = TDM_TRUE;
                                TDM_PRINT4("%s, port %3d, slot %03d to %03d\n",
                                           "TDM: shift port DOWN",
                                           port_token, x1, x2 );
                            }
                        }
                    }
                }
            }
            if (shift_happen == TDM_TRUE) break;
        }
        if (shift_happen == TDM_FALSE) break;
    }

    return TDM_PASS;
}

/*!
 * \brief Fiter same port spacing violiation in calendar array.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval TDM_PASS Filtering completed successfully.
 * \retval TDM_FAIL Filtering failed.
 */
static int
bcmtm_tdm_cal_same_filter(tdm_mod_t *tdm)
{
    int i, m, port_i, dist_i, space_i, x, y, result = TDM_PASS;
    int port, chk_x, chk_y;
    int cal_len;
    int *cal_main;
    int pipe_id;

    /* Get parameters. */
    pipe_id = bcmtm_tdm_pipe_id_get(tdm);
    cal_len = bcmtm_tdm_lr_cal_len_get(tdm, pipe_id, 0);
    cal_main = bcmtm_tdm_lr_cal_get(tdm, pipe_id, 0);
    if (cal_main == NULL || cal_len == 0) {
        return TDM_PASS;
    }

    TDM_PRINT0("TDM: Adjust same port space\n\n");
    /* Shift up. */
    for (i = 0; i < cal_len; i++) {
        port_i = cal_main[i];
        if (bcmtm_tdm_fport_chk(tdm, port_i)) {
            dist_i = bcmtm_tdm_slot_same_dist_get(tdm, cal_main,
                        cal_len, 0, i, TDM_DIR_DN);
            if (dist_i != 0) {
                space_i = bcmtm_tdm_min_same_space_get(tdm, port_i);
                for (m = 1; m < (space_i - dist_i); m++) {
                    x = (cal_len + i - m) % cal_len;
                    y = (x + 1) % cal_len;
                    chk_x = bcmtm_tdm_cal_port_space_chk(tdm, cal_main,
                                cal_len, 1, x, TDM_DIR_DN);
                    chk_y = bcmtm_tdm_cal_port_space_chk(tdm, cal_main,
                                cal_len, 1, y, TDM_DIR_UP);
                    if (chk_x == TDM_PASS && chk_y == TDM_PASS) {
                        port = cal_main[y];
                        cal_main[y] = cal_main[x];
                        cal_main[x] = port;
                        TDM_PRINT4("%s, port %3d from slot %03d to %03d\n",
                                   "[same-port-space] Shift port   UP",
                                   port, y, x);
                    } else {
                        TDM_PRINT4("TDM: %s port %3d, slot [%3d, %3d]\n",
                                   "[same-port-space violation]",
                                   port_i, i, ((i + dist_i) % cal_len));
                        break;
                    }
                }
            }
        }
    }
    /* Shift down. */
    for (i = 0; i < cal_len; i++) {
        port_i = cal_main[i];
        if (bcmtm_tdm_fport_chk(tdm, port_i)) {
            dist_i = bcmtm_tdm_slot_same_dist_get(tdm, cal_main,
                        cal_len, 0, i, TDM_DIR_UP);
            if (dist_i != 0) {
                space_i = bcmtm_tdm_min_same_space_get(tdm, port_i);
                for (m = 1; m < (space_i - dist_i); m++) {
                    x = (i + m) % cal_len;
                    y = (cal_len + x - 1) % cal_len;
                    chk_x = bcmtm_tdm_cal_port_space_chk(tdm, cal_main,
                                cal_len, 1, x, TDM_DIR_UP);
                    chk_y = bcmtm_tdm_cal_port_space_chk(tdm, cal_main,
                                cal_len, 1, y, TDM_DIR_DN);
                    if (chk_x == TDM_PASS && chk_y == TDM_PASS) {
                        port = cal_main[y];
                        cal_main[y] = cal_main[x];
                        cal_main[x] = port;
                        TDM_PRINT4("%s, port %3d from slot %03d to %03d\n",
                                   "[same-port-space] Shift port   UP",
                                   port, y, x);
                    } else {
                        TDM_PRINT4("TDM: %s port %3d, slot [%3d, %3d]\n",
                                   "[same-port-space violation]",
                                   port_i, i, ((i + dist_i) % cal_len));
                        break;
                    }
                    if (chk_x == TDM_PASS && chk_y == TDM_PASS) {
                        port = cal_main[y];
                        cal_main[y] = cal_main[x];
                        cal_main[x] = port;
                        TDM_PRINT4("%s, port %3d from slot %03d to %03d\n",
                                   "[same-port-space] Shift port DOWN",
                                   port, y, x);
                    } else {
                        TDM_PRINT4("TDM: %s port %3d, slot [%03d, %03d]\n",
                                   "[same-port-space violation]",
                                   port_i, (i - dist_i), i);
                        break;
                    }
                }
            }
        }
    }

    result = bcmtm_tdm_cal_same_chk(tdm);
    return result;
}

/*!
 * \brief Fiter sister port spacing violiation in calendar array.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval TDM_PASS Filtering completed successfully.
 * \retval TDM_FAIL Filtering failed.
 */
static int
bcmtm_tdm_cal_sister_filter(tdm_mod_t *tdm)
{
    int i, j, n, port, port_i, port_j, speed_i, speed_j,
        sister_swap, dist, num_lr_ports, result = TDM_PASS;
    int cal_len;
    int *cal_main;
    int pipe_id;

    /* Get parameters. */
    pipe_id = bcmtm_tdm_pipe_id_get(tdm);
    cal_len = bcmtm_tdm_lr_cal_len_get(tdm, pipe_id, 0);
    cal_main = bcmtm_tdm_lr_cal_get(tdm, pipe_id, 0);
    if (cal_main == NULL || cal_len == 0) {
        return TDM_PASS;
    }

    num_lr_ports = bcmtm_tdm_pipe_lr_cnt_get(tdm);

    TDM_PRINT0("TDM: Adjust sister port space (0)\n\n");
    for (n = 0; n < num_lr_ports; n++) {
        sister_swap = TDM_FALSE;
        for (i = 0; i < cal_len; i++) {
            port_i = cal_main[i];
            if (bcmtm_tdm_fport_chk(tdm, port_i)) {
                dist = bcmtm_tdm_slot_sister_dist_get(tdm, cal_main,
                            cal_len, 0, i, TDM_DIR_DN);
                if (dist != 0) {
                    j = (i + dist) % cal_len;
                    port_j = cal_main[j];
                    if (bcmtm_tdm_fport_chk(tdm, port_j)) {
                        speed_i = bcmtm_tdm_cfg_port_speed_eth_get(tdm,
                                    port_i, 0);
                        speed_j = bcmtm_tdm_cfg_port_speed_eth_get(tdm,
                                    port_j, 0);
                        port = (speed_i < speed_j) ? port_i : port_j;
                        bcmtm_tdm_cal_port_shift(tdm, port);
                        sister_swap = TDM_TRUE;
                        break;
                    }
                }
            }
        }
        if (sister_swap == TDM_FALSE) {
            break;
        }
    }
    result = bcmtm_tdm_cal_sister_chk(tdm);
    return result;
}

/*!
 * \brief Calculate slot postion in a vmap column.
 *
 * \param [in] vmap_idx Vmap column number.
 * \param [in] slot_idx Vmap row number.
 * \param [in] vmap_idx_max Max vmap clolumn number.
 * \param [in] slot_idx_max Max vmap row number.
 * \param [in] r_a_arr Pointer of r_a_array.
 * \retval Slot postion in a vmap column.
 */
static int
bcmtm_tdm_vmap_slot_pos_calc(int vmap_idx, int slot_idx, int vmap_idx_max,
                       int slot_idx_max, int **r_a_arr)
{
    int i, j, a, b, n, pos, check;

    pos = slot_idx_max+1;
    if (r_a_arr != NULL &&
        vmap_idx >= 0 && vmap_idx < vmap_idx_max &&
        slot_idx >= 0 && slot_idx < slot_idx_max) {
        check = TDM_PASS;
        for (i = 0; i <= vmap_idx; i++) {
            if (r_a_arr[i][0] <= 0 || r_a_arr[i][1] <= 0) {
                check = TDM_FAIL;
                break;
            }
            if (i != vmap_idx && r_a_arr[i][0] == r_a_arr[i][1] ) {
                check = TDM_FAIL;
                break;
            }
        }
        if (check == TDM_PASS) {
            i = slot_idx + 1;
            n = vmap_idx + 1;
            for (j = n; j > 0; j--) {
                a = r_a_arr[j-1][0];
                b = r_a_arr[j-1][1];
                if (j == n) {
                    pos = 1 + bcmtm_tdm_math_div_floor(((i-1)*b), a);
                }
                else{
                    pos = pos + bcmtm_tdm_math_div_ceil((pos*a), (b-a));
                }
            }
        }
    }

    return pos;
}

/*!
 * \brief Allocate slots of the same port in a vmap column.
 *
 * \param [in] port_token Port token.
 * \param [in] slot_req Number of requested slots.
 * \param [in] slot_lest Number of available slots.
 * \param [in] cal_len Calendar length.
 * \param [in] vmap_idx Vmap column number.
 * \param [in] vmap Pointer of vmap structure.
 *
 * \retval TDM_PASS Slot assignment completed successfully.
 * \retval TDM_FAIL Slot assignment failed.
 */
static int
bcmtm_tdm_vmap_port_alloc(int port_token, int slot_req, int slot_left,
                        int cal_len, int vmap_idx, tdm_vmap_t *vmap)
{
    int i, pos, slot_cnt = 0, result = TDM_FAIL;
    int vmap_wid, vmap_len;

    if (vmap != NULL && slot_req > 0 && slot_req <= slot_left) {
        vmap_wid = vmap->wid_max;
        vmap_len = vmap->len_max;
        if (vmap_idx >= 0 && vmap_idx < vmap_wid) {
            vmap->r_a_arr[vmap_idx][0] = slot_req;
            vmap->r_a_arr[vmap_idx][1] = slot_left;

            for (i = 0; i < slot_req; i++) {
                pos = bcmtm_tdm_vmap_slot_pos_calc(vmap_idx, i, vmap_wid,
                                                   cal_len, vmap->r_a_arr);
                if (pos > 0 && pos <= cal_len && pos <= vmap_len) {
                    vmap->tbl[vmap_idx][pos-1] = port_token;
                    slot_cnt++;
                } else{
                    TDM_ERROR7(
                        "%s, prt %0d, %0d-th slot over %0d, %s <%0d,%0d>\n",
                        "pos OVERRUN in vmap allocation",
                        port_token, i, slot_req,
                        "<pos, pos_max>=", pos, cal_len);
                }
            }
            if (slot_cnt == slot_req) result = TDM_PASS;
        }
    } else{
        TDM_ERROR4("%s, slot_req %0d, slot_avail %0d, vmap_idx %0d\n",
                  "invalid condition for pos calculation in vmap allocation",
                  slot_req, slot_left, vmap_idx);
    }

    return result;
}

/*!
 * \brief Allocate slots for one PM.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port_token Port token.
 * \param [in] pm Pointer of PM.
 * \param [in] vmap_idx Vmap column number.
 * \param [in] vmap Pointer of vmap structure.
 *
 * \retval TDM_PASS Slot assignment completed successfully.
 * \retval TDM_FAIL Slot assignment failed.
 */
static int
bcmtm_tdm_vmap_pm_alloc(tdm_mod_t *tdm, int port_token,
tdm_vmap_pm_t *pm, int vmap_idx, tdm_vmap_t *vmap)
{
    int i, j, slot_cnt, result = TDM_PASS;
    int tmp_slot_req, tmp_slot_left, tmp_idx, tmp_wid, tmp_len, tmp_result,
        tmp_port_token;
    int cfg_token_empty;
    tdm_vmap_t tmp_vmap2;

    /* Get parameters. */
    cfg_token_empty = bcmtm_tdm_token_empty_get(tdm);
    tmp_wid = (*pm).subport_cnt;
    tmp_len = (*pm).pm_slots;
    bcmtm_tdm_vmap_tbl_init(tdm, &tmp_vmap2, tmp_wid, tmp_len);

    /* Assign slots for each port. */
    tmp_idx = 0;
    tmp_slot_left = tmp_len;
    for (i = 0; i < tmp_wid; i++) {
        tmp_port_token = (*pm).subport_phy[i];
        tmp_slot_req = (*pm).subport_slot_req[i];
        tmp_result = bcmtm_tdm_vmap_port_alloc(tmp_port_token, tmp_slot_req,
                            tmp_slot_left, tmp_len, tmp_idx, &tmp_vmap2);
        if (tmp_result != TDM_PASS) {
            result = TDM_FAIL;
            TDM_ERROR4("%s, port %d, slot_req %d, slot_avail %d\n",
                      "FAILED in tmp_vmap allocation",
                      tmp_port_token, tmp_slot_req, tmp_slot_left);
            break;
        }
        tmp_slot_left -= tmp_slot_req;
        tmp_idx++;
    }

    /* Fill vmap table. */
    if (vmap->tbl != NULL && vmap_idx < vmap->wid_max) {
        slot_cnt = 0;
        for (i = 0; i < vmap->len_max; i++) {
            if (vmap->tbl[vmap_idx][i] == port_token && slot_cnt < tmp_len) {
                tmp_port_token = cfg_token_empty;
                for (j = 0; j < tmp_wid; j++) {
                    if (tmp_vmap2.tbl[j][slot_cnt] != cfg_token_empty) {
                        tmp_port_token = tmp_vmap2.tbl[j][slot_cnt];
                        break;
                    }
                }
                vmap->tbl[vmap_idx][i] = tmp_port_token;
                slot_cnt++;
            }
            if (slot_cnt >= tmp_len) break;
        }
    }

    bcmtm_tdm_vmap_tbl_free(&tmp_vmap2);

    return result;
}

/*!
 * \brief Generate vmap.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] pmlist Pointer of pmlist structure.
 * \param [in] vmap Pointer of vmap structure.
 *
 * \retval TDM_PASS Vmap generation completed successfully.
 * \retval TDM_FAIL Vmap generation failed.
 */
static int
bcmtm_tdm_vmap_gen(tdm_mod_t *tdm, tdm_vmap_pmlist_t *pmlist,
tdm_vmap_t *vmap)
{
    int result = TDM_PASS;
    int i, j, vmap_idx, port_token, port_cnt, slot_req, slot_left, result_tmp;
    int cfg_cal_len;
    int pipe_id;
    tdm_vmap_pm_t *pms;
    int num_pms;

    TDM_PRINT0("TDM: Allocate slots for each PM \n\n");
    pipe_id = bcmtm_tdm_pipe_id_get(tdm);
    cfg_cal_len = bcmtm_tdm_lr_cal_len_get(tdm, pipe_id, 0);

    /* Assign slots for each PM in pm list.
     *      -- step 1: allocate slots for all ports within a PM.
     *      -- step 2: partition slots for each subport within a PM.
     */
    vmap_idx = 0;
    slot_left = cfg_cal_len;
    pms = pmlist->pms;
    num_pms = pmlist->num_pms_limit;
    for (i = 0; i < num_pms; i++) {
        port_cnt = pms[i].subport_cnt;
        slot_req = pms[i].pm_slots;
        if (port_cnt > 0 && slot_req > 0) {
            TDM_PRINT2("TDM: Allocating PM[%2d] %2d port <",
                       i, pms[i].pm_num);
            for (j = 0; j < port_cnt && j < TDM_VMAP_PM_LANES; j++) {
                TDM_PRINT1("%3d", pms[i].subport_phy[j]);
                if (j != port_cnt - 1) TDM_PRINT0(", ");
            }
            TDM_PRINT2("> with %3d slots from %0d\n", slot_req, slot_left);

            /* Step 1: allocate slots for the entire PM. */
            port_token = pms[i].subport_phy[0];
            result_tmp = bcmtm_tdm_vmap_port_alloc(port_token, slot_req,
                            slot_left, cfg_cal_len, vmap_idx, vmap);

            if (result_tmp != TDM_PASS) {
                result = TDM_FAIL;
                TDM_ERROR4("%s, port %d, slot_req %d, slot_avail %d\n",
                          "FAILED in linerate slot vmap allocation",
                          port_token, slot_req, slot_left);
                break;
            }
            /* Step 2: partition slots for all subports. */
            bcmtm_tdm_vmap_pm_alloc(tdm, port_token,
                                      &(pms[i]), vmap_idx, vmap);
            slot_left -= slot_req;
            vmap_idx++;
        }
    }

    TDM_SML_BAR
    bcmtm_tdm_vmap_size_update(tdm, vmap);

    return result;
}

/*!
 * \brief Populate linerate calendar based on vmap.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] vmap Pointer of vmap structure.
 *
 * \retval TDM_PASS Lineate calendar generation completed successfully.
 * \retval TDM_FAIL Linerate calendar generation failed.
 */
static int
bcmtm_tdm_vmap2cal_gen(tdm_mod_t *tdm, tdm_vmap_t *vmap)
{
    int i, j, port_token;
    int cfg_pipe_id, cfg_token_empty, cfg_token_unused,
        cfg_vmap_wid, cfg_vmap_len;
    int cal_len, *cal_main;

    /* Popultate linerate main calendar based on vmap. */
    cfg_pipe_id = bcmtm_tdm_pipe_id_get(tdm);
    cfg_token_empty = bcmtm_tdm_token_empty_get(tdm);
    cfg_token_unused = TDM_VMAP_TOKEN_UNUSED;
    cfg_vmap_wid = vmap->wid_max;
    cfg_vmap_len = vmap->len_max;
    cal_len = bcmtm_tdm_lr_cal_len_get(tdm, cfg_pipe_id, 0);
    cal_main = bcmtm_tdm_lr_cal_get(tdm, cfg_pipe_id, 0);
    for (j = 0; j < cfg_vmap_len && j < cal_len; j++) {
        port_token = cfg_token_empty;
        for (i = 0; i < cfg_vmap_wid; i++) {
            if (vmap->tbl[i][j] != cfg_token_empty &&
                vmap->tbl[i][j] != cfg_token_unused) {
                port_token = vmap->tbl[i][j];
                break;
            }
        }
        cal_main[j] = port_token;
    }

    return TDM_PASS;
}

/*!
 * \brief Populate linerate calendar.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval TDM_PASS Lineate calendar generation completed successfully.
 * \retval TDM_FAIL Linerate calendar generation failed.
 */
static int
bcmtm_tdm_calmain_gen(tdm_mod_t *tdm)
{
    int result = TDM_PASS;
    int cfg_pipe_id, cfg_lr_en, cfg_vmap_wid, cfg_vmap_len;
    tdm_vmap_t vmap;
    tdm_vmap_pmlist_t pmlist;

    /* Get parameters */
    cfg_pipe_id = bcmtm_tdm_pipe_id_get(tdm);
    cfg_lr_en = bcmtm_tdm_pipe_lr_en_get(tdm);
    cfg_vmap_wid = TDM_MAX_PORTS_PER_PIPE;
    cfg_vmap_len = bcmtm_tdm_lr_cal_len_get(tdm, cfg_pipe_id, 0);

    /* Initialize variables. */
    bcmtm_tdm_vmap_tbl_init(tdm, &vmap, cfg_vmap_wid, cfg_vmap_len);
    bcmtm_tdm_pmlist_init(tdm, &pmlist, cfg_vmap_wid);

    /* Print pipe bandwidth. */
    bcmtm_tdm_pipe_print(tdm);

    /* Generate vmap. */
    if (cfg_lr_en == TDM_TRUE && result == TDM_PASS) {
        /* Construct pmlist based on all lr ports of current pipe. */
        if (bcmtm_tdm_pmlist_gen(tdm, &pmlist) != TDM_PASS) {
            result = TDM_FAIL;
        }

        /* Adjust pmlist for lr: convert special PM to standard PM. */
        bcmtm_tdm_pmlist_lr_adjust(tdm, &pmlist);

        /* Sort pmlist by max_pm_port_speed from highest to lowest. */
        bcmtm_tdm_pmlist_sort(tdm, &pmlist);

        /* Adjust pmlist for os: insert pm(s) for EMPTY slots (optional). */
        bcmtm_tdm_pmlist_os_adjust(tdm, &pmlist);

        /* Allocate vmap slots for each PM in pmlist. */
        if (TDM_PASS != bcmtm_tdm_vmap_gen(tdm, &pmlist, &vmap)) {
            result = TDM_FAIL;
        }

        /* Check and fix vmap sister port spacing violation. */
        if (bcmtm_tdm_vmap_sister_chk(tdm, &vmap) != TDM_PASS) {
            bcmtm_tdm_vmap_rotate(tdm, &vmap);
        }

        /* Check vmap singularity. */
        if (bcmtm_tdm_vmap_singularity_chk(tdm, &vmap) != TDM_PASS) {
            result = TDM_FAIL;
        }

        /* Print vmap */
        bcmtm_tdm_vmap_print(tdm, &vmap);
    }

    /* Popultate linerate main calendar based on vmap. */
    bcmtm_tdm_vmap2cal_gen(tdm, &vmap);

    /* Free memory. */
    bcmtm_tdm_vmap_tbl_free(&vmap);
    bcmtm_tdm_pmlist_free(&pmlist);

    return result;
}

/****************************************************************************
 * Internal public functions.
 */

/*!
 * \brief Populate linerate calendar based on vmap.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval TDM_PASS Lineate calendar generation completed successfully.
 * \retval TDM_FAIL Linerate calendar generation failed.
 */
int
bcmtm_tdm_vmap_drv(tdm_mod_t *tdm)
{
    int j;
    int cal_len;
    int *cal_main;
    int pipe_id;
    int cfg_token_empty, cfg_lr_en;

    /* Get parameters. */
    cfg_lr_en = bcmtm_tdm_pipe_lr_en_get(tdm);
    cfg_token_empty = bcmtm_tdm_token_empty_get(tdm);
    pipe_id = bcmtm_tdm_pipe_id_get(tdm);

    cal_len = bcmtm_tdm_lr_cal_len_get(tdm, pipe_id, 0);
    cal_main= bcmtm_tdm_lr_cal_get(tdm, pipe_id, 0);
    if (cal_main == NULL || cal_len == 0) {
        return TDM_PASS;
    }

    /* Assign linerate slots. */
    if (cfg_lr_en == TDM_TRUE) {
        if (bcmtm_tdm_calmain_gen(tdm) != TDM_PASS) {
            return TDM_FAIL;
        }
    } else {
        for (j = 0; j < cal_len; j++) {
            cal_main[j] = cfg_token_empty;
        }
    }

    bcmtm_tdm_lr_cal_en_set(tdm, pipe_id, TDM_TRUE, 0);
    bcmtm_tdm_lr_cal_valid_set(tdm, pipe_id, TDM_TRUE, 0);
    /* Check/adjust sister port space. */
    if (bcmtm_tdm_cal_sister_chk(tdm) != TDM_PASS) {
        if (bcmtm_tdm_cal_sister_filter(tdm) != TDM_PASS) {
            bcmtm_tdm_lr_cal_valid_set(tdm, pipe_id, TDM_FALSE, 0);
        }
    }

    /* Check/adjust same port space. */
    if (bcmtm_tdm_cal_same_chk(tdm) != TDM_PASS) {
        if (bcmtm_tdm_cal_same_filter(tdm) != TDM_PASS) {
            bcmtm_tdm_lr_cal_valid_set(tdm, pipe_id, TDM_FALSE, 0);
        }
    }

    return TDM_PASS;
}
