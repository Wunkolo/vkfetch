#include <VulkanUtil.hpp>

namespace Vulkan::Util
{

const char* VendorName(VendorID Vendor)
{
	switch( Vendor )
	{
	case VendorID::AMD:
	{
		return "AMD";
	}
	case VendorID::Apple:
	{
		return "Apple";
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
	default:
	case VendorID::Unknown:
	{
		return "Unknown";
	}
	}
}

std::optional<std::uint32_t>
	FindVRAMHeapIndex(const vk::PhysicalDeviceMemoryProperties& MemoryProperties
	)
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

} // namespace Vulkan::Util