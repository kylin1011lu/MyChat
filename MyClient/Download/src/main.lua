
cc.FileUtils:getInstance():setPopupNotify(false)

require "config"
require "Framework.HotRequire"
require "cocos.init"

cc.exports.DIRECTOR         = cc.Director:getInstance()
cc.exports.WINSIZE          = cc.Director:getInstance():getWinSize()
cc.exports.USERDEFAULT      = cc.UserDefault:getInstance()
cc.exports.TARGETPLATFORM   = cc.Application:getInstance():getTargetPlatform()

local function main()
    HotRequire("Manager.init")

    print("========DownloadMain")

    parser.register("common.proto","src/pbc")

    local startLayer = sLayerManager:createLayer("GameStartLayer")

    local scene = cc.Scene:create()
    scene:addChild(startLayer)
    if DIRECTOR:getRunningScene() then
        DIRECTOR:replaceScene(scene)
    else
        DIRECTOR:runWithScene(scene)
    end

end

local status, msg = xpcall(main, __G__TRACKBACK__)
if not status then
    print(msg)
end
