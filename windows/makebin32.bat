@rem $OpenXM: OpenXM_contrib2/windows/makebin32.bat,v 1.3 2009/03/18 05:19:43 ohara Exp $
@echo off

set ARCH=x86

cd gc-7.1
nmake -f NT_STATIC_THREADS_MAKEFILE gc.lib
cd ..

cd pari20
nmake -f Makefile.vc6
cd ..

for %%i in ( asir2000lib engine2000 mcpp asir32gui ) do (
  cd %%i
  nmake -f Makefile.vc
  cd ..
)

cd asir32gui
nmake -f Makefile.vc LANG=en
cd ..

cd ..\asir2000
nmake -f Makefile.vc
cd ..\windows