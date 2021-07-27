#include <cstddef>
#include <cstdint>
#include <cstdlib>

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

std::string FormatByteCount(std::size_t ByteCount)
{
	static const char* SizeUnits[] = {"Bytes", "KiB", "MiB", "GiB", "TiB",
									  "PiB",   "EiB", "ZiB", "YiB"};

	std::size_t Index;
	double      ByteSize = ByteCount;
	for( Index = 0; Index < std::extent_v<decltype(SizeUnits)>; Index++ )
	{
		if( ByteSize < 1024 )
			break;
		ByteSize /= 1024;
	}
	return std::to_string(ByteSize) + " " + SizeUnits[Index];
}

int main()
{

	vk::InstanceCreateInfo InstanceInfo = {};

	vk::ApplicationInfo ApplicationInfo = {};

	ApplicationInfo.apiVersion         = VK_API_VERSION_1_2;
	ApplicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	ApplicationInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);

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
			const auto DeviceProperties = CurDevice.getProperties();

			const auto MemoryPropertyChain = CurDevice.getMemoryProperties2<
				vk::PhysicalDeviceMemoryProperties2,
				vk::PhysicalDeviceMemoryBudgetPropertiesEXT>();
			const auto& MemoryProperties =
				MemoryPropertyChain.get<vk::PhysicalDeviceMemoryProperties2>();

			const auto& MemoryBudgetProperties =
				MemoryPropertyChain
					.get<vk::PhysicalDeviceMemoryBudgetPropertiesEXT>();

			const vk::DeviceSize MemTotal =
				MemoryProperties.memoryProperties.memoryHeaps[0].size;
			const vk::DeviceSize MemUsed =
				MemTotal - MemoryBudgetProperties.heapBudget[0];

			std::printf(
				"%s : %s\n"
				"\tMEM: %s / %s : %%%f\n",
				DeviceProperties.deviceName.data(),
				vk::to_string(DeviceProperties.deviceType).c_str(),
				FormatByteCount(MemUsed).c_str(),
				FormatByteCount(MemTotal).c_str(),
				MemUsed / static_cast<float>(MemTotal));
		}
	}

	return EXIT_SUCCESS;
}