@echo off
rem ==========================================================================
rem
rem  Set Environment Script for Windows
rem
rem  General Description
rem     Setup Qualcomm-specific environment variables for the build system.
rem
rem Copyright (c) 2009-2012 by QUALCOMM, Incorporated.
rem All Rights Reserved.
rem QUALCOMM Proprietary/GTDR
rem
rem --------------------------------------------------------------------------
rem
rem $Header: //components/rel/scons.tzw/1.0/build/scons/build/setenv.cmd#1 $
rem $DateTime: 2018/09/05 05:45:12 $
rem $Change: 17044639 $
rem
rem ==========================================================================

call %1.cmd
set  BUILDSPEC=KLOCWORK
rem Dump environment to stdout so that calling scripts can read it.
set
