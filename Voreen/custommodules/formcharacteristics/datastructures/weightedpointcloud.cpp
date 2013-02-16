#include "weightedpointcloud.h"

WeightedPoint::WeightedPoint(float x, float y, float z, float weight) {
    coords_[0] = x;
    coords_[1] = y;
    coords_[2] = z;
    coords_[3] = weight;
}

WeightedPointCloud::WeightedPointCloud() {
    
}

void WeightedPointCloud::add(const WeightedPoint& point) {
    points_.push_back(point);
}

const WeightedPoint& WeightedPointCloud::get(size_t index) const {
    return points_[index];
}
