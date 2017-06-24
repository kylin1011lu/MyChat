--聊天展示
--create by kylin @ 2017-5-15

local ContentLayer = class("ContentLayer", require("GameUI.BaseLayer"))
local winSize = cc.Director:getInstance():getWinSize()

local all_msg = {}

local DEFAULT_TTFCONFIG = {}
DEFAULT_TTFCONFIG.fontFilePath = "fonts/NotoSansHans-Regular.otf"
DEFAULT_TTFCONFIG.fontSize     = 26

local MAX_MESSAGE_WIDTH = winSize.width - 80

local bubble_config =
{
	capInsets = {x = 44, y = 32, width = 2, height = 9},
	capMargin = {left = 37,right = 35,top = 28,down = 32},
	imgSize = {width=116,height=102},
}

local test_msgs = 
{
	"你在干嘛？",
	"在看电视",
	"在读书",
	"发送发送今天真的很高心啊啊很高心",
	"字体破线、造形太烂而不能放大使用；字重粗细不均、缺字是家常便饭——可是再烂的字体设计师还得要买，"
}

function ContentLayer:onCreate()	

	self.viewSize = {width = winSize.width,height = winSize.height-42*2}
    self.tableView = cc.TableView:create(self.viewSize)
    self.tableView:setDirection(cc.SCROLLVIEW_DIRECTION_VERTICAL)
    self.tableView:setAnchorPoint(cc.p(0,0))
    self.tableView:setPosition(cc.p(0, 42*2))
    self.tableView:setDelegate()
    self:addChild(self.tableView)
    -- registerScriptHandler functions must be before the reloadData funtion
    self.tableView:registerScriptHandler(self.numberOfCellsInTableView,cc.NUMBER_OF_CELLS_IN_TABLEVIEW)  
    self.tableView:registerScriptHandler(self.scrollViewDidScroll,cc.SCROLLVIEW_SCRIPT_SCROLL)
    self.tableView:registerScriptHandler(self.scrollViewDidZoom,cc.SCROLLVIEW_SCRIPT_ZOOM)
    self.tableView:registerScriptHandler(self.tableCellTouched,cc.TABLECELL_TOUCHED)
    self.tableView:registerScriptHandler(function(table,idx) return self:cellSizeForTable(table,idx) end,cc.TABLECELL_SIZE_FOR_INDEX)
    self.tableView:registerScriptHandler(function(table,idx) return self:tableCellAtIndex(table,idx) end,cc.TABLECELL_SIZE_AT_INDEX)
    self.tableView:reloadData()

    self:RequestChatHistory()
end

function ContentLayer.scrollViewDidScroll(view)
    -- printInfo("scrollViewDidScroll")
end

function ContentLayer.scrollViewDidZoom(view)
    -- printInfo("scrollViewDidZoom")
end

function ContentLayer.tableCellTouched(table,cell)
    -- printInfo("cell touched at index: " .. cell:getIdx())
end

function ContentLayer:cellSizeForTable(table,idx) 
    return self:getCellSize(idx)
end

function ContentLayer:tableCellAtIndex(table, idx)
	-- printInfo("tableCellAtIndex")
    local strValue = string.format("%d",idx)
    local cell = table:dequeueCell()
    local label = nil
    if nil == cell then
        cell = cc.TableViewCell:new()
 		self:createCell(cell,idx,true)
    else
    	self:createCell(cell,idx,false)
    end

    return cell
end

function ContentLayer.numberOfCellsInTableView(table)
	-- printInfo("numberOfCellsInTableView")
   	return #all_msg
end

function ContentLayer:RefreshTableView()

	table.sort(all_msg,function ( a,b )
		return a.send_time > b.send_time;
	end)

	self.tableView:reloadData()
	local size = self.tableView:getContentSize()
	if size.height > self.viewSize.height then
		self.tableView:setContentOffsetInDuration(cc.p(0,0),0.4)
	end

end

function ContentLayer:caculateMessageSize(str)

    local label = cc.Label:createWithTTF(DEFAULT_TTFCONFIG, str, cc.TEXT_ALIGNMENT_LEFT)
    local size = label:getContentSize()

    local isLonger = false
    if size.width > MAX_MESSAGE_WIDTH then
    	isLonger = true
    	label:setMaxLineWidth(MAX_MESSAGE_WIDTH)
    	size = label:getContentSize()
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


function ContentLayer:getCellSize(idx)
	local onemsg = all_msg[idx+1]
	if onemsg then

		local w = bubble_config.capMargin.left + bubble_config.capMargin.right + onemsg.size.width;
		local h = bubble_config.capMargin.top + bubble_config.capMargin.down + onemsg.size.height

		return w,h
	end

	return 0,0
end

function ContentLayer:createCell(cell,idx,isNew)

	local onemsg = all_msg[idx+1]
	if not onemsg then
		return nil
	end

	local maxwidth = 0
	if onemsg.isLonger then
		maxwidth = MAX_MESSAGE_WIDTH
	end

	local w,h = self:getCellSize(idx)
	local imagebg = nil
	if isNew then
		imagebg = ccui.ImageView:create()
		imagebg:ignoreContentAdaptWithSize(false)
		imagebg:loadTexture("Images/chat_bubble_beach.png",0)
		imagebg:setScale9Enabled(true)
		imagebg:setCapInsets(bubble_config.capInsets) 
		imagebg:setLayoutComponentEnabled(true)
		imagebg:setName("imagebg")
		imagebg:setTag(7)
		imagebg:setCascadeColorEnabled(true) 
		imagebg:setCascadeOpacityEnabled(true)
		imagebg:setAnchorPoint(cc.p(0,0))
		cell:addChild(imagebg)
	else
		imagebg = cell:getChildByTag(7)
	end
	if onemsg.isself then
		imagebg:setPosition(cc.p(winSize.width, 0))
	else
		imagebg:setPosition(cc.p(0, 0))
	end
	imagebg:setFlippedX(onemsg.isself)
	
	imagebg:setContentSize({width=w,height=h})

	local label = nil
	if isNew then
		label = cc.Label:createWithTTF(DEFAULT_TTFCONFIG, onemsg.content, cc.TEXT_ALIGNMENT_LEFT,maxwidth)
	    label:setTextColor(cc.c4b(0, 0, 0, 255))
	    label:setAnchorPoint(cc.p(0.0, 0.0))
	    label:setTag(123)
	    label:setAlignment(cc.TEXT_ALIGNMENT_LEFT,cc.VERTICAL_TEXT_ALIGNMENT_CENTER)
		cell:addChild(label)
	else
		label = cell:getChildByTag(123)
		label:setMaxLineWidth(maxwidth)
		label:setString(onemsg.content)
	end

	if not onemsg.isself then
    	label:setPosition(cc.p(bubble_config.capMargin.left+onemsg.offset.w/2, bubble_config.capMargin.down+onemsg.offset.h/2))
    else
		label:setPosition(cc.p(winSize.width-w + bubble_config.capMargin.right +onemsg.offset.w/2, bubble_config.capMargin.down+onemsg.offset.h/2))
    end

end


--------

function ContentLayer:RequestChatHistory()
    local request =
    {
        last_time = os.time(),
    }

    local code = pb.encode("message.ChatHistoryRequest",request)
    sendMsg(106,code)
end

function ContentLayer:doChatHistoryResponse(data)
 	self:ParseMessage(data)
 	self:RefreshTableView()
end

function ContentLayer:doChatResponse(data)

 	self:ParseMessage(data)
 	sMsgManager:writeMsgToCache(data)
 	self:RefreshTableView()
end

function ContentLayer:ParseMessage(data)
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
				isself = (msg.send_userid == userid),
				offset = {w = offsetw,h=offseth},
				isLonger = isLonger,
				send_time = msg.send_time,
				isread = true
			}
			printInfo(msg.send_userid.. " "..os.date("%Y-%m-%d %H:%M:%S",msg.send_time).. " "..msg.chat_content)
			table.insert(all_msg,1,onemsg)
    	end
    end
end

return ContentLayer
