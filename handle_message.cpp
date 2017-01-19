#include "handle_message.h"
#include "uint_to_str.h"

#include <fstream>

#include <sys/stat.h>
#include <unistd.h>
#include <limits>
#include <cstring>
#include <iostream>

namespace http {


    ssize_t handle_message(int client) {
        char buf[input_buffer_size] = {0};

        ssize_t len = recv(client, buf, input_buffer_size, 0);

        if (len != 0) {
            if (buf[0] == 'G' && buf[1] == 'E' && buf[2] == 'T') {
                char *first = &buf[4];
                while (*first == ' ') first++;
                char *second = first;
                while ((*second != '?') && (*second != ' ')) second++;
                bool transfer_file = false;
                bool success = false;
                size_t content_len = 0;
                char *memory_block = 0;
                if (second - first == 1 && *first == '/') {
                    success = true;
                    memory_block = (char *)title;
                    content_len = size_title;
                } else {
                    std::string path(conf::Config::get_config()->directory);
                    path.append(first, second - first);

                    struct stat statbuf;
                    if (stat(path.c_str(), &statbuf) != -1) {
                        {
                            std::ifstream ifs;
                            size_t size = statbuf.st_size;
                            ifs.open(path.c_str(), std::ifstream::in);
                            memory_block = new char[size];
                            transfer_file = true;
                            success = true;
                            ifs.read(memory_block, size);
                            content_len = (size_t) size;
                            ifs.close();
                        }
                    } else {
                        memory_block = (char *)error404;
                        content_len = size_eror404;
                    }
                }
                if (success) {
                    send(client, head_ok, (size_t) size_head_ok, 0);
                } else {
                    send(client, head_error404, (size_t) size_head_error404, 0);
                }
                char buffer[10+4+1] = {0};
                int size = util::uint32_to_str(content_len, buffer);
                std::memcpy(buffer + size, "\n\n\n\n", 4);
                size += 4;
                send(client, buffer, (size_t) size, 0);
                if (content_len) {
                    send(client, memory_block, (size_t) content_len, 0);
                }
                if (transfer_file)
                    delete [] memory_block;
            }
        }
        close(client);

        return len;
    }

}
