
-- 更新主控制器(单例)
local UpdateController = class("UpdateController", {})

function UpdateController:ctor( ... )
    

end

-- 进入更新功能
function UpdateController:enterUpdate()
	-- 启动更新场景(View)
    self.updateScene = new_class(luafile.updateScene, self)
    cocosMake.runScene(self.updateScene)
	
	
	if TARGETPLATFORM == cc.PLATFORM_OS_ANDROID then
		local loadingSceneCallback
		local function callback(result)
			logc("UpdateController.initZZSDK.callback", result)
			local result = string.split(result, ",")
			local operate = result[1]
			if "noUpdate" == operate then
				self:enterHotUpdate()
			
			elseif "versionUpdate" == operate then
				loadingSceneCallback = self.updateScene:showTipPackageUpdate(result[2], result[3])
				
			else
				loadingSceneCallback(operate, result[2], result[3])
			end
		end
		osFunction.initZZSDK(PLATFORM_ID, 0, GAME_VERSION, callback)
		
	else
		self:enterHotUpdate()
	end
end

function UpdateController:enterHotUpdate()
	
    -- 准备更新环境
    self:prepareEnvironment()

    -- 检测版本更新
	self:checkUpdate()
end

-- 准备更新环境（创建更新目录及更新数据文件）
function UpdateController:prepareEnvironment()
	local versionCode = osFunction.getVersionCode()
	log("UpdateController.osFunction.versionCode=", versionCode)

	-- 准备版本存放位置及数据文件
	self.versionDir = cc.FileUtils:getInstance():getWritablePath() .. string.format("version%s/", tostring(versionCode))
	self.versionDataFile = self.versionDir .. "version.dat"
	if not cc.FileUtils:getInstance():isDirectoryExist(self.versionDir) then
		cc.FileUtils:getInstance():createDirectory(self.versionDir)
	end
	if not cc.FileUtils:getInstance():isFileExist(self.versionDataFile) then
		cc.FileUtils:getInstance():writeStringToFile(json.encode({
			versionRootPath = "",
			searchPaths = {},
		}), self.versionDataFile)
	end
end

-- 检测更新
function UpdateController:checkUpdate()
	-- 创建检查更新的 AssetsManagerEx 对象
	log("UpdateController.getManifestPath:", self:getManifestPath())
	self.checkAssetsManagerEx = cc.AssetsManagerEx:create(self:getManifestPath(), self.versionDir)
	self.checkAssetsManagerEx:retain()

	-- 本地不存在更新清单文件，跳过更新直接进入游戏
	if not self.checkAssetsManagerEx:getLocalManifest():isLoaded() then
		print("AssetsManagerEx : No local manifest file found error.")
		self:enterGame()
		return
	end

	-- 本地版本号
	self.localVersion = self.checkAssetsManagerEx:getLocalManifest():getVersion()
	log("UpdateController.localVersion:", self.localVersion)
	self:onUpdateGameVersion(self.localVersion)
	

	self.checkUpdateHandler  = cc.EventListenerAssetsManagerEx:create(self.checkAssetsManagerEx, handler(self, self.onCheckUpdateEvent))
	cc.Director:getInstance():getEventDispatcher():addEventListenerWithFixedPriority(self.checkUpdateHandler , 1)
	log("checkAssetsManagerEx:checkUpdate")
	self.checkAssetsManagerEx:checkUpdate()
end

function UpdateController:onCheckUpdateEvent(event)
	log("checkAssetsManagerEx:onCheckUpdateEvent：", event:getEventCode())
	if event:getEventCode() == cc.EventAssetsManagerEx.EventCode.ERROR_NO_LOCAL_MANIFEST then -- 发生错误:本地找不到manifest文件
		print("AssetsManagerEx : No local manifest file found, skip assets update.")
		self:enterGame()
	elseif event:getEventCode() == cc.EventAssetsManagerEx.EventCode.ERROR_DOWNLOAD_MANIFEST then -- 发生错误:下载manifest文件失败
		print("AssetsManagerEx : Fail to download manifest file, update skipped.")
		self:enterGame()
	elseif event:getEventCode() == cc.EventAssetsManagerEx.EventCode.ERROR_PARSE_MANIFEST then -- 发生错误:解析manifest文件失败
		print("AssetsManagerEx : Fail to parse manifest file, update skipped.")
		self:enterGame()
	elseif event:getEventCode() == cc.EventAssetsManagerEx.EventCode.NEW_VERSION_FOUND then -- 发现新版本
		print("AssetsManagerEx : Found new version.")
		self:tipUpdate()
	elseif event:getEventCode() == cc.EventAssetsManagerEx.EventCode.ALREADY_UP_TO_DATE then -- 已经是最新版本了，进入游戏主界面
		print("AssetsManagerEx : Already up to date.1")
		self:enterGame()
	elseif event:getEventCode() == cc.EventAssetsManagerEx.EventCode.ERROR_UPDATING then -- 发生错误:更新错误
		print("AssetsManagerEx : Error updating.")
		self:enterGame()
	elseif event:getEventCode() == cc.EventAssetsManagerEx.EventCode.UPDATE_FINISHED then -- 更新完毕，进入游戏
		print("AssetsManagerEx : Update finished.")
		self:enterGame()
	end
end

-- 提示有新版本更新
function UpdateController:tipUpdate()
	-- 检测到最新版本号
	self.remoteVersion = self.checkAssetsManagerEx:getRemoteManifest():getVersion()
	print("AssetsManagerEx : Found new version ", self.remoteVersion)

	-- 根据版本号得到更新类型
	self.updateType = self:getUpdateType(self.localVersion, self.remoteVersion)

	-- 下载下来的version.manifest文件
	local versionFileName = string.format("%sversion.manifest", self.versionDir)
	if self.updateType and cc.FileUtils:getInstance():isFileExist(versionFileName) then
		-- 读取新版本version.manifest文件信息
		local data = cc.FileUtils:getInstance():getStringFromFile(versionFileName)
		self.remoteVersionData = json.decode(data)

		-- 展示更新提示界面
		self.updateScene:showTipUpdate(self.updateType, self.remoteVersionData)
	else
		-- 未得到更新类型直接进入游戏
		self:enterGame()
	end
end

-- 正式开始更新
function UpdateController:startUpdate()
	-- 展示更新loading界面
	self.updateScene:showUpdateLoading()

	-- 更新的 AssetsManagerEx 对象
	self.assetsManagerEx = cc.AssetsManagerEx:create(self:getManifestPath(), string.format("%s%s/", self.versionDir, self.remoteVersion))
	self.assetsManagerEx:retain()

	-- 本地不存在更新清单文件，跳过更新直接进入游戏
	if not self.assetsManagerEx:getLocalManifest():isLoaded() then 
		Log("AssetsManagerEx : No local manifest file found error.")
		self:enterGame()
		return
	end

	-- 本地版本号
	self.localVersion = self.assetsManagerEx:getLocalManifest():getVersion()
	log(">>>>>>>>> 2 LocalVersion = " .. self.localVersion)
	self:onUpdateGameVersion(self.localVersion)
	
	self.updateHandler  = cc.EventListenerAssetsManagerEx:create(self.assetsManagerEx, handler(self, self.onUpdateEvent))
	cc.Director:getInstance():getEventDispatcher():addEventListenerWithFixedPriority(self.updateHandler , 1)
	self.assetsManagerEx:update()
end

function UpdateController:onUpdateEvent(event)
	local eventCode = event:getEventCode()
	log("UpdateController:onUpdateEvent：", eventCode)
	if eventCode == cc.EventAssetsManagerEx.EventCode.ERROR_NO_LOCAL_MANIFEST then -- 发生错误:本地找不到manifest文件
		print("AssetsManagerEx : No local manifest file found, skip assets update.")
		self:enterGame()
	elseif eventCode == cc.EventAssetsManagerEx.EventCode.ERROR_DOWNLOAD_MANIFEST then -- 发生错误:下载manifest文件失败
		print("AssetsManagerEx : Fail to download manifest file, update skipped.")
		self:enterGame()
	elseif eventCode == cc.EventAssetsManagerEx.EventCode.ERROR_PARSE_MANIFEST then -- 发生错误:解析manifest文件失败
		print("AssetsManagerEx : Fail to parse manifest file, update skipped.")
		self:enterGame()
	elseif eventCode == cc.EventAssetsManagerEx.EventCode.NEW_VERSION_FOUND then -- 发现新版本，开始升级
		print("AssetsManagerEx : New version found.")
	elseif eventCode == cc.EventAssetsManagerEx.EventCode.ALREADY_UP_TO_DATE then -- 已经是最新版本了，进入游戏主界面
		print("AssetsManagerEx : Already up to date.2")
		self:enterGame()
	elseif eventCode == cc.EventAssetsManagerEx.EventCode.UPDATE_PROGRESSION then -- 更新进度
		local assetId = event:getAssetId()
        local percent = math.modf(event:getPercentByFile())
        print("AssetsManagerEx : percent = " .. percent)
        self.updateScene:setUpdatePercent(percent)
	elseif eventCode == cc.EventAssetsManagerEx.EventCode.ASSET_UPDATED then
	elseif eventCode == cc.EventAssetsManagerEx.EventCode.ERROR_UPDATING then -- 发生错误:更新错误
		print("AssetsManagerEx : Error updating.")
	elseif eventCode == cc.EventAssetsManagerEx.EventCode.UPDATE_FINISHED then -- 更新完毕，进入游戏
		print("AssetsManagerEx : Update finished.")
		self:updateFinish()
	elseif eventCode == cc.EventAssetsManagerEx.EventCode.UPDATE_FAILED then
		print("AssetsManagerEx : Update failed.")
		self.assetsManagerEx:downloadFailedAssets()
	elseif eventCode == cc.EventAssetsManagerEx.EventCode.ERROR_DECOMPRESS then
		print("AssetsManagerEx : Error decompress.")
	end
end

function UpdateController:updateFinish()
	-- 更新成功把该版本的路径添加到本地存储文件中
	local data = cc.FileUtils:getInstance():getStringFromFile(self.versionDataFile)
	local versionData = json.decode(data)
	versionData.versionRootPath = string.format("%s%s/", self.versionDir, self.remoteVersion)
	local assetsManagerExSearchPaths = self.assetsManagerEx:getLocalManifest():getSearchPaths()
	for i,v in ipairs(assetsManagerExSearchPaths) do
		table.insert(versionData.searchPaths, v)
	end
	cc.FileUtils:getInstance():writeStringToFile(json.encode(versionData), self.versionDataFile)
	
	-- 本地版本号
	self.localVersion = self.remoteVersion
	print(">>>>>>>>> 1 LocalVersion = " .. self.localVersion)
	self:onUpdateGameVersion(self.localVersion)
	
	-- 延迟切换场景
	local delayTime = cc.DelayTime:create(1)
	local callFunc = cc.CallFunc:create(function ( ... )
		self:enterGame()
	end)
	self.updateScene:runAction(cc.Sequence:create(delayTime, callFunc))
end

-- 进入游戏
function UpdateController:enterGame()
	logc("testUpdate 1")
	-- 移除监听
	if self.checkUpdateHandler then
		cc.Director:getInstance():getEventDispatcher():removeEventListener(self.checkUpdateHandler)
		self.checkUpdateHandler = nil
	end
	logc("testUpdate 2")
	-- 移除监听
	if self.updateHandler then
		cc.Director:getInstance():getEventDispatcher():removeEventListener(self.updateHandler)
		self.updateHandler = nil
	end
	logc("testUpdate 3")
	-- 移除检测更新对象
	if self.checkAssetsManagerEx then
		self.checkAssetsManagerEx:release()
		self.checkAssetsManagerEx = nil
	end
	
	logc("testUpdate 4")
	-- 移除更新对象
	if self.assetsManagerEx then
		self.assetsManagerEx:release()
		self.assetsManagerEx = nil
	end
	
	logc("testUpdate 5")
	-- 根据当前的searchpath优先级，重新load lua文件
	HotRequire("preload")
	
	logc("testUpdate 6")
	HotRequire("game.preload")

	logc("testUpdate 7")
	
	-- 初始化游戏控制器
    GameController = new_class(luafile.GameController)
	
	logc("testUpdate 8")
	log("GameController:startGame()")
	GameController:startGame()
	logc("testUpdate 9")
end

function UpdateController:getManifestPath()
	local manifestPath = "project.manifest"
	local data = cc.FileUtils:getInstance():getStringFromFile(self.versionDataFile)
	if data and data ~= "" then
		local versionData = json.decode(data)
		if versionData and versionData.versionRootPath and versionData.versionRootPath ~= "" then
			manifestPath = versionData.versionRootPath .. manifestPath
		end
	end
	return manifestPath
end

-- 得到更新的类型
function UpdateController:getUpdateType(localVersion, remoteVersion)
	local regular = "^[0-9]+%.[0-9]+%.[0-9]+%.[0-9]+$"
	if not (localVersion and remoteVersion
			and string.find(localVersion, regular)
			and string.find(remoteVersion, regular)) then
		return
	end

	--分割版本号字符串
	local localVersionTab = string.split(localVersion, ".")
	local remoteVersionTab = string.split(remoteVersion, ".")

	for i = 1, #localVersionTab do
		if localVersionTab[i] ~= remoteVersionTab[i] then
			return i
		end
	end
end

-- 打印出当前的searchpaths
function UpdateController:logSearchPath(flag)
    print(string.format("===============SearchPaths %s===============", flag or ""))
    local searchPaths = cc.FileUtils:getInstance():getSearchPaths()
    for i,v in ipairs(searchPaths) do
        print(v)
    end
    print("===============SearchPaths===============")
end

function UpdateController:onUpdateGameVersion(ver)
	GAME_VERSION = ver	
	
	self.updateScene:onUpdateGameVersion()
end

return UpdateController
