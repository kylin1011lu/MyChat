--基础层
--create by kylin @ 2017-5-24

local BaseLayer = class("BaseLayer", cc.Layer)

function BaseLayer:ctor()
	if self.init then self:init() end
	if self.onCreate then self:onCreate() end
end

function BaseLayer:init()
	self:setName(self.__cname)
    local function netMessage(event)
        if event.func then 
            event.func(self,event.data)
        end
    end
    printInfo("BaseLayer:init :"..self.__cname)
    self.netListener= cc.EventListenerCustom:create("netMessage",netMessage)
    self:getEventDispatcher():addEventListenerWithSceneGraphPriority(self.netListener, self)


    local function onNodeEvent(event)
        if event == "enter" then
            if self.onEnter then self:onEnter() end
        elseif event == "exit" then
            if self.onExit then self:onExit() end
        end
    end
    self:registerScriptHandler(onNodeEvent)
end

return BaseLayer