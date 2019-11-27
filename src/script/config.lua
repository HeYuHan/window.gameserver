require("client")
local config = gConfig()
--配置pc版本路径后，服务器会自动拉起pc版本进程，并且查杀已运行的进程，建议配置
--路径为绝对路径或者和服务器同级目录的相对路径


config.ClientPath = 'server\\MAXUS_ARMultiplayerCarGame2.exe'


config.GameTime = ClientConfig.Server.GameTime
config.GameBalanceTime=ClientConfig.Server.GameBalanceTime
config.GameBalanceTimeForPad=ClientConfig.Server.GameBalanceTimeForPad
config.GameShowTime = ClientConfig.Server.GameShowTime
config.ClientArg = ClientConfig.Server.ClientArg
config.KeepAlive = ClientConfig.Server.KeepAlive
local client_config = {
    Lobby=ClientConfig.Lobby,Ingame=ClientConfig.Ingame
}
config.ClientConfig=cjson.encode(client_config)



--设置道路检测点
local config_path = AutoPath('roadchecker.json')
log_info(config_path)
if config_path ~= nil then
    local f = assert(io.open(config_path, 'r'))
    if f then
        local str = f:read('*all')
        f:close()
        local check_data = cjson.decode(str)
        local array = check_data.data
        local len = #array
        for i = 1, len do
            local data = array[i]
            config:AddCheckPoint(data.index, data.radius, data.pos.x, data.pos.y, data.pos.z, i == len)
        end
    end
end

--设置道路出生点
config_path = AutoPath('brith.json')
log_info(config_path)
if config_path ~= nil then
    local f = assert(io.open(config_path, 'r'))
    if f then
        local str = f:read('*all')
        f:close()
        local check_data = cjson.decode(str)
        local array = check_data.data
        local len = #array
        for i = 1, len do
            local data = array[i]
            config:AddBrithPose(data.pos.x, data.pos.y, data.pos.z, data.rotation.x, data.rotation.y, data.rotation.z)
        end
    end
end

--设置金币点
config_path = AutoPath('drop_coin.json')
log_info(config_path)
if config_path ~= nil then
    local f = assert(io.open(config_path, 'r'))
    if f then
        local str = f:read('*all')
        f:close()
        local check_data = cjson.decode(str)
        local array = check_data.data
        local len = #array
        print('drop coin count:' .. len)
        local drop = config:AddDropItem()
        drop.mType = 1
        drop.mMaxCount = 0
        drop.mInitCount = len
        drop.mCheckRange = 2
        for i = 1, len do
            local data = array[i]
            config:AddDropItemPosition(1,1,data.pos.x, data.pos.y, data.pos.z)
        end
    end
end

--设置障碍物点
config_path = AutoPath('drop_obstacle.json')
log_info(config_path)
if config_path ~= nil then
    local f = assert(io.open(config_path, 'r'))
    if f then
        local str = f:read('*all')
        f:close()
        local check_data = cjson.decode(str)
        local array = check_data.data
        local len = #array
        local drop = config:AddDropItem()
        drop.mType = 3
        drop.mMaxCount = 0
        drop.mInitCount = len
        drop.mCheckRange = 10
        for i = 1, len do
            local data = array[i]
            config:AddDropItemPosition(3,data.index,data.pos.x, data.pos.y, data.pos.z)
        end
    end
end