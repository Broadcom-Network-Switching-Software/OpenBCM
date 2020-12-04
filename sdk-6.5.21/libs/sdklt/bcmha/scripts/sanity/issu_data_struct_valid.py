#
# \file issu_data_struct_valid.py
#
# This script contains a class that verifies register files signature mismatch
#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#

import sys
import os
from os import walk
import os.path
import yaml        # YAML parser
from collections import OrderedDict   # keep YAML parser output in order

ENUM_PREFIX = 'enums_'
TYPE_KEY = 'type'
FIELDS_KEY = 'fields'

dtype_dict = {'uint64_t': 8, 'uint32_t': 4, 'int': 4, 'char': 1, 'short': 2,
              'long': 4, 'bool': 1, 'unsigned': 4, 'unsigned char': 1,
              'unsigned short': 2, 'unsigned long': 4, 'long long': 8,
              'unsigned long long': 8, 'uint8_t': 1, 'uint16_t': 2,
              'int8_t': 1, 'int16_t': 2, 'int32_t': 4, 'int64_t': 8,
              'shr_ha_ptr_t': 4, 'shr_ha_lptr_t': 8, 'SHR_BITDCL': 4}

# Standard yaml.load mixes the order of the dictionary so it doesn't
# match the order in the YAML file. This loader makes the trick.
def ordered_load(stream, Loader=yaml.Loader, object_pairs_hook=OrderedDict):
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

class issu_data_validate:
    """Class to validate data structure definition changes between version"""

    def __init__(self, base_path):
        self.issu_db_base_dir = base_path

    def check_versions(self, start_ver, to_ver):
        validate_file_list = []
        self.from_dir = self.issu_db_base_dir + '/ver_' + start_ver
        self.to_dir = self.issu_db_base_dir + '/ver_' + to_ver

        for (dirpath, dirnames, filenames) in walk(self.to_dir):
            for f in filenames:
                if not f.endswith('.yml'):  # File needs to be a YAML file
                    continue
                # Ignore enumeration files.
                if f.startswith(ENUM_PREFIX) and f.endswith('_v_' + to_ver + '.yml'):
                    continue
                # Ignore the file if it is not exist in the start directory.
                if not os.path.isfile(self.from_dir + '/' + f):
                    continue
                validate_file_list.append(f)

        for f in validate_file_list:
            self.__handle_file(f)

    def __handle_file(self, f):
        fh_to = open(self.to_dir + '/' + f, 'r')
        fh_from = open(self.from_dir + '/' + f, 'r')
        to_yml = ordered_load(fh_to)
        from_yml = ordered_load(fh_from)
        fh_to.close()
        fh_from.close()
        if to_yml == None or from_yml == None:
            print('Invalid YAML file %s' % f)
            return
        self.__validate_files(f, to_yml, from_yml)

    def __validate_files(self, f, to_yml, from_yml):
        if not 'component' in to_yml or not 'component' in from_yml:
            print('Invalid content of YAML file %s' % f)
            return
        comp_sec = to_yml['component']
        for c in comp_sec:
            to_comp = comp_sec[c]
            break

        comp_sec = from_yml['component']
        for c in comp_sec:
            from_comp = comp_sec[c]
            break

        for elem in to_comp:
            if not elem in from_comp:
                continue  # New data type
            to_obj = to_comp[elem]
            from_obj = from_comp[elem]
            if not TYPE_KEY in to_obj or not TYPE_KEY in from_obj:
                print('Missing type field in %s' % to_obj)
                continue

            if to_obj[TYPE_KEY] != from_obj[TYPE_KEY]:
                print('Object %s has two different types' % to_obj)
                continue

            d_type = to_obj[TYPE_KEY]
            if not FIELDS_KEY in to_obj or not FIELDS_KEY in from_obj:
                print('Missing fields section from structure %s' % to_obj)
                continue
            if d_type == 'struct':
                self.__validate_struct(elem, to_obj[FIELDS_KEY], from_obj[FIELDS_KEY])
            elif d_type == 'enum':
                self.__validate_enum(elem, to_obj[FIELDS_KEY], from_obj[FIELDS_KEY])

    def __validate_struct(self, dtype_name, to_fld, from_fld):
        for fld in from_fld:
            if not fld in to_fld:
                continue
            self.__validate_fields(dtype_name, fld, to_fld[fld], from_fld[fld])

    # For enum validation we do not want to allow deletion of a value since
    # the ISSU will not know how to replace a value that is no longer
    # available.
    def __validate_enum(self, dtype_name, to_fld, from_fld):
        for elem in from_fld:
            if elem not in to_fld:
                print('Field %s was deleted from enum type %s' % (elem, dtype_name))

    def __validate_fields(self, dtype_name, fld, to_fld_attr, from_fld_attr):
        if to_fld_attr[TYPE_KEY] == from_fld_attr[TYPE_KEY]:
            return
        src_size = dtype_dict[from_fld_attr[TYPE_KEY]]
        dest_size = dtype_dict[to_fld_attr[TYPE_KEY]]
        if src_size > dest_size:
            print('Field %s of structure %s got smaller' % (fld, dtype_name))
        if 'table' in from_fld_attr:
            return
        if not 'size' in from_fld_attr:
            return
        src_size = from_fld_attr['size']
        dest_size = to_fld_attr['size']
        if src_size.isdigit() and dest_size.isdigit():
            if int(src_size) > int(dest_size):
                print('Reduction is array size for %s field %s' % (dtype_name, fld))