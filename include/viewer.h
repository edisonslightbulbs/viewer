#ifndef RENDERER_H
#define RENDERER_H

#include <memory>
#include <mutex>
#include <vector>

#include "kinect.h"

namespace viewer {
void draw(Kinect& kinect);
}
#endif /* RENDERER_H */
