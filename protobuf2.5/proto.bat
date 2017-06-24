protoc.exe --cpp_out=. common.proto

set client_path=..\MyClient\frameworks\runtime-src\Classes\Protobuf
set server_path=..\MyServer\MyServer\protobuf
set src_path=..\MyClient\src\pbc


copy common.pb.h %client_path%\common.pb.h
copy common.pb.cc %client_path%\common.pb.cc

copy common.proto %src_path%\common.proto

copy common.pb.h %server_path%\common.pb.h
copy common.pb.cc %server_path%\common.pb.cc

pause
exit
