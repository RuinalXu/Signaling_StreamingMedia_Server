# 第4-9个视频
# 1 初始化sip信令协议栈并设置信令服务端口
内存池与内存池工厂

endpoint
- 消息的接收和响应


- 在继承过程中，基类定义了一个虚函数，想利用多态来实现不同的事件处理，那么需要将基类的析构函数也设置为`virtual`，这样基类才会产生虚表，一层一层释放内存空间，如果基类析构函数函数标识为`virtual`，那么在使用`delete`释放用子类对象实例化基类（多态的创建）所创建的对象时，基类分配的动态创建的内存才会被释放，避免内存泄漏。
- 并且释放是自下往基类开始释放；

# 2 上级注册管理
在上级服务服务中使用多态搭建任务事件处理框架，利用多态来实现不同的注册业务逻辑
- 鉴权注册：
    - 创建txdata，获取响应地址，发送响应，这三步就是上级对非鉴权注册请求的具体业务处理和响应；
- 非鉴权注册
- 下级注册与注销请求
    - 当下级发送的Message Header中的`Expires`字段大于0时，则为下级向上级发送的注册请求；为0时，则为下级向上级发送的注销请求；
    - 而且这个“判断下级是注册还是注销”需要在“发送响应”之后判断，先判断响应码是否为200，再判断`Expires`字段；
        - 如果是注册，那么就向记录当前注册时间t_0，并将Message Header中的`Register`字段修改为true；
            - 获取时间的目的是为了使用当前的时间与注册时的时间t_0作差，与`Expires`比较，得到注册信息是否过时；
        - 如果是注销，那么只需要将注册时记录的时间t_0设置为0，并将Message Header中的`Register`字段修改为false；
有效期定时检查：利用

# 下级信令服务器注册

鉴权的请求信息WWW-Authenticate，认证字段（类似于HTTP响应头），构成：<加密认证方案> <参数1>=<值1> ...
- Authenticate Scheme：加密认证方案
    - 常用的加密认证方案：basic/digest/MD5/sha265/sha384/sha512
- Realm
- Nonce Value：
- Opaque Value：
- Algorithm：  


封装请求体

# 下级心跳包发送
**保活信令**在上下级之间的用途，制定下级报活的策略并实现；
- 下级向上级发送的保活信令为XML数据，
```xml
<?xml version="1.0"?>\n
<Notify>\r\n
    <CmdType>Keepalive</CmdType>\r\n
    <SN>2749</SN>\r\n
    <DeviceID></DeviceID>\r\n
    <Status>OK</Status>\r\n
</Notify>\r\n
```

下级向上级发送保活信令（实际为封装的XML数据），PJSIP库中的函数``通过回调函数得到上级发送给下级的状态码，然后在自定义的回调函数中，判断该状态码否为200，如果不是200，那么就将上级服务事件的注册状态设置为false；

# 上级处理心跳包的业务逻辑

# 视频目录请求和推送



# SIP注册的一个代码逻辑
    // 将封装SIP消息进行分装，转化为pjsip格式的字符串pj_str_t，通过pj_str()函数，
    pj_str_t
    pj_str();


    // 定义SIP客户端注册的结构体变量；
    pjsip_regc* regc;
    // 初始化SIP客户端注册的结构体变量
    pjsip_regc_create();

    // pjsip_regc_cb()回调函数，是一个响应事件触发的一个接口

    // 利用regc结构体来生成SIP注册请求的消息体
    // 需要发送的数据的结构体定义
    pjsip_tx_data* tdata = NULL;
    // 再做封装
    pjsip_regc_register();

    // 发送请求注册消息
    pjsip_regc_send();

    // pjsip_regc_send()函数中调用了以下函数，该函数中regc_tsx_callback的回调函数需要重点关注，该回调函数在消息发送成功是才会触发回调，
    status = pjsip_endpt_send_request(regc->endpt, tdata, REGC_TSX_TIMEOUT,
                                      regc, &regc_tsx_callback);
                                      
    // 如果已经注册成功，需要在regc_tsx_callback指向的回调函数中设置下级结构体的注册状态

## 下级在使用定时器开启定时器线程时，所创建的线程没有向pjlib注册，因此出现断言
```shell
EdgeService: ../src/pj/os_core_unix.c:857: pj_thread_this: Assertion `!"Calling pjlib from unknown/external thread. You must " "register external threads with pj_thread_register() " "before calling any pjlib functions."' failed.
Aborted (core dumped)
```

# 心跳包的业务逻辑
**心跳包只会在下级注册成功后发送**，会探测上级的服务器运行是否是正常的，上级收到心跳包后，有两种处理逻辑：
- 只是收到下级的心跳包，证明下级的运行状态是正常的就行，不需要其他处理逻辑；
- 还有一种逻辑是：收到下级的心跳包后，将下级的注册时间（）进行更新

上级需要在`pj_bool_t onRxRequest(pjsip_rx_data *rdata)`判断下级的方法是**REGISTER**还是**MESSAGE**，如果是**REGISTER**，那么就处理下级的注册业务，如果是**MESSAGE**，判断XML文件的**body**部分


# 视频目录树请求与推送


# 11月26日遇到的问题
GlobalCtl::getAuth(parseFromId(msg))在线程中被阻塞了，线程池中线程一直在获取锁，没有释放锁，原因，`AutoMutexLock`自定义自动锁类中的析构函数函数中没有调用释放锁函数；