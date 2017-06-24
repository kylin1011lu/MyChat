--MsgManager
--create by kylin @ 2017-6-22

local MsgManager = class("MsgManager")
function MsgManager:ctor()
   if self.init then self:init() end
end

function MsgManager:init()
	printInfo("MsgManager:init")

	local writePath = cc.FileUtils:getInstance():getWritablePath()
	self.msgCachePath = writePath..MSG_CACHE_FILE

	self.all_msg = {}

	--Test
	-- self.all_msg =
	-- {
	-- 	["message"] = {{chat_content="测试1",send_userid=1234,send_time=1213445},
	-- 				{chat_content="测试2",send_userid=1234,send_time=1213445},
	-- 				{chat_content="测试3",send_userid=1234,send_time=1213445},
	-- 				{chat_content="测试4",send_userid=1234,send_time=1213445},
	-- 				}
	-- }

end

-- message MessageInfo
-- {
--  required bytes chat_content=3; //聊天内容
-- 	required uint32 send_userid=4; //发送者id
-- 	required uint32 send_time=5;   //发送时间
-- }

function MsgManager:loadMsgFromCache()

	local content = cc.FileUtils:getInstance():getStringFromFile(self.msgCachePath)
end

function MsgManager:writeMsgToCache(all_msg)
	-- if not self.all_msg or #self.all_msg ==0 then
	-- 	printInfo("MsgManager:writeMsgToCache: all_msg empty")
	-- 	return
	-- end 

	local msgs = {}

	for i=1,4 do
		table.insert(msgs,{chat_content="测试"..i,send_userid=1234,send_time=1213445})
	end

	local allmsg = {}
	allmsg["message"] = msgs

   	local code = pb.encode("message.MsgCacheInfo",allmsg)
	cc.FileUtils:getInstance():writeStringToFile(code,self.msgCachePath)	
end



return MsgManager