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

using boost::asio::ip::tcp;

class CommunicationManager;

class Request{
public:
    Request(const std::string& _request, const std::string& ipAddress,const int _port):request(_request),port(_port),ip(ipAddress)
    {

    }

    std::optional<std::string> getResponse()
    {
        if(isFinished==false)
            return {};
        return response;
    };

    void run(){
        communication();
    }

    bool finished() const {return isFinished;}

    const std::string& getRequest()const {return request;}

private:
    std::string request{""};
    std::string response{""};
    std::string ip{""};
    bool isFinished{false};
    int port;

private: 
    void communication(){

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
    }
};

class CommunicationManager{
    public:
        CommunicationManager(const std::string& _ipAddress,const int _port):port(_port),ipAddress(_ipAddress){

        };

        ~CommunicationManager(){
            
        }

        Request& make_request(const std::string& _reuqest){
            requests.emplace_back(_reuqest,ipAddress,port);
            std::thread t(&Request::run,&requests.back());
            t.detach();

            return requests.back();
        }

        Request& make_request(const std::string& _reuqest, const std::string& _ip,const int _port){
            requests.emplace_back(_reuqest,_ip,_port);
            std::thread t(&Request::run,&requests.back());
            t.detach();

            return requests.back();
        }

    private:
        std::vector<Request> requests;
        std::string ipAddress;
        int port;

    private: 
    void communication(){
        
    }
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
   for(int i=0;i<1000000000000000000000000;i++);
   return 0;
}


