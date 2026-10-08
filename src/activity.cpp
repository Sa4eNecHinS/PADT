#include "activity.hpp"
#include "hypr_event.hpp"

std::optional<ActivityUpdate> parse_activity(const HyprEvent& event)
{
    const std::string& data = event.event_description;

    if (event.event_name == "activewindow") {
        const std::size_t separator = data.find(',');

        if (separator == std::string::npos) {
            return std::nullopt;
        }

        ActivityUpdate update { };
        update.kind = UpdateKind::active_window_changed;
        update.app_name = data.substr(0, separator);
        update.title = data.substr(separator + 1);
        return update;
    }

    if (event.event_name == "activewindowv2") {
        ActivityUpdate update { };
        update.kind = UpdateKind::active_window_address_changed;
        update.window_address = data;
        return update;
    }

    if (event.event_name == "windowtitlev2") {
        const std::size_t separator = data.find(',');

        if (separator == std::string::npos || separator == 0) {
            return std::nullopt;
        }

        ActivityUpdate update { };
        update.kind = UpdateKind::window_title_changed;
        update.window_address = data.substr(0, separator);
        update.title = data.substr(separator + 1);
        return update;
    }

    if (event.event_name == "windowtitle") {
        if (data.empty()) {
            return std::nullopt;
        }

        ActivityUpdate update { };
        update.kind = UpdateKind::window_title_invalidated;
        update.window_address = data;
        return update;
    }

    return std::nullopt;
}
