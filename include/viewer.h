#ifndef RENDERER_H
#define RENDERER_H

#include <memory>
#include <mutex>
#include <vector>

#include "kinect.h"

namespace viewer {
void draw(std::mutex& m, Kinect kinect, int numPoints,
    std::shared_ptr<std::vector<float>>& sptr_points,
    std::shared_ptr<std::pair<Point, Point>>& sptr_threshold);
};

#endif /* RENDERER_H */
