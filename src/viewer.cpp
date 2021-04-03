#include <pangolin/gl/glvbo.h>
#include <pangolin/pangolin.h>
#define TINYPLY_IMPLEMENTATION
#include "tinyply.h"

#include "viewer.h"

void viewer::draw(std::shared_ptr<Kinect>& sptr_kinect)
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
    pangolin::GlBuffer vA(pangolin::GlArrayBuffer, sptr_kinect->getNumPoints(),
        GL_FLOAT, 3,
        GL_STATIC_DRAW); // todo: (1/7) resource race handled correctly?
    pangolin::GlBuffer cA(pangolin::GlArrayBuffer, sptr_kinect->getNumPoints(),
        GL_UNSIGNED_BYTE, 3,
        GL_STATIC_DRAW); // todo: (2/7) resource race handled correctly?
    std::vector<uint8_t> colours(sptr_kinect->getNumPoints() * 3,
        255); // todo: (3/7) resource race handled correctly?

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

        sptr_kinect
            ->getCapture(); // todo: (4/7) resource race handled correctly?
        sptr_kinect
            ->getPclImage(); // todo: (5/7) resource race handled correctly?
        vA.Upload((void*)sptr_kinect->getContextPcl()->data(),
            sptr_kinect->getNumPoints() * 3
                * sizeof(
                    float)); // todo: (6/7) resource race handled correctly?
        cA.Upload((void*)colours.data(),
            sptr_kinect->getNumPoints() * 3
                * sizeof(
                    uint8_t)); // todo: (7/7) resource race handled correctly?
        viewPort.Activate(camera);
        glClearColor(0.0, 0.0, 0.3, 1.0);

        pangolin::glDrawAxis(4000.f);
        pangolin::RenderVboCbo(vA, cA);
        pangolin::FinishFrame(); // <- swap frames and process events

        /** ungracious global exit */
        if (pangolin::ShouldQuit()) {
            sptr_kinect->release();
            sptr_kinect->close();
            std::exit(0);
        }

        /** update point cloud every 15 ms */
        usleep(15000);
    }
}
