@rem $OpenXM: OpenXM_contrib2/windows/makepkg.bat,v 1.2 2013/11/11 04:23:51 ohara Exp $
@echo off

if exist asir ( rmdir /q /s asir )

if /i "%Platform%" == "X64" (
  if not exist asir32gui\asirgui.exe ( call makebin64.bat )
) else (
  if not exist asir32gui\asirgui.exe ( call makebin32.bat )
)

mkdir asir\bin asir\help\ja asir\lib asir\lib-asir-contrib asir\share\editor 

for %%i in ( asir32gui\asirgui.exe asir32gui\ja.dll engine2000\engine.exe mcpp\cpp.exe post-msg-asirgui\cmdasir.exe ..\asir2000\asir.exe ) do (
  copy /b %%i asir\bin
)

pushd help
if not exist ja ( call makehelp2.bat ja )
if not exist en ( call makehelp2.bat en )
popd

copy /b help\ja\*.chm asir\help\ja
copy /b help\en\*.chm asir\help

for %%i in ( asirgui.mac  asir-mode.el install-ja-sjis.txt ) do ( 
  copy /b post-msg-asirgui\%%i asir\share\editor
)
copy /b ..\asir2000\LICENSE asir
xcopy /q /s ..\asir2000\lib asir\lib
del /q asir\lib\Makefile*
del /q asir\lib\help*.uu

xcopy /q /s  ..\..\OpenXM\src\asir-contrib\packages\src asir\lib-asir-contrib
copy /b ..\..\OpenXM\src\asir-contrib\testing\noro\ndbf.rr asir\lib-asir-contrib\nn_ndbf.rr
for %%i in ( de.rr gw.rr module_syz.rr mwl.rr pd.rr rewrite.rr ) do (
  copy /b ..\..\OpenXM\src\asir-contrib\testing\noro\%%i asir\lib-asir-contrib\noro_%%i
)
del /q asir\lib-asir-contrib\Makefile.in
del /q asir\lib-asir-contrib\y_prime\.keepme

if /i "%Platform%" == "X64" (
  zip -r asir_win64_%DATE:/=.%.zip asir
) else (
  zip -r asir_win32_%DATE:/=.%.zip asir
)

exit /b 0