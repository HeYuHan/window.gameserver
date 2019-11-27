ClientConfig={
    Lobby={
        --true,测试环境
        --false,正式环境
        IsTest=false,
        --后台token
        Token='maxus',
        --其他接口的URL前缀
        Url = "https://c2b.saicmaxus.com/",
        --微信授权登陆的URL地址
        WXUrl = "https://wmt.maxuscloud.com/wmt/secure/grant/second?openid=gh_fbfcaabc1843&type=1&url=https://c2b.saicmaxus.com/g50invite/weixin/callBack?deviceNumber="
    },
    Ingame={
        --相机高度
        m_CameraFollowHeight=2.27,
        --相机距离
        m_CameraFollowDistance=5.76,
        --车辆侧面截图的图片大小
        m_CaptureSize = 888,

        --最大车速
        m_CarMaxSpeed = 100,
        --轮胎摩擦力
        m_CarWheelsFriction = 2.72,
        --最大转向
        m_CarMaxSteerAngle = 60,
        --最大驱动力
        m_CarMaxDirveForce = 8000
    },
    Server={
        --游戏时间
        GameTime=1000,
        --大屏结算返回时间
        GameBalanceTime=90,
        --Pad结算返回时间
        GameBalanceTimeForPad=90,
        --镜头展示时间
        GameShowTime = 7.5,
        --PC全屏
        --ClientArg = '-MaxScreen'
        ClientArg = '',
        --心跳
        KeepAlive = 10
    }
}