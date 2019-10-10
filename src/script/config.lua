local config = gConfig()
config.GameTime = 10
config.GameBalanceTime=10
config.GameBalanceTimeForPad=10
config.GameShowTime = 7.5





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