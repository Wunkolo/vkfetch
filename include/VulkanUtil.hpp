#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string_view>

#include "VulkanConfig.hpp"

namespace Vulkan::Util
{
enum class VendorID : std::uint32_t
{
#define VENDOR(VendorID, VendorName) VendorName = VendorID,
#include "Vendors.inc"
#undef VENDOR
};

std::string_view VendorName(VendorID Vendor);

std::optional<std::uint32_t>
	FindVRAMHeapIndex(const vk::PhysicalDeviceMemoryProperties& MemoryProperties
	);

} // namespace Vulkan::Util