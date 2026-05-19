#include "curses_fallback.h"
#include <curses.h>
#include <cstring>
#include <ctime>
#include <unistd.h>

static bool hasTerminal() {
    return isatty(STDIN_FILENO) && isatty(STDOUT_FILENO);
}

bool cursesAvailable() {
    static bool checked = false;
    static bool available = false;
    if (!checked) {
        checked = true;
        available = hasTerminal();
    }
    return available;
}

static int getRemainingMs(int totalSeconds, time_t start) {
    if (totalSeconds <= 0) return -1;
    time_t now = time(nullptr);
    int elapsed = static_cast<int>(now - start);
    int remaining = totalSeconds - elapsed;
    return remaining > 0 ? remaining * 1000 : 0;
}

static void drawBorder(WINDOW *win, const char *title) {
    wborder(win, 0, 0, 0, 0, 0, 0, 0, 0);
    if (title) {
        mvwprintw(win, 0, 2, " %s ", title);
    }
}

static void showTimeout(WINDOW *win, int y, int x, int remainingSec) {
    if (remainingSec > 0) {
        wattron(win, A_BOLD);
        mvwprintw(win, y, x, "Time remaining: %ds", remainingSec);
        wattroff(win, A_BOLD);
    }
}

static std::string getWindowTitle() {
    return "pinentry-hypr";
}

bool cursesGetpin(const std::string &prompt, const std::string &desc,
                  std::string &result, int timeoutSeconds) {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    int height = 12;
    int width = 60;
    if (cols < width) width = cols - 2;
    if (rows < height) height = rows - 2;

    int startY = (rows - height) / 2;
    int startX = (cols - width) / 2;

    WINDOW *win = newwin(height, width, startY, startX);

    time_t startTime = time(nullptr);

    curs_set(1);
    bool done = false;
    bool confirmed = false;
    std::string input;

    timeout(250);

    while (!done) {
        drawBorder(win, getWindowTitle().c_str());

        int promptY = 1;
        int descY = 2;
        int inputY = 4;
        int statusY = 6;
        int btnY = 8;
        int xPad = 2;

        if (!desc.empty()) {
            mvwprintw(win, promptY, xPad, "%s", prompt.c_str());
            int maxW = width - 2 * xPad - 2;
            std::string d = desc;
            if (static_cast<int>(d.size()) > maxW) {
                d = d.substr(0, maxW - 3) + "...";
            }
            mvwprintw(win, descY, xPad, "%s", d.c_str());
        } else {
            mvwprintw(win, promptY, xPad, "%s", prompt.c_str());
        }

        int remaining = timeoutSeconds > 0
            ? timeoutSeconds - static_cast<int>(time(nullptr) - startTime)
            : -1;
        if (remaining < 0) remaining = 0;
        showTimeout(win, statusY, xPad, remaining);

        mvwprintw(win, inputY, xPad, "> ");
        for (size_t i = 0; i < input.size(); i++) {
            mvwaddch(win, inputY, xPad + 2 + i, '*');
        }
        wmove(win, inputY, xPad + 2 + static_cast<int>(input.size()));

        const char *okBtn = "[ Authenticate ]";
        const char *cancelBtn = "[ Cancel ]";
        int btnWidth = std::strlen(okBtn) + std::strlen(cancelBtn) + 3;
        int btnStart = (width - btnWidth) / 2;
        if (btnStart < 1) btnStart = 1;
        mvwprintw(win, btnY, btnStart, "%s   %s", okBtn, cancelBtn);

        wrefresh(win);

        if (remaining <= 0 && timeoutSeconds > 0) {
            done = true;
            confirmed = false;
            break;
        }

        int ch = wgetch(win);
        switch (ch) {
        case '\n':
        case '\r':
        case KEY_ENTER:
            confirmed = true;
            done = true;
            break;
        case 27:
            done = true;
            confirmed = false;
            break;
        case KEY_BACKSPACE:
        case 127:
            if (!input.empty()) input.pop_back();
            break;
        case ERR:
            break;
        default:
            if (ch >= 32 && ch <= 126) input.push_back(static_cast<char>(ch));
            break;
        }
    }

    curs_set(0);
    delwin(win);
    endwin();
    refresh();

    if (confirmed) {
        result = input;
        std::memset(input.data(), 0, input.size());
    }
    return confirmed;
}

bool cursesConfirm(const std::string &desc, const std::string &okText,
                   const std::string &cancelText, int timeoutSeconds) {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    int height = 8;
    int width = 50;
    if (cols < width) width = cols - 2;

    int startY = (rows - height) / 2;
    int startX = (cols - width) / 2;

    WINDOW *win = newwin(height, width, startY, startX);

    time_t startTime = time(nullptr);
    bool done = false;
    bool confirmed = false;

    timeout(250);

    while (!done) {
        drawBorder(win, getWindowTitle().c_str());

        int y = 1;
        int xPad = 2;
        int maxW = width - 2 * xPad - 2;
        std::string d = desc;
        if (static_cast<int>(d.size()) > maxW) {
            d = d.substr(0, maxW - 3) + "...";
        }
        mvwprintw(win, y, xPad, "%s", d.c_str());

        int remaining = timeoutSeconds > 0
            ? timeoutSeconds - static_cast<int>(time(nullptr) - startTime)
            : -1;
        if (remaining <= 0 && timeoutSeconds > 0) {
            done = true;
            confirmed = false;
            break;
        }
        showTimeout(win, 2, xPad, remaining);

        std::string ok = okText.empty() ? "OK" : okText;
        std::string cancel = cancelText.empty() ? "Cancel" : cancelText;
        std::string btnText = "[ " + ok + " ]   [ " + cancel + " ]";
        int btnStart = (width - static_cast<int>(btnText.size())) / 2;
        if (btnStart < 1) btnStart = 1;
        mvwprintw(win, 4, btnStart, "%s", btnText.c_str());

        wrefresh(win);

        int ch = wgetch(win);
        switch (ch) {
        case '\n':
        case '\r':
        case KEY_ENTER:
            confirmed = true;
            done = true;
            break;
        case 27:
            done = true;
            confirmed = false;
            break;
        case ERR:
            break;
        }
    }

    delwin(win);
    endwin();
    refresh();

    return confirmed;
}

void cursesMessage(const std::string &desc, const std::string &okText,
                   int timeoutSeconds) {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    int height = 7;
    int width = 50;
    if (cols < width) width = cols - 2;

    int startY = (rows - height) / 2;
    int startX = (cols - width) / 2;

    WINDOW *win = newwin(height, width, startY, startX);

    time_t startTime = time(nullptr);
    bool done = false;

    timeout(250);

    while (!done) {
        drawBorder(win, getWindowTitle().c_str());

        int y = 1;
        int xPad = 2;
        int maxW = width - 2 * xPad - 2;
        std::string d = desc;
        if (static_cast<int>(d.size()) > maxW) {
            d = d.substr(0, maxW - 3) + "...";
        }
        mvwprintw(win, y, xPad, "%s", d.c_str());

        int remaining = timeoutSeconds > 0
            ? timeoutSeconds - static_cast<int>(time(nullptr) - startTime)
            : -1;
        if (remaining <= 0 && timeoutSeconds > 0) {
            done = true;
            break;
        }
        showTimeout(win, 2, xPad, remaining);

        std::string ok = okText.empty() ? "OK" : okText;
        std::string btnText = "[ " + ok + " ]";
        int btnStart = (width - static_cast<int>(btnText.size())) / 2;
        if (btnStart < 1) btnStart = 1;
        mvwprintw(win, 4, btnStart, "%s", btnText.c_str());

        wrefresh(win);

        int ch = wgetch(win);
        switch (ch) {
        case '\n':
        case '\r':
        case KEY_ENTER:
        case ' ':
            done = true;
            break;
        case ERR:
            break;
        }
    }

    delwin(win);
    endwin();
    refresh();
}
