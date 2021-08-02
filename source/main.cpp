#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <optional>

#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan.hpp>

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

using FetchLog = std::vector<std::string>;

enum class VendorID : std::uint32_t
{
	AMD      = 0x1002,
	ImgTec   = 0x1010,
	Nvidia   = 0x10DE,
	ARM      = 0x13B5,
	Qualcomm = 0x5143,
	Intel    = 0x8086
};

const char* VendorName(VendorID Vendor)
{
	switch( Vendor )
	{
	case VendorID::AMD:
	{
		return "AMD";
	}
	case VendorID::ImgTec:
	{
		return "ImgTec";
	}
	case VendorID::Nvidia:
	{
		return "Nvidia";
	}
	case VendorID::ARM:
	{
		return "ARM";
	}
	case VendorID::Qualcomm:
	{
		return "Qualcomm";
	}
	case VendorID::Intel:
	{
		return "Intel";
	}
	}
	return "Unknown";
}

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
	return std::to_string(ByteSize) + " " + SizeUnits.at(Index);
}

std::optional<std::uint32_t> FindVRAMHeapIndex(
	const vk::PhysicalDeviceMemoryProperties& MemoryProperties)
{
	for( std::uint32_t Index = 0; Index < MemoryProperties.memoryHeapCount;
		 ++Index )
	{
		if( (MemoryProperties.memoryHeaps[Index].flags
			 & vk::MemoryHeapFlagBits::eDeviceLocal)
			== vk::MemoryHeapFlagBits::eDeviceLocal )
		{
			return Index;
		}
	}
	return std::nullopt;
}

std::string FormatVersion(std::uint32_t Version)
{
	return std::to_string(VK_VERSION_MAJOR(Version)) + "."
		 + std::to_string(VK_VERSION_MINOR(Version)) + "."
		 + std::to_string(VK_VERSION_PATCH(Version));
}

template<VendorID Vendor>
bool VendorDetails(FetchLog& Fetch, const vk::PhysicalDevice& PhysicalDevice)
{
	return true;
}

template<>
bool VendorDetails<VendorID::Nvidia>(
	FetchLog& Fetch, const vk::PhysicalDevice& PhysicalDevice)
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

	return true;
}

template<>
bool VendorDetails<VendorID::AMD>(
	FetchLog& Fetch, const vk::PhysicalDevice& PhysicalDevice)
{
	const auto DevicePropertyChain = PhysicalDevice.getProperties2<
		vk::PhysicalDeviceProperties2,
		vk::PhysicalDeviceShaderCorePropertiesAMD,
		vk::PhysicalDeviceShaderCoreProperties2AMD>();

	const auto ShaderCoreProperties
		= DevicePropertyChain.get<vk::PhysicalDeviceShaderCorePropertiesAMD>();
	const auto ShaderCoreProperties2
		= DevicePropertyChain.get<vk::PhysicalDeviceShaderCoreProperties2AMD>();

	Fetch.push_back(FormatString(
						"    Compute Units: %u",
						ShaderCoreProperties.shaderEngineCount
							* ShaderCoreProperties.shaderArraysPerEngineCount
							* ShaderCoreProperties.computeUnitsPerShaderArray)
						.value());

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
		= FindVRAMHeapIndex(MemoryProperties.memoryProperties).value_or(0);

	// The heap budget is how much the current process is allowed to allocate
	// from the heap. We compare it to the total amount of memory available
	// to determine how much "usable" free memory there is left
	const vk::DeviceSize MemTotal
		= MemoryProperties.memoryProperties.memoryHeaps[VRAMHeapIndex].size;
	const vk::DeviceSize MemUsed
		= MemTotal - MemoryBudgetProperties.heapBudget[VRAMHeapIndex];

	FetchLog Fetch;

	Fetch.push_back(
		FormatString(
			"%s : %s", DeviceProperties.properties.deviceName.data(),
			vk::to_string(DeviceProperties.properties.deviceType).c_str())
			.value());

	Fetch.push_back(
		FormatString(
			"    Vendor: %4x : (%s)", DeviceProperties.properties.vendorID,
			VendorName(
				static_cast<VendorID>(DeviceProperties.properties.vendorID)))
			.value());

	Fetch.push_back(
		FormatString(
			"    API: %s",
			FormatVersion(DeviceProperties.properties.apiVersion).c_str())
			.value());

	Fetch.push_back(FormatString(
						"    Driver: %s : %s",
						DeviceDriverProperties.driverName.data(),
						DeviceDriverProperties.driverInfo.data())
						.value());

	Fetch.push_back(FormatString(
						"    VRAM: %s / %s : %%%f",
						FormatByteCount(MemUsed).c_str(),
						FormatByteCount(MemTotal).c_str(),
						MemUsed / static_cast<std::float_t>(MemTotal))
						.value());

	switch( static_cast<VendorID>(DeviceProperties.properties.vendorID) )
	{
	case VendorID::AMD:
	{
		VendorDetails<VendorID::AMD>(Fetch, PhysicalDevice);
		break;
	}
	case VendorID::Nvidia:
	{
		VendorDetails<VendorID::Nvidia>(Fetch, PhysicalDevice);
		break;
	}
	case VendorID::Intel:
	{
		VendorDetails<VendorID::Intel>(Fetch, PhysicalDevice);
		break;
	}
	}

	const char* ASCII[]
		= {"# # # # #   # #  #  ## #", "# # # # #   ##  ### ## #",
		   " #  ### ### # # # # # ##"};
	const std::size_t ASCII_HEIGHT = std::extent_v<decltype(ASCII)>;
	std::size_t       CurLine      = 0;

	for( const auto& Line : Fetch )
	{
		std::printf(
			"\e[1;31m%-40s\e[0m %s\n",
			CurLine < ASCII_HEIGHT ? ASCII[CurLine] : "", Line.c_str());

		++CurLine;
	}

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