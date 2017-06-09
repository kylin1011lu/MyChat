--输入
--create by kylin @ 2017-5-19

local InputLayer = class("InputLayer", require("GameUI.BaseLayer"))

function InputLayer:onCreate()

	local visibleOrigin = cc.Director:getInstance():getVisibleOrigin()
    local visibleSize = cc.Director:getInstance():getVisibleSize()
    local winsize = cc.Director:getInstance():getWinSize()
    local framesize = cc.Director:getInstance():getOpenGLView():getFrameSize()


    local editBoxSize = cc.size(visibleSize.width - 100, 42*2)
    local inputEdit = cc.EditBox:create(editBoxSize, cc.Scale9Sprite:create("extensions/green_edit.png"))
    inputEdit:setPosition(cc.p(visibleOrigin.x+editBoxSize.width/2, editBoxSize.height/2))

	inputEdit:setFontName("fonts/NotoSansHans-Regular.otf")
    inputEdit:setFontSize(26)
    -- inputEdit:setFontColor(cc.c3b(255,0,0))
    -- inputEdit:setPlaceholderFont("fonts/NotoSansHans-Regular.otf",26)
    -- inputEdit:setPlaceHolder("")
    -- inputEdit:setPlaceholderFontColor(cc.c3b(255,255,255))
    inputEdit:setReturnType(cc.KEYBOARD_RETURNTYPE_DONE)
	--Handler
    self:addChild(inputEdit)

    local sendBtn = ccui.Button:create("extensions/button.png", "extensions/buttonHighlighted.png", "", ccui.TextureResType.localType)
    sendBtn:setAnchorPoint(cc.p(0,0))
    sendBtn:ignoreContentAdaptWithSize(false)
    sendBtn:setScale9Enabled(true)
    sendBtn:setCapInsets({x = 5, y = 10, width = 5, height = 11})
    sendBtn:setContentSize({width = 100.0000, height = 42*2})
    sendBtn:setPosition(cc.p(editBoxSize.width,0))
    sendBtn:setTitleFontName("fonts/NotoSansHans-Regular.otf")
    sendBtn:setTitleFontSize(26)
    sendBtn:setTitleText("发送")
    sendBtn:addClickEventListener(function(sender)
        local str = inputEdit:getText()
        if str == "" then
            printInfo("Please input message")
            return
        end

        printInfo(str)

        inputEdit:setText("")
        local request =
        {
            chat_content = str,
        }

        local code = pb.encode("message.ChatRequest",request)
        sendMsg(104,code)

    end)
    self:addChild(sendBtn)
end


return InputLayer