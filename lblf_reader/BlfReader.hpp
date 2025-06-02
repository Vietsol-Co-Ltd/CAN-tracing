#pragma once
#include "CanMessage.hpp"
#include <vector>
#include <string>

std::vector<CanMessage> loadCanMessagesFromBlf(const std::string& filepath);
