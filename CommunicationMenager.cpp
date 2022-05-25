#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <optional>
#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <iostream>
#include <future>
#include <algorithm>

//g++ CommunicationMenager.cpp -o client.out -pthread -lboost_thread

using boost::asio::ip::tcp;

class CommunicationManager;

class Request{
public:
        enum class MessageType{
        REQUEST,
        GET,
        SEND,
    };

    Request(const std::string& _request,const MessageType _type,const std::string& ipAddress,const int _port):request(_request),type(_type),port(_port),ip(ipAddress)
    {}

    std::optional<std::string> getResponse()
    {
        if(isFinished==false)
            return {};
        return response;
    };

    void run(){
        switch(type){
            case MessageType::REQUEST:{
                communication();
            }break;
            case MessageType::GET:{
                read_message();
            }break;
            case MessageType::SEND:{
                send_message();
            }break;
        }  
    }

    bool finished() const {return isFinished;}

    const std::string& getRequest()const {return request;}

private:
    MessageType type;
    std::string request{""};
    std::string response{""};
    std::string ip{""};
    bool isFinished{false};
    int port;

private: 
    void communication(){
        try{
        boost::asio::io_context io_context;

        tcp::resolver resolver(io_context);
        tcp::resolver::results_type endpoints =
        resolver.resolve(ip, std::to_string(port));

        tcp::socket socket(io_context);
        boost::asio::connect(socket, endpoints);
    
        boost::array<char, 1024> buf;
        boost::system::error_code error;

        socket.write_some(boost::asio::buffer(request, request.length()*sizeof(char)));

        socket.read_some(boost::asio::buffer(buf), error);
        response = static_cast<std::string>(buf.data());
        isFinished=true;
        }catch(const std::exception& e){
            std::cout<<e.what()<<std::endl;
        }
    }

    void send_message(){
        try{
        boost::asio::io_context io_context;

        tcp::resolver resolver(io_context);
        tcp::resolver::results_type endpoints =
        resolver.resolve(ip, std::to_string(port));

        tcp::socket socket(io_context);
        boost::asio::connect(socket, endpoints);

    
        boost::array<char, 1024> buf;
        boost::system::error_code error;

        socket.write_some(boost::asio::buffer(request, request.length()*sizeof(char)));
        isFinished=true;
        }catch(const std::exception& e){
            std::cout<<e.what()<<std::endl;
        }
    }

    void read_message(){
        try{
        boost::asio::io_context io_context;

        tcp::resolver resolver(io_context);
        tcp::resolver::results_type endpoints =
        resolver.resolve(ip, std::to_string(port));

        tcp::socket socket(io_context);
        boost::asio::connect(socket, endpoints);

    
        boost::array<char, 1024> buf;
        boost::system::error_code error;

        socket.read_some(boost::asio::buffer(buf), error);
        response = static_cast<std::string>(buf.data());
        isFinished=true;
        }catch(const std::exception& e){
            std::cout<<e.what()<<std::endl;
        }
    }
};

class CommunicationManager{
    public:
        CommunicationManager(const std::string& _ipAddress,const int _port):port(_port),ipAddress(_ipAddress){

        };

        ~CommunicationManager(){
            for(auto& it: threads){
                if(it.joinable())
                it.join();
            }
        }

        Request& make_request(const std::string& _reuqest){
            requests.emplace_back(_reuqest,Request::MessageType::REQUEST,ipAddress,port);
            std::thread t(&Request::run,&requests.back());
            threads.push_back(std::move(t));
            return requests.back();
        }

        Request& make_request(const std::string& _reuqest, const std::string& _ip,const int _port){
            requests.emplace_back(_reuqest,Request::MessageType::REQUEST,_ip,_port);
            std::thread t(&Request::run,&requests.back());
            threads.push_back(std::move(t));

            return requests.back();
        }

        void send_message(const std::string& _request, const std::string& _ip, const int _port){
            Request req(_request,Request::MessageType::SEND,_ip,_port);
            std::thread t(&Request::run,&req);
            threads.push_back(std::move(t));
        }

        void send_message(const std::string& _request){
            Request req(_request,Request::MessageType::SEND,ipAddress,port);
            std::thread t(&Request::run,&req);
            threads.push_back(std::move(t));
        }

        Request&  get(const std::string& _request, const std::string& _ip, const int _port){
            requests.emplace_back(_request,Request::MessageType::GET,_ip,_port);
            std::thread t(&Request::run,&requests.back());
            threads.push_back(std::move(t));
            return requests.back();
        }

        Request& get(const std::string& _request){
            requests.emplace_back(_request,Request::MessageType::GET,ipAddress,port);
            std::thread t(&Request::run,&requests.back());
            threads.push_back(std::move(t));
            return requests.back();
        }

    private:
        std::vector<Request> requests;
        std::vector<std::thread> threads;
        std::string ipAddress;
        int port;
};


int main(){
    CommunicationManager communicator("127.0.0.1",9000);

    auto& request=communicator.make_request("Hello world\n");

   std::cout<<"Hello world\n";
   for(int i=0;i<5;i++){
   std::cout<<"> ";
   int a;
   std::cin>>a;
   }

   auto result=request.getResponse();
   if(result.has_value())
   std::cout<<result.value()<<std::endl;
   for(int i=0;i<10000;i++);
   return 0;
}


