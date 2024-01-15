#include <Format.hpp>

#include <VulkanConfig.hpp>

#include <fmt/core.h>

namespace Format
{

using namespace Literals;

std::optional<std::string>
	FormatMeter(const std::size_t Width, const std::float_t Completion)
{
	if( !Width || Width < 3 || Completion < 0.0f || !std::isfinite(Completion) )
	{
		return std::nullopt;
	}

	std::string Result;
	Result += "[";

	for( std::size_t Index = 0; Index < Width - 2; ++Index )
	{
		const std::float_t BarPhase
			= Index / static_cast<std::float_t>(Width - 3);
		if( BarPhase <= Completion )
		{
			if( BarPhase < 0.5f )
			{
				Result += "\033[92m";
			}
			else if( BarPhase < 0.75f )
			{
				Result += "\033[93m";
			}
			else
			{
				Result += "\033[91m";
			}
			Result += "|";
		}
		else
		{
			Result += " ";
		}
	}

	Result += "\033[0m]";
	return Result;
}

std::string FormatByteCount(std::size_t ByteCount)
{
	static const std::array<const char*, 9> SizeUnits
		= {{"Bytes", "KiB", "MiB", "GiB", "TiB", "PiB", "EiB", "ZiB", "YiB"}};

	std::size_t Index;
	double      ByteSize = ByteCount;
	for( Index = 0; Index < SizeUnits.size(); Index++ )
	{
		if( ByteSize < 1_KiB )
			break;
		ByteSize /= 1_KiB;
	}

	return fmt::format("{:.3f} {}", ByteSize, SizeUnits.at(Index));
}

std::string FormatVersion(std::uint32_t Version)
{
	return fmt::format(
		"{}.{}.{}", VK_VERSION_MAJOR(Version), VK_VERSION_MINOR(Version),
		VK_VERSION_PATCH(Version)
	);
}

std::string ReplaceString(
	std::string Subject, const std::string& Search, const std::string& Replace
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