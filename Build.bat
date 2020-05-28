mkdir bin
cd bin 
cmake -G "Visual Studio 16 2019" --DCMAKE_BUILD_TYPE=Release ..

%cmake --build . --config Release 
pause
start "" MoosE.sln