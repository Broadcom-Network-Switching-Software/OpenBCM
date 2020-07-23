#
# \file ltid_gen.py
#
# Generate LT IDs for each type of device
#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
import sys
import subprocess
import os
import os.path
import tempfile
import filecmp
import shutil
import getopt
from os import walk
import yaml  # YAML parser
from collections import OrderedDict  # keep YAML parser output in order
from yaml.resolver import Resolver
import re
from filelock import Timeout, FileLock

ISSU_DB_DIR = '/bcmissu/dll/db'
LTID_PREFIX = 'ltids_'
LTID_SUFFIX = 'ltdb'
ENUM_PREFIX = 'enums_'
ENUM_SUFFIX = 'enums'
ISSU_DLL_DIR = '/bcmissu/dll'
COMP_ACTIONS_IN_FILE = 'component_actions.yml'
COMP_ACTION_OUT_FILE = 'comp_actions.c'
GENERIC_NAME = 'gen'
DEV_VARIANT_DELIM = '_s_'
DUMP_FILE_NAME = 'enums_all_'

# FLTG yaml file
FLTG_LTID_MAP = 'bcmltd_id.yml'
FLTG_INPUT_DIR = '/devdb'


def clean_python_default_boolean():
    for ch in "OoYyNn":
        if len(Resolver.yaml_implicit_resolvers[ch]) == 1:
            del Resolver.yaml_implicit_resolvers[ch]
        else:
            Resolver.yaml_implicit_resolvers[ch] = [x for x in
                    Resolver.yaml_implicit_resolvers[ch] if x[0] != 'tag:yaml.org,2002:bool']


# Standard yaml.load mixes the order of the dictionary so it doesn't
# match the order in the YAML file. This loader makes the trick.
def ordered_load(stream, Loader = yaml.Loader, object_pairs_hook = OrderedDict):

    class OrderedLoader(Loader):
        pass

    def construct_mapping(loader, node):
        loader.flatten_mapping(node)
        return object_pairs_hook(loader.construct_pairs(node))

    OrderedLoader.add_constructor(
        yaml.resolver.BaseResolver.DEFAULT_MAPPING_TAG, construct_mapping)
    rv = None
    try:
        rv = yaml.load(stream, OrderedLoader)
    except:
        e = sys.exc_info()[0]
        print('exception = ', e)
    return rv


def gen_comp_action_out(sdk_dir, pre_upgrade, post_upgrade):
    target_dir = sdk_dir + ISSU_DB_DIR + '/base'
    out_file = target_dir + '/' + COMP_ACTION_OUT_FILE
    # The output file is generated as temp file first.
    tmp_file = tempfile.mktemp('.gen')
    try:
        out_fh = open (tmp_file, 'w')
    except:
        print('Error: Failed to open temp file\n')
        return

    write_out_header(out_fh)

    for p in pre_upgrade:
        f = p[0] + '_' + p[1]
        spaces = ' ' * len(f)
        out_fh.write('extern int %s(int unit,\n' % f)
        out_fh.write('           %s shr_sysm_categories_t cat,\n' % spaces)
        out_fh.write('           %s bcmissu_phases_t phase);\n' % spaces)

    for p in post_upgrade:
        f = p[0] + '_' + p[1]
        spaces = ' ' * len(f)
        out_fh.write('extern int %s(int unit,\n' % f)
        out_fh.write('           %s shr_sysm_categories_t cat,\n' % spaces)
        out_fh.write('           %s bcmissu_phases_t phase);\n' % spaces)

    out_fh.write('\nstatic const bcmissu_comp_action_hlr_t'
                 ' issu_pre_upgrade_comp_hlr[] = {\n')
    for p in pre_upgrade:
        out_fh.write('   {{ "{0}", {1}_{2} }},\n'.format(p[0], p[0], p[1]))
    out_fh.write('   { NULL, NULL }\n')  # Terminate with NULL
    out_fh.write('};\n\n')

    out_fh.write('\nstatic const bcmissu_comp_action_hlr_t'
                 ' issu_post_upgrade_comp_hlr[] = {\n')
    for p in post_upgrade:
        out_fh.write('   {{ "{0}", {1}_{2} }},\n'.format(p[0], p[0], p[1]))
    out_fh.write('   { NULL, NULL }\n')  # Terminate with NULL
    out_fh.write('};\n')

    # Add two global functions to retrieve the pre/post component callback
    out_fh.write('\nconst bcmissu_comp_action_hlr_t'
                 ' *bcmissu_pre_upgrade_cb_get(void)\n')
    out_fh.write('{\n')
    out_fh.write('    return issu_pre_upgrade_comp_hlr;\n')
    out_fh.write('}\n')

    out_fh.write('\nconst bcmissu_comp_action_hlr_t'
                 ' *bcmissu_post_upgrade_cb_get(void)\n')
    out_fh.write('{\n')
    out_fh.write('    return issu_post_upgrade_comp_hlr;\n')
    out_fh.write('}\n')

    out_fh.close()

    # Make sure that the target directory exist
    if not os.path.isdir(target_dir):
        try:
            os.makedirs(target_dir)
        except OSError:
            print('Target directory exist\n')

    # Compare the temp file against the existing file. If changed move the
    # temp file onto the existing file. Otherwise, simply delete the temp file.
    if (not os.path.isfile(out_file)) or \
       (not filecmp.cmp(tmp_file, out_file)):
        shutil.move(tmp_file, out_file)
    else:
        os.remove(tmp_file)


def process_phases(sdk_dir, phases):
    pre_upgrade = []
    pre_upgrade_comp_name = []
    post_upgrade = []
    post_upgrade_comp_name = []
    for phase in phases:
        if phase == 'post_upgrade':
            components = phases[phase]
            for c_num in components:
                c_id = components[c_num]
                for c in c_id:
                    if c == 'component_name':
                        c_name = c_id[c]
                    elif c == 'handler':
                        hdlr = c_id[c]
                post_upgrade.append((c_name, hdlr))
        elif phase == 'pre_upgrade':
            components = phases[phase]
            for c_num in components:
                c_id = components[c_num]
                for c in c_id:
                    if c == 'component_name':
                        c_name = c_id[c]
                    elif c == 'handler':
                        hdlr = c_id[c]
                pre_upgrade.append((c_name, hdlr))

    gen_comp_action_out(sdk_dir, pre_upgrade, post_upgrade)


# Prepare C file containing the components actions to be taken
# in various phases.
# param: sdk_dir - The root directory of the SDK
# param: issu_dir - The directory where the YAML input file can be found.
def parse_comp_actions(sdk_dir, issu_dir):
    in_file = issu_dir + '/' + COMP_ACTIONS_IN_FILE
    try:
        fh = open(in_file, 'r')
    except:
        print('invalid source file ', in_file)
        return -1

    root = ordered_load(fh)
    fh.close()
    if root == None:
        print('Failed to parse yaml file %s - check file format' % in_file)
        return -1

    for phase in root:
        if phase == 'phase':
            process_phases(sdk_dir, root[phase])

    return 0


def write_final_table_dict(tables_dict, out_fh, variant):
    if out_fh == None:
        return

    # Now collect all the LT structures into a single array sorted by LT ID
    if (len(tables_dict)) > 0:
        out_fh.write('\nstatic const issu_ltid_dict_t issu_lt_dict_variant_%s[%d] = {\n'
                     % (variant, len(tables_dict)))
        first = True
        for tid in sorted (tables_dict.keys()):
            if first:
                first = False
            else:
                out_fh.write(',\n')
            out_fh.write('    { \"%s\", %d, %s_%s }'
                         % (tables_dict[tid], tid, tables_dict[tid].lower(), variant))
        out_fh.write('\n};\n')

        out_fh.write('\nstatic const issu_variant_info_t issu_lt_ids_variant_%s = {\n'
                     % variant)
        out_fh.write('    .variant_id = \"%s\",\n' % variant)
        out_fh.write('    .length = %d,\n' % len(tables_dict))
        out_fh.write('    .ltid_dict = issu_lt_dict_variant_%s\n' % variant)
        out_fh.write('};\n')
    else:
        out_fh.write('\nstatic const issu_variant_info_t issu_lt_ids_variant_%s = {\n'
                     % variant)
        out_fh.write('    .variant_id = \"%s\",\n' % variant)
        out_fh.write('    .length = 0,\n')
        out_fh.write('    .ltid_dict = NULL')
        out_fh.write('};\n')


# This function expected the following format per table:
# <table-name>
#    FIELD:
#       <field-name>
#          id: <field-id-value>
#          scalar: <val> (optional)
#          enum: <enum-type-name>  (optional)
#    id: <table-id-value>
# Generates the field and LT IDs data structures
# param [in]: variant - The variant associated with these LT definitions.
# param [in]: tables - The root of the TABLE tree.
# param [in]: ver - The current version string.
# param [in]: out_fh - The output file handle where all the parsed content
#       should be written.
def process_tbl_section(variant, tables, ver, out_fh, tables_dict):
    for t in tables:
        table = tables[t]
        if 'id' not in table:
            continue

        # For every LT define a variable that contains all the fields associated
        # with the table.
        out_fh.write('static const issu_ltid_f_dict_t %s_%s[] = {\n' % (t.lower(), variant))
        for tbl_info in table:
            if tbl_info == 'FIELD':
                fields = table[tbl_info]
                for field in fields:
                    for field_attr in fields[field]:
                        f_attrib = fields[field]
                        if field_attr == 'id':
                            out_fh.write('    { \"%s\", %d },\n'
                                         % (field, f_attrib[field_attr]))
            elif tbl_info == 'id':
                tables_dict[table[tbl_info]] = t
        out_fh.write('    { NULL, 0 }\n')
        out_fh.write('};\n')


def update_variant_map(device, variants, ver, fh):
    # define the variant array
    fh.write('\nstatic const issu_variant_info_t *variant_list[%d] = {\n'
                 % (len(variants) + 1))
    for variant in variants:
        fh.write('    &issu_lt_ids_variant_%s,\n' % variant)
    fh.write('    NULL\n')
    fh.write('};\n')

    fh.write('\nconst issu_device_info_t %s_%s = {\n' % (device, ver))
    fh.write('    .dev_name = \"%s\",\n' % device.upper())
    fh.write('    .variants = variant_list\n')
    fh.write('};\n')


def update_dev_map(dev_list, ver, out_fh):
    for dev in dev_list:
        out_fh.write('extern const issu_device_info_t %s_%s;\n'
                     % (dev, ver))

    # Define the devices array
    out_fh.write('\nstatic const issu_device_info_t *dev_list[%d] = {\n'
                 % (len(dev_list) + 1))
    for dev in dev_list:
        out_fh.write('    &%s_%s,\n' % (dev, ver))
    out_fh.write('    NULL\n')
    out_fh.write('};\n')

    # Now define the ltid root structure
    out_fh.write('\nstatic const issu_ltid_dict_entry_t issu_ltid_root_dir = {\n')
    out_fh.write('    .ver = \"%s\",\n' % ver)
    out_fh.write('    .gen_variant = &issu_lt_ids_variant_%s,\n' % GENERIC_NAME)
    out_fh.write('    .devices = dev_list\n')
    out_fh.write('};\n')

    # Generate a function to retrieve the dictionary
    out_fh.write('\nconst issu_ltid_dict_entry_t *bcmissu_ltid_dict_get()\n')
    out_fh.write('{\n')
    out_fh.write('    return &issu_ltid_root_dir;\n')
    out_fh.write('}\n')


def add_variant_enum_def_to_file(enum, enum_def, fh):
    fh.write('static const issu_enum_symbol_t %s[%d] = {\n' % (enum, len(enum_def) + 1))
    for elem in enum_def:
        fh.write('    {\"%s\", %s},\n' % (elem, enum_def[elem]))
    fh.write('    {NULL, 0}\n')
    fh.write('};\n')


# \brief Write an entire enum definition into C file.
# This function used for testing by generating the C structure for all the
# enum types definitions within the enum section of the input file.
# param [in] device Device name
# param [in] variant Variant name
# param [in] enum_section The ENUM section of the YAML file
# param [in] fh Output file handle
def dump_variant_section_to_c(device, variant, enum_section, fh):
    variant_enums = []

    for enum in enum_section:
        enum_variable = device + '_' + variant + '_' + enum
        add_variant_enum_def_to_file(enum_variable, enum_section[enum], fh)
        variant_enums.append(enum)

    fh.write('static const issu_enum_def_t enums_of_%s_%s[%d] = {\n' % \
             (device, variant, len(variant_enums) + 1))
    for enum in variant_enums:
        enum_variable = device + '_' + variant + '_' + enum
        fh.write('    {\"%s\", %s},\n' % (enum, enum_variable))
    fh.write('    {NULL, NULL}\n')
    fh.write('};\n\n')


def finalize_dump_c_file(enum_devices, version, fh):
    fh.write('static const issu_variants_enums_t variant_enums[] = {\n')
    for elem in enum_devices:
        # elem[0] is the variant elem[1] is the device
        if elem[0] == GENERIC_NAME:
            continue
        var_name = 'enums_of_' + elem[1] + '_' + elem[0]
        fh.write('    {\"%s\", \"%s\", %s},\n' % (elem[0], elem[1], var_name))
    fh.write('    {NULL, NULL, NULL}\n')
    fh.write('};\n\n')

    fh.write('static const issu_variants_enums_t gen_enums[] = {\n')
    for elem in enum_devices:
        # elem[0] is the variant elem[1] is the device
        if elem[0] != GENERIC_NAME:
            continue
        var_name = 'enums_of_' + elem[1] + '_' + elem[0]
        fh.write('    {\"%s\", \"%s\", %s},\n' % (elem[0], elem[1], var_name))
    fh.write('    {NULL, NULL, NULL}\n')
    fh.write('};\n\n')

    fh.write('static const issu_variants_enum_map_t enum_db = {\n')
    fh.write('   .gen_enums = gen_enums, .variants_enum = variant_enums\n')
    fh.write('};\n\n')

    fh.write('const issu_variants_enum_map_t *utissu_enum_dict_get_%s(void)\n' \
             % (version.replace('.', '_')))
    fh.write('{\n')
    fh.write('    return &enum_db;\n')
    fh.write('}\n')


def process_map_schema(dev_fh, output_dir, map_tree, dump_all_dir, dev,
                       ver, dfh, enum_devices, variant, tables_dict):
    for device in map_tree:
        dev_section = map_tree[device]
        for section in dev_section:
            if section == 'TABLE':
                process_tbl_section(variant,
                                    dev_section[section],
                                    ver, dev_fh, tables_dict)

            elif section == 'ENUM':
                if dump_all_dir != None:
                    enum_devices.append((variant, device))
                    dump_variant_section_to_c(device, variant,
                                              dev_section[section], dfh)

                out_yml_file = output_dir + ENUM_PREFIX + dev + \
                           DEV_VARIANT_DELIM + variant + '_v_' + ver + '.yml'
                try:
                    ofh = open(out_yml_file, 'w')
                except:
                    print('invalid destination file ', out_yml_file)
                    return -1

                ofh.write(yaml.dump(dev_section, encoding = 'utf-8'))
                ofh.close()


# param [in]: dev_fh - The ltid file specific for this device. The file contains
#       all the variants within this device.
# param [in] output_dir - The enum output files will be written into this
#       directory.
# param [in]: devices - The root of the object starts with the device section
#       (just below the VARIANT key word).
# param [in]: dump_all_dir - Directory string or None. If not None convert the
#       content of the enum section into a C file in the provided directory.
# param [in]: dev - The device name associated with the YAML object.
# param [in]: ver - The version string of this SDK where '.' is replaced by '_'
# param [in]: dfh - Dump file handle used only when the file invoked with -g.
#       This file handle will contain all the enums from all devices and
#       variants.
# param [out]: enum_devices - List all the devices that had enumerated
#       processed.
# param [out]: variant_list - List of all the variants that were processed
#       for the current device.
def process_variant_schema(dev_fh, output_dir, devices, dump_all_dir, dev,
                           ver, dfh, enum_devices, variant_list, tables_dict):
    for device in devices:
        variants = devices[device]
        for var in variants:
            variant = var.lower()
            if variant_list == None:
                print('Invalid data field. Top level contain variant')
                return -1
            variant_list.append(variant)
            variant_info = variants[var]
            for section in variant_info:
                if section == 'TABLE':
                    process_tbl_section(variant,
                                        variant_info[section],
                                        ver, dev_fh, tables_dict)
                elif section == 'MAP':
                    process_map_schema(dev_fh, output_dir, variant_info[section],
                                       dump_all_dir, dev, ver, dfh, enum_devices,
                                       variant, tables_dict)


# Generate LT IDs and field IDs from fltg output YAML file.
# param [in]: root - The list root of the YAML tree.
# param [in]: output_dir - The output directory to dump enum object section
#       as YAML file.
# param [in]: dev - The device name associated with the YAML object.
# param [in]: dump_all_dir - Directory string or None. If not None convert the
#       content of the enum section into a C file in the provided directory.
# param [in]: ver - The version string of this SDK where '.' is replaced by '_'
# param [in]: dev_fh - Device specific file handle use to store LTIDs for this
#       specific device and all its variants.
# param [in]: gen_out_fh - Generic ltid file handle to store the LTIDs for
#       tables that are device independent.
# param [in]: dfh - Dump file handle used only when the file invoked with -g.
#        This file handle will contain all the enums from all devices and
#        variants.
# param [out]: enum_devices - List all the devices that had enumerated
#        processed.
# param [out]: variant_list - List of all the variants that were processed
#        for the current device.
# The structure of the input file bcmltd_id.yml assumed to be as follows:
# BCMLTD
#     MAP
#        <device>
#            ENUM
#               <enum-name-type>
#                   <enum-str>: <val>
#            TABLE
#               <tbl-name>
#                   FIELD
#                   id (optional)
#     TABLE
#        <tbl-name>
#            FIELD
#            id
#     VARIANT
#         <device>
#             <variant-name>
#                 MAP
#                    <device>
#                        ENUM
#                           <enum-name-type>
#                               <enum-str>: <val>
#                        TABLE
#                           <tbl-name>
#                               FIELD
#                               id (optional)
#                 TABLE
#                    <tbl-name>
#                        FIELD
#                        id
# VERSION <ver>
def generate_ltid_file(root, output_dir, dev, dump_all_dir, ver, dev_fh,
                       gen_out_fh, dfh, enum_devices, variant_list,
                       gen_tables_dict):
    variant_tables_dict = {}
    top_level_gen_tables_dict = {}
    variant = ''

    for top_section in root:
        if top_section != 'BCMLTD_ID':
            return
        for section in root[top_section]:
            tmp = root[top_section]
            if section == 'TABLE':
                process_tbl_section(GENERIC_NAME, tmp[section],
                                    ver, gen_out_fh, top_level_gen_tables_dict)
                write_final_table_dict(top_level_gen_tables_dict, gen_out_fh, GENERIC_NAME)
            elif section == 'VARIANT':
                process_variant_schema(dev_fh, output_dir, tmp[section], dump_all_dir,
                                        dev, ver, dfh, enum_devices, variant_list,
                                        variant_tables_dict)
                variant = variant_list[-1]  # Get the last element that was added
                write_final_table_dict(variant_tables_dict, dev_fh, variant)
            elif section == 'MAP':
                process_map_schema(dev_fh, output_dir, tmp[section], dump_all_dir, dev,
                                   ver, dfh, enum_devices, GENERIC_NAME, gen_tables_dict)

    return 0


# Detect if the output file should be regenerated due to changes in its sources
# The function test for the LTID file and its sources as well as the
# corresponding enum file and its sources
# param [in]: out_f - The output LTID file name
# param [in]: dev_path - The path (under devdb) associated with the current
#       device.
#
# return: True if the sources are newer than the target. False otherwise.
def src_has_changes(out_f, dev_path, dev, ver, output_dir):
    if os.path.exists(out_f) and os.path.isfile(out_f):
        for (dirpath, dirnames, filenames) in walk(dev_path):
            for f in filenames:
                if not f.endswith('.yml'):
                    continue;
                if os.path.getmtime(dirpath + '/' + f) >= os.path.getmtime(out_f):
                    return True
        # Now check that the enum files exist
        for (dirpath, dirnames, filenames) in walk(output_dir):
            for f in filenames:
                if not f.startswith(ENUM_PREFIX + dev + DEV_VARIANT_DELIM):
                    continue
                if not f.endswith('_v_' + ver + '.yml'):
                    continue
                if os.path.getmtime(dirpath + '/' + f) >= os.path.getmtime(out_f):
                    return True
        # No change was detected above
        return False
    else:
        # Destination file doesn't exist
        return True


def process_ltid_file(output_dir, dev, dump_all_dir, ver, dev_fh,
                      fh_out, dfh, enum_devices, variants,
                      gen_tables_dict, dirpath, f):
    in_file = dirpath + '/' + f
    print('processing ', in_file)
    try:
        fh = open(in_file, 'r')
    except:
        print('Failed to open source file', in_file)
        return -1
    root = ordered_load(fh)
    fh.close()
    if root == None:
        print('Failed to parse yaml file %s - check file format' % in_file)
        return -1
    generate_ltid_file(root, output_dir, dev, dump_all_dir, ver,
                       dev_fh, fh_out, dfh, enum_devices, variants,
                       gen_tables_dict)


# param [in]: fltg_root_dir - typically $SDK/devdb
# param [in]: output_dir - The output directory within the ISSU DB. This
#       directory corresponds to the current version.
# param [in]: dump_all_dir - Boolean indication if the processing should also
#       be written into a debug directory, currently used by the ISSU UT.
# param [in]: version - the current version.
# param [in]: optimize - Boolean indication if the output files should be
#       regenerated regardless of their date and the date of the source file.
# param [in]: fh_out - output file handle for the generic ltids file.
def parse_ltid_yml_files(fltg_root_dir, output_dir, dump_all_dir,
                         version, optimize, fh_out):
    dev_list = []
    enum_devices = []
    gen_tables_dict = {}
    ver = version.replace('.', '_')

    if dump_all_dir != None:
        dump_file = dump_all_dir + '/' + DUMP_FILE_NAME + ver + '.c'
        try:
            dfh = open(dump_file, 'w')
        except:
            print('Failed to open file ', dump_file)
            return -1
        write_out_header(dfh)
    else:
        dfh = None

    in_file = fltg_root_dir + '/' + FLTG_LTID_MAP
    try:
        fh = open(in_file, 'r')
    except:
        print('Failed to open source file', in_file)
        return -1

    root = ordered_load(fh)
    fh.close()
    if root == None:
        print('Failed to parse yaml file %s - check file format' % in_file)
        return -1

    print('processing ', in_file)
    generate_ltid_file(root, output_dir, GENERIC_NAME, dump_all_dir, ver, None,
                       fh_out, dfh, enum_devices, None, None)

    path = fltg_root_dir + '/lt'
    for device in os.listdir(path):
        dev = device.lower()
        variants = []
        if filter_dev_db_out(dev):  # Ignore filtered out devices
            continue

        dev_path = path + '/' + dev
        if not os.path.isdir(dev_path):
            continue

        dev_list.append(dev)

        # Prepare the per device ltid file.
        out_f = output_dir + LTID_PREFIX + dev + '_' + ver + '.' + LTID_SUFFIX
        # Dependency Check that the input is newer than the output file.
        if (optimize and not src_has_changes(out_f, dev_path, dev, ver, output_dir)):
            continue

        try:
            dev_fh = open(out_f, "w")
        except:
            print('Failed to create file ', out_f)
            return -1
        write_out_header(dev_fh)
        print 'processing device ', dev

        for var in os.listdir(dev_path):
            var_path = dev_path + '/' + var
            if os.path.isdir(var_path):
                if filter_variant_db_out(var.lower()):
                    print 'filter out variant ', var
                    continue
                for (dirpath, dirnames, filenames) in walk(var_path):
                    for f in filenames:
                        if not f.endswith('.yml'):
                            continue;
                        process_ltid_file(output_dir, dev, dump_all_dir, ver, dev_fh,
                                          fh_out, dfh, enum_devices, variants,
                                          gen_tables_dict, dirpath, f)
            elif var.endswith('.yml'):
                process_ltid_file(output_dir, dev, dump_all_dir, ver, dev_fh,
                                  fh_out, dfh, enum_devices, variants,
                                  gen_tables_dict, dev_path, var)

        if (len(gen_tables_dict) > 0):
            write_final_table_dict(gen_tables_dict, dev_fh, GENERIC_NAME)

        # Write a map of the device and all its variants
        update_variant_map(dev, variants, ver, dev_fh)
        dev_fh.close()

    # Last update the device list - for testing purposes
    if dump_all_dir != None:
        finalize_dump_c_file(enum_devices, version, dfh)
        dfh.close()
    update_dev_map(dev_list, ver, fh_out)


def write_out_header(fh):
    fh.write('/***************************************************************************\n')
    fh.write(' *\n')
    fh.write(' * DO NOT EDIT THIS FILE!\n')
    fh.write(' * This file is auto-generated by the ISSU parser from YAML formatted file.\n')
    fh.write(' * Edits to this file will be lost when it is regenerated.\n')
    fh.write(' * Tool: bcmha/scripts/ltid_gen.py\n')
    fh.write(' *\n')
    fh.write(' * $Copyright:.$\n')
    fh.write(' *\n')
    fh.write(' ***************************************************************************/\n')
    fh.write('\n')
    fh.write("#include <bcmissu/issu_types.h>\n\n")


# Go through the version sub-directory directories of the ISSU DB directory
# If the sub directory is of the version that matches the input then rename all
# the files that start with the LTID prefix and have the LTID suffix to have
# 'c' suffix. On the other hand, if the version doesn't match the input versions
# then all the files with LTID prefix and 'c' suffix will be renamed to have
# LTID suffix (so they will not be compiled).
def rename_ltid_files(db_path, start_ver):
    dirs = []

    for (dirpath, dirnames, filenames) in walk(db_path):
        dirs.extend(dirnames)
        break

    for d in dirs:
        if d.startswith('ver_'):
            if d[4:] == start_ver:
                desired = True
            else:
                desired = False
            ver_path = db_path + '/' + d
        else:
            continue

        if desired:
            for f in os.listdir(ver_path):
                if f.startswith(LTID_PREFIX) and f.endswith(LTID_SUFFIX):
                    nfile = f.replace(LTID_SUFFIX, 'c')
                    os.rename(ver_path + '/' + f, ver_path + '/' + nfile)
        else:
            for f in os.listdir(ver_path):
                if f.startswith(LTID_PREFIX) and f.endswith('c'):
                    nfile = f.replace('c', LTID_SUFFIX)
                    os.rename(ver_path + '/' + f, ver_path + '/' + nfile)


def prepare_final_db(db_path, start_ver):
    rename_ltid_files(db_path, start_ver)
    return 0


######## The following functions related to enum final processing  ########
def add_enum_to_db(fh, dev, variant, enum, enum_obj):
    fh.write('static const issu_enum_symbol_t %s_%s_%s[%d] = {\n' %
             (dev, variant, enum, len(enum_obj) + 1))
    for elem in enum_obj:
        fh.write('    {\"%s\", %s},\n' % (elem, enum_obj[elem]))
    fh.write('    {NULL, 0}\n')
    fh.write('};\n')


def compare_enums(enum_a, enum_b):
    for e in enum_a:
        if (e not in enum_b) or (enum_a[e] != enum_b[e]):
            return 1
    return 0


def update_variant_db(fh, variant, device, enum_list):
    fh.write('static const issu_enum_def_t %s_%s[%d] = {\n' %
             (variant, device, len(enum_list) + 1))
    for e in enum_list:
        fh.write('    {\"%s\", %s_%s_%s},\n' % (e, device, variant, e))
    fh.write('    {NULL, NULL}\n')
    fh.write('};\n')


def finalize_variant_db(fh, var_list):
    # The variants list doesn't include the generic variant
    fh.write('\nstatic const issu_variants_enums_t variants[] = {\n')
    for var in var_list:
        if (var[0] != GENERIC_NAME):
            fh.write('    {\"%s\", \"%s\", %s_%s},\n' % (var[0], var[1], var[0], var[1]))
    fh.write('    {NULL, NULL, NULL}\n')
    fh.write('};\n')

    fh.write('\nstatic const issu_variants_enums_t %s[] = {\n' % GENERIC_NAME)
    for var in var_list:
        if (var[0] == GENERIC_NAME):
            fh.write('    {\"%s\", \"%s\", %s_%s},\n' % (var[0], var[1], var[0], var[1]))
    fh.write('    {NULL, NULL, NULL}\n')
    fh.write('};\n\n')

    fh.write('static const issu_variants_enum_map_t var_enum = {\n')
    fh.write('    .gen_enums = %s, .variants_enum = variants\n' % GENERIC_NAME)
    fh.write('};\n\n')

    fh.write('const issu_variants_enum_map_t *bcmissu_variants_enum_dict_get(const char *dev)\n')
    fh.write('{\n')
    fh.write('    return &var_enum;\n')
    fh.write('}')


# Compare two enum sections and save the enum definition that had changed.
# The object structure of src_data and dest_data is as follows:
#    <device>
#        ENUM
#           <enum-name-type>
#               <enum-str>: <val>
# or
#    ENUM
#        <enum-name-type>
#            <enum-str>: <val>
#
# param [in]: fh - Output file handle. This file contains all the enum
#       definitions of enum types that changed from the start to current
#       version. The definition of the enums is the start version definition.
# param [in]: device - The device associated with the enum file.
# param [in]: variant - the variant name associated with the enum DB.
# param [in]: src_data - The YAML tree containing the enum definition for the
#       current version. The format can be any of the above formats.
# param [in]: dest_data - The YAML tree containing the enum definition for the
#       start version. The format can be any of the above formats.
def update_delta_file(fh, device, variant, src_data, dest_data):
    enum_list = []
    src_enums = None
    dest_enums = None
    if 'ENUM' in src_data:
        src_enums = src_data['ENUM']
    if 'ENUM' in dest_data:
        dest_enums = dest_data['ENUM']

    if src_enums == None:
        for dev in src_data:
            dev_data = src_data[dev]
            src_enums = dev_data['ENUM']
            break;

    if dest_enums == None:
        for dev in dest_data:
            dev_data = dest_data[dev]
            dest_enums = dev_data['ENUM']
            break;

    for enum in src_enums:
        if enum not in dest_enums:
            continue
        if compare_enums(src_enums[enum], dest_enums[enum]) != 0:
            add_enum_to_db(fh, device, variant, enum, dest_enums[enum])
            enum_list.append(enum)

    # Define the device DB
    update_variant_db(fh, variant, device, enum_list)


# param [in]: fh - Output file handle. This file contains all the enum
#       definitions of enum types that changed from the start to current
#       version. The definition of the enums is the start version definition.
# param [in]: final_ver_dir - The enum DB directory of the current (latest)
#       version.
# param [in]: start_ver_dir - The enum DB directory of the start version.
# param [in]: f - Enum file name in the current (latest) version DB directory.
#       The file is of the format enums_<dev>_s_<variant>_<version>.yml
# param [in,out]: variants - list of tuples (variant, device).
def process_enum_delta(fh, final_ver_dir, start_ver_dir, f, variants):
    src_file = final_ver_dir + '/' + f
    dest_file = None
    device_list = []

    marker = f.find('_v_')
    if marker == -1:
        return -1
    marker2 = f.find(DEV_VARIANT_DELIM)
    if marker2 == -1 or marker2 + 3 > marker:
        print('invalid data file name ', f)
        return -1

    device = f[len(ENUM_PREFIX):marker2].upper()
    device_list.append(device)
    for src in os.listdir(start_ver_dir):
        if (not src.endswith('.yml')) or (not src.startswith(ENUM_PREFIX)):
            continue
        dest_marker = src.find('_v_')
        if dest_marker == -1:
            continue

        if len(src) > marker and f[:marker].upper() == src[:dest_marker].upper():
            dest_file = start_ver_dir + '/' + src
            break

    # Do nothing if the variant/device were not defined in the start version.
    if dest_file == None:
        return 0

    if (marker > len(ENUM_PREFIX)):
        variant = f[marker2 + 3:marker]
    else:
        return -1

    try:
        src_fh = open(src_file, 'r')
        dest_fh = open(dest_file, 'r')
    except:
        print('Failed to open yml files')
        return -1

    print('compare files %s to %s' % (f, src))
    src_data = yaml.load(src_fh)
    dest_data = yaml.load(dest_fh)
    src_fh.close()
    dest_fh.close()
    update_delta_file(fh, device, variant, src_data, dest_data)
    for device in device_list:
        variants.append((variant, device))
    return 0


def generate_enum_file_list(ver_path):
    f_list = []

    for f in os.listdir(ver_path):
        if f.startswith(ENUM_PREFIX) and f.endswith('.yml'):
            f_list.append(f)

    return f_list


def clear_old_issu_enum_files(target_dir, target_c_file):
    for f in os.listdir(target_dir):
        if f == target_c_file:
            continue
        if f.startswith('issu_enum') and f.endswith('.c') and \
           f.find('_to_') != -1:
            f_name = target_dir + '/' + f
            os.remove(f_name)


def prepare_enum_defs(db_path, start_ver, version, optimize):
    variants = []
    final_ver_dir = db_path + '/ver_' + version
    start_ver_dir = db_path + '/ver_' + start_ver
    target_c_file = 'issu_enum_' + \
                    start_ver.replace('.', '_') + '_to_' + \
                    version.replace('.', '_') + '.c'
    output_c_file = start_ver_dir + '/' + target_c_file

    if start_ver == version:
        return 0  # Nothing to do.

    clear_old_issu_enum_files(start_ver_dir, target_c_file)
    enum_files_list = generate_enum_file_list(final_ver_dir)
    if optimize and os.path.exists(output_c_file) and os.path.isfile(output_c_file):
        require_gen = False
        for f in enum_files_list:
            if os.path.getmtime(final_ver_dir + '/' + f) >= os.path.getmtime(output_c_file):
                require_gen = True
                break
        if not require_gen:
            return 0

    try:
        fh = open(output_c_file, 'w')
    except:
        print('invalid destination C file ', output_c_file)
        return -1

    write_out_header(fh)
    for f in enum_files_list:
        print('Processing enum file %s' % f)
        rv = process_enum_delta(fh, final_ver_dir, start_ver_dir, f, variants)
        if rv != 0:
            return rv

    if len(variants) > 0:
        finalize_variant_db(fh, variants)
    fh.close()
    return 0


def filter_dev_db_out(dev):
    global dev_included

    if len(dev_included) == 0:  # no filtering
        return False
    if dev in dev_included:
        return False
    return True


def filter_variant_db_out(variant):
    global variant_included

    if len(variant_included) == 0:  # no filtering
        return False
    if variant in variant_included:
        return False
    return True


def usage():
    print('python ltid_gen.py <options>\n')
    print('options: [-f -s <start_version> | -v <current_version>]')
    print('         -d <src-dir> -o <dest-dir> -g <out-dir> -n')
    print('         [-c <dev-id> [-t <variant-id>]*]*')
    print('Use the f parameter to generate the final version.')
    print('The -v parameter is required to determine the current version.')
    print('To generate the final version the -s parameter is mandatory.')
    print('The -d allows to point into a different directory for the input file.')
    print('By default the input directory is $SDK/devdb.')
    print('The -c restricts the DB generation of only the desired device.')
    print('After specifying a device the -t can be used to select specific')
    print('variant or a list of them (i.e. multiple -t can follow -c.')
    print('The -c flag can also appear multiple times for multiple devices.')
    print('The -o option allows to control the output root directory. By')
    print('default the output directory is $SDK/bcmissu/dll/db')
    print('The -g option simply generates a C file containing all the enums')
    print('definitions of the version. The generated file is enums_all.c.')
    print('The -g option is not applicable when the -f option being selected.')
    print('the -n option disables dependency checking and therefore will always')
    print('generate all the files.')


# The main function.
# Determine the version and invoke the lrd DB generator per device type.
def main(argv):
    final_gen = False
    version = None
    start_version = None
    fltg_root_dir = None
    output_root_dir = None
    dump_all_dir = None
    global dev_included
    global variant_included
    optimize = True

    dev_included = []
    variant_included = []
    try:
        sdk = os.environ['SDK']
    except:
        print('environment variable SDK required')
        sys.exit(-1)

    # Check if there are any input parameters
    try:
        opts, args = getopt.getopt(argv, "hfnv:s:d:o:g:c:t:", ["help"])
    except getopt.GetoptError:
        usage()
        sys.exit(-1)
    for opt, arg in opts:
        if opt in ("-h", "--help"):
            usage()
            sys.exit(0);
        elif opt == '-f':
            final_gen = True
        elif opt == '-v':
            version = arg
        elif opt == '-s':
            start_version = arg
        elif opt == '-d':
            fltg_root_dir = arg
        elif opt == '-o':
            output_root_dir = arg
        elif opt == '-g':
            dump_all_dir = arg
        elif opt == '-n':
            optimize = False
        elif opt == '-c':
            if arg.lower() not in dev_included:
                dev_included.append(arg.lower())
        elif opt == '-t':
            if arg.lower() not in variant_included:
                variant_included.append(arg.lower())
            else:
                print('Variant param must be preceed with device param -c')
                usage()
                sys.exit(-1)
        else:
            usage()
            sys.exit(-1)

    if version == None:
        print('Error: version must be defined')
        usage()
        sys.exit(-1)

    if output_root_dir == None:
        output_root_dir = sdk + ISSU_DB_DIR

    clean_python_default_boolean()
    lock_file = output_root_dir + '/issu_db.lock'

    if final_gen:
        if start_version == None:
            print('missing -s parameter')
            usage()
            sys.exit(-1)
        print('Generating final LTID DB')
        try:
            lock = FileLock(lock_file, 1800)  # wait up to 30 min
            lock.acquire()
        except Timeout:
            print('Failed to acquire ISSU DB lock')
            sys.exit(-1)
        rv = prepare_final_db(output_root_dir, start_version)
        if rv != 0:
            lock.release()
            sys.exit(rv)

        if start_version != version:
            print('Resolving ENUM differences')
            rv = prepare_enum_defs(output_root_dir, start_version, version, optimize)
            if rv != 0:
                lock.release()
                sys.exit(rv)

        # Parse component action YAML
        rv = parse_comp_actions(sdk, sdk + ISSU_DLL_DIR)
        lock.release()
        sys.exit(rv)

    if version == None:
        print('-v required')
        usage()
        sys.exit(-1)

    # Open the output file
    v = version.replace('.', '_')
    out_f = output_root_dir + '/ver_' + version + '/ltids_db_v' + v + '.ltdb'
    output_dir = output_root_dir + '/ver_' + version + '/'
    try:
        lock = FileLock(lock_file, 1800)  # wait up to 30 min
        lock.acquire()
    except Timeout:
        print('Failed to acquire ISSU DB lock')
        sys.exit(-1)
    try:
        fh = open(out_f, "w")
    except:
        print('Failed to create file ', out_f)
        lock.release()
        sys.exit(-1)

    write_out_header(fh)

    if fltg_root_dir == None:
        fltg_root_dir = sdk + FLTG_INPUT_DIR
    rv = parse_ltid_yml_files(fltg_root_dir, output_dir, dump_all_dir,
                              version, optimize, fh)

    fh.close()
    lock.release()
    sys.exit(rv)


if __name__ == "__main__":


    if sys.version_info[0] < 2 or \
       (sys.version_info[0] == 2 and sys.version_info[1] < 7):
        print('Error: Unsupported python version, min version 2.7.x')
    else:
        main(sys.argv[1:])

