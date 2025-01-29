#include <kdtree.h>
#include <obj.h>
#include <unordered_set>

std::pair<std::vector<Eigen::Vector3f>, std::vector<Eigen::Vector3f>> voxel_subsample(
    const std::vector<Eigen::Vector3f>& points,  // nuage de points
    const std::vector<Eigen::Vector3f>& normals, // normales des points
    float voxel_size)                            // taille des voxels
{
    // Trouver la boîte englobante
    Eigen::Vector3f min_bound = points[0];
    Eigen::Vector3f max_bound = points[0];

    for (const auto& p : points) {
        min_bound = min_bound.cwiseMin(p);
        max_bound = max_bound.cwiseMax(p);
    }

    // Calculer le nombre de voxels dans chaque dimension
    Eigen::Vector3f range = max_bound - min_bound;
    Eigen::Vector3i grid_dim = (range / voxel_size).array().ceil().cast<int>(); // Dimensions de la grille (en nombre de voxels)

    // Créer un ensemble pour suivre les voxels déjà "visités"
    std::unordered_set<size_t> visited_voxels;
    std::vector<Eigen::Vector3f> subsampled_points;
    std::vector<Eigen::Vector3f> subsampled_normals;

    // Fonction pour obtenir un identifiant unique pour chaque voxel
    auto voxel_key = [&min_bound, &voxel_size](const Eigen::Vector3f& p) {
        Eigen::Vector3i voxel_idx = ((p - min_bound) / voxel_size).array().floor().cast<int>();
        return static_cast<size_t>(voxel_idx.x()) * 73856093 ^ voxel_idx.y() * 19349663 ^ voxel_idx.z() * 83492791;
    };

    // Sous-échantillonnage
    for (size_t i = 0; i < points.size(); ++i) {
        size_t voxel_id = voxel_key(points[i]);

        if (visited_voxels.find(voxel_id) == visited_voxels.end()) {
            // Si ce voxel n'a pas encore été visité, conserver ce point
            visited_voxels.insert(voxel_id);
            subsampled_points.push_back(points[i]);
            subsampled_normals.push_back(normals[i]);
        }
    }

    return {subsampled_points, subsampled_normals};
}

int main(int argc, char** argv)
{
    if(argc != 2) {
        std::cout << "Error: expecting 1 argument (<filename>.obj)" << std::endl;
        return 0;
    }
    const auto filename = std::string{argv[1]};

    std::vector<Eigen::Vector3f> points, normals;
    if(not tnp::load_obj(filename, points, normals)) {
        std::cout << "Error: Failed to load point cloud" << std::endl;
        return 0;
    }

    //std::vector<Eigen::Vector3f> points_subsampled;
    //std::vector<Eigen::Vector3f> normals_subsampled;

    // step 4
    // ...
    constexpr float voxel_size = 0.5f;  // Taille des voxels (ajustez cette valeur pour contrôler le sous-échantillonnage)

    // Appliquer le sous-échantillonnage basé sur des voxels
    auto [points_subsampled, normals_subsampled] = voxel_subsample(points, normals, voxel_size);

    //

    tnp::save_obj("step4_subsampled.obj", points_subsampled, normals_subsampled, {});

    return 0;
}