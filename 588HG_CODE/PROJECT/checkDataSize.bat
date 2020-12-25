@ECHO OFF

REM **************************************************************************************
REM Author:zhifeng.wu	version:0.02	Date:080714	E-mail:zhifeng.wu@tpvaoc.com
REM **************************************************************************************

REM **************************SETTINGS****************************************************
REM You Need To Set the Warning Size of data &&*.M51 file's path
SET WARNING_SIZE=192
SET M51_PATH=.\list\*.M51
REM **************************************************************************************
FOR /F "tokens=3 delims= " %%a in ('findstr "Program Size: data=" "%M51_PATH%"') DO SET SIZE=%%a

SET SIZE=%SIZE:~5%
SET /A SIZE=SIZE
ECHO Data size=%SIZE%

  IF %SIZE% GEQ %WARNING_SIZE% (
  ECHO createobject^("wscript.shell"^).run "cmd /cecho ",0 >Message.vbs
  REM To Create a vbs file.
  REM When Data Size is more than WARNING_SIZE,It will echo "" character to call buzzer to buzz.  
  ECHO msgbox^("Data Size=%SIZE%!!!!It Maybe Overflow!Please Check Data Size."^) >>Message.vbs
  Message.vbs
  DEL /Q Message.vbs
) ELSE (
  ECHO Data size check OK.
)
