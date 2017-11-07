#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "HelloTriangleApplication.h"

const int WIDTH = 800;
const int HEIGHT = 600;

int main() {
	HelloTriangleApplication app(WIDTH, HEIGHT);

	try {
		app.run();
	}
	catch (const std::runtime_error& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}