#pragma once

#include <functional>
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include <exception>

namespace vkr
{
	class Window
	{
	public:
		Window();
		Window::Window(const int width, const int height);

		~Window();

		// No copying allowed!
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		// Moving is fine I guess
		Window(Window&& otherWindow);
		Window& operator=(Window&& otherWindow);

		vk::SurfaceKHR createWindowSurface(vk::Instance instance);
		void destroy();
		void setResizeCallback(void * object, GLFWwindowsizefun func) const;
		bool shouldClose() const;

		inline int getWidth() const { return width; }
		inline int getHeight() const { return height; }
		inline vk::Extent2D getExtent() const
		{
			vk::Extent2D extent{ (uint32_t)width, (uint32_t)height };
			return extent;
		};

		static void pollWindowEvents();
		static void initializeGLFW();
		static void terminateGLFW();

	private:
		GLFWwindow * pWindow;
		int width;
		int height;
	};
}

