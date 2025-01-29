#include <pivot.h>
#include <geometry.h>

namespace tnp {

template<typename T>
T safe_acos(T value) 
{
    if(value <= T{-1.0}) 
    {
        return T{M_PI};
    } 
    else if(value >= T{1.0}) 
    {
        return T{0.0};
    } 
    else 
    {
        return std::acos(value);
    }
}

std::optional<std::pair<int,Eigen::Vector3f>> pivot(
    int i,
    int j,
    int k,
    const Eigen::Vector3f& ball_center,
    std::vector<Eigen::Vector3f>& points, 
    const tnp::KdTree& kdtree,
    float ball_radius)
{
    // Calculer le centre de l'arête m et le vecteur unitaire u
    Eigen::Vector3f m = 0.5f * (points[i] + points[j]);
    Eigen::Vector3f u = (points[j] - points[i]).normalized();

    // Définir les vecteurs unitaires v et w pour le plan perpendiculaire à u
    Eigen::Vector3f v = (ball_center - m).normalized();
    Eigen::Vector3f w = u.cross(v).normalized();

    // Rechercher les points dans un rayon de 2r autour de m
    std::vector<int> neighbors;
    kdtree.for_each_neighbors(points, m, 2 * ball_radius, [&](int l) {
        if (l != i && l != j && l != k) { // Filtrer les points i, j, k
            neighbors.push_back(l);
        }
    });

    // Variables pour stocker le point minimisant θ
    float min_theta = 2 * M_PI;
    int best_l = -1;
    Eigen::Vector3f best_center;

    // Parcourir les voisins pour trouver le meilleur candidat
    for (int l : neighbors) {
        // Calculer le centre de la boule pour les points i, j, l
        auto center_opt = compute_center(points[i], points[j], points[l], ball_radius);
        if (!center_opt) {
            continue; // Ignorer si aucun centre valide n'existe
        }
        Eigen::Vector3f s = *center_opt;

        // Calculer les coordonnées (x, y) dans le plan (v, w)
        Eigen::Vector3f sm = s - m;
        float x = sm.dot(v);
        float y = sm.dot(w);

        // Calculer l'angle θ
        float theta = safe_acos(x / std::sqrt(x * x + y * y));
        if (y < 0) {
            theta = 2 * M_PI - theta; // Ajuster l'angle pour y < 0
        }

        // Mettre à jour le point si l'angle est minimal
        if (theta < min_theta) {
            min_theta = theta;
            best_l = l;
            best_center = s;
        }
    }

    // Retourner le point minimisant θ et le nouveau centre de la boule
    if (best_l != -1) {
        return std::make_pair(best_l, best_center);
    } else {
        return std::nullopt;
    }

}

} // namespace tnp