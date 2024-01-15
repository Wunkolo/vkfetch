#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <optional>
#include <span>

#include <fmt/core.h>

#include <VulkanConfig.hpp>
#include <VulkanUtil.hpp>

#include <Format.hpp>

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

using FetchLog = std::vector<std::string>;
using FetchArt = std::span<const char*>;

using FetchStyle = std::array<const char*, 4>;

template<Vulkan::Util::VendorID Vendor>
bool VendorDetails(
	FetchArt& Art, FetchStyle& Style, FetchLog& Fetch,
	const vk::PhysicalDevice& PhysicalDevice
)
{
	return true;
}

template<>
bool VendorDetails<Vulkan::Util::VendorID::Unknown>(
	FetchArt& Art, FetchStyle& Style, FetchLog& Fetch,
	const vk::PhysicalDevice& PhysicalDevice
)
{
	static std::array ASCII_ART = std::to_array<const char*>({
		// clang-format off
		"       ################                    ",
		"   ##########################              ",
		" #############         ##########          ",
		"#########                     ######       ",
		"#######     ####      ####         ####    ",
		" #####      ####      ####            ###  ",
		"  ####       ####    ####               ## ",
		"   ####      ####    ####                 #",
		"     ###      ####  ####                   ",
		"       ##     ####  ####                   ",
		"               ########                    ",
		"               ########                    ",
		"                ######                     ",
		// clang-format on
	});

	Art = ASCII_ART;

	Style[0] = "\033[2;31m";
	return true;
}

template<>
bool VendorDetails<Vulkan::Util::VendorID::Apple>(
	FetchArt& Art, FetchStyle& Style, FetchLog& Fetch,
	const vk::PhysicalDevice& PhysicalDevice
)
{
	static std::array ASCII_ART = std::to_array<const char*>({
		// clang-format off
		"                 ###        ",
		"               ####         ",
		"              ####          ",
		"     ####### ###  #######   ",
		"   ######################## ",
		"  ########################  ",
		" #######################    ",
		"#######################     ",
		"#######################     ",
		"########################    ",
		" #########################  ",
		"  ##########################",
		"    ######################  ",
		"     ###################    ",
		"       ######    #####      ",
		// clang-format on
	});

	Art = ASCII_ART;

	Style[0] = "\033[37m";
	return true;
}

template<>
bool VendorDetails<Vulkan::Util::VendorID::Google>(
	FetchArt& Art, FetchStyle& Style, FetchLog& Fetch,
	const vk::PhysicalDevice& PhysicalDevice
)
{
	static std::array ASCII_ART = std::to_array<const char*>({
		// clang-format off
		"            #########          ",
		"        #################      ",
		"      #####################    ",
		"    &#########    #######      ",
		"  &&&&#####          ##        ",
		" &&&&&&&#                      ",
		" &&&&&&&                       ",
		"&&&&&&&          $$$$$$$$$$$$$$",
		"&&&&&&&          $$$$$$$$$$$$$$",
		"&&&&&&&          $$$$$$$$$$$$$$",
		" &&&&&&&                $$$$$$$",
		" &&&&&&&%              $$$$$$$ ",
		"  &&&&%%%%%          %$$$$$$$$ ",
		"    &%%%%%%%%%    %%%%%$$$$$   ",
		"      %%%%%%%%%%%%%%%%%%$$     ",
		"        %%%%%%%%%%%%%%%%%      ",
		"            %%%%%%%%%%         ",
		// clang-format on
	});

	Art = ASCII_ART;

	Style[0] = "\033[31m";
	Style[1] = "\033[34m";
	Style[2] = "\033[32m";
	Style[3] = "\033[33m";
	return true;
}

template<>
bool VendorDetails<Vulkan::Util::VendorID::Intel>(
	FetchArt& Art, FetchStyle& Style, FetchLog& Fetch,
	const vk::PhysicalDevice& PhysicalDevice
)
{
	static std::array ASCII_ART = std::to_array<const char*>({
		// clang-format off
		"$$$                                  ###",
		"$$$                                  ###",
		"      ## ####     ###      ######    ###",
		"###   #########   #####  ####  ####  ###",
		"###   ###    ###  ###    ###    ###  ###",
		"###   ###    ###  ###    ##########  ###",
		"###   ###    ###  ###    ###         ###",
		"###   ###    ###  ###     ########   ###",
		"###   ###    ###   #####    ####     ###",
		// clang-format on
	});

	Art = ASCII_ART;

	Style[0] = "\033[37m";
	Style[1] = "\033[36m";
	return true;
}

template<>
bool VendorDetails<Vulkan::Util::VendorID::Nvidia>(
	FetchArt& Art, FetchStyle& Style, FetchLog& Fetch,
	const vk::PhysicalDevice& PhysicalDevice
)
{
	const auto DevicePropertyChain = PhysicalDevice.getProperties2<
		vk::PhysicalDeviceProperties2,
		vk::PhysicalDeviceShaderSMBuiltinsPropertiesNV>();

	const auto SMBuiltinsProperties
		= DevicePropertyChain
			  .get<vk::PhysicalDeviceShaderSMBuiltinsPropertiesNV>();

	// clang-format off
	Fetch.push_back(Format::FormatString("    Streaming Multiprocessors:\033[37m %u", SMBuiltinsProperties.shaderSMCount).value());
	Fetch.push_back(Format::FormatString("    Warps Per SM:\033[37m %u", SMBuiltinsProperties.shaderWarpsPerSM).value());
	// clang-format on

	static std::array ASCII_ART = std::to_array<const char*>({
		// clang-format off
		"                     ########################",
		"               ######      ##################",
		"            ###      #####      #############",
		"         ####   #####    #####     ##########",
		"       ####   ####   #       ####   #########",
		"     ####   ####     ###     ####    ########",
		"      ####   ####    #########     ##########",
		"        ###    ###   #####      ####   ######",
		"         #####   ####       #####      ######",
		"           #####     ########       #########",
		"               ######          ##############",
		"                     ########################",
		"                   NVIDIA",
		// clang-format on
	});

	Art = ASCII_ART;

	Style[0] = "\033[92m";
	return true;
}

template<>
bool VendorDetails<Vulkan::Util::VendorID::AMD>(
	FetchArt& Art, FetchStyle& Style, FetchLog& Fetch,
	const vk::PhysicalDevice& PhysicalDevice
)
{
	const auto DevicePropertyChain = PhysicalDevice.getProperties2<
		vk::PhysicalDeviceProperties2,
		vk::PhysicalDeviceShaderCorePropertiesAMD,
		vk::PhysicalDeviceShaderCoreProperties2AMD>();

	const auto ShaderCoreProperties
		= DevicePropertyChain.get<vk::PhysicalDeviceShaderCorePropertiesAMD>();

	const auto ShaderCoreProperties2
		= DevicePropertyChain.get<vk::PhysicalDeviceShaderCoreProperties2AMD>();

	const auto ActiveComputeUnits
		= ShaderCoreProperties2.activeComputeUnitCount;

	const auto TotalComputeUnits
		= ShaderCoreProperties.shaderEngineCount
		* ShaderCoreProperties.shaderArraysPerEngineCount
		* ShaderCoreProperties.computeUnitsPerShaderArray;

	Fetch.push_back(Format::FormatString(
						"    Compute Units:\033[37m %u\033[0m / %u",
						ActiveComputeUnits, TotalComputeUnits
	)
						.value());

	// clang-format off
	Fetch.push_back(Format::FormatString("    ShaderEngines:\033[37m %u", ShaderCoreProperties.shaderEngineCount).value());
	Fetch.push_back(Format::FormatString("    ShaderArraysPerEngineCount:\033[37m %u", ShaderCoreProperties.shaderArraysPerEngineCount).value());
	Fetch.push_back(Format::FormatString("    ComputeUnitsPerShaderArray:\033[37m %u", ShaderCoreProperties.computeUnitsPerShaderArray).value());
	Fetch.push_back(Format::FormatString("    SimdPerComputeUnit:\033[37m %u", ShaderCoreProperties.simdPerComputeUnit).value());
	Fetch.push_back(Format::FormatString("    WavefrontsPerSimd:\033[37m %u", ShaderCoreProperties.wavefrontsPerSimd).value());
	Fetch.push_back(Format::FormatString("    WavefrontSize:\033[37m %u", ShaderCoreProperties.wavefrontSize).value());
	// clang-format on

	static std::array ASCII_ART = std::to_array<const char*>({
		// clang-format off
		"    ###     ###      ### #########    $$$$$$$$$",
		"   #####    #####  ##### ###    ###     $$$$$$$",
		"  ### ###   ############ ###     ###   $    $$$",
		" ###   ###  ###  ##  ### ###     ###  $$    $$$",
		"########### ###      ### ###    ###  $$$$$$$ $$",
		"###     ### ###      ### #########   $$$$$    $",
		// clang-format on
	});

	Art = ASCII_ART;

	Style[0] = "\033[37m";
	Style[1] = "\033[2;32m";
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
		= MemoryPropertyChain.get<vk::PhysicalDeviceMemoryBudgetPropertiesEXT>(
		);

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

	FetchLog   Fetch = {};
	FetchArt   Art   = {};
	FetchStyle Style = {nullptr, nullptr, nullptr, nullptr};

	Fetch.push_back(
		Format::FormatString(
			"\033[1m%s\033[0m : %s",
			DeviceProperties.properties.deviceName.data(),
			vk::to_string(DeviceProperties.properties.deviceType).c_str()
		)
			.value()
	);

	Fetch.push_back(
		Format::FormatString(
			"    Device: \033[37m%04x\033[0m : \033[37m%04x\033[0m (%s)",
			DeviceProperties.properties.deviceID,
			DeviceProperties.properties.vendorID,
			Vulkan::Util::VendorName(static_cast<Vulkan::Util::VendorID>(
				DeviceProperties.properties.vendorID
			))
		)
			.value()
	);

	Fetch.push_back(Format::FormatString(
						"    Driver: \033[37m%s\033[0m : \033[37m%s\033[0m",
						DeviceDriverProperties.driverName.data(),
						DeviceDriverProperties.driverInfo.data()
	)
						.value());

	Fetch.push_back(
		Format::FormatString(
			"    API: \033[37m%s",
			Format::FormatVersion(DeviceProperties.properties.apiVersion)
				.c_str()
		)
			.value()
	);

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

	Fetch.push_back(Format::FormatString(
						"    VRAM: %s%s\033[0m / %s", PressureColor,
						Format::FormatByteCount(MemUsed).c_str(),
						Format::FormatByteCount(MemTotal).c_str()
	)
						.value());

	Fetch.push_back(Format::FormatString(
						"    %s %%%s% 3.2f\033[0m",
						Format::FormatMeter(30, MemoryPressure).value().c_str(),
						PressureColor, MemoryPressure * 100.0f
	)
						.value());

	switch( static_cast<Vulkan::Util::VendorID>(
		DeviceProperties.properties.vendorID
	) )
	{
	case Vulkan::Util::VendorID::AMD:
	{
		VendorDetails<Vulkan::Util::VendorID::AMD>(
			Art, Style, Fetch, PhysicalDevice
		);
		break;
	}
	case Vulkan::Util::VendorID::Apple:
	{
		VendorDetails<Vulkan::Util::VendorID::Apple>(
			Art, Style, Fetch, PhysicalDevice
		);
		break;
	}
	case Vulkan::Util::VendorID::Nvidia:
	{
		VendorDetails<Vulkan::Util::VendorID::Nvidia>(
			Art, Style, Fetch, PhysicalDevice
		);
		break;
	}
	case Vulkan::Util::VendorID::Google:
	{
		VendorDetails<Vulkan::Util::VendorID::Google>(
			Art, Style, Fetch, PhysicalDevice
		);
		break;
	}
	case Vulkan::Util::VendorID::Intel:
	{
		VendorDetails<Vulkan::Util::VendorID::Intel>(
			Art, Style, Fetch, PhysicalDevice
		);
		break;
	}
	default:
	case Vulkan::Util::VendorID::Unknown:
	{
		VendorDetails<Vulkan::Util::VendorID::Unknown>(
			Art, Style, Fetch, PhysicalDevice
		);
		break;
	}
	}

	std::size_t ArtWidth = 0;
	for( const auto& Line : Art )
	{
		ArtWidth = std::max(ArtWidth, std::strlen(Line));
	}

	for( std::size_t CurLine = 0; CurLine < std::max(Art.size(), Fetch.size());
		 ++CurLine )
	{
		std::string ArtLine(CurLine < Art.size() ? Art[CurLine] : "");
		// Convert '#" to reverse-video blocks
		if( Style[0] )
			ArtLine = Format::ReplaceString(
				ArtLine, "#",
				Format::FormatString("%s\033[7m \033[0m", Style[0]).value()
			);
		if( Style[1] )
			ArtLine = Format::ReplaceString(
				ArtLine, "$",
				Format::FormatString("%s\033[7m \033[0m", Style[1]).value()
			);
		if( Style[2] )
			ArtLine = Format::ReplaceString(
				ArtLine, "%",
				Format::FormatString("%s\033[7m \033[0m", Style[2]).value()
			);
		if( Style[3] )
			ArtLine = Format::ReplaceString(
				ArtLine, "&",
				Format::FormatString("%s\033[7m \033[0m", Style[3]).value()
			);

		std::printf(
			" %-*s\033[0m %s\n", ArtWidth, ArtLine.c_str(),
			CurLine < Fetch.size() ? Fetch[CurLine].c_str() : ""
		);
	}
	std::putchar('\n');

	return true;
}

int main()
{

	vk::InstanceCreateInfo InstanceInfo = {};

	static const std::array InstanceExtensions = std::to_array<const char*>({
		VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
	});
	InstanceInfo.setPEnabledExtensionNames(InstanceExtensions);

	InstanceInfo.flags |= vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR;

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
	else
	{
		std::fprintf(
			stderr, "Error creating vulkan instance: %s\n",
			vk::to_string(CreateResult.result).c_str()
		);
		return EXIT_FAILURE;
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
