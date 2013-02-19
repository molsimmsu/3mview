#ifndef VRN_WEIGHTEDPOINTCLOUD_H
#define VRN_WEIGHTEDPOINTCLOUD_H

#include "tgt/vector.h"

/*
 * Weighted point data structure.
 * TODO Access and modification public functions
 */
class WeightedPoint {
public:
    WeightedPoint(float x, float y, float z, float weight);
    virtual ~WeightedPoint() {}
    
    const float operator [](size_t index) const {
        return coords_[index];
    }
    
private:
    std::vector<float> coords_; // 0-2: coords; 3: weight
};

/*
 * Weighted point cloud data structure.
 */
class WeightedPointCloud {
public:
    WeightedPointCloud();
    virtual ~WeightedPointCloud() {}
    
    // Adds a point to a point cloud
    void add(const WeightedPoint& point);
    
    // Returns a point with a given index
    const WeightedPoint& get(size_t index) const;
    
    // Returns number of points in a cloud
    size_t size() const { return points_.size(); }
    
private:
    std::vector<WeightedPoint> points_;
};

#endif // VRN_WEIGHTEDPOINTCLOUD_H
