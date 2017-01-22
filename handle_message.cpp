#include "handle_message.h"
#include "uint_to_str.h"
#include "content_type_identify.h"

#include <fstream>

#include <sys/stat.h>
#include <unistd.h>
#include <limits>
#include <cstring>
#include <iostream>

namespace http {

    handler::handler() :
            header_buffer(new char[input_buffer_size]),
            transfer_buffer(new char[1024]) {
    }

    void handler::operator()(int client_fd) {
        std::memset(header_buffer.get(), 0, input_buffer_size);

        ssize_t len = recv(client_fd, header_buffer.get(), input_buffer_size, 0);
        const char * buf = header_buffer.get();

        if (len != 0) {
            if (buf[0] == 'G' && buf[1] == 'E' && buf[2] == 'T') {
                const char *first = &buf[4];
                while (*first == ' ') first++;
                const char *second = first;
                while ((*second != '?') && (*second != ' ')) second++;
                size_t content_len = 0;
                char *memory_block = transfer_buffer.get();
                if (second - first == 1 && *first == '/') {
                    std::memcpy(memory_block, head_ok, size_head_ok);
                    content_len = size_head_ok;
                    content_len += util::uint32_to_str(size_title, memory_block + content_len);
                    std::memcpy(memory_block + content_len, "\r\n\r\n", 4);
                    content_len += 4;
                    std::memcpy(memory_block + content_len, title, size_title);
                    content_len += size_title;
                } else {
                    std::string &directory = conf::Config::get_config()->directory;
                    file_path.reset(new char[directory.size() + (second - first)]);
                    std::memset(file_path.get(), 0, directory.size() + (second - first));
                    std::memcpy(file_path.get(), directory.c_str(), directory.size());
                    std::memcpy(file_path.get() + directory.size() - 1, first, second - first);

                    struct stat statbuf;
                    if (stat(file_path.get(), &statbuf) != -1) {
                        std::ifstream ifs;
                        size_t size = statbuf.st_size;
                        transfer_buffer.reset(new char[size + 1024]);
                        memory_block = transfer_buffer.get();

                        std::memcpy(memory_block, head_file, size_head_file);
                        content_len = size_head_file;

                        const char *point = second - 6;
                        while (point != second && *point != '.') point++;
                        if (*point == '.') point++;
                        const char *type = content_types[std::string(point, second)];
                        if (!type) {
                            type = "text/html";
                        }
                        std::strcpy(memory_block + content_len, type);

                        content_len += std::strlen(type);
                        std::strcpy(memory_block + content_len, "\r\nContent-Lenght: ");
                        content_len += std::strlen("\r\nContent-Lenght: ");

                        content_len += util::uint32_to_str(size, memory_block + content_len);
                        std::memcpy(memory_block + content_len, "\r\n\r\n", 4);
                        content_len += 4;

                        ifs.open(file_path.get(), std::ifstream::in);
                        ifs.read(memory_block + content_len, size);
                        content_len += size;
                        ifs.close();
                    } else {
                        std::memcpy(memory_block, head_error404, size_eror404);
                        content_len = size_head_error404;
                        content_len += util::uint32_to_str(size_eror404, memory_block + content_len);
                        std::memcpy(memory_block + content_len, "\r\n\r\n", 4);
                        content_len += 4;

                        std::memcpy(memory_block + content_len, error404, size_eror404);
                        content_len += size_eror404;
                    }
                }
                send(client_fd, memory_block, (size_t) content_len, 0);
            }
        }
        close(client_fd);
    }
}
