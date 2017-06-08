--load module
--create by kylin @ 2017-5-15

math.randomseed(os.time())

require "pbc.protobuf"
cc.exports.parser = require "pbc.parser"
cc.exports.pb = require "protobuf"

require "Message.Message"
require	"Definition.ConnectDef"
require	"Definition.Constants"

--Managers init
cc.exports.sNetworkManager = require("Manager.NetworkManager").create()
cc.exports.sLayerManager = require("Manager.LayerManager").create()