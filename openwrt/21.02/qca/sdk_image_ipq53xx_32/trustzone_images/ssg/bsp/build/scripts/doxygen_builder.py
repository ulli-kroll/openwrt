# Doxygen builder
import SCons.Action
import io
import os
import re
import stat
import sys
from   distutils.file_util import copy_file
from   distutils.file_util import move_file

#------------------------------------------------------------------------------
# Some constants
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Hooks for Scons
#------------------------------------------------------------------------------
def exists(env):
  return env.Detect('doxygen_builder')

def generate(env):
  #-------------------------------------------------------------------------------
  # PdfDocumentBuilder Builder
  #
  pdfdoc_act = SCons.Action.Action(pdf_doc_builder,'=== Generating $TARGET.name')
  pdfdoc_bld = env.Builder(action = pdfdoc_act)
  env.Append(BUILDERS = {'PdfDocumentBuilder' : pdfdoc_bld})

  #-------------------------------------------------------------------------------
  # AddDoxySource Builder
  #
  #   Takes in source files and adds them to a list for later use in the
  #   generation of the documentation
  #
  doxyfile_act = SCons.Action.Action(doxyfile_builder,'=== Generating $TARGET.name')
  doxyfile_bld = env.Builder(action = doxyfile_act)
  env.Append(BUILDERS = {'AddDoxySource' : doxyfile_bld})

  #-------------------------------------------------------------------------------
  # Method
  env.AddMethod(doxyfile_bld,"AddDoxySource")

#-------------------------------------------------------------------------------
# def builder for DoxygenBuilder
#
def doxyfile_builder(target, source, env):

  # target is currently unused as this points to the location in the build tree where the 'compiled'
  # source file would exist

  # Make sure we have a list
  if type(source) is not list:
    source=[source]

  # Some path information
  build_root   = os.path.expanduser(env.get('BUILD_ROOT'))
  srcdir       = build_root   + str(env.get('src_dir'))

  if not os.path.exists(str(srcdir)):
    os.mkdir(str(srcdir))

  # To make the execution of the doxygen tool easier, we place all of
  # the files we want to create documentation for in to a temporary src
  # folder. Doxygen will then be executed using the files in the
  # temporary src folder as input
  for file in source:
    copy_file(str(file), str(srcdir),  preserve_mode = 0)

  return None

#===============================================================================
# Document builder rules
#  - This is the main entry point.
#
# Using KeyWord Arguments to maintain flexibility
#
#===============================================================================
def pdf_doc_builder(env, target, source, **kwargs):

  # Make sure we have a list
  if type(source) is not list:
    source=[source]

  build_root   = os.path.expanduser(env.get('BUILD_ROOT'))
  docpath_root = build_root   + str(env.get('doxy_dir'))
  doxyfile     = docpath_root + str(env.get('doxyfilename'))
  tmpl_root    = build_root   + str(env.get('template_dir'))
  tmplfile     = tmpl_root    + str(env.get('doxyfilename'))
  #idl_src     = build_root   + str(env.get('idl_dir')) + "/"
  latexdir     = build_root   + str(env.get('latex_dir'))
  auxdir       = latexdir     + '/auxfiles'
  htmldir      = docpath_root + '/html'

  # Check if doxyfile is already present, if so delete it.
  if os.path.exists(doxyfile):
    os.remove(doxyfile)

  # Clean the latex output folder, don't remove the .sty files, we need those!
  clean_files = [".aux", ".eps", ".loc", ".lof", ".log",
                 ".png", ".toc", ".txt", ".tex", ".pdf", "Makefile"]
  for root, dirs, files in os.walk(latexdir):
    for name in files:
      if name.endswith(tuple(clean_files)):
        filename = os.path.join(root, name)
        os.remove(filename)
      elif re.match('dir_', name):
        filename = os.path.join(root, name)
        os.remove(filename)

  # Log file cleanup
  for root, dirs, files in os.walk(docpath_root):
    for name in files:
      if name.endswith('.log'):
        filename = os.path.join(root, name)
        os.remove(filename)

  # aux files, clean up and remake folder
  rmdir(auxdir)
  os.mkdir(auxdir)

  # html files, clean up and remake folder
  rmdir(htmldir)
  os.mkdir(htmldir)

  # Strip the paths to make them relative... doxygen gets grumpy when passed
  # full paths
  for file in source:
    f = str.replace(str(file), str(docpath_root) + "/", ' ')
    env.AppendUnique(INPUT = f)

  # IDL files are handled separately to avoid scons warnings about
  # two rules being applied
  #
  # @@TODO: The IDL files are such a mess to avoid massive demoralization
  #         each file will be handled individually until the work
  #         is complete.
  #
  # idl_path = os.path.relpath(str(idl_src), str(docpath_root))
  # env.AppendUnique(INPUT = " " + str(idl_path))
  env.AppendUnique(INPUT = " src")

  # File patterns for files to search in the src folder
  src_patterns = ["*.c",   "*.cc",  "*.cpp", "*.c++",
                  "*.h",   "*.hh",  "*.hpp", "*.h++",
                  "*.txt", "*.dox", "*.idl" ]
  for pattern in src_patterns:
    env.AppendUnique(FILE_PATTERNS = " " + str(pattern))

  # The output doxygen source file "doxyfile" is made from
  # an autogenerated header plus the contents of "tmplfile"
  
  #Autogenerated header using env variables
  doxyfile_src = """
#---------------------------------------------------------------------------
# Copyright (c) 2010-2011, 2013, 2015-2017 Qualcomm Technologies, Inc. 
# All rights reserved. 
# Confidential and Proprietary - Qualcomm Technologies, Inc.
#---------------------------------------------------------------------------
#---------------------------------------------------------------------------
# Project-related configuration options
#---------------------------------------------------------------------------

# This tag specifies the encoding used for all characters in the config file
# that follow. The default is UTF-8 which is also the encoding used for all
# text before the first occurrence of this tag. Doxygen uses libiconv (or the
# iconv built into libc) for the transcoding. See
# http://www.gnu.org/software/libiconv for the list of possible encodings.

DOXYFILE_ENCODING      = UTF-8
#                        PROJECT_NAME defines the document title on the title
#                        page and uses the $projectname variable defined in
#                        the header.tex and header.html files.
#                        The PROJECT_NUMBER defines the document control
#                        number on the title page and in the footer defined
#                        in the footer.tex and footer.html files.
#                        PROJECT_BRIEF defines the short document title in the pdf header
#                        uses the $projectbrief variable defined in the header.tex and header.html files.
#
#                      DOCUMENT PROPERTIES:
#                        TO REMOVE/ADD STATEMENTS AND MARKINGS FROM/TO THE PDF AND HTML TITLE PAGE, UPDATE title_page.dox
#                        TO UPDATE LEGAL STATEMENT TEXT ON THE PDF AND HTML TITLE PAGE, UPDATE TEXT BELOW UNDER "LEGAL INFO"
#                        TO CHANGE DEFAULT LEGAL STATEMENTS AND MARKINGS IN THE HTML FOOTER, UPDATE footer.html
#                        TO TURN ON/OFF PROPERTIES IN THE PDF FOOTER, SET THESE IN header.tex UNDER "FOOTER PROPERTY SETTINGS"
#                        TO CHANGE THE DOCUMENT TYPE, UPDATE \\doctype IN header.tex
#
#                        DOCUMENT TITLE ON THE PDF TITLE PAGE AND IN THE HTML HEADER:
PROJECT_NAME =             "{env[DOC_NAME]}"
#                        DOCUMENT TITLE ON THE HTML TITLE PAGE:
ALIASES +=                 "doctitlehtml={env[DOC_NAME]}"
#
#                        DOCUMENT SHORTNAME IN THE PDF HEADERS:
PROJECT_BRIEF =            "{env[DOC_SHORTNAME]}"
#                        DOCUMENT SHORTNAME ON THE HTML TITLE PAGE:
ALIASES +=                 "doctypehtml={env[DOC_SHORTNAME]}"
#
#                        DOCUMENT SUBTITLE ON THE PDF AND HTML TITLE PAGE - COMMENT OUT/IN FOR DESIRED SETTING:
#                          FALSE - NO SUBTITLE ON PDF TITLE PAGE (DEFAULT).
ALIASES +=                 "subtitlepdf=\\color{{white}}text\\color{{black}}\\\\[-1.5ex]"
#                          FALSE - NO SUBTITLE ON HTML TITLE PAGE(DEFAULT).
ALIASES +=                 "subtitlehtml=&nbsp;"
#                          TRUE - INCLUDE SUBTITLE ON THE PDF TITLE PAGE:
#ALIASES +=                "subtitlepdf=Sample Subtitle\\\\[1ex] "
#                          TRUE - INCLUDE SUBTITLE ON THE HTML TITLE PAGE:
#ALIASES +=                 "subtitlehtml=<br />Sample Subtitle<br />"
#
#                        "INTERNAL USE ONLY" STATEMENT ON THE PDF AND HTML TITLE PAGE - COMMENT OUT/IN FOR DESIRED SETTING:
#                          FALSE - NO STATEMENT ON THE PDF TITLE PAGE (DEFAULT):
ALIASES +=                 "internaluseonlypdf=\\color{{white}}text\\color{{black}}\\\\[-2.5ex]"
#                          FALSE - NO STATEMENT ON THE HTML TITLE PAGE (DEFAULT):
ALIASES +=                 "internaluseonlyhtml=&nbsp;"
#                          TRUE - INCLUDE STATEMENT ON THE PDF TITLE PAGE:
#ALIASES +=                "internaluseonlypdf=\\textbf{{INTERNAL USE ONLY}}\\\\[1.5ex]"
#                          TRUE - INCLUDE STATEMENT ON THE HTML TITLE PAGE:
#ALIASES +=                "internaluseonlyhtml=INTERNAL USE ONLY<br />"
#
#                        DOCUMENT DCN ON THE PDF TITLE PAGE, AND IN THE PDF AND HTML FOOTERS:
PROJECT_NUMBER         =   $(ENV_PRE)"{env[DOC_NUMBER]} {env[DOC_REVISION]}"
#                        DOCUMENT DCN ON THE HTML TITLE PAGE:
ALIASES +=                 "dcnhtml=$(ENV_PRE){env[DOC_NUMBER]} {env[DOC_REVISION]}"
#
#                        DOCUMENT COPYRIGHT DATE ON THE PDF AND HTML TITLE PAGE - Change dates for both:
ALIASES +=                 "copyright_titlepage=@latexonly \\begin{{center}} \\footnotesize \\copyright \\space 2018 Qualcomm Technologies, Inc. All rights reserved. \\\\ \\end{{center}} @endlatexonly @htmlonly <center>&copy;2018 Qualcomm Technologies, Inc. All rights reserved </center>@endhtmlonly"
#
  """.format(env=env)

  # Input template doxyfile as used by doxygen
  with open(str(tmplfile), 'r') as f:
    doxyfile_src += f.read();

  # Update the template then
  with open(str(doxyfile), 'w+') as f:
    f.write(doxyfile_src)
    f.write("\nINPUT = " + str(env.get('INPUT')))
    f.write("\nFILE_PATTERNS = " + env.get('FILE_PATTERNS'))

  # Everything is in place, attempt compile the document
  compile_document(env)

  return None

#-------------------------------------------------------------------------------
# doxygen execution
#-------------------------------------------------------------------------------
def compile_document(env):

  # Some path information
  build_root    = os.path.expanduser(env.get('BUILD_ROOT'))
  docpath_root  = build_root   + str(env.get('doxy_dir'))
  srcdir        = build_root   + str(env.get('src_dir'))
  latex_dir     = docpath_root + '/latex'
  auxfiles_dir  = latex_dir    + '/auxfiles'
  scripts_dir   = docpath_root + '/scripts'
  htmlfiles_dir = docpath_root + '/html'

  # Use specific versions of doxygen and plantuml
  doxygen       = docpath_root + '/tools/doxygen'
  plantuml      = docpath_root + '/tools/plantuml.jar'

  # Tools and environment check
  os.chmod(doxygen, os.stat(doxygen).st_mode | stat.S_IEXEC)
  if tool_check(env, doxygen):
    sys.exit(1)
  if tool_check(env, 'pdflatex'):
    sys.exit(1)
  if tool_check(env, 'java'):
    sys.exit(1)

  # Search source for any PlantUML and create images as required for inclusion in
  # the documentation
  #
  # Restrict search to look only under the docpath_root
  exec_command(env, 'java -Djava.awt.headless=true -jar ' + plantuml + ' -v -o ' + docpath_root + '/other_src/images "./**.uml"', docpath_root)

  # Generate some latex
  doxy_config = '/Doxy_Config.ldd'
  alias_include_file = '/other_latex_aliases.txt'
  os.environ["ALIAS_INCLUDE_FILE"] = docpath_root + alias_include_file
  os.environ["ENV_LAYOUT_FILE"] = 'Doxygen_Layout_Standard_latex.xml'
  exec_command(env, doxygen + ' ' + docpath_root + doxy_config + " > latex.doxy.log 2>&1", docpath_root)

  # Remove the created doxygen.sty and replace with our own custom version
  filename = os.path.join(latex_dir, 'default.sty')
  if os.path.isfile(filename):
    os.remove(filename)
  copy_file(latex_dir + '/template_doxygen.sty', latex_dir + '/doxygen.sty')
  os.chmod(latex_dir + '/doxygen.sty', os.stat(latex_dir + '/doxygen.sty').st_mode | stat.S_IWRITE)

  # The 'latexpostprocessing.py' script performs all post processing to the generated latex files and
  # updates are made to refman.tex.
  # All indexes are commented out and the default chapter name "Module Documentation: is changed to "Interfaces" or
  # whatever has been set in the mainpage.dox for this chapter or other chapters.
  # For each generated tex file referenced in refman.tex, the following updates are made:
  #  - If any static mbox's exist then they are deleted.
  #  - The Modules subsection is deleted.
  #  - All Detailed Description sections are deleted. For deletion to work, the "requires_insertion_of_clear_page s"tring must be
  #    be present so that a clearpage command is inserted at the end of the struct table.
  latexpostprocessing = os.path.join(scripts_dir, 'latexpostprocessing.py')
  if os.path.isfile(latexpostprocessing):
    exec_command(env, 'python ' + latexpostprocessing, latex_dir)
  else:
    env.PrintError('Missing ' + latexpostprocessing + ' file')
    sys.exit(1)

  # Documentation generation!
  env.Print('******************** Document generating ********************')

  # Move the refman.pdf to the target output filename. We do this here to avoid
  # the need to change the LaTeX post processing script generated by techpubs
  outputfilename = str(env.get('outputfilename')) + '.tex'
  source = os.path.join(latex_dir, 'refman.tex')
  dest = os.path.join(latex_dir, outputfilename)
  move_file(source, dest)

  # 1st run generates the LaTeX files: .aux, .lof, .log, .lot, .out, .toc, .tex, .bib, make.bat, Makefile,
  # doxygen.sty, and .idx (if selected in config file)
  cmd = 'pdflatex -interaction=nonstopmode ' + outputfilename
  exec_command(env, cmd, latex_dir)

  # 2nd run generates the TOC and includes, Index (from refman.idx, if selected), Bibliography (using .bib, if
  # selected). Uncomment the lines below to generate the Index and/or Bibliography if to be included in the PDF.
  # makeindex refman.idx
  exec_command(env, cmd, latex_dir)

  # 3rd run generates the TOC and linking files
  exec_command(env, cmd, latex_dir)

  # A little cleanup
  move_files = [".aux", ".log", ".toc", ".lot", ".lof"]
  files = os.listdir(latex_dir)
  for name in files:
    if name.endswith(tuple(move_files)):
      source = os.path.join(latex_dir, name)
      dest = os.path.join(auxfiles_dir, name)
      move_file(source, dest)

  # Clean the temporary src folder if it exists
  rmdir(srcdir)

  # Set up env variables before running doxygen to generate html
  #env.Print( '******************* Building HTML document ******************')
  #os.environ["ALIAS_INCLUDE_FILE"] = docpath_root + '/other_html_aliases.txt'
  #os.environ["ENV_GEN_HTML"] = 'YES'
  #os.environ["ENV_GEN_LATEX"] = 'NO'
  #os.environ["ENV_ENABLED_SECTIONS"] = 'HTML_ONLY'
  #os.environ["ENV_LAYOUT_FILE"] = docpath_root + './Doxygen_Layout_Standard_html.xml'
  #exec_command(env, doxygen + ' ' + docpath_root + doxy_config, docpath_root)

  # Copying css files to the html directory
  #copy_files = [".css"]
  #files = os.listdir(docpath_root)
  #for name in files:
  #  if name.endswith(tuple(copy_files)):
  #    source = os.path.join(docpath_root, name)
  #    dest = os.path.join(htmlfiles_dir, name)
  #    copy_file(source, dest)

  env.Print('**************** Document generation complete ***************')
  return None

#===============================================================================
# Utility functions
#===============================================================================

# Generic function to remove a top level directory plus sub-directories
def rmdir(top):
  if os.path.isdir(top):
    for root, dirs, files in os.walk(top, topdown=False):
      for file in files:
        filename = os.path.join(root, file)
        os.chmod(filename, stat.S_IWRITE)
        os.remove(filename)
      for name in dirs:
        os.rmdir(os.path.join(root, name))
    os.rmdir(top)

# Execute a command at the current working directory and return the exit status
def exec_command(env, cmd, directory):
  import subprocess
  process = subprocess.Popen(cmd, cwd=directory, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
  (stdoutdata, stderrdata) = process.communicate()
  returncode = process.wait()
  if returncode != 0:
    env.PrintError("Error: failed to execute command: " + cmd)
    env.PrintError(stderrdata)
  return returncode

# Checks if a tool is available and returns an error if it's not found
def tool_check(env, tool):
  env.Print("Checking for " + tool)
  return exec_command(env, tool + ' -version', './')


