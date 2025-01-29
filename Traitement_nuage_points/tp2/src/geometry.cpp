#include <geometry.h>

namespace tnp {

Eigen::Vector3f triangle_normal(
    const Eigen::Vector3f& p0,
    const Eigen::Vector3f& p1,
    const Eigen::Vector3f& p2)
{
    // Calculer les vecteurs p1-p0 et p2-p0
    Eigen::Vector3f u = p1 - p0;
    Eigen::Vector3f v = p2 - p0;

    // Calculer le produit vectoriel u × v
    Eigen::Vector3f normal = u.cross(v);

    // Normaliser le vecteur pour obtenir une normale unitaire
    return normal.normalized();

}

Eigen::Vector3f triangle_circumcenter(
    const Eigen::Vector3f& p0,
    const Eigen::Vector3f& p1,
    const Eigen::Vector3f& p2)
{
    // Calcul des vecteurs p10 = p1 - p0 et p20 = p2 - p0
    Eigen::Vector3f p10 = p1 - p0;
    Eigen::Vector3f p20 = p2 - p0;

    // Calcul du vecteur normal v = p10 × p20
    Eigen::Vector3f v = p10.cross(p20);

    // Vérifier que les points ne sont pas colinéaires
    float v_norm_sq = v.squaredNorm();

    // Calcul des longueurs au carré
    float p10_sq = p10.squaredNorm();
    float p20_sq = p20.squaredNorm();

    // Calcul du centre du cercle circonscrit
    Eigen::Vector3f circumcenter = p0 + ((p10_sq * p20 - p20_sq * p10).cross(v)) / (2.0f * v_norm_sq);

    return circumcenter;

}

std::optional<Eigen::Vector3f> compute_center(
    const Eigen::Vector3f& p0,
    const Eigen::Vector3f& p1,
    const Eigen::Vector3f& p2,
    float ball_radius)
{
    // Calculer le centre du cercle circonscrit
    Eigen::Vector3f circumcenter = triangle_circumcenter(p0, p1, p2);

    // Calculer le vecteur normal du triangle
    Eigen::Vector3f normal = triangle_normal(p0, p1, p2);

    // Calculer la distance du centre circonscrit au point p0
    float dist_to_circumcenter = (p0 - circumcenter).norm();

    // Vérifier si la boule de rayon r peut exister
    if (dist_to_circumcenter > ball_radius) {
        // Si la distance est plus grande que le rayon, aucune solution n'est possible
        return std::nullopt;
    }

    // Calculer la distance du centre circonscrit au centre de la boule
    float h = std::sqrt(ball_radius * ball_radius - dist_to_circumcenter * dist_to_circumcenter);

    // Deux solutions possibles : au-dessus et en dessous
    Eigen::Vector3f center_upper = circumcenter + h * normal;
    //Eigen::Vector3f center_lower = circumcenter - h * normal;

    // Retourner le centre supérieur (par convention)
    return center_upper;

}

} // namespace tnp