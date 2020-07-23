/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 $Id$
 */

#ifndef BCM_IMACSEC_H
#define BCM_IMACSEC_H
#include<soc/error.h>
#include <soc/phy/phyctrl.h>
#include "bcm_plp_base_t_secy_api.h"
#include "bcm_plp_base_t_sec_common.h"

#define IMACSEC_PLP_BASE_T_SEC_ACCESS_GET(pc,pa,dev_id) { \
                        pa = (bcm_plp_base_t_sec_access_t *)(((char *)pc->driver_data + pc->size) - sizeof(bcm_plp_base_t_sec_access_t));\
                        if(pa == NULL){\
                           return SOC_E_INTERNAL;\
                        }\
                        pa->phy_info.phy_addr= pa->macsec_dev_addr;\
                        pa->macsec_side = dev_id;\
                        }
int imacsec_plp_base_t_secy_init(int unit, soc_port_t port,
                               unsigned int macsec_side,
                               bcm_plp_base_t_sec_mutex_t *mutex,
                               bcm_plp_base_t_secy_settings_t *setting_p);
int imacsec_plp_base_t_secy_vport_add(int unit, soc_port_t port, unsigned int macsec_side,
                                    bcm_plp_base_t_secy_vport_handle_t *vport_handle_p);
int imacsec_plp_base_t_secy_vport_remove(int unit, soc_port_t port, unsigned int macsec_side,
                                      const bcm_plp_base_t_secy_vport_handle_t vport_handle);

int imacsec_plp_base_t_secy_uninit(int unit, soc_port_t port, unsigned int macsec_side);

int imacsec_plp_base_t_secy_sa_add(int unit, soc_port_t port, unsigned int macsec_side,
                                 const bcm_plp_base_t_secy_vport_handle_t vport_handle,
                                 bcm_plp_base_t_secy_sa_handle_t *sa_handle_p,
                                 const bcm_plp_base_t_secy_sa_t *sa_p);
int imacsec_plp_base_t_secy_sa_update(int unit, soc_port_t port, unsigned int macsec_side,
                                    const bcm_plp_base_t_secy_sa_handle_t sa_handle,
                                    const bcm_plp_base_t_secy_sa_t *sa_p);
int imacsec_plp_base_t_secy_sa_read(int unit, soc_port_t port, unsigned int macsec_side,
                                  const bcm_plp_base_t_secy_sa_handle_t sa_handle,
                                  const unsigned int word_offset,
                                  const unsigned int word_count,
                                  unsigned int * transform_p);

int imacsec_plp_base_t_secy_sa_remove(int unit, soc_port_t port, unsigned int macsec_side,
                                    const bcm_plp_base_t_secy_sa_handle_t sa_handle
                                    );
int imacsec_plp_base_t_secy_rule_add(int unit, soc_port_t port, unsigned int macsec_side,
                                   const bcm_plp_base_t_secy_vport_handle_t vport_handle,
                                   bcm_plp_base_t_secy_rule_handle_t *rule_handle_p,
                                   const bcm_plp_base_t_secy_rule_t *rule_p);

int imacsec_plp_base_t_secy_rule_remove(int unit, soc_port_t port, unsigned int macsec_side,
                                      const bcm_plp_base_t_secy_rule_handle_t rule_handle);

int imacsec_plp_base_t_secy_rule_update(int unit, soc_port_t port, unsigned int macsec_side,
                                      const bcm_plp_base_t_secy_rule_handle_t rule_handle,
                                      const bcm_plp_base_t_secy_rule_t *rule_p);

int imacsec_plp_base_t_secy_rule_enable(int unit, soc_port_t port, unsigned int macsec_side,
                                      const bcm_plp_base_t_secy_rule_handle_t rule_handle,
                                      const unsigned char fsync);

int imacsec_plp_base_t_secy_rule_disable(int unit, soc_port_t port, unsigned int macsec_side,
                                      const bcm_plp_base_t_secy_rule_handle_t rule_handle,
                                      const unsigned char fsync);
int imacsec_plp_base_t_secy_rule_enable_disable(int unit, soc_port_t port, unsigned int macsec_side,
                                              const bcm_plp_base_t_secy_rule_handle_t rule_handle_disable,
                                              const bcm_plp_base_t_secy_rule_handle_t rule_handle_enable,
                                              const unsigned char enable_all,
                                              const unsigned char disable_all,
                                              const unsigned char fsync);
int imacsec_plp_base_t_secy_sa_chain(int unit, soc_port_t port, unsigned int macsec_side,
                                               const bcm_plp_base_t_secy_sa_handle_t active_sa_handle,
                                               bcm_plp_base_t_secy_sa_handle_t * const new_sa_handle_p,
                                               const bcm_plp_base_t_secy_sa_t * const new_sa_p);
int imacsec_plp_base_t_secy_sa_switch(int unit, soc_port_t port, unsigned int macsec_side,
                                                const bcm_plp_base_t_secy_sa_handle_t active_sa_handle,
                                                const bcm_plp_base_t_secy_sa_handle_t new_sa_handle,
                                                const bcm_plp_base_t_secy_sa_t * const new_sa_p);
int imacsec_plp_base_t_secy_device_limits(int unit, soc_port_t port, unsigned int macsec_side,
                                                    unsigned int * const max_vport_count_p,
                                                    unsigned int * const max_rule_count_p,
                                                    unsigned int * const max_sa_count_p,
                                                    unsigned int * const max_sc_count_p);
int imacsec_plp_base_t_secy_vport_handle_issame(int unit, soc_port_t port, unsigned int macsec_side,
                                          const bcm_plp_base_t_secy_vport_handle_t * const handle1_p,
                                          const bcm_plp_base_t_secy_vport_handle_t * const handle2_p);
int imacsec_plp_base_t_secy_sa_handle_issame(int unit, soc_port_t port, unsigned int macsec_side,
                                                          const bcm_plp_base_t_secy_sa_handle_t * const handle1_p,
                                                          const bcm_plp_base_t_secy_sa_handle_t * const handle2_p);
int imacsec_plp_base_t_secy_sa_handle_sa_index_issame(int unit, soc_port_t port, unsigned int macsec_side,
                                                                      const bcm_plp_base_t_secy_sa_handle_t sa_handle,
                                                                      const unsigned int sa_index);
unsigned char imacsec_plp_base_t_secy_rule_handle_issame(int unit, soc_port_t port, unsigned int macsec_side,
                                                                      const bcm_plp_base_t_secy_rule_handle_t * const handle1_p,
                                                                      const bcm_plp_base_t_secy_rule_handle_t * const handle2_p);
int imacsec_plp_base_t_secy_sa_index_get(int unit, soc_port_t port, unsigned int macsec_side,
                                                    const bcm_plp_base_t_secy_sa_handle_t sa_handle,
                                                    unsigned int * const sa_index_p,
                                                    unsigned int * const sc_index_p);
int imacsec_plp_base_t_secy_rule_index_get(int unit, soc_port_t port, unsigned int macsec_side,
                                                      const bcm_plp_base_t_secy_rule_handle_t rule_handle,
                                                      unsigned int * const rule_index_p);
int imacsec_plp_base_t_secy_vport_index_get(int unit, soc_port_t port, unsigned int macsec_side,
                                                     const bcm_plp_base_t_secy_vport_handle_t vport_handle,
                                                     unsigned int * const vport_index_p);
int imacsec_plp_base_t_secy_sa_handle_get(int unit, soc_port_t port, unsigned int macsec_side,
                                                      const unsigned int sa_index,
                                                      bcm_plp_base_t_secy_sa_handle_t * const sa_handle_p);

int imacsec_plp_base_t_secy_rule_handle_get(int unit, soc_port_t port, unsigned int macsec_side,
                                                      const unsigned int rule_index,
                                                      bcm_plp_base_t_secy_rule_handle_t * const rule_handle_p);
int imacsec_plp_base_t_secy_vport_handle_get(int unit, soc_port_t port, unsigned int macsec_side,
                                                         const unsigned int vport_index,
                                                         bcm_plp_base_t_secy_vport_handle_t * const vport_handle_p);
int imacsec_plp_base_t_secy_device_update(int unit, soc_port_t port, unsigned int macsec_side,
                                                      const bcm_plp_base_t_secy_device_params_t  * const device_p );

int imacsec_plp_base_t_secy_bypass_set(int unit, soc_port_t port, unsigned int macsec_side,
                                                 const unsigned char fbypass);

int imacsec_plp_base_t_secy_bypass_get(int unit, soc_port_t port, unsigned int macsec_side,
                                                unsigned char *fbypass);

int imacsec_plp_base_t_secy_sa_nextpn_update(int unit, soc_port_t port, unsigned int macsec_side,
                                                     const bcm_plp_base_t_secy_sa_handle_t sa_handle,
                                                     const unsigned int next_pn_lo,
                                                     const unsigned int next_pn_hi,
                                                     unsigned char * const fnext_pn_written_p);
int imacsec_plp_base_t_secy_crypt_auth_bypass_len_update(int unit, soc_port_t port, unsigned int macsec_side,
                                                                          const unsigned int bypass_length);
int imacsec_plp_base_t_secy_rules_mtu_check_update(int unit, soc_port_t port, unsigned int macsec_side,
                                                         const unsigned int sci_index,
                                                         const bcm_plp_base_t_secy_sc_rule_mtu_check_t * const mtucheck_rule_p);

int imacsec_plp_base_t_secy_tcam_statistics_get(int unit, soc_port_t port, unsigned int macsec_side,
                                                          const bcm_plp_base_t_secy_rule_handle_t rule_handle,
                                                          bcm_plp_base_t_secy_tcam_stat_t * const stat_p,
                                                          const unsigned char fsync);
int imacsec_plp_base_t_secy_rxcam_statistics_get(int unit, soc_port_t port, unsigned int macsec_side,
                                                         const unsigned int scindex,
                                                         bcm_plp_base_t_secy_rxcam_stat_t* const stats_p,
                                                         const unsigned char fsync);
int imacsec_plp_base_t_secy_secy_statistics_i_get(int unit, soc_port_t port, unsigned int macsec_side,
                                                              bcm_plp_base_t_secy_vport_handle_t vport_handle,
                                                              bcm_plp_base_t_secy_secy_stat_i_t * const stats_p,
                                                              const unsigned char fsync);
int imacsec_plp_base_t_secy_secy_statistics_e_get(int unit, soc_port_t port, unsigned int macsec_side,
                                                              bcm_plp_base_t_secy_vport_handle_t vport_handle,
                                                              bcm_plp_base_t_secy_secy_stat_e_t * const stats_p,
                                                              const unsigned char fsync);
int imacsec_plp_base_t_secy_sa_statistics_i_get(int unit, soc_port_t port, unsigned int macsec_side,
                                                          const bcm_plp_base_t_secy_sa_handle_t sa_handle,
                                                          bcm_plp_base_t_secy_sa_stat_i_t * const stats_p,
                                                          const unsigned char fsync);
int imacsec_plp_base_t_secy_sa_statistics_e_get(int unit, soc_port_t port, unsigned int macsec_side,
                                                         const bcm_plp_base_t_secy_sa_handle_t sa_handle,
                                                         bcm_plp_base_t_secy_sa_stat_e_t * const stats_p,
                                                         const unsigned char fsync);
int imacsec_plp_base_t_secy_global_statistics_get(int unit, soc_port_t port, unsigned int macsec_side,
                                                            bcm_plp_base_t_secy_global_stat_t * const stat_p,
                                                            const unsigned char fsync);
int imacsec_plp_base_t_secy_ifc_statistics_e_get(int unit, soc_port_t port, unsigned int macsec_side,
                                                           bcm_plp_base_t_secy_vport_handle_t vport_handle,
                                                           bcm_plp_base_t_secy_ifc_stat_e_t * const stats_p,
                                                           const unsigned char fsync);
int imacsec_plp_base_t_secy_ifc_statistics_i_get(int unit, soc_port_t port, unsigned int macsec_side,
                                                          bcm_plp_base_t_secy_vport_handle_t vport_handle,
                                                          bcm_plp_base_t_secy_ifc_stat_i_t * const stats_p,
                                                          const unsigned char fsync);
int imacsec_plp_base_t_secy_device_count_summary_pifc_checkandclear(int unit, soc_port_t port, unsigned int macsec_side,
                                                                                         bcm_plp_base_t_secy_vport_handle_t ** const vport_handle_pp,
                                                                                         unsigned int * const num_ifc_indexes_p);
int imacsec_plp_base_t_secy_device_count_summary_prxcam_checkandclear(int unit, soc_port_t port, unsigned int macsec_side,
                                                                                             unsigned int ** const rx_cam_indexes_pp,
                                                                                             unsigned int * const num_rx_cam_indexes_p);

int imacsec_plp_base_t_secy_device_count_summary_psa_checkandclear(int unit, soc_port_t port, unsigned int macsec_side,
                                                                                       bcm_plp_base_t_secy_sa_handle_t ** const sa_handle_pp,
                                                                                       unsigned int * const num_sa_indexes_p);
int imacsec_plp_base_t_secy_device_count_summary_psecy_checkandclear(int unit, soc_port_t port, unsigned int macsec_side,
                                                                                         bcm_plp_base_t_secy_vport_handle_t ** const vport_handle_pp,
                                                                                         unsigned int * const nm_secy_indexes_p);
int imacsec_plp_base_t_secy_device_count_summary_ptcam_checkandclear(int unit, soc_port_t port, unsigned int macsec_side,
                                                                                 bcm_plp_base_t_secy_rule_handle_t ** const rule_handle_pp,
                                                                                 unsigned int * const num_tcam_indexes_p);
int imacsec_plp_base_t_secy_device_count_summary_sa_checkandclear(int unit, soc_port_t port, unsigned int macsec_side,
                                                                                    bcm_plp_base_t_secy_sa_handle_t sa_handle,
                                                                                    unsigned int * const count_summary_sa_p);
int imacsec_plp_base_t_secy_device_count_summary_secy_checkandclear(int unit, soc_port_t port, unsigned int macsec_side,
                                                                                        bcm_plp_base_t_secy_vport_handle_t vport_handle,
                                                                                        unsigned int * const count_summary_secy_p);
int imacsec_plp_base_t_secy_device_count_summary_tcam_global_checkandclear(int unit, soc_port_t port, unsigned int macsec_side,
                                                                                             unsigned int * const count_summary_tcam_global_p);
int imacsec_plp_base_t_secy_device_count_summary_ifc_checkandclear(int unit, soc_port_t port, unsigned int macsec_side,
                                                                                     bcm_plp_base_t_secy_vport_handle_t vport_handle,
                                                                                     unsigned int * const count_summary_ifc_p);
int imacsec_plp_base_t_secy_sa_expired_summary_checkandclear(int unit, soc_port_t port, unsigned int macsec_side,
                                                                           bcm_plp_base_t_secy_sa_handle_t ** const sa_handle_pp,
                                                                           unsigned int * const num_sa_indexes_p);
int imacsec_plp_base_t_secy_sa_pnthr_summary_checkandclear(int unit, soc_port_t port, unsigned int macsec_side,
                                                                             bcm_plp_base_t_secy_sa_handle_t ** const sa_handle_pp,
                                                                             unsigned int * const num_sa_indexes_p);
int imacsec_plp_base_t_secy_intr_enable_set(int unit, soc_port_t port, unsigned int macsec_side,
                                                       const bcm_plp_base_t_secy_intr_t *secy_intr_p);
int imacsec_plp_base_t_secy_intr_enable_get(int unit, soc_port_t port, unsigned int macsec_side,
                                                       bcm_plp_base_t_secy_intr_t *secy_intr_p);
int imacsec_plp_base_t_secy_intr_status_get(int unit, soc_port_t port, unsigned int macsec_side,
                                                       bcm_plp_base_t_secy_intr_t *secy_intr_p);
int imacsec_plp_base_t_secy_intr_status_clear(int unit, soc_port_t port, unsigned int macsec_side,
                                                   const bcm_plp_base_t_secy_intr_t *secy_intr_p);
int imacsec_plp_base_t_secy_event_status_get(int unit, soc_port_t port, unsigned int macsec_side,
	                                                    bcm_plp_base_t_secy_intr_t *secy_intr_p);
bcm_plp_base_t_secy_status_t imacsec_plp_base_t_secy_intr_set(int unit, soc_port_t port, unsigned int macsec_side,
                                                                    const bcm_plp_base_t_secy_intr_t *secy_intr, const unsigned int enable);

bcm_plp_base_t_secy_status_t imacsec_plp_base_t_secy_intr_get(int unit, soc_port_t port, unsigned int macsec_side,
                                                                     bcm_plp_base_t_secy_intr_t *secy_intr, unsigned int *enable);

int imacsec_plp_base_t_secy_build_transform_record(int unit, soc_port_t port, unsigned int macsec_side,
                                                                 bcm_plp_base_t_sa_builder_params_t *params,
                                                                 unsigned int *sa_buffer_p);
int imacsec_plp_base_t_secy_sa_active_e_get(int unit, soc_port_t port, unsigned int macsec_side,
                                                        const unsigned int vport,
                                                        bcm_plp_base_t_secy_sa_handle_t * const active_sa_handle_p);

int imacsec_plp_base_t_secy_sa_active_i_get(int unit, soc_port_t port, unsigned int macsec_side,
                                  const unsigned int vport,
                                  const unsigned char * const sci_p,
                                  bcm_plp_base_t_secy_sa_handle_t * const active_sa_handle_p);

int imacsec_plp_base_t_secy_sa_window_size_update(int unit, soc_port_t port, unsigned int macsec_side,
                                                                 const bcm_plp_base_t_secy_sa_handle_t sa_handle,
                                                                 const unsigned int window_size);


int imacsec_plp_base_t_version_get(int unit,soc_port_t port, bcm_plp_base_t_version_t* version_info);

int imacsec_plp_addr_read(int unit,            /* attached switch unit */
                          unsigned int port,         /* port number */
                          unsigned int reg_addr,     /* register address */
                          unsigned int *value);    /* read register value */

int imacsec_plp_addr_write(int unit,           /* attached switch unit */
                           unsigned int port,        /* port number */
                           unsigned int reg_addr,    /* register address  */
                           unsigned int value);    /* register value to write */
#endif /* BCM_IMACSEC_H */
