//
// Created by nis on 25.12.16.
//

#ifndef PROJECT_DAEMON_H
#define PROJECT_DAEMON_H

#include <zconf.h>
#include <iostream>
#include <sys/stat.h>

namespace server {
    template <typename T> int get_daemon(T &daemon_start_function) {
        int pid = 0;
        // daemon processing
        pid = fork();

        if (pid == -1)
        {
            std::cout << "Error to start daemon" << std::endl;
        }
        else if (!pid) {
            umask(0);
            setsid();
            chdir("/");
            close(STDIN_FILENO);
            close(STDOUT_FILENO);
            close(STDERR_FILENO);

            pid = fork();
        }
        if (!pid) {
            daemon_start_function();
        }
        return 0;
    }
}

#endif //PROJECT_DAEMON_H
