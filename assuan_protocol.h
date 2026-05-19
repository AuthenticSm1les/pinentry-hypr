#pragma once

#include <string>
#include <string_view>

enum class AssuanCmd {
    SETPROMPT,
    SETDESC,
    SETKEYINFO,
    GETPIN,
    CONFIRM,
    MESSAGE,
    OPTION,
    SETTIMEOUT,
    SETOK,
    SETCANCEL,
    SETNOTSET,
    SETERROR,
    SETTITLE,
    BYE,
    UNKNOWN
};

struct AssuanLine {
    AssuanCmd cmd = AssuanCmd::UNKNOWN;
    std::string value;
};

inline bool isCommandMatch(const std::string &line, const std::string &cmd) {
    if (line.size() < cmd.size()) return false;
    if (line.substr(0, cmd.size()) != cmd) return false;
    if (line.size() == cmd.size()) return true;
    return line[cmd.size()] == ' ';
}

inline int safeSubstrOffset(const std::string &line, size_t cmdLen) {
    return line.size() > cmdLen + 1 ? cmdLen + 1 : cmdLen;
}

inline AssuanLine parseAssuanLine(const std::string &line) {
    if (isCommandMatch(line, "SETPROMPT"))
        return {AssuanCmd::SETPROMPT, line.substr(safeSubstrOffset(line, 9))};
    if (isCommandMatch(line, "SETDESC"))
        return {AssuanCmd::SETDESC, line.substr(safeSubstrOffset(line, 7))};
    if (isCommandMatch(line, "SETKEYINFO"))
        return {AssuanCmd::SETKEYINFO, line.substr(safeSubstrOffset(line, 10))};
    if (isCommandMatch(line, "SETTIMEOUT"))
        return {AssuanCmd::SETTIMEOUT, line.substr(safeSubstrOffset(line, 10))};
    if (isCommandMatch(line, "SETOK"))
        return {AssuanCmd::SETOK, line.substr(safeSubstrOffset(line, 5))};
    if (isCommandMatch(line, "SETCANCEL"))
        return {AssuanCmd::SETCANCEL, line.substr(safeSubstrOffset(line, 9))};
    if (isCommandMatch(line, "SETNOTSET"))
        return {AssuanCmd::SETNOTSET, line.substr(safeSubstrOffset(line, 9))};
    if (isCommandMatch(line, "SETERROR"))
        return {AssuanCmd::SETERROR, line.substr(safeSubstrOffset(line, 8))};
    if (isCommandMatch(line, "SETTITLE"))
        return {AssuanCmd::SETTITLE, line.substr(safeSubstrOffset(line, 8))};
    if (isCommandMatch(line, "GETPIN"))
        return {AssuanCmd::GETPIN, ""};
    if (isCommandMatch(line, "CONFIRM"))
        return {AssuanCmd::CONFIRM, ""};
    if (isCommandMatch(line, "MESSAGE"))
        return {AssuanCmd::MESSAGE, ""};
    if (isCommandMatch(line, "OPTION"))
        return {AssuanCmd::OPTION, line.substr(safeSubstrOffset(line, 6))};
    if (isCommandMatch(line, "BYE"))
        return {AssuanCmd::BYE, ""};
    return {AssuanCmd::UNKNOWN, line};
}

inline bool hasKeyInfoOption(const std::string &option) {
    return option == "allow-external-password-cache";
}

inline std::string getPwmngrValue(const std::string &option) {
    static constexpr std::string_view prefix = "default-pwmngr=";
    if (option.size() > prefix.size() && option.substr(0, prefix.size()) == prefix) {
        return option.substr(prefix.size());
    }
    return "";
}
