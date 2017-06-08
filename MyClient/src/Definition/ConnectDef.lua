--网络连接设置
--create by kylin @ 2017-5-15

local Def = {}
cc.exports.Def = Def


--虚拟机ip
-- Def.ServerAddress = "192.168.248.128"
--本机
if TARGETPLATFORM == cc.PLATFORM_OS_ANDROID then
	--Def.ServerAddress = "10.0.2.6"
	--阿里云服务器
	Def.ServerAddress="59.110.213.172"
else
	Def.ServerAddress = "127.0.0.1"

	--阿里云服务器
	--Def.ServerAddress="59.110.213.172"
end

Def.ServerPort=12012