@echo off
CALL checkDataSize

FOR %%X IN (00 01 02 03 04 05 06 07) DO .\hex2bin32 -l 0x10000 -e X%%X ".\Obj\%1.H%%X"

MD BIN
COPY/B ".\Obj\%1.X*" ".\BIN\%2.BIN"
DEL ".\Obj\%1.X*"
DEL ".\Obj\%1.H*"

.\Checksum_64Bit ".\BIN\%2.BIN"
.\CheckSumBK2 ".\BIN\%2.BIN .\BIN\Checksum.BIN CheckSumAddr=0x2FFFE ckm32=0 BigEndianMode=1"
set yyyymmdd_hhmmss=%date:~0,4%%date:~5,2%%date:~8,2%_%time:~0,2%%time:~3,2%%time:~6,2%

echo %yyyymmdd_hhmmss%