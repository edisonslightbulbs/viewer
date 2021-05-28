#ifndef RENDERER_H
#define RENDERER_H

#include <memory>
#include <vector>

#include "i3d.h"
#include "kinect.h"

namespace viewer {
void draw(std::shared_ptr<i3d>& sptr_i3d);
}
#endif /* RENDERER_H */
