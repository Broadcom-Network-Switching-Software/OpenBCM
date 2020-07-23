/*
 *
 * $Id: cint_field_port_class_qual_64_bits.c
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_field_port_class_qual_64_bits.c
 * Purpose: Installs a FG that qualifys on port class using 64 bits.
 */

bcm_field_entry_t port_class_qual_64_bits_ent;
bcm_field_entry_t port_class_qual_64_bits_ent_default;

int cint_field_port_class_qual_64_bits_main(int unit, bcm_field_group_t grp, int group_priority)
{
    int result = BCM_E_NONE;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    uint64 val;
    uint64 mask;
    uint64 val_zero;
    uint64 mask_zero;

    COMPILER_64_ZERO(val);
    COMPILER_64_SET(val, 0x200000, 0x30000);
    COMPILER_64_ZERO(mask);
    COMPILER_64_SET(mask, 0xFFFFFFFF, 0xFFFFFFFF);
    COMPILER_64_ZERO(val_zero);
    COMPILER_64_SET(val_zero, 0x0, 0x0);
    COMPILER_64_ZERO(mask_zero);
    COMPILER_64_SET(mask_zero, 0x0, 0x0);

    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInterfaceClassProcessingPort);

    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionDropPrecedence);

    result = bcm_field_group_create_id(unit, qset, group_priority, grp);
    if (BCM_E_NONE != result)
    {
        printf("error %d when in bcm_field_group_create_id\r\n", result);
        return result;
    }

    result = bcm_field_group_action_set(unit, grp, aset);
    if (BCM_E_NONE != result)
    {
        printf("error %d when in bcm_field_group_action_set\r\n", result);
        return result;
    }

    result = bcm_field_entry_create(unit, grp, &port_class_qual_64_bits_ent);
    if (BCM_E_NONE != result)
    {
        printf("error %d when in bcm_field_entry_create\r\n", result);
        return result;
    }

    result = bcm_field_qualify_InterfaceClassProcessingPort(unit, port_class_qual_64_bits_ent, val, mask);
    if (BCM_E_NONE != result)
    {
        printf("error %d when in bcm_field_qualify_IpType\r\n", result);
        return result;
    }

    result = bcm_field_action_add(unit, port_class_qual_64_bits_ent, bcmFieldActionDropPrecedence, 0x2, 0x3);
    if (BCM_E_NONE != result)
    {
        printf("error %d when in bcm_field_action_add\r\n", result);
        return result;
    }

    result = bcm_field_entry_prio_set(unit, port_class_qual_64_bits_ent, 1);
    if (BCM_E_NONE != result)
    {
        printf("error %d when in bcm_field_entry_prio_set\r\n", result);
        return result;
    }

    /* Create a catch all entry. */
    result = bcm_field_entry_create(unit, grp, &port_class_qual_64_bits_ent_default);
    if (BCM_E_NONE != result)
    {
        printf("error %d when in bcm_field_entry_create\r\n", result);
        return result;
    }

    result = bcm_field_qualify_InterfaceClassProcessingPort(unit, port_class_qual_64_bits_ent_default, val_zero, mask_zero);
    if (BCM_E_NONE != result)
    {
        printf("error %d when in bcm_field_qualify_IpType\r\n", result);
        return result;
    }

    result = bcm_field_action_add(unit, port_class_qual_64_bits_ent_default, bcmFieldActionDropPrecedence, 0x3, 0x3);
    if (BCM_E_NONE != result)
    {
        printf("error %d when in bcm_field_action_add\r\n", result);
        return result;
    }

    result = bcm_field_entry_prio_set(unit, port_class_qual_64_bits_ent_default, 0);
    if (BCM_E_NONE != result)
    {
        printf("error %d when in bcm_field_entry_prio_set\r\n", result);
        return result;
    }

    result = bcm_field_group_install(unit, grp);
    if (BCM_E_NONE != result)
    {
        printf("error %d when in bcm_field_group_install\r\n", result);
        return result;
    }

    printf("Created group %d\r\n", grp);

    return result;
}

int cint_field_port_class_qual_64_bits_destroy(int unit, bcm_field_group_t grp)
{
    int result = BCM_E_NONE;
    result = bcm_field_entry_destroy(unit, port_class_qual_64_bits_ent);
    if (BCM_E_NONE != result)
    {
        printf("error %d when in bcm_field_entry_destroy\r\n", result);
        return result;
    }
    result = bcm_field_entry_destroy(unit, port_class_qual_64_bits_ent_default);
    if (BCM_E_NONE != result)
    {
        printf("error %d when in bcm_field_entry_destroy\r\n", result);
        return result;
    }
    result = bcm_field_group_destroy(unit, grp);
    if (BCM_E_NONE != result)
    {
        printf("error %d when in bcm_field_group_destroy\r\n", result);
        return result;
    }
    return result;
}

