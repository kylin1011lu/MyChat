--游戏启动界面
--create by kylin @ 2017-5-15

local GameStartLayer = class("GameStartLayer", require("GameUI.BaseLayer"))

function GameStartLayer:onCreate()
	--checkupdate
	
	--connect server
	sNetworkManager.connectServer()

	-- local layer = require("GameUI.ContentLayer")

	-- local str = "字重粗细不均、缺字是家常便饭——可\n字体破线、造形太烂而不能放大使用；字重粗细不均、缺字是家常便饭——可是再烂的字体设计师还得要买，"

	-- local ttfConfig = {}
 --    ttfConfig.fontFilePath = "fonts/SourceHanSansK-Normal.ttf"
 --    ttfConfig.fontSize     = 18
 --    local label2 = cc.Label:createWithTTF(ttfConfig, str, cc.TEXT_ALIGNMENT_LEFT)
 --    -- self:addChild(label2)
 --    label2:setTextColor( cc.c4b(0, 0, 0, 255))
 --    label2:setAnchorPoint(cc.p(0.0, 0.0))
 --    label2:setPosition(cc.p(30, 30))

 --    local size = label2:getContentSize()
 --    label2:setContentSize({width=486,height=54})
 --    label2:setTTFConfig(ttfConfig)
	-- print("4width:"..size.width.."  height:"..size.height)	

	-- local w = bubble_config.capMargin.left + bubble_config.capMargin.right + size.width;
	-- local h = bubble_config.capMargin.top + bubble_config.capMargin.down + size.height

	--     local Image_1 = ccui.ImageView:create()
	-- 	Image_1:ignoreContentAdaptWithSize(false)
	-- 	Image_1:loadTexture("Images/chat_bubble_beach.png",0)
	-- 	Image_1:setScale9Enabled(true)
	-- 	Image_1:setCapInsets(bubble_config.capInsets) 
	-- 	Image_1:setLayoutComponentEnabled(true)
	-- 	Image_1:setName("Image_1")
	-- 	Image_1:setTag(7)
	-- 	Image_1:setCascadeColorEnabled(true) 
	-- 	Image_1:setCascadeOpacityEnabled(true)
	-- 	Image_1:setPosition(cc.p(0, 90))
	-- 	Image_1:setAnchorPoint(cc.p(0,0))
	-- 	Image_1:setContentSize({width=w,height=h})

	-- 	Image_1:addChild(label2)
 --        self:addChild(Image_1)


end

local input_name = default_user_name()
local input_password = "123456"

function GameStartLayer:onConnected()

	printInfo("GameStartLayer:onConnected")
	local lastUserName = USERDEFAULT:getStringForKey(KEY_NAME)
	local lastUserPwd = USERDEFAULT:getStringForKey(KEY_PASSWORD)
	local userid = USERDEFAULT:getIntegerForKey(KEY_USERID,0)

	if userid <= 0 or lastUserName == "" or lastUserPwd == "" then
	    local request =
	    {
	        register_name = input_name,
	        register_password = input_password,
	    }

	    local code = pb.encode("message.AccountRegisterRequest",request)
	    sendMsg(100,code);
	else
	    local request =
	    {
	        login_name = lastUserName,
	        login_password = lastUserPwd,
	    }

	    local code = pb.encode("message.UserLoginRequest",request)
	    sendMsg(102,code);
	end;
end

--103协议返回
function GameStartLayer:doUserLogin(data)
    printInfo("[GameStartLayer:doUserLogin]"..data.ret_code.."   "..data.userid)

    if data.ret_code == "CODE_SUCCESS" then
    	USERDEFAULT:setIntegerForKey(KEY_USERID,data.userid)
	    local contentLayer = sLayerManager:createLayer("ContentLayer")
	    local inputLayer = sLayerManager:createLayer("InputLayer")

	    local scene = DIRECTOR:getRunningScene()
	    scene:addChild(contentLayer)
	    scene:addChild(inputLayer)
    end;
end

--101协议返回
function GameStartLayer:doAcctRegister(data)
    printInfo("[GameStartLayer:doAcctRegister]"..data.ret_code.. "   "..data.userid)
    if data.ret_code == "CODE_SUCCESS" then
    	USERDEFAULT:setStringForKey(KEY_NAME,input_name)
		USERDEFAULT:setStringForKey(KEY_PASSWORD,input_password)
		USERDEFAULT:setIntegerForKey(KEY_USERID,data.userid)
		local request =
	    {
	        login_name = input_name,
	        login_password = input_password,
	    }

	    local code = pb.encode("message.UserLoginRequest",request)
	    sendMsg(102,code);
	end
end

return GameStartLayer