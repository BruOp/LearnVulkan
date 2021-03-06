#include "vkrContext.h"

// Only need to define this once just FYI.
#define VMA_IMPLEMENTATION
#include "vma/vk_mem_alloc.h"


const int WIDTH = 800;
const int HEIGHT = 600;

int main() {
	vkrContext app(WIDTH, HEIGHT);

	try {
		app.run();
	}
	catch (const std::runtime_error& e) {
		std::cerr << e.what() << std::endl;
        std::getchar();
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}