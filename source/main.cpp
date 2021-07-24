#include <cstddef>
#include <cstdint>
#include <cstdlib>

#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan.hpp>

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
				"\tMEM: %14zu / %14zu : %%%f\n",
				DeviceProperties.deviceName.data(),
				vk::to_string(DeviceProperties.deviceType).c_str(), MemUsed,
				MemTotal, MemUsed / static_cast<float>(MemTotal));
		}
	}

	return EXIT_SUCCESS;
}