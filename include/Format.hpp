#pragma once

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <optional>
#include <string>

namespace Format
{

namespace Literals
{
constexpr std::size_t operator""_KiB(unsigned long long int Size)
{
	return 1024ULL * Size;
}

constexpr std::size_t operator""_MiB(unsigned long long int Size)
{
	return 1024_KiB * Size;
}

constexpr std::size_t operator""_GiB(unsigned long long int Size)
{
	return 1024_MiB * Size;
}

constexpr std::size_t operator""_TiB(unsigned long long int Size)
{
	return 1024_GiB * Size;
}

constexpr std::size_t operator""_PiB(unsigned long long int Size)
{
	return 1024_TiB * Size;
}

} // namespace Literals

template<typename... Args>
std::optional<std::string>
	FormatString(const std::string& Format, Args... Arguments)
{
	const std::size_t Size
		= std::snprintf(nullptr, 0, Format.c_str(), Arguments...) + 1;
	if( Size <= 0 )
	{
		return std::nullopt;
	}
	const auto Buffer = std::make_unique<char[]>(Size);
	std::snprintf(Buffer.get(), Size, Format.c_str(), Arguments...);
	return std::string(Buffer.get(), Buffer.get() + Size - 1);
}

std::optional<std::string>
	FormatMeter(const std::size_t Width, const std::float_t Completion);

std::string FormatByteCount(std::size_t ByteCount);

std::string FormatVersion(std::uint32_t Version);

std::string ReplaceString(
	std::string Subject, const std::string& Search, const std::string& Replace
);

} // namespace Format