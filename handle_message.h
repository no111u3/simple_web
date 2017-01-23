#ifndef HANDLE_MESSAGE_H
#define HANDLE_MESSAGE_H

#include "config.h"

#include <memory>

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

    const char head_file[] =
        "HTTP/1.0 200 Ok\r\n"
        "Server: simple web\r\n"
        "Content-Type: ";
    const size_t size_head_file = sizeof(head_file) - 1;

    const size_t input_buffer_size = (4*1024);

    class handler final {
    private:
        std::unique_ptr<char[]> file_path;
        size_t file_path_size;
        std::unique_ptr<char[]> header_buffer;
        std::unique_ptr<char[]> transfer_buffer;
        size_t transfer_size;
    public:
        handler();

        void operator ()(int client_fd);
    };
}

#endif // HANDLE_MESSAGE_H
