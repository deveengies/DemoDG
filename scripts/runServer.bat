@echo off

echo Run Server ip=%1 port=%2
cd ../Server/Debug/

IF NOT EXIST ".\libssl-1_1.dll" (
    copy ..\..\..\libraries\OpenSSL\bin\libssl-1_1.dll .
)

IF NOT EXIST ".\libcrypto-1_1.dll" (
    copy ..\..\..\libraries\OpenSSL\bin\libcrypto-1_1.dll .
)

start cmd /k Server.exe  %1 %2 %cd%/../../workdir
cd ../../scripts