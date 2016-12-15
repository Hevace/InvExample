REM Call the visual studio default environment setup for 32 bit
call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\bin\vcvars32.bat"

pushd vs
"C:\Program Files (x86)\MSBuild\12.0\Bin\msbuild" InvExample.sln  /m /t:Rebuild /p:Configuration=Release;Platform=Win32
popd

