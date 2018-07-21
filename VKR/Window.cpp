#include "Window.h"


namespace vkr
{
	Window::Window() : pWindow(nullptr), width(0), height(0) {}

	Window::Window(const int _width, const int _height) :
		pWindow(nullptr),
		width(_width),
		height(_height)
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		pWindow = glfwCreateWindow(width, height, "Vulkan", nullptr, nullptr);
	}

	Window::Window(Window && otherWindow)
	{
		pWindow = otherWindow.pWindow;
		otherWindow.pWindow = nullptr;
	}

	Window & Window::operator=(Window && otherWindow)
	{
		if (this != &otherWindow) {
			pWindow = otherWindow.pWindow;
			otherWindow.pWindow = nullptr;
		}
		return *this;
	}

	Window::~Window()
	{
		destroy();
	}

	vk::SurfaceKHR Window::createWindowSurface(vk::Instance instance)
	{
		VkSurfaceKHR vanillaVkSurface{};
		if (glfwCreateWindowSurface(static_cast<VkInstance>(instance), pWindow, nullptr, &vanillaVkSurface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create a window surface!");
		}
		auto surface = vk::SurfaceKHR{ vanillaVkSurface };
		return surface;
	}

	void Window::destroy()
	{
		if (pWindow != nullptr) {
			glfwDestroyWindow(pWindow);
		}
	}

	void Window::setResizeCallback(void * object, GLFWwindowsizefun func) const
	{
		glfwSetWindowUserPointer(pWindow, object);
		glfwSetWindowSizeCallback(pWindow, func);
	}

	bool Window::shouldClose() const
	{
		return glfwWindowShouldClose(pWindow);
	}

	void Window::pollWindowEvents()
	{
		glfwPollEvents();
	}

	void Window::initializeGLFW()
	{
		glfwInit();
	}
	void Window::terminateGLFW()
	{
		glfwTerminate();
	}
}
