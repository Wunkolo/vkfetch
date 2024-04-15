#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>

#include "VulkanConfig.hpp"

namespace Vulkan::Util
{
enum class VendorID : std::uint32_t
{
	AMD       = 0x1002,
	ImgTec    = 0x1010,
	Apple     = 0x106B,
	Nvidia    = 0x10DE,
	ARM       = 0x13B5,
	Microsoft = 0x1414,
	Google    = 0x1AE0,
	Qualcomm  = 0x5143,
	Intel     = 0x8086,
	Unknown   = 0xFFFF,
	VIV       = 0x10001,
	VSI       = 0x10002,
	Kazan     = 0x10003,
	Codeplay  = 0x10004,
	Mesa      = 0x10005,
	Pocl      = 0x10006,
	MobileEye = 0x10007,
};

const char* VendorName(VendorID Vendor);

std::optional<std::uint32_t>
	FindVRAMHeapIndex(const vk::PhysicalDeviceMemoryProperties& MemoryProperties
	);

} // namespace Vulkan::Util