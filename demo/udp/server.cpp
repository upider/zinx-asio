#include "router.hpp"
#include "udp_message.hpp"
#include "datagram_server.hpp"
#include "datagram.hpp"

//PingRouter 使用ping 测试路由
class PingRouter: virtual public zinx_asio::DatagramRouter {
    public:
        PingRouter() {}
        virtual ~PingRouter() {}

    public:
        //Handle ping
        void handle(std::shared_ptr<zinx_asio::Datagram> dataGram,
                    std::shared_ptr<zinx_asio::IMessage> msg) {
            //得到ByteBuffer
            auto& data = msg->bufferRef();
            //使用toString会保留request中的消息内容
            std::cout << "Receive from " << dataGram->getEndPoint()
                      << " " << data.size() << " bytes"
                      << " Message is " << "\"" << data.toString() << "\""
                      << '\n';
            dataGram->sendMsg("OK this is server", 17);
        }

        void preHandle(std::shared_ptr<zinx_asio::Datagram> dataGram,
                       std::shared_ptr<zinx_asio::IMessage> msg) {}
        void postHandle(std::shared_ptr<zinx_asio::Datagram> dataGram,
                        std::shared_ptr<zinx_asio::IMessage> msg) {}
};

int main(void)
{
    zinx_asio::DatagramServer server;
    RegisterRouterClass(0, PingRouter);
    server.addRouter(0, "PingRouter");
    server.addEndPoint("127.0.0.1", 8888);

    server.serve();

    return 0;
}
