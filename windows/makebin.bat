@echo off

msdev ..\asir2000\asir2000.dsp /make "asir2000 - release"
msdev asir2000lib\asir2000lib.dsp /make "asir2000lib - release"
msdev engine2000\engine2000.dsp /make "engine2000 - release"

del asir32gui\Release\asir32gui.res
copy asir32gui\asir32gui.rc.JP asir32gui\asir32gui.rc
msdev asir32gui\asir32gui.dsp /make "asir32gui - release"
move asir32gui\Release\asirgui.exe asir32gui\Release\asirgui-ja.exe 

del asir32gui\Release\asir32gui.res
copy asir32gui\asir32gui.rc.EG asir32gui\asir32gui.rc
msdev asir32gui\asir32gui.dsp /make "asir32gui - release"
move asir32gui\Release\asirgui.exe asir32gui\Release\asirgui-en.exe 

msdev cpp\cpp.dsp /make "cpp - release"

makedist