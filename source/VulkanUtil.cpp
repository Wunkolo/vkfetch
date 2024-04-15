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
	case VendorID::Microsoft:
	{
		return "Microsoft";
	}
	case VendorID::Google:
	{
		return "Google";
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
	case VendorID::VIV:
	{
		return "VIV";
	}
	case VendorID::VSI:
	{
		return "VSI";
	}
	case VendorID::Kazan:
	{
		return "Kazan";
	}
	case VendorID::Codeplay:
	{
		return "Codeplay";
	}
	case VendorID::Mesa:
	{
		return "Mesa";
	}
	case VendorID::Pocl:
	{
		return "Pocl";
	}
	case VendorID::MobileEye:
	{
		return "MobileEye";
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