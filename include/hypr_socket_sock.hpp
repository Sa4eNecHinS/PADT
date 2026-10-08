#pragma once

#include <optional>
#include <string>
#include <sys/types.h>

struct HyprClients {
    std::optional<std::string> window_address;
    std::optional<std::string> window_pid;
    std::optional<std::string> app_name;
    std::optional<std::string> title;
};

std::optional<std::string> request_hyprland_json(
    const std::string& command = "clients");
