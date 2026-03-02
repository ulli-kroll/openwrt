# ----------------------------------------------------------------------------
# Copyright (c) 2016, 2017 by QUALCOMM Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
# ----------------------------------------------------------------------------

import fnmatch
import os
import re
import subprocess
import posixpath
from hashlib import sha256
import pickle

THIS_DIR = os.path.abspath(os.path.dirname(__file__))
ROOT_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__),'..','..','..','..'))
SSG_DIR = os.path.abspath(os.path.join(ROOT_DIR,'ssg'))
HEADER_ROOT_DIR = os.path.join(ROOT_DIR,'ssg','securemsm','platform')
SYSCALL_HANDLER_FILE = os.path.join(ROOT_DIR,'ssg','securemsm','trustzone','qsee','mink','app_mgr','src','syscall_handler.c')

def hash_file(path_to_file):
  with open(path_to_file, 'rt') as f:
    return sha256(re.sub(r'\s+','',f.read())).hexdigest()

class IDLAudit():
  #the idl audit is stored as posix style relative path <> hash in idl_audit.cfg
  #but it's converted to absolute path <> hash to get around windows/posix path differences
  # Config file is of the format
  # path/to/file hash
  # e.g.
  # ../../../../ssg/securemsm/trustzone/qsee/idl/public/my_idl.idl 1234556789abcdef
  def __init__(self, audit_cfg, regenerating_audit=False):
    self.audit_cfg_file = audit_cfg
    if not regenerating_audit:
      self.idl_to_sha_map = dict()
      with open(audit_cfg, "r") as f:
        for line in f:
          if len(line) == 1:
            # The audit config will have blank lines to try to help the perforce diff engine
            continue
          idl_path,hash = line.split()
          self.idl_to_sha_map[idl_path] = hash
     
    
  def dict_to_audit_file(self, audit_dict):
    with open(self.audit_cfg_file, "w") as f:
      for idl_path, hash in sorted(audit_dict.items()):
        f.write("{} {}\n\n".format(idl_path,hash))
        
  def find_all_idls(self):
    all_idls = []
    for root, dirs, files in os.walk(SSG_DIR):
      for file in files:
        if file.endswith('.idl'):
          all_idls.append(os.path.relpath(os.path.join(root,file), os.path.dirname(__file__)))
    all_idls = [s.replace('\\','/') for s in all_idls]
    return all_idls
    
  def audit(self):
    ret = 0
    existing_idls = self.find_all_idls()
    added_files = set(existing_idls) - set(self.idl_to_sha_map.keys())
    if len(added_files) > 0:
      ret = 1
      print "the following .idl files have been added"
      print "please add them to the idl audit"
      for f in sorted(added_files):
        print '\t',f
    
    deleted_files = set(self.idl_to_sha_map.keys()) - set(existing_idls)
    if len(deleted_files) > 0:
      ret = 1
      print "the following .idl files have been deleted"
      print "please remove them to the idl audit"
      for f in sorted(deleted_files):
        print '\t',f
    
    idl_mismatches = set()
    for idl_file in sorted(set(existing_idls).intersection(self.idl_to_sha_map.keys())):
      if hash_file(idl_file) != self.idl_to_sha_map[idl_file]:
        idl_mismatches.add(idl_file)
    if len(idl_mismatches) > 0:
      ret = 1
      print "the following idl files don't match the audit"
      print "please update the mink audit"
      for f in sorted(idl_mismatches):
        print '\t',f
    return ret
    
  def regenerate_audit(self):
    new_audit = dict()
    all_idls = self.find_all_idls()
    for idl in all_idls:
      new_audit[idl] = hash_file(idl)
    self.idl_to_sha_map = new_audit
    self.dict_to_audit_file(new_audit)

class HeaderAudit():
  #the header audit is stored as posix style relative path <> hash in header_audit.cfg
  #but it's converted to absolute path <> hash to get around windows/posix path differences
  def __init__(self, audit_cfg, regenerating_audit=False):
    self.header_to_sha_map = dict()
    self.audit_cfg_file = audit_cfg
    self.additional_files = ['../../../../ssg/securemsm/trustzone/qsee/include/tz_syscall_pub.h' ]
    self.exclude_dirs = ['tools','test','unittest','build']
    self.search_file = 'tzbsp_target.h'
    self.search_path = os.path.abspath(os.path.join(os.path.dirname(__file__),'..','..','..','securemsm','trustzone','qsee','arch'))
    if not regenerating_audit:
      self.header_to_sha_map = dict()
      for cfg_dir in os.listdir(HEADER_ROOT_DIR):
        if cfg_dir in self.exclude_dirs:
          continue
        with open(os.path.join(audit_cfg,cfg_dir+"_header_audit.cfg"), "r") as f:
          for line in f:
            if len(line) == 1:
              # The audit config will have blank lines to try to help the perforce diff engine
              continue
            header_path,hash = line.split()
            self.header_to_sha_map[header_path] = hash
      with open(os.path.join(audit_cfg,"extra_header_audit.cfg"), "r") as f:
        for line in f:
          if len(line) == 1:
            # The audit config will have blank lines to try to help the perforce diff engine
            continue
          header_path,hash = line.split()
          self.header_to_sha_map[header_path] = hash

  def write_out_audit_cfg(self, audit,cfg_dir):
    with open(os.path.join(self.audit_cfg_file,cfg_dir+"_header_audit.cfg"), "w") as f:
      for header_path, hash in sorted(audit.items()):
        f.write("{} {}\n\n".format(header_path,hash))
    f.close()

  def find_all_headers(self,search_dir):
    all_headers = []
    for root, dirs, files in os.walk(search_dir):
      for dir in self.exclude_dirs :
        if dir in dirs :
          dirs.remove(dir)
      for file in files:
        if file.endswith('.h'):
          all_headers.append(os.path.relpath(os.path.join(root,file), os.path.dirname(__file__)))
    all_headers = [s.replace('\\','/') for s in all_headers]
    return all_headers
  
  def find_extra_headers(self):
    all_headers = []
    for root, dirs, files in os.walk(self.search_path):
      for file in files:
        if file.endswith(self.search_file):
          all_headers.append(os.path.relpath(os.path.join(root,file), os.path.dirname(__file__)))
    for files in self.additional_files :
      all_headers.append(files)
    all_headers = [s.replace('\\','/') for s in all_headers]
    return all_headers
  
  def audit(self):
    ret = 0
    existing_headers = set()
    for cfg_dir in os.listdir(HEADER_ROOT_DIR):
      if cfg_dir in self.exclude_dirs:
        continue
      headers = self.find_all_headers(os.path.join(HEADER_ROOT_DIR,cfg_dir))
      existing_headers.update(headers)
    headers = self.find_extra_headers()
    existing_headers.update(headers)
    cfg_set = set();
    added_files = existing_headers - set(self.header_to_sha_map.keys())
    if len(added_files) > 0:
      ret = 1
      print "the following .h files have been added"
      print "please add them to the header audit"
      for f in sorted(added_files):
        print '\t',f
        if f.find('platform') != -1 :
          cfg_set.add(f.split('platform'+os.sep)[1].split(os.sep)[0])
        else :
          cfg_set.add('extra')
    
    deleted_files = set(self.header_to_sha_map.keys()) - existing_headers
    if len(deleted_files) > 0:
      ret = 1
      print "the following .header files have been deleted"
      print "please remove them to the header audit"
      for f in sorted(deleted_files):
        print '\t',f
        if f.find('platform') != -1 :
          cfg_set.add(f.split('platform'+os.sep)[1].split(os.sep)[0])
        else :
          cfg_set.add('extra')
    
    header_mismatches = set()
    for header_file in sorted(existing_headers.intersection(self.header_to_sha_map.keys())):
      if hash_file(header_file) != self.header_to_sha_map[header_file]:
        header_mismatches.add(header_file)
    if len(header_mismatches) > 0:
      ret = 1
      print "the following header files don't match the audit"
      print "please update the header audit"
      for f in sorted(header_mismatches):
        print '\t',f
        if f.find('platform') != -1 :
          cfg_set.add(f.split('platform'+os.sep)[1].split(os.sep)[0])
        else :
          cfg_set.add('extra')
    for cfg in cfg_set :
      print '\tCheckout CFGs:', os.path.join(self.audit_cfg_file,cfg+"_header_audit.cfg")
    return ret
    
  def regenerate_audit(self):
    for cfg_dir in os.listdir(HEADER_ROOT_DIR):
      if cfg_dir in self.exclude_dirs:
        continue
      headers = self.find_all_headers(os.path.join(HEADER_ROOT_DIR,cfg_dir))
      new_audit = dict()
      for header in headers:
        new_audit[header] = hash_file(header)
      self.header_to_sha_map.update(new_audit)
      self.write_out_audit_cfg(new_audit,cfg_dir)
      
    headers = self.find_extra_headers()
    new_audit = dict()
    for header in headers:
      new_audit[header] = hash_file(header)
    self.header_to_sha_map.update(new_audit)
    self.write_out_audit_cfg(new_audit,'extra')
    
class SyscallAudit():

  def __init__(self, audit_cfg, regenerating_audit=False):
    self.audit_cfg_file = audit_cfg
    if not regenerating_audit:
      with open(audit_cfg,'r') as f:
        self.syscall_hash = pickle.load(f)
      
  def write_out_audit_cfg(self, hash):
    with open(self.audit_cfg_file, 'w+') as f:
      pickle.dump(hash, f)
    
  def audit(self):
    hash = hash_file(SYSCALL_HANDLER_FILE)
    if hash != self.syscall_hash:
      return 1
    return 0
      
  def regenerate_audit(self):
    hash = hash_file(SYSCALL_HANDLER_FILE)
    self.write_out_audit_cfg(hash)

    
class CommonlibAudit():
  
  def __init__(self, cmnlib_so_path, readelf_path, regenerating_audit=False):
    self.cmnlib_so_path = cmnlib_so_path
    self.readelf_path = readelf_path
    if cmnlib_so_path.endswith(os.path.join('scorpion', 'libcmnlib.so')):
      self.audit_cfg_file = os.path.abspath(os.path.join(THIS_DIR,'cmnlib32_audit.cfg'))
    else:
      self.audit_cfg_file = os.path.abspath(os.path.join(THIS_DIR,'cmnlib64_audit.cfg'))
    if not regenerating_audit:
      self.symbols_set = set()
      with open(self.audit_cfg_file, 'r') as f:
        for line in f:
          if len(line) == 1:
            continue
          self.symbols_set.add(line.strip())
    
  def write_out_audit_cfg(self, audit):
    with open(self.audit_cfg_file, "w") as f:
      for thing in sorted(audit):
        f.write("{}\n\n".format(thing))
      
  def find_symbols(self):
    all_symbols = set()
    output = subprocess.check_output([self.readelf_path, '-s', self.cmnlib_so_path])
    for line in output.split('\n'):
      line = line.strip()
      # we only care about symbols in the .dynsym section
      if re.match("^Symbol table",line):
        if line.split()[2] != "'.dynsym'":
          return all_symbols
      # all lines we care about look like this
      # Num:    Value  Size Type    Bind   Vis      Ndx Name
      # 30: 0002177b    48 FUNC    GLOBAL DEFAULT    1 qsee_strlcpy
      if not re.match("^[0-9]+:", line):
        continue
      #Things where Ndx is UND are weird and not really commonlib exports but
      #still show up in dynsym, so ignore them
      if line.split()[6] == 'UND':
        continue
        
      #we only audit functions, not other stuff that the linker and/or compiler
      #may or may not have thrown in like NOTYPE
      if line.split()[3] != "FUNC":
        continue

      #symbol name is always the last word in the line
      # since we're only looking in .dynsym we can assume everything is global and default visibility
      line = line.split()[-1]
      #sometimes readelf appends '@' to the symbol name for whatever reason, so strip that off
      if line.endswith('@'):
        line = line[:-1]
      # with this logic we sometimes see lines that are length 0 (because of the @ stuff)
      # so if the symbol name is 0 characters long, just skip
      if len(line) > 0:
        all_symbols.add(line)
    return all_symbols
    
  def audit(self):
    ret = 0
    all_symbols = self.find_symbols()
    removed = self.symbols_set - all_symbols
    added = all_symbols - self.symbols_set
    if len(removed) > 0:
      print "Please do not modify entry points to commonlib"
      print "The following symbols have been removed from commonlib"
      print sorted(removed)
      ret = 1
    if len(added) > 0:
      print "Please do not modify entry points to commonlib"
      print "The following symbols have been added to commonlib"
      print sorted(added)
      ret = 1
    return ret

  def regenerate_audit(self):
    sym_set = self.find_symbols()
    self.write_out_audit_cfg(sym_set)

