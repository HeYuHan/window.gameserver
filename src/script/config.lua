local config = gConfig()
config.GameTime=60
config:AddBrithPose(0,0,0,1,1,1)
config:AddDropItemPosition(1,0,0,0)
local drop = config:AddDropItem()
drop.mType=1
drop.mMaxCount=0
drop.mInitCount=10
drop.mCheckRange=1