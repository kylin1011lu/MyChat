--load module
--create by kylin @ 2017-5-15

require "pbc.protobuf"
cc.exports.parser = require "pbc.parser"
cc.exports.pb = require "protobuf"

HotRequire("Message.Message")
HotRequire("Definition.ConnectDef")
HotRequire("Definition.Constants")

--Managers init
cc.exports.sNetworkManager = HotRequire("Manager.NetworkManager").create()
cc.exports.sLayerManager = HotRequire("Manager.LayerManager").create()
cc.exports.sUpdateManager = HotRequire("Manager.UpdateManager").create()