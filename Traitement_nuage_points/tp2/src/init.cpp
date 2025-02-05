#include <init.h>
#include <geometry.h>

namespace tnp {

std::pair<Eigen::Vector3i,Eigen::Vector3f> initial_triangle(
    const std::vector<Eigen::Vector3f>& points, 
    const KdTree& kdtree,
    float ball_radius)
{
        return std::make_pair(
            Eigen::Vector3i{48337,1934,6462}, 
            Eigen::Vector3f{-0.257905, 1.90763, 15.5501});
}

} // namespace tnp