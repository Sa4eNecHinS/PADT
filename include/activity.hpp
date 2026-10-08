#pragma once

#include "hypr_event.hpp"

#include <optional>
#include <string>

struct HyprEvent;

enum class UpdateKind {
    active_window_changed,
    active_window_address_changed,
    window_title_changed,
    window_title_invalidated,
};

struct ActivityUpdate {
    UpdateKind kind;

    std::optional<std::string> window_address;
    std::optional<std::string> app_name;
    std::optional<std::string> title;
};

std::optional<ActivityUpdate> parse_activity(const HyprEvent& event);
