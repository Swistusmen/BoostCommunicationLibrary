#include <iostream>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

//g++ server.cpp -o asyncServer.out -pthread -lboost_thread

using boost::asio::ip::tcp;

class tcpConnection: public boost::enable_shared_from_this<tcpConnection>
{
public:
    typedef boost::shared_ptr<tcpConnection> pointer;

    static pointer createTCPConnection(boost::asio::io_context&io)
    {
        return pointer(new tcpConnection(io));
    }

    tcp::socket& getSocket(){return socket;};

    void start(){
      std::string request{"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"};
      boost::array<char, 1024> buf;
        boost::asio::async_read(socket,boost::asio::buffer(buf),
          boost::bind(&tcpConnection::handle_read,
          shared_from_this(),
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
        std::cout<<buf.data();
        std::cout<<"> ";
        std::string message{""};
        std::getline(std::cin,message);

        boost::asio::async_write(socket,boost::asio::buffer(message),
            boost::bind(&tcpConnection::handle_write, 
            shared_from_this(), 
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    }

private:
    tcpConnection(boost::asio::io_context& io):socket(io){}

    void handle_write(const boost::system::error_code& /*error*/,
      size_t /*bytes_transferred*/){}

      void handle_read(const boost::system::error_code& /*error*/,
      size_t /*bytes_transferred*/){}

private:
    tcp::socket socket;
};

class tcpServer{
public:
    tcpServer(boost::asio::io_context& _context):context(_context),acceptor(context,tcp::endpoint(tcp::v4(),9000))
    {
        accept_connections();
    }

private:
    void accept_connections(){
        tcpConnection::pointer connection=tcpConnection::createTCPConnection(context);
        acceptor.async_accept(connection->getSocket(),boost::bind(&tcpServer::handle_accept,this,connection));
    }

    void handle_accept(tcpConnection::pointer connection){
        connection->start();
        accept_connections();
    }

private:
    boost::asio::io_context& context;
    tcp::acceptor acceptor;
};

class printer
{
public:
  printer(boost::asio::io_context& io)
    : strand_(io),
      timer1_(io, boost::posix_time::seconds(1)),
      timer2_(io, boost::posix_time::seconds(1)),
      count_(0)
  {
    timer1_.async_wait(boost::asio::bind_executor(strand_,
          boost::bind(&printer::print1, this)));

    timer2_.async_wait(boost::asio::bind_executor(strand_,
          boost::bind(&printer::print2, this)));
  }

  ~printer()
  {
    std::cout << "Final count is " << count_ << std::endl;
  }

  void print1()
  {
    if (count_ < 10)
    {
      std::cout << "Timer 1: " << count_ << std::endl;
      ++count_;

      timer1_.expires_at(timer1_.expires_at() + boost::posix_time::seconds(1));

      timer1_.async_wait(boost::asio::bind_executor(strand_,
            boost::bind(&printer::print1, this)));
    }
  }

  void print2()
  {
    if (count_ < 10)
    {
      std::cout << "Timer 2: " << count_ << std::endl;
      ++count_;

      timer2_.expires_at(timer2_.expires_at() + boost::posix_time::seconds(1));

      timer2_.async_wait(boost::asio::bind_executor(strand_,
            boost::bind(&printer::print2, this)));
    }
  }

private:
  boost::asio::io_context::strand strand_;
  boost::asio::deadline_timer timer1_;
  boost::asio::deadline_timer timer2_;
  int count_;
};

int main(){
    boost::asio::io_context io;
    tcpServer server(io);
    printer p(io);
    boost::thread t(boost::bind(&boost::asio::io_context::run, &io));
    io.run();
    t.join();

    return 0;
}

//TODO: add second thread in which there is a normal function