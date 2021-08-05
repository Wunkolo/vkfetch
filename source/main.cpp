#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <optional>
#include <span>

#include <VulkanConfig.hpp>
#include <VulkanUtil.hpp>

#ifdef _WIN32
#define NOMINMAX
#include <Windows.h>
// Statically enables "ENABLE_VIRTUAL_TERMINAL_PROCESSING" for the terminal
// at runtime to allow for unix-style escape sequences.
static const bool _WndV100Enabled = []() -> bool {
	const auto Handle = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD      ConsoleMode;
	GetConsoleMode(Handle, &ConsoleMode);
	SetConsoleMode(Handle, ConsoleMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	GetConsoleMode(Handle, &ConsoleMode);
	return ConsoleMode & ENABLE_VIRTUAL_TERMINAL_PROCESSING;
}();
#endif

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

using FetchLog = std::vector<std::string>;
using FetchArt = std::span<const char*>;

std::string FormatByteCount(std::size_t ByteCount)
{
	static std::array<const char*, 9> SizeUnits
		= {{"Bytes", "KiB", "MiB", "GiB", "TiB", "PiB", "EiB", "ZiB", "YiB"}};

	std::size_t Index;
	double      ByteSize = ByteCount;
	for( Index = 0; Index < SizeUnits.size(); Index++ )
	{
		if( ByteSize < 1_KiB )
			break;
		ByteSize /= 1_KiB;
	}
	const std::size_t Size
		= std::snprintf(nullptr, 0, "%.2f %s", ByteSize, SizeUnits.at(Index))
		+ 1;
	const auto Buffer = std::make_unique<char[]>(Size);
	std::snprintf(Buffer.get(), Size, "%.2f %s", ByteSize, SizeUnits.at(Index));
	return std::string(Buffer.get(), Buffer.get() + Size - 1);
}

std::string FormatVersion(std::uint32_t Version)
{
	const std::size_t Size
		= std::snprintf(
			  nullptr, 0, "%u.%u.%u", VK_VERSION_MAJOR(Version),
			  VK_VERSION_MINOR(Version), VK_VERSION_PATCH(Version))
		+ 1;
	const auto Buffer = std::make_unique<char[]>(Size);
	std::snprintf(
		Buffer.get(), Size, "%u.%u.%u", VK_VERSION_MAJOR(Version),
		VK_VERSION_MINOR(Version), VK_VERSION_PATCH(Version));
	return std::string(Buffer.get(), Buffer.get() + Size - 1);
}

template<Vulkan::Util::VendorID Vendor>
bool VendorDetails(
	FetchArt& Art, FetchLog& Fetch, const vk::PhysicalDevice& PhysicalDevice)
{
	return true;
}

template<>
bool VendorDetails<Vulkan::Util::VendorID::Unknown>(
	FetchArt& Art, FetchLog& Fetch, const vk::PhysicalDevice& PhysicalDevice)
{
	static const char* ASCII_ART[] = {
		"\033[2;31m        ████████████████                     ",
		"\033[2;31m    ██████████████████████████               ",
		"\033[2;31m  █████████████         ██████████           ",
		"\033[2;31m █████████                     ██████        ",
		"\033[2;31m ███████     ████      ████         ████     ",
		"\033[2;31m  █████      ████      ████            ███   ",
		"\033[2;31m   ████       ████    ████               ██  ",
		"\033[2;31m    ████      ████    ████                 █ ",
		"\033[2;31m      ███      ████  ████                    ",
		"\033[2;31m        ██     ████  ████                    ",
		"\033[2;31m                ████████                     ",
		"\033[2;31m                ████████                     ",
		"\033[2;31m                 ██████                      ",
	};
	const std::size_t ASCII_HEIGHT = std::extent_v<decltype(ASCII_ART)>;

	Art = FetchArt(ASCII_ART, ASCII_HEIGHT);
	return true;
}

template<>
bool VendorDetails<Vulkan::Util::VendorID::Intel>(
	FetchArt& Art, FetchLog& Fetch, const vk::PhysicalDevice& PhysicalDevice)
{
	static const char* ASCII_ART[] = {
		"\033[36m ███\033[37m                                  ███ ",
		"\033[36m ███\033[37m                                  ███ ",
		"\033[36m    \033[37m   ██ ████     ███      ██████    ███ ",
		"\033[37m ███   █████████   █████  ████  ████  ███ ",
		"\033[37m ███   ███    ███  ███    ███    ███  ███ ",
		"\033[37m ███   ███    ███  ███    ██████████  ███ ",
		"\033[37m ███   ███    ███  ███    ███         ███ ",
		"\033[37m ███   ███    ███  ███     ████████   ███ ",
		"\033[37m ███   ███    ███   █████    ████     ███ ",
	};
	const std::size_t ASCII_HEIGHT = std::extent_v<decltype(ASCII_ART)>;

	Art = FetchArt(ASCII_ART, ASCII_HEIGHT);
	return true;
}

template<>
bool VendorDetails<Vulkan::Util::VendorID::Nvidia>(
	FetchArt& Art, FetchLog& Fetch, const vk::PhysicalDevice& PhysicalDevice)
{
	const auto DevicePropertyChain = PhysicalDevice.getProperties2<
		vk::PhysicalDeviceProperties2,
		vk::PhysicalDeviceShaderSMBuiltinsPropertiesNV>();

	const auto SMBuiltinsProperties
		= DevicePropertyChain
			  .get<vk::PhysicalDeviceShaderSMBuiltinsPropertiesNV>();

	Fetch.push_back(FormatString(
						"    Streaming Multiprocessors: %u",
						SMBuiltinsProperties.shaderSMCount)
						.value());

	Fetch.push_back(
		FormatString(
			"    WarpsPerSM: %u", SMBuiltinsProperties.shaderWarpsPerSM)
			.value());

	static const char* ASCII_ART[] = {
		"\033[92m                     ████████████████████████   ",
		"\033[92m               ██████      ██████████████████   ",
		"\033[92m            ███      █████      █████████████   ",
		"\033[92m         ████   █████    █████     ██████████   ",
		"\033[92m       ████   ████   █       ████   █████████   ",
		"\033[92m     ████   ████     ███     ████    ████████   ",
		"\033[92m      ████   ████    █████████     ██████████   ",
		"\033[92m        ███    ███   █████      ████   ██████   ",
		"\033[92m         █████   ████       █████      ██████   ",
		"\033[92m           █████     ████████       █████████   ",
		"\033[92m               ██████          ██████████████   ",
		"\033[92m                     ████████████████████████   ",
		"\033[1;37m                   NVIDIA",
	};
	const std::size_t ASCII_HEIGHT = std::extent_v<decltype(ASCII_ART)>;

	Art = FetchArt(ASCII_ART, ASCII_HEIGHT);
	return true;
}

template<>
bool VendorDetails<Vulkan::Util::VendorID::AMD>(
	FetchArt& Art, FetchLog& Fetch, const vk::PhysicalDevice& PhysicalDevice)
{
	const auto DevicePropertyChain = PhysicalDevice.getProperties2<
		vk::PhysicalDeviceProperties2,
		vk::PhysicalDeviceShaderCorePropertiesAMD>();

	const auto ShaderCoreProperties
		= DevicePropertyChain.get<vk::PhysicalDeviceShaderCorePropertiesAMD>();

	Fetch.push_back(FormatString(
						"    Compute Units: %u",
						ShaderCoreProperties.shaderEngineCount
							* ShaderCoreProperties.shaderArraysPerEngineCount
							* ShaderCoreProperties.computeUnitsPerShaderArray)
						.value());

	static const char* ASCII_ART[] = {
		"\033[37m    ███     ███      ███ █████████  \033[2;32m  █████████",
		"\033[37m   █████    █████  █████ ███    ███ \033[2;32m    ███████",
		"\033[37m  ███ ███   ████████████ ███     ███\033[2;32m   █    ███",
		"\033[37m ███   ███  ███  ██  ███ ███     ███\033[2;32m  ██    ███",
		"\033[37m███████████ ███      ███ ███    ███ \033[2;32m ███████ ██",
		"\033[37m███     ███ ███      ███ █████████  \033[2;32m █████    █",
	};
	const std::size_t ASCII_HEIGHT = std::extent_v<decltype(ASCII_ART)>;

	Art = FetchArt(ASCII_ART, ASCII_HEIGHT);
	return true;
}

bool FetchDevice(const vk::PhysicalDevice& PhysicalDevice)
{
	const auto DevicePropertyChain = PhysicalDevice.getProperties2<
		vk::PhysicalDeviceProperties2, vk::PhysicalDeviceDriverProperties>();

	const auto& DeviceProperties
		= DevicePropertyChain.get<vk::PhysicalDeviceProperties2>();
	const auto& DeviceDriverProperties
		= DevicePropertyChain.get<vk::PhysicalDeviceDriverProperties>();

	const auto MemoryPropertyChain = PhysicalDevice.getMemoryProperties2<
		vk::PhysicalDeviceMemoryProperties2,
		vk::PhysicalDeviceMemoryBudgetPropertiesEXT>();
	const auto& MemoryProperties
		= MemoryPropertyChain.get<vk::PhysicalDeviceMemoryProperties2>();

	const auto& MemoryBudgetProperties
		= MemoryPropertyChain
			  .get<vk::PhysicalDeviceMemoryBudgetPropertiesEXT>();

	/// Get device-local heap

	const std::uint32_t VRAMHeapIndex
		= Vulkan::Util::FindVRAMHeapIndex(MemoryProperties.memoryProperties)
			  .value_or(0);

	// The heap budget is how much the current process is allowed to allocate
	// from the heap. We compare it to the total amount of memory available
	// to determine how much "usable" free memory there is left
	const vk::DeviceSize MemTotal
		= MemoryProperties.memoryProperties.memoryHeaps[VRAMHeapIndex].size;
	const vk::DeviceSize MemUsed
		= MemTotal - MemoryBudgetProperties.heapBudget[VRAMHeapIndex];

	FetchLog Fetch = {};
	FetchArt Art   = {};

	Fetch.push_back(
		FormatString(
			"\033[1m%s\033[0m : %s",
			DeviceProperties.properties.deviceName.data(),
			vk::to_string(DeviceProperties.properties.deviceType).c_str())
			.value());

	Fetch.push_back(
		FormatString(
			"    Device: %04x:%04x (%s)", DeviceProperties.properties.deviceID,
			DeviceProperties.properties.vendorID,
			Vulkan::Util::VendorName(static_cast<Vulkan::Util::VendorID>(
				DeviceProperties.properties.vendorID)))
			.value());

	Fetch.push_back(FormatString(
						"    Driver: %s : %s",
						DeviceDriverProperties.driverName.data(),
						DeviceDriverProperties.driverInfo.data())
						.value());

	Fetch.push_back(
		FormatString(
			"    API: %s",
			FormatVersion(DeviceProperties.properties.apiVersion).c_str())
			.value());

	const std::float_t MemoryPressure
		= MemUsed / static_cast<std::float_t>(MemTotal);

	static const char* PressureColors[] = {"\033[92m", "\033[93m", "\033[91m"};

	const char* PressureColor;
	if( MemoryPressure < 0.5f )
	{
		PressureColor = PressureColors[0];
	}
	else if( MemoryPressure < 0.75f )
	{
		PressureColor = PressureColors[1];
	}
	else
	{
		PressureColor = PressureColors[2];
	}

	Fetch.push_back(FormatString(
						"    VRAM: %s%s\033[0m / %s", PressureColor,
						FormatByteCount(MemUsed).c_str(),
						FormatByteCount(MemTotal).c_str())
						.value());

	Fetch.push_back(FormatString(
						"    %s %%%s%3.2f\033[0m",
						FormatMeter(30, MemoryPressure).value().c_str(),
						PressureColor, MemoryPressure * 100.0f)
						.value());

	switch( static_cast<Vulkan::Util::VendorID>(
		DeviceProperties.properties.vendorID) )
	{
	case Vulkan::Util::VendorID::AMD:
	{
		VendorDetails<Vulkan::Util::VendorID::AMD>(Art, Fetch, PhysicalDevice);
		break;
	}
	case Vulkan::Util::VendorID::Nvidia:
	{
		VendorDetails<Vulkan::Util::VendorID::Nvidia>(
			Art, Fetch, PhysicalDevice);
		break;
	}
	case Vulkan::Util::VendorID::Intel:
	{
		VendorDetails<Vulkan::Util::VendorID::Intel>(
			Art, Fetch, PhysicalDevice);
		break;
	}
	default:
	case Vulkan::Util::VendorID::Unknown:
	{
		VendorDetails<Vulkan::Util::VendorID::Unknown>(
			Art, Fetch, PhysicalDevice);
		break;
	}
	}

	for( std::size_t CurLine = 0; CurLine < std::max(Art.size(), Fetch.size());
		 ++CurLine )
	{
		std::printf(
			" %-47s\033[0m %s\n", CurLine < Art.size() ? Art[CurLine] : "",
			CurLine < Fetch.size() ? Fetch[CurLine].c_str() : "");
	}
	std::putchar('\n');

	return true;
}

int main()
{

	vk::InstanceCreateInfo InstanceInfo = {};

	vk::ApplicationInfo ApplicationInfo = {};
	ApplicationInfo.apiVersion          = VK_API_VERSION_1_2;
	ApplicationInfo.applicationVersion  = VK_MAKE_VERSION(1, 0, 0);
	ApplicationInfo.engineVersion       = VK_MAKE_VERSION(1, 0, 0);

	InstanceInfo.pApplicationInfo = &ApplicationInfo;

	vk::UniqueInstance Instance = {};

	if( auto CreateResult = vk::createInstanceUnique(InstanceInfo);
		CreateResult.result == vk::Result::eSuccess )
	{
		Instance = std::move(CreateResult.value);
	}

	if( const auto EnumResult = Instance->enumeratePhysicalDevices();
		EnumResult.result == vk::Result::eSuccess )
	{
		for( const auto& CurDevice : EnumResult.value )
		{
			FetchDevice(CurDevice);
		}
	}

	return EXIT_SUCCESS;
}
