#include "hypr_event.hpp"

#include <unistd.h>

#include <cstdlib>
#include <iostream>
#include <string>

HyprEventReader::HyprEventReader(int socket_fd)
    : socket_fd(socket_fd)
{
}

std::string HyprEventReader::read_event()
{
    while (true) {
        std::size_t newline = pending.find('\n');

        if (newline != std::string::npos) {
            std::string event = pending.substr(0, newline);

            pending.erase(0, newline + 1);

            return event;
        }

        char buffer[4096];

        ssize_t bytes_read = read(
            socket_fd,
            buffer,
            sizeof(buffer));

        if (bytes_read == 0) {
            std::cerr << "hyprland closed connection\n";
            std::exit(EXIT_FAILURE);
        }

        if (bytes_read == -1) {
            perror("read");
            std::exit(EXIT_FAILURE);
        }

        pending.append(buffer, bytes_read);
    }
}

HyprEvent HyprEventReader::next_event()
{
    std::string event = read_event();

    std::size_t separator = event.find(">>");

    if (separator == std::string::npos) {
        return {
            event,
            ""
        };
    }

    return {
        event.substr(0, separator),
        event.substr(separator + 2)
    };
}
