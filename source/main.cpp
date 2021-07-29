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
	static std::array<const char*, 9> SizeUnits = {
		{"Bytes", "KiB", "MiB", "GiB", "TiB", "PiB", "EiB", "ZiB", "YiB"}};

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
		if( (MemoryProperties.memoryHeaps[Index].flags &
			 vk::MemoryHeapFlagBits::eDeviceLocal) ==
			vk::MemoryHeapFlagBits::eDeviceLocal )
		{
			return Index;
		}
	}
	return std::nullopt;
}

std::string FormatVersion(std::uint32_t Version)
{
	return std::to_string(VK_VERSION_MAJOR(Version)) + "." +
		   std::to_string(VK_VERSION_MINOR(Version)) + "." +
		   std::to_string(VK_VERSION_PATCH(Version));
}

template<VendorID Vendor>
bool VendorDetails(const vk::PhysicalDevice& PhysicalDevice)
{
	return true;
}

template<>
bool VendorDetails<VendorID::Nvidia>(const vk::PhysicalDevice& PhysicalDevice)
{
	const auto DevicePropertyChain = PhysicalDevice.getProperties2<
		vk::PhysicalDeviceProperties2,
		vk::PhysicalDeviceShaderSMBuiltinsPropertiesNV>();

	const auto SMBuiltinsProperties =
		DevicePropertyChain
			.get<vk::PhysicalDeviceShaderSMBuiltinsPropertiesNV>();

	std::printf(
		"\tStreaming Multiprocessors: %u\n"
		"\tWarpsPerSM: %u\n",
		SMBuiltinsProperties.shaderSMCount,
		SMBuiltinsProperties.shaderWarpsPerSM);
	return true;
}

bool FetchDevice(const vk::PhysicalDevice& PhysicalDevice)
{
	const auto DevicePropertyChain = PhysicalDevice.getProperties2<
		vk::PhysicalDeviceProperties2, vk::PhysicalDeviceDriverProperties>();

	const auto& DeviceProperties =
		DevicePropertyChain.get<vk::PhysicalDeviceProperties2>();
	const auto& DeviceDriverProperties =
		DevicePropertyChain.get<vk::PhysicalDeviceDriverProperties>();

	const auto MemoryPropertyChain = PhysicalDevice.getMemoryProperties2<
		vk::PhysicalDeviceMemoryProperties2,
		vk::PhysicalDeviceMemoryBudgetPropertiesEXT>();
	const auto& MemoryProperties =
		MemoryPropertyChain.get<vk::PhysicalDeviceMemoryProperties2>();

	const auto& MemoryBudgetProperties =
		MemoryPropertyChain.get<vk::PhysicalDeviceMemoryBudgetPropertiesEXT>();

	/// Get device-local heap

	const std::uint32_t VRAMHeapIndex =
		FindVRAMHeapIndex(MemoryProperties.memoryProperties).value_or(0);

	// The heap budget is how much the current process is allowed to allocate
	// from the heap. We compare it to the total amount of memory available
	// to determine how much "usable" free memory there is left
	const vk::DeviceSize MemTotal =
		MemoryProperties.memoryProperties.memoryHeaps[VRAMHeapIndex].size;
	const vk::DeviceSize MemUsed =
		MemTotal - MemoryBudgetProperties.heapBudget[VRAMHeapIndex];

	std::printf(
		"%s : %s\n"
		"\tVendor: %4x : (%s)\n"
		"\tAPI: %s\n"
		"\tDriver: %s : %s\n"
		"\tVRAM: %s / %s : %%%f\n",
		DeviceProperties.properties.deviceName.data(),
		vk::to_string(DeviceProperties.properties.deviceType).c_str(),
		DeviceProperties.properties.vendorID,
		VendorName(static_cast<VendorID>(DeviceProperties.properties.vendorID)),
		FormatVersion(DeviceProperties.properties.apiVersion).c_str(),
		DeviceDriverProperties.driverName.data(),
		DeviceDriverProperties.driverInfo.data(),
		FormatByteCount(MemUsed).c_str(), FormatByteCount(MemTotal).c_str(),
		MemUsed / static_cast<std::float_t>(MemTotal));

	switch( static_cast<VendorID>(DeviceProperties.properties.vendorID) )
	{
	case VendorID::AMD:
	{
		VendorDetails<VendorID::AMD>(PhysicalDevice);
		break;
	}
	case VendorID::Nvidia:
	{
		VendorDetails<VendorID::Nvidia>(PhysicalDevice);
		break;
	}
	case VendorID::Intel:
	{
		VendorDetails<VendorID::Intel>(PhysicalDevice);
		break;
	}
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