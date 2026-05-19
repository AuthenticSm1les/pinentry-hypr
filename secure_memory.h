#pragma once

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <new>
#include <string>
#include <sys/mman.h>
#include <unistd.h>

class SecureString {
public:
    SecureString() = default;

    explicit SecureString(const char *data, size_t len) {
        assign(data, len);
    }

    explicit SecureString(const std::string &s) {
        assign(s.data(), s.size());
    }

    SecureString(SecureString &&other) noexcept
        : m_data(other.m_data), m_size(other.m_size), m_capacity(other.m_capacity) {
        other.m_data = nullptr;
        other.m_size = 0;
        other.m_capacity = 0;
    }

    SecureString &operator=(SecureString &&other) noexcept {
        if (this != &other) {
            secureFree();
            m_data = other.m_data;
            m_size = other.m_size;
            m_capacity = other.m_capacity;
            other.m_data = nullptr;
            other.m_size = 0;
            other.m_capacity = 0;
        }
        return *this;
    }

    SecureString(const SecureString &) = delete;
    SecureString &operator=(const SecureString &) = delete;

    ~SecureString() { secureFree(); }

    const char *data() const { return m_data; }
    size_t size() const { return m_size; }
    bool empty() const { return m_size == 0; }

    void clear() {
        if (m_data) {
            std::memset(m_data, 0, m_size);
            m_size = 0;
        }
    }

    void assign(const char *data, size_t len) {
        if (len == 0) {
            clear();
            return;
        }
        if (len > m_capacity) {
            secureFree();
            allocate(len);
        }
        std::memcpy(m_data, data, len);
        m_size = len;
    }

    std::string toString() const {
        return m_data ? std::string(m_data, m_size) : std::string();
    }

private:
    char *m_data = nullptr;
    size_t m_size = 0;
    size_t m_capacity = 0;

    void allocate(size_t len) {
        long pageSize = sysconf(_SC_PAGESIZE);
        if (pageSize <= 0) pageSize = 4096;
        m_capacity = ((len + pageSize - 1) / pageSize) * pageSize;
        m_data = static_cast<char *>(std::aligned_alloc(pageSize, m_capacity));
        if (!m_data) throw std::bad_alloc();
        if (mlock(m_data, m_capacity) != 0) {
            std::free(m_data);
            m_data = nullptr;
            m_capacity = 0;
            throw std::bad_alloc();
        }
    }

    void secureFree() {
        if (m_data) {
            std::memset(m_data, 0, m_capacity);
            munlock(m_data, m_capacity);
            std::free(m_data);
            m_data = nullptr;
        }
        m_size = 0;
        m_capacity = 0;
    }
};
