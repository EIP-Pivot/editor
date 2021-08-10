#include "Window.hxx"
#include <iostream>
#include <pivot/graphics/VulkanApplication.hxx>

Logger *logger = nullptr;

class Application : public VulkanApplication
{
public:
    void draw(float fElapsedTime) final {}
};

int main()
{
    logger = new Logger(std::cout);
    logger->start();

    Window win("Wulkan", 500, 500);
    Application app;

    app.load3DModels({"../assets/viking_room.obj"});
    app.loadTexturess({"../assets/viking_room.png"});
    app.init(win);
    while (!win.shouldClose()) {
        win.pollEvent();
        app.draw(0);
    }

    return 0;
}