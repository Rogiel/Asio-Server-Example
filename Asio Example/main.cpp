//
//  main.cpp
//  Getting started with ASIO C++: creating a TCP server
//  http://www.rogiel.com/blog/getting-started-with-asio-cpp-creating-tcp-server
//
//  Created by Rogiel Sulzbach on 7/24/14.
//  Copyright (c) 2014 Rogiel Sulzbach. All rights reserved.
//

#include <iostream>
#include <asio.hpp>

/*
 * If you are using boost::asio instead of standalone asio, you can
 * uncomment the lines below and have an alias between the standalone
 * and the boost version. Don't forget to remove the include upthere.
 */
// #include <boost/asio.hpp>
// using asio = boost::asio;

int main(int argc, const char * argv[]) {
    // asio setup
    asio::io_service service;
    asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), 2000);
    asio::ip::tcp::acceptor acceptor(service, endpoint);
    
    // type alias, does nothing in special, just reduces verbosity
    using socket_ptr = std::shared_ptr<asio::ip::tcp::socket>;
    
    // the accept handler: it will receive a callback for every new socket
    // inside the handler we will write the message and disconnect the client
    std::function<void(socket_ptr, std::error_code)> handler = [&handler, &service, &acceptor](socket_ptr socket, std::error_code error) {
        std::cout << "New connection from " << socket->remote_endpoint().address() << std::endl;
        
        // writes the message and, once the write is complete, closes the connection
        socket->async_write_some(asio::buffer("Hello World\n"), [socket](std::error_code error, size_t bytes) {
            // closes the connection
            socket->close();
        });
        
        // accepts the next connection -- creates a new socket and gives it to the acceptor
        socket_ptr newSocket = std::make_shared<asio::ip::tcp::socket>(service);
        acceptor.async_accept(*newSocket, std::bind1st(handler, newSocket));
    };
    
    // creates a new socket for the FIRST connection
    socket_ptr socket = std::make_shared<asio::ip::tcp::socket>(service);
    
    // gives a socket reference to the acceptor and waits for the connection
    // notice the use o std::bind1st which will bind the first argument of
    // the lambda to "socket"
    acceptor.async_accept(*socket, std::bind1st(handler, socket));
    
    // runs the io queue -- will exit when the queue is empty
    service.run();
    
    // this should never be called in regular cases
    std::cout << "Done ;)" << std::endl;
    return 0;
}
