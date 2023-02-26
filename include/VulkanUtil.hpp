#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>

#include "VulkanConfig.hpp"

namespace Vulkan::Util
{
enum class VendorID : std::uint32_t
{
	AMD      = 0x1002,
	ImgTec   = 0x1010,
	Apple    = 0x106B,
	Nvidia   = 0x10DE,
	ARM      = 0x13B5,
	Qualcomm = 0x5143,
	Intel    = 0x8086,
	Unknown  = 0xFFFF
};

const char* VendorName(VendorID Vendor);

std::optional<std::uint32_t>
	FindVRAMHeapIndex(const vk::PhysicalDeviceMemoryProperties& MemoryProperties
	);

} // namespace Vulkan::Util