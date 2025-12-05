#include "SipGbPlay.h"


/**
 *  将下级流媒体服务器直接作为
 *  流媒体服务器当作直接对接摄像头的服务器，该服务器可以直接从设备直接采集视频数据或者是音频数据，
 *  而且可以为每个通道的音频数据和视频数据进行自定义头部的标记，标记的业务可以根据具体的业务来定。
 */
int SipGbPlay::recvFrame() {
    // config目录中的stream.file文件当作下级从流媒体服务器拉来的流，那么就需要对该文件的每一帧进行header的解析，再解析出裸数据
    
    // 实现音视屏PS封装与发送

    // 视频关键帧: ps-header|ps system header|ps system map|pes header|h264 raw data
    // 视频非关键帧: ps-header|pes header|h264 raw data
    // 安防领域无B帧

    // 音频: pes header|aac raw data

    // 将封装的好的PS流交给jrtplib库进行RTP的封装与推送
    // sendPsRtpStream()

    // RTP负载的数据定位13000字节左右，后期还需要加上RTP包头，传输层协议包头，网络层IP协议包头，以及链路层以太网包头，所以在RTP端进行推送的时候，
    // 整个PS包算法的拆解，
    // 而且拆解发送的大小要小于网络层传输的mtu的最大值
    // 对视频和音频数据需要实现整包发送和分批发送算法


    // 下级推流，上级收流出现丢包的情况怎么解决的？
    

}