#pragma once

#include <stdexcept>
#include <utility>

class custom_move_only_type {
    public:
    custom_move_only_type() noexcept = default;
    explicit custom_move_only_type(int value) noexcept : m_value(value) {}
    custom_move_only_type(const custom_move_only_type&) = delete;
    custom_move_only_type(custom_move_only_type&& other) noexcept = default;
    custom_move_only_type& operator=(const custom_move_only_type&) = delete;
    custom_move_only_type& operator=(custom_move_only_type&& other) noexcept =
        default;

    bool operator==(const custom_move_only_type& other) const {
        return m_value == other.m_value;
    }
    bool operator!=(const custom_move_only_type& other) const {
        return !(*this == other);
    }
    bool operator==(int other) const { return m_value == other; }
    bool operator!=(int other) const { return m_value != other; }
    friend bool operator==(int lhs, const custom_move_only_type& rhs) {
        return lhs == rhs.m_value;
    }
    friend bool operator!=(int lhs, const custom_move_only_type& rhs) {
        return lhs != rhs.m_value;
    }

    private:
    int m_value{0};
};

class custom_copy_only_type {
    public:
    custom_copy_only_type() noexcept = default;
    explicit custom_copy_only_type(int value) noexcept : m_value(value) {}
    custom_copy_only_type(const custom_copy_only_type& other) noexcept =
        default;
    custom_copy_only_type(custom_copy_only_type&& other) = delete;
    custom_copy_only_type& operator=(
        const custom_copy_only_type& other) noexcept = default;
    custom_copy_only_type& operator=(custom_copy_only_type&& other) noexcept =
        delete;

    bool operator==(const custom_copy_only_type& other) const {
        return m_value == other.m_value;
    }
    bool operator!=(const custom_copy_only_type& other) const {
        return !(*this == other);
    }
    bool operator==(int other) const { return m_value == other; }
    bool operator!=(int other) const { return m_value != other; }
    friend bool operator==(int lhs, const custom_copy_only_type& rhs) {
        return lhs == rhs.m_value;
    }
    friend bool operator!=(int lhs, const custom_copy_only_type& rhs) {
        return lhs != rhs.m_value;
    }

    private:
    int m_value{0};
};

class custom_move_constructible_only_type {
    public:
    explicit custom_move_constructible_only_type(int value) noexcept
        : m_value(value) {}
    custom_move_constructible_only_type(
        const custom_move_constructible_only_type&) = delete;
    custom_move_constructible_only_type(
        custom_move_constructible_only_type&& other) = default;
    custom_move_constructible_only_type& operator=(
        const custom_move_constructible_only_type&) = delete;
    custom_move_constructible_only_type& operator=(
        custom_move_constructible_only_type&& other) = delete;

    bool operator==(const custom_move_constructible_only_type& other) const {
        return m_value == other.m_value;
    }
    bool operator!=(const custom_move_constructible_only_type& other) const {
        return !(*this == other);
    }
    bool operator==(int other) const { return m_value == other; }
    bool operator!=(int other) const { return m_value != other; }
    friend bool operator==(int lhs,
                           const custom_move_constructible_only_type& rhs) {
        return lhs == rhs.m_value;
    }
    friend bool operator!=(int lhs,
                           const custom_move_constructible_only_type& rhs) {
        return lhs != rhs.m_value;
    }

    private:
    int m_value{0};
};

class custom_copy_constructible_only_type {
    public:
    explicit custom_copy_constructible_only_type(int value) noexcept
        : m_value(value) {}
    custom_copy_constructible_only_type(
        const custom_copy_constructible_only_type&) = default;
    custom_copy_constructible_only_type(
        custom_copy_constructible_only_type&& other) = delete;
    custom_copy_constructible_only_type& operator=(
        const custom_copy_constructible_only_type&) = delete;
    custom_copy_constructible_only_type& operator=(
        custom_copy_constructible_only_type&& other) = delete;

    bool operator==(const custom_copy_constructible_only_type& other) const {
        return m_value == other.m_value;
    }
    bool operator!=(const custom_copy_constructible_only_type& other) const {
        return !(*this == other);
    }
    bool operator==(int other) const { return m_value == other; }
    bool operator!=(int other) const { return m_value != other; }
    friend bool operator==(int lhs,
                           const custom_copy_constructible_only_type& rhs) {
        return lhs == rhs.m_value;
    }
    friend bool operator!=(int lhs,
                           const custom_copy_constructible_only_type& rhs) {
        return lhs != rhs.m_value;
    }

    private:
    int m_value{0};
};

// Assignment throws while armed, leaving the target unchanged and valid.
class custom_throwing_assign_type {
    public:
    custom_throwing_assign_type() noexcept = default;
    explicit custom_throwing_assign_type(int value,
                                         bool throw_on_assign = false) noexcept
        : m_value(value), m_throw_on_assign(throw_on_assign) {}
    custom_throwing_assign_type(const custom_throwing_assign_type&) = default;
    custom_throwing_assign_type(custom_throwing_assign_type&&) = default;
    void set_throw_on_assign(bool throw_on_assign) noexcept {
        m_throw_on_assign = throw_on_assign;
    }

    custom_throwing_assign_type& operator=(
        const custom_throwing_assign_type& other) {
        maybe_throw(other);
        m_value = other.m_value;
        return *this;
    }
    custom_throwing_assign_type& operator=(
        custom_throwing_assign_type&& other) {
        maybe_throw(other);
        m_value = std::move(other.m_value);
        return *this;
    }

    bool operator==(const custom_throwing_assign_type& other) const {
        return m_value == other.m_value;
    }
    bool operator!=(const custom_throwing_assign_type& other) const {
        return !(*this == other);
    }
    bool operator==(int other) const { return m_value == other; }
    bool operator!=(int other) const { return m_value != other; }
    friend bool operator==(int lhs, const custom_throwing_assign_type& rhs) {
        return lhs == rhs.m_value;
    }
    friend bool operator!=(int lhs, const custom_throwing_assign_type& rhs) {
        return lhs != rhs.m_value;
    }

    private:
    void maybe_throw(const custom_throwing_assign_type& other) const {
        if (m_throw_on_assign || other.m_throw_on_assign) {
            throw std::runtime_error("assignment failed");
        }
    }

    int m_value{0};
    bool m_throw_on_assign{false};
};

// Move-only type counting live instances to detect undestroyed objects.
class custom_tracked_type {
    public:
    custom_tracked_type() noexcept = default;
    custom_tracked_type(int value, int& live_count) noexcept
        : m_value(value), m_live_count(&live_count) {
        ++live_count;
    }
    custom_tracked_type(const custom_tracked_type&) = delete;
    custom_tracked_type(custom_tracked_type&& other) noexcept
        : m_value(other.m_value), m_live_count(other.m_live_count) {
        if (m_live_count != nullptr) {
            ++*m_live_count;
        }
    }
    custom_tracked_type& operator=(const custom_tracked_type&) = delete;
    custom_tracked_type& operator=(custom_tracked_type&& other) noexcept {
        m_value = other.m_value;
        if (m_live_count == nullptr) {
            m_live_count = other.m_live_count;
            if (m_live_count != nullptr) {
                ++*m_live_count;
            }
        }
        return *this;
    }
    ~custom_tracked_type() {
        if (m_live_count != nullptr) {
            --*m_live_count;
        }
    }

    bool operator==(const custom_tracked_type& other) const {
        return m_value == other.m_value;
    }
    bool operator!=(const custom_tracked_type& other) const {
        return !(*this == other);
    }
    bool operator==(int other) const { return m_value == other; }
    bool operator!=(int other) const { return m_value != other; }
    friend bool operator==(int lhs, const custom_tracked_type& rhs) {
        return lhs == rhs.m_value;
    }
    friend bool operator!=(int lhs, const custom_tracked_type& rhs) {
        return lhs != rhs.m_value;
    }

    private:
    int m_value{0};
    int* m_live_count{nullptr};
};
