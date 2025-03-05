#include <VulkanUtil.hpp>

namespace Vulkan::Util
{

std::string_view VendorName(VendorID Vendor)
{
	using namespace std::string_view_literals;

	switch( Vendor )
	{
		// "Unknown" is the first vendor
	default:
#define VENDOR(_, VendorName)                                                  \
	case VendorID::VendorName:                                                 \
		return #VendorName##sv;

#include "Vendors.inc"
#undef VENDOR
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