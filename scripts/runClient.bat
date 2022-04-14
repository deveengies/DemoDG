
echo Run Client ip=%1 port=%2 ident=%3
cd ../Client/Debug/

IF NOT EXIST ".\libssl-1_1.dll" (
    copy ..\..\..\libraries\OpenSSL\bin\libssl-1_1.dll .
)

IF NOT EXIST ".\libcrypto-1_1.dll" (
    copy ..\..\..\libraries\OpenSSL\bin\libcrypto-1_1.dll .
)

start cmd /k Client.exe  %1 %2 %3
cd ../../scripts