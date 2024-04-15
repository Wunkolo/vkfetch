#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>

#include "VulkanConfig.hpp"

namespace Vulkan::Util
{
enum class VendorID : std::uint32_t
{
#define VENDOR(VendorID, VendorName) VendorName = VendorID,
#include "Vendors.inc"
#undef VENDOR
};

const char* VendorName(VendorID Vendor);

std::optional<std::uint32_t>
	FindVRAMHeapIndex(const vk::PhysicalDeviceMemoryProperties& MemoryProperties
	);

} // namespace Vulkan::Util