--MsgManager
--create by kylin @ 2017-6-22

local MsgManager = class("MsgManager")

local caculateLabel = nil --用于计算的消息长宽

local bubble_config =
{
	capInsets = {x = 44, y = 32, width = 2, height = 9},
	capMargin = {left = 37,right = 35,top = 28,down = 32},
	imgSize = {width=116,height=102},
}

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
	if content ==nil or content =="" then
		self:RequestChatHistory()
		return
	end

	local data = pb.decode("message.MsgCacheInfo",content)
    if data == nil then
        printError("MsgManager:loadMsgFromCache() ->cache decode nil")
        self:RequestChatHistory()
        return
    end

    self:ParseMessage(data)

end

function MsgManager:writeMsgToCache()
	if not self.all_msg or #self.all_msg ==0 then
		printInfo("MsgManager:writeMsgToCache: all_msg empty")
		return
	end 

	local msgs = {}

	for i,v in ipairs(self.all_msg) do
		local msg = {}
		msg.chat_content = v.content
		msg.send_userid = v.send_userid
		msg.send_time = v.send_time

		table.insert(msgs,msg)
	end

	local allmsg = {}
	allmsg["message"] = msgs

   	local code = pb.encode("message.MsgCacheInfo",allmsg)
	cc.FileUtils:getInstance():writeStringToFile(code,self.msgCachePath)	
end

function MsgManager:ParseMessage(data)
	local parseMsgs = {}
	local userid = USERDEFAULT:getIntegerForKey(KEY_USERID,0)
    local msgs = data["message"]
    if nil ~= msgs and #msgs > 0 then
    	for i=1,#msgs do
    		local msg = msgs[i]
    		local msgsize,offseth,offsetw,isLonger = self:caculateMessageSize(msg.chat_content)
			local onemsg = 
			{
				content = msg.chat_content,
				size = msgsize,
				send_userid = msg.send_userid,
				isself = (msg.send_userid == userid),
				offset = {w = offsetw,h=offseth},
				isLonger = isLonger,
				send_time = msg.send_time,
				isread = true
			}
			printInfo(msg.send_userid.. " "..os.date("%Y-%m-%d %H:%M:%S",msg.send_time).. " "..msg.chat_content)
			table.insert(parseMsgs,onemsg)
    	end
    end

    if #parseMsgs == 1 then
    	table.insert(self.all_msg,1,parseMsgs[1])
    elseif #parseMsgs > 1 then
    	self.all_msg = nil
    	self.all_msg = parseMsgs
    else
    	return
    end

    table.sort(self.all_msg,function ( a,b )
		return a.send_time > b.send_time
	end)
	sLayerManager:getLayerByName("ContentLayer"):RefreshTableView()
    --notifyUIUpdate("RefreshTableView")
end

function MsgManager:caculateMessageSize(str)

	if caculateLabel == nil then
    	caculateLabel = cc.Label:createWithTTF(DEFAULT_TTFCONFIG, str, cc.TEXT_ALIGNMENT_LEFT)
    	caculateLabel:retain()
    end
    caculateLabel:setString(str)
    local size = caculateLabel:getContentSize()
    local isLonger = false
    if size.width > MAX_MESSAGE_WIDTH then
    	isLonger = true
    	caculateLabel:setMaxLineWidth(MAX_MESSAGE_WIDTH)
    	size = caculateLabel:getContentSize()
    end

    local offseth = 0
    local totoalh = size.height + bubble_config.capMargin.top + bubble_config.capMargin.down
    if totoalh < bubble_config.imgSize.height then
    	offseth = bubble_config.imgSize.height - totoalh
    end

    local offsetw = 0
    local totoalw = size.width + bubble_config.capMargin.left + bubble_config.capMargin.right
    if totoalw < bubble_config.imgSize.width then
    	offsetw = bubble_config.imgSize.width - totoalw
    end

    size.height = size.height + offseth
    size.width = size.width + offsetw

	--printInfo(str.."width:"..size.width.."  height:"..size.height.."   offseth:"..offseth.."   offsetw:"..offsetw)
	return size,offseth,offsetw,isLonger
end

function MsgManager:getCellSize(idx)
	local onemsg = self.all_msg[idx+1]
	if onemsg then

		local w = bubble_config.capMargin.left + bubble_config.capMargin.right + onemsg.size.width;
		local h = bubble_config.capMargin.top + bubble_config.capMargin.down + onemsg.size.height

		return w,h
	end

	return 0,0
end

function MsgManager:getCellNumber()
	return #self.all_msg
end

function MsgManager:getMsgByIndex(idx)
	return self.all_msg[idx]
end

---------------------------

function MsgManager:RequestChatHistory()
    local request =
    {
        last_time = os.time(),
    }

    local code = pb.encode("message.ChatHistoryRequest",request)
    sendMsg(106,code)
end




return MsgManager