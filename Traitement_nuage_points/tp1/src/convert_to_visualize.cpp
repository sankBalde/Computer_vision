#include <obj.h>
#include <iostream>
#include <Eigen/Geometry> // for cross

std::tuple<
    std::vector<Eigen::Vector3f>,
    std::vector<Eigen::Vector3f>,
    std::vector<Eigen::Vector3i>>
        convert(
            const std::vector<Eigen::Vector3f>& points, 
            const std::vector<Eigen::Vector3f>& normals, 
            float r);

int main(int argc, char** argv)
{
    if(argc != 3) {
        std::cout << "Error: expecting at least 2 arguments (<input>.obj and <output>.obj)" << std::endl;
        return 0;
    }
    const auto filename_in = std::string{argv[1]};
    const auto filename_out = std::string{argv[2]};

    // parameters
    constexpr auto r = 0.1;

    // point cloud
    std::vector<Eigen::Vector3f> points, normals;
    if(not tnp::load_obj(filename_in, points, normals)){
        std::cout << "Error: Failed to load point cloud" << std::endl;
        return 0;
    }

    // convert to splats
    const auto [points_converted, normals_converted, faces_converted] = convert(points, normals, r);

    tnp::save_obj(filename_out, points_converted, normals_converted, {}, faces_converted);

    return 0;
}

void orthonormal_basis(Eigen::Vector3f& u, Eigen::Vector3f& v, const Eigen::Vector3f& w)
{
    // i0: dimension where w extends the least
    int i0 = (std::abs(w[0] ) < std::abs(w[1])) ? 0  : 1;
        i0 = (std::abs(w[i0]) < std::abs(w[2])) ? i0 : 2;
    int i1 = (i0 + 1) % 3;
    int i2 = (i0 + 2) % 3;

    // setting the i0-th coordinate of u to zero ensure that u is not null
    u[i0] = 0;
    u[i1] =  w[i2];
    u[i2] = -w[i1];

    // normalize u
    u /= std::sqrt(u[i1] * u[i1] + u[i2] * u[i2]);

    v = w.cross(u);
}

std::tuple<std::vector<Eigen::Vector3f>,std::vector<Eigen::Vector3f>,std::vector<Eigen::Vector3i>> convert(
    const std::vector<Eigen::Vector3f>& points, 
    const std::vector<Eigen::Vector3f>& normals, 
    float r)
{
    std::vector<Eigen::Vector3f> points_converted(4*points.size());
    std::vector<Eigen::Vector3f> normals_converted(4*points.size());
    std::vector<Eigen::Vector3i> faces_converted(2*points.size());

    for(auto i = 0u; i < points.size(); ++i)
    {
        Eigen::Vector3f u, v;
        orthonormal_basis(u, v, normals[i]);

        points_converted[4*i+0] = points[i] - r * u - r * v;
        points_converted[4*i+1] = points[i] + r * u - r * v;
        points_converted[4*i+2] = points[i] + r * u + r * v;
        points_converted[4*i+3] = points[i] - r * u + r * v;

        normals_converted[4*i+0] = normals[i];
        normals_converted[4*i+1] = normals[i];
        normals_converted[4*i+2] = normals[i];
        normals_converted[4*i+3] = normals[i];

        faces_converted[2*i+0] = Eigen::Vector3i(4*i+0, 4*i+1, 4*i+2);
        faces_converted[2*i+1] = Eigen::Vector3i(4*i+0, 4*i+2, 4*i+3);
    }

    return std::make_tuple(points_converted, normals_converted, faces_converted);
}