#include <Format.hpp>

#include <VulkanConfig.hpp>

#include <fmt/core.h>

namespace Format
{

using namespace Literals;

using namespace std::string_view_literals;

std::optional<std::string>
	FormatMeter(const std::size_t Width, const std::float_t Completion)
{
	if( !Width || Width < 3 || Completion < 0.0f )
	{
		return std::nullopt;
	}

	std::string Result;
	Result += "["sv;

	for( std::size_t Index = 0; Index < Width - 2; ++Index )
	{
		const std::float_t BarPhase
			= Index / static_cast<std::float_t>(Width - 3);
		if( !std::isfinite(Completion) )
		{
			Result += "\033[90m-"sv;
		}
		else if( BarPhase <= Completion )
		{
			if( BarPhase < 0.5f )
			{
				Result += "\033[92m"sv;
			}
			else if( BarPhase < 0.75f )
			{
				Result += "\033[93m"sv;
			}
			else
			{
				Result += "\033[91m"sv;
			}
			Result += "|"sv;
		}
		else
		{
			Result += " "sv;
		}
	}

	Result += "\033[0m]";
	return Result;
}

std::string FormatByteCount(std::size_t ByteCount)
{
	static const std::array<std::string_view, 9> SizeUnits = {{
		"Bytes"sv,
		"KiB"sv,
		"MiB"sv,
		"GiB"sv,
		"TiB"sv,
		"PiB"sv,
		"EiB"sv,
		"ZiB"sv,
		"YiB"sv,
	}};

	std::size_t Index;
	double      ByteSize = ByteCount;
	for( Index = 0; Index < SizeUnits.size(); Index++ )
	{
		if( ByteSize < 1_KiB )
		{
			break;
		}
		ByteSize /= 1_KiB;
	}

	return fmt::format("{:.3f} {}"sv, ByteSize, SizeUnits.at(Index));
}

std::string FormatVersion(std::uint32_t Version)
{
	return fmt::format(
		"{}.{}.{}"sv, VK_VERSION_MAJOR(Version), VK_VERSION_MINOR(Version),
		VK_VERSION_PATCH(Version)
	);
}

std::string ReplaceString(
	std::string Subject, std::string_view Search, std::string_view Replace
)
{
	size_t Position = 0;
	while( (Position = Subject.find(Search, Position)) != std::string::npos )
	{
		Subject.replace(Position, Search.length(), Replace);
		Position += Replace.length();
	}
	return Subject;
}

} // namespace Format