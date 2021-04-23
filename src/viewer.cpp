#include <pangolin/gl/glvbo.h>
#include <pangolin/pangolin.h>
#define TINYPLY_IMPLEMENTATION

#include "tinyply.h"

#include <chrono>
#include <mutex>
#include <shared_mutex>
#include <thread>

#include "intact.h"
#include "viewer.h"

extern std::shared_ptr<bool> RUN_SYSTEM;

void viewer::draw(std::shared_ptr<Kinect>& sptr_kinect)
{
    /** create window and bind its context to the main thread */
    pangolin::CreateWindowAndBind("VIGITIA", 2560, 1080);

    /** initialize glew */
    glewInit();

    /**  enable mouse handler with depth testing */
    glEnable(GL_DEPTH_TEST);

    /** create vertex and colour buffer objects and register them with CUDA */
    pangolin::GlBuffer vA(pangolin::GlArrayBuffer, sptr_kinect->getNumPoints(),
        GL_FLOAT, 3, GL_STATIC_DRAW);
    pangolin::GlBuffer cA(pangolin::GlArrayBuffer, sptr_kinect->getNumPoints(),
        GL_UNSIGNED_BYTE, 3, GL_STATIC_DRAW);

    /** define camera render object for scene browsing */
    pangolin::OpenGlRenderState camera(
        pangolin::ProjectionMatrix(2560, 1080, 800, 800, 1280, 540, 0.1, 10000),
        ModelViewLookAt(-0, 2, -2, 0, 0, 0, pangolin::AxisY));
    const int UI_WIDTH = 180;

    /** add named OpenGL viewport to window and provide 3D handler */
    pangolin::View& viewPort
        = pangolin::Display("cam")
              .SetBounds(0.0, 1.0, pangolin::Attach::Pix(UI_WIDTH), 1.0,
                  -640.0f / 480.0f)
              .SetHandler(new pangolin::Handler3D(camera));

    /** render point cloud */
    while (RUN_SYSTEM) {
        sptr_kinect->record(RGB_TO_DEPTH);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        vA.Upload((void*)sptr_kinect->getContext()->data(),
            sptr_kinect->getNumPoints() * 3 * sizeof(float));
        cA.Upload((void*)sptr_kinect->getColor()->data(),
            sptr_kinect->getNumPoints() * 3 * sizeof(uint8_t));
        viewPort.Activate(camera);
        glClearColor(0.0, 0.0, 0.3, 1.0);
        pangolin::glDrawAxis(4000.f);
        pangolin::RenderVboCbo(vA, cA);
        pangolin::FinishFrame();
        sptr_kinect->release();

        /** gracious exit from rendering app */
        if (pangolin::ShouldQuit()) {
            *RUN_SYSTEM = false;
            sptr_kinect->close();
            std::exit(0);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
void viewer::draw(
    std::shared_ptr<Intact>& sptr_intact, std::shared_ptr<Kinect>& sptr_kinect)
{
    /** create window and bind its context to the main thread */
    pangolin::CreateWindowAndBind("VIGITIA", 2560, 1080);

    /** initialize glew */
    glewInit();

    /**  enable mouse handler with depth testing */
    glEnable(GL_DEPTH_TEST);

    /** create vertex and colour buffer objects and register them with CUDA */
    pangolin::GlBuffer vA(pangolin::GlArrayBuffer, sptr_kinect->getNumPoints(),
        GL_FLOAT, 3, GL_STATIC_DRAW);
    pangolin::GlBuffer cA(pangolin::GlArrayBuffer, sptr_kinect->getNumPoints(),
        GL_UNSIGNED_BYTE, 3, GL_STATIC_DRAW);

    /** define camera render object for scene browsing */
    pangolin::OpenGlRenderState camera(
        pangolin::ProjectionMatrix(2560, 1080, 800, 800, 1280, 540, 0.1, 10000),
        ModelViewLookAt(-0, 2, -2, 0, 0, 0, pangolin::AxisY));
    const int UI_WIDTH = 180;

    /** add named OpenGL viewport to window and provide 3D handler */
    pangolin::View& viewPort
        = pangolin::Display("cam")
              .SetBounds(0.0, 1.0, pangolin::Attach::Pix(UI_WIDTH), 1.0,
                  -640.0f / 480.0f)
              .SetHandler(new pangolin::Handler3D(camera));

    while (RUN_SYSTEM) {
        /** sense */
        sptr_kinect->record(RGB_TO_DEPTH);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        vA.Upload((void*)sptr_intact->getRegion()->data(),
            sptr_kinect->getNumPoints() * 3 * sizeof(float));
        cA.Upload((void*)sptr_intact->getRegionColor()->data(),
            sptr_kinect->getNumPoints() * 3 * sizeof(uint8_t));
        viewPort.Activate(camera);
        glClearColor(0.0, 0.0, 0.3, 1.0);
        pangolin::glDrawAxis(4000.f);
        pangolin::RenderVboCbo(vA, cA);
        pangolin::FinishFrame();
        sptr_kinect->release();

        /** gracious exit from rendering app */
        if (pangolin::ShouldQuit()) {
            *RUN_SYSTEM = false;
            sptr_kinect->close();
            std::exit(0);
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
