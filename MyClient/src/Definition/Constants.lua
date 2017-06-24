
cc.exports.KEY_USERID 			= "USERID"
cc.exports.KEY_NAME 			= "LOGIN_NAME"
cc.exports.KEY_PASSWORD 		= "LOGIN_PASSWORD"

cc.exports.MSG_CACHE_FILE		= "msg_cache_file.txt"

cc.exports.DIRECTOR         	= cc.Director:getInstance()
cc.exports.WINSIZE          	= cc.Director:getInstance():getWinSize()
cc.exports.USERDEFAULT      	= cc.UserDefault:getInstance()
cc.exports.TARGETPLATFORM   	= cc.Application:getInstance():getTargetPlatform()
cc.exports.MAX_MESSAGE_WIDTH 	= WINSIZE.width - 80



cc.exports.DEFAULT_TTFCONFIG = {}
DEFAULT_TTFCONFIG.fontFilePath = "fonts/NotoSansHans-Regular.otf"
DEFAULT_TTFCONFIG.fontSize     = 26