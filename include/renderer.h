#ifndef RENDERER_H
#define RENDERER_H

#include <memory>
#include <mutex>
#include <vector>

#include "kinect.h"

namespace renderer {
void render(std::mutex& m, Kinect kinect, int pointNum,
    const std::shared_ptr<std::vector<float>>& sptr_points);
};

#endif /* RENDERER_H */
