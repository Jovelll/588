@echo off
CALL checkDataSize
HEX2BIN /Q /L262144 obj\TSUM.H00 obj\TSUM56.BIN
HEX2BIN /Q /O65536 /M obj\TSUM.H01 obj\TSUM56.BIN
HEX2BIN /Q /O131072 /M obj\TSUM.H02 obj\TSUM56.BIN
HEX2BIN /Q /O196608 /M obj\TSUM.H03 obj\TSUM56.BIN
bankcheck.exe .\\List\\TSUM.m51
MD BIN
DEL /Q/S BIN\*.*
COPY Checksum.exe BIN\
cd obj
COPY "TSUM56.BIN" %1
DEL "TSUM56.*"
copy %1 ..\bin
del %1
cd..
cd bin
Checksum %1 /b
del Checksum.exe
Pause