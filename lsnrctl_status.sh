#!/bin/bash
var="$(echo `lsnrctl status` | tail -c 8)"
if(var=="refused"); then /u01/app/oracle/product/11.2.0/xe/bin/lsnrctl start; echo jailbreak32 | sudo -S service oracle-xe start; fi
exit 0 #Выход с кодом 0 (удачное завершение работы скрипта)
