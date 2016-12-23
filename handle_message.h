#ifndef HANDLE_MESSAGE_H
#define HANDLE_MESSAGE_H

#include <string>

namespace http {
    const char title[] =
        "<head><title>simple web server</title></head>"
        "<body><h3><center>simple web server</center></h3></body>";
    const size_t size_title = sizeof(title) - 1;
    const char error404[] =
        "<head><title>simple web server</title></head>"
        "<body><h3><center>404 request page not found!</center></h3></body>";
    const size_t size_eror404 = sizeof(error404) - 1;
    
    const char head_ok[] =
        "HTTP/1.0 200 Ok\r\n"
        "Server: simple web\r\n"
        "Content-Type: text/html\r\n"
        "Content-Lenght: ";
    const size_t size_head_ok = sizeof(head_ok) - 1;
    
    const char head_error404[] = 
        "HTTP/1.0 404 Not Found\r\n"
        "Server: simple web\r\n"
        "Content-Type: text/html\r\n"
        "Content-Lenght: ";
    const size_t size_head_error404 = sizeof(head_error404) - 1;
    
    const size_t input_buffer_size = (4*1024);
    
    extern std::string directory;

    ssize_t handle_message(int client);
}

#endif // HANDLE_MESSAGE_H
