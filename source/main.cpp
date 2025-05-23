#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <optional>
#include <span>
#include <string_view>

#include <fmt/core.h>

#include <VulkanConfig.hpp>
#include <VulkanUtil.hpp>

#include <Format.hpp>

#include "VendorArt.hpp"

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
using FetchArt = std::span<std::string_view>;

using FetchStyle = std::array<std::string_view, 4>;

using namespace std::string_view_literals;

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
	Art = VendorArt::Vulkan;

	Style[0] = "\033[2;31m"sv;
	return true;
}

template<>
bool VendorDetails<Vulkan::Util::VendorID::Apple>(
	FetchArt& Art, FetchStyle& Style, FetchLog& Fetch,
	const vk::PhysicalDevice& PhysicalDevice
)
{
	Art = VendorArt::Apple;

	Style[0] = "\033[37m"sv;
	return true;
}

template<>
bool VendorDetails<Vulkan::Util::VendorID::Microsoft>(
	FetchArt& Art, FetchStyle& Style, FetchLog& Fetch,
	const vk::PhysicalDevice& PhysicalDevice
)
{
	Art = VendorArt::Microsoft;

	Style[0] = "\033[31m"sv;
	Style[1] = "\033[32m"sv;
	Style[2] = "\033[34m"sv;
	Style[3] = "\033[33m"sv;
	return true;
}

template<>
bool VendorDetails<Vulkan::Util::VendorID::Google>(
	FetchArt& Art, FetchStyle& Style, FetchLog& Fetch,
	const vk::PhysicalDevice& PhysicalDevice
)
{
	Art = VendorArt::Google;

	Style[0] = "\033[31m"sv;
	Style[1] = "\033[34m"sv;
	Style[2] = "\033[32m"sv;
	Style[3] = "\033[33m"sv;
	return true;
}

template<>
bool VendorDetails<Vulkan::Util::VendorID::Qualcomm>(
	FetchArt& Art, FetchStyle& Style, FetchLog& Fetch,
	const vk::PhysicalDevice& PhysicalDevice
)
{
	Art = VendorArt::Qualcomm;

	Style[0] = "\033[34m"sv;
	return true;
}

template<>
bool VendorDetails<Vulkan::Util::VendorID::QualcommPartner>(
	FetchArt& Art, FetchStyle& Style, FetchLog& Fetch,
	const vk::PhysicalDevice& PhysicalDevice
)
{
	return VendorDetails<Vulkan::Util::VendorID::Qualcomm>(
		Art, Style, Fetch, PhysicalDevice
	);
}

template<>
bool VendorDetails<Vulkan::Util::VendorID::Intel>(
	FetchArt& Art, FetchStyle& Style, FetchLog& Fetch,
	const vk::PhysicalDevice& PhysicalDevice
)
{
	Art = VendorArt::Intel;

	Style[0] = "\033[37m"sv;
	Style[1] = "\033[36m"sv;
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
	Fetch.push_back(fmt::format("    Streaming Multiprocessors:\033[37m {}"sv, SMBuiltinsProperties.shaderSMCount));
	Fetch.push_back(fmt::format("    Warps Per SM:\033[37m {}"sv, SMBuiltinsProperties.shaderWarpsPerSM));
	// clang-format on

	Art = VendorArt::Nvidia;

	Style[0] = "\033[92m"sv;
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

	Fetch.push_back(fmt::format(
		"    Compute Units:\033[37m {}\033[0m / {}"sv, ActiveComputeUnits,
		TotalComputeUnits
	));

	// clang-format off
	Fetch.push_back(fmt::format("    ShaderEngines:\033[37m {}"sv, ShaderCoreProperties.shaderEngineCount));
	Fetch.push_back(fmt::format("    ShaderArraysPerEngineCount:\033[37m {}"sv, ShaderCoreProperties.shaderArraysPerEngineCount));
	Fetch.push_back(fmt::format("    ComputeUnitsPerShaderArray:\033[37m {}"sv, ShaderCoreProperties.computeUnitsPerShaderArray));
	Fetch.push_back(fmt::format("    SimdPerComputeUnit:\033[37m {}"sv, ShaderCoreProperties.simdPerComputeUnit));
	Fetch.push_back(fmt::format("    WavefrontsPerSimd:\033[37m {}"sv, ShaderCoreProperties.wavefrontsPerSimd));
	Fetch.push_back(fmt::format("    WavefrontSize:\033[37m {}"sv, ShaderCoreProperties.wavefrontSize));
	// clang-format on

	Art = VendorArt::AMD;

	Style[0] = "\033[37m"sv;
	Style[1] = "\033[2;32m"sv;
	return true;
}

bool HasExtension(
	vk::PhysicalDevice PhysicalDevice, std::string_view ExtensionName
)
{
	if( const auto EnumerateResult
		= PhysicalDevice.enumerateDeviceExtensionProperties();
		EnumerateResult.result == vk::Result::eSuccess )
	{
		for( const auto& ExtensionProperties : EnumerateResult.value )
		{
			if( ExtensionName.compare(ExtensionProperties.extensionName.data())
				== 0 )
			{
				return true;
			}
		}
	}
	return false;
}

std::optional<vk::DeviceSize>
	GetHeapBudget(vk::PhysicalDevice PhysicalDevice, std::uint32_t HeapIndex)
{
	// Requires `VK_EXT_memory_budget`
	if( !HasExtension(PhysicalDevice, VK_EXT_MEMORY_BUDGET_EXTENSION_NAME) )
	{
		return {};
	}

	const auto MemoryPropertyChain = PhysicalDevice.getMemoryProperties2<
		vk::PhysicalDeviceMemoryProperties2,
		vk::PhysicalDeviceMemoryBudgetPropertiesEXT>();

	const vk::PhysicalDeviceMemoryBudgetPropertiesEXT& MemoryBudgetProperties
		= MemoryPropertyChain.get<vk::PhysicalDeviceMemoryBudgetPropertiesEXT>(
		);

	const vk::DeviceSize HeapBudget
		= MemoryBudgetProperties.heapBudget[HeapIndex];

	return HeapBudget;
}

bool FetchDevice(const vk::PhysicalDevice& PhysicalDevice)
{
	const auto DevicePropertyChain = PhysicalDevice.getProperties2<
		vk::PhysicalDeviceProperties2, vk::PhysicalDeviceDriverProperties>();

	const auto& DeviceProperties
		= DevicePropertyChain.get<vk::PhysicalDeviceProperties2>();
	const auto& DeviceDriverProperties
		= DevicePropertyChain.get<vk::PhysicalDeviceDriverProperties>();

	FetchLog   Fetch = {};
	FetchArt   Art   = {};
	FetchStyle Style = {};

	Fetch.push_back(fmt::format(
		"\033[1m{}\033[0m : {}"sv,
		DeviceProperties.properties.deviceName.data(),
		vk::to_string(DeviceProperties.properties.deviceType)
	));

	Fetch.push_back(fmt::format(
		"    Device: \033[37m{:04x}\033[0m : \033[37m{:04x}\033[0m ({})"sv,
		DeviceProperties.properties.deviceID,
		DeviceProperties.properties.vendorID,
		Vulkan::Util::VendorName(static_cast<Vulkan::Util::VendorID>(
			DeviceProperties.properties.vendorID
		))
	));

	Fetch.push_back(fmt::format(
		"    Driver: \033[37m{}\033[0m : \033[37m{}\033[0m"sv,
		DeviceDriverProperties.driverName.data(),
		DeviceDriverProperties.driverInfo.data()
	));

	Fetch.push_back(fmt::format(
		"    API: \033[37m{}"sv,
		Format::FormatVersion(DeviceProperties.properties.apiVersion)
	));

	/// Get the device-local heap that most-indicates the available VRAM
	const auto&         MemoryProperties = PhysicalDevice.getMemoryProperties();
	const std::uint32_t VRAMHeapIndex
		= Vulkan::Util::FindVRAMHeapIndex(MemoryProperties).value_or(0);

	const vk::DeviceSize HeapSize
		= PhysicalDevice.getMemoryProperties().memoryHeaps[VRAMHeapIndex].size;

	std::float_t MemoryPressure
		= std::numeric_limits<std::float_t>::quiet_NaN();

	const auto HeapBudget = GetHeapBudget(PhysicalDevice, VRAMHeapIndex);
	if( HeapBudget.has_value() )
	{

		// The budget is the amount of memory that the current process is
		// allowed to allocate. We compare this to the size of the entire heap
		// to approximate the amount of globally used memory
		MemoryPressure = (HeapSize - HeapBudget.value())
					   / static_cast<std::float_t>(HeapSize);
	}

	static std::string_view PressureColors[] = {
		"\033[92m"sv,
		"\033[93m"sv,
		"\033[91m"sv,
	};

	std::string_view PressureColor;
	if( std::isfinite(MemoryPressure) )
	{

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
	}
	else
	{
		PressureColor = "\033[90m"sv;
	}

	Fetch.push_back(fmt::format(
		"    VRAM: {}{}\033[0m / {}"sv, PressureColor,
		HeapBudget.has_value() ? Format::FormatByteCount(HeapBudget.value())
							   : "???"sv,
		Format::FormatByteCount(HeapSize)
	));

	Fetch.push_back(fmt::format(
		"    {} % {}{:3.2f}\033[0m"sv,
		Format::FormatMeter(30, MemoryPressure).value(), PressureColor,
		MemoryPressure * 100.0f
	));

	switch( static_cast<Vulkan::Util::VendorID>(
		DeviceProperties.properties.vendorID
	) )
	{
	// "Unknown" is the first vendor
	default:
#define VENDOR(_, VendorName)                                                  \
	case Vulkan::Util::VendorID::VendorName:                                   \
		VendorDetails<Vulkan::Util::VendorID::VendorName>(                     \
			Art, Style, Fetch, PhysicalDevice                                  \
		);                                                                     \
		break;

#include "Vendors.inc"
#undef VENDOR
	}

	std::size_t ArtWidth = 0;
	for( const auto& Line : Art )
	{
		ArtWidth = std::max(ArtWidth, Line.size());
	}

	for( std::size_t CurLine = 0; CurLine < std::max(Art.size(), Fetch.size());
		 ++CurLine )
	{
		std::string ArtLine(CurLine < Art.size() ? Art[CurLine] : "");
		// Convert '#" to reverse-video blocks
		if( !Style[0].empty() )
			ArtLine = Format::ReplaceString(
				ArtLine, "#"sv, fmt::format("{}\033[7m \033[0m"sv, Style[0])
			);
		if( !Style[1].empty() )
			ArtLine = Format::ReplaceString(
				ArtLine, "$"sv, fmt::format("{}\033[7m \033[0m"sv, Style[1])
			);
		if( !Style[2].empty() )
			ArtLine = Format::ReplaceString(
				ArtLine, "%"sv, fmt::format("{}\033[7m \033[0m"sv, Style[2])
			);
		if( !Style[3].empty() )
			ArtLine = Format::ReplaceString(
				ArtLine, "&"sv, fmt::format("{}\033[7m \033[0m"sv, Style[3])
			);

		fmt::println(
			" {:<{}}\033[0m {}"sv, ArtLine, ArtWidth,
			CurLine < Fetch.size() ? Fetch[CurLine] : ""
		);
	}
	std::putchar('\n');

	return true;
}

int main()
{
	static const vk::ApplicationInfo ApplicationInfo = {
		.pApplicationName   = "vkfetch",
		.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
		.pEngineName        = "vkfetch",
		.engineVersion      = VK_MAKE_VERSION(1, 0, 0),
		.apiVersion         = VK_API_VERSION_1_2,
	};

	static const std::array InstanceExtensions = std::to_array<const char*>({
		VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
	});

	const vk::InstanceCreateInfo InstanceInfo = {
		.flags = vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR,
		.pApplicationInfo        = &ApplicationInfo,
		.enabledExtensionCount   = InstanceExtensions.size(),
		.ppEnabledExtensionNames = InstanceExtensions.data(),
	};

	vk::UniqueInstance Instance = {};

	if( auto CreateResult = vk::createInstanceUnique(InstanceInfo);
		CreateResult.result == vk::Result::eSuccess )
	{
		Instance = std::move(CreateResult.value);
	}
	else
	{
		fmt::println(
			stderr, "Error creating vulkan instance: {}\n"sv,
			vk::to_string(CreateResult.result)
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
