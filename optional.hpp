#pragma once
#include <memory>
#include <stdexcept>
#include <sstream>

template <typename T>
class Optional
{
public:
	Optional();
	Optional(const T& value);
	Optional(T&& value);
	Optional(const Optional& other) = default;
	Optional(Optional&& other) = default;
	virtual ~Optional() = default;
	Optional& operator=(const Optional& other) = default;
	Optional& operator=(Optional&& other) = default;

	inline bool has_value() { return has_value_; }
	T& unwrap();

private:
	bool has_value_;
	T value_;
};

template <typename T>
Optional<T>::Optional()
  : has_value_ {false}
{
}

template <typename T>
Optional<T>::Optional(const T& value)
  : has_value_ {true}
  , value_ {value}
{
}

template <typename T>
Optional<T>::Optional(T&& value)
  : has_value_ {true}
  , value_ {std::move(value)}
{
}

template <typename T>
T& Optional<T>::unwrap()
{
	if (not has_value_) {
		throw std::logic_error("Cannot unwrap empty Optional.");
	}
	return value_;
}
