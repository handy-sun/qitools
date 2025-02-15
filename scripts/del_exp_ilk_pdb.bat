@echo off
for /r %%i in (*.exp;*.ilk;*.pdb;*.a;*.lib) do (
    echo del "%%i"
    del /a /f /q %%i
)