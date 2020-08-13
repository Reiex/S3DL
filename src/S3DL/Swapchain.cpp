#include <S3DL/S3DL.hpp>

namespace s3dl
{
    Swapchain::Swapchain(const RenderWindow& window)
    {
        VkExtent2D _extent = window.getBestSwapExtent();
        VkPresentModeKHR presentMode = window.getBestSwapPresentMode();
        VkSurfaceFormatKHR surfaceFormat = window.getBestSwapSurfaceFormat();

        // Compute swap chain settings
        PhysicalDevice::SwapChainSupportDetails swapChainSupport = Device::Active->getPhysicalDevice().swapSupport;

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
            imageCount = swapChainSupport.capabilities.maxImageCount;
        
        // Create the swap chain

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = window.getVulkanSurface();
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = _extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        VkResult result = vkCreateSwapchainKHR(Device::Active->getVulkanDevice(), &createInfo, nullptr, &_swapChain);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to create swap chain. VkResult: " + std::to_string(result));

        // Extract the swap chain images

        vkGetSwapchainImagesKHR(Device::Active->getVulkanDevice(), _swapChain, &imageCount, nullptr);
        _images.resize(imageCount);
        vkGetSwapchainImagesKHR(Device::Active->getVulkanDevice(), _swapChain, &imageCount, _images.data());

        // Compute swap chain image views
        
        _imageViews.resize(_images.size());
        for (size_t i = 0; i < _images.size(); i++)
        {
            VkImageViewCreateInfo createInfo{};

            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = _images[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = surfaceFormat.format;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            VkResult result = vkCreateImageView(Device::Active->getVulkanDevice(), &createInfo, nullptr, &_imageViews[i]);
            if (result != VK_SUCCESS)
                throw std::runtime_error("Failed to create swap chain image view " + std::to_string(i) + ".");
        }

        #ifndef NDEBUG
        std::clog << "<S3DL Debug> VkSwapChainKHR successfully created with " + std::to_string(imageCount) << " swap images." << std::endl;
        #endif

        create();
    }

    Swapchain::~Swapchain()
    {
        for (int i(0); i < _renderSemaphore.size(); i++)
            vkDestroySemaphore(Device::Active->getVulkanDevice(), _renderSemaphore[i], nullptr);
        
        for (int i(0); i < _acquireFence.size(); i++)
            vkDestroyFence(Device::Active->getVulkanDevice(), _acquireFence[i], nullptr);

        #ifndef NDEBUG
        std::clog << "<S3DL Debug> Display semaphores and fences successfully destroyed." << std::endl;
        #endif

        for (int i(0); i < _imageViews.size(); i++)
            vkDestroyImageView(Device::Active->getVulkanDevice(), _imageViews[i], nullptr);

        vkDestroySwapchainKHR(Device::Active->getVulkanDevice(), _swapChain, nullptr);

        #ifndef NDEBUG
        std::clog << "<S3DL Debug> VkSwapChainKHR successfully destroyed." << std::endl;
        #endif
    }

    void Swapchain::create()
    {
        unsigned int imageCount = _images.size();

        _acquireFence.resize(imageCount);
        _renderSemaphore.resize(imageCount);

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        for (int i(0); i < imageCount; i++)
        {
            vkCreateFence(Device::Active->getVulkanDevice(), &fenceInfo, nullptr, &_acquireFence[i]);
            vkCreateSemaphore(Device::Active->getVulkanDevice(), &semaphoreInfo, nullptr, &_renderSemaphore[i]);
        }

        #ifndef NDEBUG
        std::clog << "<S3DL Debug> Display semaphores and fences successfully created." << std::endl;
        #endif
    }
}
