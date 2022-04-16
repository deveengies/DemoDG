@echo off
echo test send a file to all clients when everyone is connected ...

set /p x="press enter to run Server" 
echo Run Server
call runServer.bat 127.0.0.1 8282

set /p x="press enter to run Client 1/4"
echo Run Client joe
call runClient.bat 127.0.0.1 8282 joe 1dal

set /p x="press enter to run Client 2/4"
echo Run Client jack
call runClient.bat 127.0.0.1 8282 jack 2dal

set /p x="press enter to run Client 3/4"
echo Run Client william
call runClient.bat 127.0.0.1 8282 william 3dal

set /p x="press enter to run Client 4/4"
echo Run Client averell
call runClient.bat 127.0.0.1 8282 averell 4dal

@echo off
echo joe jack william averell should have created new files in ../workDir


