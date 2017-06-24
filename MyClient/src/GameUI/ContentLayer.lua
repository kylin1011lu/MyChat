--聊天展示
--create by kylin @ 2017-5-15

local ContentLayer = class("ContentLayer", require("GameUI.BaseLayer"))
local winSize = cc.Director:getInstance():getWinSize()

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

    --self.tableView:setVisible(false)
end

function ContentLayer:onEnter()
    sMsgManager:loadMsgFromCache()
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
    return sMsgManager:getCellSize(idx)
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
   	return sMsgManager:getCellNumber()
end

function ContentLayer:RefreshTableView()
	self.tableView:reloadData()
	local size = self.tableView:getContentSize()
	if size.height > self.viewSize.height then
		self.tableView:setContentOffset(cc.p(0,0))
		-- self.tableView:setContentOffsetInDuration(cc.p(0,0),0.4)
		-- self.tableView:runAction(cc.Sequence:create(cc.DelayTime:create(0.4),cc.CallFunc:create(function()
		-- 	self.tableView:setVisible(true)
		-- end)))
	end

end

function ContentLayer:createCell(cell,idx,isNew)

	local onemsg = sMsgManager:getMsgByIndex(idx+1)
	if not onemsg then
		return nil
	end

	local maxwidth = 0
	if onemsg.isLonger then
		maxwidth = MAX_MESSAGE_WIDTH
	end

	local w,h = sMsgManager:getCellSize(idx)
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

-- function ContentLayer:doChatHistoryResponse(data)
--  	sMsgManager:ParseMessage(data)
-- end

function ContentLayer:doChatResponse(data)
 	sMsgManager:ParseMessage(data)
end

function ContentLayer:onExit()
	sMsgManager:writeMsgToCache()
end

return ContentLayer
