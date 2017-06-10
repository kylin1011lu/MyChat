--LayerManager
--create by kylin @ 2017-5-22

local LayerManager = class("LayerManager")

LayerManager._allLayers =
{
	GameStartLayer 	=	{Path = "GameUI.GameStartLayer"	,Tag = 1,Zorder = 1},
	ContentLayer 	= 	{Path = "GameUI.ContentLayer"	,Tag = 2,Zorder = 2},
	InputLayer	 	= 	{Path = "GameUI.InputLayer"		,Tag = 3,Zorder = 3},
}

function LayerManager:ctor()
   if self.init then self:init() end
end

function LayerManager:init()
	printInfo("LayerManager:init")

end

function LayerManager:createLayer(name,...)
	local layerInfo = self._allLayers[name]
	if nil == layerInfo then
		printError("LayerManager:createLayer not find layer:"..name)
	end 

    local layer = HotRequire(layerInfo.Path).create()
    if layer then
    	layer:setTag(layerInfo.Tag)
    	layer:setLocalZOrder(layerInfo.Zorder)
    else
    	printError("LayerManager:createLayer error")
    end

    return layer
end

function LayerManager:getLayerByName(name)
    local layerInfo = self._allLayers[name]
    if nil == layerInfo then
        printError("LayerManager:getLayerByName not find layer:"..name)
    end 
    local scene=cc.Director:getInstance():getRunningScene()
    return scene:getChildByTag(layerInfo.Tag)
end

return LayerManager