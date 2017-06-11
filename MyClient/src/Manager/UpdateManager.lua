--UpdateManager
--create by kylin @ 2017-6-10

local UpdateManager = class("UpdateManager")

local assetsManager = nil
function UpdateManager:ctor()
   if self.init then self:init() end
end

function UpdateManager:init()
	printInfo("UpdateManager:init")

    local function onError(errorCode)
        if errorCode == cc.ASSETSMANAGER_NO_NEW_VERSION then
            printInfo("no new version")
        elseif errorCode == cc.ASSETSMANAGER_NETWORK then
            printInfo("network error")
        end

        sNetworkManager.connectServer()
    end

    local function onProgress( percent )
        local progress = string.format("downloading %d",percent)
        printInfo(progress)
    end

    local function onSuccess()
        printInfo("downloading ok")

        ReloadAllLuaFile()
       	sNetworkManager.connectServer()
    end

	if assetsManager == nil then
	    assetsManager = cc.AssetsManager:new(Def.UpdatePackagePath,Def.UpdateVersionPath,HOT_REQUIRE_PATH)
	    assetsManager:retain()
	    assetsManager:setDelegate(onError, cc.ASSETSMANAGER_PROTOCOL_ERROR )
	    assetsManager:setDelegate(onProgress, cc.ASSETSMANAGER_PROTOCOL_PROGRESS)
	    assetsManager:setDelegate(onSuccess, cc.ASSETSMANAGER_PROTOCOL_SUCCESS )
	    assetsManager:setConnectionTimeout(3)
	end
end

function UpdateManager:StartUpdate()
	assetsManager:checkUpdate()
    printInfo("UpdateManager:StartUpdate")
end

return UpdateManager
