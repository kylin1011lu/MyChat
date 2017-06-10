--基础层
--create by kylin @ 2017-5-24

local BaseLayer = class("BaseLayer", cc.Layer)

function BaseLayer:ctor()
	if self.init then self:init() end
	if self.onCreate then self:onCreate() end
end

function BaseLayer:init()

    local function netMessage(event)
        if event.func then 
            event.func(self,event.data)
        end
    end
    self.netListener= cc.EventListenerCustom:create("netMessage",netMessage)
    self:getEventDispatcher():addEventListenerWithSceneGraphPriority(self.netListener, self)
end

return BaseLayer