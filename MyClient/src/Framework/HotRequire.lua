--hot require table
--create by kylin @ 2017-6-10

local reloadLuaTable = {}

function HotRequire(file_name)
    if file_name == nil then
        print(debug.traceback())
    end
    package.loaded[file_name] = nil
    reloadLuaTable[file_name] = true
    return require(file_name)
end

function ReloadAllLuaFile()
    for file_name,_ in pairs(reloadLuaTable) do
        package.loaded[file_name] = nil
        reloadLuaTable[file_name] = true
        require(file_name)
    end
end

function UnloadAllLuaFile()
    for file_name,_ in pairs(reloadLuaTable) do
        package.loaded[file_name] = nil
        table.remove(reloadLuaTable, file_name)
    end
end

function ReloadSingleLuaFile(file_name)
    package.loaded[file_name] = nil
    return require(file_name)
end

--热更新目录
HOT_REQUIRE_PATH = cc.FileUtils:getInstance():getWritablePath() .. "/Download"

if not cc.FileUtils:getInstance():isDirectoryExist(HOT_REQUIRE_PATH) then
    cc.FileUtils:getInstance():createDirectory(HOT_REQUIRE_PATH)
end

cc.FileUtils:getInstance():addSearchPath(HOT_REQUIRE_PATH,true)
cc.FileUtils:getInstance():addSearchPath(HOT_REQUIRE_PATH.."/res",true)
cc.FileUtils:getInstance():addSearchPath(HOT_REQUIRE_PATH.."/src",true)