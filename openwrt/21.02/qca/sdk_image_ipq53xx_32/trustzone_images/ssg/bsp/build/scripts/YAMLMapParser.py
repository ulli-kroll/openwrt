#!/pkg/qct/software/python/2.7.6/bin/python

import argparse
import os
import sys
import StringIO
import shutil

yaml_pkg_available = 0

try:
    import yaml
    yaml_pkg_available = 1
    try:
        from yaml import CLoader as Loader
    except ImportError:
        from yaml import Loader
except ImportError:
    pass

class YAMLFile(object):
    def __init__(self, yamlfile):
        self.yamlfile = yaml.load(yamlfile, Loader=Loader)
        self.grand_totals = {}
        self.objs = {}
        self.obj_totals = {}
        self.lib_objs = {}
        self.lib_totals = {}
        self.libs = {}
        self.generated = {}
        self.unused = {}
        self.object_map_code = {}
        self.unused_symbols = []

    def get_architecture(self):
        '''Return Header->Architecture.'''
        return self.yamlfile['Header']['Architecture']

    def get_emulation(self):
        '''Return Header->Emulation.'''
        return self.yamlfile['Header']['Emulation']

    def get_address_size(self):
        '''Return Header->AddressSize.'''
        return self.yamlfile['Header']['AddressSize']

    def is_code(self, item):
        '''Return True if item is code, otherwise return False.'''
        res = False
        if 'Type' in item and 'Permissions' in item:
            permission = item['Permissions']
            if ('SHT_PROGBITS' in item['Type'] and
                'SHF_ALLOC' in permission and
                'SHF_EXECINSTR' in permission and
                not 'SHT_REL' in item['Type']):
                res = True
        return res

    def is_ro_data(self, item):
        '''Return True if item is RO data, otherwise return False.'''
        res = False
        if 'Type' in item and 'Permissions' in item:
            permission = item['Permissions']
            if ('SHT_PROGBITS' in item['Type'] and
                'SHF_ALLOC' in permission and
                not 'SHF_WRITE' in permission and
                not 'SHF_EXECINSTR' in permission and
                not 'SHT_REL' in item['Type']):
                res = True
        return res

    def is_rw_data(self, item):
        '''Return True if item is RW data, otherwise return False.'''
        res = False
        if 'Type' in item and 'Permissions' in item:
            permission = item['Permissions']
            if ('SHT_PROGBITS' in item['Type'] and
                'SHF_ALLOC' in permission and
                'SHF_WRITE' in permission and
                not 'SHF_EXECINSTR' in permission and
                not 'SHT_REL' in item['Type']):
                res = True
        return res

    def is_zi_data(self, item):
        '''Return True if item is ZI data, otherwise return False.'''
        res = False
        if 'Type' in item and 'Permissions' in item:
            permission = item['Permissions']
            if ('SHT_NOBITS' in item['Type'] and
                'SHF_ALLOC' in permission and
                'SHF_WRITE' in permission and
                not 'SHF_EXECINSTR' in permission and
                not 'SHT_REL' in item['Type']):
                res = True
        return res

    def is_debug(self, item):
        '''Return True if item is debug, otherwise return False.'''
        res = False
        if 'Type' in item and 'Permissions' in item:
            permission = item['Permissions']
            if ('SHT_PROGBITS' in item['Type'] and
                not 'SHF_ALLOC' in permission and
                not 'SHF_WRITE' in permission and
                not 'SHF_EXECINSTR' in permission and
                not 'SHT_REL' in item['Type']):
                name = self.get_name(item)
                if name.startswith('.debug_'):
                    res = True
        return res

    def get_section_type(self, section):
        '''Return section type.'''
        if self.is_code(section):
            return 'code'
        elif self.is_ro_data(section):
            return 'ro_data'
        elif self.is_rw_data(section):
            return 'rw_data'
        elif self.is_zi_data(section):
            return 'zi_data'
        elif self.is_debug(section):
            return 'debug'
        else:
            return None

    def is_lib_member(self, item):
        '''Return True if item is a member of some library, otherwise return
        False.
        '''
        origin = self.get_origin(item)
        if '(' in origin and origin.endswith(')'):
            return True
        else:
            return False

    def get_name(self, item):
        '''Return item->Name.'''
        if 'Name' in item:
            return item['Name']
        else:
            return ''

    def get_size(self, item):
        '''Return item->Size.'''
        if 'Size' in item:
            return item['Size']
        else:
            return 0

    def get_origin(self, item):
        '''Return item->Origin.'''
        if 'Origin' in item:
            return item['Origin']
        else:
            return None

    def get_grand_totals(self):
        '''Return grand_totals.'''
        if self.grand_totals:
            return self.grand_totals
        obj_totals = self.get_object_totals();
        lib_totals = self.get_library_totals();
        self.grand_totals = dict(obj_totals)
        for k in self.grand_totals:
            self.grand_totals[k] += lib_totals[k]
        return self.grand_totals

    def update_object_sizes(self, section, sec_type):
        '''Update size of objs, lib_objs and generated'''
        origin = self.get_origin(section)
        if origin is None:
            if not self.generated:
                self.generated = {
                    'code': 0,
                    'data': 0,
                    'ro_data': 0,
                    'rw_data': 0,
                    'zi_data': 0,
                    'debug': 0
                    }
            self.generated[sec_type] += self.get_size(section)
        elif self.is_lib_member(section):
            if not origin in self.lib_objs:
                self.lib_objs[origin] = {
                    'code': 0,
                    'data': 0,
                    'ro_data': 0,
                    'rw_data': 0,
                    'zi_data': 0,
                    'debug': 0
                    }
            self.lib_objs[origin][sec_type] += self.get_size(section)
        else:
            if not origin in self.objs:
                self.objs[origin] = {
                    'code': 0,
                    'data': 0,
                    'ro_data': 0,
                    'rw_data': 0,
                    'zi_data': 0,
                    'debug': 0
                    }
            self.objs[origin][sec_type] += self.get_size(section)

    def get_object_sizes(self):
        '''Populate objs, lib_objs and generated, and return objs.'''
        if self.objs:
            return self.objs
        for osection in self.yamlfile['OutputSections']:
            if self.is_code(osection):
                if osection['Contents'] is None:
                    self.update_object_sizes(osection, 'code')
                    continue
                for isection in osection['Contents']:
                    if self.is_code(isection):
                        self.update_object_sizes(isection, 'code')
                    elif (self.is_zi_data(isection) or
                          self.is_ro_data(isection) or
                          self.is_rw_data(isection)):
                        self.update_object_sizes(isection, 'data')
            else:
                sec_type = self.get_section_type(osection)
                if not sec_type:
                    continue
                if osection['Contents'] is None:
                    self.update_object_sizes(osection, sec_type)
                    continue
                for isection in osection['Contents']:
                    self.update_object_sizes(isection, sec_type)
        return self.objs

    def is_contents(self, item):
        '''Return True if item is contents, otherwise return False.'''
        return 'Contents' in item

    def get_permissions(self, item):
        '''Return item->Permissions.'''
        return item.get('Permissions', '')

    def get_address(self, item):
        '''Return item->Address.'''
        return item.get('Address', 0)

    def get_offset(self, item):
        '''Return item->Offset.'''
        return item.get('Offset', 0)

    def get_attr(self, item):
        '''Return item->attr.'''
        permission = self.get_permissions(item)
        if self.is_rw_data(item):
            return 'RW'
        elif self.is_ro_data(item):
            return 'RO'
        elif self.is_code(item) and not 'SHF_WRITE' in permission:
            return 'RO'
        elif self.is_zi_data(item):
            return 'RW'
        else:
            return '0'

    def get_type(self, item):
        '''Return item->Type.'''
        if self.is_code(item):
            return 'CODE'
        else:
            return 'DATA'

    def get_object_map_code(self, item):
        '''entry address code .'''
        i = 0
        self.object_map_code = {}
        for osection in self.yamlfile['OutputSections']:
            section_name = osection.get('Name', "")
            if self.is_contents(osection) and item == section_name:
                if osection['Contents'] is not None:
                    base = self.get_address(osection)
                    for isection in osection['Contents']:
                        addr = base + self.get_offset(isection)
                        size = self.get_size(isection)
                        type = self.get_type(isection)
                        attr = self.get_attr(isection)
                        e_section_name = self.get_name(isection)
                        origin = self.get_origin(isection)
                        permission = self.get_permissions(isection)

                        if (origin is not None and 'SHF_ALLOC' in permission and
                            item == section_name):
                            if self.object_map_code is None:
                                self.object_map_code[i] = {
                                    'addr': 0,
                                    'size': 0,
                                    'type': 0,
                                    'attr': 0,
                                    's_name': 0,
                                    'origin': 0
                                }
                            else:
                                self.object_map_code[i] = {
                                    'addr': addr,
                                    'size': size,
                                    'type': type,
                                    'attr': attr,
                                    's_name': e_section_name,
                                    'origin': origin
                                }
                                i += 1
                else:
                    return None
        return self.object_map_code

    def get_object_map(self, mapoutput):
        '''entry address code .'''
        entry_point = self.yamlfile['EntryAddress']
        mapoutput.write("Image Entry point: {0:#010x}\n".format(entry_point))
        for osection in self.yamlfile['LoadRegions']:
            if 'Sections' in osection and osection['Sections'] is not None:
                mapoutput.write("\n\n")
                mapoutput.write("Load Regions %s   Base: %08x, Size: %08x\n\n" % (osection['Name'],
                    osection['VirtualAddress'], osection['FileSize'] ))
                for isection in osection['Sections']:
                    object_map = self.get_object_map_code(isection)
                    if object_map is not None:
                        mapoutput.write("\n Execution Region  %s \n\n" % (isection ))
                        mapoutput.write("     Base Addr       Size         Type   Attr      E Section Name                        Object\n")
                        for k, v in object_map.items():
                            mapoutput.write("    {0:#010x}   {1:#011x}      {2:6s}  {3:6s}  {4:35s}     {5:11s}\n".format(
                                v['addr'], v['size'], v['type'],v['attr'], v['s_name'],
                                os.path.basename(v['origin'])))

    def get_reference(self, mapoutput):
        '''section reference code .'''
        for osection in self.yamlfile['CrossReference']:
            if osection is not None:
                refer_to = osection.get('Symbol', "")
                refer_file = osection.get('ReferencedBy', "")
                mapoutput.write (" %s refers to %s\n" % (refer_file, refer_to))

    def get_generated_sizes(self):
        '''Return generated.'''
        if not self.generated:
            self.get_object_sizes()
        if not self.generated:
            self.generated = {
                'code': 0,
                'data': 0,
                'ro_data': 0,
                'rw_data': 0,
                'zi_data': 0,
                'debug': 0
                }
        return self.generated

    def get_library_member_sizes(self):
        '''Return lib_objs.'''
        if not self.lib_objs:
            self.get_object_sizes()
        return self.lib_objs

    def get_library_sizes(self):
        '''Return libs.'''
        if self.libs:
            return self.libs
        lib_objs = self.get_library_member_sizes()
        for k, v in lib_objs.items():
            libname = get_library_name(k)
            if not libname in self.libs:
                self.libs[libname] = {
                    'code': 0,
                    'data': 0,
                    'ro_data': 0,
                    'rw_data': 0,
                    'zi_data': 0,
                    'debug': 0
                    }
            for t in v:
                self.libs[libname][t] += v[t]
        return self.libs

    def get_object_totals(self):
        '''Return obj_totals.'''
        if self.obj_totals:
            return self.obj_totals
        objs = self.get_object_sizes()
        generated = self.get_generated_sizes()
        self.obj_totals = dict(generated)
        for k, v in objs.items():
            for t in v:
                self.obj_totals[t] += v[t]
        return self.obj_totals

    def get_library_totals(self):
        '''Return lib_totals.'''
        if self.lib_totals:
            return self.lib_totals
        self.lib_totals = {
            'code': 0,
            'data': 0,
            'ro_data': 0,
            'rw_data': 0,
            'zi_data': 0,
            'debug': 0
            }
        lib_objs = self.get_library_member_sizes()
        for k, v in lib_objs.items():
            for t in v:
                self.lib_totals[t] += v[t]
        return self.lib_totals

    def get_unused(self):
        '''Return unsued sections.'''
        if self.unused:
            return self.unused
        for section in self.yamlfile['DiscardedSections']:
            self.unused[self.get_name(section)] = {
                'origin': self.get_origin(section),
                'size': self.get_size(section)
                }
        return self.unused

    def get_unused_symbols(self):
        '''Return unused symbols.'''
        if self.unused_symbols:
            return self.unused_symbols
        for section in self.yamlfile['DiscardedSections']:
            if 'Symbols' in section:
                for sym in section['Symbols']:
                    self.unused_symbols.append(sym['Symbol'])
        return self.unused_symbols

    def get_unused_objects(self, mapoutput):
        '''Return unused objects.'''
        for section in self.yamlfile['InputInfo']:
            section_used = section.get('Used', "")
            if section_used == 'NotUsed':
                file_path = section.get('Path', "")
                mapoutput.write("  Unused objects: %s\n" % file_path)
                if 'Members' in section:
                    member = section.get('Members', "")
                    for sub_section in member:
                        section_used = sub_section.get('Used', "")
                        if section_used == 'NotUsed':
                            file_path = sub_section.get('Path', "")
                            mapoutput.write("  Unused objects:     %s\n" %file_path)
            else:
                if 'Members' in section:
                    unused = True
                    lib_file_path = section.get('Path', "")
                    member = section.get('Members', "")
                    for sub_section in member:
                        section_used = sub_section.get('Used', "")
                        if section_used == 'NotUsed':
                            if unused == True:
                                mapoutput.write("  Unused objects: %s\n" % lib_file_path)
                                unused = False
                            file_path = sub_section.get('Path', "")
                            mapoutput.write("  Unused objects:    %s\n" % file_path)

    def has_key_word(self, item):
        '''Return True if item is in file, otherwise return False.'''
        return item in self.yamlfile

def get_library_name(s):
    '''Return library name.'''
    return s[0:s.find("(")]


def get_library_member_name(s):
    '''Return library member name.'''
    return s[s.find("(")+1:s.find(")")]


def handle_map(yamlfile, mapoutput):
    '''Handle --map'''
    keyword1 = 'LoadRegions'
    keyword2 = 'OutputSections'
    keyword3 = 'EntryAddress'
    if (yamlfile.has_key_word(keyword1) and yamlfile.has_key_word(keyword2) and
        yamlfile.has_key_word(keyword3)):
        mapoutput.write('\n==============================================================================\n')
        mapoutput.write("Memory Map of the image\n")
        mapoutput.write('==============================================================================\n')
        yamlfile.get_object_map(mapoutput)
    else:
        mapoutput.write('==============================================================================\n')
        mapoutput.write ("No information about map load regions\n\n")
        mapoutput.write('==============================================================================\n')


def handle_xref(yamlfile, mapoutput):
    '''Handle --xref'''
    keyword = 'CrossReference'
    if not yamlfile.has_key_word(keyword):
        mapoutput.write("No information about cross reference\n\n")
        return
    mapoutput.write("\n Section Cross References\n\n")
    yamlfile.get_reference(mapoutput)


def handle_sizes(yamlfile, mapoutput):
    '''Handle topic 'sizes' of --info.'''
    keyword = 'OutputSections'
    if not yamlfile.has_key_word(keyword):
        mapoutput.write("No information about size\n\n")
        return
    mapoutput.write('\n==============================================================================\n')
    mapoutput.write("Image component sizes\n")
    mapoutput.write('==============================================================================\n')
    objs = yamlfile.get_object_sizes()
    mapoutput.write("     Code (inc. data)   RO Data    RW Data    ZI Data      Debug   Object Name\n")
    for k, v in sorted(objs.items()):
        mapoutput.write("{0:9d}{1:11d}{2:11d}{3:11d}{4:11d}{5:11d}   {6}\n".format(
                v['code']+v['data'], v['data'], v['ro_data'], v['rw_data'],
                v['zi_data'], v['debug'], os.path.basename(k)))
    obj_totals = yamlfile.get_object_totals()
    mapoutput.write("   ----------------------------------------------------------------------\n")
    mapoutput.write("{0:9d}{1:11d}{2:11d}{3:11d}{4:11d}{5:11d}   Object Totals\n".format(
            obj_totals['code']+obj_totals['data'], obj_totals['data'],
            obj_totals['ro_data'], obj_totals['rw_data'], obj_totals['zi_data'],
            obj_totals['debug']))
    generated = yamlfile.get_generated_sizes()
    mapoutput.write("{0:9d}{1:11d}{2:11d}{3:11d}{4:11d}{5:11d}   (incl. Generated)\n"
          .format(generated['code']+generated['data'], generated['data'],
                  generated['ro_data'], generated['rw_data'],
                  generated['zi_data'], generated['debug']))
    lib_objs = yamlfile.get_library_member_sizes()
    mapoutput.write("   ----------------------------------------------------------------------\n")
    mapoutput.write("     Code (inc. data)   RO Data    RW Data    ZI Data      Debug   Library Member Name\n")
    for k, v in lib_objs.items():
        mapoutput.write("{0:9d}{1:11d}{2:11d}{3:11d}{4:11d}{5:11d}   {6}\n".format(
                v['code']+v['data'], v['data'], v['ro_data'], v['rw_data'],
                v['zi_data'], v['debug'], get_library_member_name(k)))
    lib_totals = yamlfile.get_library_totals()
    mapoutput.write("   ----------------------------------------------------------------------\n")
    mapoutput.write("{0:9d}{1:11d}{2:11d}{3:11d}{4:11d}{5:11d}   Library Totals\n".format(
            lib_totals['code']+lib_totals['data'], lib_totals['data'],
            lib_totals['ro_data'], lib_totals['rw_data'], lib_totals['zi_data'],
            lib_totals['debug']))
    libs = yamlfile.get_library_sizes()
    mapoutput.write("   ----------------------------------------------------------------------\n")
    mapoutput.write("     Code (inc. data)   RO Data    RW Data    ZI Data      Debug   Library Name\n")
    for k, v in libs.items():
        mapoutput.write("{0:9d}{1:11d}{2:11d}{3:11d}{4:11d}{5:11d}   {6}\n".format(
                v['code']+v['data'], v['data'], v['ro_data'], v['rw_data'],
                v['zi_data'], v['debug'], os.path.basename(k)))
    mapoutput.write("   ----------------------------------------------------------------------\n")
    mapoutput.write("{0:9d}{1:11d}{2:11d}{3:11d}{4:11d}{5:11d}   Library Totals\n".format(
            lib_totals['code']+lib_totals['data'], lib_totals['data'],
            lib_totals['ro_data'], lib_totals['rw_data'], lib_totals['zi_data'],
            lib_totals['debug']))
    mapoutput.write("   ----------------------------------------------------------------------\n")
    grand_totals = yamlfile.get_grand_totals()
    mapoutput.write("=============================================================================\n")
    mapoutput.write("     Code (inc. data)   RO Data    RW Data    ZI Data      Debug\n")
    mapoutput.write("{0:9d}{1:11d}{2:11d}{3:11d}{4:11d}{5:11d}   Grand Totals\n".format(
            grand_totals['code']+grand_totals['data'], grand_totals['data'],
            grand_totals['ro_data'], grand_totals['rw_data'],
            grand_totals['zi_data'], grand_totals['debug']))
    mapoutput.write("=============================================================================\n")
    total_ro_size = (grand_totals['code'] + grand_totals['data']
                     + grand_totals['ro_data'])
    mapoutput.write("   Total RO  Size (Code + RO Data)          {0:11d} ({1:7.2f}kB)\n"
          .format(total_ro_size,total_ro_size/1024.0))
    total_rw_size = grand_totals['rw_data'] + grand_totals['zi_data']
    mapoutput.write("   Total RW  Size (RW Data + ZI Data)       {0:11d} ({1:7.2f}kB)\n"
          .format(total_rw_size,total_rw_size/1024.0))
    total_rom_size = (grand_totals['code'] + grand_totals['data']
                      + grand_totals['ro_data'] + grand_totals['rw_data'])
    mapoutput.write("   Total ROM Size (Code + RO Data + RW Data){0:11d} ({1:7.2f}kB)\n"
          .format(total_rom_size,total_rom_size/1024.0))
    mapoutput.write("=============================================================================\n")


def handle_summarysizes(yamlfile, mapoutput):
    '''Handle topic 'summarysizes' of --info.'''
    keyword = 'OutputSections'
    if not yamlfile.has_key_word(keyword):
        mapoutput.write ("No information about summary sizes\n")
        return
    grand_totals = yamlfile.get_grand_totals()
    mapoutput.write("Program Size: Code={0:d} RO-data={1:d} RW-data={2:d} ZI-data={3:d}\n"
          .format(grand_totals['code']+grand_totals['data'],
                  grand_totals['ro_data'], grand_totals['rw_data'],
                  grand_totals['zi_data']))


def handle_totals(yamlfile, mapoutput):
    '''Handle topic 'totals' of --info.'''
    keyword = 'OutputSections'
    if not yamlfile.has_key_word(keyword):
        mapoutput.write("No information about totals\n\n")
        return
    mapoutput.write("Image component sizes\n")
    obj_totals = yamlfile.get_object_totals()
    mapoutput.write("     Code (inc. data)   RO Data    RW Data    ZI Data      Debug\n")
    mapoutput.write("{0:9d}{1:11d}{2:11d}{3:11d}{4:11d}{5:11d}   Object Totals\n".format(
            obj_totals['code']+obj_totals['data'], obj_totals['data'],
            obj_totals['ro_data'], obj_totals['rw_data'], obj_totals['zi_data'],
            obj_totals['debug']))
    generated = yamlfile.get_generated_sizes()
    mapoutput.write("{0:9d}{1:11d}{2:11d}{3:11d}{4:11d}{5:11d}   (incl. Generated)\n"
          .format(generated['code']+generated['data'], generated['data'],
                  generated['ro_data'], generated['rw_data'],
                  generated['zi_data'], generated['debug']))
    lib_totals = yamlfile.get_library_totals()
    mapoutput.write("{0:9d}{1:11d}{2:11d}{3:11d}{4:11d}{5:11d}   Library Totals\n".format(
            lib_totals['code']+lib_totals['data'], lib_totals['data'],
            lib_totals['ro_data'], lib_totals['rw_data'], lib_totals['zi_data'],
            lib_totals['debug']))
    libs = yamlfile.get_library_sizes()
    grand_totals = yamlfile.get_grand_totals()
    mapoutput.write("=============================================================================\n")
    mapoutput.write("     Code (inc. data)   RO Data    RW Data    ZI Data      Debug\n")
    mapoutput.write("{0:9d}{1:11d}{2:11d}{3:11d}{4:11d}{5:11d}   Grand Totals\n".format(
            grand_totals['code']+grand_totals['data'], grand_totals['data'],
            grand_totals['ro_data'], grand_totals['rw_data'],
            grand_totals['zi_data'], grand_totals['debug']))
    mapoutput.write("=============================================================================\n")
    total_ro_size = (grand_totals['code'] + grand_totals['data']
                     + grand_totals['ro_data'])
    mapoutput.write("   Total RO  Size (Code + RO Data)          {0:11d} ({1:7.2f}kB)\n"
          .format(total_ro_size,total_ro_size/1024.0))
    total_rw_size = grand_totals['rw_data'] + grand_totals['zi_data']
    mapoutput.write("   Total RW  Size (RW Data + ZI Data)       {0:11d} ({1:7.2f}kB)\n"
          .format(total_rw_size,total_rw_size/1024.0))
    total_rom_size = (grand_totals['code'] + grand_totals['data']
                      + grand_totals['ro_data'] + grand_totals['rw_data'])
    mapoutput.write("   Total ROM Size (Code + RO Data + RW Data){0:11d} ({1:7.2f}kB)\n"
          .format(total_rom_size,total_rom_size/1024.0))
    mapoutput.write("=============================================================================\n")


def handle_architecture(yamlfile, mapoutput):
    '''Handle topic 'architecture' of --info.'''
    mapoutput.write("Architecture: " + yamlfile.get_architecture()+ '\n')
    mapoutput.write("Emulation: " + yamlfile.get_emulation()+ '\n')
    mapoutput.write("AddressSize: " + yamlfile.get_address_size()+ '\n')

def get_symbol_type(sym, yamlfile):
    type = sym['Type']
    if 'STT_SECT' in type:
        return 'Section'
    if 'STT_FUNC' in type:
        if yamlfile.get_architecture() == 'aarch64':
            return  'AT64 Code'
        if sym['Region']:
            return sym['Region']
        return 'Code'
    if 'STT_OBJECT' in type:
        return 'Number'
    return sym['Region']

def get_symbols(yamlfile):
    symbols = []
    region = ''
    for osec in yamlfile.yamlfile['OutputSections']:
        if not osec.has_key('Contents') or not osec['Contents']:
            continue
        for isec in osec['Contents']:
            if not isec.has_key('Symbols'):
                continue
            for sym in isec['Symbols']:
                name = sym['Symbol']
                if name.startswith('$d.'):
                    region = 'Data'
                    continue
                if yamlfile.get_architecture() == 'aarch64':
                    if name.startswith('$x.'):
                        region = 'AT64 Code'
                        continue
                else:
                    if name.startswith('$t.'):
                        region = 'Thumb Code'
                        continue
                    if name.startswith('$a.'):
                        region = 'ARM Code'
                        continue
                sym['Origin'] = os.path.basename(isec['Origin']) + '(' + isec['Name'] + ')'
                sym['Region'] = region
                symbols.append(sym)
    return symbols

def handle_symbols(yamlfile, mapoutput):
    '''Handle Local and Global symbols'''
    title = '    Symbol Name                              Value     Ov Type        Size  Object(Section)'
    addr_field = '8'
    if yamlfile.get_address_size() == '64bit':
        addr_field = '16';
    title_fmt = '    {:<40s} {:<' + addr_field + 's}   {:<10s} {:5s} {:s}'
    title = title_fmt.format('Symbol Name', 'Value', 'Ov Type', 'Size', 'Object(Section)')
    fmt = '    {:40s} {:#0' + addr_field + 'x}   {:10s}{:#5d}   {:s}'

    mapoutput.write('==============================================================================\n')
    mapoutput.write('Image Symbol Table' + '\n')
    mapoutput.write('==============================================================================\n')
    mapoutput.write('    Local Symbols' + '\n')
    mapoutput.write('------------------------------------------------------------------------------\n')
    mapoutput.write(title + '\n')
    symbols = get_symbols(yamlfile)
    for sym in symbols:
        sym_type = get_symbol_type(sym, yamlfile)
        if sym['Scope'] and sym_type != 'Section' : # local
          mapoutput.write(fmt.format(sym['Symbol'], sym['Value'], sym_type, sym['Size'], sym['Origin']) + '\n')
    mapoutput.write('------------------------------------------------------------------------------\n')
    mapoutput.write('    Global Symbols' + '\n')
    mapoutput.write('------------------------------------------------------------------------------\n')
    mapoutput.write(title + '\n')
    for sym in symbols:
        sym_type = get_symbol_type(sym, yamlfile)
        if sym['Scope'] == 0 and sym_type != 'Section': #global
          mapoutput.write(fmt.format(sym['Symbol'], sym['Value'], sym_type, sym['Size'], sym['Origin']) +'\n')
    mapoutput.write('------------------------------------------------------------------------------\n')

def handle_unused(yamlfile, mapoutput):
    '''Handle topic 'unused' of --info.'''
    keyword = 'DiscardedSections'
    if not yamlfile.has_key_word(keyword):
        mapoutput.write('No information about unused\n')
        return
    mapoutput.write("Removing Unused input sections from the image.\n")
    unused = yamlfile.get_unused()
    total_size = 0
    for k,v in unused.items():
        total_size += v['size']
        mapoutput.write("  Removing {0}({1}), ({2:d} bytes).\n".format(
                os.path.basename(v['origin']), k, v['size']))
    mapoutput.write("{0:d} unused section(s) (total {1:d} bytes) removed from the image.\n"
          .format(len(unused), total_size))


def handle_unusedsymbols(yamlfile, mapoutput):
    '''Handle topic 'unusedsymbols' of --info.'''
    keyword = 'DiscardedSections'
    if not yamlfile.has_key_word(keyword):
        mapoutput.write ('No information about unusedsymbols\n')
        return
    mapoutput.write("Removing Unused input sections from the image.\n")
    unused_symbols = yamlfile.get_unused_symbols()
    for sym in unused_symbols:
        mapoutput.write("       Removing symbol {0}.\n".format(sym))
    mapoutput.write("{0:d} unused symbol(s) removed from the image.\n".format(
            len(unused_symbols)))

def handle_unusedobjects(yamlfile, mapoutput):
    '''Handle topic 'unusedobjects' of --info.'''
    keyword = 'InputInfo'
    if not yamlfile.has_key_word(keyword):
        mapoutput.write('No information about unusedobjects\n')
        return
    mapoutput.write("Unused objects from the inputs.\n")
    yamlfile.get_unused_objects(mapoutput)

def handle_info(yaml_file, mapoutput, info):
    '''Handle --info=topic[,topic,...].'''
    usupported_topic = ""
    info_list = info.split(',')
    for topic in info_list:
        if topic == 'sizes':
            handle_sizes(yaml_file, mapoutput)
        elif topic == 'summarysizes':
            handle_summarysizes(yaml_file, mapoutput)
        elif topic == 'totals':
            handle_totals(yaml_file, mapoutput)
        elif topic == 'architecture':
            handle_architecture(yaml_file, mapoutput)
        elif topic == 'unused':
            handle_unused(yaml_file, mapoutput)
        elif topic == 'unusedsymbols':
            handle_unusedsymbols(yaml_file, mapoutput)
        elif topic == 'unusedobjects':
            handle_unusedobjects(yaml_file, mapoutput)
        else:
            usupported_topic += topic
            mapoutput.write('Unsupported topic: ' + topic + '\n')


def yaml_map_parser(target, source, env):
    global yaml_pkg_available

    if yaml_pkg_available == 0:
        print "!! NOTICE/ERROR: yaml pkg not available @python. For mon/qsee image .yaml is intermediate map file!."
        print "!! NOTICE/ERROR: Please install yaml pkg to get more readable .map file for mon & qsee!."
        print "!! NOTICE/ERROR: More details at go/ssgtz10 PW wiki!."
        return None

    source_str = os.path.abspath(str(source[0]))
    target_str = os.path.abspath(str(target[0]))

    mapoutput = StringIO.StringIO()

    with open(source_str, 'r') as f:
        yamlfile = YAMLFile(f)

    handle_symbols(yamlfile, mapoutput)

    handle_map(yamlfile, mapoutput)

    handle_info(yamlfile, mapoutput, 'sizes')

    with open(target_str, 'w+') as fd:
        mapoutput.seek (0)
        shutil.copyfileobj (mapoutput, fd)

    mapoutput.close()

    return None

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
   return env.Detect('yaml_map_parser')

def generate(env):
   yaml_map_generate(env)

def yaml_map_generate(env):
  bld = env.Builder(action = env.GetBuilderAction(yaml_map_parser),
                    suffix = '.elf')
  env.Append(BUILDERS = {'YAMLMapParser' : bld})
