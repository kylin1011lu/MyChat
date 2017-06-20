protoc.exe --cpp_out=. common.proto

set client_path=D:\Cocos\MyChat\frameworks\runtime-src\Classes\Protobuf
set server_path=D:\GitHub\MyChat\MyServer\MyServer\protobuf
set src_path=D:\Cocos\MyChat\src\pbc


copy common.pb.h %client_path%\common.pb.h
copy common.pb.cc %client_path%\common.pb.cc

copy common.proto %src_path%\common.proto

copy common.pb.h %server_path%\common.pb.h
copy common.pb.cc %server_path%\common.pb.cc

pause
exit
