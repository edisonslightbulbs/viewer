#ifndef RENDERER_H
#define RENDERER_H

#include <context.h>
#include <memory>
#include <mutex>
#include <vector>

#include "kinect.h"

namespace viewer {
void draw(std::shared_ptr<Kinect>& kinect);
void draw(Context* ptr_context, std::shared_ptr<Kinect>& sptr_kinect);
}
#endif /* RENDERER_H */
