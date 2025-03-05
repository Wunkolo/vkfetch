#pragma once

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <optional>
#include <string>
#include <string_view>

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

std::optional<std::string>
	FormatMeter(const std::size_t Width, const std::float_t Completion);

std::string FormatByteCount(std::size_t ByteCount);

std::string FormatVersion(std::uint32_t Version);

std::string ReplaceString(
	std::string Subject, std::string_view Search, std::string_view Replace
);

} // namespace Format