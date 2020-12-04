#include <phymod/phymod.h>
#include <phymod/phymod_cell.h>
#include <phymod/phymod_cell_dispatch.h>

#include <phymod/chip/null_cell.h>

#ifdef PHYMOD_NULL_SUPPORT

int null_phy_cell_speed_config_set(const phymod_phy_access_t* phy, const phymod_cell_speed_config_t* speed_config)
{

    return PHYMOD_E_NONE;
}

int null_phy_cell_speed_config_get(const phymod_phy_access_t* phy, phymod_cell_speed_config_t* speed_config)
{
    phymod_cell_speed_config_t_init(speed_config);
    return PHYMOD_E_NONE;
}

int null_phy_cell_autoneg_set(const phymod_phy_access_t* phy, const phymod_cell_autoneg_control_t* an)
{
    return PHYMOD_E_NONE;

}

int null_phy_cell_autoneg_get(const phymod_phy_access_t* phy, phymod_cell_autoneg_control_t* an, uint32_t* an_done)
{
    phymod_cell_autoneg_control_t_init(an);
    *an_done = 0;
    return PHYMOD_E_NONE;
}

int null_phy_cell_autoneg_status_get(const phymod_phy_access_t* phy, phymod_cell_autoneg_status_t* status)
{
    phymod_cell_autoneg_status_t_init(status);
    return PHYMOD_E_NONE;
}

int null_phy_cell_autoneg_advert_ability_set(const phymod_phy_access_t* phy, const phymod_cell_autoneg_advert_abilities_t* an_advert_abilities)
{
    return PHYMOD_E_NONE;
}

int null_phy_cell_autoneg_advert_ability_get(const phymod_phy_access_t* phy, phymod_cell_autoneg_advert_abilities_t* an_advert_abilities)
{
    phymod_cell_autoneg_advert_abilities_t_init(an_advert_abilities);
    return PHYMOD_E_NONE;
}

int null_phy_cell_autoneg_remote_advert_ability_get(const phymod_phy_access_t* phy, phymod_cell_autoneg_advert_abilities_t* an_advert_abilities)
{
    phymod_cell_autoneg_advert_abilities_t_init(an_advert_abilities);
    return PHYMOD_E_NONE;
}


int null_phy_cell_link_status_get(const phymod_phy_access_t* phy, uint32_t* link_status)
{
    *link_status = 1;
    return PHYMOD_E_NONE;
}

#endif
