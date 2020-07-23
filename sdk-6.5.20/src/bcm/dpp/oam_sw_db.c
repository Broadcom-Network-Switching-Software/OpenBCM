/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    oam_sw_db.c
 * Purpose: OAM SW DB
 */

#define _ERR_MSG_MODULE_NAME BSL_BCM_OAM
#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>

#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/sw_db.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/petra_dispatch.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/field_int.h>

#include <bcm_int/dpp/oam.h>
#include <bcm_int/dpp/oam_sw_db.h>
#include <bcm_int/dpp/oam_hw_db.h>
#include <bcm_int/dpp/oam_resource.h>
#include <bcm_int/dpp/oam_dissect.h>
#include <bcm_int/dpp/bfd.h>

#include <bcm_int/dpp/alloc_mngr_glif.h>

#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/PPC/ppc_api_llp_mirror.h>
#include <soc/dpp/PPD/ppd_api_eg_mirror.h>
#include <soc/dpp/PPD/ppd_api_lif_table.h>
#include <soc/dpp/mbcm.h>
#include <soc/dpp/mbcm_pp.h>

#include <bcm/types.h>
#include <bcm/module.h>
#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/oam.h>

#include <shared/gport.h>
#include <shared/shr_resmgr.h>

#include <soc/drv.h>
#include <soc/enet.h>
#include <soc/defs.h>

#define OAM_HASHS_ACCESS  OAM_ACCESS.oam_hashs

#define ENDP_LST_ACCESS          OAM_ACCESS.endp_lst
#define ENDP_LST_ACCESS_DATA     OAM_ACCESS.endp_lst.endp_lsts_array
#define ENDP_LST_M_ACCESS        OAM_ACCESS.endp_lst_m
#define ENDP_LST_M_ACCESS_DATA   OAM_ACCESS.endp_lst_m.endp_lst_ms_array

extern _bcm_dpp_oam_bfd_sw_db_lock_t _bcm_dpp_oam_bfd_rmep_info_db_lock[BCM_MAX_NUM_UNITS];
extern _bcm_dpp_oam_bfd_sw_db_lock_t _bcm_dpp_oam_bfd_mep_info_db_lock[BCM_MAX_NUM_UNITS];


/*
 * Function:
 *   _bcm_dpp_oam_htb_size_round
 * Purpose:
 *   Hash tables must have size that is a power of 2. This function
 *   rounds up the desired size to the nearest power of 2.
 * Params:
 *   (in) size - the desired hash table size
 * Returns:
 *   uint32 rounded up size to the nearest power of 2
 *   (assuming size < (1<<31))
 */
uint32
_bcm_dpp_oam_htb_size_round(uint32 size)
{

    if (size & (size-1)) {
        /* param is not a power of 2 */
        size--;
        size |= size >> 1;
        size |= size >> 2;
        size |= size >> 4;
        size |= size >> 8;
        size |= size >> 16;
        size++;
    }
    return(size);
}

/*
 * Function:
 *  _bcm_dpp_oam_htb_cast_key_f
 * Purpose:
 *  Default routine for casting the caller defined key to a string of bytes
 * Parameters:
 *  (in)  key        - key to cast
 *  (out) key_bytes  - string of bytes representing key
 *  (out) key_size   - length of key_bytes
 * Returns:
 *   none
 * Notes:
 *   The default cast is a no-op
 */
void
_bcm_dpp_oam_htb_cast_key_f(sw_state_htb_key_t key, uint8 **key_bytes, uint32 *key_size)
{

    *key_size = 4;
    *key_bytes = key;
}

/*
 * MA name db
 */


static void _bcm_dpp_sw_db_hash_oam_ma_name_destroy_cb(int unit,sw_state_htb_data_t data)
{
	_bcm_oam_ma_name_t * ma_name_p;

	ma_name_p = (_bcm_oam_ma_name_t*)data;
	sal_free(ma_name_p);
}

int
_bcm_dpp_sw_db_hash_oam_ma_name_create(int unit)
{
    int
      rv,
      ht_indx ;
    uint32
      ht_size = _bcm_dpp_oam_htb_size_round(SOC_PPC_OAM_MAX_NUMBER_OF_MAS(unit));

    BCMDNX_INIT_FUNC_DEFS;

    rv = sw_state_htb_create(unit,&ht_indx,ht_size,
                   sizeof(uint32), sizeof(_bcm_oam_ma_name_t),
                   "OAM ma name DB _bcm_dpp_sw_db_hash_oam_ma_name_create");
    BCMDNX_IF_ERR_EXIT(rv);
    rv = OAM_HASHS_ACCESS._bcm_dpp_oam_ma_index_to_name_db.set(unit, ht_indx) ;
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_sw_db_hash_oam_ma_name_destroy(int unit)
{
    int rv, ht_indx ;
    BCMDNX_INIT_FUNC_DEFS;

    rv = OAM_HASHS_ACCESS._bcm_dpp_oam_ma_index_to_name_db.get(unit, &ht_indx) ;
    BCMDNX_IF_ERR_EXIT(rv);
    BCMDNX_IF_ERR_EXIT(sw_state_htb_destroy(unit,ht_indx, _bcm_dpp_sw_db_hash_oam_ma_name_destroy_cb));
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_sw_db_hash_oam_ma_name_iterate(int unit, sw_state_htb_cb_t restore_cb)
{
  int rv;
  int ht_indx ;

  rv = OAM_HASHS_ACCESS._bcm_dpp_oam_ma_index_to_name_db.get(unit, &ht_indx) ;
  if (rv == BCM_E_NONE)
  {
    rv = sw_state_htb_iterate(unit, ht_indx, restore_cb);
  }
  return(rv);
}

int
_bcm_dpp_sw_db_hash_oam_ma_name_find(int unit, sw_state_htb_key_t key, sw_state_htb_data_t data, int remove)
{
  int rv = BCM_E_NONE;
  int ht_indx ;

  BCMDNX_INIT_FUNC_DEFS;

  rv = OAM_HASHS_ACCESS._bcm_dpp_oam_ma_index_to_name_db.get(unit, &ht_indx) ;
  BCMDNX_IF_ERR_EXIT(rv);
  rv = sw_state_htb_find(unit,ht_indx, key, data, remove);
  BCM_RETURN_VAL_EXIT(rv);

exit:
  BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_sw_db_hash_oam_ma_name_insert(int unit, sw_state_htb_key_t key, sw_state_htb_data_t data)
{
  int rv, ht_indx ;
  BCMDNX_INIT_FUNC_DEFS;
  rv = OAM_HASHS_ACCESS._bcm_dpp_oam_ma_index_to_name_db.get(unit, &ht_indx) ;
  BCMDNX_IF_ERR_EXIT(rv);
  /*
   * Note that sw_state_htb_insert() stores that data that is pointed by the
   * fourth input parameter (sw_state_htb_data_t data).
   */
  BCMDNX_IF_ERR_EXIT(sw_state_htb_insert(unit,ht_indx, key, (sw_state_htb_data_t)data));
exit:
  BCMDNX_FUNC_RETURN;
}

/*
 * MEP to MA db
 */


static void _bcm_dpp_sw_db_hash_oam_bfd_mep_info_destroy_cb(int unit, sw_state_htb_data_t data)
{
	SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY * mep_info_p;

	mep_info_p = (SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY*)data;
	sal_free(mep_info_p);
}

int
_bcm_dpp_sw_db_hash_oam_bfd_mep_info_create(int unit)
{
    int
      rv,
      ht_indx ;
    int oam_max_meps;

    BCMDNX_INIT_FUNC_DEFS;

/* Maximal number of MEPS supported by HW is 8K x 7 MD-levels. However, this consumes proportional
   memory and persistant storage (~ number of meps x 35B).
   This value may be overriden via soc property. (Table size must be power of 2)
   If up and down mep aded on same lif and level this should be increased by 2. */
    oam_max_meps = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_mep_count", 0);
    if (oam_max_meps < 1) {
        oam_max_meps = _bcm_dpp_oam_htb_size_round(
           SOC_PPC_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit) * 8);
    }

    rv = sw_state_htb_create(unit,&ht_indx,
                   oam_max_meps,
                   sizeof(uint32),sizeof(SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY),
                   "OAM bfd info DB _bcm_dpp_sw_db_hash_oam_bfd_mep_info_create");
    BCMDNX_IF_ERR_EXIT(rv);
    rv = OAM_HASHS_ACCESS._bcm_dpp_oam_bfd_mep_info_db.set(unit, ht_indx) ;
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_sw_db_hash_oam_bfd_mep_info_destroy(int unit)
{
    int rv, ht_indx ;

    BCMDNX_INIT_FUNC_DEFS;

    rv = OAM_HASHS_ACCESS._bcm_dpp_oam_bfd_mep_info_db.get(unit, &ht_indx) ;
    BCMDNX_IF_ERR_EXIT(rv);
    BCMDNX_IF_ERR_EXIT(sw_state_htb_destroy(unit,ht_indx, _bcm_dpp_sw_db_hash_oam_bfd_mep_info_destroy_cb));
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_sw_db_hash_oam_bfd_mep_info_iterate(int unit, sw_state_htb_cb_t restore_cb)
{
  int rv, ht_indx ;

  rv = OAM_HASHS_ACCESS._bcm_dpp_oam_bfd_mep_info_db.get(unit, &ht_indx) ;
  if (rv == BCM_E_NONE)
  {
    rv = sw_state_htb_iterate(unit, ht_indx, restore_cb);
  }
  return(rv);
}

int
_bcm_dpp_sw_db_hash_oam_bfd_mep_info_find(int unit, sw_state_htb_key_t key, sw_state_htb_data_t data, int remove)
{
    int rv = BCM_E_NONE ;
    int ht_indx ;

    BCMDNX_INIT_FUNC_DEFS ;

    rv = OAM_HASHS_ACCESS._bcm_dpp_oam_bfd_mep_info_db.get(unit, &ht_indx) ;
    BCMDNX_IF_ERR_EXIT(rv);
    rv = sw_state_htb_find(unit,ht_indx, key, data, remove);
    BCM_RETURN_VAL_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_sw_db_hash_oam_bfd_mep_info_insert(int unit, sw_state_htb_key_t key, sw_state_htb_data_t data)
{
    int rv, ht_indx ;
    BCMDNX_INIT_FUNC_DEFS;
    rv = OAM_HASHS_ACCESS._bcm_dpp_oam_bfd_mep_info_db.get(unit, &ht_indx) ;
    BCMDNX_IF_ERR_EXIT(rv);
    /*
     * Note that sw_state_htb_insert() stores that data that is pointed by the
     * fourth input parameter (sw_state_htb_data_t data).
     */
    BCMDNX_IF_ERR_EXIT(sw_state_htb_insert(unit,ht_indx, key, (sw_state_htb_data_t)data));
exit:
    BCMDNX_FUNC_RETURN;
}

static void _bcm_dpp_sw_db_hash_oam_sat_gtf_info_destroy_cb(int unit, sw_state_htb_data_t data)
{
	SOC_PPC_OAM_SAT_GTF_ENTRY * gtf_info_p;

	gtf_info_p = (SOC_PPC_OAM_SAT_GTF_ENTRY*)data;
	sal_free(gtf_info_p);
 }

int
_bcm_dpp_sw_db_hash_oam_sat_gtf_info_create(int unit)
{
    int  rv = BCM_E_NONE;
    int ht_indx = 0 ;
    int sat_max_gtf = 8;

    BCMDNX_INIT_FUNC_DEFS;

   /*SAT can support 8 gtf*/
    rv = sw_state_htb_create(unit,&ht_indx,
                   sat_max_gtf,
                   sizeof(uint32),sizeof(SOC_PPC_OAM_SAT_GTF_ENTRY),
                   "OAM info DB _bcm_dpp_sw_db_hash_oam_sat_gtf_info_create");
    BCMDNX_IF_ERR_EXIT(rv);
    rv = OAM_HASHS_ACCESS._bcm_dpp_oam_mep_sat_gtf_info_db.set(unit, ht_indx) ;
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_sw_db_hash_oam_sat_gtf_info_destroy(int unit)
{
    int  rv = BCM_E_NONE;
    int ht_indx = 0 ;

    BCMDNX_INIT_FUNC_DEFS;

    rv = OAM_HASHS_ACCESS._bcm_dpp_oam_mep_sat_gtf_info_db.get(unit, &ht_indx) ;
    BCMDNX_IF_ERR_EXIT(rv);
    BCMDNX_IF_ERR_EXIT(sw_state_htb_destroy(unit,ht_indx, _bcm_dpp_sw_db_hash_oam_sat_gtf_info_destroy_cb));
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_sw_db_hash_oam_sat_gtf_info_iterate(int unit, sw_state_htb_cb_t restore_cb)
{
    int  rv = BCM_E_NONE;
    int ht_indx = 0 ;

  rv = OAM_HASHS_ACCESS._bcm_dpp_oam_mep_sat_gtf_info_db.get(unit, &ht_indx) ;
  if (rv == BCM_E_NONE)
  {
    rv = sw_state_htb_iterate(unit, ht_indx, restore_cb);
  }
  return(rv);
}

int
_bcm_dpp_sw_db_hash_oam_sat_gtf_info_find(int unit, sw_state_htb_key_t key, sw_state_htb_data_t data, int remove)
{
    int  rv = BCM_E_NONE;
    int ht_indx = 0 ;

    BCMDNX_INIT_FUNC_DEFS ;

    rv = OAM_HASHS_ACCESS._bcm_dpp_oam_mep_sat_gtf_info_db.get(unit, &ht_indx) ;
    BCMDNX_IF_ERR_EXIT(rv);
    rv = sw_state_htb_find(unit,ht_indx, key, data, remove);
    BCM_RETURN_VAL_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_sw_db_hash_oam_sat_gtf_info_insert(int unit, sw_state_htb_key_t key, sw_state_htb_data_t data)
{
    int  rv = BCM_E_NONE;
    int ht_indx = 0 ;
    
    BCMDNX_INIT_FUNC_DEFS;
    rv = OAM_HASHS_ACCESS._bcm_dpp_oam_mep_sat_gtf_info_db.get(unit, &ht_indx) ;
    BCMDNX_IF_ERR_EXIT(rv);
    /*
     * Note that sw_state_htb_insert() stores that data that is pointed by the
     * fourth input parameter (sw_state_htb_data_t data).
     */
    BCMDNX_IF_ERR_EXIT(sw_state_htb_insert(unit,ht_indx, key, (sw_state_htb_data_t)data));
exit:
    BCMDNX_FUNC_RETURN;
}


static void _bcm_dpp_sw_db_hash_oam_sat_ctf_info_destroy_cb(int unit, sw_state_htb_data_t data)
{
	SOC_PPC_OAM_SAT_CTF_ENTRY * ctf_info_p;

	ctf_info_p = (SOC_PPC_OAM_SAT_CTF_ENTRY*)data;
	sal_free(ctf_info_p);
 }


int
_bcm_dpp_sw_db_hash_oam_sat_ctf_info_create(int unit)
{
    int  rv = BCM_E_NONE;
    int ht_indx = 0 ;
    int sat_max_ctf = 32;

    BCMDNX_INIT_FUNC_DEFS;

   /*SAT can support 32 ctf*/
    rv = sw_state_htb_create(unit,&ht_indx,
                   sat_max_ctf,
                   sizeof(uint32),sizeof(SOC_PPC_OAM_SAT_CTF_ENTRY),
                   "OAM info DB _bcm_dpp_sw_db_hash_oam_sat_gtf_info_create");
    BCMDNX_IF_ERR_EXIT(rv);
    rv = OAM_HASHS_ACCESS._bcm_dpp_oam_mep_sat_ctf_info_db.set(unit, ht_indx) ;
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_sw_db_hash_oam_sat_ctf_info_destroy(int unit)
{
    int  rv = BCM_E_NONE;
    int ht_indx = 0 ;

    BCMDNX_INIT_FUNC_DEFS;

    rv = OAM_HASHS_ACCESS._bcm_dpp_oam_mep_sat_ctf_info_db.get(unit, &ht_indx) ;
    BCMDNX_IF_ERR_EXIT(rv);
    BCMDNX_IF_ERR_EXIT(sw_state_htb_destroy(unit,ht_indx, _bcm_dpp_sw_db_hash_oam_sat_ctf_info_destroy_cb));
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_sw_db_hash_oam_sat_ctf_info_iterate(int unit, sw_state_htb_cb_t restore_cb)
{
    int  rv = BCM_E_NONE;
    int ht_indx = 0 ;

  rv = OAM_HASHS_ACCESS._bcm_dpp_oam_mep_sat_ctf_info_db.get(unit, &ht_indx) ;
  if (rv == BCM_E_NONE)
  {
    rv = sw_state_htb_iterate(unit, ht_indx, restore_cb);
  }
  return(rv);
}

int
_bcm_dpp_sw_db_hash_oam_sat_ctf_info_find(int unit, sw_state_htb_key_t key, sw_state_htb_data_t data, int remove)
{
    int  rv = BCM_E_NONE;
    int ht_indx = 0 ;

    BCMDNX_INIT_FUNC_DEFS ;

    rv = OAM_HASHS_ACCESS._bcm_dpp_oam_mep_sat_ctf_info_db.get(unit, &ht_indx) ;
    BCMDNX_IF_ERR_EXIT(rv);
    rv = sw_state_htb_find(unit,ht_indx, key, data, remove);
    BCM_RETURN_VAL_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_sw_db_hash_oam_sat_ctf_info_insert(int unit, sw_state_htb_key_t key, sw_state_htb_data_t data)
{
    int  rv = BCM_E_NONE;
    int ht_indx = 0 ;
    
    BCMDNX_INIT_FUNC_DEFS;
    rv = OAM_HASHS_ACCESS._bcm_dpp_oam_mep_sat_ctf_info_db.get(unit, &ht_indx) ;
    BCMDNX_IF_ERR_EXIT(rv);
    /*
     * Note that sw_state_htb_insert() stores that data that is pointed by the
     * fourth input parameter (sw_state_htb_data_t data).
     */
    BCMDNX_IF_ERR_EXIT(sw_state_htb_insert(unit,ht_indx, key, (sw_state_htb_data_t)data));
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * MA to MEP db
 */
static void _bcm_dpp_sw_db_hash_oam_endpoint_linked_list_destroy_cb(int unit, sw_state_htb_data_t data)
{
    int rv = BCM_E_NONE;
    ENDPOINT_LIST_PTR mep_list_p = 0 ;

    BCMDNX_INIT_FUNC_DEFS;
    mep_list_p = *((ENDPOINT_LIST_PTR *)data) ;
    /*
     * Free whole linked list, including head.
     */
    if (!SOC_IS_DETACHING(unit))
    {
        /*
         * This operation is not expected during 'deinit' (after new sw state is implemented)
         * so, for now, just avoid calling it while detaching. (Otherwise, an 'assert - like'
         * macro, SW_STATE_IS_DEINIT_CHECK, will fail the system.
         */
        rv = _bcm_dpp_oam_endpoint_list_deinit(unit, mep_list_p) ;
        BCMDNX_IF_ERR_EXIT(rv) ;
    }
exit:
    BCMDNX_FUNC_RETURN_VOID ;
}

int
_bcm_dpp_sw_db_hash_oam_ma_to_mep_create(int unit)
{
    int
      rv,
      ht_indx ;
    uint32
      ht_size = _bcm_dpp_oam_htb_size_round(SOC_PPC_OAM_MAX_NUMBER_OF_MAS(unit));

    BCMDNX_INIT_FUNC_DEFS;
    rv = sw_state_htb_create(unit,&ht_indx,ht_size,
                   sizeof(uint32),sizeof(ENDPOINT_LIST_PTR),
                   "OAM ma to mep DB _bcm_dpp_sw_db_hash_oam_ma_to_mep_create");
    BCMDNX_IF_ERR_EXIT(rv);
    rv = OAM_HASHS_ACCESS._bcm_dpp_oam_ma_to_mep_db.set(unit, ht_indx) ;
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_sw_db_hash_oam_ma_to_mep_destroy(int unit)
{
    int rv, ht_indx ;

    BCMDNX_INIT_FUNC_DEFS;

    rv = OAM_HASHS_ACCESS._bcm_dpp_oam_ma_to_mep_db.get(unit, &ht_indx) ;
    BCMDNX_IF_ERR_EXIT(rv);
    BCMDNX_IF_ERR_EXIT(sw_state_htb_destroy(unit,ht_indx, _bcm_dpp_sw_db_hash_oam_endpoint_linked_list_destroy_cb));

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_sw_db_hash_oam_ma_to_mep_iterate(int unit, sw_state_htb_cb_t restore_cb)
{
  int rv, ht_indx ;

  rv = OAM_HASHS_ACCESS._bcm_dpp_oam_ma_to_mep_db.get(unit, &ht_indx) ;
  if  (rv == BCM_E_NONE)
  {
    rv = sw_state_htb_iterate(unit, ht_indx, restore_cb);
  }
  return(rv);
}

int
_bcm_dpp_sw_db_hash_oam_ma_to_mep_find(int unit, sw_state_htb_key_t key, sw_state_htb_data_t data, int remove)
{
    int rv = BCM_E_NONE;
    int ht_indx ;

    BCMDNX_INIT_FUNC_DEFS;

    rv = OAM_HASHS_ACCESS._bcm_dpp_oam_ma_to_mep_db.get(unit, &ht_indx) ;
    BCMDNX_IF_ERR_EXIT(rv);
    rv = sw_state_htb_find(unit,ht_indx, key, data, remove);
    BCM_RETURN_VAL_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_sw_db_hash_oam_ma_to_mep_insert(int unit, sw_state_htb_key_t key, sw_state_htb_data_t data)
{
    int rv, ht_indx ;

    BCMDNX_INIT_FUNC_DEFS;
    rv = OAM_HASHS_ACCESS._bcm_dpp_oam_ma_to_mep_db.get(unit, &ht_indx) ;
    BCMDNX_IF_ERR_EXIT(rv);
    /*
     * Note that sw_state_htb_insert() stores that data that is pointed by the
     * fourth input parameter (sw_state_htb_data_t data).
     */
    BCMDNX_IF_ERR_EXIT(sw_state_htb_insert(unit,ht_indx, key, (sw_state_htb_data_t)data));

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * MEP to RMEP db
 */

int
_bcm_dpp_sw_db_hash_oam_bfd_mep_to_rmep_create(int unit)
{
    int rv, ht_indx;

    BCMDNX_INIT_FUNC_DEFS;
    rv = sw_state_htb_create(unit,&ht_indx,
                   _bcm_dpp_oam_htb_size_round(
                      SOC_PPC_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit)),
                   sizeof(uint32),sizeof(ENDPOINT_LIST_PTR),
                   "OAM ma to mep DB _bcm_dpp_sw_db_hash_oam_bfd_mep_to_rmep_create");
    BCMDNX_IF_ERR_EXIT(rv);
    rv = OAM_HASHS_ACCESS._bcm_dpp_oam_bfd_mep_to_rmep_db.set(unit, ht_indx) ;
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_sw_db_hash_oam_bfd_mep_to_rmep_destroy(int unit)
{
    int rv, ht_indx ;


    BCMDNX_INIT_FUNC_DEFS;

    rv = OAM_HASHS_ACCESS._bcm_dpp_oam_bfd_mep_to_rmep_db.get(unit, &ht_indx) ;
    BCMDNX_IF_ERR_EXIT(rv);
    BCMDNX_IF_ERR_EXIT(sw_state_htb_destroy(unit,ht_indx, _bcm_dpp_sw_db_hash_oam_endpoint_linked_list_destroy_cb));

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_sw_db_hash_oam_bfd_mep_to_rmep_iterate(int unit, sw_state_htb_cb_t restore_cb)
{
    int rv, ht_indx;

    rv = OAM_HASHS_ACCESS._bcm_dpp_oam_bfd_mep_to_rmep_db.get(unit, &ht_indx) ;
    if (rv == BCM_E_NONE)
    {
        rv = sw_state_htb_iterate(unit, ht_indx, restore_cb);
    }
    return(rv);
}

int
_bcm_dpp_sw_db_hash_oam_bfd_mep_to_rmep_find(int unit, sw_state_htb_key_t key, sw_state_htb_data_t data, int remove)
{
    int rv = BCM_E_NONE;
    int ht_indx ;

    BCMDNX_INIT_FUNC_DEFS;

    rv = OAM_HASHS_ACCESS._bcm_dpp_oam_bfd_mep_to_rmep_db.get(unit, &ht_indx) ;
    BCMDNX_IF_ERR_EXIT(rv);
    rv = sw_state_htb_find(unit,ht_indx, key, data, remove);
    BCM_RETURN_VAL_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_sw_db_hash_oam_bfd_mep_to_rmep_insert(int unit, sw_state_htb_key_t key, sw_state_htb_data_t data)
{
    int rv, ht_indx ;
    BCMDNX_INIT_FUNC_DEFS;
    rv = OAM_HASHS_ACCESS._bcm_dpp_oam_bfd_mep_to_rmep_db.get(unit, &ht_indx) ;
    BCMDNX_IF_ERR_EXIT(rv);
    /*
     * Note that sw_state_htb_insert() stores that data that is pointed by the
     * fourth input parameter (sw_state_htb_data_t data).
     */
    BCMDNX_IF_ERR_EXIT(sw_state_htb_insert(unit,ht_indx, key, (sw_state_htb_data_t)data));
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * RMEP info db
 */

static void _bcm_dpp_sw_db_hash_oam_bfd_rmep_info_destroy_cb(int unit, sw_state_htb_data_t data)
{
    SOC_PPC_OAM_RMEP_INFO_DATA * rmep_info_p;

    rmep_info_p = (SOC_PPC_OAM_RMEP_INFO_DATA*)data;
    sal_free(rmep_info_p);
}


int
_bcm_dpp_sw_db_hash_oam_bfd_rmep_info_create(int unit)
{
    int rv, ht_indx;

    uint32 htb_size;

    BCMDNX_INIT_FUNC_DEFS;

    /* Hash table size must be a power of 2.
       Find the smallest power of 2 that's big enough. */
    htb_size = _bcm_dpp_oam_htb_size_round(SOC_PPC_OAM_MAX_NUMBER_OF_REMOTE_MEPS(unit));

    rv = sw_state_htb_create(unit,&ht_indx,htb_size,
                   sizeof(uint32),sizeof(SOC_PPC_OAM_RMEP_INFO_DATA),
                   "OAM rmep DB _bcm_dpp_sw_db_hash_oam_bfd_rmep_info_create");
    BCMDNX_IF_ERR_EXIT(rv);
    rv = OAM_HASHS_ACCESS._bcm_dpp_oam_rmep_info_db.set(unit, ht_indx) ;
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_sw_db_hash_oam_bfd_rmep_info_destroy(int unit)
{
    int rv, ht_indx ;

    BCMDNX_INIT_FUNC_DEFS;

    rv = OAM_HASHS_ACCESS._bcm_dpp_oam_rmep_info_db.get(unit, &ht_indx) ;
    BCMDNX_IF_ERR_EXIT(rv);
    BCMDNX_IF_ERR_EXIT(sw_state_htb_destroy(unit,ht_indx, _bcm_dpp_sw_db_hash_oam_bfd_rmep_info_destroy_cb));

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_sw_db_hash_oam_bfd_rmep_info_iterate(int unit, sw_state_htb_cb_t restore_cb)
{
  int rv, ht_indx ;

  rv = OAM_HASHS_ACCESS._bcm_dpp_oam_rmep_info_db.get(unit, &ht_indx) ;
  if (rv == BCM_E_NONE)
  {
    rv = sw_state_htb_iterate(unit, ht_indx, restore_cb) ;
  }
  return(rv) ;
}

int
_bcm_dpp_sw_db_hash_oam_bfd_rmep_info_find(int unit, sw_state_htb_key_t key, sw_state_htb_data_t data, int remove)
{
    int rv = BCM_E_NONE;
    int ht_indx ;

    BCMDNX_INIT_FUNC_DEFS;

    rv = OAM_HASHS_ACCESS._bcm_dpp_oam_rmep_info_db.get(unit, &ht_indx) ;
    BCMDNX_IF_ERR_EXIT(rv);
    rv = sw_state_htb_find(unit,ht_indx, key, data, remove);
    BCM_RETURN_VAL_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_sw_db_hash_oam_bfd_rmep_info_insert(int unit, sw_state_htb_key_t key, sw_state_htb_data_t data)
{
    int rv, ht_indx ;

    BCMDNX_INIT_FUNC_DEFS;
    rv = OAM_HASHS_ACCESS._bcm_dpp_oam_rmep_info_db.get(unit, &ht_indx) ;
    BCMDNX_IF_ERR_EXIT(rv);
    /*
     * Note that sw_state_htb_insert() stores that data that is pointed by the
     * fourth input parameter (sw_state_htb_data_t data).
     */
    BCMDNX_IF_ERR_EXIT(sw_state_htb_insert(unit,ht_indx, key, (sw_state_htb_data_t)data));
exit:
    BCMDNX_FUNC_RETURN;
}

/*
* AIS DB functions.
*/
int
_bcm_dpp_sw_db_hash_oam_mep_to_ais_id_create(int unit)
{
    int rv, ht_indx ;
    BCMDNX_INIT_FUNC_DEFS;
    rv = sw_state_htb_create(unit,&ht_indx,
                   _bcm_dpp_oam_htb_size_round(
                      SOC_PPC_OAM_MAX_NUMBER_OF_LOCAL_MEPS(unit))>>1,
                   sizeof(uint32),sizeof(ENDPOINT_LIST_PTR),
                   "OAM MEP ID to AIS ID");
    BCMDNX_IF_ERR_EXIT(rv);
    rv = OAM_HASHS_ACCESS._bcm_dpp_oam_mep_to_ais_id.set(unit, ht_indx) ;
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


/**
 * Associate an AIS id with an endpoint.
 *
 * @author sinai (14/07/2014)
 *
 * @param unit
 * @param endpoint_id
 * @param ais_id
 *
 * @return int
 */
int _bcm_dpp_sw_db_hash_oam_mep_to_ais_id_insert(int unit, uint32 endpoint_id, int ais_id)
{
    int
      ht_indx,
      rv;
    ENDPOINT_LIST_PTR
      ais_list_p = 0 ;
    uint8
      found;
    uint32
      size ;
    uint32
      endp_lst_index ;

    BCMDNX_INIT_FUNC_DEFS;

    rv = OAM_HASHS_ACCESS._bcm_dpp_oam_mep_to_ais_id.get(unit, &ht_indx) ;
    BCMDNX_IF_ERR_EXIT(rv);
    rv = sw_state_htb_find(unit,ht_indx,(sw_state_htb_key_t)&endpoint_id, (sw_state_htb_data_t)&ais_list_p, 1 /*  remove*/  );
    if (rv == BCM_E_NOT_FOUND)
    {
        rv = _bcm_dpp_oam_endpoint_list_init(unit, &ais_list_p);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else
    {
        BCMDNX_IF_ERR_EXIT(rv);
        BCM_DPP_ENDP_LST_CONVERT_HANDLE_TO_ENDPLST_INDEX(endp_lst_index,ais_list_p) ;
        rv = ENDP_LST_ACCESS_DATA.size.get(unit, endp_lst_index, &size) ;
        BCMDNX_IF_ERR_EXIT(rv) ;
        if (size > 7)
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Maximum number of AIS frames per endpoint exceeded.\n")));
        }
    }
    rv = _bcm_dpp_oam_endpoint_list_member_find(unit, ais_list_p, ais_id, 0, &found);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!found)
    {
        /*if rmep index does not exist - add it */
        rv = _bcm_dpp_oam_endpoint_list_member_add(unit, ais_list_p, ais_id);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    rv = OAM_HASHS_ACCESS._bcm_dpp_oam_mep_to_ais_id.get(unit, &ht_indx) ;
    BCMDNX_IF_ERR_EXIT(rv);
    /*
     * Note that sw_state_htb_insert() stores that data that is pointed by the
     * fourth input parameter (sw_state_htb_data_t data).
     */
    rv = sw_state_htb_insert(unit,ht_indx,(sw_state_htb_key_t)&endpoint_id, (sw_state_htb_data_t)&ais_list_p);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/**
 * Return an array of available AIS IDs
 *
 * @author sinai (14/07/2014)
 *
 * @param unit
 * @param endpoint_id
 * @param ais_id - num_found entries will be filled.
 * @param num_found - Number of AIS elements used by the
 *                  endpoint
 *
 * @return int
 */
int _bcm_dpp_sw_db_hash_oam_mep_to_ais_id_get(int unit, uint32 endpoint_id, int * ais_id, uint8 * num_found)
{
    int rv, ht_indx ;
    ENDPOINT_LIST_PTR ais_list_p = 0 ;
    ENDPOINT_LIST_MEMBER_PTR endpoint_p ;
    BCMDNX_INIT_FUNC_DEFS;

    rv = OAM_HASHS_ACCESS._bcm_dpp_oam_mep_to_ais_id.get(unit, &ht_indx) ;
    BCMDNX_IF_ERR_EXIT(rv);
    rv = sw_state_htb_find(
            unit,ht_indx,(sw_state_htb_key_t)&endpoint_id, (sw_state_htb_data_t)&ais_list_p, 0 /*  remove*/  );
    if (rv == BCM_E_NOT_FOUND)
    {
        *num_found = 0;
    }
    else
    {
        int i=0;
        BCMDNX_IF_ERR_EXIT(rv);
        *num_found = 0 ;
        rv = _bcm_dpp_oam_endpoint_list_get_first_member(unit, ais_list_p, &endpoint_p) ;
        BCMDNX_IF_ERR_EXIT(rv);
        while  (endpoint_p)
        {
            uint32 member_index ;
            ENDPOINT_LIST_MEMBER_PTR member_next ;

            rv = _bcm_dpp_oam_endpoint_list_get_member_index(unit, endpoint_p, &member_index) ;
            BCMDNX_IF_ERR_EXIT(rv);
            *(ais_id +i) = member_index ;
            rv = _bcm_dpp_oam_endpoint_list_get_member_next(unit, endpoint_p, &member_next) ;
            BCMDNX_IF_ERR_EXIT(rv);
            endpoint_p = member_next ;
            *num_found +=1;
            ++i;
            if (*num_found > 1) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Multiple AIS entries per MEP disallowed.")));
            }
        }
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/**
 * Remove an entry from the hash table.
 *
 * @author sinai (14/07/2014)
 *
 * @param uint
 * @param mep_id
 *
 * @return int
 */
int _bcm_dpp_sw_db_hash_oam_mep_to_ais_id_delete_by_id(int unit, uint32 endpoint_id )
{
    int rv, ht_indx ;
    ENDPOINT_LIST_PTR ais_list_p = 0 ;
    BCMDNX_INIT_FUNC_DEFS;

    rv = OAM_HASHS_ACCESS._bcm_dpp_oam_mep_to_ais_id.get(unit, &ht_indx) ;
    BCMDNX_IF_ERR_EXIT(rv);
    rv = sw_state_htb_find(
            unit,ht_indx,
            (sw_state_htb_key_t)&endpoint_id, (sw_state_htb_data_t)&ais_list_p, 1 /*  remove*/);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_dpp_oam_endpoint_list_deinit(unit, ais_list_p);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


/**
 * Destroy the hash table
 *
 * @author sinai (14/07/2014)
 *
 * @param uint
 *
 * @return int
 */
int _bcm_dpp_sw_db_hash_oam_mep_to_ais_id_destroy(int unit)
{
    int rv, ht_indx ;
    BCMDNX_INIT_FUNC_DEFS;

    rv = OAM_HASHS_ACCESS._bcm_dpp_oam_mep_to_ais_id.get(unit, &ht_indx) ;
    BCMDNX_IF_ERR_EXIT(rv);
    rv = sw_state_htb_destroy(unit,ht_indx, _bcm_dpp_sw_db_hash_oam_endpoint_linked_list_destroy_cb);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * End of DB functions
 */



int _bcm_dpp_oam_dbs_init(int unit, uint8 is_oam) {
    int rv;

    BCMDNX_INIT_FUNC_DEFS;

    if (!SOC_WARM_BOOT(unit))
    {
        rv = _bcm_dpp_sw_db_hash_oam_ma_name_create(unit);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _bcm_dpp_sw_db_hash_oam_ma_to_mep_create(unit);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _bcm_dpp_sw_db_hash_oam_bfd_mep_info_create(unit);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _bcm_dpp_sw_db_hash_oam_bfd_mep_to_rmep_create(unit);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _bcm_dpp_sw_db_hash_oam_bfd_rmep_info_create(unit);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _bcm_dpp_sw_db_hash_oam_sat_gtf_info_create(unit);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _bcm_dpp_sw_db_hash_oam_sat_ctf_info_create(unit);
        BCMDNX_IF_ERR_EXIT(rv);
        
        if (SOC_IS_JERICHO(unit) && is_oam) {
            rv = _bcm_dpp_sw_db_hash_oam_mep_to_ais_id_create(unit);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_oam_dbs_destroy(int unit, uint8 is_oam) {
	int rv;

    BCMDNX_INIT_FUNC_DEFS;

   	rv = _bcm_dpp_sw_db_hash_oam_ma_name_destroy(unit);
   	BCMDNX_IF_ERR_EXIT(rv);
   	rv = _bcm_dpp_sw_db_hash_oam_ma_to_mep_destroy(unit);
   	BCMDNX_IF_ERR_EXIT(rv);
   	rv = _bcm_dpp_sw_db_hash_oam_bfd_rmep_info_destroy(unit);
   	BCMDNX_IF_ERR_EXIT(rv);
   	rv = _bcm_dpp_sw_db_hash_oam_bfd_mep_info_destroy(unit);
   	BCMDNX_IF_ERR_EXIT(rv);
   	rv = _bcm_dpp_sw_db_hash_oam_bfd_mep_to_rmep_destroy(unit);
   	BCMDNX_IF_ERR_EXIT(rv);
      rv = _bcm_dpp_sw_db_hash_oam_sat_gtf_info_destroy(unit);
      BCMDNX_IF_ERR_EXIT(rv);
      rv = _bcm_dpp_sw_db_hash_oam_sat_ctf_info_destroy(unit);
      BCMDNX_IF_ERR_EXIT(rv);

    if (SOC_IS_JERICHO(unit) && is_oam) {
		rv = _bcm_dpp_sw_db_hash_oam_mep_to_ais_id_destroy(unit);
		BCMDNX_IF_ERR_EXIT(rv);
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * MA name db
 */

int
_bcm_dpp_oam_ma_db_delete_ma_cb(int unit, sw_state_htb_key_t key, sw_state_htb_data_t data)
{
    int rv = BCM_E_NONE;
    uint32 * ma_index_p;
    _bcm_oam_ma_name_t ma_name;
    uint8 found;

    BCMDNX_INIT_FUNC_DEFS;

	ma_index_p = (uint32 *)key;

    rv = _bcm_dpp_oam_ma_db_get(unit, *ma_index_p, &ma_name, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                (_BSL_BCM_MSG("Error: Group %d not found.\n"), *ma_index_p));
    }

    if ((SOC_IS_QAX(unit) || (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "maid48_enabled", 0)))
    && ma_name.name_type == _BCM_OAM_MA_NAME_TYPE_STRING_48_BYTE) {
        int is_last, rv, index,i;
        if (SOC_IS_QAX(unit)) {
            rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_oam_bfd_mep_db_ext_data_delete,(unit, ma_name.index));
            BCMDNX_IF_ERR_EXIT(rv);
            /* Free the extended entries */
            for (i = 0; i < 3; i++, ma_name.index += (SOC_IS_QUX(unit) ? 0x100 : 0x400)) {
                rv = _bcm_dpp_oam_bfd_mep_id_dealloc(unit, ma_name.index);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        } else {
            if (SOC_IS_JERICHO(unit)) {
                index = ma_name.index / 24;
            } else {
                index = ma_name.index / 48;
            }
             rv = _bcm_dpp_am_template_oamp_pe_gen_mem_maid48_free(unit, index, &is_last);
             BCMDNX_IF_ERR_EXIT(rv);
        }
    }

	rv = bcm_petra_oam_endpoint_destroy_all(unit, *ma_index_p);
    BCMDNX_IF_ERR_EXIT(rv);

	rv = bcm_dpp_am_oam_ma_id_dealloc(unit, *ma_index_p);
	BCMDNX_IF_ERR_EXIT(rv);

	BCM_EXIT;
exit:
	BCMDNX_FUNC_RETURN;

}

int _bcm_dpp_oam_ma_db_insert(int unit, uint32 ma_index, _bcm_oam_ma_name_t * ma_name)
{
    int rv;
    BCMDNX_INIT_FUNC_DEFS;
    rv = _bcm_dpp_sw_db_hash_oam_ma_name_insert(unit,
                                               (sw_state_htb_key_t)&(ma_index),
                                               (sw_state_htb_data_t)(ma_name));
    BCMDNX_IF_ERR_EXIT(rv);


    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_oam_ma_db_get(int unit, uint32 ma_index, _bcm_oam_ma_name_t * ma_name, uint8 *found) {
    int rv;

    BCMDNX_INIT_FUNC_DEFS;
    rv = _bcm_dpp_sw_db_hash_oam_ma_name_find(unit,
                                        (sw_state_htb_key_t)&(ma_index),
                                        (sw_state_htb_data_t)(void *)ma_name,
                                        FALSE);
    if (rv == BCM_E_NONE) {
        *found = TRUE;
        /*
         * Note that *ma_name has already been loaded in _bcm_dpp_sw_db_hash_oam_ma_name_find()
         * above
         */
    }
    else if (rv == BCM_E_NOT_FOUND){
        *found = FALSE;
    }
    else {
        BCMDNX_IF_ERR_EXIT(rv);
    }


    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_oam_ma_db_delete(int unit, uint32 ma_index) {
    int rv;
    _bcm_oam_ma_name_t name_dummy ;

    BCMDNX_INIT_FUNC_DEFS;
    rv = _bcm_dpp_sw_db_hash_oam_ma_name_find(unit,
                                        (sw_state_htb_key_t)&(ma_index),
                                        (sw_state_htb_data_t)(void *)&name_dummy,
                                        TRUE);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * MA to MEP db
 */

int _bcm_dpp_oam_ma_to_mep_db_insert(int unit, uint32 ma_index, uint32 mep_index)
{
    int
      rv,
      do_deinit ;
    ENDPOINT_LIST_PTR
      mep_list_p = 0 ;
    uint8
      found;

    BCMDNX_INIT_FUNC_DEFS;
    /*
     * If do_deinit is non zero (error detected) then, at exit,
     * destroy endpoint list including head.
     */
    do_deinit = 1 ;
    rv = _bcm_dpp_sw_db_hash_oam_ma_to_mep_find(unit,
                                        (sw_state_htb_key_t)&(ma_index),
                                        (sw_state_htb_data_t)(void *)(&mep_list_p),
                                        TRUE);
    if (rv == BCM_E_NOT_FOUND)
    {
        rv = _bcm_dpp_oam_endpoint_list_init(unit,&mep_list_p) ;
        /*
         * If 'init' has failed then there is no point trying to destroy...
         */
        do_deinit = 0 ;
        BCMDNX_IF_ERR_EXIT(rv);
        do_deinit = 1 ;
    }
    else
    {
        BCMDNX_IF_ERR_EXIT(rv);
    }

    rv = _bcm_dpp_oam_endpoint_list_member_find(unit, mep_list_p, mep_index, 0, &found);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!found)
    {
        /* adding new mep_index if it does not exist already */
        rv = _bcm_dpp_oam_endpoint_list_member_add(unit, mep_list_p, mep_index);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    rv = _bcm_dpp_sw_db_hash_oam_ma_to_mep_insert(unit,
                                                (sw_state_htb_key_t)&(ma_index),
                                                (sw_state_htb_data_t)&(mep_list_p));
    BCMDNX_IF_ERR_EXIT(rv);
    do_deinit = 0 ;
exit:
    /*
     * This is an error exit: In case of failure, get rid of
     * the whole list including head.
     */
    if (do_deinit)
    {
        rv = _bcm_dpp_oam_endpoint_list_deinit(unit, mep_list_p) ;
        BCMDNX_IF_ERR_CONT(rv) ;
  }
    BCMDNX_FUNC_RETURN;
}

/*The result of this get function is the member itself and not a copy*/
int _bcm_dpp_oam_ma_to_mep_db_get(int unit, uint32 ma_index, ENDPOINT_LIST_PTR *mep_list, uint8* found) {
    int rv;

    BCMDNX_INIT_FUNC_DEFS;
    rv = _bcm_dpp_sw_db_hash_oam_ma_to_mep_find(unit,
                                        (sw_state_htb_key_t)&(ma_index),
                                        (sw_state_htb_data_t)(void *)mep_list,
                                        FALSE);
    if (rv == BCM_E_NONE) {
        *found = TRUE;
    }
    else if (rv == BCM_E_NOT_FOUND){
        *found = FALSE;
    }
    else {
        BCMDNX_IF_ERR_EXIT(rv);
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_oam_ma_to_mep_db_ma_delete(int unit, uint32 ma_index) {
    int rv;
    ENDPOINT_LIST_PTR mep_list_p = 0 ;

    BCMDNX_INIT_FUNC_DEFS;
    rv = _bcm_dpp_sw_db_hash_oam_ma_to_mep_find(unit,
                                        (sw_state_htb_key_t)&(ma_index),
                                        (sw_state_htb_data_t)(void *)&mep_list_p,
                                        TRUE);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!SOC_IS_DETACHING(unit))
    {
        /*
         * This operation is not expected during 'deinit' (after new sw state is implemented)
         * so, for now, just avoid calling it while detaching. (Otherwise, an 'assert - like'
         * macro, SW_STATE_IS_DEINIT_CHECK, will fail the system.
         */
        rv = _bcm_dpp_oam_endpoint_list_deinit(unit, mep_list_p);
        BCMDNX_IF_ERR_EXIT(rv);
    }
exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_dpp_oam_ma_to_mep_db_mep_delete(int unit, uint32 ma_index, uint32 mep_index)
{
    int rv, is_empty ;
    ENDPOINT_LIST_PTR mep_list_p = 0 ;
    uint8 found;

    BCMDNX_INIT_FUNC_DEFS;
    rv = _bcm_dpp_sw_db_hash_oam_ma_to_mep_find(unit,
                                        (sw_state_htb_key_t)&(ma_index),
                                        (sw_state_htb_data_t)(void *)(&mep_list_p),
                                        TRUE);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = _bcm_dpp_oam_endpoint_list_member_find(unit, mep_list_p, mep_index, 1, &found);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_dpp_oam_endpoint_list_empty(unit, mep_list_p, &is_empty) ;
    BCMDNX_IF_ERR_EXIT(rv);
    if (is_empty)
    {
        if (!SOC_IS_DETACHING(unit))
        {
            /*
             * This operation is not expected during 'deinit' (after new sw state is implemented)
             * so, for now, just avoid calling it while detaching. (Otherwise, an 'assert - like'
             * macro, SW_STATE_IS_DEINIT_CHECK, will fail the system.
             */
            rv = _bcm_dpp_oam_endpoint_list_deinit(unit, mep_list_p);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }
    else {
        rv = _bcm_dpp_sw_db_hash_oam_ma_to_mep_insert(unit, (sw_state_htb_key_t)&(ma_index), (sw_state_htb_data_t)&mep_list_p);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Error: Endpoint %d not found\n"), mep_index));
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * MEP info db
 */
int _bcm_dpp_oam_bfd_mep_info_db_insert_no_lock(int unit, uint32 mep_index, SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY * mep_info)
{
    int rv;
    /*
     * This variable is used here only as place holder.
     */
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY mep_info_temp ;

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_sw_db_hash_oam_bfd_mep_info_find(unit,
                                        (sw_state_htb_key_t)&(mep_index),
                                        (sw_state_htb_data_t)(void *)(&mep_info_temp),
                                        FALSE);

    if (rv == BCM_E_NOT_FOUND) {
        rv = _bcm_dpp_sw_db_hash_oam_bfd_mep_info_insert(unit,
                                                (sw_state_htb_key_t)&(mep_index),
                                                (sw_state_htb_data_t)(mep_info));
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else {
        if (rv == BCM_E_NONE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,(_BSL_BCM_MSG("Error: Endpoint %d exists\n"), mep_index));
        }
        else {
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * SAT gtf info db
 */
int _bcm_dpp_oam_sat_gtf_info_db_insert(int unit, uint32 gtf_index, SOC_PPC_OAM_SAT_GTF_ENTRY * gtf_info)
{
    int rv;
    /*
     * This variable is used here only as place holder.
     */
    SOC_PPC_OAM_SAT_GTF_ENTRY gtf_info_temp ;

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_sw_db_hash_oam_sat_gtf_info_find(unit,
                                        (sw_state_htb_key_t)&(gtf_index),
                                        (sw_state_htb_data_t)(void *)(&gtf_info_temp),
                                        FALSE);

    if (rv == BCM_E_NOT_FOUND) {
        rv = _bcm_dpp_sw_db_hash_oam_sat_gtf_info_insert(unit,
                                                (sw_state_htb_key_t)&(gtf_index),
                                                (sw_state_htb_data_t)(gtf_info));
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else {
        if (rv == BCM_E_NONE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,(_BSL_BCM_MSG("Error: gtf  %d exists\n"), gtf_index));
        }
        else {
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }
exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_dpp_oam_sat_gtf_info_db_get(int unit, uint32 gtf_id, SOC_PPC_OAM_SAT_GTF_ENTRY * gtf_info, uint8 *found)
{
    int rv;

    BCMDNX_INIT_FUNC_DEFS;
    rv = _bcm_dpp_sw_db_hash_oam_sat_gtf_info_find(unit,
                                        (sw_state_htb_key_t)&(gtf_id),
                                        (sw_state_htb_data_t)(void *)(gtf_info),
                                        FALSE);
    if (rv == BCM_E_NONE) {
        *found = TRUE;
    }
    else if (rv == BCM_E_NOT_FOUND){
		*found = FALSE;
    }
	else {
		BCMDNX_IF_ERR_EXIT(rv);
	}

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_oam_sat_gtf_info_db_delete(int unit, uint32 gtf_id) {
    int rv;
    SOC_PPC_OAM_SAT_GTF_ENTRY gtf_info_dummy ;

    BCMDNX_INIT_FUNC_DEFS;
    rv = _bcm_dpp_sw_db_hash_oam_sat_gtf_info_find(unit,
                                        (sw_state_htb_key_t)&(gtf_id),
                                        (sw_state_htb_data_t)(void *)(&gtf_info_dummy),
                                        TRUE);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_dpp_oam_sat_gtf_info_db_update(int unit, uint32 gtf_id, SOC_PPC_OAM_SAT_GTF_ENTRY * gtf_info) {
	int rv;
    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_oam_sat_gtf_info_db_delete(unit,gtf_id);
    BCMDNX_IF_ERR_EXIT(rv);
    rv =  _bcm_dpp_oam_sat_gtf_info_db_insert(unit,gtf_id,gtf_info);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * SAT ctf info db
 */
int _bcm_dpp_oam_sat_ctf_info_db_insert(int unit, uint32 ctf_index, SOC_PPC_OAM_SAT_CTF_ENTRY * ctf_info)
{
    int rv;
    /*
     * This variable is used here only as place holder.
     */
    SOC_PPC_OAM_SAT_CTF_ENTRY ctf_info_temp ;

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_sw_db_hash_oam_sat_ctf_info_find(unit,
                                        (sw_state_htb_key_t)&(ctf_index),
                                        (sw_state_htb_data_t)(void *)(&ctf_info_temp),
                                        FALSE);

    if (rv == BCM_E_NOT_FOUND) {
        rv = _bcm_dpp_sw_db_hash_oam_sat_ctf_info_insert(unit,
                                                (sw_state_htb_key_t)&(ctf_index),
                                                (sw_state_htb_data_t)(ctf_info));
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else {
        if (rv == BCM_E_NONE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,(_BSL_BCM_MSG("Error: ctf %d exists\n"), ctf_index));
        }
        else {
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }
exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_dpp_oam_sat_ctf_info_db_get(int unit, uint32 ctf_id, SOC_PPC_OAM_SAT_CTF_ENTRY * ctf_info, uint8 *found)
{
    int rv;

    BCMDNX_INIT_FUNC_DEFS;
    rv = _bcm_dpp_sw_db_hash_oam_sat_ctf_info_find(unit,
                                        (sw_state_htb_key_t)&(ctf_id),
                                        (sw_state_htb_data_t)(void *)(ctf_info),
                                        FALSE);
    if (rv == BCM_E_NONE) {
        *found = TRUE;
    }
    else if (rv == BCM_E_NOT_FOUND){
		*found = FALSE;
    }
    else {
		BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_oam_sat_ctf_info_db_delete(int unit, uint32 ctf_id) {
    int rv;
    SOC_PPC_OAM_SAT_CTF_ENTRY ctf_info_dummy ;

    BCMDNX_INIT_FUNC_DEFS;
    rv = _bcm_dpp_sw_db_hash_oam_sat_ctf_info_find(unit,
                                        (sw_state_htb_key_t)&(ctf_id),
                                        (sw_state_htb_data_t)(void *)(&ctf_info_dummy),
                                        TRUE);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


int _bcm_dpp_oam_sat_ctf_info_db_update(int unit, uint32 ctf_id, SOC_PPC_OAM_SAT_CTF_ENTRY * ctf_info) {
	int rv;
    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_oam_sat_ctf_info_db_delete(unit,ctf_id);
    BCMDNX_IF_ERR_EXIT(rv);
    rv =  _bcm_dpp_oam_sat_ctf_info_db_insert(unit,ctf_id,ctf_info);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}




int _bcm_dpp_oam_bfd_mep_info_db_insert(int unit, uint32 mep_index, SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY * mep_info) {
	int rv;
    BCMDNX_INIT_FUNC_DEFS_WITH_MEP_INFO_LOCK;

    rv =  _bcm_dpp_oam_bfd_mep_info_db_insert_no_lock(unit,mep_index,mep_info);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN_WITH_MEP_INFO_UNLOCK;
}
int _bcm_dpp_oam_bfd_mep_info_db_get(int unit, uint32 mep_index, SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY * mep_info, uint8 *found)
{
    int rv;

    BCMDNX_INIT_FUNC_DEFS_WITH_MEP_INFO_LOCK;
    rv = _bcm_dpp_sw_db_hash_oam_bfd_mep_info_find(unit,
                                        (sw_state_htb_key_t)&(mep_index),
                                        (sw_state_htb_data_t)(void *)(mep_info),
                                        FALSE);
    if (rv == BCM_E_NONE) {
        *found = TRUE;
        /*
         * Note that *mep_info has already been loaded in
         * _bcm_dpp_sw_db_hash_oam_bfd_mep_info_find() above.
         */
    }
    else if (rv == BCM_E_NOT_FOUND){
		*found = FALSE;
    }
	else {
		BCMDNX_IF_ERR_EXIT(rv);
	}

exit:
    BCMDNX_FUNC_RETURN_WITH_MEP_INFO_UNLOCK;
}

int _bcm_dpp_oam_bfd_mep_info_db_delete_no_lock(int unit, uint32 mep_index) {
    int rv;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY mep_info_dummy ;

    BCMDNX_INIT_FUNC_DEFS;
    rv = _bcm_dpp_sw_db_hash_oam_bfd_mep_info_find(unit,
                                        (sw_state_htb_key_t)&(mep_index),
                                        (sw_state_htb_data_t)(void *)(&mep_info_dummy),
                                        TRUE);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_oam_bfd_mep_info_db_delete(int unit, uint32 mep_index) {
	int rv;

    BCMDNX_INIT_FUNC_DEFS_WITH_MEP_INFO_LOCK;

    rv = _bcm_dpp_oam_bfd_mep_info_db_delete_no_lock(unit,mep_index);
	BCMDNX_IF_ERR_EXIT(rv);


exit:
    BCMDNX_FUNC_RETURN_WITH_MEP_INFO_UNLOCK;
}

int _bcm_dpp_oam_bfd_mep_info_db_update(int unit, uint32 mep_index, SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY * mep_info) {
	int rv;
    BCMDNX_INIT_FUNC_DEFS_WITH_MEP_INFO_LOCK;

    rv = _bcm_dpp_oam_bfd_mep_info_db_delete_no_lock(unit,mep_index);
    BCMDNX_IF_ERR_EXIT(rv);
    rv =  _bcm_dpp_oam_bfd_mep_info_db_insert_no_lock(unit,mep_index,mep_info);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN_WITH_MEP_INFO_UNLOCK;
}



/*
 * MEP to RMEP db
 */

int _bcm_dpp_oam_bfd_mep_to_rmep_db_insert(int unit, uint32 mep_index, uint32 rmep_index) {
    int rv;
    ENDPOINT_LIST_PTR rmep_list_p = 0 ;
    uint8 found;

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_sw_db_hash_oam_bfd_mep_to_rmep_find(unit,
                                        (sw_state_htb_key_t)&(mep_index),
                                        (sw_state_htb_data_t)(void *)(&rmep_list_p),
                                        TRUE);

    if (rv == BCM_E_NOT_FOUND)
    {
        rv = _bcm_dpp_oam_endpoint_list_init(unit, &rmep_list_p) ;
    }
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_dpp_oam_endpoint_list_member_find(unit, rmep_list_p, rmep_index, 0, &found);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!found)
    {
        /*if rmep index does not exist - add it */
        rv = _bcm_dpp_oam_endpoint_list_member_add(unit, rmep_list_p, rmep_index);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    rv = _bcm_dpp_sw_db_hash_oam_bfd_mep_to_rmep_insert(unit,
                                                (sw_state_htb_key_t)&(mep_index),
                                                (sw_state_htb_data_t)&(rmep_list_p));
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*The result of this get function is the member itself and not a copy*/
int _bcm_dpp_oam_bfd_mep_to_rmep_db_get(int unit, uint32 mep_index, ENDPOINT_LIST_PTR *rmep_list, uint8* found)
{
    int rv;

    BCMDNX_INIT_FUNC_DEFS;
    rv = _bcm_dpp_sw_db_hash_oam_bfd_mep_to_rmep_find(unit,
                                        (sw_state_htb_key_t)&(mep_index),
                                        (sw_state_htb_data_t)(void *)rmep_list,
                                        FALSE);
    if (rv == BCM_E_NOT_FOUND) {
        *found = FALSE;
    }
    else {
        BCMDNX_IF_ERR_EXIT(rv);
        *found = TRUE;
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_oam_bfd_mep_to_rmep_db_mep_delete(int unit, uint32 mep_index) {
    int rv;
    ENDPOINT_LIST_PTR rmep_list_p = 0 ;

    BCMDNX_INIT_FUNC_DEFS;
    rv = _bcm_dpp_sw_db_hash_oam_bfd_mep_to_rmep_find(unit,
                                        (sw_state_htb_key_t)&(mep_index),
                                        (sw_state_htb_data_t)(void *)&rmep_list_p,
                                        TRUE);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!SOC_IS_DETACHING(unit))
    {
        /*
         * This operation is not expected during 'deinit' (after new sw state is implemented)
         * so, for now, just avoid calling it while detaching. (Otherwise, an 'assert - like'
         * macro, SW_STATE_IS_DEINIT_CHECK, will fail the system.
         */
        rv = _bcm_dpp_oam_endpoint_list_deinit(unit, rmep_list_p);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int _bcm_dpp_oam_bfd_mep_to_rmep_db_rmep_delete(int unit, uint32 mep_index, uint32 rmep_index) {
    int rv, is_empty ;
    ENDPOINT_LIST_PTR rmep_list_p = 0 ;
    uint8 found;

    BCMDNX_INIT_FUNC_DEFS;
    rv = _bcm_dpp_sw_db_hash_oam_bfd_mep_to_rmep_find(unit,
                                        (sw_state_htb_key_t)&(mep_index),
                                        (sw_state_htb_data_t)(void *)&rmep_list_p,
                                        TRUE);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = _bcm_dpp_oam_endpoint_list_member_find(unit, rmep_list_p, rmep_index, 1, &found);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = _bcm_dpp_oam_endpoint_list_empty(unit, rmep_list_p, &is_empty) ;
    BCMDNX_IF_ERR_EXIT(rv);
    if (is_empty)
    {
        if (!SOC_IS_DETACHING(unit))
        {
            /*
             * This operation is not expected during 'deinit' (after new sw state is implemented)
             * so, for now, just avoid calling it while detaching. (Otherwise, an 'assert - like'
             * macro, SW_STATE_IS_DEINIT_CHECK, will fail the system.
             */
            rv = _bcm_dpp_oam_endpoint_list_deinit(unit, rmep_list_p) ;
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }
    else
    {
        rv = _bcm_dpp_sw_db_hash_oam_bfd_mep_to_rmep_insert(unit,
                                                (sw_state_htb_key_t)&(mep_index),
                                                (sw_state_htb_data_t)&(rmep_list_p));
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,(_BSL_BCM_MSG("Error: Remote endpoint %d not found\n"), rmep_index));
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}



/*
 * RMEP to MEP db
 */

/**
 * Should be called in OAM init and BFD init. May be called
 * twice.
 *
 * @author sinai (08/06/2015)
 *
 * @return int
 */
int _bcm_dpp_oam_sw_db_locks_init(int unit) {
    _bcm_dpp_oam_bfd_sw_db_lock_t *temp_rmep_state = &_bcm_dpp_oam_bfd_rmep_info_db_lock[unit];
    _bcm_dpp_oam_bfd_sw_db_lock_t *temp_mep_state = &_bcm_dpp_oam_bfd_mep_info_db_lock[unit];

    BCMDNX_INIT_FUNC_DEFS;

    if (temp_rmep_state->is_init==0) {
        sal_memset(temp_rmep_state, 0, sizeof(_bcm_dpp_oam_bfd_sw_db_lock_t));
        temp_rmep_state->lock = sal_mutex_create("_bcm_dpp_oam_bfd_rmep_info_db_lock");
        if (!temp_rmep_state->lock) {
             BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("Failed to create mutex\r\n")));
        }

        temp_rmep_state->is_init =1;
    }

    if (temp_mep_state->is_init == 0) {
        sal_memset(temp_mep_state, 0, sizeof(_bcm_dpp_oam_bfd_sw_db_lock_t));
        temp_mep_state->lock = sal_mutex_create("_bcm_dpp_oam_bfd_mep_info_db_lock");
        if (!temp_mep_state->lock) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("Failed to create mutex\r\n")));
        }

        temp_mep_state->is_init = 1;
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/**
 * Actually insert into the hash table.
 * Function should only be called after appropriate mutex has
 * been takem
 *
 *
 * @param unit
 * @param rmep_index
 * @param rmep_info
 *
 * @return int
 */
int _bcm_dpp_oam_bfd_rmep_info_db_insert_no_lock(int unit, uint32 rmep_index, SOC_PPC_OAM_RMEP_INFO_DATA * rmep_info) {
    int rv;
    /*
     * This variable is used here only as place holder.
     */
    SOC_PPC_OAM_RMEP_INFO_DATA rmep_info_loc ;

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_sw_db_hash_oam_bfd_rmep_info_find(unit,
                                        (sw_state_htb_key_t)&(rmep_index),
                                        (sw_state_htb_data_t)(void *)(&rmep_info_loc),
                                        FALSE);

    if (rv == BCM_E_NOT_FOUND) {
        rv = _bcm_dpp_sw_db_hash_oam_bfd_rmep_info_insert(unit,
                                                (sw_state_htb_key_t)&(rmep_index),
                                                (sw_state_htb_data_t)(rmep_info));
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("Error: Remote endpoint %d exists\n"), rmep_index));
    }

exit:
    BCMDNX_FUNC_RETURN;
}



int _bcm_dpp_oam_bfd_rmep_info_db_insert(int unit, uint32 rmep_index, SOC_PPC_OAM_RMEP_INFO_DATA * rmep_info) {
	int rv;

    BCMDNX_INIT_FUNC_DEFS_RMEP_INFO_DB_WITH_LOCK;

    rv = _bcm_dpp_oam_bfd_rmep_info_db_insert_no_lock(unit, rmep_index, rmep_info);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN_RMEP_INFO_DB_WITH_UNLOCK;
}

int _bcm_dpp_oam_bfd_rmep_info_db_get(int unit, uint32 rmep_index,  SOC_PPC_OAM_RMEP_INFO_DATA * rmep_info, uint8 *found)
{
    int rv;

    BCMDNX_INIT_FUNC_DEFS_RMEP_INFO_DB_WITH_LOCK;
    rv = _bcm_dpp_sw_db_hash_oam_bfd_rmep_info_find(unit,
                                        (sw_state_htb_key_t)&(rmep_index),
                                        (sw_state_htb_data_t)(void *)rmep_info,
                                        FALSE);

    if (rv == BCM_E_NONE) {
        *found = TRUE;
        /*
         * Note that *rmep_info has already been loaded in
         * _bcm_dpp_sw_db_hash_oam_bfd_rmep_info_find() above.
         */
    }
    else if (rv == BCM_E_NOT_FOUND){
        *found = FALSE;
    }
    else {
        BCMDNX_IF_ERR_EXIT(rv);
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN_RMEP_INFO_DB_WITH_UNLOCK;
}

/**
 * Actually delete from the hash table.
 * Function should only be called after appropriate mutex has
 * been takem
 *
 * @author sinai (08/06/2015)
 *
 * @param unit
 * @param rmep_index
 *
 * @return int
 */
int _bcm_dpp_oam_bfd_rmep_info_db_delete_no_lock(int unit, uint32 rmep_index) {
    int rv;
    SOC_PPC_OAM_RMEP_INFO_DATA rmep_info_dummy ;

    BCMDNX_INIT_FUNC_DEFS;
    rv = _bcm_dpp_sw_db_hash_oam_bfd_rmep_info_find(unit,
                                        (sw_state_htb_key_t)&rmep_index,
                                        (sw_state_htb_data_t)(void *)(&rmep_info_dummy),
                                        TRUE);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}



int _bcm_dpp_oam_bfd_rmep_info_db_delete(int unit, uint32 rmep_index) {
	int rv;
    BCMDNX_INIT_FUNC_DEFS_RMEP_INFO_DB_WITH_LOCK;

    rv = _bcm_dpp_oam_bfd_rmep_info_db_delete_no_lock(unit,rmep_index);
	BCMDNX_IF_ERR_EXIT(rv);


exit:
    BCMDNX_FUNC_RETURN_RMEP_INFO_DB_WITH_UNLOCK;
}


int _bcm_dpp_oam_bfd_rmep_to_mep_db_update(int unit, uint32 rmep_index, SOC_PPC_OAM_RMEP_INFO_DATA * rmep_info ) {
	int rv;
    BCMDNX_INIT_FUNC_DEFS_RMEP_INFO_DB_WITH_LOCK;

    rv = _bcm_dpp_oam_bfd_rmep_info_db_delete_no_lock(unit, rmep_index);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = _bcm_dpp_oam_bfd_rmep_info_db_insert_no_lock(unit, rmep_index, rmep_info);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN_RMEP_INFO_DB_WITH_UNLOCK;
}


/*
 * End of DB functions
 */

/*********************************************************************
* NAME:
*   bcm_dpp_endp_lst_init
* TYPE:
*   PROC
* DATE:
*   June 09 2015
* FUNCTION:
*   Initialize control structure for ALL enpoint list
*   instances expected.
* INPUT:
*   int unit -
*     Identifier of the device to access.
*   uint32 max_nof_endp_lsts -
*     Maximal number of end point lists which can be sustained
*     simultaneously.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*     This procedure may be called twice, depending on scenario. It
*     protects itself by checking on it and skipping the second,
*     redundant, initialization.
 */
uint32
  bcm_dpp_endp_lst_init(
    SOC_SAND_IN       int                          unit,
    SOC_SAND_IN       uint32                       max_nof_endp_lsts
  )
{
  uint32 res ;
  uint8 is_allocated ;

  BCMDNX_INIT_FUNC_DEFS ;
  BCM_DPP_ENDP_LST_VERIFY_UNIT_IS_LEGAL(unit) ;

  res = ENDP_LST_ACCESS.is_allocated(unit,&is_allocated);
  BCMDNX_IF_ERR_EXIT(res) ;
  /*
   * If already initialized then just return without error.
   */
  if (!is_allocated)
  {
    res = ENDP_LST_ACCESS.alloc(unit);
    BCMDNX_IF_ERR_EXIT(res) ;

    res = ENDP_LST_ACCESS.endp_lsts_array.ptr_alloc(unit, max_nof_endp_lsts);
    BCMDNX_IF_ERR_EXIT(res) ;

    res = ENDP_LST_ACCESS.max_nof_endp_lsts.set(unit, max_nof_endp_lsts);
    BCMDNX_IF_ERR_EXIT(res) ;

    res = ENDP_LST_ACCESS.in_use.set(unit, 0);
    BCMDNX_IF_ERR_EXIT(res) ;

    res = ENDP_LST_ACCESS.occupied_endp_lsts.alloc_bitmap(unit, max_nof_endp_lsts);
    BCMDNX_IF_ERR_EXIT(res) ;
  }
exit:
  BCMDNX_FUNC_RETURN ;
}
/*********************************************************************
* NAME:
*   bcm_dpp_endp_lst_m_init
* TYPE:
*   PROC
* DATE:
*   June 09 2015
* FUNCTION:
*   Initialize control structure for ALL enpoint list member
*   instances expected.
* INPUT:
*   int unit -
*     Identifier of the device to access.
*   uint32 max_nof_endp_lst_ms -
*     Maximal number of end point list members which can be sustained
*     simultaneously.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*     This procedure may be called twice, depending on scenario. It
*     protects itself by checking on it and skipping the second,
*     redundant, initialization.
 */
uint32
  bcm_dpp_endp_lst_m_init(
    SOC_SAND_IN       int                          unit,
    SOC_SAND_IN       uint32                       max_nof_endp_lst_ms
  )
{
  uint32 res ;
  uint8 is_allocated ;

  BCMDNX_INIT_FUNC_DEFS ;
  BCM_DPP_ENDP_LST_M_VERIFY_UNIT_IS_LEGAL(unit) ;

  res = ENDP_LST_M_ACCESS.is_allocated(unit,&is_allocated);
  BCMDNX_IF_ERR_EXIT(res) ;
  /*
   * If already initialized then just return without error.
   */
  if (!is_allocated)
  {
    res = ENDP_LST_M_ACCESS.alloc(unit);
    BCMDNX_IF_ERR_EXIT(res) ;

    res = ENDP_LST_M_ACCESS.endp_lst_ms_array.ptr_alloc(unit, max_nof_endp_lst_ms) ;
    BCMDNX_IF_ERR_EXIT(res) ;

    res = ENDP_LST_M_ACCESS.max_nof_endp_lst_ms.set(unit, max_nof_endp_lst_ms) ;
    BCMDNX_IF_ERR_EXIT(res) ;

    res = ENDP_LST_M_ACCESS.in_use.set(unit, 0) ;
    BCMDNX_IF_ERR_EXIT(res) ;

    res = ENDP_LST_M_ACCESS.occupied_endp_lst_ms.alloc_bitmap(unit, max_nof_endp_lst_ms) ;
    BCMDNX_IF_ERR_EXIT(res) ;
  }
exit:
  BCMDNX_FUNC_RETURN ;
}
/*
 * This procedure returns handle to a newly allocated endpoint list
 * If no free endpoint list is found, *endpoint_list_ptr is loaded
 * by '0'.
 */
int
_bcm_dpp_oam_endpoint_list_init(int unit,ENDPOINT_LIST_PTR *endpoint_list_ptr)
{
  uint32
    res,
    endp_lst_index,
    found,
    max_nof_endp_lsts,
    in_use ;
  uint8
    bit_val ;
  ENDPOINT_LIST_PTR
    endp_lst ;

  BCMDNX_INIT_FUNC_DEFS ;
  BCM_DPP_ENDP_LST_VERIFY_UNIT_IS_LEGAL(unit) ;

  BCMDNX_NULL_CHECK(endpoint_list_ptr);
  *endpoint_list_ptr = 0 ;

  res = ENDP_LST_ACCESS.in_use.get(unit, &in_use);
  BCMDNX_IF_ERR_EXIT(res) ;

  res = ENDP_LST_ACCESS.max_nof_endp_lsts.get(unit, &max_nof_endp_lsts);
  BCMDNX_IF_ERR_EXIT(res) ;

  if (in_use >= max_nof_endp_lsts)
  {
    /*
     * If number of occupied bitmap structures is beyond the
     * maximum then quit with error.
     */
    res = BCM_E_UNAVAIL ;
    BCMDNX_IF_ERR_EXIT(res) ;
  }
  /*
   * Increment number of 'in_use' to cover the one we now intend to capture.
   */
  res = ENDP_LST_ACCESS.in_use.set(unit, (in_use + 1));
  BCMDNX_IF_ERR_EXIT(res) ;
  /*
   * Find a free endpoint list (a cleared bit in 'occupied_endp_lsts'). At this point,
   * there must be one.
   */
  found = 0 ;
  for (endp_lst_index = 0 ; endp_lst_index < max_nof_endp_lsts ; endp_lst_index++)
  {
    res = ENDP_LST_ACCESS.occupied_endp_lsts.bit_get(unit, endp_lst_index, &bit_val);
    BCMDNX_IF_ERR_EXIT(res) ;
    if (bit_val == 0)
    {
      /*
       * 'endp_lst_index' is now the index of a free entry.
       */
      found = 1 ;
      break ;
    }
  }
  if (!found)
  {
    res = BCM_E_MEMORY ;
    BCMDNX_IF_ERR_EXIT(res) ;
  }
  res = ENDP_LST_ACCESS.occupied_endp_lsts.bit_set(unit, endp_lst_index);
  BCMDNX_IF_ERR_EXIT(res) ;

  res = ENDP_LST_ACCESS_DATA.alloc(unit, endp_lst_index);
  BCMDNX_IF_ERR_EXIT(res) ;
  /*
   * Note that legal handles start at '1', not at '0'.
   */
  BCM_DPP_ENDP_LST_CONVERT_ENDPLST_INDEX_TO_HANDLE(endp_lst,endp_lst_index) ;
  /*
   * Set output of this procedure.
   */
  *endpoint_list_ptr = endp_lst ;

  res = ENDP_LST_ACCESS_DATA.first_member.set(unit, endp_lst_index, 0) ;
  BCMDNX_IF_ERR_EXIT(res) ;
  res = ENDP_LST_ACCESS_DATA.size.set(unit, endp_lst_index, 0) ;
  BCMDNX_IF_ERR_EXIT(res) ;
exit:
  BCMDNX_FUNC_RETURN ;
}
/*
 * Get 'first_member' of a specified endpoint list
 * If no error is indicated, first member is loaded into *first_member_ptr
 */
int
_bcm_dpp_oam_endpoint_list_get_first_member(int unit, ENDPOINT_LIST_PTR endpoint_list, ENDPOINT_LIST_MEMBER_PTR *first_member_ptr)
{
  uint32
    res,
    endp_lst_index ;

  BCMDNX_INIT_FUNC_DEFS ;
  BCM_DPP_ENDP_LST_VERIFY_UNIT_IS_LEGAL(unit) ;
  BCM_DPP_ENDP_LST_CONVERT_HANDLE_TO_ENDPLST_INDEX(endp_lst_index,endpoint_list) ;
  BCM_DPP_ENDP_LST_VERIFY_ENDPLST_IS_ACTIVE(endp_lst_index) ;

  res = ENDP_LST_ACCESS_DATA.first_member.get(unit, endp_lst_index, first_member_ptr) ;
  BCMDNX_IF_ERR_EXIT(res) ;
exit:
  BCMDNX_FUNC_RETURN;
}
/*
 * Set 'first_member' of a specified endpoint list
 * If no error is indicated, first member is loaded by 'first_member'
 */
int
_bcm_dpp_oam_endpoint_list_set_first_member(int unit, ENDPOINT_LIST_PTR endpoint_list, ENDPOINT_LIST_MEMBER_PTR first_member)
{
  uint32
    res,
    endp_lst_index ;

  BCMDNX_INIT_FUNC_DEFS ;
  BCM_DPP_ENDP_LST_VERIFY_UNIT_IS_LEGAL(unit) ;
  BCM_DPP_ENDP_LST_CONVERT_HANDLE_TO_ENDPLST_INDEX(endp_lst_index,endpoint_list) ;
  BCM_DPP_ENDP_LST_VERIFY_ENDPLST_IS_ACTIVE(endp_lst_index) ;

  res = ENDP_LST_ACCESS_DATA.first_member.set(unit, endp_lst_index, first_member) ;
  BCMDNX_IF_ERR_EXIT(res) ;
exit:
  BCMDNX_FUNC_RETURN;
}
/*
 * Get 'index' of a specified endpoint list member
 * If no error is indicated, 'index' is loaded into *index_ptr
 */
int
_bcm_dpp_oam_endpoint_list_get_member_index(int unit, ENDPOINT_LIST_MEMBER_PTR endpoint_list_member, uint32 *index_ptr)
{
  uint32
    res,
    endp_lst_m_index ;

  BCMDNX_INIT_FUNC_DEFS ;
  BCM_DPP_ENDP_LST_VERIFY_UNIT_IS_LEGAL(unit) ;
  BCM_DPP_ENDP_LST_M_CONVERT_HANDLE_TO_ENDPLSTM_INDEX(endp_lst_m_index,endpoint_list_member) ;
  BCM_DPP_ENDP_LST_M_VERIFY_ENDPLSTM_IS_ACTIVE(endp_lst_m_index) ;

  res = ENDP_LST_M_ACCESS_DATA.index.get(unit, endp_lst_m_index, index_ptr) ;
  BCMDNX_IF_ERR_EXIT(res) ;
exit:
  BCMDNX_FUNC_RETURN;
}
/*
 * Set 'index' of a specified endpoint list member
 * If no error is indicated, 'index' is loaded by 'index'
 */
int
_bcm_dpp_oam_endpoint_list_set_member_index(int unit, ENDPOINT_LIST_MEMBER_PTR endpoint_list_member, uint32 index)
{
  uint32
    res,
    endp_lst_m_index ;

  BCMDNX_INIT_FUNC_DEFS ;
  BCM_DPP_ENDP_LST_VERIFY_UNIT_IS_LEGAL(unit) ;
  BCM_DPP_ENDP_LST_M_CONVERT_HANDLE_TO_ENDPLSTM_INDEX(endp_lst_m_index,endpoint_list_member) ;
  BCM_DPP_ENDP_LST_M_VERIFY_ENDPLSTM_IS_ACTIVE(endp_lst_m_index) ;

  res = ENDP_LST_M_ACCESS_DATA.index.set(unit, endp_lst_m_index, index) ;
  BCMDNX_IF_ERR_EXIT(res) ;
exit:
  BCMDNX_FUNC_RETURN;
}
/*
 * Get 'next' of a specified endpoint list member
 * If no error is indicated, 'next' is loaded into *next_ptr
 */
int
_bcm_dpp_oam_endpoint_list_get_member_next(int unit, ENDPOINT_LIST_MEMBER_PTR endpoint_list_member, ENDPOINT_LIST_MEMBER_PTR *next_ptr)
{
  uint32
    res,
    endp_lst_m_index ;

  BCMDNX_INIT_FUNC_DEFS ;
  BCM_DPP_ENDP_LST_VERIFY_UNIT_IS_LEGAL(unit) ;
  BCM_DPP_ENDP_LST_M_CONVERT_HANDLE_TO_ENDPLSTM_INDEX(endp_lst_m_index,endpoint_list_member) ;
  BCM_DPP_ENDP_LST_M_VERIFY_ENDPLSTM_IS_ACTIVE(endp_lst_m_index) ;

  res = ENDP_LST_M_ACCESS_DATA.next.get(unit, endp_lst_m_index, next_ptr) ;
  BCMDNX_IF_ERR_EXIT(res) ;
exit:
  BCMDNX_FUNC_RETURN;
}
/*
 * Set 'next' of a specified endpoint list member
 * If no error is indicated, 'next' is loaded by 'next'
 */
int
_bcm_dpp_oam_endpoint_list_set_member_next(int unit, ENDPOINT_LIST_MEMBER_PTR endpoint_list_member, ENDPOINT_LIST_MEMBER_PTR next)
{
  uint32
    res,
    endp_lst_m_index ;

  BCMDNX_INIT_FUNC_DEFS ;
  BCM_DPP_ENDP_LST_VERIFY_UNIT_IS_LEGAL(unit) ;
  BCM_DPP_ENDP_LST_M_CONVERT_HANDLE_TO_ENDPLSTM_INDEX(endp_lst_m_index,endpoint_list_member) ;
  BCM_DPP_ENDP_LST_M_VERIFY_ENDPLSTM_IS_ACTIVE(endp_lst_m_index) ;

  res = ENDP_LST_M_ACCESS_DATA.next.set(unit, endp_lst_m_index, next) ;
  BCMDNX_IF_ERR_EXIT(res) ;
exit:
  BCMDNX_FUNC_RETURN;
}
/*
 * Allocate a new endpoint list member and add it to the list as 'first_member'
 * After this add operation, the new member points to the member which used to
 * be 'first_member'.
 */
int
_bcm_dpp_oam_endpoint_list_member_add(int unit, ENDPOINT_LIST_PTR endpoint_list, uint32 endpoint_index)
{
  uint32
    res,
    endp_lst_index,
    endp_lst_m_index,
    found,
    max_nof_endp_lst_ms,
    in_use ;
  uint8
    bit_val ;
  uint32
    size ;
  ENDPOINT_LIST_MEMBER_PTR
    endpoint_p ;

  BCMDNX_INIT_FUNC_DEFS ;
  BCM_DPP_ENDP_LST_VERIFY_UNIT_IS_LEGAL(unit) ;
  BCM_DPP_ENDP_LST_CONVERT_HANDLE_TO_ENDPLST_INDEX(endp_lst_index,endpoint_list) ;
  BCM_DPP_ENDP_LST_VERIFY_ENDPLST_IS_ACTIVE(endp_lst_index) ;
  /*
   * Endpoint list is active.
   * Search for free endpoint list member to attach to specified endpoint list
   */
  res = ENDP_LST_M_ACCESS.in_use.get(unit, &in_use);
  BCMDNX_IF_ERR_EXIT(res) ;

  res = ENDP_LST_M_ACCESS.max_nof_endp_lst_ms.get(unit, &max_nof_endp_lst_ms);
  BCMDNX_IF_ERR_EXIT(res) ;

  if (in_use >= max_nof_endp_lst_ms)
  {
    /*
     * If number of occupied bitmap structures is beyond the
     * maximum then quit with error.
     */
    res = BCM_E_UNAVAIL ;
    BCMDNX_IF_ERR_EXIT(res) ;
  }
  /*
   * Increment number of 'in_use' to cover the one we now intend to capture.
   */
  res = ENDP_LST_M_ACCESS.in_use.set(unit, (in_use + 1));
  BCMDNX_IF_ERR_EXIT(res) ;
  /*
   * Find a free endpoint list member (a cleared bit in 'occupied_endp_lst_ms'). At this point,
   * there must be one.
   */
  found = 0 ;
  for (endp_lst_m_index = 0 ; endp_lst_m_index < max_nof_endp_lst_ms ; endp_lst_m_index++)
  {
    res = ENDP_LST_M_ACCESS.occupied_endp_lst_ms.bit_get(unit, endp_lst_m_index, &bit_val);
    BCMDNX_IF_ERR_EXIT(res) ;
    if (bit_val == 0)
    {
      /*
       * 'endp_lst_m_index' is now the index of a free entry.
       */
      found = 1 ;
      break ;
    }
  }
  if (!found)
  {
    BCMDNX_ERR_EXIT_MSG(
      BCM_E_MEMORY,
      (_BSL_BCM_MSG("Error: No more free list members. All %d are used."),max_nof_endp_lst_ms));
  }
  res = ENDP_LST_M_ACCESS.occupied_endp_lst_ms.bit_set(unit, endp_lst_m_index);
  BCMDNX_IF_ERR_EXIT(res) ;

  res = ENDP_LST_M_ACCESS.endp_lst_ms_array.alloc(unit, endp_lst_m_index);
  BCMDNX_IF_ERR_EXIT(res) ;
  /*
   * Insert new member as first on linked list (pointed by head).
   */
  res = ENDP_LST_M_ACCESS_DATA.index.set(unit, endp_lst_m_index, endpoint_index) ;
  BCMDNX_IF_ERR_EXIT(res) ;
  res = ENDP_LST_ACCESS_DATA.first_member.get(unit, endp_lst_index, &endpoint_p) ;
  BCMDNX_IF_ERR_EXIT(res) ;
  res = ENDP_LST_M_ACCESS_DATA.next.set(unit, endp_lst_m_index, endpoint_p) ;
  BCMDNX_IF_ERR_EXIT(res) ;
  /*
   * Note that legal handles start at '1', not at '0'.
   */
  BCM_DPP_ENDP_LST_M_CONVERT_ENDPLSTM_INDEX_TO_HANDLE(endpoint_p,endp_lst_m_index) ;
  res = ENDP_LST_ACCESS_DATA.first_member.set(unit, endp_lst_index, endpoint_p) ;
  BCMDNX_IF_ERR_EXIT(res) ;
  res = ENDP_LST_ACCESS_DATA.size.get(unit, endp_lst_index, &size) ;
  BCMDNX_IF_ERR_EXIT(res) ;
  size += 1 ;
  res = ENDP_LST_ACCESS_DATA.size.set(unit, endp_lst_index, size) ;
  BCMDNX_IF_ERR_EXIT(res) ;
exit:
  BCMDNX_FUNC_RETURN;
}
/*
 * Allocate a new endpoint list member and add it after specified member.
 * Load new member by 'endpoint_index'
 * After this add operation, the new member points to the member which used to
 * be pointed by 'specified member'.
 * The handle of the newly allocated member is loaded, as output, into '*new_member_ptr'
 */
int
_bcm_dpp_oam_endpoint_list_member_add_after(
  int unit, ENDPOINT_LIST_PTR endpoint_list, uint32 endpoint_index,
  ENDPOINT_LIST_MEMBER_PTR specified_member, ENDPOINT_LIST_MEMBER_PTR *new_member_ptr)
{
  uint32
    res,
    endp_lst_index,
    endp_lst_m_index,
    endp_lst_m_specified_index,
    found,
    max_nof_endp_lst_ms,
    in_use ;
  uint8
    bit_val ;
  uint32
    size ;
  ENDPOINT_LIST_MEMBER_PTR
    endpoint_p ;
  ENDPOINT_LIST_MEMBER_PTR
    member_next ;

  BCMDNX_INIT_FUNC_DEFS ;
  BCM_DPP_ENDP_LST_VERIFY_UNIT_IS_LEGAL(unit) ;
  BCM_DPP_ENDP_LST_CONVERT_HANDLE_TO_ENDPLST_INDEX(endp_lst_index,endpoint_list) ;
  BCM_DPP_ENDP_LST_VERIFY_ENDPLST_IS_ACTIVE(endp_lst_index) ;
  BCM_DPP_ENDP_LST_M_CONVERT_HANDLE_TO_ENDPLSTM_INDEX(endp_lst_m_specified_index,specified_member) ;
  BCM_DPP_ENDP_LST_M_VERIFY_ENDPLSTM_IS_ACTIVE(endp_lst_m_specified_index) ;
  /*
   * Endpoint list is active.
   * Search for free endpoint list member to attach to specified endpoint list
   */
  res = ENDP_LST_M_ACCESS.in_use.get(unit, &in_use);
  BCMDNX_IF_ERR_EXIT(res) ;

  res = ENDP_LST_M_ACCESS.max_nof_endp_lst_ms.get(unit, &max_nof_endp_lst_ms);
  BCMDNX_IF_ERR_EXIT(res) ;

  if (in_use >= max_nof_endp_lst_ms)
  {
    /*
     * If number of occupied bitmap structures is beyond the
     * maximum then quit with error.
     */
    res = BCM_E_UNAVAIL ;
    BCMDNX_IF_ERR_EXIT(res) ;
  }
  /*
   * Increment number of 'in_use' to cover the one we now intend to capture.
   */
  res = ENDP_LST_M_ACCESS.in_use.set(unit, (in_use + 1));
  BCMDNX_IF_ERR_EXIT(res) ;
  /*
   * Find a free endpoint list (a cleared bit in 'occupied_endp_lsts'). At this point,
   * there must be one.
   */
  found = 0 ;
  for (endp_lst_m_index = 0 ; endp_lst_m_index < max_nof_endp_lst_ms ; endp_lst_m_index++)
  {
    res = ENDP_LST_M_ACCESS.occupied_endp_lst_ms.bit_get(unit, endp_lst_m_index, &bit_val);
    BCMDNX_IF_ERR_EXIT(res) ;
    if (bit_val == 0)
    {
      /*
       * 'endp_lst_m_index' is now the index of a free entry.
       */
      found = 1 ;
      break ;
    }
  }
  if (!found)
  {
    res = BCM_E_MEMORY ;
    BCMDNX_IF_ERR_EXIT(res) ;
  }
  res = ENDP_LST_M_ACCESS.occupied_endp_lst_ms.bit_set(unit, endp_lst_m_index);
  BCMDNX_IF_ERR_EXIT(res) ;

  res = ENDP_LST_M_ACCESS.endp_lst_ms_array.alloc(unit, endp_lst_m_index);
  BCMDNX_IF_ERR_EXIT(res) ;
  BCM_DPP_ENDP_LST_M_CONVERT_ENDPLSTM_INDEX_TO_HANDLE(endpoint_p,endp_lst_m_index) ;
  *new_member_ptr = endpoint_p ;
  /*
   * Insert new member after specified member on linked list.
   */
  res = ENDP_LST_M_ACCESS_DATA.index.set(unit, endp_lst_m_index, endpoint_index) ;
  BCMDNX_IF_ERR_EXIT(res) ;
  /*
   * Get 'next' of specified member...
   */
  res = ENDP_LST_M_ACCESS_DATA.next.get(unit, endp_lst_m_specified_index, &member_next) ;
  BCMDNX_IF_ERR_EXIT(res) ;
  /*
   * and make it 'next of new member
   */
  res = ENDP_LST_M_ACCESS_DATA.next.set(unit, endp_lst_m_index, member_next) ;
  BCMDNX_IF_ERR_EXIT(res) ;
  /*
   * Set new member to be 'next' of specified member.
   */
  res = ENDP_LST_M_ACCESS_DATA.next.set(unit, endp_lst_m_specified_index, endpoint_p) ;
  BCMDNX_IF_ERR_EXIT(res) ;
  res = ENDP_LST_ACCESS_DATA.size.get(unit, endp_lst_index, &size) ;
  BCMDNX_IF_ERR_EXIT(res) ;
  size += 1 ;
  res = ENDP_LST_ACCESS_DATA.size.set(unit, endp_lst_index, size) ;
  BCMDNX_IF_ERR_EXIT(res) ;
exit:
  BCMDNX_FUNC_RETURN;
}
/*
 * This procedure returns an error code.
 * If no error is detected, *is_empty is loaded by a non-zero value
 * if endpoint list is empty (has no members).
 */
int
_bcm_dpp_oam_endpoint_list_empty(int unit, ENDPOINT_LIST_PTR endpoint_list, int *is_empty)
{
  uint32
    res,
    endp_lst_index ;
  ENDPOINT_LIST_MEMBER_PTR
    endpoint_p ;

  BCMDNX_INIT_FUNC_DEFS ;
  BCMDNX_NULL_CHECK(is_empty);
  BCM_DPP_ENDP_LST_VERIFY_UNIT_IS_LEGAL(unit) ;
  BCM_DPP_ENDP_LST_CONVERT_HANDLE_TO_ENDPLST_INDEX(endp_lst_index,endpoint_list) ;
  BCM_DPP_ENDP_LST_VERIFY_ENDPLST_IS_ACTIVE(endp_lst_index) ;

  res = ENDP_LST_ACCESS_DATA.first_member.get(unit, endp_lst_index, &endpoint_p) ;
  BCMDNX_IF_ERR_EXIT(res) ;
  if (endpoint_p == 0)
  {
    *is_empty = 1 ;
  }
  else
  {
    *is_empty = 0 ;
  }
exit:
  BCMDNX_FUNC_RETURN ;
}

int
_bcm_dpp_oam_endpoint_list_member_find(int unit, ENDPOINT_LIST_PTR endpoint_list, uint32 index, uint8 delete_endpoint, uint8 *found)
{
  uint32
    res,
    endp_lst_index,
    endp_lst_m_index,
    member_index,
    size,
    in_use ;
  ENDPOINT_LIST_MEMBER_PTR
    endpoint_p ;
  ENDPOINT_LIST_MEMBER_PTR
    endpoint_prev_p ;
  ENDPOINT_LIST_MEMBER_PTR
    member_next ;

  BCMDNX_INIT_FUNC_DEFS ;
  BCMDNX_NULL_CHECK(found);
  BCM_DPP_ENDP_LST_VERIFY_UNIT_IS_LEGAL(unit) ;
  BCM_DPP_ENDP_LST_CONVERT_HANDLE_TO_ENDPLST_INDEX(endp_lst_index,endpoint_list) ;
  BCM_DPP_ENDP_LST_VERIFY_ENDPLST_IS_ACTIVE(endp_lst_index) ;

  res = ENDP_LST_ACCESS_DATA.first_member.get(unit, endp_lst_index, &endpoint_prev_p) ;
  BCMDNX_IF_ERR_EXIT(res) ;
  if (endpoint_prev_p == 0)
  {
    *found = FALSE ;
    goto exit ;
  }
  BCM_DPP_ENDP_LST_M_CONVERT_HANDLE_TO_ENDPLSTM_INDEX(endp_lst_m_index,endpoint_prev_p) ;
  res = ENDP_LST_M_ACCESS_DATA.index.get(unit, endp_lst_m_index, &member_index) ;
  BCMDNX_IF_ERR_EXIT(res) ;
  if (member_index == index)
 {
    *found = TRUE;
    if (delete_endpoint)
    {
      res = ENDP_LST_M_ACCESS_DATA.next.get(unit, endp_lst_m_index, &member_next) ;
      BCMDNX_IF_ERR_EXIT(res) ;
      res = ENDP_LST_ACCESS_DATA.first_member.set(unit, endp_lst_index, member_next) ;
      BCMDNX_IF_ERR_EXIT(res) ;
      /*
       * Mark member as NOT being in use.
       */
      res = ENDP_LST_M_ACCESS.in_use.get(unit, &in_use);
      BCMDNX_IF_ERR_EXIT(res) ;
      if ((int)in_use <= 0)
      {
        /*
         * If number of occupied bitmap structures is to go below zero
         * then quit with error.
         */
        res = BCM_E_UNAVAIL ;
        BCMDNX_IF_ERR_EXIT(res) ;
      }
      /*
       * Decrement number of 'in_use' to cover the one we now intend to free.
       */
      res = ENDP_LST_M_ACCESS.in_use.set(unit, (in_use - 1));
      BCMDNX_IF_ERR_EXIT(res) ;
      res = ENDP_LST_M_ACCESS.occupied_endp_lst_ms.bit_clear(unit, endp_lst_m_index);
      BCMDNX_IF_ERR_EXIT(res) ;
      res = ENDP_LST_M_ACCESS.endp_lst_ms_array.free(unit, endp_lst_m_index);
      BCMDNX_IF_ERR_EXIT(res) ;
      res = ENDP_LST_ACCESS_DATA.size.get(unit, endp_lst_index, &size) ;
      BCMDNX_IF_ERR_EXIT(res) ;
      size -= 1 ;
      res = ENDP_LST_ACCESS_DATA.size.set(unit, endp_lst_index, size) ;
      BCMDNX_IF_ERR_EXIT(res) ;
    }
    goto exit ;
  }
  res = ENDP_LST_M_ACCESS_DATA.next.get(unit, endp_lst_m_index, &endpoint_p) ;
  BCMDNX_IF_ERR_EXIT(res) ;
  *found = FALSE;
  while (endpoint_p != 0)
  {
    BCM_DPP_ENDP_LST_M_CONVERT_HANDLE_TO_ENDPLSTM_INDEX(endp_lst_m_index,endpoint_p) ;
    res = ENDP_LST_M_ACCESS_DATA.index.get(unit, endp_lst_m_index, &member_index) ;
    BCMDNX_IF_ERR_EXIT(res) ;
    if (member_index == index)
    {
      *found = TRUE ;
      break;
    }
    endpoint_prev_p = endpoint_p ;
    res = ENDP_LST_M_ACCESS_DATA.next.get(unit, endp_lst_m_index, &endpoint_p) ;
    BCMDNX_IF_ERR_EXIT(res) ;
  }
  if (*found == TRUE)
  {
    if (delete_endpoint)
    {
      /*
       * Get 'next' of the member to delete into 'endpoint_p'
       */
      BCM_DPP_ENDP_LST_M_CONVERT_HANDLE_TO_ENDPLSTM_INDEX(endp_lst_m_index,endpoint_p) ;
      res = ENDP_LST_M_ACCESS_DATA.next.get(unit, endp_lst_m_index, &endpoint_p) ;
      BCMDNX_IF_ERR_EXIT(res) ;
      /*
       * Set member as 'not occupied' and free memory.
       */
      res = ENDP_LST_M_ACCESS.occupied_endp_lst_ms.bit_clear(unit, endp_lst_m_index);
      BCMDNX_IF_ERR_EXIT(res) ;
      res = ENDP_LST_M_ACCESS.endp_lst_ms_array.free(unit, endp_lst_m_index);
      BCMDNX_IF_ERR_EXIT(res) ;
      /*
       * Load 'next' of the previous member by 'next' of the one deleted.
       */
      BCM_DPP_ENDP_LST_M_CONVERT_HANDLE_TO_ENDPLSTM_INDEX(endp_lst_m_index,endpoint_prev_p) ;
      res = ENDP_LST_M_ACCESS_DATA.next.set(unit, endp_lst_m_index, endpoint_p) ;
      BCMDNX_IF_ERR_EXIT(res) ;
      /*
       * Decrement number of members 'in use'.
       */
      res = ENDP_LST_M_ACCESS.in_use.get(unit, &in_use) ;
      BCMDNX_IF_ERR_EXIT(res) ;
      if ((int)in_use <= 0)
      {
        /*
         * If number of occupied bitmap structures is to go below zero
         * then quit with error.
         */
        res = BCM_E_UNAVAIL ;
        BCMDNX_IF_ERR_EXIT(res) ;
      }
      res = ENDP_LST_M_ACCESS.in_use.set(unit, (in_use - 1)) ;
      BCMDNX_IF_ERR_EXIT(res) ;
      /*
       * Decrement 'size' (number of members) of endpoint list.
       */
      res = ENDP_LST_ACCESS_DATA.size.get(unit, endp_lst_index, &size) ;
      BCMDNX_IF_ERR_EXIT(res) ;
      size -= 1 ;
      res = ENDP_LST_ACCESS_DATA.size.set(unit, endp_lst_index, size) ;
      BCMDNX_IF_ERR_EXIT(res) ;
    }
  }
exit:
  BCMDNX_FUNC_RETURN;
}

/* Free all list members but not list pointer itself */
int
_bcm_dpp_oam_endpoint_list_destroy(int unit, ENDPOINT_LIST_PTR endpoint_list)
{
  ENDPOINT_LIST_MEMBER_PTR
    endpoint_p ;
  uint32
    res,
    in_use,
    endp_lst_index,
    endp_lst_m_index ;

  BCMDNX_INIT_FUNC_DEFS;
  BCM_DPP_ENDP_LST_VERIFY_UNIT_IS_LEGAL(unit) ;
  BCM_DPP_ENDP_LST_CONVERT_HANDLE_TO_ENDPLST_INDEX(endp_lst_index,endpoint_list) ;
  BCM_DPP_ENDP_LST_VERIFY_ENDPLST_IS_ACTIVE(endp_lst_index) ;
  /*
   * Get 'first_member' on head into 'endpoint_p'
   */
  res = ENDP_LST_ACCESS_DATA.first_member.get(unit, endp_lst_index, &endpoint_p) ;
  BCMDNX_IF_ERR_EXIT(res) ;
  while (endpoint_p != 0)
  {
    /*
     * Get 'next' of the member to delete into 'endpoint_p'
     * Note: 'endp_lst_m_index' is the index of the member to delete.
     */
    BCM_DPP_ENDP_LST_M_CONVERT_HANDLE_TO_ENDPLSTM_INDEX(endp_lst_m_index,endpoint_p) ;
    res = ENDP_LST_M_ACCESS_DATA.next.get(unit, endp_lst_m_index, &endpoint_p) ;
    BCMDNX_IF_ERR_EXIT(res) ;
    /*
     * Set member as 'not occupied' and free memory.
     */
    res = ENDP_LST_M_ACCESS.occupied_endp_lst_ms.bit_clear(unit, endp_lst_m_index);
    BCMDNX_IF_ERR_EXIT(res) ;
    res = ENDP_LST_M_ACCESS.endp_lst_ms_array.free(unit, endp_lst_m_index);
    BCMDNX_IF_ERR_EXIT(res) ;
    /*
     * Decrement number of members 'in use'.
     */
    res = ENDP_LST_M_ACCESS.in_use.get(unit, &in_use) ;
    BCMDNX_IF_ERR_EXIT(res) ;
    if ((int)in_use <= 0)
    {
      /*
       * If number of occupied bitmap structures is to go below zero
       * then quit with error.
       */
      res = BCM_E_UNAVAIL ;
      BCMDNX_IF_ERR_EXIT(res) ;
    }
    res = ENDP_LST_M_ACCESS.in_use.set(unit, (in_use - 1)) ;
    BCMDNX_IF_ERR_EXIT(res) ;
    /*
     * Note that 'endpoint_p' is now the member which was 'next' to the one just deleted.
     */
  }
  /*
   * Set 'first_member' on head to zero to indicate endpoint list is empty
   */
  res = ENDP_LST_ACCESS_DATA.first_member.set(unit, endp_lst_index, 0) ;
  BCMDNX_IF_ERR_EXIT(res) ;
  /*
   * Set counter of members (on head) to zero.
   */
  res = ENDP_LST_ACCESS_DATA.size.set(unit, endp_lst_index, 0) ;
  BCMDNX_IF_ERR_EXIT(res) ;
exit:
  BCMDNX_FUNC_RETURN ;
}

/* Free all list members AND list pointer itself */
int
_bcm_dpp_oam_endpoint_list_deinit(int unit, ENDPOINT_LIST_PTR endpoint_list)
{
  ENDPOINT_LIST_MEMBER_PTR
    endpoint_p ;
  uint32
    res,
    endp_lst_index,
    endp_lst_m_index ;
  uint32
    in_use ;

  BCMDNX_INIT_FUNC_DEFS;
  BCM_DPP_ENDP_LST_VERIFY_UNIT_IS_LEGAL(unit) ;
  BCM_DPP_ENDP_LST_CONVERT_HANDLE_TO_ENDPLST_INDEX(endp_lst_index,endpoint_list) ;
  BCM_DPP_ENDP_LST_VERIFY_ENDPLST_IS_ACTIVE(endp_lst_index) ;
  /*
   * Get 'first_member' on head into 'endpoint_p'
   */
  res = ENDP_LST_ACCESS_DATA.first_member.get(unit, endp_lst_index, &endpoint_p) ;
  BCMDNX_IF_ERR_EXIT(res) ;
  while (endpoint_p != 0)
  {
    /*
     * Get 'next' of the member to delete into 'endpoint_p'
     * Note: 'endp_lst_m_index' is the index of the member to delete.
     */
    BCM_DPP_ENDP_LST_M_CONVERT_HANDLE_TO_ENDPLSTM_INDEX(endp_lst_m_index,endpoint_p) ;
    res = ENDP_LST_M_ACCESS_DATA.next.get(unit, endp_lst_m_index, &endpoint_p) ;
    BCMDNX_IF_ERR_EXIT(res) ;
    /*
     * Set member as 'not occupied' and free memory.
     */
    res = ENDP_LST_M_ACCESS.occupied_endp_lst_ms.bit_clear(unit, endp_lst_m_index);
    BCMDNX_IF_ERR_EXIT(res) ;
    /*
     * Free memory.
     */
    res = ENDP_LST_M_ACCESS.endp_lst_ms_array.free(unit, endp_lst_m_index);
    BCMDNX_IF_ERR_EXIT(res) ;
    /*
     * Decrement number of members 'in use'.
     */
    res = ENDP_LST_M_ACCESS.in_use.get(unit, &in_use) ;
    BCMDNX_IF_ERR_EXIT(res) ;
    if ((int)in_use <= 0)
    {
      /*
       * If number of occupied bitmap structures is to go below zero
       * then quit with error.
       */
      res = BCM_E_UNAVAIL ;
      BCMDNX_IF_ERR_EXIT(res) ;
    }
    res = ENDP_LST_M_ACCESS.in_use.set(unit, (in_use - 1)) ;
    BCMDNX_IF_ERR_EXIT(res) ;
  }
  /*
   * Set endpoint list as 'not occupied'.
   */
  res = ENDP_LST_ACCESS.occupied_endp_lsts.bit_clear(unit, endp_lst_index);
  BCMDNX_IF_ERR_EXIT(res) ;
  /*
   * Free endpoint list (head) itself
   */
  res = ENDP_LST_ACCESS_DATA.free(unit, endp_lst_index) ;
  BCMDNX_IF_ERR_EXIT(res) ;
  /*
   * Decrement number of endpoint lists 'in use'.
   */
  res = ENDP_LST_ACCESS.in_use.get(unit, &in_use) ;
  BCMDNX_IF_ERR_EXIT(res) ;
  if ((int)in_use <= 0)
  {
    /*
     * If number of occupied bitmap structures is to go below zero
     * then quit with error.
     */
    res = BCM_E_UNAVAIL ;
    BCMDNX_IF_ERR_EXIT(res) ;
  }
  res = ENDP_LST_ACCESS.in_use.set(unit, (in_use - 1)) ;
  BCMDNX_IF_ERR_EXIT(res) ;
exit:
  BCMDNX_FUNC_RETURN ;
}
/*
 * }
 */

