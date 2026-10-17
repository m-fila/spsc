#pragma once

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
