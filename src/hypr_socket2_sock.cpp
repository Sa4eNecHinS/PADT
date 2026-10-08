#include "hypr_socket2_sock.hpp"

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

int connect_to_socket()
{
    const char* runtime_dir = std::getenv("XDG_RUNTIME_DIR");
    const char* instance_signature = std::getenv("HYPRLAND_INSTANCE_SIGNATURE");

    if (!runtime_dir || !instance_signature) {
        std::cerr << "Hyprland environment variables are missing\n";
        return -1;
    }

    std::string socket_path = std::string(runtime_dir)
        + "/hypr/"
        + instance_signature
        + "/.socket2.sock";

    int socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (socket_fd == -1) {
        perror("socket");
        return -1;
    }

    sockaddr_un address { };
    address.sun_family = AF_UNIX;

    std::strncpy(
        address.sun_path,
        socket_path.c_str(),
        sizeof(address.sun_path) - 1);

    if (connect(
            socket_fd,
            reinterpret_cast<sockaddr*>(&address),
            sizeof(address))
        == -1) {

        perror("connect");
        close(socket_fd);

        return -1;
    }

    return socket_fd;
}
