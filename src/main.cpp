#include "activity.hpp"
#include "hypr_event.hpp"
#include "hypr_socket2_sock.hpp"
#include "hypr_socket_sock.hpp"

#include <cstdlib>
#include <iostream>

int main()
{
    int socket_fd = connect_to_socket();
    HyprEventReader hypr_event(socket_fd);
    const auto response = request_hyprland_json();

    if (!response) {
        return EXIT_FAILURE;
    }

    std::cout << *response << "\n ----- ";

    while (true) {
        HyprEvent ev = hypr_event.next_event();
        auto update = parse_activity(ev);

        if (update) {
            if (update->kind == UpdateKind::active_window_changed) {
                std::cout << "update.kind -> active_window_changed\n";
                std::cout << "      .app_name -> " << update->app_name.value_or("no_app") << "\n";
                std::cout << "      .title -> " << update->title.value_or("no_title") << "\n\n";
            }

            if (update->kind == UpdateKind::window_title_changed) {
                std::cout << "update.kind -> window_title_changed\n";
                std::cout << "      .window_address -> " << update->window_address.value_or("no_window_adress") << "\n";
                std::cout << "      .title -> " << update->title.value_or("no_title") << "\n\n";
            }
        }
    }

    return EXIT_SUCCESS;
}
