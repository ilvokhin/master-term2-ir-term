#include <cstdlib>
#include <iostream>

#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include "searcher.hpp"

// based on
// boost tcp echo server example by
// Christopher M. Kohlhoff (chris at kohlhoff dot com)

using boost::asio::ip::tcp;

class session
{
public:
  session(boost::asio::io_service& io_service, ir::searcher::searcher* srch):
    socket_(io_service), searcher_ptr_(srch)
  { }

  tcp::socket& socket()
  {
    return socket_;
  }

  void start()
  {
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
                            boost::bind(&session::handle_read, this,
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));
  }

  void handle_read(const boost::system::error_code& error, size_t bytes_transferred)
  {
    if (!error) {
      std::string response = searcher_ptr_->handle_raw_query(data_);
      boost::asio::async_write(socket_, boost::asio::buffer(response),
                               boost::bind(&session::handle_write, this,
                               boost::asio::placeholders::error));
    }
    else
      delete this;
  }

  void handle_write(const boost::system::error_code& error)
  {
    if (!error) {
      socket_.async_read_some(boost::asio::buffer(data_, max_length),
                              boost::bind(&session::handle_read, this,
                              boost::asio::placeholders::error,
                              boost::asio::placeholders::bytes_transferred));

      std::cerr << "incoming connection: " << data_;
    }
    else
      delete this;
  }

private:
  tcp::socket socket_;
  enum { max_length = 1024*1024 };
  char data_[max_length];
  ir::searcher::searcher* searcher_ptr_;
};

class server
{
public:
  server(boost::asio::io_service& io_service, short port,
         const std::string& index, const std::string& idf):
    io_service_(io_service), acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
    searcher_(index, idf)
  {
    session* new_session = new session(io_service_, &searcher_);
    acceptor_.async_accept(new_session->socket(),
                           boost::bind(&server::handle_accept, this, new_session,
                           boost::asio::placeholders::error));
  }

  void handle_accept(session* new_session, const boost::system::error_code& error)
  {
    if (!error) {
      new_session->start();
      new_session = new session(io_service_, &searcher_);
      acceptor_.async_accept(new_session->socket(),
                             boost::bind(&server::handle_accept, this,
                                         new_session,
                                         boost::asio::placeholders::error));
    }
    else
      delete new_session;
  }

private:
  boost::asio::io_service& io_service_;
  tcp::acceptor acceptor_;
  ir::searcher::searcher searcher_;
};

int main(int argc, char* argv[])
{
  try {
    if (argc != 4) {
      std::cerr << "Usage: tcp_worker <port> <index> <idf-table>" << std::endl;
      return 1;
    }

    boost::asio::io_service io_service;
    server s(io_service, std::atoi(argv[1]), argv[2], argv[3]);
    io_service.run();
  }
  catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
  }

  return 0;
}