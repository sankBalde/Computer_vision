#include <kdtree.h>
#include <obj.h>

#include <Eigen/Eigenvalues>

std::pair<std::vector<Eigen::Vector3f>,std::vector<Eigen::Vector3f>> smooth(
    const std::vector<Eigen::Vector3f>& points,
    const std::vector<Eigen::Vector3f>& normals,
    float r);

int main(int argc, char** argv)
{
    if(argc != 2) {
        std::cout << "Error: expecting 1 argument (<filename>.obj)" << std::endl;
        return 0;
    }
    const auto filename = std::string{argv[1]};

    constexpr auto r = 0.5;

    std::vector<Eigen::Vector3f> points, normals;
    if(not tnp::load_obj(filename, points, normals)) {
        std::cout << "Error: Failed to load point cloud" << std::endl;
        return 0;
    }

    const auto [points_smooth, normals_smooth] = smooth(points, normals, r);

    tnp::save_obj("step3_smoothed.obj", points_smooth, normals_smooth, {});

    return 0;
}

std::pair<std::vector<Eigen::Vector3f>,std::vector<Eigen::Vector3f>> smooth(
    const std::vector<Eigen::Vector3f>& points, 
    const std::vector<Eigen::Vector3f>& normals, 
    float r)
{
    auto points_smooth = points;
    auto normals_smooth = normals;
    
    // step 3

    // Construire le kd-tree pour accélérer les recherches de voisinage
    tnp::KdTree kdtree;
    kdtree.build(points);

    for (size_t i = 0; i < points.size(); ++i) {
        const Eigen::Vector3f& query_point = points[i];
        std::vector<Eigen::Vector3f> neighbors;

        // Collecter les voisins dans la sphère de rayon r
        kdtree.for_each_neighbors(points, query_point, r, [&](int j) {
            neighbors.push_back(points[j]);
        });

        // Calculer le barycentre p̄
        Eigen::Vector3f centroid = Eigen::Vector3f::Zero();
        for (const auto& neighbor : neighbors) {
            centroid += neighbor;
        }
        centroid /= static_cast<float>(neighbors.size());

        // Construire la matrice de covariance C
        Eigen::Matrix3f covariance = Eigen::Matrix3f::Zero();
        for (const auto& neighbor : neighbors) {
            Eigen::Vector3f diff = neighbor - centroid;
            covariance += diff * diff.transpose();
        }
        covariance /= static_cast<float>(neighbors.size());

        // Diagonaliser la matrice de covariance
        Eigen::SelfAdjointEigenSolver<Eigen::Matrix3f> solver(covariance);
        const Eigen::Vector3f& normal = solver.eigenvectors().col(0); // Vecteur propre associé à la plus petite valeur propre

        // Mettre à jour le point en le projetant sur le plan ACP
        Eigen::Vector3f projection = query_point - normal.dot(query_point - centroid) * normal;
        points_smooth[i] = projection;
        normals_smooth[i] = normal; // Remplacer le vecteur normal par le nouveau
    }

    return std::make_pair(std::move(points_smooth), std::move(normals_smooth));
}
