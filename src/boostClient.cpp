#include "boostClient.h"

#include <iostream>

#include <boost/beast.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "spdlog/spdlog.h"



BoostClient::BoostClient()
{

}

BoostClient::~BoostClient()
{

}

int BoostClient::getJsonDataFromURL(const std::string &URL, std::string &outResponce)
{
    namespace http = boost::beast::http;

    std::string ARGUMENTS = URL.substr(URL.find("/api/"), URL.length());
    std::string TOURNAMENTS_URL = URL.substr(URL.find("api.rivalry"), URL.find("/api/") - URL.find("api.rivalry"));

    try
    {
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

        if (res.result() != http::status::ok)
            return static_cast<int>(res.result());
        
        outResponce = boost::beast::buffers_to_string(res.body().data());

        // Закрываем соединение
        socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);

        return static_cast<int>(res.result());
    }
    catch (const std::exception &e)
    {
        spdlog::error("Exception: {}", e.what());
        return -2;
    }
    catch (...)
    {
        spdlog::error("Unknown exception");
        return -3;
    }
}


