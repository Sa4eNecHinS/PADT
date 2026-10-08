#pragma once

#include <string>

struct HyprEvent {
    std::string event_name;
    std::string event_description;
};

class HyprEventReader {

private:
    int socket_fd;
    std::string pending;

    std::string read_event();

public:
    explicit HyprEventReader(int socket_fd);

    HyprEvent next_event();
};
