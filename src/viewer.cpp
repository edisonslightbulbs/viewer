#include <pangolin/gl/glvbo.h>
#include <pangolin/pangolin.h>
#define TINYPLY_IMPLEMENTATION
#include "tinyply.h"

#include "point.h"
#include "viewer.h"

void context(std::mutex& m, Kinect& kinect,
    std::shared_ptr<std::vector<float>>& sptr_points,
    std::shared_ptr<std::pair<Point, Point>>& sptr_threshold)
{

    Point min;
    Point max;
    min = sptr_threshold->first;
    max = sptr_threshold->second;

    int width = k4a_image_get_width_pixels(kinect.m_pclImage);
    int height = k4a_image_get_height_pixels(kinect.m_pclImage);

    for (int i = 0; i < width * height; i++) {
        if ((*sptr_points)[3 * i + 0] > max.m_x
            || (*sptr_points)[3 * i + 0] < min.m_x
            || (*sptr_points)[3 * i + 1] > max.m_y
            || (*sptr_points)[3 * i + 1] < min.m_y
            || (*sptr_points)[3 * i + 2] > max.m_z
            || (*sptr_points)[3 * i + 2] < min.m_z) {
            (*sptr_points)[3 * i + 0] = 0.f;
            (*sptr_points)[3 * i + 1] = 0.f;
            (*sptr_points)[3 * i + 2] = 0.f;
            continue;
        }
    }
}

void viewer::draw(std::mutex& m, Kinect kinect, int numPoints,
    shared_ptr<std::vector<float>>& sptr_points,
    std::shared_ptr<std::pair<Point, Point>>& sptr_threshold)
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
        pangolin::GlArrayBuffer, numPoints, GL_FLOAT, 3, GL_STATIC_DRAW);
    pangolin::GlBuffer cA(pangolin::GlArrayBuffer, numPoints, GL_UNSIGNED_BYTE,
        3, GL_STATIC_DRAW);
    std::vector<uint8_t> colours(numPoints * 3, 255);

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

        kinect.capture();
        if (m.try_lock()) {
            kinect.pclImage(sptr_points);
            context(m, kinect, sptr_points, sptr_threshold);
            vA.Upload(
                (void*)sptr_points->data(), numPoints * 3 * sizeof(float));
            cA.Upload((void*)colours.data(), numPoints * 3 * sizeof(uint8_t));
            m.unlock();
        }
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

        /** update point cloud every 15 ms */
        usleep(15000);
    }
}
