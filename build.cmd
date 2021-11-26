@echo off

IF "%SDK_LOADED%"=="" (goto :init) else (goto :compile)

:init
	set OUTDIR=obj\x64\Debug
	set SDK_LOADED="TRUE"
	call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvars64.bat"
	call "C:\Program Files (x86)\IntelSWTools\compilers_and_libraries_2018\windows\bin\compilervars.bat" intel64		
	echo %SDK_LOADED%
	goto :compile

:compile
	cl.exe src\main.cpp ^
	-I src ^
	-I"%MKLROOT%\include" ^
	-EHsc -Od -GS -Zi ^
	-Fo"%OUTDIR%\ENTD" ^
	-Fe"%OUTDIR%\ENTD" ^
	mkl_core.lib mkl_intel_lp64.lib mkl_intel_thread.lib libiomp5md.lib
	%OUTDIR%\ENTD.exe
