ClientConfig={
    Lobby={
        --true,测试环境
        --false,正式环境
        IsTest=true,
        --后台token
        Token='a',
        Url = 'https://c2bt4.maxuscloud.com/'
    },
    Ingame={
        --相机高度
        m_CameraFollowHeight=1.93,
        --相机距离
        m_CameraFollowDistance=5.47
    },
    Server={
        --游戏时间
        GameTime=300,
        --大屏结算返回时间
        GameBalanceTime=10,
        --Pad结算返回时间
        GameBalanceTimeForPad=10,
        --镜头展示时间
        GameShowTime = 7.5
    }
}