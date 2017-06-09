local msg_handler_table = {}
local proto_msg_table = {}

cc.exports.msgParser = function(cmd, msgstr)

    if msg_handler_table[cmd] == nil then
        printError("unhandled message:"..cmd)
        return
    end

    local decode = pb.decode(proto_msg_table[cmd],msgstr)
    if decode == nil then
        printError(cmd.."message decode nil")
        return
    end
    printInfo("receive message:%d", cmd) 
    local eventDispatcher=cc.Director:getInstance():getRunningScene():getEventDispatcher()
    local event = cc.EventCustom:new("netMessage")
    event.data = decode
    event.func = msg_handler_table[cmd]
    eventDispatcher:dispatchEvent(event)

    -- printInfo("============msgParser:"..cmd.. " :"..msgstr)
    -- if cmd == 103 then 

    --     local decode = pb.decode("message.UserLoginResponse",msgstr)
    --     for k,v in pairs(decode) do
    --         print(k,v)
    --     end
    --     printInfo(decode.login_name .. "  "..decode.ret_code.."   "..decode.userid)

    --     --保存userid
    --     USERDEFAULT:setIntegerForKey(KEY_USERID,decode.userid)

    --     local contentLayer = sLayerManager:createLayer("ContentLayer")
    --     local inputLayer = sLayerManager:createLayer("InputLayer")

    --     local scene = director:getRunningScene()
    --     scene:addChild(contentLayer)
    --     scene:addChild(inputLayer)

    -- elseif cmd == 105 then
    --     local decode = pb.decode("message.ChatResponse",msgstr)
    --     printInfo("msgParser:"..decode.chat_content)

    --     local layer = sLayerManager:getLayerByName("ContentLayer")
    --     layer:addOneMessage(decode.chat_content)
    -- end
end

msg_handler_table = 
{
        --账号注册
        [101]      =        function(self,data) if self.doAcctRegister then  self:doAcctRegister(data) end end,
        --账号登录
        [103]      =        function(self,data) if self.doUserLogin then  self:doUserLogin(data) end end,
        --聊天
        [105]      =        function(self,data) if self.doChatResponse then  self:doChatResponse(data) end end,
        --聊天历史
        [107]      =        function(self,data) if self.doChatHistoryResponse then  self:doChatHistoryResponse(data) end end,
}

proto_msg_table =
{
    [101] = "message.AccountRegisterResponse",
    [103] = "message.UserLoginResponse",
    [105] = "message.ChatResponse",
    [107] = "message.ChatHistoryResponse",
}

function cc.exports.notifyUIUpdate(funcName,data)
    
    local eventDispatcher=DIRECTOR:getRunningScene():getEventDispatcher()
    local event = cc.EventCustom:new("netMessage")
    event.data=data
    event.func=function(self,data) if self[funcName] then self[funcName](self,data) end end 
    eventDispatcher:dispatchEvent(event)
end

function cc.exports.onConnected()
    notifyUIUpdate("onConnected")
end

function cc.exports.sendMsg(cmd,code)
    cci.sendmsg(cmd,code,string.len(code))
end