
///////////////////////////////////////
AzureAD+KienNguyenTrung@DESKTOP-Q68Q3TE UCRT64 /c/tool/cantracing/build
$ where libstdc++-6.dll
C:\msys64\ucrt64\bin\libstdc++-6.dll

AzureAD+KienNguyenTrung@DESKTOP-Q68Q3TE UCRT64 /c/tool/cantracing/build
$ chmod +x deploy_dlls.sh

AzureAD+KienNguyenTrung@DESKTOP-Q68Q3TE UCRT64 /c/tool/cantracing/build
$ ./deploy_dlls.sh
📦 Đang phân tích ./cantracing.exe với ldd...
🔄 Copy: libgcc_s_seh-1.dll
🔄 Copy: libstdc++-6.dll
🔄 Copy: libwinpthread-1.dll
🔄 Copy: libicuuc77.dll
🔄 Copy: libdouble-conversion.dll

copy file "deploy_dlls.sh" vào build và chạy như trên 
nếu bị lỗi entry và không chạy được double click file .exe

//////////////////////////////////////


Connect
- Power Supply
- ECU1, CM 1000, EES, ECU2, PC
- Test case 1: connect SFP + RJ45 vào EES - conf chân
- Test case 2: 2 cổng RJ45 từ CM 1000, EES vào Extend

Inject Fault
- 