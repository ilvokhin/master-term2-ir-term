#include <cstdlib>
#include <vector>
#include <memory>
#include <iostream>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

#include "searcher.hpp"

// based on
// boost tcp echo server example by
// Christopher M. Kohlhoff (chris at kohlhoff dot com)

using boost::asio::ip::tcp;

class session: public std::enable_shared_from_this<session>
{
public:
  typedef std::shared_ptr<session> ptr;

  static ptr create(boost::asio::io_service& io_service, ir::searcher::searcher* srch)
  {
    return ptr(new session(io_service, srch));
  }

  session(boost::asio::io_service& io_service, ir::searcher::searcher* srch):
    socket_(io_service), data_(max_length), searcher_ptr_(srch)
  { }

  tcp::socket& socket()
  {
    return socket_;
  }

  void start()
  {
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
                            boost::bind(&session::handle_read, shared_from_this(),
                            boost::asio::placeholders::error,
                            boost::asio::placeholders::bytes_transferred));
  }

  void handle_read(const boost::system::error_code& error, size_t bytes_transferred)
  {
    if (!error) {
      std::string query = std::string(data_.begin(), data_.begin() + bytes_transferred);
      log("incoming connection with " + query);
      std::string response = searcher_ptr_->handle_raw_query(query);
      boost::asio::async_write(socket_, boost::asio::buffer(response),
                               boost::bind(&session::handle_write, shared_from_this(),
                               boost::asio::placeholders::error));

      log(boost::str(boost::format("return response size of %ld bytes for %s")
                     % response.size() % query));
    }
  }

  void handle_write(const boost::system::error_code& error)
  {
    if (!error) {
      socket_.async_read_some(boost::asio::buffer(data_, max_length),
                              boost::bind(&session::handle_read, shared_from_this(),
                              boost::asio::placeholders::error,
                              boost::asio::placeholders::bytes_transferred));

    }
  }

  void log(const std::string& msg)
  {
    std::string trimmed(msg);
    boost::trim(trimmed); 
    std::cerr << trimmed << std::endl;
  }

private:
  tcp::socket socket_;
  enum { max_length = 1024 };
  std::vector<char> data_;
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
    session::ptr new_session = session::create(io_service_, &searcher_);
    acceptor_.async_accept(new_session->socket(),
                           boost::bind(&server::handle_accept, this, new_session,
                           boost::asio::placeholders::error));
  }

  void handle_accept(session::ptr new_session, const boost::system::error_code& error)
  {
    if (!error) {
      new_session->start();
      new_session = session::create(io_service_, &searcher_);
      acceptor_.async_accept(new_session->socket(),
                             boost::bind(&server::handle_accept, this,
                                         new_session,
                                         boost::asio::placeholders::error));
    }
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
