#include "renderer.h"

#include <pangolin/gl/glvbo.h>
#include <pangolin/pangolin.h>

#define TINYPLY_IMPLEMENTATION
#include "tinyply.h"

void renderer::render(std::mutex& m, Kinect kinect, int pointNum,
    const shared_ptr<std::vector<float>>& sptr_points)
{
    /** create window and bind its context to the main thread */
    pangolin::CreateWindowAndBind("VIGITIA", 2560, 1080);

    /** initialize glew */
    glewInit();

    /**  enable mouse handler with depth testing */
    glEnable(GL_DEPTH_TEST);

    /** unset the current context from the main thread */
    // pangolin::GetBoundWindow()->RemoveCurrent();

    /** create vertex and colour buffer objects and register them with CUDA */
    pangolin::GlBuffer vA(
        pangolin::GlArrayBuffer, pointNum, GL_FLOAT, 3, GL_STATIC_DRAW);
    pangolin::GlBuffer cA(
        pangolin::GlArrayBuffer, pointNum, GL_UNSIGNED_BYTE, 3, GL_STATIC_DRAW);
    std::vector<uint8_t> colours(pointNum * 3, 255);

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

    /** main render thread */
    while (true) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /** update point cloud every 10 ms */
        usleep(10000);
        kinect.getPcl(sptr_points);
        vA.Upload((void*)sptr_points->data(), pointNum * 3 * sizeof(float));
        cA.Upload((void*)colours.data(), pointNum * 3 * sizeof(uint8_t));

        viewPort.Activate(camera);
        glClearColor(0.0, 0.0, 0.3, 1.0);

        pangolin::glDrawAxis(4000.f);
        pangolin::RenderVboCbo(vA, cA);
        pangolin::FinishFrame(); // <- swap frames and process events

        /** ungracious global exit */
        if (pangolin::ShouldQuit()) {
            kinect.release();
            kinect.close();
            std::exit(0);
        }
    }
}
