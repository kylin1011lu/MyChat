--网络管理
--create by kylin @ 2017-5-15

local NetworkManager = class("NetworkManager")

function NetworkManager:ctor()
   if self.init then self:init() end
end

function NetworkManager:init()
	printInfo("NetworkManager:init")

end

function NetworkManager:connectServer()
	printInfo("Download NetworkManager:connectServer")
	cci.connectServer(Def.ServerAddress,Def.ServerPort)
end

return NetworkManager