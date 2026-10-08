#include "hypr_socket_sock.hpp"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include <sys/socket.h>
#include <sys/time.h>
#include <sys/un.h>
#include <unistd.h>

namespace {

struct SocketHandle {
    explicit SocketHandle(int fd)
        : fd(fd)
    {
    }
    ~SocketHandle()
    {
        if (fd >= 0) {
            ::close(fd);
        }
    }

    SocketHandle(const SocketHandle&) = delete;
    SocketHandle& operator=(const SocketHandle&) = delete;

    int fd;
};

}

std::optional<std::string> request_hyprland_json(const std::string& command)
{
    const char* runtime_dir = std::getenv("XDG_RUNTIME_DIR");
    const char* instance_signature = std::getenv("HYPRLAND_INSTANCE_SIGNATURE");

    if (!runtime_dir || !*runtime_dir || !instance_signature || !*instance_signature) {
        std::cerr << "XDG_RUNTIME_DIR or HYPRLAND_INSTANCE_SIGNATURE is missing\n";
        return std::nullopt;
    }

    if (command.empty()) {
        std::cerr << "Hyprland command must not be empty\n";
        return std::nullopt;
    }

    const std::string socket_path = std::string(runtime_dir) + "/hypr/" + instance_signature + "/.socket.sock";
    // почему команда начинается с j/ ?
    const std::string request = "j/" + command;

    sockaddr_un socket_address { };
    socket_address.sun_family = AF_UNIX;

    if (socket_path.size() >= sizeof(socket_address.sun_path)) {
        std::cerr << "Hyprland socket path is too long\n";
        return std::nullopt;
    }
    std::memcpy(socket_address.sun_path, socket_path.c_str(), socket_path.size() + 1);

    SocketHandle socket_handle(::socket(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0));
    if (socket_handle.fd == -1) {
        std::perror("socket");
        return std::nullopt;
    }

    timeval timeout { };
    timeout.tv_sec = 3;
    if (::setsockopt(socket_handle.fd, SOL_SOCKET, SO_SNDTIMEO,
            &timeout, sizeof(timeout))
            == -1
        || ::setsockopt(socket_handle.fd, SOL_SOCKET, SO_RCVTIMEO,
               &timeout, sizeof(timeout))
            == -1) {
        std::perror("setsockopt");
        return std::nullopt;
    }

    if (::connect(socket_handle.fd,
            reinterpret_cast<const sockaddr*>(&socket_address),
            sizeof(socket_address))
        == -1) {
        std::perror("connect");
        return std::nullopt;
    }

    std::size_t sent_bytes = 0;
    while (sent_bytes < request.size()) {
        const ssize_t bytes_sent = ::send(
            socket_handle.fd, request.data() + sent_bytes,
            request.size() - sent_bytes, MSG_NOSIGNAL);

        if (bytes_sent == -1) {
            if (errno == EINTR) {
                continue;
            }
            std::perror("send");
            return std::nullopt;
        }
        if (bytes_sent == 0) {
            std::cerr << "Could not send the complete Hyprland request\n";
            return std::nullopt;
        }
        sent_bytes += static_cast<std::size_t>(bytes_sent);
    }

    if (::shutdown(socket_handle.fd, SHUT_WR) == -1) {
        std::perror("shutdown");
        return std::nullopt;
    }

    std::string response;
    char buffer[4096];

    while (true) {
        const ssize_t bytes_read = ::recv(socket_handle.fd, buffer, sizeof(buffer), 0);

        if (bytes_read == -1) {
            if (errno == EINTR) {
                continue;
            }
            std::perror("recv");
            return std::nullopt;
        }
        if (bytes_read == 0) {
            break;
        }
        response.append(buffer, static_cast<std::size_t>(bytes_read));
    }

    if (response.empty()) {
        std::cerr << "Hyprland returned an empty reply\n";
        return std::nullopt;
    }

    return response;
}
