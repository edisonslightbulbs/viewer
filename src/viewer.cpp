#include <pangolin/gl/glvbo.h>
#include <pangolin/pangolin.h>
#define TINYPLY_IMPLEMENTATION

#include "tinyply.h"

#include <chrono>
#include <mutex>
#include <shared_mutex>

#include "intact.h"
#include "viewer.h"

int mode = 0;

void view()
{
    mode++;
    if (mode == 4) {
        mode = 0;
    }
}

void viewer::draw(std::shared_ptr<Intact>& sptr_intact)
{
    /** create window and bind its context to the main thread */
    pangolin::CreateWindowAndBind("VIGITIA", 2560, 1080);

    /** initialize glew */
    glewInit();

    /**  enable mouse handler with depth testing */
    glEnable(GL_DEPTH_TEST);

    /** create vertex and colour buffer objects and register them with CUDA */
    pangolin::GlBuffer vA(pangolin::GlArrayBuffer, sptr_intact->m_numPoints,
        GL_SHORT, 3, GL_STATIC_DRAW);
    pangolin::GlBuffer cA(pangolin::GlArrayBuffer, sptr_intact->m_numPoints,
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

    // register key press to trigger different view perspective
    pangolin::RegisterKeyPressCallback(pangolin::PANGO_CTRL + 'c', view);

    /** pool resources, and render */
    uint32_t pclsize = sptr_intact->m_pclsize * sizeof(int16_t);
    uint32_t imgsize = sptr_intact->m_pclsize * sizeof(uint8_t);
    int16_t* pcl;
    uint8_t* img;

    while (!sptr_intact->isStop()) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (mode == 0) {
            // render sensor point cloud
            pcl = *sptr_intact->getSensorPcl();
            img = *sptr_intact->getSensorImg_GL();
        } else if (mode == 1) {
            // render intact's point cloud
            pcl = *sptr_intact->getIntactPcl();
            img = *sptr_intact->getIntactImg_GL();
        } else if (mode == 2) {
            // render chromakey background
            pcl = *sptr_intact->getChromaBkgdPcl();
            img = *sptr_intact->getChromaBkgdImg_GL();
        } else if (mode == 3) {
            // render colored clusters
            // pcl = *sptr_intact->getChromaBkgdPcl();
            // img = *sptr_intact->getChromaBkgdImg_GL();
        }
        vA.Upload((void*)pcl, pclsize);
        cA.Upload((void*)img, imgsize);

        viewPort.Activate(camera);
        glClearColor(0.0, 0.0, 0.3, 1.0);
        pangolin::glDrawAxis(4000.f);
        pangolin::RenderVboCbo(vA, cA);
        pangolin::FinishFrame();

        /** gracious exit from rendering app */
        if (pangolin::ShouldQuit()) {
            sptr_intact->raiseStopFlag();
        }
    }
}
