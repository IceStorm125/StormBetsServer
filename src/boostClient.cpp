#include "boostClient.h"

#include <iostream>

#include <boost/beast.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>

BoostClient::BoostClient()
{

}

BoostClient::~BoostClient()
{

}

int BoostClient::getJsonDataFromURL(const std::string &URL, std::string &outResponce)
{
    namespace http = boost::beast::http;

    const std::string TOURNAMENTS_URL{"api.rivalry.com"};
    const std::string ARGUMENTS {"/api/v1/tournaments/3465"};

    boost::asio::io_context io;
    boost::asio::ip::tcp::resolver resolver(io);
    boost::asio::ip::tcp::socket socket(io);

    boost::asio::connect(socket, resolver.resolve(TOURNAMENTS_URL, "80"));

    http::request<http::string_body> req(http::verb::get, ARGUMENTS, 11);
    // Задаём поля HTTP заголовка
    req.set(http::field::host, TOURNAMENTS_URL);
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    // Отправляем реквест через приконекченный сокет
    http::write(socket, req);

    // Часть, отвечающая за чтение респонса

    boost::beast::flat_buffer buffer;
    http::response<http::dynamic_body> res;
    http::read(socket, buffer, res);

    std::cout << res << std::endl;

    // Закрываем соединение
    socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);


    return 0;
}


