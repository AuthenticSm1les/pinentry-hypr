#pragma once

#include <string>

bool cursesAvailable();
bool cursesGetpin(const std::string &prompt, const std::string &desc,
                  std::string &result, int timeoutSeconds);
bool cursesConfirm(const std::string &desc, const std::string &okText,
                   const std::string &cancelText, int timeoutSeconds);
void cursesMessage(const std::string &desc, const std::string &okText,
                   int timeoutSeconds);
