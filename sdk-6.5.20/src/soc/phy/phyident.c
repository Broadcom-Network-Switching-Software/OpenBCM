/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        phyident.c
 * Purpose:     These routines and structures are related to
 *              figuring out phy identification and correlating
 *              addresses to drivers
 */

#include <sal/types.h>
#include <sal/core/boot.h>
#include <shared/bsl.h>
#include <soc/drv.h>
#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/phyreg.h>
#include <soc/phy.h>
#include <soc/phyctrl.h>
#include <soc/phy/phyctrl.h>
#include <soc/phy/drv.h>

#ifdef PHYMOD_SUPPORT
#include <soc/phy/phymod_sim.h>
#include <phymod/chip/tsce_sim.h>
#include <phymod/chip/tscf_sim.h>
#include <phymod/chip/qsgmiie_sim.h>
#include <phymod/chip/viper_sim.h>
#include <phymod/chip/eagle_sim.h>
#include <phymod/chip/qtce_sim.h>
#include <phymod/chip/tscbh_sim.h>
#endif

#ifdef BCM_ARAD_SUPPORT
#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/ARAD/NIF/ports_manager.h>
#include <soc/dpp/drv.h>
#endif

#ifdef BCM_KATANA2_SUPPORT
#include <soc/katana2.h>
#endif

#ifdef BCM_SABER2_SUPPORT
#include <soc/saber2.h>
#endif

#ifdef BCM_GREYHOUND_SUPPORT
#include <soc/greyhound.h>
#endif

#ifdef BCM_HURRICANE3_SUPPORT
#include <soc/wolfhound2.h>
#endif

#include "phydefs.h"  /* Must include before other phy related includes */
#include "phyident.h"
#include "phyaddr.h"
#include "physr.h"

#define _MAX_PHYS       256

typedef struct {
    uint16 int_addr;
    uint16 ext_addr;
} port_phy_addr_t;

/* Per port phy address map. */
static port_phy_addr_t *port_phy_addr[SOC_MAX_NUM_DEVICES];

static soc_phy_table_t  *phy_table[_MAX_PHYS];
static int              _phys_in_table = -1;
#if defined(INCLUDE_PHY_82109)
static int _chk_huracan(int unit, soc_port_t port, soc_phy_table_t *my_entry,
         uint16 phy_id0, uint16 phy_id1, soc_phy_info_t *pi);
#endif
static int _chk_phy(int unit, soc_port_t port, soc_phy_table_t *my_entry,
                    uint16 phy_id0, uint16 phy_id1, soc_phy_info_t *pi);

static int _chk_null(int unit, soc_port_t port, soc_phy_table_t *my_entry,
                     uint16 phy_id0, uint16 phy_id1, soc_phy_info_t *pi);

static int _chk_sfp_phy(int unit, soc_port_t port, soc_phy_table_t *my_entry,
                    uint16 phy_id0, uint16 phy_id1, soc_phy_info_t *pi);

#if defined(INCLUDE_PHY_SIMUL)
static int _chk_simul(int unit, soc_port_t port, soc_phy_table_t *my_entry,
                      uint16 phy_id0, uint16 phy_id1, soc_phy_info_t *pi);
#endif /* INCLUDE_PHY_SIMUL */

#if defined(INCLUDE_PHY_5690)
static int _chk_fiber5690(int unit, soc_port_t port,
                          soc_phy_table_t *my_entry,
                          uint16 phy_id0, uint16 phy_id1, soc_phy_info_t *pi);
#endif /* INCLUDE_PHY_5690 */

#if defined(INCLUDE_XGXS_QSGMII65)
static int _chk_qsgmii53314(int unit, soc_port_t port,
                          soc_phy_table_t *my_entry,
                          uint16 phy_id0, uint16 phy_id1, soc_phy_info_t *pi);
#endif /* INCLUDE_XGXS_QSGMII65 */

#if defined(INCLUDE_PHY_54680)
static int _chk_qgphy_5332x(int unit, soc_port_t port, 
                          soc_phy_table_t *my_entry,
                          uint16 phy_id0, uint16 phy_id1, soc_phy_info_t *pi);
#endif /* INCLUDE_PHY_54680 */

#if defined(INCLUDE_PHY_56XXX)
static int _chk_fiber56xxx(int unit, soc_port_t port,
                          soc_phy_table_t *my_entry,
                          uint16 phy_id0, uint16 phy_id1, soc_phy_info_t *pi);
#endif /* INCLUDE_PHY_56XXX */

#if defined(INCLUDE_PHY_56XXX)
static int _chk_fiber56xxx_5601x(int unit, soc_port_t port,
                          soc_phy_table_t *my_entry,
                          uint16 phy_id0, uint16 phy_id1, soc_phy_info_t *pi);
#endif /* INCLUDE_PHY_56XXX */

#if defined(INCLUDE_SERDES_COMBO)
static int _chk_serdes_combo_5601x(int unit, soc_port_t port,
                          soc_phy_table_t *my_entry,
                          uint16 phy_id0, uint16 phy_id1, soc_phy_info_t *pi);
#endif /* INCLUDE_SERDES_COMBO */

#if defined(INCLUDE_SERDES_65LP)
static int _chk_serdes_65lp(int unit, soc_port_t port,
                          soc_phy_table_t *my_entry,
                          uint16 phy_id0, uint16 phy_id1, soc_phy_info_t *pi);
#endif /* INCLUDE_SERDES_65LP */

#if defined(INCLUDE_PHY_XGXS6)
static int _chk_unicore(int unit, soc_port_t port,
                        soc_phy_table_t *my_entry,
                        uint16 phy_id0, uint16 phy_id1, soc_phy_info_t *pi);
#endif /* INCLUDE_PHY_XGXS6 */

#if defined(INCLUDE_PHY_8706)
static int _chk_8706(int unit, soc_port_t port, soc_phy_table_t *my_entry,
                     uint16 phy_id0, uint16 phy_id1, soc_phy_info_t *pi);
#endif /* INCLUDE_PHY_8706 */

#if defined(INCLUDE_PHY_8072)
static int _chk_8072(int unit, soc_port_t port, soc_phy_table_t *my_entry,
                     uint16 phy_id0, uint16 phy_id1, soc_phy_info_t *pi);
#endif /* INCLUDE_PHY_8072 */

#if defined(INCLUDE_PHY_8040)
static int _chk_8040(int unit, soc_port_t port, soc_phy_table_t *my_entry,
                     uint16 phy_id0, uint16 phy_id1, soc_phy_info_t *pi);
#endif /* INCLUDE_PHY_8040 */

#if defined(INCLUDE_PHY_8481)
static int _chk_8481(int unit, soc_port_t port, soc_phy_table_t *my_entry,
                     uint16 phy_id0, uint16 phy_id1, soc_phy_info_t *pi);
#endif /* INCLUDE_PHY_8481 */

#if defined(INCLUDE_SERDES_COMBO65)
static int
_chk_serdescombo65(int unit, soc_port_t port, soc_phy_table_t *my_entry,
         uint16 phy_id0, uint16 phy_id1, soc_phy_info_t *pi);

#endif /* INCLUDE_SERDES_COMBO65 */

#if defined(INCLUDE_XGXS_16G)
static int
_chk_xgxs16g1l(int unit, soc_port_t port, soc_phy_table_t *my_entry,
         uint16 phy_id0, uint16 phy_id1, soc_phy_info_t *pi);
#endif /* INCLUDE_XGXS_16G */
#if defined(INCLUDE_XGXS_VIPER)
static int
_chk_viper(int unit, soc_port_t port, soc_phy_table_t *my_entry,
         uint16 phy_id0, uint16 phy_id1, soc_phy_info_t *pi);
#endif

#if defined(INCLUDE_XGXS_WCMOD)
static int
_chk_wcmod(int unit, soc_port_t port, soc_phy_table_t *my_entry,
         uint16 phy_id0, uint16 phy_id1, soc_phy_info_t *pi);
#endif  /* INCLUDE_XGXS_WCMOD */

#if defined(INCLUDE_PHY_53XXX)
static int _chk_fiber53xxx(int unit, soc_port_t port,
                          soc_phy_table_t *my_entry,
                          uint16 phy_id0, uint16 phy_id1, soc_phy_info_t *pi);
#endif /* INCLUDE_PHY_53XXX */

static int _chk_default(int unit, soc_port_t port, soc_phy_table_t *my_entry,
                        uint16 phy_id0, uint16 phy_id1, soc_phy_info_t *pi);

static soc_known_phy_t
    _phy_ident_type_get(uint16 phy_id0, uint16 phy_id1);

static soc_phy_table_t _null_phy_entry =
    {_chk_null, _phy_id_NULL,   "Null",    &phy_null,          NULL};

#if defined(INCLUDE_PHY_SIMUL)
static soc_phy_table_t _simul_phy_entry =
    {_chk_simul, _phy_id_SIMUL, "Simulation", &phy_simul,      NULL};
#endif /* INCLUDE_PHY_SIMUL */

#if defined(INCLUDE_PHY_5690)
static soc_phy_table_t _fiber5690_phy_entry =
    {_chk_fiber5690, _phy_id_NULL, "Internal SERDES", &phy_5690drv_ge, NULL };
#endif /* INCLUDE_PHY_5690 */

#if defined(INCLUDE_PHY_56XXX)
static soc_phy_table_t _fiber56xxx_phy_entry =
    {_chk_fiber56xxx, _phy_id_NULL, "Internal SERDES", &phy_56xxxdrv_ge, NULL };
#endif /* INCLUDE_PHY_56XXX */

#if defined(INCLUDE_PHY_53XXX)
static soc_phy_table_t _fiber53xxx_phy_entry =
    {_chk_fiber53xxx, _phy_id_NULL, "Internal  SERDES", &phy_53xxxdrv_ge, NULL};
#endif /* INCLUDE_PHY_53XXX */

static soc_phy_table_t _default_phy_entry =
    {_chk_default, _phy_id_NULL, "Unknown", &phy_drv_fe, NULL};


/*
 * Variable:
 *      _standard_phy_table
 * Purpose:
 *      Defines the standard supported Broadcom PHYs, and the corresponding
 *      driver.
 */

static soc_phy_table_t _standard_phy_table[] = {

#ifdef INCLUDE_PHY_522X
    {_chk_phy, _phy_id_BCM5218, "BCM5218",     &phy_522xdrv_fe, NULL},
    {_chk_phy, _phy_id_BCM5220, "BCM5220/21",  &phy_522xdrv_fe, NULL},
    {_chk_phy, _phy_id_BCM5226, "BCM5226",     &phy_522xdrv_fe, NULL},
    {_chk_phy, _phy_id_BCM5228, "BCM5228",     &phy_522xdrv_fe, NULL},
    {_chk_phy, _phy_id_BCM5238, "BCM5238",     &phy_522xdrv_fe, NULL},
    {_chk_phy, _phy_id_BCM5248, "BCM5248",     &phy_522xdrv_fe, NULL},
#endif /* INCLUDE_PHY_522X */

    {_chk_phy, _phy_id_BCM5400, "BCM5400",     &phy_drv_ge, NULL},

#ifdef INCLUDE_PHY_54XX
    {_chk_phy, _phy_id_BCM5401, "BCM5401",     &phy_5401drv_ge, NULL},
    {_chk_phy, _phy_id_BCM5402, "BCM5402",     &phy_5402drv_ge, NULL},
    {_chk_phy, _phy_id_BCM5404, "BCM5404",     &phy_5404drv_ge, NULL},
    {_chk_phy, _phy_id_BCM5424, "BCM5424/34",  &phy_5424drv_ge, NULL},
    {_chk_phy, _phy_id_BCM5411, "BCM5411",     &phy_5411drv_ge, NULL},
#endif /* INCLUDE_PHY_54XX */


#if defined(INCLUDE_PHY_5464_ESW)
    {_chk_phy, _phy_id_BCM5461, "BCM5461",     &phy_5464drv_ge, NULL},
    {_chk_phy, _phy_id_BCM5464, "BCM5464",     &phy_5464drv_ge, NULL},
    {_chk_phy, _phy_id_BCM5466, "BCM5466",     &phy_5464drv_ge, NULL},
    {_chk_phy, _phy_id_BCM5478, "BCM5478",     &phy_5464drv_ge, NULL},
    {_chk_phy, _phy_id_BCM5488, "BCM5488",     &phy_5464drv_ge, NULL},
    {_chk_phy, _phy_id_BCM54980, "BCM54980",   &phy_5464drv_ge, NULL},
    {_chk_phy, _phy_id_BCM54980C, "BCM54980",  &phy_5464drv_ge, NULL},
    {_chk_phy, _phy_id_BCM54980V, "BCM54980",  &phy_5464drv_ge, NULL},
    {_chk_phy, _phy_id_BCM54980VC, "BCM54980", &phy_5464drv_ge, NULL},
    {_chk_phy, _phy_id_BCM53314, "BCM53314",   &phy_5464drv_ge, NULL},
#endif /* INCLUDE_PHY_5464 */


#if defined(INCLUDE_PHY_5482_ESW)
    {_chk_phy, _phy_id_BCM5482, "BCM5482/801x",     &phy_5482drv_ge, NULL},
#endif /* INCLUDE_PHY_5482 */

#if defined(INCLUDE_PHY_54684)
    {_chk_phy, _phy_id_BCM54684, "BCM54684", &phy_54684drv_ge, NULL},
#endif /* defined(INCLUDE_PHY_54684) */

#if defined(INCLUDE_PHY_54640)
    {_chk_phy, _phy_id_BCM54640, "BCM54640", &phy_54640drv_ge, NULL},
#endif /* defined(INCLUDE_PHY_54640) */

#if defined(INCLUDE_PHY_54682)
    {_chk_phy, _phy_id_BCM54682, "BCM54682E", &phy_54682drv_ge, NULL},
    {_chk_phy, _phy_id_BCM54684E, "BCM54684E", &phy_54682drv_ge, NULL},
    {_chk_phy, _phy_id_BCM54685, "BCM54685", &phy_54682drv_ge, NULL},
#endif /* defined(INCLUDE_PHY_54682) */

#ifdef INCLUDE_PHY_54616
    {_chk_phy, _phy_id_BCM54616  , "BCM54616"  , &phy_54616drv_ge, NULL},
    {_chk_phy, _phy_id_BCM54618E , "BCM54618E" , &phy_54616drv_ge, NULL},
    {_chk_phy, _phy_id_BCM54618SE, "BCM54618SE", &phy_54616drv_ge, NULL},
#endif /* INCLUDE_PHY_54616 */

#ifdef INCLUDE_PHY_84728 
    {_chk_phy, _phy_id_BCM84707, "BCM84707",  &phy_84728drv_xe,  NULL}, 
    {_chk_phy, _phy_id_BCM84073, "BCM84073",  &phy_84728drv_xe,  NULL}, 
    {_chk_phy, _phy_id_BCM84074, "BCM84074",  &phy_84728drv_xe,  NULL}, 
    {_chk_phy, _phy_id_BCM84728, "BCM84728",  &phy_84728drv_xe,  NULL}, 
    {_chk_phy, _phy_id_BCM84748, "BCM84748",  &phy_84728drv_xe,  NULL}, 
    {_chk_phy, _phy_id_BCM84727, "BCM84727",  &phy_84728drv_xe,  NULL}, 
    {_chk_phy, _phy_id_BCM84747, "BCM84747",  &phy_84728drv_xe,  NULL}, 
    {_chk_phy, _phy_id_BCM84762, "BCM84762",  &phy_84728drv_xe,  NULL}, 
    {_chk_phy, _phy_id_BCM84764, "BCM84764",  &phy_84728drv_xe,  NULL}, 
    {_chk_phy, _phy_id_BCM84042, "BCM84042",  &phy_84728drv_xe,  NULL}, 
    {_chk_phy, _phy_id_BCM84044, "BCM84044",  &phy_84728drv_xe,  NULL}, 
#endif /* INCLUDE_PHY_84728 */ 

#ifdef INCLUDE_PHY_8806X
    {_chk_phy, _phy_id_BCM8806x, "BCM8806X",  &phy_8806xdrv,  NULL},
#endif /* INCLUDE_PHY_8806X */

#ifdef INCLUDE_MACSEC
#if defined(INCLUDE_PHY_54580)
    {_chk_phy, _phy_id_BCM54584, "BCM54584", &phy_54580drv_ge, NULL},
    {_chk_phy, _phy_id_BCM54580, "BCM54580", &phy_54580drv_ge, NULL},
    {_chk_phy, _phy_id_BCM54540, "BCM54540", &phy_54580drv_ge, NULL},
    {_chk_phy, _phy_id_BCM54585, "BCM54584", &phy_54580drv_ge, NULL},
#endif /* defined(INCLUDE_PHY_54580) */
#if defined(INCLUDE_PHY_54380)
    {_chk_phy, _phy_id_BCM54380, "BCM54380", &phy_54380drv_ge, NULL},
    {_chk_phy, _phy_id_BCM54382, "BCM54382", &phy_54380drv_ge, NULL},
    {_chk_phy, _phy_id_BCM54340, "BCM54340", &phy_54380drv_ge, NULL},
    {_chk_phy, _phy_id_BCM54385, "BCM54385", &phy_54380drv_ge, NULL},
#endif /* defined(INCLUDE_PHY_54380) */
#ifdef INCLUDE_PHY_8729
    {_chk_phy, _phy_id_BCM8729, "BCM8729",  &phy_8729drv_gexe,  NULL},
#endif /* INCLUDE_PHY_8729 */
#ifdef INCLUDE_PHY_84756 
    {_chk_phy, _phy_id_BCM84756, "BCM84756",  &phy_84756drv_xe,  NULL},
    {_chk_phy, _phy_id_BCM84757, "BCM84757",  &phy_84756drv_xe,  NULL},
    {_chk_phy, _phy_id_BCM84759, "BCM84759",  &phy_84756drv_xe,  NULL},
#endif /* INCLUDE_PHY_84756 */
#ifdef INCLUDE_PHY_84334
    {_chk_phy, _phy_id_BCM84334, "BCM84334",  &phy_84334drv_xe,  NULL},
    {_chk_phy, _phy_id_BCM84333, "BCM84333",  &phy_84334drv_xe,  NULL},
    {_chk_phy, _phy_id_BCM84336, "BCM84336",  &phy_84334drv_xe,  NULL},
    {_chk_phy, _phy_id_BCM84844, "BCM84844",  &phy_84334drv_xe,  NULL},
    {_chk_phy, _phy_id_BCM84846, "BCM84846",  &phy_84334drv_xe,  NULL},
    {_chk_phy, _phy_id_BCM84848, "BCM84848",  &phy_84334drv_xe,  NULL},
#endif /* INCLUDE_PHY_84334 */
#ifdef INCLUDE_PHY_84749 
    {_chk_phy, _phy_id_BCM84749, "BCM84749",  &phy_84749drv_xe,  NULL},
    {_chk_phy, _phy_id_BCM84729, "BCM84729",  &phy_84749drv_xe,  NULL},
#endif /* INCLUDE_PHY_84749 */
#endif  /* INCLUDE_MACSEC */
#if defined(INCLUDE_PHY_542XX)
    {_chk_phy, _phy_id_BCM54182, "BCM54182", &phy_542xxdrv_ge, NULL},
    {_chk_phy, _phy_id_BCM54185, "BCM54185", &phy_542xxdrv_ge, NULL},
    {_chk_phy, _phy_id_BCM54180, "BCM54180", &phy_542xxdrv_ge, NULL},
    {_chk_phy, _phy_id_BCM54140, "BCM54140", &phy_542xxdrv_ge, NULL},
    {_chk_phy, _phy_id_BCM54192, "BCM54192", &phy_542xxdrv_ge, NULL},
    {_chk_phy, _phy_id_BCM54195, "BCM54195", &phy_542xxdrv_ge, NULL},
    {_chk_phy, _phy_id_BCM54190, "BCM54190", &phy_542xxdrv_ge, NULL},
    {_chk_phy, _phy_id_BCM54194, "BCM54194", &phy_542xxdrv_ge, NULL},
    {_chk_phy, _phy_id_BCM54210, "BCM54210", &phy_542xxdrv_ge, NULL},
    {_chk_phy, _phy_id_BCM54220, "BCM54220", &phy_542xxdrv_ge, NULL},
    {_chk_phy, _phy_id_BCM54280, "BCM54280", &phy_542xxdrv_ge, NULL},
    {_chk_phy, _phy_id_BCM54282, "BCM54282", &phy_542xxdrv_ge, NULL},
    {_chk_phy, _phy_id_BCM54240, "BCM54240", &phy_542xxdrv_ge, NULL},
    {_chk_phy, _phy_id_BCM54285, "BCM54285", &phy_542xxdrv_ge, NULL},
    {_chk_phy, _phy_id_BCM5428x, "BCM5428X", &phy_542xxdrv_ge, NULL},
    {_chk_phy, _phy_id_BCM54290, "BCM54290", &phy_542xxdrv_ge, NULL},
    {_chk_phy, _phy_id_BCM54292, "BCM54292", &phy_542xxdrv_ge, NULL},
    {_chk_phy, _phy_id_BCM54294, "BCM54294", &phy_542xxdrv_ge, NULL},
    {_chk_phy, _phy_id_BCM54295, "BCM54295", &phy_542xxdrv_ge, NULL},
    {_chk_phy, _phy_id_BCM54296, "BCM54296", &phy_542xxdrv_ge, NULL},
#endif /* defined(INCLUDE_PHY_542XX) */
#ifdef INCLUDE_PHY_84756
#if defined(INCLUDE_FCMAP) || defined(INCLUDE_MACSEC)
    {_chk_phy, _phy_id_BCM84756, "BCM84756",  &phy_84756drv_fcmap_xe,  NULL},
    {_chk_phy, _phy_id_BCM84757, "BCM84757",  &phy_84756drv_fcmap_xe,  NULL},
    {_chk_phy, _phy_id_BCM84759, "BCM84759",  &phy_84756drv_fcmap_xe,  NULL},
#endif /* INCLUDE_FCMAP  || INCLUDE_MACSEC */
#endif /* INCLUDE_PHY_84756 */

#ifdef INCLUDE_PHY_5421S
    {_chk_phy, _phy_id_BCM5421, "BCM5421S",    &phy_5421Sdrv_ge, NULL},
#endif /* INCLUDE_PHY_5421S */

#ifdef INCLUDE_PHY_54680
    {_chk_phy, _phy_id_BCM54680, "BCM54680", &phy_54680drv_ge, NULL},
    {_chk_qgphy_5332x, _phy_id_BCM53324, "BCM53324", &phy_54680drv_ge, NULL},
#endif /* INCLUDE_PHY_54680 */

#ifdef INCLUDE_PHY_EGPHY28
    {_chk_phy, _phy_id_BCM56160_GPHY, "BCM56160-GPHY", &phy_egphy28drv_ge, NULL},
    {_chk_phy, _phy_id_BCM53540, "BCM53540-GPHY", &phy_egphy28drv_ge, NULL},
#endif /* INCLUDE_PHY_EGPHY28 */

#ifdef INCLUDE_PHY_EGPHY16
    {_chk_phy, _phy_id_BCM56275_GPHY, "BCM56275-GPHY", &phy_egphy16drv_ge, NULL},
#endif /* INCLUDE_PHY_EGPHY16 */

#ifdef INCLUDE_PHY_54880
    {_chk_phy, _phy_id_BCM54880, "BCM54880", &phy_54880drv_ge, NULL},
    {_chk_phy, _phy_id_BCM54881, "BCM54881", &phy_54880drv_ge, NULL},
    {_chk_phy, _phy_id_BCM54810, "BCM54810", &phy_54880drv_ge, NULL},
    {_chk_phy, _phy_id_BCM54811, "BCM54811", &phy_54880drv_ge, NULL},
#endif /* INCLUDE_PHY_54880 */

#ifdef INCLUDE_PHY_54640E
    {_chk_phy, _phy_id_BCM54680E, "BCM54640E", &phy_54640drv_ge, NULL},
#endif /* INCLUDE_PHY_54640E */

#ifdef INCLUDE_PHY_54880E
    {_chk_phy, _phy_id_BCM54880E, "BCM54880E", &phy_54680drv_ge, NULL},
#endif /* INCLUDE_PHY_54880E */

#ifdef INCLUDE_PHY_54680E
    {_chk_phy, _phy_id_BCM54680E, "BCM54680E", &phy_54680drv_ge, NULL},
#endif /* INCLUDE_PHY_54680E */

#ifdef INCLUDE_PHY_52681E
    {_chk_phy, _phy_id_BCM52681E, "BCM52681E", &phy_54680drv_ge, NULL},
#endif /* INCLUDE_PHY_52681E */

#ifdef INCLUDE_PHY_8703
    {_chk_phy, _phy_id_BCM8703, "BCM8703",  &phy_8703drv_xe,  NULL},
    {_chk_phy, _phy_id_BCM8704, "BCM8704",  &phy_8703drv_xe,  NULL},
#endif /* INCLUDE_PHY_8703 */
#ifdef INCLUDE_PHY_8705
    {_chk_phy, _phy_id_BCM8705, "BCM8705/24/25",  &phy_8705drv_xe,  NULL},
#endif /* INCLUDE_PHY_8705 */
#if defined(INCLUDE_PHY_8706)
    /* BCM8706_A0 and BCM8705 has the same device ID. Therefore, the probe must
     * check for 8706 before 8705 to correctly attach BCM8706. For 8706,
     * phy_8706 config must be set.
     */
    {_chk_8706, _phy_id_BCM8706, "BCM8706/8726", &phy_8706drv_xe, NULL},
    {_chk_8706, _phy_id_BCM8727, "BCM8727", &phy_8706drv_xe, NULL},
    {_chk_8706, _phy_id_BCM8747, "BCM8728/8747", &phy_8706drv_xe, NULL},
#endif /* INCLUDE_PHY_8706 */
#if defined(INCLUDE_PHY_8072)
    {_chk_8072, _phy_id_BCM8072, "BCM8072", &phy_8072drv_xe, NULL},
    {_chk_8072, _phy_id_BCM8073, "BCM8073", &phy_8072drv_xe, NULL},
    {_chk_8072, _phy_id_BCM8074, "BCM8074", &phy_8074drv_xe, NULL},
#endif /* INCLUDE_PHY_8072 */

#if defined(INCLUDE_PHY_8040)
    {_chk_8040, _phy_id_BCM8040, "BCM8040", &phy_8040drv_xe, NULL},
#endif /* INCLUDE_PHY_8040 */

#if defined(INCLUDE_PHY_8481)
    {_chk_8481, _phy_id_BCM8481x, "BCM8481X", &phy_8481drv_xe, NULL},
    {_chk_8481, _phy_id_BCM84812ce, "BCM84812", &phy_8481drv_xe, NULL},
    {_chk_8481, _phy_id_BCM84821, "BCM84821", &phy_8481drv_xe, NULL},
    {_chk_8481, _phy_id_BCM84822, "BCM84822", &phy_8481drv_xe, NULL},
    {_chk_8481, _phy_id_BCM84823, "BCM84823", &phy_8481drv_xe, NULL},
    {_chk_8481, _phy_id_BCM84833, "BCM84833", &phy_8481drv_xe, NULL},
    {_chk_8481, _phy_id_BCM84834, "BCM84834", &phy_8481drv_xe, NULL},
    {_chk_8481, _phy_id_BCM84835, "BCM84835", &phy_8481drv_xe, NULL},
    {_chk_8481, _phy_id_BCM84836, "BCM84836", &phy_8481drv_xe, NULL},
    {_chk_8481, _phy_id_BCM84844, "BCM84844", &phy_8481drv_xe, NULL},
    {_chk_8481, _phy_id_BCM84846, "BCM84846", &phy_8481drv_xe, NULL},
    {_chk_8481, _phy_id_BCM84848, "BCM84848", &phy_8481drv_xe, NULL},
    {_chk_8481, _phy_id_BCM84858, "BCM84858", &phy_8481drv_xe, NULL},
    {_chk_8481, _phy_id_BCM84856, "BCM84856", &phy_8481drv_xe, NULL},
    {_chk_8481, _phy_id_BCM84860, "BCM84860", &phy_8481drv_xe, NULL},
    {_chk_8481, _phy_id_BCM84861, "BCM84861", &phy_8481drv_xe, NULL},
    {_chk_8481, _phy_id_BCM84864, "BCM84864", &phy_8481drv_xe, NULL},
    {_chk_8481, _phy_id_BCM84868, "BCM84868", &phy_8481drv_xe, NULL},
    {_chk_8481, _phy_id_BCM84888, "BCM84888", &phy_8481drv_xe, NULL},
    {_chk_8481, _phy_id_BCM84884, "BCM84884", &phy_8481drv_xe, NULL},
    {_chk_8481, _phy_id_BCM84888E, "BCM84888E", &phy_8481drv_xe, NULL},
    {_chk_8481, _phy_id_BCM84884E, "BCM84884E", &phy_8481drv_xe, NULL},
    {_chk_8481, _phy_id_BCM84881, "BCM84881", &phy_8481drv_xe, NULL},
    {_chk_8481, _phy_id_BCM84880, "BCM84880", &phy_8481drv_xe, NULL},
    {_chk_8481, _phy_id_BCM84888S, "BCM84888S", &phy_8481drv_xe, NULL},
    {_chk_8481, _phy_id_BCM84887, "BCM84887", &phy_8481drv_xe, NULL},
    {_chk_8481, _phy_id_BCM84886, "BCM84886", &phy_8481drv_xe, NULL},
    {_chk_8481, _phy_id_BCM84885, "BCM84885", &phy_8481drv_xe, NULL},
#endif /* INCLUDE_PHY_8481 */
#ifdef INCLUDE_PHY_8750
    {_chk_phy, _phy_id_BCM8750, "BCM8750",  &phy_8750drv_xe,  NULL},
    {_chk_phy, _phy_id_BCM8752, "BCM8752",  &phy_8750drv_xe,  NULL},
    {_chk_phy, _phy_id_BCM8754, "BCM8754",  &phy_8750drv_xe,  NULL},
#endif /* INCLUDE_PHY_8750 */
#ifdef INCLUDE_PHY_84740
    {_chk_phy, _phy_id_BCM84740, "BCM84740",  &phy_84740drv_xe,  NULL},
    {_chk_phy, _phy_id_BCM84164, "BCM84164",  &phy_84740drv_xe,  NULL},
    {_chk_phy, _phy_id_BCM84758, "BCM84758",  &phy_84740drv_xe,  NULL},
    {_chk_phy, _phy_id_BCM84780, "BCM84780",  &phy_84740drv_xe,  NULL},
    {_chk_phy, _phy_id_BCM84784, "BCM84784",  &phy_84740drv_xe,  NULL},
    {_chk_phy, _phy_id_BCM84318, "BCM84318",  &phy_84740drv_xe,  NULL},
#endif /* INCLUDE_PHY_84740 */
#ifdef INCLUDE_PHY_84328
    {_chk_phy, _phy_id_BCM84328, "BCM84328",  &phy_84328drv_xe,  NULL},
#endif /* INCLUDE_PHY_84328 */
#ifdef INCLUDE_PHY_84793
    {_chk_phy, _phy_id_BCM84793, "BCM84793",  &phy_84793drv_ce,  NULL},
#endif
#ifdef INCLUDE_PHY_82328
    {_chk_phy, _phy_id_BCM82328, "BCM82328",  &phy_82328drv_xe,  NULL},
#endif
#if defined(INCLUDE_PHY_82381)
    {_chk_phy, _phy_id_BCM82381, "Furia", &phy82381_drv, NULL},
#endif
#if defined(INCLUDE_PHY_82109)
    {_chk_huracan, _phy_id_BCM82109, "Huracan", &phy82109_drv, NULL},
#endif
#if defined(INCLUDE_PHY_82764)
    {_chk_phy, _phy_id_BCM82764, "Sesto", &phy82764_drv, NULL},
#endif
#if defined(INCLUDE_PHY_82864)
    {_chk_phy, _phy_id_BCM82864, "Madura", &phy82864_drv, NULL},
#endif

#ifdef INCLUDE_PHY_82780
    {_chk_phy, _phy_id_BCM82780, "BCM82780", &phy82780_drv, NULL}, 
#endif

    {_chk_sfp_phy,0xffff,"copper sfp",&phy_copper_sfp_drv,NULL},
};

/* Internal PHY table */
static soc_phy_table_t _int_phy_table[] = {
#if defined(INCLUDE_PHY_56XXX)
    {_chk_fiber56xxx, _phy_id_NULL, "Internal SERDES",
     &phy_56xxxdrv_ge, NULL},
#endif /* INCLUDE_PHY_56XXX */

#if defined(INCLUDE_PHY_53XXX)
    {_chk_fiber53xxx, _phy_id_NULL, "Internal SERDES",
     &phy_53xxxdrv_ge, NULL},
#endif /* INCLUDE_PHY_53XXX */

#if defined(INCLUDE_SERDES_COMBO)
     {_chk_phy, _phy_id_SERDESCOMBO, "COMBO", &phy_serdescombo_ge, NULL},
#endif /* INCLUDE_SERDES_COMBO */

#if defined(INCLUDE_SERDES_100FX)
    {_chk_phy, _phy_id_SERDES100FX, "1000X/100FX", &phy_serdes100fx_ge, NULL},
#endif /* INCLUDE_SERDES_100FX */

#if defined(INCLUDE_SERDES_65LP)
    {_chk_serdes_65lp, _phy_id_SERDES65LP, "65LP", &phy_serdes65lp_ge, NULL},
#endif /* INCLUDE_SERDES_65LP */

#ifdef INCLUDE_XGXS_QSGMII65
    {_chk_qsgmii53314, _phy_id_NULL, "QSGMII65", &phy_qsgmii65_ge,
     NULL },
#endif /* INCLUDE_XGXS_QSGMII65 */

#if defined(INCLUDE_SERDES_COMBO65)
    {_chk_serdescombo65, _phy_id_SERDESCOMBO65, "COMBO65", 
     &phy_serdescombo65_ge, NULL},
#endif /* INCLUDE_SERDES_COMBO65 */

#if defined(INCLUDE_PHY_XGXS1)
    {_chk_phy, _phy_id_BCMXGXS1, "XGXS1",      &phy_xgxs1_hg, NULL},
    {_chk_phy, _phy_id_BCMXGXS2, "XGXS2",      &phy_xgxs1_hg, NULL},
#endif /* INCLUDE_PHY_XGXS1 */

#if defined(INCLUDE_PHY_XGXS5)
    {_chk_phy, _phy_id_BCMXGXS5, "XGXS5",      &phy_xgxs5_hg, NULL},
#endif /* INCLUDE_PHY_XGXS5 */

#if defined(INCLUDE_PHY_XGXS6)
    {_chk_phy,     _phy_id_BCMXGXS6, "XGXS6",  &phy_xgxs6_hg, NULL},
    {_chk_unicore, _phy_id_BCMXGXS2, "XGXS6",  &phy_xgxs6_hg, NULL},
#endif /* INCLUDE_PHY_XGXS6 */

    /* Must probe for newer internal SerDes/XAUI first before probing for
     * older devices. Newer devices reuse the same device ID and introduce
     * a new mechanism to differentiate betwee devices. Therefore, newer
     * PHY drivers implement probe funtion to check for correct device.
     */
#if defined(INCLUDE_XGXS_WC40)
    {_chk_phy, _phy_id_XGXS_WC40, "WC40/4",    &phy_wc40_hg, NULL},
#endif /* INCLUDE_XGXS_HL65 */

#if defined(INCLUDE_XGXS_HL65)
    {_chk_phy, _phy_id_XGXS_HL65, "HL65/4",    &phy_hl65_hg, NULL},
#endif /* INCLUDE_XGXS_HL65 */

#if defined(INCLUDE_PHY_56XXX)
    {_chk_fiber56xxx_5601x, _phy_id_NULL, "Internal SERDES",
     &phy_56xxx_5601x_drv_ge, NULL},
#endif /* INCLUDE_PHY_56XXX */

#if defined(INCLUDE_SERDES_COMBO)
    {_chk_serdes_combo_5601x, _phy_id_NULL, "COMBO SERDES",
     &phy_serdescombo_5601x_ge, NULL},
#endif /* INCLUDE_SERDES_COMBO */

#if defined(INCLUDE_XGXS_16G)
    {_chk_phy, _phy_id_XGXS_16G, "XGXS16G",    &phy_xgxs16g_hg, NULL},
    {_chk_xgxs16g1l, _phy_id_XGXS_16G, "XGXS16G/1", &phy_xgxs16g1l_ge, NULL},
#endif /* INCLUDE_XGXS_16G */

#if defined(INCLUDE_XGXS_WCMOD)
    {_chk_wcmod, _phy_id_XGXS_WL, "WCMOD/4",    &phy_wcmod_hg, NULL},
#endif  /* INCLUDE_XGXS_WL */

#if defined(INCLUDE_XGXS_TSCMOD)
    {_chk_phy, _phy_id_XGXS_TSC, "TSCMOD/4",    &phy_tscmod_hg, NULL},
#endif  /* INCLUDE_XGXS_TSC */

#if defined(INCLUDE_XGXS_TSCE)
    {_chk_phy, _phy_id_XGXS_TSC, "TSCE", &phy_tsce_drv, NULL},
#endif /* INCLUDE_XGXS_TSCE */

#if defined(INCLUDE_XGXS_TSCF)
    {_chk_phy, _phy_id_XGXS_TSC, "TSCF", &phy_tscf_drv, NULL},
#endif /* INCLUDE_XGXS_TSCF */

#if defined(INCLUDE_SERDES_QSGMIIE)
    {_chk_phy, _phy_id_SERDES65LP, "QSGMIIE",   &phy_qsgmiie_drv, NULL},
#endif

#if defined(INCLUDE_XGXS_VIPER)
    {_chk_viper, _phy_id_XGXS_VIPER, "VIPER", &phy_viper_drv, NULL},
#endif /* INCLUDE_XGXS_VIPER */

#if defined(INCLUDE_SERDES_QTCE)
    {_chk_phy, _phy_id_XGXS_TSC, "QTCE", &phy_qtce_drv, NULL},
#endif /* INCLUDE_XGXS_TSCF */
    {NULL}

};
#if defined (INCLUDE_PHY_56XXX) || defined (INCLUDE_SERDES_COMBO)
/*
 * Check corrupted registers by writing zeroes
 * to block address register and making sure zeroes
 * are read back.
 */
STATIC INLINE int 
_is_corrupted_reg(int unit, uint8 phy_addr)
{
    int         rv;
    uint16      data;

    rv = soc_miim_write(unit, phy_addr, 0x1f, 0);
    if (rv != SOC_E_NONE) {
        return FALSE;
    }
    rv = soc_miim_read(unit, phy_addr, 0x1f, &data);
    if (rv != SOC_E_NONE) {
        return FALSE;
    }

    return (data != 0);
}
#endif /* (INCLUDE_PHY_56XXX) || (INCLUDE_SERDES_COMBO)*/

/*
 * Function:
 *      _init_phy_table(void)
 * Purpose:
 *      Initialize the phy table with known phys.
 * Parameters:
 *      None
 */

static void
_init_phy_table(void)
{
    uint32      i;

    for (i = 0; i < COUNTOF(_standard_phy_table) && i < _MAX_PHYS; i++) {
        phy_table[i] = &_standard_phy_table[i];
    }

    _phys_in_table = i;
}

#if defined(BCM_ENDURO_SUPPORT)
static void
_enduro_phy_addr_default(int unit, int port,
                          uint16 *phy_addr, uint16 *phy_addr_int)
{
    static const uint16 _soc_enduro_int_phy_addr[] = {
        0x00, /* Port  0 (cmic)         N/A */
        0x00, /* Port  1 (N/A)                */
        0x81, /* Port  2 (8SERDES_0)    IntBus=0 Addr=0x01 */
        0x82, /* Port  3 (8SERDES_0)    IntBus=0 Addr=0x02 */
        0x83, /* Port  4 (8SERDES_0)    IntBus=0 Addr=0x03 */
        0x84, /* Port  5 (8SERDES_0)    IntBus=0 Addr=0x04 */
        0x85, /* Port  6 (8SERDES_0)    IntBus=0 Addr=0x05 */
        0x86, /* Port  7 (8SERDES_0)    IntBus=0 Addr=0x06 */
        0x87, /* Port  8 (8SERDES_0)    IntBus=0 Addr=0x07 */
        0x88, /* Port  9 (8SERDES_0)    IntBus=0 Addr=0x08 */
        0x89, /* Port 10 (8SERDES_1)    IntBus=0 Addr=0x09 */
        0x8a, /* Port 11 (8SERDES_1)    IntBus=0 Addr=0x0a */
        0x8b, /* Port 12 (8SERDES_1)    IntBus=0 Addr=0x0b */
        0x8c, /* Port 13 (8SERDES_1)    IntBus=0 Addr=0x0c */
        0x8d, /* Port 14 (8SERDES_1)    IntBus=0 Addr=0x0d */
        0x8e, /* Port 15 (8SERDES_1)    IntBus=0 Addr=0x0e */
        0x8f, /* Port 16 (8SERDES_1)    IntBus=0 Addr=0x0f */
        0x90, /* Port 17 (8SERDES_1)    IntBus=0 Addr=0x10 */
        0x91, /* Port 18 (9SERDES)      IntBus=0 Addr=0x11 */
        0x92, /* Port 19 (9SERDES)      IntBus=0 Addr=0x12 */
        0x93, /* Port 20 (9SERDES)      IntBus=0 Addr=0x13 */
        0x94, /* Port 21 (9SERDES)      IntBus=0 Addr=0x14 */
        0x95, /* Port 22 (9SERDES)      IntBus=0 Addr=0x15 */
        0x96, /* Port 23 (9SERDES)      IntBus=0 Addr=0x16 */
        0x97, /* Port 24 (9SERDES)      IntBus=0 Addr=0x17 */
        0x98, /* Port 25 (9SERDES)      IntBus=0 Addr=0x18 */
        0x99, /* Port 26 (HC0)          IntBus=0 Addr=0x19 */
        0x9a, /* Port 27 (HC1)          IntBus=0 Addr=0x1a */
        0x9b, /* Port 28 (HC2)          IntBus=0 Addr=0x1b */
        0x9c, /* Port 29 (HC3)          IntBus=0 Addr=0x1c */
    };

    static const uint16 _soc_phy_addr_bcm56334[] = {
        0x00, /* Port  0 (cmic) N/A */
        0x00, /* Port  1 ( N/A)                    */
        0x01, /* Port  2 ( ge0) ExtBus=0 Addr=0x01 */
        0x02, /* Port  3 ( ge1) ExtBus=0 Addr=0x02 */
        0x03, /* Port  4 ( ge2) ExtBus=0 Addr=0x03 */
        0x04, /* Port  5 ( ge3) ExtBus=0 Addr=0x04 */
        0x05, /* Port  6 ( ge4) ExtBus=0 Addr=0x05 */
        0x06, /* Port  7 ( ge5) ExtBus=0 Addr=0x06 */
        0x07, /* Port  8 ( ge6) ExtBus=0 Addr=0x07 */
        0x08, /* Port  9 ( ge7) ExtBus=0 Addr=0x08 */
        0x09, /* Port 10 ( ge8) ExtBus=0 Addr=0x09 */
        0x0a, /* Port 11 ( ge9) ExtBus=0 Addr=0x0a */
        0x0b, /* Port 12 (ge10) ExtBus=0 Addr=0x0b */
        0x0c, /* Port 13 (ge11) ExtBus=0 Addr=0x0c */
        0x0d, /* Port 14 (ge12) ExtBus=0 Addr=0x0d */
        0x0e, /* Port 15 (ge13) ExtBus=0 Addr=0x0e */
        0x0f, /* Port 16 (ge14) ExtBus=0 Addr=0x0f */
        0x10, /* Port 17 (ge15) ExtBus=0 Addr=0x10 */
        0x11, /* Port 18 (ge16) ExtBus=0 Addr=0x11 */
        0x12, /* Port 19 (ge17) ExtBus=0 Addr=0x12 */
        0x13, /* Port 20 (ge18) ExtBus=0 Addr=0x13 */
        0x14, /* Port 21 (ge19) ExtBus=0 Addr=0x14 */
        0x15, /* Port 22 (ge20) ExtBus=0 Addr=0x15 */
        0x16, /* Port 23 (ge21) ExtBus=0 Addr=0x16 */
        0x17, /* Port 24 (ge22) ExtBus=0 Addr=0x17 */
        0x18, /* Port 25 (ge23) ExtBus=0 Addr=0x18 */
        0x19, /* Port 26 ( hg0) ExtBus=0 Addr=0x19 */
        0x1a, /* Port 27 ( hg1) ExtBus=0 Addr=0x1a */
        0x1b, /* Port 28 ( hg2) ExtBus=0 Addr=0x1b */
        0x1c, /* Port 29 ( hg3) ExtBus=0 Addr=0x1c */
    };

    uint16 dev_id;
    uint8 rev_id;

    soc_cm_get_id(unit, &dev_id, &rev_id);

    *phy_addr_int = _soc_enduro_int_phy_addr[port];
    switch (dev_id) {
    case BCM56331_DEVICE_ID:
    case BCM56333_DEVICE_ID:
    case BCM56334_DEVICE_ID:
    case BCM56338_DEVICE_ID:
    case BCM56320_DEVICE_ID:
    case BCM56321_DEVICE_ID:
    case BCM56132_DEVICE_ID:
    case BCM56134_DEVICE_ID:
    case BCM56230_DEVICE_ID:
    case BCM56231_DEVICE_ID:
        *phy_addr = _soc_phy_addr_bcm56334[port];
        break;
    }
}
#endif /* BCM_ENDURO_SUPPORT */

#if defined(BCM_HURRICANE1_SUPPORT)
static void
_hurricane_phy_addr_default(int unit, int port,
                          uint16 *phy_addr, uint16 *phy_addr_int)
{
    static const uint16 _soc_hurricane_int_phy_addr[] = {
        0x00, /* Port  0 (cmic)         N/A */
        0x00, /* Port  1 (N/A)                */
        0x81, /* Port  2 (8SERDES_0)    IntBus=0 Addr=0x01 */
        0x81, /* Port  3 (8SERDES_0)    IntBus=0 Addr=0x02 */
        0x81, /* Port  4 (8SERDES_0)    IntBus=0 Addr=0x03 */
        0x81, /* Port  5 (8SERDES_0)    IntBus=0 Addr=0x04 */
        0x81, /* Port  6 (8SERDES_0)    IntBus=0 Addr=0x05 */
        0x81, /* Port  7 (8SERDES_0)    IntBus=0 Addr=0x06 */
        0x81, /* Port  8 (8SERDES_0)    IntBus=0 Addr=0x07 */
        0x81, /* Port  9 (8SERDES_0)    IntBus=0 Addr=0x08 */
        0x89, /* Port 10 (8SERDES_1)    IntBus=0 Addr=0x09 */
        0x89, /* Port 11 (8SERDES_1)    IntBus=0 Addr=0x0a */
        0x89, /* Port 12 (8SERDES_1)    IntBus=0 Addr=0x0b */
        0x89, /* Port 13 (8SERDES_1)    IntBus=0 Addr=0x0c */
        0x89, /* Port 14 (8SERDES_1)    IntBus=0 Addr=0x0d */
        0x89, /* Port 15 (8SERDES_1)    IntBus=0 Addr=0x0e */
        0x89, /* Port 16 (8SERDES_1)    IntBus=0 Addr=0x0f */
        0x89, /* Port 17 (8SERDES_1)    IntBus=0 Addr=0x10 */
        0x91, /* Port 18 (9SERDES)      IntBus=0 Addr=0x11 */
        0x91, /* Port 19 (9SERDES)      IntBus=0 Addr=0x12 */
        0x91, /* Port 20 (9SERDES)      IntBus=0 Addr=0x13 */
        0x91, /* Port 21 (9SERDES)      IntBus=0 Addr=0x14 */
        0x91, /* Port 22 (9SERDES)      IntBus=0 Addr=0x15 */
        0x91, /* Port 23 (9SERDES)      IntBus=0 Addr=0x16 */
        0x91, /* Port 24 (9SERDES)      IntBus=0 Addr=0x17 */
        0x91, /* Port 25 (9SERDES)      IntBus=0 Addr=0x18 */
        0x99, /* Port 26 (HC0)          IntBus=0 Addr=0x19 */
        0x99, /* Port 27 (HC0)          IntBus=0 Addr=0x19 */
        0x9a, /* Port 28 (HC1)          IntBus=0 Addr=0x1a */
        0x9a, /* Port 29 (HC1)          IntBus=0 Addr=0x1a */
    };

    static const uint16 _soc_phy_addr_bcm56142[] = {
        0x00, /* Port  0 (cmic) N/A */
        0x00, /* Port  1 ( N/A)                    */
        0x01, /* Port  2 ( ge0) ExtBus=0 Addr=0x01 */
        0x02, /* Port  3 ( ge1) ExtBus=0 Addr=0x02 */
        0x03, /* Port  4 ( ge2) ExtBus=0 Addr=0x03 */
        0x04, /* Port  5 ( ge3) ExtBus=0 Addr=0x04 */
        0x05, /* Port  6 ( ge4) ExtBus=0 Addr=0x05 */
        0x06, /* Port  7 ( ge5) ExtBus=0 Addr=0x06 */
        0x07, /* Port  8 ( ge6) ExtBus=0 Addr=0x07 */
        0x08, /* Port  9 ( ge7) ExtBus=0 Addr=0x08 */
        0x0a, /* Port 10 ( ge8) ExtBus=0 Addr=0x0a */
        0x0b, /* Port 11 ( ge9) ExtBus=0 Addr=0x0b */
        0x0c, /* Port 12 (ge10) ExtBus=0 Addr=0x0c */
        0x0d, /* Port 13 (ge11) ExtBus=0 Addr=0x0d */
        0x0e, /* Port 14 (ge12) ExtBus=0 Addr=0x0e */
        0x0f, /* Port 15 (ge13) ExtBus=0 Addr=0x0f */
        0x10, /* Port 16 (ge14) ExtBus=0 Addr=0x10 */
        0x11, /* Port 17 (ge15) ExtBus=0 Addr=0x11 */
        0x13, /* Port 18 (ge16) ExtBus=0 Addr=0x13 */
        0x14, /* Port 19 (ge17) ExtBus=0 Addr=0x14 */
        0x15, /* Port 20 (ge18) ExtBus=0 Addr=0x15 */
        0x16, /* Port 21 (ge19) ExtBus=0 Addr=0x16 */
        0x17, /* Port 22 (ge20) ExtBus=0 Addr=0x17 */
        0x18, /* Port 23 (ge21) ExtBus=0 Addr=0x18 */
        0x19, /* Port 24 (ge22) ExtBus=0 Addr=0x19 */
        0x1a, /* Port 25 (ge23) ExtBus=0 Addr=0x1a */
        0x1c, /* Port 26 ( hg0) ExtBus=0/1 Addr=0x1c */
        0,    /* Port 27 ( hg1) */
        0x1d, /* Port 28 ( hg2) ExtBus=0/1 Addr=0x1d */
        0,    /* Port 29 ( hg3) */
    };

    uint16 dev_id;
    uint8 rev_id;

    soc_cm_get_id(unit, &dev_id, &rev_id);

    *phy_addr_int = _soc_hurricane_int_phy_addr[port];
    switch (dev_id) {
    case BCM56140_DEVICE_ID:
    case BCM56142_DEVICE_ID:
    case BCM56143_DEVICE_ID:
    case BCM56144_DEVICE_ID:
    case BCM56146_DEVICE_ID:
    case BCM56147_DEVICE_ID:
    case BCM56149_DEVICE_ID:
        *phy_addr = _soc_phy_addr_bcm56142[port];
        break;
    }
    
}
#endif /* BCM_HURRICANE_SUPPORT */

#if defined(BCM_HURRICANE2_SUPPORT)
static void
_hurricane2_phy_addr_default(int unit, int port,
                          uint16 *phy_addr, uint16 *phy_addr_int)
{
    static const uint16 _soc_hurricane2_int_phy_addr[] = {
        0x00, /* Port  0 (cmic)         N/A */
        0x00, /* Port  1 (N/A)                */
        0x81, /* Port  2 (QSGMII2X_0)    IntBus=0 Addr=0x01 */
        0x81, /* Port  3 (QSGMII2X_0)    IntBus=0 Addr=0x02 */
        0x81, /* Port  4 (QSGMII2X_0)    IntBus=0 Addr=0x03 */
        0x81, /* Port  5 (QSGMII2X_0)    IntBus=0 Addr=0x04 */
        0x81, /* Port  6 (QSGMII2X_0)    IntBus=0 Addr=0x05 */
        0x81, /* Port  7 (QSGMII2X_0)    IntBus=0 Addr=0x06 */
        0x81, /* Port  8 (QSGMII2X_0)    IntBus=0 Addr=0x07 */
        0x81, /* Port  9 (QSGMII2X_0)    IntBus=0 Addr=0x08 */
        0x89, /* Port 10 (QSGMII2X_1)    IntBus=0 Addr=0x09 */
        0x89, /* Port 11 (QSGMII2X_1)    IntBus=0 Addr=0x0a */
        0x89, /* Port 12 (QSGMII2X_1)    IntBus=0 Addr=0x0b */
        0x89, /* Port 13 (QSGMII2X_1)    IntBus=0 Addr=0x0c */
        0x89, /* Port 14 (QSGMII2X_1)    IntBus=0 Addr=0x0d */
        0x89, /* Port 15 (QSGMII2X_1)    IntBus=0 Addr=0x0e */
        0x89, /* Port 16 (QSGMII2X_1)    IntBus=0 Addr=0x0f */
        0x89, /* Port 17 (QSGMII2X_1)    IntBus=0 Addr=0x10 */
        0xb1, /* Port 18 (QSGMII2X_2)    IntBus=1 Addr=0x11 */
        0xb1, /* Port 19 (QSGMII2X_2)    IntBus=1 Addr=0x12 */
        0xb1, /* Port 20 (QSGMII2X_2)    IntBus=1 Addr=0x13 */
        0xb1, /* Port 21 (QSGMII2X_2)    IntBus=1 Addr=0x14 */
        0xb1, /* Port 22 (QSGMII2X_2)    IntBus=1 Addr=0x15 */
        0xb1, /* Port 23 (QSGMII2X_2)    IntBus=1 Addr=0x16 */
        0xb1, /* Port 24 (QSGMII2X_2)    IntBus=1 Addr=0x17 */
        0xb1, /* Port 25 (QSGMII2X_2)    IntBus=1 Addr=0x18 */
        0xa1, /* Port 26 (TSC4_0)        IntBus=1 Addr=0x01 */
        0xa2, /* Port 27 (TSC4_0)        IntBus=1 Addr=0x02 */
        0xa3, /* Port 28 (TSC4_0)        IntBus=1 Addr=0x03 */
        0xa4, /* Port 29 (TSC4_0)        IntBus=1 Addr=0x04 */
        0xa5, /* Port 30 (TSC4_1)        IntBus=1 Addr=0x05 */
        0xa6, /* Port 31 (TSC4_1)        IntBus=1 Addr=0x06 */
        0xa7, /* Port 32 (TSC4_1)        IntBus=1 Addr=0x09 */
        0xa8  /* Port 33 (TSC4_1)        IntBus=1 Addr=0x08 */
    };

    static const uint16 _soc_phy_addr_bcm56150[] = {
        0x00, /* Port  0 (cmic) N/A */
        0x00, /* Port  1 ( N/A)                    */
        0x02, /* Port  2 ( ge0) ExtBus=0 Addr=0x02 */
        0x03, /* Port  3 ( ge1) ExtBus=0 Addr=0x03 */
        0x04, /* Port  4 ( ge2) ExtBus=0 Addr=0x04 */
        0x05, /* Port  5 ( ge3) ExtBus=0 Addr=0x05 */
        0x06, /* Port  6 ( ge4) ExtBus=0 Addr=0x06 */
        0x07, /* Port  7 ( ge5) ExtBus=0 Addr=0x07 */
        0x08, /* Port  8 ( ge6) ExtBus=0 Addr=0x08 */
        0x09, /* Port  9 ( ge7) ExtBus=0 Addr=0x09 */
        0x0b, /* Port 10 ( ge8) ExtBus=0 Addr=0x0b */
        0x0c, /* Port 11 ( ge9) ExtBus=0 Addr=0x0c */
        0x0d, /* Port 12 (ge10) ExtBus=0 Addr=0x0d */
        0x0e, /* Port 13 (ge11) ExtBus=0 Addr=0x0e */
        0x0f, /* Port 14 (ge12) ExtBus=0 Addr=0x0f */
        0x10, /* Port 15 (ge13) ExtBus=0 Addr=0x10 */
        0x11, /* Port 16 (ge14) ExtBus=0 Addr=0x11 */
        0x12, /* Port 17 (ge15) ExtBus=0 Addr=0x12 */
        0x14, /* Port 18 (ge16) ExtBus=0 Addr=0x14 */
        0x15, /* Port 19 (ge17) ExtBus=0 Addr=0x15 */
        0x16, /* Port 20 (ge18) ExtBus=0 Addr=0x16 */
        0x17, /* Port 21 (ge19) ExtBus=0 Addr=0x17 */
        0x18, /* Port 22 (ge20) ExtBus=0 Addr=0x18 */
        0x19, /* Port 23 (ge21) ExtBus=0 Addr=0x19 */
        0x1a, /* Port 24 (ge22) ExtBus=0 Addr=0x1a */
        0x1b, /* Port 25 (ge23) ExtBus=0 Addr=0x1b */
        0x21, /* Port 26 ( hg0) ExtBus=1 Addr=0x01 */
        0x22, /* Port 27 ( hg1) ExtBus=1 Addr=0x02 */
        0x23, /* Port 28 ( hg2) ExtBus=1 Addr=0x03 */
        0x24, /* Port 29 ( hg3) ExtBus=1 Addr=0x04 */
        0x25, /* Port 30 ( hg4) ExtBus=1 Addr=0x05 */
        0x26, /* Port 31 ( hg5) ExtBus=1 Addr=0x06 */
        0x27, /* Port 32 ( hg6) ExtBus=1 Addr=0x07 */
        0x28  /* Port 33 ( hg7) ExtBus=1 Addr=0x08 */
    };

    static const uint16 _soc_phy_addr_bcm5339x[] = {
        0x00, /* Port  0 (cmic) N/A */
        0x00, /* Port  1 ( N/A)                    */
        0x02, /* Port  2 ( ge0) ExtBus=0 Addr=0x02 */
        0x00, /* Port  3 ( N/A) */
        0x00, /* Port  4 ( N/A) */
        0x00, /* Port  5 ( N/A) */
        0x03, /* Port  6 ( ge1) ExtBus=0 Addr=0x03 */
        0x00, /* Port  7 ( N/A) */
        0x00, /* Port  8 ( N/A) */
        0x00, /* Port  9 ( N/A) */
        0x04, /* Port 10 ( ge2) ExtBus=0 Addr=0x04 */
        0x00, /* Port 11 ( N/A) */
        0x00, /* Port 12 ( N/A) */
        0x00, /* Port 13 ( N/A) */
        0x05, /* Port 14 ( ge3) ExtBus=0 Addr=0x05 */
        0x00, /* Port 15 ( N/A) */
        0x00, /* Port 16 ( N/A) */
        0x00, /* Port 17 ( N/A) */
        0x06, /* Port 18 ( ge4) ExtBus=0 Addr=0x06 */
        0x00, /* Port 19 ( N/A) */
        0x00, /* Port 20 ( N/A) */
        0x00, /* Port 21 ( N/A) */
        0x07, /* Port 22 ( ge5) ExtBus=0 Addr=0x07 */
        0x00, /* Port 23 ( N/A) */
        0x00, /* Port 24 ( N/A) */
        0x00, /* Port 25 ( N/A) */
        0x00, /* Port 26 ( N/A) */
        0x00, /* Port 27 ( N/A) */
        0x00, /* Port 28 ( N/A) */
        0x00, /* Port 29 ( N/A) */
        0x00, /* Port 30 ( N/A) */
        0x00, /* Port 31 ( N/A) */
        0x00, /* Port 32 ( N/A) */
        0x00  /* Port 33 ( N/A) */
    };

    static const uint16 _soc_hurricane2_gphy_addr[] = {
        0x00, /* Port  0 (cmic)         N/A */
        0x00, /* Port  1 (N/A)                */
        0x81, /* Port  2 (QGPHY_0)    IntBus=0 Addr=0x01 */
        0x82, /* Port  3 (QGPHY_0)    IntBus=0 Addr=0x02 */
        0x83, /* Port  4 (QGPHY_0)    IntBus=0 Addr=0x03 */
        0x84, /* Port  5 (QGPHY_0)    IntBus=0 Addr=0x04 */
        0x86, /* Port  6 (QGPHY_1)    IntBus=0 Addr=0x06 */
        0x87, /* Port  7 (QGPHY_1)    IntBus=0 Addr=0x07 */
        0x88, /* Port  8 (QGPHY_1)    IntBus=0 Addr=0x08 */
        0x89, /* Port  9 (QGPHY_1)    IntBus=0 Addr=0x09 */
        0x8b, /* Port 10 (QGPHY_2)    IntBus=0 Addr=0x0b */
        0x8c, /* Port 11 (QGPHY_2)    IntBus=0 Addr=0x0c */
        0x8d, /* Port 12 (QGPHY_2)    IntBus=0 Addr=0x0d */
        0x8e, /* Port 13 (QGPHY_2)    IntBus=0 Addr=0x0e */
        0x90, /* Port 14 (QGPHY_3)    IntBus=0 Addr=0x10 */
        0x91, /* Port 15 (QGPHY_3)    IntBus=0 Addr=0x11 */
        0x92, /* Port 16 (QGPHY_3)    IntBus=0 Addr=0x12 */
        0x93  /* Port 17 (QGPHY_3)    IntBus=0 Addr=0x13 */
    };

    uint16 dev_id;
    uint8 rev_id;
    int phy_port;

    soc_cm_get_id(unit, &dev_id, &rev_id);

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    *phy_addr_int = _soc_hurricane2_int_phy_addr[phy_port];
    switch (dev_id) {
        case BCM56150_DEVICE_ID:
        case BCM56151_DEVICE_ID:
        case BCM56152_DEVICE_ID:
        case BCM53342_DEVICE_ID:
        case BCM53343_DEVICE_ID:
        case BCM53344_DEVICE_ID:
        case BCM53346_DEVICE_ID:
        case BCM53347_DEVICE_ID:
        case BCM53333_DEVICE_ID:
        case BCM53334_DEVICE_ID:
        *phy_addr = _soc_phy_addr_bcm56150[phy_port];
        break;
        case BCM53393_DEVICE_ID:
        case BCM53394_DEVICE_ID:
        *phy_addr = _soc_phy_addr_bcm5339x[phy_port];
        break;
    }
    if ( soc_feature(unit, soc_feature_gphy) && (phy_port < 18)) {
        *phy_addr_int = 0;
        *phy_addr = _soc_hurricane2_gphy_addr[phy_port];
    }
}
#endif /* BCM_HURRICANE2_SUPPORT */

#if defined(BCM_GREYHOUND_SUPPORT)
/* GH's SKUs for TSC0Q at QSGMII mode
 *  - this mode supports 16 GE ports through SGMII.
 */
#define _GH_PPORT_IN_QSGMII_INT_PHY_RANGE(_pport)  \
        ((((_pport) >= 2) && (_pport) <= 17) ? TRUE : FALSE)

static void
_greyhound_phy_addr_default(int unit, int port,
        uint16 *phy_addr, uint16 *phy_addr_int) {

    /* MDIO address mapping for TSC0Q at QSGMII mode
     *  - in SKUs of 56064/56063/56062/53411/53401/53456/53457/53424
     *  
     * Note : 
     *  1. GH's QSGMII SerDes in PM4x10Q has no direct MDIO access interface.
     *      - This MDIO address mapping is for SW usage only.
     */
    static const uint16 _soc_gh_int_phyaddr_qsgmii[] = {
        0x00, /* Port  0 (cmic)         N/A */
        0x00, /* Port  1 (N/A)              */
        GH_QSGMII_MDIO_ADDR, /* Port  2 (QSGMII_0) IntBus=0 Addr=0x01 */
        GH_QSGMII_MDIO_ADDR, /* Port  3 (QSGMII_0) Share the same Addr */
        GH_QSGMII_MDIO_ADDR, /* Port  4 (QSGMII_0) Share the same Addr */
        GH_QSGMII_MDIO_ADDR, /* Port  5 (QSGMII_0) Share the same Addr */
        GH_QSGMII_MDIO_ADDR, /* Port  6 (QSGMII_0) Share the same Addr */
        GH_QSGMII_MDIO_ADDR, /* Port  7 (QSGMII_0) Share the same Addr */
        GH_QSGMII_MDIO_ADDR, /* Port  8 (QSGMII_0) Share the same Addr */
        GH_QSGMII_MDIO_ADDR, /* Port  9 (QSGMII_0) Share the same Addr */
        GH_QSGMII_MDIO_ADDR, /* Port 10 (QSGMII_1) Share the same Addr */
        GH_QSGMII_MDIO_ADDR, /* Port 11 (QSGMII_1) Share the same Addr */
        GH_QSGMII_MDIO_ADDR, /* Port 12 (QSGMII_1) Share the same Addr */
        GH_QSGMII_MDIO_ADDR, /* Port 13 (QSGMII_1) Share the same Addr */
        GH_QSGMII_MDIO_ADDR, /* Port 14 (QSGMII_1) Share the same Addr */
        GH_QSGMII_MDIO_ADDR, /* Port 15 (QSGMII_1) Share the same Addr */
        GH_QSGMII_MDIO_ADDR, /* Port 16 (QSGMII_1) Share the same Addr */
        GH_QSGMII_MDIO_ADDR  /* Port 17 (QSGMII_1) Share the same Addr */
    };

    /* Greyhound TOP defined MDIO address for TSCs */
    static const uint16 _soc_gh_int_phyaddr[] = {
        0x00, /* Port  0 (cmic)         N/A */
        0x00, /* Port  1 (N/A)              */
        0x81, /* Port  2 (TSC0Q_0)      IntBus=0 Addr=0x01 */
        0x81, /* Port  3 (TSC0Q_1)      Share the same Addr */
        0x81, /* Port  4 (TSC0Q_2)      Share the same Addr */
        0x81, /* Port  5 (TSC0Q_3)      Share the same Addr */
        0x00, /* Port  6 (N/A)              */
        0x00, /* Port  7 (N/A)              */
        0x00, /* Port  8 (N/A)              */
        0x00, /* Port  9 (N/A)              */
        0x00, /* Port 10 (N/A)              */
        0x00, /* Port 11 (N/A)              */
        0x00, /* Port 12 (N/A)              */
        0x00, /* Port 13 (N/A)              */
        0x00, /* Port 14 (N/A)              */
        0x00, /* Port 15 (N/A)              */
        0x00, /* Port 16 (N/A)              */
        0x00, /* Port 17 (N/A)              */
        0xa1, /* Port 18 (TSC0_0)       IntBus=1 Addr=0x01 */
        0xa1, /* Port 19 (TSC0_1)       Share the same Addr */
        0xa1, /* Port 20 (TSC0_2)       Share the same Addr */
        0xa1, /* Port 21 (TSC0_3)       Share the same Addr */
        0xa5, /* Port 22 (TSC1_0)       IntBus=1 Addr=0x05 */
        0xa5, /* Port 23 (TSC1_1)       Share the same Addr */
        0xa5, /* Port 24 (TSC1_2)       Share the same Addr */
        0xa5, /* Port 25 (TSC1_3)       Share the same Addr */
        0xa9, /* Port 26 (TSC2_0)       IntBus=1 Addr=0x09 */
        0xa9, /* Port 27 (TSC2_1)       Share the same Addr */
        0xa9, /* Port 28 (TSC2_2)       Share the same Addr */
        0xa9, /* Port 29 (TSC2_3)       Share the same Addr */
        0xad, /* Port 30 (TSC3_0)       IntBus=1 Addr=0x0d */
        0xad, /* Port 31 (TSC3_1)       Share the same Addr */
        0xad, /* Port 32 (TSC3_2)       Share the same Addr */
        0xad, /* Port 33 (TSC3_3)       Share the same Addr */
        0xb1, /* Port 34 (TSC4_0)       IntBus=1 Addr=0x11 */
        0xb1, /* Port 35 (TSC4_1)       Share the same Addr */
        0xb1, /* Port 36 (TSC4_2)       Share the same Addr */
        0xb1  /* Port 37 (TSC4_3)       Share the same Addr */
    };

    uint16 dev_id;
    uint8 rev_id;
    int phy_port;

    /* assigning default phy_addr for ext-phy */
    *phy_addr = 0xff;
    soc_cm_get_id(unit, &dev_id, &rev_id);
    phy_port = SOC_INFO(unit).port_l2p_mapping[port];

    /* assigning defualt internal PHY address */
    switch (dev_id) {
        case BCM56062_DEVICE_ID:
        case BCM56063_DEVICE_ID:
        case BCM56064_DEVICE_ID:
        case BCM53411_DEVICE_ID:
        case BCM53401_DEVICE_ID:
        case BCM53456_DEVICE_ID:
        case BCM53457_DEVICE_ID:
        case BCM53424_DEVICE_ID:
            if (_GH_PPORT_IN_QSGMII_INT_PHY_RANGE(phy_port)) {
                *phy_addr_int = _soc_gh_int_phyaddr_qsgmii[phy_port]; 
            } else {
                *phy_addr_int = _soc_gh_int_phyaddr[phy_port]; 
            }
            break;
        default : 
            *phy_addr_int = _soc_gh_int_phyaddr[phy_port]; 
            break;
    }
}
#endif /* BCM_GREYHOUND_SUPPORT */

#if defined(BCM_HURRICANE3_SUPPORT)
static void
_wolfhound2_phy_addr_default(int unit, int port,
                             uint16 *phy_addr, uint16 *phy_addr_int)
{
    static const uint16 _soc_wh2_phy_addr[] = {
        0x00, /* Port  0 (cmic)         N/A */
        0x00, /* Port  1 (N/A)              */
        0x81, /* Port  2            IntBus=0 Addr=0x1 */
        0x82, /* Port  3            IntBus=0 Addr=0x2 */
        0x83, /* Port  4            IntBus=0 Addr=0x3 */
        0x84, /* Port  5            IntBus=0 Addr=0x4 */
        0x85, /* Port  6            IntBus=0 Addr=0x5 */
        0x86, /* Port  7            IntBus=0 Addr=0x6 */
        0x87, /* Port  8            IntBus=0 Addr=0x7 */
        0x88, /* Port  9            IntBus=0 Addr=0x8 */
        0x89, /* Port 10            IntBus=0 Addr=0x9 */
        0x8a, /* Port 11            IntBus=0 Addr=0xa */
        0x8b, /* Port 12            IntBus=0 Addr=0xb */
        0x8c, /* Port 13            IntBus=0 Addr=0xc */
        0x00, /* Port 14 */
        0x00, /* Port 15 */
        0x00, /* Port 16 */
        0x00, /* Port 17 */
        0x8d, /* Port 18            IntBus=0 Addr=0xd  */
        0x8e, /* Port 19            IntBus=0 Addr=0xe  */
        0x8f, /* Port 20            IntBus=0 Addr=0xf  */
        0x90, /* Port 21            IntBus=0 Addr=0x10 */
        0x91, /* Port 22            IntBus=0 Addr=0x11 */
        0x92, /* Port 23            IntBus=0 Addr=0x12 */
        0x93, /* Port 24            IntBus=0 Addr=0x13 */
        0x94, /* Port 25            IntBus=0 Addr=0x14 */
        0x95, /* Port 26            IntBus=0 Addr=0x15 */
        0x96, /* Port 27            IntBus=0 Addr=0x16 */
        0x97, /* Port 28            IntBus=0 Addr=0x17 */
        0x98, /* Port 29            IntBus=0 Addr=0x18 */
        0x00, /* Port 30 */
        0x00, /* Port 31 */
        0x00, /* Port 32 */
        0x00, /* Port 33 */
        0x00, /* Port 34 */
        0x00, /* Port 35 */
        0x00, /* Port 36 */
        0x00  /* Port 37 */
    };

    static const uint16 _soc_wh2_int_phy_addr[] = {
        0x00, /* Port  0 (cmic)         N/A */
        0x00, /* Port  1 (N/A)              */
        0x00, /* Port  2 */
        0x00, /* Port  3 */
        0x00, /* Port  4 */
        0x00, /* Port  5 */
        0x00, /* Port  6 */
        0x00, /* Port  7 */
        0x00, /* Port  8 */
        0x00, /* Port  9 */
        0x00, /* Port 10 */
        0x00, /* Port 11 */
        0x00, /* Port 12 */
        0x00, /* Port 13 */
        0x00, /* Port 14 */
        0x00, /* Port 15 */
        0x00, /* Port 16 */
        0x00, /* Port 17 */
        0x00, /* Port 18 */
        0x00, /* Port 19 */
        0x00, /* Port 20 */
        0x00, /* Port 21 */
        0x00, /* Port 22 */
        0x00, /* Port 23 */
        0x00, /* Port 24 */
        0x00, /* Port 25 */
        0xb4, /* Port 26            IntBus=1 Addr=0x14 */
        0xb4, /* Port 27            IntBus=1 Addr=0x15 */
        0xb4, /* Port 28            IntBus=1 Addr=0x16 */
        0xb4, /* Port 29            IntBus=1 Addr=0x17 */
        0x00, /* Port 30 */
        0x00, /* Port 31 */
        0x00, /* Port 32 */
        0x00, /* Port 33 */
        0xb0, /* Port 34            IntBus=1 Addr=0x10 */
        0xb0, /* Port 35            IntBus=1 Addr=0x11 */
        0xb0, /* Port 36            IntBus=1 Addr=0x12 */
        0xb0  /* Port 37            IntBus=1 Addr=0x13 */
    };
    int phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    uint8 is_gphy;
    if (phy_port >= 34) {
        /* SGMII_4P1 ports */
        *phy_addr_int = _soc_wh2_int_phy_addr[phy_port];
        *phy_addr = 0xff;
    } else {
        soc_wolfhound2_gphy_get(unit, port, &is_gphy);
        if (!is_gphy) {
            *phy_addr_int = _soc_wh2_int_phy_addr[phy_port];
            *phy_addr = 0xff;
        } else {
            *phy_addr_int = 0;
            *phy_addr = _soc_wh2_phy_addr[phy_port];
        }
    }
}

#define _HR3_GPHY_CNT  (8)
#define _HR3_GPHY_FIRST_PORT_NUM_PGW0 (10)
#define _HR3_GPHY_FIRST_PORT_NUM_PGW1 (26)
static void
_hurricane3_phy_addr_default(int unit, int port,
                          uint16 *phy_addr, uint16 *phy_addr_int)
{
    static const uint16 _soc_hr3_int_phy_addr[] = {
        0x00, /* Port  0 (cmic)         N/A */
        0x00, /* Port  1 (N/A)              */
        0xa9, /* Port  2 (QTC_0)    IntBus=1 Addr=0x09 */
        0xa9, /* Port  3 (QTC_0)    IntBus=1 Addr=0x09 */
        0xa9, /* Port  4 (QTC_0)    IntBus=1 Addr=0x09 */
        0xa9, /* Port  5 (QTC_0)    IntBus=1 Addr=0x09 */
        0xa9, /* Port  6 (QTC_0)    IntBus=1 Addr=0x09 */
        0xa9, /* Port  7 (QTC_0)    IntBus=1 Addr=0x09 */
        0xa9, /* Port  8 (QTC_0)    IntBus=1 Addr=0x09 */
        0xa9, /* Port  9 (QTC_0)    IntBus=1 Addr=0x09 */
        0xa9, /* Port 10 (QTC_0)    IntBus=1 Addr=0x09 */
        0xa9, /* Port 11 (QTC_0)    IntBus=1 Addr=0x09 */
        0xa9, /* Port 12 (QTC_0)    IntBus=1 Addr=0x09 */
        0xa9, /* Port 13 (QTC_0)    IntBus=1 Addr=0x09 */
        0xa9, /* Port 14 (QTC_0)    IntBus=1 Addr=0x09 */
        0xa9, /* Port 15 (QTC_0)    IntBus=1 Addr=0x09 */
        0xa9, /* Port 16 (QTC_0)    IntBus=1 Addr=0x09 */
        0xa9, /* Port 17 (QTC_0)    IntBus=1 Addr=0x09 */
        0xad, /* Port 18 (QTC_1)    IntBus=1 Addr=0x0d */
        0xad, /* Port 19 (QTC_1)    IntBus=1 Addr=0x0d */
        0xad, /* Port 20 (QTC_1)    IntBus=1 Addr=0x0d */
        0xad, /* Port 21 (QTC_1)    IntBus=1 Addr=0x0d */
        0xad, /* Port 22 (QTC_1)    IntBus=1 Addr=0x0d */
        0xad, /* Port 23 (QTC_1)    IntBus=1 Addr=0x0d */
        0xad, /* Port 24 (QTC_1)    IntBus=1 Addr=0x0d */
        0xad, /* Port 25 (QTC_1)    IntBus=1 Addr=0x0d */
        0xad, /* Port 26 (QTC_1)    IntBus=1 Addr=0x0d */
        0xad, /* Port 27 (QTC_1)    IntBus=1 Addr=0x0d */
        0xad, /* Port 28 (QTC_1)    IntBus=1 Addr=0x0d */
        0xad, /* Port 29 (QTC_1)    IntBus=1 Addr=0x0d */
        0xad, /* Port 30 (QTC_1)    IntBus=1 Addr=0x0d */
        0xad, /* Port 31 (QTC_1)    IntBus=1 Addr=0x0d */
        0xad, /* Port 32 (QTC_1)    IntBus=1 Addr=0x0d */
        0xad, /* Port 33 (QTC_1)    IntBus=1 Addr=0x0d */
        0xa1, /* Port 34 (TSC_0)    IntBus=1 Addr=0x01 */
        0xa1, /* Port 35 (TSC_0)    IntBus=1 Addr=0x01 */
        0xa1, /* Port 36 (TSC_0)    IntBus=1 Addr=0x01 */
        0xa1, /* Port 37 (TSC_0)    IntBus=1 Addr=0x01 */
        0xa5, /* Port 38 (TSC_1)    IntBus=1 Addr=0x05 */
        0xa5, /* Port 39 (TSC_1)    IntBus=1 Addr=0x05 */
        0xa5, /* Port 40 (TSC_1)    IntBus=1 Addr=0x05 */
        0xa5  /* Port 41 (TSC_1)    IntBus=1 Addr=0x05 */
    };

    static const uint16 _soc_hr3_gphy_addr[] = {
        0x00, /* Port  0 (cmic)       N/A */
        0x00, /* Port  1 (N/A)            */
        0x00, /* Port  2 (N/A)        N/A */
        0x00, /* Port  3 (N/A)        N/A */
        0x00, /* Port  4 (N/A)        N/A */
        0x00, /* Port  5 (N/A)        N/A */
        0x00, /* Port  6 (N/A)        N/A */
        0x00, /* Port  7 (N/A)        N/A */
        0x00, /* Port  8 (N/A)        N/A */
        0x00, /* Port  9 (N/A)        N/A */
        0x81, /* Port 10 (QGPHY_0)    IntBus=0 Addr=0x01 */
        0x82, /* Port 11 (QGPHY_0)    IntBus=0 Addr=0x02 */
        0x83, /* Port 12 (QGPHY_0)    IntBus=0 Addr=0x03 */
        0x84, /* Port 13 (QGPHY_0)    IntBus=0 Addr=0x04 */
        0x85, /* Port 14 (QGPHY_1)    IntBus=0 Addr=0x05 */
        0x86, /* Port 15 (QGPHY_1)    IntBus=0 Addr=0x06 */
        0x87, /* Port 16 (QGPHY_1)    IntBus=0 Addr=0x07 */
        0x88, /* Port 17 (QGPHY_1)    IntBus=0 Addr=0x08 */
        0x00, /* Port 18 (N/A)        N/A */
        0x00, /* Port 19 (N/A)        N/A */
        0x00, /* Port 20 (N/A)        N/A */
        0x00, /* Port 21 (N/A)        N/A */
        0x00, /* Port 22 (N/A)        N/A */
        0x00, /* Port 23 (N/A)        N/A */
        0x00, /* Port 24 (N/A)        N/A */
        0x00, /* Port 25 (N/A)        N/A */
        0x89, /* Port 26 (QGPHY_2)    IntBus=0 Addr=0x09 */
        0x8a, /* Port 27 (QGPHY_2)    IntBus=0 Addr=0x0a */
        0x8b, /* Port 28 (QGPHY_2)    IntBus=0 Addr=0x0b */
        0x8c, /* Port 29 (QGPHY_2)    IntBus=0 Addr=0x0c */
        0x8d, /* Port 30 (QGPHY_3)    IntBus=0 Addr=0x0d */
        0x8e, /* Port 31 (QGPHY_3)    IntBus=0 Addr=0x0e */
        0x8f, /* Port 32 (QGPHY_3)    IntBus=0 Addr=0x0f */
        0x90, /* Port 33 (QGPHY_3)    IntBus=0 Addr=0x10 */
        0x00, /* Port 34 (N/A)        N/A */
        0x00, /* Port 35 (N/A)        N/A */
        0x00, /* Port 36 (N/A)        N/A */
        0x00, /* Port 37 (N/A)        N/A */
        0x00, /* Port 38 (N/A)        N/A */
        0x00, /* Port 39 (N/A)        N/A */
        0x00, /* Port 40 (N/A)        N/A */
        0x00  /* Port 41 (N/A)        N/A */
    };

    int phy_port = SOC_INFO(unit).port_l2p_mapping[port];

    /* assigning default phy_addr for ext-phy */
    *phy_addr = 0xff;

    *phy_addr_int = _soc_hr3_int_phy_addr[phy_port];

    /* assigning GPHY address */
    if (soc_feature(unit, soc_feature_gphy)) {
        if (((phy_port >= _HR3_GPHY_FIRST_PORT_NUM_PGW0) &&
            (phy_port < _HR3_GPHY_FIRST_PORT_NUM_PGW0 + _HR3_GPHY_CNT)) ||
            ((phy_port >= _HR3_GPHY_FIRST_PORT_NUM_PGW1) &&
            (phy_port < _HR3_GPHY_FIRST_PORT_NUM_PGW1 + _HR3_GPHY_CNT))) {
            *phy_addr = _soc_hr3_gphy_addr[phy_port];
            *phy_addr_int = 0;
        }
    }
}
#endif /* BCM_HURRICANE3_SUPPORT */

#if defined(BCM_GREYHOUND2_SUPPORT)
/*
 * Func : _greyhound2_phy_addr_default()
 *  - to report default Int/Ext PHY_ADDR.
 *
 * Note :
 *  - Major for default PHY address on all Int-PHY (for PM/SerDes)
 *  - Those definitions are defined in GH2's TOP Spec.
 */
static void
_greyhound2_phy_addr_default(int unit, int port,
                          uint16 *phy_addr, uint16 *phy_addr_int)
{
    static const uint16 _soc_gh2_int_phy_addr[] = {
        0x00, /* Port  0 (cmic)         N/A */
        0x00, /* Port  1 (N/A)              */
        0x81, /* Port  2 (SGMII4P_0)        IntBus=0 Addr=0x01 */
        0x81, /* Port  3 (SGMII4P_0)        Share the same Addr */
        0x81, /* Port  4 (SGMII4P_0)        Share the same Addr */
        0x81, /* Port  5 (SGMII4P_0)        Share the same Addr */
        0x85, /* Port  6 (SGMII4P_1)        Share the same Addr */
        0x85, /* Port  7 (SGMII4P_1)        Share the same Addr */
        0x85, /* Port  8 (SGMII4P_1)        Share the same Addr */
        0x85, /* Port  9 (SGMII4P_1)        Share the same Addr */
        0x89, /* Port 10 (SGMII4P_2)        IntBus=0 Addr=0x09 */
        0x89, /* Port 11 (SGMII4P_2)        Share the same Addr */
        0x89, /* Port 12 (SGMII4P_2)        Share the same Addr */
        0x89, /* Port 13 (SGMII4P_2)        Share the same Addr */
        0x8d, /* Port 14 (SGMII4P_4)        Share the same Addr */
        0x8d, /* Port 15 (SGMII4P_4)        Share the same Addr */
        0x8d, /* Port 16 (SGMII4P_4)        Share the same Addr */
        0x8d, /* Port 17 (SGMII4P_4)        Share the same Addr */
        0x91, /* Port 18 (SGMII4P_4)        IntBus=0 Addr=0x11 */
        0x91, /* Port 19 (SGMII4P_4)        Share the same Addr */
        0x91, /* Port 20 (SGMII4P_4)        Share the same Addr */
        0x91, /* Port 21 (SGMII4P_4)        Share the same Addr */
        0x95, /* Port 22 (SGMII4P_5)        Share the same Addr */
        0x95, /* Port 23 (SGMII4P_5)        Share the same Addr */
        0x95, /* Port 24 (SGMII4P_5)        Share the same Addr */
        0x95, /* Port 25 (SGMII4P_5)        Share the same Addr */
        0xc1, /* Port 26 (QTC0-SGMII/QSGMII) IntBus=2 Addr=0x01 */
        0xc1, /* Port 27 (QTC0-SGMII/QSGMII)Share the same Addr */
        0xc1, /* Port 28 (QTC0-SGMII/QSGMII)Share the same Addr */
        0xc1, /* Port 29 (QTC0-SGMII/QSGMII)Share the same Addr */
        0xc1, /* Port 30 (QTC0-QSGMII)      Share the same Addr */
        0xc1, /* Port 31 (QTC0-QSGMII)      Share the same Addr */
        0xc1, /* Port 32 (QTC0-QSGMII)      Share the same Addr */
        0xc1, /* Port 33 (QTC0-QSGMII)      Share the same Addr */
        0xc1, /* Port 34 (QTC0-QSGMII)      Share the same Addr */
        0xc1, /* Port 35 (QTC0-QSGMII)      Share the same Addr */
        0xc1, /* Port 36 (QTC0-QSGMII)      Share the same Addr */
        0xc1, /* Port 37 (QTC0-QSGMII)      Share the same Addr */
        0xc1, /* Port 38 (QTC0-QSGMII)      Share the same Addr */
        0xc1, /* Port 39 (QTC0-QSGMII)      Share the same Addr */
        0xc1, /* Port 40 (QTC0-QSGMII)      Share the same Addr */
        0xc1, /* Port 41 (QTC0-QSGMII)      Share the same Addr */
        0xc5, /* Port 42 (QTC1-SGMII/QSGMII) IntBus=2 Addr=0x05 */
        0xc5, /* Port 43 (QTC1-SGMII/QSGMII)Share the same Addr */
        0xc5, /* Port 44 (QTC1-SGMII/QSGMII)Share the same Addr */
        0xc5, /* Port 45 (QTC1-SGMII/QSGMII)Share the same Addr */
        0xc5, /* Port 46 (QTC1-QSGMII)      Share the same Addr */
        0xc5, /* Port 47 (QTC1-QSGMII)      Share the same Addr */
        0xc5, /* Port 48 (QTC1-QSGMII)      Share the same Addr */
        0xc5, /* Port 49 (QTC1-QSGMII)      Share the same Addr */
        0xc5, /* Port 50 (QTC1-QSGMII)      Share the same Addr */
        0xc5, /* Port 51 (QTC1-QSGMII)      Share the same Addr */
        0xc5, /* Port 52 (QTC1-QSGMII)      Share the same Addr */
        0xc5, /* Port 53 (QTC1-QSGMII)      Share the same Addr */
        0xc5, /* Port 54 (QTC1-QSGMII)      Share the same Addr */
        0xc5, /* Port 55 (QTC1-QSGMII)      Share the same Addr */
        0xc5, /* Port 56 (QTC1-QSGMII)      Share the same Addr */
        0xc5, /* Port 57 (QTC1-QSGMII)      Share the same Addr */
        0xa1, /* Port 58 (TSCE0)            IntBus=1 Addr=0x01 */
        0xa1, /* Port 59 (TSCE0)            Share the same Addr */
        0xa1, /* Port 60 (TSCE0)            Share the same Addr */
        0xa1, /* Port 61 (TSCE0)            Share the same Addr */
        0xa5, /* Port 62 (TSCE0)            IntBus=1 Addr=0x05 */
        0xa5, /* Port 63 (TSCE1)            Share the same Addr */
        0xa5, /* Port 64 (TSCE1)            Share the same Addr */
        0xa5, /* Port 65 (TSCE1)            Share the same Addr */
        0xa9, /* Port 66 (TSCE2)            IntBus=1 Addr=0x09 */
        0xa9, /* Port 67 (TSCE2)            Share the same Addr */
        0xa9, /* Port 68 (TSCE2)            Share the same Addr */
        0xa9, /* Port 69 (TSCE2)            Share the same Addr */
        0xad, /* Port 70 (TSCE3)            IntBus=1 Addr=0x0d */
        0xad, /* Port 71 (TSCE3)            Share the same Addr */
        0xad, /* Port 72 (TSCE3)            Share the same Addr */
        0xad, /* Port 73 (TSCE3)            Share the same Addr */
        0xb1, /* Port 74 (TSCE4)            IntBus=1 Addr=0x11 */
        0xb1, /* Port 75 (TSCE4)            Share the same Addr */
        0xb1, /* Port 76 (TSCE4)            Share the same Addr */
        0xb1, /* Port 77 (TSCE4)            Share the same Addr */
        0xb5, /* Port 78 (TSCE5)            IntBus=1 Addr=0x15 */
        0xb5, /* Port 79 (TSCE5)            Share the same Addr */
        0xb5, /* Port 80 (TSCE5)            Share the same Addr */
        0xb5, /* Port 81 (TSCE5)            Share the same Addr */
        0xb9, /* Port 82 (TSCE6)            IntBus=1 Addr=0x19 */
        0xb9, /* Port 83 (TSCE6)            Share the same Addr */
        0xb9, /* Port 84 (TSCE6)            Share the same Addr */
        0xb9, /* Port 85 (TSCF0)            Share the same Addr */
        0xe1, /* Port 86 (TSCF0)            IntBus=3 Addr=0x01 */
        0xe1, /* Port 87 (TSCF0)            Share the same Addr */
        0xe1, /* Port 88 (TSCF0)            Share the same Addr */
        0xe1  /* Port 89 (TSCF0)            Share the same Addr */
    };

    int phy_port = SOC_INFO(unit).port_l2p_mapping[port];

    /* assigning default phy_addr for ext-phy */
    *phy_addr = 0xff;

    *phy_addr_int = _soc_gh2_int_phy_addr[phy_port];

}
#endif  /* BCM_GREYHOUND2_SUPPORT */

#if defined(BCM_HELIX4_SUPPORT) 

static void
_helix4_phy_addr_default(int unit, int port,
                          uint16 *phy_addr, uint16 *phy_addr_int)
{
    static const uint16 _soc_helix4_int_phy_addr[] = {
        0x80, /* Port   0 (cmic) N/A */

        0x81, /* Port   1 (QC0) IntBus=0 Addr=0x01 */
        0x81, /* Port   2 (QC0) IntBus=0 Addr=0x02 */
        0x81, /* Port   3 (QC0) IntBus=0 Addr=0x03 */
        0x81, /* Port   4 (QC0) IntBus=0 Addr=0x04 */
        0x81, /* Port   5 (QC0) IntBus=0 Addr=0x05 */
        0x81, /* Port   6 (QC0) IntBus=0 Addr=0x06 */
        0x81, /* Port   7 (QC0) IntBus=0 Addr=0x07 */
        0x81, /* Port   8 (QC0) IntBus=0 Addr=0x08 */

        0x89, /* Port   9 (QC1) IntBus=0 Addr=0x09 */
        0x89, /* Port  10 (QC1) IntBus=0 Addr=0x0A */
        0x89, /* Port  11 (QC1) IntBus=0 Addr=0x0B */
        0x89, /* Port  12 (QC1) IntBus=0 Addr=0x0C */
        0x89, /* Port  13 (QC1) IntBus=0 Addr=0x0D */
        0x89, /* Port  14 (QC1) IntBus=0 Addr=0x0E */
        0x89, /* Port  15 (QC1) IntBus=0 Addr=0x0F */
        0x89, /* Port  16 (QC1) IntBus=0 Addr=0x10 */

        0x91, /* Port  17 (QC2) IntBus=0 Addr=0x11 */
        0x91, /* Port  18 (QC2) IntBus=0 Addr=0x12 */
        0x91, /* Port  19 (QC2) IntBus=0 Addr=0x13 */
        0x91, /* Port  20 (QC2) IntBus=0 Addr=0x14 */
        0x91, /* Port  21 (QC2) IntBus=0 Addr=0x15 */
        0x91, /* Port  22 (QC2) IntBus=0 Addr=0x16 */
        0x91, /* Port  23 (QC2) IntBus=0 Addr=0x17 */
        0x91, /* Port  24 (QC2) IntBus=0 Addr=0x18 */

        0xA1, /* Port  25 (QC3) IntBus=1 Addr=0x01 */
        0xA1, /* Port  26 (QC3) IntBus=1 Addr=0x02 */
        0xA1, /* Port  27 (QC3) IntBus=1 Addr=0x03 */
        0xA1, /* Port  28 (QC3) IntBus=1 Addr=0x04 */
        0xA1, /* Port  29 (QC3) IntBus=1 Addr=0x05 */
        0xA1, /* Port  30 (QC3) IntBus=1 Addr=0x06 */
        0xA1, /* Port  31 (QC3) IntBus=1 Addr=0x07 */
        0xA1, /* Port  32 (QC3) IntBus=1 Addr=0x08 */

        0xA9, /* Port  33 (QC4) IntBus=1 Addr=0x09 */
        0xA9, /* Port  34 (QC4) IntBus=1 Addr=0x0a */
        0xA9, /* Port  35 (QC4) IntBus=1 Addr=0x0b */
        0xA9, /* Port  36 (QC4) IntBus=1 Addr=0x0c */
        0xA9, /* Port  37 (QC4) IntBus=1 Addr=0x0d */
        0xA9, /* Port  38 (QC4) IntBus=1 Addr=0x0e */
        0xA9, /* Port  39 (QC4) IntBus=1 Addr=0x0f */
        0xA9, /* Port  40 (QC4) IntBus=1 Addr=0x10 */

        0xB1, /* Port  41 (QC5) IntBus=1 Addr=0x11 */
        0xB1, /* Port  42 (QC5) IntBus=1 Addr=0x12 */
        0xB1, /* Port  43 (QC5) IntBus=1 Addr=0x13 */
        0xB1, /* Port  44 (QC5) IntBus=1 Addr=0x14 */
        0xB1, /* Port  45 (QC5) IntBus=1 Addr=0x15 */
        0xB1, /* Port  46 (QC5) IntBus=1 Addr=0x16 */
        0xB1, /* Port  47 (QC5) IntBus=1 Addr=0x17 */
        0xB1, /* Port  48 (QC5) IntBus=1 Addr=0x18 */

        0xE3, /* Port  49 (QSG) IntBus=3 Addr=0x03 */
        0xE3, /* Port  50 (---) IntBus=3 Addr=0x04 */
        0xE3, /* Port  51 (---) IntBus= Addr=0x */
        0xE3, /* Port  52 (---) IntBus= Addr=0x */

        0xC1, /* Port  53 (WC0) IntBus=2 Addr=0x01 */
        0xC1, /* Port  54 (WC0) IntBus=2 Addr=0x02 */
        0xC1, /* Port  55 (WC0) IntBus=2 Addr=0x03 */
        0xC1, /* Port  56 (WC0) IntBus=2 Addr=0x04 */

        0xC5, /* Port  57 (WC1) IntBus=2 Addr=0x05 */
        0xC5, /* Port  58 (WC1) IntBus=2 Addr=0x06 */
        0xC5, /* Port  59 (WC1) IntBus=2 Addr=0x07 */
        0xC5, /* Port  60 (WC1) IntBus=2 Addr=0x08 */

        0xC9, /* Port  61 (WC2) IntBus=2 Addr=0x09 */
        0xC9, /* Port  62 (WC2) IntBus=2 Addr=0x0A */
        0xC9, /* Port  63 (WC2) IntBus=2 Addr=0x0B */
        0xC9, /* Port  64 (WC2) IntBus=2 Addr=0x0C */
    };

    static const uint16 _soc_phy_addr_bcm56340[] = {

        0x00, /* Port   0 (cmic) N/A */

        0x01, /* Port   1 (QC0) ExtBus=0 Addr=0x01 */
        0x02, /* Port   2 (QC0) ExtBus=0 Addr=0x02 */
        0x03, /* Port   3 (QC0) ExtBus=0 Addr=0x03 */
        0x04, /* Port   4 (QC0) ExtBus=0 Addr=0x04 */
        0x05, /* Port   5 (QC0) ExtBus=0 Addr=0x05 */
        0x06, /* Port   6 (QC0) ExtBus=0 Addr=0x06 */
        0x07, /* Port   7 (QC0) ExtBus=0 Addr=0x07 */
        0x08, /* Port   8 (QC0) ExtBus=0 Addr=0x08 */

        0x0a, /* Port   9 (QC1) ExtBus=0 Addr=0x09 */
        0x0b, /* Port  10 (QC1) ExtBus=0 Addr=0x0A */
        0x0c, /* Port  11 (QC1) ExtBus=0 Addr=0x0B */
        0x0d, /* Port  12 (QC1) ExtBus=0 Addr=0x0C */
        0x0e, /* Port  13 (QC1) ExtBus=0 Addr=0x0D */
        0x0f, /* Port  14 (QC1) ExtBus=0 Addr=0x0E */
        0x10, /* Port  15 (QC1) ExtBus=0 Addr=0x0F */
        0x11, /* Port  16 (QC1) ExtBus=0 Addr=0x10 */

        0x13, /* Port  17 (QC2) ExtBus=0 Addr=0x11 */
        0x14, /* Port  18 (QC2) ExtBus=0 Addr=0x12 */
        0x15, /* Port  19 (QC2) ExtBus=0 Addr=0x13 */
        0x16, /* Port  20 (QC2) ExtBus=0 Addr=0x14 */
        0x17, /* Port  21 (QC2) ExtBus=0 Addr=0x15 */
        0x18, /* Port  22 (QC2) ExtBus=0 Addr=0x16 */
        0x19, /* Port  23 (QC2) ExtBus=0 Addr=0x17 */
        0x1a, /* Port  24 (QC2) ExtBus=0 Addr=0x18 */

        0x21, /* Port  25 (QC3) ExtBus=1 Addr=0x01 */
        0x22, /* Port  26 (QC3) ExtBus=1 Addr=0x02 */
        0x23, /* Port  27 (QC3) ExtBus=1 Addr=0x03 */
        0x24, /* Port  28 (QC3) ExtBus=1 Addr=0x04 */
        0x25, /* Port  29 (QC3) ExtBus=1 Addr=0x05 */
        0x26, /* Port  30 (QC3) ExtBus=1 Addr=0x06 */
        0x27, /* Port  31 (QC3) ExtBus=1 Addr=0x07 */
        0x28, /* Port  32 (QC3) ExtBus=1 Addr=0x08 */

        0x2a, /* Port  33 (QC4) ExtBus=1 Addr=0x09 */
        0x2b, /* Port  34 (QC4) ExtBus=1 Addr=0x0a */
        0x2c, /* Port  35 (QC4) ExtBus=1 Addr=0x0b */
        0x2d, /* Port  36 (QC4) ExtBus=1 Addr=0x0c */
        0x2e, /* Port  37 (QC4) ExtBus=1 Addr=0x0d */
        0x2f, /* Port  38 (QC4) ExtBus=1 Addr=0x0e */
        0x30, /* Port  39 (QC4) ExtBus=1 Addr=0x0f */
        0x31, /* Port  40 (QC4) ExtBus=1 Addr=0x10 */

        0x33, /* Port  41 (QC5) ExtBus=1 Addr=0x11 */
        0x34, /* Port  42 (QC5) ExtBus=1 Addr=0x12 */
        0x35, /* Port  43 (QC5) ExtBus=1 Addr=0x13 */
        0x36, /* Port  44 (QC5) ExtBus=1 Addr=0x14 */
        0x37, /* Port  45 (QC5) ExtBus=1 Addr=0x15 */
        0x38, /* Port  46 (QC5) ExtBus=1 Addr=0x16 */
        0x39, /* Port  47 (QC5) ExtBus=1 Addr=0x17 */
        0x3a, /* Port  48 (QC5) ExtBus=1 Addr=0x18 */

        0x63, /* Port  49 (QSG) ExtBus=3 Addr=0x03 */
        0x63, /* Port  50 (---) ExtBus=3 Addr=0x04 */
        0x63, /* Port  51 (---) ExtBus= Addr=0x */
        0x63, /* Port  52 (---) ExtBus= Addr=0x */

        0x41, /* Port  53 (WC0) ExtBus=2 Addr=0x01 */
        0x42, /* Port  54 (WC0) ExtBus=2 Addr=0x02 */
        0x43, /* Port  55 (WC0) ExtBus=2 Addr=0x03 */
        0x44, /* Port  56 (WC0) ExtBus=2 Addr=0x04 */

        0x45, /* Port  57 (WC1) ExtBus=2 Addr=0x05 */
        0x46, /* Port  58 (WC1) ExtBus=2 Addr=0x06 */
        0x47, /* Port  59 (WC1) ExtBus=2 Addr=0x07 */
        0x48, /* Port  60 (WC1) ExtBus=2 Addr=0x08 */

        0x49, /* Port  61 (WC2) ExtBus=2 Addr=0x09 */
        0x4a, /* Port  62 (WC2) ExtBus=2 Addr=0x0A */
        0x4b, /* Port  63 (WC2) ExtBus=2 Addr=0x0B */
        0x4c, /* Port  64 (WC2) ExtBus=2 Addr=0x0C */
    };
    uint16 dev_id;
    uint8 rev_id;
    int phy_port;

    soc_cm_get_id(unit, &dev_id, &rev_id);

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];

    *phy_addr_int = _soc_helix4_int_phy_addr[phy_port];

    switch (dev_id) {
    case BCM56548H_DEVICE_ID:
    case BCM56548_DEVICE_ID:
    case BCM56547_DEVICE_ID:
    case BCM56344_DEVICE_ID:
    case BCM56342_DEVICE_ID:
    case BCM56340_DEVICE_ID:
    case BCM56049_DEVICE_ID:
    case BCM56048_DEVICE_ID:
    case BCM56047_DEVICE_ID:
    case BCM56042_DEVICE_ID:
    case BCM56041_DEVICE_ID:
    case BCM56040_DEVICE_ID:
        *phy_addr = _soc_phy_addr_bcm56340[phy_port];
        break;
    default:
        *phy_addr = _soc_phy_addr_bcm56340[phy_port];
        break;
    }
}
#endif /* BCM_HELIX4_SUPPORT */

#if defined(BCM_TRIUMPH3_SUPPORT) 

static void
_triumph3_phy_addr_default(int unit, int port,
                          uint16 *phy_addr, uint16 *phy_addr_int)
{
    static const uint16 _soc_triumph3_int_phy_addr[] = {
        0x80, /* Port   0 (cmic) N/A */
        0x81, /* Port   1 (XC0) IntBus=0 Addr=0x01 */
        0x82, /* Port   2 (XC0) IntBus=0 Addr=0x02 */
        0x83, /* Port   3 (XC0) IntBus=0 Addr=0x03 */
        0x84, /* Port   4 (XC0) IntBus=0 Addr=0x04 */
        0x85, /* Port   5 (XC1) IntBus=0 Addr=0x05 */
        0x86, /* Port   6 (XC1) IntBus=0 Addr=0x06 */
        0x87, /* Port   7 (XC1) IntBus=0 Addr=0x07 */
        0x88, /* Port   8 (XC1) IntBus=0 Addr=0x08 */
        0x89, /* Port   9 (XC2) IntBus=0 Addr=0x09 */
        0x8A, /* Port  10 (XC2) IntBus=0 Addr=0x0A */
        0x8B, /* Port  11 (XC2) IntBus=0 Addr=0x0B */
        0x8C, /* Port  12 (XC2) IntBus=0 Addr=0x0C */
        0x8D, /* Port  13 (XC3) IntBus=0 Addr=0x0D */
        0x8E, /* Port  14 (XC3) IntBus=0 Addr=0x0E */
        0x8F, /* Port  15 (XC3) IntBus=0 Addr=0x0F */
        0x90, /* Port  16 (XC3) IntBus=0 Addr=0x10 */
        0x91, /* Port  17 (XC4) IntBus=0 Addr=0x11 */
        0x92, /* Port  18 (XC4) IntBus=0 Addr=0x12 */
        0x93, /* Port  19 (XC4) IntBus=0 Addr=0x13 */
        0x94, /* Port  20 (XC4) IntBus=0 Addr=0x14 */
        0x95, /* Port  21 (XC5) IntBus=0 Addr=0x15 */
        0x96, /* Port  22 (XC5) IntBus=0 Addr=0x16 */
        0x97, /* Port  23 (XC5) IntBus=0 Addr=0x17 */
        0x98, /* Port  24 (XC5) IntBus=0 Addr=0x18 */
        0x99, /* Port  25 (XC6) IntBus=0 Addr=0x19 */
        0x9A, /* Port  26 (XC6) IntBus=0 Addr=0x1A */
        0x9B, /* Port  27 (XC6) IntBus=0 Addr=0x1B */
        0x9C, /* Port  28 (XC6) IntBus=0 Addr=0x1C */

        0xA1, /* Port  29 (XC7) IntBus=1 Addr=0x01 */
        0xA2, /* Port  30 (XC7) IntBus=1 Addr=0x02 */
        0xA3, /* Port  31 (XC7) IntBus=1 Addr=0x03 */
        0xA4, /* Port  32 (XC7) IntBus=1 Addr=0x04 */
        0xA5, /* Port  33 (XC8) IntBus=1 Addr=0x05 */
        0xA6, /* Port  34 (XC8) IntBus=1 Addr=0x06 */
        0xA7, /* Port  35 (XC8) IntBus=1 Addr=0x07 */
        0xA8, /* Port  36 (XC8) IntBus=1 Addr=0x08 */
        0xA9, /* Port  37 (XC9) IntBus=1 Addr=0x09 */
        0,    /* Port  38 (---) IntBus=1 Addr=0 */
        0,    /* Port  39 (---) IntBus=1 Addr=0 */
        0,    /* Port  40 (---) IntBus=1 Addr=0 */
        0xAD, /* Port  41 (XC10) IntBus=1 Addr=0x0D */
        0xAE, /* Port  42 (XC10) IntBus=1 Addr=0x0E */
        0xAF, /* Port  43 (XC10) IntBus=1 Addr=0x0F */
        0xB0, /* Port  44 (XC10) IntBus=1 Addr=0x10 */
        0xB1, /* Port  45 (XC11) IntBus=1 Addr=0x11 */
        0xB2, /* Port  46 (XC11) IntBus=1 Addr=0x12 */
        0xB3, /* Port  47 (XC11) IntBus=1 Addr=0x13 */
        0xB4, /* Port  48 (XC11) IntBus=1 Addr=0x14 */
        0xB5, /* Port  49 (XC12) IntBus=1 Addr=0x15 */
        0xB6, /* Port  50 (XC12) IntBus=1 Addr=0x16 */
        0xB7, /* Port  51 (XC12) IntBus=1 Addr=0x17 */
        0xB8, /* Port  52 (XC12) IntBus=1 Addr=0x18 */

#ifdef TR3_WCMOD_MULTIPORT_SUPPORT
        
        0xC1, /* Port  53 (WC0) IntBus=2 Addr=0x01 */
        0xC2, /* Port  54 (WC0) IntBus=2 Addr=0x02 */
        0xC3, /* Port  55 (WC0) IntBus=2 Addr=0x03 */
        0xC4, /* Port  56 (WC0) IntBus=2 Addr=0x04 */
        0xC5, /* Port  57 (WC1) IntBus=2 Addr=0x05 */
        0xC6, /* Port  58 (WC1) IntBus=2 Addr=0x06 */
        0xC7, /* Port  59 (WC1) IntBus=2 Addr=0x07 */
        0xC8, /* Port  60 (WC1) IntBus=2 Addr=0x08 */
        0xC9, /* Port  61 (WC2) IntBus=2 Addr=0x09 */
        0xCA, /* Port  62 (WC2) IntBus=2 Addr=0x0A */
        0xCB, /* Port  63 (WC2) IntBus=2 Addr=0x0B */
        0xCC, /* Port  64 (WC2) IntBus=2 Addr=0x0C */
        0xCD, /* Port  65 (WC3) IntBus=2 Addr=0x0D */
        0xCE, /* Port  66 (WC3) IntBus=2 Addr=0x0E */
        0xCF, /* Port  67 (WC3) IntBus=2 Addr=0x0F */
        0xD0, /* Port  68 (WC3) IntBus=2 Addr=0x10 */
        0xD1, /* Port  69 (WC4) IntBus=2 Addr=0x11 */
        0xD2, /* Port  70 (WC4) IntBus=2 Addr=0x12 */
        0xD3, /* Port  71 (WC4) IntBus=2 Addr=0x13 */
        0xD4, /* Port  72 (WC4) IntBus=2 Addr=0x14 */
        0xD5, /* Port  73 (WC5) IntBus=2 Addr=0x15 */
        0xD6, /* Port  74 (WC5) IntBus=2 Addr=0x16 */
        0xD7, /* Port  75 (WC5) IntBus=2 Addr=0x17 */
        0xD8, /* Port  76 (WC5) IntBus=2 Addr=0x18 */
        0xD9, /* Port  77 (WC6) IntBus=2 Addr=0x19 */
        0xDA, /* Port  78 (WC6) IntBus=2 Addr=0x1A */
        0xDB, /* Port  79 (WC6) IntBus=2 Addr=0x1B */
        0xDC, /* Port  80 (WC6) IntBus=2 Addr=0x1C */
#else
        0xC1, /* Port  53 (WC0) IntBus=2 Addr=0x01 */
        0xC1, /* Port  54 (WC0) IntBus=2 Addr=0x02 */
        0xC1, /* Port  55 (WC0) IntBus=2 Addr=0x03 */
        0xC1, /* Port  56 (WC0) IntBus=2 Addr=0x04 */
        0xC5, /* Port  57 (WC1) IntBus=2 Addr=0x05 */
        0xC5, /* Port  58 (WC1) IntBus=2 Addr=0x06 */
        0xC5, /* Port  59 (WC1) IntBus=2 Addr=0x07 */
        0xC5, /* Port  60 (WC1) IntBus=2 Addr=0x08 */
        0xC9, /* Port  61 (WC2) IntBus=2 Addr=0x09 */
        0xC9, /* Port  62 (WC2) IntBus=2 Addr=0x0A */
        0xC9, /* Port  63 (WC2) IntBus=2 Addr=0x0B */
        0xC9, /* Port  64 (WC2) IntBus=2 Addr=0x0C */
        0xCD, /* Port  65 (WC3) IntBus=2 Addr=0x0D */
        0xCD, /* Port  66 (WC3) IntBus=2 Addr=0x0E */
        0xCD, /* Port  67 (WC3) IntBus=2 Addr=0x0F */
        0xCD, /* Port  68 (WC3) IntBus=2 Addr=0x10 */
        0xD1, /* Port  69 (WC4) IntBus=2 Addr=0x11 */
        0xD1, /* Port  70 (WC4) IntBus=2 Addr=0x12 */
        0xD1, /* Port  71 (WC4) IntBus=2 Addr=0x13 */
        0xD1, /* Port  72 (WC4) IntBus=2 Addr=0x14 */
        0xD5, /* Port  73 (WC5) IntBus=2 Addr=0x15 */
        0xD5, /* Port  74 (WC5) IntBus=2 Addr=0x16 */
        0xD5, /* Port  75 (WC5) IntBus=2 Addr=0x17 */
        0xD5, /* Port  76 (WC5) IntBus=2 Addr=0x18 */
        0xD9, /* Port  77 (WC6) IntBus=2 Addr=0x19 */
        0xD9, /* Port  78 (WC6) IntBus=2 Addr=0x1A */
        0xD9, /* Port  79 (WC6) IntBus=2 Addr=0x1B */
        0xD9, /* Port  80 (WC6) IntBus=2 Addr=0x1C */
#endif /* TR3_WCMOD_MULTIPORT_SUPPORT */
    };

    static const uint16 _soc_phy_addr_bcm56640[] = {    
        0x00, /* Port   0 (cmic) N/A */
        0x01, /* Port   1 (XC0) ExtBus=0 Addr=0x01 */
        0x02, /* Port   2 (XC0) ExtBus=0 Addr=0x02 */
        0x03, /* Port   3 (XC0) ExtBus=0 Addr=0x03 */
        0x04, /* Port   4 (XC0) ExtBus=0 Addr=0x04 */
        0x05, /* Port   5 (XC1) ExtBus=0 Addr=0x05 */
        0x06, /* Port   6 (XC1) ExtBus=0 Addr=0x06 */
        0x07, /* Port   7 (XC1) ExtBus=0 Addr=0x07 */
        0x08, /* Port   8 (XC1) ExtBus=0 Addr=0x08 */
        0x09, /* Port   9 (XC2) ExtBus=0 Addr=0x09 */
        0x0A, /* Port  10 (XC2) ExtBus=0 Addr=0x0A */
        0x0B, /* Port  11 (XC2) ExtBus=0 Addr=0x0B */
        0x0C, /* Port  12 (XC2) ExtBus=0 Addr=0x0C */
        0x0D, /* Port  13 (XC3) ExtBus=0 Addr=0x0D */
        0x0E, /* Port  14 (XC3) ExtBus=0 Addr=0x0E */
        0x0F, /* Port  15 (XC3) ExtBus=0 Addr=0x0F */
        0x10, /* Port  16 (XC3) ExtBus=0 Addr=0x10 */
        0x11, /* Port  17 (XC4) ExtBus=0 Addr=0x11 */
        0x12, /* Port  18 (XC4) ExtBus=0 Addr=0x12 */
        0x13, /* Port  19 (XC4) ExtBus=0 Addr=0x13 */
        0x14, /* Port  20 (XC4) ExtBus=0 Addr=0x14 */
        0x15, /* Port  21 (XC5) ExtBus=0 Addr=0x15 */
        0x16, /* Port  22 (XC5) ExtBus=0 Addr=0x16 */
        0x17, /* Port  23 (XC5) ExtBus=0 Addr=0x17 */
        0x18, /* Port  24 (XC5) ExtBus=0 Addr=0x18 */

        0x21, /* Port  25 (XC6) ExtBus=1 Addr=0x01 */
        0x22, /* Port  26 (XC6) ExtBus=1 Addr=0x02 */
        0x23, /* Port  27 (XC6) ExtBus=1 Addr=0x03 */
        0x24, /* Port  28 (XC6) ExtBus=1 Addr=0x04 */
        0x25, /* Port  29 (XC7) ExtBus=1 Addr=0x05 */
        0x26, /* Port  30 (XC7) ExtBus=1 Addr=0x06 */
        0x27, /* Port  31 (XC7) ExtBus=1 Addr=0x07 */
        0x28, /* Port  32 (XC7) ExtBus=1 Addr=0x08 */
        0x29, /* Port  33 (XC8) ExtBus=1 Addr=0x09 */
        0x2A, /* Port  34 (XC8) ExtBus=1 Addr=0x0A */
        0x2B, /* Port  35 (XC8) ExtBus=1 Addr=0x0B */
        0x2C, /* Port  36 (XC8) ExtBus=1 Addr=0x0C */
        0x1D, /* Port  37 (XC9) ExtBus=0 Addr=0x1d */
        0,    /* Port  38 (---) ExtBus=1 Addr=0 */
        0,    /* Port  39 (---) ExtBus=1 Addr=0 */
        0,    /* Port  40 (---) ExtBus=1 Addr=0 */
        0x2D, /* Port  41 (XC10) ExtBus=1 Addr=0x0D */
        0x2E, /* Port  42 (XC10) ExtBus=1 Addr=0x0E */
        0x2F, /* Port  43 (XC10) ExtBus=1 Addr=0x0F */
        0x30, /* Port  44 (XC10) ExtBus=1 Addr=0x10 */
        0x31, /* Port  45 (XC11) ExtBus=1 Addr=0x11 */
        0x32, /* Port  46 (XC11) ExtBus=1 Addr=0x12 */
        0x33, /* Port  47 (XC11) ExtBus=1 Addr=0x13 */
        0x34, /* Port  48 (XC11) ExtBus=1 Addr=0x14 */
        0x35, /* Port  49 (XC12) ExtBus=1 Addr=0x15 */
        0x36, /* Port  50 (XC12) ExtBus=1 Addr=0x16 */
        0x37, /* Port  51 (XC12) ExtBus=1 Addr=0x17 */
        0x38, /* Port  52 (XC12) ExtBus=1 Addr=0x18 */

        0x41, /* Port  53 (WC0) ExtBus=2 Addr=0x01 */
        0x42, /* Port  54 (WC0) ExtBus=2 Addr=0x02 */
        0x43, /* Port  55 (WC0) ExtBus=2 Addr=0x03 */
        0x44, /* Port  56 (WC0) ExtBus=2 Addr=0x04 */
        0x45, /* Port  57 (WC1) ExtBus=2 Addr=0x05 */
        0x46, /* Port  58 (WC1) ExtBus=2 Addr=0x06 */
        0x47, /* Port  59 (WC1) ExtBus=2 Addr=0x07 */
        0x48, /* Port  60 (WC1) ExtBus=2 Addr=0x08 */
        0x49, /* Port  61 (WC2) ExtBus=2 Addr=0x09 */
        0x4A, /* Port  62 (WC2) ExtBus=2 Addr=0x0A */
        0x4B, /* Port  63 (WC2) ExtBus=2 Addr=0x0B */
        0x4C, /* Port  64 (WC2) ExtBus=2 Addr=0x0C */
        0x4D, /* Port  65 (WC3) ExtBus=2 Addr=0x0D */
        0x4E, /* Port  66 (WC3) ExtBus=2 Addr=0x0E */
        0x4F, /* Port  67 (WC3) ExtBus=2 Addr=0x0F */
        0x50, /* Port  68 (WC3) ExtBus=2 Addr=0x10 */
        0x51, /* Port  69 (WC4) ExtBus=2 Addr=0x11 */
        0x52, /* Port  70 (WC4) ExtBus=2 Addr=0x12 */

        0x53, /* Port  71 (WC4) ExtBus=2 Addr=0x13 */
        0x54, /* Port  72 (WC4) ExtBus=2 Addr=0x14 */
        0x55, /* Port  73 (WC5) ExtBus=2 Addr=0x15 */
        0x56, /* Port  74 (WC5) ExtBus=2 Addr=0x16 */
        0x57, /* Port  75 (WC5) ExtBus=2 Addr=0x17 */
        0x58, /* Port  76 (WC5) ExtBus=2 Addr=0x18 */
        0x59, /* Port  77 (WC6) ExtBus=2 Addr=0x19 */
        0x5A, /* Port  78 (WC6) ExtBus=2 Addr=0x1A */
        0x5B, /* Port  79 (WC6) ExtBus=2 Addr=0x1B */
        0x5C, /* Port  80 (WC6) ExtBus=2 Addr=0x1C */
    };
    uint16 dev_id;
    uint8 rev_id;
    int phy_port;

    soc_cm_get_id(unit, &dev_id, &rev_id);

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];

    *phy_addr_int = _soc_triumph3_int_phy_addr[phy_port];

    switch (dev_id) {
    case BCM56640_DEVICE_ID:
    
    case BCM56643_DEVICE_ID: 
    case BCM56644_DEVICE_ID:
    case BCM56649_DEVICE_ID:
    case BCM56540_DEVICE_ID:
    case BCM56541_DEVICE_ID:
    case BCM56542_DEVICE_ID:
    case BCM56543_DEVICE_ID:
    case BCM56544_DEVICE_ID:
    case BCM56545_DEVICE_ID:
    case BCM56546_DEVICE_ID:
        *phy_addr = _soc_phy_addr_bcm56640[phy_port];
        break;
    default:
        
        *phy_addr = _soc_phy_addr_bcm56640[phy_port];
        break;
    }
}
#endif /* BCM_TRIUMPH3_SUPPORT */

#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_APOLLO_SUPPORT) || \
    defined(BCM_VALKYRIE2_SUPPORT)
static void
_triumph2_phy_addr_default(int unit, int port,
                          uint16 *phy_addr, uint16 *phy_addr_int)
{
    static const uint16 _soc_triumph2_int_phy_addr[] = {
        0x00, /* Port  0 (cmic)         N/A */
        0x99, /* Port  1 (9SERDES)      IntBus=0 Addr=0x19 */
        0x81, /* Port  2 (8SERDES_0)    IntBus=0 Addr=0x01 */
        0x82, /* Port  3 (8SERDES_0)    IntBus=0 Addr=0x02 */
        0x83, /* Port  4 (8SERDES_0)    IntBus=0 Addr=0x03 */
        0x84, /* Port  5 (8SERDES_0)    IntBus=0 Addr=0x04 */
        0x85, /* Port  6 (8SERDES_0)    IntBus=0 Addr=0x05 */
        0x86, /* Port  7 (8SERDES_0)    IntBus=0 Addr=0x06 */
        0x87, /* Port  8 (8SERDES_0)    IntBus=0 Addr=0x07 */
        0x88, /* Port  9 (8SERDES_0)    IntBus=0 Addr=0x08 */
        0x89, /* Port 10 (8SERDES_1)    IntBus=0 Addr=0x09 */
        0x8a, /* Port 11 (8SERDES_1)    IntBus=0 Addr=0x0a */
        0x8b, /* Port 12 (8SERDES_1)    IntBus=0 Addr=0x0b */
        0x8c, /* Port 13 (8SERDES_1)    IntBus=0 Addr=0x0c */
        0x8d, /* Port 14 (8SERDES_1)    IntBus=0 Addr=0x0d */
        0x8e, /* Port 15 (8SERDES_1)    IntBus=0 Addr=0x0e */
        0x8f, /* Port 16 (8SERDES_1)    IntBus=0 Addr=0x0f */
        0x90, /* Port 17 (8SERDES_1)    IntBus=0 Addr=0x10 */
        0x91, /* Port 18 (9SERDES)      IntBus=0 Addr=0x11 */
        0x92, /* Port 19 (9SERDES)      IntBus=0 Addr=0x12 */
        0x93, /* Port 20 (9SERDES)      IntBus=0 Addr=0x13 */
        0x94, /* Port 21 (9SERDES)      IntBus=0 Addr=0x14 */
        0x95, /* Port 22 (9SERDES)      IntBus=0 Addr=0x15 */
        0x96, /* Port 23 (9SERDES)      IntBus=0 Addr=0x16 */
        0x97, /* Port 24 (9SERDES)      IntBus=0 Addr=0x17 */
        0x98, /* Port 25 (9SERDES)      IntBus=0 Addr=0x18 */
        0xd9, /* Port 26 (HC0)          IntBus=2 Addr=0x19 */
        0xda, /* Port 27 (HC1)          IntBus=2 Addr=0x1a */
        0xdb, /* Port 28 (HC2)          IntBus=2 Addr=0x1b */
        0xdc, /* Port 29 (HC3)          IntBus=2 Addr=0x1c */
        0xc1, /* Port 30 (HL0)          IntBus=2 Addr=0x01 */
        0xc2, /* Port 31 (HL0)          IntBus=2 Addr=0x02 */
        0xc3, /* Port 32 (HL0)          IntBus=2 Addr=0x03 */
        0xc4, /* Port 33 (HL0)          IntBus=2 Addr=0x04 */
        0xc5, /* Port 34 (HL1)          IntBus=2 Addr=0x05 */
        0xc6, /* Port 35 (HL1)          IntBus=2 Addr=0x06 */
        0xc7, /* Port 36 (HL1)          IntBus=2 Addr=0x07 */
        0xc8, /* Port 37 (HL1)          IntBus=2 Addr=0x08 */
        0xc9, /* Port 38 (HL2)          IntBus=2 Addr=0x09 */
        0xca, /* Port 39 (HL2)          IntBus=2 Addr=0x0a */
        0xcb, /* Port 40 (HL2)          IntBus=2 Addr=0x0b */
        0xcc, /* Port 41 (HL2)          IntBus=2 Addr=0x0c */
        0xcd, /* Port 42 (HL3)          IntBus=2 Addr=0x0d */
        0xce, /* Port 43 (HL3)          IntBus=2 Addr=0x0e */
        0xcf, /* Port 44 (HL3)          IntBus=2 Addr=0x0f */
        0xd0, /* Port 45 (HL3)          IntBus=2 Addr=0x10 */
        0xd1, /* Port 46 (HL4)          IntBus=2 Addr=0x11 */
        0xd2, /* Port 47 (HL4)          IntBus=2 Addr=0x12 */
        0xd3, /* Port 48 (HL4)          IntBus=2 Addr=0x13 */
        0xd4, /* Port 49 (HL4)          IntBus=2 Addr=0x14 */
        0xd5, /* Port 50 (HL5)          IntBus=2 Addr=0x15 */
        0xd6, /* Port 51 (HL5)          IntBus=2 Addr=0x16 */
        0xd7, /* Port 52 (HL5)          IntBus=2 Addr=0x17 */
        0xd8, /* Port 53 (HL5)          IntBus=2 Addr=0x18 */
    };

    static const uint16 _soc_phy_addr_bcm56630[] = {
        0x00, /* Port  0 (cmic) N/A */
        0x19, /* Port  1 ( ge0) ExtBus=0 Addr=0x19 */
        0x00, /* Port  2 ( N/A)                    */
        0x00, /* Port  3 ( N/A)                    */
        0x00, /* Port  4 ( N/A)                    */
        0x00, /* Port  5 ( N/A)                    */
        0x00, /* Port  6 ( N/A)                    */
        0x00, /* Port  7 ( N/A)                    */
        0x00, /* Port  8 ( N/A)                    */
        0x00, /* Port  9 ( N/A)                    */
        0x00, /* Port 10 ( N/A)                    */
        0x00, /* Port 11 ( N/A)                    */
        0x00, /* Port 12 ( N/A)                    */
        0x00, /* Port 13 ( N/A)                    */
        0x00, /* Port 14 ( N/A)                    */
        0x00, /* Port 15 ( N/A)                    */
        0x00, /* Port 16 ( N/A)                    */
        0x00, /* Port 17 ( N/A)                    */
        0x00, /* Port 18 ( N/A)                    */
        0x00, /* Port 19 ( N/A)                    */
        0x00, /* Port 20 ( N/A)                    */
        0x00, /* Port 21 ( N/A)                    */
        0x00, /* Port 22 ( N/A)                    */
        0x00, /* Port 23 ( N/A)                    */
        0x00, /* Port 24 ( N/A)                    */
        0x00, /* Port 25 ( N/A)                    */
        0x41, /* Port 26 ( hg0) ExtBus=2 Addr=0x01 */
        0x42, /* Port 27 ( hg1) ExtBus=2 Addr=0x02 */
        0x43, /* Port 28 ( hg2) ExtBus=2 Addr=0x03 */
        0x44, /* Port 29 ( hg3) ExtBus=2 Addr=0x04 */
        0x01, /* Port 30 ( ge1) ExtBus=0 Addr=0x01 */
        0x02, /* Port 31 ( ge2) ExtBus=0 Addr=0x02 */
        0x03, /* Port 32 ( ge3) ExtBus=0 Addr=0x03 */
        0x04, /* Port 33 ( ge4) ExtBus=0 Addr=0x04 */
        0x05, /* Port 34 ( ge5) ExtBus=0 Addr=0x05 */
        0x06, /* Port 35 ( ge6) ExtBus=0 Addr=0x06 */
        0x07, /* Port 36 ( ge7) ExtBus=0 Addr=0x07 */
        0x08, /* Port 37 ( ge8) ExtBus=0 Addr=0x08 */
        0x09, /* Port 38 ( ge9) ExtBus=0 Addr=0x09 */
        0x0a, /* Port 39 (ge10) ExtBus=0 Addr=0x0a */
        0x0b, /* Port 40 (ge11) ExtBus=0 Addr=0x0b */
        0x0c, /* Port 41 (ge12) ExtBus=0 Addr=0x0c */
        0x0d, /* Port 42 (ge13) ExtBus=0 Addr=0x0d */
        0x0e, /* Port 43 (ge14) ExtBus=0 Addr=0x0e */
        0x0f, /* Port 44 (ge15) ExtBus=0 Addr=0x0f */
        0x10, /* Port 45 (ge16) ExtBus=0 Addr=0x10 */
        0x11, /* Port 46 (ge17) ExtBus=0 Addr=0x11 */
        0x12, /* Port 47 (ge18) ExtBus=0 Addr=0x12 */
        0x13, /* Port 48 (ge19) ExtBus=0 Addr=0x13 */
        0x14, /* Port 49 (ge20) ExtBus=0 Addr=0x14 */
        0x15, /* Port 50 (ge21) ExtBus=0 Addr=0x15 */
        0x16, /* Port 51 (ge22) ExtBus=0 Addr=0x16 */
        0x17, /* Port 52 (ge23) ExtBus=0 Addr=0x17 */
        0x18, /* Port 53 (ge24) ExtBus=0 Addr=0x18 */
    };

static const uint16 _soc_phy_addr_bcm56634[] = {
        0x00, /* Port  0 (cmic) N/A */
        0x19, /* Port  1 ( ge0) ExtBus=0 Addr=0x19 */
        0x09, /* Port  2 ( ge1) ExtBus=0 Addr=0x09 */
        0x0a, /* Port  3 ( ge2) ExtBus=0 Addr=0x0a */
        0x0b, /* Port  4 ( ge3) ExtBus=0 Addr=0x0b */
        0x0c, /* Port  5 ( ge4) ExtBus=0 Addr=0x0c */
        0x0d, /* Port  6 ( ge5) ExtBus=0 Addr=0x0d */
        0x0e, /* Port  7 ( ge6) ExtBus=0 Addr=0x0e */
        0x0f, /* Port  8 ( ge7) ExtBus=0 Addr=0x0f */
        0x10, /* Port  9 ( ge8) ExtBus=0 Addr=0x10 */
        0x15, /* Port 10 ( ge9) ExtBus=0 Addr=0x15 */ 
        0x16, /* Port 11 (ge10) ExtBus=0 Addr=0x16 */ 
        0x17, /* Port 12 (ge11) ExtBus=0 Addr=0x17 */
        0x18, /* Port 13 (ge12) ExtBus=0 Addr=0x18 */ 
        0x21, /* Port 14 (ge13) ExtBus=1 Addr=0x01 */
        0x22, /* Port 15 (ge14) ExtBus=1 Addr=0x02 */
        0x23, /* Port 16 (ge15) ExtBus=1 Addr=0x03 */
        0x24, /* Port 17 (ge16) ExtBus=1 Addr=0x04 */
        0x29, /* Port 18 (ge17) ExtBus=1 Addr=0x09 */
        0x2a, /* Port 19 (ge18) ExtBus=1 Addr=0x0a */
        0x2b, /* Port 20 (ge19) ExtBus=1 Addr=0x0b */
        0x2c, /* Port 21 (ge20) ExtBus=1 Addr=0x0c */
        0x2d, /* Port 22 (ge21) ExtBus=1 Addr=0x0d */
        0x2e, /* Port 23 (ge22) ExtBus=1 Addr=0x0e */
        0x2f, /* Port 24 (ge23) ExtBus=1 Addr=0x0f */
        0x30, /* Port 25 (ge24) ExtBus=1 Addr=0x10 */
        0x41, /* Port 26 ( hg0) ExtBus=2 Addr=0x01 */
        0x42, /* Port 27 ( hg1) ExtBus=2 Addr=0x02 */
        0x43, /* Port 28 ( hg2) ExtBus=2 Addr=0x03 */
        0x44, /* Port 29 ( hg3) ExtBus=2 Addr=0x04 */
        0x01, /* Port 30 (ge25) ExtBus=0 Addr=0x01 */
        0x02, /* Port 31 (ge26) ExtBus=0 Addr=0x02 */
        0x03, /* Port 32 (ge27) ExtBus=0 Addr=0x03 */
        0x04, /* Port 33 (ge28) ExtBus=0 Addr=0x04 */
        0x05, /* Port 34 (ge29) ExtBus=0 Addr=0x05 */
        0x06, /* Port 35 (ge30) ExtBus=0 Addr=0x06 */
        0x07, /* Port 36 (ge31) ExtBus=0 Addr=0x07 */
        0x08, /* Port 37 (ge32) ExtBus=0 Addr=0x08 */
        0x11, /* Port 38 (ge33) ExtBus=0 Addr=0x11 */
        0x12, /* Port 39 (ge34) ExtBus=0 Addr=0x12 */
        0x13, /* Port 40 (ge35) ExtBus=0 Addr=0x13 */
        0x14, /* Port 41 (ge36) ExtBus=0 Addr=0x14 */
        0x25, /* Port 42 (ge37) ExtBus=1 Addr=0x05 */
        0x26, /* Port 43 (ge38) ExtBus=1 Addr=0x06 */
        0x27, /* Port 44 (ge39) ExtBus=1 Addr=0x07 */
        0x28, /* Port 45 (ge40) ExtBus=1 Addr=0x08 */
        0x31, /* Port 46 (ge41) ExtBus=1 Addr=0x11 */
        0x32, /* Port 47 (ge42) ExtBus=1 Addr=0x12 */
        0x33, /* Port 48 (ge43) ExtBus=1 Addr=0x13 */
        0x34, /* Port 49 (ge44) ExtBus=1 Addr=0x14 */
        0x35, /* Port 50 (ge45) ExtBus=1 Addr=0x15 */
        0x36, /* Port 51 (ge46) ExtBus=1 Addr=0x16 */
        0x37, /* Port 52 (ge47) ExtBus=1 Addr=0x17 */
        0x38, /* Port 53 (ge48) ExtBus=1 Addr=0x18 */
    };


    static const uint16 _soc_phy_addr_bcm56636[] = {
        0x00, /* Port  0 (cmic) N/A */
        0x19, /* Port  1 ( ge0) ExtBus=1 Addr=0x19 */
        0x09, /* Port  2 ( ge1) ExtBus=0 Addr=0x09 */
        0x0a, /* Port  3 ( ge2) ExtBus=0 Addr=0x0a */
        0x0b, /* Port  4 ( ge3) ExtBus=0 Addr=0x0b */
        0x0c, /* Port  5 ( ge4) ExtBus=0 Addr=0x0c */
        0x0d, /* Port  6 ( ge5) ExtBus=0 Addr=0x0d */
        0x0e, /* Port  7 ( ge6) ExtBus=0 Addr=0x0e */
        0x0f, /* Port  8 ( ge7) ExtBus=0 Addr=0x0f */
        0x10, /* Port  9 ( ge8) ExtBus=0 Addr=0x10 */
        0x15, /* Port 10 ( ge9) ExtBus=0 Addr=0x15 */
        0x16, /* Port 11 (ge10) ExtBus=0 Addr=0x16 */
        0x17, /* Port 12 (ge11) ExtBus=0 Addr=0x17 */
        0x18, /* Port 13 (ge12) ExtBus=0 Addr=0x18 */
        0x00, /* Port 14 ( N/A)                    */
        0x00, /* Port 15 ( N/A)                    */
        0x00, /* Port 16 ( N/A)                    */
        0x00, /* Port 17 ( N/A)                    */
        0x00, /* Port 18 ( N/A)                    */
        0x00, /* Port 19 ( N/A)                    */
        0x00, /* Port 20 ( N/A)                    */
        0x00, /* Port 21 ( N/A)                    */
        0x00, /* Port 22 ( N/A)                    */
        0x00, /* Port 23 ( N/A)                    */
        0x00, /* Port 24 ( N/A)                    */
        0x00, /* Port 25 ( N/A)                    */
        0x41, /* Port 26 ( hg0) ExtBus=2 Addr=0x01 */
        0x42, /* Port 27 ( hg1) ExtBus=2 Addr=0x02 */
        0x43, /* Port 28 ( hg2) ExtBus=2 Addr=0x03 */
        0x44, /* Port 29 ( hg3) ExtBus=2 Addr=0x04 */
        0x01, /* Port 30 (ge25) ExtBus=0 Addr=0x01 */
        0x02, /* Port 31 (ge26) ExtBus=0 Addr=0x02 */
        0x03, /* Port 32 (ge27) ExtBus=0 Addr=0x03 */
        0x04, /* Port 33 (ge28) ExtBus=0 Addr=0x04 */
        0x05, /* Port 34 (ge29) ExtBus=0 Addr=0x05 */
        0x06, /* Port 35 (ge30) ExtBus=0 Addr=0x06 */
        0x07, /* Port 36 (ge31) ExtBus=0 Addr=0x07 */
        0x08, /* Port 37 (ge32) ExtBus=0 Addr=0x08 */
        0x11, /* Port 38 (ge33) ExtBus=0 Addr=0x11 */
        0x12, /* Port 39 (ge34) ExtBus=0 Addr=0x12 */
        0x13, /* Port 40 (ge35) ExtBus=0 Addr=0x13 */
        0x14, /* Port 41 (ge36) ExtBus=0 Addr=0x14 */
        0x45, /* Port 42 (ge37) ExtBus=2 Addr=0x05 */
        0x00, /* Port 43 ( N/A)                    */
        0x00, /* Port 44 ( N/A)                    */
        0x00, /* Port 45 ( N/A)                    */
        0x00, /* Port 46 ( N/A)                    */
        0x00, /* Port 47 ( N/A)                    */
        0x00, /* Port 48 ( N/A)                    */
        0x00, /* Port 49 ( N/A)                    */
        0x46, /* Port 50 (ge45) ExtBus=2 Addr=0x06 */
        0x00, /* Port 51 ( N/A)                    */
        0x00, /* Port 52 ( N/A)                    */
        0x00, /* Port 53 ( N/A)                    */
    };

    static const uint16 _soc_phy_addr_bcm56638[] = {
        0x00, /* Port  0 (cmic) N/A */
        0x19, /* Port  1 ( ge0) ExtBus=1 Addr=0x19 */
        0x00, /* Port  2 ( N/A)                    */
        0x00, /* Port  3 ( N/A)                    */
        0x00, /* Port  4 ( N/A)                    */
        0x00, /* Port  5 ( N/A)                    */
        0x00, /* Port  6 ( N/A)                    */
        0x00, /* Port  7 ( N/A)                    */
        0x00, /* Port  8 ( N/A)                    */
        0x00, /* Port  9 ( N/A)                    */
        0x00, /* Port 10 ( N/A)                    */
        0x00, /* Port 11 ( N/A)                    */
        0x00, /* Port 12 ( N/A)                    */
        0x00, /* Port 13 ( N/A)                    */
        0x00, /* Port 14 ( N/A)                    */
        0x00, /* Port 15 ( N/A)                    */
        0x00, /* Port 16 ( N/A)                    */
        0x00, /* Port 17 ( N/A)                    */
        0x00, /* Port 18 ( N/A)                    */
        0x00, /* Port 19 ( N/A)                    */
        0x00, /* Port 20 ( N/A)                    */
        0x00, /* Port 21 ( N/A)                    */
        0x00, /* Port 22 ( N/A)                    */
        0x00, /* Port 23 ( N/A)                    */
        0x00, /* Port 24 ( N/A)                    */
        0x00, /* Port 25 ( N/A)                    */
        0x41, /* Port 26 ( hg0) ExtBus=2 Addr=0x01 */
        0x42, /* Port 27 ( hg1) ExtBus=2 Addr=0x02 */
        0x43, /* Port 28 ( hg2) ExtBus=2 Addr=0x03 */
        0x44, /* Port 29 ( hg3) ExtBus=2 Addr=0x04 */
        0x45, /* Port 30 ( xe0) ExtBus=2 Addr=0x05 */
        0x00, /* Port 31 ( N/A)                    */
        0x00, /* Port 32 ( N/A)                    */
        0x00, /* Port 33 ( N/A)                    */
        0x00, /* Port 34 ( N/A)                    */
        0x00, /* Port 35 ( N/A)                    */
        0x00, /* Port 36 ( N/A)                    */
        0x00, /* Port 37 ( N/A)                    */
        0x46, /* Port 38 ( xe1) ExtBus=2 Addr=0x06 */
        0x00, /* Port 39 ( N/A)                    */
        0x00, /* Port 40 ( N/A)                    */
        0x00, /* Port 41 ( N/A)                    */
        0x47, /* Port 42 ( xe2) ExtBus=2 Addr=0x07 */
        0x00, /* Port 43 ( N/A)                    */
        0x00, /* Port 44 ( N/A)                    */
        0x00, /* Port 45 ( N/A)                    */
        0x00, /* Port 46 ( N/A)                    */
        0x00, /* Port 47 ( N/A)                    */
        0x00, /* Port 48 ( N/A)                    */
        0x00, /* Port 49 ( N/A)                    */
        0x48, /* Port 50 ( xe3) ExtBus=2 Addr=0x08 */
        0x00, /* Port 51 ( N/A)                    */
        0x00, /* Port 52 ( N/A)                    */
        0x00, /* Port 53 ( N/A)                    */
    };

    static const uint16 _soc_phy_addr_bcm56639[] = {
        0x00, /* Port  0 (cmic) N/A */
        0x1d, /* Port  1 ( ge0) ExtBus=0 Addr=0x1d */
        0x01, /* Port  2 ( ge1) ExtBus=0 Addr=0x01 */
        0x02, /* Port  3 ( ge2) ExtBus=0 Addr=0x02 */
        0x03, /* Port  4 ( ge3) ExtBus=0 Addr=0x03 */
        0x04, /* Port  5 ( ge4) ExtBus=0 Addr=0x04 */
        0x05, /* Port  6 ( ge5) ExtBus=0 Addr=0x05 */
        0x06, /* Port  7 ( ge6) ExtBus=0 Addr=0x06 */
        0x07, /* Port  8 ( ge7) ExtBus=0 Addr=0x07 */
        0x08, /* Port  9 ( ge8) ExtBus=0 Addr=0x08 */
        0x09, /* Port 10 ( ge9) ExtBus=0 Addr=0x09 */
        0x0a, /* Port 11 (ge10) ExtBus=0 Addr=0x0a */
        0x0b, /* Port 12 (ge11) ExtBus=0 Addr=0x0b */
        0x0c, /* Port 13 (ge12) ExtBus=0 Addr=0x0c */
        0x0d, /* Port 14 (ge13) ExtBus=0 Addr=0x0d */
        0x0e, /* Port 15 (ge14) ExtBus=0 Addr=0x0e */
        0x0f, /* Port 16 (ge15) ExtBus=0 Addr=0x0f */
        0x10, /* Port 17 (ge16) ExtBus=0 Addr=0x10 */
        0x11, /* Port 18 (ge17) ExtBus=0 Addr=0x11 */
        0x12, /* Port 19 (ge18) ExtBus=0 Addr=0x12 */
        0x13, /* Port 20 (ge19) ExtBus=0 Addr=0x13 */
        0x14, /* Port 21 (ge20) ExtBus=0 Addr=0x14 */
        0x15, /* Port 22 (ge21) ExtBus=0 Addr=0x15 */
        0x16, /* Port 23 (ge22) ExtBus=0 Addr=0x16 */
        0x17, /* Port 24 (ge23) ExtBus=0 Addr=0x17 */
        0x18, /* Port 25 (ge24) ExtBus=0 Addr=0x18 */
        0x45, /* Port 26 ( hg0) ExtBus=2 Addr=0x05 */
        0x46, /* Port 27 ( hg1) ExtBus=2 Addr=0x06 */
        0x47, /* Port 28 ( hg2) ExtBus=2 Addr=0x07 */
        0x48, /* Port 29 ( hg3) ExtBus=2 Addr=0x08 */
        0x41, /* Port 30 ( xe0) ExtBus=2 Addr=0x01 */
        0x00, /* Port 31 ( N/A)                    */
        0x00, /* Port 32 ( N/A)                    */
        0x00, /* Port 33 ( N/A)                    */
        0x00, /* Port 34 ( N/A)                    */
        0x00, /* Port 35 ( N/A)                    */
        0x00, /* Port 36 ( N/A)                    */
        0x00, /* Port 37 ( N/A)                    */
        0x42, /* Port 38 ( xe1) ExtBus=2 Addr=0x02 */
        0x00, /* Port 39 ( N/A)                    */
        0x00, /* Port 40 ( N/A)                    */
        0x00, /* Port 41 ( N/A)                    */
        0x43, /* Port 42 ( xe2) ExtBus=2 Addr=0x03 */
        0x00, /* Port 43 ( N/A)                    */
        0x00, /* Port 44 ( N/A)                    */
        0x00, /* Port 45 ( N/A)                    */
        0x19, /* Port 46 (ge25) ExtBus=0 Addr=0x19 */
        0x1a, /* Port 47 (ge26) ExtBus=0 Addr=0x1a */
        0x1b, /* Port 48 (ge27) ExtBus=0 Addr=0x1b */
        0x1c, /* Port 49 (ge28) ExtBus=0 Addr=0x1c */
        0x44, /* Port 50 ( xe3) ExtBus=2 Addr=0x04 */
        0x00, /* Port 51 ( N/A)                    */
        0x00, /* Port 52 ( N/A)                    */
        0x00, /* Port 53 ( N/A)                    */
    };

    static const uint16 _soc_phy_addr_bcm56521[] = {
        0x00, /* Port  0 (cmic) N/A */
        0x00, /* Port  1 ( N/A)                    */
        0x01, /* Port  2 ( ge0) ExtBus=0 Addr=0x01 */
        0x02, /* Port  3 ( ge1) ExtBus=0 Addr=0x02 */
        0x03, /* Port  4 ( ge2) ExtBus=0 Addr=0x03 */
        0x04, /* Port  5 ( ge3) ExtBus=0 Addr=0x04 */
        0x05, /* Port  6 ( ge4) ExtBus=0 Addr=0x05 */
        0x06, /* Port  7 ( ge5) ExtBus=0 Addr=0x06 */
        0x07, /* Port  8 ( ge6) ExtBus=0 Addr=0x07 */
        0x08, /* Port  9 ( ge7) ExtBus=0 Addr=0x08 */
        0x09, /* Port 10 ( ge8) ExtBus=0 Addr=0x09 */
        0x0a, /* Port 11 ( ge9) ExtBus=0 Addr=0x0a */
        0x0b, /* Port 12 (ge10) ExtBus=0 Addr=0x0b */
        0x0c, /* Port 13 (ge11) ExtBus=0 Addr=0x0c */
        0x0d, /* Port 14 (ge12) ExtBus=0 Addr=0x0d */
        0x0e, /* Port 15 (ge13) ExtBus=0 Addr=0x0e */
        0x0f, /* Port 16 (ge14) ExtBus=0 Addr=0x0f */
        0x10, /* Port 17 (ge15) ExtBus=0 Addr=0x10 */
        0x11, /* Port 18 (ge16) ExtBus=0 Addr=0x11 */
        0x12, /* Port 19 (ge17) ExtBus=0 Addr=0x12 */
        0x13, /* Port 20 (ge18) ExtBus=0 Addr=0x13 */
        0x14, /* Port 21 (ge19) ExtBus=0 Addr=0x14 */
        0x15, /* Port 22 (ge20) ExtBus=0 Addr=0x15 */
        0x16, /* Port 23 (ge21) ExtBus=0 Addr=0x16 */
        0x17, /* Port 24 (ge22) ExtBus=0 Addr=0x17 */
        0x18, /* Port 25 (ge23) ExtBus=0 Addr=0x18 */
        0x41, /* Port 26 ( hg0) ExtBus=2 Addr=0x01 */
        0x42, /* Port 27 ( hg1) ExtBus=2 Addr=0x02 */
        0x43, /* Port 28 ( hg2) ExtBus=2 Addr=0x03 */
        0x44, /* Port 29 ( hg3) ExtBus=2 Addr=0x04 */
        0x00, /* Port 30 ( N/A)                    */
        0x00, /* Port 31 ( N/A)                    */
        0x00, /* Port 32 ( N/A)                    */
        0x00, /* Port 33 ( N/A)                    */
        0x00, /* Port 34 ( N/A)                    */
        0x00, /* Port 35 ( N/A)                    */
        0x00, /* Port 36 ( N/A)                    */
        0x00, /* Port 37 ( N/A)                    */
        0x00, /* Port 38 ( N/A)                    */
        0x00, /* Port 39 ( N/A)                    */
        0x00, /* Port 40 ( N/A)                    */
        0x00, /* Port 41 ( N/A)                    */
        0x00, /* Port 42 ( N/A)                    */
        0x00, /* Port 43 ( N/A)                    */
        0x00, /* Port 44 ( N/A)                    */
        0x00, /* Port 45 ( N/A)                    */
        0x00, /* Port 46 ( N/A)                    */
        0x00, /* Port 47 ( N/A)                    */
        0x00, /* Port 48 ( N/A)                    */
        0x00, /* Port 49 ( N/A)                    */
        0x00, /* Port 50 ( N/A)                    */
        0x00, /* Port 51 ( N/A)                    */
        0x00, /* Port 52 ( N/A)                    */
        0x00, /* Port 53 ( N/A)                    */
    };

    static const uint16 _soc_phy_addr_bcm56526[] = {
        0x00, /* Port  0 (cmic) N/A */
        0x1d, /* Port  1 ( ge0) ExtBus=0 Addr=0x19 */
        0x01, /* Port  2 ( ge1) ExtBus=0 Addr=0x01 */
        0x02, /* Port  3 ( ge2) ExtBus=0 Addr=0x02 */
        0x03, /* Port  4 ( ge3) ExtBus=0 Addr=0x03 */
        0x04, /* Port  5 ( ge4) ExtBus=0 Addr=0x04 */
        0x05, /* Port  6 ( ge5) ExtBus=0 Addr=0x05 */
        0x06, /* Port  7 ( ge6) ExtBus=0 Addr=0x06 */
        0x07, /* Port  8 ( ge7) ExtBus=0 Addr=0x07 */
        0x08, /* Port  9 ( ge8) ExtBus=0 Addr=0x08 */
        0x0d, /* Port 10 (ge13) ExtBus=0 Addr=0x0d */
        0x0e, /* Port 11 (ge14) ExtBus=0 Addr=0x0e */
        0x0f, /* Port 12 (ge15) ExtBus=0 Addr=0x0f */
        0x10, /* Port 13 (ge16) ExtBus=0 Addr=0x10 */
        0x11, /* Port 14 (ge17) ExtBus=0 Addr=0x11 */
        0x12, /* Port 15 (ge18) ExtBus=0 Addr=0x12 */
        0x13, /* Port 16 (ge19) ExtBus=0 Addr=0x13 */
        0x14, /* Port 17 (ge20) ExtBus=0 Addr=0x14 */
        0x15, /* Port 18 (ge21) ExtBus=0 Addr=0x15 */
        0x16, /* Port 19 (ge22) ExtBus=0 Addr=0x16 */
        0x17, /* Port 20 (ge23) ExtBus=0 Addr=0x17 */
        0x18, /* Port 21 (ge24) ExtBus=0 Addr=0x18 */
        0x19, /* Port 22 (ge25) ExtBus=0 Addr=0x19 */
        0x1a, /* Port 23 (ge26) ExtBus=0 Addr=0x1a */
        0x1b, /* Port 24 (ge27) ExtBus=0 Addr=0x1b */
        0x1c, /* Port 25 (ge28) ExtBus=0 Addr=0x1c */
        0x41, /* Port 26 ( hg0) ExtBus=2 Addr=0x01 */
        0x42, /* Port 27 ( hg1) ExtBus=2 Addr=0x02 */
        0x43, /* Port 28 ( hg2) ExtBus=2 Addr=0x03 */
        0x44, /* Port 29 ( hg3) ExtBus=2 Addr=0x04 */
        0x00, /* Port 30 ( N/A)                    */
        0x00, /* Port 31 ( N/A)                    */
        0x00, /* Port 32 ( N/A)                    */
        0x00, /* Port 33 ( N/A)                    */
        0x00, /* Port 34 ( N/A)                    */
        0x00, /* Port 35 ( N/A)                    */
        0x00, /* Port 36 ( N/A)                    */
        0x00, /* Port 37 ( N/A)                    */
        0x09, /* Port 38 ( ge9) ExtBus=0 Addr=0x09 */
        0x0a, /* Port 39 (ge10) ExtBus=0 Addr=0x0a */
        0x0b, /* Port 40 (ge11) ExtBus=0 Addr=0x0b */
        0x0c, /* Port 41 (ge12) ExtBus=0 Addr=0x0c */
        0x00, /* Port 42 ( N/A)                    */
        0x00, /* Port 43 ( N/A)                    */
        0x00, /* Port 44 ( N/A)                    */
        0x00, /* Port 45 ( N/A)                    */
        0x45, /* Port 46 ( xe0) ExtBus=2 Addr=0x05 */
        0x00, /* Port 47 ( N/A)                    */
        0x00, /* Port 48 ( N/A)                    */
        0x00, /* Port 49 ( N/A)                    */
        0x46, /* Port 50 ( xe1) ExtBus=2 Addr=0x06 */
        0x00, /* Port 51 ( N/A)                    */
        0x00, /* Port 52 ( N/A)                    */
        0x00, /* Port 53 ( N/A)                    */
    };

    static const uint16 _soc_phy_addr_bcm56685[] = {
        0x00, /* Port  0 (cmic) N/A */
        0x39, /* Port  1 ( ge0) ExtBus=1 Addr=0x19 */
        0x00, /* Port  2 ( N/A)                    */
        0x00, /* Port  3 ( N/A)                    */
        0x00, /* Port  4 ( N/A)                    */
        0x00, /* Port  5 ( N/A)                    */
        0x00, /* Port  6 ( N/A)                    */
        0x00, /* Port  7 ( N/A)                    */
        0x00, /* Port  8 ( N/A)                    */
        0x00, /* Port  9 ( N/A)                    */
        0x00, /* Port 10 ( N/A)                    */
        0x00, /* Port 11 ( N/A)                    */
        0x00, /* Port 12 ( N/A)                    */
        0x00, /* Port 13 ( N/A)                    */
        0x00, /* Port 14 ( N/A)                    */
        0x00, /* Port 15 ( N/A)                    */
        0x00, /* Port 16 ( N/A)                    */
        0x00, /* Port 17 ( N/A)                    */
        0x00, /* Port 18 ( N/A)                    */
        0x00, /* Port 19 ( N/A)                    */
        0x00, /* Port 20 ( N/A)                    */
        0x00, /* Port 21 ( N/A)                    */
        0x00, /* Port 22 ( N/A)                    */
        0x00, /* Port 23 ( N/A)                    */
        0x00, /* Port 24 ( N/A)                    */
        0x00, /* Port 25 ( N/A)                    */
        0x41, /* Port 26 ( hg0) ExtBus=2 Addr=0x01 */
        0x42, /* Port 27 ( hg1) ExtBus=2 Addr=0x02 */
        0x43, /* Port 28 ( hg2) ExtBus=2 Addr=0x03 */
        0x44, /* Port 29 ( hg3) ExtBus=2 Addr=0x04 */
        0x01, /* Port 30 ( ge1) ExtBus=0 Addr=0x01 */
        0x02, /* Port 31 ( ge2) ExtBus=0 Addr=0x02 */
        0x03, /* Port 32 ( ge3) ExtBus=0 Addr=0x03 */
        0x04, /* Port 33 ( ge4) ExtBus=0 Addr=0x04 */
        0x05, /* Port 34 ( ge5) ExtBus=0 Addr=0x05 */
        0x06, /* Port 35 ( ge6) ExtBus=0 Addr=0x06 */
        0x07, /* Port 36 ( ge7) ExtBus=0 Addr=0x07 */
        0x08, /* Port 37 ( ge8) ExtBus=0 Addr=0x08 */
        0x09, /* Port 38 ( ge9) ExtBus=0 Addr=0x09 */
        0x0a, /* Port 39 (ge10) ExtBus=0 Addr=0x0a */
        0x0b, /* Port 40 (ge11) ExtBus=0 Addr=0x0b */
        0x0c, /* Port 41 (ge12) ExtBus=0 Addr=0x0c */
        0x0d, /* Port 42 (ge13) ExtBus=0 Addr=0x0d */
        0x0e, /* Port 43 (ge14) ExtBus=0 Addr=0x0e */
        0x0f, /* Port 44 (ge15) ExtBus=0 Addr=0x0f */
        0x10, /* Port 45 (ge16) ExtBus=0 Addr=0x10 */
        0x11, /* Port 46 (ge17) ExtBus=0 Addr=0x11 */
        0x12, /* Port 47 (ge18) ExtBus=0 Addr=0x12 */
        0x13, /* Port 48 (ge19) ExtBus=0 Addr=0x13 */
        0x14, /* Port 49 (ge20) ExtBus=0 Addr=0x14 */
        0x15, /* Port 50 (ge21) ExtBus=0 Addr=0x15 */
        0x16, /* Port 51 (ge22) ExtBus=0 Addr=0x16 */
        0x17, /* Port 52 (ge23) ExtBus=0 Addr=0x17 */
        0x18, /* Port 53 (ge24) ExtBus=0 Addr=0x18 */
    };

    uint16 dev_id;
    uint8 rev_id;

    soc_cm_get_id(unit, &dev_id, &rev_id);

    *phy_addr_int = _soc_triumph2_int_phy_addr[port];
    switch (dev_id) {
    case BCM56630_DEVICE_ID:
    case BCM56520_DEVICE_ID:
    case BCM56522_DEVICE_ID:
    case BCM56524_DEVICE_ID:
    case BCM56534_DEVICE_ID:
        *phy_addr = _soc_phy_addr_bcm56630[port];
        break;
    case BCM56636_DEVICE_ID:
        *phy_addr = _soc_phy_addr_bcm56636[port];
        break;
    case BCM56638_DEVICE_ID:
        *phy_addr = _soc_phy_addr_bcm56638[port];
        break;
    case BCM56639_DEVICE_ID:
        *phy_addr = _soc_phy_addr_bcm56639[port];
        break;
    case BCM56521_DEVICE_ID:
        *phy_addr = _soc_phy_addr_bcm56521[port];
        break;
    case BCM56526_DEVICE_ID:
        *phy_addr = _soc_phy_addr_bcm56526[port];
        break;
    case BCM56685_DEVICE_ID:
    case BCM56689_DEVICE_ID:
        *phy_addr = _soc_phy_addr_bcm56685[port];
        break;
    case BCM56634_DEVICE_ID:
    case BCM56538_DEVICE_ID:
    default:
        *phy_addr = _soc_phy_addr_bcm56634[port];
        break;
    }
}
#endif /* BCM_TRIUMPH2_SUPPORT || BCM_APOLLO_SUPPORT || BCM_VALKYRIE2_SUPPORT */

#ifdef BCM_TRIUMPH_SUPPORT
static void
_triumph_phy_addr_default(int unit, int port,
                          uint16 *phy_addr, uint16 *phy_addr_int)
{
    static const uint16 _soc_triumph_int_phy_addr[] = {
        0x00, /* Port  0 (cmic)         N/A */
        0x99, /* Port  1 (serdes2:8)    IntBus=0 Addr=0x19 */
        0xc1, /* Port  2 (hyperlite0:0) IntBus=2 Addr=0x01 */
        0xc2, /* Port  3 (hyperlite0:1) IntBus=2 Addr=0x02 */
        0xc3, /* Port  4 (hyperlite0:2) IntBus=2 Addr=0x03 */
        0xc4, /* Port  5 (hyperlite0:3) IntBus=2 Addr=0x04 */
        0xc5, /* Port  6 (hyperlite1:0) IntBus=2 Addr=0x05 */
        0xc6, /* Port  7 (hyperlite1:1) IntBus=2 Addr=0x06 */
        0x81, /* Port  8 (serdes0:0)    IntBus=0 Addr=0x01 */
        0x82, /* Port  9 (serdes0:1)    IntBus=0 Addr=0x02 */
        0x83, /* Port 10 (serdes0:2)    IntBus=0 Addr=0x03 */
        0x84, /* Port 11 (serdes0:3)    IntBus=0 Addr=0x04 */
        0x85, /* Port 12 (serdes0:4)    IntBus=0 Addr=0x05 */
        0x86, /* Port 13 (serdes0:5)    IntBus=0 Addr=0x06 */
        0xc9, /* Port 14 (hyperlite2:0) IntBus=2 Addr=0x09 */
        0xca, /* Port 15 (hyperlite2:1) IntBus=2 Addr=0x0a */
        0xcb, /* Port 16 (hyperlite2:2) IntBus=2 Addr=0x0b */
        0xcc, /* Port 17 (hyperlite2:3) IntBus=2 Addr=0x0c */
        0xc7, /* Port 18 (hyperlite1:2) IntBus=2 Addr=0x07 */
        0xc8, /* Port 19 (hyperlite1:3) IntBus=2 Addr=0x08 */
        0x87, /* Port 20 (serdes0:6)    IntBus=0 Addr=0x07 */
        0x88, /* Port 21 (serdes0:7)    IntBus=0 Addr=0x08 */
        0x89, /* Port 22 (serdes1:0)    IntBus=0 Addr=0x09 */
        0x8a, /* Port 23 (serdes1:1)    IntBus=0 Addr=0x0a */
        0x8b, /* Port 24 (serdes1:2)    IntBus=0 Addr=0x0b */
        0x8c, /* Port 25 (serdes1:3)    IntBus=0 Addr=0x0c */
        0xcd, /* Port 26 (hyperlite3:0) IntBus=2 Addr=0x0d */
        0xd5, /* Port 27 (hyperlite5:0) IntBus=2 Addr=0x15 */
        0xd9, /* Port 28 (unicore0)     IntBus=2 Addr=0x19 */
        0xda, /* Port 29 (unicore1)     IntBus=2 Addr=0x1a */
        0xdb, /* Port 30 (unicore2)     IntBus=2 Addr=0x1b */
        0xdc, /* Port 31 (unicore3)     IntBus=2 Addr=0x1c */
        0xce, /* Port 32 (hyperlite3:1) IntBus=2 Addr=0x0e */
        0xcf, /* Port 33 (hyperlite3:2) IntBus=2 Addr=0x0f */
        0xd0, /* Port 34 (hyperlite3:3) IntBus=2 Addr=0x10 */
        0xd1, /* Port 35 (hyperlite4:0) IntBus=2 Addr=0x11 */
        0xd2, /* Port 36 (hyperlite4:1) IntBus=2 Addr=0x12 */
        0x8d, /* Port 37 (serdes1:4)    IntBus=0 Addr=0x0d */
        0x8e, /* Port 38 (serdes1:5)    IntBus=0 Addr=0x0e */
        0x8f, /* Port 39 (serdes1:6)    IntBus=0 Addr=0x0f */
        0x90, /* Port 40 (serdes1:7)    IntBus=0 Addr=0x10 */
        0x91, /* Port 41 (serdes2:0)    IntBus=0 Addr=0x11 */
        0x92, /* Port 42 (serdes2:1)    IntBus=0 Addr=0x12 */
        0xd6, /* Port 43 (hyperlite5:1) IntBus=2 Addr=0x16 */
        0xd7, /* Port 44 (hyperlite5:2) IntBus=2 Addr=0x17 */
        0xd8, /* Port 45 (hyperlite5:3) IntBus=2 Addr=0x18 */
        0xd3, /* Port 46 (hyperlite4:2) IntBus=2 Addr=0x13 */
        0xd4, /* Port 47 (hyperlite4:3) IntBus=2 Addr=0x14 */
        0x93, /* Port 48 (serdes2:2)    IntBus=0 Addr=0x13 */
        0x94, /* Port 49 (serdes2:3)    IntBus=0 Addr=0x14 */
        0x95, /* Port 50 (serdes2:4)    IntBus=0 Addr=0x15 */
        0x96, /* Port 51 (serdes2:5)    IntBus=0 Addr=0x16 */
        0x97, /* Port 52 (serdes2:6)    IntBus=0 Addr=0x17 */
        0x98, /* Port 53 (serdes2:7)    IntBus=0 Addr=0x18 */
    };

    static const uint16 _soc_phy_addr_bcm56624[] = {
        0x00, /* Port  0 (cmic) N/A */
        0x19, /* Port  1 ( ge0) ExtBus=1 Addr=0x19 */
        0x01, /* Port  2 ( ge1) ExtBus=0 Addr=0x01 */
        0x02, /* Port  3 ( ge2) ExtBus=0 Addr=0x02 */
        0x03, /* Port  4 ( ge3) ExtBus=0 Addr=0x03 */
        0x04, /* Port  5 ( ge4) ExtBus=0 Addr=0x04 */
        0x05, /* Port  6 ( ge5) ExtBus=0 Addr=0x05 */
        0x06, /* Port  7 ( ge6) ExtBus=0 Addr=0x06 */
        0x09, /* Port  8 ( ge7) ExtBus=0 Addr=0x09 */
        0x0a, /* Port  9 ( ge8) ExtBus=0 Addr=0x0a */
        0x0b, /* Port 10 ( ge9) ExtBus=0 Addr=0x0b */
        0x0c, /* Port 11 (ge10) ExtBus=0 Addr=0x0c */
        0x0d, /* Port 12 (ge11) ExtBus=0 Addr=0x0d */
        0x0e, /* Port 13 (ge12) ExtBus=0 Addr=0x0e */
        0x11, /* Port 14 (ge13) ExtBus=0 Addr=0x11 */
        0x12, /* Port 15 (ge14) ExtBus=0 Addr=0x12 */
        0x13, /* Port 16 (ge15) ExtBus=0 Addr=0x13 */
        0x14, /* Port 17 (ge16) ExtBus=0 Addr=0x14 */
        0x07, /* Port 18 (ge17) ExtBus=0 Addr=0x07 */
        0x08, /* Port 19 (ge18) ExtBus=0 Addr=0x08 */
        0x0f, /* Port 20 (ge19) ExtBus=0 Addr=0x0f */
        0x10, /* Port 21 (ge20) ExtBus=0 Addr=0x10 */
        0x15, /* Port 22 (ge21) ExtBus=0 Addr=0x15 */
        0x16, /* Port 23 (ge22) ExtBus=0 Addr=0x16 */
        0x17, /* Port 24 (ge23) ExtBus=0 Addr=0x17 */
        0x18, /* Port 25 (ge24) ExtBus=0 Addr=0x18 */
        0x25, /* Port 26 (ge25) ExtBus=1 Addr=0x05 */
        0x35, /* Port 27 (ge26) ExtBus=1 Addr=0x15 */
        0x41, /* Port 28 ( xg0) ExtBus=2 Addr=0x01 */
        0x42, /* Port 29 ( xg1) ExtBus=2 Addr=0x02 */
        0x43, /* Port 30 ( xg2) ExtBus=2 Addr=0x03 */
        0x44, /* Port 31 ( xg3) ExtBus=2 Addr=0x04 */
        0x26, /* Port 32 (ge27) ExtBus=1 Addr=0x06 */
        0x27, /* Port 33 (ge28) ExtBus=1 Addr=0x07 */
        0x28, /* Port 34 (ge29) ExtBus=1 Addr=0x08 */
        0x31, /* Port 35 (ge30) ExtBus=1 Addr=0x11 */
        0x32, /* Port 36 (ge31) ExtBus=1 Addr=0x12 */
        0x21, /* Port 37 (ge32) ExtBus=1 Addr=0x01 */
        0x22, /* Port 38 (ge33) ExtBus=1 Addr=0x02 */
        0x23, /* Port 39 (ge34) ExtBus=1 Addr=0x03 */
        0x24, /* Port 40 (ge35) ExtBus=1 Addr=0x04 */
        0x29, /* Port 41 (ge36) ExtBus=1 Addr=0x09 */
        0x2a, /* Port 42 (ge37) ExtBus=1 Addr=0x0a */
        0x36, /* Port 43 (ge38) ExtBus=1 Addr=0x16 */
        0x37, /* Port 44 (ge39) ExtBus=1 Addr=0x17 */
        0x38, /* Port 45 (ge40) ExtBus=1 Addr=0x18 */
        0x33, /* Port 46 (ge41) ExtBus=1 Addr=0x13 */
        0x34, /* Port 47 (ge42) ExtBus=1 Addr=0x14 */
        0x2b, /* Port 48 (ge43) ExtBus=1 Addr=0x0b */
        0x2c, /* Port 49 (ge44) ExtBus=1 Addr=0x0c */
        0x2d, /* Port 50 (ge45) ExtBus=1 Addr=0x0d */
        0x2e, /* Port 51 (ge46) ExtBus=1 Addr=0x0e */
        0x2f, /* Port 52 (ge47) ExtBus=1 Addr=0x0f */
        0x30, /* Port 53 (ge48) ExtBus=1 Addr=0x10 */
    };

    static const uint16 _soc_phy_addr_bcm56626[] = {
        0x00, /* Port  0 (cmic) N/A */
        0x19, /* Port  1 ( ge0) ExtBus=1 Addr=0x19 */
        0x01, /* Port  2 ( ge1) ExtBus=0 Addr=0x01 */
        0x02, /* Port  3 ( ge2) ExtBus=0 Addr=0x02 */
        0x03, /* Port  4 ( ge3) ExtBus=0 Addr=0x03 */
        0x04, /* Port  5 ( ge4) ExtBus=0 Addr=0x04 */
        0x05, /* Port  6 ( ge5) ExtBus=0 Addr=0x05 */
        0x06, /* Port  7 ( ge6) ExtBus=0 Addr=0x06 */
        0x09, /* Port  8 ( ge7) ExtBus=0 Addr=0x09 */
        0x0a, /* Port  9 ( ge8) ExtBus=0 Addr=0x0a */
        0x0b, /* Port 10 ( ge9) ExtBus=0 Addr=0x0b */
        0x0c, /* Port 11 (ge10) ExtBus=0 Addr=0x0c */
        0x0d, /* Port 12 (ge11) ExtBus=0 Addr=0x0d */
        0x0e, /* Port 13 (ge12) ExtBus=0 Addr=0x0e */
        0x11, /* Port 14 (ge13) ExtBus=0 Addr=0x11 */
        0x12, /* Port 15 (ge14) ExtBus=0 Addr=0x12 */
        0x13, /* Port 16 (ge15) ExtBus=0 Addr=0x13 */
        0x14, /* Port 17 (ge16) ExtBus=0 Addr=0x14 */
        0x07, /* Port 18 (ge17) ExtBus=0 Addr=0x07 */
        0x08, /* Port 19 (ge18) ExtBus=0 Addr=0x08 */
        0x0f, /* Port 20 (ge19) ExtBus=0 Addr=0x0f */
        0x10, /* Port 21 (ge20) ExtBus=0 Addr=0x10 */
        0x15, /* Port 22 (ge21) ExtBus=0 Addr=0x15 */
        0x16, /* Port 23 (ge22) ExtBus=0 Addr=0x16 */
        0x17, /* Port 24 (ge23) ExtBus=0 Addr=0x17 */
        0x18, /* Port 25 (ge24) ExtBus=0 Addr=0x18 */
        0x45, /* Port 26 ( xe0) ExtBus=2 Addr=0x05 */
        0x46, /* Port 27 ( xe1) ExtBus=2 Addr=0x06 */
        0x41, /* Port 28 ( hg0) ExtBus=2 Addr=0x01 */
        0x42, /* Port 29 ( hg1) ExtBus=2 Addr=0x02 */
        0x43, /* Port 30 ( hg2) ExtBus=2 Addr=0x03 */
        0x44, /* Port 31 ( hg3) ExtBus=2 Addr=0x04 */
    };

    static const uint16 _soc_phy_addr_bcm56628[] = {
        0x00, /* Port  0 (cmic) N/A */
        0x19, /* Port  1 ( ge0) ExtBus=1 Addr=0x01 */
        0x45, /* Port  2 ( xe0) ExtBus=2 Addr=0x05 */
        0x00, /* Port  3 ( N/A) */
        0x00, /* Port  4 ( N/A) */
        0x00, /* Port  5 ( N/A) */
        0x00, /* Port  6 ( N/A) */
        0x00, /* Port  7 ( N/A) */
        0x00, /* Port  8 ( N/A) */
        0x00, /* Port  9 ( N/A) */
        0x00, /* Port 10 ( N/A) */
        0x00, /* Port 11 ( N/A) */
        0x00, /* Port 12 ( N/A) */
        0x00, /* Port 13 ( N/A) */
        0x46, /* Port 14 ( xe1) ExtBus=2 Addr=0x06 */
        0x00, /* Port 15 ( N/A) */
        0x00, /* Port 16 ( N/A) */
        0x00, /* Port 17 ( N/A) */
        0x00, /* Port 18 ( N/A) */
        0x00, /* Port 19 ( N/A) */
        0x00, /* Port 20 ( N/A) */
        0x00, /* Port 21 ( N/A) */
        0x00, /* Port 22 ( N/A) */
        0x00, /* Port 23 ( N/A) */
        0x00, /* Port 24 ( N/A) */
        0x00, /* Port 25 ( N/A) */
        0x47, /* Port 26 ( xe2) ExtBus=2 Addr=0x07 */
        0x48, /* Port 27 ( xe3) ExtBus=2 Addr=0x08 */
        0x41, /* Port 28 ( hg0) ExtBus=2 Addr=0x01 */
        0x42, /* Port 29 ( hg1) ExtBus=2 Addr=0x02 */
        0x43, /* Port 30 ( hg2) ExtBus=2 Addr=0x03 */
        0x44, /* Port 31 ( hg3) ExtBus=2 Addr=0x04 */
    };

    static const uint16 _soc_phy_addr_bcm56629[] = {
        0x00, /* Port  0 (cmic) N/A */
        0x19, /* Port  1 ( ge0) ExtBus=0 Addr=0x19 */
        0x41, /* Port  2 ( xe0) ExtBus=2 Addr=0x01 */
        0x00, /* Port  3 ( N/A) */
        0x00, /* Port  4 ( N/A) */
        0x00, /* Port  5 ( N/A) */
        0x00, /* Port  6 ( N/A) */
        0x00, /* Port  7 ( N/A) */
        0x01, /* Port  8 ( ge1) ExtBus=0 Addr=0x01 */
        0x02, /* Port  9 ( ge2) ExtBus=0 Addr=0x02 */
        0x03, /* Port 10 ( ge3) ExtBus=0 Addr=0x03 */
        0x04, /* Port 11 ( ge4) ExtBus=0 Addr=0x04 */
        0x05, /* Port 12 ( ge5) ExtBus=0 Addr=0x05 */
        0x06, /* Port 13 ( ge6) ExtBus=0 Addr=0x06 */
        0x42, /* Port 14 ( xe1) ExtBus=2 Addr=0x02 */
        0x00, /* Port 15 ( N/A) */
        0x00, /* Port 16 ( N/A) */
        0x00, /* Port 17 ( N/A) */
        0x00, /* Port 18 ( N/A) */
        0x00, /* Port 19 ( N/A) */
        0x07, /* Port 20 ( ge7) ExtBus=0 Addr=0x07 */
        0x08, /* Port 21 ( ge8) ExtBus=0 Addr=0x08 */
        0x09, /* Port 22 ( ge9) ExtBus=0 Addr=0x09 */
        0x0a, /* Port 23 (ge10) ExtBus=0 Addr=0x0a */
        0x0b, /* Port 24 (ge11) ExtBus=0 Addr=0x0b */
        0x0c, /* Port 25 (ge12) ExtBus=0 Addr=0x0c */
        0x43, /* Port 26 ( xe2) ExtBus=2 Addr=0x03 */
        0x44, /* Port 27 ( xe3) ExtBus=2 Addr=0x04 */
        0x45, /* Port 28 ( hg0) ExtBus=2 Addr=0x05 */
        0x46, /* Port 29 ( hg1) ExtBus=2 Addr=0x06 */
        0x47, /* Port 30 ( hg2) ExtBus=2 Addr=0x07 */
        0x48, /* Port 31 ( hg3) ExtBus=2 Addr=0x08 */
        0x00, /* Port 32 ( N/A) */
        0x00, /* Port 33 ( N/A) */
        0x00, /* Port 34 ( N/A) */
        0x00, /* Port 35 ( N/A) */
        0x00, /* Port 36 ( N/A) */
        0x0d, /* Port 37 (ge13) ExtBus=0 Addr=0x0d */
        0x0e, /* Port 38 (ge14) ExtBus=0 Addr=0x0e */
        0x0f, /* Port 39 (ge15) ExtBus=0 Addr=0x0f */
        0x10, /* Port 40 (ge16) ExtBus=0 Addr=0x10 */
        0x11, /* Port 41 (ge17) ExtBus=0 Addr=0x11 */
        0x12, /* Port 42 (ge18) ExtBus=0 Addr=0x12 */
        0x00, /* Port 43 ( N/A) */
        0x00, /* Port 44 ( N/A) */
        0x00, /* Port 45 ( N/A) */
        0x00, /* Port 46 ( N/A) */
        0x00, /* Port 47 ( N/A) */
        0x13, /* Port 48 (ge19) ExtBus=0 Addr=0x13 */
        0x14, /* Port 49 (ge20) ExtBus=0 Addr=0x14 */
        0x15, /* Port 50 (ge21) ExtBus=0 Addr=0x15 */
        0x16, /* Port 51 (ge22) ExtBus=0 Addr=0x16 */
        0x17, /* Port 52 (ge23) ExtBus=0 Addr=0x17 */
        0x18, /* Port 53 (ge24) ExtBus=0 Addr=0x18 */
    };

    uint16 dev_id;
    uint8 rev_id;

    soc_cm_get_id(unit, &dev_id, &rev_id);

    *phy_addr_int = _soc_triumph_int_phy_addr[port];
    switch (dev_id) {
    case BCM56626_DEVICE_ID:
        *phy_addr = _soc_phy_addr_bcm56626[port];
        break;
    case BCM56628_DEVICE_ID:
        *phy_addr = _soc_phy_addr_bcm56628[port];
        break;
    case BCM56629_DEVICE_ID:
        if (soc_feature(unit, soc_feature_xgport_one_xe_six_ge)) {
            if (soc_property_get(unit, spn_BCM56629_40GE, 0)) {
                *phy_addr = _soc_phy_addr_bcm56624[port];
            } else {
                *phy_addr = _soc_phy_addr_bcm56629[port];
            }
        } else {
            *phy_addr = _soc_phy_addr_bcm56626[port];
        }
        break;
    case BCM56624_DEVICE_ID:
    default:
        *phy_addr = _soc_phy_addr_bcm56624[port];
        break;
    }
}
#endif /* BCM_TRIUMPH_SUPPORT */

#ifdef BCM_VALKYRIE_SUPPORT
static void
_valkyrie_phy_addr_default(int unit, int port,
                           uint16 *phy_addr, uint16 *phy_addr_int)
{
    static const uint16 _soc_valkyrie_int_phy_addr[] = {
        0x00, /* Port  0 (cmic)         N/A */
        0x99, /* Port  1 (serdes2:8)    IntBus=0 Addr=0x19 */
        0xc1, /* Port  2 (hyperlite0:0) IntBus=2 Addr=0x01 */
        0xc2, /* Port  3 (hyperlite0:1) IntBus=2 Addr=0x02 */
        0xc3, /* Port  4 (hyperlite0:2) IntBus=2 Addr=0x03 */
        0xc4, /* Port  5 (hyperlite0:3) IntBus=2 Addr=0x04 */
        0xc5, /* Port  6 (hyperlite1:0) IntBus=2 Addr=0x05 */
        0xc6, /* Port  7 (hyperlite1:1) IntBus=2 Addr=0x06 */
        0x81, /* Port  8 (N/A) */
        0x82, /* Port  9 (N/A) */
        0x83, /* Port 10 (N/A) */
        0x84, /* Port 11 (N/A) */
        0x85, /* Port 12 (N/A) */
        0x86, /* Port 13 (N/A) */
        0xc9, /* Port 14 (hyperlite2:0) IntBus=2 Addr=0x09 */
        0xca, /* Port 15 (hyperlite2:1) IntBus=2 Addr=0x0a */
        0xcb, /* Port 16 (hyperlite2:2) IntBus=2 Addr=0x0b */
        0xcc, /* Port 17 (hyperlite2:3) IntBus=2 Addr=0x0c */
        0xc7, /* Port 18 (hyperlite1:2) IntBus=2 Addr=0x07 */
        0xc8, /* Port 19 (hyperlite1:3) IntBus=2 Addr=0x08 */
        0x87, /* Port 20 (N/A) */
        0x88, /* Port 21 (N/A) */
        0x89, /* Port 22 (N/A) */
        0x8a, /* Port 23 (N/A) */
        0x8b, /* Port 24 (N/A) */
        0x8c, /* Port 25 (N/A) */
        0xcd, /* Port 26 (hyperlite3:0) IntBus=2 Addr=0x0d */
        0xd5, /* Port 27 (hyperlite5:0) IntBus=2 Addr=0x15 */
        0xd9, /* Port 28 (unicore0)     IntBus=2 Addr=0x19 */
        0xda, /* Port 29 (unicore1)     IntBus=2 Addr=0x1a */
        0xdb, /* Port 30 (unicore2)     IntBus=2 Addr=0x1b */
        0xdc, /* Port 31 (unicore3)     IntBus=2 Addr=0x1c */
        0xce, /* Port 32 (hyperlite3:1) IntBus=2 Addr=0x0e */
        0xcf, /* Port 33 (hyperlite3:2) IntBus=2 Addr=0x0f */
        0xd0, /* Port 34 (hyperlite3:3) IntBus=2 Addr=0x10 */
        0xd1, /* Port 35 (hyperlite4:0) IntBus=2 Addr=0x11 */
        0xd2, /* Port 36 (hyperlite4:1) IntBus=2 Addr=0x12 */
        0x8d, /* Port 37 (N/A) */
        0x8e, /* Port 38 (N/A) */
        0x8f, /* Port 39 (N/A) */
        0x90, /* Port 40 (N/A) */
        0x91, /* Port 41 (N/A) */
        0x92, /* Port 42 (N/A) */
        0xd6, /* Port 43 (hyperlite5:1) IntBus=2 Addr=0x16 */
        0xd7, /* Port 44 (hyperlite5:2) IntBus=2 Addr=0x17 */
        0xd8, /* Port 45 (hyperlite5:3) IntBus=2 Addr=0x18 */
        0xd3, /* Port 46 (hyperlite4:2) IntBus=2 Addr=0x13 */
        0xd4, /* Port 47 (hyperlite4:3) IntBus=2 Addr=0x14 */
        0x93, /* Port 48 (N/A) */
        0x94, /* Port 49 (N/A) */
        0x95, /* Port 50 (N/A) */
        0x96, /* Port 51 (N/A) */
        0x97, /* Port 52 (N/A) */
        0x98, /* Port 53 (N/A) */
    };

    static const uint16 _soc_phy_addr_bcm56680[] = {
        0x00, /* Port  0 (cmic) N/A */
        0x39, /* Port  1 ( ge0) ExtBus=1 Addr=0x19 */
        0x01, /* Port  2 ( ge1) ExtBus=0 Addr=0x01 */
        0x02, /* Port  3 ( ge2) ExtBus=0 Addr=0x02 */
        0x03, /* Port  4 ( ge3) ExtBus=0 Addr=0x03 */
        0x04, /* Port  5 ( ge4) ExtBus=0 Addr=0x04 */
        0x05, /* Port  6 ( ge5) ExtBus=0 Addr=0x05 */
        0x06, /* Port  7 ( ge6) ExtBus=0 Addr=0x06 */
        0x00, /* Port  8 ( N/A) */
        0x00, /* Port  9 ( N/A) */
        0x00, /* Port 10 ( N/A) */
        0x00, /* Port 11 ( N/A) */
        0x00, /* Port 12 ( N/A) */
        0x00, /* Port 13 ( N/A) */
        0x07, /* Port 14 ( ge7) ExtBus=0 Addr=0x07 */
        0x08, /* Port 15 ( ge8) ExtBus=0 Addr=0x08 */
        0x09, /* Port 16 ( ge9) ExtBus=0 Addr=0x09 */
        0x0a, /* Port 17 (ge10) ExtBus=0 Addr=0x0a */
        0x0b, /* Port 18 (ge11) ExtBus=0 Addr=0x0b */
        0x0c, /* Port 19 (ge12) ExtBus=0 Addr=0x0c */
        0x00, /* Port 20 ( N/A) */
        0x00, /* Port 21 ( N/A) */
        0x00, /* Port 22 ( N/A) */
        0x00, /* Port 23 ( N/A) */
        0x00, /* Port 24 ( N/A) */
        0x00, /* Port 25 ( N/A) */
        0x0d, /* Port 26 (ge13) ExtBus=0 Addr=0x0d */
        0x13, /* Port 27 (ge19) ExtBus=0 Addr=0x13 */
        0x41, /* Port 28 ( hg0) ExtBus=2 Addr=0x01 */
        0x42, /* Port 29 ( hg1) ExtBus=2 Addr=0x02 */
        0x43, /* Port 30 ( hg2) ExtBus=2 Addr=0x03 */
        0x44, /* Port 31 ( hg3) ExtBus=2 Addr=0x04 */
        0x0e, /* Port 32 (ge14) ExtBus=0 Addr=0x0e */
        0x0f, /* Port 33 (ge15) ExtBus=0 Addr=0x0f */
        0x10, /* Port 34 (ge16) ExtBus=0 Addr=0x10 */
        0x11, /* Port 35 (ge17) ExtBus=0 Addr=0x11 */
        0x12, /* Port 36 (ge18) ExtBus=0 Addr=0x12 */
        0x00, /* Port 37 ( N/A) */
        0x00, /* Port 38 ( N/A) */
        0x00, /* Port 39 ( N/A) */
        0x00, /* Port 40 ( N/A) */
        0x00, /* Port 41 ( N/A) */
        0x00, /* Port 42 ( N/A) */
        0x14, /* Port 43 (ge20) ExtBus=0 Addr=0x14 */
        0x15, /* Port 44 (ge21) ExtBus=0 Addr=0x15 */
        0x16, /* Port 45 (ge22) ExtBus=0 Addr=0x16 */
        0x17, /* Port 46 (ge23) ExtBus=0 Addr=0x17 */
        0x18, /* Port 47 (ge24) ExtBus=0 Addr=0x18 */
        0x00, /* Port 48 ( N/A) */
        0x00, /* Port 49 ( N/A) */
        0x00, /* Port 50 ( N/A) */
        0x00, /* Port 51 ( N/A) */
        0x00, /* Port 52 ( N/A) */
        0x00, /* Port 53 ( N/A) */
    };

    static const uint16 _soc_phy_addr_bcm56684[] = {
        0x00, /* Port  0 (cmic) N/A */
        0x00, /* Port  1 ( N/A) */
        0x01, /* Port  2 ( ge0) ExtBus=0 Addr=0x01 */
        0x02, /* Port  3 ( ge1) ExtBus=0 Addr=0x02 */
        0x03, /* Port  4 ( ge2) ExtBus=0 Addr=0x03 */
        0x04, /* Port  5 ( ge3) ExtBus=0 Addr=0x04 */
        0x05, /* Port  6 ( ge4) ExtBus=0 Addr=0x05 */
        0x06, /* Port  7 ( ge5) ExtBus=0 Addr=0x06 */
        0x00, /* Port  8 ( N/A) */
        0x00, /* Port  9 ( N/A) */
        0x00, /* Port 10 ( N/A) */
        0x00, /* Port 11 ( N/A) */
        0x00, /* Port 12 ( N/A) */
        0x00, /* Port 13 ( N/A) */
        0x09, /* Port 14 ( ge6) ExtBus=0 Addr=0x09 */
        0x0a, /* Port 15 ( ge7) ExtBus=0 Addr=0x0a */
        0x0b, /* Port 16 ( ge8) ExtBus=0 Addr=0x0b */
        0x0c, /* Port 17 ( ge9) ExtBus=0 Addr=0x0c */
        0x07, /* Port 18 (ge10) ExtBus=0 Addr=0x07 */
        0x08, /* Port 19 (ge11) ExtBus=0 Addr=0x08 */
        0x00, /* Port 20 ( N/A) */
        0x00, /* Port 21 ( N/A) */
        0x00, /* Port 22 ( N/A) */
        0x00, /* Port 23 ( N/A) */
        0x00, /* Port 24 ( N/A) */
        0x00, /* Port 25 ( N/A) */
        0x0d, /* Port 26 (ge12) ExtBus=0 Addr=0x0d */
        0x15, /* Port 27 (ge13) ExtBus=0 Addr=0x15 */
        0x41, /* Port 28 ( hg0) ExtBus=2 Addr=0x01 */
        0x42, /* Port 29 ( hg1) ExtBus=2 Addr=0x02 */
        0x43, /* Port 30 ( hg2) ExtBus=2 Addr=0x03 */
        0x44, /* Port 31 ( hg3) ExtBus=2 Addr=0x04 */
        0x0e, /* Port 32 (ge14) ExtBus=0 Addr=0x0e */
        0x0f, /* Port 33 (ge15) ExtBus=0 Addr=0x0f */
        0x10, /* Port 34 (ge16) ExtBus=0 Addr=0x10 */
        0x11, /* Port 35 (ge17) ExtBus=0 Addr=0x11 */
        0x12, /* Port 36 (ge18) ExtBus=0 Addr=0x12 */
        0x00, /* Port 37 ( N/A) */
        0x00, /* Port 38 ( N/A) */
        0x00, /* Port 39 ( N/A) */
        0x00, /* Port 40 ( N/A) */
        0x00, /* Port 41 ( N/A) */
        0x00, /* Port 42 ( N/A) */
        0x16, /* Port 43 (ge19) ExtBus=0 Addr=0x16 */
        0x17, /* Port 44 (ge20) ExtBus=0 Addr=0x17 */
        0x18, /* Port 45 (ge21) ExtBus=0 Addr=0x18 */
        0x13, /* Port 46 (ge22) ExtBus=0 Addr=0x13 */
        0x14, /* Port 47 (ge23) ExtBus=0 Addr=0x14 */
        0x00, /* Port 48 ( N/A) */
        0x00, /* Port 49 ( N/A) */
        0x00, /* Port 50 ( N/A) */
        0x00, /* Port 51 ( N/A) */
        0x00, /* Port 52 ( N/A) */
        0x00, /* Port 53 ( N/A) */
    };

    static const uint16 _soc_phy_addr_bcm56686[] = {
        0x00, /* Port  0 (cmic) N/A */
        0x00, /* Port  1 ( N/A) */
        0x45, /* Port  2 ( xe0) ExtBus=2 Addr=0x05 */
        0x00, /* Port  3 ( N/A) */
        0x00, /* Port  4 ( N/A) */
        0x00, /* Port  5 ( N/A) */
        0x00, /* Port  6 ( N/A) */
        0x00, /* Port  7 ( N/A) */
        0x00, /* Port  8 ( N/A) */
        0x00, /* Port  9 ( N/A) */
        0x00, /* Port 10 ( N/A) */
        0x00, /* Port 11 ( N/A) */
        0x00, /* Port 12 ( N/A) */
        0x00, /* Port 13 ( N/A) */
        0x00, /* Port 14 ( N/A) */
        0x00, /* Port 15 ( N/A) */
        0x00, /* Port 16 ( N/A) */
        0x00, /* Port 17 ( N/A) */
        0x00, /* Port 18 ( N/A) */
        0x00, /* Port 19 ( N/A) */
        0x00, /* Port 20 ( N/A) */
        0x00, /* Port 21 ( N/A) */
        0x00, /* Port 22 ( N/A) */
        0x00, /* Port 23 ( N/A) */
        0x00, /* Port 24 ( N/A) */
        0x00, /* Port 25 ( N/A) */
        0x00, /* Port 26 ( N/A) */
        0x46, /* Port 27 ( xe1) ExtBus=2 Addr=0x06 */
        0x41, /* Port 28 ( hg0) ExtBus=2 Addr=0x01 */
        0x42, /* Port 29 ( hg1) ExtBus=2 Addr=0x02 */
        0x43, /* Port 30 ( hg2) ExtBus=2 Addr=0x03 */
        0x44, /* Port 31 ( hg3) ExtBus=2 Addr=0x04 */
    };

    static const uint16 _soc_phy_addr_bcm56620[] = {
        0x00, /* Port  0 (cmic) N/A */
        0x00, /* Port  1 ( N/A) */
        0x01, /* Port  2 ( ge0) ExtBus=0 Addr=0x01 */
        0x02, /* Port  3 ( ge1) ExtBus=0 Addr=0x02 */
        0x03, /* Port  4 ( ge2) ExtBus=0 Addr=0x03 */
        0x04, /* Port  5 ( ge3) ExtBus=0 Addr=0x04 */
        0x05, /* Port  6 ( ge4) ExtBus=0 Addr=0x05 */
        0x06, /* Port  7 ( ge5) ExtBus=0 Addr=0x06 */
        0x00, /* Port  8 ( N/A) */
        0x00, /* Port  9 ( N/A) */
        0x00, /* Port 10 ( N/A) */
        0x00, /* Port 11 ( N/A) */
        0x00, /* Port 12 ( N/A) */
        0x00, /* Port 13 ( N/A) */
        0x09, /* Port 14 ( ge6) ExtBus=0 Addr=0x09 */
        0x0a, /* Port 15 ( ge7) ExtBus=0 Addr=0x0a */
        0x0b, /* Port 16 ( ge8) ExtBus=0 Addr=0x0b */
        0x0c, /* Port 17 ( ge9) ExtBus=0 Addr=0x0c */
        0x07, /* Port 18 (ge10) ExtBus=0 Addr=0x07 */
        0x08, /* Port 19 (ge11) ExtBus=0 Addr=0x08 */
        0x00, /* Port 20 ( N/A) */
        0x00, /* Port 21 ( N/A) */
        0x00, /* Port 22 ( N/A) */
        0x00, /* Port 23 ( N/A) */
        0x00, /* Port 24 ( N/A) */
        0x00, /* Port 25 ( N/A) */
        0x0d, /* Port 26 (ge12) ExtBus=0 Addr=0x0d */
        0x15, /* Port 27 (ge13) ExtBus=0 Addr=0x15 */
        0x41, /* Port 28 ( hg0) ExtBus=2 Addr=0x01 */
        0x42, /* Port 29 ( hg1) ExtBus=2 Addr=0x02 */
        0x43, /* Port 30 ( hg2) ExtBus=2 Addr=0x03 */
        0x44, /* Port 31 ( hg3) ExtBus=2 Addr=0x04 */
        0x0e, /* Port 32 (ge14) ExtBus=0 Addr=0x0e */
        0x0f, /* Port 33 (ge15) ExtBus=0 Addr=0x0f */
        0x10, /* Port 34 (ge16) ExtBus=0 Addr=0x10 */
        0x11, /* Port 35 (ge17) ExtBus=0 Addr=0x11 */
        0x12, /* Port 36 (ge18) ExtBus=0 Addr=0x12 */
        0x00, /* Port 37 ( N/A) */
        0x00, /* Port 38 ( N/A) */
        0x00, /* Port 39 ( N/A) */
        0x00, /* Port 40 ( N/A) */
        0x00, /* Port 41 ( N/A) */
        0x00, /* Port 42 ( N/A) */
        0x16, /* Port 43 (ge19) ExtBus=0 Addr=0x16 */
        0x17, /* Port 44 (ge20) ExtBus=0 Addr=0x17 */
        0x18, /* Port 45 (ge21) ExtBus=0 Addr=0x18 */
        0x13, /* Port 46 (ge22) ExtBus=0 Addr=0x13 */
        0x14, /* Port 47 (ge23) ExtBus=0 Addr=0x14 */
        0x00, /* Port 48 ( N/A) */
        0x00, /* Port 49 ( N/A) */
        0x00, /* Port 50 ( N/A) */
        0x00, /* Port 51 ( N/A) */
        0x00, /* Port 52 ( N/A) */
        0x00, /* Port 53 ( N/A) */
    };
    uint16 dev_id;
    uint8 rev_id;

    soc_cm_get_id(unit, &dev_id, &rev_id);

    *phy_addr_int = _soc_valkyrie_int_phy_addr[port];
    switch (dev_id) {
    case BCM56620_DEVICE_ID:
        *phy_addr = _soc_phy_addr_bcm56620[port];
        break;
    case BCM56684_DEVICE_ID:
        *phy_addr = _soc_phy_addr_bcm56684[port];
        break;
    case BCM56686_DEVICE_ID:
        *phy_addr = _soc_phy_addr_bcm56686[port];
        break;
    case BCM56680_DEVICE_ID:
    default:
        *phy_addr = _soc_phy_addr_bcm56680[port];
        break;
    }
}
#endif /* BCM_VALKYRIE_SUPPORT */

#ifdef BCM_PETRA_SUPPORT
int
_dpp_phy_addr_multi_get(int unit, soc_port_t port, int is_logical, int array_max,
                       int *array_size, phyident_core_info_t *core_info)
{
    static const uint16 _soc_phy_addr_arad[] = {
        (0x00),                                 /* Phy Port  0 (first phy port is CMIC)*/
        ((3 << PHY_ID_BUS_LOWER_SHIFT) | 0x00 | 0x80), /* Phy Port  1 (WC0 lane 0)  Bus=3 Phy=0  */
        ((3 << PHY_ID_BUS_LOWER_SHIFT) | 0x00 | 0x80), /* Phy Port  2 (WC0 lane 1)  Bus=3 Phy=0  */
        ((3 << PHY_ID_BUS_LOWER_SHIFT) | 0x00 | 0x80), /* Phy Port  3 (WC0 lane 2)  Bus=3 Phy=0  */
        ((3 << PHY_ID_BUS_LOWER_SHIFT) | 0x00 | 0x80), /* Phy Port  4 (WC0 lane 3)  Bus=3 Phy=0  */
        ((3 << PHY_ID_BUS_LOWER_SHIFT) | 0x04 | 0x80), /* Phy Port  5 (WC1 lane 0)  Bus=3 Phy=4  */
        ((3 << PHY_ID_BUS_LOWER_SHIFT) | 0x04 | 0x80), /* Phy Port  6 (WC1 lane 1)  Bus=3 Phy=4  */
        ((3 << PHY_ID_BUS_LOWER_SHIFT) | 0x04 | 0x80), /* Phy Port  7 (WC1 lane 2)  Bus=3 Phy=4  */
        ((3 << PHY_ID_BUS_LOWER_SHIFT) | 0x04 | 0x80), /* Phy Port  8 (WC1 lane 3)  Bus=3 Phy=4  */
        ((3 << PHY_ID_BUS_LOWER_SHIFT) | 0x08 | 0x80), /* Phy Port  9 (WC2 lane 0)  Bus=3 Phy=8  */
        ((3 << PHY_ID_BUS_LOWER_SHIFT) | 0x08 | 0x80), /* Phy Port 10 (WC2 lane 1)  Bus=3 Phy=8  */
        ((3 << PHY_ID_BUS_LOWER_SHIFT) | 0x08 | 0x80), /* Phy Port 11 (WC2 lane 2)  Bus=3 Phy=8  */
        ((3 << PHY_ID_BUS_LOWER_SHIFT) | 0x08 | 0x80), /* Phy Port 12 (WC2 lane 3)  Bus=3 Phy=8  */
        ((3 << PHY_ID_BUS_LOWER_SHIFT) | 0x0c | 0x80), /* Phy Port 13 (WC3 lane 0)  Bus=3 Phy=12 */
        ((3 << PHY_ID_BUS_LOWER_SHIFT) | 0x0c | 0x80), /* Phy Port 14 (WC3 lane 1)  Bus=3 Phy=12 */
        ((3 << PHY_ID_BUS_LOWER_SHIFT) | 0x0c | 0x80), /* Phy Port 15 (WC3 lane 2)  Bus=3 Phy=12 */
        ((3 << PHY_ID_BUS_LOWER_SHIFT) | 0x0c | 0x80), /* Phy Port 16 (WC3 lane 3)  Bus=3 Phy=12 */
        ((3 << PHY_ID_BUS_LOWER_SHIFT) | 0x10 | 0x80), /* Phy Port 17 (WC4 lane 0)  Bus=3 Phy=16 */
        ((3 << PHY_ID_BUS_LOWER_SHIFT) | 0x10 | 0x80), /* Phy Port 18 (WC4 lane 1)  Bus=3 Phy=16 */
        ((3 << PHY_ID_BUS_LOWER_SHIFT) | 0x10 | 0x80), /* Phy Port 19 (WC4 lane 2)  Bus=3 Phy=16 */
        ((3 << PHY_ID_BUS_LOWER_SHIFT) | 0x10 | 0x80), /* Phy Port 20 (WC4 lane 3)  Bus=3 Phy=16 */
        ((3 << PHY_ID_BUS_LOWER_SHIFT) | 0x14 | 0x80), /* Phy Port 21 (WC5 lane 0)  Bus=3 Phy=20 */
        ((3 << PHY_ID_BUS_LOWER_SHIFT) | 0x14 | 0x80), /* Phy Port 22 (WC5 lane 1)  Bus=3 Phy=20 */
        ((3 << PHY_ID_BUS_LOWER_SHIFT) | 0x14 | 0x80), /* Phy Port 23 (WC5 lane 2)  Bus=3 Phy=20 */
        ((3 << PHY_ID_BUS_LOWER_SHIFT) | 0x14 | 0x80), /* Phy Port 24 (WC5 lane 3)  Bus=3 Phy=20 */
        ((3 << PHY_ID_BUS_LOWER_SHIFT) | 0x18 | 0x80), /* Phy Port 25 (WC6 lane 0)  Bus=3 Phy=24 */
        ((3 << PHY_ID_BUS_LOWER_SHIFT) | 0x18 | 0x80), /* Phy Port 26 (WC6 lane 1)  Bus=3 Phy=24 */
        ((3 << PHY_ID_BUS_LOWER_SHIFT) | 0x18 | 0x80), /* Phy Port 27 (WC6 lane 2)  Bus=3 Phy=24 */
        ((3 << PHY_ID_BUS_LOWER_SHIFT) | 0x18 | 0x80), /* Phy Port 28 (WC6 lane 3)  Bus=3 Phy=24 */
        ((2 << PHY_ID_BUS_LOWER_SHIFT) | 0x0c | 0x80), /* Phy Port 29 (WC7 lane 0)  Bus=2 Phy=12 */
        ((2 << PHY_ID_BUS_LOWER_SHIFT) | 0x0c | 0x80), /* Phy Port 30 (WC7 lane 1)  Bus=2 Phy=12 */
        ((2 << PHY_ID_BUS_LOWER_SHIFT) | 0x0c | 0x80), /* Phy Port 31 (WC7 lane 2)  Bus=2 Phy=12 */
        ((2 << PHY_ID_BUS_LOWER_SHIFT) | 0x0c | 0x80), /* Phy Port 32 (WC7 lane 3)  Bus=2 Phy=12 */
        ((3 << PHY_ID_BUS_LOWER_SHIFT) | 0x1c | 0x80), /* Phy Port 33 (MLD 0)       Bus=3 Phy=26 */
        ((3 << PHY_ID_BUS_LOWER_SHIFT) | 0x1d | 0x80), /* Phy Port 34 (MLD 1)       Bus=3 Phy=27 */
        0x00, /* Phy Port 35 (N/A) */
        0x00, /* Phy Port 36 (N/A) */
        0x00, /* Phy Port 37 (N/A) */
        0x00, /* Phy Port 38 (N/A) */
        0x00, /* Phy Port 39 (N/A) */
        ((1 << PHY_ID_BUS_LOWER_SHIFT) | 0x00 | 0x80), /* Phy Port 40 (Fabric Link0)  Bus=1 Phy=0  */
        ((1 << PHY_ID_BUS_LOWER_SHIFT) | 0x00 | 0x80), /* Phy Port 41 (Fabric Link1)  Bus=1 Phy=0  */
        ((1 << PHY_ID_BUS_LOWER_SHIFT) | 0x00 | 0x80), /* Phy Port 42 (Fabric Link2)  Bus=1 Phy=0  */
        ((1 << PHY_ID_BUS_LOWER_SHIFT) | 0x00 | 0x80), /* Phy Port 43 (Fabric Link3)  Bus=1 Phy=0  */
        ((1 << PHY_ID_BUS_LOWER_SHIFT) | 0x04 | 0x80), /* Phy Port 44 (Fabric Link4)  Bus=1 Phy=4  */
        ((1 << PHY_ID_BUS_LOWER_SHIFT) | 0x04 | 0x80), /* Phy Port 45 (Fabric Link5)  Bus=1 Phy=4  */
        ((1 << PHY_ID_BUS_LOWER_SHIFT) | 0x04 | 0x80), /* Phy Port 46 (Fabric Link6)  Bus=1 Phy=4  */
        ((1 << PHY_ID_BUS_LOWER_SHIFT) | 0x04 | 0x80), /* Phy Port 47 (Fabric Link7)  Bus=1 Phy=4  */
        ((1 << PHY_ID_BUS_LOWER_SHIFT) | 0x08 | 0x80), /* Phy Port 48 (Fabric Link8)  Bus=1 Phy=8  */
        ((1 << PHY_ID_BUS_LOWER_SHIFT) | 0x08 | 0x80), /* Phy Port 49 (Fabric Link9)  Bus=1 Phy=8  */
        ((1 << PHY_ID_BUS_LOWER_SHIFT) | 0x08 | 0x80), /* Phy Port 50 (Fabric Link10) Bus=1 Phy=8  */
        ((1 << PHY_ID_BUS_LOWER_SHIFT) | 0x08 | 0x80), /* Phy Port 51 (Fabric Link11) Bus=1 Phy=8  */
        ((1 << PHY_ID_BUS_LOWER_SHIFT) | 0x10 | 0x80), /* Phy Port 52 (Fabric Link12) Bus=1 Phy=16 */
        ((1 << PHY_ID_BUS_LOWER_SHIFT) | 0x10 | 0x80), /* Phy Port 53 (Fabric Link13) Bus=1 Phy=16 */
        ((1 << PHY_ID_BUS_LOWER_SHIFT) | 0x10 | 0x80), /* Phy Port 54 (Fabric Link14) Bus=1 Phy=16 */
        ((1 << PHY_ID_BUS_LOWER_SHIFT) | 0x10 | 0x80), /* Phy Port 55 (Fabric Link15) Bus=1 Phy=16 */
        ((1 << PHY_ID_BUS_LOWER_SHIFT) | 0x14 | 0x80), /* Phy Port 56 (Fabric Link16) Bus=1 Phy=20 */
        ((1 << PHY_ID_BUS_LOWER_SHIFT) | 0x14 | 0x80), /* Phy Port 57 (Fabric Link17) Bus=1 Phy=20 */
        ((1 << PHY_ID_BUS_LOWER_SHIFT) | 0x14 | 0x80), /* Phy Port 58 (Fabric Link18) Bus=1 Phy=20 */
        ((1 << PHY_ID_BUS_LOWER_SHIFT) | 0x14 | 0x80), /* Phy Port 59 (Fabric Link19) Bus=1 Phy=20 */
        ((1 << PHY_ID_BUS_LOWER_SHIFT) | 0x18 | 0x80), /* Phy Port 60 (Fabric Link20) Bus=1 Phy=24 */
        ((1 << PHY_ID_BUS_LOWER_SHIFT) | 0x18 | 0x80), /* Phy Port 61 (Fabric Link21) Bus=1 Phy=24 */
        ((1 << PHY_ID_BUS_LOWER_SHIFT) | 0x18 | 0x80), /* Phy Port 62 (Fabric Link22) Bus=1 Phy=24 */
        ((1 << PHY_ID_BUS_LOWER_SHIFT) | 0x18 | 0x80), /* Phy Port 63 (Fabric Link23) Bus=1 Phy=24 */
        ((2 << PHY_ID_BUS_LOWER_SHIFT) | 0x00 | 0x80), /* Phy Port 64 (Fabric Link24) Bus=2 Phy=0  */
        ((2 << PHY_ID_BUS_LOWER_SHIFT) | 0x00 | 0x80), /* Phy Port 65 (Fabric Link25) Bus=2 Phy=0  */
        ((2 << PHY_ID_BUS_LOWER_SHIFT) | 0x00 | 0x80), /* Phy Port 66 (Fabric Link26) Bus=2 Phy=0  */
        ((2 << PHY_ID_BUS_LOWER_SHIFT) | 0x00 | 0x80), /* Phy Port 67 (Fabric Link27) Bus=2 Phy=0  */
        ((2 << PHY_ID_BUS_LOWER_SHIFT) | 0x04 | 0x80), /* Phy Port 68 (Fabric Link28) Bus=2 Phy=4  */
        ((2 << PHY_ID_BUS_LOWER_SHIFT) | 0x04 | 0x80), /* Phy Port 69 (Fabric Link29) Bus=2 Phy=4  */
        ((2 << PHY_ID_BUS_LOWER_SHIFT) | 0x04 | 0x80), /* Phy Port 70 (Fabric Link30) Bus=2 Phy=4  */
        ((2 << PHY_ID_BUS_LOWER_SHIFT) | 0x04 | 0x80), /* Phy Port 71 (Fabric Link31) Bus=2 Phy=4  */
        ((2 << PHY_ID_BUS_LOWER_SHIFT) | 0x08 | 0x80), /* Phy Port 72 (Fabric Link32) Bus=2 Phy=8  */
        ((2 << PHY_ID_BUS_LOWER_SHIFT) | 0x08 | 0x80), /* Phy Port 73 (Fabric Link33) Bus=2 Phy=8  */
        ((2 << PHY_ID_BUS_LOWER_SHIFT) | 0x08 | 0x80), /* Phy Port 74 (Fabric Link34) Bus=2 Phy=8  */
        ((2 << PHY_ID_BUS_LOWER_SHIFT) | 0x08 | 0x80), /* Phy Port 75 (Fabric Link35) Bus=2 Phy=8  */
    };

    uint32 phy_port;
    
    *array_size = 0;

    if(!is_logical) {
        phy_port = port;
        if(array_max != 1) {
            return SOC_E_PARAM;
        }
    }
    else{
        phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    }

    if(phy_port > 75){
        return SOC_E_PARAM;
    }
    /* array of one element return just the mdio address and type*/
    if(array_max == 1){
        core_info[0].mdio_addr = _soc_phy_addr_arad[phy_port]; 
        core_info[0].core_type = phyident_core_type_wc;
        core_info[0].mld_index = PHYIDENT_INFO_NOT_SET; 
        core_info[0].index_in_mld = PHYIDENT_INFO_NOT_SET;
        core_info[0].first_phy_in_core = PHYIDENT_INFO_NOT_SET;
        core_info[0].nof_lanes_in_core = PHYIDENT_INFO_NOT_SET;
        *array_size = 1;
        return SOC_E_NONE;
    }

#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit)){
        uint32 required_mdio_addr;
        soc_port_if_t interface_type;
        soc_pbmp_t quads_in_use, phys_in_use;
        uint32 lanes_in_quad;
        uint32 quad_id;
        uint32 mlds_in_use = 0;
        uint8 is_elk_ilkn_no_mld = FALSE;
        uint8 is_caui_ten_lanes_allowed = FALSE;
        int i;
        int rv;

        rv = soc_port_sw_db_interface_type_get(unit, port, &interface_type);
        if (rv != SOC_E_NONE) {
            return SOC_E_FAIL;
        }
#if defined(INCLUDE_KBP) && defined(BCM_88660)
        if(SOC_IS_ARADPLUS(unit)&& (SOC_DPP_CONFIG(unit)->arad->init.elk.ext_interface_mode)){
            uint32 offset;
            is_caui_ten_lanes_allowed = TRUE;
            if(interface_type == SOC_PORT_IF_ILKN){
                rv = soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset);
                if (rv != SOC_E_NONE) {
                    return SOC_E_FAIL;
                }
                /* ILKN1 in ext_interface_mode of ARAD PLUS does not use MLDs*/
                is_elk_ilkn_no_mld = (offset == 1);
            }
        }
#endif
        /* calaculate the required mdio entries (WCs+MLDs)*/
        rv = soc_pm_serdes_quads_in_use_get(unit, port, &quads_in_use);
        if (rv != SOC_E_NONE) {
            return SOC_E_FAIL;
        }
        SOC_PBMP_COUNT(quads_in_use, required_mdio_addr);
        if (!is_elk_ilkn_no_mld)
        {
            
            required_mdio_addr += (required_mdio_addr - 1)/ 3 + 1;
        }

        if(array_max < required_mdio_addr) {
            return SOC_E_PARAM;
        }

        /* add the cores to the array*/
        rv = soc_port_sw_db_phy_ports_get(unit, port, &phys_in_use);
        if (rv != SOC_E_NONE) {
            return SOC_E_FAIL;
        }
        SOC_PBMP_ITER(quads_in_use, quad_id)
        {
            if(quad_id > 7){
                return SOC_E_FAIL;
            }
            phy_port = (quad_id * LANES_IN_QUAD) + 1;
            /*add core_info*/
            core_info[*array_size].mdio_addr = _soc_phy_addr_arad[phy_port];
            core_info[*array_size].core_type = phyident_core_type_wc;
#if defined(INCLUDE_KBP) && defined(BCM_88660)
            if (is_elk_ilkn_no_mld){
                core_info[*array_size].mld_index = PHYIDENT_INFO_NOT_SET; 
                core_info[*array_size].index_in_mld = PHYIDENT_INFO_NOT_SET;
            } else
#endif /* defined(INCLUDE_KBP) && defined(BCM_88660) */
            {
                core_info[*array_size].mld_index = quad_id < 4 ? 0 : 1; 
                core_info[*array_size].index_in_mld = quad_id - (quad_id / 4) * 4;
                SHR_BITSET(&mlds_in_use, core_info[*array_size].mld_index);
            }        
            if((interface_type == SOC_PORT_IF_ILKN) || (is_caui_ten_lanes_allowed))
            {
                lanes_in_quad = 0;
                for ( i = 0 ; i < LANES_IN_QUAD; i++)
                {
                    if(SOC_PBMP_MEMBER(phys_in_use, phy_port + i)){
                        if(lanes_in_quad == 0){
                            core_info[*array_size].first_phy_in_core = i;
                        }
                        lanes_in_quad++;
                    }
                }
                core_info[*array_size].nof_lanes_in_core = lanes_in_quad;
            }
            (*array_size)++;
        }

        /*add the MLDs*/
        for(i = 0 ; i < 2 ; i++)
        {
            if(!SHR_BITGET(&mlds_in_use, i)){
                continue;
            }
            core_info[*array_size].mdio_addr = _soc_phy_addr_arad[33+i];
            core_info[*array_size].core_type = phyident_core_type_mld; 
            core_info[*array_size].mld_index = i;  
            core_info[*array_size].index_in_mld = 0;
            core_info[*array_size].first_phy_in_core = PHYIDENT_INFO_NOT_SET;
            core_info[*array_size].nof_lanes_in_core = PHYIDENT_INFO_NOT_SET;
            (*array_size)++;
        }
    }
#endif
    return SOC_E_NONE;
}

STATIC void
_dpp_phy_addr_default(int unit, int port,
                          uint16 *phy_addr, uint16 *phy_addr_int)
{
    int array_size, rv;
    phyident_core_info_t core_info;

    phyident_core_info_t_init(&core_info);

    *phy_addr = 0;
    rv = _dpp_phy_addr_multi_get(unit, port, 1, 1, &array_size, &core_info); 
    *phy_addr_int = core_info.mdio_addr;

    if(SOC_FAILURE(rv) || array_size==0){
         *phy_addr_int = 0; 
         LOG_VERBOSE(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "_dpp_phy_addr_default: failed to get address for port %d\n"), port));
    }
}
#endif /* BCM_PETRA_SUPPORT */

#ifdef BCM_DFE_SUPPORT
STATIC void
_dfe_phy_addr_default(int unit, int port,
                          uint16 *phy_addr, uint16 *phy_addr_int)
{
    uint32 bus_id;

    /* Physical port number starts from 0
     * Internal/external MDIO bus number = port number / 32
     * Internal MDIO address = (port number % 32) / 4
     */
    bus_id = port / 32;
    bus_id = ((bus_id & 0x3)<<PHY_ID_BUS_LOWER_SHIFT) | ((bus_id & 0xc)<<PHY_ID_BUS_UPPER_SHIFT);
    *phy_addr_int = 0x80 | bus_id | ((port % 32)/4);

    /* external MDIO address */
    *phy_addr = bus_id | ((port % 32)/4);
}
#endif /* BCM_DFE_SUPPORT */

#ifdef BCM_APACHE_SUPPORT
STATIC void
_apache_phy_addr_default(int unit, int port,
                         uint16 *phy_addr, uint16 *phy_addr_int)
{
    soc_info_t *si;
    int phy_port, bus_id_encoding, phy_id;

    si = &SOC_INFO(unit);

    phy_port = si->port_l2p_mapping[port];
    if (phy_port == -1) {
        *phy_addr_int = 0;
        *phy_addr = 0;
        return;
    }

    /*
     * Internal phy address:
     * bus 0 phy 1 to 28 are mapped to Physical port 1 to 28
     * bus 1 phy 1 to 20 are mapped to Physical port 29 to 48
     * bus 2 phy 1 to 24 are mapped to Physical port 49 to 72
     */
     if (phy_port < 29) {
         bus_id_encoding = 0;     /* bus 0, bit {8,6,5} = 0 */
         phy_id = phy_port;
     } else if (phy_port < 49) {
         bus_id_encoding = 0x020; /* bus 1, bit {8,6,5} = 1 */
         phy_id = phy_port - 28;
     } else {
         bus_id_encoding = 0x040; /* bus 2, bit {8,6,5} = 2 */
         phy_id = phy_port - 48;
     }
    *phy_addr_int = 0x80 | bus_id_encoding | phy_id;

    /*
     * External phy address:
     * bus 0 phy 1 to 28 are mapped to Physical port 1 to 28
     * bus 1 phy 1 to 20 are mapped to Physical port 29 to 48
     * bus 2 phy 1 to 24 are mapped to Physical port 49 to 72
     */
    /* external phy address encoding */
    *phy_addr = bus_id_encoding | phy_id;

    LOG_VERBOSE(BSL_LS_SOC_PHY, (BSL_META_U(unit,
        "_apache_phy_addr_default: port=%d phy_port=%d bus_id=%d phy_id=%d phy_int=%d phy_ext=%d\n"),
        port, phy_port, (bus_id_encoding >> 5), phy_id, *phy_addr_int, *phy_addr));

}
#endif /* BCM_APACHE_SUPPORT */


#ifdef BCM_TRIDENT_SUPPORT
STATIC void
_trident_phy_addr_default(int unit, int port,
                          uint16 *phy_addr, uint16 *phy_addr_int)
{
    soc_info_t *si;
    int phy_port;

    si = &SOC_INFO(unit);

    phy_port = si->port_l2p_mapping[port];
    if (phy_port == -1) {
        *phy_addr_int = 0;
        *phy_addr = 0;
        return;
    }

    /* Physical port number starts from 1
     * Internal/external MDIO bus number = (physical port number - 1) / 24
     * Internal MDIO address = (physical port number - 1) % 24 + 1
     */
    *phy_addr_int = 0x80 | (((phy_port - 1) / 24) << 5) | 
                           ((((phy_port - 1) % 24)/4)*4 + 1);

    /* external MDIO address */
    *phy_addr = (((phy_port - 1) / 24) << 5) | 
                ((phy_port - 1) % 24 + 4);
}
#endif /* BCM_TRIDENT_SUPPORT */

#ifdef BCM_TRIDENT2_SUPPORT
STATIC void
_trident2_phy_addr_default(int unit, int port,
                           uint16 *phy_addr, uint16 *phy_addr_int)
{
    soc_info_t *si;
    int phy_port, bus_id_encoding, phy_id;

    si = &SOC_INFO(unit);

    phy_port = si->port_l2p_mapping[port];
    if (phy_port == -1) {
        *phy_addr_int = 0;
        *phy_addr = 0;
        return;
    }

    /*
     * Internal phy address:
     * bus 0 phy 1 to 20 are mapped to Physical port 1 to 20
     * bus 1 phy 1 to 24 are mapped to Physical port 21 to 44
     * bus 2 phy 1 to 20 are mapped to Physical port 45 to 64
     * bus 3 phy 1 to 20 are mapped to Physical port 65 to 84
     * bus 4 phy 1 to 24 are mapped to Physical port 85 to 108
     * bus 5 phy 1 to 20 are mapped to Physical port 109 to 128
     */
    if (phy_port < 65) { /* X pipe */
        if (phy_port < 21) {
            bus_id_encoding = 0;     /* bus 0, bit {8,6,5} = 0 */
            phy_id = phy_port;
        } else if (phy_port < 45) {
            bus_id_encoding = 0x020; /* bus 1, bit {8,6,5} = 1 */
            phy_id = phy_port - 20;
        } else {
            bus_id_encoding = 0x040; /* bus 2, bit {8,6,5} = 2 */
            phy_id = phy_port - 44;
        }
    } else {
        if (phy_port < 85) {
            bus_id_encoding = 0x060; /* bus 3, bit {8,6,5} = 3 */
            phy_id = phy_port - 64;
        } else if (phy_port < 109) {
            bus_id_encoding = 0x100; /* bus 4, bit {8,6,5} = 4 */
            phy_id = phy_port - 84;
        } else {
            bus_id_encoding = 0x120; /* bus 5, bit {8,6,5} = 5 */
            phy_id = phy_port - 108;
        }
    }
    *phy_addr_int = 0x80 | bus_id_encoding | phy_id;

    /*
     * External phy address:
     * bus 0 phy 1 to 31 are mapped to Physical port 1 to 32
     * bus 1 phy 1 to 31 are mapped to Physical port 33 to 64
     * bus 2 phy 1 to 31 are mapped to Physical port 65 to 96
     * bus 3 phy 1 to 31 are mapped to Physical port 97 to 128
     */
    bus_id_encoding = (phy_port - 1) & 0x060;
    phy_id = (phy_port - 1) & 0x1f;
    /* external phy address encoding */
    *phy_addr = bus_id_encoding | phy_id;
}
#endif /* BCM_TRIDENT2_SUPPORT */

#ifdef BCM_SHADOW_SUPPORT
STATIC void
_shadow_phy_addr_default(int unit, int port,
                          uint16 *phy_addr, uint16 *phy_addr_int)
{
    soc_info_t *si;
    int phy_port;
    static const uint16 _soc_phy_addr_bcm88732_a0[] = {
        0,
        1, 1, 1, 1,       /* Radian port 1-4 => WC0 MDIO Port Address 1 */ 
        5, 5, 5, 5,       /* Radian port 5-8 => WC0 MDIO Port Address 5 */
        9, 9, 9, 9,       /* Radian port 9-10 => WC0 MDIO Port Address 9 */ 
        13, 13, 13, 13,   /* Radian port 11-12 => WC0 MDIO Port Address 13 */ 
        17, 17, 17, 17,   /* Radian port 13-14 => WC0 MDIO Port Address 17 */
        21, 21, 21, 21    /* Radian port 15-16 => WC0 MDIO Port Address 21 */
    };
    static const uint16 _soc_phy_addr_switch_link_xaui[] = {
        9,  9,       /* Radian port 9-10 => WC0 MDIO Port Address 9 */ 
        13, 13,    /* Radian port 11-12 => WC0 MDIO Port Address 13 */ 
        17, 17,    /* Radian port 13-14 => WC0 MDIO Port Address 17 */
        21, 21    /* Radian port 15-16 => WC0 MDIO Port Address 21 */
    };

    static const uint16 _soc_phy_addr_switch_link_r2[] = {
        9,  9,       /* Radian port 9-10 => WC0 MDIO Port Address 9 */ 
        13, 13,    /* Radian port 11-12 => WC0 MDIO Port Address 13 */ 
        17, 17,    /* Radian port 13-14 => WC0 MDIO Port Address 17 */
        21, 21    /* Radian port 15-16 => WC0 MDIO Port Address 21 */
    };
    si = &SOC_INFO(unit);

    if (soc_feature(unit, soc_feature_logical_port_num)) {
        phy_port = si->port_l2p_mapping[port];
    } else {
        phy_port = port;
    }
    if (phy_port < 1 || phy_port > 16) {
        *phy_addr_int = 0;
        *phy_addr = 0;
        return;
    }
    if (phy_port < 9) {
       *phy_addr_int = 0x80 | (_soc_phy_addr_bcm88732_a0[phy_port]);
       *phy_addr = *phy_addr_int; /* no external PHYs */
    } else {  /* switch link side */
        if (soc_property_get(unit, spn_BCM88732_1X40_4X10, 0) || 
            soc_property_get(unit, spn_BCM88732_4X10_4X10, 0)) {
            *phy_addr_int = 0x80 | (_soc_phy_addr_switch_link_xaui[phy_port-9]);
            *phy_addr = *phy_addr_int; /* no external PHYs */
        } else if (soc_property_get(unit, spn_BCM88732_2X40_8X12, 0) || 
            soc_property_get(unit, spn_BCM88732_1X40_4X10_8X12, 0) ||
            soc_property_get(unit, spn_BCM88732_4X10_1X40_8X12, 0) ||
            soc_property_get(unit, spn_BCM88732_8X10_8X12, 0) ||
            soc_property_get(unit, spn_BCM88732_8X10_4X12, 0) ||
            soc_property_get(unit, spn_BCM88732_6X10_2X12, 0) ||
            soc_property_get(unit, spn_BCM88732_8X10_2X12, 0) ||
            soc_property_get(unit, spn_BCM88732_2X40_8X10, 0) ||
            soc_property_get(unit, spn_BCM88732_8X10_8X10, 0)) {
            *phy_addr_int = 0x80 | (_soc_phy_addr_switch_link_r2[phy_port-9]);
            *phy_addr = *phy_addr_int; /* no external PHYs */
        } else {
            *phy_addr_int = 0x80 | (_soc_phy_addr_bcm88732_a0[phy_port]);
            if (soc_property_get(unit, spn_BCM88732_2X40_2X40, 0) ||
                soc_property_get(unit, spn_BCM88732_8X10_2X40, 0) ||
                (soc_property_get(unit, "IL3125", 0)))  { /* for 16Lanes */
                /* 
                 * Use the MDIO address assignment similar to that of 
                 * Switch link port configuration. 
                 */
                *phy_addr_int = 0x80 | (_soc_phy_addr_switch_link_r2[phy_port-9]);
            } else { 
                 *phy_addr_int = 0x80 | (_soc_phy_addr_bcm88732_a0[phy_port]);
            }
            *phy_addr = *phy_addr_int; /* no external PHYs */
        }
    }
}
#endif /* BCM_SHADOW_SUPPORT */

#ifdef BCM_SCORPION_SUPPORT
static int
_scorpion_phy_addr_default(int unit, int port,
                           uint16 *phy_addr, uint16 *phy_addr_int)
{
    static const uint16 _soc_phy_addr_bcm56820_a0[] = {
 /* extPhy intPhy          */
    0x00, 0x00, /* Port  0 (cmic) N/A */
    0x42, 0xc1, /* Port  1 ( xe0) */
    0x43, 0xc2, /* Port  2 ( xe1) */
    0x44, 0xc3, /* Port  3 ( xe2) */
    0x45, 0xc4, /* Port  4 ( xe3) */
    0x46, 0xc5, /* Port  5 ( xe4) */
    0x47, 0xc6, /* Port  6 ( xe5) */
    0x48, 0xc7, /* Port  7 ( xe6) */
    0x49, 0xc8, /* Port  8 ( xe7) */
    0x4a, 0xc9, /* Port  9 ( xe8) */
    0x4b, 0xca, /* Port 10 ( xe9) */
    0x4c, 0xcb, /* Port 11 (xe10) */
    0x4d, 0xcc, /* Port 12 (xe11) */
    0x4e, 0xcd, /* Port 13 (xe12) */
    0x4f, 0xce, /* Port 14 (xe13) */
    0x50, 0xcf, /* Port 15 (xe14) */
    0x51, 0xd0, /* Port 16 (xe15) */
    0x52, 0xd1, /* Port 17 (xe16) */
    0x53, 0xd2, /* Port 18 (xe17) */
    0x54, 0xd3, /* Port 19 (xe18) */
    0x55, 0xd4, /* Port 20 (xe19) */
    0x21, 0xd5, /* Port 21 (xe20) */
    0x26, 0xd6, /* Port 22 (xe21) */
    0x2b, 0xd7, /* Port 23 (xe22) */
    0x30, 0xd8, /* Port 24 (xe23) */
    0x59, 0xd9, /* Port 25 (ge0) */
    0x5a, 0xda, /* Port 26 (ge1) */
    0x5b, 0xdb, /* Port 27 (ge2) */
    0x5c, 0xdc  /* Port 28 (ge3) */
    };
    int rv = TRUE;
    uint16 dev_id;
    uint8 rev_id;
                                                                                
    soc_cm_get_id(unit, &dev_id, &rev_id);

    switch (dev_id) {
    case BCM56820_DEVICE_ID:
        *phy_addr = _soc_phy_addr_bcm56820_a0[port * 2];
        *phy_addr_int = _soc_phy_addr_bcm56820_a0[port * 2 + 1];
        break;
    default:
        rv = FALSE;
        break;
    }
    return rv;
}
#endif /* BCM_SCORPION_SUPPORT */
#ifdef BCM_HURRICANE4_SUPPORT
STATIC void
_hurricane4_phy_addr_default(int unit, int port,
                           uint16 *phy_addr, uint16 *phy_addr_int)
{
    soc_info_t *si;
    int phy_port, bus_id_encoding, phy_id;

    si = &SOC_INFO(unit);

    phy_port = si->port_l2p_mapping[port];
    if (phy_port == -1 || phy_port == 0 || phy_port > 64) {
        *phy_addr_int = 0;
        *phy_addr = 0;
        return;
    }

    /*
     * Internal phy address:
     * bus 0 phy 1 to 24 are mapped to Physical port  1 to 24 GPHY
     * bus 1 phy 1 to 4  are mapped to Physical port 25 to 40 PMQ-0
     * bus 1 phy 4 to 8  are mapped to Physical port 41 to 56 PMQ-1
     * bus 1 phy 9 to 12 are mapped to Physical port 61 to 64 Merlin
     * bus 2 phy 1 to 4  are mapped to Physical port 57 to 60 Falcon
     * bus 3 phy 1 to 4  are mapped to Physical port IPROC PM
     */
    if (phy_port <= 24) {                      /* GPHY */
        bus_id_encoding = 0;                   /* bus 0, bit {8,6,5} = 0 */
        phy_id = phy_port;
        *phy_addr_int = 0;
        *phy_addr = 0x80 | bus_id_encoding | phy_id;
        return;
    } else if (phy_port <= 56) {               /* PMQ-0 & PMQ-1 */
        bus_id_encoding = 0x020;               /* bus 1, bit {8,6,5} = 1 */
        phy_id = ((phy_port - 25) % 4) + 1;
    } else if (phy_port <= 60) {               /* Falcon */
        bus_id_encoding = 0x060;               /* bus 2, bit {8,6,5} = 2 */
        phy_id = phy_port - 56;
    } else {                                   /* Merlin */
        bus_id_encoding = 0x020;               /* bus 1, bit {8,6,5} = 1 */
        phy_id = phy_port - 60;
    }
    *phy_addr_int = 0x80 | bus_id_encoding | phy_id;

    /*
     * External phy address:
     */
    /* external phy address encoding */
    *phy_addr = bus_id_encoding | phy_id;
}
#endif /* BCM_HURRICANE4_SUPPORT */

#ifdef BCM_TRIDENT3_SUPPORT
STATIC void
_trident3_phy_addr_default(int unit, int port,
                           uint16 *phy_addr, uint16 *phy_addr_int)
{
    soc_info_t *si;
    int phy_port_base, pbm_count = 0, last_pm4x25_phy_port=128;
    int phy_port, bus_id_encoding, phy_id;

    si = &SOC_INFO(unit);

    phy_port = si->port_l2p_mapping[port];
    if (phy_port == -1 || phy_port == 0 || phy_port > 131) {
        *phy_addr_int = 0;
        *phy_addr = 0;
        return;
    }
    phy_port_base = (1 + ((SOC_INFO(unit).port_l2p_mapping[port] - 1) & ~0x3));

    SOC_PBMP_COUNT(si->management_pbm, pbm_count);
    if (pbm_count == 2) last_pm4x25_phy_port = 127;

    /*
     * Internal phy address:
     * bus 0 phy 1 to 24 are mapped to physical port 1 to 24
     * bus 1 phy 1 to 16 are mapped to physical port 25 to 40
     * bus 2 phy 1 to 24 are mapped to physical port 41 to 64
     * bus 3 phy 1 to 24 are mapped to physical port 65 to 88
     * bus 4 phy 1 to 16 are mapped to physical port 89 to 104
     * bus 5 phy 1 to 24 are mapped to physical port 105 to 128
     * bus 6 phy 1 is physical port 129
     * bus 6 phy 2 is physical port 128
     */
    if (phy_port < 65) { /* PM0 - PM15 */
        if (phy_port < 25) {
            bus_id_encoding = 0;     /* bus 0, bit {8,6,5} = 0 */
            phy_id = phy_port;
        } else if (phy_port < 41) {
            bus_id_encoding = 0x020; /* bus 1, bit {8,6,5} = 1 */
            phy_id = phy_port - 24;
        } else {
            bus_id_encoding = 0x040; /* bus 2, bit {8,6,5} = 2 */
            phy_id = phy_port - 40;
        }
    } else if (phy_port <= last_pm4x25_phy_port) { /* PM16 - PM31 */
        if (phy_port < 89) {
            bus_id_encoding = 0x060; /* bus 3, bit {8,6,5} = 3 */
            phy_id = phy_port - 64;
        } else if (phy_port < 105) {
            bus_id_encoding = 0x100; /* bus 4, bit {8,6,5} = 4 */
            phy_id = phy_port - 88;
        } else {
            bus_id_encoding = 0x120; /* bus 5, bit {8,6,5} = 5 */
            phy_id = phy_port - 104;
        }
    } else { /* PM32 */
        bus_id_encoding = 0x140; /* bus 6, bit {8,6,5} = 6 */
        phy_id = phy_port == 129 ? 1 : 3;
    }
    phy_id = phy_id - (phy_port - phy_port_base);
    *phy_addr_int = 0x80 | bus_id_encoding | phy_id;

    /*
     * External phy address:
     * bus 0 phy 2 to 32 are mapped to Physical port 1 to 32
     * bus 1 phy 2 to 32 are mapped to Physical port 33 to 64
     * bus 2 phy 2 to 32 are mapped to Physical port 65 to 96
     * bus 3 phy 2 to 32 are mapped to Physical port 97 to 128
     */
    bus_id_encoding = (phy_port - 1) & 0x060;
    phy_id = (((phy_port - 1) & 0x1f) / 4) + 2;
    /* external phy address encoding */
    *phy_addr = bus_id_encoding | phy_id;
}
#endif /* BCM_TRIDENT3_SUPPORT */

#ifdef BCM_TOMAHAWK_SUPPORT
STATIC void
_tomahawk_phy_addr_default(int unit, int port,
                           uint16 *phy_addr, uint16 *phy_addr_int)
{
    soc_info_t *si;
    int phy_port_base;
    int phy_port, bus_id_encoding, phy_id;

    si = &SOC_INFO(unit);

    phy_port = si->port_l2p_mapping[port];
    if (phy_port == -1 || phy_port == 0 || phy_port > 131) {
        *phy_addr_int = 0;
        *phy_addr = 0;
        return;
    }
    phy_port_base = (1 + ((SOC_INFO(unit).port_l2p_mapping[port] - 1) & ~0x3));

    /*
     * Internal phy address:
     * bus 0 phy 1 to 24 are mapped to physical port 1 to 24
     * bus 1 phy 1 to 16 are mapped to physical port 25 to 40
     * bus 2 phy 1 to 24 are mapped to physical port 41 to 64
     * bus 3 phy 1 to 24 are mapped to physical port 65 to 88
     * bus 4 phy 1 to 16 are mapped to physical port 89 to 104
     * bus 5 phy 1 to 24 are mapped to physical port 105 to 128
     * bus 6 phy 1 is physical port 129
     * bus 6 phy 2 is physical port 131
     */
    if (phy_port < 65) { /* PM0 - PM15 */
        if (phy_port < 25) {
            bus_id_encoding = 0;     /* bus 0, bit {8,6,5} = 0 */
            phy_id = phy_port;
        } else if (phy_port < 41) {
            bus_id_encoding = 0x020; /* bus 1, bit {8,6,5} = 1 */
            phy_id = phy_port - 24;
        } else {
            bus_id_encoding = 0x040; /* bus 2, bit {8,6,5} = 2 */
            phy_id = phy_port - 40;
        }
    } else if (phy_port < 129) { /* PM16 - PM31 */
        if (phy_port < 89) {
            bus_id_encoding = 0x060; /* bus 3, bit {8,6,5} = 3 */
            phy_id = phy_port - 64;
        } else if (phy_port < 105) {
            bus_id_encoding = 0x100; /* bus 4, bit {8,6,5} = 4 */
            phy_id = phy_port - 88;
        } else {
            bus_id_encoding = 0x120; /* bus 5, bit {8,6,5} = 5 */
            phy_id = phy_port - 104;
        }
    } else { /* PM32 */
        bus_id_encoding = 0x140; /* bus 6, bit {8,6,5} = 6 */
        phy_id = phy_port == 129 ? 1 : 3;
    }
    phy_id = phy_id - (phy_port - phy_port_base);
    *phy_addr_int = 0x80 | bus_id_encoding | phy_id;

    /*
     * External phy address:
     * bus 0 phy 2 to 32 are mapped to Physical port 1 to 32
     * bus 1 phy 2 to 32 are mapped to Physical port 33 to 64
     * bus 2 phy 2 to 32 are mapped to Physical port 65 to 96
     * bus 3 phy 2 to 32 are mapped to Physical port 97 to 128
     */
    bus_id_encoding = (phy_port - 1) & 0x060;
    phy_id = (((phy_port - 1) & 0x1f) / 4) + 2;
    /* external phy address encoding */
    *phy_addr = bus_id_encoding | phy_id;
}
#endif /* BCM_TOMAHAWK_SUPPORT */

#ifdef BCM_TOMAHAWK2_SUPPORT
STATIC void
_tomahawk2_phy_addr_default(int unit, int port,
                            uint16 *phy_addr, uint16 *phy_addr_int)
{
    soc_info_t *si;
    int phy_port, bus_id_encoding, phy_id;

    si = &SOC_INFO(unit);

    phy_port = si->port_l2p_mapping[port];
    if (phy_port == -1 || phy_port == 0 || phy_port > 259) {
        *phy_addr_int = 0;
        *phy_addr = 0;
        return;
    }

    /*
     * Internal phy address:
     * bus 0 phy 1 to 28 are mapped to Physical port 1 to 28
     * bus 1 phy 1 to 28 are mapped to Physical port 29 to 56
     * bus 2 phy 1 to 16 are mapped to Physical port 57 to 72
     * bus 3 phy 1 to 28 are mapped to Physical port 73 to 100
     * bus 4 phy 1 to 28 are mapped to Physical port 101 to 128
     * bus 5 phy 1 to 28 are mapped to Physical port 129 to 156
     * bus 6 phy 1 to 28 are mapped to Physical port 157 to 184
     * bus 7 phy 1 to 16 are mapped to Physical port 185 to 200
     * bus 8 phy 1 to 28 are mapped to Physical port 201 to 228
     * bus 9 phy 1 to 28 are mapped to Physical port 229 to 256
     * bus 10 phy 1 is mapped to Physical port 257 and 
     *        phy 3 is mapped to Physical port 259
     */
    if (phy_port < 129) { /* PM0 - PM31 */
        if (phy_port < 29) {
            bus_id_encoding = 0;     /* bus 0, bit {9,8,6,5} = 0 */
            phy_id = phy_port;
        } else if (phy_port < 57) {
            bus_id_encoding = 0x020; /* bus 1, bit {9,8,6,5} = 1 */
            phy_id = phy_port - 28;
        } else if (phy_port < 73) {
            bus_id_encoding = 0x040; /* bus 2, bit {9,8,6,5} = 2 */
            phy_id = phy_port - 56;
        } else if (phy_port < 101) {
            bus_id_encoding = 0x060; /* bus 3, bit {9,8,6,5} = 3 */
            phy_id = phy_port - 72;
        } else {
            bus_id_encoding = 0x100; /* bus 4, bit {9,8,6,5} = 4 */
            phy_id = phy_port - 100;
        }
    } else if (phy_port < 257) { /* PM32 - PM63 */
        if (phy_port < 157) {
            bus_id_encoding = 0x120; /* bus 5, bit {9,8,6,5} = 5 */
            phy_id = phy_port - 128;
        } else if (phy_port < 185) {
            bus_id_encoding = 0x140; /* bus 6, bit {9,8,6,5} = 6 */
            phy_id = phy_port - 156;
        } else if (phy_port < 201) {
            bus_id_encoding = 0x160; /* bus 7, bit {9,8,6,5} = 7 */
            phy_id = phy_port - 184;
        } else if (phy_port < 229) {
            bus_id_encoding = 0x200; /* bus 8, bit {9,8,6,5} = 8 */
            phy_id = phy_port - 200;
        } else {
            bus_id_encoding = 0x220; /* bus 9, bit {9,8,6,5} = 9 */
            phy_id = phy_port - 228;
        }
    } else { /* PM64 */
        bus_id_encoding = 0x240;     /* bus 10, bit {9,8,6,5} = 10 */
        phy_id = phy_port == 257 ? 1 : 3;
    }
    *phy_addr_int = 0x80 | bus_id_encoding | phy_id;

    /*
     * External phy address:
     * bus 0 phy 0 to 31 are mapped to Physical port 1 to 32
     * bus 1 phy 0 to 31 are mapped to Physical port 33 to 64
     * bus 2 phy 0 to 31 are mapped to Physical port 65 to 96
     * bus 3 phy 0 to 31 are mapped to Physical port 97 to 128
     * bus 4 phy 0 to 31 are mapped to Physical port 129 to 160
     * bus 5 phy 0 to 31 are mapped to Physical port 161 to 192
     * bus 6 phy 0 to 31 are mapped to Physical port 193 to 224
     * bus 7 phy 0 to 31 are mapped to Physical port 225 to 256
     */
    bus_id_encoding = (phy_port - 1) & 0x1e0;
    bus_id_encoding = (bus_id_encoding & 0x180) << 1 |
                      (bus_id_encoding & 0x060);
    phy_id = (phy_port - 1) & 0x1f;
    /* external phy address encoding */
    *phy_addr = bus_id_encoding | phy_id;
}
#endif /* BCM_TOMAHAWK2_SUPPORT */

#ifdef BCM_TOMAHAWK3_SUPPORT
STATIC void
_tomahawk3_phy_addr_default(int unit, int port,
                            uint16 *phy_addr, uint16 *phy_addr_int)
{
    soc_info_t *si;
    int phy_port, bus_id_encoding, phy_id;

    si = &SOC_INFO(unit);

    phy_port = si->port_l2p_mapping[port];
    if (phy_port == -1 || phy_port == 0 || phy_port > 258) {
        *phy_addr_int = 0;
        *phy_addr = 0;
        return;
    }
    /*
     * Internal phy address:
     * bus 0 phy 1 to 24 are mapped to Physical port 1 to 24
     * bus 1 phy 1 to 24 are mapped to Physical port 25 to 48
     * bus 2 phy 1 to 24 are mapped to Physical port 49 to 72
     * bus 3 phy 1 to 24 are mapped to Physical port 73 to 96
     * bus 4 phy 1 to 24 are mapped to Physical port 97 to 120
     * bus 5 phy 1 to 24 are mapped to Physical port 121 to 144
     * bus 6 phy 1 to 24 are mapped to Physical port 145 to 168
     * bus 7 phy 1 to 24 are mapped to Physical port 169 to 192
     * bus 8 phy 1 to 24 are mapped to Physical port 193 to 216
     * bus 9 phy 1 to 24 are mapped to Physical port 217 to 240
     * bus 10 phy 1 to 16 are mapped to Physical port 241 to 256
     * bus 11 phy 1 is mapped to Physical port 257 and
     *        phy 2 is mapped to Physical port 258
     */
    if (phy_port < 129) { /* PM0 - PM15 */
        if (phy_port < 25) {
            bus_id_encoding = 0;     /* bus 0, bit {9,8,6,5} = 0 */
            phy_id = phy_port;
        } else if (phy_port < 49) {
            bus_id_encoding = 0x020; /* bus 1, bit {9,8,6,5} = 1 */
            phy_id = phy_port - 24;
        } else if (phy_port < 73) {
            bus_id_encoding = 0x040; /* bus 2, bit {9,8,6,5} = 2 */
            phy_id = phy_port - 48;
        } else if (phy_port < 97) {
            bus_id_encoding = 0x060; /* bus 3, bit {9,8,6,5} = 3 */
            phy_id = phy_port - 72;
        } else if (phy_port < 121) {
            bus_id_encoding = 0x100; /* bus 4, bit {9,8,6,5} = 4 */
            phy_id = phy_port - 96;
        } else {
            bus_id_encoding = 0x120; /* bus 5, bit {9,8,6,5} = 5 */
            phy_id = phy_port - 120; /* PM15 only */
        }
    } else if (phy_port < 257) { /* PM16 - PM31 */
        if (phy_port < 145) {        /* bus 5, continued, PM16, 17 */
            bus_id_encoding = 0x120;
            phy_id = phy_port - 120;
        } else if (phy_port < 169) {
            bus_id_encoding = 0x140; /* bus 6, bit {9,8,6,5} = 6 */
            phy_id = phy_port - 144;
        } else if (phy_port < 193) {
            bus_id_encoding = 0x160; /* bus 7, bit {9,8,6,5} = 7 */
            phy_id = phy_port - 168;
        } else if (phy_port < 217) {
            bus_id_encoding = 0x200; /* bus 8, bit {9,8,6,5} = 8 */
            phy_id = phy_port - 192;
        } else if (phy_port < 241) {
            bus_id_encoding = 0x220; /* bus 9, bit {9,8,6,5} = 9 */
            phy_id = phy_port - 216;
        } else {
            bus_id_encoding = 0x240; /* bus 10, bit {9,8,6,5} = 10 */
            phy_id = phy_port - 240;
        }
    } else { /* PM64 */
        bus_id_encoding = 0x260;     /* bus 10, bit {9,8,6,5} = 11 */
        phy_id = phy_port == 257 ? 1 : 2;
    }
    *phy_addr_int = 0x80 | bus_id_encoding | phy_id;

    
#if 0
    /*
     * External phy address:
     * bus 0 phy 0 to 31 are mapped to Physical port 1 to 32
     * bus 1 phy 0 to 31 are mapped to Physical port 33 to 64
     * bus 2 phy 0 to 31 are mapped to Physical port 65 to 96
     * bus 3 phy 0 to 31 are mapped to Physical port 97 to 128
     * bus 4 phy 0 to 31 are mapped to Physical port 129 to 160
     * bus 5 phy 0 to 31 are mapped to Physical port 161 to 192
     * bus 6 phy 0 to 31 are mapped to Physical port 193 to 224
     * bus 7 phy 0 to 31 are mapped to Physical port 225 to 256
     */
    bus_id_encoding = (phy_port - 1) & 0x1e0;
    bus_id_encoding = (bus_id_encoding & 0x180) << 1 |
                      (bus_id_encoding & 0x060);
    phy_id = (phy_port - 1) & 0x1f;
    /* external phy address encoding */
    *phy_addr = bus_id_encoding | phy_id;
#endif
}
#endif /* BCM_TOMAHAWK3_SUPPORT */


#ifdef BCM_XGS3_SWITCH_SUPPORT
static void
_xgs3_phy_addr_default(int unit, int port,
                      uint16 *phy_addr, uint16 *phy_addr_int)
{
    int phy_addr_adjust = 0;

    if (SOC_IS_GOLDWING(unit)) {
        /* In Goldwing due to the hardware port remapping, we need to adjust
         * the PHY addresses. The PHY addresses are remapped as follow.
         * Original : 0  ... 13 14 15 16 17 18 19
         * Remapped : 0  ... 13 16 17 18 19 14 15
         */
        if (port == 14 || port == 15) {
            phy_addr_adjust = 4;
        } else if (port > 15) {
            phy_addr_adjust = -2;
        }
    }
    if (IS_HG_PORT(unit, port) || IS_XE_PORT(unit, port) ||
        IS_GX_PORT(unit, port)) {
        pbmp_t pbm;
        int    temp_port;
        int    found     = 0;
        int    mdio_addr = 1;
        /*
         * Internal XAUI (Internal sel bit 0x80) on XPORT MDIO bus(0x40)
         * External Phy  (Internal sel bit 0x00) on XPORT MDIO bus(0x40)
         * Assume External MDIO address starts at 1 as this is on a
         * seperate BUS.
         */

        /*
         * First, assign lowest addresses to GE ports which are also
         * GX ports.
         */
        pbm = PBMP_GX_ALL(unit);
        SOC_PBMP_AND(pbm, PBMP_GE_ALL(unit));

        PBMP_ITER(pbm, temp_port) {
            if (temp_port == port) {
                found = 1;
                break;
            }
            mdio_addr++;
        }

        /*
         * Second, assign external addresses for XE ports.
         */
        if (!found) {
            PBMP_XE_ITER(unit, temp_port) {
                if  (temp_port == port) {
                    found = 1;
                    break;
                }
                mdio_addr++;
            }
        }

        /*
         * Finally, assign external adddresses for HG ports.
         */
        if (!found) {
            PBMP_HG_ITER(unit, temp_port) {
                if  (temp_port == port) {
                    found = 1;
                    break;
                }
                mdio_addr++;
            }
        }

        *phy_addr = mdio_addr + 0x40;
        *phy_addr_int = port + 0xc0 + phy_addr_adjust;

        if (SOC_IS_SC_CQ(unit)) {
            *phy_addr_int = port + 0xc0;
            *phy_addr = port + 0x41;
        }
        if (SAL_BOOT_QUICKTURN) {
            *phy_addr = port + 0x41;
            if (SOC_IS_SCORPION(unit)) {
                /* Skip over CMIC at port 0 */
                *phy_addr -= 1;
            }
        }
    } else {
        /*
         * Internal Serdes (Internal sel bit 0x80) on GPORT MDIO bus(0x00)
         * External Phy    (Internal sel bit 0x00) on GPORT MDIO bus(0x00)
         */
        *phy_addr = port + 1 + phy_addr_adjust;
        *phy_addr_int = port + 0x80 + phy_addr_adjust;
        if (SOC_IS_SC_CQ(unit)) {
            *phy_addr_int = port + 0xc0;
            *phy_addr = port + 0x40;
        }
    }
}
#endif /* BCM_XGS3_SWITCH_SUPPORT */

#ifdef BCM_HAWKEYE_SUPPORT
static void
_hawkeye_phy_addr_default(int unit, int port,
                           uint16 *phy_addr, uint16 *phy_addr_int)
{
    static const uint16 _soc_phy_addr_bcm53314_a0[] = {
    0x80, 0x0, /* Port  0 (cmic) N/A */
    0x81, 0x0, /* Port  1 ( ge0) ExtBus=0 ExtAddr=0x01 */
    0x82, 0x0, /* Port  2 ( ge1) ExtBus=0 ExtAddr=0x02 */
    0x83, 0x0, /* Port  3 ( ge2) ExtBus=0 ExtAddr=0x03 */
    0x84, 0x0, /* Port  4 ( ge3) ExtBus=0 ExtAddr=0x04 */
    0x85, 0x0, /* Port  5 ( ge4) ExtBus=0 ExtAddr=0x05 */
    0x86, 0x0, /* Port  6 ( ge5) ExtBus=0 ExtAddr=0x06 */
    0x87, 0x0, /* Port  7 ( ge6) ExtBus=0 ExtAddr=0x07 */
    0x88, 0x0, /* Port  8 ( ge7) ExtBus=0 ExtAddr=0x08 */
    0x09, 0x89, /* Port  9 ( ge8) ExtBus=0 ExtAddr=0x09 IntBus=0 IntAddr=0x09 */
    0x0a, 0x89, /* Port 10 ( ge9) ExtBus=0 ExtAddr=0x0a IntBus=0 IntAddr=0x09 */
    0x0b, 0x89, /* Port 11 (ge10) ExtBus=0 ExtAddr=0x0b IntBus=0 IntAddr=0x09 */
    0x0c, 0x89, /* Port 12 (ge11) ExtBus=0 ExtAddr=0x0c IntBus=0 IntAddr=0x09*/
    0x0d, 0x89, /* Port 13 (ge12) ExtBus=0 ExtAddr=0x0d IntBus=0 IntAddr=0x09 */
    0x0e, 0x89, /* Port 14 (ge13) ExtBus=0 ExtAddr=0x0e IntBus=0 IntAddr=0x09 */
    0x0f, 0x89, /* Port 15 (ge14) ExtBus=0 ExtAddr=0x0f IntBus=0 IntAddr=0x09 */
    0x10, 0x89, /* Port 16 (ge15) ExtBus=0 ExtAddr=0x10 IntBus=0 IntAddr=0x09 */
    0x12, 0x91, /* Port 17 (ge16) ExtBus=0 ExtAddr=0x12 IntBus=0 IntAddr=0x11 */
    0x13, 0x91, /* Port 18 (ge17) ExtBus=0 ExtAddr=0x13 IntBus=0 IntAddr=0x11 */
    0x14, 0x91, /* Port 19 (ge18) ExtBus=0 ExtAddr=0x14 IntBus=0 IntAddr=0x11 */
    0x15, 0x91, /* Port 20 (ge19) ExtBus=0 ExtAddr=0x15 IntBus=0 IntAddr=0x11 */
    0x16, 0x91, /* Port 21 (ge20) ExtBus=0 ExtAddr=0x16 IntBus=0 IntAddr=0x11 */
    0x17, 0x91, /* Port 22 (ge21) ExtBus=0 ExtAddr=0x17 IntBus=0 IntAddr=0x11 */
    0x18, 0x91, /* Port 23 (ge22) ExtBus=0 ExtAddr=0x18 IntBus=0 IntAddr=0x11 */
    0x19, 0x91, /* Port 24 (ge23) ExtBus=0 ExtAddr=0x19 IntBus=0 IntAddr=0x11 */
    };

    uint16 dev_id;
    uint8 rev_id;

    soc_cm_get_id(unit, &dev_id, &rev_id);

    switch (dev_id) {
    case BCM53312_DEVICE_ID:
    case BCM53313_DEVICE_ID:
    case BCM53314_DEVICE_ID:
    case BCM53322_DEVICE_ID:
    case BCM53323_DEVICE_ID:
    case BCM53324_DEVICE_ID:
        *phy_addr = _soc_phy_addr_bcm53314_a0[port * 2];
        *phy_addr_int = _soc_phy_addr_bcm53314_a0[port * 2 + 1];
        break;
    }
}
#endif /* BCM_HAWKEYE_SUPPORT */

#ifdef BCM_KATANA_SUPPORT
STATIC void
_katana_phy_addr_default(int unit, int port,
                          uint16 *phy_addr, uint16 *phy_addr_int)
{
    uint16 _katana_phy_int_addr_mxqport[] = {
        0xa0, /* Port  25 Int XG Bus=1 IntAddr=0x80 */
        0xa1, /* Port  26 Int XG Bus=1 IntAddr=0x81 */
        0xa2, /* Port  27 Int XG Bus=1 IntAddr=0x82 */
        0xa6, /* Port  28 Int XG Bus=1 IntAddr=0x86 */
        0xa7, /* Port  29 Int XG Bus=1 IntAddr=0x87 */
        0xa8, /* Port  30 Int XG Bus=1 IntAddr=0x88 */
        0xa9, /* Port  31 Int XG Bus=1 IntAddr=0x89 */
        0xa3, /* Port  32 Int XG Bus=1 IntAddr=0x83 */
        0xa4, /* Port  33 Int XG Bus=1 IntAddr=0x84 */
        0xa5, /* Port  34 Int XG Bus=1 IntAddr=0x85 */
    };
    static const uint16 _katana_phy_ext_addr_mxqport[] = {
        0x2c, /* Port  25 Ext Bus=1 ExtAddr=0x0c */
        0x30, /* Port  26 Ext Bus=1 ExtAddr=0x10 */
        0x34, /* Port  27 Ext Bus=1 ExtAddr=0x14 */
        0x38, /* Port  28 Ext Bus=1 ExtAddr=0x18 */
        0x39, /* Port  29 Ext Bus=1 ExtAddr=0x19 */
        0x3a, /* Port  30 Ext Bus=1 ExtAddr=0x1a */
        0x3b, /* Port  31 Ext Bus=1 ExtAddr=0x1b */
        0x35, /* Port  32 Ext Bus=1 ExtAddr=0x15 */
        0x36, /* Port  33 Ext Bus=1 ExtAddr=0x16 */
        0x37, /* Port  34 Ext Bus=1 ExtAddr=0x17 */
    };

    uint16 dev_id;
    uint8 rev_id;
    soc_pbmp_t pbmp_valid; 
    int        local_port = 0 ;
    int        mxq2cnt = 4;
    int        mxq3cnt = 4;

    soc_cm_get_id(unit, &dev_id, &rev_id);
    pbmp_valid = soc_property_get_pbmp(unit, spn_PBMP_VALID, 0);

    SOC_PBMP_ITER (pbmp_valid, local_port) {
        if (!SOC_PORT_VALID(unit,port)) {
            continue; 
        } 
        if ((local_port == 27) || (local_port == 32) || 
            (local_port == 33) || (local_port == 34)) {
             mxq2cnt--;
        }
        if ((local_port == 28) || (local_port == 29) || 
            (local_port == 30) || (local_port == 31)) {
             mxq3cnt--;
        }
    }
    if ((mxq2cnt != 4) && (!SOC_PORT_VALID(unit, 27))) {
        _katana_phy_int_addr_mxqport[27-25] = 0xa2;
        _katana_phy_int_addr_mxqport[32-25] = 0xa2;
        _katana_phy_int_addr_mxqport[33-25] = 0xa2;
        _katana_phy_int_addr_mxqport[34-25] = 0xa2;
    }
    if ((mxq3cnt != 4) && (!SOC_PORT_VALID(unit, 28))) {
        _katana_phy_int_addr_mxqport[28-25] = 0xa6;
        _katana_phy_int_addr_mxqport[29-25] = 0xa6;
        _katana_phy_int_addr_mxqport[30-25] = 0xa6;
        _katana_phy_int_addr_mxqport[31-25] = 0xa6;
    }

    if(port < 25) {
        *phy_addr_int = (0x80 | port);
        *phy_addr = port;
    } else {
        *phy_addr_int = _katana_phy_int_addr_mxqport[port-25];
        switch (dev_id) {
            case BCM56440_DEVICE_ID:
            case BCM55440_DEVICE_ID:
            case BCM55441_DEVICE_ID:
            case BCM56445_DEVICE_ID:
            case BCM56448_DEVICE_ID:
            case BCM56449_DEVICE_ID:
                /* Valid ports 25-28 only */
                *phy_addr = (0x20 | port); /* Bus-1 */
                break;
            case BCM56441_DEVICE_ID:
            case BCM56446_DEVICE_ID:
            case BCM56443_DEVICE_ID:
            case BCM56240_DEVICE_ID:
            case BCM56241_DEVICE_ID:
            case BCM56242_DEVICE_ID:
            case BCM56243_DEVICE_ID:
            case BCM56245_DEVICE_ID:
            case BCM56246_DEVICE_ID:
                *phy_addr = _katana_phy_ext_addr_mxqport[port-25];
                break;
            case BCM56442_DEVICE_ID:
            case BCM56447_DEVICE_ID:
                /* Shouldn't be here.. 442 only has 16 ports */
                break;
            default:
                
                break;
        }
    }
}
#endif /* BCM_KATANA_SUPPORT */
#ifdef BCM_SABER2_SUPPORT
void
_saber2_phy_addr_default(int unit, int port,
                          uint16 *phy_addr, uint16 *phy_addr_int)
{
   uint8 mdio_bus_ring=0;
   uint8 mdio_phy_addr=0;

   static int8 mapped_phy_addr[28+1]={0,
  /* Bus=0 PhySical Port versus Phy Addresses mapping */
               0x01, 0x01, 0x01,0x01, /* VIPER 0=1.. 4    */
               0x05, 0x05, 0x05,0x05, /* VIPER 1=5.. 8    */
               0x09, 0x09, 0x09,0x09, /* VIPER 2=9.. 12   */
               0x0d, 0x0d, 0x0d,0x0d, /* VIPER 3=13..16   */
               0x11, 0x11, 0x11,0x11, /* VIPER 4=17..20   */
               0x15, 0x15, 0x15,0x15, /* VIPER 5=21..24   */
  /* Bus=1 PhySical Port versus Phy Addresses mapping */
               0x01, 0x01, 0x01,0x01  /* EAGLE  =1.. 4    */
   };
   if ((port < 0 ) || (port >  (sizeof(mapped_phy_addr)-1))) {
        LOG_CLI((BSL_META_U(unit,
                            "Internal Error: Invalid port=%d \n"),port));
        return;
   }
   mdio_bus_ring = (port - 1) / 24;
   mdio_phy_addr = mapped_phy_addr[port] ;
   /* As per CMIC_CMCx_MIIM_PARAM */
   *phy_addr_int =    (1<<7)   | (mdio_bus_ring << 5) | mdio_phy_addr;
   *phy_addr     = /* (0<<7) */  (mdio_bus_ring << 5) | mdio_phy_addr;
}
#endif

#ifdef BCM_METROLITE_SUPPORT
void
_ml_phy_addr_default(int unit, int port,
        uint16 *phy_addr, uint16 *phy_addr_int)
{
    uint8 mdio_bus_ring=0;
    uint8 mdio_phy_addr=0;

    static int8 mapped_phy_addr[12+1]={0,
        /* Bus=0 PhySical Port versus Phy Addresses mapping */
        0x01, 0x01, 0x01, 0x01, /* VIPER 0=1.. 4    */
        /* Bus=4 PhySical Port versus Phy Addresses mapping */
        0x01, 0x01, 0x01, 0x01, /* EAGLE  =1.. 4    */
        0x05, 0x05, 0x05, 0x05  /* EAGLE  =5.. 8    */
    };
    if ((port < 0 ) || (port >  (sizeof(mapped_phy_addr)-1))) {
        LOG_CLI((BSL_META_U(unit,
                        "Internal Error: Invalid port=%d \n"),port));
        return;
    }

    if (port > 0 && port < 5) {
        mdio_bus_ring = 0;
    } else {
        mdio_bus_ring = 1;
    }

    mdio_phy_addr = mapped_phy_addr[port] ;
    /* As per CMIC_CMCx_MIIM_PARAM */
    *phy_addr_int =    (1<<7)   | (mdio_bus_ring << 5) | mdio_phy_addr;

    /* external phy address encoding */
    *phy_addr =    (1<<4) | (port - mdio_phy_addr);

    LOG_VERBOSE(BSL_LS_SOC_PHY, (BSL_META_U(unit,
        "_ml_phy_addr_default: port=%d phy_int=0x%x phy_ext=0x%x\n"),
        port, *phy_addr_int, *phy_addr));
}
#endif

#ifdef BCM_KATANA2_SUPPORT
void
_katana2_phy_addr_default(int unit, int port,
                          uint16 *phy_addr, uint16 *phy_addr_int)
{
   uint8 mdio_bus_ring=0;
   uint8 mdio_phy_addr=0;
   uint8 mxqblock=0;
   bcmMxqConnection_t connection_mode = bcmMqxConnectionUniCore;
#if 0
   /* Actual Addressing scheme but Looks like WARP Core Not happy with
      independent lane mode and works with AER scheme only*/
   /* Keeping this block for JUST REFERENCE */
   static int8 mapped_phy_addr[40+1]={0,
   /* Bus=0 PhySical Port versus Phy Addresses mapping */
   /* MXQ0=1..4(0x1..0x4)    MXQ1=5..8(0x5..0x8)    MXQ2=9..12(0x9..0xc)      */
   /* MXQ3=13..16(0xd..0x10) MXQ4=17..20(0x11..0x14) MXQ5=21..24(0x15..0x18)  */
                                     0x01, 0x02, 0x03,0x04, /* MXQ 0=1.. 4    */
                                     0x05, 0x06, 0x07,0x08, /* MXQ 1=5.. 8    */
                                     0x09, 0x0a, 0x0b,0x0c, /* MXQ 2=9.. 12   */
                                     0x0d, 0x0e, 0x0f,0x10, /* MXQ 3=13..16   */
                                     0x11, 0x12, 0x13,0x14, /* MXQ 4=17..20   */
                                     0x15, 0x16, 0x17,0x18, /* MXQ 5=21..24   */
  /* Bus=1 PhySical Port versus Phy Addresses mapping */
  /* MXQ6=25,35..37(0x1..0x4) MXQ7=26,38..40(0x5..0x8)                        */
  /* MXQ8=27,32..34(0x9..0xb) MXQ9=28,35..37(0xc..0x10)                       */
                                     0x01, 0x05, 0x09,0x0d, /* 25..28 */
                                     0x0e, 0x0f, 0x10,0x0a, /* 29..32 */
                                     0x0b, 0x0c, 0x02,0x03 ,/* 33..36 */
                                     0x04, 0x06, 0x07,0x08 ,/* 37..40 */
                                    };
#else
   static int8 mapped_phy_addr[40+1]={0,
   /* Bus=0 PhySical Port versus Phy Addresses mapping */
   /* MXQ0=1..4(0x1..0x4)    MXQ1=5..8(0x5..0x8)    MXQ2=9..12(0x9..0xc)      */
   /* MXQ3=13..16(0xd..0x10) MXQ4=17..20(0x11..0x14) MXQ5=21..24(0x15..0x18)  */
                                     0x01, 0x02, 0x03,0x04, /* MXQ 0=1.. 4    */
                                     0x05, 0x06, 0x07,0x08, /* MXQ 1=5.. 8    */
                                     0x09, 0x0a, 0x0b,0x0c, /* MXQ 2=9.. 12   */
                                     0x0d, 0x0e, 0x0f,0x10, /* MXQ 3=13..16   */
                                     0x11, 0x12, 0x13,0x14, /* MXQ 4=17..20   */
                                     0x15, 0x16, 0x17,0x18, /* MXQ 5=21..24   */
  /* Bus=1 PhySical Port versus Phy Addresses mapping */
  /* MXQ6=25,35..37(0x1..0x4) MXQ7=26,38..40(0x5..0x8)                        */
  /* MXQ8=27,32..34(0x9..0xb) MXQ9=28,35..37(0xc..0x10)                       */
                                     0x01, 0x05, /* 25..26 */
                                     0x09, /* WC0:27 */
                                     0x0d, /* WC1:28 */
                                     0x0d, 0x0d, 0x0d, /* WC1:29..31 */
                                     0x09, 0x09, 0x09, /* WC0:32..34 */
                                     0x02,0x03 ,/* 35..36 */
                                     0x04, 0x06, 0x07,0x08 ,/* 37..40 */
                                    };

#endif
   if ((port < 0 ) || (port >  (sizeof(mapped_phy_addr)-1))) {
        LOG_CLI((BSL_META_U(unit,
                            "Internal Error: Invalid port=%d \n"),port));
        return;
   }
   if (SOC_INFO(unit).olp_port[0] && (port == KT2_OLP_PORT)) {
       mdio_bus_ring = 2;
       mdio_phy_addr = 3;
   } else { 
       if (soc_katana2_get_port_mxqblock( unit,port,&mxqblock) != SOC_E_NONE) {
           LOG_CLI((BSL_META_U(unit,
                               "Unable to get mxqblock : Invalid port=%d \n"), port));
           return;
       }
       if (soc_katana2_get_phy_connection_mode(
           unit, port,mxqblock,&connection_mode) != SOC_E_NONE) {
           LOG_CLI((BSL_META_U(unit,
                               "Unable to get connection mode:Invalid port=%d\n"), port));
           return;
       }
       if ((mxqblock == 6) &&
           (connection_mode == bcmMqxConnectionWarpCore)) {
           if (port == 25) {
               port = 32;
           }
           if (port == 36) {
               port = 34;
           }
       }
       if ((mxqblock == 7) &&
           (connection_mode == bcmMqxConnectionWarpCore)) {
           if (port == 26) {
               port = 29;
           }
           if (port == 39) {
               port = 31;
           }
       }
       mdio_bus_ring = (port - 1) / 24;
       mdio_phy_addr = mapped_phy_addr[port] ;
   }

   /* As per CMIC_CMCx_MIIM_PARAM */
   *phy_addr_int =    (1<<7)   | (mdio_bus_ring << 5) | mdio_phy_addr;
   *phy_addr     = /* (0<<7) */  (mdio_bus_ring << 5) | mdio_phy_addr;
}
#endif /* BCM_KATANA2_SUPPORT */



/*
 * Function:
 *      _soc_phy_addr_default
 * Purpose:
 *      Return the default PHY addresses used to initialize the PHY map
 *      for a port.
 * Parameters:
 *      unit - StrataSwitch unit number
 *      phy_addr - (OUT) Outer PHY address
 *      phy_addr_int - (OUT) Intermediate PHY address, 0xff if none
 */

static void
_soc_phy_addr_default(int unit, int port,
                      uint16 *phy_addr, uint16 *phy_addr_int)
{

#ifdef BCM_XGS12_FABRIC_SUPPORT
    if (SOC_IS_XGS12_FABRIC(unit)) {
        _XGS12_FABRIC_PHY_ADDR_DEFAULT(unit, port, phy_addr, phy_addr_int);
    } else
#endif /* BCM_XGS12_FABRIC_SUPPORT */

#ifdef BCM_RAPTOR_SUPPORT
    if (SOC_IS_RAPTOR(unit) || SOC_IS_RAVEN(unit)) {
        _RAPTOR_PHY_ADDR_DEFAULT(unit, port, phy_addr, phy_addr_int);
    } else
#endif /* BCM_RAPTOR_SUPPORT */

#if defined(BCM_ENDURO_SUPPORT)
    if (SOC_IS_ENDURO(unit)) {
        _enduro_phy_addr_default(unit, port, phy_addr, phy_addr_int);
    } else
#endif /* BCM_ENDURO_SUPPORT */

#if defined(BCM_HURRICANE1_SUPPORT)
    if (SOC_IS_HURRICANE(unit)) {
        _hurricane_phy_addr_default(unit, port, phy_addr, phy_addr_int);
    } else
#endif /* BCM_HURRICANE1_SUPPORT */

#if defined(BCM_HURRICANE2_SUPPORT)
    if (SOC_IS_HURRICANE2(unit)) {
        _hurricane2_phy_addr_default(unit, port, phy_addr, phy_addr_int);
    } else
#endif /* BCM_HURRICANE2_SUPPORT */

#if defined(BCM_GREYHOUND_SUPPORT)
    if (SOC_IS_GREYHOUND(unit)) {
        _greyhound_phy_addr_default(unit, port, phy_addr, phy_addr_int);
    } else
#endif /* BCM_GREYHOUND_SUPPORT */

#if defined(BCM_HURRICANE3_SUPPORT)
    if (SOC_IS_HURRICANE3(unit)) {
        if (soc_feature(unit, soc_feature_wh2)) {
            _wolfhound2_phy_addr_default(unit, port, phy_addr, phy_addr_int);
        } else {
            _hurricane3_phy_addr_default(unit, port, phy_addr, phy_addr_int);
        }
    } else
#endif /* BCM_HURRICANE3_SUPPORT */

#if defined(BCM_GREYHOUND2_SUPPORT)
    if (SOC_IS_GREYHOUND2(unit)) {
        _greyhound2_phy_addr_default(unit, port, phy_addr, phy_addr_int);
    } else
#endif /* BCM_GREYHOUND2_SUPPORT */

#if defined(BCM_HELIX4_SUPPORT) 
    if (SOC_IS_HELIX4(unit)) {
        _helix4_phy_addr_default(unit, port, phy_addr, phy_addr_int);
    } else
#endif /* BCM_HELIX4_SUPPORT */

#if defined(BCM_TRIUMPH3_SUPPORT) 
    if (SOC_IS_TRIUMPH3(unit)) {
        _triumph3_phy_addr_default(unit, port, phy_addr, phy_addr_int);
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */

#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_APOLLO_SUPPORT) || \
    defined(BCM_VALKYRIE2_SUPPORT)
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        _triumph2_phy_addr_default(unit, port, phy_addr, phy_addr_int);
    } else
#endif /* BCM_TRIUMPH2_SUPPORT */

#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TRIUMPH(unit)) {
        _triumph_phy_addr_default(unit, port, phy_addr, phy_addr_int);
    } else
#endif /* BCM_TRIUMPH_SUPPORT */

#ifdef BCM_VALKYRIE_SUPPORT
    if (SOC_IS_VALKYRIE(unit)) {
        _valkyrie_phy_addr_default(unit, port, phy_addr, phy_addr_int);
    } else
#endif /* BCM_VALKYRIE_SUPPORT */

#ifdef BCM_DFE_SUPPORT
    if (SOC_IS_DFE(unit)) {
        _dfe_phy_addr_default(unit, port, phy_addr, phy_addr_int);
    } else
#endif /* BCM_DFE_SUPPORT */

#ifdef BCM_PETRA_SUPPORT
    if (SOC_IS_DPP(unit)) {
        _dpp_phy_addr_default(unit, port, phy_addr, phy_addr_int);
    } else
#endif /* BCM_PETRA_SUPPORT */

#ifdef BCM_TOMAHAWK_SUPPORT
#ifdef BCM_TOMAHAWK3_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        _tomahawk3_phy_addr_default(unit, port, phy_addr, phy_addr_int);
    } else
#endif /* BCM_TOMAHAWK3_SUPPORT */
#ifdef BCM_TOMAHAWK2_SUPPORT
    if (SOC_IS_TOMAHAWK2(unit)) {
        _tomahawk2_phy_addr_default(unit, port, phy_addr, phy_addr_int);
    } else
#endif /* BCM_TOMAHAWK2_SUPPORT */
    if (SOC_IS_TOMAHAWK(unit)) {
        _tomahawk_phy_addr_default(unit, port, phy_addr, phy_addr_int);
    } else
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        _hurricane4_phy_addr_default(unit, port, phy_addr, phy_addr_int);
    } else
#endif /* BCM_HURRICANE4_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
        _trident3_phy_addr_default(unit, port, phy_addr, phy_addr_int);
    } else
#endif /* BCM_TRIDENT3_SUPPORT */

#ifdef BCM_APACHE_SUPPORT
    if (SOC_IS_APACHE(unit)) {
        _apache_phy_addr_default(unit, port, phy_addr, phy_addr_int);
    } else
#endif /* BCM_APACHE_SUPPORT */
#ifdef BCM_TRIDENT_SUPPORT
#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(unit)) {
        _trident2_phy_addr_default(unit, port, phy_addr, phy_addr_int);
    } else
#endif /* BCM_TRIDENT2_SUPPORT */
    if (SOC_IS_TD_TT(unit)) {
        _trident_phy_addr_default(unit, port, phy_addr, phy_addr_int);
    } else
#endif /* BCM_TRIDENT_SUPPORT */
#ifdef BCM_TRIDENT2PLUS_SUPPORT
    if (SOC_IS_TRIDENT2PLUS(unit)) {
        _trident2_phy_addr_default(unit, port, phy_addr, phy_addr_int);
    } else
#endif /* BCM_TRIDENT2PLUS_SUPPORT */

#ifdef BCM_SHADOW_SUPPORT
    if (SOC_IS_SHADOW(unit)) {
        _shadow_phy_addr_default(unit, port, phy_addr, phy_addr_int);
    } else
#endif /* BCM_SHADOW_SUPPORT */

#ifdef BCM_SCORPION_SUPPORT
    if (SOC_IS_SCORPION(unit) &&
        _scorpion_phy_addr_default(unit, port, phy_addr, phy_addr_int)) {
    } else
#endif /* BCM_SCORPION_SUPPORT */

#ifdef BCM_HAWKEYE_SUPPORT
    if (SOC_IS_HAWKEYE(unit)) {
        _hawkeye_phy_addr_default(unit, port, phy_addr, phy_addr_int);
    } else
#endif /* BCM_HAWKEYE_SUPPORT */

#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANA(unit)) {
        _katana_phy_addr_default(unit, port, phy_addr, phy_addr_int);
    } else
#endif /* BCM_KATANA_SUPPORT */

#ifdef BCM_METROLITE_SUPPORT
    if (SOC_IS_METROLITE(unit)) {
        _ml_phy_addr_default(unit, port, phy_addr, phy_addr_int);
    } else
#endif /* BCM_METROLITE)SUPPORT */

#ifdef BCM_SABER2_SUPPORT
    if (SOC_IS_SABER2(unit)) {
        _saber2_phy_addr_default(unit, port, phy_addr, phy_addr_int);
    } else
#endif /* BCM_SABER2_SUPPORT */

#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        _katana2_phy_addr_default(unit, port, phy_addr, phy_addr_int);
    } else
#endif /* BCM_KATANA2_SUPPORT */

#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {
        _xgs3_phy_addr_default(unit, port, phy_addr, phy_addr_int);
    } else
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    {
        *phy_addr     = port + 1;
        *phy_addr_int = 0xff;
    }

    /*
     * Override the calculated address(es) with the per-port properties
     */
    *phy_addr = soc_property_port_get(unit, port,
                                      spn_PORT_PHY_ADDR,
                                      *phy_addr);
}

int
soc_phy_deinit(int unit)
{
    if (port_phy_addr[unit] != NULL) {
        sal_free(port_phy_addr[unit]);
    }

    port_phy_addr[unit] = NULL;
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_phy_init
 * Purpose:
 *      Initialize PHY software subsystem.
 * Parameters:
 *      unit - StrataSwitch unit number
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phy_init(int unit)
{
    uint16              phy_addr=0, phy_addr_int=0;
    soc_port_t          port;

    if (_phys_in_table < 0) {
        _init_phy_table();
    }

    if (port_phy_addr[unit] == NULL) {
        port_phy_addr[unit] = sal_alloc(sizeof(port_phy_addr_t) *
                                        SOC_MAX_NUM_PORTS,
                                        "port_phy_addr");
        if (port_phy_addr[unit] == NULL) {
            return SOC_E_MEMORY;
        }
    }

        sal_memset(port_phy_addr[unit], 0,
                   sizeof(port_phy_addr_t) * SOC_MAX_NUM_PORTS);

    PBMP_PORT_ITER(unit, port) {
        _soc_phy_addr_default(unit, port, &phy_addr, &phy_addr_int);

        SOC_IF_ERROR_RETURN
            (soc_phy_cfg_addr_set(unit,port,0, phy_addr));
        SOC_IF_ERROR_RETURN
            (soc_phy_cfg_addr_set(unit,port,SOC_PHY_INTERNAL, phy_addr_int));

        PHY_ADDR(unit, port)     = phy_addr;
        PHY_ADDR_INT(unit, port) = phy_addr_int;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_phy_port_init
 * Purpose:
 *      Initialize PHY software for a single port.
 * Parameters:
 *      unit - unit number
 *      port - port number
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phy_port_init(int unit, soc_port_t port)
{
    uint16              phy_addr=0, phy_addr_int=0;

    _soc_phy_addr_default(unit, port, &phy_addr, &phy_addr_int);

    SOC_IF_ERROR_RETURN
        (soc_phy_cfg_addr_set(unit,port,0, phy_addr));
    SOC_IF_ERROR_RETURN
        (soc_phy_cfg_addr_set(unit,port,SOC_PHY_INTERNAL, phy_addr_int));

    PHY_ADDR(unit, port)     = phy_addr;
    PHY_ADDR_INT(unit, port) = phy_addr_int;
    

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_phy_add_entry
 * Purpose:
 *      Add an entry to the PHY table
 * Parameters:
 *      entry - pointer to the entry
 * Returns:
 *      SOC_E_NONE - no error
 *      SOC_E_INIT - not initialized
 *      SOC_E_MEMORY - not no more space in table.
 */

int
soc_phy_add_entry(soc_phy_table_t *entry)
{
    assert(_phys_in_table >= 0);        /* Fatal if not already inited */

    if (_phys_in_table >= _MAX_PHYS) {
        return SOC_E_MEMORY;
    }

    phy_table[_phys_in_table++] = entry;

    return SOC_E_NONE;
}

#if defined(INCLUDE_SERDES_COMBO65)
/*
 * Function:
 *      _chk_serdescombo65
 * Purpose:
 *      Check function for Raven Combo SerDes PHYs
 * Parameters:
 *      See soc_phy_ident_f
 *      *pi is an output parameter filled with PHY info.
 * Returns:
 *      TRUE if this PHY matches.
 *      FALSE otherwise.
 * Notes:
 *      Sets the phy_info structure for this port.
 */

static int
_chk_serdescombo65(int unit, soc_port_t port, soc_phy_table_t *my_entry,
         uint16 phy_id0, uint16 phy_id1, soc_phy_info_t *pi)
{
    COMPILER_REFERENCE(unit);

    if (SOC_IS_RAVEN(unit)) {
        if (port == 1 || port == 2 || port == 4 || port == 5) {
            pi->phy_name = my_entry->phy_name;
            return TRUE;
        }
    }


    return FALSE;
}

#endif /* INCLUDE_SERDES_COMBO65 */

#if defined(INCLUDE_XGXS_16G)
/*
 * Function:
 *      _chk_xgxs16g1l
 * Purpose:
 *      Standard check function for PHYs (see soc_phy_ident_f)
 * Parameters:
 *      See soc_phy_ident_f
 *      *pi is an output parameter filled with PHY info.
 * Returns:
 *      TRUE if this PHY matches.
 *      FALSE otherwise.
 * Notes:
 *      Sets the phy_info structure for this port.
 */

static int
_chk_xgxs16g1l(int unit, soc_port_t port, soc_phy_table_t *my_entry,
         uint16 phy_id0, uint16 phy_id1, soc_phy_info_t *pi)
{
    if (my_entry->myNum == _phy_ident_type_get(phy_id0, phy_id1)) {
#ifdef BCM_KATANA_SUPPORT
         if (SOC_IS_KATANAX(unit) && IS_GE_PORT(unit,port)) {
             uint32 rval;
             int port_mode = 2;

             if (IS_MXQ_PORT(unit,port)) {
                 SOC_IF_ERROR_RETURN(READ_XPORT_MODE_REGr(unit, port, &rval));
                 port_mode = soc_reg_field_get(unit, XPORT_MODE_REGr, rval, PHY_PORT_MODEf);
             }
             if (port_mode) {
                 return TRUE;
             } else {
                 return FALSE;
             }
         } else
#endif /* BCM_KATANA_SUPPORT */

#ifdef BCM_TRIUMPH3_SUPPORT
        if (SOC_IS_TRIUMPH3(unit) && IS_GE_PORT(unit, port)) {
            pi->phy_name = my_entry->phy_name;
            return TRUE;
        }
#endif

        if (SOC_IS_SCORPION(unit) && !IS_GX_PORT(unit, port)) {
            pi->phy_name = my_entry->phy_name;
            return TRUE;
        }

#ifdef BCM_ENDURO_SUPPORT
    if (SOC_IS_ENDURO(unit) && (port > 25) && IS_GE_PORT(unit,port)) {
            pi->phy_name = my_entry->phy_name;
            return TRUE;      
    }
#endif /* BCM_ENDURO_SUPPORT */



    }
    return FALSE;
}
#endif /* INCLUDE_XGXS_16G */
#if defined(INCLUDE_XGXS_VIPER)
/*
 * Function:
 *      _chk_viper
 * Purpose:
 *      Standard check function for VIPER PHYs (see soc_phy_ident_f)
 * Parameters:
 *      See soc_phy_ident_f
 *      *pi is an output parameter filled with PHY info.
 * Returns:
 *      TRUE if this PHY matches.
 *      FALSE otherwise.
 * Notes:
 *      Sets the phy_info structure for this port.
 */
static int
_chk_viper(int unit, soc_port_t port, soc_phy_table_t *my_entry,
         uint16 phy_id0, uint16 phy_id1, soc_phy_info_t *pi)
{
   if (SOC_IS_SABER2(unit) || SOC_IS_GREYHOUND2(unit) ||
       soc_feature(unit, soc_feature_wh2)) {
       return TRUE;
   }
   return _chk_phy(unit, port, my_entry, phy_id0, phy_id1, pi);
}
#endif /* INCLUDE_XGXS_VIPER */

#if defined(INCLUDE_XGXS_WCMOD)
/*
 * Function:
 *      _chk_wcmod
 * Purpose:
 *      Standard check function for PHYs (see soc_phy_ident_f)
 * Parameters:
 *      See soc_phy_ident_f
 *      *pi is an output parameter filled with PHY info.
 * Returns:
 *      TRUE if this PHY matches.
 *      FALSE otherwise.
 * Notes:
 *      Sets the phy_info structure for this port.
 */

static int
_chk_wcmod(int unit, soc_port_t port, soc_phy_table_t *my_entry,
           uint16 phy_id0, uint16 phy_id1, soc_phy_info_t *pi)
{
    if (SOC_IS_TRIUMPH3(unit)) {
        if (IS_GE_PORT(unit, port) &&
            !soc_property_port_get(unit, port, "phy_wcmod", 1)) {
            /* Allow debug fallback to legacy XGXS16g1l PHY driver */
            return FALSE;
        }
    }

    return _chk_phy(unit, port, my_entry, phy_id0, phy_id1, pi);
}
#endif  /* INCLUDE_XGXS_WCMOD */

#if defined(INCLUDE_PHY_82109)
/*
 * Function:
 *      _chk_huracan
 * Purpose:
 *      Standard check function for Huracan PHY's (see soc_phy_ident_f)
 * Parameters:
 *      See soc_phy_ident_f
 *      *pi is an output parameter filled with PHY info.
 * Returns:
 *      TRUE if this PHY matches.
 *      FALSE otherwise.
 * Notes:
 *      Sets the phy_info structure for this port.
 */
static int
_chk_huracan(int unit, soc_port_t port, soc_phy_table_t *my_entry,
         uint16 phy_id0, uint16 phy_id1, soc_phy_info_t *pi)
{
    COMPILER_REFERENCE(unit);

    if (my_entry->myNum == _phy_id_BCM82109 ){
        switch (phy_id0) {
            case 0x2109:
            case 0x2108:
            case 0x2112:
            case 0x2181:
                pi->phy_name = my_entry->phy_name;
                return TRUE;
            default:
                break;
        }
    }

    return FALSE;
}
#endif

#ifdef INCLUDE_SERDES_65LP
/*
 * Function:
 *      _chk_serdes_65lp
 * Purpose:
 *      Standard check function for serdes 65LP
 * Parameters:
 *      See soc_phy_ident_f
 *      *pi is an output parameter filled with PHY info.
 * Returns:
 *      TRUE if this PHY matches.
 *      FALSE otherwise.
 * Notes:
 *      Sets the phy_info structure for this port.
 */
static int
_chk_serdes_65lp(int unit, soc_port_t port, soc_phy_table_t *my_entry,
         uint16 phy_id0, uint16 phy_id1, soc_phy_info_t *pi)
{
    COMPILER_REFERENCE(unit);
        if (my_entry->myNum == _phy_ident_type_get(phy_id0, phy_id1)) {
            pi->phy_name = my_entry->phy_name;
            return TRUE;
        }

    return FALSE;
}


#endif




/*
 * Function:
 *      _chk_phy
 * Purpose:
 *      Standard check function for PHYs (see soc_phy_ident_f)
 * Parameters:
 *      See soc_phy_ident_f
 *      *pi is an output parameter filled with PHY info.
 * Returns:
 *      TRUE if this PHY matches.
 *      FALSE otherwise.
 * Notes:
 *      Sets the phy_info structure for this port.
 */

static int
_chk_phy(int unit, soc_port_t port, soc_phy_table_t *my_entry,
         uint16 phy_id0, uint16 phy_id1, soc_phy_info_t *pi)
{
    COMPILER_REFERENCE(unit);

    if (my_entry->myNum == _phy_ident_type_get(phy_id0, phy_id1)) {
        pi->phy_name = my_entry->phy_name;
        return TRUE;
    }

    return FALSE;
}

#if defined(INCLUDE_PHY_SIMUL)
#if defined(SIM_ALL_PHYS)
#define USE_SIMULATION_PHY(unit, port)  (TRUE)
#else
#define USE_SIMULATION_PHY(unit, port) \
     (soc_property_port_get(unit, port, spn_PHY_SIMUL, 0))
#endif
#else
#define USE_SIMULATION_PHY(unit, port)  (FALSE)
#endif

/*
 * Function:
 *      _chk_null
 * Purpose:
 *      Check function for NULL phys.
 * Returns:
 *      True if this phy matches.
 *      False otherwise.
 */

static int
_chk_null(int unit, soc_port_t port,  soc_phy_table_t *my_entry,
          uint16 phy_id0, uint16 phy_id1, soc_phy_info_t *pi)
{
#if defined(INCLUDE_CES)
    uint16              dev_id;
    uint8               rev_id;
#endif

    if ((SAL_BOOT_PLISIM && (!SAL_BOOT_RTLSIM &&
        !USE_SIMULATION_PHY(unit, port))) ||
        !soc_property_get(unit, spn_PHY_ENABLE, 1) ||
        soc_property_port_get(unit, port, spn_PHY_NULL, 0) 
#ifdef BCM_TRIUMPH3_SUPPORT
        || ((SOC_IS_TRIUMPH3(unit) 
#ifdef BCM_TRIDENT2_SUPPORT
          || SOC_IS_TD2_TT2(unit)
#endif
        ) && SOC_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit, all), port))
#endif
        ) {
        pi->phy_name = my_entry->phy_name;

        return TRUE;
    }

#if defined(INCLUDE_CES)
/*
 * For CES on the 56441/2/3 where the CES mii port 
 * is internal and there is no physical phy set the
 * phy device to the null phy.
 */
    if (soc_feature(unit, soc_feature_ces) && port == 1) {
    soc_cm_get_id(unit, &dev_id, &rev_id);

    if (dev_id == BCM56441_DEVICE_ID ||
        dev_id == BCM56442_DEVICE_ID ||
        dev_id == BCM56443_DEVICE_ID) {
        pi->phy_name = my_entry->phy_name;
        return TRUE;
    }
    }
#endif

#if defined(INCLUDE_RCPU)
    if (SOC_IS_RCPU_ONLY(unit)) {
        pi->phy_name = my_entry->phy_name;
        return TRUE;
    }
#endif /* INCLUDE_RCPU */

    return FALSE;
}

#if defined(BCM_XGS3_SWITCH_SUPPORT) 
/*
 * Function:
 *      _chk_gmii
 * Purpose:
 *      Check function for GMII port.
 * Returns:
 *      True if this phy matches.
 *      False otherwise.
 */

static int
_chk_gmii(int unit, soc_port_t port,  soc_phy_table_t *my_entry,
          uint16 phy_id0, uint16 phy_id1, soc_phy_info_t *pi)
{
    if (IS_GMII_PORT(unit, port)) {
        pi->phy_name = my_entry->phy_name;

        return TRUE;
    }

    return FALSE;
}
#endif /* BCM_XGS3_SWITCH_SUPPORT */

#if defined(INCLUDE_PHY_SIMUL)

/*
 * Function:
 *      _chk_simul
 * Purpose:
 *      Check function for simulation phys.
 * Returns:
 *      True if this phy matches.
 *      False otherwise.
 */

static int
_chk_simul(int unit, soc_port_t port,  soc_phy_table_t *my_entry,
           uint16 phy_id0, uint16 phy_id1, soc_phy_info_t *pi)
{
    if (USE_SIMULATION_PHY(unit, port) && !SOC_IS_ARDON(unit)) {
        pi->phy_name = my_entry->phy_name;

        return TRUE;
    }

    return FALSE;
}
#endif /* include phy sim */

#undef USE_SIMULATION_PHY

#ifdef INCLUDE_XGXS_QSGMII65
/*
 * Function:
 *      _chk_qsgmii53314
 * Purpose:
 *      Check for using Internal 53314 SERDES Device 
 * Returns:
 *      TRUE if this PHY matches.
 *      FALSE otherwise.
 * Notes:
 *      This routine should be called after checking for external
 *      PHYs because it will default the PHY driver to the internal
 *      SERDES in the absense of any other PHY.
 */

static int
_chk_qsgmii53314(int unit, soc_port_t port, soc_phy_table_t *my_entry,
               uint16 phy_id0, uint16 phy_id1, soc_phy_info_t *pi)
{
    COMPILER_REFERENCE(my_entry);
    COMPILER_REFERENCE(phy_id0);
    COMPILER_REFERENCE(phy_id1);
    COMPILER_REFERENCE(pi);

    if (SOC_IS_HAWKEYE(unit) && port >= 9) {
        pi->phy_name = "Phy53314";
        return TRUE;
    }

    if (IS_GE_PORT(unit, port) && (SOC_IS_HURRICANE(unit) && (port >= 2) && (port <= 25))) {
        pi->phy_name = "Phy53314";
        return TRUE;
    }

    return FALSE;
}
#endif /* INCLUDE_XGXS_QSGMII65 */

#ifdef INCLUDE_PHY_54680
/*
 * Function:
 *      _chk_qgphy_5332x
 * Purpose:
 *      Check for using 5332x QGPHY Device 
 * Returns:
 *      TRUE if this PHY matches.
 *      FALSE otherwise.
 * Notes:
 *      Because The Id of QGPHY device of HKEEE is same as BCM54682E,
 *      the _chk_phy can be used for QGPHY device of HKEEE
 */

static int
_chk_qgphy_5332x(int unit, soc_port_t port, soc_phy_table_t *my_entry,
               uint16 phy_id0, uint16 phy_id1, soc_phy_info_t *pi)
{
    COMPILER_REFERENCE(my_entry);
    COMPILER_REFERENCE(phy_id0);
    COMPILER_REFERENCE(phy_id1);
    COMPILER_REFERENCE(pi);

    if (SOC_IS_HAWKEYE(unit) && 
        soc_feature (unit, soc_feature_eee) && 
        (port <= 8) && 
        (port != 0)) {
        pi->phy_name = "BCM53324";
        return TRUE;
    }

    return FALSE;
}
#endif /* INCLUDE_PHY_54680 */

#if defined(BCM_XGS3_SWITCH_SUPPORT)
/*
 * Function:
 *      _chk_fiber56xxx
 * Purpose:
 *      Check for using Internal 56XXX SERDES Device for fiber
 * Returns:
 *      TRUE if this PHY matches.
 *      FALSE otherwise.
 * Notes:
 *      This routine should be called after checking for external
 *      PHYs because it will default the PHY driver to the internal
 *      SERDES in the absense of any other PHY.
 */

static int
_chk_fiber56xxx(int unit, soc_port_t port, soc_phy_table_t *my_entry,
               uint16 phy_id0, uint16 phy_id1, soc_phy_info_t *pi)
{
    COMPILER_REFERENCE(my_entry);
    COMPILER_REFERENCE(phy_id0);
    COMPILER_REFERENCE(phy_id1);

    if (IS_GE_PORT(unit, port) && !IS_GMII_PORT(unit, port) &&
        !SOC_IS_HAWKEYE(unit) && !SOC_IS_HURRICANE2(unit) &&
        !SOC_IS_HURRICANE4(unit) &&
        soc_feature(unit, soc_feature_dodeca_serdes)) {
        pi->phy_name = "Phy56XXX";

        return TRUE;
    }

    return FALSE;
}
#endif /* BCM_XGS3_SWITCH_SUPPORT */

#if defined(INCLUDE_PHY_XGXS6)
/*
 * Function:
 *      _chk_unicore
 * Purpose:
 *      Check for Unicore, which may return two different PHY IDs
 *      depending on the current IEEE register mapping. One of
 *      these PHY IDs conflicts with the BCM5673/74 PHY ID, so
 *      we need to check the port type here as well.
 * Parameters:
 *      See soc_phy_ident_f
 *      *pi is an output parameter filled with PHY info.
 * Returns:
 *      TRUE if this PHY matches.
 *      FALSE otherwise.
 * Notes:
 *      Sets the phy_info structure for this port.
 */

static int
_chk_unicore(int unit, soc_port_t port, soc_phy_table_t *my_entry,
             uint16 phy_id0, uint16 phy_id1, soc_phy_info_t *pi)
{
    COMPILER_REFERENCE(unit);

    if (my_entry->myNum == _phy_ident_type_get(phy_id0, phy_id1) &&
        soc_feature(unit, soc_feature_xgxs_v6)) {
        pi->phy_name = my_entry->phy_name;
        return TRUE;
    }

    return FALSE;
}
#endif /* INCLUDE_PHY_XGXS6 */

static int
_chk_sfp_phy(int unit, soc_port_t port, soc_phy_table_t *my_entry,
         uint16 phy_id0, uint16 phy_id1, soc_phy_info_t *pi)
{
    COMPILER_REFERENCE(unit);

    if (soc_property_port_get(unit, port, spn_PHY_COPPER_SFP, 0)) {
        if (!(phy_id0 == (uint16)0xFFFF && phy_id1 == (uint16)0xFFFF)) {
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "_chk_sfp_phy: u=%d p=%d id0=0x%x, id1=0x%x,"
                                 " oui=0x%x,model=0x%x,rev=0x%x\n"),
                      unit, port,phy_id0,phy_id1,PHY_OUI(phy_id0, phy_id1),
                      PHY_MODEL(phy_id0, phy_id1),PHY_REV(phy_id0, phy_id1)));
            pi->phy_name = my_entry->phy_name;
            return TRUE;
        }
    }
    return FALSE;
}

#if defined(INCLUDE_PHY_8706)
static int
_chk_8706(int unit, soc_port_t port, soc_phy_table_t *my_entry,
             uint16 phy_id0, uint16 phy_id1, soc_phy_info_t *pi)
{
    if (my_entry->myNum == _phy_ident_type_get(phy_id0, phy_id1) ||
        soc_property_port_get(unit, port, spn_PHY_8706, FALSE)) {
        pi->phy_name = my_entry->phy_name;
        return TRUE;
    }
    return FALSE;
}

#endif /* INCLUDE_PHY_8706 */

#if defined(INCLUDE_PHY_8040)
static int
_chk_8040(int unit, soc_port_t port, soc_phy_table_t *my_entry,
             uint16 phy_id0, uint16 phy_id1, soc_phy_info_t *pi)
{
    if (my_entry->myNum == _phy_ident_type_get(phy_id0, phy_id1)) {
        pi->phy_name = my_entry->phy_name;
        return TRUE;
    }
    return FALSE;
}
#endif /* INCLUDE_PHY_8040 */

#if defined(INCLUDE_PHY_8072)
static int
_chk_8072(int unit, soc_port_t port, soc_phy_table_t *my_entry,
             uint16 phy_id0, uint16 phy_id1, soc_phy_info_t *pi)
{
    if (my_entry->myNum == _phy_ident_type_get(phy_id0, phy_id1) ||
        soc_property_port_get(unit, port, spn_PHY_8072, FALSE)) {
        pi->phy_name = my_entry->phy_name;
        return TRUE;
    }
    return FALSE;
}

#endif /* INCLUDE_PHY_8072 */

#if defined(INCLUDE_PHY_8481)
static int
_chk_8481(int unit, soc_port_t port, soc_phy_table_t *my_entry,
             uint16 phy_id0, uint16 phy_id1, soc_phy_info_t *pi)
{
    /* for GE/XE/HG ports only */
    if ( ! (IS_XE_PORT(unit, port) || IS_HG_PORT(unit, port)
                                   || IS_GE_PORT(unit, port)) ) {
        return FALSE;
    }

    if (my_entry->myNum == _phy_ident_type_get(phy_id0, phy_id1)) {
        pi->phy_name = my_entry->phy_name;
        return TRUE;
    }

    return FALSE;

}
#endif /* INCLUDE_PHY_8481 */

#if defined(INCLUDE_PHY_56XXX)
/*
 * Function:
 *      _chk_fiber56xxx_5601x
 * Purpose:
 *      Check for using Internal SERDES Device for fiber with shadow registers
 * Returns:
 *      TRUE if this PHY matches.
 *      FALSE otherwise.
 * Notes:
 *      This routine should be called after checking for external
 *      PHYs because it will default the PHY driver to the internal
 *      SERDES in the absense of any other PHY.
 */

static int
_chk_fiber56xxx_5601x(int unit, soc_port_t port, soc_phy_table_t *my_entry,
               uint16 phy_id0, uint16 phy_id1, soc_phy_info_t *pi)
{
    uint8       phy_addr;

    COMPILER_REFERENCE(my_entry);
    COMPILER_REFERENCE(phy_id0);
    COMPILER_REFERENCE(phy_id1);

    phy_addr = PORT_TO_PHY_ADDR_INT(unit, port);

    if (SOC_IS_RAPTOR(unit) && 
        soc_feature(unit, soc_feature_fe_ports) &&
        ((port == 4) || (port == 5)) &&
        (soc_property_port_get(unit, port, spn_SERDES_SHADOW_DRIVER, FALSE) ||
         _is_corrupted_reg(unit, phy_addr))) {
        pi->phy_name = "Phy56XXX5601x";
        return TRUE;
    }

    return FALSE;
}
#endif /* INCLUDE_PHY_56XXX */

#if defined(INCLUDE_SERDES_COMBO)
/*
 * Function:
 *      _chk_serdes_combo_5601x
 * Purpose:
 *      Check for using Internal SERDES Device for fiber with shadow registers
 * Returns:q
 *      TRUE if this PHY matches.
 *      FALSE otherwise.
 * Notes:
 *      This routine should be called after checking for external
 *      PHYs because it will default the PHY driver to the internal
 *      SERDES in the absense of any other PHY.
 */

static int
_chk_serdes_combo_5601x(int unit, soc_port_t port, soc_phy_table_t *my_entry,
               uint16 phy_id0, uint16 phy_id1, soc_phy_info_t *pi)
{
    uint8       phy_addr;

    COMPILER_REFERENCE(my_entry);
    COMPILER_REFERENCE(phy_id0);
    COMPILER_REFERENCE(phy_id1);

    phy_addr = PORT_TO_PHY_ADDR_INT(unit, port);

    if (SOC_IS_RAPTOR(unit) && 
        soc_feature(unit, soc_feature_fe_ports) &&
        ((port == 1) || (port == 2)) &&
        (soc_property_port_get(unit, port, spn_SERDES_SHADOW_DRIVER, FALSE) ||
         _is_corrupted_reg(unit, phy_addr))) {
        pi->phy_name = "COMBO5601x";
        return TRUE;
    }    

    return FALSE;
}
#endif /* INCLUDE_SERDES_COMBO */

#if defined(INCLUDE_PHY_53XXX)
/*
 * Function:
 *      _chk_fiber53xxx
 * Purpose:
 *      Check for using Internal 53XXX SERDES Device for fiber
 * Returns:
 *      TRUE if this PHY matches.
 *      FALSE otherwise.
 * Notes:
 *      This routine should be called after checking for external
 *      PHYs because it will default the PHY driver to the internal
 *      SERDES in the absense of any other PHY.
 */

static int
_chk_fiber53xxx(int unit, soc_port_t port, soc_phy_table_t *my_entry,
               uint16 phy_id0, uint16 phy_id1, soc_phy_info_t *pi)
{
    COMPILER_REFERENCE(my_entry);
    COMPILER_REFERENCE(phy_id0);
    COMPILER_REFERENCE(phy_id1);

    return FALSE;
}
#endif /* INCLUDE_PHY_53XXX */
/*
 * Function:
 *      _chk_default
 * Purpose:
 *      Select a default PHY driver.
 * Returns:
 *      TRUE
 * Notes:
 *      This routine always "finds" a default PHY driver and can
 *      be the last entry in the PHY table (or called explicitly).
 */

static int
_chk_default(int unit, soc_port_t port, soc_phy_table_t *my_entry,
             uint16 phy_id0, uint16 phy_id1, soc_phy_info_t *pi)
{
    pi->phy_name = my_entry->phy_name;

    return TRUE;
}

STATIC int
_forced_phy_probe(int unit, soc_port_t port,
                  soc_phy_info_t *pi, phy_ctrl_t *ext_pc)
{
#if defined(INCLUDE_PHY_SIMUL) || defined(BCM_XGS3_SWITCH_SUPPORT)
    phy_driver_t   *phyd = NULL;
#endif

#if defined(INCLUDE_PHY_SIMUL)
    /* Similarly, check for simulation driver */
    if (phyd == NULL &&
        _chk_simul(unit, port, &_simul_phy_entry, 0xffff, 0xffff, pi)) {
        ext_pc->pd  = _simul_phy_entry.driver;
        pi->phy_id0 = 0xffff;
        pi->phy_id1 = 0xffff;
    }
#endif

#if defined(BCM_XGS3_SWITCH_SUPPORT)
    /* Check for property forcing fiber mode on XGS3 switch */
    if (phyd == NULL &&
        soc_property_port_get(unit, port, spn_PHY_56XXX, FALSE) &&
        _chk_fiber56xxx(unit, port, &_fiber56xxx_phy_entry,
                        0xffff, 0xffff, pi)) {
        /* Correct internal SerDes PHY driver is already attached by
         * internal PHY probe. Therefore, just assign NULL to external PHY
         * driver.
         */
        ext_pc->pd   = NULL;
        pi->phy_id0  = 0xffff;
        pi->phy_id1  = 0xffff;
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

    /* Forced PHY will have internal PHY device ID */
    return SOC_E_NONE;
}

STATIC int
_int_phy_probe(int unit, soc_port_t port,
               soc_phy_info_t *pi, phy_ctrl_t *int_pc)
{
    uint16               phy_addr;
    uint16               phy_id0, phy_id1;
    int                  i;
    int                  rv;
    phy_driver_t         *int_phyd;
#if defined(BCM_KATANA2_SUPPORT)
    uint8 lane_num = 0;
    uint8 phy_mode = 0;
    uint8 chip_num = 0;
#endif

    phy_addr = int_pc->phy_id;
    int_phyd = NULL;
    /* There is always one extra "NULL" entry in the _int_phy_table to satisfy pedants */
    i = sizeof(_int_phy_table) / sizeof(_int_phy_table[0]) - 1;

    /* Make sure page 0 is mapped before reading for PHY dev ID */

#if defined(BCM_TRIUMPH3_SUPPORT) 
    if (SOC_IS_TRIUMPH3(unit) &&
        (SOC_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit,all), port))) {
        
        phy_id0 = phy_id1 = 0;
    } else 
#endif /* BCM_TRIUMPH3_SUPPORT */
    {
        if (SOC_IS_ARDON(unit)) {
            phy_id0 = 0x600d;
            phy_id1 = 0x8770;
        } else {

            if(SOC_IS_SABER2(unit)) {
                /* For saber2, the Sbus mdio access is expected along 
                 * with AER information 
                 */
                (void)int_pc->write(unit, phy_addr, 0x1f, 0xFFD0); 
                (void)int_pc->write(unit, phy_addr, 0x1e, 0); 
            }

            (void)int_pc->write(unit, phy_addr, 0x1f, 0); 

            (void)int_pc->read(unit, phy_addr, MII_PHY_ID0_REG, &phy_id0);
            (void)int_pc->read(unit, phy_addr, MII_PHY_ID1_REG, &phy_id1);

        }

    }

    
    pi->phy_id0       = phy_id0;
    pi->phy_id1       = phy_id1;
    pi->phy_addr_int  = phy_addr;
    int_pc->phy_id0   = phy_id0;
    int_pc->phy_id1   = phy_id1;
    int_pc->phy_oui   = PHY_OUI(phy_id0, phy_id1);
    int_pc->phy_model = PHY_MODEL(phy_id0, phy_id1);
    int_pc->phy_rev   = PHY_REV(phy_id0, phy_id1);

    for (i = i - 1; i >= 0; i--) {
        if ((_int_phy_table[i].checkphy)(unit, port, &_int_phy_table[i],
                                         phy_id0, phy_id1, pi)) {
            /* Device ID matches. Calls driver probe routine to confirm
             * that the driver is the appropriate one.
             * Many PHY devices has the same device ID but they are
             * actually different.
             */

            rv = PHY_PROBE(_int_phy_table[i].driver, unit, int_pc);
            if ((rv == SOC_E_NONE) || (rv == SOC_E_UNAVAIL)) {
                LOG_INFO(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                     "<%d> int Index = %d Mynum = %d %s\n"),
                                     rv, i, _int_phy_table[i].myNum, _int_phy_table[i].phy_name));
                int_phyd = _int_phy_table[i].driver;
                break;
            }
        }
    }

#if defined(INCLUDE_XGXS_16G)
    if (IS_GE_PORT(unit, port) && (&phy_xgxs16g1l_ge == int_phyd)) {
        /* using XGXS16G in independent lane mode on GE port.  */
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_INDEPENDENT_LANE);
#if defined(BCM_SCORPION_SUPPORT)
        if (SOC_IS_SCORPION(unit)) {
            switch(port) {
            case 25:
                pi->phy_name = "XGXS16G/1/0";
                int_pc->lane_num = 0;
                break;
            case 26:
                pi->phy_name = "XGXS16G/1/1";
                int_pc->lane_num = 1;      
                break;
            case 27:
                pi->phy_name = "XGXS16G/1/2";
                int_pc->lane_num = 2;
                break;
            case 28:
                pi->phy_name = "XGXS16G/1/3";
                int_pc->lane_num = 3;
                break;
            default:
                break;
            }
        }
#endif
    }
#endif

#if defined(INCLUDE_XGXS_HL65)
    if (IS_GE_PORT(unit, port) && (&phy_hl65_hg == int_phyd)) {
        /* If using HyperLite on GE port, use the HyperLite in independent
         * lane mode.
         */
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_INDEPENDENT_LANE);
        pi->phy_name = "HL65/1";
#if defined(BCM_HURRICANE1_SUPPORT)
        if (SOC_IS_HURRICANE(unit)) {
            int_pc->phy_mode = PHYCTRL_DUAL_LANE_PORT;
            int_pc->flags |= PHYCTRL_MDIO_ADDR_SHARE;
            switch (port) {
                case 26:
                    pi->phy_name = "HL65/0/0";
                    int_pc->lane_num = 0;
                    break;
                case 27:
                    pi->phy_name = "HL65/0/2";
                    int_pc->lane_num = 2;
                    break;
                case 28:
                    pi->phy_name = "HL65/1/0";
                    int_pc->lane_num = 0;
                    break;
                case 29:
                    pi->phy_name = "HL65/1/2";
                    int_pc->lane_num = 2;
                    break;
                default:
                    break;
            }
        }
#endif
#if defined(BCM_VALKYRIE_SUPPORT) || defined(BCM_TRIUMPH_SUPPORT)
        if (SOC_IS_VALKYRIE(unit) || SOC_IS_TRIUMPH(unit)) {
            switch(port) {
            case 2:
            case 6:
            case 14:
            case 26:
            case 27:
            case 35:
                pi->phy_name = "HL65/1/0";
                int_pc->lane_num = 0;
                break;

            case 3:
            case 7:
            case 15:
            case 32:
            case 36:
            case 43:
                pi->phy_name = "HL65/1/1";
                int_pc->lane_num = 1;
                break;

            case 4:
            case 16:
            case 18:
            case 33:
            case 44:
            case 46:
                pi->phy_name = "HL65/1/2";
                int_pc->lane_num = 2;
                break;

            case 5:
            case 17:
            case 19:
            case 34:
            case 45:
            case 47:
                pi->phy_name = "HL65/1/3";
                int_pc->lane_num = 3;
                break;

            default:
                break;

            }
        }
#endif /* BCM_VALKYRIE_SUPPORT */
#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_APOLLO_SUPPORT) || \
    defined(BCM_VALKYRIE2_SUPPORT)
        if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
            SOC_IS_VALKYRIE2(unit)) {
            switch(port) {
            case 30:
            case 34:
            case 38:
            case 42:
            case 46:
            case 50:
                pi->phy_name = "HL65/1/0";
                int_pc->lane_num = 0;
                break;

            case 31:
            case 35:
            case 39:
            case 43:
            case 47:
            case 51:
                pi->phy_name = "HL65/1/1";
                int_pc->lane_num = 1;
                break;

            case 32:
            case 36:
            case 40:
            case 44:
            case 48:
            case 52:
                pi->phy_name = "HL65/1/2";
                int_pc->lane_num = 2;
                break;

            case 33:
            case 37:
            case 41:
            case 45:
            case 49:
            case 53:
                pi->phy_name = "HL65/1/3";
                int_pc->lane_num = 3;
                break;

            default:
                break;

            }
        }
#endif /* BCM_TRIUMPH2_SUPPORT */
    }

#if defined(BCM_HURRICANE1_SUPPORT)
        if ((&phy_hl65_hg == int_phyd) && SOC_IS_HURRICANE(unit) && ((IS_HG_PORT(unit, port)) || IS_XE_PORT(unit, port))) {
            if ((port == 26) && (!SOC_PORT_VALID(unit, 27))) {
                LOG_INFO(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                     "Port 26 in combo mode\n")));
                pi->phy_name = "HL65/0";
            } else if ((port == 28) && (!SOC_PORT_VALID(unit, 29))) {
                LOG_INFO(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                     "Port 28 in combo mode\n")));
                pi->phy_name = "HL65/1";
            } else {
                LOG_INFO(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                     "Port %d in HGd mode\n"), port));
                PHY_FLAGS_SET(unit, port, PHY_FLAGS_INDEPENDENT_LANE);
                int_pc->phy_mode = PHYCTRL_DUAL_LANE_PORT;
                int_pc->flags |= PHYCTRL_MDIO_ADDR_SHARE;
                switch (port) {
                    case 26:
                        pi->phy_name = "HL65/0/0";
                        int_pc->lane_num = 0;
                        break;
                    case 27:
                        pi->phy_name = "HL65/0/2";
                        int_pc->lane_num = 2;
                        break;
                    case 28:
                        pi->phy_name = "HL65/1/0";
                        int_pc->lane_num = 0;
                        break;
                    case 29:
                        pi->phy_name = "HL65/1/2";
                        int_pc->lane_num = 2;
                        break;
                    default:
                        break;
                }
            }
        }
#endif /* BCM_HURRICANE_SUPPORT */

#endif /* INCLUDE_XGXS_HL65 */

#if defined(INCLUDE_XGXS_QSGMII65)
    if (IS_GE_PORT(unit, port) && (&phy_qsgmii65_ge == int_phyd)) {
        pi->phy_name = "QSGMII65";
#if defined(BCM_HAWKEYE_SUPPORT)
        if (SOC_IS_HAWKEYE(unit)) {
            switch(port) {
            case 9:
            case 17:
                pi->phy_name = "QSGMII65/0";
                int_pc->lane_num = 0;
                break;

            case 10:
            case 18:
                pi->phy_name = "QSGMII65/1";
                int_pc->lane_num = 1;
                break;

            case 11:
            case 19:
                pi->phy_name = "QSGMII65/2";
                int_pc->lane_num = 2;
                break;

            case 12:
            case 20:
                pi->phy_name = "QSGMII65/3";
                int_pc->lane_num = 3;
                break;

            case 13:
            case 21:
                pi->phy_name = "QSGMII65/4";
                int_pc->lane_num = 4;
                break;

            case 14:
            case 22:
                pi->phy_name = "QSGMII65/5";
                int_pc->lane_num = 5;
                break;

            case 15:
            case 23:
                pi->phy_name = "QSGMII65/6";
                int_pc->lane_num = 6;
                break;

            case 16:
            case 24:
                pi->phy_name = "QSGMII65/7";
                int_pc->lane_num = 7;
                break;

            default:
                break;

            }
        }
#endif /* SOC_IS_HAWKEYE */
#if defined(BCM_HURRICANE1_SUPPORT)
        if (SOC_IS_HURRICANE(unit)) {
            switch(port) {
            case 2:
            case 10:
            case 18:
                pi->phy_name = "QSGMII65/0";
                int_pc->lane_num = 0;
                break;

            case 3:
            case 11:
            case 19:
                pi->phy_name = "QSGMII65/1";
                int_pc->lane_num = 1;
                break;

            case 4:
            case 12:
            case 20:
                pi->phy_name = "QSGMII65/2";
                int_pc->lane_num = 2;
                break;

            case 5:
            case 13:
            case 21:
                pi->phy_name = "QSGMII65/3";
                int_pc->lane_num = 3;
                break;

            case 6:
            case 14:
            case 22:
                pi->phy_name = "QSGMII65/4";
                int_pc->lane_num = 4;
                break;

            case 7:
            case 15:
            case 23:
                pi->phy_name = "QSGMII65/5";
                int_pc->lane_num = 5;
                break;

            case 8:
            case 16:
            case 24:
                pi->phy_name = "QSGMII65/6";
                int_pc->lane_num = 6;
                break;

            case 9:
            case 17:
            case 25:
                pi->phy_name = "QSGMII65/7";
                int_pc->lane_num = 7;
                break;

            default:
                break;

            }
        }
#endif /* BCM_HURRICANE_SUPPORT */
    }
#endif /* INCLUDE_XGXS_QSGMII65 */

#if defined(BCM_GREYHOUND_SUPPORT)
    if (SOC_IS_GREYHOUND(unit) && 
            (IS_GE_PORT(unit, port) && !IS_XL_PORT(unit, port))) {
        int_pc->lane_num = (port - 2) % 8;  /* lane_num in {0~7} */
        int_pc->chip_num = (port - 2) / 8;  /* chip_num in {0,1}*/ 
        int_pc->phy_mode = PHYCTRL_QSGMII_CORE_PORT;
        int_pc->flags |= PHYCTRL_MDIO_ADDR_SHARE; 
    }
#endif  /* BCM_GREYHOUND_SUPPORT */

#if defined(BCM_KATANA2_SUPPORT)
    if ((SOC_IS_KATANA2(unit)) && (!SOC_IS_SABER2(unit)) &&
        (port <= KT2_MAX_PHYSICAL_PORTS)) {
        if (katana2_get_wc_phy_info(unit,port,&lane_num, &phy_mode, &chip_num) == SOC_E_NONE) {
            int_pc->lane_num = lane_num;
            int_pc->phy_mode = phy_mode;
            int_pc->chip_num = chip_num;
            int_pc->flags |= PHYCTRL_MDIO_ADDR_SHARE;
            /* PHY_FLAGS_SET(unit, port, PHY_FLAGS_INDEPENDENT_LANE); */
        }
        if ((SOC_INFO(unit).olp_port[0]) && (port == KT2_OLP_PORT)) {
            int_pc->lane_num = 2;
        }
    }

#endif

#if defined(INCLUDE_PHY_56XXX) && defined(INCLUDE_PHY_XGXS6)
    /* If we detetcted a Unicore driver for a GE port, attach internal SerDes
     * driver.
     * Current Unicore driver does not support external GE PHY.
     */
    if (IS_GE_PORT(unit, port) &&  (&phy_xgxs6_hg == int_phyd)) {
        if (_chk_fiber56xxx(unit, port, &_fiber56xxx_phy_entry,
                       phy_id0, phy_id1, pi)) {
            int_phyd = &phy_56xxxdrv_ge;
        }
    }
#endif /* INCLUDE_PHY_56XXX && INCLUDE_PHY_XGXS6 */


#if defined(INCLUDE_PHY_56XXX) 
    /* If we detected a shadow register driver, allocate driver data */
    if (&phy_56xxx_5601x_drv_ge == int_phyd) {
        serdes_5601x_sregs_t *sr;

        /* Allocate shadow registers */
        sr = sal_alloc(sizeof(serdes_5601x_sregs_t), 
                       "SERDES_COMBO shadow regs");
        if (sr == NULL) {
            return SOC_E_MEMORY;
        }
        int_pc->driver_data = sr;
    }
#endif /* INCLUDE_PHY_56XXX */

#if defined(INCLUDE_SERDES_COMBO)
    /* If we detected a shadow register driver, allocate driver data */
    if (&phy_serdescombo_5601x_ge == int_phyd) {
        serdescombo_5601x_sregs_t *sr;

        /* Allocate shadow registers */
        sr = sal_alloc(sizeof(serdescombo_5601x_sregs_t), 
                       "SERDES_COMBO shadow regs");
        if (sr == NULL) {
            return SOC_E_MEMORY;
        }
        int_pc->driver_data = sr;
    }
#endif /* INCLUDE_SERDES_COMBO */

#if defined(INCLUDE_PHY_53XXX)
    /* If we detetcted a Unicore driver for a GE port, attach internal SerDes
     * driver.
     * Current Unicore driver does not support external GE PHY.
     */
    if (IS_GE_PORT(unit, port)) {
        if (_chk_fiber53xxx(unit, port, &_fiber53xxx_phy_entry,
                       phy_id0, phy_id1, pi)) {
            int_phyd = &phy_53xxxdrv_ge;
        }
    }
#endif /* INCLUDE_PHY_53XXX */

#ifdef BCM_XGS_SUPPORT
    if (int_phyd == NULL) {
        if (IS_XE_PORT(unit, port) || IS_HG_PORT(unit, port)) {
            /* If no appropriate driver is installed in the phy driver table
             * use a default higig driver for XE port */
#if defined (INCLUDE_PHY_XGXS6)
            if (soc_feature(unit, soc_feature_xgxs_v6)) {
                int_phyd = &phy_xgxs6_hg;
            } else
#endif /* INCLUDE_PHY_XGXS6*/
#if defined(INCLUDE_PHY_XGXS5)
            if (soc_feature(unit, soc_feature_xgxs_v5)) {
                int_phyd = &phy_xgxs5_hg;
            } else
#endif /* INCLUDE_PHY_XGXS5 */
            {
                int_phyd = &phy_xgxs1_hg;
            }
        }
    }
#endif /* BCM_XGS_SUPPORT */


#if defined(BCM_XGS3_SWITCH_SUPPORT) 
    /* If GMII port, attach NULL PHY driver to
     * internal PHY driver. GMII port does not have SerDes.
     */
    if (_chk_gmii(unit, port, &_null_phy_entry, 0xffff, 0xffff, pi)) {
        int_phyd = _null_phy_entry.driver;
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

    int_pc->pd = int_phyd;

    return SOC_E_NONE;
}



int
_ext_phy_probe(int unit, soc_port_t port,
               soc_phy_info_t *pi, phy_ctrl_t *ext_pc)
{
    uint16               phy_addr;
    uint32               id0_addr, id1_addr;
    uint16               phy_id0=0, phy_id1=0;
    int                  i;
    phy_driver_t        *phyd;
    int                  rv;
    int                  cl45_override = 0;
    char                 *propval;
#if  defined(BCM_HURRICANE3_SUPPORT)
    uint16               dev_id = 0;
    uint8                rev_id = 0;
#endif /* BCM_HURRICANE3_SUPPORT */

    phy_addr = ext_pc->phy_id;
    phyd     = NULL;

    if (SOC_IS_ARDON(unit)) {
        ext_pc->pd = NULL;
        return SOC_E_NONE;
    }
    /* Clause 45 instead of Clause 22 MDIO access */
    if (soc_property_port_get(unit, port, spn_PORT_PHY_CLAUSE, 22) == 45) {
        cl45_override = 1;
    }

    if (IS_XE_PORT(unit, port) || IS_HG_PORT(unit, port) || IS_CE_PORT(unit, port) || SOC_IS_ARAD(unit)) {
        if (SOC_IS_ARAD(unit)) {
            if (cl45_override) {
                id0_addr = SOC_PHY_CLAUSE45_ADDR(PHY_C45_DEV_PMA_PMD,
                                         MII_PHY_ID0_REG);
                id1_addr = SOC_PHY_CLAUSE45_ADDR(PHY_C45_DEV_PMA_PMD,
                                         MII_PHY_ID1_REG);
            } else {
                id0_addr = MII_PHY_ID0_REG;
                id1_addr = MII_PHY_ID1_REG;
            }
        } else {
            id0_addr = SOC_PHY_CLAUSE45_ADDR(PHY_C45_DEV_PMA_PMD,
                                         MII_PHY_ID0_REG);
            id1_addr = SOC_PHY_CLAUSE45_ADDR(PHY_C45_DEV_PMA_PMD,
                                         MII_PHY_ID1_REG);
        }
    } else if ( IS_GE_PORT(unit, port) ) {
        if (cl45_override) {
            id0_addr = SOC_PHY_CLAUSE45_ADDR(PHY_C45_DEV_PMA_PMD,
                                     MII_PHY_ID0_REG);
            id1_addr = SOC_PHY_CLAUSE45_ADDR(PHY_C45_DEV_PMA_PMD,
                                     MII_PHY_ID1_REG);
        } else {
            id0_addr = MII_PHY_ID0_REG;
            id1_addr = MII_PHY_ID1_REG;
        }
    } else {
        id0_addr = MII_PHY_ID0_REG;
        id1_addr = MII_PHY_ID1_REG;
    }

    if (soc_property_port_get(unit, port,
                              spn_PORT_PHY_PRECONDITION_BEFORE_PROBE, 0)) {
       for (i = _phys_in_table - 1; i >= 0; i--) {
           rv = PHY_PRECONDITION_BEFORE_PROBE(phy_table[i]->driver,
                                               unit, ext_pc);
           if (rv == SOC_E_NONE) {
               break;
           }
       }
    }

    propval = soc_property_port_get_str(unit, port, spn_PORT_PHY_ID0);
    if (propval != NULL){
        phy_id0 = soc_property_port_get(unit, port, spn_PORT_PHY_ID0, 0xFFFF);
    }
    else{
        (void)ext_pc->read(unit, phy_addr, id0_addr, &phy_id0);
    }
    propval = soc_property_port_get_str(unit, port, spn_PORT_PHY_ID1);
    if (propval != NULL){
        phy_id1 = soc_property_port_get(unit, port, spn_PORT_PHY_ID1, 0xFFFF);
    }
    else{
        (void)ext_pc->read(unit, phy_addr, id1_addr, &phy_id1);
    }

    /* Look through table for match */
    for (i = _phys_in_table - 1; i >= 0; i--) {
        if ((phy_table[i]->checkphy)(unit, port, phy_table[i],
                                     phy_id0, phy_id1, pi)) {
            /* Device ID matches. Calls driver probe routine to confirm
             * that the driver is the appropriate one.
             * Many PHY devices has the same device ID but they are
             * actually different.
             */
            rv = PHY_PROBE(phy_table[i]->driver, unit, ext_pc);
            if ((rv == SOC_E_NONE) || (rv == SOC_E_UNAVAIL)) {
                LOG_INFO(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                     "<%d> ext Index = %d Mynum = %d %s\n"),
                                     rv, i, phy_table[i]->myNum, phy_table[i]->phy_name));

                phyd = phy_table[i]->driver;
                pi->phy_id0       = phy_id0;
                pi->phy_id1       = phy_id1;
                pi->phy_addr      = phy_addr;
                if (ext_pc->dev_name) {
                    pi->phy_name      = ext_pc->dev_name;
                }
                ext_pc->phy_id0   = phy_id0;
                ext_pc->phy_id1   = phy_id1;
                ext_pc->phy_oui   = PHY_OUI(phy_id0, phy_id1);
                ext_pc->phy_model = PHY_MODEL(phy_id0, phy_id1);
                ext_pc->phy_rev   = PHY_REV(phy_id0, phy_id1);


#ifdef BCM_HURRICANE3_SUPPORT
                if (soc_feature(unit,
                                soc_feature_hr3_egphy28_phyrev_override) &&
                    PHY_IS_BCM56160GPHY(ext_pc)) {
                    soc_cm_get_id(unit, &dev_id, &rev_id);
                    ext_pc->phy_rev = rev_id;
                }
#endif /* BCM_HURRICANE3_SUPPORT */

                PHY_FLAGS_SET(unit, port, PHY_FLAGS_EXTERNAL_PHY);

                break;
            }
        }
    }



#if defined(INCLUDE_PHY_54684)
    if (IS_GE_PORT(unit, port) && (phyd == &phy_54684drv_ge)) {
#if defined(BCM_HAWKEYE_SUPPORT)
        if (SOC_IS_HAWKEYE(unit)) {
            switch(port) {
            case 9:
            case 17:
                ext_pc->lane_num = 0;
                break;

            case 10:
            case 18:
                ext_pc->lane_num = 1;
                break;

            case 11:
            case 19:
                ext_pc->lane_num = 2;
                break;

            case 12:
            case 20:
                ext_pc->lane_num = 3;
                break;

            case 13:
            case 21:
                ext_pc->lane_num = 4;
                break;

            case 14:
            case 22:
                ext_pc->lane_num = 5;
                break;

            case 15:
            case 23:
                ext_pc->lane_num = 6;
                break;

            case 16:
            case 24:
                ext_pc->lane_num = 7;
                break;

            default:
                break;

            }
        }
#endif /* SOC_IS_HAWKEYE */
#if defined(BCM_HURRICANE1_SUPPORT)
        if (SOC_IS_HURRICANE(unit)) {
            switch(port) {
            case 2:
            case 10:
            case 18:
                ext_pc->lane_num = 0;
                break;

            case 3:
            case 11:
            case 19:
                ext_pc->lane_num = 1;
                break;

            case 4:
            case 12:
            case 20:
                ext_pc->lane_num = 2;
                break;

            case 5:
            case 13:
            case 21:
                ext_pc->lane_num = 3;
                break;

            case 6:
            case 14:
            case 22:
                ext_pc->lane_num = 4;
                break;

            case 7:
            case 15:
            case 23:
                ext_pc->lane_num = 5;
                break;

            case 8:
            case 16:
            case 24:
                ext_pc->lane_num = 6;
                break;

            case 9:
            case 17:
            case 25:
                ext_pc->lane_num = 7;
                break;

            default:
                break;

            }
        }
#endif /* BCM_HURRICANE_SUPPORT */
    }
#endif /* INCLUDE_PHY_54684 */

    ext_pc->pd = phyd;

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_phy_probe
 * Purpose:
 *      Probe the PHY on the specified port and return a pointer to the
 *      drivers for the device found.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      phyd_ptr - (OUT) Pointer to PHY driver (NULL on error)
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      Loop thru table making callback for each known PHY.
 *      We loop from the table from top to bottom so that user additions
 *      take precedence over default values.  The first checkphy function
 *      returning TRUE is used as the driver.
 */
int
soc_phy_probe(int unit, soc_port_t port, phy_ctrl_t *ext_pc,
              phy_ctrl_t *int_pc)
{
    soc_phy_info_t      *pi;
    uint16               phy_addr;
    uint16               phy_addr_int;

    /* Always use default addresses for probing.
     * This make sure that the external PHY probe works correctly even
     * when the device is hot plugged or the external PHY address is
     * overriden from previous probe.
     */
    SOC_IF_ERROR_RETURN
        (soc_phy_cfg_addr_get(unit,port,0,&phy_addr));
    SOC_IF_ERROR_RETURN
        (soc_phy_cfg_addr_get(unit,port,SOC_PHY_INTERNAL,&phy_addr_int));
    int_pc->phy_id = phy_addr_int;
    ext_pc->phy_id = phy_addr;

    /*
     * Characterize PHY by reading MII registers.
     */
    pi       = &SOC_PHY_INFO(unit, port);

    /* Probe for null PHY configuration first to avoid MII timeouts */

    if (_chk_null(unit, port, &_null_phy_entry, 0xffff, 0xffff, pi)) {
        ext_pc->pd     = _null_phy_entry.driver;
        int_pc->pd     = _null_phy_entry.driver;
    }

#ifdef PORTMOD_SUPPORT
    if (SOC_USE_PORTCTRL(unit)
#ifdef BCM_HURRICANE4_SUPPORT
        /* Skip GPHY's in HR4 */
        && !(SOC_IS_HURRICANE4(unit) && IS_EGPHY_PORT(unit, port))
#endif
        ) {
        /* If portmod is enabled all the phymod compliant 
         * phy probes would be handled by portmod. All the 
         * internal phys are phymod compliant. We just need 
         * to probe for legacy external phys */
        
        /* Assign catch all extphy_to_phymod pd table to intpc
         * to handle all the notifications for internal phy
         * from external phy */

        int_pc->pd     = &phy_ext_to_int_drv; 

    } else {
#endif /* PORTMOD_SUPPORT */
        /* Search for internal phy */
        if (NULL == int_pc->pd) {
            SOC_IF_ERROR_RETURN
                (_int_phy_probe(unit, port, pi, int_pc));
        }

#ifdef PORTMOD_SUPPORT
    }
#endif
    /* Search for external PHY */
    if (NULL == ext_pc->pd) {
        SOC_IF_ERROR_RETURN
            (_ext_phy_probe(unit, port, pi, ext_pc));
    }


    /* Override external PHY driver according to config settings */
    SOC_IF_ERROR_RETURN
        (_forced_phy_probe(unit, port, pi, ext_pc));


    if (ext_pc->pd != NULL) {
        if (IS_GMII_PORT(unit, port)) {
            /* If GMII port has external PHY, remove the NULL PHY driver
             * attached to internal PHY in _int_phy_probe().
             */
            int_pc->pd = NULL;
        }
        if ((int_pc->pd == _null_phy_entry.driver) &&
            (ext_pc->pd == _null_phy_entry.driver)) {
            /* Attach NULL PHY driver as external PHY driver */
            int_pc->pd = NULL;
        }
    }

   if ((ext_pc->pd == NULL) && (int_pc->pd == NULL) &&
        _chk_default(unit, port, &_default_phy_entry, 0xffff, 0xffff, pi)) {
        ext_pc->pd = _default_phy_entry.driver;
    }

    assert((ext_pc->pd != NULL) || (int_pc->pd != NULL));

    if (ext_pc->pd == NULL ||        /* No external PHY */
        ext_pc->pd == int_pc->pd) {  /* Forced PHY */
        /* Use internal address when application trying to access
         * external PHY.
         */
        pi->phy_addr = pi->phy_addr_int;

        /* If there is no external PHY, the internal PHY must be in
         * fiber mode.
         */
        if (soc_property_port_get(unit, port,
                                      spn_SERDES_FIBER_PREF, 1)) {
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_FIBER);
        } else {
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_FIBER);
        }
    }

    /*
     * The property if_tbi_port<X> can be used to force TBI mode on a
     * port.  The individual PHY drivers should key off this flag.
     */
    if (soc_property_port_get(unit, port, spn_IF_TBI, 0)) {
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_10B);
    }

    pi->an_timeout =
        soc_property_port_get(unit, port,
                              spn_PHY_AUTONEG_TIMEOUT, 250000);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "soc_phy_probe: port=%d addr=0x%x "
                         "id1=0x%x id0=0x%x flg=0x%x driver=\"%s\"\n"),
              port,
              pi->phy_addr, pi->phy_id0, pi->phy_id1,
              pi->phy_flags, pi->phy_name));

    return SOC_E_NONE;
}

int
soc_phy_check_sim(int unit, int port, phy_ctrl_t *pc)
{
#ifdef PHYMOD_SUPPORT
    uint16 phy_id;
#if !defined (EXCLUDE_PHYMOD_VIPER_SIM) && defined (PHYMOD_VIPER_SUPPORT)
    int viper_sim = 0;
#endif
#if !defined (EXCLUDE_PHYMOD_TSCE_SIM) && defined (PHYMOD_TSCE_SUPPORT)
    int tsce_sim = 0;
#endif
#if !defined (EXCLUDE_PHYMOD_TSCF_SIM) && defined (PHYMOD_TSCF_SUPPORT)
    int tscf_sim = 0;
#endif
#if !defined (EXCLUDE_PHYMOD_TSCBH_SIM) && defined (PHYMOD_TSCBH_SUPPORT)
    int tscbh_sim = 0;
#endif
#if !defined (EXCLUDE_PHYMOD_EAGLE_SIM) && defined (PHYMOD_EAGLE_SUPPORT)
    int eagle_sim = 0;
#endif
#if !defined (EXCLUDE_PHYMOD_QSGMIIE_SIM) && defined (PHYMOD_QSGMIIE_SUPPORT)
    int qsgmiie_sim = 0;
#endif
#if !defined (EXCLUDE_PHYMOD_QTCE_SIM) && defined (PHYMOD_QTCE_SUPPORT)
    int qtce_sim = 0;
#endif

    /* Get PHYAD associated with this port */
    soc_phy_cfg_addr_get(unit, port, SOC_PHY_INTERNAL, &phy_id);

#if !defined (EXCLUDE_PHYMOD_VIPER_SIM) && defined (PHYMOD_VIPER_SUPPORT)
    viper_sim = soc_property_port_get(unit, port, "viper_sim", 0);
    if (viper_sim) {
        if (SOC_IS_GREYHOUND2(unit) || soc_feature(unit, soc_feature_wh2)) {
            SOC_IF_ERROR_RETURN(
                soc_physim_add(unit, phy_id, &viper_sp2_sim_drv));
        }
        else {
            SOC_IF_ERROR_RETURN(
                soc_physim_add(unit, phy_id, &viper_sim_drv));
        }
        pc->read  = soc_physim_read;
        pc->write = soc_physim_write;
        /* Assume MDIO access if viper_sim >= 22 */
        if (viper_sim < 22) {
            pc->wrmask = soc_physim_wrmask;
        }
    }
#endif

#if !defined (EXCLUDE_PHYMOD_TSCE_SIM) && defined (PHYMOD_TSCE_SUPPORT)
    tsce_sim = soc_property_port_get(unit, port, "tsce_sim", 0);
    if (tsce_sim) {
        SOC_IF_ERROR_RETURN(
            soc_physim_add(unit, phy_id, &tsce_sim_drv));
        pc->read  = soc_physim_read;
        pc->write = soc_physim_write;
        /* Assume MDIO access if tsce_sim >= 22 */
        if (tsce_sim < 22) {
            pc->wrmask = soc_physim_wrmask;
        }
    }
#endif

#if !defined (EXCLUDE_PHYMOD_TSCF_SIM) && defined (PHYMOD_TSCF_SUPPORT)
    tscf_sim = soc_property_port_get(unit, port, "tscf_sim", 0);
    if (tscf_sim) {
        SOC_IF_ERROR_RETURN(
            soc_physim_add(unit, phy_id, &tscf_sim_drv));
        pc->read  = soc_physim_read;
        pc->write = soc_physim_write;
        /* Assume MDIO access if tscf_sim >= 22 */
        if (tscf_sim < 22) {
            pc->wrmask = soc_physim_wrmask;
        }
    }
#endif

#if !defined (EXCLUDE_PHYMOD_TSCBH_SIM) && defined (PHYMOD_TSCBH_SUPPORT)
    tscbh_sim = soc_property_port_get(unit, port, "tscbh_sim", 0);
    if (tscbh_sim) {
        SOC_IF_ERROR_RETURN(
            soc_physim_add(unit, phy_id, &tscbh_sim_drv));
        pc->read  = soc_physim_read;
        pc->write = soc_physim_write;
        /* Assume MDIO access if tscf_sim >= 22 */
        if (tscbh_sim < 22) {
            pc->wrmask = soc_physim_wrmask;
        }
    }
#endif

#if !defined (EXCLUDE_PHYMOD_EAGLE_SIM) && defined (PHYMOD_EAGLE_SUPPORT)
    eagle_sim = soc_property_port_get(unit, port, "eagle_sim", 0);
    if (eagle_sim) {
        SOC_IF_ERROR_RETURN(
            soc_physim_add(unit, phy_id, &eagle_sim_drv));
        pc->read  = soc_physim_read;
        pc->write = soc_physim_write;
        /* Assume MDIO access if tsce_sim >= 22 */
        if (eagle_sim < 22) {
            pc->wrmask = soc_physim_wrmask;
        }
    }
#endif

#if !defined (EXCLUDE_PHYMOD_QSGMIIE_SIM) && defined (PHYMOD_QSGMIIE_SUPPORT)
    qsgmiie_sim = soc_property_port_get(unit, port, "qsgmiie_sim", 0);
    if (qsgmiie_sim) {
        SOC_IF_ERROR_RETURN(
            soc_physim_add(unit, phy_id, &qsgmiie_sim_drv));
        pc->read  = soc_physim_read;
        pc->write = soc_physim_write;
        /* Assume MDIO access if qsgmiie_sim >= 22 */
        if (qsgmiie_sim < 22) {
            pc->wrmask = soc_physim_wrmask;
        }
    }
#endif

#if !defined (EXCLUDE_PHYMOD_QTCE_SIM) && defined (PHYMOD_QTCE_SUPPORT)
    qtce_sim = soc_property_port_get(unit, port, "qtce_sim", 0);
    if (qtce_sim) {
        SOC_IF_ERROR_RETURN(
            soc_physim_add(unit, phy_id, &qtce_sim_drv));
        pc->read  = soc_physim_read;
        pc->write = soc_physim_write;
        /* Assume MDIO access if qtce_sim >= 22 */
        if (qtce_sim < 22) {
            pc->wrmask = soc_physim_wrmask;
        }
    }
#endif
#endif /* PHYMOD_SUPPORT */

    return SOC_E_NONE;
}

void 
phyident_core_info_t_init(phyident_core_info_t* core_info) {
    core_info->mdio_addr = 0xFFFF;
    core_info->core_type = phyident_core_types_count;
    core_info->mld_index = PHYIDENT_INFO_NOT_SET;
    core_info->index_in_mld = PHYIDENT_INFO_NOT_SET;
    core_info->qsmii_lane = PHYIDENT_INFO_NOT_SET;
    core_info->first_phy_in_core = 0;
    core_info->nof_lanes_in_core = 4;
}

/*
 * Function:
 *      soc_phy_addr_multi_get
 * Purpose:
 *      Provide a list of internal MDIO addresses corresponding to a port.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      array_max - maximum number of elements in mdio_addr array.
 *      array_size - (OUT) number of valid elements returned in mdio_addr.
 *      mdio_addr - (OUT) list of internal mdio addresses for the port.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      Return the list of relevant internal MDIO addresses connected
 *      to a given port.
 *      Currently, only 100G+ ports require multiple MDIO addresses.
 */
int
soc_phy_addr_multi_get(int unit, soc_port_t port, int array_max,
                       int *array_size, phyident_core_info_t *core_info)
{
    int addr_num = 0;
    int i;

    if ((0 >= array_max) || (NULL == array_size) || (NULL == core_info)) {
        return SOC_E_PARAM;
    }

    for(i=0 ; i<array_max ; i++) {
        phyident_core_info_t_init(&(core_info[i]));
    }

#ifdef BCM_PETRA_SUPPORT
    if(SOC_IS_DPP(unit)) {
        return _dpp_phy_addr_multi_get(unit, port, 1, array_max, array_size, core_info);
    }
#endif

    if (IS_CL_PORT(unit, port)) {
        if (SOC_IS_TRIUMPH3(unit)) {
            uint32 mld_index = (0 == SOC_BLOCK_NUMBER(unit, SOC_PORT_BLOCK(unit, 
                          SOC_INFO(unit).port_l2p_mapping[port]))) ? 0 : 1;
            if (array_max > 1) {
                core_info[addr_num].mdio_addr = port_phy_addr[unit][port].int_addr;
                core_info[addr_num].core_type = phyident_core_type_wc;
                core_info[addr_num].mld_index = mld_index;
                core_info[addr_num].index_in_mld = 0;
                addr_num++;
            }
            if (array_max > 2) {
                core_info[addr_num].mdio_addr = core_info[addr_num - 1].mdio_addr + 4;
                core_info[addr_num].core_type = phyident_core_type_wc;
                core_info[addr_num].mld_index = mld_index;
                core_info[addr_num].index_in_mld = 1;
                addr_num++;
            }
            if (array_max > 3) {
                core_info[addr_num].mdio_addr = core_info[addr_num - 1].mdio_addr + 4;
                core_info[addr_num].core_type = phyident_core_type_wc;
                core_info[addr_num].mld_index = mld_index;
                core_info[addr_num].index_in_mld = 2;
                addr_num++;
            }
            if (array_max > 4) {
                core_info[addr_num].mdio_addr = mld_index ? 0xde : 0xdd;
                core_info[addr_num].core_type = phyident_core_type_mld;
                core_info[addr_num].mld_index = mld_index;
                core_info[addr_num].index_in_mld = 0;
                addr_num++;
            }
            *array_size = addr_num;
        } else {
            
            return SOC_E_UNAVAIL;
        }
    } else {
        SOC_IF_ERROR_RETURN
            (soc_phy_cfg_addr_get(unit, port, SOC_PHY_INTERNAL,
                                  &(core_info[0].mdio_addr)));
        core_info[0].core_type = phyident_core_type_wc;
        *array_size = 1;
    }
    return SOC_E_NONE;
}

/*
 * Variable:
 *      phy_id_map
 * Purpose:
 *      Map the PHY identifier register (OUI and device ID) into
 *      enumerated PHY type for prototypical devices.
 */

typedef struct phy_id_map_s {
    soc_known_phy_t     phy_num;        /* Enumerated PHY type */
    uint32              oui;            /* Device OUI */
    uint16              model;          /* Device Model */
    uint16              rev_map;        /* Device Revision */
} phy_id_map_t;

#define PHY_REV_ALL   (0xffff)
#define PHY_REV_0_3   (0x000f)
#define PHY_REV_4_7   (PHY_REV_0_3 << 4)
#define PHY_REV_8_11  (PHY_REV_0_3 << 8)
#define PHY_REV_12_15 (PHY_REV_0_3 << 12)
#define PHY_REV_0     (1 << 0)
#define PHY_REV_1     (1 << 1)
#define PHY_REV_2     (1 << 2)
#define PHY_REV_3     (1 << 3)
#define PHY_REV_4     (1 << 4)
#define PHY_REV_5     (1 << 5)
#define PHY_REV_6     (1 << 6)
#define PHY_REV_7     (1 << 7)
#define PHY_REV_8     (1 << 8)
#define PHY_REV_9     (1 << 9)
#define PHY_REV_10    (1 << 10)
#define PHY_REV_11    (1 << 11)
#define PHY_REV_12    (1 << 12)

STATIC phy_id_map_t phy_id_map[] = {
    { _phy_id_BCM5218,      PHY_BCM5218_OUI,        PHY_BCM5218_MODEL,
      PHY_REV_ALL },
    { _phy_id_BCM5220,      PHY_BCM5220_OUI,        PHY_BCM5220_MODEL,
      PHY_REV_ALL }, /* & 5221 */
    { _phy_id_BCM5226,      PHY_BCM5226_OUI,        PHY_BCM5226_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM5228,      PHY_BCM5228_OUI,        PHY_BCM5228_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM5238,      PHY_BCM5238_OUI,        PHY_BCM5238_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM5248,      PHY_BCM5248_OUI,        PHY_BCM5248_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM5400,      PHY_BCM5400_OUI,        PHY_BCM5400_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM5401,      PHY_BCM5401_OUI,        PHY_BCM5401_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM5402,      PHY_BCM5402_OUI,        PHY_BCM5402_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM5404,      PHY_BCM5404_OUI,        PHY_BCM5404_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM5411,      PHY_BCM5411_OUI,        PHY_BCM5411_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM5421,      PHY_BCM5421_OUI,        PHY_BCM5421_MODEL,
      PHY_REV_ALL}, /* & 5421S */
    { _phy_id_BCM5424,      PHY_BCM5424_OUI,        PHY_BCM5424_MODEL,
      PHY_REV_ALL}, /* & 5434 */
    { _phy_id_BCM5464,      PHY_BCM5464_OUI,        PHY_BCM5464_MODEL,
      PHY_REV_ALL}, /* & 5464S */
    { _phy_id_BCM5466,       PHY_BCM5466_OUI,       PHY_BCM5466_MODEL,
      PHY_REV_ALL}, /* & 5466S */
    { _phy_id_BCM5461,       PHY_BCM5461_OUI,       PHY_BCM5461_MODEL,
      PHY_REV_ALL}, /* & 5461S */
    { _phy_id_BCM5461,       PHY_BCM5462_OUI,       PHY_BCM5462_MODEL,
      PHY_REV_ALL}, /* & 5461D */
    { _phy_id_BCM5478,       PHY_BCM5478_OUI,       PHY_BCM5478_MODEL,
      PHY_REV_ALL}, /*   5478 */
    { _phy_id_BCM5488,       PHY_BCM5488_OUI,       PHY_BCM5488_MODEL,
      PHY_REV_ALL}, /*   5488 */
    { _phy_id_BCM5482,       PHY_BCM5482_OUI,       PHY_BCM5482_MODEL,
      PHY_REV_ALL}, /* & 5482S */
    { _phy_id_BCM5481,       PHY_BCM5481_OUI,       PHY_BCM5481_MODEL,
      PHY_REV_ALL}, /* & 5481 */
    { _phy_id_BCM54980,      PHY_BCM54980_OUI,      PHY_BCM54980_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM54980C,     PHY_BCM54980C_OUI,     PHY_BCM54980C_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM54980V,     PHY_BCM54980V_OUI,     PHY_BCM54980V_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM54980VC,    PHY_BCM54980VC_OUI,    PHY_BCM54980VC_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM54680,      PHY_BCM54680_OUI,      PHY_BCM54680_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM54880,      PHY_BCM54880_OUI,      PHY_BCM54880_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM54880E,     PHY_BCM54880E_OUI,     PHY_BCM54880E_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM54680E,     PHY_BCM54680E_OUI,     PHY_BCM54680E_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM52681E,     PHY_BCM52681E_OUI,     PHY_BCM52681E_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM54881,      PHY_BCM54881_OUI,      PHY_BCM54881_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM54810,      PHY_BCM54810_OUI,      PHY_BCM54810_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM54811,      PHY_BCM54811_OUI,      PHY_BCM54811_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM54684,      PHY_BCM54684_OUI,      PHY_BCM54684_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM54684E,     PHY_BCM54684E_OUI,     PHY_BCM54684E_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM54682,      PHY_BCM54682E_OUI,     PHY_BCM54682E_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM54685,      PHY_BCM54685_OUI,      PHY_BCM54685_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM54640,      PHY_BCM54640_OUI,      PHY_BCM54640_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM54616,      PHY_BCM54616_OUI,      PHY_BCM54616_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM54618E,     PHY_BCM54618E_OUI,     PHY_BCM54618E_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM54618SE,    PHY_BCM54618SE_OUI,    PHY_BCM54618SE_MODEL,
      PHY_REV_ALL},

#ifdef INCLUDE_MACSEC
#if defined(INCLUDE_PHY_54380)
    { _phy_id_BCM54380,      PHY_BCM54380_OUI,      PHY_BCM54380_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM54382,      PHY_BCM54380_OUI,      PHY_BCM54382_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM54340,      PHY_BCM54380_OUI,      PHY_BCM54340_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM54385,      PHY_BCM54380_OUI,      PHY_BCM54385_MODEL,
      PHY_REV_ALL},
#endif
#endif    

#if defined(INCLUDE_PHY_542XX)
    { _phy_id_BCM54182,      PHY_BCM5418X_OUI,      PHY_BCM54182_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM54185,      PHY_BCM5418X_OUI,      PHY_BCM54185_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM54180,      PHY_BCM5418X_OUI,      PHY_BCM54180_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM54140,      PHY_BCM5418X_OUI,      PHY_BCM54140_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM54192,      PHY_BCM5419X_OUI,      PHY_BCM54192_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM54195,      PHY_BCM5419X_OUI,      PHY_BCM54195_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM54190,      PHY_BCM5419X_OUI,      PHY_BCM54190_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM54194,      PHY_BCM5419X_OUI,      PHY_BCM54194_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM54210,      PHY_BCM542XX_OUI,      PHY_BCM54210_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM54220,      PHY_BCM542XX_OUI,      PHY_BCM54220_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM54280,      PHY_BCM542XX_OUI,      PHY_BCM54280_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM54282,      PHY_BCM542XX_OUI,      PHY_BCM54282_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM54240,      PHY_BCM542XX_OUI,      PHY_BCM54240_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM54285,      PHY_BCM542XX_OUI,      PHY_BCM54285_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM54290,      PHY_BCM542XX_OUI,      PHY_BCM54290_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM54292,      PHY_BCM542XX_OUI,      PHY_BCM54292_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM54294,      PHY_BCM542XX_OUI,      PHY_BCM54294_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM54295,      PHY_BCM542XX_OUI,      PHY_BCM54295_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM54296,      PHY_BCM542XX_OUI,      PHY_BCM54296_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM5428x,      PHY_BCM542XX_OUI,      PHY_BCM5428X_MODEL,
      PHY_REV_ALL},
#endif

#if defined(INCLUDE_PHY_8806X)
    { _phy_id_BCM8806x,      PHY_BCM8806X_OUI,      PHY_BCM8806X_MODEL,
      PHY_REV_ALL},
#endif /* INCLUDE_PHY_8806X */

#ifdef INCLUDE_MACSEC
#if defined(INCLUDE_PHY_54580)
    { _phy_id_BCM54584,      PHY_BCM54580_OUI,      PHY_BCM54584_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM54580,      PHY_BCM54580_OUI,      PHY_BCM54580_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM54540,      PHY_BCM54580_OUI,      PHY_BCM54540_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM54585,      PHY_BCM54580_OUI,      PHY_BCM54585_MODEL,
      PHY_REV_ALL},
#endif
#endif /* INCLUDE_MACSEC */
    { _phy_id_BCM8011,       PHY_BCM8011_OUI,       PHY_BCM8011_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM8040,       PHY_BCM8040_OUI,       PHY_BCM8040_MODEL,
      PHY_REV_0 | PHY_REV_1 | PHY_REV_2},
    { _phy_id_BCM8703,       PHY_BCM8703_OUI,       PHY_BCM8703_MODEL,
      PHY_REV_0 | PHY_REV_1 | PHY_REV_2},
    { _phy_id_BCM8704,       PHY_BCM8704_OUI,       PHY_BCM8704_MODEL,
      PHY_REV_3},
    { _phy_id_BCM8705,       PHY_BCM8705_OUI,       PHY_BCM8705_MODEL,
      PHY_REV_4},
    { _phy_id_BCM8706,       PHY_BCM8706_OUI,       PHY_BCM8706_MODEL,
      PHY_REV_5},
    { _phy_id_BCM8750,       PHY_BCM8750_OUI,       PHY_BCM8750_MODEL,
      PHY_REV_0},
    { _phy_id_BCM8752,       PHY_BCM8752_OUI,       PHY_BCM8752_MODEL,
      PHY_REV_0},
    { _phy_id_BCM8754,       PHY_BCM8754_OUI,       PHY_BCM8754_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM8072,       PHY_BCM8072_OUI,       PHY_BCM8072_MODEL,
      PHY_REV_5},
    { _phy_id_BCM8727,       PHY_BCM8727_OUI,       PHY_BCM8727_MODEL,
      PHY_REV_6},
    { _phy_id_BCM8073,       PHY_BCM8073_OUI,       PHY_BCM8073_MODEL,
      PHY_REV_6},
    { _phy_id_BCM8747,       PHY_BCM8747_OUI,       PHY_BCM8747_MODEL,
      PHY_REV_7},
    { _phy_id_BCM84740,      PHY_BCM84740_OUI,      PHY_BCM84740_MODEL,
      PHY_REV_0},
    { _phy_id_BCM84164,      PHY_BCM84164_OUI,      PHY_BCM84164_MODEL,
      PHY_REV_0_3},
    { _phy_id_BCM84758,      PHY_BCM84758_OUI,      PHY_BCM84758_MODEL,
      PHY_REV_0_3},
    { _phy_id_BCM84780,      PHY_BCM84780_OUI,      PHY_BCM84780_MODEL,
      PHY_REV_4_7},
    { _phy_id_BCM84784,      PHY_BCM84784_OUI,      PHY_BCM84784_MODEL,
      PHY_REV_8_11},
#ifdef INCLUDE_MACSEC
#ifdef INCLUDE_PHY_8729
    { _phy_id_BCM8729,       PHY_BCM5927_OUI,       PHY_BCM5927_MODEL,
      PHY_REV_4},
    { _phy_id_BCM8729,       PHY_BCM8729_OUI,       PHY_BCM8729_MODEL,
      PHY_REV_12},
#endif
#ifdef INCLUDE_PHY_84756
    { _phy_id_BCM84756,      PHY_BCM84756_OUI,      PHY_BCM84756_MODEL,
      PHY_REV_0_3},
    { _phy_id_BCM84757,      PHY_BCM84756_OUI,      PHY_BCM84756_MODEL,
      PHY_REV_8_11},
    { _phy_id_BCM84759,      PHY_BCM84756_OUI,      PHY_BCM84756_MODEL,
      PHY_REV_4_7},
#endif
#ifdef INCLUDE_PHY_84334
    { _phy_id_BCM84334,      PHY_BCM84334_OUI,      PHY_BCM84334_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM84333,      PHY_BCM84333_OUI,      PHY_BCM84333_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM84336,      PHY_BCM84336_OUI,      PHY_BCM84336_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM84844,      PHY_BCM84844_OUI,      PHY_BCM84844_MODEL,
      PHY_REV_ALL},
#endif
#ifdef INCLUDE_PHY_84749
    { _phy_id_BCM84749,      PHY_BCM84749_OUI,      PHY_BCM84749_MODEL,
      PHY_REV_0},
    { _phy_id_BCM84729,      PHY_BCM84729_OUI,      PHY_BCM84729_MODEL,
      PHY_REV_0},
#endif
#endif  /* INCLUDE_MACSEC */
#if defined(INCLUDE_FCMAP) || defined(INCLUDE_MACSEC)
#ifdef INCLUDE_PHY_84756
    { _phy_id_BCM84756,      PHY_BCM84756_OUI,      PHY_BCM84756_MODEL,
      PHY_REV_0_3},
    { _phy_id_BCM84757,      PHY_BCM84756_OUI,      PHY_BCM84756_MODEL,
      PHY_REV_8_11},
    { _phy_id_BCM84759,      PHY_BCM84756_OUI,      PHY_BCM84756_MODEL,
      PHY_REV_4_7},
#endif  /*INCLUDE_PHY_84756 */
#endif  /* INCLUDE_FCMAP || INCLUDE_MACSEC */
#ifdef INCLUDE_PHY_84728 
    { _phy_id_BCM84707,      PHY_BCM84707_OUI,      PHY_BCM84707_MODEL, 
      PHY_REV_4_7}, 
    { _phy_id_BCM84073,      PHY_BCM84073_OUI,      PHY_BCM84073_MODEL, 
      PHY_REV_8_11}, 
    { _phy_id_BCM84074,      PHY_BCM84074_OUI,      PHY_BCM84074_MODEL, 
      PHY_REV_12_15}, 
    { _phy_id_BCM84728,      PHY_BCM84728_OUI,      PHY_BCM84728_MODEL, 
      PHY_REV_0_3}, 
    { _phy_id_BCM84748,      PHY_BCM84748_OUI,      PHY_BCM84748_MODEL, 
      PHY_REV_4_7}, 
    { _phy_id_BCM84727,      PHY_BCM84727_OUI,      PHY_BCM84727_MODEL, 
      PHY_REV_8_11}, 
    { _phy_id_BCM84747,      PHY_BCM84747_OUI,      PHY_BCM84747_MODEL, 
      PHY_REV_12_15}, 
    { _phy_id_BCM84762,      PHY_BCM84762_OUI,      PHY_BCM84762_MODEL, 
      PHY_REV_0_3}, 
    { _phy_id_BCM84764,      PHY_BCM84764_OUI,      PHY_BCM84764_MODEL, 
      PHY_REV_4_7}, 
    { _phy_id_BCM84042,      PHY_BCM84042_OUI,      PHY_BCM84042_MODEL, 
      PHY_REV_8_11}, 
    { _phy_id_BCM84044,      PHY_BCM84044_OUI,      PHY_BCM84044_MODEL, 
      PHY_REV_12_15}, 
#endif 
    { _phy_id_BCM8481x,      PHY_BCM8481X_OUI,      PHY_BCM8481X_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM84812ce,    PHY_BCM84812CE_OUI,    PHY_BCM84812CE_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM84821,      PHY_BCM84821_OUI,      PHY_BCM84821_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM84822,      PHY_BCM84822_OUI,      PHY_BCM84822_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM84823,      PHY_BCM84823_OUI,      PHY_BCM84823_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM84833,      PHY_BCM84833_OUI,      PHY_BCM84833_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM84834,      PHY_BCM84834_OUI,      PHY_BCM84834_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM84835,      PHY_BCM84835_OUI,      PHY_BCM84835_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM84836,      PHY_BCM84836_OUI,      PHY_BCM84836_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM84844,      PHY_BCM84844_OUI,      PHY_BCM84844_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM84846,      PHY_BCM84846_OUI,      PHY_BCM84846_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM84848,      PHY_BCM84848_OUI,      PHY_BCM84848_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM84858,      PHY_BCM84858_OUI,      PHY_BCM84858_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM84856,      PHY_BCM84856_OUI,      PHY_BCM84856_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM84860,      PHY_BCM84860_OUI,      PHY_BCM84860_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM84861,      PHY_BCM84861_OUI,      PHY_BCM84861_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM84864,      PHY_BCM84864_OUI,      PHY_BCM84864_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM84868,      PHY_BCM84868_OUI,      PHY_BCM84868_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM84888,      PHY_BCM84888_OUI,      PHY_BCM84888_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM84884,      PHY_BCM84884_OUI,      PHY_BCM84884_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM84888E,     PHY_BCM84888E_OUI,     PHY_BCM84888E_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM84884E,     PHY_BCM84884E_OUI,     PHY_BCM84884E_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM84881,      PHY_BCM84881_OUI,      PHY_BCM84881_MODEL,
       PHY_REV_ALL},
    { _phy_id_BCM84880,      PHY_BCM84880_OUI,      PHY_BCM84880_MODEL,
       PHY_REV_ALL},
    { _phy_id_BCM84888S,     PHY_BCM84888S_OUI,     PHY_BCM84888S_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM84885,      PHY_BCM84885_OUI,      PHY_BCM84885_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM84886,      PHY_BCM84886_OUI,      PHY_BCM84886_MODEL,
       PHY_REV_ALL},
    { _phy_id_BCM84887,      PHY_BCM84887_OUI,      PHY_BCM84887_MODEL,
       PHY_REV_ALL},
#ifdef INCLUDE_PHY_84328
    { _phy_id_BCM84328,      PHY_BCM84328_OUI,      PHY_BCM84328_MODEL,
      PHY_REV_ALL},
#endif
#ifdef INCLUDE_PHY_84793
    { _phy_id_BCM84793,      PHY_BCM84793_OUI,      PHY_BCM84793_MODEL,
      PHY_REV_ALL},
#endif
#ifdef INCLUDE_PHY_82328
    { _phy_id_BCM82328,      PHY_BCM82328_OUI,      PHY_BCM82328_MODEL,
      PHY_REV_ALL},
#endif
#ifdef INCLUDE_PHY_82780
    { _phy_id_BCM82780,      PHY_BCM82780_OUI,      PHY_BCM82780_MODEL,
      PHY_REV_ALL},
#endif

#ifdef INCLUDE_PHY_84740
    { _phy_id_BCM84318,      PHY_BCM84318_OUI,      PHY_BCM84318_MODEL,
      PHY_REV_ALL},
#endif
    { _phy_id_BCMXGXS1,      PHY_BCMXGXS1_OUI,      PHY_BCMXGXS1_MODEL,
      PHY_REV_ALL},
    /*
     * HL65 has the same device ID as XGXS1.
     *{ _phy_id_XGXS_HL65,   PHY_XGXS_HL65_OUI,     PHY_XGXS_HL65_MODEL,
     * PHY_REV_ALL }
     */
    { _phy_id_BCMXGXS2,      PHY_BCMXGXS2_OUI,      PHY_BCMXGXS2_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCMXGXS5,      PHY_BCMXGXS5_OUI,      PHY_BCMXGXS5_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCMXGXS6,      PHY_BCMXGXS6_OUI,      PHY_BCMXGXS6_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM53314,      PHY_BCM53314_OUI,     PHY_BCM53314_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM53324,      PHY_BCM53324_OUI,     PHY_BCM53324_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM56160_GPHY, PHY_BCM56160GPHY_OUI, PHY_BCM56160GPHY_MODEL,
      PHY_REV_ALL},  
    { _phy_id_BCM53540,      PHY_BCM53540_OUI,     PHY_BCM53540_MODEL,
      PHY_REV_ALL},
    { _phy_id_BCM56275_GPHY, PHY_BCM56275GPHY_OUI, PHY_BCM56275GPHY_MODEL,
      PHY_REV_ALL},  
    { _phy_id_SERDES100FX,   PHY_SERDES100FX_OUI,   PHY_SERDES100FX_MODEL,
      PHY_REV_4 | PHY_REV_5},
    { _phy_id_SERDES65LP,    PHY_SERDES65LP_OUI,    PHY_SERDES65LP_MODEL,
      PHY_REV_ALL},
    { _phy_id_SERDESCOMBO,   PHY_SERDESCOMBO_OUI,   PHY_SERDESCOMBO_MODEL,
      PHY_REV_8 },
    { _phy_id_XGXS_16G,      PHY_XGXS_16G_OUI,      PHY_XGXS_16G_MODEL,
      PHY_REV_0 },
    { _phy_id_XGXS_TSC,      PHY_XGXS_TSC_OUI,      PHY_XGXS_TSC_MODEL,
      PHY_REV_0 },
    { _phy_id_XGXS_VIPER,    PHY_XGXS_VIPER_OUI,    PHY_XGXS_VIPER_MODEL,
      PHY_REV_9 },
#ifdef INCLUDE_PHY_82381
    { _phy_id_BCM82381,      PHY_BCM82381_OUI,      PHY_BCM82381_MODEL,
      PHY_REV_ALL},
#endif
#ifdef INCLUDE_PHY_82764
    { _phy_id_BCM82764,      PHY_BCM82764_OUI,      PHY_BCM82764_MODEL,
      PHY_REV_ALL},
#endif
#ifdef INCLUDE_PHY_82864
    { _phy_id_BCM82864,      PHY_BCM82864_OUI,      PHY_BCM82864_MODEL,
PHY_REV_ALL},
#endif


};

/*
 * Function:
 *      _phy_ident_type_get
 * Purpose:
 *      Check the PHY ID and return an enumerated value indicating
 *      the PHY.  This looks very redundant, but in the future, more
 *      complicated PHY detection may be necessary.  In addition, the
 *      enum value could be used as an index.
 * Parameters:
 *      phy_id0 - PHY ID register 0 (MII register 2)
 *      phy_id1 - PHY ID register 1 (MII register 3)
 */

static soc_known_phy_t
_phy_ident_type_get(uint16 phy_id0, uint16 phy_id1)
{
    int                 i;
    phy_id_map_t        *pm;
    uint32              oui;
    uint16              model, rev_map;

    oui       = PHY_OUI(phy_id0, phy_id1);
    model     = PHY_MODEL(phy_id0, phy_id1);
    rev_map   = 1 << PHY_REV(phy_id0, phy_id1);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META("phy_id0 = %04x phy_id1 %04x oui = %04x model = %04x rev_map = %04x\n"),
              phy_id0, phy_id1, oui, model, rev_map));
    for (i = 0; i < COUNTOF(phy_id_map); i++) {
        pm = &phy_id_map[i];
        if ((pm->oui == oui) && (pm->model == model)) {
            if (pm->rev_map & rev_map) {
                return pm->phy_num;
            }
        }
    }

    return _phy_id_unknown;
}

/*
 * Function:
 *      soc_phy_nocxn
 * Purpose:
 *      Return the no_cxn PHY driver
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      phyd_ptr - (OUT) Pointer to PHY driver.
 * Returns:
 *      SOC_E_XXX
 */

int
soc_phy_nocxn(int unit, phy_driver_t **phyd_ptr)
{
    *phyd_ptr = &phy_nocxn;

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_phy_info_get
 * Purpose:
 *      Accessor function to copy out PHY info structure
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      pi - (OUT) Pointer to output structure.
 * Returns:
 *      SOC_E_XXX
 */

int
soc_phy_info_get(int unit, soc_port_t port, soc_phy_info_t *pi)
{
    soc_phy_info_t *source;

    source = &SOC_PHY_INFO(unit, port);

    sal_memcpy(pi, source, sizeof(soc_phy_info_t));
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_phy_an_timeout_get
 * Purpose:
 *      Return autonegotiation timeout for a specific port
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 * Returns:
 *      Timeout in usec
 */

sal_usecs_t
soc_phy_an_timeout_get(int unit, soc_port_t port)
{
    return PHY_AN_TIMEOUT(unit, port);
}

/*
 * Function:
 *      soc_phy_addr_of_port
 * Purpose:
 *      Return PHY ID of the PHY attached to a specified port.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 * Returns:
 *      PHY ID
 */

uint16
soc_phy_addr_of_port(int unit, soc_port_t port)
{
    return PHY_ADDR(unit, port);
}

/*
 * Function:
 *      soc_phy_addr_int_of_port
 * Purpose:
 *      Return PHY ID of a intermediate PHY on specified port.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 * Returns:
 *      PHY ID
 * Notes:
 *      Only applies to chip ports that have an intermediate PHY.
 */

uint16
soc_phy_addr_int_of_port(int unit, soc_port_t port)
{
    return PHY_ADDR_INT(unit, port);
}

/*
 * Function:
 *      soc_phy_cfg_addr_get
 * Purpose:
 *      Get the configured PHY address for this port.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      flag - internal phy or external phy
 *      addr_ptr - hold the retrieved address
 * Returns:
 *      SOC_E_NONE 
 */

int
soc_phy_cfg_addr_get(int unit, soc_port_t port, int flags, uint16 *addr_ptr)
{
    if (flags & SOC_PHY_INTERNAL) {
        *addr_ptr = port_phy_addr[unit][port].int_addr;
    } else {
        *addr_ptr = soc_property_port_get(unit, port,
                                      spn_PORT_PHY_ADDR,
                                      port_phy_addr[unit][port].ext_addr);
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_phy_cfg_addr_set
 * Purpose:
 *      Configure the port with the given PHY address.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      flag - internal phy or external phy
 *      addr - the address to set
 * Returns:
 *      SOC_E_NONE
 */

int
soc_phy_cfg_addr_set(int unit, soc_port_t port, int flags, uint16 addr)
{
    if (flags & SOC_PHY_INTERNAL) {
        port_phy_addr[unit][port].int_addr = addr;
    } else {
        port_phy_addr[unit][port].ext_addr = addr;
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_phy_addr_to_port
 * Purpose:
 *      Return the port to which a given PHY ID corresponds.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 * Returns:
 *      Port number
 */

soc_port_t
soc_phy_addr_to_port(int unit, uint16 phy_id)
{
    return PHY_ADDR_TO_PORT(unit, phy_id);
}

/*
 * Function:
 *      soc_phy_id1reg_get
 * Purpose:
 *      Return the PHY ID1 field from the PHY on a specified port.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 * Returns:
 *      PHY ID
 */

uint16
soc_phy_id1reg_get(int unit, soc_port_t port)
{
    return PHY_ID1_REG(unit, port);
}

/*
 * Function:
 *      soc_phy_id1reg_get
 * Purpose:
 *      Return the PHY ID0 field from the PHY on a specified port.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 * Returns:
 *      PHY ID
 */

uint16
soc_phy_id0reg_get(int unit, soc_port_t port)
{
    return PHY_ID0_REG(unit, port);
}

/*
 * Function:
 *      soc_phy_is_c45_miim
 * Purpose:
 *      Return TRUE  if Phy uses Clause 45 MIIM, FALSE otherwise
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 * Returns:
 *      Return TRUE  if Phy uses Clause 45 MIIM, FALSE otherwise
 */

int
soc_phy_is_c45_miim(int unit, soc_port_t port)
{
    return PHY_CLAUSE45_MODE(unit, port);
}

/*
 * Function:
 *      soc_phy_name_get
 * Purpose:
 *      Return name of PHY driver corresponding to specified port.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 * Returns:
 *      Static pointer to string.
 */

char *
soc_phy_name_get(int unit, soc_port_t port)
{
    if (PHY_NAME(unit, port) != NULL) {
        return PHY_NAME(unit, port);
    } else {
        return "<Unnamed PHY>";
    }
}

#if (defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT))
#ifdef BROADCOM_DEBUG
/*
 * Function:
 *      soc_phy_dump
 * Purpose:
 *      Display the phy dirvers that are compiled into the driver.
 * Parameters:
 *      unit - StrataSwitch unit #.
 * Returns:
 *      none
 */

void soc_phy_dump(void)
{
    int idx1, idx2;

    for(idx1 = 0; idx1 < _phys_in_table; idx1 += 4) {
        if ( idx1 == 0 ) {
             LOG_CLI((BSL_META("PHYs: ")));
        } else {
             LOG_CLI((BSL_META("      ")));
        }
        for(idx2 = idx1; idx2 < idx1 + 4 && idx2 < _phys_in_table; idx2++) {
             LOG_CLI((BSL_META("\t%s%s"),
phy_table[idx2]->phy_name,
                      idx2 < _phys_in_table? "," : ""));
        }
         LOG_CLI((BSL_META("\n")));
    }

    return;
}
#endif /* BROADCOM_DEBUG */


int
soc_phy_list_get(char *phy_list[],int phy_list_size,int *phys_in_list)
{
    int       idx, base;

    if (phy_list == NULL) {
        return SOC_E_PARAM;
    }

    if (phys_in_list == NULL) {
        return SOC_E_PARAM;
    }

    base = *phys_in_list;

    /* Check invalid param */
    if ((base < 0) || (base > _MAX_PHYS)) {
        return SOC_E_PARAM;
    }

    /* Check invalid table  */
    if ((_phys_in_table < 0) || (_phys_in_table > _MAX_PHYS)) {
        return SOC_E_INTERNAL;
    }

    if ((phy_list_size + base) < _phys_in_table) {
        /*  If phy_list can be completely filled 
           i.e the phy_table has more elements starting from base
           than the size of phy_list */
        *phys_in_list = phy_list_size;
    } else {
        /* If phy_list cannot be completely filled 
           i.e the phy_table has less elements starting from base
           than the size of phy_list*/
        *phys_in_list = _phys_in_table - base;
    }

    /* At this point phys_in_list has the number of items(phys)
       that are going to be set in phy_list starting from the base
       in phy_table[] 
    */
    for (idx = 0; (idx < *phys_in_list) && (idx + base < _MAX_PHYS); idx++) {
        /* coverity[overrun-local : FALSE] */
        phy_list[idx] = phy_table[idx + base]->phy_name;
    }

    /* Returns the status to the calling function if
        there are still more entries in the list */
    if ((idx + base) < _phys_in_table)  {
        return SOC_E_FULL;
    }
    return SOC_E_NONE;
}

#endif /* (defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)) */
