mkdir bin
cd bin 
cmake -G "Visual Studio 15 2017 Win64" --DCMAKE_BUILD_TYPE=Release ..

%cmake --build . --config Release 
pause