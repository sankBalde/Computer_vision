#include <kdtree.h>
#include <obj.h>

// compute for each point the distance to its nearest neighbor in the range r
std::vector<float> compute_nearest_dist(
    const std::vector<Eigen::Vector3f>& points, // point cloud
    float r);                                   // radius search

// compute mu (mean) and sigma (standard deviation) of values
std::pair<float,float> compute_stat(const std::vector<float>& values);

// filter out point if its nearest distance is greater than a threshold
std::pair<std::vector<Eigen::Vector3f>, std::vector<Eigen::Vector3f>> filter(
    const std::vector<Eigen::Vector3f>& points,  // points
    const std::vector<Eigen::Vector3f>& normals, // normal vectors
    const std::vector<float>& nearest_dist,      // nearest distances for each point
    float mu, float sigma, float alpha);         // for thresholding

// ****************************************************************************
int main(int argc, char** argv)
{
    if(argc != 2) {
        std::cout << "Error: expecting 1 argument (<filename>.obj)" << std::endl;
        return 0;
    }
    const auto filename = std::string{argv[1]};

    // parameters
    constexpr auto r = 0.1;
    constexpr auto alpha = 10.0;

    // point cloud
    std::vector<Eigen::Vector3f> points, normals;
    if(not tnp::load_obj(filename, points, normals)){
        std::cout << "Error: Failed to load point cloud" << std::endl;
        return 0;
    }

    const auto nearest_dist = compute_nearest_dist(points, r);

    const auto [mu, sigma] = compute_stat(nearest_dist);
    std::cout << "mu        = " << mu << std::endl;
    std::cout << "sigma     = " << sigma  << std::endl;

    const auto [points_filtered, normals_filtered] = filter(points, normals, nearest_dist, mu, sigma, alpha);
    tnp::save_obj("step2_filtered.obj", points_filtered, normals_filtered, {});

    std::cout << "point count init = " << points.size()  << std::endl;
    std::cout << "point count filt = " << points_filtered.size()  << std::endl;
    std::cout << "diff             = " << points.size() - points_filtered.size()
        << " (" << double(points.size()-points_filtered.size())/points.size()*100 << "%)" << std::endl;

    return 0;
}
// ****************************************************************************

std::vector<float> compute_nearest_dist(
    const std::vector<Eigen::Vector3f>& points,
    float r)
{
    //std::vector<float> results(points.size(), 0);

    // step 2.1

    std::vector<float> results(points.size(), std::numeric_limits<float>::infinity());
    tnp::KdTree kdtree;
    kdtree.build(points); // Construire le kd-tree

    for (size_t i = 0; i < points.size(); ++i) {
        const Eigen::Vector3f& query_point = points[i];
        float r_squared = r * r;

        kdtree.for_each_neighbors(points, query_point, r, [&](int j) {
            if (i != j) { // Exclure le point lui-même
                float dist_squared = (points[j] - query_point).squaredNorm();
                if (dist_squared < results[i]) {
                    results[i] = dist_squared;
                }
            }
        });

        // Convertir les distances au carré en distances normales
        results[i] = std::sqrt(results[i]);
    }

    return results;
}

std::pair<float,float> compute_stat(const std::vector<float>& values)
{
    auto mu = 0.0;
    auto sigma = 0.0;

    // step 2.2

    float sum = 0.0f;
    size_t count = 0;

    // Calcul de la somme (ignorer les valeurs infinies)
    for (float val : values) {
        if (val != std::numeric_limits<float>::infinity()) {
            sum += val;
            ++count;
        }
    }

    mu = sum / count;

    // Calcul de l'écart type
    float variance = 0.0f;
    for (float val : values) {
        if (val != std::numeric_limits<float>::infinity()) {
            variance += (val - mu) * (val - mu);
        }
    }
    sigma = std::sqrt(variance / count);


    return std::make_pair(mu, sigma);
}

std::pair<std::vector<Eigen::Vector3f>, std::vector<Eigen::Vector3f>> filter(
    const std::vector<Eigen::Vector3f>& points,
    const std::vector<Eigen::Vector3f>& normals,
    const std::vector<float>& average_dist,
    float mu, float sigma, float alpha)
{
    std::vector<Eigen::Vector3f> filtered_points;
    std::vector<Eigen::Vector3f> filtered_normals;

    // step 2.3
    float threshold = mu + alpha * sigma;

    for (size_t i = 0; i < points.size(); ++i) {
        if (average_dist[i] <= threshold) {
            filtered_points.push_back(points[i]);
            filtered_normals.push_back(normals[i]);
        }
    }
    return std::pair(std::move(filtered_points), std::move(filtered_normals));
}
