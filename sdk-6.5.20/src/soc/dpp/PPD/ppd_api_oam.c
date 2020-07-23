/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_OAM



#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_oam.h>
#include <soc/dpp/JER/JER_PP/jer_pp_oam.h>





#define _ARAD_PP_OAM_SIZE_OF_HOST_BUFFER_FOR_EVENT_FIFO_USED_BY_DMA 80
#define _ARAD_PP_OAM_NOF_INTERRUPT_MESSAGES_IN_FIFO 20



















uint32
  soc_ppd_oam_init(
    SOC_SAND_IN  int                                 unit,
	SOC_SAND_IN  uint8                                  is_bfd
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_INIT);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_init_unsafe(
          unit,
		  is_bfd
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_init()", 0, 0);
}


uint32
  soc_ppd_oam_deinit(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint8                    is_bfd,
    SOC_SAND_IN  uint8                    tcam_db_destroy
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_INIT);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_deinit_unsafe(
          unit,
	  is_bfd,
          tcam_db_destroy
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_deinit()", 0, 0);
}


uint32
  soc_ppd_oam_icc_map_register_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                       icc_ndx,
    SOC_SAND_IN  SOC_PPC_OAM_ICC_MAP_DATA     * data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_ICC_MAP_REGISTER_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_icc_map_register_set_verify(
          unit,
          icc_ndx,
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_icc_map_register_set_unsafe(
          unit,
          icc_ndx,
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_icc_map_register_set()", 0, 0);
}


uint32
  soc_ppd_oam_icc_map_register_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                       icc_ndx,
    SOC_SAND_OUT  SOC_PPC_OAM_ICC_MAP_DATA     * data
  )
{
  

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_ICC_MAP_REGISTER_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_icc_map_register_get()", 0, 0);
}


uint32
  soc_ppd_oam_classifier_oam1_entries_insert_default_profile(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_OAM_LIF_PROFILE_DATA       *profile_data,
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
    SOC_SAND_IN  uint8                            is_bfd
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_OAM1_ENTRIES_INSERT_DEFAULT_PROFILE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_classifier_oam1_entries_insert_default_profile_verify(
          unit,
          profile_data,
          classifier_mep_entry,
          is_bfd
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_classifier_oam1_entries_insert_default_profile_unsafe(
          unit,
          profile_data,
          classifier_mep_entry,
          is_bfd
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_classifier_oam1_entries_insert_default_profile()", 0, 0);
}


uint32
  soc_ppd_oam_classifier_oem_mep_profile_replace(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry,
    SOC_SAND_IN  uint32                           update_mp_type
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_OEM_MEP_PROFILE_REPLACE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_classifier_oem_mep_profile_replace_verify(
          unit,
          classifier_mep_entry,
		  update_mp_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_classifier_oem_mep_profile_replace_unsafe(
          unit,
          classifier_mep_entry,
		  update_mp_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_classifier_oem_mep_profile_replace()", 0, 0);
}


uint32
  soc_ppd_oam_classifier_oem_mep_add(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY * classifier_mep_entry,
    SOC_SAND_IN  uint8                    update,
    SOC_SAND_IN  uint8                    is_arm
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_OEM_MEP_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_classifier_oem_mep_add_verify(
          unit,
          mep_index,
          classifier_mep_entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_classifier_oem_mep_add_unsafe(
          unit,
          mep_index,
          classifier_mep_entry,
          update,
          is_arm
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_classifier_oem_mep_add()", 0, 0);
}


uint32
  soc_ppd_oam_classifier_mep_delete(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint32                   mep_index,
	SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY   *classifier_mep_entry
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_MEP_DELETE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_classifier_mep_delete_verify(
          unit,
    	  mep_index,
		  classifier_mep_entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_classifier_mep_delete_unsafe(
          unit,
          mep_index,
		  classifier_mep_entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_classifier_mep_delete()", 0, 0);
}


uint32
  soc_ppd_oam_classifier_oem1_entry_set(
    SOC_SAND_IN   int                                     unit,
	SOC_SAND_IN   SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY      *oem1_key,
	SOC_SAND_IN   SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD  *oem1_payload
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_OEM1_ENTRY_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_classifier_oem1_entry_set_unsafe(
          unit,
          oem1_key,
          oem1_payload
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_classifier_oem1_entry_set()", 0, 0);
}


uint32
  soc_ppd_oam_classifier_oem1_entry_get(
    SOC_SAND_IN   int                                     unit,
	SOC_SAND_IN   SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY      *oem1_key,
	SOC_SAND_OUT  SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD  *oem1_payload,
	SOC_SAND_OUT  uint8                                      *is_found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_OEM1_ENTRY_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_classifier_oem1_entry_get_unsafe(
          unit,
          oem1_key,
          oem1_payload,
    	  is_found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_classifier_oem1_entry_get()", 0, 0);
}


uint32
  soc_ppd_oam_classifier_oem1_entry_delete(
    SOC_SAND_IN   int                                     unit,
	SOC_SAND_IN   SOC_PPC_OAM_CLASSIFIER_OEM1_ENTRY_KEY      *oem1_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_OEM1_ENTRY_DELETE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_classifier_oem1_entry_delete_unsafe(
          unit,
          oem1_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_classifier_oem1_entry_delete()", 0, 0);
}


uint32
  soc_ppd_oam_classifier_oem2_entry_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_KEY       *oem2_key,
	SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD   *oem2_payload
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_OEM2_ENTRY_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_classifier_oem2_entry_set_unsafe(
          unit,
          oem2_key,
          oem2_payload
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_classifier_oem2_entry_set()", 0, 0);
}


uint32
  soc_ppd_oam_classifier_oem2_entry_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_KEY       *oem2_key,
	SOC_SAND_OUT  SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_PAYLOAD  *oem2_payload,
	SOC_SAND_OUT  uint8                                      *is_found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_OEM2_ENTRY_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_classifier_oem2_entry_get_unsafe(
          unit,
          oem2_key,
          oem2_payload,
    	  is_found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_classifier_oem2_entry_get()", 0, 0);
}


uint32
  soc_ppd_oam_classifier_oem2_entry_delete(
    SOC_SAND_IN   int                                     unit,
	SOC_SAND_IN   SOC_PPC_OAM_CLASSIFIER_OEM2_ENTRY_KEY      *oem2_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_OEM2_ENTRY_DELETE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_classifier_oem2_entry_delete_unsafe(
          unit,
          oem2_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_classifier_oem2_entry_delete()", 0, 0);
}



uint32
  soc_ppd_oam_oamp_rmep_set(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint32                   rmep_index,
	SOC_SAND_IN  uint16                   rmep_id,
	SOC_SAND_IN  uint32                   mep_index,
	SOC_SAND_IN  SOC_PPC_OAM_MEP_TYPE     mep_type,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_RMEP_DB_ENTRY  *rmep_db_entry,
	SOC_SAND_IN  uint8                    update
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_RMEP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_oamp_rmep_set_verify(
		unit,
		rmep_index,
		rmep_id,
		mep_index,
		mep_type,
		rmep_db_entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_oamp_rmep_set_unsafe(
		unit,
		rmep_index,
		rmep_id,
		mep_index,
		mep_type,
		rmep_db_entry,
		update
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_oamp_rmep_set()", 0, 0);
}


uint32
  soc_ppd_oam_oamp_rmep_get(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint32                   rmep_index,
    SOC_SAND_OUT  SOC_PPC_OAM_OAMP_RMEP_DB_ENTRY  *rmep_db_entry
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_RMEP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_oamp_rmep_get_verify(
          unit,
          rmep_index,
          rmep_db_entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_oamp_rmep_get_unsafe(
          unit,
          rmep_index,
          rmep_db_entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_oamp_rmep_get()", 0, 0);
}


uint32
  soc_ppd_oam_oamp_rmep_delete(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint32                   rmep_index,
	SOC_SAND_IN  uint16                   rmep_id,
	SOC_SAND_IN  uint32                   mep_index,
	SOC_SAND_IN  SOC_PPC_OAM_MEP_TYPE     mep_type
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_RMEP_DELETE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_oamp_rmep_delete_verify(
		 unit,
		 rmep_index,
		 rmep_id,
		 mep_index,
		 mep_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_oamp_rmep_delete_unsafe(
		 unit,
		 rmep_index,
		 rmep_id,
		 mep_index,
		 mep_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_oamp_rmep_delete()", 0, 0);
}


uint32
  soc_ppd_oam_oamp_rmep_index_get(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint16                   rmep_id,
	SOC_SAND_IN  uint32                   mep_index,
	SOC_SAND_IN  SOC_PPC_OAM_MEP_TYPE     mep_type,
	SOC_SAND_OUT uint32                   *rmep_index,
	SOC_SAND_OUT  uint8                   *is_found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_RMEP_INDEX_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_oamp_rmep_index_get_verify(
		 unit,
		 rmep_id,
		 mep_index,
		 mep_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_oamp_rmep_index_get_unsafe(
		 unit,
		 rmep_id,
		 mep_index,
		 mep_type,
		 rmep_index,
		 is_found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_oamp_rmep_index_get()", 0, 0);
}


uint32
  soc_ppd_oam_oamp_mep_db_entry_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_MEP_DB_ENTRY  *mep_db_entry,
    SOC_SAND_IN  uint8                    allocate_icc_ndx,
    SOC_SAND_IN  SOC_PPC_OAM_MA_NAME      name,
    SOC_SAND_IN  int                      is_new_ep
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_oamp_mep_db_entry_set_verify(
          unit,
          mep_index,
    	  mep_db_entry,
    	  allocate_icc_ndx,
    	  name
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_oamp_mep_db_entry_set_unsafe(
          unit,
          mep_index,
          mep_db_entry,
          allocate_icc_ndx,
          name,
          is_new_ep
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_oamp_mep_db_entry_set()", 0, 0);
}


uint32
  soc_ppd_oam_oamp_mep_db_entry_get(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_OUT  SOC_PPC_OAM_OAMP_MEP_DB_ENTRY  *mep_db_entry
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_oamp_mep_db_entry_get_verify(
          unit,
          mep_index,
    	  mep_db_entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_oamp_mep_db_entry_get_unsafe(
          unit,
          mep_index,
    	  mep_db_entry
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_oamp_mep_db_entry_get()", 0, 0);
}


uint32
  soc_ppd_oam_oamp_mep_db_entry_delete(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint32                   mep_index,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_MEP_DB_ENTRY *mep_db_entry,
	SOC_SAND_IN  uint8                    deallocate_icc_ndx,
	SOC_SAND_IN  uint8                    is_last_mep
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_MEP_DB_ENTRY_DELETE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_oamp_mep_db_entry_delete_verify(
          unit,
    	  mep_index,
		  deallocate_icc_ndx,
	     is_last_mep
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_oamp_mep_db_entry_delete_unsafe(
          unit,
          mep_index,
          mep_db_entry,
		  deallocate_icc_ndx,
	      is_last_mep
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_oamp_mep_db_entry_delete()", 0, 0);
}


uint32
  soc_ppd_oam_classifier_find_mep_and_profile_by_lif_and_mdlevel(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  uint32                   lif,
	SOC_SAND_IN  uint8                    md_level,
	SOC_SAND_IN  uint8                    is_upmep,
	SOC_SAND_OUT uint8                    *found_mep,
	SOC_SAND_OUT uint32                   *profile,
	SOC_SAND_OUT uint8                    *found_profile,
	SOC_SAND_OUT uint8                    *is_mp_type_flexible,
    SOC_SAND_OUT  uint8                    *is_mip
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_CLASSIFIER_FIND_MEP_BY_LIF_AND_MD_LEVEL);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_classifier_find_mep_and_profile_by_lif_and_mdlevel_verify(
          unit,
    	  lif,
		  md_level,
		  is_upmep
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_classifier_find_mep_and_profile_by_lif_and_mdlevel_unsafe(
          unit,
    	  lif,
		  md_level,
		  is_upmep,
	      found_mep,
		  profile,
		  found_profile,
		  is_mp_type_flexible,
          is_mip
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_classifier_find_mep_and_profile_by_lif_and_mdlevel()", 0, 0);
}


uint32
  soc_ppd_oam_counter_range_set(
    SOC_SAND_IN   int                                     unit,
	SOC_SAND_IN   uint32                                     counter_range_min,
	SOC_SAND_IN   uint32                                     counter_range_max
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_COUNTER_RANGE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_counter_range_set_verify(
          unit,
    	  counter_range_min,
		  counter_range_max
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_counter_range_set_unsafe(
          unit,
    	  counter_range_min,
		  counter_range_max
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_counter_range_set()", 0, 0);
}


uint32
  soc_ppd_oam_counter_range_get(
    SOC_SAND_IN   int                                     unit,
	SOC_SAND_OUT  uint32                                     *counter_range_min,
	SOC_SAND_OUT  uint32                                     *counter_range_max
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_COUNTER_RANGE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_counter_range_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_counter_range_get_unsafe(
          unit,
    	  counter_range_min,
		  counter_range_max
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_counter_range_get()", 0, 0);
}


uint32
  soc_ppd_oam_event_fifo_read(
    SOC_SAND_IN  int                                        unit,
    SOC_SAND_IN   SOC_PPC_OAM_DMA_EVENT_TYPE                 event_type,
	SOC_SAND_OUT  uint32                                       *rmeb_db_ndx,
	SOC_SAND_OUT  uint32                                       *event_id,
	SOC_SAND_OUT  uint32                                       *valid,
    SOC_SAND_OUT  uint32                                       *event_data,
    SOC_PPC_OAM_INTERRUPT_GLOBAL_DATA               *interrupt_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_EVENT_FIFO_READ);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_event_fifo_read_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_event_fifo_read_unsafe(
          unit,
          event_type,
    	  rmeb_db_ndx,
		  event_id,
          valid,
          event_data,
          interrupt_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_event_fifo_read()", 0, 0);
}


uint32
  soc_ppd_oam_pp_pct_profile_set(
    SOC_SAND_IN  int                                 unit,
	SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                           local_port_ndx,
    SOC_SAND_IN  uint8                                  oam_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_PP_PCT_PROFILE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_pp_pct_profile_set_verify(
          unit,
    	  local_port_ndx,
		  oam_profile
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_pp_pct_profile_set_unsafe(
          unit,
		  core_id,
    	  local_port_ndx,
		  oam_profile
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_pp_pct_profile_set()", local_port_ndx, 0);
}


uint32
  soc_ppd_oam_pp_pct_profile_get(
    SOC_SAND_IN  int                                 unit,
	SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PPC_PORT                           local_port_ndx,
    SOC_SAND_OUT uint8                                  *oam_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_PP_PCT_PROFILE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_pp_pct_profile_get_verify(
          unit,
    	  local_port_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_pp_pct_profile_get_unsafe(
          unit,
		  core_id,
    	  local_port_ndx,
		  oam_profile
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_pp_pct_profile_get()", 0, 0);
}


uint32
  soc_ppd_oam_eth_oam_opcode_map_set(
    SOC_SAND_IN   int                                     unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_eth_oam_opcode_map_set_unsafe(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_eth_oam_opcode_map_set()", 0, 0);
}



uint32
  soc_ppd_oam_bfd_ipv4_tos_ttl_select_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        ipv4_tos_ttl_select_index,
	SOC_SAND_IN  SOC_PPC_BFD_IP_MULTI_HOP_TOS_TTL_DATA        *tos_ttl_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_IPV4_TOS_TTL_SELECT_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_bfd_ipv4_tos_ttl_select_set_verify(
          unit,
          ipv4_tos_ttl_select_index,
    	  tos_ttl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_bfd_ipv4_tos_ttl_select_set_unsafe(
          unit,
          ipv4_tos_ttl_select_index,
    	  tos_ttl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_bfd_ipv4_tos_ttl_select_set()", ipv4_tos_ttl_select_index, 0);
}


uint32
  soc_ppd_oam_bfd_ipv4_tos_ttl_select_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        ipv4_tos_ttl_select_index,
	SOC_SAND_OUT SOC_PPC_BFD_IP_MULTI_HOP_TOS_TTL_DATA        *tos_ttl_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_IPV4_TOS_TTL_SELECT_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_bfd_ipv4_tos_ttl_select_get_verify(
          unit,
          ipv4_tos_ttl_select_index
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_bfd_ipv4_tos_ttl_select_get_unsafe(
          unit,
          ipv4_tos_ttl_select_index,
    	  tos_ttl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_bfd_ipv4_tos_ttl_select_get()", ipv4_tos_ttl_select_index, 0);
}


uint32
  soc_ppd_oam_bfd_ipv4_src_addr_select_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        ipv4_src_addr_select_index,
	SOC_SAND_IN  uint32                                       src_addr
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_IPV4_SRC_ADDR_SELECT_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_bfd_ipv4_src_addr_select_set_verify(
          unit,
          ipv4_src_addr_select_index,
    	  src_addr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_bfd_ipv4_src_addr_select_set_unsafe(
          unit,
          ipv4_src_addr_select_index,
    	  src_addr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_bfd_ipv4_src_addr_select_set()", ipv4_src_addr_select_index, 0);
}


uint32
  soc_ppd_oam_bfd_ipv4_src_addr_select_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        ipv4_src_addr_select_index,
	SOC_SAND_OUT uint32                                       *src_addr
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_IPV4_SRC_ADDR_SELECT_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_bfd_ipv4_src_addr_select_get_verify(
          unit,
          ipv4_src_addr_select_index
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_bfd_ipv4_src_addr_select_get_unsafe(
          unit,
          ipv4_src_addr_select_index,
    	  src_addr
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_bfd_ipv4_src_addr_select_get()", ipv4_src_addr_select_index, 0);
}



uint32
  soc_ppd_oam_bfd_tx_rate_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        bfd_tx_rate_index,
	SOC_SAND_IN  uint32                                       tx_rate
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_TX_RATE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_bfd_tx_rate_set_verify(
          unit,
          bfd_tx_rate_index,
    	  tx_rate
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_bfd_tx_rate_set_unsafe(
          unit,
          bfd_tx_rate_index,
    	  tx_rate
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_bfd_tx_rate_set()", bfd_tx_rate_index, 0);
}


uint32
  soc_ppd_oam_bfd_tx_rate_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        bfd_tx_rate_index,
	SOC_SAND_OUT uint32                                       *tx_rate
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_TX_RATE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_bfd_tx_rate_get_verify(
          unit,
          bfd_tx_rate_index
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_bfd_tx_rate_get_unsafe(
          unit,
          bfd_tx_rate_index,
    	  tx_rate
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_bfd_tx_rate_get()", bfd_tx_rate_index, 0);
}



uint32
  soc_ppd_oam_bfd_req_interval_pointer_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        req_interval_pointer,
	SOC_SAND_IN  uint32                                       req_interval
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_REQ_INTERVAL_POINTER_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_bfd_req_interval_pointer_set_verify(
          unit,
          req_interval_pointer,
    	  req_interval
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_bfd_req_interval_pointer_set_unsafe(
          unit,
          req_interval_pointer,
    	  req_interval
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_bfd_req_interval_pointer_set()", req_interval_pointer, 0);
}


uint32
  soc_ppd_oam_bfd_req_interval_pointer_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        req_interval_pointer,
	SOC_SAND_OUT uint32                                       *req_interval
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_REQ_INTERVAL_POINTER_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_bfd_req_interval_pointer_get_verify(
          unit,
          req_interval_pointer
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_bfd_req_interval_pointer_get_unsafe(
          unit,
          req_interval_pointer,
    	  req_interval
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_bfd_req_interval_pointer_get()", req_interval_pointer, 0);
}


uint32
  soc_ppd_oam_mpls_pwe_profile_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        push_profile,
	SOC_SAND_IN  SOC_PPC_MPLS_PWE_PROFILE_DATA            *push_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_MPLS_PWE_PROFILE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_mpls_pwe_profile_set_verify(
          unit,
          push_profile,
    	  push_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_mpls_pwe_profile_set_unsafe(
          unit,
          push_profile,
    	  push_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_mpls_pwe_profile_set()", push_profile, 0);
}



uint32
  soc_ppd_oam_mpls_pwe_profile_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        push_profile,
	SOC_SAND_OUT SOC_PPC_MPLS_PWE_PROFILE_DATA            *push_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_MPLS_PWE_PROFILE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_mpls_pwe_profile_get_verify(
          unit,
          push_profile
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_mpls_pwe_profile_get_unsafe(
          unit,
          push_profile,
    	  push_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_mpls_pwe_profile_get()", push_profile, 0);
}



uint32
  soc_ppd_oam_bfd_mpls_udp_sport_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint16            							  udp_sport
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_MPLS_UDP_SPORT_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_bfd_mpls_udp_sport_set_verify(
          unit,
          udp_sport
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_bfd_mpls_udp_sport_set_unsafe(
          unit,
          udp_sport
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_bfd_mpls_udp_sport_set()", 0, 0);
}


uint32
  soc_ppd_oam_bfd_mpls_udp_sport_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_OUT uint16                                       *udp_sport
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_MPLS_UDP_SPORT_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_bfd_mpls_udp_sport_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_bfd_mpls_udp_sport_get_unsafe(
          unit,
          udp_sport
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_bfd_mpls_udp_sport_get()", 0, 0);
}



uint32
  soc_ppd_oam_bfd_ipv4_udp_sport_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint16            							  udp_sport
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_IPV4_UDP_SPORT_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_bfd_ipv4_udp_sport_set_verify(
          unit,
          udp_sport
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_bfd_ipv4_udp_sport_set_unsafe(
          unit,
          udp_sport
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_bfd_ipv4_udp_sport_set()", 0, 0);
}



uint32
  soc_ppd_oam_bfd_ipv4_udp_sport_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_OUT  uint16                                      *udp_sport
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_IPV4_UDP_SPORT_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_bfd_ipv4_udp_sport_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_bfd_ipv4_udp_sport_get_unsafe(
          unit,
          udp_sport
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_bfd_ipv4_udp_sport_get()", 0, 0);
}


uint32
  soc_ppd_oam_bfd_pdu_static_register_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  SOC_PPC_BFD_PDU_STATIC_REGISTER              *bfd_pdu
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_bfd_pdu_static_register_set_verify(
          unit,
          bfd_pdu
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_bfd_pdu_static_register_set_unsafe(
          unit,
          bfd_pdu
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_bfd_pdu_static_register_set()", 0, 0);
}


uint32
  soc_ppd_oam_bfd_pdu_static_register_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_OUT SOC_PPC_BFD_PDU_STATIC_REGISTER              *bfd_pdu
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_PDU_STATIC_REGISTER_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_bfd_pdu_static_register_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_bfd_pdu_static_register_get_unsafe(
          unit,
          bfd_pdu
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_bfd_pdu_static_register_get()", 0, 0);
}


uint32
  soc_ppd_oam_bfd_cc_packet_static_register_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  SOC_PPC_BFD_CC_PACKET_STATIC_REGISTER              *bfd_cc_packet
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_bfd_cc_packet_static_register_set_verify(
          unit,
          bfd_cc_packet
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_bfd_cc_packet_static_register_set_unsafe(
          unit,
          bfd_cc_packet
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_bfd_cc_packet_static_register_set()", 0, 0);
}



uint32
  soc_ppd_oam_bfd_cc_packet_static_register_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_OUT SOC_PPC_BFD_CC_PACKET_STATIC_REGISTER              *bfd_cc_packet
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_bfd_cc_packet_static_register_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_bfd_cc_packet_static_register_get_unsafe(
          unit,
          bfd_cc_packet
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_bfd_cc_packet_static_register_get()", 0, 0);
}


uint32
  soc_ppd_oam_bfd_discriminator_range_registers_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint32  						              range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_DISCRIMINATOR_RANGE_REGISTERS_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_bfd_discriminator_range_registers_set_verify(
          unit,
          range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_bfd_discriminator_range_registers_set_unsafe(
          unit,
          range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_bfd_discriminator_range_registers_set()", 0, 0);
}

uint32
  soc_ppd_oam_bfd_discriminator_range_registers_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_OUT  uint32  						              *range
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_DISCRIMINATOR_RANGE_REGISTERS_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_bfd_discriminator_range_registers_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_bfd_discriminator_range_registers_get_unsafe(
          unit,
          range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_bfd_discriminator_range_registers_get()", 0, 0);
}



uint32
  soc_ppd_oam_bfd_my_bfd_dip_ip_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint16                                     dip_index,
	SOC_SAND_IN  SOC_SAND_PP_IPV6_ADDRESS					*dip							
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_MY_BFD_DIP_IPV4_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_bfd_my_bfd_dip_ip_set_verify(
          unit,
          dip_index,
		  dip							
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_bfd_my_bfd_dip_ip_set_unsafe(
          unit,
          dip_index,
		  dip
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_my_bfd_dip_ipv4_set()", 0, 0);
}

uint32
  soc_ppd_oam_bfd_my_bfd_dip_ip_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint16                                     dip_index,
	SOC_SAND_OUT  SOC_SAND_PP_IPV6_ADDRESS  				*dip							
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_MY_BFD_DIP_IPV4_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_bfd_my_bfd_dip_ip_get_verify(
          unit,
          dip_index							
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_bfd_my_bfd_dip_ip_get_unsafe(
          unit,
          dip_index,
		  dip
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_my_bfd_dip_ipv4_get()", 0, 0);
}


uint32
  soc_ppd_oam_bfd_tx_ipv4_multi_hop_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES              *tx_ipv4_multi_hop_att
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_TX_IPV4_MULTI_HOP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_bfd_tx_ipv4_multi_hop_set_verify(
          unit,
          tx_ipv4_multi_hop_att							
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_bfd_tx_ipv4_multi_hop_set_unsafe(
          unit,
          tx_ipv4_multi_hop_att	
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_bfd_tx_ipv4_multi_hop_set()", 0, 0);
}

uint32
  soc_ppd_oam_bfd_tx_ipv4_multi_hop_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_OUT  SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES              *tx_ipv4_multi_hop_att
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_TX_IPV4_MULTI_HOP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_bfd_tx_ipv4_multi_hop_get_verify(
          unit							
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_bfd_tx_ipv4_multi_hop_get_unsafe(
          unit,
          tx_ipv4_multi_hop_att	
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_bfd_tx_ipv4_multi_hop_get()", 0, 0);
}


uint32
  arad_pp_oam_bfd_tx_mpls_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES              *tx_mpls_att
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_BFD_TX_MPLS_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_bfd_tx_ipv4_multi_hop_set_verify(
          unit,
          tx_mpls_att							
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_bfd_tx_ipv4_multi_hop_set_unsafe(
          unit,
          tx_mpls_att	
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_oam_bfd_tx_mpls_set()", 0, 0);
}




uint32
  soc_ppd_oam_oamp_tx_priority_registers_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint32                     	              priority,
	SOC_SAND_IN  SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES              *tx_oam_att
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_TX_PRIORITY_REGISTERS_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_oamp_tx_priority_registers_set_verify(
          unit,
		  priority,
		  tx_oam_att					
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_oamp_tx_priority_registers_set_unsafe(
          unit,
          priority,
		  tx_oam_att
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_oamp_tx_priority_registers_set()", 0, 0);
}

uint32
  soc_ppd_oam_oamp_tx_priority_registers_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint32                     	              priority,
	SOC_SAND_OUT  SOC_PPC_OAMP_TX_ITMH_ATTRIBUTES             *tx_oam_att
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_TX_PRIORITY_REGISTERS_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_oamp_tx_priority_registers_get_verify(
          unit,
		  priority						
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_oamp_tx_priority_registers_get_unsafe(
          unit,
          priority,
		  tx_oam_att
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_oamp_tx_priority_registers_get()", 0, 0);
}

uint32
  soc_ppd_oam_oamp_enable_interrupt_message_event_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        *interrupt_message_event_bmp
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_ENABLE_INTERRUPT_MESSAGE_EVENT_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_oamp_enable_interrupt_message_event_set_verify(
          unit,
		  interrupt_message_event_bmp							
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_oamp_enable_interrupt_message_event_set_unsafe(
          unit,
          interrupt_message_event_bmp	
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_oamp_enable_interrupt_message_event_set()", 0, 0);
}

uint32
  soc_ppd_oam_oamp_enable_interrupt_message_event_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_OUT  uint8                                        *interrupt_message_event_bmp
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_OAM_OAMP_ENABLE_INTERRUPT_MESSAGE_EVENT_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_oamp_enable_interrupt_message_event_get_verify(
          unit							
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_oamp_enable_interrupt_message_event_get_unsafe(
          unit,
          interrupt_message_event_bmp	
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_oamp_enable_interrupt_message_event_get()", 0, 0);
}


uint32
  soc_ppd_oam_bfd_diag_profile_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        profile_ndx,
	SOC_SAND_IN  uint32                                       diag_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_bfd_diag_profile_set_verify(
          unit,
		  profile_ndx,
		  diag_profile						
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_bfd_diag_profile_set_unsafe(
          unit,
		  profile_ndx,
		  diag_profile	
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_bfd_diag_profile_set()", 0, 0);
}

uint32
  soc_ppd_oam_bfd_diag_profile_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        profile_ndx,
	SOC_SAND_OUT  uint32                                       *diag_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_bfd_diag_profile_get_verify(
          unit,
		  profile_ndx					
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_bfd_diag_profile_get_unsafe(
          unit,
		  profile_ndx,
		  diag_profile	
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_bfd_diag_profile_get()", 0, 0);
}


uint32
  soc_ppd_oam_bfd_flags_profile_set(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        profile_ndx,
	SOC_SAND_IN  uint32                                       flags_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_bfd_flags_profile_set_verify(
          unit,
		  profile_ndx,
		  flags_profile						
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_bfd_flags_profile_set_unsafe(
          unit,
		  profile_ndx,
		  flags_profile	
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_bfd_flags_profile_set()", 0, 0);
}

uint32
  soc_ppd_oam_bfd_flags_profile_get(
    SOC_SAND_IN  int                                       unit,
	SOC_SAND_IN  uint8                                        profile_ndx,
	SOC_SAND_OUT  uint32                                       *flags_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_bfd_flags_profile_get_verify(
          unit,
		  profile_ndx					
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_bfd_flags_profile_get_unsafe(
          unit,
		  profile_ndx,
		  flags_profile	
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_bfd_flags_profile_get()", 0, 0);
}



uint32
  soc_ppd_oam_mep_passive_active_enable_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_IN  uint8                                  enable
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_mep_passive_active_enable_set_verify(
          unit,
		  profile_ndx,
		  enable							
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_mep_passive_active_enable_set_unsafe(
          unit,
		  profile_ndx,
		  enable	
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_mep_passive_active_enable_set()", 0, 0);
}

uint32
  soc_ppd_oam_mep_passive_active_enable_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 profile_ndx,
    SOC_SAND_OUT  uint8                                 *enable
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_mep_passive_active_enable_get_verify(
          unit,
		  profile_ndx						
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_mep_passive_active_enable_get_unsafe(
          unit,
		  profile_ndx,
		  enable	
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_mep_passive_active_enable_get()", 0, 0);
}


uint32
  soc_ppd_oam_oamp_error_trap_id_and_destination_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_TRAP_TYPE             trap_type,
    SOC_SAND_IN  uint32                                 trap_id,
    SOC_SAND_IN  SOC_TMC_DEST_INFO                         dest_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_oamp_error_trap_id_and_destination_set_verify(
          unit,
		  trap_type,
          trap_id,
          dest_info			
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_oamp_error_trap_id_and_destination_set_unsafe(
          unit,
		  trap_type,
          trap_id,
          dest_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_oamp_error_trap_id_and_destination_set()", 0, 0);
}

uint32
  soc_ppd_oam_oamp_error_trap_id_and_destination_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_TRAP_TYPE             trap_type,
    SOC_SAND_OUT  uint32                                *trap_id,
    SOC_SAND_OUT  SOC_TMC_DEST_INFO                        *dest_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_oamp_error_trap_id_and_destination_get_verify(
          unit,
		  trap_type			
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_oamp_error_trap_id_and_destination_get_unsafe(
          unit,
		  trap_type,
          trap_id,
          dest_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_oamp_error_trap_id_and_destination_get()", 0, 0);
}

uint32
  soc_ppd_oam_oamp_lm_dm_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_LM_DM_MEP_DB_ENTRY     *mep_db_entry
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_oamp_lm_dm_set_verify(unit, mep_db_entry );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_oamp_lm_dm_set_unsafe(unit, mep_db_entry);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}
uint32
  soc_ppd_oam_oamp_next_index_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_OUT uint32                               *next_index,
    SOC_SAND_OUT    uint8                              *has_dm
  )
{
      uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_oamp_next_index_get_verify(unit, endpoint_id, next_index, has_dm);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;
  
  res = arad_pp_oam_oamp_next_index_get_unsafe(unit, endpoint_id, next_index, has_dm);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}



uint32
  soc_ppd_oam_oamp_eth1731_and_oui_profiles_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_OUT uint32                               *eth1731_prof,
    SOC_SAND_OUT uint32                               *da_oui_prof
  )
{
    uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    res = arad_pp_oam_oamp_eth1731_and_oui_profiles_get_verify(unit, endpoint_id,eth1731_prof,da_oui_prof);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = arad_pp_oam_oamp_eth1731_and_oui_profiles_get_unsafe(unit, endpoint_id,eth1731_prof,da_oui_prof);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

  exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
    ARAD_PP_DO_NOTHING_AND_EXIT;
  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}



uint32
    soc_ppd_oam_oamp_nic_profile_get(
       SOC_SAND_IN  int                                 unit,
       SOC_SAND_IN  uint32                                 endpoint_id,
       SOC_SAND_OUT uint32                               *da_nic_prof
       )
{
    uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    res = arad_pp_oam_oamp_nic_profile_get_verify(unit, endpoint_id,da_nic_prof);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = arad_pp_oam_oamp_nic_profile_get_unsafe(unit, endpoint_id,da_nic_prof);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

  exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
    ARAD_PP_DO_NOTHING_AND_EXIT;
  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}



uint32
  soc_ppd_oam_oamp_create_new_eth1731_profile(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_IN  uint8                          was_previously_alloced,
    SOC_SAND_IN  uint8                          profile_indx,
    SOC_SAND_IN SOC_PPC_OAM_ETH1731_MEP_PROFILE_ENTRY     *eth1731_profile
  )
{
    uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    res = arad_pp_oam_oamp_create_new_eth1731_profile_verify(unit, endpoint_id,was_previously_alloced,profile_indx,eth1731_profile);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = arad_pp_oam_oamp_create_new_eth1731_profile_unsafe(unit, endpoint_id,was_previously_alloced,profile_indx,eth1731_profile);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

  exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
    ARAD_PP_DO_NOTHING_AND_EXIT;
  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


uint32
  soc_ppd_oam_oamp_set_oui_nic_registers(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 endpoint_id,
    SOC_SAND_IN  uint32                                 msb_to_oui,
    SOC_SAND_IN  uint32                                 lsb_to_nic,
    SOC_SAND_IN  uint8                          profile_indx_oui,
    SOC_SAND_IN  uint8                          profile_indx_nic
  )
{
    uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    res = arad_pp_oam_oamp_set_oui_nic_registers_verify(unit, endpoint_id, msb_to_oui,lsb_to_nic,profile_indx_oui, profile_indx_nic);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = arad_pp_oam_oamp_set_oui_nic_registers_unsafe(unit, endpoint_id, msb_to_oui,lsb_to_nic,profile_indx_oui, profile_indx_nic);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

  exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
    ARAD_PP_DO_NOTHING_AND_EXIT;
  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


uint32
  soc_ppd_oam_oamp_lm_dm_remove(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN uint32                                        endpoint_id,
    SOC_SAND_IN  uint8                          is_lm,
    SOC_SAND_IN  SOC_PPC_OAM_OAMP_LM_DM_MEP_DB_ENTRY *mep_db_entry,
    SOC_SAND_IN uint8                           exists_piggy_back_down,
    SOC_SAND_OUT uint8                               * num_removed,
    SOC_SAND_OUT uint32                              * removed_index
  )
{
    uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    res = arad_pp_oam_oamp_lm_dm_remove_verify(unit, endpoint_id,is_lm,exists_piggy_back_down, num_removed , removed_index);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = arad_pp_oam_oamp_lm_dm_remove_unsafe(unit, endpoint_id,is_lm,mep_db_entry, exists_piggy_back_down,num_removed , removed_index);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

  exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
    ARAD_PP_DO_NOTHING_AND_EXIT;
  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));

}


uint32
  soc_ppd_oam_classifier_counter_disable_map_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                                  packet_is_oam,
    SOC_SAND_IN  uint8                                  profile,
    SOC_SAND_IN  uint8                                  counter_enable
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_classifier_counter_disable_map_set_verify(
          unit,
		  packet_is_oam,
          profile,
          counter_enable			
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_classifier_counter_disable_map_set_unsafe(
          unit,
		  packet_is_oam,
          profile,
          counter_enable
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_classifier_counter_disable_map_set()", 0, 0);
}

uint32
  soc_ppd_oam_classifier_counter_disable_map_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                                  packet_is_oam,
    SOC_SAND_IN  uint8                                  profile,
    SOC_SAND_OUT  uint8                                 *counter_enable
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_oam_classifier_counter_disable_map_get_verify(
          unit,
		  packet_is_oam,
          profile			
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_oam_classifier_counter_disable_map_get_unsafe(
          unit,
		  packet_is_oam,
          profile,
          counter_enable
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_oam_classifier_counter_disable_map_get()", 0, 0);
}

uint32 
    soc_ppd_oam_oamp_loopback_remove(
       SOC_SAND_IN  int                                                  unit
       )
{
    uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    res = arad_pp_oam_oamp_loopback_remove_verify(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = arad_pp_oam_oamp_loopback_remove_unsafe(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

  exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
    ARAD_PP_DO_NOTHING_AND_EXIT;
  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}




uint32 
     soc_ppd_oam_dma_reset(
       SOC_SAND_IN  int                                                  unit
       )
{
    uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    res = arad_pp_oam_dma_reset_verify(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = arad_pp_oam_dma_reset_unsafe(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

  exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
    ARAD_PP_DO_NOTHING_AND_EXIT;
  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


uint32 
     soc_ppd_oam_dma_clear(
       SOC_SAND_IN  int                                                  unit
       )
{
    uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    res = arad_pp_oam_dma_clear_verify(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = arad_pp_oam_dma_clear_unsafe(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

  exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
    ARAD_PP_DO_NOTHING_AND_EXIT;
  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


uint32 
   soc_ppd_oam_register_dma_event_handler_callback(
       SOC_SAND_IN  int                                                  unit,
       SOC_SAND_INOUT        dma_event_handler_cb_t          event_handler_cb
       )
{
    uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    res = arad_pp_oam_register_dma_event_handler_callback_verify(unit,event_handler_cb );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = arad_pp_oam_register_dma_event_handler_callback_unsafe(unit,event_handler_cb );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

  exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
    ARAD_PP_DO_NOTHING_AND_EXIT;
  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


void 
   soc_ppd_oam_dma_event_handler(
      SOC_SAND_INOUT  void        *     unit_ptr,
      SOC_SAND_INOUT  void        *     event_type_ptr,
      SOC_SAND_INOUT  void        *     cmc_ptr,
      SOC_SAND_INOUT  void        *     ch_ptr,
      SOC_SAND_INOUT  void        *    unused4
       )
{
    int unit = BSL_UNIT_UNKNOWN;
    int cmc, ch;
    SOC_PPC_OAM_DMA_EVENT_TYPE event_type;
    uint32
      res = SOC_SAND_OK;


    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    unit = PTR_TO_INT(unit_ptr);
    cmc = PTR_TO_INT(cmc_ptr);
    ch = PTR_TO_INT(ch_ptr);
    event_type = (SOC_PPC_OAM_DMA_EVENT_TYPE)
                    (INT_TO_PTR(event_type_ptr));

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;


    res = arad_pp_oam_dma_event_handler_verify(unit, event_type);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    

    res = arad_pp_oam_dma_event_handler_unsafe(unit, event_type, cmc, ch);
    SOC_SAND_SOC_IF_ERROR_RETURN(res,2322,exit,res);


  exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}






uint32
  soc_ppd_oam_diag_print_lookup(
     SOC_SAND_IN int unit
   )
{
    uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    res = arad_pp_oam_diag_print_lookup_verify(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res,10,exit);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;
    res = arad_pp_oam_diag_print_lookup_unsafe(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res,10,exit_semaphore);


exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("Error in soc_ppd_oam_diag_print_lookup()",0,0);
}

uint32
    soc_ppd_oam_get_crps_counter(
      SOC_SAND_IN int unit, 
      SOC_SAND_IN uint8 crps_counter_number, 
      SOC_SAND_IN uint32 reg_number, 
      SOC_SAND_OUT uint32* value
      )
{
    uint32 res = SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    res = arad_pp_get_crps_counter_verify(unit, crps_counter_number,reg_number, value);
    SOC_SAND_CHECK_FUNC_RESULT(res,10,exit);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;
    res = arad_pp_get_crps_counter_unsafe(unit, crps_counter_number,reg_number, value);
    SOC_SAND_CHECK_FUNC_RESULT(res,10,exit_semaphore);


exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("Error in soc_ppd_get_crps_counter()",0,0);
}


uint32
  soc_ppd_oam_diag_print_rx(
     SOC_SAND_IN int unit,
     SOC_SAND_IN int core_id

   )
{
    uint32 res= SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = arad_pp_oam_diag_print_rx_verify(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res,10,exit);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = arad_pp_oam_diag_print_rx_unsafe(unit, core_id);
    SOC_SAND_CHECK_FUNC_RESULT(res,10,exit_semaphore);


exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


uint32
  soc_ppd_oam_diag_print_em(
     SOC_SAND_IN int unit,
     SOC_SAND_IN int lif
   )
{
    uint32 res= SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = arad_pp_oam_diag_print_em_verify(unit, lif);
    SOC_SAND_CHECK_FUNC_RESULT(res,10,exit);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = arad_pp_oam_diag_print_em_unsafe(unit, lif);
    SOC_SAND_CHECK_FUNC_RESULT(res,10,exit_semaphore);


exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


uint32
  soc_ppd_oam_diag_print_ak(
     SOC_SAND_IN int unit,
     SOC_SAND_IN SOC_PPC_OAM_ACTION_KEY_PARAMS *key_params
   ) {
    uint32 res= SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = arad_pp_oam_diag_print_ak_verify(unit, key_params);
    SOC_SAND_CHECK_FUNC_RESULT(res,10,exit);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = arad_pp_oam_diag_print_ak_unsafe(unit, key_params);
    SOC_SAND_CHECK_FUNC_RESULT(res,10,exit_semaphore);


exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

uint32
  soc_ppd_oam_diag_print_ks(
     SOC_SAND_IN int unit,
     SOC_SAND_IN SOC_PPC_OAM_KEY_SELECT_PARAMS *key_params
   ) {
    uint32 res= SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = arad_pp_oam_diag_print_ks_unsafe(unit, key_params);
    SOC_SAND_CHECK_FUNC_RESULT(res,10,exit_semaphore);


exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

uint32
  soc_ppd_oam_diag_print_oamp_counter(
     SOC_SAND_IN int unit
   )
{
    uint32 res= SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = arad_pp_oam_diag_print_oamp_counter_verify(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res,10,exit);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = arad_pp_oam_diag_print_oamp_counter_unsafe(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res,20,exit_semaphore);


exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}


uint32
  soc_ppd_oam_diag_print_debug(
     SOC_SAND_IN int unit,
     SOC_SAND_IN int cfg,
     SOC_SAND_IN int mode
   )
{
    uint32 res= SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (cfg) {
        res = arad_pp_oam_diag_print_debug_verify(unit, mode);
        SOC_SAND_CHECK_FUNC_RESULT(res,10,exit);
    }

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = arad_pp_oam_diag_print_debug_unsafe(unit, cfg, mode);
    SOC_SAND_CHECK_FUNC_RESULT(res,20,exit_semaphore);


exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

uint32
  soc_ppd_oam_diag_print_oam_id(
     SOC_SAND_IN int unit,
     SOC_SAND_IN int core_id
   )
{
    uint32 res= SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = arad_pp_oam_diag_print_oam_id_verify(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res,10,exit);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = arad_pp_oam_diag_print_oam_id_unsafe(unit, core_id);
    SOC_SAND_CHECK_FUNC_RESULT(res,20,exit_semaphore);


exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

uint32
  soc_ppd_oam_diag_print_tcam_entries(
     SOC_SAND_IN int unit,
     SOC_SAND_IN int core_id
   )
{
    uint32 res= SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = arad_pp_oam_diag_print_tcam_entries_verify(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res,10,exit);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = arad_pp_oam_diag_print_tcam_entries_unsafe(unit, core_id);
    SOC_SAND_CHECK_FUNC_RESULT(res,10,exit_semaphore);


exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong")));
}

uint32
  soc_ppd_oam_diag_print_hw_tcam_entries(
     SOC_SAND_IN int unit
   )
{
    uint32 res= SOC_SAND_OK;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = arad_pp_oam_diag_print_hw_tcam_entries_verify(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res,10,exit);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = arad_pp_oam_diag_print_hw_tcam_entries_unsafe(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res,10,exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR_SOCDNX((_BSL_SOCDNX_SAND_MSG("Something went wrong in dump HW TCAM")));
}
#include <soc/dpp/SAND/Utils/sand_footer.h>

